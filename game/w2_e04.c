/* w2_e04.c — clean CPS ports of ScriptVM bra/bsr piece-entries
 * (guest 0x801831E0 + 0x801832F0-0x80183418, SLES_017.22_full_38.c).
 *
 * All eight addrs are CPS split-piece entries / IRQ-resume PCs of the
 * scriptop_bra (guest 0x80182F48) and scriptop_bsr (guest 0x80183248)
 * opcode handlers — reached via `cpu->pc = <addr>` coroutine dispatches,
 * NOT ordinary `jr $ra` function entries. They are therefore routed
 * through game/clean_cps.map (CLEAN_ENT_CPS wrappers that hand the fn
 * cpu->pc == the dispatched addr), exactly like the cps_clean.c bodies.
 * Each body below is a faithful op-by-op transcription of the generated
 * br2eu_func_* body: same guest ops, same order, same yield/continuation
 * points, minus cycle/icache/cosim/debug instrumentation. The primary
 * entries 0x80182F48/0x80183248 stay on the hand-ported scriptop_bra /
 * scriptop_bsr in game/vm.c — these transliterations cover the piece
 * entries and their shared continuation blocks only.
 */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
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

/* ======== scriptop_bra_cps — ScriptVM BRA opcode CPS body ========
 * Faithful CPS transliteration of generated br2eu_func_80182F48
 * (guest scriptop_bra, 0x80182F48, 768 bytes / 40 blocks). Covers the
 * piece-entry 0x801831E0 plus every continuation/resume block of the
 * body (the _cont switch also lists sibling piece entries owned by
 * other ports — they simply never dispatch here). Primary entry
 * 0x80182F48 remains routed to game/vm.c scriptop_bra. */
void scriptop_bra_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182FB8u: goto block_80182FB8;
            case 0x80182F80u: goto block_80182F80;
            case 0x80182F9Cu: goto block_80182F9C;
            case 0x80182FC8u: goto block_80182FC8;
            case 0x80182FD4u: goto block_80182FD4;
            case 0x80182FF0u: goto block_80182FF0;
            case 0x80183008u: goto block_80183008;
            case 0x80183018u: goto block_80183018;
            case 0x80183024u: goto block_80183024;
            case 0x80183034u: goto block_80183034;
            case 0x80183044u: goto block_80183044;
            case 0x8018304Cu: goto block_8018304C;
            case 0x8018305Cu: goto block_8018305C;
            case 0x8018306Cu: goto block_8018306C;
            case 0x8018307Cu: goto block_8018307C;
            case 0x8018308Cu: goto block_8018308C;
            case 0x8018309Cu: goto block_8018309C;
            case 0x801830ACu: goto block_801830AC;
            case 0x801830BCu: goto block_801830BC;
            case 0x801830CCu: goto block_801830CC;
            case 0x801830DCu: goto block_801830DC;
            case 0x801830ECu: goto block_801830EC;
            case 0x801830FCu: goto block_801830FC;
            case 0x8018310Cu: goto block_8018310C;
            case 0x80183124u: goto block_80183124;
            case 0x80183128u: goto block_80183128;
            case 0x80183148u: goto block_80183148;
            case 0x80183188u: goto block_80183188;
            case 0x8018318Cu: goto block_8018318C;
            case 0x801831A4u: goto block_801831A4;
            case 0x801831C8u: goto block_801831C8;
            case 0x801831E0u: goto block_801831E0;
            case 0x801831F0u: goto block_801831F0;
            case 0x801831FCu: goto block_801831FC;
            case 0x80183208u: goto block_80183208;
            case 0x80183218u: goto block_80183218;
            case 0x80183224u: goto block_80183224;
            case 0x80183228u: goto block_80183228;
            case 0x8018322Cu: goto block_8018322C;
            default: break;
        }
    }
    /* Address: 0x80182F48, Size: 768 bytes, Blocks: 40 */

    /* scriptop_bra entry — s1=ctx(a2), s0=*(ctx+36)
     * script stream, s3=a0 owner obj. Fetch op halfword; ctx[110]
     * flag halfword selects path; ctx[56] = repeat/condition counter. */
