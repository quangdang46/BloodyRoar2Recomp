# `battle+0x92` / `+0x9b` / `+0x9c` / `+0xb4` / `+0xb6` — complete driver map and Arcade wedge analysis

Date: current session. Live instance: debug port **8976** (headless, arcade/1P).
Supersedes/refines `arcade_cpu_phase_seeding.md` and `arcade_spawn_phase_starvation.md`
where they conflict — see "Corrections" below.

---

## 1. TL;DR

- `battle+0x92` is a **u16 with two fields**: high byte = phase *class*
  (`& 0xf00`-style: 0x100/0x200/0x300/0x400 = "who has confirmed select"),
  low byte = sub-state 0–0x10 driven by the versus/round sub-phase machine.
- **Class writer:** `confirm_bookkeeping` (`game/menu.c:3940`, guest
  `0x8015113c`) — the *only* function that writes the class word
  (`*(u16*)(battle+0x92) = 0x100/0x200/0x300/0x400`, menu.c:3976).
  `mc_slot_action` (save.c:2033) writes `0x400` for mode 3;
  `game_globals_reset` (game.c:1133) writes `0`.
- **Low-byte writers:** `post_battle_dispatch` (menu.c:6861–7226, values
  1,2,3,4,5,6,7,8,0xb,0xc,0xd,0xe,0x10), `vs_confirm_c/d/e`
  (menu.c:7801/7844/7866), `vs_confirm_b` (menu.c:9430–9481, 0xf/0x10),
  `hud_state_machine` (save.c:5466, 0x10). All preserve the class byte
  (`(v & 0xff00) | sub`).
- **All of them are downstream of flow state 8** (`post_battle_dispatch` is
  called only from `battle_flow_state_machine` outer case `\x08`,
  menu.c:768) or from the versus screen — **none run while
  `battle_frame` is spinning in its inner case 3**.
- **The live 8976 wedge is the already-documented CD deferred-coroutine
  starvation**, not a phase-machine bug: `cdrom_state` shows
  `pending {cmd:0x09 CDREAD, active:0, phase:1}`, `request:0x80`,
  `irq_flag:0x00`, `read_msf [1,59,27]` vs `seek_msf [1,58,70]`, and
  `sp_ring` = the `cd_timer_read→kernel-yield→cd_chan_full_init` per-frame
  spin (identical signature to `arcade_spawn_phase_starvation.md`).
  The phase/`+0x92` values are a **frozen mid-flight snapshot**, not the
  cause.
- **Correction:** the earlier "P1 `+0xb6` never initialized" snapshot is
  stale. Live 8976 now shows **`+0xb4 = +0xb6 = 0x0300`** — both players
  at the select-confirmed class. See §5.

---

## 2. Field map (battle scratchpad `0x1f800000`)

| Offset | Field | Meaning |
|--------|-------|---------|
| `+0x00` | u8 | flow sub-state (9 during versus commit) |
| `+0x01` | u8 `DAT_1f800001` | **flow state byte** — shared by `battle_flow_state_machine` outer switch AND `battle_frame` inner dispatcher AND `battle_intro_flow` |
| `+0x68`/`+0x6a` | u16 ×2 | per-player pad **edge** words (P0/P1), written only by `pad_state_refresh` (menu.c:2869, guest `0x8012a72c`) |
| `+0x74`/`+0x76` | u16 ×2 | per-player pad **held** words (debounced copy) |
| `+0x8c` | s16 | global timer/counter |
| `+0x92` | u16 | **global phase**: class (hi) + sub-state (lo) |
| `+0x9b` | u8 | battle/mode selector (0=arcade, 1/4=?, 2=versus, 3=?, 4=?) |
| `+0x9c` | u8 | match-mode class (0=arcade/1P, 1/2/3/4/5 = versus variants) |
| `+0xa4` | s16 | gate/counter vs char-record `+0x10` |
| `+0xb0` | u8 | versus commit flag |
| `+0xb4`/`+0xb6` | u16 ×2 | **per-player phase words** (P0/P1), `& 0xf00` = class |
| `+0xc0` | ptr | char-record table base (`0x801c1200`) |
| `+0xc8`/`+0xcc` | ptr | secondary tables (`0x801c15f8` …) |

Per-side config records: `0x1f8000e8` (P0) / `0x1f8001f0` (P1), stride
`0x108`; the `fighter_spawn_setup`/`battle_frame` record table `T` is
`sp+0x40`, stride `0x30`, `+0x10` = "slot active" flag, `+0x1c` = cursor,
`+0x24` = confirm handshake state, `+0x08` = merged pad word.

