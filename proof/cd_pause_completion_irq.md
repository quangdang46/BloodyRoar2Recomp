# Pause completion IRQ — mechanism determination (arcade DAT1=3 wedge)

Status: **DETERMINED — mechanism A.** The Pause `CDIRQ_COMPLETE` (reason 2)
was raised into `I_STAT` bit2, delivered as a guest IRQ, and the *entire*
guest handler chain ran to completion: `cd_intc_dispatch` →
`cd_response_pump` → `cd_response_process` consumed the response byte,
the guest acknowledged HINTSTS, and `I_STAT` bit2 was cleared by the
guest's own ack — byte-for-byte the same fntrace signature as the healthy
Pause completions 14 frames earlier. Nothing was dropped at the INTC /
IRQ-presentation layer. The wedge is **downstream of interrupt delivery**:
the result-flag bytes `0x801BCB25/26` the parked `cd_result_wait_a`
polls were never written, and no further CD command was ever issued.

Reproduced deterministically on fresh port **9830** (nav script
`/tmp/nav_arcade_p2_9830.py`, slot-1 CPU-confirm injection). Wedge state
identical to ports 9810/9820: `gs=8 ph=03 b4=0300 b6=0300 +92=0100`,
`DAT1=3`, `0x801BCB24..27 = 02 00 00 00`, `cdrom_state`:
`irq_flag=0 irq_enable=7 i_stat=0 pending.cmd=0x09 phase=1
response_read=1 response_count=1`.

## Instrumentation

`fntrace_arm` (14 targets, armed before the CD-heavy phase):

- `0x801979F8` cd_response_pump, `0x8019A8EC` cd_intc_dispatch,
  `0x8019A814` cd_chan_full_init
- `0x801985C4` cd_read_step_cb, `0x80198838` cd_read_finish_cb
- `0x80196370` cd_response_process + CPS alias entries
  `0x801965CC/0x801966CC/0x80196718/0x80196798/0x80196818`
- `0x801968CC` cd_result_wait_a, `0x80196B4C`, `0x80196E14` cd_cmd_issue

## CD trace — the final Pause (frame 1583–1585, seq 42231–42302)

Identical on 9810 and 9830 (seq numbers differ, order identical):

| seq | frame | kind | detail |
|-----|-------|------|--------|
| 42259/42260 | 1583 | write/cmd | cmd `0x09` (Pause) issued, pc `0x80196FC4`, `i_stat=0x4` |
| 42261 | 1583 | set_irq | reason `0x03` (ACK/INT3), `irq_flag 0→3` |
| 42262–42277 | 1583 | MMIO | guest reads stat/HINTSTS, consumes ACK response, writes HINTSTS ack `0x07` → `irq_flag=0` (42270), `i_stat` bit2 cleared (by 42274) |
| 42278 | 1583 | fire_irq | reason `0x03` again at pc `0x8019A60C`, `i_stat=0x4` |
| **42283** | **1585** | **set_irq** | **reason `0x02` (COMPLETE) at pc `0x8019AA98` — inside `cd_intc_dispatch`; `irq_flag=0x02`, `i_stat=0` pre-raise** |
| **42284/42285** | **1585** | **fire_irq** | **reason `0x02` → `i_stat=0x00000004`; second fire at pc `0x8019A9AC` (dispatch body)** |
| 42286–42295 | 1585 | MMIO | guest IRQ handler: pump reads `0x1F801800`=0x78 (pc `0x80197A1C`), response_process reads HINTSTS `0xE2`, response byte `0x02` (seq 42291, `resp_read 0→1`), writes HINTSTS ack `0x07` → `irq_flag=0` |
| 42296–42302 | 1585 | MMIO | pump re-polls, stat `0x58`, HINTSTS `0xE0` (no pending CD IRQ), exits. `i_stat` cleared to 0 by 42299. **Trace ends — permanently.** |

`psx_irq_raise(2, CDIRQ_COMPLETE)` **occurred** (`set_irq`+`fire_irq`
reason 2, `i_stat=0x4` recorded). `IRQ_DELIVER` **occurred** — guest code
inside `cd_response_pump`/`cd_response_process` executed MMIO in the same
trace window. The HINTSTS acknowledgement at seq 42295 is the *guest's
own normal ack*, identical to every healthy interrupt; it is not a
spurious clear inside the 5000-cycle present window (mechanism B would
require `irq_flag`/`s_irq_i_stat_reason` cleared *before* `I_STAT`
presentation — here presentation happened first, at 42284/42285, and the
flag was cleared only after the handler consumed the response).

## fntrace — the handler chain ran, identically to healthy completions

Wedged completion, frame 1585 (seq → target → ra → args):

```
14107  0x801979F8 pump            ra=0x8019A9C8  a0=4 a1=1
14108  0x80196370 resp_process    ra=0x80197A34 a0=4 a1=1
14109  0x801966CC resp+resume     ra=0x80197A34 a0=0x801BDCC8 a1=1
14110  0x80196370 resp_process    ra=0x80197A34 a0=0x00000002 a1=0x801FC6C8
14111  0x801979F8 pump            ra=0x8019A9C8  a0=4 a1=0x801FC6C8
14112  0x80196370 resp_process    ra=0x80197A34 a0=4 a1=0x801FC6C8
```

Healthy Pause completion, frame 1571 (seq → same targets):

```
13791  pump / 13792 resp_process a0=4 a1=1 / 13793 resume 0x801966CC /
13794  resp_process a0=0x00000002 a1=0x801FC6C8 /
13795  pump / 13796 resp_process a0=4 a1=0x801FC6C8
```

