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

/* ======== cmd_input_parse_body — clean CPS transliteration of br2eu_psx_alias_body_801434D4
 * (shared alias body; entry + cmd_input_parse_cps continuation blocks). */
static void cmd_input_parse_body(CPUState *cpu, uint32_t entry)
{

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8014359Cu: goto block_8014359C;
            case 0x801435ACu: goto block_801435AC;
            case 0x801435B8u: goto block_801435B8;
            case 0x801435DCu: goto block_801435DC;
            case 0x8014360Cu: goto block_8014360C;
            case 0x80143630u: goto block_80143630;
            case 0x8014363Cu: goto block_8014363C;
            case 0x80143644u: goto block_80143644;
            case 0x80143650u: goto block_80143650;
            case 0x8014365Cu: goto block_8014365C;
            case 0x80143678u: goto block_80143678;
            case 0x8014368Cu: goto block_8014368C;
            case 0x801436A0u: goto block_801436A0;
            case 0x801436A8u: goto block_801436A8;
            case 0x801436B8u: goto block_801436B8;
            case 0x801436D0u: goto block_801436D0;
            case 0x801436E4u: goto block_801436E4;
            case 0x801436ECu: goto block_801436EC;
            case 0x80143700u: goto block_80143700;
            case 0x80143714u: goto block_80143714;
            case 0x80143724u: goto block_80143724;
            case 0x80143734u: goto block_80143734;
            case 0x80143748u: goto block_80143748;
            case 0x8014377Cu: goto block_8014377C;
            case 0x80143804u: goto block_80143804;
            case 0x8014380Cu: goto block_8014380C;
            case 0x80143820u: goto block_80143820;
            case 0x80143834u: goto block_80143834;
            case 0x80143838u: goto block_80143838;
            case 0x8014385Cu: goto block_8014385C;
            case 0x80143894u: goto block_80143894;
            case 0x801438C4u: goto block_801438C4;
            case 0x801438D4u: goto block_801438D4;
            case 0x801438E0u: goto block_801438E0;
            case 0x801438F0u: goto block_801438F0;
            case 0x801438FCu: goto block_801438FC;
            case 0x80143948u: goto block_80143948;
            case 0x80143950u: goto block_80143950;
            case 0x80143970u: goto block_80143970;
            case 0x801439C4u: goto block_801439C4;
            case 0x801439CCu: goto block_801439CC;
            case 0x801439D0u: goto block_801439D0;
            case 0x801439D8u: goto block_801439D8;
            case 0x801439E8u: goto block_801439E8;
            case 0x801439F0u: goto block_801439F0;
            case 0x80143A18u: goto block_80143A18;
            case 0x80143A28u: goto block_80143A28;
            case 0x80143A2Cu: goto block_80143A2C;
            case 0x80143A34u: goto block_80143A34;
            case 0x80143A48u: goto block_80143A48;
            case 0x80143A58u: goto block_80143A58;
            case 0x80143A64u: goto block_80143A64;
            case 0x80143AA4u: goto block_80143AA4;
            case 0x80143AA8u: goto block_80143AA8;
            case 0x80143AB4u: goto block_80143AB4;
            default: break;
        }
    }
    switch (entry) {
        case 0x801435F0u: goto block_801435F0;
        case 0x801435FCu: goto block_801435FC;
        case 0x801436C0u: goto block_801436C0;
        case 0x80143798u: goto block_80143798;
        case 0x801437A4u: goto block_801437A4;
        case 0x801437B0u: goto block_801437B0;
        case 0x801437BCu: goto block_801437BC;
        case 0x801437C8u: goto block_801437C8;
        case 0x801437D4u: goto block_801437D4;
        case 0x801437ECu: goto block_801437EC;
        case 0x801438A0u: goto block_801438A0;
        case 0x80143978u: goto block_80143978;
        default: return;
    }

block_8014359C:
    t3 = cpu->read_word(t7);
    /* nop */  /* 0x801435A0: 0x00000000 */
    int cond_t3_eq_zero = (t3 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801435A8: 0x00000000 */
    if (cond_t3_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801439E8u);
        goto block_801439E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801435ACu);
    }

block_801435AC:
    v0 = cpu->read_word(a0 + 48);
    /* nop */  /* 0x801435B0: 0x00000000 */
    t3 = t3 + v0;
    /* fall through to block_801435B8 */

block_801435B8:
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x801435BC: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    v1 = cpu->read_half(v0);
    /* nop */  /* 0x801435CC: 0x00000000 */
    v0 = (v1 < (uint32_t)5) ? 1 : 0;  /* 0x801435D0: 0x2C620005 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801435DCu);
    }

block_801435DC:
    v0 = v0 + s2;
    v0 = cpu->read_word(v0);
    /* nop */  /* 0x801435E4: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801435EC: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

block_801435F0:
    cpu->write_half(t0, (uint16_t)zero);
    /* delay slot (always executes) */
    t9 = 1;
    psx_check_interrupts_at(cpu, 0x801439CCu);
    goto block_801439CC;  /* j */

