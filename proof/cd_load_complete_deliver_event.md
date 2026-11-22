# CD load-complete → DeliverEvent(0xF0000009,0x20) — drop-point analysis

Status: **drop point localized; root cause still open.** This document records
hard runtime + disassembly evidence gathered with a new env-gated DMA probe
(`PSX_DMAPROBE=1`, `dmaprobe()` in `psxrecomp/runtime/src/dma.c`) on a fresh
repro (port 8941) that reached the identical wedge signature as ports
8915/8930/8920/8934 (last sector LBA 8801, `sector_read_pos=2060/2340`,
`pending.cmd=0x09` Pause idle, `reading=0`).

## Proven facts (this session)

1. **The file-load data path is DMA channel 3, never channel 4.**
   `dmaprobe` logged every ch3/ch4 kick+completion for the whole run:
   ~2100 ch3 kick/done pairs (pcs `0x801978AC`, `0x80199694`, `0x80199CEC`,
   `0xBFC1E88C` BIOS, `0x80192FA4`), **zero ch4 events of any kind**.
   Therefore `cd_dma_chan_reset` (`0x8019B7E0`, the ch4 done-handler that
   calls `DeliverEvent(0xF0000009,0x20)` when `*0x801BDEE8==0`) is **not**
   the producer for this load. The earlier "ch4 completion chain works in
   versus" observation is a different (audio/SPU) path.

2. **ch3 completions intentionally raise no DMA IRQ.** Live DICR stayed
   `0x00900000` = bit23 (master) + bit20 (ch4 per-channel enable) only.
   ch3's enable bit (bit19) is clear, so `channel_irq_enabled(3)==false`
   and `complete_transfer(3)` never latches DICR bit27 — faithful to HW.
   The ch3 handler-table entry (`0x801BDE00`) is also 0. ch3 completion is
   consumed by the **CD-ROM IRQ2 path**, not by `cd_cb7_run`'s table.

3. **`cd_cb7_run` does run.** Its generated body (`br2eu_func_8019AF7C`,
   spans `0x8019AF7C–0x8019B0FC`) performs the DICR ack write at guest pc
   `0x8019B008` — observed 1101× in the probe, last at frame 1551.

4. **All DMA went silent at frame ~1551 and never resumed** (frame counter
   kept advancing past 25000; game logic wedged). Last ch3 done at f=1547;
   final DICR writes f=1549–1551 from `0x80192FA4`/`0x8019B008`/`0x80193210`.
   After that: no kicks, no completions, no DICR writes — CD subsystem idle
   while the guest spins.

5. **IRQ2 completion dispatch is `cd_cb8_dispatch` (`0x8019AE68`, 108 B,
   delegated to generated).** Decoded: it bumps a counter at `0x801BDDE0`,
   then loops `i=0..7` calling `tbl[i]` where `tbl = 0x801BDDC0`
   (clean twin: `cd_cb8_run_param`, game/cd.c ~line 5346). The table is
   zeroed by `cd_init_stage_a` (`0x8019AE10`) and slots are registered
   indirectly through unnamed generated fn `0x8019AED4`
   (`tbl[a0]=a1` if different).

6. **At the wedge all 8 callback slots `0x801BDDC0..0x801BDDDC` are 0**
   (verified `mem_words` on wedged 8915: entire region zero except
   `0x801BDDDC=0x801A0110`, `0x801BDDE0=0x0011F2EF`, `0x801BDDE4=0x1F801114`,
   `0x801BDDF0=0x1F8010F4`). So `cd_cb8_dispatch` fires on CD IRQ2 but
   calls **nothing** — the per-command completion callbacks are absent.

7. **`DeliverEvent(0xF0000009,0x20)` has exactly one guest producer in the
   clean tree**: `cd_dma_chan_reset` via `cd_bios_svc_07` (ch4 path), plus
   generated `br2eu_func_8019B7E0` doing `jr 0xB0` svc7. Other svc7
   trampolines deliver different classes (`cd_bios_cmd20` →
   `0xF0000003/0x20`, `cd_bios_cmd40*` → `0xF0000003/0x40`).

