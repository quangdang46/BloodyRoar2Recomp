# CD wedge — DMA dispatcher / channel-submit / mode-setter RE + live wtrace capture

Combines the static RE of the DMA dispatch path (subagent report) with a
fresh always-on `wtrace` capture on port 8994 (arena `0x80010000-0x800103FF`,
SM control block `0x801FC680-0x801FC6FF` armed before boot).

## 1. DMA dispatcher `0x8019AF7C`

- `0x8019AF80`: `lw v0,-0x2210(v0)` → pointer cell `0x801BDDF0`; `0x8019AFA4`:
  `lw v0,(v0)` → `*(0x801BDDF0)=0x1F8010F4` = **DICR**.
- `0x8019AFAC-0x8019AFB4`: `srl v0,0x18; andi s1,v0,0x7F` → pending done mask
  = DICR bits 24..30 (per-channel IRQ flags).
- Handler table base `0x801BDDF4`, slot `s0` ↔ DICR bit `24+s0`; cursor `s4`
  walks `+4` per channel, `s0` 0..6.
- Per set bit: AND-write DICR ack (`v0=(1<<(s0+24))|0x00FFFFFF`), then
  `jalr *s4` at `0x8019B01C` (link `ra=0x8019B024`) if handler nonzero.
- `0x8019B034-0x8019B050`: re-reads DICR; loops while bits 24..30 nonzero.
- Tail `0x8019B058+`: if `(DICR&0xFF000000)==0x80000000 || (DICR&0x8000)` →
  debug print via `0x8018E550`.
- Handler `a0` at `jalr` is a side effect (`0x1F8010F4`), not an argument.

## 2. Channel submit `0x80199D88` (clean: `game/cd.c` `cd_chan_cmd_submit`)

- Channel index `a0`; regs at `0x1F801080 + a0*0x10` (MADR/BCR/CHCR=+8).
- Busy poll on CHCR bit `0x01000000`, cap `0x10000` iterations → printf
  `0x80125B18` on overflow.
- Flag byte `*(*(0x801BCC5C))+2` bit `1<<a0`; control RMW
  `1<<(a0*4+3)` OR'd into `*(*(0x801BCC58))`.
- Writes MADR=`a1`, BCR=`(a2<<16)|a3`, waits `*(*(0x801BCC40))&0x40`, writes
  CHCR=stack arg5, returns. Does NOT wait for completion.
- SM payload copies pass `a0=3` at `0x80199690/0x80199C4C/0x80199CE8` →
  **channel index 3** (CDROM DMA channel on real HW layout).
- SM defer-path busy test `*(*(0x801BCC60))&0x01000000` resolves to
  `0x1F801098` = **channel-1 CHCR** ("MDEC-out" is a doc-level label only).

## 3. `cd_mode_set_0e` `0x80198F60` (clean wrapper `game/cd.c:1496`)

- Sole found caller: `cd_stream_mode_init` (`0x80159CE4`) with `a0=0x1E0`
  (clean `game/menu.c:2363` `cd_stream_mode_init` loop).
- `(s0&0x100)!=0` → `0x801FC6F8 = (s0&0x20)==0`; installs
  `cd_dir_entry_finalize`(`0x801990F0`) via `0x801961A8` and
  `cd_mc_sm_wrap`(`0x80198FE4`) via `0x80195D78` into bc84c-family cell.
- Then `cd_mode_flag_set(0x1B,0,0)`.

## 4. `0x801BC84C` lifecycle (generated store/load PCs)

- Stores: `0x80195AC0`(gpr3), `0x80195D88`(gpr4), `0x80197478`(0),
  `0x801974F8`(0). Loads: `0x80195D7C`, `0x80196A54`, `0x80196CD4`,
  `0x80197130`, `0x80197A4C` (inside `cd_response_pump`).
- Park/restore protocol (`proof/cd_callback_cells_perop.md`): `read_setup`
  parks bc848/bc84c→bcba8/bcbac and zeroes live cells; `read_open` installs
  `read_step_cb`; `init_done_cb` restores. Zero bc84c at idle is normal.

## 5. Live wtrace capture — port 8994 (fresh boot → arcade wedge)

Ranges armed pre-boot: arena phys `0x00010000-0x00010400`,
`0x001FC680-0x001FC700`. Nav reached `gs=8 ph=03 b4=0300` and wedged.

### SM drive pattern (healthy, f3350–f3441)

- SM invoked via **`ra=0x80197A6C`** (inside `cd_response_pump` IRQ path —
  i.e. `bc84c=sm_wrap` firing on CD IRQs), ~once per 2–3 frames.
