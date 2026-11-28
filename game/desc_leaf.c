/* desc_leaf.c — clean CPS ports for the tiny single-block leaf pieces in
 * the descriptor/event region (0x8017A6E0 .. 0x8017BD8C).
 *
 * Each guest piece is one basic block ending in `jr $ra`; the only yield
 * point is the IRQ check latched on the return target, so each port keeps
 * the clean-CPS idiom: latch $ra, run the delay slot, psx_check_interrupts_at,
 * tail-dispatch cpu->pc. Registered in game/renames.map (canonical names)
 * and game/clean_cps.map (piece addr -> CLEAN_ENT_CPS body).
 *
 * These are internal split pieces of the larger desc/ev dispatchers
 * (see renames.map neighbours: desc_cursor_select, desc_flagbit_dispatch,
 * stat_field_dispatch, ev4_branch_dispatch), not standalone functions --
 * names describe only the observed guest ops.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* ======== desc_ret_empty_a (guest 0x8017a6e0) ======== */

/* Faithful CPS transliteration of br2eu_func_8017A6E0 (full_34.c):
 * bare `jr $ra` with nop delay slot — shared early-return tail. */
void desc_ret_empty_a(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017A6E0: ;
    uint32_t jt_8017A6E0 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A6E4: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_8017A6E0);
    cpu->pc = jt_8017A6E0; return;  /* CPS: jr $ra */
}

/* ======== desc_f88_sel (guest 0x8017a990) ======== */

/* Faithful CPS port of br2eu_func_8017A990 (full_34.c, 3 blocks):
 *   v0 = lhu(a2); v0 <<= 16; v1 = sx8(v0 >> 24)  (sign byte of the
 *   halfword's high byte); v0 = 1 (delay slot).
 *   v1 == $zero -> tail 0x8017A9B8 (desc_f88_get: v0 = *(a1+0x88))
 *   v1 == 1      -> tail 0x8017A9C4 (desc_f88_set_a2p2: v0=a2+2,
 *                  *(a1+0x88)=v0)
 *   else         -> jr $ra returning v0 = 1.
 * Continuation addrs 0x8017a9a8/0x8017a9b0 re-enter via cpu->pc. */
void desc_f88_sel(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

  if (cpu->pc != 0u) {
    uint32_t _cont = cpu->pc; cpu->pc = 0;
    switch (_cont) {
      case 0x8017A9A8u: goto block_8017A9A8;
      case 0x8017A9B0u: goto block_8017A9B0;
      default: break;
    }
  }

block_8017A990: ;
    v0 = cpu->read_half(a2);
    v0 <<= 16;
    v1 = (uint32_t)((int32_t)v0 >> 24);
    {
      int cond = (v1 == zero);
      v0 = 1;                       /* delay slot */
      if (cond) {
        psx_check_interrupts_at(cpu, 0x8017A9B8u);
        cpu->pc = 0x8017A9B8u; return;  /* CPS taken: split */
      }
    }
    psx_check_interrupts_at(cpu, 0x8017A9A8u);

block_8017A9A8: ;
    {
      int cond = (v1 == v0);
      if (cond) {
        psx_check_interrupts_at(cpu, 0x8017A9C4u);
        cpu->pc = 0x8017A9C4u; return;  /* CPS taken: split */
      }
    }
    psx_check_interrupts_at(cpu, 0x8017A9B0u);

block_8017A9B0: ;
    {
      uint32_t jt_8017A9B0 = ra;  /* latch indirect target before delay slot */
      /* delay slot (always executes) */
      /* nop */  /* 0x8017A9B4: 0x00000000 */
      psx_check_interrupts_at(cpu, jt_8017A9B0);
      cpu->pc = jt_8017A9B0; return;  /* CPS: jr $ra */
    }
}

/* ======== desc_f88_get (guest 0x8017a9b8) ======== */

/* Faithful CPS transliteration of br2eu_func_8017A9B8 (full_34.c):
 * v0 = *(a1 + 0x88); jr $ra. */
void desc_f88_get(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017A9B8:
    v0 = cpu->read_word(a1 + 136);
    uint32_t jt_8017A9BC = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A9C0: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_8017A9BC);
    cpu->pc = jt_8017A9BC; return;  /* CPS: jr $ra */
}

/* ======== desc_f88_set_a2p2 (guest 0x8017a9c4) ======== */

/* Faithful CPS transliteration of br2eu_func_8017A9C4 (full_34.c):
 * v0 = a2 + 2; *(a1 + 0x88) = v0 (delay slot); jr $ra. */
