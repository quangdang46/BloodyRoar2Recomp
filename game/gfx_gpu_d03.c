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
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

uint32_t draw_env_timing_status_store(void)
{
  CPUState *cpu = clean_cpu_state();

  if (cpu->pc != 0u) {
    uint32_t continuation = cpu->pc;
    cpu->pc = 0;
    switch (continuation) {
    case 0x80191C10u: goto block_80191C10;
    case 0x80191C24u: goto block_80191C24;
    case 0x80191C28u: goto block_80191C28;
    case 0x80191C38u: goto block_80191C38;
    case 0x80191C3Cu: goto block_80191C3C;
    case 0x80191C50u: goto block_80191C50;
    case 0x80191C5Cu: goto block_80191C5C;
    case 0x80191C68u: goto block_80191C68;
    case 0x80191C74u: goto block_80191C74;
    case 0x80191C78u: goto block_80191C78;
    default: break;
    }
  }

block_80191C08:
  cpu->gpr[31] = 0x80191C10u;
  psx_check_interrupts_at(cpu, 0x8019B1E4u);
  cpu->pc = 0x8019B1E4u;
  return cpu->gpr[2];

block_80191C10:
  br2_st8(cpu->gpr[17] + 18u, (uint8_t)cpu->gpr[2]);
  cpu->gpr[2] &= 0x00FFu;
  cpu->gpr[4] = (int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 10u);
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[16] = cpu->gpr[4] + 19u;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C28u);
      goto block_80191C28;
    }
    psx_check_interrupts_at(cpu, 0x80191C24u);
    goto block_80191C24;
  }

block_80191C24:
  cpu->gpr[16] = cpu->gpr[4] + 16u;
  /* fall through */

block_80191C28:
  cpu->gpr[2] = (int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 14u);
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[18] = cpu->gpr[16] + cpu->gpr[2];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C3Cu);
      goto block_80191C3C;
    }
    psx_check_interrupts_at(cpu, 0x80191C38u);
    goto block_80191C38;
  }

block_80191C38:
  cpu->gpr[18] = cpu->gpr[16] + 240u;
  /* fall through */

block_80191C3C:
  cpu->gpr[3] = (int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 4u);
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < 281;
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[6] = cpu->gpr[0];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C78u);
      goto block_80191C78;
    }
    psx_check_interrupts_at(cpu, 0x80191C50u);
    goto block_80191C50;
  }

block_80191C50:
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < 353;
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[6] = 1;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C78u);
      goto block_80191C78;
    }
    psx_check_interrupts_at(cpu, 0x80191C5Cu);
    goto block_80191C5C;
  }

block_80191C5C:
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < 401;
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[6] = 2;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C78u);
      goto block_80191C78;
    }
    psx_check_interrupts_at(cpu, 0x80191C68u);
    goto block_80191C68;
  }

block_80191C68:
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < 561;
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    cpu->gpr[6] = 4;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191C78u);
      goto block_80191C78;
    }
    psx_check_interrupts_at(cpu, 0x80191C74u);
    goto block_80191C74;
  }

block_80191C74:
  cpu->gpr[6] = 3;
  /* fall through */

block_80191C78:
  cpu->gpr[3] = (int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 8u);
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] += cpu->gpr[6];
  cpu->gpr[2] = br2_ld8(cpu->gpr[2] - 31716u);
  cpu->pc = 0x80191C88u;
  return cpu->gpr[2];
}

uint32_t draw_env_timing_interpolate(void)
{
  CPUState *cpu = clean_cpu_state();

  if (cpu->pc != 0u) {
    uint32_t continuation = cpu->pc;
    cpu->pc = 0;
    switch (continuation) {
    case 0x80191CD4u: goto block_80191CD4;
    case 0x80191CE0u: goto block_80191CE0;
    case 0x80191CF4u: goto block_80191CF4;
    case 0x80191CFCu: goto block_80191CFC;
    case 0x80191D08u: goto block_80191D08;
    default: break;
    }
  }

block_80191C88:
  /* MULT v1,v0: write the full signed product to LO/HI. */
  {
    int64_t product =
        (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2];
    cpu->lo = (uint32_t)product;
    cpu->hi = (uint32_t)((uint64_t)product >> 32);
  }
#ifdef PSX_ENABLE_BLOCK_CYCLES
  psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
#endif
  cpu->gpr[3] = br2_ld8(cpu->gpr[17] + 18u);
  cpu->gpr[2] = cpu->gpr[3] << 2;
  cpu->gpr[2] += cpu->gpr[3];
  cpu->gpr[2] += cpu->gpr[6];
  cpu->gpr[2] <<= 2;
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] += cpu->gpr[2];
  cpu->gpr[3] = br2_ld16(cpu->gpr[3] - 31756u);
  cpu->gpr[1] = 0x801C0000u;
  cpu->gpr[1] += cpu->gpr[2];
  cpu->gpr[2] = br2_ld16(cpu->gpr[1] - 31754u);
  cpu->gpr[5] = (int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 12u);
  cpu->gpr[2] -= cpu->gpr[3];
  cpu->gpr[7] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
  psx_muldiv_stall(cpu);
#endif
  {
    int branch_taken = cpu->gpr[5] == cpu->gpr[0];
    cpu->gpr[4] = cpu->gpr[3] + cpu->gpr[7];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191CE0u);
      goto block_80191CE0;
    }
    psx_check_interrupts_at(cpu, 0x80191CD4u);
    goto block_80191CD4;
  }