- Each pass walks 4 entry-groups `{0x02-0x05},{0x0B-0x0E},{0x11-0x14},{0x1A-0x1D}`
  (= decimal {2-5},{11-14},{17-20},{26-29}); per group `FC684` u16 sub-cursor
  steps 1→2→3→4 then resets (pc `0x80199B48`/`0x80199C60`), `FC6A4` group index
  ++ (pc `0x80199D2C`).
- Every SM entry arms `FC68C 0→1` at `0x8019949C` (defer path, ch1 busy).
- SM writes entry records itself: pc `0x80199694` writes entry fields
  (e.g. `0x80010160, 0x00050001, 0x00002710, 0x00000A98, 0x00F00140,
  0x38000720, 0x00020001` at f3441 into entry@0x80010340).
- `FC6A8` (finalize idx) last written **f3360** → `0x19`; never advanced again.

### The stop (f3441 → wedge)

- Last SM-driven write: f3441 `FC6A4 0x1C→0x1D`, `FC684 3→4`, `FC68C 0→1`.
- **Zero** further writes to FC684/FC6A4/FC68C/FC69C/FC6A8 for the rest of the
  run (verified: ring total 43723 < cap 262144, no eviction).
- CDROM-DMA (ch3) bursts of 256 words each at **f3464, f3625, f3639, f3689**
  wrote disc file data into `0x80010000-0x800103FF` (pc recorded
  `0x801978AC`, ra `0x801986EC`, dma_ch=3) — overwriting live entry slots.
- Current wedge state: `FC6A4=2, FC6A8=0, FC68C=0, FC684=2, FC6C0=0x80010000,
  FC6C4=0x20, FC6AC=0x1E`.

### Invisible-writer discovery (tooling gap)

`FC6A4` last *recorded* write = `0x1D` @f3441, yet reads `2` at the wedge;
`FC68C` last recorded `0→1` @f3441, yet reads `0`. The writes that produced the
current values **bypassed wtrace** — clean-C `game/cd.c` accesses these cells
through `*(uint32_t *)guest_at(0x801fc6xx)` host pointers (e.g. `cd_dir_reset`
`game/cd.c:1557-1571` zeroes FC6AC/A8/A4/69C/68C/684/680; `cd_dir_table_set`
`:1355-1361`; `cd_channel_poll` `:1658+`; `cd_dir_entry_finalize` `:3552+`).
Host-pointer stores are invisible to the write-trace ring — a real observability
gap for exactly this class of bug.

## 6. Interpretation

- SM was healthy while `bc84c=sm_wrap` was installed (mode-0x0E window via
  `cd_stream_mode_init(0x1E0)`) and CD IRQs flowed through `cd_response_pump`.
- At ~f3441 the session transitioned: `cd_dir_reset`/table re-init (clean-C,
  invisible) reset FC6A4/FC684/FC68C; ch3 DMA delivered the next file payload
  into the shared arena f3464–3689.
- The new pending work never got an SM driver: `bc84c` no longer `sm_wrap`,
  `fc68c` cleared by reset and never re-armed (SM is its only writer), and
  `tile_stream_pump` is not running in this phase (poke of `fc68c=1` on 8993
  was never consumed).
- Game spins `B0:0x0B TestEvent` (~1.46M calls) for the finalize-produced event
  → deterministic wedge at `gs=8 ph=03 b4=0300`, identical across instances.

## 7. Second capture — port 8997 (fntrace on callback primitives)

Armed: `mc_session_b`(0x80195D78 bc84c), `mc_session`(0x80195D64 bc848),
`cd_chan_reset_b`, `cd_cmd_dispatch3`, `cd_chan_cb_set`, `tile_stream_pump`,
`cd_mc_sm_wrap`, `cd_mc_state_machine`, `cd_response_pump`, `cd_mode_set_0e`,
`cd_stream_mode_init`.

### bc84c lifecycle (complete)

- **f1080**: `mc_session_b(0x80198FE4)` from ra `0x80198FC4` — **sm_wrap
  installed exactly once** by `cd_mode_set_0e(0x1E0)` (the bit8 path).
- f1080–f1148: SM runs via `resp_pump→bc84c` (CD IRQ cadence ~2–3 frames).
- **f1148**: last `resp_pump`, last `cd_mc_sm_wrap`, last `SM`, last
  `tile_stream_pump` (via DMA dispatcher `ra=0x8019B024`) — the entire CD IRQ
  callback chain goes silent.
- f1194–f1521: seven file-read sessions (Setloc/ReadN/Pause pairs). Each does
  `mc_session_b(0x801985C4)` [install read_step_cb] + clears; bc848 gets
  `0x80198590` at f1194. **No bc84c/bc848 writes after f1194** — sessions reuse
  the installed callbacks.