block_801435FC:
    v0 = cpu->read_word(a0 + 88);
    /* nop */  /* 0x80143600: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143608: 0x00000000 */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014360Cu);
    }

block_8014360C:
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x80143610: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    a3 = cpu->read_half(v0 + 2);
    /* nop */  /* 0x80143620: 0x00000000 */
    v0 = a3 & 0x4000;  /* 0x80143624: 0x30E24000 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x8014363Cu);
        goto block_8014363C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143630u);
    }

block_80143630:
    v0 = a3 & 0x0FFF;  /* 0x80143630: 0x30E20FFF */
    /* delay slot (always executes) */
    a3 = v0 + a1;
    psx_check_interrupts_at(cpu, 0x80143644u);
    goto block_80143644;  /* j */

block_8014363C:
    a1 = zero;
    a3 = 1;
    /* fall through to block_80143644 */

block_80143644:
    v0 = ((int32_t)a1 < (int32_t)a3) ? 1 : 0;  /* 0x80143644: 0x00A7102A */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014364C: 0x00000000 */
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143650u);
    }

block_80143650:
    t1 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x80143654: 0x00000000 */
    v0 = a1 + t1;
    /* fall through to block_8014365C */

block_8014365C:
    v0 = v0 << 1;
    v0 = v0 + t3;
    a2 = cpu->read_half(v0 + 2);
    /* nop */  /* 0x80143668: 0x00000000 */
    v0 = a2 & 0x1000;  /* 0x8014366C: 0x30C21000 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a2 & 0x0FFF;  /* 0x80143674: 0x30C20FFF */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x801436A0u);
        goto block_801436A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143678u);
    }

block_80143678:
    v1 = cpu->read_byte(t4 + 124);
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x80143680: 0x00000000 */
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143688: 0x00000000 */
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x801436A8u);
        goto block_801436A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014368Cu);
    }

block_8014368C:
    v0 = cpu->read_word(a0 + 92);
    /* nop */  /* 0x80143690: 0x00000000 */
    v0 = v0 & 0x0040;  /* 0x80143694: 0x30420040 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a2 & 0x0FFF;  /* 0x8014369C: 0x30C20FFF */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801436A8u);
        goto block_801436A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436A0u);
    }

block_801436A0:;
    int cond_t8_eq_v0 = (t8 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = -1;
    if (cond_t8_eq_v0) {
        psx_check_interrupts_at(cpu, 0x80143950u);
        goto block_80143950;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436A8u);
    }

block_801436A8:
    a1 = a1 + 1;
    v0 = ((int32_t)a1 < (int32_t)a3) ? 1 : 0;  /* 0x801436AC: 0x00A7102A */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a1 + t1;
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x8014365Cu);
        goto block_8014365C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436B8u);
    }

block_801436B8:
    /* delay slot (always executes) */
    cpu->write_half(t0, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x801439CCu);
    goto block_801439CC;  /* j */

block_801436C0:
    v0 = cpu->read_word(a0 + 88);
    /* nop */  /* 0x801436C4: 0x00000000 */
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801436CC: 0x00000000 */
    if (cond_v0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436D0u);
    }

block_801436D0:
    v1 = cpu->read_half(t2 + 2);
    /* nop */  /* 0x801436D4: 0x00000000 */
    v0 = v1 & 0x0FFF;  /* 0x801436D8: 0x30620FFF */
    int cond_v0_ne_t8 = (v0 != t8);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x1000;  /* 0x801436E0: 0x30621000 */
    if (cond_v0_ne_t8) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436E4u);
    }

block_801436E4:;
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801436E8: 0x00000000 */
    if (cond_v0_eq_zero_5) {
        psx_check_interrupts_at(cpu, 0x80143714u);
        goto block_80143714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436ECu);
    }

block_801436EC:
    v1 = cpu->read_byte(t4 + 124);
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x801436F4: 0x00000000 */
    int cond_v1_ne_v0_2 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801436FC: 0x00000000 */
    if (cond_v1_ne_v0_2) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143700u);
    }

block_80143700:
    v0 = cpu->read_word(a0 + 92);
    /* nop */  /* 0x80143704: 0x00000000 */
    v0 = v0 & 0x0040;  /* 0x80143708: 0x30420040 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143710: 0x00000000 */
    if (cond_v0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143714u);
    }

block_80143714:
    v1 = (int32_t)(int16_t)cpu->read_half(t2 + 4);
    /* nop */  /* 0x80143718: 0x00000000 */
    int cond_v1_lt_0 = ((int32_t)v1 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143720: 0x00000000 */
    if (cond_v1_lt_0) {
        psx_check_interrupts_at(cpu, 0x80143748u);
        goto block_80143748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143724u);
    }

block_80143724:
    v0 = (int32_t)(int8_t)cpu->read_byte(t4 + 126);
    /* nop */  /* 0x80143728: 0x00000000 */
    int cond_v0_ge_0_3 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143730: 0x00000000 */
    if (cond_v0_ge_0_3) {
        psx_check_interrupts_at(cpu, 0x80143748u);
        goto block_80143748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143734u);
    }

