# CPU opponent confirm source — definitive answer

Question: where does the CPU opponent's character-select / versus
confirmation input originate on real hardware? Four sub-questions:

1. Who writes the temporary two-player select records' `record+8`
   button/edge field consumed by `select_screen_tick` (`0x8014f260`)?
2. Who fills pad record `0x801befca` (SIO slot 1), and can it be
   populated from CPU/AI state instead of hardware?
3. Does the CPU character-selection AI also emit confirmation?
4. Is there an automatic CPU-confirm branch in the versus
   orchestration (`vs_confirm_a/b/c/d/e`, `post_battle_dispatch`,
   arcade `battle+0x9b==0` / versus-CPU `==1`) — or does the CPU side
   consume a real controller-2 edge?

## Verdict

**No guest-side CPU-confirm synthesizer exists in the audited code.**
Every confirm path — arcade (`battle_frame`), versus
(`select_screen_tick`), and in-battle (`battle_input_dispatch`) —
ultimately consumes rising-edge button words that are produced by
exactly one writer chain:

```
SIO pad poll (channel task, registered via cd_channel_table_init)
  -> pad records 0x801befa8 (slot0) / 0x801befca (slot1)
  -> attr_word_decode            0x8012a650   game/save.c:1604
  -> pad_state_refresh           0x8012a72c   game/menu.c:2869
       held  -> battle+0x6c/+0x6e      edges -> battle+0x68/+0x6a
  -> consumers:
       fighter_spawn_setup       0x801487ac   record+8 merge (select ctx)
       battle_config_eval/bcfg_step_a 0x80153544  phase advance, needs &0x800
       battle_input_dispatch     0x8012b55c   in-battle pad handlers
```

The CPU side is literally "controller 2" at every layer that matters.
The missing piece in the headless runtime is a **controller-2 device
source for SIO slot 1** — runtime/SIO-layer work, not a `game/*.c`
change. Confidence: **high** for the arcade path (runtime-proven);
**high** for versus-CPU statically (same sole writer chain, no
alternate branch found).

## 1. `record+8` — sole writer is `fighter_spawn_setup`

`select_screen_tick` (`game/menu.c:3100`, guest `0x8014f260`) builds
two temporary player records on its guest frame (`sp+0x40` area,
stride `0x30`) and reads `*(uint32_t *)(record + 8)` for cursor moves
and confirm (`select_confirm_handler(0x1f800000, idx, record,
record[+8], 1)`).

Audit of every `+8`-relative store on those records across `game/*.c`
and `generated/`:

- **`fighter_spawn_setup` (`0x801487ac`, `game/menu.c:3768`) is the
  only writer.** Its jump-table dispatch on `battle+0x9b` merges the
  battle pad words into `record+8`:
  - mode 0: `bp[0x68]`/held/repeat for P0 only.
  - mode 1/4 (versus / versus-CPU): P0 record gets slot-0 words,
    P1 record gets slot-1 words (`battle+0x6a` edge /
    `+0x6e` held / `+0x76` repeat). Returns after the first gated
    player per call (disasm-verified epilogue jump).
  - mode 2: both records get both pad merges.
  - mode 3: phase reset path.
  All sources are `battle+0x68..0x7e` — pad-derived only. No CPU/AI
  buffer, no mode-conditional synthesizer.

- `battle_frame`'s internal select state machine
  (`game/menu.c:~8300-8460`, inside `0x8014cdd0`) has the same
  contract: record `s0+8` is read for confirm at `menu.c:8363-8374`
  (`& 0xe0` / `& 0x800`) and is filled by the same
  `fighter_spawn_setup` call at its case-3 (`menu.c:~8195`).

- The only "auto-confirm-like" preset found anywhere is
  `battle_frame` L_case9 (`menu.c:8493-8501`): after a side commits
  (`rec+0x10==1 && phase==0x700`), it writes the *other* record's
  `+0x24 = 1` (confirm-pending). That is post-commit bookkeeping —
  the committing side still needed a real `0x700` phase, which for
  class `0x000` requires `bcfg_step_a` = pad edge `& 0x800`
  (`menu.c:4195` region). It synthesizes no button word.

## 2. `0x801befca` — filled by the SIO/channel pad poll; no CPU type exists

- The two pad records are registered as channel entries at boot:
  `cd_channel_table_init(g_DAT_801befa8, g_DAT_801befca)`
  (`game/game.c:981`; `0x801a36f0`, clean wrapper delegates to
  generated at `game/cd.c:1065-1074`). They are serviced by the same
  async channel/poll framework as CD channels; the record bytes
  (+1 type, +2/+3 buttons, +6/+7 analog/status) are written by the
  pad-poll task from SIO — i.e., the device layer.

- `attr_word_decode` (`0x8012a650`, `game/save.c:1604-1640`) reads
  the type nibble at `record+1`: pad types `0x40/0x50/0x60/0x70`
  decode button bytes `+2/+3` (inverted, masked `0xf9ff`, analog
  extension from `+6/+7`); **any other type → `-1` → the slot
  contributes zero words.** There is no "CPU"/"AI" record type and no
  fallback synthesizer.

- `pad_state_refresh` (`0x8012a72c`, `game/menu.c:2869-2916`) has no
  branch on `battle+0x9b`/player type: it decodes both records,
  writes held at `+0x6c/+0x6e`, rising edges `(new^old)&new` at
  `+0x68/+0x6a`, repeat/timing state, aggregates at `+0x7c..0x80`,
  then `pad_slot_state_walk` (`0x8018bba8`).

- `mc_pad_button_handler` (`0x80153264`) / `mc_slot_action`
  (`0x8015260c`, `game/save.c:1732/2003`) only re-arm per-slot phase
  words `+0xb4/+0xb6` when real edges (`+0x68/+0x6a & 0x840`) are
  observed; mode-specific init, no synthesis.

