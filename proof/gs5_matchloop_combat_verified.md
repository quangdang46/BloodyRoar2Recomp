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
