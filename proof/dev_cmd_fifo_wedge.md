# Device-command layer "fifo_space_check" wedge — DECODED: healthy SIO pad-poll, NOT the blocker

Live instance: debug port **8997**, `gs=8`, `DAT_1f800001=3` (ph=3),
`b4=0x0300`, `b6=0x0000`, `+0x92=0x0100`, frame ~149k and advancing.

## Verdict (up front)

The frozen `clean_trace` loop through `0x801A1834 / 0x801A189C /
0x801A1B50 / 0x801A1F74 / 0x801A4230` is the game's **SIO device-driver
layer** — a record-based command processor bound to the **SIO controller
(0x1F801040) and I_STAT/I_MASK (0x1F801070/0x1F801074)**, i.e. the
**pad + memory-card bus**, NOT CD-ROM and NOT GPU. It runs **every frame**
as kernel-registered per-frame callbacks and is **healthy background
work** — it is NOT the wedge. `fifo_space_check` is a **T2-timer pacing /
timeout primitive**, not a FIFO-space check; "space exhausted" is normal
flow control.

The real wedge is upstream in the **versus-spawn `battle_init` state
machine** (guest `0x801506D0`): the SM phase is frozen at 3 even though
`frame_divisor_get()==0` (which would advance it), proving the game thread
is blocked inside `battle_init`'s bring-up chain (or asleep in a kernel
wait reached from it). All file loads and LZSS decompression completed
long ago; the CD is idle because the game requested nothing further.

## 1. What the addresses actually are (renames.map names are WRONG)

`game/renames.map` labels this region `cd_*`/`gpu_*` — those names are
guessed and incorrect. Verified by the MMIO addresses the bodies touch:

| Guest addr | renames.map name (WRONG) | Actual identity |
|---|---|---|
| 0x801A1834 | `op_flag_fn_check` | VBLANK-gated callback trampoline: returns 1 early unless `(I_MASK&1)&&(I_STAT&1)` (VBLANK pending+unmasked) and `*0x801BE7C8!=0`, else `jalr *0x801BE7C8`. |
| 0x801A189C | `cd_cmd_queue_service` | Device-command queue service pass over the 240-byte records. |
| 0x801A1B50 | `dev_cmd_seq` | Per-record SIO command sequencer (see §3). |
| 0x801A1F74 | `dev_byte_xfer` | Paced SIO byte transmit/receive state machine (see §4). |
| 0x801A4230 | `fifo_space_check` | T2-timer elapsed-time check (see §5). |
| 0x801A4210 | `fifo_space_sample` | Arms the T2 timestamp/limit pair. |
| 0x80192F14 | `cd_queue_push` | Upper-layer queue push that ends up invoking the device service (see §6). |
| 0x80193424 / 0x8019261C / 0x80192E00 / 0x80192E78 / 0x801936B0 / 0x8019A7FC | `gpu_*`/`cd_*` | Game-side queue/deferred-command machinery that *calls into* the device layer; also mis-named but not the focus. |

Evidence the device is SIO: `0x801BE7E0 = 0x1F801070` (I_STAT),
`0x801BE7E4 = 0x1F801040` (SIO_TX/RX_DATA). `dev+4 = 0x1F801044` =
SIO_STAT, `dev+8 = 0x1F801048` = SIO_MODE, `dev+10 = 0x1F80104A` =
SIO_CTRL, `dev+14 = 0x1F80104E` = SIO_BAUD. I_STAT bit7 = IRQ_SIO0
(`psxrecomp/runtime/src/sio.c:29 #define IRQ_SIO0 7`).

## 2. Live cell map (mem_words @0x801BE7A0, stable across probes)

```
0x801BE7A0 = 0x801A3E94   drain/state callback  (record state machine)
0x801BE7A4 = 0x801A39AC   ctx callback
0x801BE7B4 = 0x801FF080   record-table base (240-byte records)
0x801BE7B8 = 1
0x801BE7C0 = 2            device/slot index
0x801BE7D4 = 1
0x801BE7D8 = 0xFFFFFFFF   per-slot pending-count tbl[0] = -1 (idle marker)
0x801BE7DC = 0            tbl[1]
0x801BE7E0 = 0x1F801070   ctrl ptr -> I_STAT   (also read as tbl[2] — see note)
0x801BE7E4 = 0x1F801040   dev ptr  -> SIO regs
0x801BE7E8 = 1
0x801BE7FC = 0x1F801040   status ptr
0x801BE810.. = 0x801A30C0,0x801A3108,0x801A31E0,0x801A328C,0x801A3350
                          command fn-ptr table (flush/process family)
```

