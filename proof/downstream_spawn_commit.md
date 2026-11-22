# Downstream spawn-commit: the `0x300 -> 0x500/0x700` phase predicate

Instance under test: live debug port **9045** (do **not** kill). Companion
document: `proof/downstream_spawn_wedge.md` (the DAT1=3 CD/decompression
event-wait that starves everything downstream). This file isolates the
*fighter-spawn commit* predicate and proves where the `0x300` phase word is
supposed to go and why it never does.

---

## 1. The spawn-commit predicate (exact gate)

`fighter_spawn_setup` — guest `0x801487ac`, clean `game/menu.c:3768`.

The per-player spawn commit is the loop at `game/menu.c:3787-3832`:

```c
for (s1 = 0; s1 < 2; s1++) {
  if (*(uint32_t *)(s0 + 0x10) == 1 &&
      ((bp[0xb4/2 + s1] & 0xf00) == 0x700)) {   /* menu.c:3788-3789 */
      /* SPAWN COMMIT: write record fields, alloc task(s), DAT1 advance */
  }
  s0 += 0x30;
}
```

- `bp` = battle base `0x1f800000`; `bp[0xb4/2 + s1]` is the per-player phase
  word (`+0xb4` for player 0, `+0xb6` for player 1).
- `s0` walks the two **temp battle-frame records** at
  `ctx + 0x28 + s1*0x30` (`s6 = param_4 + 0x28`, `param_4` = the
  `battle_frame` guest-frame ctx stored at `0x801f10bc`). These are the same
  records `battle_frame` case-6 iterates as `T + i*0x30`.
- `record + 0x10` is a **sub-state** (observed values 0 / 1 / 2), not a simple
  boolean. `== 1` means "this side's intro/spawn sequence is armed".

So the commit needs **both**: `record[i]+0x10 == 1` **and**
`(phase[i] & 0xf00) == 0x700`.

`fighter_spawn_setup` is invoked every frame from `battle_frame`
(`game/menu.c:8195` case-3 and `:8213` case-6) with `param_5 = 0`
(`fp = 0` → the `fp != 0` commit arm is *not* taken; the `else` arm at
`menu.c:3800-3825` runs: it sets `DAT_1f800001 = 2` (from 3) or `5` (from 6),
frees the two task nodes and re-allocs them via `task_alloc_wrap_c`).

## 2. How phase `0x700` is produced — and why `0x300` cannot reach it

`battle_config_eval` (`game/menu.c:4008`, guest `0x80153544`) is the *only*
per-frame phase evaluator. Its class dispatch (`cls = phase & 0xf00`):

| cls   | action                                                        |
|-------|---------------------------------------------------------------|
| 0x000 | `L_state0` → mode table; arcade (`9b==0`) → `L_A0` → `bcfg_step_a`; `r!=0` → **`L_set700` writes `0x700`** (`menu.c:4149-4151`) |
| 0x100 | writes `0x200` (`menu.c:4030`)                                |
| 0x200 | `L_ret` — terminal                                            |
| **0x300** | `L_state3` → `L_B0` → `stage_fight_announce` / `fight_timer_announce` — **display-only, writes NO phase** (`menu.c:4063-4120`) |
| 0x400 | `L_ret68`                                                      |
| 0x500 | `L_ret68` (fighter_spawn_setup promotes `0x500→0x600` first)  |
| 0x600 | `continue_countdown`; `r>0` + foe `select_phase_gate==0` → **`L_set700`** (`menu.c:4132-4140`) |
| 0x700 | `bcfg_step_c` (reset-to-idle `0x100`)                          |
| 0x800/0x900 | `0x800→0x900`, low-byte increments to `0x96` then wraps to `0` |

**`cls == 0x300` is terminal for phase advance.** The announce helpers
(`stage_fight_announce` `menu.c:4344`, `fight_timer_announce` `menu.c:4286`)
only gate on a char-record flag and call `hud_text_blk*` to *draw* the
"STAGE / FIGHT" banner — they never store `+0xb4/+0xb6`. Verified: `bce_cls`
reads `0x300` and `g_bce_calls` climbs into the tens of thousands while the
phase words never move.

