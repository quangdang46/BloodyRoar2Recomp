# cd_kobj_param_copy clean-CPS restore (0x8019E5CC)

## What

First GENERATED_ONLY addr restored to clean-CPS dispatch. `cd_kobj_param_copy`
(guest `0x8019E5CC`, clean body `game/w5_e00.c`) is a 3-block, 52-byte pure
copy loop: `v0=0xDF80`, copy 28 words `0x801AE430..0x801AE4A0 -> 0x0000DF80`,
`jr $ra`. No calls, no BIOS/kernel boundary, no fn-ptr — the smallest
self-contained coroutine cluster in the GENERATED_ONLY set.

## Why it was excluded

Commit `bbbd6966` retired ALL `clean_cps.map` rows in
`0x80194000–0x801A8000` after `cpst_8018cfdc` (cd_file_load_sm) was proven to
corrupt flow (~frame-1467 `DISPATCH FATAL 0x00120201`). The region retirement
was collateral: it also deleted the already-verified w5_e00/w5_e01 faithful
CPS transliterations, and `GENERATED_ONLY` was added so the fnmap-named
entries would not flip back to flag-0 wrappers.

`cd_kobj_param_copy` is provably self-contained: dispatch table routes
exactly `{0x8019E5CC, 0x8019E5E0, 0x8019E5F8}` -> `br2eu_func_8019E5CC` and
nothing else. Its only yield point is the copy-loop
`psx_check_interrupts_at(0x8019E5E0)`; resume lands on the registered
continuation row and the body's `_cont` switch re-enters `block_8019E5E0`.

## Verification (insn-level)

Generated `br2eu_func_8019E5CC` (SLES_017.22_full_48.c:9513) diffed against
`game/w5_e00.c:296`: identical ops — `v0=0xDF80`, `t2=0x801AE430`,
`t1=0x801AE4A0`, loop `lw v1,0(t2); sw v1,0(v0); t2+=4; v0+=4 (delay slot);
bne t2,t1 -> 0x8019E5E0`, exit `jr ra` via `psx_check_interrupts_at(ra)`.
The clean body preserves every yield/check point; only instrumentation
(cycles/icache/cosim/debug/PGXP) is stripped, per the established CPS
transliteration contract.

## Runtime proof (RelWithDebInfo build, port 19780)

- Boot -> attract `gs=6`, frames ~280/s for 60k+ frames, no wedge.
- `clean_stats`: `0x8019E5CC` hit n=1 — clean body executed during the
  init-time GPU-state patch sequence (matches the frame-172 execution
  recorded in `682d54ab`). Continuation addrs `0x8019E5E0/F8` correctly
  absent from hit counts (reached via in-body `goto`, not dispatch — the
  28-word loop completes inside one dispatch absent a mid-loop IRQ).
- `dispatch_stats`: `static_hits=21.5M, miss_total=0`.
- `versus_fight_mash.py 19780`: versus fight reached (`gs=8 ph=8`),
  rounds completed (`LEFT FIGHT gs=8 ph=0`), sustained combat to
  frame 152k+ with CD `seq` climbing past 12.6M and `reading=1` —
  ~4.5 min of CD-streaming-heavy versus, zero DISPATCH FATAL / wedge.

## Change surface

- `game/clean_cps.map`: +3 rows (`8019e5cc/8019e5e0/8019e5f8
  cd_kobj_param_copy`) — entry + both continuation addrs, CLEAN_ENT_CPS.
- `game/gen_clean_table.py`: `0x8019e5cc` removed from `GENERATED_ONLY`
  (46 remain).
- `src/clean_table.c`: regenerated (6325 wrappers, 63 skipped).
