/* w3_e02.c — clean CPS ports of shared psx_alias_body regions
 * (worktree orch/w3/e02).
 *
 * The recompiler merged several guest entry points into one CPS body per
 * region and emits thin `br2eu_func_<entry>` trampolines that call
 * `br2eu_psx_alias_body_<base>(cpu, entry)`. The bodies below are faithful
 * op-by-op transliterations of those alias bodies (same transform as
 * game/cps_clean.c: guest ops/order/yield points preserved, cycle/icache/
 * cosim/debug instrumentation dropped, MMIO-aware cpu->read/write accessors).
 *
 * Dispatch: the 11 task addrs are registered in game/clean_cps.map to the
 * named wrappers below; every CPS continuation/branch-target addr of each
 * body is registered to the family *_cps symbol so coroutine yields resume
 * inside clean C. Each wrapper hands the body cpu->pc == the dispatched addr
 * (or an explicit entry for fresh entries) so the body's _cont/entry
 * switches pick the right block — identical to the generated contract.
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
/* ======== ko_count_announce_body — clean CPS transliteration of br2eu_psx_alias_body_80150F04
 * (shared alias body; entry + ko_count_announce_cps continuation blocks). */
static void ko_count_announce_body(CPUState *cpu, uint32_t entry)
{

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801510E0u: goto block_801510E0;
            case 0x80151128u: goto block_80151128;
            case 0x80150F5Cu: goto block_80150F5C;
            case 0x80150F80u: goto block_80150F80;
            case 0x80150F84u: goto block_80150F84;
            case 0x80150FA0u: goto block_80150FA0;
            case 0x80150FBCu: goto block_80150FBC;
            case 0x80150FE4u: goto block_80150FE4;
            case 0x80150FE8u: goto block_80150FE8;
            case 0x80151004u: goto block_80151004;
            case 0x80151030u: goto block_80151030;
            case 0x80151054u: goto block_80151054;
            case 0x80151058u: goto block_80151058;
            case 0x80151084u: goto block_80151084;
            case 0x801510ACu: goto block_801510AC;
            case 0x801510B0u: goto block_801510B0;
            case 0x801510CCu: goto block_801510CC;
            case 0x801510D8u: goto block_801510D8;
            case 0x80151100u: goto block_80151100;
            case 0x80151104u: goto block_80151104;
            case 0x80151120u: goto block_80151120;
            default: break;
        }
    }
    switch (entry) {
        case 0x80150F50u: goto block_80150F50;
        case 0x80151014u: goto block_80151014;
        case 0x80151074u: goto block_80151074;
        case 0x801510C8u: goto block_801510C8;
        default: return;
    }

block_80150F50:
    v0 = 9;
    int cond_a0_ne_v0 = (a0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_a0_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80150FA0u);
        goto block_80150FA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150F5Cu);
    }

block_80150F5C:
    a2 = v0 + 1772;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s0 + 34);
    v1 = v1 + -176;
    v0 = (uint32_t)v1 >> 31;
    v1 = v1 + v0;
    v1 = v1 << 15;
    int cond_a1_ge_0 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (cond_a1_ge_0) {
        psx_check_interrupts_at(cpu, 0x80150F84u);
        goto block_80150F84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150F80u);
    }

block_80150F80:
    a1 = a1 + 15;
    /* fall through to block_80150F84 */

block_80150F84:
    v0 = (int32_t)a1 >> 4;
    cpu->write_word(sp + 16, v0);
    v0 = 4;
    cpu->write_word(sp + 20, v0);
    a1 = cpu->read_word(s0 + 12);
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x80151120u);
    goto block_80151120;  /* j */

block_80150FA0:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x80150FA4: 0x00000000 */
    v0 = cpu->read_byte(v0 + 16);
    /* nop */  /* 0x80150FAC: 0x00000000 */
    v0 = ((int32_t)a0 < (int32_t)v0) ? 1 : 0;  /* 0x80150FB0: 0x0082102A */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80151004u);
        goto block_80151004;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150FBCu);
    }

block_80150FBC:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    a2 = v0 + 1788;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s0 + 34);
    v1 = v1 + -144;
    v0 = (uint32_t)v1 >> 31;
    v1 = v1 + v0;
    v1 = v1 << 15;
    int cond_a1_ge_0_2 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (cond_a1_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x80150FE8u);
        goto block_80150FE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150FE4u);
    }

block_80150FE4:
    a1 = a1 + 15;
    /* fall through to block_80150FE8 */

