# Spawn-phase wedge — channel scripts COMPLETE; P2 seed mechanism + correct button encoding verified; residual 0x300 stall is faithful spawn-commit wait

Date: this session. Live instances: **port 9980** (wedge specimen, later
died) and **fresh port 9986** (`game_1pad.toml`, `--headless`,
`arcade_fight_mash.py 9986`) which reproduced the identical wedge in ~11 s
wall time (headless runs ~350 fps; wedge at frame ~4000).

## TL;DR

1. The channel-script VM (`LAB_80137acc`) is **NOT frozen** — both
   announce scripts ran every timed entry to completion and hit the
   `0x4000` terminator; the tasks legitimately stepped off
   (`tramp_calls=177` total, identical on both instances). The earlier
   "frozen tick" was fixed by `1fd2a9b` (u16 gate).
2. `+0xb6 = 0x0000` is **faithful gating**: the CPU opponent's confirm
   arrives as a **slot-1 SIO pad edge whose decoded bit is `0x0800`**.
   No guest CPU/AI synthesizer exists (re-confirmed). The mash script's
   attack buttons never assert that bit.
3. **Correction to `phase8_runtime_probe.md`**: the working injection is
   `press {"slot":1,"buttons":0xfff7}` — **not** `0xf7ff`. The debug
   override→SIO-report path byte-swaps the button word, so `0xfff7`
   produces report `0xf7ff` → decoded pressed bit `0x0800`.
   Verified live twice: `+0xb6` goes `0x0000 → 0x0300` within a few
   frames (bcfg_step_a `0x700` → bcfg_step_c `0x100` → reseed `0x300`).
4. With both phase words at `0x0300`, the residual stall is a **faithful
   spawn-commit wait**: `gwatch` on `+0xb4/+0xb6/+0x92` records **zero**
   writes over 20 s+; `bce` only announces (cls `0x300` terminal);
   `pbd_calls=0`; DAT1=`8` = `battle_frame` inner case-8 polling
   `rec+0x10==1 && phase==0x700`.

## Evidence — script completion (port 9980 & 9986 identical)

Decoded both channel streams with `read_ram` (guest RAM is LE host order;
s16 fields read LE):

- Script1 (allocated `battle_frame` case1, `task_alloc_wrap_e(0,
  0x801b2764,0x801b2784)`): 8 timed entries `{tick,count=1,hidx}` ticks
  `0,0xa,0xa,0x14,0x1e,0x23,0x32,0x3e` → handler table
  `0x801B2764` (`options_pages_reset`, `dual_redraw_*`,
  `resource_rebuild_b`, `resource_rebuild_a`). Param block `0x801D0228`:
  cursor `0x801B2784→0x801B27B4`, tick `0→0x3F`, final entry `0x4000`.
- Script2 (allocated case7, `task_alloc_wrap_e(0,0x801b2820,0x801b2844)`):
  9 entries ticks `0xa,0x14,0x1e,0x37,0x3a,0x3c,0x3e,0x50,0x51` → table
  `0x801B2820` (`desc_slot_fill_x15`…`0x8014cdbc`). Param `0x801D03D0`:
  tick `0→0x52`, cursor ends `0x801B287A` = `0x4000` terminator.
- `resource_rebuild_a` (`menu.c:11322`) tail writes
  `*(u8*)0x1f800001 = 5` — this is what drove the inner FSM
  5→6→7→8 (case7 re-arms script2, case8 = spawn watcher).
- `tramp_calls=177`, `tramp_last_node=0x801D03D0`,
  `tramp_last_cur=0x801B287A` — frozen because **done**, not stuck.

## Evidence — P2 seed (faithful gating, correct encoding)

- `pad_state_refresh` (`menu.c:2873`): `+0x68/+0x6a` = per-frame
  `(new^old)&new` edge u16s of decoded pad1/pad2 words packed at `+0x6c`.
- `bcfg_step_a` requires `edge & 0x800` for that slot
  (`battle_config_eval` cls-`0x000` → `L_A0_body` → `L_set700`).
- Override→report mapping verified: `press slot:1 buttons=0xfff7` →
  `0x801befca` = `00 41 f7 ff` → decoded word bit `0x0800` →
  `+0x6a` edge `0x0800` (1-frame pulse) → `+0xb6` advances.
  `buttons=0xf7ff` yields report `0xfff7` → decoded `0x0008` — wrong bit.
