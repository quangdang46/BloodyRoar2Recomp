/* w3_e03.c — clean CPS ports of announce/bookkeeping piece-entries
 * (guest 0x80151074 / 0x80151300 / 0x80151494-0x80151968,
 * SLES_017.22_full_16.c + SLES_017.22_full_17.c).
 *
 * All eleven task addrs are CPS piece-entries of three overlapping-alias
 * bodies whose parent functions already have readable clean ports
 * (ko_count_announce / confirm_bookkeeping / hud_announce). Each guest
 * alias entry jumps into the shared body mid-function, skipping the
 * a0/a1 prologue, so they cannot reuse the parent's C signature — they
 * are dispatched through game/clean_cps.map (CLEAN_ENT_CPS wrappers that
 * hand the fn cpu->pc == the dispatched addr), exactly like the
 * cps_clean.c / gfx_emit_p03.c bodies. Each body below is a faithful
 * op-by-op transcription of the generated br2eu_func_* body: same guest
 * ops, same order, same yield/continuation points, minus cycle/icache/
 * cosim/debug instrumentation. The pc-switch covers every block leader
 * so IRQ resumes (psx_check_interrupts_at) re-enter at the right block.
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

/* ======== 0x80151074 — ko_count_announce_cps ========
 * Clean CPS twin of ko_count_announce (guest 0x80150F04, menu.c) covering
 * the overlapping-alias piece-entries of the KO/count announce body
 * (SLES_017.22_full_16.c, 568 bytes / 27 blocks). Guest alias entries
 * 0x80150F50/0x80151014/0x80151074/0x801510C8 all enter this shared body
 * mid-function (per-state layout tails that skip the a0/a1 prologue);
 * this port is dispatched at 0x80151074 plus every continuation/branch
 * block leader so IRQ resumes and CPS splits stay in clean C. The parent
 * entry 0x80150F04 keeps its own readable port (ko_count_announce) and
 * is intentionally NOT a case here. Faithful op-by-op transliteration of
 * generated br2eu_func_80150F04 / br2eu_psx_alias_body_80150F04. */
void ko_count_announce_cps_e03(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801510E0u: goto block_801510E0;
            case 0x80151128u: goto block_80151128;
            case 0x80150F30u: goto block_80150F30;
            case 0x80150F50u: goto block_80150F50;
            case 0x80150F5Cu: goto block_80150F5C;
            case 0x80150F80u: goto block_80150F80;
            case 0x80150F84u: goto block_80150F84;
            case 0x80150FA0u: goto block_80150FA0;
            case 0x80150FBCu: goto block_80150FBC;
            case 0x80150FE4u: goto block_80150FE4;
            case 0x80150FE8u: goto block_80150FE8;
            case 0x80151004u: goto block_80151004;
            case 0x80151014u: goto block_80151014;
            case 0x80151030u: goto block_80151030;
            case 0x80151054u: goto block_80151054;
            case 0x80151058u: goto block_80151058;
            case 0x80151074u: goto block_80151074;
            case 0x80151084u: goto block_80151084;
            case 0x801510ACu: goto block_801510AC;
            case 0x801510B0u: goto block_801510B0;
            case 0x801510C8u: goto block_801510C8;
            case 0x801510CCu: goto block_801510CC;
            case 0x801510D8u: goto block_801510D8;
            case 0x80151100u: goto block_80151100;
            case 0x80151104u: goto block_80151104;
            case 0x80151120u: goto block_80151120;
            default: break;
        }
    }
    goto block_80151074;  /* unmatched/stale cpu->pc -> this piece-entry's
     * own block (mirrors the generated alias switch(entry));
     * unreachable for clean_cps.map dispatches, all of which
     * are cases above. */

    /* Address: 0x80150F04, Size: 568 bytes, Blocks: 27 */

