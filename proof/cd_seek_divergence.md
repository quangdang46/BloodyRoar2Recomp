# CD seek wedge — divergence hunt (read-only diagnosis)

Instances: wedged **6020** (PID 1061, frame ~282k, TestEvent spin) and **5800**
(PID 28536, Game.cue — found ALSO wedged at the identical signature: frame
~65.9k, `pending.cmd=0x09 phase=1`, BIOS spin ra=0x8019AA9C). The earlier
"working" 5900 is gone; 5800 is a second wedged specimen, useful as a
cross-check (its records differ only in `rec[74]`).

## Runtime state (6020, live dumps)

Channel records (240 B each, base `0x801FF080` + idx*0xF0):

| field | rec0 `0x801FF080` | rec1 `0x801FF170` |
|---|---|---|
| rec+48 datptr | `0x801BEFA8` → `00 41 ff ff…` | `0x801BEFCA` |
| rec+60 cmdptr | `0x801FEFF0` → `00 5a ff ff…` | `0x801FF013` → `00 5a ff ff…` |
| rec[52..55] | `00 00 01 00` (54=1,55=0) | same |
| rec[68..71] | `00 02 01 00` (69=2,**70=1**,71=0) | same |
| rec[72..75] | `00 01 03 00` (**73=1**,74=3) | `00 01 04 00` (74=4) |
| rec[80] | 0 | 0 |
| rec[20],rec[24] fn | 0, 0 | 0, 0 |
| rec[232],[233] | 0, 0 | 0, 0 |

5800 rec0 identical except `rec[74]=0x0A`.

fntrace (armed live on 6020, ring tail):
- `cd_cmd_state_rx` `0x801A3E94` fires **every frame per record**:
  rec0 ra=`0x801A3550` a1=`0x801BE7C4`; rec1 ra=`0x801A1C98` a1=`0xFFFFFFFE`.
  (ra sites = return into generated trampolines `br2eu_func_801A352C` /
  `br2eu_func_801A1C90`, which jalr through the `*(0x801BE7A0)=state_rx` /
  `*(0x801BE7A4)=qbyte_take` hook table.)
- `cd_cmd_data_byte` `0x801A39BC` fires per frame per record (ra `0x801A3614`/
  `0x801A3584`, inside `br2eu_func_801A35DC` response-byte pump).
- **Never fire**: `cd_cmd_state_commit` `0x801A3D84`, `dev_cmd_seq`
  `0x801A1B50`, `dev_cmd_round_dispatch` `0x801A1E84`, `cd_cmd_queue_service`
  `0x801A189C`, `cd_cmd_rec_init` `0x801A3014`, `op_tag_dispatch` `0x801A24DC`,
  `cd_cmd_rec_scan` `0x801A3A78`, `cd_cb7_run` `0x8019AF84`,
  `cd_dma_chan_reset` `0x8019B7E0` (fntrace only sees generated-side calls;
  clean→clean `call_guest` hops are invisible, so "never" is strong for
  generated callers, weak for clean-only callers).
- `cd_cb8_dispatch` `0x8019AE68` fires every frame (ra `0x8019A9C8`); its
  channel-handler table `0x801BDDC0[7]` = `0x801A0110` = `chan_job_pump`
  (clean `game/cd.c:5595`). Job counter `*(0x801BE768)=0xFFFFFFFF` (negative)
  → `job_counter_sign()`=1 → pump takes the idle path; continuation fn
  `*(0x801FEBE0+0x44)=0` → nothing pending. Channel-job layer is IDLE.

## STATIC: clean-C vs generated, game/cd.c:5714-6100

### cd_cmd_state_rx (`game/cd.c:5822-5900`) vs `br2eu_func_801A3E94`
(`generated/SLES_017.22_full_50.c:7256-9194`)

Verified instruction-faithful for: nibble-0 early path (datptr[0]=0xff,
datptr[1]=0, rec[232]=rec[53]=0, jalr `*(0x801BE784)`=cd_cmd_rec_clear),
nibble latch + nibble==15 restore, datptr head reset + payload copy
`datptr[i]=cmdptr[i]` i=2..rec[68)-1 with unconditional `rec[53]=rec[68]`,
the cmd[1]/rec[70]/rec[20]/rec[80] notify-gate lattice, `rec[232]==a1s`
notify skip, `rec[74]=0`, b70==255→255, `b70!=0 && rec[55]==0 → b70-2`,
`(b70-2)&0xff<252 && cmd[0]!=0xf3 → BE784`, b70==1→{rec[71]=0,rec[70]=2,ret2},
b70==0→{rec[73]=1,rec[70]=1,ret1}, else `rec[24]`-fn/`0x801a2560` +
`rec[70]+=r`.