block_80143734:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 188);
    /* nop */  /* 0x80143738: 0x00000000 */
    v0 = ((int32_t)v1 < (int32_t)v0) ? 1 : 0;  /* 0x8014373C: 0x0062102A */
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143744: 0x00000000 */
    if (cond_v0_ne_zero_4) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143748u);
    }

block_80143748:
    v0 = cpu->read_word(a0 + 108);
    v1 = 0x0200 << 16;  /* 0x02000000 */
    v0 = v0 | v1;
    cpu->write_word(a0 + 108, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x8014375C: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    v1 = cpu->read_half(v0 + 4);
    /* nop */  /* 0x8014376C: 0x00000000 */
    v0 = (v1 < (uint32_t)8) ? 1 : 0;  /* 0x80143770: 0x2C620008 */
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_6) {
        psx_check_interrupts_at(cpu, 0x8014380Cu);
        goto block_8014380C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014377Cu);
    }

block_8014377C:
    v0 = v0 + 644;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x8014378C: 0x00000000 */
    uint32_t jr_target_2 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143794: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr v0 */

block_80143798:
    t5 = cpu->read_half(a0 + 124);
    /* delay slot (always executes) */
    /* nop */  /* 0x801437A0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014380Cu);
    goto block_8014380C;  /* j */

block_801437A4:
    t5 = cpu->read_half(a0 + 122);
    /* delay slot (always executes) */
    /* nop */  /* 0x801437AC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014380Cu);
    goto block_8014380C;  /* j */

block_801437B0:
    t5 = cpu->read_half(a0 + 128);
    /* delay slot (always executes) */
    /* nop */  /* 0x801437B8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014380Cu);
    goto block_8014380C;  /* j */

block_801437BC:
    t5 = cpu->read_half(a0 + 132);
    /* delay slot (always executes) */
    /* nop */  /* 0x801437C4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014380Cu);
    goto block_8014380C;  /* j */

block_801437C8:
    t5 = cpu->read_half(a0 + 130);
    /* delay slot (always executes) */
    /* nop */  /* 0x801437D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014380Cu);
    goto block_8014380C;  /* j */

block_801437D4:
    v1 = cpu->read_half(a0 + 124);
    v0 = cpu->read_half(a0 + 122);
    v1 = v1 & 0xF000;  /* 0x801437DC: 0x3063F000 */
    v0 = v0 & 0x00E8;  /* 0x801437E0: 0x304200E8 */
    /* delay slot (always executes) */
    t5 = v1 | v0;
    psx_check_interrupts_at(cpu, 0x8014380Cu);
    goto block_8014380C;  /* j */

block_801437EC:
    v0 = cpu->read_half(a0 + 122);
    t5 = cpu->read_half(a0 + 128);
    v1 = v0 & 0x00E8;  /* 0x801437F4: 0x304300E8 */
    v0 = 192;
    int cond_v1_eq_v0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 136;
    if (cond_v1_eq_v0) {
        psx_check_interrupts_at(cpu, 0x8014380Cu);
        goto block_8014380C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143804u);
    }

block_80143804:;
    int cond_v1_ne_v0_3 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143808: 0x00000000 */
    if (cond_v1_ne_v0_3) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014380Cu);
    }

block_8014380C:
    v0 = cpu->read_byte(a0 + 5);
    /* nop */  /* 0x80143810: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x80143814: 0x30420001 */
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014381C: 0x00000000 */
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x80143838u);
        goto block_80143838;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143820u);
    }

block_80143820:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 214);
    /* nop */  /* 0x80143824: 0x00000000 */
    v0 = ((int32_t)v0 < 7680) ? 1 : 0;  /* 0x80143828: 0x28421E00 */
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143830: 0x00000000 */
    if (cond_v0_eq_zero_7) {
        psx_check_interrupts_at(cpu, 0x80143838u);
        goto block_80143838;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143834u);
    }

block_80143834:
    t5 = t5 & 0xFFDF;  /* 0x80143834: 0x31ADFFDF */
    /* fall through to block_80143838 */

block_80143838:
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x8014383C: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    v1 = cpu->read_half(v0 + 6);
    v0 = cpu->read_half(v0 + 8);
    v1 = t5 & v1;  /* 0x80143850: 0x01A31824 */
    int cond_v1_ne_v0_4 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143858: 0x00000000 */
    if (cond_v1_ne_v0_4) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014385Cu);
    }

block_8014385C:
    v0 = cpu->read_half(t2);
    /* nop */  /* 0x80143860: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_half(t2, (uint16_t)v0);
    v1 = (int32_t)(int16_t)cpu->read_half(t0);
    v0 = v0 << 16;
    v1 = v1 << 1;
    v1 = v1 + t3;
    v1 = cpu->read_half(v1 + 2);
    v0 = (int32_t)v0 >> 16;
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x80143884: 0x0043102A */
    v1 = cpu->read_half(t0);
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + 5;
    if (cond_v0_ne_zero_6) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143894u);
    }

