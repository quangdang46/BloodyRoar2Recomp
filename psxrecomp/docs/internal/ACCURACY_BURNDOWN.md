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

cd_read_log EXONERATION: all 1557 CD sector-read DMA records have dest <=
0x1be800 (max) - ZERO writes into globals phys 0x1c0000-0x1fffff. The
corrupting ch3 file-data write to 0x1F68D8 is therefore NOT a sector-read;
it is the file-load DMA->final-dest path inside generated cd_file_load_sm
(0x8018CFDC), whose MADR/dest comes from param_2 (= caller's _DAT_801c47b8 /
uVar2 heap cursor). The file data legitimately fills 0x80010000..~0x801d0000;
_DAT_801c47b8 reached 0x801c90b8 then a ~202KB (0x63-sector) load overshot
to 0x801fa8b8 into globals. Over-advance of _DAT_801c47b8 accumulated across
prior pump loads/decodes (~107KB excess vs oracle boundary).
NEXT: instrument _DAT_801c47b8 advancement (clean-C guest_at writes bypass
wtrace) OR audit generated cd_file_load_sm's dest/size computation; compare
per-file load dest+size sequence ours vs oracle cd_read_log (same JSON cmd
works on :4371) to find the first divergent load.

================================================================
SESSION 17 — frame-1546 WEDGE ROOT-CAUSED + FIXED (commit 4711294)
================================================================
SYMPTOM: clean-enabled build froze deterministically at framecnt=1546
(0x60A) during the file-0x89 menu load. clean_calls, event_ring, and
framecnt all froze together; gen-only (CLEAN_MIN=MAX=0xffffffff) sailed
past to frame ~542K reading lba ~122K — a clean-only divergence.

ROOT CAUSE (instruction-log + register proof): the CPU livelocked in the
low-RAM kernel IRQ trampoline `0xB0: li t0,0x2AA0; jr t0` -> `0x2AA0`,
re-entered from cd_intc_dispatch tail (ra=0x8019AA9C) and cd_read_step_cb
(ra=0x80198824). i_stat=0x1 (VBlank) pending forever, IEc=0. NOT a CD-data
drain bug — ch3 DMA was complete; it was an IRQ-dispatch storm.

MECHANISM: bios_table_tramp()/bios_kobj_tramp()/bios_tramp_a044() are
()-signature stubs -> br2_bios_tramp() reads LIVE cpu->gpr[4..7]+gpr[31]
to build svc_call. Generated callers set a0-a3 explicitly before `jal`
(e.g. cd_mode_reset_seq does `move a0,zero` @0x8019E3B8). Clean C callers
invoke the stub bare -> cpu->gpr holds the CALLER's stale args ->
svc_call(B0/0x5b, C0/0x0a, A0/0x44) gets wrong args -> kernel IRQ/event
attributes corrupt -> IRQ misroute -> the 0xB0->0x2AA0 storm.

FIX: delegated the 8 functions that call the gpr-forwarding trampolines
to generated MIPS (internal jal then runs with correct live gpr):
  cd_read_param_set, cd_channel_main_init, cd_mode_reset_seq,
  cd_mode_apply_seq, cd_chan_init_trio, cd_subsys_init_full,
  cd_chan_slot_set, cd_channel_init  (+ IRQ-path helpers cd_intc_busy_read,
  cd_cb8_dispatch, cd_init_done_cb, op_flag_fn_check, gpu_mode_regs_apply).
Verified vs generated disasm. Result: framecnt 9331->123K+ advancing,
clean_calls ~113M, no storm, valid memory cards, pad_buttons reach SIO.

NOTE: explicit-arg clean reconstruction is possible (a0/a1 recovered per
site; a2/a3 carry over from callee exit-gpr) but br2_call_generated on the
WHOLE function is strictly more faithful (correct ra/nested-call handling).
Kept as bridge; convert to readable C only when RE'ing each function's
full logic (these are thin BIOS-call sequences — real clean source needs
the svc semantics, a separate effort).

OPEN (unchanged from 16q): the _DAT_801c47b8/_DAT_801e3cfc heap/stream
cursor over-advance into globals during heavy cd_file_pump remains the
documented gameplay blocker. On the post-fix instance cursors read SANE
(0x80010000 / 0x8003f5dc) — corruption triggers only under heavy pumping
(scene transitions / attract demos), which headless menu-idle hasn't hit.
Headless display is disabled so visual fight-confirmation isn't possible
in this env; game executes all systems (IRQ/CD/GPU/kernel/pad/memcard).

## Session 18 — post-wedge state is a CD event-flag livelock (not attract idle)

After the frame-1546 fix the instance runs stably (framecnt 139K+, IRQs clean,
pad/memcard/GPU all live) BUT the main thread is parked in an infinite
kernel-event poll — NOT attract/menu idle:

- clean_trace = 100% `cd_seek_state_machine` (0x8019d5b0) = `jr 0xB0` B0/0x0B
  TestEvent, ~832M calls, ra=0x8019d550 (cd_seek_complete_poll wait loop).
- irqctx_ring: VBlank fires every frame; interrupted PC is ALWAYS
  0x8019d550/0x80156630 (CD waits). cdrom_active=0 always.
- hle_dump (LLE backend): OpenEvent B0/0x08 class 0xF0000009 ->
  EnableEvent B0/0x0C a0=0x8019B0FC -> TestEvent B0/0x0B a0=0x8019B0FC
  repeat=832408939. The event handle stored @0x801bde38 is 0x8019B0FC —
  a CODE pointer (br2eu_func_8019B0FC), NOT a normal 0xF100000x EvCB handle
  (compare class-0xF0000003 OpenEvents which DO return 0xF100000x).
- cdrom_state: pending.active=0 (NO command in flight), irq_flag=0,
  last_sector.frame=347. DeliverEvent is ONLY ever called for class
  0xF0000003 — NEVER 0xF0000009 (the CD class the game polls).

INTERPRETATION: the game issued a CD op ~frame 347; its completion
DeliverEvent(0xF0000009,spec) never ran (or the OpenEvent handle itself is
bogus), so TestEvent never reads READY and the seek/read wait spins forever
without issuing the next command. Two candidate roots, both in the
clean->generated->BIOS-call boundary:
  (a) OpenEvent(0xF0000009) returned 0x8019B0FC instead of 0xF1xxxxxx —
      a v0/return-value propagation bug through FUN_8019b380's
      br2_call_generated->generated->jr 0xB0 chain (same class as the
      frame-1546 gpr-forwarding bug, but on the RETURN path).
  (b) the CD ISR/deferred coroutine that should DeliverEvent(0xF0000009)
      is starved/not resuming (cd_intc_dispatch never appears in trace).
NEXT: A/B whether FUN_8019b380's OpenEvent return is corrupted by forcing
cd_ext_init_once/cd_dma_chan_init fully generated, OR trace the LLE kernel
OpenEvent return for class 0xF0000009 vs 0xF0000003. The 0x8019B0FC handle
is the anomaly to confirm first — if it's genuinely wrong, TestEvent can
never succeed regardless of DeliverEvent.

## Session — CD event OpenEvent returns 0 (TestEvent spins on slot 0)

RESOLVED (partial): hypothesis (a) CONFIRMED + FIXED. Delegating
cd_ext_init_once/cd_dma_chan_init/cd_hw_chan_init to generated MIPS
(commit 0cd07f7) restored correct $v0 through the `jr 0xB0` BIOS boundary —
cd_hw_chan_init's 8 OpenEvents now return proper 0xF1000000-7 handles and
populate the EvCB table (verified via read_ram @0x8e08). The bogus
0x8019B0FC handle is gone.

REMAINING BUG (narrowed): the game STILL parks in
cd_seek_complete_poll -> cd_seek_state_machine -> TestEvent(*(0x801bde38)).
Now 0x801bde38 = 0 (was 0x8019B0FC). TestEvent(0) polls EvCB slot 0, which
is a 0xF4000001 CALLBACK event (never READY) -> infinite spin.

EvCB table @0x8e08 (16 slots, LLE kernel-managed — route=0 for ALL B0 calls,
s_call_hle_on=0): only slots 0-7 populated, ALL class 0xF4000001/0xF0000011
CALLBACK events from cd_hw_chan_init. NO class 0xF0000009 event exists, and
NO OpenEvent(0xF0000009) appears in hle_dump (2295-entry ring).

Generated cd_ext_init_once (br2eu_func_8019B308) DOES emit the full sequence:
guard @0x801BDEA0 -> 0x8018E300 -> cd_ext_table_init(0x8019BEE0) ->
FUN_8019b380(0xF0000009,0x20,0x2000,0)=OpenEvent -> sw v0 -> 0x801BDE38 ->
FUN_8019b390 EnableEvent -> 0x8018E310. $at IS reloaded @0x8019B354 before
the store, so the store address is correct; v0=OpenEvent return = 0.

CONCLUSION: LLE kernel OpenEvent(0xF0000009,0x20,MARK,0) returns 0 — either
(a) cd_ext_init_once's init chain exits before block_8019B338 (guard set=1
at block_8019B31C but 0x801BDDF4 INTC-table cursor is 0 -> cd_ext_table_init
may not have populated handlers), or (b) kernel rejects the open because the
event system isn't ready at gfx-init time (cd_hw_chan_init opens succeed —
it runs LATER). NOTE: cd_dma_chan_init @0x8019b310 is a PHANTOM — it is the
`addiu sp` insn inside cd_ext_init_once, has NO dispatch entry, and NO
callers; its br2_call_generated delegation is a harmless no-op. The real
function is cd_ext_init_once @0x8019B308.
NEXT: determine if block_8019B338/OpenEvent executes (watch 0x801BDE38
write + FUN_8019b380 entry for class 0xF0000009), and whether the LLE
kernel event table is initialized before cd_chan_boot_init->cd_ext_init_once
runs. If ordering, the fix is to defer the CD-event open until kernel
InitEvents completes.

