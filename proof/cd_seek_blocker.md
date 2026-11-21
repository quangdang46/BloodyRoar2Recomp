# CD seek blocker — proof report (read-only investigation)

Instance: `./build/BloodyRoar2_Recompiled --disc "disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue" --debug-port 6020 --headless` (wedged).
Reference: port 5900 same build/nav — progressed past the same point (frame 15781, cd seq 160537) ⇒ intermittent, timing-sensitive.

## Verdict up front

The user's suspected mechanism — "the next file's seek never reaches the device
(deferred command-issue / register-window doorbell in
`cd_seek_param_issue`→`cd_reg_window_op`→worker)" — is **disproved for this
wedge**. The command *does* reach the device, executes, completes, and its IRQ
is presented *and* acknowledged. What never happens is the **guest-side
completion event** `DeliverEvent(0xF0000009, 0x20)`, so the delegated
`TestEvent` spin in `cd_seek_complete_poll` never unblocks and the *next*
Setloc is never issued. The break is in the deferred channel-job continuation
(record state machine → `cd_dma_chan_reset` → `DeliverEvent`), not in the
command-issue path.

## Exact stall point (cdrom_trace_dump tail, seq 68163→68207, frames 3690→3692)

```
68163 write 0x1F801801 0x09   pc=0x80196FC4  fr=3690  <- Pause cmd byte via reg window
68164 cmd     val=0x09        pc=0x80196FC4  fr=3690  <- device executes Pause
68165 set_irq val=0x03        pc=0x80196FC4  fr=3690  <- CDIRQ_ACK
68166..68186 guest ACK/status handshake: reads 0x1F801803 (0xE0->0xE3->0xE2),
           index regs; pending_delay ~1.19M cycles counting down, pend=1
68187 set_irq val=0x02        pc=0x8019AE94  fr=3692  <- CDIRQ_COMPLETE;
           pc inside cd_cb8_dispatch (0x8019AE68) — deferred coroutine was running
68188 fire_irq val=0x02       pc=0x80192FA4  fr=3692  i_stat=0x00000004 (CD IRQ bit2)
68189 fire_irq val=0x02       pc=0x8019A9AC  fr=3692  i_stat=0x00000004
68190..68207 guest reads 0x1F801800=0x78 / 0x1F801803=0xE2 (per-frame pump poll)
           then trace total frozen at 68207 — no further CD MMIO ever.
```

Live `cdrom_state` after wedge: `pending.cmd=0x09 active=0 phase=1`,
`irq_flag=0x00`, `i_stat=0x00000000`, `reading=0`, `seek_msf=[1,58,70]`,
`read_msf=[1,59,27]`, `ring_dropped=1`, `bfrd_set=3633`.
`dma_state`: ch3 (CD) `madr=0x001D47C0` (file buffer), `chcr=0`, idle;
`dicr=0x00900000` → done bits (`>>24 & 0x7f`) = 0.
`irq_state`: `i_stat=0`, `i_mask=0xD`, `pending=0`, `IEc=1` — interrupt
controller clean, no stuck CD line.

## Guest-side wedge

- `bioscall_dump`: B0/0x0B `TestEvent` count = **1,462,853** and climbing;
  B0/0x07 `DeliverEvent` total only 109. Tail entries: `a0=0xF1000008`,
  `ra=0x8019AA9C`, `in_exc=1` — the spin is inside the delegated
  `cd_seek_state_machine` trampoline (`0x8019D5B0` = `bios_svc(0xb0,0x0b,...)`),
  reached via `cd_seek_complete_poll` (`0x8019D500`, ra `0x8019D550`) inside
  `cd_file_read_seq` (`0x801565EC`).
- Event object exists: EvCB slot 8, class `0xF0000009`, spec `0x00002000`
  (MARK mode), handle `0xF1000008`, status stuck non-ready — i.e. the event was
  opened/enabled correctly; it is simply never *delivered* again after frame
  ~3692.

## Who should have delivered it — and didn't

`DeliverEvent(0xF0000009, 0x20)` is emitted by **`cd_dma_chan_reset`**:
guest entry **`0x8019B7E0`** (generated `br2eu_func_8019B7E0`, 188 B / 11
blocks, `generated/SLES_017.22_full_46.c:39084`; clean wrapper
`cd_dma_chan_reset` at `game/cd.c:5103`, renames `8019b7e8`). Body: clears CD
channel status via `0x801bdeb0`→`0x1F801C00` blk+0xd5 (`&0xffcf`), polls busy
bits `&0x30`, then — `0x801bdee8 == 0` confirmed live — calls
`cd_bios_svc_07(0xF0000009,0x20)`. Invoked either from `cd_cb7_run`
(`0x8019AF84`, `game/cd.c:5171`) via the DICR-done table cursor
(`0x801BDDF4`, ch-slot handler `0x801B7E0` @ `0x801BDE04`), or from the
per-channel record continuation driven by `cd_cb8_dispatch`.

fntrace (armed before/at wedge on 6020):
- `cd_intc_dispatch` `0x8019A8EC` fires **every frame** (ra `0x8019A88C`,
  a0 `0x801BCD50`, sp `0x801BDD30`) — the deferred coroutine resume machinery
  is alive.
- `cd_cmd_state_rx` `0x801A3E94` (clean C, `game/cd.c:5822`) fires per frame on
  both channel records `0x801FF080` (id `0x265B3`) and `0x801FF170` (id
  `0x265B4`), a1 alternating `0xFFFFFFFE`/`0x801BE7C4` — cycling, never
  reaching the completion transition.
