# CPU-input architecture: versus reaches real combat; wedge isolated to select-screen pad-2 dependency

Session: versus-1P live reproduction on port 9821 + full input-path trace.

## Headline correction

**versus mode (`DAT_1f80009b == 2`) reaches REAL sustained combat** when both
pad slots are fed. `tools/versus_fight_mash.py` drives slot-0 AND slot-1 via
the debug `press` command (per-SIO-slot override). Observed live:

- slot phase cells `0x1f8000b4`/`0x1f8000b6` cycle through genuine battle
  states: `0x0300` (announce) → `0x1400`/`0x0500` (in-fight) →
  `0x1100`/`0x0100` (round boundary) → `0x0300` (next round). Rounds resolve.
- CD path healthy throughout: `cdrom_state` showed `reading=1`,
  `ring_partial=19517`, `bfrd_set=41447`, `irq_present_max_late=0`,
  `ring_starved=0`. `bioscall_dump` showed `B0/0x07 DeliverEvent = 770`
  (events flowing — NOT the CD-starvation wedge signature).
- `frame` advanced continuously (29k→86k+); gs cycled `8`(battle)↔`4`(exit)
  as rounds ended and the mash re-entered.

This corrects the earlier over-correction ("no mode reaches `0x700` combat"):
versus DOES reach and sustain combat. The `0x700` cell is a *specific*
sub-state, not the whole combat phase; the `0x1400`/`0x1100` family are the
real in-fight/round-boundary phases.

## The wedge is the SELECT screen, not fight input

Full input-path trace (all clean-ported, faithful):