---

## 3. Complete writer inventory

### 3.1 `battle+0x92` (global phase u16)

| Writer | Guest | File:line | Writes |
|--------|-------|-----------|--------|
| `confirm_bookkeeping` | `0x8015113c` | menu.c:3976 | full u16 = 0x100/0x200/0x300/0x400 from `select_phase_gate` of both players |
| `mc_slot_action` | `0x8015260c` | save.c:2033 | `0x400` (mode `+0x9b`==3 only) |
| `game_globals_reset` | — | game.c:1133 | `0` |
| `post_battle_dispatch` | `0x80154930` | menu.c:6816,6861–7226 | `|0x1000` flag; low byte 1,2,3,4,5,6,7,8,0xb,0xc,0xd,0xe,0x10 |
| `vs_confirm_c` | `0x80153ee8` | menu.c:7801 | low byte `cls` |
| `vs_confirm_d` | `0x80154368` | menu.c:7844,7866 | low byte 0xb |
| `vs_confirm_e`→`vs_confirm_b` | `0x801545d0`/`0x80152bdc` | menu.c:7896,9430–9481 | low byte 0xf, 0x10 |
| `hud_state_machine` | — | save.c:5466 | low byte 0x10 (versus "GO" commit, also sets `+0xb4=0xa00`, `g[0]=g[1]=9`) |

`battle.c` `+0x92` writes (3844/4056/4143/4838+) are on **fighter control
blocks** (`param_1/param_2` ≠ battle base) — unrelated.

### 3.2 `battle+0x9b` / `+0x9c` (mode bytes)

Set during menu/mode-select and match setup — not per-frame drivers.
Observed live: `+0x9b=0, +0x9c=0` (arcade/1P). `+0x9b` is read by
`battle_config_eval` phase-0x000/0x300 jump tables (menu.c:4051,4063),
`fighter_spawn_setup` button-merge (3838), `battle_state_gate` (4185),
`mc_slot_action` (2018), `side_flag_merge` (9040). `+0x9c` read by
`battle_state_gate` (4183), `selection_stat_bump` (3622),
`battle_config_eval` L_600 (4124), `post_confirm_config_build` tail.

### 3.3 `battle+0xb4` / `+0xb6` (per-player phase u16)

| Writer | Guest | File:line | Effect |
|--------|-------|-----------|--------|
| `rec_timer_pair_clear` | `0x801525c4` | save.c:1455 | zero both (called by `fighter_spawn_tail`, game.c:1154, menu.c:579/1817, battle_frame case0 via `clear_battle_counters`? — no: `clear_battle_counters` only clears `+0x68..+0x7e`) |
| `mc_pad_button_handler` | `0x80153264` | save.c:1737 | zero both **every menu frame**, then re-seed via `mc_slot_action` on pad edges `+0x68/+0x6a & 0x840` |
| `mc_slot_action` | `0x8015260c` | save.c:2021–2035 | mode 0: pressed slot `=0x100`; mode 1/4: `0x100`+foe `0`; mode 2: both `0x100`; mode 3: both `0`, `+0x92=0x400` |
| `select_confirm_handler` | — | menu.c:3584 | `0x200 → 0x300` on confirm (case-6 select only) |
| `battle_state_init` | `0x80151328` | menu.c:4618/4622 | `→0x300` **only if** `select_phase_gate`==0 for that player; then `confirm_bookkeeping` |
| `bcfg_step_c`/`bcfg_step_d` | — | menu.c:4239/4262 | `→0x100` reset |
| `battle_config_eval` | `0x80153544` | menu.c:4029,4124–4161 | the per-frame class machine (§4) |
| `fighter_spawn_setup` | — | menu.c:3781/3782 | `0x500 → 0x600` patch |
| `post_battle_dispatch` | `0x80154930` | menu.c:6837–6840 (`|0x1000`), 7114–7146 (`0x300→0x500`/`0x1400`), 7185+/7219+ (0x600 checks), 7645/7650 (`→0x900` via `side_phase_advance`) | versus/outer-state-8 round machine |
| `vs_confirm_b` | `0x80152bdc` | menu.c:9355–9480 | `0x100`/`0x600`/`0xa00` class writes |
| `hud_state_machine` | — | save.c:5463 | one side `→0xa00` at versus "GO" |
| `battle_intro_flow`→`fighter_spawn_tail` | `0x80167330`/`0x80166398` | menu.c:2046, save.c:2480 | `rec_timer_pair_clear` (zero) then `battle_state_init` (→0x300 gated) |