Therefore `0x300 -> 0x700` **cannot** be produced inside `battle_config_eval`.
The phase words `0x1f8000b4`/`0x1f8000b6` are registered as **battle-script-VM
globals** (`game/bind.c:1789-1790` `g__DAT_1f8000b4`/`g__DAT_1f8000b6`, and
the `vars[]` table at `bind.c:1838-1839`). The "ROUND / FIGHT!" intro
sequence — run by the script/event VM via the `battle_frame` task callbacks
(`task_alloc_wrap_c/e`, `script_stream_tick` `0x80137824`,
`battle_intro_fsm` `0x80137098`) — is what writes the phase word to `0x700`
(and drives `record+0x10`) when the intro timeline completes. That timeline
is gated on the **fighter model/animation assets being resident**, which are
delivered by the CD decompress stream.

### Writers of `+0xb4`/`+0xb6` (complete audit)

Clean + generated store audit (`write_half … +180/+182`):

- `battle_state_init` `0x80151328` → seeds `0x300` (`menu.c` via
  `fighter_spawn_tail`/round reset).
- `select_confirm_handler` `0x80148528` → `(ph & 0xf000) | 0x300`
  (`menu.c:3584`) — the *confirm → announce* transition that lands us at
  `0x300`.
- `battle_config_eval` `0x80153544` → `0x200/0x700/0x800/0x900`/increment
  (see table).
- `fighter_spawn_setup` `0x801487ac` → `0x500→0x600` promotion only
  (`menu.c:3781-3782`).
- `bcfg_step_c` `0x8015279c` / `bcfg_step_d` `0x80152878` → reset `0x100`.
- `side_phase_advance` `0x8015297c` → `0x900` (name-entry, versus).
- `vs_confirm_b` `0x80152bdc` → `0x100/0x600/0xa00` (versus).
- **`post_battle_dispatch` `0x80154930` → `0x500`/`0x1400`**
  (`menu.c:7121` `MW16(s1+0xb6,0x500)`, `:7147` `MW16(s1+0xb4,0x500)`).

The `post_battle_dispatch` `0x300→0x500` stores the task asked about are
**real but not on the arcade path**: `post_battle_dispatch` only runs when
`DAT_1f80009b == 4` (versus round-end). Live counters prove it never fires in
this wedge (`g_pbd_calls = 0`), and `9b = 0` (arcade). In arcade the `0x500`
writer is simply absent — consistent with `0x500` being a *versus*
round-transition class.

## 3. Live wedge state (port 9045, `mem_words`)

| cell | value | meaning |
|------|-------|---------|
| `0x1f8000b4` / `0x1f8000b6` | `0x0300` / `0x0300` | both phases stuck at announce class |
| `0x1f800001` (DAT1) | `0x03` | `battle_frame` case-3 loop |
| `0x1f800092` | `0x0100` | combined phase — **stale** (last `confirm_bookkeeping` ran when only P0 gate passed; not recomputed per-frame) |
| `0x1f80008c` | `0xffff` | `battle_bringup_b` resource SM finished |
| `0x801f10bc` | `0x801FFE58` | `battle_frame` ctx guest ptr |
| `0x801f10c0` / `0x801f10c4` | `0x801D660C` / `0x801D71FC` | task-pool record bases (allocated, non-zero) |
| rec0 `+0x10` @`0x801FFE90` | `0x00000000` | player-0 sub-state = 0 (not armed) |
| rec1 `+0x10` @`0x801FFEC0` | `0x00000001` | player-1 sub-state = 1 (armed) |

So **neither** side can commit: player-1 has `record+0x10==1` but its phase
`+0xb6` is `0x300` (needs `0x700`); player-0 has neither. The asymmetry
(player-1 sub-state ahead) shows the per-record sub-state machine *is*
progressing — the shared blocker is the **phase word**, which both sides
need at `0x700`.

`frame` keeps advancing (~98k frames observed); `cdrom_state` shows
`reading=0`, `pending.cmd=0x09 Pause`, `last_sector lba=8801 frame=1571`,
`ring_dropped=1`, `ring_partial=898`, `bfrd_set=2047` vs `bfrd_clear=13`.

