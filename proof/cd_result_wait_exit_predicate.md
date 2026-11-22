# cd_result_wait_a/b exit predicates + 0x801BCB25/26 writer — and the REAL wedge site

Date: session continuing arcade/versus-CPU wedge investigation.
Live instance: port 9850, wedged at `gs=8 ph=03`, frame ~45k+, codec thread
(TCB 0x8FE0) yielding once per frame.

**Headline correction to the working premise:** the codec thread is NOT parked
in `cd_result_wait_a`/`cd_result_wait_b`. The `0x801BCB25/26` bytes are stale
leftovers from an earlier completed wait. The live guest stack shows the thread
blocked in `cd_file_read_seq → cd_seek_complete_poll → cd_seek_state_machine
(B0/0x0B TestEvent)`, waiting on event `0xF1000008` / flag `0x801BDE40` — the
channel/SPU "doorbell" completion that never arrives. The CB25/26 question is
answered below anyway because it is now fully decoded.

---

## 1. `cd_result_wait_a` (0x801968CC) — exact exit predicate

Disasm: `disc/SLES_017.22`, text base `0x8011E800`+0x800. Generated body:
`generated/SLES_017.22_full_45.c` `br2eu_func_801968CC` (33 blocks, 640 B).
Clean wrapper `game/cd.c:618` delegates via `br2_call_generated` — correct,
the Sync-wait must cross block boundaries for `psx_check_interrupts_at`.

Register map: `s6=param_1`, `s7=param_2` (out buf), `s2=0x801BCB24`,
`s5=0x801BCB25`, `s3=2`, `s4=0x801BC8EC`, `fp=0x801BC86C`.

Per outer iteration (loop head `0x8019694C`):

1. `a0=-1; jal cd_timer_read (0x8019A460)` — returns `*(0x801BDDE0)` tick,
   NON-blocking for a0<0 (early-out at `0x8019A4C0`).
2. Deadline check `0x80196954-0x80196964`: `*(0x801FC6E0)` (set at entry to
   `first_ret+0x3C0`) `< v0` → timeout path `0x80196998`.
3. Iteration counter `*(0x801FC6E4)`++ ; `>0x3C` (60) → same timeout path.
4. Timeout path: `cd_str_print("CD timeout: ")` + printf
   `"%s:(%s) Sync=%s, Ready=%s\n"` (fmt `0x801257B4`, name cell
   `0x801FC6E8`="CD_sync" `0x8012582C`), `cd_cmd_state_reset` (0x801972A8),
   `j 0x80196A04` with `v0=-1` → **return -1**.
5. `cd_intc_busy_read` (0x8019A7D4 = `lh 0x801BCD1A`); if 0 → skip to result
   check `0x80196ABC`.
6. Else drain loop `0x80196A34`: `s1 = *(*(0x801BCB0C)→0x1F801800) & 3`;
   `s0 = cd_response_process()`; while `s0!=0` dispatch session callbacks —
   `s0&4` → `jalr *0x801BC84C` with `a0=lbu(0x801BCB25)`, `a1=0x801FC6D0`;
   `s0&2` → `jalr *0x801BC848` with `a0=lbu(0x801BCB24)`, `a1=0x801FC6C8`;
   loop until `s0==0`, then write `s1` back through `*0x801BCB0C`.
7. **Result check `0x80196ABC`**: `a2 = lbu(0x801BCB24) & 0xFF`.
   - `a2==2` (`beq a2,s3`) **or** `a2==5` (`bne a2,5` not taken) → success
     `0x80196AD8`: `sb 2 → 0x801BCB24` (consume marker), copy 8 bytes
     `0x801FC6C8 → param_2` (loop `0x80196AF4`, 8 iters), **return a2 (2 or 5)**.
   - otherwise `0x80196B14`: `param_1==0` → loop to step 1; `param_1!=0` →
     **return 0** (non-blocking poll).

So `param_1` = "don't block" flag; `param_2` receives the 8-byte result block
at `0x801FC6C8`. The wait is **bounded**: ≤60 outer iterations or the
`+0x3C0` deadline → `-1` + "CD timeout" print + `cd_cmd_state_reset`.

## 2. `cd_result_wait_b` (0x80196B4C) — the CB25/26 waiter

Identical skeleton; differences (`s7=param_1`, `s3=param_2`, `s2=CB24`,
`s6=CB25`, `s4=CB26`, marker string `0x80125834`="CD_ready"):

- Result check `0x80196D3C`: `a2 = lbu(0x801BCB26)`; if nonzero →
  `sb 0 → CB26` (consume), copy 8B `0x801FC6D8 → param_2`, **return a2**.
- elif `a2 = lbu(0x801BCB25)` nonzero → `sb 0 → CB25`, copy 8B
  `0x801FC6D0 → param_2`, **return a2**.
- else `param_1==0` → loop; `!=0` → return 0. Timeout → -1.

