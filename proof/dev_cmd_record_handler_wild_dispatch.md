# dev_cmd record-handler wild dispatch — 0x801BCB94/98/A0

Crash signature (reproduced on multiple builds/instances):

```
FAIL-FAST unknown dispatch: addr=0x801BCB98 phys=0x001BCB98
    ra=0x801A1EC0 a0=0x801FF170 a1=0x6D38A510  frame≈26552 (varies 26k-62k)
```

Targets vary across `0x801BCB94 / 0x801BCB98 / 0x801BCBA0` — consecutive word
addresses inside the **CD read-session descriptor block** `0x801BCB80–0x801BCBB0`
(`DAT_801bcb80`=buf, `+8`=mode, `+0x10`=sector size, `+0x28/+0x2c`=session ptrs;
populated per-read by `cd_read_setup`/`cd_cmd_issue`, `game/cd.c:256-360`).
These are **data cells, not code**.

## Ruled out (runtime-verified, not guessed)

- **NOT** the `dev_cmd_round_dispatch` round-robin table. `drr_ring`
  (`game/save.c` per-call `{idx,table_addr,fn}`) + `devcmd_rr` freeze peek show
  all 5 slots valid (`0x801A30C0/3108/31E0/328C/3350`), counter cycling 0-4.
- **NOT** exception/IRQ context corruption. `ctxchain.epcval=0x8019A648` (valid
  code), `intable=1`, `ctx0=0x4000` runnable — TCB+0x88 resume-PC slot clean.
- **NOT** a corrupted driver callback cell. Fatal-moment `devcmd_rr_a/b/c` peek
  of `0x801BE780–0x801BE830` shows every cell = valid `0x801A3xxx` fn ptr
  (`0x801BE798 = 0x801A3D84 = cd_cmd_state_commit`), handler table clean.

## Confirmed dispatch path

`ra=0x801A1EC0` is the `jalr fn` continuation inside `dev_cmd_round_dispatch`
(`0x801A1E84`, `game/save.c`). It `br2_call_generated(fn=table[idx], ctx=record)`
with `ctx = 0x801FF080 + head*240`; `a0=0x801FF170` = **record slot 1**.

Inside that handler call, `cpu->gpr[31]` stays `0x801A1EC0`. A deeper
`call_guest`/`br2_call_generated` that does **not** install its own ra inherits
it — matching the crash `ra` exactly.

The handler cell `0x801BE798` → `cd_cmd_state_commit` (`0x801A3D84`,
`game/cd.c:5846`). Verified insn-for-insn vs generated `func_801A3D84`:

```c
b70 = rec[70];                       // s0[70], byte offset 0x46
if (b70==1)   -> cd_cmd_rec_init(rec,1)
if (b70==254) -> cd_cmd_rec_init(rec,0)
if (b70==0||255) return 0;
/* others: */
fn = *(uint32_t *)(rec + 0x14);      // rec[20] per-record state handler
if (fn) call_guest(fn, rec, a1);     // <- jalr  *(rec+0x14)   CRASH SITE
else    call_guest(0x801a24dc, rec, a1);
```

`cd_cmd_state_rx` (`0x801A3E94`, `game/cd.c:~5890`) is the sibling:
`fn = *(rec + 0x18); call_guest(fn, rec, a1s)`.

So `*(rec+0x14)` held `0x801BCB98` = `&DAT_801bcb98` — a **CD session-data
pointer in a function-pointer slot**.

## Record handler-slot semantics (verified)

The 240-byte device-command records at `0x801FF080 + slot*240` carry two
per-record state callbacks:

- `+0x14` (rec[20]) — invoked by `cd_cmd_state_commit` "others" path
- `+0x18` (rec[24]) — invoked by `cd_cmd_state_rx` ">=2" path

Submitters assign **real function addresses**:
- `cd_ctx_cmd_submit` (`game/cd.c:3765`): `+0x14=0x801a2de4`,
  `+0x18=0x801a2e00`, `rec[70]=1`.
