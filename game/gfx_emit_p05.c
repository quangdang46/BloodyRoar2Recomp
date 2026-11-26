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

/* printf_cps_body — faithful clean CPS transliteration of the generated
 * br2eu_func_8018E590 body (guest 0x8018E590-0x8018EC24, "printf_core"),
 * produced by game/gen_cps_clean.py and hand-verified block-for-block.
 * The nine guest addresses below are NOT standalone functions: they are
 * alias piece-entries the recompiler exposes inside this one CPS body
 * (br2eu_psx_alias_body_8018E590 switch(entry)). Each one enters the
 * shared body mid-stream — after printf_core's prologue has already built
 * the 0x78-byte guest frame — so they can only ever be reached as CPS
 * continuation/branch-target PCs, never by a fresh jal. clean_cps.map
 * registers them with the CLEAN_ENT_CPS flag so clean_try_dispatch sets
 * cpu->pc = <addr> and the _cont switch below picks the right block,
 * exactly like the generated alias body.
 *
 * block_8018E670 exists only in the alias body (the canonical function
 * keeps 0x8018E670 mid-block inside block_8018E668): entering there runs
 * just the j-delay-slot `ori s3,s3,0x10` then rejoins block_8018E60C.
 * The label + _cont case below reproduce that split faithfully. */
static void printf_cps_body(CPUState *cpu)
{
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018E89Cu: goto block_8018E89C;
            case 0x8018E8C0u: goto block_8018E8C0;
            case 0x8018EA5Cu: goto block_8018EA5C;
            case 0x8018EA80u: goto block_8018EA80;
            case 0x8018EA94u: goto block_8018EA94;
            case 0x8018EAA0u: goto block_8018EAA0;
            case 0x8018EACCu: goto block_8018EACC;
            case 0x8018EAFCu: goto block_8018EAFC;
            case 0x8018EB2Cu: goto block_8018EB2C;
            case 0x8018EB44u: goto block_8018EB44;
            case 0x8018EB80u: goto block_8018EB80;
            case 0x8018EBC8u: goto block_8018EBC8;
            case 0x8018EBECu: goto block_8018EBEC;
            case 0x8018E5C4u: goto block_8018E5C4;
            case 0x8018E5CCu: goto block_8018E5CC;
            case 0x8018E5E0u: goto block_8018E5E0;
            case 0x8018E5F0u: goto block_8018E5F0;
            case 0x8018E5F8u: goto block_8018E5F8;
            case 0x8018E60Cu: goto block_8018E60C;
            case 0x8018E624u: goto block_8018E624;
            case 0x8018E63Cu: goto block_8018E63C;
            case 0x8018E644u: goto block_8018E644;
            case 0x8018E64Cu: goto block_8018E64C;
            case 0x8018E654u: goto block_8018E654;
            case 0x8018E668u: goto block_8018E668;
            case 0x8018E670u: goto block_8018E670;
            case 0x8018E678u: goto block_8018E678;
            case 0x8018E680u: goto block_8018E680;
            case 0x8018E694u: goto block_8018E694;
            case 0x8018E6A4u: goto block_8018E6A4;
            case 0x8018E6ACu: goto block_8018E6AC;
            case 0x8018E6CCu: goto block_8018E6CC;
            case 0x8018E6F0u: goto block_8018E6F0;
            case 0x8018E6F8u: goto block_8018E6F8;
            case 0x8018E700u: goto block_8018E700;
            case 0x8018E708u: goto block_8018E708;
            case 0x8018E710u: goto block_8018E710;
            case 0x8018E714u: goto block_8018E714;
            case 0x8018E740u: goto block_8018E740;
            case 0x8018E760u: goto block_8018E760;
            case 0x8018E76Cu: goto block_8018E76C;
            case 0x8018E774u: goto block_8018E774;
            case 0x8018E77Cu: goto block_8018E77C;
            case 0x8018E784u: goto block_8018E784;
            case 0x8018E7A0u: goto block_8018E7A0;
            case 0x8018E7B0u: goto block_8018E7B0;
            case 0x8018E7B8u: goto block_8018E7B8;
            case 0x8018E7C8u: goto block_8018E7C8;
            case 0x8018E7D4u: goto block_8018E7D4;
            case 0x8018E7DCu: goto block_8018E7DC;
            case 0x8018E7E8u: goto block_8018E7E8;
            case 0x8018E7F4u: goto block_8018E7F4;
            case 0x8018E7FCu: goto block_8018E7FC;
            case 0x8018E810u: goto block_8018E810;
            case 0x8018E824u: goto block_8018E824;
            case 0x8018E834u: goto block_8018E834;
            case 0x8018E83Cu: goto block_8018E83C;
            case 0x8018E84Cu: goto block_8018E84C;
            case 0x8018E85Cu: goto block_8018E85C;
            case 0x8018E86Cu: goto block_8018E86C;
            case 0x8018E880u: goto block_8018E880;
            case 0x8018E888u: goto block_8018E888;
            case 0x8018E890u: goto block_8018E890;
            case 0x8018E8A4u: goto block_8018E8A4;
            case 0x8018E8B0u: goto block_8018E8B0;
            case 0x8018E8B8u: goto block_8018E8B8;
            case 0x8018E8C4u: goto block_8018E8C4;
            case 0x8018E8CCu: goto block_8018E8CC;
            case 0x8018E8DCu: goto block_8018E8DC;
            case 0x8018E8E4u: goto block_8018E8E4;
            case 0x8018E8F4u: goto block_8018E8F4;
            case 0x8018E904u: goto block_8018E904;
            case 0x8018E91Cu: goto block_8018E91C;
            case 0x8018E924u: goto block_8018E924;
            case 0x8018E934u: goto block_8018E934;
            case 0x8018E944u: goto block_8018E944;
            case 0x8018E94Cu: goto block_8018E94C;
            case 0x8018E954u: goto block_8018E954;
            case 0x8018E958u: goto block_8018E958;
            case 0x8018E95Cu: goto block_8018E95C;
            case 0x8018E964u: goto block_8018E964;
            case 0x8018E96Cu: goto block_8018E96C;
            case 0x8018E974u: goto block_8018E974;
            case 0x8018E984u: goto block_8018E984;
            case 0x8018E990u: goto block_8018E990;
            case 0x8018E994u: goto block_8018E994;
            case 0x8018E9B8u: goto block_8018E9B8;
            case 0x8018E9CCu: goto block_8018E9CC;
            case 0x8018E9D8u: goto block_8018E9D8;
            case 0x8018E9ECu: goto block_8018E9EC;
            case 0x8018E9F4u: goto block_8018E9F4;
            case 0x8018E9F8u: goto block_8018E9F8;
            case 0x8018E9FCu: goto block_8018E9FC;
            case 0x8018EA08u: goto block_8018EA08;
            case 0x8018EA0Cu: goto block_8018EA0C;
            case 0x8018EA18u: goto block_8018EA18;
            case 0x8018EA1Cu: goto block_8018EA1C;
            case 0x8018EA30u: goto block_8018EA30;
            case 0x8018EA34u: goto block_8018EA34;
            case 0x8018EA3Cu: goto block_8018EA3C;
            case 0x8018EA4Cu: goto block_8018EA4C;
            case 0x8018EA54u: goto block_8018EA54;
            case 0x8018EA70u: goto block_8018EA70;
            case 0x8018EA78u: goto block_8018EA78;
            case 0x8018EA84u: goto block_8018EA84;
            case 0x8018EA8Cu: goto block_8018EA8C;
            case 0x8018EAA4u: goto block_8018EAA4;
            case 0x8018EAB0u: goto block_8018EAB0;
            case 0x8018EAC4u: goto block_8018EAC4;
            case 0x8018EAE0u: goto block_8018EAE0;
            case 0x8018EAF4u: goto block_8018EAF4;
            case 0x8018EB10u: goto block_8018EB10;
            case 0x8018EB1Cu: goto block_8018EB1C;
            case 0x8018EB34u: goto block_8018EB34;
            case 0x8018EB3Cu: goto block_8018EB3C;
            case 0x8018EB58u: goto block_8018EB58;
            case 0x8018EB64u: goto block_8018EB64;
            case 0x8018EB78u: goto block_8018EB78;
            case 0x8018EB94u: goto block_8018EB94;
            case 0x8018EBA8u: goto block_8018EBA8;
            case 0x8018EBACu: goto block_8018EBAC;
            case 0x8018EBC0u: goto block_8018EBC0;
            case 0x8018EBD4u: goto block_8018EBD4;
            case 0x8018EBE4u: goto block_8018EBE4;
            case 0x8018EBF4u: goto block_8018EBF4;
            default: break;
        }
    }
    /* Address: 0x8018E590, Size: 1684 bytes, Blocks: 125 */