block_80143894:
    cpu->write_half(t0, (uint16_t)v0);
    /* delay slot (always executes) */
    cpu->write_half(t2, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x801435B8u);
    goto block_801435B8;  /* j */

block_801438A0:
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x801438A4: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    a2 = cpu->read_half(v0 + 2);
    /* nop */  /* 0x801438B4: 0x00000000 */
    v0 = a2 & 0x2000;  /* 0x801438B8: 0x30C22000 */
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t9 = zero;
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x801438FCu);
        goto block_801438FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438C4u);
    }

block_801438C4:
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x801438C8: 0x00000000 */
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = t9;
    if (cond_v0_eq_zero_9) {
        psx_check_interrupts_at(cpu, 0x801438FCu);
        goto block_801438FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438D4u);
    }

block_801438D4:
    t1 = a2 & 0x0FFF;  /* 0x801438D4: 0x30C90FFF */
    a3 = v0;
    v1 = t4;
    /* fall through to block_801438E0 */

block_801438E0:
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 60);
    /* nop */  /* 0x801438E4: 0x00000000 */
    int cond_t1_eq_v0 = (t1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 1;
    if (cond_t1_eq_v0) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438F0u);
    }

block_801438F0:
    v0 = ((int32_t)a1 < (int32_t)a3) ? 1 : 0;  /* 0x801438F0: 0x00A7102A */
    int cond_v0_ne_zero_7 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 + 2;
    if (cond_v0_ne_zero_7) {
        psx_check_interrupts_at(cpu, 0x801438E0u);
        goto block_801438E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438FCu);
    }

block_801438FC:
    v1 = a2 & 0x0FFF;  /* 0x801438FC: 0x30C30FFF */
    s1 = cpu->read_half(t2 + 6);
    v0 = -1;
    cpu->write_half(t2 + 2, (uint16_t)v1);
    cpu->write_half(t2 + 4, (uint16_t)v0);
    cpu->write_half(t2 + 6, (uint16_t)zero);
    v0 = cpu->read_half(t0);
    /* nop */  /* 0x80143918: 0x00000000 */
    v0 = v0 + 2;
    cpu->write_half(t0, (uint16_t)v0);
    cpu->write_half(t2, (uint16_t)zero);
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x8014392C: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    v0 = cpu->read_half(v0);
    /* nop */  /* 0x8014393C: 0x00000000 */
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v1;
    if (cond_v0_eq_zero_10) {
        psx_check_interrupts_at(cpu, 0x801439D8u);
        goto block_801439D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143948u);
    }

block_80143948:
    /* delay slot (always executes) */
    t7 = t7 + 4;
    psx_check_interrupts_at(cpu, 0x801439D0u);
    goto block_801439D0;  /* j */

block_80143950:
    cpu->write_half(t2 + 2, (uint16_t)a2);
    cpu->write_half(t2 + 4, (uint16_t)v0);
    cpu->write_half(t2 + 6, (uint16_t)zero);
    v0 = cpu->read_half(t0);
    /* nop */  /* 0x80143960: 0x00000000 */
    v0 = v0 + 1;
    /* delay slot (always executes) */
    v0 = v0 + a3;
    psx_check_interrupts_at(cpu, 0x801439C4u);
    goto block_801439C4;  /* j */

block_80143970:
    /* delay slot (always executes) */
    cpu->write_half(t0, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x801439CCu);
    goto block_801439CC;  /* j */

block_80143978:
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x8014397C: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    v0 = cpu->read_half(v0 + 2);
    /* nop */  /* 0x8014398C: 0x00000000 */
    v0 = v0 << 4;
    cpu->write_half(t2 + 4, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x8014399C: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    v0 = cpu->read_half(v0 + 4);
    /* nop */  /* 0x801439AC: 0x00000000 */
    v0 = v0 << 4;
    cpu->write_half(t2 + 6, (uint16_t)v0);
    v0 = cpu->read_half(t0);
    /* nop */  /* 0x801439BC: 0x00000000 */
    v0 = v0 + 3;
    /* fall through to block_801439C4 */

block_801439C4:
    cpu->write_half(t0, (uint16_t)v0);
    cpu->write_half(t2, (uint16_t)zero);
    /* fall through to block_801439CC */

block_801439CC:
    t7 = t7 + 4;
    /* fall through to block_801439D0 */

block_801439D0:
    t2 = t2 + 10;
    t0 = t0 + 10;
    /* fall through to block_801439D8 */

block_801439D8:
    t6 = t6 + 1;
    v0 = ((int32_t)t6 < 36) ? 1 : 0;  /* 0x801439DC: 0x29C20024 */
    int cond_v0_ne_zero_8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801439E4: 0x00000000 */
    if (cond_v0_ne_zero_8) {
        psx_check_interrupts_at(cpu, 0x8014359Cu);
        goto block_8014359C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801439E8u);
    }

block_801439E8:;
    int cond_t9_eq_zero = (t9 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 << 16;
    if (cond_t9_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80143A2Cu);
        goto block_80143A2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801439F0u);
    }

