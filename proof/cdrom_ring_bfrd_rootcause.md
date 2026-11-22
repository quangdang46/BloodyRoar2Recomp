# cdrom.c ring_dropped / BFRD root-cause — LBA 208850 guest-side skip

Instance: `build/BloodyRoar2_Recompiled --disc Game.cue --debug-port 9880 --headless`,
navigated to gs=8 ph=03 wedge (same wedge as port 9850: `decomp_trace` stalls at
seq 213 `heapA_w_menu` dst=0x800E1064 src=0x800E9078 len=0x22F2, CD paused).

## 1. The dropped sector — identified

New forensic instrumentation (`cdrom_drop_history` debug command, per-slot LBA in
`CdSectorBuf`) captured the single `ring_dropped` event:

```json
{"seq":0,"cycle":738633183,"frame":1090,"old_lba":208850,"new_lba":208858,
 "slot":6,"ring_read":5,"ring_write":5}
```

- **Dropped sector: LBA 208850** (MSF 46:26:50), delivered frame ~1087 during a
  `ReadS (0x1B)` stream, `Setloc 46:26:29` → first LBA ~208829, `SetMode 0xE0`
  (whole-sector + double-speed + XA-filter bit), sector_history shows
  `xa_file=1, xa_channel=1, xa_submode=0x48` — an XA-channel header-scan stream.
- Overwritten unread by LBA 208858 three frames later (slot 6, pos==0).
- Counters reproduced deterministically on a second instance (9850):
  `ring_dropped=1, ring_partial=898, bfrd_set=2047, bfrd_clear=13` — identical.

## 2. Why it dropped — guest-side skip, not a model overwrite error

Full `cdrom_trace` (42303 entries, complete since boot — cap 65536) analysis:

- The guest consumes this stream by reading **only the 12-byte sector header**
  via CPU FIFO `0x1F801802` (plus some DMA payload — pos reached 44/2060 on
  neighbors). 276 FIFO bursts total in the run.
- **Every burst's first 3 bytes decode (BCD MSF) to exactly the LBA of the
  sector whose INT1 was being serviced — except LBA 208850, which has ZERO
  bursts anywhere in the trace.** Sequence: …208849 → 208851 → 208852…
- INT1 for 208850 **was** delivered and seen by INTC (`irq_armed=1,
  intc_seen=1`, cdrom_timing_dump seq 431) and the guest **did ack it**
  (ack writes to `0x1F801803` present in its service window).
- But its service window contains **no BFRD arm** (no `request_reg=0x80`
  write) and **no FIFO reads** — the ISR was still inside the previous
  sector's status-poll loop (`0x1F801803`/`0x1F801800` reads returning
  0xE0/0x18/0x58…) when the next sector's INT1 arrived. Verified
  programmatically: SVC-208850 → `bfrd_arm=False, fifo_reads=0`;
  SVC-208851 → `bfrd_arm=True, fifo_reads=12`.

So the guest received the data-ready interrupt, acked it, and never consumed
the buffer — a one-sector skip in the game's own INT1 handling (its handler
was ~1 sector period late once; the poll-loop latency is guest-code-bound and
identical on real hardware). The buffer sat available for a full ~6.7 ms
(2x) sector period before being overwritten — the model gave the guest every
opportunity HW would have.

## 3. BFRD asymmetry — instrumentation artifact, not desync

- `bfrd_set` (2047) counts **every REQUEST write with bit7** — the per-sector
  "buffer ready" arm the guest performs inside each INT1 service.
- `bfrd_clear` (13) counts **`clear_sector_buffer()` calls while BFRD was
  set** — i.e. read-stream restarts (Setloc+ReadN), not per-sector events.

Different events entirely; `request_reg` stays `0x80` between sectors as
expected. No desync.

## 4. Latent fidelity gap noted (did NOT manifest here)

`s_ring_read = s_ring_write` on every delivery makes the read pointer track
the *newest* slot. If the guest had been slightly less late, its BFRD arm +
FIFO read for sector N would have landed after N+1's delivery and silently
read N+1's bytes. On real HW the 8-buffer FIFO preserves order and the host
reads the *announced* (oldest unconsumed) sector. This is a real semantic
divergence in `cdrom.c`, but it produced **no wrong-sector read** in this
run (all 276 header reads matched their INT1's LBA) and fixing it is a
per-slot consumption-tracking redesign of the kind that previously caused
multiple documented regressions in this exact code (see comments at
`deliver_read_sector_without_irq`, `process_read_stream`). Per the mission
rule — fix only a *proven* defect line — it is documented here, not changed.

## 5. Wedge correlation — the drop is NOT the cause

- The drop is in the XA header-scan stream (~frame 1087-1172), ~500 frames
  before the wedge.
- The decompressor's final stream (LBAs 8770-8801, mode 0xA0, frames
  ~1528-1624) had **zero drops**: all 32 sectors delivered, guest consumed
  each to pos=2060 (the benign whole-sector EDC tail), then the game issued
  its own `Pause (0x09)` — `reading=0`, `int1_lost=0`, `int1_pended=0`.
- `decomp_trace` still stalls at seq 213 on the fresh instance — identical
  to the 9850 wedge. The compressed source for block 213 either already
  arrived intact or the decompressor is gated on a guest-side signal
  (consistent with prior proofs: SM channel-finalize / result-flag routing,
  not CD data loss).

## 6. Changes made (observability only — no behavior change)

- `psxrecomp/runtime/include/cdrom.h`: `CDROMDropEntry` +
  `CDROM_DROP_HISTORY_CAP` + `cdrom_debug_get_drop_history` /
  `cdrom_debug_clear_drop_history` decls.
- `psxrecomp/runtime/src/cdrom.c`: `int lba` per `CdSectorBuf` (init -1,
  reset -1 on savestate parse); `s_drop_history` ring +
  `note_sector_drop()` recorded on every `pos==0` overwrite;
  `cdrom_debug_clear_drop_history()` on `cdrom_init`.
- `psxrecomp/runtime/src/debug_server.c`: `cdrom_drop_history` command +
  handler.

## Verdict

`ring_dropped=1` = LBA 208850, a faithful guest-side one-sector skip in an
unrelated XA header-scan stream. **No cdrom.c defect proven for the wedge**;
the CD layer delivered every sector the decompressor's stream requested.
The wedge remains the guest-side signal-routing issue documented in
`cd_deferred_irq_starvation.md` / `downstream_spawn_commit.md`.
