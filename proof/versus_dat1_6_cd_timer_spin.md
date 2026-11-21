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