**DIVERGENCE A — missing `b70==254` case.** Generated dispatch tail:
`0x801A4050` loads rec[70]; `0x801A4060` splits `<2`/`>=2`; on the `>=2`
path `0x801A4078: beq v1, v0(=254)` → `block_801A40A8` stores 254 back to
rec[70] and returns v0=254 **without calling any handler**. Clean C
(`game/cd.c:5892-5899`) sends b70==254 down the generic
`rec[24]`-fn/`0x801a2560` path and does `rec[70] += r`. Fix location:
`game/cd.c` inside `cd_cmd_state_rx` dispatch — add
`if (b70 == 254) return 254;`-equivalent (generated also rewrites
rec[70]=254, a no-op store) before the `b70<2`/fn tail. Not the wedge
cause (rec[70]=1 live) but a real mistranslation.

### cd_cmd_state_commit (`game/cd.c:5779-5805`) vs `br2eu_func_801A3D84`
(`generated/SLES_017.22_full_50.c:6245-7244`)

Verified: `cmd==0xf3 && rec[232]!=0 && (rec[70]==0xff||rec[73]==2)` →
jalr `*(0x801BE784)`; b70==1→`cd_cmd_rec_init(rec,1)`; b70==254→
`cd_cmd_rec_init(rec,0)` (generated `0x801A3E18 beq rec70,254`→3E44);
b70==255→return.

**DIVERGENCE B — b70==0 must still run the fn/tag path.** Generated:
after the 254/255 compares fail, **b70==0 falls through to `0x801A3E58`**:
`fn=*(rec+20)` nonzero → `jalr fn(rec,a1)`; else `jal 0x801A24DC`
(op_tag_dispatch); all exits `v0=0` except the 255 path which returns
`v0=255` (li at `0x801A3E1C` survives to epilogue). Clean C
(`game/cd.c:5797`) does `if (b70==0 || b70==255) return 0;` — it skips
 BOTH the fn call for b70==0 AND returns 0 where generated returns 255.
For the live wedge rec[20]=0 so the missing call would be
op_tag_dispatch(rec70=0) = no-op anyway; still a mistranslation.
Fix location: `game/cd.c:5797` — b70==0 must NOT early-return; only
b70==255 returns (and should return 255, not 0, if callers consume v0).

### Not yet compared line-by-line
`cd_cmd_rec_scan` (`game/cd.c:5915`, gen `br2eu_func_801A3A78` @
full_50.c:3621), `cd_cmd_queue_drain` (`game/cd.c:6047`, gen
`br2eu_func_801A38B8` @ full_50.c:2186), `cd_cmd_queue_service`
(`game/cd.c:5996`, gen `br2eu_func_801A189C` @ full_49.c:16503),
`cd_cmd_data_byte`/`cd_cmd_rec_clear`/`qbyte_take`/`dev_cmd_*` (save.c).
Given two confirmed mistranslations in this region, these are the next
static targets.

## Wedge mechanics (what the dumps prove)

- `state_rx` runs per frame per record but the record never leaves
  `rec[70]=1`. With `*cmdptr=0x00` the nibble-0 early path returns
  `cd_cmd_rec_clear(rec,a1)` — which would zero rec[73]/rec[70] — yet the
  snapshot persistently shows rec[70]=1, rec[73]=1 and datptr[0]=0x00
  (early path would leave 0xff). Most consistent reading: each frame the
  record is cleared/re-armed by the queue+response path before the next
  pump — a stable limbo, not a stuck single call.
- `state_rx` faithfully returns `b70-2 = -1` for (b70=1, rec[55]=0) —
  verified identical in generated (`0x801A3FFC`→`0x801A4004`→ret a0-2).
  rec[55]=0 means no class tag (rec_init/op_tag_* never stamped it), so
  the record is armed-but-untagged: it waits for a response/commit that
  never arrives.
- The completion event `DeliverEvent(0xF0000009,0x20)` can only come from
  `cd_dma_chan_reset` `0x8019B7E0`, reachable via (a) cb7_run DICR-done
  walk — `dicr` done bits are 0, legitimately idle — or (b) the record
  continuation, which requires the record machine to reach a commit/
  completion transition. `state_commit` never fires ⇒ the trigger that
  would advance rec[70] past 1 never runs.

## Root-cause classification

