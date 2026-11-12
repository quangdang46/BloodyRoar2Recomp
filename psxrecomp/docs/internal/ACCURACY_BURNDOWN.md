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
