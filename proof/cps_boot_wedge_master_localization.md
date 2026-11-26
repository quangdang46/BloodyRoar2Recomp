# cps_ boot-wedge — master-state localization (session N+1)

## Confirmed wedge signature (current master `3433c49`)

Deterministic on every fresh instance, reproduced many times:

- scratchpad `0x1F800000` stays **all zeros** (`gs`/`ph` never written).
- Main thread parked in BIOS/kernel idle: `ra=0xBFC05E00`, `sp=0x801FFD68`,
  `s0=0x00008FE0` (the documented CD deferred-coroutine TCB), frozen across
  samples; `cop0_cause=0x400` IRQ-pending, `epc` stuck at a CD/game addr.
- `bioscall_dump`: ~1.36M historical `TestEvent` (B0/0x11) spins — the
  deferred-IRQ/event starvation class.
- `card_mgr_trace`/`dispatch_tail`: ~2.78M memcard-manager calls from BIOS
  `0x1FC091E0`/`0x1FC0957C`/`0x1FC08360`, tail ends `0x1FC05E00` — the CPU
  livelocks in the BIOS memcard path waiting for a card/IRQ that never lands.
- VBlank frames keep advancing (BIOS ticks regardless) — "frames increase" is
  NOT a health signal; scratchpad-nonzero is.

## A/B isolation (the durable result)

| config | scratchpad | verdict |
|---|---|---|
| default (named clean + all `cps_`) | zeros | **WEDGED** |
| `PSX_NO_CLEAN=1` (all generated) | nonzero `gs=6` | runs |
| `PSX_NO_CPS=1` (named clean, `cps_`→generated) | `gs=5` TITLE | **runs** |

=> The wedge lives in the **`cps_` transliterations** (`game/cps_clean.c`,
`gen_cps_clean.py`), not named clean ports and not the runtime.

## Commit bisect (ground truth)

- `c0c611e` (pre-mass-sweep): **boots** (`gs=6`, frame 54k).
- `e00e5c6`: boots (`gs=3`). `5edaca1`: **wedges**. => first-bad = `5edaca1`
  ("Port 7 CD IRQ-wait delegates to clean CPS").
- Function-level bisect at `5edaca1` (its 8 new fns are self-contained):
  removing only `cd_chan_cmd_pump` (`cps_801a0660`) entries → **boots**.
  => `cps_801a0660` was a culprit *at that commit*.

## Master-state narrowing — and its limitation

`PSX_CPS_KEEP=lo-hi` (runtime, no rebuild) routes `cps_` entries outside the
ranges to generated. Bisect results on master:

- keep `0x80180000-0x801BFFFF` (CD/kernel region) only → **runs** (game code
  executing, `pc=0x80126260`, `epc=0x80196E14`).
- keep `0x80120000-0x80130000` only → **wedges**.
- keep `0x80120000-0x80124000` only → **runs** (`gs=6`).
- keep `0x80124000-0x80126000` only → **runs**.
- keep `0x80126000-0x80126800`/`0x80126800-0x80127000` → `0x80126800+` runs,
  `0x80126000-0x80126800` wedges.
- keep `0x80126000-0x801261c0` (tiny memset cluster) → **wedges**.

**⚠ LIMITATION — fallthrough contamination.** `cps_X()` split-piece fallthrough
is a *direct C call* that bypasses `clean_try_dispatch`, so "keeping a range"
does NOT cleanly isolate: a kept low-address function still pulls every
downstream `cps_` sibling via `cps_Y(); return;`. The narrowing therefore only
proves "the bug is reachable downstream of `0x80126178`" — i.e. somewhere in
the init/memset fallthrough chain, NOT specifically inside
`0x80126000-0x801261c0`. Treat the region results as a *reachability* bound,
not a culprit list.

## Last-executed evidence

`clean_trace` (last clean dispatches before park): only
`0x80126178` and `0x80126188`, both `ra=0xBFC06694` — the BIOS-called memset
/ BSS-clear at the head of the `0x80126178…` fallthrough chain. After it, no
more clean dispatches: the game went into BIOS/kernel and parked. The buggy
`cps_` function therefore runs **inside the untracked fallthrough chain**
(direct `cps_Y()` calls are not traced) during early init — consistent with a
missed IRQ/event registration that leaves the memcard/CD wait starving.

## Ruled out

- **Instruction bodies**: `cd_chan_cmd_pump` (63 instrs) verified
  instruction-identical to generated (branches, jal targets, delay slots,
  ALU, lw/sw). The memset cluster `0x80126178/88/94/98/9c` verified faithful
  incl. GPR dest regs (`gpr[1]=at` matches).
- **Resume PCs**: all `cd_chan_cmd_pump` continuation addrs are in
  `clean_cps.map`.
- **Yield structure**: 0 yield-target mismatches across all ~1283 cps_ fns.
- **Flat cycle under-charge**: `PSX_CPS_COST_MUL=300` (3× per-dispatch cost)
  does NOT fix → not a simple total-cycle deficit. (The per-instruction
  `psx_cyc_*` load-wait/interlock *state* is still stripped — timing-position,
  not count, may still matter, but scaling didn't rescue it.)
- **`br2_lwl/lwr/swl/swr`**: byte-identical to `psx_*` (cycle accounting
  dropped only).

## Open hypotheses (ranked)

1. A specific `cps_` function in the `0x80126178…` init fallthrough chain
   mistransliterates a value/ordering that corrupts IRQ or event-flag state
   (the park is a missed DeliverEvent, not a CPU fault).
2. Coroutine yield/resume across a *named-clean → cps_* boundary*: a named
   clean port calls into a cps_ coroutine (or vice-versa) and the
   `cpu->pc`/`gpr[31]` hand-off drops the continuation.
3. `psx_cyc_*` stripping loses load-delay/interlock *state* that a
   timing-critical init loop depends on (cost-scaling didn't help, but the
   inline-state version is untested — needs transformer regen).

## Next steps

- **Fallthrough-aware isolation**: to bisect without contamination, either
  (a) regenerate `cps_clean.c` routing split-piece fallthrough through
  `clean_try_dispatch` instead of direct `cps_Y()` calls, or (b) physically
  delete candidate `clean_cps.map` addr ranges + `gen_clean_table.py` +
  rebuild per candidate (slow but exact).
- **Trace the buggy function**: arm `fntrace`/`pc_probe`/`fn_entry` early and
  dump the last *game* PC before the BIOS park; or enlarge `clean_trace`
  (currently only catches dispatch entries, misses direct fallthroughs).
- **Prime suspect region**: the coroutine functions with `_cont` resume
  switches reachable from the `0x80126178` memset chain in `0x8012`/`0x8013`
  (init/BSS-clear path), plus `memcard_*`/`cd_*` IRQ-bound functions.
