# cps_clean.c reconstruction worklist

Date: after `88e3e2d`/`1100dfa`. Goal: turn the ~1283 `cps_*` transliterations
into idiomatic readable C. This file is the analytical foundation + target
list. Read `proof/masked_function_override_audit.md` first (the named-fn
override audit — most named fns are correctly cps_, do NOT unmask delegates).

## The CPS model (verified against cps_clean.c)

- `cps_clean.c`: **1283 `void cps_<addr>(void)` defs**, each = ONE guest
  basic block (4–40 guest bytes) OR a small multi-block whole-fn unit.
- **Sequential composition** = direct C call `cps_Y(); return;`
  ("fallthrough to split piece").
- **Branches / `jal` / coroutine resume** = `psx_check_interrupts_at(cpu,
  RESUME); cpu->pc = TARGET; return;` → returns to `clean_try_dispatch`,
  which looks TARGET up in `clean_table` → the `cps_` def for that block.
- Every def opens with `if (cpu->pc != 0) { switch(_cont) case RESUME_:
  goto block_RESUME; }` — the **resume switch** that makes mid-function
  re-entry work after a yield.
- `clean_cps.map` registers EVERY block addr → its `cps_` def, so any block
  can be a dispatch/resume entry.

## Scale (measured)

| Metric | Count |
|---|---|
| cps_ defs | 1283 |
| single-block defs | 658 |
| multi-block (whole-fn) defs | 625 |
| defs with yield points | 1052 |
| terminal defs (no `cps_Y()` chain-out) | 790 |
| internal branch dispatch edges | 617 |
| external `jal` dispatch edges | 793 |
| clean_cps.map rows (block entries) | ~7507 |

## Reconstruction = CFG reassembly (NOT mechanical porting)

A whole guest function = a cluster of `cps_` defs linked by `cps_Y()`
calls + `cpu->pc=TARGET` dispatches, ending at `jr ra` (`cpu->pc=ra;return`)
or an external `jal`. To port:

1. Pick an entry addr (a `jal` target — `renames.map` named, or an unnamed
   `fnmap.c` `FUN_`/dispatch target).
2. Follow `cps_Y()` chains + resolve `cpu->pc=TARGET` edges via
   `clean_cps.map` (TARGET→`cps_` def) until `jr ra`/external boundary.
3. The yield points (`psx_check_interrupts_at` + `cpu->pc` resume switch)
   are the hard part: collapsing to structured C removes mid-function IRQ
   resume. **Only safe for functions proven leaf/atomic**, or keep the
   resume semantics (see `cd_response_pump` in `game/cd.c` — a hand-written
   clean fn that manages its OWN `cpu->pc` continuation switch; that is the
   established pattern for coroutine-capable ports).

## Candidate tiers (smallest/cleanest first)

Filter used: multi-block def, ≤1 external `jal`, ≤4 yields, <4KB, no
`cps_Y()` chain-out (self-contained). ~426 defs pass the loose filter;
the tightest (2-block, 0-ext, no-chain) list:

```
cps_801289d0  blk=2 ext=0 y=2   cps_801296f0  blk=2 ext=0 y=2
cps_801297b0  blk=2 ext=0 y=2   cps_80191e18  blk=2 ext=0 y=2
cps_80199ab0  blk=2 ext=0 y=2   cps_801a1c90  blk=2 ext=0 y=2
cps_801a3510  blk=2 ext=0 y=2   cps_80126858  blk=2 ext=0 y=3
cps_80128520  blk=2 ext=0 y=3   cps_801291c8  blk=2 ext=0 y=3
cps_801295e0  blk=2 ext=0 y=3   cps_80129c9c  blk=2 ext=0 y=3
cps_8012a710  blk=2 ext=0 y=3   cps_8012aa7c  blk=2 ext=0 y=3
... (all UNNAMED — internal utils; register a name in renames.map to port)
```

Caveat: even these have yield/resume switches — verify the function is
never re-entered mid-body (check whether any IRQ/coroutine path resumes at
its non-entry block addrs) before collapsing. If unsure, port in the
`cd_response_pump` style (clean C + internal `cpu->pc` switch) rather than
straight-line C.

## Larger whole-fn transliterations (state machines — stay cps_)

`cps_8012fce0` (355 blk), `cps_8015d69c` (332), `cps_8015b190` (175),
`cps_80160a64` (133), `cps_8018ef6c` (121) — these are the coroutine/state
machines that legitimately need CPS. Do NOT attempt.

## Verification constraint

`read_ram`/`write_ram` bypass overlay/dirty-RAM-interpreter resolution —
game data/stack read as zeros (documented in AGENTS.md). State-level
correctness of a ported fn can't be confirmed headless; only
frame/dispatch/cdrom health + `fntrace`/`clean_trace`/`gwatch` are usable.
Prefer ports on paths exercised during boot (CD init, file loads) so a
regression wedges visibly.