block_80182F48:
    sp = sp + -72;
    cpu->write_word(sp + 52, s1);
    s1 = a2;
    cpu->write_word(sp + 64, ra);
    cpu->write_word(sp + 60, s3);
    cpu->write_word(sp + 56, s2);
    cpu->write_word(sp + 48, s0);
    s0 = cpu->read_word(s1 + 36);
    s3 = a0;
    v0 = cpu->read_half(s0);
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 110);
    /* nop */  /* 0x80182F74: 0x00000000 */
    int cond_v1_eq_zero = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 = (uint32_t)v0 >> 8;
    if (cond_v1_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80182FC8u);
        goto block_80182FC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182F80u);
        goto block_80182F80;  /* not taken */
    }

block_80182F80:
    a0 = cpu->read_word(s1 + 120);
    /* nop */  /* 0x80182F84: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s1;
    v0 = ((int32_t)v0 < 196) ? 1 : 0;  /* 0x80182F90: 0x284200C4 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80182FC8u);
        goto block_80182FC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182F9Cu);
        goto block_80182F9C;  /* not taken */
    }

block_80182F9C:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 21376;
    v1 = s2 << 2;
    v1 = v1 + v0;
    a2 = cpu->read_word(v1);
    ra = 0x80182FB8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 12560;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80182FB8:
    v1 = cpu->read_word(s1 + 120);
    /* nop */  /* 0x80182FBC: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s1 + 120, v1);
    /* fall through to block_80182FC8 */

block_80182FC8:
    v0 = (s2 < (uint32_t)12) ? 1 : 0;  /* 0x80182FC8: 0x2E42000C */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182FF0u);
        goto block_80182FF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182FD4u);
        goto block_80182FD4;  /* not taken */
    }

block_80182FD4:
    v0 = v0 + 13028;
    v1 = s2 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80182FE4: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182FEC: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

    /* scriptop_bra_pc_commit — ctx[36] = halfword(s0+2) +
     * obj[48]: commit branch target = operand + stream base. */
block_80182FF0:
    v1 = cpu->read_half(s0 + 2);
    a0 = cpu->read_word(s3 + 48);
    v0 = 1;
    v1 = v1 + a0;
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v1);
    psx_check_interrupts_at(cpu, 0x8018322Cu);
    goto block_8018322C;  /* j */

    /* scriptop_bra_subops — conditional variants: ctx[56]<=0
     * picks operand s0+6 and clears ctx[56], else operand s0+2;
     * each arm commits ctx[36] then joins epilogue. */
block_80183008:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x8018300C: 0x00000000 */
    int cond_v0_le_0 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183014: 0x00000000 */
    if (cond_v0_le_0) {
        psx_check_interrupts_at(cpu, 0x80183024u);
        goto block_80183024;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183018u);
        goto block_80183018;  /* not taken */
    }

block_80183018:
    v0 = cpu->read_half(s0 + 2);
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183034u);
    goto block_80183034;  /* j */

block_80183024:
    cpu->write_word(s1 + 56, zero);
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x8018302C: 0x00000000 */
    cpu->write_word(s1 + 36, v0);
    /* fall through to block_80183034 */

block_80183034:
    v1 = cpu->read_word(s1 + 36);
    /* nop */  /* 0x80183038: 0x00000000 */
    int cond_v1_ne_zero = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 8;
    if (cond_v1_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183218u);
        goto block_80183218;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183044u);
        goto block_80183044;  /* not taken */
    }

block_80183044:
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183228u);
    goto block_80183228;  /* j */

block_8018304C:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x80183050: 0x00000000 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018305Cu);
        goto block_8018305C;  /* not taken */
    }