- `cd_cmd_rec_clear` (`game/cd.c:5781`): `+0x14=+0x18=0` on teardown.

`0x801BCB98` is neither a function nor the cleared `0` — it is a pointer into
the CD read-session block. Either the slot was overwritten post-submit, or a
submitter/record-build copied a session pointer into the handler field, or the
record reached `state_commit`'s "others" dispatch with `rec[70]` in a state the
real hardware path would not produce there.

## Decisive datum — `recvalring` + `rec_*` peeks (commit `ae268fb`, frame 31204)

`recvalring` (record-region store watch, `memory.c`) captured the writers that
populate the record with CD-session pointers. Newest tail at crash frame 31203:

```
seq  pc         gaddr      val        field
73957 0x80196E38 0x801FF184 0x801BCB8C  rec+0x14   <- the handler slot
73956 0x80196E20 0x801FF180 0x801BCB80  rec+0x10
73955 0x80196E28 0x801FF160 0x801BC7F8  (rec-0x10)
73954 0x80195D90 0x801FF17C 0x801BCB8C  rec+0x0C
73953 0x80198924 0x801FF1B8 0x801BCB80  rec+0x48
73959 0x8019A47C 0x801FF190 0x801BCB80  rec+0x20
73958 0x8019A478 0x801FF194 0x801BCB8C  rec+0x24
```

Writer PCs cluster in the **CD read/issue path**: `0x80196E*` (`cd_cmd_issue`
`0x80196e14`, delegated→generated), `0x8019A4*` (`cd_timer_read` region),
`0x80195D90` (`mc_session`), `0x80198924` (`cd_read_open`). NB the recorded `pc`
is `effective_store_pc()` = last `g_debug_last_store_pc`; several hits
(`0x80196E20/28/38`) are prologue `sw $sN,off($sp)` that store to the *stack*,
so the PC attribution is **stale/misattributed** — but `gaddr`/`val` are the
real store. The pattern is unambiguous: generated read-path code fills the
record's `+0x0C/+0x10/+0x14/+0x20/+0x24` with session-block pointers.

`rec_*` peeks of the full 240-byte record at `0x801FF170` confirm it is a
**CD read/data command record**:

- `+0x0C=0x801BCB8C`, `+0x10=0x801BCB80`, `+0x14=0x801BCB8C`, `+0x20=0x801BCB80`,
  `+0x24=0x801BCB8C` — **five** fields hold session-block pointers.
- `+0x18=0x0000B0C7` (not a pointer at all).
- Function pointers elsewhere: `+0x28=0x80198AD0`, `+0x2C=0x80198AC0`,
  `+0x44=0x80195E74`, `+0x54=0x80198E08`, `+0x9C=0x8018D0CC`, `+0xA0=0x8019A53C`,
  `+0xC8/+0xCC=0x80126664`, `+0xF8=0x80196954`.
- `+0x3C` (cmd ptr) = 0; `rec[70]` (byte `+0x46`) = `0x19`.

## Revised conclusion

`+0x14` is **not** an isolated corruption — it is one of several session-data
pointers the read-command builder legitimately stores. This record's handler
slots are *data*, and `cd_cmd_state_commit`'s "others" jalr treats `+0x14` as a
function because `rec[70]=0x19` routes there. On real hardware this record must
NOT reach that jalr — either `rec[70]` should be a terminal/in-flight value
that returns early, or the record should be claimed by `state_rx`/a read
handler rather than `state_commit`.

The `0x801BCB98` vs peeked `0x801BCB8C` delta is explained by `+0x14` being a
**live session sub-pointer rewritten every frame** as the read progresses;
`recvalring` retains only the last write per slot.

`cd_cmd_flush_a` (`game/cd.c:5716`) compounds it: `sub=*(rec+0x0C)=0x801BCB8C`
then `state_commit(sub)` reads `*(0x801BCB8C+0x14)=*(0x801BCBA0)` — directly
producing target `0x801BCBA0` when the session block is mistaken for a record.

