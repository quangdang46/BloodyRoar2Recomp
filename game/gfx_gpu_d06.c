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
extern uint32_t br2_call_generated(uint32_t addr, uint32_t a0, uint32_t a1,
                                   uint32_t a2, uint32_t a3);
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

/*
 * GPU image upload through the indirect GP0/status and DMA register cells.
 * The block PCs are registered in clean_cps.map because every status poll
 * can yield and must resume at its guest continuation.
 */
uint32_t gpu_image_transfer_dma(void)
{
  CPUState *cpu = clean_cpu_state();

  if (cpu->pc != 0u) {
    switch (cpu->pc) {
    case 0x80192BA8u: goto block_80192BA8;
    case 0x80192BB8u: goto block_80192BB8;
    case 0x80192BD4u: goto block_80192BD4;
    case 0x80192BDCu: goto block_80192BDC;
    case 0x80192BE0u: goto block_80192BE0;
    case 0x80192BF0u: goto block_80192BF0;
    case 0x80192C10u: goto block_80192C10;
    case 0x80192C18u: goto block_80192C18;
    case 0x80192C1Cu: goto block_80192C1C;
    case 0x80192C20u: goto block_80192C20;
    case 0x80192C4Cu: goto block_80192C4C;
    case 0x80192C54u: goto block_80192C54;
    case 0x80192C80u: goto block_80192C80;
    case 0x80192C84u: goto block_80192C84;
    case 0x80192C8Cu: goto block_80192C8C;
    case 0x80192C94u: goto block_80192C94;
    case 0x80192CB4u: goto block_80192CB4;
    case 0x80192D2Cu: goto block_80192D2C;
    case 0x80192D30u: goto block_80192D30;
    case 0x80192D38u: goto block_80192D38;
    case 0x80192D40u: goto block_80192D40;
    case 0x80192D60u: goto block_80192D60;
    case 0x80192D70u: goto block_80192D70;
    case 0x80192D90u: goto block_80192D90;
    case 0x80192D98u: goto block_80192D98;
    case 0x80192DDCu: goto block_80192DDC;
    case 0x80192DE0u: goto block_80192DE0;
    default: break;
    }
  }

block_80192B80:
  cpu->gpr[29] -= 40;
  cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
  cpu->gpr[17] = cpu->gpr[4];
  cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
  cpu->gpr[18] = cpu->gpr[5];
  cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[31]);
  cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[20]);
  cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[19]);
  cpu->gpr[31] = 0x80192BA8u;
  cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
  psx_check_interrupts_at(cpu, 0x801936B0u);
  cpu->pc = 0x801936B0u;
  return cpu->gpr[2];

block_80192BA8:
  cpu->gpr[5] = (uint32_t)(int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 4);
  cpu->gpr[3] = br2_ld16(cpu->gpr[17] + 4);
  {
    int take = (int32_t)cpu->gpr[5] < 0;
    cpu->gpr[4] = cpu->gpr[3];
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192BDCu);
      goto block_80192BDC;
    }
    psx_check_interrupts_at(cpu, 0x80192BB8u);
    goto block_80192BB8;
  }

block_80192BB8:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = (uint32_t)(int32_t)(int16_t)br2_ld16(cpu->gpr[2] - 31964u);
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld16(cpu->gpr[3] - 31964u);
  cpu->gpr[2] = (int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[5];
  {
    int take = (cpu->gpr[2] == cpu->gpr[0]);
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192BE0u);
      goto block_80192BE0;
    }
    psx_check_interrupts_at(cpu, 0x80192BD4u);
    goto block_80192BD4;
  }

block_80192BD4:
  cpu->gpr[4] = cpu->gpr[3];
  psx_check_interrupts_at(cpu, 0x80192BE0u);
  goto block_80192BE0;

block_80192BDC:
  cpu->gpr[4] = cpu->gpr[0];

block_80192BE0:
  cpu->gpr[5] = (uint32_t)(int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 6);
  cpu->gpr[3] = br2_ld16(cpu->gpr[17] + 6);
  {
    int take = (int32_t)cpu->gpr[5] < 0;
    br2_st16(cpu->gpr[17] + 4, (uint16_t)cpu->gpr[4]);
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192C18u);
      goto block_80192C18;
    }
    psx_check_interrupts_at(cpu, 0x80192BF0u);
    goto block_80192BF0;
  }

