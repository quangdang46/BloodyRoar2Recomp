# gs=5 / DAT1=3 wedge — root cause and fix

## Symptom

Arcade mode wedged with `g_game_state == 5` (`match_loop`) and
`DAT_1f800001 == 3`. `match_loop` (guest `0x80147034`, `game/game.c`)
handles only DAT1 ∈ {0, 1, 2, 0xf0, 0xf1, 0xff}; every other value falls
through to `frame_tick()` and loops forever. The fighter region at
`0x1f800200` stayed byte-frozen — no combat.

## Writer of DAT1=3 (proven)

`guest_watch` on `0x1f800001` (mprotect trap, host-RIP attribution —
`g_debug_last_store_pc` is stale for IRQ/clean-C stores) caught the write:

- host RIP symbolised to `battle_frame` (`game/menu.c`, the
  `L_case2`/`0x8014d120` path):
  `engine_record_pair_update(0x1f800000, …); MW8(0x1f800001, 3);`
- call chain: `battle_frame` ← `battle_flow_state_machine` (`0x8016fcc4`)
  ← clean dispatch ← generated top-level SM (`0x80126234` region),
  guest `$ra = 0x80126664` (the `jal battle_flow_state_machine` link in
  top-SM case 8).

DAT1=3 is a **legitimate battle-flow substate** — not a fight-engine
round state, not a script/VM state. `battle_frame` parks DAT1=3 while it
performs the battle setup transition; `battle_flow_state_machine`
consumes 3→4→…→0xff→0 internally.

## Why match_loop saw it

`DAT_1f800001` is a **shared per-frame scratch byte multiplexed between
several clean-C state machines**. `match_loop`'s machine and the
battle-flow machine use disjoint value sets; the guest guarantees the
handoff is clean because `battle_flow_state_machine`'s **only**
guest-reachable exit writes `DAT_1f800001 = 0` before returning
(`game/menu.c`, the `DAT1 == -1` branch). The generated top-level SM
then does gs 8→5 and calls `match_loop`, which always sees DAT1=0.

The leak path is the runtime **bail unwind** (`g_psx_call_bail`): a
wild-return contract violation inside the BFSM call tree (a callee
C-returned with `$sp`/`$ra` not matching the `psx_dispatch_call`
contract — `psx_bail_record` wild_pc `0x80126664`, i.e. `$ra` still held
the BFSM jal link) forces every clean-C frame to return early. BFSM's
`br2_bail_pending()` exits propagated the return **without** executing
the `DAT1 = 0` exit write, so a mid-machine substate (3) survived into
`match_loop` → permanent dead loop. `bail_flattened` counts on wedged
runs: 9 (port 9740), 8 (port 9741).

## Fix (`game/menu.c`)

Restore each DAT1-owning mega-loop's exit invariant on the bail-forced
return path — write `DAT_1f800001 = 0` (the value every normal exit of
that machine produces) before returning:

- `battle_flow_state_machine` — 3 sites (loop head `joined_r0x8016fd30`,
  post-`battle_frame`, post-`menu_res_load_sm`)
- `results_scoreboard` — loop head `joined_r0x80164744`
- `battle_intro_flow` — loop head `joined_r0x801673ac` + mid-loop
  `menu_res_load_sm` bail site
- `system_config_menu_a` / `system_config_menu_b` — loop heads
  (`joined_r0x8015c784` / `joined_r0x8015b9e0`), preserving the
  `return uVar` value

This is not a phase jump: it enforces the function's own guest-guaranteed
post-condition at the only exit path that bypassed it. `player_name_entry`
(gs=9) has no `br2_bail_pending` checks at all — separate latent issue,
not touched.

## Runtime proof (port 9741, this build)

`gwatch` on `0x1f800001` during an arcade mash run:

- `NV=A0000003` writes still occur (BFSM `battle_frame`, host RIP
  `0x103A1C1AC`, `$ra=0x80126664`) — DAT1=3 is still produced normally.
- Each is immediately followed by `NV=A0000000` at host RIP
  `0x103A07108` (inside `battle_flow_state_machine`) — the bail-exit
  invariant restoration firing.
- `match_loop` then runs **complete** DAT1 lifecycles repeatedly
  (`1→2→0xf0→0xf1→0xff→0` at `$ra` `0x80126594/0x80126428/0x80126430/
  0x801265f4`) — matches reach their end states; no DAT1=3 ever reaches
  `match_loop`.

The wedge is gone: gs=5/DAT1=3 no longer persists.

## Remaining issue (separate, pre-existing)

Later in the same run the instance fatal-halted on
`FAIL-FAST unknown dispatch: addr=0x801BCBA0 ra=0x801A1EC0` inside the
CD/save command path (`ctx_fn_send → cd_cmd_state_commit → call_guest`),
after 8 bail flattens. This is downstream wild-flow/bail state
corruption — the root wild-return contract violation that *triggers*
the bails (a clean callee leaking guest `$sp`, suspected in the
`cd_cmd_queue_load`/`dev_cmd_round_dispatch` path, `last_fn=0x80192634`)
is still unfixed and is the real follow-up. It is independent of the
DAT1 ownership fix: the wedge symptom is eliminated; the bail mechanism
itself still loses guest state on unwind.