Note on `tbl[2]`: `dev_cmd_seq` computes `tbl = 0x801BE7D8 + slot*4` with
`slot = *0x801BE7C0 = 2`, landing on `0x801BE7E0` which holds the I_STAT
pointer `0x1F801070` (positive). The drain path is nonetheless healthy —
the drain callback `0x801A3E94` is the record state machine and the queue
is empty in practice (records are idle). This is a guest-side table layout
where the "slot" is a device index, not a port number; do not "fix" it.

## 3. dev_cmd_seq (0x801A1B50) decoded

Per 240-byte record (`rec`, base `0x801FF080`/`0x801FF170`):

1. `dev = *0x801BE7E4`; `sh 0x40→dev+10`, `sh 0→dev+10`, `sh 0x0D→dev+8`
   (SIO_MODE=0x0D), `sh 0x88→dev+14` (SIO_BAUD=0x88).
2. `fifo_space_sample(rec[0xE8]==8 ? 0x50 : 0x91)` — arm pacing.
3. `slot = *0x801BE7C0`; `dev+10 = (slot==0 ? 0x1003 : 0x3003)` —
   SIO_CTRL = ACK_IRQ_EN(0x1000) | [RX_IRQ_EN(0x2000)] | TX_EN|SELECT(0x3).
4. Drain pending records via `*0x801BE7A0` cb, then `*0x801BE7A4` cb.
5. Send phase (only if `SIO_STAT & 0x200`): set CTRL bit4, retry
   `fifo_space_check` (timeout), `dev->b0 = 1`, `sample(2000)`,
   `dev_cmd_commit()`, `dev_stat_bit1_wait()` (SIO_STAT.RX_RDY),
   `sample(0x1AE)`.
6. Two wait+send rounds gated on `*ctrl & 0x80` (**I_STAT bit7 = SIO IRQ**)
   with `fifo_space_check` timeout → `dev_cmd_commit`/`dev_stat_bit1_wait`;
   bytes sent: `1`, `'B'`(0x42), `1`. Any commit failure → return 0
   ("still busy, call again").

`dev_cmd_commit` (clean, `game/save.c:9959`): clears I_STAT bit7
(`*0x1F801070 = 0xFFFFFF7F`), waits `SIO_STAT & 0x80` clear with
`fifo_space_check` bailout, then sets CTRL bit4 (ACK).

## 4. dev_byte_xfer (0x801A1F74) decoded — generated body
`generated/SLES_017.22_full_49.c:22355` (528 B / 20 blocks; clean wrapper
delegates whole body via `br2_call_generated`, `game/save.c:9880`).

- `ctx = a0` (one of `0x801FF080` / `0x801FF170` — the two SIO channel
  endpoints), `a1` = byte (`-2` = query/flush path).
- Loads `dev = *0x801BE7E4` (SIO base), ctx buf ptr `+0x40`, counters
  `+0x44/+0x45`.
- **Wait loop A** (`0x801A1FE0`): busy-poll `lh(dev+4) & 1` =
  **SIO_STAT.TX_RDY (bit0)** until set.
- **Wait loop B** (`0x801A1FF4`→`0x801A1FFC`): call `fifo_space_check`
  until nonzero — this is the **baud-pacing delay** between bytes.
- Then `sb ~a1 → *dev` (write byte to SIO_TX_DATA `0x1F801040`), advance
  ctx counters, loop for the transfer length.
- The `a1=-2` calls seen in `clean_trace` are the per-frame query/flush
  passes on both channel endpoints.

This is a **software-paced byte pump**: it does not itself wait for the
card/pad to answer; it spaces TX bytes by the T2 timer and hardware
TX_RDY.

## 5. fifo_space_check (0x801A4230) / fifo_space_sample (0x801A4210)

