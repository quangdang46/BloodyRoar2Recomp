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

Traced the `+0xb6=0` wedge through source:

- `battle_state_init` (menu.c:4552) writes `+0xb4`/`+0xb6 = 0x300` ONLY
  when `select_phase_gate(a2,idx)==0` (menu.c:3586), i.e. the phase
  hi-nibble is ALREADY in {0x100,0x200,0x300,0x400,0x700}. `+0xb6`=0 ->
  gate fails -> never armed.
- `match_setup_apply` (save.c:3260) does NOT touch phases (rules/timer/
  stage/chars only). The initial `0x100` arm must come from select /
  CPU-confirm, upstream of battle_state_init.
- Phase machine: `vs_confirm_b` (menu.c:9253) / `vs_confirm_e` (7843/7845,
  a3=0/1 per side). `+0xb4`/`+0xb6`->`0x100` arms at ~9303/9337 (guarded
  `MBS16!=0`) and `L_2f74`/`L_3094` (~9370/9394).
- Versus arms BOTH sides (2 human pads). Arcade's CPU opponent uses an
  auto-confirm path that apparently never arms `+0xb6` — game-logic or
  CPU-record issue, possibly distinct from frame corruption.

Delegated to agent 42283317 (CPU-select/phase trace) alongside the
frame audit agent 251d7926.

## Residual / open
`battle_init` fix is faithful but did NOT alone unblock arcade. The
same `cd_intc_dispatch` in-service-stuck signature persists, so either
(a) another unreserved `guest_at(guest_sp()-N)` site in the active
battle_init→frame_tick→CD-wait call tree is still clobbering state, or
(b) a genuine deferred-coroutine resume defect remains that versus
avoided via timing. ~35 unreserved sites exist across game/*.c
(menu.c, save.c, game.c, cd.c, battle.c, gfx.c) — audit in progress.
Active-path suspects already cleared: frame_tick, battle_bringup_a,
deferred_cmd_drain (all clean). cd.c:509 is inside a `#if 0` reference
body (not compiled).
