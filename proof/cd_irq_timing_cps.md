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

## Batch 9 (orch/w5/e05) — embedded entries; self-delegates reach ZERO

Transformer extension: `gen_cps_clean.py --timing` now supports
EMBEDDED ENTRIES — guest addrs that are instruction anchors mid-block
(jal targets inside a parent's first block, e.g. 0x8019D508 inside
br2eu_func_8019D500). find_body also matches the per-instruction
`/* 0xADDR: ENC */` op comment; when the requested addr has no block
label or _cont case, `_truncate_embedded_entry` rewrites the parent's
FIRST block only:

- drops instruction segments before the entry anchor (prologue kept
  through the first psx_icache_fetch/psx_cyc_step so fetch accounting
  still brackets the block)
- rewrites psx_slice_block to (entry addr, bcyc - skipped_slots, side)
  — bcyc is instr-slot count, skipped = (entry - block_start)/4
- renames the block label to the entry addr and injects an explicit
  `case 0xADDR: goto block_ADDR` into the _cont switch
- entries in LATER blocks are rejected (branch targets must stay whole)

Bodies this batch:
| symbol | coverage |
|---|---|
| cpst_8018cfdc | cd_file_load_sm — 25 blocks / 372B standalone body |
| cpst_8019d500 | cd_seek_complete_poll parent — 12 blocks (rows for all blocks) |
| cpst_8019d508 | cd_seek_step EMBEDDED entry — truncated first block starts at the addiu sp,-32; skips parent's lui/lw v0 preload exactly as a real jal to 0x8019D508 does |
| (rows) | 8019b310 -> cpst_8019b308 (block inside cd_ext_init_once), 8018f220 -> cpst_8018ef6c (sprintf piece _cont case) |

Result: clean_table self-delegate count 4 -> 0 (8419 wrappers, 0
skipped). The four former self-delegates all resolve through timed CPS
bodies now; the clean-C wrappers' literal call_guest4(self) stays
unwrapped by design but dispatches into the cpst_ body.

Verification: clean build EU+USA; headless :4877 frames 23659 ->
65220 attract CD streaming, no wedge. clean_stats: new entries 0 hits
in attract (file-load/seek/sprintf-piece paths not exercised
headless — faithful-by-construction); cd_cmd_issue still ~84M
dispatches.

## Batch 10 (orch/w5/e05) — residual leaf sweep + missing-row completion

Coverage completion first: 109 clean_cps.map rows added for _cont/entry
cases that existed inside already-emitted cpst_ bodies but were never
registered — sprintf_core_engine resumes (107 rows -> cpst_8018ef6c) and
two alias-body cases (-> cpst_8018e8d0). Those resumes previously fell
through to generated/interp mid-coroutine.

New timed bodies (all 1-block leaf functions, --timing):
| entry | fnmap name | size |
|---|---|---|
| 0x80195A80 | cd_probe_retry | 16B |
| 0x80195B10 | cd_ready_probe | 16B |
| 0x80195BE0 | cd_status_raw | 16B |
| 0x801961EC | cd_file_open | 260B |
| 0x8019A7D4 | cd_intc_busy_read | 16B |
| 0x8019BE28 | cd_mmio_strobe_20 | 40B |
| 0x80191074 | gpu_flag_8322_read | 16B |
| 0x80192580 | gpu_coord_pack | 28B |

clean_table: 8526 wrappers, 0 skipped. Verified: clean build EU+USA;
headless :4883 frames 31281 -> 38938+ attract CD streaming, no wedge.
clean_stats: new entries 0 hits in attract window (init/probe leaves —
faithful-by-construction).

Also landed: proof/generated_zero_audit.md — full audit of
br2_call_generated / psx_dispatch_call sites. Verdict: zero reachable
generated-bypass sites; 20 literal self-delegates remain but are
shadowed dead code (all targets now route to cpst_/clean-C via
clean_cps.map before fnmap wrappers are consulted).