block_8018E590:
    sp = sp + -120;
    cpu->write_word(sp + 84, s1);
    s1 = a2;
    cpu->write_word(sp + 116, ra);
    cpu->write_word(sp + 112, fp);
    cpu->write_word(sp + 108, s7);
    cpu->write_word(sp + 104, s6);
    cpu->write_word(sp + 100, s5);
    cpu->write_word(sp + 96, s4);
    cpu->write_word(sp + 92, s3);
    cpu->write_word(sp + 88, s2);
    int _bc_8018E5BC = (a1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 80, s0);
    if (_bc_8018E5BC) {
        psx_check_interrupts_at(cpu, 0x8018E5CCu);
        goto block_8018E5CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E5C4u);
        goto block_8018E5C4;  /* not taken */
    }

block_8018E5C4:
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, 0x8018EBF4u);
    goto block_8018EBF4;  /* j */

block_8018E5CC:
    s7 = a1;
    a3 = 0x8012 << 16;  /* 0x80120000 */
    a3 = a3 + 20456;
    cpu->write_word(sp + 72, a3);
    cpu->write_word(sp + 56, zero);
    /* fall through to block_8018E5E0 */

block_8018E5E0:
    a0 = cpu->read_byte(s7);
    /* nop */  /* 0x8018E5E4: 0x00000000 */
    int _bc_8018E5E8 = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 37;
    if (_bc_8018E5E8) {
        psx_check_interrupts_at(cpu, 0x8018EBC0u);
        goto block_8018EBC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E5F0u);
        goto block_8018E5F0;  /* not taken */
    }

block_8018E5F0:
    int _bc_8018E5F0 = (a0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s3 = zero;
    if (_bc_8018E5F0) {
        psx_check_interrupts_at(cpu, 0x8018EBE4u);
        goto block_8018EBE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E5F8u);
        goto block_8018E5F8;  /* not taken */
    }

block_8018E5F8:
    s4 = -1;
    fp = zero;
    cpu->write_word(sp + 60, zero);
    cpu->write_word(sp + 64, zero);
    cpu->write_word(sp + 68, zero);
    /* fall through to block_8018E60C */

block_8018E60C:
    s7 = s7 + 1;
    v1 = cpu->read_byte(s7);
    /* nop */  /* 0x8018E614: 0x00000000 */
    v0 = (v1 < (uint32_t)121) ? 1 : 0;  /* 0x8018E618: 0x2C620079 */
    int _bc_8018E61C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (_bc_8018E61C) {
        psx_check_interrupts_at(cpu, 0x8018EBD4u);
        goto block_8018EBD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E624u);
        goto block_8018E624;  /* not taken */
    }

