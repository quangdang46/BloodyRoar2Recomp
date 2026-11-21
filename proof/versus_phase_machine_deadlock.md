# Versus battle-entry stall — phase-machine deadlock (CD exonerated for this wedge)

Live instance: port **8125**, frame ~410208, `gs=8`, `0x1f800001=3`.
Supersedes/refines `cd_versus_consumer_stall.md`,
`versus_framedivisor_post_cd.md`, `cd_seek_divergence.md` for the
*current* wedge signature. Prior "DeliverEvent starvation / TestEvent
spin" signature is **gone** — this wedge is different.

## Symptom chain (verified live)

`battle_frame` (`game/menu.c:7995`) internal dispatcher, `L_case3`
(`menu.c:8099`, guest `0x8014D134`) → `fighter_spawn_setup`
(`menu.c:3735`) → `battle_config_eval` ×2 per iteration → `L_tail`
(`task_pools_tick(1/2)`, `frame_tick`) → loop. Phase never reaches the
`(battle+0xb4/b6 & 0xf00)==0x700` gate, so `s3[1]` stays `3` forever.

Live scratchpad (`read_ram 0x1f800090` / `0x1f8000a0`):

- `+0x92 = 0x0100` (battle sub-phase; `confirm_bookkeeping` wrote `0x100`
  ⇒ **only one side confirmed**: g0==0, g1!=0)
- `+0x9b = 0`, `+0x9c = 0`, `+0xa4 = 0`
- `+0xb4 = 0x0300` (player0 phase class **0x300**)
- `+0xb6 = 0x0000` (player1 phase class **0x000 — never initialized**)
- `+0xc0 = 0x801C1200` (char-record table), `+0xcc = 0x801C15F8`

Side records: `0x1f8001f0` (P2 block) = `00 01 01 05 …` ⇒ confirm byte
+1=1, char +3=5 (a character WAS picked). `0x1f8000e8` (P1 block) is all
zeros. The versus select records and the battle phase words are
**inconsistent** — P2 "confirmed" in the select record but its battle
phase `+0xb6` is still `0x000`.

## Why it can never self-recover (static, disasm-verified)

`battle_config_eval` (`menu.c:3974`, guest `0x80153544`) is the ONLY
per-frame phase evaluator inside the case3 loop (via
`fighter_spawn_setup`). Its class dispatch:

- `cls==0x300` → `L_state3` (`menu.c:4029`) → `idx=B[0x9b]` → `L_B0` →
  reads `+0x92` class → `L_B0_774`/`L_B0_7cc`/`L_B23` → **announce only,
  no phase write, every sub-path**. There is NO `0x300` exit in
  `battle_config_eval`, for any `+0x9b`/`+0x92` combination.
- `cls==0x000` (b6) → `L_state0` → `L_A0` → `L_A0_body` →
  `bcfg_step_a` → `L_set700` **can** reach `0x700` — but only if
  `bcfg_step_a` returns nonzero (needs `battle_state_gate` etc.).

The versus phase advancer `vs_confirm_b` (`menu.c:9216`, guest
`0x80152bdc`) — the function that writes `0x300→0x100→0x600→0x700` —
is reached **only** via `post_battle_dispatch` (`menu.c:6709`) →
`vs_confirm_e` (`menu.c:7804`) → tail-call when
`MBS16(s0+0x8c) >= 0x12c`. `post_battle_dispatch` runs at outer-machine
byte `==8` (`menu.c:768`), which is *downstream* of `battle_frame`
completing its own byte machine — unreachable while case3 spins.

`select_phase_gate` (`menu.c:3572`) returns 0 for classes
{0x100,0x200,0x300,0x400,0x700} — `battle_state_init` (`menu.c:4538`)
forces `0x300` for any player already in a select class. That is why
b4 sits at `0x300`: versus confirm (`select_confirm_handler`
`menu.c:3551`) moved it `0x200→0x300`, and `battle_state_init` kept it.

## CD side — EXONERATED for this wedge

- `cdrom_state`: `reading=0`, `irq_flag=0`, `pending.cmd=0x09 active=0`
  (Pause completed), `int1_lost=0`, `irq_present_max_late=0`,
  `sector_read_pos=2060/2340` (known benign EDC tail).
