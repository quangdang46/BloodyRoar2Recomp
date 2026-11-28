# devin-ac — CD-band cpst_ cluster triage: 2 ports + 2 retire batches

Agent: devin-ac, branch agent/devin-ac (base 3844eb67).

## Targets & verdicts

| cluster | rows | verdict |
|---|---|---|
| cpst_80192714 | 17 | PORT -> gpu_prim_pkt_emit_cps (recon_8019.c) |
| cpst_80191fc8 | 15 | PORT -> gpu_texprim_pkt_emit_cps (recon_8019.c) |
| cpst_8018bba8 | 16 | RETIRE — intra-fn residue of clean pad_slot_state_walk |
| cpst_801843f8 (+4 shadow cpst_801843c0/d4/e0/f0) | 18 rows / 13 addrs | RETIRE — intra-fn residue of clean scriptop_cmp |

## Shadow-audit evidence (proof/cpst_8018e590_8018ef6c_retire.md recipe)

### cpst_8018bba8 — RETIRE
- Guest fn 0x8018BBA8 (380B) = `pad_slot_state_walk`, clean-ported
  synchronously in game/menu.c:5036; fnmap.c:2832 routes the entry there.
- The 5 interior CPS resume PCs (0x8018BC2C/78/B4/D0/E4) already have
  clean self-delegate entries in game/gfx_emit_p01.c:151-182 — the clean
  parent runs the whole switch atomically and never yields cpu->pc.
- All 16 cpst_ row addrs lie inside the fn span (0x8018BBA8..0x8018BD24)
  and have generated dispatch coverage via
  br2eu_psx_alias_body_8018BBA8 (generated/SLES_017.22_dispatch.c).
- Zero `cpu->pc = 0x<row>` references anywhere in generated/*.c — no
  external jal/j/CPS edge can land on a piece PC; the only producer would
  be the parent's own psx_check_interrupts_at, which never runs while the
  clean parent owns the entry.

### cpst_801843f8 — RETIRE (incl. shadow rows)
- Guest fn 0x80184378 (212B) = `scriptop_cmp`, clean-ported in
  game/vm.c:727; fnmap.c:2700 routes the entry.
- The internal switch jump table at guest 0x80123680-0x801236B4 holds
  piece PCs 0x801843C0/F8/D4/E0/F0 — all inside the fn span; reached only
  by the fn's own `jr` dispatch.
- clean_cps.map carried DUPLICATE coverage: 14 rows -> cpst_801843f8
  (incl. a duplicated 0x801843F8 row) shadowed by 4 single rows ->
  cpst_801843c0/d4/e0/f0 (separate transliterations of the same parent's
  alias entries). All 18 rows / 13 unique addrs retired; every addr has
  generated coverage (br2eu_func_801843xx alias bodies into
  br2eu_func_80184378).
- Zero external `cpu->pc` refs to any row addr.

### cpst_80192714 — PORT (gpu_prim_pkt_emit_cps)
- Real unnamed fn (560B/17 blocks) in the GPU func table at
  0x801B82E4 (between cd_queue_push 0x80192F14 and gpu_tag_cmd_store
  0x80192E00; neighbors gpu_draw_env_dma/gpu_image_transfer_dma).
  Reached via `jr` through the table — no static jal/j, so it needs the
  clean CPS body for the indirect-call path.
- Body: clamps signed halfword coord pairs at a0+4/+6 against screen
  bounds at 0x801C8324/26, packs a GPU primitive packet into the fixed
  staging buffer 0x801FAD20 (0x80200000-21216), commits register writes
  via gpu_reg_cmd_commit (a0=3,4,5), finishes with
  gpu_reg_cells_init(0x801FAD20). Two variants selected by
  (h[0]&0x3F)!=0.
- Ported VERBATIM from cpst_80192714 (all psx_cyc_step /
  psx_cyc_load_half/word / psx_icache_fetch / psx_slice_block /
  psx_check_interrupts_at preserved; cpu->pc resume-switch + CPS
  jal/jr tail-dispatch contract intact).

### cpst_80191fc8 — PORT (gpu_texprim_pkt_emit_cps)
- Real unnamed fn (504B/15 blocks) between cd_coord_pack_entry
  (0x80191F88, ends jr ra at 0x80191FC0) and gpu_prim_pkt_build
  (0x801921C0). No static jal/j/absolute-pointer refs found in the EXE —
  reached indirectly (jr/jalr, likely a sibling GPU-table slot or
  runtime-built pointer); ported anyway since the CPS rows were live
  dispatch and the body is self-contained.
- Body: textured primitive packet emitter — reads a source record
  (s0=a0), builds header words via gpu_coord_clamp_a/b +
  gpu_coord_pack + gpu_pkt_pack + gpu_texpage_pack into dest (s1=a1),
  writes 0xE6000000 tag, then optionally clamps+packs a second vertex
  block when byte s0+24 != 0.
- Ported VERBATIM, same timing contract.

## Changes

- game/recon_8019.c: +847 lines — gpu_prim_pkt_emit_cps (452) +
  gpu_texprim_pkt_emit_cps (390) appended with banner convention.
- game/renames.map: +2 entries (80192714, 80191fc8).
- game/clean_cps.map: 32 rows repointed cpst_* -> new names; 34 rows
  deleted (18 scriptop_cmp span + 16 pad_slot_state_walk span).
- game/fnmap.c, src/clean_table.c: regenerated
  (python3 game/gen_fnmap.py && python3 game/gen_clean_table.py).
  clean_table: 5375 -> 5346 wrappers (29 unique addrs retired),
  22 self-delegate skips unchanged.
- cpst_ bodies in game/cd_irq_probe.c left as dead code (precedent).

## Runtime verification

cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo + build -j8: clean.

Headless: PSX_FN_FILTER="0x80010000:0x80200000"
./build/BloodyRoar2_Recompiled --disc "disc/...cue" --debug-port 19872
--headless --game game_1pad.toml + tools/arcade_fight_mash.py 19872:

- FIGHT REACHED frame=2341, frames sustained past 78,000.
- Fight phases cycled gs=8/gs=9 repeatedly (multiple rounds).
- cdrom_state mid-run: reading=1, read_msf advancing, last_sector
  lba=31624 mode 0xCA, irq_present_max_late=0, ring_starved=0.
- No wedge/livelock. PASS.
