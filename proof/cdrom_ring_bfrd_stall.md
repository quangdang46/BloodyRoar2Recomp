# CD-ROM ring / BFRD / IRQ-late stall — disproven

Date: 2026-11-22  
Runtime: `./build/BloodyRoar2_Recompiled --disc /Users/tranquangdang21/Projects/bloody-roar-2/Game.cue --debug-port 8934 --headless`  
Disc: SLES-01722 (`Game.cue`)

## Verdict

The reported sustained Mode-2 CD-ROM streaming wedge is **not** a faithful-core defect in `psxrecomp/runtime/src/cdrom.c`. The CD device completed the read, drained the useful Mode-2 payload, fired the completion IRQ, and went idle. The residual freeze is downstream guest/kernel event-thread scheduling, not sector-ring overflow, BFRD lifecycle, or late CD IRQ presentation.

No `cdrom.c`/`dma.c`/IRQ-timing change is warranted from this evidence. Do not patch BFRD clear-on-byte, ring partial accounting, or IRQ deadline counters; they are not the fault.

## What the CD device actually did

`cdrom_command_history` tail (port 8934):

- `seq=2266`, frame `56994`: `Setloc` (`0x02`) params `[0x01,0x58,0x70]` = MSF `1:58:70` (start LBA 8770), then
- `seq=2267`, frame `56994`: `ReadN` (`0x06`) started streaming,
- `seq=2268`, frame `57005`: `Pause` (`0x09`) issued with `pending=1`, `i_stat=0x00000004` (CD IRQ2 pending).

`cdrom_trace_dump` around the same completion:

- `seq=1022729`, frame `57007`: `set_irq val=0x02` (`CDIRQ_COMPLETE`) from `func=0x00002970`, `pc=0x8019AE94`; `pending` cleared, `irq_flag=0x02`.
- `seq=1022730/1022731`, frame `57007`: `fire_irq val=0x02` from `func=0x000029CC/0x00002970`, `i_stat=0x00000004`.
- After that the command history ends: the CD drive is intentionally idle after Pause.

`cdrom_state` after the wedge:

- `seq=1022749` frozen, `last_sector.lba=8801`, `last_sector.size=2340`, `last_sector.mode=0xA0`, `last_sector.frame=57005`.
- `reading=0`, `pending.cmd=0x09`, `pending.active=0`, `pending.phase=1`, `irq_flag=0x00`, `int1_pended=0`.
- `sector_read_pos=2060`, `sector_size=2340`: this is the **expected Mode-2 whole-sector boundary**, not a half-read. The guest consumes a 12-byte header plus 2048-byte payload; the remaining 280 bytes are EDC/ECC tail that the game intentionally does not drain. Therefore `ring_partial` in the tens of thousands is dominated by unread ECC tails, not consumer starvation.
- `ring_starved=0`, `irq_present_max_late=0`, `irq_present_late=30579` (counter only; no late-present maximum).
- `bfrd_set=46915` vs `bfrd_clear=41` is expected lifecycle skew: BFRD is set for sector-buffer availability and cleared as part of sector-buffer recycle, not once per payload byte.

## DMA / event side at the freeze

`dma_state`:

- ch4 (SPU stream channel used by this game's channel layer): `madr=0x0004F6E0`, `bcr=0x0F930010`, `chcr=0x00000201`, `active=0`, `remaining_words=0`.
- ch3 CD-ROM DMA: `chcr=0x00000000`, idle.
- `dpcr=0x3B3BBBBB`, `dicr=0x00900000` = ch4 IRQ-enable (bit20) + master IRQ-enable (bit23); no pending DMA done flags in bits24-30.

Guest channel/event cells (`read_ram 0x801BDDD0`):

- `0x801bde38 = 0xF1000008` — BIOS EvCB handle for slot 8.
- `0x801bde40 = 0`, `0x801bdee4 = 1`, `0x801bdee8 = 0`, `0x801bdeec = 0`, `0x801bdf00 = 0`.
- EvCB slot 8 (`evcb_snapshot`): `class=0xF0000009`, `spec=0x00000020`, `mode=0x00002000` (MARK), `status=0x00002000` (enabled), `fhandler=0`.

`cd_seek_complete_poll` (`0x8019D500`) semantics decoded from generated code: if `0x801bdee4 == 1` it returns without polling TestEvent; if it is not 1 it calls `TestEvent(0xF1000008)` until nonzero. Live value `0x801bdee4=1` means the seek-completion gate is satisfied; the earlier TestEvent storm has stopped.

## Where the freeze actually is

Live scheduler/BIOS state while frames continue:

- `frame` advances (~96k-98k), VBlank IRQs continue, GPU/OTC DMA still cycles.
- `get_registers`: `pc=0x00000000` idle sentinel, `cop0_epc=0x8019A648`, `i_stat=0x00000001` or `0`, `i_mask=0x0000000D`.
- `sp_ring` repeats the kernel scheduler/deferred-resume ping-pong each frame:
  - `0x801FFE18 -> 0x00008548`, target `0x00002924` (kernel)
  - `0x00008548 -> 0x801BDD30`, target `0x8019A87C` (`cd_ctx_restore` resume region)
  - `0x801BDD30 -> 0x801FFE18`, target `0x8019A53C`
- `bioscall_dump` deltas over ~1s samples: B0 index 11 (`TestEvent`) frozen at `2803812`, B0 index 7 (`DeliverEvent`) frozen at `977`, B0 index 10 (`WaitEvent`) frozen at `320`, while B0 index 23 (`ReturnFromException`) climbs (`135613 -> 136774`).

Interpretation: the CD read and its completion handshake completed; the machine is now in a later guest/kernel wait/schedule state (idle pc + per-frame kernel ReturnFromException + deferred CD coroutine resume attempts that find no pending CD work). The missing producer is not the CD-ROM device.

## Why the old hypotheses fail

- **Sector-ring overflow / consumer starvation:** `ring_starved=0`; `sector_read_pos=2060/2340` is the Mode-2 payload boundary; `ring_partial` is ECC-tail residue.
- **BFRD set/clear imbalance:** BFRD is cleared by the sector-buffer lifecycle, not per byte; `bfrd_clear=41` vs `bfrd_set=46915` does not prove a missing clear.
- **Late CD IRQ:** `irq_present_max_late=0`; the completion IRQ was raised (`set_irq 0x02`, `fire_irq 0x02`) at frame 57007 and consumed (`irq_flag` cleared, `pending.active=0`).
- **Pause never completed:** command history shows the Pause completed; `pending.cmd=0x09` is residual command state, not an active command.

## Required follow-up (not CD-ROM)

Investigate the post-CD kernel/event-thread wait separately: which thread/TCB is parked, which EvCB/spec it waits on, and why no runnable game thread remains while VBlank/GPU DMA keep cycling. Relevant handles already decoded: EvCB slot 8 `0xF1000008` = `OpenEvent(0xF0000009,0x20)`; current CD seek gate `0x801bdee4=1`; deferred resume target `0x8019A87C`; scheduler idle `pc=0`.
