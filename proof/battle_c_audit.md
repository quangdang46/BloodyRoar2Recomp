# battle.c faithfulness audit — div/MIPS-semantics sweep

Scope: `game/battle.c` (~18.3k LOC, largest clean-ported gameplay file).
Method: extracted all 121,049 guest instructions from
`generated/SLES_017.22_full_*.c` (`/* 0xADDR: 0xWORD */` comments), disassembled
the full map, enumerated every `div`/`divu` (126 sites), then attributed each
site to its containing CPS fragment (`br2eu_func_*`) and checked whether that
fragment's logical function is clean-ported in `src/clean_table.c`.

Key semantics verified against `generated/` + runtime
(`psxrecomp` MULDIV path): guest `div` has **no trap** —
div-by-zero yields `LO = (dividend>=0) ? -1 : 1`, `INT_MIN/-1` yields
`INT_MIN`. Clean C `/` would SIGFPE on the same inputs.

## Fixed (all use `mips_div_lo`, tagged `/* guest div @0xADDR */`)

`static int32_t mips_div_lo(num, den)` added at battle.c:1243 (same idiom as
resolve.c:120).

| Guest div | Clean site | Function | Divisor hazard |
|---|---|---|---|
| 0x80132030 | ~L2320 | battle_camera_step | `lh fighter+0x7a` can be 0 |
| 0x8017401C | ~L2510 | battle_hud_draw | `lh rec+0xb0` can be 0 |
| 0x80174294 | ~L2578 | battle_hud_draw | `lh +0xb0` |
| 0x80174448 | ~L2620 | battle_hud_draw | `lh +0xb0` |
| 0x80144C14 | ~L3860 | hit_react_apply | `lh +0x24` can be 0 |
| 0x80144EB0 | ~L4264 | hit_react_apply | `lh +0x24` |
| 0x80144EE0 | ~L4267 | hit_react_apply | `lh +0x24` |
| 0x80146268 | ~L4611 | camera_aim_offset | variable |
| 0x801462C8 | ~L4617 | camera_aim_offset | variable |
| 0x8018235C | ~L6347 | fighter_script_fsm | `lh +0x88` can be 0 |
| 0x80182B00 | ~L6070 | fighter_script_fsm | `lh +0x88` |
| 0x8013B3F0 | ~L13887 | iris_wipe_update | `local_40[0]<<2` can be 0 |
| 0x80140F94 | ~L16105 | firework_task_update | `_DAT_1f8003f8<<2` can be 0 |

Second fix in firework_task_update (~L16101): `_DAT_1f800024` was read as a
full u32; guest does `lhu` + `sll16/sra16` = sign-extend of the **low
halfword only**. Now `(int)(int16_t)(uint16_t)_DAT_1f800024`
(0x1f800024 is the screen-OX u16 per game.c:260).

## Audited SAFE (verified against generated)

- **seg_sphere_hit** @0x801454F0 (div @0x80145788): clean `iVar4==0→return 0`
  at ~L5642 is equivalent — guest div-by-zero LO=-1 fails the `<0x1001` check.
- **angle_from_xy** @0x80142B3C (divs @80142BA0/42BCC/42C04/42C28; 42C40 is in
  a generated fragment): all 5 divisors are abs values coerced ≥1.
- **particle_burst_spawn** @0x80136114 (divs @801361B8/362A4/3639C/36498 in
  sibling fragments): `iVar3/(iVar4+1)` guarded by `0 < iVar4` → den ≥ 2.
- **sprite_prim_update** @0x8018694C (divs @801869F4/80186A44): both
  divisors `lh +0xe`/`+0x12` guarded `> 0`.
- **fixed_point_divide** @0x8018D300 (divs @8018D33C/8018D360): `v0` = max of
  two abs values ≥ 1 after `v1==0` early-out; `t1 ≥ 0x20000`.
- **tween_seek** @0x801A99E0 (divs @801A9A5C guarded inline; @801A9AA4):
  second-div numerator/denominator both reduce by `t1` which is provably 0
  every iteration (reset at 801A9AF4) → clean `(iVar4*0x1000)/uVar1` with
  `uVar1==1` is exact.
- **tween_seq_step** (guard @~L14356 `(_den==0)?-1:0x1000/_den`) — already
  faithful; guest divs live in generated fragment anyway.
- **prim_group_alloc** (guard @~L11914 `param_6==0?-1:0x1000000/param_6`) —
  already faithful.
- **atan2_lut** (resolve.c:2638): divisor = max(|x|,|y|) ≥ 1.
- **trig_lut_read / trig_vel_flat / trig_vel_up / trig_vel_diag**
  (resolve.c): already use `mips_div_lo`.
- **impact_fx_update** @0x80138CEC: fully delegated
  (`br2_call_generated`) — generated handles its 4 divs.
- **op_anim_delta_walk** @0x801A984C: save.c, fixed in 9814b15.
- All `%` sites use constant divisors.

## Out of scope (divs in generated-only fragments — faithful by construction)

The remaining ~80 guest div sites live in fragments whose logical functions
are NOT clean-ported (unnamed gap functions like br2eu_func_8013B844 iris-wipe
worker, scriptop_move @8018444C/scriptop_div @80184914 which ARE clean but in
other files' audit scope, CD/BIOS/printf paths, etc.). Examples:
iris_wipe_spawn-range divs @8013B920-8013C6C0 belong to unported fragment
functions, not the tiny clean `iris_wipe_spawn` @8013B5A8.

## Exhaustive `/` sweep of battle.c

Every variable-denominator `/` in battle.c (19 sites) was enumerated and
classified above — no unguarded C `/` remains that corresponds to a guest
`div`. Constant-divisor `/` and `%` sites cannot SIGFPE.

## Runtime verification

- `tools/mode_monitor.py 4405 row1`: MENU → mode (0,0), gs cycled
  3→4→5→6→8 through battle/transitions for ~131 s, no wedge.
- `tools/arcade_fight_mash.py 4405`: ran to completion ("watch done");
  instance kept advancing — 622k+ frames, gs cycling 5↔6, no wedge.
  (mips_div_lo is bit-identical to C `/` for all nonzero denominators;
  it only changes the previously-SIGFPE den==0/INT_MIN edge cases.)
- Build: `cmake --build . -j8` clean.