- **hle_dump tail (decisive):** last BIOS activity at cycle ~9.78e9 /
  frame ~14546 — `B0/0x0A EnableEvent(0xF1000008)` (ra `0x8019CF48`)
  → **`B0/0x07 DeliverEvent(0xF0000009, 0x20)` FIRED** (ra `0x8019B88C`,
  hle seq 2427) → `B0/0x0B TestEvent(0xF1000008)` ×6 (ra `0x8019D550`).
  The CD command-completion event **was delivered and consumed**. After
  that: zero HLE calls and zero CD MMIO — the device is idle because the
  game requested nothing further, not because a completion was lost.
- `bioscall_dump`: B0/0x0B TestEvent count **frozen** at 1,424,248 over a
  5 s window — the old "TestEvent spin" wedge is NOT the current state.
- `clean_trace` (live, 512-entry tail ≈ 5 frames): only the CD
  record-machine round-robin (`0x801A38B8` queue_drain, `0x801A3E94`
  state_rx, `0x801A3A78` rec_scan, `0x801A3350` dev_cmd_process, …) and
  the GPU deferred queue (`0x80192E78`/`0x8019A7FC`/`0x801936B0`) run per
  frame on the kernel stack (`sp=0x801FFF98`). No menu/battle clean
  functions appear because they call each other directly (C-to-C, no
  dispatch) — but the per-frame IRQ work is exactly what
  `br2_irq_poll`/`frame_tick` drives inside the case3 spin.
- Ring cursors `0x801FC6A8/AC/…`: producer/finalizer ~2, consumer ~0x1E
  — residual desync, but no pending command exists to complete.

**Conclusion:** the CD went idle *because* the versus handshake stalled
upstream of any further load request. Treating this as a CD bug was a
red herring for the current signature; the earlier TestEvent-spin wedge
(different signature, `cd_seek_divergence.md`) may still be real but is
not what port 8125 shows now.

## Root cause (guest-side, versus phase machine)

The versus select→battle handoff left the battle phase words in an
invalid combination: P0=`0x300` (confirmed, select class — terminal in
`battle_config_eval`), P1=`0x000` (never entered select classes),
`+0x92=0x100` (only one side confirmed). `battle_frame` case3 therefore
waits forever for `0x700` that no reachable function will write.

The missing piece is **where P1's (`+0xb6`) phase should have been
initialized** during versus setup, and why `+0x92` is `0x100` rather than
`0x300` (both-confirmed). Prime suspects (all versus-mode-gated paths
not yet fully traced):

- versus P2/CPU select init — the code that should write `0x100`/`0x200`
  to `+0xb6` when the second combatant is registered (human pad2 or CPU);
- `match_setup_apply` / `per_side_setup` / `battle_sides_commit`
  versus branches;
- a still-generated (unported) versus init function that the clean tree
  delegates but whose caller gate is mistranslated.

## Recommended next step (fresh instrumented run)

Boot fresh (port 8400+), navigate title→mode select→**versus**→char
select→fight with `press` (Start `0x0008`, Cross `0x4000`), and arm:

- `fntrace_arm`/`pc_probe_arm` on `select_screen_tick` (`0x8014f260`),
  `vs_confirm_e` (`0x801545d0`), `vs_confirm_b` (`0x80152bdc`),
  `battle_state_init` (`0x80151328`), `confirm_bookkeeping`,
  `match_setup_apply` (`0x8015a69c`);
- `gwatch`/`mem_words` on `0x1f8000b4`,`0x1f8000b6`,`0x1f800092` to
  catch the exact writer (or absence) of P1's phase init;
- determine whether versus P2 is human (needs pad2 input) or CPU
  (auto-confirm) — if human and unconfirmed, the stall may be *legitimate*
  input starvation rather than a bug; versus should offer CPU.

## Evidence commands used

`cdrom_state`, `cdrom_command_history`, `cdrom_trace_dump`,
`bioscall_dump`, `bios_svc_trace`, `hle_dump`, `clean_trace`,
`sp_ring`, `fn_stats`, `phase_profile`, `get_registers`,
`mem_words`/`read_ram` on `0x1f800000` region, `0x801FC680` ring
globals, `0x80010000` ring base, `0xA0000108`/`0x8FE0` kernel PCB/TCB,
side records `0x1f8000e8`/`0x1f8001f0`, char table `0x801C1200`.