block_8018305C:
    v0 = cpu->read_half(s0 + 2);
    v1 = cpu->read_word(s3 + 48);
    /* delay slot (always executes) */
    v0 = v1 + v0;
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_8018306C:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x80183070: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018307Cu);
        goto block_8018307C;  /* not taken */
    }

block_8018307C:
    v0 = cpu->read_half(s0 + 2);
    v1 = cpu->read_word(s3 + 48);
    /* delay slot (always executes) */
    v0 = v1 + v0;
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_8018308C:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x80183090: 0x00000000 */
    int cond_v0_le_0_2 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_le_0_2) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018309Cu);
        goto block_8018309C;  /* not taken */
    }

block_8018309C:
    v0 = cpu->read_half(s0 + 2);
    v1 = cpu->read_word(s3 + 48);
    /* delay slot (always executes) */
    v0 = v1 + v0;
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_801830AC:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x801830B0: 0x00000000 */
    int cond_v0_lt_0 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_lt_0) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801830BCu);
        goto block_801830BC;  /* not taken */
    }

block_801830BC:
    v0 = cpu->read_half(s0 + 2);
    v1 = cpu->read_word(s3 + 48);
    /* delay slot (always executes) */
    v0 = v1 + v0;
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_801830CC:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x801830D0: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801830DCu);
        goto block_801830DC;  /* not taken */
    }

block_801830DC:
    v0 = cpu->read_half(s0 + 2);
    v1 = cpu->read_word(s3 + 48);
    /* delay slot (always executes) */
    v0 = v1 + v0;
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_801830EC:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x801830F0: 0x00000000 */
    int cond_v0_gt_0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_gt_0) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801830FCu);
        goto block_801830FC;  /* not taken */
    }

block_801830FC:
    v0 = cpu->read_half(s0 + 2);
    v1 = cpu->read_word(s3 + 48);
    /* delay slot (always executes) */
    v0 = v1 + v0;
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_8018310C:
    a1 = s0 + 3;
    a2 = zero;
    a3 = cpu->read_byte(s0 + 2);
    /* nop */  /* 0x80183118: 0x00000000 */
    int cond_a3_eq_zero = (a3 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a2;
    if (cond_a3_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80183148u);
        goto block_80183148;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183124u);
        goto block_80183124;  /* not taken */
    }

block_80183124:
    a0 = sp + 16;
    /* fall through to block_80183128 */

block_80183128:
    v0 = cpu->read_byte(a1);
    a1 = a1 + 1;
    v1 = v1 + 1;
    a2 = v0 + a2;
    cpu->write_half(a0, (uint16_t)v0);
    v0 = ((int32_t)v1 < (int32_t)a3) ? 1 : 0;  /* 0x8018313C: 0x0067102A */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 2;
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80183128u);
        goto block_80183128;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183148u);
        goto block_80183148;  /* not taken */
    }

block_80183148:
    v1 = cpu->read_byte(s1 + 4);
    /* nop */  /* 0x8018314C: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s1 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s1);
    v1 = v1 & 0x00FF;  /* 0x8018315C: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80183168: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a2; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    v0 = cpu->read_half(sp + 16);
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t0 >> 8;
    a0 = a0 - v0;
    int cond_a0_lt_0 = ((int32_t)a0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_a0_lt_0) {
        psx_check_interrupts_at(cpu, 0x801831A4u);
        goto block_801831A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183188u);
        goto block_80183188;  /* not taken */
    }

block_80183188:
    a1 = sp + 16;
    /* fall through to block_8018318C */

block_8018318C:
    a1 = a1 + 2;
    v0 = cpu->read_half(a1);
    /* nop */  /* 0x80183194: 0x00000000 */
    a0 = a0 - v0;
    int cond_a0_ge_0 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 + 1;
    if (cond_a0_ge_0) {
        psx_check_interrupts_at(cpu, 0x8018318Cu);
        goto block_8018318C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801831A4u);
        goto block_801831A4;  /* not taken */
    }

