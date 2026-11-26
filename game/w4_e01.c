/* w4_e01.c — clean CPS ports for guest piece-entries
 * 0x80184620-0x801846E0 (SLES_017.22_full_39.c, alias body of
 * scriptop_move 0x8018444C) and 0x80184758-0x801847F0 (alias body of
 * scriptop_add 0x80184710).
 *
 * All nine tasked addrs are CPS piece/jump-table-case entries inside two
 * large coroutine bodies, NOT standalone functions:
 *
 *   - scriptop_move (op05 Move, clean entry ported in game/vm.c): the
 *     src==15 "ext byte" field-source cases. Each piece loads its operands,
 *     stores the advanced stream pc (s1+4 -> ctx+0x24), computes sv into
 *     s0, then merges on the shared tails:
 *       block_80184680  obj+0x178 - own+0x94        (ext src 9/10)
 *       block_801846AC  100*(v1-a0)/maxHP(s4)       (ext src 8 foe dmg%,
 *                       11 self hp-bar% — numerator pair differs per entry)
 *       block_80184620  100*(maxHP-hp)/maxHP(s4)    (ext src 7 self dmg%)
 *       block_801846D8/DC/E0  s0 -> ctx+0x3C+dst*2, epilogue, jr $ra
 *   - scriptop_add (op07 Add, clean entry ported in game/vm.c): src-selector
 *     cases that all merge on block_801847EC (sw pc+2 -> ctx+0x24) /
 *     block_801847F0 (reg[dst] += sv, v0=1, jr $ra):
 *       0x80184758  sv = s16 ctx+0x3C+src*2   (src 1..5 register)
 *       0x80184778  sv = s16 ctrl+0x76        (src 8 distY)
 *       0x80184784  sv = u16 ctx+8            (src 9 aux8)
 *       0x80184790  sv = u16 a3+0x94 & 0x7fff (src 10 flag94)
 *
 * Each body below is a faithful op-by-op transcription of the generated
 * br2eu_psx_alias_body_* pieces: same guest ops, same order, same delay
 * slots, same yield/continuation points (psx_check_interrupts_at +
 * cpu->pc CPS dispatch), minus cycle/icache/cosim/debug instrumentation.
 * Internal continuation addrs covered by these bodies (0x80184680/AC/D8/
 * DC/E0 and 0x801847EC/F0) are also routed through clean_cps.map so a
 * yielded coroutine resumes inside clean C; continuations not covered here
 * stay generated-authoritative.
 */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef int8_t   byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef int      int32_t_ghidra;
#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern CPUState *clean_cpu_state(void);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st32(uint32_t a, uint32_t v);
extern uint8_t  br2_ld8(uint32_t a);
extern uint16_t br2_ld16(uint32_t a);
extern void     br2_st8(uint32_t a, uint8_t v);
extern void     br2_st16(uint32_t a, uint16_t v);
extern int      psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);
extern uint32_t br2_call_generated(uint32_t addr, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))

static void swl_guest(uint32_t addr, uint32_t v)
{
  uint32_t al = addr & ~3u;
  uint32_t w = DB32(al);
  switch (addr & 3u) {
  case 0: w = (w & 0xFFFFFF00u) | (v >> 24); break;
  case 1: w = (w & 0xFFFF0000u) | (v >> 16); break;
  case 2: w = (w & 0xFF000000u) | (v >> 8);  break;
  default: w = v; break;
  }
  DB32(al) = w;
}

static void prim_ot_link_indexed(uint32_t ot, uint32_t out, uint32_t otz)
{
  uint32_t slot = ot + (otz & 0x1ffcu);
  uint32_t old  = DB32(slot);
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
}

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability). */
#define zero cpu->gpr[0]
#define at   cpu->gpr[1]
#define v0   cpu->gpr[2]
#define v1   cpu->gpr[3]
#define a0   cpu->gpr[4]
#define a1   cpu->gpr[5]
#define a2   cpu->gpr[6]
#define a3   cpu->gpr[7]
#define t0   cpu->gpr[8]
#define t1   cpu->gpr[9]
#define t2   cpu->gpr[10]
#define t3   cpu->gpr[11]
#define t4   cpu->gpr[12]
#define t5   cpu->gpr[13]
#define t6   cpu->gpr[14]
#define t7   cpu->gpr[15]
#define s0   cpu->gpr[16]
#define s1   cpu->gpr[17]
#define s2   cpu->gpr[18]
#define s3   cpu->gpr[19]
#define s4   cpu->gpr[20]
#define s5   cpu->gpr[21]
#define s6   cpu->gpr[22]
#define s7   cpu->gpr[23]
#define t8   cpu->gpr[24]
#define t9   cpu->gpr[25]
#define k0   cpu->gpr[26]
#define k1   cpu->gpr[27]
#define gp   cpu->gpr[28]
#define sp   cpu->gpr[29]
#define fp   cpu->gpr[30]
#define ra   cpu->gpr[31]