`action.c`/`battle.c`/`resolve.c` `+0xb4/+0xb6` writes are on fighter
records, not the battle base.

---

## 4. The phase state machine

### 4.1 Drivers that share `DAT_1f800001` (`battle+1`)

Three top-level machines dispatch on the same byte:

1. **`battle_flow_state_machine`** (`0x8016FCC4`, menu.c:551) — session/menu
   driver. Outer cases 0–8: case `\x01`+`+0x9c==0` calls **`battle_frame()`**
   when `+0x9b==0 && cfg+0x11!=1` (arcade), else `select_screen_tick`;
   outer case `\x08` calls `post_battle_dispatch` (versus) or
   `battle_sides_commit` (`+0x9b==4`).
2. **`battle_frame`** (`0x8014CDD0`, menu.c:8075) — **arcade-only** inner
   loop, states 0–9/0xf0/0xf1:
   - `0`: bringup — task pools, `clear_battle_counters` (pad words only),
     `battle_config_eval`×2, `post_confirm_config_build`→`confirm_bookkeeping`
     → writes `+0x92` class.
   - `1`,`2`: GPU/engine setup → `MW8(+1,3)`.
   - `3`: **`fighter_spawn_setup` every frame** — the wedge site.
   - `4`,`5`: → `6`.
   - `6`: **select screen** — cursor nav, `select_confirm_handler`
     (`0x200→0x300`), per-record `+0x24` handshake → `7` when both `==2`.
   - `7`→`8`: eval fp=1, latch players at phase `0x700`.
   - `9`: `confirm_bookkeeping`+`post_confirm_config_build`+
     `record_init_sweep`, arm foe `+0x24=1` → back to `1` (round loop);
     `0xf0`→jingle→`0xf1`→`0xff` exit.
3. **`battle_intro_flow`** (`0x80167330`, menu.c:1961) — called per-frame
   from `game.c:699`; state `0` runs `fighter_spawn_tail`
   (`rec_timer_pair_clear`→zero phases, `battle_state_init`→0x300 gated),
   then `menu_res_load_sm`, `battle_round_init`, versus-screen
   (`hud_state_machine`) machinery, `0xf0/0xf1` exit.

### 4.2 `battle_config_eval` (`0x80153544`, menu.c:4007) — the only per-frame phase advancer inside `battle_frame` case 3

Dispatches on `phase & 0xf00`, then on `+0x9b` jump tables for classes
0x000/0x300:

| Class | Mode `+0x9b`==0 (arcade) path | Result |
|-------|------------------------------|--------|
| `0x000` | `L_A0` → needs `+0x92` class ∈ {0x100,0x200,0x300} → `L_A0_body` → `bcfg_step_a` | → `0x700` **iff** `bcfg_step_a` passes |
| `0x100` | → `0x200` (auto, next eval) | advances |
| `0x200` | `L_ret` — waits for `select_confirm_handler` (case 6 only) | **terminal in case 3** |
| `0x300` | `L_B0` → `+0x92` class 0x100/0x200→`L_B0_774`, 0x300→`L_B0_7cc`: announce only | **terminal — no outgoing transition** |
| `0x400` | `L_ret68` | terminal |
| `0x500` | patched `→0x600` by `fighter_spawn_setup` (menu.c:3781) | → countdown |
| `0x600` | `L_600` → `continue_countdown` → `0x700` (ready) or `0x800` | advances |
| `0x700` | `bcfg_step_c` → reset `0x100` | transient |
| `0x800`/`0x900` | → `0x900`, low-byte counts to 0 | round-end timer |

`bcfg_step_a` (menu.c:4206) = `battle_state_gate(battle,0) != 0` **AND**
`(player_pad_edge & 0x800) != 0` **AND** `ret_zero_b(player) >= 0`.
`ret_zero_b` (`0x80163898`, save.c:2705) is a faithful `jr ra; move v0,zero`
— always returns 0, so `>= 0` always passes (verified, not a stub bug).
`battle_state_gate` (menu.c:4175): fails if `s16(+0x82)!=0`, `+0x9c∈[1,5]`,
`+0x9b∈[1,3]`; for `+0x9b==0` additionally requires
`s16(+0xa4) >= cr[+0x10]`; then jump-table on `+0x92` low byte
(states {0-4,15}→1, {5-11,16}→0, {12-14}→`+0xa4>=0`).

