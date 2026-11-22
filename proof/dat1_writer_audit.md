# `0x1f800001` (DAT_1f800001) writer audit + battle_frame state-3 escape

Date: static audit of `game/*.c` clean tree (companion to
`proof/arcade_residual_wedge.md`, `proof/arcade_spawn_phase_starvation.md`).

## TL;DR

`battle_frame` (guest `0x8014cdd0`, `game/menu.c:8076`) is a synchronous
`for(;;)` dispatch on `DAT_1f800001`. While it is parked, no outer writer can
run — the only concurrent writers are task/script callbacks pumped by
`task_pools_tick` inside its own tail (`menu.c:8513-8516`). The designed
state-3 escape is **`resource_rebuild_a` (`0x8014c8d4`) writing `5`** via the
task/script path; failing that, the observed `==3` freeze is downstream of
the CD deferred-coroutine starvation already proven in
`proof/cd_event_drop_point.md` (spawn commit gate needs `rec+0x10==1 &&
(phase&0xf00)==0x700`, which requires loaded battle assets).

## Writer table for `0x1f800001`

| Guest addr | Clean name | Values | Context |
|---|---|---|---|
| `0x80127574` | `game_globals_reset` | 0 | boot/init reset (`game/game.c:1130`) |
| `0x80147034` | `match_loop` | 0,1,+1,2,0xf0,0xff | attract/demo loop, separate top level |
| `0x8014751c` | `attract_menu_dispatch` | 0xf0 | attract mode-select confirm |
| `0x801487ac` | `fighter_spawn_setup` | 6 (param_5!=0); 3→2, 6→5 (param_5==0) | battle_frame case 3/6 + select_screen_tick state 6 (`menu.c:3799,3809-3810`) |
| `0x8014c8d4` | `resource_rebuild_a` | **5** | indirect via task/script handler during `task_pools_tick` (`menu.c:11318-11340`) |
| `0x8014cdd0` | `battle_frame` | 0,1,2,3,6,7,8,9→1,0xf1,0xff,0 | the parked FSM itself |
| `0x8014f260` | `select_screen_tick` | 0 entry; 0xf0/0xf1/0xff/0; 6 via spawn_setup(...,1) | synchronous call from battle_flow_state_machine |
| `0x80155c14` | `battle_sides_commit` | 6 when `s0&0x100` | outer post-battle commit (`menu.c:4883-4885`) |
| `0x8015c754`/`0x8015b9a0` | `system_config_menu_a/b` | 0,+1,0xf0,0xff | options menus |
| `0x80164668` | `results_scoreboard` | 0,+1,0xf0,0xff | post-match |
| `0x80164ea4` | `player_name_entry` | 1..4,+1,0xf0,0xff,0 | name entry |
| `0x80165cb4` | `transition_screen_flow` | +1,0xf0,0xff,0 | fades |
| `0x801660c0` | `card_fade_sm` | 0,+1,0xf0,0xff | card fade (`save.c:3696-3768`) |
| `0x80167330` | `battle_intro_flow` | +1,0xf0,0xff,0 | versus intro |
| `0x8016fcc4` | `battle_flow_state_machine` | 0,1,2,3,4,6,7,8,12,-1/0xff | outer FSM; calls battle_frame/select_screen_tick synchronously |

`game/bind.c` only binds `g_DAT_1f800001` (declaration, not a writer).

## State-3 mechanics

- `L_case3` (`menu.c:8189-8199`): `fighter_spawn_setup(0x1f800000, recA, recB,
  sp+0x18, 0)` every iteration.
- Commit gate inside `fighter_spawn_setup` (`menu.c:3788-3789`):
  `rec+0x10==1 && (phase&0xf00)==0x700`.
- On commit: `3→2` (`menu.c:3809`); `L_case2` immediately rewrites `3`
  (`menu.c:8185-8186`) → **re-arm, not escape**.
- `4/5→L_case45` writes `6`; `L_case6` handshake → `7→8`; `9→1` resets.
- `L_tail` runs `task_pools_tick(1/2)` every iteration → queued task nodes
  can invoke script handlers (`task_alloc_wrap_e` → `LAB_80137acc`
  interpreter, `battle.c:16729-16823`) **while parked** — this is the only
  concurrent-write channel.

## Escape analysis

- No `3→4/5/6` writer exists inside `battle_frame` case 3.
- `resource_rebuild_a` (`0x8014c8d4`) writes `5` and is reachable only via
  the task/script table (`0x801b2764/0x801b2784/0x801b2820/0x801b2844` are
  **data tables**, not code entry points — `task_alloc_wrap_c` stores them as
  script/jump fields; `task_alloc_wrap_e` uses `param_1[3]` as handler table).
- `battle_flow_state_machine` case 0 routes to `battle_frame()` iff
  `DAT_1f80009b==0 && syscfg+0x11!=1`, else `select_screen_tick(0)` — both
  synchronous; neither can run while `battle_frame` is parked.
- Conclusion: `DAT_1f800001==3` freeze = spawn-commit prerequisites unmet.
  Per `proof/cd_event_drop_point.md` + `proof/cd_event_drop_point.md` chain,
  the main TCB is parked in the CD deferred-coroutine `TestEvent` spin, so
  battle assets never finish loading → `rec+0x10`/`phase 0x700` never set →
  state 3 forever. Fix the CD finalize starvation first; the state-3 writer
  question is downstream and likely moot.

## Not proven (needs runtime probe after CD fix)

- Which script table entry invokes `0x8014c8d4` (`resource_rebuild_a`);
  verify with `mem_words`/`fntrace` on the four table addresses above once
  the CD path is unblocked.
