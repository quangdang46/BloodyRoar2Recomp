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
  IRQ dispatcher `0x16E4` calls `0x2610` per pending bit with handle
  `a0=0xF0000003`(CD bit2), `0xF0000009`(bit0x200), `0xF0000002`, `0xF0000010`,
  `a1=0x1000`. For CDROM it needs an object `class==0xF0000003 && attr==0x1000
  && f4==0x2000 && size==0x2000` — **NO such object exists in the table** →
  `jalr` never reached → game's CD IRQ callback never invoked → stall.
  The `0xF000000X` IRQ-source objects are a SEPARATE registration from the CD
  channel objects (0xF4000001/0xF0000011) and the mark EvCB — likely created by
  a kernel interrupt-source register call during CD/IRQ init that either never
  ran or wrote to a different table. NEXT: find the alloc that creates class
  `0xF000000X` IRQ-source objects (candidates: a BIOS/kernel "register int
  source" svc, `cd_subchan_table_install`, or the IntRP setup `0x801FEB40`)
  and why it didn't populate `0x8e08`. VBLANK works → its IRQ-source object
  must exist SOMEWHERE; dump which table/class VBLANK's handle resolves to for
  comparison. This is faithful-core kernel-object registration, NOT a game hack.

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