## CORRECTED root cause — stack spills overwrite the record (frame ~28800/31204)

`recvalring` writer-PC disassembly proves the `0x801FF184`-region stores are
**NOT record-field writes — they are stack spills**. Every captured `pc` is a
prologue `sw $sN,off($sp)` (`write_word(gpr[29]+off, gpr[sN])`):

```
0x80196E38 = sw $s1,28($sp)  -> 0x801FF184   (sp = 0x801FF168)
0x80196E20 = sw $s0,24($sp)  -> 0x801FF180   (sp = 0x801FF168)
0x80196E28 = sw $s6,48($sp)  -> 0x801FF160   (sp = 0x801FF130)
0x8019A478 = sw $s1,28($sp)  -> 0x801FF194   (sp = 0x801FF178)
0x8019A47C = sw $s0,24($sp)  -> 0x801FF190   (sp = 0x801FF178)
0x80195D90 = sw $s1,20($sp)  -> 0x801FF17C   (sp = 0x801FF168)
0x80198924 = sw $s0,24($sp)  -> 0x801FF1B8   (sp = 0x801FF1A0)
```

All in the CD read/issue path (`cd_cmd_issue` `0x80196e14`, `cd_timer_read`
`0x8019a460`, `cd_read_open` `0x80198908`, `mc_session`). The spilled values
(`0x801BCB80/8C/A4`, `0x801BC7C8/7F8`) are **saved registers holding
session-block pointers** — the functions' own locals, not record data.

`0x801FFxxx` is the game's **dense top-of-RAM kernel stack/scratch region**:
the freeze dump shows dozens of distinct `sp` values from `0x801FF108` to
`0x801FFF30`, many *inside* the 480-byte record table (`0x801FF080–0x801FF260`,
`cd_bzero(0x801ff080,0x1e0)` = exactly 2 records). Thread stacks legitimately
coexist there — but a frame at `sp≈0x801FF168` writes `sp+20/24/28` =
`0x801FF17C/180/184` = **record slot1 `+0x0C/+0x10/+0x14`**.

## Mechanism (final)

1. A CD read command occupies record slot1 (`0x801FF170`); its fields are
   legitimately populated with session pointers by the generated read path.
2. CD-path functions run with `sp` inside the record-table region; prologue
   `sw $sN,off($sp)` spills saved-register values (session/function pointers)
   onto the record's `+0x0C/+0x10/+0x14/+0x20/+0x24` and the `+0x44` word that
   `rec[70]` (`+0x46`) reads as `0x19`.
3. `ctx_fn_send`→`cd_cmd_state_commit` reads `rec[70]=0x19` → "others" →
   `call_guest(*(rec+0x14))` where `+0x14` now holds a spilled session pointer
   (`0x801BCB8C/98`) → `unknown_dispatch`.

On real hardware the same code does not crash, so the guest `sp` for this
context must NOT reach into the record table — i.e. our runtime places a stack
frame lower than the game's real layout (faithful-core stack/context bug), or a
context/IRQ restore sets `gpr[29]` wrong. `g_guest_sp` default `0x801ff000`
(`game/bind.c:46`) sits right at the table base.

## Faithful fix direction (do NOT clamp 0x801BCBxx, do NOT patch state_commit)

Find why the CD-command-processing context's `gpr[29]` lands at
`~0x801FF168` (inside the record table) instead of a proper stack base. Suspects:
- thread/TCB stack allocation overlapping the game's static `0x801FF080` table
  (BIOS/kernel stack allocator or memory map),
- a context/IRQ restore committing a wrong `sp` (the earlier `ctxchain`/`badctx`
  machinery),
- a clean→generated boundary failing to preserve `gpr[29]`.

`recvalring` now records `sp` per write (`273d95d`) to name the exact stack
frame. Next: dump `sp` for the `0x801FF184` write and trace which thread/TCB
owns that stack vs the record-table owner.