block_801439F0:
    v1 = cpu->read_byte(a0 + 2);
    a1 = 35;
    v0 = v1 << 1;
    v0 = v0 + v1;
    v1 = v0 << 4;
    v1 = v1 - v0;
    v1 = v1 << 3;
    v0 = 0x801F << 16;  /* 0x801F0000 */
    v0 = v0 + 2784;
    t0 = v1 + v0;
    /* fall through to block_80143A18 */

block_80143A18:
    cpu->write_half(t0, (uint16_t)zero);
    a1 = a1 + -1;
    int cond_a1_ge_0 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = t0 + 10;
    if (cond_a1_ge_0) {
        psx_check_interrupts_at(cpu, 0x80143A18u);
        goto block_80143A18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A28u);
    }

block_80143A28:
    v0 = s0 << 16;
    /* fall through to block_80143A2C */

block_80143A2C:;
    int cond_v0_lt_0 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143A30: 0x00000000 */
    if (cond_v0_lt_0) {
        psx_check_interrupts_at(cpu, 0x80143AB4u);
        goto block_80143AB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A34u);
    }

block_80143A34:
    v0 = cpu->read_word(a0 + 92);
    /* nop */  /* 0x80143A38: 0x00000000 */
    v0 = v0 & 0x0040;  /* 0x80143A3C: 0x30420040 */
    int cond_v0_ne_zero_9 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143A44: 0x00000000 */
    if (cond_v0_ne_zero_9) {
        psx_check_interrupts_at(cpu, 0x80143A64u);
        goto block_80143A64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A48u);
    }

block_80143A48:
    v0 = (int32_t)(int8_t)cpu->read_byte(t4 + 126);
    /* nop */  /* 0x80143A4C: 0x00000000 */
    int cond_v0_ge_0_4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143A54: 0x00000000 */
    if (cond_v0_ge_0_4) {
        psx_check_interrupts_at(cpu, 0x80143A64u);
        goto block_80143A64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A58u);
    }

block_80143A58:
    v0 = cpu->read_byte(a0 + 218);
    /* nop */  /* 0x80143A5C: 0x00000000 */
    cpu->write_half(t4 + 58, (uint16_t)v0);
    /* fall through to block_80143A64 */

block_80143A64:
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x80143A68: 0x00000000 */
    v0 = v0 << 1;
    v0 = t4 + v0;
    cpu->write_half(v0 + 60, (uint16_t)s0);
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x80143A7C: 0x00000000 */
    v0 = v0 << 1;
    v0 = t4 + v0;
    cpu->write_half(v0 + 92, (uint16_t)s1);
    a1 = cpu->read_byte(t4 + 125);
    v0 = cpu->read_byte(t4 + 125);
    v1 = a1 + 1;
    a0 = v1;
    int cond_v1_ge_0 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(t4 + 126, (uint8_t)v0);
    if (cond_v1_ge_0) {
        psx_check_interrupts_at(cpu, 0x80143AA8u);
        goto block_80143AA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143AA4u);
    }

block_80143AA4:
    a0 = a1 + 16;
    /* fall through to block_80143AA8 */

block_80143AA8:
    v0 = a0 & 0x01F0;  /* 0x80143AA8: 0x308201F0 */
    v0 = v1 - v0;
    cpu->write_byte(t4 + 125, (uint8_t)v0);
    /* fall through to block_80143AB4 */

block_80143AB4:
    s2 = cpu->read_word(sp + 8);
    s1 = cpu->read_word(sp + 4);
    s0 = cpu->read_word(sp);
    uint32_t jr_target_3 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 16;
    psx_check_interrupts_at(cpu, jr_target_3);
    cpu->pc = jr_target_3; return;  /* CPS: jr $ra */
}

/* ======== battle_bringup_a_body — clean CPS transliteration of br2eu_psx_alias_body_80148170
 * (shared alias body; entry + battle_bringup_a_cps continuation blocks). */