## Batch 11 (orch/w5/e05) — untimed-cps_ residual sweep: LAST live path closed

Sweep result: zero untimed `cps_` map rows exist at >=0x80180000 (all
4512 remaining untimed rows live in 0x8012-0x8017 game-logic, deliberately
untimed — no device deadlines). BUT one live untimed EXECUTION path
remained outside the map: game/gfx_emit_p08.c's nine GPU packet-command
wrappers (gpu_pkt_cmd_f22c..f608) called `cps_8018ef6c()` — the UNTIMED
transliteration of the sprintf/GPU-packet interpreter — directly, bypassing
cpst_8018ef6c. That body is hot (GPU packet dispatch every frame) and its
untimed run had exactly the intra-piece cycle-loss the --timing mode fixes.

Fix: extern + all 9 call sites switched cps_8018ef6c -> cpst_8018ef6c
(identical void(void) contract; cpst_ has every _cont case the untimed
body had, verified incl. 0x8018F504). The untimed body is now fully dead
(still defined in cps_clean.c, zero callers).

Peer check: gfx_emit_p05.c's printf_cps_body is a separate HAND-VERIFIED
timed transliteration of printf_core (163 psx_cyc_/psx_check_interrupts
calls) — already timed, correctly left alone. w3_e09.c cps_ defs are
0x8016/0x8017 game-logic — out of region.

Audit re-check (proof/generated_zero_audit.md): no new generated-dispatch
sites introduced; conclusions unchanged.

Verified: clean build EU+USA; headless :4889 frames 32698 -> 61246+
attract CD streaming, no wedge.

## Batch 11b (orch/w5/e05) — CD leaf-function sweep (8 bodies, 238 rows)

Next tranche of 1-piece CD leaf parents + one medium body, all via
--timing:
| entry | fnmap name | blocks |
|---|---|---|
| 0x80195B4C | cd_bios_cmd20 | 1 |
| 0x80195BF0 | cd_mode_byte_read | 1 |
| 0x801962F0 | cd_stamp_to_sector | 1 |
| 0x80197220 | cd_cmd_bytes | 1 |
| 0x80199420 | cd_read_param_block | 1 |
| 0x8019A7FC | cd_u16_swap_store | 1 |
| 0x8019DAD0 | cd_chan_attr_push | 112 |
| 0x801A1148 | cd_queue_a_read | 1 |

clean_table: 8637 wrappers, 0 skipped. Verified: clean build EU+USA;
headless :4897 frames 29328 -> 57498 attract CD streaming, no wedge.
clean_stats: new entries 0 hits in attract (init/query leaves —
faithful-by-construction); cd_cmd_issue remains hot.

Audit follow-up sweep (batch 11 continuation): cpu->pc literal writes
(842 sites) are all CPS tail-dispatch contract inside cpst_/peer CPS
bodies — they feed the outer dispatch loop, not bypasses. br2_irq_poll
sites are framework IRQ delivery. No new generated-dispatch bypasses
found; proof/generated_zero_audit.md conclusions stand.

## Batch 11c (orch/w5/e05) — 10 more CD 1-block leaves (20 rows)

cd_xchg_bc850(0x80195CA8), cd_status_bcc90(0x80199FFC), cd_xchg_bde20
(0x8019B1D0), cd_mmio_strobe_22(0x8019BE50), cd_xchg_fec24(0x8019FA6C),
cd_job_slots_reset(0x801A0A00), cd_xchg_fefb4(0x801A11E4),
cd_xchg_fefb8(0x801A11F8), cd_state_clear(0x801A1A2C),
cd_ctx_set_28_34(0x801A24C0) — all 1-block timed cpst_ bodies.

clean_table: 8637 wrappers, 0 skipped. Verified: clean build EU+USA;
headless :4901 frames 30684 -> 45772+ attract CD streaming, no wedge.

## Batch 12 (orch/w5/e05) — dead cps_ body sweep + cps_clean.c slim