block_80191CD4:
  {
    int64_t product =
        (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[5];
    cpu->lo = (uint32_t)product;
    cpu->hi = (uint32_t)((uint64_t)product >> 32);
  }
#ifdef PSX_ENABLE_BLOCK_CYCLES
  psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
#endif
  cpu->gpr[7] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
  psx_muldiv_stall(cpu);
#endif
  cpu->gpr[2] = (uint32_t)((int32_t)cpu->gpr[7] >> 8);
  /* fall through */

block_80191CE0:
  cpu->gpr[3] = cpu->gpr[4] + cpu->gpr[2];
  cpu->gpr[2] = br2_ld8(cpu->gpr[17] + 18u);
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    cpu->gpr[2] = (int32_t)cpu->gpr[4] < 540;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D80u);
      cpu->pc = 0x80191D80u;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191CF4u);
    goto block_80191CF4;
  }

block_80191CF4:
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[5] = 540;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D0Cu);
      cpu->pc = 0x80191D0Cu;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191CFCu);
    goto block_80191CFC;
  }

block_80191CFC:
  cpu->gpr[2] = (int32_t)cpu->gpr[4] < 3221;
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    cpu->gpr[5] = 3220;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D0Cu);
      cpu->pc = 0x80191D0Cu;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191D08u);
    goto block_80191D08;
  }

block_80191D08:
  cpu->gpr[5] = cpu->gpr[4];
  cpu->pc = 0x80191D0Cu;
  return cpu->gpr[2];
}

uint32_t draw_env_timing_span_clamp(void)
{
  CPUState *cpu = clean_cpu_state();

  if (cpu->pc != 0u) {
    uint32_t continuation = cpu->pc;
    cpu->pc = 0;
    switch (continuation) {
    case 0x80191D30u: goto block_80191D30;
    case 0x80191D3Cu: goto block_80191D3C;
    case 0x80191D40u: goto block_80191D40;
    case 0x80191D44u: goto block_80191D44;
    case 0x80191D4Cu: goto block_80191D4C;
    case 0x80191D58u: goto block_80191D58;
    case 0x80191D60u: goto block_80191D60;
    case 0x80191D64u: goto block_80191D64;
    case 0x80191D78u: goto block_80191D78;
    default: break;
    }
  }

block_80191D0C:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] += cpu->gpr[6];
  cpu->gpr[2] = br2_ld8(cpu->gpr[2] - 31716u);
  cpu->gpr[4] = cpu->gpr[5];
  cpu->gpr[2] <<= 2;
  cpu->gpr[5] = cpu->gpr[4] + cpu->gpr[2];
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[5];
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[2] = (int32_t)cpu->gpr[16] < 19;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D44u);
      goto block_80191D44;
    }
    psx_check_interrupts_at(cpu, 0x80191D30u);
    goto block_80191D30;
  }

block_80191D30:
  cpu->gpr[2] = (int32_t)cpu->gpr[3] < 3261;
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    cpu->gpr[5] = 3260;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D40u);
      goto block_80191D40;
    }
    psx_check_interrupts_at(cpu, 0x80191D3Cu);
    goto block_80191D3C;
  }

block_80191D3C:
  cpu->gpr[5] = cpu->gpr[3];
  /* fall through */

block_80191D40:
  cpu->gpr[2] = (int32_t)cpu->gpr[16] < 19;
  /* fall through */

block_80191D44:
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[3] = cpu->gpr[5];
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D60u);
      goto block_80191D60;
    }
    psx_check_interrupts_at(cpu, 0x80191D4Cu);
    goto block_80191D4C;
  }

block_80191D4C:
  cpu->gpr[2] = (int32_t)cpu->gpr[16] < 304;
  {
    int branch_taken = cpu->gpr[2] == cpu->gpr[0];
    cpu->gpr[5] = 303;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D64u);
      goto block_80191D64;
    }
    psx_check_interrupts_at(cpu, 0x80191D58u);
    goto block_80191D58;
  }

block_80191D58:
  cpu->gpr[5] = cpu->gpr[16];
  psx_check_interrupts_at(cpu, 0x80191D64u);
  goto block_80191D64;

block_80191D60:
  cpu->gpr[5] = 19;
  /* fall through */

block_80191D64:
  cpu->gpr[16] = cpu->gpr[5];
  cpu->gpr[5] = cpu->gpr[16] + 2u;
  cpu->gpr[2] = (int32_t)cpu->gpr[18] < (int32_t)cpu->gpr[5];
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[2] = (int32_t)cpu->gpr[18] < 306;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191E18u);
      cpu->pc = 0x80191E18u;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191D78u);
    goto block_80191D78;
  }

block_80191D78:
  cpu->gpr[5] = 305;
  psx_check_interrupts_at(cpu, 0x80191E0Cu);
  cpu->pc = 0x80191E0Cu;
  return cpu->gpr[2];
}

uint32_t draw_env_timing_axis_limit(void)
{
  CPUState *cpu = clean_cpu_state();

  if (cpu->pc != 0u) {
    uint32_t continuation = cpu->pc;
    cpu->pc = 0;
    switch (continuation) {
    case 0x80191D8Cu: goto block_80191D8C;
    default: break;
    }
  }

block_80191D80:
  cpu->gpr[2] = (int32_t)cpu->gpr[4] < 500;
  {
    int branch_taken = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[5] = 500;
    if (branch_taken) {
      psx_check_interrupts_at(cpu, 0x80191D9Cu);
      cpu->pc = 0x80191D9Cu;
      return cpu->gpr[2];
    }
    psx_check_interrupts_at(cpu, 0x80191D8Cu);
    goto block_80191D8C;
  }

block_80191D8C:
  cpu->gpr[2] = (int32_t)cpu->gpr[4] < 3251;
  cpu->pc = 0x80191D90u;
  return cpu->gpr[2];
}
