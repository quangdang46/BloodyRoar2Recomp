# Non-arcade/versus mode sweep — attract OK, options OK, two new wedges (menu rows 3 & 5)

Session: headless instance, debug port **9900** (bind verified in
`/tmp/br2_9900.log`). Disc `Game.cue` (SLES-01722). Read-mostly
investigation; no `game/*.c` edits.

> Port note: a second launch on 9901 failed `debug server bind` — traffic
> on 9901 belonged to a *different* agent's instance and was discarded.
> All data below is from 9900 only.

## Menu map (live-verified, `game/menu.c:2819` `attract_menu_dispatch`)

Top-level stepper (`match_loop`, `game/game.c:1543` `sd.max = 7`,
callback guest `0x8014751c`) has **7 rows**. `DAT_1f80009b/0x9c` update on
highlight; rows 6/7 leave them stale:

| row | (9b,9c) | dispatch param | result on confirm |
|-----|---------|----------------|-------------------|
| 1 | (0,0) | 1 | gs=8 → returns to attract cleanly |
| 2 | (2,1) | 2 | versus (already verified) |
| 3 | (1,4) | 3 | **WEDGE-A** (below) |
| 4 | (0,3) | 4 | arcade (known CPU-select wedge) |
| 5 | (3,2) | 5 | **WEDGE-B** (below) |
| 6 | (stale) | 6 | gs=11, `g_sub_state=4` → `system_config_menu_b` |
| 7 | (stale) | 7 | gs=11, `g_sub_state=3` → `system_config_menu_a` |

Nav: DOWN cycles rows; UP/LEFT/RIGHT are **not** menu nav on this screen
(UP=0xFEFF in the old harness notes actually clears bit8=L2 — correct pad
map: UP `0xFFEF`, DOWN `0xFFBF`, LEFT `0xFF7F`, RIGHT `0xFFDF`,
SELECT `0xFFFE`, START `0xFFF7`, CROSS `0xBFFF`).

## 1. ATTRACT / DEMO — HEALTHY

Zero input from boot: `gs` cycled `3→4→5→6` repeatedly over ~16k frames
(gs=4 = `scene_entry` demo-fight load per `game/game.c:582`, gs=6 =
post-match →3). At least 4 full attract cycles observed, frames advancing
~40–70/s, CD healthy (`irq_present_max_late=0`). Pressing START at
`gs=5 sub=1` → `sub=2` main menu. **No wedge.**

## 2. OPTIONS / SYSTEM menu (gs=11) — WORKS

Row 6 → `g_game_state=0xb`, `system_config_menu_b` (`game/menu.c:1162`;
`game/game.c:752-764`). DAT1=1 stepper (`local_5a=0xe` rows for menu_a
variant, `menu.c:1096`). Responded to corrected dpad input: 16×DOWN +
CROSS exited cleanly → `gs=5 sub=1` title. SELECT (`0xFFFE`) alone did
not exit; the exit path is the stepper-callback (`_DAT_1f80007c & 0xe0`
confirm or `& 0x10`) at `menu.c:1047-1058`. **No wedge; full
enter→navigate→exit cycle verified.**

## 3. WEDGE-A — menu row 3, mode `(9b=1, 9c=4)` — CD stream-load stall

Confirm at menu row 3 (`sig=(1,4)`): `gs=8`, `DAT_1f800001=0`,
`b4=0x0300`, then **permanent stall ~35k+ frames**.

Live evidence (port 9900, wedge from ~frame 17028):

- `frame` keeps advancing (~140/s — VBLANK-driven; not a frozen core).
- `bce_calls` frozen at **6** (no `battle_config_eval` per-frame work —
  the game thread is blocked *inside a single call*, not looping).
- `pbd_calls=0`, `cdrom_command_history` tail: last device command
  **frame 17005** (`0x09` Pause exec, `pending=1`); zero CD commands for
  ~35k frames after.
- `cdrom_state`: `pending {cmd:0x09, active:0, phase:1}`, `reading=0`,
  `irq_flag=0`, `last_sector` frame 17005 — **identical signature to the
  known CD deferred-completion starvation** (`pending 0x09 phase 1`
  never drains).
- `bioscall_dump` delta over 8 s: only `B0/0x17` kernel bottom-half
  (~170/s VBLANK servicing); **no TestEvent spin** — different from the
  arcade `gs=9` wedge.
- `sp_ring`: only TCB `0x8FE0` `cd_timer_read`/`cd_chan_full_init`
  per-frame kernel loop — game thread never scheduled back.
- `mdec_state`: `busy=0`, last decode activity pre-wedge.

Path: BFSM `9c==4` case (`game/menu.c:660-677`) → `scene_entry
(0x80010000,1, cfg+0x28>>1&1)` (`game/game.c:1601`) →
`tile_stream_job_init`/`tile_stream_open`/`blk_io_retry` loop
(`game.c:1658-1700`). The tile-stream CD load issued its command stream
(0x02 seek / 0x0E / 0x06 read / 0x09 pause cycling healthily until
f=17005) then the pending `0x09` never completed → `blk_io_retry` spins
forever inside `scene_entry`. **This is the same CD pending-command
starvation class as `proof/cd_deferred_coroutine_starvation.md`, hit
through a different consumer (stage/tile stream vs battle assets).**

