# 0x801BCB94/98/A0 wild-dispatch — context-restore saved-PC corruption (runtime + static)

Crash family: `FAIL-FAST unknown dispatch addr=0x801BCB94 / 0x801BCB98 /
0x801BCBA0, ra=0x801A1EC0`, always `in_exception=1`, `bail_flattened=8`,
on the SIO/CD-poll path, at frames 29041 / 31233 / 49548 / 62137 (not
frame-deterministic — a timing race).

`0x801BCB94..0x801BCBA0` are fields inside the CD read-session command
block (`game/cd.c`, `DAT_801bcba0` = completed-sector counter). They are
pure BSS data, never code.

## What the value actually is

The bogus PC is **not** a code pointer and **not** a table/index read. It
is a *guest-kernel thread-context saved-PC* (`ctx+0x88`) that happens to
hold a pointer into the CD command block. The thread scheduler / kernel
`ReturnFromException`/`ChangeThread` ends `jr $k0` where
`k0 = *(ctx + 0x88)`; when that slot contains a CD-block pointer the CPU
dispatches data-as-code.

## Proven this session (3 instrumented crashes)

Freeze-dump sections added in `9b401e5`/`02ec83c` and captured live:

- **`tctxring` EMPTY (seq=0) on every crash** — `psx_save_context_to_tcb`
  and `psx_restore_context_from_tcb` (the *host* cooperative TCB
  scheduler, `traps.c:365/397`) **never ran**. The host scheduler is
  dormant for this game; all thread switching is the *guest kernel's own*
  recompiled `ChangeThread`/`RFE` (`jr $k0` in MIPS), which bypasses every
  host context function.
- **`badctxring` EMPTY on 9773** — the `psx_syscall` case-3 manual TCB
  restore (`traps.c:1060`) is also **not** the commit path. The resume PC
  is committed by recompiled guest `jr $k0`, not host C.
- **`wvalring`** (always-on store watch, `memory.c wval_note_write`):
  records every main-RAM word store whose *value* is a CD-block pointer.
  Decisive rows at the crash frame:
  - `pc=0x8019A478 -> 0x801FF194 val=0x801BCB8C`
  - `pc=0x8019A47C -> 0x801FF190 val=0x801BCB80`
  These PCs are the **prologue of `cd_timer_read` (`0x8019A460`)** —
  `addiu $sp,$sp,-40` then `sw $s1,28($sp)` / `sw $s0,24($sp)`. So
  `cd_timer_read` saves callee-saved regs holding CD pointers onto the
  **kernel stack at ~0x801FF178** (top of RAM).
  - Recurring per-frame stores `pc=0xBFC207A8 -> 0x8FEC val=0x801C0000`
    (BIOS) and `pc=0x28D0 -> 0x9058 val=0x801BEEDC` show the kernel
    constantly writes CD pointers into the **same low-RAM region as the
    TCB table** (TCB table base `*(0x110)=0x8FE0`, size `*(0x114)=0x300`,
    0xC0-stride entries).
- **`badvalcdptr`** (RAM scan for CD-pointer values): 145 cells hold
  CD-block pointers — the CD command block self-references plus kernel
  globals and TCB-adjacent slots (e.g. `0x80009058 -> 0x801BEEDC`).

## Mechanism

The guest kernel's thread context save area (`ctx+8` GPRs, `ctx+0x88`
resume-PC, matching `psx_save_context_to_tcb`'s layout) lives in low
kernel RAM interleaved with CD/SIO bookkeeping. During the IRQ-dense
SIO/CD poll, a context block is resolved whose `+0x88` slot contains a
CD-command-block pointer (saved there either by a guest `SaveContext`
that ran while a register held a CD pointer, or by a transient/deferred
context block aliasing the architectural TCB). `jr $k0` then commits the
data address as `cpu->pc`.

`in_exception=1` + nested IRQ delivery is the trigger window: the kernel
is mid-`ChangeThread`/RFE when the deferred CD/SIO work leaves a CD
pointer in the slot the restore reads as PC.

## Ruled out

- Host cooperative scheduler (`psx_save/restore_context_to_tcb`) — never
  invoked (tctxring empty).
- `psx_syscall` case-3 manual restore — not the commit (badctxring empty).
- `dev_cmd_round_dispatch` table/index — counter=1, table entries valid.
- `ra=0x801A1EC0` — stale `$ra`, not the dispatch source.
- Offset mismatch — host save (+0x88) and guest restore (+0x88) agree.

## Faithful fix direction (do NOT clamp the address)

Correct the **context ownership / save timing**, not the symptom:

1. Ensure `*(0x108) -> TCBH -> ctx` resolves to a real thread context
   block at every nested-IRQ `ChangeThread`/RFE boundary — a transient
   kernel-stack or CD-deferred context must not be treated as the
   architectural current context.
2. The `ctxchain` freeze-dump section (added `02ec83c`) reads the live
   `*(0x108)->TCBH->TCB->+0x88` chain at freeze and reports whether the
   resolved base is a real TCB-table entry (`0xC0`-aligned inside
   `[*(0x110),+*(0x114))`) or a stack/CD address — this names the guilty
   block on the next crash.
