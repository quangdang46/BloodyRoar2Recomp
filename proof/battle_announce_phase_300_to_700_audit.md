# Battle announce phase 0x300→0x700 — exhaustive writer audit + live wedge proof

Live specimen: headless arcade, debug port **9986**, `game_1pad.toml`.
Wedge signature: `DAT_1f800001 = 8`, `+0xb4 = +0xb6 = 0x0300`,
`+0x92 = 0x0100`, frame ~5800+, `battle_frame` spinning in its inner
`for(;;)` dispatcher (`game/menu.c:8080`, guest `0x8014cdd0`).

## Question

Find the guest writer that advances the per-player battle phase
(`u16 @ 0x1F8000B4` / `0x1F8000B6`, hi-nibble class) from `0x300`
(announce) to `0x700` (ROUND-FIGHT / spawn-ready), or prove the
transition is gated on a missing task/resource.

## Runtime evidence (port 9986)

- `mem_words 0x1f8000b0`: `+0xb0=1`, `+0xb4/+0xb6 = 0x03000300`,
  `+0xb8=0`, `+0xbc=0x801F0270`. Both phase words pinned at class 0x300.
- `gwatch` armed on `0x1F8000B4` (len 8) + `0x1F800092` (len 2):
  **zero writes** for the entire observed wedge window — no code path
  touches the phase words at all while DAT1=8 polls.
- `press {"slot":0,"buttons":0xfff7}` (decoded edge 0x0800): phase words
  unchanged. Expected — `bcfg_step_a` only runs for phase class 0x000.
- Record table (battle_frame guest frame, `T = sp+0x40` @ `0x801ffe80`,
  stride 0x30): rec0 `+0x10 = 0`, rec1 `+0x10 = 1` (already "ready").
  case8 needs `rec+0x10==1 && phase==0x700` for the SAME player — rec1
  is ready but its phase is 0x300, not 0x700.
- `decomp_trace` tail: all LZSS block decodes finished (2533 events,
  none new over a 2 s window) — fighter/menu asset decompression is
  complete. CD/file-load counters long frozen at completion.
- Channel scripts: both streams reached their `0x4000` terminators
  (`tramp_calls=177`, cursors `0x801B27B4`/`0x801B287A`). Alive task
  nodes: `0x801D088C` → `LAB_80137680` (script cursor, pool 1,2) and
  `0x801D80E0` → `0x8015B810` (options-menu node, pool 2,2). No
  fight-start / fighter task nodes allocated yet.
- Task-pool failure latch: `tpool_fail_count=1`, `p=(2,2)`,
  `size=0x24D8` — traced to `options_menu_alloc` requesting
  `task_node_alloc(2,2,0,0x8015B810,0,0x24BC)` (+0x1C node header).
  Pool `(2,2)` first-free block `0x844` — genuine exhaustion/fragmentation
  of a MENU allocation, unrelated to the battle phase wedge.

## Phase-writer audit (clean + guest asm)

`battle_config_eval` (`game/menu.c:4012`, guest `0x80153544`,
`decomp/FUN_80153544_full.asm`) is the ONLY per-frame phase evaluator
reachable inside `battle_frame`'s loop (called every frame from
`L_case8` `menu.c:8471-8472` with `fz=1`, and from `fighter_spawn_setup`
in cases 3/6). Class dispatch:

| cls | behavior | writes phase? |
|-----|----------|---------------|
| 0x000 | `L_A0_body` → `bcfg_step_a` → `L_set700` | **0x700** (needs pad edge `+0x68 & 0x800`) |
| 0x100 | → `(ph&0x100)|0x200` | 0x200 |
| 0x200 | terminal | no |
| **0x300** | `L_B0` → `L_B0_774`/`L_B0_7cc`/`L_B23` → `stage_fight_announce`/`fight_timer_announce` | **NO — announce/HUD text only, every sub-path** |
| 0x400/0x500 | `L_ret68` | no |
| 0x600 | `continue_countdown`; r>0 → `L_set700`, r<0 → 0x800 | **0x700/0x800** |
| 0x700 | `bcfg_step_c` | no |
| 0x800/0x900 | → 0x900, low-byte counter | 0x900 |

`L_set700` (guest `0x801538a4`, `sh $v1,0xb4($v0)` v1=0x700) is reached
ONLY from the cls-0x000 `bcfg_step_a` path and the cls-0x600
`continue_countdown` path. Neither player is at 0x000 or 0x600.

Writers of intermediate classes:
- `0x500`: `post_battle_dispatch` `L_cwin`/`L_dwin` (`menu.c:7125/7151`)
  — post-match round-win only; `vs_confirm_b` (`menu.c:9434/9458` writes
  `(ph&0xf000)|0x600`) — versus confirm screen only.
- `0x500→0x600`: `fighter_spawn_setup` (`menu.c:3785-3786`) — but 0x500
  is never written pre-fight in arcade.
- `0x300`: `select_confirm_handler` (`menu.c:3623`) — select confirm,
  already done.

`post_battle_dispatch` is NOT reachable: it runs at outer-FSM byte 8
only when `+0x9b != 0` (versus); arcade `+0x9b==0` routes to
`battle_frame()` (`menu.c:605`), which loops internally until
`DAT1==0xff` and never yields to the outer FSM.

## DAT1=9 has no in-function writer

Guest asm `FUN_8014cdd0.asm`: case8 (`0x8014ddf8`) calls
`battle_config_eval` ×2 then scans records for `+0x10==1 && phase==0x700`,
storing flags at `sp+0x3c/0x3d`, then `j 0x8014df6c` (L_tail). **No
`sb` to `1($s5)` (DAT1) anywhere in case8.** The dispatcher
(`0x8014cf14`) reaches case9 (`0x8014de74`) only when `DAT1==9`, but
nothing inside `battle_frame` writes 9 — it must come from an external
agent (task callback via `task_pools_tick`, deferred command, or script
op). This is moot until a phase hits 0x700 anyway.

## Conclusion

**Faithful wait / missing trigger — NOT a clean-port defect, and NOT the
task-pool failure.**

The announce→fight transition has NO self-advancing path from phase
class 0x300 anywhere in the arcade `battle_frame` reachable set:
`battle_config_eval` is terminal-announcement-only at 0x300, the only
0x700 writers require classes 0x000/0x600 that nothing reaches pre-fight,
and the intermediate 0x500/0x600 writers live in `post_battle_dispatch`/
`vs_confirm_b` — versus/post-match machines that do not run for arcade
(`+0x9b==0`).

The most probable faithful mechanism is an **external trigger that never
fires**: either (a) a task/script-VM op handler that writes phase or
DAT1=9 on announce completion — the announce scripts ran to their
terminators but no surviving task node performs a phase write; or
(b) the versus-style `post_battle_dispatch`/`vs_confirm_b` phase machine
is genuinely the only 0x300-exit and arcade first-fight relies on a
different entry into it. Identifying the exact guest trigger requires
either a runtime watch on `DAT1`/`+0xb4` writes from generated (non-clean)
code during a versus run (where `post_battle_dispatch` DOES execute) or a
full generated-code audit of every `sb/sh` to `battle+0xb4/b6` and
`battle+1` reachable from `task_pools_tick` callbacks.

No code change made — no defect demonstrated. The task-pool `(2,2)`
`0x24D8` allocation failure is an unrelated menu-object exhaustion.
