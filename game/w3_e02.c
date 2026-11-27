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
    int _bc_801435A4 = (t3 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801435A8: 0x00000000 */
    if (_bc_801435A4) {
        psx_check_interrupts_at(cpu, 0x801439E8u);
        goto block_801439E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801435ACu);
        goto block_801435AC;  /* not taken */
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
    int _bc_801435D4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (_bc_801435D4) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801435DCu);
        goto block_801435DC;  /* not taken */
    }

block_801435DC:
    v0 = v0 + s2;
    v0 = cpu->read_word(v0);
    /* nop */  /* 0x801435E4: 0x00000000 */
    uint32_t _jt_801435E8 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801435EC: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801435E8);
    cpu->pc = _jt_801435E8; return;  /* CPS: jr v0 */

block_801435F0:
    cpu->write_half(t0, (uint16_t)zero);
    /* delay slot (always executes) */
    t9 = 1;
    psx_check_interrupts_at(cpu, 0x801439CCu);
    goto block_801439CC;  /* j */

block_801435FC:
    v0 = cpu->read_word(a0 + 88);
    /* nop */  /* 0x80143600: 0x00000000 */
    int _bc_80143604 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143608: 0x00000000 */
    if (_bc_80143604) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014360Cu);
        goto block_8014360C;  /* not taken */
    }

block_8014360C:
    v0 = (int32_t)(int16_t)cpu->read_half(t0);
    /* nop */  /* 0x80143610: 0x00000000 */
    v0 = v0 << 1;
    v0 = v0 + t3;
    a3 = cpu->read_half(v0 + 2);
    /* nop */  /* 0x80143620: 0x00000000 */
    v0 = a3 & 0x4000;  /* 0x80143624: 0x30E24000 */
    int _bc_80143628 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    if (_bc_80143628) {
        psx_check_interrupts_at(cpu, 0x8014363Cu);
        goto block_8014363C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143630u);
        goto block_80143630;  /* not taken */
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
    int _bc_80143648 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014364C: 0x00000000 */
    if (_bc_80143648) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143650u);
        goto block_80143650;  /* not taken */
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
    int _bc_80143670 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a2 & 0x0FFF;  /* 0x80143674: 0x30C20FFF */
    if (_bc_80143670) {
        psx_check_interrupts_at(cpu, 0x801436A0u);
        goto block_801436A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143678u);
        goto block_80143678;  /* not taken */
    }

block_80143678:
    v1 = cpu->read_byte(t4 + 124);
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x80143680: 0x00000000 */
    int _bc_80143684 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143688: 0x00000000 */
    if (_bc_80143684) {
        psx_check_interrupts_at(cpu, 0x801436A8u);
        goto block_801436A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014368Cu);
        goto block_8014368C;  /* not taken */
    }

block_8014368C:
    v0 = cpu->read_word(a0 + 92);
    /* nop */  /* 0x80143690: 0x00000000 */
    v0 = v0 & 0x0040;  /* 0x80143694: 0x30420040 */
    int _bc_80143698 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a2 & 0x0FFF;  /* 0x8014369C: 0x30C20FFF */
    if (_bc_80143698) {
        psx_check_interrupts_at(cpu, 0x801436A8u);
        goto block_801436A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436A0u);
        goto block_801436A0;  /* not taken */
    }

block_801436A0:
    int _bc_801436A0 = (t8 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = -1;
    if (_bc_801436A0) {
        psx_check_interrupts_at(cpu, 0x80143950u);
        goto block_80143950;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436A8u);
        goto block_801436A8;  /* not taken */
    }

block_801436A8:
    a1 = a1 + 1;
    v0 = ((int32_t)a1 < (int32_t)a3) ? 1 : 0;  /* 0x801436AC: 0x00A7102A */
    int _bc_801436B0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a1 + t1;
    if (_bc_801436B0) {
        psx_check_interrupts_at(cpu, 0x8014365Cu);
        goto block_8014365C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436B8u);
        goto block_801436B8;  /* not taken */
    }

block_801436B8:
    /* delay slot (always executes) */
    cpu->write_half(t0, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x801439CCu);
    goto block_801439CC;  /* j */

block_801436C0:
    v0 = cpu->read_word(a0 + 88);
    /* nop */  /* 0x801436C4: 0x00000000 */
    int _bc_801436C8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801436CC: 0x00000000 */
    if (_bc_801436C8) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436D0u);
        goto block_801436D0;  /* not taken */
    }

block_801436D0:
    v1 = cpu->read_half(t2 + 2);
    /* nop */  /* 0x801436D4: 0x00000000 */
    v0 = v1 & 0x0FFF;  /* 0x801436D8: 0x30620FFF */
    int _bc_801436DC = (v0 != t8);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x1000;  /* 0x801436E0: 0x30621000 */
    if (_bc_801436DC) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436E4u);
        goto block_801436E4;  /* not taken */
    }

block_801436E4:
    int _bc_801436E4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801436E8: 0x00000000 */
    if (_bc_801436E4) {
        psx_check_interrupts_at(cpu, 0x80143714u);
        goto block_80143714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801436ECu);
        goto block_801436EC;  /* not taken */
    }

block_801436EC:
    v1 = cpu->read_byte(t4 + 124);
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x801436F4: 0x00000000 */
    int _bc_801436F8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801436FC: 0x00000000 */
    if (_bc_801436F8) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143700u);
        goto block_80143700;  /* not taken */
    }

block_80143700:
    v0 = cpu->read_word(a0 + 92);
    /* nop */  /* 0x80143704: 0x00000000 */
    v0 = v0 & 0x0040;  /* 0x80143708: 0x30420040 */
    int _bc_8014370C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143710: 0x00000000 */
    if (_bc_8014370C) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143714u);
        goto block_80143714;  /* not taken */
    }

block_80143714:
    v1 = (int32_t)(int16_t)cpu->read_half(t2 + 4);
    /* nop */  /* 0x80143718: 0x00000000 */
    int _bc_8014371C = ((int32_t)v1 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143720: 0x00000000 */
    if (_bc_8014371C) {
        psx_check_interrupts_at(cpu, 0x80143748u);
        goto block_80143748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143724u);
        goto block_80143724;  /* not taken */
    }

block_80143724:
    v0 = (int32_t)(int8_t)cpu->read_byte(t4 + 126);
    /* nop */  /* 0x80143728: 0x00000000 */
    int _bc_8014372C = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143730: 0x00000000 */
    if (_bc_8014372C) {
        psx_check_interrupts_at(cpu, 0x80143748u);
        goto block_80143748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143734u);
        goto block_80143734;  /* not taken */
    }

block_80143734:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 188);
    /* nop */  /* 0x80143738: 0x00000000 */
    v0 = ((int32_t)v1 < (int32_t)v0) ? 1 : 0;  /* 0x8014373C: 0x0062102A */
    int _bc_80143740 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143744: 0x00000000 */
    if (_bc_80143740) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143748u);
        goto block_80143748;  /* not taken */
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
    int _bc_80143774 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_80143774) {
        psx_check_interrupts_at(cpu, 0x8014380Cu);
        goto block_8014380C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014377Cu);
        goto block_8014377C;  /* not taken */
    }

block_8014377C:
    v0 = v0 + 644;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x8014378C: 0x00000000 */
    uint32_t _jt_80143790 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143794: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80143790);
    cpu->pc = _jt_80143790; return;  /* CPS: jr v0 */

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
    int _bc_801437FC = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 136;
    if (_bc_801437FC) {
        psx_check_interrupts_at(cpu, 0x8014380Cu);
        goto block_8014380C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143804u);
        goto block_80143804;  /* not taken */
    }

block_80143804:
    int _bc_80143804 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143808: 0x00000000 */
    if (_bc_80143804) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014380Cu);
        goto block_8014380C;  /* not taken */
    }

block_8014380C:
    v0 = cpu->read_byte(a0 + 5);
    /* nop */  /* 0x80143810: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x80143814: 0x30420001 */
    int _bc_80143818 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014381C: 0x00000000 */
    if (_bc_80143818) {
        psx_check_interrupts_at(cpu, 0x80143838u);
        goto block_80143838;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143820u);
        goto block_80143820;  /* not taken */
    }

block_80143820:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 214);
    /* nop */  /* 0x80143824: 0x00000000 */
    v0 = ((int32_t)v0 < 7680) ? 1 : 0;  /* 0x80143828: 0x28421E00 */
    int _bc_8014382C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143830: 0x00000000 */
    if (_bc_8014382C) {
        psx_check_interrupts_at(cpu, 0x80143838u);
        goto block_80143838;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143834u);
        goto block_80143834;  /* not taken */
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
    int _bc_80143854 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143858: 0x00000000 */
    if (_bc_80143854) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014385Cu);
        goto block_8014385C;  /* not taken */
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
    int _bc_8014388C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + 5;
    if (_bc_8014388C) {
        psx_check_interrupts_at(cpu, 0x801439CCu);
        goto block_801439CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143894u);
        goto block_80143894;  /* not taken */
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
    int _bc_801438BC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t9 = zero;
    if (_bc_801438BC) {
        psx_check_interrupts_at(cpu, 0x801438FCu);
        goto block_801438FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438C4u);
        goto block_801438C4;  /* not taken */
    }

block_801438C4:
    v0 = cpu->read_byte(t4 + 125);
    /* nop */  /* 0x801438C8: 0x00000000 */
    int _bc_801438CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = t9;
    if (_bc_801438CC) {
        psx_check_interrupts_at(cpu, 0x801438FCu);
        goto block_801438FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438D4u);
        goto block_801438D4;  /* not taken */
    }

block_801438D4:
    t1 = a2 & 0x0FFF;  /* 0x801438D4: 0x30C90FFF */
    a3 = v0;
    v1 = t4;
    /* fall through to block_801438E0 */

