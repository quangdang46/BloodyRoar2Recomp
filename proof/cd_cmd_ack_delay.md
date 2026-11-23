# CD command ACK delay — faithful `ack_delay` deferral

## Divergence found

`cdrom.c` ran `exec_command()` **synchronously** on the command-byte MMIO
store whenever no response was pending (`irq_flag == 0`). That made the
command's ACK — `irq_flag` (HINTSTS `0x1F801803`) and the response FIFO —
visible to the guest **~0 cycles** after the command was issued.

Only the *INTC* `i_stat` presentation was delayed
(`CDROM_IRQ_PRESENT_DELAY = 5000`); the HINTSTS bit and response bytes were
installed immediately inside `exec_command`.

## Oracle evidence (DuckStation `cdrom.cpp`)

`CDROM::BeginCommand` does **not** execute the command on the FIFO write.
It schedules `command_event` at `GetAckDelayForCommand(command)`:

- `Init` (`0x0A`) → **80000** cycles
- disc present → **25000** cycles
- no disc → **15000** cycles

`ExecuteCommand` (which emits `SendACKAndStat` → `Interrupt::ACK`) runs only
when that event fires. So on hardware-modeled timing the ACK appears
~25000 cycles after issue — the sub-CPU genuinely takes that long to process
a command. The comment at `cdrom.c` ("the controller takes thousands of
cycles … before raising the response IRQ") acknowledged this but only the
INTC leg was delayed, not the HINTSTS/FIFO visibility the game polls
synchronously via `cd_response_process`.

## Why it matters (record-table crash link)

`cd_result_wait_a` (`0x801968CC`) is a bounded poll that exits as soon as a
CD response is visible. With a ~0-cycle ACK, waits released instantly and
the game issued the next command immediately → more in-flight CD responses
→ deeper `cd_result_wait_a → cd_cmd_issue → cd_result_wait_a` recursion on
the main stack (`sp` reached `0x801FF0F0`, inside the `0x801FF080–0x801FF260`
command-record BSS). On real timing each command costs ~25000 cycles, so the
same logical sequence paces slower and nests shallower. The record-table
collision is timing-dependent; this brings command latency to the modeled
hardware value.

## Change

- `QueuedCmd` gains `exec_due`.
- `cmd_ack_delay(cmd)`: `0x0A`→80000, else `has_disc()?25000:15000`.
- `queue_or_exec_command` now **always** snapshots cmd+params into
  `queued_cmd` and sets `exec_due = psx_cycle_count + cmd_ack_delay(cmd)`;
  it never calls `exec_command` synchronously (mirrors `BeginCommand`).
- `try_execute_queued_command` (run every `cdrom_advance`) executes only
  when `irq_flag == 0` **and** `psx_cycle_count >= exec_due`.

A command issued while one is pending replaces it — the same cancel/override
`BeginCommand` applies to an in-flight command.

## Verification (port 9794, fresh build)

`cdrom_command_history` shows every command now waits exactly **25000**
cycles between `queued` and `exec`:

```
seq 828 queued 0x09 @ 6094773729 → seq 829 exec @ 6094798729  (+25000)
seq 826 queued 0x06 @ 6074643145 → seq 827 exec @ 6074668145  (+25000)
seq 824 queued 0x0E @ 6074608615 → seq 825 exec @ 6074633615  (+25000)
seq 822 queued 0x02 @ 6074573491 → seq 823 exec @ 6074598491  (+25000)
seq 820 queued 0x0E @ 6074538678 → seq 821 exec @ 6074563678  (+25000)
seq 818 queued 0x09 @ 6074503749 → seq 819 exec @ 6074528749  (+25000)
```

Game boots, streams CD, and advances through menus normally — commands
complete, just on the hardware schedule. `exec_command` is now reachable
only via `try_execute_queued_command`, so no path bypasses the delay.

## Result — faithful but NOT the crash fix

Driven to `arcade_init` (gs=8) on port 9794, the **same** wild dispatch
recurred at frame 38987: `addr=0x801BCB94 ra=0x801A1EC0 a0=0x801FF170`.
`spdepth` shows the descent is **unchanged** — still reaches `0x801FF0F0`
via the identical `cd_file_load_sm→mc_init_reset→cd_result_wait_a→
cd_cmd_issue→cd_timer_read` chain. The nesting depth is **structural**
(a fixed number of in-flight CD ops per file-load), not ACK-rate-driven,
so slowing command issue did not shallow it.

Record layout confirmed fixed: `rec = 0x801FF080 + slot*240`
(`cd.c:6124`), `cd_bzero(0x801ff080,0x1e0)` — two 240-byte device-command
records the game itself places at `0x801FF080`/`0x801FF170`. `a0=0x801FF170`
is slot 1, not a corrupted pointer.

**Remaining faithful question:** on HW the same generated wait/issue code
runs, so either (a) HW's main stack genuinely reaches `0x801FF0F0` and the
records are consumed/cleared before the deep window (an IRQ-vs-main-stack
scheduling difference), or (b) the deep CD response/callback processing
runs on the IRQ (`0x85xx`) or deferred-coroutine (`0x801BDxxx`) stack on HW
but synchronously nests on the main stack here. The `cd_result_wait_a →
cd_response_process → cd_mc_state_machine → cd_cmd_issue` recursion runs
inside the main-thread poll loop; if HW services those callbacks from the
CD interrupt context instead, the nesting would live on `0x85xx` and never
touch `0x801FFxxx`.
