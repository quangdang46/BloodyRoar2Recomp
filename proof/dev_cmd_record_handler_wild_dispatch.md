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

## Refined suspect ranking

1. **TCB/IRQ context-restore committing a wrong `gpr[]`** (sp→0x801FFxxx or
   the 0x8548 anomaly) — would explain BOTH the stack-into-record spills AND
   the corrupted crash-time s0/s1/sp. Highest suspicion given the register
   evidence.
2. Thread stack allocator handing the CD-command thread a base that overlaps
   the static `0x801FF080` table.
3. Faithful game layout where timing prevents the overlap on HW (least likely
   given the deterministic-looking crash window).