block_801438E0:
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 60);
    /* nop */  /* 0x801438E4: 0x00000000 */
    int _bc_801438E8 = (t1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 1;
    if (_bc_801438E8) {
        psx_check_interrupts_at(cpu, 0x80143970u);
        goto block_80143970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438F0u);
        goto block_801438F0;  /* not taken */
    }

block_801438F0:
    v0 = ((int32_t)a1 < (int32_t)a3) ? 1 : 0;  /* 0x801438F0: 0x00A7102A */
    int _bc_801438F4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 + 2;
    if (_bc_801438F4) {
        psx_check_interrupts_at(cpu, 0x801438E0u);
        goto block_801438E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801438FCu);
        goto block_801438FC;  /* not taken */
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
    int _bc_80143940 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v1;
    if (_bc_80143940) {
        psx_check_interrupts_at(cpu, 0x801439D8u);
        goto block_801439D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143948u);
        goto block_80143948;  /* not taken */
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
    int _bc_801439E0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801439E4: 0x00000000 */
    if (_bc_801439E0) {
        psx_check_interrupts_at(cpu, 0x8014359Cu);
        goto block_8014359C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801439E8u);
        goto block_801439E8;  /* not taken */
    }

block_801439E8:
    int _bc_801439E8 = (t9 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 << 16;
    if (_bc_801439E8) {
        psx_check_interrupts_at(cpu, 0x80143A2Cu);
        goto block_80143A2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801439F0u);
        goto block_801439F0;  /* not taken */
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
    int _bc_80143A20 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = t0 + 10;
    if (_bc_80143A20) {
        psx_check_interrupts_at(cpu, 0x80143A18u);
        goto block_80143A18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A28u);
        goto block_80143A28;  /* not taken */
    }

block_80143A28:
    v0 = s0 << 16;
    /* fall through to block_80143A2C */

block_80143A2C:
    int _bc_80143A2C = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143A30: 0x00000000 */
    if (_bc_80143A2C) {
        psx_check_interrupts_at(cpu, 0x80143AB4u);
        goto block_80143AB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A34u);
        goto block_80143A34;  /* not taken */
    }

block_80143A34:
    v0 = cpu->read_word(a0 + 92);
    /* nop */  /* 0x80143A38: 0x00000000 */
    v0 = v0 & 0x0040;  /* 0x80143A3C: 0x30420040 */
    int _bc_80143A40 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143A44: 0x00000000 */
    if (_bc_80143A40) {
        psx_check_interrupts_at(cpu, 0x80143A64u);
        goto block_80143A64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A48u);
        goto block_80143A48;  /* not taken */
    }

block_80143A48:
    v0 = (int32_t)(int8_t)cpu->read_byte(t4 + 126);
    /* nop */  /* 0x80143A4C: 0x00000000 */
    int _bc_80143A50 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143A54: 0x00000000 */
    if (_bc_80143A50) {
        psx_check_interrupts_at(cpu, 0x80143A64u);
        goto block_80143A64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143A58u);
        goto block_80143A58;  /* not taken */
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
    int _bc_80143A9C = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(t4 + 126, (uint8_t)v0);
    if (_bc_80143A9C) {
        psx_check_interrupts_at(cpu, 0x80143AA8u);
        goto block_80143AA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143AA4u);
        goto block_80143AA4;  /* not taken */
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
    uint32_t _jt_80143AC0 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 16;
    psx_check_interrupts_at(cpu, _jt_80143AC0);
    cpu->pc = _jt_80143AC0; return;  /* CPS: jr $ra */
}

/* ======== hit_react_apply_body — clean CPS transliteration of br2eu_psx_alias_body_80143D38
 * (shared alias body; entry + hit_react_apply_cps continuation blocks). */
static void hit_react_apply_body(CPUState *cpu, uint32_t entry)
{

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801448D4u: goto block_801448D4;
            case 0x80144900u: goto block_80144900;
            case 0x80144D4Cu: goto block_80144D4C;
            case 0x80144D6Cu: goto block_80144D6C;
            case 0x80144D90u: goto block_80144D90;
            case 0x80144DACu: goto block_80144DAC;
            case 0x80144130u: goto block_80144130;
            case 0x80144134u: goto block_80144134;
            case 0x801441C4u: goto block_801441C4;
            case 0x801441F0u: goto block_801441F0;
            case 0x801441FCu: goto block_801441FC;
            case 0x80144208u: goto block_80144208;
            case 0x80144214u: goto block_80144214;
            case 0x8014421Cu: goto block_8014421C;
            case 0x80144230u: goto block_80144230;
            case 0x8014423Cu: goto block_8014423C;
            case 0x80144248u: goto block_80144248;
            case 0x80144254u: goto block_80144254;
            case 0x8014425Cu: goto block_8014425C;
            case 0x80144270u: goto block_80144270;
            case 0x80144284u: goto block_80144284;
            case 0x80144290u: goto block_80144290;
            case 0x801442A4u: goto block_801442A4;
            case 0x801442B0u: goto block_801442B0;
            case 0x801442C4u: goto block_801442C4;
            case 0x801442D0u: goto block_801442D0;
            case 0x801442D8u: goto block_801442D8;
            case 0x801442E0u: goto block_801442E0;
            case 0x801442E4u: goto block_801442E4;
            case 0x801442ECu: goto block_801442EC;
            case 0x801442F0u: goto block_801442F0;
            case 0x80144310u: goto block_80144310;
            case 0x8014431Cu: goto block_8014431C;
            case 0x80144324u: goto block_80144324;
            case 0x8014432Cu: goto block_8014432C;
            case 0x80144330u: goto block_80144330;
            case 0x80144368u: goto block_80144368;
            case 0x80144370u: goto block_80144370;
            case 0x80144394u: goto block_80144394;
            case 0x801443A0u: goto block_801443A0;
            case 0x801443A8u: goto block_801443A8;
            case 0x801443B4u: goto block_801443B4;
            case 0x801443B8u: goto block_801443B8;
            case 0x801443D0u: goto block_801443D0;
            case 0x801443E0u: goto block_801443E0;
            case 0x801443F4u: goto block_801443F4;
            case 0x80144400u: goto block_80144400;
            case 0x80144414u: goto block_80144414;
            case 0x80144420u: goto block_80144420;
            case 0x8014442Cu: goto block_8014442C;
            case 0x80144438u: goto block_80144438;
            case 0x80144448u: goto block_80144448;
            case 0x80144458u: goto block_80144458;
            case 0x80144460u: goto block_80144460;
            case 0x80144470u: goto block_80144470;
            case 0x80144478u: goto block_80144478;
            case 0x80144488u: goto block_80144488;
            case 0x80144494u: goto block_80144494;
            case 0x801444A0u: goto block_801444A0;
            case 0x801444ACu: goto block_801444AC;
            case 0x801444BCu: goto block_801444BC;
            case 0x801444C8u: goto block_801444C8;
            case 0x801444D8u: goto block_801444D8;
            case 0x801444E0u: goto block_801444E0;
            case 0x801444F0u: goto block_801444F0;
            case 0x801444F8u: goto block_801444F8;
            case 0x80144508u: goto block_80144508;
            case 0x80144518u: goto block_80144518;
            case 0x80144528u: goto block_80144528;
            case 0x80144530u: goto block_80144530;
            case 0x80144540u: goto block_80144540;
            case 0x80144550u: goto block_80144550;
            case 0x8014455Cu: goto block_8014455C;
            case 0x80144568u: goto block_80144568;
            case 0x80144570u: goto block_80144570;
            case 0x8014457Cu: goto block_8014457C;
            case 0x80144588u: goto block_80144588;
            case 0x80144590u: goto block_80144590;
            case 0x801445A0u: goto block_801445A0;
            case 0x801445ACu: goto block_801445AC;
            case 0x801445B8u: goto block_801445B8;
            case 0x801445C0u: goto block_801445C0;
            case 0x801445CCu: goto block_801445CC;
            case 0x801445D8u: goto block_801445D8;
            case 0x801445E0u: goto block_801445E0;
            case 0x801445E8u: goto block_801445E8;
            case 0x801445F8u: goto block_801445F8;
            case 0x80144604u: goto block_80144604;
            case 0x80144610u: goto block_80144610;
            case 0x80144618u: goto block_80144618;
            case 0x80144624u: goto block_80144624;
            case 0x8014462Cu: goto block_8014462C;
            case 0x8014463Cu: goto block_8014463C;
            case 0x80144648u: goto block_80144648;
            case 0x80144654u: goto block_80144654;
            case 0x8014465Cu: goto block_8014465C;
            case 0x80144668u: goto block_80144668;
            case 0x80144670u: goto block_80144670;
            case 0x80144680u: goto block_80144680;
            case 0x8014468Cu: goto block_8014468C;
            case 0x80144698u: goto block_80144698;
            case 0x801446A0u: goto block_801446A0;
            case 0x801446ACu: goto block_801446AC;
            case 0x801446B4u: goto block_801446B4;
            case 0x801446C4u: goto block_801446C4;
            case 0x801446D0u: goto block_801446D0;
            case 0x801446DCu: goto block_801446DC;
            case 0x801446E4u: goto block_801446E4;
            case 0x801446F4u: goto block_801446F4;
            case 0x80144700u: goto block_80144700;
            case 0x8014470Cu: goto block_8014470C;
            case 0x80144714u: goto block_80144714;
            case 0x80144720u: goto block_80144720;
            case 0x80144734u: goto block_80144734;
            case 0x80144748u: goto block_80144748;
            case 0x8014474Cu: goto block_8014474C;
            case 0x80144750u: goto block_80144750;
            case 0x80144754u: goto block_80144754;
            case 0x8014477Cu: goto block_8014477C;
            case 0x8014478Cu: goto block_8014478C;
            case 0x80144794u: goto block_80144794;
            case 0x801447B0u: goto block_801447B0;
            case 0x801447C8u: goto block_801447C8;
            case 0x801447D4u: goto block_801447D4;
            case 0x801447E0u: goto block_801447E0;
            case 0x80144800u: goto block_80144800;
            case 0x80144804u: goto block_80144804;
            case 0x80144818u: goto block_80144818;
            case 0x8014482Cu: goto block_8014482C;
            case 0x80144850u: goto block_80144850;
            case 0x80144864u: goto block_80144864;
            case 0x801448A4u: goto block_801448A4;
            case 0x801448C0u: goto block_801448C0;
            case 0x80144D0Cu: goto block_80144D0C;
            case 0x80144D1Cu: goto block_80144D1C;
            case 0x80144D20u: goto block_80144D20;
            case 0x80144D2Cu: goto block_80144D2C;
            case 0x80144D40u: goto block_80144D40;
            case 0x80144D44u: goto block_80144D44;
            case 0x80144D54u: goto block_80144D54;
            case 0x80144D60u: goto block_80144D60;
            case 0x80144D74u: goto block_80144D74;
            case 0x80144D84u: goto block_80144D84;
            case 0x80144D98u: goto block_80144D98;
            case 0x80144DA4u: goto block_80144DA4;
            case 0x80144DC0u: goto block_80144DC0;
            case 0x80144DF8u: goto block_80144DF8;
            case 0x80144E08u: goto block_80144E08;
            case 0x80144E10u: goto block_80144E10;
            case 0x80144E14u: goto block_80144E14;
            case 0x80144E28u: goto block_80144E28;
            case 0x80144EC0u: goto block_80144EC0;
            case 0x80144F04u: goto block_80144F04;
            case 0x80144F0Cu: goto block_80144F0C;
            case 0x80144F10u: goto block_80144F10;
            case 0x80144F1Cu: goto block_80144F1C;
            case 0x80144F28u: goto block_80144F28;
            case 0x80144F30u: goto block_80144F30;
            case 0x80144F5Cu: goto block_80144F5C;
            default: break;
        }
    }
    switch (entry) {
        case 0x80144034u: goto block_80144034;
        case 0x80144054u: goto block_80144054;
        case 0x80144070u: goto block_80144070;
        case 0x80144094u: goto block_80144094;
        case 0x801440DCu: goto block_801440DC;
        case 0x801440F8u: goto block_801440F8;
        case 0x80144114u: goto block_80144114;
        case 0x80144158u: goto block_80144158;
        case 0x801441C8u: goto block_801441C8;
        default: return;
    }

