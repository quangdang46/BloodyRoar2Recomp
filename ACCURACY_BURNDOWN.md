
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
