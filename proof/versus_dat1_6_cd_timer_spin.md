# Versus wedge is DISTINCT from arcade — DAT1=6 setup hang in cd_timer_read deferred-coroutine pacer

Fresh `:8902` driven by `tools/versus_fight_mash.py` (proper dual-pad nav):

- Reached `gs=8` versus (`sig=0201`), **BOTH phases `+0xb4=+0xb6=0x0300`**
  — versus phase-init WORKS (both human players confirm; unlike arcade
  where CPU `+0xb6` stays 0).
- Then wedged at **`DAT1=6`** for 57s+ (`8c=0xffff`), never reaching
  `DAT1=8` (caseD_8 = the player fight the mash script watches for).

`sp_ring` on the wedge shows the SAME deferred-coroutine/CD-timer pacer
loop seen in earlier stalls:
```
target=0x8019A87C  ra=0x8019A87C   <- cd_chan_full_init (deferred CD coroutine)
target=0x8019A53C  ra=0x8019A53C   <- cd_timer_read (GPUSTAT+vsync pacer)
```
Frames kept advancing (61211->61213) — not frozen, looping. A `caseD_6`
setup sub-call (per_side_setup / action_enter_fwd / fighter_anim_table_init
/ a resource load) is waiting on a condition the deferred CD coroutine
never satisfies.

## Why this matters

- Arcade (`gs=8 DAT1=3`, `b6=0`): phase-init bug — CPU opponent phase
  never enters a select class (bcfg_step_a needs 0x800 confirm-edge on
  `+0x6a`). UPSTREAM of setup.
- Versus (`gs=8 DAT1=6`, both `0x300`): phase-init SUCCEEDED; hang is in
  `caseD_6` heavy setup — a deferred-coroutine/CD-resource wait. DOWNSTREAM
  and a different root cause (same family as the be17669 FMV wedge:
  deferred-IRQ coroutine resume ordering).

Versus is FURTHER along — fixing its `caseD_6` hang may reach
`caseD_8` combat independently of the arcade phase-init bug.

## Precise stall signature (:8903, frozen over 9s+)

`cdrom_state` at the wedge:
```
reading=0  sector_read_pos=2060/2340  sector_size=2340
pending={cmd:0x09, active:0, delay:0, phase:1}
seq=30599 (frozen)  irq_flag=0x00  irq_present_max_late=0
last_sector lba=8769 mode=0xA0
```

`2060 = 12-byte header + 2048 data` — the useful payload is fully read;
the trailing 280 bytes are EDC/ECC. The sector delivered its data, but
the NEXT command (`0x09`) sits `active=0` in `pending` and never issues
— `seq` and `sector_read_pos` are both frozen. The deferred
command-issue coroutine (`cd_chan_full_init`, spinning in
`cd_timer_read`/`vsync_wait_n`) is not resuming to push `0x09` to the
drive.

So versus `caseD_6` hangs because a fighter/stage CD read's *follow-up*
command never issues — same deferred-coroutine/command-issue family as
be17669 and the AGENTS.md §8 `cd_seek_param_issue->cd_reg_window_op`
blocker, but a distinct trigger (post-sector command-issue, not stream
chunk delivery).

## Mechanism refinement (:8903 live wedge)

cmd `0x09` = **CD Pause**. Sequence at the wedge:
- Game read sector LBA 8769 to `2060/2340` (header+2048 data = full
  payload; trailing 280 = EDC/ECC), then issued **Pause** to flush
  before the next asset load.
- Pause ran `stop_read_stream`/`set_irq(CDIRQ_ACK)`/`pending_arm(0x09)`;
  `pending` cleared (`active=0`) => Pause-complete fired.
- **No follow-up command ever issues**: `seq` frozen, `reading=0`,
  `pending.cmd` stays `0x09`, `i_stat=0x01` (VBLANK only — no CD IRQ
  bit2), BIOS histogram idle (no TestEvent spin).
- The deferred coroutine `cd_chan_full_init` spins in
  `cd_timer_read`/`vsync_wait_n` — frames advance, but it never drains
  the command queue to push the next read.

So the guest's post-Pause CD-wait / deferred-coroutine resume is not
completing: the game's CD command-issue path stalls after Pause, so the
`caseD_6` fighter/stage asset load never finishes -> `DAT1` stuck at 6.
Distinct trigger from be17669 (that was stream chunk-deadline
advertising; this is post-Pause command-issue / coroutine resume).

## INTC dispatch state at wedge (:8910, gs=8 DAT1=6)

Guest CD INTC dispatcher state block @0x801bcd18:
- +0 u16 in-service flag = **0x0001 (SET)** — cd_intc_dispatch entered and
  is stuck mid re-poll loop.
- +2 u16 busy flag = 0x0000.
- handler table @0x801bcd1c: [0]=0x8019ae68 = cd_cb8_dispatch.
- software mask @0x801bcd48 = 0x000d (bits 0,2,3).
- ptrA=0x1f801070, ptrB=0x1f801074 (CD IRQ flag/enable regs).
- mask = *ptrA & *ptrB & 0x000d.

cd_intc_dispatch re-polls until the interrupt mask drains (clears each
set bit in *ptrA then calls tbl[i]); tail bumps "intr timeout" counter
@0x801bddb0 when a bit never clears. In-service stuck SET + coroutine
spinning in cd_timer_read => the CD-complete interrupt bit is not being
cleared/dispatched, so the completion flags the post-Pause wait needs
never get set -> next command never issues.

Device side verified correct: process_pending case 0x09 clears pending,
response_clear, pushes status, set_irq(CDIRQ_COMPLETE)->INT2,
fire_cdrom_irq. pause_complete_delay ~1.1M cycles (~2 frames) per
psx-spx. So INT2 IS generated; the guest INTC dispatch is not draining it.

## ptrA/ptrB identified = CPU INTC regs (memory.c:854-855,1107-1109)

0x1F801070 = i_stat (interrupt status, AND-acknowledge),
0x1F801074 = i_mask (interrupt enable). So cd_intc_dispatch mask =
i_stat & i_mask & 0x000d — it watches GLOBAL IRQ bits 0,2,3, not a
CD-private INTC.

Live irq_state on wedged :8910: i_stat oscillates 0x00->0x01 (bit0 =
VBLANK re-fires each frame and is consumed normally), i_mask=0x000D
(bits 0,2,3 enabled), IM2=1 IEc=1. **bit2 (CD-ROM IRQ line) is NEVER
pending in i_stat** — only bit0 flickers. So the CD-complete interrupt
either already fired+consumed without reaching cd_intc_dispatch, or is
never presented as a pending INTC bit. cd_intc_dispatch's completion
handler therefore never runs -> the flags cd_result_wait_b waits on
(~0x801bcb25/26) never set -> post-Pause next command never issues.

Next: determine whether the CD-complete INT2 reaches the CPU INTC as a
pending bit at all (present_cdrom_irq -> i_stat bit2), and whether the
deferred cd_ctx_restore->cd_intc_dispatch path is what consumes it vs
the normal INTC dispatch. in-service flag @0x801bcd18 stuck 0x0001.
