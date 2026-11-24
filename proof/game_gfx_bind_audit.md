# game.c / gfx.c / bind.c faithfulness audit + flagged-suspect deep dive

Pass over `game/game.c`, `game/gfx.c`, `game/bind.c` plus the flagged
suspects from `proof/ev_anim_action_audit.md`. Method: guest MIPS ground
truth taken two ways — (a) `generated/SLES_017.22_full_*.c` CPS bodies and
(b) **raw disassembly of `disc/SLES_017.22`** via capstone (needed because
the EV4 op-table case bodies at 0x8017AC64..0x8017ADF4 have *no* generated
coverage at all — the clean port is the only implementation; verified
against the actual instruction bytes instead).

## Confirmed divergences

**NONE.** Every audited function matched the guest insn-for-insn. No code
changes were required; nothing to rebuild or runtime-verify.

## Flagged suspects — resolved

### 1. `ev4_branch` inline op table (ops 0-26) — VERIFIED FAITHFUL
Guest `br2eu_func_8017AB74` @0x8017AB74 + jump table @0x801228EC…
correction: table @**0x8012272C** (27 entries, dumped live from
Game.bin). Full disasm 0x8017AB74-0x8017AE14 walked op-by-op:

- Head decode: `lhu v0,(a2); srl 8; andi 0xc0` cls, `andi 0x3f` op,
  `sltiu 0x1b` fail — matches clean `GU8(pc,1)&0xc0` / `&0x3f` / `op>=27`.
- cls==0x40: `lbu 5(a0)&1` set → return pc+4; clear → t=ctrl. ✓
- cls==0x80: bit CLEAR → pc+4; set → t=ctrl. ✓
- cls==0xc0: `lw a3,0x2c(a0)` foe ctrl. ✓
- Fail stub 0x8017AE00: `lhu v1,2(a2); lw v0,0x30(a0); addu` →
  ctrl+0x30 + **lhu**(pc+2). Clean `GU16` zero-extend correct (prior fix
  holds).
- Pass stub 0x8017AC44: `jr ra; addiu v0,a2,4` → pc+4.
- ops 1-4: `lw 0x94(a1)` = motion FLAGT s32; beqz/bltz/bgez polarity all
  match (op1 !=0, op2 ==0, op3 <0, op4 >=0).
- ops 5-9: fail path stores `sw v0,0x8c(a1)` where v0 = `addiu a2,4`
  computed in an earlier delay slot — unconditional on fail, absent on
  pass. Clean `GSU32(motion,MO_8C)=guest_of(pc+4)` gated on `!pass` ✓.
- op10/11: `lbu 5(a3)&1` → jumps into AC38/AC50 shared tails; polarity
  (set→pass / clear→pass) ✓.
- op12: `lh 0xcc(a1)` bnez→pass; `lh 0xca(a1)` blez→fail — pass =
  `!(cc==0 && ca<=0)` ✓.
- op13: `lbu (a3)&0x80` → pass when clear ✓.
- op14: `lbu 5(a3)&0x10` set→fail; `lh 0xd6(a3)` slti 0x1e00 → pass.
  Clean `G16(t,FC_D6)<0x1e00` — FC_D6=0xd6, signed lh ✓.
- op15 = same target as op0 (AE00, always fail) ✓.
- op16: `lbu 3(a3)` unmasked !=0 → pass ✓.
- ops17-26: `lbu v1,3(a3)` vs li N=op-16 {1..0xa}; beq→fail — clean
  `GU8(t,3)!=(uint8_t)(op-16)` ✓ (op20 has inline beq+addu delay-slot
  pc+4, others share AD78 tail — same result).

### 2. `ev2_hitstun_gate` negative cases — VERIFIED FAITHFUL (all 7)
Jump table @0x8012270C dumped from Game.bin; index = sx8(pc[1])+7:
{idx0→AB04, 1→AAEC, 2→AAC4, 3→AAA8, 4→AA84, 5→AA60, 6→AA38}.
Return stubs: 0x8017A9FC = `addiu v0,a2,2` (pc+2 = PASS),
0x8017AA04 = `move v0,zero` (0 = FAIL) — polarity confirmed by reading
both 8-byte stub bodies.

- b=-7 (AB04): `(ctrl+0x5c & 0x28)!=0 → pass; else lh(ctrl+0xbc)>0 →
  pass` — clean case0 `ST&0x28 || G16(FC_ATK3=0xbc)>0` ✓ (lh signed).
- b=-6 (AAEC): `lh(ctrl+0xd2)==0 → pass; else → AA38 cc/ca tail` —
  clean `FC_D2==0 || (cc==0&&ca<=0)` ✓.
- b=-5 (AAC4): `lh(ctrl+0x94) - lw(obj+0x178) < 231 → pass` — clean
  `< 0xe7` signed, same operands (obj = lw ctrl+0x20) ✓.
- b=-4 (AAA8): `lw(ctrl+0x5c)&0x28 !=0 → pass; else → AA38 tail` —
  clean `(ST&0x28)!=0 || (cc==0&&ca<=0)` ✓.
