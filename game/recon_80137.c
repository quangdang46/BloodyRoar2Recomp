#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st32(uint32_t a, uint32_t v);

/* battle_intro_fsm_tail_cps — guest 0x80137528/0x8013752C/0x80137548,
 * the shared exit tail of battle_intro_fsm (0x80137098) / its alias body:
 *   s0 += 0x758; sw v0,4(s0); sw zero,0x801F0940/0x801F0944/0x801F0930;
 *   restore s0-s6/ra, sp += 0x38, jr ra.
 * 3-piece CPS chain (clean_cps.map routes all three addrs here). The
 * intermediate pieces carry no psx_check_interrupts_at — the guest has no
 * IRQ boundary between them — but each remains its own dispatch quantum
 * (cpu->pc = next; return) so per-piece cycle accounting matches the
 * generated CPS twins. All state lives in cpu->gpr/guest RAM so scheduler
 * longjmps preserve it. */
uint32_t battle_intro_fsm_tail_cps(void)
{
    CPUState *c = clean_cpu_state();
    uint32_t pc;
    if (!c) return 0;
    pc = c->pc;
    if (pc == 0x8013752cu) goto b52C;
    if (pc == 0x80137548u) goto b548;
    /* entry 0x80137528 (and stale/0 pc) falls through */

    c->gpr[16] = c->gpr[16] + 1880u;                /* s0 += 0x758 */
    c->pc = 0x8013752cu;                            /* fallthrough piece */
    return c->gpr[2];

b52C:
    br2_st32(c->gpr[16] + 4u, c->gpr[2]);            /* sw v0,4(s0) */
    c->gpr[2] = 0x801F0000u;                         /* v0 = 0x801F0000 */
    br2_st32(c->gpr[2] + 2368u, c->gpr[0]);          /* sw zero,0x801F0940 */
    c->gpr[2] = c->gpr[2] + 2368u;                   /* v0 += 0x940 */
    br2_st32(c->gpr[2] + 4u, c->gpr[0]);             /* sw zero,0x801F0944 */
    c->gpr[2] = 0x801F0000u;                         /* v0 = 0x801F0000 */
    br2_st32(c->gpr[2] + 2352u, c->gpr[0]);          /* sw zero,0x801F0930 */
    c->pc = 0x80137548u;                             /* fallthrough piece */
    return c->gpr[2];

b548:
    c->gpr[31] = br2_ld32(c->gpr[29] + 52u);         /* restore ra */
    c->gpr[22] = br2_ld32(c->gpr[29] + 48u);         /* restore s6 */
    c->gpr[21] = br2_ld32(c->gpr[29] + 44u);         /* restore s5 */
    c->gpr[20] = br2_ld32(c->gpr[29] + 40u);         /* restore s4 */
    c->gpr[19] = br2_ld32(c->gpr[29] + 36u);         /* restore s3 */
    c->gpr[18] = br2_ld32(c->gpr[29] + 32u);         /* restore s2 */
    c->gpr[17] = br2_ld32(c->gpr[29] + 28u);         /* restore s1 */
    c->gpr[16] = br2_ld32(c->gpr[29] + 24u);         /* restore s0 */
    c->gpr[29] = c->gpr[29] + 56u;                   /* delay: sp += 0x38 */
    { uint32_t _jt = c->gpr[31];                     /* jr ra */
      psx_check_interrupts_at(c, _jt);
      c->pc = _jt;
      return c->gpr[2]; }
}
