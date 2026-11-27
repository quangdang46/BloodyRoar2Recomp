# Roadmap — br2-recomp clean reconstruction

Adapted from the ChatGPT 8-phase plan to actual project state.
Phases 0–2 (recon, decomposition, runtime) are DONE; we are inside
Phase 3–4.

## Phase 0–2 — DONE

- ROM identity: SLES-01722 EU (`disc/SLES_017.22`), SYSTEM.CNF,
  PS-X EXE extracted; memory map known (game text 0x80010000–0x80200000).
- Binary decomposition: `generated/SLES_017.22_full_*.c` recompiler
  output = per-function oracle with addresses/sizes/blocks.
- Runtime: faithful psxrecomp core boots the game, versus combat and
  attract mode run headless; debug server on TCP for inspection.

## Phase 3 — C decompilation / clean porting — IN PROGRESS

- `game/` clean C bodies dispatched via clean_cps.map/fnmap.c.
- Coverage: named-fn porting at its floor — every named fn is
  hand-ported, correctly delegated (§4), or correctly cps_
  (self-delegate recursion prevention / coroutine resume).
- Readability campaign: Ghidra identifiers/types eliminated;
  uninit-local and ABI audits done (16+ real bugs fixed);
  cpst_ shadow rows for leaf ports being unmapped (rotmat family
  done, `ad893e5c`).
- Remaining: per-function hand ports reconstructing whole guest
  functions across cps_/cpst_ piece boundaries — especially the
  unnamed clusters and large coroutine state machines.

## Phase 4 — Clean source reconstruction — STARTED

- `renames.map` semantic naming; `game/*.c` module split by
  subsystem (battle/save/menu/cd/gfx/ev/vm/resolve).
- Ongoing: struct/offset documentation in function headers;
  `proof/` audit trail per subsystem.

## Phase 5 — Native engine abstraction — NOT STARTED

- Separate gameplay logic from PS1 hardware deps (GPU/GTE/SPU/DMA/
  CD-ROM). The clean C layer already isolates most of this via
  `guest_at`/`br2_ld*` accessors — formalize into a platform layer.

## Phase 6 — macOS port — PARTIAL (as recomp product)

- `BloodyRoar2_Recompiled` already runs natively on macOS Apple
  Silicon via the recomp runtime. The clean-source native build is
  the remaining work (depends on Phase 3 coverage).

## Phase 7–8 — Validation & release — NOT STARTED

- Function matching report, gameplay regression suite, frame/state
  comparison vs original, reproducible release build.
