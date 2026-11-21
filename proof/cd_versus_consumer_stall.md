# Versus CD consumer stall — diagnosis (post-be17669)

Live wedge on port 8125: versus/attract battle (`gs=0x308`, `0x1f800001`=3,
`0x1f8000b4`=0x0300 needs 0x700) — fighter assets never finish loading.

## Symptom chain
`battle_frame` L_case3 → `fighter_spawn_setup` waits `(battle+0xb4 & 0xf00)==0x700`.
Spawn phase pinned 0x300 → battle never reaches fight emit.

## CD state at wedge (cdrom_state / cdrom_command_history / cdrom_trace_dump)
- `reading=0`, `irq_flag=0`, `int1_pended=0`, `int1_lost=0`,
  `irq_present_max_late=0` — be17669 IRQ-presentation fix is HOLDING.
- `pending.cmd=0x09 active=0` — cmd 0x09 = **Pause**; active=0 means it
  COMPLETED (cmd field is residue). Not stuck pending.
- `last_sector.lba=8801 frame=14546` vs current frame ~246000 — **no sector
  delivered in ~230k frames**. CD fully idle.
- `sector_available=1 sector_read_pos=2060/2340` — a sector left ~2060/2340
  consumed (280-byte EDC/ECC tail unread — the known ring_partial benign case).
- `ring_dropped=1`.

## Command history tail (all frame ~14532-14546)
seq170 cmd0x09 Pause (pending=1,func=0x2970,pc=0x80196FC4,i_stat=0x4)
seq171 cmd0x02 Setloc seek params MSF [1,58,70]
seq172 cmd0x06 ReadN (reading=1)
seq173 cmd0x09 Pause (pending=1)
=> After frame 14546: NOTHING. `queued_cmd=0`, `pending.active=0`.

## cdrom_trace_dump — last register activity frame ~14544
Active `ReadN(0x06)` stream (`read_cmd=0x06`, `read_delay` counting down
216372→0, `reading=1`, sectors delivering) → then Pause(0x09) at 14546 →
**no CD register access since frame ~14544**. The read stream was paused
mid-sector (2060/2340 consumed) and never resumed or replaced.

## Guest-side block location (pc_probe on 8125)
- `cd_intc_dispatch` guest 0x8019a8ec: **701 hits** — CD ISR/dispatch RUNS
  every frame. Dispatch path is healthy; it just finds no pending
  completion to deliver.
- IRQ trampoline 0x8019a87c: 701 hits (pairs with dispatch).
- `cd_seek_step` 0x8019d508: **0 hits** — NOT the current waiter.
- Per-frame sp_ring loop: `0x8019A53C` → `0x2924` (BIOS TestEvent) →
  `0x8019A87C`. 0x8019A53C lies inside **`cd_cmd_sequence` (0x8019a00c,
  delegated to generated — clean port regressed boot per cd.c:1110)**.

## bioscall_dump
B0:11 (TestEvent) count = **1,424,248** — the guest polls TestEvent ~7x/frame
for an event flag that never gets DeliverEvent'd.

## Interpretation
cd_cmd_sequence (generated) is the per-frame CD pump. It issued the
ReadN stream then Pause(0x09) — Pause completed device-side and its IRQ
fired, cd_intc_dispatch ran, but the completion did NOT DeliverEvent the
flag cd_cmd_sequence polls (0xF0000009 family). With no pending/queued
command at the device, cd_cmd_sequence waits forever for the next
completion event before issuing the fighter-asset read.

## Hypotheses to test (fix agent)
1. Pause interrupted the read mid-sector (2060/2340); the read-session
   "end of data / command complete" event the game awaits is gated on a
   condition Pause left inconsistent (partial sector / read_delay residue /
   stat_reg READ bit).
2. Pause's CDIRQ_COMPLETE handler inside cd_intc_dispatch DeliverEvents a
   different flag than the read-session waiter polls — the game may be
   waiting for a READ-complete/INT2 event that Pause's INT1/COMPLETE
   doesn't produce.
3. The async-command completion callback DAT_801bc848 isn't invoked for
   Pause in this session type, so no DeliverEvent.
4. cd_cmd_sequence is waiting on a RESPONSE-read (Getstat/GetResponse)
   that Pause's response_push(stat_reg) should satisfy but the game's
   response-consumption state is off-by-one (response_count=1 unread).

## Prior proof refs
cd_dispatch_localization.md, cd_wedge_mistranslation.md,
cd_seek_blocker.md, versus_framedivisor_post_cd.md (frame-divisor resolved).
