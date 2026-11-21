# Arcade DAT1=3 wedge — battle_init frame fix + spawn-phase starvation (CD is HEALTHY)

## ⚠ CORRECTION (agent 7f928e12, port 8930) — CD is NOT the bug
All earlier "CD stall" theories in this doc were WRONG. Live evidence:
- CD finished ALL reads correctly and is **idle/healthy**: `reading=0`,
  `response_count=1/1` (the last Pause response legitimately resident),
  `irq_flag=0`, `i_stat=0`, `cd_intc_dispatch` (0x8019A8EC) ticks EVERY
  frame → **IRQs are NOT masked**; the `cd_state_blk_save` in-service
  flag @0x801bcd18=1 is normal/benign, not an open critical section.
- `sector_read_pos=2060/2340` is NORMAL Mode-2 layout (12B header +
  2048B user consumed; 280B EDC/ECC tail intentionally unread).
- The wedge is **game-logic spawn-phase starvation** in `battle_frame`
  DAT1=3, detailed below. Treat every "CD deferred-coroutine / IRQ /
  response-FIFO" hypothesis in this file as SUPERSEDED.

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

## Sharpened wedge — CD Pause(0x09) completion starved (deferred coroutine)

Live re-diagnosis (port 8915) corrected two earlier readings:

- `+0xb6` (CPU phase) reached `0x0300` on 8915 — BUT agent 42283317's
  controlled fresh-instance run (port 8920) proved this was MY earlier
  `press(slot=1)` injection arming it, NOT self-resolution. On a clean
  instance `+0xb6` stays `0x0000`. CORRECTED below.
- REAL wedge A (CPU confirm): `battle+0x6a` (P1 pad edge word) never
  gets the `0x800` Start edge because slot1 is registered as a PHYSICAL
  pad and no CPU/AI injector writes `0x801befca`. `bcfg_step_a` needs
  `(flags&0x800)` → `+0xb6` stuck 0 → `select_phase_gate(s1,1)` fails →
  `post_battle_dispatch` armed path spins at the 10-tick wait.
  `press(slot=1,START)` → `+0xb6=0x0300` proves downstream works.
- REAL wedge B (CD load): `DAT1=3` = caseD_3 → `battle_init` blocked in
  `cd_file_load_sm` on a starved Pause(0x09) completion (below).
  `frame_divisor_get()` (byte@0x1f800019) returns 0 → caseD_3 entered
  battle_init; it never returns because a CD load never completes.

CD stall (cdrom_command_history + cdrom_state + irq_state, frozen):
- Last command seq316 = **Pause `0x09`, pending=1, pending_cmd=0x09**,
  stat=0x02, request=0x80, device i_stat=0x00000004 (CD IRQ bit2 set).
- INTC irq_state: i_stat=0x0, pending=0x0, i_mask flickers 0x0/0xD —
  the CD-complete IRQ is NOT reaching the CPU dispatch.
- cdrom_state: seq=120699 static, sector_read_pos=2060/2340 (read
  delivered 88% then stopped, reading=0), response_count=1 undrained.
- decomp_trace: dispatch→lzss→blk_ret→heapA_w_menu heap writes STOPPED
  (frontier static @0x800e1064).

Early read (superseded — see REFINED AGAIN below): a sector READ
delivered most of a Mode-2 sector then a Pause was issued; the
Pause-completion response byte appeared to sit in the FIFO undrained.
REFINED (live mask-input trace): `cd_intc_dispatch` mask =
`*(u16*)0x1F801070(i_stat) & *(u16*)0x1F801074(i_mask) & 0xD`; with
INTC i_stat=0 the mask drains → dispatch EXITS cleanly.

REFINED AGAIN (frame-audit agent 251d7926, commit 002997d): the Pause
0x09 actually COMPLETED device-side — response pushed, IRQ presented
AND acked (irq_flag=0, FIFO drained). "response_count=1 undrained" was
transient. The REAL stall is game-side, precisely located:

- `cd_state_blk_save` (0x8019aca4, game/cd.c:4329) sets in-service
  `*(u16*)0x801bcd18=1`, masks IRQs via `FUN_8018e310`, stashes mask to
  0x801bdda8/0x801bddac, and calls `FUN_8019ad80`=B0:0x19.
- `cd_state_blk_restore` (0x8019ac04, game/cd.c:4363) unmasks via
  `FUN_8018e300`, restores regs, calls `FUN_8019ad70`=B0:0x18, clears
  in-service=0.
- LIVE: `0x801bcd18=0x00000001` STUCK — save ran (~frame 3904) but
  **restore NEVER ran**. This is a nested critical-section /
  deferred-dispatch window that never closed → IRQs stayed masked → CD
  IRQ starved → `cd_file_load_sm`/`battle_init` blocks forever.
  decomp_trace froze ~cycle 2.6e9; vblank ticks, no game code runs.