NOT a FIFO. It is the game's **elapsed-time check** built on the free-running
Timer-2 counter:

- `fifo_space_sample(limit)` (`game/save.c:9896`, faithful clean port):
  `0x801FF260 = u16@0x1F801120` (T2_COUNT snapshot = "base"),
  `0x801FF264 = limit`.
- `fifo_space_check()` (`game/save.c:9924`):
  `a0 = u16@0x1F801120`; if `a0 < base` then `a0 += u16@0x1F801128`
  (T2 target; `+0x10000` when target==0); `v0 = a0 - base`;
  if `!(u16@0x1F801124 & 0x200)` (T2 mode bit9 = CLK_SRC1, i.e. /8 clock)
  then `v0 >>= 3`; **return `v0 >= limit`** (1 = "time expired").

Live values confirm it is re-armed constantly:
`0x801FF260` observed `0x86D5 → 0x97B1 → 0xADAF → 0x83B2` across probes
(fresh T2 snapshot each pass), `0x801FF264` alternates `0x1AE`/`0x3C`
matching `sample(0x1AE)`/`sample(60)` in `dev_cmd_seq`. T2 counter is
live-advancing (timers_state ch2: 18703→22287→22759). "Space exhausted"
returning is the **normal** "timeout elapsed, proceed" signal — it is
retried next frame by design.

## 6. Who calls it / how often — proof it is background

`clean_trace` tail (frozen-identical over seconds) shows the per-frame
cycle:

```
0x80193424/0x8019261C/0x80192E00   ra=0x80126664   (game queue machinery)
0x801936B0 / 0x8019A7FC / 0x80192E78 / 0x80193020(=cd_queue_push+0x10c)
0x801A1834 ra=0x0000293C           VBLANK-gated cb trampoline
0x801A189C ra=0x00002954           queue service
0x801A1B50 ra=0x00002954           dev_cmd_seq
0x801A1E84 → 0x801A30C0 → dev_byte_xfer → 0x801A4230 ×~20 (pacing)
0x801A3108/0x801A31E0/0x801A328C/0x801A3350(dev_cmd_process)/0x801A3E94…
```

The `ra=0x000029xx` callers are **low-memory kernel trampolines** — these
functions were registered as per-frame/IRQ callbacks with the BIOS kernel
and run from the VBLANK/idle path **independent of the game thread**.

Rate evidence (decisive): `sio_state.tx_writes` grew
`2989054 → 2991994 → 2995614` (~1.5–1.8 k writes/s) while `frame` grew
`148923 → 149070 → 149251` (~75–90 fps) → **~20 TX bytes/frame** — exactly
a per-frame pad/device poll cadence, not a tight wedge spin. `sio_stat`
returns to `0x0005` (TX_RDY|TX_EMPTY = idle) between transactions,
`sio_ctrl=0`, `card_txn_dump open:false`, `mc_cmds` frozen at 114
(all card reads finished at guest cycle ~1.5e8, i.e. boot — current cycle
~1.08e11). `mc_status`: two `.mcd` cards present and healthy.

`bioscall_dump` delta over 4 s: **only B0 index 23 (~92/s)** — the
`cd_intc_dispatch` INTC-enable tail. B0:0x0B (TestEvent) count is
**frozen** (not spinning); no thread is polling kernel events.

## 7. The actual wedge (upstream, game-side)

Scratchpad (`read_ram 0x1f800000`):

```
0x1f800000 gs=0x08   0x1f800001 ph=0x03   0x1f800002 sub=0x00
0x1f800019 frame_divisor byte = 0x00   (frame_divisor_get()==0)
0x1f80008c = 0xFFFF   battle_bringup_a progress = DONE (L_tail48 stored -1)
0x1f800090 = 0x01000000 → u16@0x92 = 0x0100 (one side confirmed)
0x1f8000b4 = 0x0300 (P0 phase class, terminal in battle_config_eval)
0x1f8000b6 = 0x0000 (P1 phase class, never initialized)
```

