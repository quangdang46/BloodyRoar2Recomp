# cps_clean.c annotation coverage — verification proof

Date: post-merge audit on `orch/w5-e00` @ merge `31bfd4c` + w5e01 final-111
(`0e9e978 "cps_clean: annotate final 111 un-annotated bodies"`).

## Claim verified

**Every `cps_*` transliteration body in `game/cps_clean.c` carries a
descriptive comment.** Zero bodies are bare; zero bodies carry only the
generic "clean CPS transliteration of …" boilerplate.

## Method

Scripted scan over all `void cps_<8 hex>(void)` definitions (1283 bodies,
7197 `block_*` labels total):

1. For each body, walk backwards from the definition to the enclosing
   `/* … */` header comment.
2. **Bare check** — no comment block found, or comment contains no
   descriptive text beyond boilerplate (`from SLES_*`, `clean CPS
   transliteration`, `generated`, `do not edit`, bare addresses):
   **0 hits**.
3. **Boilerplate-only check** — after stripping boilerplate phrases,
   addresses, and `cps_*` names, fewer than 8 alpha chars remain:
   **0 hits**.
4. **Cross-reference check** — every `continues at 0x<addr>` and
   `split piece of cps_<addr>` target resolved against the body set:
   543 refs checked, 1 "dangling" (`cps_8012f160` → `0x8012f164`) which
   resolves to `status_text_dump` in `game/fnmap.c` — the guest fallthrough
   target is a clean-ported function, not a `cps_` body. **Not a defect.**

## Annotation census (1283 bodies)

| Tier | Count | Meaning |
|---|---|---|
| semantic | ~512 | doc comment naming guest semantics (CD record lifecycle, anim/tween, GTE kernels, script VM, screen FSMs, …) |
| split-piece | ~466 | `split piece of cps_<parent>` links to the owning multi-block body |
| renames.map | ~30 | canonical names (`cd_status_raw`, `gpu_callback_dispatch`, …) |
| structural/mechanical | ~369 | terse op-signature one-liners: `single word store to a0+N`, `N-block leaf state step`, `frame epilogue`, `fallthrough stub → 0x…`, `X_wrap: Y caller` |

Per-band mechanical residue: 0x8012:115, 0x8013:56, 0x8014:6, 0x8015:1,
0x8016:2, 0x8017:13, 0x8018:0, 0x8019:19, 0x801a:154, 0x801b:3.

## Honest caveat — the mechanical tier is annotated, not semantic

The 369 structural-tier comments are **accurate** (verified against op
signatures during generation) but shallow. Breakdown by body size:

- 258 bodies = 1 block (degenerate stubs — structural description is the
  appropriate depth)
- 28 bodies = 2 blocks
- 83 bodies = ≥3 blocks — **these are the residual upgrade candidates**;
  75 have ≥4 blocks (max 26). Examples: `cps_80134c30` (26 blk,
  "prim_task_run_wrap"), `cps_8013fea0` (20 blk, "gte_xform_accum_wrap"),
  `cps_80132868` (20 blk, "gte_xform1_wrapper"), `cps_8012bce4` (18 blk,
  "ui_state_step"). Most already name their callee/role; a deeper pass
  would describe internal branch semantics.

## Other files — uncommented-function audit

Heuristic scan (col-0 function definitions lacking a comment in the
preceding lines). Shared boilerplate helpers (`swl_guest`,
`prim_ot_link_indexed`, `br2_lwr/swl/swr`) recur in every `w*.c` file —
generated support code, excluded from counts below.

| File | Uncommented / total | Notes |
|---|---|---|
| game/cps_clean.c | 3 / 1287 | only the 3 inline CPS primitives — **clean** |
| game/w5_e00.c | 0 / 5 | clean |
| game/menu.c | 32 / 183 | **w4e01 exclusive — do not touch** |
| game/battle.c | 181 / 268 | largest gap; named ported functions, mostly self-describing names but no doc comments |
| game/cd.c | 57 / 280 | mixed; trace/dump helpers + ported fns |
| game/save.c | 52 / 285 | ported fns |
| game/bind.c | 59 / 62 | ⚠ diverges from master — confirm ownership before annotating |
| game/gfx.c | 29 / 193 | |
| game/game.c | 22 / 64 | |
| game/ev.c | 9 / 27 | |
| game/vm.c | 6 / 42 | |
| game/resolve.c | 2 / 40 | |
| game/gfx_emit_p04.c | 14 / 14 | all uncommented (euler_rotmat family) |
| game/gfx_gpu_d03/d07.c | 6 / 6 each | all uncommented |
| w2_e*/w3_e*/w4_e*/w5_e* files | ≤9 each | mostly the shared swl_guest/prim_ot_link_indexed boilerplate |

## Verdict

`cps_clean.c` annotation coverage: **COMPLETE** — 1283/1283 bodies
commented, cross-refs valid. The 369-body mechanical tier is documented
above as the only residual depth gap (83 bodies ≥3 blocks).

Next annotation targets by uncommented count (excluding contested
menu.c/bind.c): **battle.c (181)**, **cd.c (57)**, **save.c (52)**,
**gfx.c (29)**, **game.c (22)**.