block_8018E624:
    at = 0x8012 << 16;  /* 0x80120000 */
    at = at + v0;
    v0 = cpu->read_word(at + 20504);
    /* nop */  /* 0x8018E630: 0x00000000 */
    uint32_t _jt_8018E634 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E638: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8018E634);
    cpu->pc = _jt_8018E634; return;  /* CPS: jr v0 */

block_8018E63C:
    int _bc_8018E63C = (fp != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E640: 0x00000000 */
    if (_bc_8018E63C) {
        psx_check_interrupts_at(cpu, 0x8018E60Cu);
        goto block_8018E60C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E644u);
        goto block_8018E644;  /* not taken */
    }

block_8018E644:
    /* delay slot (always executes) */
    fp = 32;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E64C:
    /* delay slot (always executes) */
    s3 = s3 | 0x0008;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E654:
    s1 = s1 + 4;
    a3 = cpu->read_word(s1 + -4);
    /* nop */  /* 0x8018E65C: 0x00000000 */
    int _bc_8018E660 = ((int32_t)a3 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 68, a3);
    if (_bc_8018E660) {
        psx_check_interrupts_at(cpu, 0x8018E60Cu);
        goto block_8018E60C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E668u);
        goto block_8018E668;  /* not taken */
    }

block_8018E668:
    a3 = zero - a3;
    cpu->write_word(sp + 68, a3);
block_8018E670:
    /* delay slot (always executes) */
    s3 = s3 | 0x0010;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E678:
    /* delay slot (always executes) */
    fp = 43;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E680:
    s7 = s7 + 1;
    v1 = cpu->read_byte(s7);
    v0 = 42;
    int _bc_8018E68C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (v1 < (uint32_t)128) ? 1 : 0;  /* 0x8018E690: 0x2C620080 */
    if (_bc_8018E68C) {
        psx_check_interrupts_at(cpu, 0x8018E6A4u);
        goto block_8018E6A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E694u);
        goto block_8018E694;  /* not taken */
    }

block_8018E694:
    s1 = s1 + 4;
    s0 = cpu->read_word(s1 + -4);
    /* delay slot (always executes) */
    s4 = s0;
    psx_check_interrupts_at(cpu, 0x8018E6F8u);
    goto block_8018E6F8;  /* j */

block_8018E6A4:
    int _bc_8018E6A4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = zero;
    if (_bc_8018E6A4) {
        psx_check_interrupts_at(cpu, 0x8018E6F0u);
        goto block_8018E6F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E6ACu);
        goto block_8018E6AC;  /* not taken */
    }

block_8018E6AC:
    a0 = cpu->read_byte(s7);
    v0 = 0x801B << 16;  /* 0x801B0000 */
    v0 = v0 + a0;
    v0 = cpu->read_byte(v0 + 26369);
    /* nop */  /* 0x8018E6BC: 0x00000000 */
    v0 = v0 & 0x0004;  /* 0x8018E6C0: 0x30420004 */
    int _bc_8018E6C4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 << 2;
    if (_bc_8018E6C4) {
        psx_check_interrupts_at(cpu, 0x8018E6F0u);
        goto block_8018E6F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E6CCu);
        goto block_8018E6CC;  /* not taken */
    }

block_8018E6CC:
    s7 = s7 + 1;
    v0 = v0 + s0;
    v0 = v0 << 1;
    v0 = v0 + -48;
    v1 = cpu->read_byte(s7);
    /* nop */  /* 0x8018E6E0: 0x00000000 */
    v1 = (v1 < (uint32_t)128) ? 1 : 0;  /* 0x8018E6E4: 0x2C630080 */
    int _bc_8018E6E8 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v0 + a0;
    if (_bc_8018E6E8) {
        psx_check_interrupts_at(cpu, 0x8018E6ACu);
        goto block_8018E6AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E6F0u);
        goto block_8018E6F0;  /* not taken */
    }

block_8018E6F0:
    s7 = s7 + -1;
    s4 = s0;
    /* fall through to block_8018E6F8 */

block_8018E6F8:
    int _bc_8018E6F8 = ((int32_t)s0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E6FC: 0x00000000 */
    if (_bc_8018E6F8) {
        psx_check_interrupts_at(cpu, 0x8018E60Cu);
        goto block_8018E60C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E700u);
        goto block_8018E700;  /* not taken */
    }

block_8018E700:
    /* delay slot (always executes) */
    s4 = -1;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E708:
    /* delay slot (always executes) */
    s3 = s3 | 0x0020;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E710:
    s0 = zero;
    /* fall through to block_8018E714 */

block_8018E714:
    v0 = cpu->read_byte(s7);
    s7 = s7 + 1;
    v1 = s0 << 2;
    v1 = v1 + s0;
    v1 = v1 << 1;
    v1 = v1 + -48;
    a0 = cpu->read_byte(s7);
    /* nop */  /* 0x8018E730: 0x00000000 */
    a0 = (a0 < (uint32_t)128) ? 1 : 0;  /* 0x8018E734: 0x2C840080 */
    int _bc_8018E738 = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v1 + v0;
    if (_bc_8018E738) {
        psx_check_interrupts_at(cpu, 0x8018E760u);
        goto block_8018E760;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E740u);
        goto block_8018E740;  /* not taken */
    }

block_8018E740:
    v0 = cpu->read_byte(s7);
    at = 0x801B << 16;  /* 0x801B0000 */
    at = at + v0;
    v0 = cpu->read_byte(at + 26369);
    /* nop */  /* 0x8018E750: 0x00000000 */
    v0 = v0 & 0x0004;  /* 0x8018E754: 0x30420004 */
    int _bc_8018E758 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E75C: 0x00000000 */
    if (_bc_8018E758) {
        psx_check_interrupts_at(cpu, 0x8018E714u);
        goto block_8018E714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E760u);
        goto block_8018E760;  /* not taken */
    }