static void battle_bringup_a_body(CPUState *cpu, uint32_t entry)
{

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801481F0u: goto block_801481F0;
            case 0x80148234u: goto block_80148234;
            case 0x80148248u: goto block_80148248;
            case 0x80148254u: goto block_80148254;
            case 0x8014825Cu: goto block_8014825C;
            case 0x80148278u: goto block_80148278;
            case 0x80148288u: goto block_80148288;
            case 0x801482B0u: goto block_801482B0;
            case 0x801482E0u: goto block_801482E0;
            case 0x801482F4u: goto block_801482F4;
            case 0x801482FCu: goto block_801482FC;
            case 0x80148334u: goto block_80148334;
            case 0x80148344u: goto block_80148344;
            case 0x8014836Cu: goto block_8014836C;
            case 0x80148374u: goto block_80148374;
            case 0x8014837Cu: goto block_8014837C;
            case 0x8014838Cu: goto block_8014838C;
            case 0x8014839Cu: goto block_8014839C;
            case 0x801483B0u: goto block_801483B0;
            case 0x801483C8u: goto block_801483C8;
            case 0x801483E0u: goto block_801483E0;
            case 0x801483F8u: goto block_801483F8;
            case 0x80148410u: goto block_80148410;
            case 0x80148428u: goto block_80148428;
            case 0x80148448u: goto block_80148448;
            case 0x80148460u: goto block_80148460;
            case 0x80148478u: goto block_80148478;
            case 0x80148490u: goto block_80148490;
            case 0x801484A4u: goto block_801484A4;
            case 0x801484ACu: goto block_801484AC;
            case 0x8014823Cu: goto block_8014823C;
            case 0x80148280u: goto block_80148280;
            case 0x801482B8u: goto block_801482B8;
            case 0x801482C0u: goto block_801482C0;
            case 0x801482E8u: goto block_801482E8;
            case 0x801482ECu: goto block_801482EC;
            case 0x80148310u: goto block_80148310;
            case 0x80148318u: goto block_80148318;
            case 0x80148324u: goto block_80148324;
            case 0x8014833Cu: goto block_8014833C;
            case 0x80148394u: goto block_80148394;
            case 0x801484C0u: goto block_801484C0;
            case 0x801484C4u: goto block_801484C4;
            default: break;
        }
    }
    switch (entry) {
        case 0x801481E4u: goto block_801481E4;
        case 0x80148224u: goto block_80148224;
        case 0x8014825Cu: goto block_8014825C;
        case 0x80148304u: goto block_80148304;
        case 0x80148380u: goto block_80148380;
        case 0x801484BCu: goto block_801484BC;
        default: return;
    }

block_801481E4:
    a0 = -1;
    ra = 0x801481F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x8018CFDCu);
    cpu->pc = 0x8018CFDCu; return;  /* CPS jal -> 0x8018CFDC */

block_801481F0:
    a0 = 0x800D << 16;  /* 0x800D0000 */
    a0 = a0 | 0xC800;
    s0 = 8;
    v0 = 0x801C << 16;  /* 0x801C0000 */
    v1 = 0x801E << 16;  /* 0x801E0000 */
    cpu->write_word(v0 + 17312, zero);
    v0 = -1;
    cpu->write_byte(v1 + 15616, (uint8_t)v0);
    v1 = 0x801C << 16;  /* 0x801C0000 */
    v0 = 0x8001 << 16;  /* 0x80010000 */
    cpu->write_word(v1 + 18360, v0);
    v0 = 0x801E << 16;  /* 0x801E0000 */
    cpu->write_word(v0 + 15612, a0);
    /* fall through to block_80148224 */

block_80148224:
    s1 = 0x801C << 16;  /* 0x801C0000 */
    a1 = cpu->read_word(s1 + 18360);
    ra = 0x80148234u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 140;
    psx_check_interrupts_at(cpu, 0x8018CFDCu);
    cpu->pc = 0x8018CFDCu; return;  /* CPS jal -> 0x8018CFDC */

block_80148234:;
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014823Cu);
    }

block_8014823C:
    a0 = cpu->read_word(s1 + 18360);
    ra = 0x80148248u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = 16;
    psx_check_interrupts_at(cpu, 0x801566A0u);
    cpu->pc = 0x801566A0u; return;  /* CPS jal -> 0x801566A0 */

block_80148248:
    a0 = 17;
    ra = 0x80148254u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x801566D4u);
    cpu->pc = 0x801566D4u; return;  /* CPS jal -> 0x801566D4 */

block_80148254:
    ra = 0x8014825Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_8014825C:
    v0 = 0x801B << 16;  /* 0x801B0000 */
    v0 = v0 + 18936;
    v0 = s4 + v0;
    a0 = cpu->read_byte(v0);
    a1 = s3;
    ra = 0x80148278u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 199;
    psx_check_interrupts_at(cpu, 0x8018CFDCu);
    cpu->pc = 0x8018CFDCu; return;  /* CPS jal -> 0x8018CFDC */

block_80148278:;
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014827C: 0x00000000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80148280u);
    }

block_80148280:
    ra = 0x80148288u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x801274D8u);
    cpu->pc = 0x801274D8u; return;  /* CPS jal -> 0x801274D8 */

block_80148288:
    a0 = s3;
    a1 = zero;
    a2 = 501;
    a3 = 1;
    v0 = 512;
    s0 = a3;
    cpu->write_word(sp + 16, v0);
    cpu->write_word(sp + 20, zero);
    ra = 0x801482B0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, s0);
    psx_check_interrupts_at(cpu, 0x8018BFCCu);
    cpu->pc = 0x8018BFCCu; return;  /* CPS jal -> 0x8018BFCC */

block_801482B0:;
    int cond_s4_ne_s2 = (s4 != s2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (cond_s4_ne_s2) {
        psx_check_interrupts_at(cpu, 0x801482E8u);
        goto block_801482E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801482B8u);
    }

block_801482B8:;
    int cond_s2_eq_v0 = (s2 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    if (cond_s2_eq_v0) {
        psx_check_interrupts_at(cpu, 0x801482E8u);
        goto block_801482E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801482C0u);
    }