## Session — CD-event guard traced to cd_dma_xfer_start mistranslation

Instrumented 0x801BDEA0 across a fresh boot: it is BSS-zero early, then goes
0->1 during CD init. cd_dma_xfer_start (clean, cd.c) had
`DB32(0x801bdea0)=1` — but the GENERATED cd_dma_xfer_start (0x8019B89C)
writes 0x801BDF00 there, NOT 0x801BDEA0. The clean port mistranslated the
store target: 0x801BDEA0 is cd_ext_init_once's one-shot init guard, the only
other writer besides ext_init itself. When a CD DMA read ran before
cd_ext_init_once, it pre-set the guard -> ext_init took the skip path ->
OpenEvent(0xF0000009) never ran -> 0x801BDE38 stayed 0 -> TestEvent(0) spun.

FIXES LAND (all faithful delegations to generated MIPS, committed):
- cd_dma_xfer_start -> br2_call_generated(0x8019b89c): removes the bogus
  guard write AND makes the DMA-start faithful (generated is ~10x larger).
- cd_chan_boot_init -> br2_call_generated(0x8019b220): keeps the whole
  cd_chan_reg_init + cd_ext_init_once chain in generated context (the clean
  cd_ext_init_once->br2_call_generated->FUN_8018e300(syscall EnterCritical)
  nested hop was suspect).
- cd_ext_table_init -> br2_call_generated(0x8019bee0): removed a clean
  cd->generated->clean->generated hop inside generated cd_ext_init_once.

REMAINING / ROOT CAUSE NARROWED (fntrace on fresh boot, build with
cd_chan_boot_init reverted to clean + cd_dma_xfer_start delegated):
- 0x8019B308 (cd_ext_init_once) IS entered once at frame 318, ra=0x80126260.
- Guard 0x801BDEA0 transitions 0->1 => block_8019B31C ran (init path taken).
- Then NOTHING: no FUN_8018e300 entry with ra in 0x8019B3xx, no 0x8019BEE0
  (cd_ext_table_init) entry, no OpenEvent(0xF0000009), 0x801BDE38 stays 0.
- Generated flow confirmed: block_8019B31C sets guard -> CPS jal 0x8018E300
  (FUN_8018e300 = EnterCriticalSection, `syscall` a0=1) ra=0x8019B32C ->
  block_8019B32C -> CPS jal 0x8019BEE0 -> block_8019B338 -> OpenEvent ->
  block_8019B350 stores v0->0x801BDE38.
- Dispatch table DOES map 0x8019B32C/0x8019B338 -> br2eu_func_8019B308 cont.
- psx_syscall EnterCritical (a0=1) returns 0 synchronously (cpu->pc=0 then
  jr ra overwrites) — does NOT context-switch. So the break is NOT the
  syscall yielding.
KEY COMPARISON: cd_hw_chan_init (0x801a0be0, delegated, generated-DIRECT —
called from generated code under the main dispatch loop) ALSO CPS-jals
FUN_8018e300 and its 8 OpenEvents DO fire. cd_ext_init_once is the ONLY one
reached via a NESTED br2_call_generated (clean cd_chan_boot_init -> clean
cd_ext_init_once -> br2_call_generated). Delegating cd_chan_boot_init to
generated was tried: then 0x8019B308 was NEVER entered (generated
br2eu_func_8019B220's CPS path to the ext_init call did not dispatch —
likely its own caller cd_chan_boot_fwd is clean 0x8019b200 so it is STILL
nested one level up).
ROOT CAUSE (best current model): cd_ext_init_once's generated body runs
inside a nested psx_dispatch_call (br2_call_generated). Somewhere between
block_8019B31C's CPS jal to FUN_8018e300 and the continuation resume at
0x8019B32C, the nested-dispatch continuation is lost — the same class of
bug as the frame-1546 gpr-forwarding fix but on the CPS/continuation axis.
The EnterCritical syscall's cpu->pc=0 side-effect or psx_check_interrupts_at
at the continuation boundary is the prime suspect inside the nested frame.
NEXT: instrument psx_dispatch_call to log cpu->pc transitions for the
0x8019B308 call tree (or set a watchpoint on the 0x8019B32C dispatch) to
see exactly where cpu->pc diverges from 0x8019B32C. Alternative: hoist
cd_ext_init_once invocation into generated context by making the whole
cd_chan_boot_fwd->cd_chan_boot_init->cd_ext_init_once chain generated-direct
from its generated caller (find the generated jal site that reaches
0x8019b200/0x8019b220 and ensure no clean wrapper intercepts).

=== DEFINITIVE ROOT CAUSE (dispatch-instrumented, supersedes model above) ===
Instrumented psx_dispatch_impl / dirty_ram_dispatch / psx_check_interrupts_at
with [CDP]/[CDINIT]/[CDIRQ]/[CDBAIL] tags on a fresh boot. Findings:

1. cd_ext_init_once (0x8019B308) IS dispatched, returns pc=0x8018E300
   ra=0x8019B32C (correct CPS continuation), found=1 -- BUT g_psx_call_bail
   is ALREADY 1 at psx_dispatch_game_compiled ENTRY (before its body runs).
   The prior 0x8018E300 dispatch had bail=0, so bail was set in the intervening
   clean->generated window.

2. The SOLE bail-init all run:
     [CDBAIL] init addr=0x801A1AA8 stop=0x80126260 pc=0x00000000
              ra=0x801A1AC4 sp=0x801FFFA8 spc=0x801FFFC0 depth=2 outer=0
   0x801A1AA8 is a CPS continuation inside br2eu_func_801A1A40, an indirect-
   call dispatcher: block_801A1AA8 loads a func ptr into $v0 and `jalr v0`.
   The loaded target was 0 (null table entry at guest 0x801BE768) -> cpu->pc=0
   -> the cpu->pc==0 contract check in psx_dispatch_impl declared wild-flow ->
   g_psx_call_bail=1, cpu->pc=0x801A1AC4 (unwind target).

3. PROPAGATION BUG (fixed in src/clean_bridge.c): br2_call_generated ran
   psx_dispatch_call then UNCONDITIONALLY restored saved cpu->pc/gpr[31]/gpr[29]
   and s0-s3. That clobbered the unwind's cpu->pc target AND let clean C keep
   dispatching fresh guest calls while g_psx_call_bail stayed set. The next
   br2_call_generated (cd_ext_init_once) inherited the stale bail -> its
   psx_dispatch_impl took the bail path and returned instead of tail-
   dispatching 0x8018E300 -> OpenEvent(0xF0000009) never ran -> 0x801BDE38=0
   -> TestEvent(*(0x801bde38)) wedge at frame ~347.

FIX LANDED (src/clean_bridge.c br2_call_generated): bail-aware. If
   g_psx_call_bail is set on ENTRY, return $v0 without dispatching a fresh
   guest call (unwind is in flight; new calls would inherit the stale bail).
   If the callee SETS bail during psx_dispatch_call, return WITHOUT restoring
   saved pc/ra/sp/s0-s3 so the wild-flow target survives to the flatten point.
   Normal path (bail=0) is byte-for-byte unchanged.

REMAINING BLOCKER: the NULL jalr itself. 0x801A1A40 is reached via CPS jal
   from ~0x801A1260 (caller region 0x801A12xx, CD-driver). It reads a func ptr
   from guest table 0x801BE768 which is 0 at that moment. On real hardware a
   jalr to 0 would crash, so the table is expected to be populated earlier --
   likely by cd_ext_table_init (0x8019BEE0) which runs INSIDE cd_ext_init_once,
   i.e. chicken-and-egg: the dispatcher fires before its table exists, OR the
   table is populated by a different init that hasn't run / ran with wrong
   index. NEXT: identify function 0x801A1A40 + caller 0x801A1260, find what
   writes 0x801BE768, and determine correct population order vs the dispatcher.

