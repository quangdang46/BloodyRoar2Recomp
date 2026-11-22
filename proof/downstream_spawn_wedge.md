# Downstream wedge: phase-seed fix exposes announce→spawn stall

Status: CONFIRMED live. This is the SECOND, independent wedge behind the
CPU-phase-seeding wedge documented in `cpu_phase_wedge_disconnected_pad.md`.

## What was proven this session

The CPU-phase-seed fix (synthesizing the missing slot-1 confirm edge so
`mc_slot_action` seeds `battle+0xb6`) DOES advance the state machine — but it
exposes a downstream stall that was previously masked.

Live repro (headless instance, port 9045, arcade mode `9b=0`):

- Injected a slot-1 confirm edge into the pad pipeline → `battle+0xb6` seeded.
- Both phase words reached `0x0300`:
  `b4=0300 b6=0300 92=0100 8c=ffff 9e=00`, DAT1=03.
- The game then sat STABLE at that state for thousands of frames
  (frame 7848 → 8253+, ~6+ s at ~60 fps) — far longer than a "ROUND 1 / FIGHT"
  announce cinematic (~2–3 s). It is wedged, not merely slow.

So the phase-seed is necessary but NOT sufficient: the announce→spawn commit
never fires.

## State at the downstream wedge

Scratchpad `0x1f8000b4/b6` = `0x0300/0x0300`; `+0x92` class word `0x0100`;
`+0x8c` counter `0xffff`; DAT1 `0x03` (battle active).

`battle_config_eval` phase-`0x300` path only calls the announce helpers
(`stage_fight_announce`, `fight_timer_announce`, `mode_name_announce`) and
returns — it does NOT advance the phase. Those helpers are clean-ported
(`game/menu.c:4286/4344/4465`) and are COSMETIC (draw "STAGE X / ROUND N /
FIGHT" text via `hud_text_blk_alt`); they do not write the next phase.

`fighter_spawn_setup` (`game/menu.c:3758`) promotes `0x500→0x600` and only runs
fighter setup when a side reaches phase `0x700`. The `0x300→…→0x700` advance
must come from the spawn/task/asset-load path, not from `battle_config_eval`.

## CD-ROM anomaly at the wedge (likely the real blocker)

`cdrom_state` on the wedged instance:

- `last_sector lba=8801 frame=1571` — last delivered sector is early; drive idle.
- `pending.cmd=0x09 (Pause) active=0 phase=1` — a Pause left pending.
- `reading=0`, `sector_available=1`, `response_read=1`.
- **`bfrd_set=2047` vs `bfrd_clear=13`** — buffer-full flow-control asserted
  ~2047 times but cleared only ~13: a massive imbalance.
- **`ring_dropped=1`, `ring_partial=898`** — the sector ring lost/dropped
  payload and delivered ~898 partial sectors during the sustained asset stream.

Interpretation: the fighter/asset stream used the CD ring buffer heavily and
the flow-control / partial-sector accounting diverged. If the spawn waits on a
file-load-done event or a resource-ready flag that the ring never satisfied,
the announce→spawn commit starves — matching the observed stable `0x300` stall.

This mirrors the earlier CD deferred-completion wedge class
(`cd_finalize_gate.md`, `cd_dma_dispatch_channel.md`): a completion edge the
game counts is not being produced by the runtime's CD ring/DMA model.

## What is NOT the cause

- NOT the announce helpers being stubs — they have strong clean ports and are
  cosmetic.
- NOT `battle_config_eval` failing to advance — by design it only announces at
  `0x300`; the advance is downstream (spawn/task/asset-ready).
- NOT a missing controller — both phases are seeded; input is no longer the
  blocker at this state.

## Refinement (this session): the wedge is DAT1=3 inside `battle_init`, an event-wait

Live probes on the wedged instance (port 9045) narrowed the stall precisely:

- **DAT1 (`0x1f800001`) is stable at `0x03`** across repeated samples — the
  battle bring-up state, NOT the announce substate. (`battle+0xb4/b6=0x300`,
  `+0x8c=0xffff`, `+0x92=0x0100` all confirmed.)
- **`post_battle_dispatch` (`0x80154930`, the `+0x92` substate machine that
  would advance announce→spawn) has `pbd_calls = 0`** — it is only invoked
  from `battle_flow_state_machine` case `DAT1=='\b'` (state 8), which is never
  reached. `battle_config_eval` still runs 53814× via `fighter_spawn_setup`,
  always seeing class `0x300`.
- DAT1 flow (`battle_flow_state_machine` `0x8016fcc4`): `2`→`battle_state_init`
  (seeds phases)→`3`→`battle_init` (internal `for(;;)` bring-up loop)→`4`→
  `menu_res_load_sm`→`5`→`battle_session_arm`→`6/7`→`8`→`post_battle_dispatch`.
  DAT1=3 calls `battle_init` once `frame_divisor_get()` (`0x1f800019` byte)
  returns 0 — confirmed 0 — then `battle_init` loops internally on `frame_tick`
  until `s6==0xff`. `+0x8c=0xffff` is `battle_bringup_a`'s done marker, so the
  CD `cd_file_load_sm` bring-up completed; the stall is LATER in `battle_init`.
- **`bioscall_dump`: B0:0x0B (`TestEvent`) = 1,402,471 calls** — the game is
  spinning on an event flag that never gets set. This is the same
  deferred-completion wedge *class* as the earlier CD finalize starvation
  (`cd_finalize_gate.md`): a completion edge the loader counts is not produced.
- The `decomp_trace` ring (LZSS `heapA_w_menu`/`lzss`/`blk_ret`) filled heapA
  `0x800B…→0x800E1064` then **froze at seq 213** — a large decompression/load
  ran to a point and stopped, consistent with a load that never signalled done.

So the announce→spawn stall is really: **`battle_init` (DAT1=3) is parked in a
`TestEvent` wait for an asset/decompression completion event that the CD
channel/deferred path never delivers.** `post_battle_dispatch` never runs
because DAT1 never reaches 8.

## Next steps (handed to runtime agent)

1. Identify WHICH event the `TestEvent` (B0:0x0B) spin waits on inside
   `battle_init`'s DAT1=3 path — the asset/decompression completion flag.
2. Determine whether that event is produced by the CD channel/deferred path
   (`cd_file_load_sm`/`cd_mc_state_machine`/DMA-done `DeliverEvent`) and which
   edge is dropped — compare against `psxrecomp/runtime/src/cdrom.c` ring/BFRD
   (`bfrd_set=2047` vs `bfrd_clear=13`, `ring_partial=898`, `ring_dropped=1`).
3. The faithful fix is in the runtime CD model or a clean-port callback —
   never a direct DAT1/phase poke.
