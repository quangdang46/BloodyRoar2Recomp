# Kernel thread / event-wait wedge — `gs=8 ph=03` arcade announcement stall

Live instance: debug port 9850, wedged at frame ~101k+ (`gs=8`, `ph=03`,
phase `0x300` announcement; needs `0x700` to spawn fighters). Scope: the
**kernel-thread / BIOS-event-wait layer only** (sibling agents own the
`cdrom.c` ring and `cd_result_wait_a` exit-predicate forensics).

**Headline:** there is **no parked BIOS thread and no BIOS event wait**.
Exactly one kernel thread exists and it is *runnable* — it is livelocked
inside `cd_file_read_seq`'s frame-synchronized CD command-issue / status
poll (`cd_timer_read` → vsync_wait_n → `C0:0x0A ChangeClearRCnt` yield).
The CPU idles at `pc=0` only inside each frame slice; the VBlank IRQ
coroutine runs once per frame. The CD device has been **completely silent
since frame ~1539**, so the status/flag the poll waits for never changes.
The kernel thread/event machinery itself is healthy.

---

## 1. Kernel thread enumeration (physical kernel RAM)

OpenBIOS tables live in low kernel RAM (physical addresses, **not** the
`0x801Fxxxx` KSEG0 window — `mem_words` wants the physical/kernel-relative
address). Globals decoded at `0x8D80..0x8E00`:

| Cell | Value | Meaning |
|------|-------|---------|
| `0x8DD8` | `0x00008D98` | self/kernel ptr |
| `0x8DE0` | process table | stride record |
| `0x8E00` | `0x00006538 / 0x1C8` | thread-table meta |
| `0x8E08` | **EvCB base** | `struct EventInfo`, stride `0x1C` (7 words) |
| `0x8FD0` | `0x00008FE0` | **current TCB pointer → 0x8FE0** |

### EvCB snapshot (stride `0x1C`, base `0x8E08`)

Hardware IRQ events occupy slots 0–7 (`class=0xF4000001`, `status=0x2000`
enabled, per-slot `spec`, `handler=0x801A0B10/24/38/4C`).

**Slot 8 @ `0x8EE8`** — the only game event:

```
class   = 0xF0000009      (CD channel/SPU "doorbell" class)
status  = 0x2000          ENABLED — NOT pending (0x4000 would be pending)
spec    = 0x20
mode    = 0x2000          no-callback (polled via TestEvent/WaitEvent)
handler = 0
```

Slots 9–15 all zero (free). `DeliverEvent(0xF0000009,0x20)` has **never
left this event pending** — consistent with `TestEvent` never returning 1.

### Thread table

`0x8FC8`: `0x6538 / 0x10` header. **TCB[0] @ `0x8FE0`** (record stride
`0xC0`; next slot @ `0x90A0` = `flags 0x1000` unused, rest zero — and all
subsequent slots likewise free):

```
+0x00 flags  = 0x00004000   ACTIVE (in-use/running — NOT a wait state)
saved context (gp..ra block):
  gp  = 0x801BEEDC   sp  = 0x801FFDF8   fp  = 0x80200000
  ra  = 0x8019A53C   epc = 0x8019A600   SR  = 0x40000404  cause = 0x400
  a0  = 0x00017B63   a1  = 0x8000       t1  = 0x0000000B  t3 = 0xF0000009
```

The saved context is the **CD deferred-coroutine / timer-sync context**
(`epc=0x8019A600` is inside the vsync_wait_n `0x8019A5D8`; `ra=0x8019A53C`
is the `cd_timer_read` return point). Only **one** thread exists — the
main game thread. There is no second runnable or blocked thread, and no
thread parked in a BIOS `WaitEvent` record.

---

## 2. The actual wait — a frame-synced poll, not a BIOS event wait

### Live call chain (guest stack @ `sp≈0x801FFDF8`)

Return addresses on the thread stack:

```
0x801ffe10  0x8019A53C   <- cd_timer_read (0x8019A460) frame
0x801ffe18  0x801926AC   <- caller of cd_timer_read
0x801ffe1c  0x80156628   <- cd_file_read_seq (0x801565EC) region
0x801ffe04  0x80192944   <- deeper CD-driver frame
```

`cd_file_read_seq` disasm (`0x801565EC`):

```
jal cd_mode_store(0)                 ; 0x8019D4D0
jal cd_status_shifted_read(s0)      ; 0x8019D470
jal cd_seek_param_issue_clamped(s1,s2) ; 0x8019D410  <- issues seek+read
LOOP: jal cd_seek_complete_poll(0)  ; 0x8019D500
      beqz v0, LOOP                 ; 0x80156628->0x80156630
```

`cd_seek_complete_poll(0)` calls the guest function at `0x8019D5B0`
(`cd_seek_state_machine` in `renames.map`) **once** per outer iteration;
its body is literally the BIOS trampoline `t2=0xB0, t1=0x0B; jr $t2` =
**B0/0x0B `TestEvent`**, and returns the result; the `do{}while(0)` loop
in `cd_file_read_seq` re-polls.

### Decisive: the thread is NOT in the TestEvent loop

`bioscall_dump` lifetime counts, sampled twice 2 s apart:

| svc | count before → after |
|-----|----------------------|
| B0:0x0B TestEvent | 1,402,471 → **1,402,471 (FROZEN)** |
| B0:0x0A WaitEvent | 5 → 5 |
| B0:0x07 DeliverEvent | 72 → 72 |
| B0:0x17 (IRQ-return/EI) | — |

TestEvent is **not being called at all** right now. The thread never
reaches `cd_seek_complete_poll`'s TestEvent — it is stuck **earlier**, in
the command-issue / status path, inside `cd_timer_read`'s frame-sync
delay.

### The spin site — `cd_timer_read` → `vsync_wait_n` → `C0:0x0A`

`get_registers` samples (pc always `0`, epc varies):

```
epc = 0x8019A648 / 0x8019A600   <- inside vsync_wait_n 0x8019A5D8 loop
ra  = 0x8019A53C / 0x80192F88   <- cd_timer_read / CD-driver callers
sp  = 0x801FFDF8 / 0x801FFE18   <- thread stack (kernel-idle uses 0x8548)
```

`vsync_wait_n` (`0x8019A5D8`, `a0=deadline_tick, a1=count`):

```
counter = a1 << 15                       ; bounded iteration budget
while (tick@0x801BDDE0 < a0 && --counter != -1) { }
on counter expiry: jal 0x80197AD0; jal 0x80199FE0;
                   jal 0x8019A670  ->  C0:0x0A = ChangeClearRCnt  ; frame-sync yield
                   -> return
```

`cd_timer_read(0x8019A460)` is the CD driver's frame-synchronizer: for
`a0>0` it computes a tick deadline, calls the vsync_wait_n twice
(`ra=0x8019A53C` = return from the 2nd call `delay(tick+1,1)`), then does a
VBlank-edge wait. Its status-pointer cells `0x801BCBA8 / 0x801BCBAC /
0x801BCBB0` are **all 0** → it dereferences phys `0` (`=0x00000800`,
bit6 clear → the vsync-edge branch is skipped; the "wait stable" reads
phys0 twice → equal → returns). So `cd_timer_read` **returns** each call —
the hang is the *caller's* poll loop re-issuing it, not `cd_timer_read`
itself.

### Why `pc=0` / "no runnable thread"

`sp_ring` (319k entries) shows an identical 3-switch pattern **every
frame** on `tcb=0x8FE0`:

```
sp 0x801FFE18 -> 0x801BDD30  target 0x8019A53C   ; IRQ -> CD coroutine stack
sp 0x801BDD30 -> 0x00008548  target 0x00002924   ; -> kernel idle (ra 0x282C)
sp 0x00008548 -> 0x801BDD30  target 0x8019A87C   ; coroutine resume point
```

