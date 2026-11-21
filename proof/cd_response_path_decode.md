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

## UPDATE — i_stat bit2 IS visible; break is in dispatch decision, not delivery

Live rtrace on fresh :7500 wedge (frame ~9600, cursors prod2/fin0/cons30):
- irqctx_ring: 743 IRQ deliveries with istat=0x04 (CDROM bit2) at
  take_pc=0x8019A600/0x8019A648 -> CD IRQs ARE taken as exceptions.
- rtrace i_stat(0x1F801070) reads: bit2 set+returned 13689x in 6s window.
  cd_intc_dispatch-range readers see bit2 (e.g. 244 hits). => i_stat bit2
  IS guest-visible during CD IRQs.
- i_mask(0x1F801074)=0xD (bit2 set) 5896x; sw-mask u16@0x801bcd48=0xD;
  handler tbl @0x801bcd1c[2]=0x801979F8=cd_response_pump (verified
  renames.map/fnmap.c).
- mask = i_stat & i_mask & 0xD -> bit2 SHOULD be set -> tbl[2]=pump.
- YET pc_probe: cd_response_pump(0x801979F8)=0 hits across FMV+post-FMV.
  cd_intc_dispatch(0x8019A8EC)=7373 hits (v0=1 IRQ-trampoline ctx).
  cd_response_process(0x80196370)=7325 via cd_result_wait_b(0x80196b4c).

CONCLUSION: every operand of cd_intc_dispatch's dispatch mask is correct
and bit2-visible, yet the bit->handler loop never selects i=2->pump.
Remaining hypotheses:
  (a) cd_intc_dispatch reads i_stat/i_mask at a CPS point where the values
      differ from the rtrace-sampled reads (block-attributed PC is fuzzy);
  (b) the bit-iteration/dispatch in generated CPS has a subtlety
      (continuation/ordering) that skips i=2;
  (c) IRQ-timing: the cd_intc_dispatch invocation that sees i_stat bit2
      runs in poll-context AFTER the IRQ handler already acked i_stat bit2
      (the *ptrA=~(1<<i) ack inside cd_intc_dispatch itself, or kernel ack),
      so mask bit2 is transiently present in i_stat reads but the SAME
      invocation's i_mask/sw-mask read catches a cleared state.
NEXT: instrument the actual mask value + per-bit dispatch decision inside
cd_intc_dispatch (needs a targeted watch/probe on the mask computation
block, or runtime IRQ-delivery-vs-ack ordering trace). Runtime agent
6f7d7f6e owns interrupts.c/cdrom.c IRQ timing.

## REFRAME (agent 40dd14de, live :7300) — wedge is recurring phase; break is entry-validation gate fed by bad/clobbered CD data

The "wedge" ALTERNATES stall<->healthy (prod=fin=cons advancing e.g. 6/5/5).
Not a hard deadlock — each new segment re-enters the broken path.

REAL mechanism (all live-measured):
- cd_mc_state_machine(0x80199440) issues next read ONLY when
  u16[entry+6] == u16[entry+4]+1  (block 0x80199BB8-0x80199BD4).
  Wait path 0x80199C78=97 hits, issue path 0x80199BD8=18 hits — gate rarely passes.
- finalize (0x80199D44 jal 0x801990F0) gated on *(u32)0x801FC69C!=0; that flag
  reads 0 throughout stalls -> cd_dir_entry_finalize never runs -> no state=2.
- Walking entry ptr *(0x801FEB18) pinned at entry1 (0x80010020); its +4/+6
  hold DISC PAYLOAD (0x1800/0x0421, 0xffff/0xff40...) not stable counters.

ROOT: the 0x20B channel entries are being OVERWRITTEN by CD DMA — two writers
to 0x80010000-0x800103FF:
  pc 0x80199694 (cd_mc_state_machine via cd_chan_cmd_submit ch=3): structured
    8-word descriptors, LBA ~208829+, mode 0xE0, wrapping all 32 slots.
  pc 0x801978AC (cd_cmd_issue_b kick, ra=0x801986EC cd_read_step_cb): raw
    sector payload FFFF0000/0000FF40/FFFFFFFF into SAME slots.
So u16@entry+0 "state" and +4/+6 are disc bytes, rewritten every DMA pass —
state==2 only in a narrow window AND finalize can't run (69c stuck 0).

The +4/+6 gate fails because reads deliver WRONG/STALE sectors (record-content
mismatch) — consistent with AGENTS.md §8 CD command-sequencing blocker
(cd_seek_param_issue->cd_reg_window_op->worker, deferred command issue).
Two live streams read very different regions: ~208829+ mode 0xE0 vs
~280/8535/9268/9534 mode 0xA0.

RECONCILE with pump/i_stat findings: cd_response_pump=0 and the i_stat-bit2
dispatch gap are likely DOWNSTREAM symptoms — if command completion/data is
misclassified or sectors are wrong, the entry-validation never passes. The
PRIMARY break is CD read delivering incorrect data / clobbering the table.
NEXT: verify what LBA/mode/size the guest requests vs what runtime delivers
for the streaming reads (cdrom command_history vs guest intent); check the
DMA destination address for the sector writes hitting 0x80010000.

## RULED OUT (clean-C layer + sector format are correct)
- Entire streaming read path is DELEGATED to generated (faithful):
  cd_read_open(0x80198908), cd_read_step_cb(0x801985c4) [MSF-stamp verify],
  cd_read_finish_cb(0x80198838), cd_cmd_issue(0x80196e14),
  cd_cmd_issue_b/c(0x80197800/0x80197900), cd_read_setup(0x80198c90).
  => NOT a clean-port bug.