void desc_f88_set_a2p2(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017A9C4:
    v0 = a2 + 2;
    uint32_t jt_8017A9C8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(a1 + 136, v0);
    psx_check_interrupts_at(cpu, jt_8017A9C8);
    cpu->pc = jt_8017A9C8; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_a2p2 (guest 0x8017a9fc) ======== */

/* Faithful CPS transliteration of br2eu_func_8017A9FC (full_34.c):
 * v0 = a2 + 2 (delay slot); jr $ra. */
void desc_ret_a2p2(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017A9FC: ;
    uint32_t jt_8017A9FC = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 2;
    psx_check_interrupts_at(cpu, jt_8017A9FC);
    cpu->pc = jt_8017A9FC; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_zero (guest 0x8017aa04) ======== */

/* Faithful CPS transliteration of br2eu_func_8017AA04 (full_34.c):
 * v0 = 0 (delay slot); jr $ra. */
void desc_ret_zero(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017AA04: ;
    uint32_t jt_8017AA04 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, jt_8017AA04);
    cpu->pc = jt_8017AA04; return;  /* CPS: jr $ra */
}

/* ======== desc_f84_f90_commit (guest 0x8017ab5c) ======== */

/* Faithful CPS transliteration of br2eu_func_8017AB5C (full_34.c):
 * v1 = sext(half(a0 + 0xBE)) << 4; a0 = a2 + 2; *(a1 + 0x84) = a0;
 * half(a1 + 0x90) = v1 (delay slot); jr $ra. */
void desc_f84_f90_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017AB5C:
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + 190);
    a0 = a2 + 2;
    cpu->write_word(a1 + 132, a0);
    v1 <<= 4;
    uint32_t jt_8017AB6C = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a1 + 144, (uint16_t)v1);
    psx_check_interrupts_at(cpu, jt_8017AB6C);
    cpu->pc = jt_8017AB6C; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_a2p4 (guest 0x8017ac44) ======== */

/* Faithful CPS transliteration of br2eu_func_8017AC44 (full_34.c):
 * v0 = a2 + 4 (delay slot); jr $ra. */
void desc_ret_a2p4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017AC44: ;
    uint32_t jt_8017AC44 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 4;
    psx_check_interrupts_at(cpu, jt_8017AC44);
    cpu->pc = jt_8017AC44; return;  /* CPS: jr $ra */
}

/* ======== desc_h2_f30_sum (guest 0x8017ae00) ======== */

/* Faithful CPS transliteration of br2eu_func_8017AE00 (full_34.c):
 * v1 = half(a2 + 2); v0 = *(a0 + 0x30); v0 = v1 + v0; jr $ra. */
void desc_h2_f30_sum(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017AE00:
    v1 = cpu->read_half(a2 + 2);
    v0 = cpu->read_word(a0 + 48);
    /* nop */  /* 0x8017AE08: 0x00000000 */
    v0 = v1 + v0;
    uint32_t jt_8017AE10 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AE14: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_8017AE10);
    cpu->pc = jt_8017AE10; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_empty_b (guest 0x8017ae10) ======== */

/* Faithful CPS transliteration of br2eu_func_8017AE10 (full_34.c):
 * bare `jr $ra` with nop delay slot — second shared return tail. */
void desc_ret_empty_b(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017AE10: ;
    uint32_t jt_8017AE10 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AE14: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_8017AE10);
    cpu->pc = jt_8017AE10; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_a2p4_b (guest 0x8017b648) ======== */

/* Faithful CPS transliteration of br2eu_func_8017B648 (full_35.c):
 * v0 = a2 + 4 (delay slot); jr $ra. */
void desc_ret_a2p4_b(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017B648: ;
    uint32_t jt_8017B648 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 4;
    psx_check_interrupts_at(cpu, jt_8017B648);
    cpu->pc = jt_8017B648; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_a2p6 (guest 0x8017b8b4) ======== */

/* Faithful CPS transliteration of br2eu_func_8017B8B4 (full_35.c):
 * v0 = a2 + 6 (delay slot); jr $ra. */
void desc_ret_a2p6(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017B8B4: ;
    uint32_t jt_8017B8B4 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 6;
    psx_check_interrupts_at(cpu, jt_8017B8B4);
    cpu->pc = jt_8017B8B4; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_a2p6_b (guest 0x8017baac) ======== */

/* Faithful CPS transliteration of br2eu_func_8017BAAC (full_35.c):
 * v0 = a2 + 6 (delay slot); jr $ra. */
void desc_ret_a2p6_b(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017BAAC: ;
    uint32_t jt_8017BAAC = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 6;
    psx_check_interrupts_at(cpu, jt_8017BAAC);
    cpu->pc = jt_8017BAAC; return;  /* CPS: jr $ra */
}

/* ======== desc_ret_a2p2_b (guest 0x8017bd8c) ======== */

/* Faithful CPS transliteration of br2eu_func_8017BD8C (full_35.c):
 * v0 = a2 + 2 (delay slot); jr $ra. */
void desc_ret_a2p2_b(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8017BD8C: ;
    uint32_t jt_8017BD8C = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 2;
    psx_check_interrupts_at(cpu, jt_8017BD8C);
    cpu->pc = jt_8017BD8C; return;  /* CPS: jr $ra */
}