block_80150F04:
    sp = sp + -56;
    cpu->write_word(sp + 40, s0);
    s0 = a0;
    cpu->write_word(sp + 44, s1);
    cpu->write_word(sp + 48, ra);
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 162);
    v1 = cpu->read_byte(s0 + 156);
    a0 = v0 + 1;
    v0 = (v1 < (uint32_t)6) ? 1 : 0;  /* 0x80150F24: 0x2C620006 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s1 = a1;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80151128u);
        goto block_80151128;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150F30u);
    }

block_80150F30:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 1848;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80150F44: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80150F4C: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

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
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 56;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/* ======== 0x80151300 — confirm_bookkeeping_cps ========
 * Clean CPS twin of confirm_bookkeeping (guest 0x8015113C) covering the
 * two overlapping-alias piece-entries of the confirm/bookkeeping body
 * (SLES_017.22_full_17.c, 492 bytes / 22 blocks): 0x80151300 writes a
 * flag byte into the record then falls into the shared epilogue,
 * 0x8015130C is the bare epilogue entry. Dispatched at 0x80151300 plus
 * every continuation/branch block leader. Parent entry 0x8015113C keeps
 * its own clean port and is not a case here. Faithful to generated
 * br2eu_func_8015113C / br2eu_psx_alias_body_8015113C. */
void confirm_bookkeeping_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80151170u: goto block_80151170;
            case 0x80151180u: goto block_80151180;
            case 0x80151194u: goto block_80151194;
            case 0x8015119Cu: goto block_8015119C;
            case 0x801511A4u: goto block_801511A4;
            case 0x801511ACu: goto block_801511AC;
            case 0x801511D4u: goto block_801511D4;
            case 0x801511DCu: goto block_801511DC;
            case 0x80151200u: goto block_80151200;
            case 0x80151220u: goto block_80151220;
            case 0x80151258u: goto block_80151258;
            case 0x80151268u: goto block_80151268;
            case 0x80151270u: goto block_80151270;
            case 0x8015127Cu: goto block_8015127C;
            case 0x80151298u: goto block_80151298;
            case 0x801512ACu: goto block_801512AC;
            case 0x801512BCu: goto block_801512BC;
            case 0x801512D0u: goto block_801512D0;
            case 0x801512E4u: goto block_801512E4;
            case 0x80151300u: goto block_80151300;
            case 0x8015130Cu: goto block_8015130C;
            default: break;
        }
    }
    goto block_80151300;  /* unmatched/stale cpu->pc -> this piece-entry's
     * own block (mirrors the generated alias switch(entry));
     * unreachable for clean_cps.map dispatches, all of which
     * are cases above. */

    /* Address: 0x8015113C, Size: 492 bytes, Blocks: 22 */

block_8015113C:
    sp = sp + -40;
    cpu->write_word(sp + 16, s0);
    s0 = a0;
    cpu->write_word(sp + 24, s2);
    s2 = 0x1F80 << 16;  /* 0x1F800000 */
    s2 = s2 | 0x00E8;
    cpu->write_word(sp + 28, s3);
    s3 = 0x1F80 << 16;  /* 0x1F800000 */
    s3 = s3 | 0x01F0;
    a1 = zero;
    cpu->write_word(sp + 32, ra);
    ra = 0x80151170u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x80153484u);
    cpu->pc = 0x80153484u; return;  /* CPS jal -> 0x80153484 */

block_80151170:
    a0 = s0;
    a1 = 1;
    ra = 0x80151180u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s1 = v0;
    psx_check_interrupts_at(cpu, 0x80153484u);
    cpu->pc = 0x80153484u; return;  /* CPS jal -> 0x80153484 */

block_80151180:
    v1 = cpu->read_byte(s0 + 155);
    a0 = v0;
    v0 = ((int32_t)v1 < 5) ? 1 : 0;  /* 0x80151188: 0x28620005 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80151190: 0x00000000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80151270u);
        goto block_80151270;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151194u);
    }

block_80151194:;
    int cond_v1_lt_0 = ((int32_t)v1 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80151198: 0x00000000 */
    if (cond_v1_lt_0) {
        psx_check_interrupts_at(cpu, 0x80151270u);
        goto block_80151270;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015119Cu);
    }

block_8015119C:;
    int cond_s1_ne_zero = (s1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801511A0: 0x00000000 */
    if (cond_s1_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801511D4u);
        goto block_801511D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801511A4u);
    }

block_801511A4:;
    int cond_a0_eq_zero = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801511A8: 0x00000000 */
    if (cond_a0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80151200u);
        goto block_80151200;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801511ACu);
    }

