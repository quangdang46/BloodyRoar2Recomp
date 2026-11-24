# Post-fix mode sweep — versus & arcade RESTORED, attract hard-freeze found, rows 3 & 5 still wedge (new signatures)

Session: 8 parallel headless instances on debug ports 9960–9967 (+9970 attract
repro), disc `Game.cue` (SLES-01722), current master `b8993bd`. Harness:
`tools/versus_p1only.py`, `tools/arcade_p1only.py`, and a new generic driver
`tools/mode_monitor.py` (menu-row nav by DOWN-count, START/CROSS confirm,
state sampling, wedge auto-dump). Corrected active-low pad map used:
UP `0xFFEF` DOWN `0xFFBF` LEFT `0xFF7F` RIGHT `0xFFDF` SELECT `0xFFFE`
START `0xFFF7` CROSS `0xBFFF` CIRCLE `0x7FFF`.

> Launch note: firing 8 instances simultaneously races the shared mod-state
> file (`cannot publish state`) — stagger launches ~3 s apart.

## Verdict table

| Mode | Menu row / sig | Verdict | Evidence |
|------|---------------|---------|----------|
| ATTRACT / demo | — | **WEDGES (hard freeze)** | instance 9960 froze solid in demo versus fight; see §1 |
| VERSUS | row 2 `(9b=2,9c=1)` | **WORKS** | 700+ s sustained combat, `b4/b6` phase cells cycling, rounds completing (§2) |
| ARCADE | row 4 `(9b=0,9c=3)` | **WORKS** | repeated `gs=8` fight → `gs=9` post-match → `gs=5` → next battle cycles (§3) |
| Menu row 1 | `(9b=0,9c=0)` | **WORKS** | `gs=8` flash ~2 s → clean return to attract cycle (§4) |
| Menu row 3 | `(9b=1,9c=4)` | **WEDGES (new, deeper)** | now reaches `gs=8 DAT1=0xb`, parks inside `hud_state_machine(1)` poll forever (§5) |
| Menu row 5 | `(9b=3,9c=2)` | **WEDGES (WEDGE-B persists)** | select-screen handshake deadlock; pad edges now land but `rec+0x24` never seeds (§6) |
| Options row 6 | `gs=11 sub=4` | **WORKS** | enter → DOWN-nav → exit verified (§7) |
| Options row 7 | `gs=11 sub=3` | **WORKS** | same (§7) |

---

## 1. ATTRACT / DEMO — HARD FREEZE in demo battle (regression-class finding)

Instance 9960, zero input. Cycled attract normally, then entered a demo
battle (`gs=8, DAT1=0x08, sig=(2,1)` — versus-config CPU fight) and
**froze solid at frame 52552**.

Signature (all counters static over >60 s sampling):

- `frame` = 52552 frozen; `freeze_check.total_checks` = 1,335,687 frozen;
  `sp_ring.total` = 267,018 frozen; `bioscall_dump` static.
- `cdrom_state`: idle, `pending {cmd:0x09, phase:1}` (the benign idle state
  seen on healthy instances too), last CD activity frame 52549.
- `mdec_state`: `busy=0`, `decode_macroblocks=17120`.
- `irq_state`: `i_stat=0`, `i_mask=0xD` — no pending IRQ.
- Process still ~40 % CPU: the debug server stays alive because
  `ev_stream_dispatch` calls `debug_server_poll()` every 1024 iterations
  (`game/resolve.c:3137`) — **host-side socket service only, no guest
  interrupt/resume semantics**.

Root mechanism — **same bug as `proof/versus_eventvm_actsel_deadlock.md`**:

- `decomp_trace` tail: endless `evop` records (tag 60 =
  `ev_stream_dispatch` at `game/resolve.c:3127`) cycling ops
  **1 → 5 → 2** (`altstream → actsel → hitstun`) at stream pcs
  `0x800DDB92/96/98` — the documented 3-opcode closed cycle.
- Live guest RAM at the motion record `0x801F5440` (confirmed: `+0x84` =
  `0x800DDB92`, the looping stream pc):
  - `+0xa0 = 1`, `+0xa2 = 0x00f8`, **`+0xa4 = 0x859f` (negative garbage)**,
    `+0xa8 = 0x80018904` (byte-identical to prior hung runs),
    `+0xac = 0x800E83D8` (per-run heap).
