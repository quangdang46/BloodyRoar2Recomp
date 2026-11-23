# Clean-dispatch guest-$sp frame model — experiment + finding

## Goal

Make `clean_try_dispatch` reproduce each clean function's MIPS prologue
stack frame so generated callees see a hardware-faithful `sp`. Motivation:
clean C runs on the host stack and never touches `cpu->gpr[29]`, so every
clean ancestor's frame is missing from the guest `sp` seen by generated
callees — our `sp` runs shallower than HW by the cumulative clean-frame
deficit, relocating every spill's landing address. This was a candidate
cause of the `cd_cmd_state_commit` wild dispatch (`rec+0x14` holding a
session-data pointer) during `cd_file_load_sm`.

## Implementation (landed, default OFF)

- `game/gen_clean_table.py::clean_frame(addr)` decodes the function's
  first MIPS insn from `disc/SLES_017.22`: a leading
  `addiu sp,sp,-N` (`0x27BDxxxx`, `imm>=0x8000`) → frame `0x10000-imm`;
  anything else → 0.
- `CleanDispatchEnt` gained a `frame` field; regenerated
  `src/clean_table.c` (1304 wrappers, 59 skipped — unchanged counts).
- `clean_try_dispatch` applies `cpu->gpr[29] -= e->frame` before `e->fn`,
  restores after, mirrors `guest_sp_set`, and is bail-aware (on
  `g_psx_call_bail` it leaves `$sp` decremented, matching
  `br2_guest_frame_abandon` / generated CPS epilogue-skip).
- Runtime gate: `PSX_CLEAN_FRAME=1` enables; **default OFF**.

## Result — faithful `sp` livelocks the CD path

A/B on identical binaries (only `PSX_CLEAN_FRAME` differs):

| build | reaches | outcome |
|-------|---------|---------|
| frames OFF | `arcade_init` `gs=8` | progresses to the known `rec+0x14` crash window |
| frames ON  | `gs=4` title/attract | **starvation livelock** at `pc=0x80199694` (`cd_mc_state_machine`), `in_exc=1`, BIOS SIO pad-poll spin → `starvation_watchdog` abort (~frame 31k) |

The deeper (faithful) `sp` makes generated callees spill onto addresses the
frame-less `sp` never reached — overlapping live CD command/queue state →
`cd_mc_state_machine` waits forever for a completion that never arrives.

## Why it's OFF by default

The clean ports were **calibrated around frame-less `sp`**: their
`br2_guest_frame(...)` reservations, `guest_sp`-relative accesses, and the
spill placement of generated callees all assume the old shallow `sp`.
Applying prologue frames shifts that entire calibration → misplacement →
livelock. Faithful `sp` therefore requires reworking the clean ports to be
stack-aware (per-function `br2_guest_frame` audit vs the decoded MIPS
prologue), not a dispatch-table tweak. The `frame` field + decode +
bail-aware apply path are kept behind `PSX_CLEAN_FRAME=1` for that work.

## Open

- Does faithful `sp` (once ports are stack-aware) actually prevent the
  `rec+0x14` corruption, or is IRQ-dispatch timing the dominant term?
  Still open — the frame model alone is necessary-but-not-sufficient and
  currently unsafe.
- Per-function audit: which clean functions' `br2_guest_frame` sizes
  duplicate their MIPS prologue frame (double-count) vs allocate genuinely
  additional guest-visible space.
