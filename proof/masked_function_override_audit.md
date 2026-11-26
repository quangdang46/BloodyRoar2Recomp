# Masked-function analysis — clean_cps.map override audit

Date: session continuing after `f13682b` (named-GPR pass). Commit `88e3e2d`.

## Question

`game/clean_cps.map` rows OVERWRITE the `renames.map`/`fnmap.c` entry per
addr in `gen_clean_table.py` (`_by_addr[int(a,16)] = ...`). For the ~38
NAMED functions that are overridden to `cps_*`, is the override stale (a
real clean body exists and is safe) or load-bearing?

## Method

Intersected `renames.map` (1446 named fns) and `fnmap.c` with
`clean_cps.map` (7507 rows). Result: **38 named fns overridden to cps_**
(36 via fnmap, same set). For each, inspected the clean C body in
`game/*.c`.

## Classification — the override is almost always LOAD-BEARING

| Class | Count | Why cps_ is required |
|---|---|---|
| **Self-delegate bodies** | ~30 | Clean body is `return br2_call_clean(0xOWN_ADDR,…)` / `call_guest4(0xOWN_ADDR,…)`. The `clean_cps.map` override makes that self-call resolve to `cps_` via `clean_try_dispatch`. Removing it → `clean_table → clean body → br2_call_clean(self) → clean_table → clean body` **infinite recursion**. |
| **Coroutine / IRQ-resume** | ~5 | Need per-block `psx_check_interrupts_at` + live `cpu->pc` resume points the single clean C fn can't express (§4). |
| **BIOS-boundary / indirect-jalr** | ~3 | `mc_file_cmd`, `op_cb*_store_ctx8`, `keyframe_cursor_search` — kernel services / dynamic dispatch. |

## Fixed this session — the only stale overrides

`cd_status_raw` (0x80195BE0), `cd_mode_byte_read` (0x80195BF0),
`cd_param_buf_get` (0x80195C10) — leaf byte/pointer loads, single block,
no calls/yields. Correct clean bodies already existed in `cd.c` and were
used by internal clean callers; the CPS rows (added in `24a527c`) were a
transform artifact. Removed → `gen_clean_table.py` now wraps them to the
clean bodies (ptr return via `guest_of`). Verified: build clean, boot
healthy frame 14k+, CD actively reading, `dispatch_stats` 0 misses.
Commit `88e3e2d`.

## Borderline candidate (deferred — unverifiable)

`mc_job_a_result_poll` (0x80168838): the ONLY masked fn with a real
(non-delegate) clean body whose callees (`mc_job_issue_a`→`mc_issue`,
`cd_job_result`) are also clean. Body is a `for(;;)` retry-poll. NOT
unmasked because: memcard path is unreachable headless (no `read_ram`
state verification), and its coroutine-yield semantics vs the clean
`while(…==0)` poll loops are unproven — §6 compile-clean ≠ proof.
`cps_` transliteration remains faithful. Revisit only with working
state-inspection.

## Reconstruction candidates assessed (all deferred)

- `cd_file_read_seq` (0x801565EC): 4 sequential `jal`s, each a CPS yield
  point. Collapsing to sequential `call_guest` loses inter-call scheduler
  yields → IRQ-timing risk on the wedge-prone CD path. Its clean body is
  itself a `br2_call_clean` self-delegate ("BISECT: needs live-gpr
  coroutine resume").
- `cd_read_issue` (0x8018D150), `mc_job_b_result_poll` (0x801688F0):
  same retry-poll/call-sequence shape — same yield-semantics concern.

## Bottom line

Named-function porting is at its floor: every named fn is hand-ported,
correctly delegated (§4), or correctly `cps_` (delegates need the override
for recursion-prevention; coroutines need it for resume). The ~7490 `cps_`
rows are overwhelmingly **continuation/piece entries** of these functions,
not separate portable units. The remaining readability frontier is
structural reconstruction of whole functions from CPS fragments —
decompilation-scale, not mechanical porting.