3. Audit the guest kernel's `SaveContext` for the CD/SIO worker thread:
   confirm the resume-PC slot receives the thread's real continuation PC
   and not a working register holding a CD-block pointer.

## Instrumentation added (all committed)

- `memory.c wval_note_write` — always-on ring of stores whose value is a
  CD-block pointer (writer PC + target + frame).
- `freeze_heartbeat.c` — `tctxring`, `badvalcdptr` (RAM scan),
  `wvalring`, `badctxring`, `ctxchain` dump sections.
- `traps.c` — `g_badctx_ring` latch on the case-3 path (proved negative).

## Update — restore stub identified statically (OpenBIOS_func_00003554)

The guest kernel commit path is `OpenBIOS_func_00003554`
(`generated/OpenBIOS_full.c` ~line 232872; referenced ROM addrs
`0xBFC2167C`–`0xBFC2169C`). It is a minimal IRQ/exception-return
trampoline that restores only the clobbered regs and jumps to the saved
resume PC:

```
k0 = 0x100                      ; gpr[0]+256  ($zero+0x100)
k0 = *(k0 + 8)                  ; = *(0x108)  -> TCBH cell
k0 = *(k0 + 0)                  ; = *(*(0x108)) -> current ctx base
at = *(ctx + 12)
v0 = *(ctx + 16)
v1 = *(ctx + 20)
ra = *(ctx + 132)
k0 = *(ctx + 136)               ; = *(ctx + 0x88)  <- resume PC slot
pc = k0                         ; jr $k0
```

So the committed PC is `*(*(*(0x108)) + 0x88)`. The `ctxchain` dump reads
exactly this chain (`p108=*(0x108)`, `pcb=*p108`, `epcslot=pcb+0x88`,
`epcval=*(pcb+0x88)`).

`wvalring` shows `cd_timer_read`'s prologue (`0x8019A478/7C`) storing
`$s0/$s1` (CD-block pointers `0x801BCB80/8C`) into the kernel-stack
region `0x801FFxxx`. Hypothesis to confirm via `ctxchain.pcb`: the
resolved `*(*(0x108))` context base lands in the `0x801FFxxx`
kernel-stack/exception-frame region (where a nested IRQ frame overlaps
the CD-pointer spill) rather than a real TCB-table entry (`0x8FExx`),
so `+0x88` reads a CD data pointer instead of a code resume-PC.

9774 repro instance (ctxchain-armed) is live in `battle_frame_tick`
verified via stack sample; awaiting the ~30-60k-frame crash window to
capture `ctxchain`.

## Update — save-side + nested-IRQ mechanism (subagent static analysis)

**Save side (who writes ctx+0x88):** exactly two writers.
- `0xBFC207EC` (`sw $v1,128($k0)`, OpenBIOS_func_000027AC/00002810,
  interrupt/exception ENTRY): stores COP0.EPC (+4 syscall fixup) — always
  a code addr on real HW.
- `0xBFC068D8` (OpenTh/get_free_TCB_slot): stores caller-supplied entry
  PC `a0` — a raw arg store; a bad `a0` seeds a data ptr that persists
  until first real exception save.

**Ownership:** `*(*(0x108))` = `TCBH_Ptr[0].entry` is architecturally
ALWAYS a TCB-table entry in `[*(0x110), *(0x110)+*(0x114))`
(`[0x8FE0,+0x300)`, 0xC0-stride). Never kernel-stack. So a stack/CD base
in `ctxchain.pcb` means `TCBH[0].entry` itself was corrupted by a bad
store, not a deref-mechanics error.

**Nested-IRQ mechanism (prime suspect):** host delivers nested IRQs —
`interrupts.c:1484-1490` only blocks at `nest_depth>=2`, so depth 1->2
IS allowed. The guest exception-entry context save
(func_000027AC->func_00002810) keeps the ctx base in `k0`/`k1` across
`psx_check_interrupts_at` block boundaries (~200-insn window). A nested
IRQ2 mid-save re-runs the SAME save into the SAME TCB, and IRQ2's RFE
(func_000029CC `lw $k0,128($k1); jr k0`) CLOBBERS k0 — IRQ1's remaining
stores then write to the wrong base. On real HW the exception entry
clears SR.IE so this window is non-interruptible until the handler
re-enables; **if the host delivers depth 1->2 inside the save window,
that is itself a faithfulness bug producing the TCB clobber.**

**Faithful fix direction (not a clamp):** make the exception-entry
context-save window non-interruptible to match SR.IE semantics — either
the guest handler genuinely re-enables IE only after save completes
(then host must honor that) or host must not deliver nested IRQ while
the outer handler's save is mid-flight. Audit `psx_check_interrupts_at`
nesting vs the guest SR/IE state; preserve k0/k1 context base across any
legitimate nested delivery.

**Next instrumentation:** watchpoint on writes to `TCB[i]+0x88` for all
4 slots to catch the exact writer (EPC-save vs thread-init vs wild
store); `ctxchain.intable` on next crash distinguishes TCB-entry
corruption from slot overwrite.
