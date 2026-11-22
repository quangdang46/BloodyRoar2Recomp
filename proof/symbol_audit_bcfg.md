# Symbol-resolution audit — battle spawn phase machine

Date: audit of `build/BloodyRoar2_Recompiled` (Mach-O arm64), build target `psx-runtime` + main exe, `cmake --build . -j8` clean.

## Verdict

**No weak stub is live.** Every audited symbol resolves to exactly one
strong (`T`) definition in the real `game/*.c` implementation. The
`__attribute__((weak))` stubs in `game/bind.c` (lines ~2212–2247) lost at
link time to the strong definitions, as the Mach-O/ELF linker rule requires
(strong always beats weak regardless of object order). `atos` on each
resolved address confirms the file:line of the strong implementation.

The bind.c weak stubs exist only so bind.c can reference these names when
the real TU isn't linked; they are never selected. **The Arcade/1P wedge is
NOT caused by a stub shadowing `bcfg_step_*` / `battle_config_eval`.**

## Resolution table (nm address → atos file:line)

| Symbol | Guest addr | fnmap.c route | clean_table.c wrapper | Resolved def (strong) | bind.c weak stub? |
|---|---|---|---|---|---|
| battle_config_eval | 0x80153544 | → `battle_config_eval` | wrap_80153544 → clean fn | menu.c:4008 (0x101666bb0) | yes @2238, dead |
| bcfg_step_a | 0x801533e0 | → `bcfg_step_a` | wrap_801533e0 → clean fn | menu.c:4207 (0x101667a58) | yes @2240, dead |
| bcfg_step_b | 0x80152548 | → `bcfg_step_b` | wrap_80152548 → clean fn | menu.c:4218 (0x101667fcc) | yes @2246, dead |
| bcfg_step_c | 0x8015279c | → `bcfg_step_c` | wrap_8015279c → clean fn | menu.c:4234 (0x10166778c) | yes @2245, dead |
| bcfg_step_d | 0x80152878 | → `bcfg_step_d` | wrap_80152878 → clean fn | menu.c:4257 (0x101667ee0) | yes @2244, dead |
| battle_state_gate | 0x801521a4 | → `battle_state_gate` | wrap_801521a4 → clean fn | menu.c:4176 (0x101668058) | yes @2247, dead |
| continue_countdown | 0x8015226c | → `continue_countdown` | wrap_8015226c → clean fn | menu.c:4535 (0x101668250) | yes @2243, dead |
| select_phase_gate | 0x80153484 | → `select_phase_gate` | wrap_80153484 → clean fn | menu.c:3606 (0x101667490) | none |
| name_rec_gate | 0x80163cbc | → `name_rec_gate` | wrap_80163cbc → clean fn | save.c:2631 (0x10169b1ac) | none |
| status_flag_cond_set | 0x801279d0 | → `status_flag_cond_set` | wrap_801279d0 → clean fn | game.c:1185 (0x101655220) | none |
| ret_zero_b | 0x80163898 | → `ret_zero_b` | wrap_80163898 → clean fn | save.c:2705 (0x10169940c) | none |
| fighter_spawn_setup | 0x801487ac | → `fighter_spawn_setup` | wrap_801487ac → clean fn | menu.c:3770 (0x101666490) | none |
| battle_frame | 0x8014cdd0 | → `battle_frame` | wrap_8014cdd0 → clean fn | menu.c:8076 (0x1016736c4) | none |
| battle_frame_tick | 0x80166544 | → `battle_frame_tick` | wrap_80166544 → clean fn | battle.c:339 (0x101616b54) | none |
| task_pools_tick | 0x801a49d0 | → `task_pools_tick` | wrap_801a49d0 → clean fn | battle.c:8321 (0x101628e74) | none |
| task_alloc_wrap_c | 0x80137e70 | → `task_alloc_wrap_c` | wrap_80137e70 → clean fn | battle.c:8969 (0x10162ee54) | none |
| deferred_cmd_post | 0x80156ef4 | → `deferred_cmd_post` | wrap_80156ef4 → clean fn | resolve.c:2539 (0x101693f6c) | none |
| confirm_bookkeeping | 0x8015113c | → `confirm_bookkeeping` | wrap_8015113c → clean fn | menu.c:3941 (0x1016675d0) | none |
| mc_pad_button_handler | 0x80153264 | → `mc_pad_button_handler` | wrap_80153264 → clean fn | save.c:1733 (0x101699340) | none |
| mc_slot_action | 0x8015260c | → `mc_slot_action` | wrap_8015260c → clean fn | save.c:2004 (0x101699414) | yes @2212, dead |
| pad_state_refresh | 0x8012a72c | → `pad_state_refresh` | wrap_8012a72c → clean fn | menu.c:2871 (0x1016634e0) | none |

## Dispatch-path details

- `game/fnmap.c`: exactly one entry per guest addr for all 22 addrs
  (verified `grep -c` = 1 each); each maps to the clean symbol name, which
  binds to the strong def.
- `src/clean_table.c`: each `wrap_*` calls the clean C function directly
  with `cpu->gpr[4..]` args (pointer args via `guest_at`, matching the
  AGENTS.md guest-address convention for buffer args). **None of the 22
  wrappers route through `br2_call_generated`.**
- Intra-clean calls are direct C calls, not guest-addr indirection:
  `battle_config_eval` (menu.c:4007+) calls `bcfg_step_a/b/c/d`,
  `continue_countdown`, `select_phase_gate`, `name_rec_gate`,
  `status_flag_cond_set`, `ret_zero_b` directly (menu.c ~4042–4170).
  `bcfg_step_a` calls `battle_state_gate` + `ret_zero_b` directly.
  `fighter_spawn_setup` (menu.c:3768) calls `battle_config_eval`,
  `select_phase_gate`, `confirm_bookkeeping`, `task_alloc_wrap_c`,
  `deferred_cmd_post` directly.
- No `br2_call_generated` appears inside any of the audited function
  bodies — the phase machine is fully clean-C, no partial delegation.

## Notes / non-issues

- `ret_zero_b` (save.c:2705) is `return 0` **by faithful port**, not a
  stub: save.c:2703 comment documents the guest function is a bare
  `jr ra; move v0,zero`. Same for `ret_zero_a/c`.
- `name_rec_gate` returns early when `*(int8_t*)guest_at(0x1f80009c)`
  (pad/mode byte) is in [2,6) — save.c:2633; that's guest-faithful gating,
  worth remembering if name records look skipped, but not a wedge cause.
- bind.c weak stubs of interest for future sweeps (same hazard class, not
  in this call path): `mc_slot_action`@2212, `battle_config_eval`@2238,
  `bcfg_step_a`@2240, `continue_countdown`@2243, `bcfg_step_d`@2244,
  `bcfg_step_c`@2245, `bcfg_step_b`@2246, `battle_state_gate`@2247 — all
  confirmed dead in this binary; the pattern is safe **only because** a
  strong def exists in another TU. Any future rename/removal of the strong
  def would silently resurrect the stubs — flag for `renames.map` hygiene.

## Conclusion

Symbol resolution is clean end-to-end for the spawn-phase call graph.
The Arcade/1P battle-spawn wedge must be sought elsewhere: guest-memory
state feeding `battle_state_gate` (battle+0x82 mode byte, +0xa4, +0x9b/0xb9
fields), `select_phase_gate` inputs, `continue_countdown` state, or the
upstream mode/phase driver that calls `fighter_spawn_setup` — i.e. a
runtime-state issue, not a link/dispatch issue.