This sits inside the `cd_chan_full_init` deferred-coroutine contract
(game/cd.c:4604): `cd_ctx_save(0x801bcd50)` snapshots gpr + registers
the frame via B0/0x19→mem[0x4C18]; on CD IRQ kernel calls
`cd_ctx_restore`(0x8019adcc)→re-enters→`cd_intc_dispatch`. The open
question for agent 7f928e12: which caller pairs state_blk_save→dispatch
→state_blk_restore, and why the restore `jal` is skipped (early-return /
exception / nested reentry leaving in-service set).

Active workstreams: agent 7f928e12 owns Wedge B (this CD
save/restore imbalance — the binding DAT1=3 constraint); agent
474880e3 owns Wedge A (CPU press-start / slot1 controller-type, see
"Sharpened wedge" above); frame-audit agent 251d7926 DONE (002997d).

## Residual / open — spawn-phase starvation (the REAL wedge)

`battle_init` fix is faithful and verified (bringup completes,
`battle_frame` live, CD IRQs fire every frame). The wedge is now
precisely the **inner `battle_frame` spawn gate**, NOT CD:

- `battle_frame` DAT1=3 → `fighter_spawn_setup` (game/menu.c:3767). Per
  player it commits spawn only when `*(u32*)(rec+0x10)==1 &&
  (phase@0x1f8000b4/b6 & 0xf00)==0x700` (menu.c:3786-3789). On commit it
  writes `s3[1]=2` (DAT1 3→2→case2→3, one player per pass, then return).
- `battle_config_eval` (menu.c:4007) advances per-player phase u16
  through classes 0x100/0x200/0x300/0x600/0x700/0x800/0x900. Phase-0x300
  with intro-class `+0x92==0x100` only ANNOUNCES (`L_B0_774`,
  menu.c:4104-4116) and waits for `+0x92` to progress — it does NOT
  advance to 0x700 on its own.
- `battle+0x92` intro class set once by `confirm_bookkeeping`
  (menu.c:3937-3978, values 0x100-0x400 from select_phase_gate); low
  byte ticked by `battle_sides_commit`/post-battle (menu.c:4860-4877).
- LIVE blocked state: `+0x92=0x0100` (intro class 0x100 substate 0),
  `+0x9b=0`, `+0x9c=0`, phases `+0xb4=0x0300` `+0xb6=0x0000`, fighter
  recs `0x801C1200`/`0x801C122C`. P0 armed to 0x300; P1 (CPU) phase
  record never seeded.

Circular-wait hypothesis (agent 7f928e12): spawn waits phase==0x700;
phase-0x300 advance waits `+0x92` progression; `+0x92` progression may
depend on `battle_intro_fsm` (`_DAT_801f0738`, game/battle.c:8827) which
only runs inside the fight-render path AFTER spawn — OR on a
task/script callback (`0x801b2ed8`/`0x801b2ef4`, allocated menu.c:3818)
silently not running.

Two sub-wedges now distinguished:
- **Wedge A (seeding):** P1 `+0xb6` never leaves 0 — CPU press-start
  edge never synthesized (slot1 = physical pad, no AI feed). Agent
  474880e3 owns this.
- **Wedge C (advancement):** BOTH players stuck at phase 0x300 →
  `+0x92` intro class stuck 0x100 → never reach spawn's required 0x700.
  Affects P0 too, so INDEPENDENT of Wedge A. Candidates to trace:
  `battle_intro_fsm`, `select_phase_update`,
  `engine_record_pair_update` (generated 0x8014a708), channel-cursor
  task callbacks `0x801b2ed8`/`0x801b2ef4`.

Note: fntrace only observes generated-dispatch entries — clean-ported
battle functions (battle_frame/frame_tick/battle_config_eval/
fighter_spawn_setup) are invisible to it; use decomp_trace tags or a
clean-entry probe. `gwatch` cannot watch `0x1f800001` (scratchpad
mirror). Frame-audit sweep DONE (002997d) — ~20 sp-frame bugs fixed;
none was the binding wedge.

## Wedge A investigation result (agent 474880e3 — exhaustive, NO fix)

Versus-screen confirm path is FULLY SYMMETRIC pad-driven — zero CPU
branch anywhere: post_battle_dispatch→battle_config_eval→bcfg_step_a
needs `(flags&0x800)` on `battle+0x6a`. `pad_state_refresh`
(menu.c:2888-2913) is the ONLY writer to `+0x68`/`+0x6a` (one u32 store
of both edge words) and reads ONLY physical pad buffers
`0x801befa8`/`0x801befca` via `attr_word_decode`. Device records
`0x801ff080`/`0x801ff170` are byte-identical physical SIO pad pollers.
`pad_hist_record` (battle.c:5878) is the IN-FIGHT CPU/demo playback —
bypasses pad, produces fighter words not `+0x6a`. NO guest routine found
that synthesizes a CPU pad report for the versus screen.

 players_ctrl_init (game.c:2542) builds per-player descriptor vtables
