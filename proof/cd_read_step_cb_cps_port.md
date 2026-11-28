# cd_read_step_cb_cps — dormant port enabled (0x801985C4, 39 rows)

## Root cause of the devin-s livelock

The dormant port in `game/recon_8019.c` was a *stripped* variant of
`cpst_801985c4` (`game/cd_irq_probe.c:63697`): instruction semantics were
verified identical block-by-block (all 39 CPS blocks, delay slots, chan
struct 0x801BCB80/+16..+52 fields, ring bump @0x801BCB88/90/94/A0,
flag 0x801BC7B4 bit0 sync_wait_c-vs-b select, tail `jalr *0x801BC7B0`
cb(a0=5|2, s1)) — the ONLY delta was the removal of every
`psx_slice_block` / `psx_icache_fetch` / `psx_cyc_step` /
`psx_cyc_load_word` call.

`recon_8019.c`'s header contract (proof/cps_fallthrough_cycle_wedge.md)
requires those calls verbatim in this band: device deadlines (CD IRQ
presentation, DMA, the chan+28 +1200 `cd_timer_read` polls) service
inside a piece ONLY through the cycle/load accounting. With all cycle
charges stripped, guest time did not advance inside the per-sector hot
path → the 0x801A1Fxx CD queue service spun forever (frames advanced,
gs/ph stuck 0) — exactly the observed livelock.

Note: `cd_result_wait_a/b_cps` in the same file are also stripped and
work — they are not on the per-sector deadline-critical path. This
function is.

## Fix

`cd_read_step_cb_cps` body replaced wholesale with the verbatim
`cpst_801985c4` transliteration (269 timing calls restored), keeping the
`uint32_t` CPS contract (`cpu->pc = target; return cpu->gpr[2];` on
jal/jr edges, `return cpu->gpr[2]` on slice_block early-out). No guest
constant or state-field changes were needed — devin-s's
0x801BCB78→0x801BCB88 ring-base fix was already correct.

## Wiring

- `game/clean_cps.map`: all 39 rows `cpst_801985c4` → `cd_read_step_cb_cps`
  (0x801985C4..0x8019881C).
- `python3 game/gen_fnmap.py` + `python3 game/gen_clean_table.py`
  regenerated `src/clean_table.c` (5398 wrappers, 22 skipped — all
  self-delegates, unchanged set).

## Verification (RelWithDebInfo build, headless, port 19868)

- `PSX_FN_FILTER=0x80010000:0x80200000 ./build/BloodyRoar2_Recompiled
  --disc "disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue"
  --debug-port 19868 --headless --game game_1pad.toml`
- `python3 tools/arcade_fight_mash.py 19868`
- **FIGHT REACHED frame=3811** (gs=8 ph=8 arcade fight).
- Frames sustain: 68k+ and climbing; game cycles fight → results →
  re-enters gs=8 ph=8 on its own (observed gs=9 ph=4 → gs=8 ph=8
  transition live via read_ram @0x1f800000).
- CD sectors actively reading throughout: `cdrom_state`
  `last_sector lba=31467 frame=35073` (fresh vs frame=35075),
  `read_cmd=0x1B`, `irq_present_max_late=0`.
- No 0x801A1Fxx queue-service livelock, no TestEvent spin.
