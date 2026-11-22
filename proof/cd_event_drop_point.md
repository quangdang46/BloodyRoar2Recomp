# CD event drop point — where the completion-event chain actually breaks

Diagnosis-only proof. No source was modified. Fresh headless instance on
debug port **8995** (`PSX_DMAPROBE=1 PSX_CDEV=1 PSX_CDEVDBG=1`), navigated
with `/tmp/nav_arcade_8995.py` into the same battle/file-load wedge family
as `arcade_residual_wedge.md` (port 8993) and
`cd_deferred_coroutine_starvation.md`. Ports 8976/8981 untouched; this
instance was killed after capture.

## TL;DR — the exact drop point

The break is **not** IRQ2 delivery and **not** the deferred coroutine.
Both are proven alive end-to-end. The chain breaks one stage later:

> **After the final `Pause(0x09)` completes at frame ~3810, the file-data
> channel never submits its DMA-channel-4 transfer, so
> `cd_dma_chan_reset → DeliverEvent(0xF0000009,0x20)` never runs, the
> `TestEvent(0xF1000008)` flag is never set, and the game-side file-load
> waiter parks forever (~200k frames, zero further CD commands).**

Every prior successful read chunk ends with **two ch4 DMA kick/done pairs**
whose completion IRQ produces the two `DeliverEvent(0xF0000009)` calls the
waiter polls. In the failing chunk **zero ch4 activity occurs after the
Pause** — the channel-entry finalize that should trigger the ch4 submit
never runs.

## 1. The last transaction, frame-by-frame (cdrom_trace_dump, seq 81809–81858)

All at frame 3808–3810, guest PCs resolved via `game/fnmap.c`:

| seq | kind | detail | guest PC → function |
|-----|------|--------|----------------------|
| 81817/81818 | write/cmd `0x09` | Pause issued | `0x80196FC4` → `cd_cmd_issue` |
| 81819 | set_irq `0x03` | ACK phase | `0x80196FC4` |
| 81820–81835 | MMIO R/W `0x1F801800/801/802/803` | IRQ handler drains HIRQSTAT/response FIFO | `0x8019638C/0x80196420/0x80196464/0x80196474/0x80196484` → **`cd_response_process` (0x80196370)**; `0x80197A1C/0x80197AB0` → **`cd_response_pump` (0x801979F8)**; `0x80196984/0x80196AB8` → `cd_result_wait_a` |
| 81836 | fire_irq `0x03` | i_stat=`0x04` presented | `0x8019A60C` → `vsync_wait_n` region (deferred coroutine) |
| 81841 | set_irq `0x02` | COMPLETE phase (Pause delay ~1.2M cycles counting down in `pending_delay`) | `0x8019AA98` → `cd_intc_dispatch` epilogue |
| 81842/81843 | fire_irq `0x02` ×2 | i_stat=`0x04` | `0x80192FA4` → `cd_queue_push`; `0x8019A9AC` → `cd_intc_dispatch` |
| 81844–81856 | MMIO R/W | second-pass ack: reads stat `0x78/0x79`, writes `0x1F801802=7`, `0x1F801803=7`, `0x1F801800=0` | same response functions |
| — | **end of trace** | `total:81869`; last `0x1F8018xx` access frame **3810** | — |

Host log `/tmp/br2_8995.log` line 13226, immediately after the Pause:

```
[cdev] DeliverEvent a0=0xF0000003 a1=0x00000040 cyc=2579631411
```

→ the command-completion event **was** delivered. Then silence.

## 2. What every successful chunk does (same log)

Per read chunk the game performs, in order:

1. `SetLoc(0x02)` → `SetMode(0x0E)` → `ReadN(0x06)` — ch3 DMAs stream
   sectors into the `0x80010000` scratch arena (`dmaprobe kick/done ch3`,
   pc `0x801978AC` → `cd_cmd_issue_b`).
2. `Pause(0x09)` → `DeliverEvent 0xF0000003/0x40` (cmd completion).
3. **`kick ch4` at pc `0x8019BBAC` → `cd_reg_window_op`** — the
   file-data channel DMA (src inside the `0x8001xxxx` arena).
