# battle bring-up fatal halt — wild PC in kernel/thread context (post-u16-fix)

Date: this session. Reproduced **twice** on fresh headless instances
(`game_1pad.toml`, arcade nav). Both runs fatal-halted at ~frame 1700
during `battle_frame` DAT1=3 (fighter bring-up / task-pool tick), *before*
reaching the previously-documented DAT1=8 wedge.

- Run A: port 9941, halt frame ~1626, wild target `0x0000EE93`
- Run B: port 9942, halt frame ~1755, wild target `0x0000E23E`

The wild value differs per run but the crash site/class is identical:
`DISPATCH FATAL: misaligned target 0x0000Exxx`, `cpu->pc=0`,
`$ra=<same wild value>`, `COP0_EPC=0x8019A648`.

## Why this is new

Commit `1fd2a9b` fixed the `0x1F800082` sim/pause gate width (u16, was u32).
That unfroze the `LAB_80137acc`/`LAB_80137cd0` channel-script interpreter
tasks — they now actually run their timed-op scripts. Prior wedged
instances (DAT1=8) were built **before** that fix, so the script VM was
inert and the game limped to DAT1=8 differently. The script/task path
unlocked by the fix now executes and hits this crash earlier, at DAT1=3.

## Crash forensics (port 9942, post-mortem via live debug server)

### Register/dispatch state

```
DISPATCH FATAL: misaligned target 0x0000E23E
  cpu->pc:   0x00000000
  $ra:       0x0000E23E          <- return addr IS the wild value
  $t9:       0x000000B0          <- B0 syscall vector region
  COP0_EPC:  0x8019A648          <- inside vsync_wait_n (0x8019a5d8)
```

`RAM[0x801FFE58] = 0x0000E23E` — the wild PC sits in a guest stack slot
in the `0x801FFExx` frame region (battle_frame's guest frame / kernel
thread stack area).

### sp_ring tail (last ~30 entries, all "normal")

Every frame ends with the same 3-call cycle:

```
target=0x8019A53C  ra=0x8019A53C   sp 0x801BDD30 -> 0x801FFE18/0x801FFDF8
target=0x00002924  ra=0x0000282C   sp -> 0x00008548        (kernel-space)
target=0x8019A87C  ra=0x8019A87C   sp 0x00008548 -> 0x801BDD30
```

`0x8019A53C`/`0x8019A87C` are in the CD/timer driver block
(0x8019axxx); `0x2924`/`0x282C`/`0x8548` are low-RAM OpenBIOS kernel
thread/trampoline addresses. The fatal `jr`/`jalr` to `0xE23E` is a
**register jump inside generated code** — it never passed through the
sp_ring `jal` tracker, so the exact jump site is not in the ring.

### dispatch_tail

The last 64 dispatches are all small (<0x10000) kernel-space PCs —
normal OpenBIOS trampoline/kernel dispatch chain, not data-as-PC. The
chain simply ends at the wild target.

### Key memory cells (post-mortem)

- `0x801BDDE0 = 0x00000631` — the VBlank frame counter
  (`cd_cb8_run_param` writes `param_1+1` here each VBlank via
  `cd_cb8_dispatch`). Value 0x631 = 1585 — it advanced normally then the
  CPU halted; it is **not** zero/stuck-at-init.
- `0x801BDDE0` region otherwise sane (DICR/callback cells nearby normal).

## Mechanism assessment

`vsync_wait_n` (0x8019a5d8, delegated to generated) polls
`DAT_801bdde0` with a timeout; `cd_timer_read`'s comment (game/cd.c:376)
already documents the contract: the counter is advanced by
`cd_cb8_dispatch` on the **VBlank IRQ** through the deferred-coroutine
path (`cd_ctx_restore -> cd_intc_dispatch -> cd_cb8_dispatch`).

The crash signature — a wild `$ra`/PC materializing in a guest stack
slot after a kernel thread context switch — matches the documented
hazard class in `psxrecomp/CLAUDE.md` §0: *"the host
coroutine/fiber cooperative-thread scheduler"* non-determinism, i.e. a
TCB/context restore supplying a bad PC, or a deferred-IRQ coroutine
resume clobbering `$ra`.

The repeating per-frame triple above is the normal CD-pump ↔ kernel
wait handshake; it ran cleanly for ~1750 frames then one iteration's
context restore produced `0xE23E`.

## What is NOT the cause (re-confirmed)

- Not the DAT1=8 spawn-phase wedge — this halt is earlier (DAT1=3) and
  pre-empts it. Whether the DAT1=8 stall still exists downstream is
  unknown until this crash is fixed.