Callers of the `0x80195D44` thunk: `memcard_tick` (0x80157A00),
`cd_read_poll` (0x80198E2C), `cd_mc_state_machine` (0x80199440).

## 3. WRITER of 0x801BCB25/0x801BCB26 — `cd_response_process` (0x80196370)

`0x801BCB24` is a 3-byte result record `{class, r1, r2}`. Written ONLY inside
`cd_response_process`'s per-type cases. The "type" is read at `0x80196388-0x801963B4`:
`a0 = *(0x801BCB18)` = **`0x1F801803` (HINTSTS)**, `type = lbu(a0) & 7` =
`irq_flag & 7` = the pending CD interrupt class:

| irq_flag | CDIRQ_* | case target | writes |
|---|---|---|---|
| 1 | DATA_READY | `0x80196718` | `CB25 = (s1?5:1)`; copy param→`0x801FC6D0`; ret 4 |
| 2 | COMPLETE | `0x801966CC` | `CB24 = (s1?5:2)`; copy→`0x801FC6C8`; ret 2 |
| 3 | ACK | `0x801965CC` | `CB24 = 3 or 2` (table `0x801CC90C[ch]`); ret 1/2 |
| 4 | DATA_END | `0x80196798` | `CB26=4`, `CB25=4`; copy→`0x801FC6D8`+`0x801FC6D0`; ret 4 |
| 5 | ERROR | `0x80196818` | `CB25=5`, `CB24=5`; copies; ret 6 |
| 0,6,7 | —/unknown | `0x801968B4`/`0x80196898` | ret 0 (+ "CDROM: unknown intr(%d)" print) |

Jump table `0x80125818` (5 entries, indexed by `type-1`).

`s1 = *(*(0x801BCB0C)→0x1F801800) & 3` = `index_reg & 3` (0 in normal
operation) — it only selects the *value* written (1 vs 5), never whether.

**Gating:** the writes are unconditional inside each type case. The case is
reached only when (a) `cd_response_process` is actually invoked — by the wait
loops' drain (`cd_intc_busy_read()!=0` path) or the deferred IRQ coroutine —
and (b) `irq_flag&7` equals that type, i.e. a DATA_READY / DATA_END / ERROR
interrupt is pending. `0x801BC848/0x801BC84C` session callbacks are NOT the
flag writers; they are invoked *after* `cd_response_process` returns bit
flags 4/2, receiving `a0=lbu(CB25 or CB24)`, `a1=ctx`.

## 4. Why 0x801BCB25/26 stay 0 — and why that's a red herring

Live reads (9850): `0x801BCB20..` = `{ptr 0x1F801C00, word@CB24 = 0x00000002}`
→ `CB24=2, CB25=0, CB26=0`. Marker cells `0x801FC6C0..`: `C6C8=2, C6D0=0x22,
C6E0=0x91B, C6E4=1, C6E8=0x80125834`("CD_ready") — **stale**: wait_b stores
that string at entry and nothing clears the cells on exit, so they only prove
wait_b ran *sometime earlier*, not that it is running now.

`fntrace` (armed 0x801968CC/0x80196B4C/0x80195D44/0x80198E2C/0x80199440/
0x80157A00/0x80196370): **zero entries in ~34k frames** — no CD driver wait or
`cd_response_process` has been *entered*; the only CD-side activity is the
deferred coroutine `cd_intc_dispatch` (0x8019A8EC) at 1/frame taking the idle
branch (`i_stat&0xD==0`, CD idle). `irq_flag=0`, `pending.cmd=0x09` consumed,
`reading=0`, last CD command = Pause (cmd 0x09) exec @frame 1539 → COMPLETE
(type 2) → `CB24=2`. Nothing since.

So CB25/26 are 0 because **no DATA_READY/DATA_END/ERROR CD interrupt has been
presented since they were last consumed** — the drive went idle after the
Pause. That part is faithful guest behavior, not a dropped write.

## 5. The ACTUAL blocked site (live guest stack, sp≈0x801FFDF8, TCB 0x8FE0)

Code-pointer cells on the codec thread's guest stack (9850):

```
0x801FFE10 = 0x8019A53C   ra → cd_timer_read (0x8019A460)+0xDC — return addr
                          of `jal vsync_wait_n` (0x8019A5D8) at 0x8019A534,
                          i.e. the a0>0 "wait ~1 tick + GPUSTAT(0x1F801814)
                          bit22/bit31 flip" path. Innermost game frame.
0x801FFE18 = 0x801926AC   ra → cd_cmd_queue_load (0x80192634)+0x78 — return
                          addr of `jal cd_timeout_arm` (0x801936B0).
0x801FFE1C = 0x80156628   ra → cd_file_read_seq (0x801565EC)+0x3C — return
                          addr of `jal cd_seek_param_issue_clamped`
                          (0x8019D410) at 0x80156620.
0x801FFE38/3C = 0x80126664 → inside the battle/scene state machine
                          (ret of `jal 0x8016FCC4` at 0x8012665C).
```