Residual sweep result: ZERO untimed cps_ map rows >= 0x80180000; the only
cps_ defs left in clean_cps.map are 0x8012-0x8017 game-logic (deliberately
untimed). cps_clean.c carried 543 dead cps_ bodies in 0x8018-0x801B with
no clean_cps.map rows — leftovers from earlier coverage passes where a
body was emitted then its map rows were reassigned to cpst_/peer ports.

Regeneration was REJECTED: ~19 addrs in 0x801261xx-0x801267xx (game-logic
band) have hand-written/readability-pass bodies in cps_clean.c that the
mechanical transformer cannot reproduce (e.g. embedded entries in later
blocks of split pieces — _truncate_embedded_entry only handles first-block
entries). A full regen would have lost those hand edits.

Instead: surgical removal of the 543 dead defs (prototype + body) via
script, verified symbol-exact: remaining defs = exactly the 740 syms that
have clean_cps.map rows; no live def lost, no extra defs added.
File: 115528 -> 67056 lines.

Verified: clean build EU + USA targets; headless :4907 frames
24143 -> 30733+ attract CD streaming, no wedge.

Frontier status: every clean_cps.map row in 0x8018-0x801B resolves to a
timed cpst_ body or a peer hand-port. Remaining generated execution is
unconverted parent piece-groups (~1300) — mechanical --timing candidates.

## Batch 13 (orch/w5/e05) — post-merge untimed-cps_ regression sweep

After fast-forwarding to master 1c35503 (w5e01 cps-final-111 + w5e02
cps-8018-gte merges), re-ran the untimed-cps_ sweep: master's map regen
had re-introduced 6 rows pointing at UNTIMED cps_ bodies in the CD
region — all were _cont resume addrs whose coverage batches 3/7 had
already converted:

- 8018f628/8018f6b4/8018f6e8 -> cps_8018ef6c  (sprintf/GPU-packet
  interpreter continuations; timed body cpst_8018ef6c has all 3 as
  _cont cases — verified case-insensitive, untimed body uses UPPERCASE
  hex labels)
- 80196718/80196798/80196818 -> cps_80196370  (cd_response_process
  continuations; cpst_80196370 has all 3 block/case labels)

Repointed all 6 rows to the cpst_ symbols. Post-fix: ZERO untimed cps_
map rows >= 0x80180000 again.

Direct-call audit on merged tree: only peer hand-ports remain
(printf_cps_*/scriptop_*_cps_* named variants in gfx_emit_p05/w2_e07,
plus fnmap slots 0x80120000/0x80125698 -> cps_801a8ee0/cps_80195cf0
which are 0x8012-band game-logic, deliberately untimed).

clean_table regen: 8733 wrappers, 5 skipped — all legit framework
skips: cps_80175964 no-def (known peer map-row gap) + 4 peer
self-delegate fragments (text_format_*/draw_env_* in gfx_gpu_d0*).

Verified: clean build EU + USA; headless :4923 frames 22753 -> 27514+
attract CD streaming, no wedge.

## Batch 14 (orch/w5/e05) — first uncovered piece-ENTRY conversions

Targeted the worklist's remaining 2-block CD-region defs that have NO
clean_cps.map rows and NO fnmap rows — generated-reachable piece entries:

- cpst_80191e18 (blocks 80191E18/80191E48)
- cpst_80199ab0 (blocks 80199AB0/80199AD0; fallthrough -> 80199AEC
  stays generated — mid-cluster piece, status quo)
- cpst_801a1c90 (blocks 801A1C90/801A1C98)
- cpst_801a3510 (blocks 801A3510/801A3524; fallthrough -> 801A3568
  stays generated)

8 clean_cps.map rows (entry + _cont/block addrs each). Entries now run
timed CPS; intra-piece psx_cyc_*/psx_slice_block charges apply; IRQ
preemption resumes via _cont cases. Continuation pieces reached only by
cpu->pc tail-dispatch still run generated — faithful (same boundary the
untimed cps_ bodies used), strictly better than before (entries were
generated too).