block_80144034:
    a0 = 0x6666 << 16;  /* 0x66660000 */
    a0 = a0 | 0x6667;
    v0 = s2 << 3;
    v0 = v0 + s2;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    v1 = a1 << 3;
    psx_check_interrupts_at(cpu, 0x80144130u);
    goto block_80144130;  /* j */

block_80144054:
    a0 = 0x6666 << 16;  /* 0x66660000 */
    a0 = a0 | 0x6667;
    v0 = s2 << 3;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    v1 = a1 << 3;
    psx_check_interrupts_at(cpu, 0x80144134u);
    goto block_80144134;  /* j */

block_80144070:
    a0 = 0x6666 << 16;  /* 0x66660000 */
    a0 = a0 | 0x6667;
    v0 = s2 << 3;
    v0 = v0 - s2;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    v1 = a1 << 3;
    /* delay slot (always executes) */
    v1 = v1 - a1;
    psx_check_interrupts_at(cpu, 0x80144134u);
    goto block_80144134;  /* j */

block_80144094:
    a0 = 0x6666 << 16;  /* 0x66660000 */
    a0 = a0 | 0x6667;
    v1 = s2 << 1;
    v1 = v1 + s2;
    v1 = v1 << 1;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v0 = a1 << 1;
    a2 = cpu->hi;
    psx_muldiv_stall(cpu);
    v0 = v0 + a1;
    v0 = v0 << 1;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    v1 = (int32_t)v1 >> 31;
    a0 = (int32_t)a2 >> 2;
    s2 = a0 - v1;
    v0 = (int32_t)v0 >> 31;
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    v1 = (int32_t)t1 >> 2;
    psx_check_interrupts_at(cpu, 0x801441C4u);
    goto block_801441C4;  /* j */

block_801440DC:
    v0 = (uint32_t)s2 >> 31;
    v0 = s2 + v0;
    s2 = (int32_t)v0 >> 1;
    v0 = (uint32_t)a1 >> 31;
    v0 = a1 + v0;
    /* delay slot (always executes) */
    a1 = (int32_t)v0 >> 1;
    psx_check_interrupts_at(cpu, 0x801441C8u);
    goto block_801441C8;  /* j */

block_801440F8:
    a0 = 0x6666 << 16;  /* 0x66660000 */
    a0 = a0 | 0x6667;
    v0 = s2 << 2;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    v1 = a1 << 2;
    psx_check_interrupts_at(cpu, 0x80144134u);
    goto block_80144134;  /* j */

block_80144114:
    a0 = 0x6666 << 16;  /* 0x66660000 */
    a0 = a0 | 0x6667;
    v0 = s2 << 1;
    v0 = v0 + s2;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    v1 = a1 << 1;
    /* fall through to block_80144130 */

block_80144130:
    v1 = v1 + a1;
    /* fall through to block_80144134 */

block_80144134:
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v0 = (int32_t)v0 >> 31;
    a0 = (int32_t)t1 >> 2;
    s2 = a0 - v0;
    v1 = (int32_t)v1 >> 31;
    a1 = cpu->hi;
    psx_muldiv_stall(cpu);
    v0 = (int32_t)a1 >> 2;
    /* delay slot (always executes) */
    a1 = v0 - v1;
    psx_check_interrupts_at(cpu, 0x801441C8u);
    goto block_801441C8;  /* j */

block_80144158:
    v0 = 0x6666 << 16;  /* 0x66660000 */
    v0 = v0 | 0x6667;
    { int64_t result = (int64_t)(int32_t)s2 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s2));
    v1 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* nop */  /* 0x80144168: 0x00000000 */
    /* nop */  /* 0x8014416C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));
    v1 = (int32_t)v1 >> 1;
    v0 = (int32_t)s2 >> 31;
    s2 = v1 - v0;
    v0 = (int32_t)a1 >> 31;
    a0 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    v1 = (int32_t)a0 >> 1;
    psx_check_interrupts_at(cpu, 0x801441C4u);
    goto block_801441C4;  /* j */

block_801441C4:
    a1 = v1 - v0;
    /* fall through to block_801441C8 */

block_801441C8:
    v0 = s2 << 1;
    v0 = v0 + s2;
    v0 = v0 << 5;
    v0 = v0 + 2048;
    cpu->write_half(s1 + 178, (uint16_t)v0);
    v0 = cpu->read_byte(s4 + 5);
    /* nop */  /* 0x801441E0: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x801441E4: 0x30420010 */
    int _bc_801441E8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = 0x2400 << 16;  /* 0x24000000 */
    if (_bc_801441E8) {
        psx_check_interrupts_at(cpu, 0x801441FCu);
        goto block_801441FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801441F0u);
        goto block_801441F0;  /* not taken */
    }

block_801441F0:
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 256);
    /* delay slot (always executes) */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));
    psx_check_interrupts_at(cpu, 0x80144208u);
    goto block_80144208;  /* j */

block_801441FC:
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 254);
    /* nop */  /* 0x80144200: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));
    /* fall through to block_80144208 */

block_80144208:
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    int _bc_8014420C = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)v0 >> 7;
    if (_bc_8014420C) {
        psx_check_interrupts_at(cpu, 0x8014421Cu);
        goto block_8014421C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144214u);
        goto block_80144214;  /* not taken */
    }

block_80144214:
    v0 = v0 + 127;
    a1 = (int32_t)v0 >> 7;
    /* fall through to block_8014421C */

block_8014421C:
    v0 = cpu->read_byte(s1 + 5);
    /* nop */  /* 0x80144220: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80144224: 0x30420010 */
    int _bc_80144228 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014422C: 0x00000000 */
    if (_bc_80144228) {
        psx_check_interrupts_at(cpu, 0x8014423Cu);
        goto block_8014423C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144230u);
        goto block_80144230;  /* not taken */
    }

block_80144230:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 260);
    /* delay slot (always executes) */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));
    psx_check_interrupts_at(cpu, 0x80144248u);
    goto block_80144248;  /* j */

block_8014423C:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 258);
    /* nop */  /* 0x80144240: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));
    /* fall through to block_80144248 */

block_80144248:
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    int _bc_8014424C = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)v0 >> 7;
    if (_bc_8014424C) {
        psx_check_interrupts_at(cpu, 0x8014425Cu);
        goto block_8014425C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144254u);
        goto block_80144254;  /* not taken */
    }

block_80144254:
    v0 = v0 + 127;
    a1 = (int32_t)v0 >> 7;
    /* fall through to block_8014425C */

block_8014425C:
    v0 = cpu->read_word(s1 + 92);
    v1 = 0x2000 << 16;  /* 0x20000000 */
    v0 = v0 & v1;  /* 0x80144264: 0x00431024 */
    int _bc_80144268 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014426C: 0x00000000 */
    if (_bc_80144268) {
        psx_check_interrupts_at(cpu, 0x80144290u);
        goto block_80144290;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144270u);
        goto block_80144270;  /* not taken */
    }

block_80144270:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x80144274: 0x00000000 */
    v0 = v0 & 0x0100;  /* 0x80144278: 0x30420100 */
    int _bc_8014427C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (uint32_t)a1 >> 31;
    if (_bc_8014427C) {
        psx_check_interrupts_at(cpu, 0x80144290u);
        goto block_80144290;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144284u);
        goto block_80144284;  /* not taken */
    }

block_80144284:
    v0 = a1 + v0;
    /* delay slot (always executes) */
    a1 = (int32_t)v0 >> 1;
    psx_check_interrupts_at(cpu, 0x801442D8u);
    goto block_801442D8;  /* j */