**Entry-for-entry identical.** The `a0=0x00000002 a1=0x801FC6C8`
response_process invocation is the command-complete event dispatch in
both cases.

What happens *after* diverges — and it is all guest-thread side:

- Healthy f1572: `cd_cmd_issue a0=0x02` (Seek, seq 13799) →
  `cd_result_wait_a` (seq 13800) → `cd_cmd_issue a0=0x06` (ReadN,
  seq 13808) → `cd_read_step_cb` resumes firing every sector IRQ.
  Streaming continues for hundreds of frames.
- Wedged f1585: `0x80196B4C` poll (seq 14113) then **nothing**. No
  `cd_cmd_issue`, no `cd_result_wait_a` re-entry, no `cd_read_step_cb`,
  no pump/response_process — `cd_response_pump`'s newest fntrace entry
  is frame 1585; `cd_response_process`'s newest is frame 1585;
  `cd_intc_dispatch` continues once per frame (latest frame ~11000+)
  taking the correct `mask=0` idle branch (`i_stat=0`, `i_mask=0xD`).

## Where the parked thread actually is

`disp_ring` tail (17.2M dispatches) shows the live hot loop repeating:

```
0x80192F14 cd_queue_push   <- ra 0x80126664
0x801936B0 cd_timeout_arm  <- ra 0x80192F40
0x8019A460 cd_timer_read   <- ra 0x80192F40 / 0x80192F88 / 0x80193030
0x8019A7FC cd_u16_swap_store
0x80192E78 gpu_reg_cells_init / 0x80193020 / 0x80193030 ...
```

`cd_result_wait_a`'s decoded loop is "check result bytes → call
`cd_response_process` → yield via `cd_timer_read`/vsync". The loop is
*running* (not a hard block), but `cd_response_process` has not been
re-entered via the wait's direct path (`ra=0x80196A3C`, last seen seq
14094 @ f1583) since the completion IRQ — i.e. the per-iteration
response drain the wait relies on is not re-firing, and the flag bytes
it polls (`0x801BCB25/26`) stay `00 00`.

## Live wedge state (port 9830 `mem_words`)

- `0x801BCB24..27 = 02 00 00 00` — class byte `2` (Pause complete)
  consumed; awaited flag bytes `25/26` never written.
- `0x801BC848 = 0`, `0x801BC84C = 0` — both pump completion callbacks
  NULL (consistent with prior proofs: `sm_wrap`/`read_step_cb` were
  cleared by `scene_entry` (`br2eu_func_801264B4`→`0x80159028`) during
  the battle load and never reinstalled).
- `0x801BC854 = 0x00000002` (status ready, bit4 clear),
  `0x801BC858 = 0`.
- INTC block `0x801BCD18`: in-service/busy word `0x00000001`;
  handler table `0x801BCD1C`: `[0]=0x8019AE68 [2]=0x801979F8 [3]=0x8019AF7C`;
  JB frame `0x801BCD50 = 0x8019A87C` (trampoline), saved regs
  `0x801BDD30/0x80200000/0x801BCD18`.
- Mask `0x801BCD48 = 0x000D`; `cdrom_state.i_stat = 0`.

## Verdict

- **Mechanism: A.** `psx_irq_raise(2, CDIRQ_COMPLETE)` fired, `I_STAT`
  bit2 was presented, the IRQ was delivered, and the guest handler chain
  (`cd_intc_dispatch` → `cd_response_pump` → `cd_response_process` →
  HINTSTS ack) executed completely and identically to the healthy Pause
  completions earlier in the same run.
- **Not B:** no evidence of `irq_flag`/`s_irq_i_stat_reason` being
  cleared inside the `CDROM_IRQ_PRESENT_DELAY` window; the only
  `irq_flag` clear is the guest's own post-consumption HINTSTS ack,
  which is the normal protocol.
- **The dropped edge is not an interrupt edge.** It is the
  *result-routing* step: whatever writes `0x801BCB25/26` (or otherwise
  releases `cd_result_wait_a`) did not run after the Pause completion —
  and correspondingly the game never issued the next Seek/ReadN that
  every healthy Pause→wait cycle is followed by. Both session callbacks
  (`0x801BC848/0x801BC84C`) are NULL at the wedge, matching the prior
  `sm_wrap`-cleared-by-`scene_entry` finding in
  `proof/cd_deferred_irq_starvation.md`.

## Open sub-question for the next pass

Why the identical IRQ-side completion produces flags in healthy cycles
but not here. Candidates to check next (all guest-side, no IRQ defect
indicated):

1. Whether `cd_result_wait_a`'s exit is gated on a *second* response
   byte / result slot that the Pause completion should have delivered
   (`response_count=1` at wedge — verify byte count vs real HW for
   Pause INT2).
2. Whether the wait's direct `cd_response_process` poll
   (`ra=0x80196A3C`) is gated on `0x801BCB18&7` / `0x801BC854&0x10`
   state that the IRQ-side drain already consumed, so the wait never
   re-pumps — i.e. a consume-once vs poll-forever ordering bug in the
   *game's own* driver logic that only manifests when Pause interrupts
   an active ReadN (`read_cmd=0x06 reading=1` at issue time, seq 42259).
3. Whether `pending.cmd=0x09 phase=1` in `cdrom_state` reflects a
   runtime-side pending record the guest already acked — i.e. whether
   `phase` should have advanced on the reason-2 completion.

No runtime fix implemented: the evidence does not support an IRQ/INTC
patch; the break is in guest-visible result state after a fully
delivered interrupt. `generated/` untouched.