- **f1521**: last CD command (Pause 0x09); `mc_session_b(0)` last bc84c write.
- Wedge: bc848=bc84c=bcba8=bcbac=0 — sm_wrap **lost** (never restored; parked
  save also 0 because bc84c was already dead when first parked).

### Wedge steady-state (8997)

- Only BIOS activity: B0:0x17 CD-worker poll (~96/s). **B0:0x0B frozen at
  1,402,471 — NOT spinning** (earlier "TestEvent spin" was historical).
- `i_stat=0, pending=0, i_mask=0xD`; CD idle-paused at MSF [1,59,23] LBA 8801;
  `pending.cmd=0x09` already fired (process_pending clears correctly).
- All DMA channels idle, `dicr=0x00900000`; game fully blocked — no CD commands
  issued for thousands of frames.
- `clean_trace` spin = `fifo_space_check`(0x801A4230)←`dev_cmd_*`/`cd_cmd_*`
  queue layer — a **queued CD/device command that can never be sent** because
  its completion event never arrived.
- FC block frozen: `FC6A4=2, FC6A8=0, FC68C=0, FC684=2`; FEB10=0; arena holds
  the file directory (entries overwritten by file-load payloads).

### Revised causal model

1. Versus-spawn file loads (f1194–1521) share arena `0x80010000` with the
   channel-entry table — by design (AGENTS.md §8).
2. The last read session's completion path should deliver an event that
   unblocks the command-queue layer (`fifo_space_check` spin) AND/OR resumes
   the stream session (`cd_stream_mode_init`→`mode_set_0e`→reinstall sm_wrap).
3. That event never fires → game idle-waits; SM starves (no driver); the
   pending channel entries are the *symptom*, not the cause.
4. `resp_pump` dying at f1148 (before the read sessions) means the CD IRQ→
   callback chain broke first — the read sessions then ran on the
   read_step_cb path which does not need resp_pump.

### Observability gaps found (tooling bugs, rule 15)

- `wtrace` only sees generated-code + DMA writes; clean-C `guest_at()`/
  `DB32()` host-pointer stores are **invisible** (the FC-block reset and all
  bc84c writes via `mc_session*` evade it).
- `fntrace` only records **guest-dispatched** entries; clean→clean C calls
  (`tile_stream_open`→`cd_dir_table_set`, `cd_stream_mode_init`→
  `cd_mode_set_0e`) are invisible — `mode_set_0e`/`stream_mode_init`/
  `dir_table_set`/`dir_reset` showed zero entries despite necessarily running.

## 8. Open questions

- Exactly which clean-C call reset the FC block after f3441 (candidate:
  `cd_dir_reset` via `cd_dir_table_set` for the versus-spawn file session) and
  whether the *generated* path would have re-armed `bc84c=sm_wrap` there —
  i.e. is the missing re-arm a port divergence or faithful guest behavior
  gated on an event we drop?
- Why `resp_pump` stopped at f1148 — was the CD IRQ line still firing and the
  tbl8 dispatch broken, or did the peripheral legitimately go quiet while the
  read-step path polled?
- The `fifo_space_check` spin: which queue/device is it gating, and which
  completion event should release it (read-done callback? SM finalize?).
- The `0x801BCC58` pointer-table target for the cmd-submit control RMW.

## 9. Third capture — the wedge thread identified (port 8997)

The frozen `clean_trace` loop resolves to **`battle_flow_state_machine`**
(0x8016FCC4, invoked per-frame via dispatcher 0x80126640 which also calls
`match_mode_set` 0x801560F8). Inside it, the 0x80192xxx-0x801A4xxx
**device-command queue layer** runs every frame:

- `dev_byte_xfer` (0x801A1F74) fntrace: ~4 calls/frame, ra=0x801A1EC0
  (`dev_cmd_round_dispatch`+0x3C), alternating two contexts:
  - `a0=0x801FF080, a1=0xFFFFFFFE(-2)` and `a0=0x801FF080, a1=0`
  - `a0=0x801FF170, a1=0xFFFFFFFE(-2)` and `a0=0x801FF170, a1=0`
- Two device contexts at 0x801FF080 / 0x801FF170 (mostly-zero structs;
  +0x10 self-ptr, +0x24=1, +0x2C→0x801FF0A4/0x801FF194,
  +0x30→0x801BEFA8/0x801BEFCA = bitmask table in the DMA-handler region,
  +0x34=0x10400, +0x3C→0x801FEFF0/0x801FF013).