- `motion+0xa4` negative ⇒ `ev5_action_select` case-0 count check fails ⇒
  actsel returns `pc+4` without `fighter_anim_advance` ⇒ the three handlers
  never return 0 and never mutate the pc ⇒ `ev_stream_dispatch`'s
  `for(;;)` spins forever **with no `psx_check_interrupts_at`**, so guest
  IRQs/coroutines starve → hard freeze (guest cycle counter stops at
  35778673356).

`d20db90` fixed the versus path's `.ACT` relocation ordering, but the
attract demo battle still produces a corrupt `.ACT` reaction-table count
(`+0xa4`). Repro attempt on fresh instance 9970 was still in the gs=3→6
attract cycle at t≈240 s (demo battle not yet reached) — watch
`/tmp/drv_9970.log`.

## 2. VERSUS `(9b=2,9c=1)` — WORKS (was wedged pre-d20db90)

`tools/versus_p1only.py` on 9961: menu nav → select (CROSS both slots,
START gate) → `gs=8 ph=8` fight at ~t=30 s, then **sustained combat for
700+ s** with `b4/b6` cycling through fight phases (`0003/0003`,
`0014/0005`, `0011/0001` …) — rounds resolving and re-entering, matching
the proven-versus signature. No wedge dumps, no TestEvent spin, CD idle
`pending 0x09` only. **d20db90 confirmed effective.**

## 3. ARCADE `(9b=0,9c=3)` — WORKS

`tools/arcade_p1only.py` on 9962: sig-nav to `(0,3)` → `gs=8` select →
`*** FIGHT t=125 ***` → combat → `gs=9` post-match → `gs=5` → next battle.
Observed **multiple complete battle cycles** (t≈200, 456, 552 …) — CPU
opponent live, rounds finishing. **The old `select_phase_gate`/`b6=0`
wedge is gone.**

## 4. Menu row 1 `(9b=0,9c=0)` — WORKS

`mode_monitor.py 9963 row1`: confirm → `gs=8 dat1=00` for ~2 s → clean
return `gs=5→6→3→4→5` attract cycle, repeated 3+ times. Matches prior
"clean return" verdict.

## 5. Menu row 3 `(9b=1,9c=4)` — WEDGES, NEW deeper signature

