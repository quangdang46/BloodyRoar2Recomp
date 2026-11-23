# gs=5 clean case omits match_loop — latent port bug (dead code today)

## Finding

`entry_0x80126234` (clean `main()`, `game/game.c:588`) gs=5 case
(`game/game.c:770-795`) does **not** call `match_loop()`. The generated
gs=5 case **does**, unconditionally, as its last call before the
`g_game_state` re-check.

Generated execution order (followed block chain `block_80126534`→`…A4` in
`generated/SLES_017.22_full_00.c`):

| # | guest addr | clean name        |
|---|-----------|-------------------|
| 1 | 0x801560F8| match_mode_set    |
| 2 | 0x8012708C| display_mode_setup|
| 3 | 0x8012AEEC| frame_tick        |
| 4 | 0x8012748C| frame_advance_helper |
| 5 | 0x8012AEEC| frame_tick        |
| 6 | 0x8015F644| memcard_poll_frame|
| 7 | 0x80147034| **match_loop**    |

then `block_80126594`: `lbu g_game_state; li 5; bne → redispatch;
sltiu <0xe` → the `uVar6<4 ? gs=6 : gs=2` tail.

Current clean order is scrambled and drops match_loop:
`match_mode_set, display_mode_setup, frame_tick, memcard_poll_frame,
frame_advance_helper, frame_tick, memcard_poll_frame` — wrong order at
pos 4-7 and **no match_loop()**.

Correct faithful order:
`match_mode_set, display_mode_setup, frame_tick, frame_advance_helper,
frame_tick, memcard_poll_frame, match_loop()`.

## Why it is NOT the live wedge

`entry_0x80126234` and `entry_0x80126178` (_start) are **absent from
`src/clean_table.c`** (grep count 0) — the runtime dispatches the main
loop through the *generated* `br2eu_func_80126234`, whose gs=5 case
`jal 0x80147034` resolves via clean_table `wrap_80147034` → clean
`match_loop()`. So clean `match_loop` DOES run (its DAT1 machine is the
observed `0→1→2→f0→f1→ff→0` cycle), and the clean gs=5 case body is
currently unreachable dead code.

The `DAT1==3` dead-state wedge lives inside `match_loop` itself (both the
clean body and the generated `block_801470F0/80147104/801474E4` fall-
through agree: value 3 → bare `frame_tick` spin). See
`proof/gs5_matchloop_dat1_3_dead_state.md`.

## Action

Fix the clean gs=5 case to the faithful order above **when**
`entry_0x80126234` is wired into clean_table (i.e. when main() itself is
clean-ported). Until then it is dormant; do not "fix" by reordering
runtime behavior — the generated path is authoritative.

## RESOLVED (commit pending)

Clean gs=5 case (`game/game.c` `switchD_80126370_caseD_80126534`) rewritten
to the faithful generated order — `match_mode_set, display_mode_setup,
frame_tick, frame_advance_helper, frame_tick, memcard_poll_frame,
match_loop()` — verified block-by-block against
`generated/SLES_017.22_full_00.c` `block_80126534`→`…8C`→`…594`. The prior
body had `memcard_poll_frame` twice, `frame_advance_helper` out of order,
and no `match_loop`. `match_loop(void)` reads scratchpad/globals (not $a0),
so the generated `move $a0,$zero` delay-slot arg is immaterial to the clean
port. Still dormant (entry_0x80126234 absent from clean_table); generated
main remains authoritative at runtime — this makes the clean source faithful
for when main is clean-ported.