8. **XA audio is not being delivered**: `cdrom_sector_history` shows
   `xa_audio_delivered:0` on every sampled Mode-2 XA sector (raw_mode 0x02,
   xa_submode 0x08/0x89). Consistent with zero ch4 (SPU) DMA all run.

## Drop-point hypothesis (needs one more probe)

The missing event is upstream of `DeliverEvent`: the game's CD command
layer registers per-command completion callbacks into `tbl[0..7]`
(`0x801BDDC0`) via `0x8019AED4`. At the wedge the table is empty, so IRQ2
completion dispatches to nothing. Two candidate break points:

- (a) The final `Pause(0x09)`/read-tail completion path never re-registers
  (or prematurely clears) the callback slot that would run the
  "transfer complete → arm/DeliverEvent 0xF0000009" step; or
- (b) The deferred coroutine (`cd_ctx_restore → cd_chan_full_init →
  cd_intc_dispatch → cd_cb8_dispatch`) does not resume for the final
  IRQ2, so `tbl[]` is never even consulted (consistent with prior
  `cd_ctx_restore` zero-invocation observations).

The dmaprobe data cannot distinguish (a)/(b) because it only covers DMA.
Next instrumentation: env-gated `fprintf` at `cd_cb8_dispatch` entry
(generated — needs a runtime hook or pc_probe on `0x8019AE68`),
`0x8019AED4` registration calls, and `cd_ctx_restore` (`0x8019ADCC`)
entries, correlated by `s_frame_count`.

## Repro recipe

```
PSX_DMAPROBE=1 ./build/BloodyRoar2_Recompiled \
  --disc ../bloody-roar-2/Game.cue --debug-port 8941 --headless \
  > /tmp/br2_8941.log 2>&1 &
python3 tools/arcade_fight_mash.py 8941 > /tmp/mash_8941.log 2>&1 &
```

Wedge reproduces deterministically (same LBA-8801 signature); probe tail
in `/tmp/br2_8941.log` ends at `[dmaprobe] f=1551`.

---

## Session-2 update — blocked call-site + completion-flag pinpointed

Fresh instance (port 8950, `PSX_DMAPROBE=1 PSX_CB7=1 PSX_CDPROBE=1`,
`fn_filter lo=0x80140000 hi=0x801A1000` armed at boot + `fn_clear`,
`arcade_fight_mash.py 8950`). Game wedged at `gs=8 ph=3` (battle-init),
frame ~88000+, still advancing.

### The blocked thread is inside `cd_file_read_seq`'s seek-complete poll

Live guest stack of the spinning thread (TCB 0x8FE0, sp≈0x801FFDF8):

- `0x801FFE1C = 0x80156628` → return into `cd_file_read_seq`
  (`0x801565EC`+0x3C = the `jal 0x8019D500` loop-back edge, block
  `0x80156628`).
