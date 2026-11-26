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

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability), same
 * convention as game/cps_clean.c. */
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


/* reset_guest_cps — clean CPS transliteration of br2eu_func_80175898 (guest 0x80175964)
 * from SLES_017.22_full_32.c. */
void reset_guest_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80175944u: goto block_80175944;
            case 0x80175964u: goto block_80175964;
            case 0x8017597Cu: goto block_8017597C;
            case 0x80175980u: goto block_80175980;
            case 0x80175990u: goto block_80175990;
            case 0x801759A0u: goto block_801759A0;
            case 0x801759B0u: goto block_801759B0;
            case 0x801759C4u: goto block_801759C4;
            case 0x801759D0u: goto block_801759D0;
            case 0x80175A10u: goto block_80175A10;
            default: break;
        }
    }
    /* Address: 0x80175898, Size: 384 bytes, Blocks: 11 */

block_80175898:
    a2 = a0;
    v0 = 480;
    cpu->write_half(a2 + 206, (uint16_t)v0);
    v0 = 0x801F << 16;  /* 0x801F0000 */
    v1 = cpu->read_byte(a2 + 2);
    a0 = cpu->read_byte(a2 + 228);
    v0 = v0 + 21224;
    cpu->write_half(a2 + 124, (uint16_t)zero);
    cpu->write_half(a2 + 122, (uint16_t)zero);
    cpu->write_half(a2 + 138, (uint16_t)zero);
    cpu->write_half(a2 + 140, (uint16_t)zero);
    cpu->write_half(a2 + 142, (uint16_t)zero);
    cpu->write_half(a2 + 144, (uint16_t)zero);
    cpu->write_half(a2 + 146, (uint16_t)zero);
    cpu->write_half(a2 + 148, (uint16_t)zero);
    cpu->write_half(a2 + 150, (uint16_t)zero);
    cpu->write_half(a2 + 152, (uint16_t)zero);
    cpu->write_byte(a2 + 218, (uint8_t)zero);
    cpu->write_half(a2 + 198, (uint16_t)zero);
    cpu->write_byte(a2 + 219, (uint8_t)zero);
    cpu->write_half(a2 + 250, (uint16_t)zero);
    cpu->write_half(a2 + 200, (uint16_t)zero);
    cpu->write_byte(a2 + 262, (uint8_t)zero);
    cpu->write_half(a2 + 192, (uint16_t)zero);
    cpu->write_half(a2 + 196, (uint16_t)zero);
    cpu->write_half(a2 + 178, (uint16_t)zero);
    cpu->write_half(a2 + 208, (uint16_t)zero);
    cpu->write_byte(a2 + 221, (uint8_t)zero);
    cpu->write_half(a2 + 202, (uint16_t)zero);
    cpu->write_half(a2 + 154, (uint16_t)zero);
    cpu->write_half(a2 + 156, (uint16_t)zero);
    cpu->write_half(a2 + 158, (uint16_t)zero);
    cpu->write_half(a2 + 162, (uint16_t)zero);
    cpu->write_half(a2 + 164, (uint16_t)zero);
    cpu->write_half(a2 + 166, (uint16_t)zero);
    v1 = v1 << 5;
    v1 = v1 + v0;
    cpu->write_byte(a2 + 229, (uint8_t)a0);
    cpu->write_word(a2 + 20, v1);
    v1 = a1 & 0x000F;  /* 0x80175934: 0x30A3000F */
    v0 = (v1 < (uint32_t)5) ? 1 : 0;  /* 0x80175938: 0x2C620005 */
    int _bc_8017593C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_8017593C) {
        psx_check_interrupts_at(cpu, 0x80175A74u);
        cpu->pc = 0x80175A74u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80175944u);
        goto block_80175944;  /* not taken */
    }

block_80175944:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 9880;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80175958: 0x00000000 */
    uint32_t _jt_8017595C = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80175960: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017595C);
    cpu->pc = _jt_8017595C; return;  /* CPS: jr v0 */

block_80175964:
    v0 = 7680;
    cpu->write_half(a2 + 214, (uint16_t)v0);
    v1 = cpu->read_byte(a0 + 155);
    v0 = 4;
    int _bc_80175974 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 7680;
    if (_bc_80175974) {
        psx_check_interrupts_at(cpu, 0x80175980u);
        goto block_80175980;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017597Cu);
        goto block_8017597C;  /* not taken */
    }

block_8017597C:
    cpu->write_half(a2 + 214, (uint16_t)v0);
    /* fall through to block_80175980 */

block_80175980:
    v1 = cpu->read_byte(a0 + 156);
    v0 = 3;
    int _bc_80175988 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017598C: 0x00000000 */
    if (_bc_80175988) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80175990u);
        goto block_80175990;  /* not taken */
    }

block_80175990:
    v0 = cpu->read_byte(a2 + 1);
    /* nop */  /* 0x80175994: 0x00000000 */
    int _bc_80175998 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017599C: 0x00000000 */
    if (_bc_80175998) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801759A0u);
        goto block_801759A0;  /* not taken */
    }

block_801759A0:
    v0 = cpu->read_half(a0 + 162);
    /* nop */  /* 0x801759A4: 0x00000000 */
    int _bc_801759A8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801759AC: 0x00000000 */
    if (_bc_801759A8) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801759B0u);
        goto block_801759B0;  /* not taken */
    }

block_801759B0:
    v0 = cpu->read_half(a2 + 172);
    /* nop */  /* 0x801759B4: 0x00000000 */
    cpu->write_half(a2 + 170, (uint16_t)v0);
    /* delay slot (always executes) */
    cpu->write_half(a2 + 174, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801759D0u);
    goto block_801759D0;  /* j */

block_801759C4:
    cpu->write_half(a2 + 172, (uint16_t)zero);
    cpu->write_half(a2 + 170, (uint16_t)zero);
    cpu->write_half(a2 + 174, (uint16_t)zero);
    /* fall through to block_801759D0 */

block_801759D0:
    v1 = (int32_t)(int16_t)cpu->read_half(a2 + 176);
    v0 = cpu->read_byte(a2 + 222);
    /* nop */  /* 0x801759D8: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = 0x51EB << 16;  /* 0x51EB0000 */
    v0 = v0 | 0x851F;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    a0 = (int32_t)(int16_t)cpu->read_half(a2 + 214);
    cpu->write_half(a2 + 216, (uint16_t)zero);
    v1 = (int32_t)v1 >> 31;
    v0 = cpu->hi;
    psx_muldiv_stall(cpu);
    v0 = (int32_t)v0 >> 5;
    v0 = v0 - v1;
    int _bc_80175A08 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a2 + 176, (uint16_t)v0);
    if (_bc_80175A08) {
        psx_check_interrupts_at(cpu, 0x80175A74u);
        cpu->pc = 0x80175A74u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80175A10u);
        goto block_80175A10;  /* not taken */
    }

block_80175A10:
    uint32_t _jt_80175A10 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a2 + 214, (uint16_t)zero);
    psx_check_interrupts_at(cpu, _jt_80175A10);
    cpu->pc = _jt_80175A10; return;  /* CPS: jr $ra */
}