=== RESOLVED: frame-347 CD wedge (commit pending) ===
ROOT CAUSE (final): clean cd_channel_table_init (0x801a36f0) wrote the
channel fn-ptr table to 0x801c4780+ but generated stores target 0x801BE780+
(a uniform 0x6000 base error). So cd_channel_main_init's callback slot
0x801BE784 stayed NULL -> its `jalr $v0` jumped to 0 -> wild-flow bail ->
cd_ext_init_once's CPS continuation was cut by the stale bail -> OpenEvent
(0xF0000009) never ran -> TestEvent(*(0x801bde38)) wedge @frame~347.
FIX: delegated cd_channel_table_init to generated br2_call_generated
(0x801a36f0) — generator auto-skips the clean wrap (no recursion). Verified:
cd_ext_init_once now runs its FULL CPS chain with bail=0, OpenEvent+EnableEvent
(BIOS B0) fire, cd_ext_table_init runs.
ALSO FIXED: clean_bridge.c br2_call_generated bail-aware (don't clobber unwind
target / don't start fresh calls mid-unwind); save.c ctx_fn_send missing
guest_at() on ctx+0x3c ptr deref.
NEXT BUG (now reachable, was masked by wedge): dev_byte_xfer (save.c:9116)
reads device-struct ptr @0x801be3e4 -> mem_ptr -> wild deref 0x85208510.
Likely uninitialized device/memcard global — init-ordering issue in the device
path, NOT a missing guest_at. Investigate what populates 0x801be3e4.

=== SESSION 24: battle_intro_fsm state-3 freeze — diagnosis (in progress) ===
STATUS: game boots -> loads assets -> runs battle_intro_fsm STATE 3 (correct
steady "fight running" state; no exit by design — match end via
DAT_1f800001->0xf0 upstream). 10+ min, zero crashes, ~90% CPU. BUT all guest
battle state FROZEN: 0x1f800000-0x1f800400 (both fighter ctrls + ctx) and
motion sub-structs 0x801f5328/0x801f5440 byte-identical over 12s+. Not simulating.

FIXED+PUSHED 6ed43cf: unaff_gp ($gp r28) uninitialized locals in 10 battle fns
(task_pools_tick/task_slot_step/task_slot_fns_run +7). Added guest_gp() ->
cpu->gpr[28] (live $gp=0x801beedc). Did NOT unfreeze battle.

CONFIRMED LIVE FACTS (lldb on running proc):
- ctx+0x82 pause gate (0x1f800082) = 0 -> fighter block in battle_frame_tick
  (battle.c:342-346) RUNS: fighter_contact_resolve + FUN_801787dc(=fighter_frame
  #def battle.c:131, resolve.c:1512)x2 + fighter_pair_update.
- battle_frame_tick(0x1f800000,0x1fb) from battle_intro_flow (menu.c:1891).
- fighter ctrl param_1[0]=0x01 (active), &DAT_1f8000e8==guest_at(0x1f8000e8)
  = real RAM (binding correct).
- task_slot_fns_run post-fix: fn-table ptr @(gp+0x20)=guest 0x801beefc ->
  0x801b17b4 valid; pool-1 slots 1,3,8,11 populated -> fns 0x801ac0d4,
  0x801aa47c, 0x801387dc, 0x8013cde8.
- fighter_frame (resolve.c:1512) has NO early return before action_commit
  (resolve.c:1833, unconditional -> ev_stream_dispatch+motion_blend_frame).
  param_1[2]=0 so the goto@1815 skip is not taken.

OPEN HYPOTHESES (tracer subagent aeb9ef21 investigating):
1. fighter_frame/action_commit writes gated by a fighter-state condition or
   land in guest_arena (wrong buffer) via a pointer that resolves there.
2. cd_timer_read -> br2_call_generated(0x8019a460): if the frame/timer counter
   it reads never advances, ALL game timing freezes (matches total stall).
   frame_counter_bump(0x801274c4) only runs in game-state 3/4/5, NOT state-6
   battle_intro_flow -> 0x1f800012 stays 0 (may be normal, not the signal).
3. ~200 unaff_s0-s7 register artifacts (same Ghidra class, context-dependent):
   unaff_s3 = battle_intro_flow fight-duration countdown (1500 attract /
   9000 VS, ->0xf0 at 0); unaff_s2 = match_loop 300-frame attract timeout.
   Both uninitialized locals = UB; need disasm (bloody-roar-2/disasm_fn.py
   0x80167620 / match_loop) to find true storage (global or spilled reg).
4. _g_pad_buffer bound to guest_arena+0 (name has no hex -> guest_for arena
   fallback); runtime entry_0x80126234 sets it to guest_at(sp-0x30) but live
   value stayed arena. Input path suspect (attract demo may not need live pad).

NEXT: await tracer root cause -> fix the exact gate. Then resolve unaff_s2/s3
via disasm. Then verify fighter anim/action/HP actually advance = playable.

=== SESSION 24b: task_pools_tick infinite loop FIXED — game now simulates ===
ROOT CAUSE (subagent aeb9ef21, live-proven): guest_at() NEVER returns NULL
(returns ram+phys for any value, scratchpad, or g_openbus). task_pools_tick
tested the RESOLVED host ptr `while(piVar3!=0)` -> never false -> unconditional
infinite loop on first battle frame -> all state frozen mid-frame, debug
server dead (emu busy). Guest tests RAW next-ptr (beqz/bnez).
FIX (a1c9005): test raw guest values headg/nextg before guest_at; same latent
bug fixed in task_node_alloc/free, quad_fx_update, task_slot_step, fx site.
ALSO 6ed43cf: unaff_gp $gp small-data -> guest_gp()=cpu->gpr[28].

VERIFIED PROGRESS: game now dispatches REAL task fns — task_pools_tick ->
task_invoke -> radial_glow_draw. No more freeze/spin.

NEW BLOCKER: EXC_BREAKPOINT in radial_glow_draw.cold.1 (battle.c:12906 area,
GTE/COP2 glow effect: setCopReg/copFunction/getCopReg + isqrt). isqrt_refine
is pure int math (not the trap). Likely a COP2/GTE function trap or abort —
investigate copFunction/setCopReg/getCopReg impls + cop2_clz. fn_guest arg
was a real task node -> task system works.
NOTE: correct build target is `psx-runtime` (NOT --target BloodyRoar2_Recompiled
which silently no-ops -> stale binary fooled earlier test).

=== SESSION 24c: GTE/COP2 coprocessor is a STUB — the real 3D blocker ===
After task_pools_tick fix, game reaches radial_glow_draw (real task) then
EXC_BREAKPOINT in isqrt.cold path. ROOT: bind.c:878-884 stubs the whole
coprocessor:
  cop_regs[32]/cop_cw[16] only; setCopReg/getCopReg ignore r>=32 (return 0);
  copFunction(f,op) is a NO-OP -> NO GTE computation ever runs.
Consequences: cop2_clz always returns 0 (setCopReg 0xf000 ignored,
getCopReg 0xf800 ->0); RTPS/perspective/LZCR/CLIP/AVSZ/OP/GPF all no-op.
All 3D transform/lighting/camera math is broken -> isqrt/gte_* get garbage,
overflow -> trap. THIS is the core missing subsystem for playable 3D.
NEXT: implement real COP2/GTE (RTPS, NCLIP, AVSZ, OP, GPF, LZCR/HLE-style
faithful per docs) — large faithful-RE task, replaces bind.c stubs. Also
verify whether trap is UBSan (-fsanitize) on int overflow in isqrt input.

=== GTE INTEGRATION SPEC (next session) ===
Clean stubs to replace (game/bind.c:878-884): setCopReg/getCopReg/
copFunction/setCopControlWord/getCopControlWord. These are COP2 semantics
(Ghidra helpers), NOT guest fns — port faithfully, do NOT just call gte_execute
blindly (register `r` uses game's encoding: in_zero/in_at are computed data-reg
selectors; 0xf000/0xf800/0x800/0x4000/0x9800/0xe/0xd are status/control/special
regs). Map each to cpu->gte_data[]/cpu->gte_ctrl[] via gte.cpp's
gte_mtc2/gte_mfc2/gte_ctc2/gte_cfc2 (lines ~1391+, used in gte_execute 1906-38).
copFunction(2,op): op low6 = GTE func (0x180001&0x3f=1=RTPS) -> gte_execute
(g_clean_cpu, op) which imports/exports cpu->gte_data/ctrl. Access g_clean_cpu
via clean_bridge (static there; add accessor or extern). Verify reg-index
mapping against EXE disasm (disasm_fn.py) — game setCopReg reg# != raw gte idx
necessarily. After wiring, radial_glow_draw/isqrt + all gte_* should compute.

=== SESSION — stall #2: post-attract CD file-load re-wedge (i_stat fix landed) ===
FIX LANDED+PUSHED 4baaa3b (runtime/src/cdrom.c): CD IRQ presentation race.
set_irq() now captures the IRQ reason (s_irq_i_stat_reason/_armed) so a fast
HINTSTS poll-ack inside CDROM_IRQ_PRESENT_DELAY cannot zero irq_flag and lose
the owed i_stat bit2 presentation. Level-triggered CD line modeled correctly:
HINTSTS ack drops the source, cannot un-present an already-latched INTC bit.
snap_parse re-derives the pair from restored irq_flag (wire format unchanged).
VERIFIED: CDROM IRQs reach INTC (irqctx istat=0x04); game advances past the
first file-load wedge into the attract loop (g_game_state 3->4->5->6).

REMAINING STALL #2 (same DeliverEvent(0xF0000009) class as Session-18, later):
After attract the game re-wedges at transition_screen_flow ph0 ->
card_sprite_load_sm -> cd_file_load_sm. Live evidence (debug :4400):
- g_game_state=3, phase byte DAT_1f800001=0 (card_sprite_load_sm never returns).
- TestEvent B0/11 FROZEN at 1567926 (blocked inside ONE call = kernel sleep,
  not spinning). DeliverEvent B0/7 FROZEN at 127 (never fires for this load).
- cdrom_state: seq 113787 frozen, pending cmd 0x09 (Pause, read-complete from
  cd_read_finish_cb) active=0, response FIFO drained (read==count==1) =>
  cd_response_pump DID consume the completion via the IRQ path.
- Wait flag DB8(0x801bcb24)=2 (command-done SET) => cd_result_wait is NOT the
  blocker; main thread is in vsync_wait_n (epc 0x8019A648) inside the CD
  channel/kernel thread while the game thread sleeps in TestEvent.
- cd_subsystem_init (0x801974b8) faithfully clears 0x801bc848/84c — VERIFIED
  generated body also stores C848/C84C/C858/C854, so 848=0 is faithful, NOT a
  clean-port bug (earlier suspect ruled out).
- DeliverEvent(0xF0000009,0x20) lives ONLY in cd_dma_chan_reset (0x8019b7e8),
  which does a CD MMIO 0xd5 wait then bios_svc(0xb0,7,0xF0000009,0x20). It is
  NOT in the 0x801be780/0x801c4780 channel fn-ptr tables — invoked via a
  different completion/session path (generated call graph / deferred coroutine).
- cd_read_open registers per-sector cb cd_read_step_cb(0x801985c4) via
  mc_session_b and complete cb cd_read_finish_cb(0x80198838) via
  cd_cmd_dispatch3. cd_read_finish_cb issues async Pause(9) then
  call_guest(0x801bc7b0=cd_bios_cmd40_b=0x80195b9c) -> DeliverEvent(0xF0000003,
  0x40) — a DIFFERENT class than the 0xF0000009 the file-load polls.

OPEN QUESTION (next): why cd_dma_chan_reset / the 0xF0000009 DeliverEvent is
not invoked. Candidates: (a) deferred coroutine cd_ctx_restore (registered
via B0/0x19=HookEntryInt -> mem[0x4C18]) not resuming on CD IRQ so the
session/channel completion handler never runs; (b) the channel/session
completion callback table slot for cd_dma_chan_reset is unpopulated or the
wrong slot (cf. the 0x801BE784 NULL-jalr class of bug); (c) call_guest to
0x801bc7b0/0x801bdee8 boundary arg mismatch. Watch 0x801bdee8 (alt DeliverEvent
cb) + the channel-completion dispatch on a fresh boot to the stall.

=== SESSION — stall #2 ROOT MECHANISM MAPPED (0x801bdf00 channel-reset gate) ===
Traced the full DeliverEvent(0xF0000009) chain live + in generated code:
- cd_ext_init_once -> cd_ext_table_init(0x8019b7e0) registers br2eu_func_8019B7E0
  as the CHANNEL-4 (CD DMA) done handler via 0x8019A6E0(4, 0x8019b7e0).
- cd_dma_xfer_start (0x8019b89c, generated) arms 0x801BDF00 during DMA kick.
- cd_reg_window_op (clean, 0x8019b944): op0 sets 0x801BDF00=a2, op1 clears =0.
  cd_win_shift_op/cd_seek_param_issue/cd_mode_apply_deferred all drive ops
  2/1/3 (or 2/0/3) leaving 0x801BDF00 = 0 after the transfer.
- On channel-4 done, callback 0x8019b7e0 (br2eu_func_8019B7E0) checks
  *(0x801BDF00): !=0 -> MMIO 0xd5 reset + DeliverEvent(0xF0000009,0x20);
  ==0 -> jal cd_delay_3c (0x8019BE78) early-return, NO DeliverEvent.
- LIVE: 0x801BDF00 == 0, all DMA ch idle, response FIFO drained, irq_flag=0,
  cmd-done flag 0x801bcb24=2 SET, TestEvent(0xF1000008) blocked, DeliverEvent
  frozen at 127. => callback ran (or would run) but saw flag==0 -> skipped.
OPEN (next session, decisive): WHY is 0x801BDF00==0 at channel-done callback
time. Two hypotheses: (a) ORDERING — cd_reg_window_op op1 clears the flag in
  the read-complete sequence BEFORE the channel-4 callback checks it (would be
  a faithful-ordering bug in generated dispatch or a clean op-port); (b) the
  file-load DMA path (cd_mode_apply_deferred ops 2/1/3) never arms the flag
  because it uses cd_reg_window_op not cd_dma_xfer_start. NOTE: clean
  cd_dma_chan_reset (fnmap 0x8019b7e8) delivers UNCONDITIONALLY (no flag
  check) — it is the reset BODY for direct 0x8019b7e8 calls; the registered
  callback uses 0x8019b7e0 (flag-gated dispatcher). Do NOT "fix" by removing
  the generated guard — that would diverge from hardware.
  EXPERIMENT: fresh boot, wtrace_arm 0x801BDF00 + fntrace_arm 0x8019b7e0 +
  0x8019b89c + 0x8019b944; run to first file-load; dump ordering of
  [xfer_start set] vs [op1 clear] vs [0x8019b7e0 callback guard-read].
  BIOS NOTE: running openbios.bin (PCSX-Redux HLE-compiled); real SCPH ROMs
  absent. Deferred coroutine (HookEntryInt->EntryInt jmp_buf->ExceptionHandler
  ->IntRP) is a real-kernel mechanism — if openbios ExceptionHandler lacks
  IntRP/EntryInt restore, cd_ctx_restore can't resume; verify against
  generated/OpenBIOS_full.c if the flag ordering proves correct.

=== SESSION — coroutine RESUME RULED OUT; flag never armed (0x801BDF00) ===
Fresh boot (PID relaunch) + fntrace on the whole coroutine chain
(0x8019ADCC ctx_restore / 0x8019A814 chan_full_init / 0x8019A8EC intc_dispatch
/ 0x801979F8 resp_pump / 0x80196370 resp_process / 0x8019B7E0 chan_reset):
- cd_intc_dispatch (0x8019A8EC) is invoked ~36,000x, ALWAYS with
  ra=0x8019A88C (inside cd_chan_full_init), a0=0x801BCD50 (the saved coroutine
  frame), a1=1. => cd_ctx_restore -> cd_chan_full_init -> cd_intc_dispatch
  RESUMES correctly on IRQs. The HookEntryInt->EntryInt->ExceptionHandler->IntRP
  deferred-coroutine contract WORKS under openbios.bin. Hypothesis (a) from the
  prior session (coroutine not resuming) and the openbios-ExceptionHandler
  concern are both RULED OUT.
- cd_dma_chan_reset / br2eu_func_8019B7E0 IS invoked as an INTC-table handler
  on every dispatch, but ALWAYS takes the *(0x801BDF00)==0 branch ->
  jal cd_delay_3c -> NO DeliverEvent(0xF0000009). (verified generated:
  block_8019B7E0 loads *(0x801C0000-8448)=*(0x801BDF00); !=0 -> block_8019B7FC
  MMIO-wait+reset+DeliverEvent; ==0 -> block_8019B7F4 -> 0x8019BE78 delay.)
- wtrace on 0x801BDF00..0x801BDF04 captured ZERO writes all boot. The only
  writers in generated: cd_dma_xfer_start store @0x8019B928 (gpr[2]),
  0x8019B9E8 (gpr[0]=0 clear), 0x8019BA54 (gpr[6]=a2) — all inside the
  cd_dma_xfer_start / 0x8019B954 reg_window family that fntrace shows were
  NEVER invoked this boot.
=> The file-load's CD transfer path that arms 0x801BDF00 never runs. The break
   is UPSTREAM of event delivery: whatever should kick the sector DMA /
   transfer-mode write (arming the flag the channel-reset handler checks) is
   not executing. Command side completes (Pause 0x09 pending.active=0,
   response drained, irq_flag=0, sector 2060/2340 partial, BFRD set) but no
   transfer armed the flag -> handler skips -> TestEvent(0xF1000008) spins.
NEXT (decisive): find which game function SHOULD call cd_dma_xfer_start /
   write 0x801BDF00 for this load and why it isn't reached. Candidates: the
   cd_file_load_sm / session state machine may take a PIO path (no DMA) on
   real HW too — in which case DeliverEvent(0xF0000009) comes from a DIFFERENT
   table handler, and cd_dma_chan_reset is a red herring. Disambiguate by
   checking whether ANY generated site besides 0x8019B7E0 builds 0xF0000009
   for a B0/7 call, and whether the load's sector count actually completed.

## Stall#2 — session N+1 refinement (channel-index correction)

Corrected a mis-analysis: `0x801BDF00` only gates a *delay* in `br2eu_func_8019B7E0`,
NOT the DeliverEvent — both branches converge on `block_8019B7FC`. The real gate is
`0x801BDEE8` (user-callback ptr): `!=0 -> call_guest(cb,0xF0000000,0)`, `==0 ->
DeliverEvent(0xF0000009,0x20)`. Live `0x801BDEE8==0`, so the deliver WOULD fire if the
handler is reached.

Decisive facts this session:
- Event handle `0xF1000008` @`0x801BDE38` = `OpenEvent(0xF0000009,0x20,...)` set up by
  `cd_ext_init_once` (0x8019b308, generated). `TestEvent` is `cd_seek_step`(0x8019d508)
  -> `cd_seek_state_machine`(0x8019d5b0) -> `bios_svc(B0,0x0B,handle)`.
- CD data reads use **DMA channel 3** (`cd_chan_cmd_submit` s0==3 x1108, writes
  0x1F8010B0). DMA ch3 completes (`active:0`).
- `cd_cb7_run` (0x8019af84) walks DICR done-bits -> handler table `0x801BDDF4`,
  cursor +4 per channel index. Table has `0x8019b7e0` ONLY at index 4 (`0x801BDE04`);
  index 3 (`0x801BDE00`, CDROM ch3) is NULL. If handler index == DMA channel, ch3-done
  -> NULL -> no DeliverEvent. OPEN QUESTION: is index-4 registration faithful (CD uses
  a different done channel) or a port/table-population bug.
- `cd_read_finish_cb` (0x80198838) was a clean body in the IRQ/completion family;
  delegated to generated `br2_call_generated(0x80198838)` matching `cd_read_step_cb`.
  Did NOT clear the stall (game still wedges identically) — kept as faithful cleanup.
- Fresh-boot fntrace on the whole load chain returned EMPTY (seq_lo=0): none of
  cd_file_load_sm/cd_read_issue/cd_read_setup/cd_read_poll/cd_dma_xfer_*/
  cd_dma_chan_reset/cd_read_finish_cb were hit — the wedge is a SEEK-path wait, and
  either fntrace misses br2_call_generated dispatches or this op never reaches them.

NEXT: fresh-boot trace of `cd_cb7_run` DICR reads + per-channel handler calls (does
ch3-done fire? which index?) and of the CDROM-IRQ completion path (cd_response_pump
s0&2/s0&4 -> 0x801bc848/84c, both NULL-guarded). Compare channel-index mapping against
the generated `cd_ext_table_init`/`0x8019A6E0` registration to confirm faithful slot.

## Stall#2 — session N+2: cd_cb7_run (channel-done) NEVER invoked

Fresh-boot fntrace (423k records) is decisive:
- cd_file_load_sm(0x8018CFDC)x18, cd_read_issue(0x8018D150)x1 @frame4969 -> read issued to 0x800DC800
- cd_response_process(0x80196370)x143, cd_response_pump(0x801979F8)x94 -> CDROM-IRQ completion processed
- **cd_cb7_run(0x8019AF7C/84) = 0 hits, cd_dma_chan_reset(0x8019B7E0) = 0 hits — the DMA/channel-done path NEVER runs**
- irqctx ring tail: ONLY VBlank IRQs; cdrom_active=0, dma_depth=0 (post-load idle)

cd_intc_dispatch (generated 0x8019A8EC) iterates 11 CD interrupt causes (bit pos 0..10,
`gpr[17]<11`): pending-mask gpr[16] (from 0x801BDDA4=0x1F801070 & CD status) selects which
handler-table slots (base gpr[20]=0x801BCD1C) get jalr'd. Table:
  cause0=0x8019ae68(cb8_dispatch) cause1=0 cause2=0x801979f8(response_pump) cause3=0x8019af7c(cb7_run)
response_pump runs (cause2) but cause3 (channel/DMA done -> cb7_run -> cd_dma_chan_reset ->
DeliverEvent 0xF0000009/0x20) is NEVER pending -> handler skipped -> TestEvent(0xF1000008) spins.

NEXT: find what sets CD pending-mask bit3 (channel-done cause). Likely the CDROM-IRQ/DICR
status path in runtime cdrom.c/dma.c — the DMA ch3 completion must surface as CD cause-3.
Suspect: DMA-ch3 done sets DICR bit27 + raises irq bit3, but the CD driver's cause-3 pending
bit isn't being set (status register not reflecting channel-done), OR this op (seek) produces
no channel-done and the event should come from a different cause.

## Stall#2 — session N+3: REFRAMED — game is past TestEvent, wedged in cd_result_wait

CORRECTION to N+2: the game is NOT spinning TestEvent. bioscall_dump shows
B0/0x0B(TestEvent 0xF1000008) FROZEN at 1,567,926 — the seek-event wait EXITED
(DeliverEvent eventually satisfied it or it timed out). The runtime advanced to a
LATER wait. Live stack (sp=0x801FFF78) reconstructs the real call chain:

  br2eu_func_8018CFDC  = cd_file_load_sm   (load state machine; fntrace x18 earlier)
    -> br2eu_func_80192634 / 0x80196E14   (CD read issue path)
    -> br2eu_func_80195D8C = cd_mode_flag_set (CD command issuer, delegated)
    -> br2eu_func_80196B4C = cd_result_wait_b (delegated)
    -> br2eu_func_801968CC = cd_result_wait_a (delegated)
    -> br2eu_func_8019A460 = cd_timer_read   (delegated)
    -> br2eu_func_8019A5D8 = vsync_wait_n    (delegated, countdown loop)

cd_result_wait_a polls a result flag (set by the INT2 command-complete deferred
coroutine) + a timeout vs free-running timer *(0x801FC6E0). The result flag never
sets -> infinite frame-synced wait -> static screen, Start ignored.

Verified HEALTHY (rules out prior IRQ-corruption theories):
- irqctx_ring: VBlank IRQ taken @0x8019A648 and RESTORED cleanly every frame
  (restored=1 same_thread=1, v0/v1/ra exit==saved). Interrupt machinery works.
- DAT_801bdde0 (0x801BDDE0 frame counter) increments ~400/s — NOT frozen.
- CD subsystem fully idle post-f5286: irq_flag=0, int1_pended=0, i_stat=VBlank
  only, response_read==response_count (no backlog). Last CD cmd = Pause(0x09)
  @frame5286 from pc=0x80196FC4; read of 1 sector @f4969 was the last data op.
- DeliverEvent(B0/7)=127, OpenEvent=19 — event layer fine for earlier ops.

So the break is NARROW: after cd_file_load_sm issues its read/result command,
the INT2 command-complete path (cd_ctx_restore -> cd_intc_dispatch -> result-setter)
does not set the flag cd_result_wait_a/b polls. Reads DID complete earlier
(f3255-f4969) so the path works generally — the specific op sequence ending ~f5286
(Pause then silence) leaves one result unset.

ALSO THIS SESSION: delegated cd_reg_window_op(0x8019b944) -> generated. Its op0/1/3
busy-wait CDMMIO16(0xd3/0xd5) loops use br2_ld16 which does NOT advance guest cycles,
so the clean-C spin could burn the 0xf01 cap in native time before the cycle-driven
CD model moved status bits -> premature -2 -> op3 ch4-DMA kick skipped. Faithful fix
per the MMIO-busy-wait delegation contract; did NOT clear this stall (kept as correct).

NEXT (decisive): identify the exact command cd_mode_flag_set issued last and which
result flag cd_result_wait_a/b polls (read *(0x801bcb25/26) + the param_2 result buf
on the stack @0x801fff40/44 = 0x801bcb9c/0x801bcba4 region). Then trace whether
cd_intc_dispatch ran for that command's INT2 — if it ran but wrote the wrong slot,
it's a result-slot mapping bug; if it never ran, the INT2 wasn't delivered (check
cdrom.c irq_flag->I_STAT bit2 path for THIS command type vs reads).

## Stall#2 — session N+4: CD path fully decoded; NULL-callback theory DISPROVEN

Deep decode of the generated CD interrupt/result path + live MMIO/cmd-history:

cd_intc_dispatch (0x8019A8EC) — fully decoded:
- State block @0x801BCD18: +0 u16 in-service flag (=1 live, healthy), +2 busy,
  handler table u32[11] @0x801BCD1C.
- Gate: lhu in-service; ==0 -> "unexpected interrupt" path (0x8018E550), !=0 ->
  normal dispatch. Runs EVERY frame (caller invokes regardless), finds no CD cause.
- pending mask = *(0x1F801070 I_STAT) & *(0x1F801074 I_MASK) & u16@(0x801BCD48).
  Per set INTC bit i: ACK (*ptrA = ~(1<<i)) then jalr handler_table[i].
  CDROM = INTC bit2 -> cd_response_pump. Re-polls until mask drains.

Live MMIO: I_STAT=0x00000001 (VBlank ONLY), I_MASK=0x0000000D (bit2 CDROM
ENABLED but not pending), CD stat=0x02 request=0x80 mode=0xA0 irq_enable=0x07.
=> CD subsystem is fully IDLE, not broken. No CD IRQ is owed.

cdrom_command_history (128-ring) — the real command sequence:
- Reads are chunked: Setloc(0x02)->ReadN(0x06)->Pause(0x09) repeating, ~1 sector
  per ReadN via INT1 data-ready (cd_read_step_cb x282, last f4982).
- LAST commands: seq314 ReadN f4969 -> seq315/316/317 Pause f4982/f4986/f5286.
- Then ZERO CD commands for 25k+ frames. Every command's INT2 completed
  (pend cleared). The game simply STOPPED issuing reads after f4982 and now
  only re-issues Pause on each cd_result_wait retry.

cdrom_state: pending={cmd:0x09,active:0,phase:1} — last Pause's process_pending
ran (response_clear + push stat + set_irq COMPLETE + fire_cdrom_irq); response
consumed (read==count==1), 0x801BCB24=0x02 (the Pause stat byte). irq_flag=0.
Read-session cells: bcb94(remaining)=0, bcba0(cursor)=8509, bcba4(done)=0.

cd_result_wait_a/b decoded: they are BOUNDED SYNC waits, not the flag poll —
deadline=cd_timer_read+960 -> 0x801FC6E0, spin counter 0x801FC6E4 vs 0x3C0000,
timeout strings 0x801257A4/0x80125834, yield to IRQs each iter via
cd_timer_read->vsync_wait_n. cd_result_wait_b loops wait_a; on timeout it
re-arms and retries (infinite). The actual "result ready" flag is set elsewhere.

DISPROVEN — 0x801BC848/0x801BC84C NULL callbacks: cd_read_setup calls
mc_session(0)/mc_session_b(0) which INTENTIONALLY clear both and stash the old
values in bcba8/bcbac for session restore. NULL is the correct file-load state;
cd_response_pump only invokes them when non-NULL (cd.c:4494-4498). NOT the bug.

REMAINING HYPOTHESIS (narrowed): the reads stopped because the per-chunk read
session reached its sector budget (bcb94=0) and the game is now waiting — inside
cd_file_load_sm's state machine — for either (a) a NEW read session it never
issues because an upstream condition is unmet, or (b) a "load complete / next
chunk" signal that depends on a result flag cd_response_process writes for a
specific response type. Since the CD is idle and every issued command completed,
the missing piece is UPSTREAM of the CD IRQ: cd_file_load_sm is blocked on a
non-CD condition (file-size/byte-count vs delivered, or a different event), OR
cd_result_wait_b's caller interprets a wait_a return status that never becomes
the expected value because the consumed Pause response maps to the wrong slot.

NEXT decisive: read cd_file_load_sm (0x8018CFDC) state machine — find which
state it's parked in and what single condition advances it (byte count? a flag
in the 0x801BCB80 session block? an event?). Cross-check the expected sector/
byte total vs bcba0 cursor=8509 / bytes delivered. The CD side is proven healthy;
the wedge is the load state machine's advance condition.

## Stall#2 — session N+5: session-block decode + sector-size lead

Read-session block 0x801BCB80 (live):
  +0x00=0x28  +0x04=dest 0x800DC800  +0x08=cursor 0x800F0800  +0x0c=mode 0xA0
  +0x10=sector_size 0x200 (512)  +0x14=remaining 0  +0x18/+0x1c=cd_timer_read
  timestamps (4812/4799 — TIME not bytes)  +0x20=LBA cursor 8509  +0x24=done 0

- bcb98/bcb9c are cd_timer_read TIMESTAMPS (cd.c:335/485/525), NOT byte counters —
  the earlier "13-byte gap" was a red herring.
- request_reg=0x80 = CDROM_REQUEST_BFRD (buffered-read request) — normal while a
  read session is armed; cleared on next command. Not obviously stuck.
- cd_file_load_sm (0x8018CFDC) is a SUSPENDABLE coroutine-style state machine —
  dispatch table shows continuation blocks 0x8018D018..0x8018D074+ all -> same fn.
- cd_cmd_issue (0x80196E14) CLEARS the response byte 0x801BCB24 before issuing
  (sb zero @0x80196F3C) then waits for the IRQ to set it. Live 0x801BCB24=0x02
  is SET — so the raw response IS available; the wait is NOT blocked on the
  response byte itself.

LEADING HYPOTHESIS (sector-size / byte-accounting): the read session configured
sector_size=0x200 (512 B). If the file assembly expects a different per-sector
byte count (e.g. full 2048 B Mode1 or 0x246/0x249 XA), the consumer's byte
counter never reaches the expected total even though bcb94 hit 0 -> the load
state machine waits for data that was never requested. Verify: does the cd model
deliver exactly the configured sector_size bytes per ReadN INT1, and does the
game's byte accounting (bcB84 dest cursor advance) match? Compare a WORKING
earlier file-load's sector_size vs this stuck one (was it also 0x200?).

ALT: the wait may be for a result flag written by cd_response_process for a
specific response TYPE (not the Pause stat byte) — check which response code
cd_response_process emits for ReadN-complete vs Pause-complete and which one
cd_result_wait_b's caller actually tests.

## N+6 — frame-348 crash IS the same deadlock; NONDETERMINISTIC race; register-window is SPU not CD

Reproduced a deterministic-looking FATAL "dispatch recursion guard tripped" at
frame ~348 on fresh boot. Traced the runaway: disp_ring = 128x
`0x8019B944`(cd_reg_window_op) <- `0x80156628`, ALL at guest cycle 235749733
(FROZEN — zero cycle advance across 128 dispatches).

Call chain decoded (generated full_19/full_47):
  cd_file_read_seq(0x801565EC)
    -> cd_seek_param_issue_clamped(0x8019D410)   [issue seek]
    -> LOOP: cd_seek_complete_poll(0x8019D500)   [wait seek-done]
         -> cd_seek_state_machine(0x8019D5B0)->cd_state_store(0x8019D5C0)
         -> reads @0x801BDEE8 (seek-state cell)
  cd_seek_complete_poll is DELEGATED to generated specifically so
  psx_check_interrupts_at runs per iter -> deferred coroutine
  (cd_ctx_restore->cd_chan_full_init->cd_intc_dispatch) can resume and
  DeliverEvent(0xF0000009). See game/cd.c:1822-1836.

CRITICAL REFRAME — the "CD register window" is actually SPU:
  @0x801BDEB0 = 0x1F801C00  = SPU register region (memory.c:1127 labels
  0x1F801C00-0x1F801FFF SPU), NOT CDROM (real CD regs are 0x1F801800-3).
  CDMMIO16(blk,0xd5) -> 0x1F801DAA = SPUCNT. The cd_* names in renames.map
  are misleading; this path may be SPU/ADPCM (streamed audio), not file CD.
  Live: ALL SPU regs (DAA/D9C/D9E/DA4/DA6/DA8) read 0x0000 = idle.

NONDETERMINISM PROOF (not a code regression):
  Working binary built ~22:07 (incl 78c4489 cd_reg_window_op delegation).
  Crashing binary 22:40 built from IDENTICAL code — only docs commits
  dfbcfd3+820cd2c landed between; menu.c int16 fix was REVERTED (verified
  baseline ALSO crashes at 348 => pre-existing, not my edit).
  => Same source, different outcome = TIMING RACE in the completion path.

Frozen-cycle anomaly (key unsolved piece):
  128 dispatches of 0x8019B944 all at cycle 235749733. A real MMIO busy-poll
  would advance psx_cycle_count each iter. Frozen => each dispatched body
  returns ~0 cycles. Suspects: (a) g_psx_call_bail stuck set ->
  br2_call_generated returns stale v0 w/o dispatch (clean_bridge.c:380,396);
  (b) psx_check_interrupts_at diverting to a stuck IRQ handler before cycle
  charge; (c) CPS continuation lost across clean<->generated boundary.

Live post-mortem state:
  @0x801BDEE8=0 (seek-state never set), @0x801BDEE4=1, @0x801BDE40=0,
  @0x801BDECC=0 (mode), @0x801BDE38=0xF1000008, @0x801BDEF0=0x07070707.
  pending.cmd=0x09(Pause) active=0; CD stat=0x02 idle; I_STAT=VBlank only.

NEXT: determine why psx_cycle_count freezes — instrument psx_cyc_step /
br2_call_generated bail flag during the runaway, OR check whether the SPU
register-window ops (0xd3/0xd5 -> 0x1F801DA6/DAA) return a value the game
reads as "not ready". The SPU-idle-but-game-waits mismatch is the lead:
game may expect a nonzero SPU status the emulator returns 0 for.

## N+7 — recursion is OUTER coroutine layer, not cd_reg_window_op itself

cd_reg_window_op(0x8019B954) body makes only TWO calls: cd_mmio_strobe_20
(0x8019BE28) and cd_mmio_strobe_22(0x8019BE50) — leaf MMIO-write strobes, no
self-recursion. So the unbounded host-stack recursion (depth>4096, still
tripped at PSX_RECURSION_LIMIT=4096) is NOT inside the register-window op.

Mechanism: the codec sequencer (cd_file_load_sm/cd_file_read_seq ->
cd_seek_complete_poll) is a SUSPENDABLE coroutine. Its CPS continuations are
being RESUMED as nested synchronous psx_dispatch_call frames instead of
yielding back to the frame scheduler / vsync boundary. Each resume nests a
host frame without the prior one returning -> g_psx_dispatch_depth grows
unbounded -> frozen psx_cycle_count (resumed frames return ~0 cycles before
the next nested dispatch). cd_reg_window_op(0x8019B944) is just the innermost
dispatch the ring keeps sampling.

ROOT-CAUSE CLASS: CPS-coroutine re-entrancy in the dispatch layer — a
suspendable generated function's continuation must RETURN control to the
scheduler (so the host frame unwinds each frame), not chain into the next
continuation synchronously. The clean<->generated boundary
(br2_call_generated -> psx_dispatch_call) preserves ra/pc/sp/s0-s3 but does
NOT flatten coroutine resumes, so nested continuations accumulate.

NEXT (needs live instrumentation): find WHERE the codec coroutine's yield
should return to the scheduler — likely the vsync_wait_n / frame boundary —
and why the resume path bypasses it. Compare against how OTHER working
suspendable coroutines (battle_intro_flow, transition_screen_flow) yield vs
this codec path. Possibly a missing "yield to scheduler" CPS opcode or a
continuation PC that points back into the read loop instead of the frame top.

## N+8 — bail path RULED OUT; genuine coroutine self-re-entrancy

Crash log (/tmp/br2_*.log) has ZERO [CDBAIL] lines -> the cpu->pc==0
sp/ra-mismatch bail unwind (OpenBIOS_dispatch.c:9176-9202) is NOT the
mechanism. exc_sp_straddle_ theory disproven for this stall.

Therefore g_psx_dispatch_depth grows via NESTED psx_dispatch_call where each
codec-coroutine resume keeps its parent frame alive: cd_file_load_sm (or the
codec sequencer) is re-entered while its previous continuation is still
suspended on the host stack. The coroutine's yield/resume path uses a nested
dispatch (br2_call_generated or CPS jal into dispatch_call) instead of
returning to the frame scheduler so the host frame unwinds each vsync.

This is the SAME structural class as a fiber/coroutine that must be resumed
by the scheduler (flat, depth resets per frame) but is instead resumed
synchronously inside a still-live parent dispatch -> unbounded host nesting.
The vsync_wait_n / frame boundary is where the resume SHOULD flatten.

FIX DIRECTION: find the codec coroutine's suspend point and ensure its
continuation is dispatched from the TOP-LEVEL frame loop (psx_dispatch, not
nested psx_dispatch_call), matching how working suspendable coroutines yield.
Likely a generated CPS jal targets a dispatcher that calls dispatch_call
recursively rather than returning pc to the outer loop.

## N+9 — crash-report register state: recursion is DeliverEvent(0xF0000009) kernel-call path

psx_last_run_report.json @ frame 348, dispatch_depth=4097:
  ra=0x80156628 (cd_file_read_seq codec path), sp=0x801FFFA0, gp=0x801BEEDC
  epc=0x8019A648 (game IRQ/kernel-tramp region, near cd_dispatch_tbl*/bios_kobj_tramp)
  t2=0xBFC056B4  = OpenBIOS kernel routine (~0xBFC05600 block in OpenBIOS_full.c)
  t3=0xF0000009  = the DeliverEvent event flag the codec coroutine waits for
  im2=1 iec=1    -> an interrupt/exception is live at the trip
  xprobe: mixed_depth_now=4094, first_depth_frame=170 (depth grew f170->f348)

INTERPRETATION: the codec coroutine issues DeliverEvent(0xF0000009) via the
BIOS kernel (0xBFC056B4). The event delivery resumes the waiting coroutine,
which re-issues the SPU op and calls DeliverEvent again before the prior
frame unwinds -> nested psx_dispatch_call cycle -> unbounded depth. The IRQ
(epc in CD/SPU dispatch-tramp region) is entangled: the event is normally
raised BY the CD/SPU interrupt handler; if the handler's DeliverEvent and
the coroutine's re-wait race, the coroutine never observes the event as
consumed and re-arms forever.

NEXT: trace OpenBIOS 0xBFC056B4 (the kernel DeliverEvent/syscall body) and
confirm whether the codec coroutine's event-wait flag (@0x801BDEE8 region /
TestEvent) is cleared on delivery. The coroutine must see the event ONCE;
if the flag isn't cleared or the coroutine re-suspends on the same event,
it recurses.