## UPDATE — reproduces on ARCADE too (shared battle-init bug, not versus-specific)

Fresh instance :8400 driven by tools/arcade_fight_mash.py (ARCADE = 1P vs
CPU, P2 auto-confirms — NOT 2P human) reaches gs=8 ph=3 and deadlocks
IDENTICALLY: b4=0x0300, b6=0x0000, +0x92=0x0100, 9b=9c=0. Persists >24s.
Both P1(0x1f8000e8)/P2(0x1f8001f0) select records populated (chars picked).

=> The phase deadlock is NOT a versus/2P-input issue. It hits the SHARED
battle-entry path for combatant index 1 (+0xb6) in arcade AND versus.

## Mechanism pinned (menu.c)

- battle_state_init (0x80151328, menu.c:4538): writes phase 0x300 to
  +0xb4 (P0) and +0xb6 (P1) ONLY when select_phase_gate(a2,idx)==0.
  Wedge: P0->0x300 written, P1->0 skipped because select_phase_gate(a2,1)
  returned 1 — P1's phase was 0x000, NOT in a select class.
- select_phase_gate (menu.c:3572) returns 0 only for phase classes
  {0x100,0x200,0x300,0x400,0x700}. P1=0x000 -> gate=1 -> no write.
- select_confirm_handler (menu.c:3541-3551): advances a player's phase
  0x200->0x300 on confirm. P0 did; P1 never reached 0x200/0x300.
- battle_sides_commit (menu.c:4763) runs battle_config_eval(s2)+
  battle_config_eval(s3) for both sides and drives the +0x92 setup
  sub-state machine (cases 0-4) — candidate site for the missing P1
  phase-class init.
- The per-player phase word lives at battle+0xb4+idx*2 (idx0=+0xb4 P0,
  idx1=+0xb6 P1). Initial select-class value (0x100/0x200) must be written
  during the char-select->battle handoff for EACH combatant.

## Sharp question for the fix

WHERE is each combatant's phase first set to a select class
(0x100/0x200) during battle entry? For P0 it happened; for P1 (+0xb6)
it did not. Likely a per-side loop that only processes index 0, or a
versus/arcade-gated branch that skips the second side's phase init, in
match_setup_apply / per_side_setup / battle_sides_commit / battle_state_init
callers. Trace which function writes battle+0xb6 (and +0xb4) during the
select->battle transition and why the idx==1 write is absent.

## ROOT CAUSE PINNED (2026-11-22) — arcade CPU-opponent phase never init; versus WORKS

Live A/B on fresh instances:
- :8600 VERSUS (proper dual-pad): DAT1=6, b4=b6=0x0300 — BOTH human players
  confirm via pad -> both battle phases init -> progressing through intro.
  Mode bytes 9b=2/9c=1. P2 record+1=0 (human).
- :8400 ARCADE (arcade_fight_mash, 1P vs CPU): DAT1 stuck=3,
  b4=0x0300 (human ok), b6=0x0000 (CPU NEVER init). Mode 9b=0/9c=0.
  P2 record+1=1 (CPU-opponent flag).

=> NOT a shared bug, NOT input starvation on versus. The wedge is
ARCADE-SPECIFIC: the CPU opponent (slot1) never gets its battle phase
initialized because its char-select confirm doesn't run the human
select_confirm_handler path (no pad). bcfg_step_a then gates phase 0->0x700
on flag 0x800 @ battle+0x6a which stays clear; fighter_spawn_setup (case3)
waits for phase 0x700 forever.

Phase machine decoded (battle_config_eval menu.c:3974):
  cls=ph&0xf00; idx=B[0x9b]. 0x000->L_state0->bcfg_step_a->0x700;
  0x100->0x200; 0x200->wait; 0x300->announce stage/fight (stable until
  intro completes); 0x600->continue_countdown->0x700/0x800; 0x700->bcfg_step_c
  (reset 0x100); 0x800->0x900->tick->0.
Per-player words: phase u16 @ battle+0xb4+slot*2, flag u16 @ battle+0x68+slot*2
(=0x1f800068/0x6a, cleared by clear_battle_counters at battle init).
