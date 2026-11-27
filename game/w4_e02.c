/* w4_e02.c — clean CPS ports (worktree orch/w4/e02).
 *
 * Three leftover guest entries not yet routed through clean_table:
 * (the 0x80188200/0x80188308/0x80188434 prim-emit CPS twins were retired
 *  — their generated bodies dropped psx_swl stores; clean ports
 *  prim_quad_emit_54/5c + prim_emit_48 in gfx_emit_p00.c are
 *  authoritative.)
 *
 *   0x80194470  gte_screen_init_defer_mfhi — 2-insn alt entry: mfhi $t6;
 *                                      nop; falls through into
 *                                      gte_screen_init_defer (0x80194478).
 *   0x80194614  fixed_point_ret_neg1  — shared error tail: li $v0,-1;
 *                                      jr $ra (reached from
 *                                      fixed_rsqrt_q12_ex LZCR guards).
 *   0x80194940  fixed_recip_neg_exp   — negative-exponent tail of
 *                                      fixed_recip_lut: $t1=-$t1;
 *                                      $v0=$t5>>($t1&31); jr $ra.
 *
 * Bodies are faithful op-by-op CPS transliterations (same transform as
 * game/cps_clean.c / gen_cps_clean.py): identical guest ops/order/yield
 * points, cycle/icache/cosim/debug instrumentation dropped, MMIO-aware
 * cpu->read_x/cpu->write_x accessors. Dispatch addrs are registered in
 * game/clean_cps.map; renames.map carries the addr->name authority.
 *
 * NOT in this file: 0x80196370 (cd_response_process) and 0x80199440
 * (cd_mc_state_machine) are CD-ROM functions already delegated to
 * generated per AGENTS.md §4 via call_guest4 wrappers in game/cd.c —
 * their CPS continuation PCs must stay on the generated resume path,
 * so they get no clean_cps.map entries here.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern void     psx_muldiv_stall(CPUState *cpu);
extern void     psx_gte_stall(CPUState *cpu);
extern void     psx_gte_read(CPUState *cpu, uint32_t rt);
extern void     gte_execute(CPUState *cpu, uint32_t cmd);
extern uint32_t gte_read_data(CPUState *cpu, uint8_t reg);
extern void     gte_write_data(CPUState *cpu, uint8_t reg, uint32_t val);
extern void     gte_precision_store_word(uint32_t addr, uint8_t reg);

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability),
 * same convention as game/cps_clean.c. */
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

/* ======== 0x80194470 ======== */
void gte_screen_init_defer_mfhi(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80194470, Size: 8 bytes, Blocks: 1 */

block_80194470:
    t6 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* nop: write to $zero */  /* 0x80194474: 0x00440000 */
    cpu->pc = 0x80194478u; return;  /* fallthrough to split piece */
    cpu->pc = 0x80194478u; return;/* fallthrough to next function */
}

/* ======== 0x80194614 ======== */
void fixed_point_ret_neg1(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80194614, Size: 8 bytes, Blocks: 1 */

block_80194614:;
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = -1;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80194940 ======== */
void fixed_recip_neg_exp(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80194940, Size: 12 bytes, Blocks: 1 */

block_80194940:
    t1 = zero - t1;
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = (uint32_t)t5 >> (t1 & 0x1F);
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}
