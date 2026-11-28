# devin-aa cpst_ cluster sweep — 8019a460 / 801846a4 / 80199440

Agent: devin-aa, branch agent/devin-aa (base e793e674). 114 rows total.

## cpst_8019a460 — RETIRED (19 rows) — commit 4b0b3bc4

- Guest fn `cd_timer_read` @0x8019A460 is a **complete synchronous clean
  port** (`game/cd.c:476`, `uint32_t cd_timer_read(int mode)`): seqlock
  tick sample + vsync_wait_n spins via `br2_irq_poll`, `br2_guest_frame`
  sp reservation, timeout jals via `cd_jal`. Verified insn-by-insn vs the
  generated 19-block body (comment at cd.c:454). It never yields via
  `cpu->pc`, so the CPS body is unreachable — the 19 rows were residual
  CPS scaffolding **shadowing the fnmap entry** (clean_cps.map overrides
  fnmap at the same addr).
- Retirement **activates** the clean port for the ~50 external
  `CPS jal -> 0x8019A460` sites (generated/SLES_017.22_full_*.c) that
  previously ran the cpst_ body — a real coverage upgrade, not just
  dead-row removal.
- All 19 addrs verified in `generated/SLES_017.22_dispatch.c` as
  `br2eu_func_8019A460` alias pieces — faithful fallback, no PC=0 gap.

## cpst_801846a4 — RETIRED (30 rows) — commit a3650102

- Rows are CPS piece/continuation PCs inside the 0x8018444C–0x801846E0
  `scriptop_move` coroutine (one CPS region; generated dispatch splits it
  into br2eu_func_8018444C / _801845B8 / _801845C4 / ... entries).
- Coroutine entry 0x8018444C is NOT a cps row — it dispatches to the
  synchronous clean `scriptop_move` (`game/vm.c:1583`), which never
  yields via `cpu->pc` → the CPS body is unreachable. The late ext-case
  pieces (0x80184620–0x801846E0) are separately covered by the w4_e01.c
  `scriptop_move_ext_cases` ports, whose continuations (shared tails
  0x80184680/AC/D8/DC/E0) never fall back into the retired row set.
- Post-removal coverage (all faithful):
  - 18 addrs → generated `br2eu_func_8018444C` alias pieces
  - 5 addrs → generated `br2eu_func_801845B8` pieces/entry
  - 6 addrs (801845c4/d0/dc/f0/fc/a4) → earlier dedicated
    `cpst_801845XX` rows become effective again (the mega-body
    cpst_801846a4 had shadowed them — those clusters are NOT devin-aa's)
- All addrs have generated dispatch coverage; no fnmap entries at any
  retired addr.

## cpst_80199440 — PORTED (65 rows) — commit aa4ed0a6

- `cd_mc_state_machine` @0x80199440 (memcard/mc-session coroutine, 65
  CPS blocks / 1640 bytes) is the one cluster that could NOT retire:
  the fnmap clean wrapper (`game/cd.c:4612`) is itself a **delegate** —
  `call_guest4(0x80199440)` re-enters `clean_try_dispatch`, so removing
  the cps rows would produce infinite recursion
  (cd_mc_state_machine → call_guest4 → cd_mc_state_machine). The CPS
  body IS the faithful implementation: the clean-C port cannot reserve
  the guest stack frame `cd_result_wait_b`'s out-buffer needs (see
  comment at cd.c:4614-4624).
- Ported timing-faithful 1:1 into `game/recon_8019.c` as
  `cd_mc_state_machine_cps` — all `psx_cyc_*`/`psx_icache_fetch`/
  `psx_slice_block`/`psx_check_interrupts_at` calls verbatim, `cpu->pc`
  resume-switch contract, tail jals via `cpu->pc` re-dispatch. Added
  `rs_lwl/lwr/swl/swr` (cycle-faithful unaligned word helpers, verbatim
  ports of cd_irq_probe.c's static `cpst_lwl` family — needed at one
  unaligned lwl/lwr/sw site @0x801996E8-F8).
- All 65 clean_cps.map rows repointed; `cpst_80199440` body left in
  cd_irq_probe.c as unreachable shadow (generator-owned).

## Verification (all three on RelWithDebInfo builds)

| Commit | Rows | Result |
|--------|------|--------|
| 4b0b3bc4 | 19 retired | FIGHT frame=4197, 24k+ frames / 275s, rounds cycle |
| a3650102 | 30 retired | FIGHT frame=5754, 58k+ frames / 235s, ~12 rounds |
| aa4ed0a6 | 65 ported  | FIGHT frame=4649, 56k+ frames / 249s, ~12 rounds |

Harness: `PSX_FN_FILTER="0x80010000:0x80200000"
./build/BloodyRoar2_Recompiled --disc "disc/...cue" --debug-port <port>
--headless --game game_1pad.toml` + `python3 tools/arcade_fight_mash.py`.
PASS = FIGHT REACHED + frames sustain across multiple gs=8/9 round
phases. Zero wedges.

## Leftovers / notes

- `cpst_80199440` shadow body remains in cd_irq_probe.c (dead code,
  same precedent as other retired/ported clusters).
- The 6 addrs under cpst_801845c4/d0/dc/f0/fc remain CPS-covered by
  their own clusters (unowned by devin-aa; possibly future targets).
- clean_table.c: 5356 wrappers, 22 skipped (all self-delegate).