block_80150FE8:
    v0 = (int32_t)a1 >> 4;
    cpu->write_word(sp + 16, v0);
    v0 = 4;
    cpu->write_word(sp + 20, v0);
    a1 = cpu->read_word(s0 + 12);
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x80151120u);
    goto block_80151120;  /* j */

block_80151004:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a2 = (int32_t)(int16_t)cpu->read_half(s0 + 162);
    /* delay slot (always executes) */
    a1 = a1 + 1800;
    psx_check_interrupts_at(cpu, 0x801510D8u);
    goto block_801510D8;  /* j */

block_80151014:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x80151018: 0x00000000 */
    v0 = cpu->read_byte(v0 + 16);
    /* nop */  /* 0x80151020: 0x00000000 */
    v0 = ((int32_t)a0 < (int32_t)v0) ? 1 : 0;  /* 0x80151024: 0x0082102A */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80151074u);
        goto block_80151074;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151030u);
    }

block_80151030:
    a2 = v0 + 1812;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s0 + 34);
    v1 = v1 + -144;
    v0 = (uint32_t)v1 >> 31;
    v1 = v1 + v0;
    v1 = v1 << 15;
    int cond_a1_ge_0_3 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (cond_a1_ge_0_3) {
        psx_check_interrupts_at(cpu, 0x80151058u);
        goto block_80151058;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151054u);
    }

block_80151054:
    a1 = a1 + 15;
    /* fall through to block_80151058 */

block_80151058:
    v0 = (int32_t)a1 >> 4;
    cpu->write_word(sp + 16, v0);
    v0 = 4;
    cpu->write_word(sp + 20, v0);
    a1 = cpu->read_word(s0 + 12);
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x80151120u);
    goto block_80151120;  /* j */

block_80151074:
    t0 = cpu->read_byte(s0 + 155);
    v0 = 4;
    int cond_t0_ne_v0 = (t0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    if (cond_t0_ne_v0) {
        psx_check_interrupts_at(cpu, 0x801510CCu);
        goto block_801510CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151084u);
    }

block_80151084:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    a2 = v0 + 1824;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s0 + 34);
    v1 = v1 + -112;
    v0 = (uint32_t)v1 >> 31;
    v1 = v1 + v0;
    v1 = v1 << 15;
    int cond_a1_ge_0_4 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (cond_a1_ge_0_4) {
        psx_check_interrupts_at(cpu, 0x801510B0u);
        goto block_801510B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801510ACu);
    }

block_801510AC:
    a1 = a1 + 15;
    /* fall through to block_801510B0 */

block_801510B0:
    v0 = (int32_t)a1 >> 4;
    cpu->write_word(sp + 16, v0);
    cpu->write_word(sp + 20, t0);
    a1 = cpu->read_word(s0 + 12);
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x80151120u);
    goto block_80151120;  /* j */

block_801510C8:
    a0 = sp + 24;
    /* fall through to block_801510CC */

block_801510CC:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a2 = (int32_t)(int16_t)cpu->read_half(s0 + 162);
    a1 = a1 + 1836;
    /* fall through to block_801510D8 */

block_801510D8:
    ra = 0x801510E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a2 + 1;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_801510E0:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s0 + 34);
    v0 = v0 + -96;
    v1 = (uint32_t)v0 >> 31;
    v0 = v0 + v1;
    v0 = v0 << 15;
    int cond_a1_ge_0_5 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v0 >> 16;
    if (cond_a1_ge_0_5) {
        psx_check_interrupts_at(cpu, 0x80151104u);
        goto block_80151104;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151100u);
    }

block_80151100:
    a1 = a1 + 15;
    /* fall through to block_80151104 */

block_80151104:
    a0 = s1;
    v0 = (int32_t)a1 >> 4;
    cpu->write_word(sp + 16, v0);
    v0 = 4;
    cpu->write_word(sp + 20, v0);
    a1 = cpu->read_word(s0 + 12);
    a2 = sp + 24;
    /* fall through to block_80151120 */

block_80151120:
    ra = 0x80151128u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80151124: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80151128:
    ra = cpu->read_word(sp + 48);
    s1 = cpu->read_word(sp + 44);
    s0 = cpu->read_word(sp + 40);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 56;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}
/* ko_count_announce_cps — CPS re-entry symbol for every continuation/branch-target addr of
 * the ko_count_announce_body region (game/clean_cps.map). cpu->pc == dispatched addr. */
void ko_count_announce_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  ko_count_announce_body(cpu, cpu->pc);
}
/* 0x80151014 — alias piece-entry into ko_count_announce_body (see game/renames.map). */
void ko_count_announce_alt_1014(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  ko_count_announce_body(cpu, 0x80151014u);
}
