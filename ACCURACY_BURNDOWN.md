
## Session 16h — CD stream +4 credit: full mechanism decoded via annotated disasm

Read `psxrecomp/docs/internal/cdmc_80199440.asm` (human-annotated MIPS). Decoded
the complete dir-entry streaming state machine `cd_mc_state_machine` (0x80199440):

**Dispatch (top):**
- `fc69c==1` -> early return (completion pending)
- `fc688!=0 && flag` -> re-arm `fc68c`, return 1
- else `jal 0x80195d44` status; entry = `fc6c0 + fc6a4*0x20` cached to `FEB18`
- `entry->state(+0)==0` -> CLAIM path (0x80199574); `state!=0` -> return 4 "busy"

**Claim path (state==0):** writes entry tag to `+0x1c`, validates, then issues the
first read. `fc6b0==0` -> `cd_chan_cmd_submit(ch3, madr=entry, bcr=8, chcr=t0)`;
`fc6b0!=0` -> `cd_copy_u32`. Then GPU-packet stores + state bookkeeping.

**In-progress path (+4>0, 0x80199b10):** computes double-buffer slot
`fc6bc = fc6c0 + stride*0x20 + cursor*2016` (=0xBE0 for ent0, MATCHES live ch3
madr). Reads `entry+6`(expected) & `entry+4`(progress). If `progress==expected-1`
-> sets `fc69c=1`, issues final read, `fc6a0++`, tail sets `state=3`, `cursor++`,
`jal 0x801990f0` (cd_dir_entry_finalize) when `fc69c`. Else issues `0x1f8` read.

**+4==0 path (0x801998c4):** reads `entry+8`(field8/LBA), compares `fc6b4`;
either skip (`jal 0x80199320`, state->0, callback) or bounds-check consume
(`(fc6c4-fc6a4-1) < entry+6`) -> sets state=1 + callback, OR falls to fc6a8=fc6a4.

**CONFIRMED: `entry+4` is READ but NEVER WRITTEN inside cd_mc_state_machine.**
All writes are to `entry+0` (state). `+4` is credited EXTERNALLY by the
read-completion path.

**Live DMA ground truth (`dma_cdrom_history`, 469 entries):**
- ALL 256 captured transfers are `requested_words=512` (2048-byte bulk sector
  loads), `completed=1`, sequential LBA/addr, frames ~306-436. A large file load.
- ZERO `0x1f8`-byte or `bcr=8` stream transfers EVER reached the DMA engine.
- Live ch3: `madr=0xBE0 bcr=0x1F8 chcr=0` — programmed but never transferred.

**Runtime DMA path verified faithful:** `cd_chan_cmd_submit` writes MADR/BCR,
spins on `*(0x1F801800)&0x40` (=data_fifo_ready = request&BFRD && rb_available),
writes CHCR. `try_execute` needs CHCR bit24 + `channel_enabled(3)`(DPCR bit15).
`execute_ch3_cdrom`->`start_async_cdrom_transfer`. `rb_available()` true live
(pos=0<size=2340), request=0x80 BFRD set -> data_fifo_ready TRUE -> wait passes.

**Narrowed root cause:** the stream-read ch3 submits either (a) never reach
`cd_chan_cmd_submit` because ent0 is already `state=3` (busy -> return 4) and the
claim path only fires on `state==0`, or (b) the submit's FIFO spin or the DMA
kick fails silently. ent0 `state=3,+4=0` means it was claimed+first-read-issued
earlier but `+4` never credited -> the FIRST read's completion is the break.

**Next:** determine whether ent0's first `cd_chan_cmd_submit(ch3,bcr=8)` DMA
actually kicked (check DICR/DPCR bit15 + cdrom_async.active at submit time), and
find the +4-crediting read-done handler (likely the ch3-DMA-done IRQ path or a
paired consumer of the 0xBE0 double-buffer).

## Session 16i — PRIME SUSPECT: stale $t0 -> wrong CHCR in fc6b0==0 claim path

