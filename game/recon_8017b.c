/* recon_8017b.c — clean-room reconstruction of guest range
 * 0x80178000-0x8017FFFF (worktree orch-w5-e05; w5e02 owns 0x80170-0x80177).
 *
 * Faithful clean-C ports of functions still covered by cps_ transliterations
 * in game/cps_clean.c. Each port takes over its guest entry addr via
 * game/renames.map + gen_clean_table.py; the cps_<addr> def remains as
 * unreachable shadow code (expected — same class as the shadowed delegates
 * in proof/generated_zero_audit.md).
 *
 * Leaf ports keep the guest `jr $ra` interrupt check
 * (psx_check_interrupts_at with resume = $ra) exactly like the cps_ body
 * they replace; the CLEAN_ENT_CPS wrapper converts the untouched cpu->pc
 * back to $ra on return.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

/* ======== part_field4c_clear (guest 0x80179A60) ========
 * br2eu_func_80179A60 — single block, 12 bytes:
 *   sw zero, 0x4C(a1)        0x80179A60: 0xACA0004C
 *   jr ra  (delay: nop)      0x80179A64/68
 * Clears the +0x4C word of the a1 record and returns. */
void part_field4c_clear(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;
    cpu->write_word(cpu->gpr[5] + 76u, cpu->gpr[0]);  /* sw zero,0x4C(a1) */
    psx_check_interrupts_at(cpu, cpu->gpr[31]);        /* jr $ra resume check */
}
