# Port frontier — generated→0 worklist (post-`2b57002`)

Date: session 21. Built by diffing `generated/*_dispatch.c` `br2eu_func_*`
entries against `src/clean_table.c` coverage.

## ⚠ Reading the numbers correctly

`br2eu_func_<addr>` entries are **CPS piece-entries**, not logical
functions — one guest function spans many block-entry addresses (the
dispatch table maps each block addr to its own `br2eu_func_*`). An address
absent from `clean_table` is *not* necessarily unported: internal
continuation blocks are reached via fallthrough inside a parent
`cps_*`/clean entry and need no independent dispatch slot.

So "1430 unported func-starts" **overcounts**. The real port frontier is
the **~45 unnamed logical functions** AGENTS.md cites (jal-targets lacking
clean coverage) — identify them via the call graph, not this flat diff.

## Region distribution of unported piece-entries (for reference)

| region | subsystem | count |
|--------|-----------|-------|
| 0x8012 | game-core | 67 |
| 0x8013 | game-core | 11 |
| 0x8014 | game-core | 83 |
| 0x8015 | menu/save | 240 |
| 0x8016 | menu/battle | 123 |
| 0x8017 | battle | 127 |
| 0x8018 | CD/IRQ | 275 |
| 0x8019 | CD/BIOS | 162 |
| 0x801A | CD | 315 |
| 0x801B | kernel/data | 27 |

- **0x8012–0x8017 (651)**: covered by `cps_*` transliterations
  (`clean_cps.map`) — dispatch-clean but *mechanical*, not hand-readable
  clean C. These are the "encode→clean" readability frontier.
- **0x8018–0x801B (779)**: run **generated** post-`2b57002` because cps_
  loses intra-piece timing (CD/IRQ/MMIO deadlines). §4-hard: faithful
  clean ports need per-block `psx_check_interrupts_at` + live guest
  resume-PC, which plain clean C can't express for IRQ/CD-boundary fns.

## Two paths to shrink generated→0

1. **Readability pass on `0x8012`–`0x8017` cps_ functions** — convert
   mechanical `gpr[]`/`goto` transliterations into named-variable clean C,
   one logical function at a time. Tractable, no IRQ boundary. Highest
   volume, lowest risk per function.
2. **CD/IRQ region (`0x8018`–`0x801B`)** — needs the proper fix first:
   real per-instruction `psx_cyc_*` charging inside `cps_` bodies
   (`gen_cps_clean.py`), OR hand-port the ~45 unnamed logical fns that are
   pure-logic and don't touch the IRQ boundary. The §4 `br2_call_generated`
   delegates (53/55 must-delegate) stay generated until a clean port can
   express per-block interrupt resume.

## Suggested next concrete step

Pick ONE unnamed logical function in `0x8014`–`0x8017` (isolated entry,
no MMIO/`0x1F80`/BIOS-svc in its body, ≤ ~15 blocks), port it to named
clean C per §3, regen `clean_table`, build, runtime-verify the path
executes. Repeat. Use `game/renames.map` for naming; keep guest addresses
as guest addresses (§5).
