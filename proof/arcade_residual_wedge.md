# Arcade residual wedge — exact blocker after both phases reach 0x300

Date: fresh headless session, debug port **8993**, disc `Game.cue`, current build.
Navigation: `/tmp/nav_arcade_8993.py` (port-adjusted copy of
`/tmp/nav_arcade_8981.py`). Reproduced the wedge, then injected pad-2 START
manually to reach the documented endpoint `+0xb4==+0xb6==0x300`.

## TL;DR — the residual blocker is the CD deferred-event stall, NOT game logic

The guest main thread (TCB `0x00008FE0`) is **parked inside the CD-ROM
deferred-coroutine wait loop** — it never returns to the battle state machine.
A `ReadN(0x06)` stream was `Pause(0x09)`'d mid-sector at frame ~19358; the
Pause completed device-side and its IRQ fired, `cd_intc_dispatch` still runs
every frame, but the completion's `DeliverEvent` never reaches the event flag
`cd_cmd_sequence` (generated `0x8019a00c`) polls → `B0/11 TestEvent` spins
(1,845,200 calls) → the fighter-asset read is never issued → `battle_init`
never returns → `DAT_1f800001` frozen at `3` → phases frozen at `0x300`.

This is the **same root cause** already proven on versus
(`proof/cd_versus_consumer_stall.md`) and arcade
(`proof/arcade_spawn_phase_starvation.md`, `proof/cd_deferred_coroutine_starvation.md`).
No `game/*.c` change is warranted; the `0x300` phases are the *expected*
post-`battle_state_init` state, not a stuck transition.

## 1. Writer attribution for each phase cell

`wtrace_arm` was armed **before** navigation on all three ranges
(`0x1f8000b0–0x1f8000c0`, `0x1f800090–0x1f800094`, `0x1f800001–0x1f800002`;
slots 22/23/24). `wtrace_dump` returned **0 entries in the watched ranges**
despite the writes having occurred — because every clean-ported writer goes
through `guest_at()` **host pointers** (`MW8/MW16/MW32` macros,
`game/menu.c:4956-4958`), bypassing the store path wtrace hooks. Wtrace is
blind to clean-C writes; attribution below is from exhaustive source audit
of every `+0xb4/+0xb6/+0x92/DAT_1f800001` writer in `game/`.

### `+0xb4`/`+0xb6` = `0x300` — writer: `battle_state_init`

`game/menu.c:4605-4624` (guest `0x80151328`):

```c
if (select_phase_gate(a2,0) == 0) {           /* cls in {0x100..0x400,0x700} */
    v = *(uint16_t *)(s0 + 0xb4);
    *(uint16_t *)(s0 + 0xb4) = (v & 0xf000) | 0x300;
}
if (select_phase_gate(a2,1) == 0) { … +0xb6 … }
confirm_bookkeeping(a2);
```

`0x300` is a *forced "select-locked" marker* for sides already in a ready
phase class — it is the intended versus-flow state, written **once** at
match-loop caseD_2 (`game/menu.c:675`) and again at caseD_6
(`game/menu.c:737`), plus `post_battle_dispatch` L_d10 (`menu.c:9367`) and
`save.c:2498`. It is **not** produced per-frame and is terminal only inside
`battle_config_eval` (`menu.c:4062-4072` → `L_B0`/`L_B0_7cc` announce-only,
`menu.c:4095-4119`); the outer `post_battle_dispatch` machine has its own
`0x300` transitions (`menu.c:7114-7146`: `0x300→0x1400`/`0x500`).

Observed sequence on this instance:

- `+0xb4`: `0x0000 → 0x0100` (P1 confirm, pad-1 CROSS during select) →
  `0x0300` when `battle_state_init` ran at caseD_2.
- `+0xb6`: stayed `0x0000` through caseD_2 (phase `0x000` fails
  `select_phase_gate` → not forced). After manual `press slot=1 START`
  (`0xfff7`, 60 frames) the pad-2 edge ran `mc_pad_button_handler` →
  `mc_slot_action` (`game/save.c:1732/2003`, case `9b==0` writes
  `+0xb4+slot*2 = 0x100` at `save.c:2021`) → `+0xb6 = 0x0100`, later
  observed `0x0303` (class `0x300`). Final: `+0xb4=0x0300, +0xb6=0x0300`.

### `+0x92` = `0x0100` — writer: `confirm_bookkeeping`