## Crash-time register anomaly (frame-28800 freeze CPU state)

The frozen CPU state at the wedge shows a **corrupted register file**, not a
normal dispatch context:

```
gpr[4]  (a0)  = 0x801FF170   <- the record (correct)
gpr[29] (sp)  = 0x00008548   <- INVALID (not 0x801FFxxx, not a RAM stack)
gpr[16] (s0)  = 0x801A189C   <- cd_cmd_queue_service CODE addr in s0
gpr[17] (s1)  = 0x801A1834   <- CODE addr in s1
gpr[31] (ra)  = 0x801A1EC0   <- dev_cmd_round_dispatch return (correct)
epc          = 0x8019A648   <- cd_timer_read region
```

`s0`/`s1` holding code addresses and `sp` a tiny non-stack value are NOT what
a healthy `dev_cmd_round_dispatch`→`state_commit` frame looks like — this
smells like a **TCB context-restore loading the wrong register set** (BIOS
`0x29CC` ReturnFromException/ChangeThread restores `gpr[]` from the TCB save
area; `interrupts.c` already latches `tcb88`/`saved_gpr` for the pc=0 wedge
family). If the restored `sp`/`s0`/`s1` are wrong, the *next* generated
prologue spills land wherever the bad `sp` points — consistent with frames at
`sp≈0x801FF168` inside the record table.

Note `call_guest`/`call_guest4`/`br2_call_generated` (`src/clean_bridge.c`)
only save+restore `gpr[29]` around `psx_dispatch_call`; they do NOT fabricate a
frame, so a generated callee's prologue spills use whatever `gpr[29]` the last
context restore left live. `ctx_fn_send` (`game/save.c:7972`) is confirmed
**unconditional** — it always dispatches `cd_cmd_state_commit(rec)` for
whatever record the round-robin selects, no record-type gate.

## Live RAM dump — the record is a real CD-read record (frame ~46595)

`read_ram` of `0x801FF000–0x801FF400` on a live CD-reading instance shows the
region is a dense array of ~`0xF0`-stride device-command records interleaved
with task/coroutine context saves. Slot1 (`0x801FF170`, the crash record)
contains a **legitimate CD-read record**:

```
+0x0C=0x801BCB8C  +0x10=0x801BCB80  +0x14=0x801BCB8C  +0x20=0x801BCB80
+0x24=0x801BCB8C   (session-descriptor context pointers — consistent pattern)
+0x28=0x80198AD0 (cd_response_pump)  +0x2C=0x80198AC0 (cd_response_pump)
+0x44=0x80195E74 (cd_mode_flag_set jal-link)  -> rec[70]=byte2=0x19
+0x54/+0xC8/+0xF8 region: cd_read_issue / cd_file_load_sm / cd_timer_read /
                        gpu_state_patch_defer fn-ptrs
```

Slot0 (`0x801FF080`) has `rec[70]=0xFF` (gating → `state_commit` early-returns).
So `+0x14`=session-ptr and `rec[70]`=0x19 are **record-layout-inherent** for an
active read record — `+0x14` is the documented handler slot
(`cd_ctx_cmd_submit` sets it to a fn) but read records repurpose the low
context fields for session pointers. `state_commit` jalr's `+0x14` as code
when `rec[70]` isn't gating — faithful to generated `func_801A3D84`.

## sp_ring — the CD coroutine stacks live in the record region

`sp_ring` (64KB-domain transitions = genuine stack/context switches) shows the
CD-subsystem coroutines running on guest stacks that overlap the record table:

```
per-frame: 0x801FF398 (cd_timer_read resume 0x8019A53C)
          ↔ 0x801BDD30 (cd_chan_full_init resume 0x8019A87C)
          ↔ 0x00008548  (BIOS IRQ stack, target 0x2924)
CD-read burst (frame 46594): thread-B resumes at DEEP sp
          0x801FF130/180/1A0/1D8/1F0/1F8 via cd_result_wait_b (0x80197030),
          cd_init_session (0x80198C5C), cd_intc_busy_read (0x8019A7D4)
```

