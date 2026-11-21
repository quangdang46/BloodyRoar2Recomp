# Wedge C — arcade/1P spawn-phase starvation: diagnosis

Date: session continuing after commit `8adec77` ("CD is healthy; actual wedge
is spawn-phase starvation").

## TL;DR — diagnosis REVISED

The observed freeze (`g_game_state==8`, `DAT_1f800001==3`,
`battle+0xb4==0x0300`, `battle+0xb6==0x0300`/`0x0303`,
`battle+0x92==0x0100`/`0x0001`, `_DAT_801f0738==3`) is **not** a missing
battle spawn-phase driver. The main thread (TCB `0x00008FE0`) is **parked
inside the CD-ROM deferred-interrupt coroutine**, spinning

```
cd_timer_read (0x8019a460) -> kernel yield (0x00002924) -> cd_chan_full_init (0x8019a814)
```

once per frame (`sp_ring`, ~640 k cycles/iter, `frame` counter advancing).
A `CDREAD (0x09)` issued for battle-asset streaming never completes, so the
coroutine is never resumed past the wait and the whole battle state machine —
including `battle+0xb4/+0xb6/+0x92` — is frozen as a **downstream symptom**.

## Runtime evidence (debug port 8932, headless, fresh arcade/1P)

`sp_ring` tail — identical 3-call loop every frame, same TCB:

```
target 0x8019A53C  ra 0x8019A53C   (cd_timer_read, generated-delegated)
target 0x00002924  ra 0x0000282C   (kernel/idle trampoline)
target 0x8019A87C  ra 0x8019A87C   (cd_chan_full_init, generated-delegated)
```

`cdrom_state` (frozen across 3 s wall time — `seq` stuck at 997505):

```
pending {cmd:0x09, active:0, phase:1}   reading:0   stat:0x02
request:0x80 (BFRD set)                irq_flag:0x00  int1_pended:0
sector_read_pos:2060 / 2340            seek_msf:[1,58,70]  read_msf:[1,59,27]
ring_dropped:29   ring_partial:22918   bfrd_set:45373  bfrd_clear:41
irq_present_late:29720
```

- The CD engine is **completely idle-frozen**: `seq`, `sector_read_pos`,
  `pending`, `stat`, `reading` all identical over seconds while guest frames
  advance. No CD command is being processed.
- `request:0x80` = `CDROM_REQUEST_BFRD` stuck **set**; `bfrd_set:45373` vs
  `bfrd_clear:41` — the buffer-ready flag was raised ~1000x more often than
  cleared. `ring_dropped:29` + `ring_partial:22918` show the sector ring
  overflowed/dropped data during the stalled read.
- `irq_present_late:29720` — CD IRQs are being presented late at scale; the
  completion IRQ the coroutine waits on is not arriving in the window the
  driver expects.
- `DAT_801bdde0` (the vsync/frame counter incremented by `cd_cb8_dispatch`
  on CD IRQ) **is** advancing (`0x25d11 -> 0x25dc1`), so the IRQ dispatch
  path itself is alive — the stall is specific to this read's completion.

`cdrom_command_history` tail: `0x02` seek -> `0x06` read at MSF 1:58:70 ->
`0x09` CDREAD (the pending record). LBA ~8770-8801, mode `0xA0`,
`last_sector.have_raw:1`.

## What this means for the "spawn-phase starvation" symptom

`battle_state_init` (`game/menu.c:4605`) forces `battle+0xb4/+0xb6` to
`0x300` for select-gated players and is called at outer `battle_flow_state_machine`
case 2 (and again at case 6). `DAT1==3` is the outer case-3 wait
(`frame_divisor_get()==0` -> `battle_init`). The phases being `0x300` at
`DAT1==3` is the **expected** post-`battle_state_init` state — they are not
stuck mid-transition; the machine simply never gets past the CD-blocked
point that precedes/accompanies `battle_init`.

## Exhaustive negative results (battle-side drivers — all ruled out)

Every function the task named was inspected in clean C *and* cross-checked
against `generated/`; none is a missing per-frame phase driver:

- `battle_config_eval` (`game/menu.c`, guest `0x80153544`): phase-class
  dispatch verified line-by-line. cls `0x300` -> `L_B0`/`L_B0_7cc`
  (`game/menu.c:4090-4112`) is **announce-only** (`stage_fight_announce`,
  `fight_timer_announce`); it writes no phase. cls `0x000` -> `bcfg_step_a`
  can reach `0x700`; cls `0x600` -> `continue_countdown` -> `0x700`/`0x800`;
  cls `0x700` -> `bcfg_step_c` resets to `0x100`; cls `0x800/0x900` tick the
  low byte. There is no `0x300 -> 0x700` edge inside this function — and the
  clean body matches the generated semantics.
- `select_phase_update` (`game/save.c:3158`, guest `0x8015a1e4`): runs every
  frame at outer case 1; writes select bookkeeping (`+0xa8/+0xac` tables,
  `s3/s2` record fields) — **never** `+0xb4/+0xb6`.
- `confirm_bookkeeping` (`game/menu.c:3940`): recomputes `+0x92` from
  `select_phase_gate`; explains the stale `+0x92==0x0100` (P1 later reached
  cls `0x300`, which is gated, but bookkeeping only re-runs on
  commit/state-init/case-9 — not per-frame in the loop).
- `engine_record_pair_update` (`game/menu.c`, guest `0x8014a708`): pure
  GPU/OT packet emission (`palette_attr_record_write`,
  `packed_u16_expand`) — no phase writes.
- `side_phase_advance` (`game/menu.c:7583`, guest `0x8015297c`): the only
  in-`0x80152xxx` writer of `+0xb2/+0xb4/+0xb6`, but its **only** callers are
  `post_battle_dispatch` win paths (`game/menu.c:7122,7148`) — downstream of
  battle exit, unreachable at `DAT1==3`.
- `vs_confirm_a/b/c/d/e`, `post_battle_dispatch`, `battle_sides_commit`:
  all run at outer case 8 / post-battle — downstream, cannot unblock `DAT1==3`.
- `battle_intro_fsm` (`game/battle.c:8827`, guest `0x80137098`): drives
  `_DAT_801f0738` 0->1->2->3 and intro task pools from the per-fighter render
  path; writes no `+0x92`/phase. Observed `_DAT_801f0738==3` means the intro
  FSM already completed its setup on a prior pass — consistent with a
  later-round re-entry, not a missing driver.
- Task callbacks `0x801b2ed8`/`0x801b2ef4`: allocated by
  `fighter_spawn_setup` **after** a successful `0x700` spawn commit — they
  cannot be the thing that produces `0x700`.
- `battle_frame` (`game/menu.c:8075`, guest `0x8014cdd0`) is a `for(;;)`
  internal machine that only returns at `DAT1==0xff`; its per-iteration tail
  (`L_tail`/`L_aeec`) runs `task_pools_tick(1)/(2)` + `frame_tick`. During
  internal case 3 the only phase evaluator is `fighter_spawn_setup` ->
  `battle_config_eval`, which cannot exit cls `0x300`.

**Conclusion on the circular-wait question:** there is no *battle-logic*
circular wait. The wait is a **CD-ROM completion wait**: the deferred
coroutine parked in `cd_chan_full_init`/`cd_timer_read` never receives the
read-complete event because the emulator's CD model froze the `CDREAD`.

## Suspect area for the actual fix (next investigation)

`psxrecomp/runtime/src/cdrom.c` — the sector ring / BFRD / IRQ-present path:

- `s_ring_dropped` / `s_ring_partial` (`cdrom.c:~200-218,1549`): unread ring
  slots overwritten while the consumer is blocked -> `ring_dropped`.
- `s_bfrd_set_count` (`cdrom.c:63,3079`): `CDROM_REQUEST_BFRD` writes; the
  45373-vs-41 imbalance means BFRD is re-asserted without the matching clear
  path running — likely the same consumer-blocked condition.
- `irq_present_late` (`cdrom.c`, `irq_present_set_remaining`/`irq_present_rem_cycles`):
  IRQ presentation is landing late at scale; the read-complete INT1 the
  coroutine waits on is presumably among them.
- `pending_arm`/`pending_set_remaining` (`cdrom.c:758-790`): the `0x09`
  pending record shows `active:0, phase:1` — need to determine whether the
  read's data-phase was ever scheduled or was dropped when the ring overflowed.

The faithful fix is in the CD model's ring-buffer flow control / IRQ-present
scheduling — **not** in `game/` battle code. No `game/*.c` change is warranted
by this evidence; forcing `0x700` or adding a phase driver would violate the
no-speculative-fix rule and would not address the real block.

## Caveats / open items

- This instance was driven to the wedge via the debug harness; earlier
  "CD healthy" proof (`8adec77`) exercised a simpler read path. The stalled
  `0x09` may be the first *sustained streaming* read (battle asset stream),
  which is exactly the path the ring/BFRD counters implicate.
- `pending param_count:0` on the `0x09` record is suspicious — either the
  command-history param snapshot missed the 8-word parameter block, or the
  command was issued through a path that bypassed param FIFO accounting.
  Worth checking `exec_command`/`record_command_history` for `0x09`.
- Not yet proven *which* game call site issued this read (battle-init asset
  stream vs. an earlier menu load). The frozen `DAT1==3` + `0x300` phases
  localize it to the post-`battle_state_init` window.
- No source changes were made; no rebuild/commit performed. `tools/wedge_c_watch.py`
  (untracked) remains the session's watch harness.
