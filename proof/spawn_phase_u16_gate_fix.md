# Spawn-phase wedge RESOLVED — `_DAT_1f800082` u32-vs-u16 mistranslation froze the channel-script VM

Status: **root cause found, faithfully fixed, verified live.** The wedge was
NOT the decompressor, NOT the CD path, NOT the task scheduler — it was a
single clean-port type error: `game/battle.c` read the guest sim/pause gate
`lh @0x1F800082` as a **u32**, which always read nonzero (it picked up the
low half of the adjacent pointer `0x801BEF90` stored at `0x1F800084`), so
every channel-script task early-returned before advancing its cursor/tick.

## Q1 — Did the decompressor finish? YES (verified, port 9850)

`decomp_trace` ring (214 entries) + live state:

- File `0x81` loaded into the tag table `0x801C47C0` (`file_load` seq 125);
  `battle_bringup_b` state `0x30` then ran its full decode schedule:
  blocks `0..0xa`, `0x20..0x25`, `0x30..0x35` = **22 blocks, all 22
  dispatch/lzss/blk_ret triples present** (seqs 126–212).
- seq 213 `heapA_w_menu 0x800E1064→0x800E9078` is only the **final cursor
  advance** after block `0x35`'s `blk_ret` (seq 212, dst=`0x800DC800` = the
  second cursor `0x801e3cfc`) — nothing follows because `battle_bringup_b`
  completed: `MW16(battle+0x8c, 0xffff); return 1`.
- Live: `*(u16*)(0x1f80008c) = 0xFFFF` (bringup DONE), cursor
  `0x801c47b8 = 0x800E9078`. The `-1` offset-table entries
  (`0x801c47c0+`, indices ~14–33, 39–48, 52) are **legitimately empty
  blocks** — the driver only requests populated indices; every requested
  block decoded. **Decompressor finished cleanly; not a stall.**

## Q2 — SM/cd_file_load_sm: EXONERATED

`cd_file_load_sm(0x81, table)` returned nonzero (the decode ran); `bc84c`
dormancy is normal post-load. No SM re-arm needed.

## Q3 — The phase-0x700 / record+0x10 writer: the channel-script VM

- Only ONE live task node at the wedge: pool1 sub-list 6 →
  `0x801D0228`, callback `0x80137ACC` = `LAB_80137acc`
  (channel command interpreter, `game/battle.c:16733`), allocated by
  `task_alloc_wrap_e(0,0x801b2764,0x801b2784)` in `battle_frame` case1.
- Script stream `0x801B2784`: 8 timed records `{tick,1,hidx}` (ticks
  0,0xa,0xa,0x14,0x1e,0x23,0x32,0x3e) → handler table `0x801B2764`
  (`options_pages_reset`, `dual_redraw_*`, `quad_redraw_flag`,
  `task_spawn_flag`, `resource_rebuild_*`). These drive the
  ROUND/FIGHT timeline that writes phase `0x300→0x500→0x600→0x700`
  (via `battle_config_eval`/`fighter_spawn_setup`) and sets record `+0x10`.
- **Frozen state (port 9850):** `tramp_calls≈107k` (~1/frame) but
  `tramp_last_cur=0x801B2784`, `tramp_last_op=0` — cursor and tick NEVER
  advanced despite being invoked every frame.

## ROOT CAUSE — `lh` ported as `lw`

`LAB_80137acc` prologue (generated `SLES_017.22_full_07.c:16811`):

```
0x80137AD0: lh  $v0, 0x82($v0)   ; v0 = 0x1F800000  →  lhu/lh @0x1F800082
bnez $v0 → return                ; sim/pause gate
```

Clean port (`game/battle.c:170`, before fix):

```c
extern uint32_t *g__DAT_1f800082;
#define _DAT_1f800082   (*g__DAT_1f800082)   /* u32 @0x1f800082 — WRONG */
```

`0x1F800082` is **unaligned**; the faithful access is a halfword. The u32
read at the wedge returned `0xEF900000` — bytes `[00 00][90 EF]` = the u16
flag (0) plus the low half of the pointer `0x801BEF90` stored at
`0x1F800084`. Nonzero forever → `if (_DAT_1f800082 != 0) return;` fired on
every call → cursor/tick frozen → phase stuck at `0x300` →
`fighter_spawn_setup` spawn predicate (`rec+0x10==1 && phase&0xf00==0x700`)
never satisfied → `battle_frame` case3 spins forever.

