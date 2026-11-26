# Faithful Timing Core — Game Plan (psxrecomp)

**READ THIS EACH SESSION.** Referenced from CLAUDE.md Rule -1 and from the
auto-memory ([[psxrecomp-build-faithful-core-not-hacks]],
[[precise_irq_slice_state]]). This is the authoritative plan; update the
"Status / Log" section every session.

---

## 0. North star + guardrails (non-negotiable)

Build the **faithful hardware-timing core** of the static recompiler. The PSX
recompiler is being BUILT, not preserved:

- The correct fix is ALWAYS the faithful, class-level core — NEVER a surgical
  per-game patch, symptom workaround, `game.toml` hack, or "make native agree
  with interp even if both are fake."
- Breaking other titles is acceptable; they were built on a faulty ecosystem and
  will be **regenerated**. Backward-compat is NOT a constraint.
- No stubs, no HLE, no interpreter-as-fallback (Architecture A locked). Fix the
  recompiler/runtime and regenerate; never edit `generated/*.c`.
- Don't guess (PRINCIPLES). Confirm every mechanism with the oracle + rings
  BEFORE changing code. Build observability first.
- Confer with ChatGPT via the **Chrome MCP browser at chatgpt.com** — the
  existing "PSX Static Recompiler Debug" chat (the user has Plus logged in).
  NOT the `codex` CLI (usage-limited).

## 1. The problem (diagnosis, confirmed)

A static recompiler charges cycles **block-granular** (instruction count up front
at each block leader) and checks IRQs at **block edges**. Real HW and the
sanctioned dirty-RAM interpreter have a **per-instruction** cycle timeline and
take IRQs at the exact instruction. Games that read timers / poll IRQ-driven
flags in tight loops fork between backends.

Tomba 2 (SCUS-94454) logo→FMV stall is the canonical case. The cascade:
1. Timer1 debounce value-fork @ pc 0x8008592C (frame 1823) — fixed by exact block
   cycle costs ("Fix A", already in tree).
2. **CURRENT BLOCKER:** measured **−8 cycle drift** (native BEHIND interp),
   entering in the BIOS→overlay init transition (func 0x80050B0C subtree). The
   frame-1824 logo-delay wait loop (caller 0x8008AE48 → RCnt reader 0x80085900;
   exits when *0x80102748[=960] < elapsed Timer1) loops ~1557× in interp (reaches
   FMV) vs ~42× native (stuck on logo).
   **Mechanism (located in code_generator.cpp):** when a branch's delay slot is
   ALSO a block leader (`exit_has_delay && !delay_slot_in_block`, ~line 1243), the
   branch block's `instruction_count` excludes the delay slot; on the TAKEN path
   the delay-slot clone runs but its cycle is charged by neither the branch block
   nor the (unentered) delay-slot block → undercount 1/site. ~8 sites = −8.
3. Interrupt take-point granularity (block-edge vs exact instruction) — the
   "precise IRQ slicing" track. PARKED (default off); it is a later correctness
   upgrade, NOT the FMV blocker. Validated design exists (block-leader
   continuations); see §5.

## 2. The target architecture (what "faithful core" means)

Per ChatGPT (validated) + standard practice:
- ONE shared **per-instruction cycle-cost function** `psx_instr_base_cycles(pc,
  insn)` used by BOTH the dirty-interp and the recompiler. No two approximate
  models.
- Recompiler emits **exact** accumulated cycle charges (collapses to a constant
  per pure-compute block); every dynamically executed instruction charged exactly
  once; delay slots owned by the branch bundle.
- **Segmented charge** before any guest-visible time observation (MMIO read/write
  to timers/GPUSTAT/SPUSTAT/DMA/CD/I_STAT/I_MASK, BIOS/device calls, backedges,
  calls/returns) so native and interp observe devices at the same architectural
  boundary.
- **Timers derived on-demand** from a global guest-cycle counter at read time;
  DMA/CD/GPU/IRQ on **scheduled event deadlines** (not per-cycle ticking) so
  compiled stays fast.
- Invariant: *every execution backend may differ in host implementation, but not
  in guest-visible time.* At same-PC convergence points, native cycle total ==
  interp cycle total.
- pc=0 means ONLY a real guest pc=0 / explicit termination — NEVER "dispatcher
  couldn't re-enter." Fail closed + log on undispatchable PCs.

## 3. Phased plan (each phase: confirm → build → regen → run → measure → screenshot)

- **P1 — Cycle-audit observability.** Add a per-function/at-convergence cycle
  audit: record native vs interp cumulative guest cycles at same-PC points; expose
  via TCP/ring. SUCCESS: reproduces the flat −8 and pinpoints the entering site(s).
- **P2 — Delay-slot cycle ownership (the −8).** Fix in code_generator.cpp: branch
  bundle charges its delay slot; not-taken fallthrough → branch_pc+8 (not the
  delay-slot leader); delay-slot-as-standalone-leader charges itself; no
  double-count on the not-taken path. SUCCESS: audit shows −8 → 0; Tomba 2 reaches
  the intro FMV (screenshot). Likely the FMV unblock.
- **P3 — Shared per-instruction cost function.** Single `psx_instr_base_cycles`
  consumed by both backends; recompiler emits exact accumulated charges with
  MMIO/boundary segmentation. SUCCESS: first-divergence hashes identical past frame
  1824 across a longer run; audit stays 0.
- **P4 — On-demand timers + event deadlines.** Timer1/2/0 computed from global
  cycle counter at read; devices on scheduled deadlines. SUCCESS: no perf
  regression; timing-sensitive paths stable.
- **P5 — Precise take-points (fold in parked work).** Re-enable slicing; emit
  EVERY block leader as a CPS continuation (global dispatch → owning func w/
  cpu->pc; never a new entry; fail-closed on undispatchable). SUCCESS: exact-
  instruction IRQ delivery with bounded (one-block) hand-back.
- **P6 — Regression + faithfulness.** Regen + screenshot-smoke ALL titles (BIOS,
  Tomba 1, MMX6, Ape, Tomba 2); delete Tomba2 `overlay_native_block` (must still
  reach FMV/title); calibrate the shared model against Beetle/psx-spx. Pin bump is
  user-gated.

## 3b. Cycle-cost model SOURCE (no clean-room needed — transcribe + verify)

The HW-intended cycle model is documented AND available as reference source we
already have in-tree (our oracle's own code). Stage-2 = transcribe the NUMBERS
(facts, not GPL-protected expression; also in psx-spx) into OUR shared cost
function, then VERIFY each against Beetle at runtime. Do NOT paste Beetle code
(architecture differs + GPLv2 hygiene); write our own informed by the facts.

Extraction map — `psxrecomp/beetle-psx/mednafen/psx/` (main checkout):
- **CPU base / instruction fetch:** cpu.cpp `ReadInstruction()` (~L534) — icache
  model: `timestamp += 4` cache-disabled (0xA000_0000+), `+3` on cache miss/fill,
  `+1` per fill word, near-0 on hit. For a static recompiler this becomes a
  per-block fetch-cost constant (assume cache-enabled steady state; calibrate).
- **Memory wait-states:** cpu.cpp `ReadMemory()` (~L365) / `WriteMemory()` (~L454)
  — `timestamp += (ReadFudge>>4)&2`, the `lts` delta from `PSX_MemRead*` is the
  region wait-state; `LDAbsorb = lts - timestamp` is the load-delay absorb. Charge
  in OUR psx_read/write path by region (RAM fast, BIOS ROM slow, scratchpad fast,
  MMIO per-device). Split clean from CPU base (don't double-count).
- **Mult/Div latency:** cpu.cpp `MULT_Tab24` (~L101), `muldiv_ts_done` (~L154) —
  mult/div set a completion timestamp; a later MFHI/MFLO stalls until then. Model
  as a documented latency (mult ~6-13 by operand magnitude via MULT_Tab; div/divu
  ~36). Encode as instruction cost + optional stall-on-read.
- **GTE/COP2 per-command cycles:** gte.cpp `GTE_Instruction()` (L1713) returns the
  count via each op fn (DPCS/MVMVA/NCDS/…). Well-known table (also psx-spx):
  RTPS=15 RTPT=23 MVMVA=8 SQR=5 OP=6 AVSZ3=5 AVSZ4=6 NCLIP=8 NCDS=19 NCDT=44
  NCCS=17 NCCT=39 NCS=14 NCT=30 CC=11 CCS? CDP=13 DPCS=8 DPCT=17 DCPL=8 INTPL=8
  GPF=5 GPL=5 (verify each against gte.cpp op-fn returns + psx-spx before use).
- **Timers (already partly faithful):** timer.cpp — divider ratios already used
  (T1 hblank ÷2146 etc.); move to on-demand counter = f(global cycles) at read.

Build order for the model (P3 → Stage-2):
1. Shared header (single source of truth) consumed by interp (runtime) AND
   recompiler (it already includes ../../runtime/include/*.h): identity first
   (cost=1) → regen → prove byte-identical generated cycle charges (zero behaviour
   change) → seam established.
2. Fill real costs from the extraction map, ONE component at a time, each verified
   against Beetle at runtime (native cumulative cycles == Beetle at convergence).
3. Memory wait-states in the psx_read/write path (region table).
DO each transcription with the Beetle source open + a runtime cross-check; a wrong
cycle number CREATES divergence, so verify, don't rush.

## 3c. STAGE 2 — full hardware cycle accuracy (the goal; -8 is DONE/past)

The -8 was backend-disagreement (native vs our interp); FIXED (FMV reached). Stage 2
makes the cycle model match REAL R3000A timing, validated against Beetle. We are NOT
hardware-cycle-accurate yet: model is ~1 cycle/instruction; Beetle charges ~2x.

### The validation breakthrough: DELTA comparison (offset-independent)
Absolute-cycle comparison through boot is meaningless (native is ~121M cycles off
Beetle due to turbo-loads/overlay load-model differences). BUT the cyc_watch
comparator's per-hit DELTAS cancel that offset: between two consecutive hits of the
same anchor (one iteration of identical code), native charged 46 cycles vs Beetle 91
(@0x80017FC4). That ~2x gap IS the cycle-model inaccuracy, measured cleanly. So:
  VALIDATE STAGE 2 BY MATCHING native Δcycles == Beetle Δcycles over identical
  regions (consecutive same-anchor hits, or entry/exit anchor pairs), NOT absolute.
First concrete target: make the 0x80017FC4 inter-hit Δ 46 -> 91 (== Beetle).

### Stage-2 progress log
- #1a data-load cost DONE (2ef47bd): psx_instr_base_cycles +2 per CPU load (LWC2 +1).
  Δ gate @0x80017FC4: native per-iter 46 -> 56 (Beetle 91). FMV still streams (no
  regression). Closed ~10/45. Approximation: no scratchpad-free / region / load-delay
  ABSORB yet — those are refinements (absorb would LOWER native, so it's not the
  remaining 35; the remaining gap is other components below).
- REMAINING ~35 cyc: DISASSEMBLED func_80017FC4 — it is only loads/stores/ALU/branches
  + a countdown delay loop; NO mult/div/GTE/MMIO. So the gap is NOT those, for this fn.
  BUT func_80017FC4 exits via a CPS TAIL-CALL to 0x8001EFFC (no normal return), so the
  single-anchor entry-to-next-entry window SPANS MULTIPLE functions (80017FC4 ->
  8001EFFC -> ... -> re-call). => single-anchor Δ is TOO COARSE for per-component
  attribution; the 56/91 covers code we haven't disassembled.
- TOOLING NEXT (before more cost components): add a TWO-ANCHOR region mode to cyc_watch
  (capture cycles at region START anchor A and END anchor B; report Δ(B−A) per pass) on
  BOTH backends. Then validate the cost model on a KNOWN, fully-disassembled single
  code path (no calls/loops crossing out) — e.g. a leaf function entry→its terminator.
  That gives rigorous per-component attribution instead of an opaque multi-fn window.
  Only then resume adding components (fetch / mult-div-stall / GTE / load-absorb).

### Components to transcribe (from in-tree Beetle + psx-spx; verify each by Δ)
The ~2x gap is dominated by what 1/insn ignores. Implement one at a time, re-measure Δ:
1. **Memory access wait-states (biggest lever).** Real loads/stores cost >1 cycle by
   region (RAM/BIOS-ROM/scratchpad/MMIO). Beetle: cpu.cpp ReadMemory `lts` delta +
   LDAbsorb (load-delay). Charge in the load/store path: interp exec_one's mem ops AND
   the recompiler-emitted cpu->read/write (or a per-load/store charge). Region table.
2. **Instruction fetch / I-cache timing.** Beetle ReadInstruction (+1 hit / +fill on
   miss). For the recompiler, fold a per-block fetch-cost constant.
3. **Mult/Div latency.** Beetle MULT_Tab/muldiv_ts_done: mult ~6-13, div ~36, stall on
   HI/LO read. Encode in psx_instr_base_cycles (+ optional stall-on-read).
4. **GTE/COP2 per-command.** Beetle gte.cpp GTE_Instruction table (RTPS=15, NCDS=19,
   NCDT=44, ...). Encode in psx_instr_base_cycles for COP2 ops.
All land in the single-source psx_instr_base_cycles (opcode costs) + a memory-path
wait-state charger (address-dependent). Both backends consume the same model (seam
already in place). Each component: transcribe -> regen/build -> Δ-compare vs Beetle
on a fixed region -> next.

### Caveats
- Δ-region must be IDENTICAL code on both (a tight loop body, or a pure-compute
  function). Avoid regions that cross turbo-load / overlay / dirty boundaries.
- Relocated BIOS-shell funcs (phys 0x30000-0x5AFFF) dispatch at a different native
  phys — anchor on game-text / BIOS-ROM, or the relocated phys.
- This is a multi-component effort; do it methodically, one validated component at a
  time. The comparator (cyc_watch + cycle_compare.py) is the validation backbone.

## 4. Tooling / oracle
- Runtime TCP port 4500; Beetle oracle 4382. Always-on rings: `event_ring`,
  `wtrace_all` (write trace; `newest=1`). `freeze_check` has slice-trace + cycle
  fields. `PSX_EXIT_HALT=1` halts-and-serves at the pc=0 exit for post-mortem.
- Build runtime: `cmake --build Tomba2Recomp/build-t2 --target psx-runtime`
  (PATH=/c/msys64/mingw64/bin). Recompiler: `cmake --build
  _wt-tomba2/psxrecomp/recompiler/build-t2 --target psxrecomp-game`. Regen:
  `recompiler/build-t2/psxrecomp-game.exe --config game.toml` (rebuild tool first).
- Reference: nocash psx-spx; the dirty-RAM interp is the in-process oracle for
  compiled code; Beetle is the HW oracle.

## 5. Status / Log (update every session)

- **2026-11-15 (battle path reached LIVE — same frame-counter freeze wall now
  hit from `battle_frame`, not just save-menu; VBlank counter confirmed
  advancing during menus):**
  - Clean-C pointer/ABI fixes (commits `9ffb080`,`174c6d9`,`79a840d`,`becc0d2`,
    `9cf7fa4`, all pushed to `mine/master`) took the game from fully-stuck-at-
    title to a LIVE battle: menu stepper desc made contiguous, `attract_menu_
    dispatch`/`battle_flow_state_machine` `int param_1`→`intptr_t` host-ptr
    fixes, `g_game_state`/`g_sub_state` re-aliased to the guest scratchpad bytes
    they were split-brain'd away from (THE unlock — `gs=8` finally reaches
    `battle_flow_state_machine`), and `task_invoke` now passes a guest-u32 arg
    to `script_move_timer_eval` (was a host ptr → infinite move-timer loop).
  - Verified live: `gs=8` sustained, `battle_flow_state_machine → battle_frame
    → task_pools_tick → script_move_timer_eval` running at ~98% CPU, character-
    select phase words advancing, debug server responsive.
  - Then hit the DOCUMENTED wall at frame ~1509: `frame_tick → cd_timer_read →
    psx_unknown_dispatch`. `unknown_dispatch_log`: `phys=0x04000000`, `ra=0`,
    `a1=0x801F68F0`, `epc=0x8019A648` (inside `vsync_wait_n`). Run report:
    `FAIL-FAST unknown dispatch addr=0x04000000` — the `vsync_wait_n` timeout
    falling into `bios_table_tramp`/`bios_kobj_tramp` → wild `jr` to
    `0x04000000` (bad kernel-object ptr). Same signature as the 2026-11-11
    save-menu freeze, now reached via the battle path.
  - **CORRECTED mechanism (supersedes the CD-IRQ-dependence theory below):**
    sampled live — `DAT_801bdde0` advances steadily through boot→menu→attract→
    gs=3/4/5 transitions with `cd_status=0x2` (CD fully IDLE) and
    `intc_dbg(0x801bce00)=0` the whole time. So the frame counter is NOT
    CD-IRQ-driven; `cd_intc_dispatch` is not its path. `cd_cb8_dispatch`
    (0x8019AE68, the registered VBlank/frame handler — writes into intc table
    @0x801bcd1c via vtable+8 reg fn 0x8019aabc) does `framecnt++` inline at
    0x8019AE94 then runs the 0x801bddc0 callback table.
  - **Live battle freeze confirmed:** launched gs=8/sub=3, framecnt went
    0x515→0x540 then FROZE solid while `gs=8`/`sub=3` stayed stable. During the
    freeze `I_STAT=0x1` (VBlank pending), `I_MASK=0xD`, and `irqctx_ring`/
    `event_ring` show VBlank IRQ_DELIVER events firing constantly with
    `take_pc=real_epc=0x8019A648` (inside `vsync_wait_n`) — **IRQs ARE
    delivered** — but every entry has `restored=0, exit_pc=0x0,
    same_thread=0, exit_reason=0, redirects=0`. That is the deferred-coroutine
    / fork-peer path (the "host coroutine/fiber cooperative-thread scheduler"
    landmine, CLAUDE.md §0): the IRQ does NOT restore/complete inline; the
    handler work (→ cd_cb8_dispatch → framecnt++) is deferred to a coroutine
    that `battle_frame`'s blocking clean-C `do{}while` never yields to resume.
    `clean_trace` confirms zero calls to 0x8019ae68/0x8019ae70 during battle.
    `v0_exit` decrements 0x100 per IRQ = the vsync_wait_n timeout counting
    down → hits 0 → BIOS trampoline → wild `jr` 0x04000000.
  - **Why menus work:** there `vsync_wait_n` is reached via frame_tick in the
    top-level dispatch loop, which returns to the scheduler between frames so
    the deferred IRQ coroutine gets resumed → cd_cb8_dispatch runs → framecnt
    advances. In battle, `battle_frame` (clean C) → frame_tick → cd_timer_read
    → `br2_call_generated`→vsync_wait_n is a deep clean→generated nest inside a
    blocking loop; the deferred resume can't unwind through the clean-C frames
    / is starved → framecnt frozen.
  - **Precise defer mechanism (interrupts.c:695-706,1347-1404):** a nested IRQ
    sets `s_defer_switch_pending` (from_tcb→to_tcb, the guest cooperative
    ChangeThread). It is honored only at a "materialized clean boundary" — a
    `psx_check_interrupts` poll where `s_last_interrupt_check_pc` is a valid
    `psx_is_dispatchable` resume PC AND `psx_sched_current_tcb==from_tcb`.
    Two failure modes in battle_frame's spin: (a) `vsync_wait_n`'s poll PCs
    (0x8019A648 mid-function) may never be `psx_is_dispatchable` → pending
    forever; (b) if `current_tcb != from_tcb` at the poll it is dropped as
    STALE (line ~1372) → switch lost → handler never runs. Both starve
    cd_cb8_dispatch → framecnt frozen.
  - **NEXT (faithful-core, do NOT patch around):** make the deferred cooperative
    IRQ/ChangeThread switch honor inside `battle_frame`'s nested blocking loop —
    either reach a dispatchable thread-save boundary per frame, or fix the
    resume-PC/TCB matching so the deferred switch isn't dropped. Then
    cd_intc_dispatch→cd_cb8_dispatch runs → framecnt advances → vsync_wait_n
    returns. Compare IRQ-delivery + cooperative-switch behaviour vs
    Beetle/DuckStation oracle at the battle-intro vsync_wait_n. Runtime work in
    `psxrecomp/runtime/src/interrupts.c` + `psx_scheduler` — NOT a clean-C
    game-port bug.
  - **2026-11-15 (freeze_check exposes the REAL pathology — exception
    re-entry + PC-zero, NOT a cleanly-starved coroutine):** `freeze_check` on
    the frozen battle reads `exception_entries=26577`,
    **`exception_reentry_blocks=4672`**, `nestgate_rfepend=4672`,
    **`pczero_count=1187`** (`pczero_addr=0x80000080` = general-exception
    vector, `pczero_epc=0x8019A648` = inside `vsync_wait_n`,
    `pczero_in_exc=1`, `pczero_ra=0x8019A53C`), `async_rfe_set=0 /
    async_rfe_fire=0 / async_rfe_resume_pc=0` (the async RFE resume path NEVER
    fires), `current_func=0x29CC` (a kernel-call-gate address), `in_exception=0`,
    `frame_count=3213` stuck, `i_stat=0x1`/`i_mask=0xD`. So the IRQ is NOT
    cleanly deferred-and-starved: the handler is entered, does work, then its
    RETURN (RFE→resume `vsync_wait_n` at a dispatchable PC) FAILS → `pc`→0 →
    `0x80000080` re-exception → the handler re-runs → loop. `cd_cb8_dispatch`
    is never reached (framecnt never written — `gwatch` total=0), so the hang
    is BEFORE its `jalr`, i.e. inside `cd_intc_dispatch` setup: it calls
    `printf_forward` (0x8018E550) then `bios_svc` trampoline 0x8019AD60 =
    `svc(0xB0,0x17)` before the handler dispatch at block_8019A9C0. The
    re-entry/PC-zero storm + dead async-RFE points at the exception-return /
    cooperative-resume path, not the ChangeThread defer gate alone.
    `PSX_HLE_SCHEDULER=0` (LLE host-fiber path) does NOT fix it — same freeze,
    same `0x4000000` — ruling out the HLE defer gate as the sole cause.
    Secondary fatal confirmed: vsync timeout prints "VSync: timeout" to
    `0x801F68F0`, `console_pct` flushes via BIOS `B0/0x35`, which returns -1
    repeatedly then dispatches `0x4000000` → `psx_fatal_halt`. BIOS calls
    work elsewhere (1.4M recorded) → context-specific to the IRQ/console
    nested path.
  - **fntrace baseline (WORKING menu phase) captured:** per VBlank IRQ the
    generated path runs `cd_intc_dispatch`(0x8019A8EC, ra=0x8019A88C,
    a0=0x801BCD50) → `cd_cb8_dispatch`(0x8019AE68, **ra=0x8019A9C8** = the
    block_8019A9C0 jalr) → `0x8019AD60` return trampoline (ra=0x8019AA9C,
    a1=0x1F801070=I_STAT, a2=0x1F801074=I_MASK). All three fire every frame
    and the IRQ completes (`restored=1`). `0x8019AD60` is a kernel-call
    trampoline `li $t2,0xB0; li $t1,0x17; jr $t2` → **B0 svc 0x17 =
    ReturnFromException** (B0_Table[0x17] per psx_bios_disasm.txt) — i.e.
    cd_intc_dispatch's own exception return goes through the kernel RFE.
  - **Correction to "fork-peer" reading:** the irqctx `restored=0/exit_pc=0/
    same_thread=0/exit_reason=0` are the ENTRY-half defaults (interrupts.c
    ~1668) — they mean the handler **never reaches the exception-exit
    epilogue** (~2016), i.e. it is entered and then HANGS mid-body, not that
    it completes on a peer. Combined with `cd_cb8_dispatch` never firing
    (framecnt `gwatch` total=0) the hang is inside `cd_intc_dispatch` BEFORE
    the block_8019A9C0 handler `jalr` — in its setup calls (`printf_forward`
    0x8018E550, the B0/0x17 RFE trampoline, or the I_STAT/I_MASK mask compute).
  - **Battle reached + freeze is TRANSIENT (new data):** drove to gs=8/sub=3
    live battle. fntrace showed the IRQ path WORKING during early battle —
    `cd_intc_dispatch`→`cd_cb8_dispatch`→`0x8019AD60`(RFE trampoline) all
    firing, framecnt advancing `0xb94e→0xba1e` (~200 frames). Then it wedged
    → same `0x4000000` fatal. So the handler is NOT permanently broken in
    battle; it works until a specific event (likely a thread-switch inside a
    VBlank callback or a nested IRQ) triggers the wedge.
  - **Wedge localized to kernel ctx-restore:** `freeze_check` at the crash
    reads `current_func=0x29CC` — and per this plan (lines ~1754/1921/2200)
    `0x29CC` = **ReturnFromException / exception context-restore** (resume a
    blocked thread). `exception_entries=92066`,
    `exception_reentry_blocks=5552` — massive re-entry. This matches the
    documented `returnFromException(0x29CC) → exceptionHandler … (256 deep)`
    recursion. So: IRQ handler completes → RFE → kernel ctx-restore (0x29CC)
    → re-triggers exceptionHandler → RFE → ctx-restore → … infinite recursion
    → the interrupted `vsync_wait_n` never resumes → framecnt frozen →
    timeout → `console_pct`→BIOS `B0/0x35`→`0x4000000` fatal. The proximate
    defect is the **exception-return / thread-context-restore recursion** in
    the IRQ path, triggered once battle runs a thread-switching callback.
  - **Live recursion captured (post-crash rings):** `disp_ring` tail = ONLY
    `target=0x80000080` (general exception vector) repeating, `ra=0x8019A53C`,
    `sp=0x801FFF78`, cycle advancing ~8k/entry — an infinite exception
    re-entry loop, NOT the kernel-func call-cycle of the CD case. `freeze_
    check`: `pczero_addr=0x80000080`, `pczero_epc=0x8019A648`(vsync_wait_n),
    `pczero_cause=0x400`(Cause.IP pending field → VBlank IRQ), `badvaddr=0`
    (NOT a memory fault), `I_STAT=1`/`I_MASK=0xD` (VBlank never acked),
    `in_exception=0` between fires. So: VBlank IRQ taken → handler runs →
    return produces `cpu->pc=0` → top dispatch routes to `0x80000080` →
    exception re-taken (I_STAT still pending) → loop. `async_rfe_resume_pc`
    is DEAD CODE (never assigned nonzero anywhere) — not the active resume.
  - **pc=0 producer located:** `dirty_ram_interp.c:2798` — sentinel dispatch
    `addr==0x80000048` sets `cpu->pc=0`; when not `in_exception` it checks
    `g_async_rfe_resume_pc`(=0) then `return 1` leaving pc=0 → top-level
    dispatch treats pc=0 as exception-vector entry `0x80000080`. The IRQ
    handler's return (kernel `returnFromException`/`0x29CC` → thread ctx
    restore) lands on this sentinel/`pc=0` path in battle's deep-nesting
    context instead of resuming `vsync_wait_n`. Menus resume cleanly because
    the IRQ is taken at a shallow/dispatchable boundary.
  - **Thread-switch RULED OUT:** `thread_trace` + `thread_ctx_ring` are EMPTY
    (total=0) for the whole run incl. battle — no ChangeThread/event-30/31
    fired. So `pc=0` is NOT a bad thread-resume-PC; it is the plain IRQ
    ack/handler-dispatch path. The loop is: VBlank IRQ → kernel
    exceptionHandler → game `cd_intc_dispatch` → return → **I_STAT bit0 never
    acked** → next `psx_check_interrupts` re-fires → `0x80000080` again.
    fntrace showed `cd_intc_dispatch`/`cd_cb8_dispatch` firing during early
    battle then STOPPING — once they stop, nothing acks I_STAT → storm.
    Open question narrowed to: what makes the kernel stop dispatching to the
    game's `cd_intc_dispatch` (registered intc handler) after ~200 battle
    frames — a corrupt handler-table pointer, a kernel HandlerInfo/verifier
    rejecting IRQ bit0, or the game handler itself wedging before its I_STAT
    ack write.
  - **NEXT (refined):** with fntrace armed on {0x8019A8EC,0x8019AE68,
    0x8019AD60,0x8018E550,0x8019A5D8}, drive to battle and diff which entries
    STOP firing vs the menu baseline — that pinpoints the exact stall call.
    Then determine why the IRQ taken deep in `battle_frame`'s nested
    clean→generated dispatch wedges the handler (exception-return /
    cooperative-resume path in `interrupts.c`), vs Beetle-oracle.
  - **2026-11-15 fntrace decisive data (IRQ-entry armed):** armed fntrace on
    the game's IRQ entry wrapper `0x8019A87C` (calls cd_intc_dispatch) +
    `0x8019A8EC` + `0x8019AE68` + `0x8019A814`. Reproduced battle → crash.
    Ring tail shows **all three fire per-frame through battle, last at guest
    frame ~2073** — the kernel DOES reach the game IRQ handler the whole time;
    it stops being invoked only at the very end. So "kernel stops dispatching"
    is the SYMPTOM, not a corrupt handler table.
  - **returnFromException is a LEAF (verified in OpenBIOS_full.c
    `OpenBIOS_func_000029CC`):** reads current TCB via `*(0x108)→*k1`, restores
    every GPR from TCB offsets (LO@144/HI@132/EPC→k0@128/…), `mtc0 a1→SR`,
    `jr $k0`+`rfe`, `pc=gpr[26]`=saved EPC. **No thread scheduling inside** —
    it completes and resumes `vsync_wait_n` correctly every time it runs.
  - **Refined break location:** IRQ works through ~2073 battle frames, then
    the LAST `cd_cb8_dispatch` fires and the NEXT VBlank never reaches
    `0x8019A87C`. Since RFE resumes correctly, the break is in the
    **RFE→resume→next-IRQ-entry cycle**: after resume, the next exception
    entry produces `pc=0`/sentinel `0x80000048` instead of dispatching the
    handler. That sentinel path needs `!in_exception && async_rfe_resume_pc==0`
    (dirty_ram_interp.c:2798); `g_async_rfe_resume_pc` is latched from
    `g_dirty_safe_resume_pc` at dirty-safe exception entry — but this IRQ
    interrupts COMPILED `vsync_wait_n` (block-boundary pump leaves
    `g_dirty_safe_resume_pc==0`), so the async-RFE latch never arms and the
    compiled-code sentinel+longjmp path must carry it instead. Working
    hypothesis: at ~frame 2073 a VBlank IRQ lands on a compiled-code resume
    point where `in_exception`/`g_exception_owner_fiber` no longer matches
    (fiber resumed on different fiber, or in_exception cleared early), so
    `psx_rfe_escape_check` declines → trampoline dispatches `cpu->pc`=sentinel
    → `pc=0`→`0x80000080` storm. Needs fiber/in_exception state capture at the
    failing IRQ take vs the 2072nd working one, vs Beetle oracle.
  - **2026-11-15 sentinel path RULED OUT + storm signature captured:** added a
    `0x80000048`-reach diagnostic in `traps.c` (counts sync-longjmp / async-resume
    / fallthrough + latches first-fallthrough GPR/COP0/fiber context, exposed via
    `freeze_check` `sent_*`). Reproduced battle→crash: **`sent_reach=0`** — the
    sentinel is NEVER dispatched, so the async-RFE/sentinel hypothesis is dead.
    `irqctx_ring` storm entries are uniform: `take_pc=real_epc=0x8019A648`
    (vsync_wait_n, REAL-EPC not sentinel), `ra_exit=ra_saved=0x8019A53C`,
    `v1=0xFFFFFFFF`, but **`escape_reason=NONE`, `cpu->pc=0`, `restored=0`** —
    i.e. the handler DOES run and return each iteration, but leaves `cpu->pc=0`
    with no recognised RFE/SYSCALL escape. Live TCB dump: `*(0x108)`→TCBH
    `0x8FD0`, `entry`→TCB `0x8FE0`, saved EPC (TCB+0x88)=`0x8019A600` VALID —
    the interrupted thread's context is NOT corrupt. RULED OUT this session:
    sentinel path, TCB/EPC corruption, thread-switch (thread rings empty), null
    `jr ra` (ra=0x8019A53C valid), unaligned/overflow early-return
    (`psx_unaligned_access`/`psx_arith_overflow` don't publish pc=0).
  - **Refined producer:** `pc=0` is set inside the COMPILED OpenBIOS
    exceptionHandler / game-IRQ-handler (`0x8019A87C`) return chain — a generated
    `cpu->pc = <reg>` (jr/jalr/RFE) resolving to 0, or a function returning with
    pc unset. `current_func=0x29CC`(kernel RFE)+`last_store_pc=0x8019A60C`
    (vsync_wait_n) bracket it. Next: instruction/block trace WITHIN
    `OpenBIOS_psx_dispatch`/`br2eu_func_8019A87C` tail to catch the exact
    `cpu->pc=0` write, vs Beetle oracle IRQ/RFE semantics.
  - **2026-11-15 dispatch_tail = the storm cycle, game handler ABSENT:** the
    FATAL crash report's `dispatch_tail` (256 addrs) shows ONE repeating
    OpenBIOS-kernel cycle per storm iteration (low-RAM funcs):
    `0x16E4 0x1830..0x1884 0x2610 0x18AC 0x2970 0xB0 0x2AA0 0x29CC 0x27AC
    0x279C 0x27EC 0x1F6C 0x2074 0x1A74 0x2610 0x1AB4 0x1BD0 0x1ACC 0x1B24
    0x1B7C 0x371C 0x3E6C 0x26E4 0x3F00..0x3F30 0x4000..0x4010 0x3FDC` then
    repeats. Named: `0x27AC`=exceptionHandler, `0x279C`=GetCauseEpc,
    `0x29CC`=returnFromException, `0xB0`=BIOS vector. **`0x8019A87C` (game IRQ
    entry) NEVER appears** — consistent with fntrace stopping. So the kernel
    exceptionHandler runs its full cycle each VBlank but does NOT invoke the
    game's IRQ handler → the game's I_STAT ack (`cd_intc_dispatch` 0x8019A9AC)
    never runs → I_STAT bit0 stays 1 → level-triggered VBlank re-fires forever.
    The `pc=0` is produced inside this kernel cycle (likely the EntryInt
    `jr ra`→JB_PC or a func2 handler `jalr` resolving to 0 / a func returning
    pc unset). NEXT: decode the tail funcs `0x3F00-0x3FDC`/`0x4000-0x4010`
    (the handler-invocation + return) and the EntryInt JB_PC value to find the
    null dispatch, vs Beetle oracle IRQ-handler registration semantics.
  - **2026-11-15 MAJOR REFRAME — clean-C nested-dispatch is the real layer:**
    the IRQ path itself (`vsync_wait_n` 0x8019A648, IRQ entry 0x8019A87C,
    `cd_cb8_dispatch` 0x8019AE68, ra 0x8019A53C) is ALL generated — verified
    NOT in `clean_table.c`. BUT the battle main loop (`battle_frame`,
    `frame_tick`, `battle_frame_tick`, ~thousands of `wrap_801*` entries in
    0x8012-0x801F) IS clean-C, calling generated funcs via `call_guest`/
    `br2_call_generated`→`psx_dispatch_call` (nested, `dispatch_depth=3` at
    crash). **`pc==0` is the NORMAL nested-call return contract**: generated
    code returns by `cpu->pc = $ra`; when a nested call parks `$ra=0`
    (`BR2_NESTED_RA`), the final `jr $ra` yields `pc==0` which
    `psx_dispatch_impl` treats as a plain C return — NOT a fault. The bug is
    a `pc==0` ESCAPING the nested `psx_dispatch_call` and reaching the
    OUTERMOST dispatch, where it routes to `0x80000080`. Mechanism candidates:
    (a) IRQ fires inside a generated func nested under clean-C; RFE produces
    `pc==0` and the nested-depth accounting fails to claim it as a return;
    (b) `psx_check_interrupts_dispatch_entry` (clean_table.c:4677) delivers
    VBlank at clean-fn entry with `resume_pc=addr`, and the RFE resume into a
    clean-C boundary yields `pc==0`. NEXT: instrument `psx_dispatch_impl`'s
    pc==0 handling to log dispatch_depth + whether it was a nested return vs
    a genuine null — the frame-2073 transition is where nesting first breaks.
  - **2026-11-15 MECHANISM IDENTIFIED — poisoned TCB resume-PC from IRQ taken
    deep in nested host dispatch (the documented bug class, now live in
    battle):** resume PC lives at **TCB+136 (0x88)** — guest RFE `0x29CC` does
    `k0=*(TCB+0x88); jr $k0;rfe`, host `psx_restore_context_from_tcb`
    (traps.c:373) reads the same slot into `gpr[26]`. traps.c:380-392 +
    interrupts.c:695-711 ALREADY document this exact failure: a ChangeThread/
    IRQ honored *mid-nested-host-dispatch* saves a POISONED TCB — the latched
    resume-PC (`s_compiled_interrupt_resume_pc`/`g_dirty_safe_resume_pc`) is
    AHEAD of the live GPRs because the true block PC lives only in a host C
    frame's local `addr`, not CPUState. Result: RFE `jr $k0` resumes to a
    stale/zero PC → `cpu->pc=0` → unwind → outermost sees pc==0 → the IRQ
    re-fires (I_STAT never acked because the game handler's continuation was
    cut) → `0x80000080` storm. Battle reaches `dispatch_depth=3`
    (clean `battle_frame`→`frame_tick`→generated `vsync_wait_n`), so the VBlank
    IRQ lands inside a nested `psx_dispatch_call` where the resume-PC latch
    desyncs — matching why menus (shallow) work and battle breaks ~2073 frames
    in (first IRQ to land at the deepest nesting). FIX DIRECTION: defer the
    IRQ/ChangeThread honor to the outermost dispatch boundary (the
    `s_defer_switch_pending` mechanism already exists for thread-switch —
    extend the same deferral to the IRQ-resume-PC capture so the TCB is saved
    with the materialized PC, not the stale latch), vs Beetle oracle IRQ-timing.

- **2026-11-11 (frame-counter freeze ROOT-CAUSE narrowed — MMIO fix confirmed
  landed; counter depends on CD IRQ, freezes when CD idles):**
  - **MMIO guest_at-drop fix CONFIRMED LANDED in cd.c** — `cd_chan_full_init`,
    `cd_state_blk_save/restore`, `cd_chan_slot_set`, `cd_cb7_run`,
    `cd_init_stage_a/b` all route INTC/DMA/CD regs through `br2_ld*/br2_st*`;
    ZERO `guest_at(0x1f801xxx)` literals remain in `game/*.c`. Bug #1
    (CD-command MMIO) is closed for the IRQ path.
  - **clean_trace ring (last 512 clean calls) shows `cd_cb8_dispatch`
    (0x8019AE68) ran 17x** — so IRQ → `cd_intc_dispatch` → `cb8_dispatch` →
    `DAT_801bdde0`++ DOES execute. `cd_intc_dispatch`/`cd_ctx_restore`/
    `cd_chan_full_init` show 0 only because they are GENERATED (clean_trace
    tracks clean calls only).
  - Live state at wedge: `framecnt(0x801bdde0)=0x60a` FROZEN,
    `cd_status(0x801bcb24)=0x2` READY/idle (the WORKING signal per earlier
    bisect), `inservice(0x801bcd18)=1`, `intc_dbg(0x801bce00)=0`,
    `cop0_epc=0x8019A648` (inside `vsync_wait_n`), `pc=0` after OpenBIOS
    `0x27AC` trampoline. IRQs ARE delivered (`irqctx istat=0x1/0x5`).
  - **REFINED mechanism:** `cb8_dispatch` is handler[0]=VBlank bit inside
    `cd_intc_dispatch`, but `cd_intc_dispatch` is resumed by the deferred
    coroutine (`cd_ctx_restore`→`cd_chan_full_init`) which the kernel invokes
    on IRQ. It ran 17x then STOPPED once CD went idle (`cd_status=0x2`, no
    pending CD command → no CD IRQ → coroutine not resumed → `cd_intc_dispatch`
    stops → `framecnt` freezes → `vsync_wait_n` timeout → crash). The earlier
    "clean-C nesting starves the switch" theory is WRONG — `cd_ctx_restore`
    resumes INLINE via `psx_restore_state_escape`/`exception_jmpbuf`, no
    thread switch needed.
  - **NEXT (two concrete unknowns):** (a) is the hooked IRQ entry
    (HookEntryInt→`cd_chan_full_init` coroutine) resumed on ALL maskable IRQs
    or CD-ROM-source only — if CD-only, the frame counter genuinely needs
    ongoing CD IRQs; (b) should a CD command/read stay ACTIVE during
    `vsync_wait_n` (streaming read or periodic poll) that our CD emulation
    completes/idles prematurely — compare the CD command + IRQ sequence
    against the Beetle/DuckStation oracle at the same battle-intro point.
    `cdcell_u16` (cd.c:1548) `guest_at(*(u32*)guest_at(cell))` double-indirect
    is a remaining MMIO-audit candidate if the cell can hold `0x1f801xxx`.

- **2026-11-11 (REFINED — heapA receives each decode's `srcg`, not a cursor):**
  Added tag15 `hser` (heap_serialize entry-blk/ret-cur/emit-count) + tag16
  `rzd` (ret_zero_d in/ret/delta). Result: heap_serialize NEVER reached (zero
  hser — heap_record_emit early-returns all slots, heapA unchanged); ret_zero_d
  fired once +0x1C14 (normal); terrain spr_pair records all post-date the jump.
  The +0x1906C4 heapA write (seq413 0x80034298→0x801C495C) is none of those.
  **KEY: heapA_w seq413/427/450/452... = 0x801C495C/0x801C63F8/0x801C79FC...
  EXACTLY match the decode blk_ret `src` sequence — heapA is assigned each
  block's compressed-source pointer (srcg).** srcg = offset_table_lookup's
  return AND stream_block_decode's mode3_dispatch arg. So a decode-adjacent
  path stores the block's source pointer into heapA instead of output cursor.
  NEXT: tag the heapA_w write with caller-PC/callsite to find which
  BR2_W32(0x801c47b8,...) receives srcg; audit mode3_dispatch/stream_block_decode
  for a srcg-returning path and the save.c sibling SM (~6793+) heap/tag loop.

- **2026-11-11 (heap-A over-production LOCALIZED to `heap_serialize` cur
  return; CD delivery PROVEN byte-perfect; file-0x82 dest bug FIXED):**
  - **CD data integrity VERIFIED** — dumped delivered sector (lba 314) vs
    `Game.bin` bytes: 64/64 match. Extent→LBA decode delivers correct data.
    The "CD data integrity vs disc unverified" suspect is CLOSED.
  - **file-0x82 mistranslation FIXED (commit 92b06d4):** generated
    `0x80147F28` loads file 0x82 dest = `*(0x801C47B8)` (heap-A cursor);
    clean-C passed `s3`=tag-table `0x801c47c0`, overwriting the master decode
    tag table → `offset_table_lookup` read foreign data → wild cursor jump.
    Real faithful fix, BUT in `battle_bringup_b` path — trace shows NO file
    0x82/0x81 load before the active corruption, so it does NOT fix THIS stall.
  - **Named decomp_trace tags** (event_ring.c): 9=spr_pair 10=file_load
    11=blk_ret 12=heap_emit 13=heapA_w(old→new) 14=off_lookup.
  - **PRECISE localization of the +1.64MB heap-A jump (seq412
    0x80034298→0x801C495C):** it is `heap_record_emit`→`heap_serialize`
    returning `cur`=0x801C495C — a COMPRESSED-SOURCE block address (was the
    seq411 decode `src`), NOT an output cursor and NOT offset_table_lookup
    (zero tag14 records). Per-tag serializers are generated-faithful
    (BR2_TAG→br2_call_generated), so the bug is in `heap_serialize`'s
    clean-C walk @menu.c:14250 (guest 0x8016B7C0, ~1239 insns): record
    parsing `cnt=MB32(blk+8)`, `ctx=MB32(blk+4)`, `src=MB32(ctx+4)+
    MB32(rp+8)*4`, stride `rec=rp+4+f4*4`, block-list `blk=MB32(blk)` —
    OR `blk`(=MB32(heapB slot)) points at undecoded/garbage data making
    `cnt` huge → runaway emit. heapA then self-perpetuates 0x801C495C→
    0x801C90B8 via repeated polls; file 0x88 DMA overwrites globals → L10.
  - NEXT: diff `heap_serialize` clean walk vs generated `br2eu_func_8016B7C0`
    focusing on `cnt`/`ctx`/`src`/stride and the blk-list termination; OR dump
    the decoded heapB block + its record table at the seq411 instant to check
    `cnt` sanity. Also audit the OTHER heapA_w jumps (seq449-457 +0x2dc steps
    = off_lookup-driven table walk landing heapA in 0x801C region).

- **2026-11-11 (CAUSAL CHAIN CLOSED — state-6 stall = heap/decode overrun,
  NOT a CD bug):** Disassembled generated `cd_file_load_sm`(`br2eu_func_8018CFDC`,
  25 blocks): filetab@`0x801B5B94`+file_id*8 `{extent_ptr,sector_count}`; gates
  on `[extent_ptr]!=0` AND `DAT_801F68D8==0` (SM state counter, decremented
  per call until 0 -> issue read); then `cd_read_issue(extent,dest,count)` /
  `cd_read_poll(1,0)`; `0x801B55EC`=read-in-flight flag, `0x801C4580`=loading
  byte. LIVE state: `0x801F68D8`=`0x11e5eae0` (huge garbage, actively
  decrementing) = FILE DATA — file-0x88 DMA wrote `0x801C90B8->0x801FA8B8`
  over it. heap-A cursor `0x801c47b8`=`0x801c90b8` (over-produced ~108KB past
  safe bound `0x801AE800`); heap-B `0x801e3cfc`=`0xdf0fd1c` (also file data).
  => decode over-produces -> heap cursor too high -> file-0x88 load dest too
  high -> DMA overwrites `0x801F68D8`/`0x801e3cfc` -> cd_file_load_sm reads
  garbage state -> never issues read -> returns 0 -> save_load_state_machine
  L10 -> battle_intro_flow resource-load -> g_game_state=6 frozen. CD driver
  VERIFIED faithful (data chunks differ per sector; bios_svc->svc_call real;
  bcba4=in-flight flag, 0=idle/complete). Decode fns (`block_copy_ret_len`,
  `lzss_decode`, `mode3_dispatch`) faithfully return header `*(src+8)` length
  => over-production = WILD srcg from corrupted source table/cursor, not a
  decoder bug. NEXT: fresh run + decomp_trace during the overrun to catch the
  decode whose srcg/ret is wild (ring rolled to post-corruption len=0). The
  heap-B cursor feeding param_2 source-table is the prime suspect.

- **2026-11-11 (state-6 stall ROOT-LOCALIZED to CD read done-flag `bcba4`
  never latching — data path verified OK):** Full hang chain traced live:
  `g_game_state=6` frozen → `battle_intro_flow` sub-state `DAT_1f800001=1`
  (resource-load) → `menu_res_load_sm` st20 → `save_load_state_machine` L10
  → `cd_file_load_sm(sel+0xb0, tag_table)` returns 0 forever. CD-driver state:
  `bcb94`(sectors-remaining)=0 and `bcba4`(done-flag)=0 **stably** — all sectors
  consumed but the done-flag never latched, so `cd_read_poll` returns 0 ~195k×
  and the SM never advances. `cd_read_step_cb`/`cd_read_finish_cb` set
  `bcba4=1` when `bcb94` hits 0 (cd.c:1103/1130); `cd_init_done_cb`(0x80198590)
  clears it at next-session init. Since `bcb94` stayed 0 (no new read), the
  done-path that sets `bcba4=1` appears to have run then been cleared by the
  async session/Pause-complete callback — an ordering race — OR never ran.
  **CD data integrity VERIFIED OK:** `cd_read_log` shows all `size=2048` reads
  at command-base `lba=9247` but consecutive dest chunks (`+0x800`) contain
  DIFFERENT bytes → drive auto-advances sectors internally (logged lba is the
  command base, not per-sector). filetab@`0x801b5b94`+id*8 = {extent_ptr,
  sector_count}: fid `0x88`→extent `0x801b5810` cnt `0x63`(99, matches active
  `bcb80=0x63` read), fid `0xb0`→extent `0x801b58b0` cnt `0x0c`(12) — the
  99-sector read in flight is NOT `0xb0`'s primary data, so `cd_file_load_sm`
  reads multiple streams/files per call. NEXT: determine whether `bcba4` is
  ever written 1 (gwatch noisy — page hammered, 52M oor) vs cleared by
  `cd_init_done_cb`; inspect generated `cd_file_load_sm`(0x8018cfdc) exit
  condition + which guest flag it polls; check `mc_session`/session-callback
  scheduling vs the synchronous `bcba4=1` latch. Disc symlinked
  (`~/Projects/bloody-roar-2/Game.{bin,cue}`) → boots, renders, reaches battle.

- **2026-11-11 (kernel objects FOUND in low RAM — structure decoded; class-desc
  space 0xF0000000 still unmapped):** Scanned KUSEG low RAM (`0xA0000000+phys`)
  and located the live kernel objects the `B0/0x08` allocs created:
  * Handle table @phys `0x4bd4`: `0xF1000004..0xF1000000` (and mirrored cells
    `0xef70-0xef80` = the `0x8020ef70` cells cd_hw_chan_init wrote).
  * Object instances @phys `0x8e08`, stride `0x1C`, layout:
    `{+0x00 class_id, +0x04 0x2000, +0x08 attr, +0x0c size, +0x10 CALLBACK,
      +0x14 0, +0x18 0}`. The 8 CD-channel objects carry callbacks
    `0x801a0b10/24/38/4c` (class `0xF4000001`) and `0x801a0b60/74/88/9c`
    (class `0xF0000011`) — **CD IRQ callbacks ARE registered in the objects**.
  * One class-descriptor entry @phys `0x9010`: `{0xbfc04974, 0xf0000009, ...}`
    = {BIOS func, class_id} — class descriptors live in low RAM too.
  * SO: object instances + class descriptors are all in normal low RAM and
    populated correctly. The break is PURELY that `0x2610` is handed the class
    ID `0xF0000003` and dereferences it as a **virtual address**
    (`lw s0,0x20(a0)`→`0xF0000023`), but `0xF0000000` is KSEG3/unmapped in the
    recomp → open-bus → the class→object→handler walk never reaches the
    registered `0x801a0bXX` callbacks.
  * OPEN (next): `0x2610` uses a0 DIRECTLY as a pointer — no handle→ptr
    translation in its prologue. So on real HW `0xF0000000` MUST be a mapped
    class-descriptor/virtual region. Determine its physical backing: either a
    KSEG3 alias the recomp must implement in `psx_phys_addr`, or the kernel
    object "class address" is `0xF0000000|index` into a table at a fixed low-RAM
    base. Compare against how `0x2610`'s siblings (VBLANK etc, which DO work)
    receive their class arg — if VBLANK passes a low-RAM pointer while CDROM
    passes `0xF0000003`, the difference is the bug.

- **2026-11-11 (kernel-object handle scheme decoded via bios_svc_trace):**
  `B0/0x08` (=FUN_8019b380 kernel-object alloc) calls and returns:
  `{type=0xF4000001|0xF0000011|0xF0000009, attr={4,0x8000,0x100,0x2000,0x20}}
  → handle=0xF1000000..0xF1000008`. So **object instances = 0xF1XXXXXX handles,
  class/type IDs = 0xF0/0xF4XXXXXX**. The IRQ dispatcher `0x16E4` passes the
  **class ID `0xF0000003`** (CDROM) to `0x2610` — i.e. `0x2610` is the kernel's
  "invoke class interrupt handler" that maps class-ID → registered callback.
  `B0/0x5B` registers per-source handlers (`a1=0x8019AE68` seen), `C0/0x02+0x03`
  register IntRP records (`a1=0x801FEB40`), `B0/0x15` issues CD cmds
  (`a0=0x20000001,a1=0x801FEB28`). memory.c:1677 already special-cases the
  `0xF0000011`/`0xF000000X` **type-tag values stored inside normal-RAM objects**
  — so object bodies live in RAM, but the `0xF0000000`-based CLASS-descriptor
  space that `0x2610` dereferences (`lw 0x20(a0)`) reads open-bus. OPEN: is
  `0xF0000000` a real mapped class-descriptor table the recomp must back, or a
  handle `0x2610` should translate via a kernel table first? Disasm `0x2610`
  fully (it continues past 0x2660) + find where class descriptors are written.

- **2026-11-11 (ROOT-CAUSE CANDIDATE: kernel objects live at 0xF0000000 KSEG3 —
  unmapped → open-bus → CD callback lookup fails):** Disassembled the game's own
  kernel IRQ path straight from guest RAM (`read_ram` @0x8000xxxx):
  * `0x16E4` (hw-IRQ dispatch): `lw v0,I_MASK(0x1F801074); lw v1,I_STAT(0x1F801070);
    and; andi 4` → tests **CDROM bit2 only** → `jal 0x2610` with `a0=0xF0000003`
    (CDROM kernel-object handle), `a1=0x1000`. Sibling blocks do the same for
    other IRQ-source handles.
  * `0x2610` (kernel-object invoke): prologue then `lw s0,0x20(a0)` /
    `lw s1,0x24(a0)` — **dereferences a0=0xF0000003 as a raw pointer** into
    `0xF0000000`-based kernel-object space, then bounds-checks and (on the
    non-taken path past `0x2660`) walks object fields to reach the registered
    interrupt callback.
  * `psx_phys_addr(0xF0000003)` = `0xF0000003 & 0x1FFFFFFF` = `0x10000003` —
    **outside RAM/MMIO/BIOS** → every `lw` in `0x2610` returns open-bus (0).
    So the kernel object's callback field reads as 0 → no game CD handler is
    ever invoked → busy flag `0x801bcd1a` stays 0 → `cd_cmd_issue` spins.
  * `read_ram(0xF0000003)` confirms all-zero. `0xF0000000` is KSEG3
    (0xE0000000-0xFFFFFFFF) — TLB-mapped kernel space on real HW; recomp has
    NO KSEG3/TLB model → kernel-object derefs are dropped.
  * HYPOTHESIS: the game kernel maps its device-object heap to `0xF0000000`
    (KSEG3) via TLB, OR `0xF0000003` is a handle the kernel is supposed to
    translate before deref. Either way the recomp's flat `&0x1FFFFFFF` phys
    fold cannot reach it. FIX DIRECTION: find where `cd_hw_chan_init`'s
    `FUN_8019b380`(B0/0x08) kernel-object allocs actually placed the objects in
    RAM (they ARE created — bios_svc_trace shows the allocs), determine the
    `0xF0000000`→RAM alias the game kernel uses, and map it in `psx_phys_addr`
    / the memory model. This is faithful-core work (real HW kernel-object
    space), NOT a game hack.
  * VERIFY FIRST: confirm `0x2610`'s `lw` on `0xF0000023` actually returns
    open-bus at runtime (not a mapped alias I haven't found) — set a read
    watch or check `g_kseg2_ignored_reads`/open-bus counters while CD IRQ
    fires. Then locate the real object backing store.

- **2026-11-11 (IRQ callback path fully traced — CD handler genuinely never
  invoked; OpenBIOS ruled out):** Live pc_probe campaign on the running headless
  build resolved the dispatch topology:
  * IRQ delivery + kernel dispatch WORK: exception entry `0x27AC`=3911,
    hw-irq dispatcher `0x16E4`=3911, kernel-object invoke `0x2610`=11733 (~3 per
    IRQ batch). VBLANK path fine.
  * The game's OWN kernel handles IRQs — OpenBIOS is NOT in the IRQ path:
    `exceptionHandler`/`cdromDMAHandler`/`cdromIOHandler`/`cdromGetStatus`
    (all `0xBFC2xxxx`→masked `0x1FC2xxxx`) = **count 0**. Game installs its own
    low-mem kernel (`0x0000xxxx`) post-boot; OpenBIOS only services B0 calls.
  * IntRP record @`0x801FEB40`: `{id=0x461c, cb1=0x801aa244, cb2=0x801aa2ac,
    +0x10=0x8019a048, +0x18=bfc07734, +0x1c=bfc07744, +0x20=0x8019a038}`.
    `0x801aa244`/`0x801aa2ac` DO run (3911/7822) — but they alias into
    `elem_pair_field_set` (save/memcard, save.c) = **memcard poll, red herring**.
  * Handler table @`0x801bcd1c` IS populated: `[0]=0x8019ae68, [2]=0x801979f8
    (cd_response_pump), [3]=0x8019af7c` — but ALL of `0x801979f8/0x8019ae68/
    0x8019af7c/0x8019a7d4/0x8019a8ec/0x8019a048/0x8019a038` probe = **count 0**.
  * `cd_intc_dispatch`(0x8019a8ec) is a DECOY — only called from the never-taken
    `cd_ctx_save` re-entry path (cd.c:5204); not the IRQ entry. Its address
    `0x8019a8ec`/`0x8019a87c`/`0x8019a7d4` appear NOWHERE in guest RAM.
  * NET: CDROM IRQ is pending at INTC (i_stat bit2, irq_flag=0x03) and CPU takes
    IRQs, but the game's CD-completion handler is never reached → busy flag
    `0x801bcd1a` stays 0 → `cd_sync_wait`/`cd_cmd_issue` spins → same-sector
    retry forever. The kernel-object `0xF0000003` (CDROM) callback that SHOULD
    reach the game CD handler is either unregistered or mis-dispatched.
  * NEXT: locate kernel object `0xF0000003` in kernel space and read its
    registered interrupt callback; determine which BIOS/kernel call installs it
    during CD init (`cd_chan_init_trio`/`cd_subchan_table_install`/`cd_hw_chan_init`
    → `FUN_8019b380` kernel-object allocs). Also bisect whether this is a
    regression (game reportedly reached VS fights before) vs always-broken.

- **2026-11-11 (DECISIVE: all-generated baseline stalls identically → runtime/kernel
  bug, NOT clean code):** Ran the definitive A/B: `CLEAN_MIN=0xffffffff
  CLEAN_MAX=0xffffffff` makes `clean_range_ok` false for every guest addr →
  `clean_try_dispatch` returns 0 → **100% generated code path**. Result is
  IDENTICAL to the clean run: `busyflag @0x801bcd1a=0`, `result @0x801bcb24=0`,
  `cd_intc_dispatch`/`cd_response_pump` counters = 0, and `cdrom_state.seq`
  FROZEN at 5734 with `read_msf` pinned at `[48,10,56]` (no new CD commands
  issued) while `frame` keeps advancing (VBLANK-only progress). Also ran
  `CLEAN_SKIP=<whole IRQ-response cluster>` (force 11 CD-IRQ addrs to generated)
  — same stall. CONCLUSION: the CD-completion stall is a **shared-runtime /
  kernel-object IRQ delivery bug affecting generated AND clean code equally**.
  The clean `game/` source is NOT the regression; it faithfully reproduces the
  same kernel boundary the generated code hits. The real gap is upstream of all
  game code: the kernel CDROM object (`0xF0000003`) interrupt→callback dispatch
  never invokes the game's registered CD handler, so `cd_intc_busy_read` stays
  0 → `cd_cmd_issue` waits forever → game retries the same sector read.
  VBLANK IRQ works (frames advance) so generic IRQ→kernel→game dispatch is
  fine; ONLY the CDROM-source kernel-object callback is missing. NEXT (runtime,
  not game/): instrument the kernel `0x2610` kernel-object invoke for source
  bit2/CDROM — dump the object table entry for handle `0xF0000003`, confirm
  whether a callback was ever registered (B0 svc that installs it), and whether
  the kernel CDROM ISR (`func 0x1FC04A8C` region) runs on CD irq_flag=0x03.
  The indirect CD callbacks `0x801AA244`/`0x801AA2AC` ARE seeded into dispatch
  cells `0x801FEB44/48` by `cd_subchan_table_install` — verify the kernel calls
  THOSE (they have real `br2eu_func_*` generated bodies) rather than
  `0x8019A87C`. Suspect: kernel-object callback registration BIOS call is a
  no-op/misrouted, OR the CDROM kernel ISR isn't wired to the game's IntRP
  record. This is faithful-core work in `psxrecomp/`, game/ is exonerated.

- **2026-11-11 (ROOT CAUSE CORRECTED + object table decoded — CDROM IRQ-source
  kernel object class `0xF0000003` is ABSENT):** Re-disassembled `0x2610`
  (kernel-object invoke) from live RAM — the earlier "dereferences `0xF0000003`
  as a pointer / KSEG3 unmapped" hypothesis is **WRONG**. Corrected decode:
  `0x2610` sets `v0=0x100` (FIXED), reads object-table base `s0=*(0x120)` and
  span `s1=*(0x124)`, then iterates objects at **stride 0x1C** calling
  `callback=*(obj+0x10)` via `jalr`. Match requires ALL of:
  `obj->f4(+4)==0x2000`, `obj->class(+0)==a0`, `obj->attr(+8)==a1`,
  `obj->size(+0xc)==0x2000`. `a0`/`a1` are class/attr TAGS compared against
  object fields — never dereferenced. So `0xF0000000`-space is NOT an address
  and the KSEG2-guard/open-bus theory is moot.
  LIVE TABLE @phys `0x8e08` (span `0x1c0`=16 objs): objs0-3 class `0xF4000001`
  cb `0x801a0b10/24/38/4c`; objs4-7 class `0xF0000011` cb `0x801a0b60/74/88/9c`;
  obj8 class `0xF0000009` attr `0x20` size `0x2000` cb `0` (the mark EvCB from
  `FUN_8019b380(0xf0000009,0x20,0x2000,0)`); objs9-15 empty. Sibling tables:
  `*(0x100)`=0x8de0/0x20, `*(0x108)`=0x8fd0/4, `*(0x110)`=0x8fe0/0x300.
  IRQ dispatcher `0x16E4` calls `0x2610` per pending bit, ALL with `a1=0x1000`:
  bit0x0004(CD)=`0xF0000003`, bit0x0200=`0xF0000009`, bit0x0002=`0xF0000002`,
  bit0x0400=`0xF0000010`, bit0x0100=`0xF0000011`, bit0x0001(VBLANK)=`0xF0000001`.
  CAVEAT (self-correction): the match ALSO requires `size(+0xc)==0x2000`, and
  the ONLY size-0x2000 object is obj8 (mark EvCB, attr 0x20) — so `0x2610`
  finds NO match for ANY irq incl. VBLANK, yet VBLANK works. => `0x2610` is
  probably a SECONDARY kernel-object notify that legitimately no-ops here; the
  REAL irq dispatch is the IntRP queue funcs (`ToT[0]`=0x8DE0, Queue3 hw-irq
  func1=0x16E4 plus siblings). The missing `0xF0000003` object is therefore
  NOT proven to be the blocker — do NOT over-claim. REAL question: which IntRP
  func delivers CDROM bit2 to the game's CD handler (`cd_intc_dispatch`/
  `0x8019A87C` jmpbuf resume)? Plan earlier: IntRP @0x801FEB40 cb1=0x801aa244
  cb2=0x801aa2ac run but alias to memcard; CD addrs 0x801979f8/0x8019ae68/
  0x8019af7c/0x8019a8ec/0x8019a048/0x8019a038 all probe=0. NEXT: enumerate the
  IntRP queue3 func list for the CDROM source and find why the game's CD func
  isn't in it (registration gap vs dispatch gap). Faithful-core, not a hack.

- **2026-11-11 (IRQ chain fully mapped — gap isolated to kernel-object CD callback):**
  Traced the complete maskable-IRQ path live. CPU exception → kernel
  `ExceptionHandler` (RAM vector `0x80000080`→`0x27AC`, valid low-mem kernel
  code, saves TCB/GPRs) → iterates 4 IntRP callback queues via `ToT[0]`=0x8DE0
  → **Queue3 (hardware IRQs)** func1=`0x16E4` = kernel hw-irq dispatcher.
  Disasm of `0x16E4`: reads `I_MASK(0x1F801074) & I_STAT(0x1F801070)`, and for
  each pending bit calls kernel `0x2610` with a per-source kernel-object handle
  (`0xF0000003` for CDROM bit2, others for VBlank/etc), then conditionally ACKs
  via flag `0x6514` (currently NULL — likely "auto-ack" flag, game acks manually).
  `0x2610` dereferences the kernel object and invokes its registered callback.
  KEY: VBlank IRQ works (frame counter advances, game logic runs) so the
  kernel→game IRQ mechanism is functional — ONLY the CD-specific callback is
  not invoked. `br2eu_func_8019A87C` (generated) is a conditional wrapper that
  `jal 0x8019A8EC`=cd_intc_dispatch on its not-equal path; it and
  `br2eu_func_8019A8EC` DO exist as real generated functions (in
  `generated/SLES_017.22_full_46.c`), reached only via indirect dispatch.
  `0x8019A87C` is NOT stored as a pointer anywhere in guest RAM (full scan) —
  it is registered as a callback inside kernel-object space (`0xF000000X`),
  populated by a BIOS call. CD subsystem init verified complete
  (`0x801BCC90`=1, kernel objects `0x801FEB58/5C`=BIOS addrs, IntRP records
  `0x801FEB40`/`0x801FEFC0` registered via C0/0x02+0x03 confirmed in
  `bios_svc_trace`). NEXT: find the BIOS/kernel call that registers the game's
  CD IRQ callback into the CDROM kernel object and verify it ran + the object
  holds `0x8019A87C` (or wrapper). Suspect a clean-code BIOS-call boundary that
  silently no-ops the registration.

- **2026-11-11 (clean↔generated bisect + ROOT CAUSE: MMIO derefs dropped via
  guest_at):** Added `CLEAN_MIN`/`CLEAN_MAX` (inclusive guest-addr range gate) +
  `CLEAN_SKIP` (comma-list of addrs forced to generated) to `clean_try_dispatch`
  for live A/B bisect. Reliable success signal = byte0 of `0x801BCB24` (CD
  status): `0x02`=ready/idle in the working generated baseline (~57 fps),
  `0x00`=stuck in the `cd_cmd_issue` retry loop (~13 fps).
  Bisect: pure generated = OK; clean covering ONLY `0x8019A000-0x8019A800` = OK;
  ONLY `0x8019A800-0x8019B000` = OK; `0x8019A000-0x8019A900` = BROKEN; and
  `CLEAN_SKIP=0x8019a814` (force `cd_chan_full_init`→generated) inside that
  range = OK again → **`cd_chan_full_init` is the confirmed culprit**.
  ROOT CAUSE: the clean ports dereference MMIO through `guest_at()`. Cells like
  `0x801bdda4/8/ac/e4/f0` and `0x801bcb0c-0x801bcb20` hold POINTERS into the
  `0x1F8010xx`/`0x1F8018xx` INTC/DMA/CD register space. `*(u16*)guest_at(
  DB32G(0x801bdda8)) = 0` reads the cell→`0x1f801074` then does a RAW
  `guest_at(0x1f801074)` write — but `guest_at` only maps RAM/scratchpad, so the
  MMIO write lands in the open-bus fallback and is SILENTLY DROPPED (I_MASK/
  I_STAT/DMA never actually updated → CD IRQ never dispatches → `cd_cmd_issue`
  retries forever). Generated code uses `cpu->write_half/write_word` which route
  to the device models. FIX: route every MMIO deref through `br2_ld8/16/32` +
  `br2_st8/16/32` (br2_addr_direct → RAM/scratchpad via guest_at, else
  psx_read/write_* device) or the `br2_ldNp`/`br2_stNp` pointer-cell variants.
  Landed fixes: `cd_chan_full_init` (I_MASK/I_STAT/DMA init), `cd_state_blk_save`/
  `restore` (INTC ctx save/restore), `cd_chan_slot_set` (I_MASK RMW), `cd_cb7_run`
  (DMA chan reg), `cd_init_stage_a/b` (timer/DMA reg). STILL OPEN: ~50 more
  `guest_at(DB32G/DB32(0x801bccXX/0x801b84XX/0x801bcbXX))` sites across cd.c
  (memcard `0x801bcc*`, GPU-queue `0x801b84*`, CD data `0x801bcb*`) need the same
  audit — each cell's target (RAM vs `0x1f801xxx` MMIO) must be checked against a
  live RAM dump. NEXT: dump the full `0x801b8000-0x801be000` pointer-cell table,
  mechanically convert MMIO-pointing derefs to br2_ld/st, rebuild, re-run full
  clean until `0x801BCB24` byte0 holds `0x02`.

- **2026-11-11 (clean `game/` runtime integration — boots, CD IRQ stall localized):**
  Wired the clean-room `game/` C tree into the live recomp runtime as the
  canonical game implementation. `clean_try_dispatch` now runs first inside
  `psx_dispatch_game_compiled`; `src/clean_table.c` (generated by
  `game/gen_clean_table.py`, 1164 wrappers) adapts guest→host ABI, guest
  memory via `guest_at`/`guest_of`, and guest stack frames. BIOS services
  route through `bios_svc`→`psx_dispatch_call` (recompiled OpenBIOS, LLE);
  DI/EI route through `psx_syscall`; `call_guest`/`br2_call_generated` give
  clean code real nested guest dispatch; `gpu_invoke` bridges to generated
  libgpu. Fixed a `guest_at` region-top boundary wipe that zeroed host
  globals (killed `g_backend_count` → interpreter fallback → unknown-dispatch
  crash) and made `br2_irq_poll` advance `psx_cycle_count` so CD deadlines
  fire inside clean busy-waits. MMIO-aware `br2_ld*/br2_st*` accessors route
  CD/GPU/DMA/timer/SIO/SPU through device models.
  RESULT: game boots, issues real CD commands (Pause/Seek/ReadN observed in
  cdrom history), frames advance ~36k, `gp0_writes`/`draw` nonzero in an
  earlier config, `dispatch_stats` shows zero unknown-dispatch misses.
  CURRENT BLOCKER (localized, not yet fixed): the run wedges in
  `cd_cmd_issue`'s response wait — `clean_trace` shows only
  `cd_timer_read`(0x8019A460)/`cd_intc_busy_read`(0x8019A7D4) polling, and
  `bios_svc_trace` shows repeated svc-B0/0x35 printf = the timeout path.
  `pc_probe` confirms `cd_intc_dispatch`(0x8019A8EC), its caller
  `br2eu_func_8019A87C`, and the per-IRQ handlers (0x801979F8/0x8019AE68)
  are NEVER executed (count=0). CORRECTION to an earlier hypothesis: IRQs
  ARE delivered — `irqctx_ring` shows ~700k deliveries ongoing at frame
  27281 with `istat=0x5` (bit0 VBLANK + bit2 CDROM both pending),
  `imask=0xD`, `sr=0x40000401` (IE=1 at delivery). A transient
  `irq_state` sample showing `cop0_sr=0x40000404`/`IEc=0` was a DI
  critical-section moment, NOT the blocker. So the CPU takes maskable
  IRQs and vblank/game logic runs, but the game's CD dispatch chain is
  never reached: the registered IRQ handler that runs on each IRQ does
  not call `cd_intc_dispatch` for the CDROM bit. `cd_intc_busy_read`
  (0x801bcd1a) stays 0 → `cd_response_process` never runs → result flag
  0x801bcb24 never reaches 2/5 → timeout→printf→retry→black screen.
  `cd_intc_dispatch` reads mask = *I_STAT(0x1F801070) & *I_MASK(0x1F801074)
  & u16@0x801bcd48(=0xD) and calls tbl[11]@0x801bcd1c — it is the game's
  maskable-IRQ group dispatcher for bits {0,2,3}. NEXT: find the game's
  TOP-LEVEL IRQ handler (the address the kernel/BIOS exception chain calls
  on every IRQ — likely a wrapper that fans out to cd_intc_dispatch) and
  determine why the CDROM bit (istat bit2) does not reach it: missing
  handler registration, wrong dispatch table, or the running handler is
  the BIOS/kernel one that acks CDROM before the game layer sees it.

- **2026-08-31 (GPU DMA2 review correction — source gate passed):**
  The first fork review found two valid timing defects in the DMA2 candidate. The
  linked-list engine now reads and emits one live payload word at each
  one-clock boundary. A CPU rewrite after an earlier word transfers can now
  affect a later word. The optional widescreen prepass now fingerprints its
  cached nodes and commands. It discards all cached transform metadata if live
  RAM differs. The second fork review found three more valid issues. Late
  service now consumes every elapsed DMA boundary. Header and link rewrites
  now invalidate cached prepass topology at the exact header-read boundary.
  The obsolete opt-in polygon-drop filter was removed. The focused regressions
  and full build pass. The runtime suite passes 61 of 62 enabled tests. The
  remaining `mod_runtime_test` crash reproduces on the unchanged upstream base,
  and two pre-existing tests remain disabled. Fresh Spot and Vampire Hunter D
  builds also pass their
  600-frame headless gates. Visible software-renderer routes and another fork
  review are still required before the public branch can change.

- **2026-07-28 (per-game host audio cushion — implemented, parser validated):**
  Added `[audio] buffer_ms` as a runtime-only developer setting with a guarded
  30–500 ms range. The compatibility default remains 180 ms, preserving the
  reserve required by titles with long streamed-stage production gaps; a game
  may opt into a lower target after validation to reduce audible latency.
  `audio_stats` and the opt-in runtime cadence report now expose both actual
  fill and configured target. This changes only the host playback bridge and
  does not alter guest SPU state, instruction timing, or code generation.
  Recompiler build and all 33 registered tests pass.

- **2026-07-27 (BIOS boot-skip parity across BIOS images — FIXED, validated):**
  `bios_hle`'s boot-skip did nothing under the bundled OpenBIOS. Root cause was
  ordering, not policy: `main.cpp` correctly forced the *kernel-call* tier off
  when an image exports no `deliver_event_ret`, by assigning `bios_hle = false`,
  and then derived `boot_skip` from that already-mutated flag. OpenBIOS omits
  `deliver_event_ret` (B0 semantics unvalidated) but DOES export
  `shell_entry_phys`, so the skip was structurally available and got cancelled
  anyway — one player-facing flag meaning two different things depending on a
  BIOS detail no player can see. Enhancement-phase (load-time) defect; no
  timing-core change.
  Fix: the two axes are now decided by one pure, dependency-free function,
  `psx_bios_hle_plan()` (`runtime/src/bios_hle_plan.c`), which reads the
  REQUESTED `bios_hle` for the boot decision and gates each axis on only the
  anchor it actually needs; refusals are reported at startup instead of silently
  downgrading. `psx_bios_hle_configure()` additionally clamps `boot_skip` to
  `shell_entry_phys != 0` so the banner and `hle_dump` cannot overstate what can
  fire. New `bios_hle_plan_test` (16/16 runtime ctest green) pins the matrix,
  including an exhaustive 64-case sweep asserting the boot decision is
  independent of `deliver_event_ret` and the call decision independent of
  `shell_entry_phys`.
  Validated on Ape Escape (SCUS-94423) built against this framework, both BIOS
  backends linked, headless: OpenBIOS → `bios_boot=skip to game (shell
  skipped)`, `hle_dump route=2` shows the one-shot fire at `vec 0x30000`,
  `ra=0xBFC06EA8` (the exact continuation `bios/OpenBIOS.toml` documents),
  game running on screen. Retail SCPH-1001 → same banner, same `vec 0x30000`
  fire at `ra=0xBFC0702C`, game running; differs only in
  `bios_backend=HLE (LLE fallback)` vs `LLE (recompiled BIOS)`. Negative control
  `PSX_BIOS_HLE=0` on OpenBIOS → `bios_boot=real intro`, ring total 0 (hook not
  installed), display 640x478 shell mode: the flag still turns the skip off.
  Also in this pass: a BIOS profile's `[runtime]` block was parsed into
  `BiosConfig::runtime` and read by NOTHING, and `bios/OpenBIOS.toml` carried an
  inert `bios_hle = false` there that read as the reason HLE was off.
  `load_bios_config` now REJECTS `[runtime]` in a BIOS profile. And
  `psx_icache_fastpath_test` had been unbuildable ("redefinition of
  `psx_advance_cycles`" — the test's counting stub vs the header's `static
  inline`), which blocked every runtime test registered after it; it now builds
  with `PSX_OVERLAY_DLL_BUILD=1`, the seam `psx_cycles.h` already provides.

- **2026-07-21 (VLC load-charge batching — shipped; dual still ~22 ms):**
  Runtime-only batch: under `psx_next_service_cycle`, `psx_cyc_charge`
  accumulates into `g_psx_cyc_batch` (no per-insn `psx_cycle_count` store);
  flush at IRQ check / MMIO sync / savestate / advance-past-deadline.
  Absorb/fudge still per-insn; guest totals at barriers unchanged; no MotK
  regen. Dual headless MotK (`PSX_NETPLAY_TIMING=1`, pinned halves): heavy
  25–50 band host med ≈39.7 fps / guest ≈21.9 ms/f / admit ≈3.1 (guest peer
  ≈39.8 / 22.5 / 2.8) — same floor as pre-batch (~21 ms / ~40–42). Counter
  publish was not the dual-peer tax; residual remains load-delay volume /
  LLC under phase-locked VLC. Next: PGO retrain after hot-path edits, or
  accept same-machine lockstep FMV floor.
- **2026-07-21 (MotK FMV host cost — MDEC/IRQ/charge; dual still ~21 ms):**
  Aimed to cut MotK FMV host work so two lockstep peers fit ~16.7 ms/f.
  Shipped bit-exact host opts: MDEC MB output reserve (no per-byte
  ensure_capacity), sparse-column IDCT, ch0 DMA burst feed
  (`mdec_dma_write_words`), sticky-IRQ undeliverable early-out in
  `psx_check_interrupts`, `psx_cyc_charge` pre-deadline bump on compiled
  loads/steps. Dual headless MotK (`PSX_NETPLAY_TIMING=1`, pinned halves):
  band 25–50 fps still guest ≈21 ms/f / fps ≈40–42 (admit ≈2.5–3) — same
  floor as before. HARD_CAP 16K→64K tried, no gain (real CD/timer events
  already shorten the deadline); reverted. Residual is phase-locked dual
  VLC load-delay volume / LLC contention, not MDEC FIFO or present/admit.
  Next: emitter-level load-charge batching for VLC leaves, PGO retrain
  after these hot-path edits, or accept same-machine lockstep FMV floor.
- **2026-07-21 (netplay FMV — lockstep guest inflation, not present/admit):**
  User A/B: two windowed offline MotK intros fine; headless netplay FMV still
  slow vs offline headless. Opt-in `PSX_NETPLAY_TIMING=1` splits the [FPS]
  line into guest ms/f vs admit ms/f. Heavy FMV (~25–50 fps samples): guest
  ≈21 ms/f, admit ≈3 ms/f (median) — frame time is dominated by the guest
  quantum under phase-locked dual MDEC, not Swap and not INPUT_CONFIRM wait.
  Offline headless same stretch is ~17 ms/f (≈57 fps). Pipelined CONFIRM
  tried in recomp-net (tip publish, drain next tick) — no FPS gain on
  localhost (~41→~41.5); reverted. Present-path / half-rate work is a dead
  end for this regression. Next lever: reduce MotK FMV host cost so two
  aligned peers fit a 16.7 ms budget (or accept same-machine lockstep floor).
- **2026-07-21 (netplay FMV — restore present-before-admit):**
  User confirmed early same-machine netplay FMV was ~50–60 and gameplay
  under lockstep stays ~60 — so the rematch-safe `finish→admit→pace→present`
  order was the FMV regression (expensive depth24 CPU present after admit).
  Restored `finish→present→admit/pace` via RAII `NetplayVblankTail` (admit
  on every return path; offline still paces before present). Kept half-rate
  depth24 present skip + UDP `poll()` barrier; no `SDL_Delay(0)`. Verify
  MotK intro FPS + tick-0 arm after rebuild.
- **2026-07-21 (netplay FMV — re-land half-rate depth24 present):**
  Windowed same-machine MotK netplay FMV was back at ~30–40 after the
  rematch-safe `finish→admit→pace→present` order. Re-landed host-only
  half-rate depth24 present: after admit, skip pace+Swap every other
  depth24 vblank (present first, then alternate); admit/barrier unchanged
  (no `SDL_Delay(0)` / present-before-admit). Offline path untouched.
  Rebuild MotK `build-release` + verify intro FPS and tick-0 arm.
- **2026-07-21 (lobby game_version + release pins):**
  WS lobby now carries `game_version` alongside `game_name` (create/list/join).
  Server rejects `version_mismatch` / `game_mismatch`; list can filter by either.
  MotK/MW bake release pins from repo `VERSION` (Release → e.g. `0.1.0`, else
  `dev`) via `PSX_GAME_VERSION` / `SNESRECOMP_BUILD_VERSION`. Clients send the
  pin on create/join and filter the lobby browser. Redeploy lobby server for
  remote matchmaking. Docs: `recomp-net-server/docs/WS_LOBBY.md`.
- **2026-07-21 (portable .pst / boot_state v3 LE wire):**
  Savestate / boot_state version → 3: header + section framing and all
  module snapshots emit little-endian field wires (`pst_wire.h`) — no
  host-struct padding (TimerRegs, DMA async/delayed, SpuVoice, McSlotState,
  CDROM Pending/Queued). Netplay host→guest blob transfer is identical on
  Win/Linux x86_64 and macOS ARM. Old v2 `.pst` files are rejected (recapture).

- **2026-07-20 (netplay match_caps — host settings enforce):**
  Lobby `create` / `set_match_caps` / `start` carry host sim caps
  (aspect, turbo_loads, bios_hle, fast_boot, auto_skip_fmv, input_delay,
  language). Server echoes on join/update/launch; guests apply before boot.
  recomp-net-server + psx_lobby_client + MotK launcher. SNES mirror:
  widescreen/hud/ignore_aspect/input_delay/ws_extra via snes_lobby + MW main.

- **2026-07-20 (launcher: persist controller selection immediately):**
  Device/mode/deadzone now write `settings.toml` on change (not only Launch),
  so Refresh / Quit / soft-return keep the pad. Refresh falls back to saved
  GUID if the dropdown index is stale.

- **2026-07-20 (lobby default → public host):**
  `psx_lobby_default_url` now `ws://netplay.retcomm.net:8765`
  (match SNES); override still `PSX_NET_LOBBY_URL`. Synced MotK vendored
  `psx_lobby_client.{c,h}` + recomp-net `docs/lobby.md`.

- **2026-07-20 (Metal Warriors H2H: top-edge prop pop):**
  Full-frame present recenters dual cam ~$40 up; spawn/OAM top was only
  −$70/−$70 so platforms popped at Y=0. Spawn −$A8, OAM CMP −144, present
  Y wrap peek for −64..0, dist-limit +64 when vert-widen.

- **2026-07-20 (launcher: controller Refresh rescan):**
  Dashboard Device row (P1 + offline P2) has Refresh — pumps SDL joysticks,
  re-enumerates gamecontrollers, keeps selection by GUID. Offline + netplay.

- **2026-07-20 (launcher: lobby lock emoji via symbol fallback font):**
  Password lobbies showed □ for 🔒 because the primary face has no emoji.
  Load a symbol fallback face through the shared Dear ImGui font atlas so
  missing glyphs resolve.

- **2026-07-20 (launcher lobbies: button order + dblclick join):**
  Lobbies actions: Return to Launcher → Change Player Name → Host Game →
  Join Lobby. Double-click a lobby row joins (same path as Join Lobby,
  including password modal).

- **2026-07-20 (launcher: offline↔netplay switch buttons):**
  Dashboard footer: "Switch to Netplay" beside Launch Game (only when
  `PSX_HAS_RECOMP_NET`); "Switch to Offline" beside Netplay Lobbies.
  Home Netplay tile gated the same way; no-netplay builds skip home chooser.

- **2026-07-20 (netplay load — apply freeze after hash match):**
  Suppressing INPUT at `np_begin_load_apply` deadlocked both peers in
  `netplay_barrier_admit`: tips stopped → `try_admit` never succeeded →
  guest never ran → `savestate_poll` never applied. Fix: keep INPUT during
  APPLYING; suppress only at `np_enter_load_ready` until `hard_resync`+prime.

- **2026-07-20 (netplay load — false peer_disconnect → lobby):**
  Hash-match apply suppresses INPUT for seconds → `peer_disconnected(1500)`
  fired → soft-exit to lobby → rematch. Fix: timeout=0 (BYE-only) while
  `in_load_barrier`; HELLO keepalive every 250 ms during suppress/stall.

- **2026-07-20 (netplay post-load — stale INPUT clobber):**
  2nd+ loads: correct frame, then ~3–5s frozen while FPS lived. Cause: during
  LOAD apply/ready the slower peer kept emitting pre-resync INPUT tips; those
  ticks share ring slots with the new tip (`tick % 128`) and first-wins /
  overwrite races blocked `remotes_ready_for_sim` after `hard_resync`. Fix:
  suppress INPUT sends for the load barrier; reject out-of-window remote ticks;
  host `probe_finish` before sync+prime; re-anchor frame pacer on restore.

- **2026-07-20 (savestate load — force GL present after identical frame):**
  2nd+ load of the same `.pst` left FPS climbing while the picture stayed
  frozen: `gl_renderer_present_vram` / wide early-out skipped `SwapWindow`
  when display rect + present-dirty matched the last swap (common after
  restoring into an already-shown frame). Fix: `gl_renderer_invalidate_present`
  marks all present tiles dirty, clears path latches, resets interp history,
  and forces 8 presents; called from `psx_frontend_on_savestate_loaded`.

- **2026-07-20 (netplay post-load — admit barrier symmetric):**
  Host dropped `LOAD_READY` before `try_admit` (guest did not) → confirm
  wait with barrier already down; keeping remotes let stale tip=D
  first-wins. Fix: both stay in `LOAD_READY` until admit; `hard_resync`
  clears remotes again; sync+prime at mutual ready only.

- **2026-07-20 (netplay post-load — mutual-ready sync):**
  `hard_resync`+prime at apply let the later peer wipe the earlier tip
  (2nd load slower). Sync once at mutual ready. (Remote-keep reverted —
  see admit-barrier note above.)

- **2026-07-20 (netplay post-load — symmetric ready release):**
  Guest cleared `LOAD_READY` on READY ACK while host still had
  `state_stall_sim` → confirm wait / intermittent hitch. Guest now ACKs
  but stays in `LOAD_READY` until `try_admit` succeeds (pre-sends
  INPUT_CONFIRM so host can admit on the same poll as `probe_finish`).
  Ready-probe retransmit 40→8 ms; confirm retransmit 16→4 ms. MotK rebuild.

- **2026-07-20 (netplay post-load resume — frozen picture + FPS):**
  After load, FPS kept climbing while the window stayed on a stale/blank
  frame: (1) present blank-latch skipped redraw after restore; (2) delay
  rings empty after `hard_resync` while peers could still advance during
  APPLYING. Fix: `hard_resync` resets `sim_tick→0` + `prime_delay_inputs`;
  stall admit for APPLYING when `!savestate_pending` and all LOAD_READY;
  `psx_frontend_on_savestate_loaded` forces restage/blank once; skip FPS
  CLI during load barrier. MotK rebuild.

- **2026-07-20 (netplay host-only save/load commands):**
  User `savestate_request_*` refused on netplay guest; F-keys / debug TCP /
  `PSX_LOAD_SLOT` host-only or routed via `psx_netplay_request_*`. Guest
  follow-host sync uses `savestate_request_*_protocol`.

- **2026-07-20 (netplay load — post-restore lockstep rendezvous):**
  Hash-match load applied on each peer at different times after early
  `hard_resync` → rings/ticks drifted → admit hang / starvation. Fix:
  stage load → apply while admit runs → `hard_resync` only after restore →
  LOAD size=0 ready probe until both ACK → then resume. Heartbeat in
  admit barrier. MotK rebuild.

- **2026-07-20 (netplay save hang — coord probe must not stall):**
  Shift+F1 stalled admit before `savestate_poll` could write → deadlock.
  Fix: `STATE_PROBE` with `size==0` (coord) leaves admit running; only
  hash probe (`size!=0`) + chunk transfer stall. Guest retransmit replies
  without re-staging saves. MotK rebuild after sync.

- **2026-07-20 (netplay host-owned saves — hash probe + chunk transfer):**
  recomp-net: `RNET_STATE_MAX` → 8 MiB, `STATE_PROBE`/`PROBE_REPLY`, stall
  admit through probe + transfer; restored `rnet_session_wait_recv`.
  psx_netplay: guest sandbox `saves/netplay/`, host-only F-keys, match-start
  memcard probe, save = coord local write → hash-agree → transfer on miss +
  post-CRC verify; load same pattern + `hard_resync`. MotK `build-release`
  linked; verify Shift+F1 / F1 across LAN.

- **2026-07-20 (MotK title after FMV — leave-depth24 restage):**
  On exit from GP1 depth24, GL/VK `depth24_upload_policy` restaged full
  CPU VRAM as 1555 into the FBO. CPU still held packed RGB888 from MDEC →
  rainbow/static title background (text/overlays still drew as prims).
  V2: skip only framebuffer-sized depth24 transfers (keep texture A0s);
  on leave scissor-clear the skipped FB union (GL) — never blind-restage
  RGB888-as-1555. Char-select shrink-to-left-center still under probe
  (OFX=256 @ 512 CRTC looks correct; may be authored layout / separate).

- **2026-07-20 (MotK 2nd intro right-edge stretch):**
  `depth24_fix_trailing_margin` replicated the last good column when any
  chroma>40 pixel sat in the trailing 8 cols. On the starfield FMV that
  smeared stars into an 8-wide flickering strip. Now requires dense chroma
  (~12% of margin) and black-fills instead of column-replicate. Still no
  CRTC/content_w crop.

- **2026-07-20 (MotK netplay FMV — lockstep floor, not present path):**
  A/B: offline headless FMV ~59; two offline headless concurrent ~59; two
  netplay headless ~38–40. Not dual-CPU contention and not GL present.
  Lockstep (INPUT_CONFIRM frame barrier + same-tick input rendezvous)
  keeps both MDEC peaks aligned. Async confirm / peer drift made FMV
  *worse* (~22–28) via overlapped memory traffic. Barrier now UDP `poll()`
  (not `SDL_Delay(1)`); localhost peers pin to disjoint CPU halves (~45
  in A/B). Pipeline admit rewrite hung tick-0 — reverted.

- **2026-07-20 (MotK netplay FMV — restore pre-rematch vblank order):**
  User: same-machine netplay was 50–60 before rematch playback tweaks.
  Reverted `NetplayVblankGuard` present-before-admit and half-rate depth24
  present. Order is again finish→admit→pace→present. Kept: `s_present_w/h`
  clear (black rematch FMV), depth24 FBO upload skip, trailing-margin
  in-buffer fix, vsync-off while lockstep armed.

- **2026-07-20 (MotK netplay tick-0 hang — revert admit latency hacks):**
  After half-rate present, both peers armed lockstep then sat at frame 0
  until peer_disconnect. Cause: `SDL_Delay(0)` busy-spin starved peer UDP
  on dual localhost; same-call `try_admit` publish when CONFIRM pre-seen
  also unsafe. Reverted both.

- **2026-07-20 (MotK FMV netplay FPS — half-rate depth24 present):**
  Measured: headless dual-peer lockstep holds ~60 guest FPS through intro;
  windowed dual-peer ~30–40. Bottleneck is two GL CPU-presents serializing
  before the guest fiber resumes — not admit/guest. Fix: under netplay +
  depth24, present every other vblank (host-only; admit still every tick).

- **2026-07-20 (MotK FMV netplay FPS — skip depth24 FBO upload queue):**
  MDEC A0 was still queued as 1555 CPU→FBO uploads (`UP_RECTS_MAX`=16),
  force-flushing mid-movie. While `gpu_display_is_depth24()`, do not queue
  GL/VK uploads; on leave, drop queue (no full restage — see leave-depth24
  log above). Alone did not restore
  windowed netplay to offline rates (present cost remained).

- **2026-07-20 (netplay FMV host FPS — present/vsync ordering):**
  Offline MotK intro ~50+; netplay ~30–40 was host path, not guest
  divergence. Fixes (determinism unchanged): (1) force GL/VK swap
  interval 0 while lockstep is armed (restore on soft-exit) so driver
  vsync does not double-block after the wall pacer; (2) move
  `finish_frame`→present→`admit`+pacer so local Swap overlaps the peer's
  guest quantum. `turbo_loads` stays off in netplay.

- **2026-07-19 (MotK FMV right-edge — no present-width crop):**
  Upload-span + `content_w` left-aligned GL crop removed the chroma junk
  but replaced it with a flickering black pillar (span varied per frame,
  especially during lighting). Rework: keep full CRTC width always;
  `depth24_fix_trailing_margin` only replicates the last good column
  into the last 8 RGB cols when chroma junk is detected — in-buffer,
  no viewport shrink. Half-texel nearest UV clamp remains.

- **2026-07-19 (MotK FMV right-edge — trailing margin, not CRTC shrink):**
  Root cause: MotK depth24 crawl is 512×128 CRTC, but ~8 trailing RGB
  columns are stale/black in VRAM; GL edge sampling flickered that strip
  as garbage. Fix (no 2/3 width): track A0 upload span
  (`gpu_depth24_rgb_limit`); blank/crop last 8 cols on short depth24
  bands (`h<240`); `gl_renderer_present(..., content_w)` left-aligned
  UV crop + half-texel nearest clamp; screenshot skips `sync_cpu` on
  depth24 (was clobbering RGB888). MotK release+PGO rebuilt; user-verify
  2nd intro (Star Wars logo) edge + ~50 FPS.

- **2026-07-19 (MotK FMV right-edge / 2/3 revert):**
  Tried depth24 width=(CRTC*2)/3 (512→341) for right-edge junk; MotK
  intros rendered left-shifted with the right of the video clipped —
  confirms the Jul-18 finding (logo is centered in a 512 RGB line).
  Reverted 2/3. Kept: depth24→fmv_frame, short-band without force_4_3
  gate, nearest present on depth24. Right-edge junk needs a different
  fix (not shrinking CRTC width).

- **2026-07-19 (MotK FMV FPS after rematch patches):**
  Rematch video fixed; ~30–40 vs prior ~50+ was not a present-path
  regression. Prior ~50 med was MotK intro PGO (`PSX_PGO=use`); LTO-only
  baseline is ~39. Mistakenly cleared PGO — restored `PSX_PGO=use` with
  existing intro `.gcda`. `fmv_frame` restored (depth24 only forces
  `pin_43` short-band letterbox). Re-train via `scripts/pgo_motk_intro.sh`
  after large rematch edits if profiles go stale.

- **2026-07-19 (netplay rematch: black FMV = stale present tex size):**
  Root cause: `s_present_w/h` survived GL context destroy; rematch FMV same
  size as prior CPU present took `glTexSubImage2D` into a new unallocated
  `s_present_tex` → black. Cleared on shutdown + init_context. Also dropped
  per-frame `flush_cpu_uploads` on depth24 (was cutting intro ~50→~30 FPS);
  depth24 CPU scanout needs neither FBO sync nor upload flush.

- **2026-07-19 (netplay rematch: black FMV, audio OK):**
  Rematch linked and played, but MotK intros had XA audio with black video
  (FPS still dipped ~30–40 → MDEC ran). Earlier mis-attribution to sync_cpu;
  also reset present/FPS/MDEC/iso session statics; `iso_close` before reopen;
  depth24 forces 4:3 pin for short-band letterbox.

- **2026-07-19 (netplay rematch: sticky I_STAT/I_MASK):**
  After cycle-reset fix, rematch still starved: dump meta showed
  `psx_cycle_count=4` with leftover `i_stat=VBlank` + game `i_mask`.
  `interrupts_init` / `memory_init` now clear I_STAT/I_MASK (+ mem_ctrl);
  `starvation_ring_reset` on `session_reboot` so dumps are rematch-clean.

- **2026-07-19 (stick→D-Pad axial deadzone again):**
  Radial+sign for digital stick→D-Pad made left/right fire Up/Down from tiny
  Y drift (jump/crouch). Stick→button sources use per-axis `controller_deadzone`
  again; analog `axes_to_pad_pair` / hybrid stick-detect stay radial.

- **2026-07-19 (netplay rematch: stuck `psx_in_device_service`):**
  Rematch still froze after `lockstep armed`: soft-exit longjmps out of the
  vblank callback while inside `psx_devices_service_to_now`, leaving
  `psx_in_device_service=1`. Every later `psx_advance_cycles` then skips
  device service → no vblanks → hang after tick-0. Fix: clear the guard on
  every scheduler longjmp escape; `psx_cycles_reset_for_boot()` zeros the
  guest clock + deadline bookkeeping at `session_reboot`.

- **2026-07-19 (Digital stick→D-Pad radial deadzone):**
  Stick-as-D-Pad used a per-axis square threshold so centre drift twitched
  movement even with a large launcher deadzone. Stick axis→button sources now
  require radial magnitude past `controller_deadzone` (same idea as
  `axes_to_pad_pair`); triggers stay per-axis. Hybrid stick-detect matches.

- **2026-07-19 (netplay rematch HLE shell-skip latch):**
  After soft-return rematch both peers linked (`lockstep armed`) then froze:
  `s_shell_skipped` stayed set from the first match so HLE boot-skip never
  re-fired and both ran the interactive BIOS shell under netplay.
  `psx_bios_hle_configure` now clears the latch; `cdrom_init` resets boot
  disc speed to 1x; lobby launch uses `input_player=-1` (auto) again.

- **2026-07-19 (netplay rematch session_id + endpoint guard):**
  Rematch HELLO hang: server now allocates a fresh `session_id` on every
  `start`/`launch` (stale BYE/HELLO from the prior UDP session no longer match)
  and refuses start when host/guest endpoints are empty. Client refuses
  `fill_netplay_launch` / `launch_pending` when `peer_hostport` is missing.

- **2026-07-19 (netplay return-to-lobby rematch):**
  Lobby WS stays up across Launch. Window-close / Escape / peer BYE soft-exits
  via `PSX_RUN_RETURN_TO_LOBBY` (scheduler longjmp or pre-entry flag) instead of
  `exit(0)` when the match started from a lobby room. Teardown keeps the WS;
  launcher resumes on `netplay_room` with ready cleared; rematch Launch
  re-enters `session_reboot` (re-init guest + netplay). Server clears ready on
  `start` so both peers must Ready again.

- **2026-07-19 (lobby server → closed-source Rust):**
  Proprietary `recomp-net-server` (Rust) owns WS lobby + privacy/docs;
  removed C `servers/lobby` from open `recomp-net`. Client WS helpers
  vendored at `runtime/src/lobby_ws/`. Default
  `ws://netplay.retcomm.net:8765`.

- **2026-07-19 (netplay lobby server + launcher menus):**
  Lobby WS+JSON owned by proprietary `recomp-net-server` (was C
  `servers/lobby/` under open recomp-net);
  `psx_lobby_client` + shared launcher home → Offline / Netplay → lobbies table
  (host/join/password). Launch hands `PsxNetplayConfig` to
  `psx_netplay_start` (LAN endpoints from lobby). ICE relay stubbed.

- **2026-07-19 (netplay peer disconnect QoL):**
  Barrier `SDL_PollEvent` on `SDL_QUIT`/Escape → `shutdown_runtime`+exit.
  recomp-net `BYE` (pkt 7) + `rnet_session_peer_disconnected(~1.5s)`;
  `psx_netplay_shutdown` sends BYE. Surviving peer prints and exits instead
  of spinning in admit.

- **2026-07-19 (netplay latch + INPUT_CONFIRM + exclusive capture):**
  Host stages one pad per sim tick (`latched_for_tick`); barrier only
  re-samples via `needs_local_sample`, stalls on `input_desync`
  (INPUT_CONFIRM hash mismatch). Netplay capture is exclusive to the
  assigned PlayerInput (no keyboard-all / all-controllers merge) so peer
  hashes agree. Cleared on `finish_frame` advance. recomp-net: preserve
  early peer INPUT_CONFIRM when activating (wipe raced slower peer into
  permanent stall). Smoke: two headless MotK peers both `lockstep armed`,
  frames advance, no INPUT desync.

- **2026-07-19 (netplay lockstep stall + per-peer input device):**
  True delay-sync gate: pre-scheduler + each vblank `finish_frame` then
  blocking `poll_admit` (guest fiber parks; no free-run on admit fail /
  linking). Auto/`--net-input-player`: host samples P1 device, guest samples
  P2 when assigned (same-PC C40+keyboard); pad blob deadzone normalize.

- **2026-07-19 (netplay pad ownership — session slots always plugged):**
  Host local device → net-slot 0 (sim P1); guest local → net-slot 1 (sim P2).
  While active, SIO is network-only (no local/`override` writes). Both session
  ports stay connected from `psx_netplay_start` through linking so in-game
  2P/VS detect works; `refresh_player_devices` no longer clears them.

- **2026-07-19 (delay-sync netplay bring-up — recomp-net LAN):**
  Wired `recomp-net` into the runtime as CLI/env LAN delay-sync (not GGPO).
  `psx_netplay.{c,h}` + CMake auto-discover `../recomp-net`; vblank owns
  `pump`/`try_admit`/`publish`/`advance`; local pads stage only — publish is
  sole SIO writer while active. Turbo + low-latency re-sample gated off.
  Lobby UI / ICE server deferred. Smoke: two procs with `--netplay --net-slot`.

- **2026-07-19 (MotK title/char-select — savestate PC + flat GEO batch):**
  User still saw ~10 FPS after draw-area reject. Real char-select profile:
  ~30k/s on-screen GP0(68h) starfield dots, `gpu_share`~0.8 (not empty clip).
  Also: `boot_state_load` forced `pc=entry_pc`, so F1 loads desynced (display
  off; false ~60 FPS). Restored saved PC; batched flat GEO tris in
  `gpu_gl_renderer.c`. Char-select Shift+F1: **~60 FPS locked**, gpu_share~0.06.

- **2026-07-19 (MotK title/char-select — GPU draw-area reject re-applied):**
  Shift+F1 title + Arcade char select were &lt;10 FPS: same OT drain as the
  inter-movie cliff — GP0(E3/E4)=(0,0)-(0,0) + thousands of clipped `0x68`
  dots / quads; GL built 2 tris/prim. Re-applied inclusive draw-area reject
  in `gpu.c` (prior revert blamed a “gap race”; crawl wrap was the separate
  24bpp present-width bug). Alone insufficient for live starfield menus.

- **2026-07-19 (MotK intro — native/hot/inline + multi-run PGO):**
  Shipped `-march=native`, `[recompiler] hot_funcs` for VLC
  `0x8006A9F8`/`0x8006CBE4`, Release-inline `debug_server_log_call_entry`
  (cpu_state.h), HIT-inline `psx_icache_fetch` (psx_icache.h), multi-run
  PGO train (`PGO_TRAIN_RUNS`/`PGO_TRAIN_SECS`). Remeasure clean logo still
  **~51 med** (more mid/high-50 samples; not locked 60). Load-delay cycle
  volume remains the ceiling. Inter-movie cliff open. No MotK VSync HLE.

- **2026-07-19 (MotK intro — PGO + advance_cycles host cost):**
  (1) `psx_advance_cycles`: drop per-charge watchdog/PC-sample (moved into
  `service_to_now`, HARD_CAP cadence); (2) `psx_devices_mmio_sync` recomputes
  deadline in-place instead of dirtying `next_service=0` (was forcing service
  on the next insn after every GPU/CD/MDEC MMIO); (3) MotK intro PGO via
  `scripts/pgo_motk_intro.sh` (`-DPSX_PGO=generate|use`, 311 .gcda). Clean
  logo (until inter-movie cliff): **~49–51 med** (was ~39 LTO-only). Crawl
  after gap recovers ~47–50. Inter-movie ~7 FPS GPU cliff still open. No
  MotK VSync HLE.

- **2026-07-19 (MotK intro — VLC host opts + Release LTO):**
  Host-side work on MotK VLC (`0x8006A9F8`/`0x8006CBE4`), not disc cache:
  (1) idle_skip no longer defeats IRQ fast-path; deferred idle GPR snap;
  (2) IRQ mid-path when bits already pending; (3) `psx_slice_block` header
  inline when parked; (4) main-RAM `psx_cyc_load_word`/`half` inlined;
  (5) MotK Release `CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE` (-flto).
  Logo window: ~34 med → ~39 med (samples mostly 38–40). Still short of 50;
  load-delay cycle volume remains the ceiling (PSX_LOAD_DELAY=0 → hundreds
  FPS). Inter-movie ~5–7 FPS GPU cliff open. No MotK VSync HLE.

- **2026-07-18 (MotK intro — FMV pace: inline cycle advance):**
  Host FPS still short of 50 with real MDEC/XA. Inlined `psx_advance_cycles`
  (deadline fast path) + merged load fudge/cost into one charge; bit-exact
  DC-only MDEC IDCT. Release FMV ~33 med / ~39 avg (steadier; was dipping to
  teens). Gap ~7. Remaining: static VLC `0x8006A9F8`/`0x8006CBE4` + load-delay
  host cost. Do not revive MotK VSync HLE / HARD_CAP without mdec rising.

- **2026-07-18 (MotK intro — 24bpp CRTC width + short-band present):**
  MotK FMV CRTC is 512 (X1/X2÷5); logo centered in 512 RGB (~86..431).
  Reverted blanket 24-bit mode×2/3 (341 cropped the right). Width = GP1(06h)÷
  dot-clock for 15/24-bit. Short GP1(07h)=128 no longer fills 4:3 — present
  letterboxes src_h/240 (GL/SDL/VK). Inter-movie GPU cliff still open.
  CD-only `spu_render` kept.

- **2026-07-18 (MotK intro FMV — false 60 FPS / no video; rollback):**
  MotK `load_accel.vsync_query` + event-horizon_any / HARD_CAP→564480 /
  in-exception VBlank chunking produced host FPS ~60 with **no MDEC**
  (`mdec_decode_count` stayed 0, display disabled) — guest time raced past the
  STR. A/B: VSync(-1) HLE alone is enough to keep MotK MDEC at 0; disabling
  it restores decode/XA. Reverted those accelerations for MotK; kept sticky
  CD IRQ deadline, load-delay coalesce, MDEC DMA bulk/IDCT skips. Real intro
  with video is again ~30–40 FPS host-bound under load-delay. Do not claim
  MotK FMV pace wins without `fmv_state.mdec_decode_count` rising.

- **2026-07-18 (earlier same day — sticky CD deadline + load-path host cost):**
  `cdrom_cycles_to_irq` sticky presented IRQ → 1-cycle deadline after sync;
  fixed. Load-delay coalesce + inline `psx_advance_cycles`. Necessary but not
  sufficient for MotK FMV ≥50 with video.

- **2026-07-11 (Tomba 2 OpenGL full-attract performance + audio acceptance):**
  Resolved the shared renderer/overlay/capture cascade that made Beach, Whoopee
  FMVs, Mines, and Mine Cart slow. OpenGL now avoids mandatory present readback,
  batches Tomba's painter-ordered blend stream, and suppresses unchanged 30 Hz
  source presents. Overlay dispatch now distinguishes exact lazy entries from
  CPS interior continuations: continuations use the loaded range owner first,
  while exact entries reached inside local dirty flow can publish their cached
  native DLL. The hot `0x80106424`/`0x80106688` FMV helpers consequently dropped
  from ~60 interpreted entries/frame to zero. Small dynamic-text DLL images are
  mapped on a bounded worker (141 Tomba variants, not the 712-DLL vault), and
  auto-capture base64/file I/O now runs from a coherent RAM/seed snapshot on a
  low-priority worker. The audio bridge uses real callback duration, bounded
  P-only correction, and a measured 160 ms reserve inside its existing 250 ms
  ring. Release acceptance: 540 s / 107 five-second records, Beach -> Whoopee ->
  Mines -> Mine Cart -> repeat, guest min 59.76 Hz / median 59.94 Hz, **zero
  output underruns**, zero post-start overflows, and no cache growth (849 DLLs).
  Current-code screenshots visually confirmed Beach, FMV, Mines, and Mine Cart.

- **2026-07-10 (Tomba 2 Whoopee auto-skip dwell + native-wide Beach backdrop):**
  Added an opt-in silent-MDEC post-decode hold so presentation-side FMV
  auto-skip remains unpaced through a preloaded logo's authored release wait;
  the faithful guest timeline is unchanged and the default remains four
  vblanks. Added title-opted native-wide mirror gates for flat primitives and
  the textured pre-shaded backdrop phase. This keeps the canonical 4:3 buffer
  untouched while filling Tomba 2 Beach Town at 16:9 and 21:9 without stretching
  the later 3D foreground. Also fixed JSON escaping in the `fmv_state` path and
  extended its resolved-config reporting. Validated Release build, unattended
  first-attract captures at 4:3/16:9/21:9, and zero unknown dispatches.

- **2026-07-02 (HLE PIVOT implemented — HLE as a first-class swappable tier, gbarecomp model):**
  USER-DIRECTED pivot (supersedes "no HLE" §0; CLAUDE.md amended 2026-07-02, memory
  hle_tier_architecture.md). Built the full stack this session: (1) EMITTER —
  full_function_emitter.cpp now emits a null-by-default `g_psx_bios_hle_hook` consult at
  the top of every psx_dispatch_impl iteration (pre-normalize phys, BEFORE the game/
  dirty-RAM/static backends; handled ⇒ resume at $ra; NULL default = pure LLE,
  dispatch-identical). (2) RUNTIME TIER — runtime/src/bios_hle.c(+.h): v1 call-HLE =
  the B0 event family (DeliverEvent/OpenEvent/CloseEvent/TestEvent/EnableEvent/
  DisableEvent) ground-truthed against the SCPH1001 kernel disassembly (Ghidra,
  0xBFC11644..0xBFC11A84; EvCB [0x120]/[0x124], stride 0x1C), operating on the real
  guest EvCBs, callback delivery via psx_dispatch_call with the kernel-true $ra
  0x1720; everything else (WaitEvent/threads/pads/card/A0/C0) falls through to LLE.
  (3) BOOT HLE — one-shot shell-entry intercept (RAM 0x30000; LoadRunShell's indirect
  call always dispatches): real recompiled kernel init + SYSTEM.CNF + EXE load run
  authentically under boot-turbo; only the shell (boot animation) is skipped. The old
  fast_boot snapshot restore is REMOVED (fast_boot=true now aliases boot-skip only).
  (4) SELECTION — [runtime] bios_hle / bios_hle_keep_intro / hle_scheduler in
  config_loader (+ settings.toml bios_hle mirror), PSX_BIOS_HLE / PSX_BIOS_HLE_KEEP_INTRO
  env, startup banner bios_backend=/bios_boot=. PSX_HLE_SCHEDULER spike folded in
  (default via psx_hle_scheduler_set_default, env wins). (5) OBSERVABILITY — always-on
  16K HLE ring (route LLE/HLE/boot-skip) + hle_dump TCP command (../TCP_COMMANDS.md).
  Regen era-consistent: BIOS + Tomba + MMX6 images (emitter changed). NEXT: Tomba
  save+load validation under BOTH backends (user drives); overlay-shard cg-tag refresh
  per title; grow the handler set (UnDeliverEvent, RCnt, A0 libc) with kernel-decompile
  + Beetle checks per handler.
- **2026-07-01 (Tomba pause-menu wedge RESOLVED — stale-recompiler shards, guard shipped):**
  The post-merge Tomba menu wedge (loaded saves only) was NOT the IRQ-resume class the
  prior handoff claimed. Added an always-on exception-EXIT half to irqctx_ring
  (interrupts.c: take_pc/real_epc/exit_pc/exit_reason/same_thread/restored/v1/ra/redirects)
  — it proved every VBLANK resume restores the interrupted GPRs correctly, and the
  0x80016588 "spin" is the normal per-frame vsync wait. Real root cause: autocompiled
  overlay shards were emitted by a recompiler binary (build-t2, 07:39) OLDER than the
  09:28 emitter changes (553d993), yet stamped with the CURRENT cg tag (compile_overlays
  derives the tag from --runtime-include, nothing verified the emitting binary). The
  stale-emitter shards corrupted the display-list task queue (0x801FD800 slots) during
  load-game at Stormy Mountain → menu drew nothing. Repro matrix (compiled/interp ×
  newgame/load), cache-absent A/B, and RAM diff (empty OT 0x8009CA10 vs linked prims
  @0x800Bxxxx) pinned it; recompiler mtime vs emitter commit time was the smoking gun.
  FIXED: rebuilt recompiler, purged + regenerated build-cosim AND build-prod caches —
  menu opens with cache fully enabled. GUARD (class-closing, general): psxrecomp-game
  now bakes the emitter-source hash (shared canonical list
  runtime/codegen_hash_sources.cmake + hash_codegen.cmake) and prints it via
  `--codegen-hash`; compile_overlays.py HARD-FAILS when the binary hash ≠ the tag hash
  (verified positive + negative, both cache and --static modes). Kept the
  same_thread_resume GPR-restore refinement in interrupts.c (ring-verified equal-value
  no-op in practice; faithful). Tooling gaps logged in memory
  (divergence_tooling_gaps_2026_07_01). PENDING: MMX6 cutscene→gameplay gate
  (interrupts.c changed), user validation of prod build (menu + title/save-menu lag —
  lag likely the same stale-shard all-interp fallback + rehash churn).

- **2026-06-27 (device-region MMIO read waits — DONE, branch wt/tomba2-mmio-waits off the
  I-cache tip):** Replaced the placeholder `region = (phys<RAM_SIZE)?3:0` in psx_cyc_readmem
  (memory.c) with the full Beetle MemRW device-region read-wait table (libretro.cpp:859-1131),
  size-aware: main RAM (phys<0x800000) +3; SPU 0x1F801C00-1FFF +36 (32-bit) / +16 (8/16-bit);
  CDC 0x1F801800-180F +6×size; GPU/MDEC/SysControl/FrontIO/SIO/IRQ/DMA/Timers (within
  0x1F801000-113F) +1; BIOS ROM / Expansion-PIO / unmatched +0; scratchpad +0 (early-out).
  Threaded the access size (1/2/4) from psx_cyc_load_word/half/byte + psx_cyc_lwc2_read into
  psx_cyc_readmem (the SPU/CDC waits are width-dependent). The device wait combines with the
  existing +2 completion (+1 LWC2) and fudge exactly as Beetle ReadMemory (LDAbsorb = region +
  completion). RUNTIME-ONLY — psx_cyc_load_* signatures unchanged, so NO emitter regen; just
  rebuild runtime/cyctest. New ruler #2 loops `mmio_timer` (Timer0 read → +3 = 1 dev + 2 compl)
  and `mmio_spu` (32-bit SPU read → +38 = 36 + 2). VALIDATED: cyctest COMPILED (4600) == Beetle
  (4382) EXACT on ALL 15 loops incl. mmio_timer +3 / mmio_spu +38; the 13 prior loops unchanged.
  Tomba 2 boots past the BIOS to its "SCEA Presents" intro splash, total_checks advancing, no
  freeze (the faster-MMIO change did not trigger a device-timing cascade like load=4 did).
  RESIDUAL (documented, unmodeled dynamic axis): DMACycleSteal — Beetle adds the live DMA
  bus-steal count to EVERY read (libretro.cpp:868); non-zero only during active DMA, needs the
  steal count threaded out of the DMA controller, can't be isolated by a static ruler.
  memory.c + gen_testrom.py.

- **2026-06-27 (I-cache fetch — Stage 2 DONE: compiled-path emit + production default-on):**
  Both static emitters now charge the I-cache fetch cost at each cache-line LEADER, BEFORE
  the per-instruction interlock/load (Beetle ReadInstruction order, so a fetch miss clears
  the pending load give-back first). Leader = a block leader / mid-block jump-table target
  (any non-fall-through entry → possibly-cold line) OR a 16-byte-line start (addr&0xC==0);
  intra-line fall-through followers are provably hits (the leader refilled the line to its
  end) so they emit nothing (+0). code_generator (game): `addr` is already the KSEG0 runtime
  PC. full_function_emitter (BIOS): the loop addr is the ROM/compile addr, mapped to the
  RUNTIME guest PC via the existing `relocate_ra` (BIOS main stays in-place KSEG1 0xBFC..
  uncached; kernel Part 2 → 0x500+, shell → 0x80030000+) so the shared TV array evolves
  identically to the interp's cpu->pc and the KSEG1 (>=0xA0000000) uncached test sees the
  true virtual address. The compiled path emits at the SAME address value the interp would
  for the same instruction (they share s_icache_tv), so mixed compiled/interp stays
  consistent. VALIDATED: ruler #2 COMPILED (port 4600, PSX_ICACHE=1) == Beetle (4382) EXACT
  on all 13 loops incl. `icache_miss +14` (was +0 pre-Stage-2); ruler #1 [0x1C5C→0x1CA4]
  steady delta 0 (56==56) AND native now produces the I-cache cold-refill spikes (77/84,
  Beetle's range) that were absent before — exact per-hit magnitude varies run-to-run
  because the two processes free-run (Rule 16), not a bug. Tomba 2 boots past the load wedge
  → intro FMV (jungle) + crisp PS BIOS logo, total_checks advancing, no freeze. Then flipped
  `psx_icache_enabled()` DEFAULT ON (PSX_ICACHE=0 still disables for A/B) — re-validated the
  default-on path (cyctest no-env == +14; Tomba 2 boots clean). psx_icache.c + both emitters.
  NEXT axis: DMA cycle-steal / device-region MMIO load waits (SPU +36 etc.), currently
  unmodeled. Eventually merge wt/tomba2-load-accuracy to master after cross-title regen+smoke.

- **2026-06-27 (I-cache fetch — MODEL built + interp-validated EXACT; Stage 1 of 2):**
  New runtime/src/psx_icache.c: faithful direct-mapped (4 KB / 256-line) instruction-cache
  fetch cost, transcribed from Beetle PS_CPU::ReadInstruction — HIT +0 (no give-back clear),
  KSEG1/uncached +4, cached miss +3 + refill from the missing word to the line end (earlier
  words stay invalid), miss clears the load give-back. Mirrors only the per-word TV tag array.
  Wired into the dirty-RAM interp (exec_one) per instruction, charged BEFORE §1 (Beetle order).
  New ruler #2 loop `icache_miss` (loop top + victim 0x1000 apart alias the same line → refill
  miss every iteration). VALIDATED interp vs Beetle (PSX_FORCE_INTERP=1 PSX_ICACHE=1):
  icache_miss native +14 == Beetle +14; the other 12 loops unchanged at +0 fetch. So the
  hit AND refill-miss costs are MEASURED equal to the oracle on the interp path. Opt-in via
  PSX_ICACHE=1 (default OFF) — charging fetch in only one backend would fork mixed
  compiled/interp timing. Default-off → no production change (Tomba 2 FMV/logo verified
  byte-identical). Commit 958a928.
  STAGE 2 (pending): emit psx_icache_fetch at each cache-line leader in BOTH static emitters
  (code_generator + full_function_emitter), using the RUNTIME address (handle ROM->RAM
  relocated shell code); flip PSX_ICACHE default on so both backends charge it; validate
  ruler #1's cold first-hit spike (Beetle 84/77 vs steady 56) reproduces on the compiled
  path. RISK: the compiled cache state must match Beetle cycle-for-cycle across the whole
  boot to reproduce the cold spike — a careful cache-state-fidelity validation.

- **2026-06-27 (interp-path Δ-ruler — INTERP == Beetle EXACT on all 12 components):**
  Closed the last validation gap: the dirty-RAM INTERPRETER is now MEASURED equal to the
  oracle, not just shared-by-construction. New tooling: `PSX_FORCE_INTERP=1` makes
  `dirty_ram_is_dirty` (memory.c) report all RAM above the kernel window dirty, so the
  dispatcher routes clean compiled game text through the dirty-RAM interpreter (the same path
  overlays take) — no emitter/dispatch change. Launch psx-cyctest with the env set; the test
  ROM runs interpreted (dirty_ram_insns → hundreds of millions). measure.py --port 4600
  (interp) vs 4382 (Beetle): ALL 12 loops match EXACTLY (baseline/alu/load/load2/load_use/
  div/div_spaced/mult/gte_rtps/gte_nclip/gte_read_use/ld_div). Commit b0391bc.
  TWO PROCESS LESSONS (cost real time — now in cyctest README): (1) launch psx-cyctest via
  PowerShell Start-Process — a bash '&' launch fails to boot (pc=0); (2) sample cyc_watch /
  freeze_check AT STEADY STATE — an early query (before the BIOS boots to the EXE entry)
  reports dirty_ram_insns=0 / warm-up values. That premature-sampling artifact was the entire
  "dispatch paradox" I chased (the interp engages only after the EXE entry is reached).
  NEXT axis: I-cache fetch (ruler #1 84/77 cold-refill spikes).

- **2026-06-27 (GTE-read + MFC0 + muldiv give-back — IMPLEMENTED + VALIDATED, ruler #2 100%):**
  Closed the last steady-state divergences. KEY METHODOLOGY FINDING (Rule 15): Beetle's
  cyc_watch must be sampled at STEADY STATE — its boot/warm-up window reports the
  no-give-back value, which is why earlier sessions mis-recorded gte_rtps as "+15 EXACT"
  (true steady = +11). Added ruler #2 probes (load_use, gte_read_use, ld_div) to Δ-gate it.
  Shipped: `psx_gte_read` (MFC2/CFC2: stall to gte_ts_done AND arm ld_absorb=stall/
  ld_which_t=rt give-back; MTC2/CTC2 keep stall-only `psx_gte_stall`); MFC0 arms
  ld_absorb=0/ld_which_t=rt (suppresses a following load's fudge); `psx_muldiv_stall` now
  CONSUMES read_absorb during the MFLO/MFHI stall + the muldiv_ts_done-1 off-by-one — all
  transcribed from Beetle cpu.cpp:1332-1341/1723-1736, in both emitters + the interp.
  **All 12 ruler #2 loops == Beetle at steady state** (gte_rtps 18→14, gte_nclip 11→7,
  gte_read_use 19→14, ld_div 45→49 fixed; the 8 CPU-load/alu/div/mult loops held); ruler #1
  delta 0; Tomba 2 FMV plays (no regression). The R3000A load-delay + GTE/muldiv interlock
  is now hardware-faithful across every micro-benchmark. NEXT axis: I-cache fetch (ruler #1
  84/77 cold-refill spikes). Commits on wt/tomba2-load-accuracy (unpushed).

- **2026-06-27 (load ReadFudge/LDAbsorb — IMPLEMENTED + VALIDATED, both rulers exact):**
  Shipped the shared per-instruction R3000A load-delay interlock. New `runtime/include/
  psx_cyc.h`: §1 base + GPR_DEPRES + DO_LDS (`psx_cyc_step`) as static-inline helpers over
  new CPUState fields `read_absorb[33]/read_absorb_which/read_fudge/ld_which_t/ld_absorb`;
  `psx_cyc_load_word/half/byte` + `psx_cyc_lwc2_read` in memory.c do the Beetle ReadMemory
  timing (clear give-back, +2 fudge iff predecessor committed no load, region RAM +3 +
  completion +2/+1 as the LDAbsorb give-back, scratchpad +0). The pure dep/res classifier
  `psx_cyc_dep_res_mask` (transcribed from Beetle per-opcode GPR_DEP/RES) lives in
  psx_instr_cost.h. Wired into the dirty interp + BOTH static emitters (code_generator game,
  full_function_emitter+strict_translator BIOS); loads now route value reads through the
  UNCHARGED psx_read_* (cpu->read_* rewired in main.cpp; the flat +4 charge_main_ram_read is
  gone). **Δ-validated against Beetle:** ruler #2 `load2` +10 → **+11** == Beetle, every other
  component still exact (alu+1/load+5/div+38/mult+15/gte_rtps+15/gte_nclip+8); ruler #1
  [c5c→ca4] 54 → **56** == Beetle steady-state (84/77 spikes = I-cache cold refill, P2). Tomba2
  boots to the intro FMV (screenshot pixels, no regression). Builds clean (tools/BIOS/game/
  runtime/cyctest). FOLLOW-UP (separate commit): GTE-read/MFC0 give-back + muldiv-stall
  give-back consumption (don't affect rulers; needed for mixed-code faithfulness). Supersedes
  the "MODEL NAILED, impl pending" entry below.

- **2026-06-27 (load ReadFudge/LDAbsorb — MODEL NAILED empirically, impl pending):**
  Derived the last load-path component (the residual on both rulers) by measuring
  Beetle's PER-INSTRUCTION cost via adjacent-PC region cyc_watch. Confirmed:
  fudge = +2 iff the previous instruction committed no pending load (ReadFudge=0x20;
  `(reg>>4)&2` is 0 for all real regs), else 0; region+completion=5 (LDAbsorb excludes
  fudge); the load-delay-slot instruction does NOT absorb (its §1 precedes its DO_LDS),
  the instructions after it do. Per-instruction Beetle data: load = lw7/addiu1/bne0/nop0;
  load2 = lw7/lw6/addiu1/bne0/nop0. Full model + implementation spec in
  `accuracy/load_readfudge_ldabsorb.md`. Implementation is pervasive (per-instruction
  ReadAbsorb + GPR_DEP/RES in both emitters + interp) → a focused next task on a clean
  tree; validate via the ruler-loop anchors (baseline 3 / load 8 / load2 14). No code
  changed this step (read-only empirical derivation).

- **2026-06-27 (interp-path cycle ruler — enabler DONE + validated):** The dirty
  interp now emits `debug_server_cyc_observe(pc)` per instruction (gated), so
  interp-executed PCs are cyc_watch-anchorable (were not). Validated: a live
  Tomba2 overlay interp loop (0x8010724C) records stable cyc_watch hits at 38
  cyc/iter, parity with compiled-PC anchoring; FMV no-regression over 150M+
  interp insns. Commit fc85d8b. This lets interp-side cycle work (the muldiv +
  GTE stalls) be MEASURED, not just by-construction. REMAINING for a fully
  isolated single-component interp ruler: the cyctest harness does not route
  indirect jumps to the dirty interp (a jalr to a scratch dirty address left
  dirty_ram_insns=0 there), so a clean dirty-RAM component loop needs cyctest
  interp-dispatch wiring (or an overlay_cache=off Tomba2 component anchor).

- **2026-06-27 (GTE per-command completion-stall — VALIDATED EXACT):** Modeled
  GTE (COP2) command latency + stall-on-COP2-access. New CPUState.gte_ts_done;
  a GTE command arms it (now + cost-1, serializing back-to-back ops); any COP2
  reg access (MFC2/CFC2/MTC2/CTC2/LWC2/SWC2) stalls to it. Cost table
  (psx_cycles.c) transcribed+verified from beetle gte.cpp op returns (note
  AVSZ4=5 not the psx-spx-doc's 6). Set armed in the shared gte_execute (both
  backends); stall emitted at every COP2 reg-access site in both emitters + the
  interp (offset cancels like muldiv). Added gte_rtps/gte_nclip loops to ruler
  #2: native +15/+8 == Beetle +15/+8 EXACT; all other components unchanged;
  Tomba2 boots to FMV no-regression. Commit ec1fd76. Required regen. UNPUSHED.

- **2026-06-27 (dirty-interp mult/div completion-stall — backend parity):**
  Completed the mult/div stall to the SECOND backend. The dirty-RAM interpreter
  (Tomba2 overlays) charged 0 for mult/div while the compiled emitters already
  set `muldiv_ts_done` + stall MFHI/MFLO — an inter-backend cost inconsistency
  that drifts the shared guest-cycle timeline. `dirty_ram_interp.c` now mirrors
  the compiled emitter exactly via the shared helpers (MULT→`psx_mult_latency_s`,
  MULTU→`_u`, DIV/DIVU→37, MFHI/MFLO→`psx_muldiv_stall`), under
  `PSX_ENABLE_BLOCK_CYCLES`. The interp charges base after exec_one (vs compiled
  "+1 at top") but the set/stall offset cancels (verified algebraically). The
  latency VALUES are already oracle-EXACT on rulers #1/#2 (compiled path); this
  makes the interp apply the identical model. Validated: Tomba2 boots to FMV,
  no regression. Caveat: validation is by-construction + no-regression, NOT a
  direct interp Δ — interp emits no cyc_observe and the testrom isn't an overlay,
  so a true interp-path ruler (interp cyc_observe + force-interp routing) is
  future tooling. Commit 75d5d1a, runtime-only, no regen, UNPUSHED.

- **2026-06-27 (load=4 boot wedge RESOLVED — faithful guest-cycle pad ACK):**
  The oracle-accurate load wait-state (=4) had deterministically wedged Tomba 2
  boot in the BIOS shell (handle s1=104 → 1672-stride table index → wild ptr
  0x8013B608 → RAM corruption → pc=0). Step A (confirm, not hypothesise): the
  proximate runaway was **100% controller (pad) polling** — `sio_irq_dump` showed
  the last 150+ SIO IRQs all `source=pad, delay=4, active_device=PAD, mc_state=0`
  (card idle), NOT the memcard enumeration the handoff guessed. Source-confirmed
  unfaithfulness: the pad fast-path (`sio.c:1386`) armed the access-paced
  `sio_irq_countdown=SIO_IRQ_DELAY_PAD(4)`, decremented once **per SIO register
  access** (sio_tick is only ever called cycles=0), so pad ACK→IRQ7 was
  access-count-paced, not guest-cycle-paced; the faster (accurate) CPU fired it at
  the wrong guest-cycle phase vs the cycle-paced timers/VBLANK → BIOS pad-detect
  state machine diverged. Step B (faithful fix): the pad fast-path now arms the
  **guest-cycle-paced ack scheduler** (`sio_pending_ack`/`sio_ack_remaining =
  BAUD+ACK = 1258 cyc`, driven by `sio_advance`←`psx_advance_cycles`), identical
  to the already-faithful card path. RESULT: load=4 boots **past the wedge to the
  intro FMV** (screenshot-verified, frame 11k+ stable). Ruler #1 native 54 vs
  Beetle 56 = the known load-ReadFudge gap on the load=4 branch, NOT a regression
  (SIO timing can't change CPU instruction cost). Runtime-only (`runtime/src/sio.c`),
  no regen, UNCOMMITTED. Write-up: WEDGE_load4_shell_rootcause.md. Follow-up
  (completeness, non-blocking): axis5 Fix-6 / "1.0e-e2" fully removes the pad
  fast-path so pad+card share one shifter path — needs menu input validation.

- **2026-06-27 (BIOS emitter muldiv stall — ruler #1 now EXACT):** Applied
  per-instruction cycle charging + the mult/div completion-stall to the BIOS
  emitter (full_function_emitter.cpp: +1 at the top of every in-function
  instruction + the 4 inlined orphaned-delay-slot sites; block-up-front charge
  off in per-insn mode) and StrictTranslator (MULTU/DIV/DIVU → psx_muldiv_set,
  MFHI/MFLO → psx_muldiv_stall). RESULT: ruler #1 [0x80001C5C→0x80001CA4] native
  30→56 == Beetle 56, STEADY DELTA 0 — EXACT. Both rulers now match the oracle for
  mult/div (ruler #2 game-side already exact). FMV no regression (i_stat 0x8D,
  full frame). Commit 180b821. The +1-at-top convention cancels the divu-set /
  mflo-stall offset identically to the game emitter (both exact). NEXT: I-cache
  fetch (ruler #1 residual = Beetle's 84-on-cold-hit refill spikes vs native flat
  56); then load wait-state calibration (memory.c +6 → ReadFudge model); then GTE.

- **2026-06-27 (RULER #2 closed + mult/div completion-stall VALIDATED EXACT):**
  Built the full cycle micro-benchmark harness (ruler #2) and used it to land the
  biggest Stage-2 component.
  - **ruler #2 = `tools/cycle_testrom/`**: hand-encoded PS-X EXE of single-component
    isolation loops (baseline/alu/load/load2/div/div_spaced/mult), each measured by
    consecutive-anchor Δ = one iteration; baseline subtraction isolates the cost.
    Both backends boot the SAME synthetic disc (mkpsxiso; license region extracted
    from an OWNED disc via dumpsxiso — LOCAL ONLY, gitignored). Beetle loads it via
    --disc; native via a dedicated psx-cyctest runtime target (boots disc, serial
    CYCT-00101 so disc-identity matches). measure.py compares per-component costs.
  - **Beetle ORACLE costs** (the HW targets): baseline 3, alu +1, load +5, load2
    +11 (2nd load +1 = ReadFudge), div +38 (~36 stall), div_spaced +38 (fillers
    ABSORBED), mult +15 (~13 stall).
  - **MULT/DIV completion-stall IMPLEMENTED + VALIDATED EXACT.** MULT/MULTU/DIV/DIVU
    set CPUState.muldiv_ts_done = now+latency (DIV=37; MULT via MULT_Tab24 14/10/7
    on operand magnitude); MFLO/MFHI stall guest cycles to the deadline
    (psx_muldiv_set/stall in psx_cycles.c). Native previously charged ZERO. Required
    PER-INSTRUCTION cycle charging (PSX_CODEGEN_CYCLE_PER_INSN) — now the DEFAULT on
    this audit branch — so the stall absorbs (the running cycle count must be
    accurate mid-block; block-up-front can't). Game emitter emits set/stall at the
    op sites. RESULT vs oracle: div +38==+38, div_spaced +38==+38 (absorb correct),
    mult +15==+15 — ALL EXACT. Tomba 2 still reaches the FMV (no regression).
  - **Load double-count fix** (earlier today): psx_instr_base_cycles reverted to
    pure execute base (loads=1); memory.c owns the data-access wait-state.
  - Commits 9cec60a, 2b5ad88, 47bcfec, a3e8f28 (+ cyc_watch dedupe). NOT pushed.
  NEXT: (1) calibrate memory.c load wait-state (native +7 vs Beetle +5: flat +6 →
  ~4 + a ReadFudge term). (2) Apply per-instruction mode + muldiv stall to the BIOS
  emitter (full_function_emitter.cpp) + dirty interp → closes ruler #1's div-stall
  gap (still 30 vs 56). (3) GTE per-command cycles (same stall mechanism, gte.cpp
  table). (4) I-cache fetch. Each Δ-gated on the rulers, FMV-verified.

- **2026-06-26 (RULER #1 BUILT + load double-count bug found & fixed):** Built the
  game-independent BIOS-kernel cycle ruler the §3c "TOOLING NEXT" called for, and
  it immediately paid off. Details:
  - **New oracle-model doc `CYCLE_MODEL_BEETLE.md`** — transcribed the full R3000A
    cycle model verbatim from in-tree Beetle cpu.cpp (base +1/insn minus load-delay
    absorb; I-cache fetch +0 hit / +4 KSEG1 / +3+refill miss; ReadMemory loads
    scratchpad=0/region-wait+2, posted stores; mult 6-13 / div 36 stall-on-MFHI/LO;
    GTE per-command table). This is the calibration ground truth.
  - **cyc_watch double-fire FIXED** (debug_server.c): observe was called from BOTH
    the dispatcher (trace_dispatch) AND the function prologue (log_call_entry) at the
    same cycle → every dispatched entry double-recorded. Added (phys,cycle) dedupe.
    Real tooling bug (Rule 15); native deltas were corrupted before this.
  - **Per-block-leader cycle observe ADDED** (full_function_emitter.cpp →
    debug_server_cyc_observe, #ifndef PSX_NO_DEBUG_TOOLS so prod = zero overhead).
    Native previously observed only at FUNCTION ENTRIES; now it samples at EVERY
    compiled block leader, matching Beetle's before-every-instruction sample. This
    lets cyc_watch anchor ANY block-leader PC (interior loop tops, prologue exits) →
    a clean KNOWN-instruction region on both backends. Emitted at normalize_address()
    (runtime phys) so relocated-kernel anchors match.
  - **THE RULER:** BIOS kernel EvCB-search fn at guest 0x80001C5C (relocated ROM,
    identical in every PSX title). Region [0x80001C5C→0x80001CA4] (18-insn prologue,
    contains divu+mflo + 2 RAM loads, no MMIO/GTE). Both backends now record it.
  - **BUG FOUND — loads double-counted.** Native [c5c→ca4] = 34, fully decomposed:
    block c5c advance 20 (=14×1 + **2 loads×3**) + block c9c 2 + **memory.c +6×2 = 12**
    = 34. The Stage-2 #1a commit (2ef47bd) made psx_instr_base_cycles return 3 for
    loads (+2 data-access) WHILE memory.c's charge_main_ram_read already charged +6
    per main-RAM read — the load data-access cost was counted TWICE. The opaque
    0x80017FC4 window hid this because the load over-charge masked the entirely-
    unmodeled divu→mflo stall (~30 cyc Beetle, 0 native). **FIX:** reverted
    psx_instr_base_cycles to pure execute base (loads=1), per the header's own stated
    "data access charged separately in the memory path" contract. memory.c is the
    single address-keyed owner (like Beetle's ReadMemory). Regen BIOS+game, rebuild.
  - **RESULT:** native [c5c→ca4] 34→**30** (exact: 16+2+12), dead stable. Beetle 56
    steady (84 cold = I-cache line refill). FMV still streams (i_stat 0x8D, no
    regression). The remaining −26 gap is now HONEST and decomposed: native is
    missing the divu→mflo execute stall, and memory.c's flat +6/load needs Beetle
    calibration. NEXT: isolate those two components — the BIOS prologue combines
    div+loads in one block (leader anchors can't split them), so the principled next
    step is **ruler #2 (HW test ROM, Amidog)** for hand-crafted single-component
    isolation loops (div-only, load-only), per the user's "do both rulers /
    completeness not convenience" directive. Then Δ-gate each EXECUTE-latency
    component (mult/div, GTE) into psx_instr_base_cycles and CALIBRATE memory.c's
    wait-state per region. All on wt/tomba2-cycle-audit, uncommitted.

- **2026-06-26 (measure: Beetle cycle clock BUILT + VALIDATED):** Added absolute
  guest-cycle exposure to the Beetle oracle (MAIN checkout, additive diagnostic):
  beetle-psx/libretro.cpp accumulates per-frame `timestamp` (CPU->Run slice) into
  `beetle_total_guest_cycles` (+ reset on init) with `extern "C"
  beetle_core_get_guest_cycles()`; runtime/src/beetle_debug_server.c h_ping now
  reports `guest_cycles`. Rebuilt beetle static lib + psx-beetle. VALIDATED (Rule
  0): guest_cycles advances ~565,022 cyc/frame = real PSX rate (33.8688MHz/~59.94).
  Beetle needs the .CUE (not raw .bin). FIRST CROSS-CHECK: native psx_cycle_count
  rate = 565,470 cyc/frame vs Beetle 565,022 (within 0.08%) => gross cycle-rate
  parity confirmed; remaining drift is fine per-instruction-path (needs same-PC
  alignment). Main-checkout Beetle edits are UNCOMMITTED (additive; master has
  other prior uncommitted work — leave for user to manage).
  NEXT (aligned comparator): Beetle has get_registers (PC) but NO run-to/step/pause,
  so same-PC cycle comparison needs a "capture guest_cycles when guest reaches PC X"
  hook on BOTH servers (native has run_to_frame/step; Beetle needs a PC-watch). Then
  diff cycles@PC native vs Beetle to see the residual drift, and Stage-2 cost
  transcription verified against it.
- **2026-06-26 (P3 step 1 DONE — single-source cost seam, identity):** Created
  runtime/include/psx_instr_cost.h `psx_instr_base_cycles(insn)` (identity, 1/insn).
  Routed BOTH backends through it: interp (exec_delay_slot, dirty-dispatch loop,
  precise-slice) + recompiler (code_generator.cpp sums it per block + outside
  delay-slot clone, folding into the compile-time block charge). PROVEN behavior-
  preserving: regen byte-identical (full.c + dispatch.c diff = empty) and Tomba 2
  still streams the FMV (i_stat 0x8D). Commit b00b81f. Stage-2 now edits ONLY this
  one function. ACCURACY_BURNDOWN.md added (all-axes burndown; axis-5 peripherals,
  esp. SIO/controller hybrid-pad bug, flagged weakest), 09a5d45.
  NEXT (measure before Stage-2 costs — don't guess): build the native↔Beetle cycle
  comparator. Feasibility CONFIRMED: beetle_debug_server.c (in worktree) already
  exposes beetle_get_frame_count via the beetle glue — add a parallel
  beetle_get_guest_cycles. Sub-steps: (1) find mednafen's running master-cycle
  timestamp in beetle-psx/mednafen/psx (psx.cpp PSX_Update / the CPU
  pscpu_timestamp_t accumulator — note it's slice-relative, must accumulate to an
  absolute guest-cycle count); (2) add a C accessor through beetle_libretro.cpp +
  a `guest_cycles` debug command; (3) rebuild Beetle static lib + psx-beetle
  (slow: `cd beetle-psx && make platform=mingw_x86_64 STATIC_LINKING=1
  HAVE_LIGHTREC=0 -j8`); (4) comparator: native psx_cycle_count (already in
  freeze_check) vs Beetle guest_cycles at same-PC convergence. THEN transcribe
  Stage-2 costs (mult/div, GTE table, mem wait-states) one at a time, each verified
  by this comparator. Native cycle side already exists; Beetle side is the gap.
- **2026-06-26 (holistic cycle-model audit, post-P2):** Audited ALL cycle-charging
  sites for the dominant class (delay-slot undercount) + cost-model consistency:
  - GAME + OVERLAY emitter (code_generator.cpp `translate_basic_block`): FIXED in
    P2 (block_exec_cycles +1 for outside delay-slot clone). Overlay/alias path
    shares translate_basic_block → covered.
  - BIOS emitter (full_function_emitter.cpp): NO undercount — different model. It
    emits delay slots IN-LINE at their real address (charged by the owning block
    via block_cycles count to next leader) and defers the branch via
    psx_taken_/psx_delay_ flags, rather than emitting an uncounted clone. So the
    delay-slot-is-leader case charges correctly. No change needed.
  - INTERP (exec_one callers): charges psx_advance_cycles(1u) per instruction
    (3 sites). 
  => The cost MODEL is "1 cycle/instruction", duplicated in 3 places (interp hard
  1u; game emitter instruction_count; BIOS emitter leader-to-leader count). They
  agree (Stage-1 backend-equivalent) but are NOT a shared function and NOT HW-
  accurate (Stage-2). recompiler CAN include runtime headers (already includes
  ../../runtime/include/ws_backdrop_detect.h) → a shared psx_instr_base_cycles()
  header is feasible for P3.
  NEXT CORRECTNESS STEPS (deliberate, not tail-of-session):
  1. MEASURE FIRST (don't guess HW costs): native exposes psx_cycle_count already;
     build the Beetle half — add additive guest-cycle exposure to the Beetle oracle
     (main checkout beetle_debug_server.c) + a native-vs-Beetle cycle/first-
     divergence harness (find_divergence.py is STALE/DuckStation-era port 4371 —
     replace with a Beetle 4382 comparator). This is the holistic correctness
     instrument; it makes drift visible for ALL code/titles, FMV being one measure.
  2. P3 shared psx_instr_base_cycles() seam (identity first → byte-identical regen
     proof → then Stage-2 real R3000A costs calibrated against the measure).
  3. P6: regress other titles (breaking is OK per Rule -1; just know), delete
     Tomba2 overlay_native_block.
  COMMIT: f9d50d7 on wt/tomba2 (local, not pushed, not merged to master).
- **2026-06-26 (P2 DONE — Tomba 2 reaches the intro FMV):** Implemented the
  delay-slot cycle-ownership fix in code_generator.cpp (translate_basic_block):
  `block_exec_cycles = instruction_count + (exit branch sits AT end_addr with a
  delay slot outside the block ? 1 : 0)` — charges the always-executed delay-slot
  clone that was previously uncounted (the -8 undercount). Applied to BOTH the
  slice budget and the block cycle charge. Regen + build clean. RESULT: native
  progresses past the frame-1824 logo-delay loop; screen animates; i_stat shows
  CDROM+DMA+SIO active; screenshot = the lush jungle intro FMV. The multi-week
  logo stall is GONE via the faithful fix (no hack, overlay_native_block untouched
  for now). Mechanism was structurally confirmed (instruction_count excludes the
  outside delay-slot clone) before the change; end-to-end confirmed by FMV
  screenshot. NEXT: P3 (shared per-instruction cost fn + MMIO segmentation), P6
  validation (delete overlay_native_block; regress other titles), and Stage-2 HW
  cycle calibration vs Beetle/psx-spx (current model is 1 cycle/insn = backend-
  equivalent, NOT yet hardware-accurate). Apply the same delay-slot fix to the
  BIOS emitter (full_function_emitter.cpp) and overlay/alias paths.
- **2026-06-26 (earlier):** Diagnosis corrected (timing-faithfulness, not take-point).
  Directive persisted (CLAUDE.md Rule -1, memory, MEMORY.md banner). Precise
  slicing root-caused (mid-function clean-text resume not dispatchable) + ChatGPT-
  validated fix (all block leaders = CPS continuations) — PARKED default-off;
  `psx_game_is_function_entry` predicate + slice-trace diagnostics + env toggle
  `PSX_PRECISE_SLICE` left in tree (inert). −8 mechanism located in
  code_generator.cpp (delay-slot-is-leader undercount). Tree builds + boots clean.
  NEXT: P1 (cycle-audit) → P2 (delay-slot ownership fix).

- **2026-07-XX (clean-runtime CD IRQ — precise gap localized):** With the
  systematic pointer-cell->MMIO fixes in `game/cd.c` (commits 33d7427,
  1ede216, e9e9b21 — `br2_ld*/br2_st*` + `*p` variants for every cell that
  resolves into 0x1F801xxx), the CD device itself reaches ready
  (`cdrom_state.stat=0x02`, `sector_available=1`, `irq_flag=0x03`) and the
  frame counter advances. Remaining stall: the game's CD *interrupt
  response* path never runs, so `cd_intc_busy_read` (@0x801bcd1a) stays 0 and
  `cd_cmd_issue` spins until its cycle timeout then retries.
  Instrumented proof (guest-RAM scratch counters, read via read_ram):
    - `cd_intc_dispatch` (@0x8019a8ec, thunk->generated) counter @0x801bce00 = 0
    - `cd_response_pump` (@0x801979f8, clean)        counter @0x801bce04 = 0
    - `0x801bcd1a` busy flag = 0, `0x801bcb24` result = 0
  IRQ delivery itself is healthy: `irqctx_ring` ~70k deliveries,
  `istat=0x5` (VBLANK bit0 + CDROM bit2 both pending), `imask=0xD`, `IEc=1`,
  `IM2=1`, and VBLANK-driven frame progression works — so the kernel->game
  IRQ mechanism is fine; ONLY the CDROM-source callback is missing.
  Chain confirmed: kernel hw-IRQ dispatcher -> kernel object 0xF0000003 ->
  callback @0x8019A87C (`br2eu_func_8019A87C`, generated-only wrapper) ->
  jal 0x8019A8EC (cd_intc_dispatch) -> reads INTC mask -> slot table
  @0x801bcd1c -> chan2 = cd_response_pump. The slot table IS correctly
  populated (chan0=cd_cb8_dispatch, chan2=cd_response_pump, chan3=0x8019af7c)
  via `cd_dispatch_tbl8(2,0x801979f8)`->`cd_chan_slot_set`.
  Neither 0x8019A8EC nor 0x8019A87C is stored anywhere in game RAM (full
  0x80100000-0x80200000 scan = 0 hits) -> they are reached ONLY through the
  kernel object's function pointer in kernel space, never via a game-RAM
  data callback. => The kernel object 0xF0000003 either was never created
  with callback 0x8019A87C, or the kernel-side CDROM dispatch isn't reaching
  it. NEXT: inspect the kernel object table / Intc dispatch in kernel RAM
  (0x1FC0xxxx) to read the registered CDROM callback, and verify the
  B0/0x57 kernel-object creation in cd_kobj_init_a/b actually stored
  0x8019A87C. Also still open: ~50 `guest_at(CDREG32(0x801bdeb0))` sites —
  0x801bdeb0 -> 0x1F801C00 (SPU/reg-window) — still raw guest_at (audio/SPU
  path, separate from the CD IRQ blocker).

## 2025 — kernel IRQ/object model fully decoded; IRQ-class objects absent

Capstone disasm of the game kernel (low RAM, accessed as 0x8000xxxx) resolved
the earlier "0xF0000000 is a pointer" misreading. `0x2610` does NOT deref a0;
it is a kernel-object-list walker:

- object-manager base = fixed `0x100`; list head `mem[0x120]`, tail `mem[0x124]`.
- IRQ dispatcher `0x16E4` tests `I_MASK & I_STAT` bits and calls
  `0x2610(class_id=0xF0000000|src, attr=0x1000)` per pending source
  (CDROM bit2 -> 0xF0000003, bit1 -> 0xF0000002, bit8 -> 0xF000000B, ...).
- `0x2610` walks objects stride `0x1c`, matches `+0x00==class_id`,
  `+0x04==0x2000`, `+0x08==attr`, `+0x0c in {0x1000,0x4000}`, then `jalr +0x10`.
- sibling `0x26e4` = same walk but `+0x04==0x4000` match (find/modify).
- `0x27ac/0x29cc` = exception ctx save/restore; `0xe80` = BIOS syscall/cause
  dispatcher; `0x2970` = ctx-switch return via `mem[0x4bf8]`.

Live object list (head 0x8e08, ~16 slots stride 0x1c) contains ONLY
device/channel objects — NO IRQ-class objects:
  class 0xf4000001 x4 cb 0x801a0b10/24/38/4c
  class 0xf0000011 x4 cb 0x801a0b60/74/88/9c   (CD channel class)
  class 0xf0000009 x1 cb 0
So `0x2610(0xF0000003,0x1000)` for CDROM finds no match -> returns silently.
IRQ class objects (0xF0000001..0xB) were never created.

Class 0xF0000011 objects ARE used: kernel scheduler ~0x3ee0 iterates them via
0x26e4/0x2610 gated by flag 0x1faef4 (CD channel processing). VBLANK works via
IntRP records (0x801aa244 etc fire every IRQ), not necessarily 0x2610 — so
absence of IRQ-class objects may be normal for VBLANK but fatal for CDROM if
the CDROM IRQ-class object is the intended bridge to the CD channel pump.

OPEN: find where IRQ-class objects (class_id 0xF000000N) are created — the
game's interrupt-handler registration / kernel intr-init. They are NOT made
via FUN_8019b380/B0-0x08 (bios_svc_trace shows only 0xf4000001/0xf0000011/
0xf0000009 device types). Determine whether registration ran and failed, or
uses a separate kernel path. Then verify flag 0x1faef4 / scheduler 0x3ee0 is
reached on CD IRQ.

## 2025 — CD stall localized to kernel CD-channel TASK never enabled

Live headless run confirms real stall: cdrom_state.seq creeps but read_msf
frozen at [48,10,56], irq_flag=0x03 pending, i_stat=0x5. bios_svc_trace shows
the game looping B0/0x08 (alloc 8 chan objs 0xF1000000-7) + B0/0x0C (free them)
+ B0/0x35 (poll) — CD channel init retrying forever.

Kernel decode (low RAM, guest 0x8000xxxx):
- Scheduler ~0x3c00-0x3f00 is a kernel task loop. CD-channel processing branch
  at 0x3ee0 iterates class-0xF0000011 objects via 0x26e4/0x2610, gated by flag
  mem[0xAEF4] (reach) then mem[0xAEE4] (beqz->skip). Flag 0xAEE4=0 live.
- Function 0x435c = "enable CD channel task": calls init 0x37c0, sets
  mem[0xAEC8]=1, mem[0xAEE4]=1, bit0 of 0xBF801074, syscalls. Function 0x43dc
  clears 0xAEE4. 0x435c/0x43dc are entries in kernel func table @phys 0x4ac0
  (0x435c at +0x38 = index 14); reached indirectly, no direct jal.
- pc_probe: 0x435c, 0x3ee0, and all chan callbacks 0x801a0b10-9c count=0 even
  after manually setting flag 0xAEE4=1 -> the scheduler branch never runs, so
  flag alone insufficient; full 0x435c (task register/enable) required.
- Kernel svc helpers: 0x1d84 = indexed var-table set/get @0x64fc;
  0x1da0/0x1dc4 = DMA chan regs @0xBF801100+idx*16.

UNIFIED THEORY: CDROM kernel object class 0xf0000003 is referenced by BOTH
IRQ dispatch (0x2610) and BIOS B0/7 kernel-object ops (cd_bios_svc_07), but is
ABSENT from the object list (only 0xf4000001/0xf0000011/0xf0000009 device objs).
The CDROM kobj is never created -> CD driver never enables its channel task
(0x435c) -> IRQ completion path dead -> game retries CD read forever.

NEXT: find the game->kernel call that should create/enable the CDROM kobj
(class 0xf0000003) and CD task (0x435c). Candidates: B0/0x57 (cd_kobj_init_a/b,
not in recent trace), a kernel-call reaching func-table index 14, or CD-driver
init tied to kobj creation. Verify whether cd_kobj_init_a/b run and whether
B0/0x57 succeeds.

## 2026 — B0 kernel-service table fully decoded; CD enable = B0/0x4b

The game's own kernel (low RAM 0x8000xxxx) installs its OWN BIOS-call dispatch,
separate from OpenBIOS. Vector stubs at phys 0xa0/0xb0/0xc0:
  0xa0 -> jr 0x2a80  (A0 svc tbl @phys 0x200)
  0xb0 -> jr 0x2aa0  (B0 svc tbl @phys 0x49cc)
  0xc0 -> jr 0x2ac0  (C0 svc tbl @phys 0x494c)
  (+0x2ae0 -> tbl @0x4b7c). Each dispatcher: t2 = tbl + t1*4; lw t2,(t2); jr t2.
  t1 = service number, a0-a3 args. Kernel code calls these via jalr/jr 0xb0.

B0 service table @0x49cc (live-dumped, index=svc#):
  0x07 -> 0x2610   kobj method invoke (class+attr)   <- cd_bios_svc_07
  0x08 -> bfc05648 kobj alloc                          <- FUN_8019b380
  0x0c -> bfc056b4 kobj free                           <- FUN_8019b390
  0x20 -> 0x26e4   kobj find/modify
  0x4b -> 0x435c   ENABLE CD channel task  (sets 0xAEC8/0xAEE4, BF801074 bit0)
  0x4c -> 0x43dc   DISABLE CD channel task (clears 0xAEE4)
  0x4e/4f/50/5c -> CD channel cmd ops                  <- cd_svc_4e/4f/50/5c
  0x17 -> 0x29cc   ctx restore ;  0x11/0x1a-0x1f -> 0x2afc (stub)
CORRECTION: earlier "func table @0x4ac0 index 14" was really B0-table[0x4b]
(0x4af8-0x49cc=0x12c/4=0x4b). The CD enable is reached ONLY via bios_svc(0xb0,0x4b).

FACT: neither clean game/ nor generated/ contains a bios_svc(0xb0,0x4b) call or
a t1=0x4b trampoline -> the game never explicitly enables CD channels. On real HW
the enable must come from the kernel CD-driver init (auto on kobj/driver create)
or a path not yet traced. pc_probe confirms 0x435c count=0.

UNIFIED ROOT CAUSE (refined): CDROM kernel object class 0xf0000003 is absent
from the object list. It is the linchpin for THREE paths:
  (a) IRQ dispatch 0x16E4 -> 0x2610(0xf0000003) finds no object -> CD IRQ cb dead
  (b) B0/7 kobj ops cd_bios_svc_07(0xf0000003,0x20/0x40) -> 0x2610 finds nothing
  (c) CD channel task enable (0x435c via B0/0x4b or kobj method) never runs
The CDROM kobj is created during kernel CD-driver init. That init either isn't
reached or its create call targets a class/attr the live object list lacks.

NEXT (concrete): find the kernel function that registers the CDROM driver /
creates class-0xF0000003 objects — likely a "register IRQ/driver" helper the game
CD init calls, OR the kernel auto-creates 0xF000000N per IRQ source on
EnableIntc. Trace which game init path should invoke it and whether a BIOS svc
it depends on (B0/0x57 kobj-info, B0/0x15 cmd) returns wrong data in recomp.

## 2026b — IntRP IRQ path decoded; CDROM handler IS registered (revises object theory)

bios_svc_trace (109 entries) decoded fully:
- B0/0x08 allocs (9): 4x class 0xF4000001 + 4x class 0xF0000011 (CD chan objs,
  cb 0x801a0b10-9c) + 1x class 0xF0000009 (attr 0x20, cb=0). NO 0xF0000003 alloc.
- NO B0/0x4b (CD enable), NO B0/0x07, NO B0/0x57 in window.
- C0/0x03 = register IntRP record: a0=irq_src, a1=record_addr.
  src1 -> rec 0x801FEB40 ; src2 -> rec 0x801FEFC0.
- C0/0x02 = unregister/disable same records. C0/0x0A = intc enable (a0=mask).

IntRP record layout (live):
  +0x00 kernel dispatch func (src1=0x461c, src2=0xaecc)
  +0x04 handler A, +0x08 handler B, +0x10.. extra/method slots
src2/CDROM rec 0x801FEFC0: handlers 0x801a189c + op_flag_fn_check(0x801a1834).
src1 rec 0x801FEB40: handlers 0x801aa244/0x801aa2ac + slots holding the CD
  kobj methods 0xbfc07734/0xbfc07744 (the FUN_8019a350/364 indirect targets).

=> The CDROM IRQ handler IS registered via IntRP (src2). op_flag_fn_check is a
generic "check ctx flag -> call registered fn-ptr" deferred-callback. So the
missing 0xF0000003 kernel OBJECT is likely a red herring for IRQ delivery — real
CD completion goes IRQ -> IntRP src2 -> 0xaecc dispatch -> op_flag_fn_check ->
(cd handler / sets intc-busy) -> cd_response_process.

REVISED ROOT CAUSE: the break is downstream — either (a) CDROM IRQ never reaches
the IntRP src2 dispatch (0xaecc), or (b) op_flag_fn_check's flag/fn-ptr is never
armed, or (c) cd_intc_dispatch(0x8019a8ec) never runs to set intc-busy.
cd_cmd_issue uses br2_call_generated so the wait IS interruptible; the IRQ must
land mid-spin. NEXT: pc_probe 0xaecc, 0x801a189c, 0x801a1834(op_flag_fn_check),
0x8019a8ec(cd_intc_dispatch) during a CD read; read intc-busy flag + the
op_flag ctx flag/fn-ptr cells to see which link is dead.

## 2026c — IntRP table + IRQ dispatch chain fully mapped; live probe results

IntRP mechanism (kernel funcs):
- mem[0x100] = IntRP table base; per-IRQ-source slots, stride 8.
- C0/0x03 (0x1edc) = register/find: slot = tbl + src*8; returns record->+0x00.
- C0/0x02 (0x1eb8) = remove: swaps record out, links prev.
- C0/0x06 -> 0x27ac = IRQ exception context save.
- Record +0x00 = kernel dispatch func (src1=0x461c, src2/CDROM=0xaecc),
  +0x04/+0x08 = game handlers (op_flag_fn_check etc).

IRQ dispatcher 0x16e4 (FULL decode): per pending I_MASK&I_STAT bit ->
  0x2610(0xF0000000|per-source-class, attr=0x1000) object-notify, then clears
  I_STAT bit ONLY if per-source "handled" flag mem[0x650c+bit*4] set.
  CDROM bit2 -> class 0xF0000003, handled-flag @0x6514.
  NOTE: 0x16e4 does OBJECT notify only; IntRP record dispatch is elsewhere.

LIVE pc_probe (virtual addrs) during CD read stall:
  RUN: 0x800016e4=13794, 0x80002610=41382  (IRQ+object dispatch active)
  ZERO: 0x8000aecc,0x8000461c (IntRP dispatchers), 0x80003ee0(sched),
        0x8000435c(CD enable), 0x801a189c,0x801a1834,0x8019a8ec(cd_intc),
        0x801a0b60(chan cb)
=> IRQs fire and object-notify runs, but the IntRP record dispatch path that
   should invoke the CDROM handlers is NEVER reached. Also CDROM kobj
   0xF0000003 absent and B0/0x4b enable never called.

NEXT (precise): find the kernel function that reads mem[0x100] IntRP table and
calls record->+0x00 per pending IRQ source — it is the IRQ-handler sibling of
0x16e4 (likely in the exception/IRQ path before or after 0x16e4). Determine why
it doesn't run: gated flag, missing call in the IRQ epilogue, or the game IRQ
handler that drains IntRP is starved because main thread spins in
cd_cmd_issue->br2_call_generated.

## 2026d — kernel is a deferred IRQ->thread-wakeup RTOS; CD needs scheduler to run

Kernel thread/scheduler region 0x1b00-0x1fd4 decoded:
- mem[0x650c..0x6538] = per-IRQ-source flag array (what 0x16e4 "handled" checks).
- 0x1cc8 = "signal source": sw a1 -> mem[0x650c + src*4]  (set pending flag).
- 0x1bd0/0x1c04/0x1c38/0x1c6c = per-source "IRQ->wake thread": if flag set,
  clear I_STAT bit + jal 0x29cc (context RESTORE = resume a blocked thread).
- 0x1ca0 = IRQ cleanup: clear flag array + C0/0x02 remove IntRP records.
- 0x1ce0 = C0/0x00 interrupt setup (registers handler table, masks INTC).
- 0x1b80 = another IRQ-source handler -> 0x2610(0xF2000000,2) (class base F2!).

MODEL: IRQ -> kernel sets source flag (0x1cc8) -> kernel scheduler/idle loop
runs wakeup fns (0x1bd0..) -> 0x29cc restores CD worker thread -> thread does
CD DMA/complete -> sets intc-busy -> game cd_response_process proceeds.

LEADING HYPOTHESIS: the game's kernel RTOS scheduler/idle-loop that drives these
wakeup fns + thread switches is NOT running while the main thread spins in
cd_cmd_issue->br2_call_generated. On real HW the kernel preempts/schedules the
CD worker; in recomp the spin may starve the kernel scheduler => CD thread never
wakes. NEXT: find the kernel main-loop/scheduler entry that calls 0x1bd0-0x1c6c
+ the thread dispatcher, and check via pc_probe whether it runs during the stall.
If it doesn't run, the faithful-core fix is in how the recomp drives guest
kernel preemption/scheduling (timer IRQ -> scheduler), NOT game code.

## 2026e — EXACT causal chain nailed: deferred-IRQ resume never fires

Poll loop in cd_cmd_issue (0x80196e14 generated body) spins ~1.17M iters:
  0x80197028 jal cd_timer_read(0x8019a460) ; timeout check
  0x801970e8 jal cd_intc_busy_read(0x8019a7d4) -> u16 @0x801bcd1a
  beqz -> loop. Only when busy!=0 does it jal cd_response_process(0x80196370).
=> cd_intc_busy_read(@0x801bcd1a) returns 0 forever => infinite poll.

cd_intc_busy_read flag @0x801bcd1a is SET inside cd_intc_dispatch
(0x8019a8ec, the CD IRQ handler, runs via br2_call_generated).

cd_intc_dispatch is invoked from cd_chan_full_init (0x8019a814) ONLY under:
    if (cd_ctx_save(s0+0x38) != 0) cd_intc_dispatch();
cd_ctx_save saves guest CPU context and returns 0 on first entry; it returns
NONZERO only when the kernel RESUMES a context that was deferred by an
interrupt. So cd_intc_dispatch runs iff a CD IRQ was deferred and is now
being resumed through the kernel's context save/restore path.

ROOT CAUSE (precise): the game's custom kernel IRQ-deferral mechanism —
CD IRQ -> kernel saves ctx (cd_ctx_save) -> later resume -> cd_ctx_save!=0 ->
cd_intc_dispatch -> sets busy -> cd_response_process -> CD completes —
never triggers the resume. The deferred CD interrupt is not being delivered/
resumed by the recomp's interrupt+context model. This is faithful-core:
the game kernel expects IRQs deferred through ITS context-switch path
(0x27ac save / 0x29cc restore / cd_ctx_save), which the recomp's IRQ
delivery does not drive for the CDROM source.

NEXT: determine how the game kernel defers the CDROM IRQ — which kernel
function calls cd_ctx_save on IRQ and sets up the resume, and why the recomp
doesn't reach it. Compare IRQ delivery path for CDROM (bit2) vs VBLANK (bit0,
which works via 0x1bd0->0x29cc). The difference between a working source and
CDROM is the faithful-core gap.

## 2026f — deferred-IRQ mechanism UNMASKED; restore path now hard-hangs

FIX (clean source): cd_chan_full_init (0x8019a814) + cd_ctx_save (0x8019ad90)
converted to br2_call_generated thunks -> gen_clean_table skips them -> they
run GENERATED code. Rationale: the deferred-interrupt contract snapshots the
LIVE guest register file (cpu->gpr) into frame @0x801bcd50 and later resumes
via cd_ctx_restore (0x8019adcc, already generated). A clean C body owns a
host stack, not cpu->gpr, so it cannot participate. The clean port had ALSO
dropped the B0/0x19 call that registers the frame into mem[0x4C18].

VERIFIED after fix:
- ctx frame @0x801bcd50 now populated with real regs (ra=0x8019a87c = ret
  into cd_chan_full_init, sp=0x801bdd30, gp=0x80200000, fp=0x801bcd18).
- B0/0x19 ran; mem[0x4C18] was armed then consumed (one-shot).
- cd_intc_dispatch RAN once: busy flag @0x801bcd1a = 1 (was 0 forever).

NEW BLOCKER (faithful-core): CPU dispatch now hard-hangs at pc=0 (0.4% cpu,
all pc_probe=0 incl IRQ dispatch 0x16e4). i_stat=0x04 (CDROM) stays pending,
irq_flag=0x03, sector_available=1 unconsumed. The cd_ctx_restore non-local
context jump (lw ra/gp/sp/fp/s0-s7; jr ra -> 0x8019a87c) abandons the IRQ
handler mid-exception; the recomp's exception/IRQ dispatch does not compose
with this coroutine-style resume -> dispatch dead. This bug was previously
MASKED because B0/0x19 never ran (mem[0x4C18]=0) so the kernel never
attempted the restore.

NEXT: trace the kernel IRQ->cd_ctx_restore call site (who reads mem[0x4C18])
and how the recomp should model the deferred-resume return (eret vs jr).
Compare IRQ delivery for the deferred path vs normal handler return.

## 2026g — CD fatal ROOT-CAUSED + FIXED; CD hw path proven; now game-logic Pause stall

CORRECTION to 2026f: the "hard-hang at pc=0" was NOT a deferred-resume
corruption — host `sample` showed psx_unknown_dispatch -> psx_fatal_halt
-> nanosleep. The guest jalr'd to 0x0801A110 (a LOW addr, no code).

ROOT CAUSE = clean-port transcription bug in cd_chan_block_boot
(game/cd.c): the channel-block callback was written 0x801a110 instead of
the real 0x801A0110 (lui 0x801A + addiu 0x110 @0x8019E6D0-D4). Dropped
hex digit -> invalid address -> FAIL-FAST unknown dispatch. This was
MASKED until the deferred-IRQ fix let the callback path actually run.

FIX (ae9b08c): 0x801a110 -> 0x801a0110. Verified br2eu_func_801A0110
exists in dispatch table.

VERIFIED after fix — CD hardware path works END-TO-END at register level:
- cd_response_pump runs ~4.88M times; register cells correct
  (0x801bcb0c->1F801800 idx, 0x801bcb10->1F801801 data, 0x801bcb18->1F801803 irq).
- IRQ handler reads status 1F801800 (0x39/0x19), reads response 1F801801
  (=0x02), writes controller ACK 1F801803=0x07 clearing irq_flag, and
  pending_arm fires set_irq COMPLETE(0x02). ACK+COMPLETE phases both cycle.
- Command seq now in the MILLIONS (was frozen at 6515 / fatal).

NEW BLOCKER (game-logic, NOT hw): game issues Pause(0x09) ~5M times from
cd_cmd_issue (pc 0x80196FC4) in a retry loop. cd_read_setup(0x80198c90)
issues Pause when cd_status_raw()&0xe0 (busy), then cd_read_open(0)
fails BEFORE issuing ReadN(6) -> caller retries -> re-Pause. cd_status_raw
byte @0x801BC85C = 0x00 (bit4 clear, passes first gate). Likely failure:
cd_mode_flag_set(0xe)/cd_mode_byte_read mismatch inside cd_read_open, or a
higher-level read-state machine stuck. Next: trace cd_read_open's error
return path / which cd_mode_flag_set call returns 0.

## 2026h — Pause stall ROOT-CAUSED to faithful timing race (timer-timeout vs CD-complete)

Refined 2026g: the Pause loop is menu.c:9628 `while(cd_cmd_issue_sync(9,0,0)==0)`.
cd_cmd_issue_sync returns cd_result_wait_a(...)->CDB8(0x801bcb24)==2.

PROVEN mechanism (register+event-ring verified):
- CD IRQ DOES fire+deliver: event_ring shows 12777 ISTAT_RAISE detail=2
  (CDROM bit2) + IRQ_DELIVER at pc=0x80197060 (the wait loop, func=0x29CC
  deferred path), istat=0x04 at delivery. i_stat bit2 is acked too fast for
  200ms RAM samples to catch — earlier "i_stat=0" was a sampling artifact.
- cd_response_process RUNS: response buffer 0x801fc6c8=0x02 populated.
- BUT result byte 0x801bcb24 stays 0 -> sync never sees 2.
- ACK path (case3) gives result=3 not 2 because I32AT(0x801bc90c,drive=9)
  @0x801bc930 = 1 (nonzero) -> case3 else-branch -> CDB8(0x801bcb24)=3.
- Only COMPLETE (case2, local_20=irq_flag&7==2) sets result=2.

DEADLOCK: irq_flag stuck at 0x03 (ACK) — COMPLETE(2) never surfaces.
cdrom.c exec_command cancels pending on each new command (line ~2091).
Pause COMPLETE = pause_complete_delay_cycles(): 5000 cycles when not
reading/playing, ~1.1M+ when reading. pending.delay samples 872-4532
(=~5000 not-reading path) but JUMP, never reach 0 -> re-armed each retry.

ROOT CAUSE = faithful TIMING RACE: cd_result_wait_a (game/cd.c:562-571)
timeout = cd_timer_read()+0x3c0 (960 timer ticks) OR >0x3c0000 polls.
It returns 0xffffffff -> sync false -> menu retries Pause -> exec_command
cancels pending COMPLETE -> irq_flag stays ACK(3) -> repeat forever.
For real HW this race must NOT happen: the game's 960-tick budget has to
exceed Pause completion. Suspects (need Beetle oracle to disambiguate):
  (a) cd_timer_read reads a timer whose clock-source/divider makes 960
      ticks elapse in <5000 guest cycles (timer too fast), OR
  (b) pause_complete_delay_cycles over-estimates (should be <<5000 or
      the not-reading path should be near-instant), OR
  (c) reading/stat_reg wrongly reports "reading" so delay is 1.1M not 5000.
NEXT: identify which timer cd_timer_read uses (DB32G(0x801bccac) pointer),
its configured clock source, and compare 960-tick span + Pause latency vs
Beetle PS_CDC. Fix the faithful side (timer rate or CD latency), not the
game timeout.

## 2026i — mc_state_init transcription bug FIXED; cd_init_session is next stall

CORRECTION to 2026h: the "timing race" hypothesis was WRONG. Guest-RAM
instrumentation (CDDBG counters @0x801bce10+) proved the sync path WORKS:
cd_cmd_issue(9) returns 0 always, cd_result_wait_a returns result=2
(0x801bce2c=2), cd_cmd_issue_sync returns TRUE. The Pause "spam" was NOT
the menu.c:9628 loop.

REAL BUG #1 (FIXED, commit d9f4350): game/save.c mc_state_init @0x801bef60
`while(cd_cmd_issue_sync(9)!=0)` was INVERTED. Generated MIPS
br2eu_func_801573EC block_80157418 = `beq $v0,$zero -> loop` i.e.
`while(sync==0)` (retry Pause until success). Clean had `!=0` -> looped
while Pause SUCCEEDED -> infinite ~7M-command Pause spin at memcard init.
Fix: `!=` -> `==`. Verified vs generated SLES_017.22_full_20.c.

After fix, game progressed past memcard init into cd_file_pump ->
cd_init_session @0x80198af0. NEW STALL: cd_init_session waits for
DAT_801bcba4==0 (async Pause callback 0x80198590) in a clean-C spin.
Two compounding issues:
  (a) clean C wait-loop is NOT interruptible — generated version has
      psx_check_interrupts_at at every block. Adding br2_irq_poll() DOES
      deliver the CD IRQ but crashes in cd_intc_dispatch @0x8019AA98
      (write_half via gpr[1]=at + -13030, wild ptr) — IRQ-handler register
      context not set up for clean-C callers. Reverted.
  (b) cd_timer_read @0x8019A460 is broken: pointer cells
      0x801bcca8/0x801bccac hold 0x14/0x10 (NOT 0x1F8011xx timer-reg
      ptrs) -> br2_ld32(0x14/0x10)=0 -> returns constant -> the 0x79-tick
      timeout in cd_init_session (and ALL CD timeouts) never fire.
      Init that writes these cells NOT found in clean or generated code —
      likely kernel/BIOS-side timer-pointer setup still missing.
NEXT: (1) find/fix timer-pointer-cell init so cd_timer_read works ->
0x79 timeouts fire -> cd_init_session exits via timeout; (2) OR make
cd_init_session's wait interruptible safely (br2_call_generated like
cd_cmd_issue, or fix cd_intc_dispatch IRQ context for br2_irq_poll).

## 2026j — timer-pointer cells are STATICALLY CORRECT in EXE image

Read disc/SLES_017.22 (PS-X EXE, taddr 0x8011e800, text 0xa0800, hdr 0x800):
  0x801bcca8 = 0x1f801814   (static .data pointer)
  0x801bccac = 0x1f801110   (Timer1 counter, base 0x1F801100+1*0x10)
  0x801bcb20 = 0x1f801c00   (CD reg base)
  0x801bcb0c = 0x1f801800   (CD cmd reg)
Both cells lie inside the loaded image [0x8011e800,0x801BF000) -> .data.
So the pointer VALUES are correct in the disc. The earlier runtime
reading of 0x10/0x14 was either a misread/stale OR a real .data-section
load bug (image loads text but not all of .data). UNVERIFIED — debug
server starved by the cd_init_session spin before cells could be read
in early boot. NEXT: read 0x801bcca8/ac early-boot (before stall) or via
freeze-dump to determine if .data loads correctly. If cells ARE correct,
cd_timer_read works and cd_init_session's ONLY blocker is the
non-interruptible clean-C spin (br2_irq_poll crashes in cd_intc_dispatch
IRQ ctx; alternative = br2_call_generated for the wait, per cd_cmd_issue
precedent). If cells are WRONG -> .data load bug (bigger, affects all
hardware-pointer globals).

## 2026k — cd_init_session stall = deferred-IRQ RE-ENTRANCY in cd_intc_dispatch

After mc_state_init fix (d9f4350), game reaches cd_file_pump -> cd_init_session
(0x80198af0). Its wait loops spin on DAT_801bcba4 (cleared by session cb
0x80198590 via CD IRQ -> cd_ctx_save/restore deferred path). Clean C spin is
not interruptible; two unblock attempts both crash the SAME way:

  br2_irq_poll in the clean wait  -> SIGSEGV @ cd_intc_dispatch store 0x8019AA98
  cd_init_session -> br2_call_generated -> FATAL dispatch_depth=257

Decoded 0x8019AA98 = `sh $zero,-13030($at)` (rs=$at=gpr1, CORRECT codegen —
earlier gpr1-vs-gpr2 suspicion was a misdecode; at=0x801C0000 -> 0x801BCD0A
valid BSS flag clear). last_store_pc is diagnostic only. REAL halt = IRQ
re-entrancy: cd_intc_dispatch is re-entered ~257x because the CD IRQ source
stays pending/unmasked while the handler runs its own psx_check_interrupts_at.
On HW the exception masks further IRQs until intc re-enable (svc 0x17 tail).

ROOT: deferred CD-interrupt path does not suppress nested CD IRQ delivery.
Needs IRQ-mask/acknowledge fix in the faithful core (cdrom I_STAT clear or
in-exception nesting guard covering the deferred ctx_save/restore path), NOT
a clean-source change. cd_init_session reverted to clean spin (stable, no
crash). Timer-pointer cells confirmed statically correct in EXE image.

## 2026l — deferred-IRQ re-entrancy: refined hypothesis (INTC mask bypass)

cd_intc_dispatch recursion (dispatch_depth=257, dirty_block tail = BIOS B0
kernel-vector calls a0=0x65/0x20/0x62) is CD-IRQ-specific. INTC model:
i_stat (pending, write-1-clear via 0x1F801F00.stat) & i_mask (enable, 0x1F801F04).
Normal IRQ entry masks the source; the deferred path (cd_chan_full_init ->
cd_ctx_save @0x801bcd50 -> kernel -> cd_ctx_restore @0x8019adcc -> re-enter
0x8019A838/0x8019A874 -> cd_intc_dispatch) may resume cd_intc_dispatch OUTSIDE
the kernel handler's masked window, leaving i_mask bit2 set + i_stat bit2
pending -> next psx_check_interrupts_at nests another CD IRQ -> unbounded.

NEXT (empirical): trigger the path (cd_init_session->generated) under
PSX_RECURSION_LIMIT and read irqctx_ring + imask_trace + g_cdrom_deliver_count
to confirm i_mask bit2 / i_stat bit2 state inside cd_intc_dispatch. If mask
bypass confirmed, fix = ensure deferred resume runs under the handler's INTC
mask (or ack i_stat bit2 before re-present). Clean coverage is otherwise strong:
1162 clean wrappers; fnmap-vs-clean_table delta is only intentional
generated-thunks (IRQ-context) + BIOS boundary.

## 2026m — recursion cycle NAMED: OpenBIOS kernel event/thread dispatch

Post-mortem rings DISPROVED the IRQ-reentrancy hypothesis: irqctx_ring shows
NORMAL once-per-frame CDROM(bit2)/DMA(bit3) deliveries (imask=0xC steady,
exit_reason=1, restored=1) — NOT nested IRQs. imask_trace shows the game DOES
mask i_mask bit2 (CDROM) around cd_intc_dispatch (0x8019A8BC). cdrom_state:
irq_flag=0, i_stat=0 (acked). cd_init_done_cb (0x80198590) is trivial.

REAL recursion = guest C-call cycle in dispatch_tail (256 nested):
  0x1FC06084 -> 0x1FC086C4 -> 0x1FC060A8 -> 0x1FC00C20   (~x50)
  0x1FC00CDC -> 0xB0 -> 0x2AA0 -> 0x1FC05648 -> 0x1FC05600 -> 0x1FC0565C
All 0x1FC0xxxx = recompiled OpenBIOS KERNEL funcs (OpenBIOS_func_*), 0xB0 =
kernel vector, 0x2AA0 = kernel svc. The deferred CD IRQ (cd_init_session
async-Pause -> cd_chan_full_init ctx_save/restore -> cd_intc_dispatch)
drives the OpenBIOS kernel event/thread dispatch into an unbounded cycle.
last_store_pc 0x8019AA98 is diagnostic only (valid BSS flag clear, codegen OK).

ROOT: OpenBIOS kernel recompilation recurses on this deferred interrupt —
likely event-loop/thread-wakeup (ChangeThread/DeliverEvent) re-trigger, NOT
game clean source, NOT CD hw. Needs OpenBIOS kernel RE or Beetle oracle to
find which kernel func fails to mark the event/thread consumed. cd_init_session
reverted to clean spin (stable, no crash).

## 2026n — recursion = OpenBIOS kernel IRQ dispatch cycle (NAMED functions)

dispatch_tail addrs are kernel-ramtext funcs; VMA 0x500 -> LMA 0xBFC1E4D4 maps
them to ELF symbols (openbios_elf_seeds.json). Recursion cycle:

  exceptionHandler(0x27AC) -> getCop0CauseAndEPC(0x279C)
  -> IRQVerifier(0x16E4/0x1F6C) + T0-T3verifier(0x1B7C/1B24/1ACC/1A74)
     + sio0Verifier(0x371C) + syscallVerifier
  -> deliverEvent(0x2610) -> B0Handler(0x2AA0)/C0Handler(0x2AC0)
  -> returnFromException(0x29CC) -> exceptionHandler ... (256 deep)

Timer IRQ verifiers (T0-T3) IN the cycle: cd_init_session's CD-event handler
path touches timer regs (cd_timer_read) -> likely re-fires a timer IRQ that
nests back through exceptionHandler->deliverEvent->syscall. The deferred CD
IRQ (cd_ctx_save/restore) is the trigger; the kernel dispatch lacks a bound
on this timer/IRQ re-entry. Faithful-core OpenBIOS kernel bug — NOT clean
game source, NOT CD hw, NOT INTC mask (imask verified correct).

## 2026o — mechanism: deferred resume bypasses exception_nest_depth unwind

Timer IRQs ruled out (irq_line=0 all ch, i_stat=0). T0-T3 verifiers are just
the IRQVerifier dispatch table walked per-IRQ. Real cycle:
exceptionHandler->deliverEvent->B0/C0Handler->returnFromException->
exceptionHandler. For dispatch_depth to reach 257 the host frames must NEST,
but exception_nest_depth caps guest nesting at 2 — UNLESS the deferred CD
resume (cd_ctx_restore does jr $ra into cd_chan_full_init->cd_intc_dispatch,
NOT a normal exception return) skips the epilogue that decrements
exception_nest_depth. Result: nest_depth stays low, host dispatch_depth grows
unbounded -> trips at 257. Likely fix area: the deferred-interrupt resume must
run inside the exception's nest accounting (or the IRQ that drives it must be
delivered through the normal in_exception path so unwind decrements depth).

## 2026o — deferred-interrupt contract fully traced; bug is OpenBIOS-kernel-side

Game side verified correct (all generated, faithful):
- cd_chan_full_init (0x8019A814): cd_ctx_save(0x801bcd50) snapshots live gpr,
  registers frame via B0/0x19 -> mem[0x4C18]. Resumable: v0==0 arms, v0!=0 runs
  cd_intc_dispatch. Already br2_call_generated (owns cpu->gpr).
- cd_ctx_save (0x8019AD90): snapshots gpr to frame.
- cd_ctx_restore (0x8019ADCC): 60-byte reg-restore stub only (ra/fp/sp/s0-s7/
  a0-a3 from frame+a0). Does NOT touch mem[0x4C18].
- cd_intc_dispatch (0x8019A8EC): handles CD IRQ, acks via response/0x1F801803.

=> The deferred-frame CONSUMPTION (clearing mem[0x4C18] + marking the kernel
event delivered) is 100% OpenBIOS-kernel-side (exceptionHandler->deliverEvent
->deferred-frame dispatch). The recursion cycle (exceptionHandler->verifiers->
deliverEvent->B0/C0Handler->returnFromException) is the kernel re-delivering
the same unconsumed deferred event. Game code is exonerated; fix requires
OpenBIOS kernel RE of the deferred-interrupt/event-consume path (ramtext
0x2610 deliverEvent, 0x27AC exceptionHandler, 0x2AA0 B0Handler region).

## 2026o — recursion is a guest CALL-CYCLE, not IRQ storm; deferred-frame offset

Post-mortem DURING recursion (PSX_RECURSION_LIMIT=4000): i_stat=0, i_mask=0xD,
pending=0, cdrom irq_flag=0, pending.cmd=0x09 active=0 (Pause stuck). NO IRQ
is pending -> the recursion is NOT interrupt-driven. It is a guest-level call
cycle that the recompiler turns into nested host frames (dispatch_depth->4000).

Mechanism (verified in generated code):
- cd_chan_full_init (0x8019A814) reads in-service flag lh v0,0(0x801BCD18):
  !=0 -> dispatch cd_intc_dispatch; ==0 -> arm via cd_ctx_save+B0/0x19.
- cd_intc_dispatch clears the BUSY flag (sh zero -> 0x801BCD1A) at 0x8019AA98
  but the IN-SERVICE flag 0x801BCD18 consumption + deferred-frame slot
  (kernel struct +0x4C18 offset, NOT absolute) clearing is OpenBIOS-kernel-side.
- deliverEvent (OpenBIOS_func_00002610) iterates EvCB table (mem[0x120] base,
  mem[0x124] size, stride, s2=0x2000) delivering enabled events; the deferred
  CD event is not marked consumed -> kernel re-dispatches it -> guest call
  cycle exceptionHandler->deliverEvent->handler->returnFromException->...

FIX PATH: RE the OpenBIOS kernel deferred-frame/event-consume (the code that
reads the B0/0x19-registered frame and clears it after cd_ctx_restore resumes)
OR enable+validate the bios_hle kernel-call tier (needs deliver_event_ret
anchor for OPENBIOS, currently absent per OpenBIOS.toml:88).

## 2026o — located deferred-IRQ dispatcher: OpenBIOS_func_00003870

Kernel ram 0x3870 (ROM BFC21844+): the deferred-interrupt dispatch loop.
- mem[0x4C04] = registered deferred-handler COUNT (loop bound, a0).
- mem[0x4C48] = rotating slot counter (incremented each iter, s0+0x4C48).
- handler array at ABSOLUTE 0x4C18: lw v0,[0x4C18 + idx*4]; jalr ra,v0.
- sh v1,4170(a1) sets flags 0x12 (in-deferred-IRQ marker) on a1 ctx.
- idx derived from counter -> sll v0,v0,2 -> +0x4C18 -> lw -> jalr.

This is what calls the game's B0/0x19-registered cd_ctx_restore. The recursion
= this dispatcher re-invoking the CD deferred handler because the slot/frame
is not consumed/cleared after cd_intc_dispatch returns. Next: find where the
kernel clears the 0x4C18 slot + 0x4C04 count after a deferred handler
completes (likely gated on the in-service flag 0x801BCD18 or a return code),
and why it isn't reached.

## 2026o — deferred-handler return-value contract (consume gate)

Dispatcher OpenBIOS_func_00003870 post-jalr: v0==0 -> 0xBFC21944, v0==1 ->
0xBFC21980, else post-process (clear 0x4C5C, sh 0->0x1F80104A, check 0x4C50/
0x4C58). So the deferred handler's RETURN VALUE decides slot consumption.
cd_intc_dispatch tail: clears busy flag (0x801BCD1A) then jal 0x8019AD60 =
BIOS svc 0x17 (return-from-IRQ / re-enable). If the resumed
cd_chan_full_init->cd_intc_dispatch->svc0x17 path returns a v0 the kernel
reads as "not done" (nonzero/non-1), the 0x4C18 slot + 0x4C04 count stay
armed -> dispatcher re-invokes -> guest call-cycle -> dispatch_depth guard.
NEXT: determine exact v0 contract (what cd_ctx_restore/cd_chan_full_init must
return for the kernel to clear the slot) and whether svc0x17/re-enable path
drops/alters v0 in recomp.

## 2026o — full recursion chain traced to B0:0x17 kernel handler

cd_intc_dispatch tail = jal 0x8019AD60 = B0 syscall 0x17 (li t2,0xB0;
li t1,0x17; jr t2). B0Handler (OpenBIOS_func_00002AA0, BFC20A74):
t2 = mem[0x49CC + t1*4]; jr t2. So B0:0x17 handler = mem[0x4A28].

FULL CYCLE (guest call-cycle, i_stat=0 — not IRQ storm):
  deferred-dispatcher 0x3870 jalr -> cd_ctx_restore -> cd_chan_full_init
  (resume, in-service flag 0x801BCD18!=0) -> cd_intc_dispatch
  -> clears busy flag 0x801BCD1A -> jal svc B0:0x17 -> B0Handler
  -> kernel B0:0x17 handler (mem[0x4A28]) -> [delivers pending events /
     re-enables] -> deliverEvent -> deferred-dispatcher 0x3870 -> ...

CONSUME CONTRACT: deferred-dispatcher reads handler v0: ==0 -> BFC21944
(ack: write 0xFFFFFF7F->0xBF801070, set bit7 0xBF801074, return);
==1 -> BFC21980; else post-process. The CD frame stays armed because
B0:0x17's kernel handler re-delivers it BEFORE the slot is consumed, OR
cd_chan_full_init's resume returns nonzero v0.

NEXT: disassemble kernel B0:0x17 handler (mem[0x4A28] -> find its ROM addr
via syscall-table init) to see if it calls deliverEvent/deferred-dispatch
while the CD frame is still armed. Likely fix: ordering — the deferred
slot must be consumed before B0:0x17 re-enables event delivery.

## 2026o — bioscall histogram: B0:0x0B TestEvent storm = cd_seek_state_machine retry

bioscall_dump post-halt: B0 idx11(0x0B TestEvent)=1,361,041 (~99.9% of all
bioscalls); idx23(0x17)=769, idx61(0x3D)=753, idx32(0x20)=195 (per-recursion-
iteration); DeliverEvent/OpenEvent/EnableEvent small counts.

0x8019d5b0 = TestEvent trampoline (li t1,0x0B; jr 0xB0), renamed
cd_seek_state_machine. Callers storm it via retry loops:
- cd_seek_step (0x8019d508): do{v0=cd_seek_state_machine(DB32G(0x801bde38));}
  while(v0==0)
- cd_queue_a_status/cd_queue_b_status/cd_pending_args_pump: for(;;) poll
  queue cells then drain via cd_seek_state_machine.
- bind.c:2019 has __attribute__((weak)) cd_seek_state_machine(){return 0}
  FALLBACK — clean C callers resolve to it (return 0 -> while(v0==0) spins),
  while guest/dispatch callers of 0x8019d5b0 get real TestEvent.

=> CD seek/state event never reports READY -> TestEvent retry storm AND the
deferred-frame recursion are TWO SYMPTOMS of one root cause: the CD
IRQ/event is not being delivered+consumed. The weak stub returning 0 is a
separate latent bug for clean callers (must route to generated TestEvent,
not return 0).

## 2026p — OpenBIOS CD-ROM driver symbols + cd_seek_state_machine fixed

openbios_elf_seeds.json names the kernel CD/event machinery:
- cdromIOHandler 0xBFC1E9E8 / cdromDMAHandler 0xBFC1EA24 — OpenBIOS's own
  CD IRQ handlers. cdromIOHandler: `lw v0,24280(v0)` gates on kernel cell
  0x5ED8 (enqueued-handler count); nonzero -> dispatch, then `li t1,0x17`
  + `sw -5 -> 0xBF801070` = B0:0x17 IRQ-handler return/ack.
- enqueue/dequeueCDRomHandlers, resetAllCDRomIRQs, enableAllCDRomIRQs,
  cdromUndeliverAll(ExceptAckAndRdy), initializeCDRomHandlersAndEvents.
- testEvent 0xBFC0574C, waitEvent 0xBFC0578C, deliverEvent 0xBFC205E4,
  undeliverEvent 0xBFC206B8, exceptionHandler 0xBFC20780,
  syscallVerifier 0xBFC1FF40, enqueueIrqHandler 0xBFC1FC74,
  enqueueSyscallHandler 0xBFC20050.

CONFIRMED: B0:0x17 = kernel "IRQ handler return/ack" — used BOTH by
cdromIOHandler AND by game's cd_intc_dispatch tail (jal 0x8019AD60).

FIX LANDED (42bb43c): cd_seek_state_machine weak stub (bind.c:2019 returned
0) replaced by strong def in cd.c -> bios_svc(0xb0,0x0b,a0,0). Clean callers
now get real TestEvent. NOTE: does NOT unblock boot — generated path already
calls real TestEvent 1.36M times and event still never reports ready; the
weak stub was a latent correctness bug, not the primary blocker.

REMAINING ROOT CAUSE (unchanged): deferred-IRQ frame (B0:0x19, slot array
0x4C18, count 0x4C04, rotate idx 0x4C48) is not consumed. Dispatcher
OpenBIOS_func_00003870 jalr's cd_ctx_restore -> cd_chan_full_init(resume)
-> cd_intc_dispatch -> B0:0x17. Consume gated on handler v0==0 -> ack
0xBF801070/0xBF801074. If B0:0x17 re-enters deliverEvent/deferred-dispatch
while frame armed, slot never clears -> guest call-cycle -> recursion
guard. NEXT: read dequeueCDRomHandlers 0xBFC03DA0 + the deferred-dispatcher
post-handler path to find where slot 0x4C18[idx]/count 0x4C04 should clear.

## 2026q — CORRECTION: deferred frame = setExceptionJmpBuf longjmp, not func_00003870

Subagent RE corrected the mechanism (func_00003870 is the SIO0/memcard
action queue — mcHandler region — NOT the CD dispatcher; earlier notes
mis-identified it).

REAL game deferred-IRQ contract:
- Register: game B0:0x19 = setExceptionJmpBuf. cd_ctx_save snapshots
  {ra,sp,fp,s0-s7,gp} into frame @0x801BCD50; kernel stores ptr at
  mem[0x4BF8] = g_exceptionJmpBufPtr.
- Consume: exceptionHandler @0xBFC20780 saves ctx to thread->Registers,
  walks 4 priority handlersArray lists (mem[0x100]; HandlerInfo{next,
  handler,verifier}). If NO node claims the IRQ, tail @0xBFC20940-98 does
  longjmp: lw a0,[0x4BF8] -> restore jmpbuf -> jr ra via
  psx_restore_state_escape() = HOST-side unwind (not nested call).
- B0:0x17 = returnFromException @0xBFC209A0: restores all GPRs+hi/lo+mtc0
  Status from thread->Registers, erets. Returns NO consume value.
- THERE IS NO SLOT/COUNT to clear — frame is a single ptr consumed by the
  longjmp itself.

WHY THE LOOP: every CD IRQ -> no kernel HandlerInfo claims it -> longjmp
to game jmpbuf -> cd_ctx_restore -> cd_chan_full_init(resume) ->
cd_intc_dispatch -> B0:0x17(returnFromException) -> resume interrupted
ctx -> IRQ still pending -> next check -> longjmp again. The kernel CD
drivers cdromIOHandler/cdromDMAHandler (enqueued by
initializeCDRomHandlersAndEvents @0xBFC02E70 via enqueueCDRomHandlers
@0xBFC03D28, removed by dequeueCDRomHandlers @0xBFC03DA0) are MISSING
from handlersArray OR their verifiers reject IRQ bit2 -> every CD IRQ
falls through to game jmpbuf.

ALSO: the 1.36M TestEvent storm = B0:0x0B SYSCALLS; each is a syscall
exception -> exceptionHandler -> syscallVerifier -> B0Handler -> testEvent
-> returnFromException. If psx_restore_state_escape leaks host frames,
dispatch_depth grows -> recursion guard. And the mark-mode EvCB the game
polls only reaches READY(0x4000) via kernel cdromIOHandler's deliverEvent
— absent -> cd_init_session spins ~98% forever.

DECISIVE NEXT PROBE: dump mem[0x100] handlersArray heads + each
HandlerInfo{handler,verifier} + verifier(IRQbit2) result + I_STAT/I_MASK
(0xBF801070/74) + mem[0x4BF8] during the stall. If CD HandlerInfos absent
-> bug in enqueueCDRomHandlers/initializeCDRomHandlersAndEvents ordering
or errant dequeueCDRomHandlers/cdromUndeliverAll. If present but verifier
returns 0 -> bug in cdromIOVerifier/cdromDMAVerifier IRQ-bit test
(I_STAT ack/edge-detect mismatch in recompiled kernel).

## 2026r — deliverEvent decoded (CORRECT func; earlier read was openEvent)

deliverEvent = 0xBFC205E4 (ram 0x2610), OpenBIOS_full.c ~line 220530.
NOTE: func_1FC05648 @0xBFC05648 is openEvent (allocates slot, returns
handle 0xF1000000|idx) — do not confuse.

deliverEvent(a0=class,a1=spec): iterates EvCB base(mem[0x120])..+size(mem[0x124])
stride 0x1C; s2=0x2000:
  status!=0x2000 skip ; class(+0)!=a0 skip ; spec(+8)!=a1 skip
  mode(+0xC)==0x2000 MARK -> status(+4)=0x4000 READY  (testEvent sees it)
  mode==0x1000 CALLBACK -> jalr handler(+0x10)(class,spec); NO status change
    (event stays ENABLED -> re-fires on next deliverEvent)

=> mark-mode events reach 0x4000 correctly. Recursion = CD IRQ never ACKed:
exceptionHandler->deliverEvent->cb handler(B0 syscall)->B0Handler->
deliverEvent->returnFromException->IRQ re-fires->inf. i_stat=0 at halt is
between-fires sampling. FIX = find why CD IRQ (bit2) isn't acked: either
cdromIOHandler/cdromDMAHandler absent from handlersArray(mem[0x100]) or
game jmpbuf path (cd_intc_dispatch) doesn't write CDROM ack. Subagent
3f974b24 comparing vs upstream openbios source.

## 2026s — bios_svc_trace nails the event mismatch + recursion is host-dispatch

bios_svc_trace (port 4376 post-halt) shows the FULL event sequence:
  B0:0x08 OpenEvent x9: 8 CALLBACK (class 0xF4000001/0xF0000011, mode 0x1000,
    handlers 0x801A0B10-0x801A0B9C flag-setters) -> handles 0xF1000000-7
    PLUS 1 MARK: a0=0xF0000009 a1=0x20 a2=0x2000 -> handle 0xF1000008 (EvCB[8])
  B0:0x0C EnableEvent x9 -> all enabled
  B0:0x0B TestEvent a0=0 -> v0=0 (storm, event never READY)
  B0:0x07 DeliverEvent a0=0xF0000003 a1=0x20 -> v0=0  (ra=0x8019A9C8, x3)

=> game's CD handler delivers class 0xF0000003 spec 0x20, but its mark event is
   class 0xF0000009 spec 0x20 -> NO EvCB match -> mark event never 0x4000.
   Kernel IRQVerifier delivers (0xF0000009, spec=0x1000) on CDROM IRQ: class
   right, SPEC wrong (0x1000 != 0x20). spec=0x20 is never delivered by kernel.

irqctx_ring: CDROM IRQ (istat bit2=0x4, imask 0xD) repeatedly delivered to GAME
  PCs 0x80197048/0x801970DC (game jmpbuf path, mem[0x4BF8]=0x801BCD50), each
  restored=1 but re-fires. mem[0x6514]=0 -> CD autoack OFF (cdromIOHandler/
  cdromDMAHandler must ack; both write I_STAT+call B0:0x17, no deliverEvent).

Kernel VERIFIED upstream-faithful (subagent 3f974b24): deliverEvent/exception
  Handler/B0Handler/IRQVerifier/syscallVerifier all match pcsx-redux OpenBIOS.
  Callback events intentionally stay enabled -> recursion is call-cycle, NOT a
  deliverEvent codegen bug.

RECURSION = host-side nested-exception/dispatch issue: kernel event cycle
  exceptionHandler->deliverEvent->cb handler->B0 syscall->B0Handler->
  deliverEvent->returnFromException->exceptionHandler nests psx_dispatch ->
  g_psx_dispatch_depth grows -> dirty_ram_interp recursion guard.

Game CD regs CORRECT: 0x801BCB0C-18 = 0x1F801800-803 (banked CD interface).
CD controller ack = write 0x1F801803 index_reg==1 (cdrom.c:2973) clears irq_flag
  + re-arms latch. openEvent class consts: 0xF0000009 @0x8019B338 (mark event),
  0xF0000003 @0x80195B54 (different event class).

=== 2026-… cd_init_session ROOT CAUSE — corrected mechanism (game-side, not kernel) ===

The mark event the game polls is a GAME-side signal, NOT a kernel IRQ event.
Full clean-source trace (game/cd.c):

- cd_init_session (0x80198b.., game/cd.c:119): mc_session(0x80198590) registers
  session cb cd_init_done_cb; cd_cmd_issue_async(9,0) issues Pause(0x09) ONCE;
  sets DAT_801bcba4=1; waits <=0x79 frame ticks (cd_timer_read -> 0x801bdde0
  VBlank counter) for cb to clear flag. Timeout path returns normally.
- cd_init_done_cb (0x80198590, cd.c:526): mc_session(prev); DAT_801bcba4=0.
  Invoked via the CD IRQ -> cd_response_process -> session-callback path.
- The mark event (0xF0000009,0x20,MARK) opened via FUN_8019b380(0xf0000009,
  0x20,0x2000) at cd.c:1860/5679 is delivered by cd_dma_chan_reset (0x8019b7e8,
  cd.c:5786):  if DB32G(0x801bdee8)==0 -> cd_bios_svc_07(0xf0000009,0x20)
  else call_guest(0x801bdee8,0xf0000000,0).  Reached only after CD DMA/init seq.

Event-class map (verified against generated OpenBIOS + clean cd.c):
- Kernel IRQVerifier (func_000016E4) delivers per-IRQ-bit class:
    bit2 CDROM -> deliverEvent(0xF0000003,0x1000)   [0xBFC1F6E8]
    bit9 (0x200)-> deliverEvent(0xF0000009,0x1000)   [0xBFC1F738]
  deliverEvent (func_00002610) requires EXACT spec match (entry+8==a1) and
  status==0x2000; MARK mode(+0xC==0x2000) sets status=0x4000(READY).
- Game CD IRQ callbacks (cd_response_pump->cd_response_process, reads
  HINTSTS@0x1F801803 via ptr 0x801bcb18): INT2->cd_bios_cmd20
  DeliverEvent(0xF0000003,0x20); INT1->cd_bios_cmd40 DeliverEvent(0xF0000003,0x40).
- So class 0xF0000003 = CD channel events; class 0xF0000009 spec0x20 = the
  game's DMA-channel-reset done event.  Kernel's (0xF0000009,0x1000) is a
  coincidental class match, DIFFERENT spec -> does NOT satisfy the mark event.

cd_intc_dispatch (0x8019a8ec, clean cd.c:6244) = top-level INTC dispatcher:
  mask = *0x801bdda4(I_STAT 0x1F801070) & *0x801bdda8(I_MASK 0x1F801074) &
         u16@0x801bcd48(=0xD live). tbl[i]@0x801bcd1c: [0]=0x8019ae68 VBlank,
  [2]=0x801979f8 cd_response_pump(CD), [3]=0x8019af7c. Per set bit calls tbl[i].

OPEN QUESTION (next): why doesn't the CD init/DMA sequence reach
cd_dma_chan_reset -> mark event, forcing cd_init_session to time out and boot
to re-issue Pause(0x09) (the observed cmd=0x09 spam at pc=0x80196FC4)?
Suspects: (a) session cb 0x80198590 not invoked because CD INT2/INT3 response
path doesn't reach mc_session callback; (b) cd_timer_read frame counter frozen
(VBlank IRQ not delivered) so the 0x79 timeout never expires -> infinite spin;
(c) clean cd_init_session can't deliver IRQ (br2_irq_poll wild-store, cd.c:115)
so relies on generated body — verify which body actually runs.
The recursion-guard halt is a SEPARATE host-side nested-psx_dispatch issue.

## Boot unblocked -> CD file-load retry loop (commit 024e02e)

cd_init_session now runs the GENERATED body via br2_call_generated thunk
(deferred-IRQ wait needs per-block interrupt poll + live cpu->gpr; clean-C
spin could never receive INT2->cd_init_done_cb). Plus two clean-port pointer
bugs fixed: save_rec_table_copy host-ptr arg, apply_saved_settings
(int)param_1 64->32 truncation. Boot now completes settings and reaches
cd_file_pump (CD file loading).

CURRENT BLOCKER — CD file-load retry loop, not IRQ-rooted:
- PSX_RB_CD_BISECT ReadN-arm trace: only ~14 arms, LBAs cycle a tiny set
  {16,18,22, 216333,216334,216335, -150} — game re-reads the same files,
  never advances through the ~27-file pump. lba=-150 (invalid seek,
  mode 0x80->0xa0) is the failure point.
- Starvation dump: pc=0x8019A9AC (cd_intc_dispatch I_STAT-ack), i_stat=0x4
  (CDROM bit2), i_mask=0xD, in_exc=1. The IRQ "storm" is per-sector INT1
  data-ready during these reads — i_stat bit2 re-presents each generation
  because irq_flag is re-set continuously (sectors stream faster than the
  nested game->BIOS->game dispatch drains them).
- IRQ/ack chain verified correct END-TO-END: br2_st8p(0x801bcb18,7) ->
  psx_write_byte(0x1F801803) -> memory.c -> cdrom_write index_reg==1 ->
  irq_flag &= ~7. index_reg set via br2_st8p(0x801bcb0c,1)->0x1F801800.
  psx_irq_raise count ~0 during spin => i_stat bit2 stays SET (not re-raised);
  exception re-fires because the bit is never cleared -> cd_intc_dispatch
  re-entered continuously.

NEXT HYPOTHESES (ordered):
(a) CD sector DATA is wrong/corrupt -> file parse yields lba=-150 -> retry.
    Verify iso_read_sector output vs known-good sector for lba 16/18/22.
(b) Read-completion / CD-DMA-done event (mark event 0xF0000009,0x20 via
    cd_dma_chan_reset 0x8019b7e8, or INT4 data-end) not reaching game ->
    game thinks read incomplete -> retries. Check whether cd_dma_chan_reset
    is reached and whether INT4/CDIRQ_DATA_END is ever raised.
(c) ReadN stream never terminates: game expects N sectors then Stop, but
    runtime keeps delivering (reading=1 until stop_read_stream). If the
    game's sector-count/stop logic depends on a status the runtime
    mis-reports, the stream overruns.

## CD file-load: command_history decode (lldb, live process)

Attached lldb to the spinning process (debug server starved at 100% CPU).
Read cd_dma_log + command_history rings directly:

- cd_dma_log: directory reads -> scratch dest 0x8598; then SLES_017.22 EXE
  body -> dest 0x11e800+ advancing 0x800/sector, but **lba CONSTANT 216335**
  for ~300 entries. CD read position never advances between per-sector DMAs.
- command_history (82 cmds): each file sector = full cycle
  Setloc(0x02)->SeekL(0x15)->Setmode(0x0E,0x80)->ReadN(0x06)->Pause(0x09)->
  GetStat(0x01). Tail: Init(0x0A)->Demute(0x0C)->Pause x4 -> Setloc[0,0,0]
  ->Setmode(0xa0)->ReadN @ rd00:00:00  = the lba=-150 seek = drive reset+retry.
- dispatch_depth=1 (flat IRQ loop, NOT recursion). Thread in dma_advance.
  dma_cdrom_transfer_active()=0. cd_dma_log_total=333.

INTERPRETATION: game loads SYSTEM.CNF+SLES_017.22 correctly sector-by-sector,
then a file-load completion FAILS -> drive Init/reset -> re-seek MSF 00:00:00
-> whole load retries (the ReadN LBA cycle seen in bisect). The -150 read
returns a zeroed sector (lba<0 -> iso_read fails -> memset 0, data_delivered=1).
File offset 0 = LBA 0 = MSF 00:02:00 (PVD CD001 verified at file-sec 16); the
.bin has NO 150-frame lead-in, so MSF 00:00:00 legitimately has no data.

NEXT: find WHY the file load doesn't complete. The per-sector read cycle
(Setloc->SeekL->Setmode->ReadN->Pause->GetStat) must be signalling "sector
done" via an event/flag the runtime isn't producing, OR the last Pause/INT2
completion isn't reaching the game's file-job state machine -> timeout->retry.
Check cd_file_open_wait / job_result_poll_stable completion path and whether
Pause(0x09) INT2 + the mark event (0xF0000009,0x20 via cd_dma_chan_reset)
fire for the per-sector reads.

=== UPDATE: cd_cmd_issue_b/c pointer-cell mistranslation FIXED ===

ROOT CAUSE of the file-load livelock found via lldb stack + generated disasm:
cd_cmd_issue_b (0x80197800) / cd_cmd_issue_c (0x80197900) drive the game's
software CD command block at 0x801bcbXX. EVERY access is a pointer-cell
indirection: `lw ptr,[0x801BCBXX]; sb/sw/lbu/lw *ptr` into the CD register
file. The clean port mistranslated all of them as direct `DB32(0x801bcbXX)`
cell stores/reads. Concretely the done-poll `while(!(DB32(0x801bcb0c)&0x40))`
read the POINTER VALUE (a 0x1F8018xx register address) and tested bit6 of an
address — never the real status byte — so it polled forever at 100% CPU
(stack: cd_cmd_issue_b -> br2_irq_poll). Faithful generated code does
`lbu v0,0(v1)` where v1=*0x801BCB0C = the status byte through the pointer.

FIX: converted both to `br2_call_generated` thunks (0x80197800/0x80197900) and
removed their clean_table wrap+entries — same contract as cd_init_session.
Also fixed cdrom.c present_cdrom_irq: re-arm cdrom_intc_request_latched when
i_stat bit2 reads back clear while irq_flag still pending (level-triggered CD
IRQ line must re-assert after a generic I_STAT ack that didn't ack HINTSTS;
otherwise irq_flag sticks -> INT1 starved -> int1_lost ~48k).

RESULT: livelock broken — game now runs cd_file_pump -> gpu_invoke ->
cd_queue_push -> wrap_80192f14 actively (was hard-spinning in cd_cmd_issue_b).
cmds 82->83 but dmas still 333: file reads queue but DMA completion still not
landing. NEXT: trace why queued CD reads don't produce DMA — likely the
cd_queue consumer / CD-DMA ch3 drain or another pointer-cell mistranslation
in the queue-pop/execute path (cd_queue_a/b_status, cd_queue_a_read use
packed guest reads — verify they're not also direct-vs-indirect bugs).

=== UPDATE2: past cd_cmd_issue livelock -> cd_queue busy-wait stall ===

After the thunk fix the game reaches cd_file_pump -> gpu_invoke ->
wrap_80192f14(=cd_queue_push 0x80192f14) -> cd_queue_push -> busy-wait.
cmds 82->83 but dmas frozen at 333: queued CD reads never produce DMA.

NEW STALL: cd_queue_push (game/cd.c ~6113) and cd_queue_drain (~6068) contain
`while ((br2_ld32(DB32G(0x801b8428)) & 0x400000) == 0) { }` — a busy-wait on a
hardware status bit via pointer cell 0x801b8428 (into the 0x1F8010xx DMA
register file per comment @5900) with NO br2_irq_poll inside. If that DMA/GPU
status bit never sets the queue spins at 100% (stack: guest_at+cd_queue_push).
NEXT: identify which 0x1F8010xx register 0x801b8428 points to and what sets
bit22 — likely a DMA channel-ready/idle flag the runtime isn't producing, or
another pointer-cell mistranslation in the queue consumer (cd_queue_a/b_status,
cd_queue_a_read, cd_pump_watchdog). Verify the pointer cell's target register
and whether the corresponding device models that status bit.

=== UPDATE3: cd_queue stall ROOT-CAUSED to uninitialized pointer cell ===

Live lldb on the spinning process (cd_file_pump->gpu_invoke->wrap_80192f14
->cd_queue_push+644 busy-wait):

Pointer table @0x801b8420-0x801b8444 (register fast-path cells, populated by
generated init via computed addressing — NOT literal addrs, NOT clean C):
  8424 -> 0x1F801810  (GPU GP0/GPUREAD)
  8428 -> 0           *** UNINITIALIZED ***
  842c -> 0x1F801000  (mem_ctrl[0])   8430 -> 0x1F801004
  8434 -> 0x1F801008  (mem_ctrl[2])   8438 -> 0x1F801040 (SIO)
  843c -> 0x1F801044                   8440 -> 0x1F801048  8444 -> 0x1F801050
MMIO map (memory.c): 1F801000-03C mem_ctrl, 040-05F SIO, 060 ram_size,
070/074 I_STAT/I_MASK, 080-0FF DMA ch, 810+ GPU.

cd_queue_push (cd.c:6138) + cd_queue_drain (cd.c:6081) busy-wait:
  while ((br2_ld32(DB32G(0x801b8428)) & 0x400000) == 0) {}
DB32G(0x801b8428)=0 -> br2_ld32(0)=0x03000001 -> bit22 clear -> spins forever
(no br2_irq_poll in the wait). cd_pump_watchdog writes 0x200/0x100 to the same
reg (*0x801b8428=a2a) as reset commands.

NEXT: determine what register 0x801b8428 SHOULD point to (candidates: a DMA ch
reg 0x1F8010Bx CHCR/Dn_CHCR busy bit, I_STAT, or a SIO reg) and which init
routine populates this table — it is generated code using computed addressing
(lui/addiu base + sw offset), so grep for literal 0x801b8428 finds nothing.
Find the init via the pointer VALUES (search generated for the lui 0x1f80 /
addiu sequences building 0x1f801810 / 0x1f801000 stores), or breakpoint the
store that writes 0x801b8424=0x1f801810 and inspect the sibling store.
Once the correct target reg is known, either fix the init port or confirm the
cell is set later (GPU/DMA device-init ordering) and trace why it didn't run.

=== UPDATE4 (session): CD file reads now COMPLETE; blocker moved to seek-complete IRQ->DeliverEvent ===

Committed+pushed to mine/master. Game now streams real file data (was
lead-in-zero livelock). Faithful fixes, all "delegate to generated where
clean C lost guest state / starved IRQ":
- cd_reg_window_op op3 DMA-register MMIO via br2_st32p (34b3829)
- cd_result_wait_a/b -> generated (1e6264e)
- cd_file_load_sm -> generated: SetLoc gets real MSF 1:55:63, LBA 8538+
  (was [0,0,0] -> lead-in) (62a2d98)
- cd_read_open -> generated: Setmode 0xA0 whole-sector+stamp, reads complete
  (1e1a5fb)
- cd_seek_complete_poll -> generated: CD deferred coroutine now resumes
  (0x8019A87C in sp_ring) (5b8c8f4)

CURRENT BLOCKER (verified live, headless :4370):
- Game reaches cd_file_load_sm -> cd_seek_complete_poll -> spins in BIOS
  TestEvent on mark-event handle 0xF1000008 (class 0xF0000009 spec 0x20
  MARK). bioscall_dump: TestEvent(B0:0x0B)~660M, DeliverEvent(B0:0x07)=49
  frozen.
- File 0x80 read fully (mode 0xA0, remaining->0, sectors thru LBA 337).
  Then SeekL->0:6:15 + Pause(0x09). Device IDLE: seq 7941, irq_flag=0,
  i_stat=0, pending.cmd=0x09 active=0.
- cdrom.c Pause path CONFIRMED fires CDIRQ_COMPLETE via
  set_irq+fire_cdrom_irq (cdrom.c:2562-2566); irq_enable=0x07 mask passes
  reason 2 -> psx_irq_raise(2,2) -> I_STAT bit2 (imask 0xD enables it).
- BUT cd_response_pump counter 0x801bce04 frozen at 152 -> the COMPLETE IRQ
  is ACKed (irq_flag=0) yet never reaches response_pump->cd_dma_chan_reset
  ->DeliverEvent(0xf0000009,0x20). Break is in kernel->game-jmpbuf->
  cd_intc_dispatch->tbl dispatch, NOT the device.
- irqctx_ring: only VBLANK IRQs recently (CD IRQs buried under vblank wrap).
- cd_dma_chan_reset (clean_table wrap_8019b7e8) delivers mark event gated
  on 0x801bdee8==0 (verified =0) else call_guest(*0x801bdee8,...).

CLEAN-SOURCE integration (committed, runtime-verified reach CD stall, 0 crash):
- save.c: 28 faithful leaf ports (dead code until fnmap wired) (9e9f51a)
- ev.c: EV handler rewrites + EV7/12/13 delegates, live via
  ev_stream_dispatch->PTR_FUN_801b4e68 (9075cbb)
- bind.c: weak stubs -> br2_call_generated delegates incl strong
  return-0 holes FUN_801468ec/8018b100; br2_guest_frame stack-arg staging
  (9075cbb)
- battle.c agent ports REVERTED: COP2 delegates had host/guest ptr bug ->
  ADEL crash; needs redo with correct guest_of marshaling (gameplay-only,
  deferred). Agents running: CD-IRQ diagnosis, resolve.c, gfx.c ports.

=== UPDATE5 (session): ROOT CAUSE of nondeterministic CD->.text DMA found+fixed ===

Symptom: CD-ROM DMA sometimes wrote file data over dispatched .text
(0x8018E0B0..0x801970D0) -> unknown-dispatch crashes. Good runs loaded the
same file at 0x801320D0; bad runs at 0x8018E0D0 (delta 0x5C000).

ROOT CAUSE (host-pointer-as-guest-address in clean-C stream decoder):
- stream_block_decode (menu.c, guest 0x80163154) took `int *param_2` =
  HOST pointer into guest RAM (all menu.c/game.c callers pass
  `(int*)guest_at(tbl)`), but computed mode3_dispatch's a1 as
  `(int)param_2 + entry + (count+1)*4` — truncating the HOST pointer to
  32 bits. mode3_dispatch then did guest_at(~0x87xxxxxx) -> open-bus
  mode word -> nondeterministic return length -> heap cursor cells
  (_DAT_801e3cfc @0x801E3CFC, _DAT_801c47b8 @0x801C47B8) corrupted
  (observed live: 0x878B0FD0/0x878BC7D0 in cursor AND DAT_801BCB84/88
  CD dest cells) -> cd_read_setup wrote bogus dest -> DMA into .text.
- Same bug class in save.c: direct stream_block_decode call sites passed
  GUEST addresses (g32() values) where the contract wants a host int* —
  wild host dereference ~0x80xxxxxx.
- mode3_dispatch was `void` relying on v0-leftover for the consumed
  length; MIPS original returns block_copy_ret_len/lzss_decode length,
  or v0=2 for unhandled mode words.

FIXES (faithful, general — guest_of()/guest_at() marshaling):
- menu.c stream_block_decode: a1 = guest_of(param_2) + entry + hdr.
- save.c: corrected extern decl; wrapped table args in guest_at() at
  card_sprite_load_sm + display_page_commit_a/b call sites.
- save.c mode3_dispatch: returns uint32_t faithfully (len / 2 default).
- menu.c extern decl updated to match.

VERIFICATION: two fresh headless runs post-fix stable >2M/1.7M frames;
cells now hold valid guest ptrs (0x801E3CFC=0x8003F5DC,
0x801BCB84=0x8003F5DC, 0x801BCB88 advanced) vs garbage pre-fix.

ALSO THIS SESSION: added guest_watch.c — mprotect/lazy-finish guest-RAM
write watcher (gwatch/gwatch_dump/gwatch_disarm/gwatch_reset cmds) to
cover clean-C raw guest_at() stores that bypass psx_write_*/wtrace_all.
On macOS arm64e code-page patching is unsafe (W^X/recursive SIGBUS), so
the watcher uses protected-page faults + lazy finish at guest_at()/block
ticks; it captures host RIP + guest store PC + reg provenance per write.
NOTE: watcher sees CPU stores only — DMA writes bypass it (CD cells are
better traced via wtrace_all + fntrace PSX_FN_FILTER env).

FOLLOW-UP AUDIT (same bug class, not yet fixed): `(int)(intptr_t)&DAT_*`
casts of host scratchpad pointers in battle.c (~31 sites) truncate host
pointers to 32 bits — verify each callee's contract before changing.

## 2026 — CD-completion blocker confirmed KERNEL-side; cd_dma_chan_reset MMIO fix

STATUS: game stable (no crashes, frame counter advances = vblank only) but
still logically parked at the SAME CD stall: cdrom_state seq:7941 lba:337
seek_msf[0,6,15] Pause phase:1 irq_flag:0; response_pump frozen@152; PC
parked calling memcard_info_query (0x80126290->0x8015F590) which is
INCIDENTAL per-frame work, not the blocker (agent e6eb9e42 confirmed).

CONFIRMED (live probes :4381 + generated oracle):
- Deferred-IRQ contract intact: mem[0x4BF8]=0x801BCD50 -> cd_ctx_save frame
  registered; frame resume target 0x8019A87C valid+dispatched; state blk
  0x801BCD18 in-service=1, tbl[0]=0x8019AE68 tbl[2]=0x801979F8.
- Coroutine DOES resume (sp_ring shows 0x8019A87C->kernel->back) but
  cd_intc_dispatch never reaches the completion handler -> DeliverEvent
  (0xF0000009,0x20) never fires -> TestEvent(0xF1000008) spins.
- Per plan 2026s: kernel cdromIOHandler/cdromDMAHandler are MISSING from
  handlersArray(mem[0x100]) OR their verifiers reject IRQ bit2 -> every CD
  IRQ falls to game jmpbuf; kernel never acks (mem[0x6514]=0 autoack OFF)
  -> IRQ re-fires; game handler delivers wrong class 0xF0000003 vs mark
  EvCB 0xF0000009 -> mark event never READY(0x4000).

FIX THIS SESSION: cd_dma_chan_reset (game/cd.c) had the SAME MMIO
open-bus mistranslation the agent fixed elsewhere — w[0xd5] CD channel
status @blk+0x1aa via guest_at() on 0x801bdeb0 cell hit open-bus mirror.
Converted to CDMMIO16/CDMMIO16_ST. Committed 44ce842. (Correct faithful
fix; not the direct unblock — the channel-callback may not even be the
one not firing; kernel HandlerInfo registration is the real locus.)

REMAINING (kernel RE, agent c9f8c5a4 instrumented): determine whether
cdromIOHandler/cdromDMAHandler are absent from handlersArray or verifier-
rejected; fix enqueueCDRomHandlers/initializeCDRomHandlersAndEvents
ordering or cdromIOVerifier/cdromDMAVerifier IRQ-bit test in the
recompiled kernel — faithful core fix, not a game patch.

## 2026 — CD stall narrowed to IRQ2-not-dispatching; device side proven sound

LIVE PROBES (:4390, post-MMIO-fix binary):
- cd_intc_dispatch clean-thunk counter @0x801bce00 = 0 -> guest NEVER jumps
  to 0x8019a8ec. fnmap routes it to the clean thunk, so this is definitive:
  the CD interrupt dispatcher is never invoked -> the cd_ctx jmpbuf coroutine
  never resumes -> DeliverEvent never runs.
- evcb[8] @0x8ee8 decoded: class=0xF0000009 spec=0x20 mode=MARK(0x2000)
  status=0x2000 STILL WAITING (never 0x4000 READY). Confirms
  DeliverEvent(0xF0000009,0x20) never executed. cd_dma_chan_reset would call
  cd_bios_svc_07(0xf0000009,0x20) in its else-branch (0x801bdee8==0 live)
  but is never reached (indirect channel-task callback).
- irqctx_ring: EVERY entry vblank:1 cdrom_active:0 istat:0x1 -> NO CD IRQ2
  context switch ever recorded despite imask:0xD (bit2 enabled).
- cdrom_trace_dump: i_stat:0x4 (CD IRQ bit2 ASSERTED at INTC) while CPU spins
  polling 0x1F801800/0x1F801803 in cd driver (pcs 0x8019787C/0x80196984/
  0x8019780C = cd_cmd_issue_b region). IRQ line high but exception never
  taken -> break is INTC->CPU IRQ2 dispatch, OR game in permanent DI.
- Device side PROVEN: Pause(0x09) pending_arm->process_pending->
  set_irq(CDIRQ_COMPLETE)->present_cdrom_irq->psx_irq_raise(2,2); mask
  cdrom_irq_mask_matches_reason(0x07,2)=match. irq_flag consumed (0),
  pending disarmed -> completion fired and was acked.
- ANOMALY: cdrom_state still reading:1 read_cmd:0x06 read_delay~218K
  sector_pos advancing AFTER Pause -> read stream may not have stopped
  (stop_read_stream in Pause handler) — possible device-model secondary bug.

SUSPECT ORDER: (1) CPU/INTC IRQ2 exception dispatch (i_stat bit2 set but
no take) — check psx_irq_raise(2)/exception path & SR.IM/IE state; (2) game
CD driver polling in DI; (3) kernel exceptionHandler not reaching jmpbuf
tail. Agents c9f8c5a4 (IRQ->dispatch) + da811ba6 (OpenBIOS handler reg).

FIXES THIS SESSION (faithful, committed): cd_dma_chan_reset + cd_dma_xfer_
start + cd_dma_xfer_start_b + cd_reg_bit40_ctrl MMIO open-bus -> CDMMIO16
(44ce842, 1d5c086). Verified no-regression; stall unchanged (kernel/INTC
locus, not these).

## 2026 — CD stall: game owns INTC; Pause CDIRQ_COMPLETE dropped (pending.pending=0, no set_irq(2))

MAJOR REFRAME (agent da811ba6 + OpenBIOS source fetched):
- Game DELIBERATELY removes kernel CD handlers (deinitCDRom A0:72,
  CloseEvent x5, DequeueCdIntr A0:A3, SysDeqIntRP) and installs its OWN
  INTC @0x801BCD18, handler tbl @0x801BCD1C: tbl[2]=cd_response_pump
  (0x801979f8). Absence from handlersArray is BY DESIGN, not a bug.
- cd_intc_dispatch (0x8019a8ec) DOES run ~1.3M times (per-IRQ dispatch) —
  earlier counter@0x801bce00=0 was a FALSE signal: generated dispatch
  handles 0x8019a8ec directly, bypassing the clean thunk.
- OpenBIOS statemachine.c confirms kernel complete() delivers spec 0x0200
  for IDLE state anyway — game must use its own jmpbuf/INTC path. Kernel
  side is faithful; bug is in CD device pending-completion delivery.

COMMAND HISTORY (cdrom_command_history, port 4382): game loops
SeekL(0x15)->Setloc(0x02)->Setmode(0x0E)->ReadN(0x06)->Pause(0x09).
84 cycles COMPLETED fine over frames 4->340. 85th Pause (seq85,frm340)
STALLS. So Pause->CDIRQ_COMPLETE->DeliverEvent works generally; one
instance drops it.

DEVICE PATH (cdrom.c): Pause case0x09 -> set_irq(CDIRQ_ACK=3) +
pending_arm(0x09,~1.1-2.2M cyc,phase1). process_pending (every
cdrom_advance) gates on `psx_cycle_count>=due_cyc && irq_flag==0`, then
set_irq(CDIRQ_COMPLETE=2)+fire_cdrom_irq. irq_enable=0x07 mask OK.

ANOMALY (the precise break): cdrom_state pending={cmd:0x09,active:0,
delay:0,phase:1} -> pending.pending=0, irq_flag=0, reading=0, i_stat=0x1.
NO set_irq(0x02) in cdrom_trace window; NO new exec_command after seq85.
pending.pending cleared WITHOUT firing CDIRQ_COMPLETE. Only two clears:
line2102 (new-command cancel, faithful psx-spx) & line2529 (completion,
which WOULD set_irq2). Neither obviously applies -> suspect a QUEUED
command via try_execute_queued_command->exec_command cancel, OR
irq_flag!=0 starvation during residual ReadN DATA_READY drain, OR a
pending-clear path not yet found. NEW COMMAND cancel is documented
faithful, so if a queued cmd cancelled it, real HW would too -> then
the game's wait would be a genuine game-side expectation needing the
completion delivered differently.

GAME SIDE: cd_response_pump reads reason CDB8P(0x801bcb18)&7; reason2
returns 2 -> calls call_guest(DB32G(0x801bc848),..) ONLY if !=0.
LIVE: 0x801bc848=0 (mc_session callback cleared) -> even IF pump ran,
complete callback skipped. Secondary suspect. cd_dma_chan_reset
(DeliverEvent 0xF0000009/0x20) is the REAL mark-event path, gated behind
indirect channel-task cb — 0 hits.

NEXT: instrument the single Pause-completion episode — watch
pending.pending transition + set_irq(2) + irq_flag ack source (guest PC
of the 0x1F801803.1 write). Distinguish queued-cmd-cancel vs irq_flag
starvation vs missing-clear. Agent c9f8c5a4 tracing IRQ->dispatch.

## 2026 — session: ch4-DMA→mark-event chain PROVEN working; deferred coroutine never resumes

LIVE PROBES (:4412, fresh binary w/ cd_cb7_run fix 106698b):
- dma_trace seq1579-1581: ch4 kick(frame341, pc=0x80156608, 10560w) ->
  delayed-complete(frame342) -> DICR 0x00900000->0x90900000 (bit28) +
  IRQ3(i_stat 0x08) -> DICR write pc=0x8019B008 INSIDE cd_cb7_run clears
  flag -> handler -> DeliverEvent. FULL CHAIN WORKS when ch4 completes.
- After that: NO further ch4 kicks. ch4 idle (active=0, remaining=0,
  chcr=0x201). Game is NOT re-kicking ch4 -> upstream gate, not DMA bug.
- pc_probe armed live: cd_intc_dispatch(0x8019A8EC)=155061 hits (runs
  constantly, per-IRQ); cd_cb7_run(0x8019AF7C)=0, tbl4 0x8019B7E0/
  cd_dma_chan_reset 0x8019B7E8/tbl4op4 0x8019AC04=0 in window (no DMA
  IRQ since arming — consistent w/ ch4 idle).
- cd_cb7_run runs GENERATED for the INTC jalr path: clean_table key is
  0x8019af84 (mid-func), entry 0x8019af7c not clean-routed, so generated
  body runs whole. Clean-port a0->DICR-cell fix (106698b) is FAITHFUL
  (generated reads *(*(0x801BDDF0)=0x1F8010F4)>>24&0x7F, ignores a0) but
  is NOT the runtime bug.
- DEFERRED COROUTINE: cd_chan_full_init(0x8019a814) gates on
  u16@0x801BCD18 (in-service): set->resume 0x8019A8DC, clear->init
  0x8019A838. cd_ctx_save(0x8019ad90) snapshots ra/sp/fp/s0-s7/gp to
  frame; B0/0x19 arms deferred ctx. LIVE: 0x4BF8=0x801bcd50(frame),
  0x4C18=0x2b5c -> ctx ARMED. cd_ctx_restore(0x8019ADCC)=0 hits ->
  kernel NEVER resumes the coroutine.
- Kernel ExceptionHandler (bios disasm): saves TCB ctx, runs IntRP
  queues ToT[0] func1()->func2(result), then restores EntryInt jmp_buf
  (JB_PC/JB_SP/JB_GP/JB_FP/JB_S0-7) and jr ra. cd_ctx_restore reached
  via EntryInt.JB_PC or a stored cb — NOT a direct jal (only dispatch-
  table entry exists).
- HLE check: hle_service_b0 handles only svc 0x07-0x0D; svc 0x19 ->
  default LLE. Deferred registration runs faithful recompiled BIOS.

UNIFIED HYPOTHESIS: game yields CD coroutine via cd_ctx_save + arms
deferred ctx; on CD IRQ the kernel should resume via cd_ctx_restore ->
cd_chan_full_init -> cd_intc_dispatch -> consume completion -> issue
next CD read -> ch4 DMA kick -> ch4 complete -> mark event -> TestEvent
ready. cd_ctx_restore=0 => coroutine starved => no new CD/ch4 activity
=> mark event never re-delivered => TestEvent spins => black screen.
ALTERNATIVE (plan's device-side): 85th Pause pending.pending dropped
without set_irq(0x02) -> game never sees INT2 -> same downstream stall.
DISCRIMINATOR NEEDED: does the stalled Pause's CDIRQ_COMPLETE(INT2)
actually fire+get consumed? If INT2 dropped -> cdrom.c pending bug.
If INT2 consumed but coroutine still starved -> kernel EntryInt/deferred
dispatch bug (who sets EntryInt.JB_PC=cd_ctx_restore / reads 0x4BF8).

=== B0 TABLE DECODE (bios disasm B0_Table, index=t1) ===
0x07 DeliverEvent, 0x08 OpenEvent, 0x09 CloseEvent, 0x0A WaitEvent,
0x0B TestEvent, 0x0C EnableEvent, 0x0D DisableEvent, 0x17
ReturnFromException, 0x18 ResetEntryInt, **0x19 HookEntryInt**,
0x20 UnDeliverEvent. => the game's cd_chan_full_init calls
HookEntryInt(jmpbuf) to arm EntryInt; BIOS ExceptionHandler then
restores EntryInt (JB_PC/JB_SP/JB_GP/JB_FP/JB_S0-7) + jr ra on EVERY
irq/exception -> resumes coroutine.
cd_ctx_restore(0x8019ADCC) signature decoded: a0=frame, restores
ra/sp/fp/s0-s7/gp from frame, **v0=a1**, jr ra. Frame=0x801BCD50
(state blk 0x801BCD18+0x38). Resume lands block_8019A87C: v0!=0 ->
jal cd_intc_dispatch. So kernel must call cd_ctx_restore(0x801BCD50,
val) — reached via EntryInt.JB_PC=stub->cd_ctx_restore, NOT a direct
jal (only dispatch-table entry exists).
NEXT KERNEL CHECK: is HookEntryInt(B0/0x19) actually invoked + does
recompiled-BIOS ExceptionHandler EntryInt-restore (the longjmp,
psx_restore_state_escape) correctly resume? Verify EntryInt ptr
(dword_75D0->guest RAM) points at a jmp_buf whose JB_PC leads to
cd_ctx_restore at the moment CD IRQ2 fires.

=== 2026 — BREAKTHROUGH: game progressed PAST CD/SPU stage; now blocked in memcard channel-job ===
Live-verified on port 4427 (HLE default). The earlier TestEvent/ch4/SPU-stream
stall is RESOLVED upstream — game now runs versus_state_init + versus-screen
setup, then blocks in memcard_info_query. g_game_state@0x1F800000 stays 0.

EXACT BLOCK CHAIN (all clean C except final handler):
  br2eu_func_80126290 -> jal 0x8015F590 memcard_info_query (clean,
    game/game.c:2222) -> mc_data_read (save.c:3879) -> mc_op_result_poll
    (cd.c:1985) -> cd_job_result(0,..) (cd.c:2571) spins
    `while(st[2]@0x801febc8==0) br2_irq_poll()` forever.
  Channel-job state @0x801febc0: st[0]=2 (in-service), st[2]=0 (never done).
  CPU parked at return-addr 0x801262A8 (40/40 get_registers samples identical).

JOB DISPATCH (clean, running every frame — confirmed in clean_trace):
  op_chan_fn_invoke (save.c:8035, guest 0x801a0a8c) reads count
  *(0x801be768)=0, handler *(0x801fef60+0*4)=0x8019eb00, calls
  fn(0x801ff960+0*16). Handler returns 0 (pending) forever -> slot never
  popped -> st[2] never set -> mc_op_result_poll spins.

HANDLER 0x8019EB00 = generated br2eu_func_8019EB00
  (generated/SLES_017.22_full_48.c:13675, 680B/48 blocks). Reads context
  a0=0x801ff960 (live: ALL ZEROS), opcode=context[0]=0 (<51 -> memcard-read
  op). Returns 0 pending. SIO/memcard FROZEN: mc_probes=74/acks=74/reads=72
  all done, 0 new cmds since versus-init -> handler NOT issuing SIO; stuck
  waiting on a flag/status before issuing, or a completion IRQ that never
  fires. Cards card1.mcd/card2.mcd present, memcard_dir="saves" configured.

SUSPECT: clean-port mistranslation in the memcard-job channel chain —
slot-context base mismatch is suspicious: cd_job_slot_alloc zeroes
0x801fef2c+idx*0x10 but op_chan_fn_invoke passes 0x801ff960+i*16 as handler
arg (different regions). Handler may get wrong/empty context -> returns 0.
Agent 5e538f5f tracing handler wait-condition + slot-base correctness vs
generated oracle.

=== 2026 — RESOLVED: memcard/job-channel stall -> game reaches versus/title load ===
Three faithful clean-port fixes unblocked the entire channel-job system.
All verified against generated oracle + live debug-server probes.

FIX 1 (536f76c): job-channel state block base 0x801FEBE0, not 0x801FEBC0.
  cd_chan_job_submit/cd_job_result/cd_mc_op_wait/cd_chan_job_pump all used
  0x801FEBC0 (0x20 low). Submit wrote mode=2 to phantom block; real state
  stayed idle -> handler saw idle channel, issued no SIO -> done-flag never
  set -> cd_job_result spun -> memcard_info_query hung.

FIX 2 (48944f0): op_chan_fn_invoke dispatch fn_lookup -> call_guest.
  Slot fn-ptrs (0x801FEF60+i*4) are guest addresses that may be generated
  OR clean. fn_lookup only resolves clean -> returned NULL for generated
  handler 0x8019EB00 -> handler never invoked. call_guest reproduces jalr:
  clean_try_dispatch first, psx_dispatch_call fallback. Live: SIO memcard
  probes 74->112, reads 72->109.

FIX 3 (16dfdff): cd_job_slot_alloc zero region 0x801FEF20, not 0x801FEF2C.
  Generated 0x801A0A10 zero-fills 16-byte slot record at 0x801FEF20+idx*16
  (count-down loop ending at +0xC). Clean used 0x801FEF2C -> ctx state word
  (*s1 handler opcode) NEVER cleared -> handlers inherited stale state
  (ctx=31 leftover), hit default: return 0 forever -> job starvation.
  Slot records tile [0x801FEF20,0x801FEF60) ending at fn-ptr table.

LIVE RESULT (port 4427, HLE default):
  - slot index 0x801BE768 -> -1 (all channel jobs drain/pop)
  - g_game_state 0x1F800000: 0 -> 4 (versus scene_entry loading)
  - CD sequencer streaming versus/title assets: seq 7941 -> 4.6M+, reading=1
  - GPU active: disabled=0, gp0_draw/env/writes climbing
  - cd_read_log: diverse LBAs (315/8538/9542/216335) = multi-file asset load
  - clean_trace hot fns now cd_intc_busy_read/cd_timer_read/cd_response_pump
    (CD busy-wait read loop) — main thread in scene_entry->cd_read_poll.

CURRENT: state 4 = scene_entry(0x80010000,..) loading versus/title scene.
  Returns 0 while loading -> stays 4; nonzero -> state 5. Heavy asset pull
  (seq still climbing). draw count 597 during load (static loading screen).
  NEXT: confirm 4->5 transition + title/menu render + screenshot proof.

=== 2026 — CD same-sector retry livelock: EntryInt deferred-coroutine resume broken ===
After the channel-job fixes the game reached state 4 and streamed thousands of
CD sectors, then hit a NEW stall: same-sector retry livelock at LBA 208829.

LIVE EVIDENCE (port 4427, cdrom_command_history + cdrom_trace_dump + pc_probe):
- Game loops SetMode(0x0E p=0)x2 -> SetLoc(0x02 p=46,26,29) -> ReadS(0x1B)
  forever at pc=0x80196FC4 (func 0x29CC). Same LBA every iteration.
- cdrom_state: irq_flag=0x03 STUCK (ACK+DATA_READY never cleared),
  irq_enable=0x07, int1_pended grows unbounded (2500+), sector_available=0,
  read_msf frozen [46,26,29], reading=1, stat=0x22, pending.cmd=0x09(Pause).
- Game READS 0x1F801803 -> 0xE3 (=irq_flag|0xE0) — sees pending INT, never acks.
- i_mask=0xD (IRQ2 CDROM enabled), cop0_sr IM/IE on; IRQ2 fires transiently
  (i_stat=0x04 in cdrom_trace). irq_flag clears ONLY on guest ack WRITE to
  0x1F801803 index_reg==1 (cdrom.c:2982 irq_flag &= ~(val&0x1F)).
- cdrom.c: irq_flag!=0 && no DMA -> deliveries take pending_dataready path ->
  s_int1_pended++ (matches growth). Same-sector retry = game never sees
  data-ready consumed.

pc_probe (3s window, decisive):
  cd_intc_dispatch 0x8019a8ec = 623 hits (runs per-IRQ, mostly VBLANK)
  cd_ctx_restore   0x8019adcc = 0 hits  <-- coroutine NEVER resumes
  cd_response_pump 0x801979f8 = 0 hits  <-- IRQ2 sub-handler never called
  cd_chan_full_init 0x8019a814 / cd_ctx_save 0x8019ad90 = 0 (ran at init)

ROOT CAUSE (matches plan's 2624-2687 decode): the CD completion coroutine is
armed via cd_ctx_save + HookEntryInt(B0/0x19). On CD IRQ2 the kernel
ExceptionHandler must restore EntryInt jmp_buf (JB_PC/SP/GP/FP/S0-7) via
psx_restore_state_escape -> deferred_exception_longjmp -> psx_fiber_switch
(g_exception_owner_fiber) -> resume -> cd_ctx_restore(0x8019adcc) ->
cd_intc_dispatch -> cd_response_pump -> ack write 0x1F801803 -> clear irq_flag.
cd_ctx_restore=0 => EntryInt resume never fires => coroutine starved => no ack
=> irq_flag stuck => same-sector retry livelock.

NOTE: cd_intc_dispatch clean-thunk counter @0x801bce00=0 is a RED HERRING —
generated 0x8019a8ec runs directly (623 pc_probe hits); the clean thunk is
bypassed. Do NOT use that counter to judge dispatch liveness.

OPEN: is the break (a) HookEntryInt(B0/0x19) never invoked/armed, (b)
EntryInt jmp_buf JB_PC doesn't lead to cd_ctx_restore, or (c)
psx_restore_state_escape/psx_fiber_switch fails to resume the owner fiber?
Tracer 1a9f9079 on the EntryInt/ExceptionHandler/fiber path.

=== 2026 — FIXED: cd_mode_set_0e host-ptr -> SetMode param 0x00 (commit 0bf25c2) ===
ROOT CAUSE of the mode/sector-error livelock found + fixed. clean
cd_mode_set_0e passed `&b` (HOST stack ptr) as param_2 to cd_mode_flag_set
-> generated cd_cmd_issue read it as a GUEST addr -> out of guest RAM ->
delivered SetMode param 0x00 (user-data, no MSF stamp) instead of the
computed mode byte -> cd_read_step_cb "sector error" -> cd_read_open(1)
retry livelock. Same class as cd_read_open/cd_cmd_issue guest-stack params.
FIX: delegate whole body to br2_call_generated(0x80198F60); regenerated
clean_table.c (gen_clean_table.py) drops wrap_80198f60 (br2_call_generated
bodies are skipped) so 0x80198F60 routes to generated directly — no
recursion (br2_call_generated bypasses clean_try_dispatch top-level, and
the addr is no longer clean-routed).
LIVE VERIFY: mode_reg 0x00->0xE0, SetMode delivers 0xE0, int1_pended
2500+->~90 (IRQ storm gone), IRQ/ack/dispatch all confirmed working.
NOTE: br2_call_generated does NOT fully bypass clean dispatch —
psx_dispatch_game_compiled calls clean_try_dispatch FIRST, so delegating
a still-clean-routed addr recurses; the target MUST be dropped from
clean_table (regen) for delegation to be safe.

REMAINING BLOCKER (next): read still livelocks at LBA 208829 but now for a
DIFFERENT reason — sectors are NOT delivered as DATA_READY.
- cdrom_trace: set_irq ONLY type 0x03 (CDIRQ_ACK); CDIRQ_DATA_READY(0x01)
  NEVER fires -> deliver_read_sector's set_irq(1) not reached -> INT1
  consumer cd_read_step_cb (0x801985c4) starved -> no BFRD/CHCR kick.
- last_sector.lba=208829 recorded (a delivery happened) but read_msf frozen,
  read_delay re-arms non-monotonically each retry (game re-issues
  SetLoc+ReadS before consuming).
- cd_cmd_issue_b(0x80197800)/cd_cmd_issue_c(0x80197900)/cd_mmio_bit_wait
  (0x8019799c)/cd_read_step_cb(0x801985c4) = 0 pc_probe hits.
- Pointer cells VERIFIED correct: 0x801bcb50->0x1F8010B8(CHCR),
  0x801bcb0c-18->0x1F801800-803. Not a corrupted-cell bug.
NEXT: why doesn't DATA_READY fire / why does the game retry ReadS before
sector delivery? Check deliver_read_sector gates (irq_flag!=0 pend path),
whether read_delay ever expires, and whether the INT1->cd_read_step_cb->
BFRD/CHCR chain has another host-ptr mistranslation.

=== 2026 — CD retry-livelock narrowed: re-open every ~32k cyc, not sector timeout ===
Deep live-probe of the post-0bf25c2 livelock at LBA 208829.

TIMING DECODED (cdrom_command_history cycle stamps):
- Full quad SetLoc(0x02)->SetMode(0x0E,0x00)->SetMode(0x0E,0xE0)->ReadS(0x1B)
  repeats CONTIGUOUSLY every ~32k guest cycles (~8k per command), all inside
  one frame. ReadS re-issued ~14x before the ~450k-cycle first-sector delay
  could elapse -> start_read_stream resets read_delay + clears pending_dataready
  each time -> sector 208829 re-delivered via pend path but INT1 never presents.
- cd_timer_read reads MMIO 0x1F801110 = Timer1 COUNT (timers base
  0x1F801100+t*0x10). T1 mode=0x1D00 -> HBLANK clock, 2146 cyc/tick.
  => 0x3c=60 ticks ~=128k cyc, 0x4b0=1200 ticks ~=2.6M cyc.
- Re-open cadence ~32k cyc ~=15 T1 ticks < 60-tick per-sector timeout
  => the cd_read_poll retry is NOT the sector timeout; it is almost
  certainly DAT_801bcb94<0 (cd_read_open returning -1 error) OR a
  different caller. DAT_801bcb94 sampled = 0 (need finer sampling).

cd_read_open clean body (cd.c:452, real=generated 0x80198908) retry path
(param_1!=0) issues: cd_mode_flag_set(9)=Pause -> (2)=SetLoc -> (0xe)=SetMode
-> cd_cmd_issue_async(6)=ReadN; sets DAT_801bcb94=count, DAT_801bcb98=now.
Returns -1 on: status bit4 early-out, mode_flag_set(2)==0, or
mode_flag_set(0xe)==0. mode_flag_set->cd_cmd_issue waits for the command
response; a 0 return = command/response timeout.

clean_trace dominated by cd_timer_read(0x8019A460)+vsync-wait
(0x8019A7D4) from RAs 0x80197030/0x801970F0 (a sync wait loop), plus
cd_response_pump(0x801979F8)<-0x8019A9C8 and cd_mode_flag_set(0x80195D8C)
<-0x80198F84. cd_read_poll/open/step_cb absent (generated thunks).

OPEN (tracer 06ce9cfc): exact caller of retrying cd_read_open; whether it
returns -1; which cd_read_poll condition fires; does read_delay ever
expire before next ReadS; any BFRD(0x1F801803 idx0)/CHCR(0x1F8010B8)
write ever. Hypothesis: cd_mode_flag_set's synchronous command-response
wait returns 0 (ACK/response not seen in time) -> cd_read_open -1 ->
tight re-open; OR a higher-level file-read loop re-calls without waiting.

=== 2026 — mc_date_serial_cmp delegated; DATA_READY now fires; new blocker = cd_result_wait_b busy ===
FIX COMMITTED 509c55a (pushed mine): mc_date_serial_cmp (0x80159ab4) clean-C
poll loop starved guest time + IRQs (same contract as cd_file_read_seq /
cd_result_wait_a/b / cd_init_session). Delegated whole body to
br2_call_generated(0x80159ab4); guest_of() recovers the two guest_at() host
ptrs; gen_clean_table dropped wrap_80159ab4.

LIVE-VERIFIED post-fix:
- irq_flag clears (was stuck 0x03); int1_pended/int1_lost stay 0;
  sector_available=1; read_msf advances through the ReadS stream.
- CDDBG histogram 0x801bce34+type*4: type-1 (INT1/data-ready) processed
  ~28.6k times; pump 0x801bce04 ~117k. DATA_READY DOES present now — the
  earlier "DATA_READY never fires" note is STALE.
- cd_response_pump -> cd_response_process case1 returns 4 ->
  call_guest(*(0x801bc84c)=0x80198FE4=cd_mc_sm_wrap) -> cd_mc_state_machine.

REMAINING BLOCKER (precise): cd_mc_state_machine (clean, 0x80199440) runs
per INT1 but ALWAYS early-outs at cd.c:3762-3764 — status byte at
guest_sp()-0x10 (filled by cd_result_wait_b_fwd(1,...) -> cd_result_wait_b
@0x80196B4C, already generated) has bit 0x04 set -> DB32(0x801bcc88)=3 ->
return BEFORE cd_chan_cmd_submit / cd_dir_entry_finalize. Channel entry
state stays 0 (0x80010000 u16=0), ch3 CHCR stays 0, blk_io_retry -1 ->
ReadS reissue livelock persists.

cd_result_wait_b decoded (generated full_45.c:18596, 712B/37blk):
- prologue saves regs, s3=param_2(out buf); s2=0x801BCB24, s6=+1, s4=+2
  (result-code cells cd_response_process writes); fp=0x801BC86C,
  s5=0x801BC8EC.
- arms timeout cells 0x801FC6E0(deadline)/E4(count)/E8(name 0x80125834)
  — same mechanism as cd_busy_wait; deadline via cd_timer_read(0x8019A460).
- polls result bytes + counter vs 0x3C0000; on timeout prints "Sync" err
  + cd_cmd_state_reset(0x801972A8).
- two jalr predicates: _jt_80196CEC (a1=0x801FC6D0) and _jt_80196D1C
  (a1=0x801FC6C8) — the result-buffer addrs cd_response_process fills
  (case1->0x801fc6d0, case2/3->0x801fc6c8). v0 = fn ptr (result decoder).
- LIVE: 0x801bcb24=0x02(ack ok), 0x801bcb25=0x00, 0x801bcb26=0x00;
  0x801fc6e4 counter=1 (fresh wait, not timed out).

OPEN: what does param_1=1 wait on / which result cell+value = "done" vs
"busy"(out byte0 bit2)? Candidates (subagent 6fc69eac): job-slot busy flag
(0x801FEF20=0x0a,0x801FEF30=0x0b), prev channel entry consumed state
(circular wait), or kernel event/semaphore the CD-DMA-done path should
signal. Each ReadS restart calls start_read_stream->
cdrom_clear_pending_dataready()+clear_sector_buffer() and rewinds read_msf
(observed 46:26:29<->46:27:61) — symptom amplifier; re-check vs Beetle
Read-while-reading once the wait predicate is identified.

NEXT: fully decompile cd_result_wait_b predicate + the fn-ptr table it
indexes by param_1; identify the "done" result the wait expects and which
upstream producer should set it.

=== UPDATE (session): CD consumer UNBLOCKED; new blocker = IRQ cross-thread resume poison @frame445 ===

cd_mc_state_machine (0x80199440) clean-C port could NOT reserve the guest
stack frame cd_result_wait_b needs: it passes guest_sp()-0x10 as param_2,
but with no sp-=0x40 prologue that lands inside the callee's own saved-reg
area (wait_b saves s6/s7 @sp+0x30/0x34); the 8-byte response copy then
collides and read-back sees stack garbage (bit4 -> status 3 livelock).
FIX: delegate whole body to generated 0x80199440 via br2_call_generated
thunk (same contract as cd_result_wait_b/mc_date_serial_cmp); gen_clean_table
now skips 0x80199440 (no wrap -> no recursion). Verified LIVE:
  - 0x801BCC88 status 3 -> 0; ring_dropped 12509 -> ~44 (sectors CONSUMED)
  - cd_channel_poll now returns 0 (channel entry reaches state==2)

Also fixed cd_response_pump callback buffer args: was 0x801ECBD0/0x801ECBC8,
authoritative disasm FUN_80197A00 shows 0x801FC6D0 (data-ready) /
0x801FC6C8 (ack) — off by 0x10000.

NEW BLOCKER (deterministic, frame 445): FAIL-FAST unknown dispatch
addr=0x04000600 phys=0x04000600 ra=0x80159AE0 a0=0x801FFFA8 a1=0x801FFFAC.
irqctx_ring tail: IRQ @epc=0x80159AD8 (the `jal cd_channel_poll` inside
generated mc_date_serial_cmp) exits same_thread=0 restored=0 -> cross-thread
switch; target thread resume produces pc=0x04000600 (unmapped low addr).
thread_ctx_ring EMPTY -> not a TCB save/restore path; this is the documented
"mid-nest switch -> poisoned context / smeared jumptable index" failure
(traps.c ~L365). Triggered only once CD streaming actually completes a
channel read — i.e. DOWNSTREAM of the SM fix, a separate latent IRQ/scheduler
bug, not caused by the delegation. mc_date_serial_cmp generated body has NO
indirect jump (only jal cd_channel_poll + jr ra), so 0x04000600 enters via
the IRQ-exit resume_pc, not game code.

NEXT: trace the IRQ-exit path for the cross-thread switch at 0x80159AD8 —
find which TCB/escape target supplies resume_pc=0x04000600 and why the saved
context is poisoned (mid-nest ChangeThread/RFE during clean<->generated CD
call). Candidates: g_sched_escape.target_tcb resume, dirty_safe_resume_pc
latch, or exception-handler $k0 table base.

=== UPDATE (session): ROOT localized — kernel-global memory corruption by mc_date_serial_cmp wild-pointer write ===

Narrowed hard via gwatch (guest write-watchpoint, captures store_pc):
- Crash reproduces identically under PSX_HLE_SCHEDULER=0 (host-fiber) AND
  PSX_BIOS_HLE=0 (LLE boot) -> NOT a host-scheduler bug; it is GUEST-side.
- thread_trace + thread_ctx_ring both EMPTY -> all thread switching is
  guest-kernel-driven (recompiled OpenBIOS ChangeThread/ReturnFromException);
  host only sees the resulting dispatch PC.
- OpenBIOS func_000029CC = guest ReturnFromException: k1=mem[mem[0x108]]
  (TCBH->current TCB), k1+=8, lw k0,128(k1)=TCB+0x88=returnPC, restores regs,
  jumps k0. 0x04000600 is literally the returnPC slot value.
- TCB layout VERIFIED correct for OpenBIOS too (returnPC @ Thread+0x88,
  globals @0x108 processes / 0x110 threads / 0x114 size). HLE offsets fine.

MEMORY-CORRUPTION SMOKING GUN (gwatch armed 0x80 len 0x280, frame 744):
- Physical 0x80-0x27F overwritten with repeating 0x000600fe/0x000400fe.
- These halfwords are EXACTLY the date/time fields mc_date_serial_cmp
  writes into its result struct (comment: "stores scaled halves into
  s2 @+0x1c/1e/24/26/32").
- store_pc = 0x80159C08 = `sh v1,50(s2)` inside br2eu_func_80159AB4
  (mc_date_serial_cmp). dma.depth=0 -> CPU store, not DMA.
- mc_date_serial_cmp: s2=a0=param_1, s1=a1=param_2 (move s2,a0 @0x80159ABC).
  Callers pass param_1 = struct+0x4700 -> 0x801F4700 (valid). At crash
  gpr[18]s2=0x801F4700 VALID, gpr[17]s1=0x801B3ED8 VALID.
- => a SEPARATE/Prior mc_date_serial_cmp ran with WILD param_1 (~0x4E),
  writing date fields into low RAM 0x80-0x27F, corrupting kernel globals
  incl. TCBH@0x108. Next ReturnFromException reads garbage TCBH -> bad TCB
  -> returnPC=0x04000600 -> FAIL-FAST.

UNIFIED ROOT CAUSE (hypothesis): the IRQ cross-thread switch at
0x80159AD8 resumes a thread whose saved context/registers are corrupted;
that thread runs memcard code with a wild pointer AND/OR its TCB returnPC
is garbage -> both the 0x80-region wild write and the 0x04000600 resume.
Still need: WHICH thread/context is corrupted and where its registers/
returnPC first went bad (upstream of mc_date_serial_cmp).

NEXT: catch the FIRST wild mc_date_serial_cmp call — arm a probe/watch on
param_1 (a0) into 0x80159AB4, or dump the full thread/TCB array
(dword_110 base / dword_114 size) at crash to find the never-initialized
or corrupted TCB. OpenBIOS does NOT zero Thread registers (unlike retail
InitThreads) -> uninitialized returnPC = heap garbage is a live suspect.

=== UPDATE (session): dispatch-depth + thread-array ruled out; residual = defer/pending-switch state ===

Thread array dump (pre-crash, globals still valid @0x108=0x8FD0/0x110=0x8FE0):
- ONLY thread0 @0x8FE0 active: flags=0x4000, returnPC=0x80159ae8 (VALID,
  inside mc_date_serial_cmp), sp=0x801fff90. thread1-3 free(0x1000),
  thread4 flags=0x6538 odd, rest zero. NO pre-poisoned TCB with
  returnPC=0x04000600 -> the bad returnPC is WRITTEN during the crash
  sequence (after globals@0x108 are corrupted), not pre-existing.

Marshaling audit (game/bind.c + clean_bridge.c):
- guest_at(addr)=guest_arena+(addr&0x1fffff) [weak]; guest_of(p) returns
  0x80000000|off for arena ptrs, 0x1f800000|off for scratch, ELSE 0.
  blk_io_retry(0x801f4700)->s1=guest_at->arena; mc_date_serial_cmp clean
  does br2_call_generated(0x80159ab4, guest_of(s1), guest_of(p2)).
  Round-trips correctly for arena addrs -> param_1 wild means the CALLER's
  a0 (blk_io_retry param_1, or generated caller's s2+0x4700) was already bad.
- br2_call_generated saves ra/pc/sp/s0-s3 only (NOT s4-s7 gpr[20-23], NOT
  full arg/temp). Probably not the IRQ bug (clean callers use host locals)
  but a real ABI gap worth noting.

Dispatch-depth audit (THE key mechanism):
- g_psx_dispatch_depth++ ONLY in OpenBIOS psx_dispatch_impl
  (OpenBIOS_dispatch.c:9015). Game dispatch br2eu_psx_dispatch_game_compiled
  + clean_try_dispatch do NOT touch it.
- br2_call_generated -> psx_dispatch_call -> OpenBIOS_psx_dispatch_call ->
  psx_dispatch_impl => depth++ DOES happen for game addrs routed via
  PSX_HAS_GAME_DISPATCH. So inside generated mc_date_serial_cmp depth>=1.
- IRQ handler psx_check_interrupts ZEROES g_psx_dispatch_depth (interrupts
  .c:1813) to run guest handler as outermost, restores saved at :1865.
- Thread-switch honor (interrupts.c:2100): at_outermost=(depth==0 &&
  call_unit==0). can_defer -> DEFER path (2144-2171): restores saved_gpr[],
  cpu->pc=g_exception_real_epc, sets s_defer_switch_{from,target,pending},
  resumes OUTGOING thread transparently; honored at next outermost boundary.
  !can_defer -> immediate longjmp PSX_RUN_YIELD_TO_TCB (poisoned-context
  path the comment warns about).

CONCLUSION: depth tracking is correct through the clean->generated boundary,
so at_outermost is FALSE inside mc_date_serial_cmp and the DEFER path SHOULD
engage. The residual bug is therefore in the DEFER/pending-switch state
machine: either (a) saved_gpr[]/g_exception_real_epc captured at IRQ-entry
is stale because generated code keeps live regs in C locals and only syncs
cpu->gpr[] at block boundaries (recompiler materialization gap), or (b) the
pending-switch honor at the next outermost boundary re-saves a context that
desynced. Faithful fix = guarantee cpu->gpr[] is fully materialized at every
interruptible point inside nested clean->generated dispatch, OR correct the
defer capture/restore. NOT a per-game hack; needs recompiler/runtime work.

RETAIL-BIOS NOTE: SCPH7502.bin present but is JAPANESE region; game is
SLES_017.22 EUROPEAN -> region mismatch + needs per-image Ghidra seeds that
don't exist. Not a quick sidestep; OpenBIOS kernel bug is the real target.

=== UPDATE (session): mc_date_serial_cmp EXONERATED — corruptor is un-instrumented writer ===

fntrace (armed 0x80159AB4+0x80159A14, captures a0-a3 per dispatch):
- ALL 130 mc_date_serial_cmp calls: a0=0x801F4700 VALID, a1=0x801B3ED8,
  ra=0x801264E0, sp=0x801FFF90, frame=445. NO wild param_1 exists.
- One call writes only ~5 halfwords @s2+{0x1c,1e,24,26,32} (~10 bytes) —
  CANNOT fill the 0x200-byte 0x80-0x27F region. mc_date_serial_cmp is
  INNOCENT.

disp_tail (always-on last-128 dispatch ring):
- Pure poll loop: cd_channel_poll(0x80199360) <-> 0x80159AE0 (mc_date_serial_cmp
  jal-return continuation) repeating, constant ra=0x80159AE0 sp=0x801FFF90.
- FINAL entry seq=...357: target=0x04000600 = the IRQ cross-thread resume
  dispatching a corrupted thread's resume-PC. The BIOS IRQ handler
  (OpenBIOS ChangeThread/RFE) runs between these game dispatches.

store_pc REINTERPRETATION:
- g_debug_last_store_pc is set ONLY by instrumented generated stores
  (`g_debug_last_store_pc = 0x...` lines). cpu->write_word/psx_write_word/
  clean br2_st*/guest_at-deref/memcpy do NOT set it -> reported
  store_pc=0x80159C08 was STALE (last instrumented store before the wild
  write), NOT the actual writer.
- => the 0x80-0x27F corruptor is an UN-INSTRUMENTED writer: clean C code
  (br2_st*/guest_at pointer deref) or memcpy/memset writing a repeating
  0x000600FE/0x000400FE pattern to a bad low guest address. It bypasses
  per-store instrumentation but still hits psx_write/watchpoints.
- DMA ruled out: ch1(CD)->0x801F470C valid+active; ch3/ch4 low-madr but
  chcr=0 inactive. No active low-memory DMA.

NEXT (concrete): catch the REAL writer. Options: (a) arm gwatch and read
  the WRITE VALUE + correlate seq/frame with disp_tail/fntrace to ID the
  executing function; (b) since clean-C guest_at-deref bypasses psx_write,
  instrument br2_st*/guest_at write path OR add a RAM-write hook that
  records the CALLING clean function (g_clean_trace_ra already logs clean
  dispatch ra — correlate the wild-write frame with the last clean dispatch).
  The repeating pattern (date/dir-entry-like) points at memcard directory/
  file-metadata code in save.c (mc_data_write/mc_data_read/cd_dir_entry_probe
  family) writing to a bad computed guest pointer.

=== UPDATE (session): memory-corruption theory WEAKENED — gwatch caught ZERO writes ===

- Re-armed gwatch addr=0x80 len=512 (covers 0x80-0x27F) on a fresh HLE run,
  ran to the SAME frame-445 0x04000600 crash: gwatch total=0, ZERO watched
  writes. Yet post-crash read_ram shows 0x100-0x13C uniformly
  0xFE000600/0xFE000400.
- g_psx_ram == memory_get_ram_ptr() == same `ram[]` buffer (memory.c:43,102,
  120) -> ONE RAM image; guest_at derefs, psx_write_*, generated stores all
  hit the same mprotect-protected pages. A real write to 0x80-0x27F SHOULD
  fault. total=0 => either the 0xFE0006xx pattern is LEGITIMATE kernel/heap
  data (not corruption), or the writer bypasses the protected pages
  (bulk memcpy already-unprotected window, or a non-ram mapping).
- irqctx seq 19797 (fatal): CD IRQ take_pc=real_epc=0x80159AD8, then
  exit_pc=0, exit_reason=NONE(0), same_thread=0, restored=0, ALL saved
  v0/v1/ra=0 -> the guest handler did a cross-thread ChangeThread->RFE that
  longjmp'd via sched_escape (did NOT populate exit fields / did NOT restore
  same-thread). Prior IRQs 19792-1966 were same_thread=1 restored=1 normal.
- REVISED LEADING HYPOTHESIS: the crash is the IRQ cross-thread resume
  reading a bad/uninitialized TCB returnPC (subagent: OpenBIOS does NOT zero
  Thread registers -> uninitialized returnPC = heap garbage 0x04000600),
  possibly because 0x108 (TCBH) holds a non-pointer value 0xFE000600 at this
  kernel state so mem[mem[0x108]] derefs garbage. The 0xFE0006xx region is
  likely legitimate data, NOT corruption -> the whole "wild write" theory
  may be a red herring. Need: resolve WHICH TCB/RFE path yields
  returnPC=0x04000600 and whether 0x108 legitimately holds 0xFE000600
  (kernel globals relocated?) vs a real bad write gwatch can't see.

=== UPDATE (session): CORRUPTOR IDENTIFIED — cd_bitstream_decode stale output ptr ===

The "IRQ kernel corruption" theory is RESOLVED — the wild writer is the clean
C tile-stream decoder, NOT generated MIPS and NOT an IRQ/context bug.

MECHANISM (all live-verified via gwatch host-RIP + wtrace + generated disasm):
- guest_watch (mprotect page watch) DID catch the writer this run: at frame
  744/445 a sequential fill hits phys 0x100-0x13E via TWO host RIPs
  0x101F713A0 / 0x101F7128C (276B apart = the copy loop's store sites).
  atos -l resolved them to **cd_bitstream_decode +628 / +352** (game/cd.c).
- cd_bitstream_decode's L_1f8 FLUSH loop (cd.c:3193) writes 0xfe00 halfwords
  from a1g up to limit CDREG32(0x801b6928). Its OUTPUT pointer a1g derives
  from arg a1.
- Caller chain: blk_io_retry(0x801f4700) [clean save.c:2172, faithful vs
  generated 0x80159A14] -> cd_bitstream_decode(s0, *(u32*)(s1+idx*4), vl).
  s1=guest_at(0x801f4700); idx toggles on s1+8; a1 = param_1[idx] = the
  decode output buffer pointer.
- Live context read: 0x801f4700(param_1[0])=0x0, 0x801f4704(param_1[1])=
  0x2000000, 0x801f4708(idx)=1 -> idx=0 -> a1 = param_1[0] = **0**.
- a1=0 -> decoder writes decoded output + 0xfe00 flush-pads starting at
  guest phys 0 -> floods 0x100-0x13E (kernel globals incl TCBH@0x108) ->
  IRQ ReturnFromException reads mem[0x108]=garbage -> returnPC=0x04000600.

ROOT CAUSE (buffer ptr stale): param_1[0]/[1] should be 0x80020000/0x80048000
(set by tile_stream_job_init menu.c:2083-2086 from _DAT_801f4744/48, which
scene_entry game.c:1554-55 sets = param_1+0x10000/+0x38000). The copy is gated
by DAT_801b3f64==1 ("buffers changed" flag). wtrace frames176-445: flag NEVER
written, buffer slots NEVER written -> refresh never ran for this context ->
stale 0/0x2000000 used.

OPEN: why DAT_801b3f64 is never set (buffer-reinit trigger). Not set by clean
scene_entry NOR generated scene_entry(0x80159028). Set by decode-job completion
/ buffer-change detector still to be located. Downstream of CD-stream decode
state — connects to the CD XA/Mode2 delivery work.

EXONERATED this session: mc_date_serial_cmp (fntrace all a0=0x801F4700 valid,
~10B/call), guest_of/guest_at marshaling, dispatch-depth/defer-switch,
host scheduler (HLE+fibers), BIOS HLE/LLE. store_pc=0x80159C08 was STALE
(g_debug_last_store_pc only set by instrumented generated stores; clean-C
guest_at derefs bypass it).

=== UPDATE (session): FULL ROOT CAUSE — decode-buffer lifecycle, flag never re-armed ===

DEFINITIVE causal chain (all live-verified):
1. cd_bitstream_decode (clean, game/cd.c) L_1f8 flush writes 0xfe00 halfwords
   to output ptr a1g. Its a1 = *(u32*)(guest_at(0x801f4700)+idx*4) = param_1[idx]
   (blk_io_retry save.c:2172, faithful vs generated 0x80159A14).
2. param_1[0]/[1] @ 0x801f4700/04 should be 0x80020000/0x80048000 — set ONLY by
   tile_stream_job_init (menu.c:2083-86) inside `if (DAT_801b3f64==1)` from
   _DAT_801f4744/48 (scene_entry game.c:1554-55 = param_1+0x10000/+0x38000).
3. Generated tile_stream_job_init (0x80159734) CONFIRMS clean port faithful:
   param_1[0]/[1] written only in block_80159754 (flag==1 path); flag cleared
   to 0 in same block. No other generated code writes param_1[0]/[1].

LIVE TIMELINE (gwatch host-RIP + wtrace + read_ram):
- 0x801B3F64 is in game .data; FILE value = 0x1 -> loads correctly (verified
  neighbors 0x801b3f70=0x80120eac, 0x801b3f5c=0x140 match). Flag=1 at boot.
- First tile_stream_job_init consumes flag (1->0), sets param_1[0]/[1] valid.
- frame ~170 (HLE): memset (guest 0x80126188 word-clear loop) clears the WHOLE
  decode context 0x801f4700+ -> param_1[0]/[1]=0. gwatch: 4 writes same RIP.
- After memset, flag==0 -> subsequent tile_stream_job_init SKIPS buffer
  refresh -> param_1[0]/[1] stay 0/0x2000000 forever.
- frame ~445: blk_io_retry -> cd_bitstream_decode(a1=param_1[idx]=0) -> writes
  decoded output + 0xfe00 pads to guest phys 0 -> floods 0x100-0x13E ->
  kernel globals incl TCBH@0x108 destroyed -> IRQ ReturnFromException derefs
  garbage -> returnPC=0x04000600 FAIL-FAST.

EXHAUSTIVE NEGATIVE RESULTS:
- NO code (generated full_*.c OR clean game/*.c) writes 1 to 0x801B3F64 after
  .data init. Searched all offset/base forms (16228/0x3F64, neighbors, byte/
  half). scene_entry sets _DAT_801f4744/48 but NOT the flag.
- Therefore on real HW the flag is ALSO consumed once and stays 0. The design
  intent: param_1[0]/[1] are set ONCE and must PERSIST across decode jobs.

OPEN ROOT-CAUSE QUESTION (the actual bug): the memset that clears the decode
context runs and wipes param_1[0]/[1], but they are never restored. Either
(a) the memset clears a context/lifecycle point where real HW would NOT clear
    param_1[0]/[1] (our memset range/timing diverges), or
(b) a buffer-rearm path exists that our generated/clean code doesn't reach
    (decode-job-done / buffer-free / overlay), or
(c) param_1[0]/[1] are meant to be re-derived each job via a path other than
    the flag (e.g. tile_stream_open/cd_stream_mode_init).
NEXT: identify the memset caller (function resetting the decode context) and
the decode-job-done path; determine whether param_1[0]/[1] should persist or
be re-armed. This is a decode-job lifecycle bug, NOT an IRQ bug.

=== UPDATE (session): decode-buffer ROOT CAUSE RESOLVED (uint8_t bind) + new XA-audio livelock ===

RESOLVED the "OPEN ROOT-CAUSE QUESTION" above — it was NOT a flag/lifecycle
bug. The buffer pointers ARE re-derived each job; the SOURCE globals were
simply mistyped. Committed bd682f0 (local; push 403-blocked):

- game/menu.c: _DAT_801f4744/48/4c/50 are 32-bit guest buffer POINTERS
  consumed by tile_stream_job_init as u32, but the binds were uint8_t ->
  job_init read only the low byte (0x80020000 & 0xFF = 0) -> param_1[0..4]=0
  -> cd_bitstream_decode flooded phys 0 -> kernel globals clobbered ->
  ReturnFromException returnPC=0x04000600 FAIL-FAST @frame744.
  Generated code uses lw (full word). Fixed binds to uint32_t.
- game/cd.c: cd_file_open_b param_1 is a GUEST address (wrapper passes
  cpu->gpr[4]); it was cast straight to host ptr -> SIGSEGV @frame481.
  Now guest_at(param_1) before cd_file_open.
- game/game.c: scene_entry passed local_38 (host ptr) where guest ABI
  wants guest addr -> now guest_of(local_38); fixed stale extern decl.

RESULT: game runs past frame 744 -> frame ~29000+. Decode buffers populate
correctly (0x80020000/0x82048000) and CONTAIN real tile data (0x80048000
has 0x8008/0xfe00 VRAM tile patterns) -> earlier decode worked. Channel
entry[0] cycled state 2->consumed->4.

NEW BLOCKER (frame ~29000, demo/attract load): scene_entry(0x80010000,0,1)
= demo fight (param_3=1). blk_io_retry livelock:
- fntrace: cd_channel_poll(0x80199360) called 548M times from generated
  mc_date_serial_cmp (ra=0x80159AE0); cd_dir_entry_finalize NEVER fires.
- Channel table @0x80010000 (base 0x801fc6c0, idx 0x801fc6ac=0): states
  frozen [4,3,3,3,3]; poll needs state==2, sees 4/3 -> returns 1 forever.
- blk_io_retry -> mc_date_serial_cmp returns 0 -> -1 -> scene_entry
  re-opens file (cd_file_open_b) -> seek+mode+read to SAME MSF[16,22,6]
  every ~35 frames -> infinite loop. gp0_draw frozen 597, GPU disabled.
- ROOT: the file blk_io_retry opened (stamp 0x00292646 @0x801b5610,
  param_2=0) streams PURE CD-XA audio: last 64 sectors ALL xa_file=1,
  submode=0x64 (AUDIO|REALTIME), xa_audio_delivered=1, data_delivered=0.
  maybe_deliver_xa_audio routes them to SPU (mode_reg=0xE0 has bit6=0x40
  XA-audio-enable SET). Audio sectors do NOT set_irq -> no DATA_READY INT1
  -> request_reg(BFRD)=0 -> sector_available=0 -> channel state stuck=3.
  irq_flag always 0x00, i_stat bit2 (CDROM IRQ) never asserts.

CONFIRMED WRONG-FILE (psx-spx authoritative): per psx-spx cdc, with
CdlModeRT(Setmode bit6)/CdlModeSF set, "INT1 comes only for the data
sectors" — XA audio sectors route to the ADPCM decoder->SPU with NO INT1.
Our cdrom.c model (maybe_deliver_xa_audio -> data_delivered=0 ->
deliver_read_sector returns 0 -> no set_irq) is FAITHFUL. So real HW also
gets zero INT1 reading this region -> real HW does NOT read this file for
blk_io_retry -> the file/sector resolution is WRONG.

DISC LAYOUT (verified by reading Game.bin raw, MODE2/2352, LBA0=off0):
- LBA ~73550..~166000: pure XA audio bank, ALL sectors (file=1,ch=0,
  submode=0x64 AUDIO|REALTIME, coding=0x01 ADPCM, payload=0 silence).
  73000-74500 window: 1500/1500 identical audio, ZERO data substreams.
- LBA ~166000-170000: transition (file=1,ch=1,submode=0x48).
- LBA ~170000..~235441(end): MODE1 (mode byte 0) DATA sectors.
blk_io_retry seeks MSF[16,22,6]=LBA~73505, DEEP inside the audio bank,
~100k sectors from the data region. File record @0x801b5610 (rec8 of the
0x801b5b94 ptr table, selected by scene_entry index (param_2+8)*2=16 for
param_2=0) holds stamp 0x00292646 + size 0x1d50; cd_cmd_issue (generated
0x80196e14 via br2_call_generated) resolves it to LBA 73505=audio.
0x801feb00=0x16220502 is STALE (set by cd_dir_entry_store/finalize which
never fire for audio reads; last value from entry[0]'s earlier data read).

NEXT: pin down whether the wrong link is (a) scene_entry's record
selection index -> wrong stamp 0x00292646, or (b) cd_cmd_issue's
stamp->sector directory lookup landing on audio. Compare generated
scene_entry table-index math vs clean (param_2+8)*2; disasm cd_cmd_issue
file-stamp->LBA path (needs Ghidra up or generated-block trace).

=== REFINEMENT (frame ~55000, deeper disc scan) ===
NOT pure audio after all. Full forward scan from LBA 73505 finds SPARSE
DATA sectors (mode2, file=1,ch=0, submode=0x0 = no AUDIO|REALTIME bit ->
data_delivered path) at: 76599,76600, 119275-119280, 135975-76,
155047-48, 165891-96, then file=0/ch=1 submode=0x48/0x89 streams ~165897.
So file 0x00292646 is a MIXED XA stream: long audio runs punctuated by
rare data sectors (~every 30k sectors). First data sector = LBA 76599,
~3100 sectors ahead of the 73505 start.

Channel entries @0x80010000 (stride 0x20, data @idx*0x7e0) decode as 5
CONSECUTIVE single-sector reads: stamp@+0x1c = BCD-MSF -> e0=16:22:05
LBA73505 (state4=consumed via cd_channel_poll 2->4), e1-e4=73506-73509
(state3 stuck). 0x801feb00 = e0's stamp (LE bytes 16 22 05 02 -> u32
0x02052216; cd_stamp_to_sector -> LBA 73505). NOTE: earlier "0x16220502"
was an endianness misread; real u32 = 0x02052216.

Generated-vs-clean VERIFIED faithful: func_80159028 passes sp+0x28
(&local_38) to cd_file_open_b(0x8019917C) AND cd_stream_mode_init
(0x80159CE4) identically; local_38[0]=*puVar6 is a single swl/swr word
store in BOTH. File stamp 0x00292646 is STATIC EXE .data (SLES_017.22
file off 0x97610 -> guest 0x801b5610), not runtime-corrupted.

REVISED MODEL: blk_io_retry polls channel entries for state==2 (set by
cd_dir_entry_store/finalize on DATA_READY). Audio sectors give no INT1
(psx-spx confirmed faithful). The read RESETS to ~73506 each blk_io_retry
timeout (read_msf oscillates 73518-73580, never reaches 76599). DEADLOCK:
need a data-sector INT1 to advance 0x801feb00/resume pos, but re-seek
resets before reaching the sparse data at 76599.

TWO HYPOTHESES remain:
(A) TIMING: blk_io_retry's 2000x mc_date_serial_cmp budget elapses (guest
    cycles) before the CD streams ~3100 sectors to first data. If our
    guest-cycle/CD-delivery rate ratio is off, real HW reaches it. Check
    sector-delivery rate vs poll cycle cost.
(B) WRONG FILE: 0x00292646 is an audio bank; blk_io_retry should read a
    different (pure-data) file record. rec8 may be the demo's AUDIO, not
    its tile data.
NEXT: measure CD sector-delivery rate vs blk_io_retry poll budget; if
sound, the file record selection is wrong -> find correct record for
scene-0 tile data.

=== CHANNEL-POLL MECHANISM (cd.c:1911, disasm 0x80199360) ===
cd_channel_poll reads entry[idx@0x801fc6ac] (0x20 stride @base
0x801fc6c0->0x80010000). state==2 -> consume to 4, return data ptr; else
return 1 (still polling). Live: idx=0 -> entry[0] state=4 (CONSUMED/dead,
can never reach 2) -> returns 1 forever -> mc_date_serial_cmp 0 ->
blk_io_retry 2000x -> -1 -> cd_file_open_b re-seek 0x801feb00+1.

DECISIVE: the 5 armed channel entries (sectors 73505-73509) are ALL XA
audio (submode 0x64). NONE can reach state==2 because audio gives no
DATA_READY INT1 (psx-spx-faithful). So the game armed DATA-channel
entries for an AUDIO file and waits for completions that can never come.
This is NOT a timing/budget problem — even infinite time won't complete
an audio sector's channel entry. It is a WRONG-FILE / wrong-sector
resolution: blk_io_retry should be reading tile DATA, not this audio
bank. The file record 0x801b5610/stamp 0x00292646 is static+faithful, so
the divergence is in HOW the stamp resolves to LBA 73505 (cd_cmd_issue
generated 0x80196e14 directory lookup) OR which record scene_entry
should select for scene-0 tile data.

NEXT: disasm cd_cmd_issue (0x80196e14) file-stamp->LBA directory lookup;
determine if 0x00292646 SHOULD map to a data region (~LBA 170000 Mode1)
but lands on audio 73505, OR if rec8 is simply the wrong record for
scene-0 tiles (find the correct record). Beetle oracle on same ISO =
definitive ground truth for which sector/file the game reads here.

=== POLL-BUDGET CONSTANTS (generated, faithful) ===
blk_io_retry (0x80159a14): outer s2=2000, calls mc_date_serial_cmp.
mc_date_serial_cmp (br2eu_func_80159AB4, full_21.c): inner s0=2000
(`li s0,2000` @0x80159ACC), loops jal cd_channel_poll(0x80199360);
returns handle on poll==ready else 0 after 2000 inner iters.
=> up to 2000*2000 = 4,000,000 cd_channel_poll calls per blk_io_retry
   before it returns -1 and scene_entry re-seeks.
CD rate: CDROM_SINGLE_SPEED_SECTOR_CYCLES=451584; at Setmode bit7 2x ->
~225792 cyc/sector ~= 150 sectors/sec @33.87MHz (faithful).
First data sector after LBA73505 start = 76599 -> ~3100 sectors ->
~700M guest-cycles needed in ONE blk_io_retry budget to reach it.
Observed: read_msf trapped 73511-73601 (~90-sector window), no ratchet.
OPEN: does 4M poll-iters span >=700M guest-cycles? Depends on per-poll
psx_cyc_step cost + whether psx_check_interrupts_at advances to CD
delivery deadlines. If budget < needed -> faithful-core CYCLE bug
(poll loop under-counts cycles vs real HW) OR CD delivery too slow.
Measure: guest-cycle delta across one full blk_io_retry -1 return.

=== CALLER CORRECTION (frame ~106212) ===
fntrace shows mc_date_serial_cmp(0x80159AB4) is called from
ra=0x801264E0 (jal ~0x801264D8, ret->branch-check) — NOT from
blk_io_retry (0x80159A14 has ZERO fntrace entries). a0=0x801F4700.
Caller func = br2eu_func_801264B4 (full_00.c), which ALSO calls
scene_entry(0x80159028) and 0x80156048. => scene_entry/blk_io_retry
ALREADY RETURNED; the livelock is func_801264B4's OWN mc_date_serial_cmp
wait loop polling a CD channel that never reaches state==2. The channel
entries (73505-73509, all XA audio) belong to THIS wait — likely an
audio/video stream the driver waits on, not the tile loader. Re-frame:
this may be a legitimate "wait for stream data sector" that can't
complete because the armed entries are audio-only, OR the wrong channel
cursor. Need: what func_801264B4 waits for (its channel/file context)
and whether the armed entries should be data not audio.

=== MDEC EVIDENCE — likely VIDEO playback, not tile load ===
mdec_state: busy=1, input_count=63170 < expected_halfwords=99716,
decode_macroblocks=0, decode_blocks=0, enable_dma_in/out=1.
command=0x2AC2C2C2 (suspicious/possibly mis-set). => the game is feeding
an MDEC video decode and STARVED for bitstream — consistent with an
FMV/cutscene (BR2 attract-mode video) rather than scene tile data.
The XA file 0x00292646->73505 is likely a VIDEO+audio stream; blk_io_retry
feeds MDEC video sectors. Audio ch=0 fills the read; video data (the
sparse submode=0x0 sectors, possibly ch=1) starves -> channel never
completes -> livelock. NEXT: determine which XA channel/submode carries
the MDEC video payload and whether blk_io_retry should read THAT
substream (ch=1?) not ch=0 audio. Check sector_history xa_channel
distribution + MDEC input source (which decode buffer/sector feeds it).

=== EMPTY-REGION FINDING (decisive disc scan) ===
Direct Game.bin scan (single MODE2/2352 track, LBA0=offset0, verified vs
real data at LBA216335 "SURVIVAL RANKING"):
- LBA 73500-170000 (~95k sectors, ~21min): EVERY sector has valid XA
  subheader (file=1,ch=0,submode=0x64) but ZERO 2328-byte payload.
  audio_stats cd_in tap: 981M frames pushed, nonzero=0 (all silence -
  CORRECT, payload really is zeros on disc).
- Sparse submode=0x00 data markers inside: 76599-76600, 119275-119280,
  ~135975, ~155047, ~165891 (~8 total, ~35000 apart - too sparse for
  blk_io_retry ~46-sector budget).
- Real content: LBA 0-73500 and 170000-235396 (Mode1 data, e.g.
  216335+ has ASCII strings).
=> file 0x00292646 -> LBA73505 lands at the START of a 21-minute EMPTY
   XA-payload gap. blk_io_retry reads empty sectors via INT1 channel
   entries; only ~8 data markers exist, none reachable in budget.
   mode_reg=0xE0 (game-set CdlModeRT) -> audio->ADPCM->no INT1 (faithful
   per psx-spx). cd_dir_entry_finalize/store are INT1/response-driven
   (cd_response_pump->callbacks->cd_mc_state_machine). Clean port ==
   generated (scene_entry index (param_2+8)<<3 -> rec8/0x801b5610 verified
   instruction-level). cd_read_log shows game DOES read real data at
   LBA216335 successfully => data path works; 73505 stream is separate.
OPEN: is 73505 a WRONG resolution (should point to real data ~216335 or
   another region) or a legit mostly-empty stream file? All resolution
   code is faithful, so suspect upstream: file-record->stamp->sector
   directory lookup in generated cd_cmd_issue (0x80196e14) OR the file
   ID 0x00292646 encodes a directory entry that resolves wrong.
   NEED: ground-truth which LBA real HW reads for this file (oracle is
   Windows-only DuckStation - not buildable on macOS) OR trace
   cd_cmd_issue's directory lookup.

=== CONCLUSIVE: ALL XA AUDIO EMPTY ON DISC ===
Random-sample scan (3000 sectors): 2003 XA-audio sectors (submode&0x60)
ALL have zero payload; only 166/997 Mode1-data sectors have payload.
Full-sector hex of LBA80000: only bytes0-31 nonzero (sync+hdr+subhdr),
bytes32-2351 all zero. => The disc's ENTIRE XA-audio layer is silent.
Either (a) dump missing audio payloads, or (b) BR2 stores audio as Mode1
data and the XA region is unused reserved space. cd_read_log shows game
DOES read real data at LBA~315-9542 and ~208829-216335 (Mode1) - so data
path works; the stuck stream at 73505 is separate.
scene_entry IS the stage/tile loader (game.c:1531) - needs real data, not
an empty XA stream. File 0x00292646->73505 is faithful end-to-end
(clean==gen, static .data, cd_cmd_issue faithful). File ID not duplicated
in a RAM dir table (scanned 0x801a0000-0x801d0000: only record site).
HINTSTS BFWRDY/BFEMPT (bits3,4) return irq_flag|0xE0 -> always ~0; runtime
bypasses ADPCM buffer via spu_cd_audio_push direct-to-SPU. If game polls
BFWRDY for XA flow-control this is a real modeling gap - but blk_io_retry
uses INT1 channel entries (data path) not HINTSTS.
BLOCKER: cannot get ground truth - DuckStation oracle is Windows-only
(VS2022/MSBuild), no macOS PSX emulator installed, Ghidra MCP down.
NEXT concrete: (1) user verify Game.bin dump integrity / provide known-
good dump or emulator; (2) OR build file-ID->sector directory parser by
finding the on-disc directory the boot sequence reads (early LBAs).

=== DUMP DEFECT: Game.bin XA/Form2 payloads ALL ZERO ===
Verified by direct Game.bin reads (lba*2352): every XA sector in the
Mode2/Form2 regions (73500-170000, incl file-table idx2-5 @76601/119281/
135977/155049 and stuck pos 73506) has valid sync+header+subheader
(file=1,ch=0,submode=0x64,coding=0x01) but ZERO bytes nonzero in payload
offset 24-2351 (checked 400+ sectors/region x5). Mode1/Form1 data sectors
(216335,76600,208829) DO have payload -> dump captured Form1 fine, Form2
zeroed = classic Mode2/Form2 dump-tool bug. Game.bin is complete size
(235396*2352=528MB, single track MODE2/2352). => BR2 CANNOT have working
audio/video from this image; likely explains attract-mode stream stall.
Game seeks MSF 16:22:06 (=LBA73506, empty XA) via Setloc+Setmode(0xE0)+
ReadS, reads ~106 sectors, blk_io_retry times out (audio->no INT1), re-seeks.
File stamps ARE BCD-MSF (cd_stamp_to_sector): idx8->LBA208829 real data,
idx2-5->XA region. MSF16:22:06 is NOT a file-table start -> stream/offset pos.
NEXT: need known-good dump to verify; XA empty payload may itself cause
stall if game validates sector content. HINTSTS BFEMPT(bit3)/BFWRDY(bit4)
currently = irq_flag|0xE0 -> bits3,4 unmodeled (no ADPCM buf state).

=== SESSION: CD livelock — refined to position/channel conflict + dump defect ===
psx-spx confirms our XA->ADPCM routing is FAITHFUL: try_deliver_as_adpcm_sector
sends audio+realtime (submode bit2+bit6) sectors to decoder, no INT1, exactly as
cdrom.c does. Diagnostic maybe_deliver_xa_audio->0 (force data path) did NOT
fix blk_io_retry: channel entries still state3, poll cursor stuck @entry0
(state4), finalize cursor a8=5 already at end. => livelock is NOT just audio
routing; it is a channel-entry arm/finalize/poll desync + wrong position.
file_table.json (361 ISO9660 entries) + cd/ extraction: REAL game assets live
at LOW LBAs and are INTACT — stages ST* 468-1313, characters (ALI/BAK/BUS/GAD/
JEN/LON/MAR/RON/STU/ULI/YUG) 4387-1313, CMN 9100-11064, MOV 249190-317741.
cd/XA/*.XA extracted files are ALSO all-zero (extracted from same Game.bin) =>
dump defect confirmed for XA/MOV/Form2 payload. KEY: stuck read MSF16:22:05
(LBA73505) lands INSIDE XA/BR2_MC02.XA (lba31055 size46896 -> ~77951), i.e. a
DATA bitstream reader (blk_io_retry->cd_bitstream_decode) is positioned inside
a MUSIC file. Two CD channel systems coexist: 0x801fc6xx (blk_io_retry:
poll/finalize/probe) vs 0x801bc6xx (cd_dir_entry_store). Both write resume
stamp 0x801feb00. Suspect concurrent music-XA + data-stream multiplexing
conflict OR resume-stamp cross-talk pulling the data read into the music
region. Game assets intact => gameplay MAY be reachable if stream conflict
fixed (music would be silent but fighters/stages have data). OPEN: trace which
file/stream arms the 0x801fc6xx entries @16:22:05-09 and why position resolves
into BR2_MC02; determine if music stream (bc6) and data stream (fc6) share
0x801feb00 incorrectly.

=== SESSION2: submode proves upstream divergence, not just dump ===
Sector-format scan: game DATA uses submode 0x48/0x08 (data bit -> INT1) with
real payload (file8@208829 nz~1779, assets@5000/1000 nz~2200). Music/XA uses
submode 0x64 (audio+realtime) -> ADPCM, and payload is EMPTY (dump defect).
file_table.json: 73500-170000 region = ONLY XA music (BR2_MC02 tail + MC03);
NO data files. All 286 game-asset files at LBA<73500 intact.
DEDUCTION: submode0x64 is structural (header bytes preserved by dump), so real
HW also gets no INT1 there -> real game NEVER runs blk_io_retry @73505. Our
reimpl diverged UPSTREAM leaving stale CD/stream state that mis-positions the
data read into the music region. 0x801feb00 (shared resume stamp, written by
BOTH cd_dir_entry_finalize fc6 AND cd_dir_entry_store bc6) is the suspect
cross-talk point: a music/other-stream finalize may have poisoned the data
read's resume position. Dump defect remains REAL but SEPARATE (kills only
music/video/ADPCM content; game assets intact => core gameplay data present).
NEXT: trace which stream writes 0x801feb00=16:22:05 and whether bc6
(cd_dir_entry_store) is an active concurrent stream vs idle; find the
upstream divergence that armed fc6 entries @music position.

=== SESSION3: MECHANISM ISOLATED — stale 0x801feb00 poisons file-open ===
cd_file_open_b(local_38): v0=cd_stamp_to_sector(0x801feb00); then
cd_file_open(v0+1, local_38) OVERWRITES local_38 with the BCD-MSF of the
RESUME position (not the file stamp). scene_entry then calls
cd_stream_mode_init(local_38) -> Setloc uses the now-overwritten local_38.
=> A stale/wrong 0x801feb00 directly corrupts the Setloc target.
LIVE: 0x801feb00=0x02052216 (MSF16:22:05, music) STATIC; 0x801fcb00 (seq
path resume) all-zero => single active stream. fc6 entries armed 16:22:05-09.
0x801feb00 written ONLY by cd_dir_entry_finalize(0x801990f0,fc6) and
cd_dir_entry_store(0x80199100,bc6) from entry+0x1c stamp.
HYPOTHESIS: music/XA stream (bc6) or a prior read left 0x801feb00=73505
(music pos) before this blk_io_retry session; cd_file_open_b then resumes
the DATA read there. On real HW 0x801feb00 would hold the correct bitstream
position. ROOT = whatever first wrote the wrong stamp (upstream divergence
in channel arm/finalize or a missing per-file-open resume reset).
NEXT: instrument/watch 0x801feb00 writes (clean-C fns, can log via debug
ring) on a fresh run to catch FIRST bad write + its PC; OR read generated
cd_mc_state_machine(0x80199440) arm path to see stamp source.

=== SESSION4: arming-position vs command-position DISCONNECT isolated ===
Instrumented all three position chokepoints (cd_file_open_b tag3,
cd_stream_mode_init tag4, cd_mode_flag_set/cd_cmd_issue tag5) + feb00
writers (finalize tag1, bc6-store tag2). Fresh-run seek trace:
  tag4 00292646->208829   (file8 open, correct)
  tag5 Setloc 208829      (command position CORRECT)
  tag3 02052216->73505    (file_open_b reads 0x801feb00 ALREADY=16:22:05)
  tag4 00062216->73506    (stream_mode_init seeks music)
  ... livelock
=> By the FIRST file_open_b, 0x801feb00 was already 73505. Between the
   correct Setloc(208829) and that read, cd_dir_entry_finalize wrote
   0x801feb00=entry+0x1c=0x02052216. So a channel entry ARMED AT 73505
   completed (INT1) during the 208829 data read.
=> DISCONNECT: cd_mc_state_machine armed fc6 entries @16:22:05-09 while
   the issued Setloc/read was @46:26:29 (208829). The entry-stamp source
   is NOT the command position — it is read from the 0x801febxx/0x801Cxxxx
   position state (generated code loads it onto stack sp+40 then lwl/lwr
   -> entry+0x1c). THIS is the upstream divergence to fix.
Confirmed SINGLE stream: cdrom_command_history shows ONLY Setloc
16:22:06->Setmode0->SetmodeE0->ReadS retries (pc 0x80196FC4); no music/
XA file open interleaved. bc6 store (tag2) NEVER fired -> no concurrent
music stream via that table. feb00 written exactly ONCE (tag1).
Live state: CD free-runs fwd thru music region (read_msf 16:22:47, lba
73546), mode 0xE0 realtime -> audio sectors to ADPCM, no INT1 -> armed
data entries never reach state2 -> blk_io_retry livelock.
cd_file_open_b NEVER resets 0x801feb00 (pure resume); scene_entry bounds
check uses 0x801feb04 descriptor (=1, in-range) so stale 73505 stamp is
accepted and local_38 overwritten -> Setloc 73506 forever.
ROOT CAUSE (narrowed): whatever feeds cd_mc_state_machine's entry-stamp
position yields 16:22:05 instead of the 208829 read position. Candidates:
(a) a stale/queued read-request position populated before file8 open,
(b) Getloc/status position our runtime reports differently than the
game's Setloc+count expectation, (c) bitstream block->sector map entry.
NEXT: trace the position value inside generated cd_mc_state_machine —
find the guest load that produces 0x02052216 (it is copied to sp+40 then
lwl/lwr'd to entry+0x1c @0x8019970C). Identify the source struct offset
(0x801feb18 / 0x801CCC48 / read-request queue) and which earlier write
put 73505 there. Compare against a position the game computed for the
208829 stream.

=== SESSION5: XA channel interleave — game data FULLY intact; only ch0 audio empty ===
Sector subheader decode (file,ch,submode,coding @+16):
  ch0 submode0x64 = XA AUDIO -> routed ADPCM, payload EMPTY (dump defect)
  ch1 submode0x48 = DATA -> INT1, payload nz~1779 INTACT
CRITICAL CORRECTION: the disc interleaves channels per-LBA. "High-LBA
region" is NOT pure music — ch1 DATA files live at 165898/176069/194654/
208829 (all nz1779). ONLY ch0 audio (music BR2_MC0x + the 73000-105000
pure-audio span) has empty payload. => ALL game data present; dump defect
kills ONLY music/video/ADPCM, NOT gameplay data.
Game file table (0x801b5600+) decoded: stage uses 4 ch1-data files
@208829,165898,176069,194654 + many low-LBA char/stage files. NONE ->73505.
Channel entry layout (0x20 stride @0x80010000): +0x00 u16 state
(4=consumed,3=armed,2=done), +0x04=(batch<<16|seqidx), +0x1c=MSF stamp.
Livelock entries = ONE 5-sector read @MSF16:22:05-09 = LBA73505-09, which
is PURE ch0-audio (verified 73000-105000 = 1000% audio, 0 ch1-data) ->
no INT1 -> entries never complete -> blk_io_retry livelock.
73000-105000 has ZERO ch1-data sectors -> a data read there can NEVER
complete on real HW either => the position 73505 itself is the bug,
queued by the stream scheduler (cd_mc_state_machine read-request path).
ROOT (final narrowing): the block scheduler computes next-read sector
73505 instead of continuing the 208829 ch1-data stream. Source is in
generated cd_mc_state_machine / read-request queue — needs value-flow
trace of the sector-number -> cd_file_open(BCD) -> entry-stamp chain.
NEXT: find the guest function that produces sector number 73505 for the
read request (likely reads a block->sector map or increments a position
counter); compare vs correct continuation of the 208829 stream.

=== SESSION6: stamp source = CD data-FIFO reads (0x1F801802); arming is decoupled ===
Generated cd_mc_state_machine (0x80199440) fills stack buf sp+40 via a
byte-copy loop, then lwr's it toward entry+0x1c (the MSF stamp copied to
0x801feb00 by cd_dir_entry_finalize). The copy loop loads a register
address from table @0x801BCC48 (=0x1F801802, CD data FIFO) and reads
bytes into sp+40. => the "position stamp" is derived from CD register /
sector-payload reads, i.e. the game parses positions out of data it pulls
from the drive, OR sp+40 is a decode buffer and the real stamp source is
adjacent. Either way the value 0x02052216 (MSF16:22:05=LBA73505) enters
via the CD read path, not a clean software position counter.
Confirmed: NO raw sector 73505 / stamp 0x02052216 stored anywhere in
0x801e0000-0x80200000 except 0x801feb00 itself => position is transient
(computed per-arm inside generated code).
File ptr table 0x801b5b94 -> [16]=0x801b5610 asset dir: first 4 stamps =
stage files 208829/165898/176069/194654; ~120 more valid asset positions
(3000-200000). NONE = 73505 => position is computed, not table-looked-up.
cd_bitstream_decode = pure VLC, no position logic. blk_io_retry only
polls/decodes/frees; positions armed by cd_mc_state_machine read-ahead.
OPEN: exact guest load producing 0x02052216 inside 0x80199440 (sp+40
fill) — needs Ghidra decompile (server down) or Beetle oracle divergence,
or a write-watchpoint on entry+0x1c catching the FIRST arm PC+value.

=== SESSION7: ROOT CAUSE + FIX — whole-sector MSF header regenerated ===
RESOLVED the frame-29000 blk_io_retry livelock. The "position" the game
parses is the sector's own MSF address header, delivered in whole-sector /
Form2 mode (mode_reg & 0x20; BR2 uses mode 0xE0). In this mode the data FIFO
(RB_ ring, shared by programmed-I/O 0x1F801802 and CD DMA) delivers the raw
sector starting at WHOLE_SECTOR_OFFSET=12 = the 3-byte MSF address + mode +
subheader + user data (2340 bytes). The game reads the first 4 bytes
(MSF[3] + mode[1]) as a u32 -> the channel-entry stamp at entry+0x1c.
  stamp 0x02052216 = bytes 16 22 05 02 = BCD MSF 16:22:05 + mode 0x02.
The have_raw path memcpy'd the STORED header from Game.bin, but this dump
re-heads every high-LBA extent at MSF 16:22:05 (verified: file-offset LBAs
208829/165898/176069/194654 and 73505 ALL carry header MSF 16:22:05; low
LBAs 0/1/100 are correct). So every data file reported position LBA 73505
-> entries armed into a pure-XA-audio span -> no data INT1 -> livelock.
FIX (commit 507ab79): MSF==absolute-position is a hardware invariant on
every playable disc; the drive knows the position independent of stored
header bytes. The have_raw whole-sector path now regenerates wb->data[0..2]
= bin_to_bcd(min/sec/sect) — identical to the fallback path and to what a
correctly-mastered disc stores. Mode byte, subheader, user data stay
disc-authentic. General fix (all games/sectors), not per-game.
VERIFIED live: read_msf now tracks 46:xx (LBA ~208xxx stage data) instead
of sticking at 16:22:05; int1_lost=0; frame advancing past the old stall.
NOTE: dump still has wrong stored MSF headers + empty ch0 audio payload —
a re-dump would be cleaner, but the runtime now tolerates it faithfully.

=== SESSION7b: post-fix status — CD layer correct, consumer re-reads file8 head ===
After 507ab79 the CD layer is verified correct: entries arm at real
positions (46:26:29=LBA208829=file8), reach state 4, int1_lost=0, ra stays
in blk_io_retry/cd_channel_poll (0x80159AE0). BUT the game still loops:
last ~4000 delivered sectors ALL fall in LBA 208830-208936 (~106-sector
window at file8 head), ~100-120 sectors/sec, ring_dropped grows (~18k).
file8 payload is structured/valid (60 01 01 80 magic + incrementing
seq/offset fields per sector) — not obviously corrupt data.
INTERPRETATION: entries complete (data delivered) yet blk_io_retry keeps
re-requesting the same window => the bitstream/tile-stream CONSUMER is not
advancing its read offset. Either (a) chunk-offset/next-block fields in
the dump's file8 data are also corrupt (like the MSF headers were) so the
scheduler can't advance, or (b) a downstream decode/stream bug.
NEXT: determine whether file8's chunk directory/offsets are intact vs
dump-corrupt; compare read pattern vs Beetle oracle; inspect the
bitstream-decode consumed-offset cursor.

=== SESSION8: stall narrowed to entry-completion starvation ===
Restarted clean (turbo_loads off, instant_max_per_frame reset 32->1 live).
Re-read loop is REAL, not a turbo artifact — persists at authentic speed.

PRECISE CHARACTERIZATION (all live-verified via debug server):
- cmd loop every ~36 frames: Setloc(46:26:30=LBA208830) + SetMode(0) +
  SetMode(0xE0) + ReadS(0x1B) = blk_io_retry timeout -> cd_stream_mode_init
  re-seek -> re-read. read_msf oscillates 46:26:47-46:27:62 (~208840-2089xx).
- Channel entries @0x80010000 FROZEN: e0 state=4(consumed) stamp 46:26:29,
  e1-4 state=3(armed) stamps 46:26:30-33 (=LBA 208829-833). Cursor
  @0x801fc6ac=0 stuck on consumed e0; arm cursor @0x801fc6a8=5.
- cd_feb00_trace total=1: cd_dir_entry_finalize ran EXACTLY ONCE (e0 only).
  e1-4 never reach state 2 => cd_channel_poll never fires => blk_io_retry
  2000-poll timeout => -1 => re-seek. DEADLOCK: cursor on consumed e0.
- cd_read_log (CD DMA): all 5 entries' transfers logged lba=208829 —
  same last_sector_lba for every entry though stamps want 208829-833.
  Data bufs @0x80010400+idx*0x7e0 match cd_channel_poll's idx*0x7e0.
- ring_dropped grows ~100/sec even at instant_rate=1 => drops are a
  SYMPTOM: game stuck polling (not consuming FIFO) so ring overflows.
- irq_flag=0 (game acks CD IRQ), i_mask=0xd bit2(CD) enabled, int1_lost=0.
- cd_intc_dispatch clean-C counter @0x801bce00 = 0 (may be bypassed by
  generated path — inconclusive).

ROOT-CAUSE HYPOTHESIS: entries1-4 don't finalize because cd_mc_state_machine
(generated 0x80199440, driven per-INT1 by the deferred CD-DMA->DeliverEvent
->cd_ctx_restore->cd_intc_dispatch coroutine) is not iterating them. Either
(a) the coroutine starves (documented failure mode: clean-C spin lacks
br2_irq_poll/live guest PC -> coroutine never resumes), or (b) per-entry
sector data isn't matched (all DMAs logged same lba -> stale/duplicate
sector delivery vs distinct 208829-833).

NEXT: determine whether CD DMA delivers DISTINCT consecutive sectors per
entry or repeats 208829 (DMA gating / delivery-ahead-of-demand bug), and
whether the deferred coroutine resumes per-INT1. Needs Ghidra decompile of
0x80199440/0x8019a8ec (no JDK locally) or Beetle oracle divergence, or a
watchpoint on cd_dir_entry_finalize entry + cd_mc_state_machine invocation.

=== SESSION8b: SECOND root cause — cd_dir_entry_probe address-space mix ===
FIXED (commit 03b69e3). cd_dir_entry_probe mixed guest/host address
spaces: param_1 is a guest KSEG0 addr (entry data buffer, e.g.
0x80010400) but base=guest_at() is a HOST pointer. a0 -= batch_off +
(uintptr_t)base subtracted a host ptr from a guest addr -> wild magic-div
idx (0xfff2f7a2) -> rec wild -> state!=4 early-return -> cursor
@0x801fc6ac stuck on consumed entry0 -> blk_io_retry timeout -> re-seek.
FIX: index math in guest space (param_1 and table base @0x801fc6c0=0x80010000
both guest addrs); host ptr only for rec deref. VERIFIED: idx=0, cursor
0->5, entries cleared to state 0.

Also established (live):
- CD DMA (ch3) delivers DISTINCT correct sectors per entry:
  208829->e0,208830->e1,...208833->e4, all completed=1 (dma_cdrom_history).
  Earlier "all lba 208829" was stale last_sector_lba in cd_read_log.
- Sector payload == entry record (60 01 01 80 + fields); stamp comes from
  sector MSF header (my 507ab79 fix makes it correct).
- blk_io_retry now SUCCEEDS once (consumed batch5) then TIMES OUT ~41x:
  producer (generated cd_mc_state_machine) does NOT arm batch6 -> no
  state-2 entry -> 2000-poll timeout -> cd_stream_mode_init re-seek.
  Entries stay state 0, seq stuck batch5, stamps stale.
REMAINING: why cd_mc_state_machine stops arming after batch5 — stream
read-ahead/request accounting (decode consumption signal) or another
guest/host mix in the arm path. blk_io_retry is faithful (2000-poll);
check whether producer arms ahead-of-consume correctly.

=== SESSION8c: arm-side starvation precisely isolated ===
LIVE FINDINGS (fresh run, turbo off, port 4458):
- Entry table @0x80010000 has 32 slots (fc6c4=0x20 headers x0x20 = 0x400,
  data bufs 0x7e0-stride from fc6bc=0x80012380). Entries 5-31 = 0x11111111
  FREE marker. Cursors a4=a8=ac=5 = "next batch at index 5". Batch6 NEVER
  armed -> no state-3 entries -> blk_io_retry polls entry[5] forever.
- cd_mc_state_machine invoked ~100/sec (counter @0x801bce88, now 60k+) via
  POLLING callers (menu.c:2137/11240), NOT via CD-event coroutine. It scans
  but finds no pending read request -> writes nothing.
- cd_dir_entry_finalize ran ONCE (e0). a8 set =a4=5 after it -> finalize
  cursor sits at 5; e1-4 (state3) are never revisited.
- IRQ path ALIVE: event_ring shows istat=0x4 (CD) + 0x1 (VBLANK) raising/
  delivering constantly; pc=0x8019A9AC is inside cd_intc_dispatch range
  (handler runs, clears I_STAT). cd_intc_dispatch clean-C counter @0x801bce00
  =0 because generated code reaches 0x8019a8ec directly (wrapper bypassed).
- DMA: DICR=0x009A0000 -> bit23 master + bit19 ch3/CD IRQ ENABLE set.
  channel_irq_flag_armed(3)=true. BUT ch3 IDLE (chcr=0,active=0): no CD DMA
  queued because no entries armed. No istat=0x8 (DMA IRQ) — consistent.
- LOOP: blk_io_retry 2000-poll timeout -> cd_stream_mode_init re-seek ->
  Setloc(seek_msf 46:26:30=LBA208830) because 0x801feb00 still holds e0's
  stamp (208829, no new finalize) -> ReadS streams -> ring_dropped grows
  (game never drains FIFO while polling) -> repeat. read_msf oscillates
  208836-208913, never escapes 208936.
ROOT: producer arm request never issued. cd_mc_state_machine arms only on a
pending read request; the request-generation path (stream offset / decode
consumption / next-chunk seek) is not producing batch6. Either (a) another
guest/host address-space bug in a clean-C function feeding the request, or
(b) file8 internal next-offset fields dump-corrupt (like MSF headers were),
or (c) cd_bitstream_decode not advancing the stream read-offset so the
producer thinks the current window is unfinished.
NEXT: trace what feeds cd_mc_state_machine's arm decision — the pending
read-request/sector-count state (stream job @0x801f4700 + decode state
@0x801b6900/04). Compare vs Ghidra decompile of 0x80199440 arm gate, or
Beetle oracle. file8 payload = 60 01 01 80 entry/GPU records; check whether
an embedded next-LBA/offset field points backward into the window.

=== SESSION8d: cd_mc_state_machine disassembled (capstone) ===
Disasm of 0x80199440 (582 insns) saved to docs/internal/cdmc_80199440.asm.
Globals are 0x80200000-base + negative offset (NOT gp-relative):
- lw -0x3964 -> 0x801FC69C ; ==1 -> EARLY RETURN (reentrancy guard)
- lw -0x3978 -> 0x801FC688 ; ==0 -> skip mode-block to 0x801994c4
- lw -0x33a0 -> 0x801FCC60 deref ; tests bit 0x100000
- fc6b0==0 -> sw 1 -> 0x801FC68C (re-arms tile_stream_pump gate) ; else
  fc6a0++ (error-retry counter @0x801FC6A0) ; sw 1 -> 0x801FCC88
- 0x801994c4: jal 0x80195d44 (helper, a1=sp+0x30 out-buf); v0==5 -> return
REMAINING: trace the arm-gate — the entry-table walk at a4=0x801FC6A4,
tag/seq validation, and the state=3 store. Suspect it early-outs on a
"no pending read request" condition fed by stream/decode state. The
0x801FC69C==1 early-return is a candidate: if a stale 1 is left set, the
whole function no-ops every call -> explains 100/sec invokes, zero arms.
CHECK: read 0x801FC69C live — if ==1, find who should clear it.

=== SESSION8e: arm-gate fully traced to CD-command pump ===
Disasm (capstone, docs/internal/cdmc_80199440.asm + cdmc_helper_96b4c.asm).
cd_mc_state_machine flow (globals 0x80200000-base, neg off):
  fc69c==1 -> ret (reentrancy). fc688!=0 && *0x1f801098&0x1000000 -> mode blk.
  jal 0x80195d44 -> tail-jal 0x80196b4c (CD cmd/response pump, 177 insns,
  calls 0x8019a460/0x80197ad0/0x8018e550/0x801972a8/0x8019a7d4/0x80196370).
  GATE A: helper ret==5 -> EARLY RETURN (no status write; status stays 1).
  GATE B: resp.byte0(sp+0x30)&0x4 -> status=3 return; clear -> entry walk.
  ENTRY WALK 0x80199508: entry=*(0x801fc6c0)+(*(0x801fc6a4)<<5);
    lhu entry.state==0 -> ARM path 0x80199574 (writes seq/stamp/state=3);
    !=0 -> status=4 return.
LIVE: status@0x801bcc88=1 (=> GATE A path: helper returns 5, exits before
entry walk). entries 5-15 state manually zeroed -> STILL not armed, proving
exit is BEFORE entry walk = helper-returns-5 = NO PENDING CD READ COMMAND.
ROOT: stream/decode consumer never queues the next sector-range read.
Decode state @0x801b6900=0x00ffffff idle; stream pos @0x801f472c=0 frozen;
GPU OT frozen. blk_io_retry keeps polling empty entries -> timeout ->
cd_stream_mode_init re-seek(ReadS raw) -> repeat.
NEXT: find who queues the channel-entry read request (the sector range
the helper should process). Likely cd_file_open_b/stream setup wrote it
once for batch1 and nothing re-queues after consume. Trace 0x80196b4c's
"pending command" source or the request-queue field it drains.

=== SESSION8f: MDEC streaming decode — ROOT CAUSE of CD livelock FIXED ===
Root cause of the whole CD-deadlock chain was MDEC, not CD logic:
  cd_mc_state_machine GATE: *0x1f801098 (DMA ch1 CHCR, MDEC-out) & 0x1000000.
  ch1 wedged active=1 remaining=2880 frozen. MDEC produced ZERO output
  because execute_command() only ran execute_decode() when
  input_count >= expected_halfwords (all-at-once buffering).
  Game feeds MDEC-in via DMA ch0 in chunks -> partial input -> no output
  -> ch1 can't drain -> busy bit set -> cd_mc_state_machine returns
  status 1 -> never arms entries -> CD starves -> game can't feed rest
  of MDEC input -> DEADLOCK.
FIX (committed b53fa57): faithful streaming/incremental MDEC decode —
  decode_rle_block(final) rewinds incomplete blocks; decode_stream_available
  pumps complete blocks/macroblocks as input arrives via write_data +
  mdec_dma_write_words; stream_pos in snapshot. Output identical:
  mdec_pin + mdec_e2e BYTE-EXACT vs Beetle reference.
RESULT (live-verified via lldb on guest RAM):
  CD cursors a4=a8 advanced 5->10 (arm+finalize working, was frozen at 5).
  Entry-table stamps INCREASE forward (0x02292646..0x02362646, +0x10000):
  real forward sector streaming, NOT re-read. Pipeline UNBLOCKED.

=== SESSION8g: NEW blocker — cd_bitstream_decode infinite loop ===
Game now reaches blk_io_retry -> cd_bitstream_decode (game/cd.c:3075,
disasm 0x8018ff10-0x80190258). Stuck at +832 in L_f0 main VLC loop.
State @0x801b6904 (resume block) ALL ZERO + static -> stuck in FIRST pass,
never reaches L_2028 save-state. Loop exits only when output a1g hits
limit t6 (=a1g + CDREG32(0x801b6900)*2; reg reads 0x00ffffff -> t6 ~512MB,
effectively infinite) OR escape sym 0x7c1f/0xfe00 -> L_1f8 flush.
VLC table lookup keeps returning t3==0 (no output sym) -> consumes input
bits forever, a0g marches off valid data -> INFINITE.
DIAGNOSIS: bitstream input bytes are GARBAGE — decoder never finds a
terminator. Same corruption class as the MSF-header bug: CD is delivering
wrong payload to the stream. blk_ok=1 blk_to=0 frozen (stuck inside call).
NEXT: verify the bytes at stream input ptr a0 (arg to cd_bitstream_decode,
= s0 return of mc_date_serial_cmp 0x80159ab4, delegated generated). Compare
stream buffer contents vs Game.bin sectors at the armed entry stamps
(LBA ~0x023x region). Check Form2 userdata strip vs whole-sector, and
whether a0 is a pointer vs a handle misinterpreted.

=== SESSION8h: cd_bitstream_decode infinite loop — globals corrupted ===
blk_io_retry -> cd_bitstream_decode(s0, *(s1+idx*4), *(u32*)0x801f4758).
a0=s0=mc_date_serial_cmp ret (guest ptr ~0x8001xxxx file data, VALID).
a1=job[0]/[1]=0x80020000/0x82048000 ping-pong out. a2=*(0x801f4758).
LIVE (lldb guest RAM):
  0x801f4758 held 0xfff12a00 = HOST PTR leaked into guest RAM (VLC base).
  WHOLE block 0x801f4740-0x801f4800+ = high-entropy STREAM DATA, not the
  scene_entry pointers (_DAT_801f4744 should=0x80020000 reads 0x39beff7d;
  _DAT_801f4754 should=param_1=0x80010000 reads 0x003aaaff).
  job[0]@0x801f4700=0x80020000 still correct (copied at init before clobber).
  => stream/decode data OVERWRITES globals 0x801f4740+ AFTER setup.
PATCH TEST: wrote 0x801f4758=0x8007f000 (=param_1+0x6f000, scene_entry val).
  Decode changed from single-PC spin (+832) to cycling real VLC lookups
  (+816/+832/+876/+896) -> VLC base now plausible. VLC table data confirmed
  present ~guest 0x8007fa00 (entries w/ 0xfe00 escape) + 0x80080400.
  BUT decode STILL never returns (blk_ok=1 frozen, resume state all-zero,
  never reaches L_2028). => INPUT bitstream a0 produces no terminator
  (0x7c1f/0xfe00 escape) -> input stream data wrong/incomplete, OR a0
  points at wrong chunk. Output limit t6 ~512MB (reg 0x00ffffff) so only
  escape sym ends decode.
ROOT ISSUE: buffer writes stream data over 0x801f4740-58 globals -> VLC
  base (+ likely stream chunk ptrs) corrupted -> decode reads bad table/
  input -> infinite. NEXT: fresh-run hw watchpoint on guest 0x801f4740
  from BEFORE scene_entry to catch the corrupting writer (one-time write,
  already done in current run). Suspect CD sector-delivery or stream-input
  buffer with wrong guest address (host/guest ptr confusion class).

=== SESSION8i: CORRUPTION WRITER ROOT-CAUSED — MDEC-out DMA ch1 ===
Armed guest_watch from BOOT via new PSX_GWATCH env (debug_server.c) so the
one-time ~8s write is caught before the decode livelock starves the emu
thread. Extended guest_watch GwEntry with host fp/lr/sp + a 10-deep unwound
host return-address chain (ra_chain) captured AT fault time in the SIGSEGV
handler — survives later stack reuse that defeated post-hoc lldb unwinding.
CATCH (35 in-range entries on 0x801f4740:64):
  seq16-18 legit scene_entry pointer writes (store_pc 0xBFC1FD70 BIOS).
  seq19-34 CORRUPTION: 8+ contiguous u32 stream words (afffd7ff 7dffbe39
  65ff45c2 00ff5600 f87d0000 003aaaff fff12a00 04005a7e ...) marching
  0x801F4740->0x801F477C via psx_write_word_raw.
RESOLVED CALL CHAIN (image lookup on ra_chain):
  advance_mdec_channel+472 -> dma_advance+80 -> psx_devices_service_to_now
  -> br2eu_func_80159AB4 -> br2eu_psx_dispatch_game_compiled ->
  dirty_ram_dispatch -> psx_dispatch_impl -> br2_call_generated ->
  blk_io_retry+88 -> scene_entry+736.
WRITER = advance_mdec_channel (dma.c:541) ch==1 MDEC->RAM branch:
  psx_write_word(addr, mdec_dma_read_word()), addr=channels[1].madr.
  DMA provenance: madr marched 0x1F4740->0x1F477C, bcr=0x005A0020.
  => MDEC-out DMA is streaming decoded output into guest 0x801F4740,
     overwriting the scene pointer globals (_DAT_801f4744-58) that
     cd_bitstream_decode needs as its VLC-table base -> decode spins.
NOTE: init_pc=0x80192EBC (func br2eu_func_80192E78) programs DMA ch2 GPU
  regs (0x1F8010A0/a4/a8 = ch2 madr/bcr/chcr), NOT ch1 — s_dma_ch_initiator
  attribution is last-store-PC, misleading here. Real ch1 madr writer is a
  different guest store to 0x1F801090 still to be pinned.
OPEN: is madr=0x801F4740 game-intended (shared decode/video workspace,
  ordering bug) or a wrong/garbage madr (b53fa57 incremental-MDEC timing
  regression suspect — it made ch1 stream output continuously mid-feed)?
NEXT: watch channels[1].madr writes (dma_write_masked reg==0x00 ch==1) to
  catch the guest PC/value that programs 0x801F4740, and check whether the
  MDEC transfer count/buffer is the scene decode workspace being reused.

=== SESSION8j: ch1 MADR provenance — MDEC ch regs NEVER written via MMIO ===
Extended dma_write_masked to trace_dma_reg_write every per-channel
MADR/BCR/CHCR store (kind 'W', captures guest store pc + func). Rebuilt,
fresh PSX_GWATCH run to livelock, dumped full dma_trace ring (16384).
RESULT: the ONLY DMA channel-register writes in the window are GPU ch2
(0x1F8010A0/A4/A8 = madr/bcr/chcr, pc 0x80192e98/ea8/ebc = func_80192E78)
and ch6 block (0x1F8010E0/E4/E8). ZERO writes to ch0 (0x1F801080) or
ch1 (0x1F801090/94/98) MDEC registers.
=> channels[1].madv=0x801F4740 (which advance_mdec_channel ch==1 streams
   MDEC-out to) is NOT programmed by a normal DMA MMIO write in this window.
   Either (a) ch1 regs written before trace window wrapped, (b) madr set by
   an internal path (finish_async_mdec_transfer / mdec init / snapshot), or
   (c) mdec_async[1] is a STALE/PHANTOM active transfer replaying to a
   garbage madr — consistent with b53fa57 incremental-MDEC making ch1 drain
   continuously. channels[]/mdec_async[] are static in _MergedGlobals; read
   live next session to confirm active/remaining_words/start_addr.

=== SESSION8k: ROOT CAUSE + FIX — scene_entry MDEC slot deref (RESOLVED) ===
The SESSION8j "phantom ch1" hypothesis was WRONG. dma_trace showed ch0/ch1
MDEC regs ARE written — just not via dma_write_masked MMIO (the game uses
cached reg-pointer cells 0x801B68xx -> 0x1F8010xx, bypassing that path).
Full ch0/ch1 programming cluster captured at seq 98136-98144:
  ch0 (MDEC-IN):  madr=0x801F4708 bcr=0x4000020 chcr=0x1000201 (32768w)
  ch1 (MDEC-OUT): madr=0x801F470C bcr=0x5A0020  chcr=0x1000200 (2880w)
ch1 'S'->'C' consecutive = legit decode, but output landed ON the job struct.

TRUE ROOT CAUSE — clean-C porting bug in scene_entry (game/game.c):
Original br2eu_func_80159028 does `lw a0, 0xC(flag*4 + $s1)` / `lw a0,
flag2*4($s1)` — DEREFERENCES the job ping-pong slot to get the real
MDEC-out buffer pointer (slots hold file+0x60000=0x80070000 etc, written
by tile_stream_job_init). The clean port passed the slot ADDRESS
(0x801F470C) directly to cd_fileop_forward / gpu_pkt_attr_set, so MDEC
wrote 2880 decoded words onto 0x801F4700+ and obliterated the pointer
globals at 0x801f4740 -> cd_bitstream_decode read a garbage VLC base and
spun forever. tile_stream_pump (menu.c) already had the correct `*(...)`.

FIX (commit 557415b): dereference both slots via guest_at in scene_entry.
VERIFIED headless (PSX_HEADLESS --disc): frames advance ~66fps past scene
decode (was infinite livelock), epc progresses 0x80159xxx->0x80199xxx,
CD actively streaming (sector 46:27:07, reading:1), all frame_fingerprint
hashes differ per frame = real animated output. Globals 0x801f4740+ now
hold correct buffers 0x80020000/0x80048000/0x80070000/0x80077800.

NOTE: GUI launcher path stalls in SDL event-wait (no emulation) in this
environment; PSX_HEADLESS=1 + --disc build-wasm/disc/Game.cue is the
reliable verification path. Debug server 127.0.0.1:4370.

=== SESSION8l: MDEC input-gate FIX verified + new CD channel-job blocker ===
FIX (a86ed31): mdec_dma_write_ready() no longer gates ch0 input on
output_pos<output_size. b53fa57 streaming decode emits output mid-feed;
the vendor gate then refused further input -> decoder froze at 180/192
halfwords, busy=1 forever. Real MDEC input/output are independent paths;
output[] is a growable pending-drain buffer, not a fixed FIFO.
VERIFIED headless: the stalled DECODE (0x32000880, 4352 hw) now completes
- 645 macroblocks, ~484KB output, ch1 drains. Was hard-frozen at 16 MB.

NEW BLOCKER (next bring-up bug, deeper): game reaches scene_entry tile
streaming, MDEC decode completes, but is now stuck in a CD read-retry
loop at sector MSF 46:26:55 (~38-sector block, lba ~208923-208961):
  cmd pattern repeats Setloc->SetMode(0)->SetMode(0xE0)->ReadS(0x1B).
  read_continues_current_stream() returns 0 each Setloc (s_setloc_lba !=
  next_lba) -> start_read_stream restarts the whole block -> never advances.
ROOT SYMPTOM: cd_channel_poll (0x80199360, the game's epc) waits for a
channel-table entry (base@0x801fc6c0 + idx@0x801fc6ac *0x20) state u16==2.
No entry ever reaches 2 -> blk_io_retry returns -1 -> cd_file_open_b
re-issues the read -> infinite loop. Frames still ~66fps (tile_decode_wait
timeout lets the render loop run), CD seq >1.1M.
KEY EVIDENCE: request_reg=0x00 -> BFRD NEVER set; DMA ch3 (CD data) chcr=0
idle; sector ring fills (sector_avail=1, size 2340) but guest never drains
-> ring_dropped grows. So sector data never reaches the game's channel
buffers -> channel jobs (cd_chan_job_submit -> cd_job_slot_alloc ->
cd_intc_dispatch) never complete -> entries never state=2.
LEAD: the game's CD async-job path isn't arming the data-drain (BFRD/ch3)
or the IRQ-driven channel-state update (cd_intc_dispatch @0x8019a8ec) isn't
marking entries. Likely another clean-C porting divergence in the CD job /
DMA-arm / IRQ-handler chain (same class as the scene_entry slot-deref bug).
NEXT: trace who is supposed to set request_reg BFRD + arm ch3 and mark
channel state=2; check cd_intc_dispatch actually runs on CD IRQ and walks
the job/channel tables. i_mask=0xD (CDROM bit2 enabled), i_stat=0 between
serviced IRQs.

=== SESSION8m: SMOKING GUN — cd_intc_dispatch never invoked ===
cd_intc_dispatch (0x8019a8ec) bumps scratch counter @0x801bce00 on entry.
read_ram shows it stays 0x00000000 -> the game's CD-subsystem INTC
dispatcher is NEVER called, even though cd_irq_delivered=44280 (CD IRQs
fire + are taken by the CPU) and CD commands ACK fine via polling.
=> CD data-ready IRQs reach the CPU but the game's CD driver never runs
   its deferred dispatch -> channel-table entries never reach state==2 ->
   cd_channel_poll spins -> blk_io_retry -1 -> read-retry loop.
Also confirmed: request_reg=0x00 (BFRD never set), DMA ch3 idle -> the
sector ring fills but the guest never drains it into channel buffers.
s_irq_defer_cdrom is env-gated OFF by default, so IRQs deliver normally;
the break is in the game's ISR->cd_intc_dispatch routing or the
deferred-coroutine ctx_save/ctx_restore resume (cd.c:4962 contract:
cd_ctx_save(0x801bcd50) ... on CD IRQ kernel calls cd_ctx_restore ->
cd_chan_full_init -> cd_intc_dispatch). Needs live cpu->gpr -> must run
generated, not clean-C. NEXT: verify the game's CD ISR actually invokes
the deferred path on CDROM IRQ (bit2); check whether cd_ctx_save armed a
resume that never fires, or the ISR mask/table skips CD events.

=== SESSION8n: deferred-init armed; break is IRQ->cd_ctx_restore resume ===
Verified live: ctx frame @0x801bcd50 IS populated (cd_ctx_save ran) and
INTC handler table @0x801bcd1c holds 0x8019ae68/0x801979f8/0x8019af7c —
so cd_chan_full_init (0x8019a814) is parked in its deferred wait. Channel
table (base *(0x801fc6c0)=0x80010000, idx@0x801fc6ac=5): entries show
state 0/3/4, NONE reach state==2 (data-ready) that cd_channel_poll needs.
request_reg=0x00 (BFRD never set), DMA ch3 idle -> sector data never
reaches channel buffers -> channels can't complete -> state!=2.
CHAIN: CD IRQ fires (cd_irq_delivered=44280) -> should hit the game's
registered deferred hook (cd_ctx_restore 0x8019adcc, registered via
B0/0x19 -> kernel mem[0x4C18]) -> resume cd_chan_full_init ->
cd_intc_dispatch -> arm data path / mark channels. The resume never
fires, so the whole CD receive path stays unarmed = chicken-and-egg.
NEXT: verify the recompiled-BIOS interrupt-hook dispatch actually calls
the game's CD deferred handler on CDROM IRQ (i_stat bit2). Check whether
B0/0x19 registration landed in the kernel hook table the IRQ dispatcher
walks, and whether cd_ctx_restore (0x8019adcc) is reachable from it.
cd_intc_dispatch thunk-counter may be bypassed by direct generated
calls, so confirm via side-effects not the counter.

=== SESSION8o: channel-table cursor deadlock — steady-state starvation ===
Refined SESSION8n. Deferred-init DID complete enough for real work: ch3 CD
DMA ran frames ~444-454 (dma_trace: 8-word entry writes to 0x80010000+0x20*n
and 504-word=0x7e0 data writes to channel buffers, i_stat bit2 firing), and
cd_dir_entry_finalize ran 6x early. Then ALL DMA went idle for 24k+ frames.
LIVE STATE (frame ~32k):
- cd_mc_state_machine (0x80199440) invoked 87k times (ctr @0x801bce88),
  returns status=4 (@0x801bcc88) EVERY call -> blk_io_retry retries forever.
- Disasm cdmc_80199440.asm decoded: it computes entry = base@0x801fc6c0 +
  cursor@0x801fc6a4 *0x20, loads entry.state (lhu +0). beqz -> proceeds to
  submit (jal cd_chan_cmd_submit 0x80199d88 with a0=3 = DMA ch3, OR
  cd_copy_u32 0x80199d5c when flag@0x801fc6b0!=0). state!=0 -> returns 4.
- cursor@0x801fc6a4=30 -> entry[30] @0x800103C0 state=1 (stuck nonzero).
  Entries 0-9: states 0,0,0,0,0,4,3,3,3,3 — reads issued (state3) but the
  3->2 completion never fires. Poll cursor@0x801fc6ac=5 sits on state4.
DEADLOCK CHAIN: an entry stuck non-zero blocks the submit cursor -> status4
-> ch3 never re-arms -> in-progress entries starve for data -> stay state3
-> cursor never frees. cd_channel_poll (epc 0x80199360) waits state==2.
DATA PATH: ReadS streams (read_msf advances 46:26:55->46:28:12, re-seeks
every ~36 frames), sectors land in ring but ring_dropped=88203 (consumer
never drains). request_reg=0x00 -> BFRD(bit7) never set -> ring won't
deliver. So sector->channel-buffer DMA (ch3) is the missing link AND the
IRQ->cd_dir_entry_finalize(state=2) completion is also not firing now.
NOTE: 0x801bdeb0 block -> 0x1F801C00 + ch4/ch7 = SPU ctx, NOT CD. Real CD
DMA is ch3 (0x1F8010B0) via cd_chan_cmd_submit base=0x1f801080+ch*0x10.
NEXT: find what sets entry.state 3->2 and 1->0. state=2 producer =
cd_dir_entry_finalize(0x801990f0)/store(0x80199100), reached via CD IRQ
-> cd_intc_dispatch -> cd_response_pump. state=1 likely = reserved/armed.
Determine why the IRQ->finalize path stopped after frame ~454: is the
game's top-level IRQ handler (vector 0x80000080 -> k0=0x27AC kernel
dispatch) still calling the CD sub-handler, and is cd_response_pump
(0x801979f8, in INTC table) executing? Add a counter to cd_chan_cmd_submit
ch3-arm + cd_dir_entry_finalize to see which side of the break is dead.

=== SESSION8p: counters isolate the deadlock — submit cursor blocked ===
Added CDDBG counters @0x801bcf00-f10 (chan_cmd_submit calls / ch3 arms /
finalize / store / ch3 CHCR-kick). Live read at frame ~3303 (stuck loop):
  submit=61  ch3arm=61  finalize=6  store=0  ch3kick=61  -- ALL FROZEN.
cd_response_pump=264k, cd_response_process=530k (type2=129k dominant,
type4=4849). cd_mc_state_machine=127k returns status=4 every call.
DEADLOCK CONFIRMED both directions:
- SUBMIT side dead: cd_mc_state_machine disasm shows it only calls
  cd_chan_cmd_submit when entry[cursor@0x801fc6a4=30].state==0; entry[30]
  @0x800103C0 is stuck state=1 -> status4 -> never reaches submit -> the
  submit/ch3arm/kick counters froze at 61 (all from early boot).
- COMPLETE side dead: finalize=6 (early only), store=0. ch3 DMA completes
  (all 61 kicks finished, active:0) but does NOT raise i_stat bit3 (zero
  DMA IRQs in irqctx) -> completion is CDROM-IRQ driven, not DMA-IRQ.
  cd_response_pump runs constantly but the type-2 path (dominant, 129k)
  has cb@0x801bc848==0 (per-session cb, NULL for this op) -> skipped;
  type-4 (4849) fires cd_mc_sm_wrap(0x80198fe4)->cd_mc_state_machine.
ENTRY LIFECYCLE (from cdmc_80199440.asm): state0=free->submit; nonzero->
status4. Machine writes state=0 in cleanup paths (0x80199754/84/804).
entry30.state=1 = reserved/DMA-in-flight awaiting completion that never
signals. entries 0-9: 0,0,0,0,0,4,3,3,3,3 (reads issued, no state=2).
OPEN: what transitions a state=1/3 entry to state=2? cd_dir_entry_finalize
(0x801990f0)/store(0x80199100) are the only state=2 writers, reached via
CDROM IRQ -> cd_response_pump -> type-callback -> channel-complete. Find
which response type / callback marks a finished ch3 channel read state=2,
and why it stopped after ~6 early completions (frame ~454). Likely the
same deferred-coroutine resume class as cd_chan_full_init.

=== SESSION8q: INTC dispatch decoded; stranded state=1 entry is the wedge ===
cd_intc_dispatch (0x8019a8ec) mask = i_stat(0x1F801070) & i_mask(0x1F801074)
& u16@0x801bcd48(=0xD). Handler table @0x801bcd1c u32[11]:
  tbl[0]=0x8019ae68 cd_cb8_dispatch (VBlank bit0)
  tbl[2]=0x801979f8 cd_response_pump (CDROM bit2) -- runs 264k, alive
  tbl[3]=0x8019af7c (DMA bit3) -- channel/DMA completion, generated-only
So channel state=2 completion is DMA-IRQ (i_stat bit3) -> tbl[3]=0x8019af7c
-> cd_dir_entry_store/finalize. Runs only while ch3 DMA is active.
ENTRY STATE MACHINE (cdmc_80199440.asm fully decoded):
  state0=free -> claim(state1 @0x801999f4/0x80199ab0) -> submit DMA
  (cd_chan_cmd_submit a0=3 OR cd_copy_u32 when flag@0x801fc6b0) ->
  state3 (@0x80199d0c) -> cursor++ -> finalize(prev)@0x80199d44.
  state!=0 at cursor -> status4 early return (never reaches submit/finalize).
LIVE: cursor@0x801fc6a4=30, finalize-idx@0x801fc6a8=30, poll@0x801fc6ac=5.
entry[30]@0x800103C0 state=1 sector=0x2592646 (~MSF46:26) = CLAIMED but
DMA submit never ran -> stranded. cd_mc_state_machine only submits on
state==0, so a stranded state=1 blocks the whole table (status4 x127k).
entries 0-4 state0(free,behind cursor), 5 state4(consumed), 6-9 state3.
ROOT QUESTION: why did the claim->submit path return early leaving
state=1? A status path (5,6,7,8,9,0xa) fired between the state1 write and
the state3 write. That early-return is gated by a register/entry-field
read -- prime suspect is an MMIO/register read returning wrong data
(faithful-core bug class), OR a legitimate CD-status timeout. Next: put a
watchpoint/counter on the state1->early-return edges in generated
0x80199440, or dump which status code path entry30's claim took.

=== SESSION8r: channel ring fully decoded — circular stall, not single wedge ===
LIVE channel table @0x80010000 (stride 0x20, state=u16@+0):
  e0-4 state0 free | e5 state4 consumed | e6-9,11-14,16-19,21-24,26-29
  state3 in-flight | e10,15,20,25 state2 DATA-READY | e30 state1 CLAIMED
  e31 sentinel. => ring is HEALTHY: CD delivers, entries DO reach state2.
Cursors: produce/sub@0x801fc6a4=30, finalize@0x801fc6a8=30,
  poll/consume@0x801fc6ac=5, mode@0x801fc6b0=0(DMA path), reset@0x801fc6b4=0.
0x801feb18=0x800103C0 = current-entry ptr (=e30). 0x801feb00=0x2542646 =
  e25 sector = last finalized. Callbacks 0x801feb10/0x801feb14 BOTH NULL.
STATE MACHINE (cdmc_80199440.asm): state is TRANSIENT inside one call —
  claim writes state1 (@0x801999f4 or @0x80199ab0), submit writes state3
  (@0x80199d0c). state!=0 at cursor -> status4 return (NO advance, NO
  retry-counter bump when 0x801fc6b0==0). So e30.state1 = a call claimed it
  then EARLY-RETURNED before state3. Candidate strand exits after claim:
  status8 (@0x80199a4c, gated *(0x801fc6b0)!=0), status9 (@0x80199aa8),
  or a status5/6/7 path in the submit window. fn@0x801FEB14 is NULL so the
  post-claim jalr is skipped.
CONSUME SIDE: cd_channel_poll waits entry[poll_idx].state==2; e5.state4 ->
  return1 forever. e5 is freed only by cd_dir_entry_probe(entry_addr)
  (tag==4 check, clears count entries, sets 0x801fc6ac=a0+idx). Probe is
  called by game AFTER consuming data; if it isn't running e5 stays 4.
cd_dir_entry_store (random state2 producer, idx+base args) NEVER ran
  (counter 0x801bcf0c=0); only sequential finalize (0x801bcf08) produced
  the state2 entries. 0x801feb10 notify cb NULL -> no downstream kick.
BFRD=0: clear_sector_buffer() clears REQUEST.BFRD on every seek/read;
  re-arm needs the produce path (blocked at e30). ch3 DMA data is gated
  request_reg&BFRD && rb_available (cdrom.c:1875). ring_dropped grows.
STALE-PAUSE: pending.cmd=0x09(Pause) armed; cdrom.c:2105 documents Pause
  INT2 (~1.1Mcyc) overlapping ReadS -> stale COMPLETE -> read-fail retry.
  Game loops Setloc(46:26:55)->SetMode->SetMode->ReadS forever = the
  read-retry the channel layer is starving on.
CIRCULAR STALL: e30.state1 blocks produce -> BFRD stays0 -> ring drops ->
  stream restarts at same seek -> e30's sector never delivered -> state1.
NEXT: (a) histogram status codes written to 0x801bcc88 to catch the
  strand status (8/9/5/6/7) for e30's claim; (b) check if 0x801fc6b0
  toggles (mode flag) — e30 may've been claimed under copy-mode; (c)
  verify whether e5.state4 SHOULD have been freed by cd_dir_entry_probe
  (is probe running? add counter); (d) the stale-Pause INT2 vs ReadS
  overlap is a real timing hazard to validate against Beetle.

=== SESSION8t: consume-side live counters; BFRD exonerated ===
NEW DIAG (0x801bcf14-0x801bcf84 + cdrom bfrd_set/clear):
  cd_channel_poll = 260M calls, found state2 only 2x, stuck idx=5
    (e5.state4 consumed). 0x801bcf1c=last entry state seen=4.
  cd_dir_entry_probe = 2 calls only (freed 1). arg 0x80012B60 -> idx -5
    (not a table entry; probe barely runs -> poll cursor can't advance).
  cd_mc_state_machine status histogram @0x801bcc88:
    st4=3261 (entry-busy wall), st8=1, st0xa=30, others 0.
    cursor@0x801fc6a4=30, entry30.state=1 confirmed live.
BFRD EXONERATED: cdrom bfrd_set=496, bfrd_clear=13, request=0x00 now.
  Game DOES set BFRD regularly -> data IS offered to DMA when armed.
REFINED MODEL: entry.state is TRANSIENT in one cd_mc_state_machine call.
  state0->claim(state1)->issue-read->ret status8 ("read pending, deferred").
  status8 is the NORMAL post-claim return, NOT a strand bug. state1 =
  "CD read issued, awaiting sector data". The 1->3 (DMA-arm) transition
  is done by a DIFFERENT path than cd_mc_state_machine (which returns
  status8/4 for state!=0 and never re-processes state1).
  => entry30.state1 persists because its CD sector data never completes
     the 1->3 step. ring_dropped grows = sectors arrive but the specific
     consumer for e30's read isn't draining.
REMAINING QUESTION: which function performs state1->state3 on CD
  data-ready (INT1)? Candidates: a DMA-arm helper, cd_dir_entry_store
  (random state2 producer, never ran), or the ch3-DMA-done path.
  cd_dir_entry_store(0x80199100) is reached via jalr/jal from generated;
  its trigger/callback registration is the next thing to find.

=== SESSION8u: accel exonerated; deadlock = channel data-delivery->entry-completion ===
accel_consumer_waits=0 EXPECTED (g_disc_speed_divisor=1, acceleration OFF).
ring_dropped=18023 is genuine consumer starvation, not an enhancement bug.
cd_response_process type histogram @0x801bce34+type*4: type1(data-ready
INT1)=43491 fires constantly -> cd_response_pump returns 4 -> fires
0x801bc84c=cd_mc_sm_wrap->cd_mc_state_machine on EVERY data-ready.
CONFIRMED: cd_mc_state_machine cannot advance state!=0 entries (returns
status8 post-claim / status4 entry-busy). So state1->state3 (DMA-arm on
data-ready) is done by a SEPARATE function not yet identified. Candidates
examined: cd_drain_dispatch(->cd_queue_drain, no), tbl[3]=0x8019af7c
(DMA-IRQ handler, generated-only, loads *(0x801BDDF0) dispatch tbl then
falls into cd_cb7_run region). cd_dir_entry_store(0x80199100, random
state2 producer) reached via jalr, trigger unregistered/unfired.
GAME STATE: intro FMV loop (game.c:1584 blk_io_retry->cd_channel_poll)
parked at epc 0x80159AE8, vblank advancing, MDEC decode works (645 mb).
The channel ring is healthy (e10/15/20/25 state2) but BOTH cursors wedged:
produce@30(e30.state1 awaiting data->DMA-arm), consume@5(e5.state4
awaiting cd_dir_entry_probe free; probe ran only 2x).
ROOT (refined): a channel read's sector data is not completing the
state1->state3 DMA-arm for the frontier entry. Everything downstream
(consume, reuse, BFRD re-arm, stream-advance) is starved by that.
NEXT concrete step: find the data-delivery function that writes sector
data into channel buffers (base+idx2*0x20+idx*0x7e0 from cd_channel_poll)
and marks the entry — likely the ch3-DMA-done path via tbl[3]=0x8019af7c
or a CD-data-ready->DMA-arm helper. Disasm 0x8019af7c / find who writes
channel data buffers + transitions state1->3.

=== SESSION9: CD deadlock ROOT-CAUSED + clean-C guest-pointer/prototype sweep ===
BREAKTHROUGH — the CD channel livelock is SOLVED. Two faithful-port bugs:
1. cd_dir_entry_probe (0x80199290): clean-C port dropped the MIPS
   signed magic-div ADD-BACK term. MIPS does mfhi->addu hi,hi,n->sra8->
   sub sign(pre-shift dividend). Port did ((hi+sign)>>8)-sign -> idx=-5
   instead of 5 -> consumed ring entries never freed -> consume cursor
   wedged @entry5.state4 -> produce starved @entry30.state1 -> BFRD never
   re-armed -> Setloc/ReadS retry livelock in intro FMV. FIXED: restored
   ((hi + a0_pre>>2) >> 8) - sign(n_pre). Verified: probe_freed 1->6,
   consume cursor 5->30, ring drains to state0.
2. cd_read_param_block (0x80199420): is a 3-arg fn storing a2->
   @0x801fc6b4 (channel stuck-entry reset flag). Clean-C had 2-arg sig,
   dropped a2 -> reset flag never set -> wedged state1 entries unrecoverable.
   FIXED: 3-arg sig + clean_table wrapper passes gpr[6].
THEN a sweep of clean-C guest-address bugs (all crash-verified):
- attract_stage_load_step: param_1=0x1f800000 scratchpad guest addr was
  deref'd as host ptr (+0x8c). -> guest_at.
- text_render_simple/text_render_full: +0x1c cell holds GUEST text ptr,
  was double-deref'd as host. -> guest_at(*guest_at(cell)).
- fighter_spawn_tail caller passed raw 0x1f800000 guest addr to a fn
  expecting host ptr. -> guest_at(0x1f800000).
- save_channel_init_commit: +0x38 cell holds GUEST ptr, +0xa8 store
  deref'd as host. -> guest_at(*guest_at(cell)+0xa8).
- terrain_packet_gen: weak ()-stub shadowed real 8-arg impl in menu.c;
  calls compiled "no prototype/too many args" -> param_3 read as 0x25.
  -> real extern prototype.
RESULT: game now boots, plays intro FMV (MDEC 645mb), reaches attract/
demo, deep into battle setup + save/resource loading. Committed cd06290.
NEW BLOCKER (current): GPU GP0 "unknown command 0xFE" (word 0xFE760F09)
halt. gp0_frame_dump shows 68 well-formed identical draw packets
(ops 01/02/0E/24/28/3C/BF/E1-E6) parsing cleanly, then ONE malformed word
at the packet boundary. DMA src (masked &0x1FFFFC) = 0x2C..0x10C =
scratchpad 0x1F80002C region -> the GPU command/OT list built in
scratchpad has a bad word at offset ~0x110. Likely an upstream clean-C
writer producing malformed OT data OR a DMA length/pointer error.
NEXT: locate the scratchpad OT builder, find what writes word@+0x110,
check count/terminator. Ghidra MCP still DOWN (port 8080 refused).

=== SESSION9b: display pipeline black-screen root-caused to draw-env struct ===
Game runs (CD/FM/attract/battle logic) but renders BLACK. Traced via
gpu_frame_dump + gpu_state + read_ram:
- GP0 env cmds all from pc 0x80192E88 <- ra 0x801265F4 (frame_tick path).
- draw_area = [0,254,319,254] -> top==bottom=254 = ZERO HEIGHT clip ->
  nothing rasterizes. X is correct (0..319 = 320px).
- h_display=[3220,3260] -> GP1(06h) X-range only 40 video-cycles ->
  width computes to 4px (should be ~2560 cycles/320px). Separate GP1 bug.
- Screen dims OK: _DAT_1f800020=320, _DAT_1f800022=240 (scratchpad).
- Draw-env struct ptr = scratchpad[4]=0x801e61f0 (also fixed addr in
  oam_list_rebuild). gpu_draw_env_put->gpu_prim_pkt_build reads env[0..5]
  ushorts: env[1]=drawY-top, env[3]=height -> bottom=env[3]+env[1]-1.
- sprite_attr_init_a(0x801e61f0,0,0xf0,320,240) SHOULD set
  env[1]=240,env[3]=240 -> draw_area [0,240,319,479] (back buffer, correct
  for double-buffered 320x240). OBSERVED [0,254,319,254] means the struct
  is CORRUPTED at call time: env[1]&0x3ff=254, env[3]+env[1]-1=254.
- Dump of 0x801e61f0 shows PACKED sprite/vertex data (0x5505,0x5555,
  0xac00,0xc455,0xfe...), NOT the env ushorts -> the draw-env struct is
  being OVERWRITTEN by render/sprite writes between init and
  gpu_draw_env_put. Likely another clean-C guest-pointer/aliasing bug:
  a sprite/vertex writer targeting 0x801e61f0 as a buffer.
NEXT: find who writes packed render data over 0x801e61f0 (gwatch on
0x801e61f0+0x02 for the env[1] u16), or whether frame_tick's iVar8 path
reads a stale/reused buffer. ALSO separate: GP1(06h) h_display 3220/3260
bad value source.

=== SESSION9c: draw-env corruption mechanism identified ===
_DAT_1f800004 (scratchpad+4) is NOT a fixed env template — game.c:1630
sets it = _DAT_1f80000c*0x2150 - 0x7fe1bf60, a PER-PLAYER-OBJECT pointer
(=0x801e61f0 observed). menu.c:2042-45 does sprite_attr_init_a(ptr) then
gpu_draw_env_put(ptr) adjacently (fresh env). BUT frame_tick calls
gpu_draw_env_put(iVar8=scratchpad[4]) with NO preceding re-init -> reads
stale env fields that the object's sprite/render writes have overwritten
with packed vertex/command data (0x5505,0x5555,0xac00,0xc455,0xfe...).
Result: draw_area=[0,254,319,254] zero-height -> black; and the same
corrupted packet bytes surface as bogus GP0 commands (0xF4/0xFE desync).
ROOT CANDIDATES: (a) frame_tick should re-init env per frame like
menu.c does, or read a stable env copy (gpu_draw_env_put memcpys env to
0x801b8330 — maybe should read THAT); (b) _DAT_1f800004 points to wrong
object; (c) sprite writes alias env[0..5]. ALSO unresolved: GP1(06h)
h_display=[3220,3260] -> 4px width, separate bad-value source.
This is squarely the render-pipeline backlog (battle.c COP2/GTE + sprite/
draw-env port), todo item 10 — large subsystem, not a quick fix.

=== SESSION9d: display pipeline — draw env CLEAN, h_display=4px isolated ===
VERIFIED via live read_ram (headless+software, port 4370):
- Draw env @0x801e40a0/0x801e61f0 stays VALID whole run: u16
  [0,0x00f0,0x0140,0x00f0,...] = [x0,y240,w320,h240] -> draw_area
  [0,0,319,239] CORRECT (earlier [0,254,319,254] was post-halt noise).
- Display envs @+0x5c (0x801e40fc/0x801e624c) plausible.
- gpu_state draw_area now [0,0,319,239]; ONLY defect = h_display
  [3220,3260]=[0xc94,0xcbc] -> dots=40,cycles=8 -> width=4 (black).
ROOT-TRACED to draw_env_timing (gfx.c:674) GP1(06h):
  iVar4=(env[9]*5+iVar9)*4 ; env[9]=cVar3=GP32(0x801bde20)=1 (faithful:
  MIPS sb gpr[2]->env+0x12 confirmed @0x80191AAC; flag set=1 in boot()
  via cd_xchg_bde20(1) @game.c:787). iVar9=1 (w=320). iVar4=24 ->
  t16[24]=0x1810 -> uVar7=6160 overflows -> clamps to 0xc94/0xcbc.
  Page0 (env[9]=0) gives iVar4=4 -> t16[4]=0x021b -> ~2560 dots = 320px.
OPEN QUESTION (needs Beetle/psx-spx oracle): is 0x801bde20=1 correct so
the game legitimately selects timing-table page1 (then the table@0x801b83f4
or runtime GP1(06h)->width formula is the bug), OR does the real game keep
page0? Table is [page][mode][4xu16]; page1 horiz values 0x1810/0x1f80 look
like VERTICAL timing -> page1 likely invalid for GP1(06h).
ALSO STILL OPEN: GP0 desync halt (0xF4/0xF7/0xFE opcode) — separate issue,
command-buffer word corruption at packet boundary.
gwatch on env region crashed process (mprotect fault-handler fragility) —
avoid for now; use read_ram sampling instead.

=== SESSION9e: display FIXED (320x240 + real draw area); rendering CONFIRMED; GP0 desync isolated ===
VERIFIED FIXES (committed+pushed 7b7e850, 42ab50c):
1. draw_env_timing timing-table index bug (gfx.c): iVar4=(env[9]*5+iVar9)*4
   is a BYTE offset into table@0x801b83f4, but clean-C indexed t16[iVar4]
   (u16 index = *2). For iVar4=24 read t16[24]=0x801b8424 (VERTICAL table)
   -> degenerate GP1(06h)=[0xc94,0xcbc]=4px. MIPS uses lhu base+iVar4 (byte).
   FIX: index by iVar4/2 -> reads 0x801b840c=0x0270/0x0c70 = PAL-320
   [624,3184]=2560 dots=320px EXACT (matches psx-spx PAL range).
   env[9]=GP32(0x801bde20)=1 = legit PAL page (verified faithful MIPS).
2. coord_clamp viewport dims segment bug (gfx.c): read 0x801c8324/326
   (garbage code bytes 0xb8f4/0xbe0f) instead of 0x801b8324/326
   (=0x0400/0x0200 real VRAM dims). FIX -> draw_area=[0,240,319,479]
   real double-buffered back-buffer clip region (was [243,526,243,526]).
RESULT: gpu_state w=320 h=240 draw_area=[0,240,319,479] h_display=[624,3184].
Screenshot psx_screenshot.png now has REAL game colors (dark red/green/
teal/purple ~6.7% coverage) -> primitives reach framebuffer. NOTE: image
is byte-identical across frames -> display buffer appears static/frozen on
one presented frame (present/swap path may need check; game still simulates,
cur_frame advances).

=== SESSION9f: GP0 desync PRECISELY ISOLATED (not word-count; systematic) ===
Halt: "GPU GP0 unknown command 0xFE (word 0xFE1F220E)" at frame ~634.
Command buffer located @guest 0x8002D974 (read_ram). Raw LE words:
  ...0x17E90000(NOP) 0x04002121(NOP) 0x266B9828(tex-tri n7)
  0xFFF306A2(NOP) 0x00000000 0xD0000181(VRAM->CPU n3) 0x477E004D(line n3)
  0x43A0FF2D(line n3) 0x6C1B0000(1x1tex n3) 0x9BD600BF(VRAM->VRAM n4)
  0x00001D9D 0x2704ABF9 0xC3900047 | 0xFE1F220E <- HALT
  then VALID prims resume: 0x34573819(0x34 shaded-tex-tri) 0x2C310000(0x2C)
VERIFIED all word-counts CORRECT vs psx.wiki/psx-spx: VRAM->VRAM=4,
1x1tex=3, lines=3, tex-tri=7, VRAM->CPU header=3. src advances
consistently = parser self-consistent. NO polylines (0x48-0x5F) in frame.
0xF0+ has NO valid GP0 cmd on real HW -> 0xFE1F220E is DATA being read as
cmd => a command EARLIER under/over-consumed (systematic desync), OR the
packet/OT builder emitted a boundary artifact. DIAG: NOPing 0xF0-0xFF let it
pass frame634 but died silently later -> multiple stray words = systematic.
NEXT: find FIRST misaligned cmd in frame (42k-entry ring; likely a
variable-len cmd: CPU->VRAM 0xA0-0xBF pixel count, or a rare prim type at
this scene). Emitter = br2eu_func_8018CFDC (generated, faithful). Likely
clean-C upstream data corruption into the prim template. ALSO: display
buffer static -> check present/swap path separately.

=== SESSION9g: GP0 desync FIXED (game reaches frame 648); new dispatch bug ===
FIX (committed+pushed ff4f78f): gp0_command_word_count + gp0_execute_command
NOP range extended from {0x03-0x1E,0xE0,0xE7-0xEF,0xFF} to include the WHOLE
undefined env tail >=0xE7. psx.wiki: env group 0xE0-0xFF defines only
E0(NOP)+E1-E6; E7-FF all undefined -> real HW ignores them. The game emits
0xFE1F220E (undefined env cmd) which runtime fatal-halted as "unknown".
Now NOPs faithfully -> game blew past frame634 halt, reached frame648.
NOTE: earlier "thousands of 0x03-0x1E NOPs" in frame634 ring are a per-prim
data table that decodes as harmless NOPs on real HW too — NOT the desync.
The REAL desync was the incomplete env-NOP range (0xF0-0xFE missing).

NEW BLOCKER (frame648): FAIL-FAST unknown dispatch addr=0xFFFF8040
phys=0x1FFF8040 ra(epc)=0x801265F4 a0=0x80192944. CPU gpr[8]=t0=0xFFFF8040.
0xFFFF8040 = SIGN-EXTENSION of 0x8040 -> classic signed-vs-unsigned 16-bit
load bug (lh/lb sign-extends 0x8040->0xFFFF8040 where lhu/lbu gives 0x8040),
OR a jump-table/function-pointer-table entry read wrong. epc=0x801265F4 is
frame_tick resume after jal 0x80167330 (big state-machine func, full_27.c);
the bad indirect target came from inside it or a nested call. NEXT: find the
jr/jalr producing 0xFFFF8040 — check for lh/lbu vs lhu/lbu sign-extension
on a 16-bit func-offset table, or a clean-C callback-table port bug.

=== SESSION9h: branch-likely interp gap found+fixed; new trampoline livelock ===
ROOT-CAUSE CHAIN for frame-648 dispatch fail: epc path = frame_tick
(0x801265F4) -> BIOS syscall/IRQ -> OpenBIOS low-RAM kernel trampolines
(0xB0-0xD0, dirty RAM) -> context-restore OpenBIOS_func_000029CC which does
lw k0,128(k1); jr k0 -> restored saved-PC was 0xFFFF8040 (sign-ext of 0x8040)
-> unknown dispatch. The frame's resume-PC field was corrupt.
interp_unsupported showed dirty_ram_interp hit insn 0x55050100 = BNEL
(opcode 0x15) at pc 0xD0 block_entry 0xC0, entry_ra=0x801265F4 (matched).
GAP: static recompiler supports branch-likely 0x14-0x17 (code_generator
is_likely, conditional delay slot) but dirty_ram_interp did NOT -> the
runtime-written kernel trampoline's BNEL aborted (abort_unsupported,
non-fatal) so the branch never executed -> corrupted exception-return path.
FIX (dirty_ram_interp.c): implemented BEQL/BNEL/BLEZL/BGTZL (0x14-0x17) with
recompiler-identical semantics (delay slot executes only when taken,
nullified otherwise); added them to dirty_ram_word_looks_decodable and to
the delay-slot control-transfer reject list. NOTE: R3000A is MIPS-I (no
branch-likely) but project recompiler already models them -> interp aligned.
RESULT: interp_unsupported midblock_count 2->0 (BNEL now executes). BUT the
failure MODE changed: instead of clean dispatch-fail@648 we now get a
LIVELOCK — dirty_block_tail shows millions of alternating dispatches
0xB0<->0xC0 (ra=0x801265F4 a0=0xFFFFFFFF) then signal_11 @frame647.
Interpretation: BNEL now runs correctly, so the trampoline executes its real
loop and spins on a condition that is corrupt upstream (the same root cause
that produced the bad saved-PC). NEXT: dump the 0xB0-0xD0 trampoline bytes +
the context frame k1 points to, find which flag/PC field stays wrong. The
livelock is real signal, not a regression — faithful BNEL exposed it.

=== SESSION9i: trampoline bytes decoded; livelock = stub<->handler spin ===
read_ram 0xA0-0xFF @frame629: the low vectors are JUMP STUBS, 16B apart:
  0xA0: li t0,0x2a80 ; jr t0   (0x24082a80 / 0x01000008)
  0xB0: li t0,0x2aa0 ; jr t0
  0xC0: li t0,0x2ac0 ; jr t0
  0xD0+: data/nops (0x00000100...) — NOT the 0x55050100 BNEL seen at crash;
  trampoline RAM is dynamic/re-written per state.
So dispatch_tail cycle 0x2AC0,0x1D84,0xB0,0x2AA0,0x40A0,0xC0 = stub->handler->
ret->next-stub. The livelock (dirty_block_tail, seq~5.15M, B0<->C0,
a0=0xFFFFFFFF) = a handler keeps re-entering the stubs -> a flag/queue the
handler waits on never clears. SAME upstream corruption that yielded the bad
saved-PC 0xFFFF8040. BNEL fix is faithful (matches recompiler is_likely);
the spin is the real signal. NEXT: identify which OpenBIOS handler 0x2a80/
0x2aa0/0x2ac0 is (likely IRQ/syscall dispatch), what flag it polls, and why
it never satisfies — probably a missing IRQ ack / event delivery / TCB state.

=== SESSION9j: livelock = A0/B0/C0 syscall vectors + ChangeThread scheduler ===
Syscall tables dumped (frame549): A0@0x200, C0@0x494C, B0@0x49CC — standard
PSX kernel tables. KEY: B0[23]=0x29cc = the context-restore/ChangeThread
syscall return. C0[10]=0x1d84 (the spinning C0 handler). Dispatch cycle
0x2AC0(C0disp)->0x1D84->0xB0stub->0x2AA0(B0disp)->~0x40A0->0xC0stub =
C0-syscall-handler triggers B0-syscall triggers C0 -> thread ping-pong.
Handlers are low-RAM kernel fns (0x1xxx-0x4xxx) + ROM (0xbfc0xxxx); 0x2afc
= not-implemented default. a0=0xFFFFFFFF = looping syscall arg (thread id?
priority -1? event?).
HYPOTHESIS: boot uses "thread scheduler = HLE (deterministic TCB)
(PSX_HLE_SCHEDULER)". The LLE ChangeThread path (interrupts.c ~1590-1720:
EPC=real resume PC, same-thread discriminator via PCB[0]/TCB, RFE-escape)
may be restoring the SAME thread's frame each switch -> a thread re-runs
its syscall forever instead of yielding to another -> livelock. Same root
as the 0xFFFF8040 bad saved-PC. NEXT: instrument the ChangeThread syscall
(t1/B0[23]) to log entry_tcb vs exit_tcb + resumed PC per switch; confirm
whether PCB[0] actually changes. If the HLE scheduler isn't rotating TCBs,
that's the bug — check psx_scheduler_* / deterministic TCB selection.

=== SESSION9k: deterministic frame-647 kernel livelock; stub "corruption" was post-mortem artifact ===
CORRECTION to SESSION9j hypothesis: the low-RAM "syscall stub corruption"
(0xA0-0x100 showing 0x55555555/invalid-opcode garbage) is a POST-MORTEM
artifact, NOT live corruption. Proven: armed wtrace on 0x80-0x140 for a full
run (incl. PSX_DEBUG_FMV_QUIET=0 so FMV quiet can't hide writes) -> ZERO
writes to the region in ~490 frames, and a tight read_ram(0xA0) poller saw
the clean stub 0x24082A80 (li t0,0x2A80; jr t0; nop; nop) right up to death.
The garbage bytes only appear when read AFTER the crash.
GP0(1Fh) fixed (commit 866e4a7): real IRQ1-request command, was fatal-halt
"unimplemented" at ~frame 611 -> now sets irq1_flag + raises IRQ_GPU(1).
DETERMINISM: BOTH schedulers now hit the IDENTICAL crash — fiber
(PSX_HLE_SCHEDULER=0) and HLE (default) both signal_11 at frame 647. So the
bug is in the GUEST kernel path, not the scheduler backend. thread_ctx_ring
stays EMPTY during the livelock -> thread switches run through the LLE
ChangeThread syscall (func_0x29CC), not the HLE TCB rotation.
Livelock signature (dispatch_tail, 6.8M dispatches): cycle
0xB0stub->0x2AA0(B0disp)->handler->0xC0stub->0x2AC0(C0disp)->0x1D84->0xB0,
also 0x40A0 in cycle; a0=0xFFFFFFFF; last_func=0x29CC.
Decoded handlers: func_1D84 = get/set table @0x64FC+idx*4; func_40A0 =
get/set kernel var @0xAEC8; func_2AA0/2AC0 = B0/C0 syscall-table dispatchers.
=> a kernel routine polls kernel variables via syscalls waiting on a
flag/event that never arrives (IRQ/timer/DMA/event delivery gap), spins
millions of dispatches, then signal_11 (host segv, not recursion — depth=1).
NEXT: identify which kernel variable the poller waits on and which IRQ/
event should set it. Compare vs Beetle oracle at frame ~647 for the exact
divergence (which interrupt/TCB stalls). Candidates: VBLANK handler not
reaching kernel flag, timer IRQ, DMA-done IRQ, or DeliverEvent not firing.

=== SESSION9l: frame-647 livelock decoded — syscall numbers + DispatchThread + IRQ health ===
Added t0-t2/a2-a3/sp to crash_trace dirty_block_tail (commit 49611f0) so the
post-mortem report carries the A0/B0/C0 syscall number (t1=gpr[9]) at each stub
dispatch. Re-ran to frame 647 signal_11 and read the tail.

SYSCALL NUMBERS AT THE LIVELOCK (dirty_block_tail, t1 field):
- frame_tick 0x801265F4 -> B0 stub t1=0x5B(91)  AND  C0 stub t1=0x0A(10), a0=0xFFFFFFFF
- game 0x8019AA9C (func_8019A8EC) -> B0 stub t1=0x17(23)=ChangeThread,
  via thunk func_8019AD60 (li t2,0xB0; li t1,23; jr t2). a1/a2=I_STAT/I_MASK
  addrs were leftover regs, not args.
- Kernel thunk family: func_8019AD60=B0#23, func_8019AD70=B0#24, func_8019AD80=B0#25,
  func_80199FE0=B0#91. Each is `li t2,0xB0; li t1,N; jr t2`.
- C0[10]=func_1D84 = atomic get/set on kernel table 0x64FC (lw old; sw a1 in
  delay slot). func_40A0 = get/set var 0xAEC8.

DISPATCHTHREAD = func_2810 (OpenBIOS). It is the kernel context-switch/dispatch:
  prologue saves ALL regs into TCB frame (s0-s7@64-92,t0-t9@32-60+96-100,
  gp/sp/fp@112-120,hi/lo@132-136; resume PC at +128 — matches func_29CC restore).
  Then loop @0x28F8-0x2964 walks linked list s6 (lw s6,0(s6)=next):
    s1(node)->v0 predicate (event flags); if v0!=0 && s0!=0 -> s0(v0) action;
    advance. predicate returns v0=0xF2000000 / 0xF0000003 (event flag masks).
  Spinning = list rescanned endlessly; either circular/corrupt next-ptr or
  every node's predicate keeps returning nonzero without a transition.

IRQ/EVENT HEALTH (event_ring.json, 65536 entries, seq 525474-591009):
  kinds: ISTAT_RAISE 28454, DEQ 16221, IRQ_DELIVER 16217, ENQ 4539, IRQ_GATE 105.
  UNIFORM across all 16 seq-windows — NO degradation/stall:
    IRQ0(VBLANK)~250x, IRQ2(CDROM)~1531x per window; imask=0xD (IRQ0,2,3 enabled).
  ENQ/DEQ sources: pc 0x80159AD0 dominant (kernel event loop), 0x80196FC4,
  0x8019AA98. func=0x29CC (context restore) on all — recorded during dispatch.
  IRQ_GATE all at pc=0x8019AA98 func=0x29CC = CD IRQ gated during ctx switch (normal).
CONCLUSION: kernel IRQ loop + event queue are HEALTHY and periodic. The livelock
  is a GAME thread blocked on a specific event the periodic IRQ loop never
  satisfies — NOT a dead IRQ path. signal_11 is a host segv AFTER the spin
  (dispatch_depth=1, stack_profile all-zero => host-side, not guest stack).
OPEN: which event/spec the game thread WaitEvent's on; whether DeliverEvent
  sends a mismatched spec, or a non-IRQ DeliverEvent source is missing.
  parity_trace (CHANGETHREAD/YIELD/SAVE/RESTORE ring) + Beetle oracle diff at
  frame ~647 is the mandated next step to find the exact divergence.

=== SESSION9m: CD-ROM ruled out; live livelock inspection; low-RAM pattern is ambiguous ===
Disk was 100% full (39x ~100MB psx_freeze_dump_*.json = 3.3GB) — deleted them,
freed 3.5GB. Rebuilt launch: build/BloodyRoar2_Recompiled --headless --no-launcher
  --disc <bloody-roar-2>/Game.cue --bios build/bios/openbios.bin
  (debug server 127.0.0.1:4370; HLE scheduler default).

CD-ROM ELIMINATED as the stall cause:
  cdrom_command_history: game runs SetLoc(0x02)->ReadN(0x06)->Pause(0x09)
  chunk-stream loop, all completing. dma_cdrom_history (newest, frames 630-645):
  every read completed=1 moved 512/512 through lba 479 @ frame 645, then reads
  STOPPED (game finished loading -> new phase). pending.cmd=0x09 was stale
  (pending.active=0 => Pause already completed). ring_dropped=516 is sector-ring
  overflow noise, not a stall. CD is HEALTHY; the livelock is post-load.

LIVE LIVELCK (process stayed alive at frame 648, stable — not instant SIGSEGV):
  dirty_block_log live tail = func_2810 dispatch loop PCs 0x2914-0x2964 cycling,
  t1=0xA(C0#10->func_1D84) t2=0x1D84, predicate returns a0=0xF2000000/0x1/0x8FD0,
  a1=0x400 const. func_2810 fully decoded = kernel DispatchThread: outer reload
  s6=*(s3) where s3=*(0x100); inner walks s6->next calling pred@8/action@4.
  Two-level rescan = kernel IDLE loop (normal when no thread ready).

irqctx_ring (IRQ delivery context): IRQs delivered normally thru ~frame 644,
  ~1 VBLANK(istat=1)+several CD(istat=4) per frame. take_pc always in GAME code
  (0x801265F4 frame_tick, 0x8018D0xx, 0x80192F14, 0x80196Cxx, 0x8019A460,
  0x80192634) + occasional kernel 0x1D84. EVERY entry same_thread=1 restored=1
  => single active thread; worker threads never scheduled => they starve.

LOW-RAM 0x0-0x400 PATTERN (read_ram live @648): repeating ~0x50B records with
  0x5505xxxx increasing vals, 0x4005xxxx COP0-looking, 0x555555 fill. This does
  NOT match the `li t0,X;jr t0` stub form, BUT B0/C0 syscalls still dispatch fine
  (func_2AA0/2AC0 run) => recompiler routes those PCs natively; RAM stub bytes
  may be irrelevant OR this is legit OpenBIOS kernel trampoline data. AMBIGUOUS —
  needs ground-truth compare vs canonical OpenBIOS low-memory image. NOT confirmed
  as the corruptor (earlier "clean@506 vs weird@648" may reflect a legit region
  I mis-assumed was stubs).

NET: kernel idle loop waits for a worker-thread wake event that never arrives.
  IRQs+event-queue healthy; CD healthy. Missing piece = WHICH event/spec the
  blocked thread waits on. parity_trace empty (switches via LLE ChangeThread not
  HLE hooks). Next: Beetle oracle diff @~645, or RE the game's CreateThread/
  WaitEvent specs to name the exact event.

=== SESSION9n: low-RAM "corruption" was MISDIAGNOSIS — it's legit tagged kernel tables ===
CORRECTION to session9m: the 0x0-0x8000 repeating pattern (0x4005XXXX/0x5505XXXX/
0x555555) is the LEGIT OpenBIOS kernel dispatch-table format, NOT corruption.
Evidence: values embed func addrs (0x400529CC->0x29CC ChangeThread,
0x40052610->0x2610, 0x40051E58->0x1E58, 0x55052AFC->0x2AFC notimpl-default).
Kernel runs fine 648 frames => tables valid; read_ram reads same ram[] as CPU
(phys&0x1FFFFF fold verified). Frame-593 "clean pointers" (0x25C8/0x40A0) vs
frame-648 "tagged" was a misread of which table/format — NOT a live overwrite.
ABANDON corruption theory (again). wtrace zero-hits was correct all along.

GAME SYSCALL MAP (39 B0/C0 thunks extracted from generated code):
  B0#7,8,9,10,11,12,18,19,20,21,22,23,24,25,50,51,52,53,54,66,67,69,74,75,76,
  78,79,80,91,92 ; C0#2,3,10.  B0#23=ChangeThread(func_29CC), #91=func_40A0
  get/set var 0xAEC8, C0#10=func_1D84 get/set table 0x64FC.

INPUT TEST: pad press (START/Cross via debug 'press') accepted but frame frozen
  at 648 => the game thread that reads pad is ASLEEP; only kernel idle runs.
  Not an input-wait livelock.

NET (unchanged): func_2810 DispatchThread idles; a worker thread stays blocked
  on an undelivered kernel event. IRQs+event-queue+CD all healthy. The specific
  WaitEvent spec / DeliverEvent source is still unidentified. Needs oracle diff
  or RE of the game's OpenEvent/WaitEvent/CreateThread call sites + specs.

=== SESSION9o: exception-vector trampoline fix LANDED (da96b5b); stall = thread-readiness, not events ===

FIX (committed+pushed da96b5b): psx_check_interrupts no longer blindly decodes
the RAM exception vector @0x80000080 as a Sony lui/addiu stub. OpenBIOS installs
a kernel-call TRAMPOLINE there (same 0x4005xxxx/0x??5555?? format InitSyscall
copies to A0/B0/C0). The old decode read w0=0x40050000/w1=0x27488040 ->
target=0xFFFF8040 wild dispatch (frame-648 FAIL-FAST). Now: validate the
lui/addiu/jr-t0 pattern (opcode+reg match, rt!=0); on match use the fast-path,
else dispatch to 0x80000080 so the trampoline EXECUTES (faithful, matches
interp_exception + the A0/B0/C0 fall-through path). Frame-648 crash GONE.

POST-FIX STATE (boot_skip path, bios_hle=true default):
  - frame counter climbs past 648 to 600K+ (VBLANK-driven; NOT real progress).
  - get_registers identical every sample: ra=0xBFC05E00 epc=0xBFC05784 =
    kernel DispatchThread IDLE loop. Game/worker thread NEVER re-dispatched.
  - gpu_state: gp0_writes=0, gpu_ring total=0 -> ZERO GPU cmds this run.
  - frame_fingerprint byte-identical across frames (pure idle).

STALL CHARACTERIZATION (new data this session):
  - bioscall_dump: B0#11 TestEvent=62661 (froze), B0#10 WaitEvent=0 !!!,
    B0#32 UnDeliverEvent=38, B0#23 ReturnFromException=22, B0#7 DeliverEvent=8,
    B0#8 OpenEvent=10, B0#12 EnableEvent=10. Game NEVER calls WaitEvent.
  - cycles_to_next_event=0xFFFFFFFF; timers/cdrom/dma/sio all none -> only
    VBLANK i_stat=0x1 pending. NOTHING scheduled to wake anything.
  - Kernel globals (phys): *(0x108)=TCBH@0x8FD0, *(0x110)=TCBbase@0x8FE0,
    *(0x114)=0x300 => 4 threads x 0xC0. TCB state @*(tcb): 0x4000=runnable.
  - HLE thread scheduler (PSX_HLE_SCHEDULER, default ON, traps.c) only switches
    to a TCB whose state==0x4000; else falls back / idles. Stall => no TCB is
    runnable. Suspect: worker thread's TCB state stuck non-0x4000 (never woken)
    OR scheduler not seeing the game-created threads.

LLE BOOT (PSX_BIOS_HLE=0, real intro) CRASHES DIFFERENTLY:
  - frame ~946/647, signal_11 in HOST console_emit (clean game/gfx.c:1734).
    console_emit writes a 32B line buffer @guest 0x801f68f0 indexed by count
    @0x801b6794; a corrupted/negative cnt -> OOB write -> host SIGSEGV.
    guest_at() maps these to valid RAM; the COUNT is garbage because the guest
    console state was never properly initialized (boot skipped / broken state).
    Symptom of the same underlying stall, surfaced as a host crash.
  - dispatch_tail at crash = same kernel idle pattern (0x40A0->C0->2AC0->1D84
    ->B0->2AA0). So LLE boot hits the SAME thread stall, earlier, then crashes.

NET: the frame-648 dispatch crash is FIXED. The remaining blocker is a
THREAD-READINESS stall: after boot the kernel has 4 TCBs but none runnable, so
DispatchThread idles forever. Not an event-wait (WaitEvent=0), not CD/timer/DMA.
Next: read the 4 TCB states+saved PCs at the stall (need a non-crashing window —
boot_skip idles stably but crashed once at 647; retry early-frame TCB dump),
then RE which B0 thread call (OpenTh/CreateThread/ChangeTh) leaves a TCB
non-0x4000, or diff vs Beetle oracle thread schedule.

=== SESSION9p: BREAKTHROUGH — game RENDERS + reaches intro FMV; stall moved to video-player event wait ===

SCHEDULER A/B (the decisive experiment):
  - PSX_HLE_SCHEDULER=1 (default, deterministic TCB): stalls EARLY, before any
    rendering. gp0_writes=0, game thread never dispatched, kernel idles on
    VBLANK. 4 TCBs, none state==0x4000 -> DispatchThread has no target.
  - PSX_HLE_SCHEDULER=0 (legacy host FIBERS): game ALIVE. epc=0x801265F4
    (game frame code), TestEvent climbs past 1.38M, GPU DMA ordering tables
    process (gpu_ot starts=245345 completes=127761), gp0_writes=57,335,865,
    real screenshot captured (320x240, ~95-133 unique colors = real art).
  => The HLE deterministic-TCB scheduler is the EARLY stall. The fiber path
     lets the game run. (Fiber path has known non-determinism landmines per
     CLAUDE.md, but it is the only path that currently reaches gameplay.)

NEW STALL (fiber path, much later — frame ~300K+):
  - Game boots, initializes, GPU-renders, then plays the INTRO FMV via MDEC.
  - mdec_state: command=0x32000780, decode_macroblocks=17477,
    decode_blocks=447282, dma_in_words=113728, dma_out_words=155520,
    output_pos=622080 / output_size=13422336 (~4.6%), busy=0, input_count=0,
    decode_input_pos=3796/3840. MDEC IDLE — finished a slice, waits for the
    game to issue the NEXT decode command.
  - cdrom_state: has_disc=1, reading=0, read_msf=0:08:30 seek=0:08:12,
    ring_dropped=516 (unconsumed ring slots overwritten — possible stream
    corruption under fast delivery, TBD benign vs bug).
  - dma_state: ALL channels active=0. gpu_ot active=0, starts/completes/
    cancels FROZEN (no GPU work submitted). Game fully parked.
  - get_registers: ra=epc=0x801265F4 (return addr after `jal 0x80167330`),
    sp=0x801FFFC0, t0=0xFFFF8040 (leftover kernel scratch — HARMLESS, kernel
    repurposes t0; NOT the old wild-dispatch bug), cause=0x400 (IP2 pending),
    i_stat=0x1 i_mask=0xD.
  - bioscall_dump (fiber run): total=1,725,912 calls. B0#23 ChangeThread/
    RFE=6202 (real yields), B0#11 TestEvent=1,383,560 (froze), B0#32
    UnDeliverEvent=499, B0#8 OpenEvent=19, B0#12 EnableEvent=19. Only 19
    events opened — game uses few kernel events.

WHERE IT PARKS: func 0x80167330 (br2eu_func_80167330, 1296B/82 blocks) is the
game's scratchpad (0x1F800000) packet-builder / GPU-DMA submission routine —
it sets s0=s1=0x1F800000 and stages GPU packets there. The game thread called
it and never returned: blocked inside on a kernel wait for a completion event
(DMA-done / frame-consumed / semaphore) that never gets delivered. Same ROOT
CLASS as the HLE-scheduler stall (kernel event/thread not delivered), just
reached later via the fiber path.

SCRATCHPAD[0]=0x06 already equals the polled value (the lbu/bne-6 loop at
0x801265F4 is the CALLER's post-call code, not the block point). The real block
is inside 0x80167330 awaiting an event.

NET: recompilation is PROVEN through boot -> init -> GPU render -> intro FMV.
Remaining blocker = kernel event/thread delivery in the video-player path
(a worker/driver thread that should DeliverEvent on DMA/VBLANK-done isn't
scheduled or its event isn't hooked). Next: identify which TCB/thread should
wake the parked thread — read TCB states+saved EPCs in the fiber run, or trace
the DeliverEvent/UnDeliverEvent (B0#7/#32) pairing around the park.

=== SESSION9q: stall ROOT-ISOLATED — FMV video-player frame-sync poll (ChangeClearRCnt+ChangeClearPad), MDEC starved ===

REFINED DIAGNOSIS (both boot paths converge — faithful, deterministic):
  - PSX_BIOS_HLE=0 (real LLE intro) + PSX_HLE_SCHEDULER=0 (fibers) reaches the
    IDENTICAL state as boot-skip: ~57.3M gp0_writes, parks at epc=ra=
    0x801265F4. => stall is NOT a boot-skip artifact; it is a real
    video-playback path bug.

LIVE LOOP (bioscall_dump tail + dirty_insn_log):
  - Game spins alternating C0#10 ChangeClearRCnt(0xFFFFFFFF) <-> B0#91
    ChangeClearPad(0xFFFFFFFF), ra=0x801265F4, frame stuck ~946. Thousands of
    iterations, all a0=0xFFFFFFFF. This is the game's per-frame
    timing+input poll — it waits for a root-counter/frame advance that never
    arrives. NOT a kernel thread-block: CPU busy in interpreted code, kernel
    only idles on VBLANK.

HARDWARE/IRQ STATE at stall:
  - i_mask=0xD (VBLANK+CDROM+DMA only). imask_trace shows it only ever toggles
    0xD<->0x000 — game does NOT use timer IRQs (bits4-6 stay masked).
  - event_ring: ONLY IRQ_GATE/IRQ_DELIVER VBLANK repeating, returning to
    0x801265F4. No timer/CD/DMA/SIO IRQs pending or firing.
  - timers_state: all 3 counters increment (hardware timers tick fine).
  - mdec_state: FROZEN — output_pos=622096/13422336 (~4.6%), decode_macroblocks
    =17477, busy=0, input_count=0, decode_input_pos=3796/3840 (buffer full,
    not consumed), dma_in_words=113728. MDEC IDLE waiting for the NEXT decode
    command the game never issues.
  - cdrom_state: reading=0, read_msf=0:08:30 seek=0:08:12, ring_dropped=516,
    sector_available=1. CD not actively reading.
  - gpu_ot/dma: all channels active=0, no GPU work submitted.
  - pad_buttons=0xFFFF (slot1 all-ones — possible phantom/2P pad anomaly).

CAUSAL MODEL: the intro FMV player decoded ~4.6% of the video (17K macroblocks
-> 622KB of 13.4MB), then the game's video loop entered the
ChangeClearRCnt+ChangeClearPad frame-sync poll and never advanced to feed the
next MDEC command. Most likely the video elementary stream stalled — either
the next CD sector/chunk isn't being read (reading=0, ring_dropped=516 may have
corrupted the ring) or a frame-present/DMA-done flag the poll waits on is never
set. frame:946 = the game's video-frame counter frozen.

NEXT (needs oracle or deeper RE):
  - Determine whether the game is waiting on (a) CD data for the next video
    chunk, (b) a DMA-done/GPU-present semaphore, or (c) the BIOS root counter
    (RCnt) value that the VBLANK handler should increment. Read the RCnt
    variable in kernel RAM across a VBLANK to see if it advances; check the
    game's hooked VBLANK/DefInt handler is actually running and incrementing
    the frame counter the poll reads.
  - The 0x80167330 function is the scratchpad packet/video submit routine;
    its callees 0x80163868 (BIOS-call thunks), 0x80166398, 0x80129F78 lead to
    the ChangeClearRCnt/ChangeClearPad poll.

=== SESSION9r: stall deepened — OpenBIOS guest-thread livelock, no host fiber switches ===

NEW EVIDENCE (fiber scheduler, real LLE boot, frame ~1.9M host / game stuck ~946):
  - thread_trace + thread_ctx_ring BOTH total:0 despite 6203 ChangeThread bioscalls.
    => psx_change_thread_fiber NEVER runs. OpenBIOS's own changeThread
    (recompiled C, elf 0xBFC06924) performs thread switches purely in guest RAM
    via its own scheduler — NO host fibers involved. The runtime just runs
    whatever guest PC the kernel dispatches.
  - dirty_insn_log tail: CPU spins INSIDE the OpenBIOS low-RAM kernel-call gate
    at phys 0xB0: synthesized `addiu t0,a0,0x2AA0; jr t0` looping continuously,
    with current_tcb alternating 0x400500FE <-> 0x00008FE0 and ra alternating
    0x801265F4 <-> 0x8019AA9C (two game call sites ping-ponging through the gate).
  - dispatch_stats.static_hits FROZEN at 12692180; fn_stats.direct_seen FROZEN at
    14063929; bioscall total frozen at 1704030 (last = C0#10 ChangeClearRCnt).
    => ZERO compiled-game dispatches and ZERO new bioscalls. The game thread is
    fully suspended; only the kernel/VBLANK idle path executes (frame_fingerprint
    pc-hash identical every frame).

OPENBIOS KERNEL MAP (from recompiler/seeds/openbios_elf_seeds.json — elf syms):
  changeThread=0xBFC06924 openThread=0xBFC0688C closeThread=0xBFC068F8
  getFreeTCBslot=0xBFC06834 initThreads=0xBFC06728 init_main_thread=0xBFC0019C
  openEvent=0xBFC05648 closeEvent=0xBFC05724 testEvent=0xBFC0574C
  waitEvent=0xBFC0578C deliverEvent=0xBFC205E4 undeliverEvent=0xBFC206B8
  enableEvent=0xBFC056B4 disableEvent=0xBFC056EC getFreeEvCBSlot=0xBFC05600
  initEvents=0xBFC05578 enqueueRCntIrqs=0xBFC1FCB4 waitVSync=0xBFC0BAA8

ROOT-CAUSE HYPOTHESIS (strong):
  - OpenBIOS.toml itself flags the EvCB/TCB ABI as "believed compatible — but
    believed is not verified", and deliver_event_ret anchor is ABSENT pending
    B0-event validation. The scheduler (traps.c) reads TCB globals at hardcoded
    SONY SCPH1001 offsets phys 0x108(PCB)/0x110(base)/0x114(size); under
    OpenBIOS those phys words decode to 0x4005xxxx/0x488040 trampoline/code
    descriptors, NOT clean pointers — i.e. OpenBIOS stores its kernel globals at
    DIFFERENT addresses. Guest thread state therefore cannot be read reliably.
  - Combined with the livelock: a guest thread yielded (ChangeThread) into a
    state the kernel scheduler can't wake — most likely waiting on an event
    (frame tick / CD-data-ready / MDEC-done) that OpenBIOS's DeliverEvent path
    does not raise, OR a TCB state word the runtime misreads.

NEXT (concrete, faithful):
  - Disassemble OpenBIOS changeThread/DispatchThread/testEvent/waitEvent +
    deliverEvent in generated/OpenBIOS_full.c to recover OpenBIOS's ACTUAL
    TCBH/TCB/EvCB global addresses and state encoding; correct the scheduler's
    hardcoded Sony offsets OR key them per-BIOS (OpenBIOS.toml runtime_exports).
  - Verify OpenBIOS DeliverEvent actually enqueues the game's event flags; the
    19 OpenEvent/EnableEvent + 62 DeliverEvent + 1.36M TestEvent pattern says the
    video thread polls an event flag that should be set per-frame.
  - Beetle/DuckStation oracle (psx-dsoracle) is the definitive diff tool but is
    MSVC/Windows-only (tools/build_oracle.sh) — not runnable on this Mac.

=== SESSION10: interp reproduces identical stall -> NOT a recompiler bug; GPU OT DMA is the mechanism ===

DECISIVE TEST (PSX_FORCE_INTERP=1, same binary, port 4371):
  - The dirty-RAM interpreter backend reproduces the IDENTICAL stall: same
    ra=0x801265F4, sp=0x801FFFC0, s1=0x1F800000, mdec_decode_count=17477,
    cd_irq_delivered=1086, bioscall poll counts (B0#91=167018, C0#10=167015).
  - => The bug is in SHARED hardware simulation / OpenBIOS guest kernel, NOT
    compiled-code generation. Both backends compute byte-identical wrong state.

ROOT-CAUSE MECHANISM (verified via generated game code + OpenBIOS source):
  - Stall PC function br2eu_func_801265E4 (game): jal 0x80167330 (GPU packet
    submit) -> lbu s0,[s1=0x1F800000] -> require scratchpad[0]==6 -> write 3 ->
    loop to 0x80126344. scratchpad[0] is a DMA/GPU-completion handshake flag.
  - Two guest threads ping-pong through the 0xB0 kernel gate: thread A
    (ra=0x801265F4, frame/render) and thread B (ra=0x8019AA9C, calls B0#23 =
    returnFromException = IRQ/exception return path). Cooperative yield loop,
    neither progresses.
  - scratchpad[0] currently reads 0x06 (precondition met at snapshot), but the
    steady-state handshake for the NEXT packet never completes.

GPU OT DMA PATHOLOGY (dma_state, faithful baseline - widescreen ws.active=0):
  - Channel 2 = GPU ordering-table linked-list DMA. gpu_ot stats:
    starts=245345 completes=127761 cancels=117584.
  - nodes_max=262144 = the 0x40000 node CAP; words_max/cycles_last=9.7M.
    A single OT transfer walks ~9.7M words / ~9.7M cycles (~17 PSX frames of
    GPU time) because the walker hits the node cap instead of finding the
    0x00FFFFFF end-of-OT terminator (dma_gpu_ll.c: next_addr==0xFFFFFF).
  - gpu_ll_complete->complete_transfer(2) raises the DMA-done IRQ on natural
    finish; cancel_async_transfer(2)->dma_gpu_ll_cancel fires when the guest
    clears CHCR bit24 (GPU_abort / explicit stop) and raises NO irq.
  - OpenBIOS gpu.c: GPU_sendPackets->GPU_sync waits for DMA_CTRL[DMA_GPU].CHCR
    bit24 (busy) to clear, 0x10000000-iter timeout -> GPU_timeout->GPU_abort
    writes CHCR=0x401 (cancels the in-flight OT). 117K cancels ~= GPU_sync
    timing out / games aborting a perpetually-backed-up GPU.

DEADLOCK SHAPE:
  - Render thread submits OT -> GPU falls ~17x behind -> GPU_sync timeout ->
    GPU_abort cancels -> packet never signals done -> scratchpad[0] handshake
    stalls -> frame loop spins. MDEC+CD go idle (video stream starved) because
    the render/consume side never drains decoded frames -> producer-consumer
    deadlock. Only VBLANK IRQ fires (i_stat=0x1, i_mask=0xD = VBLANK+CDROM+DMA).

OPEN QUESTIONS / NEXT:
  - Is nodes_max=262144 an OT-parsing bug (walker misreads header>>24 word_count
    or header&0xFFFFFF next_addr, or misses the terminator) or a legitimately
    huge OT? Read the actual OT the game built (channels[2].madr resolved) and
    hand-walk first nodes to check for a 0xFFFFFF terminator the parser misses.
  - Confirm whether completes are natural-end (hit_limit=0) or cap-hit
    (hit_limit=1); channels[2].madr=0xFC residue suggests cap-hit.
  - If OT parsing is faithful, the faithful fix is GPU OT DMA timing (cycles/
    word + node-walk cost) vs real hardware — cross-ref psx-spx/Beetle OT rate.
  - Oracle still needed for ground truth; DuckStation macOS GUI boot still
    blocked pre-BootSystem (Qt/cocoa), Beetle oracle is Windows-only.

================================================================
SESSION 11 — dirty-RAM boot crash root-caused + FIXED (noinline);
           game now boots to menu/save; next blocker = clean_bridge
           wild cpu pointer.
================================================================

FRESH-BUILD BOOT CRASH (blocked all runtime work):
  - Symptom: SIGSEGV in dirty_ram_dispatch at dirty_ram_interp.c:2487
    reading s_last_dirty_irq_pump_insns via ldr x9,[x28,#0x680].
    Fault addr 0x8001f22080010680 = two packed guest KSEG0 addrs.
    x28 (callee-saved, used as adrp base for file-statics) held
    guest-derived data.
  - Reproduced identically under PSX_FORCE_INTERP=1 and both
    scheduler modes -> shared dirty-RAM path, not codegen, not BIOS.
  - Regenerated OpenBIOS via psxrecomp-bios --config bios/OpenBIOS.toml:
    output BYTE-IDENTICAL to in-tree copy -> the CMake "stale emitter"
    warning is only an emitter-source hash drift, NOT wrong BIOS code.
    Suspect eliminated.
  - ROOT CAUSE: dirty_ram_dispatch_inner is inlined into
    dirty_ram_dispatch by -O2 (confirmed via lldb: lr shows
    "dirty_ram_dispatch_inner + 192" inlined). The interpreter body
    leaves a guest-derived 64-bit value live in callee-saved x28 across
    the psx_check_interrupts setjmp/longjmp boundary; on the normal
    return path the wrapper reads file-statics through x28 as an adrp
    base -> host SIGSEGV.
  - FIX (verified): __attribute__((noinline)) on dirty_ram_dispatch_inner
    gives it a real call frame; callee-saved regs restore per ABI and
    the wrapper's static base stays intact. Game now boots past OpenBIOS
    into menu/save code (previously died ~40s at boot; now alive 50s+
    and reaches clean-source battle_intro_flow/menu/save).

NEXT BLOCKER (not yet fixed): clean_bridge wild cpu pointer.
  - After boot fix, crash moved to br2_bios_tramp (clean_bridge.c)
    reading cpu->gpr[] with g_clean_cpu = 0x16f5555fa — a MISALIGNED
    stack-region pointer (not a valid 8-aligned CPUState*).
  - Backtrace: battle_intro_flow(menu.c) -> menu_res_load_sm(save.c)
    -> save_load_state_machine -> cd_timer_read(cd.c) -> vsync_wait_n
    -> br2_bios_tramp(table=0xB0,svi=91). Pure native C calls inside
    clean-room game code.
  - g_clean_cpu is set ONLY by clean_try_dispatch (clean_table.c) from
    psx_game_backend's dispatch cpu. The cpu arriving is already wild,
    so the corruption is UPSTREAM of clean dispatch (a stack CPUState
    snapshot cpuN/snap in overlay_loader/savestate/rewind/selfcheck, or
    a corrupted cpu pointer in the dispatch chain). Tried scoping
    g_clean_cpu save/restore in clean_try_dispatch — did NOT fix (cpu
    is bad before dispatch). Reverted (unproven).
  - OPEN: trace which stack CPUState reaches clean_try_dispatch during
    the save/load path; candidate sources overlay_loader.c cpuN/cpuI
    (shadow-diff native pass), savestate.c snap, psx_rewind.c snap,
    psx_selfcheck.c dig_cpu. s_native_exec=1 (shadow diff) is ON by
    default — PSX_OVERLAY_NATIVE_OFF=1 disables it (untested as fix).

BUILD NOTE: two runtime targets — psx-runtime (PAL ->
BloodyRoar2_Recompiled) and psx-runtime-us (USA ->
BloodyRoar2_Recompiled_USA). The -us variant FAILS to link:
undefined _cd_feb00_trace_dump/_cd_seek_dump (clean game symbols the
USA target doesn't compile). Pre-existing, unrelated to this work.

================================================================================
SESSION 12 — clean_bridge crash ROOT-CAUSED + FIXED; GPU OT stall narrowed to
            one pathological frame-661 OT (format verified vs psx-spx)
================================================================================

CLEAN_BRIDGE g_clean_cpu CORRUPTION — ROOT CAUSE FOUND + FIXED (0767f60)
  - Symptom: br2_bios_tramp SIGSEGV, g_clean_cpu = 0x16f5555fa (misaligned
    stack value). gpr[] is CPUState's first member so cpu->gpr[4]=cpu+16=
    0x16f55560a = exact fault addr.
  - KEY EVIDENCE: nm showed .bss layout — g_openbus[0x10000] ends at
    0x13a28ec00 and g_clean_cpu sits at EXACTLY 0x13a28ec00 (immediately
    after). guest_at() returns g_openbus + (addr & 0xFFFF) for any
    MMIO/open-bus address, but clean code treats the result as a
    struct/array and indexes it by UNMASKED field offsets (and some paths
    memcpy variable lengths through it). A guest-indexed store reaching
    offset >=0x10000 writes straight into g_clean_cpu.
  - Confirms why a cpu-pointer watchpoint / setter diagnostic never fired:
    the pointer was VALID when stored; the static was overwritten later by
    an unrelated guest-indexed openbus access.
  - FIX: g_openbus 0x10000 -> 0x100000 (1 MiB). .bss is lazily committed so
    cost is virtual only. Verified: clean dispatch ran 11,490,361 calls
    with zero corruption; game progressed past menu/save to the GPU-OT
    stall instead of crashing. Pushed 0767f60.

GPU OT / scratchpad[0]==6 STALL — narrowed (was "frame-944 FMV stall")
  - With clean_bridge fixed the game BOOTS, runs clean source, renders
    ~647 frames, then stalls in the main state machine
    (game/game.c switchD_80126370, ra=0x801265F4, s1=0x1F800000 scratchpad,
    t9=0xB0) inside frame_advance_helper's frame-completion poll.
  - get_registers: cop0_epc=ra=0x801265F4, sp=0x801FFFC0 — same signature
    as the earlier producer/consumer stall.
  - VERIFIED CORRECT vs psx-spx/problemkaputt:
      * CHCR bits[9:10]=SyncMode (0x01000401 -> bit10 -> sync_mode=2 =
        linked-list/OT). bit0=direction(RAM->dev), bit24=start/busy.
      * OT node = count:8 | next:24, next=low24 of phys byte addr,
        0xFFFFFF=end. resolve_address (canonical&0x00FFFFFC then
        &0x001FFFFC) folds to 2MB RAM correctly.
      * dma_gpu_ll.c walker terminator check next_addr==0x00FFFFFF present
        in both word_count==0 header path and post-payload path.
  - dma_trace_dump (256-entry window, frames 647/661):
      * 16 ch2 OT DMAs at frame 647 ALL completed clean (madr->0x00FFFFFF).
      * ONE ch2 complete at frame 661 with madr=0x00050000 = hit_limit
        wander end (gpu_ll_complete sets madr=current_addr on node-cap,
        0x00FFFFFF on clean terminator).
      * ch2 bases are KSEG0 0x801E618C/0x801E620C/0x801E82DC/0x801E40BC
        (phys 0x1E....). The 0x50000 "GPU command" data seen earlier is
        just where the wander ended, NOT the OT base.
      * Buffer at 0x1E618C IS a valid OT: sparse node headers
        (count~0x24-0xF8) with GP0 command payloads between them.
  - CONCLUSION: OT walking is correct for 16/17 transfers. The frame-661
    OT sends the walker into a garbage chain -> node-cap (0x40000) ->
    ~9.7M cycles -> GPU_sync timeout -> GPU_abort cancels -> but abort
    DOES call dma_gpu_ll_cancel (dma.c:410-424), so lifecycle is clean.
    The "complete" fired via hit_limit->complete_transfer->IRQ, yet the
    game's scratchpad[0] never reaches 6 -> frame_advance_helper spins.
  - HYPOTHESES for the divergent OT (need failing OT's live node chain):
      a) a next pointer resolving to wrong memory (scratchpad/MMIO fold,
         or off-by-N) -> reads command buffer as tags -> self-sustaining
         garbage chain;
      b) walker reads a concurrently-rewritten OT (game builds next
         frame's OT in same buffer while event-driven walk still runs) —
         timing-faithfulness issue, would explain why only the large/complex
         frame-661 (first real 3D battle scene) diverges;
      c) a genuine OT cycle (node A->...->A) from game reusing entries.
  - OPEN: capture the failing OT's first bad next pointer. The transfer
    already completed (hit_limit) so its start is outside the trace
    window; needs a targeted watch on dma_gpu_ll_update for the
    frame-661 walk, or RAM snapshot + offline re-walk.

BUILD NOTE (unchanged): psx-runtime-us fails to link (undefined
_cd_feb00_trace_dump/_cd_seek_dump) — pre-existing, unrelated.

SESSION 12b — OT stall is NONDETERMINISTIC + ot_trace diagnostic added
  - A second instrumented run stalled EARLIER (frame ~444, ra=0x282C low
    memory, clean count frozen) — a DIFFERENT stall point than frame-661.
    The runtime has multiple/nondeterministic stall points; which one a
    run hits varies (timing-sensitive).
  - ot_trace instrumentation could NOT capture the failing OT chain this
    session because the run stalled before reaching frame-661.
  - ADDED (uncommitted->committed as opt-in diagnostic): dma_gpu_ll.c now
    keeps a 256-entry ring of recent (node_addr -> next_addr) hops and
    dumps it to stderr when a walk hits the 0x40000 node cap. Enabled with
    BR2_OT_TRACE=1 (default OFF = ~zero overhead). NEXT STEP: run with
    BR2_OT_TRACE=1, reach a node-cap stall, read the dumped chain to find
    the first divergent next-pointer (cycle vs garbage region vs wrong
    resolve). This is the decisive datum for the OT stall.

SESSION 12c — earlier DISTINCT stall found at frame ~444
  - With BR2_OT_TRACE=1 the run stalls at frame ~444 (ra=0x0000282C,
    cop0_epc=0x00002924, low memory/kernel region; clean count frozen).
    Reproduced identically across two instrumented runs. This is a
    DIFFERENT, EARLIER stall than the frame-661 GPU-OT node-cap — the OT
    trace never fired (not a ch2 walk divergence). Likely a kernel/BIOS
    or low-memory wait (0x28xx/0x29xx = exception/idle region). The
    nondeterministic stall frontier means boot->menu has MULTIPLE
    timing-sensitive waits; frame-444 may be the more common first blocker.
  - NEXT: identify the 0x282C/0x2924 wait (disasm that region / check if
    it's a vsync/kernel idle loop vs a device-ready poll).

SESSION 13 — frame-444 crash ROOT-CAUSED + FIXED (clean dispatch unblocked)
  - The frame-444 "low-memory stall" was NOT a wait — it was an OpenBIOS
    interrupt-dispatcher AdEL (epc=0x2924, cause=0x410) caused by clean
    source corrupting kernel globals at phys 0/0x100 (handler-table base
    *(u32*)0x100 -> 0, *(u32*)0x0 -> 0x80010003).
  - ISOLATION: generated-only (CLEAN_MIN=1 CLEAN_MAX=0) runs to frame
    100K+ — clean dispatch was the SOLE blocker. Low-cluster-only clean
    (0x8012-0x8018) reproduced the crash; cd_channel_poll ruled out.
  - ROOT CAUSE: clean ports intermittently pass guest address 0/tiny to
    guest_at() — NULL-deref port bugs. Traced via a temporary
    guest_at(low-phys) diagnostic + __builtin_return_address to the exact
    C call sites: text_render_simple (battle.c:12389, descriptor +0x1c
    text ptr=0), cd_chan_attr_push/mode_*/spr_draw_* (NULL channel/sprite
    bases). A stray clean store through RAM[0] smashed the kernel tables.
  - FIX (cd1e492): guest_at() now routes raw guest addr < 0x10000 to a
    zeroed g_low_dummy[0x10000] — reads see empty data, writes absorbed.
    Legit KSEG0/scratchpad/KUSEG-high pointers unaffected. Verified: clean
    dispatch runs to frame 260K+ with real 3D rendering (25M GP0 writes,
    140K draws), matching the generated-only baseline.
  - CAVEAT: this neuters the NULL-deref CLASS; the upstream NULL producers
    (text ptr, cd channel bases) still need individual port fixes — they
    now fail safe instead of corrupting the kernel.
  - NEXT: (a) hunt the upstream NULL producers now that crashes are gone;
    (b) confirm long-run stability + the frame-661 GPU-OT node-cap is the
    remaining faithful-timing frontier (BR2_OT_TRACE=1 ready).

SESSION 13b — upstream NULL-producer mechanism identified (guest_of round-trip)
  - Clean dispatch now stable to frame 236K+ (verify run, port 4406).
  - The guest_at(0) NULL derefs trace to hud_text_blk_alt's
    `_DAT_1f8003a8 = guest_of(param_3)`: param_3 arrives as a HOST pointer
    (callers pass `(const void*)guest_at(pt)` via BR2_E078, menu.c:5412).
    guest_of() only reverses RAM/scratchpad pointers — returns 0 for
    host string literals / non-RAM arenas -> text ptr = 0 -> guest_at(0).
  - Same class likely feeds cd_chan_*/spr_draw_* NULL bases: a guest
    pointer that should resolve to RAM comes back 0 because the producer
    stored a host-only pointer or an unmapped guest addr.
  - NEXT (real port fixes, now fail-safe under the guard): audit every
    `guest_of(...)` result used as a stored guest pointer; ensure string/
    struct data lives in guest-mapped RAM (or add a host-string arena
    guest_of can reverse). Priority: text/HUD announce path (menu.c
    BR2_E078 callers), CD channel attr bases.

SESSION 14 — frame-sync stall ROOT-CAUSED: OT next-links held host pointers (40e9bbf)
  - The "frame advancing but GP0 frozen" stall was NOT a wait: get_registers
    showed pc=0x00000000, ra=0x801265F4. GPU packets (0x4005/0x5555 GP0 words)
    had overwritten the game data/BSS region — the GPU func-table ptr at
    0x801b8318 read packet bytes instead of 0x801b82d8, so the next
    gpu_call->gpu_invoke ran br2_call_generated on a garbage fn -> pc=0.
    The "frame" counter kept bumping only because the VBLANK IRQ is
    independent of the halted main loop.
  - dma_state showed gpu_ot nodes_last=262144 (the hard cap) and
    cancels=117583: the OT linked-list walker followed a `next` chain into
    arbitrary RAM until it self-looped at addr 0.
  - ROOT CAUSE (clean-port bug): battle.c built OT `next` link fields as
    `(uintptr_t)host_ptr & 0xffffff` — the low 24 bits of the HOST address,
    not the guest address. The DMA walker resolved those as guest addrs ->
    garbage nodes -> node-cap -> abort -> packet buffer overran into BSS.
  - FIX (commit 40e9bbf): 23 link-store sites in battle.c now use
    guest_of(ptr) & 0xffffff (matching the gfx.c idiom). Affected:
    battle_hud_draw, rect_prim_tick, sprite packet builders
    (puVar*/local_78/psVar9/&DAT_801f4cbc operands). Guest-address-int
    operands (iVar1, uint param_N) were already correct, left unchanged.
  - SAME BUG CLASS still open (clean-source backlog): ~64 sites store
    `(undefined4)(uintptr_t)host_ptr` (full low-32 host addr, not masked)
    into guest pointer/link fields — task lists, packet chains, object
    links. Needs the same guest_of treatment but each operand must be
    verified host-ptr-into-RAM vs guest-addr-int. A safe universal helper
    br2_gaddr(v)=guest_of(v)?guest_of(v):(uint32_t)v handles both cases.

SESSION 15 — host-pointer->guest-address leak sweep (br2_gaddr rollout)
  - Continued the SESSION-14 bug class: `(undefined4)(uintptr_t)host_ptr`
    stores the low-32 bits of a HOST pointer where the guest expects a
    guest address. Added `br2_gaddr(const void*)` in clean_bridge.c:
    guest_of(p) if p maps into guest RAM/scratchpad, else (uint32_t)p —
    so it is a strict no-op for values already holding guest addresses.
  - Applied br2_gaddr to 52 unambiguous sites in game/battle.c across three
    commits (8f493b5, ae41866, 1da85bd, 5fec9ec):
      * bare pointer-var stores into guest memory fields;
      * function-argument forms (`,`/`)`-terminated puVar/piVar);
      * parenthesized pointer+element-offset `(puVarN + M)` link stores;
      * `&DAT_xxx` operands (resolve to host ptr into guest RAM);
      * pointer-typed `param_N` (short*/undefined4* host pointers).
    Redundant double `(undefined4)(uintptr_t)` casts collapsed.
  - DEFERRED (ambiguous, per-site review required):
      * `(uintptr_t)ptr + N` where +N is OUTSIDE the cast — integer byte
        offset vs element offset is not decidable from the decompile;
      * `(intptr_t)ptr + off` forms — integer arithmetic, br2_gaddr's
        const void* signature cannot take them without a cast;
      * `(&PTR_DAT_xxx)[idx]` indexed pointer-table lookups (1 site);
      * `(type*)(uintptr_t)` reverse-direction casts (guest-addr->host-ptr
        for callees that re-guest_at) — correct-by-convention in some
        places, `&local_48` host-stack cases differ;
      * `(int)/(uint32_t)(uintptr_t)` casts feeding guest_at() address
        args — already correct.
  - VERIFY: rebuilt psx-runtime cleanly; detached run PID on :4413 with
    BR2_OT_TRACE=1 exercising the new binary. Prior OT-fix run reached
    frame ~9351, clean_trace 723M, 0 HIT_LIMIT/FATAL.

SESSION 15b — demo-scene CD load livelock (game_state 4, _DAT_801f4740 never set)
  - With the host-pointer fixes live, the game boots cleanly into
    g_game_state=4 = scene_entry(0x80010000,0,1) demo/attract fight load.
    It never reaches state 5 (match_loop) because scene_entry's do/while
    only exits on _DAT_801f4740==1 (all scene data loaded) or a skip
    button; the flag stays 0 forever.
  - _DAT_801f4740 is only ever written =0 in clean source; the =1 setter
    lives in generated code (CD/stream completion path).
  - cdrom_command_history shows a repeating ~36-frame cycle:
    Seek(MSF 46:26:29) -> Setmode(0) -> Setmode(0xE0) -> ReadN(0x1B),
    reading ~1.5s of sectors (read_msf advances 46:26:29 -> 46:27:61)
    then seeking BACK to 46:26:29 and repeating. Classic read-retry
    livelock: data is delivered but the game never accepts/validates it.
  - cdrom_state: CD INT1 flags DO fire (irq_flag 0x03 in cmd history),
    reads progress, but s_ring_dropped climbs ~600/s (8-sector ring,
    guest a full ring behind -> unread slots overwritten = real loss for
    whichever stream is starving; may be benign XA audio or fatal data).
  - Suspects: (a) CD sector data reaching wrong guest dest / decode
    mismatch so validation fails; (b) ring-buffer consumer (MDEC/DMA ch0
    or CD-ch3) too slow -> drops corrupt the stream; (c) completion
    handshake the generated setter waits on never satisfied.
  - Input injection WORKS: pad_status shows both pads connected,
    press{buttons:65527(Start=0xFFF7),frames:N,slot} sets override and
    slot0 reports 0xFFF7 — but state-4 scene load ignores Start (only
    the 0x850 skip mask in the do/while would break it, and that path
    also needs the load to have started).

SESSION 15c — CD livelock refined: consumer drain path + double-speed
  - dma_state ch3 (CD-ROM DMA) chcr=0x00000000 = DISABLED -> the game
    drains CD data via BFRD programmed-I/O polling (cdrom_dma_read /
    cdrom_dma_ready on request_reg&BFRD), not DMA bursts.
  - ring_starved=0 (drains never find an empty slot) while ring_dropped
    climbs ~600/s -> the BFRD poll consumer is keeping up JUST enough to
    avoid starvation-empties but NOT fast enough to prevent the 8-slot
    ring overwriting unread sectors. Net: steady real data loss.
  - Stuck sectors are Mode2/Form2 whole-sector (mode_reg=0xE0: bit7
    double-speed + bit5 whole-sector + bit6) -> ~150Hz production. The
    game set this mode itself, so it expects 2x cadence; our production
    honours it, but the consumer path (BFRD poll -> MDEC/XA decode) is
    not draining at 150Hz in guest-cycle terms.
  - Faithful-core suspects, in order:
    (a) BFRD/DMA-ready handshake timing — cdrom_dma_ready() may assert
        BFRD in a way that lets the guest poll slower than hw, or the
        per-word cdrom_dma_read() path lacks the burst cadence real
        hardware gives a polling CPU;
    (b) no producer flow-control — sectors are written to s_ring_write
        unconditionally on the delay schedule; real drive+FIFO would
        stall rather than overwrite unread data. Adding ring-full
        back-pressure (hold production until consumer advances
        s_ring_read) is the faithful fix IF hw models it;
    (c) MDEC/XA consumer scheduling — if these sectors feed MDEC/SPU
        via a path that runs on guest-cycle deadlines slower than the
        CD clock, the drain starves.
  - NOTE: previous session reached g_game_state=5/match_loop, so the
    load CAN complete — the livelock is timing/probabilistic, consistent
    with a marginal drain-rate deficit rather than a hard protocol break.

SESSION 15d — CD livelock ROOT refined: NOT a cdrom.c flow-control bug
  - process_read_stream already HAS producer flow-control, but ONLY on
    two gated paths: warm_route_consumer_blocked (HLE warm-route only)
    and accelerated_consumer_blocked (speed-divisor/instant only).
    cdrom_state shows accel_consumer_waits=0 -> we run AUTHENTIC cadence
    (divisor=1, realtime), where flow-control is DELIBERATELY absent.
  - The in-code comment is explicit: at authentic cadence "the guest is
    guaranteed a full sector period (~6.6ms at 2x) to ack each
    data-ready INT1 before the next sector can clobber the buffer;
    hardware losing a sector there means the game really was too slow,
    and the one-deep pend-then-lose below is Beetle-faithful."
  - CONCLUSION: ring_dropped at authentic cadence is "faithful clobber" —
    it means OUR guest consumes each sector SLOWER than one sector period
    in guest-cycle terms. The bug is therefore UPSTREAM in the consumer
    path's cycle accounting, not in cdrom.c delivery:
      * INT1 (CD data-ready) ack latency — if interrupt delivery/ack
        costs more guest cycles than hardware, the pend-then-lose path
        drops sectors the game needed;
      * BFRD programmed-I/O drain rate — ch3 DMA is disabled so the game
        polls cdrom_dma_ready/cdrom_dma_read per word; if the MMIO read
        path or poll loop is cycle-slower than hw, drain lags;
      * MDEC/XA decode consumer scheduling on guest-cycle deadlines.
  - NEXT INVESTIGATION: measure guest-cycle cost of one INT1 ack + one
    sector BFRD drain vs the 225792-cycle (2x) / 451584-cycle (1x)
    sector period; find which component over-counts. This is the
    IRQ/cycle axis of this plan, cross-checked vs Beetle cdc.cpp
    HandleSectorRead / INT1 presentation timing.

## SESSION 15e — CD livelock redirected AGAIN: response coroutine stuck "entry-busy", dir-table base write lost

Live debug-server dig (port 4413, frame ~50K, still g_game_state=4):

- `cd_channel_poll` (clean, game/cd.c) has run ~2.5e9 calls; its
  state2-found counter `0x801bcf20` = 0. It reads the dir-table base
  from `*(u32*)0x801fc6c0`.
- **`0x801fc6c0` = 0 while `0x801fc6c4` = 0x20.** `cd_dir_table_set(a0,a1)`
  (game/cd.c:1692) is the ONLY writer of both, called once from
  `tile_stream_open` (game/menu.c:2117) as
  `cd_dir_table_set(_DAT_801f4754, 0x20)`. `_DAT_801f4754` = 0x80010000
  (verified via read_ram: `00000180`), `param_1` to `scene_entry` =
  0x80010000 (game.c:668). So `a0` SHOULD be 0x80010000 — yet the a0
  store reads back 0 while the a1 store (0x20) persists. A later clear
  or a lost/aliased write is suspected; manual `write_ram` poke of
  0x801fc6c0=0x80010000 PERSISTED (not re-cleared), so `cd_dir_table_set`
  is not re-running — it ran once and its a0 write did not stick.
- `cd_mc_state_machine` (game/cd.c:3818) delegates its whole body to
  GENERATED `br2_call_generated(0x80199440)` — the clean-C port was
  abandoned for a guest-stack-frame collision (see its comment). DIAG
  counters show it IS running: `0x801bce88` ≈ 175K invocations.
- But the generated body leaves status `0x801bcc88` = **4 = "entry-busy"**
  every call; `cd_dir_entry_finalize` (`0x801bcf08`, the state=2 setter)
  = **0 calls**. Entry cursor `0x801fc6a4`=3, `ent=0x80010060`.
- `cd_stream_mode_init`'s `do{cd_mode_flag_set}while(0)` infinite-retries
  because `cd_cmd_issue`→generated `0x80196e14` never reports command
  completion → repeating Seek(0x02)→Setmode(0x0e)→ReadN(0x1b) ~every 36f.

CONCLUSION: the livelock is NOT sector-production pacing (SESSION 15d's
consumer-cycle hypothesis is superseded). It is the **CD command/response
completion path**: the deferred-interrupt coroutine (`cd_intc_dispatch →
cd_response_pump → cd_mc_state_machine`) runs but every dir entry stays
"busy" — the CD command response that should mark it ready never arrives
or never matches. Two candidate roots now:
  (a) `0x801fc6c0` a0-store lost → poll/finalize index the wrong table;
  (b) INT1/CD-response delivery to the coroutine broken (irq fires per
      `irq_flag:0x03` but the response buffer/flag the generated
      `0x80199440` waits on may never be written).
NEXT: determine whether generated `0x80199440`'s wait is on a response
flag the INT1 handler should set; and find the real writer/clearer of
`0x801fc6c0` (add a write-watch on it). Both are faithful-core IRQ/MMIO
issues, not cdrom.c pacing.

### SESSION 15e addendum — consumer-drain is the throughput bottleneck

- Poked `0x801fc6c0=0x80010000` via `write_ram` (4 byte-writes); value
  PERSISTED (wtrace on 0x1fc6c0-0x1fc6c4 caught only my own pokes —
  nothing re-clears it). `state2` counter `0x801bcf20` stayed 0 and
  `_DAT_801f4740` stayed 0 → the base was a SYMPTOM (one-time early
  clear), not the blocker. Corrected conclusion below.
- `cdrom_state` snapshot: `ring_dropped`=174086 and climbing,
  `ring_starved`=0, `bfrd_set`=468 / `bfrd_clear`=13 (tiny — the guest
  barely drains via BFRD programmed-I/O), `reading`=1,
  `read_cmd`=0x1B, mode=0xE0 (double-speed whole-sector ~150Hz),
  `irq_flag`=0x03 during exec, `int1_pended`=0.
- MECHANISM (reconciles 15d+15e): sectors are produced at ~150Hz into
  the 8-slot ring unconditionally; the consumer (BFRD/DMA→dir-entry
  buffers) drains SLOWER → producer overwrites unread slots
  (`ring_dropped`) → the specific sectors a dir entry waits for are
  lost → `cd_mc_state_machine` (generated 0x80199440) sees the entry
  "busy" (status 4) forever → `cd_dir_entry_finalize` never sets
  state=2 → `cd_channel_poll` spins ~billions → `blk_io_retry` never
  returns → `scene_entry` re-issues Seek/Setmode/ReadN forever.
- ROOT = consumer drain rate < 150Hz production in guest-cycle terms.
  `bfrd_set`≈468 over the whole run means the BFRD poll path is barely
  exercised — either the game drains via a path we're not pacing right,
  or an entire consumer (MDEC/XA→dir-entry commit) is starved. NEXT:
  find WHICH guest path is supposed to drain the ring for these
  Mode2/Form2 sectors (BFRD target buffer vs MDEC vs SPU) and measure
  its per-sector guest-cycle cost vs the 225792-cycle (2x) period.

### SESSION 15e addendum 2 — INT1 dispatch CONFIRMED working; gap is the coroutine resume

- `irqctx_ring` shows `istat=0x4` (CDROM IRQ, i_mask bit2 enabled,
  cop0_sr IEc=1) taken at `take_pc=0x80199360` = `cd_channel_poll`,
  `exit_reason=1 same_thread=1 restored=1`, repeating every CD INT1.
  → The CD interrupt IS delivered and DOES enter the game's CD poll.
- `ring_dropped`≈174K vs ~179K sectors produced ⇒ the 0x1F801802 FIFO
  drain effectively never runs; reads can't complete ⇒ dir entries stay
  "busy" ⇒ `cd_channel_poll` returns 1 forever.
- The missing link is the DEFERRED-INTERRUPT coroutine:
  `cd_ctx_save/cd_ctx_restore → cd_chan_full_init → cd_intc_dispatch →
  cd_response_pump → cd_mc_state_machine`. Its clean-C port was rejected
  (cd.c:183) because it needs LIVE cpu->gpr — a wild store @0x8019AA98
  otherwise — so it must run via generated code with a saved/restored
  guest context. If that resume isn't firing (or restores a stale
  context), the response pump never marks the sector arrival that flips
  a dir entry to state 2.
- NEXT (faithful-core IRQ axis): trace whether INT1 actually resumes the
  deferred coroutine — check `cd_ctx_restore`/`cd_intc_dispatch` reach
  and the guest SP/context it restores vs what the recompiled-BIOS INT1
  dispatch provides. This is the same class as the earlier
  `PSX_HLE_SCHEDULER` fiber-vs-LLE scheduling landmine.

### SESSION 15f — DECISIVE: CD FIFO consumer drains ZERO bytes (scheduling deadlock)

- `cdrom_state` sampled repeatedly: `sector_read_pos` (RB_.pos) = **0**
  ALWAYS, `sector_available`=1, `sector_size`=2340, `bfrd_set`=468
  (static), `reading`=1. A full 2340-byte Mode2/Form2 sector sits in the
  ring UNREAD while `ring_dropped` climbs ~200/sec.
- ⇒ The guest reads **0 bytes** from the `0x1F801802` data FIFO (and the
  `cdrom_dma_read` 4-byte path is likewise unused — CD DMA ch3 chcr=0).
  This is NOT a slow consumer; the drain routine is **never scheduled**.
- The FIFO drain is performed by GENERATED game code (the CD streaming
  routine issues `0x1F801802` reads / arms CD DMA). The clean source
  delegates those sequences (`cd_file_read_seq`, `cd_result_wait_*`,
  `cd_seek_complete_poll`) to `br2_call_generated` precisely because they
  need live `cpu->gpr` for the deferred-interrupt coroutine.
- DEADLOCK SHAPE: INT1 → `cd_channel_poll` returns 1 (no dir entry at
  state 2) → the streaming/drain routine that would consume the FIFO is
  gated behind a channel-ready or coroutine-resume that never fires →
  `wb->pos` stays 0 → every sector overwrite counts `ring_dropped` →
  dir entries never see complete data → never finalize → state never 2.
- This matches the documented failure mode in `cd_seek_complete_poll`
  (cd.c:2191): "a clean-C while+br2_irq_poll cannot supply a dispatchable
  resume PC for the deferred switch, so the coroutine starves."
- NEXT (faithful-core scheduler/IRQ axis): find WHICH guest routine owns
  the `0x1F801802` drain and what resumes/schedules it — a kernel thread,
  the B0/0x19 deferred callback (`cd_ctx_restore`→`cd_intc_dispatch`),
  or a per-INT1 handler tail. Verify that resume path actually fires on
  CD IRQ under `PSX_HLE_SCHEDULER=0` (fibers). Suspect the same
  fiber-vs-LLE scheduling landmine that `PSX_HLE_SCHEDULER` was added for.

### SESSION 15g — clean cd_channel_poll exonerated; bug is upstream channel completion

- Decoded generated `br2eu_func_80199360` (cd_channel_poll) vs clean port.
  Generated is a TWO-STEP scan the clean port simplified:
    state==1 -> clear idx@0x801FC6AC + entry state, recompute, recheck
    state==2 -> write state=4, return channel ptrs (success)
    else     -> return busy
  Clean port only does `state!=2 -> busy`. BUT the live entry state is
  0x10 (CDDBG 0x801bcf1c), which matches NEITHER generated==1 nor ==2 —
  so generated would ALSO return busy here. Clean port is NOT the bug
  for the current stall.
- Tested PSX_HLE_SCHEDULER=1 (HLE deterministic-TCB) on :4420 — reached
  gamestate=4 identically, sector_read_pos=0, ring_dropped climbing.
  Scheduler is NOT the cause; fiber vs HLE identical.
- CONCLUSION: dir/channel entries never progress to state 1/2 because
  the underlying CD channel read never delivers sector data into the
  channel buffer. The 0x1F801802 FIFO drain (sector_read_pos=0) is the
  missing consumer — it is driven by GENERATED game CD-streaming code
  whose trigger (channel-ready / INT1 data-ready callback) never fires.
  This is the faithful-core frontier: trace the generated CD
  streaming/INT1-callback path that owns the 0x1F801802 drain and find
  why it never runs. Requires Ghidra/disasm of the generated CD driver,
  not more black-box polling.

### SESSION 15h — CD drain is a software-DMA worker that never runs

Live probes (`:4413`, state-4 livelock) closed the loop on *what* should
drain `0x1F801802`:

- `cd_dma_xfer_start` (clean `cd.c:4773`) does NOT touch hardware DMA ch3.
  It programs a **software register block** at guest `0x801bdeb0` (KSEG0 RAM):
  writes the channel base ptr to `+0x04`, `(size<<16)|0x10` to `+0x08`,
  sets start flag `0x801bdea0=1`, and `0x1000200` to `+0x1c`. `CDMMIO16`
  offsets `0xd3/0xd5/0x22` index this block.
- Live block state: `0x801bdea0=1` (**transfer armed**),
  `0x801bdeb0..bc` = four pointers `0x1f801c00/10c0/14c0/18c0` (0x400 stride)
  — the per-channel descriptor table.
- `cd_intc_dispatch` clean thunk counter `0x801bce00` = **0** — the clean
  entry is never used; generated `0x8019a8ec` is invoked directly by its
  generated callers. `cd_response_pump` counter `0x801bce04` ≈ 616K and
  climbing, so the response coroutine DOES run via generated dispatch.
- `cd_response_process` handles command *responses* (the `local_20` switch),
  not sector data. The per-sector `0x1F801802` drain is the software-DMA
  worker gated on `0x801bdea0`, which stays armed-but-incomplete.

Conclusion: the missing piece is the CD-driver **software-DMA worker** that
should copy `0x1F801802`→channel buffer when `0x801bdea0` is set. It never
completes → dir entries stay busy → `cd_channel_poll` spins. This worker
lives in generated code (the same deferred-coroutine family as
`cd_ctx_save`/`cd_intc_dispatch`); resolving it needs disassembly of the
generated worker loop to find why the armed transfer doesn't drain.

### SESSION 15i — TWO CD paths: BIOS DMA works, game's custom driver is the stuck one

`cdrom_trace_dump` + `fn_filter`/`fn_entry_dump` + `cd_read_log` +
`dma_cdrom_history` on the live livelock (`:4413`) gave the decisive split:

- **Guest NEVER reads `0x1F801802`** (data FIFO): 0 reads across ~38K CD
  register accesses. It only *writes* `0x1F801802` (param FIFO, 124×) and
  polls index/status/response (`0x1F801800/803/801`). So the drain is NOT
  programmed-I/O — consistent with DMA or a not-yet-reached drain.
- **`irq_flag` is an enum** (DATA_READY=1/COMPLETE=2/ACK=3/DATA_END=4), and
  status `0x1F801803` returns `irq_flag|0xE0`. Live trace shows
  `irq_flag=0x01`/`status=0xE1` = **DATA_READY is being signaled**; the guest
  sees it (polls status 594×) yet never drains.
- **fn trace (filter 0x80180000-0x801A0000)** during the livelock runs ONLY:
  `cd_chan_full_init`, `cd_intc_dispatch`, `cd_guest_zero32_a`,
  `cd_mc_state_machine` (+0x90c continuation), `cd_result_wait_b`.
  **No data-FIFO/drain function is ever entered.**
- **BIOS CD path WORKS**: `dma_cdrom_history` shows 471 *completed* DMA-ch3
  transfers, all `pc=0xBFC1E88C` (BIOS/kernel CdRead), streaming hundreds of
  consecutive 2048-byte sectors (lba 216335→216578+) to a contiguous buffer
  in frames 8–139 — the attract/initial loads.
- The stuck `scene_entry` tile-stream uses the **game's CUSTOM CD driver**
  (`0x8019xxxx`: `cd_channel_poll`/`cd_dir_*`/`cd_mc_state_machine`/
  `cd_dma_xfer_start` software-DMA via the `0x801bdeb0` channel block), a
  DIFFERENT path from BIOS CdRead. `cd_read_log` shows it re-reads lba
  208829 (32B hdr + 2016B data chunks) each retry — data moves but the
  dir-entry never reaches state 2.

Conclusion: the bug is isolated to the game's **custom CD streaming
driver's completion path** — the INT1 data-ready dispatch never invokes the
FIFO→channel drain (or channel-ready mark) that would advance dir entries
to state 2. The BIOS DMA path is exonerated. Next: trace the custom driver's
INT1 dispatch (`cd_intc_dispatch` 0x8019a8ec generated body) to find the
data-ready branch that should call the drain and why it isn't reached.

### SESSION 15j — correction: cd_dir_table_set is NOT buggy; table base was a red herring

Follow-up on the `0x801fc6c0==0` anomaly exonerated the clean port:

- `game/cd.c:1692 cd_dir_table_set` DOES store `a0->0x801fc6c0`,
  `a1->0x801fc6c4` before `cd_dir_reset()` (verified against generated
  `br2eu_func_80195800`: `sw a0` then `jal cd_dir_reset` w/ `sw a1` in the
  delay slot). An earlier awk excerpt that showed only `cd_dir_reset()` was
  a misread — no stores are missing.
- `guest_for` only accepts `[0-9a-f]` (lowercase); the `DAT` in both
  `g_DAT_801f4754` and `g__DAT_801f4754` is uppercase and not hex, so BOTH
  resolve to the single hex run `801f4754` -> guest `0x801f4754`. game.c and
  menu.c share the same cell. No write/read mismatch.
- Live: `0x801f4754 = 0x80010000` (= scene_entry param_1), so
  `cd_dir_table_set` is called with the correct `a0`. The `0x801fc6c0==0`
  seen pre-poke was a one-time early-init artifact, NOT a recurring clear
  (wtrace caught only the manual pokes).
- The manual poke to `0x80010000` set the CORRECT value and it persisted,
  yet `state2`/`cd_dir_entry_finalize`/`load_done` stayed 0. => the dir-table
  base is necessary-but-not-sufficient; the real blocker is upstream: dir
  entries never reach state 2 because the custom CD driver's data/completion
  path (INT1 data-ready -> FIFO->channel drain -> entry finalize) never
  runs. BIOS CD path (DMA ch3, pc=0xBFC1xxxx) is fully working and
  exonerated.


### SESSION 15k — dir table is 0x11-poison: cd_dir_clear never reaches 0x80010000

Live dir-table dump at the livelock (base `0x801fc6c0 = 0x80010000`, count
`0x801fc6c4 = 0x20`, idx `0x801fc6ac = 0`, `0x801fc6b4 = 0xffffffff`):

- All 32 entries (0x20 stride) at `0x80010000` have `firstu32 = 0x11111111`
  (heap poison) except entries 0/1 which hold scene/stream data
  (`ent0 firstu32=0x10`, `ent1=0x42202529`). NONE has a valid `state` u16.
- `cd_dir_clear(0,0x20)` is supposed to write `u32=0` to every entry's first
  word via `cd_dir_table_set -> cd_dir_reset -> cd_dir_clear`. Generated
  `br2eu_func_80199320` confirmed faithful (reloads base from `0x801FC6C0`
  each iter, stores 0). Clean port is functionally identical.
- => Either `cd_dir_clear` does not execute its writes to `0x80010000`, or
  the region is re-poisoned AFTER clearing (i.e. `0x80010000` doubles as a
  heap/stream arena that gets `0x11`-filled, overlapping the CD table).
- `cd_channel_poll` clean port had a REAL fidelity bug (committed d8fba2e):
  generated resets `0x801FC6AC=0` UNCONDITIONALLY on `state==1` (delay slot of
  the `0x801FC6B4` test); clean had it inside the `!=0` branch. Does NOT
  manifest while `0x801fc6b4=0xffffffff`, but fixed for faithfulness.
- The CD driver vtable at `0x801bdd80` confirmed: `0x801bde00 =
  cd_dir_entry_finalize(0x801990f0)`, `0x801bdd8c = cd_chan_full_init
  (0x8019a814)`, plus HW reg ptrs. Dispatch machinery intact.
- NEXT: determine whether `cd_dir_clear` writes land (wtrace on 0x80010000)
  and whether the table base overlaps a reused stream buffer. If entries are
  genuinely never populated, the read-issue path that creates them is the
  missing link — trace `cd_dir_entry_probe/store` callers.

### SESSION 15n — rtrace PROOF: guest never reads CD data FIFO; drain worker never invoked

Live `rtrace` (MMIO-read trace) on the livelock (`:4413`, PID 28173) gave the
decisive hardware-level confirmation of SESSION 15f/15i:

- Armed `rtrace` on `0x1F801800-0x1F801807` + DMA range. ~11K MMIO reads captured.
- The guest reads `0x1F801803` (CD status) ×305 and `0x1F801800` (index) ×305,
  `0x1F801801` ×61, and `0x1F801070/074` (I_STAT/I_MASK) ×384 — i.e. it POLLS
  status/index/interrupt regs constantly.
- **`0x1F801802` (CD data FIFO): ZERO reads.** The sector-data drain is
  definitively never executed. No drain/FIFO function is ever entered.

PC attribution of the polls:
- `0x8019A9AC`/`0x8019A954` -> `br2eu_func_8019A8EC` = **cd_intc_dispatch**
- `0x80199570`/`0x80199454` -> `br2eu_func_80199440` = **cd_mc_state_machine**
- `0x80196C04`, `0x801AA260` -> other CD-region fns

Decoded `cd_intc_dispatch` block_8019A944 (the resume-time gate):
- Loads ptr cells `0x801BDDA4=0x1F801070` (I_STAT), `0x801BDDA8=0x1F801074`
  (I_MASK); state field `*(u16*)0x801BCD48 = 0x000d`.
- Computes `I_MASK & 0x000d & I_STAT`; nonzero -> continue, zero -> exit path.
- With `i_stat=0` it exits normally — the dispatcher is NOT stuck; it simply
  finds no pending IRQ in mask {0,2,3} on each resume.

Interrupt path verified healthy:
- `irq_enable=0x07` enables ALL CD IRQ reasons (mask_matches_reason ANDs reason).
- `i_mask=0x0000000D` (VBLANK+CDROM+DMA bits) — CDROM bit2 ENABLED.
- `int1_lost=0`, sectors delivered (`last_sector.lba` advancing 208829->208893),
  `ring_dropped` grows ~CD rate = consumer fully stalled.

Custom-driver read pattern (`cd_read_log` tail): lba 208829 -> 32B headers to
physical 0x0/0x20/0x40 (dir entries) + 2016B data to 0x400/0xBE0/0x13C0.
Confirmed dir table IS at physical 0 (fc6c0=0 correct); buf2@0x13C0 EMPTY =
its sector was ring-dropped before the (non-running) drain could grab it.

CONCLUSION (unchanged, now hardware-proven): the game's custom CD streaming
coroutine never invokes its `0x1F801802` FIFO-drain worker, so channel data
buffers never fill -> dir entries stuck state=3 -> cd_channel_poll spins ->
blk_io_retry livelock. The drain call is gated inside generated
`cd_mc_state_machine`/`cd_intc_dispatch` coroutine dispatch. NEXT: decompile
the cd_mc_state_machine per-channel "data-arrived -> drain" branch to find
the exact flag/counter that never gets set (likely the INT1 data-ready ->
channel-ready mark), OR diff against the DuckStation oracle's CD path.

## CD livelock — session 15u/v refinement (callback architecture nailed)

- `cd_response_pump`(INT1,0x801979f8) loops `cd_response_process()`->s0:
  `s0&4`->per-sector cb `DAT_801bc84c`, `s0&2`->complete cb `DAT_801bc848`.
- LIVE: cb84c=`0x80198fe4`=`cd_mc_sm_wrap`->`cd_mc_state_machine` = the
  STREAMING per-sector cb. cb848=0. File-load cb `cd_read_step_cb`(0x801985c4)
  dbgctr frozen @0x42 -> file-load session ended; streaming owns cb84c now.
- `cd_response_process` returns 4 (sector-ready) ONLY for response-type 1
  (`bcb18&7==1`); bcb25 stayed 0 -> type-1 responses may be starved OR
  cd_mc_state_machine returns status-4 before sector accounting.
- `fc6b0`==0 forces the status-4/retry path but does NOT gate +4 accounting
  (verified: both fc6b0 branches return 4; it only gates fc6a0 retry counter).
- dir ent+4 reached 0/1/3 then froze -> +4 writer ran partially then stopped.
  `cd_mc_state_machine` only READS entry+4 (asm 0x80199810/d0/9bc8).
- ring_dropped=511k: drive reads ahead, guest never drains (BFRD mostly 0,
  sector_read_pos=0, ch3 idle). Flow-control holds exist but consumer dead.
- ORACLE BLOCKED on macOS: `-nogui` exits (Qt Cocoa needs a window); container
  build fails "Prebuilt dependencies not found" (only macos-universal deps
  present, no linux-arm64). No built macOS binary present.
NEXT: find the +4 writer (channel +68/+70/+72 -> dir +4 consumer) OR get a
working oracle (build linux-arm64 deps / fix macOS headless boot).

## CD livelock — session 15x: STREAMING FIXED, new sync-load blocker isolated

- guest_at low-KUSEG fix (fc1e966) BROKE the stream deadlock: bfrd_set
  468->1246, request 0->0x80, ring_dropped 30722->516, cd_read_log shows
  active sector-by-sector file loads. CD streaming path now healthy.
- Game progressed MUCH further: boots -> memcard -> attract ->
  battle_intro_flow (g_game_state path state 6). BUT stuck: _DAT_1f80008c
  frozen at 0x20 inside match_loop stage-1 -> menu_res_load_sm ->
  save_load_state_machine -> cd_file_load_sm(0xa0).
- NEW ROOT CAUSE (precise): the CD/GPU immediate-mode + DMA
  register-pointer-cell context is ENTIRELY UNINITIALIZED.
  * 0x801b82c0-0x801b8320 (GPU func-table region): garbage
    (0x555555/0x488040/0xb40000fa pattern)
  * 0x801b8300-0x801b8500 (imm-mode cmd ctx + queue cells 0x801b8438-44):
    same garbage
  * 0x801bdeb0-0x801bded4 (DMA chan ptr cells): same garbage
    (0x801bdec0 should ->0x1F8010F0 DPCR, holds 0xb80000ff)
- Mechanism: cells hold MMIO pointers dereferenced via br2_st32p/br2_ld32
  (cell -> 0x1F801xxx -> hw reg). Garbage cells -> cd_cmd_queue_load writes
  commands to non-hw addrs -> drive seq FROZEN -> cd_read_poll/cd_intc_busy
  loop forever -> zero GPU submits -> no render.
- Streaming path UNAFFECTED: uses computed 0x1f801080+ch*0x10 directly,
  bypassing these cells. That's why boot/attract loads worked but the first
  synchronous cd_file_load_sm hangs.
- Cells are pointer-table entries; values (0x1F801xxx) come from a data
  table / computed base, NOT immediates (none in generated code).
NEXT: find the register-pointer-table init fn (fills 0x801bdeb0+/0x801b84xx
cells with 0x1F801xxx). Suspect it lives inside cd_chan_full_init (resumable
coroutine via cd_ctx_restore) or a one-time CD/GPU ctx setup that either
never ran or is gated. Once cells hold real MMIO addrs, sync loads complete.

## Wild-write root cause — session 15y: decompression runaway, NOT missing init

- CORRECTION to 15x: pointer cells are NOT uninitialized. Fresh boot (4417)
  shows 0x801bdeb0=0x1f801c00, 0x801bdec0=0x1f8010f0, 0x801b8438=0x1f8010e0
  — all CORRECT (match EXE .data). The image loads fine.
- The cells get CORRUPTED at runtime: valid at frame ~645, garbage by ~2907,
  exactly when match_loop stage-1 -> menu_res_load_sm -> save_load_state_machine
  -> cd_file_load_sm(t1+0xa0) runs. ~500KB of guest RAM
  (0x80180000-0x801f8000) overwritten with a repeating structured pattern
  (0x55555555/0x40408048/0xfa/0xfc) — trashes pointer cells AND heap cursors
  (_DAT_801e3cfc, _DAT_801c47b8) -> cd_cmd_queue_load writes to garbage ->
  sync load hangs -> zero GPU submits.
- SOURCE of wild write: decompression path stream_block_decode(0x80163154)
  -> mode3_dispatch -> {lzss_decode | block_copy_ret_len | lzss_trap}.
  * block_copy_ret_len: guest_memcpy(a0, a1+0xc, s0) with s0 = *(u32*)(src+8)
    — garbage s0 = huge copy.
  * lzss_decode: writes *op++ until a len==0 terminator in the SOURCE stream;
    garbage src => no terminator => unbounded write to dst.
- stream_block_decode already carries a guest_of(param_2) fix (menu.c:2243)
  for the source-base host->guest bug, but the src offset
  (param_2[param_3+1] + (*param_2+1)*4) or the heap cursor dst may still be
  wrong => a1 points into unloaded/garbage -> runaway decode.
NEXT: find the FIRST bad write. Trace stream_block_decode's a0(dst)/a1(src)
for the battle-intro load; check whether cd_file_load_sm actually populated
the compressed blocks (CD sync path) and whether the tag_table offsets are
valid. Likely a wrong src offset OR heap cursor corrupted by an earlier bad
length return.

## Session 15z — heap-A cursor overrun narrowed to a single +1.64MB offset_table set; every mechanism verified faithful

- Instrumented decomp_trace with: tag10 = cd_file_load_sm(file_id,dest)
  [deduped vs retry flood], tag11 = stream_block_decode return length,
  tag12 = heap_record_emit heap-A advance, tag13 = every write to
  _DAT_801c47b8 (old->new). Added seq_lo paging to decomp_trace so full
  pump history survives the post-corruption retry livelock.
- Cursor-write trace (tag13) caught the dominant advance: _DAT_801c47b8
  jumps +0x1906C4 (~1.64MB) in ONE write, 0x80034298 -> 0x801C495C, inside
  battle_bringup_b's L_state10 offset-table loop
  (menu.c:8624 `MW32(0x801c47b8, o2[0xb]+(o2[1]-o2[0]))`). 0x801C495C is a
  valid file-0x82 offset-table/blob region (holds 0xFFFFFFFF empty slots).
- Verified faithful vs generated MIPS: offset_table_lookup (0x80163204)
  returns a0+off+(count+1)*4+0xc — identical; caller loads a0=*(0x801C47B8)
  faithfully; lzss_decode/block_copy_ret_len return src[8] declared size;
  stream_block_decode/heap_alloc54 advance by return (guest_of fix holds);
  terrain_packet_gen grid dims sane (12x12); file 0x88 = 99 sectors real;
  cd_file_load_sm(0x88, *(0x801C47B8)) dest faithful.
- Memory map confirmed vs oracle (DuckStation :4371): file/heap data
  legitimately fills 0x80010000 -> ~0x801D-0x801E0000; globals
  (_DAT_801e3cfc cursor ptr, 0x801F68D8 counter) live higher and MUST stay
  clean. Ours: heap-A reaches 0x801C90B8, file 0x88 DMA writes
  0x801C90B8->0x801FA8B8 (~198KB) covering both globals -> stall.
- ALSO FIXED real split-brain (commit 626cd74): bind_globals_2 pointed 15
  g_DAT_*/scratch globals into bind.c's private guest_arena/guest_scratch
  (dead in runtime build); now routed through guest_at()->ram[]/scratchpad.
- REMAINING: heap-A accumulates ~123KB+ excess vs real HW ceiling. All
  per-op mechanisms faithful => divergence is data-dependent (a decode
  emitting too much, or the offset-table input at cursor being wrong).
  NEXT: compare _DAT_801c47b8 trajectory + decode output vs Beetle oracle
  (psx-beetle :4380, parity tools) at the battle-load stage; OR verify the
  offset-table base MB32(0x801c47b8)=0x80034298 held a valid table at the
  L_state10 instant (memory is transient post-corruption). The pad-producer
  issue remains a parallel open thread.

## Session 15z-addendum — decode output overlaps compressed-source region (self-corruption)

- DECOMP_TRACE_CAP 16K->1M + tag14 offset_table_lookup(a0,n,ret) captured the
  FULL 491-entry pump history. PROOF: zero tag14 records -> clean
  offset_table_lookup is NEVER called this path; the +1.64MB cursor write is
  NOT the offset-table set (earlier lead corrected).
- _DAT_801c47b8 is purely the heap/decode OUTPUT cursor (+= decode return).
  File table decoded: {extent_ptr@0x801b5b94+id*8, sector_count}; file 0x88
  =99 sectors=0x31800 bytes, safe cursor bound =0x801AE800 (globals@0x801E0000).
  Ours reaches 0x801C90B8 -> over by ~120KB -> file 0x88 DMA writes
  0x801C90B8->0x801FA8B8 covering _DAT_801e3cfc + 0x801F68D8.
- NEW MECHANISM: decode dst (output) climbs 0x800F0258->0x801C8F0C->0x801C90B8
  while decode src (compressed blocks) sit at 0x801C495C->0x801CCExx in the
  SAME 0x801Cxxxx file-data region. LZSS output outgrows input, so the output
  cursor catches up and can overwrite not-yet-decoded compressed blocks ->
  those decode to garbage -> even larger returns -> runaway. Root is still
  the ~120KB heap over-production; the overlap is the corruption amplifier.
- Remaining suspects for the over-produce: save.c st10 `cursor=head+*head`
  size-skip trusting file-0x10's first u32; terrain_packet_gen/heap emit
  absolute returns; OR a decode reading a wrong tag-table entry (tag table
  @0x801c47c0 overwritten by file 0xa0/0xb0 loads). CD data integrity vs disc
  still unverified (extent->LBA decode pending).

## Session 16 — CD/event stall root-caused to a starved sector consumer; full kernel IRQ map decoded

Live instance (auto-boot, deterministic, no input) wedges at framecnt=0x60a.
MEMORY IS CLEAN (heap-A cursor 0x8006AA34, globals intact) => this is an
EARLIER stall than 15z's heap-corruption point, not that bug.

CONFIRMED MECHANISM (corrects several prior wrong hypotheses):
- Exception vector @0x80000080 decodes `lui $k0,0; addiu $k0,$k0,0x27ac; jr $k0`
  => IRQs dispatch to the GAME kernel handler 0x000027ac (interpreted dirty-RAM),
  NOT OpenBIOS. Table 0x494c[6]=0x27ac confirms it is the IRQ ExceptionHandler.
- cd_intc_dispatch (0x8019A8EC, GENERATED) IS RUNNING — it calls kernel services
  via trampoline slots 0xA0/0xB0/0xC0 -> dispatcher 0x2AA0/0x2AC0 ->
  `lw table[0x49cc+$t1*4]; jr $t2`. Earlier "cd_intc_dispatch starved / fntrace 0"
  was a FALSE NEGATIVE: fntrace hooks the native dispatch and these calls run
  under dirty-RAM interp context; the interp insn_log (dirty_insn_log) shows
  ra=0x8019AA9C/0x8019A9C8 = return addrs INSIDE cd_intc_dispatch.
- Service table 0x49cc: t1=0x17->0x29CC (context/jmp_buf RESTORE ~30 reg loads),
  t1=0x07->0x2610 (DeliverEvent), t1=0x0d->0xBFC056EC (BIOS).
- HookEntryInt IS armed: kernel var @0x4BF8 = 0x801BCD50, a valid jmp_buf
  (JB_PC=0x8019A87C resume->cd_intc_dispatch, JB_SP=0x80200000). Called once.
- IRQs delivered normally: event_ring IRQ_DELIVER func=0x29CC, istat=0x1 VBlank
  AND 0x4 CD, imask=0xD, DMA_KICK/SCHED/DONE active. sp/current_tcb constant =>
  per-IRQ service calls, NOT recursion.

THE STALL (evidence):
- bioscall_dump: TestEvent (B0/0x0B) called 1,417,188x; DeliverEvent (B0/0x07)
  only 69x; OpenEvent 19x; EnableEvent 19x; CloseEvent 5x; HookEntryInt 1x.
- Game spins in cd_seek_complete_poll(0x8019D500, delegated->generated) whose
  a0==1 loop calls cd_seek_state_machine=TestEvent(*(0x801BDE38)=0xF1000008).
- cdrom_state FROZEN over time: sector_read_pos=2060/2340 (mid-sector, 280 left),
  sector_available=1, bfrd_set=1660 vs bfrd_clear=13, ring_dropped=692,
  reading=0, last_sector lba8240 frame1713, i_stat=0x1, irq_flag=0.
- cb8_dispatch (0x8019AE68, VBlank frame counter) count flat 17=>17, framecnt
  0x60a => the IRQ->VBlank-handler dispatch stopped feeding the counter.

ROOT CAUSE (high confidence): the CD sector CONSUMER that drains the
0x1F801802 data FIFO stalled mid-sector at offset 2060. bfrd_set(1660)>>
bfrd_clear(13) => sector buffers fill but are never consumed; the read never
completes => no CD completion IRQ/event => game's TestEvent busy-poll (does NOT
yield) spins forever => downstream cb8_dispatch/frame-counter frozen => wedge.
The consumer starvation is the single upstream fault; everything else is
downstream symptom.

OPEN: WHY the consumer stalls at 2060/2340. Candidates: (a) consumer is the same
thread now spinning in TestEvent (self-deadlock: it can't drain because it's
polling); (b) data-ready IRQ not reaching cd_response_pump/the stream worker;
(c) a clean-C stream/seek function starved. cd_response_pump ran 86x (clean_trace)
so it IS entered — need to check whether it actually reads 0x1F801802 or exits
early. NEXT: trace which function reads CD data reg 0x1F801802 and whether the
per-sector loop advances read_pos; check if the consumer shares the TestEvent
thread's timeslice.

## Session 17 — heap_ptr_fixup nested-table fix landed (70076c8); game still wedges at the SAME Session-16 CD stall (framecnt frozen ~1461)

- **heap_ptr_fixup fix COMMITTED + PUSHED (`70076c8`).** Phase-1 (the pass that
  relocates the nested index table's `0x8000xxxx` tagged offsets into
  `a0+off*4` heap pointers) was gated on `MB32(a0+4)!=0`, but EVERY decompressed
  heap block arrives with `+4==0` (verified faithful via independent LZSS decode
  producing exactly `0x7dac` bytes, and the original MIPS disasm confirms the
  gate). So phase-1 never ran -> `ctx+0x14` stayed an unrelocated low offset ->
  `heap_serialize` wrote fixup records into OpenBIOS kernel RAM (phys
  0x1d1d-0x2727, confirmed live kernel code on the SCPH1001 oracle too) ->
  `unknown dispatch 0x27AC`. Fix: gate phase-1 on `+8` still holding an
  unresolved word-offset (`noff < 0x80000000`) instead of `+4`; phase-1 rewrites
  `+8` to the resolved KSEG0 pointer so the test is idempotent on re-entry.
  Verified correct heap pointers produced (0x80100388-class).
- **CORRECTION — the game does NOT actually progress past the CD stall.** An
  earlier read of "13+ min stable at ~99% CPU, memcard writes, clean dispatch
  active" was MISLEADING: high CPU + clean_try_dispatch is exactly what a
  busy-poll wedge looks like. framecnt(0x801bdde0) is FROZEN (read 3x over 8s via
  lldb: 0x5b5=1461 constant). The process is wedged in Session 16's CD stall,
  NOT running gameplay. memcard writes were from early boot only.
- **NEW OBSERVABILITY CHANNEL — lldb reads guest RAM directly.** The debug
  server starves completely under the busy-poll (every command incl. set_input
  and read_ram times out; all commands run on the saturated emu thread). But
  `g_psx_ram` is a flat host array: KSEG0 0x801bdde0 -> phys 0x1bdde0. Use
  `lldb -p PID --batch -o "expr/x *(unsigned int*)(g_psx_ram+0x1bdde0)" -o
  detach -o quit` to read ANY guest word, and `p <host_global>` for runtime
  counters — no debug-server dependency. THIS IS THE WAY to introspect a
  wedged run.
- **Live wedge state captured (matches Session 16 exactly):**
  framecnt=0x5b5 frozen; cd_status(0x1bcb24)=0x2; inservice(0x1bcd18)=1;
  intc_dbg(0x1bce00)=0; TestEvent flag(0x1bde38)=0xf1000008; guest ra=0x801265f4
  in the cd_seek/TestEvent poll. HOST side: s_ring_read==s_ring_write==2,
  RB_.pos=2060/RB_.size=2340 (IDENTICAL to Session 16), s_bfrd_set=1248 >>
  s_bfrd_clear=13, s_ring_dropped=518, reading=1, irq_flag=0, pending_dataready=0,
  s_warm_route_active=0, g_disc_speed_divisor=1, read_delay=0, all flow-control
  wait counters 0. g_vblank_raise_count=1631 (hardware vblanks DO fire;
  cycles_since_vblank accumulating) -> the game's VBlank HANDLER (cb8_dispatch)
  is masked/not running during the seek, freezing framecnt — but CD advance
  still runs via cdrom_advance at event deadlines.
- **ROOT CAUSE PINNED — `heap_serialize` infinite block-list loop (menu.c:14305).
  `sample` shows the guest parked in `clean_try_dispatch -> battle_intro_flow
  (menu.c:1928) -> menu_res_load_sm (save.c:4185) -> save_load_state_machine
  (save.c:6374) -> heap_record_emit (menu.c:13481) -> heap_serialize
  (menu.c:14307) -> guest_at`. heap_serialize's outer `for(;;)` walks the block
  list via `blk = MB32(blk)` (next-block ptr at blk+0) and only breaks on
  `blk == 0xffffffff`. It NEVER terminates -> the clean C loop spins in guest_at
  reads without returning to a cycle-charge boundary -> **psx_cycle_count frozen
  (verified: 0x41daacc1 identical across 4s, cycles_since_vblank frozen 39777)**
  -> cdrom_advance/process_read_stream never run -> CD frozen -> the resource
  load that called heap_serialize never completes -> wedge. THIS is the real
  upstream fault; Session 16's "CD consumer starved / TestEvent busy-poll" was
  the downstream symptom (CD frozen because guest cycles frozen).
- **WHY it loops:** either (a) the block list genuinely cycles (blk+0 next-ptr
  forms a loop or points into a non-terminating chain) because heap_ptr_fixup
  phase-1 (70076c8) over-relocated a next-block pointer, or (b) a faithful data
  cycle the generated body also hits. MUST diff clean `blk=MB32(blk)` walk vs
  generated br2eu_func_8016B7C0 block-advance (generated uses computed-goto;
  termination constant to verify — clean uses 0xffffffff sentinel).
- **RESOLVED the wedge (`652e58a`):** lldb frame-select showed heap_serialize's
  locals at the spin: `blk=0, cnt=0, s3=0`. The block list reached a NULL
  next-pointer (blk==0 = end-of-list) but the clean port only broke on
  `blk==0xffffffff`. blk=0 -> `cnt=MB32(8)=0` -> next_block -> `blk=MB32(0)=0`
  (guest[0]==0) = fixed-point infinite loop. Fix: also break on `blk==0` (a
  null next-ptr is never a valid heap block; blocks live at 0x800Fxxxx+).
  heap_serialize now COMPLETES; framecnt advanced 1461->1547.
- **SESSION 18 — 0x27AC is CORRUPTION, not a dispatch gap (gwatch caught writer):**
  Armed `gwatch` on phys 0x27AC before relaunch; reproduced the crash
  deterministically. Watchpoint entries (frame 1717) show the writer:
  `store_pc=0x8016A8D4` inside generated `br2eu_func_8016A82C` =
  `heap_serialize_tag_01000000`. It is a struct copy writing heap pointers
  (0x800507xx) to dest `0x800027B7` = phys 0x27B7 — the IRQ trampoline.
  `dirty_ram_word_looks_decodable(0x27AC)=0` confirms the bytes are non-code.
  Oracle (SCPH1001) at phys 0x27AC holds real MIPS (`0x03e00008`=jr ra,
  `0x27bdffe0`=addiu sp,-0x20 prologue) — ours differs => genuine corruption.
- **ROOT CAUSE (unrelocated heap-ctx pointers):** decomp_trace (tags 27/28)
  shows the serialize ctx struct at valid heap `ctx=0x800F061C` but
  `ctx+0x14=0x80001D6F` / `0x80001D1D` and `ctx+0x10=0x80001AD9` — LOW
  unrelocated KSEG0 pointers into kernel space (phys 0x1Dxx). heap_serialize
  reads record data from kernel RAM -> garbage tags/offsets -> the heap-A
  output cursor `cur` (param_2=s1=g32(0x801c47b8)) runs away to 0x800027B7 ->
  tag_01000000 writes serialized output over the IRQ trampoline -> next IRQ
  dispatch hits non-code -> unknown dispatch 0x27AC. This is the SAME
  low-RAM-corruption class as the original frame-1717 bug; heap_ptr_fixup
  (70076c8) fixed ctx+0x14 for ONE block shape but ctx+0x10/+0x14 of these
  serialize ctx structs are still unrelocated.
- **CORRECTION (heap_ptr_fixup is FAITHFUL):** decoded generated
  br2eu_func_8016CB20 store sites — it writes only a0+8, a0+4, nested a2
  entries, and a1+0/+4/+8, IDENTICAL to the clean port. It does NOT relocate
  ctx+0x10/+0x14 either. So the clean port is NOT dropping coverage; those
  fields are produced by stream_block_decode (@0x80163154, clean-ported) or
  resolved at serialize time inside the tag serializers. Delegating
  heap_ptr_fixup->generated via br2_call_generated(0x8016cb20) RECURSES
  (fnmap maps the addr back to the clean fn) — reverted.
- **EXACT FORMULA (decoded br2eu_func_8016A82C):** the tag serializer
  computes `write_dest($a1) = MB32(rp+0x18) + ctx+0x14` and
  `read_src($v1) = MB32(rp+0x18) + ctx+0x10`, where MB32(rp+0x18) is the
  per-record offset. So ctx+0x14 = OUTPUT base ptr, ctx+0x10 = INPUT base
  ptr — both must be valid heap pointers. Ours are 0x80001AD9/0x80001D6F
  (raw decompressed values; lzss_decode is a faithful byte-copy so real HW
  has the same raw values => a relocation pass MUST convert them before
  serialize). dest=0xA48+0x80001D6F=0x800027B7 = the trampoline write.
- **RELOCATOR STILL UNFOUND:** heap_ptr_fixup faithful (no +0x10/+0x14),
  lzss_decode/stream_block_decode faithful (no relocation), data_relocate
  is a different format (magic 0x000ed1c4). The ctx+0x10/+0x14 relocation
  must be inside generated heap_serialize (br2eu_func_8016B7C0, ~5000
  lines, inlined per-tag) OR an upstream block-ctx init path. NEXT: in
  generated heap_serialize find where ctx (=MB32(blk+4)) fields +0x10/+0x14
  are read — check whether generated adds a base (blk/heap-image) that the
  clean port's raw MB32(ctx+0x10/0x14) drops. That base-add is the bug.
- **NEXT (concrete, superseded by above):** capture the looping blk sequence — add a bounded-iter +
  blk dump to heap_serialize's `for(;;)` (or read s_decomp tag22 hser_in entries
  via lldb for entry blk, then walk MB32(blk) in g_psx_ram to find the cycle).
  Determine if blk+0 was wrongly relocated by phase-1 (compare blk+0 value vs
  the unresolved-offset form). The heap_ptr_fixup fix is correct for ctx+0x14
  but may need to EXCLUDE next-block-list pointers from relocation.

## Session 19 — serialize ctx relocation + clear_f4ee8 pointer fix

ROOT CAUSE (0x27AC crash, frame 1717): heap_serialize's ctx (blk+4) carries
five I/O base pointers at +4/+8/+0xc/+0x10/+0x14 as 0x8000xxxx offset-encoded
tags into the 0x800F0000 heap image. Generated heap_serialize AND the tag
serializers read them RAW (verified in generated br2eu_func_8016B7C0 /
br2eu_func_8016A82C — psx_cyc_load_word + raw stack-arg stores, no base-add).
heap_ptr_fixup (faithful, verified store-for-store) only covers node +0/+4/+8
and does NOT reach ctx+0x10/+0x14. With fields left at 0x8000xxxx the tag
serializer used phys 0x0DA8-0x2800 (BIOS kernel window — CODE on both
SCPH1001 oracle and OpenBIOS) as record I/O bases -> runaway heap-A cursor ->
serialize_tag_01000000 wrote heap ptrs over the IRQ trampoline at phys 0x27AC.

FIX (commit 72dd5d7): in heap_serialize, after ctx=MB32(blk+4), resolve each
of the five fields 0x8000xxxx -> 0x800F0000 + (v & 0x7fffffff), guarded to
fire only while the field still carries the unrelocated tag. Byte-offset
chosen (heap-image header pointers 0x800f0xxx are byte addresses). NOTE:
heap_ptr_fixup uses *4 word-indexing for the runtime heap — if a later
correctness pass shows the image ctx needs *4 too, revisit scaling.

RESULT: 0x27AC crash gone; game reaches frame 1824.

NEXT BUG (frame 1824): host SIGSEGV in clear_f4ee8_u16s — signature took
uint8_t* (host ptr) but all callers pass guest addrs (fnmap a0, save.c
0x1f8000e8/uint32_t). a0[2] deref'd host 0x1f8000ea. Fixed -> now crashes in
apply() (action.c .ANI section bind) fault_addr 0x100000000: wild
ani + h[7]/h[8] offset => corrupt .ANI blob or ctrl FC_ANI pointer. Commit
1fb3cf8. Investigate whether serialize byte-vs-word scaling or .ANI load is
the source of the bad ani pointer.

## Session 19b — apply() crash fixed; game runs past frame 1824

ROOT CAUSE (frame-1824 apply SIGSEGV, fault_addr 0x100000000):
game/action.c apply() line ~265 did
  GS8(*(uint8_t **)guest_at(GU32(ctrl,0x14)),0)=0xff
The *(uint8_t**) deref is an 8-BYTE host-pointer load, but guest RAM holds
4-byte guest addresses. It concatenated two adjacent u32 cells into a garbage
64-bit pointer (high cell 0x00000001 <<32 | low guest ptr = 0x100000000).
atos confirmed fault at action.c:265. The .ANI header reads earlier in apply
were VALID all along (my temporary h[] sanity guard never fired — removed).

FIX (commit 9fca07d): read the inner cell as guest u32 + guest_at(), matching
the codebase guest-pointer model. Game now runs clean 3+ min past frame 1824
(was deterministic crash). NOTE: *(type**)guest_at and
(uint32_t)(uintptr_t)(host_ptr+off) mistranslations are SYSTEMIC in
battle.c (~3138 uintptr casts + ~71 ** derefs / 17.7k lines) — the remaining
clean-source debt; fix per-site as they crash or via dedicated RE pass.

ALSO this session: heap_serialize ctx I/O-base relocation (72dd5d7) got past
frame-1717 0x27AC; clear_f4ee8_u16s guest-addr fix (1fb3cf8).

## Session 20 — battle frame pipeline unblocked; skeleton-data corruption next

RUNTIME PATH: `--no-launcher --headless`-style runs need the windowed
frontend; plain `--headless` hits a separate early `0xB0` BIOS-dispatch
fail-fast (unrelated). Reliable iteration = `--no-launcher` windowed +
lldb `process handle SIGSEGV` (runtime's own crash handler does NOT fire
on these host SIGSEGVs — silent exit 139, no psx_last_run_report).

FIXES (all committed+pushed to mine/quangdang46):
- 0956297 battle.c: ~70 `*(type**)guest_at` sites read 8-byte host ptrs
  from 4-byte guest cells -> `(type*)guest_at(*(u32*)guest_at(...))`;
  line 6449 write-through -> store guest_of(param_1) as u32.
- caad28c guest-address-as-host-pointer in battle frame path:
  fighter_state_tick -> pad_edge_scan/cmd_input_parse/action_from_input
  now pass br2_gaddr(param_1); battle_frame_tick -> fighter_frame param_2
  now guest_at(param_1); fighter_anim_advance -> keyframe_advance now
  guest_at(param_1),guest_at(param_2) (param_2 was dropped).

VERIFIED PROGRESSION (lldb backtraces, each deeper):
  cmd_input_parse -> fighter_frame -> keyframe_advance -> rtps_point_xform
  -> node_xform_apply -> hitbox_corners_xform -> fighter_pair_update ->
  battle_frame_tick -> battle_intro_flow. The whole input/action/anim
  pipeline now executes inside a real battle frame.

CURRENT BLOCKER: node_xform_apply(param_1=1,param_2=0x20) — skeleton/pose
node tree is corrupt; hitbox_corners_xform reads fighter+0x20 (skeleton
root ptr) = 0. Same upstream corruption class as the part_link pose-chain
walk (action.c diagnostic cap still in place, tags 40-42). Root cause is
in fighter/pose init record setup — the systematic guest-vs-host
mistranslation debt in battle.c/resolve.c/ev.c/menu.c, NOT another
isolated cast. menu.c also has a stale 1-arg keyframe_advance(uint32_t)
prototype vs the real (byte*,byte*) def.

=== Session 21 — systematic guest-vs-host pointer audit (battle/render path) ===

Method: lldb breakpoint+backtrace per crash; each fix verified to advance
the crash deeper. Commits a1fd0a9, 9c9a488, 4584812 (all pushed to mine).

FIXED (all same root class — guest address used as host pointer, or host
pointer truncated to 32-bit guest-address param):
- fighter_pair_update/fighter_contact_resolve/battle_hud_draw/
  fighter_height_check/task_alloc_wrap args: (int)(intptr_t)&DAT_x ->
  (int)guest_of(&DAT_x) so int params receive real guest addrs.
- fighter_attach_run: part-attach jump table read from EMPTY host array
  g_PTR_FUN_801b55c0 -> now reads guest RAM 0x801b55c0 + br2_call_generated.
- rotmat2_pair_build uint* results truncated through undefined4 (4 sites
  feeding gte_rtps_xform1_raw) -> block-scoped uint *rm.
- battle_camera_step rotmat2_pair_build/rtps_basis_xform args -> guest_at.
- angle_gte_vec/vec2_rot literal 0x1f8003xx addrs -> guest_at.
- prim_list_gte_invoke/prim_list_persp_step: int* param_1 callers pass
  guest_at(); internal gte_xform3/range_circle_test guest addrs -> guest_at.
- fighter_trail_emit callers + fighter_reinit_tail (int)(intptr_t)s0 ->
  (int)guest_of(s0).

VERIFIED PROGRESSION: hitbox_corners_xform (param_1 now correct guest
0x1f8000e8) -> fighter_attach_run -> gte_rtps_xform1_raw ->
rotmat2_pair_build -> angle_gte_vec/vec2_rot -> prim_list_gte_invoke ->
prim_list_persp_step -> range_circle_test -> fighter_trail_emit ->
fighter_reinit_tail -> battle_intro_flow (menu.c).

CURRENT BLOCKER: menu.c battle_intro_flow(param_1=0) line 1859 —
_DAT_1f80008c derefs wild ptr 0x3e5000000007fff. g__DAT_1f80008c binding
verified correct (names[]/vars[] aligned, guest_for->guest_at(0x1f80008c));
value overwritten by upstream corruption OR uninit menu/attract path.

DEBT: ~36+ remaining (type*)(uintptr_t)X sites where X may be a guest
address (many are false positives — param already host ptr). Thousands of
(uint32_t)(uintptr_t) sites remain. This is the large clean-source RE
audit — best done by parallel subagents per-file (battle.c, resolve.c,
ev.c, menu.c, action.c) with lldb crash-oracle verification.

=== Session 22 — data-binding root cause + event-stream OOB fixed ===

ROOT-CAUSE FIX (fbc6f00): guest_for()'s hex-parse buffer `char buf[64]`
was NEVER NUL-terminated before strtoul(buf,&e,16). strtoul read past the
hex run into uninitialized stack, producing a garbage 'best' guest address
for EVERY g_DAT_*/s_* binding. All bound globals pointed into unrelated
BSS (zeros) instead of live PSX RAM — rodata tables like the orbit
descriptor table at 0x801b1d70 read zeros. Fixed with buf[n]=0 at both
strtoul sites + noinline on weak guest_at/guest_of so calls bind to the
strong real-RAM clean_bridge implementations. VERIFIED: g_DAT_801b1d70
now = 0x1117832e8 = guest_at(0x801b1d70) = real RAM with correct EXE
bytes (00 01 02 03...). This was THE systematic data-corruption bug
affecting thousands of bindings.

EVENT-STREAM OOB FIX (fd25718): ev_stream_dispatch indexed
PTR_FUN_801b4e68[(uint8_t)*pbVar1] but the clean table only ports 24
handlers while the real guest table at 0x801b4e68 holds 48+ entries
(verified in EXE: consecutive 0x801xxxxx code ptrs). Opcode 30 (0x1e)
indexed OOB -> null call -> EXC_BAD_ACCESS at PC=0. Now opcodes >=24
dispatch to the real guest handler via br2_call_generated(h, ctrl_g,
motion_g, pc_g, 0); clean handlers kept for 0-23.

VERIFIED PROGRESSION: battle_frame_tick -> fighter_frame ->
action_commit -> ev_stream_dispatch -> (guest handlers) ->
prim_group_build/orbit_fx_spawn.

CURRENT BLOCKER: prim_group_build(param_2) via orbit_fx_spawn —
param_2 = DAT_801c43a8[orbit_type+0x1e] returns a bad guest pointer.
0x801c43a8 is a slot->descriptor-handle table (BSS) populated from
0x801c47b8 master table by save.c lifecycle code (save.c:6085-6092,
6995). aux_tbl_store_a is a weak no-op (bind.c:2004). The requested
orbit descriptor was never allocated/aliased -> stale/garbage handle.
Deep render-resource (prim/orbit descriptor) lifecycle RE remaining.

## Session — frame-348 recursion ROOT-CAUSED + FIXED (14b83f4): stale clean_table thunk wrappers

RESOLVED the long-running frame-348 dispatch-recursion crash. Live [NESTCYC]
ring instrumentation proved it was NOT an IRQ/event-resume bug (the N+12
hypothesis) but a stale clean_table.c: it still held wrap_* entries for
cd_read_finish_cb (0x80198838) and cd_reg_window_op (0x8019B944), both
converted to pure br2_call_generated thunks. Every guest jal to those
addresses ping-ponged generated->clean->generated->clean->generated, nesting
a psx_dispatch_call frame per codec register-window op; the level-triggered
CD IRQ (irq_flag until 0x1F801803.1 ack) re-fired on each nested MMIO
boundary -> unbounded recursion -> guard trip ~frame 348.

FIX: `python3 game/gen_clean_table.py` regenerated clean_table.c, dropping
both stale wrappers (1118 wrappers / 75 skipped). Generated callers now use
flat CPS tail-dispatch — no clean boundary, no nested frame.

VERIFIED: two clean-build runs pass frame 348, climb past 14000 frames,
GPU draws + LZSS decomp active, zero recursion trips. Committed + pushed.

REMAINING BLOCKER (next): display BLACK despite frames advancing — CD sector
drain lossy (bfrd_set=5867 vs bfrd_clear=14, ring_dropped=2591). Fighter
file 0xb1 / sector-buffer-ready consumer handshake needs its own trace.

## Session — battle VBlank wedge: BIOS-0x29CC TCB-restore hypothesis RULED OUT

Deep IRQ/exception investigation of the ~2073-frame battle freeze. Decisive
instrumentation results (all committed diagnostics in interrupts.c / traps.c /
debug_server.c):

- RFE escape path is HEALTHY: rfe_mark_total == esc_fired (~31818),
  esc_dec_fiber=0, esc_dec_noexc=0, esc_dec_sentinel=0. Every armed RFE
  longjmps correctly. NOT an RFE/fiber/sentinel bug.
- Anomaly signature: handler's psx_dispatch returns NORMALLY (jmpval=0) with
  cpu->pc==0, escape_reason=NONE, in_exception still 1 at the pc==0 moment.
  lastfunc=0x29CC (BIOS ReturnFromException / ChangeThread context restore).
- pc=0 is the normal nested-call C-return contract leaking to the outermost
  boundary -> psx_dispatch_check_return_boundary re-polls the still-pending
  level-triggered VBlank IRQ -> re-delivers -> pc=0 storm. I_STAT never acked.
- HOST save path RULED OUT: psx_save_context_to_tcb never writes resume_pc==0
  (tcbsave_zero_count=0). The sentinel assert rejects 0x80000048 but NOT 0.
- 0x29CC TCB-restore RULED OUT: decoded generated func_000029CC — it does
  k1=*(0x108)->*(pcb)->TCB, k1+=8, lw $k0,128($k1) = *(TCB+0x88), jr $k0; rfe.
  A psx_dbg_null_jr probe at that exact jr showed gpr[26] != 0 on EVERY call
  (rfe29cc_null_count=0). 0x29CC always resumes a VALID pc. Current TCB 0x8FE0
  +0x88 = 0x8019A600 (vsync_wait_n) — valid.

CONCLUSION: pc=0 is a GENERIC null indirect jump (jr/jalr/jr-ra -> 0) produced
inside the IRQ handler's call tree AFTER 0x29CC resumed — i.e. in game/kernel
code reached post-RFE, while in_exception==1. NOT the BIOS context restore.
Next: a general null-jump-target tracer (emitter-level cpu->pc=gpr[X]==0
watchpoint) or the Beetle/DuckStation oracle for IRQ/RFE timing. Downstream
symptom seen earlier: FAIL-FAST unknown dispatch addr=0x04000000.

## Session — wedge terminal signature: BIOS service-gate loop -> 0x04000000

Post-mortem of the FAIL-FAST (frame 6471, dispatch_depth=3, epc=0x8019A648,
cause=0x400 IRQ, pc=0, ra=0, a0=1, a1=0x801F68F0):

- dispatch_tail ends in a tight repeating `0xB0,0xB0,0xC0` loop = the OpenBIOS
  kernel-call GATE stubs at phys 0xA0/0xB0/0xC0 (native_stub table,
  OpenBIOS_dispatch.c:8949). These decode a target from embedded lui/addiu and
  route to callB0. Repeating B0/C0 = BIOS service spam = IRQ livelock symptom
  (VBlank level-triggered, I_STAT never acked because handler exits pc=0 early).
- Decoded callB0 (func_00002AA0): t0=0x49CC service-table base,
  t2=*(0x49CC + t1*4); jr $t2. Verified table[0x35] (t1=0x35 at crash) =
  0xBFC04AA8 = VALID BIOS ROM ptr, NOT 0x04000000. Service table not corrupted
  at that index; 0x04000000 comes from a different/later wild jump.
- pc=0 remains the root: handler's psx_dispatch returns pc=0 (nested C-return
  contract) before its RFE -> boundary re-polls pending VBlank -> storm.
  Still need the exact guest insn producing the first pc=0 (game-side handler
  0x8019A87C is fn-traceable; OpenBIOS BIOS funcs are not in fn_entry ring).

## Session — ROOT CAUSE FOUND: task-pool node allocated into exception-vector page

Decisive capture via `gwatch` (guest_watch.c — mprotect write watch, catches
DMA + all stores that bypass the wtrace CPU-store hook). wtrace ring had ZERO
vector writes -> corruption is NOT a normal CPU store; gwatch `dma.ch=-1`
confirmed CPU store but `store_pc`/`sp`/`func` are STALE globals
(g_debug_last_store_pc = hot cd_timer_read poll loop). The REAL writer is the
`rip` host fault PC -> resolved via vmmap __TEXT base + atos.

WRITER = `script_stream_tick` (game/battle.c:8445, guest 0x80137824), a
task-callback bytecode-stream interpreter. Lines 8491-8506 write quad/sprite
VERTEX BYTES through `*param_1` and `param_1[1]` dest pointers:
  *(char*)guest_at(*param_1 + 0xc/0xd/0x14/0x15/0x1c/0x1d/0x24/0x25) = coords
Observed write addrs 0x80000084/85/8C/8D/94/95/9C/9D decode EXACTLY:
  param_1[0] = 0x80000078  (0x78+0xc=0x84 ... +0x25=0x9D)
  param_1[1] ~= 0x800000A0 (0xA0+0xc=0xAC ... )
Both dest pointers sit in the EXCEPTION-VECTOR page (phys 0x78/0xA0 =
KSEG 0x80000078/0x800000A0). Writes recur every ~7 battle frames -> overwrite
IRQ vector handler-addr load at 0x80000084 -> jr k0=0 -> pc=0 IRQ storm -> wedge.

PROVENANCE (clean-C):
- script_stream_tick state set by task_alloc_wrap_b/d -> task_node_alloc
  (battle.c:8018). Node data buffer is INLINE at node+0x1C (piVar1[4]=guest_of
  piVar1+7); allocated size = param_6+0x1c via task_pool_alloc.
- param_1[0]=br2_gaddr(node_data)=0x80000078  =>  the task NODE itself is at
  guest phys ~0x5C, i.e. INSIDE the exception-vector page.
- task_pool_alloc (battle.c:10844) returns `iVar5+0x10` walking a free list
  (block hdr: +4 next, +8 size, +0xc flags). A free-list block header sits at
  guest 0x8000004C -> the POOL FREE LIST IS CORRUPTED to include a block in
  low memory (phys 0x4C).

ROOT CAUSE: task-pool heap/free-list corruption. A block `next`/`prev` link (or
the pool head ptr at param_1*0x3c+param_2*0xc-0x7fe00d98) points into phys
0x4C, so allocations hand out nodes in the vector page; script_stream_tick's
per-frame quad writes then trash the IRQ vectors. Same memory-corruption class
as the documented heap_serialize low-RAM overwrites — need to find what writes
a wild pointer into the pool free list (or mis-sets the pool arena base).

## Session — refined: dest pointers are GARBAGE, not a low-mem node

Added sst_bad latch in script_stream_tick (battle.c) + tpool_bad latch in
task_pool_alloc; exposed via freeze_check. Result on live wedge:
  sst_bad_count=693, sst_bad_state=0x12060790 (HOST ptr of stream state),
  sst_bad_d0=0x28, sst_bad_d1=0xFA0, sst_bad_sbase=0x801B31B0,
  sst_bad_spc=0x801B31B4 (script base/pc VALID).
  tpool_bad_count=0 -> the free-list walk does NOT drop <0x80100000, so the
  earlier "corrupt free-list link" model is WRONG for this path.

The quad-vertex DEST pointers param_1[0]/param_1[1] hold GARBAGE small values
(0x28, 0xFA0; a sibling instance reaches 0x80000078 -> writes phys 0x84).
script_stream_tick writes sprite bytes via guest_at(*param_1+off) /
guest_at(param_1[1]+off) -> lands in kernel/vector page -> IRQ vector corrupt.

PROVENANCE of the dest ptrs (task_alloc_wrap_b/d, battle.c:7272/8733):
  puVar2[0]=param_1=br2_gaddr(puVar14)
  puVar2[1]=param_2=(undefined4)(uintptr_t)puVar14 + 10   <-- RAW truncated
             host ptr, NOT br2_gaddr'd (inconsistent with param_1).
br2_gaddr (clean_bridge.c:109): guest_of(p) returns 0 when p is not a
recognized RAM/scratchpad host ptr -> falls back to (uint32_t)(uintptr_t)p
= low-32 of a host pointer = garbage "guest address". So BOTH dest fields can
hold garbage: param_1[0] via br2_gaddr fallback on an unrecognized puVar14,
param_1[1] via raw host-ptr truncation.

ROOT CAUSE (refined): clean-C guest<->host pointer-translation bug in the
effect/sprite stream task setup. puVar14 (the sprite node's inline GPU-cmd
buffer, node+0x1C) is either a bad/near-null host pointer or its guest
address is computed wrong, so the stream state's dest pointers become garbage
and script_stream_tick's per-frame quad writes corrupt the exception vectors.
NEXT: verify puVar14's actual value & whether the sprite node's +0x10 data
ptr is valid; check every `(undefined4)(uintptr_t)ptr` store that should be
br2_gaddr'd in the effect-task path (task_alloc_wrap_b/d and siblings).

## RESOLVED — battle IRQ wedge FIXED (commit d8610ab)

The dest pointers were NOT from task_alloc_wrap_b/d in battle.c. Creation-time
arg latches (wd_*/wb_*) proved task_alloc_wrap_d fires 99x with bad args, but
the REAL caller is a SECOND call site: options_menu_alloc in game/menu.c:13560.

ROOT CAUSE: options_menu_alloc read the task node's +0x10 inline-buffer ptr via
  s6 = MB32(guest_of(blk) + 0x10)
blk is already a GUEST address (task_node_alloc returns guest_of(node)), so
guest_of() — which expects a HOST pointer — returned 0 -> s6=0. All 100
sprite-stream dest ptrs collapsed to small offsets (s2=s6+i*0x28 -> 0xF78,
s0=s6+0xfa0 -> 0xFA0, param_3=s6+i*4+0x2260 -> 0x23EC at i=99, matching
wd_last_p1/p2/p3 exactly). script_stream_tick then wrote quad-vertex bytes via
guest_at(*param_1+off) into low mem -> overwrote 0x80000084 IRQ vector ->
jr k0->0 -> pc=0 storm -> wedge ~2k frames into battle.

FIX (menu.c:13556): read the field by blk's guest-addr value:
  s6 = MB32((uint32_t)(uintptr_t)blk + 0x10u);
matching the MB32(blk + 0x10) idiom used by sibling fns (menu.c:12449).

Also fixed earlier: task_alloc_wrap_d host-ptr truncation args
((undefined4)(uintptr_t)puVar14 + N) -> br2_gaddr(puVar14 + N).

VERIFIED: battle runs 90k+ frames (~270fps), combat inputs register, gs=8
sub=8 stable, sst_bad/wd_bad/pczero all 0, exception vector healthy
(3C1A0000 275A27AC 03400008 00000000). Committed d8610ab -> mine/master.

OPEN QUESTION for next session: does combat actually deal damage / do rounds
end? gs=8 sub=8 stable for 48s+ sampling. Need to locate fighter HP + round
timer state (battle.c uses decompiled generic names; fighter struct accessed
via guest_at(host_ptr+off), +0xbc sub-struct, +0xc0 counter). If rounds never
end, combat damage may be stubbed -> real gameplay bug to chase next.

## Session — versus->fight root-caused to MISSING character .ANI (FC_ANI=0)

NAVIGATION (committed 1a040b5): the earlier "battle" was character/vs-select,
NOT combat. Correct path = 2P VS mode (battle+0x9b=2, +0x9c=1) ->
select_screen_tick -> CROSS confirm both SIO slots -> START at ph==7 pad gate.
tools/nav_to_fight.py automates it; reaches gs=8/stg=8 (post_battle_dispatch).

FIGHT-INIT CRASH (new, precise): host sample shows
  battle_flow_state_machine -> fighter_frame -> action_commit ->
  ev_stream_dispatch -> br2_call_generated -> psx_unknown_dispatch ->
  psx_fatal_halt  ("DISPATCH FATAL: misaligned target 0x00030001").
NOT the IRQ/pc=0 storm — a clean fatal halt. Guest regs: a0=0x1F8000E8 (P1
ctrl), a1=0x801F5328 (P1 motion), epc=0x8019A648 (vsync_wait_n spin ctx).

ROOT CAUSE (verified live):
- fighter ctrl+0x30 (FC_ANI) = 0 for BOTH fighters; motion+0x00 (MO_SEC0)=0,
  motion+0x84 (event/script PC)=0; whole motion object zeroed; HP=0.
- ctrl+0x20 node_root=0x801e1b3c, +0x38 motion ptr, +0x3c AI ctx ARE set ->
  fighter STRUCT allocated, but .ANI resource never linked.
- action.c apply() (FUN_8017a05c) reads FC_ANI and EARLY-RETURNS on 0 ->
  motion stays zeroed. action_switch_guest then computes
  stream = FC_ANI(0)+off = 0 -> motion+0x84=0.
- ev_stream_dispatch reads opcode at scriptPC=0 -> guest[0]=0xFF -> op>=24 ->
  handler table 0x801b4e68+op*4 OOB -> dispatch target 0x00030001 -> fatal.
- RAM scan: searched exact 16-byte .ANI header sigs of ALL 14 characters
  (cd/<CHAR>/<CHAR><N>P.ANI) across full 2MB guest RAM -> ZERO hits. The
  character .ANI blob was NEVER loaded (not a link bug — the file isn't there).
- .DAT/model data DID load (node_root populated) -> PARTIAL char load; .ANI
  specifically missing. Consistent with documented CD sector loss
  (ring_dropped) OR the 2P-VS resource-load request path.

OPEN: prior session (post-d8610ab) reached STABLE combat gs=8 sub=8 — so .ANI
loaded on THAT path. Either 2P-VS selects a char/mode whose .ANI load differs,
or a regression since. NEXT: determine which char ids 2P-VS selected
(P2 ctrl byte2=0x01 byte3=0x05), confirm their .ANI LBA was read from CD, and
check whether the .ANI file-load (cd_file_load_sm) was requested/dropped.

### versus->fight deep-dive (cont.) — char resource load in-flight at crash

- Char ids: P1 ctrl+0x03=0, P2 ctrl+0x03=5. FC_PLAYER +0x02 = 0/1. FC_ANI +0x30=0 both.
- node_root (ctrl+0x20)=0x801e1b3c points to an EMPTY struct too -> WHOLESALE
  char-resource absence, not .ANI-specific. .DAT/model also not landed.
- CD DMA IS working: dma_cdrom_history shows full 512-word sector transfers
  streaming to guest 0x8011E800+ early, and CONTINUING to a staging buffer
  0x800D0000-0x800DE000 (~56KB ring) at frames 1567-1577, ~16 frames before
  the psx_fatal_halt (~1593). Load was IN-FLIGHT when ev_stream_dispatch fired.
- Staging buffer 0x800D0000 holds dense binary (not a clean .ANI header) ->
  char files arrive packed/encoded or .ANI is extracted+relocated by a later
  generated step that sets ctrl+0x30 (FC_ANI writer is generated, not clean).
- ring_dropped=523: sectors overwritten before guest consumed (8-deep ring,
  s_sector_ring). At authentic speed drops are "hardware-faithful" -> guest
  consumption (CD data-ready IRQ / DMA drain) may be lagging in the
  clean-C battle path = the deferred-IRQ contract again, OR genuine slowness.
- HYPOTHESES: (A) async-load RACE — fighter_frame runs ev_stream_dispatch
  before the char .ANI link completes (load-complete gate broken/skipped);
  (B) .ANI inside packed char archive needs an unpack/relocate step that
  didn't run; (C) CD consumption IRQ starving -> ring drops -> incomplete file.
- NEXT: find the generated fn that writes ctrl+0x30 (FC_ANI) — likely a
  CD-completion callback or deferred task; check whether it's gated on a
  "resources ready" flag that battle_flow should wait on before stage 8.

### versus->fight ROOT CAUSE CONFIRMED — CD sector drops corrupt .ANI

Reproduced twice. Run A: FC_ANI=0 (load didn't finish -> null stream ->
0x00030001 fatal). Run B (.ANI loaded): fatal at addr=0x80122F0C which is a
STRING/DATA table ("OFFBEAST"/"LEVER"/"REV"/"SPOSE"), NOT code.

VERIFIED DATA-PATH BUG:
- P1=JEN(char0) .ANI@0x8006ba04, P2=MAR(char5) .ANI@0x800ce958 — headers load.
- Diffed P2 MAR .ANI (48500B) vs disc MAR1P.ANI: first 0x16a8 bytes
  (~3 sectors) BYTE-IDENTICAL, then ~40KB diverges to dense foreign data
  matching the 0x800D0000 staging buffer -> .ANI body overwritten/incomplete.
- ring_dropped grew 523 -> 2767. bfrd_set=6325 vs bfrd_clear=14: guest CD
  consumption/ack handshake barely runs -> 8-deep s_sector_ring overflows ->
  sectors dropped -> file data corrupt.
- accel_consumer_waits=0: disc at divisor=1 (authentic); drops are "faithful"
  only if guest genuinely slow — here consumption is impaired by the
  clean-C battle path (deferred-IRQ/coroutine contract), NOT real HW.
- Corrupt .ANI -> S4 relocated ptrs land in string table -> ev_stream_dispatch
  -> br2_call_generated -> psx_unknown_dispatch -> psx_fatal_halt.

ROOT: CD data-ready IRQ / DMA drain does not keep up during the clean-C
battle_flow resource load -> ring drops -> corrupt fighter files. The fix is
the deferred-IRQ/consumption path (same contract as vsync_wait_n), NOT a
disc-speed shim (non-faithful). NEXT: find why CD consumption IRQs stall in
the battle_flow clean-C dispatch window; check pending_dataready/irq_flag
delivery vs clean_try_dispatch nesting depth.

### Session N+1 — third repro + IRQ-starvation hypothesis REFINED

Third versus->fight run: FC_ANI=0 again (null-stream fatal via
ev_stream_dispatch->br2_call_generated->psx_unknown_dispatch->psx_fatal_halt).
ring_dropped froze at 523 once the read went idle (CD stat idle, irq_flag=0,
sector_read_pos stuck) — drops accumulate ONLY during the active read window,
then the load "finishes" with FC_ANI still 0 -> hang-then-fatal.

EMPIRICAL data this session:
- Sector production is STEADY ~1 sector/frame (cdrom_sector_history frames
  1654-1655, sequential LBAs) — NOT bursting. 8-deep ring needs a >8-frame
  consumption stall to drop; drops are cumulative over the whole load.
- clean_trace (last-512 dispatch ring) dominated by fifo_space_check
  (0x801A4230, GPU FIFO space poll) x7.3M from ra=0x801A1FFC — normal per-frame
  GPU command submission, a red herring, NOT the stall.
- CRITICAL REFINEMENT: clean-C advances guest cycles ONLY via br2_irq_poll()
  (clean_bridge.c:290, psx_advance_cycles(256)+psx_check_interrupts_at). Every
  clean spin that charges cycles self-checks IRQs. So naive "clean-C spins
  without IRQ check" is NOT the drop mechanism — the starvation is subtler:
  (a) generated-code consumption coroutine cd_read_step_cb/cd_read_finish_cb
      (cd.c:776/803, delegated via br2_call_generated across the deferred-IRQ
      boundary) not being re-invoked promptly per sector, or
  (b) a consumption-rate deficit vs 2x production (mode_reg=0xA0, game-set),
      or (c) psx_check_interrupts_dispatch_entry not delivering CD INT1 at the
      clean-function granularity the load path needs.
- mode_reg=0xA0 = game-requested 2x (faithful; BR2 reads at 2x on HW too).
  Production timing CDROM_SINGLE_SPEED_SECTOR_CYCLES=451584 (~13.4ms 1x,
  ~6.7ms 2x) is accurate. INT1 data-ready is LEVEL-triggered from irq_flag
  (cdrom.c:996-1010) — re-fires while data ready, faithful.

NEXT (concrete): instrument cdrom.c to timestamp ring_dropped increments vs
guest PC / clean-function, OR check whether cd_read_step_cb (the per-sector
consumer, generated coroutine) is invoked once per produced sector during the
char-load window. The consumption side is the suspect, not production.

### Session N+2 — REFRAME: ring_dropped is FAITHFUL; real suspect = intro_movie_state / buffer overlap

KEY CORRECTION: cdrom.c:1600-1618 documents that sector-buffer overwrite is
HARDWARE-FAITHFUL clobbering ("the sector buffer is overwritten on schedule
(hardware clobbers the FIFO the same way)"). ring_dropped=523 (~55% of 942
sectors) is NORMAL PSX streaming behaviour for sectors the game does not
consume in time — likely a RED HERRING for the .ANI failure, NOT the cause.
bfrd_clear=13 = clear_sector_buffer ran ~13x (per read-session flush), normal.

NEW CONCRETE LEAD — decomp_trace file_load sequence (this run, FC_ANI=0):
  0x14->0x800D0000 (big packed decode -> 0x80010000+ blocks)
  0x8B->0x800D0000, 0x16->0x800D0000  <- LAST loads before crash
These are intro_movie_state (game.c:1795, guest 0x8012c0b4) resources, NOT the
character .ANI (which uses a separate loader — never appears in file_load).
CRITICAL: .ANI blob sits at 0x800ce958 (Run B) spanning to 0x800DA6CC; the
intro staging buffer 0x800D0000 lies INSIDE that span at offset 0x16a8 —
EXACTLY where the Run-B .ANI byte-diff diverged. Staging writes to 0x800D0000
overwrite the .ANI tail.

TWO LIVE HYPOTHESES:
  (A) BUFFER COLLISION: char .ANI placed at 0x800ce958 overlaps the
      intro_movie_state staging cursor 0x800D0000 (_DAT_801e3cfc). If the
      versus->fight path runs intro_movie_state (pre-fight cinematic /
      attract replay) AFTER .ANI is resident, staging clobbers .ANI tail.
      On HW the order/buffers must not collide -> suggests wrong dest or
      wrong sequencing in our port.
  (B) WRONG STATE: versus->fight may be entering intro_movie_state (loading
      cinematic file_ids 0x14/0x15/0x8b/0x16) instead of / before real fight
      init, so the character .ANI link (FC_ANI) never runs -> FC_ANI=0 ->
      ev_stream_dispatch null-stream fatal. The last decomp activity IS
      intro_movie_state file_loads, supporting this.
NEXT: determine whether versus->fight is SUPPOSED to run intro_movie_state
(pre-fight cinematic) or whether it's a state-machine bug; find the char
.ANI loader (separate from cd_file_load_sm) and its dest/gating; check if
_DAT_801e3cfc heap cursor is shared/reset between intro staging and .ANI.

### Session N+3 — corrected leads; char .ANI is an async pre-load, action_enter assumes it done

CORRECTIONS to prior hypotheses:
- menu_res_load_sm (save.c:4098, guest 0x801709fc) is a SAVE/MEMORY-CARD
  state machine (save_channel_init_commit/save_load_state_machine/
  save_state_machine_alt), NOT the character-resource gate. The
  battle_flow case-4 gate (menu.c:692-696) waits on save/memcard +
  _DAT_1f800090, unrelated to .ANI.
- battle_state_init (menu.c:4427) only zeroes state/phase fields; does
  NOT load .ANI.
- action_enter (action.c:660) -> apply() reads FC_ANI(ctrl+0x30) and
  early-returns on 0. It ASSUMES .ANI already resident; it does NOT
  trigger the load. So FC_ANI must be set by an async resource task
  kicked off earlier in versus->fight setup (char-select confirm /
  battle entry), BEFORE action_enter_fwd (menu.c:723-724).

CONCLUSION: the versus->fight bug is a LOAD-ORDERING / completion-gate
failure in the async character-.ANI resource pipeline:
  (a) FC_ANI=0 runs: the async .ANI load never completed/linked before
      action_enter -> apply() early-return -> zeroed motion ->
      ev_stream_dispatch null-stream fatal.
  (b) FC_ANI=corrupt runs: .ANI loaded (header ok) but its tail
      (>=0x800D0000) was clobbered by intro_movie_state staging
      (_DAT_801e3cfc=0x800D0000) -> relocated ptr into string table ->
      ev_stream_dispatch fatal.
The .ANI blob loader + its completion flag are GENERATED code (clean
code only reads FC_ANI). Next: locate the generated fighter-resource/
.ANI loader (Ghidra disasm of fighter-init region, or wtrace on the
.ANI staging region during a fresh nav_to_fight run) and find the
completion gate action_enter is supposed to wait on.

### Session N+4 — ROOT CAUSE CONFIRMED (byte-exact): char .ANI overlaps cinematic staging arena

DEFINITIVE reproduction (fresh run, nav_to_fight -> FIGHT, frame ~5495):
- P1 JEN .ANI FC_ANI=0x8006ba04, P2 MAR .ANI FC_ANI=0x800ce958 (deterministic).
- MAR1P.ANI is 0xBD74 bytes -> occupies 0x800CE958..0x800DA6CC.
- Byte-diff loaded .ANI vs disc: IDENTICAL until offset 0x16A8 = guest
  0x800D0000, then diverges into staging/decode data. 100% reproducible.

DMA + decomp_trace prove the collision in time AND address:
- .ANI DMA load: frame 3365, dst 0x800CE958..0x800DA158 (lba 3818-3841).
- intro_movie_state / cinematic decode uses 0x800D0000 as staging AND as
  LZSS decode SOURCE: stream_block_decode reads src 0x800D0030/0x800D3470/
  0x800D5C60/... -> outputs to 0x8001xxxx-0x8006xxxx. Then file_load
  0x8B->0x800D0000, 0x16->0x800D0000 (decomp seq 4075/4076).
- Later loads ALSO hit 0x800D0000 at frames 5451/5474/5487 (lba 8575/61/8765)
  — squarely inside the live .ANI range 0x800CE958-0x800DA6CC.

CONCLUSION: the character .ANI heap allocation at 0x800CE958 overlaps the
fixed cinematic/decode staging arena at 0x800D0000(+). A 48KB .ANI cannot
fit below 0x800D0000 (only ~5.8KB room), so 0x800CE958 is a BAD heap
placement — the char-resource allocator is handing out memory inside the
reserved 0x800D0000-0x800Exxxx staging/decode arena. On HW the .ANI must
land above the staging arena (or staging elsewhere); our heap cursor /
arena bound is mistranslated.

NEXT: find the char-resource heap allocator + arena bounds (the heap that
produces 0x800CE958). Heaps seen: _DAT_801c47b8 (base 0x80010000) and
_DAT_801e3cfc (0x800d0000/0x800dc800/0x800f0000 staging cursors). Determine
which heap owns 0x800CE958 and why its top bound allows overlap into the
reserved 0x800D0000 arena. Likely a guest->host pointer or arena-limit bug.

### Session N+5 — caller-ra provenance + NEW deterministic deadlock (fifo_space_check)

Instrumentation (commit 3053320): clean_caller_ra() exposes gpr[31] at
clean-port entry; file_load decomp_trace `len` field now carries the
generated caller's post-jal ra. Live data (attract/menu loads):
  fid 0x80->0x801C47C0 ra=0x80126260; fid 0x89->0x8003F5DC ra=0x80126260
  fid 0x1c/1d/1f->0x800F0000 ra=0x801263B0/428/430; fid 0x8a->0x80010000
  fid 0x90/91->0x800DC800 ra=0x80126594; fid 0x14/8b/16->0x800D0000
  ra=0x80126664 (intro_movie_state region — confirms staging caller).

NEW BLOCKER (deterministic, 2/2 runs): nav_to_fight reaches vs_init then
the game FREEZES at frame ~1650, BEFORE char-select/.ANI load. State:
- clean_trace = fifo_space_check (0x801A4230) x7.3M called from 0x801A1FFC
- fifo_space_check reads u16 @0x1f801120 (=0), @0x1f801124 (=0),
  @0x1f801128 (=0); base=*(0x801ff260)=0x2f37, lim2=*(0x801ff264)=0x3c.
  a0=0 < base -> a0 += 0x10000; v0=(0x10000-0x2f37)>>3=0x1A19 >= lim2
  -> returns "space exhausted" forever -> caller spins.
- CD idle (reading=0), MDEC stalled (busy=0,input_count=0,
  output_pos frozen 622080/13MB), i_stat=0 pending=0, frame frozen.
  TOTAL pipeline deadlock: nothing progresses.

The 0x1f8011xx FIFO-depth vars are game-side libgpu command-queue
trackers (NOT GPU regs; real GP0 is 0x1F801810). They stay 0 -> the
game believes the GPU command FIFO is permanently full. The drain side
(GPU command consumption / completion that should advance these vars)
is not running. Hypothesis: same deferred-IRQ/task-scheduling starvation
class as the CD-consumption issue — the async GPU-command drain isn't
driven while the main thread spins in the clean fifo_space_check loop.

This deadlock fires during the intro_movie_state cinematic (post
vs_init), so it MASKS the .ANI-load path — we never reach file_id 0x40
in these runs. Earlier sessions reached frame ~5495/.ANI because the
timing differed; the deadlock is timing-sensitive but now reproduces.

NEXT: (a) determine what writes 0x1f801120/124/128 on real HW (GPU DMA
completion? a libgpu drain task? an IRQ?) and why it's not running;
(b) get the .ANI (fid 0x40) caller ra by getting past this deadlock or
by tracing the select-screen resource path directly.

### Session N+5b — CORRECTION: the "fifo deadlock" is the pc=0 wedge

Deeper probe of the frame-1650 freeze shows it is NOT a FIFO/timer bug:
- get_registers: pc=0x00000000, epc=0x8019A600, ra(gpr31)=0x80126664
  (intro_movie_state region), cop0_cause=0, sr=0x40000401.
- dispatch_tail addrs are LOW memory (0x00004000,0x000016E4,0x00003Fxx)
  = guest executing from physical ~0, i.e. it branched/jalr'd to NULL.
- guest cycle counter frozen (cycles_to_next_event constant 654619),
  all 3 timer counters frozen, clean_trace count frozen -> total halt.

So versus->fight -> intro_movie_state (ra 0x80126664) performs an
indirect call/jump through a NULL/bad pointer -> PC=0 -> executes the
null page -> hard freeze. This is the SAME pc=0 signature as the
earlier versus->fight IRQ-storm wedge (todo #9). fifo_space_check x7.3M
was the last clean fn before the wild jump, a red herring.

LINK to Session N+4: the .ANI@0x800CE958 / staging@0x800D0000 overlap
corrupts pointers; a corrupted function/dispatch pointer in the
cinematic path is the likely source of the null indirect target. The
pc=0 wedge and the .ANI-corruption fatal are probably the SAME root
(memory overlap) manifesting at different points.

### Session N+5c — pc=0 localized: battle_flow_state_machine case 8, FC_ANI=0

Traced the versus->fight pc=0 crash to its exact site:
- Call chain: generated 0x80126xxx orchestrator -> jal
  battle_flow_state_machine (0x8016FCC4, CLEAN) -> ... -> PC=0.
- Live battle block @0x1f800000: DAT_1f80009c=0x01 (2P-VERSUS mode),
  DAT_1f800001=0x08 -> case 8 = in-fight per-frame logic.
- Case 8 calls prim_list_gte_invoke (battle.c:11848) which loops
  task_invoke(piVar4[3],piVar4[2]) over the prim/task list, plus reads
  fighter objects _DAT_1f800110/_DAT_1f800218. task_invoke itself is
  null-safe (fn_lookup->NULL skips), so the wild jump is a generated
  jr/jalr through a zeroed pointer in the prim/task/fighter data.
- FC_ANI (ctrl+0x30): P1=0x1f800118 -> 0, P2=0x1f800220 -> 0. BOTH
  fighters have FC_ANI=0 -> the char .ANI never linked. With no .ANI,
  action/anim/task data derived from it is absent -> downstream null
  deref -> PC=0. This is the FC_ANI=0 manifestation (vs the earlier
  FC_ANI=corrupt -> ev_stream_dispatch fatal).

COMMON ROOT: the char .ANI resource (file_id 0x40) is not reaching
FC_ANI correctly — whether via failed load, bad heap dest
(0x800CE958 overlaps 0x800D0000 staging, Session N+4), or the
select/battle resource path not completing. All versus->fight crash
signatures (pc=0, ev_stream_dispatch fatal, FC_ANI=0/corrupt) trace
to the char .ANI resource not being resident+intact when battle
case 7/8 consumes it.

NEXT (unchanged): find the char-resource loader/heap that produces
the .ANI dest and why FC_ANI stays 0 / the blob overlaps staging.
Ghidra MCP backend (port 8080) still down; generated-code archaeology
or a gwatch on FC_ANI during a run that reaches char-select needed.

### Session N+5d — versus->fight: fighter objects EMPTY, not just FC_ANI

Corrected an earlier mis-read: the FIGHTER OBJECTS (not the scratchpad
ctrl blocks) are what battle case 8 dereferences.
- _DAT_1f800110 -> 0x801f5258 (P1 fighter obj), _DAT_1f800218 ->
  0x801f52a0 (P2 fighter obj). These are heap objects, NOT the
  0x1f8000e8/0x1f8001f0 scratchpad ctrl blocks (those are the per-side
  arg/ctrl records; FC_ANI lives inside the heap object).
- Both heap objects are ~EMPTY: first 0x10 bytes zero, FC_ANI(+0x30)=0,
  and the case-8 fields +0x10 (prim list), +0x24, +0x34 are 0. Only a
  few heap back-pointers (+0x18/+0x1c/+0x3c/+0x40) are set.
- battle_flow case 8 does guest_at(obj+0x10) -> prim_list_gte_invoke
  and guest_at(obj+0x34) -> uVar3 -> guest_at(uVar3): with those fields
  0, guest_at(0)/guest_at(NULL) -> wild jump -> PC=0. task_invoke is
  null-safe so the fault is the prim-list/obj+0x34 deref, not task_invoke.

State-machine path (battle_flow_state_machine is CLEAN, menu.c:549):
  case0->1 select_screen_tick -> case2 match_setup_apply+battle_state_init
  -> case3 battle_init -> case4 menu_res_load_sm -> case5
  battle_struct_init+task_table_init+sprite_desc_table_init+
  battle_session_arm -> case6 match_setup_apply+battle_state_init+
  per_side_setup+action_enter_fwd(both)+fighter_anim_table_init+
  sprite_anim_chain_build -> case7 wait -> case8 in-fight (CRASH).

ALL of battle_init/battle_struct_init/per_side_setup/action_enter_fwd/
fighter_pair_update/post_battle_dispatch/battle_sides_commit/
fighter_contact_resolve are CLEAN C (verified definitions in
game/menu.c + game/battle.c). The fighter-object ALLOC + populate
(the write of 0x801f5258 into guest 0x1f800110, and the .ANI/prim
bind) is in GENERATED code — not yet ported.

CONCLUSION: versus->fight reaches case 8 with fighter objects
unpopulated (empty heap objs, FC_ANI=0, no prim list). The char
resource load + fighter spawn that fills them either (a) never ran,
(b) ran async and didn't complete before case 8, or (c) ran but the
heap dest overlaps the 0x800D0000 staging arena (Session N+4) so the
data is corrupt/absent. cd_file_load_sm decomp_trace shows NO file_id
0x40 (.ANI) call in versus mode — the .ANI/char-pack load uses a
different (streaming/pack-decode) path OR isn't triggered.

NEXT: locate the generated fighter-spawn/resource-bind that writes
0x1f800110/0x1f800218 and fills the heap object (+0x10 prim list,
+0x30 FC_ANI, +0x34). Ghidra MCP backend still down (port 8080).

### Session N+5e — gwatch PROOF: fighter spawn/.ANI bind is SKIPPED

Armed guest_watch (mprotect, catches ALL write paths incl. clean
MW32/memcpy/DMA — unlike wtrace) on the four versus->fight slots and
drove tools/nav_to_fight.py to FIGHT REACHED:
  0x1f800118 (P1 ctrl FC_ANI), 0x1f800220 (P2 ctrl FC_ANI),
  0x1f800110 (P1 fighter-obj ptr), 0x1f800218 (P2 fighter-obj ptr).
gwatch covers scratchpad (GW_SCRATCH_PHYS=0x1F800000, host
SCRATCHPAD_ALLOC). Result: total=0 — ZERO writes to ANY of the four
slots during the entire versus->fight navigation.

Yet post-run the slots hold stale values: fighter-obj ptrs
0x801f5258/0x801f52a0 (set during BOOT/attract, before arming) and
FC_ANI=0. Conclusion: versus->fight does NOT re-run the fighter
spawn/.ANI->FC_ANI bind at all. The battle_flow state machine reaches
case 8 (in-fight) with fighter ctrl+heap objects left over from a
prior mode, unpopulated for the versus chars -> case 8 derefs
obj+0x10/+0x34 = 0 -> PC=0.

This REFRAMES the bug: it is NOT (only) the .ANI/staging heap overlap
(Session N+4). It is that the versus->fight path SKIPS the fighter
resource bind/spawn step entirely — a state/trigger omission. The
select->battle transition (select_screen_tick return -> case2
match_setup_apply/battle_state_init -> case3 battle_init/battle_bringup_a
-> case6 action_enter_fwd) is not invoking whatever binds .ANI->FC_ANI
and fills the 0x801f5xxx fighter objects.

select_confirm_handler (clean, menu.c:3412) only records the char id
into ctrl+3 / confirm record — it does NOT bind .ANI. The .ANI bind +
fighter spawn is elsewhere (generated, or a select-screen preview-load
path that versus isn't triggering).

NEXT: find the select-screen char-preview .ANI load + the
select->battle fighter-spawn trigger. Candidates: the cursor/confirm
resource loader in select_screen_tick's generated tail, or a
"vs_resource_load"/fighter_spawn guest fn invoked on the
select_screen_tick!=0 -> case2 transition. gwatch on the SELECT-screen
.ANI dest (the 0x8001-0x8003 decode region) during char-select would
show whether the preview .ANI loads at all in versus mode.

### Session N+6 — versus->fight wedge: op3-timeout RULED OUT; CD->event flow-control stall

Built on N+5e. The versus->fight path now spawns both fighters (the
zero-cycle clean-dispatch bug is FIXED — decode-based per-function
costs, commits d8a3445/28d628e) then wedges in the CD/SPU resource
load. Reproduced ~6x; crash is nondeterministic in WHICH kernel event
the game waits on but CONSISTENT in mechanism.

DIAGNOSTICS (all committed, freeze dump device_state):
- regwin_* counters prove cd_reg_window_op op3 NEVER returns -2
  (regwin_op3_to=0; all op3 ret=0). The "op3 status-poll timeout skips
  ch4 kick" hypothesis is DISPROVEN — the kick happens when op3 runs.
- dma_chan_hist: ch3(CDROM)/ch2(GPU)/ch6(OTC) all complete normally;
  ch4(SPU) kicks+completes a few times then stops being issued.
- EvCB decode: waited handles 0xF1000006/7 -> slots 6/7 = class
  0xF0000011 (channel/hw events, cbs 0x801A0B88/9C); 0xF1000008 ->
  slot 8 = class 0xF0000009 (CD/SPU done, cb7->cd_dma_chan_reset).
- cdrom: game byte-reads sectors via 0x1F801802; RB_.pos frozen at the
  SAME 2060/2340 every run; bfrd_set~2350 vs bfrd_clr~13 (only ~14 of
  2350 delivered sectors consumed); ring_drop~971.
- irq_counts: vb_raise 1871 vs vb_deliver 1701 (170 VBlank IRQs raised
  but not delivered = coalescing under load, likely benign); irq_deliver
  22459 = legitimate heavy CD/DMA streaming, NOT a stuck-flag storm
  (DICR 0x00900000 = master+ch4 IRQ enable, all done-flags clear).

CURRENT MODEL: the game stalls mid-sector consuming CD data because it
is blocked in TestEvent waiting for a channel/hw or CD-completion
event whose producing DeliverEvent never arrives. The CD side finished
its command (pend_cmd=0x09 Pause done, irq_fl=0, CD idle) — the missing
piece is a downstream consumer/flow-control event (SPU ch4 stream or a
0xF0000011 channel event). Which event is needed varies run-to-run =
whichever the pipeline needed next when it wedged.

NEXT: identify the producer of the waited class (0xF0000011 spec
0x100/0x2000 and 0xF0000009 spec 0x20) — trace which IRQ handler calls
DeliverEvent for it and whether that handler runs during the wedge.
Likely suspects: SPU-voice/ADPCM decode completion gating the CD feed,
or a channel event whose IRQ is masked/coalesced. Beetle oracle
(psx-beetle :4380) ground-truth on the same path is the RULE-2 check
but is not built on this host (oracle build is MSVC/DuckStation).

### Session N+7 — versus->fight TRANSITION FIXED; wedge confirmed = N+6 CD/SPU event deadlock

Two real port bugs masked the versus->fight path; both fixed+committed.

FIX 1 (fed8fdf): versus SIGTRAP `bl FUN_80194500; brk` in
battle_camera_step + vec3_step_toward_spd. Root cause = LLVM mid-end
miscompile: decompiler emits vector locals as separate `int` decls that
must be contiguous on the MIPS stack, but C gives no contiguity ->
LLVM drops "dead" Y/Z stores AND (worse) proves the else-branch
unreachable after an uninitialized/escaped-local read -> emits
`call;noreturn-brk`. Reproduced in isolation: initializing the escaped
out-array locals (local_28/24/20, always overwritten by FUN_80194630
before reads) clears the brk. FUN_80194500 proto -> variadic.
NOTE: the broader vector-local->array contiguity defect is SYSTEMATIC
and still latent in other decompiled fns (only X component computed,
Y/Z dropped) — needs a dedicated array refactor.

FIX 2 (a62b16a): versus SIGSEGV prim_list_persp_step param_1=0.
menu.c declared scratchpad ptr globals _DAT_1f800104/_DAT_1f80020c/
_DAT_1f800208 as uint8_t (truncated 32-bit guest ptr to 1 byte) and
passed them raw instead of guest_at() — battle.c does both correctly.
Fixed to uint32_t + guest_at at the two prim_list_persp_step call
sites. Same width-error class likely latent elsewhere in menu.c
(_DAT_1f800154/25c mask 0x8000000, _DAT_1f8000b4/0b6 mask 0xf00 —
impossible under uint8_t) — audit pending.

VERIFIED: nav_to_fight.py -> FIGHT REACHED, versus sustains gs=8 ph=8
(sig 2,1 = 2P VS) without crash. Arcade regression PASSED — reaches+sustains
gs=8 ph=8 @ ~98.7% CPU actively simulating.

REMAINING BLOCKER (vs arcade): versus reaches the fight STATE but the
sim STALLS — ~0.2% CPU, freeze_check current_func=0x29CC
(scheduler-idle), in_exception=1, esc_dec_nopend=8.3M (spinning on IRQs
that never come), frame_count still advancing. cdrom_state CONFIRMS
the N+6 wedge byte-for-byte: bfrd_set=1274 vs bfrd_clear=13,
sector_read_pos=2060 frozen, ring_dropped=523, pending.cmd=0x09 (Pause)
done/idle, i_stat=0x1 (VBlank only). Game buffered ~1274 CD sectors,
consumed 13, then blocked in TestEvent waiting for a channel/hw or
CD-completion DeliverEvent that never fires — versus's heavier resource
load (2 fighter records + stage assets) triggers the deadlock arcade's
lighter load avoids.

NEXT (unchanged from N+6): identify the producer of the waited event
class (0xF0000011 spec 0x100/0x2000 channel/hw, 0xF0000009 spec 0x20
CD/SPU-done). Trace which IRQ handler calls DeliverEvent and whether it
runs during the wedge. Suspects: SPU-voice/ADPCM decode completion
gating the CD feed, or a channel event whose IRQ is masked/coalesced.
The debug-server commands used in N+6 (dma_chan_hist/regwin/evcb) are
NOT in the current build — re-add as proper debug commands if needed.

### Session N+8 — REFRAME: NO mode is genuinely playable; FC_ANI=0 hollow fight

versus->fight transition crashes are FIXED (fed8fdf, a62b16a) and both
modes reach gs=8 ph=8. BUT live memory proves neither produces a real
fight — the "playable" milestone is NOT met.

LIVE EVIDENCE (arcade, gs=8 ph=8 sustained, ~98% CPU):
- battle_flow_state_machine substate DAT_1f800001=8 = live per-frame
  battle loop (fighter_contact_resolve/pair_update/camera/hud all run).
- Fighter ctrl structs at scratchpad 0x1f8000e8 (P1) / 0x1f8001f0 (P2):
  link/resource ptrs populated (+0x28 fighter-def, +0x2c opponent,
  +0x38 S5, +0x3c motion, +0x44/48 script) BUT every combat field = 0
  (flags/pos/HP) and the action-state block @+0x3c (0x801f55c0/0x801f5704)
  is ALL ZEROS.
- SMOKING GUN: FC_ANI = ctrl+0x30 = guest 0x1f800118 = 0x00000000 for
  BOTH fighters — the .ANI animation-stream pointer was NEVER bound.
  action.c action_switch computes stream=FC_ANI+off=0 -> empty action.
  fighter_state_tick reads action-id @motion+6 = 0; position +0x4e = 0.
- Selectors DAT_1f800099 (char/anim id)=0, DAT_1f80009a (action)=0,
  DAT_1f80009b..9f=0. fighter_anim_table_init(0)/action_enter_fwd(...,0)
  ran with NO valid character data.
- Input (attack/direction) produces NO fighter-state change.

ROOT CAUSE (matches N+5e/N+5d, now confirmed live in BOTH modes):
the select->battle .ANI->FC_ANI bind is SKIPPED. per_side_setup(3,0)
(menu.c:4603) -> record_table_select + mode-dispatch call_guest(fn,
s0=0x801f04a8, 0x1f800000) via table 0x801b1774[idx=(s8)B[0x98]*4].
The bind that writes 0x1f800118 (P1) / 0x1f800220 (P2) never fires or
writes 0. Without FC_ANI the fighters are allocated+linked shells with
no animation/action/combat data -> battle_intro_fsm waits forever ->
hollow fight at high CPU, no interactivity.

versus ADDITIONALLY deadlocks earlier on the N+6 CD/SPU channel-event
stall during its heavier resource load (bfrd_set=1274 vs clr=13).

NEXT: trace the .ANI->FC_ANI write. Candidates: the call_guest dispatch
in per_side_setup, record_table_select, fighter_rec_fetch_apply, or the
character-record .ANI loader. gwatch on 0x1f800118/0x1f800220 during
arcade->fight will show whether ANY write fires and from which PC.
The uncommitted game/action.c reset() work (ctrl+0x14 player-block +
battle-mode adj) is related in-progress fighter-init fix — preserve it.

### Session N+9 — SHARPENED: fighter resource BUFFERS all-zero; char data never loaded

gwatch on the WHOLE P1 ctrl block 0x1f8000e8 (len 0x100) during a fresh
arcade->fight nav: only 2 writes total, both to fighter+1 @0x8019A5A4
during the fight. The populated ctrl fields (+0x28/+0x38/+0x3c/+0x44/48)
were written during BOOT/ATTRACT, before arming — the select->battle
transition does NOT re-run fighter spawn/.ANI bind (confirms N+5e for
arcade too, not just versus).

FOLLOW-ON: every fighter resource pointer targets ZEROED memory —
ctrl+0x44/0x48 (0x801e01ec/08ac P1, 0x801e054c/0c0c P2) script/matrix,
+0x3c motion-state (0x801f55c0/5704), +0x38 S5 (0x801f5328/5440) all
read 0/32 nonzero. Buffers are ALLOCATED but the character .ANI/.ACT/
script payload was NEVER loaded. menu_res_load_sm (save.c:4105,
guest 0x801709fc) is the async loader driving s1+0x8c through
0->0x10->0x20->0x40->0x50->0xf00 via cd_file_load_sm /
save_channel_init_commit / save_load_state_machine /
save_state_machine_alt; it reached case8 (returned 1) yet left all
fighter buffers empty -> the CD-read/decode/.ANI-bind produced nothing.

NOTE: char ids ARE set (DAT_1f8000eb/1f3 = fighter+3, RNG via
table_byte_next_a; P1=0 P2=5) and DAT_1f800099 is a config value
(config[4]*2-2), NOT the char id — earlier "no char selected" read was
wrong. The bug is the resource LOAD yielding empty data, not selection.

NEXT: determine whether menu_res_load_sm's save_* states actually
issued CD reads for the char files (vs returned success early /
read wrong LBA / decode to wrong dest). Check cdrom_command_history +
cd_file_load_sm/save_*_state_machine impls for the char .ANI/.ACT
file table. versus hits the N+6 CD/SPU deadlock during this same load;
arcade "completes" it but empty — possibly the same root (CD data not
delivered/decoded) manifesting differently.

### Session N+10 — CORRECTION + ROOT: unified blocker is CD-DMA ch3 stall, not fighter bind

REVISED DIAGNOSIS (supersedes N+9's "bind skipped" hypothesis):

- Fighters ARE bound when given attract/load time: live read of
  0x1f8000e8/0x1f8001f0 showed FC_ANI=0x800722c4/0x800d47dc, nparts=22/19,
  char=4/0, spawn pos +-1024, action-state blocks populated. N+9's all-zero
  buffers were a PREMATURE-SAMPLE artifact (async bind not finished).
- BUT the fight still never simulates: fighter ctrl + motion + matrix +
  action-state buffers are byte-for-byte FROZEN over seconds (0 diffs),
  screen ~99.7% black (673/243712 nonblack px), input produces no change.

ROOT CAUSE (unified across arcade AND versus):
- freeze_check: current_func=0x29CC (main thread busy-spin, NOT kernel
  sleep), frame_count advances ~278/s via VBlank IRQ independently.
- irqctx_ring: main thread interrupted at guest pc~0x8019A648 EVERY VBlank —
  a BUSY-WAIT inside the BIOS CD/vsync region (vsync_wait_n 0x8019a5d8 /
  cd_timer_read 0x8019a460 / cd_dispatch 0x8019a680), not a DeliverEvent sleep.
- g_pbd_calls=0 across 126k frames: post_battle_dispatch (the real fight
  driver, menu.c:6587, called from battle_flow_state_machine caseD_8 /
  substate 8) NEVER runs -> the main thread never reaches the battle dispatch.
- g_game_state=DAT_1f800000=8, DAT_1f800001=8 -> routes to
  battle_flow_state_machine, whose caseD_8 calls post_battle_dispatch +
  fighter_contact_resolve->fighter_state_tick. None of it executes.

CD-ROM DEADLOCK (the actual wedge):
- cdrom_state: pending.cmd=0x09(Pause) active=0, reading=0,
  sector_read_pos=2060/2340 FROZEN, bfrd_set=12787 vs bfrd_clear=16
  (~800x imbalance, still growing), ring_dropped=5808, i_stat=0.
- dma_state: ALL channels inactive, dma_depth=0. CD data moves on DMA ch3
  (cdrom_async, dma.c:1000 dma_cdrom_transfer_active / dma_advance:1129).
- dma_cdrom_history: ch3 previously moved sector words fine (512-word chunks,
  sector_read_pos 12->2060, lba 3774, completed=1). LAST state: ch3
  chcr=0x00000000 active=0 remaining=0 madr=0x1D47C0 bcr=0x10200 — the DMA
  finished its armed 512-word chunk MID-SECTOR (pos 2060 of 2340) and was
  NEVER re-armed for the remaining ~280B + next sectors.
- cdrom_dma_ready() returns TRUE (BFRD set, rb_available, pos+4<=size) but
  no DMA is active to pull cdrom_dma_read() -> RB_.pos frozen at 2060 ->
  the game's CD read never signals complete -> main thread polls CD status
  at 0x8019A648 forever.

WHY IT MANIFESTS AS "FIGHT REACHED BUT EMPTY": char .ANI/.ACT loads far
enough to spawn fighters (FC_ANI bound), then a LATER streaming/stage CD
read stalls mid-sector -> main thread blocks in the CD poll -> the battle
substate machine (post_battle_dispatch -> battle_frame_tick ->
fighter_state_tick) never runs -> fighters frozen, black screen. versus
hits the same wall during its heavier resource load (N+6).

NEXT: find why CD-DMA ch3 does not re-arm after completing a 512-word chunk
mid-sector. Candidates: (a) DMA-completion IRQ (ch3 / DICR bit3) not
delivered so the game's CD-read loop never re-kicks ch3; (b) the game's
re-arm waits on a CD "data ready"/sector-done flag (INT1/STAT) that the
model sets at the wrong granularity (per-sector vs per-chunk); (c) BCR word
count 512 vs sector 2340B(585w) mismatch — the model or the game's chunking
expects different completion. Inspect the CD-DMA complete path
(dma.c cancel_async_transfer / delayed_complete / DICR ch3) and the game's
CD-read loop at func 0x2970/pc 0x801978AC (the ch3 kick site) + the poll at
0x8019A648. Compare against real HW: does ch3 auto-reload BCR or must the
driver re-write CHCR per chunk?

---

## Session N+11 — CD buffered-read stall REFINED: read ends idle, ring drops sectors, file-load never completes

Corrects/refines the N+10 "ch3 stalls mid-sector" framing with live state.

MAIN THREAD IS NOT HARD-BLOCKED IN vsync_wait_n:
- pc samples land at 0x8019A648 but that is cd_timer_read (0x8019a460)'s
  per-frame pacing primitive — it calls vsync_wait_n (0x8019a5d8) twice:
  once "wait until DAT_801bdde0>=target" and once "wait 1 frame"
  (a0=counter+1,a1=1). DAT_801bdde0 advances ~292/s (per-IRQ-batch, not
  clean 60Hz). vsync_wait_n has a bounded a1<<15 delay budget + a timeout
  path (block_8019A620 -> 3 calls -> return). It RETURNS; the main loop
  iterates (frame_count ~294k).

REAL BLOCK: battle_flow_state_machine is NEVER CALLED.
- decomp_trace_record(60,...) at menu.c:563 fires every frame substate!=0,1
  and tag 60 maps to "?" in decomp_tag_name. decomp ring total FROZEN at
  8102 -> NO records of any tag -> battle_flow_state_machine not running.
- g_game_state=DAT_1f800000=8 -> game.c case 8 -> match_mode_set(1) +
  battle_flow_state_machine(auStack_30). It should reach caseD_8 ->
  post_battle_dispatch (DAT_1f80009b=0 != 4). pbd_calls=0 confirms none of
  it runs. The main thread is stuck in a NESTED file-load/CD wait from an
  earlier state, never returning to the top-of-loop dispatch.

CD STATE — read ENDED, did not stall mid-transfer:
- pending.cmd=0x09(Pause) active/pending=0 -> Pause COMPLETED.
  reading=0, irq_flag=0, i_stat=0, all DMA channels inactive, dma_depth=0.
- last_sector lba=8801 size=2340 frame=10524 — coincides with last cmd.
  No sector/command/DMA activity for ~285k frames since.
- cd_intc_dispatch (0x8019a8ec, drives cd_read_step_cb 0x801985c4 /
  cd_read_finish_cb 0x80198838) entry counter @0x801bce00 reads 0 — but
  that only proves the CLEAN thunk wasn't hit; generated IRQ path may
  bypass it. Inconclusive alone.

SMOKING GUN — buffered-read accounting broken:
- ring_dropped=5808 (sector ring overflowed, data LOST)
- bfrd_set=12787 vs bfrd_clear=16 (~800x imbalance)
- sector_available=1 (one sector sits unconsumed in RB_)
- response_count=1 response_read=1 (one response consumed)
- file_load caller = func 0x80167330 invoked from main-loop helper
  0x801265E4 (ret 0x801265F4), which then checks a state byte==6.

HYPOTHESIS: the game issues buffered CD reads; the model's sector ring
overflows (game DMA-consumes slower than drive produces) and DROPS sectors.
The read-completion accounting expects the full requested sector count —
dropped sectors mean it never reaches "done", so cd_read_finish_cb /
the file-load done-flag never fires -> file_load waits forever -> main
thread never returns to dispatch -> gs=8 ph=8 hollow. On real HW the drive
flow-controls (no drop) or the game drains fast enough; the model must
backpressure rather than drop, OR the completion must count delivered-not-
dropped. NEXT: inspect rb_available()/ring push vs the game's drain rate,
whether s_ring_write-s_ring_read overflow should stall sector production
instead of dropping, and whether ReadN completion counts requested vs
delivered sectors.

## Session N+12 — ROOT ISOLATED: CD-interrupt deferred coroutine never completes the async op; flag 0x801BDE40 stays 0

Full call-chain + flag decode; supersedes the "ring overflow is the cause"
framing — the drops are a SYMPTOM, the wedge is the async-op completion path.

EXACT BLOCKING CALL CHAIN (stack unwind, sp=0x801fff78):
  main 0x80126220 -> game.c case8 region 0x80126664 -> cd_file_read_seq
  0x801565ec -> cd_cmd_queue_load 0x80192634 -> cd_timer_read 0x8019a460 ->
  vsync_wait_n 0x8019a5d0 (hot poll pc 0x8019a648).
cd_file_read_seq's tail loop (gen full_19.c block_80156628/30):
    do { v0 = func_0x8019D500(0); } while (v0 == 0);
func_0x8019D500 reads the async-CD-op result slot at 0x801BDE40 (and code at
0x801BDECC / frame ctr 0x801bdde0 nearby) and returns nonzero only when the
slot shows done. func_0x8019D4D0 = cd_mode_store WRITES that slot — invoked
from the game's CD response handler func_0x801582C0, which is reached ONLY
through the CD deferred-interrupt coroutine:
    CD IRQ -> kernel -> cd_ctx_restore -> cd_chan_full_init(0x8019a814)
      -> cd_intc_dispatch(0x8019a8ec) -> cd_cb8_dispatch
      -> game CD handler 0x801582C0 -> cd_mode_store 0x8019D4D0 -> flag.

LIVE STATE PROVES THE COROUTINE NEVER COMPLETES THE OP:
- read_ram 0x801BDE40 = 0, 0x801BDECC = 0 -> async op never reported done.
- Event ring (65536 entries): ZERO ISTAT_RAISE/IRQ_DELIVER with i_stat bit2
  (CDROM=0x4). ONLY VBlank bit0 fires. cd_dataready_fires=7114 and
  cd_irq_delivered=7915 FROZEN — CD IRQs worked during load then stopped.
- irq_flag=0, pending.cmd=0x09 Pause pending=0 (INT2 fired+acked),
  reading=0, stat=0x02(MOTOR), i_mask=0xD (CDROM bit2 enabled), irq_enable=0x07.
- Command history tail: per-file Pause->SetLoc->ReadN->stream->Pause. Last
  exec seq613 frame10524 = Pause, i_stat was 0x4 at issue, now fully consumed.

CONCLUSION: every CD interrupt was delivered+acked, but the deferred
coroutine did NOT run cd_mode_store for the awaited op -> flag stays 0 ->
cd_file_read_seq spins forever -> main thread never reaches
battle_flow_state_machine -> pbd_calls=0 -> gs=8 ph=8 hollow black screen.
The coroutine resume (cd_ctx_restore re-entering cd_chan_full_init) is the
break — NOT a missing i_stat raise (irq_flag legitimately 0 after ack) and
NOT the ring drop itself.

SECONDARY (faithful-model gap, Beetle cross-ref):
- cdrom.cpp ProcessDataSector keeps 8 sector_buffers with INDEPENDENT
  current_read/write_sector_buffer; guest drains ring[read] then read++.
- Ours sets s_ring_read = s_ring_write on EVERY delivery
  (deliver_read_sector L1935, _without_irq L1968, process_pending L2597)
  and NEVER advances s_ring_read on drain-complete (cdrom_dma_read only
  bumps RB_.pos). So RB_ collapses to newest sector; there is no multi-
  sector drain queue -> any drain/delivery overlap corrupts and any >1
  sector lag drops. This is why ring_dropped=5808: the game's DMA-arm is
  gated on the SAME broken coroutine/INT1 path, so it can't keep up.

NEXT (faithful, class-level):
1. Verify whether cd_ctx_restore/cd_chan_full_init coroutine actually
   resumes on CD IRQ — instrument the kernel IRQ->coroutine dispatch, or
   compare cd_cb8_dispatch/cd_intc_dispatch execution vs Beetle's CD IRQ
   handler reaching the game's response routine. The break is between
   "i_stat bit2 IRQ taken+acked" and "game CD handler runs".
2. Give s_ring_read proper drain-advance semantics ((read+1)%8 when
   RB_.pos>=RB_.size) matching Beetle's current_read_sector_buffer, so
   queued sectors drain in order instead of collapsing to newest.
3. Re-run arcade+versus; confirm pbd_calls>0, fighter_state_tick runs,
   screen renders.

=== SESSION N+13 — coroutine DOES run every frame; break is NULL work-ptr ===
fntrace_arm on the IRQ/coroutine path gave GROUND TRUTH (corrects N+12):
- cd_intc_dispatch(0x8019A8EC, a0=ctx 0x801BCD50) AND
  cd_cb8_dispatch(0x8019AE68, a0=1,a1=1,a3=0x4000) fire ONCE PER FRAME
  (~560 entries). The deferred coroutine RESUMES every VBlank — the
  "coroutine never resumes" hypothesis is DISPROVEN.
- cd_cb8_dispatch body: bumps DAT_801bdde0 (frame counter), then reads a
  function pointer at 0x801BDDC0 and jalr's it ONLY if non-NULL. Live
  read_ram 0x801BDDC0 = 0 -> the per-tick WORK callback is NULL, so the
  dispatch is a no-op besides the frame counter. func_0x801582C0
  (cd_mode_store caller) and cd_mode_store(0x8019D4D0) NEVER appear in
  the trace -> they are not reached on the VBlank path.
- CORRECTION: func_0x801582C0 is chan_reset_register (fnmap.c:1582,
  menu.c:9960) — CD channel INIT/reset that calls cd_mode_store(0). It
  is NOT the per-IRQ handler. cd_mode_store has only 2 callers, both
  reset paths. STATE(0x801BDE40)=1 is written by the async state machine
  func_0x8019D5F0 (892B/75blk), pumped synchronously by func_0x8019D500
  each poll — NOT by an IRQ callback.
- Dispatch table live: 0x801BCD18=1(flag->dispatch path),
  0x801BCD1C=0x8019ae68(cd_cb8_dispatch), 0x801BCD24=0x801979f8,
  0x801BCD28=0x8019af7c, 0x801BCD48=0xd. Coroutine ctx 0x801BCD50 holds
  resume PC 0x8019a87c (armed, valid).
- CD state at wedge: reading=0 stat=0x02(MOTOR) pending=0 irq_flag=0
  response_count=response_read=1 sector_available=1(leftover @pos2060/2340)
  ring_dropped=5808. Last CD cmd seq613=Pause INT2 fired+acked frame10524.
- DMA history: per-sector the game arms 3-word header DMA + 512-word data
  DMA (sector_pos 0->12->2060 of 2340); each COMPLETES. ch3 works; the
  game simply stops re-arming once the read is aborted.

CONCLUSION (refined): the coroutine+IRQ delivery is HEALTHY. The break is
that the async op func_0x8019D500->cd_seek_state_machine(0x8019D5F0) never
observes the completion condition because the awaited CD response/sector
stream was truncated by the game-issued Pause after 5808 ring drops. On
real HW the buffered-read drive flow-controls (pauses production when the
sector buffer is full) so NO sectors are lost; ours DROPS on ring-full in
authentic mode -> guest sector accounting short -> state machine never
sees "read done" -> STATE stays 0 -> infinite pump -> battle_flow never
runs. NEXT: implement buffer-full backpressure for ReadN/ReadS sector
production (gate deliver on ring space, resume on drain) matching real
CD flow control; verify against Beetle sector-buffer-full semantics, then
re-run arcade+versus for pbd_calls>0.

=== SESSION N+14 — CORRECTIONS: async poll returns done; block is GPU/OTC DMA queue loop on already-loaded data; backpressure hypothesis REFUTED ===

Disassembled the exact wait chain + cross-checked Beetle source. Three
corrections to N+12/N+13:

1. func_0x8019D500 (the cd_file_read_seq tail poll) is NOT the block.
   Decoded full_47.c: it reads STATE=load(0x801BDE40); if ==1 -> return 1.
   Else reads SELECTOR=load(0x801BDEE4); if ==1 -> return 1. Only when
   SELECTOR!=1 does it pump the state machine via op_cb(0x801BDE38=
   0xf1000008 -> trampoline 0x8019D5B0 -> func_0x8019D5F0). Live
   0x801BDEE4=1 -> func_0x8019D500 returns 1 (done). cd_file_read_seq's
   `while(func_0x8019D500(0)==0)` EXITS. (NOTE: selector addr is
   0x801BDEE4, not 0x801BDEEC — earlier decode had an off-by-8.)

2. The live block is cd_cmd_queue_load (0x80192634, 224B/7blk) running
   REPEATEDLY as an INDIRECT callback inside the data-delivery path —
   NOT a single stuck call and NOT a CD-completion wait. Decoded body:
   it is a DMA-channel loader for ch6 (GPU/OTC): stores data ptr into
   ch6 MADR(0x1F8010E0 via cell 0x801B8438), count into BCR(0x1F8010E4
   via 0x801B843C), kicks CHCR(0x1F8010E8 via 0x801B8440) with
   0x11000002, arms timeout(0x801936B0), then loops
   `while(ch6_CHCR & 0x01000000) { cd_pump_watchdog(0x801936E4); recheck }`.
   Live ch6 CHCR=0x00000002 -> busy bit24 CLEAR -> each call COMPLETES.
   madr OSCILLATES 0x801E82DC<->0x801E618C across samples -> the caller
   iterates a double-buffered queue. CD is 100% IDLE (seq frozen 244091,
   reading=0, ring_dropped frozen 5808, no new cmds for ~285k frames) ->
   the loop processes ALREADY-LOADED RAM, not waiting on CD data.

3. N+13's "implement buffer-full backpressure" is REFUTED by the oracle.
   Beetle cdrom.cpp ProcessDataSector (L3457-61) DROPS on buffer-full
   ("previous sector dropped") and DeliverAsyncInterrupt (L1444-45) sets
   current_read_sector_buffer=current_write_sector_buffer on each INT1 —
   read snaps to write IDENTICALLY to ours. Our ring/drop model is
   FAITHFUL; adding backpressure would DIVERGE from the oracle. Sector
   size also verified: ours 2340 == Beetle RAW_SECTOR_OUTPUT_SIZE
   (2352-12); game intentionally reads 2060 leaving 280 (comment L2690).

CONCLUSION (N+14): every CD mechanic checked is faithful. The wedge is a
GPU/OTC DMA command-queue processing loop (cd_cmd_queue_load invoked as a
callback) that iterates already-loaded data and never reaches its exit
condition — a GPU/DMA-completion or queue-state issue, NOT a CD read
completion issue. NEXT: find cd_cmd_queue_load's caller/callback
registration (it is invoked indirectly — no direct jal; likely a
function-pointer in the CD/GPU queue ctx) and the loop's exit condition;
check whether it waits on a GPU fence/OTAG-drain or a queue-slot count
that our GPU/DMA model never signals. pbd_calls still 0; gs=8 ph=8 hollow.

=== SESSION N+15 — wedge localized to TRUNCATED file upload: GPU/OTC loop ping-pongs exactly 2 buffers ===

fntrace_arm(0x80192634) captured the real caller + args:
- ra=0x80126664 -> game.c case8 dispatch region; the loader is
  func_8016FCC4 (3384B/218blk, called at 0x80126660 with a0=sp+16).
- a0 ALTERNATES only 0x801E4110 <-> 0x801E6260 (two ~8.3KB staging
  buffers, 0x2150 apart); a1=0x820 (2080-word chunks); a2=0.
- Over a multi-second window the DISTINCT a0 set is EXACTLY those two —
  the upload pointer NEVER advances to a third buffer. It is a hard
  ping-pong livelock re-uploading the same two chunks, not slow progress.
- ch6 DMA completes every call (CHCR busy bit24 clear); CD 100% idle
  (seq frozen 244091, reading=0, ring_dropped frozen 5808).

INTERPRETATION: the file-load produced only ~2 buffers of valid data
(~16KB) then stopped advancing — consistent with the CD read having
dropped 5808 sectors so the loader's byte/sector accounting never reached
the file's real size. The GPU/OTC upload loop (func_8016FCC4) iterates
the staging ring waiting for the next chunk that never arrives ->
infinite ping-pong -> battle_flow_state_machine never reached.

Sector timing verified faithful: sector_delay_cycles()=451584 (1x) or
/2 (mode bit7=2x -> 225792 cyc = authentic 150 sectors/s @33.87MHz);
sector_size=2340 == Beetle RAW_SECTOR_OUTPUT_SIZE; game reads 2060
leaving 280 intentionally. Ring read=write snap == Beetle
DeliverAsyncInterrupt. ALL CD mechanics faithful.

OPEN ROOT CAUSE: WHY did this specific (large) file read drop so many
sectors that only 2 buffers loaded? Two live hypotheses:
 (a) TIMING: guest CD-drain (per-sector INT1->3w hdr +512w data DMA)
     falls >8 sectors behind during this heavier battle-resource load.
     If our cycle accounting makes the drain loop slower than real HW,
     drops -> truncation. Compare guest cycles/instr vs Beetle at the
     same load; check apply_read_speed/divisor is 1 (authentic) here.
 (b) A specific sector/LBA region this file crosses that our ISO reader
     or sector-delivery mishandles (bad LBA -> zero/garbage sector ->
     game aborts early). Check last_sector_lba range vs file extent.
NEXT: dump the op struct / file-load state func_8016FCC4 uses (the
staging-ring write cursor + expected total) to get the EXACT
expected-vs-delivered sector count; then determine if the shortfall is
drops (timing) or a bad region (data).

=== SESSION N+16 — MAJOR CORRECTION: battle_flow IS running the fight loop; screen is black because render data is absent ===

N+15's identification of func_8016FCC4 as a "file-upload loader" was WRONG.
fnmap.c:1740 maps 0x8016FCC4 -> battle_flow_state_machine. The two
ping-pong buffers 0x801E4110/0x801E6260 (stride 0x2150, 2080-word chunks)
are the NORMAL double-buffered OTAG region, NOT a truncated file upload:
- menu.c computes _DAT_1f800008 = _DAT_1f80000c*0x2150 + 0x801e4110;
  live _DAT_1f80000c cycles 0->1 (buffer index advancing per frame).
- game.c:1244 calls gpu_clear_otag_r(0x801e4110,0x820) — it is an
  ordering-table clear region, contents are pointer/index tables
  (0x1e4110,0x1e4114,...), not file bytes.
- cd_cmd_queue_load (0x80192634) is a GPU/OTC DMA queue loader on ch6;
  it completes every call (CHCR busy bit24 clear). The "livelock" is just
  per-frame OTAG maintenance.

LIVE STATE (scratchpad 0x1f800000): gs=8, DAT_1f800001(phase)=8,
_DAT_1f800082(sim gate)=0 (OPEN), DAT_1f80009b=0 -> caseD_8 calls
post_battle_dispatch (NOT battle_sides_commit), DAT_1f80009e/9d/9a=0,
DAT_1f800098=3, DAT_1f800099=0. _DAT_1f80000c advanced 0->1 between
reads -> per-frame iteration is LIVE.

caseD_8 (menu.c:737) is the REAL battle loop: every frame it runs
post_battle_dispatch, then (gate _DAT_1f800082==0) record_state_stepper,
fighter_contact_resolve, FUN_801787dc x2, fighter_pair_update, then
battle_hud_draw, battle_camera_step, camera_state_cache, fighter_flash,
player_light. ALL prior docs claiming "battle_flow never runs /
pbd_calls=0 / stuck in loader" were WRONG — the game reaches and SUSTAINS
the battle simulation.

FIGHTER STATE: control structs 0x1f8000e8(P1)/0x1f8001f0(P2) are
populated with valid pointers (+0x14 player_block, +0x38 motion, +0x3c
action-state). Motion/action sub-structs (0x801f5328/0x801f55c0 etc.)
contain real data pointers into the loaded game region — NOT empty.
CD is 100% idle (seq frozen, reading=0). Frame counter ~270/s.

DECISIVE VISUAL: screenshot_file -> /tmp/br2_frame.png is ~99.8% BLACK
(15202/15232 sampled px = 0x000000; a few dark-red/green specks = tiny
HUD artifact). The battle sim runs every frame but renders NO scene —
stage/fighter geometry is not reaching the screen.

NOTE on fntrace: arming the clean fn addresses (post_battle_dispatch
0x80154930, fighter_pair_update 0x801809c8, etc.) captured NOTHING —
clean game/*.c functions are invoked by NAME (C call), not via their
0x8xxxxxxx generated entry, so entry-instrumentation does not see them.
This is an observability gap, not proof they don't run (the decompiled
call path + open gates prove they do).

NEW ROOT CAUSE (supersedes N+15): the game is IN the fight loop but the
stage/fighter RENDER resources are absent/empty -> black screen. This is
consistent with the earlier CD file-load dropping 5808 sectors: the
resource files (stage model/texture, fighter .ANI/model) were truncated,
so the draw lists are empty even though sim structs are bound. The wedge
is NO LONGER "battle_flow stuck" — it is "file-load delivered truncated
render data".

OPEN: (a) confirm WHICH resource file truncated and whether its buffer is
zeroed vs partial; (b) the real root cause of the 5808 sector drops —
guest CD-drain falling >8 sectors behind during the heavy battle-resource
load (timing) vs a bad LBA region; (c) whether black is purely missing
geometry or also a broken draw-submission path. NEXT: identify the
truncated file's destination buffer + expected size, and measure the
CD-drain cycle budget vs Beetle during that specific load.

=== SESSION N+17 — architecture resolved: battle loop runs GENERATED code; clean port not live for battle ===

CORRECTION to N+16's framing. The clean game/*.c functions run as HOST C
calls — they never set a guest PC, so fntrace/decomp_trace CANNOT see them
(guest-PC probes are blind to direct C calls). The REAL signals:
- pbd_calls=0  -> clean post_battle_dispatch (menu.c:6587, has g_pbd_*
  counters) is NEVER invoked.
- decomp_trace empty -> clean battle_flow_state_machine's entry
  decomp_trace_record(60) never fires.
- bce_calls=3.15M -> clean battle_config_eval (menu.c:3867) DOES run,
  but only via the MENU/SELECT/spawn callers (menu.c:3322/3638/4664/7943),
  NOT via post_battle_dispatch.
- cd_cmd_queue_load(0x80192634) fires via fntrace = GENERATED code path.

CONCLUSION: menu/select screens execute CLEAN code, but once g_game_state
hits 8 the BATTLE loop executes GENERATED SLES code (func_8016FCC4 etc.).
The clean_table DOES contain wrap_8016fcc4/wrap_80154930, but the live
battle path does not route through them — the generated main/battle loop
drives the fight. So the "2 source trees" issue is REAL and load-bearing:
the clean port is a parallel reference, NOT yet the execution path for
combat. All game-logic analysis of game/menu.c battle_flow is therefore
about code that is NOT running.

OBSERVABLE STATE (unchanged, generated-path): gs=8, phase byte
DAT_1f800001=8, all battle sub-state bytes (0x90-0x9e,0x92) frozen at 0,
fighter control blocks bound + descriptor tables populated, BUT fighter
animation FSM gated by status flag 0x80 -> pose 100% frozen (0/96B over
1s), framebuffer ~99.8% black + tiny centered splash, ~1 polygon/frame
(gte nproj=1). intro_fsm stage=3 (setup done). NOT input-gated (START
no-op). CD 100% idle.

ROOT-CAUSE CHAIN (best current model): the earlier large CD resource
read dropped 5808 sectors (guest fell >8 sectors behind the sector ring)
-> battle/fighter resource files (.ACT/.ANI action scripts, stage/fighter
models) delivered TRUNCATED -> fighters bound but have no valid action
script -> flag 0x80 stays set -> animation FSM never runs -> frozen
fighters + empty draw lists -> black pre-fight screen that never reaches
combat.

STRATEGIC PIVOT: stop tracing game logic (clean port isn't live for
battle anyway). The blocker is the RUNTIME/hardware sim — specifically WHY
the guest CD-drain falls >8 sectors behind during the heavy battle-
resource load when real HW/Beetle keeps up. That is the faithful-timing
core (P1-P6): per-instruction cycle cost, IRQ delivery latency, DMA
completion timing, CD sector-production vs guest-drain budget. Fixing the
timing so the resource load completes untruncated is the faithful,
class-level fix — it helps EVERY game, not a per-game patch.

NEXT CONCRETE STEP: measure the CD-drain cycle budget vs Beetle during
the specific battle-resource ReadN — count guest cycles/instructions per
sector-drain (INT1 -> 3w hdr DMA + 512w data DMA) vs the 225792-cycle
(2x) / 451584-cycle (1x) sector period; find whether our per-DMA or
per-instruction cost inflates the drain past the budget. Cross-check the
INT1->handler->DMA-arm latency against Beetle's CD interrupt path.

=== SESSION N+18 — CORRECTION: not in battle at all; stuck in VERSUS SELECT, config uninitialized ===

N+17's "battle loop runs generated code / CD-truncation" pivot was WRONG.
This session proved the game never reaches the fight: it is wedged in the
VERSUS character-select/confirm state machine. gs=8 + DAT_1f800001=8 are
the select screen's reuse of the shared scratchpad phase bytes, NOT a live
battle (battle_flow_state_machine / post_battle_dispatch are NOT running).

DECISIVE COUNTERS (freeze_check, sampled):
- pbd_calls=0, bce_fz0 frozen at 132  -> clean post_battle_dispatch (param_3=0)
  NEVER runs; the whole battle dispatcher is absent.
- bce_fzN climbs ~2/frame (548/274fr), bce_p3=1 -> ALL battle_config_eval
  calls come from the SELECT path with param_3=1, i.e. menu.c:8251-8252.

EXECUTING PATH (identified): vs_confirm_e (menu.c:7682) switch on
*(s0+0x24) is in case 8 (menu.c:8250). L_case7 sets DAT_1f800001=8 then
falls into L_case8, which calls battle_config_eval(playerA/B,0x1f800000,1)
x2 per frame polling both players' select phase (pl+0xb4/b6) for 0x700
(ready). This is the versus confirm FSM, not battle.

WHY PLAYERS NEVER BECOME READY — bcfg_step_a (menu.c:4066) returns 0:
  bcfg_step_a = battle_state_gate(battle) && (player_flags&0x800) &&
                ret_zero_b(player)>=0.
Live scratchpad shows the versus select config was NEVER populated:
  - *(u32*)0x1f8000c0 (active-mode config/char-record ptr) = 0x00000000 NULL
    -> battle_state_gate's "s16(0xa4) < u8(cr+0x10)" reads null -> returns 0.
  - u16 player flags @0x1f800068 (P1) and @0x1f80006a (P2) = 0x0000
    -> (flags & 0x800)==0 -> bcfg_step_a returns 0 for BOTH players.
  - player phases: P1 0xb4=0x0300 (cls 0x300, mid-select), P2 0xb6=0x0000.
  - global select phase u16@0x92 = 0x0100 (gcls 0x100, valid).

INPUT IS NOT THE BLOCKER: pad data reaches the game — 0x1f80007e/0x80
update with debug `press` (verified 0x1234->0xedc9). But NO button on
either slot changes phases/flags/0xc0: the select FSM has no populated
records to confirm into, so it cannot advance. versus_state_init
(menu.c:10320) copies the versus-screen backdrop config but does NOT set
0x1f8000c0 or the +0x68 player flags — those come from the versus
CHARACTER-SELECT resource/config load that apparently never ran/completed.

SCREEN: ~99% black + a teal (8,168,168) UI panel on the right = the versus
select backdrop (P2 side), consistent with an un-populated select screen.

ROOT-CAUSE (best model): the versus character-select entry did not finish
initializing the per-player select records (0x1f8000c0 config ptr +
+0x68 confirm flags). Either the versus-select resource load (CD) was
skipped/truncated, or the mode->versus transition took a path that bypassed
the select-config init. NOT a timing/IRQ issue this time — timers, CD,
memcard, SIO all verified live; vsync_wait_n (0x8019a5d8) is the normal
per-frame limiter working correctly (DAT_801bdde0 VBlank counter advances
~280/s, timeout counter resets each call = completes+recalls).

NEXT CONCRETE STEP: find the versus/character-select entry that is
supposed to allocate the select config and store its pointer to
0x1f8000c0 + set player +0x68 flags (likely a generated function reached
from the mode->versus transition, or a cd_file_load_sm completion for the
select resources). Trace the versus entry from mode_select_commit /
select_screen_tick / select_confirm_handler and identify which init call
is missing or whose resource load did not complete. THEN the select can
populate records -> players confirm -> 0x700 -> versus_state_init case9 ->
battle_struct_init/match_setup_apply/battle_state_init -> real fight.

=== SESSION N+19 — 0x1f8000c0 ROOT CAUSE FOUND+FIXED (ed72abb); versus->fight hang is NEXT ===

ROOT CAUSE (confirmed, faithful clean-port omission — NOT generated, NOT CD):
game_globals_reset (game.c:1115, guest 0x80127574, registered clean in
fnmap) initializes the battle-block pointer cells. The GENERATED code at
0x80127604 does `sw $v0(=0x801C1200), 0xc0($s0)` where s0=0x1f800000 —
i.e. it stores the SYSTEM CONFIG guest address 0x801C1200 into scratchpad
0x1f8000c0, alongside +0xbc=0x801F0270, +0xb8=0, +0xcc=0x801C15F8.
The clean port set _g_system_config_ptr=g_DAT_801c1200 and the 0xbc/0xb8/
0xcc cells but OMITTED the 0x1f8000c0 store -> cell stayed NULL forever.

FIX (game.c:1144): *(uint32_t*)guest_at(0x1f8000c0)=guest_of(g_DAT_801c1200);
placed in generated order (0xc0 before 0xbc). guest_of() is the same
convention as _DAT_1f8000c4=guest_of(_g_system_config_ptr+0x2c) (menu.c:593).
NOTE: 0x1f8000c0 is the SYSTEM CONFIG ptr (0x801C1200), not a separate
versus roster — select_confirm_handler/per_side_setup/battle_state_gate/
battle_config_eval/post_confirm_config_build all index it as
cr[player+0x1b]=chosen char, cr[0x10]=count, cr[4]=players, etc.

VERIFIED LIVE (fresh boot + nav_to_fight.py):
  - 0x1f8000c0 = 0x801C1200 (was NULL)
  - both fighter byte0 = 0x01 ACTIVE (was 0x81 inactive)
  - P2 phase 0xb6 advanced 0x000 -> 0x300 (was stuck 0); P1 0x300
  - versus-select stall CLEARED; progressed into versus->fight transition

NEXT BLOCKER (new, separate): versus->fight transition now HARD-HANGS the
emulator — psx_cycle_count, DAT_801bdde0 (VBlank), frame_count all frozen;
in_exception=1; cop0_epc=0x8019A600 (vsync_wait_n), cause=0x400 (IRQ);
cdrom_state reading=0 (CD idle, NOT a pending read). irqctx_ring shows the
CPU was executing CD/kernel wait code (0x8019A600/0x8019D540/0x80197030)
under repeated VBlank IRQs then parked in exception delivery. nestgate_
rfepend=4950. This is the versus->fight handoff bug family (kernel/IRQ
deadlock during fight-init resource bring-up), distinct from the select
stall just fixed. pcdzero_count=0 (not the literal pc=0 fetch storm).
NEXT: trace the exception/IRQ delivery deadlock at fight init — likely a
kernel SleepEvent/WaitEvent or IRQ-handler reentry that never completes;
compare irqctx/exception path vs oracle once oracle reaches versus.

=== SESSION N+20 — FIGHT REACHED; crash root-caused to .ANI event-stream selection ===

PRIOR "kernel deadlock" THEORY DISPROVEN. psx_last_run_report + `sample`
showed two DIFFERENT fatal halts, neither an IRQ deadlock:

CRASH 1 (earlier instance, frame 1583): FAIL-FAST unknown dispatch
addr=0x80000080 — psx_check_interrupts read the RAM exception vector at
0x80000080/84, found it did NOT match the lui/addiu handler-stub pattern,
and dispatched the vector itself; the vector page was ALL ZEROS (never
written / cleared). Vector verified NONZERO at fresh boot
(0x3c1a0000/0x275a27ac/0x03400008 = lui/addiu/jr $k0 stub -> kernel
handler ~0x27AC, matching dispatch_tail kernel PCs). gwatch armed on
0x80000080/0x20 for the whole nav run: ZERO writes after boot — the
vector was never touched again, so crash-1's zeroed vector was likely a
one-off stale-state artifact of that older instance, NOT reproduced on
the fresh run (vector stayed intact through frame 9326).

CRASH 2 (fresh run + nav_to_fight.py, frame 9326) — THE REAL ONE:
The game reached ACTUAL FIGHT CODE. Host stack:
  battle_flow_state_machine (menu.c:756)
    -> fighter_frame (resolve.c:1833)
      -> action_commit (resolve.c:1931)
        -> ev_stream_dispatch (resolve.c:3111)
          -> br2_call_generated -> psx_unknown_dispatch(0x80122BCC)
FAIL-FAST: addr=0x80122BCC ra=0x80126664 a0=0x1F8000E8 a1=0x801F5328.

ROOT CAUSE (fully traced live):
- 0x80122BCC is NOT code — guest RAM there holds ASCII animation-state
  names ("DOWN","DAMAGE","SMALL","FALL","JUMP","FLOAT","QUICK","SLAM"...).
- ev_stream_dispatch reads op=*stream_pc; op>=24 indexes guest table
  0x801b4e68+op*4 and jalr's the entry. Real handler table = ops 0..47
  (0x8017xxxx/0x8014xxxx code ptrs). Entries "op48+" point into the
  0x80122Bxx NAME-STRING region — i.e. the table is only ~48 entries and
  op>=48 reads PAST it into an animation-name pointer table.
- The dispatched op was 0x58=88 -> table[88]=0x80122bcc -> jalr into
  ASCII -> unknown dispatch.
- Stream pc = motion+0x84 = 0x8006f648 (motion obj 0x801F5328).
  action_switch_guest (action.c:779, == generated func_8017A6E8 verified
  insn-for-insn): stream = ctrl->FC_ANI(+0x30) + u16(motion[0] + idx*2).
  Live: FC_ANI=0x8006f648, motion[0]=0x80077a0c, idx=1.
- ANOMALY: the u16 offset table at 0x80077a0c is ALL ZEROS (verified
  0x80077900-0x80077d00 = 1024 zero bytes) -> off=0 -> stream=FC_ANI.
- FC_ANI=0x8006f648 points at animation FRAME/keyframe data (u32 offset
  array + u16 key pairs), NOT an event-command stream -> first byte 0x58.

So either (a) the .ANI offset-table section was never populated (resource
load/parse gap — zeros where u16 action offsets belong), or (b) FC_ANI
points at the wrong .ANI section (frame data instead of event stream).
Both are .ANI load/parse bugs, upstream of ev_stream_dispatch. The clean
port's action_switch_guest is verified faithful to generated — the bug
is in whatever fills motion[0]'s table and ctrl+0x30.

NEXT: find the .ANI loader (sets motion+0x00..0x10 section ptrs and
ctrl+0x30) — likely generated fighter-resource code; check whether the
offset-table section is loaded at all (CD sector drop? wrong section
mapping? parse skip?). game/ani.c is only 116 lines — the real loader is
generated or in battle.c/menu.c fighter-spawn path.

=== SESSION N+21 — vector corruption root-caused: fighter+0xbc battle-struct ptr NULL ===

NEW CRASH (fresh run, frame ~1567), distinct from N+20's .ANI dispatch crash:
FAIL-FAST unknown dispatch addr=0x80000080 — the RAM exception vector was
corrupted mid-fight-init, then the next IRQ dispatched the bad vector.

EXACT WRITER FOUND via new instrumentation:
- guest_at() diagnostic (src/clean_bridge.c) logged VECGA addr=0x00000082
  ra0=post_battle_dispatch -> a CLEAN-C pointer store, not generated code.
- post_battle_dispatch (menu.c:6655, guest 0x80154930) phase-0 runs
  MW16(MB32(g2+0xbc)+0x82, -1) per player. g2=guest_of(&P1)=0x1F8000E8.
- MB32(0x1F8001A4) [P1 fighter+0xbc] = 0 -> write target = guest 0x82 ->
  br2_phys folds low addrs into RAM -> phys 0x82 = exception-vector page.
  Writes 0xFFFF at vector+2 -> corrupts the lui/addiu stub -> next IRQ
  (ra=0x8019A53C vsync path) dispatches 0x80000080 -> FAIL-FAST.

LIVE STATE at crash (all verified via read_ram):
- P1 fighter+0xbc (0x1F8001A4) = 0, P2 (0x1F8002AC) = 0.
- ENTIRE fighter-struct tail +0xA0..+0xC8 = 0 for both players (per-fighter
  battle/hitbox/collision struct ptrs never assigned).
- SHARED battle struct _DAT_1f8000bc = 0x801F0270 is ALL ZEROS (first 32B)
  -> battle_struct_init (menu.c:4564, writes +0x7c=1/+0x7a=0x1000/+0x82=-1)
  NEVER ran on it.
- Player phases = 0x300, fighter byte0 = active — PARTIAL init only.
- CD was actively seeking/reading frames ~1558-1560 (resource load live or
  stalled); async-op flag 0x801BDE40 = 0.

ROOT CAUSE: versus->fight runs post_battle_dispatch battle phases on
UNINITIALIZED battle/fighter structs. fighter+0xbc points to a per-fighter
battle/hitbox struct that is never allocated/linked, and the shared battle
struct 0x801F0270 is never battle_struct_init'd. The versus transition does
NOT execute the full battle-init sequence (menu.c:690-729 case5/6:
battle_struct_init/match_setup_apply/battle_state_init/per_side_setup/
action_enter_fwd) before post_battle_dispatch phase-0 dereferences them.

LIKELY UNIFIED WITH N+20: the .ANI offset-table zeros (N+20) and the
fighter+0xbc battle-struct NULLs (N+21) are BOTH symptoms of incomplete
fighter/battle resource init in versus->fight — consistent with the known
CD deferred-coroutine / async resource-load stall (todo #17): resource-
derived structs stay zeroed because the CD completion path never finishes.

NEXT: determine whether versus->fight ISSUES the fighter battle-struct /
resource load at all, and whether the async CD completion that would run
battle_struct_init + assign fighter+0xbc is firing. Compare vs oracle.
Do NOT patch fighter+0xbc or the vector — fix the missing init upstream.

=== SESSION N+22 — init sequence DOES run; bug narrowed to fighter+0xbc only ===

Instrumented battle_intro_flow/battle_round_init/battle_struct_init/
fighter_spawn_tail with stderr markers; fresh boot + nav_to_fight.py.

CONFIRMED the versus->fight init sequence RUNS TO COMPLETION:
  [BIF] sub=0 gs=6            -> battle_intro_flow entered (game_state 6)
  [FST] fighter_spawn_tail    -> fighter spawn ran
  [BIF] res_load iVar4=0 x~46 -> menu_res_load_sm polled async CD load
  [BIF] res_load iVar4=1      -> load COMPLETED
  [BRI] battle_round_init bc=0x801f0270
  [BSI] battle_struct_init p1=0x801f0270 p2=0x1f800000   (x2, two rounds)
  [FST] fighter_spawn_tail    -> second spawn
Then VECGA addr=0x00000082 -> same crash.

So the shared battle struct 0x801F0270 IS initialized (battle_struct_init
ran twice) and the whole menu_res_load_sm->battle_round_init->
battle_struct_init chain executes. The earlier "all zeros" post-mortem was
a PRE-init snapshot — the crash happens AFTER init completes.

REMAINING BUG (narrowed): post_battle_dispatch reads MB32(FIGHTER+0xbc)
where FIGHTER=0x1F8000E8/0x1F8001F0 -> scratchpad 0x1F8001A4/0x1F8002AC.
This is the PER-FIGHTER battle/hitbox-struct pointer — DISTINCT from the
shared battle-block+0xbc (0x1f8000bc=0x801F0270, which IS set+init'd).
fighter+0xbc is still NULL -> write folds to guest 0x82 -> vector corrupt.

NEXT: find what assigns fighter+0xbc (per-fighter battle struct ptr). It is
NOT in any clean-C fn (grepped: no '+0xbc =' writes) and NOT in
fighter_link_init/battle_round_init/battle_struct_init — so it is set by
GENERATED code, most likely menu_res_load_sm (per-fighter collision/hitbox
resource bind) or a fighter-resource relocation. Verify whether menu_res_load_sm
writes fighter+0xbc for versus fighters, or whether a per-fighter battle-
struct allocator/link step is being skipped.

=== SESSION N+22 — two clean-port guest/host pointer bugs fixed; next blocker = x28 clobber via context restore ===

FIXED (both verified vs generated/disasm, runtime-confirmed progress):

1. post_battle_dispatch phase-0 wrong base reg (game/menu.c ~6662-6673):
   `MB32(g2 + 0xbc)` read fighter-record +0xbc (legit 0) instead of the
   shared battle-state block. Guest asm 0x80154bc4 is `lw $v1,0xbc($s1)`
   with s1=param_3=0x1f800000 -> cell 0x1f8000bc = 0x801F0270. Clean code
   already used `MB32(s1 + 0xbc)` everywhere else in the same function;
   only the phase-0 block used g2. Fixed all 4 sites to `MB32(s1 + 0xbc)`.
   This was the N+21 "fighter+0xbc NULL / battle struct zeros" crash —
   NOT missing init (battle_struct_init DID run; struct is initialized).

2. battle_hud_draw local_78 wild pointer (game/battle.c:2318):
   `*(uint**)(&DAT_801f5210 + iVar4*4)` read 8 bytes = TWO guest u32 table
   entries as one host pointer -> local_78 = 0x801e9eb4801e9634 ->
   `*puVar16` SIGSEGV. Table 0x801f5210 holds guest pointers; fixed to
   `(uint*)guest_at(*(uint32_t*)(&DAT_801f5210 + iVar4*4))`.

3. keyframe_cursor_search table access (game/battle.c:14801-14810):
   `*(uint**)(pbVar6+0x18)` 8-byte read + bogus `guest_at(iVar5*20)+(uint)puVar7`
   index. Generated 0x801A97A4 is `lw 0x18(node)` (guest ptr) then
   `lw 4(tbl + iVar5*0x14)`. Fixed to guest_at(tbl) + u32-indexed +4 read.

NEW BLOCKER (frame ~1601, signal_11, battle_flow_state_machine):
- lldb: fault `ldrb w21,[x8]` at join-tail; x8=[x28]=0x100000407.
- x28 SHOULD be &_g__DAT_1f80000c (0x111bbcbc0) but was
  0x111bb0000-equivalent (file 0x111264000, inside .bss near the
  dispatch/ring buffers). Hardware watchpoint on g__DAT_1f80000c NEVER
  fired -> the global itself is fine (p/x shows valid arena ptr).
- => x28 (callee-saved) was CLOBBERED mid-function. Only longjmp/
  ucontext/asm can do that: the exception setjmp/longjmp or POSIX-fiber
  scheduler restored a stale x28 captured while executing ring/buffer
  code (irqctx/dispatch tails). This is the deferred-interrupt coroutine
  family — a psxrecomp RUNTIME bug, not game code.
- [PBD] debug print confirms fix #1 live: s1bc=801f0270 g2bc=0 f0bc=801f0270.

NEXT: trace which context-restore path delivers the stale x28
(psx_fiber ucontext swap vs exception_jmpbuf longjmp vs deferred
exception longjmp on wrong fiber). Watch g_exc_setjmp_epoch /
s_pending_exception_longjmp around the versus->fight IRQ storm.

=== SESSION N+23 — versus->fight crashes in .ANI event VM; character-data load never runs ===

NEW CRASH (after N+22 fixes), distinct from the +0xbc vector write:
  FAIL-FAST unknown dispatch: addr=0x80122DC8  ra=0x80126664
  a0=0x1F8001F0 (P2 fighter)  a1=0x801F5440 (P2 FC_MOTION default table)
  [PBD] absent -> post_battle_dispatch NOT reached; crash is EARLIER.

MECHANISM: script_op_dispatch (game/battle.c:13697) reads an opcode byte from
the script cursor (param_3+0x24) and calls handler *(0x801b5528 + op*4). The
handler table has 19 entries; a garbage op indexes past it into string/data ->
handler ptr = 0x80122DC8 (a data region) -> jump to non-code -> FAIL-FAST.
Root: the .ANI event stream the cursor walks is NOT the selected character's —
it reads frame/struct data as opcodes.

WHY: versus fighters keep BOOT-DEFAULT descriptor tables. players_ctrl_init
(game.c:2300) binds FC_MOTION=0x801f5328/0x801f5440 etc. at boot; nothing
rebinds them to the selected characters' loaded .ANI/.ACT/motion data.

CONFIRMED MISSING CALL: stage_char_data_load (0x80129218, game/menu.c:12083)
— copies the per-character param block 0x80200000 + idx*23140 -> work buffers
0x801c15f8/0x801c47c0/0x801cf88c/0x801e1b3c/0x801e0f6c — is NEVER invoked.
[SCDL] instrumentation produced ZERO output across a full versus->fight run.
It has no static clean-C caller; reachable only via an indirect guest
function-pointer. The versus character-select->fight transition is missing
the character-data populate (and likely the .ANI/motion file bind that goes
with it).

NOTE: fighter+0xbc (0x1f8001a4/0x1f8002ac) also shows ZERO gwatch writes, but
that is now MOOT — the +0xbc crash was the N+22 wrong-base-register port bug,
already fixed. Do not chase +0xbc as missing init.

NEXT: trace the versus select->fight path (battle_flow_state_machine substates
in game/menu.c:550, DAT_1f800001 machine; versus = DAT_1f80009b/09c modes) to
find where the original issues the character-data + .ANI load — the indirect
call that should reach stage_char_data_load / the per-character file loader —
and what args (char index, stage). Compare against oracle execution. Then port
the missing call faithfully; do NOT inject a fallback handler or fake the
opcode table.

=== SESSION N+24 — CORRECTION: versus->fight .ANI path WORKS; new blocker = scheduler-idle channel wedge ===

N+23's "missing character-data load" hypothesis was WRONG. Live instrumented
run ([APL]/[ASW] in apply()/action_switch_guest) proves versus fighters get
VALID .ANI blobs and event streams:

  [APL] ctrl=1f8000e8 ani=8006ba04 size=b80c char=7
  [ASW] p1=1f8000e8 ani=8006ba04 s0tbl=800738d4 stream=8006c4a0
  [APL] ctrl=1f8001f0 ani=800ce958 size=bd74 char=4
  [ASW] p1=1f8001f0 ani=800ce958 s0tbl=800d6e08 stream=800cf3ca

Live read_ram at the versus fight: gs=8 ph=8 sub=1, mode 9b=2 (versus),
P1 char=1 FC_ANI=0x8007121c, P2 char=8 FC_ANI=0x800d1020 — two DIFFERENT
characters, both valid .ANI. The 0x80122DC8 event-VM crash is GONE (fixed by
the N+22 guest/host pointer commits e55ff5a). stage_char_data_load is NOT
required for the fight .ANI path (it is the select-screen stats loader).

NEW BLOCKER — scheduler-idle wedge at frame 2319:
  get_registers: pc=0x00000000 (scheduler-idle sentinel — ALL kernel threads
  blocked on undelivered channel events), ra=0x80126664 (battle_flow_state_
  machine call site), a0=0x1F8001F0 a1=0x801F5440, cop0_epc=0x8019A648.
  frame frozen at 2319, process 99% CPU (busy-wait inside one frame tick).
  vblank_rate: cycle_paced_raise=2319 delivered=2149 -> ~170 vblanks LOST.
  cdrom_state: reading=0 request=0x80 (CD parked, not streaming).
  dma_state ch2: active=1 madr=0x801E82DC chcr=0x01000401 remaining=0.
  sio_state healthy: tx_writes=35556, both pads 0xFFFF, mc 114/114.

DIAGNOSIS: identical family to ACCURACY_BURNDOWN's channel-completion wedge —
kernel threads block on TestEvent/WaitEvent for channel specs whose done-bit
never registers. Versus reaches gs=8 ph=8 then starves; attract demos
completed (10 fight-init cycles) so this is versus/2P-specific — likely the
second-pad or heavier 2-char CD-stream path hitting the undelivered-spec set
{0x8000,0x100,0x2000}/F0000011. NOT a game-code bug; runtime IRQ/event fidelity.

NEXT: identify which kernel channel spec the versus fight's threads block on
(reuse ACCURACY_BURNDOWN ev_agg/TestEvent map) and which physical transfer
(SPU stream vs DMA vs CD sub-channel) its done-bit binds. Do NOT patch game
code — this is the deferred-interrupt/channel-delivery runtime bug.

=== SESSION N+25 — wedge root-caused: TestEvent spins on CD-DMA done (class F0000009 spec 0x20) ===

Live diagnosis of the versus->fight scheduler-idle wedge (frame 2319, pc=0):

- bioscall_dump: BIOS B0/0x0B = TestEvent called 1,440,713x (the busy-wait).
- bios_svc_trace: spinning call is TestEvent(a0=0xF1000008, a1=0,
  a2=0x01000201) -> v0=0 forever. ra=0x8019D550.
- Handle 0xF1000008 = EvCB slot 8. Event table base @0xA0000120->0x00008e08,
  16 slots x 0x1C. Slot8: class=0xF0000009 spec=0x00000020 status=0x2000
  mode=0x2000 func=0  => class F0000009/spec 0x20 = **CD-DMA completion**
  (matches ACCURACY_BURNDOWN "Slot8 F0000009/0x20 CD-DMA resolves").
- irqctx_ring: IRQs ARE delivered (vblank etc.); event_ring shows DMA
  KICK/DONE + ENQ/DEQ cycling — low-level machinery healthy.
- cdrom_command_history: last CD cmd = 0x09 (Pause) issued frame 2313,
  completed (pending.active=0 now). Prior: ReadN sectors 8767-8769 delivered
  frames 2312-2313 (sector_history data_delivered=1). reading=0, CD parked.
- cdrom_state: irq_flag=0x00 (NOT stuck — process_pending NOT blocked),
  request=0x80, stat=0x02.
- dma_state ch2 active=1 madr=0x801E82DC chcr=0x01000401 remaining=0 (GPU OT).
- sio healthy (tx_writes=35556, pads 0xFFFF, mc 114/114).

CONCLUSION: versus fight reaches gs=8 ph=8 then the game's CD-streaming thread
spins TestEvent waiting for a CD-DMA-done kernel event that never fires.
The game's CD driver calls DeliverEvent(F0000009,0x20) only when a CD data
transfer completes; no transfer is completing because the read pipeline is
stalled after Pause — either the game needs to issue another ReadN/ReadS that
isn't being issued, or a queued/in-flight CD-DMA isn't being serviced.

NEXT (runtime, cdrom.c/dma): determine whether a CD read/DMA the versus fight
requested is stuck un-issued (queued_cmd/pending) or issued-but-not-completing.
Compare against oracle at the same point (versus fight entry) to see the exact
CD command + DMA channel sequence the original runs. The versus path likely
streams 2-char match data the attract path doesn't, hitting a read the attract
demos never needed.

=== SESSION N+26 — wedge narrowed to starved CD-INTC coroutine ===

Live globals at the versus->fight wedge (frame ~2319, pc=0 scheduler-idle):

- testev_handle @0x801bde38 = 0xF1000008  (slot8 CD-DMA done, confirmed)
- cb8_dee8 @0x801bdee8    = 0             (callback fork NULL -> DeliverEvent path)
- ch4_handler @0x801bde04 = 0x8019B7E0    (cd_dma_chan_reset registered correctly)
- cb7_cursor  @0x801bddf4 = 0
- intc_count  @0x801bce00 = 0  DELTA=0 over 1s  -> cd_intc_dispatch NEVER re-entered
- intc_mask48 @0x801bcd48 = 0x0000000D
- irq_state: i_stat=0 i_mask=0xD pending=0 cop0_sr=0x40000401 (IEc=1 IM2=1)
- cdrom_state: reading=0 irq_flag=0 stat=0x02 request=0x80 mode=0xA0
               ring_dropped=1181 bfrd_set=2860  seq=56489
- dma_state: only ch2 (GPU OT) active; NO CD-data DMA channel in flight.

MECHANISM: the versus fight's CD streaming runs through the game's
cd_intc_dispatch coroutine (heartbeat 0x801bce00 bumped per entry). That
coroutine is STARVED — intc_count frozen at 0 — so the
CD-DMA-done -> cd_cb7_run -> cd_dma_chan_reset -> DeliverEvent(F0000009,0x20)
chain never executes for the pending transfer. slot8 stays ENABLED(0x2000)
instead of flipping READY(0x4000); TestEvent spins forever.

This is the SAME deferred-interrupt-coroutine family as the N+22 x28-clobber:
a coroutine resumed via deferred IRQ context is not being re-scheduled.
cdprobe showed cb7_run/chan_reset DO fire during attract fights (cumulative
hits), so the machinery works when the coroutine runs — versus hits a path
where the resume is dropped.

ring_dropped=1181 + CD fully idle (no read in flight, no IRQ pending) means
the game believes a CD-DMA is outstanding but the controller has none: the
read that should produce the DMA-done either was never issued, was dropped
from the ring, or completed without raising the IRQ that resumes the
coroutine.

NEXT: determine why cd_intc_dispatch's deferred-IRQ resume is dropped at
versus fight entry (vs attract which completes). Check the coroutine resume
path (deferred IRQ dispatch -> intc_state block 0x801bcd18) against oracle.
Do NOT hand-deliver the event — fix the coroutine resume.

=== SESSION N+27 — wedge = deferred-IRQ CD coroutine starved; resume chain broken ===

Deep-traced the starved cd_intc_dispatch (intc_count @0x801bce00 frozen at 0).

Coroutine contract (game/cd.c):
- cd_chan_full_init (0x8019a814, GENERATED) calls cd_ctx_save(0x801bcd50)
  which snapshots live cpu->gpr into a 12-word frame, registers it via
  BIOS B0/0x19 -> mem[0x4C18].
- On CD IRQ the kernel calls cd_ctx_restore (0x8019adcc) which reloads the
  frame and re-enters cd_chan_full_init with v0!=0 -> cd_intc_dispatch.
- cd_intc_dispatch clean thunk bumps intc_count then br2_call_generated.

Observed at wedge:
- intc_count=0 delta=0 -> cd_intc_dispatch NEVER entered -> coroutine never
  resumed after initial setup.
- ctx_frame @0x801bcd50[0]=0x8019a87c (saved ra in cd_intc region) present.
- mem[0x4C18]=0x0c3300005c2b0000... does NOT obviously hold a
  cd_ctx_restore(0x8019adcc) pointer -> B0/0x19 deferred registration may be
  un-wired or consumed.
- Spinner IS generated: ra=0x8019D550 inside cd_seek_complete_poll
  (0x8019D500, delegated to br2_call_generated) -> provides dispatchable
  resume PCs, so a CD IRQ *could* resume the coroutine IF one fired.
- CD queue EMPTY: pending={cmd 0x09 Pause, active 0}; last_sector lba 8769
  frame 2313; reading=0; i_stat=0 no IRQ pending; ring_dropped=1181.

CONCLUSION: the game finished streaming (last sector f2313), issued Pause,
went idle — but the FINAL CD-DMA-done interrupt that resumes the deferred
coroutine and DeliverEvents slot8 never fired / its resume was dropped.
slot8 stays ENABLED(0x2000); TestEvent spins forever.

NEXT (runtime deferred-IRQ / kernel B0-0x19): verify (a) cd_ctx_save's B0/0x19
call actually registered cd_ctx_restore at the kernel deferred-handler slot
the CD IRQ dispatches through, and (b) a CD completion IRQ actually reaches
that handler at fight entry. The versus path likely issues a read whose done
IRQ lands while the CPU is at a non-dispatchable PC (clean-C or BIOS boundary)
-> deferred switch dropped -> coroutine starves. Compare the deferred-IRQ
dispatch (mem[0x4C18] handler, psx_check_interrupts_at resume-PC capture)
against oracle. Do NOT hand-deliver the event.

=== SESSION N+28 — CORRECTION + sharper root cause: CD IRQ taken but cd_intc_dispatch sees I_STAT=0 ===

CORRECTION to N+26/N+27: intc_count @0x801bce00 is bumped ONLY by the clean
cd_intc_dispatch thunk, but gen_clean_table SKIPS 0x8019a8ec (the thunk's own
br2_call_generated makes gen_clean_table skip it) -> jal 0x8019a8ec runs
GENERATED br2eu_func_8019A8EC directly. intc_count=0 is EXPECTED and does NOT
mean the dispatch is starved. Discard that signal.

What IS true (live reads at wedge):
- cd_intc_dispatch computes mask = I_STAT(0x1F801070) & I_MASK(0x1F801074)
  & u16@0x801bcd48(=0xD). 0x1F801070/74 = PSX I_STAT/I_MASK (debug_server
  mmio map confirms). CD IRQ = I_STAT bit2 (0x4).
- irqctx_ring: IRQs ARE taken every frame at take_pc~0x8019A648 (inside CD
  wait fn 0x8019A5D8) with istat alternating 0x1 (VBLANK) AND 0x4 (CDROM).
  -> CD IRQs DO fire and ARE dispatched at a valid resume PC. The deferred
  coroutine resume is NOT starved for lack of dispatch.
- handler table @0x801bcd1c u32[11]: idx0=0x8019ae68(cd_cb8_dispatch)
  idx2=0x801979f8 idx3=0x8019af7c. Mask bits 0x1/0x4/0x8 -> idx 0/2/3.
  bit2(0x4=CD) -> handler[2]=0x801979f8 = the CD-DMA-done -> DeliverEvent path.
- I_STAT sampled live = 0 (between IRQs). slot8 stays ENABLED(0x2000);
  TestEvent spins. async-op flag 0x801bde40=0 (op completed).

ROOT CAUSE (narrowed): a CD IRQ (istat 0x4) IS taken and the deferred resume
reaches the CD dispatch region, but when cd_intc_dispatch samples
I_STAT&mask the CD bit is already clear -> effective_mask=0 -> handler[2]
(0x801979f8 -> cd_cb7_run -> cd_dma_chan_reset -> DeliverEvent F0000009/0x20)
never runs -> slot8 never READY -> TestEvent spins forever.

LIKELY BUG: I_STAT bit2 (CD) is being acked/cleared BEFORE cd_intc_dispatch
reads it — i.e. the runtime clears the CD interrupt status at IRQ-take (or
the kernel entry path) instead of letting the game's INTC handler ack it, so
the deferred dispatcher's I_STAT sample sees 0. cdrom.c comments describe
"the handler's own INTC ack then clears it" — the ack ordering is the suspect.
Versus streams more CD data than attract -> hits the lost-done window.

NEXT: trace one istat=0x4 IRQ take -> confirm whether cd_intc_dispatch runs
and what I_STAT it samples; check where I_STAT bit2 is cleared relative to
the deferred resume (psx_check_interrupts_at / kernel IRQ entry vs the game's
INTC ack in cd_intc_dispatch). Compare ack ordering vs oracle/Beetle.

=== SESSION N+29 — versus→fight reaches battle init; new blocker = event-stream OOB opcode ===

PROGRESS: post_battle_dispatch s1-base fix (menu.c, commit e55ff5a class) held —
run now reaches FIGHT REACHED, battle_round_init/struct_init, .ANI resource
loads, and fight runs ~5368 frames before failing.

NEW FAIL-FAST: unknown dispatch addr=0x80122F0C ra=0x80126664 a0=0x1F8001F0
(P2 ctrl) a1=0x801F5440 (P2 motion), frame 5368.

ROOT-CAUSE CHAIN (established this session):
- Dispatch site = clean ev_stream_dispatch (resolve.c:3094, guest 0x8017EA7C).
  op>=24 path reads handler = *(u32*)(0x801B4E68 + op*4) then
  br2_call_generated(h, gaddr(ctrl), gaddr(motion), pc, 0). Crash args match
  exactly. The GENERATED original (full_36.c:20856) has IDENTICAL semantics —
  lbu op; sll*4; lw table[op]; jalr — NO bounds check. Clean port is faithful;
  bug is upstream (bad stream byte), not the dispatcher.
- Guest table 0x801B4E68: entries 0-47 = real handler code ptrs (0x8017xxxx);
  entries 48-239 = a SECOND table of string/name ptrs (0x80122xxx — the
  "FURI/YORO/DOWN/BREAK" move-name pool). op=0xDF=223 indexed entry 223 =
  0x80122F0C (a string address) → unknown dispatch.
- Stream cursor = motion+0x84 = 0x800DC99A, byte there = 0xDF. Valid opcodes
  are only 0-47 → stream misaligned or stream base wrong.
- P1 healthy: motion+0x84=0x8006BF2E inside its .ANI (0x8006BA04+0xB80C);
  P1 S0 table @motion[0]=0x800738D4 = clean u16 offsets (0x49c,0x4a6,...).
- P2 corrupt: motion[0]=0x800D6E08=.ANI(0x800CE958)+0x84B0 = S0 section, but
  contains DENSE BINARY not u16 offsets. action_switch_guest computed
  stream=.ANI+u16(S0[idx])=0x800CE958+0xE042=0x800DC99A — offset exceeds
  .ANI size 0xBD74 → points into unrelated file region (files 0x90/0x94
  loaded at 0x800DC800).
- .ANI file 0x40 was LZSS-loaded to 0x800CE958 (decomp seq 3932). S0 section
  at ~69% into file is garbage → suspect truncated/corrupt delivery OR S0
  populated by a separate async load still in flight. cdrom_state at crash:
  ring_dropped=2767 (grew from 1181), last_sector lba8769 frame5362 — drive
  STILL streaming when fight logic consumed the S0 table.

OPEN: (a) does disc file 0x40's S0 section contain a valid u16 table
(subagent verifying vs Game.bin); (b) which of the 24 clean op<24 handlers
could return a misaligned next-pc (subagent auditing all vs generated);
(c) is motion+0x84 ever written as u16 elsewhere (field aliasing — unlikely,
battle.c +0x84 hits are different structs); (d) does the .ANI/S0 load complete
before action_switch reads it, or does ring_dropped leave holes.

NEXT: await subagent reports; if disc S0 is valid → CD delivery bug (holes
from ring_dropped or truncated LZSS); if disc S0 is also binary → wrong
section interpretation (motion[0] should point elsewhere / s0 field misuse).

=== SESSION N+29 — decoded the wedge spin: cd_seek_complete_poll wait loop ===

Disassembled generated br2eu_func_8019D500 (cd_seek_complete_poll). The spin:

  0x8019D500  lw v0, 0x801BDE40      ; async-op flag
  0x8019D51C  beq v0,1 -> ret(1)     ; async op in flight -> early out
  0x8019D524  lw v0, 0x801BDEEC      ; completion flag
  0x8019D530  bne v0,1 -> WAIT       ; not complete -> enter TestEvent loop
  WAIT (0x8019D540/560):
    lw a0, 0x801BDE38 (=0xF1000008 = EvCB slot8, CD-DMA done F0000009/0x20)
    jal 0x8019D5B0 (TestEvent trampoline)
    bnez v0 -> exit                  ; TestEvent==1 -> done
    else loop

Live values at wedge: 0x801BDE40=0 (async done), 0x801BDEEC=0 (complete flag
never set), 0x801BDE38=0xF1000008. So the game correctly waits for the
CD-DMA-done event; it never arrives because the CD-DMA channel-4 completion
interrupt is not being delivered through cd_intc_dispatch->handler[2]->
cd_dma_chan_reset->DeliverEvent.

dma_state shows NO active CD-data DMA channel (only GPU ch2) -> the ch4
CD-DMA transfer already finished (or never ran) and its done-IRQ was dropped
before cd_intc_dispatch could DeliverEvent. DeliverEvent(B0/7) ran 88x total
but not for this pending wait.

OPEN: determine whether the ch4 CD-DMA completion raised a CD IRQ (istat
bit2) that cd_intc_dispatch consumed without delivering, or whether the DMA
finished without raising it. Compare cdrom.c DMA-done->set_irq->present_
cdrom_irq path vs oracle. The versus path streams enough CD data to hit a
ch4 DMA completion the attract path doesn't.

=== SESSION N+30 — DMA IRQ cessation + ch4 CD-DMA channel idle ===

irqctx_ring histogram (1727 entries at wedge):
  istat 0x1 VBLANK x844, istat 0x4 CDROM x879, istat 0x8 DMA only x4-window
  -> actually DMA IRQs fired steadily frames ~1082-1716 (~every 2 frames)
     at take_pc inside the CD wait loop (0x8019D5B0/550/580) = the CD-DMA-done
     deliveries that let cd_seek_complete_poll proceed. LAST DMA IRQ seq21905
     frame1716 take_pc=0xBFC0574C (kernel). After frame1716: ZERO DMA IRQs
     while CD IRQs continue to the wedge (~2319).

DMA channel state at wedge:
  ch4 (CD-DMA, ch4_handler=0x8019b7e0=cd_dma_chan_reset):
    madr=0x000DDE60 bcr=0x03490010 chcr=0x00000201 active=0 remaining=0
    -> configured but NOT started (chcr bit24 start clear), idle.
  DICR=0x00900000 -> ch4 IRQ-enable (bit20) SET, so ch4 CAN interrupt.
  Only ch2 (GPU OT) active.

CONCLUSION: the CD-DMA-done the game waits on is a DMA-channel-4 completion
-> I_STAT bit3(0x8) -> cd_intc_dispatch tbl[3]=0x8019af7c -> cd_dma_chan_reset
-> DeliverEvent(F0000009,0x20) -> slot8 READY -> TestEvent returns 1.
DMA IRQs stopped being delivered after ~frame1716. ch4 is configured but not
running -> either the pending transfer's START was never kicked, or it
completed and its IRQ was dropped. The versus path initiates a ch4 CD-DMA
that attract never does (or at a point the IRQ path is already broken).

NEXT: determine whether ch4 was kicked for the pending read (watch ch4
chcr bit24 / madr/bcr writes via gwatch on 0x1F8010C0-C8) and whether a
completion IRQ was generated-but-dropped vs never-started. Compare the
versus CD-DMA channel usage vs attract/oracle.

=== SESSION N+29b — ROOT CAUSE CONFIRMED: heapB cinematic staging overlaps heapA .ANI ===

Subagent forensics (disc-verified): file 0x40 = MAR/MAR1P.ANI (LBA 6104,
24 sectors, raw/uncompressed, real size 0xBD74). Disc S0 section @file
0x84B0 is a VALID u16 table (RE_NOTES: motion+0 == relocated S0 copy,
verified verbatim on HW path). In-memory S0 is garbage because the .ANI
tail (file offset >=0x16A8 = guest 0x800D0000) was OVERWRITTEN POST-LOAD
by heapB staging cursor _DAT_801E3CFC.

Mechanism: select_screen_tick (menu.c:3324, legit pre-fight select path)
-> intro_movie_state -> sets _DAT_801e3cfc=0x800d0000 (game.c:1832) and
cd_file_load_sm(0x8b/0x16) loads packed cinematic files there (decomp
seq 4075/4076, frames 5347-5360, caller ra 0x80126664 = inside
battle_flow_state_machine call tree). P2's .ANI spans
0x800CE958..0x800DA6CC — it CROSSES 0x800D0000 → S0 section (offset
0x84B0 → guest 0x800D6E08) destroyed → action_switch_guest reads garbage
u16=0xE042 → stream=0x800DC99A (past .ANI end, inside TITLE*.DAT staging
at 0x800DC800) → opcode 0xDF → table[223]=string ptr 0x80122F0C →
unknown dispatch FAIL-FAST at frame 5368.

Live state at crash: g_game_state=8, battle_flow DAT_1f800001=8 (fight),
_DAT_801e3cfc=0x800D0000 (heapB base active), _DAT_801c47b8=0x800680DC.

Handler audit (subagent): all 24 clean op<24 EV handlers exonerated —
next-pc arithmetic matches generated/RE_NOTES. ev_stream_dispatch
(resolve.c:3094) is faithful to guest 0x8017EA7C (no bounds check in
either). Minor non-fatal bugs found: ev16_atkwin_init wrong arg
(ev.c:907 should be GU8(pc,2) not GU8(ctrl,2)); ev17_approach inverted
gate (ev.c:937); dead ev5_action wrong mode-1 gate (ev.c:160, no callers);
fprintf debug spam action.c:798/109 (no-printf rule — clean up later).

ROOT CAUSE = heap layout divergence: heapA placed .ANI spanning
0x800D0000 which heapB owns for cinematic staging. On HW heapA must keep
.ANI below 0x800D0000 (or heapB uses a different base at this phase).
Matches documented Session-15z heapA over-production (~+100KB). The
versus→fight cinematic load is LEGIT (select_screen_tick path) — the
bug is heapA trajectory, not a spurious state.

NEXT: trace heapA allocation trajectory (_DAT_801c47b8 decode cursor +
the .ANI placement allocator in save_state_machine_alt) vs oracle to
find the over-production source; OR verify whether heapB base 0x800d0000
is phase-correct (maybe heapB should be higher/lower during fight bring-
up). Compare heapA top-of-allocations vs DuckStation/Beetle.

### Session N+36 — heapA NOT over-produced; .ANI is select-preview leftover + bind skipped

Subagent full-trace analysis (/tmp/decomp_snap.jsonl, 5373 records,
boot->attract->versus->crash) REFUTES the "+100KB heapA over-production"
theory: versus->fight heapA consumption 0xF49E8 is the SMALLEST of all
cycles (attract C1 0xFC364, C2 0xF74C8). File sizes all faithful
(sector-count match). heap_serialize walk verified vs generated
(SLES_017.22_full_28.c br2eu_func_8016B7C0): cnt/ctx/rec/stride match.

REVISED mechanism (file_load caller-ra provenance):
- versus SELECT loads JEN.ANI(0x58)->0x8006BA04 + MAR.ANI(0x40)->0x800CE958
  (select-preview pair).
- versus FIGHT loads BUS.ANI(0x48)->0x8006EF18 + YUG.ANI(0x20)->0x800D2F70
  (DIFFERENT chars — selection is input/timing dependent per run).
- The fight .ANI lands at/above heapB staging base 0x800D0000 -> its S0
  tail is stomped by the post-load cinematic (intro_movie_state sound
  loader, files 0x14/0x8b/0x16 SND/*.DAT — FAITHFUL, confirmed via
  file_table.json: CHASEL_1.SND battle music etc).
- heapA is NOT reset between fight-load and cinematic in a way that
  saves the .ANI; _DAT_801e3cfc=0x800D0000 staging overwrites it.

LINK to Session N+5e gwatch proof: versus->fight does NOT re-run
fighter spawn/.ANI->FC_ANI bind (zero writes to 0x1f800118/220/110/218).
FC_ANI=0x800CE958 is a STALE select-preview pointer. Combined: the fight
uses a leftover .ANI that (a) may be the wrong character's and (b) sits
in the staging kill-zone. On HW the async char-resource pipeline must
re-load/re-bind .ANI to a safe heapA slot before case 8.

OPEN: is the versus->fight missing the fighter-resource-bind a state-
machine bug (wrong case/gate) or an async CD-load completion failure?
The .ANI files ARE loaded (file_load records prove it) but to heapA
addresses that collide with heapB staging — suggests heapA should top
lower OR .ANI/.ACT belong to a different arena on HW. Compare heapA
top vs Beetle oracle at versus->fight (needs 2P-capable oracle or
attract-demo equivalent).

### Session N+37 — versus→fight window fully mapped; ARCADE also wedges (general bug)

Live arcade run (tools/nav_arcade.py, port 4396) reached gs=8/case=8
("FIGHT REACHED") but is a SILENT WEDGE, not a fight: battle struct
_DAT_1f8000bc=0x801F0270 is ALL ZEROS, fighter objects 0x801F5258/
0x801F52A0 have only +0x18/+0x1c/+0x3c populated (heapB ptrs), ctrl
blocks 0x1f8000e8/0x1f8001f0 cross-linked but NO .ANI/FC_ANI binding
anywhere, sp_ring shows only the per-frame vsync ping-pong
(0x8019A53C<->0x8019A87C). Confirms commit ad1f507: fight-init bug is
GENERAL (arcade+versus); versus just crashes loudly on the corrupted
.ANI while arcade soft-locks.

SM-sharing discovery: DAT_1f800001 (0x1f800001) is a SHARED sub-state
byte used by battle_flow_state_machine (cases 0-13), select_screen_tick
(own do-while stages 0->6->0xf0->0xf1->0xff, resets to 0 on exit),
match_loop, system_config_menu_a/b, results_scoreboard,
player_name_entry, transition_screen_flow, battle_intro_flow,
attract_menu_dispatch. select_screen_tick is a BLOCKING infinite SM
called from battle_flow case 1; it calls intro_movie_state at its
stage 0 (menu.c:3324) and fighter_spawn_setup at stage 6
(menu.c:3008-3009) — the select-preview fighter spawn.

Versus→fight file_load window (decomp_snap.jsonl seq4880-5373):
- fight resources ra=0x801265F4 (menu_res_load_sm pump):
  0x48 BUS.ANI->0x8006EF18, 0x4c BUS.ACT->0x8007A5BC, 0xc4->0x800F0000,
  0xc0->0x801C47C0, 0x25->0x801C47C0, 0x20 YUG.ANI->0x800D2F70,
  0x24 YUG.ACT->0x800DEAC8
- phase reset ra=0x80126594: 0x8a->0x80010000 (heapA reset),
  0x90/0x96->0x800DC800 (title/stage staging)
- cinematic ra=0x80126664 (outer-loop level, entry case region):
  0x14 P00.DAT->0x800D0000, 0x8b CHASEL_1.SND->0x800D0000,
  0x16 SE00.DAT->0x800D0000 — battle music/stage data, requested as
  fight begins (seq5337+ AFTER .ANI loads; async completion order).

P2 .ANI at 0x800D2F70 sits INSIDE heapB staging arena (base 0x800D0000).
Attract fights also place P2 .ANI high (0x800D1020/0x800D1FF0) but
survive: no cinematic stages at 0x800D0000 while their .ANI is live.
Versus is uniquely exposed: .ANI live + immediate sound staging at
same base. On HW this cannot corrupt -> either heapA tops ~0x30000
lower (over-production) or the sound staging/.ANI lifetime differs.
Oracle attract-demo heapA-top check in flight (agent 225dc84c).

ANOMALY: arcade log has ZERO [BSI]/[BIF]/[PBD]/[SCDL] fprintf hits yet
gs=8/case=8 persists -> either battle_flow entered with case byte
pre-set to 8 (skipping cases 0-7 incl. battle_struct_init) or the
byte was left at 8 by a prior SM. battle_flow has no entry guard for
case>1 (only -1/0xff resets to 0). Needs verification whether case 8
body actually executes (PBD gated by DAT_1f80009b==4 ->
battle_sides_commit path has no print).

### Session N+38 — wedge = kernel channel-event deadlock (clean-independent); live DMA/SPU state captured

Live arcade wedge (port 4396) root-caused via debug server:
- bios_svc_trace: B0/0x0B TestEvent(0xF1000008) spins ~1.4M calls,
  ra=0x8019D550 (cd_seek_complete_poll region). Event handle
  0xF1000008 @0x801BDE38 = OpenEvent(0xF0000009,0x20) — the CD-DMA-done
  flag delivered by game cd_dma_chan_reset (cb7 handler).
- gs=8/case-byte=8 are FROZEN scratchpad values: gwatch on
  0x1f800008/0x1f80000c (written every battle_flow call) = 0 hits;
  fntrace on battle_flow/match_loop/battle_struct_init = 0 hits.
  battle_flow IS blocked mid-case-8 inside a callee — frames still
  advance via VBlank/other kernel threads (sp_ring ping-pong
  0x8019A53C<->0x8019A87C = thread scheduler).
- dma_state: ALL channels idle (active=0, delayed_complete drained).
  ch3(CDROM) CHCR=0 done; ch4(SPU) CHCR=0x201 = direction+enable, NOT
  busy (bit24 clear) — SPU transfer completed. DICR=0x00900000.
- spu_status: active_mask=0, all voices inactive env_phase=3,
  last_peak=0 — SPU silent (sequencer never re-arms voices).
- event_ring tail: healthy per-frame VBlank->GPU(ch2)/OTC(ch6)
  DMA_KICK/DMA_DONE cycling; ZERO ch4(SPU) or ch3(CD) activity —
  the versus→fight stream is dead while the render loop lives.
- bioscall_dump: B0/0x0B(TestEvent)=1.4M dominant, B0/0x17=218K.

CONCLUSION: the versus→fight blocker is the documented kernel
channel-event deadlock (ACCURACY_BURNDOWN N+1): game waits on
DeliverEvent for F4000001/{0x8000,0x2000} + F0000011/0x04 +
F0000009/0x20 whose BIOS device-ISR producers never fire for the
needed specs. i_mask=0xD, i_stat shows only VBlank pending. This is
CLEAN-INDEPENDENT (pure-gen reproduces) = faithful-hardware IRQ/DMA
completion->DeliverEvent edge, NOT a clean-port bug. The .ANI
corruption is a versus-specific SECOND issue (or downstream symptom).

OPEN: which device IRQ should raise the missing DeliverEvents —
ch4-SPU done? ch3-CDROM done? SIO? DICR=0x00900000 shows only ch4
enable+master; need spec->IRQ-bit map vs what real HW raises during
versus→fight multi-stream. SPU agent (5f215e7d) + ordering agent
(225dc84c) + port-audit agent (7cc2edcc) still running.

### Session N+39 — versus→fight ordering resolved: shared loader-stage byte 0x1f80008c + attract-pipeline detour

Agent 225dc84c ra-provenance map (file_load len = caller ra =
g_game_state case): case6 0x801265F4=battle_intro_flow→menu_res_load_sm;
case5 0x80126594=match_loop→attract_stage_load_step;
case8 0x80126664=battle_flow→select_screen_tick→intro_movie_state.

Faithful versus→fight order (decomp_snap seq):
- gs6 battle_intro_flow→menu_res_load_sm: ALL fight .ANI/.ACT into heapA
  (JEN/MAR preview pair, then BUS/YUG fight pair; P2 .ANI->0x800D2F70,
  heapA tops 0x8010D2E4). st00 resets heapA=0x80010000 heapB=0x800f0000.
- gs5 match_loop→attract_stage_load_step: heapA RESET->0x80010000,
  heapB->0x800DC800, TITLE files->0x800DC800.
- gs8 battle_flow→select_screen_tick→intro_movie_state: heapB->0x800D0000,
  files 0x14/0x8B/0x16->0x800D0000 STOMP the high .ANI -> crash ~5368.

heapB=0x800D0000 stomp is GENERATED-FAITHFUL (intro_movie_state stores
verified vs SLES_017.22_full_02.c:6560). NOT the bug.

ATTRACT SURVIVAL MECHANISM (the key): every attract cycle does
[resources high] -> [heapA reset] -> [file 0x13 + FULL fighter-resource
RELOAD ra=0x801265F4 into fresh low heapA] -> fight. The reload re-binds
.ANI->FC_ANI so the stomped copy is dead weight. Versus detours through
gs5/gs6 attract pipeline and crashes BEFORE its post-movie reload.

SHARED-BYTE ALIASING (verified clean+generated): guest byte 0x1f80008c
is ONE stage variable shared by intro_movie_state (game.c:1796-1872,
-1=done) AND menu_res_load_sm (save.c:4114 reads s1+0x8c, <0 early-
return) AND match_loop resets it to 0 at entry (game.c:1450). So after
the movie sets -1, a subsequent menu_res_load_sm SKIPS unless match_loop
(or another resetter) zeroes it first. The faithful reload requires a
gs5->gs6 cycle AFTER the movie.

LIVE arcade wedge confirms: 0x1f80008c=0xffff (-1) at freeze,
pad_state_07c=0 (attract-menu gate 0x840 clear — attract detour NOT
armed in arcade), DAT_1f800098=5.

OPEN (agent prime suspects):
1. match_loop clean port vs generated br2eu_func_80147034 — the
   DAT_1f800001==2 -> attract-menu transition + _DAT_1f80007c&0x840 pad
   gate; phantom pad state in 2P versus could arm attract_menu_dispatch
   -> g_game_state=8 erroneously.
2. battle_intro_flow (menu.c:1857) state2 fight-rounds engagement for
   versus vs yielding to attract; leftover fprintf debug at 1871/1956.
3. Whether faithful HW reloads .ANI post-movie (needs oracle heapA-top
   or a versus-capable oracle — oracle read_ram returned stale constants).
4. State-byte audit across the 5+ SMs sharing DAT_1f800001/08c/08e/090.

### Session N+40 — versus→fight and arcade CONVERGE on one deadlock: fight-init kernel event wait

Live versus repro (port 4397, nav_to_fight.py): reaches gs=8 ph=8
(FIGHT REACHED) at ~frame 2270 then frame FREEZES at 2276 — identical
signature to the arcade wedge. NOT the .ANI crash this run (heapA stayed
low at 0x800680dc; .ANI below heapB staging → no stomp). The .ANI crash
is a SEPARATE character/timing-dependent latent bug.

State-byte trace (/tmp/vs_trace.jsonl) captured the faithful versus
order: gs6 battle_intro_flow preload (heapA->0x8010ae54) -> gs5
match_loop RESET heapA->0x80010000 heapB->0x800DC800 + title load ->
gs8 battle_flow -> select stage0 intro_movie_state (heapB->0x800D0000,
b08c stages 16->32->48->-1) -> ph=8 fight. The gs6 preload IS followed
by a heapA reset + reload into low heapA — the reload mechanism fired.

DEADLOCK SIGNATURE (both modes):
- cpu.pc=0x00000000 = scheduler-idle sentinel; NO runnable thread.
- ra=0x80126664 (battle_flow→select case8 caller) = last real context.
- bioscall B0:11 TestEvent = 1,440,713 cumulative — kernel thread
  blocked in TestEvent on an event flag never DeliverEvent'd.
- i_stat=0, pending=0, imask=0xD (VBlank+CDROM+DMA IRQs enabled).
- ALL DMA channels inactive EXCEPT ch2: active=1, bcr=0x0 (count 0!),
  chcr=0x01000401, cycles_accum=1 — degenerate stuck "active" GPU/OT
  channel, never completes.
- CDROM: reading=0, pending=0 (Pause 0x09 completed/cancelled), last
  sector lba=8769 @frame2270, sector_read_pos=2060/2340.
- Last event_ring entries: VBlank IRQ_DELIVER then DEQ storm, then
  silence. CD command history ends: Setloc(0x02)→ReadN(0x06)→Pause(0x09)
  @frame2270 — the Pause is the last CD op before the wait.

OPEN: which event flag the blocked TestEvent waits on (EvCB walk empty,
need TCB wait-queue decode), and which producer should DeliverEvent it.
Prime suspects: (a) ch2 GPU/OT DMA stuck-active never completing;
(b) CD Pause(0x09)/read-complete DeliverEvent lost; (c) SIO0 channel
event. LIVE repro preserved on port 4397 (frame 2276 frozen).

### N+40b — deadlock refined: WHOLE cycle/event pump frozen, not just DMA

Follow-up probes on the live 4397 wedge (frame 2276):
- event_ring total FROZEN at 113926 (no new events over 3s) =>
  psx_cycle_count is NOT advancing. This is a HARD stall of the exec/
  dispatch path, not merely device starvation.
- cpu.pc=0x00000000 idle sentinel; process at 0.6% CPU (BLOCKED/sleeping,
  not spinning — vs arcade 4396 which spins TestEvent at 99.7%).
- Last events mode=INTERP pc=0x801926A8 func=0x000029CC (kernel).
- GPU OT linked-list walk (ch2) frozen at nodes=1 words=10 active=1 —
  consistent with global pump stall (dma_advance never called).
- idle_skip DISABLED (skips=0) — ruled out.
- OT table dump @0x801E618C region shows a valid terminator node
  0x09FFFFFF (wc=9,next=END) matching words_last=10 — walk was 1 cycle
  from finishing when the pump froze.

=> The versus wedge is the runtime BLOCKING (no runnable thread / a wait
that never wakes), while arcade spins TestEvent. Same fight-init root,
different host manifestation. Prime suspect shifts to the fiber/
coroutine scheduler or a blocking wait in the INTERP dispatch path —
NOT a per-device completion bug. Deadlock agent 4dea3363 on live repro.

### Session N+41 — UNIFICATION: versus crash + arcade wedge + SPU starvation = ONE root

Deadlock agent 4dea3363 (live repro 4397) FATAL-HALT decoded via `sample`:

```
psx_fatal_halt <- psx_unknown_dispatch <- br2_call_generated(0x80122DC8)
<- ev_stream_dispatch (resolve.c:3106) <- action_commit <- fighter_frame
<- battle_flow_state_machine (menu.c:758)
FAIL-FAST unknown dispatch addr=0x80122DC8 ra=0x80126664 frame 2292
```

0x80122DC8 = ASCII string data ("FURI/YORO/DOWN/BREAK…"), NOT code —
the event-opcode table @0x801b4e68 has only 48 valid handlers (ops 0-47);
op>=48 over-reads into the adjacent string table. Guest 0x8017ea7c has
NO bounds check either → real HW faults identically. So the crash is
DATA CORRUPTION upstream, not a dispatch bug. Do NOT seed 0x80122DC8.

motion+0x84 (event-stream pc) = 0x800D4165 = odd addr inside binary
keyframe data (repeating 12 00 XX XX) → the .ANI/.ACT stream base or a
prior stream next-pc is corrupt → op byte 0xBF → table[191]=0x80122DC8.

UPSTREAM ROOT (unifies ALL symptoms):
- spu_status: active_mask=0, eon=0xFFFF08, reverb_on=0 — ZERO voices.
- ch4 DMA prepared (madr=0xDDE60 bcr=0x03490010 chcr=0x201 NO start bit)
  but never kicked. Kick comes only from audio_seq_reset via state-9 cb
  0x801bdeec — never re-armed for fight.
- => sound stream starves => kernel channel events (F0000009/0x20 ch4-DMA,
  F4000001 specs, F0000011 SIO) never DeliverEvent => async file-load /
  stream consumer never finishes => .ANI/.ACT incomplete => ev_stream
  reads garbage op => fatal (versus) / TestEvent spin (arcade).

The "kernel channel-event deadlock", "CD-DMA mid-sector stall",
"SPU stream starvation", and ".ANI corruption" are ONE bug: the fight's
audio/SPU stream (ch4) is never kicked, starving the shared kernel
event pipeline that also drives CD file loads.

CH2 GPU-OT "stuck" was a RED HERRING (normal linked-list walk in flight;
DMA_KICK→DMA_DONE pairs completing). CD Pause(0x09) healthy. idle_skip
off. The whole-pump "freeze" was psx_fatal_halt's nanosleep debug loop.

FAITHFUL FIX DIRECTION: root why audio_seq_reset/state-9 cb 0x801bdeec
isn't re-armed for versus/arcade fight (game-state sequencing) OR why
the ch4 kick path drops it (runtime). Verify .ANI load completion via
watchpoint on motion+0x84 (0x801F54C4). NO fakes — fix the real kick.

### Session N+42 — slot-9/ch4 mechanism decoded; versus→fight flow-routing is the unified suspect

Two agent reports converge (4dea3363 fatal-halt decode + 5f215e7d SPU
wedge decode) + my live RAM reads on the halted 4397:

CONFIRMED LIVE STATE (versus→fight fatal @frame2292):
- P1 FC_ANI(+0x30)=0x8007121C (SAFE, low heapA)
- P2 FC_ANI(+0x30)=0x800D1020 — INSIDE heapB staging kill-zone (0x800D0000+)
  → stomped → S0 table corrupt → stream_pc(motion+0x84)=0x800D4165 into
  keyframe data → op 0xBF → table[191]=0x80122DC8 string → fatal.
- DMA chan-handler table 0x801bddf4: ONLY ch4=0x8019B7E0 armed; ch0-3,5,6
  NULL. Audio slot registry 0x801bcd1c: slots 0,2,3 armed (CD handlers),
  slot9=0, mask 0x801bcd48=0xD (bit9 CLEAR).
- cd_dma_chan_reset (cd.c:5008): DeliverEvent(0xF0000009,0x20) fires only
  when 0x801bdee8 callback==0; else calls the callback.

ROOT-CAUSE CANDIDATE (unifies .ANI crash + audio wedge):
versus→fight routes intro_movie_state through battle_flow→select_screen_
tick (gs8), NOT through save_flow_state_machine (0x8015D420). Only the
save_flow path calls chan_reset_register (0x8015D4B4, gated on
intro_movie_state!=0) which arms slot9→ch4→DeliverEvent. battle_flow's
intro_movie_state does NOT arm slot9 → ch4 starves → file-load coroutine
TestEvent(0xF0000009/0x20) spins → resource load incomplete → .ANI corrupt.

Q: is faithful versus→fight SUPPOSED to route through save_flow (arming
slot9 + re-binding .ANI) and our clean port takes battle_flow instead?
That would be ONE state-routing divergence causing BOTH symptoms.
chan_reset_register/clear + audio_seq_reset + cd_state9_store are clean;
0x8015D69C flow SM + save_flow are the suspects. Oracle (DuckStation
:4371) is stuck at gs=4 — unusable for nav without more work.

### N+42b — P2 FC_ANI confirmed pointing into staged SOUND data, not a real .ANI

Live RAM dump on halted versus 4397:
- P1 FC_ANI=0x8007121C → valid .ANI section-offset table (0xB064,0x7AF4,
  0xE4,0x7F38… plausible S0-S9 offsets).
- P2 FC_ANI=0x800D1020 → bytes `21042004 04000500 22000600…` = packed
  16-bit keyframe/sound data, NOT a section table.
- heapB staging @0x800D0000 contains the SAME `2104 2104…` byte pattern
  as P2's "ANI" → P2 FC_ANI points INTO the staged SND/DAT region
  (files 0x14/0x8B/0x16), i.e. FC_ANI is a STALE pointer into sound
  data, not a loaded .ANI at all.

=> versus→fight never gives P2 a valid .ANI binding. P1 bound correctly
(low heapA), P2's FC_ANI still references the pre-reset/high region
that heapB staging overwrote with sound data. The faithful path must
re-load/re-bind BOTH fighters' .ANI after the heap reset + staging;
versus binds only P1 (or P2's bind targets the stale slot).

This is now the PRIMARY versus→fight defect (independent of the audio
slot-9 wedge which is the arcade/versus TestEvent symptom — likely the
same incomplete-init root). Flow-routing agent a1f5c1dc + heapA agent
03b98d5b hunting the faithful sequence.

### N+43 — versus→fight reframed: ph-flicker re-entry, NOT save_flow / slot9

Subagent a1f5c1dc (decisive flow-routing) REFUTED two prior hypotheses:
- versus→fight does NOT route through save_flow_state_machine — that SM
  is the OPTIONS/config screen (gs=11) stepper callback, reached only via
  dialog_msg_dispatch case 9. chan_reset_register/clear only run there.
- slot9=0 / mask 0xD during versus→fight is FAITHFUL — a session that
  never opens options legitimately never arms audio slot9. The earlier
  "missing slot9 → CD wedge" theory was a red herring for versus.

REAL mechanism (live port 4490): versus→fight reaches gs=8 ph=8, then
DAT_1f800001 (battle_flow sub-state) FLICKERS 8→1→8 each cycle. Every
flicker re-enters select_screen_tick/intro_movie_state → re-stages
heapB at 0x800D0000 → stomps P2's high-heapA .ANI (FC_ANI=0x800D1FF0,
legit heapA high-water) → stream_pc into keyframe data → op 0xBF →
FAIL-FAST unknown dispatch. Fight GTE loop (0x801851D8) runs between
flickers. heapB=0x800D0000 (intro_movie value, NOT menu_res_load_sm's
0x800F0000) is direct evidence of movie-phase re-entry.

Also fixed (faithful, verified vs generated lui+addiu sign-extension):
- menu.c:13256 stepper cb 0x8016d69c -> 0x8015d69c
- menu.c:13337 stepper cb 0x8016ee40 -> 0x8015ee40
(masked today: fn_lookup returns NULL; observably wrong guest values)

NEXT: subagent 23003b64 hunting the 0x1f800001 writer that resets ph
during fight (shared-SP-byte clobber vs rollback path). Arcade
(09c=0→battle_frame) comparison pending.

### N+44 — ch3 CD-ROM DMA IS kicking+completing (refines N+43c/d "PIO" read)

Live dma_trace_dump on fresh versus run (port 4512) shows channel-3
CD-ROM DMA transfers START+COMPLETE cleanly and repeatedly:
  start ch3 chcr=0x01000000 (bit24 start, dir=to-RAM)
  complete ch3, madr advances 0x800DC800→0x800DD000→0x800DD800→0x800D0000
  bcr=0x10200 (~sector-sized blocks) — these ARE the file-load dests.
dma_state snapshot ch3.chcr=0 is just BETWEEN transfers (async).

=> The wedge is NOT "ch3 never fires". File data DOES arrive via ch3 DMA.
The gap is narrower: either (a) ONE specific transfer stalls mid-sector
(N+43 pos 2060/2340) and never completes, or (b) a completion→
cd_dma_chan_reset→DeliverEvent link drops for a particular transfer,
or (c) versus ph-flicker is a SEPARATE bug from the arcade TestEvent
wedge. dicr=0x00900000 steady (bit23+bit20) across snapshots.

Open: which single ch3 transfer stalls, and does its complete_transfer
fire? Agents 23003b64 (ph-flicker) + 03b98d5b (heapA/CD) + 7cc2edcc
(latent) still running.

### N+44 — CD wedge root-caused to ORPHANED deferred-IRQ coroutine (this session)

Live-traced the :4405 versus->fight TestEvent wedge end-to-end (fntrace +
irqctx_ring + read_ram + generated disasm). The CD file read is NOT a DMA
transfer — it is a PIO read driven by a per-sector coroutine, and the
completion signal is a kernel deferred-interrupt coroutine that is never
resumed.

Confirmed chain (all live-verified):
- Read data DOES arrive: buffer @0x800d2800 fully populated, sectors-remaining
  DAT_801bcb94 5->0. cdrom_state frozen at sector_read_pos=2060/2340 with
  sector_available=1 + response_count=1 (game stopped issuing PIO reads
  mid-sector because its coroutine is blocked, not because the drive stalled).
- cd_file_read_seq -> cd_seek_complete_poll(a0=1) waits on
  cd_seek_state_machine(@0x801bde38) for event flag 0xf0000009.
- 0xf0000009 is delivered ONLY by cd_dma_chan_reset (0x8019b7e8) via
  cd_bios_svc_07, reached through cd_intc_dispatch's channel-done handler.
- cd_intc_dispatch (0x8019a8ec) is reached ONLY via the deferred coroutine:
  cd_chan_full_init(0x8019a814) -> cd_ctx_save(@0x801bcd50, resume PC
  0x8019a87c) -> [CD IRQ] -> cd_ctx_restore(0x8019adcc) -> re-enter
  @0x8019A87C -> cd_intc_dispatch -> DeliverEvent.

THE BREAK: cd_chan_full_init IS registered (ptr @0x801bdd8c in the CD driver
object @0x801bdd80) and its ctx frame IS armed (@0x801bcd50 ra=0x8019a87c),
but cd_intc_dispatch / cd_ctx_restore / cd_chan_full_init ALL have zero
invocations (fntrace total=0 over 3s) despite CDROM IRQs firing continuously
(irqctx_ring istat=0x4, take_pc=0x8019A648/0x8019A600, same_thread=1,
restored=1). The kernel/deferred-resume that should call cd_ctx_restore on CD
IRQ is not wired -> coroutine never resumes -> no DeliverEvent -> TestEvent
spin -> gs=8 ph=8 frozen.

Secondary: even if cd_intc_dispatch ran, its mask = *ptrA(0x1f801070)=0 &
*ptrB(0xd) & mask(0xd) = 0 -> no handler bits -> nothing dispatched. ptrA
reads the CD interrupt-pending status as 0 (IRQ may be acked before dispatch,
or ptrA targets the wrong status source).

NOTE vs N+43: this CD wedge is a REAL defect (any TestEvent(0xf0000009)
waiter wedges) but versus->fight is nondeterministic — some runs hit this
CD wedge, others hit the N+43 DAT_1f800001-flicker -> heapB-stomp -> .ANI
corruption crash. Likely shared root = incomplete async init / IRQ-resume
fidelity; the flicker is the documented PRIMARY versus defect (subagent
23003b64 / vs_watch.py hunting the 0x1f800001 writer).

NEXT: find what should invoke cd_ctx_restore/cd_chan_full_init on CD IRQ —
the kernel deferred-handler dispatch (OpenBIOS intr path) or a game-registered
slot. Compare OpenBIOS interrupt dispatch vs DuckStation kernel model.

### N+45 — versus ph-flicker: writer inventory + arcade is STABLE (no flicker)

Subagent 23003b64 report:
- store_pc field was a red herring; true writer = host rip via
  `atos -l <slide>`. All captured 0x1f800001 writes are LEGIT SM
  transitions (match_loop=0, select_screen_tick, battle_flow case7→8).
  No rogue 8→1 captured yet — flicker is intermittent/timing-gated.
- Complete writer inventory: clean-C (battle_flow, select_screen_tick,
  match_loop, player_name_entry, transition_screen_flow, card_fade_sm,
  battle_intro_flow, attract_menu_dispatch, system_config_menu_a/b,
  results_scoreboard) + generated-only leaves (0x80126704 abort,
  0x8014CDA8 ph=9, 0x8014CDBC ph=0xF0). 23 byte+1 store sites, no
  hidden sw/sh/memset clobber of 0x1f800001.
- KEY: arcade (09c=0→battle_frame) is STABLE at ph=8 — NO flicker.
  Versus (09c=1→select_screen_tick) flickers → versus-specific.
- Prime suspects for 8→1: match_loop(game.c:1469 =1, attract loop —
  would fire if gs dispatch wrongly re-enters it), player_name_entry
  (menu.c:1645 =1), battle_flow re-entry (case0→1 / -1 exit→re-init).
- Repro left armed: port 4512 (versus, stable ph=8, gwatch armed over
  scratchpad window) + port 4511 (arcade). Poll gwatch_dump until an
  8→1 write lands, then atos the rip.

PARALLEL: new agent c112f7fb hunting the CD deferred-coroutine resume
(cd_ctx_restore never invoked on CD IRQ → no DeliverEvent → TestEvent
wedge). Both threads likely share "incomplete async init / IRQ-resume
fidelity" root.

### N+46 — :4405 "CD wedge" was POST-MORTEM; real crash = ev_stream wild dispatch + host-ptr leak

lldb backtrace on halted :4405 (PID 68349, port 4405) proves the instance was
NOT a live CD wedge — it had already fatal-halted and parked in
`psx_fatal_halt -> nanosleep`. Every "frozen" CD/irqctx/frame reading taken on
it (sector_read_pos=2060, irqctx seq=23175, frame=2292, i_stat=0) is POST-MORTEM
state, not a live stall. Correcting the earlier N+44 attribution for THIS
instance.

Actual crash chain (native bt, all live-verified):
  battle_flow_state_machine -> fighter_frame -> action_commit
    -> ev_stream_dispatch (resolve.c:3111) -> br2_call_generated
    -> psx_unknown_dispatch(addr=0x80122F0C) -> trap_crash -> psx_fatal_halt

0x80122F0C is rodata ASCII ("FURI/YORO/DOWN/BREAK"). ev_stream_dispatch reads
op=*pbVar1 (pbVar1=guest_at(motion+0x84)); a corrupt stream PC/op makes
h=*(0x801b4e68+op*4) read OOB -> br2_call_generated(garbage) -> wild dispatch.
This is the N+43 .ANI-corruption manifestation (P2's stream PC points into
staged sound/text data after the heapB stomp).

SECOND manifestation — root psx_last_run_report.json (frame 540):
  "DISPATCH FATAL: misaligned target 0x00120201", ra=0x801265F4 (battle_flow),
  a0=0x1183C1F0, a3=0x1F800000. a0 is a HOST heap pointer (clean-struct addr)
  sitting in a guest register -> generated code used it as a guest addr ->
  wild dispatch. This is the host<->guest ptr-convention leak (todo #18)
  reaching the dispatch boundary, distinct from the .ANI-corruption path but
  same battle_flow->ev_stream/action_commit callsite region.

NET: versus->fight has (at least) two live crash families — (a) N+43
DAT_1f800001 flicker -> heapB stomp -> .ANI stream-PC corruption -> ev_stream
OOB handler -> wild dispatch; (b) host ptr leaking into guest reg/mem ->
misaligned dispatch. Both nondeterministic, both in the fighter_frame/
action_commit/ev_stream path. CD deferred-coroutine (cd_ctx_restore) remains
a SEPARATE real defect for the TestEvent(0xf0000009) wedge manifestation but
is NOT what halted :4405.

NEXT: capture motion+0x84 (stream PC) + the op byte at crash to confirm
stream-PC-vs-data corruption; and trace where a host ptr (0x118xxxxx) is
stored into guest-readable state that generated code later dispatches on.

### N+46 — latent clean-port bug sweep (agent 7cc2edcc, committed 6456497)

Instruction-level audit vs generated (delay slots accounted). Real bugs
fixed, verified faithful:
- ani_fixup (0x80181DB8): was flag-only stub -> full faithful port
  (relocates cat+0xc by ctrl+0x30, once-only 5-word table relocate gated
  by ctrl+0xb, descriptor fills, variant tail ==1->9 / &0x40->variant).
  Also added missing ctrl+0xb=1 on class-1 path (guest sets it on BOTH
  call sites) — else relocation loop re-ran every entry.
- action_select (bind.c): was no-op stub -> faithful action_switch_guest
  via motion obj ctrl+0x38.
- ev17_approach (ev.c:938): dist was sx16-truncated; guest does
  sx12(sx16+sx16) via <<20>>20 — fixed sign-extension.
- ev16_atkwin_init, ev5_action_select: verified CORRECT, no change.
- Removed debug fprintf [APL]/[ASW] + stdio.h.

FLAGGED for later: reaction_load/trace_op still no-op (dead ev_step
path); whole ev_step + ev0-ev21 handlers are dead code superseded by
resolve.c ev_stream_dispatch -> PTR_FUN_801b4e68 -> ev5_action_select.
Consider deleting/annotating dead ev path in a cleanup pass.

ani_fixup/action_select were .ANI/action-path stubs — plausibly
contributed to versus .ANI/stream corruption. Worth re-testing versus
after the ph-flicker + CD-coroutine fixes land.

### N+47 — versus->fight is a DETERMINISTIC ev_stream crash; arcade->fight WORKS

Live-instance triage (ports queried + lldb bt) resolves the "nondeterministic
wedge vs crash" framing into a crisp versus-specific defect:

- port 4520  gs=8 ph=8 mode09b=0 sub09c=0  frame ADVANCING  -> arcade/attract
  fight is ALIVE and rendering (gpu_ring_stats newest_frame advancing,
  gp0_draw=5.8M). THE FIGHT ENGINE WORKS.
- ports 4401/4404/4397  gs=8 ph=8 mode09b=2 sub09c=1  frame FROZEN(delta 0)
  -> versus->fight. lldb bt on 96999(4401) + 54507(4397): BOTH halted in
  IDENTICAL chain: battle_flow_state_machine -> fighter_frame ->
  action_commit -> ev_stream_dispatch -> br2_call_generated ->
  psx_unknown_dispatch -> psx_fatal_halt -> nanosleep.
- port 4407 attract alive; port 4405 prior halt = same ev_stream crash.

CONCLUSION: versus->fight is a REPRODUCIBLE crash (not a timing wedge) in the
versus char-select -> fight handoff. fighter_frame is invoked as
fighter_frame(&DAT_1f8000e8 / &DAT_1f8001f0 [P1/P2 scratchpad ctrl blocks],
motion). ev_stream_dispatch reads op=*pbVar1 (pbVar1=guest_at(motion+0x84)
= .ANI event-stream PC); corrupt stream PC/op -> h=*(0x801b4e68+op*4) OOB ->
br2_call_generated(garbage) -> wild dispatch (seen: 0x80122F0C rodata ASCII,
0x00120201 misaligned). Arcade binds the same fighters correctly, so the
defect is in the VERSUS-ONLY asset/anim path: select_screen_tick / versus
char-select P2 .ANI (FC_ANI) binding — consistent with N+43 heapB re-stage
stomping P2's binding when DAT_1f800001 flickers 8->1->8 and re-enters the
stage loader, leaving motion+0x84 pointing into reused sound/text data.

NEXT (faithful): instrument ev_stream_dispatch to record motion+0x84 (guest
stream PC), the op byte, and the resolved handler h on the versus crash;
then trace which versus-only stage re-load frees/reuses the .ANI region that
P2's motion+0x84 still references. Compare versus vs arcade FC_ANI bind
addresses on a live pair. Do NOT bounds-check op (would mask = stub).

### N+48 — versus P2 stream-PC ALWAYS lands >0x800d0000 (deterministic); heap layout comparison

Live comparative reads (halted versus vs live arcade/attract):

| port | mode | P1 FC_ANI | P1 streamPC(+0x84) | P2 FC_ANI | P2 streamPC(+0x84) |
|---|---|---|---|---|---|
| 4397 | versus | 0x8007121c | 0x80071712 (ok) | 0x800d1020 | 0x800d4165 (>D0000 BAD) |
| 4401 | versus | 0x8006ba04 | 0x8006bf2e (ok) | 0x800ce958 | 0x800dc99a (>D0000 BAD) |
| 4520 | arcade | 0 | 0 | 0 | 0 (alive, renders) |

heap cursors (0x801c47b8 heapA / 0x801e3cfc heapB):
- versus: heapA=0x800680dc heapB=0x800d0000/1  (staging AT the 0x800d0000 arena)
- arcade: heapA=0x800e9078 heapB=0x800dc800  (heapA grew PAST D0000, works)
- attract:heapA=0x80010000 heapB=0x800f0000

=> versus P2's .ANI/stream is allocated in heapA at ~0x800ce958-0x800d1020,
INSIDE the fixed intro_movie_state staging arena @0x800D0000. P1 lands
low (~0x8006-0x8007) always safe. The versus->fight loads BOTH fighters'
full resource sets; P2 (allocated last) crosses the 0x800d0000 boundary.
heapA is a faithful bump cursor with NO bounds check — the question is
whether ORIGINAL versus->fight heapA legitimately reaches 0x800d0000+
(then staging arena must be elsewhere / heapA reset first) or our load
over-produces by ~a few KB pushing P2 over.

heapB_cur differs per mode (arcade 0x800dc800, attract 0x800f0000,
versus 0x800d0000) — versus re-stages at the LOWEST arena, maximising
overlap with heapA's high allocations.

Agents 03b98d5b (heapA) + 34845d78 (ph-writer) have this data; the
faithful fix is the heapA-boundary / staging-arena / load-order question.

### N+49 — versus crash MECHANISM pinned: P2 stream byte 0xdf -> OOB handler table -> wild dispatch

Corroborates N+48 (parallel heap-layout analysis) with the exact fault chain,
read live on halted versus :4401 (motion @0x801F5440, ram_base 0x114210008):

- P2 motion+0x84 streamPC = 0x800dc99a -> first byte 0xdf (op=223 >=24)
  -> ev_stream_dispatch reads h=*(0x801b4e68 + 223*4)=*(0x801b51e4), ~0x37C
  bytes past the 48-entry handler table -> h=garbage ->
  br2_call_generated(garbage) -> psx_unknown_dispatch -> fatal halt.
- P1 motion+0x84 streamPC = 0x8006bf2e -> first byte 0x02 (valid op<24).
- Stream bytes @0x800dc980: `45 00 11 00 f0 51 65 ce 0a 33 f0 22 ae 4d 22
  0e 25 00 c1 da 51 d1 0e e1 0d 42 df fd f1 11 ...` — packed/high-byte data,
  NOT an .ANI event stream (cf. P1's `02 ff 01 01 05 00 00 00 02 ff 01 00`).

So the versus P2 stream PC lands in heapA's high arena (>0x800d0000, the
intro_movie_state staging region per N+48) where the bytes are sound/packed
data, not the .ANI event stream -> first opcode byte is garbage -> OOB
dispatch. The faithful fix is upstream (heapA boundary / staging-arena /
load-order, owned by agents 03b98d5b+34845d78): get P2's real .ANI stream
into a region the stream PC actually references. Do NOT bounds-check op —
that masks the corruption (stub).

### N+49 — versus→fight ROOT CAUSE CONFIRMED: heapA overflows into heapB staging arena, stomps P2 .ANI S0 table

Live RAM on halted versus :4401 (deterministic crash):
- P2 motion @0x801f5440: MO_SEC0(+0x00)=0x800d6e08, MO_STREAM(+0x84)=0x800dc99a.
- P2 S0 table @0x800d6e08 = GARBAGE `caef e03e feab ccfe...` — NOT u16
  action->stream offsets. P1 S0 @0x800738d4 = clean `049c 04a6 054c...`.
- P2 FC_ANI @0x800ce958 header IS valid (.ANI section offsets) — the .ANI
  loaded; its S0 TABLE region got overwritten.
- stream offset read from garbage S0 = 0xE042 > .ANI size 0xBD74 ->
  stream=0x800dc99a lands ~0x22CE PAST .ANI end -> packed/sound data ->
  op=0xdf=223 OOB -> wild dispatch.

=> heapA grew to ~0x800d6e08 during versus->fight resource load, i.e.
PAST the heapB staging base 0x800d0000 (faithful constant, generated
0x800D<<16). P2's .ANI S0 table landed inside the staging kill-zone and
was overwritten by staged file-0x14/sound data. heapA is a faithful bump
cursor with NO bounds check — so on HW heapA must NOT reach 0x800d0000+
during versus->fight. Our load over-produces (extra heapA allocation)
OR heapA isn't reset/freed where the original does.

heapB base 0x800d0000 is FAITHFUL (generated uses it) — NOT the bug.
The bug is heapA over-production pushing P2's S0 table over the boundary.
heapA agent 03b98d5b has the trajectory; this is the concrete proof.

### N+50 — COMPLETE causal chain proven: ph-flicker -> heapB reset -> .ANI overwrite -> ev_stream crash

Synthesis of N+43..N+49 with the decisive heap-cursor evidence:

versus_crash :4401  heapA=0x800680dc heapB=0x800d0000 P2motion=0x801f5440
              P2 streamPC(+0x84)=0x800dc99a  (first byte 0xdf -> OOB -> halt)
arcade_live  :4520  heapA=0x800e9078 heapB=0x800dc800 (works, frame advancing)

DECISIVE: heapB cursor = 0x800d0000 = the arena BASE (i.e. heapB was RESET to
base / is re-staging from empty), yet P2's .ANI stream sits at 0x800dc99a —
ABOVE the cursor, inside the region heapB is actively re-staging. A bump
allocator never places a live allocation above its own top; the only way
.ANI@0x800dc99a coexists with heapB=0x800d0000 is that heapB was reset AFTER
the .ANI was bound, and the re-stage wrote sound/intro data over it.

FULL CHAIN (all live-verified):
  DAT_1f800001 flicker 8->1->8 (versus-only; arcade stable)
    -> select_screen_tick / intro_movie_state re-entered
      -> heapB (_DAT_801e3cfc) reset to 0x800d0000, re-stages intro/sound
        -> overwrites P2's already-bound .ANI event stream @0x800dc99a
          -> motion+0x84 still points there; first byte now 0xdf (was .ANI op)
            -> ev_stream_dispatch op=223 -> h=*(0x801b4e68+223*4) OOB
              -> br2_call_generated(garbage) -> psx_unknown_dispatch -> halt

Fresh-build repro :4600 (current tree) crashed even earlier — signal_11
SIGSEGV during vs_init (ph 0xf1->0) — versus->fight has MULTIPLE crash
manifestations (ev_stream halt, SIGSEGV, SIGTRAP, CD wedge) all downstream of
the same versus-only init/heap corruption. Arcade->fight is unaffected.

FIX (faithful, owned by ph-writer agent 34845d78 + heapA agent 03b98d5b):
stop the DAT_1f800001 8->1->8 re-entry so intro_movie_state/heapB staging
runs ONCE before fighter .ANI bind; OR make heapB re-stage not clobber a
bound .ANI region. Do NOT bounds-check ev_stream_dispatch op (masks = stub).

### N+50 — versus→fight heap trajectory (vs_trace.jsonl): intro_movie_state re-runs at gs=8 ph=0, drops heapB to 0x800d0000

Time-series heapA/heapB cursor + gs/ph for versus→fight:
  t=1.33 gs=5 ph=0  heapA=0x80010000 heapB=0x800dc800   (match/attract reset)
  t=2.17 gs=5       heapA=0x8002e440 heapB=0x800dc800   (resource load grows heapA)
  t=3.21 gs=5 ph=2 sub=1 b09c=1                          (versus flag set)
  t=3.35 gs=8 ph=241 heapA=0x8004c880 heapB=0x800dc800   (-> battle_flow)
  t=3.90 gs=8 ph=0  heapA=0x800680dc heapB=0x800d0000   *** heapB -> 0x800d0000 ***
  t=4.86 gs=8 ph=8  heapA=0x800680dc heapB=0x800d0001   (fight; heapB stays ~0x800d0000)

=> versus->fight ENTERS gs=8 (battle_flow/select_screen_tick) and at ph=0
re-runs intro_movie_state -> _DAT_801e3cfc(heapB)=0x800d0000 (game.c:1832).
heapB then STAYS ~0x800d0000 through fight (ph=8). menu_res_load_sm's
heapB=0x800f0000 (save.c:4150) either ran earlier or its value was
overwritten by the intro_movie re-run.

heapB base writers (clean tree):
  game.c:1832  intro_movie_state        -> 0x800d0000   <-- versus fight value
  game.c:866   (-0x7fff0000 = 0x80010000)
  menu.c:2593  attract_stage_load_step  -> 0x800dc800   <-- arcade/match value
  save.c:*     menu_res_load_sm/save    -> 0x800f0000

arcade->fight works with heapB=0x800dc800 (attract_stage_load_step /
match_loop path). versus->fight is left at intro_movie's LOWEST arena
0x800d0000, which sits squarely in heapA's growth path -> P2 .ANI S0
@0x800d6e08 lands inside the staged file-0x14/sound region -> stomped.

OPEN (faithful question): on HW does versus->fight (a) run intro_movie's
0x800d0000 staging and still place P2 .ANI elsewhere (heapA bounded /
different alloc), or (b) reach the fight via a path that resets heapB to
0x800dc800/0x800f0000 like arcade? The versus ph-writer agent + heapA
trajectory agent are resolving which.

### N+51 — PRECISE mechanism: intro_movie_state re-loads file 0x14 -> 0x800D0000 AFTER fight .ANI bind, stomping P2 .ANI

decomp_snap file_load trace (ra = cd_file_load_sm call-site return addr):
  ra=0x801265F4 = fighter/resource loader (menu_res_load_sm/save_state_machine_alt) — 62 calls
  ra=0x80126594 = staging loader — 24 calls
  ra=0x80126664 = intro_movie_state (game.c) — 8 calls; loads file 0x14/0x15/0x16 + the -1 reset

Last versus->fight cycle ordering (seq):
  5305  file 0x20 -> 0x800D2F70   (heapA .ANI/resource, ra=0x801265F4)
  5318  file 0x24 -> 0x800DEAC8   (heapA .ANI/resource, ra=0x801265F4)
  5321  file 0x8A -> 0x80010000   (heapA reset via ra=0x80126594)
  5322  file 0x90 -> 0x800DC800   (staging)
  5329  file 0x96 -> 0x800DC800   (staging)
  5336  file -1  -> reset         (cd_file_load_sm(-1,0), ra=0x80126664 = intro_movie_state:1827)
  5337  file 0x14-> 0x800D0000    (intro_movie_state:1835, heapB=0x800d0000 :1832)
  -> then 0xb stream_block_decode grow heapA from 0x80010000 (game.c:1839-1841)

file 0x14 is loaded to 0x800D0000 TWICE in the trace: seq=107 (first
intro) and seq=5337 (re-run). The re-run happens AFTER the fight .ANI
resources were bound to high heapA (0x800D2F70/0x800DEAC8). intro_movie_state
hardcodes heapB=0x800d0000 (game.c:1832) and heapA=0x80010000 (:1831), then
decodes the cinematic into heapA. The file-0x14 staged blob + decoded
stream data occupy 0x800D0000+, overlapping P2's .ANI S0 @0x800D6E08.

=> versus->fight interleaves [fight .ANI bind to high heapA] with
[intro_movie_state cinematic staging at fixed 0x800D0000]. On HW the two
must be sequenced so they don't collide — either the cinematic fully
completes before fight .ANI bind, or heapA .ANI is placed below 0x800D0000.
The versus ph-writer agent (34845d78) is naming the DAT_1f800001 writer that
drives this interleave; heapA agent (03b98d5b) has the over-production side.

### N+52 — versus ph-flicker ROOT: phase-machine re-entry loop; fight never advances past b92==12

Agent 34845d78 live capture (port 4525, versus 09c=1) + atos host-RIP:
- DAT_1f800001(ph) cycles 6->7->8->6->7->8... gs stays 8 (no 8<->5 flicker).
- Writer of ->6: side_phase_commit game/menu.c:7500 MW8(s0+1,6), called from
  post_battle_dispatch case 12 (menu.c:6974/6983). ->7/->8: battle_flow
  menu.c:733/737. ALL writes are faithful clean-C (generated-confirmed).
- Mechanism: post_battle_dispatch switches on b92=MB8(s1+0x92) (byte
  0x1f800092). In case 12, once s1+0x8c counter hits 0x5a it calls
  side_phase_commit + resets counter. side_phase_commit writes ph=6 whenever
  MB8(s0+0x9b)<4; versus 09b=2 keeps gate open -> refires every ~90 ticks.
- Arcade (09c=0): battle_frame path, 09b differs -> stable ph=8.
- Versus (09c=1): select_screen_tick orchestrates two-side dispatch; the
  versus-only 09b=2 state keeps the commit gate open -> infinite 6/7/8 loop.
- This re-entry re-runs intro_movie_state/resource staging (N+51) -> heapB
  0x800d0000 re-stage stomps high-heapA .ANI -> ev_stream wild dispatch.
  ALSO manifests as pc=0/epc=0x8019A648 exception storm (~frame 1516).

ROOT (faithful fix target, upstream of phase byte): the versus battle/round
never advances s1[0x92] past 12 — round timer / KO / side_phase_advance is
stuck in the versus path. Likely a ported-function divergence in
battle.c/fight.c combat-or-round progression under select_screen_tick's
two-side versus dispatch, vs arcade's battle_frame path.

LIVE REPRO: port 4520 alive+stable gs=8/ph=8 versus (09c=1), gwatch armed;
log /tmp/br2_4520.log. guest_watch.c instrumentation kept (observability).

### N+53 — agent roster + committed faithful fixes; CD coroutine agent respawned

Committed this session:
- 445b1a0 fix(menu): heap_record_emit faithful record layout (u16 zeros
  +0xc/+0xe, drop extra +0 store) — matches generated 0x8016CC5C.
- battle.c guest_of(param_2/param_3) host->guest ptr fixes (uncommitted,
  agent 8a8f7d6b in flight) — build clean.

Agent 34845d78 (versus ph-writer) DONE: phase re-entry loop, b92 stuck@12.
Agent 03b98d5b (heapA) DONE: heap_record_emit fix + confirmed versus still
blocks on CD deferred-IRQ deadlock (EvCB 0xF0000009/0x20 fn=0, CD Pause
pend, cd_ctx_restore->cd_intc_dispatch->DeliverEvent not firing).
Agent c112f7fb (CD coroutine) returned EMPTY — respawned as afcfd8bf with
full evidence.

Active agents (disjoint scope):
- 78d6700f  versus round-progression: what advances s1[0x92] past 12
            (battle.c/fight.c; may NOT edit menu.c/cd.c).
- 8a8f7d6b  host-ptr leak: guest_of fixes in battle.c camera/dual_target.
- afcfd8bf  CD deferred-IRQ coroutine resume (cd.c + runtime IRQ dispatch).

Convergent hypothesis: CD deferred-event delivery (DeliverEvent
0xF0000009/0x20) is the force multiplier — if file-load completes, versus
round resources finish -> round progresses -> b92 advances -> no phase
loop -> no intro_movie re-stage -> no .ANI stomp. The phase-loop and the
CD stall likely share this root.

### N+54 — host-ptr leak FIXED (family-b crash removed); battle.c guest_of sweep

Agent 8a8f7d6b proved + fixed the host-pointer leak:
- battle.c:3255 fighter_state_tick passed (int)(uintptr_t)param_1 (host
  ptr) to fighter_script_fsm which treats it as a GUEST addr -> guest $a0
  = host-ptr low32 0x1183C1F0 -> generated opcode handler derefs as guest
  struct -> wild jr/jalr -> DISPATCH FATAL misaligned 0x00120201.
- Fixed 6 sites: fighter_script_fsm arg, dual_target_track[_b],
  camera_track_shot/step, gte_xform_add_off, tween_track_alloc (br2_gaddr).
- Committed d9cd287. Build clean.
- Versus lldb run reached FIGHT REACHED (gs=8 ph=8); then hit the separate
  pc=0 exception-storm blocker. Non-lldb versus runs SIGSEGV at versus init
  (gs=8 ph=0) intermittently — possible separate host-side crash, follow-up.

versus->fight crash families status:
  (b) host-ptr leak -> wild dispatch      FIXED d9cd287
  (a) .ANI stream-PC corruption           open (phase-loop symptom, N+51/52)
  CD deferred-IRQ wedge                  open (agent afcfd8bf)
  phase re-entry loop b92 stuck@12        open (agent 78d6700f)

### N+55 — post-host-ptr-fix versus->fight crash = pc=0 jump inside CD/vsync path (ra=intro_movie cd_file_load_sm)

Fresh versus repro (port 4601, build w/ d9cd287 host-ptr fix + 445b1a0
heap_record_emit) still crashes, but signature CHANGED — no longer the
.ANI wild-dispatch / host-ptr misaligned target. Now:

freeze psx_freeze_dump_..._1794856792: wedge_kind=fatal,
exception_entries=21524 (exception storm), cpu.pc=0x00000000,
epc=0x8019A648, ra=0x80126664, a0=0xFFFFFFFF, a1=0x1F801110 (CDROM reg),
s1=0x1F800000, sp=0x801FFFC0.

- epc 0x8019A648 is inside vsync_wait_n (renames 0x8019a5d8): reads a
  counter at 0x801BDDA0, compares vs a0. Called within the CD file-load /
  frame-present path.
- ra 0x80126664 = intro_movie_state's cd_file_load_sm call site (game.c
  :1827/:1835 region) — SAME ra as the file-0x14->0x800D0000 staging load.
- a1=0x1F801110 = CDROM register -> CD-command path.
- => the versus->fight now dies by jumping to pc=0 somewhere in the
  intro_movie cd_file_load_sm -> CD/vsync/deferred-coroutine chain.
  Consistent with the CD deferred-IRQ resume gap (agent afcfd8bf): a null
  resume-PC / uninitialized handler in the deferred dispatch.

This is likely the SAME root as the CD wedge — versus->fight re-runs
intro_movie_state (phase loop N+52) which issues cd_file_load_sm; if the
deferred coroutine/context is corrupt or a dispatch entry is null -> pc=0.

Arcade regression: nav_arcade 4600 -> FIGHT REACHED gs=8 ph=8 stable.
No regression from committed fixes.

### N+56 — versus->fight crash detail: per-frame exception storm inside vsync_wait_n region

restore_trace tail (every frame 1517-1524 identical pattern):
  rfe_resume  -> 0x8019A648 / 0x8019A600   (vsync_wait_n body, func 0x29CC)
                 ra=0x8019A53C sp=0x801FFF78 in_exc=1
  restore_escape/resume -> 0x8019A87C       (IRQ/exception handler, func 0x2970)
                 sp=0x801BDD30 epc=0x8019A648/0x8019A600 istat=0x4/0x1

=> an IRQ/exception fires during vsync_wait_n EVERY frame; handler
0x8019A87C runs; rfe resumes into vsync_wait_n; re-faults. exception_entries
=21524 over 1524 frames (~14/frame), exception_reentry_blocks=4950 (nested
re-entry = abnormal). Ends pc=0 (jump to null) -> fatal.

istat toggles 0x00000004 / 0x00000001; i_mask=0x0000000D. The stuck IRQ is
the prime suspect — a level-triggered device IRQ (CD? VBLANK? DMA ch?) not
being acknowledged/cleared, OR the deferred-coroutine resume corrupting
state so the handler re-faults. Directly relevant to afcfd8bf (CD deferred
IRQ) — the versus->fight intro_movie cd_file_load_sm path is where it
manifests (ra=0x80126664).

vsync_wait_n @0x8019a5d8 reads counter @0x801BDDA0; epc alternates
0x8019A648/0x8019A600 = the poll/compare block.

### N+57 — versus->fight IRQ storm quantified: 21524 IRQ deliveries, only ~2481 are VBLANK+CDROM

freeze ..._1794856792 device_state:
  irq_counts: vb_raise=1524 vb_deliver=1354 vb_ack=1357  (VBLANK ~1/frame OK)
              cdrom_deliver=1127                        (CDROM IRQs)
              irq_deliver=21524  == exception_entries   (TOTAL hardware IRQ dispatches)
  => ~19000 IRQ deliveries are NOT VBLANK/CDROM. Either a stuck i_stat
     source re-presented at every psx_check_interrupts boundary, or many
     unaccounted device IRQs.
  exception_reentry_blocks=4950  => NESTED exceptions (IRQ taken while
     already in handler) = abnormal; implies SR IRQ-disable / i_mask not
     respected during handling, OR a level-triggered source stays asserted.

irqctx tail: every IRQ 'take' has epc/xpc=0x8019A648|0x8019A600 (inside
vsync_wait_n poll loop), ra=0x8019A53C, rsn=1 st=1 rst=1. v0x varies
(0x3063/0x1/0x7E4F...) -> not a single constant vector.

intc_state[14]=0x8019A87C = the CD/INTC dispatch handler (cd region);
[15]=0x801BDD30 kernel stack.

ev_agg.deliver (BIOS DeliverEvent): only ~65 total — the storm is NOT
DeliverEvent-driven; it's raw INTC re-delivery.

cdrom.pend_cmd=0x09 (Pause) pending, pend_ph=1; cd_read buffer
bcb84=0x800D0000 (file data -> staging arena). CD file-0x14 load active.

=> faithful-core IRQ hypothesis: a device IRQ source asserts i_stat but is
never edge-latched/acked, so psx_check_interrupts re-delivers it at every
block boundary -> re-entrant exception storm starves vsync_wait_n -> pc=0.
Same family as the cdrom.c latch comments (unacked CD INT = unbounded
re-entry storm). afcfd8bf owns the runtime IRQ-dispatch investigation.

### N+58 — versus->fight CD activity is PROGRESSING file loads, not a stuck re-read

cd_cmds tail: repeating seek(0x02)->?(0x06)->read(0x09) cycles to DIFFERENT
MSF positions each iteration (01:38:19 / 00:02:3D / 01:3A:41 ...) — the
loader advances through a file sequence, not re-reading one file. ~100+
loads over 1524 frames -> the 21524 IRQs are largely legitimate per-file
seek/read IRQs, amplified by the phase loop re-running intro_movie.

cd_read buffer bcb84=0x800D0000 (file data -> heapB staging arena, the
same region that stomps .ANI). pend_cmd=0x09 Pause pending.

=> the IRQ storm is the SYMPTOM of repeated resource loading driven by the
phase re-entry loop (b92 stuck@12 -> intro_movie re-run -> file-0x14 +
resource reload each cycle). Primary fix target remains round/match
progression (78d6700f); CD IRQ fidelity (afcfd8bf) is the secondary
correctness thread. If round progression is faithful and the loop is
legitimate multi-round play, then the crash is a resource/heap collision
under sustained load — revisit N+49 heapA boundary.

### N+59 — refinement: inner b92 is NOT stuck@12 — confirm_bookkeeping resets it to 0 each commit; the loop is round-cycling without match-end

confirm_bookkeeping (menu.c:3842) writes *(u16*)(s0+0x92)=ph where ph is
0x100/0x200/0x300/0x400 -> LOW BYTE ALWAYS 0. side_phase_commit calls it
when 09b<4 (versus 09b=2 -> runs). So after case12->side_phase_commit->
confirm_bookkeeping, inner b92 low byte resets to 0 -> next dispatch sees
b92=0 -> case0 -> NEW round. battle_state_init does NOT touch +0x92 but
confirm_bookkeeping does.

=> the 6->7->8->(fight)->12->6 cycle is NORMAL per-round cycling, NOT a
stuck phase. The real defect: the match never ENDS — the round-win counter
t0+0xa4 (side_phase_advance, saturates 0xffff/-1 at threshold a0b=
MB8(MB32(t0+0xc0)+0x10)) never reaches match-end, so rounds repeat forever.
Likely cause: versus combat isn't actually progressing (fighters not
fighting / no KO / round timer not resolving) -> no winner -> counter never
saturates. Connects to resource-load: if fight .ANI/combat data is corrupt
(.ANI stomp N+49) or missing (CD load), combat can't run -> instant/empty
rounds -> infinite loop -> eventually IRQ/heap crash.

78d6700f tracing win-counter/match-end; afcfd8bf on CD-IRQ. The unifying
suspect remains: versus fight resources/combat state not fully initialized
-> rounds can't resolve -> phase loop.

### N+60 — live versus repro confirms acute wedge = IRQ storm at ph=8 (fighter state frozen, debug starved)

Fresh nav_to_fight.py run on :4700 reached gs=8 ph=8 (FIGHT REACHED) at
frame ~4323 then wedged: process pinned 100% CPU, debug server stops
answering read_ram, fighter base structs (0x1f8000e8/0x1f8001f0) byte-
identical across samples. Matches N+55/N+56: per-frame IRQ storm inside
vsync_wait_n/CD deferred path -> guest spins, all progress frozen, eventual
pc=0. The ACUTE versus->fight blocker is the IRQ storm (afcfd8bf scope);
the round-cycling-without-match-end (78d6700f scope) is the underlying
logic defect visible only if the storm is cleared. Both must be fixed for
a stable versus fight.

### N+61 — IRQ-storm source narrowed to DMA bit3 (not CDROM): ~19000 non-VB/non-CDROM deliveries, i_mask=0xD

Freeze-dump forensics (psx_freeze_dump_1794856940):
- exception_entries=21530, reentry_blocks=4950, i_mask=0xD (bits 0=VBLANK,
  2=CDROM, 3=DMA).
- ev_agg.deliver (BIOS DeliverEvent) totals only ~65: 0xF0000003/0x20 x34,
  /0x10 x13, /0x40 x11, 0xF0000009/0x20 (CDROM) x3, 0xF4000001/0x4 x4.
- => the 21530 IRQ-line TAKES almost never reach a BIOS DeliverEvent. The
  INTC line stays asserted and the exception handler RFE's without servicing
  the source -> immediate re-take.
- vb_deliver~1354 + cdrom_deliver~1127 account for ~2481; the remaining
  ~19000 deliveries are bit3 DMA (only other enabled source in 0xD).
- cdrom.irq_fl=0x00 at freeze (HINTSTS cleared) and irqctx epc pinned at
  0x8019A600/0x8019A648 (vsync_wait_n). CDROM is NOT the re-firing source.

DMA bit3 is raised EDGE-triggered on DICR master-flag 0->1
(dma.c raise_dma_irq_on_master_edge, detail=(dicr>>24)&0x7f). For ~19000
deliveries the DICR per-channel done-flags must be re-setting repeatedly:
either (a) the guest CD-file-load DMA (ch3) completes per-sector and the
handler never clears the channel done-flag so each new completion re-raises
the master edge, or (b) a channel is left enabled+complete so bit31 toggles.
NEXT: dump DICR channel-done flags (bits24-30) + which channel completes in
the storm window; check whether the game's DMA-done ISR acks DICR (writes
channel flag clear) or only does a generic I_STAT ack. If the ISR path is
delegated/starved (cd_file_load_sm continuation), the done-flags never clear
-> faithful-core DMA IRQ storm. This is the acute versus->fight wedge.

### N+62 — DMA IRQ storm is a SYMPTOM of CD file-load retry loop, not a DICR-ack bug

DICR model (dma.c): per-channel done-flags bits24-30 are write-1-to-ack
(DICR write @0x1F8010F4, dma.c:1252-1266); master flag bit31 = OR of armed
done-flags; i_stat bit3 raised EDGE-triggered on master 0->1 only
(raise_dma_irq_on_master_edge). ~19000 bit3 deliveries REQUIRE the master
flag to toggle 0->1 ~19000 times => the done-flags ARE being cleared each
cycle (the game's DMA-done ISR RUNS and write-1-acks DICR), then a NEW ch3
CD-DMA completes and re-sets them.

=> NOT a missing-DICR-ack bug. The ISR path works. The storm is the CD
file-load coroutine issuing ~19000 ch3 DMA reads because the deferred
completion event (DeliverEvent 0xF0000009/0x20, only x3 delivered) never
arrives -> load never finishes -> it retries the sector DMA forever ->
DMA IRQ per retry. THE FIX IS THE CD DEFERRED-IRQ/DeliverEvent RESUME
(afcfd8bf scope: cd_ctx_restore->cd_chan_full_init->cd_intc_dispatch->
DeliverEvent). Clearing that unblocks the load AND stops the DMA storm.

### N+63 — CORRECTION to N+62: DMA storm channels are ch2(GPU)+ch6, NOT confirmed ch3; CDROM IRQ actually fires 1713x

Re-examined freeze dump (1794856940) dma_trace.tail + irq_counts:
- dma_trace completions observed: ch=2 (GPU, tot=65536w, pc=0x80192EBC,
  madr=0x801E620C) and ch=6 (tot=2080w, pc=0x801926A8, madr=0x801E618C).
  These are RENDERING DMAs (GPU VRAM + ordering-table), NOT ch3 CD-DMA.
  The 24-entry tail may not represent the whole storm — per-channel IRQ
  detail (psx_irq_raise bit3 detail=(dicr>>24)&0x7f) still needed.
- irq_counts: irq_deliver=22459, vb_raise=1871/vb_deliver=1701/vb_ack=1704,
  cdrom_deliver=1713. So CDROM i_stat bit2 DOES fire ~1713x (~1.1/frame) —
  the CDROM IRQ path is ACTIVE, not dead.
- ev_agg.deliver 0xF0000009/0x20 = only x3 is a DIFFERENT metric (BIOS
  DeliverEvent for that one event class) vs cdrom_deliver=1713 (raw CDROM
  IRQ-line takes). CDROM IRQs fire but rarely produce the file-complete
  event the load waits on.

=> Revised: the ~19000 non-VB/non-CDROM IRQs are DMA bit3 from RENDERING
channels (ch2 GPU + ch6) repeating during the intro_movie/cd_file_load
path — likely movie/graphics DMA retried because a frame/vsync or CD
completion event starves. The CD deferred-coroutine DeliverEvent gap
(afcfd8bf) remains the leading root but the proximate IRQ spammer is
rendering DMA, not CD sector DMA. Needs per-channel irq detail + which
DMA channel's done-flag toggles in the storm window.

### N+64 — CORRECTION (major): NOT an IRQ storm — it's a logic busy-loop taking NORMAL IRQs; DMA ruled out

event_ring.json per-source IRQ_DELIVER tally (istat bit -> count):
  bit0 VBLANK=1869, bit2 CDROM=1497, bit7 SIO=972 (all early menu/attract
  cycles 286-301M, NOT fight), bit3 DMA=60 only.
dma_chan_hist (freeze 1794857902): ch2=1091, ch3=1336, ch6=604 DMA dones
  total ~3036 — far below irq_deliver=22459, and bit3 delivered only 60x.

=> My N+61/N+62/N+63 "DMA bit3 storm" theory is WRONG. There is no abnormal
IRQ source re-firing. The ~22459 deliveries are the NORMAL VBLANK+CDROM
mix accumulated because the guest is stuck in a BUSY LOOP: cd_file_load_sm /
vsync_wait_n spins thousands of iterations, each taking ordinary VBLANK+CDROM
IRQs. The wedge is a LOGIC loop (deferred completion event never arrives ->
load state machine never advances -> spins), not an interrupt-controller bug.

REAL QUESTION (afcfd8bf): CDROM IRQ fires ~1497x but BIOS DeliverEvent
0xF0000009/0x20 only x3 — the CDROM ISR runs yet the file-complete event
isn't signaled. Trace cd_intc_dispatch handler-table dispatch: which of the
11 handlers should fire on the completion HINTSTS reason, and why it doesn't
DeliverEvent. Suspect: the completion reason's handler isn't installed, or
cd_ctx_restore resumes to the wrong continuation, or the load waits on a
different event class than what's delivered.

### N+65 — DeliverEvent source pinpointed: cd_dma_chan_reset @0x8019b7e8

The ONLY path that fires BIOS DeliverEvent(0xF0000009,0x20) is
cd_dma_chan_reset (game/cd.c:5008, guest 0x8019b7e8): it polls CDMMIO
status (blk+0x1aa off 0xd5) bits 0x30 until clear (cap 0xf01), then either
call_guest(DB32G(0x801bdee8),0xf0000000,0) if a callback is installed, else
cd_bios_svc_07(0xf0000009,0x20). No direct C caller — reached via guest
dispatch (DMA-done/channel-reset handler table). If this fn isn't invoked
on ch3 CD-DMA completion, or DB32G(0x801bdee8) holds a stale/wrong callback,
DeliverEvent starves -> cd_file_load_sm spins -> versus wedge. Spawned
focused agent 22f3537a to trace which of (a-e) breaks the link.
Arcade->fight re-verified stable (gs=8 ph=8, frames advancing, no storm).

### N+66 — wedge CD-command pattern: phase-loop drives REPEATED multi-file loads (not one stuck file)

cd_cmds.tail (freeze 1794858054): repeating groups at pc=0x80196FC4
(fn 0x29CC/0x2970): Pause(0x09) -> 0x0E -> 0x02 -> ReadN(0x06), with 'seek'
MSF changing each group (02:03:37, 01:38:19, 00:02:3D, 01:3A:41). The game
is NOT stuck on one file — it cycles through MANY different file loads.
=> the versus phase-loop (round-cycling, N+59) re-triggers the resource-
load sequence each cycle; each load issues real CD reads+DMA (ch3=1336
dones, seeks work) but DeliverEvent(0xF0000009/0x20) never signals
completion -> load never cleanly finishes -> loop repeats -> wedge.
Confirms: CD hardware+DMA+seek all WORK; the single missing link is the
completion-event delivery (cd_dma_chan_reset -> DeliverEvent). Both the
phase-loop AND the DeliverEvent gap must be addressed; fixing DeliverEvent
likely lets each load complete -> resource manifest finishes -> phase
advances -> fight stabilizes.

### N+67 — save.c stub audit: guard_callee_* weak stubs -> generated delegation (build+runtime verified)

Audited game/save.c (~9.3k LOC memcard/save/LZSS) for `return 0/1` guesses
and weak stubs. Findings:

- ret_zero_a/b/c @0x80163850/98/a0 — VERIFIED FAITHFUL, left as-is: each
  guest body is literally `jr ra; move v0,zero` (generated
  br2eu_func_80163850/98/A0, 8-byte single block). Return-0 is the real
  semantics, not a stub.
- ret_zero_f @0x8019e3d4 — looked like a stub but is ALREADY OWNED by
  cd.c:3921 (jals 0x8019e420/0x8019e600 -> B0-svc bridges). save.c's weak
  `return 0` def was dead code shadowed by cd.c's strong def; removed it,
  kept an extern decl for cd_tail_call_a. No behavior change.
- memcard_block_verify / job_result_poll_stable weak stubs — shadowed by
  real defs in menu.c:13182/13207. Left (weak fallback only).
- guard_callee_a0/a1/b0/b1/c0/c1/d0/d1 @0x801a5bd0/6538/5d90/66dc/5f50/
  6350/6150/6880 — REAL BUG FIXED: these were `__attribute__((weak))
  int f(){return 0;}` stubs, and no strong def existed anywhere, so the
  guarded-dispatch family (guard_dispatch_a-d @0x801a86bc/8744/87cc/8854)
  silently dropped every callee invocation. Guest bodies are 0x1a4-0x200
  bytes — real functions. Replaced with br2_call_generated delegations.
  Verified arg setup insn-by-insn against generated 0x801A86BC/0x801A8854:
  a0=*(u32*)0x801bef24 (+v1*4 on index path), a1=*(u32*)0x801bef00,
  a2=object guest addr (macro now passes guest_of(host_ptr) instead of
  the host pointer — the exact host-ptr-where-guest-addr-expected bug
  class), a3=caller passthrough $a3 (v1==0 path, read live via
  clean_cpu_state()->gpr[7]) or sign-extended v1 (v1!=0 path).

Proof: `cmake --build . -j8` clean; headless run boots identically to
baseline — frame 1871 then the SAME pre-existing FATAL
unknown-dispatch 0x0B808000 @ battle_struct_init with AND without the
patch (verified by stash-rebuild-rerun), i.e. zero regression; the
0x0B808000 battle-init crash is a separate frontier blocker owned by the
battle/menu workstream, not save.c.

Outstanding (out of scope, flagged): ret_zero_e @0x80161630 is a
`return 0` stub defined in game/bind.c:2173 but the guest body is 624
bytes — needs the bind.c owner to port/delegate.

### N+68 — second wild-dispatch signature: P1 fighter -> 0x0B808000 (same .ANI/stream-corruption family)

psx_crash.txt now shows TWO fail-fast wild dispatches, same family:
- addr=0x0B808000 ra=0x00062409 a0=0x1F8000E8(P1) a1=0x801F5328(P1 motion)
- addr=0x80122F0C ra=0x80126664 a0=0x1F8001F0(P2) a1=0x801F5440(P2 motion)
Both jump from a fighter battle-motion struct to garbage — the corrupted
.ANI/event-stream dispatch (DeliverEvent-starved resource load leaves
stale/corrupt motion+stream pointers). P1 variant seen in a plain headless
run (attract path) at frame ~1871 inside battle_struct_init — confirms the
corruption is not versus-specific; ANY path that reaches fight with an
incomplete .ANI bind can wild-dispatch. Reinforces: fix the DeliverEvent/
resource-completion gap (22f3537a) and the stale-FC_ANI re-bind.

### N+69 — weak return-0 stub audit COMPLETE: bind.c/save.c landscape resolved

Systematic check of every `return 0`/weak stub vs strong defs:
- guard_callee_* (8): FIXED -> br2_call_generated (commit 1bc47da).
- ret_zero_e @0x80161630 (624B real fn): FIXED -> delegation (agent e91eb732,
  bind.c:2173 now br2_call_generated).
- ret_zero_a/b/c @0x80163850/98/a0: FAITHFUL (real return-0, disabled
  save-slot op table) — verified vs generated.
- ret_zero_d: NOT a stub — br2_call_generated(0x8016dadc) delegation,
  misleading name only.
- cd_seek_state_machine: bind.c weak return-0 is DEAD — cd.c:3579 strong
  def (bios_svc B0/0x11 TestEvent) wins at link. Harmless.
- bind.c weak return-0 stubs (text_*, gpu_sprite_*, bcfg_*, announce_*):
  ALL have strong defs in gfx.c/save.c/menu.c -> overridden, harmless.
- memcard_block_verify/job_result_poll_stable (save.c weak): INTENTIONAL
  deferred — menu.c strong defs take guest u32 addrs but mc_op_seq_probe
  forwards HOST buffer ptrs; documented host/guest-ptr mistranslation
  needing dedicated RE (save.c:5616 comment). Known latent, not a guess.

=> The return-0 stub surface is now clean except the two documented
deferred memcard ptr-contract items. No load-bearing guessed stubs remain
in bind.c/save.c.

### N+70 — ret_zero_e delegation verified: build clean, boot reaches known N+68 blocker

bind.c:2173 `ret_zero_e` is now `br2_call_generated(0x80161630, a0)`
(int32_t arg, int return — matches menu.c/save.c extern decls; return
value unused by callers). `python3 game/gen_clean_table.py` regenerated
src/clean_table.c (1115 wrappers, 79 skipped; 0x80161630 correctly stays
generated-authoritative since the clean body delegates). Build clean.
Headless run (`--headless --debug-port 4381`): boots LLE, reaches frame
1871 and fails at the DOCUMENTED N+68 wild-dispatch 0x0B808000 inside
battle_struct_init — pre-existing blocker, NOT a regression from this
change (the stub->delegation swap cannot produce a wild dispatch; the
crash signature is identical to the N+68 attract-path record). Menu/save
call sites now execute the real 1572-byte/74-block guest body (20+ jals
into generated callees, scratchpad +0x1F80008C halfword init, IRQ-check
block boundaries — a faithful clean port is a future RE task, delegation
is the correct interim per AGENTS.md §4).

### N+71 — ATTRACT-mode fight ALSO crashes with same wild-dispatch -> bug is UNIVERSAL to fight-entry, not versus-specific

Plain headless run (NO input -> title -> attract demo -> fight) crashes at
frame 1871 with the IDENTICAL signature: FAIL-FAST unknown dispatch
addr=0x0B808000 ra=0x00062409 a0=0x1F8000E8(P1) a1=0x801F5328(P1 motion)
inside battle_struct_init. gs byte read = 0x02.

=> The .ANI/event-stream wild-dispatch is NOT a versus-only bug. Attract
demo AND versus->fight BOTH crash; only the specific nav_arcade char/
timing path reaches a stable fight. The unified root is the
resource-completion gap: whenever a fight-entry path reaches battle init
with an .ANI/FC_ANI bind that didn't fully complete (DeliverEvent-
starved load OR stale select-preview pointer), the motion/stream dispatch
jumps to garbage. Fixing the DeliverEvent/resource-completion link
(22f3537a) + the stale-FC_ANI re-bind should fix ALL fight paths, not just
versus. This RAISES the priority of the resource-completion fix — it is
the difference between "some fights work" and "every fight works."

### N+72 — CD deferred-IRQ path VERIFIED working; clean_table guest_at regression fixed; .ANI heap-alloc is the live blocker

Two corrections landed this session:

1. **CD deferred-interrupt path is faithful end-to-end** (agent afcfd8bf,
   investigation-only, no edits). Live + generated-code trace proves the
   full chain: CDROM irq_flag -> fire_cdrom_irq -> kernel deferred
   coroutine -> cd_ctx_restore (0x8019ADCC) -> cd_chan_full_init
   (0x8019A814) -> cd_intc_dispatch (0x8019A8EC) -> clean
   cd_response_pump (cd.c:4454, byte-faithful to generated
   br2eu_func_801979F8) -> cd_dma_chan_reset -> DeliverEvent
   (0xF0000009,0x20). A versus run reached battle_round_init +
   battle_struct_init = ALL CD file loads completed. The earlier
   "DeliverEvent starvation" theory is RESOLVED for the current blocker —
   cd_response_process returns 0 correctly, EvCB fn=0 is legitimate
   MARK-mode flag-only behavior (bios_hle.c:221-256). N+64's residual
   wedge question is moot unless the TestEvent spin recurs AFTER the heap
   fix.

2. **src/clean_table.c regen regression FIXED** (commit 88ea0c4).
   gen_clean_table.py's param_kind() classifies intptr_t as scalar 'val',
   so a regen dropped guest_at() for three clean fns taking a HOST pointer
   as intptr_t param_1 (pad_state_refresh 0x8012a72c, attract_menu_dispatch
   0x8014751c, battle_flow_state_machine 0x8016fcc4) -> host deref of a
   guest address -> SIGSEGV (afcfd8bf saw fault_addr=0x801FFFD2). Added
   HOST_PTR_PARAMS override (per-addr param-index set) so the generator
   emits ((intptr_t)guest_at(cpu->gpr[N])) on every regen. intptr_t stays
   ambiguous (match_end_check/frame_tick take a GUEST addr and call
   guest_at internally -> need raw), so override is per-addr not global.
   Verified: post-fix run reaches the SAME documented 0x0B808000 heap
   fatal at frame ~1871, not the pointer SIGSEGV.

**Live blocker (unchanged, now isolated):** the .ANI/heapB stomp.
intro_movie_state (game.c:1799) resets _DAT_801e3cfc=0x800d0000 (game.c:1832)
and cd_file_load_sm(0x14)+stream_block_decode writes over P2's .ANI which
lands ~0x800d1xxx. Spawned agent f3f44605 to determine whether the .ANI
heap allocation (which cursor: heapA vs heapB/_DAT_801e3cfc) is faithful
or a clean-port bug, scoped to game/action.c, ani.c, ev.c, battle.c.
78d6700f still owns the match-end/combat-resolution root cause.

### N+73 — POST-FIX REGRESSION CHECK: player-arcade->fight STABLE+RENDERING; attract crashes; versus wedges silently

After commit 88ea0c4 (clean_table guest_at restore), ran both nav paths:

- **nav_arcade.py (player 1P arcade, port 4741): WORKS.** Reached
  gs=8 ph=8 FIGHT REACHED and HELD stable; frame advanced 4402->4806->5215,
  fn_stats direct_seen=5.83M dispatches -> fight engine live+rendering.
  This is the regression oracle — player-arcade resource path is correct.

- **plain headless / attract (port 4730):** crashes frame ~1871
  FAIL-FAST unknown dispatch 0x0B808000 in battle_struct_init (the
  documented .ANI/heapB stomp).

- **nav_to_fight.py (2P versus, port 4742):** reached vs_init gs=8 ph=0
  sub=1 sig=(2,1), then WEDGED SILENTLY — process state Ss @0% CPU,
  debug port stops answering, NO FATAL/crash in log. The [PBD] line
  (p1=0x1144a40e8 p2=0x1144a41f0 p3=0x1F800000 s1bc/f0bc=0x801f0270) is
  the known diagnostic, NOT a crash indicator (per ACCURACY_BURNDOWN
  ~3873: host ptrs into guest RAM are valid; [PBD] != crash). The versus
  wedge is now a SILENT stall (dead debug, 0% CPU) rather than the
  earlier 100%-CPU IRQ busy-loop — likely the same handoff stall reaching
  a different terminal state, OR the .ANI corruption manifesting as a
  hang before the wild-dispatch fatal.

=> Refinement of N+71 "universal": PLAYER-ARCADE works; the .ANI/heapB
stomp hits ATTRACT-demo and VERSUS (the two paths that stage resources
differently / run the phase-loop). The resource-path divergence between
working arcade and crashing attract/versus is the precise question —
f3f44605 (.ANI heap-alloc cursor) + 78d6700f (match-end/phase-loop)
own it.

intptr_t/uintptr_t first-param AUDIT (complete): all other intptr_t fns
(battle_session_arm, match_end_check, frame_tick, frame_present,
tile_decode_wait, rec_timer_pair_clear, cd_cmd_bytes, cd_cmd_retry)
do guest_at(param_1) INTERNALLY -> they take a GUEST addr -> wrapper
correctly passes raw. Only the 3 in HOST_PTR_PARAMS take a HOST ptr.
Override set is complete, not partial.

### N+74 — .ANI heap-alloc is FAITHFUL; real Q = does heapA legitimately reach ~0x800D1000 (over-production vs faithful)

Agent f3f44605 (investigation-only) traced the FC_ANI write:

- Write site: `game/save.c:7035` inside `save_state_machine_alt`
  (@0x80171890, state L30) — the per-fighter resource loader.
  `heapA=(g32(0x801C47B8)+3)&~3; cd_file_load_sm(...,heapA)` loads .ANI
  into heapA (cursor cell 0x801C47B8, base reset 0x80010000 in
  menu_res_load_sm st00 save.c:4150); `s4+0x30=heapA` stores FC_ANI.
- VERIFIED instruction-level vs generated br2eu_func_80171890
  (0x80172050 lw cursor / 0x8017205C sw s4+0x30 / cursor+=size) —
  IDENTICAL. The .ANI heapA allocation is faithful, NOT a port bug.
- Post-mortem RAM (attract crash): P1 FC_ANI=0x8007121C (safe),
  P2 FC_ANI=0x800D1020; P2 .ANI spans 0x800D1020-0x800DBDC0 — sits ABOVE
  the 0x800D0000 heapB staging base intro_movie_state resets to
  (game.c:1832). Ends just below heapB battle base 0x800DC800 — tight.
- Both .ANI blobs looked INTACT at crash time -> the 0x0B808000 wild
  dispatch may involve a DIFFERENT corrupt ptr (.ACT cursor motion+0xac,
  S5 skeleton motion+0x10, param_tbl) — worth fntrace/pc_probe capture.

=> The faithful question is now precise: SHOULD heapA top out ~0x800D1000
during versus/attract fight resource load? If HW tops lower (~0x800Cxxxx)
there is a heapA over-producer (~+100KB) in a clean port
(stream_block_decode/heap_alloc54/heap_record_emit/data_relocate/
heap_ptr_fixup sizes). If HW heapB staging uses a different base during
fight, intro_movie_state's 0x800d0000 needs phase-gating. Player-arcade
works (heapA presumably tops lower with fewer/different resources).
Spawned follow-up agent on the heap-trajectory audit.

### N+75 — attract-crash post-mortem: P1 pointers INTACT; P2 .ANI confirmed in kill-zone; dispatch table intact

Reproduced attract crash on :4744, dumped guest RAM post-FAIL-FAST:

- Crash: `unknown dispatch addr=0x0B808000 ra=0x00062409 a0=0x1F8000E8
  a1=0x801F5328` (P1 ctrl + P1 motion).
- P1 ctrl+0x30 FC_ANI=0x8007121C (safe low heapA). P1 motion 0x801F5328:
  +0x00 MO_SEC0=0x80078D10 (=.ANI+0x7AF4, relocation correct),
  +0x84/+0x88 stream PC=0x80071C52, +0xac=0x8007C28C — ALL point into
  P1's own heapA .ANI region, values plausible.
- Stream bytes @0x80071C52 = `04 05 b8 02 04 05 50 03 0a...` — valid
  .ANI event opcodes, NOT garbage.
- Handler table 0x801B4E68 entries [0..15] all valid 0x8017xxxx — table
  itself intact.
- P2 ctrl+0x30 FC_ANI=0x800D1020 (CONFIRMED in heapB 0x800D0000 staging
  kill-zone); P2 ctrl+0x10=0x800D10BC inside same region.

=> P1's .ANI/stream/dispatch-table are INTACT at crash. The wild
dispatch 0x0B808000 (ra=0x00062409, both non-guest-code values) most
likely originates from P2's event stream whose .ANI region (0x800D1020)
is the heapB-staging overlap — OR a transient stream-state/handler-return
corruption. This STRENGTHENS the heapB-stomp theory specifically for P2
while showing P1 is clean. The fight dispatches both fighters' streams;
the corrupt one is P2's. a0861f48 (heapA over-production) + 78d6700f
(match-end/phase-loop) own the fix; 8432f37e owns versus-wedge forensics.

### N+76 — ROOT CAUSE FOUND (78d6700f): CD data-DMA ch3 completion IRQ never fires -> DeliverEvent starves -> file loads spin -> fight-entry phase-loop/wild-dispatch

This SUPERSEDES the heap/.ANI-stomp framing for the PRIMARY blocker.
78d6700f traced the match/round state machine (faithful, no bug in
battle.c/fight.c/menu.c post_battle_dispatch) and found the real break:

- `TestEvent(0xF1000008)` (the OpenEvent(0xF0000009,0x20) CD completion
  handle @0x801BDE38) = **1,418,266 calls**, 0 DeliverEvent in trace tail.
- DMA ch3 (CDROM) starts+completes **5300x** but `dicr` is CONSTANT
  `0x00900000` across all 40000 dma_trace entries = only ch4 IRQ-enable
  (bit20) + master (bit23); **ch3 IRQ-enable (bit19) is NEVER set**.
- -> `channel_irq_flag_armed(3)` fails -> no DICR done-flag -> `i_stat`
  bit3 (DMA IRQ) never sets (i_stat only ever 0x4=CDROM or 0).
- `cd_intc_dispatch` mask = i_stat & i_mask(0xD) & 0xD; cause3
  (cd_cb7_run -> channel-handler table -> cd_dma_chan_reset/0x8019b7e0 ->
  DeliverEvent) requires i_stat bit3 -> **never pends**.
- cb7 channel-handler table @0x801BDDF4: `0x8019b7e0` registered only at
  INDEX 4; index 3 = NULL. CD data DMA is hardwired to **channel 3**;
  channel 4 never runs. So even a fired DMA IRQ wouldn't match ch3.

=> Real sectors+DMA complete, but the completion EVENT never delivers ->
file loads never finish -> versus/attract fight-entry either phase-loops
re-loading forever (s1[0x92] cycling) or wild-dispatches through a
corrupt motion/.ANI fn-ptr (0x0B808000 / 0x00062401 @RAM[0x801B56B8]).

TWO candidate break points (both faithful-core, not game-logic):
1. **runtime dma.c/interrupts.c**: DICR ch3 IRQ-enable (bit19) never set
   -> complete_transfer/raise_dma_irq doesn't arm ch3 done->i_stat bit3.
   Verify faithful DICR bit layout + per-channel IRQ-enable vs Beetle.
2. **game/cd.c cb7 registration**: handler 0x8019b7e0 at index 4 but CD
   DMA is ch3 -> index-3 NULL. Either registration index wrong, or CD
   completion is meant via CDROM-IRQ (bit2->cause2->response_pump) not
   DMA-IRQ — then a DIFFERENT handler should DeliverEvent.

This explains BOTH symptoms: attract wild-dispatch (corrupt .ANI from
incomplete load) AND versus silent-wedge (phase-loop re-loading).
Player-arcade survives because its lighter single-side load completes
via the working CDROM-IRQ path before the DMA-dependent stage.

Owner: 22f3537a (cd_dma_chan_reset/cd.c) + spawning runtime-DMA agent.
a0861f48 (heapA) + 8432f37e (wedge) results now secondary — the DMA-IRQ
root likely subsumes them.

### N+77 — DICR decode confirmed: runtime layout bits16-22=per-ch IRQ-enable, bit23=master, bits24-30=done-flags; game enabled ch4(bit20) NOT ch3(bit19)

Read psxrecomp/runtime/src/dma.c DICR model:
- `channel_irq_flag_armed(ch)` = bit(16+ch) [per-ch IRQ-enable] AND bit23
  [master enable]. complete_transfer sets done-flag bit(24+ch) only if
  armed, then raise_dma_irq_on_master_edge -> i_stat bit3.
- Per-channel regs at 0x1F801080+ch*0x10; CD data DMA runs on ch3
  (execute_ch3_cdrom/cdrom_async) — observed 5300 ch3 completes.
- Observed dicr=0x00900000 = bit23(master) + bit20 = **ch4 IRQ-enable**.
  ch3 IRQ-enable = bit19 (0x80000) is NOT set.

So the game writes DICR enabling channel-4 IRQ, not channel-3. Two
readings, both need the game/cd.c + oracle to disambiguate:
(a) FAITHFUL: game enables ch4 for GPU/OT DMA and CD completion is meant
    via CDROM-IRQ (bit2->cause2->cd_response_pump->reason cb->
    cd_dma_chan_reset->DeliverEvent), NOT DMA-IRQ. Then the bug is the
    CDROM-IRQ path not DeliverEvent-ing on re-loads (afcfd8bf said it
    works for initial loads; 78d6700f saw TestEvent spin on re-loads).
(b) BUG: game should enable ch3 but runtime mis-stores the DICR write,
    OR CD DMA should run on ch4 not ch3 (channel-number mismatch between
    game cb7 index-4 registration and runtime ch3 execution).

The cb7 handler table @0x801BDDF4 has 0x8019b7e0 at INDEX 4, index3=NULL
— consistent with the game treating CD-DMA-done as channel-4. If the
game's channel numbering is 1-based in that table (idx4 = ch3 0-based),
registration is fine and only the DICR-enable channel is in question.
DECISIVE next probe: what value does the guest WRITE to DICR
(0x1F8010F4), and does real HW/game enable ch3 or ch4 for CD. Agents
3aeaa95d (runtime dma.c) + 22f3537a (game/cd.c) converge here.

### N+78 — DECISIVE: game registers CD-DMA-done handler at cb7 index 4, but cmd_submit uses channel slot 3 -> channel-mapping contradiction is THE bug

Generated-code proof (br2eu_func_8019BEE0 = cd_ext_table_init):
- `li a0,4` + `jal 0x8019A6E0` -> registers handler 0x8019b7e0
  (cd_dma_chan_reset) into cb7 channel-handler table @0x801BDDF4 at
  INDEX 4 (= DMA channel 4, since cb7_run maps done-bit(24+s0)->tbl[s0]).
- Separately, `cd_chan_cmd_submit` (cd.c:3594) programs DMA via
  `base=0x1f801080+(s0<<4)` with s0=channel slot; its own DIAG marks
  `s0==3` as "ch3 (CDROM) arms" -> CD data DMA uses channel slot 3.

=> The game programs CD data DMA on channel 3 BUT registers its
completion handler + enables DICR IRQ for channel 4. On real hardware
this MUST be consistent — so one of these is a recomp/runtime divergence:

(a) The cb7 index / DICR-enable channel is NOT the DMA channel number but
a driver channel-slot that maps ch3-DMA -> slot4. If so, cd_cb7_run's
sequential tbl[s0]<->DICR-bit(24+s0) mapping is faithful and the bug is
that complete_transfer sets the WRONG done-bit (ch3->bit27 but game
expects the completion signaled as if ch4->bit28), i.e. runtime channel
numbering off-by-one vs the game's cb7/DICR view.

(b) CD data DMA genuinely runs on ch4 in this game and cmd_submit's
s0==3 path is a DIFFERENT CD op (param/command vs data). Then runtime
ch3 hardwiring (execute_ch3_cdrom/cdrom_async on channels[3]) is wrong —
should be channels[4].

DECISIVE probe for agents: which channel slot does the CD DATA-read path
(cd_file_load_sm -> sector DMA) actually pass to cd_chan_cmd_submit, and
does the runtime's channels[3] CDROM path correspond to the game's ch3
or ch4? If the game's data-DMA uses slot 4 while runtime does ch3, the
fix is runtime channel remap; if slot 3, the fix is cb7/DICR indexing.
3aeaa95d (runtime dma.c) + 22f3537a (game/cd.c) own the resolution.

### N+79 — CORRECTION to N+78: cd_ext_table_init's a0=4 goes through a trampoline, channel-index not yet proven

Generated br2eu_func_8019A6E0 (the callee cd_ext_table_init jumps to with
a0=4, a1=0x8019b7e0) is NOT the registration body — it's a 48-byte
TRAMPOLINE: `lui v0,0x801c; lw v0,-0x2260(v0)=*(0x801BDDA0); lw v0,4(v0);
jalr v0`. It loads a fn-ptr from *(*(0x801BDDA0)+4) and tail-jumps,
forwarding a0=4/a1=handler. So a0=4 is passed THROUGH to whatever driver
object 0x801BDDA0 points to — it is NOT proven to be the cb7 channel index
directly. The cb7 table @0x801BDDF4 index-4 observation (78d6700f) is the
post-registration RESULT; whether a0=4 CAUSED index-4 or the driver maps
CD->slot4 internally is unresolved.

CONFIRMED separately: all 3 generated call sites of cd_chan_cmd_submit
(0x80199674/0x80199C30/0x80199CCC) do `li a0,3` -> CD data DMA programs
DMA channel slot 3 (regs 0x1f8010B0). So CD data DMA IS channel 3.

OPEN (agents 3aeaa95d runtime + 22f3537a cd.c): trace what the
0x8019A6E0 trampoline target does with a0=4 — does it write cb7 table[4]
(ch4) for the CD-done handler (=> runtime must signal ch4-done for ch3
DMA = channel remap bug), or does the CD driver keep an internal
channel-slot abstraction where slot4==DMA-ch3 (=> cb7_run's sequential
tbl[s0]<->DICR-bit(24+s0) mapping or complete_transfer's done-bit is the
divergence)? Also whether CD completion is meant via CDROM-IRQ path
(cd_response_pump) not DMA-IRQ at all.

### N+80 — REFINEMENT: ch4/cb7 is likely a RED HERRING for CD; CD completion probably uses CDROM-IRQ not DMA-IRQ

Runtime-verified on :4745 (early CD-init snapshot):
- cb7 channel-handler table @0x801BDDF4: ONLY tbl[4]=0x8019B7E0
  (cd_dma_chan_reset); tbl[0,1,2,3,5,6,7]=NULL.
- DICR=0x00900000 = bit23(master)+bit20(ch4 IRQ-enable); ch3 IRQ-enable
  (bit19) NOT set.
- All 3 cd_chan_cmd_submit call sites pass a0=3 -> CD data DMA = ch3.

Analysis: cb7_run maps DICR done-bit(24+s0)->tbl[s0]. CD ch3 DMA-done ->
DICR bit27 -> tbl[3]=NULL. tbl[4] fires only on ch4 done (bit28), and ch4
IRQ-enable(bit20) IS set -> tbl[4] is armed for CHANNEL 4 (GPU DMAot),
NOT CD ch3. Since ch4 DMA never runs during CD loads and CD loads DO
complete (afcfd8bf reached battle_round_init), the CD completion event
is delivered via the CDROM-IRQ path (i_stat bit2 -> cd_intc_dispatch ->
cd_response_pump -> reason cb -> cd_dma_chan_reset -> DeliverEvent),
NOT via ch3 DMA-IRQ. ch3 IRQ-enable being unset is therefore FAITHFUL —
the game does not use DMA-IRQ for CD data completion.

=> 78d6700f's "ch3 IRQ-enable unset -> DeliverEvent starves" is likely a
RED HERRING for the CD-data path. The cb7 tbl[4]/DICR-ch4 is the GPU
channel-4 handler, unrelated to CD ch3. afcfd8bf's CDROM-IRQ-works
verdict stands.

REMAINING real question (back to afcfd8bf's residual + heap agents):
WHY does TestEvent(0xF1000008) spin 1.4M times during the versus/attract
FIGHT-ENTRY re-loads specifically? Initial loads complete; the
phase-loop re-loads don't. Likely: a per-command reason-callback cell
(0x801BC84C/0x801BC848) not installed for the re-load command type, OR
cd_response_process returns non-zero (status/reason mismatch), OR the
load is genuinely still running (heapA/heapB resource staging, the
.a0861f48 heapA-overproduction + 8432f37e wedge agents' scope).
The DMA-IRQ/ch4 thread is deprioritized unless a ch3-DMA-done-specific
load is found.

### N+81 — versus->fight REPRODUCED post-N+80: reaches battle init, then FAIL-FAST wild dispatch via bogus fn-ptr 0x80062401 (data-as-code); arcade->fight confirmed STABLE in-fight

Fresh versus nav on :4370/:4372/:4747 (nav_to_fight.py): all three runs
reach `gs=8 ph=8` FIGHT REACHED territory — [BRI] battle_round_init and
[BSI] battle_struct_init(p1=0x801f0270 p2=0x1f800000) print — then at
frame ~1871 the runtime FAIL-FASTs:

    unknown dispatch: addr=0x0B808000 ra=0x00062409 a0=0x1F8000E8 a1=0x801F5328

Post-mortem disp_ring tail (port 4372, before it was reaped) gives the
exact dispatch chain, all at sp=0x801FFFC0:

    seq 8777022  target=0x80195290  (gte_tr_load+0x20)
    seq 8777023  target=0x801357DC  (burst_slot_active+0xb8)
    seq 8777024  target=0x00062401  ra=0x801265F4   <-- bogus fn ptr
    seq 8777025  target=0x0B808000  ra=0x00062409  <-- FAIL-FAST

Decisive facts:
- 0x80062401 is BELOW the exe text base (PS-X EXE t_addr=0x8011E800,
  t_size=0xA0800 -> text ends 0x801BF000). It is RAM data, not code.
  RAM@0x80062400 = E0200010 E001310E 014B0107 1D22CD24 ... (packed
  data, not MIPS). The dispatch "succeeded" only because the
  runtime-written-code path executed the data until a jalr produced
  0x0B808000.
- The value is misaligned (+1) and does not appear as an aligned dword
  anywhere it was expected; it is a corrupted/derived pointer, not a
  stale-but-valid code address.
- a1=0x801F5328 = fighter struct (words there: 0x80078D10, 0x80071434,
  0x80079154 ... -> 0x80078D10 holds u16-pair keyframe data = .ANI
  stream). Consistent with N+75: P2's .ANI/stream data is the corrupt
  input; the VM/action dispatch reads a handler ptr that is stream data.
- CD side is CLEAN: wedged-instance dma_cdrom_history tail showed all
  ch3 transfers completed=1, dicr constant 0x009000000 (ch4 IRQ only,
  faithful per N+80), i_stat=0x04; pending Pause already fired.
  DeliverEvent starvation is NOT the live blocker — N+80 stands.

ARCADE REGRESSION CHECK: nav_arcade.py on :4746 reached gs=8 ph=8 and is
still running at frame 55379+ in-fight (live query: gs=8 ph=8, cdrom
idle, sector_available=1). Player-arcade->fight remains STABLE — the
crash is versus/attract-specific (P2 resource path), matching N+71/N+73.

=> The versus->fight wedge symptom has EVOLVED: versus now reaches
ph=8 (the old TestEvent spin is gone — CD completion delivers), but
fight init corrupts P2's action/.ANI stream and the action-VM dispatch
jumps through a data word. Next probe: identify which table/struct
supplied 0x80062401 (likely fighter->action/EV handler table indexed by
a stream opcode or an unvalidated .ANI chunk pointer), and whether the
P2 .ANI load wrote to the wrong heap slot (heapA-overproduction, N+74)
or the stream itself is mis-decoded. Sibling gwatch/wtrace agents own
the write-attribution hunt.

### N+83 — menu.c heap-producer audit: 3 faithful-bug fixes (heap_ptr_fixup gate+store, heap_record_emit record-init, stream_block_decode domain); heapA/heapB cursors advance by real emitted sizes

Instruction-level audit of every heap-size producer named in the versus
crash hunt (generated bodies read-only; clean `game/menu.c` compared
store-for-store):

- `heap_ptr_fixup` @0x8016CB20 — **was buggy, FIXED** (menu.c ~13466).
  Generated phase-1 runs iff `MB32(a0+4)==0` (the "already fixed" flag);
  the port gated on the +8 offset instead, skipping (+4==0,+8==0) blocks
  the guest still relocates and running it on (+4!=0,+8 small) blocks the
  guest skips. Inner loop: generated stores the resolved pointer back to
  the SAME slot it loaded (a2 advances in the delay slot after the sw)
  and tests ONLY bit31 — a `0x80000000` entry stores `a0+0`. The port
  wrote one word too far (post-increment store) and added a spurious
  `(v1 & 0x7fffffff)!=0` guard. Now `a2 = a0 + noff*4`, `MW32(a0+8,a2)`,
  nested row/col loops with in-place `a0 + (v1 & 0x7fffffff)*4` stores,
  `MW32(a0+4,1)`. a0+off*4 scaling and real-block base confirmed.
- `heap_record_emit` @0x8016CC5C — **was buggy, FIXED** (menu.c ~13560).
  Generated writes `sw zero -> rec+0` (0x8016CD18) then five u16 zeros at
  +4/+6/+8/+0xc/+0xe, `+0x20=src cursor`, `+0x24=s1-old`. The port was
  missing the +0 u32 zero (stale heap data survived as the record tag)
  and only ran the count<=0 early-out when arg5==0 — generated computes
  the FINAL count (`arg5!=0 ? arg5 : v1-a3`, lw sp+0x60 / sub at
  0x8016CCC8) then `blez` (0x8016CCCC) before the first slot, so a
  negative nonzero arg5 must not enter the do-loop. Both corrected;
  return is the advanced cursor `s1`, no size inflation.
- `stream_block_decode` @0x80163154 — **was buggy, FIXED** (menu.c
  ~2264). `param_2` is a HOST pointer (`guest_at()` of the table);
  generated adds the table's GUEST base to the entry offset. The port
  truncated the host pointer into the address math -> mode3_dispatch
  read open-bus mode word -> garbage return length -> heap cursor
  corruption (the original suspect for heapA overrun). Now
  `srcg = guest_of(param_2) + entry + (count+1)*4`.
- `heap_serialize` @0x8016B7C0 — **faithful** (menu.c ~14368). Emit-table
  walk uses 16-byte slots (`count<<4`, generated 0x8016B838 sll 4 +
  addu), terminator = next-ptr 0/0xffffffff, per-tag dispatch via
  B7C0_EMIT/B7C0_CALL{5,7}; all 28 `heap_serialize_tag_*` are pure
  `br2_call_generated` delegations with correct guest addrs + o32 stack
  staging (menu.c 14626-14666) — generated-authoritative by design.
- `terrain_packet_gen` @0x80135A14 — **faithful** (menu.c ~13890).
  Header stores match generated (`sw a1,0x1c(a0)` 0x80135A30, u16 zeros
  +0xc/+0xe, args 8/9 via guest stack sp+0x44/+0x48); constants
  0x140000 / tables 0x8019ac54 / 0x8019ab84 and <<4 strides verified.
- Cursor cells: heapA @0x801C47B8 and heapB @0x801E3CFC advance ONLY by
  serializer/emit return values (gwatch NV sequence 0x80010000 ->
  ... -> 0x800680DC during vs_init heap setup = real emitted sizes, no
  literal inflation found in any producer).

Runtime after fixes (headless, debug port): versus still dies in
vs_init, but the failure MOVED — no more FAIL-FAST 0x0B808000; the
process now SIGSEGVs inside `battle_flow_state_machine` via
`clean_try_dispatch` (host deref at 0x100000407, crash report
2026-11-17-034329.ips). That is battle.c territory (sibling-owned);
the heap producers are no longer the suspect for the wild dispatch.
Arcade path unchanged through gs=5 menu.

OPEN: battle_flow_state_machine host-pointer deref (sibling scope);
0x80062401 write-attribution still open per N+84.

### N+84 — dispatch mechanism PINNED: OOB opcode index reads string/data table as fn-ptr (both dispatchers FAITHFUL; bug is corrupt stream upstream)

Live-captured the wedge stack on a bare attract run pinned at frame 1871
(debug server starved — tight interpreted loop, same starvation class as
the script_stream_tick wedge):

  battle_intro_flow -> battle_frame_tick -> fighter_contact_resolve
  -> fighter_state_tick -> fighter_script_fsm (battle.c:5837, case 8)
  -> script_op_dispatch (battle.c:13722)
  -> psx_dispatch -> dirty_ram_dispatch -> dispatch_nonlocal_call
  -> psx_unknown_dispatch            (spin)   OR   FAIL-FAST 0x0B808000

Two faithful dispatchers, both read a stream opcode byte then index a
handler table with NO bounds check (correct — real HW never sees op>=size
on a valid stream):

- script_op_dispatch @0x80184f28 (battle.c:13697-13728):
    bVar1 = *(byte*)guest_at(*(u32*)guest_at(param_3+0x24));   // opcode
    h = *(u32*)guest_at(0x801b5528 + bVar1*4);                  // table
    br2_call_generated(h, ...)
  Table @0x801b5528 has EXACTLY 19 entries (ends 0x801b5574). The STRING
  table @0x801b5574 is immediately adjacent. bVar1>=19 reads a string/data
  pointer as a handler -> br2_call_generated(0x80062401) -> data-as-code.
- ev_stream_dispatch @0x8017ea7c (resolve.c:3094): op<24 -> clean
  PTR_FUN_801b4e68[24]; op>=24 -> guest table @0x801b4e68+op*4 (48+ real
  entries). op>=48 reads OOB into adjacent data -> same wild dispatch.

=> 0x80062401 is a value read OOB from the string/data table region, NOT a
relocated code pointer and NOT a heap-stomp artifact per se. The faithful
fix is UPSTREAM: the script/.ANI stream PC (param_3+0x24 -> motion+0x84)
points at corrupt bytes whose "opcode" is >=table-size. param_3 domain is
verified clean (guest_of(guest_at(param_1[0xf])) round-trips). Do NOT add
bounds checks to either dispatcher — that would mask the real corruption.

FIXED this session (all faithful, committed):
- 88ea0c4 clean_table guest_at() for 3 host-ptr intptr_t fns (regression)
- c5d4c7d save.c data_relocate wrong-base (stale slot vs heap cursor h)
- 3b5779c dma.c DICR done-flag latch on per-ch-enable alone (psx.wiki)
- 082a560 battle.c script_stream_tick host-ptr -> guest_of for script PC
  (the versus WEDGE manifestation; clean while-loop starved debug/vblank)

CD deferred-interrupt path verified FAITHFUL end-to-end (afcfd8bf):
CDROM irq -> cd_ctx_restore -> cd_chan_full_init -> cd_intc_dispatch ->
cd_response_pump -> cd_dma_chan_reset -> DeliverEvent(0xF0000009,0x20).
versus reaches battle_struct_init = all CD loads complete.

OPEN: which resource/stream is corrupt for versus+attract but not arcade
(P2 .ANI/script). heapA legitimately reaches ~0x8010AE54 (save.c faithful);
menu.c heap_serialize*/stream_block_decode/terrain_packet_gen producers +
heap_ptr_fixup scaling under audit (agent 67c5fb11); 0x80062401 write-
attribution under trace (agent 8ef478a3).

### N+85 — ROOT CAUSE of 0x80062401 wild-dispatch FOUND+FIXED: .ANI S4 cat_tab double-relocated by clean apply() (game/action.c)

N+84 pinned the mechanism (OOB opcode -> string-table -> wild fn-ptr) and
left "which resource/stream is corrupt" OPEN. This session traced the
corrupt stream PC upstream to its producer and fixed the faithful bug.

LIVE STATE at crash (post-mortem, debug :4821):
  VM ctx (ScriptVM/cat) @0x801f55c0  [ctrl+0x3c of P1 ctrl 0x1f8000e8]
    +0x00 obj_registry = 0x801f0001
    +0x0c cat_tab      = 0x000EACEC   <-- WRONG (should be 0x80079AD0)
    +0x14 cat_1        = 0x7F9635CB   <-- garbage (read from bad cat_tab)
    +0x18 cat_2        = 0xAE64CC9B   <-- garbage
    +0x24 pc           = 0x8007121C   <-- lands in .ANI keyframe/channel
                                       data, NOT script bytecode -> opcode
                                       byte 0x64 -> table[0x64]=0x00062401
                                       -> wild dispatch 0x0B808000
  ctrl+0x30 (FC_ANI .ANI blob base) = 0x8007121C  [== pc, blob base]
    .ANI header there: size=0xB064, h[5]=sec_off[4]=0x88B4

EXACT ARITHMETIC (proves double relocation):
  correct cat_tab = ani_base + sec_off[4] = 0x8007121C + 0x88B4 = 0x80079AD0
  observed cat_tab = sec_off[4] + 2*ani_base
                   = 0x88B4 + 2*0x8007121C = 0x88B4 + 0x1000E2438
                   = 0x1000EACEC -> (mod 2^32) = 0x000EACEC  == OBSERVED

ROOT CAUSE — clean-vs-generated diff in game/action.c `apply()`
(clean port of guest func_8017A05C, .ANI rebind+relocate):

  Guest func_8017A05C (@0x8017A05C, generated full_34.c):
    - seeds  vm+0x0c = h[5] = sec_off[4]  RAW file offset  (@0x8017A09C)
    - relocation pass (@0x8017A260-0x8017A398) adds ctrl+0x30 base to
      motion fields {0,4,8,c,ac,10,e8,ec,f0,f4} (10) and param_tbl
      {8,c,2c,30} (4) ONLY. It does NOT touch vm+0x0c.
    => vm+0x0c stays file-relative after apply().

  Guest func_80181DB8 = ani_fixup (@0x80181DB8, generated full_37.c):
    - UNCONDITIONALLY does cat+0x0c += ctrl+0x30  (@0x80181DBC-0x80181DCC)
      BEFORE the ctrl+0xb once-flag check (which gates only the 5-word
      cat_tab[] entry relocation, not cat_tab itself).
    => cat_tab relocated to absolute EXACTLY ONCE, here.

  Clean apply() had an EXTRA line the guest does not have:
      if (vm)  GSU32(vm, 0x0c) += base;        // action.c:169 (removed)
  so clean left vm+0x0c = sec_off[4]+base already absolute, then clean
  ani_fixup added base a SECOND time -> cat_tab = sec_off[4]+2*base.

FIX (game/action.c, apply()): deleted the vm+0x0c += base relocation and
left a NOTE. vm+0x0c is now seeded raw by apply() and relocated exactly
once by ani_fixup() — byte-faithful to the guest pair 8017A05C+80181DB8.
apply()/ani_fixup() are static helpers of clean action_enter_fwd
(0x80175FEC); no clean_table regen needed (no mapped fn changed).

SECONDARY CORRUPTION this also explains: with cat_tab=0x000EACEC,
ani_fixup's once-flag cat_tab[0..4] relocation (when ctrl+0xb==0) and the
cat+0x14/+0x18 = cat_tab[1]/[2] reads all operate on phys 0xEACEC — a
non-.ANI data region — producing the observed garbage cat_1/cat_2 and a
mis-seeded VM pc.

RUNTIME VERIFICATION:
- ATTRACT (headless, :4821): reaches gs=8 ph=8 (fight live) and stays
  stable — frames 6638->8698+ sampled gs=8/ph=8 continuously, and an
  earlier run held ph=8 through frame ~19461 (was crashing ~1871 before).
  Demo fights cycle ph=1<->ph=8 normally. cat_tab fix confirmed.
- VERSUS (nav_to_fight.py :4821): still dies, but at frame ~1594 in
  versus INIT with signal_11 epc=0x8019A648 (battle_flow_state_machine
  SIGSEGV) — BEFORE fight entry / before action_enter/apply/ani_fixup run.
  VERIFIED PRE-EXISTING + UNRELATED: stashed the fix, rebuilt original
  binary, re-ran versus -> IDENTICAL signal_11 epc=0x8019A648 lastfunc
  0x000029CC at frame 1594. This is the N+83 battle_flow_state_machine
  SIGSEGV (battle.c, sibling scope), an earlier blocker upstream of the
  cat_tab path. The cat_tab fix will apply once versus clears that.

NOTE on debug-server "DEAD" readings: during heavy interpreted fight
loops the debug TCP server starves (documented N+84); connection-refused
polls are false-negatives — the process stays alive (frame keeps
advancing). Confirmed via direct frame query.

OPEN: battle_flow_state_machine SIGSEGV (versus init, sibling scope);
versus fight-entry cat_tab path now unblocked in principle but unreachable
until that earlier crash is fixed.

### N+86 — versus->fight ADVANCED: triple_track_spawn PTR_DAT 8-byte-index SIGSEGV fixed; new blocker 0x80122F0C wild-jump at frame ~5393

The versus-init SIGSEGV flagged at N+85 (epc=0x8019A648, fault_addr
0x801B1DE4801B1DD4) is root-caused + fixed (commit e5a9047).

ROOT CAUSE — same host/guest domain class as N+83/N+85, but a NEW
sub-variant: STRIDE + WIDTH, not just domain.
  game/battle.c triple_track_spawn did:
      puVar3 = (undefined2 *)(&PTR_DAT_801b1e8c)[param_3];
  &PTR_DAT_801b1e8c == g_PTR_DAT_801b1e8c, declared `uint8_t **` in
  bind.c and bound by guest_for("g_PTR_DAT_801b1e8c") -> guest_at(0x801b1e8c)
  = host ptr to the TABLE BASE. The table holds 4-byte GUEST pointers to
  u16-pair data (generated 0x80138AAC-0x80138AE0: lui 0x801B/addiu 0x1E8C,
  entry=base+param_3*4, lw -> guest ptr, lhu -> u16, store entry+2 twice).
  But `uint8_t**` indexing reads sizeof(uint8_t*)=8 bytes at param_3*8,
  folding TWO adjacent 4-byte guest entries into ONE bogus 64-bit host
  pointer -> *puVar3 SIGSEGV. fault_addr 0x801B1DE4801B1DD4 is literally
  entries[param_3],[param_3+1] concatenated.

FIX (faithful, matches generated):
      puVar3 = (undefined2 *)guest_at(((uint32_t *)&PTR_DAT_801b1e8c)[param_3]);
  u32* index = 4-byte stride; guest_at() converts the guest entry addr to
  the host u16-data pointer. br2_gaddr(puVar3+1) then yields entry+2 as a
  guest addr, matching generated sw of gpr[2]=entry+2.

VERIFIED: versus now reaches gs=8 ph=8 (fight live) and advances to
frame ~5393 — far past the prior ~1900 SIGSEGV. triple_track_spawn cleared.

NEW BLOCKER (next layer, same bug FAMILY):
  frame ~5393 -> FAIL-FAST unknown dispatch addr=0x80122F0C
  ra=0x80126664 a0=0x1F8001F0 a1=0x801F5440. 0x80122F0C has NO recompiled
  block (only br2eu_func_80122170 exists in 0x80122xxx; no block_80122F0C).
  ra=0x80126664 is inside br2eu_func_80126654 = alias into
  br2eu_psx_alias_body_80126234. Prime suspect: a FUNCTION-POINTER/JUMP
  table read with the same uint8_t** 8-byte-index bug -> wild code jump:
    battle.c:8995  (*(code*)(&PTR_LAB_801200b8)[uVar3])()
    battle.c:10435 (&PTR_FUN_801b1734)[char_idx]
    + data tables 2368/2370/6629/15133 (same stride bug, latent)
  Two agents deployed: (a) comprehensive &PTR_*/PTR_FUN_*/PTR_LAB_*
  audit+faithful-fix across game/*.c + versus re-verify; (b) read-only
  trace pinning the exact table/index that yields 0x80122F0C.

STATUS: versus->fight is now UNBLOCKED through init (was the N+85 SIGSEGV);
remaining versus work is the in-fight 0x80122F0C wild-jump (PTR-table class)
plus any post-fight progression issues. Attract remains stable (N+85).

### N+87 — versus in-fight crash refined: 0x80122EB0 = SAME opcode-over-read; VECGA 0xBD storm is PRE-EXISTING; PTR-table audit landed

Post-triple_track_spawn (e5a9047), versus reliably reaches gs=8 ph=8
(FIGHT REACHED via nav_to_fight.py) then dies in-fight. Two manifestations,
SAME root cause (event-stream opcode over-read, N+84/N+86):

- FAIL-FAST `unknown dispatch addr=0x80122EB0` (was 0x80122F0C) —
  ra=0x80126664 a0=0x1F8001F0(P2 ctrl) a1=0x801F5440(P2 motion). Different
  garbage fn-ptr each run because the corrupt stream-PC byte varies; both
  land in the 0x80122xxx string/rodata pool. ev_stream_dispatch reads op at
  motion+0x84 (guest 0x801F54C4) -> ~0x800DC99A (heapB-staged, NOT .ANI) ->
  OOB past handler table 0x801B4E68.
- VECGA addr=0xBD/0xAC/0xBC exception storm (esp=0xBFC05B18 BIOS stack,
  dspc~0x801AA260) — near-null deref, PRE-EXISTING (identical on pre-PTR-fix
  baseline binary br2base.log). NOT a regression; likely same corrupt
  motion/ctrl struct feeding a bad pointer.

PTR-TABLE AUDIT (agent, uncommitted battle.c+clean_table.c): faithful
fixes at 2368/2370 (u32 stride), 3047-3065 (PTR_DAT_801b4e10/14 ptr-cell
double-deref), 6629 (u32 stride), 6723+15046 (&PTR_FUN host->br2_gaddr),
15133 (u32 stride+guest_at), 10435 (PTR_FUN_801b1734 was calling
UNINITIALIZED host code* -> now dispatches real guest table via
br2_call_generated+br2_guest_frame), 8995 impact_fx_update DELEGATED to
generated (jump-table-into-mid-function consumes live s-regs; correct
per delegation rule). clean_table.c regen dropped wrap_80138cec+wrap_80135180.

ROOT CAUSE (confirmed, being fixed): P2 .ANI ~0x800D1xxx-0x800DCxxx overlaps
heapB staging (cursor _DAT_801e3cfc bases 0x800d0000 intro_movie_state /
0x800f0000 menu_res_load_sm / 0x80010000 elsewhere) which sits INSIDE heapA
growth range (heapA -> ~0x8010AE54). heapB staging writes stomp P2 .ANI ->
corrupt stream PC -> opcode over-read -> wild jump / near-null deref.
Versus loads BOTH fighters -> P2 .ANI lands in kill zone; attract (demo)
loads fewer -> survives.

THREE agents converging: (a) static generated-diff of versus rebind/
menu_res_load_sm ordering (save.c/menu.c/game.c); (b) runtime watchpoint on
0x800DC99A/0x801F54C4/heap cursors to catch exact stomp-write PC; (c) PTR
audit finishing verify. Goal: find faithful ordering/reload/cursor fix.

### N+88 — STALE-FC_ANI CONFIRMED with live dump: P2 .ANI zeroed inside heapB band; PTR-table fixes committed (e11a9ee) cleared 0x80122F0C wild-jump

PTR-TABLE AUDIT COMMITTED (e11a9ee): all &PTR_*/PTR_FUN_*/PTR_LAB_*
stride+domain bugs fixed vs generated (u32 stride, ptr-cell double-deref,
host->br2_gaddr, PTR_FUN_801b1734 real guest-table dispatch,
impact_fx_update delegated). RESULT: versus 0x80122F0C/0x80122EB0
wild-jump CLEARED — reaches gs=8 ph=8 FIGHT REACHED. Baseline died earlier.

REMAINING versus blocker = pc=0 / VECGA near-null storm in-fight.

ROOT CAUSE NOW PROVEN (live fcani.log dump, port 4890):
  P1ani=0x8006e814  P2ani=0x800d1ff0  heapA_cursor=0x800680dc  heapB=0x800d0000
  P1head=a0ad0000087900003c0100004c7d0000   VALID .ANI header
  P2head=00000000000000000000000000000000   ALL ZEROS — stomped
Both FC_ANI (.ANI stream base, fighter record +0x30) values are STALE —
above the regrown heapA cursor 0x800680dc, i.e. allocated during an
earlier heapA pass before intro_movie_state reset _DAT_801c47b8=0x80010000
(game/game.c:1831). heapA regrew only to 0x800680dc; the .ANI pointers were
never re-stored.
P1's stale .ANI (0x8006e814) survives — below heapB band, data intact.
P2's stale .ANI (0x800d1ff0) lands INSIDE heapB staging band
[0x800d0000,0x800f0000] -> zeroed by heapB writes -> corrupt stream PC ->
ev_stream_dispatch opcode over-read / near-null deref -> VECGA/pc=0 storm.
At t=32.0 heapA reset to 0x80010000 (ph=0 transition) and P2head showed
transient garbage then zeros — consistent with repurpose+stale pointer.

FAITHFUL BUG: after intro_movie_state resets heapA mid-versus-flow,
generated must RELOAD .ANI from disc and RE-STORE FC_ANI before fight;
clean leaves the stale record+0x30 pointer. Versus loads BOTH fighters so
P2's stale .ANI lands in the heapB kill-zone; attract/demo loads fewer and
survives. Agent b4709d7e tracing the exact generated reload/rebind gate
(fighter-loaded flag surviving the heap reset) — fix pending.

---

## N+89 — .ANI/heap stomp is a GENERAL attract+versus bug; heapA ~1MB high-water is the suspect

NEW EVIDENCE (fcani_poll.log timeline, headless attract gs=6 auto-demo, no input):
  The opcode-over-read / VECGA crash is NOT versus-specific. attract demo fights
  crash after 2–16 cycles (br2_emu4 wedged ~2, br2_emu3 ~16, br2_4840 FAIL-FAST
  0x80122F0C on 3rd). battle_intro_flow (menu.c:1857) is the SHARED attract+versus
  fight path -> menu_res_load_sm -> save_state_machine_alt loads .ANI per fight.

  heapA cursor (_DAT_801c47b8) high-water during ONE demo resource load reaches
  ~0x801117c4 (~1MB of heapA allocations). P2 .ANI lands ~0x800d7xxx-0x800d1xxx —
  freshly loaded, BELOW cursor, valid at load time. P2ani DOES change each fight
  (0x800d297c->0x800d783c->0x800d1e58) so .ANI IS reloaded — the pointer is not
  simply skipped.

  Then gs=3 transition resets heapA ->0x80010000 while P2ani stays high (stale),
  heapA regrows only ~0x8004c880 during select. Next fight reloads .ANI again —
  but it keeps landing in the 0x800dxxxx band that heapB staging
  (_DAT_801e3cfc = 0x800d0000 intro_movie_state / 0x800dc800 / 0x800f0000)
  overwrites. heapB grows UP from a low base THROUGH the .ANI region.

OPEN QUESTION (agent c8b13257 tracing): does GENERATED heapA also reach
0x801117c4 (faithful overlap, stomp is ordering) or does clean OVER-ALLOCATE
heapA pushing .ANI into heapB's band? If over-allocation -> find the clean fn
inflating heapA. If faithful -> the stomp is heapB-0x800d0000 staging running
after .ANI load; find the generated ordering that avoids it.

Repro is now input-free: headless attract cycles demo fights until crash.
Stray fprintf debug at menu.c:1871,1956 (battle_intro_flow [BIF]) — pre-existing,
flagged for cleanup (no-printf rule).

## N+90 — versus progresses to frame ~24K post-e11a9ee; blocker is now a near-null deref, .ANI valid

POST-e11a9ee (PTR-table audit, committed) versus/attract state:
  - versus emu (port 4890) advances to frame ~24149+ and keeps going — was
    crashing ~frame 5393 before PTR fixes. e11a9ee cleared the 0x80122F0C
    wild-jump class AND bought large versus progress.
  - attract emu (port 4900, same binary) cycled 106K+ frames gs=3/4/5/6
    WITHOUT the earlier attract crash — attract may be stable now.
  - .ANI headers in latest versus capture are BOTH VALID
    (P1head=a4b6..., P2head=24c4...) — the heapB stomp did NOT trigger this run.
    .ANI IS reloaded each fight (P2ani changes per cycle).
  - Remaining crash = VECGA near-null storm, addr=0xAC/0xBC phys=0x0AC/0x0BC,
    ra0 host ~0x101d35f60-84 repeating. Guest dereferences a near-NULL pointer.
    26K+ exceptions logged; frame counter still advances (guest retries).

REFINED HYPOTHESIS: with .ANI valid, the near-null deref is likely a
versus-specific FIGHTER ENTITY / motion-ctrl sub-struct pointer that is NULL —
versus loads TWO human-selected fighters via versus_stage_commit/
battle_config_eval whereas attract CPU fighters are auto-provisioned. If P2's
entity/motion struct (e.g. 0x801f5440 family, stream-PC cell +0x84) has a NULL
sub-pointer the fight logic derefs -> 0xAC/0xBC. Agents b4709d7e (versus
rebind/entity) + c8b13257 (heap-overlap) converging; the heap-overlap may be
faithful (generated cd_file_load_sm reproduces it) — versus entity init is the
sharper lead.

## N+91 — versus->fight near-null root narrowed: intro_movie_state heapA/heapB self-overlap

FAITHFUL-BASES PROOF: generated br2eu_func_8012C0B4 stores
  0x8012C158: 0x80010000 -> [0x801C47B8] (heapA cursor)
  0x8012C164: 0x800D0000 -> [0x801E3CFC] (heapB cursor)
clean intro_movie_state (game/game.c:1831-1832) matches EXACTLY. Bases are correct.

MECHANISM: intro_movie_state is a multi-state decode SM (_DAT_1f80008c
0x10->0x20->0x30). It cd_file_load_sm's the movie into heapB@0x800d0000, then
stream_block_decode reads compressed source from heapB and writes decoded output
to heapA (cursor _DAT_801c47b8 advancing from 0x80010000). versus loads BOTH
fighters so heapA grows ~0xC0000+ during resource load; .ANI lands ~0x800d4f38 —
INSIDE heapB's [0x800d0000,+] movie region. When heapA decoded output reaches
0x800d0000 it overwrites the compressed source stream_block_decode is still
reading -> corrupt decode -> near-null pointer -> VECGA 0xAC/0xBC storm.
Attract never runs intro_movie_state's 0x800d0000 heapB path -> stable (106K+ fr).

OPEN: does clean stream_block_decode/intro_movie_state produce the SAME heapA
growth as generated, or over-produce pushing heapA into heapB early? If faithful
overlap, generated must bound heapA<0x800d0000 or order decode to avoid self-
overwrite; find that bound/order clean is missing. Agents b4709d7e/c8b13257/
a497e6a0 converging.

## N+92 — VECGA is a CLEAN-C bad-pointer diagnostic, not a guest MIPS exception

src/clean_bridge.c:78-82 guest_at() logs "VECGA addr=.. phys=0x80-0xC0 ra0=%p"
whenever clean C calls guest_at() with an address mapping into the PSX vector
page (phys 0x80-0xC0). The observed addr=0xAC/0xBC/0xBD are GUEST addresses
~0x000000AC — i.e. a CLEAN-C function is dereferencing a near-NULL / small-integer
value as a guest pointer.

This means the versus->fight blocker is a clean port reading a struct field
that holds a SMALL value (state/count/ID/offset — NOT a pointer) and treating
it as a pointer -> guest_at(0xAC) -> VECGA. ra0=__builtin_return_address(0) is
the HOST caller (PIE slide varies per run; map via per-run __TEXT base
~0x1049bc000 -> nm). NOT a guest MIPS load exception — the guest is fine; the
clean port's pointer domain is wrong.

Prime suspect class: versus P2 fighter entity/motion/ctrl sub-struct field read
as pointer but actually a small scalar (uninitialized or wrong-offset field).
Attract CPU-fighter path provisions the same structs differently -> survives.
Agent a497e6a0 pinpointing the exact clean function + field.

## N+93 — ROOT CAUSE PINPOINTED: prim_task_run reads unpopulated +0x1c invoke-list ptr

VECGA ra0 (PIE slide -> nm) resolves to _prim_task_run = prim_task_run,
guest 0x8018A4F8, clean-ported game/battle.c:13216.

VERIFIED FAITHFUL vs generated br2eu_func_8018A4F8:
  lh v0,0xc(s0)      -> *(short*)guest_at(param_1+0xc)   [active flag]
  lw s2,0x1c(s0)     -> *(u32*)guest_at(param_1+0x1c)    [invoke-list ptr P]
  lw s6,0(s2)        -> *(int*)guest_at(P)               [entry count]
  entries 16B: {+4 arg2, +8 arg1, +0xc func} jalr func
Clean port matches exactly -> prim_task_run is NOT the bug.

BUG = DATA: *(param_1+0x1c) holds 0xAC/0xBC/0xBD (small ints, not ptrs) ->
guest_at(0xAC)->VECGA. The 0x28-stride primitive-task descriptor's invoke-list
pointer is UNPOPULATED for versus.

Task array @ scratchpad _DAT_801f04c0 (0x1f8004c0), fed by channel_script_emit*
(battle.c:16730/16793/16801) via per-fighter channel-script struct param_3:
  base = *(param_3+8)+0xf0  or  *(param_3)   [+0xa0 variant]
Allocator = task_pool_alloc (guest 0x801a4ebc, battle.c:10843).

REFRAME: NOT heap-overlap/.ANI/decode (those were faithful/symptoms). This is a
primitive/render task provisioning gap for versus P2's fighter channel-script.
Attract CPU fighters provision differently -> stable 106K+ frames.
NEXT: find who writes task-desc +0x1c invoke-list for versus P2 channel script
and why it's skipped/garbage. a497e6a0 owns battle.c/fight.c/action.c scope.

## N+94 — SECOND fault site + full versus->fight mechanism mapped

Live versus run (port 4920) reached gs=8 ph=8 FIGHT then wedged in VECGA storm
(37K+ exceptions, debug server starved). Two fault sites, BOTH verified faithful:

1. prim_task_run (0x8018A4F8, battle.c:13216): *(desc+0x1c) invoke-list ptr =
   0xAC/0xBC -> guest_at(near-null). ra0 -> _prim_task_run+0x5c.
2. channel_script_emit_chain_gate (0x80134f2c, battle.c:17409): iVar3 =
   *(param_4+0xbc) is a small garbage value; guest_at(iVar3+0x4a)/(iVar3+0x4e)
   -> 0xBD VECGA. ra0 -> _channel_script_emit_chain_gate+0x24.

Generated br2eu_func_80134F2C verified instruction-identical (lw s5,0xbc(a3);
lw a0,0/8(s1); lh v0,0x26(s1); lh v0,0x4a/0x4e(s5)). BOTH ports faithful.

ROOT: versus P2's fighter channel-script/model context (param_3, param_4) has
UNPOPULATED pointer fields — *(param_4+0xbc) garbage, *(param_3+8) task-array
descriptors' +0x1c garbage. The per-frame render emit chain reads them -> VECGA.

param_3/param_4 are built upstream by the fighter model/channel-script
instantiation during versus->fight setup. Attract CPU fighters provision the
same contexts correctly -> stable. NEXT: find the versus P2 context builder
that skips/under-fills param_3/param_4 (likely fighter_spawn_setup /
battle_config_eval / model-load path). Agents f56f8eec + a497e6a0 on it.

## N+95 — menu.c select_screen_tick "fix" was a REGRESSION (reverted); versus_stage_commit is prime suspect

TEST: stashed the uncommitted menu.c select_screen_tick change
(goto LAB_80150558_2 -> spr_attr_alloc_b+return0  ==>  direct call + fallthrough).
Rebuilt, ran versus:
  PRE-FIX  (menu.c reverted): versus reaches gs=8 ph=8 FIGHT -> VECGA storm
           (the real prim_task_run bug). CONFIRMED working-to-fight.
  POST-FIX (menu.c change):  versus STUCK at gs=8 ph=6 (DAT_1f800001=6),
           never reaches fight. REGRESSION.
The return-0 was load-bearing for the clean port's structure even though
generated block_80150558 appears to fall through — the clean function's
do-while(1)/return-value contract differs from raw block flow. menu.c change
is STASHED (git stash "wip-menu-fix-test"), NOT committed. Whoever owns it:
the faithful form needs the correct return/tail structure, not a naive
fall-through — do NOT re-apply as-is.

PRIME SUSPECT for the real versus->fight VECGA (versus-specific, clean-dispatched):
versus_stage_commit (0x80128c18) / versus_stage_commit_b (0x80128d98),
menu.c:9461/9489 -> dec_finish -> victory_record_restore(battle, a1).
  a1 = dec_wrap(lh(s0+2), s2+1) then dec_walk (r -= 0x235c quirk) while lh(rec)<0.
  victory_record_restore copies record[a1]+0x1bdc (0x194 blob holding the
  [0]/[8] prim-task descriptor-array ptrs) into live 0x801f04a8.
If a1 resolves wrong for versus P2 -> 0x801f04a8[0]/[8] point at a wrong/empty
descriptor block -> channel_script_emit->prim_task_run reads garbage +0x1c -> VECGA.
versus_stage_commit is called at menu.c:7652(s0,2),9028(s0,1),9048_b(s0,1);
victory_record_restore also at menu.c:8631(param_1,4),12103(s1,0).
NEXT: verify a1 index math + per-side record selection vs generated
0x80128C18/0x80128268; dump 0x801f04a8[0]/[8] live at fight (catch before storm).

## N+96 — VECGA rate-limit reveals versus fight is STABLE (not a hard crash)

clean_bridge.c guest_at() VECGA fprintf was UNBOUNDED -> phys 0x80-0xC0 is
VALID low kernel RAM that faithful code legitimately reads (agent note:
fighter_trail_emit->prim_task_run NULL trail-slot base +0xa0). The per-access
fprintf+fflush stormed stderr and STARVED the debug server/heartbeat, making
the versus fight LOOK fatal. Rate-limited to first 256 hits.

RESULT (port 4950, rate-limited build, versus nav):
  reaches gs=8 ph=8 FIGHT, HOLDS ph=8 stable 30s+, frames ~100fps advancing,
  debug server fully responsive. VECGA capped at 256 (benign low-RAM reads).
The versus->fight "crash" was largely a DIAGNOSTIC ARTIFACT. The fight runs.

REMAINING QUESTIONS (next level, not blockers):
  - Is the fight fully CORRECT? (fighters animate/deform, HP/damage, round
    timer, KO->results->next round). ph=8 constant = in-fight phase (normal).
  - prim_task_run +0x1c garbage (0xAC/0xBC) may still mean SOME primitive
    invoke-lists unpopulated -> missing/degraded visual primitives for P2,
    even though non-fatal. versus_stage_commit/victory_record_restore index
    path (N+95) still the suspect for THAT.
  - Verify attract>16 fights + arcade regression on this build.

## N+97 — record-stride fix committed (944fd47); versus "crash" = benign VECGA + flaky battle_flow SIGSEGV

RESOLVED the N+95 prime suspect. `victory_record_restore`/`results_record_build`/
`dec_walk` used stride `0x2354` forward but `-0x235c` decrement — INCONSISTENT.
Generated computes `rec = 0x801c47c0 + idx*9052` (`idx*0x235c`) via sll3/add/sll3/
add/sll5/sub/sll2 BOTH directions; `0x2354`(9044) is only a field OFFSET (+9044),
never the stride. Clean mixed `*0x2354` fwd / `-0x235c` dec -> idx>0 resolved the
WRONG record slot -> copied wrong/empty 0x194 blob into live 0x801f04a8 (the
prim-task descriptor-array ptrs feeding prim_task_run +0x1c). Fix committed
944fd47 (menu.c, 3 sites + comments). Faithful at instruction level.

VECGA CONFIRMED BENIGN (agent a497e6a0, verified vs generated block_801356B0):
fighter_trail_emit does `a0 = *(0x801F04C0)+0xA0`; cell=0 -> a0=0xA0 ->
guest_at(0xAC/0xBC) = phys 0x80-0xC0 = the kernel exception-vector trampoline
page (BIOS legitimately writes handler stubs there). Generated does the
IDENTICAL read; guest never faults. Clean prim_task_run is instruction-faithful.
The "storm" was purely the host fprintf+fflush DoS (rate-limited f4564a8).

VERSUS STATUS: reaches gs=8 ph=8 FIGHT, holds stable ~100fps, debug responsive
(confirmed by a497e6a0 + b4709d7e + c8b13257, multiple runs). Attract cycles
demo fights stable (c8b13257, ~2min, 64 battle_struct_init).

REMAINING REAL BUG (flaky, pre-existing, multi-site): host SIGSEGV in
battle_flow_state_machine (menu.c:550, guest 0x8016FCC4) during versus init,
frame ~1572. fault_addr=0x60200008FE0 = wild HOST pointer (guest/host ptr
confusion), guest pc=0. Documented earlier at bf3ead3/N+85. Pinpoint agent
a9d30c03 on it now (freeze dump psx_freeze_dump_...2042 + report).

## N+98 — battle_flow_state_machine SIGSEGV = callee-saved x28 clobbered by host ctx-switch (RUNTIME bug, not game port)

PINPOINTED (agent a9d30c03 + my disasm confirm). Fault `ldrb w21,[x8]` at
battle_flow_state_machine+0x158 (static 0x1016582D0) = menu.c:566-567 loop head
`joined_r0x8016fd30`. x8=[x28]; x28 SHOULD be &_g__DAT_1f80000c (bound once by
constructor bind_globals_6 -> guest_for -> guest_at(0x1f80000c)=scratchpad+0xc).
[x28] yielded wild host ptr 0x60200008FE0 (heap/mmap/fiber-stack territory, NOT a
guest_at product: ram~0x112080008, scratch~0x112284000, openbus~0x13C4C2xxx).

ROOT CAUSE (favored): callee-saved x28 CLOBBERED mid-function by host
context restore — SAME bug as SESSION N+22 (~line 7756). Hardware watchpoint on
g__DAT_1f80000c NEVER fires -> the global is fine; x28 drifts into a ring/context
buffer holding host ptrs. Nondeterministic (frame ~1563-1572), versus-only timing
-> POSIX-fiber scheduler / exception setjmp-longjmp not preserving x19-x28.
NOT a guest/host confusion in menu.c logic; clean port faithful.
GENERATED CANNOT produce this (uses cpu->gpr[]+psx_read/write, no host ptrs).

FIX BELONGS IN psxrecomp RUNTIME (fiber/exception ctx-switch must save/restore
callee-saved regs x19-x28), NOT game/. NEXT: lldb repro — watchpoint on
&_g__DAT_1f80000c (fires only if global written=mechanism b) + break at bfsm+0x154
print $x28 each hit (confirms x28 drift=mechanism a). If global watchpoint silent
-> confirmed N+22 ctx-restore bug -> fix fiber/exception reg preservation.

## N+99 — clean-port sweep wave launched (ani/fight/vm) + x28 runtime fix in flight

PORT WAVE (parallel subagents, disjoint game/*.c scopes):
  3981ed7f -> game/ani.c  (116 LOC — animation subsystem, nearly all generated)
  dc8048c7 -> game/fight.c (93 LOC — fight logic, nearly all generated)
  51e181c9 -> game/vm.c   (542 LOC — script/event VM)
  Rationale: these are the THINNEST subsystems = biggest generated->clean gap.
  Big files already heavily ported (menu.c 539KB, save.c 310KB, cd.c 183KB).
  Delegates pending faithful ports: 144 total (cd.c:61, bind.c:41, save.c:21,
  menu.c:5, battle.c:6, ev.c:4, resolve.c:2, gfx.c:2, game.c:2).

x28 RUNTIME FIX (ed5184f8, ~65min): battle_flow_state_machine SIGSEGV root-
caused to callee-saved x28 restored WRONG across psx_check_interrupts
setjmp/longjmp + fiber switch. Register proof: x28=0x111A30000 (.bss ~0x4BC0
below globals, inside g_spdom_ring region) vs correct &_g__DAT_1f80000c
~0x111A34BC0; siblings x19-x27 correct. = context-restore frame mixup, NOT
data corruption. Suspects: deferred_exception_longjmp owner-fiber path +
nested-exception saved_exception_jmpbuf guard (interrupts.c:1205/2051).

INTEGRATION PLAN: agents port+build+verify on unique debug ports, commit only
their game/*.c; parent runs central gen_clean_table.py + build + verify +
commit clean_table.c once.

N+99a — second crash site = same root cause (context corruption), NOT a port bug.
  ed5184f8 lldb capture: EXC_BAD_ACCESS addr=0x0 in ev_stream_dispatch
  (resolve.c) <- action_commit <- fighter_frame <- battle_frame_tick.
  Checked PTR_FUN_801b4e68[24] (ev.c:1382): ALL 24 entries populated (w0-w23),
  so NOT a null table entry. pc=0 arises inside a handler OR via
  br2_call_generated(h) where h=*(guest 0x801b4e68+op*4) is garbage because
  op/param_2+0x84 stream-pc was corrupted upstream. => SAME context-corruption
  root cause as the x28 battle_flow_state_machine crash, manifesting at a
  second clean site. Confirms: ONE runtime fix (ed5184f8) should clear both.

N+99b — REAL clean-coverage numbers (corrects earlier estimate):
  clean_table.c wraps 1113 guest fns. generated/ has ~3878 fn syms.
  renames.map names 1195 (the NAMED subset — ~93% of those are clean).
  TRUE unported bodies = ~2797, spread across generated/full_00..full_54
  (dispatch.c is the master symbol table, NOT bodies). Heaviest files:
  full_35=183, full_53=162, full_00=150, full_01=182, full_39=98 unported.
  => Mission is a LONG porting haul; wave-2 should target heaviest
     generated files OR hot gameplay address ranges, partitioned disjoint.
  NOTE: ani.c is a .ANI file-format PARSER (not recompiled game code —
  nothing to port); fight.c has only ~2 guest fns (FUN_80176248,
  FUN_801787dc). vm.c is the real VM interpreter (moderate scope).

N+100 — PORT WAVE 1+2 COMPLETE (5 agents, all committed + runtime-verified):
  6fae410 ani.c   9 GTE/anim math fns (vec3 normalize, rsqrt, angle)
  102a108 fight.c 2 dispatch regs (action_resolve, fighter_frame — were
                  ported-but-unregistered)
  4a7d036 vm.c    3 script opcodes (cmp/add/move-delegate) + helpers
  68cda22 action.c 5 *_guest dispatch regs (apply/reset/entry_setup/
                  ani_fixup/pending_gate — same ported-but-unregistered gap)
  6f08b94 gfx.c   18 GTE/render ports (fixed 3 real bugs: mask-order,
                  shift-dir, gte[6] src) + 2 delegates
  5c5fe39 gen     clean_table 1113->1155 wrappers (+42), fnmap 1247
  KEY INSIGHT: much "unported" work = already-ported code LACKING
  renames.map/dispatch registration. Agents efficiently wire existing
  clean C to guest addrs (near-dormant faithful coverage).
  REMAINING named-unwrapped=123: ~50 cd_* are CORRECT delegates
  (interrupt-bound), ~30 heap_serialize (agent 8af32e18 on it),
  printf/mc_*/misc small clusters. 2679 unnamed FUN_ still the bulk.
  ed5184f8: PSX_X28_WATCH instrumentation pinpointing x28 clobber
  across interrupts.c/clean_bridge.c/game.c — fix pending.

=== SESSION N+101 — x28 SIGSEGV REPRODUCED + register-level pinpoint (crash_trace sig_regs) ===

REPRO: plain HLE nav run SIGSEGV'd at frame 1562 (versus-init window,
matches N+98 ~1571). Not every run — still flaky but reproduces.

NEW TOOL (kept, crash_trace.c +54): sigaction ucontext now captures the
FULL arm64 GP file (x0-x28,fp,lr,sp) into the crash JSON "sig_regs".
This pinpoints the stale-register mechanism directly at the fault —
strictly better than the boundary x28watch (which never fired).

FAULT DECODE (psx_last_run_report.json):
  fault_symbol = battle_flow_state_machine, fault_pc = bfsm+0x158
  (image_base 0x1024D8000, fault_pc 0x103B317E4 -> file 0x10165982C,
   the `ldrb w21,[x8]` after `bl _frame_tick`).
  fault_addr = 0x100000407  (= [x28], the wild pointer dereferenced)
  x28  = 0x113740000 runtime = FILE 0x111268000
  CORRECT x28 = &_g__DAT_1f80000c = FILE 0x111274bc0 (runtime 0x11374CBC0)
  Siblings x19/x24/x25/x26/x27 = 0x11374Bxxx/0x11374Cxxx = CORRECT bfsm
  static bases. ONLY x28 is wrong -> single-register restore, not a
  wholesale wrong-frame.

WHAT 0x111268000 IS: inside the RUNTIME diagnostic-ring .bss region —
_g_irqctx_ring(0x1111fa440,4096 entries),_g_disp_tail(0x11126bec0),
_g_spdom_ring(0x11126cac0),_g_callret_ring,_g_pc0exc_ring,
_g_nestgate_*/_g_sched_escape/_g_scheduler_jmpbuf(0x111271acc) all live
0x11126xxxx-0x11127xxxx. So the donated x28 is a RUNTIME interrupt/
exception/debug code adrp base (or ring pointer), NOT a game clean-C
static. => the nonlocal unwind that restored it ran on a RUNTIME
stack/context (psx_check_interrupts exception path / scheduler escape /
fiber switch) whose x28 pointed into these rings.

MECHANISM (confirmed, same class as dirty_ram_dispatch_inner 57e5a72):
a host callee-saved register (x28) restored STALE across a
setjmp/longjmp or fiber/context switch. Donor context = runtime code
holding a ring-region base in x28. NOT data corruption (global itself
is fine — watchpoint never fired in N+98), NOT guest/host confusion in
menu.c (port faithful).

REVERTED: the x28watch boundary instrumentation in interrupts.c
(psx_check_interrupts body/wrapper split), clean_bridge.c (br2_irq_poll),
game.c (frame_tick canary). It never fired and perturbs the hot
interrupt path; superseded by sig_regs. crash_trace.c sig_regs KEPT.

NEXT (needs lldb repro — it DOES reproduce ~frame1562):
  break/watch to catch the unwind that leaves x28=0x111268000 in the
  bfsm frame. Prime suspects (runtime, ring-adjacent): the
  exception_jmpbuf setjmp/longjmp epilogue in psx_check_interrupts
  (interrupts.c ~2063 + saved_exception_jmpbuf memcpy ~2055/2087),
  deferred_exception_longjmp owner-fiber path (interrupts.c:1205-1221),
  g_scheduler_jmpbuf escape (traps.c:858), or a generated block-leader
  psx_check_interrupts_at whose inlined body keeps a ring base in x28.
  The generic faithful fix = ensure the unwind boundary cannot deliver a
  foreign callee-saved register file (per-context jmp_buf or explicit
  x19-x28 save/restore around the escape), NOT a game-side workaround.

SEPARATE BLOCKER (unchanged, deterministic): heapB cinematic staging
overwrites heapA .ANI S0 -> wild ev_stream opcode -> unknown dispatch
0x80122F0C (N+29b). Independent of this host-register bug.

N+102 — x28 bug narrowed to a frame_tick CALLEE (4 agents, still unfixed):
  x28 = globals ADRP PAGE BASE 0x111280000 (missing +0xbc0 add); bfsm sets
  x28=x19=0x111280bc0=&_g__DAT_1f80000c, corruption during `bl _frame_tick`.
  frame_tick does NOT save/use x27/x28 -> passes x28 to ~40 callees.
  VERIFIED CLEAN: psx_check_interrupts/_at/_dispatch_entry, br2_irq_poll,
  br2_call_generated, fibers(ucontext), runtime interp — all save x19-x28.
  => culprit = a frame_tick CALLEE keeping a globals page-base in x28 live
     across psx_check_interrupts (same class as 57e5a72 noinline fix).
  Escape-capable callees: _memcard_tick, _deferred_cmd_drain,
  _pad_state_refresh, _draw_env_timing. NEXT: bisect which callee returns
  foreign x28 — break at _frame_tick entry during versus (gs==8), finish,
  compare x28; or noinline-audit those 4. Repro flaky + masked by separate
  versus->fight pc=0 DISPATCH FATAL bug (isolate separately).

- **session (OpenBIOS permissive patch — boot wedge RESOLVED):**
  Early-boot "TestEvent spin -> pc=0" was an OpenBIOS patch_hook fatal
  halt, not an event/CD bug: BR2's legitimate GetC0Table call (tail-chain
  ra=0x80126260, hash 0xf12639c8, not a registered patch site) hit
  g_patch_permissive=0 -> "Couldn't find C0 patch hash ... Stopping."
  Patched openbios.bin in place (0xBFC07F24 lw->li v0,1; commit c74f4db),
  regenerated BIOS, verified boot proceeds: CD streaming active,
  frames ~330/s, CD deferred coroutine (0x8019A87C/TCB 0x8FE0) per frame.
  NEW active blocker: CD consumer stall — repeating
  SetLoc(1:55:63)->ReadN->Pause loop, ring_dropped growing, last_lba
  pinned; debugger agent tracing ch3-DMA-vs-PIO read path + DeliverEvent
  link (handler 0x8019B7E0 / cd_dma_chan_reset 0x8019b7e8, EvCB
  0xF1000008). NOTE: pc=0 samples with advancing BIOS calls = normal
  idle park under HLE scheduler, not a crash.

- **session (game bring-up — all crash gates cleared):** Massive
  faithful-fix campaign took the game from frame-1899 crashes to
  frame-30000+ clean. Root causes fixed: exception_jmpbuf liveness
  (3fd28d2), prim_group_build host-ptr (ee5c6d1), dirty-RAM MOVZ/MOVN
  (d963244), call_guest4 arg poisoning + $t0 (822088f/9f74a65),
  ret_zero_d loop semantics (fbc8d52), callee-saved preservation
  (interrupts.c), bail-unwind guard sweep all clean-C poll loops
  (ced22d6+a639dbc), heap_serialize nm constants + tag mask
  (ee57989/dcebda1), prim_list_gte_invoke a1=0 kernel corruption
  (afd29f0), stepper cb guest addrs (4193975), camera/light host-ptr +
  arg-order (f5cfd28), mode_name_announce v0=4 (8bc5e35). CD streams
  real Mode-2/XA continuously; attract reaches gs=5 ph=1 title.
  PSX_FAST_FORWARD_SPEED=max = ~100fps enables versus verification.
  CURRENT BLOCKER: kernel SIO/pad driver (func ~0x27EC) stopped issuing
  pad-read TX @byte_seq 15464 -> tx_writes frozen -> guest never sees
  pad -> versus input dead (confirmed end-to-end: START injected,
  title won't advance). Agents investigating why pad-poll thread
  blocked/exited. Transcription audit swept constant tables clean.

- **session (versus fight UNBLOCKED — OT-cycle root cause fixed):**
  Pad input FIXED via OpenBIOS initCard patch (NOP store of caller a0=0
  to s_padStarted @BIOS+0x22320; regenerated openbios.bin sha256
  d35c5f10...). Versus navigation reaches gs=8 ph=8 fight init. The
  ph=8 GPU OT linked-list cycle wedge root-caused to a clean-port
  miscompile: battle_flow_state_machine cached _DAT_1f800008/00c at
  entry instead of reloading per-iteration like guest block_8016FD38
  (lw $2C,0xC($16)/lw $2B,8($16) each pass) — pinned OT base, stale
  cells, closed packet cycles (f2eeb5f). Versus fight now advances
  past frame 6150. Audit campaign continued: BR2_RECIP_TABLE base
  0x20 low (e7d646f), ev.c 4 divergences sx12/sx8/signed-idx (1626632),
  desc_opcode_dispatch case2/3 arg-swap + case9 index (6f74887, alias
  of ev_cmd_dispatch body). RESIDUAL: fight frames advance ~0.1fps,
  gp0_draw ~70-80K/frame, emit:clear 64:21 — suspect frame-divisor
  byte 0x1F800019 stuck 0 gating battle_frame emit. Agent on it.

- **session (CD/FM V streaming wedge ROOT-CAUSED + FIXED; versus 0.1fps
  RESOLVED as wedge fallout):** The recurring intermittent FMV/tile-stream
  wedge (consumer parks, attract_stage_load_step clobbers shared arena
  0x80010000) was exhaustively exonerated on the delivery side — sector data
  byte-exact vs Game.bin, LBAs consecutive, paired CD-DMA descriptor/data
  destinations correct, flow-control gates inert, s_int1_lost=0, idle_skip
  off, dispatch 0 misses, and cdrom_timing_dump IDENTICAL fast vs
  conservative. Decisive A/B: PSX_EVENT_STEP_CONSERVATIVE=1 control ran
  clean to frame ~57k (7.5x past the ~5556 fast-mode wedge). Root cause was
  NOT the arena sharing (intentional guest design) nor a delivery gap — it
  was cdrom_cycles_to_irq() UNDER-ADVERTISING the armed i_stat-bit2 IRQ
  presentation deadline to the fast deadline-chunker: it tested live
  irq_flag, but present_cdrom_irq() raises for s_irq_i_stat_reason while
  s_irq_i_stat_armed. A HINTSTS ack inside the 5000-cycle
  CDROM_IRQ_PRESENT_DELAY window zeroed irq_flag without cancelling the
  owed presentation -> chunks stepped up to PSX_DEADLINE_HARD_CAP (16384)
  cycles over the CD IRQ boundary -> deferred command-issue
  (cd_seek_param_issue->cd_reg_window_op->worker) missed its doorbell ->
  cd_mc_state_machine producer starved -> file load reused the dead arena.
  FIX (be17669, faithful — deadline predicate now byte-identical to
  present_cdrom_irq: s_irq_i_stat_armed?s_irq_i_stat_reason:irq_flag, plus
  due-now coverage for queued_cmd/xa_data_end/cdda_data_end/lid and
  cdda_delay advertisement). New cdrom_state irq_present_late/max_late:
  max_late 15539->0 cycles over 2210+ presents. Fast instances clean past
  the wedge to 144k+ frames at ~300-400fps. The frame-divisor 0x1F800019
  "stuck 0 -> 0.1fps" RESIDUAL is RESOLVED as wedge fallout: player combat
  wedged in battle-init CD loads so the scroll/divisor path never engaged;
  post-fix versus fights (gs=8) run at full speed with divisor=0 across
  multiple instances. Game now runs boot->title->attract demos->versus
  combat cleanly. proof/cd_dispatch_localization.md has the full chain.

- **session (CD-ROM ring/BFRD/IRQ-late hypothesis DISPROVEN):** Live
  debug-port-8934 capture shows the sustained Mode-2 stream completed:
  Setloc 1:58:70 -> ReadN -> Pause through LBA 8801, Pause completed at
  frame 57005, CD IRQ2 set/fire consumed at frame 57007, then CD state
  frozen idle (`reading=0`, `pending.active=0`, `irq_flag=0`,
  `irq_present_max_late=0`). `sector_read_pos=2060/2340` is the normal
  Mode-2 payload boundary (12B header + 2048B payload; 280B EDC/ECC tail
  intentionally unread), not consumer starvation; `ring_partial` is ECC
  residue. ch4 DMA finished (`chcr=0x201`, `remaining=0`, DICR no pending
  done bits), CD seek gate `0x801BDEE4=1`, EvCB slot8 `0xF1000008` =
  `OpenEvent(0xF0000009,0x20)`. Residual wedge is post-CD kernel/event
  scheduling: frames advance, GPU/OTC DMA cycles, `pc=0` idle, B0 TestEvent
  frozen while B0 ReturnFromException climbs and `sp_ring` repeats
  `0x801FFE18<->0x00008548<->0x801BDD30` toward `0x8019A87C`/`0x8019A53C`.
  No `cdrom.c`/`dma.c`/IRQ-timing patch is justified; proof:
  proof/cdrom_ring_bfrd_stall.md.

- **session (arcade CD wedge narrowed to session-callback loss):**
  proof/cd_event_drop_point.md had localized the drop to channel-entry
  finalize starvation after a mid-ReadN Pause(0x09). Live descriptor dump
  (proof/cd_finalize_gate.md) shows the 0x80010000 arena IS overwritten by
  ch3 stream DMA (known §8 recurrence) BUT the initiating defect is
  producer starvation: session callback cells 0x801bc848/0x801bc84c AND
  parked saves 0x801bcba8/0x801bcbac are all 0, so cd_response_pump has
  nothing to invoke and cd_mc_state_machine (0x80199440, correctly
  delegated) never runs. Clearers identified: cd_chan_reset_b (0x8019746C,
  via cd_init_mode_dispatch mode==2, 0x80195C1C) and cd_subsystem_init
  (0x801974B8) both zero the cells; installer is cd_probe_retry
  (0x80195AA4 -> 0x80195b4c/0x80195b74). Static agent tracing the exact
  loss sequence. Separately documented (proof/cd_pause_sector_semantics.md)
  a REAL psx-spx divergence in exec_command case 0x09: hardware keeps
  delivering buffered-sector INT1s after Pause (response queue), while our
  stop_read_stream()+cdrom_clear_pending_dataready() kills them at
  dispatch and the ACK reports READ already cleared (hw shows 0x22 then
  0x02). Not the wedge cause here, but a faithfulness fix candidate.
  proof/dat1_writer_audit.md resolved the battle_frame state-3 question:
  escape is async resource_rebuild_a(->5) via task_pools_tick; the ==3
  freeze is downstream of this CD stall.

- **session (record-table wild-dispatch crash — IRQ-vs-main-stack race,
  ack_delay faithful fix landed):** FAIL-FAST `addr=0x801BCB94
  ra=0x801A1EC0 a0=0x801FF170` reproduces during battle CD streaming
  (frame ~39-46k). Full chain pinned: CD IRQ → `cd_intc_dispatch`
  (0x8019a8ec, IRQ stack 0x85xx, delegated) → `cd_cmd_queue_service`
  (0x801a189c)/`cd_cmd_queue_drain` (0x801a38b8) → `dev_cmd_round_dispatch`
  (0x801a1e84) on `rec=0x801FF080+head*240` → `cd_cmd_state_commit`
  (0x801a3d84) reads `rec[70]` state → jalr `*(rec+0x14)` handler. Records
  are FIXED 240-byte BSS slots the game places (`cd_bzero(0x801ff080,0x1e0)`),
  shared CD/SIO/memcard; `a0=0x801FF170`=slot1. `recvalring` shows main-thread
  `sw $sN` spills writing session ptrs (0x801BCB8C) onto rec+0x14 while
  `cd_file_load_sm→mc_init_reset→cd_result_wait_a→cd_cmd_issue→cd_timer_read`
  nests the main stack to `sp=0x801FF0F0` (INSIDE the record table). New
  `spdepth` debug ring (fntrace.c) proves excursions are faithful per-frame
  generated wait/poll nesting (same tcb=0x8FE0, clean unwind), NOT a leak —
  `br2_call_generated` preserves guest sp exactly. **Faithful fix landed
  faa6a76:** `exec_command` ran synchronously on the command-byte store so
  HINTSTS/response FIFO were visible ~0 cycles after issue; DuckStation
  `cdrom.cpp BeginCommand→GetAckDelayForCommand` schedules execution at
  Init=80000 / disc=25000 / no-disc=15000 cycles. Now every command routes
  through `queued_cmd` with `exec_due`; verified every command execs exactly
  +25000 cycles after queue, game boots/streams normally. **Did NOT fix the
  crash** — spdepth descent unchanged (structural depth = fixed in-flight CD
  ops/file-load, not ACK-rate). Crash is a genuine RACE: IRQ record-dispatch
  reads a record at the instant main-stack spills leave rec[70]=dispatch-state
  + rec+0x14=session-ptr. On HW the same depth occurs, so the divergence is
  IRQ/cooperative-switch TIMING — whether the CD-completion/callback work
  that nests on the main stack should be deferred to the 0x801BDxxx coroutine
  or the IRQ dispatch should never coincide with the corruption window.
  A/B test `PSX_DEFER_SWITCH=0` (legacy immediate cooperative switch) launched
  on :9795 to discriminate the deferred-switch frontier. proof/cd_cmd_ack_delay.md.

- **session (defer_state instrumentation + cooperative-switch RULED OUT for
  this crash):** added `defer_state` debug cmd exposing
  `psx_irq_delivery_diag` out[3,4,5] = s_defer_switch_pending/target/from.
  On the record-table crash (9796, frame 6897, same 0x801BCB94) the latch
  reads `defer_pending=0 target=0 from=0` — no BIOS cooperative ChangeThread
  was armed at crash time (caveat: latch is transient, 3s poll could miss an
  arm→consume). IMPORTANT distinction: the game's CD deferred-coroutine
  (0x801BDxxx stack) is a SEPARATE mechanism from the BIOS s_defer_switch —
  it runs via `cd_ctx_save`/`cd_ctx_restore` (jmp-buf 0x801BCD50={ra=0x8019A87C,
  sp=0x801BDD30}) and is what increments DAT_801bdde0 to release the
  cd_result_wait_a/cd_timer_read polls (cd.c:376-389). PSX_DEFER_SWITCH=0 A/B
  crashed differently (0x80196BD4 timer/ctx-restore @5789) — timing-sensitive
  but not a clean discriminator. The record-table crash is therefore an
  IRQ-presentation race: cd_intc_dispatch's record dispatch reads
  0x801FF080+slot*240 records at the instant the main thread's faithful
  cd_file_load_sm nesting (sp→0x801FF0F0) has spilled session ptrs onto
  rec+0x14. Next faithful lever: CD IRQ/HINT presentation timing vs the
  deep-stack window — does the IRQ that runs record-dispatch fire while the
  wait-loop stack is still deep on HW, or is it presented after unwind?

- **session (record-table wild-dispatch crash FIXED via faithful IRQ
  timing; clean-frame model landed but OFF):** Two parallel tracks.
  (a) CLEAN-FRAME MODEL — `gen_clean_table.py::clean_frame` decodes each
  clean fn's `addiu sp,sp,-N` prologue into a new `CleanDispatchEnt.frame`;
  `clean_try_dispatch` charges it bail-aware. A/B (PSX_CLEAN_FRAME) showed
  frames-ON livelocks the CD path (cd_mc_state_machine spin→watchdog): the
  clean ports' br2_guest_frame/guest_sp usage is calibrated around frame-less
  sp, so faithful sp needs the ports reworked stack-aware first. Landed
  default-OFF behind `PSX_CLEAN_FRAME=1`; proof/clean_sp_frame_model.md.
  (b) IRQ TIMING — DuckStation cdrom.cpp diff (subagent) showed our flat
  `CDROM_IRQ_PRESENT_DELAY=5000` put EVERY CD IRQ ~+5000cy late vs DS's
  sync-immediate/async-1000-500 model, and that the 5000 guard was sized for
  the OLD synchronous-exec model (now moot: cmd_ack_delay defers exec 25000cy
  past issue so the issuing ISR is gone). Implemented the two-tier model in
  cdrom.c `set_irq`: ACK/ERROR present due=now; DATA_READY/COMPLETE/DATA_END
  follow QueueDeliverAsyncInterrupt (deliver now if >=1000cy since last
  HINTSTS ack else +500); `s_cd_last_interrupt_time` stamped on the
  0x1F801803.1 ack that drives irq_flag->0. Re-arm latch + s_irq_i_stat_reason
  preserve lost-IRQ protection. **VERIFIED: port 9804 ran to frame 91136+
  with continuous mode-0xA0 file-loads, unknown_dispatch_log total=0 (zero
  wild dispatches), int1_lost=0, irq_present_max_late=0 — previously crashed
  ~39-46k with the 0x801BCB94 wild dispatch.** The +5000 skew was the lever:
  it held cd_intc_dispatch's record reads ~5000cy later, inside the
  file-load deep-stack spill window; faithful timing moves the dispatch out
  of the corruption window. Commit 206ad35.

- **session (clean-port wave + host-buffer->guest-addr bug class fixed):**
  After the IRQ-timing crash fix, ported 14 unported guest fns to clean C
  (commit f02418a): fight.c ret_zero_g/ret_nop/part_link_tail/
  ev2_hitstun_gate_dispatch; menu.c width3_num_space_pad/width3_num_zero_pad/
  sprite_record_tick/3x task_node_cb_noop_*/sprite_slot_commit-adjacent;
  vm.c ev4_branch_dispatch/ev6_guard_sync_dispatch/ev20_timer_dispatch/
  desc_flagbit_dispatch. Correctly LEFT generated: vm.c desc_op_head/
  desc_small/desc_op_e/desc_op_d/cond_eval_f/desc_op_c/desc_eval_largest
  (live $v0/$v1/$t0/$t1 or CPS-split $ra semantics), and 0x8015607C
  (CPS-split continuation inside gauge_scale_fixed, not standalone).
  clean_table.c now 1318 wrappers.

  Then fixed a SYSTEMIC host-buffer->guest-address bug class found by audit
  (proof/host_buffer_guest_addr_bugs.md): clean ports kept guest-stack
  buffers/structs as host C locals (often Ghidra-split scalars) and handed
  them to callees doing guest_of()/guest_at() -> guest_of(host-stack)=0 ->
  garbage/blank. Fixed so far: brightness_packet_send (28fa473, host buf[4]
  -> cd_cmd_retry guest-addr), sprite_slot_commit + hud_announce cases 3/21
  (7672085, host blk/buf -> gpu_texclut_pkt/descblk_variant_e). In flight:
  player_name_entry name-buffer + 6 stepper-descriptor callback sites
  (menu.c + save.c) where call_guest(cb, guest_of(host-desc)=0) starves
  generated dialog callees (dialog_sel_dispatch/_wide/msg_dispatch).

  COMBAT STATUS (unchanged, now precisely scoped): NO run has reached actual
  combat. gs=6 = battle intro/results only. Live fight needs announce phase
  0x300->0x700 (or DAT1->9); writer unidentified — prime suspect is script2
  handler table 0x801B2820 (handlers 0x8014cd48..0x8014cdbc, last tick 0x51
  ->0x8014cdbc) driven by channel-script task LAB_80137acc/script_stream_tick.
  Phase globals at scratchpad 0x1f8000b4/0x1f8000b6. Versus path (2P,
  game.toml players=2) reaches gs=8 differently but wedges on wild-dispatch
  0x80122DC8 (not a fn entry — corruption-class, separate bug). Static audit
  of script2 handlers launched.

  Verification: debug :9804 at frame 1,000,182 (~25x prior crash window),
  :9806 636k, :9807 175k, :9808 (pointer-fix binary) 71k cycling gs=6 battle
  states — all unknown_dispatch_log total=0.

- **session (pointer-bug class CLOSED + versus reaches real combat):**
  All 4 audit clusters in proof/host_buffer_guest_addr_bugs.md now fixed:
  stepper-desc callback ABI (6 sites menu.c+save.c, commit 03ada72) —
  descriptors guest-frame-backed at real prologue sizes + dialog_*
  _dispatch callees normalised via br2_gaddr dual-ABI; player_name_entry
  name buffer (4 contiguous 8B slots @sp+0x18, scalar macros -> pne_slots);
  sprite_slot_commit + hud_announce (7672085); brightness_packet_send
  (28fa473). Named-function clean coverage ~95.7% (1318/1377 fnmap;
  remainder correctly BIOS/IRQ-delegated). Mapper confirmed residual is
  unnamed CPS-split blocks; small gfx/game leaf candidates handed to a
  port agent (0x80194A70/0x8018D408/0x80192EC0 + 4 jr$ra leaves).

  COMBAT — versus path CONFIRMED reaching real fights: 2-player instance
  (game.toml players=2, :9810) + versus_fight_mash + :9809 versus gs=8 both
  cycle select->intro->round->RESULT with phase win/lose transitions
  (0x1f8000b4/b6: 0300->1400/0500->1100/0100->0300). Win/lose phases prove
  rounds resolve = combat executes on the versus path (post_battle_dispatch/
  vs_confirm_b, all clean). NO 0x80122DC8 wedge this build (pointer fixes may
  have cleared it). The 0x300->0x700 blocker is ARCADE-SPECIFIC: the
  announce->fight phase write is NOT in clean C and NOT in script2 handlers
  (9 handlers fully decoded: cosmetic OT bits + DAT1=9/0xF0 only). Prime
  suspect now EV/event-script opcode or unported channel-script/task
  handler — g__DAT_1f8000b4/0xb6 bindings in bind.c exist for generated
  script code. Generated-side grep for the phase writer in flight.
  Rule stands: do NOT fake the phase write in game/*.c.

- **session (0x700 writer RESOLVED + arcade stall localized to BFSM ph=0):**
  Exhaustive generated scan (55 files): the ONLY instruction producing 0x700
  in phase cells 0x1f8000b4/b6 is `sh $v1,0xB4($v0)` @ guest PC 0x801538B8 in
  battle_config_eval (0x80153544, clean menu.c:4093, L_set700 :4235), gated
  behind cls 0x000/0x600 — faithful wait, NOT a missing fn. Slot is runtime-
  variable (AB[2]) so no literal +182 store exists. Alias-table bypass
  analyzed: 5 mid-fn aliases (0x8015364C/6D8/730/7B0/7CC) absent from
  clean_table but clean port uses C switch (no call_guest) — not a bug.
  NEW TOOLING GAP: wtrace hooks cpu->write_* (generated path) only — clean-C
  host-pointer writes to scratchpad are INVISIBLE (0 in-range writes across
  a full arcade run). Use bound-global counters / read_ram polling instead.
  ARCADE BLOCKER localized: battle_flow_state_machine (menu.c:553) drives
  ph=0x1f800001 0->1->2->3->4 switching on DAT_1f80009c (mode/sub). Arcade
  pins at gs=8 ph=0 before phase cells engage. Prime suspect: br2_bail_pending
  reset loop (menu.c:583/630) or select_screen_tick/battle_frame not
  returning. Versus reaches real combat (rounds resolve). Full writeup:
  proof/arcade_fight_entry_phase_analysis.md. Next: call_bail at stall.

- **session CORRECTION (arcade wedge = runtime gaps, NOT game/*.c):**
  Re-read prior proof docs — arcade fight-entry is a documented multi-wedge
  chain already root-caused, and it is NOT a clean-port defect. Wedge A
  (arcade_cpu_phase_seeding, H1 CONFIRMED): CPU opponent phase battle+0xb6
  stays 0 because its select-confirm rides the pad-2 decode channel
  (0x801befca->battle+0x6a edges); headless has no controller-2. Only
  two-human versus (9b=2, seeds both slots) works; every CPU mode wedges
  identically. Wedge B (cd_deferred_irq_starvation): after phases hit 0x300,
  a battle-asset streaming read's DeliverEvent never reaches cd_result_wait_a
  -> TestEvent spins -> battle_init never returns; CD deferred-coroutine
  IRQ/event starvation in psxrecomp runtime. Clean C tree is faithful; the
  blockers are faithful-core runtime (input-device model + CD coroutine
  scheduling). Updated proof/arcade_fight_entry_phase_analysis.md.

- **session N+101 (arcade wedge deep-dive; CORRECTION + mechanism pinned):**
  CORRECTION to my earlier "versus reaches real combat": the 0x700 combat
  phase NEVER fired on versus either — the 0300->0500/1400->1100->0100
  transitions I logged are pre-combat announce/round-boundary phases, not
  combat. Consistent with N+40/N+81/N+88: NO mode reaches sustained 0x700.
  RESOLVED this session: the ONLY 0x700 phase-cell writer is `sh`@guest
  0x801538B8 inside battle_config_eval (clean menu.c:~4235), gated behind
  phase classes 0x000/0x600 — a faithful wait on upstream progression, NOT
  a missing function (proof/arcade_fight_entry_phase_analysis.md).
  Wedge A narrowed to TWO clean-verified phase gates in the select path:
  (1) 0x100 seed via mc_slot_action case-0 needs the slot's pad EDGE
  (+0x68/+0x6a) — CPU slot-1 needs pad-2; (2) 0x300 confirm via
  select_confirm_handler from select_screen_tick per-slot puVar6+8 flags —
  CPU record's button-flag source (internal cursor-AI vs pad-2) UNRESOLVED.
  Wedge B mechanism pinned deeper: deferred cooperative switch
  (interrupts.c:1748-1793) requires s_last_interrupt_check_pc to be a
  materialized, 4-aligned, psx_is_dispatchable block-leader PC; br2_irq_poll
  passes cpu->gpr[31] ($ra, a call-site return addr — NOT dispatchable) and
  clean-C has no mid-body guest resume point, so a deferred ChangeThread
  detected while a clean-C loop polls can never be honored -> the CD/BIOS
  kernel thread starves. Reconciles with cd_deferred_coroutine_starvation:
  cd_response_pump (handler[2]) = 0 hits — CD IRQ bit2 raised but cleared
  before cd_intc_dispatch polls (INTC ack ordering) OR IRQ never reaches
  the poll window; TestEvent spins 2M+; channel cursors frozen.
  NEXT (faithful, high-risk — interrupts.c/cdrom.c): reproduce the wedge
  via versus-CPU (9b=1, same CD path as arcade, easier nav than 1P) then
  audit (a) INTC I_STAT ack ordering so cd_response_pump sees bit2, and
  (b) whether the deferred-switch resume-PC requirement can be satisfied
  for the kernel thread (NOT the clean-C game thread) or needs the §0
  cooperative-scheduler HLE replacement. Do NOT fake phase writes or
  inject CPU confirms. Instances healthy: 9804 @1.80M fr, 9811 @370k fr.

- **session N+102 (versus-1P live repro + CPU-input architecture traced; wedge
  isolated to SELECT-screen pad-2, NOT fight input):**
  HEADLINE CORRECTION to N+101: versus mode (DAT_1f80009b==2) DOES reach
  REAL sustained combat when both pad slots are fed — versus_fight_mash
  injects slot-0 AND slot-1 via the debug press command. Observed on 9821:
  slot phase cells 0x1f8000b4/b6 cycle 0x0300(announce)->0x1400/0x0500
  (in-fight)->0x1100/0x0100(round boundary)->0x0300(next round); rounds
  resolve. CD healthy: reading=1, ring_partial=19517, irq_present_max_late=0,
  ring_starved=0; bioscall B0/0x07 DeliverEvent=770 (events flowing — NOT the
  CD-starvation signature). frame advanced 29k->86k+. The 0x700 cell is a
  specific sub-state, not the whole combat phase — the 0x1400/0x1100 family
  are the real in-fight/round-boundary phases. (proof/
  cpu_input_architecture_select_wedge.md)
  CPU-INPUT ARCHITECTURE (all clean-ported, faithful): FIGHT input for
  CPU/demo fighters comes from fighter_script_fsm (battle.c:5927), a
  per-fighter script/AI state machine reading the fighter's +0xf script
  buffer and emitting command words -> fighter+0x7a/+0x7c -> pad_edge_scan/
  action_from_input. It BYPASSES pad-2 entirely — CPU fight input needs no
  controller-2 (attract demos fight autonomously). SELECT input is the
  opposite: pad_state_refresh decodes only the two hardware pad records
  (DAT_801befa8 pad-1 / DAT_801befca pad-2) — no CPU branch;
  mc_slot_action case-0 seeds only the pressed slot (+0xb4 pad-1 / +0xb6
  pad-2 edge 0x840) — no CPU auto-seed; fighter_spawn_setup case-0 builds
  slot-1 select buttons from battle+0x76/0x6e/0x6a = pad-2 words only.
  => The wedge is PRECISELY the select-screen slot-1 confirm requiring a
  pad-2 edge. No CPU-select mechanism exists in the clean tree.
  RESOLUTION candidates: (a) a GENERATED/unported function synthesizing
  pad-2 record data or calling mc_slot_action(1)/seeding +0xb6 for CPU;
  (b) real arcade HW providing pad-2; (c) a select-screen CPU cursor in
  generated code not yet ported. press(slot=1) works end-to-end (proven),
  so the pad-2->select->fight->combat chain is functional once slot-1
  input exists.
  OBSERVABILITY: ev_stream_dispatch now calls debug_server_poll() every
  0x400 opcodes (580f274) — server stayed responsive through attract AND
  the versus fight on 9821 (previously the clean-C for(;;) starved it
  permanently; sample showed battle_intro_flow->battle_frame_tick->
  ev_stream_dispatch->ev5_action_select hot loop).
  NEXT: find the CPU-select generator — grep generated/ for writers of the
  pad-2 record (0x801befca region) or +0x1f8000b6 seeding, and disassemble
  the pad-poll/SIO path for a CPU/demo-input branch. If found in generated,
  port it clean (it is the game's own faithful mechanism, not a fake).

- **2026-11-26 (BR2 GTE helper ports; no timing-core change):** Added clean
  dispatch for four generated GTE/math helpers at 0x8019457C, 0x80194614,
  0x8019466C, and 0x80194940, including the 0x80194678 CPS continuation.
  Debug-enabled headless verification on port 4507 advanced frames 2206 to
  8446 to 19726 without a crash; the runtime was then stopped. No timing-core
  behavior changed.
