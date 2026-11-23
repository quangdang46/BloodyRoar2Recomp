# gs=5 / match_loop wedge — DAT_1f800001 == 3 is a DEAD state; fighter region frozen

## TL;DR

A fresh arcade-mash run (port 9731, HEAD `233e13b`, `defer_switch=false`)
drives cleanly to `g_game_state=5` / `DAT_1f800001=3` and then sits there
**stable for 37k+ frames** (frame ~3895 → 41478+). The fighter/battle region
@ `0x1f800200` is **byte-frozen** (0/256 bytes changed in 2 s) — this is a
genuine wedge, not live combat.

`DAT_1f800001 == 3` is a **dead state** in `match_loop` (`game/game.c:1469`,
guest `0x80147034`): both the clean port and the generated body
(`br2eu_func_80147034`, `generated/SLES_017.22_full_13.c:13617`) only handle
DAT1 ∈ {0,1,2,0xf0,0xf1,0xff}. Value 3 falls through every branch to a bare
`frame_tick()` — an infinite no-op loop. Verified against generated blocks:
`DAT1==0xff`→exit (`block_801474FC`), `==2`→`block_80147450`, `<3`→0/1
handlers, `>=3`→`block_801470F0` which only special-cases `0xF0`/`0xF1`
(`block_80147494`/`block_801474CC`); 3 lands on `block_80147104`→frame_tick.

## Evidence

- `gs=5`, `DAT1=3`, `sub(0x9c)=0`, `09b=0`, stable across minutes.
- CD idle (`cdrom_state`: `reading=0`, `stat=0x02`, `sector_available=1`).
- `decomp_trace` frozen at seq 2532 (`heapA_w_menu` last write) — asset
  decompression complete, not stalled.
- `frame_divisor_get()` byte `0x1f800019 == 0` consistently — not a divisor
  gate.
- `pad_status`: slot0 `connected:true`, `analog:false`, override live
  (`override=[4091,-1]`, `override_frames` counting). Input injection works;
  DOWN/CROSS/START do **not** move DAT1 — consistent with a dead state, not
  a missing input.
- `clean_trace` tail shows the per-frame CD-pump cycle
  (`cd_timeout_arm`→`0x8019A7FC`→`0x80192E78`→`0x80193020`) still running —
  `frame_tick`/CD maintenance alive, fight sim dead.

## DAT1 write history (gwatch `0x1f800001` len=1, armed @ frame 1007)

DAT1 cycled the attract-demo lifecycle ~5× then wedged:

```
0→1 (0x801265C4 f1595)  1→2 (0x80135800 f1863)  2→0xf0  0xf0→0xf1  0xf1→0xff  0xff→0
   ... repeats (0→1→2→0xf0→0xf1→0xff→0) ...
seq33 f~3800: 1→0     seq34 f3895: 0→3   <<< the wedge write
```

After seq34 (frame 3895) **no game-code DAT1 write for 37k+ frames** — only
BIOS/IRQ-context scratchpad noise (`store_pc` 0x8019A48C/0x8019A5A4/0x80192FA4,
all `func=0x29CC` stale metadata). The `0→3` writer's recorded `store_pc`
(`0x801931D8`) is a `sw s0,16(sp)` **stack store inside `cd_queue_drain`** —
i.e. gwatch `store_pc` is unreliable here; the true writer is unattributed.

## Interpretation

- gs=5 = `match_loop` = the match loop (attract demos AND real fights share
  it; see `proof/gs5_matchloop_combat_verified.md`). Its DAT1 machine is the
  match *lifecycle*: 0 boot-select → 1 running → 2 ending → 0xf0/0xf1
  shutdown → 0xff return. DAT1=3 is outside that machine.
- The repeated `0→1→2→0xf0→0xf1→0xff→0` cycles are attract demo matches
  completing. At frame 3895 DAT1 went `0→3` — a value no match_loop branch
  handles — and the fighter region froze.
- DAT1 is a *shared* scratchpad byte multiplexed across state machines
  (BFSM uses 0–0xf0, `player_name_entry` uses 3/4/0xf0/0xf1, match_loop uses
  0/1/2/f0/f1/ff). A `3` here is either (a) written by the fight engine
  inside the DAT1==1 branch and not restored before the top-level dispatch
  re-reads it, or (b) written by an external task/callback that owns a
  DAT1==3 sub-state match_loop isn't meant to drive.

## Open question / next step

Identify the true writer of `DAT_1f800001=3` at ~frame 3895 and what should
consume it. Candidates:
- the fight engine reached from `match_loop` DAT1==1 (`match_end_check`,
  `battle_frame_tick`, round/KO substates) setting DAT1=3 without restoring;
- a scheduled task/EV handler (script-VM op, `task_stream_tick`) writing a
  phase/substate match_loop should delegate to;
- a missing clean-port case: match_loop's top-level dispatch may need to
  route DAT1 values 3–0xef to the fight engine exactly as the generated
  `block_80147104` fall-through does on real HW — needs instruction-level
  confirmation that `block_80147104`→`block_801474E4` is truly a no-op for 3
  and not a fight-engine call the clean port dropped.

Do **not** hardcode a phase write. Find the faithful owner of DAT1==3.
