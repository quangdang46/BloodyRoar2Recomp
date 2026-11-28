# battle_hud_draw SIGSEGV @0x801f5224 + quad_fx_spawn SIGSEGV @0x801e24d0 — guest-address-as-host-pointer fixes

Agent: devin-p, worktree br2-wt-devin-p, branch agent/devin-p.

## Bug 1 (reported): intermittent SIGSEGV in battle_hud_draw

- Crash reports `~/Library/Logs/DiagnosticReports/BloodyRoar2_Recompiled-*.ips`
  (e.g. 2026-11-28-191014.ips): `EXC_BAD_ACCESS KERN_INVALID_ADDRESS at
  0x00000000801f5224`, frame 0 = `battle_hud_draw`, callers
  `battle_flow_state_machine <- guest_main <- clean_try_dispatch`.
- Reproduced only during ATTRACT demo battles, frames ~2500-4100,
  intermittent (one run survived 38k+ frames).

### Root cause

`game/battle.c` `battle_hud_draw` tail block (guest ~0x80175288+, the
per-player combo/hit-counter HUD tail after `task_pools_tick(0)`):

```c
o14 = (uint32_t *)0x801f5220;          /* RAW GUEST ADDR as host pointer */
t2 = (intptr_t)o14;                    /* guest addr (needed by guest_at) */
t3 = (intptr_t)o14;
...
slot = (int *)((intptr_t)o14 + (t8 ^ 1) * 4);
*slot = (int)r0;                       /* host store -> 0x801f5220/0x801f5224 */
*(uint16_t *)((intptr_t)o14 + t0 + 0x10) = ...   /* same class */
t6 = (intptr_t)o14 + t0;               /* consumed by guest_at() below */
```

The generated reference (`generated/SLES_017.22_full_32.c`,
`br2eu_func_801737EC`, block_80175288 @ line ~32290) keeps guest address
`0x801F5220` in a register (`lui 0x801F; addiu +0x5220`) and performs every
access through `cpu->read_word/write_word/half` — i.e. the guest memory
path. The clean port mixed the two domains on ONE variable:

- direct derefs `*slot`, `*(uint16_t*)o14`, `(intptr_t)o14 + t0` need a
  HOST arena pointer;
- `guest_at(t2/t3/t6 + off)` calls need the GUEST address `0x801f5220`.

First reached write faults at host `0x801f5224` = `0x801f5220 + (t8^1)*4`
for t8=0 — exactly the crash address. Intermittency: the block is gated by
`t7 = *(u8*)(t1+0xdd) + *o2 != 0` (per-player combo counter nonzero) and
the surrounding `0x20 < _DAT_801f4f20` warmup, so it only fires in some
demo-battle states.

### Fix (faithful)

- `o14 = (uint32_t *)guest_at(0x801f5220)` — host pointer for the direct
  field R/W (stores to guest cells 0x801f5220/0x801f5224 and the
  +0x10..+0x30 u16 fields).
- `t2 = t3 = 0x801f5220` (guest address constants) for all `guest_at()`
  accesses; `t6 = 0x801f5220 + t0` likewise.

## Bug 2 (found during verification): SIGSEGV in quad_fx_spawn

After fix 1, `tools/arcade_fight_mash.py` runs crashed the instance at
~frame 4400/15000 (new .ips 2026-11-28-193409.ips):
`EXC_BAD_ACCESS at 0x00000000801e24d0`, frame 0 = `quad_fx_spawn`,
callers `cond_triple_spawn <- battle_reset_tail <- wrap_8017e77c <-
call_guest4 <- ev_stream_dispatch <- action_commit <- fighter_frame`.
Same bug class: raw guest address `0x801e24d0` (= `obj+0x124` vec3)
dereferenced as a host pointer.

### Root cause

`quad_fx_spawn` @0x80141814 declared `uint32_t *pos` and dereferenced it
directly (`*pos`, `pos[1]`, `pos[2]`). The generated function treats `a2`
as a guest address: it stores the raw value into `pay[3]` and
`quad_fx_update` @0x80141610 later re-derefs `pay[3]` through the guest
memory path (`guest_at(*(uint32_t*)guest_at(rec+0xc))` triple-deref,
battle.c:10127-10129). Callers were inconsistent:

- `cond_triple_spawn` (menu.c:10514): passes `MB32(rec+0x20)+0x124` — a
  GUEST address (the crasher).
- `fighter_reset_fx` (resolve.c:2788): passed
  `(uint32_t*)guest_at(g)` — a HOST pointer (also wrong under the real
  contract; fixed to pass the guest address).
- `debug_strike_fx_a` (battle.c:5999): passed a host stack array; guest
  builds the vec3 at `sp+0x20` and passes that guest address
  (generated_SLES_017.22_full_10.c `br2eu_func_8013F034`,
  `gpr[6] = gpr[29]+32` before `jal 0x80141814`). Fixed by staging the 3
  u32s at `guest_sp()-0x20` (below-sp red zone, same lifetime as the guest
  frame slot; `fl=0x1000` so `quad_fx_update` never re-reads pos —
  bit 0x2000 clear) and passing that guest address.

### Fix (faithful)

- Signature: `quad_fx_spawn(int f,int kind,uint32_t pos,...)` — pos is a
  guest address. Body reads via `pp = (uint32_t*)guest_at(pos)`;
  `pay[3] = (int)pos` stores the raw guest pointer for the task callback.
- `src/clean_table.c` regenerated (`python3 game/gen_clean_table.py`,
  6054 wrappers) — dispatch now passes `cpu->gpr[6]` raw instead of
  `guest_at(cpu->gpr[6])`.
- resolve.c extern decl updated to `uint32_t`.

## Verification

Headless: `./build/BloodyRoar2_Recompiled --disc "disc/...cue"
--debug-port <port> --headless --game game_1pad.toml` with
`PSX_FN_FILTER="0x80010000:0x80200000"`.

- ATTRACT (no input, demo battles cycle):
  - port 19850: 207k+ frames clean (fix-1 binary)
  - port 19851: 170k+ frames clean (fix-1 binary)
  - port 19854: 118k+ frames clean (fix-1+2 binary)
  - Historical crash window was frames ~2500-4100; all runs far exceed it.
- ARCADE via `tools/arcade_fight_mash.py 19855`: FIGHT reached frame 4133,
  16+ "LEFT FIGHT" transitions through frame ~79k and still running at
  last check — previously SIGSEGV'd at ~frame 4400/15000 in the same path.

## Known latent issue (NOT fixed — flagged for follow-up)

`battle_hud_draw` ~line 2595-2626 (`w0 = (uint32_t *)0x0` block, gated by
`0x20 < _DAT_801f4f20 && -1 < *(char*)(fighter+0x99)` plus per-player
`aux+6/aux+8/aux+0xc` counters): if reached, `w0[t8*0x30 - 0x1ff82c19]`
dereferences near-NULL/wild. Generated analysis (block_801741D8..80174360):
the real destination base is guest `0x801F4F90 + (fighter_c^1)*0xC0`
(s4 = gpr[3]+gpr[8], gpr[8]=0) with u32 stores at +8/+12/+16/+20 of
`rd[-2], rd[-1], *rd, rd[1]` (two via swl/swr unaligned pairs) — the port
both uses a NULL base AND scrambles the store order/values. Has NOT fired
in 200k+ attract frames (likely gated by `fighter+0x99`), but is a
provable mistransliteration needing a careful re-port of that ~40-line
block against generated — left untouched rather than guessed.