block_801831A4:
    v0 = a3 + 4;
    v1 = v1 << 1;
    v0 = v0 & 0x01FE;  /* 0x801831AC: 0x304201FE */
    v0 = v0 + s0;
    v1 = v1 + v0;
    v1 = cpu->read_half(v1);
    /* nop */  /* 0x801831BC: 0x00000000 */
    int cond_v1_ne_zero_2 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v1);
    if (cond_v1_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80183218u);
        goto block_80183218;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801831C8u);
        goto block_801831C8;  /* not taken */
    }

block_801831C8:
    v0 = a3 << 1;
    v0 = v0 + a3;
    v0 = v0 + 4;
    v0 = v0 & 0x07FE;  /* 0x801831D4: 0x304207FE */
    /* delay slot (always executes) */
    v0 = s0 + v0;
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

    /* BRA piece-entry (CPS resume): ctx[56]==0 -> target
     * halfword(s0+4), else halfword(s0+2); NULL check on ctx[36]. */
block_801831E0:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x801831E4: 0x00000000 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801831EC: 0x00000000 */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x801831FCu);
        goto block_801831FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801831F0u);
        goto block_801831F0;  /* not taken */
    }

block_801831F0:
    v0 = cpu->read_half(s0 + 2);
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183208u);
    goto block_80183208;  /* j */

block_801831FC:
    v0 = cpu->read_half(s0 + 4);
    /* nop */  /* 0x80183200: 0x00000000 */
    cpu->write_word(s1 + 36, v0);
    /* fall through to block_80183208 */

block_80183208:
    v1 = cpu->read_word(s1 + 36);
    /* nop */  /* 0x8018320C: 0x00000000 */
    int cond_v1_eq_zero_2 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 6;
    if (cond_v1_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183218u);
        goto block_80183218;  /* not taken */
    }

block_80183218:
    v0 = cpu->read_word(s3 + 48);
    /* nop */  /* 0x8018321C: 0x00000000 */
    v0 = v1 + v0;
    /* fall through to block_80183224 */

block_80183224:
    cpu->write_word(s1 + 36, v0);
    /* fall through to block_80183228 */

block_80183228:
    v0 = 1;
    /* fall through to block_8018322C */

block_8018322C:
    ra = cpu->read_word(sp + 64);
    s3 = cpu->read_word(sp + 60);
    s2 = cpu->read_word(sp + 56);
    s1 = cpu->read_word(sp + 52);
    s0 = cpu->read_word(sp + 48);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 72;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/* ======== scriptop_bsr_cps — ScriptVM BSR opcode CPS body ========
 * Faithful CPS transliteration of generated br2eu_func_80183248
 * (guest scriptop_bsr, 0x80183248). Covers piece-entries
 * 0x801832F0/0x80183338/0x801833B8/0x801833D0/0x801833E8/0x80183400/
 * 0x80183418 plus their continuation/resume blocks (the _cont switch
 * also lists sibling piece entries 0x80183430/0x8018344C/0x80183514
 * owned by other ports). Primary entry 0x80183248 remains routed to
 * game/vm.c scriptop_bsr. */