/* ======== scriptop_move ext-byte (src==15) field cases ========
 * Guest pieces of br2eu_psx_alias_body_8018444C. s4 = self ctrl,
 * s2 = foe ctrl, s3 = ctx, s1 = stream pc, s6 = dst index.
 * Faithful to the generated blocks 0x80184620..0x801846E0. */
void scriptop_move_ext_cases(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

  /* CPS re-entry: the clean_cps wrapper hands cpu->pc == dispatched addr. */
  if (cpu->pc != 0u) {
    uint32_t _cont = cpu->pc; cpu->pc = 0;
    switch (_cont) {
      case 0x80184620u: goto block_80184620;
      case 0x80184658u: goto block_80184658;
      case 0x80184668u: goto block_80184668;
      case 0x80184678u: goto block_80184678;
      case 0x80184680u: goto block_80184680;
      case 0x80184694u: goto block_80184694;
      case 0x801846ACu: goto block_801846AC;
      case 0x801846D8u: goto block_801846D8;
      case 0x801846DCu: goto block_801846DC;
      case 0x801846E0u: goto block_801846E0;
      default: break;
    }
  }
  return;

block_80184620:
  /* ext src 7 — self damage percent: sv = 100*(maxHP-hp)/maxHP (s4). */
  a0 = (int32_t)(int16_t)cpu->read_half(s4 + 176);  /* 0x80184620: lh a0,0xb0(s4) */
  v1 = (int32_t)(int16_t)cpu->read_half(s4 + 172);  /* 0x80184624: lh v1,0xac(s4) */
  v0 = s1 + 4;                                      /* 0x80184628: addiu v0,s1,4 */
  cpu->write_word(s3 + 36, v0);                     /* 0x8018462C: sw v0,0x24(s3) */
  v1 = a0 - v1;                                     /* 0x80184630: subu v1,a0,v1 */
  v0 = v1 << 1;                                     /* 0x80184634: sll v0,v1,1 */
  v0 = v0 + v1;                                     /* 0x80184638: addu v0,v0,v1 */
  v0 = v0 << 3;                                     /* 0x8018463C: sll v0,v0,3 */
  v0 = v0 + v1;                                     /* 0x80184640: addu v0,v0,v1 */
  v0 = v0 << 2;                                     /* 0x80184644: sll v0,v0,2 */
  /* 0x80184648: div v0,a0 (signed; a0==0 -> lo = v0>=0 ? -1 : 1) */
  if (a0 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)a0); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)a0); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
  psx_muldiv_set(cpu, 37u);
  s0 = cpu->lo;                                     /* 0x8018464C: mflo s0 */
  psx_muldiv_stall(cpu);
  /* j 0x801846DC — delay slot */
  v0 = s6 << 1;                                     /* 0x80184654: sll v0,s6,1 */
  psx_check_interrupts_at(cpu, 0x801846DCu);
  goto block_801846DC;

block_80184658:
  /* ext src 8 — foe damage percent numerator (s2 maxHP/hp); shared
   * denominator maxHP(s4) is loaded in block_801846AC. */
  v1 = (int32_t)(int16_t)cpu->read_half(s2 + 176);  /* 0x80184658: lh v1,0xb0(s2) */
  a0 = (int32_t)(int16_t)cpu->read_half(s2 + 172);  /* 0x8018465C: lh a0,0xac(s2) */
  /* j 0x801846AC — delay slot */
  /* nop */  /* 0x80184664: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x801846ACu);
  goto block_801846AC;

block_80184668:
  /* ext src 9 — self obj+0x178 - own+0x94. */
  v0 = cpu->read_word(s4 + 32);                     /* 0x80184668: lw v0,0x20(s4) */
  a0 = (int32_t)(int16_t)cpu->read_half(s4 + 148);  /* 0x8018466C: lh a0,0x94(s4) */
  /* j 0x80184680 — delay slot */
  /* nop */  /* 0x80184674: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x80184680u);
  goto block_80184680;

block_80184678:
  /* ext src 10 — foe obj+0x178 - own+0x94. */
  v0 = cpu->read_word(s2 + 32);                     /* 0x80184678: lw v0,0x20(s2) */
  a0 = (int32_t)(int16_t)cpu->read_half(s2 + 148);  /* 0x8018467C: lh a0,0x94(s2) */
  /* fall through to block_80184680 */

