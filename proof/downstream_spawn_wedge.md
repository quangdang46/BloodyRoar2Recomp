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

## Next steps (handed to runtime agent)

1. Trace what writes `battle+0xb4/b6` OUT of `0x300` (target `0x500`/`0x700`) —
   find the spawn-commit / asset-ready predicate.
2. Correlate with the CD ring: was the fighter-asset file load issued, did its
   done-event fire, and did `ring_partial`/`bfrd` starvation lose it?
3. Compare against Beetle oracle for the same spawn window if needed.
4. Fix must be faithful (runtime CD ring/BFRD model or a clean-port callback),
   never a direct phase poke.
