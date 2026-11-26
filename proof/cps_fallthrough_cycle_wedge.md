# CPS transliteration early-boot wedge — root cause + fix

## Symptom

Fresh headless instances parked deterministically before the title:
scratchpad `gs`/`ph` (`0x1F800000` bytes 0/1) stayed `0`, the main thread sat
in a BIOS `TestEvent` wait, and `DeliverEvent` fired only ~44× vs ~1.36M
`TestEvent` spins. VBlank kept ticking so `frame` advanced — masking the wedge.

## What was ruled out

- **Instruction bodies are faithful.** `cps_801a0be0` (`cd_hw_chan_init`)
  matched `generated/SLES_017.22_full_49.c` block-for-block (prologue, arg
  setup, `jal` links, scratchpad stores, fallthrough).
- **Resume contract is correct.** Every yield target is a registered
  continuation in `clean_cps.map`; `clean_try_dispatch` sets `cpu->pc = addr`
  for `CLEAN_ENT_CPS` entries so the body pc-switch picks the right block.
- **`psx_slice_block` is a no-op** unless `g_psx_precise_slice` (parked).
- **Flat cost scaling doesn't help.** `PSX_CPS_COST_MUL=300` still wedged.
- **Values are identical.** `cpu->read_word == psx_read_word`; `psx_cyc_load_*`
  differs only in *timing* side-effects, not load values.

## Root cause (two cooperating defects)

`cps_*` transliterations strip the per-instruction `psx_cyc_step` /
`psx_cyc_load_*` / `psx_cyc_readmem` calls that generated code uses to advance
`psx_cycle_count` and service device deadlines (CD IRQ, DMA, VBlank) *during*
execution. Two distinct losses resulted:

1. **Piece-boundary collapse (all regions).** Split-piece / next-function
   fallthroughs were emitted as bare `cps_X();` **direct C calls**. A chain of
   pieces then ran inside ONE `clean_try_dispatch`, so `e->cost` was charged
   once for the entry piece and `cpu->pc` never updated between pieces —
   per-piece cycle accounting collapsed → IRQ deadlines starved.
   **Fix:** emit `cpu->pc = <piece>; return;` (tail-dispatch) for every
   fallthrough, exactly mirroring generated CPS. 721 sites converted.

2. **Intra-piece timing loss (CD/IRQ/kernel region `0x8018`–`0x801B`).** Even
   with per-piece dispatch, a `cps_` piece runs its whole basic-block body
   with a frozen cycle counter. Functions that poll CD/IRQ/MMIO state
   (`0x1F801xxx`) or wait on BIOS/kernel coroutines need the per-instruction
   `psx_advance_cycles`/`psx_cyc_readmem` deadline servicing that only
   generated code performs — `psx_check_interrupts_at` at block boundaries
   sees a stale counter, so the interrupt that would break the wait never
   fires. **Fix:** these transliterations are not faithful for hardware-
   boundary functions; route them to generated (the §4-faithful fallback).

## The fix (verified: gs cycles 3/5/6, frame 141k+, no park)

- `game/clean_cps.map` — removed the 2976 `cps_*` transliterations in
  `0x80180000`–`0x801C0000` (CD/IRQ/kernel). Kept the 4511 game-logic
  transliterations in `0x80120000`–`0x80180000` **and** the 20 hand-ported
  faithful coroutine bodies (`cd_irq_init_seq`, `cd_response_pump`, `0x8019`).
- `game/gen_cps_clean.py` — `transform()` now emits `cpu->pc = piece; return`
  for every `br2eu_func_*`/`br2eu_psx_alias_body_*` fallthrough (was `cps_X()`).
- `game/cps_clean.c` — regenerated-equivalent: all 721 fallthrough call sites
  are yields.
- `game/gen_clean_table.py` — self-delegate detection now scans **all** bridge
  calls (`br2_call_generated|br2_call_clean|call_guest|call_guest4|…`), not
  just `br2_call_generated`. The 27 CD/IRQ §4 delegates that forward their
  whole body via `call_guest4(<own addr>)` are now correctly skipped → they
  run generated instead of recursing through a wrap.
- `src/clean_table.c` — regenerated: 6003 wrappers, 27 self-delegate skips.

## A/B evidence

| config | result |
|---|---|
| restricted map + NO yield | gs=0 wedge |
| restricted map + yield | **gs cycles, boots** |
| FULL map + yield | gs=0 wedge |

Both changes are required; neither alone is sufficient.

## Caveat / next step

The CD/IRQ region now runs generated — faithful but slower, and it forgoes the
cps_ coverage the mass sweep added there. The *proper* long-term fix is to
give `cps_` transliterations real per-instruction cycle charging (restore
`psx_cyc_step`/`psx_cyc_load_*` or an equivalent per-block `psx_cyc_charge`)
so hardware-boundary functions can be transliterated safely. Until then the
region restriction is the correct conservative boundary: cps_ is valid for
pure game-logic (`0x8012`–`0x8017`) but not for CD/IRQ/kernel (`0x8018`–`0x801B`).
Gameplay (versus/arcade) should be re-verified — boot→title is proven, deeper
CD-heavy paths (asset streaming, FMV) exercise the generated fallback.

## Gameplay verification (post-fix)

- **Versus (P1-only):** reached `gs=8` FIGHT at t=4s and sustained **125+s of
  continuous combat** — `ph` cycling round phases (`0808→0804→0803→0800→0808`),
  `b4b6` advancing battle substates, fighter ctrl blocks (`0x1f8000e8`/`0x1f8001f0`)
  mutating every sample (live positions/anim, state flag toggling). Real fight,
  not a livelock.
- **Arcade (P1-only):** reached `gs=9` fight states and cycled fight phases
  (`0904→0901→0900`) — running, not wedged. (The driver tool's "left fight"
  prints are its versus-centric `gs==8` check misfiring on arcade's `gs==9`.)

Boot wedge fix holds under real CD/asset load — the generated fallback for the
`0x8018`–`0x801B` region is faithful.