## N+10 — dirty_block_tail: the recursing kernel routine is OpenBIOS 0x00002914-0x00002964

dirty_block_tail (frame 348) shows a tight loop through targets
0x00002914/0x00002924/0x00002934/0x0000293C/0x00002944/0x0000294C/0x00002954/
0x00002964 — a ~0x50-byte OpenBIOS kernel routine (OpenBIOS_full.c:222967+,
kernel mapped at LOW addrs not 0xBFC0). Called repeatedly with:
  a0 = 0xF0000001 / 0xF0000009   (kernel event flags)
  a1 = 0x00001000 / 0x0000FFFF
  sp = 0x00008548                (separate low/kernel stack, NOT 0x801FFxxx)
Last entry transitions back to game: tgt 0x000000B0, ra=0x8019AA9C
(in cd_chan region just before cd_chan_slot_set 0x8019aabc), sp=0x801BDD08.

So: game CD/SPU codec code (~0x8019AA98) invokes an OpenBIOS kernel
event primitive (DeliverEvent/TestEvent family) on a kernel stack; the
primitive + the codec coroutine's event-wait recurse through nested
psx_dispatch_call. The 0xF0000001 AND 0xF0000009 flags both appear ->
multiple event deliveries interleaving.

NEXT: identify the OpenBIOS routine at 0x00002914 (read OpenBIOS_full.c
~222967) — likely DeliverEvent or TestEvent — and check its re-entrancy
contract vs the game's event-wait flag clearing.

