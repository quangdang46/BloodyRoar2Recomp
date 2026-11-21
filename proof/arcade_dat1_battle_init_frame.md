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
