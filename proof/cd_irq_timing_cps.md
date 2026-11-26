# Timing-faithful CPS transliteration for the CD/IRQ region

Date: orch/w5/e05. Builds on `proof/cps_fallthrough_cycle_wedge.md` (the
intra-piece timing loss that got `0x8018`–`0x801B` evicted from
`clean_cps.map`) and `proof/cps_reconstruction_worklist.md`.

## Mechanism

`game/gen_cps_clean.py --timing` emits `cpst_<addr>` bodies that are the
same CPS transliteration as `cps_*` **except the per-instruction timing
calls are preserved verbatim**:

- `psx_cyc_step(cpu, mask)` — R3000A load-delay pipeline interlock + base
  cycle charge, per instruction.
- `psx_cyc_load_word/half/byte(cpu, addr, rt, mask)` — load value AND
  region-wait/completion cycle charge + ReadAbsorb arm (NOT rewritten to
  plain `cpu->read_*`).
- `psx_cyc_lwc2_read`, `psx_icache_fetch`, `psx_slice_block` — kept.
- `psx_cyc_bb_defer_*` batch calls still dropped: with no
  `psx_cyc_bb_defer_begin`, every `psx_cyc_charge` takes the unbatched
  `psx_advance_cycles` path — identical accounting, minus batching.
- `psx_check_interrupts_at(cpu, resume)` + `cpu->pc` tail-dispatch +
  `_cont` resume switch — unchanged; IRQ preemption inside a piece
  resumes at the exact guest PC, same as generated.

Still dropped: `debug_server_*`, `cosim_*`, `PGXP_*`/`_pgx` temps (unwrapped
to real ops), `psx_game_text_native_ok` stale-static guard, `#` directives
(all timing defines are ON for this target).

`br2eu_psx_alias_body_<base>(cpu, 0x<entry>)` 2-arg calls now emit
`cpu->pc = <entry>; return;` — equivalent dispatch through clean_try_dispatch.

## Prototype

`game/cd_irq_probe.c` — `cpst_80198e2c` = `cd_read_poll` (18 blocks, 24
yield points, ~28 KB generated). This is the function `game/cd.c:300`
deliberately delegates via `call_guest4` because "a clean-C poll loop
cannot supply the live $ra/resume-PC the generated CPS frame provides"
(§4 deferred-coroutine starvation class). The cpst_ body supplies BOTH:
the `_cont` switch IS the live resume-PC mechanism and `psx_cyc_*` restores
intra-piece deadline servicing. All 18 block/continuation addrs registered
in `clean_cps.map` → `cpst_80198e2c` (CLEAN_ENT_CPS), which overrides the
fnmap route for the entry addr — `call_guest4(0x80198e2c)` now lands in the
timing-faithful CPS body instead of the generated twin.

## Verification

- `python3 game/gen_fnmap.py && python3 game/gen_clean_table.py` — 7648
  wrappers, cpst_ wraps emitted for all 18 addrs.
- Build clean (`-DPSX_DEBUG_TOOLS=ON`).
- Headless :4821 — frames 12790 → 19703 advancing through attract-mode CD
  streaming; no wedge.

## Status / next

Mechanism PROVEN for one function. The same `--timing` transform applies
to any `0x8018`–`0x801B` function; the ~540 mechanical cps_ bodies evicted
by the wedge fix and the ~132 large uncovered CD bodies are now
transliterable. Per-function caution still applies: functions whose
generated bodies use constructs the transform doesn't model (none seen so
far — `psx_syscall`, `psx_restore_state_escape`, GTE/muldiv timing calls
all pass through verbatim) need individual review before registration.