- Not CD data/IRQ delivery — `cd_cb8_dispatch`/frame counter advanced
  to 0x631; CD path was healthy through the halt.
- Not input — no pad edge involved; crash is input-independent.

## Next step

Trace the kernel-thread context switch / deferred-coroutine resume that
produced `$ra=0xE23E`: which TCB/ctx was restored, what wrote
`0x801FFE58`, and whether a clean-C caller (`task_pools_tick ->
call_guest -> vsync_wait_n`) supplied a frame the coroutine resume
disagrees with. Suspect the same guest-frame/`$ra` contract that
`br2_guest_frame`/`br2_call_generated` manage — a nested generated call
inside the clean battle loop may leave `$ra`/`sp` inconsistent for the
IRQ coroutine's restore.

## Mechanism assessment — REFINED (same session, deeper trace)

`freeze_check` on the halted instance showed `current_func=0x000029CC` —
the CPU died **inside the kernel context-restore** (`ReturnFromException`/
`ChangeThread` epilogue at guest `0x29CC`), which ends `jr $k0` where
`k0 = *(TCB + 0x88)` = the saved `resume_pc`
(`psx_restore_context_from_tcb`, traps.c:399 `gpr[26]=read_word(save+128)`).

- `pczero_count=4`, `pczero_addr=0x8014AEF4` (epilogue of generated fn
  `br2eu_func_8014AAE0`, `lw ra,0x54(sp); jr ra`), `pczero_epc=0x80193184`
  (CD region). The restore jumped to `pc=0` **four times and recovered**,
  then once produced `0xE23E` → `misaligned target` → fatal.
- All dedicated latches read zero: `rfe29cc_null_count=0`,
  `irqanom_count=0`, `tcbsave_zero_count=0` — so the poisoned save had a
  **nonzero, non-sentinel** `resume_pc` (`0xE23E`), slipping past both
  guards in `psx_save_context_to_tcb` (traps.c:346).
- Wild values `0xE23E`/`0xEE93` are ~`0xExxx` — suspiciously like the low
  16 bits of `0x801FExxx` stack addresses; a truncated/stack-derived PC
  is a candidate shape.
- traps.c:411-414 already documents this failure class: *"Switching
  mid-nest saves a poisoned TCB context (latched PC ahead of the live
  registers) → the resumed thread dispatches a jumptable with a smeared
  index → misaligned DISPATCH FATAL."* The deferred-switch fix covers the
  known path; this crash is a residual variant — likely a `resume_pc`
  sourced from `g_dirty_safe_resume_pc` /
  `s_compiled_interrupt_resume_pc` while the interrupted context was a
  **clean-C call** (`task_pools_tick → call_guest → vsync_wait_n`) whose
  guest `cpu->pc`/resume state is not a normal block boundary.

## Next step

Trace which save produced `resume_pc=0xE23E`: arm the
`psx_save_context_to_tcb`/`psx_restore_context_from_tcb` path (or
`thread_ctx_ring`) on a repro and dump the TCB + resume_pc at the fatal
restore; identify whether the poisoned value came from a stale
`cpu->pc`, a corrupted TCB slot, or a wrong-TCB selection
(`psx_current_tcb_ptr`). The fix is in the runtime's resume-PC selection
for IRQ delivery inside nested clean→generated calls — not game logic.

## Live-specimen forensics (ports 9942 + fresh 9960, frame ~1650)

Both instances reproduce identically and are caught LIVE-SPINNING at
`current_func=0x29CC` (`dispatch_count=0`, `pc=0`, dirty-RAM blocks still
advancing) — the fatal `jr` has not fired yet; `$ra` already holds the
wild value.

`get_registers` (9960): `$ra(gpr31)=0x0000EE93`, `$sp=0x801FFE60`,
`$k0=0`, `$k1=0`, `cop0_epc=0x8019A648`, `sr=0x40000401`.
Stack `0x801FFE58 = 0x0000EE93` — the exact same value as `$ra`, i.e.
`$ra` was loaded from this slot (`sp-8`). Neighbors are a clean BIOS
call chain: `0x801FFE40=0xBFC09B83`, `0x801FFE4C=0xBFC049E0`,
`0x801FFE54=0xBFC04D74`, `0x801FFE7C=0xBFC00000` — so `0x801FFE58`
should hold another `0xBFC0xxxx`/`0x801xxxxx` return address. Prior run
(9942) had `0x0000E23E` at the same slot — value nondeterministic,
location + timing deterministic.

