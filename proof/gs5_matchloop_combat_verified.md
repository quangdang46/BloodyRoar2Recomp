# gs=5 = match_loop = REAL COMBAT — battle engine verified on HEAD; bug isolated to gs=8 pre-fight flow

## State-byte semantics (corrected)

`g_game_state` = `DAT_1f800000` = byte @ `0x1f800000` (my "gs").
`DAT_1f800001` = byte @ `0x1f800001` (my "DAT1") — match_loop's internal
battle-state machine (0,1,…,0xf0,0xf1,0xff).

`entry_0x80126234` state map (game.c ~574):
- 3 -> stage select
- 4 -> `scene_entry` demo-fight load -> 5
- **5 -> per-frame `match_loop` = THE FIGHT**
- 6 -> post-match -> 3
- 7,8 -> `battle_flow_state_machine` (options / pre-fight setup) -> 5

## Verified on fresh :8901 (no input, attract mode)

gs cycled repeatedly through the attract demo:
```
t=4s  gs=5 DAT1=1   <- match_loop (fight) — fighter region mutating
t=24s gs=4 DAT1=0   <- scene_entry load
t=6,14,18s gs=5     <- repeated demo fights
t=0,20s gs=6 DAT1=0xf1/1  <- post-match exit
```

Probe evidence: during `gs=5`, the fighter/battle region @ `0x1f800200`
changed **23 bytes in 1s** — live combat simulation running.

Git history already proved this: `e3b07d8` "attract demo battle struct
mutates live — real combat sim confirmed", `cf43581` "fresh boot reaches
attract demo combat", `48b6f30` "combat re-confirmed".

## What this isolates

The battle engine (match_loop, match_end_check, fighter sim) **works on
current HEAD**. The player-path wedge is NOT in the fight itself — it is
upstream, in **`gs=8` = `battle_flow_state_machine`** (the pre-fight
char-select / stage / fighter-spawn flow), which fails to transition to
`gs=5`.

- Arcade wedge: `gs=8 DAT1=3`, CPU-opponent phase `+0xb6` stuck `0x000`
  (`bcfg_step_a` gate on flag `0x800` @ `+0x6a` never set by CPU-select).
- Versus wedge: `gs=8 DAT1=6` char-select, a player never reaches confirm
  state 2.

Attract reaches combat because it goes `gs=4 -> gs=5` directly via
`scene_entry`+`match_loop`, **bypassing the buggy `gs=8` player-select
flow entirely**.

## Consequence for the fix

Once `gs=8 -> gs=5` unblocks (correct phase-init / confirm for the
CPU-opponent and versus-P2 paths), the already-working `match_loop`
fight takes over. The bug is confined to `battle_flow_state_machine` /
`battle_frame` phase setup in `game/menu.c` — NOT the battle engine,
CD, GPU, or hardware.

## Refinement — the exact phase-init gate (menu.c)

`gs=8` `battle_flow_state_machine` runs its own `DAT1` setup machine
(0,1,2,3,4,5,6,7,**8**). `caseD_8` IS the player fight
(`fighter_pair_update`, `fighter_contact_resolve`, `battle_camera_step`,
`battle_hud_draw`, `player_light_apply`). The wedges sit earlier:

- Arcade `gs=8 DAT1=3` = `caseD_3` (`frame_divisor` wait -> `battle_init`)
- Versus `gs=8 DAT1=6` = `caseD_6` (heavy setup: `per_side_setup`,
  `action_enter_fwd`, `fighter_anim_table_init`, ...)

Phase words `battle+0xb4/+0xb6` (hi-nibble class) progression:
`0x000`->`bcfg_step_a`(mode-dispatch) / `0x100`->`0x200`(selecting) /
`0x200`->`select_confirm_handler`->`0x300` / `0x300`->intro announce /
`0x700`->`bcfg_step_c` round-reset.

`bcfg_step_a` (menu.c:4176) early-returns 0 unless
`(*(u16*)(pl+0x68) & 0x800)` — `pl+0x68`/`+0x6a` are the **per-player
input edge-detect words** (P0=`+0x68`, P1=`+0x6a`), computed by the
pad-attribute decoder (menu.c:~2873) from `+0x6c/+0x70`. `0x800` is a
confirm/face button edge.

Live wedge `:8703` (arcade): `+0xb4=0x0300`, `+0xb6=0x0000`,
`+0x68=+0x6a=0` (transient edges, idle). P0 reached `0x300` via its
human confirm edge; the **CPU opponent's `+0x6a` never received a `0x800`
confirm edge** — no pad, so the CPU AI must synthesize it. Its phase
never entered a select class -> `battle_state_init`'s
`select_phase_gate(a2,1)` rejected `0x000` -> `+0xb6` stays 0 ->
`bcfg_step_a` returns 0 forever -> setup never reaches `caseD_8`.

Fix target: the CPU-opponent select/confirm path that should drive the
`+0x6a` input edge (or directly seed the phase) during `DAT1=0/1/6`.