Cluster caveat (worklist): these are pieces of larger state-machine
clusters — e.g. cpst_801a3510's sibling piece 801A3570 tail-dispatches
to 6+ addrs (80199B10/801A36C0/801A4210/801A23FC/801A35B8/801A35B0 +
jalr). Full-cluster timed conversion is future work sized per-cluster.

clean_table: 8741 wrappers, 5 skipped (unchanged legit set).
Verified: clean build EU + USA; headless :4927 frames 7834 -> 14198+
boot/attract, no wedge. clean_stats: new entries 0 hits in window
(infrequent paths — faithful-by-construction).

## Batch 15 (orch/w5/e05) — 8 smallest unrouted CD leaves -> cpst_

Enumerated the remaining pool: 533 unrouted cps_ bodies >=0x8018 in
cps_clean.c (the unconverted parent piece-groups). Took the 8 smallest
(single-block, ~240-440B each):

cpst_8018bb48, cpst_8018e024, cpst_8018da28, cpst_8018de68,
cpst_8018d474, cpst_80191e14, cpst_8018d668 (has jalr _jt_),
cpst_8018d2e0 — 8 clean_cps.map rows (entry-only; no _cont cases).

Notable: cpst_80191e14 tail-dispatches to 0x80191E18 which now routes
to batch-14's cpst_80191e18 — first timed->timed piece chain.
All other fallthrough targets (8018BB4C/E028/DA2C/DE6C/D47C/D2EC)
stay generated — faithful boundary, status quo.

clean_table: 8742 wrappers (+1 new addr; 7 addrs already had fnmap
wrappers, now retargeted to cpst_). 5 skips unchanged (legit set).
Verified: clean build EU + USA; headless :4931 frames 42689 -> 47620+
attract CD streaming, no wedge. clean_stats: 0 hits in window
(infrequent leaves — faithful-by-construction).

## Batch 16 (orch/w5/e05) — transformer fix: exact-entry match + 8 leaves

Transformer fix in gen_cps_clean.py find_body(): it kept the LARGEST
body covering an addr, so addrs that are BOTH a standalone generated
piece (br2eu_func_<addr>) AND an instruction anchor inside a larger
parent resolved to the parent — then _truncate_embedded_entry refused
(non-first-block) and emitted nothing. 5 of 8 candidates failed this
way (801a1edc/801a9e38/801aa5c8/801a1dd4/801aedb8 — all 4-byte embedded
pieces). Fix: exact function-name match (name endswith _<addr>) now
wins immediately; largest-body fallback retained for true continuations.
Verified: cpst_801a1edc now emits the correct standalone 4-byte piece
(v0=3; pc=1EE0) matching untimed cps_ semantics, timed.

8 bodies -> cpst_, 8 map rows: 801a1e58, 801a9ed0, 8019ff2c,
801a1edc, 801a9e38, 801aa5c8, 801a1dd4, 801aedb8. Fallthrough pieces
stay generated.

clean_table: 8750 wrappers, 5 skips (unchanged legit set).
Verified: clean build EU + USA; headless :4937 frames 43061 -> 52410+
attract CD streaming, no wedge.

## Batch 17 (orch/w5/e05) — 12 more unrouted CD leaves -> cpst_

Exact-entry fix from batch 16 cleared all embedded-piece failures —
12/12 candidates emitted cleanly, zero transformer errors:

cpst_801a9b1c, cpst_801a9ebc, cpst_8019fe78, cpst_801a9d30,
cpst_801af938, cpst_801af97c, cpst_801af9c0, cpst_801af9f8,
cpst_801afb24, cpst_801a1d50, cpst_801aa558, cpst_801a4fc0 —
all single-block leaves, 12 map rows (entry-only).