Between IRQ slices the CPU sits in the **kernel idle loop** (`pc=0`,
kernel stack `0x8548`, target `0x2924`). The game thread contributes one
bounded `vsync_wait_n` slice per frame and yields — a frame-synced
**busy-poll**, not a descheduled wait. `fntrace` confirms the deferred CD
coroutine is alive: exactly one `cd_intc_dispatch`(0x8019A8EC)+
`cd_cb8_dispatch`(0x8019AE68) pair per frame (VBlank tick + job pump).

The tick `@0x801BDDE0` advances normally (`0x18852→0x18881` over ~17
frames ≈ +1/frame), driven by the VBlank coroutine — so the
`vsync_wait_n` deadline is reachable; the loop simply re-arms forever
because the polled condition never becomes true.

---

## 3. The producer that should satisfy the wait — and why it never fires

The game is waiting for the **CD channel command/read to be accepted and
complete** — the same "doorbell" completion family that ends in
`DeliverEvent(0xF0000009,0x20)` (EvCB slot 8). Producer chain:

```
CD channel op (ch4 DMA / custom channel @ blk 0x1F801C00)
  -> DMA-done IRQ (I_STAT bit3) or CDROM IRQ (I_STAT bit2)
  -> kernel IRQ -> deferred coroutine resume @0x8019A87C
  -> cd_intc_dispatch (0x8019A8EC): pending = I_STAT & I_MASK & 0xD
       handler[0]=0x8019AE68 cb8_dispatch   (VBlank tick + job pump)
       handler[2]=0x801979F8 cd_response_pump (CDROM status/callbacks)
       handler[3]=0x8019AF7C DMA dispatch -> tbl[4]=0x8019B7E0
            cd_dma_chan_reset -> DeliverEvent(0xF0000009,0x20)
            (game/cd.c:5188-5191 -> cd_bios_svc_07, unless cb cell
             0x801BDEE8 set — currently 0 -> DeliverEvent armed)
```

INTC block `@0x801BCD18`: `handler[0]=0x8019AE68`, `handler[2]=0x801979F8`,
`handler[3]=0x8019AF7C`, `maskC=0xD`; deferred-resume cell `0x801BCD50 =
0x8019A87C`, coroutine stack `0x801BDD30`.

### Everything upstream is dormant / silent

- **CD device silent since frame ~1539.** `cdrom_command_history` last
  entry seq144 = cmd `0x09` (Pause) at frame 1539; `cdrom_state` shows
  `reading=0`, `pending` stale (`active=0` — `pending.cmd/phase` are
  display leftovers, `pending.pending=0`), `irq_flag=0`, `i_stat=0`.
  `cdrom_timing_dump` last sector = lba 8801 @ frame 1539. **No CD
  command issued or completed for ~89k frames.**
- `cd_file_load_sm` (0x8018CFDC) — **0 fntrace hits**: the decompressor's
  requested asset block 213 (`heapA_w_menu` → `0x800E1064`) never entered
  the file-load SM.
- Session callbacks `0x801BC848 = 0x801BC84C = 0`; channel/job queue
  `0x801BDDC0..0x801BDDD8 = 0` (only `0x801BDDDC = 0x801A0110 chan_job_pump`
  ptr); channel-copy SM dormant (`0x801FC68C=0`, scan `2>finalize=0`).
- DMA ch4 (the CD data channel; MADR ptr cell `0x801BDEB4=0x1F8010C0`,
  CHCR `0x801BDEB8=0x1F8010C8`): last `MADR=0x0004F6E0`, `CHCR=0x201`,
  `active=0 remaining=0` — finished; `DICR=0x00900000` has **no** ch4
  done-flag (bit28) pending.
- Completion/session cells `0x801BCBA4/0x801BCBA8/0x801BCBAC = 0`;
  `0x801BCB25/0x801BCB26` result bytes are stale leftovers (per
  `proof/cd_result_wait_exit_predicate.md`).