block_801482C0:
    a1 = zero;
    a2 = 502;
    a3 = 1;
    v0 = 608;
    cpu->write_word(sp + 16, v0);
    cpu->write_word(sp + 20, zero);
    ra = 0x801482E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, s0);
    psx_check_interrupts_at(cpu, 0x8018BFCCu);
    cpu->pc = 0x8018BFCCu; return;  /* CPS jal -> 0x8018BFCC */

block_801482E0:
    /* delay slot (always executes) */
    s0 = 48;
    psx_check_interrupts_at(cpu, 0x801482ECu);
    goto block_801482EC;  /* j */

block_801482E8:
    s0 = 32;
    /* fall through to block_801482EC */

block_801482EC:
    ra = 0x801482F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_801482F4:
    ra = 0x801482FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x801274D8u);
    cpu->pc = 0x801274D8u; return;  /* CPS jal -> 0x801274D8 */

block_801482FC:
    /* delay slot (always executes) */
    cpu->write_half(s5 + 140, (uint16_t)s0);
    psx_check_interrupts_at(cpu, 0x801484C0u);
    goto block_801484C0;  /* j */

block_80148304:
    v0 = 8;
    int cond_s2_ne_v0 = (s2 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (cond_s2_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80148318u);
        goto block_80148318;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80148310u);
    }

block_80148310:
    /* delay slot (always executes) */
    a0 = 63;
    psx_check_interrupts_at(cpu, 0x80148324u);
    goto block_80148324;  /* j */

block_80148318:
    v0 = v0 + 18936;
    v0 = s2 + v0;
    a0 = cpu->read_byte(v0);
    /* fall through to block_80148324 */

block_80148324:
    /* nop */  /* 0x80148324: 0x00000000 */
    a0 = a0 + 199;
    ra = 0x80148334u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s3;
    psx_check_interrupts_at(cpu, 0x8018CFDCu);
    cpu->pc = 0x8018CFDCu; return;  /* CPS jal -> 0x8018CFDC */

block_80148334:;
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80148338: 0x00000000 */
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014833Cu);
    }

block_8014833C:
    ra = 0x80148344u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x801274D8u);
    cpu->pc = 0x801274D8u; return;  /* CPS jal -> 0x801274D8 */

block_80148344:
    a0 = s3;
    a1 = zero;
    a2 = 502;
    a3 = 1;
    v0 = 608;
    cpu->write_word(sp + 16, v0);
    v0 = a3;
    cpu->write_word(sp + 20, zero);
    ra = 0x8014836Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, v0);
    psx_check_interrupts_at(cpu, 0x8018BFCCu);
    cpu->pc = 0x8018BFCCu; return;  /* CPS jal -> 0x8018BFCC */

block_8014836C:
    ra = 0x80148374u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80148374:
    ra = 0x8014837Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x801274D8u);
    cpu->pc = 0x801274D8u; return;  /* CPS jal -> 0x801274D8 */

block_8014837C:
    s0 = 48;
    /* fall through to block_80148380 */

block_80148380:
    a0 = 129;
    ra = 0x8014838Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s3;
    psx_check_interrupts_at(cpu, 0x8018CFDCu);
    cpu->pc = 0x8018CFDCu; return;  /* CPS jal -> 0x8018CFDC */

block_8014838C:;
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80148390: 0x00000000 */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80148394u);
    }

block_80148394:
    ra = 0x8014839Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x801274D8u);
    cpu->pc = 0x801274D8u; return;  /* CPS jal -> 0x801274D8 */

block_8014839C:
    a1 = s3;
    s1 = 0x801C << 16;  /* 0x801C0000 */
    a0 = cpu->read_word(s1 + 18360);
    ra = 0x801483B0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = 49;
    psx_check_interrupts_at(cpu, 0x80163154u);
    cpu->pc = 0x80163154u; return;  /* CPS jal -> 0x80163154 */

block_801483B0:
    s2 = v0;
    a1 = 768;
    a2 = 256;
    a0 = cpu->read_word(s1 + 18360);
    ra = 0x801483C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 1;
    psx_check_interrupts_at(cpu, 0x8018BF40u);
    cpu->pc = 0x8018BF40u; return;  /* CPS jal -> 0x8018BF40 */

block_801483C8:
    a1 = s3;
    a0 = cpu->read_word(s1 + 18360);
    a2 = 50;
    a0 = a0 + s2;
    ra = 0x801483E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s1 + 18360, a0);
    psx_check_interrupts_at(cpu, 0x80163154u);
    cpu->pc = 0x80163154u; return;  /* CPS jal -> 0x80163154 */

block_801483E0:
    s2 = v0;
    a1 = 640;
    a2 = 256;
    a0 = cpu->read_word(s1 + 18360);
    ra = 0x801483F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 1;
    psx_check_interrupts_at(cpu, 0x8018BF40u);
    cpu->pc = 0x8018BF40u; return;  /* CPS jal -> 0x8018BF40 */

block_801483F8:
    a1 = s3;
    a0 = cpu->read_word(s1 + 18360);
    a2 = 48;
    a0 = a0 + s2;
    ra = 0x80148410u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s1 + 18360, a0);
    psx_check_interrupts_at(cpu, 0x80163154u);
    cpu->pc = 0x80163154u; return;  /* CPS jal -> 0x80163154 */