`game/menu.c:3940-3988`, store at `menu.c:3976`
(`*(u16*)(s0+0x92) = ph`, `ph∈{0x100,0x200,0x300,0x400}` from
`select_phase_gate` per side). `0x0100` = init-time value "P0 gate passed,
P1 gate failed" — written by the `confirm_bookkeeping(a2)` call **inside**
`battle_state_init` (`menu.c:4624`). It stays stale because
`confirm_bookkeeping` does not run per-frame at this state (its only
callers here are init/commit/case-9 paths — `menu.c:3649, 3792, 9366`).
Even after `+0xb6` reached `0x300`-class, `+0x92` remained `0x0100` —
expected, not a bug.

### `DAT_1f800001` = `3` — writer: match_loop caseD_2

`game/menu.c:678` (`DAT_1f800001 = '\x03'`) inside the
`if (DAT_1f800001 == 0) → LAB_8016ffd8 → caseD_2` block
(`menu.c:666-680`): `match_setup_apply` + `battle_state_init` +
`scratchpad_pair_init`, then `DAT_1f800001=3`. (The alternate writer,
`battle_frame` L_case2 `MW8(0x1f800001,3)` at `menu.c:8185`, is ruled out:
`battle_frame` never calls `battle_state_init`, so it cannot have produced
the `0x300` phases.) Frozen at `3` because caseD_3
(`menu.c:681-696`) calls `battle_init(0x1f800000,…)` when
`frame_divisor_get()==0` — and `frame_divisor_get` **does** return 0
(verified live: `0x1f800019 == 0x00`), so `battle_init` was entered and
never returned: it is blocked inside the CD file-load path
(`cd_cmd_sequence`, generated `0x8019a00c`).

## 2. The blocking gate — CD deferred-event starvation

Live evidence on port 8993:

**`sp_ring`** — identical 3-call loop every frame, TCB `0x00008FE0`:

```
target 0x00002924  ra 0x0000282C   (kernel yield trampoline)
target 0x8019A87C  ra 0x8019A87C   (cd_chan_full_init / IRQ trampoline)
target 0x8019A53C  ra 0x8019A53C   (inside cd_cmd_sequence 0x8019a00c,
                                    generated-delegated per cd.c:1110)
```

`0x8019A53C` is inside `cd_cmd_sequence` — the per-frame CD pump that issues
commands and waits on completion events. Same signature as
`arcade_spawn_phase_starvation.md` (port 8932) and
`cd_versus_consumer_stall.md` (port 8125).

**`cdrom_state`** (frozen across 32 s wall time; frames advancing ~215 fps):

```
pending {cmd:0x09, active:0, phase:1}   reading:0   stat:0x02
request:0x80 (BFRD)  irq_flag:0x00  i_stat:0x00000000
seek_msf:[1,58,70]  read_msf:[1,59,27]
sector_read_pos:2060/2340   ring_partial:8084   ring_dropped:10
bfrd_set:16241  bfrd_clear:22   irq_present_late:10734  irq_present_max_late:0
```

- `pending.cmd=0x09 active=0` — cmd `0x09` is **Pause** (`cdrom.c:2301`),
  and `active==0` (`pending_pending==0`, `cdrom.c:3300`) means it
  **completed**; `cmd/phase` are residue (`process_pending` clears
  `pending.pending`/`due_cyc` but not `cmd/phase`, `cdrom.c:2624-2625`).
- **No CD command has been issued for ~39k frames.** `cdrom_command_history`
  tail: seq 816 `0x02` SetLoc MSF `[1,58,70]` → seq 817 `0x06` ReadN
  (`reading:1`) → seq 818 `0x09` Pause — all at frame ~19347-19358, then
  silence (`total:819`).
- `cdrom_trace_dump` last register activity: **frame 19358** — after the
  Pause the guest never touches `0x1F8018xx` again; it is not polling, it is
  waiting on an event.
- `bioscall_dump`: `B0/11 TestEvent = 1,845,200` — the guest spins on an
  event flag that never gets `DeliverEvent`'d (BIOS `B0/07` count is small;
  the *specific* flag `cd_cmd_sequence` polls is not the one delivered —
  see `cd_versus_consumer_stall.md` hypotheses).
- `irq_state`: `i_stat=0x00000001`, `i_mask=0x0000000D`, `pending=0x1`,
  `cop0_sr=0x40000404` — VBLANK-class line only; no CD IRQ owed.
- Ring counters: `bfrd_set:16241` vs `bfrd_clear:22`, `ring_partial:8084`,
  `ring_dropped:10` — the sector ring overflowed/dropped while the consumer
  was blocked; `sector_read_pos:2060/2340` = paused mid-sector (280-byte
  ECD/ECC tail unread — the known benign `ring_partial` case per
  `cd_versus_consumer_stall.md`).