block_80144290:
    v1 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x80144294: 0x00000000 */
    v0 = v1 & 0x0800;  /* 0x80144298: 0x30620800 */
    int _bc_8014429C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0803 << 16;  /* 0x08030000 */
    if (_bc_8014429C) {
        psx_check_interrupts_at(cpu, 0x801442D8u);
        goto block_801442D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801442A4u);
        goto block_801442A4;  /* not taken */
    }

block_801442A4:
    v0 = v1 & v0;  /* 0x801442A4: 0x00621024 */
    int _bc_801442A8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801442AC: 0x00000000 */
    if (_bc_801442A8) {
        psx_check_interrupts_at(cpu, 0x801442D8u);
        goto block_801442D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801442B0u);
        goto block_801442B0;  /* not taken */
    }

block_801442B0:
    v0 = cpu->read_word(s4 + 100);
    /* nop */  /* 0x801442B4: 0x00000000 */
    v0 = v0 & 0x1000;  /* 0x801442B8: 0x30421000 */
    int _bc_801442BC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a1 << 1;
    if (_bc_801442BC) {
        psx_check_interrupts_at(cpu, 0x801442D8u);
        goto block_801442D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801442C4u);
        goto block_801442C4;  /* not taken */
    }

block_801442C4:
    v0 = v0 + a1;
    int _bc_801442C8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)v0 >> 2;
    if (_bc_801442C8) {
        psx_check_interrupts_at(cpu, 0x801442D8u);
        goto block_801442D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801442D0u);
        goto block_801442D0;  /* not taken */
    }

block_801442D0:
    v0 = v0 + 3;
    a1 = (int32_t)v0 >> 2;
    /* fall through to block_801442D8 */

block_801442D8:
    int _bc_801442D8 = (s2 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801442DC: 0x00000000 */
    if (_bc_801442D8) {
        psx_check_interrupts_at(cpu, 0x801442E4u);
        goto block_801442E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801442E0u);
        goto block_801442E0;  /* not taken */
    }

block_801442E0:
    s2 = 1;
    /* fall through to block_801442E4 */

block_801442E4:
    int _bc_801442E4 = (a1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_801442E4) {
        psx_check_interrupts_at(cpu, 0x801442F0u);
        goto block_801442F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801442ECu);
        goto block_801442EC;  /* not taken */
    }

block_801442EC:
    a1 = 1;
    /* fall through to block_801442F0 */

block_801442F0:
    a0 = v0 + 624;
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 34);
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 78);
    /* nop */  /* 0x801442FC: 0x00000000 */
    v0 = v0 + v1;
    v0 = v0 << 20;
    int _bc_80144308 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero - s2;
    if (_bc_80144308) {
        psx_check_interrupts_at(cpu, 0x8014431Cu);
        goto block_8014431C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144310u);
        goto block_80144310;  /* not taken */
    }

block_80144310:
    v0 = v1 << 1;
    /* delay slot (always executes) */
    v0 = v0 + v1;
    psx_check_interrupts_at(cpu, 0x80144324u);
    goto block_80144324;  /* j */

block_8014431C:
    v0 = s2 << 1;
    v0 = v0 + s2;
    /* fall through to block_80144324 */

block_80144324:
    int _bc_80144324 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144328: 0x00000000 */
    if (_bc_80144324) {
        psx_check_interrupts_at(cpu, 0x80144330u);
        goto block_80144330;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014432Cu);
        goto block_8014432C;  /* not taken */
    }

block_8014432C:
    v0 = v0 + 3;
    /* fall through to block_80144330 */

block_80144330:
    v0 = (int32_t)v0 >> 2;
    cpu->write_half(a0 + 134, (uint16_t)v0);
    v0 = -833;
    s0 = s0 & v0;  /* 0x8014433C: 0x02028024 */
    v1 = 0x801F << 16;  /* 0x801F0000 */
    v0 = 4;
    cpu->write_half(v1 + 760, (uint16_t)v0);
    v0 = s2 + 6;
    cpu->write_half(s1 + 194, (uint16_t)v0);
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    v0 = ((int32_t)v0 < 16) ? 1 : 0;  /* 0x8014435C: 0x28420010 */
    int _bc_80144360 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s1 + 200, (uint16_t)a1);
    if (_bc_80144360) {
        psx_check_interrupts_at(cpu, 0x80144370u);
        goto block_80144370;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144368u);
        goto block_80144368;  /* not taken */
    }

block_80144368:
    v0 = 16;
    cpu->write_half(s1 + 194, (uint16_t)v0);
    /* fall through to block_80144370 */

block_80144370:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 194);
    a0 = (int32_t)(int16_t)cpu->read_half(s1 + 194);
    v1 = cpu->read_half(s1 + 194);
    v0 = v0 + -5;
    v0 = v0 << 4;
    cpu->write_half(s1 + 204, (uint16_t)v0);
    v0 = ((int32_t)a0 < 27) ? 1 : 0;  /* 0x80144388: 0x2882001B */
    int _bc_8014438C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a0 < 22) ? 1 : 0;  /* 0x80144390: 0x28820016 */
    if (_bc_8014438C) {
        psx_check_interrupts_at(cpu, 0x801443A0u);
        goto block_801443A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144394u);
        goto block_80144394;  /* not taken */
    }

block_80144394:
    s0 = s0 | 0x0200;
    /* delay slot (always executes) */
    v0 = v1 + -26;
    psx_check_interrupts_at(cpu, 0x801443B8u);
    goto block_801443B8;  /* j */

block_801443A0:
    int _bc_801443A0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -16;
    if (_bc_801443A0) {
        psx_check_interrupts_at(cpu, 0x801443B4u);
        goto block_801443B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801443A8u);
        goto block_801443A8;  /* not taken */
    }

block_801443A8:
    s0 = s0 | 0x0100;
    /* delay slot (always executes) */
    v0 = v1 + -21;
    psx_check_interrupts_at(cpu, 0x801443B8u);
    goto block_801443B8;  /* j */

block_801443B4:
    s0 = s0 | 0x0040;
    /* fall through to block_801443B8 */

block_801443B8:
    cpu->write_half(s1 + 194, (uint16_t)v0);
    a0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x801443C0: 0x00000000 */
    v0 = a0 & 0x0800;  /* 0x801443C4: 0x30820800 */
    int _bc_801443C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0008 << 16;  /* 0x00080000 */
    if (_bc_801443C8) {
        psx_check_interrupts_at(cpu, 0x801443F4u);
        goto block_801443F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801443D0u);
        goto block_801443D0;  /* not taken */
    }

block_801443D0:
    v0 = 0x0803 << 16;  /* 0x08030000 */
    v0 = a0 & v0;  /* 0x801443D4: 0x00821024 */
    int _bc_801443D8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_801443D8) {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801443E0u);
        goto block_801443E0;  /* not taken */
    }

block_801443E0:
    v0 = cpu->read_word(s1 + 92);
    v1 = 0x0004 << 16;  /* 0x00040000 */
    v0 = v0 & v1;  /* 0x801443E8: 0x00431024 */
    int _bc_801443EC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0008 << 16;  /* 0x00080000 */
    if (_bc_801443EC) {
        psx_check_interrupts_at(cpu, 0x801444A0u);
        goto block_801444A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801443F4u);
        goto block_801443F4;  /* not taken */
    }

block_801443F4:
    v0 = s3 & v0;  /* 0x801443F4: 0x02621024 */
    int _bc_801443F8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0400 << 16;  /* 0x04000000 */
    if (_bc_801443F8) {
        psx_check_interrupts_at(cpu, 0x8014442Cu);
        goto block_8014442C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144400u);
        goto block_80144400;  /* not taken */
    }

block_80144400:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x80144404: 0x00000000 */
    v0 = v0 & 0x0080;  /* 0x80144408: 0x30420080 */
    int _bc_8014440C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144410: 0x00000000 */
    if (_bc_8014440C) {
        psx_check_interrupts_at(cpu, 0x80144420u);
        goto block_80144420;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144414u);
        goto block_80144414;  /* not taken */
    }

block_80144414:
    v0 = 0x0080 << 16;  /* 0x00800000 */
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_80144420:
    v0 = 0x0080 << 16;  /* 0x00800000 */
    /* delay slot (always executes) */
    v0 = v0 | 0x0C00;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_8014442C:
    v0 = s3 & v0;  /* 0x8014442C: 0x02621024 */
    int _bc_80144430 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0084 << 16;  /* 0x00840000 */
    if (_bc_80144430) {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144438u);
        goto block_80144438;  /* not taken */
    }

block_80144438:
    v0 = 0x0100 << 16;  /* 0x01000000 */
    v0 = s3 & v0;  /* 0x8014443C: 0x02621024 */
    int _bc_80144440 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0081 << 16;  /* 0x00810000 */
    if (_bc_80144440) {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144448u);
        goto block_80144448;  /* not taken */
    }

block_80144448:
    v0 = 0x0020 << 16;  /* 0x00200000 */
    v0 = s3 & v0;  /* 0x8014444C: 0x02621024 */
    int _bc_80144450 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144450) {
        psx_check_interrupts_at(cpu, 0x80144460u);
        goto block_80144460;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144458u);
        goto block_80144458;  /* not taken */
    }

block_80144458:
    /* delay slot (always executes) */
    v0 = v0 | 0x2400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_80144460:
    v0 = 0x0010 << 16;  /* 0x00100000 */
    v0 = s3 & v0;  /* 0x80144464: 0x02621024 */
    int _bc_80144468 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144468) {
        psx_check_interrupts_at(cpu, 0x80144478u);
        goto block_80144478;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144470u);
        goto block_80144470;  /* not taken */
    }

block_80144470:
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_80144478:
    v0 = 0x0040 << 16;  /* 0x00400000 */
    v0 = s3 & v0;  /* 0x8014447C: 0x02621024 */
    int _bc_80144480 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144484: 0x00000000 */
    if (_bc_80144480) {
        psx_check_interrupts_at(cpu, 0x80144494u);
        goto block_80144494;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144488u);
        goto block_80144488;  /* not taken */
    }

