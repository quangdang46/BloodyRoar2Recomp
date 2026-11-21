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