block_80192BF0:
  cpu->gpr[4] = cpu->gpr[3];
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = (uint32_t)(int32_t)(int16_t)br2_ld16(cpu->gpr[2] - 31962u);
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld16(cpu->gpr[3] - 31962u);
  cpu->gpr[2] = (int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[5];
  {
    int take = (cpu->gpr[2] == cpu->gpr[0]);
    cpu->gpr[2] = cpu->gpr[4] << 16;
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192C20u);
      goto block_80192C20;
    }
    cpu->gpr[4] = cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x80192C10u);
    goto block_80192C10;
  }

block_80192C10:
  cpu->gpr[4] = cpu->gpr[3];
  psx_check_interrupts_at(cpu, 0x80192C1Cu);
  goto block_80192C1C;

block_80192C18:
  cpu->gpr[4] = cpu->gpr[0];

block_80192C1C:
  cpu->gpr[2] = cpu->gpr[4] << 16;

block_80192C20:
  cpu->gpr[3] = (uint32_t)(int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 4);
  cpu->gpr[2] = (uint32_t)((int32_t)cpu->gpr[2] >> 16);
  {
    int64_t product = (int64_t)(int32_t)cpu->gpr[3] *
                      (int64_t)(int32_t)cpu->gpr[2];
    cpu->lo = (uint32_t)product;
    cpu->hi = (uint32_t)(product >> 32);
  }
#ifdef PSX_ENABLE_BLOCK_CYCLES
  psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
#endif
  br2_st16(cpu->gpr[17] + 6, (uint16_t)cpu->gpr[4]);
  cpu->gpr[6] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
  psx_muldiv_stall(cpu);
#endif
  cpu->gpr[3] = cpu->gpr[6] + 1;
  cpu->gpr[2] = cpu->gpr[3] >> 31;
  cpu->gpr[3] += cpu->gpr[2];
  cpu->gpr[4] = (uint32_t)((int32_t)cpu->gpr[3] >> 1);
  {
    int take = (int32_t)cpu->gpr[4] > 0;
    cpu->gpr[16] = (uint32_t)((int32_t)cpu->gpr[3] >> 5);
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192C54u);
      goto block_80192C54;
    }
    psx_check_interrupts_at(cpu, 0x80192C4Cu);
    goto block_80192C4C;
  }

block_80192C4C:
  cpu->gpr[2] = 0xFFFFFFFFu;
  psx_check_interrupts_at(cpu, 0x80192DE0u);
  goto block_80192DE0;

block_80192C54:
  cpu->gpr[3] = cpu->gpr[16];
  cpu->gpr[2] = cpu->gpr[3] << 4;
  cpu->gpr[16] = cpu->gpr[4] - cpu->gpr[2];
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = br2_ld32(cpu->gpr[2] - 31704u);
  cpu->gpr[20] = cpu->gpr[3];
  cpu->gpr[2] = br2_ld32(cpu->gpr[2]);
  cpu->gpr[3] = 0x04000000u;
  cpu->gpr[2] &= cpu->gpr[3];
  {
    int take = cpu->gpr[2] != cpu->gpr[0];
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192CB4u);
      goto block_80192CB4;
    }
    psx_check_interrupts_at(cpu, 0x80192C80u);
    goto block_80192C80;
  }

block_80192C80:
  cpu->gpr[19] = 0x04000000u;

block_80192C84:
  cpu->gpr[31] = 0x80192C8Cu;
  psx_check_interrupts_at(cpu, 0x801936E4u);
  cpu->pc = 0x801936E4u;
  return cpu->gpr[2];

block_80192C8C:
  {
    int take = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[2] = 0xFFFFFFFFu;
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192DE0u);
      goto block_80192DE0;
    }
    psx_check_interrupts_at(cpu, 0x80192C94u);
    goto block_80192C94;
  }

block_80192C94:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = br2_ld32(cpu->gpr[2] - 31704u);
  cpu->gpr[2] = br2_ld32(cpu->gpr[2]);
  cpu->gpr[2] &= cpu->gpr[19];
  {
    int take = cpu->gpr[2] == cpu->gpr[0];
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192C84u);
      goto block_80192C84;
    }
    psx_check_interrupts_at(cpu, 0x80192CB4u);
    goto block_80192CB4;
  }

