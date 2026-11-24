# Regression sweep — session 5 (post-ee1d892)

Date: current master @ `ee1d892` (cd_ctx_save port + fnmap dedup + faithfulness
fixes). Disc: `disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue`.
Build: `./build/BloodyRoar2_Recompiled --headless`, debug ports 4430–4432.
All instances killed after each test; `ps` clean at end.

## Verdict table

| # | Mode / test | Driver | Result | Evidence |
|---|-------------|--------|--------|----------|
| 1 | ATTRACT/demo idle | passive watch, port 4430 | **PASS** | frame 10,110 → **64,146+** over ~2 min idle; `gs` cycled `06→03→05→06→03→05`; CD healthy throughout |
| 2 | row1 `(9b=0,9c=0)` | `tools/mode_monitor.py 4431 row1 240` | **PASS** | `MENU reached` t=3, `CONFIRMED gs=8` t=30, then gs cycled `3→4→5→6` for full 240 s monitor, frames → **77,608**; FINAL `gs=5 dat1=01`. Matches known-good: gs=8 flash → clean return to attract/title (`proof/non_arcade_mode_sweep.md` §5) |
| 3 | row3 / mode `(1,4)` | `tools/row3_twopad.py 4432 300` | **PASS** | tally `dat1` cycled `03→04→08→0b→03` continuously for 306 s, `sig=(1,4)` steady, `e4` per-side counters `00/00→03/02` each round; NO stuck tally, NO wedge |
| 4 | Battle to gs=8 (versus) | `tools/versus_p1only.py 4431` | **PASS** | `*** FIGHT (P1-only)` reached; `gs=08` sustained, `dat1(ph)` cycling `03/04/08/00/01`, `b4b6` round phases `00030003→00140005→00110001` cycling; P1 ctrl @`0x1f8001f0` and P2 ctrl @`0x1f8002f8` (stride 0x108) **mutating every sample**; frames → **120,879** at t≈395 s, no stall |
| 5 | CD health | `cdrom_state` on all instances | **PASS** | `irq_present_max_late=0` on every sample; `irq_present_late` ~5–10k cumulative but max-late clean; sectors flowing (`last_sector` advancing, `bfrd_set` growing, `reading` toggling 0/1, `stat` 0x02/0x22); `pending {cmd:0x09,active:0,phase:1}` = known stale completed-Pause residue, NOT a stuck command |

## Notes / non-findings

- **First row1 attempt FAILED confirm** (`CONFIRM FAILED`, dat1 bounced
  0/1/2 at menu) — but this was run **concurrently** with the ATTRACT
  instance on a second core-hungry headless process. Solo re-run passed
  cleanly. Judged CPU-contention flake in the wall-clock press pacing,
  not a regression. No code change indicated.
- **`arcade_fight_mash.py` on fresh boot looped in attract** (gs 3↔5↔6↔8,
  never reached `ph==8` fight in ~8 min). Consistent with documented
  known-state: menu top item is row1 `(0,0)` which flashes gs=8 and returns
  to attract; arcade proper is row4 `(0,3)` with the known CPU-select
  wedge family (`proof/cpu_input_architecture_select_wedge.md`,
  `non_arcade_mode_sweep.md` §5). Versus `(2,1)` used for the battle test
  instead — it is the verified-good combat path.
- `dat1=0xf0/0xf1` transient values seen during attract/gs-5 cycles —
  same as previous sessions, not new.

## Conclusion

**NO REGRESSIONS.** All five matrix items pass on `ee1d892`. The ~12-commit
session (CD ports `cd_ctx_save`/`cd_busy_wait`/`cd_timer_read`/
`cd_cmd_issue_b/c`, MIPS div faithfulness fixes, fnmap dedup) is runtime-clean
across attract, menu rows 1 & 3, versus combat, and CD streaming.