block_80148410:
    s2 = v0;
    a1 = zero;
    a2 = 480;
    a0 = cpu->read_word(s1 + 18360);
    ra = 0x80148428u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 1;
    psx_check_interrupts_at(cpu, 0x8018BF40u);
    cpu->pc = 0x8018BF40u; return;  /* CPS jal -> 0x8018BF40 */

block_80148428:
    a1 = s3;
    a2 = 52;
    s0 = 0x801E << 16;  /* 0x801E0000 */
    v0 = cpu->read_word(s1 + 18360);
    a0 = cpu->read_word(s0 + 15612);
    v0 = v0 + s2;
    ra = 0x80148448u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s1 + 18360, v0);
    psx_check_interrupts_at(cpu, 0x80163154u);
    cpu->pc = 0x80163154u; return;  /* CPS jal -> 0x80163154 */

block_80148448:
    s2 = v0;
    a1 = 704;
    a2 = zero;
    a0 = cpu->read_word(s0 + 15612);
    ra = 0x80148460u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 1;
    psx_check_interrupts_at(cpu, 0x8018BF40u);
    cpu->pc = 0x8018BF40u; return;  /* CPS jal -> 0x8018BF40 */

block_80148460:
    a1 = s3;
    a0 = cpu->read_word(s0 + 15612);
    a2 = 53;
    a0 = a0 + s2;
    ra = 0x80148478u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 15612, a0);
    psx_check_interrupts_at(cpu, 0x80163154u);
    cpu->pc = 0x80163154u; return;  /* CPS jal -> 0x80163154 */

block_80148478:
    s2 = v0;
    a1 = 704;
    a2 = 256;
    a0 = cpu->read_word(s0 + 15612);
    ra = 0x80148490u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 1;
    psx_check_interrupts_at(cpu, 0x8018BF40u);
    cpu->pc = 0x8018BF40u; return;  /* CPS jal -> 0x8018BF40 */

block_80148490:
    v0 = cpu->read_word(s0 + 15612);
    a0 = zero;
    v0 = v0 + s2;
    ra = 0x801484A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 15612, v0);
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_801484A4:
    ra = 0x801484ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x801274D8u);
    cpu->pc = 0x801274D8u; return;  /* CPS jal -> 0x801274D8 */

block_801484AC:
    v0 = 1;
    v1 = -1;
    /* delay slot (always executes) */
    cpu->write_half(s5 + 140, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x801484C4u);
    goto block_801484C4;  /* j */

block_801484BC:
    cpu->write_half(s5 + 140, (uint16_t)s0);
    /* fall through to block_801484C0 */

block_801484C0:
    v0 = zero;
    /* fall through to block_801484C4 */

block_801484C4:
    ra = cpu->read_word(sp + 56);
    s5 = cpu->read_word(sp + 52);
    s4 = cpu->read_word(sp + 48);
    s3 = cpu->read_word(sp + 44);
    s2 = cpu->read_word(sp + 40);
    s1 = cpu->read_word(sp + 36);
    s0 = cpu->read_word(sp + 32);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 64;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

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

/* cmd_input_parse_cps — CPS re-entry symbol for every continuation/branch-target addr of
 * the cmd_input_parse_body region (game/clean_cps.map). cpu->pc == dispatched addr. */
void cmd_input_parse_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cmd_input_parse_body(cpu, cpu->pc);
}

/* battle_bringup_a_cps — CPS re-entry symbol for every continuation/branch-target addr of
 * the battle_bringup_a_body region (game/clean_cps.map). cpu->pc == dispatched addr. */
void battle_bringup_a_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  battle_bringup_a_body(cpu, cpu->pc);
}

/* ko_count_announce_cps — CPS re-entry symbol for every continuation/branch-target addr of
 * the ko_count_announce_body region (game/clean_cps.map). cpu->pc == dispatched addr. */
void ko_count_announce_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  ko_count_announce_body(cpu, cpu->pc);
}

/* 0x80143978 — alias piece-entry into cmd_input_parse_body (see game/renames.map). */
void cmd_input_parse_alt_3978(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  cmd_input_parse_body(cpu, 0x80143978u);
}

/* 0x80148224 — alias piece-entry into battle_bringup_a_body (see game/renames.map). */
void battle_bringup_a_alt_8224(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  battle_bringup_a_body(cpu, 0x80148224u);
}

/* 0x80148304 — alias piece-entry into battle_bringup_a_body (see game/renames.map). */
void battle_bringup_a_alt_8304(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  battle_bringup_a_body(cpu, 0x80148304u);
}

/* 0x80148380 — alias piece-entry into battle_bringup_a_body (see game/renames.map). */
void battle_bringup_a_alt_8380(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  battle_bringup_a_body(cpu, 0x80148380u);
}

/* 0x80151014 — alias piece-entry into ko_count_announce_body (see game/renames.map). */
void ko_count_announce_alt_1014(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  ko_count_announce_body(cpu, 0x80151014u);
}