block_80192CB4:
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld32(cpu->gpr[3] - 31704u);
  cpu->gpr[2] = 0x04000000u;
  br2_st32(cpu->gpr[3], cpu->gpr[2]);
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld32(cpu->gpr[3] - 31708u);
  cpu->gpr[2] = 0x01000000u;
  br2_st32(cpu->gpr[3], cpu->gpr[2]);
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld32(cpu->gpr[3] - 31708u);
  cpu->gpr[2] = 0xC0000000u;
  br2_st32(cpu->gpr[3], cpu->gpr[2]);
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld32(cpu->gpr[3] - 31708u);
  cpu->gpr[2] = br2_ld32(cpu->gpr[17]);
  br2_st32(cpu->gpr[3], cpu->gpr[2]);
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld32(cpu->gpr[3] - 31708u);
  cpu->gpr[2] = br2_ld32(cpu->gpr[17] + 4);
  br2_st32(cpu->gpr[3], cpu->gpr[2]);
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld32(cpu->gpr[3] - 31704u);
  cpu->gpr[2] = br2_ld32(cpu->gpr[3]);
  cpu->gpr[3] = 0x08000000u;
  cpu->gpr[2] &= cpu->gpr[3];
  {
    int take = cpu->gpr[2] != cpu->gpr[0];
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192D60u);
      goto block_80192D60;
    }
    psx_check_interrupts_at(cpu, 0x80192D2Cu);
    goto block_80192D2C;
  }

block_80192D2C:
  cpu->gpr[17] = 0x08000000u;

block_80192D30:
  cpu->gpr[31] = 0x80192D38u;
  psx_check_interrupts_at(cpu, 0x801936E4u);
  cpu->pc = 0x801936E4u;
  return cpu->gpr[2];

block_80192D38:
  {
    int take = cpu->gpr[2] != cpu->gpr[0];
    cpu->gpr[2] = 0xFFFFFFFFu;
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192DE0u);
      goto block_80192DE0;
    }
    psx_check_interrupts_at(cpu, 0x80192D40u);
    goto block_80192D40;
  }

block_80192D40:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = br2_ld32(cpu->gpr[2] - 31704u);
  cpu->gpr[2] = br2_ld32(cpu->gpr[2]);
  cpu->gpr[2] &= cpu->gpr[17];
  {
    int take = cpu->gpr[2] == cpu->gpr[0];
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192D30u);
      goto block_80192D30;
    }
    psx_check_interrupts_at(cpu, 0x80192D60u);
    goto block_80192D60;
  }

block_80192D60:
  cpu->gpr[16] -= 1;
  cpu->gpr[2] = 0xFFFFFFFFu;
  {
    int take = cpu->gpr[16] == cpu->gpr[2];
    cpu->gpr[3] = 0xFFFFFFFFu;
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192D90u);
      goto block_80192D90;
    }
    psx_check_interrupts_at(cpu, 0x80192D70u);
    goto block_80192D70;
  }

block_80192D70:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = br2_ld32(cpu->gpr[2] - 31708u);
  cpu->gpr[2] = br2_ld32(cpu->gpr[2]);
  cpu->gpr[16] -= 1;
  br2_st32(cpu->gpr[18], cpu->gpr[2]);
  {
    int take = cpu->gpr[16] != cpu->gpr[3];
    cpu->gpr[18] += 4;
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192D70u);
      goto block_80192D70;
    }
    psx_check_interrupts_at(cpu, 0x80192D90u);
    goto block_80192D90;
  }

block_80192D90:
  {
    int take = cpu->gpr[20] == cpu->gpr[0];
    cpu->gpr[3] = 0x04000000u;
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192DDCu);
      goto block_80192DDC;
    }
    psx_check_interrupts_at(cpu, 0x80192D98u);
    goto block_80192D98;
  }

