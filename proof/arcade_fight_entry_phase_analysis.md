# Arcade fight-entry wedge — phase-cell writer + state-machine analysis

Status: **root-caused to `battle_flow_state_machine` ph=0 gate; runtime
confirmation of the exact bail/blocker pending.** Versus mode reaches real
combat; arcade stalls earlier, at char-select/state-init (`gs=8 ph=0`),
before the per-player battle phase cells (`0x1f8000b4/b6`) ever engage.

---

## 1. The `0x700` phase writer — definitively identified

Exhaustive scan of all 55 `generated/SLES_017.22_full_*.c` files (store idiom
is always `cpu->write_{byte,half,word}(cpu->gpr[N] [+ const], val)`; no
register-offset or `_pgxa` integer stores). The **only** instruction that can
produce `0x700` in the phase cells:

```
0x801538A8  li    $v1, 0x700
0x801538AC  sll   $v0, $v0, 1          ; player slot index * 2
0x801538B0  addu  $v0, $s1, $v0        ; battle base + slot*2
0x801538B8  sh    $v1, 0xB4($v0)       ; → 0x1F8000B4 or 0x1F8000B6 (dynamic)
```

Inside **`0x80153544` = `battle_config_eval`** (`game/menu.c:4093`,
clean-ported; clean label `L_set700` at `menu.c:4235`).

The slot is **runtime-variable** (`argblock+2` → `AB[2]`), which is why no
literal `+182` store exists anywhere — one instruction covers both cells.

**Gate:** `0x700` is reached only from phase classes `0x000` (`bcfg_step_a`
returns nonzero → `L_set700`) and `0x600` (`continue_countdown` returns `r>0`
→ `L_set700`). See `menu.c:4113` (`cls==0x000 → L_state0`), `menu.c:4126`
(`cls==0x600 → L_600`), `menu.c:4234` (`L_set700`).

**No generated-only function writes literal `0x700`/`0x0700` to the phase
cells.** All literal-0x700 producers live in clean-ported hosts
(`0x80153544`, `0x801487AC`, `0x80149114`, `0x8014CDD0`, `0x80153484`,
`0x80154930`) or are compare-only (`0x80134648`, `0x80134C30`).

→ The `0x300 → 0x700` transition is **not a missing function**; it is a
faithful-wait on upstream phase-class progression (`0x300 → 0x000/0x600`).

## 2. Alias-table bypass (analyzed — NOT a bug to fix)

`0x80153544` head is clean-dispatched, but five mid-function alias entries are
absent from `src/clean_table.c`/`game/fnmap.c`:
`0x8015364C, 0x801536D8, 0x80153730, 0x801537B0, 0x801537CC` — targets of jump
tables `0x801209E0`/`0x801209F8` (the `battle+0x9b` phase-class re-dispatch).

Verified: clean `battle_config_eval` uses a C `switch`/`goto` for that
dispatch (`menu.c:4136-4158`) — it does **not** `call_guest` to alias addrs.
The aliases run generated bodies only if *external* code `jr`s to them, which
nothing should (they are internal case labels). Generated mid-function bodies
are the authoritative fallback anyway. Same for `fighter_spawn_setup`
(`0x80148A38/B00/B54/C5C`) and `post_battle_dispatch` (16 aliases).

**Conclusion:** alias entries cannot be naively added to clean_table — they
are CPS continuation points needing generated resume semantics. Not the wedge.

## 3. Tooling gap — `wtrace` is blind to clean-C writes

Armed `wtrace_range`/`wtrace_add` on `0x1f800000-008` (gs/ph),
`0x1f800098-0a0` (mode byte `0x9b/0x9c`), `0x1f8000b4-b8` (phase cells).
Across a full arcade attempt: **0 captured writes in-range** despite the state
machine visibly cycling `gs`/`ph`.

**Cause:** `wtrace` hooks `cpu->write_{byte,half,word}` (generated MIPS→C
path). Clean-ported C writes guest RAM via host pointers (`guest_at()` →
host arena, `br2_st*`, `DB32`, bound `g_DAT_*`) and **bypasses the hook**.
Every writer of interest (`battle_flow_state_machine`, `battle_config_eval`)
is clean-ported → invisible to `wtrace`.