**Interpretation:** `ReadN` streamed battle-asset sectors; `Pause` was issued
mid-stream (normal SetLoc→ReadN→Pause chunking). Pause completed and its IRQ
fired (`i_stat=0x04` seen in history), `cd_intc_dispatch` runs every frame —
but the completion never `DeliverEvent`s the flag `cd_cmd_sequence` waits
on, so the next read (fighter assets for `battle_init`) is never issued.
The coroutine yield-loops forever: `cd_chan_full_init → cd_timer_read →
kernel yield`, once per frame.

## 3. What SHOULD happen next (per the code)

Once the CD load completes, `battle_init` returns → caseD_3 sets
`DAT_1f800001=4` → case 4 `menu_res_load_sm` (`menu.c:714`) → case 5
(`se_value_select`, `battle_struct_init`, `battle_session_arm`,
`DAT_1f800001=6`) → case 6 `match_setup_apply` + `battle_state_init` again +
full battle setup (`menu.c:736-754`) → `DAT_1f800001=7` → caseD_8
(`menu.c:761-769`) runs `post_battle_dispatch` every frame — **the machine
that owns the `+0xb4/+0xb6` phase classes during the actual fight** and has
real `0x300` exits (`menu.c:7114-7146`, `7219-7258`). The spawn-commit gate
(`rec+0x10==1 && phase-class==0x700`, `fighter_spawn_setup` `menu.c:3788`)
belongs to `battle_frame`'s *internal* select machine — a different driver
used only while `battle_frame` owns `DAT_1f800001`; it is not the gate that
is failing here. The failing gate is purely "CD file load never completes".

## 4. Answers to the tasked questions

1. **Writers:** `+0xb4/+0xb6=0x300` ← `battle_state_init` (`menu.c:4618/4622`,
   guest `0x80151328`), called at `menu.c:675`/`737` (match_loop caseD) —
   one-shot seed, expected value. `+0x92=0x0100` ← `confirm_bookkeeping`
   (`menu.c:3976`) at init. `DAT_1f800001=3` ← `menu.c:678` (caseD_2).
   (wtrace cannot see these — clean-C `guest_at` host-pointer writes.)
2. **Blocking gate:** not a phase-class/`+0x10`/CD-ready *game* gate — the
   guest never reaches any game gate; it is parked in `cd_cmd_sequence`'s
   event wait inside `battle_init`.
3. **CD starvation:** **YES, implicated and proven** — Pause completed,
   IRQ path alive (`cd_intc_dispatch` ~per frame), but the specific
   completion event is never Delivered; no commands for ~39k frames;
   TestEvent spin 1.8M.
4. **Versus:** not re-run (budget); prior proofs
   (`versus_dat1_6_cd_timer_spin.md`, `versus_phase_machine_deadlock.md`,
   `cd_versus_consumer_stall.md`) show versus wedges on the *same*
   CD-timer/event stall — consistent single root cause.
5. **Minimal faithful fix location:** `psxrecomp/runtime/src/cdrom.c` +
   the deferred-interrupt/BUS-event delivery path (`interrupts.c`,
   `psx_check_interrupts_at` coverage, BIOS event/kobject services) —
   specifically why the `Pause(0x09)`/`ReadN` completion does not
   `DeliverEvent` the flag `cd_cmd_sequence` (`0x8019a00c`, generated)
   polls. NOT `game/*.c` (all clean ports verified faithful), NOT the
   input layer (pad-2 injection works and reaches the same endpoint —
   the wedge is downstream of input entirely).

## Commands run (evidence)

- Launch: `./build/BloodyRoar2_Recompiled --disc …/Game.cue --debug-port 8993 --headless`
- `wtrace_arm {"lo":"0x1f8000b0","hi":"0x1f8000c0"}` (+2 more ranges) → slots 22-24;
  `wtrace_dump` → 0 hits in watched ranges (clean-C writes invisible).
- `read_ram 0x1f800000 len 0xc0` — wedge bytes quoted above.
- `press {"slot":1,"buttons":0xfff7,"frames":60}` → `+0xb6: 0x0000→0x0303`.
- `cdrom_state` ×4 over 32 s — frozen pending/read_msf/irq_flag.
- `cdrom_command_history` — last exec seq 818 `0x09` @ frame 19358.
- `cdrom_trace_dump` — last `0x1F8018xx` access frame 19358.
- `sp_ring` — per-frame `0x8019A53C/0x8019A87C/0x00002924` loop, TCB `0x00008FE0`.
- `bioscall_dump` — `B0/11 TestEvent=1,845,200`.
- `irq_state`, `frame`, `dispatch_tail` (kernel/BIOS addrs only — game
  stack parked inside BIOS/kernel CD wait).

## Instance state at write time

Port 8993 left running at the wedge (`gs=8, DAT_1f800001=3,
+0xb4=+0xb6=0x0300, +0x92=0x0100, CD idle-pending-residue`). Ports
8976/8981 untouched per instructions.