All `tcb=0x8FE0`. The crash-time `sp=0x8548`/`s0`/`s1` anomaly was the **BIOS
IRQ stack context**, not register corruption — `0x8548` is the exception stack.

## Resolved memory model (subagent be7180ce — decisive)

There is **no per-coroutine stack allocator** and nothing "placed the records
inside a stack". The map is:

- `0x801FFxxx` = the game's **single top-of-RAM main/kernel stack**
  (`stack_base=0x801FFFF0` from the PS-X EXE header, `game_1pad.toml:14`).
- `0x801FF080–0x801FF260` = the 2×`0xF0` device-command records; `0x801FF268+`
  = task_pool descriptor + bucket-head tables (`game/battle.c:11059/8271`).
  All are **static BSS ~3.4–4 KB below the stack top, directly in its growth
  path** — they cannot coexist with deep frames on real HW either.
- `0x801BDxxx` = the **CD deferred-coroutine stack** (`cd_ctx_save` snapshots
  `{ra,sp,fp,s0-s7,gp}` to jmp_buf `0x801BCD50`, arms BIOS `B0/0x19`; on CD IRQ
  `cd_ctx_restore` reloads `sp=0x801BDD30` → `cd_chan_full_init` →
  `cd_intc_dispatch` → handler tbl `@0x801BCD1C[2]=cd_response_pump`).
  Verified faithful (`proof/crash_defer_switch_ab.md`).
- `0x00008548` = BIOS exception/idle stack (`sp_ring` target `0x2924`).

So the spilling CD functions (`cd_response_pump`→`cd_cmd_issue`,
`cd_init_session`, `cd_read_open`, `mc_session`, `cd_timer_read`) running at
`sp=0x801FF130–0x801FF1C8` are on the **main/kernel stack, ~3.7 KB deep** —
far below the ~`0x801FF260` collision threshold. On HW the same BSS is there,
so these frames must either run shallower or on the `0x801BDxxx` coroutine
stack. Reaching the record table = **anomalously deep IRQ-side / nested CD
activations on the shared kernel stack**.

## Same bug family as crash_defer_switch_ab.md

`game_1pad.toml:57-63` already documents this class: *"the deferred
in-exception thread-switch honor corrupts the shared kernel-stack coroutine
frames — vsync_wait_n's saved `$ra` slot at `0x801FFE58` is overwritten by a
later IRQ-side `cd_timer_read` activation."* `defer_switch=false` fixed that
frame-~1600 saved-`$ra` clobber (immediate RFE-boundary honor survives 60k+
frames). The present crash at frame 46595 is the **deeper manifestation of the
same mechanism**: nested IRQ-side CD call chains now reach ~3.7 KB into the
record-table BSS instead of ~0.4 KB into a saved-`$ra` slot.

## Refined suspect ranking

1. **IRQ-side/nested CD activations nest too deep on the shared main/kernel
   stack** (`0x801FFxxx`), colliding with the record-table/task-table BSS.
   Faithful question: should these run on the `0x801BDxxx` coroutine stack, or
   is extra depth coming from our IRQ delivery (`interrupts.c` nested-exception
   / `PSX_DEFER_SWITCH` path) vs HW? Discriminator: dump `sp` distribution on
   a healthy non-CD-reading run — if it never drops below `0x801FF260`, the
   deep frames are CD-burst-specific.
2. Faithful game layout where HW timing prevents the overlap (records are
   static BSS; HW stack may simply never reach them while records are live).
3. TCB/IRQ context-restore committing a wrong `gpr[]` — unlikely (sp is a
   faithful guest value; `0x8548` is just the IRQ stack).

Next: determine whether the deep `0x801FF1xx` CD frames are IRQ-side
activations that should run on the `0x801BDxxx` coroutine stack (a skipped/
mistimed stack-switch in the kernel trampoline or BIOS IntRP path) or carry
extra nesting depth from our IRQ delivery vs HW.