→ For clean-C writers, use bound-global instrumentation (e.g. the existing
`g_bce_calls/g_bce_cls/g_bce_p3` counters in `battle_config_eval`) or direct
`read_ram` polling — not `wtrace`.

## 4. The actual arcade blocker — `battle_flow_state_machine` ph=0

`gs`/`ph` are scratchpad bytes `0x1f800000`/`0x1f800001`
(`g_DAT_1f800000/1` bind via `guest_for` → `guest_at(0x1f800001)` — confirmed
same cell nav reads as `ph`).

`battle_flow_state_machine` (`menu.c:553`) switches on `DAT_1f800001`:

- `ph=0 → 1`: case-1 inits, then switches on **`DAT_1f80009c`** (mode
  selector, nav's `sub`):
  - `sub=0` → `battle_frame()` directly (the fight)
  - `sub=1,2,3` → `select_screen_tick(N)` (char/stage select)
  - `sub=4` → `select_screen_tick(2)` + countdown/hud
  - `sub=5` → `select_screen_tick(1|3)` → on done `gs=0xb, sub=4`
- then `ph=2` (`match_setup_apply`/`battle_state_init`) → `ph=3`
  (`battle_init`) → `ph=4` (fight loop, `menu.c:736+`).

**Arcade stalls at `gs=8 ph=0`** — never advances. Candidate gates, in source
order:

1. `br2_bail_pending()` at `menu.c:583` and `:630` — each does
   `DAT_1f800001=0; return`. A **perpetual bail-unwind** resets ph=0 every
   frame → infinite stall. `call_bail` debug cmd exposes
   `g_psx_call_bail`, `g_br2_bail_drops`, `g_br2_bail_unwinds`,
   `g_br2_bail_last_fn/ra` — query it at the stall.
2. `frame_divisor_get() != 0` at `menu.c:608`/`707` — holds ph in case-1/3.
3. `select_screen_tick`/`battle_frame` not returning 0 (waiting on input or
   a load that never completes).

Versus reaches `gs=8` and cycles `ph` through real fight phases
(`0300→0500/1400→1100→0100→0300`, win/lose transitions = genuine combat via
`post_battle_dispatch`/`vs_confirm_b`). The `0x700` announce class appears
arcade-specific; versus uses a different announce path.

## 5. CORRECTED root cause — NOT a game/*.c bug (see prior proof docs)

This session's `gs=8 ph=0` observation is an *early* symptom of a
**documented multi-wedge chain** already root-caused in prior sessions.
The clean C source is faithful; the blockers are **runtime/device-model
gaps**, not porting defects:

**Wedge A — CPU-phase-seeding / missing controller-2 feed**
(`proof/arcade_cpu_phase_seeding.md`, `arcade_cpu_phase_init.md`,
`arcade_cpu_confirm.md` — H1 CONFIRMED, disasm-verified):

- `mc_pad_button_handler` (`save.c:1732`) zeroes `+0xb4..+0xb7` each menu
  frame, re-seeds only slots whose pad **edge** fires: `+0x68&0x840`→slot0
  (P0/human), `+0x6a&0x840`→slot1 (P1/CPU).
- `mc_slot_action` (`save.c:2003`) on `battle+0x9b`: case0 (arcade `9b=0`)
  seeds ONLY the pressed slot `0x100`; **case2 (versus `9b=2`) seeds BOTH
  `0x100`** — the only reason two-human versus works.
- The CPU opponent's select-confirm is consumed through the **same pad-2
  decode channel** as a physical controller-2 (`0x801befca`→`battle+0x6a`
  edges→`bcfg_step_a`). **Headless has no controller-2** → `battle+0xb6`
  stays `0` → `battle_state_init` gate `{0x100..0x400,0x700}` skips it →
  CPU fighter never spawns.
- Verified: every CPU mode wedges identically (`9b=0` arcade, `9b=1`
  versus-CPU); only `9b=2` two-human versus works. Injecting slot-1 `press`
  advances the CPU phase exactly like a physical pad-2.
- **Verdict (prior docs): "not a clean-port mistranslation — the missing
  controller-2 input feed for the CPU opponent. No faithful game/*.c phase
  patch exists."**

**Wedge B — CD deferred-coroutine / BIOS-event starvation**
(`proof/cd_deferred_irq_starvation.md`, `cd_deferred_coroutine_starvation.md`,
`arcade_residual_wedge.md`, `arcade_spawn_phase_starvation.md`):

- After both phases reach `0x300` (via injected pad-2), a battle-asset
  streaming read (`CDREAD 0x09`/`ReadN`) completes device-side but its
  `DeliverEvent` never reaches the flag `cd_cmd_sequence`/`cd_result_wait_a`
  (`0x801968CC`) polls → result bytes `0x801BCB25/26` stay `0` → `TestEvent`
  (B0:11) spins ~7×/frame → `battle_init` never returns → `DAT1` frozen.
- `cd_intc_dispatch` runs every frame but sees `i_stat=0` after the last
  sector — the deferred coroutine that presents the command-complete IRQ is
  starved. **psxrecomp runtime IRQ/coroutine scheduling bug** — fix in
  `interrupts.c`/`cdrom.c`/BIOS event services, NOT `game/*.c`/`generated/`.
- Doc: "single runtime bug underlying ALL remaining fidelity/gameplay gaps."

## 6. Implications for the mission

- The clean C tree is **not** the arcade blocker. Both wedges are faithful-
  core runtime issues: (A) headless lacks the controller-2/CPU-input feed
  the game expects on the pad-2 decode channel; (B) CD deferred-coroutine
  IRQ/event delivery starves after streaming.
- Versus works because `9b=2` seeds both phase slots from real pad-1+pad-2
  input — no CPU feed needed, and (on current build) its CD path completes.
- Next frontier is **runtime**, not porting: (A) emulate/feed the CPU
  opponent's select-confirm through the pad-2 channel the way the game
  expects (verify whether the game's own CPU-AI writes that buffer vs. a
  genuine hardware controller-2 requirement); (B) fix the CD deferred-
  coroutine resume so command-complete `DeliverEvent` reaches the waiter.
- Do **not** fake the `0x700`/phase writes or inject CPU confirms in
  `game/*.c` — that would mask the real device/runtime gaps.

## 7. Refinement — pad-2 is needed ONLY for the select-screen confirm, NOT fight inputs

Live watch on attract instance (:9811, `game.toml` players=2) through a full
demo cycle (`gs=3→4→5→6`): **`DAT_801befca` (pad-2 buttons) stayed constant
`…41ffff…` (no-controller sentinel) the entire time, including during `gs=6`
demo fights that run REAL CPU-vs-CPU combat.** `battle+0xb4/b6` read `0`
between demos.

⇒ **CPU/demo FIGHT inputs do NOT travel the pad-2 hardware channel.** The
per-fighter AI/demo input is merged into the fighter input structs directly
(`battle_input_dispatch`→`pad_handler_*` consume a per-fighter button mask
computed by `battle_config_eval`'s callees). Attract CPU-vs-CPU works with
zero pad-2 traffic.

⇒ The pad-2 dependency is **narrowly the SELECT-screen opponent confirm**:
`mc_pad_button_handler` (`save.c:1732`) seeds a slot's phase only on that
slot's pad **edge** (`+0x68` P0 / `+0x6a` P1), and `mc_slot_action`
(`save.c:2003`) case-0 (arcade `9b=0`) writes `phase[param_2]=0x100` for the
single pressed slot only. The CPU slot (1) reaches `0x100` solely via a
`battle+0x6a` (pad-2) edge — which `pad_state_refresh` derives solely from
`DAT_801befca` (SIO controller-2). No clean- OR generated-literal writer
feeds `DAT_801befca`/`battle+0x6a` for CPU.

**Open question (next session):** on real hardware, does the select-screen
CPU opponent's confirm come from (a) an internal CPU-select-AI that writes
pad-2/synthetic input — in which case it lives in generated/unported code
and porting/fixing that path is the answer, or (b) a genuine second
controller — in which case the fix is a device-model virtual controller-2.
Discriminator: find the CPU cursor/auto-confirm writer in the select-screen
call tree (`select_screen_tick` 0x8014f260 → `select_cursor_step`
0x8014f1c8 → `select_confirm_handler` 0x80148528) or consult the Beetle
oracle for arcade-mode pad-2 behaviour.
