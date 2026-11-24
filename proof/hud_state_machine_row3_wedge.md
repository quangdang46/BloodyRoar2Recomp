# hud_state_machine mode-(1,4) tally wedge — investigation proof

## Symptom

Versus-attract / menu row-3 path reaches `gs=8`, `dat1=0x0b`, `sig=(1,4)`
(match-end tally) and parks forever in a busy poll inside
`hud_state_machine(0x8016e21c)`. Live LLDB backtraces show:

```
hud_state_machine(a0=1) -> frame_tick(0x1f800000) -> cd_timer_read ->
generated br2eu_func_8019A5D8 / psx_check_interrupts
```

i.e. the machine is re-entering `case 0` -> `poll` every frame; it never
advances past the `sp48 == 0` gate in `mode1body`.

## Root-cause candidate examined and FIXED (real mistranslation)

Guest `hud_state_machine` computes a row-select flag once at entry:

```
F = ((lhu(g+0x92) & 0xf00) ^ 0x100) != 0      ; guest sp+0x58
```

and uses `F*2` to index the per-side HUD row at `g+0x68/0x6c/0xb4`
(`generated/SLES_017.22_full_30.c` `0x8016E270-0x8016E280` computes F;
`0x8016EA8C-0x8016EAA0` does `lw sp+0x58; sll 1; addu 0x1f800000`).

Clean `game/save.c` kept `sp58` as the INVERSE
(`((x & 0xf00) ^ 0x100) == 0`, i.e. `==0x100`) — correct for the side-block
`sp20` select, wrong for the four row-select sites. Fixed by indexing
with `(sp58 ^ 1) * 2` at:

- `+0x6c` gate in `after678` (~save.c:5349)
- afterdraw `+0x68` check (~save.c:5421)
- `+0xb4` update in `mode0body` (~save.c:5484)
- `case2` `+0x68` check (~save.c:5530)

This is a genuine clean-port bug (wrong side's pad/flag rows read/written),
but **the wedge still reproduces after the fix** — it was not the sole
cause.

## Wedge mechanism (verified faithful to generated)

At tally entry, `case 0` latches `sp48` from the per-side round-win
counters:

```
sp20 = (sp58 ? 0xe8 : 0x1f0)          ; nibble-selected "primary" side block
sp24 = *(sp20 + 0x2c)                 ; the other side block
b5    = lhu(sp20 + 0xe4)              ; primary's round-win count
b4    = lhu(sp24 + 0xe4)              ; other side's count
if (b4 < b5) sp48 = 0 else sp48 = 1
```

`mode1body` parks in `poll` while `sp48 == 0`. Clean code at
`save.c:5410-5440` is instruction-for-instruction identical to generated
`0x8016E420-0x8016E4A0` (verified: same load order, same `sltu` direction,
same branch targets).

The `g+0x92` f00 nibble is written by `confirm_bookkeeping` as
`ph = select_phase_gate(g0) | select_phase_gate(g1)<<4`-style encoding;
each side's gate passes when `lhu(g+0xb4 or +0xb6) & 0xf00` is in the
active phase set. Live wedged state (port 9961):

- `lhu(g+0x92) = 0x010E` → f00 was `0x100` at latch (side0 active, side1 not)
- `g+0xb4 = 0x0100` (side0 phase armed — it won the last round)
- `g+0xb6 = 0x0000` (side1 never armed)
- side blocks: `0x1f8000e8+0xe4 = 3` wins, `0x1f8001f0+0xe4 = 2` wins
- `s1+0x99` byte = `0xff` (-1) → versus mode → `post_battle_dispatch`
  case 7/8 increments the two `+0xe4` counters **independently**
  (per-side round wins), so a decisive 3–2 produces `sp20.e4 > sp24.e4`
  → `sp48 = 0` → park.

For the guest this state would also produce `sp48 = 0` **if reached with
these values** — the clean port is faithful. Therefore the divergence must
be **upstream state**: either the f00 nibble should have differed at tally
entry (e.g. both sides active → `sp20 = 0x1f0`, the 2-win side → `sp48=1`),
or the `+0xe4` counters should have been equal/reset, or `DAT1=0x0b`
should never have been armed for this match configuration.

## Experiment: delegate hud_state_machine to generated

`save.c` was temporarily changed to
`br2_call_generated(0x8016e21c, a0,0,0,0); return;`. Result: the instance
hit `FATAL: dispatch recursion guard tripped — runaway self-call` during
the run (generated body re-enters dispatch through a path that loops back
to the clean wrapper) — **inconclusive, reverted**. Diagnostic code and
the `decomp_trace_record` poll hook were fully removed; `git diff` now
shows only the four row-select fixes.

## Remaining hypotheses for the upstream divergence (ranked)

1. **`confirm_bookkeeping` phase-gate timing**: `g+0xb4/+0xb6` are armed
   per-round during `post_battle_dispatch`; if the guest re-arms/updates
   them after match end (winner/loser bookkeeping) before `DAT1=0x0b`,
   `ph` would encode both-active (`0x300/0x400`) → `sp20=0x1f0` → tally
   proceeds. Needs a trace of who writes `g+0xb4/+0xb6` between last round
   and tally entry on a known-good path.
2. **`s1+0x99` match-config byte**: if it should be `>= 0` for this mode,
   counters increment as a pair → always equal → `sp48=1` always. The
   byte comes from `stage_char_data_load` (`MB8(s0+0x14)` char record);
   verify which mode (1,4) actually selects versus-per-side vs shared.
3. **Beetle oracle A/B**: run the identical input script on
   `psx-beetleoracle` to get ground truth for `sp48`, `+0xe4` counters, and
   the `0x92` nibble at tally entry. This is the decisive next step —
   everything in the clean port is now verified faithful, so any remaining
   wedge is either guest-faithful behavior on a divergent state or a
   hardware-timing artifact only the oracle can resolve.

## Artifacts

- Fix: `game/save.c` four `(sp58 ^ 1) * 2` row selects (+ comments).
- Live evidence: port 9961 wedged instance (still running, PID list in
  session), ports 9945/9955/9960/9971 earlier captures.
- Generated reference: `generated/SLES_017.22_full_30.c`
  `0x8016E21C`–`0x8016EB00` (hud_state_machine body).
