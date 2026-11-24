# Post-d11b51d runtime verification — row5 / attract / row3

Date: session after `d11b51d` (menu.c:3961 inverted `+0x24` compare fix),
`f1d0894`/`800e54c` (foe-NULL guard removal), `ad5f4a7` (div-by-zero LO).
Disc: `disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue`.
Build: current master, `./build/BloodyRoar2_Recompiled --headless`.

Instances: 9941=row5, 9942=attract, 9943=row3 (all still running at report
time; frames advancing on all three).

---

## 1. ROW5 `(9b=3, 9c=2)` — WORKS (d11b51d verified)

**Verdict: the select-screen handshake deadlock is FIXED.** Both per-side
select records' `+0x24` promoted `0 → 1 → 2` under simulated pad input, the
select exited, and the mode-3 flow ran through battle states.

### Evidence

- `tools/mode_monitor.py 9941 row5 180`: reached `gs=8 sig=(3,2)` select;
  DAT1 cycled `0/6/3/4/8` (`8 = '\b'` = BFSM battle case) — i.e. it got
  PAST select into battle, then returned to the mode loop.
- Select records live at `*(u32*)0x801f10bc - 0x20 + 0x48`, stride `0x30`
  (`select_screen_tick` frame, `game/menu.c:3195-3220`). Live dump while
  mashing CROSS on both slots:

  ```
  rec0 @0x801fff08: +8=0 +10=1 +24=0→1→2 +2c=0→8
  rec1 @0x801fff38: +8=0 +10=1 +24=0→1→2 +2c=0→7/5
  ```

  Both `+0x24` reached `2` (the exit predicate
  `iVar4==2 && local_94[0]==2 && local_94[0xc]==2` at menu.c:3283 fired).
  After exit the frame was repurposed (records overwritten by other
  structs — expected, `br2_guest_frame` window reuse).
- Pad-bit merge confirmed live: `fighter_spawn_setup` mode-3
  (`game/menu.c:3956-3983`) now writes merged pad bits into the first
  `+0x24==0` record's `+8` each frame (observed `rec0+8 = 0x4000` flicker
  during DOWN/CROSS presses; decoded dpad lands at bits 12-15 → cursor
  steps, CROSS→bit6 `0x40` → `& 0xe0` confirm).
- Post-select: `gs=8 dat1=06 sig=(3,2)` steady, `frame` 152k→171k+,
  `bce_calls` ~350/s growing (per-frame `fighter_spawn_setup`→
  `battle_config_eval` loop = live select/round machinery, not frozen),
  `pbd_phase=0x0D`. Mode `(3,2)` appears to be a CPU/demo battle that
  loops select→battle→select; full match termination not observed in the
  window but the specific deadlock is gone.

---

## 2. ATTRACT — WORKS (f1d0894/800e54c verified)

**Verdict: NO freeze.** Instance 9942 idled ~35 min, `frame` reached
**184,185+** and still advancing — 3.5× past the old ~52,552 demo-battle
freeze. `gs` cycled `3→4→5→6→3` (title/attract/demo loop) continuously;
sig transiently `(3,0)` during demo battles. The `motion+0xa4` garbage
livelock in `ev_stream_dispatch` is gone.

Monitor log: `/tmp/mon_attract.log` — 80+ clean attract transitions,
no `FRAME FROZEN`/`STATE STALL` dump.

---

## 3. ROW3 `(9b=1, 9c=4)` — STILL WEDGES; root-cause gate identified

**Verdict: WEDGES** at `gs=8 DAT1=0x0b sig=(1,4)` — `hud_state_machine(1)`
never returns (frame still advances ~40/s via internal `frame_tick`;
`bce_calls`/`pbd_calls` frozen at 2124/1059). Same signature as
`proof/post_fix_mode_sweep.md` §5.

### Live memory proof (port 9943, wedged steady-state)

| Cell | Value | Meaning |
|------|-------|---------|
| u16 @0x1f800092 | `0x010e` | `&0xf00=0x100` → `sp58=1` → `sp20=0x1f8000e8`, `sp24=*(sp20+0x2c)=0x1f8001f0` |
| u8 @0x1f8001cc (`sp20+0xe4`) | **0x03** | P1 record +0xe4 |
| u8 @0x1f8002d4 (`sp24+0xe4`) | **0x02** | P2 record +0xe4 |
| u16 @0x1f80008c / @0x1f80008e | `0xffff` / `0xffff` | `display_page_commit_a/b` done — **not** the gate |
| u16 @0x1f80006c/0x6e (held), 0x68/0x6a (edge) | live, update on press | pad path healthy |
| u16 @0x1f8000a4 | `0x0000` | announce index |
| u16 @0x1f800030/0x40 | `0x0040` / `0x0000` | `g+0x40==0` → draw678→after678 direct |