block_80144488:
    v0 = 0x0080 << 16;  /* 0x00800000 */
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_80144494:
    v0 = 0x0080 << 16;  /* 0x00800000 */
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_801444A0:
    v0 = a0 & 0x0100;  /* 0x801444A0: 0x30820100 */
    int _bc_801444A4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0008 << 16;  /* 0x00080000 */
    if (_bc_801444A4) {
        psx_check_interrupts_at(cpu, 0x80144530u);
        goto block_80144530;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801444ACu);
        goto block_801444AC;  /* not taken */
    }

block_801444AC:
    v0 = s3 & v0;  /* 0x801444AC: 0x02621024 */
    cpu->write_half(s1 + 144, (uint16_t)zero);
    int _bc_801444B4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s1 + 146, (uint16_t)zero);
    if (_bc_801444B4) {
        psx_check_interrupts_at(cpu, 0x801444C8u);
        goto block_801444C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801444BCu);
        goto block_801444BC;  /* not taken */
    }

block_801444BC:
    v0 = 0x0080 << 16;  /* 0x00800000 */
    /* delay slot (always executes) */
    v0 = v0 | 0x0C00;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_801444C8:
    v0 = 0x0040 << 16;  /* 0x00400000 */
    v0 = s3 & v0;  /* 0x801444CC: 0x02621024 */
    int _bc_801444D0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_801444D0) {
        psx_check_interrupts_at(cpu, 0x801444E0u);
        goto block_801444E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801444D8u);
        goto block_801444D8;  /* not taken */
    }

block_801444D8:
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_801444E0:
    v0 = 0x0020 << 16;  /* 0x00200000 */
    v0 = s3 & v0;  /* 0x801444E4: 0x02621024 */
    int _bc_801444E8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_801444E8) {
        psx_check_interrupts_at(cpu, 0x801444F8u);
        goto block_801444F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801444F0u);
        goto block_801444F0;  /* not taken */
    }

block_801444F0:
    /* delay slot (always executes) */
    v0 = v0 | 0x2400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_801444F8:
    v0 = 0x0100 << 16;  /* 0x01000000 */
    v0 = s3 & v0;  /* 0x801444FC: 0x02621024 */
    int _bc_80144500 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0081 << 16;  /* 0x00810000 */
    if (_bc_80144500) {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144508u);
        goto block_80144508;  /* not taken */
    }

block_80144508:
    v0 = 0x0400 << 16;  /* 0x04000000 */
    v0 = s3 & v0;  /* 0x8014450C: 0x02621024 */
    int _bc_80144510 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0084 << 16;  /* 0x00840000 */
    if (_bc_80144510) {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144518u);
        goto block_80144518;  /* not taken */
    }

block_80144518:
    v0 = 0x0010 << 16;  /* 0x00100000 */
    v0 = s3 & v0;  /* 0x8014451C: 0x02621024 */
    int _bc_80144520 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144520) {
        psx_check_interrupts_at(cpu, 0x80144750u);
        goto block_80144750;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144528u);
        goto block_80144528;  /* not taken */
    }

block_80144528:
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_80144530:
    v0 = s3 & v0;  /* 0x80144530: 0x02621024 */
    cpu->write_half(s1 + 144, (uint16_t)zero);
    int _bc_80144538 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s1 + 146, (uint16_t)zero);
    if (_bc_80144538) {
        psx_check_interrupts_at(cpu, 0x80144590u);
        goto block_80144590;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144540u);
        goto block_80144540;  /* not taken */
    }

block_80144540:
    v0 = 0x0001 << 16;  /* 0x00010000 */
    v0 = s3 & v0;  /* 0x80144544: 0x02621024 */
    int _bc_80144548 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_80144548) {
        psx_check_interrupts_at(cpu, 0x80144570u);
        goto block_80144570;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144550u);
        goto block_80144550;  /* not taken */
    }

block_80144550:
    v0 = s0 & v0;  /* 0x80144550: 0x02021024 */
    int _bc_80144554 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144554) {
        psx_check_interrupts_at(cpu, 0x80144750u);
        goto block_80144750;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014455Cu);
        goto block_8014455C;  /* not taken */
    }

block_8014455C:
    v0 = s0 & v0;  /* 0x8014455C: 0x02021024 */
    int _bc_80144560 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144560) {
        psx_check_interrupts_at(cpu, 0x801445D8u);
        goto block_801445D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144568u);
        goto block_80144568;  /* not taken */
    }

block_80144568:
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_80144570:
    v0 = s0 & v0;  /* 0x80144570: 0x02021024 */
    int _bc_80144574 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144574) {
        psx_check_interrupts_at(cpu, 0x801445D8u);
        goto block_801445D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014457Cu);
        goto block_8014457C;  /* not taken */
    }

block_8014457C:
    v0 = s0 & v0;  /* 0x8014457C: 0x02021024 */
    int _bc_80144580 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144580) {
        psx_check_interrupts_at(cpu, 0x801445D8u);
        goto block_801445D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144588u);
        goto block_80144588;  /* not taken */
    }

block_80144588:
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_80144590:
    v0 = 0x0040 << 16;  /* 0x00400000 */
    v0 = s3 & v0;  /* 0x80144594: 0x02621024 */
    int _bc_80144598 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_80144598) {
        psx_check_interrupts_at(cpu, 0x801445E8u);
        goto block_801445E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801445A0u);
        goto block_801445A0;  /* not taken */
    }

block_801445A0:
    v0 = s3 & v0;  /* 0x801445A0: 0x02621024 */
    int _bc_801445A4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_801445A4) {
        psx_check_interrupts_at(cpu, 0x801445C0u);
        goto block_801445C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801445ACu);
        goto block_801445AC;  /* not taken */
    }

block_801445AC:
    v0 = s0 & v0;  /* 0x801445AC: 0x02021024 */
    int _bc_801445B0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_801445B0) {
        psx_check_interrupts_at(cpu, 0x80144754u);
        goto block_80144754;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801445B8u);
        goto block_801445B8;  /* not taken */
    }

block_801445B8:
    /* delay slot (always executes) */
    s0 = s0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x80144750u);
    goto block_80144750;  /* j */

block_801445C0:
    v0 = s0 & v0;  /* 0x801445C0: 0x02021024 */
    int _bc_801445C4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_801445C4) {
        psx_check_interrupts_at(cpu, 0x801445B8u);
        goto block_801445B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801445CCu);
        goto block_801445CC;  /* not taken */
    }

block_801445CC:
    v0 = s0 & v0;  /* 0x801445CC: 0x02021024 */
    int _bc_801445D0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_801445D0) {
        psx_check_interrupts_at(cpu, 0x801445E0u);
        goto block_801445E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801445D8u);
        goto block_801445D8;  /* not taken */
    }

block_801445D8:
    /* delay slot (always executes) */
    s0 = s0 | 0x0C00;
    psx_check_interrupts_at(cpu, 0x80144750u);
    goto block_80144750;  /* j */

block_801445E0:
    /* delay slot (always executes) */
    v0 = v0 | 0x4400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_801445E8:
    v0 = 0x0020 << 16;  /* 0x00200000 */
    v0 = s3 & v0;  /* 0x801445EC: 0x02621024 */
    int _bc_801445F0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_801445F0) {
        psx_check_interrupts_at(cpu, 0x8014462Cu);
        goto block_8014462C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801445F8u);
        goto block_801445F8;  /* not taken */
    }

block_801445F8:
    v0 = s3 & v0;  /* 0x801445F8: 0x02621024 */
    int _bc_801445FC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_801445FC) {
        psx_check_interrupts_at(cpu, 0x80144618u);
        goto block_80144618;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144604u);
        goto block_80144604;  /* not taken */
    }

block_80144604:
    v0 = s0 & v0;  /* 0x80144604: 0x02021024 */
    int _bc_80144608 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144608) {
        psx_check_interrupts_at(cpu, 0x80144754u);
        goto block_80144754;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144610u);
        goto block_80144610;  /* not taken */
    }

block_80144610:
    /* delay slot (always executes) */
    s0 = s0 | 0x2400;
    psx_check_interrupts_at(cpu, 0x80144750u);
    goto block_80144750;  /* j */

block_80144618:
    v0 = s0 & v0;  /* 0x80144618: 0x02021024 */
    int _bc_8014461C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_8014461C) {
        psx_check_interrupts_at(cpu, 0x80144610u);
        goto block_80144610;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144624u);
        goto block_80144624;  /* not taken */
    }

block_80144624:
    /* delay slot (always executes) */
    v0 = v0 | 0x2400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_8014462C:
    v0 = 0x0100 << 16;  /* 0x01000000 */
    v0 = s3 & v0;  /* 0x80144630: 0x02621024 */
    int _bc_80144634 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_80144634) {
        psx_check_interrupts_at(cpu, 0x80144670u);
        goto block_80144670;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014463Cu);
        goto block_8014463C;  /* not taken */
    }

block_8014463C:
    v0 = s3 & v0;  /* 0x8014463C: 0x02621024 */
    int _bc_80144640 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_80144640) {
        psx_check_interrupts_at(cpu, 0x8014465Cu);
        goto block_8014465C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144648u);
        goto block_80144648;  /* not taken */
    }

block_80144648:
    v0 = s0 & v0;  /* 0x80144648: 0x02021024 */
    int _bc_8014464C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_8014464C) {
        psx_check_interrupts_at(cpu, 0x80144750u);
        goto block_80144750;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144654u);
        goto block_80144654;  /* not taken */
    }

block_80144654:
    /* delay slot (always executes) */
    v0 = v0 | 0x0400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_8014465C:
    v0 = s0 & v0;  /* 0x8014465C: 0x02021024 */
    int _bc_80144660 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0081 << 16;  /* 0x00810000 */
    if (_bc_80144660) {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144668u);
        goto block_80144668;  /* not taken */
    }

block_80144668:
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    psx_check_interrupts_at(cpu, 0x80144748u);
    goto block_80144748;  /* j */

