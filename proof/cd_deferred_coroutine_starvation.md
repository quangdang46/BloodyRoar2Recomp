# UNIFIED ROOT CAUSE — CD deferred-coroutine / BIOS-event starvation

Single runtime bug underlying ALL remaining fidelity/gameplay gaps.

## Symptoms (one mechanism, three surfaces)
1. ATTRACT FMV truncates ~87%: MDEC output_pos climbs to ~10.83M/12.41M
   every cycle then stops; dma_in_words frozen; movie-end flag fires and
   scene_entry exits gracefully. Video fidelity gap, not a hard hang.
2. PLAYER COMBAT (arcade-1P + versus) never reaches real fighting:
   battle_init/menu_res_load_sm wedge in CD file loads — cd_stream_mode_init
   /cd_file_load_sm retry cd_mode_flag_set forever, 19 identical seeks,
   pending_cmd=0x09 frozen, fighters hp=-1, inputs ignored.
3. GPU GP0 stray word 0xFE1F220E: stale guest coordinate word in a packet
   payload — consistent with resource loads that never completed leaving
   packet-pool slots uninitialized (agent 3647246a).

WHAT WORKS: attract DEMO (CPU-vs-CPU) runs REAL fight simulation
(decomp_trace evop/battle80, fighter act/mpos change) because its assets
are resident and never hit the streaming CD-load path.

## Evidence the CD DEVICE is healthy
- cdrom_command_history: repeating 0x02(seek)->0x09->0x06(read) cycle;
  every command EXECUTES; i_stat=0x04 (CDROM IRQ bit2) fires per command.
- Sectors deliver and are consumed to byte 2060 = 12hdr+2048data (normal
  Mode-2 consumption; the 280-byte tail is intentionally unread).
- irq_state: i_mask=0x0D enables CDROM; between commands only VBLANK shows.

## The actual wedge (guest-side)
The deferred-interrupt coroutine that processes CD completion is STARVED:
  CD IRQ -> cd_intc_dispatch -> cd_response_pump -> cd_mc_state_machine
which marks channel-table entries state==2 (ready) and advances the
dir-entry cursors 0x801fc6a4/a8/ac.

- cd_channel_poll (game/cd.c:1663) polls entry u16 state@+0 == 2 at
  base*(0x801fc6c0)+consume(0x801fc6ac)*0x20; observed stuck at state=1.
- cd_mc_state_machine is kicked by flag 0x801FC68C or cb slot 0x801BC84C;
  0x801FC68C reads 0 throughout the wedge -> never kicked.
- blk_io_retry->mc_date_serial_cmp (delegated 0x80159ab4)->cd_channel_poll
  returns 1 forever -> -1 after 2000 -> scene_entry retries via
  cd_stream_mode_init.

This is the AGENTS.md section4 failure mode made concrete: generated code
relies on psx_check_interrupts_at at basic-block boundaries to resume the
deferred CD coroutine and deliver BIOS WaitEvent/TestEvent/DeliverEvent.
For streaming/seek patterns the coroutine's resume never runs -> the
completion event is never consumed -> entries never finalize.

## Fix location
psxrecomp runtime — interrupts.c deferred-coroutine resume /
psx_check_interrupts_at coverage, cdrom.c IRQ->event delivery,
bios event/kobject services (bios_hle.c / psx_bios_backend.c /
event_ring.c). NOT game/*.c, NOT generated/ (read-only).

Agent 6f7d7f6e investigating the precise starving function/flag.

## Precise failing operation: Pause(0x09) completion event

pending_cmd=0x09 = CD Pause. cd_read_open/cd_init_session issue
Pause(9) then wait for its completion event (BIOS WaitEvent/TestEvent)
before SetLoc/Setmode/ReadN. The Pause EXECUTES at device level (seen in
cdrom_command_history) and raises CD IRQ bit2, but the deferred coroutine
that runs DeliverEvent on pause-done is starved -> the game's TestEvent
poll spins -> read sequence stalls -> channel entry never state==2 /
battle-init file load wedges. Attract demo avoids it (resident data, no
streaming Pause+read cycle).
