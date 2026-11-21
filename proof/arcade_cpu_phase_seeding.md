# Arcade versus-screen CPU phase wedge — `battle+0xb6 == 0`

Task: test H1 (initial-phase seeding skipped), H2 (attract uses a different
path), H3 (descriptor/vtable misbinding) for the arcade CPU opponent whose
`battle+0xb6` phase stays `0` while P0 `battle+0xb4` reaches `0x300`.

## Verdict

**H1 CONFIRMED — and now with the exact writer identified.** The CPU phase is
not "skipped"; it is *pad-edge-gated* and the CPU has no controller-2, so the
gate never opens. H2 and H3 are disproven.

## The complete mechanism (all disasm-verified clean ports)

### 1. The seeder — `mc_pad_button_handler` + `mc_slot_action`

`mc_pad_button_handler` (`game/save.c:1732`, guest `0x80153264`) runs every
menu frame (called from the menu dispatcher `game/menu.c:2752/2765/2776/2790/2801`).
At entry it **zeroes both phase words**:

```c
param_1[0xb4]=0; param_1[0xb5]=0; param_1[0xb6]=0; param_1[0xb7]=0;   // save.c:1737
```

then re-seeds only the slot whose pad edge fires:

```c
if (*(u16*)(param_1+0x68) & 0x840)  mc_slot_action(param_1,0);   // P0 edge -> slot 0
if (*(u16*)(param_1+0x6a) & 0x840)  mc_slot_action(param_1,1);   // P1 edge -> slot 1
```

`mc_slot_action` (`game/save.c:2003`, guest `0x8015260c`) dispatches on
`battle+0x9b` (the battle/mode selector):

```c
case 0:  phase[param_2]      = 0x100;   // arcade/single — ONLY the pressed slot
case 2:  phase[0]=phase[1]=0x100;       // versus — BOTH seeded by one confirm
case 3:  phase[0]=phase[1]=0;           // clear
```

`param_1` is `guest_at(0x1f800000)` (battle base); `+0xb4/+0xb6` are the P0/P1
phase words, `+0x68/+0x6a` the P0/P1 pad **edge** words.

### 2. Where `+0x68`/`+0x6a` come from — physical pads only

`pad_state_refresh` (`game/menu.c:2869`, guest `0x8012a72c`) is the **only**
writer of the edge words. It decodes the two physical pad records and merges
them — there is **no CPU/controller-type branch**:

```c
uVar2 = attr_word_decode(&DAT_801befa8);   // pad-0 record
uVar3 = attr_word_decode(&DAT_801befca);   // pad-1 record  <- CPU reads THIS
...*(u32*)(param_1+0x68) = edge;  *(u32*)(param_1+0x6c)=hold; ...
```

`DAT_801befca` (pad-1 record) is filled by the pad-poll driver from SIO. With
no controller-2 present it stays released → `attr_word_decode` returns the
neutral word → `+0x6a` edge is always `0` → `mc_slot_action(1)` never runs →
`+0xb6` stays `0` after the per-frame zeroing.

### 3. Why P0 reaches `0x300` but P1 stays `0`

`battle_state_init` (`game/menu.c:4605`, guest `0x80151328`) seeds `0x300`
**only for slots whose phase is already in an eligible class**:

```c
if (select_phase_gate(a2,0)==0) +0xb4 = (v&0xf000)|0x300;
if (select_phase_gate(a2,1)==0) +0xb6 = (v&0xf000)|0x300;
```

`select_phase_gate` (`game/menu.c:3605`) returns `0` (pass) only when the
phase hi-nibble is `0x100/0x200/0x300/0x400/0x700`. P0's `+0xb4` was seeded to
`0x100` by its own pad confirm (`mc_slot_action(0)`), so the gate passes and it
is forced to `0x300`. P1's `+0xb6` is still `0` (class `0x000`) → gate fails →
left at `0`.

### 4. Why versus works but arcade wedges — the `+0x9b` difference

`battle+0x9b` selects the `mc_slot_action` case:

| mode | `+0x9b` | `mc_slot_action` case | effect |
|------|---------|------------------------|--------|
| versus (2P) | `2` | case 2 | **both** `+0xb4`/`+0xb6` seeded `0x100` by P0's single confirm |
| arcade/1P | `0` | case 0 | only the **pressed** slot seeded; CPU needs its own `+0x6a` edge |

Versus survives because one human confirm arms both sides. Arcade expects
slot-1 to independently confirm — which requires controller-2 input the CPU
does not produce through this path.

## Hypothesis verdicts

- **H1 — CONFIRMED.** Initial phase seeding is not skipped; it is gated on a
  controller-2 pad edge (`+0x6a & 0x840`) that the CPU never emits. The precise
  mechanism is `mc_pad_button_handler`→`mc_slot_action` (pad-edge) →
  `select_phase_gate`/`battle_state_init` (class gate).
- **H2 — DISPROVEN.** Attract/demo uses `pad_hist_record`
  (`game/battle.c:5878`) in-fight input playback, a separate channel that does
  not touch `+0xb4/+0xb6` or `+0x6a`. It cannot seed the versus phase and does
  not explain the arcade wedge.
- **H3 — DISPROVEN.** The `+0x28` descriptor tables (`0x801f5258`/`0x801f52a0`)
  built by `players_ctrl_init` (`game/game.c:2542`) are character/stage data
  tables, not input vtables. Phase seeding reads raw pad edge words
  `+0x68/+0x6a`, never through those descriptors.

## Relationship to the live wedge (important)

`proof/arcade_spawn_phase_starvation.md` and
`proof/arcade_dat1_battle_init_frame.md` document a **later** freeze where
`+0xb4==+0xb6==0x0300` (both seeded) and the machine stalls in the CD-ROM
deferred-interrupt coroutine (`cd_timer_read`/`cd_chan_full_init` spin,
`DeliverEvent(0xF0000009)` never firing). That is a **different, downstream**
wedge — the runtime/kernel event-delivery issue, not this phase-seeding one.

This document covers **only** the `+0xb6==0` seeding wedge: once the CPU phase
is armed to `0x300`, the separate CD/event wedge (owned by other proof docs /
the parallel `+0x92`/`0x700` investigation) is what gates actual combat.

## Fix direction (no patch applied — see below)

The clean ports are disasm-faithful; the guest genuinely sources the versus
phase seed from controller-2 pad edges. There is **no CPU-input synthesizer in
the versus/select path** in `game/*.c` (exhaustively searched: `pad_state_refresh`,
`fighter_spawn_setup` input merge `game/menu.c:3841-3922`, `select_screen_tick`,
`post_confirm_config_build`, `select_phase_update`, `match_setup_apply`,
`mc_slot_action`, `vs_confirm_b/e`). Synthesizing a `0x840` edge in game code
would be a guess and is forbidden.

The faithful fix is therefore an **input-layer** concern: the CPU opponent must
present versus-screen input on controller-2 (the pad-1 record `0x801befca` /
`battle+0x6a`), exactly as a real second controller — or a dedicated CPU
"controller" device — would. The runtime already has a controller/SIO layer
(`psxrecomp/runtime`, `sio.h`, per-game `p1/p2_device`/`p2_mode` config). The
correct integration is to drive controller-2 with the CPU's versus confirm when
`battle+0x9b==0` marks slot-1 as CPU — NOT to patch the phase words directly.

Until that input feed exists, `press(slot=1)` reaches `+0xb6=0x300` (confirming
the mechanism) but is not valid proof of autonomous behavior.
