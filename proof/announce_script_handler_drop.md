# Announce wedge — script2 generated-handler drop (DAT1 8 → 9 → 0xF0 never fires)

Date: 2026-11-23. Build: HEAD `cecdfd7` (+ working tree). Disc: SLES-01722 PAL, `game_1pad.toml` (`bios_hle = true`).

## TL;DR

The announce wedge is **not** the class-`0x300` phase wait investigated earlier —
it is one stage earlier in the same `battle_frame` state machine. The channel-script
task allocated at `DAT1==7` (`task_alloc_wrap_e(0, 0x801b2820, 0x801b2844)`,
`game/menu.c` `L_case7`) is **designed** to end the announce: its last two timed
records invoke generated handlers that write `DAT1=9` (tick 80) and `DAT1=0xF0`
(tick 81). Live `gwatch`/`fntrace`/`pc_probe` prove the script runs to its
terminator but **neither `DAT1` write ever executes** — the two generated
handler invocations are silently dropped inside `br2_call_generated`
(`src/clean_bridge.c:488`, the `g_psx_call_bail` early-return is the only
no-dispatch path). `battle_frame` therefore spins forever at `DAT1==8`.

## The announce state machine (game/menu.c `battle_frame`, guest 0x8014CDD0)

- `L_case7` (`0x8014dddc`): `task_alloc_wrap_e(0,0x801b2820,0x801b2844)` then
  `MW8(0x1f800001,8)` — allocates script2, enters announce.
- `L_case8` (`0x8014ddf8`): every frame — `battle_config_eval` ×2 (announce
  HUD) + spawn watcher: for each fighter record `r=T+i*0x30`, if
  `*(u32*)(r+0x10)==1 && (phase&0xf00)==0x700` → `B36_8(0x3c)=1`,
  `B36_8(0x3d)=i`.
- `L_case9` (`0x8014de74`): if `B36_8(0x3c)` → fighter spawn commit
  (`record+0x24=0`, `+0x10=0`, DAT1=1 → back to fight loop).
- `L_casef0` (`0x8014df34`): `deferred_cmd_post(0x204)`,
  `victory_jingle_schedule`, `DAT1=0xf1→0xff` → `battle_frame` **exits**;
  the outer FSM re-enters battle → round-1 fight.

So the intended announce exit is `DAT1: 8 →(script tick80)→ 9 →(script tick81)→
0xF0 → 0xFF (exit)`. The `0x700` phase watcher is for later-round/continue
transitions, not round-1 announce completion.

## Script2 record stream (decoded from live RAM @0x801B2844 and from disc image)

Handler table `0x801B2820`: `[0]0x8014CAB4 [1]0x8014CB64 [2]0x8014CC00
[3]0x8014CCD0 [4]0x8014CD48 [5]0x8014CD68 [6]0x8014CD88 [7]0x8014CDA8
[8]0x8014CDBC`.

Records (tick,count,hidx), 6 bytes each, terminator `0x4000` @ `0x801B287A`:

| tick | count | hidx | target | effect |
|------|-------|------|--------|--------|
| 10 | 1 | 0 | 0x8014CAB4 | clean `desc_slot_fill_x15` |
| 20 | 1 | 1 | 0x8014CB64 | clean `menu_quad_reset_redraw` |
| 30 | 1 | 2 | 0x8014CC00 | clean `dual_redraw_b` |
| 55 | 1 | 3 | 0x8014CCD0 | clean `display_list_poke_redraw` |
| 58 | 1 | 4 | 0x8014CD48 | **generated** — `ctx+0x1C |= 0x100` |
| 60 | 1 | 5 | 0x8014CD68 | **generated** — `ctx+0x1C |= 0x200` |
| 62 | 1 | 6 | 0x8014CD88 | **generated** — `ctx+0x1C |= 0x400` |
| 80 | 1 | 7 | 0x8014CDA8 | **generated** — `sb 9 → 0x1F800001` (DAT1=9) |
| 81 | 1 | 8 | 0x8014CDBC | **generated** — `sb 0xF0 → 0x1F800001` |

(`ctx` = `*(u32*)0x801F10BC` = battle_frame frame `sp+0x18`.)

`LAB_80137acc` (`game/battle.c:16740`, verified instruction-for-instruction
against `SLES_017.22` disasm @0x80137acc–0x80137ccc) invokes each handler via
`fn_lookup` → clean call, else `br2_call_generated(fn_g, param_1[0],0,0,0)`.
Handlers 4–8 are **not** in `fnmap.c`/`clean_table.c` → all five go through
`br2_call_generated` → `psx_dispatch_call` → generated bodies
(`br2eu_func_8014CD48/68/88/CDA8/CDBC` in `generated/SLES_017.22_full_15.c`).

## Runtime evidence (two independent wedged instances)

Instance A (port 9997, prior thread) and instance B (port 9999, this session,
frame ~15844):

- `freeze_check`: `tramp_calls=177`, `tramp_last_node=0x801D03D0`,
  `tramp_last_cur=0x801B287A` (terminator), `tramp_last_op=0x52` — script2
  consumed every record and exited via `task_slot_step`.
