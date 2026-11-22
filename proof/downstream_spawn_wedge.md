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

### Decompression-starvation mechanism (refined)

The `decomp_trace` ring shows the load that stalls is a **bulk LZSS
decompression into heapA** (`heapA_w_menu` → `dispatch`/`lzss`/`blk_ret`),
consuming compressed blocks from a source buffer in the `0x801CAxxx–0x801D1xxx`
region and writing sequentially `0x800B…→0x800E1064`. It then **stops producing
entries** (ring frozen at seq 213) while `TestEvent` keeps spinning — i.e. the
decompressor is starved of compressed input mid-stream.

The compressed source is CD-delivered data. `cdrom_state` on the same instance
shows the delivery-side divergence: **`bfrd_set=2047` vs `bfrd_clear=13`**,
**`ring_partial=898`**, **`ring_dropped=1`**. Sectors were lost/partial-delivered
during the sustained asset stream, so the compressed buffer has gaps → the
decompressor can never reach its terminator → the "load done" event never fires
→ `battle_init`'s `TestEvent` spins forever → DAT1 stays 3.

This makes the downstream wedge the **same root cause as the earlier CD
ring/DMA starvation** (`cd_finalize_gate.md`, `cd_dma_dispatch_channel.md`):
the runtime's CD sector ring / BFRD flow-control drops or re-orders delivery
under sustained load. The faithful fix belongs in `psxrecomp/runtime/src/cdrom.c`
(ring/BFRD/sector accounting), not in `game/*.c`.

## DECISIVE reframe: the CPU-phase-seed is NOT the blocker

A **no-injection** arcade run (fresh instance port 9060, slot-1 present but
NEVER pressed — `/tmp/nav_arcade_noinject.py`) reproduced the **identical**
wedge:

```
t=30.0 gs=8 DAT1/ph=f1 9b=0 9c=0 b4=0100 b6=0000
t=39.3 gs=8 DAT1/ph=03 9b=0 9c=0 b4=0300 b6=0000   <- same DAT1=3 wedge
```

`cdrom_state` on 9060: `reading=0`, `pending.cmd=0x09 Pause`,
`last_sector lba=8801 frame=1583`, `ring_dropped=1`, `ring_partial=898` —
byte-for-byte the same signature as the injected instance (9045).

**Conclusion:** `battle_state_init` (DAT1=2) advances DAT1→3 regardless of
whether `battle+0xb6` seeded — it only *conditionally* promotes phases
(`select_phase_gate==0`). The DAT1=3 CD/decompression event-wedge is reached
with or without the CPU confirm. The earlier "CPU-phase-seed wedge" was a
**misdiagnosis**: `b6=0` is just the not-yet-confirmed CPU state and does NOT
block reaching battle bring-up. It may matter later at DAT1=8
(`post_battle_dispatch`, actual fighter spawn), but that is downstream of —
and secondary to — the DAT1=3 CD wedge that blocks ALL modes.

**Reprioritization:** the ONLY critical-path bug is the DAT1=3
`battle_init`/`frame_tick` event-wait (a completion event the CD/deferred
path never delivers). Fix that first; re-evaluate whether the CPU side needs
a literal pad-2 edge afterwards (likely only for versus modes, and possibly
resolved by battle-spawn logic once assets load).

## Next steps (handed to runtime agent)

1. Identify WHICH event the `TestEvent` (B0:0x0B) spin waits on inside
   `battle_init`'s DAT1=3 path — the asset/decompression completion flag.
2. Determine whether that event is produced by the CD channel/deferred path
   (`cd_file_load_sm`/`cd_mc_state_machine`/DMA-done `DeliverEvent`) and which
   edge is dropped — compare against `psxrecomp/runtime/src/cdrom.c` ring/BFRD
   (`bfrd_set=2047` vs `bfrd_clear=13`, `ring_partial=898`, `ring_dropped=1`).
3. The faithful fix is in the runtime CD model or a clean-port callback —
   never a direct DAT1/phase poke.

## DECISIVE mechanism pin-down (this session, live on port 9060)

The awaited event and the starving producer are now identified exactly:

- **The spin is `TestEvent(0xF1000008)` inside `cd_seek_step` /
  `cd_seek_complete_poll`** (guest `0x8019d508`/`0x8019d500`, both delegated to
  generated). `0xF1000008` = EvCB slot8 = `OpenEvent(0xF0000009,0x20)` — the
  **CD-DMA-done** flag. B0 table decode: `0x0B`=TestEvent (the 1.4M-call
  spin), `0x17`=ReturnFromException (the constant IRQ-epilogue calls —
  normal, not the stall).
- **`DeliverEvent(0xF0000009)` is produced by `cd_dma_chan_reset`**
  (`0x8019b7e8`, clean `game/cd.c:5113`) on a DMA-channel completion when the
  per-channel callback cell `0x801bdee8==0`. Live: `0x801bdee8=0` → the
  DeliverEvent path is the armed one; the event simply never gets produced
  because the completing channel work never runs.
- **All requested CD sectors WERE delivered.** `cdrom_sector_history`: every
  LBA through 8801 `data_delivered=1`. `dma_cdrom_history` tail (game path
  `func=0x2970 pc=0x801978AC`): each sector = a 3-word header DMA + a
  512-word (2048 B) payload DMA, ALL `completed=1`, frames 1581–1583. Last
  CD command = `Pause 0x09` (seq144, frame 1583). So the ch3 CD→scratch feed
  is complete and correct; nothing is still streaming.
- **The channel-finalize SM is dormant with orphaned work.** `0x801fc68c=0`
  (per-frame gate), `0x801bc848=0`/`0x801bc84c=0` (session callbacks —
  `cd_mc_sm_wrap` NOT installed), `0x801fc6a0=0`/`fc6a4=2`/`fc6a8=0`
  (scan=2 finalize=0 → two scanned entries never finalized). Table base
  `0x801fc6c0=0x80010000`, 32 slots (`0x801fc6c4=0x20`); slot head
  `0x80010000=0x1D` then `0xFFFFFFFF` empties, entry payloads ~`0x80010b00`.
  `0x801bcb24=2` (cmd-done flag SET).
- **DMA channels all idle:** `active=0 remaining=0` everywhere; ch4
  `madr=0x4F6E0 bcr=0x0F930010 chcr=0x201`, `dicr=0x00900000`.

### Refined causal chain

The game finished reading every requested sector into scratch (ch3 DMA all
`completed`), then needs the channel/finalize path to turn the buffered
payload into the completion that fires `DeliverEvent(0xF0000009)`. That path
is the `cd_mc_state_machine` ring-walker, driven by (a) the per-frame
`fc68c` gate, (b) the IRQ `bc84c=cd_mc_sm_wrap` session callback, and (c) a
DMA dispatcher. At the wedge **all three drivers are zero** while
`scan=2 > finalize=0` entries remain → the SM never runs → the ch4/doorbell
completion never submits → `DeliverEvent(0xF0000009)` never fires →
`TestEvent(0xF1000008)` spins forever inside `battle_init` → DAT1 stuck at 3.

This is the same channel-finalize starvation class as
`cd_finalize_gate.md`/`cd_dma_dispatch_channel.md`, now localized to the
precise awaited event (`F0000009`/handle `F1000008`) and the precise spin
site (`cd_seek_step`). The open sub-question handed to the runtime agent:
whether the break is (A) the SM never re-driven after the last session
(driver cells cleared and not reinstalled), (B) a submitted ch4 DMA whose
done-IRQ→`cd_dma_chan_reset`→`DeliverEvent` edge dropped in the runtime,
or (C) `Pause 0x09` truncating a read the game expected to finish
(psx-spx/Beetle Pause semantics).

## DMA-done callback registration — the producer side (this session)

`cd_dma_chan_reset` is reached through the game's DMA-channel done-callback
table, NOT a direct call:

- `cd_chan_cb_set` (`0x8019B0FC`, `game/cd.c:4115`) registers a per-channel
  done-handler at `0x801BDDF4 + idx*4` and sets the matching DICR IRQ-enable
  bit `1 << ((idx+16)&31)` in the DICR register (`0x1F8010F4`, pointer cell
  `0x801BDDF0`).
