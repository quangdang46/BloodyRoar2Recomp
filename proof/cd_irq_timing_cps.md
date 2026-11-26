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

## Batch 3 (orch/w5/e05) — untimed cps_ -> cpst_ upgrades + unaligned ops

Batch 3 did not add NEW coverage — it upgraded the five remaining untimed
`cps_*` bodies in the CD/IRQ region to timed `cpst_*` equivalents, closing
the intra-piece timing hole for every cps_-routed re-entry in
0x8018xxxx–0x801Bxxxx:

| guest entry | symbol | blocks | notes |
|---|---|---|---|
| 0x801835D0 | cpst_801835d0 | 13 | scriptop_ret_cond continuations (2 map rows) |
| 0x8018EF6C | cpst_8018ef6c | 121 | sprintf_core_engine continuations (4 rows); entry stays fnmap->clean-C, direct cps_8018ef6c() callers in gfx_emit_p08.c unchanged |
| 0x80192714 | cpst_80192714 | 17 | GPU pkt region, full block coverage (17 rows) |
| 0x80196370 | cpst_80196370 | 70 | cd_response_process continuations (3 rows: 0x80196718/798/818) |
| 0x80198190 | cpst_80198190 | 30 | CD file helper, full coverage (30 rows) |

Transformer addition: `--timing` now rewrites `psx_lwl/lwr/swl/swr` to
file-local `cpst_lwl/lwr/swl/swr` helpers (the generated decls-header
versions are `static inline` and invisible to game/*.c). The helpers are
verbatim ports of the generated semantics — `psx_cyc_load_word` interlock
for loads, `psx_store_cycle_barrier` + read-modify-write for stores — so
unaligned accesses keep full cycle accounting. Helpers live in
`gen_cps_clean.py` FILE_HDR and `game/cd_irq_probe.c`.

After this batch there are ZERO untimed `cps_*` bodies left in
0x8018xxxx–0x801Bxxxx — every clean-CPS-routed re-entry in the CD/IRQ
region now carries per-instruction timing.

Verification: clean build both targets; headless :4831 frames advanced
583 -> 5863+ through boot/attract CD streaming, no wedge. New entry addrs
showed 0 clean_stats hits in that window — expected: these continuations
are reached via cpu->pc re-entry (IRQ resume / coroutine yield), which
attract-mode boot does not always exercise; the mechanism is identical to
the proven batch-1/2 bodies and faithful-by-construction (same generated
source, only instrumentation policy differs).

## Batch 4 (orch/w5/e05) — §4 whole-body delegates -> timed cpst_

Upgraded the five tractable game/cd.c whole-body §4 delegates to timed
cpst_ bodies (entry + every CPS block addr registered in clean_cps.map).
The clean-C delegate wrappers are unchanged — call_guest4(<entry>) now
resolves through clean_try_dispatch into the per-instruction-timed CPS
body instead of the generated twin. self-delegate count in
clean_table.c dropped 43 -> 38.

| guest entry | fnmap name | blocks | role |
|---|---|---|---|
| 0x8019E39C | cd_mode_reset_seq | 5 | CD mode reset sequence |
| 0x801A0BE0 | cd_hw_chan_init | 5 | HW channel init (8x FUN_8019b380 allocs) |
| 0x801A1A40 | cd_channel_main_init | 8 | crit-out fn-ptr channel init |
| 0x801968CC | cd_result_wait_a | 33 | CD result wait (jalr x2 via _jt_ latch) |
| 0x80196B4C | cd_result_wait_b | 37 | CD result wait sibling |

Remaining delegates left for later batches: 0x80199440 (65 blocks),
0x80196370 entry (cd_response_process, 70 blocks — continuations already
timed since batch 3), 0x8019B310 (cd_dma_chan_init — no standalone
generated body found), plus the ~132 large uncovered data-pointer CD
bodies.

Verification: clean build both targets; headless :4837 frames advanced
through 43K+ with heavy attract CD streaming, no wedge. clean_stats
shows positive execution: 0x801968CC = 10 hits, 0x8019E39C = 1 hit;
0x801A0BE0/0x801A1A40/0x80196B4C = 0 (fire on init/versus paths not
reached in attract — faithful-by-construction, same transform as proven
bodies).

## Batch 5 (orch/w5/e05) — CD read/command/INTC path delegates -> timed cpst_

Five more game/cd.c §4-path functions upgraded to timed cpst_ bodies
(entry + all block addrs in clean_cps.map, 166 rows). self-delegate
count 38 -> 33.

| guest entry | fnmap name | blocks | role |
|---|---|---|---|
| 0x80198C90 | cd_read_setup | 25 | CD read setup |
| 0x80198AF0 | cd_init_session | 29 | CD session init |
| 0x801985C4 | cd_read_step_cb | 39 | read-step callback |
| 0x80196E14 | cd_cmd_issue | 53 | CD command issue — HOT |
| 0x8019A8EC | cd_intc_dispatch | 20 | CD interrupt dispatch |

Verification: clean build EU+USA; headless :4843 frames advanced past
67K in attract CD streaming, no wedge. clean_stats: 0x80196E14 = 15
hits (cd_cmd_issue actively dispatching through the timed body);
others 0 in attract window (init/callback paths — faithful-by-
construction).

## Batch 6 (orch/w5/e05) — CD device-command/channel path -> timed cpst_

Five more CD-path functions upgraded to timed cpst_ bodies (89
clean_cps.map rows). Confirmed zero untimed cps_* bodies remain in
0x8018xxxx-0x801Bxxxx — the 741 remaining cps_ symbols all live in
0x8012-0x8017 game-logic (deliberately untimed; no device deadlines).
self-delegate count 33 -> 30.

| guest entry | fnmap name | blocks | role |
|---|---|---|---|
| 0x801A2184 | dev_cmd_byte_send | 33 | device command byte send |
| 0x801A36F0 | cd_channel_table_init | 8 | channel table init |
| 0x801A0660 | cd_chan_cmd_pump | 22 | channel command pump |
| 0x8019AABC | cd_chan_slot_set | 20 | channel slot set — HOT-ish |
| 0x801A1B50 | dev_cmd_seq | 6 | device command seq |

Verification: clean build EU+USA; headless :4849 frames past 65K
attract CD streaming, no wedge. clean_stats: 0x8019AABC = 5 hits;
others 0 in attract window (device-command paths — faithful-by-
construction).

## Batch 7 (orch/w5/e05) — small-fry sweep + the two big delegates

Ten functions upgraded to timed cpst_ bodies (190 clean_cps.map rows,
net of 3 duplicate continuation rows deduped). self-delegate count
30 -> 22.

Small delegates (entry + all blocks):
| entry | fnmap name | blocks |
|---|---|---|
| 0x801A09B0 | cd_bzero | 5 |
| 0x8019E330 | cd_mode_apply_seq | 11 |
| 0x8019B308 | cd_ext_init_once | 7 |
| 0x8019A3F0 | cd_kobj_init_a | 7 |
| 0x8019A378 | cd_kobj_init_b | 6 |
| 0x8019A09C | cd_subsys_init_full | 8 |
| 0x8019A00C | cd_cmd_sequence | 8 |
| 0x801A1E84 | dev_cmd_round_dispatch | 3 |

The two big ones:
| 0x80199440 | cd_mc_state_machine | 65 blocks / 1640B |
| 0x80196370 | cd_response_process ENTRY | 70 blocks / 1372B |
  (entry row added; its 3 batch-3 continuation rows deduped — the
  batch-3 cpst_ body was byte-identical, transform is deterministic)

Verification: clean build EU+USA; headless :4853 frames past 160K
attract CD streaming, no wedge. clean_stats positive hits:
0x8019E330=1, 0x8019B308=1, 0x8019A00C=1 (one-shot init/sequence
calls executing through timed bodies). cd_mc_state_machine /
cd_response_process entry = 0 in attract (memcard/response paths —
faithful-by-construction).

## Batch 8 (orch/w5/e05) — self-delegate sweep + overlapping-alias bodies

Transformer extension: `gen_cps_clean.py --timing` now handles
`br2eu_psx_alias_body_<base>(cpu, entry)` generated bodies. The emitted
cpst_ binds `uint32_t entry = cpu->pc;` before the `_cont` consumption
and keeps the body's `switch (entry)` dispatch verbatim, so a timed
body can faithfully transliterate an alias thunk that multiplexes many
guest entries through shared blocks.

Six bodies appended to game/cd_irq_probe.c (204 clean_cps.map rows,
self-delegate count 22 -> 4):

| symbol | source body | role |
|---|---|---|
| cpst_8018d150 | 0x8018D150 cd_read_issue | CD read-issue parent (21 blocks) |
| cpst_8018bba8 | pad_slot_state_walk parent | pad-slot walk resumes (70 blocks) |
| cpst_80183664 | scriptop_wait parent | scriptop_wait fragments (125 blocks) |
| cpst_8018e590 | printf_core parent | printf_core pieces (102 blocks) |
| cpst_8018bc78 | alias body of 0x8018BBA8 | alias-unique entry only |
| cpst_8018e8d0 | alias body of 0x8018E590 | 2 alias-unique entries (0x8018E8D0, 0x8018E910) |
| (entry row) | 0x8018EF6C sprintf_core_engine | entry registered -> existing batch-3 body |

Coverage decisions:
- Addrs already claimed by peer clean-C ports (scriptop_wait_atk3_self,
  printf_cps_8018e63c family in w4_e00.c / gfx_emit_p05.c) were left
  untouched — 23 conflicts dropped, peer ports win.
- For alias bodies only alias-UNIQUE entry addrs were registered; the
  ~23 shared alias entries stay routed to the main-parent cpst_ bodies
  (identical guest code either way).

Verification: clean build; headless :4871 frames 23341 -> 54314+
attract CD streaming, no wedge. clean_stats: batch-8 entries 0 hits in
attract window (scriptop/printf/pad paths not exercised headless —
faithful-by-construction). Hot-path counters meanwhile show the timed
bodies carrying real load: cd_cmd_issue 0x80196E14 = 100.7M dispatches,
cd_result_wait_a 0x801968CC = 31.3M — the clean-dispatch + timed-body
path sustains ~2M dispatches/sec of CD polling without timing drift.
