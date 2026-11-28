# Retire 120 redundant CPS rows — generated dispatch already covers every entry

## Audit method

For each candidate cluster in `game/clean_cps.map` (10–18 rows, unclaimed):

1. **fnmap containment** — every row addr lies inside the fnmap interval of an
   already-clean function (e.g. `cps_8017e4d4` rows sit inside
   `desc_cursor_advance`'s interval).
2. **Generated-dispatch coverage** — every row addr appears in
   `generated/SLES_017.22_dispatch.c`'s block table, mapping to the
   corresponding `br2eu_func_*` generated function with the exact block-entry
   addr (verified programmatically: `missing_gen=0` for all 120 rows).
3. **Duplicate-source check** — each `cps_*`/`cpst_*` body in
   `game/cps_clean.c` / `game/cd_irq_probe.c` is a machine transliteration of
   the same `br2eu_func_*` generated function (source comments confirm, e.g.
   `cps_8017e4d4 — clean CPS transliteration of br2eu_func_8017E4D4`,
   `cpst_8018bba8 — clean CPS transliteration of pad_slot_state_walk
   (br2eu_func_8018BBA8)`). Same MIPS source, same `psx_slice_block` /
   `psx_cyc_step` / `psx_icache_fetch` timing hooks.

## Retired clusters (120 rows)

| symbol | rows | band |
|---|---|---|
| cps_8017e4d4 | 17 | 0x8017 desc ops (br2eu_func_8017E4D4) |
| cps_8017d5e0 | 14 | 0x8017 desc leaf (after desc_ret_a2p4_c) |
| cps_8017da68 | 11 | 0x8017 button_cond_eval region |
| cpst_8018bba8 | 16 | 0x8018 pad_slot_state_walk |
| cpst_80184eb8 | 14 | 0x8018 scriptop_camera |
| cpst_801843f8 | 14 | 0x8018 scriptop_cmp |
| cpst_801ac8f0 | 12 | 0x801A node_tree_free region |
| cpst_801a9f64 | 12 | 0x801A tween_apply region |
| cpst_80191d9c | 10 | 0x8019 draw_env_timing_axis_limit |

## Not retired (left for CD-band owner / further audit)

CD-subsystem cpst_ clusters — the §4 CD wedge history (session updates
19–20) makes this band higher-risk; all five also have full generated
dispatch coverage but were excluded from this batch:

- cpst_80198e2c (18, cd_read_poll), cpst_80192714 (17, cd_cmd_queue_load),
  cpst_80191fc8 (15, cd_coord_pack_entry), cpst_8019d500 (12,
  cd_seek_complete_poll/cd_seek_step), cpst_8019e330 (11, cd_mode_apply_seq)

## Mechanism

Removing a `clean_cps.map` row removes its `wrap_*` entry from
`src/clean_table.c` (regen: 5375 → 5270 wrappers). Piece-entry dispatch for
those PCs then falls through to the generated dispatch table, which routes
each addr to the faithful `br2eu_func_*` body — identical coverage class to
the retired CPS transliteration, minus the duplicate.

The `cps_*`/`cpst_*` C functions themselves remain in `cps_clean.c` /
`cd_irq_probe.c` (still referenced by any remaining rows and by their own
continuation switches); only the dispatch rows are retired.

## Addendum — 0x8012 band (85 more rows, commit 2)

Same audit applied to the eight unclaimed 0x8012-band clusters; all 85
rows have full generated dispatch coverage and each cps_ symbol is a
transliteration of its br2eu_func_* twin. Retired:

cps_8012ab30(13) cps_8012abe8(11) cps_8012c824(11) cps_8012a94c(10)
cps_80129664(10) cps_8012b700(10) cps_8012b604(10) cps_8012f1b0(10)

clean_table regen 5270 -> 5185 wrappers. `cps_801337bc` (11 rows)
excluded — 0x8013 band is devin-q's claimed cluster.

Verified: build clean; headless arcade — FIGHT frame 6600, LEFT FIGHT
gs=8/9 cycling, frames sustain 34k+ (arcade_fight_mash port 19875).