- `cd_cb7_run` `0x8019AF84` and `cd_dma_chan_reset` **never invoked** after the
  wedge (DICR done bits are 0, so the cb7 path legitimately has nothing to do).
- Caveat: fntrace was armed on `0x8019B7E8`; the true entry is `0x8019B7E0`,
  so "never ran" is strong-but-not-conclusive for that one address.

Record state at wedge (both records identical shape): `rec[70]=0x02`,
`rec[55]=0`, `rec[53]=0`, `rec[68]=0`, `rec+0xE8=0`, `fn` slot `rec+0x18=0`
(→ default `0x801a2560`), datptr→`0x801BEFA8`/`0x801BEFCA` containing
`ff 00 ff ff…` (empty response), cmdptr→`0x801FEFF0`/`0x801FF013` containing
`00 5a ff ff…`.

## Classification

**(1)/(2) boundary — leaning (1) clean-C mistranslation in the record
continuation, with (3) runtime CD-status semantics not excluded.** Reasoning:

- NOT a missing command: Pause `0x09` was written to `0x1F801801` at
  `pc=0x80196FC4` (inside generated `br2eu_func_80196E14` = `cd_cmd_issue`
  region) and executed; the next Setloc is gated behind the completion event.
- NOT a stuck IRQ line: `i_stat` returned to 0 after the guest ack sequence;
  INTC state clean.
- The whole IRQ/coroutine path is delegated to faithful generated MIPS
  (`cd_response_process` `0x80196370`, `cd_intc_dispatch` `0x8019A8EC`,
  `cd_cb8_dispatch` `0x8019AE68`, `cd_chan_full_init` `0x8019A814`,
  `cd_seek_complete_poll`/`cd_seek_step` `0x8019D500/0x8019D508`,
  `cd_seek_state_machine` `0x8019D5B0`). `cd_cb8_dispatch` observably ran at
  frame 3692 (the `set_irq` trace pc `0x8019AE94` is its `DAT_801bdde0`
  increment store).
- The non-delegated surface still on the completion path is the **command
  record state machine**: `cd_cmd_state_rx` (`game/cd.c:5822`),
  `cd_cmd_rec_scan` (`:5915`), `cd_cmd_state_commit` (`:5779`),
  `cd_cmd_queue_drain` (`:6047`), `cd_cmd_data_byte`/`cd_cmd_rec_clear`/
  `qbyte_take`. It runs every frame but never observes "op complete", so the
  continuation that reaches `cd_dma_chan_reset`→`DeliverEvent` never fires.
  A single mistranslated state/flag byte there produces exactly this wedge.
- Alternative not excluded: `cd_response_process` deposits response bytes that
  the record machine interprets; if the runtime's CD status/response values
  (the repeated `0x1F801803=0xE2`, index `0x79/0x02/0x59` reads) diverge from
  real hardware, the *faithful* record machine would also stall. The
  intermittency (5900 passed the identical sequence) fits a timing race in
  either layer.

## Recommended next step (diagnosis, not a fix)

On a fresh wedge: arm fntrace on **`0x8019B7E0`** (true entry) and
`0x8019AF84`; sample both records' byte fields (`+0x44..+0x5F`, `+0xE8`,
cmdptr/datptr contents) per frame; and run the same capture on the working
5900 instance at the same CD seq. The first divergent field decides:
record-machine state wrong ⇒ clean-C mistranslation in `game/cd.c:5714-6100`;
response bytes wrong ⇒ runtime `psxrecomp/runtime/src/cdrom.c` status/response
semantics. Fix location then follows: `game/cd.c` record machine vs
`cdrom.c` response/status model — do **not** patch `cd_seek_param_issue`/
`cd_reg_window_op`; they are not on the failing path.

## Evidence index

- Trace tail: `cdrom_trace_dump` seq 68163-68207 (above), `cdrom_state`,
  `dma_state`, `irq_state` outputs captured live on port 6020.
- fntrace ring: `cd_intc_dispatch`/`cd_cmd_state_rx` per-frame entries,
  seq ~60047-60046 window, frames ~141392-141411.
- bioscall histogram: B0 idx 11 (TestEvent) = 1,462,853; B0 idx 7
  (DeliverEvent) = 109.
- Guest map: `game/renames.map` lines for `8019ae68 cd_cb8_dispatch`,
  `8019af84 cd_cb7_run`, `8019b7e8 cd_dma_chan_reset`,
  `801a3e94 cd_cmd_state_rx`, `80196370 cd_response_process`,
  `8019a8ec cd_intc_dispatch`, `80196e14 cd_cmd_issue`,
  `8019d500 cd_seek_complete_poll`, `8019d5b0 cd_seek_state_machine`.
- Clean sources inspected (no edits): `game/cd.c:340-360` (delegation
  rationale), `:5101-5130` (`cd_dma_chan_reset`), `:5171-5215` (`cd_cb7_run`),
  `:4185-4193` (`cd_cb8_dispatch`), `:5714-6100` (record machine),
  `:643-652` (`cd_response_process` delegation), `:5512-5527`
  (`cd_intc_dispatch` delegation).
- Generated (read-only): `generated/SLES_017.22_full_46.c:31795`
  (`br2eu_func_8019AE68`), `:39084` (`br2eu_func_8019B7E0`).

No source files were modified. Scratch only under `/tmp` (`/tmp/cdmon.py`,
`/tmp/nav_*.log`); this report under `proof/`.
