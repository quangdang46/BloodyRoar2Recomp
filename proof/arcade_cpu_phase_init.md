# Arcade/1P versus-spawn wedge — `battle+0xb6 == 0` (CPU slot-1 phase never armed)

Continuation of `proof/arcade_cpu_confirm.md` and
`proof/arcade_cpu_phase_seeding.md`. This session re-derived the full
phase-init chain, ran a **fresh versus-CPU reproduction** on debug port
9021, and confirmed the conclusion those docs reached: **the defect is not
a `game/*.c` mistranslation — it is the missing controller-2 input feed
for the CPU opponent.** No faithful `game/*.c` phase patch exists.

## Fresh runtime evidence (this session, port 9021, `PSX_GWATCH=0x1f8000b4,4`)

Drove `/tmp/vs_cpu_nav.py 9021` (menu: DOWN×2 + START → `9b=1,9c=4`).
Result — **versus-CPU wedges identically to arcade**:

```
gs=8 st1=00 9b=1 9c=4 92=0100 8c=ffff
b4=0300 b6=0000
c0=801c1200 a2=0000 a4=0000 a8=801f47a0 ac=801f4770
side P0 0x1f8000e8: all-zero
side P1 0x1f8001f0: +1=1 +2=1 +3=5 +4=0     <- CPU char index 5 present
charrecs @0x801c1200 populated
```

So **every CPU mode** (arcade `9b=0`, versus-CPU `9b=1`) fails the same
way; only two-human versus (`9b=2`) works. The CPU side record *is*
populated (character picked), but its battle phase word stays `0`.

## Phase-init chain (all verified faithful to generated)

1. `mc_pad_button_handler` (`game/save.c:1732`, guest `0x80153264`) zeroes
   `+0xb4..+0xb7` every menu frame, then re-seeds only slots whose pad
   **edge** fires: `+0x68&0x840`→slot0, `+0x6a&0x840`→slot1.
2. `mc_slot_action` (`game/save.c:2003`, guest `0x8015260c`) dispatches on
   `battle+0x9b`: case0 (arcade) seeds only the pressed slot `0x100`;
   case1/4 seeds pressed slot + clears foe; **case2 (versus) seeds BOTH
   `0x100`** — this is the only reason versus works.
3. `battle_state_init` (`game/menu.c:4606`, guest `0x80151328`) writes
   `0x300` only to slots passing `select_phase_gate`
   (`game/menu.c:3605`). `b6=0` → class `0x000` → gate returns 1 → skipped.
   Generated `func_80153484` confirms the accepted set is exactly
   `{0x100,0x200,0x300,0x400,0x700}` — clean port is faithful.
4. `battle_config_eval` (`game/menu.c:4008`) never writes `0x100` to a
   zero phase: cls`0x000`→`L_A0`→`bcfg_step_a`→`0x700` (needs pad edge
   `&0x800`); cls`0x100`→`0x200`; cls`0x700`→`bcfg_step_c`→`0x100`.
5. `select_confirm_handler` (`game/menu.c:3555`) writes `0x300` only when
   current class is `0x200`; for `b6=0` the gate fails → else-branch → no
   phase write (only side-record fields).
6. The only other pre-battle `+0xb6` writers are `fighter_spawn_setup`'s
   `0x500→0x600` patch and `vs_confirm_b`/`side_phase_*` — all downstream
   of `battle_frame`/post-battle dispatch (circular, per task brief).

## Why poking `+0xb6=0x300` did not unwedge (re-confirmed)

`+0x8c` stays `0xffff` → `post_battle_dispatch` is never reached; the spin
is inside `battle_frame`'s internal `st==8` loop which waits for a
`phase&0xf00==0x700` record to set `B36_8(0x3c)` → `st==9`. With `b6=0`
the CPU side can never reach `0x700` (`bcfg_step_a` needs the `+0x6a&0x800`
pad edge). Forcing `b6=0x300` puts it in the announce-only terminal class —
still no `0x700` — so the wedge persists. The missing input is upstream of
the phase word, not the word itself.

## Root cause

`pad_state_refresh` (`game/menu.c:2869`, guest `0x8012a72c`) is the **only**
writer of the `+0x68/+0x6a` edge words; it decodes the two physical pad
records `0x801befa8` (slot0) and `0x801befca` (slot1) via
`attr_word_decode`. There is **no CPU/AI synthesizer** anywhere in
`game/*.c` or `generated/` that feeds slot-1 — verified by exhaustive
grep for writers of `+0x6a/+0x6e/+0x72/+0x76`, `0x801befca`, and all
`+0xb4/+0xb6` store sites (only `mc_slot_action`, `battle_state_init`,
`fighter_spawn_setup`, `vs_confirm_b`, `side_phase_*`, `post_battle_*`,
and in-fight `battle.c` writers exist).

On real hardware the CPU opponent's versus-screen confirm is delivered on
controller-2 exactly like a second pad. In headless there is no
controller-2 device, so `+0x6a` never edges and `mc_slot_action(1)` /
`bcfg_step_a(player1)` never fire.

## Conclusion / fix direction

- **Not a clean-port mistranslation.** Every function in the chain
  (`mc_pad_button_handler`, `mc_slot_action`, `select_phase_gate`,
  `battle_state_init`, `battle_config_eval`, `bcfg_step_a`,
  `battle_state_gate`, `fighter_spawn_setup`, `select_confirm_handler`)
  matches its generated disassembly.
- **Not fixable by patching `+0xb6`** — that would be a forbidden guess and
  does not unwedge anyway (proven by `write_ram` poke).
- **Faithful fix is input-layer**: provide a controller-2 (SIO slot-1)
  source for the CPU opponent's versus/select confirm when `battle+0x9b`
  marks slot-1 as CPU — or a guest-side AI pad-feed into `0x801befca` if
  one exists at the pad-driver/BIOS boundary (not yet located; the pad
  buffers are written by the BIOS pad driver, not game code).
- Manual verification path: `press {"slot":1,"buttons":<START>,"frames":N}`
  reaches `+0xb6` exactly as a physical pad-2 would (per prior proofs).

## No source changes made

Consistent with `arcade_cpu_confirm.md` §"Answers": **no `game/*.c` change
is justified by the evidence.** The clean ports are faithful; the missing
piece is a device/input feed outside the game-logic layer.