## N+11 — the recursing routine is the OpenBIOS THREAD-SCHEDULER context-restore

The dispatch-0x00002914 block maps to BIOS ~0xBFC208D0-0xBFC20964. Its
prologue is unambiguous:
  0xBFC208D0  lw $sp, 19332($sp)   <- restore SP from saved-context offset
  0xBFC208D4  lw $s3, 0($s3)
  0xBFC208D8  lui $gp,0 / addiu $gp
  0xBFC208E0  or  $fp,$sp,$zero
  0xBFC208E4  addi $s4,$s3,32
  0xBFC208E8  lw $s6,0($s3) / beq $s6,$zero  <- scan saved ctx / wait-flag

This is the OpenBIOS cooperative-thread SCHEDULER's context save/restore +
wait-flag scan — i.e. the thread RESUME / WaitEventFlag path. The codec
operation runs on a kernel THREAD; when it waits it yields to the scheduler,
and DeliverEvent/the CD-SPU IRQ wakes it -> scheduler resumes the thread.

ROOT CAUSE (final form): the thread RESUME is being dispatched as a NESTED
psx_dispatch_call inside the waker's (IRQ handler / DeliverEvent) frame,
instead of returning to the top-level scheduler loop so the resume frame
unwinds. Each wake nests a scheduler context-restore -> g_psx_dispatch_depth
grows ~23/frame, frozen cycle, guard trips. Same class as the documented
MoH/Driver-2 "IRQ at return boundary swaps sp" freezes but here it is the
THREAD context-switch sp (0x8548 kernel stack) that the return-boundary gate
can't recognize as a legitimate scheduler resume.