void scriptop_bsr_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801832B8u: goto block_801832B8;
            case 0x80183280u: goto block_80183280;
            case 0x8018329Cu: goto block_8018329C;
            case 0x801832C8u: goto block_801832C8;
            case 0x801832D4u: goto block_801832D4;
            case 0x801832F0u: goto block_801832F0;
            case 0x80183308u: goto block_80183308;
            case 0x80183338u: goto block_80183338;
            case 0x80183348u: goto block_80183348;
            case 0x80183354u: goto block_80183354;
            case 0x80183364u: goto block_80183364;
            case 0x80183368u: goto block_80183368;
            case 0x80183378u: goto block_80183378;
            case 0x80183390u: goto block_80183390;
            case 0x80183398u: goto block_80183398;
            case 0x801833B8u: goto block_801833B8;
            case 0x801833C8u: goto block_801833C8;
            case 0x801833D0u: goto block_801833D0;
            case 0x801833E0u: goto block_801833E0;
            case 0x801833E8u: goto block_801833E8;
            case 0x801833F8u: goto block_801833F8;
            case 0x80183400u: goto block_80183400;
            case 0x80183410u: goto block_80183410;
            case 0x80183418u: goto block_80183418;
            case 0x80183428u: goto block_80183428;
            case 0x80183430u: goto block_80183430;
            case 0x80183440u: goto block_80183440;
            case 0x80183444u: goto block_80183444;
            case 0x8018344Cu: goto block_8018344C;
            case 0x80183474u: goto block_80183474;
            case 0x80183478u: goto block_80183478;
            case 0x80183498u: goto block_80183498;
            case 0x801834D8u: goto block_801834D8;
            case 0x801834DCu: goto block_801834DC;
            case 0x801834F4u: goto block_801834F4;
            case 0x80183514u: goto block_80183514;
            case 0x80183524u: goto block_80183524;
            case 0x80183530u: goto block_80183530;
            case 0x8018353Cu: goto block_8018353C;
            default: break;
        }
    }
    /* Address: 0x80183248, Size: 784 bytes, Blocks: 40 */

    /* scriptop_bsr entry — same ctx setup; s2 = op>>8
     * sub-op; stack bounds check: ctx[120]-124-ctx < 196 (call
     * stack lives inside ctx, 196-byte limit). */
block_80183248:
    sp = sp + -72;
    cpu->write_word(sp + 52, s1);
    s1 = a2;
    cpu->write_word(sp + 64, ra);
    cpu->write_word(sp + 60, s3);
    cpu->write_word(sp + 56, s2);
    cpu->write_word(sp + 48, s0);
    s0 = cpu->read_word(s1 + 36);
    s3 = a0;
    v0 = cpu->read_half(s0);
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 110);
    /* nop */  /* 0x80183274: 0x00000000 */
    int cond_v1_eq_zero = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 = (uint32_t)v0 >> 8;
    if (cond_v1_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801832C8u);
        goto block_801832C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183280u);
        goto block_80183280;  /* not taken */
    }

block_80183280:
    a0 = cpu->read_word(s1 + 120);
    /* nop */  /* 0x80183284: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s1;
    v0 = ((int32_t)v0 < 196) ? 1 : 0;  /* 0x80183290: 0x284200C4 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801832C8u);
        goto block_801832C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018329Cu);
        goto block_8018329C;  /* not taken */
    }

block_8018329C:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 21424;
    v1 = s2 << 2;
    v1 = v1 + v0;
    a2 = cpu->read_word(v1);
    ra = 0x801832B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 12560;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_801832B8:
    v1 = cpu->read_word(s1 + 120);
    /* nop */  /* 0x801832BC: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s1 + 120, v1);
    /* fall through to block_801832C8 */

block_801832C8:
    v0 = (s2 < (uint32_t)12) ? 1 : 0;  /* 0x801832C8: 0x2E42000C */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x801832F0u);
        goto block_801832F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801832D4u);
        goto block_801832D4;  /* not taken */
    }

block_801832D4:
    v0 = v0 + 13076;
    v1 = s2 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x801832E4: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801832EC: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

    /* BSR piece-entry — push return pc / select target
     * operand by sub-op. */
block_801832F0:
    v1 = cpu->read_half(s1 + 108);
    /* nop */  /* 0x801832F4: 0x00000000 */
    a0 = v1 & 0xFFFF;  /* 0x801832F8: 0x3064FFFF */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x801832FC: 0x2C820008 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x80183440u);
        goto block_80183440;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183308u);
        goto block_80183308;  /* not taken */
    }