block_8018E760:
    cpu->write_word(sp + 68, s0);
    /* delay slot (always executes) */
    s7 = s7 + -1;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E76C:
    /* delay slot (always executes) */
    s3 = s3 | 0x0002;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E774:
    /* delay slot (always executes) */
    s3 = s3 | 0x0004;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E77C:
    /* delay slot (always executes) */
    s3 = s3 | 0x0001;
    psx_check_interrupts_at(cpu, 0x8018E60Cu);
    goto block_8018E60C;  /* j */

block_8018E784:
    s2 = sp + 16;
    s1 = s1 + 4;
    s6 = 1;
    v0 = cpu->read_byte(s1 + -4);
    fp = zero;
    /* delay slot (always executes) */
    cpu->write_byte(sp + 16, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x8018E9FCu);
    goto block_8018E9FC;  /* j */

block_8018E7A0:
    s3 = s3 | 0x0001;
    v0 = s3 & 0x0001;  /* 0x8018E7A4: 0x32620001 */
    int _bc_8018E7A8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 & 0x0004;  /* 0x8018E7AC: 0x32620004 */
    if (_bc_8018E7A8) {
        psx_check_interrupts_at(cpu, 0x8018E7C8u);
        goto block_8018E7C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E7B0u);
        goto block_8018E7B0;  /* not taken */
    }

block_8018E7B0:
    int _bc_8018E7B0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E7B4: 0x00000000 */
    if (_bc_8018E7B0) {
        psx_check_interrupts_at(cpu, 0x8018E7C8u);
        goto block_8018E7C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E7B8u);
        goto block_8018E7B8;  /* not taken */
    }

block_8018E7B8:
    s1 = s1 + 4;
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + -4);
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E7C4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E7D4u);
    goto block_8018E7D4;  /* j */

block_8018E7C8:
    s1 = s1 + 4;
    v1 = cpu->read_word(s1 + -4);
    /* nop */  /* 0x8018E7D0: 0x00000000 */
    /* fall through to block_8018E7D4 */

block_8018E7D4:
    int _bc_8018E7D4 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 10;
    if (_bc_8018E7D4) {
        psx_check_interrupts_at(cpu, 0x8018E95Cu);
        goto block_8018E95C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E7DCu);
        goto block_8018E7DC;  /* not taken */
    }

block_8018E7DC:
    v1 = zero - v1;
    /* delay slot (always executes) */
    fp = 45;
    psx_check_interrupts_at(cpu, 0x8018E95Cu);
    goto block_8018E95C;  /* j */

block_8018E7E8:
    v0 = s3 & 0x0001;  /* 0x8018E7E8: 0x32620001 */
    int _bc_8018E7EC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 & 0x0004;  /* 0x8018E7F0: 0x32620004 */
    if (_bc_8018E7EC) {
        psx_check_interrupts_at(cpu, 0x8018E810u);
        goto block_8018E810;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E7F4u);
        goto block_8018E7F4;  /* not taken */
    }

block_8018E7F4:
    int _bc_8018E7F4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E7F8: 0x00000000 */
    if (_bc_8018E7F4) {
        psx_check_interrupts_at(cpu, 0x8018E810u);
        goto block_8018E810;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E7FCu);
        goto block_8018E7FC;  /* not taken */
    }

block_8018E7FC:
    s1 = s1 + 4;
    v0 = cpu->read_word(s1 + -4);
    a3 = cpu->read_half(sp + 56);
    /* delay slot (always executes) */
    cpu->write_half(v0, (uint16_t)a3);
    psx_check_interrupts_at(cpu, 0x8018EBECu);
    goto block_8018EBEC;  /* j */

block_8018E810:
    s1 = s1 + 4;
    v0 = cpu->read_word(s1 + -4);
    a3 = cpu->read_word(sp + 56);
    /* delay slot (always executes) */
    cpu->write_word(v0, a3);
    psx_check_interrupts_at(cpu, 0x8018EBECu);
    goto block_8018EBEC;  /* j */

block_8018E824:
    s3 = s3 | 0x0001;
    v0 = s3 & 0x0001;  /* 0x8018E828: 0x32620001 */
    int _bc_8018E82C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 & 0x0004;  /* 0x8018E830: 0x32620004 */
    if (_bc_8018E82C) {
        psx_check_interrupts_at(cpu, 0x8018E84Cu);
        goto block_8018E84C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E834u);
        goto block_8018E834;  /* not taken */
    }

block_8018E834:
    int _bc_8018E834 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 8;
    if (_bc_8018E834) {
        psx_check_interrupts_at(cpu, 0x8018E84Cu);
        goto block_8018E84C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E83Cu);
        goto block_8018E83C;  /* not taken */
    }

block_8018E83C:
    s1 = s1 + 4;
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + -4);
    /* delay slot (always executes) */
    fp = zero;
    psx_check_interrupts_at(cpu, 0x8018E95Cu);
    goto block_8018E95C;  /* j */

block_8018E84C:
    s1 = s1 + 4;
    v1 = cpu->read_word(s1 + -4);
    /* delay slot (always executes) */
    a1 = 8;
    psx_check_interrupts_at(cpu, 0x8018E958u);
    goto block_8018E958;  /* j */

block_8018E85C:
    s1 = s1 + 4;
    v1 = cpu->read_word(s1 + -4);
    /* delay slot (always executes) */
    a1 = 16;
    psx_check_interrupts_at(cpu, 0x8018E958u);
    goto block_8018E958;  /* j */