- **Live on the wedged instance:** handler table `0x801BDDF4..` is all zero
  EXCEPT `0x801bde04 = 0x8019B7E0` (= `cd_dma_chan_reset` prologue;
  clean port registered at `0x8019b7e8`). So **channel-table index 4 has
  `cd_dma_chan_reset` armed** → its completion calls
  `cd_bios_svc_07(0xf0000009,0x20)` = `bios_svc(0xb0,7,…)` = **B0/0x07
  DeliverEvent(0xF0000009,0x20)** — exactly the awaited event.
- `dicr=0x00900000` → bits 20 and 23 set = DICR IRQ enabled for idx 4
  (bit `(4+16)=20`) and idx 7 (bit 23, handler[7]=0 — enabled but no
  handler). So channel-4 completion IRQ is armed and would dispatch to
  `cd_dma_chan_reset`.
- `cd_dma_chan_reset` itself (`game/cd.c:5113`): if the per-channel
  callback cell `0x801bdee8 != 0` it calls that instead; else it calls
  `cd_bios_svc_07(0xf0000009,0x20)`. Live `0x801bdee8=0` → DeliverEvent path.

**So the missing edge is concrete:** a DMA transfer on the channel-index-4
slot must run to completion → DICR IRQ → `cb`-dispatch →
`cd_dma_chan_reset` → `DeliverEvent(0xF0000009)` → `TestEvent(0xF1000008)`
satisfied. `dma_state` shows that channel `active=0 remaining=0` — either
the transfer completed without its IRQ reaching the dispatcher, or it was
never submitted because the channel-SM/finalize path that arms it is dormant
(`fc68c=0`, `bc84c=0`, `scan=2>finalize=0`). `dma_trace_dump` tail is flooded
with unrelated per-frame ch2 GPU-OT DMAs; isolating the idx-4 submit/complete
pair around frame ~1583 is the subagent's live-trace target.

## REFINEMENT — the awaited channel is ch4 = SPU, not CD file data

`cd_cb7_run` (`0x8019af84`, `game/cd.c:5189`) is the DICR walker: it reads
`(DICR>>24)&0x7f` (per-channel done flags at DICR bits 24-30, matching
`complete_transfer`'s `dicr |= 1<<(24+ch)`) and calls `handler[ch]` at
`0x801BDDF4+ch*4`. So handler-table **index == hardware DMA channel**.
handler[4]=`cd_dma_chan_reset` therefore fires **only on DMA channel 4 =
the SPU channel** completion.

Consequence: `DeliverEvent(0xF0000009)` requires a **ch4 (SPU) DMA
completion** — i.e. the game is waiting for a sound-data upload / SPU-RAM
transfer to finish during battle bring-up, NOT for CD file sectors (those
all completed). This widens the wedge from "CD stream" to **the game's
DMA-channel orchestration**: the channel-finalize SM that submits channel
transfers (CD-data via ch3 AND SPU/sound via ch4) is dormant, so whichever
transfer the current load phase needs never gets kicked.

Live SPU state confirms no transfer is in flight or stalled:
- `spu_status`: `ctrl=0xC001` (SPUCNT transfer-mode bits5:4 = 0 = STOP),
  `active_mask=0`, `cd_frames=0`/`cd_push_frames=0` (CD-audio path idle),
  `render_frames≈59.8M` (SPU IS clocking → sample boundaries advance, so a
  mode-gated `spu_wait` would resolve if a transfer were pending).
- `spu_events`: only 54 events, all `KEYON` at frame 318 (boot init) — no
  SPU DMA/voice activity during the wedge.
- `audio_events` tail: only per-frame `RENDER`, zero `DMA_WRITE` — **no ch4
  transfer has run recently at all.**
- ch4 `chcr=0x201` (direction=to-SPU, busy bit24 CLEAR) → not in
  `spu_ch4_arm_mode_wait`; simply **never submitted**.

So hypothesis (A) is confirmed by elimination: **the game-side submitter
never kicked the awaited ch4 transfer** because the upstream channel-SM /
load coordinator is dormant (`fc68c=0`, `bc84c=0`, `scan=2>finalize=0`).
This is faithful-game-state starvation, not a runtime IRQ/DMA drop — the
runtime correctly reports "no ch4 transfer pending." The faithful fix is to
find why the SM's drivers were cleared and not re-armed for this load
phase (the `cd_chan_reset_b` mode-2 teardown vs the `cd_mode_set_0e`
`sm_wrap` reinstall ordering), NOT to fake a completion.