block_80183308:
    v0 = v1 + 1;
    cpu->write_half(s1 + 108, (uint16_t)v0);
    v0 = a0 << 2;
    v0 = s1 + v0;
    v1 = s0 + 4;
    cpu->write_word(v0 + 76, v1);
    v1 = cpu->read_half(s0 + 2);
    a0 = cpu->read_word(s3 + 48);
    v0 = 1;
    v1 = v1 + a0;
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v1);
    psx_check_interrupts_at(cpu, 0x8018353Cu);
    goto block_8018353C;  /* j */

    /* BSR piece-entry — stack push + branch commit. */
block_80183338:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x8018333C: 0x00000000 */
    int cond_v0_le_0 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183344: 0x00000000 */
    if (cond_v0_le_0) {
        psx_check_interrupts_at(cpu, 0x80183354u);
        goto block_80183354;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183348u);
        goto block_80183348;  /* not taken */
    }

block_80183348:
    v0 = cpu->read_half(s0 + 2);
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183364u);
    goto block_80183364;  /* j */

block_80183354:
    cpu->write_word(s1 + 56, zero);
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x8018335C: 0x00000000 */
    cpu->write_word(s1 + 36, v0);
    /* fall through to block_80183364 */

block_80183364:
    t0 = s0 + 8;
    /* fall through to block_80183368 */

block_80183368:
    v0 = cpu->read_word(s1 + 36);
    /* nop */  /* 0x8018336C: 0x00000000 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183374: 0x00000000 */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x80183390u);
        goto block_80183390;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183378u);
        goto block_80183378;  /* not taken */
    }

block_80183378:
    v1 = cpu->read_half(s1 + 108);
    /* nop */  /* 0x8018337C: 0x00000000 */
    a0 = v1 & 0xFFFF;  /* 0x80183380: 0x3064FFFF */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x80183384: 0x2C820008 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + 1;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183398u);
        goto block_80183398;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183390u);
        goto block_80183390;  /* not taken */
    }

block_80183390:
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, t0);
    psx_check_interrupts_at(cpu, 0x80183444u);
    goto block_80183444;  /* j */

block_80183398:
    cpu->write_half(s1 + 108, (uint16_t)v0);
    v0 = a0 << 2;
    v0 = s1 + v0;
    cpu->write_word(v0 + 76, t0);
    v0 = cpu->read_word(s1 + 36);
    v1 = cpu->read_word(s3 + 48);
    /* delay slot (always executes) */
    v0 = v0 + v1;
    psx_check_interrupts_at(cpu, 0x80183440u);
    goto block_80183440;  /* j */

    /* BSR piece-entry — stack push + branch commit. */
block_801833B8:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x801833BC: 0x00000000 */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x801832F0u);
        goto block_801832F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801833C8u);
        goto block_801833C8;  /* not taken */
    }

block_801833C8:
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183444u);
    goto block_80183444;  /* j */

block_801833D0:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x801833D4: 0x00000000 */
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_eq_zero_5) {
        psx_check_interrupts_at(cpu, 0x801832F0u);
        goto block_801832F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801833E0u);
        goto block_801833E0;  /* not taken */
    }

block_801833E0:
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183444u);
    goto block_80183444;  /* j */

block_801833E8:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x801833EC: 0x00000000 */
    int cond_v0_gt_0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_gt_0) {
        psx_check_interrupts_at(cpu, 0x801832F0u);
        goto block_801832F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801833F8u);
        goto block_801833F8;  /* not taken */
    }

block_801833F8:
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183444u);
    goto block_80183444;  /* j */

block_80183400:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x80183404: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x801832F0u);
        goto block_801832F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183410u);
        goto block_80183410;  /* not taken */
    }

block_80183410:
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183444u);
    goto block_80183444;  /* j */

    /* BSR tail — epilogue / next-piece yield. */
block_80183418:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x8018341C: 0x00000000 */
    int cond_v0_lt_0 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_lt_0) {
        psx_check_interrupts_at(cpu, 0x801832F0u);
        goto block_801832F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183428u);
        goto block_80183428;  /* not taken */
    }

