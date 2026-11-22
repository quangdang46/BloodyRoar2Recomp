# CPU phase wedge — disconnected-pad experiment + `vs_confirm_b` seed analysis

Continuation of `proof/arcade_cpu_phase_seeding.md`,
`proof/arcade_cpu_phase_init.md`. This session ran the decisive
**controller-absence** experiment the prior docs could not, and re-read the
versus confirm state machine for an unconditional CPU seed path.

## New runtime evidence — pad-2 fully disconnected

Launched a headless instance with slot-1 **genuinely absent** (not merely
neutral). `build/settings.toml` `[controller] p2_device="keyboard"` overrides
`game.toml`'s `p2_device`, so the test used an exe-dir `settings.toml`
(`p2_device="none"`) + `players=1` via a symlinked exe dir
(`/tmp/br2exe`). Verified before nav:

```
pad_status: slot0 connected:true  slot1 connected:false
pad0  @0x801befa8: 00 41 ff ff ...   (digital pad, neutral)
pad1  @0x801befca: ff 00 00 00 ...   (0xff = NO DEVICE)
```

Arcade nav (slot-0 presses only, no slot-1 injection) reached the **identical
wedge**:

```
gs=8 ph=03 9b=0 9c=0 +92=0100  +68=0000 +6a=0000  b4=0300 b6=0000
```

**Conclusion:** removing controller-2 does not trigger any CPU auto-confirm
fallback. `attr_word_decode(&DAT_801befca)` on the `0xff` record returns
`0xffffffff` → `pad_state_refresh` maps it to `0` → `+0x6a` edge stays `0`.
The game needs an actual slot-1 **button edge**, regardless of whether
controller-2 is present-but-neutral or absent. This refines the fix
direction: it is not "present a second controller" but "supply the CPU's
versus/select confirm as slot-1 pad-edge input."

## `vs_confirm_b` re-read — it CAN seed `0x100` but is downstream-gated

`vs_confirm_b` (`game/menu.c:9307`, guest `0x80152bdc`) is the versus-screen
phase state machine. It contains **unconditional** phase-seed writes that the
prior docs did not highlight:

- `L_2f74` (`0x80152f74`): `+0xb4 = (v & 0xf000) | 0x100` — unconditional P0
- `L_3094` (`0x80153094`): `+0xb6 = (v & 0xf000) | 0x100` — unconditional P1
- `L_316c` (`0x8015316c`): `+0xb4` or `+0xb6 = ...|0x100` on `a3`

…plus `L_d10`/`L_case4` which re-arm `0x100` only when the phase is already
nonzero (`MBS16 != 0` guard). These fire only when the shared versus progress
`battle+0x92 & 0xf00` reaches `0x100/0x200/0x300` **and** `a3` names the
confirming side.

`vs_confirm_b` is reached from `vs_confirm_e` (`game/menu.c:7885`) on a
`MBS16(s0+0x8c) >= 0x12c` (~300-frame) timeout, and from the versus tick
handlers (`vs_confirm_a/c/d` call sites ~7030/7079/7173/7762). **None of these
are reached for the CPU side in the wedge** — the versus/select phase never
advances past the point where the CPU's own confirm would be consumed, because
the CPU confirm itself is the missing input. Chicken-and-egg: the state machine
that could seed `+0xb6` is gated behind the very pad-edge it would replace.

## Why this is still faithful (no game-logic patch)

Every clean port in the chain is disasm-verified (`mc_pad_button_handler`,
`mc_slot_action`, `pad_state_refresh`, `select_phase_gate`, `battle_state_init`,
`bcfg_step_a`, `battle_config_eval`, `vs_confirm_b`). The generated bodies agree.
The CPU genuinely sources its versus/select confirm from **controller-2 pad
edges** — there is no CPU/AI synthesizer in `game/*.c` or `generated/`.

On real hardware the CPU opponent's confirm must therefore arrive on
controller-2. Two candidate mechanisms remain open (handed to a focused trace):

1. A guest pad-driver/BIOS-boundary function fills `0x801befca` from an
   internal CPU-input buffer when slot-1 is a CPU opponent (the channel poll
   registered by `cd_channel_table_init`).
2. The versus-screen per-player record `+8` button flags (read in
   `select_screen_tick` `game/menu.c:3173-3197`) are populated for the CPU by
   an AI path that emits a synthetic confirm.

Until that source is located, the faithful fix is **input/device-layer**: the
runtime must present the CPU's versus/select confirm as SIO slot-1 pad-edge
input when `battle+0x9b` marks slot-1 as CPU — not a phase-word patch in
`game/*.c` (forbidden guess).

## Status

- Wedge reproduces deterministically at `gs=8 ph=03 9b=0 b4=0300 b6=0000`.
- `press {"slot":1,...}` still advances the phase (mechanism confirmed) but is
  not autonomous proof.
- A focused trace (`proof/cpu_confirm_source.md`, in flight) is locating the
  exact CPU-confirm source to design the faithful input-layer fix.
