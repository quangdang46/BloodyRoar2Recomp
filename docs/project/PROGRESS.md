# Progress snapshot — br2-recomp

Live state as of session 28 (master ~70 commits ahead of mine/master,
unpushed). `AGENTS.md` session updates are the authoritative changelog;
this is the one-page dashboard.

## Done

- Faithful runtime: boot → attract → versus combat headless-verified
  (frame counters advancing past 200k, no wedges since session 21).
- Generated-oracle coverage: every dispatch addr routes clean-first;
  `br2_call_generated` call sites = 0 in game/*.c (session 17).
- Readability: all Ghidra identifiers (`local_*`, `in_*`, `undefined*`,
  `byte`/`ushort`/`uint` typedefs) eliminated across game/*.c.
- Hygiene: uninit-local sweep complete (0 warnings); strict
  -Wtautological/-Wtype-limits clean; goto-fold pass removed ~2900
  no-op gotos from cps transliterations.
- ABI audits (3 rounds, subagent-driven): 16 real bugs fixed —
  host-ptr/guest-addr confusions, dropped args/returns, dead-param
  signatures, frame alloc/free imbalance.
- cpst_ unmask sweep started: rotmat family (8 entries, 62 rows)
  now dispatches clean (`ad893e5c`); leaf-safe criterion established.
- Prototype tightening: ~49 extern decls carry real signatures;
  4 deliberate `()` decls documented.

## In progress / remaining

- Per-function hand ports of cps_/cpst_ bodies — the true frontier.
  Structuring proven dead (every block is a pinned `_cont` resume
  target); work = whole-function reconstruction across piece
  boundaries, prioritized by call frequency and size.
- Coroutine/IRQ-resume functions (cd_* state machines,
  scene_node_walk, printf/sprintf cores, menu_stepper_sm,
  impact_fx_update self-delegate) legitimately keep CPS bodies —
  document or restructure per-function, not mechanically.
- Phase 5+ from ROADMAP: native engine abstraction, clean-source
  macOS build, validation/release.

## Verification commands

- Build: `cmake --build build -j8`
- Headless: `PSX_FN_FILTER="0x80010000:0x80200000" ./build/BloodyRoar2_Recompiled --disc "disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue" --debug-port P --headless --game game_1pad.toml`
- Drive: `python3 tools/versus_fight_mash.py P`; poll `{"cmd":"frame_fingerprint"}` on P.
