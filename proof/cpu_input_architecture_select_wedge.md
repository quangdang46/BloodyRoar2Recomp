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