FIX DIRECTION: thread resumes must go through the top-level psx_dispatch
(frame loop), not nested psx_dispatch_call. Find where the OpenBIOS
scheduler's resume/DeliverEvent issues the nested dispatch_call for the
woken thread and route it to the flat dispatcher — OR extend the
return-boundary gate to recognize the kernel-stack (sp~0x8548) context
restore as a legitimate resume, mirroring exc_sp_straddle_.

## N+12 — LIVE-CONFIRMED mechanism: IRQ/DeliverEvent re-entrancy via psx_dispatch_call

Instrumented psx_dispatch_call (temporary fprintf, since reverted) and ran to
the frame-348 crash. The [NEST] trace shows EVERY nested call identical:
  addr=0x8019B944 (cd_reg_window_op)  ra=0x80156628 (cd_file_read_seq)
  sp=0x801FFFA0 (CONSTANT)  pc=0x00000000 (CONSTANT)
Constant sp/ra/pc => NOT guest-stack recursion; the SAME call is re-entered
via interrupt/event resume, each entry nesting a host psx_dispatch_call frame.

CONFIRMED CHAIN:
  cd_reg_window_op generated body -> MMIO strobe (cd_mmio_strobe_20/22)
    -> psx_check_interrupts_at -> pending IRQ (I_STAT nonzero during runaway)
    -> IRQ handler -> DeliverEvent(0xF0000009) kernel path
    -> resumes codec coroutine via psx_dispatch_call  [NESTED, depth+1]
    -> coroutine re-enters cd_file_read_seq -> cd_reg_window_op
    -> strobe -> psx_check_interrupts_at -> IRQ STILL pending -> repeat