## cd_chan_full_init resume contract (generated 0x8019A814)

`cd_chan_full_init` prologue `addiu sp,-24`, saves s0/ra, loads flag
`*(0x801CDDA8)`→v0 and branches (`0x8019A830`): the flag selects first-call
(`cd_ctx_save(0x801bcd50)` snapshot + `B0/0x19` jmp-buf arm + wait) vs resume
(`v0!=0` → `cd_intc_dispatch`). On CD IRQ the game's low-mem kernel trampoline
invokes `cd_ctx_restore` (`0x8019ADCC`), which reloads `{ra,sp,fp,s0-s7,gp}`
from the saved frame — `sp=0x801BDD30` — and re-enters `cd_chan_full_init` on
the **`0x801BDxxx` coroutine stack**. `cd_intc_dispatch`→handler tbl
`@0x801BCD1C[2]=cd_response_pump`→`cd_cmd_issue`/`cd_init_session`/
`cd_read_open`/`mc_session`/`cd_timer_read` should therefore ALL run at
`sp≈0x801BDxxx`.

The `recvalring` writers instead run at `sp=0x801FF130–0x801FF1C8` with
callers inside `cd_response_pump` (`ra=0x80198AC0/AD0/A3C/C5C/DDC`) — so for
this activation `cd_response_pump`/`cd_intc_dispatch` executed on the **main
kernel stack**, i.e. the `cd_ctx_restore`→`0x801BDxxx` stack-switch did NOT
engage. Two candidate faithful divergences:

- **(a) Resume not taken:** the BIOS `B0/0x19` exception-jmp-buf
  (`mem[0x4C18]`)→`cd_ctx_restore` path is not invoked for this CD IRQ, so the
  handler chain falls through on the interrupted main-stack context. Check
  `irq_state`/`bioscall_dump` for `B0/0x19` arm + the `0x4C18` cell, and
  whether `cd_ctx_restore` (`0x8019ADCC`) ever runs near frame 46594.
- **(b) Synchronous second path:** a non-coroutine caller reaches
  `cd_intc_dispatch`/`cd_response_pump` on the main stack (deep legit chain
  that HW never makes because the IRQ always defers first).

Beetle-oracle discriminator: `emu_trace_addr`/`emu_read_ram` on `cpu.gpr[29]`
+ `0x801BCD50` jmp-buf during the same CD-read window gives the HW sp and
whether `cd_ctx_restore` fires.

## Crash CPU state — confirms IRQ/exception context

`cpu.epc=0x8019A600` (inside `cd_timer_read`/`vsync_wait_n`), `cpu.cause=
0x400`, `cpu.sr=0x40000404`, `dispatch_tail` = all BIOS low-mem (`0x2B20`,
`0x3CB4`, `0x3E14`…) — the wild dispatch fired during heavy BIOS IRQ/exception
dispatch while `cd_timer_read` was the interrupted frame. The spilling CD
chain is IRQ-side, on the shared kernel stack.

Live jmp-buf check (9792 @frame 46595): `0x801BCD50` frame = `{ra=0x8019A87C,
sp=0x801BDD30, fp, s0=0x801BCD18}` — **the coroutine IS armed correctly**.
So `cd_ctx_save`/`B0/0x19` works; the failing activation is one where the
resume did not take the `0x801BDxxx` stack.

## Faithful-core lead — exception_jmpbuf edge