In `cd_mc_state_machine` claim path (state==0 entry), the `fc6b0==0` branch
(0x80199674) calls `cd_chan_cmd_submit(ch3, madr=entry, bcr=8)` and passes CHCR
via `sw $t0,0x10($sp)` at 0x80199688. **$t0 is never initialized on this path** —
`lui $t0,0x1100` only executes in the sibling `fc6b0!=0` branch (0x80199644) and
the `+4>0` path (0x80199b70/0x80199ba4).

Chicken-and-egg hypothesis:
- First stream read (claim path) needs CHCR bit24 set to kick ch3 DMA.
- Valid CHCR (0x11000000) is only produced by the `+4>0` path's `lui $t0`.
- `+4>0` requires a completed read; the first read's DMA never kicks if CHCR is
  stale -> `+4` stays 0 -> `+4>0` never reached -> `t0` never set -> livelock.

On hardware t0 is caller-saved and plausibly already 0x11000000 from prior code;
in the recomp the value depends on coroutine/fiber register preservation across
the state machine's continuation re-entry. If the fiber scheduler does not
preserve $t0 (or the claim path runs before any +4>0 block in the same
activation), CHCR is garbage -> try_execute sees bit24 clear -> no DMA -> +4=0.

VERIFY: dump $t0 at 0x80199688 on a live run (gpr trace / watchpoint), and check
whether cd_chan_cmd_submit's CHCR arg has bit24. If stale, the faithful fix is to
confirm the original binary's t0 provenance (it may be set by jal 0x80195d44's
callee or an earlier block in the same activation) — NOT to hardcode CHCR.

=== RESOLVED: dev_byte_xfer SIO device-ptr mistranslation (commit 99b190d) ===
ROOT CAUSE: clean dev_byte_xfer (0x801a1f74) read the SIO device struct/ctrl
pointers from 0x801be3e4/0x801be3e0 — but the real globals are 0x801be7e4/
0x801be7e0 (-6172/-6176 off the 0x801C0000 base; the port had a 0x400-low
typo, same class as the cd_channel_table_init 0x6000-base bug). be3e0/e4 sit
in an unrelated .data table (arithmetic sequence 0x840c83fd..0x85d985c9), so
*(0x801be3e4)=0x85208510 -> mem_ptr wild deref -> segfault during CD/SIO init.
The real pointers hold MMIO addrs baked into .data: be7e4=0x1F801040 (SIO_DATA
dev struct: [0]=byte buf,+4=status u16,+0xe=cmd u16,+0xa=u16) and
be7e0=0x1F801070 (I_STAT ctrl u32 bit7/bit2). Body also busy-polls hardware
FIFO status in while-loops needing IRQ/DMA-driven updates.
FIX: delegated whole body to generated br2_call_generated(0x801a1f74) —
mem_ptr() routes 0x1F801xxx to the open-bus mirror (guest_at has no MMIO
backing) and a clean-C spin never yields to psx_check_interrupts_at ->
livelock; same contract as cd_timer_read/cd_mc_state_machine/cd_channel_init.
a0 is a host ptr into guest RAM; guest_of() recovers the guest $a0.
VERIFIED: game now reaches frame 1718 (was crashing in dev_byte_xfer ~frame<100).

=== NEW BLOCKER @frame1718: wild dispatch to low-KUSEG 0x27AC ===
symptom: FAIL-FAST unknown dispatch addr=0x27AC ra=0x8019A53C (vsync_wait_n
caller cont), last_func=0x29CC, epc=0x8019A600, frame=1718. Single miss
(unknown_dispatch_log count=1), k0(gpr26)=0x27AC, sp=0x801FFF78, a0=0x60C
a1=0x8000. Region 0x2000-0x2800 is the PSX kernel/low-RAM segment (kernel
fns like OpenEvent/TestEvent land ~0x2500-0x3000); 0x29a0-0x29fc is clean
MIPS (lw struct fields, jr ra @0x29c4, fn entry @0x29cc). 0x27AC itself
holds 0xd8000000 inside a data-table-looking run (repeating 0x??800507 /
0x??e00008) -> likely jumped to a DATA word = bad kernel/interrupt pointer,
OR a recompiler discovery gap for kernel code the static pass never found.
NEXT: identify the jumping insn (jr/jalr through $k0 or a kernel fn-ptr
table) — likely another uninit callback/trampoline in the SIO/memcard or
kernel-call path; check whether 0x27AC is code needing a seed vs corrupt
state. PSX_FAIL_FAST_UNKNOWN_DISPATCH=0 survives-and-logs for enumeration.