block_80192D98:
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = br2_ld32(cpu->gpr[2] - 31704u);
  cpu->gpr[3] |= 0x00000003u;
  br2_st32(cpu->gpr[2], cpu->gpr[3]);
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = br2_ld32(cpu->gpr[2] - 31700u);
  cpu->gpr[4] = 0x01000000u;
  br2_st32(cpu->gpr[2], cpu->gpr[18]);
  cpu->gpr[2] = cpu->gpr[20] << 16;
  cpu->gpr[3] = 0x801C0000u;
  cpu->gpr[3] = br2_ld32(cpu->gpr[3] - 31696u);
  cpu->gpr[2] |= 0x00000010u;
  br2_st32(cpu->gpr[3], cpu->gpr[2]);
  cpu->gpr[2] = 0x801C0000u;
  cpu->gpr[2] = br2_ld32(cpu->gpr[2] - 31692u);
  cpu->gpr[4] |= 0x00000200u;
  br2_st32(cpu->gpr[2], cpu->gpr[4]);

block_80192DDC:
  cpu->gpr[2] = cpu->gpr[0];

block_80192DE0:
  cpu->gpr[31] = br2_ld32(cpu->gpr[29] + 36);
  cpu->gpr[20] = br2_ld32(cpu->gpr[29] + 32);
  cpu->gpr[19] = br2_ld32(cpu->gpr[29] + 28);
  cpu->gpr[18] = br2_ld32(cpu->gpr[29] + 24);
  cpu->gpr[17] = br2_ld32(cpu->gpr[29] + 20);
  cpu->gpr[16] = br2_ld32(cpu->gpr[29] + 16);
  {
    uint32_t return_pc = cpu->gpr[31];
    cpu->gpr[29] += 40;
    psx_check_interrupts_at(cpu, return_pc);
    cpu->pc = return_pc;
    return cpu->gpr[2];
  }
}

/* Read the unsigned command-tag byte at 0x801FAD68 + index. */
uint32_t gpu_tag_cmd_byte_get(uint32_t index)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[2] = br2_ld8(0x801FAD68u + index);
  psx_check_interrupts_at(cpu, cpu->gpr[31]);
  return cpu->gpr[2];
}

/* Send a GP0 command header followed by count words from a guest buffer.
 * The stores in both branch delay slots are intentional, including count=0.
 */
uint32_t gpu_command_words_send(void)
{
  CPUState *cpu = clean_cpu_state();

  if (cpu->pc != 0u) {
    switch (cpu->pc) {
    case 0x80192E50u: goto block_80192E50;
    case 0x80192E54u: goto block_80192E54;
    case 0x80192E70u: goto block_80192E70;
    default: break;
    }
  }

block_80192E38:
  cpu->gpr[6] = cpu->gpr[5] - 1;
  cpu->gpr[3] = br2_ld32(0x801B8428u);
  cpu->gpr[2] = 0x04000000u;
  {
    int take = cpu->gpr[5] == cpu->gpr[0];
    br2_st32(cpu->gpr[3], cpu->gpr[2]);
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192E70u);
      goto block_80192E70;
    }
    psx_check_interrupts_at(cpu, 0x80192E50u);
    goto block_80192E50;
  }

block_80192E50:
  cpu->gpr[5] = 0xFFFFFFFFu;

block_80192E54:
  cpu->gpr[3] = br2_ld32(cpu->gpr[4]);
  cpu->gpr[4] += 4;
  cpu->gpr[2] = br2_ld32(0x801B8424u);
  cpu->gpr[6] -= 1;
  {
    int take = cpu->gpr[6] != cpu->gpr[5];
    br2_st32(cpu->gpr[2], cpu->gpr[3]);
    if (take) {
      psx_check_interrupts_at(cpu, 0x80192E54u);
      goto block_80192E54;
    }
    psx_check_interrupts_at(cpu, 0x80192E70u);
    goto block_80192E70;
  }

block_80192E70:
  {
    uint32_t return_pc = cpu->gpr[31];
    cpu->gpr[2] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, return_pc);
    cpu->pc = return_pc;
    return cpu->gpr[2];
  }
}

/* This two-instruction entry falls through into gte_screen_init_defer.
 * Keep the HI result and multiply interlock, then dispatch the next guest PC.
 */
uint32_t gte_screen_init_hi_entry(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[14] = cpu->hi;
#ifdef PSX_ENABLE_BLOCK_CYCLES
  psx_muldiv_stall(cpu);
#endif
  cpu->pc = 0x80194478u;
  return cpu->gpr[2];
}
