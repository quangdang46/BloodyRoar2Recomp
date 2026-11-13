# PSX Accuracy Burndown (living doc)

Companion to FAITHFUL_TIMING_PLAN.md. That doc owns the CYCLE/TIMING axis (the
area under active implementation); THIS doc is the full-coverage burndown across
ALL accuracy axes for the faithful-core build.

## ⚠️ LESSON (2026-06-26): oracle-validate OUTPUT before applying a fix

The MDEC "fix" (agent branch `fix/mdec-faithful-accuracy`) was applied from
research-claimed source-level divergences and REGRESSED both Tomba 2 FMVs
(Whoopee Camp logo + Tomba intro) on color — REVERTED. The original MDEC output
was user-verified CORRECT. Takeaways, now binding:
- A research-claimed discrepancy is a HYPOTHESIS, not a bug. Validate the OUTPUT
  (diff our decoded result vs Beetle's on the SAME input) BEFORE changing code.
- "Matches Beetle's source conventions" does NOT justify rewriting code whose
  OUTPUT is already correct. Our impl may be a valid equivalent.
- The user's eyes / the oracle's OUTPUT override source-reading. "Looked right
  before, wrong after" = revert, always.
- MDEC fix is PARKED on its branch pending a real MDEC output-diff harness (decode
  the same stream on native + Beetle, compare blocks). Only fix proven output
  divergence. Do NOT re-apply blind.
- Same gate applies to ALL agent fixes (hybrid-pad etc.): reconcile onto wt/tomba2
  + validate vs the oracle BEFORE trusting.

Separately: the Tomba intro FMV left/right SPLIT seam is a PRE-EXISTING GPU/display
presentation bug (present before today's work), not MDEC — its own axis-5 item.

## Method (non-negotiable)

- Every item gets: **status**, the **external comparative(s)** to cross-reference
  it against, and a **validation method**. "Looks good" is NOT a status — an item
  is only GREEN once cross-referenced against a reference (psx-spx / Beetle source
  / DuckStation / a hardware test ROM) AND validated against the oracle at runtime.
  Self-agreement (compiled == our interp) proves backend-equivalence, NOT
  correctness — both can be identically wrong (CLAUDE.md §15).
- Don't do it all in one pass. Tomba 2 is the **stomping ground**: validate
  everything we can here, then validate the rest against the other games
  (Tomba 1, MMX6, Ape, BIOS), then merge wt/tomba2 → master, then keep a
  **post-merge burndown** for whatever remains.
- Governed by CLAUDE.md Rule -1 (faithful core, no hacks, breaking other titles OK).

## Comparative sources (the reference shelf)

- **nocash psx-spx** — the canonical hardware reference. Cite the section per item.
- **Beetle/Mednafen PSX** (IN-TREE: `psxrecomp/beetle-psx/mednafen/psx/`) — our
  oracle's own source: cpu.cpp, gte.cpp, gpu.cpp, spu.cpp, cdrom.cpp, dma.cpp,
  timer.cpp, frontio.cpp/`input/`, mdec.cpp, sio.cpp, spu/reverb. Read for the
  exact model; run as the oracle on port 4382 for runtime diff.
- **DuckStation** source — clean, heavily-commented; excellent cross-check for GTE
  cycle table, GPU rasterization rules, CD timing (use as a 2nd opinion vs Beetle).
- **Hardware test ROMs** (ground truth above emulators): Amidog CPU & GTE tests,
  PeterLemon/PSX, the psx-spx test suite, GPU/timing test ROMs. Running these on
  native vs Beetle is the strongest single validation we can add.
- **Real HW via Beetle oracle** (port 4382) — first-divergence on the relevant
  state surface (VRAM for GPU, audio samples for SPU, cycle counts for timing).

## Validation infrastructure to BUILD (prerequisite tooling)

- [ ] **Native↔Beetle cycle/first-divergence comparator** (replaces stale
  DuckStation-era find_divergence.py port 4371). Needs additive guest-cycle
  exposure in beetle_debug_server.c. The backbone measure for axes 2/3.
- [ ] **State-surface diffs**: VRAM byte diff (GPU), SPU sample-stream diff
  (audio), CD sector/response diff. Per-axis oracle comparators.
- [ ] **Hardware-test-ROM harness**: run Amidog/GTE/CPU test ROMs on native and
  Beetle, diff pass/fail + result registers. (Highest-leverage axis-1/2 validator.)

---

## Axis 1 — Instruction semantics (decoder)

Status: STRONG (recompiler core; proven byte-identical to interp on many funcs).
- [ ] ALU/shift/logical/sign-extension — cross-ref Amidog CPU test ROM.
- [ ] LWL/LWR/SWL/SWR unaligned — psx-spx "CPU Load/Store"; Amidog.
- [ ] MULT/MULTU/DIV/DIVU → HI/LO, div-by-zero & overflow results — psx-spx; Amidog.
- [ ] Overflow-trapping ADD/ADDI/SUB (vs ADDU/etc.) — do we trap? psx-spx "Exceptions".
- [ ] **Load-delay slot hazard** — KNOWN SIMPLIFICATION: interp loads land
  immediately (no 1-instruction delay). Cross-ref psx-spx "CPU pipeline"; Beetle
  LDAbsorb. Decide: model it or document why safe.
- [ ] Branch-delay slot (incl. branch-likely absence on R3000) — done; spot-check.
- [ ] COP0 (mtc0/mfc0/rfe, Status/Cause/EPC/BadVaddr) — psx-spx "COP0".
- [ ] GTE/COP2 math: fixed-point, saturation, FLAG register, all 30+ ops —
  cross-ref Amidog **GTE test ROM** (the definitive validator) + DuckStation gte.cpp.

## Axis 2 — Cycle/timing  ← ACTIVE (see FAITHFUL_TIMING_PLAN.md)

Status: Stage-1 (1 cycle/insn, single-source seam in place); Stage-2 in progress.
Oracle model fully transcribed in `CYCLE_MODEL_BEETLE.md`. Game-independent
BIOS-kernel ruler BUILT (region [0x80001C5C→0x80001CA4]); per-block-leader cycle
observe added to the recompiler so ANY block leader is anchorable on both backends.
- [x] Single-source `psx_instr_base_cycles` seam (identity), both backends.
- [x] cyc_watch dispatcher+prologue double-fire dedupe (was corrupting native Δ).
- [x] Per-block-leader observe (native now samples like Beetle, not only fn-entry).
- [x] **Load double-count FIXED**: Stage-2 #1a put +2 data-access in
  psx_instr_base_cycles while memory.c already charged +6/main-RAM-load → counted
  twice. Reverted opcode fn to pure execute base; memory.c is the single address-
  keyed owner. Ruler [c5c→ca4]: native 34→30 (exact), no FMV regression.
- [x] **Memory wait-state — DONE (full R3000A load model).** load=4 LANDED (the boot
  wedge was a real device-timing bug the accurate load exposed — pad ACK→IRQ7 made
  guest-cycle-paced, see axis 5; commit d8c4a8e). ReadFudge + LDAbsorb give-back state
  machine SHIPPED across both emitters + interp (psx_cyc.h §1/deps/DO_LDS; commits
  d8c4a8e/fade560/d597797) — load2 10→11. **Device-region MMIO read waits DONE** (commit
  9ae534d, branch wt/tomba2-mmio-waits): Beetle MemRW table in psx_cyc_readmem, size-aware
  (SPU +36/+16, CDC +6×size, GPU/MDEC/SysCtrl/FIO/SIO/IRQ/DMA/Timers +1, RAM +3, else +0).
  Ruler #2: all 15 loops (incl. mmio_timer +3, mmio_spu +38) == Beetle EXACT.
  RESIDUAL: DMACycleSteal (dynamic per-read DMA bus-steal, libretro.cpp:868) unmodeled.
- [x] **Mult/div completion-stall — DONE, validated EXACT in BOTH emitters**
  (commits a3e8f28 game, 180b821 BIOS). CPUState.muldiv_ts_done set by
  MULT/MULTU/DIV/DIVU, MFLO/MFHI stall to it (psx_cycles.c). Per-instruction
  charging default on this branch (both emitters). Ruler #2 (game): div +38,
  div_spaced +38 (absorb), mult +15 — ALL == Beetle. Ruler #1 (BIOS kernel):
  [c5c→ca4] native 30→56 == Beetle 56, STEADY DELTA 0 EXACT. FMV no regression.
- [x] **Instruction-fetch / I-cache — DONE both backends.** Stage 1 (interp, commit
  958a928) + Stage 2 (BOTH static emitters at cache-line leaders via the runtime PC;
  relocate_ra for BIOS shell/kernel; default-on; commit 0edb935). Faithful direct-mapped
  4 KB/256-line model (psx_icache.c, from Beetle ReadInstruction: +0 hit / +4 KSEG1 /
  +3+refill miss). Ruler #2 icache_miss == Beetle; ruler #1 [c5c→ca4] steady 56==Beetle
  AND native now produces the cold-refill spikes (77/84) that were absent. FMV no-reg.
- [x] **GTE per-command cycles — DONE, validated EXACT** (commit ec1fd76).
  CPUState.gte_ts_done armed in gte_execute (cost-1 table verified from beetle
  gte.cpp op returns; AVSZ4=5 not 6); any COP2 reg access stalls to it; both
  emitters + interp. Ruler #2 gte_rtps +15 / gte_nclip +8 == Beetle. FMV no-reg.
- [x] **Mult/div stall — also completed to the dirty-RAM interpreter** (commit
  75d5d1a): backend parity (interp was charging 0 for mult/div). By-construction
  + no-regression (no interp-path ruler yet — see below).
- [x] Instruction-fetch / I-cache timing — DONE (see above; commits 958a928 + 0edb935).
  The ruler's 56→84 cold spread (I-cache line-refill transient) now reproduced natively.
- [ ] **HW test-ROM ruler (#2)** — Amidog CPU/GTE timing ROMs for hand-crafted
  single-COMPONENT isolation (div-only, load-only loops) that organic BIOS code
  can't give (the prologue combines div+loads in one block). Strongest validator.
- [ ] Validation: native cumulative cycles == Beetle == analytic on the rulers.

## Axis 3 — Interrupt / event timing

Status: PARTIAL.
- [ ] Device IRQ-raise timing (VBLANK scanline, timer overflow/target, DMA/CD
  completion) — tied to axis 2/5; psx-spx per device.
- [ ] **IRQ take-point** (HW = exact instruction; us = block edge) — the parked
  precise-slicing (PRECISE_IRQ_SLICE.md). Validate vs Beetle exc_ring.
- [ ] Exception entry record (EPC/Cause.ExcCode/BD/Status-stack) — currently uses
  a sentinel EPC; cross-ref psx-spx "Exceptions"; Beetle. Validate exc_ring match.

## Axis 4 — Memory map / MMIO

Status: MODERATE-STRONG (regions games use).
- [ ] KUSEG/KSEG0/KSEG1 mirroring, scratchpad, cache-isolation (IsC) — psx-spx.
- [ ] I/O register semantics: read-to-clear, write-1-ack (I_STAT), masking,
  unmapped/garbage reads — psx-spx "I/O Map"; Beetle memory.cpp.

## Axis 5 — Peripherals / devices  ← SUSPECTED WEAKEST (user flag)

Status: MIXED — "works for tested games," NOT edge-validated. Likely more gaps
than we think; the **hybrid-pad failure in Tomba is an axis-5 (SIO/controller)
bug**, not timing.
- [~] **SIO / controllers / memcard**: DualShock config-mode handshake (0x43),
  analog vs digital pad ID, the **hybrid pad mode failure** (Tomba) — Beetle
  frontio/input + psx-spx "Controllers and Memory Cards". HIGH PRIORITY per user.
  - DONE 2026-06-27: **pad ACK→IRQ7 timing made guest-cycle-paced** (was
    access-count-paced via `sio_irq_countdown=SIO_IRQ_DELAY_PAD`; pad fast-path
    now arms the cycle-paced ack scheduler, BAUD+ACK=1258 cyc, like the card
    path). This was the cause of the load=4 Tomba 2 boot wedge (accurate CPU
    exposed access-paced pad timing). See WEDGE_load4_shell_rootcause.md.
  - DONE 2026-07-26: Tomba now offers explicit Analog / D-Pad modes; the
    Tomba-only legacy pad-config fork and its debug/config surface were removed.
    Every title now uses the modern DualShock config state machine.
  - TODO (axis5 Fix-6 / "1.0e-e2"): fully remove the pad fast-path so pad+card
    share the unified shifter path.
- [ ] **GPU**: GP0/GP1 command set, rasterization rules (top-left fill, dithering,
  semi-transparency modes, mask bit, texture windows, blending), VRAM-as-texture —
  cross-ref DuckStation gpu_*, Beetle gpu.cpp, GPU test ROMs; validate by VRAM diff.
- [ ] **SPU**: 24 voices, ADSR, pitch/sample-rate, reverb, volume sweeps, IRQ —
  Beetle spu.cpp; psx-spx "SPU"; validate by audio-sample diff.
- [ ] **CDROM**: command set, response timing, sector read (data/XA/CD-DA), seek,
  shell/lid — Beetle cdrom.cpp; psx-spx "CDROM"; validate by sector/response diff.
  - **KNOWN OK-TO-DIVERGE (we are MORE faithful than the oracle) — from PR #9:**
    Suppressed in `tools/devtrace_diff.py` (`KNOWN_DIVERGENCES["cdrom"]`; use
    `--strict` to see raw). Beetle stays the independent oracle (unpatched); the
    tool LABELS these deltas, it does not silence real ones.
    - **CD controller version (Test `19h`,`20h`)**: we return `94 09 19 C0` (real
      SCPH-1001 sub-CPU, 1994-09-19, per psx-spx "CDROM Test Commands"). Beetle
      hardcodes `97 01 10 C2` (`cdc.cpp:2253`, a later PSone board) regardless of
      BIOS. Version byte `< 95h` keeps shell CD-init flag `[0xA000DFFC]` clear;
      `>= 95h` sets it and forces a spurious boot ReadTOC. So native (94h) issues
      NO ReadTOC where Beetle (97h) does → expect a CDROM event delta at CD-init,
      possibly cascading briefly. Verified vs psx-spx, NOT the oracle. MMX6+Tomba
      soak PASSED (2026-07-06). Trivially revertable (4 bytes, `cdrom.c` Test 0x20).
    - **Status reg `0x1F801800` bit 2 (ADPBUSY)**: idles at 0 (was wrongly pinned
      set under an "ADPCM empty" label). psx-spx: bit2 = XA playback → 0 when idle.
    - **GetID license region**: derived from disc serial (`SCEA`/`SCEE`/`SCEI`)
      instead of hardcoded `SCEI`; NTSC-U (MMX6/Tomba) now correctly report `SCEA`.
    - Method reminder (top-of-doc LESSON): these were HYPOTHESES until OUTPUT
      validation — the MMX6/Tomba playtest soak WAS that validation. Revert any
      that regress a shipped title.
- [ ] **DMA**: all 7 channels, block/linked-list/chain modes, timing, DICR/DPCR —
  Beetle dma.cpp; psx-spx "DMA".
- [ ] **MDEC**: macroblock decode, IDCT, color conversion, RLE — Beetle mdec.cpp;
  validate FMV frame diff vs Beetle.
- [ ] **Timers (0/1/2)**: all clock sources (sysclk/dotclock/hblank/÷8), modes
  (target/overflow/reset/IRQ-repeat/one-shot), sync modes — Beetle timer.cpp;
  psx-spx "Timers".

## Axis 6 — Static-vs-dynamic fidelity (recompiler-unique)

Status: STRONG (most project effort lives here).
- [ ] Self-modifying / install-at-runtime code (dirty-RAM interp) — ongoing.
- [ ] Function discovery / dispatch completeness (no missed indirect/jump-table
  targets) — resolve all dispatch misses each run (Tomba2Recomp CLAUDE.md).
- [ ] Call/return contract + stack fidelity — the blue-screen/wedge class.
- [ ] Backend equivalence (compiled == interp) — necessary, not sufficient.

## Axis 7 — Determinism

Status: SOFT SPOT.
- [ ] Boot run-to-run variance observed (sometimes wedges, sometimes clean) —
  track down; faithfulness presupposes determinism.

---

## Research findings (2026-06-26, parallel subagents, cross-referenced vs in-tree Beetle + psx-spx)

Per-axis deep-dives in `accuracy/*.md`. Headlines + priorities below. NOTE much is
already faithful — these are the GAPS. Implement serially (one branch each, code to
just-before-build, pull in + validate on the oracle one at a time). NONE of these
overlap the cycle axis or each other (different files), so they parallelize.

- **`accuracy/axis5_sio_controller.md` — HYBRID PAD BUG ROOT-CAUSED.** P0: cmd `0x43`
  must transmit the LIVE button frame (like `0x42`) + use the trailing byte to toggle
  config; we send all-zeros → active-low → "all buttons pressed" phantom input every
  `0x43` frame. P0: `0x45` status must report the LIVE analog/digital mode, not always
  analog-ON (→ frame-length misparse after a flip). Both were fixed, and the
  Tomba legacy fork was deleted 2026-07-26 when Tomba moved to explicit
  Analog / D-Pad modes. P1: analog-mode lock (`0x44`); P2: `0x4D`
  rumble echo + cycle-paced pad ACK. Validate: `sio_trace` diff on `0x43`/`0x45`.
- **`accuracy/axis5_gpu.md`.** P1: dithering ENTIRELY MISSING in both renderers (decoded
  but never forwarded) — largest systematic banding divergence. P2: SW rasterizer uses
  float + inclusive spans vs HW fixed-point half-open DDA w/ top-left bias → shared-edge
  double-draw + 1px over-fill. P3: sprite FlipX/FlipY unmodeled; mono-rect size mask
  wrong. FAITHFUL: all 4 semi-transparency modes, mask set/check, texture-window, CLUT
  4/8/15, texel-0, ×2 modulation, GP0/GP1 coverage. Validate: VRAM byte-diff + GPU test ROMs.
- **`accuracy/axis5_spu.md` — weakest subsystem.** P0: reverb ENTIRELY MISSING; P0:
  volume sweeps missing + ~2× gain-scale error; P1: noise gen + pitch-modulation absent;
  P1/P2: no SPU IRQ address-match, no capture buffers, SPUSTAT hardcoded; P1: host-audio-
  pull timing not guest-768-cycle clock. FAITHFUL: ADPCM decode + ADSR envelope (verbatim
  Beetle ports). Recommend: port Beetle PS_SPU on a guest-clock timeline. Validate: audio
  sample-stream diff.
- **`accuracy/axis5_mdec.md` — explains the slightly-off FMV.** HIGH: IDCT rounding/clamp
  diverges (contrast/ringing); HIGH: YUV→RGB lacks HW green-precision truncation (off-hue
  greens); HIGH: dequant domain/bias/clamp wrong (DC banding). MED: 4bpp output broken
  (textures, not FMV); MED: one-shot decode vs FIFO/block state machine. Validate:
  per-block decode diff + live FMV pixel-diff.
- **`accuracy/axis4_memory_mmio.md`.** GOOD: I_STAT/I_MASK/DICR ack semantics CORRECT.
  P1: RAM mirror wrong (we gate `<2MB`; HW aliases 2MB DRAM across an 8MB window 4× —
  mirror accesses silently read 0 / drop writes). P2: `IsC` over-broadly drops scratchpad
  writes (scratchpad is the D-cache, must stay addressable). P3: level-IRQ relatch; P4:
  per-segment addr masking; P5: open-bus high bits on I_STAT readback. Validate: MMIO
  trace diff + RAM-mirror sentinel probe.

Cross-cutting: every validation is a native(4500)↔Beetle(4382) ring-buffer diff on the
relevant state surface — the same oracle methodology as the cycle axis. Several need the
test-ROM harness (axis 1/GPU) which is worth building early.

## Phasing

1. NOW: cycle axis (axis 2) Stage-2 on Tomba 2 (FAITHFUL_TIMING_PLAN.md).
2. Build the comparator/test-ROM tooling (enables GREEN-ing items above).
3. Burn down axes here on Tomba 2 where validatable; axis 5 (esp. SIO/controller
   for the hybrid-pad bug) is the priority second front.
4. Validate cross-title (Tomba 1, MMX6, Ape, BIOS) before merge.
5. Merge wt/tomba2 → master; keep this doc as the post-merge burndown for the rest.

## SESSION 15m — CD dir-table base CORRECTED: fc6c0=0 is right (physical-0 table)

CORRECTION to SESSION 15k/15j: `0x801fc6c0 = 0` is NOT a bug. `guest_at(base)=
guest_arena+(base&0x1fffff)`, so base=0 resolves to physical 0 = guest
`0x80000000`. The CD dir table legitimately lives at physical 0.

PROOF (cd_read_log DMA dest = madr&0x1FFFFF = physical offset):
- 32B headers -> phys 0x0,0x20,0x40  (0x20-stride entry headers)
- 2016B data  -> phys 0x400,0xBE0,0x13C0  (= count*0x20 + idx*0x7e0, count=0x20)
This EXACTLY matches cd_channel_poll layout `entry=base+idx*0x20`,
`data=base+count*0x20+idx*0x7e0` with base=0,count=0x20. Poking fc6c0=0x80010000
was WRONG (points at scene data) — reverted.

LIVE counters (CDDBG): chan_cmd_submit=ch3_arms=ch3_chcr_kick=6,
finalize=0, store_state2=0, probe_calls=0. Entries at guest 0x80000000 show
state=3 (in-progress). Deferred INT1 coroutine RUNS (fn trace: cd_intc_dispatch,
cd_chan_full_init, cd_mc_state_machine, cd_result_wait_b all execute) but
cd_mc_state_machine never reaches the finalize block (br2eu_func_80199CF0 ->
cd_dir_entry_finalize 0x801990F0) that marks entry state==2.

REMAINING: find what cd_mc_state_machine (0x80199440) polls to detect a channel
read as complete (DMA status? byte counter? CD irq flag?). Entries stay state=3
=> the "channel done" predicate never fires. mc_date_serial_cmp/blk_io_retry
poll cd_channel_poll 27Kx waiting for state==2.

## SESSION 15o — CD livelock: state machine fully decoded, break isolated to sector->channel accounting

`cd_mc_state_machine` (0x80199440) fully disassembled -> docs/internal/cdmc_80199440.asm.
It is a SUBMIT-SIDE scanner, NOT the sector consumer:

- Entry gate: `fc69c==1` -> idle return; `fc688==0` OR `D1_CHCR bit24 clear` -> work path (0x801994c4).
- Work path calls stub 0x80195d44, then checks `entry[fc6a4].state` (u16 @ +0).
  - `state==0` (FREE) -> 0x80199574 real work: MMIO cmd + DMA setup via cd_copy_u32/cd_chan_cmd_submit.
  - `state!=0` (occupied, incl. done=2) -> status 4 busy return. CORRECT, not a bug.
- Completion predicate at 0x80199bc4: `entry[+6]-1 == entry[+4]` -> sets fc69c=1 ->
  cd_dir_entry_finalize on exit -> state=2. entry+4 = received/progress (low16 of word1),
  entry+6 = expected+1 (high16). For ent0 word1=0x00050000: +4=0, +6=5 -> needs +4 to reach 4.
- entry+4 written by 0x801AA2AC (store_pc 0x801AA2BC) but with CONSTANT value -> progress source stalled.

Channel entries: ent0-2 = streaming reads state=3 (flags 0x8001), ent3 = done state=2.
fc6a4 submit cursor=3, fc6a8 finalize cursor=0, fc6ac poll cursor=0. All slots busy -> status 4 correct.

DMA done-bit theory RULED OUT: DuckStation dma.cpp ShouldSetIRQFlag = irq_enable[ch] AND master(bit23).
ch3 CDROM irq_enable (DICR bit19)=0 -> done-bit27 never sets on real HW either. cd_cb7_run/finalize
is NOT the ch3 path. CHCR bit24 (busy) clears on completion = the real completion signal (polled).

OPEN: what increments entry+4 (received count) per delivered sector? Sectors ARE produced
(cdrom_sector_history: lba advancing past 208829, data_delivered=1, total 205937) and INT1 fires
(cd_mc_state_machine invoked ~179K times). The per-sector -> entry+4 accounting is the break.
cd_read_step_cb (0x801985c4) = file-read path, NOT channel streaming; never invoked (correct).

## SESSION 15p — BIOS sub-channel sector callback decoded (0x801AA1E4/0x801AA244/0x801AA2AC)

cd_subchan_table_install (cd.c:1555) registers TWO aliases of br2eu_func_801AA1E4 into the
BIOS CD sub-channel dispatch cells @0x801feb44 (0x801aa244) and @0x801feb48 (0x801aa2ac),
then bios_tramp_c003/c002(1,0x801feb40) arms them. a0 = subchannel ctx.

Callback signature: (a0=ctx, a1=sector_index, a2=data_a, a3=data_b).
ctx layout: +52(0x34)=tableA of channel-entry ptrs, +56(0x38)=tableB, +68(0x44)=channel count.
Per call, for index a1 (bounds-checked < ctx->count):
  entryA = ctx->tableA[a1]; if nonzero: entryA->+70=a2, ->+72=a3, ->+68 |= 1
  entryB = ctx->tableB[a1]; if nonzero: entryB->+70=a2, ->+72=a3, ->+68 |= 1
store_pc 0x801AA2BC = `entry->+68 |= 1` (data-ready flag). gwatch CONFIRMED this fires.

So per-sector delivery DOES reach the channel entries (sets +68 ready flag + data ptrs).
REMAINING BREAK: the consumer that reads channel-entry +68/+70/+72 and increments the
dir-table entry's progress halfword (+4) so the 0x80199bc4 predicate
(entry[+6]-1 == entry[+4]) can fire cd_dir_entry_finalize (state 3->2). That consumer is
either a second INT handler, a poll in cd_mc_state_machine's work path, or a BIOS hook —
not yet located. dir entries (0x20B: state@+0, prog@+4/+6) are a DIFFERENT struct than the
channel entries (+68/+70/+72); the index/link between them is the open question.

## SESSION 15q — LIVE dir-entry decode + deadlock characterization

Dir table base fc6c0=0 (guest addr 0, low-mem alias of main RAM). Active entries 0x20B stride:
  ent0 @0x00 state=3 w1=0x00050000 (+4 lo=0, +6 hi=5) stamp=0x02292646
  ent1 @0x20 state=3 w1=0x00050001 (+4 lo=1) stamp=0x02302646
  ent2 @0x40 state=3 w1=0x00050003 (+4 lo=3) stamp=0x02312646
  ent3 @0x60 state=2 (done) -> freed manually -> auto-reinited 0x80010000, fc6a4 wrapped 3->0

Completion predicate (cdmc_80199440.asm 0x80199bc4): entry=*(0x801FEB18)=entry[fc6a4];
  lhu +6 -1 == lhu +4 -> fc69c=1 -> jal 0x801990f0 finalize. u32@+4 packs lo16=received,hi16=expected+1.
Submit path (0x80199cf0+): sh state=3 to entry, fc6a4++, if fc6b0&&fc69c -> finalize.
Status-4 early return (0x80199570) fires when entry[fc6a4].state!=0 -> skips submit+finalize.

DEADLOCK: ent0-2 state=3 frozen (+4 = 0,1,3 need 4). ch3 DMA idle (6 transfers @frame444 then stop).
sector_read_pos=0, ring_dropped>425k growing -> drive produces sectors, consumer never drains.
Freeing ent3 proved scanner wraps (fc6a4 3->0, ent3 reinited) but ent0-2 still starve -> status4.
ROOT: streaming entries stopped receiving sectors. Delivery = BIOS subchan cb (0x801AA2AC sets
channel-entry +68) OR ch3 DMA ring->channel. Neither advancing. Needs oracle RAM-diff or the
channel->dir progress consumer located.

## SESSION 15r — ROOT localized to recompiled BIOS CD-streaming path; oracle building

LIVE verification (port 4413, fibers+default identical):
- Game RUNS: frame advancing ~30fps, attract/demo rendering. NOT a hard hang.
- cd_read_log: linear file load lba216335 -> dest 0x172000+ (+0x800/sector). Data CONFIRMED
  landing (0x172000/0x172800/0x173000/0x174000 all non-zero asset bytes). Sector->dest works.
- BUT ring_dropped>436k growing, sector_read_pos=0, ch3 DMA idle -> drive re-reads same
  LBAs (208850-208913 oscillating) = stream-buffer-full livelock on the CHANNEL path.
- 3 dir entries ent0-2 (state=3) frozen at +4=0,1,3 (need 4). These are the streaming
  channels (stamps 0x2292646/0x2302646/0x2312646 = consecutive LBAs). ent3 done/freed.

Delivery chain for streaming channels:
  BIOS streams sectors -> invokes subchan cb 0x801AA2AC (real BIOS via bios_svc->svc_call,
  NOT HLE) -> marks channel-entry +68/+70/+72 -> consumer advances dir-entry +4 ->
  predicate +6-1==+4 -> fc69c -> finalize -> state2 -> cd_channel_poll -> state4 -> free.
BREAK: dir-entry +4 never advances => channel->dir progress consumer OR the BIOS
  subchannel-callback invocation is broken in recompiled BIOS MIPS (generated code).

ORACLE PATH (now viable): Docker daemon started. `duckstation_oracle.py image` building
arm64 ubuntu:22.04 builder in background (/tmp/oracle_image.log). Next:
  python3 psxrecomp/tools/duckstation_oracle.py build --container
  python3 psxrecomp/tools/duckstation_oracle.py install --bios <scph7502>
  python3 psxrecomp/tools/duckstation_oracle.py start --disc Game.cue   (port 4371)
Then diff dir entries @0x00-0x7f + fc6c0-cursors + channel ctx vs our runtime to see
whether real HW advances +4 and which consumer does it.

## SESSION 15s — SMOKING GUN: subchannel cb fires but consumer dead

gwatch on dir-entry +4 fields (ent2+4=0x44) caught store_pc=0x801AA2BC firing EVERY FRAME
(~200K cyc) with old==new — the BIOS subchannel callback re-sets channel-entry +68 ready
flag but NOTHING consumes it. Channel->dir progress consumer is dead/absent.

Callback args live: a0=0x8FD0 (ctx), a1=0x400. The +68 flag stays set; no reader advances
dir-entry +4 -> entries starve -> status4 -> ch3 never re-arms -> ring overflows.
NEXT: locate the consumer that reads channel-entry +68/+70/+72 and bumps dir +4. It is
either a per-INT handler, a per-frame pump, or gated on a flag our runtime leaves clear.
Oracle (building) will show whether real HW invokes it and from where.

## SESSION 15t — dir entries = disc-loaded STREAM DESCRIPTORS; payload buffers

cd_read_log tail decode (lba 208829): the game reads 32-byte descriptors into
0x00/0x20/0x40/0x60 (= the dir-entry addrs, size=0x20) AND 2016-byte payloads into
0x400/0xBE0/0x13C0. So dir entries are channel/stream DESCRIPTORS loaded from disc;
+4/+6 are stream position fields advanced as payload sectors arrive.

Buffer check: 0x400 and 0xBE0 have data; 0x13C0 is ALL ZEROS -> one stream's payload
never landed -> its descriptor +4 frozen -> entry never completes -> cd_file_pump
stuck at state 0x100 (cd_file_load_sm returns 0 forever) -> fight never loads.

Interrupt map fully decoded: mask@0x801bcd48=0xd -> VBLANK(tbl0=0x8019ae68),
CDROM(tbl2=0x801979f8=cd_response_pump), DMA(tbl3=0x8019af7c=cd_cb7_run).
ptrA=0x1f801070(I_STAT) ptrB=0x1f801074(I_MASK). ch3 CDROM irq-enable(DICR bit19)=0
so its done-bit never sets -> cb7_run can't see it (matches DuckStation ShouldSetIRQFlag).

BIOS svc trace: game spins B0/0x0B(1.36M)+C0/0x0A(49K) polling CD status forever.
NEXT: find why stream payload for the starving channel doesn't reach its buffer —
likely the per-sector->descriptor+4 update in the recompiled stream-read path, OR
ch3/PIO delivery gating. Oracle RAM-diff will show correct +4 progression.

## SESSION 15u — TWO-callback CD architecture decoded; streaming cb = cd_mc_sm_wrap

cd_response_pump (CDROM INT1, tbl[2]=0x801979f8) loops cd_response_process()->s0:
  s0&4 -> call_guest(DAT_801bc84c, DB8(0x801bcb25), 0x801fc6d0)  // per-sector cb
  s0&2 -> call_guest(DAT_801bc848, DB8(0x801bcb24), 0x801fc6c8)  // complete cb
DAT_801bc84c/848 set via mc_session_b/mc_session.

LIVE: pump_cnt(0x801bce04) incrementing ~180/s (INT1 firing). cb84c=0x80198fe4 =
cd_mc_sm_wrap -> cd_mc_state_machine = the STREAMING/channel per-sector cb.
cb848=0 (no complete cb). cd_read_step_cb(0x801985c4, file-load cb) dbgctr frozen
@0x42 -> file-load session ended; STREAMING session now owns cb84c.

cd_mc_state_machine only READS entry+4 (0x80199810/d0/9bc8), never writes it ->
+4 increment lives in a DIFFERENT sector-arrival fn. dir ent0-2 state=3 +4 frozen
(0,1,3 need 4). NEXT: find the +4 writer (channel +68/+70/+72 consumer) OR oracle
diff. cb84c path IS the live one to trace.

## SESSION 15v — payload-DMA submission path frozen; deadlock cycle isolated

Channel-command subsystem (game/cd.c): cd_chan_job_submit(0x8019ea98) ->
cd_mc_op_wait(0x8019ff4c) -> cd_chan_cmd_burst(0x8019dxxx) -> cd_chan_cmd_pump
-> cd_chan_cmd_submit(0x80199d88). cd_chan_cmd_submit arms DMA ch regs at
0x1f801080+a0*0x10 (+0 MADR,+4 BCR,+8 CHCR) then stores a4->CHCR to start.

LIVE counters: cd_chan_cmd_submit(0x801bcf00)=7 FROZEN. Job state @0x801febe0
st[0]=0 (idle, not stuck). febd0=3 febd4=5. So the payload-DMA submitter ran
7 times then STOPPED being invoked -> streams got initial chunks (0x400/0xBE0
have data, 0x13C0 empty) then starve -> dir +4 frozen -> entries never
complete -> cd_mc_state_machine status-4 -> no new submits. DEADLOCK CYCLE:
  payload DMA done -> should trigger next submit -> doesn't -> +4 frozen ->
  entry busy -> status4 -> no submit.

cd_mc_state_machine confirmed READ-ONLY on entry+4/+6 (only psx_cyc_load_half
at +4/+6 in full_46.c:2778/3298/3975). +4 writer is a separate consumer that
isn't running. Both subchan cbs 0x801aa244/0x801aa2ac alias br2eu_func_801AA1E4
(sets channel +68/+70/+72 only).
NEXT: find what re-triggers cd_chan_job_submit after a payload DMA completes
(DMA-done path or per-frame pump), OR restart+early-gwatch to catch +4 writer
PC during stream setup.

## SESSION 15w — streams are cd_stream/tile_stream system, NOT intro movie

intro_movie_state(0x8012c0b4) state byte @0x1f80008c = 0xffff (-1 IDLE) ->
intro/attract movie path is DONE/not active. The stuck dir entries ent0-2
(state=3, +4 frozen) belong to the SEPARATE continuous-stream system:
cd_stream_mode_init(0x80159ce4) + tile_stream_pump(0x80159864)->cd_mc_state_machine,
driven per-sector via cb84c=cd_mc_sm_wrap(0x80198fe4).

mc_op_wait/cd_chan_job_*/cd_chan_cmd_* are MEMORY-CARD ops (mc=memcard), NOT the
CD stream path — earlier suspicion corrected. cd_chan_cmd_submit frozen@7 is
memcard activity, unrelated.

OPEN: the continuous-stream consumer that advances dir-entry +4 as payload
sectors arrive. cd_mc_state_machine reads +4 (load_half only). Writer is a
separate fn not yet isolated — likely reads channel +68/+70/+72 (set by
br2eu_func_801AA1E4 subchan cb) and bumps dir +4 + copies payload to stream buf.
Both oracles unavailable on macOS (DuckStation -nogui needs window; no linux-arm64
prebuilt deps for container; no beetle-psx source/lib present).

## SESSION 16a — PHYSICAL-0 COLLISION: BIOS channel entries overlap game dir table

Fresh LLE-boot run (PSX_BIOS_HLE=0, port 4436) reproduces the livelock AND reaches
frame 743 before the IRQ->0xC000 dispatch crash (same crash as HLE-boot frame-444;
the 0xC000 is the IRQ/exception path firing during the infinite CD poll, a SYMPTOM
of the livelock not a separate bug). gwatch on dir table 0x80000000-0x80000080:

- 433 writes to 0x80000000 (ent0 state) from BIOS store_pc 0xBFC1EAC8 — descriptor/state churn.
- **1615 writes to 0x80000044 from store_pc 0x801AA2BC = the BIOS subchannel cb
  `entry->+68 |= 1`.** 0x80000044 is ent2's +4 field. cb a0=0x8FD0(ctx), a1=0x400.

COLLISION DECODED (generated full_52.c br2eu_func_801AA1E4 + alias body):
- cb: entryA=*(ctx->tableA[a1]) gets +70=a2,+68|=1 ; entryB=*(ctx->tableB[a1]) gets
  +72=a3,+68|=1. tableA=ctx+52, tableB=ctx+56, count=ctx+68.
- +68 store hit guest 0x80000044 -> channel entry base = 0x80000000 = **SAME as
  dir table base (fc6c0=0)**. Channel entry fields +68/+70/+72 land at
  0x80000044/46/48 = ent2's +4/+6/+8. So "ent2 +4 progress" is actually the
  channel ready-flag, not a progress counter — the dir/channel structures
  OVERLAP in physical 0.
- ctx @0x80008FD0: tableA(+52)=0x0 (NULL->phys0), tableB(+56)=0xb0 (phys0). Both
  resolve into physical-0, i.e. the BIOS subchan ctx tables point INTO the same
  low-RAM region the game uses for dir descriptors + payload (data @0x400+).

CONCLUSION: ONE of the two phys-0 base assignments is wrong. fc6c0=0 was "proven"
by descriptor DMA dests (phys 0/0x20/0x40 headers, 0x400+ payloads) — but that
only proves the DESCRIPTOR reads land at phys 0; it does NOT prove the streaming
channel tables belong there. The BIOS CD streaming ctx tableA/tableB=phys0 is the
prime suspect: on real HW the channel-entry array should be a distinct BIOS/kernel
buffer, not aliased onto the game's descriptor table. If tableA/tableB are wrong,
the cb marks a phantom entry (always 0x80000000, +68 stuck=1, never drained)
instead of the real per-channel entries -> no consumer advances dir +4 -> livelock.

NEXT: (a) trace who writes ctx->tableA/tableB (ctx+52/+56 @0x80009004/08) during
BIOS CD init — find the intended channel-entry array base; OR (b) oracle RAM-diff
(DuckStation oracle build still incomplete on macOS — build/ has only CMakeCache).
Verify whether ctx+68 count and the a1=0x400 index are sane vs a real channel count.

## SESSION 16b — IRQ path CONFIRMED WORKING; break is stream bookkeeping, not interrupts

Live livelock instance (port 4419, frame 645+, PC transient-0 is normal: generated
code clears cpu->pc at each block entry). Corrects a false lead from session 16a:

- fntrace on ENTRY addresses showed total:0 for cd_intc_dispatch / cd_mc_state_machine
  / cd_mc_sm_wrap / cd_ctx_restore / cd_chan_full_init — but this is an ARTIFACT.
  The deferred-interrupt coroutine re-enters these functions at CONTINUATION basic
  blocks, never at their hooked entry. cdrom_trace_dump proves the ISR runs:
  seq 29666 set_irq(0x01=DATA_READY) -> 29667/29668 fire_irq -> i_stat=0x4, and the
  consumer PC 0x8019A9AC = cd_intc_dispatch+0xc0 (inside its 0x8019a8ec-0x8019aab8
  body). IRQs fire, irq_flag cycles set->present->ack->0, cd_cmd_issue_b /
  cd_result_wait_a/b drive Setloc/Read/Seek and poll 0x1F801800/803.
- cdrom_state: irq_enable=0x07 (matches ALL encoded reasons 1-5 per
  cdrom_irq_mask_matches_reason), irq_flag=0 at snapshot (acked), sector_pos
  advanced 0->12->2060 of 2340, ring_dropped=516 (response FIFO overflow because
  consumer reads 1-at-a-time while drive streams).
- CONCLUSION: hardware IRQ delivery + BIOS ISR + response decode ALL work. The
  livelock is purely that dir/channel entry +4 progress never reaches the
  +6-1 completion predicate, so cd_mc_state_machine scans forever without
  finalizing -> cd_channel_poll never sees state==2 -> blk_io_retry spins.
- Remaining suspect unchanged: physical-0 collision. The +4 progress writer is the
  BIOS channel-entry consumer that reads ctx->tableA/tableB entries (+68 ready,
  +70/+72 payload) and credits the descriptor. With tableA=0/tableB=0xb0 resolving
  into phys-0 (the dir table itself), that consumer reads dir-table words as
  channel entries -> finds no valid ready entry -> never credits +4. The a1=0x400
  callback arg and the intended channel-entry array base are still unverified.
  ALSO NOTE: 0x801BCE00 region now shows terrain-packet bytes (0x55555555/0x48804040)
  — heap-cursor corruption is overwriting CD globals too; may be a second-order
  effect of the same stream failure or an independent emitter bug.

## SESSION 16c — ROOT UNIFIED: terrain-packet emit overrun destroys CD/heap control structures

Live instance dumps prove the "CD livelock" and "heap corruption" are ONE bug.
Terrain packet records (0x5555xxxx markers, 0x48804040 vertex words, 0x0500xxxx
fields) are written across low memory, overwriting every structure the CD stream
depends on:

- phys-0 dir/channel table 0x80000000+ (ent fields -> phantom "progress")
- BIOS CD streaming ctx 0x80008FD0+ (reads as packet bytes, NOT a real ctx —
  earlier tableA=0/tableB=0xb0 "collision" was packet data misread as pointers)
- CD globals 0x801BCE00+
- heap control block 0x801C4780-0x801C47DC: cursor cell 0x801c47b8 = 0x555503bc
  (packet word, was a cursor), tag-table ptr 0x801c47c0 = 0x555503bc, etc.

terrain_packet_gen (0x80135a14) emits a param_6 x param_7 grid of 0x48-byte GPU
records forward from param_2 = g32(0x801c47b8) (the heap cursor). The emit range
0x801c46c8.. overlaps the control block at 0x801c47b8/c0 only ~0xF0 bytes later,
so even a modest grid marches through and overwrites cursor+tag-table+neighbors.
The cursor is legitimately seeded to guest_of(tag_table_get()) = guest addr of
g_DAT_801c47c0's pointee, and advanced by stream_block_decode/mode3_dispatch
returns (block_copy_ret_len / lzss_decode — both verified to return GUEST values,
not host pointers; the earlier host-pointer theory is disproven).

So the emit DESTINATION (cursor) or grid dims are wrong: packets land in the
scene/control region instead of a dedicated packet heap. Once 0x801c47b8/c0 are
packet bytes, every downstream cursor/tag read is garbage -> decode dst wild ->
CD stream descriptors/channel ctx destroyed -> +4 never completes -> blk_io_retry
livelock -> IRQ->0xC000 crash. IRQ path itself is healthy (session 16b).

NEXT: determine the intended terrain_packet_gen output buffer. Check whether
param_2 should be a dedicated GPU-packet heap region (not the decode cursor),
whether tag_table_get()/g_DAT_801c47c0 should point at a real tag array vs the
emit region, and whether param_6/param_7 (BKX lhu4/lhu6 stage dims) are sane.
Likely a clean-port arg/return conflation between the decode cursor and the
packet-emit cursor.

## SESSION 16d — gwatch: CD DMA delivers file data ONTO heap control block

Fresh LLE run (port 4440) with PSX_GWATCH="0x801C47B8,64". The 0x5555/0x48804040
"terrain packets" are actually CD FILE DATA (game asset bytes) landing on the
heap control block — terrain_packet_gen is NOT the writer.

Decisive entry seq21 (frame 617): store to 0x801C47C0 with
  dma.madr=0x1C4804, dma.init_pc=0x801978AC (=cd_cmd_issue_b+0xac), bcr=0x00010200
=> a CD DMA channel wrote ~sector data to phys 0x1C4804 = guest 0x801C4804,
   i.e. ONTO the tag-table global g_DAT_801c47c0 / cursor-cell neighborhood.

ROOT REFINEMENT: the CD file-load / decode destination resolves to ~0x801C47C0,
which IS the tag-table POINTER global (tag_table_get() returns g_DAT_801c47c0 =
*(u32*)0x801c47c0). Loading file data AT 0x801c47c0 overwrites the tag-table
pointer itself -> tag_table_get() returns file-data garbage -> decode dst wild ->
cursor/tag/control all corrupt -> CD stream +4 never completes -> livelock.

On real HW the tag-table buffer and its pointer global must be distinct: file
data should go to *g_DAT_801c47c0 (the pointee array), never to 0x801c47c0.
The bug is a dest/pointer conflation in the file-load path — likely
cd_file_load_sm(0x13, guest_of(tag_table_get())) at save.c:6050 passing the
POINTER-GLOBAL address as the DMA/decode dest instead of the pointee buffer,
OR g_DAT_801c47c0 never initialized to a real heap tag array.

NEXT: read g_DAT_801c47c0 (0x801c47c0) BEFORE corruption to get intended tag-array
base; check cd_file_load_sm dest arg semantics (guest addr of buffer vs of the
pointer cell); find who is supposed to allocate/init the tag array.

## Session 16d — dir-entry lifecycle decoded (generated-MIPS trace)

Entry record (0x20 stride, base=*(u32*)0x801fc6c0, idx via fc6a4/fc6a8):
  +0x00 u16 state   (2=complete, 3=in-progress)
  +0x04 u16 progress
  +0x06 u16 expected
  +0x08 u32 data addr
  +0x1c u32 tag/id

Key functions (all generated, full_46.c):
- 0x80199CF0 = dir-entry SUBMIT. Loads entry ptr from cached 0x801FEB18,
  write_half(entry,3) -> state=3; fc6a4++; calls cd_dir_entry_finalize ONLY
  when fc69c!=0 (synchronous/immediate path). fc69c is the ready-to-finalize
  flag.
- 0x80199BC8 = completion DETECTOR. load_half(entry+4)=progress, compares
  (expected-1 from entry+6); if progress==expected-1 -> fc69c=1. Called
  indirectly (no direct jal; fn-ptr/continuation).
- cd_dir_entry_finalize 0x801990F0: state=2, +0x1c->0x801feb00,
  +8->0x801feb04, fires cb@0x801feb10, fc69c=0.
- cd_mc_state_machine 0x80199440 caches entry=fc6c0+fc6a4*0x20 into
  0x801FEB18 (store @0x80199524) and re-reads it ~10x; completion block at
  0x801999C4-0x801999D8 reads entry+6 (expected) and gates finalize.

OPEN: the entry+4 PROGRESS INCREMENTER is still unidentified. gwatch on the
phys-0 dir table showed ent0/ent1 +4 receive ZERO writes of any kind (CPU or
DMA) across the whole livelock -> the per-sector progress path is DEAD/gated,
not merely slow. 0x80199BC8 (the consumer that would set fc69c) therefore
never sees progress==expected-1. NEXT: find the DMA/sector-arrival handler
that increments entry+4 and why it is not reached (likely the CD data is not
landing in the entry's expected buffer, or the IRQ pump that calls the
incrementer is gated on a channel-ready flag that never sets).

## Session 16e — live-verified stuck entry (port 4440, actively streaming)

read_ram ground truth on a LIVE (non-crashed) instance:
  fc69c=0 fc6a4=1 fc6a8=0 fc6ac=0 fc6b4=0xFFFFFFFF fc6c0=0(dir base=phys0)
  0x801FEB18 (cached entry ptr) = 0
  ent0 @0x80000000: state=3, +4(progress)=0, +6(expected)=5, +8=1, +0x1c=0x02292646
  ent1/ent2: empty

CONFIRMED: ent0 is the sole active stream descriptor. progress(+4)=0 vs
expected(+6)=5; completion detector 0x80199BC8 needs progress==expected-1=4.
+4 receives ZERO writes (CPU or DMA) across the entire run -> the per-sector
PROGRESS-INCREMENT handler is never invoked for ent0. This is a DEAD PATH,
not a wrong value: the CD read was submitted (state=3) but the sector-arrival
consumer that bumps +4 never runs.

The incrementer is a generic u16 field write (174 candidate `sh +4` sites
game-wide) -> cannot isolate statically. It is invoked from the CD
sector/DMA-arrival path (IRQ -> response_pump -> per-channel cb). Since +4
stays 0, either (a) ent0's sectors are DMA'd to a buffer the arrival handler
doesn't associate with ent0, or (b) the arrival callback for this stream is
never registered/fired. NEXT: trace which function owns the entry+4 RMW by
gwatch on 0x80000004 during a FRESH submit (catch the first/only writer PC),
and check the DMA dest (ent0 +8/+0xc..+0x14) vs where ch3 actually writes.

## Session 16f — +4 credit is external (DMA/sector-arrival), bc848=NULL

- cd_mc_state_machine does NOT store entry+4/+6 (only stack + FEB18-cached
  entry+0 state). Its +4 reads (0x80199810,0x801998D0) are consume-vs-init
  gates: progress==0 -> init branch (0x801998E0 reads entry+8 -> CD regs
  0x801FC680/684); progress!=0 -> consume path 0x80199B10.
- => +4 must be credited by the CD DMA/sector-ARRIVAL handler OUTSIDE the
  state machine. Since +4 stays 0, that handler never runs for ent0.
- Live cb regs (4440): 0x801BC848(type-0 cb)=NULL, 0x801BC84C(type-1 cb)=
  0x80198FE4(cd_mc_sm_wrap). If sector-data arrival is a type-0 response it
  has NO callback -> +4 never credited. cd_response_pump fires cb by response
  type; verify which type carries per-sector DMA-done and whether its cb is
  registered.
- ent0 buffer params: +8=1, +0xc=0xeb0, +0x10=0x00f00140, +0x14=0x38000880.
  NEXT: dma_state -> does ch3 write ent0's expected buffer? and which
  response type / cb should bump +4.

## Session 16g — ROOT CAUSE CANDIDATE: ch3 CD-DMA programmed but IDLE

dma_state on live 4440 (actively "streaming"):
  ch3 (CD-ROM DMA): madr=0x00000BE0 bcr=0x000001F8 chcr=0x00000000
                   active=0 remaining=0
  => channel ARMED (madr/bcr set) but CHCR=0 -> DMA NOT transferring.
  cdrom_state: reading=1 sector_available=1 -> drive reads sectors into the
  controller buffer but DMA never drains them to guest RAM.
  => no sector-arrival -> entry+4 never credited -> state=3 forever -> livelock.

The DMA kick is cd_chan_cmd_submit (game/cd.c): writes MADR(+0),BCR(+4),
then waits on *(0x801BCC40) bit 0x40 (controller-ready) before writing
CHCR(+8). If that ready-wait never satisfies OR the CHCR write value is 0 /
goes to the wrong place, the transfer never starts. chcr=0 (not even the
enable/direction bits) suggests the final CHCR store wrote 0 or was skipped.
ALSO possible: a SECOND DMA manager (BIOS CD driver) owns ch3 and the game's
submit fights it; or the DMA-done IRQ that re-arms the next sector never
fires because the first transfer never completed.

NEXT: instrument cd_chan_cmd_submit's CHCR write (value + addr) live; check
0x801BCC40 ready-bit semantics; determine whether BIOS or game owns ch3 MADR.

## Session 16h — dead-path confirmations + oracle assessment

- fc6c0=0 (phys-0 dir base) is CORRECT: _DAT_801f4754=0x80010000 holds asset
  data not entries; the phys-0 table IS the live dir table.
- _DAT_801fc68c (pump gate) = 0; cd_mc_state_machine also driven IRQ-side via
  cd_mc_sm_wrap(0x80198FE4)->0x80199440.
- fntrace entry-hooks ALL show total=0 for 0x80199360/0x80199BC8/0x801990F0/
  0x80199CF0 -> coroutine continuation re-entry bypasses hooks; fntrace is
  UNRELIABLE for this subsystem. Need block-level trace or oracle.
- wtrace during livelock shows only BIOS housekeeping writes (0xBFC1xxxx),
  no CD stream data landing in an entry-associated buffer.

ASSESSMENT: the +4 progress credit is a per-sector DMA/arrival consumer inside
the cd_mc_state_machine coroutine cluster (0x80199440-0x80199Cxx continuation
blocks). Static trace of 65 blocks + coroutine register flow is at the limit
of tractability. The reliable unblock is a HARDWARE ORACLE RAM-diff:
  - psx-beetle needs libmednafen_psx.a built via MinGW (Windows-only path).
  - psx-oracle (port 4371) is a co-sim deterministic build, NOT hardware truth.
  - DuckStation oracle at ~/.local/share/retcomm/oracle/duckstation is a full
    src checkout + Ninja/clang++-19 CMakeCache (incomplete build). Building it
    on macOS arm64 is the remaining path to ground-truth RAM/state compare.
NEXT: either finish the DuckStation oracle build (background, long) OR add a
block-level guest-PC trace (not entry fntrace) to catch the +4 writer PC.

## Session 16i — ORACLE LIVE + fc6c0 divergence (CORRECTS 16h)

DuckStation oracle BUILT natively on macOS arm64 and LIVE on port 4371
(running Game.cue, frame 10000+, working correctly). Tools:
psxrecomp/tools/duckstation_oracle.py. BIOS caveat: JP SCPH7502 vs PAL game.

ORACLE-VERIFIED DIVERGENCE (same game, same streaming subsystem):
  ours : fc6c0(dir base)=0x00000000   _DAT_801f4754=0x80010000  fc6c4=0x20
  orac : fc6c0(dir base)=0x80010000   _DAT_801f4754=0x80010000  fc6c4=0x20

=> cd_dir_table_set RAN (fc6c4=0x20 correct) but captured a0=0, i.e.
   _DAT_801f4754 was 0 at call time; it became 0x80010000 later.
   Init-ordering/timing divergence: on HW _DAT_801f4754 is set before
   tile_stream_open; in recomp it is not (or fc6c0 is reset afterward).

CONSEQUENCE: dir entries + stream DMA land at phys-0 = kernel/exception
vector region -> kernel corruption + the 0xC000 dispatch + +4 never credited.
THIS is the likely root cause of the CD livelock (supersedes the t0/CHCR
suspect — that was a symptom of DMA targeting phys-0).

CORRECTION to 16h: "fc6c0=0 is CORRECT" was WRONG — I misread the
0x80010000 region as asset data; it IS the dir table (oracle confirms).
cd_dir_table_set is the ONLY writer of fc6c0 (clean cd.c + generated
0x8019580C); cd_dir_reset/cd_dir_clear do NOT touch fc6c0.

NEXT: oracle `watch` on phys 0x1FC6C0 to capture the correct write sequence
+ timing vs _DAT_801f4754 writes; determine whether tile_stream_open runs
before scene_entry sets _DAT_801f4754, or fc6c0 is reset post-scene_entry.

## Session 16j — livelock loop identified + CD state (gwatch/clean_trace)

clean_trace tail shows the ACTUAL livelock: an infinite alternating poll
  cd_queue_push (0x80192F14, ra=0x801265F4) <-> cd_timeout_arm (0x801936B0,
  ra=0x80192970). A CD command is submitted, times out, retries forever.
  => the pending CD op never reports DONE -> queue never drains -> spin.

cdrom_state at livelock:
  mode=0xA0 (was 0xE0 earlier), ring_dropped=516 (sectors dropped - consumer
  not draining), reading=0, pending_cmd=0x09, read_cmd=0x00,
  sector_read_pos=2060/2340 (partially consumed), seek_msf=00:08:12,
  read_msf=00:08:30. irq_flag=0x00, i_stat=0x04.
cmd history tail: seq128 cmd0x06 reading=1 (frame639), seq129 cmd0x09
  pending=1 (frame645) -> stuck on cmd 0x09 since ~frame645.

gwatch on fc6c0/_DAT_801f4754: only frame-647 writes from store_pc
  0x801929F8 (bulk region init). NO cd_dir_table_set write of 0x80010000
  ever landed -> our recomp reaches PC=0 (crash) BEFORE the dir-table
  setup the oracle completes at ~frame1348. fc6c0=0 is the uninitialised
  BSS value, i.e. SYMPTOM of crashing early, not necessarily the cause.

REVISED MODEL: the PRIMARY divergence is upstream — a CD command
  (0x09 = likely a seek/read-setup for the attract stream) pends but its
  completion IRQ/response never arrives -> cd_queue_push timeout-retry
  livelock -> eventually PC=0. fc6c0=0 is downstream fallout.
NEXT: find why cmd 0x09 never completes — check the CD response/IRQ
  delivery for that command vs oracle; ring_dropped=516 hints the
  sector->consumer path stalls. Oracle is live on :4371 for RAM-diff.

## Session 16k — ROOT CAUSE FOUND: CD pointer-table corruption (gwatch)

Oracle-vs-recomp RAM diff (DuckStation :4371 vs recomp) PROVED the CD
driver register-pointer cells are corrupted in ours:
  cell        oracle(correct)   ours(corrupt)
  0x801b8428  0x1f801814         0x55051814   (CD status reg ptr)
  0x801b8434  0x1f8010a8         0x1c555503   (DMA reg ptr)
  0x801b842c  0x1f8010a0         0x20555555
  0x801b8444  0x1f8010f0         0x2c555503
Once corrupt, cd_queue_drain busy-waits *(wild)&0x04000000 forever ->
cd_queue_push<->cd_timeout_arm livelock -> PC=0. fc6c0=0 was DOWNSTREAM.

Tight gwatch (0x801b8428..8447, excludes hot timer @8450+) caught the
corrupting writer: ALL writes store_pc=0x801929F8 in br2eu_func_80192944,
ra=0x801265F4, frame 647, DETERMINISTIC (identical values every run).
Instruction 0x801929F8 = `sh a0,6(s1)` (0xA6240006) inside a fixed-point
routine: `lh v1,4(s1)` coeff, `mult v1,v0`, `sh a0,6(s1)` store, `mflo a2`.
s1 walks a buffer writing halfword results -> the destination buffer
wrongly overlaps the CD pointer table (0x801b8420). Likely audio/ADPCM
decode or transform writing PCM/coeff output to a bad pointer.
NEXT: trace a0->s1 provenance (caller @0x801265F4 -> func 0x80192944) to
find why the output buffer resolves to CD-driver data instead of its
proper heap/audio region. Ghidra MCP was DOWN this session (conn refused).

## Session 16k-addendum — converges with 15y decompression runaway

func 0x80192944 (fixed-point halfword transform: lh coeff/mult/sh result)
sits in the generated gap between cd_cmd_queue_load(0x80192634) and
cd_queue_push_wrap — a CD/GPU data-path routine, NOT in fnmap (no clean
port). Call chain: br2eu_func_80126328 -> match_mode_set(0x801560f8) ->
battle_intro_flow(0x80167330, clean menu.c) -> coroutine 0x80192944.

RECONCILE with 15y: the 0x5555 halfword writes match 15y's documented
"~500KB RAM overwritten with repeating 0x55555555/0x40408048/0xfa/0xfc
pattern during menu_res_load_sm->cd_file_load_sm", sourced from
stream_block_decode->mode3_dispatch->{lzss_decode|block_copy_ret_len}
runaway on garbage src. func 0x80192944's output buffer (s1=a0) resolving
to 0x801b8420 is most likely a VICTIM: its dst ptr was corrupted by the
earlier decompression overflow, OR it is a decode/transform stage fed
poisoned src. The TRUE root cause remains 15y's: validate stream_block_
decode src offset + heap cursor dst so decode can't run away.

## Session 16k-final — corruption is WIDESPREAD (decompression runaway confirmed)

Live RAM on corrupted instance: heap cursors ALSO poisoned —
  _DAT_801e3cfc=0xf0488040, _DAT_801c47b8=0x0146aeae (both 0x5555/0x0480
  pattern, not valid heap ptrs). stream_block_decode uses these as the
  mode3_dispatch DST cursor -> once poisoned, every decode writes to a
  garbage dst -> cascade. func 0x80192944's s1 dst=~0x801b8420 is one
  victim; CD ptr table another; heap cursors the propagator.
ROOT CAUSE (per 15y, now re-confirmed): stream_block_decode src offset /
heap cursor produces a bad mode3_dispatch dst or runaway decode length.
FIX TARGET: find FIRST bad write — validate srcg (tag_table offset +
header) and the dst cursor BEFORE mode3_dispatch runs; ensure decode
length is bounded by the real compressed-block size, not a garbage src
terminator. Ghidra MCP down this session.

## Session 16l — decompression PROVEN innocent; real writer = terrain_packet_gen

New `decomp_trace` ring (decomp_trace_record + debug cmd) instrumented
mode3_dispatch/block_copy_ret_len/lzss_decode. On a fresh run to the
frame-647 corruption: ALL ~190 decode calls had valid dst/src/mode and
bounded output — ZERO lzss_runaway hits. => 15y/16k "decompression
runaway" hypothesis is DISPROVEN for this corruption. The 0x5555/0x4040
"poison" is terrain_packet_gen's literal packet padding
(MW32(a2w+0x18,0x55555555)), not decode output.

gwatch store_pc was STALE (clean-C writes don't update
g_debug_last_store_pc). Authoritative writer = host `rip`, symbolized:
  rip 0x1018CF5xx = terrain_packet_gen (menu.c:13821-13828 MW16 vertex
  coords), ra_chain save_load_state_machine->menu_res_load_sm->
  battle_intro_flow->clean_try_dispatch->psx_dispatch->dirty_ram.
=> terrain_packet_gen IS a real writer to 0x801B8428 (CD ptr table).
   (16d's "terrain_packet_gen is NOT the writer" was for 0x801C47C0/CD-DMA
   file data — a DIFFERENT address/writer. Both hit the heap control block.)

CURSOR RUNAWAY (oracle-verified): bump-allocator cursor _DAT_801c47b8
  oracle=0x80010000 (reset to arena base)   ours=0x816312a2 (~23MB, >RAM)
It is the SHARED dst cursor for stream_block_decode AND terrain_packet_gen
(save.c:6440 chains 5 calls via BR2_W32(0x801c47b8,ret)).
terrain_packet_gen returns t2 = param_2 + emitted records; emits a
param_6 x param_7 grid of 0x48B records. Measured args (tag9 trace):
  param_6=0x300(768) param_7=0x300/0x2, cursor already 0x816xxxxx.
  BKX(v)=v/2; dims come from table 0x801b1524 (lhu4/lhu6) — that table is
  ITSELF overwritten by the same packet poison (oracle clean).
768x768 cells ~590K records ~27MB -> the runaway advance that marches the
cursor through 0x801b1524 -> 0x801b8420 -> 0x816312a2.

OPEN: are param_6/param_7 (grid dims) wrong, or is the cursor/arena
management wrong (missing reset / wrong return)? On HW the cursor stays
bounded ~0x80010000. Faithful fix = make the emit cursor + dims match HW
arena accounting, NOT clamp/symptom-patch.

## Session 16m — ROOT CAUSE FIXED: BKX grid-dim mistranslation (commit 9dc0f0d)

gwatch on cursor cell 0x801C47B8 + host-rip symbolization (atos, live
vmmap base) gave the AUTHORITATIVE writers — store_pc 0x801929F8 was STALE
(clean-C writes don't update g_debug_last_store_pc). Real writers to the
cursor cell: save_load_state_machine bookkeeping (save.c:6364/6443/6511/
6515) + terrain_packet_gen packet emits (menu.c:13814/13818).

Cursor trace (seq): reset 0x80010000 @fr439/611 -> climbs via heap_alloc54
+ stream decodes -> 0x801C495c @fr647 (CD ctrl region) -> FIRST >RAM write
seq178 = save.c:6443 storing terrain_packet_gen call-1 RETURN 0x8160d00c
(~21MB emitted). terrain_packet_gen param_6=param_7=768 -> 768x768 cells.

ROOT CAUSE = clean-port mistranslation in save_load_state_machine BKX:
  generated arg setup (SLES_017.22_full_31.c, all 5 calls) computes the
  grid dims as  lh s2h+4/+6 -> sll 16 -> sra 23   =  v/128  =  0x600/128 = 12
  clean port BKX was  (v<<16)>>17                 =  v/2    =  768
Oracle table 0x801b1524 confirmed lhu4=lhu6=0x600 for every entry -> HW
also feeds 0x600, but HW transform yields 12 (12x12 grid ~144 cells).
768x768 ~590K cells ~21MB -> cursor runaway -> CD ptr table 0x801B8420 +
heap cursors overwritten -> cd_queue_drain wild-ptr livelock @frame647.

FIX: BKX(v) = ((int32_t)(((uint32_t)(int32_t)(int16_t)(v))<<16))>>23
(sign-extend lh, sll16, sra23 — faithful to generated). Result: CD ptr
0x801B8428 stays 0x1F801814, cursor stays in-RAM, game passes frame 647
and runs (frame counter >300k). func 0x80192944 + decompression fully
exonerated (victims/downstream).

## Session 16n — RESULTS-STALL root-caused to cd_file_load_sm parked coroutine (NOT pad)

Symptom: after a full attract battle the game freezes on the results/scoreboard
(state 0x0601, sub-state DAT_1f800001==1) indefinitely; frame counter keeps
advancing ~55fps. NOT a pad problem — the resource reload itself never finishes.

Call chain (all clean ports, verified live):
  state6 -> battle_intro_flow(0) [menu.c:1834, blocking coroutine]
    DAT_1f800001==1 waits on menu_res_load_sm(...)!=0   [menu.c:1928]
      -> save_load_state_machine state 0x10 (L10)      [save.c:6526]
        -> cd_file_load_sm(sel+0xb0 = 0xb3) returns 0 forever
           -> menu_res_load_sm st20 (s1+0x8c==0x20)     [save.c:4178]

Live-verified frozen state (port 4460, read_ram):
  0x1f800000=0601  0x1f80008c(menu sm)=0x20  0x1f80008e(save sm)=0x10  — all static >8s
  cd_file_load_sm coroutine state @0x801B55EC == 1 (parked continuation)
  fntrace on cd_read_issue(0x8018D150)+cd_read_poll(0x80198E2C): ZERO calls in 3s
    => the coroutine is NOT reaching the read-issue/poll stage; it is parked
       on an earlier continuation waiting on a completion flag.

CD drive is HEALTHY (rules out a disc/IRQ stall):
  cdrom_state: reading=0 read_delay=0 irq_flag=0 sector_read_pos advanced
  2060->2062 (sectors ARE delivered). i_mask=0x0D enables CDROM(bit2)+DMA(bit3);
  only VBLANK(bit0) pending. DMA ch3 configured, all channels idle.
  Cumulative bfrd_set=1468/bfrd_clear=13, ring_dropped=611 (whole-session
  counters — may be normal streaming, not the stall).

File being loaded: file_id 0xb3 -> table 0x801B5B94+0xb3*8 = {ptr=0x801b58bc,
  val=0x16}; metadata @0x801b58bc = sector extents 0x0967,0x1032,0x1107,0x1149
  (valid packed-CD layout). Entry val static 0x16 (a descriptor, not progress).

Memcard NOT the blocker: mc_status slot0+slot1 present, valid "MC" magic,
  dirty=false. memcard_err path not implicated.

## Session 16o — ROOT MECHANISM FOUND: cd_file_load_sm throttle counter @0x801F68D8 holds garbage

Session 16n's "parked on a completion flag" was superseded by full disassembly
of generated br2eu_func_8018CFDC (372B, full_41.c:37631). The function is a
NON-blocking per-frame state machine keyed on state word @0x801B55EC:

  entry: s1=*(0x801B5B94+file_id*8)=file data ptr, s3=*(entry+4)=size/meta,
         s4=param_2 dest. a0>=0 -> block_8018D074.
  block_8018D074: v0=*(s1); if 0 -> ret -1.
  block_8018D08C: s0=*(0x801F68D8); s0--; if s0!=0 -> *(0x801F68D8)=s0-1, ret 0.
                 if s0==0 -> proceed to state machine.
  state==0: cd_read_issue(...) then *(0x801B55EC)=1, ret 0.
  state==1: cd_read_poll -> done/ret1 / re-issue / ret0.

=> 0x801F68D8 is a PER-FRAME THROTTLE/DLY counter: each call decrements it and
   returns 0 (busy) until it reaches 0, only THEN touching the read state
   machine. This is why fntrace showed ZERO cd_read_issue/cd_read_poll calls —
   the coroutine never gets past the throttle.

LIVE-VERIFIED BUG (gwatch mprotect write-watch on 0x801F68D8, port 4460):
  * Field starts 0 (BSS). cd_file_load_sm init path stores 0 via 0x8018D064
    (frames ~547/611). Then between init and first decrement (frame ~685) it
    becomes 0x12220000-class garbage. It then decrements ~3300/sec (headless
    uncapped) but needs ~303M -> effectively infinite -> results stall.
  * gwatch ring (2048 entries, seq 0..2047): ONLY store_pc 0x8018D064 (init)
    and 0x8018D0A0 (decrement). NO cpu->store wrote the huge value => the
    corrupting write came through a path gwatch does not instrument as a CPU
    store: DMA, an alias/dirty-RAM host mapping, or EXE-init data.
  * Memory @0x801F6800..0x801F6900 is dense asset/PRNG-seed data, NOT clean
    globals: 0x801F68E0 = LCG PRNG seed (battle.c:13659 rand: seed*0x41c64e6d
    +0x3039), 0x801F68F0 = console_emit 32B line buffer. 0x801F68D8 neighbours
    (0x801F68DC=0xf1111101, 0x801F68E0=0x4200022f) are STATIC; only D8 is
    decremented. The 0x122xxxxx value is classic LCG-range output.

REFINED (write_ram probe): the field is a LEGIT countdown, re-corrupted each
  iteration. Zeroing 0x801F68D8 -> cd_file_load_sm proceeded, read-issue set it
  to 0xfaff (64255 = real byte/sector count), it drained ~5300/sec to ~0, the
  load stage completed, then the NEXT stage re-wrote it to 0x1221f670 (same
  0x122xxxxx garbage) -> stuck again. So 0x801F68D8 is NOT inherently corrupt;
  a per-iteration writer feeds it a bad count. The bad value 0x122xxxxx is
  LCG-range output and the field sits 8B below the PRNG seed @0x801F68E0 —
  suspect the count/size source (file extent, sector LBA, or a struct field)
  is being read from corrupted/adjacent data, OR a DMA/alias write lands the
  PRNG/data bytes on it. Next: trace WHO writes 0x801F68D8 with the huge value
  at the stage transition (gwatch armed, catch the non-decrement store_pc —
  it was absent from the CPU-store ring, so look at DMA completion / cd_read_*
  writers / dirty-ram alias).

HYPOTHESIS (next session): 0x801F68D8 either (a) sits inside a file/asset
  load DMA destination buffer that overruns onto it, or (b) is a field that a
  CD-load-setup path should reset to a small value but the reset is not
  running for this file_id path. Determine the file-load DMA destination
  (cd_read_issue param_2 dest via save_load_state_machine->menu_res_load_sm
  chain) and whether its [dest,dest+size) covers 0x801F68D8. If so the fix is
  the load-dest/buffer bound (faithful core). If not, find the alias-mapping
  writer (dirty_ram / guest_at host-pointer class, cf. the OT-nextlink and
  heap-cursor fixes).

SEPARATE confirmed issue (still open): pad producer dead — records
0x801BEFA8/CA never written, zero 0x42 SIO polls, tx_writes frozen. Both pad
records and this throttle field live in the game's async I/O / channel-driver
layer (0x801ff080 channel blocks, fn-table 0x801c4780) that is not being
serviced while blocking coroutines hold the frame loop.

SESSION 16p — 0x801F68D8 corruption ROOT-CAUSE CLASS NARROWED to DMA/asset
overwrite of the globals region.

  * Live memory dump @0x801F6800..0x801F6900: the field sits inside a DENSE
    quantized engine table (repeated 0x10/0x11/0x12/0xf1/0xef byte patterns —
    LUT/GTE-style data), NOT clean counter globals. Neighbours confirmed:
    0x801F68DC=0xf1111101, 0x801F68E0=0x4200022f (PRNG seed), 0x801F68F0=
    console_emit line buffer, gfx.c uses 0x801f6910/0x801f6d10 as indexed
    transform-table bases. The whole 0x801f68xx-0x801f6dxx span is one
    engine-data cluster.
  * gwatch on 0x801F68D8 caught a LIVE corrupt jump 0x18 -> 0x1221ffd4. The
    2048-entry ring held ONLY cd_file_load_sm CPU stores (0x8018D064 init,
    0x8018D0A0 decrement) — every CPU store to the field writes small values
    (3/0/decrement). No CPU store wrote the huge value => external writer.
  * gwatch header counters: oor=254,063,987 / dropped=42,161,719 — the whole
    4KB page phys 0x1F6000 is a CONTINUOUSLY hot write target (a DMA streams
    through it). dma.madr in entries marched 0x1F6144->0x1F6430 climbing
    toward phys 0x1F68D8. dma.init_pc=0x80192EBC (GPU-OT DMA setup fn
    0x80192E78). The corrupting write is a DMA/host-alias write that either
    bypasses the mprotect page or was evicted by the ~5000/sec decrement flood.
  * SMOKING GUN — heapB top @0x801e3cfc reads 0x0df0fd1c (INVALID; heapA top
    @0x801c47b8 = 0x801c90b8 valid KSEG0). The entire 0x801e3cxx region reads
    as the SAME quantized table bytes (0x1f2d21f2, 0xd32f3e10, ...). So
    0x801e3cfc is not a heap pointer at all — it sits INSIDE an asset/table
    buffer that a DMA has filled. I.e. large BSS/globals spans are being
    overwritten by loaded asset data.

ROOT-CAUSE MODEL (high confidence): a file/asset CD-DMA (or bulk table fill)
has a destination that lands in the 0x801Exxxx/0x801F6xxx globals region
instead of its intended heap/scratch buffer — overwriting heap tops, the CD
throttle counter, PRNG seed neighbours, etc. This is the same host-pointer /
guest-address mis-resolution class as the OT-nextlink (40e9bbf) and
heap-cursor (7394315) fixes: a buffer guest-address is computed wrong (host
ptr masked/truncated, or guest_of on a non-RAM pointer), so DMA writes asset
bytes over live globals. The throttle counter is just the most visible victim
because cd_file_load_sm busy-waits on it.

NEXT (concrete): find the DMA destination / heap-alloc that produces a
0x801e3cxx-0x801f6xxx write. Candidates: (a) heap_alloc54 / heap top
0x801e3cfc returning a wild guest addr; (b) a cd_read_setup dest param
(DAT_801bcb84) computed from a bad extent/host ptr; (c) the engine LUT/table
load whose fixed dest overlaps globals. Instrument dma.c write path (or
psx_write_word) to log any write landing in [0x801e0000,0x801f8000) with the
channel + initiator PC — that names the exact transfer. Then fix the
destination computation in the faithful core (NOT a counter reset).

SESSION 16q — CORRECTION + precise writer identified.

  * NEIGHBOR-WATCH experiment: armed gwatch on 0x801F68C0..0x801F68D7 (table
    bytes around the counter, EXCLUDING it). The huge re-corruption still
    fired (0x18->0x1221fc50) but the ring stayed flooded by the SAME
    store_pc 0x8018D0A0 decrement — because gwatch 'ranges:3' kept the old
    0x801F68D8 watch armed too (arm ADDS ranges, does not replace). The
    neighbours 0x801F68DC=0xf1111101 / 0x801F68E0=0x4200022f stayed STATIC
    across every read => the corrupting write is a TARGETED 4-byte store to
    EXACTLY 0x801F68D8, NOT a contiguous table/DMA fill. The 'asset table'
    reading earlier was coincidental static globals.
  * DISPROVED the DMA/asset-overwrite theory: cd_read_log shows ch3 CD DMA
    only bounce-buffers sectors at phys 0x8598; dma.madr cycling
    0x1F5F08-0x1F6430 is the GPU-OT command DMA (init_pc 0x80192EBC =
    fn 0x80192E78), which READS RAM->GPU and cannot write guest RAM.
    heapB 'pointer' 0x801e3cfc=0xdf0fd1c is likewise a misread static
    field, not a live heap top.
  * PRECISE WRITER: 0x801F68D8 is written ONLY inside br2eu_func_8018CFDC
    (cd_file_load_sm) — three stores, all offset 26840 from base 0x801F0000:
      0x8018D060 sw gpr[2]=3 (init const), 0x8018D064 sw gpr[0] (VARIABLE),
      0x8018D0A0 sw gpr[2]=s0-1 (decrement). No other generated function or
    clean-C site touches it (_DAT_801f68d8 defined game.c:274, unused).
  * The huge value MUST come through 0x8018D064 storing gpr[0]. block_
    8018D064 is reached ONLY from block_8018D040 (_bc_8018D04C branch),
    which follows the init-call chain mc_init_reset(0x80157368) ->
    cd_init_session(0x80198AF0) -> cd_mode_flag_set(0x80195D8C). BUT
    cd_mode_flag_set returns only 0/1, so gpr[0] on the pure-init path is
    small — meaning block_8018D040/64 is ALSO reachable as a coroutine
    block-leader (case 0x8018D040) from a LATER state where gpr[0] holds
    computed file/extent metadata. That is the huge-value source.
  * The values are NOT constant (0x1221ffd4 / 0x1221fc50 / 0x11eb8d35 /
    0xfa990cf / 0x121e240c) => a COMPUTED count (sector LBA / file offset /
    extent-derived), varying per stage — consistent with mis-parsed packed
    extent metadata (file table @0x801B5B94+file_id*8 -> {extent_ptr s1,
    val s3}; extents are packed words like 0x00300800).

NEXT (concrete): catch the 0x8018D064 store with huge gpr[0] and read the
gwatch gpr/ra[] context — ra[] names the caller that computed the count.
Because the decrement (0x8018D0A0) floods the 2048-ring, either (a) dump
gwatch within <~300ms of the stage transition, or (b) temporarily raise the
ring / add a store_pc filter. Then trace gpr[0]'s producer: the extent
iterator that turns file-table {ptr,val} + packed extent words into the
sector/byte count — THAT is where the miscount lives (faithful-core fix,
not a counter reset).

================================================================
SESSION UPDATE — 0x801F68D8 ROOT CAUSE FOUND (CD-DMA overwrite)
================================================================
The earlier "only cd_file_load_sm writes it / computed-count" theory is
DISPROVEN. Live wtrace (WRITE_TRACE_CAP=262144, full reg context) caught
the decisive store after a CORRECT 4-byte counter zero (write_ram is
BYTE-ONLY — earlier "zero" attempts only cleared the low byte, which is
why prior captures never saw a clean re-arm):

  seq 261  addr=0x1F68D8  old=0x00000000  new=0x12220000  w=4
          pc=0x801978AC  cpu_pc=0x00000000  dma_ch=3  frame=52427447
          ra=0x8019A9C8  v0=0x11000000  a0=0x1F8010B8  a1=0x00010200
          a2=0x00020943  sp=0x801BDD08

KEY: dma_ch=3 => the write is CD-ROM DMA channel 3, NOT a CPU store.
pc=0x801978AC is STALE g_debug_last_store_pc (the CPU store that wrote
ch3 CHCR to kick the DMA). new=0x12220000 is raw CD sector data.

DECODED WRITER PATH: br2eu_func_80197800 (full_45.c:27934) is the CD
command/DMA sequencer. It loads dest pointers from the CD state block
@0x801BCB0C..0x801BCB50 — which are DMA ch3 register addresses:
  0x801BCB48 -> 0x1F8010B0 = ch3 MADR (dest)
  0x801BCB4C -> 0x1F8010B4 = ch3 BCR  (count)
  0x801BCB50 -> 0x1F8010B8 = ch3 CHCR (control; store @0x801978AC kicks it)
Live ch3 regs: MADR=0x1FA8B8 (phys -> KSEG0 0x801FA8B8), BCR=0x10200.
The DMA sweeps UP through phys 0x1F68D8 (=KSEG0 0x801F68D8) writing file
data over the engine globals.

ROOT CAUSE: the file-load DESTINATION CURSOR is corrupt. cd_file_pump
(game.c:828) loads files to dest=_DAT_801e3cfc, a streaming cursor init
to 0x80010000 (game.c:862) and advanced by stream_block_decode returns
(_DAT_801e3cfc += iVar3, game.c:870-878). _DAT_801e3cfc now reads
0x0DF0FD1C — itself CD file data, i.e. the cursor was ALREADY overwritten
by an earlier bad load. Self-reinforcing: file data lands on globals ->
overwrites the dest cursor + throttle + PRNG/console/gfx neighbours ->
next load uses the corrupted dest -> more globals overwritten.

CONFIRMED globals hit by the same CD DMA: _DAT_801f68d8 (throttle),
_DAT_801e3cfc (dest cursor, =0x0df0fd1c garbage), and the dense
quantized region 0x801F68xx-0x801F6Dxx (gfx transform tables, PRNG
seed @0x801F68E0, console buf @0x801F68F0) — all read as file/LUT bytes.

NEXT (faithful fix, NOT a counter reset): find why the file-load dest
cursor/base lands in 0x801F6xxx-0x801FAxxx (globals/BSS) instead of the
0x80010000 streaming buffer / heap. Suspect the SAME host-pointer->
guest-address mis-resolution class as OT-nextlink (40e9bbf) and
heap-cursor (7394315): a heap-top / tag_table / stream_block_decode
return feeding _DAT_801e3cfc or the cd_file_load_sm dest arg is a host
ptr or wrong guest addr. Trace stream_block_decode's iVar3 return and
the heap-top globals (_DAT_801c47b8=0x801C90B8 ok; _DAT_801e3cfc bad)
to find the first bad dest.

REFINEMENT (same session): the corrupt dest is the STREAMING cursor
_DAT_801e3cfc, advanced by stream_block_decode -> mode3_dispatch ->
block_copy_ret_len/lzss_decode return lengths. NOTE menu.c:2241-2248
already documents+fixed ONE instance of this exact class (host-ptr
truncation -> garbage length -> cursor corruption -> CD dest into
globals) via guest_of(param_2). Corruption persists => either that fix
is incomplete (another host-ptr path / block_copy_ret_len / lzss_decode
still returns a bad length) or the cursor is overwritten by the CD DMA
itself (self-reinforcing once file data lands on globals).

ORACLE COMPARISON (port 4371 vs ours 4460):
  0x801F68D8/0x801F6A00/0x801FA000 : oracle=zeros/structured globals,
    ours=CD file data  => real HW never file-loads into globals.
  _DAT_801e3cfc cursor : oracle=0x800F0000 (sane, ~900KB into 0x8001xxxx
    stream buffer), ours=0x0DF0FD1C (file-data garbage) => ours ran
    ~2MB too far / was overwritten.
NEXT: audit block_copy_ret_len + lzss_decode return lengths (the cursor
advance) for host-pointer truncation or mis-sized decode; and find the
FIRST write that puts file data on globals (the initial bad dest), since
_DAT_801e3cfc itself gets clobbered by the same DMA.

=== 0x801F68D8 / results-stall: PRECISE boundary + runaway cursor ===

ORACLE MEMORY MAP (DuckStation :4371, persistent conn required):
  0x801d0000 = dense file/heap data; 0x801e0000 = zeros
  => file/stream data legitimately spans 0x80010000 .. ~0x801d-0x801e0000.
  0x801e3cfc(cursor)=0x800f0000 sane; 0x801f68d8(counter)=0 clean;
  0x801c47b8(heap cursor)=0x80000180 (early-boot value).
  0x801c6000-0x801ca000 on oracle = dense file data => 0x801Cxxxx is LEGIT
  stream-buffer territory, NOT off-limits globals. Globals start ~0x801e0000.

OURS: CD DMA wrote file data 0x801c90b8->0x801fa8b8 (~202KB, 0x63=99 sectors)
covering 0x801e3cfc(cursor-ptr) + 0x801f68d8(counter) => ~0x1a000 (~107KB)
PAST oracle's file-data boundary into globals.

ROOT: heap/file-load cursor _DAT_801c47b8 over-advances ~107KB into globals
during cd_file_pump. It is advanced by (a) stream_block_decode returns,
(b) terrain_packet_gen returns (save.c:6443+ BR2_W32(0x801c47b8,ret)),
(c) cd_file_load_sm file loads. The corrupting CD read dest = _DAT_801c47b8
value 0x801c90b8 (already in file-data tail), then 202KB tipped into globals.

EXONERATED: terrain_packet_gen grid dims sane (decomp_trace tag9 param6=param7
=12 => 12x12 grid, ~7.8KB/call). Decode dst reaching 0x801c856c is LEGIT
(oracle has file data there too). decomp_trace shows the globals decode
session (dst 0x801c63f8+) had cursor ALREADY in globals = symptom not cause.

SUSPECT (next): a file-load sector-count/size miscount (~99 sectors read vs
~50 real => ~100KB overrun) OR cumulative decode/terrain over-advance.
File table @0x801b5b94 = {ptr->0x801b55f0+idx*4, val=byte size}; extent table
@0x801b55f0 holds packed LBA/sector descriptors (0x00691402-style).
NOTE: clean-C writes to _DAT_801c47b8/_DAT_801e3cfc go through raw guest_at()
pointers and BYPASS wtrace entirely -> use decomp_trace (decode dst/src) +
CD-state block (0x801bcb80-94) + oracle boundary, not wtrace, for these.

FIXED THIS SESSION: bind.c bind_globals_2 pointed 15 g_DAT_*/scratch globals
into private guest_arena/guest_scratch (dead in runtime build) -> split-brain
vs generated code on real ram[]. Now routes via guest_at() -> ram[]/scratchpad.
Commit 626cd74.