**Lean: clean-C mistranslation is present and real (two confirmed), but the
proximate wedge is upstream of them** — the record never receives the
commit/advance trigger. The record machine's *observable* per-frame
behavior (state_rx returning -1 at b70=1/rec55=0) is faithful to
generated, so the stall is most likely **(3) runtime CD-status/response
semantics or a missing commit trigger in the delegated/generated response
path** (`cd_response_process` `0x80196370`, `cd_cb8_dispatch` chain
`0x801A0110`→job pump, queue service), NOT the two clean-C bugs found —
unless one of the un-compared functions (`rec_scan`, `queue_drain`,
`queue_service`, `data_byte`, `qbyte_take`, `dev_cmd_seq/round_dispatch`)
holds the actual mistranslation that prevents rec[55] from being stamped.

The intermittency (5900 passed the identical sequence once) fits a timing
race in whichever layer drops the commit trigger.

## Recommended next steps (diagnosis only — no fixes applied)

1. Finish the static diff for `cd_cmd_rec_scan`/`cd_cmd_queue_drain`/
   `cd_cmd_queue_service`/`dev_cmd_seq`/`dev_cmd_round_dispatch` vs
   generated — highest remaining mistranslation probability.
2. Fix the two confirmed divergences (parent decision):
   - `game/cd.c:5892-5899` add b70==254 early-return-254 case in
     `cd_cmd_state_rx`.
   - `game/cd.c:5797` `cd_cmd_state_commit`: b70==0 must fall through to
     the fn/`op_tag_dispatch` path; b70==255 returns 255 not 0.
3. Runtime: find who is SUPPOSED to call `cd_cmd_state_commit`
   `0x801A3D84` (it is only reached via fn-ptr tables — search generated
   for stores of `0x801A3D84` into RAM, e.g. the BE7xx table region
   `0x801BE780-0x801BE800` and record fn slots) and why it never fires
   after the Pause IRQ. Compare `cdrom_trace_dump` tail + `irq_state` +
   `bioscall_dump` DeliverEvent histogram on a fresh wedge vs a passing
   run.
4. Consider delegating `cd_cmd_state_commit`/`cd_cmd_rec_scan`/
   `cd_cmd_queue_drain` to generated (`br2_call_generated`) per AGENTS §4
   until the record machine is proven faithful — they are on the
   completion path and currently carry two confirmed mistranslations.

## Evidence index

- Live dumps: `mem_words 0x801FF080/0x801FF170/0x801BE784/0x801BE7A0/
  0x801BDD C0/0x801BDDE0/0x801FEBE0/0x801FEF60/0x801BE768` on 6020 & 5800;
  `read_ram` datptr/cmdptr contents; `fntrace_arm`+`fntrace_dump`
  (target_lo/target_hi filters) on 14 addresses; `bioscall_dump` tail;
  `cdrom_state`; `frame`.
- Generated (read-only): `SLES_017.22_full_50.c:7256-9194` (state_rx),
  `:6245-7244` (state_commit), `:3621` (rec_scan), `:2186` (queue_drain);
  `SLES_017.22_full_49.c:19563` (thunk 0x801A1C90), `:39618` (wrapper
  0x801A352C), `:39849` (0x801A3570), `:16503` (queue_service),
  `:5232`/`:5564` (op_chan_fn_invoke/job_counter_sign);
  `SLES_017.22_full_48.c:37206` (chan_job_pump 0x801A0110);
  `SLES_017.22_full_46.c:31795` (cb8_dispatch), `:27613` (intc_dispatch).
- Clean (read-only): `game/cd.c:5595-5618` (chan_job_pump),
  `:5719-6077` (record machine), `:5103-5228` (cd_dma_chan_reset/cb7_run);
  `game/save.c:8374-8387` (op_chan_fn_invoke), `:8947-8960`
  (op_tag_dispatch), `:9232-9257` (tag setters).
- Helper: `/tmp/dbg.py` (JSON-lines debug client).

No source files were modified. Prior context: `proof/cd_seek_blocker.md`.

---

## CORRECTION (verified 2025-11-21, commit 2894c11 era)

**DIVERGENCE B above is WRONG — do NOT apply it.** A careful block-by-block
decode of `br2eu_func_801A3D84` (generated/SLES_017.22_full_50.c:6245-7244)
shows the b70 dispatch is:

- `3DF0`: `lb v1,70(s0)`; `li v0,1`; `beq b70,1`→`3E30` (rec_init a1=1);
  delay-slot `slti v0,b70,2` → gpr[2]=(b70<2)