block_80144670:
    v0 = 0x0400 << 16;  /* 0x04000000 */
    v0 = s3 & v0;  /* 0x80144674: 0x02621024 */
    int _bc_80144678 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_80144678) {
        psx_check_interrupts_at(cpu, 0x801446B4u);
        goto block_801446B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144680u);
        goto block_80144680;  /* not taken */
    }

block_80144680:
    v0 = s3 & v0;  /* 0x80144680: 0x02621024 */
    int _bc_80144684 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_80144684) {
        psx_check_interrupts_at(cpu, 0x801446A0u);
        goto block_801446A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014468Cu);
        goto block_8014468C;  /* not taken */
    }

block_8014468C:
    v0 = s0 & v0;  /* 0x8014468C: 0x02021024 */
    int _bc_80144690 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0004 << 16;  /* 0x00040000 */
    if (_bc_80144690) {
        psx_check_interrupts_at(cpu, 0x80144750u);
        goto block_80144750;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144698u);
        goto block_80144698;  /* not taken */
    }

block_80144698:
    /* delay slot (always executes) */
    v0 = v0 | 0x0400;
    psx_check_interrupts_at(cpu, 0x8014474Cu);
    goto block_8014474C;  /* j */

block_801446A0:
    v0 = s0 & v0;  /* 0x801446A0: 0x02021024 */
    int _bc_801446A4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0084 << 16;  /* 0x00840000 */
    if (_bc_801446A4) {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801446ACu);
        goto block_801446AC;  /* not taken */
    }

block_801446AC:
    /* delay slot (always executes) */
    v0 = 0x0004 << 16;  /* 0x00040000 */
    psx_check_interrupts_at(cpu, 0x80144748u);
    goto block_80144748;  /* j */

block_801446B4:
    v0 = 0x0010 << 16;  /* 0x00100000 */
    v0 = s3 & v0;  /* 0x801446B8: 0x02621024 */
    int _bc_801446BC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_801446BC) {
        psx_check_interrupts_at(cpu, 0x801446E4u);
        goto block_801446E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801446C4u);
        goto block_801446C4;  /* not taken */
    }

block_801446C4:
    v0 = s3 & v0;  /* 0x801446C4: 0x02621024 */
    int _bc_801446C8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_801446C8) {
        psx_check_interrupts_at(cpu, 0x801446DCu);
        goto block_801446DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801446D0u);
        goto block_801446D0;  /* not taken */
    }

block_801446D0:
    v0 = s0 & v0;  /* 0x801446D0: 0x02021024 */
    int _bc_801446D4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_801446D4) {
        psx_check_interrupts_at(cpu, 0x80144754u);
        goto block_80144754;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801446DCu);
        goto block_801446DC;  /* not taken */
    }

block_801446DC:
    /* delay slot (always executes) */
    s0 = s0 | 0x1400;
    psx_check_interrupts_at(cpu, 0x80144750u);
    goto block_80144750;  /* j */

block_801446E4:
    v0 = 0x0080 << 16;  /* 0x00800000 */
    v0 = s3 & v0;  /* 0x801446E8: 0x02621024 */
    int _bc_801446EC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_801446EC) {
        psx_check_interrupts_at(cpu, 0x80144714u);
        goto block_80144714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801446F4u);
        goto block_801446F4;  /* not taken */
    }

block_801446F4:
    v0 = s3 & v0;  /* 0x801446F4: 0x02621024 */
    int _bc_801446F8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_801446F8) {
        psx_check_interrupts_at(cpu, 0x8014470Cu);
        goto block_8014470C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144700u);
        goto block_80144700;  /* not taken */
    }

block_80144700:
    v0 = s0 & v0;  /* 0x80144700: 0x02021024 */
    int _bc_80144704 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144704) {
        psx_check_interrupts_at(cpu, 0x80144754u);
        goto block_80144754;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014470Cu);
        goto block_8014470C;  /* not taken */
    }

block_8014470C:
    /* delay slot (always executes) */
    s0 = s0 | 0x8400;
    psx_check_interrupts_at(cpu, 0x80144750u);
    goto block_80144750;  /* j */

block_80144714:
    v0 = s3 & 0x1000;  /* 0x80144714: 0x32621000 */
    int _bc_80144718 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0003 << 16;  /* 0x00030000 */
    if (_bc_80144718) {
        psx_check_interrupts_at(cpu, 0x80144750u);
        goto block_80144750;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144720u);
        goto block_80144720;  /* not taken */
    }

block_80144720:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x80144724: 0x00000000 */
    v0 = v0 & v1;  /* 0x80144728: 0x00431024 */
    int _bc_8014472C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_8014472C) {
        psx_check_interrupts_at(cpu, 0x80144754u);
        goto block_80144754;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144734u);
        goto block_80144734;  /* not taken */
    }

block_80144734:
    v0 = cpu->read_word(s1 + 92);
    /* nop */  /* 0x80144738: 0x00000000 */
    v0 = v0 & 0x0080;  /* 0x8014473C: 0x30420080 */
    int _bc_80144740 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_80144740) {
        psx_check_interrupts_at(cpu, 0x80144754u);
        goto block_80144754;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144748u);
        goto block_80144748;  /* not taken */
    }

block_80144748:
    v0 = v0 | 0x0400;
    /* fall through to block_8014474C */

block_8014474C:
    s0 = s0 | v0;
    /* fall through to block_80144750 */

block_80144750:
    a0 = 0x0080 << 16;  /* 0x00800000 */
    /* fall through to block_80144754 */

block_80144754:
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 78);
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 116);
    a0 = a0 | 0x0400;
    v0 = v0 + v1;
    v0 = v0 << 20;
    v0 = (int32_t)v0 >> 20;
    cpu->write_half(s1 + 196, (uint16_t)v0);
    v0 = s0 & a0;  /* 0x80144770: 0x02041024 */
    int _bc_80144774 = (v0 != a0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144778: 0x00000000 */
    if (_bc_80144774) {
        psx_check_interrupts_at(cpu, 0x801447B0u);
        goto block_801447B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014477Cu);
        goto block_8014477C;  /* not taken */
    }

block_8014477C:
    v0 = cpu->read_byte(s4 + 219);
    /* nop */  /* 0x80144780: 0x00000000 */
    int _bc_80144784 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 64;
    if (_bc_80144784) {
        psx_check_interrupts_at(cpu, 0x80144794u);
        goto block_80144794;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014478Cu);
        goto block_8014478C;  /* not taken */
    }

block_8014478C:
    /* delay slot (always executes) */
    cpu->write_byte(s4 + 219, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x801447B0u);
    goto block_801447B0;  /* j */

block_80144794:
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 78);
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 198);
    /* nop */  /* 0x8014479C: 0x00000000 */
    v0 = v0 + v1;
    v0 = v0 << 20;
    v0 = (int32_t)v0 >> 20;
    cpu->write_half(s1 + 196, (uint16_t)v0);
    /* fall through to block_801447B0 */

block_801447B0:
    a1 = cpu->read_half(s1 + 192);
    /* nop */  /* 0x801447B4: 0x00000000 */
    a0 = a1 << 16;
    v1 = (int32_t)a0 >> 16;
    int _bc_801447C0 = ((int32_t)v1 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_801447C0) {
        psx_check_interrupts_at(cpu, 0x80144804u);
        goto block_80144804;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801447C8u);
        goto block_801447C8;  /* not taken */
    }

block_801447C8:
    v0 = s0 & v0;  /* 0x801447C8: 0x02021024 */
    int _bc_801447CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (uint32_t)a0 >> 31;
    if (_bc_801447CC) {
        psx_check_interrupts_at(cpu, 0x801447E0u);
        goto block_801447E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801447D4u);
        goto block_801447D4;  /* not taken */
    }

block_801447D4:
    v0 = v1 + v0;
    /* delay slot (always executes) */
    v0 = (int32_t)v0 >> 1;
    psx_check_interrupts_at(cpu, 0x80144800u);
    goto block_80144800;  /* j */

block_801447E0:
    v0 = s2 << 4;
    v0 = a1 + v0;
    cpu->write_half(s1 + 192, (uint16_t)v0);
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    v0 = ((int32_t)v0 < 4801) ? 1 : 0;  /* 0x801447F4: 0x284212C1 */
    int _bc_801447F8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4800;
    if (_bc_801447F8) {
        psx_check_interrupts_at(cpu, 0x80144804u);
        goto block_80144804;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144800u);
        goto block_80144800;  /* not taken */
    }

block_80144800:
    cpu->write_half(s1 + 192, (uint16_t)v0);
    /* fall through to block_80144804 */

block_80144804:
    v0 = cpu->read_byte(s4 + 5);
    /* nop */  /* 0x80144808: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x8014480C: 0x30420010 */
    int _bc_80144810 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144814: 0x00000000 */
    if (_bc_80144810) {
        psx_check_interrupts_at(cpu, 0x80144850u);
        goto block_80144850;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144818u);
        goto block_80144818;  /* not taken */
    }

block_80144818:
    v0 = cpu->read_word(s4 + 108);
    /* nop */  /* 0x8014481C: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x80144820: 0x30428000 */
    int _bc_80144824 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144828: 0x00000000 */
    if (_bc_80144824) {
        psx_check_interrupts_at(cpu, 0x80144850u);
        goto block_80144850;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014482Cu);
        goto block_8014482C;  /* not taken */
    }

block_8014482C:
    v0 = cpu->read_byte(s4 + 218);
    /* nop */  /* 0x80144830: 0x00000000 */
    v0 = (uint32_t)v0 >> 1;
    v1 = v0 << 4;
    v1 = v1 - v0;
    v0 = cpu->read_half(s4 + 216);
    v1 = v1 << 2;
    v0 = v0 + v1;
    cpu->write_half(s4 + 216, (uint16_t)v0);
    /* fall through to block_80144850 */

