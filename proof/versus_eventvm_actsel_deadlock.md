# Versus/attract combat hang — event-VM infinite loop on corrupt `motion+0xa4`

Status: **diagnosed, reproducible, NOT yet fixed.** New signature distinct from
the earlier `DAT1=6` CD-timer spin and the `battle_config_eval` phase deadlock.

## Symptom

Headless versus/attract run reaches combat (`g_game_state=8`, `DAT1` cycling
through 0/3/4/8, frame counter advancing for minutes) and then **hard-hangs**:
the debug TCP port stops responding (`PORT-DEAD`) and the
`starvation_watchdog` kills the process after ~413 s without a heartbeat.

Reproduced on two independent launches (debug ports 8900→dead, 8911). Sampled
stack is identical both times:

```
psx_scheduler_run → psx_dispatch_impl → dirty_ram_dispatch
  → psx_dispatch_game_compiled → clean_try_dispatch
  → battle_flow_state_machine → fighter_frame → action_commit
  → ev_stream_dispatch → ev5_action_select   (clean C, spinning in guest_at)
```

The host never returns from a single `ev_stream_dispatch` call, so the
scheduler's debug-server poll and heartbeat starve → watchdog abort.

## The loop (guest event stream @ 0x80070d5e..0x80070d64)

Three opcodes cycle forever (verified by reading guest RAM under lldb and by
the `decomp`/event trace):

| pc        | op  | handler   | effect                                             |
|-----------|-----|-----------|----------------------------------------------------|
| 0x80070d64| 0x01| altstream | b=0 → jump to `motion->MO_ALT` (=0xd5e)            |
| 0x80070d5e| 0x05| actsel    | ctrl=0x00 → op=0,param=0 → case 0 count check fails → return pc+4=0xd62 |
| 0x80070d62| 0x02| hitstun   | b=-1 → MO_CC==0 && MO_CA<=0 → return pc+2=0xd64     |

`ev_stream_dispatch` loops `while (npc_g != 0)`; none of the three handlers
ever returns 0 and none mutates the stream pc, so it is a closed cycle.

## Root-cause field

`ev5_action_select` case 0 (`game/ev.c` ~line 1234) gates on:

```c
if (iVar9 < *(short *)(p2 + 0xa4)) { ... fighter_anim_advance ... }
```

`p2+0xa4` is `motion+0xa4` (the .ACT reaction-table count). In BOTH hung runs
it held a **garbage negative** value while every other initialized field was
correct:

- run A (port 8900): `motion+0xa0=1, +0xa2=0xf7, +0xa4=0xc59b, +0xa8=0x80018904, +0xac=0x8007c3c4, +0xc0=1`
- run B (port 8911): `motion+0xa0=1, +0xa2=0,   +0xa4=0xd33b, +0xa8=0x80018904, +0xac=0x8007d164, +0xc0=1`

`+0xa8` is identical across runs (fixed table), `+0xac` is per-run heap, and
`+0xa4` differs per run → consistent with **uninitialized / mis-relocated data
read from the .ACT cursor**, not a fixed wrong constant.

Because `+0xa4` is negative, `0 < s16(+0xa4)` is false → actsel takes the
`break` path and returns `pc+4` **without** calling `fighter_anim_advance`, so
no state ever changes and the loop is truly infinite (the guest would exit only
if actsel advanced the reaction, or hitstun failed).

## Where `motion+0xa4` is written

`game/action.c` `apply()` (clean port of guest `func_8017A05C`, reached via
`apply_guest` @ 0x8017a05c). Lines ~205-222:

```c
uint32_t cur_g = GU32(motion, MO_AC);        /* .ACT cursor, +0xac */
uint8_t *cur  = (uint8_t *)guest_at(cur_g);
uint16_t v    = GU16(cur, 0);                 /* <-- this is +0xa4 */
GSU32(motion, MO_AC) = cur_g + 4;
GSU16(motion, 0xa4) = v;                      /* line ~214 */
/* or the same-char foe-mirror path: +0xa4 = GU16(guest_at(cur-4),0) */
```

`apply()` demonstrably ran: it also sets `motion+0xa0=1` and `motion+0xc0=1`
(action.c ~246-247), and both are `1` in the dumps. So `+0xa4` was actively
written — with a value read out of the **.ACT blob** at the relocated cursor.

The `.ACT` cursor (`motion+0xac`) and the `.ACT` secondary list (`+0xa8`) are
produced by the pointer-relocation block in the same `apply()`
(action.c ~166-203): stored file offsets are `+= base` where
`base = ctrl->FC_ANI` (the loaded .ANI/.ACT blob guest address). Two candidate
failure modes, not yet distinguished:

1. **Clean-port mistranslation** in `apply()`'s relocation/cursor math →
   `MO_AC` points into wrong bytes → `v` = garbage. (Would be a `game/action.c`
   fix.)
2. **Corrupt .ACT source data** — the fighter `.ANI/.ACT` blob delivered by the
   CD loader is wrong (ties back to the long-running CD asset-load wedge) →
   even a correct `apply()` reads garbage. (Would be a `cdrom.c`/`game/cd.c`
   fix.)

## Why this matters for the versus wedge

This is a *combat-runtime* hang, downstream of asset load. It is consistent
with (but does not prove) the theory that the CD path sometimes delivers a bad
fighter asset, after which the event VM deadlocks the whole process. It is a
separate, more severe signature than the soft `DAT1=6` stall: there the game
waited politely; here the host spins inside one clean-C call and starves the
debugger.

## Evidence / reproduction

- Two hung PIDs sampled with identical stacks (see above).
- `motion` guest addr `0x801f5328` is a fixed per-slot scratch region; host
  base = `memory_get_ram_ptr()` (`g_psx_ram`). For PID 8911 run:
  `g_psx_ram=0x111f8c008`, `motion+0xa0` @ host `0x1121813d0`.
- Guest stream bytes @ `0x80070d38..0x80070d77` dumped via lldb confirm the
  opcodes/operands above.
- `starvation_watchdog` fired at ~413 s in `/tmp/br2_8917.log`.

## Next steps (for the fix session)

1. Diff clean `apply()` (action.c ~94-290) against generated `func_8017A05C`
   instruction-by-instruction, focusing on the `moffs[]`/`toffs[]` relocation
   adds and the `.ACT` once-relocation (`GU8(ctrl,FC_FLAGB)==0` block) — a
   single wrong `base`/offset produces exactly this garbage `+0xa4`.
2. If `apply()` is faithful, dump the `.ANI/.ACT` blob (`ctrl->FC_ANI`,
   `motion->MO_AC` target) and check it against a known-good sector/LBA from
   `cdrom_sector_history` → proves/disproves the CD-corruption branch.
3. Consider a defensive (but faithful) check: the guest relies on `+0xa4`
   being a valid count; there is no bounds guard in `ev5_action_select`, so a
   corrupt `.ACT` is fatal by design — the fix must be upstream, not a clamp.