`interrupts.c` implements the BIOS exception-jmp-buf as a host
`setjmp`/`longjmp` (`exception_jmpbuf`, ~line 853) plus a known edge
`g_esc_decline_deadframe` (~line 119: "armed RFE but exception_jmpbuf frame
is dead"). If the CD IRQ's RFE/exception path hits a dead-or-mismatched
jmp_buf frame, `cd_ctx_restore` is skipped and the INTC handler chain
(`cd_intc_dispatch`→`cd_response_pump`→`cd_cmd_issue`…) runs on the
interrupted kernel-stack context → deep `0x801FF1xx` frames → record-table
collision. Next: trace `exception_jmpbuf` arm/longjmp + `g_esc_decline_deadframe`
around frame 46594, and whether `cd_ctx_restore` (`0x8019ADCC`) executes for
the crashing IRQ.

## exception_jmpbuf edge — RULED OUT (freeze_check on wedged 9792)

`freeze_check` @frame 46595: `esc_calls=78.5M`, `esc_fired=112470`,
`esc_dec_nopend=78.4M`, **`esc_dec_noexc=0`, `esc_dec_sentinel=0`,
`esc_dec_fiber=0`, `esc_dec_deadframe=0`**, `irqanom_count=0`,
`in_exception=1`, `exception_entries=112471`. The deadframe/fiber/sentinel
decline edges **never fired** — the host `exception_jmpbuf` + RFE honor is
healthy; hypothesis (a) is falsified. `cd_ctx_save`/`B0/0x19` arm is intact
(live jmp-buf `sp=0x801BDD30`).

So the spilling CD chain runs on the main/kernel stack with **faithful guest
`sp`** — the only remaining faithful-vs-bug axis is **stack depth / IRQ
timing vs hardware**: does HW's kernel stack reach `0x801FF130` during a CD
burst (it cannot, or records would corrupt there too), or does our runtime
nest deeper / preempt differently? Decisive instrument: **Beetle oracle**
`emu_read_ram`/`emu_trace_addr` on `cpu.gpr[29]` + `0x801BCD50` during the
same CD-read window, OR a `sp`-distribution histogram on a healthy
non-crashing CD run to see if `sp<0x801FF260` ever occurs without corruption.

## recvalring sp+ra — the writers are a deep CD call chain (frame 46594/46595)

Crash `addr=0x801BCBA0 ra=0x801A1EC0 a0=0x801FF170`. The ring now records
caller `ra`; decoding the spill entries:

```
pc=0x80196E38  gaddr=0x801FF184(=rec+0x14) val=0x801BCB8C sp=0x801FF168 ra=0x80198AC0
   -> cd_cmd_issue prologue, called FROM cd_response_pump (0x80198AC0)
pc=0x8019A478  gaddr=0x801FF194            val=0x801BCB8C sp=0x801FF178 ra=0x80198AD0
   -> cd_timer_read prologue, caller 0x80198AD0 (cd_response_pump region)
pc=0x80195D90  gaddr=0x801FF17C            val=0x801BCB8C sp=0x801FF168 ra=0x80198A3C
   -> mc_session prologue, caller 0x80198A3C
pc=0x80198924  gaddr=0x801FF1B8            val=0x801BCB80 sp=0x801FF1A0 ra=0x80198928
   -> cd_read_open prologue, caller 0x80198928
pc=0x8019A478  gaddr=0x801FF1E4            val=0x801BCBA4 sp=0x801FF1C8 ra=0x80198C5C (xN)
   -> cd_init_session prologue in a poll loop, caller 0x80198C5C
```

These are **ordinary generated `jal` call chains** (ra = caller function), not
context restores — `cd_response_pump → cd_cmd_issue`, `cd_init_session`,
`cd_read_open`, `mc_session`, `cd_timer_read` all nest deeply enough that `sp`
reaches `0x801FF130`–`0x801FF1C8`, inside the record table
(`0x801FF080`–`0x801FF260`). The spilled values are the functions' saved
session-block registers.

So the overlap is a **genuine deep call chain on the CD coroutine stack**,
not a register-restore fault. Whether `sp≈0x801FF130` is faithful depends on
the intended stack base/size for this coroutine — the subagent is tracing the
game's task-stack allocator. If HW runs the same chain to the same depth, the
record table would overlap there too, meaning either the table is placed to
avoid it or the timing differs.
