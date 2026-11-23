# Announce-wedge fix — bail telemetry + clean-C unwind propagation

Follow-up to `proof/announce_script_handler_drop.md` (defect proof).
This document records the instrumentation, the faithful unwind-contract
fix, and the runtime verification status.

## What was implemented

### 1. Bail-drop telemetry (`src/clean_bridge.c`, `psxrecomp/runtime/src/traps.c`)

- `g_br2_bail_drops` — incremented on every `br2_call_generated()` call that
  arrives while `g_psx_call_bail` is latched (the silent no-dispatch path).
  Records `g_br2_bail_last_fn` (dropped target) and `g_br2_bail_last_ra`
  (caller `$ra`).
- `g_br2_bail_unwinds` — incremented when `br2_call_generated()` DID dispatch
  and the callee returned with bail still set (normal propagation, not a
  drop). Guest `pc/ra/sp/s0-s3` are deliberately NOT restored on this path so
  the wild-flow target survives to the outermost dispatch.
- Counters live in `traps.c` (not `clean_bridge.c`) so targets that do not
  compile `clean_bridge.c` (`psx-runtime-us/-japan/-universal`) still link
  the debug server's references.
- New debug-server command `{"cmd":"call_bail"}`
  (`psxrecomp/runtime/src/debug_server.c` `handle_call_bail`) reporting:
  `bail`, `bail_first`, `bail_resolved`, `bail_flattened`, `bail_anomaly`,
  `drops`, `unwinds`, `last_fn`, `last_ra`, the first-bail ledger
  (`site_ra`, `wild_pc`, `site_sp`, `first_frame`, `in_exc`) and live
  `pc/ra/sp/v0`.

### 2. Faithful unwind propagation through clean-C callers

Generated CPS frames already honor the unwind contract: on
`g_psx_call_bail` they `return` immediately without running their
continuation, letting `psx_dispatch_impl` resolve the contract or flatten
the wild target at the outermost depth. Clean-C callers had no equivalent —
a latched bail stayed latched forever inside e.g. `battle_frame`'s
`for(;;)`, silently dropping every later `br2_call_generated` (the
`DAT1=9`/`DAT1=0xF0` script-handler loss). Added `br2_bail_pending()`
early-returns at the loop heads / post-call points of every clean-C
mega-loop on the battle path:

- `game/battle.c`
  - `task_invoke()` — skip callback resolution/invocation while bail is set.
  - `task_pools_tick()` — stop ticking remaining nodes after a task that
    began an unwind.
  - `LAB_80137acc()` (channel-script interpreter `0x80137ACC`) — entry check
    plus a post-handler check placed **before** the script cursor advance
    (`*piVar6 = …+6`) and the tick increment (`*psVar5 + 1`), so a record
    whose handler began an unwind re-fires intact after resolution instead
    of being consumed with its handlers dropped.
- `game/menu.c`
  - `battle_frame()` (`0x8014CDD0`) — loop-head check; on bail calls
    `br2_guest_frame_abandon()` and returns 0. The guest epilogue
    (`MW8(0x1f800001,0)`, `$sp` restore) intentionally does NOT run — the
    same thing generated CPS bail-returns do.
  - `battle_flow_state_machine()` (`0x8016FCC4`) — loop-head check and a
    post-`battle_frame()` propagation check.
  - `system_config_menu_a/b`, `results_scoreboard`, `battle_intro_flow` —
    matching loop-head checks.
- `game/game.c`
  - `match_loop()` (`0x80147034`) — loop-head check.
  - `entry_0x80126234()` / `_start` — dispatch-loop check + unwind return
    (defensive; the live main loop is the generated `0x80126234` body —
    `0x80126234`/`0x80126178` are not in `clean_table`).

### 3. `br2_guest_frame_abandon()` (`src/clean_bridge.c`)

Pops the host-side `g_frm_sp` LIFO record **without** restoring guest `$sp`.
On a bail exit the guest function never ran its epilogue, so `$sp` must stay
at the un-returned frame exactly as generated CPS bail-returns leave it;
only the host bookkeeping is discarded so later `br2_guest_frame/free`
pairs stay balanced.

No unconditional bail clearing anywhere; `cd_*` delegation unwind behavior
is untouched.

## Runtime evidence

Build: `cd build && cmake --build . -j8` — clean, `[100%] Built target
psx-runtime`.

Headless runs (`--game game_1pad.toml --disc "disc/Bloody Roar 2 … (Europe).cue"
--debug-port 999x --headless` + `tools/arcade_fight_mash.py`), `call_bail`
samples (see `proof/wedge_9991.log`, `proof/wedge_hunt2.log`):

