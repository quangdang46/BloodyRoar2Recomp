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

## Correction/nuance — DeliverEvent DOES fire (~per command)

bioscall_dump on wedged :6900:
  B0/11 TestEvent = 5,138,859  (hot poll — likely normal per-frame event wait)
  B0/07 DeliverEvent = 2,494   (~once per CD command; 2614 cmds total)
  B0/08 OpenEvent=19, B0/09 CloseEvent=5, B0/12 EnableEvent=19

So CD IRQ -> guest handler -> DeliverEvent DOES run (~per command). The
starvation is NOT "no DeliverEvent." It is narrower: either the SPECIFIC
event flag the CD-wait thread polls is not the one being Delivered, OR the
delivered event wakes a thread that then fails to advance
cd_mc_state_machine / mark channel entries state==2. TestEvent spin alone
is not proof — need to identify WHICH event spec the wedged wait polls vs
which spec DeliverEvent targets.

## Probe refinement — state machine RUNS but does not finalize

pc_probe_arm on wedged :6900 (~10s window):
  0x80199440 cd_mc_state_machine : 196 hits  (RUNS — not kick-starved)
  0x80199360 cd_channel_poll     : 0 hits    (never invoked)
  0x80198BC4 bc84c resp callback : 4 hits
  0x80198E2C cd_read_poll        : 349 hits, v0=2 every sample (busy)

So cd_mc_state_machine executes ~20/s yet entries never reach state==2 and
cd_channel_poll is never reached. The finalize predicate (+4 received vs
+6 expected, or the mode/state gate inside the generated body) is not
satisfied for the streaming/battle-init entries -> the machine spins
producing/issuing without completing. This is narrower than "coroutine
starved": the machine RUNS but its per-entry completion condition fails.

## Mechanism map (clean+generated, verified in source)

Deferred-coroutine chain that must mark channel entries state==2:
  CD IRQ (INT1, bit2) -> cd_ctx_restore(0x8019adcc) -> re-enter
  cd_chan_full_init(0x8019a814) -> cd_intc_dispatch(0x8019a8ec)
  -> cd_response_pump(0x801979f8, clean cd.c:4540)
  -> cd_response_process(0x80196370, GENERATED 1372-byte response SM)
  -> on flags&4 call DB32G(0x801bc84c)=cd_read_step_cb;
     on flags&2 call DB32G(0x801bc848)=cd_read_finish_cb
  -> those drive cd_mc_state_machine(0x80199440, GENERATED)
  -> entry.state -> 2 -> cd_channel_poll(0x80199360, CLEAN cd.c:1663)
     returns ready.

cd_channel_poll semantics (clean cd.c:1663-1683, disasm-verified):
  entry = base@0x801fc6c0 + consume@0x801fc6ac *0x20 ; state=u16@+0.
  state==1 -> consume=0, maybe clear, recheck entry[0].
  state==2 -> state=4, *p1=base+fc6c4*0x20+idx*0x7e0 (data), *p2=entry, ret 0.
  else -> ret 1 (keep polling).

Consumer chain (FMV/memcard block IO):
  scene_entry(0x80159028) -> blk_io_retry(0x80159a14, clean save.c:2228)
  -> mc_date_serial_cmp(0x80159ab4, DELEGATED save.c:2270) whose generated
     body loops cd_channel_poll with per-block psx_check_interrupts_at —
     the yields that let the CD coroutine mark state==2.
  Delegation comment (save.c:2254-2269) documents this is REQUIRED: a
  clean-C poll cannot advance guest time / supply a resume PC, so the CD
  stream never delivers -> blk_io_retry livelocks.

NOTE: cd_channel_poll=0 hits on :6900/:7100 is EXPECTED when the instance
is past the FMV wedge into attract-demo streaming (which uses the
cd_read_poll/cd_file_load_sm path, not blk_io_retry->cd_channel_poll).
A 0-hit cd_channel_poll alone is NOT proof of the wedge — must probe
during the actual FMV/boot window or a player-combat battle-init load.

## Live wedge signature (fresh :7100, frame ~4800)

  cursors 0x801fc6a4/a8/ac = prod=2, fin=0, cons=30 (0x1E)  [FROZEN]
  entry30 @0x800103C0 state u16=0 (empty) -> poll would return 1 forever
  read-session 0x801bcb94 progress=0 (reads DO complete); bcb80 total and
    dest change each sample -> new sessions issue (pipeline not dead)
  MDEC output_pos ~10.83M/12.4M (~87%), decode_input_pos 3796/3840
    (input nearly exhausted), dma_in_words frozen, busy=0
  movie state 0x1f80008c low16=0xFFFF(-1); decode cursor 0x801c47b8
    stuck at 0x80010000 (never advanced)

Interpretation: CD reads complete and new sessions issue, but the
channel-entry finalize (state->2) never happens for the streaming
consumer, so blk_io_retry/mc_date_serial_cmp starve and the MDEC feed
stops -> FMV truncates ~87% then state machine exits to demo fight.

## DECISIVE: cd_ctx_restore NEVER fires (live :7200, FMV window)