- `3E00`: `beq (b70<2),0` i.e: `b70>=2`→`3E18`; `b70<2` (==0, since !=1)→`3E08`
- `3E08` (**b70==0**): `beq b70,0`→`3E84` epilogue, `v0=0` ⇒ **return 0**
- `3E18` (b70>=2): `beq b70,254`→`3E44`; else `3E20`
- `3E20`: `beq b70,255`→`3E84` ⇒ **return 0**; else `3E28`→`3E58` fn path
- `3E58`: `lw v0,20(s0)`; `beqz`→`3E78` jal `0x801A24DC`(op_tag_dispatch);
  else `3E68` jalr rec[20](rec,live a1)

So **b70==0 returns 0** — it does NOT fall through to the fn path. The clean
`if (b70==0||b70==255) return 0;` at game/cd.c is CORRECT and faithful.
Commit 2975467 applied the wrong "b70==0→fn path" reading and caused a
deterministic early-boot `unknown dispatch 0x294C` crash (rec[20] is not a
valid callable pointer on that path); it was reverted in a729431. The
slti(b70,2) delay-slot split is what makes b70==0 a return, not a fallthrough.

---

## fntrace wedge localization (6091, commit 2894c11+)

Armed fntrace over the CD record machine at the gs=8 ph=3 wedge. Per-frame
counts over a 6s window (frames ~35152-35153):

- `0x8019AE68` cd_cb8_dispatch  x800   (the per-frame CD pump, runs)
- `0x801A38B8` cd_cmd_queue_drain x1600 (selector a0=0 -> BE7D8[head]=0 path;
  op_frame_advance/qbyte_take NOT reached)
- `0x801A3E94` cd_cmd_state_rx   x1600 (a0=0x801FF080/0x801FF170 records,
  ra=0x801A3550 — runs every drain pass)

**Never observed:** `0x801A3D84` state_commit, `0x801A189C` queue_service,
`0x801A1B50` dev_cmd_seq, `0x801A1E84` dev_cmd_round, `0x801A40F8`
op_frame_advance.

Conclusion: the record machine IS invoked correctly each frame; state_rx
re-processes the same armed records but never satisfies the commit
transition. seq (cdrom_state) frozen => the CD DEVICE command is not
completing, so the record response buffer never receives fresh data and
the record waits forever. The mistranslation is therefore UPSTREAM of the
record machine — in the device command issue / DMA fill / IRQ delivery
path (the cd_dma_chan_reset->DeliverEvent gap in proof/cd_seek_blocker.md),
NOT in state_rx/state_commit/queue_drain (all verified faithful vs
generated this session).

---

## WEDGE = DeliverEvent(0xF0000009) starvation — REGRESSION since f019a17

Live diagnosis on 6091 (arcade, HEAD=2894c11+): deterministic stall at
gs=8 ph=3, frames advance, cdrom seq frozen at 81861.

- bioscall_dump: B0/0x0B TestEvent = 1,462,829 (the spin) — game waits on
  event 0xF0000009 spec 0x20 (EvCB 0xF1000008), the CD/SPU-done event.
- fntrace: cb8_dispatch->queue_drain(sel=0)->state_rx run EVERY frame on
  records 0x801FF080/0x801FF170; cd_intc_dispatch (0x8019a8ec, generated)
  runs x500 as the cb8 POLL path. cb7_run (0x8019af84) and
  cd_dma_chan_reset (0x8019b7e8 -> DeliverEvent) NEVER fire.
- irq_state: i_stat=0, i_mask=0, pending=0 — no INTC line pending.
- cdrom_command_history: LAST device command exec at frame 3885 (cmd 0x09);
  nothing since — device did its work, completion event was LOST.

The game's deferred CD coroutine (cd_chan_full_init -> cd_ctx_save jmp_buf
-> on-IRQ cd_ctx_restore -> 0x8019A87C -> cd_intc_dispatch -> DeliverEvent)
never gets resumed, so DeliverEvent(0xF0000009) never fires and TestEvent
spins. This is the f019a17 HookEntryInt bug class — f019a17 IS in HEAD and
psxrecomp/bios/openbios.bin (sha256 d35c5f10, includes f019a17+c74f4db+
84ca34a) shows NOP'd returnFromException prologues at 0xBFC1E9E8/EA24.

=> The wedge is a REGRESSION (or uncovered path) introduced between
f019a17 (verified gs=8/ph=8) and HEAD. Suspect commits touching the
CD/event/coroutine path: aeed967 (11 CD record/queue ports), e9fc366,
c784bf1, 13576ce, 2acfce8, 50376cc/d2dcecc (handler-table base), plus
2894c11/a729431 cd.c+save.c record-machine edits. Next: bisect or fntrace
the IRQ jmp_buf resume (0x8019A87C) + DeliverEvent call sites to find
which commit broke the resume->DeliverEvent->TestEvent-unblock chain.

