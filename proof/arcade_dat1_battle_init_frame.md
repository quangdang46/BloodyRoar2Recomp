# Arcade DAT1=3 wedge — battle_init guest-frame fix + residual CD-dispatch stall

## Status
- HEAD before: `8d84f45` (versus fix). This change adds the same
  `br2_guest_frame` reservation to `battle_init` (guest `0x801506d0`).
- Arcade still wedges at `gs=8 DAT1=3` after the fix — see "Residual".

## What was wrong in battle_init
`battle_init` modeled its guest frame via
`auStack_18 = guest_at(guest_sp() - 0x150)` — the sp+0x18 argblock of a
0x168-byte frame — with **no `br2_guest_frame()` reservation**. Every
nested call inside the init loop (`battle_bringup_a`, `task_pool_init`,
`task_pool_alloc`, `record_init_sweep`, `frame_tick`, and the state-6
record sweeps) allocated its own frame in the same window, clobbering
the argblock that is stored to `0x801f10bc` and passed to
`battle_bringup_a` as `sp+0x118`.

Fix mirrors the `8d84f45` pattern: `br2_guest_frame(0x168)` reserves the
window (aligns to 0x170; frame is 8 mod 16 so guest `sp+N` maps to
`sst_frm + N + 8`, `sp+0x18 -> sst_frm+0x20`), freed once before the
single `return 0xff`.

## Runtime evidence (port 8915, post-fix build)
- `0x801f10bc = 0x801FFE58` — battle_init frame cell written (function runs).
- `battle+0x8c = 0xffff` — `battle_bringup_a` bring-up state COMPLETE
  (all `cd_file_load_sm` fighter loads returned). `battle+0x9c = 0`
  (the `==3` sprite blocks skipped).
- decomp_trace showed a real file-load/decompress burst
  (`lzss`/`blk_ret`/`heapA_w_menu`/`dispatch`) then went quiet — the
  sector data was delivered and decompressed.
- CD then froze: `seq` static, `sector_read_pos=2060/2340`,
  `response_count=1`, `pending.cmd=0x09 (Pause) active=0`.
- `sp_ring` leaf = `0x8019A53C`/`0x8019A87C` CD-timer pacer + BIOS
  `0x2924` — the deferred-coroutine wait spin (frames advancing = live
  livelock, not hard hang).
- `0x801bcd18` (cd_intc_dispatch in-service) = `0x00000001` STUCK SET;
  mask `0x801bcd48=0x000d`; `i_stat=0`. Identical signature to the
  versus wedge before `8d84f45`.

## Update — native stack sample re-scopes the wedge (post-fix, :8915)

`sample` on the live PID shows the game is NOT inside battle_init —
battle_init already returned. Real call path:

```
battle_flow_state_machine (menu.c:605, DAT_1f80009c switch case 0)
  -> battle_frame (menu.c:8713)          <- the per-frame battle tick
    -> frame_tick (game.c:1352)
      -> br2_call_generated -> guest 0x8019A5D8 (cd_timer_read region)
        -> psx_check_interrupts_at -> IRQ -> cdrom_advance -> present_cdrom_irq
```

- `DAT_1f80009c=0` + `DAT_1f80009b=0` selects `battle_frame()` (case 0),
  which runs every frame regardless of the `DAT_1f800001` sub-state.
- `battle+0x8c=0xffff` (bringup done) but caseD_3's reset never ran ->
  consistent with battle_init having returned on an earlier pass.
- CD IRQs ARE being delivered (`present_cdrom_irq`/`cdrom_advance` hot).
- Remaining symptom: per-side phase words `battle+0xb4`(P0)=0x300 armed,
  `battle+0xb6`(CPU/P1)=0x0000 NOT armed. `select_phase_gate` needs the
  hi-nibble in {0x100..0x700}. Arcade is 1P-vs-CPU so the CPU opponent's
  record/AI path is the suspect — versus armed both sides (both human).
- ~half the samples sit in frame_tick->cd_timer_read->IRQ->cdrom service;
  the other half in battle_frame's own per-frame work. Frames advance.

## Residual root-cause localization — CPU phase +0xb6 never armed

Traced the `+0xb6=0` wedge through source AND live guest RAM. Full causal
chain now proven:

Per-frame driver: `post_battle_dispatch` (menu.c:6746, guest 0x80154930)
calls `battle_config_eval(param_1/param_2, s1, 0)` for BOTH side records
(menu.c:6764-6765). Each side record's `AB[2]` = slot index (verified live:
P0 rec 0x1f8000e8 byte+2=0, P1 rec 0x1f8001f0 byte+2=1 — both correct).