**Corroboration:** `flow_cfg_init` (`game/save.c:1566-1567`) writes
`_DAT_1f800082 = 0` as u16 and treats `0x1f800084` as a separate u32
pointer — the two fields are adjacent, not one word. `menu.c:432` and
`save.c:236` already bound the symbol as `uint16_t`; `battle.c` was the
only file that got it wrong. All 15 `_DAT_1f800082` uses in `battle.c`
are `==0`/`!=0` gate tests (`script_stream_tick`, `slot_triple_tick`,
`quad_fx_update`, `confetti_burst_update`, `firework_task_update`,
`LAB_8013e558`, `LAB_80140510`, `LAB_80137680`, `LAB_80137acc`,
`LAB_80137cd0`, `LAB_8013eb04`, …) — every channel-script task was frozen.

## THE FIX (committed)

`game/battle.c:169-177`:

```c
extern uint32_t *g__DAT_801f0654, *g__DAT_801f0656;
extern uint8_t  *g__DAT_1f800082;
#define _DAT_1f800082   (*(uint16_t *)g__DAT_1f800082)
```

## Runtime verification

Fresh headless run (`--debug-port 9900`, `arcade_fight_mash.py`):

- Before fix (port 9850): `tramp_last_cur=0x801B2784`, `tramp_last_op=0`
  forever; phase `0x300`; `DAT1=3`.
- After fix: `freeze_check` shows `tramp_last_cur=0x801B278A`,
  `tramp_last_op=0xA` — **the script VM is advancing through its timeline**
  (cursor moved off script start, op counter incrementing). The
  `_DAT_1f800082` gate now reads the faithful halfword `0`.

## NEW BLOCKER (next bug, deterministic, NOT caused by the fix)

With the script VM running, the intro timeline now executes far enough to
hit a **deterministic crash** at ~frame 1558–1598 (reproduced identically
twice, ports 9900 and 9917):

```
DISPATCH FATAL: misaligned target 0x0000EE93
  cpu->pc: 0x00000000   $ra: 0x0000EE93   $t9: 0x000000B0
  COP0_EPC: 0x8019A648 / 0x8019A600   (inside delegated vsync_wait_n,
                                       generated 0x8019a5d8..0x8019a670)
  RAM[0x801FFE58] = 0x0000EE93
```

- `0x0000EE93` looks like a **truncated `0xBFC0EE93`** (BIOS addr & 0xffff)
  or a 16-bit-truncated kernel pointer; `$t9=0xB0` is the BIOS B0 group
  number. `RAM[0x801FFE58]` sits in the kernel TCB/thread area
  (`0x801FFxxx`).
- `dispatch_tail` ends in BIOS-service offsets (`…0x00002970, 0x000000B0,
  0x00002AA0, 0x000029CC`) — the crash is in the BIOS/kernel-call boundary,
  consistent with the `vsync_wait_n → bios_table_tramp/bios_kobj_tramp`
  family noted in `game/save.c:1533-1546`.
- State at crash: phase still `0x300`, `DAT1=3`, `tramp_calls=11`,
  `tramp_last_cur=0x801B278A`, `tramp_last_op=0xA`, `bce_calls=24`,
  `bce_cls=0x000` — the script had only just started stepping when the
  kernel-side crash hit (it is NOT the script failing; the VM itself is
  healthy now).
- `wtrace` armed on phys `0x1FFE40–0x1FFE70` for the next repro to catch
  the writer of `0x0000EE93`.

## Files changed

- `game/battle.c` — `_DAT_1f800082` u32→u16 (the fix).

## Commands run

- `cd build && cmake --build . -j8` — clean (pre-existing warnings only).
- Fresh instances: `./build/BloodyRoar2_Recompiled --disc …/Game.cue
  --debug-port {9900,9917,9918,9919} --headless` + `arcade_fight_mash.py`.
- Live reads on wedged port 9850: `mem_words`, `decomp_trace`,
  `cdfn_dump`, `freeze_check`, `wtrace_ranges`.