`sp_ring` shows the per-frame cooperative schedule: main thread resumes
at `0x8019A53C` (= `cd_timer_read` generated body, the
`DAT_801bdde0`/`vsync_wait_n` frame-wait loop) on stack `0x801FFE18+`,
TCB `0x8FE0`. The current TCB save area (`0x8FE8+`) is CLEAN:
`resume_pc=0x8019A648`, saved `ra=0x8019A53C` — so the wild `$ra` is
NOT from a poisoned TCB restore; it is written into the main thread's
stack frame downstream of resume.

`thread_ctx_ring` total=0 → host `psx_save/restore_context_to_tcb` never
ran; the whole path is guest-side (recompiled OpenBIOS + generated game).

## Narrowed conclusion

The defect is a **full-word write of a ~`0x0000Exxx` value into the
saved-`$ra` slot `0x801FFE58`** of the main thread's frame inside the
`cd_timer_read → vsync_wait_n → BIOS-wait` call chain, at ~frame 1650
during `battle_frame` DAT1=3 bring-up. `0xExxx` ≈ low-16 of an
`0x801FExxx` stack/code address → candidate shapes: (a) u16-truncated
pointer store, (b) `sh`-for-`sw` codegen width bug, (c) sp/frame drift
making `lw ra` read a data slot, (d) faithful guest read of upstream-
corrupted data. NOT a TCB/resume_pc/kernel-restore fault (that path is
clean and unused here).

## Next decisive step

Catch the writer of `0x801FFE58`: needs a conditional memory-write
breakpoint (value in `0x0000E000–0x0000EFFF`, high16==0) or a
`sw`-site audit of the generated epilogues in the `0x8019A4xx–0x8019A8xx`
cluster + the OpenBIOS wait/ChangeThread frames that own stack
`0x801FFE18–0x801FFE60`. Repro is deterministic (~frame 1650) on a
fresh `game_1pad.toml` + `arcade_fight_mash.py` drive.

## WRITER + READER CAUGHT (rwatchpc + wtrace, port 9970, frame ~1632)

New instrumentation (`rwatchpc_arm/rt` read-PC recorder + existing
`wtrace`) caught both sides on a live repro:

**WRITER — `sw v0,16(sp)` at guest `0x8019A48C`** (cd_timer_read
prologue, `generated/SLES_017.22_full_46.c:24180`, opcode `0xAFA20010`).
cd_timer_read frame `sp=0x801FFE48`; `sp+16 = 0x801FFE58`. It spills
`v0 = **0x801CCCAC` — a double-deref whose final pointer is
`a1 = *0x801CCCAC = 0x1F801110` = **T2_COUNT** (PSX timer-2 counter).
So `v0 = *0x1F801110` = a raw timer count (`0x0000EE93`, `0x0000618D`,
`0x00006354` — all benign timer snapshots). **The spill itself is
faithful and harmless; the value is NOT a corrupted pointer.**

wtrace shows `0x801FFE58` received ONLY these v0 spills (4 writes total:
seq 133/134 @f872 `0x6354→0x618D`, seq 1881/1882 @f1194 `0x618D→0xEE93`).
**No context-save ever wrote a real ra there** → the restore reads a
slot that never held a return address.

**READER — `lw $ra` (rt=31) inside `OpenBIOS_func_000029CC`**
(context restore / ReturnFromException-ChangeThread tail). rwatchpc
recorded exactly ONE ra-load of `0x801FFE58` in the whole run — the
crash consumer (cpu_pc=0, func=0x29CC, frame 1632).

**Mechanism — kernel-stack/context-frame collision:**
`sp_ring` shows the cooperative schedule cycling sp across
`0x801FFE18 / 0x801FFDF8` (kernel stack, top-of-RAM), `0x801BDD30`
(game/CD stack), and `0x00008548` (low-mem BIOS trampoline stack),
all under `tcb=0x8FE0`. The 0x29CC restore resolves its context base
`k1 = *(*0x108)` → thread kernel-stack context ≈ `0x801FFDDC`, whose
saved-ra slot lands at `0x801FFE58` — **the same address cd_timer_read
uses for its v0 spill**. cd_timer_read runs inside the CD/IRQ deferred
coroutine on the kernel stack; its 40-byte frame overlaps the
exception/context save area. When the coroutine yields/switches, the
context restore reads the spill (timer count) as `$ra` → wild PC →
`0x29CC` livelock.

**This is a stack-frame overlap between the deferred-IRQ coroutine's
guest frames and the kernel exception/context save area — a runtime
stack-management defect, not game logic and not a TCB-resume_pc fault.**

## Refined next step

