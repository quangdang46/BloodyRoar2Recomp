# Spawn-phase wedge — channel-script task frozen at script start

Status: **root cause localized to the per-frame channel-command task
`LAB_80137acc` (`0x80137ACC`) — its script cursor + tick are frozen, so the
intro/fight timeline never reaches the phase-advance command.**

## Where the game is stuck (all verified live, port 9850)

- `DAT_1f800001 = 3` → `battle_frame` (`0x8014CDD0`) `for(;;)` dispatcher,
  `st==3` → `L_case3` → `fighter_spawn_setup(...)` → `goto L_tail` every frame.
- `fighter_spawn_setup` (`menu.c:3772`) spawn-commit predicate:
  `*(u32*)(rec+0x10)==1 && (phase & 0xf00)==0x700` per player. Phase words
  `0x1f8000b4/0x1f8000b6 = 0x0300` — **never reach 0x700**.
- `battle_config_eval` (`menu.c:4012`) at cls `0x300` → `L_state3`→`L_B0`→
  announce only (`stage_fight_announce`); **it does not self-advance 0x300**.
  `bce_calls=198856`, `bce_cls=0x300`, `bce_fzN=0` (always param_3==0).
- The `0x300→0x700` advance must come from the intro/fight **timeline script**,
  driven by the task pools ticked in `battle_frame` `L_tail`
  (`task_pools_tick(1)` + `task_pools_tick(2)`).

## Task-pool state at the wedge (decisive)

Heads `pool1 @0x801FF3E0`, `pool2 @0x801FF428` (16 sub-list heads each,
stride computed `param*0x48 - 0x7fe00c68`):

- **pool2: completely empty.**
- **pool1: exactly ONE active node** — sub-list 6 head → state block
  `0x801D0228` running callback `0x80137ACC` = `LAB_80137acc`
  (the *channel command interpreter task*).
- The intro driver tasks are **absent**: `battle_intro_fsm`
  (`0x80137098`, allocs task `0x80137590`→pool1 sub9) is gated behind
  `param_4+0xb1<0` and the per-player update that only runs at `st>=6`
  — never reached at `st==3`. `_DAT_801f0738 = 0` confirms it never ran.
  `script_stream_tick` (`0x80137824`, `task_alloc_wrap_d`) likewise absent.
- `tpool_fail_count=0`, `tpool_bad_count=0` — allocation is healthy; the
  intro tasks simply were never scheduled on this path.

## The frozen channel-script state (smoking gun)

`LAB_80137acc(param_1)` state block @ guest `0x801D0228`
(`tramp_last_node=0x801D0228`, `tramp_calls≈107k` and climbing ~1/frame):

| node+off | field | value | meaning |
|---|---|---|---|
| +0x08 | `param_1[2]` script cursor | `0x801B2784` | **frozen at script start** |
| +0x0c | `param_1[3]` handler table | `0x801B2764` | |
| +0x10 | `param_1[4]` tick (`*psVar5`) | `0x00000000` | **frozen at 0** |

Sampled repeatedly over minutes: cursor and tick **never change** while the
function is invoked every frame. `LAB_80137acc` is a *timeline* interpreter:
`*psVar5` (tick @ node+0x10) increments once per call; script entries fire
when their leading s16 timestamp `== *psVar5`, then the cursor
(`param_1[2]`) advances +6 per entry. With tick stuck at 0 only the
leading tick-0 entries can fire → the "FIGHT"/phase-advance command is
never reached → phase stays `0x300` → spawn never commits → `DAT1=3` forever.

## Why the tick/cursor don't advance — candidate mechanisms (unresolved)

`LAB_80137acc` (`game/battle.c:16740`) structure:

1. `if (_DAT_1f800082 != 0) return;` — **ruled out**: `_DAT_1f800082` is a
   u16 sim/pause gate; live `*(u16*)0x1f800082 = 0x0000` (gate open).
   (Prior u32-vs-u16 misread already fixed; see comment `battle.c:171-177`.)
2. `sVar2==0x4000` → `task_slot_step(); return;` — **skips `*psVar5++`**.
   But `*psVar3 = *(s16*)0x801B2784 = 0x0000`, not `0x4000`.
3. Dispatch path `LAB_80137c14`: `if (*psVar5==*psVar3)` (0==0 true) → runs
   `handler_table[psVar3[2]]` handlers → advances cursor → `*psVar5++` →
   `return`. **The unconditional `*psVar5++` is not landing** → the only
   remaining explanations:
   - the dispatched **handler longjmps/bails** (`br2_bail_pending` unwind)
     before `*psVar5++`, every frame — handler_table[0]=`0x8014BFE4`
     (`options_pages_reset` per renames.map — possibly mislabeled);
   - an inner `do/while` control-op cycle (0x1000/0x2000/0x3000) loops
     without reaching the increment (would hang the frame — frames DO
     advance, so unlikely);
   - `param_1` state block is reset/realloc'd each frame (fresh tick=0) —
     but node addr is stable `0x801D0228`.

## Exonerated (do NOT re-chase)

CD device/IRQ/event/doorbell, INTC, OpenBIOS EvCB delivery, SIO/controller
input, cdrom.c ring/BFRD flow control, decompression (finished seq 213 —
legit sparse `-1` slots), `cd_file_load_sm`/`cd_mc_state_machine` (frozen
counters = done, not called), kernel thread/event waits (subagent
`b01e1fe8`: faithful, no parked thread — CD idles healthily),
`battle_intro_fsm`/`script_stream_tick` (never scheduled at st=3).

## Next decisive step

Instrument `LAB_80137acc` to record **which exit path** each call takes
(`_DAT_1f800082` early-return vs `0x4000` slot-step vs normal dispatch) and
whether the dispatched handler sets `br2_bail_pending`. Then read the
generated disasm of `0x80137ACC` + handler_table[0] `0x8014BFE4` to find the
faithful fix (likely a clean-port bail/unwind or a handler that should
advance the timeline but is gated on a flag that never sets).