**The pad edge**: for player slot `i`, `battle_config_eval` reads
`*(u16*)(battle + i*2 + 0x68)` = `+0x68` (P0) / `+0x6a` (P1) — written only
by `pad_state_refresh` from physical pad records `0x801befa8` (pad-0) and
`0x801befca` (pad-1). Bit `0x800` = the confirm/start edge.

### 4.3 `fighter_spawn_setup` exit condition (the actual wedge gate)

menu.c:3784–3831: scans the two `T` records; exits state 3 → `s3[1]=5/6/2`
**on the FIRST player** with `rec+0x10==1 && (phase & 0xf00)==0x700`.
Only then does it run `confirm_bookkeeping`, alloc the CPU/anim task pair
(`0x801b3054`/`0x801b30dc`/`0x801b312c` callbacks), `deferred_cmd_post`,
and return. Otherwise it falls through to the mode-`+0x9b` button-merge
tail and returns with state still 3.

**Corollary:** case 3 can only be exited by a player reaching `0x700`,
which in arcade (`+0x9b==0`) is reachable **only** from phase classes
`0x000` (confirm edge), `0x500→0x600→countdown`, or already `0x700`.
Classes `0x100–0x400` are dead ends inside case 3.

---

## 5. Live runtime evidence — port 8976

`mem_words 0x1f800000` (two reads ~1 s apart, frames advancing):

```
+0x00 u32 = 0x00000308  -> +0x01 = 0x03   (battle_frame case 3, spinning)
+0x90 u16 = 0x0100  then 0x0003          (+0x92 oscillating!)
+0x94..+0x9f: +0x98=1, +0x9b=0, +0x9c=0  (arcade/1P confirmed)
+0xb4 u16 = 0x0300,  +0xb6 u16 = 0x0300 (BOTH players select-confirmed)
+0xa4 s16 = 0, +0xa8=0x801f47a0, +0xac=0x801f4770, +0xb0=1
+0xc0 = 0x801c1200 (char recs), +0xc8 = 0x801c122c, +0xcc = 0x801c15f8
```

`cdrom_state`: `pending {cmd:0x09, active:0, phase:1}`, `request:0x80`,
`irq_flag:0x00`, `int1_pended:0`, `read_msf [1,59,27]` vs
`seek_msf [1,58,70]`, `ring_dropped:2`, `ring_partial:1824`,
`bfrd_set:3931`/`bfrd_clear:14`.

`sp_ring` tail — identical 3-call loop every frame on TCB `0x00008FE0`:

```
0x8019A87C  cd_chan_full_init   (generated-delegated)
0x00002924  kernel/idle trampoline
0x8019A53C  cd_timer_read       (generated-delegated)
```

`frame` counter advances (3 671 650+) — the deferred-interrupt coroutine
spins once per frame; the battle state machine is frozen downstream.

**This is the same signature documented in `arcade_spawn_phase_starvation.md`**
(port 8932): a `CDREAD (0x09)` for battle-asset streaming never completes,
the coroutine never resumes, and `+0xb4/+0xb6/+0x92` are frozen snapshots.

### Corrections to earlier hypotheses

1. **"`+0x92` stays 0"** — wrong on two counts: live shows `0x0100`/`0x0003`
   (it is *written every pass* by `confirm_bookkeeping` at case 0 and by
   the low-byte machinery), and it is not the blocker — it is a *read-only
   gate input* to `battle_config_eval`/`battle_state_gate`, not a driver
   the wedge waits on.
2. **"P1 `+0xb6` never initialized"** — stale. Current live shows
   `+0xb6 = 0x0300`. The earlier `0x0000` snapshot was either pre-init or
   a different run state. `battle_state_init` (or the versus-screen
   confirm path) *did* seed both players to the select class.
3. **The phase values are consistent, not corrupt**: `+0xb4=+0xb6=0x300`
   = "both players select-confirmed"; `+0x92` class would be `0x300` by
   `confirm_bookkeeping` — the observed `0x0100`/`0x0003` is a torn
   mid-transition read while the machine is frozen (P1's gate result was
   captured before its phase reached 0x300, or the low-byte writer ran
   after the class byte was cleared by a re-init).

---

## 6. Answering the original questions

**Every writer/driver of `+0x92`, `+0x9b`, `+0x9c`:** §3. `+0x92` class ←
`confirm_bookkeeping`/`mc_slot_action`/`game_globals_reset`; `+0x92` low ←
`post_battle_dispatch`/`vs_confirm_*`/`hud_state_machine`. `+0x9b/+0x9c` ←
mode/match select (menu flow), read-only downstream.