block_80144850:
    v0 = cpu->read_byte(s1 + 5);
    /* nop */  /* 0x80144854: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80144858: 0x30420010 */
    int _bc_8014485C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x5555 << 16;  /* 0x55550000 */
    if (_bc_8014485C) {
        psx_check_interrupts_at(cpu, 0x801448A4u);
        goto block_801448A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144864u);
        goto block_80144864;  /* not taken */
    }

block_80144864:
    v0 = cpu->read_half(s1 + 200);
    a0 = a0 | 0x5556;
    v0 = v0 << 16;
    v1 = (int32_t)v0 >> 16;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v0 = (int32_t)v0 >> 31;
    t0 = cpu->hi;
    psx_muldiv_stall(cpu);
    v0 = t0 - v0;
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    v1 = v0 << 4;
    v1 = v1 - v0;
    v0 = cpu->read_half(s1 + 216);
    v1 = v1 << 2;
    /* delay slot (always executes) */
    v0 = v0 + v1;
    psx_check_interrupts_at(cpu, 0x801448C0u);
    goto block_801448C0;  /* j */

block_801448A4:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 200);
    /* nop */  /* 0x801448A8: 0x00000000 */
    v1 = v0 << 4;
    v1 = v1 - v0;
    v0 = cpu->read_half(s1 + 216);
    v1 = v1 << 3;
    v0 = v0 - v1;
    /* fall through to block_801448C0 */

block_801448C0:
    cpu->write_half(s1 + 216, (uint16_t)v0);
    s6 = 3;
    a0 = s6;
    ra = 0x801448D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s2;
    psx_check_interrupts_at(cpu, 0x80138B00u);
    cpu->pc = 0x80138B00u; return;  /* CPS jal -> 0x80138B00 */

block_801448D4:
    a0 = fp;
    a3 = s2 | 0x1000;
    a1 = (int32_t)(int16_t)cpu->read_half(s1 + 196);
    a2 = cpu->read_byte(s4 + 219);
    s7 = s7 | 0x0008;
    cpu->write_word(sp + 16, s4);
    a1 = a1 + -1024;
    a1 = a1 << 20;
    a1 = (int32_t)a1 >> 20;
    ra = 0x80144900u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a2 << 4;
    psx_check_interrupts_at(cpu, 0x8013EE40u);
    cpu->pc = 0x8013EE40u; return;  /* CPS jal -> 0x8013EE40 */

block_80144900:
    /* delay slot (always executes) */
    /* nop */  /* 0x80144904: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80144D0Cu);
    goto block_80144D0C;  /* j */

block_80144D0C:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 178);
    /* nop */  /* 0x80144D10: 0x00000000 */
    int _bc_80144D14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 104;
    if (_bc_80144D14) {
        psx_check_interrupts_at(cpu, 0x80144D20u);
        goto block_80144D20;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144D1Cu);
        goto block_80144D1C;  /* not taken */
    }

block_80144D1C:
    cpu->write_half(s1 + 180, (uint16_t)v0);
    /* fall through to block_80144D20 */

block_80144D20:
    v0 = s0 & 0x0800;  /* 0x80144D20: 0x32020800 */
    int _bc_80144D24 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s4;
    if (_bc_80144D24) {
        psx_check_interrupts_at(cpu, 0x80144D44u);
        goto block_80144D44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144D2Cu);
        goto block_80144D2C;  /* not taken */
    }

block_80144D2C:
    v1 = 0x0080 << 16;  /* 0x00800000 */
    v1 = v1 | 0x4000;
    v0 = s0 & v1;  /* 0x80144D34: 0x02031024 */
    int _bc_80144D38 = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0081 << 16;  /* 0x00810000 */
    if (_bc_80144D38) {
        psx_check_interrupts_at(cpu, 0x80144D54u);
        goto block_80144D54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144D40u);
        goto block_80144D40;  /* not taken */
    }

block_80144D40:
    a0 = s4;
    /* fall through to block_80144D44 */

block_80144D44:
    ra = 0x80144D4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    psx_check_interrupts_at(cpu, 0x80144F8Cu);
    cpu->pc = 0x80144F8Cu; return;  /* CPS jal -> 0x80144F8C */

block_80144D4C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80144D50: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80144DACu);
    goto block_80144DAC;  /* j */

block_80144D54:
    v0 = s0 & v1;  /* 0x80144D54: 0x02031024 */
    int _bc_80144D58 = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0080 << 16;  /* 0x00800000 */
    if (_bc_80144D58) {
        psx_check_interrupts_at(cpu, 0x80144D74u);
        goto block_80144D74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144D60u);
        goto block_80144D60;  /* not taken */
    }

block_80144D60:
    a0 = s4;
    ra = 0x80144D6Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    psx_check_interrupts_at(cpu, 0x8014512Cu);
    cpu->pc = 0x8014512Cu; return;  /* CPS jal -> 0x8014512C */

block_80144D6C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80144D70: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80144DACu);
    goto block_80144DAC;  /* j */

block_80144D74:
    v1 = v1 | 0x2000;
    v0 = s0 & v1;  /* 0x80144D78: 0x02031024 */
    int _bc_80144D7C = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0084 << 16;  /* 0x00840000 */
    if (_bc_80144D7C) {
        psx_check_interrupts_at(cpu, 0x80144D98u);
        goto block_80144D98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144D84u);
        goto block_80144D84;  /* not taken */
    }

block_80144D84:
    a0 = s4;
    ra = 0x80144D90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    psx_check_interrupts_at(cpu, 0x801451D4u);
    cpu->pc = 0x801451D4u; return;  /* CPS jal -> 0x801451D4 */

block_80144D90:
    /* delay slot (always executes) */
    /* nop */  /* 0x80144D94: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80144DACu);
    goto block_80144DAC;  /* j */

block_80144D98:
    v0 = s0 & v1;  /* 0x80144D98: 0x02031024 */
    int _bc_80144D9C = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s4;
    if (_bc_80144D9C) {
        psx_check_interrupts_at(cpu, 0x80144DACu);
        goto block_80144DAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144DA4u);
        goto block_80144DA4;  /* not taken */
    }

block_80144DA4:
    ra = 0x80144DACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    psx_check_interrupts_at(cpu, 0x801452D0u);
    cpu->pc = 0x801452D0u; return;  /* CPS jal -> 0x801452D0 */

block_80144DAC:
    v0 = cpu->read_word(s1 + 84);
    /* nop */  /* 0x80144DB0: 0x00000000 */
    v0 = v0 & 0x0020;  /* 0x80144DB4: 0x30420020 */
    int _bc_80144DB8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144DBC: 0x00000000 */
    if (_bc_80144DB8) {
        psx_check_interrupts_at(cpu, 0x80144DF8u);
        goto block_80144DF8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144DC0u);
        goto block_80144DC0;  /* not taken */
    }

block_80144DC0:
    v0 = cpu->read_half(s1 + 178);
    a0 = cpu->read_half(s1 + 152);
    v0 = v0 << 16;
    v1 = (int32_t)v0 >> 16;
    v0 = (uint32_t)v0 >> 31;
    v1 = v1 + v0;
    v1 = (int32_t)v1 >> 1;
    a0 = a0 << 16;
    v0 = (int32_t)a0 >> 16;
    a0 = (uint32_t)a0 >> 31;
    v0 = v0 + a0;
    v0 = (int32_t)v0 >> 1;
    cpu->write_half(s1 + 178, (uint16_t)v1);
    cpu->write_half(s1 + 152, (uint16_t)v0);
    /* fall through to block_80144DF8 */

block_80144DF8:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 178);
    /* nop */  /* 0x80144DFC: 0x00000000 */
    int _bc_80144E00 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144E04: 0x00000000 */
    if (_bc_80144E00) {
        psx_check_interrupts_at(cpu, 0x80144F0Cu);
        goto block_80144F0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144E08u);
        goto block_80144E08;  /* not taken */
    }

block_80144E08:
    int _bc_80144E08 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144E0C: 0x00000000 */
    if (_bc_80144E08) {
        psx_check_interrupts_at(cpu, 0x80144E14u);
        goto block_80144E14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144E10u);
        goto block_80144E10;  /* not taken */
    }

block_80144E10:
    cpu->write_half(s1 + 178, (uint16_t)zero);
    /* fall through to block_80144E14 */

block_80144E14:
    v0 = cpu->read_word(s1 + 108);
    /* nop */  /* 0x80144E18: 0x00000000 */
    v0 = v0 & 0x00F0;  /* 0x80144E1C: 0x304200F0 */
    int _bc_80144E20 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80144E24: 0x00000000 */
    if (_bc_80144E20) {
        psx_check_interrupts_at(cpu, 0x80144EC0u);
        goto block_80144EC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144E28u);
        goto block_80144E28;  /* not taken */
    }

block_80144E28:
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 78);
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 112);
    /* nop */  /* 0x80144E30: 0x00000000 */
    v0 = v0 + v1;
    v0 = v0 << 20;
    v0 = (int32_t)v0 >> 20;
    cpu->write_half(s4 + 196, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 180);
    v1 = 0x5555 << 16;  /* 0x55550000 */
    cpu->write_half(s4 + 180, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 178);
    v1 = v1 | 0x5556;
    v0 = v0 << 1;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    v0 = (int32_t)v0 >> 31;
    v1 = cpu->hi;
    psx_muldiv_stall(cpu);
    v0 = v1 - v0;
    cpu->write_half(s4 + 178, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 178);
    /* nop */  /* 0x80144E74: 0x00000000 */
    v0 = v0 << 16;
    v1 = (int32_t)v0 >> 16;
    v0 = (uint32_t)v0 >> 31;
    v1 = v1 + v0;
    v1 = (int32_t)v1 >> 1;
    cpu->write_half(s1 + 178, (uint16_t)v1);
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 178);
    /* nop */  /* 0x80144E94: 0x00000000 */
    v0 = v1 << 5;
    v0 = v0 - v1;
    v0 = v0 << 2;
    v0 = v0 + v1;
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 36);
    v0 = v0 << 2;
    if (v1 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)v1); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)v1); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
    psx_muldiv_set(cpu, 37u);
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    /* nop */  /* 0x80144EB8: 0x00000000 */
    cpu->write_half(s4 + 178, (uint16_t)v0);
    /* fall through to block_80144EC0 */