4. ch4 done → DICR IRQ (dicr `0x00900000→0x90900000`, i_stat `0x08`) →
   done-side pc `0x8019D520` → `cd_seek_step` → **`cd_dma_chan_reset
   (0x8019B7E0)` → `DeliverEvent 0xF0000009/0x20`** (the
   `0x801BDEE8==0` direct-BIOS path, `game/cd.c:5146`).
5. A second ch4 kick/done → second `0xF0000009` →
   `B0/0B TestEvent a0=0xF1000008 -> v0=0x1` → next chunk.

Successful examples (log): f3496→f3499, f3645→f3646, f3692→f3696.
**Last ch4 activity in the entire log: frame 3696.** After that the game
kept streaming ch3 sectors (f3804–3808) and Pausing, but never kicked ch4
again.

## 3. Live wedge state (frames ~186k–220k, still advancing)

- `cdrom_state`: `pending{cmd:0x09,active:0,phase:1}` (residue per
  `arcade_residual_wedge.md` §2), `reading:0`, `sector_read_pos:2062/2340`
  (the documented benign 2060-byte consume + 280-byte EDC/ECC tail —
  `cdrom.c:200-216`), `i_stat:0`, `irq_flag:0`, last sector lba 8801 @f3808.
- Channel ring `0x801FC6C0`: table `0x80010000`, cap `0x20`,
  **producer cursor `0x801FC6C8`=2, finalize cursor `0x801FC6CC`=0**,
  consumer-side `0x801FC6AC`=0x1E — entries produced, **none finalized**.
- Job channel: counter `0x801BE768`=`0xFFFFFFFF` (sign set →
  `chan_job_pump` skips `op_chan_fn_invoke` every frame),
  state `0x801FEBE0`=0, mc pump-request flag `0x801FC68C`=0.
- Response cells: `0x801BC854`=2, `0x801BCB24` block `{02,00,00,00}`.
- `irq_state`: `i_stat:0x81` mid-exception sample, `i_mask:0x0D`,
  `dicr:0x00900000` — no CD IRQ owed; device legitimately idle.
- `bioscall_dump`: `B0/11 TestEvent` frozen at **1,462,829** (no longer
  spinning — the waiter is blocked, not polling), `B0/07 DeliverEvent`
  frozen at 111, all event services frozen.
- `gpu_state`: full-frame GP0 traffic continues (27.9M writes) but
  `last_world3d_frame:2901` — a static non-3D screen.

## 4. Function-level liveness (fntrace_arm, ~15 s window, frames ~213k)

Per-frame hits: `0x8019A87C` (deferred entry), `0x8019A8EC`
(`cd_intc_dispatch`), `0x8019AE68` (`cd_cb8_dispatch` → jalr's slot-7
callback `chan_job_pump` `0x801A0110`), `0x8019A460` (`cd_timer_read`),
`0x8019A5D8` (`vsync_wait_n`), `0x80192634` (`cd_cmd_queue_load` — the
per-frame GPU/OTC queue pump, *not* a CD command).

Zero hits (IRQ-time-only functions, correctly dormant): `0x801979F8`
(`cd_response_pump`), `0x80196370` (`cd_response_process`),
`0x80199440` (`cd_mc_state_machine`), `0x8019A814` (`cd_chan_full_init`),
`0x8019AED4` (callback register). Note the earlier "`cd_response_pump`
0 hits" reading was a false negative of arming after the wedge — the
trace dump proves both response functions executed at f3810.

The deferred coroutine's suspended stack still shows the
`cd_file_read_seq(0x80156628) → cd_cmd_queue_load(0x801926AC)` chain —
the file-load caller never returned.

## 5. Answers to the tasked questions

1. **CD IRQ2 fires?** YES — twice at f3810 (`fire_irq` seq 81836/81842/81843,
   `i_stat=0x04`), acked by the guest.
2. **`cd_intc_dispatch` / `cd_response_pump` / `cd_mc_state_machine` run?**
   `cd_intc_dispatch` every frame; `cd_response_pump`+`cd_response_process`
   ran exactly once at f3810 (IRQ-time); `cd_mc_state_machine` never in the
   wedge window.
3. **Deferred coroutine resumes?** YES — every frame (sp_ring TCB
   `0x00008FE0` → `0x8019A87C` → `0x8019A53C`).
