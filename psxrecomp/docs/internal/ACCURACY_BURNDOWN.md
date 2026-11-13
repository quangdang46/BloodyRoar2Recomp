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
