# row3 `(9b=1,9c=4)` tally wedge — `battle+0xb6` phase-arm trace

Continuation of `proof/hud_state_machine_row3_wedge.md`,
`proof/arcade_cpu_phase_seeding.md`, `proof/arcade_cpu_phase_init.md`,
`proof/post_fix_mode_sweep.md`. This session produced the **complete live
write-trace** of `battle+0xb4/+0xb6/+0x92/+0x8c/DAT1` through a full
row3 match→tally run, verified every relevant clean port against the
generated bodies instruction-for-instruction, and ran two decisive live
experiments (pad-1 menu confirm; `write_ram` poke of `+0xb6`).

## Verdict (up front)

**No clean-vs-generated divergence exists in this path.** Every function
in the chain is faithful. `+0xb6` stays `0` for the entire match because
the *only* code path that can first-arm it in mode `(1,4)` is
`mc_slot_action(g,1)`, which requires a **pad-1 (controller-2) `0x840`
edge while the attract menu has row 3 highlighted** — i.e. the second
player must press START/CROSS at the main menu. With only pad-0 input
the state produced is exactly what the guest binary specifies, and the
tally park is faithful guest behavior on that state.

## Live trace (port 9982, fresh boot → row3 → wedge)

gwatch window `0x1f8000b0..bf` armed before boot; 2048-entry ring dumped
after stall. Reconstructed timeline (all values guest-LE):

| frame | event | b4 | b6 | 92 | DAT1 |
|-------|-------|----|----|----|------|
| 5887 | menu confirm region | 0000 | 0000 | — | 02 |
| 6360 | `mc_slot_action(g,0)` arm | **0100** | 0000 | — | f0→…→00 |
| 6401 | `battle_state_init` round 1 | 0300 | 0000 | 0100 | 00 |
| 6575 | dat1=6 (spawn/fight) | 0300 | 0000 | 0101 | 06 |
| 7743–7896 | round 1 fight: 92 = 1→2→3→7 | 0300 | 0000 | …→0107 | 02,03,04,08 |
| 7946–8038 | round end: 92 = b→c→(next round)0 | 0300 | 0000 | 010b→010c→0100 | — |
| 8130–8243 | round 2 same cycle | 0300 | 0000 | …→0107→010b→010c | — |
| 8334–8426 | round 3 same cycle | 0300 | 0000 | …→0107 | 06,07,08 |
| 8538 | round 3 end: 92 = b | 0300 | 0000 | **010b** | — |
| 8539 | `post_battle_dispatch` case 11 `L_dwin`: `b4 0x300→0x500` (`(v&0xf000)|0x500`, menu.c:7231-7235); 92 → **010e** | **0500** | 0000 | 010e | — |
| 8540–8614 | `vs_confirm_e` phase 0xe: `+0x8c` counts 0→0x49, clamped to **0x1356** at fr8614 (`s5&0x800` path, menu.c:7984-7985) | 0500 | 0000 | 010e | — |
| 8615 | `vs_confirm_e` `+0x8c>=0x12c` branch → `MW16(+0x8c,0)` + `vs_confirm_b(s1,s2,s3,a3)`; **`+0xb4: 0500→0100`** (`(0x500&0xf000)|0x100`, menu.c:9554-9556); **`+0xb6` store skipped** (menu.c:9557-9558 `if (MBS16(s1+0xb6)!=0)` — faithful) | **0100** | **0000** | 010e | — |
| 8616 | `vs_confirm_b` `L_case4` tail: `MW8(s1+1,0xb)` | 0100 | 0000 | 010e | **0b** |
| 8649+ | `hud_state_machine(1)` tally park forever | 0100 | 0000 | 010e | 0b |

`+0xb6` was written **exactly once** in the entire captured window —
the single `0x1F8000B6` gwatch hit predates the ring (fr<5401, menu/boot
region) and never recurred. Round-win counters at stall: side0 `+0xe4=3`,
side1 `+0xe4=2`.

## Why `+0xb6` can never arm in mode `(1,4)` — complete writer census

Every guest store to `battle+0xb6` (offset 182) in `generated/`:

| store PC | function | reachability for `(9b=1,9c=4)` |
|----------|----------|-------------------------------|
| `0x801526B8/CC` | `mc_slot_action` | **only first-arm** — needs pad-1 `0x840` edge at menu confirm |
| `0x80153288` | `mc_pad_button_handler` entry | zero-fill only |
| `0x801513AC` | `battle_state_init` | gated by `select_phase_gate(1)` — chicken-and-egg |
| `0x80152AFC` | `side_phase_advance` `L_300` | requires `9c==0` (ours is 4) |
| `0x80152D60/E70/3088/313C/3160/319C` | `vs_confirm_b` | `L_d10`/`L_case4` nonzero-guarded; `L_3094/3140/316c` behind `L_ecc` which requires `9b==0` |
| `0x80154AE4/5520/5608` | `post_battle_dispatch` case 11 | `==0x300`-class guarded (`menu.c:7211-7213,7233-7235`) |
| `0x801487AC→0x80148840` | `fighter_spawn_setup` | only upgrades existing `0x500→0x600` |
| `0x8017EB78→0x8017EC20` | action/commit module | unrelated fighter-state struct path |

`battle_config_eval` for `idx=1` with `cls==0x000` dispatches `9b=1 →
L_A14` (announce-only, `menu.c:4145-4157`) — no arm. `bcfg_step_c/d`
write `0x100` but only from `cls==0x600/0x700` — unreachable from 0.
`select_confirm_handler` writes `ph[param_2]` only when
`select_phase_gate(param_2)==0 && cls==0x200` — same chicken-and-egg.

**Conclusion: `mc_slot_action(g,1)` is the sole first-arm.** Verified
faithful against generated `block_801526D0`: own slot `+0xb4+slot*2=0x100`,
foe `(slot^1)*2+0xb4=0` — for slot 1 that is `+0xb6=0x100`, `+0xb4=0`.

## Menu-confirm mechanics (why pad-1 must press at the menu)

`attract_menu_dispatch` (`menu.c:2819+`) is the stepper callback; while
the cursor sits on row 3 it sets `9b=1,9c=4` and calls
`mc_pad_button_handler(0x1f800000)` (`save.c:1739`), which **zeroes
`+0xb4..+0xb7` every call** then checks pad edge words:

- `+0x68 & 0x840` (pad-0 edge) → `mc_slot_action(g,0)` → `b4=0x100,b6=0`
- `+0x6a & 0x840` (pad-1 edge) → `mc_slot_action(g,1)` → `b6=0x100,b4=0`

Decoded pad map measured live (port 9984): raw `press` bits → decoded
held word at `+0x6e`: START(`0xFFF7`)→`0x0800`, CROSS(`0xBFFF`)→`0x0040`,
DOWN(`0xFFBF`)→`0x4000`, UP(`0xFFEF`)→`0x1000`. So mask `0x840` =
**START or CROSS edge** — the confirm buttons.

First nonzero return exits to `gs=8` immediately, so the two slots cannot
both arm across separate presses; even a same-frame double edge nets only
the last slot's arm (foe-clear). Versus mode `(9b=2)` avoids this
differently: `mc_slot_action` case 2 writes **both** `+0xb4=+0xb6=0x100`
unconditionally — which is why `(2,1)` versus works with one confirm
(`proof/arcade_cpu_phase_seeding.md` table).

## Live experiment A — pad-1 confirm at menu (port 9984)

Fresh boot, cursor on row3, single `press(slot=1, START)`:

- `gs 5→8` triggered by pad-1 alone; `mc_slot_action(g,1)` ran →
  `b6=0x100,b4=0`; `battle_state_init` → `b6=0x300`, `b4=0`.
- Result: **mirror wedge** — side-0 unarmed, select screen stalls at
  `gs=8 dat1=00` forever (P1 never armed). Proves the arming path works
  exactly as the guest specifies and that exactly one side can be armed
  per menu confirm.

## Live experiment B — `write_ram` poke `+0xb6=0x0100` (port 9986)

Fresh boot → row3 → slot-0 confirm (`b4=0x100,b6=0`) → immediately
`write_ram 0x1f8000b6=0x00, 0x1f8000b7=0x01`:

- `b4=b6=0x300`, `92=0x0003` — **both gates pass**, `confirm_bookkeeping`
  writes `ph=0x300` as designed.
- But the run then stalls at `gs=8 dat1=00` (select screen): with
  `gate(1)` now passing, `fighter_spawn_setup` merges pad-1 words into
  side-1's record; pad-1 neutral → record stays empty → side-1 is treated
  as a human slot needing real input → select never completes.
  Reproduces the prior "poke does not unwedge" finding
  (`arcade_cpu_phase_init.md` §"Why poking +0xb6=0x300 did not unwedge").

## Tally mechanism (why `b6=0` ⇒ permanent park)

`hud_state_machine` case 0, mode 1 (`save.c:5145-5156`):

```
sp58 = ((lhu(g+0x92) & 0xf00) ^ 0x100) == 0   // true iff phase-nibble == 0x100
sp20 = sp58 ? side0_blk(0x1f8000e8) : side1_blk(0x1f8001f0)
sp24 = *(u32*)(sp20 + 0x2c)                  // opposing side record
b5 = sp20[0xe4]; b4 = sp24[0xe4];
if (b4 < b5) sp48 = 0  else sp48 = 1
mode1body: if (sp48 == 0) goto poll;         // permanent park
```

With `b6=0`: `confirm_bookkeeping` gets `g0=0,g1=1` → `ph=0x100` →
`92&0xf00=0x100` → `sp58=1` → `sp20`=side0 (`e4=3` wins) vs side1 (`2`):
`b4(2) < b5(3)` → `sp48=0` → park. Had both words been armed,
`ph=0x300` → `sp58=0` → `sp20`=side1 (`e4=2`) vs side0 (`3`):
`3 < 2` false → `sp48=1` → tally proceeds. The park is therefore
*specifically* the "armed side won" configuration — faithful guest
behavior for a versus tally that expects the second side's bookkeeping.

## Bottom line for the parent agent

- **No `game/*.c` fix is justified.** Clean and generated agree at every
  inspected site (jump tables `0x801209c8/e0/f8` dumped live and matched;
  `vs_confirm_b` case dispatch, `mc_slot_action` case 1/4 stores,
  `battle_state_init`/`post_battle_dispatch` guards all verified).
- The wedge needs an **input/device-layer** answer: mode `(1,4)` is a
  two-sided versus variant whose second side is confirmed by a pad-1
  `0x840` edge at the main menu (`mc_pad_button_handler` →
  `mc_slot_action(g,1)`). `mode_monitor.py row3` only ever presses
  slot 0 at the menu (slot-1 mashes happen later, in `gs=8`, where
  `mc_pad_button_handler` no longer runs).
- Two sub-questions remain genuinely open (hardware-faithful, not
  clean-port bugs):
  1. On real hardware does `(1,4)` expect **both** pads at the menu
     (VS HUMAN), or does the second side join/confirm during the versus
     select screen through a path not yet identified? The foe-clear in
     `mc_slot_action` case 1/4 means even simultaneous dual-confirm arms
     only one side — suggesting the intended flow arms side-1 *later*.
  2. If a guest-side AI/CPU pad-feed into `0x801befca` exists (pad-driver
     boundary), it was not found in `game/*.c` or `generated/`; prior
     docs reached the same negative.
- Suggested next step (unchanged from prior docs): Beetle-oracle A/B of
  the identical input script to learn the ground-truth `+0xb6`/`+0x92`
  sequence, or a pad-2 `input_route` capture timed to the menu window.

## Artifacts

- Live dumps: `/tmp/gw_9982.json` (2048-entry gwatch ring),
  `/tmp/mode_9982_row3_state_stall.json`, `/tmp/emu_{9984,9985,9986}.log`.
- Probe scripts: `/tmp/probe_padmap.py`, `/tmp/probe_bothpads.py`,
  `/tmp/probe_poke_b6.py`.
- Instances left running: 9982 (wedged tally), 9984 (select stall,
  b6-armed mirror), 9985 (wedged tally), 9986 (select stall, poked).