Chain note: cpst_801aa558 tail-dispatches to 801AA55C — the parent of
batch-16's embedded entry cpst_801aa5c8; and the 0x801AF9xx run
(af938/af97c/af9c0/af9f8/afb24) are sibling leaves of one cluster,
all now timed at their entries.

clean_table: 8762 wrappers, 5 skips (unchanged legit set).
Remaining pool ~423 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4941 frames 23834 -> 27954+
attract CD streaming, no wedge.

## Batch 18 (orch/w5/e05) — 12 more unrouted CD leaves -> cpst_

cpst_801a3568 (batch-14 fallthrough target, now timed),
cpst_801a9d10, cpst_80195c40, cpst_8019fe74, cpst_80195c84,
cpst_801a35b0, cpst_8019494c, cpst_80195adc, cpst_8018b88c,
cpst_80199c50, cpst_801a4fb8, cpst_801ad208 — all single-block
leaves, 12 map rows (entry-only). 12/12 emitted, zero transformer
errors.

clean_table: 8773 wrappers, 5 skips (unchanged legit set).
Remaining pool ~411 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4947 frames 19445 -> 23733+
attract CD streaming, no wedge.

## Batch 19 (orch/w5/e05) — 12 more unrouted CD leaves -> cpst_

cpst_801ad5c0, cpst_80195bfc, cpst_801aa600, cpst_801a9e08,
cpst_801a54d4, cpst_801a54e0, cpst_801a54ec (three sibling leaves of
the 0x801A54xx cluster), cpst_801a1d4c, cpst_8018cfd4, cpst_801aa5cc,
cpst_801ad95c, cpst_8018bae8 — 12/12 clean emits, entry-only rows.

clean_table: 8784 wrappers (+12), 5 skips unchanged.
Remaining pool ~399 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4951 frames 17959 -> 22826+
attract CD streaming, no wedge.

## Batch 20 (orch/w5/e05) — 12 more unrouted CD leaves -> cpst_

cpst_801a4fc8, cpst_801a5a04, cpst_801a861c/801a8624/801a862c/801a8634
(four-sibling 0x801A86xx cluster), cpst_801ad5c8, cpst_801ad73c,
cpst_801ad8e8, cpst_801a9cac, cpst_80199aec (batch-14 fallthrough
target now timed), cpst_8018d2d8 — 12/12 clean emits, entry-only rows.

clean_table: 8796 wrappers (+12), 5 skips unchanged.
Remaining pool ~387 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4957 frames 25806 -> 33257+
attract CD streaming, no wedge.

## Batch 21 (orch/w5/e05) — 12 more unrouted CD leaves -> cpst_

cpst_8018d290, cpst_801ad2dc, cpst_801ad950, cpst_801aad18,
cpst_801a1e3c, cpst_801ad938, cpst_801ad944 (0x801AD9xx sibling
pair), cpst_80195a90, cpst_801979ec, cpst_801a9ea8, cpst_801aa5d4,
cpst_801aa5e0 — 12/12 clean emits, entry-only rows.

clean_table: 8808 wrappers (+12), 5 skips unchanged.
Remaining pool ~375 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4961 frames 23830 -> 29198+
attract CD streaming, no wedge.

## Batch 22 (orch/w5/e05) — 12 more unrouted CD leaves -> cpst_

cpst_801a1f64, cpst_801aa95c, cpst_8018def4, cpst_801a1b40,
cpst_801aa0d4, cpst_801ad974, cpst_8018d27c, cpst_801a0b10,
cpst_801a0b24, cpst_801a0b38, cpst_801a0b4c, cpst_801a0b60 (five
sibling 0x801A0Bxx leaves) — 12/12 clean emits, entry-only rows.

clean_table: 8820 wrappers (+12), 5 skips unchanged.
Remaining pool ~363 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4967 frames 24722 -> 29655+
attract CD streaming, no wedge.

## Batch 23 (orch/w5/e05) — 12 more unrouted CD leaves -> cpst_

