# Arcade combat is REAL — CPU opponent is live (b6=0 is not a missing fighter)

## Question

Does arcade mode (`battle+0x9b==0`) actually run a CPU-controlled second
fighter, or does it degenerate to P1 fighting alone? The open symptom was
`battle+0xb6 == 0` for the entire `gs=8` battle while versus showed both
phase cells active.

## Method

Runtime inspection of the live fighter control contexts on a headless
instance driven through arcade by `tools/arcade_p1only.py` (sends **only**
`slot=0` / P1 input — never `slot=1`). Compared against the known-good
versus instance (port 9821, `versus_p1only.py`, sustained rounds over
~550k frames).

## Findings

### Fighter ctrls are the scratchpad pair, cross-linked as foes

`game/battle.c:4` — "Fighter ctrls are the absolute globals
`0x1f8000e8` (P1) / `0x1f8001f0` (P2)", stride `0x108`. `FC_FOE=+0x2c`
cross-links them: `P1.+0x2c == 0x1f8001f0`, `P2.+0x2c == 0x1f8000e8`.
`fighter_link_init` (`game/save.c:2542`) initializes both every battle.

### Arcade fighter-ctrl signature == versus signature

| field | arcade (9850) | versus (9821, proven combat) |
|-------|---------------|------------------------------|
| `gs` / `st` | `08` / `08` | `08` / `08` |
| `FC_HP` `+0xac` | `-1` | `-1` |
| `FC_MAXHP` `+0xb0` | per-char `150`/`200` | per-char `150` |
| `FC_FOE` `+0x2c` | cross-linked | cross-linked |

`HP=-1` is **normal**, not a dead sentinel — the proven-versus fight shows
the identical value. `+0xac` is the persistent player-record HP field;
the live combat HP lives in the task-spawned fight entity the ctrl points
to (`+0x1c/+0x20/+0x30/+0x38..+0x48` → `0x801exxxx`/`0x8007xxxx` structs).
`battle.c:2520`'s `HP>-1` "alive" test applies to a different lifecycle
phase than `st=08` fighting.

### P2 is a live, animated, CPU-driven fighter

Diffing the full `0x108`-byte P2 ctrl block across ~0.4 s samples during
`gs=8 st=08` shows **continuous mutation every sample**: position/velocity
(`+0x70..+0x77`), action/anim state (`+0x94..+0x9f`), hit/aux state
(`+0xa2..+0xa7`, `+0xb2..+0xb7`, `+0xc4..`). This is an actively simulating
entity, not a static config record.

The arcade harness issues **zero** `slot=1` presses, so P2's autonomous
motion is produced by the game's own CPU-opponent driver — i.e. the CPU
AI is running.

### Battles complete and cycle

`gs=8` (fight) → `gs=9` (post-match/name-entry) → `gs=5` → `gs=8` (next
battle) repeats indefinitely. Each `gs=8` window lasts only a few seconds
because the harness's P1 is mostly idle and the CPU finishes the round;
that is consistent with a real fight, not a stall.

## Conclusion

**Arcade mode runs real combat against a live CPU opponent.** `b6==0` is
the arcade single-player-vs-CPU phase encoding — the versus second-player
phase cell is simply not used on this path. It is NOT evidence of a
missing fighter.

The earlier `proof/arcade_cpu_phase_init.md` /
`proof/arcade_cpu_phase_seeding.md` "no CPU-input synthesizer" conclusion
applied to the **earlier hard-wedge state** (game frozen, not cycling).
That wedge was resolved by the `player_name_entry` `intro_movie_state(0)`
fix (commit `475e316`), after which the battle loop runs normally. The
`battle_state_init` gate-seeding (`select_phase_gate` accepting only
`0x100..0x700` classes) seeds `b4` for P1 and intentionally leaves `b6`
unseeded for the CPU side on the arcade path.

## Caveat

A direct HP-delta (damage number) was not captured — `+0xac` reads `-1`
in both arcade and versus, so it is not the live combat HP. The proof of
combat rests on: live per-frame P2 entity mutation, CPU-autonomous
control with no pad-2, versus-identical ctrl signature, and round cycling.
If a future session wants belt-and-suspenders proof, watch the
task-spawned fight entity (`ctrl.+0x1c`/`+0x20` chain) HP or the
round-result/winner cells across a `gs=8→gs=9` transition.