- Node payload @`0x801D03D0`: `[0]0x801D03E4 [1]0x801B2844 [2]0x801B287A
  [3]0x801B2820 [4]0x00000052` — counter frozen at 82 exactly as the
  terminator path predicts.
- `gwatch` on `0x1F800001` (instance A): last write `DAT1: 7→8` at frame
  ~3927; **zero** subsequent writes across >100 frames of wedge.
- `fntrace_arm`+`pc_probe_arm` on `0x8014CDA8`,`0x8014CDBC`,`0x8014CD48`,
  `0x8014CD68`,`0x8014CD88`,`0x80137ACC` (instance B): **all counts 0** —
  none of the generated handlers ever entered dispatch.
- `read_ram 0x1F800001` = `08` indefinitely; phase words `0x1F8000B4/B6`
  = `0x0300` (unchanged — consistent with the earlier faithful-wait finding
  for the *phase* axis; the DAT1 axis is the actual wedge).

Conclusion: records 7 and 8 **did** match (cursor could not otherwise reach
`0x4000`), so `br2_call_generated(0x8014CDA8,…)` and
`br2_call_generated(0x8014CDBC,…)` were invoked — and produced **no dispatch,
no store, no trace**. The only silent no-dispatch path in
`br2_call_generated` is `if (g_psx_call_bail) return cpu->gpr[2];`
(`src/clean_bridge.c:488`).

## Why this wedges permanently (mechanism)

`g_psx_call_bail` is the wild-flow unwind flag. It is set inside
`psx_dispatch_impl` (`generated/OpenBIOS_dispatch.c` ~line 9155) when a
generated callee C-returns with `cpu->pc==0` and an `($ra,$sp)` mismatch, and
cleared only when a dispatch iteration observes the wild flow land exactly on
a call contract (`stop_addr`/`sp_at_call` match → `bail_resolved`, or
outermost flatten → `bail_flattened`).

Clean-C callers have **no CPS frame** to absorb that unwind:
`LAB_80137acc → br2_call_generated → psx_dispatch_call` is a nested dispatch;
if bail is set on entry, `br2_call_generated` returns `cpu->gpr[2]` without
dispatching and without restoring guest regs — and every subsequent
`br2_call_generated` in the same clean-C activation hits the same early
return. Nothing inside `battle_frame`'s `for(;;)` ever reaches a dispatch
boundary that could resolve the bail, so the flag stays latched and **every
generated fallback call in the battle is silently dropped for the rest of the
battle** — including script2's `DAT1=9`/`DAT1=0xF0` writes. The same mechanism
explains earlier-stage wedges: script1 (allocated `L_case1`,
`task_alloc_wrap_e(0,0x801b2764,0x801b2784)`) has **all-generated** handlers
(`0x8014BFE4…0x8014C8D4`); its tick-62 op `0x8014C8D4` is the `DAT1=5` writer.
If bail latches during `DAT1==3`, script1's writes drop too → `DAT1` freezes
at 3 → `battle_frame` spins → observed "frame freeze" wedges.

This also explains the flaky `DISPATCH FATAL: misaligned target` crashes
(`0x0000E69E`, `0x0000EE93`, …): when the latched wild `cpu->pc`/`$ra`
eventually does reach a dispatch boundary (IRQ path, nested call), it is
dispatched as a real target → wild jump.

## Status: defect demonstrated, fix NOT yet implemented

- **Demonstrated**: the announce exit writes are real game code being
  silently dropped; `battle_frame` wedges at `DAT1==8` as a direct result.
  This is a clean-port/runtime-contract defect, not faithful gating.
- **Not yet proven at runtime**: that `g_psx_call_bail` was actually set
  during the wedge (the only silent-drop path found by audit; freeze-dump
  `bail_first==bail_resolved` samples were captured on the *separate*
  frame-~1600 kernel freeze, not on a `DAT1==8` wedge).
- **Blocked verification**: entering battle with input now reliably hits a
  *separate* known runtime bug — `DISPATCH FATAL: misaligned target` /
  `cpu.pc=0` kernel idle at ~frame 1600 (IRQ storm at `0x8019A600/0x8019A648`,
  `ra_exit=0x8019A53C`, kernel-stack `entry_sp=0x801FFDF8`; the documented
  nested-IRQ kernel-stack ra-corruption family, proofs `165fc89`/`bc34ea8`/
  `0129f3e`). Reproduces identically on pre-`cecdfd7` timing → **not** caused
  by the CRTC commit. Idle (no-input) run is stable at ~270 fps.

## Recommended next step

1. Expose `g_psx_call_bail` (+ a `br2_call_generated` bail-drop counter) via
   the debug server; re-run to the `DAT1==8` wedge and confirm the flag is
   latched when script2's tick-80/81 invokes fire.
2. Fix the unwind contract for clean-C→generated calls: either propagate the
   bail out of clean C (check after each `br2_call_generated` and return so
   the outer dispatch can flatten), or make `br2_call_generated` resolve a
   stale bail instead of dropping the call. Must preserve the legitimate
   unwind path used by `cd_*` delegations.
3. The frame-~1600 battle-entry crash is a separate blocker for all
   battle-path verification; it belongs to the IRQ/kernel-stack workstream.