### IRQ-resume link verified — break is at DMA-done dispatch

fntrace round-2 (armed resume/handler PCs):
- 0x8019A87C jmpbuf_resume  x572  — IRQ coroutine resume RUNS every cycle
  (f019a17 HookEntryInt fix is effective; the deferred coroutine IS resumed)
- 0x8019A8EC intc_dispatch  x572  — reached from the resume path
- 0x8019AF84 cb7_run, 0x8019B7E0 spu_done_hdlr, 0x8019B7E8 dma_chan_reset
  = 0 hits — the DMA-channel-done walker + DeliverEvent NEVER fire.

So the resume->intc_dispatch link works; intc_dispatch simply finds NO
DMA-done interrupt pending (irq_state i_stat=0/pending=0, DICR done bits
clear) so cb7_run is never invoked and DeliverEvent(0xF0000009) never
fires. The break is UPSTREAM of the BIOS fix: the CD/SPU DMA channel is
not signaling completion (ch4 SPU active=0 / no done bit set in DICR), so
there is nothing for intc_dispatch->cb7_run to dispatch. Next: inspect the
DMA channel done-bit path — why the completed transfer at frame 3885 did
not leave a pending DICR done bit / raise the channel IRQ that cb7_run
consumes (psxrecomp DMA ch3/ch4 completion -> INTC/DICR).

### CD hardware proven CLEAN — wedge is downstream in game event/SPU layer

cdrom_state on wedged 6091:
- irq_enable=0x07 (all reasons enabled), irq_flag=0x00 (HINTSTS fully acked)
- int1_pended=0, int1_lost=0, int1_pending_now=0 — NO data-ready IRQ lost
- reading=0, pending={cmd:0x09,active:0,phase:1} — no active CD transfer
- sector_read_pos=2060/2340 — a sector buffer partially consumed via PIO
- seq frozen at 81861; last device cmd exec frame 3885

=> Every CD interrupt WAS generated, presented (i_stat bit2), delivered to
the game ISR, and acked (irq_flag=0). The "CD IRQ never fires" hypothesis is
DISPROVEN. The CD subsystem is idle with all work acked.

The wedge is therefore NOT a missing CD IRQ. It is that the game's
DeliverEvent(0xF0000009,0x20) -> TestEvent unblock never happens even though
CD data+IRQ completed. Per ACCURACY_BURNDOWN the event couples CD-read to
SPU-stream drain; DICR showed only ch4(SPU) IRQ enabled, no done bits. The
remaining suspect is the SPU/audio DMA channel (ch4) completion ->
cd_dma_chan_reset -> DeliverEvent path, OR the game's record/event state
machine observing a completion flag the clean-C port sets wrong.

### NOT a regression — longstanding wedge across binary vintages

Multi-instance sweep (all ./build/BloodyRoar2_Recompiled, various launch
times = different loaded images):
- 6020: gs=8 ph=3 frame=618788 seq=68207 (wedged, huge frame count)
- 6060: gs=8 ph=3 frame=406047 seq=42303 (wedged)
- 6091: gs=8 ph=3 frame=108035 seq=81861 (wedged)
- 6090: gs=8 ph=6 frame=161727 (VERSUS reached char-select ph=6)