Frame-layout caveat: intermediate ra cells are consistent with
`cd_file_read_seq → cd_seek_param_issue_clamped → cd_seek_param_issue
(0x8019BBC4) → cd_cmd_queue_load → cd_timeout_arm → cd_timer_read →
vsync_wait_n`, with `cd_seek_complete_poll`/`cd_seek_state_machine`
(B0/0x0B trampoline) frames interleaved per the loop structure below.

Decoded flow (`disc/SLES_017.22` disasm):

- `cd_file_read_seq` (0x801565EC): `cd_mode_store(0)` → `cd_status_shifted_read`
  (0x8019D470) → `cd_seek_param_issue_clamped` (0x8019D410 → clamps a1 to
  ≤0x7FEFF0, calls `cd_seek_param_issue` 0x8019BBC4, clears `0x801BDEE4`)
  → **`do { cd_seek_complete_poll(0) } while (!v0)`** (`0x80156628` jal /
  `beq v0` loop at `0x80156630`).
- `cd_seek_complete_poll` (0x8019D500): early-out if `*(0x801BDE40)==1` or
  `*(0x801BDEE4)==1`; else `a0=*(0x801BDE38)=0xF1000008`, loop
  `cd_seek_state_machine(0x8019D5B0)` = **BIOS B0/0x0B trampoline**
  (`jr 0xB0`, `t1=0xB`) = TestEvent on the channel/SPU doorbell event;
  result stored to `0x801BDEE4`.
- `bioscall_dump`: **B0/0x0B = 1,402,471 calls** — the TestEvent spin.
  (`bios_svc_trace` tail is all `B0:0x17` from `0x8019AA9C` = the healthy
  per-frame `cd_intc_dispatch` idle path.)

Live flag cells (9850): `0x801BDE38=0xF1000008` (armed), `0x801BDE40=0`,
`0x801BDF84=0`, `0x801BDEE8=0`, `0x801BE768=0xFFFFFFFF` (no active job),
IRQ2 cb8 table `0x801BDDC0[0..6]=0`, `[7]=0x801A0110` (`chan_job_pump`).
Channel ring cursors: `0x801FC6C8(prod)=2`, `0x801FC6CC(fin)=0`.

This is the SAME wedge pinpointed in `proof/cd_load_complete_deliver_event.md`
(session-2) and `proof/cd_read_done_event.md`: the codec thread waits for the
**final ch4 (SPU) DMA "doorbell"** whose done-IRQ → `cd_dma_chan_reset` →
`DeliverEvent(0xF0000009,0x20)` should set the event flag and wake
TestEvent. The doorbell channel command was produced (prod=2) but never
finalized (fin=0); no ch4 kick/done after ~frame 3797 while ch3 sector DMA
streamed to LBA 8801. The per-command completion-callback table
`0x801BDDC0[0..6]` is empty, so the IRQ2 dispatch has nothing to run the
completion step that would produce the doorbell.

## 6. Answers to the posed questions — summary

1. **wait_a exit predicate:** `lbu(0x801BCB24) ∈ {2,5}` → consume (write 2),
   copy 8B `0x801FC6C8→param_2`, return the byte; other value → loop while
   `param_1==0`, else return 0; 60-iteration/`+0x3C0` deadline → print
   "CD timeout: Sync=…", `cd_cmd_state_reset`, return -1. (Disasm addrs:
   check `0x80196ABC-0x80196AD0`, success `0x80196AD8-0x80196B10`, loop-back
   `0x80196B14→0x8019694C`, timeout `0x80196960/0x8019698C→0x80196998`.)
2. **CB25/26 writer:** `cd_response_process` (0x80196370) cases for CD IRQ
   types DATA_READY(1)→CB25, DATA_END(4)→CB25+CB26, ERROR(5)→CB25 —
   unconditional once that response type is processed; gated only on a
   pending irq_flag of that class reaching `cd_response_process`.
   `bc848/bc84c` are downstream session callbacks, not the writers.
3. **Why they stay 0:** no DATA_READY/DATA_END/ERROR interrupt since last
   consume — drive idle post-Pause(COMPLETE→CB24=2). Faithful, not a dropped
   write. **And the codec thread isn't waiting on them anyway** — it is in
   `cd_seek_complete_poll`'s B0/0x0B TestEvent on `0xF1000008`/
   `0x801BDE40`, starved by the missing ch4-doorbell completion
   (channel ring prod=2/fin=0, empty `0x801BDDC0` callback slots) — the
   previously-documented wedge, unchanged.

No speculative fix implemented; this is faithful guest behavior downstream of
the missing doorbell production (tracked in cd_load_complete_deliver_event.md).