- `0x801FFE18 = 0x801926AC` → `cd_cmd_queue_load`+0x78 (`cd_timer_read`
  call site inside the poll's inner wait `0x8019D5B0`).

`cd_file_read_seq` (generated `br2eu_func_801565EC`, 100 B) is:
`mode_store(0)` → `status_shifted_read` → `seek_param_issue_clamped`
(`0x8019D410`→`0x8019BBC4`→`cd_reg_window_op` ops 2/1/3 since
`0x801BDECC==0`) → **loop `cd_seek_complete_poll` (`0x8019D500`) until
nonzero**.

### The exact wait condition

`cd_seek_complete_poll` loops until `*(u32*)0x801BDE40 == 1` (early-out)
or the `0x8019D5B0` event wait returns. Live wedge values:

- `0x801BDE38 = 0xF1000008` (the F0000009 event handle — armed, waiting)
- `0x801BDE40 = 0`  ← the flag the completion path must set
- `0x801BDF84 = 0`
- `0x801BDEE8 = 0`  (so `cd_dma_chan_reset` would take the
  `DeliverEvent(0xF0000009,0x20)` branch if reached)

### Completion inputs all idle/empty

- CD command queue empty (`0x801B8448==0x801B844C==0xD`, head==tail).
- `chan_job_pump` runs every IRQ2 but job counter `0x801BE768=0xFFFFFFFF`
  (no pending job); job slots hold only stale fns `0x8019F474/0x8019E798`.
- IRQ2 cb8 table `0x801BDDC0[0..7]` = all 0 except slot7 `chan_job_pump`.
- INTC handler table `0x801BCD1C` populated: bit0→`cb8_dispatch`,
  bit2→`cd_response_pump`, bit3→`cb7_run`.
- CD device idle: last command `0x09`(Pause) at cyc≈1.13G; no new
  seek/read commands since → the load never reached CD-command issue;
  it is stuck at the **channel/seek-complete** stage.

### Interpretation

The dropped link is the **per-command completion callback registration**
into `tbl[0..7]` (`0x801BDDC0`) via registrar `0x8019AED4`. With all
slots empty, the IRQ2 `cd_cb8_dispatch`/`cd_cb8_run_param` calls nothing,
so the in-flight command's "transfer/seek complete" step never runs →
`0x801BDE40` never set → `DeliverEvent(F0000009)` never fires →
`cd_file_read_seq` polls forever. This matches hypothesis (a) in the
previous section and is consistent with the loader being parked *before*
any new CD device command is issued.

### Next instrumentation needed

Arm `fn_entry`/`fntrace` on `0x8019AED4` (registrar), `0x8019AE70`
(`cd_cb8_run_param`), `0x80196370` (`cd_response_process`), and the
command-issue path on a **fresh** run, filtered to `0x80150000-0x80190000`
(excludes the per-frame IRQ2 spam `0x8019A460/0x8019A5D8/0x8019A87C/
0x8019A8EC/0x8019AE68/0x80192634/0x80192F14/0x801A0110` so the ring
retains the whole run). Determine which command's done-callback slot
should be non-empty and which registration call is missing.

## Session-3 update — spin localized to TestEvent poll; completion ran but DeliverEvent dropped

Parent live-trace on wedged instance (port 8941) + B0 table decode:

- `cd_seek_state_machine(a0)` = `bios_svc(0xb0,0x0b,a0,0)` = **B0 svc 0x0B =
  TestEvent** (B0_Table index 11; DeliverEvent=0x07, OpenEvent=0x08,
  WaitEvent=0x0A, TestEvent=0x0B, EnableEvent=0x0C). Two defs exist:
  cd.c:3618 `bios_svc(0xb0,0x0b,a0,0)` and bind.c:2402 `bind_bios_call_live(0x0b,ret)`.
- `cd_seek_complete_poll` (0x8019d500, delegated) → `cd_seek_state_machine(@0x801bde38)`
  = **TestEvent(flag)** spins until nonzero. `@0x801bde38=0xF1000008`, `@0x801bde40=0`.
- So `cd_file_read_seq` busy-polls **TestEvent** waiting for the matching
  DeliverEvent. Command queue EMPTY (`0x801B8448==0x801B844C==0xD`), CD idle,
  last device cmd Pause(0x09). ALL commands completed — the final DeliverEvent
  step was dropped, not an in-flight command.
- cb8 table `0x801BDDC0`: only slot7=`chan_job_pump`(0x801A0110); slots0-6=0.
  chan_job_pump runs every IRQ2 (~37k, counter 0x801BDDE0=0x921F).
- Job system RAN to completion: `@0x801BE768`=-1; slots `0x801fef60`=[cd_job_submit5
  0x8019F474, gen 0x8019E798,0,0]; entries [0xA,..],[0xB,..].
- `cd_dma_chan_reset`(0x8019b7e8) would DeliverEvent(0xF0000009,0x20) when
  `0x801bdee8==0` (confirmed 0) — but it's a tbl4[4]/cb7-DMA handler and this
  load is all-ch3 (zero ch4), so it is NOT this load's producer.
- File-read completion path: `cd_read_step_cb`(0x801985c4)+`cd_read_finish_cb`
  (0x80198838) both delegated; finish fires user cb `0x801bc7b0`→DeliverEvent.
- `bios_hle` DeliverEvent anchor UNAVAILABLE on OPENBIOS → kernel calls LLE.

OPEN: which completion callback was SUPPOSED to DeliverEvent and why it didn't
run/finish. Clean-port coroutine-starvation suspects in chain: chan_job_pump,
op_chan_fn_invoke(save.c:8438), cd_job_slot_alloc, cd_cb8_run_param. cb8 slots
0-6 empty may be NORMAL (only chan_job_pump needed) — do NOT assume agent's
cb8-registration theory without proof.

## Session-4 — decisive fresh-run trace (build ecedbb2 + PSX_DMAPROBE, ports 8960/8971)

Live wedge reproduced deterministically on TWO fresh processes; CD freezes at
identical state: `seq=81861`, `last_sector lba=8801` (Mode2/XA), `sector_read_pos=2060`
of 2340, `ring_dropped=2`, `ring_partial=1824`, `bfrd_set=3931` vs `bfrd_clear=14`,
`pending cmd=0x09(Pause) active=0`, `i_stat=0x01`, `I_MASK=0xD`. Game state `gs=8 ph=3`
(battle-load phase); frame counter keeps advancing (loading screen spins) while the
CD/load pipeline is hard-frozen. `cdrom_state.seq` identical across runs => deterministic
logic stall, not a timing race.

### Answers to the four posed questions (fntrace ring, 93061 entries)

- (a) `cd_read_step_cb` `0x801985C4` ran **324x then STOPPED** (delta 0 over 4 s).
  `cd_read_finish_cb` `0x80198838` ran **0x** — BUT this is EXPECTED, not a drop:
  `cd_read_open` only registers it via `cd_cmd_dispatch3(0x80198838)` when
  `DAT_801bc7b4 & 1`; live `0x801bc7b4 = 0`, so the finish-cb was never armed for
  this read. Read counters: `0x801bcb94` (sectors-remaining) = **0** => the read is
  COMPLETE from the driver's view.
- (b) user cb `0x801bc7b0` region holds `0x80195B9C`; the INT2 completion callbacks
  are the fn-ptr triple `0x801bc848/0x801bc84c/0x801bc7b0`, fired by
  `cd_response_pump` (INTC handler[2] @0x801bcd1c, verified live in table).
- (c) `DeliverEvent` B0/7 total = **111**; the F0000009/F1000008 flag IS delivered —
  last at frame ~3797, driven by a **ch4 DMA doorbell** (`done ch4` -> cb7 tbl[4]
  `cd_dma_chan_reset` -> `DeliverEvent(0xF0000009,0x20)`). This REVISES the earlier
  "all-ch3, cd_dma_chan_reset not the producer" note: ch4 doorbells DID fire 4x
  (dmaprobe frames 1865/3496/3746/3793->3797).
- (d) Thread is parked in `cd_file_read_seq`->`cd_seek_complete_poll`, spinning
  **B0/11 TestEvent = 1,462,829 calls** on `0x801BDE38=0xF1000008`/`flag@0x801BDE40=0`.

### The precise dropped link

Each load ends with a **ch4 DMA "doorbell"** whose done-IRQ -> `cd_dma_chan_reset`
-> `DeliverEvent(F0000009)` wakes the TestEvent poll. dmaprobe shows **zero ch4
kick/done after frame 3797** while ch3 sector DMA streamed to LBA 8801 (frame ~3909).
Channel-command ring cursors frozen: prod `0x801fc6c8`=2, fin `0x801fc6cc`=0.
Job counter `0x801BE768`=-1 (no active job); job slots hold stale
`0x8019F474`/`0x8019E798`, entries state 0xA/0xB. => the final doorbell channel
command was never produced/executed, so no DeliverEvent, so the poll spins forever.

### Fix attempt + result

Delegated `cd_response_pump` (`0x801979f8`, the IRQ2 CDROM completion dispatcher)
to `br2_call_generated` — it is a clean `for(;;)+br2_bail_pending` frame sandwiched
between generated `cd_intc_dispatch` and generated `cd_response_process`/callback
`call_guest`s (deferred-coroutine contract). Regenerated `src/clean_table.c`
(1284 wrappers), rebuilt OK. **Fresh arcade DAT1=3 run reproduced the IDENTICAL
wedge** (same seq/LBA/sector_read_pos=2060/ring_dropped=2). => cd_response_pump is
NOT the dropped link (kept the delegation — it is faithful regardless).

### Narrowed suspect set (clean frames still on the IRQ2->job->DMA->doorbell path)

`cd_cb8_run_param` (0x8019ae70), `chan_job_pump` (0x801a0110, cb8 slot7),
`op_chan_fn_invoke` (0x801a0a8c), `cd_job_slot_alloc` (0x801a0a10),
`cd_chan_job_submit` (0x8019ea98 -> allocs job fn `0x8019eb00`),
`cd_chan_job_pump` (0x8019f80c), `cd_pending_args_pump` (0x801a0e90),
`cd_svc_dual` (0x801a0620), `cd_status_map` (0x801a00bc), `cd_job_result`,
`cd_mc_op_wait`. The doorbell command producer is the job SM; prod cursor frozen
means no new channel command was submitted after read completion. Next step:
determine which submit path (read-completion cb @0x801bc848/4c -> channel cmd ->
job alloc) fails to enqueue the ch4 doorbell, then delegate that clean frame.
`ring_dropped=2` + `sector_read_pos=2060` may be symptom (consumer stopped) not cause.

## Session-5 update — DOORBELL HYPOTHESIS REFUTED; wedge is battle-FSM wait state

Agent fresh-run (port 8976, seq 81861) overturns the doorbell theory:

- `cd_file_read_seq` (0x801565EC) ran exactly 3× (frames ~3496/3697/3744). EACH
  completed the full ch4 doorbell -> cb7 slot4 -> cd_dma_chan_reset(0x8019B7E0)
  -> DeliverEvent(F0000009,0x20) -> TestEvent(0xF1000008)=1. **Event path WORKS.**
- After 3rd read_seq returned (~f3748), battle FSM parked; NOTHING in armed CD
  path ran for ~70k frames. cd_cb7_run last f3864, cd_seek_complete_poll last f3748.
- TestEvent B0/0x0B count FROZEN at 1,462,829 — **NO TestEvent spin** (earlier
  spin observation was a different/stale instance). Thread is in a game-level
  cooperative wait, not a BIOS block.
- Callers: br2eu_func_80126640(ret 0x80126664)/0x8012657C -> match_mode_set
  (0x801560F8) + battle_flow_state_machine (0x8016FCC4) = battle FSM file-load
  phase. After 3rd read_seq the FSM parked, never issued another read.
- CD device idle (pending.cmd=0x09 but active:0 = stale display). cmd-ring prod
  0x801FC6C8=2 / fin 0x801FC6CC=0; job ctr -1; sectors-remaining 0; flag7b4=0.
- Only ONE active TCB (0x8FE0); frames+VBlank IRQ advance normally.

CONCLUSION: CD/DeliverEvent/doorbell all WORK. The wedge is UPSTREAM — the
battle FSM (match_mode_set / battle_flow_state_machine, both CLEAN-PORTED in
game.c/menu.c) parked in a wait state that stopped driving cd_file_read_seq.
Load incomplete (280B + subsequent) but FSM isn't issuing reads = deadlock
between FSM wait-condition and loader pump. NOT a CD hardware/IRQ bug.

cd_dma_chan_reset faithfulness fix committed: delay_3c gate is mem32[0x801BDF00]
==0 (generated 0x8019B7E4 lw + bnez), NOT param_1 (never read by guest body).

## Session-6 update — CD EXONERATED; wedge is battle_frame case3 (DAT1=3 spawn phase)

Live wedged instance (port 8976, frame ~302k) dissected with pc_probe armed on
every delegated/generated CD entry PC. Result: the game is NOT in any CD path.

### pc_probe histogram (12 delegated CD/generated entry PCs, ~8 s window)

- Per frame the ONLY guest-visible CD calls are: `cd_cmd_queue_load`
  (0x80192634) ×1, `cd_timer_read` (0x8019a460) ×~4, `vsync_wait_n`
  (0x8019a5d8) ×2 — all delegated, all faithful.
- ZERO hits: `cd_file_load_sm` (0x8018cfdc), `cd_file_read_seq` (0x801565ec),
  `cd_cmd_issue` (0x80196e14), `cd_result_wait_a` (0x801968cc),
  `cd_reg_window_op` (0x8019b944), `chan_job_pump` (0x801a0110),
  `op_chan_fn_invoke` (0x801a0a8c), `cd_response_pump` (0x801979f8),
  `cd_cb8_run_param` (0x8019ae70). The earlier stack sightings of
  `cd_file_read_seq+0x3c`/`cd_cmd_issue+0xc8`/`cd_result_wait_a+0x88` on
  0x801ffdf8.. were STALE residues — current sp 0x801FFDF8 sits in the
  BIOS/kernel idle loop (ra 0x8019A53C, epc inside vsync_wait_n).

### Which function is looping — PROVEN battle_frame

- `DAT_1f800001 = 3` (spawn/select phase).
- `0x801f10bc = 0x801FFE58` = battle_frame's ctx slot (b36_sp+0x18).
- b36_sp = 0x801FFE40: prologue slots verified live —
  `*(0x801FFE40+0x130)=0x1f8000e8`, `*(+0x134)=0x1f8001f0`
  (battle_frame stores exactly those at B36_32(0x130/0x134)).
- task pools allocated: `0x801f10c0=0x801D660C`, `0x801f10c4=0x801D71FC`.
- `battle_flow_state_machine` tag70 decomp_trace FROZEN (seq 2533) → the
  flow SM is blocked inside the nested battle_frame for(;;) call, as designed.

### Live record state (T = b36_sp+0x40 = 0x801FFE80, stride 0x30)

- P1 rec (T+0):   f8=0x1b, f1c=0, f10=0, f24=1
- P2 rec (T+0x30): f8=0,   f1c=1, f10=1, f24=0xb

`fighter_spawn_setup` early path needs `rec+0x10==1 &&
(bp[0xb4/2+s1]&0xf00)==0x700` to arm the spawn task pair
(task_alloc_wrap_c → 0x801b2ed8/0x801b2ef4 descriptors) and post
`deferred_cmd_post(0x400,0x41)`. P2 has f10=1 but f24=0xb (not the
spawn-complete marker); the phase word gate has not fired, so DAT1 stays 3
and battle_frame case3 → L_tail → frame_tick loops forever.

### CD-side state (all idle/normal, NOT the bug)

- command queue BE7C0(head)=2 > BE7D4(tail)=1 → drained/empty.
- job counter 0x801BE768=-1 = all 4 job slots free (chan_job_pump correctly
  idles; job_counter_sign()==1).
- ring cursors 0x801FC6C8=2 / 0x801FC6CC=0 — produced!=finished is the
  NORMAL steady state when no channel command is in flight.
- DAT_801bdde0 vsync counter advancing (0x49B34→0x49C7B); frames advancing;
  B0/0x0B TestEvent frozen at 1,462,829 (no CD poll — correct, nothing to poll).
- cdrom_command_history: last device command frame ~3860 (cmd 0x09 Pause);
  the 3 completed read_seq batches were the bringup loads — bringup_a is
  DONE (+0x8c halfword = 0xFFFF = -1, verified live).

### Revised conclusion

The "cd_file_load_sm stalls after 3 read_seq batches" premise is DISPROVEN
for this wedge: the bringup loads completed; no loader is running or needed.
The wedge is the battle_frame DAT1=3 spawn/select phase never advancing —
either (a) the P2 spawn task/phase-0x700 gate isn't being satisfied
(task_pools_tick → task fn mistranslation or missing fighter data), or
(b) the pad-edge/confirm path (the previously-documented arcade
controller-2 0x840 edge requirement, commit 078147d) isn't being fed in
this headless run.

### Next step

Diff the P2 record fields + phase words (0x1f8000b4/0xb6, rec+0x10/+0x24)
against a WORKING versus/arcade instance at the same DAT1=3 frame, and
fntrace `task_pools_tick`'s task callbacks (0x801b2ed8/0x801b2ef4 descriptor
targets, task_node fn ptrs) to see whether the spawn task runs and where it
stops. If the task fn is clean-ported and mistranslated → fix C; if it needs
the deferred-coroutine → delegate.