block_80184680:
  v1 = cpu->read_word(v0 + 376);                    /* 0x80184680: lw v1,0x178(v0) */
  v0 = s1 + 4;                                      /* 0x80184684: addiu v0,s1,4 */
  cpu->write_word(s3 + 36, v0);                     /* 0x80184688: sw v0,0x24(s3) */
  /* j 0x801846D8 — delay slot */
  s0 = v1 - a0;                                     /* 0x80184690: subu s0,v1,a0 */
  psx_check_interrupts_at(cpu, 0x801846D8u);
  goto block_801846D8;

block_80184694:
  /* ext src 11 — self hp-bar percent numerator (hp - hpdisp, s4);
   * shared denominator maxHP(s4) loaded in block_801846AC. */
  v1 = (int32_t)(int16_t)cpu->read_half(s4 + 172);  /* 0x80184694: lh v1,0xac(s4) */
  a0 = (int32_t)(int16_t)cpu->read_half(s4 + 174);  /* 0x80184698: lh a0,0xae(s4) */
  /* j 0x801846AC — delay slot */
  /* nop */  /* 0x801846A0: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x801846ACu);
  goto block_801846AC;

block_801846AC:
  a1 = (int32_t)(int16_t)cpu->read_half(s4 + 176);  /* 0x801846AC: lh a1,0xb0(s4) */
  v0 = s1 + 4;                                      /* 0x801846B0: addiu v0,s1,4 */
  cpu->write_word(s3 + 36, v0);                     /* 0x801846B4: sw v0,0x24(s3) */
  v1 = v1 - a0;                                     /* 0x801846B8: subu v1,v1,a0 */
  v0 = v1 << 1;                                     /* 0x801846BC: sll v0,v1,1 */
  v0 = v0 + v1;                                     /* 0x801846C0: addu v0,v0,v1 */
  v0 = v0 << 3;                                     /* 0x801846C4: sll v0,v0,3 */
  v0 = v0 + v1;                                     /* 0x801846C8: addu v0,v0,v1 */
  v0 = v0 << 2;                                     /* 0x801846CC: sll v0,v0,2 */
  /* 0x801846D0: div v0,a1 (signed; a1==0 -> lo = v0>=0 ? -1 : 1) */
  if (a1 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)a1); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)a1); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
  psx_muldiv_set(cpu, 37u);
  s0 = cpu->lo;                                     /* 0x801846D4: mflo s0 */
  psx_muldiv_stall(cpu);
  /* fall through to block_801846D8 */

block_801846D8:
  v0 = s6 << 1;                                     /* 0x801846D8: sll v0,s6,1 */
  /* fall through to block_801846DC */

block_801846DC:
  v0 = s3 + v0;                                     /* 0x801846DC: addu v0,s3,v0 */
  /* fall through to block_801846E0 */

block_801846E0:
  /* shared epilogue: store sv, restore s0-s6/ra, return 1. */
  cpu->write_half(v0 + 60, (uint16_t)s0);           /* 0x801846E0: sh s0,0x3c(v0) */
  ra = cpu->read_word(sp + 44);                     /* 0x801846E4: lw ra,0x2c(sp) */
  s6 = cpu->read_word(sp + 40);                     /* 0x801846E8: lw s6,0x28(sp) */
  s5 = cpu->read_word(sp + 36);                     /* 0x801846EC: lw s5,0x24(sp) */
  s4 = cpu->read_word(sp + 32);                     /* 0x801846F0: lw s4,0x20(sp) */
  s3 = cpu->read_word(sp + 28);                     /* 0x801846F4: lw s3,0x1c(sp) */
  s2 = cpu->read_word(sp + 24);                     /* 0x801846F8: lw s2,0x18(sp) */
  s1 = cpu->read_word(sp + 20);                     /* 0x801846FC: lw s1,0x14(sp) */
  s0 = cpu->read_word(sp + 16);                     /* 0x80184700: lw s0,0x10(sp) */
  v0 = 1;                                           /* 0x80184704: addiu v0,zero,1 */
  uint32_t _jt_80184708 = ra;  /* latch indirect target before delay slot */
  /* delay slot (always executes) */
  sp = sp + 48;                                     /* 0x8018470C: addiu sp,sp,0x30 */
  psx_check_interrupts_at(cpu, _jt_80184708);
  cpu->pc = _jt_80184708; return;  /* CPS: jr $ra */
}