block_80144EC0:
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 178);
    /* nop */  /* 0x80144EC4: 0x00000000 */
    v0 = v1 << 5;
    v0 = v0 - v1;
    v0 = v0 << 2;
    v0 = v0 + v1;
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 36);
    v0 = v0 << 2;
    if (v1 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)v1); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)v1); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
    psx_muldiv_set(cpu, 37u);
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    /* nop */  /* 0x80144EE8: 0x00000000 */
    cpu->write_half(s1 + 178, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 178);
    /* nop */  /* 0x80144EF4: 0x00000000 */
    v0 = ((int32_t)v0 < 24577) ? 1 : 0;  /* 0x80144EF8: 0x28426001 */
    int _bc_80144EFC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x2000 << 16;  /* 0x20000000 */
    if (_bc_80144EFC) {
        psx_check_interrupts_at(cpu, 0x80144F10u);
        goto block_80144F10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144F04u);
        goto block_80144F04;  /* not taken */
    }

block_80144F04:
    v0 = 24576;
    cpu->write_half(s1 + 178, (uint16_t)v0);
    /* fall through to block_80144F0C */

block_80144F0C:
    v0 = 0x2000 << 16;  /* 0x20000000 */
    /* fall through to block_80144F10 */

block_80144F10:
    v0 = s3 & v0;  /* 0x80144F10: 0x02621024 */
    int _bc_80144F14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0400 << 16;  /* 0x04000000 */
    if (_bc_80144F14) {
        psx_check_interrupts_at(cpu, 0x80144F30u);
        goto block_80144F30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144F1Cu);
        goto block_80144F1C;  /* not taken */
    }

block_80144F1C:
    v0 = s5 & v0;  /* 0x80144F1C: 0x02A21024 */
    int _bc_80144F20 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0008 << 16;  /* 0x00080000 */
    if (_bc_80144F20) {
        psx_check_interrupts_at(cpu, 0x80144F30u);
        goto block_80144F30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80144F28u);
        goto block_80144F28;  /* not taken */
    }

block_80144F28:
    s0 = s0 | v0;
    cpu->write_half(s1 + 178, (uint16_t)zero);
    /* fall through to block_80144F30 */

block_80144F30:
    v1 = 0xF9FF << 16;  /* 0xF9FF0000 */
    v0 = cpu->read_word(s1 + 92);
    v1 = v1 | 0xFFFF;
    cpu->write_word(s1 + 104, s0);
    v0 = v0 & v1;  /* 0x80144F40: 0x00431024 */
    v0 = v0 | s5;
    cpu->write_word(s1 + 92, v0);
    v1 = cpu->read_word(s4 + 92);
    v0 = s6;
    v1 = v1 | s7;
    cpu->write_word(s4 + 92, v1);
    /* fall through to block_80144F5C */

block_80144F5C:
    ra = cpu->read_word(sp + 60);
    fp = cpu->read_word(sp + 56);
    s7 = cpu->read_word(sp + 52);
    s6 = cpu->read_word(sp + 48);
    s5 = cpu->read_word(sp + 44);
    s4 = cpu->read_word(sp + 40);
    s3 = cpu->read_word(sp + 36);
    s2 = cpu->read_word(sp + 32);
    s1 = cpu->read_word(sp + 28);
    s0 = cpu->read_word(sp + 24);
    uint32_t _jt_80144F84 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 64;
    psx_check_interrupts_at(cpu, _jt_80144F84);
    cpu->pc = _jt_80144F84; return;  /* CPS: jr $ra */
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

block_80148234:
    int _bc_80148234 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    if (_bc_80148234) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014823Cu);
        goto block_8014823C;  /* not taken */
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

block_80148278:
    int _bc_80148278 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014827C: 0x00000000 */
    if (_bc_80148278) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80148280u);
        goto block_80148280;  /* not taken */
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

block_801482B0:
    int _bc_801482B0 = (s4 != s2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (_bc_801482B0) {
        psx_check_interrupts_at(cpu, 0x801482E8u);
        goto block_801482E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801482B8u);
        goto block_801482B8;  /* not taken */
    }

block_801482B8:
    int _bc_801482B8 = (s2 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    if (_bc_801482B8) {
        psx_check_interrupts_at(cpu, 0x801482E8u);
        goto block_801482E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801482C0u);
        goto block_801482C0;  /* not taken */
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
    int _bc_80148308 = (s2 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (_bc_80148308) {
        psx_check_interrupts_at(cpu, 0x80148318u);
        goto block_80148318;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80148310u);
        goto block_80148310;  /* not taken */
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

block_80148334:
    int _bc_80148334 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80148338: 0x00000000 */
    if (_bc_80148334) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014833Cu);
        goto block_8014833C;  /* not taken */
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

block_8014838C:
    int _bc_8014838C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80148390: 0x00000000 */
    if (_bc_8014838C) {
        psx_check_interrupts_at(cpu, 0x801484BCu);
        goto block_801484BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80148394u);
        goto block_80148394;  /* not taken */
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
    uint32_t _jt_801484E0 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 64;
    psx_check_interrupts_at(cpu, _jt_801484E0);
    cpu->pc = _jt_801484E0; return;  /* CPS: jr $ra */
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
    int _bc_80150F54 = (a0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_80150F54) {
        psx_check_interrupts_at(cpu, 0x80150FA0u);
        goto block_80150FA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150F5Cu);
        goto block_80150F5C;  /* not taken */
    }

block_80150F5C:
    a2 = v0 + 1772;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s0 + 34);
    v1 = v1 + -176;
    v0 = (uint32_t)v1 >> 31;
    v1 = v1 + v0;
    v1 = v1 << 15;
    int _bc_80150F78 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (_bc_80150F78) {
        psx_check_interrupts_at(cpu, 0x80150F84u);
        goto block_80150F84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150F80u);
        goto block_80150F80;  /* not taken */
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
    int _bc_80150FB4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    if (_bc_80150FB4) {
        psx_check_interrupts_at(cpu, 0x80151004u);
        goto block_80151004;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150FBCu);
        goto block_80150FBC;  /* not taken */
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
    int _bc_80150FDC = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (_bc_80150FDC) {
        psx_check_interrupts_at(cpu, 0x80150FE8u);
        goto block_80150FE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80150FE4u);
        goto block_80150FE4;  /* not taken */
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
    int _bc_80151028 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_80151028) {
        psx_check_interrupts_at(cpu, 0x80151074u);
        goto block_80151074;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151030u);
        goto block_80151030;  /* not taken */
    }

block_80151030:
    a2 = v0 + 1812;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s0 + 34);
    v1 = v1 + -144;
    v0 = (uint32_t)v1 >> 31;
    v1 = v1 + v0;
    v1 = v1 << 15;
    int _bc_8015104C = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (_bc_8015104C) {
        psx_check_interrupts_at(cpu, 0x80151058u);
        goto block_80151058;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151054u);
        goto block_80151054;  /* not taken */
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
    int _bc_8015107C = (t0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    if (_bc_8015107C) {
        psx_check_interrupts_at(cpu, 0x801510CCu);
        goto block_801510CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151084u);
        goto block_80151084;  /* not taken */
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
    int _bc_801510A4 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v1 >> 16;
    if (_bc_801510A4) {
        psx_check_interrupts_at(cpu, 0x801510B0u);
        goto block_801510B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801510ACu);
        goto block_801510AC;  /* not taken */
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
    int _bc_801510F8 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)v0 >> 16;
    if (_bc_801510F8) {
        psx_check_interrupts_at(cpu, 0x80151104u);
        goto block_80151104;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80151100u);
        goto block_80151100;  /* not taken */
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
    uint32_t _jt_80151134 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 56;
    psx_check_interrupts_at(cpu, _jt_80151134);
    cpu->pc = _jt_80151134; return;  /* CPS: jr $ra */
}

/* cmd_input_parse_cps — CPS re-entry symbol for every continuation/branch-target addr of
 * the cmd_input_parse_body region (game/clean_cps.map). cpu->pc == dispatched addr. */
void cmd_input_parse_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cmd_input_parse_body(cpu, cpu->pc);
}

/* hit_react_apply_cps — CPS re-entry symbol for every continuation/branch-target addr of
 * the hit_react_apply_body region (game/clean_cps.map). cpu->pc == dispatched addr. */
void hit_react_apply_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  hit_react_apply_body(cpu, cpu->pc);
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

/* 0x80144034 — alias piece-entry into hit_react_apply_body (see game/renames.map). */
void hit_react_apply_alt_4034(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  hit_react_apply_body(cpu, 0x80144034u);
}

/* 0x80144054 — alias piece-entry into hit_react_apply_body (see game/renames.map). */
void hit_react_apply_alt_4054(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  hit_react_apply_body(cpu, 0x80144054u);
}

/* 0x80144070 — alias piece-entry into hit_react_apply_body (see game/renames.map). */
void hit_react_apply_alt_4070(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  hit_react_apply_body(cpu, 0x80144070u);
}

/* 0x80144094 — alias piece-entry into hit_react_apply_body (see game/renames.map). */
void hit_react_apply_alt_4094(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  hit_react_apply_body(cpu, 0x80144094u);
}

/* 0x80144114 — alias piece-entry into hit_react_apply_body (see game/renames.map). */
void hit_react_apply_alt_4114(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  hit_react_apply_body(cpu, 0x80144114u);
}

/* 0x80144158 — alias piece-entry into hit_react_apply_body (see game/renames.map). */
void hit_react_apply_alt_4158(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  cpu->pc = 0;  /* fresh-entry semantics: skip _cont switch, entry switch routes */
  hit_react_apply_body(cpu, 0x80144158u);
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
