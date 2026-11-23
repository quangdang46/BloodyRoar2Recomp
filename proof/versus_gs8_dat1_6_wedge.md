# Versus `gs=8` / `DAT_1f800001==6` wedge — characterization + current-build verdict

Date: runtime investigation on a fresh headless instance (debug port 9760,
`game_1pad.toml`, current `master` @ `060bef4`). Read-only; no source edits.

Supersedes/refines `proof/versus_dat1_6_cd_timer_spin.md` (2026-11-22 04:08),
`proof/versus_phase_machine_deadlock.md` (03:13),
`proof/dat1_writer_audit.md`, `proof/spawn_phase8_writer_audit.md`.

## TL;DR

**The wedge does NOT reproduce on the current build.** Versus mode
(`battle+0x9b=0x02`, `+0x9c=0x01` — verified live) flows
title → versus select → fight and sustains the match loop: `DAT1` cycles
`8→6→7→8` per round plus the `8→1→0→6→f0→f1→ff→0→2→3→4→6→7→8` intermission
sequence, for 15+ minutes / dozens of rounds under `tools/versus_fight_mash.py`.

The documented wedge was a **clean-port defect, already fixed**: unreserved
guest stack frames in `select_screen_tick` (and `battle_frame`) let nested
calls overwrite the two 0x30-byte per-player select records at `sp+0x40`,
clobbering the `+0x10`/`+0x24` confirm-state fields so the exit predicate
(`local_94[0]==2 && local_94[0xc]==2`) could never be satisfied → faithful
wait on `DAT1==6` became permanent. Fix: commit `8d84f45` (2026-11-22 04:17,
**9 minutes after** the wedge doc's runtime capture) + follow-ups `002997d`,
`5034ff2`, `add49c9`, `1fd2a9b`. See `proof/vs_battle_entry_frame_fix.md`.

`DAT1==6` in `gs=8` is **not** a dead state and **not** primarily the
`battle_flow_state_machine` case-6 block the earlier doc guessed ("caseD_6"):
it has **two distinct owners** (below). The versus-relevant one is
`select_screen_tick`'s character-select wait — a faithful predicate on
both players' confirm records.

## 1. Reproduction

```
./build/BloodyRoar2_Recompiled --game game_1pad.toml \
    --disc "disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue" \
    --debug-port 9760 --headless
python3 tools/versus_fight_mash.py 9760
```

Navigation (from `tools/versus_fight_mash.py`):
- `gs=5 ph=1` → `press START slot0` (title confirm)
- `gs=5 ph=2` → `press DOWN slot0` ×until `sig==(2,1)` (selects the VERSUS
  menu item, not arcade top item), then `press START slot0`
- `gs=8 ph=6` → `press CROSS (0xBFFF) slot0` and `slot1` every ~1 s
- `gs=8 ph=7` → `press START` both slots; `ph=8` = fight

Result this run: `FIGHT REACHED frame=8725`, then continuous versus match
cycling (`LEFT FIGHT`/re-entry every ~1600 frames ≈ 8 s at ~150–210 fps).

Note: first launch attempt wedged in early OpenBIOS (repeated `VECWR` at
`0x801AA260`, debug port accepted TCP but never answered) — a flaky boot,
unrelated; clean relaunch booted normally.

## 2. Wedge-state dump (live `DAT1==6` catch, frame 100645)

Caught mid char-select; all values HEALTHY (this is what the state looks
like when the predicate is merely waiting, not wedged):

| Cell | Value | Meaning |
|---|---|---|
| `0x1f800000` byte0 `gs` | `0x08` | match flow |
| `0x1f800001` `DAT1` | `0x06` | select_screen_tick char-select |
| `0x1f80009b` / `0x9c` | `0x02` / `0x01` | versus mode signature |
| `u16 @0x1f800092` | `0x0300` (later `0x030d`) | both-confirmed class / round sub-state advancing |
| `u16 @0x1f8000b4` / `0xb6` | `0x0300` / `0x0300` | BOTH phase words armed (the old arcade wedge had `b6=0x0000`) |
| `0x1f800200` fighter region | live pointers (`0x800dc608`, `0x801f5308`, …) | populated, not frozen |
| `cdrom_state` | `seq=1832829` advancing, `reading=1` during loads, `irq_flag=0x00`, `response_count=1`, `sector_read_pos=2060/2340` (benign EDC tail) | CD healthy; later sample `reading=1`, pending `0x09` inactive leftover |
| `pad_status` | slot0+slot1 `connected:true`, `buttons=0xFFFF`, `override=[-1,-1]` | both human pads live |
| `irq_state` | `i_stat=0`, `i_mask=0xD`, `IEc=1 IM2=1` | idle-normal |
| `frame` | advancing ~150 fps host | not frozen |

Contrast with the documented wedge (`versus_dat1_6_cd_timer_spin.md`):
`seq` frozen, `pending.cmd=0x09 active=0`, `response_count=1` persisting,
INTC in-service flag `@0x801bcd18` stuck `0x0001`, `i_stat` bit2 never
pending. None of that is present now.

## 3. What `DAT1==6` actually is — two owners, one shared byte

`DAT_1f800001` is a shared phase byte dispatched by **three** different
clean-C loops. In versus (`9b=2`), `battle_flow_state_machine`
(`game/menu.c:551`, guest `0x8016fcc4`) case 1 routes to
`select_screen_tick(0)` (line 623), NOT `battle_frame` (that is the
`9b==0` arcade path, line 610-611).

### Owner A — `select_screen_tick` char-select (the versus wedge site)

`select_screen_tick` (guest `0x8014f260`, `game/menu.c:3123`) runs a
`do{}while(1)` frame loop with its own `DAT1` sub-states:

- Entry writes `DAT1=0` (line 3165); when `intro_movie_state` succeeds it
  sets `bVar1=6` → `DAT1=6` (lines 3496, 3509-3510; generated store block
  ≈ `0x8015060c`, `generated/SLES_017.22_full_16.c`).
- `if (DAT_1f800001 == 6)` block (line 3167): calls
  `fighter_spawn_setup(...,fp=1)` then the per-player confirm logic —
  cursor step on `rec+8 & 0x1000/0x2000/0x4000/0x8000`, confirm on
  `rec+8 & 0xe0` or `& 0x800` (lines 3196-3224) →
  `select_confirm_handler` → `rec+0x24=1`, `rec+0x2c=8`.
- `rec+0x24` promotes `1→2` only when the *other* player's bookkeeping
  (`local_a8`/`local_94` = record-array `+0x10`/`+0x24` aliases) is
  positive (lines 3174-3191).
- Exit: `iVar4==2 && local_94[0]==2 && local_94[0xc]==2` → `DAT1=0xf0`
  (line 3227-3228) → `0xf1` → `0xff` → restore `DAT1=0`, `return 0`
  (lines 3502-3527).

**This is a faithful wait**: `DAT1==6` persists exactly as long as at
least one player's confirm record hasn't reached state 2. An idle
second pad legitimately dwells here forever on real hardware too.

### Owner B — `battle_flow_state_machine` case 6 (transient, self-advancing)

Outer FSM `case '\x06'` (`menu.c:746-767`): one-shot setup block
(`match_setup_apply`, `battle_state_init`, `per_side_setup`,
`action_enter_fwd`×2, `fighter_anim_table_init`,
`sprite_anim_chain_build`, `frame_tick`) then unconditionally
`DAT1=7` (line 767) → case 7 → `DAT1=8`. Writers of the `6` it consumes:
`case5` fallthrough (`menu.c:745`), `battle_sides_commit`
(`0x80155c14`, `menu.c:4883-4885`, writes 6 when `s0&0x100` — post-battle
re-arm), `fighter_spawn_setup` `fp!=0` commit (`0x801487ac`,
`menu.c:~3810`, `s3[1]=6`). Never a wait — always advances next iteration.

### Owner C — `battle_frame` (arcade only, `9b==0`)

`battle_frame` (`0x8014cdd0`, `menu.c:8099`) has its own `DAT1` dispatch
(states 3/4/8 etc.). Not active in versus.

## 4. `gwatch` attribution of the `DAT1=6` writers (live, port 9760)

`gwatch 0x1f800001` captured 87 writes over ~4 versus cycles. The
`store_pc` field is **not** a reliable semantic writer here — it reports
the dispatch/interrupt-context PC (`0x8019A48C`, `0x8019A5A4`,
`0x80192FA4`, and once `0xBFC057D4` = BIOS, i.e. an IRQ-side store),
exactly the caveat in the task. But the **guest `sp` + transition
signature** cleanly separates the owners:

| seq pattern | sp | transition | owner |
|---|---|---|---|
| `00→06` then `06→f0→f1→ff→00` over ~5 frames | `0x801FFEC0` | char-select entry/exit | **select_screen_tick** (deeper frame — `br2_guest_frame(0xf8)` reservation) |
| `04→06` or `08→06` then `06→07→08` same cycle (~1.4 M cyc) | `0x801FFFC0` | post-confirm / post-battle re-arm | **battle_flow_state_machine** case5→6 / `battle_sides_commit` |

Example (frames 77568-77573): `00→06` @sp `0x801FFEC0` → `06→f0` →
`f0→f1` → `f1→ff` → `ff→00` → `00→02` → `02→03` — the full healthy
char-select pass in ~5 frames under mash auto-confirm.

Static writer cross-check (`proof/dat1_writer_audit.md` table, confirmed
against `game/menu.c`): `6` is written by `select_screen_tick`
(`menu.c:3496` via `bVar1`), `battle_flow_state_machine` (`menu.c:745`),
`fighter_spawn_setup` (`menu.c:~3810`, fp=1 commit), `battle_sides_commit`
(`menu.c:4883`), plus unrelated menu writers (`match_loop`,
`system_config_menu_*`, `results_scoreboard`, …).

## 5. Verdict: faithful gating vs defect

- **Current build: faithful.** `DAT1==6` dwell = char-select confirm wait;
  exit predicate is `both rec+0x24 == 2`, driven by real pad edges on both
  slots (`rec+8 & 0xe0` attack bits or `& 0x800`). Versus P2 is human
  pad-2 (mode `2,1`), not CPU — no AI synthesizer exists or is needed.
- **Documented wedge: clean-port defect, already fixed.** The
  `versus_dat1_6_cd_timer_spin.md` capture (04:08) predates fix `8d84f45`
  (04:17) by 9 minutes — same pre-fix build. Unreserved guest frames in
  `select_screen_tick` (`guest_at(guest_sp()-N)` without
  `br2_guest_frame`) let every nested call's frame overwrite the
  `sp+0x40` player records and `sp+0x18` param block → `+0x10`/`+0x24`
  confirm fields clobbered → predicate never satisfied → `DAT1==6`
  forever. Identical bug class in `battle_frame` (`sp+0x130/+0x134`
  argblock pointers → P2 `+0xb6` never armed). Follow-up fixes:
  `002997d` (global sp-relative locals), `5034ff2` (`battle_init`),
  `add49c9` (`fighter_spawn_setup` commit dispatch `6→5`),
  `1fd2a9b` (u16 read of sim/pause gate `0x1F800082` — had frozen the
  channel-script VM at phase `0x300`).
- **The "CD timer spin" signature** (pending `0x09` never re-issues,
  INTC in-service stuck, response FIFO not draining) was real on that
  build but is **not present now**: through dozens of versus cycles
  `cdrom_state.seq` advances past 1.8 M, `reading=1` during loads,
  `irq_flag=0`, B0/0x0B TestEvent count is cumulative-normal (no delta
  spin sampled). Whether it was an independent CD defect or downstream
  of the clobbered-frame wedge is unresolved historically — the CD
  record-machine fix series (`2975467`, `2894c11`, `a729431`, `c11c718`,
  `596e5c3` `cd_response_pump`→generated, `b7cce7a`) all landed
  2026-11-21/22 in the same window. No residual CD stall observed.

## 6. Recommended faithful next step

**None for this wedge — it is fixed; keep it as a regression case.**

- Do NOT force `DAT1` or skip states: `DAT1==6` without pad-2 confirm is
  correct hardware behavior. To distinguish wedge-vs-wait at runtime,
  inspect the select records (`rec+0x24` at the `select_screen_tick`
  frame, `sp`≈`0x801FFEC0` region / `0x801f10bc` cell) and pad-edge
  fields `battle+0x68/+0x6a`, not `DAT1` alone.
- If versus `DAT1==6` ever dwells again: check (a) `pad_status` slot1
  connected + `override` state, (b) `rec+8` button flags reaching the
  record (pad→`attr_word_decode`→`pad_state_refresh` chain), (c) the
  `+0x10`/`+0x24` promote predicates at `menu.c:3174-3191`, (d) whether
  the dwell is Owner A (sp≈`0x801FFEC0`, exits via `0xf0`) vs Owner B
  (sp≈`0x801FFFC0`, should self-advance in one iteration — a dwell there
  WOULD be a defect).
- The still-open versus-adjacent issue from `cpu_phase_wedge_disconnected_pad.md`
  (arcade CPU-opponent confirm source) is unrelated to this wedge.

## Evidence commands used

`frame`, `read_ram`/`mem_words` on `0x1f800000/0x90/0xb0/0x200`,
`gwatch`/`gwatch_reset`/`gwatch_dump` on `0x1f800001` (87 entries,
`/tmp/gwatch_9760.json`), `cdrom_state`, `pad_status`, `irq_state`,
`bioscall_dump`, `tools/versus_fight_mash.py` (port 9760),
`git show 8d84f45` + `proof/vs_battle_entry_frame_fix.md`.
