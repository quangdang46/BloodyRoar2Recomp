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
