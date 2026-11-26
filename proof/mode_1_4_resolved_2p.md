# Mode (1,4) — RESOLVED post-boot-fix: faithful 2P mode, NOT a wedge

Date: post-`2b57002` (cps_ fallthrough + CD-region restriction). Debug port
5555, `tools/row3_twopad.py` (two-pad confirm) vs `tools/mode_monitor.py` +
`tools/row3_cpuwin2.py` (single-pad).

## Verdict

Mode `(1,4)` (`gs=8`, `sig=(g+0x9b,g+0x9c)=(0x01,0x04)`) is a **2-player
versus mode** and works correctly. The apparent "tally wedge" reported in
earlier sessions was **faithful single-pad behavior**: with only P1
confirming at the menu, side1's `+0xb6` phase never arms, so the match
parks at the results tally (`dat1=0x0b`) waiting for a second player that
never joined. Confirming on **both** pad slots arms side1 and the mode
demo-loops matches indefinitely — correct versus-attract behavior.

## Evidence

### Single-pad (mode_monitor / row3_cpuwin2, slot 0 only)
- Reaches `gs=8`, real fights run, scores climb `e4` to `03/02`.
- `p92` (battle phase @ `st[0x92]`) reaches `0x0e` — the match-end victory
  sequence (case 14) fires correctly.
- Then parks at `dat1=0x0b` (results), fighter ctrl blocks frozen, START /
  CROSS do not advance. `e4` frozen at `03/02`.
- This is the documented faithful wait: side1 `+0xb6` unarmed → tally
  cannot conclude → waits for P2. NOT a bug.

### Two-pad (row3_twopad.py, slots 0 AND 1 confirm at menu)
- Continuous match loop for **111+s / ~100 matches**, process 100% CPU:
  ```
  dat1=0b (tally, e4=03/02)
    -> dat1=04 (intro) -> dat1=08 (fight, e4 00/00->01/01->02/02)
    -> dat1=0b (tally 03/02) -> next match ...
  ```
- `e4` resets `00/00` each match and climbs to `03/02` — real
  CPU/2P-driven fights completing back-to-back. No stall, no livelock.

## Why earlier sessions flagged it

`proof/mode_1_4_matchend.md` + `proof/mode_1_4_resolution.md` already
established match-end is faithful and the tally wedge is a *clean-port*
bug (fixed `b88ea82`). The residual "open question" (legitimately loops vs
match-end issue) is now answered: **it loops forever by design** — it is
the versus-attract path. Single-pad runs legitimately wait for P2.

## Regression check vs the cps_ change

Mode `(1,4)`'s match/mode-flow lives in clean-ported `game/menu.c` /
`game/save.c` (`hud_state_machine`, `post_battle_dispatch`), dispatched as
named clean ports — NOT `cps_` transliterations. The CD path it uses is
now generated (post-`2b57002`), identical to when the matchend doc observed
healthy looping. No regression: the two-pad loop above ran on the current
build.

## Mode coverage status (all verified on current master)

| Mode | sig | Result |
|------|-----|--------|
| Boot→title | — | gs cycles 3/5/6, frame 141k+ |
| Versus | (2,1) | sustained combat 125s+, live fighter mutation |
| Arcade | — | gs=9 fight phases cycling |
| Attract | — | demo active, CD streaming (heap churn) |
| Mode (1,4) 2P | (1,4) | ~100-match loop, healthy |
| Mode (1,4) 1P | (1,4) | faithful wait-for-P2 at tally |

No known mode wedges remain.
