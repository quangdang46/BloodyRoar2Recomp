# ev/ani/action faithfulness audit — ev.c, ani.c, action.c (+fight/gfx/game spot-check)

Commit: `c8a4557` on master (pushed to `mine`).

## Confirmed divergences found + fixed

### 1. `game/ev.c` `ev9_physics` op4 — MIPS div-by-zero sign (FIXED)
- Guest `div` @`0x8017B97C` (generated `br2eu_func_8017B860` region,
  full_35.c ~line 6940): `lo = (dividend>=0) ? 0xFFFFFFFF : 1` when
  divisor `p2 == 0`; dividend = `(min(p1,ctrl+0x76)) << 7`.
- Clean had `p2 ? (a1<<7)/p2 : -1` — constant `-1`. For `a1 < 0` the
  guest stores `+1` (0x0001), clean stored `0xFFFF`.
- Fix: `: (a1 < 0 ? 1 : -1)` — dividend sign == a1 sign (s16-range, <<7
  cannot wrap).

### 2. `game/vm.c` `ev4_branch_dispatch` — lhu vs lh on fail-path offset (FIXED)
- Guest @`0x8017AE00` (`br2eu_func_8017AE00`, full_34.c:30852):
  instruction `0x94C30002` = **LHU** (opcode 0x25), zero-extends
  `pc+2`, adds `lw(ctrl+0x30)` (.ANI base), `jr $ra`.
- Clean had `(int32_t)(int16_t)G16(p,2)` — sign-extended. For
  branch-table offsets >= 0x8000 the target lands 0x10000 early.
  (`ev.c`'s live `ev4_branch` already used `GU16` correctly; this was
  the fnmap-dispatched twin.)
- Fix: `(uint32_t)GU16(p, 2)`.

## Verified faithful (insn-checked vs generated, no change needed)

- `game/fight.c` `pending_gate` vs `br2eu_func_80178664` (full_34.c:4):
  full permission-mask decode walked block-by-block — req_a&0x800 /
  motion+0x18&0x10 cancel-permit, req_b&0x800 → &4/&8 masks,
  req_b&0x10 → &2/&1 masks, deny → ctrl+0x6c &= ~0x100 + ret -1,
  accept → pend=lh(motion+0x1c), store -1, pending_hit_a_clear(0x80142DA0)
  + hit_chain_clear(0x80142D20), ret pend+0x90. All match.
- `game/ani.c` `fixed_rsqrt_q12` vs `br2eu_func_80194500` (full_44.c:27042):
  LZCS→LZCR==32→ret 0; lz&=~1; sh=(31-lz)>>1 sra; mantissa sra/sll split
  at lz-24; idx=(m-64)<<1 into 0x801B8480 via 0x801C0000-31616;
  table load is LH (0x85AD8480); result = (v<<sh)>>12 logical. Match.
- `game/ani.c` `anim_angle_approach` vs `br2eu_func_801858F8`
  (full_39.c:24754): a2 negate, (2d-x)*a3 mflo, +4095 floor on neg,
  sra12, xu(lhu)+q store. Match.
- `game/ev.c` `ev14_chip` op3 scale path vs `br2eu_func_8017C35C`
  (full_35.c:19917): ctrl+0x6c&0x100 selects d6*2+0x12BFB vs
  d6*3+0x1A3F9; mult→mflo; magic 0xDA77B1C9 hi+(x) sra17 -(x>>31) —
  `ev_fixdiv` reproduces exactly; +0x3a accumulates the scaled result;
  foe +0x112 = flag&0x10000000 ? 0 : 0x40; +0x114 threshold chain
  (f==8→0x4b, f<9→{0→4,else chip}, f==0x10→0x4c, f==0x20→4, else chip
  thresholds 0x1a/0x15). Match.
- `game/ev.c` `ev17_approach` vs `br2eu_func_8017C7B0` (full_35.c:23672):
  SX12 dist = sx16(ctrl+0x74)+sx16(mo+0xd2) <<20>>20, abs, thresh gate
  (ctrl+0x5c&0x40 ? lh(pc+2) : 0x400), fixdiv magic 0xB40B40B5 sra7
  -(x>>31) → +0x10a, thresh → +0x10c; tail ctrl+0x5c = (old&0xF7FFFFFF)
  then bit0x20000 = (+0x10a!=0). Match.
