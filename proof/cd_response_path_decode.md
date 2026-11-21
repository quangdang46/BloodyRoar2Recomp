# CD response-path decode — where streaming completion breaks

Source: explore agent 4811f2fb (generated-code trace) + live probes :7300/:7400.

## Deferred-coroutine contract (CORRECTED — not a stuck predicate)
`cd_chan_full_init`(0x8019A814) -> `cd_ctx_save`(0x8019AD90) snapshots regs
into frame @0x801BCD50 (JB_PC=0x8019A87C). On EVERY PS1 IRQ the BIOS
ExceptionHandler restores that jump buffer, sets v0=1, jumps to 0x8019A87C:
  beq v0,zero -> 0x8019A88C (ready path, runs ONCE on first sync pass)
  else -> 0x8019A884: jal cd_intc_dispatch -> B0/0x17 ReturnFromException
So 0x8019A87C/~2315 hits and 0x8019A88C=0 are NORMAL steady-state.
cd_ctx_restore(0x8019ADCC)=0 is expected (only on first yield). The
HookEntryInt->EntryInt->ExceptionHandler->IntRP contract WORKS
(ACCURACY_BURNDOWN.md:1643 already proved ~36k dispatches ra=0x8019A88C).

## cd_response_process (0x80196370) — synchronous classifier
Reads: I_STAT idx via cell->0x1F801800; response bytes via ->0x1F801801
(up to 8 onto stack); IRQ reason via ->0x1F801803 (&7, stability-waited);
ACKs CD controller via write 0x1F801803 idx1 (irq_flag &= ~7).
Writes: status -> 0x801BC854 (=response[0]&0x1D = cd_status_raw),
param -> 0x801BC858, class byte -> 0x801BCB24 ((s1==0)?2:5).
Game gates command-completion on *(u32)0x801BC854 & 0x10 (bit4 busy).
Called 1127x via cd_result_wait_b->0x80196B4C (DIRECT path, NOT the pump).
Live :7300/:7400: 0x801BC854=0x02 (bit4 clear=ready), 0x801BCB24=0x02.

## cd_response_pump (0x801979F8) — IRQ-context completion dispatcher
handler[2] in cd_intc_dispatch table @0x801bcd1c (11 entries:
[0]=0x8019AE68 cb8_dispatch, [2]=0x801979F8 pump, [3]=0x8019AF7C).
cd_intc_dispatch mask = *I_STAT(0x1F801070) & *I_MASK(0x1F801074) & u16@0x801bcd48(0xD).
Per set bit i: writes *ptrA=~(1<<i) [acks i_stat] then jalr tbl[i].
Pump loops cd_response_process; nonzero return dispatches callbacks:
  s0&4 -> jalr *(0x801BC84C), a0=lbu(0x801BCB25), a1=0x801FC6D0
  s0&2 -> jalr *(0x801BC848), a0=lbu(0x801BCB24), a1=0x801FC6C8
These callbacks mark channel entries state=2 and set event flags.
LIVE: cd_response_pump = 0 hits (FMV + post-FMV). => i_stat bit2 never set
in cd_intc_dispatch's mask when it runs.

## IRQ delivery IS healthy
cd_irq_delivered(g_cdrom_deliver_count)=37006; irqctx_ring captures
istat=0x04 (CDROM bit2) at take_pc=0x8019A648/0x8019A600.
psx_irq_raise(2)->i_stat|=(1<<2), gated by cdrom_irq_mask_matches_reason
(irq_enable&reason&7; irq_enable=0x07 matches CD reasons).
cdrom_intc_request_latched keeps bit2 set until guest ack.
command_history shows i_stat=0x04 transiently on command completion.

## THE BREAK (precise)
CD IRQ sets i_stat bit2 at delivery, but cd_intc_dispatch never observes
bit2 set -> handler[2]=cd_response_pump never runs -> completion callbacks
(0x801BC848/0x801BC84C) never fire -> channel entries never state=2 ->
cd_channel_poll starves -> MDEC freezes ~87%, cursors frozen prod2/fin0/
cons30 (HARD wedge, 48s+ frozen), guest spins B0/0x11 TestEvent=2.05M calls,
CD device idle (reading=0, pending.active=0).

OPEN: is i_stat bit2 cleared before cd_intc_dispatch reads it (ack ordering
in guest INTC dispatch / recompiled BIOS), or does cd_intc_dispatch not run
in CD-IRQ context? Next probe: rtrace_dump on 0x1F801070 reads (PC+value)
during a FRESH FMV wedge to see i_stat bit2 at cd_intc_dispatch read-time.
Runtime lever if timing: CDROM_IRQ_PRESENT_DELAY / ack ordering in
cdrom.c+interrupts.c (agent 6f7d7f6e scope).