KEY GAP: interrupt delivery is gated by in_exception / exception_nest_depth
(interrupts.c:1190,1270,1462) AND hw_deliverable requires SR.IE (bit10,
interrupts.c:1513). BUT the DeliverEvent user-callback resume uses
psx_dispatch_call (a kernel-call, NOT an exception take) which does NOT set
in_exception and does not clear/ack the pending i_stat bit. So the resumed
coroutine's MMIO strobe sees the SAME still-pending IRQ and re-delivers it,
recursing through nested psx_dispatch_call frames that never unwind.

On real hardware the IRQ would be acked (handler writes I_STAT) or IE masked
during the coroutine resume, so the strobe wouldn't re-fire the same source.

FIX DIRECTION (faithful): ensure the pending IRQ i_stat bit is acked/cleared
(or IE/in_exception covers the DeliverEvent callback window) BEFORE the
resumed coroutine's MMIO can re-observe it — i.e. the IRQ handler must ack
its source before DeliverEvent resumes the waiter, matching hardware where
the exception handler clears the cause before dispatching the event.
Verify which i_stat bit (CD bit2? SPU? VBlank?) stays pending and whether
the handler's ack write is being skipped because the coroutine resume
longjmps/returns before reaching it.

## N+13 — ROOT CAUSE FOUND + FIXED (commit 14b83f4): stale clean_table thunk wrappers

The N+12 hypothesis (DeliverEvent resume re-entrancy) was WRONG. Live [NESTCYC]
ring instrumentation (recorded addr/return_addr/depth into a 96-entry ring,
dumped at depth>=240) proved the recursion is a clean<->generated PING-PONG,
not an event-resume:

  EVERY nested psx_dispatch_call was byte-identical:
    addr=0x8019B944 (cd_reg_window_op)  ra=0x80156628 (cd_file_read_seq)
    in_exception=0  sp=0x801FFFA0 (main stack)  pc=0
  depth OSCILLATED 181->212->181->240 (bounded re-entrancy ratcheting, not
  monotonic stack growth) — frames DO unwind between IRQ re-deliveries.

ROOT CAUSE: src/clean_table.c was STALE. It still contained wrap_* entries
for cd_read_finish_cb (0x80198838) and cd_reg_window_op (0x8019B944) — both
converted to pure `return br2_call_generated(...)` thunks (the bodies
busy-wait on live CD drive status and need generated per-instruction cycle
accounting). gen_clean_table.py's thunk-skip (line ~50) correctly excludes
br2_call_generated bodies, but the checked-in table predated the delegation.

So every guest `jal 0x8019B944` routed: generated caller -> dispatch ->
wrap_8019b944 (clean) -> clean cd_reg_window_op -> br2_call_generated ->
psx_dispatch_call [depth+1] -> generated body -> MMIO -> psx_check_interrupts_at
-> level-triggered CD IRQ (irq_flag held until 0x1F801803.1 ack) -> handler ->
codec work -> cd_reg_window_op again -> recurse. The clean<->generated
round-trip added a host frame per codec register-window op (3x per
cd_seek_param_issue), and each nested MMIO boundary re-observed the still-
asserted CD IRQ.

