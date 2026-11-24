# op_anim_delta_walk (0x801A984C) — full block audit

Clean port: `game/save.c` `op_anim_delta_walk`. Generated reference:
`br2eu_func_801A984C` in `generated/SLES_017.22_full_52.c` (33 blocks,
0x801A984C–0x801A99DC).

## Verdict: FIXED — faithful port verified at runtime

`tools/mode_monitor.py 4403 row1`: MENU reached (fr≈6000), CONFIRMED,
gs cycles 3→4→5→6→3 continuously for 150s+/50k frames. The previous
faithful attempt's gs=5 wedge does NOT reproduce with the block-exact
port below — the earlier attempt must have carried a secondary
divergence (most likely the `a0==0` host-vs-guest pointer test or a
dv==0 LO polarity error).

## Block-by-block findings

- **Prologue**: `s3=a0`, `s2=a1`, `s1=0`; `beqz s3` @0x801A9868 tests the
  **guest address** a0, not a host pointer. Old clean tested
  `guest_at(a0)==0` which is never NULL (guest_at(0)=arena base) — dead
  guard. Fixed: `if (a0 == 0) return 0;` before `guest_at`.
- **Clamp**: `bgez a1`→keep, else s2=0; `slti v0,s2,0x1001`→if false
  s2=0x1000. Clean `if (s2<0) s2=0; if (s2>=0x1001) s2=0x1000;` ✓
- **Slot head** @0x801A988C/0x801A9894: s1==0→`lw [s3+0x34]`,
  s1==1→`lw [s3+0x38]`; `lw s0,[arr]`; `beqz s0`→slot_done;
  `lh cnt,[s0+6]`; `bltz cnt`→slot_done. ✓ (clean `goto slot_done`)
- **Inner loop** @0x801A98E8:
  `ev = *(u32*)(*(u32*)(s0+0x18) + a0o + 4)`; `dv = lh[s0+0xc]`;
  `a3 += ev` (gpr[7], wraps); `num = a3 << 12`;
  `div num/dv` → **LO**.
- **dv==0 (the masked mistranslation)**: div @0x801A9904 emulates
  MIPS div-by-zero → `LO = num>=0 ? 0xFFFFFFFF : 1`. The `break 7`
  @0x801A9910 is a recompiler **no-op** — execution falls through to
  `mflo` @0x801A992C and the normal `slt/xori/beqz` path. Faithful:
  `quo = (num>=0) ? -1 : 1`, then `quo<s2`→advance. Since s2>=0,
  num>=0 ⇒ quo=-1 ⇒ always advance; num<0 ⇒ quo=1 ⇒ advance iff s2>1.
- **Dead break-guards**: `break 7`@0x801A9910, `break 6`@0x801A9928
  (INT_MIN/-1), `break 7`@0x801A9958, `break 6`@0x801A9970 — ALL no-ops;
  every path converges on the two `mflo`s. No semantic effect.
- **Finish** @0x801A9944–0x801A998C: `gpr[3]=(s2-t0)<<12`,
  `gpr[2]=(xori_result)-t0 = 1-0 = 1` (t0/gpr[8] provably 0: cleared
  @0x801A98D0 and re-cleared every advance @0x801A999C);
  second div → LO = s2<<12; `bgez` skips +0xfff (s2<<12 ≥ 0 always);
  `jal 0x801A9C24` = **tween_apply(s0, a1c, sra(q2,12)=s2)**.
- **Advance** @0x801A9998: `a1c++`(gpr[5]), `t0=0`(gpr[8]),
  `cnt=lh[s0+6]` reload, `a2c++`(gpr[6]), `a0o+=0x14`(gpr[4]);
  loop while `cnt>=a2c` (`slt cnt<a2c; beqz→loop`).
- **Slot done** @0x801A99B4: `s1++`; `slti v0,s1,2`; loop while s1<2.
  All inner exits (s0==0, cnt<0, cnt<a2c, tween done) converge here.
- **Return**: v0 = `slt(s1,2)` at exit = 0; a0==0 early-out returns
  stale v0 (clean returns 0 — caller treats as bool, fine).

## Secondary mistranslation found during sweep (also fixed)

`fighter_anim_table_init` (0x80172A34), `game/save.c` ~4699:
guest @0x801736C8–0x801736E8 does `lh dv,[s0+0xb0]`; `lui v1,0x64`
→ numerator = **0x00640000 = 100<<16** (16.16 fixed-point);
`div; mflo; sw → 0x801f5228[i]`. Clean had `100 / dv` — off by 65536×.
dv==0 arm `0xffffffff` was coincidentally correct (LO=-1). Fixed to
`(100 << 16) / dv`. Verified: row1 monitor still healthy after change.
