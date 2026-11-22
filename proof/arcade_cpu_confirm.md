# Arcade/1P battle-spawn wedge — CPU-confirm mechanism investigation

Date: fresh headless session, debug port 8981, disc `Game.cue`, build current.
Navigation: `/tmp/nav_arcade_8981.py` reproduced the wedge
(`g_game_state==8`, `DAT_1f800001==3`, `battle+0x9b==0`, `+0x9c==0`,
`+0x92==0x0100`, `+0xb4==0x0300`, `+0xb6==0x0000`, `+0x68/+0x6a==0`).

## Verdict

**The wedge is NOT missing guest CPU-confirm logic and NOT a clean-port
mistranslation of the phase machine. It is a headless device-model gap:
the CPU side's select confirm is consumed through the SAME pad-2 decode
channel as a real second controller (`0x801befca` → `battle+0x6a` edges →
`bcfg_step_a`), and headless has no controller-2.** Injecting slot-1 input
through the debug `press` command advances the CPU phase exactly as a
physical pad-2 would. A residual second wedge exists *after* both phases
reach `0x300` (see "Residual wedge" below) that still needs one write-watch
to attribute.

## Verified mechanism (runtime + disasm)

### 1. Path routing is correct — `battle_frame` is the intended arcade path

`match_loop`/`battle_flow_state_machine` (`game/menu.c:604`):

```c
case 0:                                   /* battle+0x9c == 0 */
  if ((DAT_1f80009b == 0) &&
      (*(char *)(_g_system_config_ptr + 0x11) != 1)) {
      iVar4 = battle_frame();              /* arcade/1P internal select+spawn */
  } else {
      iVar4 = select_screen_tick(0);       /* full select screen (versus machinery) */
  }
```

Config defaults at `g_DAT_801b1280` (`game/game.c:541`
`apply_saved_settings_or_defaults`, verified live `read_ram 0x801b1280`):
byte `+0x11 == 0x00` → `!= 1` → `battle_frame()` is the **correct** route.
Attract dispatch `param_2==1` sets `config+0x10=8`, `9b=0`, `9c=0`
(`game/menu.c:2787`), matching the observed wedge bytes.

### 2. Per-frame driver — `battle_frame` case 3 → `fighter_spawn_setup`

`battle_frame` (`game/menu.c:8075`, guest `0x8014cdd0`) dispatches on
`DAT_1f800001`; st==3 runs `fighter_spawn_setup(0x1f800000, …, fp=0)`
every frame (menu.c:8195-ish `L_case3`). `fighter_spawn_setup`
(menu.c:3768, guest `0x801487ac`) calls `battle_config_eval` once per
side, patches `0x500→0x600`, and commits a side record only when
`rec+0x10==1 && phase-class==0x700`; on commit it calls
`confirm_bookkeeping` and mutates `DAT_1f800001` (st 3→2→3…→5/6).

Runtime counters baked into clean `battle_config_eval`
(`game/menu.c:4019-4022`): `bce_calls=295200`, `bce_fz0=295200`,
`bce_fzN=0`, `bce_cls=0x300`, `pbd_calls=0`. Every call is the
`fighter_spawn_setup` driver path (`param_3==0`); **`post_battle_dispatch`
is never entered** in this state — the `+0x92` round-phase machine that
versus uses does not run here.

### 3. Phase machine inside `battle_config_eval` (guest `0x80153544`)

Per-side phase word `battle+0xb4/+0xb6`, hi-nibble class:

- `0x000` → `L_A0` (menu.c:4060): requires `(+0x92 & 0xf00) ∈ {0x100,0x200,0x300}`
  then `bcfg_step_a` = `battle_state_gate && (edge & 0x800) && ret_zero_b>=0`
  → success jumps `L_set700` → phase `= 0x700` (menu.c:4151).
- `0x100` → `(ph & 0x100) | 0x200`.
- `0x200` → terminal (return edge word).
- `0x300` → `L_B0` (menu.c:4088): **announce-only for every `+0x92` class —
  no outgoing transition.** `0x300` is a stable "confirmed" state.
- `0x500`→patched to `0x600` by `fighter_spawn_setup`; `0x600` →
  `continue_countdown`/`select_phase_gate` can reach `L_set700`;
  `0x700` → `bcfg_step_c` (reset-to-idle `0x100`).

`battle_state_gate` (menu.c:4170): for `9b==0` requires
`s16(+0xa4) >= u8(char_rec_ptr(+0xc0) + 0x10)`, then a 17-entry kind table
on `+0x92` (`{0-4,15}→pass`, `{5-11,16}→fail`, `{12-14}→+0xa4>=0`).
`+0x92==0x0100` → kind 1 → pass (given the `+0xa4` check).

### 4. Pad-2 injection PROOF — CPU confirm is the pad-2 channel

`pad_state_refresh` (menu.c:2869, guest `0x8012a72c`) decodes
`0x801befa8` (slot0) and `0x801befca` (slot1) via `attr_word_decode`,
stores held words at `+0x6c/+0x6e`, rising edges `(new^old)&new` at
`+0x68/+0x6a`. `battle_config_eval` reads the edge for side `i` at
`battle + i*2 + 0x68` (i.e. P1 → `+0x6a`). **There is no CPU/AI
synthesizer anywhere in `game/` or `generated/`** — searched for writers
of `+0x6a`, `+0x9b/+0x9c`, `+0x92`, attract/demo CPU-input logic,
`scriptop_gpad` (`game/vm.c`, operates on fighter script streams, not pad
buffers). The CPU side is literally "controller 2" as far as the select
phase machine is concerned.

Live experiment on the wedged instance:

```
press {"slot":1,"buttons":0xfff7,"frames":60}   # START on pad-2
read_ram 0x801befca -> 00 41 f7 ff ...           # slot-1 record updated (~0xfff7)
read_ram 0x1f80006c  -> held/edge words live (0x0008 visible)
read_ram 0x1f8000b4/0xb6:
  before: +0xb4=0x0300  +0xb6=0x0000
  after : +0xb4=0x0300  +0xb6=0x0300   <-- CPU phase advanced on pad-2 edge
```

So headless **can** drive the CPU confirm — the missing piece is a
controller-2 device source (or a guest-side AI pad feed that does not
exist in this build's input path). `press slot=1` is a valid workaround
for manual verification but is not a game fix.

### 5. Attract/demo does NOT use this path

Attract (`g_game_state` 3/4/5 cycles) reaches combat via
`battle_intro_flow` (`0x80167330`) which picks random fighters
(`table_byte_next_a`) and calls `fighter_spawn_tail` +
`battle_round_init` + `battle_frame_tick` directly — no
`battle_config_eval` pad gates, no `+0xb4/+0xb6` select phases. Passive
attract watching never produced `+0xb4/+0xb6` activity; versus
(`9c=1/2`) uses `select_screen_tick` → `post_battle_dispatch` (the
`+0x92` machine, menu.c:6771+) and `vs_confirm_*` handlers — a different,
fuller select stack that DOES contain per-side confirm bookkeeping.

## Residual wedge (open, needs one write-watch)

After the pad-2 injection both phases sit at `0x300/0x300` with
`+0x92==0x0100` and the game still does not spawn:

- `0x300` is terminal in `battle_config_eval` (announce only).
- `+0x92` is rewritten **only** by `confirm_bookkeeping`
  (menu.c:3940, `*(u16*)(s0+0x92)=ph` with `ph∈{0x100,0x200,0x300,0x400}`
  from `select_phase_gate` per side). Its callers in this path:
  `post_confirm_config_build` (init), `battle_state_init` (init),
  `fighter_spawn_setup` (only on a `0x700` hit), `side_phase_commit`,
  `battle_frame` case9. None run per-frame at st==3 → `+0x92` stays
  `0x0100` (the init-time value: P0 gate passed, P1 gate failed).
- P1's `0x000→0x300` transition is **not** produced by any clean writer
  found (`L_set700` writes `0x700`; `battle_state_init` is one-shot).
  Candidates: a generated-fallback callee, or a re-entry through
  `battle_state_init` after `battle_frame` returned. A `wtrace`/`ramw`
  on `0x1f8000b6` is the decisive next probe.

Hypothesis to verify: in real arcade flow the CPU side reaches `0x700`
directly from `0x000` via `bcfg_step_a` (pad-2 START edge), `fighter_spawn_setup`
commits it, `DAT_1f800001` walks 3→2→3→…→5/6 and the fight starts — the
observed `0x300` may be an init-seeded value for the *other* side only,
and the true blocker may be that `battle_state_gate`'s `+0xa4` vs
`char_rec+0x10` check (menu.c:4181) fails for the CPU side, diverting it
onto a path that lands at `0x300` instead of `0x700`.

## Answers to the tasked questions

1. **Attract vs arcade path:** attract bypasses select entirely
   (`battle_intro_flow`); arcade uses `battle_frame` st=3 →
   `fighter_spawn_setup` → `battle_config_eval` per side per frame.
   Same `battle_config_eval`, different driver; attract never touches
   `+0xb4/+0xb6`.
2. **Attract `+0x9b`:** observed `3` in passive cycles; the arcade wedge
   reproduction used `9b=0,9c=0` (dispatch param_2==1).
3. **CPU confirm mechanism:** there is **no** CPU pad synthesizer, AI
   think-tick feeding `+0x6a`, or versus-screen auto-confirm on this
   path. The CPU side consumes pad-2 edges through `bcfg_step_a`
   (`battle+0x6a & 0x800`). Versus mode auto-confirms through
   `post_battle_dispatch`/`vs_confirm_*` — not present in `battle_frame`.
4. **Faithful fix location:** **input/device layer**, not game logic —
   headless needs a controller-2 source for SIO slot 1 (or the guest AI
   pad-feed if one exists in generated code not yet identified). No
   `game/*.c` change is justified by current evidence; the clean ports
   of `pad_state_refresh`, `battle_config_eval`, `fighter_spawn_setup`,
   `battle_state_gate`, `bcfg_step_a`, `confirm_bookkeeping` all match
   their disasm comments and behave correctly under injected pad-2 input.

## Commands run

- Fresh launch: `./build/BloodyRoar2_Recompiled --disc …/Game.cue
  --debug-port 8981 --headless`
- Debug: `read_ram`, `press`, `freeze_check`, `fn_stats`, `mem_words`-equivalents
- Source reads: `game/menu.c` (battle_flow_state_machine:551,
  match_loop case0:600-680, select_phase_gate:3605,
  confirm_bookkeeping:3940, battle_config_eval:4007-4160,
  battle_state_gate:4170, bcfg_step_a:4195, fighter_spawn_setup:3768,
  battle_frame:8075-8560, post_battle_dispatch +0x92 machine:6771-7260),
  `game/game.c` (apply_saved_settings_or_defaults:541, entry:615-640),
  `game/bind.c` (guest_for:21).

## Prior related proofs

- `proof/arcade_cpu_phase_seeding.md` — established H1: `+0xb6` is
  pad-edge-gated via `mc_pad_button_handler`/`mc_slot_action` with no
  CPU branch. This session **confirms and extends** it: the gate is
  `bcfg_step_a` inside `battle_config_eval`, and pad-2 injection provably
  opens it.
- `proof/arcade_spawn_phase_starvation.md` — earlier CD-side wedge;
  superseded (CD proven healthy; this is the spawn/select wedge).
