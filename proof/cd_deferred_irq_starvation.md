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

## bc84c write ring — the SM-driver smoking gun (fresh repro, port 9850)

`cdfn_dump` `bc84c_seq`/`bc84c_caller` (new instrumentation) captured the full
`0x801BC84C` lifecycle across the wedge. Decisive entries:

- Per read session: `0x80198A88` installs `read_step_cb`(`0x801985C4`),
  `0x801987B8` clears it — normal park/install cycle, repeats ~20×.
- **`sm_wrap`(`0x80198FE4`) installed exactly once** — `new=0x80198FE4
  caller=0x80198FC4` (`cd_mode_set_0e`) — then **cleared one entry later**:
  `new=0 old=0x80198FE4 caller=0x801264E0`.
- `0x801264E0` is the return-addr inside generated `br2eu_func_801264B4`,
  which tail-calls **`scene_entry`(`0x80159028`)** with `a0=0x80010000` —
  i.e. the battle/scene loader cleared `sm_wrap` mid-load and it was **never
  reinstalled**. After that only the `read_step_cb` cycle remains.

`scene_entry` (`game/game.c:1580`) is the battle/arena loader: `blk_io_retry`
loops → `cd_stream_mode_init` → `cd_subsystem_reset` → `cd_cmd_issue_sync(9)`
(Pause) → `battle_session_arm`. The `mc_session_b(0)` that zeroed `bc84c`
ran inside its call tree.

## Wedge is NOT a CD spin on this instance (fntrace, port 9850)

Armed `blk_io_retry`/`cd_mc_state_machine`/`cd_result_wait_a`/
`cd_response_pump`/`cd_intc_dispatch` for ~4 s at the wedge:

- **Only `cd_intc_dispatch`(`0x8019A8EC`) fires — once/frame** from
  `cd_chan_full_init` (`ra=0x8019A88C`), the idle INTC poll (`i_stat&0xD=0`).
- `blk_io_retry`, `cd_mc_state_machine`, `cd_result_wait_a`,
  `cd_response_pump`: **0 hits.** The codec/file-load threads are *idle*, not
  spinning — the earlier "codec starves in `cd_result_wait_a`" was a different
  (older-binary) instance's transient, not the steady-state wedge.

So the CD subsystem is healthy-and-idle; the freeze is the **game-state /
battle spawn path** waiting on the channel-entry `state==2` finalize that the
dormant SM (`fc68c=0`, `bc84c=0`, `scan=2>finalize=0`) never produces.

## Refined causal model (corrected — mechanism A confirmed)

Subagent `2bdfd8d5` (proof/cd_pause_completion_irq.md, commit `1aae6f3`)
proved **mechanism A**: the final Pause `CDIRQ_COMPLETE` was raised,
delivered to INTC, and fully serviced — `cd_response_pump`→`cd_response_process`
read HINTSTS `0xE2` + response byte `0x02` and acked, entry-for-entry
identical to a healthy Pause completion. **No IRQ edge was dropped.**

The freeze is therefore **guest-side result-flag routing**, downstream of a
healthy CD command path:

1. `scene_entry` loads battle/arena assets; per-read `bc84c` cycles
   `read_step_cb` normally and `cd_mode_set_0e` installs `sm_wrap` once.
2. A nested call inside `scene_entry` (`0x801264E0`) runs `mc_session_b(0)`,
   clearing `bc84c` — `sm_wrap` never reinstalled → SM dormant
   (`fc68c=0`, `bc848=bc84c=0`, `scan=2>finalize=0`).
3. Final Pause completes cleanly, but the game **never issues the next
   Seek/ReadN** — a thread hot-loops in `cd_result_wait_a` polling result
   flags `0x801BCB25/26` that stay `0`.