cpst_801a0b74, cpst_801a0b88, cpst_801a0b9c (rest of the 0x801A0Bxx
cluster), cpst_8018f9a8, cpst_80198f30, cpst_80198f44,
cpst_801aa608, cpst_80195bb8, cpst_80195c10, cpst_801aeca4,
cpst_801a11d0, cpst_801ad774 — 12/12 clean emits, entry-only rows.

clean_table: 8832 wrappers (+12), 5 skips unchanged.
Remaining pool ~351 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4971 frames 39364 -> 49114+
attract CD streaming, no wedge.

## Batch 24 (orch/w5/e05) — 12 leaves + PEER-PORT SHADOWING AUDIT

12 bodies -> cpst_ (80192e24 SKIPPED: fnmap->gpu_tag_cmd_byte_get is a
real peer port in gfx_gpu_d06.c; clean_cps.map rows override fnmap at
dispatch so a cpst_ row would shadow it). clean_table 8808->? wrappers.

AUDIT FINDING (needs orchestrator decision): 56 unique addrs now have
cpst_ rows sitting over fnmap targets that are REAL clean-C ports
(non-delegate): 28 in cd.c, 9 gfx_emit_p06, 5 gfx_emit_p02, 3 gfx.c,
3 gfx_emit_p03, 2 save.c, 2 gfx_emit_p00, 1 each menu/vm/p01/p09.
Examples: cd_probe_retry(cd.c:1802 real loop), prim_emit_ret_a0,
lut_tail_pos, gpu_pkt_field2_get, rotmat2_build_s16, dir_emit_hpair.
My leaf sweep (batches 14-23) picked addrs by unrouted-cps_-body only
and did NOT check fnmap targets — shadowing peer ports violates the
peer-ports-win convention. Both bodies are faithful-by-construction so
this is a convention/maintainability issue, not a correctness bug.
Candidates for row-removal: the ~15 gfx_emit_p0* addrs are clearly
peer GFX ports; cd.c entries need per-function check (leaf ports fine,
coroutine-resume addrs may still need CPS). cpst_ bodies can stay as
dead code or be swept later.

Verified: clean build EU + USA; headless :4977 frame 21286+ advancing.

## Batch 24 resolution + Batch 25 (orch/w5/e05)

Shadowing audit RESOLVED (commit 3a6d01a): removed all 56 cpst_ map
rows whose addrs resolve to non-delegate fnmap targets — verified each
target body has no call_guest/br2_call/dispatch pattern. cd.c ports
(cd_probe_retry, cd_bios_cmd20, cd_chan_attr_push, cd_intc_busy_read,
cd_xchg_*, ...) and gfx_emit_p0*/gfx.c/save.c/menu.c/vm.c ports all
win again at dispatch. Continuation rows not shadowing a peer port
kept (CPS-resume coverage clean-C ports cannot provide). cps_clean.c
dead defs left in place (harmless; peer sweep territory).

Batch 25: pool re-enumerated WITH fnmap-target filter — 76 of 351
remaining unrouted cps_ bodies >=0x8018 sit on peer-port addrs and are
now excluded. Converted 14 filtered single-block leaves -> cpst_
(80199b10, 801a4c88, 801ad650, 801ad8f0, 801ad908, 801ad9a4, 801ada40,
801ada58, 801ada70, 801ae268, 801ae280, 801ae44c, 801ae460, 801ae478),
14 entry-only rows appended to clean_cps.map, bodies spliced into
game/cd_irq_probe.c.

clean_table: 8851 wrappers (+14), 5 legit skips unchanged
(no-def cps_80175964 + 4 self-delegate).
Remaining filtered pool ~261 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4989 frames 6606 -> 12264+
attract CD streaming, no wedge.

## Batch 26 (orch/w5/e05) — 16 filtered leaves

16 single-block leaves -> cpst_ (80195c00, 80199afc, 801a1e70,
801ad8d0, 801ae674, 801adb24, 801adc00, 801ad9bc, 801ae4c4,
801ae098, 801adae4, 8019a8dc, 801adb70, 801adbb8, 801adbd0,
801afeb8), 16 entry-only rows. fnmap-target filter applied — no
peer-port shadowing.

