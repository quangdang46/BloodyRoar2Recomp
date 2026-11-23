# Record-table wild-dispatch crash — FIXED via faithful CD IRQ timing

## Crash (recap)

`FAIL-FAST addr=0x801BCB94 ra=0x801A1EC0 a0=0x801FF170` during battle CD
streaming (~frame 39–46k). Chain: CD IRQ → `cd_intc_dispatch` (IRQ stack
0x85xx) → `cd_cmd_queue_service`/`cd_cmd_queue_drain` →
`dev_cmd_round_dispatch` (0x801A1E84) on `rec=0x801FF080+slot*240` →
`cd_cmd_state_commit` (0x801A3D84) reads `rec[70]` state → jalr `*(rec+0x14)`.

`cd_cmd_state_commit` semantics (game/cd.c:5846): `rec[70]`==0/255 return,
==1/254 rec_init, **any other value** dispatches `fn=*(rec+20)`=`rec+0x14`.
Records are fixed 240-byte BSS slots (`cd_bzero(0x801ff080,0x1e0)`), shared
CD/SIO/memcard. `a0=0x801FF170`=slot1.

Main-thread `cd_file_load_sm→mc_init_reset→cd_result_wait_a→cd_cmd_issue→
cd_timer_read` nests the main stack to `sp=0x801FF0F0` — inside the record
table — spilling session pointers onto `rec+0x14` while `rec[70]` holds a
dispatch-active byte. IRQ-side record dispatch reads them mid-corruption →
wild jalr.

## Root cause — IRQ presentation timing, not stack depth

Stack depth is faithful (generated frames real; `br2_call_generated`
preserves guest sp). On HW the same depth occurs — survival is purely *when*
the IRQ-side record dispatch reads the records relative to the spill window.

DuckStation `cdrom.cpp` diff (read-only subagent) found our flat
`CDROM_IRQ_PRESENT_DELAY=5000` put **every** CD IRQ ~+5000 cycles late vs
DuckStation's model:

| stage | psxrecomp (before) | DuckStation |
|---|---|---|
| INT3/ACK → i_stat | exec + 5000 | exec + 0 (same tick) |
| INT1/INT2/INT4 → i_stat | set_irq + 5000 always | 0 if ≥1000cy since last ack, else +500 |
| post-ack pending IRQ | +5000 | +500 |

The 5000 guard was sized for the OLD synchronous-exec model (present the
response synchronously inside the issuing store → lost to that ISR's
trailing INTC ack → SeekL wedge). That hazard is now moot: `cmd_ack_delay`
(commit faa6a76) defers `exec_command` 25000 cycles past issue, so the
issuing ISR returned long before the response presents — exactly why
DuckStation's `SetInterrupt` asserts the IRQ line the same tick
`ExecuteCommand` runs.

## Fix (commit 206ad35, cdrom.c `set_irq`)

Two-tier presentation, faithful to `QueueDeliverAsyncInterrupt`/
`DeliverAsyncInterrupt`:

- **Sync** (`CDIRQ_ACK`, `CDIRQ_ERROR` raised inside `exec_command`):
  `cdrom_irq_present_due = now` — present immediately.
- **Async** (`CDIRQ_DATA_READY`, `CDIRQ_COMPLETE`, `CDIRQ_DATA_END`):
  deliver now if `now - s_cd_last_interrupt_time >= 1000`, else `+500`.
- `s_cd_last_interrupt_time` stamped on the `0x1F801803.1` ack that drives
  `irq_flag → 0` (matches DS stamping on HINTSTS-clear).

Lost-IRQ protection preserved: the `cdrom_intc_request_latched` re-arm +
`s_irq_i_stat_reason`/`s_irq_i_stat_armed` owed-presentation latch still
guarantee a cleared i_stat re-presents while `irq_flag` holds an unacked
reason — so `due=now` cannot drop the IRQ (verified: no SeekL wedge).

## Verification (runtime, port 9804)

- Boot → title → menu → arcade_init clean; CD streaming healthy.
- `cdrom_state`: `int1_lost=0`, `irq_present_max_late=0`, continuous
  mode-0xA0 (Mode2 Form2) data-sector file-loads across the disc.
- **frame 91136+ with `unknown_dispatch_log total=0` — zero wild dispatches.**
  Prior instances crashed ~39–46k with the `0x801BCB94` wild dispatch.

The +5000 skew was the lever: it held `cd_intc_dispatch`'s record reads
~5000cy later — inside the file-load deep-stack spill window. Faithful
timing moves the record dispatch out of the corruption window.

## Parallel track — clean-dispatch $sp frame model (default OFF)

`gen_clean_table.py::clean_frame` + `CleanDispatchEnt.frame` model each
clean fn's MIPS prologue frame in `clean_try_dispatch` (bail-aware). A/B
showed frames-ON livelocks the CD path — clean ports' `br2_guest_frame`/
`guest_sp` usage is calibrated around frame-less sp. Landed default-OFF
behind `PSX_CLEAN_FRAME=1`; faithful sp needs the ports reworked
stack-aware first. See `proof/clean_sp_frame_model.md`.