## Session-7 update — spawn-phase wedge dissected; pad-2 injection seeds CPU phase

Live wedged instance (port 8976) re-dissected with correct u16 reads:

- DAT_1f800001=3 (battle_frame case3 spawn/select). mode 9b=0 (arcade), 9c=0.
- GLOBAL phase +0x92 = 0x0100 (u16 — earlier byte-read of low byte misreported 0).
- Per-player phases +0xb4=0x0300 (P0), +0xb6=0x0000 (P1/CPU).
- Pad edge words +0x68/+0x6a = 0 (transient; only nonzero on press frame).
- Spawn records (T=0x801FFE80, stride 0x30): rec0(P0)+0x10=0, rec1(P1)+0x10=1.
- Argblocks 0x1f8000e8/0x1f8001f0: AB[2]=0/1 -> P0 slot0, P1 slot1(CPU).
- Pad buffers 0x801befa8/0x801befca BOTH report `00 41 ff ff` = present
  digital pad (type 0x40), all buttons released -> phantom controller-2.

### Phase machine map (disasm-verified)

- battle_config_eval (0x80153544, menu.c:4007) dispatches on phase class
  (+0xb4/+0xb6 & 0xf00). cls 0x000 -> L_A0 -> bcfg_step_a needs (edge&0x800)
  START edge in the player's pad-edge word (+0x68 for slot0, +0x6a for slot1).
  cls 0x100->0x200 auto; 0x300/0x400 -> announce only (NO self-advance).