block_8018E86C:
    s1 = s1 + 4;
    s2 = cpu->read_word(s1 + -4);
    /* nop */  /* 0x8018E874: 0x00000000 */
    int _bc_8018E878 = (s2 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E87C: 0x00000000 */
    if (_bc_8018E878) {
        psx_check_interrupts_at(cpu, 0x8018E888u);
        goto block_8018E888;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E880u);
        goto block_8018E880;  /* not taken */
    }

block_8018E880:
    s2 = 0x8012 << 16;  /* 0x80120000 */
    s2 = s2 + 20476;
    /* fall through to block_8018E888 */

block_8018E888:
    int _bc_8018E888 = ((int32_t)s4 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    if (_bc_8018E888) {
        psx_check_interrupts_at(cpu, 0x8018E8B8u);
        goto block_8018E8B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E890u);
        goto block_8018E890;  /* not taken */
    }

block_8018E890:
    a1 = zero;
    ra = 0x8018E89Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x8018ECA0u);
    cpu->pc = 0x8018ECA0u; return;  /* CPS jal -> 0x8018ECA0 */

block_8018E89C:
    int _bc_8018E89C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s6 = v0 - s2;
    if (_bc_8018E89C) {
        psx_check_interrupts_at(cpu, 0x8018E8B0u);
        goto block_8018E8B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E8A4u);
        goto block_8018E8A4;  /* not taken */
    }

block_8018E8A4:
    v0 = ((int32_t)s4 < (int32_t)s6) ? 1 : 0;  /* 0x8018E8A4: 0x0296102A */
    int _bc_8018E8A8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    fp = zero;
    if (_bc_8018E8A8) {
        psx_check_interrupts_at(cpu, 0x8018E9FCu);
        goto block_8018E9FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E8B0u);
        goto block_8018E8B0;  /* not taken */
    }

block_8018E8B0:
    /* delay slot (always executes) */
    s6 = s4;
    psx_check_interrupts_at(cpu, 0x8018E8C4u);
    goto block_8018E8C4;  /* j */

block_8018E8B8:
    ra = 0x8018E8C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    psx_check_interrupts_at(cpu, 0x8018E510u);
    cpu->pc = 0x8018E510u; return;  /* CPS jal -> 0x8018E510 */

block_8018E8C0:
    s6 = v0;
    /* fall through to block_8018E8C4 */

block_8018E8C4:
    /* delay slot (always executes) */
    fp = zero;
    psx_check_interrupts_at(cpu, 0x8018E9FCu);
    goto block_8018E9FC;  /* j */

block_8018E8CC:
    s3 = s3 | 0x0001;
    v0 = s3 & 0x0001;  /* 0x8018E8D0: 0x32620001 */
    int _bc_8018E8D4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 & 0x0004;  /* 0x8018E8D8: 0x32620004 */
    if (_bc_8018E8D4) {
        psx_check_interrupts_at(cpu, 0x8018E8F4u);
        goto block_8018E8F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E8DCu);
        goto block_8018E8DC;  /* not taken */
    }

block_8018E8DC:
    int _bc_8018E8DC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 10;
    if (_bc_8018E8DC) {
        psx_check_interrupts_at(cpu, 0x8018E8F4u);
        goto block_8018E8F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E8E4u);
        goto block_8018E8E4;  /* not taken */
    }

block_8018E8E4:
    s1 = s1 + 4;
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + -4);
    /* delay slot (always executes) */
    fp = zero;
    psx_check_interrupts_at(cpu, 0x8018E95Cu);
    goto block_8018E95C;  /* j */

block_8018E8F4:
    s1 = s1 + 4;
    v1 = cpu->read_word(s1 + -4);
    /* delay slot (always executes) */
    a1 = 10;
    psx_check_interrupts_at(cpu, 0x8018E958u);
    goto block_8018E958;  /* j */

block_8018E904:
    a3 = 0x8012 << 16;  /* 0x80120000 */
    a3 = a3 + 20484;
    cpu->write_word(sp + 72, a3);
    v0 = s3 & 0x0001;  /* 0x8018E910: 0x32620001 */
    int _bc_8018E914 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 & 0x0004;  /* 0x8018E918: 0x32620004 */
    if (_bc_8018E914) {
        psx_check_interrupts_at(cpu, 0x8018E934u);
        goto block_8018E934;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E91Cu);
        goto block_8018E91C;  /* not taken */
    }

block_8018E91C:
    int _bc_8018E91C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 16;
    if (_bc_8018E91C) {
        psx_check_interrupts_at(cpu, 0x8018E934u);
        goto block_8018E934;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E924u);
        goto block_8018E924;  /* not taken */
    }

block_8018E924:
    s1 = s1 + 4;
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + -4);
    /* delay slot (always executes) */
    v0 = s3 & 0x0008;  /* 0x8018E930: 0x32620008 */
    psx_check_interrupts_at(cpu, 0x8018E944u);
    goto block_8018E944;  /* j */

block_8018E934:
    s1 = s1 + 4;
    v1 = cpu->read_word(s1 + -4);
    a1 = 16;
    v0 = s3 & 0x0008;  /* 0x8018E940: 0x32620008 */
    /* fall through to block_8018E944 */

block_8018E944:
    int _bc_8018E944 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    fp = zero;
    if (_bc_8018E944) {
        psx_check_interrupts_at(cpu, 0x8018E95Cu);
        goto block_8018E95C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E94Cu);
        goto block_8018E94C;  /* not taken */
    }

block_8018E94C:
    int _bc_8018E94C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E950: 0x00000000 */
    if (_bc_8018E94C) {
        psx_check_interrupts_at(cpu, 0x8018E95Cu);
        goto block_8018E95C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E954u);
        goto block_8018E954;  /* not taken */
    }

block_8018E954:
    s3 = s3 | 0x0040;
    /* fall through to block_8018E958 */