Determine why cd_timer_read's frame (`sp=0x801FFE48`) overlaps the
kernel context-save slot (`0x801FFE58`): trace the context-base `k1`
resolution in `0x29CC` at the crash and the kernel-stack pointer stored
in the TCB (`0x8FE0` region) vs the sp the deferred coroutine runs at.
Candidate fixes live in `interrupts.c`/`traps.c` deferred-coroutine
stack selection — the guest frames and the BIOS context frames must
not share addresses.

## Context-restore layout decoded (OpenBIOS_full.c)

`0x29CC` (`OpenBIOS_func_000029CC`) is the FULL kernel-context restore:
`k1 = *(*0x108)` → `k1 += 8` → reads saved GPRs/status/EPC at
`k1+4..k1+144`, `lw $ra` at `k1+124`, `jr $k0` at `k1+128`, then `rfe`.
Its counterpart `0x27AC` (`OpenBIOS_func_000027AC`) is the SAVE side —
same `*(*0x108)` chain, `sw` regs to `k0+8+offsets`. Both match the
host `psx_save/restore_context_from_tcb` layout (traps.c:346-404):
`save=tcb+8`, gpr[i]@`save+i*4`, resume_pc@`save+128`, sp=gpr[29]@`save+116`.

Live TCB dump (port 9970, `0x8FE0`): **the main-thread saved context is
CLEAN** — saved sp `0x905C=0x801FFDF8`, ra `0x9064=0x8019A53C`,
resume_pc/k0 `0x9068=0x8019A600`, status `0x9074=0x40000404`. So the
wild restore is NOT the main TCB thread-switch.

For the wild `lw ra`@`0x801FFE58`, `k1+124 = 0x801FFE58` →
`k1 = 0x801FFDDC` → `*(*0x108) = 0x801FFDD4` — i.e. the current-context
pointer resolved to a **kernel-stack address**, not the `0x8FE0` TCB.
Either (a) the current-TCB slot (`0x8FD0`) transiently held a stack
pointer (deferred-coroutine/exception temporary context), or (b) a
nested IRQ frame on the kernel stack is the restore target and its
saved-ra slot genuinely overlaps cd_timer_read's spill. The decisive
missing datum is the crash-time value of `*(*0x108)` / `k1` inside
`0x29CC` — needs a k1 watchpoint or an `rwatchpc`-style recorder on the
`lw k1,8(0x108)` / `lw k1,0(k1)` loads (phys `0x108`, `*(0x108)`).

## CRASH IS FLAKY / TIMING-DEPENDENT (port 9980)

A fresh run with `wtrace` on `0x8FD0` + `rwatchpc` on `0x8FD0`(rt=27)
+ `0x801FFE58`(rt=31) armed **did NOT crash** — sailed past frame ~1630
to `FIGHT REACHED` @3852 and was still advancing @4202 (battle phase
`0x1F8000B4=0x0300`). `rwatchpc` on the current-TCB slot `0x8FD0`
recorded **4264 reads, ALL `val=0x00008FE0`** (clean TCB) from
`func=0x29CC` — the context-restore runs every frame for scheduling and
always resolved cleanly this run.

So the `*(*0x108) → stack-pointer` corruption is **transient/racy**,
not deterministic logic — consistent with a timing-sensitive overlap
between the deferred-IRQ coroutine's guest frames and the BIOS
context-save area (an IRQ landing at the unlucky sp). Three prior runs
crashed ~frame 1630; this instrumented run dodged it (instrumentation
perturbs timing → Heisenbug). The faithful fix is still required — the
collision is real when the timing hits.

## CRASH-MOMENT READ TRACE (crash_9990.json, deterministic repro)

`tools/crash_hunt.py` reproduced the halt **6/6** at frame ~1560-1600 with
identical signature (`ra=0x0000EE93`, `sp=0x801FFE60`, `epc=0x8019A648`).
Not flaky under this harness — the earlier "flaky" read was instrumentation
perturbation on a different path.

`rwatchpc` newest entries (func=`0x29CC`, rt=31 = `$ra` restores), frame 1572:

```
seq  phys      val        note
33002 0x1FFE18 0x801926AC valid saved-ra
33006 0x1FFE10 0x8019A518 valid saved-ra
33007 0x1FFE10 0x8019A53C valid saved-ra
33009 0x1FFE10 0x80192F40 valid saved-ra
33013 0x1FFE18 0x801926AC valid saved-ra
33015 0x1FFE58 0x0000EE93  <-- WILD (timer spill)
```

