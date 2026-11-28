# cps_80132864 / cps_80132868 / cps_801329a8 / cps_801329ac retirement — midpoint_angles_set residual CPS coverage

Agent: devin-ab, branch agent/devin-ab (base e793e674).

## Verdict: RETIRE (residual CPS coverage), not port

All four `cps_*` symbols are intra-function CPS pieces of
**`midpoint_angles_set` @0x801326D4** — already clean-ported as an atomic
synchronous C function at `game/battle.c:10670`
(`void midpoint_angles_set(int *pa,int *pb,int idx)`), dispatch-active via
fnmap.c:1962 + clean_table wrap_801326d4.

### Evidence

- `br2eu_func_801326D4` (generated, 400B/5 blocks) contains block_80132850,
  which does `jal 0x80195160` (gte_rtps_xform1_raw family) with
  **jal link = 0x80132868** — i.e. 0x80132868 is a *return-address
  continuation* inside midpoint_angles_set, not a function entry. The
  continuation then runs the second GTE call (`a0=0x1F800394,
  a1=0x1F8003A8` → `gte_xform1_ir`) plus the abs/min/scale math and the
  `sh result,0x68(idx)` epilogue — all present verbatim in the clean port
  (`_DAT_1f80039c/3a8/3a0/3ac/3a4/3b0` reads map 1:1 onto the CPS body's
  `s5+16/28/20/32/24/36` with s5=0x1F80038C, and `s6+104` = idx+0x68).
- `cps_80132864` (1 row) is the jal delay-slot tail `a2 |= 0x039C` inside
  the same generated span; `cps_801329a8`/`cps_801329ac` (1 row each) are
  the shared `v0 = s3 - v0` tail + register-restore/`jr $ra` epilogue —
  the clean port's `return` covers them.
- Reachability: zero `cpu->pc = 0x80132xxx` dispatch edges into the
  cluster from outside it (only internal fallthroughs + the jal link).
  The clean port is atomic (no cpu->pc continuation, no yields), so no
  CPS resume can ever land on a piece addr — same class as devin-r's
  printf_core / scene_node_walk retires.
- All 23 retired addrs verified present in
  `generated/SLES_017.22_dispatch.c` block tables (br2eu_func_80132864 /
  _80132868 / _801329A8 / _801329AC entries) — no PC=0 gap.

## Changes

- `game/clean_cps.map`: removed 23 rows (20 `cps_80132868` +
  1 `cps_80132864` + 1 `cps_801329a8` + 1 `cps_801329ac`).
- `src/clean_table.c`: regenerated via `python3 game/gen_clean_table.py`.
- `game/cps_clean.c`: `cps_80132864`/`cps_80132868`/`cps_801329a8`/
  `cps_801329ac` bodies left in place as dead code (batch-44/retire
  precedent — generator-owned transliterations).

## Runtime verification

(build + headless arcade_fight_mash results appended below)
Build: `cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo` +
`cmake --build build -j8` — clean (5375 wrappers, 22 skipped self-delegate).

Headless: `PSX_FN_FILTER="0x80010000:0x80200000"
./build/BloodyRoar2_Recompiled --disc "disc/...cue" --debug-port 19871
--headless --game game_1pad.toml` + `python3 tools/arcade_fight_mash.py
19871`:

- `*** FIGHT REACHED frame=11811 ***`, then `LEFT FIGHT gs=8 ph=3`
  round-cycling at fr=15510/19591; `frame` query = 21219 and climbing.
- Zero wedge, zero crash. midpoint_angles_set (camera midpoint tracking)
  runs every versus/arcade frame — the retired pieces were unreachable
  residual coverage as predicted.
