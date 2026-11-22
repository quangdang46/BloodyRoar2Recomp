# CD deferred-interrupt starvation — consolidated evidence (arcade spawn wedge)

Status: **root cause narrowed to the CD response/result pipeline; the deferred
coroutine runs but is never fed a command-complete interrupt after the last
sector stream.** Deterministic across instances. Not yet fixed.

This consolidates three independent instrumented runs (ports 9200 subagent,
9810 main) plus static decode of the generated CD driver.

## The visible wedge

Arcade path (with slot-1 CPU confirm injection to pass the *separate*
CPU-phase-seeding wedge) reaches `gs=8 ph=03 b4=0300 b6=0300 +92=0100` and
freezes. `DAT1=3` (`0x1F800001` byte = 3). Fighter spawn never happens.

## Confirmed blocked thread

Codec/file-load thread parked in `cd_result_wait_a` (`0x801968CC`), polling
result flag bytes `0x801BCB25`/`0x801BCB26` (near response byte
`0x801BCB24`). Live read at wedge: `0x801BCB24..27 = 02 00 00 00` —
**both result-flag bytes stay 0 forever.** The function is correctly
`br2_call_generated`-delegated; the stall is in guest logic, not a clean-port
defect in the wait itself.

`cd_result_wait_a` loop (decoded from `generated/SLES_017.22_full_45.c`):
- `s2 = 0x801BCB24`; reads `lbu a0,0(s2)` (response) and `lbu v0,1(s2)`/`2(s2)`
  (the two flag bytes), indexes a result table at `0x801BC8EC` by flag byte,
  calls `cd_response_process` (`0x80196370`), then yields via
  `cd_timer_read`/`vsync_wait_n`.

## The deferred coroutine runs — but is never fed

`fntrace_arm` on the wedged instance (9810):

- `cd_intc_dispatch` (`0x8019A8EC`) **runs every frame** (~1/frame), called
  from `cd_chan_full_init` resume path (`ra=0x8019A88C`, `a0=0x801BCD50`,
  `a1=1`, `a3=0x4000` constant).
- `cd_response_process` (`0x80196370`) **does NOT run at all** in the last
  256+ frames (both targets armed; only `0x8019A8EC` entries present).

`cd_intc_dispatch` decoded (`generated/SLES_017.22_full_46.c`):
- Gate: `lhu v0, 0(s1)` where `s1=0x801BCD18` → value **1** (nonzero → "has work" path).
- Dispatch predicate: `result = i_mask & i_stat & 0x000D` where
  - `0x801BDDA4 → 0x1F801070` = **I_STAT**, `0x801BDDA8 → 0x1F801074` = **I_MASK**
  - mask `0x801BCD48 = 0x000D` = bits {VBLANK=0, CDROM=2, DMA=3}
- At wedge `irq_state`: `i_stat=0x00000000`, `i_mask=0x0000000D`,
  `pending=0`, `cop0_sr=0x40000401` (IE on). → `result=0` → takes the
  no-dispatch branch (`block_8019AA0C`) every frame. **Correct idle.**

## The IRQ timeline — CDROM IRQs fire during streaming, then stop dead

`cdrom_trace_dump` (healthy tail, frames ~1617-1618): `set_irq`/
`fire_irq` events show `i_stat=0x00000004` (**CDROM bit2 raised**) on every
sector `DATA_READY`, reads/writes of `0x1F801800`/`0x1F801803` (HINTSTS)
acking each. `read_cmd=0x06` (ReadN), `reading=1`, `pending_cmd=0x09` (Pause
queued).

`event_ring_tail` during the wedge: **only `istat=0x00000001` (VBLANK)
ISTAT_RAISE/IRQ_DELIVER events, ~1/frame, `pc=0x8019A60C`. No CDROM (bit2)
raise has occurred since the stream ended.** i_stat stays 0 between VBLANKs.

`cdrom_state` at wedge: `pending.cmd=0x09 active=0` (Pause already consumed by
`process_pending` → `case 0x09` cleared READ, pushed response, `set_irq
(CDIRQ_COMPLETE)`, `fire_cdrom_irq`), `response_read=1`, `irq_flag=0`,
`reading=0`, last LBA 8801 all `data_delivered`, `irq_present_late=1519`,
`ring_dropped=1`, `ring_partial=898`, `bfrd_set=2047` vs `bfrd_clear=13`.

