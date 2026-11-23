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