- `game/action.c` `apply` div site vs `br2eu_func_8017A05C` @0x8017A24C:
  real guest `div`; div==0 gives LO=-1 (dividend cfgbyte<<8 always >=0),
  +1 → ctrl+0xf8 = 0 — clean `div ? … : 0` coincidentally identical.
- `game/action.c` `reset` /100 vs `br2eu_func_80175898`: guest uses
  magic-multiply signed div-by-100 on the LOW 32 bits of
  lh(ctrl+0xb0)*lbu(ctrl+0xde); clean `(int32_t)(u32 product)/100`
  matches (signed, trunc-toward-zero).
- `game/action.c` `reset` ctrl+0x14 = 0x801F52E8 + ctrl[2]<<5 vs
  generated @0x801758A4-0x801758B4 (0x801F0000+21224=0x52E8). Match.
- `game/ev.c` `ev10_flags` head decode vs `br2eu_func_8017BA3C`
  (full_35.c:8496): val = lhu(pc+2)|lhu(pc+4)<<16; cls = u8(pc+1)&0xC0
  four-way gate (0x40 flag-set→pc+6, 0x80 flag-clear→pc+6, 0xC0 foe,
  else ctrl). Match.
- `game/gfx.c` console-pct radix: uVar10 is always 0x10 on the div path —
  `/`/`%` cannot trap.

## Suspects left (not confirmed, documented)

- `game/ev.c` `ev4_branch` inline op table (ops 0-26) vs the 27 generated
  case handlers @table 0x8012272c — claimed verified at port time; a full
  27-case re-diff was out of scope. The shared fail path (GU16) is correct.
- `game/ev.c` `ev2_hitstun_gate` inline negative cases vs generated case
  bodies 0x8017AA38..0x8017AB04 — partially spot-checked (motion+0xcc/0xca
  pair seen at 0x8017AA38 consistent with the cc==0&&ca<=0 shape); full
  7-case diff pending.
- `game/ev.c` `ev12_damage` 8-case jump table @0x801228EC — cases
  0/1/2/3/4/7 read; cases 5/6 share the BF64 tail (verified shape).
- `game/ani.c` `gte_vec3_normalize_core` GPF tail — front half verified
  identical to `fixed_rsqrt_q12`'s proven sequence; GPF register writeback
  (sra by sh) assumed from shared structure.
- `game/fnmap.c:2151` duplicate `{0x80185e6c → ev_step}` after
  `fighter_anim_advance` — fn_lookup binary-search lower_bound returns
  the FIRST match (fighter_anim_advance), so it's dead weight today, but
  `ev_step` has a mismatched signature (`uint8_t*(*)(u8*,u8*,u8*)` vs
  `void(*)(void)`) and would corrupt state if ever selected. fnmap.c is
  auto-generated — regenerate/fix the generator, don't hand-edit.
- `game/ev.c` `ev_step` + `ev5_action` static path is dead code (live
  path is `ev_stream_dispatch` → `PTR_FUN_801b4e68` → `ev5_action_select`);
  divergences between the two EV5 implementations are latent only.

## Coverage

ev.c: dispatcher, EV0-21 handlers, EV5 select (both impls), EV10/12/14/17
spot-verified insn-level; ani.c: all 8 guest fns (normalize core x3,
rsqrt x2, angle_approach, ret_nop, container accessors reviewed);
action.c: apply/reset/ani_fixup/slot_apply/entry_setup/part_link*
reviewed + div sites verified; fight.c: pending_gate verified insn-level,
node_list_walk/part_link_tail/hud_emit reviewed; game.c/gfx.c: all `/`,`%`
sites triaged (constant or proven-nonzero divisors).

## Runtime verification

`python3 tools/mode_monitor.py 4407 row1 150` on the fixed build:
MENU reached at fr≈3689, game cycles gs 3→4→5→6 load states continuously
to fr≈47.6k, CD reads progressing (last_sector lba advancing, mode 0xA0/
0xE0), no wedge, no SIGFPE. Row1 path exercises apply/reset/ev dispatch
through intro+demo loads.
