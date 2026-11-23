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

## 5. Next steps (fresh session)

- Re-drive arcade to `gs=8 ph=0`; query **`call_bail`** — if
  `g_br2_bail_unwinds`/`drops` climbs and `g_psx_call_bail` latches, the
  stall is a CPS/bail starvation in the `sub=0→battle_frame` or
  `select_screen_tick` path. `g_br2_bail_last_fn/ra` names the culprit.
- If not bail: check `frame_divisor_get()` and which `sub` (`0x1f80009c`)
  arcade selects; trace `select_screen_tick` return / `battle_frame` entry.
- Expose `g_bce_cls`/`g_bce_calls` via debug server to confirm whether
  `battle_config_eval` runs at all during arcade (it shouldn't until a
  battle phase cell is live).
- Do **not** fake the `0x700` write or the ph transition — faithful wait.
