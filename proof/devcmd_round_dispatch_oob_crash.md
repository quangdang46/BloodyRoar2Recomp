# dev_cmd_round_dispatch wild-dispatch crash (frame ~49548, gs=8)

## Crash
`FAIL-FAST unknown dispatch: addr=0x801BCBA0 ra=0x801A1EC0` on the fixed
binary after ~49.5k frames of real battle (post `DAT1=3` fix). Frozen dump:
`in_exception=1, dispatch_count=0, sio_stat=0x0005, sio_card_active=0,
bail_first=8, bail_flattened=8`.

## Mechanism (static + live table dump)
`dev_cmd_round_dispatch` (`game/save.c:10388`, guest `0x801A1E84`) is the
SIO/memcard command round-robin dispatcher:

```
idx = *(0x801be7c4)                      // round-robin counter
fn  = *(0x801BE808 + idx*4)              // handler table
*(0x801be7c4) = idx + 1                  // increment BEFORE the call
r   = br2_call_generated(fn, ctx, ...)   // jalr handler
// ... post-call counter wrap/decrement ...
```

Live table @0x801BE808 (port 9760): 5 valid code handlers
(`0x801A30C0/3108/31E0/328C/3350`), then zeros. Crash `fn=0x801BCBA0` is a
**data address** — NOT a table entry → the read went out of bounds.

## Hypothesis (leading)
`0x801be7c4` is incremented before `br2_call_generated(fn,...)`. If that
call **bails** (`g_psx_call_bail` unwind), the function returns early and
the post-call counter wrap/decrement is skipped → the counter drifts +1 per
bail, unbounded → `0x801BE808 + idx*4` eventually reads unrelated data →
`fn` = garbage code-looking/data value → wild `jalr`.

Same bail-unwind family as the `DAT1=3` leak (BFSM bail-return skipping the
`DAT1=0` exit write). Both are "clean-C early-return on bail bypasses a
guest postcondition the generated CPS frame would have run."

## To confirm
- Dump `*(0x801be7c4)` on the crashed 9741 (frozen): if large/OOB vs the
  healthy 0–4 range on 9760, counter-drift confirmed.
- Check whether `br2_call_generated(fn,…)` inside dev_cmd_round_dispatch
  is the bail source (bail_first=8 in the freeze dump).
- Faithful fix direction: preserve the guest counter wrap semantics across
  bail (bound/restore idx), NOT a hardcoded clamp — must match what the
  generated CPS frame does on unwind.

## Correction — counter is NOT drifted (live dump on crashed 9741)
`*(0x801be7c4) = 0x00000001` on the frozen crash instance — inside the
valid 0–4 range, and the table @0x801BE808 is intact (5 code handlers then
zeros). So simple counter-drift is RULED OUT: with idx=1 the read yields
`0x801A3108` (valid code), not `0x801BCBA0`.

Revised possibilities:
- `fn=0x801BCBA0` did not come from THIS table read — the wild dispatch may
  be deeper: `br2_call_generated(fn=0x801A30C0/…)` internally dispatched
  to 0x801BCBA0, with `ra=0x801A1EC0` only the outer frame. Check whether
  the handler at table[idx] itself contains a bad indirect call, OR whether
  a `g_psx_call_bail` unwind corrupted `cpu->pc`/`ra` to the data addr.
- The error-handler path reads `fn = *(0x801be780)` (different table) —
  verify which table slot actually held 0x801BCBA0 by dumping 0x801BE780
  region and the exact read address at crash time.
- `bail_flattened=8` in the freeze dump — bail unwinds were active; a
  bail-forced return inside the handler could leave `cpu->pc`/guest state
  pointing at a data addr that the next dispatch treats as code.

Next instrumentation: record the actual table-read address + loaded `fn`
value + `cpu->pc` at the moment of the wild dispatch (wtrace/pc_probe on
0x801A1EB8 jalr and on the dispatch of 0x801BCBA0).

## Freeze-dump forensics (psx_freeze_dump_..._0.json, frame 49548)

The `ra=0x801A1EC0` / `last_store_pc=0x801A2160` leads are MISLEADING —
`ra` is just `$ra` (stale), `last_store_pc` is an unrelated byte-store
(`sb` to `s0+0x44`). The real evidence is in the trace rings:

- **`sio_pc_trace` tail:** the CPU was in a tight **SIO/MMIO polling
  loop** — `pc=0x801A2160` repeatedly writing SIO regs `0x1F801048/04A/04E`
  (SIO_STAT/CTRL/TX/RX block), `func=0x00001B7C`, frames ~3284771 seq.
  This is the memcard/controller SIO poll path (`memcard_poll_frame` /
  `dev_cmd_*`), NOT dev_cmd_round_dispatch's table jalr.

- **`restore_trace` tail (frame 49547, one frame before crash):** continuous
  `restore_escape → restore_resume → rfe_resume` cycling between BIOS addrs
  `0x8019A87C` (context restore) and `0x8019A648`/`0x8019A600` (RFE resume
  targets), `sp=0x801BDD30` (kernel stack), `epc=0x8019A600/648`. IRQs were
  firing every few cycles — the SIO poll loop is IRQ-dense.

- **`wtrace` tail (frame ~49388-49404):** IRQ-context stores (`cpu_pc=0`,
  `sp=0x801BDCB0` kernel stack) writing BIOS/kernel data `0x000E3880/884`,
  `ra=0x801986EC`, `pc=0x801978AC` — the BIOS IRQ handler body.

- **`cpu` at crash:** `pc=0x00000000`, `epc=0x8019A600`, `cause=0x400`
  (interrupt), `in_exception=1`, `sr=0x40000404`. The dispatch target that
  FAIL-FAST'd was `cpu->pc=0x801BCBA0` — a **CD/SIO device-data cell**
  (neighbor of `g_DAT_801bcb80..bcbb0`), not code.

## Revised mechanism (leading)
During IRQ-dense SIO polling, an interrupt's **context save/restore (or an
RFE resume) produced a resume `cpu->pc = 0x801BCBA0`** — a data pointer
read where a code resume address belonged. Same *family* as the earlier
frame-~1600 wild-`ra` crash (context-restore / deferred-switch), but a new
manifestation at frame 49548 inside the SIO poll path. NOT the
round-robin counter (idx=1, table intact) and NOT a handler-table read.

## Next instrumentation
- Correlate the exact IRQ entry whose saved-EPC/context-restore yields
  `pc=0x801BCBA0`: watch writes of `0x801BCBA0` into the TCB save area /
  exception frame, and the `rfe_resume`/`restore_resume` that loads it.
- Check whether `0x801BCBA0` is a legit *saved* value (a coroutine/TCB
  field the kernel legitimately resumes to — i.e. the resume target is a
  *data* cell because the wrong context block was restored) vs a corrupted
  stack slot.
- The `func=0x00001B7C` SIO poll routine and the IRQ restore path are the
  two halves to join: which TCB/context does the restore pick, and does it
  match the interrupted SIO-poll thread?