- b=-3 (AA84): `lw(ctrl+0x2c)=foe; lw(foe+0x5c)&0x80 ==0 → pass` —
  clean `(GU32(foe,FC_STATE1)&0x80)==0` ✓.
- b=-2 (AA60): `lw(ctrl+0x20)=obj; lw(obj+0x38) >= lh(ctrl+0x94) →
  pass` (slt+beqz) — clean `>=` signed ✓.
- b=-1 (AA38): `lh(mo+0xcc)!=0 → fail; lh(mo+0xca)<=0 → pass` —
  clean `cc==0 && ca<=0` ✓.

### 3. `ev12_damage` jump-table cases 5/6 — VERIFIED FAITHFUL
Table @0x801228EC (8 entries) dumped from Game.bin:
{BDE0,BE38,BEBC,BEDC,BF00,BF54,BF60,BF78}.
Head @0x8017BD94: `lhu v0,(s0); sll16; sra24` = sx8(pc[0]);
`sltiu 8`; `move a3,a0` (a3=ctrl), s0=pc. Clean op decode ✓.

- case5 @0x8017BF54: `lh a0,0xc2(a3=ctrl)` → shared BF64:
  `lhu v0,0xcc(a1); sll v1,a0,4; addu; sh → mo+0xcc` =
  `mo.cc + sx16(ctrl+0xc2)*16`. Clean identical ✓.
- case6 @0x8017BF60: same with `lh a0,0xbe(a3)` = ctrl+0xbe ✓.
- case7 @0x8017BF78 spot-checked: `lh a0,2(s0=pc); sll4` added into
  mo+0xca, +0xc2, +0xc8 — clean adds `G16(pc,2)<<4` to all three ✓.

## game.c — verified faithful (insn-checked vs disasm)

- `sprite_node_alloc` @0x8018CDF8: stride math (v1*0x34 via
  sll/addu chain), gen/gen-1 reject pair, cursor wrap `slti 8`,
  `*slot=gen`, ret slot+4 — all match (host-pointer widening is the
  documented signature change, sole in-file callers).
- `sprite_node_emit_a` @0x8018CE70: full arg map confirmed
  (a0-a3 = p1-p4, stack 0x48-0x5c = p5-p10; s6=p6, s2=p10). All 8
  coord stores, tag select (p10!=0 → 0x3a000000|(p10-1)<<5 else
  0x38000000/tag0), prim_packet_init_tex(node+0x24, B7, B6, tag, 0),
  OT link `*head|0x08000000` / `*head=node&0xffffff` /
  `node+0x24=(node&0xffffff)|0x02000000` / `*head=(node+0x24)&0xffffff`
  — identical.
- `u16_lut_lookup` @0x8018E3BC, `lut_mask_or` @0x8018E3F4,
  `lut_mask_clear` @0x8018E424, `u16_lut_slot_clear` @0x8018E458:
  `andi 0xffff` index, `slti 3` signed-safe (idx ≤ 0xffff), table
  bases 0x801b66e0/e4/e8, store unconditional + ret `(idx<3)` / `1` /
  `0` — all match.
- `ctype_adj_m32` @0x8018EC30 / `ctype_adj_p32` @0x8018EC64: ctype tbl
  0x801b6701[a0&0xff], bit2→-32 / bit1→+32, `sll/sra 24` int8 result —
  match.
- `file_rec_size_get` @0x8018CFB4: `bltz a0`→0, else
  `lw 4(0x801b5b94 + a0*8)` = 0x801b5b98+a0*8 — match.
- `clear_battle_counters` @0x8012755C: zero stores 0x7e,0x7c,0x74,0x68
  (u16,u16,u32,u32) — match.
- `set_graph_debug` @0x80190F74: flag @0x801B8322, old-byte return,
  `andi 0xff` gate, cb(0x80125444, newflag, flag[-2], flag[1]) via
  PTR @0x801B831C — match.
- `memcard_info_query` @0x8015F590: 13-arg mc_data_read marshal
  (a0=0,a1=t[0],a2=t[1],a3=t[2], stack t[3..9]+0x801c11e0+0x414);
  return-0-for-all-nonzero-r path (`slti 8`/`bgtz`/`bnez` chain) then
  clear 0x801c15f0 + lbu(*(0x1f8000c0)+0x22) — match.
- `debug_stat_print` @0x8012A088: scroll_offset_set(0,0xdc),
  u16@0x801E83A8 select ED40/ED54, lhu 0x1f80001a/1c, jal 0x8012CC6C —
  match.
- `rand_seed` @0x8018E500: `sw a0,0x801f68e0` — match.
- `frame_counter_bump`: `sb (a0+1),0x1f800012` — match.

## gfx.c — verified faithful

- `spr_desc_slot_alloc` @0x8018C18C: idx*0x24 slot math, reject on
  `slot[0]==gen || ==gen-1`, wrap `slti 0x20`, store order
  (index then slot[0]=gen), ret slot+4 — match.