The restore walks a **kernel-stack context frame** (~`0x801FFE00`–`0x801FFE60`)
and every saved-reg slot returns a plausible return address EXCEPT `0x1FFE58`,
which `cd_timer_read`'s `sw v0,16(sp)` overwrote with a T2 count. So both the
SAVE (`0x27AC`) and RESTORE (`0x29CC`) used the SAME transient stack-addr
context pointer `*(*0x108)` — i.e. during this exception/IRQ window the
current-context chain is redirected onto the kernel stack, and the BIOS
context-save area collides with a live `cd_timer_read` frame.

This refines the model: it is NOT the main TCB (`0x8FE0`) and NOT a one-off
bad pointer — it is a **nested/temporary context living on the kernel stack**
whose saved-`$ra` slot (`ctx+124 = 0x801FFE58`) is exactly `cd_timer_read`'s
`sp+16` spill. The faithful fix must keep the exception context off the
transient kernel stack (or give the deferred-coroutine frame a non-overlapping
stack), in `interrupts.c`/`traps.c` context management — not in game code.

## ROOT-CAUSE REFINEMENT (crash_9990-9995, wtrace/rwatchpc stack-window capture)

The earlier "`0x29CC` restores a kernel-stack ctx frame" reading was a
**stale-attribution artifact**: `rwatchpc`/`wtrace` stamp `func` from
`g_debug_current_func_addr`, which game code never updates — `cpu_pc` is
`0x00000000` on every generated load/store, so `func=0x29CC` only means
"last BIOS entry was 0x29CC", NOT that the access ran inside it.
`func_000029CC`'s prologue can only ever read `k1+124 = 0x9064` (main TCB);
it provably did (`gwatch` shows **zero** writes to `0x108`/`0x8FD0`/`0x8FC0`
pages after frame 28, and the rt=27 k1-chain always resolves
`0x108→0x8FD0→0x8FE0→0x9054` cleanly at the crash frame).

**The real reader** is `vsync_wait_n` (`0x8019A5D8`, `game` CD/timer module)
epilogue at `0x8019A660`: `lw $ra,24($sp); addiu $sp,$sp,32; jr $ra`.
With `sp=0x801FFE40` the saved-ra slot is exactly `0x801FFE58` — matching
the crash `sp=0x801FFE60` (= caller sp after the `+32`).

**The writer** is `cd_timer_read` (`0x8019A460`) `sw $v0,16($sp)` at
`0x8019A48C`: with `sp=0x801FFE48` it writes the T2 timer count
(`a1=0x1F801110`) to `0x801FFE58`. `wtrace` on the tight slot
(`0x1FFE50–0x1FFE64`) captured every hit: timer-count writes at frames
320/454/666/872/1194 (`0xB8A9,0x5DEF,0x6354,0x618D,0xEE93`); `0xEE93` then
sat untouched ~400 frames until `vsync_wait_n`'s epilogue read it at the
crash frame (~1560–1620, 6/6 identical signature).

**Call geometry:** `cd_timer_read` `jal`s `vsync_wait_n` (ra=`0x8019A53C`),
and both run on the **kernel stack** (top-of-RAM `0x801FFE…`) as BIOS
IntRP/callback frames — `cd_timer_read@sp=0x801FFE60` →
`vsync_wait_n@sp=0x801FFE40` (ra slot `0x801FFE58`). A second,
shallower `cd_timer_read` activation at `sp=0x801FFE48` (ra=
`0x80196908/0x80196954`, a different caller) overlaps the same slot.
The overlap requires **nested-exception frame reuse of `KernelSp`**: the
BIOS `ExceptionHandler` does `sp = KernelSp` for every delivery, so a
nested IRQ taken while `vsync_wait_n`'s frame is live re-uses the same
kernel-stack addresses and the nested callback's spill clobbers the
outer callback's saved `$ra`.

**Faithfulness status:** nested IRQ delivery when the guest re-enables
`IEc` inside a handler is *intentional* (`interrupts.c:1742-1786`,
Tomba2 precedent). The collision is genuine guest stack behavior *given*
the delivery cycle — so the defect is most likely **IRQ delivery timing**
(a nested timer/vblank IRQ landing inside the `vsync_wait_n` wait window
at a cycle real HW would not take it), not a context-pointer bug. The
`func=0x29CC` trail was a red herring.

**Next step for the fix:** compare IRQ delivery cycles against the Beetle
oracle (`find_first_divergence`) around frame ~1194/1560, or audit whether
the timer-1 (`0x1F801110`) IRQ that drives the nested `cd_timer_read`
activation is delivered at a hardware-accurate deadline vs the
root-counter model — the flaky ~frame-1560 alignment is a cycle-timing
race, not a logic error in `interrupts.c`/`traps.c` context management.
