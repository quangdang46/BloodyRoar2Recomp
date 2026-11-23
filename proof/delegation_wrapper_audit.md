# Delegation audit — whole-function `br2_call_generated` wrappers

Audit of every clean-C function whose entire body is
`return br2_call_generated(0x<guest>, ...)` — the "clean wrapper pending"
set. Goal: find which are safe faithful-port candidates vs which MUST stay
delegated per AGENTS.md §4 (IRQ/coroutine boundary, BIOS/kernel svc, live
guest frame/resume-PC, MMIO busy-wait needing per-block cycle accounting).

## Totals

**55 whole-function wrappers.** cd.c: 49, save.c: 4, game.c: 1, battle.c: 1.
menu.c / resolve.c / bind.c / ev.c / vm.c / fight.c / ani.c / action.c /
gfx.c: **zero** whole-function wrappers (their `br2_call_generated` hits
are inline jalr fallbacks inside larger clean bodies — correctly not
wrappers).

## Classification

- **MUST-DELEGATE: 53** — IRQ/coroutine boundary, BIOS `B0` svc trampoline,
  `TestEvent`/event-flag wait needing a dispatchable guest resume PC,
  live `cpu->gpr` snapshot/forward (`cd_ctx_save`, `cd_chan_slot_set`),
  MMIO busy-wait needing per-block `psx_check_interrupts_at`, or guest
  stack-frame construction (`cd_read_open`, `cd_mc_state_machine`).
- **PORT-SAFE candidates: 2** — both cd.c, both delegated for *diagnosed
  mistranslation bugs* rather than inherent IRQ constraints:
  - `cd_channel_table_init` `0x801A36F0` (cd.c:1109) — prior port wrote the
    fn-ptr table to wrong base (`0x801c4780` vs real `0x801BE780`).
  - `cd_dma_xfer_start` `0x8019b89c` (cd.c:4449) — prior port wrote guard
    `0x801bdea0` instead of flag `0x801bdf00`.
- **UNCERTAIN: 2** — `cd_cmd_issue_b` `0x80197800` / `cd_cmd_issue_c`
  `0x80197900` (cd.c:903/910); pointer-cell indirection understood but they
  poll CD drive status — likely stay MUST-DELEGATE like `cd_reg_window_op`.

## Verification note (parent)

`generated/` IS present (gitignored — a `find_file_by_name` audit missed it;
grep works). Both candidates' generated bodies were read:

- `0x8019B89C` (`cd_dma_xfer_start`, full_46.c:39687, 4 blocks): loads a
  channel base ptr from `*(0x801BDEB0)`, writes `param_2`→`base+0x1A6`,
  calls `0x8019BE78` twice + `0x8019BE50` once, ORs `base+0x1AA |= 0x30`,
  then writes through MMIO register **pointers** `*(*0x801BDEB4/B8/BC)` and
  sets flag `0x801BDF00=1`. NOT a trivial port — needs the three sub-callees
  and correct MMIO-pointer store primitives (`br2_st*`, not `guest_at`).
- `0x801A36F0` (`cd_channel_table_init`, full_50.c:969, 8 blocks): builds a
  function-pointer/handler table (stores to `0x801CExxx` cells), calls
  `0x801A3D50`, multiple table writes + `call_guest` dispatches. Also not
  trivial.

**Both work correctly via generated delegation today** (CD loads byte-exact,
battles run). Porting them is generated-coverage reduction, not a bug fix —
defer to a dedicated subagent with fresh context rather than a rushed port.

## Conclusion

The whole-function delegation layer is **healthy**: 53/55 are correctly
delegated per §4; the 2 candidates are complex CD MMIO/table functions that
are faithful-as-delegated. **The easy-wrapper surface is exhausted.** The
real remaining clean-coverage frontier is the ~995 generated functions with
NO clean port at all — a much larger sweep that should be attacked
per-subsystem by parallel subagents (runtime-verified), not by auditing
wrappers.