- `sprite_quad_emit` @0x8018C31C: full 97-insn diff — s2=((s0>>16)&
  0xff)+1, s3=(s0>>24)+1 (srl!), sx16(s5/s6) via sll/sra, mult/mflo
  with +0xfff neg-round then sra12, sin=0x8018D29C→s0 / cos=0x8018D298
  →v0 (renames.map confirmed), all 8 corner products with +0x1fff
  round/sra13, corner store offsets 8/a/10/12/18/1a/20/22 pairing
  x=(s7+t), y=(s7>>16+t), tag 0x2c808080 @+4, link `*out|0x09000000`
  then `*out=(node)&0xffffff`, ret 0 — identical.
- `gpu_prim_pkt_build` @0x801921C0: coord-clamp calls (sx16 args),
  coord_pack, pkt_pack(env+0x17, env+0x16, env+0x14), texpage_pack
  (env+0xc), 0xe6000000 tag, env[0xc] gate, local {env0..3} copy,
  x/y clamp to DAT_801b8324/26 (sx16 compare, u16 store), aligned
  check `(env0&0x3f)==0 && (clampedX&0x3f)==0` → 0x2000000 path vs
  0x60000000 textured path with env0-env4/env1-env5 deltas, dst[3]=
  t0-1 (6 or 9) — all match including delay-slot store ordering.
- `prim_persp_ot_emit` @0x8018B76C (+`_112` @0x8018B98C, tail
  @0x8018B884): word-level decode of the cop2 stream — lwc2 0-5,
  cnt-- before RTPT, t1/t2 = cur+0x18/0x1c, cfc2 FLAG **before** NCLIP
  (stale-by-one), mask 0x7FC40000 skip, unconditional `sw SXY0` in the
  blez-MAC0 delay slot, SXY1/2 stride stores, post-RTPS SXY2 re-read,
  sx cull via `psx_ws_cull_sltiu(.,512)` + sy `slti 480` on sra16,
  sx_ok==0 / sy_ok==0 skips, AVSZ4→OTZ slti 0x800 (2048) reject,
  swl pair `swl(ot+2, base<<8)` / `swl(base+2, old<<8)` —
  `swl_guest` merge table byte-identical to `psx_swl` in
  generated/SLES_017.22_decls.h:86. Match.
- `printf_core` radix div: uVar10 ∈ {8,10,16} on every path to the
  digit loop (set at each case label) — `/`/`%` cannot trap; the
  empty `if (uVar10==0)` is dead-but-harmless.

## bind.c — verified faithful

- `FUN_8019E4C8` @guest 0x8019E4C8: DI→B0:0x56 live-a0-a3→
  p=*(obj+0x18), dst=((lw(p+0x70)&0xffff)<<16)+(lw(p+0x74)&0xffff)+0x28,
  5-word copy 0x8019E4A0..B4, `*(0xdffc)=dst_final`, jal
  gpu_state_patch_defer, no EI — match. NOTE: guest also saves/restores
  $ra through fixed cell 0x801FEB90; clean omits it — unobservable
  (DI held throughout, cell is scratch).
- `FUN_8019E55C`: B0:0x57, p=*(obj+0x16c), 5-word copy to p+0x9c8 —
  match.
- `FUN_8019E5CC`: 28-word copy 0x8019E430..4A0 → 0xdf80 — match.
- `cd_seek_state_machine` @0x8019D5B0: `jr $t2(0xB0)`, `t1=0xB` delay —
  B0:0x0B trampoline; clean forwards live a0-a3 via
  bind_bios_call_live with caller ra — match.
- `FUN_8019B380`/`FUN_8019B390`/`cd_mode_apply*`/`bios_svc6` family:
  thin BIOS-vector trampolines per §4-safe pattern — match.
- `bind_jal`: ra/pc/sp save-restore around clean_try_dispatch +
  psx_dispatch_call — structurally correct.

## Coverage notes / not fully diffed

- `frame_tick` @0x8012AEEC, `scene_entry` @0x80159028,
  `match_loop`, `intro_movie_state`, `print_dispatch*` family,
  `sprite_node_emit_b` @0x8018C92C, `hud_gauge_render` @0x8012C374,
  `vram_tile_decode` @0x80190260, `sprite_9slice_build` @0x8018C1F4,
  `text_win_printf` family, gte_* wrappers (0x801952xx-0x801954xx),
  `gpu_mode_regs_apply` @0x80193424, `prim_depth_shade_emit`
  @0x8018A9E8 (structure mirrors verified siblings; OTZ shade-word
  math z<256 / 256-1280 / >=1280 with 0x03C00000 mask not
  insn-diffed), `FUN_801468ec` @0x801468EC (large hand port in
  bind.c — GTE CT loads, mode_map {0,4,2,6,1,7}, three bit-sections;
  spot-checked structure only).
- `prim_depth_shade_emit`'s `z - 256u < 1024u` ↔ guest boundary and
  the `mem | 0x03C00000` / `& ~0x03C00000` polarity are the highest-
  value remaining suspects in gfx.c.
- fnmap.c:2151 duplicate `ev_step` entry still latent (auto-generated;
  fix the generator, not the file).

## Runtime verification

No code changed → no rebuild/verify needed. Prior runtime state stands
(mode_monitor row1 → MENU, versus/arcade/attract healthy per
proof/post_d11b51d_mode_verify.md).
