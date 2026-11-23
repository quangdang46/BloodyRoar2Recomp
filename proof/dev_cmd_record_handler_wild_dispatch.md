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

## Pending decisive datum

Fatal-path `rec_*` RAM peeks (`crash_trace.c`, commit `2fdd514`) dump the full
240-byte record at `a0` on the next crash: `+0x14/+0x18` handler values, `+0x0C`
sub-record ptr, `+0x3C` cmd ptr, `rec[70]`/`rec[73]` state bytes, `rec[232]`.
This names whether `+0x14` is isolated corruption vs a mis-populated field and
which command type/state triggered the dispatch.

## Faithful fix direction (do NOT clamp 0x801BCBxx)

Find the writer that places `&DAT_801bcb9x` into `rec+0x14`/`+0x18` (or the
state divergence that routes a data-pointer record into the handler jalr) and
fix the clean port / record-build faithfully. Candidates: a submitter copying
the wrong descriptor field, an adjacent-record overflow, or a `rec[70]` state
byte diverging from hardware so `state_commit` takes "others" on a record whose
`+0x14` legitimately holds a data pointer.
