# Retire 1166 redundant CPS rows — final unclaimed sweep (devin-af)

Agent: devin-af, branch agent/devin-af (base 85eacfca).

## Audit method (devin-ab reusable rule, scripted)

For every `cps_*`/`cpst_*` symbol in `game/clean_cps.map` not owned by a
sibling lane:

1. **Generated-dispatch coverage** — every row addr appears in
   `generated/SLES_017.22_dispatch.c`'s block table routing to a faithful
   `br2eu_func_*` body (25,976 entries indexed; `missing_gen=0` required).
2. **Transliteration provenance** — the symbol's definition comment in
   `game/*.c` matches `/* <sym> — clean CPS transliteration of ...`,
   i.e. machine CPS scaffolding duplicating the same `br2eu_func_*` body
   (same `psx_slice_block`/`psx_cyc_step`/`psx_icache_fetch` timing).

Result: 732 cps_/cpst_ symbols / 2119 rows audited → **557 symbols /
1166 rows RETIRED**; only `cpst_8019b308` (8 rows) failed coverage
(`0x8019B310`-adjacent embedded entry, no generated dispatch row — the
known cd_dma_chan_init piece, deliberately kept per session 43).

## Ownership exclusions honoured

- All `0x8013xxxx` rows (devin-q 0x8013 cluster, incl. devin-ad's
  gameplay cps_80136294/801361bc/80136488/8013638c/cps_8013c5e0).
- cpst_80192714 / cpst_80191fc8 / cpst_8018bba8 / cpst_801843f8 +
  in-span stragglers (devin-ac).
- cpst_80198e2c / cpst_8019d500 / cpst_8019e330 + span (devin-ae).
- cpst_80199440 / cpst_801846a4 / cpst_8019a460 + spans (devin-aa).
- devin-ad gameplay span 0x80136000–0x80136600.

## Deliberately NOT retired — CD/dev_cmd band (77 rows, 14 clusters)

Mechanically they pass the audit (full generated coverage +
transliteration comments), but they transliterate the §4 CD-wedge-history
functions and were excluded per the devin-ab precedent ("CD-subsystem
cpst_ clusters warrant dedicated audit"):

cpst_8018d150(cd_read_issue) cpst_801990f0(cd_dir_entry_finalize)
cpst_8019a00c(cd_cmd_sequence) cpst_8019a09c(cd_subsys_init_full)
cpst_8019a378(cd_kobj_init_b) cpst_8019a3f0(cd_kobj_init_a)
cpst_8019a6e0(cd_dispatch_tbl4) cpst_8019a814(cd_chan_full_init —
devin-k restored wedge-class)
cpst_8019e39c(cd_mode_reset_seq) cpst_801a0be0(cd_hw_chan_init)
cpst_801a1a40(cd_channel_main_init) cpst_801a1b50(dev_cmd_seq)
cpst_801a1e84(dev_cmd_round_dispatch) cpst_801a36f0(cd_channel_table_init)

## Retired rows by band

| band | rows |
|---|---|
| 0x8012 (gameplay/math CPS pieces) | 639 |
| 0x8017 | 120 |
| 0x8018 | 45 |
| 0x8019 | 10 |
| 0x801A | 321 |
| 0x801B | 31 |
| **total** | **1166** |

`src/clean_table.c` regenerated: **5185 → 4013 wrappers** (23 skipped,
all self-delegate). The `cps_*`/`cpst_*` C bodies remain compiled in
`game/cps_clean.c`/`game/cd_irq_probe.c` as dead code (prior-batch
precedent); only the dispatch rows were removed — piece-entry PCs now
fall through to the generated dispatch table's faithful `br2eu_func_*`
entries, identical coverage class minus the duplicate.

## Ports this session

None needed — every unowned cps_/cpst_ cluster either retired under the
rule or is the CD-band set above (generated fallback already faithful;
no clean-C port required by the mission rule "PORT only what genuinely
needs clean C").

## Runtime verification

- Build: `cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo` +
  `cmake --build build -j8` — clean, zero warnings-of-note.
- Headless: `PSX_FN_FILTER="0x80010000:0x80200000"
  ./build/BloodyRoar2_Recompiled --disc "disc/Bloody Roar 2 - Bringer of
  the New Age (Europe).cue" --debug-port 19875 --headless --game
  game_1pad.toml` + `python3 tools/arcade_fight_mash.py 19875`:
  - `*** FIGHT REACHED frame=4268 ***`
  - `LEFT FIGHT` sustained: frames 7997→38236 over ~160s,
    gs=8 ph=3 cycling with gs=9 ph=1 transitions — real combat, no wedge.

## What remains (and why)

- ~77 rows: the 14 CD/dev_cmd clusters above — awaiting the CD-band
  owner's dedicated audit.
- 8 rows: cpst_8019b308 — no generated coverage for its embedded entry;
  must stay CPS until generated gains the entry.
- Everything else cps_/cpst_-named: owned by sibling lanes (devin-aa/ac/
  ad/ae/q) — in flight.
- Named clean rows (cd_*_cps, op_verts_*, prim_*, etc.): LIVE
  continuation dispatch — untouched by design.