### Exact stall gate

`hud_state_machine` (`game/save.c:5082`) is one call whose `for(;;)`
only exits via `fp==0xff` (set only by `fp=0xf0`→`deferred_cmd_post(0x203)`
in `mode0body`/`mode2sel`/`case2`). With `mode==1`:

1. First iteration (`fp==0`, `mode==1` branch, save.c:5145-5158):
   `b5 = sp20[0xe4] = 3`, `b4 = sp24[0xe4] = 2` → `b4 < b5` → **`sp48 = 0`**
   (verified faithful vs generated `block_8016E420`/`8016E434`:
   `lbu +0xE4(s3)` vs `lbu +0xE4(s4)`, `sltiu` → taken stores nothing,
   not-taken stores `sp48=1` at `0x8016E44C`).
2. `fp→1` → `case1` → `draw678` (`g+0x40==0` → `after678`) → all gates
   pass (`g+0x8c<0`, `g+0x8e<0`, `sp50==sp54==0`, `(r+0x6c)&0xf==0`) →
   `afterdraw` pad-edge gate (`u16@(r+0x68) & 0xe0`/`&0x800`,
   save.c:5419-5441) → on edge `modechk` → **`mode1body`:
   `if (sp48 == 0) goto poll;` (save.c:5496) — PERMANENT.**
   `sp48` is a call-local latched once at `fp==0`; nothing inside the
   loop can set it. Even a perfect pad edge only reaches `mode1body`,
   which then spins at `poll` forever. **The terminal gate is `sp48==0`,
   caused by `sp24[0xe4]=2 < sp20[0xe4]=3`.**

Without pad edges it spins one step earlier (the `afterdraw` edge gate),
but that is cosmetic — the call cannot exit either way.

### What `+0xe4` is and who writes it

`+0xe4` on the `0x1f8000e8`/`0x1f8001f0` fighter-side records = **per-side
round-win counters** (u8):

- incremented in `post_battle_dispatch` (`game/menu.c:7160-7161`,
  versus round-win bookkeeping, case-11 win compare vs
  `MB8(MB32(s1+0xc0)+4)` limit at menu.c:7889) and `vs_confirm_c`
  (`game/menu.c:7824`, `7848`);
- cleared ONLY by `scratchpad_pair_init` (`game/game.c:2781-2796`,
  guest `0x8016fbfc`), invoked on the BFSM `caseD_2` battle-init path
  (`game/menu.c:701`) and post-battle (`game/menu.c:9535`).

The `9c==4` path (`game/menu.c:660-677`: `scene_entry` →
`select_screen_tick(2)` → `countdown_display_sm(0)` → `hud_state_machine(0)`
→ BFSM `case '\v'` → `hud_state_machine(1)` at menu.c:961) **never calls
`scratchpad_pair_init`** — so asymmetric win counters (3 vs 2, plausibly
left by CPU/demo versus rounds earlier in the attract cycle; attract
instance 9942 showed `+0xe4=0/0` through its sampled window but its demo
mix differs) persist into the announce and wedge it.

### Assessment

- The clean port is faithful (generated block-level diff matches).
- This is a **data/upstream issue, not a mistranslation**: the guest would
  wedge identically IF `+0xe4` were asymmetric at this entry on real HW.
  On real HW the counters are presumably equal (0-0 or tied) because the
  versus-round writers that produced 3-2 either don't run for mode
  `(1,4)`, or an earlier reset (`scratchpad_pair_init`/battle init)
  zeroes them on the real path. The missing writer to investigate:
  whichever guest function is supposed to clear/equalize `+0xe4` before
  `hud_state_machine(1)` on the `9c==4` route — candidates:
  `scene_entry` (`0x80010000` arg), `select_screen_tick(2)` tail,
  `countdown_display_sm`, or `battle_state_init`/`battle_init` not
  reaching the pair-init on this mode. Trace who last wrote
  `0x1f8001cc`/`0x1f8002d4` (watchpoint on those two bytes across the
  `9c==4` entry) to name the writer and the expected reset.

---

## Commands run / artifacts

- Launched: `./build/BloodyRoar2_Recompiled --disc "disc/Bloody Roar 2 - Bringer
  of the New Age (Europe).cue" --debug-port {9941,9942,9943} --headless`
- Monitors: `tools/mode_monitor.py 9941 row5 180`, `9942 attract 240`,
  `9943 row3 240` → logs `/tmp/mon_{row5,attract,row3}.log`, diag dumps
  `/tmp/mode_994{1,2,3}_*.json`.
- Instances left running (PIDs 61606/61754/61998) for follow-up; kill with
  `kill 61606 61754 61998` when done. Other pre-existing instances on
  9931/9960/9971 were not touched.