block_80183428:
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183444u);
    goto block_80183444;  /* j */

block_80183430:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x80183434: 0x00000000 */
    int cond_v0_le_0_2 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 + 4;
    if (cond_v0_le_0_2) {
        psx_check_interrupts_at(cpu, 0x801832F0u);
        goto block_801832F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183440u);
        goto block_80183440;  /* not taken */
    }

block_80183440:
    cpu->write_word(s1 + 36, v0);
    /* fall through to block_80183444 */

block_80183444:
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x8018353Cu);
    goto block_8018353C;  /* j */

block_8018344C:
    a1 = s0 + 3;
    a2 = zero;
    a3 = cpu->read_byte(s0 + 2);
    v1 = a2;
    v0 = a3 << 1;
    v0 = v0 + a3;
    v0 = v0 + 4;
    v0 = v0 & 0x07FE;  /* 0x80183468: 0x304207FE */
    int cond_a3_eq_zero = (a3 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = s0 + v0;
    if (cond_a3_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80183498u);
        goto block_80183498;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183474u);
        goto block_80183474;  /* not taken */
    }

block_80183474:
    a0 = sp + 16;
    /* fall through to block_80183478 */

block_80183478:
    v0 = cpu->read_byte(a1);
    a1 = a1 + 1;
    v1 = v1 + 1;
    a2 = v0 + a2;
    cpu->write_half(a0, (uint16_t)v0);
    v0 = ((int32_t)v1 < (int32_t)a3) ? 1 : 0;  /* 0x8018348C: 0x0067102A */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 2;
    if (cond_v0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x80183478u);
        goto block_80183478;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183498u);
        goto block_80183498;  /* not taken */
    }

block_80183498:
    v1 = cpu->read_byte(s1 + 4);
    /* nop */  /* 0x8018349C: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s1 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s1);
    v1 = v1 & 0x00FF;  /* 0x801834AC: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x801834B8: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a2; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    v0 = cpu->read_half(sp + 16);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 8;
    a0 = a0 - v0;
    int cond_a0_lt_0 = ((int32_t)a0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_a0_lt_0) {
        psx_check_interrupts_at(cpu, 0x801834F4u);
        goto block_801834F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801834D8u);
        goto block_801834D8;  /* not taken */
    }

block_801834D8:
    a1 = sp + 16;
    /* fall through to block_801834DC */

block_801834DC:
    a1 = a1 + 2;
    v0 = cpu->read_half(a1);
    /* nop */  /* 0x801834E4: 0x00000000 */
    a0 = a0 - v0;
    int cond_a0_ge_0 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 + 1;
    if (cond_a0_ge_0) {
        psx_check_interrupts_at(cpu, 0x801834DCu);
        goto block_801834DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801834F4u);
        goto block_801834F4;  /* not taken */
    }

block_801834F4:
    v0 = a3 + 4;
    v1 = v1 << 1;
    v0 = v0 & 0x01FE;  /* 0x801834FC: 0x304201FE */
    v0 = v0 + s0;
    v1 = v1 + v0;
    v0 = cpu->read_half(v1);
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183368u);
    goto block_80183368;  /* j */

block_80183514:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x80183518: 0x00000000 */
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = s0 + 6;
    if (cond_v0_eq_zero_6) {
        psx_check_interrupts_at(cpu, 0x80183530u);
        goto block_80183530;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183524u);
        goto block_80183524;  /* not taken */
    }

block_80183524:
    v0 = cpu->read_half(s0 + 2);
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183368u);
    goto block_80183368;  /* j */

block_80183530:
    v0 = cpu->read_half(s0 + 4);
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183368u);
    goto block_80183368;  /* j */

block_8018353C:
    ra = cpu->read_word(sp + 64);
    s3 = cpu->read_word(sp + 60);
    s2 = cpu->read_word(sp + 56);
    s1 = cpu->read_word(sp + 52);
    s0 = cpu->read_word(sp + 48);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 72;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}