4. Those flags are written by the deferred-IRQ coroutine when it processes a
   pending CD response; with CD idle and the session callbacks NULL, the
   awaited result never materializes → `cd_result_wait_a` spins → channel
   entries never finalize → spawn-commit (`state==2`) never true → the
   battle/game-state SM waits forever at `gs=8 ph=03`.

**Root cause is the `bc84c`/`sm_wrap` (and `bc848`) session-callback
lifecycle being cleared inside `scene_entry` without re-arm — a guest-side
ordering issue in the clean-port session handling (`game/cd.c`/`game/game.c`),
NOT a `cdrom.c` IRQ-presentation bug, NOT an OpenBIOS event bug, and NOT a
codec-thread IRQ starvation.** Open sub-question (agent `311baba4`): the exact
`cd_result_wait_a` exit predicate and the `0x801BCB25/26` writer's gating —
whether the awaited result needs a command the dormant SM never issues, or a
response whose flag-write was skipped on a NULL callback.

## Live driver audit (port 9850, ~frame 34k, wedge steady-state)

fntrace armed on all SM drivers + loaders for ~256 entries: **only
`cd_intc_dispatch`(`0x8019A8EC`) fires — once/frame** (the deferred-coroutine
idle poll). **Zero hits** on `cd_mc_state_machine`(`0x80199440`),
`cd_mc_sm_wrap`(`0x80198FE4`), DMA dispatch(`0x8019AF7C`), and
`scene_entry`(`0x80159028`). `cd_result_wait_a`(`0x801968CC`) also 0 hits on
*this* instance (the codec/load thread is parked, not spinning — instance
dependent; agent `2bdfd8d5` saw it hot-loop on 9830).

SM state block `0x801FC680`: `fc68c=0` (work flag), `fc6a4=2` (scan idx),
`fc6a8=0` (finalize idx), `fc6a0=0` (seq), `fc6c0=0x80010000`/`fc6c4=0x20`
(table base/32 entries), `fc6f8=0` (work-pending).

**Channel table `0x80010000` does NOT hold channel entries** — it contains a
parsed file directory (offset table `+0x44..` stepping ~`0xC22C`, then
u16-pair records `+0x90..`). The 32-slot entry table the SM scans has been
overwritten by file-load payload in the shared scratch arena — consistent
with the documented arena-sharing (`AGENTS.md` §8). Whether the SM *should*
still have live entries here (i.e. a finalize was owed before the overwrite)
is the faithful-vs-divergent discriminator.

EvCB snapshot: slot 8 `class=0xF0000009 spec=0x20 mode=0x2000(MARK)
status=0x2000(ENABLED)` — the CD-DMA-done event is armed but never PENDING
because no channel-4 DMA completion has fired since the CD went idle.

## Synthesis with downstream_spawn_commit.md (consumer-side view)

`proof/downstream_spawn_commit.md` (port 9045) isolates the same wedge from
the consumer side and they **reconcile into one causal chain**:

- Spawn commit needs `record[i]+0x10==1 && phase[i]==0x700`. Live: rec1 armed
  (`+0x10==1`), rec0 not; both phases `0x300`.
- `0x300` is terminal announce in `battle_config_eval`; the `0x300→0x700`
  write comes from the **intro/"FIGHT" script-VM sequence**, gated on
  **fighter model/anim assets being resident** — delivered by the CD
  decompress stream.
- `decomp_trace` froze at `seq 213` mid-LZSS; `cdrom_state` shows
  `ring_dropped=1`, `bfrd_set=2047`/`bfrd_clear=13`, last sector LBA 8801
  @frame ~1539 then CD idle.

**Reconciliation:** the CD *command/IRQ* path is healthy (mechanism A —
Pause completed + serviced). What is missing is the **data-completion /
load-done edge** the decompressor and the channel SM both wait on: the last
requested sector (8801) was consumed to byte 2060 (the expected
header+data tail for whole-sector mode), the game then issued Pause and went
idle — but the *stream-finalize / channel-entry `state==2`* signal that would
flush the decompressor's last LZSS block and write `phase=0x700` is produced
only by `cd_mc_state_machine` finalize, which is dormant (`bc848=bc84c=0`,
`fc68c=0`, all drivers silent, table overwritten by dir data).