- `call_bail` responds with the full ledger on every attempt — the command
  works.
- Every run records `drops=9`, `last_fn=0x80192634`,
  `last_ra=0x80126664`, `unwinds=0` — nine `br2_call_generated` calls were
  dropped while a bail was latched during early boot.
- `bail_first=1`, `bail_flattened=1`, `bail_resolved=0`,
  `first_frame≈1568–1629`, `site_ra=0x000029CC`,
  `wild_pc=0x80126664` (= generated main's post-`battle_flow_state_machine`
  jal link, `0x80126660` block) — the single observed bail latched during
  boot and was flattened at the outermost dispatch, exactly per contract.

## Verification status — blocker

**The `DAT1==8` announce stage was not reached.** Battle entry is currently
blocked ~frame 1600 by the *separate, pre-existing* nested-IRQ
kernel-stack `$ra`-corruption family documented in
`proof/announce_script_handler_drop.md` (`DISPATCH FATAL: misaligned
target 0x0000E69E`, `cpu.pc=0`, kernel idle at `0x8019A600/0x8019A648`,
`ra_exit=0x8019A53C`, `entry_sp=0x801FFDF8`):

- **Pre-change binary** (stash A/B, port 9992): crashed at frame 1639 with
  `DISPATCH FATAL … RAM[0x801FFE58]=0x0000E69E` during BFSM bring-up
  (`gs=8`, `DAT1=3`). Log: `proof/pre_change_9992.log`.
- **Fixed binary** (3+ attempts, ports 9991/9993): survives the crash window
  but the game returns to `gs=5` where `match_loop` (`0x80147034`) cannot
  dispatch stage 3 (its stage machine handles only 0/1/2/0xf0/0xf1/0xff —
  verified identical in `generated/SLES_017.22_full_13.c`
  `br2eu_func_80147034`, so this is faithful generated behavior, not a port
  bug) → `frame_tick`-only spin.

### DAT1=3 writer identified — `gwatch` on `0x1f800001` (port 9994)

A dedicated run with `{"cmd":"gwatch","addr":"0x1f800001","len":1}` armed
before boot captured every write to the battle-phase byte
(`proof/gwatch_9994.log`):

- Periodic writes every ~200 frames from `store_pc=0x8019A48C` /
  `0x8019A5A4` / `0x80192FA4` on `sp=0x801FFFC0` (IRQ/kernel stack) —
  inside `cd_timer_read` (`0x8019a460`) / `vsync_wait_n` (`0x8019a5d8`).
- The decisive `0x00→0x03` write at **frame 1562** came from
  `store_pc=0x801931D8` = inside **`cd_queue_drain` (`0x801931c4`)**,
  running on `sp=0x801FFE40` (IRQ stack).

So the `DAT1=3` that `match_loop` spins on is **not** a
`battle_flow_state_machine`/`battle_frame`/`player_name_entry` stage
leftover — it is scratchpad byte `0x1F800001` being written by the
CD/IRQ deferred-coroutine path. `call_bail` recorded exactly **one** bail
latch per run (`bail_first=1`), at `first_frame≈1568–1629` — coincident
with battle bring-up (`gs=8` reached ~frame 1600) and with
`wild_pc=0x80126664` = generated main's post-`battle_flow_state_machine`
jal link, i.e. the unwind propagated out of `battle_flow_state_machine`
exactly as designed and flattened at its caller's contract site. The
orphaned `DAT1=3`, however, was written by `cd_queue_drain`, not by the
aborted BFSM stage machine.

Net effect of the fix at the earliest reachable stage: the bail that used
to latch forever (dropping every generated fallback for the rest of the
battle) now unwinds through the clean-C chain and resolves/flattens at the
outermost dispatch — observable as `bail_flattened=1`, `bail=0`, and the
game continuing past the point where it previously wedged. The remaining
blocker to `DAT1==8` is the IRQ/`$ra` corruption that *causes* the bail,
not the bail propagation itself.

## Files changed

- `src/clean_bridge.c` — drop/unwind counters, `br2_guest_frame_abandon()`.
- `psxrecomp/runtime/src/traps.c` — counter definitions.
- `psxrecomp/runtime/src/debug_server.c` — `call_bail` command.
- `game/battle.c` — `task_invoke`, `task_pools_tick`, `LAB_80137acc`.
- `game/menu.c` — `battle_flow_state_machine`, `battle_frame`,
  `system_config_menu_a/b`, `results_scoreboard`, `battle_intro_flow`.
- `game/game.c` — `match_loop`, `entry_0x80126234`, `_start`.
- `tools/wedge_hunt.py` — retry harness used for the runs above.

`generated/` untouched.
