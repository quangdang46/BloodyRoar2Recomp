# Spawn-phase stall at DAT1=8 — phase-word writer audit (post-u16-fix)

Date: session after commit `1fd2a9b` (u16 gate fix). Live instance: port 9910,
`game_1pad.toml`, headless, arcade nav with slot-1 confirm injection.

## State at wedge (verified live)

- `DAT_1f800001 = 8` — owned by **`battle_frame`'s inner loop** (case8), NOT the
  outer `battle_flow_state_machine` (its case '\b' would call
  `post_battle_dispatch`; `g_pbd_calls = 0` proves it never runs — correct,
  since `battle_frame` only returns on DAT1=0xff and resets DAT1=0).
- `battle_config_eval` is called ~2×/battle_frame-iteration from case8 (plus
  ×2 inside `fighter_spawn_setup`); `bce_calls` grows ~16k/s.
- Player phase words (`u16 @0x1F8000B4` / `0x1F8000B6`):
  - slot0 = `0x0300`, slot1 = `0x0000`  (word @0x1f8000b4 = 0x00000300)
- Global state `u16 @0x1F800092 = 0x0100` → low byte 0x00 (sub-state),
  `gcls = 0x100`.
- `battle+0x68/+0x6a` (per-player pad **edge** u16s, written by
  `pad_state_refresh` as `(new^old)&new` of the two pad reports):
  both `0x0000` — no button edges pending.
- `DAT_801befa8` (pad1 report) / `DAT_801befca` (pad2 report) decode via
  `attr_word_decode` → pad1 ≈ 0x0080, pad2 ≈ 0x0020 constants (idle).
- Task pools nearly empty: pool1 slot → node 0x801d088c fn `0x80137680`
  (LAB_80137680, generated); pool2 slot0 → node 0x801d80e0 fn `0x8015b810`
  (generated). No fighter/AI task nodes allocated yet.
- `0x801f10bc = 0x801ffe58` = battle_frame ctx block guest addr.

## Phase state machine (clean-verified, `game/menu.c`)

`battle_config_eval` dispatches on `phase & 0xf00`:

| cls | behavior |
|-----|----------|
| 0x000 | `L_state0` → idx=`B[0x9b]`(=0 arcade) → `L_A0` → gcls∈{0x100,0x200,0x300} → `L_A0_body` → `bcfg_step_a` → **writes 0x700** on success |
| 0x100 | writes `(ph&0x100)|0x200` → 0x200 |
| 0x200 | terminal (ret only) |
| 0x300 | `L_state3` → idx 0 → `L_B0` → gcls 0x100/0x200→774, 0x300→7cc — **announce only, NEVER writes phase** |
| 0x400/0x500 | `L_ret68` — returns `pl+0x68`, no write |
| 0x600 | `continue_countdown`; r>0 → maybe `L_set700`; r<0 → 0x800 |
| 0x700 | `bcfg_step_c` only |
| 0x800 | writes 0x900; 0x900 = low-byte increment/reset |

`fighter_spawn_setup` (case3/case6): converts **0x500→0x600** only; commit
predicate `rec+0x10==1 && phase==0x700` → clears rec+0x10, writes DAT1
(3→2 / 6→5), allocates 2 task nodes (`0x801b2ed8/0x801b2ef4` handlers).

`post_battle_dispatch` (outer case '\b', post-match only): the ONLY clean
writer of phase **0x500** (`L_cwin`/`L_dwin`, when cls==0x300) and **0x1400**.

## The gap

For the FIRST fight there is no post_battle_dispatch run, so phase 0x300 has
**no clean-C writer out of announce**. Candidates for the real advancer:

1. A generated/delegated function writes `battle+0xb4/b6` (not yet found —
   scratchpad stores are computed, not literal-address greppable).
2. `rec+0x10==1` ("player ready") is the actual gate and the phase follows a
   different path than assumed.
3. The CPU slot's pad2 feed: `bcfg_step_a` requires
   `(*(u16*)(pl+0x68) & 0x800) != 0` — a pad **edge** bit for that slot.
   `pad_state_refresh` packs `(pad2<<16)|pad1` edge bits at `battle+0x68`:
   slot0 edge = u16@0x68 (pad1), slot1 edge = u16@0x6a (pad2). Slot1's phase
   is 0x000 → needs pad2 edge bit 0x800. If the CPU opponent's inputs are
   synthesized by a guest AI task (or expected from a real pad-2), that feed
   is missing → slot1 can never reach 0x700 via `bcfg_step_a`.

## Open questions (for parallel agents)

- Q1: enumerate ALL guest writers of `0x1F8000B4/0x1F8000B6` (clean +
  generated), especially any 0x500/0x600/0x700 store outside the functions
  above.
- Q2: who writes `rec+0x10 = 1` (T-records at battle_frame sp+0x68/+0x98),
  and what is the intended first-fight phase itinerary 0x300→?→0x700.
- Q3: is there a guest CPU-AI pad synthesizer for slot1 in arcade mode
  (9b=0), or does versus-CPU genuinely consume SIO pad-2? If AI, which
  function/task feeds `DAT_801befca`'s report or `battle+0x6a`.
- Q4: does DAT1=8's case8→9→1→2→3 cycle self-resolve once phases hit 0x700,
  or is another trigger (script op, task) required.