So producer-side (this doc: SM/callback dormancy) and consumer-side
(downstream_spawn_commit: decompressor starved) are the **same defect** — the
channel-copy finalize never runs. The open faithful-vs-divergent question is
*why* the SM lost its driver: a runtime IRQ/callback-timing miss, or a
guest-side ordering the clean port must reproduce.

## Decompressor freeze pinpointed (decomp_trace, port 9850)

`decomp_trace` tail: the asset stream decompresses heap blocks
(`heapA_w_menu` → `dispatch` → `lzss` → `blk_ret`) into sequential dst
addresses (`0x800C7014`→`0x800CF028`→`0x800D703C`→`0x800DC800`→`0x800E1064`…).
**The last entry is seq 213 `heapA_w_menu` dst=`0x800E1064` src=`0x800E9078`
len=`0x22F2` at cycle ~1044005062 — with NO following `dispatch`/`lzss`/
`blk_ret`.** The heap block was allocated and a decompress was requested, but
the LZSS pass never ran → the decompressor is **waiting for the compressed
source bytes of block 213**, which arrive via the CD stream. This is a
*data-starvation* stall, not a logic stall.

## cdrom.c ring/BFRD accounting — the concrete anomalies

`cdrom_state` (authentic cadence — `mode_reg=0xA0`, `xa_stream_active` off,
`accelerated_read_active()=0` because `mode_reg&0x68 != 0`; `accel_consumer_
waits=0` confirms the accel guard never engaged):

- `ring_dropped=1` — exactly **one** sector was overwritten with `pos==0`
  (guest never read a byte) = "guest a ring behind". At authentic speed the
  guest has a full sector period per sector, so a single drop is anomalous.
- `ring_partial=898` — matches sector count; dominated by the documented
  benign format-tail (game reads 2060/2340). Not the defect.
- `ring_starved=0` — the O(1) tripwire never fired.
- `bfrd_set=2047` vs `bfrd_clear=13` — heavily asymmetric REQUEST/BFRD
  accounting (guest wrote BFRD-request 2047×, cleared 13×). Worth auditing
  whether these counters reflect real flow-control desync or just asymmetric
  instrumentation.

**Correction to the ring/BFRD emphasis above:** the decompressor's compressed
source is *not* read via the BFRD byte/word path — asset files are loaded by
`cd_file_load_sm` (`0x8018CFDC`, game/game.c:1717, generated-delegated) →
`cd_read_issue` → the **channel-DMA + `cd_mc_state_machine` finalize** path.
So the missing edge for decompress-block-213's source is the **channel-copy
finalize / "file loaded" signal the SM produces** — `ring_dropped`/`bfrd_*`
are almost certainly unrelated noise on this path. The decompressor waits on
the same SM finalize that the spawn-commit predicate (`channel entry
state==2`) waits on.

**Working root-cause hypothesis (faithful-core):** the channel-copy SM
(`cd_mc_state_machine`) is dormant because all three drivers are silent —
`bc84c`/`bc848` session callbacks NULL (cleared inside `scene_entry`,
`sm_wrap` never reinstalled), `fc68c=0`, DMA dispatcher idle. Whatever should
re-arm the SM (the next session setup, a DMA-completion edge, or a parked
`session_ctx+0x2C` restore) did not run → pending channel copies never
finalize → decompress source never marked ready → `phase` never `0x700` →
spawn never commits. The faithful fix is in the **`bc84c`/`sm_wrap` /
session-callback lifecycle** (`game/cd.c` `mc_session_b` park/restore
discipline, or the `scene_entry` teardown ordering in `game/game.c`) — NOT
cdrom.c IRQ, NOT OpenBIOS events, NOT game spawn logic.

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
