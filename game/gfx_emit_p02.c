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
extern void     gte_light_set(uint32_t a0, uint32_t a1, uint32_t a2);

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

/* guest 0x8018C1D4 — spr_desc_slot_alloc commit tail: keep v1 when it is
 * already < 32 else clamp to 0, publish the cursor at gp+0x55E0, mark the
 * slot with a1, and return slot+4. */
uint32_t spr_desc_slot_alloc_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc = ((int32_t)cpu->gpr[3] < 32);
  cpu->gpr[4] = cpu->gpr[6];                    /* delay slot: a0 = a2 */
  if (!_bc) cpu->gpr[3] = cpu->gpr[0];
  cpu->gpr[2] = cpu->gpr[4] + 4;
  br2_st32(cpu->gpr[7] + 21984, cpu->gpr[3]);
  br2_st32(cpu->gpr[4], cpu->gpr[5]);           /* jr $ra delay slot */
  return cpu->gpr[2];
}

/* guest 0x8018CC4C — spr_pool_a_alloc commit tail: same shape with the
 * 24-slot cursor at gp+0x55E4 and the sentinel in a2. */
uint32_t spr_pool_a_alloc_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc = ((int32_t)cpu->gpr[3] < 24);
  cpu->gpr[4] = cpu->gpr[5];                    /* delay slot: a0 = a1 */
  if (!_bc) cpu->gpr[3] = cpu->gpr[0];
  cpu->gpr[2] = cpu->gpr[4] + 4;
  br2_st32(cpu->gpr[7] + 21988, cpu->gpr[3]);
  br2_st32(cpu->gpr[4], cpu->gpr[6]);           /* jr $ra delay slot */
  return cpu->gpr[2];
}

/* guest 0x8018CE50 — sprite_node_alloc commit tail: same shape with the
 * 8-slot cursor at gp+0x55E8 and the sentinel in a2. */
uint32_t sprite_node_alloc_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc = ((int32_t)cpu->gpr[3] < 8);
  cpu->gpr[4] = cpu->gpr[5];                    /* delay slot: a0 = a1 */
  if (!_bc) cpu->gpr[3] = cpu->gpr[0];
  cpu->gpr[2] = cpu->gpr[4] + 4;
  br2_st32(cpu->gpr[7] + 21992, cpu->gpr[3]);
  br2_st32(cpu->gpr[4], cpu->gpr[6]);           /* jr $ra delay slot */
  return cpu->gpr[2];
}

/* guest 0x8018CFD4 — file_rec_size_get negative-index tail: jr $ra with
 * v0 = 0 in the delay slot. */
uint32_t file_rec_size_get_zero(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[2] = cpu->gpr[0];
  return cpu->gpr[2];
}

/* guest 0x8018D27C — LUT sign tail: jr $ra with v0 = -v0 in the delay slot. */
uint32_t lut_tail_neg(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
  return cpu->gpr[2];
}

/* guest 0x8018D290 — LUT positive tail: jr $ra; nop, v0 unchanged. */
void lut_tail_pos(void)
{
}

/* guest 0x8018D284 — trig_sin_lookup saturation tail: v1 = a0 & 0x400,
 * v0 = 0x7FFF, then tail-dispatch to the neg/pos return stubs. */
uint32_t lut_sat_sign(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[3] = cpu->gpr[4] & 0x0400;
  cpu->gpr[2] = 0x7FFF;
  if (cpu->gpr[3] != cpu->gpr[0])
    return lut_tail_neg();
  lut_tail_pos();
  return cpu->gpr[2];
}

/* guest 0x8018D2D8 — lut_sin_q12 positive tail: jr $ra; nop, v0 unchanged. */
void lut_q12_tail_pos(void)
{
}

/* guest 0x8018D2E0 — GTE light set with Q4 inputs: a0-a2 are shifted left
 * by 4, then the body falls through to gte_light_set (0x8018D2EC). */
void gte_light_set_q4(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[4] = cpu->gpr[4] << 4;
  cpu->gpr[5] = cpu->gpr[5] << 4;
  cpu->gpr[6] = cpu->gpr[6] << 4;
  gte_light_set(cpu->gpr[4], cpu->gpr[5], cpu->gpr[6]);
}