block_8018E958:
    fp = zero;
    /* fall through to block_8018E95C */

block_8018E95C:
    int _bc_8018E95C = ((int32_t)s4 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 60, s4);
    if (_bc_8018E95C) {
        psx_check_interrupts_at(cpu, 0x8018E96Cu);
        goto block_8018E96C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E964u);
        goto block_8018E964;  /* not taken */
    }

block_8018E964:
    v0 = -33;
    s3 = s3 & v0;  /* 0x8018E968: 0x02629824 */
    /* fall through to block_8018E96C */

block_8018E96C:
    int _bc_8018E96C = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 = sp + 56;
    if (_bc_8018E96C) {
        psx_check_interrupts_at(cpu, 0x8018E984u);
        goto block_8018E984;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E974u);
        goto block_8018E974;  /* not taken */
    }

block_8018E974:
    a3 = cpu->read_word(sp + 60);
    /* nop */  /* 0x8018E978: 0x00000000 */
    int _bc_8018E97C = (a3 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = sp - s2;
    if (_bc_8018E97C) {
        psx_check_interrupts_at(cpu, 0x8018E9F8u);
        goto block_8018E9F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E984u);
        goto block_8018E984;  /* not taken */
    }

block_8018E984:
    if (a1 != 0) { cpu->lo = v1 / a1; cpu->hi = v1 % a1; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v1; }
    psx_muldiv_set(cpu, 37u);
    int _bc_8018E988 = (a1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E98C: 0x00000000 */
    if (_bc_8018E988) {
        psx_check_interrupts_at(cpu, 0x8018E994u);
        goto block_8018E994;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E990u);
        goto block_8018E990;  /* not taken */
    }

block_8018E990:
    /* break(7168) — trap, no-op in recompiler */  /* 0x8018E990: 0x0007000D */
    /* fall through to block_8018E994 */

block_8018E994:
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = cpu->hi;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 72);
    /* nop */  /* 0x8018E9A0: 0x00000000 */
    v0 = a3 + v0;
    a0 = cpu->read_byte(v0);
    s2 = s2 + -1;
    int _bc_8018E9B0 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s2, (uint8_t)a0);
    if (_bc_8018E9B0) {
        psx_check_interrupts_at(cpu, 0x8018E984u);
        goto block_8018E984;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E9B8u);
        goto block_8018E9B8;  /* not taken */
    }

block_8018E9B8:
    a3 = 0x8012 << 16;  /* 0x80120000 */
    a3 = a3 + 20456;
    v0 = s3 & 0x0008;  /* 0x8018E9C0: 0x32620008 */
    int _bc_8018E9C4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 72, a3);
    if (_bc_8018E9C4) {
        psx_check_interrupts_at(cpu, 0x8018E9F4u);
        goto block_8018E9F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E9CCu);
        goto block_8018E9CC;  /* not taken */
    }

block_8018E9CC:
    v0 = 8;
    int _bc_8018E9D0 = (a1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = sp - s2;
    if (_bc_8018E9D0) {
        psx_check_interrupts_at(cpu, 0x8018E9F8u);
        goto block_8018E9F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E9D8u);
        goto block_8018E9D8;  /* not taken */
    }

block_8018E9D8:
    v0 = a0 << 24;
    v0 = (int32_t)v0 >> 24;
    v1 = 48;
    int _bc_8018E9E4 = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = sp - s2;
    if (_bc_8018E9E4) {
        psx_check_interrupts_at(cpu, 0x8018E9F8u);
        goto block_8018E9F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E9ECu);
        goto block_8018E9EC;  /* not taken */
    }

block_8018E9EC:
    s2 = s2 + -1;
    cpu->write_byte(s2, (uint8_t)v1);
    /* fall through to block_8018E9F4 */

block_8018E9F4:
    v0 = sp - s2;
    /* fall through to block_8018E9F8 */

block_8018E9F8:
    s6 = v0 + 56;
    /* fall through to block_8018E9FC */

block_8018E9FC:
    a3 = cpu->read_word(sp + 64);
    int _bc_8018EA00 = (fp == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s4 = s6 + a3;
    if (_bc_8018EA00) {
        psx_check_interrupts_at(cpu, 0x8018EA0Cu);
        goto block_8018EA0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA08u);
        goto block_8018EA08;  /* not taken */
    }

block_8018EA08:
    s4 = s4 + 1;
    /* fall through to block_8018EA0C */

block_8018EA0C:
    v0 = s3 & 0x0040;  /* 0x8018EA0C: 0x32620040 */
    int _bc_8018EA10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EA14: 0x00000000 */
    if (_bc_8018EA10) {
        psx_check_interrupts_at(cpu, 0x8018EA1Cu);
        goto block_8018EA1C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA18u);
        goto block_8018EA18;  /* not taken */
    }

block_8018EA18:
    s4 = s4 + 2;
    /* fall through to block_8018EA1C */

block_8018EA1C:
    s5 = cpu->read_word(sp + 60);
    /* nop */  /* 0x8018EA20: 0x00000000 */
    v0 = ((int32_t)s5 < (int32_t)s4) ? 1 : 0;  /* 0x8018EA24: 0x02B4102A */
    int _bc_8018EA28 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 & 0x0030;  /* 0x8018EA2C: 0x32620030 */
    if (_bc_8018EA28) {
        psx_check_interrupts_at(cpu, 0x8018EA34u);
        goto block_8018EA34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA30u);
        goto block_8018EA30;  /* not taken */
    }

block_8018EA30:
    s5 = s4;
    /* fall through to block_8018EA34 */

block_8018EA34:
    int _bc_8018EA34 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = fp;
    if (_bc_8018EA34) {
        psx_check_interrupts_at(cpu, 0x8018EA70u);
        goto block_8018EA70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA3Cu);
        goto block_8018EA3C;  /* not taken */
    }