So the DeliverEvent producer is not "missing" — it is correctly wired —
but **no CD channel operation is in flight to complete**. The last CD
activity was a `Pause(0x09)` that finished at frame ~1539; whatever should
have issued the next read/channel op for the asset load never ran, so the
game's issue-path poll waits on a status that can never arrive.

---

## 4. Root-cause classification

**Faithful guest behavior at the thread/event layer; the defect is
upstream in the CD command/channel producer (sibling scope).**

- The single TCB is correctly scheduled (flags `0x4000`, context save/
  restore via `0x8019AD80/0x8019ADCC`, IRQ coroutine resumes every VBlank).
- The guest legitimately frame-sync-polls the CD channel inside
  `cd_file_read_seq → cd_seek_param_issue_clamped → cd_seek_param_issue
  (0x8019BBC4) → cd_reg_window_op (0x8019B944)` using
  `cd_timer_read`/`vsync_wait_n`/`C0:0x0A ChangeClearRCnt` — this is the
  game's own busy-wait-with-frame-yield idiom, not a BIOS block.
- `TestEvent`/`DeliverEvent`/EvCB slot 8 are intact; the event simply has
  no producer because the CD device issued nothing after frame ~1539.
- **Not a kernel-thread bug, not a BIOS-event bug, not a TestEvent
  livelock** (the count is frozen — TestEvent isn't even reached).

The wedge is therefore the *CD command-issue / channel-state* layer:
the read for asset block 213 was never issued (or its completion never
signaled) after the frame-1539 `Pause`. That is precisely the
`cdrom.c` ring / `cd_result_wait_a` / `cd_mc_state_machine` territory
owned by the sibling agents — see also `proof/cd_deferred_coroutine_
starvation.md`, `proof/cd_read_done_event.md`,
`proof/cd_result_wait_exit_predicate.md`.

**No code fix implemented** — no concrete faithful defect was proven in
the kernel thread/event layer; the evidence points to the CD producer
path, which is outside this task's scope.

---

## 5. Evidence index (all from live port 9850)

| Fact | Command / address |
|------|-------------------|
| 1 thread, TCB 0x8FE0 flags 0x4000 | `mem_words 0x8fd0/0x8fe0/0x90a0` |
| EvCB[8] enabled not pending | `mem_words 0x8ee8` = `F0000009 2000 20 2000 0 0 0` |
| INT dispatch table | `mem_words 0x801bcd18` → handlers/mask `0xD` |
| Deferred resume `0x8019A87C` | `mem_words 0x801bcd50` |
| TestEvent frozen 1,402,471 | `bioscall_dump` ×2 (2 s apart) |
| DeliverEvent 72 lifetime | `bioscall_dump` B0 idx7 |
| Coroutine per frame | `fntrace_dump` → 0x8019A8EC+0x8019AE68 ×1/frame |
| Kernel-idle ping-pong | `sp_ring` → `0x8548/0x2924` each frame |
| epc in vsync_wait_n | `get_registers` → `0x8019A600/0x8019A648` |
| CD silent since f1539 | `cdrom_command_history` seq144 Pause; `cdrom_timing_dump` last lba8801 |
| ch4 DMA idle | `dma_state` ch4 `active=0`; `DICR=0x00900000` |
| Session/job cells 0 | `mem_words 0x801bc848/0x801bcba4/0x801bddc0` |
| Tick advancing | `mem_words 0x801bdde0` ×4 |

Guest disasm referenced: `cd_file_read_seq 0x801565EC`,
`cd_seek_complete_poll 0x8019D500`, `cd_seek_param_issue_clamped
0x8019D410`, `cd_seek_param_issue 0x8019BBC4`, `cd_reg_window_op
0x8019B944`, `cd_reg_stream_push 0x8019B620`, `cd_timer_read
0x8019A460`, `vsync_wait_n 0x8019A5D8`, `C0:0x0A ChangeClearRCnt`
trampoline `0x8019A670`, `cd_dma_chan_reset 0x8019B7E8`, DMA dispatch
`0x8019AF7C`.