0x801f5258(P0)/0x801f52a0(P1) with DIFFERENT fn-ptrs — the
controller-type binding layer, unresolved.

REFINED HYPOTHESIS (not yet tested): `+0xb6` may need to be SEEDED to
0x100 (or higher) during match_setup/select so it bypasses the
cls-0x000 pad-confirm path — vs synthesizing a pad edge. Check who
writes the INITIAL phase to +0xb4/+0xb6 (bcfg_step_c/d write 0x100) and
whether the CPU side is skipped. ALSO untested: compare ATTRACT's
versus-screen flow (reaches combat) vs arcade — does attract pre-arm or
bypass the CPU confirm via pad_hist_record/AI?

## ⚠⚠ ROOT CAUSE (agent 4e176c55, port 8932) — stalled CD streaming read, NOT phase logic

Spawn-phase starvation is a DOWNSTREAM SYMPTOM. Main thread parked in
CD deferred coroutine `cd_timer_read(0x8019a460)→kernel yield→
cd_chan_full_init(0x8019a814)` once/frame, waiting for a `0x09` CDREAD
completion that never arrives.

cdrom_state frozen: seq stuck, `pending{cmd:0x09,active:0,phase:1}`,
`reading:0`, `sector_read_pos:2060/2340`, `request:0x80`(BFRD stuck),
`irq_flag:0`. **Ring/BFRD counters expose the bug**: `ring_dropped:29`,
`ring_partial:22918`, `bfrd_set:45373` vs `bfrd_clear:41`,
`irq_present_late:29720` — sector ring OVERFLOWED, IRQs landing late at
scale. First SUSTAINED streaming read; earlier "CD healthy" covered
simpler reads. `cd_cb8_dispatch` alive (DAT_801bdde0 advances) — stall is
this read's data-phase completion.

=> REAL BUG is in `psxrecomp/runtime/src/cdrom.c`: sector ring-buffer
flow-control / BFRD / IRQ-present scheduling. Both "CD stall" and
"spawn starvation" are the SAME root cause seen from two angles; Wedge
A (+0xb6 seeding) may ALSO be downstream (resources not loaded). Battle
side exhaustively ruled out (no 0x300→0x700 driver exists; intro_fsm
done; task callbacks post-spawn only).

## ⚠ ROOT CAUSE v3 (agents 9463bc49 + live EvCB/Bioscall) — CD-load-complete DeliverEvent never fires

CD read COMPLETED fully (Setloc 1:58:70→ReadN→Pause thru LBA 8801,
IRQ2 fired+consumed frame 57007, irq_present_max_late=0). Ring/BFRD/
IRQ-late hypotheses DISPROVEN (proof/cdrom_ring_bfrd_stall.md). sector_
read_pos=2060/2340 is the normal Mode-2 boundary (12B hdr+2048B payload;
280B ECC tail unread). ring_partial=BFRD skew are NORMAL lifecycle.

REAL wedge = kernel event-thread scheduling stall AFTER CD completes:
- EvCB slot8 = OpenEvent(0xF0000009,0x20) MARK, status ENABLED — the
  CD-load-complete flag. ONLY armed event (slots 9-15 free).
- A thread is blocked waiting for DeliverEvent(0xF0000009,0x20) to set
  it READY. B0 bioscall: TestEvent(11)/DeliverEvent(7)/WaitEvent(10)
  counts FROZEN; ReturnFromException(23) climbs; pc=0 idle sentinel;
  VBlank/GPU DMA keep cycling; sp_ring ping-pongs kernel scheduler +
  cd_ctx_restore resume region (0x8019A87C) finding no pending work.
- Producer chain that SHOULD DeliverEvent: CD/DMA completion →
  cd_cb7_run(0x8019af84, DMA-done dispatch) → ch4 handler 0x8019B7E0 =
  cd_dma_chan_reset(0x8019b7e8) → 0x801bdee8==0 → cd_bios_svc_07
  (0xF0000009,0x20)=DeliverEvent. Both cd_cb7_run + cd_dma_chan_reset
  are FULL clean-C ports (worked in versus).
- DMA ch4 completed (chcr=0x201,remaining=0) but DICR done bits already
  cleared (dicr=0x00900000 enable-only). cd_cb7_run either saw s1=0 or
  the done-bit/handler dispatch missed for THIS load.
- bios_hle=true: HLE handles DeliverEvent/TestEvent/OpenEvent (MARK→
  READY). If cd_bios_svc_07 reached HLE it WOULD set slot8 READY — so
  the break is UPSTREAM: cd_dma_chan_reset/DeliverEvent never invoked.

Open question for next agent: trace the EXACT completion path for THIS
sustained load (which DMA channel / which IRQ / which registered handler)
and find where the done-bit→handler→DeliverEvent dispatch is dropped —
a state/timing miss versus didn't hit. Versus ran identical chain OK.