- Occupancy/timestamp cells: 0x801FF260 = live T2-derived timestamp
  (0x19A9→0x7BAD across probes), 0x801FF264 = 0x3C limit,
  0x801FE380 = 0.
- `fifo_space_check` (0x801A4230) reads T2_COUNT(0x1F801120),
  T2_COMP(0x1F801128), guest cell — T2 verified LIVE (ch2 counter
  advancing, mode 0x0400). "Space exhausted" is computed from live
  timestamps, not stale zeros — the queue genuinely never drains.
- psx-spx confirms our timer model is correct (T2 = 0x120 count /
  0x124 mode / 0x128 comp, sysclk, optional /8).
- The drain side (who consumes the two queues / posts completion) is the
  open question — delegated to runtime RE agent.

## 10. Correction — dev_byte_xfer is the SIO pad-poll path, NOT the wedge

Follow-up decode of `dev_byte_xfer` (0x801A1F74) instruction stream +
live probes overturn §9's hypothesis:

- `0x801BE7E0`/`0x801BE7E4` hold **0x1F801070 (I_STAT) / 0x1F801040
  (SIO_DATA)** — dev_byte_xfer is the **SIO byte-transfer state machine**
  (state byte <9 at ctx+0x44, flag test `I_STAT & 0x80` = IRQ_SIO0 bit7).
- It writes the pacing cells 0x801FF260/0x801FF264 itself
  (`sw 0x1FAE→0x801FF264`, `sw a0→0x801FF260`) and reads T2_COUNT for
  pacing — `fifo_space_check` "space exhausted" is normal flow control,
  retried next frame.
- Live `sio_trace` (1.9M entries): continuous healthy 5-byte pad polls
  `tx 01 42 00 00 00 → rx FF 41 5A FF FF`, dev state cycling 0→1,
  mc_state=0 (MC_IDLE) — **SIO/memcard fully healthy**, pads connected,
  mc_reads=111 vs mc_read_done=108 gap is historical, not live.
- The two "contexts" 0x801FF080/0x801FF170 are the two SIO channel
  endpoints (com1/com2 or pad/memcard split), polled ~4x/frame by
  `dev_cmd_round_dispatch` — this runs every frame in ALL phases,
  healthy or wedged. **Red herring — not the blocker.**

### What the wedge actually is (refined)

- Game is ~100% idle: only B0:0x17 CD-worker poll (~96/s), the per-frame
  `battle_flow_state_machine`→pad-poll path, and VBlank run. All game
  tasks asleep in kernel WaitEvent (B0:0x0B TestEvent frozen at
  1,402,471 — cumulative, not spinning).
- CD went silent after the last read session: Setloc[1,58,70]+ReadN
  (f1510) → Pause (f1521) → no further commands ever.
- SM last ran **f1171** on this instance — the IRQ-driven SM path
  (`resp_pump`→`bc84c`=sm_wrap) died BEFORE the file-read sessions
  (f1194+) even started; those sessions ran via the synchronous/poll
  read path (`cd_result_wait_*`), which does not need resp_pump.
- `bc848`/`bc84c`/`bcba8`/`bcbac` all 0 at wedge — session callbacks
  cleared by the last teardown.
- `cd_init_done_cb` (0x80198590, the bc848 session callback) is just a
  52-byte trampoline → `mc_session` (0x80195D64); `cd_read_step_cb`
  (0x801985C4) drives per-sector session state and re-installs/clears
  the callback cells.

### The missing event

The game waits (WaitEvent) for the completion signal of the last
file-load session. That signal is produced by the read-session
done-path — either a final `cd_read_step_cb` step gated on the last
sector's data-ready, or the session-teardown DeliverEvent. Both depend
on the CD IRQ→callback chain (`resp_pump`→`bc84c`) that went silent at
~f1148-1171. If the last sector's data-ready/completion IRQ found
`bc84c` already cleared (or resp_pump not re-armed for the session),
the done-path never runs → WaitEvent forever → deterministic wedge.

### Open (next session)

- Whether the last ReadN (f1510, seek [1,58,70]) delivered its full
  sector count before Pause (f1521) — compare `s_read_sectors`/
  delivered-sector count vs the game's expected count.
- Whether `resp_pump` is clean-ported (fntrace-invisible) and actually
  ran during f1194-1521 sessions — arm `call_focus`/`pc_probe` on its
  entry PC rather than fntrace.
- The session done-path: which function DeliverEvents on read
  completion, and which cell/flag gates it (fntrace can't see clean-C
  calls — need wtrace on the event-flag cell or bios_svc DeliverEvent
  args filtered by svc=0x07).
