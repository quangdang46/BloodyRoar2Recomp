# Battle bring-up wedge — scheduler A/B, bail characterization, and "FIGHT REACHED" reinterpretation

Date: this session. Live specimen: headless `game_1pad.toml`,
`PSX_HLE_SCHEDULER=0` (LLE host-fiber bridge), debug port **9720**,
driven by `tools/arcade_fight_mash.py`.

## TL;DR

1. **Scheduler backend is exonerated.** `PSX_HLE_SCHEDULER=0` (legacy
   host-fiber bridge — each guest thread gets its own host stack, so
   clean-C frames *would* survive a cooperative switch) wedges
   identically to the default HLE deterministic-TCB scheduler: same
   `g_psx_call_bail` signature, same DAT1 stall. The wedge is **not** a
   host-stack/scheduler artifact.
2. **Bail signature is stable across schedulers.** First bail at
   ~frame 3933: `site_ra=0x000029CC` (kernel call-site return address,
   `site_sp=0x801FFE40` = kernel stack), `wild_pc = guest_sp =
   0x80126664` (post-`battle_frame`-jal main-loop continuation; note
   `guest_sp` holds a **code address**, i.e. the restored `$sp` slot is
   corrupt). `bail_flattened=1`, `resolved=0`, `drops=9`, `in_exc=0`.
   Same family as the frame-~1600 kernel-stack context collision
   (`proof/battle_bringup_fatal_halt.md`) — `defer_switch=false` turned
   the fatal variant into this benign-looking flatten, but the underlying
   saved-context corruption may still be firing.
3. **"FIGHT REACHED" does NOT mean combat.** `tools/arcade_fight_mash.py:127-130`
   prints it when `gs==8 && DAT1==8`. DAT1=8 is the **spawn-commit wait**
   state (`battle_frame` case8 polls `rec+0x10==1 && phase==0x700`), not
   the fight. Prior "successful" runs (9983 @1905, 9980 @3852) only
   proved DAT1 reached 8 — they may have wedged at the *same* phase-0x300
   wall one state later (the 9221338 audit specimen was exactly
   DAT1=8/`+0xb4=+0xb6=0x0300`). **No run has been shown to reach actual
   combat.** The real unsolved problem is the announce `0x300 → 0x700`
   phase transition, which has no observed writer in any run.

## This session's specimen (port 9720, fiber mode)

- Wedge at `gs=5`, `DAT1=3` (stale scratchpad; outer FSM never reached
  `gs=8` battle). `freeze_check` @ frame 106943:
  - `bce_calls=6`, `bce_cls=0x000`, `bce_fz0=6` — `battle_config_eval`
    barely engaged (DAT1=3 spawn-prep, not the DAT1=8 poll loop).
  - `tramp_calls=1`, `tramp_last_node=0x801D0228`,
    `tramp_last_cur=0x801B2784` — the channel-script task
    (`LAB_80137acc`) ran **once** and its cursor sits at script1 start
    (`0x801B2784`), matching the *earlier* "channel-script frozen at
    start" wedge class (`proof/spawn_phase_channel_script_wedge.md`),
    not the "scripts completed" class of the 9221338 audit.
  - `tpool_fail_count=0` — no pool exhaustion this run.
  - `exception_entries=128158`, `esc_fired=128158` — IRQ/exception
    machinery active; `current_func=0x000029CC` at sample time.
- Mash never printed FIGHT/LEFT/WEDGE — `gs=5,ph=3` matches none of its
  drive conditions (`ph==1` title, `ph==2` menu), so it idled. This
  specimen is inconclusive for the battle phase wedge but confirms the
  fiber path boots/navigates to `gs=5`.

## What is now established vs still open

Established (prior + this session):
- `fighter_spawn_setup` (`game/menu.c:3781-3839`) faithfully waits for
  `rec+0x10==1 && (phase&0xf00)==0x700`; phases pinned at `0x300` is a
  faithful wait, not a clean defect (audit 9221338).
- `battle_config_eval` cls-`0x300` is announce-only/terminal; the only
  clean `0x700` writers need cls `0x000`/`0x600`, unreachable pre-fight.
- Slot-1 `0x0800` edge correctly advances P2 `+0xb6` `0x000→0x300`.
- Channel scripts *can* run to their `0x4000` terminators (audit
  specimen: `tramp_calls=177`), but this run froze at `tramp_calls=1`.

Still open (the actual blocker):
- **The guest writer of phase `0x700` (or DAT1=9) after the intro
  timeline is unidentified.** Candidates not yet exonerated:
  `script_stream_tick` (`0x80137824`, clean `game/battle.c:8709`, armed
  via `task_alloc_wrap_b/d/e`), `LAB_80137cd0`, an EV/event script, or
  the unported channel-script handlers in `0x8014cd48..0x8014cdbc`
  (script2 table `0x801B2820`, last entry tick `0x51` → `0x8014cdbc`).
- Whether the ~frame-3900 bail *causes* the missing trigger (abandons a
  clean-C frame that was arming it) or is a coincident symptom is
  **unproven**. `guest_sp=0x80126664` (code-addr-as-SP) suggests real
  context-restore corruption, but bail_flattened=1 is survivable.

## Recommended next steps (for the parent agent)

1. Run a **versus** specimen (`tools/versus_fight_mash.py`) — versus
   reaches `gs=8` with both phases `0x300` and `post_battle_dispatch`
   actually executes there; watching `0x1f8000b4/b6` writes during a
   versus run can expose the `0x300→0x700`/DAT1 writer the arcade audit
   could not find statically.
2. Catch the **first bail live**: fresh run, `fntrace_arm 0xFFFFFFFF`
   (or targeted `0x8019A8EC/0x8019AA9C/0x80192634`) before ~frame 3800,
   `fntrace_dump` the window around `first_frame` to see exactly which
   guest call chain produced `site_ra=0x29CC`/`wild=0x80126664`, and
   correlate with whether a phase-write task was pending.
3. Audit the script2 handler table `0x801B2820` entries (esp. the
   `0x8014cdbc` tail) in `generated/SLES_017.22_full_*.c` for `sh`/`sb`
   to the scratchpad phase globals or DAT1=9 — the audit never decoded
   these handler bodies.
4. If the bail proves causal, the faithful fix is in the **call
   contract / context-restore path** (`cpu_state.h psx_call_contract`,
   `dirty_ram_interp.c` contract sites, `interrupts.c`/`traps.c`
   context management) — recognize legitimate cross-thread/context
   returns instead of flattening, matching the `exc_sp_straddle`
   precedent already in `OpenBIOS_dispatch.c:9191-9222`. Do **not** patch
   `game/*.c` to fake the phase write.

No code change made — no defect proven fixable within this session's
scope; prior exhaustive audit (9221338) found no clean-port defect.