For CPU slot (`+0xb6`=0 → phase cls=0x000):
`battle_config_eval` → `L_state0` → `B[0x9b]`=0 → `L_A0` → reads global
`+0x92` (live byte=0 → `gcls` passes) → `bcfg_step_a(s1, slot=1, +0x6a)`.

`bcfg_step_a` (menu.c:4187) returns nonzero iff ALL of:
  1. `battle_state_gate(s1,0) != 0`  — live `+0x92` byte=0 → jt_kind[0]=1 → PASSES
  2. `(param_3 & 0x800) != 0`        — param_3 = u16 @ battle+0x6a = P1 edge word
  3. `ret_zero_b(slot) >= 0`

**The sole failing condition is #2: `+0x6a & 0x800 == 0`.**

`+0x6a` is the P1 rising-edge word produced by `pad_state_refresh`
(menu.c:2850): `attr_word_decode(&DAT_801befca)` → packed into `+0x6c`,
edge into `+0x68`(P0)/`+0x6a`(P1). `attr_word_decode` (save.c:1604) reads
the per-player device struct: `+1`=type nibble (`0x40`=digital pad),
`+2/+3`=buttons (active-low, inverted on decode → `0x800` = confirm).

Live `DAT_801befca` (0x801befca): `+0=0x00`(present), `+1=0x41`(digital),
`+2/+3=0xFF,0xFF` = all-released → decode 0. **Static across frames — the
CPU-AI never writes a confirm into the P1 pad struct.** So `+0x6a` never
gets a `0x800` rising edge → `bcfg_step_a` returns 0 → `+0xb6` stays 0.

### Mode comparison (decisive)

- ATTRACT (CPU-vs-CPU): real fight sim works (proof/cd_dir_entry_progress.md)
  → the CPU-input path CAN feed a CPU slot. AI proven.
- VERSUS: harness injects BOTH pads — `press(CROSS/START,…,slot=1)` in
  tools/versus_*.py. Slot1 is driven as a *human pad2*, NOT the CPU AI.
  So versus armed `+0xb6` via injected input; it did NOT exercise the
  CPU-opponent path. (Correction to earlier "both human confirm" note —
  versus is human-vs-human driven by the debug `press` command.)
- ARCADE (human-vs-CPU): first mode with a REAL CPU opponent on slot1.
  `arcade_fight_mash.py` presses slot1 but comments "harmless if ignored"
  — external pad1 input does not reach the CPU's input source.

### Open question (agent 42283317 tracing)

What writes `0x801befca+2/+3` for a CPU slot, and what per-slot
controller-type/mode flag selects "physical pad" vs "CPU-AI" as the input
source? Candidates: the channel-poll pad driver (IntRP 0x801FEFC0 → kernel
queue 2, wired by `call_guest(0x801a1250)` in `cd_channel_init_seq`,
game.c:979-987), a per-slot function-pointer table, or a fight-AI brain
that emits pad words. `players_ctrl_init` (game.c:2542) builds per-player
descriptor tables @0x801f5258/0x801f52a0 (resource/channel ptrs, dumped —
no obvious input mux). The AI/controller is generated code; needs live
write-tracing on `0x801befca+2/+3` to catch the writer (or prove absence)
and the gate that should enable it for arcade slot1.

Delegated to agent 42283317 (CPU-select/phase trace) alongside the
frame audit agent 251d7926.

## Residual / open
`battle_init` fix is faithful and verified (bringup completes, battle_frame
live, CD IRQs fire). The earlier "cd_intc_dispatch in-service-stuck" reading
was a downstream symptom of the live battle loop's CD-timer pacer, NOT the
root cause — native `sample` proved the game sits in
`battle_flow_state_machine→battle_frame→frame_tick→cd_timer_read` with
`present_cdrom_irq`/`cdrom_advance` active. The REAL residual wedge is the
CPU-opponent input path above (`+0xb6`/`+0x6a` 0x800 edge). ~35 unreserved
`guest_at(guest_sp()-N)` sites remain across game/*.c — audit in progress
(agent 251d7926) in case one still corrupts the CPU record/input path.
Active-path suspects already cleared: frame_tick, battle_bringup_a,
deferred_cmd_drain (all clean). cd.c:509 is inside a `#if 0` reference
body (not compiled).
