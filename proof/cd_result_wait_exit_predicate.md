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

## 5. The ACTUAL blocked site — REFUTED by pc_probe (stale stack residue)

**§5's original "blocked in `cd_file_read_seq → cd_seek_complete_poll →
TestEvent`" conclusion is WRONG — it read stale stack residues.** Decisive
`pc_probe_arm` on the live wedged instance (port 9850, ~5 s window,
`n=48`) recorded **zero hits on every CD-path entry PC**:

| PC | function | hits |
|----|----------|------|
| `0x801565EC` | `cd_file_read_seq` | **0** |
| `0x8019D500` | `cd_seek_complete_poll` | **0** |
| `0x8019D5B0` | `cd_seek_state_machine` (B0/0x0B) | **0** |
| `0x8018CFDC` | `cd_file_load_sm` | **0** |
| `0x801968CC` | `cd_result_wait_a` | **0** |

The `cd_file_read_seq`/`cd_seek_complete_poll` return-addresses on
`sp≈0x801FFDF8` are leftovers from the last *completed* read (~frame 3748) —
the exact stale-residue trap `cd_load_complete_deliver_event.md` session-6
warned about ("current sp 0x801FFDF8 sits in the BIOS/kernel idle loop").
The cumulative `B0/0x0B = 1,402,471` count is frozen history, not a live spin.

**Convergent, pc_probe-verified conclusion** (`cd_load_complete_deliver_event`
sessions 5/6/7 + this probe + `cpu_input_source_arcade.md` +
`cdrom_ring_bfrd_rootcause.md`):

- CD device / IRQ / event / doorbell path: **fully WORKING, EXONERATED.** All
  sectors delivered; all 3 `cd_file_read_seq` batches completed their ch4
  doorbell → `cd_dma_chan_reset` → `DeliverEvent(F0000009)` → `TestEvent`.
- Input: **exonerated.** cdrom.c ring/BFRD: **exonerated** (the one
  `ring_dropped` was a guest-side XA-skip on an unrelated stream).
- The wedge is **`battle_frame` `case3` (DAT1=3) spawn/select phase never
  advancing** — a cooperative game-logic wait, not a kernel block, not CD.
  Spawn commit needs `record+0x10==1 && (phase&0xF00)==0x700`; the
  `0x300→0x700` phase advance + spawn-task arm are not firing. The
  decompressor freeze at seq 213 is the same game-logic starvation (decompress
  task not scheduled), not missing CD data.

**Real open question (upstream of CD entirely):** what writes the `0x700`
phase word and sets `record+0x10==1`, and why isn't it running? That is the
spawn-phase / intro-script / task-scheduling path in
`game/menu.c`/`game/fight.c`/`game/battle.c`.

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
   write. **And no CD wait is live at the wedge** (§5 refuted) — the codec
   thread is in `battle_frame` `case3`, a game-logic spawn-phase wait.

§1–§4 predicate/writer decodes remain valid and useful.