=> The gs=8 ph=3 DeliverEvent wedge reproduces on OLD and NEW binaries —
it is a LONG-STANDING runtime/game bug, NOT a regression from the recent
game/*.c clean-C commits (2894c11, a729431, 3f35069). f019a17's verified
ph=8 was an intermittent pass. Recent clean-C CD/record work is exonerated
as the cause; the bug is the deep event/completion path the two running
agents are now hunting (runtime DMA/SPU done-IRQ vs game record/event
commit condition).

### PRECISE BREAKPOINT: ch4/SPU DMA never completes -> no DICR done-bit28

Handler table @0x801BDDF4 (indexed by DICR done-bit position = HW ch):
- ch0..ch3,ch5,ch6 = 0 (ch3/CDROM NULL, ch6 OTC NULL)
- ch4 @0x801BDE04 = 0x8019B7E0 -> cd_dma_chan_reset -> DeliverEvent
- callback ptr 0x801bdee8 = 0 -> DeliverEvent path (not callback)

DICR=0x00900000: IME4 (ch4 IRQ enable, bit20) SET, master-enable bit23 SET,
but done-bits24-30 ALL CLEAR -> ch4 done-bit28 never latched.

=> The ONLY path to DeliverEvent(0xF0000009,0x20) is ch4 DMA completion.
ch4 (SPU) CHCR=0x201 (start+chopping) but active=0 and BCR=0x0F930010 —
the transfer never runs to completion in the sim, so complete_transfer()
never latches DICR bit28 -> IRQ3 never fires -> intc_dispatch->cb7_run
never dispatches -> DeliverEvent never fires -> TestEvent spins.

ROOT CAUSE CANDIDATE: psxrecomp/runtime/src/dma.c ch4/SPU channel — a
started CHCR whose transfer does not execute/complete (direction, BCR
count decode, or SPU-side handshake stalls it), leaving CHCR.start set,
active=0, no done bit. This is the unified wedge for arcade ph=3 + versus.

### Command history + DMA trace refine the picture

cdrom_command_history (last entries):
- seq227 frame3871: cmd 0x09 Pause
- seq228 frame3874: cmd 0x02 SetLoc [1,58,70]
- seq229 frame3874: cmd 0x06 ReadN reading=1 irq_flag=0x03
- seq230 frame3885: cmd 0x09 Pause  <- LAST command; then SILENCE ~100k frames

dma_trace_dump: ch2(GPU/OT) very active (233k starts/completes = normal
rendering); ch4(SPU) has ZERO recent kicks; ch3(CDROM) idle CHCR=0.
No DMA done-bits pending on any channel.

=> The game completed a ReadN sector load then a Pause, delivered+acked all
CD IRQs, and STOPPED issuing CD commands. It is NOT waiting on a ch4 kick
(it never made one recently) nor on more CD commands. The DeliverEvent
(0xF0000009) that should unblock TestEvent is simply never produced by
whatever completion the game believes is outstanding. Open question for
the agents: which guest path is SUPPOSED to call DeliverEvent here —
cb7_run/ch4 (not kicked), the CD ISR channel-dispatch inside
cd_intc_dispatch, or a game coroutine that polls a completion flag the
clean-C record machine sets wrong (state_rx runs, state_commit never).

### sp_ring: the exact wedge loop (kernel-thread coroutine)

sp_ring tail shows a repeating per-frame cycle on tcb=0x8FE0:
  cd_timer_read region (0x8019A53C, fn 0x8019a460)
    -> kernel stub 0x00002924 (ra 0x0000282C)   [kernel wait/sleep]
    -> 0x8019A87C jmpbuf_resume -> cd_intc_dispatch
    -> repeat every frame

cd_timer_read (0x8019a460, generated) dereferences CD-state pointers
(0x801C0000 table -> struct -> MMIO reads) each iteration — the deferred
CD coroutine is a KERNEL THREAD polling CD timer/status, then kernel-wait,
resumed by IRQ jmp_buf each frame. It never observes the completion
condition, so it never reaches the DeliverEvent(0xF0000009) that unblocks
TestEvent. The missing piece is WHAT flag/register cd_timer_read's caller
checks to decide "done" — that is the completion the runtime/game fails to
produce. Agents d5cfe31a (runtime DMA/SPU done) + 74ecd030 (game record/
event path) are tracing this.

### Versus also wedged — ONE unified bug across both modes

6090 (versus): gs=8 ph=6 frame=230567 seq=147953 FROZEN, DICR=0x00900000,
i_stat=0x1 — identical signature to arcade ph=3 wedge. Versus progressed
further (ph=6 char-select) only because it loaded different resources
before hitting the same DeliverEvent(0xF0000009) starvation. Confirms a
single root cause gates ALL CD-completion-dependent progression.

---

## CONVERGED ROOT CAUSE (agents d5cfe31a + 74ecd030) — game-side, not runtime

Runtime agent (d5cfe31a): NO runtime IRQ/DMA bug. complete_transfer,
raise_dma_irq_on_master_edge, psx_irq_raise, I_STAT ack all correct.
CDROM IRQ delivers+acks; ch3 CD-DMA completes but game doesn't enable its
IRQ (faithful — game uses CDROM IRQ). ch4 SPU kick (cd_dma_xfer_start_b)
never happens because its producer never runs. cd_reg_window_op(0x8019B944)
+ cd_dma_chan_reset(0x8019B7E0) + cb7_run(0x8019AF84) NEVER entered.

Versus agent (74ecd030): controllers+char-lock RULED OUT (both pads work,
phase words 0x300 locked). Stall = versus handshake select_screen_tick needs
the deferred task/interrupt path starved by the same CD wedge. Points to
cd_seek_param_issue->cd_reg_window_op->worker doorbell (AGENTS.md §8).

SYNTHESIS: After the last completed CD command (Pause), the game's
deferred command-issue / record-commit path never produces the NEXT op or
the DeliverEvent(0xF0000009). fntrace: queue_drain+state_rx run; queue_service
+state_commit+op_frame_advance+reg_window_op+dma_chan_reset+cb7_run never.
The break is the clean-C record-machine commit/issue GATING — a mistranslated
compare/condition or a delegated wrapper dropping a side-effect. Fresh agent
cadad74f tracing the exact flush->state_commit->DeliverEvent trigger.

---

## CORRECTION (agent cadad74f): record machine is HEALTHY, not stuck

fntrace "state_commit/queue_service never fire" was a LOGGING ARTIFACT —
call_guest/clean-table paths don't emit fntrace entries; only
br2_call_generated callees are logged. Live fntrace (correct targets)
shows the FULL round-robin every frame per record: ctx_fn_send, flush_a,
flush_b, issue_poll, dev_cmd_process -> state_rx -> queue_drain. The two
records (0x801FF080/0x801FF170) are in a legitimate TERMINAL state:
rec[70]=0xFF, rec[73]=2, rec[232]=4 -> state_rx ret 255, state_commit ret 0
(verified faithful vs generated full_50.c:6245+). op_frame_advance
starvation is consistent with idle queue, not a mistranslation.

REAL remaining gap: DeliverEvent(0xF0000009,0x20) has ONE emitter —
cd_dma_chan_reset via cb7_run walking DICR done-bits -> cb7 table @0x801BDDF4
which has a handler ONLY at index4 (ch4/SPU -> 0x8019B7E0). All done bits
are 0 (DICR=0x00900000). So the awaited event needs a PSX DMA ch4 done
that never arrives. ch4 regs (MADR=0x4F6E0 BCR=0x0F930010 CHCR=0x201,
bit24=0) look like stale residue of an old RAM->SPU sound upload OR an
un-kicked transfer. The CD data path uses the CD's INTERNAL engine
(cd_dma_xfer_start_b reg-window strobe), NOT PSX DMA ch3.

OPEN: which guest path should kick the awaited ch4/SPU transfer (likely
CD-audio/XA streaming for versus/attract) whose done delivers the event?
Agent 5da22cd8 tracing the ch4 kick producer + whether versus/arcade
enables CD-audio streaming via SPU ch4.

---

## IDENTIFIED: stall is the CD-XA AUDIO stream, not file-load

Live decode on wedged arcade 6091 (frame ~350k):
- cdrom_state: last_sector lba=8801 mode=0xA0 (Mode2-Form2 = XA) size=2340,
  sector_read_pos=2060/2340 (mid-sector), reading=0, pending.cmd=0x09
  (Pause, completed), ring_partial=1824, bfrd_set=3931.
- spu_status: active_mask=0 (NO voices), cd_l=cd_r=0, cd_frames=0,
  cd_push_frames=0, nonzero_frames=7053/318M (only early boot SFX),
  last_peak=0 -> SPU/CD-audio path fully starved, silent now.
- dma_state: ch4 (SPU) madr=0x4F6E0 bcr=0x0F930010 chcr=0x201 bit24=0
  (configured, NOT started); DICR=0x00900000 no done bits; DPCR all-on.
- Pending-args cells 0x801fef70-8c = event flags 0xF1000000..0xF1000007
  (8 per-channel stream flags) + 0xF1000008 (=0xF0000009 CD flag @0x801bde38).
  cd_pending_args_pump/queue_a/b_status call cd_seek_state_machine->
  TestEvent(B0:0x0B) on each -> the 1.46M TestEvent count is the per-frame
  CD-stream service poll (NORMAL), not the block.

CONCLUSION: game streams XA music CD->RAM->ch4DMA->SPU. Stream stalled at
sector 2060/2340; SPU starved; ch4 never kicked/completed -> no
DeliverEvent(0xF0000009) -> stream controller never issues next read.
The block is the game's audio-stream kick/completion path (likely
sound-driver, possibly still generated/), NOT the file-load record machine
(which is terminal/healthy) and NOT the runtime (agent d5cfe31a clean).

---

## CORRECTION: stall is an FMV (MDEC video), not XA audio

fmv_state/mdec_state on wedged 6091:
- mdec_decode_count=34257, decode_macroblocks=17103, dma_out_words=2892483
- output_size=12394752, output_pos=10829580 (~87% decoded)
- busy=0 (MDEC idle), decode_input_pos=3796/3840 (input consumed)
- xa_stream_active=0, cd_dataready_fires=2410, cd_irq_delivered=2708
- last_sector lba=8801 mode=0xA0 size=2340, sector_read_pos=2060/2340

So the game is mid-FMV (vs-intro/attract movie): CD streams video sectors
-> MDEC decodes -> framebuffer. Stream stalled at sector 2060/2340; MDEC
starved for input (busy=0, input consumed); CD issued Pause and no more
reads for ~350k frames. The CD->MDEC stream controller stopped issuing
reads — same DeliverEvent(0xF0000009)/channel-flag family, consumer is
MDEC not SPU.

=== SESSION — dormant DeliverEvent producer chain + channel-table stall ===

fntrace on wedged :6091 (attract FMV, TestEvent spin on 0xF0000009):
- ZERO hits on the ENTIRE audio/DeliverEvent producer chain:
  dialog_msg_dispatch 0x8015caa8, save_flow_state_machine 0x8015d420,
  audio_seq_reset 0x80158244, chan_reset_register 0x801582c0,
  cd_dma_xfer_start 0x8019b89c — all dormant (matches burndown N+42).
- Only cd_cb8_dispatch / cd_intc_dispatch / record-machine fns cycle.

menu.c:1054 — dialog_msg_dispatch (0x8015caa8) is stored as the intro-movie
STEP CALLBACK (local_54 -> step_desc+0xc) invoked by menu_stepper_state_advance
as a guest code addr. It never runs => the stepper/movie SM never reaches the
step that arms audio init => ch4 never kicked => no DeliverEvent.

SEPARATE active stall: movie/file stream in blk_io_retry -> mc_date_serial_cmp
(delegated 0x80159ab4) -> cd_channel_poll polls channel-table entry for
state==2. Live: base@0x801fc6c0=0x80010000, idx@0x801fc6ac=0x1e,
entry@0x800103c0 state word = 0x0000050A (never 2) => poll returns 1 forever.

UNIFIED: both consumers starve because CD read-completion bookkeeping does not
advance the game's higher-level state (channel-table state==2 AND ch4/audio
DeliverEvent). ch3 sector DMA completes 1064x (low-level works); the missing
link is the completion->game-state transition.

NEXT: trace WHY channel-table entries never reach state==2 — which generated/
clean fn writes entry state on CD read completion (record machine state_commit
-> channel mark), and why it doesn't fire for this read.

=== SESSION — ROOT CAUSE: CD consumer task starved (data-ready backlog) ===

Agent a967ee56 proved: channel-table state==2 writer = cd_dir_entry_finalize
(0x801990f0, clean cd.c:3553), gated by 0x801FC69C which needs
entry+4(progress)==entry+6(expected)-1. entry+4 gets ZERO writes — the
per-sector accounting consumer is dead. fntrace unreliable for coroutine
re-entry (continuation blocks), but live IRQ/cdrom state is decisive:

LIVE on wedged :6091 (cdrom_state):
- sector_available=1  BUT sector_read_pos=2060/2340, reading=0
  => CD delivered a sector; NOTHING is consuming it.
- bfrd_set=3931 vs bfrd_clear=14  => data-ready signaled ~4000x, acked 14x
  => massive unconsumed backlog.
- int1_pending_now=0, irq_flag=0, irq_enable=0x07
  => drive already signaled + is waiting for the game to PULL; no IRQ pending.
- pending cmd Pause(0x09) phase=1 = stale residue (completed earlier).
- fntrace cycles ONLY cb8_dispatch/intc_dispatch/record-machine; the
  response_pump->state_machine->finalize + audio/DeliverEvent chain absent.

ROOT CAUSE (high confidence): the game's CD stream-CONSUMER task — the one
that reads the available sector, credits channel-entry +4 progress, marks
state==2, and kicks ch4/audio->DeliverEvent — is NEVER SCHEDULED. This is the
N+42 cooperative-scheduler starvation: a TestEvent-spinning thread holds the
CPU so the producer/consumer task never runs. Data piles up unconsumed
(bfrd 3931:14), channel entry never reaches state==2, DeliverEvent never fires.

NEXT (faithful): find WHY the consumer task isn't scheduled — is it (a) not in
the ready queue (never woken / lost wakeup), (b) starved by a non-yielding
TestEvent loop that should yield/sleep, or (c) blocked on a different event.
Inspect the kernel TCB/ready-queue state + which thread owns the CD consumer.