- mc_pad_button_handler (save.c:1732) zeroes +0xb4..+0xb7 each call, re-seeds
  via mc_slot_action on +0x68/+0x6a & 0x840 edges. Mode 0 (arcade) seeds ONLY
  the pressed slot; mode 2 (versus) seeds BOTH.
- mc_slot_action case 0 (arcade): +0xb4+slot*2 = 0x100 for pressed slot only.
- confirm_bookkeeping (menu.c:3940) is the SOLE writer of global +0x92
  (picks 0x100/0x200/0x300/0x400 from select_phase_gate of both players).
- post_battle_dispatch (0x80154930) advances per-player phases 0x300->0x500
  ->0x600->0x700 via +0x92 jump table — but is ONLY called from outer
  battle_flow_state_machine case8 (VERSUS path), NOT battle_frame.

### EXPERIMENT — controller-2 injection seeds CPU phase (PROOF)

Injecting `press slot=1 buttons=0x0800` on the wedged instance:
  +0xb6 (P1/CPU phase): 0x0000 -> 0x0300  (matches P0's 0x0300)
So the CPU confirm DOES flow through the pad-2 edge path — the mechanism is
real, not a missing game path. On real hardware the CPU's pad-2-equivalent
input must be generated by the game's CPU/AI routine writing the pad-2 buffer
(0x801befca decode source) OR an auto-confirm the harness isn't reproducing.

### Remaining wedge

Both players now at phase 0x300, global 0x100 — STABLE STUCK. Neither
battle_config_eval (no 0x300 self-advance) nor repeated START presses advance
them. The 0x300->0x500->0x600->0x700 advance in the ARCADE/battle_frame path
is NOT done by battle_config_eval — it must come from a task callback
(task_pools_tick) or a function battle_frame calls that versus reaches via
post_battle_dispatch. Next: identify the per-frame phase driver inside
battle_frame (task callbacks 0x801b2ed8/0x801b2ef4, fighter intro/ready
tasks) and why it doesn't advance arcade phases.