- No non-SIO writer of `+0x6a` exists; `p2_device="none"` + mash attack
  set `[0xEFFF,0x7FFF,0xDFFF,0xBFFF]` never asserts `0x0800`.

## Evidence — residual stall (both `0x0300`)

- `gwatch` armed on `0x1f8000b4`(8B) + `0x1f800092`(8B): **0 hits** over
  20 s (~50+ frames); earlier 16-byte window on `+0x68` showed only the
  per-frame pad-poll rewrite flood.
- `freeze_check` (9986): `bce_calls` climbing, `bce_cls=0x300`,
  `pbd_calls=0`, `tpool_fail_count=1` (`head=0x801DAD98`,
  `next=0x801D65EC`, `size=0x844`, `p=(2,2,9432)` — one pool-2 alloc of
  0x844 bytes failed; pool2 also hosts the 0x24bc options-menu object).
- Task pools: pool1 = one node `0x801D088C` (`LAB_80137680` channel
  cursor, param `0x801D08A8` — script cursor cycling `0x801b2e84..`
  jump-table region, `0xfe00` loop-back = fighter idle channel, healthy);
  pool2 = `0x801D80E0` (`options_menu_alloc` task, dormant).
  `battle_intro_fsm` active: `_DAT_801f0738=3` (per-frame
  `task_pools_tick(2)`+`task_slot_fns_run(1)` branch).
- Fighter spawn tasks (`0x801b2ed8/0x801b2ef4` handlers via
  `task_alloc_wrap_c`) are **absent from pools** — `fighter_spawn_setup`
  commit (`rec+0x10==1 && phase==0x700`, `menu.c:3788`) never fired.
  T-records @`0x801ffe80`: `T[0]+0x10=0`, `T[1]+0x10=1`, both `+0x24=2`.
- CD counters frozen (`cdfn_dump` all static): `file_load_sm=638`,
  `tile_stream_pump=2004`, `mc_sm=948` — loads complete, drive idle
  (`last_sector frame≈3840`, `pending Pause 0x09`). `bfrd_set=3931` vs
  `bfrd_clear=14`, `ring_partial=1824`, `ring_dropped=2` — the recurring
  imbalance signature, but **no load is in flight** so it is not
  currently starving a consumer.

## Phase-machine analysis (why 0x300 has no clean exit)

`battle_config_eval` (`menu.c:4012`): cls `0x300` → `L_state3`→`L_B0`→
announce only (and in case8 `param_3=1` → `fz=0` → even the announce is
skipped). The **only** clean-C writer of phase `0x700` is `L_set700`
(`menu.c:4154`), reachable from cls `0x000` (`bcfg_step_a`) or cls
`0x600` (`continue_countdown`). Writers of `0x500` exist only in
`post_battle_dispatch` (`menu.c:7151`, versus `9b==4` — `pbd_calls=0`).

So for the first arcade fight, exiting announce `0x300` requires either
(a) a script-VM/task writer of the phase globals
(`g__DAT_1f8000b4/0xb6`, `bind.c:1789`) that is not currently scheduled,
or (b) a `0x300→0x000/0x100` reset followed by `bcfg_step_a` — no clean
writer of that reset was found in the per-frame path.

## Open question handed forward

Identify the guest function/task that stores `0x700` into
`0x1f8000b4/b6` after the intro timeline (the "script-VM globals" of
`downstream_spawn_commit.md` §2). Candidates not yet exonerated:
`script_stream_tick` (`0x80137824`, `task_alloc_wrap_d`) — never
observed allocated at this state; `LAB_80137cd0` variant; an event/EV
script; or a channel-script handler in the `0x8014cd48..0x8014cdbc`
tail entries (unported → generated). The `tpool_fail_count=1` (0x844
pool-2 alloc) is a candidate missing-task lead.

## Ops note

`pkill -f arcade_fight_mash.py` was run during the session — it killed
the mash drivers on **all** fleet ports (9962,9970,9980,9982,9990-9995),
not just 9980. The game instances themselves were not killed (9980's
instance later exited on its own — likely the known flaky crash).
Siblings should restart their drivers if needed.