## bc84c / SM-driver lifecycle (independent instrumentation)

`cdfn_dump` + `bc84c_seq`/`bc84c_caller` rings (new this session —
`mc_session_b` is the only clean-C writer of `0x801BC84C`; ring records
`(new,old,caller_ra)` per write):

- `bc84c` cycles `0 ↔ 0x801985C4` (`cd_read_step_cb`) across read sessions —
  park (`cd_read_setup`/`cd_read_open`) → install read cb → teardown clear.
- `sm_wrap` (`0x80198FE4` = `cd_mc_sm_wrap`) written **exactly once** as a
  `new` value (caller in `cd_mode_set_0e` region `0x80198FC4`), then cleared
  once, **never restored**. After that only the read-cb cycle remains.
- Caveat: `caller_ra` (`clean_caller_ra` = live guest `$ra`) is precise for
  generated→clean calls but coarse for clean→clean calls (holds the outer
  invoker's ra). The `(new,old)` sequence itself is exact.

## Refined causal model

Two symptoms, one pipeline:

1. The channel-finalize state machine (`cd_mc_state_machine`) loses its
   `bc84c=sm_wrap` IRQ driver → `scan>finalize` orphaned entries → the
   doorbell/DMA-completion `DeliverEvent` the battle-init waits on never fires.
2. The codec thread waits in `cd_result_wait_a` for `0x801BCB25/26`, which
   only `cd_response_process` writes — and `cd_response_process` is never
   reached because `cd_intc_dispatch` sees `i_stat&0xD==0` (no CDROM IRQ
   pending) every frame.

Both reduce to: **after the last ReadN+Pause stream, no CDROM interrupt
(i_stat bit2) is ever raised again**, so neither the deferred-coroutine
dispatch nor the SM finalize is re-triggered. The Pause completion's
`CDIRQ_COMPLETE` was consumed (`process_pending` ran, response read) but the
guest-side result/flag bookkeeping for it never ran.

## Guest-side call chain (decoded `generated/SLES_017.22_full_45.c`, this session)

Precise structure of the two `cd_response_process` (`0x80196370`) feeders —
both are gated on the CDROM IRQ line, so **no i_stat bit2 ⇒ no flags, ever**:

- `cd_result_wait_a` (`0x801968CC`, codec thread): loop calls
  `cd_intc_busy_read` (`0x8019A7D4`) which returns the pending-INTC-IRQ
  status. `block_80196A14` tests `ret==0`:
  - **ret≠0 (CD IRQ pending)** → `block_80196A1C`: loads the response-table
    pointer at `0x801BCB04`, `lbu`s its flag byte, `&3`, then `jal
    cd_response_process` — this is the only in-loop path that sets flags.
  - **ret==0** → `block_80196ABC`: polls the awaited result-flag byte
    (`lbu` at `$s2`, compare vs `$s3`, special-case `==5`), then yields via
    `cd_timer_read` and repeats. **No `cd_response_process` here.**
- `cd_response_pump` (`0x801979F8`) = **INTC handler[2] @`0x801bcd1c`**, the
  CDROM IRQ-exception handler (gated on I_STAT bit2). On entry it loads the
  same `0x801BCB04` pointer → `lbu` → `&3` → **unconditional `jal
  cd_response_process`** at `block_80197A2C`, then fires the `bc84c`/`bc848`
  completion callbacks. It only runs when the CPU takes the CDROM IRQ.
- `cd_intc_dispatch` (`0x8019A8EC`) is the *deferred-coroutine* poll — it
  computes `i_mask & i_stat & 0x000D` and early-outs on 0 (correct idle),
  resuming `cd_chan_full_init`. It is **not** the flag setter.

Net: `0x801BCB25/26` can only be written while i_stat bit2 is observable —
either by the IRQ handler (`cd_response_pump`) or by the wait loop's
`cd_intc_busy_read`-gated `cd_response_process` call. With bit2 never
presented, both starve and the codec thread spins forever. Confirms the wedge
is entirely upstream: **the CDROM device must keep i_stat bit2 asserted
until the IRQ exception dispatches `handler[2]`.**

## ⚠ CORRECTION (live trace, port 9200) — the CDROM IRQ path is NOT broken

A full `cdrom_trace_dump` tail (seq 42233→42299, frames 1556→1558) **disproves
the "no i_stat bit2" hypothesis.** The final Pause (`0x09`) command's
`CDIRQ_COMPLETE` interrupt fired and was processed end-to-end:

- `42283 set_irq val=0x02` (CDIRQ_COMPLETE) → `42284/42285 fire_irq` →
  `i_stat=0x04` (bit2 raised).
- `42286-42291`: guest runs `cd_response_pump` (`pc 0x80197A1C`) →
  `cd_response_process` (`pc 0x8019638C/0x801963A8/0x80196420/0x80196464/
  0x80196474/0x80196484` — all inside `0x80196370-0x801968CC`), reads the
  Pause response byte `0x02` (`resp_read 0→1`).
- `42294-42295`: HINTSTS ack `0x07` → `irq_flag 0x02→0x00`; `42299` i_stat
  bit2 cleared. **Interrupt fully serviced.**

So `cd_response_process` DID run for the last command — the earlier
"never runs" was an fntrace-window artifact (armed after frame 1558). The
codec thread is *not* starved of CD IRQs.

## Live wedge state (port 9200 `mem_words`)

- `0x801bc848`/`0x801bc84c` = **0** — both session/completion callbacks cleared.
- `0x801fc68c` = **0** — `cd_mc_state_machine` dormant; `0x801fc6a4`(scan)=2,
  `0x801fc6a8`(finalize)=0 → orphaned channel entries.
- `0x801bcb24..27` = `02 00 00 00` — result byte present, awaited flags 25/26=0.
- INTC handler table `0x801bcd1c`: `[2]=0x801979F8` (cd_response_pump)
  **correctly installed**; enable mask `0x801bcd48=0xD` (bit2 CDROM enabled).

## Refined causal model (corrected)

The codec thread's awaited signal is produced by the **channel-entry
finalize / read-session-done path**, not by the raw CD IRQ. Sequence:

1. Read session streams sectors → game issues Pause → `CDIRQ_COMPLETE`
   processed correctly (above).
2. `cd_response_pump` fires the `bc848`/`bc84c` completion callbacks — but
   both are **0** (cleared by session teardown before the final IRQ), so the
   callback that advances channel state / produces the codec-done flag never
   runs.
3. `cd_mc_state_machine` stays dormant (`fc68c=0`, no `bc84c=sm_wrap` driver)
   → `scan=2 > finalize=0` orphaned → the doorbell `DeliverEvent` and the
   codec's awaited result flag never materialize → `cd_result_wait_a` spins.

**Root cause is therefore the `bc848`/`bc84c` callback lifecycle + SM driver
starvation — a guest-side teardown/re-arm ordering issue (or a faithful-port
defect in `game/cd.c` session handling), NOT a `cdrom.c` IRQ-presentation bug.**

## Open question (next experiment — supersedes the IRQ-presentation hypotheses above)

The CDROM IRQ path is verified working. The live question is the
**`bc848`/`bc84c` completion-callback + `cd_mc_state_machine` driver
lifecycle**: at the Pause-complete IRQ (frame 1558) both callback cells were
already 0, so the read-session finalize never ran. Determine:

- (a) Was a valid callback *supposed* to still be installed at Pause-complete
  (i.e., session teardown cleared it too early — a faithful-port ordering bug
  in `game/cd.c`), or does real hardware also clear it and produce the codec's
  awaited flag via a different path (the SM tick / `fc68c`) that our runtime
  fails to re-arm?
- (b) Which exact write zeroes `bc848`/`bc84c`/`bcbac` between the last
  `cd_read_step_cb` and the Pause-complete IRQ — the `bc84c_seq`/`bc84c_caller`
  ring on a *fresh instrumented* repro (port with current build) names the
  clearers.
- (c) Does `cd_mc_state_machine` need `bc84c=sm_wrap` re-installed by
  `cd_mode_set_0e` after the read session, and is that reinstall being skipped?

Discriminator: compare against the Beetle oracle — does the channel-entry
finalize run after the equivalent Pause on real hardware?

## Artifacts

- `proof/battle_dat1_3_codec_wait.md` — subagent f8f0de08 codec-thread trace.
- `proof/cd_callback_cells_perop.md`, `cd_finalize_gate.md`,
  `cd_dma_dispatch_channel.md` — earlier SM/callback lifecycle proofs.
- Instrumentation: `game/cd.c` `CDFN_*` counters + `g_bc84c_seq`/`g_bc84c_caller`
  rings; `cdfn_dump` debug command (`debug_server.c`); arrays in `interrupts.c`.