/* ======== scriptop_add src-selector cases ========
 * Guest pieces of br2eu_psx_alias_body_80184710. a2 = ctx, a3 = aux obj,
 * t1/t2 = ctrl regs. All merge on block_801847EC (store pc+2 -> ctx+0x24)
 * / block_801847F0 (reg[dst] += sv, return 1).
 * Faithful to the generated blocks 0x80184758..0x8018480C. */
void scriptop_add_src_cases(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

  /* CPS re-entry: the clean_cps wrapper hands cpu->pc == dispatched addr. */
  if (cpu->pc != 0u) {
    uint32_t _cont = cpu->pc; cpu->pc = 0;
    switch (_cont) {
      case 0x80184758u: goto block_80184758;
      case 0x80184778u: goto block_80184778;
      case 0x80184784u: goto block_80184784;
      case 0x80184790u: goto block_80184790;
      case 0x801847ECu: goto block_801847EC;
      case 0x801847F0u: goto block_801847F0;
      default: break;
    }
  }
  return;

block_80184758:
  /* src 1..5 — sv = s16 ctx+0x3C+src*2 (VREG). */
  v0 = a0 << 1;                                     /* 0x80184758: sll v0,a0,1 */
  v0 = a2 + v0;                                     /* 0x8018475C: addu v0,a2,v0 */
  a0 = (int32_t)(int16_t)cpu->read_half(v0 + 60);   /* 0x80184760: lh a0,0x3c(v0) */
  /* j 0x801847EC — delay slot */
  v0 = a1 + 2;                                      /* 0x80184768: addiu v0,a1,2 */
  psx_check_interrupts_at(cpu, 0x801847ECu);
  goto block_801847EC;

block_80184778:
  /* src 8 — sv = s16 ctrl+0x76 (distY). */
  a0 = (int32_t)(int16_t)cpu->read_half(t1 + 118);  /* 0x80184778: lh a0,0x76(t1) */
  /* j 0x801847EC — delay slot */
  v0 = a1 + 2;                                      /* 0x80184780: addiu v0,a1,2 */
  psx_check_interrupts_at(cpu, 0x801847ECu);
  goto block_801847EC;

block_80184784:
  /* src 9 — sv = u16 ctx+8 (VX_AUX8). */
  a0 = cpu->read_half(a2 + 8);                      /* 0x80184784: lhu a0,8(a2) */
  /* j 0x801847EC — delay slot */
  v0 = a1 + 2;                                      /* 0x8018478C: addiu v0,a1,2 */
  psx_check_interrupts_at(cpu, 0x801847ECu);
  goto block_801847EC;

block_80184790:
  /* src 10 — sv = u16 a3+0x94 & 0x7fff. */
  v1 = cpu->read_half(a3 + 148);                    /* 0x80184790: lhu v1,0x94(a3) */
  v0 = a1 + 2;                                      /* 0x80184794: addiu v0,a1,2 */
  cpu->write_word(a2 + 36, v0);                     /* 0x80184798: sw v0,0x24(a2) */
  /* j 0x801847F0 — delay slot */
  a0 = v1 & 0x7FFF;                                 /* 0x801847A0: andi a0,v1,0x7fff */
  psx_check_interrupts_at(cpu, 0x801847F0u);
  goto block_801847F0;

block_801847EC:
  cpu->write_word(a2 + 36, v0);                     /* 0x801847EC: sw v0,0x24(a2) */
  /* fall through to block_801847F0 */

block_801847F0:
  /* shared tail: reg[dst] = (s16)(reg[dst] + sv); return 1. */
  v1 = t2 << 1;                                     /* 0x801847F0: sll v1,t2,1 */
  v1 = a2 + v1;                                     /* 0x801847F4: addu v1,a2,v1 */
  v0 = (int32_t)(int16_t)cpu->read_half(v1 + 60);   /* 0x801847F8: lh v0,0x3c(v1) */
  /* nop */  /* 0x801847FC: 0x00000000 */
  v0 = v0 + a0;                                     /* 0x80184800: addu v0,v0,a0 */
  cpu->write_half(v1 + 60, (uint16_t)v0);           /* 0x80184804: sh v0,0x3c(v1) */
  uint32_t _jt_80184808 = ra;  /* latch indirect target before delay slot */
  /* delay slot (always executes) */
  v0 = 1;                                           /* 0x8018480C: addiu v0,zero,1 */
  psx_check_interrupts_at(cpu, _jt_80184808);
  cpu->pc = _jt_80184808; return;  /* CPS: jr $ra */
}