### Fight input — CPU works WITHOUT pad-2
- `fighter_state_tick` (battle.c:3260) → `fighter_script_fsm`
  (battle.c:5927) is a per-fighter script/AI state machine. It reads a
  per-fighter script buffer (`param_3`, the fighter's `+0xf` state) and emits
  command words (`param_3+6`, e.g. `0x400`/`0x8400`) that become the fighter's
  input. This runs for CPU- and demo-controlled fighters and writes
  `fighter+0x7a`/`+0x7c` directly — **bypassing pad-2 entirely**.
- `pad_edge_scan`/`action_from_input` (battle.c:5111/5379) consume
  `fighter+0x7a`(held)/`+0x7c`(edge) regardless of source.

=> CPU *fight* input is NOT pad-2-dependent. Attract demos (CPU-vs-CPU, zero
pad injection) fight autonomously through this path.

### Select input — slot-1 is EXCLUSIVELY pad-2
- `pad_state_refresh` (menu.c:2954) decodes two hardware pad records
  (`DAT_801befa8` pad-1, `DAT_801befca` pad-2) via `attr_word_decode`
  (save.c:1611) into `battle+0x6c` packed words and `+0x68`/`+0x6a` edges.
  **No CPU branch.** `attr_word_decode` returns `0xffffffff` for an absent
  pad → treated as no-input.
- `mc_pad_button_handler`/`mc_slot_action` (save.c:1739/2010): the `0x100`
  phase seed. case-0 (arcade) seeds ONLY the pressed slot —
  `+0xb4`(slot0) on pad-1 edge, `+0xb6`(slot1) on pad-2 edge `0x840`.
  **No CPU auto-seed.** `picker_table_shuffle` only randomizes character
  *display order* (anti-memorization), not selection.
- `fighter_spawn_setup` case-0 (menu.c:3853): builds each select record's
  `+8` button word. For slot-1 (`s1=1`) it reads `battle+0x76/0x6e/0x6a` —
  **the pad-2 words**. No CPU synthesis.
- `select_screen_tick`/`select_confirm_handler` consume `puVar6+8` (the
  per-slot button word) — again pad-2-derived for slot-1.

=> Every clean-code path for select slot-1 traces to pad-2. **No CPU
select-confirm exists in the clean tree.**

## What this means

- The arcade/CPU wedge is a **select-screen input-source gap**, not a missing
  fight-input path and not a CD/IRQ bug. The battle pipeline (CD, ev_stream,
  combat resolution, `fighter_script_fsm` CPU input) is proven working by the
  versus result.
- Resolution requires finding the CPU-select mechanism. Candidates:
  1. A GENERATED (unported) function that synthesizes pad-2 record data or
     calls `mc_slot_action(1)`/seeds `+0xb6` for the CPU — would run via the
     generated fallback if present and dispatched.
  2. Real arcade hardware providing a pad-2 (some cabinets wire port 2).
  3. A select-screen CPU cursor/auto-pick in generated code not yet ported.
- `press(slot=1)` injection works end-to-end (proven by versus combat), so
  the pad-2→select→fight→combat chain is fully functional once slot-1 input
  exists. If a CPU-select generator is found, feeding it the same channel
  is faithful (it IS the game's own mechanism).

## Observability fix verified

`game/resolve.c` `ev_stream_dispatch` now calls `debug_server_poll()` every
`0x400` opcodes (commit `580f274`). Verified live: the debug server stayed
responsive through attract cycles AND the versus fight on 9821, where the
clean-C `for(;;)` previously starved it permanently (the earlier 9820 wedge
showed `battle_intro_flow→battle_frame_tick→ev_stream_dispatch→ev5_action_select`
in `sample` with a dead server).

## UPDATE — versus-1P reaches sustained CPU combat with ONLY P1 input

`tools/versus_p1only.py` (new) navigates to versus (`9b=2`) injecting ONLY
slot-0 — never slot-1. Observed on port 9830:

- Reached "FIGHT" at t=4s with zero slot-1 injection.
- Slot phases `0x1f8000b4`/`b6` cycled `0x0300`(announce) →
  `0x1400`/`0x0500`(in-fight) → `0x1100`/`0x0100`(round end) → `0x0300`
  repeatedly over 30+s — **multiple rounds resolving**.
- `gs/ph` cycled `8`(battle)↔`3`/`4`(transitions) as rounds ended/re-entered.

**Conclusion: versus mode on a 1-player instance auto-drives the slot-1 CPU
opponent through BOTH select-confirm AND fight — no pad-2 needed.** The
CPU-select/CPU-input machinery is fully functional in versus (`9b=2`).

Reconciliation with `mc_slot_action`: case-2 (versus) seeds BOTH `+0xb4`
and `+0xb6` to `0x100` on ANY confirm press — so versus never needed pad-2
for the *seed*; and the slot-1 cursor/confirm is evidently CPU-driven when
no pad-2 is present (the versus_fight_mash's slot-1 injection was
unnecessary for CPU play).

**=> The wedge is ARCADE-SPECIFIC (`9b=0`).** Versus-1P is a fully working
CPU-opponent mode — a live, faithful testbed for the clean tree. Arcade
uses `mc_slot_action` case-0 (per-slot pad seed) + `battle_frame`, a
different select flow that is the actual broken path.

## UPDATE 2 — arcade wedge reproduced live; versus-vs-arcade select divergence

Menu map (live probe): mode options cycle `(2,1)`=versus → `(1,4)` →
**`(0,3)`=arcade (`9b=0`,`9c=3`)** → `(3,2)` → wrap. Arcade `9c=3` →
`select_screen_tick(3)` (versus `9c=1` → `select_screen_tick(1)`).

`tools/arcade_p1only.py` (P1-only, target `(0,3)`), port 9831:
- P1 (`b4`) progressed `0x100`→`0x300`→`0x500`→`0x0a00`; **CPU slot `b6`
  stayed `0x0000` — never seeded** (mc_slot_action case-0 needs pad-2 edge).
- Reached `gs=9 ph=0` then FROZE (frames tick via VBLANK only).
- `bioscall_dump`: **B0/0x0B TestEvent = 1,632,362** spinning (DeliverEvent
  only 224) — kernel-event starvation at gs=9.
- `cdrom_state`: idle; `irq_state`: VBLANK only.

`tools/arcade_both.py` (both-slot injection, target `(0,3)`), port 9832:
- Reached FIGHT then same `gs=9 ph=0`, `b6` still 0 — BUT injection fired on
  versus phases (`ph==6/7`) which do NOT match arcade's select flow, so
  slot-1 was never fed during arcade's actual select window. Inconclusive.

### Interpretation
- versus (`select_screen_tick(1)`) auto-handles the missing-pad-2 slot-1
  (CPU opponent fully works — proven). arcade (`select_screen_tick(3)` /
  `battle_frame`) does NOT seed `b6` without pad-2 and wedges at gs=9 on a
  kernel event (TestEvent spin).
- Two candidate roots, not yet separated:
  (a) arcade CPU-select is a generated/unported path that should auto-assign
      CPU (like versus) but is missing/broken;
  (b) `b6=0` is normal for arcade and the real wedge is kernel-event/CD
      starvation at gs=9 (same deferred-coroutine issue).
- Next discriminator: feed slot-1 CONTINUOUSLY through arcade's real select
  window (arcade_pad2spam.py spams both slots every frame in gs=8/9) OR dump
  the TestEvent event-flag index to classify the wait as CD vs select.

## UPDATE 3 — arcade wedge root-caused to CD event-flag starvation (0xF0000009)

Live reproduction on port 9840 (`tools/arcade_pad2spam.py`, continuous
both-slot injection through gs=8/9):

- Arcade (`9b=0`) navigates fine to select (`sig=(0,3)`), P1 `b4` advances
  `0x100→0x300→0x500→0x0a00`, then freezes at **gs=9 ph=0**.
- **`b6` stays `0x0000` even under continuous pad-2 spam** — arcade's CPU
  opponent is NOT pad-2-driven; versus's slot-1 auto-confirm does not apply.
- `bioscall_dump`: **B0/0x0B TestEvent = 1,575,861** spinning (vs DeliverEvent
  ~224). Frames ~frozen (10883→10884). CD idle (`reading=0`, `irq_flag=0`,
  last CD cmd frame 7128 — ~12000 frames before wedge).

### Mechanism (traced in clean tree)
- The spinning syscall is kernel **TestEvent** — clean wrapper
  `cd_seek_state_machine` (cd.c:3660 → `bios_svc(0xb0,0x0b,a0,0)`), guest
  0x8019d5b0. Its callers are the `while(v0==0)` seek/state retry loops.
- The awaited flag is the **CD channel completion event `0xF0000009`**.
- Producer chain: CD DMA done → INTC/CD IRQ (i_stat bit2) → `cd_cb7_run`
  (cd.c:5234, DICR done-mask dispatch) → per-channel reset →
  `cd_dma_chan_reset` (cd.c:5188) → `cd_bios_svc_07(0xF0000009,0x20)`
  (DeliverEvent) or callback `0x801bdee8`.
- `cd_seek_step` (0x8019d508) is ALREADY `br2_call_generated`-delegated
  (cd.c:5114) for exactly this class: "IRQ-poll spin on TestEvent …
  DeliverEvent needs the deferred CD coroutine resumed on IRQ, which a
  clean-C spin cannot drive."

### Where it stands
- Scheduler/kernel threads are ALIVE (`sp_ring` shows TCB 0x8FE0 cycling
  BIOS DMA servicing every VBLANK; `event_ring_tail` shows DMA_DONE/ENQ/DEQ/
  ISTAT_RAISE flowing). This is NOT a dead scheduler.
- The wedge is the **producer side**: DeliverEvent(0xF0000009) never fires
  because either (a) the CD IRQ bit2 never reaches `cd_cb7_run`, or (b)
  `cd_cb7_run`/`cd_dma_chan_reset` (both clean ports) run without the
  dispatch boundary the deferred coroutine needs.
- pc_probe cannot observe the clean producers (no guest block PCs) — count 0
  is a false negative for clean-C functions, same class as the earlier
  cd_response_pump pc_probe false-negative.

### Faithful next step
Determine whether the CD IRQ (i_stat bit2) reaches `cd_cb7_run` during the
wedge — i.e. instrument the INTC→cd_cb7_run dispatch boundary, or add a
debug-server PC/regs dump (none exists: `regs`/`cpu_state`/`pc`/`current_tcb`
all "unknown command"). If `cd_cb7_run` is clean-C and its DICR `while`
loops lack the per-block IRQ resume the generated body had, the §4 fix is to
delegate it (or the spinning consumer) to generated. Versus-1P avoids this
path entirely (different asset-load/seek timing), which is why it fights.

## UPDATE 4 — CPU permanently parked in BIOS kernel bottom-half (irqctx proof)

`get_registers` / `irqctx_ring` / `restore_trace` on the wedged instance:

- `get_registers`: `cop0_epc=0x8019A648`, `ra=0x8019A53C`, `sp=0x801FFEE8`
  (kernel stack), `s0`-region points at CD INTC state block `0x801BCD18`.
- `irqctx_ring` (last 6 VBLANK entries, frames 105408-105413): EVERY
  interrupt has `take_pc=real_epc=exit_pc=0x8019A648`, `same_thread=1`,
  `restored=1`, `entry_sp=0x801FFEE8`. The interrupted context is ALWAYS the
  same BIOS kernel bottom-half — the game thread never regains the CPU.
- `restore_trace`: `cd_intc_dispatch` (state block `0x801BCD18`) runs each
  IRQ → `cd_cb7_run`/`cd_cb8_dispatch` DICR path is reached. `defer_state`:
  VBLANK `raise≈deliver` (98128/97957), `defer_pending=0`.
- `bios_svc_trace` last-256: ALL `B0/0x17` (kernel DMA/IRQ bottom-half,
  `ra=0x8019AA9C` BIOS-internal). The 1.5M `TestEvent` count is cumulative;
  the *live* hot loop is the kernel thread, not the game thread.

### Refined mechanism
The game thread is BLOCKED in a BIOS wait (its TCB resume_pc is not in any
ring — `thread_ctx_ring` empty, `thread_trace` empty). The kernel/IRQ
bottom-half runs every VBLANK servicing DMA/INTC but never produces the
event flag the game thread waits on, AND the scheduler never selects a
ready thread that would. Two sub-cases remain open:
  (a) the awaited event is a CD-channel completion (`0xF0000009`) whose
      DeliverEvent producer is a clean-C function (`cd_cb7_run` /
      `cd_dma_chan_reset`) that ran without the deferred-coroutine resume
      boundary → flag never set;
  (b) the awaited event is a thread-signal/frame-sync produced by a second
      game thread that is itself blocked → genuine deadlock needing the
      game thread's blocked resume_pc to identify.

### Blocking diagnostic gap
No debug command exposes a BLOCKED thread's saved PC. `get_registers` only
shows the currently-running (kernel) context; `thread_ctx_ring`/
`thread_trace` are unpopulated. To identify the exact guest wait, either
(a) populate thread_ctx_ring on thread-block, or (b) walk the kernel TCB
wait-queue in guest RAM via `read_ram` and decode the saved context.

## UPDATE 5 — CORRECTION: live frame loop, wedge is game-logic b6=0 (not kernel deadlock)

`pc_probe_arm` on the GENERATED dispatch entries (observable, unlike clean fns)
during the wedge — decisive reframe:

- `vsync_wait_n` (0x8019a5d8): **1056 hits** — normal per-frame pacing.
- `cd_intc_dispatch` (0x8019a8ec): **528 hits**; `cd_cb8_dispatch`
  (0x8019ae68): **528 hits** — exactly 2:1 vsync:intc = healthy VBLANK→INTC→
  cb8→`cd_cb8_run_param`(increments DAT_801bdde0 + fires frame callbacks).
- **`cd_result_wait_a` (0x801968cc): 0 hits** — NOT the active wait. The
  earlier stack hits were stale frames.
- `DAT_801bdde0` (frame counter) advances ~150/s; `irqctx_ring` shows VBLANK
  IRQs deliver every frame. CD command history: **no CD cmd since frame
  ~7128** — CD is idle, not the blocker.

### Actual mechanism (clean-verified)
- `vsync_wait_n` (save.c:1554, delegated) waits `DAT_801bdde0 >= a0`; counter
  advances fine → returns each frame. The CPU spends its time here because
  it's the per-frame yield/pump, NOT because it's the bug.
- The game is in a LIVE frame loop stuck at **gs=9, b4=0x0a00, b6=0x0000**.
- `b6` = CPU side's select/battle phase cell (`_DAT_1f8000b6`).
- `battle_state_init` (menu.c:4706) seeds `b6→0x300` ONLY IF
  `select_phase_gate(a2,1)==0`, which requires `b6 & 0xf00 ∈
  {0x100,0x200,0x300,0x400,0x700}` (menu.c:3695). **b6=0 → gate=1 → skip.**
- Chicken-and-egg: `b6` must reach a valid phase class during SELECT (via the
  per-side state machine at menu.c:9607/9613/9640 writing 0x100/0x600/0xa00,
  gated on side-selector `a3`) BEFORE battle_state_init. Those transitions
  never ran for the CPU side → b6 stays 0 → battle_state_init skips it →
  gs=9 waits forever for a side that was never seeded.

### Conclusion
NOT a kernel/CD/scheduler wedge. The arcade CPU opponent's SELECT-phase
transitions never execute — the same "CPU select-confirm missing" root
isolate from UPDATE 1, now pinned to the exact cell (`b6`) and gate
(`select_phase_gate`). versus-1P avoids it because its select path
auto-seeds both sides. The fix is in the select state machine's CPU-side
(a3=1) handling for arcade mode — find why the CPU side's phase transitions
(0x100 arm etc.) don't fire.
