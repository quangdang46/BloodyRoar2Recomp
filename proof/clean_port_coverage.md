# Clean-port coverage analysis (2026-11-21)

## Coverage numbers

- `game/fnmap.c` named guest functions: **1345**
- `src/clean_table.c` clean wrappers (`wrap_<addr>`): **1279**
- Unwrapped (pure generated dispatch): **64**
- `br2_call_generated` delegation points inside clean `game/*.c`: **~119**
  (cd.c 58, save.c 36, battle.c 9, bind.c 8, menu.c 4, resolve.c 2, game.c 2)

=> ~95% of *named* functions have a clean C entry point. The remaining
generated load-bearing is concentrated in two buckets:

### Bucket A — CD/hardware boundary (~57 of 64 unwrapped)

`cd_*`, `mc_*`, `dev_*`, `cd_file_read_seq`, `mc_date_serial_cmp` etc.
These are unwrapped because guest→guest calls must run the generated
MIPS body (IRQ/deferred-coroutine boundary — the `br2_call_generated`
delegation contract in AGENTS.md §4). They already have clean *internal*
wrappers in `game/cd.c`/`game/save.c` for clean-C callers; the guest
dispatch table correctly routes external calls to generated. Porting these
cleanly requires reproducing live-guest-register + per-block-interrupt
semantics — the known-hard case, NOT a quick win.

### Bucket B — non-CD game functions (7 unwrapped, pure generated)

| Guest addr | Name | Gen size (lines) | Notes |
|---|---|---|---|
| 0x8012ecbc | player_light_apply | 927 | render/fx |
| 0x8012ee48 | camera_state_cache | 756 | camera |
| 0x80135180 | fighter_trail_emit | 439 | fx |
| 0x80138cec | impact_fx_update | 4531 | fx (huge) |
| 0x80184f28 | script_op_dispatch | 763 | script VM |
| 0x80186b2c | fighter_attach_run | 540 | attach |
| 0x80137acc | LAB_80137acc | 137 | small but CPS continuations + scratchpad reads |

None are trivial leaves — all have CPS/continuation points or sizable
bodies. `LAB_80137acc` (137 gen lines) is smallest but still splits on
`psx_check_interrupts_at` and reads scratchpad `0x1f800082`.

## Conclusion

The clean tree is ~95% ported for named functions. The residual
generated coverage is dominated by the CD/hardware IRQ boundary — the
structurally hardest class — not by easily-portable game logic. The next
meaningful coverage gains require either (a) faithfully porting the
IRQ-boundary functions (needs live-guest-register + per-block-interrupt
expression in clean C — the delegation-rule hard case), or (b) porting the
7 non-CD functions above (medium/large, need runtime verification).

This is honest status, not a blocker: versus/arcade combat is verified
working on HEAD (see cd_dir_entry_progress.md), so the generated
fallback is load-bearing but the game runs.
