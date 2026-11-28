# GENERATED_ONLY restore: cpst_8019a814 / cpst_8019aabc / cpst_8019b308 + CPS call-drive fix

## What was restored

Three wedge-class GENERATED_ONLY addrs, rows recovered verbatim from
`git show bbbd6966^:game/clean_cps.map` (the bbbd6966 collateral retire):

- `0x8019A814` cd_chan_full_init — 3 rows -> cpst_8019a814
- `0x8019AABC` cd_chan_slot_set — 20 rows -> cpst_8019aabc
- `0x8019B308` cd_ext_init_once — 7 rows -> cpst_8019b308
  (0x8019B310 cd_dma_chan_init stays GENERATED_ONLY; a faithful
  cpst_8019b310 embedded-entry body now exists in game/cd_irq_probe.c
  for a future restore)

All three cpst_ bodies were already generator-current (`--timing` regen
produced identical output) — no transliteration changes were needed.

## Root cause of the restore wedge (NEW class-level bug found + fixed)

Restoring 0x8019B308 alone wedged boot at gs=00: BIOS B0/11 TestEvent
count spun ~1.36M, cd_intc_dispatch (still generated) hammered, and the
@0x801BDEA0 init-done flag was SET while the real init never ran.

Mechanism: `cd_ext_init_once()` / `cd_dma_chan_init()` in game/cd.c are
deliberate whole-body delegates via `call_guest4(0x8019b308/0x8019b310)`
(session-18 boundary fix). `call_guest4` (and call_guest, br2_call_clean,
cd_jal, bind_jal, br2_waypoint_jal, br2_jal, wb_jal — 8 synchronous
clean-first call sites) ran `clean_try_dispatch` exactly ONCE. A CPS
coroutine piece that ends in a tail jal/jr leaves the rest of the chain
pending in `cpu->pc`; the call helper then restored `cpu->pc` and
returned, truncating the callee after piece 1. For cd_ext_init_once that
meant: guard flag set, `cpu_critical_enter`/`cd_ext_table_init`/
`bios_tramp_b008` (OpenEvent)/`bios_tramp_b00c` never executed — the
TestEvent waiter spun forever.

The generated path never had this problem: `psx_dispatch_impl`'s
`addr = cpu->pc` tail loop re-dispatches every continuation until
`pc == stop_addr`.

## Fix — `br2_clean_call_drive` (src/clean_bridge.c)

After a successful `clean_try_dispatch` from a synchronous call site,
drive any pending CPS continuation to completion by single-stepping
`cpu->pc` through `psx_dispatch_game_compiled` (clean-first, generated
fallback — the same funnel the top-level dispatch uses) until the chain
returns to the entry `$ra` (or pc==0). Deliberately NOT
`psx_dispatch_call`: its stop_addr/sp_at_call contract assumes dispatch
entry before the callee prologue; driving mid-frame would misread the
live guest frame's sp as recursion.

Wired into all 8 synchronous call sites:
src/clean_bridge.c (call_guest, call_guest4, br2_call_clean),
game/cd.c (cd_jal), game/bind.c (bind_jal), game/battle.c
(br2_waypoint_jal), game/ev.c (br2_jal), game/recon_80138.c (wb_jal).

This is the likely systemic blocker behind several remaining
GENERATED_ONLY "wedge-class" addrs — any CPS callee with internal jals
reached via these helpers was silently truncated.

## Verification (headless, port 19796, game_1pad.toml)

- Build: `cmake --build build -j8` clean (6404 wrappers, 37 skipped).
- Run 1 (0x8019b308 only): boot -> attract gs=06, versus_fight_mash
  FIGHT REACHED frame 158575, sustained past frame 218187 across
  multiple rounds, zero wedge/FATAL. Post-run RAM: @0x801BDEA0=1,
  @0x801BDE38=0xF1000008 (OpenEvent handle stored — init completed).
- Run 2 (all three): boot -> attract gs=06 ph=01, versus FIGHT REACHED
  frame 48193, sustained past frame 197622 with repeated
  LEFT FIGHT/round cycles, zero wedge.