block_801511AC:
    cpu->write_byte(s2 + 1, (uint8_t)zero);
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x801511B4: 0x00000000 */
    v0 = cpu->read_byte(v0 + 20);
    /* nop */  /* 0x801511BC: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_byte(s3 + 1, (uint8_t)v0);
    v1 = cpu->read_word(s0 + 192);
    /* delay slot (always executes) */
    v0 = 256;
    psx_check_interrupts_at(cpu, 0x80151258u);
    goto block_80151258;  /* j */

block_801511D4:;
    int cond_a0_ne_zero = (a0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801511D8: 0x00000000 */
    if (cond_a0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80151220u);
        goto block_80151220;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801511DCu);
    }

block_801511DC:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x801511E0: 0x00000000 */
    v0 = cpu->read_byte(v0 + 19);
    cpu->write_byte(s3 + 1, (uint8_t)zero);
    v0 = v0 + 1;
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    v1 = cpu->read_word(s0 + 192);
    /* delay slot (always executes) */
    v0 = 512;
    psx_check_interrupts_at(cpu, 0x80151258u);
    goto block_80151258;  /* j */

block_80151200:
    cpu->write_byte(s2 + 1, (uint8_t)zero);
    cpu->write_byte(s3 + 1, (uint8_t)zero);
    v1 = cpu->read_word(s0 + 192);
    v0 = 768;
    cpu->write_half(s0 + 146, (uint16_t)v0);
    v0 = cpu->read_byte(v1 + 5);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x80151268u);
    goto block_80151268;  /* j */

block_80151220:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x80151224: 0x00000000 */
    v0 = cpu->read_byte(v0 + 19);
    /* nop */  /* 0x8015122C: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x8015123C: 0x00000000 */
    v0 = cpu->read_byte(v0 + 20);
    /* nop */  /* 0x80151244: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_byte(s3 + 1, (uint8_t)v0);
    v1 = cpu->read_word(s0 + 192);
    v0 = 1024;
    /* fall through to block_80151258 */

block_80151258:
    cpu->write_half(s0 + 146, (uint16_t)v0);
    v0 = cpu->read_byte(v1 + 6);
    /* nop */  /* 0x80151260: 0x00000000 */
    v0 = v0 + 1;
    /* fall through to block_80151268 */

block_80151268:
    cpu->write_byte(v1 + 4, (uint8_t)v0);
    v1 = cpu->read_byte(s0 + 155);
    /* fall through to block_80151270 */

block_80151270:
    v0 = 4;
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80151278: 0x00000000 */
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x801512D0u);
        goto block_801512D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015127Cu);
    }

block_8015127C:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x80151280: 0x00000000 */
    cpu->write_byte(v0 + 4, (uint8_t)zero);
    v0 = cpu->read_byte(s2 + 1);
    /* nop */  /* 0x8015128C: 0x00000000 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80151294: 0x00000000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x801512ACu);
        goto block_801512AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151298u);
    }

block_80151298:
    v0 = cpu->read_byte(s0 + 212);
    /* nop */  /* 0x8015129C: 0x00000000 */
    v0 = v0 | 0x0040;
    v0 = v0 + 1;
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    /* fall through to block_801512AC */

block_801512AC:
    v0 = cpu->read_byte(s3 + 1);
    /* nop */  /* 0x801512B0: 0x00000000 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801512B8: 0x00000000 */
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x801512D0u);
        goto block_801512D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801512BCu);
    }

block_801512BC:
    v0 = cpu->read_byte(s0 + 212);
    /* nop */  /* 0x801512C0: 0x00000000 */
    v0 = v0 | 0x0040;
    v0 = v0 + 1;
    cpu->write_byte(s3 + 1, (uint8_t)v0);
    /* fall through to block_801512D0 */

block_801512D0:
    v1 = cpu->read_byte(s0 + 156);
    /* nop */  /* 0x801512D4: 0x00000000 */
    v0 = (v1 < (uint32_t)6) ? 1 : 0;  /* 0x801512D8: 0x2C620006 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x8015130Cu);
        goto block_8015130C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801512E4u);
    }

block_801512E4:
    v0 = v0 + 1872;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x801512F4: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801512FC: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

block_80151300:
    v1 = cpu->read_word(s0 + 192);
    v0 = 1;
    cpu->write_byte(v1 + 4, (uint8_t)v0);
    /* fall through to block_8015130C */

block_8015130C:
    ra = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}