FIX: regenerated clean_table.c via `python3 game/gen_clean_table.py` — both
stale wrappers dropped (1118 wrappers, 75 skipped). Generated callers now
dispatch straight to the generated bodies via flat CPS (cpu->pc tail-
dispatch inside psx_dispatch_impl's loop), so no clean boundary, no nested
psx_dispatch_call frame, no recursion.

VERIFIED: two independent clean-build runs both pass frame 348 and climb
past 14000 frames (GPU gp0_draw/gp0_env active, LZSS decomp live), zero
recursion-guard trips. Commit 14b83f4.

REMAINING (separate bug): game runs deep but display is BLACK — CD sector
drain still lossy (bfrd_set=5867 vs bfrd_clear=14, ring_dropped=2591).
Fighter file 0xb1 load / sector-buffer-ready handshake needs its own trace.

## N+14 — STATE-3 BLACK-SCREEN STALL ROOT-CAUSED + FIXED (commit 503c627)

After N+13 the game ran deep but sat at g_game_state=3 forever on black.
Live scratchpad reads localized it:

  g_game_state @0x1F800000 = 3  (stuck, never reached 4/5)
  DAT_1f800001 phase byte = 0   (transition_screen_flow loop never exits)
  *(int16_t*)(0x1F80008C) = 0x0100  (256 -- UNRECOGNIZED load state)

CHAIN: state3 -> transition_screen_flow(3) -> card_sprite_load_sm ->
card_sprite_load_sm reads s1=lh(0x1f80008c). Valid states are -1/0/0x10;
0x0100 hits `s1!=0x10 -> return 0` BEFORE ever calling cd_file_load_sm, so
DAT_1f800001 stays 0 and the do{}while(frame_tick) spins forever.

ROOT CAUSE -- width-confused clean-port global. _DAT_1f80008c is shared
scratchpad accessed at THREE widths in the original:
  transition_screen_flow reset  : `sh $0,0x8C`  (HALFWORD, clears 2 bytes)
  card_sprite_load_sm / intro_movie_state : `lh`/`sh` (int16)
  battle_intro_flow counter     : `lbu`/`sb`   (BYTE)
The clean port defined it uint8_t in menu.c, so `_DAT_1f80008c = 0`
emitted a BYTE store clearing only 0x1f80008c. A prior uint32 frame
counter (battle_intro_flow, faithfully wider) had left 0x01 at 0x1f80008d
(value 0x00000100=256 frames). The byte-reset left the stale high byte ->
int16 read = 0x0100 -> card_sprite_load_sm wedged.

FIX: menu.c `#define _DAT_1f80008c` uint8_t -> int16_t, so resets write
`sh`-equivalent (2 bytes) matching the original. game.c still has
uint32_t _DAT_1f80008c/DAT_1f80008c macros and battle.c uses the field in
other widths -- audit pending if a later stall appears.

VERIFIED: game now progresses 3->4->5->6 (battle_intro_flow), cycles the
attract/demo loop, DAT_1f800001 reaches 0xf1/0xff. File 0x1D (the
previously-stuck transition sprite) now loads to 0x800F0000.

## N+15 — NEXT BLOCKER: battle submits draws but framebuffer is flat color

State 6 (battle) reached and the GPU is BUSY: gp0_draw=284143,
gp0_env=329792, gpu_opcodes histogram shows 0x2D=157718, 0x64=92708
(sprites), 0x2C=15219 (gouraud quads), draw_area=[0,0,319,239].
DAT_801c43a8 orbit table IS populated (valid 0x8001xxxx ptrs) -- the
aux_tbl_store_a/b weak no-ops in bind.c:2012 did NOT leave it empty
(generated dispatch populates it); deprioritize that theory.

YET the displayed framebuffer is a SINGLE flat color ((0,16,40) dark blue
during battle, (248,248,248) during a fade). VRAM has non-zero content
(vram_peek 0x296c pattern). So primitives are submitted but produce no
visible variation -> candidates: degenerate/clipped prims, wrong
CLUT/texture page, OT entries pointing at bad prim data, or the GPU
walking an OT whose leaf tags are stale. NEXT: dump the live OT leaf
chain + a few prim headers during state 6 and check coords/uv/clut;
verify the framebuffer the GPU writes is the one screenshot/present reads
(display_y=240 observed -- check which 240-line half is shown).

## N+16 — flat-color battle: render pipeline traced to rasterize stage

Follow-up forensics on the N+15 flat framebuffer (game now reaches state 6,
GPU busy, OT populated, textures in VRAM, primitives built at 0x801e6210+
with env cmds 0xE3/0xE4/0xE5/0x0E + vertex/UV/color words):

- OT entries 0x001E6xxx are VALID: low-21-bit RAM offsets
  (0x801E6288 & 0x1FFFFF = 0x1E6288); gpu.c:1811 ORs 0x80000000 back,
  gpu.c:5000 masks to RAM range. NOT a pointer-truncation bug.
- CPU-side VRAM (gpu_vram_peek, x/y pixel coords) shows flat 0x296c across
  BOTH displayed pages (display_y=240 -> visible band y=240..479, x=0..319)
  AND x=0..319 of page0. BUT the renderer is OpenGL (FBO); CPU VRAM is only
  a shadow, so flat CPU VRAM != proof of flat GPU output.
- gl_fbo_peek returns "GL pipeline inactive" in headless mode, so the real
  GPU-side framebuffer can't be peeked directly; screenshot_file reads the
  true presented output and it IS a single flat color that CHANGES per
  attract scene ((0,16,40)->(99,90,82)->white fades).

CONCLUSION: stage/backdrop color varies per scene (so the display path and
per-frame present DO update), primitives+textures exist, yet no geometry
detail rasterizes. Most likely a SYSTEMIC issue, not fighter-specific:
  (a) GTE/projection produces off-screen/degenerate coords for ALL prims
      (camera/projection matrix or GTE transform broken) -> only the
      fullscreen backdrop/clear survives;
  (b) primitive packet format mismatch so the GPU walker drops/misreads
      every draw prim (env cmds parse, draw cmds don't);
  (c) draw-area/scissor or OT-leaf-tag interpretation clips everything.
NEXT: oracle-diff one frame's GPU command stream + rasterized pixels vs
DuckStation/Beetle (the project already keeps a psx-spx oracle), OR decode
a single sprite/poly prim header the GPU walker consumes and check its
coords/CLUT/UV against sane ranges. A GTE sanity check (transform a known
vertex, verify screen-space result) would quickly separate (a) from (b).

## N+17 — flat battle ROOT-CAUSED: phase-2 exits immediately (P2 fighter byte0=0x81)

The flat framebuffer is NOT primarily a rasterize bug. Live scratchpad
dump (read_ram 0x1f800000, state 6):

- g_game_state=6, DAT_1f800001=0xf1, divisor byte 0x1f800019=0x21 (stuck).
- P1 fighter struct @0x1f8000e8: byte0=0x01, byte1=0x11.
- P2 fighter struct @0x1f8001f0: byte0=0x81, byte1=0x12.

battle_intro_flow phase-2 body (menu.c ~L1895) runs EVERY frame:
    _DAT_1f80008e--;
    if ((DAT_1f8000e8 != 1) || (DAT_1f8001f0 != 1)) DAT_1f800001 = 0xf0;
    if (bVar1) battle_frame_tick(...);
So phase 2 -> 0xf0 on the FIRST frame because P2 byte0=0x81 != 1.
battle_frame_tick (the GTE/prim path) never gets a real fight frame ->
gte_verts stays ~1 -> no fighter geometry -> flat backdrop only.

VERIFIED the check is FAITHFUL, not a mis-port. Generated
battle_intro_flow @0x80167330 (SLES_017.22_full_27.c):
  0x801676E8 lbu $3,0($18)   ; fighterA.byte0
  0x801676F0 bne $3,$21 ->0xf0 ; $21=1 (li at 0x8016741c)
  0x801676F8 lbu $2,0($17)   ; fighterB.byte0
  0x80167700 beq $2,$3 ->cont ; continue only if B==A
=> continue iff A==1 && B==A == A==1 && B==1. Identical to clean port.

byte0 = fighter action/state byte. action_enter (action.c ~L668) writes
GS8(ctrl,0x00)=1 only for action class cls==1; P1=0x01 = active/idle,
P2=0x81 = 0x01|0x80 = active state + 0x80 flag. The 0x80 bit is OR'd on
top of a valid low state somewhere in the fighter state machine
(battle.c fighter update / KO / spawn-intro). It is NOT a permanent P2
marker (the original would never fight otherwise) and NOT an loader
error flag (save_state_machine_alt returns 1 without touching byte0).

NEXT: find the writer that ORs 0x80 into fighter byte0 (offset 0 of the
0x1f8000e8/0x1f8001f0 ctrl blocks). Likely the fighter spawn/intro or
first-hit state that never completes because P2's .ANI/.ACT/model or its
action stream didn't load/advance -> fighter stuck in a non-idle action.
Trace fighter_anim_advance / the per-frame fighter ctrl update for the
byte0 store, and check whether P2's animation section is present/valid.
If P2's anim data is missing, that is the real asset-load bug to fix;
forcing byte0=1 would only mask it (model still wouldn't render).

SECONDARY (intermittent): after N attract battles the game wedges at
DAT_1f800001=0xf1 with 0x1f800019 frozen nonzero. frame_divisor_get reads
that byte; the decrement lives in the 0x8012A9xx frame-timing module +
cd_timer_read's vsync_wait_n which spins on DAT_801bdde0 (CD coroutine
counter) advanced only via CD IRQ -> cd_cb8_dispatch. When pending IRQs
stall (i_stat frozen 0x05), the counter freezes -> 0xf1 never exits.
Same deferred-IRQ/coroutine family as the frame-348 fix; reproduce by
letting attract run ~70k frames then inspect i_stat/CD coroutine.

## N+17b — byte0 0x80 bit = fighter_script_fsm DISABLE flag (P2 frozen)

fighter_state_tick (battle.c:3212, guest 0x8017fb34) reads the fighter
ctrl block as uint32*: param_1[0] = bytes 0-3. It tests:
  (*param_1 & 0x80)   -> byte0 bit7
  (*param_1 & 0x8080) -> byte0 bit7 | byte1 bit7
When (*param_1 & 0x8080) != 0 it SKIPS fighter_script_fsm() — the per-
frame action/state machine that advances the fighter's current action.

Live P2 ctrl @0x1f8001f0: byte0=0x81 (0x01|0x80), byte1=0x12.
=> byte0 bit7 set => P2's fighter_script_fsm is DISABLED => P2 cannot
   act/animate -> stays in a non-idle state -> phase-2 check (byte0!=1)
   exits the round on frame 1 -> no fight -> flat screen.

byte1 is the slot marker (P1=0x11, P2=0x12); byte2=player index
(P1=0,P2=1); byte3=char/form id (P1=5,P2=8). P1=0x01 runs the FSM.

action_enter (action.c) sets GS8(ctrl,0)=1 for BOTH cls==1 and the
else-branch, so byte0=1 is the normal post-entry value; the 0x80 bit is
OR'd on afterward by whatever marks the slot "externally driven /
non-script". NEXT: find that writer. Prime suspects: the attract/demo
input-replay path (CPU/demo fighters may legitimately run with the FSM
disabled and be fed recorded actions), a fighter KO/down/dead-state
setter, or fighter_record_apply marking an unloadable char. If attract
is SUPPOSED to show byte0=1 for both (real CPU-vs-CPU fight), then the
bug is that P2's FSM-disable is being set erroneously during spawn —
check whether P2's .ANI/.ACT stream or AI/input record loaded.