`9b=1,9c=4` is plausibly the **demo/CPU-battle or team-battle** entry —
`scene_entry` param_3 is the demo flag and `select_screen_tick(2)` runs
after it.

## 4. WEDGE-B — menu row 5, mode `(9b=3, 9c=2)` — select-screen handshake stall

Confirm at menu row 5 (`sig=(3,2)`): intro FMV played (MDEC
`decode_macroblocks=17456`, `busy=0` after), then parked at
`gs=8`, `DAT_1f800001=6`, `9b=3 9c=2`, `+0x92=0x0400`, `b4=b6=0x0000`
for **~20k+ frames**. No exit via START/CROSS/SELECT on either slot.

Live evidence (port 9900, wedge from ~frame 43040):

- `frame` advances (~70/s); **live per-frame loop**, not a core freeze.
- `freeze_check`: `bce_calls` growing ~470/s (~2/frame =
  `fighter_spawn_setup`→`battle_config_eval`×2 with `param_3=1`),
  `pbd_calls=0` (never reaches `post_battle_dispatch`/state 8).
- Pad words `0x1f800068..0x77` read **all-zero even while mashing
  CROSS/START on slot 0** — `pad_state_refresh` edges not landing (or
  not running) in this mode.
- `bioscall_dump`: cumulative `B0/0x0B TestEvent ≈ 1.94M` is *frozen
  history* — live delta over 10 s showed only `B0/0x17` (~211/s). Not a
  TestEvent livelock.
- `cdrom_state`: idle since f≈42954; `pending {cmd:0x09,phase:1}` again.
- `get_registers` always samples kernel bottom-half
  (`epc=0x8019A648`, kernel sp) — expected; the live loop is clean-C.

Mechanism (clean-verified): `DAT1==6` here is **select_screen_tick's own
substate** (`game/menu.c:3229`), not BFSM's — it calls
`fighter_spawn_setup(...,1)` every iteration (the bce traffic) and exits
only when both per-side records' `+0x24` handshake `==2`
(`menu.c:3289-3291` → `DAT1=0xf0`). Mode `9b=3` routes
`fighter_spawn_setup`'s button-merge to the case-3 tail
(`menu.c:3956-3983`) which writes `rec+8` **only if some `rec+0x24 != 0`**
— and `mc_slot_action` case 3 (`game/save.c:2033`) deliberately zeroes
`+0xb4/+0xb6` and sets `+0x92=0x400`. With pad edges all-zero, nothing
seeds `+0x24` → permanent select stall. Same *family* as the arcade
CPU-select gap (`proof/cpu_input_architecture_select_wedge.md`) but a
different mode path: here even P1 confirm produced no `+0x24` advance,
consistent with `pad_state_refresh` not feeding this flow (or the
record table not being the one being driven).

## 5. Mode row 1 `(9b=0,9c=0)` and row 4 `(0,3)` arcade

- Row 1: `gs=8` flashed (~300 frames, `b4` 0x0001→0x0003) then clean
  return to attract/title. No wedge.
- Row 4 = arcade `(0,3)`: not re-driven this session (known
  `select_phase_gate`/`b6=0` wedge, `cpu_input_architecture_select_wedge.md`
  UPDATE 5).

## Most actionable next investigation

**WEDGE-A is the higher-value target**: the `pending 0x09 phase 1`
non-completion now reproduced through *two independent consumers*
(arcade battle-asset load at gs=9, and `scene_entry` tile-stream at
9c=4). It is a single CD-side bug, not per-mode logic. Next step:
instrument the `pending.cmd==0x09 && active==0 && phase==1` drain path —
find which deferred-coroutine/doorbell should transition `phase 1→exec`
(`cdrom_command_history` shows exec entries carry `func`/`pc`; compare a
healthy stream's last `queued 0x09` vs the stalled one). `cd_seek_param_issue`
/ `cd_reg_window_op` and the `s_irq_i_stat` presentation predicate fixed
in `be17669` are the prime suspects for a residual gap.

WEDGE-B second: determine whether `pad_state_refresh` runs at all in
`9b=3` select (arm a watch on `0x1f800068` during the wedge) — if it
does run, the merged `rec+8`/`+0x24` handshake seeding for mode 3 is
the broken piece; if it doesn't, the mode-3 select entry is missing its
input-refresh call entirely.

## Evidence index

- Runtime: port 9900 `frame`/`read_ram`/`press`/`cdrom_state`/
  `cdrom_command_history`/`bioscall_dump`/`freeze_check`/`sp_ring`/
  `mdec_state`/`get_registers` (this session).
- Clean sources: `game/game.c` (`entry_0x80126234` gs dispatch 588-809,
  `match_loop` stepper 1474-1592, `scene_entry` 1601-1714),
  `game/menu.c` (`attract_menu_dispatch` 2819-2950,
  `battle_flow_state_machine` 553+, `select_screen_tick` 3185+,
  `fighter_spawn_setup` 3853+, mode-3 tail 3956-3983,
  `post_battle_dispatch` 6875+, `system_config_menu_a/b` 984/1162),
  `game/save.c` (`mc_slot_action` 2016+, case 3 at ~2033).
- Prior proofs: `proof/cpu_input_architecture_select_wedge.md`,
  `proof/cd_deferred_coroutine_starvation.md`,
  `proof/cd_dispatch_localization.md`,
  `proof/global_phase_92_driver.md`.