**What advances the global phase:** the versus/round sub-phase machine —
`post_battle_dispatch` (flow state 8) dispatching to `vs_confirm_b/c/d/e`,
`side_phase_advance/commit`, plus `hud_state_machine` on the versus
screen. **None of it runs while `battle_frame` is in case 3.**

**Why Arcade stays low:** it doesn't "stay" — `confirm_bookkeeping` rewrites
the class every case-0 pass. The observed freeze is the **CD deferred-
interrupt starvation** (active blocker, already proven), which parks the
whole battle machine. The phase words are downstream symptoms.

**Missing task / unmet condition / mode-gated path?** The proximate cause
is the CD `0x09` read never completing (deferred coroutine starvation —
see `cd_deferred_coroutine_starvation.md`, `arcade_spawn_phase_starvation.md`).
A *secondary latent risk* exists independent of CD: if arcade ever reaches
`battle_frame` case 3 with phases in select classes `0x100–0x400` (as the
frozen `0x300/0x300` snapshot shows), case 3 **cannot exit** — `0x300` is
terminal in `battle_config_eval` and `fighter_spawn_setup` requires `0x700`.
Versus avoids this because `post_battle_dispatch` (state 8) performs the
`0x300→0x500/0x1400` conversion (menu.c:7114–7146) that arcade's case-3
path lacks. Whether arcade legitimately presents `0x300` phases to case 3
on real hardware — or whether a phase-reset/commit is missing from the
arcade entry — **cannot be proven until the CD wedge is fixed** and a clean
arcade run reaches case 3.

---

## 7. Faithful fix location — status: NOT PROVEN, do not patch yet

Per the report-first constraint, no `game/*.c` change is justified yet.

Candidate locations **once CD is fixed and a clean arcade run is observed**:

1. If phases enter case 3 as `0x300` on real hardware too → the missing
   piece is a `0x300→battle-class` commit for arcade analogous to
   `post_battle_dispatch` menu.c:7114–7146 — likely inside
   `fighter_spawn_setup`'s mode-0 tail, `battle_state_init`, or the
   versus-screen exit (`hud_state_machine`/`versus_stage_commit` family).
2. If phases should be `0x000` (post `fighter_spawn_tail`
   `rec_timer_pair_clear`) → the CPU opponent additionally needs a
   `+0x6a & 0x800` confirm edge; `pad_state_refresh` reads physical pad-1
   record `0x801befca` only — the faithful source of CPU input (AI task vs
   synthesized pad record) must be identified in generated disasm.
3. If the CD starvation is the sole blocker → fix belongs in the CD
   deferred-command-issue path (`cd_seek_param_issue`→`cd_reg_window_op`),
   not here.

**Next runtime step:** after the CD fix, arm a write-watch on
`0x1f8000b4`/`0x1f8000b6`/`0x1f800092` during a fresh arcade boot
(menu→Arcade→stage→battle) and record every writer PC from versus-screen
exit through `battle_frame` case 3; diff against `generated/` disasm of
the versus-screen exit and `battle_state_init` callers.

---

## 8. Evidence index

- Clean sources: `game/menu.c` (battle_frame 8075, fighter_spawn_setup
  3768, battle_config_eval 4007, bcfg_step_a 4206, battle_state_gate
  4175, confirm_bookkeeping 3940, select_phase_gate 3605,
  select_confirm_handler 3555, battle_state_init 4605,
  post_battle_dispatch 6789, vs_confirm_b 9306, side_phase_advance 7583,
  side_phase_commit 7675, pad_state_refresh 2869, battle_flow_state_machine
  551, battle_intro_flow 1961); `game/save.c` (mc_slot_action 2003,
  mc_pad_button_handler 1732, rec_timer_pair_clear 1451,
  fighter_spawn_tail 2480, hud_state_machine 5064, attr_word_decode 1604,
  ret_zero_b 2705, select_phase_update 3158); `game/game.c`
  (game_globals_reset 1127, clear_battle_counters 2387).
- Prior proofs: `proof/arcade_cpu_phase_seeding.md` (H1: CPU pad-edge
  seeding gap — still valid for the *versus-screen* path),
  `proof/arcade_spawn_phase_starvation.md` (CD deferred-coroutine
  starvation — confirmed live on 8976),
  `proof/cd_deferred_coroutine_starvation.md`.
- Runtime: port 8976 `mem_words`/`cdrom_state`/`sp_ring`/`frame` (this
  session); earlier snapshots on 8125/8400/8600/8700/8200 were stale or
  dead this session.
