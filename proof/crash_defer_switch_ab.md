# BR2 frame-~1600 fatal halt — deferred-switch honor corrupts shared kernel-stack frames

## Verdict on the cd_ctx_save / saved-SP hypothesis: FALSIFIED

`proof/crash_9101.json` `mem_ctx` shows the CD deferred context at
`0x801BCD50` holds `sp = 0x801BDD30` — the CD worker **thread** stack, not an
IRQ/kernel-stack address (`0x801FFE18`/`0x801FFDF8`). `rwatchpc` on
`0x801BCD54` confirmed `cd_ctx_restore` (BIOS fn `0x00002970`) repeatedly
loads `0x801BDD30` into `$sp`. The saved context is correct; the coroutine
resumes on the right stack.

## The actual collision (proof/crash_9301.json)

Two activations share the main/kernel stack window `0x801FFE40–0x801FFE80`:

- `vsync_wait_n` (`0x8019A5D8`) keeps a live frame with its saved-`$ra` slot at
  `0x801FFE58`; its epilogue `lw $ra,24($sp)` at `0x8019A660` runs with
  `sp = 0x801FFE40`.
- A nested IRQ-side chain `cd_result_wait_a` (`0x801968CC`) →
  `cd_timer_read` (`0x8019A460`) runs with `cd_timer_read` frame `sp =
  0x801FFE48`; its spill `sw $v0,16($sp)` at `0x8019A48C` writes
  `0x801FFE58` — the same slot — with timer-derived values
  (`0x00005CE4 → 0x0000E69E` at frame 1192, again later).

`wtrace` on `0x1FFE40–0x1FFE80` shows **every** write to `0x1FFE58` is either
the `cd_timer_read` spill (`pc=0x8019A48C`) or a one-time BIOS burst at frame
170 — `vsync_wait_n`'s own prologue store is not in the retained window
because the crashing activation is a *resumed* coroutine whose frame was
established earlier. Final regs: `ra=0x0000E69E`, `sp=0x801FFE60`,
`pc=0`, `EPC=0x8019A600` — fatal dispatch on the wild timer value.

## Root cause: the deferred in-exception thread-switch honor

`irqctx_ring` shows IRQs preempting `vsync_wait_n` at `0x8019A600/0x8019A648`
with `entry_sp=0x801FFDF8` every frame, all `same_thread=1`. When the guest
BIOS handler's RFE moves `PCB[0]` to another thread inside the exception,
interrupts.c defers the host-side honor (`s_defer_switch_*`,
`PSX_DEFER_SWITCH`) to the next outermost block-leader boundary and
**re-saves the outgoing thread there** (`psx_sched_save_context`,
interrupts.c ~line 1770) with `resume_pc = s_last_interrupt_check_pc`.

That re-save captures the thread at a *later, shallower* point on the shared
kernel stack than the interrupt-time context the guest BIOS itself recorded.
The frozen frame slots (`0x801FFE40–0x801FFE70`) are then reused by subsequent
IRQ-side activations (`cd_result_wait_a → cd_timer_read`), which overwrite
the saved-`$ra` slot. When the deferred thread is restored it resumes
mid-`vsync_wait_n` at the shallow SP and the epilogue loads the clobbered
value → wild `ra` → `pc=0` fatal.

The immediate-switch path (the pre-defer baseline, and what real hardware
does — the exception entry context is the architectural save) keeps the
guest's interrupt-time TCB context, whose deeper frames sit *below* the
reused lane → no collision.

## A/B proof (all with `tools/arcade_fight_mash.py` driving, HLE TCB scheduler ON)

| Run | Setting | Result |
|-----|---------|--------|
| crash_9101/9301 | default (`defer_switch` on) | deterministic FAIL-FAST ~frame 1560–1642 |
| port 9601 | `PSX_HLE_SCHEDULER=0` (LLE host fibers) | alive, frame 67091+ |
| port 9701 | `PSX_DEFER_SWITCH=0` | alive, frame 46763+ |
| port 9801 | `defer_switch = false` in `game_1pad.toml` (this fix, no env) | alive, frame 67637+, CD streaming healthy (`irq_present_max_late=0`, seq 42257) |

`PSX_DEFER_SWITCH=0` isolates the deferred honor as the corruptor while
keeping the deterministic TCB scheduler — the smaller, targeted delta.

## Fix

New `[runtime] defer_switch` config key (default `true`, preserving the
Ape-memcard fix globally), plumbed through `config_loader.{h,cpp}` →
`psx_defer_switch_set_default()` → `defer_switch_enabled()` in
`interrupts.c` (`PSX_DEFER_SWITCH` env still wins). `game_1pad.toml` sets
`defer_switch = false` for Bloody Roar II.

Note: this selects the immediate (guest-faithful RFE-boundary) honor for
this title. The deferred re-save itself remains suspect for other titles —
its honor-time SP semantics on the shared kernel stack should be revisited
against the Beetle oracle (whether the outgoing thread should keep its
interrupt-time context instead of the boundary-time re-save).

## Artifacts

- `proof/crash_9101.json`, `proof/crash_9301.json` — decisive dumps
  (mem_ctx saved-SP `0x801BDD30`; wtrace collision writes; irqctx ring).
- `proof/emu_9601.log`, `emu_9701.log`, `emu_9801.log` — surviving runs.
- Code: `psxrecomp/runtime/src/interrupts.c`,
  `psxrecomp/runtime/include/interrupts.h`,
  `psxrecomp/recompiler/src/config_loader.{h,cpp}`,
  `psxrecomp/runtime/src/main.cpp`, `game_1pad.toml`, `tools/crash_hunt.py`.
