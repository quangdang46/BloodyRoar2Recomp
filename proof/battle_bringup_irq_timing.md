# Battle bring-up fatal halt — faithful CRTC timing fix + narrowed root cause

Session goal: find the unfaithful IRQ deadline behind the deterministic
~frame-1560–1620 `DISPATCH FATAL` (wild `$ra` = timer-count spill into
`vsync_wait_n`'s saved-ra slot `0x801FFE58`).

## What was fixed (faithful, runtime-only)

Two integer-Hz approximations replaced with the exact rational
CRTC-derived periods the GPU/Timers actually produce (DuckStation
`gpu.cpp`/`timers.cpp` constants: `sysclk_per_crtc = 451584/den`):

1. **VBlank period** (`psxrecomp/runtime/src/interrupts.c`)
   - Was: fixed `vblank_cycles` = 564480 (NTSC) / 677376 (PAL, nominal 50 Hz).
   - Faithful: one VBlank per `horizontal_total*vertical_total` CRTC ticks.
     NTSC 3413×263 → **566203.78** cycles; PAL 3406×314 → **680823.46** cycles.
   - The PAL value ran **~0.5% fast = ~3447 cycles of drift PER FRAME**
     (~5M cycles by frame 1500) against every cycle-exact device deadline.
   - Implemented as a Bresenham `num/den` accumulator
     (`psx_vblank_period_configure`, region-switched in `main.cpp`); the
     per-frame threshold alternates floor/floor+1 exactly like
     `psx_vblank_clock`. `interrupts_cycles_to_vblank` /
     `cycles_to_next_event` / `fire_vblank_edge` all consume the rational
     period; `vblank_cycles` kept as the floor for legacy consumers.

2. **Timer 1 HBlank clock** (`psxrecomp/runtime/src/timers.c`)
   - Was: fixed `timer_advance_divided(t=1, cycles, 2146)` — a uniform
     ~2146-cycle divisor, ~0.3% fast on NTSC and ~1% fast on PAL, AND it
     spread hblank ticks evenly instead of locking them to the CRTC
     scanline grid.
   - Faithful: one tick per scanline = `horizontal_total*451584/den`
     cycles (NTSC 2152.87, PAL 2168.23). Implemented as a u64
     `cycles*den` accumulator (`timers_hblank_period_configure`,
     PAL-configured in `main.cpp`); `timers_cycles_to_irq` computes the
     deliverable deadline in the same rational units (floor = safe
     under-estimate). Mode-write re-arm resets counter but not the
     free-running line phase, matching hardware.

Both are real unfaithfulnesses regardless of the crash outcome.

## Result: crash PERSISTS — these were not the (only) unfaithful deadline

Fresh `tools/crash_hunt.py` on the new build: **4/4 ports crashed** at
frames ~1591–1599 with the identical signature (`pc=0`,
`epc=0x8019A600/0x8019A648`, `RAM[0x801FFE58]=0x0000Exxx` timer spill).
The clobbering write is still `cd_timer_read`'s `sw $v0,16($sp)` at
`0x8019A48C` (frame ~1192), consumed by `vsync_wait_n`'s epilogue
`lw $ra,24($sp)` at `0x8019A660`.

## Decisive negative results (narrows the search)

- **`PSX_EVENT_STEP_CONSERVATIVE=1` (port 9991) crashed identically at
  frame ~1571** — same `0x801FFE58` slot, same wild-value class
  (`0x0000E601`). The per-instruction, exact-cycle event path reproduces
  the bug ⇒ **NOT a fast-path block-batching / deadline-slicing artifact.**
  The unfaithful deadline lives in a device model's `i_stat` raise cycle
  or in IRQ/context semantics shared by both paths.
- irqctx near the conservative crash shows the frequent nested IRQs are
  **`i_stat=0x8` (DMA aggregate, bit 3)** taken inside `vsync_wait_n`
  (`take_pc 0x8019A600/0x8019A648`, `entry_sp 0x801FFDF8`), plus one
  VBlank/frame (`istat=0x1`). The earlier fast-path dump showed `0x4`
  (CD) — both sources deliver inside the wait window.
- `PSX_PRECISE_SLICE=1` (port 9990) still running at frame ~1084 when
  last checked (much slower path; inconclusive but no early crash).

## Oracle status — BLOCKED

`psx-beetleoracle`/DuckStation **cannot reach battle for this title**:
both `-fastboot` and full-BIOS boots park in `cd_channel_poll`
(`0x801993BC/0x80199410`) with the CDROM DMA channel configured
(`madr=0xF1000`, `bcr=0x10200`) but `chcr=0` — the drive never completes
the outstanding command, `i_stat` stays `0x1` (VBlank only), black
screen, `gs=4` constant past frame ~18k. Input (`set_input`, PS1 mask,
pad-1 verified via `get_frame` `pad` field) reaches the controller but
the game never leaves the CD wait. This is a DuckStation CD-emulation
gap for this game's command sequence, unrelated to the recomp runtime.
`find_first_divergence`/IRQ-cycle comparison therefore could not be
produced this session.

## Refined mechanism (unchanged, now better bounded)

`cd_result_wait_a`→`cd_timer_read`→`vsync_wait_n` runs on `KernelSp`
inside the IRQ/deferred-coroutine chain; `vsync_wait_n` busy-waits ~1
frame on `DAT_801CDDE0` with IE re-enabled, so a pending DMA/CD IRQ is
legitimately taken inside it; the nested handler re-enters
`cd_result_wait_a`→`cd_timer_read` at `sp=0x801FFE48` whose spill hits
the outer saved-ra slot `0x801FFE58`. `vsync_wait_n` has **no DI/EI** —
nesting is architecturally faithful *given the delivery cycle*.

## Next steps for the parent agent

1. The remaining unfaithful deadline is most likely **DMA-completion IRQ
   timing** (`dma.c` per-word costs: GPU/CDROM/MDECin=1, MDECout=14,
   SPU=48 cyc/word vs DuckStation `Bus::GetDMARAMTickCount` ≈
   `word_count + ceil(word_count/16)` ≈1.06 cyc/word RAM-side plus
   device-side completion) or **CD IRQ presentation**
   (`CDROM_IRQ_PRESENT_DELAY=5000`, sector/seek deadlines). Neither was
   changed this session.
2. Alternative hypothesis not yet excluded: the deferred CD coroutine
   (`cd_ctx_save`/`cd_ctx_restore` → `cd_chan_full_init` →
   `cd_intc_dispatch` → `cd_result_wait_a`) may be resumed on `KernelSp`
   where real HW would run the CD driver on a kernel-thread stack — a
   context/scheduling faithfulness issue rather than a cycle deadline.
   Decide by tracing whether `cd_result_wait_a`'s IRQ-context activation
   is the game's real IntRP callback path or a runtime-scheduled resume.
3. To get oracle IRQ-cycle data, DuckStation's `cd_channel_poll` hang
   must be fixed first (investigate which CD command it issued — likely
   needs `cdrom` register visibility the debug server lacks, or a
   DuckStation-side patch).
