# Battle DAT1=3 wedge — codec thread alive, waits on unset result flag

Run: port 9200, headless, frame ~122k, `g_game_state` byte@0x1F800000 = 0x08,
battle phase byte@0x1F800001 = 0x03 (DAT1=3). CD command history ends ~frame
5286; ~117k frames of no new CD commands since.

## Live thread state (sp_ring, TCB 0x8FE0 — single codec thread)

Per-frame 3-phase cooperative cycle, healthy yield/resume:

1. `0x801BDD30 → 0x801FFE18 target 0x8019A53C` — game stack, `cd_timer_read`
   deferred entry (called from `cd_result_wait_a` poll loop).
2. `0x801FFE18 → 0x00008548 target 0x00002924` — kernel idle/yield
   (`ChangeClearRCnt` path), `ra=0x0000282C`.
3. `0x00008548 → 0x801BDD30 target 0x8019A87C` — resume into the CD
   deferred/IRQ-poll region.

The codec thread is NOT deadlocked — it polls once per frame and yields.
It is waiting for a flag that never gets set.

## Guest stack at block time (from prior session dump)

Frames present: `cd_file_read_seq` (0x801565EC) → `cd_cmd_issue`
(ra=0x80196EDC inside 0x80196E14) → `cd_result_wait_a` (ra=0x80196954
inside 0x801968CC) → `cd_timer_read` (0x8019A460) → `vsync_wait_n`
(0x8019A648, epc observed there).

`cd_result_wait_a`/`cd_result_wait_b` are delegated to generated
(`game/cd.c:618`) precisely because the Sync-wait must cross basic-block
boundaries for `psx_check_interrupts_at` — delegation is correct; the wait
itself is faithful.

## CD hardware / event state at stall

- `pending.cmd = 0x09` (Pause), `active = 0`, response FIFO drained
  (`response_read == response_count`), `irq_flag = 0`.
- `0x801BCB24 = 0x02` — last Pause response byte consumed.
- Read session block `0x801BCB80`: `sector_size = 0x200`,
  `0x801BCB94` (remaining) = 0 — last one-sector chunk completed;
  LBA cursor 0x2262 (8802), last sector 8801.
- `I_STAT` = VBlank only; VBlank/DMA/GPU/SPU all healthy.
- Last `DeliverEvent(0xF0000003, 0x40)` (read-complete class) @ ~1.054B
  cycles; last `DeliverEvent(0xF0000009, 0x20)` (Pause/result class)
  @ ~978M cycles. No DeliverEvent since.
- `TestEvent(0xF1000008)` spins ran until ~978.67M then stopped —
  consistent with the seek-poll consuming the Pause READY and the read
  sequence completing.

## Result-flag cells

`game/cd.c:627,784` comments: the deferred CD-IRQ coroutine sets
`0x801BCB25`/`0x801BCB26` on command completion — these are the flags
`cd_result_wait_a/b` poll. Live read: `0x801BCB24..27` bytes =
`02 00 00 00` — **0x801BCB25 and 0x801BCB26 are never set**.

## Hypothesis (unproven — next step)

The codec thread waits in `cd_result_wait_a` for a per-command result
byte (0x801BCB25/26 family, written by `cd_response_process` /
the deferred coroutine via computed index — no literal references in
generated output). Either:

(a) the last issued command's completion was consumed by the wrong
    slot/index (off-by-one in the command→result-cell mapping), or
(b) `cd_response_process`/the deferred coroutine stopped running before
    writing the result cell (its own upstream wait unmet), or
(c) `cd_file_load_sm` issued a command whose completion path expects a
    different event class than what the CD IRQ delivered.

CD IRQ delivery itself was proven healthy earlier (sectors + Pause
completions consumed; `bios_svc_trace` shows TestEvent/DeliverEvent
flowing). The missing edge is therefore **inside the result-cell write
path or the state machine's next-session condition**, not IRQ hardware.

## Next diagnostic steps

1. `fntrace_arm`/`pc_probe_arm` on `cd_response_process` (0x80196370)
   and the deferred-coroutine resume target (0x8019A53C caller chain) —
   confirm whether the coroutine still runs each frame and where it exits.
2. Dump the TCB 0x8FE0 saved context / guest stack at 0x801FFE18 region
   to get the exact blocked guest PC inside `cd_result_wait_a` (the flag
   load address reveals which cell it polls).
3. Compare against an earlier successful `cd_file_load_sm` session:
   session fields at 0x801BCB80 (LBA, remaining, dest cursor, size) —
   determine whether remaining=0 is correct (file fully read → consumer
   flag expected elsewhere) or premature (file truncated → upstream
   size/sector accounting bug).
4. Check whether `cd_file_load_sm` state byte expects a "load complete"
   flag written by the decompression/tag consumer rather than CD — the
   stack args `0x1F8000E8`/`0x1F8001F0` seen in the wait frame are the
   battle side-record pointers, hinting the wait may be for decoded-data
   readiness, not CD.