4. **Which flag does `cd_cmd_sequence` wait on?** The BIOS event flag for
   spec `0xF1000008` (TestEvent `a0=0xF1000008`), fed by
   `DeliverEvent(0xF0000009,0x20)`. Delivered twice per successful chunk;
   **zero times in the failing chunk**.
5. **Callback slots `0x801BDDC0..0x801BDDDC`?** Slot 7
   (`0x801BDDDC`)=`0x801A0110` (`chan_job_pump`) populated and invoked
   every frame via `cd_cb8_dispatch`; slots 0–6 zero — consistent with
   `cd_init_stage_a` clearing + selective registration. Not the break.

## 6. The broken link, precisely

```
ReadN sectors → ch3 DMA drain → channel-entry finalize
      → ch4 file-data DMA submit (pc 0x8019BBAC, cd_reg_window_op)
      → ch4 done IRQ (i_stat bit3) → cd_seek_step → cd_dma_chan_reset
      → DeliverEvent(0xF0000009,0x20) → TestEvent(0xF1000008)=1 → next chunk
                                     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                        NEVER HAPPENS after the f3808 Pause
```

The Pause at f3808 ran `stop_read_stream()` (`cdrom.c:2303`) while the
ReadN was still streaming — `sector_read_pos` froze at 2062/2340 and the
last ch3 drain finished the same frame. The channel entry covering that
range never reached ready state (finalize cursor stuck at 0), so the
game-side channel layer never submitted the ch4 transfer and never
produced the completion event. The command-side half of the same
transaction *did* complete cleanly (ACK+COMPLETE IRQs, Pause response,
`0xF0000003` delivered).

## 7. Candidate faithful fix locations (ranked by evidence)

1. **`psxrecomp/runtime/src/cdrom.c` — Pause/read-stream truncation
   semantics.** `exec_command` case `0x09` (`cdrom.c:2301-2313`) calls
   `stop_read_stream()` which clears `reading/read_cmd/read_delay` and
   pending data-ready with **no data-end preservation** (`cdrom.c:1764`).
   On hardware a Pause mid-read still lets the drive finish the current
   sector burst / reports the read state the game's channel layer expects.
   Verify whether at the f3808 Pause point the runtime had delivered fewer
   sectors than hardware would have (sector cadence / `data_fifo_ready`
   BFRD pacing, `cdrom.c:1953`), i.e. whether the entry was *actually*
   incomplete or the runtime signalled "not reading" while bytes were still
   owed. The `ring_dropped=2` counter (`cdrom.c:1549`) should be checked
   for *when* those two unread-slot overwrites occurred — if inside this
   load, a dropped sector is the direct cause of the unfinalizable entry.
2. **`psxrecomp/runtime/src/dma.c` — channel-4 completion → DICR → game
   handler.** Proven healthy (every prior ch4 done produced the IRQ and the
   DeliverEvent), so only revisit if (1) shows the submit *was* issued and
   lost — it was not; no ch4 kick exists after f3696.
3. **`game/` generated `cd_mc_state_machine` (0x80199440) finalize gate**
   — only if the entry descriptor bytes at `0x80010000` are shown corrupt
   (the AGENTS.md §8 arena-sharing/tag-overwrite class). The table region
   currently holds file payload; confirming the +4/+6 sequence tags would
   distinguish "game correctly waiting on incomplete data" from "tags
   overwritten by payload DMA".
4. `event_ring.c` / `bios_hle.c` / `psx_bios_backend.c` — **not
   implicated**: DeliverEvent and TestEvent both work (111 deliveries,
   flag consumed every prior chunk).

## 8. Reproduction commands

```
PSX_DMAPROBE=1 PSX_CDEV=1 PSX_CDEVDBG=1 \
  ./build/BloodyRoar2_Recompiled --disc ".../Game.cue" \
  --debug-port 8995 --headless
# navigate: /tmp/nav_arcade_8995.py ; then:
cdrom_command_history {"n":25}   # last exec: 0x09 Pause @frame 3808
dmaprobe log /tmp/br2_8995.log   # last ch4 kick f3696; DeliverEvent tail
fntrace_arm {"target":"0x80199440"} etc → per-frame pollers only
mem_words 0x801FC6C0 / 0x801BE768 / 0x801FEBE0 / 0x801FC68C
```