## 4. Why the phase never leaves `0x300` — the stuck cell

`0x300` is the **post-confirm announce** class. Exiting it requires the
intro/"FIGHT" script-VM sequence to run to completion and store `0x700` into
the phase globals. That sequence is driven by the same CD-delivered,
LZSS-decompressed asset stream documented in `downstream_spawn_wedge.md`:

- `decomp_trace` froze at `seq 213` mid-`heapA_w_menu` LZSS block — the
  decompressor starved of compressed input because CD sectors were
  lost/partial-delivered (`ring_dropped=1`, `ring_partial=898`,
  `bfrd_set=2047`/`bfrd_clear=13`).
- With the fighter/intro payload incomplete, the script/event VM never
  reaches the opcode that writes `DAT_1f8000b4/b6 = 0x700` and never sets
  player-0's `record+0x10 = 1`.
- Result: `battle_config_eval` announces forever (`bce_cls=0x300`),
  `fighter_spawn_setup`'s commit predicate stays false, DAT1 parks at 3.

This is **not** normal announce timing — the announce has been static for
~98k frames while the decompressor trace is frozen and the CD ring shows
permanent loss. It is a true wedge.

## 5. Where the faithful fix belongs

- **Not** a phase-word poke (forbidden, and would only mask the stall).
- **Not** `game/*.c` game logic — the clean ports of `battle_config_eval`,
  `fighter_spawn_setup`, `battle_frame`, and the announce helpers are
  disasm-verified correct; `0x300` being terminal in `bce` *is* the real
  guest behavior.
- **`psxrecomp/runtime/src/cdrom.c`** — the CD sector ring / BFRD
  flow-control that drops or re-orders delivery under the sustained asset
  stream (`bfrd_set=2047` vs `bfrd_clear=13`, `ring_partial=898`,
  `ring_dropped=1`). Restoring correct sector accounting lets the
  decompressor finish → the "load done" event fires → the deferred
  coroutine resumes → the intro script writes `phase = 0x700` and arms
  `record+0x10` → `fighter_spawn_setup` commits the spawn.

This is the **same root cause** as `downstream_spawn_wedge.md` /
`cd_finalize_gate.md` / `cd_dma_dispatch_channel.md`: the runtime CD model,
not the game code.

## 6. Contradictions / corrections to earlier notes

- `proof/arcade_spawn_phase_starvation.md` argued "CD is healthy; residual is
  game-logic phase starvation." The live `cdrom_state` on 9045
  (`ring_dropped=1`, `ring_partial=898`, `bfrd 2047/13`, frozen
  `decomp_trace`) plus the frozen decompressor **contradict** "CD healthy" —
  the sector-loss signature is present and the consumer is starved. The
  starvation is real; its *proximate* trigger is the CD ring/BFRD accounting.
- The `post_battle_dispatch` `0x300→0x500` stores (`menu.c:7121/7147`) are
  versus-round-end writers (`9b==4`); `g_pbd_calls=0` proves they are not the
  arcade spawn path. Do not chase them for this wedge.
- The earlier "CPU-phase-seed" theory is confirmed a misdiagnosis
  (`downstream_spawn_wedge.md` DECISIVE section): `+0xb6` seeding does not
  gate reaching DAT1=3.

## 7. Evidence index

- Spawn gate: `game/menu.c:3788-3789` (`record+0x10==1 && phase==0x700`).
- Phase `0x700` writer: `game/menu.c:4149-4151` (`L_set700`), reachable only
  from cls `0x000`/`0x600`.
- `0x300` terminal announce: `game/menu.c:4063-4120` (`L_state3`/`L_B0`).
- `0x500` versus writers: `game/menu.c:7121,7147` (`post_battle_dispatch`).
- Script-VM phase globals: `game/bind.c:1789-1790,1838-1839`.
- `battle_frame` dispatch + case-3/6 spawn calls: `game/menu.c:8112-8216`.
- Live reads: port 9045 `mem_words`/`cdrom_state`/`frame`/`decomp_trace`
  (values in §3).