block_8018EA3C:
    a3 = cpu->read_word(sp + 68);
    /* nop */  /* 0x8018EA40: 0x00000000 */
    int _bc_8018EA44 = (a3 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)s5 < (int32_t)a3) ? 1 : 0;  /* 0x8018EA48: 0x02A7102A */
    if (_bc_8018EA44) {
        psx_check_interrupts_at(cpu, 0x8018EA70u);
        goto block_8018EA70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA4Cu);
        goto block_8018EA4C;  /* not taken */
    }

block_8018EA4C:
    int _bc_8018EA4C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = s5;
    if (_bc_8018EA4C) {
        psx_check_interrupts_at(cpu, 0x8018EA70u);
        goto block_8018EA70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA54u);
        goto block_8018EA54;  /* not taken */
    }

block_8018EA54:
    ra = 0x8018EA5Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 32;
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EA5C:
    a3 = cpu->read_word(sp + 68);
    s0 = s0 + 1;
    v0 = ((int32_t)s0 < (int32_t)a3) ? 1 : 0;  /* 0x8018EA64: 0x0207102A */
    int _bc_8018EA68 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = fp;
    if (_bc_8018EA68) {
        psx_check_interrupts_at(cpu, 0x8018EA54u);
        goto block_8018EA54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA70u);
        goto block_8018EA70;  /* not taken */
    }

block_8018EA70:
    int _bc_8018EA70 = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 & 0x0040;  /* 0x8018EA74: 0x32620040 */
    if (_bc_8018EA70) {
        psx_check_interrupts_at(cpu, 0x8018EA84u);
        goto block_8018EA84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA78u);
        goto block_8018EA78;  /* not taken */
    }

block_8018EA78:
    ra = 0x8018EA80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EA7C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EA80:
    v0 = s3 & 0x0040;  /* 0x8018EA80: 0x32620040 */
    /* fall through to block_8018EA84 */

block_8018EA84:
    int _bc_8018EA84 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = s3 & 0x0030;  /* 0x8018EA88: 0x32630030 */
    if (_bc_8018EA84) {
        psx_check_interrupts_at(cpu, 0x8018EAA4u);
        goto block_8018EAA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EA8Cu);
        goto block_8018EA8C;  /* not taken */
    }

block_8018EA8C:
    ra = 0x8018EA94u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 48;
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EA94:
    a0 = (int32_t)(int8_t)cpu->read_byte(s7);
    ra = 0x8018EAA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EA9C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EAA0:
    v1 = s3 & 0x0030;  /* 0x8018EAA0: 0x32630030 */
    /* fall through to block_8018EAA4 */

block_8018EAA4:
    v0 = 32;
    int _bc_8018EAA8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EAAC: 0x00000000 */
    if (_bc_8018EAA8) {
        psx_check_interrupts_at(cpu, 0x8018EAE0u);
        goto block_8018EAE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EAB0u);
        goto block_8018EAB0;  /* not taken */
    }

block_8018EAB0:
    a3 = cpu->read_word(sp + 68);
    /* nop */  /* 0x8018EAB4: 0x00000000 */
    v0 = ((int32_t)s5 < (int32_t)a3) ? 1 : 0;  /* 0x8018EAB8: 0x02A7102A */
    int _bc_8018EABC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = s5;
    if (_bc_8018EABC) {
        psx_check_interrupts_at(cpu, 0x8018EAE0u);
        goto block_8018EAE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EAC4u);
        goto block_8018EAC4;  /* not taken */
    }

block_8018EAC4:
    ra = 0x8018EACCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 48;
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EACC:
    a3 = cpu->read_word(sp + 68);
    s0 = s0 + 1;
    v0 = ((int32_t)s0 < (int32_t)a3) ? 1 : 0;  /* 0x8018EAD4: 0x0207102A */
    int _bc_8018EAD8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EADC: 0x00000000 */
    if (_bc_8018EAD8) {
        psx_check_interrupts_at(cpu, 0x8018EAC4u);
        goto block_8018EAC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EAE0u);
        goto block_8018EAE0;  /* not taken */
    }

block_8018EAE0:
    a3 = cpu->read_word(sp + 60);
    s0 = s4;
    v0 = ((int32_t)s0 < (int32_t)a3) ? 1 : 0;  /* 0x8018EAE8: 0x0207102A */
    int _bc_8018EAEC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EAF0: 0x00000000 */
    if (_bc_8018EAEC) {
        psx_check_interrupts_at(cpu, 0x8018EB10u);
        goto block_8018EB10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EAF4u);
        goto block_8018EAF4;  /* not taken */
    }

block_8018EAF4:
    ra = 0x8018EAFCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 48;
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EAFC:
    a3 = cpu->read_word(sp + 60);
    s0 = s0 + 1;
    v0 = ((int32_t)s0 < (int32_t)a3) ? 1 : 0;  /* 0x8018EB04: 0x0207102A */
    int _bc_8018EB08 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EB0C: 0x00000000 */
    if (_bc_8018EB08) {
        psx_check_interrupts_at(cpu, 0x8018EAF4u);
        goto block_8018EAF4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EB10u);
        goto block_8018EB10;  /* not taken */
    }

block_8018EB10:
    s0 = s6 + -1;
    int _bc_8018EB14 = ((int32_t)s0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EB18: 0x00000000 */
    if (_bc_8018EB14) {
        psx_check_interrupts_at(cpu, 0x8018EB44u);
        goto block_8018EB44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EB1Cu);
        goto block_8018EB1C;  /* not taken */
    }