`battle_flow_state_machine` (`game/menu.c:551`) case `DAT_1f800001==3`
→ `caseD_3` (`menu.c:682`): `frame_divisor_get()==0` ⇒ calls
`battle_init(0x1f800000,…)` (`game/menu.c:5379`, guest `0x801506D0`)
**before** storing phase 4. Phase is frozen at 3 with divisor==0, so the
SM is not completing an iteration → the game thread is **blocked inside
`battle_init`'s bring-up chain** (or asleep in a kernel wait reached from
it). `battle_bringup_a` (`menu.c:5699`, guest `0x80148170`) is a
file-load state machine (`s5+0x8c`: 0→8→0x10→0x20→0x30→0xffff) that calls
`cd_file_load_sm` per asset; its progress cell `0x1f80008c=0xffff` shows
it **finished** — the block is in the engine bring-up / state-6 record
sweeps / `victory_jingle_schedule`→`deferred_cmd_post` /
`frame_tick` tail of `battle_init`.

`frame_tick` (`game/game.c:1314`) calls `memcard_tick()`,
`memcard_slot_poll()`, `pad_state_refresh()`, `gpu_draw_sync(0)`,
`deferred_cmd_drain()` — any of these is a candidate blocking wait
(GPU draw-sync / memcard op / deferred-queue drain). `decomp_trace` tail
is entirely old LZSS/file_load blocks (last at guest cycle ~1.03e9 vs
current ~1.08e11) — **all versus-spawn file loads and decompression
completed**; the CD is idle because the game issued no further request.

CD session cells corroborate: `bcb94=0` (sectors-remaining hit 0),
`bcb88=0x801D47C0 = bcb84(0x801C47C0)+0x10000` = exactly 32 sectors ×
0x800 advanced — the last read session **delivered all 32 sectors**;
`bc848=bc84c=bcba8=bcbac=0` (session callbacks cleared = normal idle
torn down, `bcba4=0` init flag clear).

## 8. Minimal faithful fix location

- **NOT the runtime** (SIO/timer models verified correct vs psx-spx;
  `timers.c` T2 at 0x1F801120/124/128 live-advancing).
- **NOT the device-command layer** — it is healthy per-frame background.
- **NOT a generated-delegation bug** in `dev_byte_xfer`/`fifo_space_check`
  — both are faithful (delegation is correct for the IRQ-boundary
  `dev_byte_xfer`; `fifo_space_check`/`fifo_space_sample` are exact clean
  ports).
- **The fix is game-side**, in the `battle_init` bring-up chain
  (`game/menu.c:5379` → callees `task_pool_init/alloc`,
  `record_init_sweep`, `sprite_place_centered`,
  `victory_jingle_schedule`→`deferred_cmd_post`, and the per-pass
  `frame_tick` → `memcard_tick`/`memcard_slot_poll`/`gpu_draw_sync`).

**Open sub-question for the next agent:** identify the exact blocking
wait inside that chain. Recommended probes on the live 8997 instance:
`wtrace_arm`/`pc_probe_arm` on `memcard_tick`, `memcard_slot_poll`,
`gpu_draw_sync`, `deferred_cmd_drain`, `task_pool_alloc`; `mem_words` on
the deferred-command queue head/tail and the GPU status; and the guest
`sp`/`ra` of the blocked thread (kernel TCB) to recover the WaitEvent
event flag. The missing wakeup is whatever that wait expects — most
likely a memcard/GPU/deferred-queue completion that a prior clean port
stopped delivering, consistent with the versus-spawn stall family in
`proof/versus_phase_machine_deadlock.md` and
`proof/cd_dma_dispatch_channel.md` §9–10.

## 9. Corrections to prior notes

- `renames.map` `cd_*`/`gpu_*` names for `0x801A1834/0x801A189C/
  0x801A1B50/0x801A1F74/0x801A4230/0x80192F14` are **wrong** — they are the
  SIO device-driver layer (pad + memory card), not CD/GPU.
- `fifo_space_check` is a **timer pacing check**, not FIFO space; the
  `0x801FF260/264` cells are `{T2 timestamp, tick limit}`, re-armed every
  pass — they are not a stuck queue.
- The earlier "fifo_space_check deadlock" in FAITHFUL_TIMING_PLAN
  "Session N+5" was a different (pc=0 crash) issue; this wedge has frames
  advancing and is **not** a fifo_space_check livelock.