clean_table: 8867 wrappers (+16), 5 legit skips unchanged.
Remaining filtered pool ~245 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4993 frames 17955 -> 23805+
attract CD streaming, no wedge.

## Batch 27 (orch/w5/e05) — 16 filtered leaves

16 single-block leaves -> cpst_ (801adb40, 801ae4a8, 801a9f48,
8018dc00, 801adc18, 801adbe8, 801adc30, 80199d4c, 801ada88,
801adb88, 801adb58, 801adba0, 801a1f50, 801aff04, 80199aa8,
80199b9c). One was fnmap-covered already (+15 unique wrappers).

clean_table: 8882 wrappers, 5 legit skips unchanged.
Remaining filtered pool ~229 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :4997 frames 6648 -> 10254+
attract CD streaming, no wedge.

## Batch 28 (orch/w5/e05) — 14 filtered leaves

14 single-block leaves -> cpst_ (801ae298, 801ae300, 801ae5b0,
801adb00, 8019ff30, 801ae4e0, 801ae524, 801ae2dc, 801ae344,
801ae3b0, 801ae5f4, 801add14, 8019b26c, 801adcf4). fnmap-target
filter applied.

clean_table: wrappers +14 region, 5 legit skips unchanged.
Verified: clean build EU + USA; headless :5001 frames 5575 -> 8633+
attract CD streaming, no wedge.

## Batch 29 (orch/w5/e05) — 14 filtered leaves

14 single-block leaves -> cpst_ (801ae2b8, 801ae320, 801ae38c,
801ae5d0, 80199bb8, 801a1d54, 801ae548, 801a9d14, 801ae58c,
801ae500, 8019a7e4, 801ae368, 801a9d84, 801ae568).

clean_table: 8910 wrappers (+14), 5 legit skips unchanged.
Remaining filtered pool ~201 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :5007 frames 15933 -> 22559+
attract CD streaming, no wedge.

## Batch 30 (orch/w5/e05) — 14 filtered leaves

14 single-block leaves -> cpst_ (801adf00, 801ae6f8, 801a9dac,
801ae6d0, 801a9e5c, 80199ff0, 801add5c, 801adde4, 801ae010,
801ae6a8, 801adf58, 801a9db4, 801ade3c, 801ae068).

clean_table: 8924 wrappers (+14), 5 legit skips unchanged.
Remaining filtered pool ~187 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :5011 frames 7898 -> 11549+
attract CD streaming, no wedge.

## Batch 31 (orch/w5/e05) — 14 filtered leaves

14 single-block leaves -> cpst_ (801adf88, 801ad854, 801ade0c,
801ae038, 801a1e4c, 80191e0c, 801a36c0, 801adfe0, 801adf28,
801adfb0, 801a1c48, 80199c54, 801a9ed4, 801a9e14).

clean_table: 8938 wrappers (+14), 5 legit skips unchanged.
Remaining filtered pool ~173 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :5017 frames 4923 -> 8287+
attract CD streaming, no wedge.

## Batch 32 (orch/w5/e05) — 14 leaves + first _cont row

14 leaves -> cpst_ (801a9e8c, 801ada08, 80199b74, 801ad9ec,
801afed0, 80199ccc, 80199c30, 801ae424, 801ae3fc, 80191d90,
801afe7c, 801ae7b0, 801ae3d4, 801ade6c). cpst_80191d90 carries one
_cont case (0x80191D98) — added resume row 80191d98 -> cpst_80191d90
(no fnmap conflict).

clean_table: 8953 wrappers (+15), 5 legit skips unchanged.
Remaining filtered pool ~159 unrouted cps_ bodies >=0x8018.
Verified: clean build EU + USA; headless :5023 frames 6908 -> 10257+
attract CD streaming, no wedge.