`mode_monitor.py 9964 row3`: confirm → BFSM `9c==4` case
(`scene_entry` + `select_screen_tick(2)` + `countdown`/`hud_state_machine(0)`)
→ **got past the old scene_entry stall** (e9be0fe/d20db90 progress: the
`pending-0x09` CD starvation did NOT recur; CD went idle cleanly after
last sector frame 8618) → battle-init chain ran (`bce_calls`→2072,
`pbd_calls`→1033) → parked at **`gs=8, DAT1=0x0b` (`\v`), sig=(1,4)`**
for 150+ s.

Live signature:

- `frame` advances ~40/s (VBLANK alive); **`bce_calls`/`pbd_calls` frozen
  at 2072/1033** — the whole BFSM loop is blocked inside ONE call.
- `DAT1=0x0b` = BFSM `case '\v'` → `hud_state_machine(1)`
  (`game/menu.c:961`). Persistent `0x0b` means `hud_state_machine(1)`
  **never returned even once** — it is spinning in its internal
  `for(;;) { … poll: frame_tick(); }` loop (`game/save.c:5096-5541`)
  waiting on a non-pad gate (`sp48` counter compare, `display_page_commit_a/b`,
  `s7` announce countdown, or the `rec+0x68 & 0xe0` edge).
- Every button on both slots (CROSS/START/SELECT/`0xEFFF`/`0xDFFF`/
  `0x7FFF`/dpad) pressed for seconds: **no state change** — non-input gate.
- Pad edge words DO update live (`0x1f800068..0x77` nonzero during holds).
- CD idle since frame 8618; `pending {0x09, phase:1}` benign.

Suspected mechanism: the HUD/announce state machine waits for a
round/win-counter or display-page condition that the not-yet-started
battle never produces (mode `(1,4)` appears to be a CPU/demo battle whose
fighters never reach the KO the announce sequence counts). Next step:
read `fp`/`sp48`/`sp54`/`s7` live — they are `hud_state_machine` locals,
so arm `fntrace`/`pc_probe` on `0x8016e21c` or add a temporary probe;
alternatively dump the two fighter ctrl `+0xe4` bytes (`sp20`/`sp24`
compare at `game/save.c:5145-5156`) to see which side of the compare is
stuck.

## 6. Menu row 5 `(9b=3,9c=2)` — WEDGE-B persists (pad fix verified, handshake still dead)

`mode_monitor.py 9965 row5`: intro FMV → `gs=8 DAT1=0x06 sig=(3,2)`
select screen → **still parked after 200+ s** with `bce_calls` growing
(~470/s = `fighter_spawn_setup`→`battle_config_eval`×2 live loop),
`pbd_calls=0`.

What changed vs the pre-fix sweep:

- **Pad edges now land**: holding CROSS/START on either slot writes
  `0x1f800068..0x77` live (verified with `set_input` mid-hold). The
  `e9be0fe` host-pointer fix DID restore `pad_state_refresh` in this
  mode — the old "all-zero pad words" symptom is gone.
- **The handshake still never seeds**: the two per-side select records
  (found via cell `0x801f10bc` → records at `+0x28`) read live:
  `rec0 +8=0 +10=1 +1c=0 +24=0 +2c=0`, `rec1 +8=0 +10=1 +1c=2 +24=0 +2c=0`.
  Both `+0x24 == 0` ⇒ `fighter_spawn_setup`'s mode-3 tail
  (`game/menu.c:3956-3983`) writes **nothing** to `rec+8` (it only merges
  pad bits into a record whose `+0x24 != 0`) ⇒ the select loop's
  `puVar6+8 & 0xe0`/`0x800` confirm and `0x1000..0x8000` cursor checks
  (`menu.c:3256-3286`) can never fire ⇒ `+0x24` stays 0 ⇒ permanent
  stall. Same deadlock family as before; the missing piece is whatever
  should seed `rec+0x24` for mode `9b=3` (CPU-side handshake — likely
  `battle_config_eval`/`mc_slot_action` case-3 path in `game/save.c`),
  not pad delivery.

## 7. OPTIONS rows 6 & 7 — WORKS (exit button identified)

Both entered `gs=11 dat1=01` (row6→`system_config_menu_b` sub=4, row7→
`system_config_menu_a` sub=3; sig bytes stay stale `(3,2)` as documented).

- DOWN-nav works (`menu_stepper` consumes `0x1f80007e` edges; holding
  DOWN shows `0x7e=0x4000` live).
- **Exit button is raw `0xEFFF` (~bit `0x1000`, decoded edge `0x10`)** —
  NOT CROSS/START/SELECT/CIRCLE. `system_config_menu_b` exits on
  `_DAT_1f80007c & 0x10` (`game/menu.c:~1240`) or `& 0xe0` at the confirm
  row; the `0x10` path fired immediately → `gs=5`. Verified exit on both
  9966 and 9967. (Prior sweep's "16×DOWN + CROSS" probably exited via the
  `0xe0` confirm-row path after navigating to the bottom row.)

## Still-open wedges (ranked)

1. **Attract demo battle hard-freeze** — `ev_stream_dispatch` infinite
   op-cycle on corrupt `motion+0xa4` (same root cause as
   `versus_eventvm_actsel_deadlock.md`; versus path fixed by `d20db90`,
   attract path still produces the bad `.ACT` count). Highest value: it
   fires with ZERO input on a fresh boot.
2. **Row 3 `(1,4)` `hud_state_machine(1)` poll stall** — new, deeper than
   the old scene_entry/CD wedge; needs `fp`/counter visibility.
3. **Row 5 `(3,2)` select handshake** — `rec+0x24` seeding for mode 3;
   pad delivery now proven working, so the bug is in the CPU/record
   handshake, not input.

## Evidence index

- Driver logs: `/tmp/drv_996{0..7}.log`, `/tmp/drv_9970.log`;
  instance logs `/tmp/br2_99{60..70}.log`.
- Auto-dumps: `/tmp/mode_996{3,4,5,6,7}_*_state_stall.json`,
  `/tmp/mode_9964_row3_final.json`, `/tmp/attract_9960_wedge.json`.
- Live RAM proofs: motion record `0x801F5440` (+0xa4=0x859f, +0x84=stream
  pc), select records via `0x801f10bc` (both `+0x24=0`, `+8=0`).
- Code refs: `game/resolve.c:3094-3147` (ev_stream_dispatch loop +
  debug_server_poll), `game/ev.c:256-315,401+,1576` (EV1/EV2/EV5 +
  table), `game/save.c:5082-5543` (hud_state_machine),
  `game/menu.c:3228-3294` (select handshake), `game/menu.c:3923-4011`
  (fighter_spawn_setup mode tails), `game/menu.c:960-965` (BFSM case `\v`).