- Runtime proof (headless, port 8981 — see
  `proof/arcade_cpu_confirm.md`): with slot 1 disconnected the CPU
  phase `battle+0xb6` stayed `0x0000`; `press {"slot":1,...}` updated
  `0x801befca` and advanced `+0xb6` `0x0000 → 0x0300`. The phase
  machine demonstrably consumes a slot-1 edge; device *presence*
  alone does nothing.

## 3. The "CPU character-selection AI" picks characters — it does not confirm

`select_phase_update` (`0x8015a1e4`, `game/save.c:3158-3275`) is the
only AI-flavored select logic found. It:

- maintains two 11-entry pick tables at `0x801f47a0`/`0x801f4770`
  (refilled from `table_byte_next_a` when the round counter
  `+0xa2 >= 0xb`),
- writes side records `0x1f8000e8` (P0) / `0x1f8001f0` (P1/CPU):
  `+3` character index, `+4` lock flag, `+1` participation —
  character bookkeeping only,
- is called once per match-loop iteration (`game/menu.c:599`) and at
  init (`scratchpad_pair_init`, `game/game.c:2752`).

It never writes `0x801befa8/ca`, `battle+0x68..0x7e`, select-record
`+8`, or any confirm field. It selects *which* character the CPU
side shows; it does not press the button.

## 4. Versus orchestration — no auto-confirm branch

- `select_screen_tick` record state machine (`game/menu.c:3151-3200`):
  `rec+0x24 == 0` requires `rec+8 & 0xe0` or `& 0x800` (the pad word)
  to reach `1`; `== 1` auto-advances to `2` only when the *opponent*
  record's `+0x24 > 0` or flag bit is set. The `==1` state itself is
  only reachable via the input confirm (or `battle_frame` L_case9's
  post-commit preset). No path presets a CPU record to `1` in the
  versus flow — audited writers of record `+0x24` are
  `select_screen_tick` (`menu.c:3160/3167/3198/3200`),
  `fighter_spawn_setup` (`=0`, `menu.c:3796/3804`), and
  `battle_frame` (`menu.c:8374/8399/8420/8498/8501`).
- `battle_config_eval` (`0x80153544`): for `battle+0x9b ∈ {1,4}`
  (versus/versus-CPU) phase class `0x000` is announce-only — versus
  phase advancement flows through `post_battle_dispatch` →
  `vs_confirm_c/d/e` downstream of the select confirm, i.e. still
  gated on the same pad words. For `9b==0` (arcade) `bcfg_step_a`
  explicitly requires `(edge & 0x800)` for **both** sides including
  the CPU side.
- `battle_state_init` (`0x80151328`, `game/menu.c:4606`) force-seeds
  `+0xb4/+0xb6` to `0x300` when `select_phase_gate==0` — this is the
  observed `0x300` seeding, not a confirm.
- `battle_input_dispatch` (`0x8012b55c`) + `pad_handler_a/b/c`
  (`0x80161c54/0x80162280/0x80162838`, `game/menu.c:5959/6119/6263`)
  are pure consumers of pad masks passed from
  `battle_config_eval`/`post_battle_dispatch` return values; no
  synthesis.
- Generated-tree audit: no writes to `0x801befca`, `battle+0x6a`,
  or a synthetic confirm buffer were found; the address never appears
  literally in `generated/` (accessed via channel-table pointers).

## Input-layer contract the runtime must satisfy

For the CPU side to confirm exactly as on hardware, SIO slot 1 must
deliver a **valid pad record** at `0x801befca`:

- `record+1` type nibble ∈ {`0x40,0x50,0x60,0x70`} (else
  `attr_word_decode` → `-1` → zero words);
- `record+2/+3` button bytes such that the decoded word produces a
  **rising edge** at `battle+0x6a` containing bit `0x800` (confirm)
  — and `0xe0`-family bits where the select-record path needs them;
- held-state continuity so `(new^old)&new` fires once, then releases.

Controller *presence* (connected flag / non-`0xff` record) is
insufficient — proven live. The correct fix is in the controller-2 /
SIO input feeding (device model or a faithful guest-visible pad
source), **not** a speculative clean-port change to select/vs logic.
Whether real versus-CPU hardware auto-confirms without a pad-2 edge
via a versus-setup preset not yet identified is an open caveat; all
evidence in this build shows the pad-2 channel is the only writer,
and versus-CPU shares it.

## Files/lines cited

- `game/menu.c:2869-2916` pad_state_refresh; `:3100-3197`
  select_screen_tick record loop; `:3555` select_confirm_handler;
  `:3768+` fighter_spawn_setup; `:4007-4211` battle_config_eval /
  battle_state_gate / bcfg_step_a; `:4606` battle_state_init;
  `:4892-4906` pad_slot_state_walk decl; `:5154-5232`
  battle_input_dispatch; `:5959/6119/6263` pad_handler_a/b/c;
  `:6790-7219` post_battle_dispatch; `:7697+` vs_confirm_c/d/e;
  `:8075-8560` battle_frame incl. internal select machine.
- `game/save.c:1604-1640` attr_word_decode; `:1732` mc_pad_button_handler;
  `:2003-2049` mc_slot_action; `:3158-3275` select_phase_update;
  `:3285-3391` match_setup_apply.
- `game/game.c:592-617` `_g_pad_buffer` staging; `:981`
  cd_channel_table_init(pad0,pad1); `:1314-1345` frame_tick →
  pad_state_refresh; `:2742-2752` scratchpad_pair_init.
- `game/cd.c:1057-1074` cd_channel_table_init (delegates to generated).
- `proof/arcade_cpu_confirm.md` — runtime session behind the
  pad-2 injection result.
