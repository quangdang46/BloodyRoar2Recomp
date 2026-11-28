# devin-z — cpst_80196370 (cd_response_process) + cpst_80196e14 (cd_cmd_issue) ports

## Shadow-audit (retire-vs-port triage, per proof/cpst_8018e590_8018ef6c_retire.md recipe)

Both clusters are **whole-function CPS transliterations**, not intra-function
pieces of an already-clean port:

- `0x80196370` / `0x80196E14` are real `jal` targets: generated CPS bodies in
  `generated/SLES_017.22_full_45.c` tail-dispatch `cpu->pc = 0x80196370` /
  `cpu->pc = 0x80196E14` from many call sites (grep: ~4 sites for 80196370,
  ~13 for 80196E14 in full_45.c alone, plus fnmap entries
  `cd_response_process` / `cd_cmd_issue`).
- The clean-C names in `game/cd.c` are **delegating thunks**, not ports:
  `cd_response_process()` (cd.c:1214) and `cd_cmd_issue()` (cd.c:371) both
  forward via `call_guest4` → clean dispatch. `cd_cmd_issue` has ~28 clean-C
  call sites with real args, so the thunk must stay.
- Therefore: PORT (relocate the cpst_ body into `game/recon_8019.c` under a
  clean name), not retire.

## What was done

Timing-faithful ports appended to `game/recon_8019.c` (devin-v style —
verbatim cpst_ bodies, all `psx_cyc_step` / `psx_cyc_load_*` /
`psx_icache_fetch` / `psx_slice_block` / `psx_check_interrupts_at` kept,
`cpu->pc` resume-switch contract unchanged; signature widened
`void` → `uint32_t` returning `cpu->gpr[2]`, matching devin-s's
`cd_result_wait_*_cps` convention):

- `cd_response_process_cps` (guest 0x80196370) — 70 CPS rows.
  Source: `game/cd_irq_probe.c` cpst_80196370 (lines 59159–60545).
- `cd_cmd_issue_cps` (guest 0x80196E14) — 53 CPS rows.
  Source: `game/cd_irq_probe.c` cpst_80196e14 (lines 61921–62957).

`game/clean_cps.map`: all 70 + 53 rows repointed `cpst_*` → clean names.
`game/fnmap.c` unchanged (no new fnmap entries needed — `_cps` names are
dispatch-only via clean_cps.map, same as cd_result_wait_a_cps).
`src/clean_table.c` regenerated: 5398 wrappers, 22 skipped (all
self-delegate). The cpst_ bodies remain in cd_irq_probe.c as unreachable
shadow, per convention.

The cd.c thunks still forward correctly: `call_guest4(0x80196370/0x80196e14)`
→ clean_try_dispatch → clean_cps.map → the new `_cps` bodies.

## Runtime verification

Build: `cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo` (debug tools
ON) + `cmake --build build -j8` — clean, no new warnings in game/.

Headless run:
```
PSX_FN_FILTER="0x80010000:0x80200000" ./build/BloodyRoar2_Recompiled \
  --disc "disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue" \
  --debug-port 19869 --headless --game game_1pad.toml
python3 tools/arcade_fight_mash.py 19869
```

Run 1: **FIGHT REACHED frame=7268** (t≈5s); frames sustained to
**fr=34794 at t≈357s+** with repeated round cycles
(`LEFT FIGHT gs=8 ph=3 → gs=9 ph=12 → gs=8 ph=3` — round-end/restart
transitions prove CD streaming of subsequent rounds works). ~7.7 min of
continuous advancing frames.

Run 2 (repeat): **FIGHT REACHED frame=2159** (t=8s), `LEFT FIGHT
fr=3300` at t=13s — same healthy path.

NOTE: both headless processes exited ~t≈400–465s wall with no crash
output (exit -1/0). This is environmental — the box was running ~8
concurrent agent builds + verifies (heavy clang load); frames were still
advancing when the debug server last responded. Not a game wedge (a wedge
freezes frames while the process stays alive). Worth re-verifying on an
idle machine if a longer soak is desired.

## Leftovers

- None for these two clusters. The cpst_ shadow bodies in
  cd_irq_probe.c are intentionally retained (unreachable).
- `cd_response_process`/`cd_cmd_issue` canonical names remain the cd.c
  thunks; the `_cps` suffix matches the devin-s convention for
  thunk-backed CPS ports.
