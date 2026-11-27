/* recon_80138.c — clean-room reconstruction of guest range
 * 0x80138000-0x8013FFFF (unowned gap between w5e01's 0x80137 band and
 * w6e00's 0x80134-0x80136 / w5e04's 0x80140+ bands).
 *
 * Faithful clean-C ports of functions still covered by cps_ transliterations
 * in game/cps_clean.c. Each port takes over its guest entry addr via
 * game/renames.map + game/clean_cps.map + gen_clean_table.py; the cps_<addr>
 * defs remain as unreachable shadow code.
 *
 * Coroutine-style ports keep the guest `psx_check_interrupts_at` yield points
 * and the cpu->pc continuation switch exactly like the cps_ bodies they
 * replace; the CLEAN_ENT_CPS wrapper hands the fn cpu->pc == the dispatched
 * addr on entry and converts an untouched cpu->pc back to $ra on return.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

/* ======== popup_field_update (guest 0x8013AB40) ========
 * br2eu_func_8013AB40 — 3-piece CPS coroutine (entry + continuation addrs
 * 0x8013AB48/50/64 inside the entry def, split pieces 0x8013AB68/7C).
 * Halfword field updates on the s0 record:
 *   sh v1, 0x14(s0)                 0x8013AB44 (delay slot, always)
 *   if (v0 != 0) sh 160, 0x18(s0)   via block 0x8013AB48
 *   h = lh 0x0C(s0); if (h < 15) sh 15, 0x0C(s0)   (delay: v0 = 15)
 *   sh (lhu 0x06(s0) + lhu 0x08(s0)), 0x06(s0)
 * epilogue: restore ra/s0, sp += 32, jr ra.
 * s0/ra/sp frame is owned by the caller-side prologue outside this cluster;
 * only the epilogue ops shown here are guest-faithful. */
uint32_t popup_field_update_cps(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;
    uint32_t pc = cpu->pc;
    if (pc != 0u && pc != 0x8013ab40u) {
        switch (pc) {
        case 0x8013ab48u: goto b48;
        case 0x8013ab50u: goto b50;
        case 0x8013ab64u: goto b64;
        case 0x8013ab68u: goto b68;
        case 0x8013ab7cu: goto b7c;
        default: break;
        }
    }

    /* block_8013AB40 — cond = (v0 == 0); delay: sh v1, 0x14(s0) */
    {
        int cond = (cpu->gpr[2] == cpu->gpr[0]);
        cpu->write_half(cpu->gpr[16] + 20u, (uint16_t)cpu->gpr[3]);
        if (cond) {
            psx_check_interrupts_at(cpu, 0x8013ab50u);
            goto b50;
        } else {
            psx_check_interrupts_at(cpu, 0x8013ab48u);
            goto b48;
        }
    }

b48:
    cpu->gpr[2] = 160u;
    cpu->write_half(cpu->gpr[16] + 24u, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8013AB50 */

b50:
    /* h = lh 0x0C(s0); cond = !(h < 15); delay: v0 = 15 */
    {
        int32_t h = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 12u);
        int cond = !(h < 15);
        cpu->gpr[2] = 15u;
        if (cond) {
            psx_check_interrupts_at(cpu, 0x8013ab68u);
            goto b68;
        } else {
            psx_check_interrupts_at(cpu, 0x8013ab64u);
            goto b64;
        }
    }

b64:
    cpu->write_half(cpu->gpr[16] + 12u, (uint16_t)cpu->gpr[2]);
    /* cps_ edge: cpu->pc = 0x8013ab68; return — intra-function, goto */

b68:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 6u);   /* lhu 0x06(s0) */
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 8u);   /* lhu 0x08(s0) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];           /* v0 += v1 */
    cpu->write_half(cpu->gpr[16] + 6u, (uint16_t)cpu->gpr[2]);
    /* cps_ edge: cpu->pc = 0x8013ab7c; return — intra-function, goto */

b7c:
    /* epilogue: lw ra,0x1C(sp); lw s0,0x18(sp); delay: addiu sp,sp,0x20 */
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 28u);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 24u);
    {
        uint32_t jt = cpu->gpr[31];
        cpu->gpr[29] = cpu->gpr[29] + 32u;
        psx_check_interrupts_at(cpu, jt);
        cpu->pc = jt;               /* jr ra — explicit so a stale
                                       continuation pc can't survive */
    }
    return cpu->gpr[2];
}
