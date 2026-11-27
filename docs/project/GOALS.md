# Bloody Roar 2 — Project Goals

Canonical objective statement for the br2-recomp clean-room
reconstruction. `AGENTS.md` is the operational handbook (workflow,
verification, git rules); this file is the *what* and *why*.

## 1. Main goal

Reconstruct Bloody Roar 2 (SLES-01722 EU reference) from the original
PlayStation 1 binary into **clean, understandable, verifiable C
source code** — not an emulator wrapper, not unreviewed decompiler
output passed off as source.

## 2. Reconstruction standard

- Every ported function is hand-verified against the generated
  recompiler oracle (`generated/SLES_017.22_full_*.c`) and/or Ghidra
  disassembly — instruction-level evidence, not guesses.
- Faithfulness first: guest register/stack/cycle/IRQ semantics are
  preserved exactly, including stale-value reads, delay slots,
  sign extension, and coroutine yield points. Readability comes
  *after* correctness, never instead of it.
- Naming: functions get semantic names (`renames.map`) only after
  their behavior is understood; addresses remain the fallback ID.
- No stubs, no fake behavior, no "works on my path" claims —
  verification is headless runtime evidence (frame counters,
  versus-combat mash) plus build-clean strict warnings.

## 3. Architecture (current, working)

- `psxrecomp/` runtime: faithful PS1 core (CPU, memory, GPU, SPU,
  CD-ROM, DMA, IRQ) — the LLE baseline and oracle.
- `generated/` recompiler output: authoritative reference for every
  guest function; never hand-edited.
- `game/` clean C: the reconstruction target — ~348k lines across 63
  files, dispatched per-function via `clean_cps.map` /
  `fnmap.c` → `src/clean_table.c` wrappers.
- `game/cps_clean.c` + `game/cd_irq_probe.c`: CPS transliterations —
  faithful but not yet readable; the remaining frontier.

## 4. Non-goals

- Not a PS1 emulator product; the recomp runtime is infrastructure,
  not the deliverable.
- Not static-recompilation-only: generated bodies are the fallback,
  the goal is their replacement by clean C.
- Not byte-matching the MIPS binary (different artifact); matching
  here means behavioral equivalence verified against the oracle.

## 5. Completion criteria

- All guest functions reachable in gameplay have clean C bodies
  (or a documented, justified delegation for §4 CD/IRQ coroutine
  boundaries).
- `cps_*`/`cpst_*` transliteration bodies are eliminated or reduced
  to genuinely-irrepresentable coroutine cores.
- Headless versus-combat and attract-mode runs stay healthy with
  zero generated hot-path coverage on ported families.
- The project builds reproducibly on macOS (Apple Silicon) and runs
  the game natively.