=== DIAGNOSED: frame-1718 wild dispatch 0x27AC (kernel-RAM overwrite) ===
MECHANISM (verified): 0x27AC is a REAL dispatch-table key
(OpenBIOS_func_000027AC, kbless kernel window RAM[0x500,0x4BA0) <- ROM
0x1E4D4). The kernel's exception/interrupt RETURN is
OpenBIOS_func_000029CC: it restores the full GPR set from a saved context
block via $k1, loads $k0=*(k1+128) (the saved resume-EPC slot), then
`jr $k0; rfe` -> cpu->pc=$k0. The saved resume PC was 0x27AC.
ROOT CAUSE: live kernel RAM at 0x2700-0x27c0 & 0x281c-0x285c does NOT match
the ROM image (bless->MISMATCH). Region holds a runtime-written DATA
structure (repeating 0x??800507/0x??e00008 pattern; NOT present anywhere in
openbios.bin -> not relocated code). 0x27c4-0x2818 still matches ROM.
So a kernel data object (TCB/event/install table) overwrote relocated kernel
CODE. On return, jr $k0 -> 0x27AC reads 0xd8000000 (op 0x36, unassigned
R3000A) -> dirty-RAM interp can't decode first insn (insns==0) ->
dirty_ram_dispatch returns 0 -> psx_unknown_dispatch FAIL-FAST.
Page 0x2000 IS dirty (bitmap word0=0xffff2fff) so interp WAS reached; the
failure is purely the non-decodable first word = data where code was expected.
NEXT: catch the writer of 0x2700-0x27c0 (gwatch armed) — likely a kernel
object alloc with wrong base/size during CD/SIO/memcard activity. Alternatively
the saved-context $k1 block itself may be corrupt (offset128 read wrong).

=== WRITER IDENTIFIED: heap_serialize writes kernel RAM (frame 1717) ===
PSX_GWATCH on 0x2790+208 caught it: at frame 1717, store_pc=0x8016A8D4
(`sw $v0,0($a1)` = 0xACA20000, plus swl/swr unaligned copies) inside generated
br2eu_func_8016A82C wrote the 0x??800507 data pattern over relocated kernel
code at 0x2797-0x27BF. Caller: game fn @0x8016C594 does `jal 0x8016A82C`
with $a1=$s4 (serialize-object ptr), $a0=$s1, $a2/a3=*(s2+8/12).
0x8016A82C = heap_serialize_tag_01000000 (clean menu.c wrapper ->
br2_call_generated; NOT in clean_table -> faithful generated body runs).
So the wild write is FAITHFUL guest behaviour: $s4 pointed into kernel RAM
[0x500,0x4BA0). On real HW the game works, so $s4 SHOULD point to a normal
buffer -> the pointer is wrong upstream (caller 0x8016C5xx sets $s4, or the
serialize object's dst field is mistranslated/corrupt).
ALTERNATIVE: game legitimately uses a kernel-allocated buffer (alloc_kernel_
memory / kernel-call) and the collision is a kernel-layout difference — but
then real HW would corrupt the same code, so unlikely.
NEXT: trace $s4 (gpr20) provenance in caller fn @~0x8016C5xx; check whether
serialize dst comes from a clean-port field read (guest_at missing?) or a
kernel-alloc return. The 0x??800507 pattern = a table of ptr/offset entries.
