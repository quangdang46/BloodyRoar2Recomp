# Arcade CPU-input source — definitive answer + wedge exoneration

Follow-up to `proof/cpu_confirm_source.md` and
`proof/downstream_spawn_wedge.md`. Question re-opened: is there a
guest-side CPU-input feed we missed, does unconnected SIO slot 1 produce a
"phantom controller" pattern on real hardware, and does a *complete*
slot-1 confirm sequence advance the announce→spawn transition
(`battle+0xb4/+0xb6: 0x0300 → 0x0700`)?

## Verdict

1. **No non-SIO writer of the CPU-side input words exists.** The only
   writer of the pad edge/held block `battle+0x68..0x80` is
   `pad_state_refresh` (`game/menu.c:2869`, guest `0x8012a72c`), fed
   solely by `attr_word_decode` of the two SIO pad records
   `0x801befa8`/`0x801befca`. Re-audited `game/*.c` and `generated/`
   (incl. indirect/pointer writes): every other touch of `+0x68..+0x80`
   is either a different struct (fighter records in `battle.c`) or a
   *consumer* (`mc_pad_button_handler`, `bcfg_step_a`,
   `battle_config_eval`, `fighter_spawn_setup`, `battle_input_dispatch`).
   No CPU/AI synthesizer, no versus preset, no auto-confirm timer writes
   button words. `select_phase_update` still only writes character-pick
   bookkeeping (`save.c:3158`).

2. **Unconnected slot 1 = true absence, faithfully modeled.** In
   `psxrecomp/runtime/src/sio.c:1393-1403` a pad poll on a slot with
   `!(pad_connected & (1<<lp))` hi-Z's the bus: `sio_rx_data = 0xFF`, no
   ACK, zero response bytes. That is exactly what real hardware returns
   for an empty port, and it makes `attr_word_decode` return `-1` → the
   slot contributes **zero words** (`save.c:1604-1640`, type nibble must
   be `0x4x/0x5x/0x6x/0x7x`). A *connected* digital pad answers
   `0x41 0x5A btnL btnH` (4-byte poll, `sio.c:1414-1431`) — also
   hardware-faithful. There is no "phantom CPU pad" pattern on real
   arcade hardware: with no controller 2 the CPU side sees the same
   all-zero words we produce. (`pad_fill_status8`, `sio.c:251`, only
   applies to multitap bulk `0x42` responses and likewise returns all
   `0xFF` for empty seats.)

3. **Arcade mode does NOT need a slot-1 edge to reach announce — and the
   current wedge is not input-gated at all.** `battle_state_init`
   (`game/menu.c:4606-4629`, guest `0x80151328`) force-seeds **both**
   `+0xb4` and `+0xb6` to `0x300` whenever `select_phase_gate(side)==0`,
   with no pad involvement. That is how both sides reached `0x0300` on
   real hardware with no controller 2. The only writers of `0x700` are
   `battle_config_eval`'s `L_set700` (`menu.c:4154`), reachable solely
   from phase class `0x000` via `bcfg_step_a` (requires edge `& 0x800`)
   or from class `0x600` via `continue_countdown` — **neither applies at
   class `0x300`**, whose mode-0 path (`L_state3 → L_B0`,
   `menu.c:4098-4116`) only runs cosmetic announce helpers and returns.
   No auto-confirm/versus preset synthesizes the advance.

4. **Runtime-proven: a perfect slot-1 confirm does not move the wedge.**
   Live headless instance (port 9850, `battle+0x9b==0` arcade,
   `+0xb4=+0xb6=0x0300`):

   - `press {"slot":1,"buttons":0x4000,"frames":600}` (Cross) produced a
     valid slot-1 record and held word `0xF9BF` at `battle+0x6c`
     (slot-1 half of the packed held u32; `0xF9BF & 0x800 != 0`), and
     rising edges at `+0x68`/`+0x6a` — the injection reaches the pad
     pipeline end-to-end.
   - Repeated held+release sequences (Cross ×3, Start `0x0008`,
     Cross+Start, L1+R1, each ~120 frames with `clear_input` gaps) left
     `+0xb4/+0xb6 = 0x03000300` **unchanged**. Sampling during holds
     confirmed the edge/held words were live the whole time.
   - Instance state identical to `downstream_spawn_wedge.md`:
     `g_game_state=8` (byte `0x1f800000`), `DAT_1f800001=3` (battle
     bring-up state inside `battle_flow_state_machine`),
     `+0x8c=0xffff`, `+0x92=0`, and the same CD anomaly:
     `bfrd_set=2047` vs `bfrd_clear=13`, `ring_dropped=1`,
     `ring_partial=898`, `pending.cmd=0x09 (Pause) phase=1`,
     `last_sector lba=8801 frame≈1539`.

## Root-cause verdict

**Input is exonerated.** There is no guest-side CPU-input synthesizer to
find — on real arcade hardware the CPU side simply has no pad-2 words,
and the game is built to spawn fighters anyway (the `0x300` announce
phase is force-seeded, and the `0x300→…→0x700` advance belongs to the
downstream spawn/task/asset-ready path, not the pad path). The observed
wedge is the already-documented `DAT1=3` battle bring-up event-wait with
the CD sector-ring flow-control divergence (`bfrd_set`/`bfrd_clear`
imbalance, dropped/partial sectors) starving the fighter/asset stream —
see `proof/downstream_spawn_wedge.md` and
`proof/cd_deferred_irq_starvation.md`. The earlier "needs controller-2"
conclusion applied only to the *earlier* phase-seed wedge (`+0xb6==0` in
the menu frame loop, where `mc_pad_button_handler` zeroes both phase
words each frame); even that was ultimately resolved by
`battle_state_init`'s gate-based seeding, not by pad-2.

Runtime note: the headless runtime's slot-1 `press` injection is a
faithful digital-pad-2 device and works correctly — it is simply not on
the critical path of this wedge. No `game/*.c` change is indicated; the
next investigation target is the CD ring/buffer-full accounting that
leaves `bfrd_set` asserted ~2047× vs `bfrd_clear` 13× during the
sustained fighter-asset stream.

## Files/lines cited

- `game/menu.c:2869-2916` pad_state_refresh (sole writer of
  `+0x68..+0x80`); `:4007-4211` battle_config_eval/bcfg_step_a/
  battle_state_gate; `:4154` sole `0x700` store; `:4606-4629`
  battle_state_init 0x300 force-seed; `:5362+` battle_init;
  `:6790+` post_battle_dispatch (+0x92 machine, `pbd_calls=0` at wedge).
- `game/save.c:1604-1640` attr_word_decode; `:1732-1766`
  mc_pad_button_handler; `:2003-2049` mc_slot_action;
  `:3158-3275` select_phase_update (char picks only).
- `game/game.c:592-617` pad buffer staging; `:981` channel table init;
  `:1425+` match_loop stage machine.
- `psxrecomp/runtime/src/sio.c:88-152` pad FSM; `:247-269`
  pad_fill_status8; `:1393-1431` disconnected hi-Z vs connected
  `0x41/0x5A` poll response; `:4452/14810` (main.cpp)
  `sio_set_pad_connected` from `[controller] p2_device`.
- `game_1pad.toml` `[controller] p2_device="none"` — the faithful
  no-controller-2 configuration.
- `proof/cpu_confirm_source.md`, `proof/downstream_spawn_wedge.md`,
  `proof/cpu_phase_wedge_disconnected_pad.md`,
  `proof/cd_deferred_irq_starvation.md`.