- cd_cmd_issue_async (clean cd.c:419) uses 0x801BC854&0x10 busy gate correctly.
- mode_reg set by Setmode(0x0E)->param_fifo[0]=0xA0 (cdrom.c:2348); 0xA0&0x20
  => whole-sector path.
- MSF stamp REGENERATED from read position via bin_to_bcd for whole-sector
  (cdrom.c:1551-1553) — already the faithful fix for corrupt stored MSF.
- DMA ch3 targets CD-driver buffers (0x801BDCE8/0x5D434), not obviously wrong.

=> Remaining: runtime TIMING (IRQ delivery vs guest ack/dispatch ordering) or
   sector DATA CONTENT correctness (wrong LBA delivered). Agents:
   6f7d7f6e=interrupts.c/cdrom.c IRQ timing; ef8202ef=CD read data correctness.

## ROOT CAUSE CONFIRMED (wtrace, live :7500 frame 147705)
CD DMA channel 3 writes SECTOR PAYLOAD into the channel-entry table.
Filtered wtrace on guest 0x800103C0-0x800103DC (masked 0x103C0-0x103FF):
  dma_ch:3, ra=0x801986EC, pc=0x801978AC wrote:
    0x800103C0 old=0x80010001 new=0xFFFF0000
    0x800103C4 old=0x00050000 new=0x0000FF00
    0x800103C8 old=0x00002711 new=0xFFFFFFFF
    ... (all 8 words of the awaited entry overwritten with sector/pixel data)
The awaited consumer entry (cursor=30 -> 0x800103C0) is clobbered by CD DMA,
so u16[+4]/u16[+6] hold disc bytes, never satisfying the gate.
=> CD read DMA destination (ch3 MADR) points INTO the descriptor table.
   Either dest addr derived from already-corrupt entry field, OR runtime
   DMA address translation off. NEXT: find where ch3 MADR is programmed
   for a read and what value it gets.

## REFRAME #2 — table writes are INTENTIONAL paired CD DMA (not clobber)
dma_cdrom_history shows each stream LBA does TWO ch3 transfers:
  - 8-word (0x20B) DESCRIPTOR write -> 0x10060+idx*0x20  (pc=0x80199694, mode 0xE0)
    first_word=0x80010160 (a pointer). THIS IS the channel entry being filled
    BY CD DMA — entries are DISC-DATA descriptors, not CPU-built.
  - 504-word (~0x7E0) DATA write -> 0x11BA0+idx*0x7e0 (pc=0x80199CEC/0x80199C50)
    = base+capacity*0x20+idx*0x7e0 data buffer (matches cd_channel_poll formula).
Entry idx <-> data idx correspond (entry3<->0x11BA0, entry4<->0x12380, entry5<->0x12B60).
LBAs consecutive (209047/8/9). mode=0xE0 (streaming/FMV), vs 0xA0 file-load.

=> +4/+6 are SEQUENCE TAGS stored in the disc stream; gate u16[+4]==u16[+6]-1
   validates consecutive delivery. cd_read_step_cb does NOT write them.
=> WEDGE = a sector SEQUENCE BREAK: a skipped/duplicated/out-of-order/wrong
   sector makes the disc sequence non-consecutive -> gate never passes.
   This is a CD DATA-CORRECTNESS bug (ef8202ef's domain), NOT a table clobber.
   pump=0 is likely pc_probe FALSE-NEGATIVE (clean-ported host C) + moot
   because cd_result_wait_b does the same callback dispatch synchronously.

## ROOT CAUSE — COMPLETE CAUSAL CHAIN (live-proven, agent ef8202ef + parent)
Sector data is BYTE-EXACT vs Game.bin (right LBA/mode/sector slices). The wedge
is a GUEST-MEMORY COLLISION:

  tile_stream_open (menu.c:2244) -> cd_dir_table_set(_DAT_801f4754=0x80010000,0x20)
    => channel-entry table lives at guest 0x80010000 (32 desc x 0x20 + data ring).
  File loads ALSO DMA payload into 0x80010000:
    cd_file_load_sm(file_id,dest) -> cd_read_issue(0x8018d150) ->
    cd_read_setup(0x80198c90) -> DAT_801bcb84=dest=0x80010000 -> ch3 DMA.
  CAUGHT LIVE (wtrace :7500 f311728): bcb84 0x800F0000->0x80010000,
    pc=0x80198D98, ra=0x8018D198(cd_read_issue), func=0x29CC, a1=0x80010000.
  b88 cursor swept ~168KB (0x80010000->0x8003A800) overwriting ALL 32 desc slots.
  File payload in entry+4/+6 => cd_mc_state_machine gate fails => wedge.

=> 0x80010000 is a SHARED CD scratch/stream arena (guest-authentic reuse).
   Safe on real HW ONLY if the tile stream fully drains/is torn down before
   the file load reuses the arena. The overlap => a stream-completion /
   teardown event is MISSED => deferred-IRQ/TestEvent timing bug.
   FIX LIVES in the IRQ/completion path (agent 6f7d7f6e), NOT the data path.
   Descriptor format decoded: +4=u16 pos-in-group(0-4), +6=u16 group-size(5);
   gate fires on 5th sector of each 5-sector group.
