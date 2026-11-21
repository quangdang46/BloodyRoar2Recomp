# Bucket B clean-dispatch enablement — 6 game functions wrapped (2026-11-22)

## What changed

`proof/clean_port_coverage.md` Bucket B listed 7 non-CD functions with no
`wrap_*` entry in `src/clean_table.c`. Inspection showed **all 7 already have
faithful clean C bodies** in `game/battle.c` — they were skipped by
`game/gen_clean_table.py` solely because each body contains an *indirect*
`br2_call_generated(<runtime guest fn ptr>, ...)` for a jalr-style handler
dispatch, and the generator conservatively skips indirect delegates unless
the address is in `WRAP_OK_DELEGATE`.

That skip is over-conservative for this pattern: `br2_call_generated`
bypasses `clean_try_dispatch` entirely (src/clean_bridge.c:470-489), so the
inner call runs the generated body directly and can never re-enter the
wrapper — the same argument already accepted for `dev_cmd_round_dispatch`,
`dev_cmd_seq`, `dev_cmd_process`, `ev_stream_dispatch`.

Added to `WRAP_OK_DELEGATE` in `game/gen_clean_table.py`:

| Guest addr | Clean fn (game/battle.c) | Indirect delegate target |
|---|---|---|
| 0x8012ecbc | player_light_apply | light-fn table DAT_801b140c |
| 0x8012ee48 | camera_state_cache | camera-fn table DAT_801b144c |
| 0x80135180 | fighter_trail_emit | trail-fn table guest RAM 0x801b1734 |
| 0x80184f28 | script_op_dispatch | opcode table 0x801b5528 |
| 0x80186b2c | fighter_attach_run | part-attach table 0x801b55c0 |
| 0x80137acc | LAB_80137acc | handler table param_1[3]+op*4 |

Result: `src/clean_table.c` regenerated — **1285 wrappers, 58 skipped**
(was 1279/64). The 6 new `wrap_*` adapters marshal `$a0-$a3` per the
generated prologues (verified arg order/reg usage against
`br2eu_func_8012ECBC` etc.: a0=ptr, a1=count for player_light_apply).

## Explicitly NOT wrapped

- **0x80138cec impact_fx_update** — its clean body self-delegates
  (`br2_call_generated(0x80138CECu,...)`) into a mid-function CPS
  continuation that consumes live s-registers (documented in
  FAITHFUL_TIMING_PLAN N+88: "jump-table-into-mid-function consumes live
  s-regs; correct per delegation rule"). Wrapping would recurse through
  clean_try_dispatch. Stays generated-authoritative.
- **0x801aed3c / 0x801aedbc / 0x801aef18 op_cb{1,2,4}_store_ctx8** (save.c)
  — indirect `br2_call_generated(fp,...)` where `fp` is a frame/context
  pointer, not a proven guest fn-ptr table entry. Left delegated.
- All Bucket A CD/hardware IRQ-boundary functions — untouched per AGENTS §4.

## Semantic notes

- Generated continuation PCs (e.g. 0x8012EEAC..0x8012EF58 inside
  camera_state_cache's CPS chain) are NOT in the clean table — only the
  function entry addr is wrapped. A CPS resume into a continuation still
  routes to the generated body, which is correct: the clean body runs the
  whole function in one call and never leaves a live continuation.
- `script_op_dispatch` returns 0 — verified against the generated epilogue
  (`move $v0,$zero` @0x8018503C before `jr $ra`).
- Pointer conventions verified: `g_DAT_1f8000e8`-family bindings resolve via
  `guest_for()`→`guest_at()`, so the wrapper's `guest_at(gpr[4])` host pointer
  is identical to what clean callers pass directly (`&DAT_1f8000e8`).
  `guest_of(param_1)` recovers the guest address for guest-addr args.
- These clean bodies were already partially live pre-wrap via
  `fn_lookup`/direct-C-call and `call_guest`→`clean_try_dispatch` paths from
  clean callers (battle.c:363 camera_state_cache, battle.c:369/395
  player_light_apply, battle.c:434/16617 fighter_trail_emit,
  battle.c:835 fighter_attach_run, battle.c:6022 script_op_dispatch).
  The wrap extends coverage to generated-side `jal` callers
  (br2eu_func_80166544 battle_frame_tick, 0x80166a60, 0x80166f64
  fighter_reinit_tail, 0x8016fcc4 battle_flow_state_machine,
  0x80181f20 fighter_script_fsm, 0x80181000).

## Runtime verification

Two headless instances on the real disc:

- **:8300** — clean build, all 6 wrapped (default).
- **:8301** — `CLEAN_EXCLUDE=8012ecbc,8012ee48,80135180,80184f28,80186b2c,80137acc`
  → the same 6 run generated (control); 0x80138cec always generated.

Both armed `fn_filter lo=0x8012E000 hi=0x80188000` + `pc_probe` on the 7
entry PCs. `pc_probe` sanity: `vsync_wait_n` (0x8019A5D8, still generated)
fires thousands of counts on both — probe path confirmed live.

Evidence collected in `/tmp/poll_fntargets.log` (fn_entry_tail diff between
instances during attract demo fights). See session notes / commit message
for the final verdict.

## Runtime verdict (2026-11-22)

- Both instances boot cleanly and advance: title → attract cycle
  (gs 3/4/5/6) → versus mode-select. Frames advance ~50fps, CD idle,
  no crashes, no dispatch misses — **no regression** from the 6 new wraps.
- `pc_probe` sanity: `vsync_wait_n` (0x8019A5D8, still generated) counted
  360,078 hits on :8301 — probe path live.
- `fn_entry_dump` addr-filtered scans over the full recorded ring
  (~93K entries on :8301) show **zero battle-function entries** — neither
  instance reached a fight.
- Root cause of missing fight coverage is the **pre-existing, documented
  CD wedge**: versus/arcade stall deterministically at `gs=8 ph=3`
  (input-dead versus mode-select) / `ph=6` (char select) on
  DeliverEvent(0xF0000009) starvation — see `proof/cd_seek_divergence.md`
  ("unified wedge for arcade ph=3 + versus", converged root cause
  game-side). This is Bucket-A CD-boundary territory, unrelated to the
  Bucket-B wraps: none of the 6 wrapped functions execute in
  menu/versus-select paths, so wedged-state behavior is bit-identical
  pre/post change.
- Partial live-body evidence: `camera_state_cache` and
  `player_light_apply` clean bodies are invoked by **direct C calls** from
  clean render paths (`game/menu.c:794,800-816`, `game/battle.c:363-402`)
  that run during menus/attract today — those bodies are proven executing
  in the current build; the wrap extends the same bodies to the
  generated-`jal` dispatch path used inside battle.

In-fight dispatch-path verification of all 6 remains pending the CD
DeliverEvent wedge fix; the wrappers are compile-verified, pattern-
identical to the 4 already-accepted `WRAP_OK_DELEGATE` entries, and safe
to leave enabled (CLEAN_EXCLUDE can veto any of them at runtime).