block_8018EB1C:
    a0 = (int32_t)(int8_t)cpu->read_byte(s2);
    s2 = s2 + 1;
    ra = 0x8018EB2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = s0 + -1;
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EB2C:
    int _bc_8018EB2C = ((int32_t)s0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EB30: 0x00000000 */
    if (_bc_8018EB2C) {
        psx_check_interrupts_at(cpu, 0x8018EB44u);
        goto block_8018EB44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EB34u);
        goto block_8018EB34;  /* not taken */
    }

block_8018EB34:
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EB38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018EB1Cu);
    goto block_8018EB1C;  /* j */

block_8018EB3C:
    ra = 0x8018EB44u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 48;
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EB44:
    a3 = cpu->read_word(sp + 64);
    /* nop */  /* 0x8018EB48: 0x00000000 */
    a3 = a3 + -1;
    int _bc_8018EB50 = ((int32_t)a3 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 64, a3);
    if (_bc_8018EB50) {
        psx_check_interrupts_at(cpu, 0x8018EB3Cu);
        goto block_8018EB3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EB58u);
        goto block_8018EB58;  /* not taken */
    }

block_8018EB58:
    v0 = s3 & 0x0010;  /* 0x8018EB58: 0x32620010 */
    int _bc_8018EB5C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EB60: 0x00000000 */
    if (_bc_8018EB5C) {
        psx_check_interrupts_at(cpu, 0x8018EB94u);
        goto block_8018EB94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EB64u);
        goto block_8018EB64;  /* not taken */
    }

block_8018EB64:
    a3 = cpu->read_word(sp + 68);
    /* nop */  /* 0x8018EB68: 0x00000000 */
    v0 = ((int32_t)s5 < (int32_t)a3) ? 1 : 0;  /* 0x8018EB6C: 0x02A7102A */
    int _bc_8018EB70 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = s5;
    if (_bc_8018EB70) {
        psx_check_interrupts_at(cpu, 0x8018EB94u);
        goto block_8018EB94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EB78u);
        goto block_8018EB78;  /* not taken */
    }

block_8018EB78:
    ra = 0x8018EB80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 32;
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EB80:
    a3 = cpu->read_word(sp + 68);
    s0 = s0 + 1;
    v0 = ((int32_t)s0 < (int32_t)a3) ? 1 : 0;  /* 0x8018EB88: 0x0207102A */
    int _bc_8018EB8C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EB90: 0x00000000 */
    if (_bc_8018EB8C) {
        psx_check_interrupts_at(cpu, 0x8018EB78u);
        goto block_8018EB78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EB94u);
        goto block_8018EB94;  /* not taken */
    }

block_8018EB94:
    v1 = cpu->read_word(sp + 68);
    /* nop */  /* 0x8018EB98: 0x00000000 */
    v0 = ((int32_t)v1 < (int32_t)s5) ? 1 : 0;  /* 0x8018EB9C: 0x0075102A */
    int _bc_8018EBA0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EBA4: 0x00000000 */
    if (_bc_8018EBA0) {
        psx_check_interrupts_at(cpu, 0x8018EBACu);
        goto block_8018EBAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018EBA8u);
        goto block_8018EBA8;  /* not taken */
    }

block_8018EBA8:
    v1 = s5;
    /* fall through to block_8018EBAC */

block_8018EBAC:
    a3 = cpu->read_word(sp + 56);
    /* nop */  /* 0x8018EBB0: 0x00000000 */
    a3 = a3 + v1;
    /* delay slot (always executes) */
    cpu->write_word(sp + 56, a3);
    psx_check_interrupts_at(cpu, 0x8018EBECu);
    goto block_8018EBEC;  /* j */

block_8018EBC0:
    ra = 0x8018EBC8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EBC4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018EDECu);
    cpu->pc = 0x8018EDECu; return;  /* CPS jal -> 0x8018EDEC */

block_8018EBC8:
    v0 = cpu->read_word(sp + 56);
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EBD0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018EBF4u);
    goto block_8018EBF4;  /* j */

block_8018EBD4:
    a3 = cpu->read_word(sp + 56);
    a0 = (int32_t)(int8_t)cpu->read_byte(s7);
    a3 = a3 + 1;
    cpu->write_word(sp + 56, a3);
    /* fall through to block_8018EBE4 */

block_8018EBE4:
    ra = 0x8018EBECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018EBE8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018ECF0u);
    cpu->pc = 0x8018ECF0u; return;  /* CPS jal -> 0x8018ECF0 */

block_8018EBEC:
    /* delay slot (always executes) */
    s7 = s7 + 1;
    psx_check_interrupts_at(cpu, 0x8018E5E0u);
    goto block_8018E5E0;  /* j */

block_8018EBF4:
    ra = cpu->read_word(sp + 116);
    fp = cpu->read_word(sp + 112);
    s7 = cpu->read_word(sp + 108);
    s6 = cpu->read_word(sp + 104);
    s5 = cpu->read_word(sp + 100);
    s4 = cpu->read_word(sp + 96);
    s3 = cpu->read_word(sp + 92);
    s2 = cpu->read_word(sp + 88);
    s1 = cpu->read_word(sp + 84);
    s0 = cpu->read_word(sp + 80);
    uint32_t _jt_8018EC1C = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 120;
    psx_check_interrupts_at(cpu, _jt_8018EC1C);
    cpu->pc = _jt_8018EC1C; return;  /* CPS: jr $ra */
}

/* The nine CPS piece-entries. Each is a coroutine continuation of
 * printf_core (0x8018E590); all mutable state lives in cpu->gpr[]/guest
 * RAM so the body survives scheduler longjmps. */
uint32_t printf_cps_8018e63c(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e64c(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e654(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e670(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e678(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e680(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e708(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e710(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}

uint32_t printf_cps_8018e76c(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  printf_cps_body(c);
  return c->gpr[2];
}
