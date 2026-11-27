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

/* swl_guest — unaligned word store: RMW of the aligned word,
 * byte lane selected by addr&3 (guest swl semantics). */
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

/* prim_ot_link_indexed — ordering-table link: slot = ot +
 * (otz&0x1ffc); writes out<<8 into the slot and back-links the
 * old head into out+2. */
static void prim_ot_link_indexed(uint32_t ot, uint32_t out, uint32_t otz)
{
  uint32_t slot = ot + (otz & 0x1ffcu);
  uint32_t old  = DB32(slot);
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
}

/* fixed_rsqrt_q12_zero_result — rsqrt Q12 edge: zero input ->
 * return 0 via ra-resume. */
uint32_t fixed_rsqrt_q12_zero_result(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t resume_pc = cpu->gpr[31];

  cpu->gpr[2] = 0;
  psx_check_interrupts_at(cpu, resume_pc);
  cpu->pc = resume_pc;
  return cpu->gpr[2];
}

/* fixed_rsqrt_q12_ex_failure — rsqrt Q12 edge: exponent
 * failure -> return -1 (0xFFFFFFFF). */
uint32_t fixed_rsqrt_q12_ex_failure(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t resume_pc = cpu->gpr[31];

  cpu->gpr[2] = UINT32_MAX;
  psx_check_interrupts_at(cpu, resume_pc);
  cpu->pc = resume_pc;
  return cpu->gpr[2];
}

/* vec3_s32_h_normalize_store_tail — normalize tail piece: writes
 * the s16 x/y/z result (gpr8/9/10) to *a0=gpr5, restores ra. */
uint32_t vec3_s32_h_normalize_store_tail(void)
{
  CPUState *cpu = clean_cpu_state();

  switch (cpu->pc) {
  case 0x80194678u:
    goto block_80194678;
  default:
    goto block_8019466C;
  }

block_8019466C:
  cpu->gpr[7] = cpu->gpr[31];
  cpu->gpr[31] = 0x80194678u;
  psx_check_interrupts_at(cpu, 0x80194690u);
  cpu->pc = 0x80194690u;
  return cpu->gpr[2];

block_80194678:
  cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[8]);
  cpu->write_half(cpu->gpr[5] + 2, (uint16_t)cpu->gpr[9]);
  cpu->write_half(cpu->gpr[5] + 4, (uint16_t)cpu->gpr[10]);
  cpu->gpr[31] = cpu->gpr[7];
  {
    uint32_t resume_pc = cpu->gpr[31];
    psx_check_interrupts_at(cpu, resume_pc);
    cpu->pc = resume_pc;
  }
  return cpu->gpr[2];
}

/* fixed_recip_negative_shift_tail — recip tail: gpr9 = 0-gpr9
 * (negate exponent), returns gpr13 >> (gpr9 & 31). */
uint32_t fixed_recip_negative_shift_tail(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t resume_pc;

  cpu->gpr[9] = cpu->gpr[0] - cpu->gpr[9];
  resume_pc = cpu->gpr[31];
  cpu->gpr[2] = cpu->gpr[13] >> (cpu->gpr[9] & 0x1Fu);
  psx_check_interrupts_at(cpu, resume_pc);
  cpu->pc = resume_pc;
  return cpu->gpr[2];
}