pc_probe during active FMV wedge (frame ~11000, movie state 0x00C5FFFF):
  cd_ctx_restore   0x8019ADCC : 0 hits   <-- IRQ-resume DEAD
  cd_chan_full_init 0x8019A814 : 1 hit @3361 (ran once, set up coroutine)
  cd_intc_dispatch 0x8019A8EC : 2479 hits (runs via NON-restore/poll path)
  cd_response_process 0x80196370 : 1127 hits
  cd_mc_state_machine 0x80199440 : 196 hits
  cd_channel_poll  0x80199360 : 0 hits
  cd_response_pump 0x801979F8 : 0 hits

cd_chan_full_init (generated 0x8019A814, CPS) flow:
  setup (0x33333333 marker) -> cd_guest_zero32_a(0x41A)
  -> CPS jal cd_ctx_save(s0+0x38) -> yields (cpu->pc=0x8019AD90; return)
cd_ctx_save(0x8019AD90) dumps ra/s0-s7/fp/sp/gp into ctx frame.
  LIVE frame @0x801bcd50 VERIFIED VALID: +0 ra=0x8019A87C (the jal-link
  back into cd_chan_full_init continuation), +4 sp=0x801BDD30, etc.

So the context IS saved correctly, but on CD IRQ the kernel NEVER calls
cd_ctx_restore(0x8019ADCC) to reload it -> the deferred coroutine never
resumes -> whatever it was waiting to complete (channel-ready / response
drain that marks entries state==2) never happens.

BREAK LOCATION narrowed to the IRQ->registered-callback dispatch:
  cd_ctx_save/registration is via "B0/0x19 -> mem[0x4C18]" (BIOS/kernel).
  Live mem[0x4C18]=0x0000330C,0x00002B5C (NOT code ptrs -> either wrong
  slot interpretation or registration stores indices/flags, OR the B0/0x19
  arm never ran because cd_chan_full_init's post-save continuation never
  executed).

Two candidate root causes (runtime agent to discriminate):
  A) cd_chan_full_init yields at cd_ctx_save and its continuation that
     issues the B0/0x19 arm-call never runs (CPS resume-PC broken).
  B) B0/0x19 arm ran but the kernel IRQ->cd_ctx_restore dispatch is broken
     (INT1 handler doesn't route to the registered restore frame).

## REFINED: coroutine spins in cd_intc_dispatch poll; completion flag never set

br2eu_func_8019A87C (cd_chan_full_init continuation) decoded:
  block_8019A87C (entry):  if (gpr[2]==gpr[0]) -> block_8019A88C (READY)
                          else -> block_8019A884
  block_8019A884:          jal-link 0x8019A88C; CPS jal -> cd_intc_dispatch
                           (0x8019A8EC) -> loops back to re-check
  block_8019A88C (READY):  s0=0x801BCD54; calls 0x8019AD80, 0x8019AE10

Live probe: 0x8019A87C + 0x8019A884 = 2315 hits each; 0x8019A88C+ = 0.
=> the deferred coroutine is ALIVE and polling: each iteration calls
   cd_intc_dispatch to drain CD responses, then re-checks gpr[2]==gpr[0].
   The completion condition NEVER becomes true -> infinite poll -> wedge.
   cd_intc_dispatch runs ~2479x (matches the 2315 poll iterations + other
   callers) but never sets whatever flag gpr[2] tracks.

So the break is NOT "coroutine starved / IRQ not delivered" — the
coroutine RUNS and dispatches interrupts every iteration. The break is
that cd_intc_dispatch -> cd_response_process -> cd_mc_state_machine does
not produce the completion the coroutine waits for (the specific CD
response/event for THIS operation is never recognized/completed).

This points INTO the generated cd_response_process(0x80196370)/
cd_mc_state_machine(0x80199440) response-classification: they run but
do not match/complete the pending op's response -> flag stays unset.

## Channel-state region live (:7200)

cd_intc_dispatch reads channel globals ~0x801BDD44/48 (cd_chan_mode_set
region). Live dump: 0x801bdd40-0x801bdd7c ALL ZERO; 0x801bde44/48=0,
0x801bde4c=0x0000FFFE. Consistent with a CD init stage never completing —
the coroutine (cd_chan_full_init->0x8019A87C) waits for the prior stage's
done-signal that cd_response_process/cd_mc_state_machine never sets.
Ready path 0x8019A88C -> cd_init_stage_a(0x8019AE10) is the NEXT init
stage, confirming this is the CD subsystem init sequence stalling.

## Update — channel table is a 32-entry RING buffer (live :7400, frame ~3400-8700)

Pointer cells (verified live):
- `*(u32)0x801fc6c0` = `0x80010000` = entry-array base (scratchpad)
- `0x801fc6c4` = `0x20` = **capacity 32** (ring size, NOT stride)
- `0x801fc6c8` = producer cursor, `0x801fc6cc` = finalize cursor,
  `0x801fc6ac` = consume cursor
- Observed wedge cursors: prod=2, fin=0, **cons=30** — consumer sits near
  ring end (idx 30 of 32) while producer has wrapped to 2.

Entry dump (base + idx*0x20, 8 words): entry30 @0x800103c0 reads all-zero
(state=0) — the consume cursor points at a slot the producer already
wrapped past. Whether this is normal ring wraparound the consumer must
chase, or a genuine cursor desync, is the open question agent 40dd14de
is resolving with a live time-series.
