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

/* ======== battle_intro_fsm_alias (alias body guest 0x80137098) ======== */

/* Overlapping-alias body for br2eu_func_80137098 (battle_intro_fsm @0x80137098): jump-table/continuation entries 0x8013738C/0x80137394/0x801373B0/0x801373BC plus 25 CPS blocks. Faithful CPS transliteration of generated/SLES_017.22_full_07.c br2eu_psx_alias_body_80137098.
 * cpu->pc dispatch: continuation PCs hit the _cont switch; alias entry
 * addrs fall through to the entry switch (entry = cpu->pc). */
void battle_intro_fsm_alias(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t entry;
  if (!cpu) return;
  entry = cpu->pc;
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801373A8u: goto block_801373A8;
            case 0x801373BCu: goto block_801373BC;
            case 0x80137400u: goto block_80137400;
            case 0x80137424u: goto block_80137424;
            case 0x80137448u: goto block_80137448;
            case 0x801374F4u: goto block_801374F4;
            case 0x80137508u: goto block_80137508;
            case 0x80137514u: goto block_80137514;
            case 0x8013752Cu: goto block_8013752C;
            case 0x801373B4u: goto block_801373B4;
            case 0x801373C0u: goto block_801373C0;
            case 0x801373CCu: goto block_801373CC;
            case 0x801373E8u: goto block_801373E8;
            case 0x801373F0u: goto block_801373F0;
            case 0x80137408u: goto block_80137408;
            case 0x8013742Cu: goto block_8013742C;
            case 0x8013743Cu: goto block_8013743C;
            case 0x8013744Cu: goto block_8013744C;
            case 0x8013745Cu: goto block_8013745C;
            case 0x80137480u: goto block_80137480;
            case 0x801374D0u: goto block_801374D0;
            case 0x80137548u: goto block_80137548;
            default: break;
        }
    }
    switch (entry) {
        case 0x8013738Cu: goto block_8013738C;
        case 0x80137394u: goto block_80137394;
        case 0x801373B0u: goto block_801373B0;
        case 0x801373BCu: goto block_801373BC;
        default: return;
    }

block_8013738C:
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x801373B4u);
    goto block_801373B4;  /* j */

block_80137394:
    a0 = 2;
    a1 = 1;
    a2 = 3;
    ra = 0x801373A8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = a1;
    psx_check_interrupts_at(cpu, 0x801AD38Cu);
    cpu->pc = 0x801AD38Cu; return;  /* CPS jal -> 0x801AD38C */

block_801373A8:
    /* delay slot (always executes) */
    s1 = zero;
    psx_check_interrupts_at(cpu, 0x801373C0u);
    goto block_801373C0;  /* j */

block_801373B0:
    a0 = 6;
    /* fall through to block_801373B4 */

block_801373B4:
    ra = 0x801373BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801373B8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801385DCu);
    cpu->pc = 0x801385DCu; return;  /* CPS jal -> 0x801385DC */

block_801373BC:
    s1 = zero;
    /* fall through to block_801373C0 */

block_801373C0:
    v0 = 0x801F << 16;  /* 0x801F0000 */
    s2 = v0 + 1872;
    s3 = s1;
    /* fall through to block_801373CC */

block_801373CC:
    v0 = 0x1F80 << 16;  /* 0x1F800000 */
    v0 = v0 | 0x00E8;
    s0 = s3 + v0;
    v1 = cpu->read_byte(s0 + 3);
    v0 = 4;
    int cond_v1_eq_v0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (cond_v1_eq_v0) {
        psx_check_interrupts_at(cpu, 0x8013742Cu);
        goto block_8013742C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801373E8u);
    }

block_801373E8:;
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80137448u);
        goto block_80137448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801373F0u);
    }

block_801373F0:
    a1 = a0;
    a2 = s1 + a0;
    ra = 0x80137400u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = v0;
    psx_check_interrupts_at(cpu, 0x801AD38Cu);
    cpu->pc = 0x801AD38Cu; return;  /* CPS jal -> 0x801AD38C */

block_80137400:;
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s2, v0);
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80137448u);
        goto block_80137448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80137408u);
    }

block_80137408:
    a1 = s0;
    a2 = cpu->read_word(a1 + 32);
    a3 = zero;
    cpu->write_word(sp + 16, zero);
    a0 = cpu->read_word(s2);
    ra = 0x80137424u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a2 + 272;
    psx_check_interrupts_at(cpu, 0x801AD260u);
    cpu->pc = 0x801AD260u; return;  /* CPS jal -> 0x801AD260 */

block_80137424:
    /* delay slot (always executes) */
    s2 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x8013744Cu);
    goto block_8013744C;  /* j */

block_8013742C:
    v1 = cpu->read_byte(s0 + 4);
    v0 = 2;
    int cond_v1_ne_v0_2 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    if (cond_v1_ne_v0_2) {
        psx_check_interrupts_at(cpu, 0x80137448u);
        goto block_80137448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013743Cu);
    }

block_8013743C:
    a1 = 0x0FFF << 16;  /* 0x0FFF0000 */
    ra = 0x80137448u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 | 0xFFFF;
    psx_check_interrupts_at(cpu, 0x8013B5A8u);
    cpu->pc = 0x8013B5A8u; return;  /* CPS jal -> 0x8013B5A8 */

block_80137448:
    s2 = s2 + 4;
    /* fall through to block_8013744C */

block_8013744C:
    s1 = s1 + 1;
    v0 = ((int32_t)s1 < 2) ? 1 : 0;  /* 0x80137450: 0x2A220002 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s3 = s3 + 264;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801373CCu);
        goto block_801373CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013745Cu);
    }

block_8013745C:
    s1 = zero;
    v0 = 0x801C << 16;  /* 0x801C0000 */
    v0 = v0 + 17320;
    a3 = v0;
    v0 = 0x801F << 16;  /* 0x801F0000 */
    a1 = v0 + 1936;
    v0 = 0x801F << 16;  /* 0x801F0000 */
    a0 = v0 + 1920;
    a2 = cpu->read_word(a3 + 352);
    /* fall through to block_80137480 */

block_80137480:
    /* nop */  /* 0x80137480: 0x00000000 */
    v0 = cpu->read_word(a2);
    /* nop */  /* 0x80137488: 0x00000000 */
    cpu->write_word(a0, v0);
    v0 = cpu->read_word(a2 + 16);
    s1 = s1 + 1;
    cpu->write_word(a1, v0);
    v0 = cpu->read_word(a0);
    v1 = cpu->read_word(a3 + 352);
    a2 = a2 + 4;
    v0 = v0 + v1;
    cpu->write_word(a0, v0);
    v0 = cpu->read_word(a1);
    v1 = cpu->read_word(a3 + 352);
    a0 = a0 + 4;
    v0 = v0 + v1;
    cpu->write_word(a1, v0);
    v0 = ((int32_t)s1 < 4) ? 1 : 0;  /* 0x801374C4: 0x2A220004 */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 4;
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80137480u);
        goto block_80137480;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801374D0u);
    }

block_801374D0:
    a0 = 1;
    a1 = -1;
    a2 = a1;
    a3 = a1;
    v0 = 0x801F << 16;  /* 0x801F0000 */
    cpu->write_byte(v0 + 2776, (uint8_t)zero);
    v0 = v0 + 2776;
    ra = 0x801374F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(v0 + 1, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x8012EA18u);
    cpu->pc = 0x8012EA18u; return;  /* CPS jal -> 0x8012EA18 */

block_801374F4:
    a0 = 2;
    a1 = -1;
    a2 = a1;
    ra = 0x80137508u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = a1;
    psx_check_interrupts_at(cpu, 0x8012EA18u);
    cpu->pc = 0x8012EA18u; return;  /* CPS jal -> 0x8012EA18 */

block_80137508:
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    ra = 0x80137514u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 | 0x00E8;
    psx_check_interrupts_at(cpu, 0x80172A04u);
    cpu->pc = 0x80172A04u; return;  /* CPS jal -> 0x80172A04 */

block_80137514:
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    a0 = a0 | 0x01F0;
    s0 = 0x801F << 16;  /* 0x801F0000 */
    cpu->write_word(s0 + 1880, v0);
    ra = 0x8013752Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = s0 + 1880;
    psx_check_interrupts_at(cpu, 0x80172A04u);
    cpu->pc = 0x80172A04u; return;  /* CPS jal -> 0x80172A04 */

block_8013752C:
    cpu->write_word(s0 + 4, v0);
    v0 = 0x801F << 16;  /* 0x801F0000 */
    cpu->write_word(v0 + 2368, zero);
    v0 = v0 + 2368;
    cpu->write_word(v0 + 4, zero);
    v0 = 0x801F << 16;  /* 0x801F0000 */
    cpu->write_word(v0 + 2352, zero);
    /* fall through to block_80137548 */

block_80137548:
    ra = cpu->read_word(sp + 52);
    s6 = cpu->read_word(sp + 48);
    s5 = cpu->read_word(sp + 44);
    s4 = cpu->read_word(sp + 40);
    s3 = cpu->read_word(sp + 36);
    s2 = cpu->read_word(sp + 32);
    s1 = cpu->read_word(sp + 28);
    s0 = cpu->read_word(sp + 24);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 56;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== impact_fx_type_alias (alias body guest 0x80138CEC) ======== */

/* Overlapping-alias body for br2eu_func_80138CEC (impact_fx_update @0x80138CEC): 15 computed-goto type entries (0x80138D54..0x80139DE8) plus 108 CPS blocks. Faithful CPS transliteration of generated/SLES_017.22_full_08.c br2eu_psx_alias_body_80138CEC.
 * cpu->pc dispatch: continuation PCs hit the _cont switch; alias entry
 * addrs fall through to the entry switch (entry = cpu->pc). */
void impact_fx_type_alias(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t entry;
  if (!cpu) return;
  entry = cpu->pc;
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80138D6Cu: goto block_80138D6C;
            case 0x80138FC4u: goto block_80138FC4;
            case 0x8013946Cu: goto block_8013946C;
            case 0x80139BC4u: goto block_80139BC4;
            case 0x80139C00u: goto block_80139C00;
            case 0x80139C3Cu: goto block_80139C3C;
            case 0x80139C78u: goto block_80139C78;
            case 0x80139CB4u: goto block_80139CB4;
            case 0x80139CF0u: goto block_80139CF0;
            case 0x80139D2Cu: goto block_80139D2C;
            case 0x80139D68u: goto block_80139D68;
            case 0x80138D64u: goto block_80138D64;
            case 0x80138D84u: goto block_80138D84;
            case 0x80138DA0u: goto block_80138DA0;
            case 0x80138DCCu: goto block_80138DCC;
            case 0x80138DD4u: goto block_80138DD4;
            case 0x80138DD8u: goto block_80138DD8;
            case 0x80138E04u: goto block_80138E04;
            case 0x80138E0Cu: goto block_80138E0C;
            case 0x80138E10u: goto block_80138E10;
            case 0x80138E3Cu: goto block_80138E3C;
            case 0x80138E54u: goto block_80138E54;
            case 0x80138E70u: goto block_80138E70;
            case 0x80138E9Cu: goto block_80138E9C;
            case 0x80138EA4u: goto block_80138EA4;
            case 0x80138EA8u: goto block_80138EA8;
            case 0x80138ED4u: goto block_80138ED4;
            case 0x80138EDCu: goto block_80138EDC;
            case 0x80138EE0u: goto block_80138EE0;
            case 0x80138F0Cu: goto block_80138F0C;
            case 0x80138F14u: goto block_80138F14;
            case 0x80138F24u: goto block_80138F24;
            case 0x80138F2Cu: goto block_80138F2C;
            case 0x80138F8Cu: goto block_80138F8C;
            case 0x801390D0u: goto block_801390D0;
            case 0x801390D8u: goto block_801390D8;
            case 0x801390DCu: goto block_801390DC;
            case 0x8013914Cu: goto block_8013914C;
            case 0x80139154u: goto block_80139154;
            case 0x80139158u: goto block_80139158;
            case 0x801391D4u: goto block_801391D4;
            case 0x801391ECu: goto block_801391EC;
            case 0x801392DCu: goto block_801392DC;
            case 0x801392E4u: goto block_801392E4;
            case 0x801392E8u: goto block_801392E8;
            case 0x80139358u: goto block_80139358;
            case 0x80139360u: goto block_80139360;
            case 0x80139364u: goto block_80139364;
            case 0x8013937Cu: goto block_8013937C;
            case 0x801393CCu: goto block_801393CC;
            case 0x801393D0u: goto block_801393D0;
            case 0x801393E8u: goto block_801393E8;
            case 0x801393ECu: goto block_801393EC;
            case 0x80139404u: goto block_80139404;
            case 0x80139408u: goto block_80139408;
            case 0x80139434u: goto block_80139434;
            case 0x80139450u: goto block_80139450;
            case 0x801394B8u: goto block_801394B8;
            case 0x801394BCu: goto block_801394BC;
            case 0x801394D4u: goto block_801394D4;
            case 0x801394D8u: goto block_801394D8;
            case 0x801394F0u: goto block_801394F0;
            case 0x801394F4u: goto block_801394F4;
            case 0x80139514u: goto block_80139514;
            case 0x80139570u: goto block_80139570;
            case 0x801395D4u: goto block_801395D4;
            case 0x801395D8u: goto block_801395D8;
            case 0x801395E8u: goto block_801395E8;
            case 0x80139608u: goto block_80139608;
            case 0x8013960Cu: goto block_8013960C;
            case 0x801397ACu: goto block_801397AC;
            case 0x801397B4u: goto block_801397B4;
            case 0x801397B8u: goto block_801397B8;
            case 0x8013981Cu: goto block_8013981C;
            case 0x80139824u: goto block_80139824;
            case 0x80139828u: goto block_80139828;
            case 0x80139834u: goto block_80139834;
            case 0x80139874u: goto block_80139874;
            case 0x8013998Cu: goto block_8013998C;
            case 0x80139994u: goto block_80139994;
            case 0x80139998u: goto block_80139998;
            case 0x801399FCu: goto block_801399FC;
            case 0x80139A04u: goto block_80139A04;
            case 0x80139A08u: goto block_80139A08;
            case 0x80139A0Cu: goto block_80139A0C;
            case 0x80139AB8u: goto block_80139AB8;
            case 0x80139AC8u: goto block_80139AC8;
            case 0x80139AECu: goto block_80139AEC;
            case 0x80139AF0u: goto block_80139AF0;
            case 0x80139B78u: goto block_80139B78;
            case 0x80139BACu: goto block_80139BAC;
            case 0x80139BB8u: goto block_80139BB8;
            case 0x80139D98u: goto block_80139D98;
            default: break;
        }
    }
    switch (entry) {
        case 0x80138D54u: goto block_80138D54;
        case 0x80138D74u: goto block_80138D74;
        case 0x80138E44u: goto block_80138E44;
        case 0x80138F18u: goto block_80138F18;
        case 0x80138F48u: goto block_80138F48;
        case 0x80139160u: goto block_80139160;
        case 0x8013936Cu: goto block_8013936C;
        case 0x80139424u: goto block_80139424;
        case 0x80139504u: goto block_80139504;
        case 0x80139560u: goto block_80139560;
        case 0x80139620u: goto block_80139620;
        case 0x80139664u: goto block_80139664;
        case 0x80139830u: goto block_80139830;
        case 0x80139A74u: goto block_80139A74;
        case 0x80139DE8u: goto block_80139DE8;
        default: return;
    }

block_80138D54:
    v0 = cpu->read_word(s1 + 80);
    /* nop */  /* 0x80138D58: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D60: 0x00000000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D64u);
    }

block_80138D64:
    ra = 0x80138D6Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D68: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A482Cu);
    cpu->pc = 0x801A482Cu; return;  /* CPS jal -> 0x801A482C */

block_80138D6C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D70: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80139DE8u);
    goto block_80139DE8;  /* j */

block_80138D74:
    v0 = cpu->read_half(s4 + 130);
    v1 = cpu->read_word(s1 + 96);
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D80: 0x00000000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D84u);
    }

block_80138D84:
    v0 = cpu->read_half(v1);
    /* nop */  /* 0x80138D88: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(v1, (uint16_t)v0);
    v0 = v0 << 16;
    int cond_v0_le_0 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D9C: 0x00000000 */
    if (cond_v0_le_0) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DA0u);
    }

block_80138DA0:
    v0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138DA4: 0x00000000 */
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = v1 << 3;
    v1 = s1 + v1;
    a0 = cpu->read_byte(v1 + 4);
    /* nop */  /* 0x80138DBC: 0x00000000 */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x80138DC0: 0x2C820008 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80138DD4u);
        goto block_80138DD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DCCu);
    }

block_80138DCC:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 4, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80138DD8u);
    goto block_80138DD8;  /* j */

block_80138DD4:
    cpu->write_byte(v1 + 4, (uint8_t)v0);
    /* fall through to block_80138DD8 */

block_80138DD8:
    v0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138DDC: 0x00000000 */
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = v1 << 3;
    v1 = s1 + v1;
    a0 = cpu->read_byte(v1 + 5);
    /* nop */  /* 0x80138DF4: 0x00000000 */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x80138DF8: 0x2C820008 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80138E0Cu);
        goto block_80138E0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E04u);
    }

block_80138E04:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 5, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80138E10u);
    goto block_80138E10;  /* j */

block_80138E0C:
    cpu->write_byte(v1 + 5, (uint8_t)v0);
    /* fall through to block_80138E10 */

block_80138E10:
    v0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138E14: 0x00000000 */
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = v1 << 3;
    v1 = s1 + v1;
    a0 = cpu->read_byte(v1 + 6);
    /* nop */  /* 0x80138E2C: 0x00000000 */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x80138E30: 0x2C820008 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (cond_v0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
        goto block_80138F0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E3Cu);
    }

block_80138E3C:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 6, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80138F18u);
    goto block_80138F18;  /* j */

block_80138E44:
    v0 = cpu->read_half(s4 + 130);
    v1 = cpu->read_word(s1 + 96);
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E50: 0x00000000 */
    if (cond_v0_ne_zero_4) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E54u);
    }

block_80138E54:
    v0 = cpu->read_half(v1 + 8);
    /* nop */  /* 0x80138E58: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(v1 + 8, (uint16_t)v0);
    v0 = v0 << 16;
    int cond_v0_le_0_2 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E6C: 0x00000000 */
    if (cond_v0_le_0_2) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E70u);
    }

block_80138E70:
    v0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138E74: 0x00000000 */
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = v1 << 3;
    v1 = s1 + v1;
    a0 = cpu->read_byte(v1 + 4);
    /* nop */  /* 0x80138E8C: 0x00000000 */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x80138E90: 0x2C820008 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x80138EA4u);
        goto block_80138EA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E9Cu);
    }

block_80138E9C:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 4, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80138EA8u);
    goto block_80138EA8;  /* j */

block_80138EA4:
    cpu->write_byte(v1 + 4, (uint8_t)v0);
    /* fall through to block_80138EA8 */

block_80138EA8:
    v0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138EAC: 0x00000000 */
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = v1 << 3;
    v1 = s1 + v1;
    a0 = cpu->read_byte(v1 + 5);
    /* nop */  /* 0x80138EC4: 0x00000000 */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x80138EC8: 0x2C820008 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x80138EDCu);
        goto block_80138EDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138ED4u);
    }

block_80138ED4:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 5, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80138EE0u);
    goto block_80138EE0;  /* j */

block_80138EDC:
    cpu->write_byte(v1 + 5, (uint8_t)v0);
    /* fall through to block_80138EE0 */

block_80138EE0:
    v0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138EE4: 0x00000000 */
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = v1 << 3;
    v1 = s1 + v1;
    a0 = cpu->read_byte(v1 + 6);
    /* nop */  /* 0x80138EFC: 0x00000000 */
    v0 = (a0 < (uint32_t)8) ? 1 : 0;  /* 0x80138F00: 0x2C820008 */
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (cond_v0_eq_zero_5) {
        psx_check_interrupts_at(cpu, 0x80138F14u);
        goto block_80138F14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
    }

block_80138F0C:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 6, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80138F18u);
    goto block_80138F18;  /* j */

block_80138F14:
    cpu->write_byte(v1 + 6, (uint8_t)v0);
    /* fall through to block_80138F18 */

block_80138F18:
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 100);
    /* nop */  /* 0x80138F1C: 0x00000000 */
    v0 = (v1 < (uint32_t)19) ? 1 : 0;  /* 0x80138F20: 0x2C620013 */
    /* fall through to block_80138F24 */

block_80138F24:;
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_6) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F2Cu);
    }

block_80138F2C:
    v0 = v0 + 184;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80138F3C: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F44: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

block_80138F48:
    v0 = s1 + 88;
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));
    /* nop */  /* 0x80138F54: 0x00000000 */
    /* nop */  /* 0x80138F58: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80138F5C: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s3), (cpu->gte_data[14]));
    gte_precision_store_word((s3), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80138F68: 0x00000000 */
    t4 = (int32_t)t4 >> 2;
    cpu->write_word(s5, t4);
    a0 = cpu->read_word(s5);
    /* nop */  /* 0x80138F78: 0x00000000 */
    v0 = a0 + -4;
    v0 = (v0 < (uint32_t)2059) ? 1 : 0;  /* 0x80138F80: 0x2C42080B */
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F88: 0x00000000 */
    if (cond_v0_eq_zero_7) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F8Cu);
    }

block_80138F8C:
    v0 = cpu->read_half(s4 + 36);
    v1 = cpu->read_word(s1 + 84);
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = a0 << 2;
    /* nop */  /* 0x80138FA8: 0x00000000 */
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)v0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);
    s0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = 0x801F << 16;  /* 0x801F0000 */
    a0 = (int32_t)(int16_t)cpu->read_half(v0 + 664);
    ra = 0x80138FC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 160;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80138FC4:
    a0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138FC8: 0x00000000 */
    v1 = a0 << 2;
    v1 = v1 + a0;
    v1 = v1 << 3;
    v1 = v1 + s1;
    a0 = cpu->read_half(s3);
    a1 = (uint32_t)s0 >> 31;
    { int64_t result = (int64_t)(int32_t)s0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s0));
    a1 = s0 + a1;
    a1 = (int32_t)a1 >> 1;
    a0 = a0 - a1;
    cpu->write_half(v1 + 8, (uint16_t)a0);
    a0 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80138FFC: 0x00000000 */
    v1 = a0 << 2;
    v1 = v1 + a0;
    v1 = v1 << 3;
    v1 = v1 + s1;
    a0 = cpu->read_half(s3 + 2);
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = (int32_t)v0 >> 12;
    v0 = (uint32_t)v0 >> 31;
    v0 = a3 + v0;
    a2 = (int32_t)v0 >> 1;
    a0 = a0 - a2;
    cpu->write_half(v1 + 10, (uint16_t)a0);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139034: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    v1 = v1 + s0;
    cpu->write_half(v0 + 16, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x8013905C: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3 + 2);
    v0 = v0 + s1;
    v1 = v1 - a2;
    cpu->write_half(v0 + 18, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139080: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 24, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x801390A4: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a0 = v0 + s1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a2;
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x801390D8u);
        goto block_801390D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801390D0u);
    }

block_801390D0:
    /* delay slot (always executes) */
    v0 = v1 + a2;
    psx_check_interrupts_at(cpu, 0x801390DCu);
    goto block_801390DC;  /* j */

block_801390D8:
    v0 = v1 + a3;
    /* fall through to block_801390DC */

block_801390DC:
    cpu->write_half(a0 + 26, (uint16_t)v0);
    v0 = cpu->read_word(s4 + 12);
    v1 = cpu->read_half(s3);
    a0 = v0 << 2;
    a0 = a0 + v0;
    a0 = a0 << 3;
    a0 = a0 + s1;
    v0 = (uint32_t)s0 >> 31;
    v0 = s0 + v0;
    v0 = (int32_t)v0 >> 1;
    v1 = v1 - v0;
    v1 = v1 + s0;
    cpu->write_half(a0 + 32, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139114: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a1 = v0 + s1;
    v0 = (uint32_t)a3 >> 31;
    v0 = a3 + v0;
    a0 = (int32_t)v0 >> 1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a0;
    if (cond_v0_ne_zero_6) {
        psx_check_interrupts_at(cpu, 0x80139154u);
        goto block_80139154;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013914Cu);
    }

block_8013914C:
    /* delay slot (always executes) */
    v0 = v1 + a0;
    psx_check_interrupts_at(cpu, 0x80139158u);
    goto block_80139158;  /* j */

block_80139154:
    v0 = v1 + a3;
    /* fall through to block_80139158 */

block_80139158:
    /* delay slot (always executes) */
    cpu->write_half(a1 + 34, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80139A0Cu);
    goto block_80139A0C;  /* j */

block_80139160:
    v0 = s1 + 88;
    v1 = cpu->read_word(s1 + 96);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));
    /* nop */  /* 0x80139170: 0x00000000 */
    /* nop */  /* 0x80139174: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139178: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s3), (cpu->gte_data[14]));
    gte_precision_store_word((s3), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139184: 0x00000000 */
    t4 = (int32_t)t4 >> 2;
    cpu->write_word(s5, t4);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v1)));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v1 + 4))));
    /* nop */  /* 0x80139198: 0x00000000 */
    /* nop */  /* 0x8013919C: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x801391A0: 0x4A180001 */
    v0 = sp + 24;
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[14]));
    gte_precision_store_word((v0), 14);
    a0 = (int32_t)(int16_t)cpu->read_half(s3);
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 24);
    v0 = cpu->read_half(s3 + 2);
    a2 = a0 - v1;
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 26);
    a0 = cpu->read_half(s3);
    int cond_a2_ge_0 = ((int32_t)a2 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = v0 - v1;
    if (cond_a2_ge_0) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801391D4u);
    }

block_801391D4:
    v0 = cpu->read_word(s5);
    /* nop */  /* 0x801391D8: 0x00000000 */
    v0 = v0 + -4;
    v0 = (v0 < (uint32_t)2059) ? 1 : 0;  /* 0x801391E0: 0x2C42080B */
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (uint32_t)a2 >> 31;
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801391ECu);
    }

block_801391EC:
    a1 = a2 + a1;
    v1 = cpu->read_word(s4 + 12);
    a1 = (int32_t)a1 >> 1;
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = v0 + s1;
    v1 = a0 - a1;
    cpu->write_half(v0 + 8, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    a0 = cpu->read_half(s3 + 2);
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = v0 + s1;
    v1 = (uint32_t)t0 >> 31;
    v1 = t0 + v1;
    a3 = (int32_t)v1 >> 1;
    a0 = a0 - a3;
    cpu->write_half(v0 + 10, (uint16_t)a0);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139240: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    v1 = v1 + a2;
    cpu->write_half(v0 + 16, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139268: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3 + 2);
    v0 = v0 + s1;
    v1 = v1 - a3;
    cpu->write_half(v0 + 18, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x8013928C: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 24, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x801392B0: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a0 = v0 + s1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_7 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a3;
    if (cond_v0_ne_zero_7) {
        psx_check_interrupts_at(cpu, 0x801392E4u);
        goto block_801392E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801392DCu);
    }

block_801392DC:
    /* delay slot (always executes) */
    v0 = v1 + a3;
    psx_check_interrupts_at(cpu, 0x801392E8u);
    goto block_801392E8;  /* j */

block_801392E4:
    v0 = v1 + t0;
    /* fall through to block_801392E8 */

block_801392E8:
    cpu->write_half(a0 + 26, (uint16_t)v0);
    v0 = cpu->read_word(s4 + 12);
    v1 = cpu->read_half(s3);
    a0 = v0 << 2;
    a0 = a0 + v0;
    a0 = a0 << 3;
    a0 = a0 + s1;
    v0 = (uint32_t)a2 >> 31;
    v0 = a2 + v0;
    v0 = (int32_t)v0 >> 1;
    v1 = v1 - v0;
    v1 = v1 + a2;
    cpu->write_half(a0 + 32, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139320: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a1 = v0 + s1;
    v0 = (uint32_t)t0 >> 31;
    v0 = t0 + v0;
    a0 = (int32_t)v0 >> 1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a0;
    if (cond_v0_ne_zero_8) {
        psx_check_interrupts_at(cpu, 0x80139360u);
        goto block_80139360;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139358u);
    }

block_80139358:
    /* delay slot (always executes) */
    v0 = v1 + a0;
    psx_check_interrupts_at(cpu, 0x80139364u);
    goto block_80139364;  /* j */

block_80139360:
    v0 = v1 + t0;
    /* fall through to block_80139364 */

block_80139364:
    /* delay slot (always executes) */
    cpu->write_half(a1 + 34, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80139A0Cu);
    goto block_80139A0C;  /* j */

block_8013936C:
    v0 = cpu->read_half(s4 + 130);
    /* nop */  /* 0x80139370: 0x00000000 */
    int cond_v0_ne_zero_9 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (cond_v0_ne_zero_9) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013937Cu);
    }

block_8013937C:
    a1 = cpu->read_word(s1 + 96);
    v0 = cpu->read_half(s1 + 88);
    v1 = cpu->read_half(a1);
    /* nop */  /* 0x80139388: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 88, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 90);
    v1 = cpu->read_half(a1 + 2);
    /* nop */  /* 0x8013939C: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 90, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 92);
    v1 = cpu->read_half(a1 + 4);
    /* nop */  /* 0x801393B0: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 92, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(a1);
    v1 = cpu->read_half(a1);
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801393C8: 0x00000000 */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x801393D0u);
        goto block_801393D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801393CCu);
    }

block_801393CC:
    v0 = v0 + 7;
    /* fall through to block_801393D0 */

block_801393D0:
    v0 = (int32_t)v0 >> 3;
    a0 = (int32_t)(int16_t)cpu->read_half(a1 + 2);
    a2 = cpu->read_half(a1 + 2);
    v0 = v1 - v0;
    int cond_a0_ge_0 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a1, (uint16_t)v0);
    if (cond_a0_ge_0) {
        psx_check_interrupts_at(cpu, 0x801393ECu);
        goto block_801393EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801393E8u);
    }

block_801393E8:
    a0 = a0 + 7;
    /* fall through to block_801393EC */

block_801393EC:
    v0 = (int32_t)a0 >> 3;
    v1 = (int32_t)(int16_t)cpu->read_half(a1 + 4);
    a0 = cpu->read_half(a1 + 4);
    v0 = a2 - v0;
    int cond_v1_ge_0 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a1 + 2, (uint16_t)v0);
    if (cond_v1_ge_0) {
        psx_check_interrupts_at(cpu, 0x80139408u);
        goto block_80139408;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139404u);
    }

block_80139404:
    v1 = v1 + 7;
    /* fall through to block_80139408 */

block_80139408:
    v0 = (int32_t)v1 >> 3;
    v1 = cpu->read_half(a1 + 2);
    v0 = a0 - v0;
    cpu->write_half(a1 + 4, (uint16_t)v0);
    v1 = v1 + 1;
    /* delay slot (always executes) */
    cpu->write_half(a1 + 2, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139424:
    v0 = cpu->read_half(s4 + 130);
    /* nop */  /* 0x80139428: 0x00000000 */
    int cond_v0_ne_zero_10 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (cond_v0_ne_zero_10) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139434u);
    }

block_80139434:
    s0 = cpu->read_word(s1 + 96);
    /* nop */  /* 0x80139438: 0x00000000 */
    v0 = cpu->read_word(s0 + 12);
    v1 = 3;
    v0 = v0 + 1;
    int cond_v0_ne_v1 = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 12, v0);
    if (cond_v0_ne_v1) {
        psx_check_interrupts_at(cpu, 0x8013946Cu);
        goto block_8013946C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139450u);
    }

block_80139450:
    a0 = s1 + 88;
    a1 = 15;
    a2 = zero;
    a3 = a2;
    cpu->write_word(s0 + 12, zero);
    ra = 0x8013946Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);
    psx_check_interrupts_at(cpu, 0x80139FD0u);
    cpu->pc = 0x80139FD0u; return;  /* CPS jal -> 0x80139FD0 */

block_8013946C:
    v0 = cpu->read_half(s1 + 88);
    v1 = cpu->read_half(s0);
    /* nop */  /* 0x80139474: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 88, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 90);
    v1 = cpu->read_half(s0 + 2);
    /* nop */  /* 0x80139488: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 90, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 92);
    v1 = cpu->read_half(s0 + 4);
    /* nop */  /* 0x8013949C: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 92, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s0);
    v1 = cpu->read_half(s0);
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801394B4: 0x00000000 */
    if (cond_v0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x801394BCu);
        goto block_801394BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394B8u);
    }

block_801394B8:
    v0 = v0 + 3;
    /* fall through to block_801394BC */

block_801394BC:
    v0 = (int32_t)v0 >> 2;
    a0 = (int32_t)(int16_t)cpu->read_half(s0 + 2);
    a1 = cpu->read_half(s0 + 2);
    v0 = v1 - v0;
    int cond_a0_ge_0_2 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0, (uint16_t)v0);
    if (cond_a0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x801394D8u);
        goto block_801394D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394D4u);
    }

block_801394D4:
    a0 = a0 + 3;
    /* fall through to block_801394D8 */

block_801394D8:
    v0 = (int32_t)a0 >> 2;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 4);
    a0 = cpu->read_half(s0 + 4);
    v0 = a1 - v0;
    int cond_v1_ge_0_2 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 2, (uint16_t)v0);
    if (cond_v1_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x801394F4u);
        goto block_801394F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394F0u);
    }

block_801394F0:
    v1 = v1 + 3;
    /* fall through to block_801394F4 */

block_801394F4:
    v0 = (int32_t)v1 >> 2;
    v0 = a0 - v0;
    /* delay slot (always executes) */
    cpu->write_half(s0 + 4, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139504:
    v0 = cpu->read_half(s4 + 130);
    /* nop */  /* 0x80139508: 0x00000000 */
    int cond_v0_ne_zero_11 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (cond_v0_ne_zero_11) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139514u);
    }

block_80139514:
    a0 = cpu->read_word(s1 + 96);
    v0 = cpu->read_half(s1 + 88);
    v1 = cpu->read_half(a0);
    /* nop */  /* 0x80139520: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 88, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 90);
    v1 = cpu->read_half(a0 + 2);
    /* nop */  /* 0x80139534: 0x00000000 */
    v0 = v0 + v1;
    v1 = cpu->read_half(s1 + 92);
    cpu->write_half(s1 + 90, (uint16_t)v0);
    v0 = cpu->read_word(s1 + 84);
    a0 = cpu->read_half(a0 + 4);
    v0 = v0 + 16;
    v1 = v1 + a0;
    cpu->write_word(s1 + 84, v0);
    /* delay slot (always executes) */
    cpu->write_half(s1 + 92, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139560:
    v0 = cpu->read_half(s4 + 130);
    /* nop */  /* 0x80139564: 0x00000000 */
    int cond_v0_ne_zero_12 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (cond_v0_ne_zero_12) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139570u);
    }

block_80139570:
    a0 = cpu->read_word(s1 + 96);
    v0 = cpu->read_half(s1 + 88);
    v1 = cpu->read_half(a0);
    /* nop */  /* 0x8013957C: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 88, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 90);
    v1 = cpu->read_half(a0 + 2);
    /* nop */  /* 0x80139590: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 90, (uint16_t)v0);
    v0 = cpu->read_half(s1 + 92);
    v1 = cpu->read_half(a0 + 4);
    /* nop */  /* 0x801395A4: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s1 + 92, (uint16_t)v0);
    v0 = cpu->read_half(a0 + 2);
    /* nop */  /* 0x801395B4: 0x00000000 */
    v0 = v0 + 2;
    cpu->write_half(a0 + 2, (uint16_t)v0);
    v1 = cpu->read_word(s1 + 84);
    /* nop */  /* 0x801395C4: 0x00000000 */
    v0 = ((int32_t)v1 < 51) ? 1 : 0;  /* 0x801395C8: 0x28620033 */
    int cond_v0_ne_zero_13 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;
    if (cond_v0_ne_zero_13) {
        psx_check_interrupts_at(cpu, 0x801395D8u);
        goto block_801395D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801395D4u);
    }

block_801395D4:
    cpu->write_word(s1 + 84, v0);
    /* fall through to block_801395D8 */

block_801395D8:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 90);
    /* nop */  /* 0x801395DC: 0x00000000 */
    int cond_v0_lt_0 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (cond_v0_lt_0) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801395E8u);
    }

block_801395E8:
    v0 = cpu->read_half(a0 + 2);
    /* nop */  /* 0x801395EC: 0x00000000 */
    v0 = v0 << 16;
    v1 = (int32_t)v0 >> 16;
    v0 = (uint32_t)v0 >> 31;
    v0 = v1 + v0;
    int cond_v1_ge_0_3 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)v0 >> 1;
    if (cond_v1_ge_0_3) {
        psx_check_interrupts_at(cpu, 0x8013960Cu);
        goto block_8013960C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139608u);
    }

block_80139608:
    v1 = v1 + 3;
    /* fall through to block_8013960C */

block_8013960C:
    v0 = (int32_t)v1 >> 2;
    v0 = a1 + v0;
    v0 = zero - v0;
    /* delay slot (always executes) */
    cpu->write_half(a0 + 2, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139620:
    v1 = cpu->read_word(s1 + 96);
    /* nop */  /* 0x80139624: 0x00000000 */
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x8013962C: 0x00000000 */
    v0 = cpu->read_half(v0);
    /* nop */  /* 0x80139634: 0x00000000 */
    cpu->write_half(s1 + 88, (uint16_t)v0);
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80139640: 0x00000000 */
    v0 = cpu->read_half(v0 + 4);
    /* nop */  /* 0x80139648: 0x00000000 */
    cpu->write_half(s1 + 90, (uint16_t)v0);
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80139654: 0x00000000 */
    v0 = cpu->read_half(v0 + 8);
    /* nop */  /* 0x8013965C: 0x00000000 */
    cpu->write_half(s1 + 92, (uint16_t)v0);
    /* fall through to block_80139664 */

block_80139664:
    v0 = s1 + 88;
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));
    /* nop */  /* 0x80139670: 0x00000000 */
    /* nop */  /* 0x80139674: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139678: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s3), (cpu->gte_data[14]));
    gte_precision_store_word((s3), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139684: 0x00000000 */
    t4 = (int32_t)t4 >> 2;
    cpu->write_word(s5, t4);
    v0 = cpu->read_half(s4 + 36);
    v1 = cpu->read_word(s1 + 84);
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v0 = cpu->read_word(s5);
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v0 << 2;
    /* nop */  /* 0x801396B0: 0x00000000 */
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)v0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);
    s0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v1 = cpu->read_word(s4 + 12);
    a0 = cpu->read_half(s3);
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = v0 + s1;
    v1 = (uint32_t)s0 >> 31;
    v1 = s0 + v1;
    a1 = (int32_t)v1 >> 1;
    a0 = a0 - a1;
    cpu->write_half(v0 + 8, (uint16_t)a0);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x801396EC: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3 + 2);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 10, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139710: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    v1 = v1 + s0;
    cpu->write_half(v0 + 16, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139738: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3 + 2);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 18, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x8013975C: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 24, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139780: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a0 = v0 + s1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_14 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (cond_v0_ne_zero_14) {
        psx_check_interrupts_at(cpu, 0x801397B4u);
        goto block_801397B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801397ACu);
    }

block_801397AC:
    /* delay slot (always executes) */
    v0 = v1 + a1;
    psx_check_interrupts_at(cpu, 0x801397B8u);
    goto block_801397B8;  /* j */

block_801397B4:
    v0 = v1 + s0;
    /* fall through to block_801397B8 */

block_801397B8:
    v1 = (uint32_t)s0 >> 31;
    v1 = s0 + v1;
    cpu->write_half(a0 + 26, (uint16_t)v0);
    v0 = cpu->read_word(s4 + 12);
    a1 = (int32_t)v1 >> 1;
    a0 = v0 << 2;
    a0 = a0 + v0;
    a0 = a0 << 3;
    v0 = cpu->read_half(s3);
    a0 = a0 + s1;
    v0 = v0 - a1;
    v0 = v0 + s0;
    cpu->write_half(a0 + 32, (uint16_t)v0);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x801397F0: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a0 = v0 + s1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_15 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (cond_v0_ne_zero_15) {
        psx_check_interrupts_at(cpu, 0x80139824u);
        goto block_80139824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013981Cu);
    }

block_8013981C:
    /* delay slot (always executes) */
    v0 = v1 + a1;
    psx_check_interrupts_at(cpu, 0x80139828u);
    goto block_80139828;  /* j */

block_80139824:
    v0 = v1 + s0;
    /* fall through to block_80139828 */

block_80139828:
    /* delay slot (always executes) */
    cpu->write_half(a0 + 34, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80139D98u);
    goto block_80139D98;  /* j */

block_80139830:
    v0 = s1 + 88;
    /* fall through to block_80139834 */

block_80139834:
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));
    /* nop */  /* 0x8013983C: 0x00000000 */
    /* nop */  /* 0x80139840: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139844: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s3), (cpu->gte_data[14]));
    gte_precision_store_word((s3), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139850: 0x00000000 */
    t4 = (int32_t)t4 >> 2;
    cpu->write_word(s5, t4);
    a0 = cpu->read_word(s5);
    /* nop */  /* 0x80139860: 0x00000000 */
    v0 = a0 + -4;
    v0 = (v0 < (uint32_t)2059) ? 1 : 0;  /* 0x80139868: 0x2C42080B */
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139870: 0x00000000 */
    if (cond_v0_eq_zero_9) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139874u);
    }

block_80139874:
    v0 = cpu->read_half(s4 + 36);
    v1 = cpu->read_word(s1 + 84);
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = a0 << 2;
    /* nop */  /* 0x80139890: 0x00000000 */
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)v0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);
    s0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v1 = cpu->read_word(s4 + 12);
    a0 = cpu->read_half(s3);
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = v0 + s1;
    v1 = (uint32_t)s0 >> 31;
    v1 = s0 + v1;
    a1 = (int32_t)v1 >> 1;
    a0 = a0 - a1;
    cpu->write_half(v0 + 8, (uint16_t)a0);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x801398CC: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3 + 2);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 10, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x801398F0: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    v1 = v1 + s0;
    cpu->write_half(v0 + 16, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139918: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3 + 2);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 18, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x8013993C: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v1 = cpu->read_half(s3);
    v0 = v0 + s1;
    v1 = v1 - a1;
    cpu->write_half(v0 + 24, (uint16_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139960: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a0 = v0 + s1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_16 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (cond_v0_ne_zero_16) {
        psx_check_interrupts_at(cpu, 0x80139994u);
        goto block_80139994;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013998Cu);
    }

block_8013998C:
    /* delay slot (always executes) */
    v0 = v1 + a1;
    psx_check_interrupts_at(cpu, 0x80139998u);
    goto block_80139998;  /* j */

block_80139994:
    v0 = v1 + s0;
    /* fall through to block_80139998 */

block_80139998:
    v1 = (uint32_t)s0 >> 31;
    v1 = s0 + v1;
    cpu->write_half(a0 + 26, (uint16_t)v0);
    v0 = cpu->read_word(s4 + 12);
    a1 = (int32_t)v1 >> 1;
    a0 = v0 << 2;
    a0 = a0 + v0;
    a0 = a0 << 3;
    v0 = cpu->read_half(s3);
    a0 = a0 + s1;
    v0 = v0 - a1;
    v0 = v0 + s0;
    cpu->write_half(a0 + 32, (uint16_t)v0);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x801399D0: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a0 = v0 + s1;
    v1 = cpu->read_half(s3 + 2);
    v0 = cpu->read_byte(s4 + 21);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    int cond_v0_ne_zero_17 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (cond_v0_ne_zero_17) {
        psx_check_interrupts_at(cpu, 0x80139A04u);
        goto block_80139A04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801399FCu);
    }

block_801399FC:
    /* delay slot (always executes) */
    v0 = v1 + a1;
    psx_check_interrupts_at(cpu, 0x80139A08u);
    goto block_80139A08;  /* j */

block_80139A04:
    v0 = v1 + s0;
    /* fall through to block_80139A08 */

block_80139A08:
    cpu->write_half(a0 + 34, (uint16_t)v0);
    /* fall through to block_80139A0C */

block_80139A0C:
    v0 = cpu->read_word(s4 + 12);
    a0 = cpu->read_word(s4 + 8);
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = v1 << 3;
    v0 = cpu->read_word(s5);
    v1 = v1 + s1;
    v0 = v0 << 2;
    v0 = v0 + a0;
    v0 = cpu->read_word(v0);
    a0 = 0x0900 << 16;  /* 0x09000000 */
    v0 = v0 | a0;
    cpu->write_word(v1, v0);
    v1 = cpu->read_word(s5);
    v0 = cpu->read_word(s4 + 8);
    a0 = cpu->read_word(s4 + 12);
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = a0 << 2;
    v0 = v0 + a0;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v0 = v0 << 8;
    v0 = (uint32_t)v0 >> 8;
    /* delay slot (always executes) */
    cpu->write_word(v1, v0);
    psx_check_interrupts_at(cpu, 0x80139DE8u);
    goto block_80139DE8;  /* j */

block_80139A74:
    v0 = s1 + 88;
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));
    /* nop */  /* 0x80139A80: 0x00000000 */
    /* nop */  /* 0x80139A84: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139A88: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s3), (cpu->gte_data[14]));
    gte_precision_store_word((s3), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139A94: 0x00000000 */
    t4 = (int32_t)t4 >> 2;
    cpu->write_word(s5, t4);
    v0 = cpu->read_word(s5);
    /* nop */  /* 0x80139AA4: 0x00000000 */
    v0 = v0 + -4;
    v0 = (v0 < (uint32_t)2059) ? 1 : 0;  /* 0x80139AAC: 0x2C42080B */
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139AB4: 0x00000000 */
    if (cond_v0_eq_zero_10) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AB8u);
    }

block_80139AB8:
    v0 = cpu->read_half(s4 + 130);
    s2 = cpu->read_word(s1 + 96);
    int cond_v0_ne_zero_18 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139AC4: 0x00000000 */
    if (cond_v0_ne_zero_18) {
        psx_check_interrupts_at(cpu, 0x80139B78u);
        goto block_80139B78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AC8u);
    }

block_80139AC8:
    a0 = cpu->read_half(s2 + 2);
    v0 = cpu->read_half(s2);
    v1 = a0;
    v0 = v0 + a0;
    v1 = v1 + -50;
    cpu->write_half(s2 + 2, (uint16_t)v1);
    v1 = v1 << 16;
    int cond_v1_ge_0_4 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s2, (uint16_t)v0);
    if (cond_v1_ge_0_4) {
        psx_check_interrupts_at(cpu, 0x80139AF0u);
        goto block_80139AF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AECu);
    }

block_80139AEC:
    cpu->write_half(s2 + 2, (uint16_t)zero);
    /* fall through to block_80139AF0 */

block_80139AF0:
    v0 = cpu->read_word(s1 + 84);
    /* nop */  /* 0x80139AF4: 0x00000000 */
    v0 = v0 + 4;
    cpu->write_word(s1 + 84, v0);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139B04: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_byte(v0 + 4);
    /* nop */  /* 0x80139B1C: 0x00000000 */
    v1 = v1 + -24;
    cpu->write_byte(v0 + 4, (uint8_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139B2C: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_byte(v0 + 5);
    /* nop */  /* 0x80139B44: 0x00000000 */
    v1 = v1 + -24;
    cpu->write_byte(v0 + 5, (uint8_t)v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139B54: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_byte(v0 + 6);
    /* nop */  /* 0x80139B6C: 0x00000000 */
    v1 = v1 + -24;
    cpu->write_byte(v0 + 6, (uint8_t)v1);
    /* fall through to block_80139B78 */

block_80139B78:
    v0 = cpu->read_half(s4 + 36);
    v1 = cpu->read_word(s1 + 84);
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v0 = cpu->read_word(s5);
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v1 = cpu->read_byte(s4 + 21);
    v0 = v0 << 2;
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)t0 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)t0 % (int32_t)v0); } else { cpu->lo = ((int32_t)t0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)t0; }
    psx_muldiv_set(cpu, 37u);
    s0 = cpu->lo;
    psx_muldiv_stall(cpu);
    int cond_v1_ne_zero = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = s0;
    if (cond_v1_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80139BB8u);
        goto block_80139BB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139BACu);
    }

block_80139BAC:
    v0 = (uint32_t)s0 >> 31;
    v0 = s0 + v0;
    s5 = (int32_t)v0 >> 1;
    /* fall through to block_80139BB8 */

block_80139BB8:
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139BC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139BC0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139BC4:
    { int64_t result = (int64_t)(int32_t)s0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s0));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139BCC: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 + a0;
    cpu->write_half(v0 + 8, (uint16_t)v1);
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139C00u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139BFC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139C00:
    { int64_t result = (int64_t)(int32_t)s0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s0));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139C08: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 - a0;
    cpu->write_half(v0 + 32, (uint16_t)v1);
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139C3Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139C38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139C3C:
    { int64_t result = (int64_t)(int32_t)s5 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s5));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139C44: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3 + 2);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 + a0;
    cpu->write_half(v0 + 10, (uint16_t)v1);
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139C78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139C74: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139C78:
    { int64_t result = (int64_t)(int32_t)s5 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s5));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139C80: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3 + 2);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 - a0;
    cpu->write_half(v0 + 34, (uint16_t)v1);
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139CB4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 1024;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139CB4:
    { int64_t result = (int64_t)(int32_t)s0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s0));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139CBC: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 + a0;
    cpu->write_half(v0 + 16, (uint16_t)v1);
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139CF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 1024;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139CF0:
    { int64_t result = (int64_t)(int32_t)s0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s0));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139CF8: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 - a0;
    cpu->write_half(v0 + 24, (uint16_t)v1);
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139D2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 1024;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139D2C:
    { int64_t result = (int64_t)(int32_t)s5 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s5));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139D34: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3 + 2);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 + a0;
    cpu->write_half(v0 + 18, (uint16_t)v1);
    a0 = (int32_t)(int16_t)cpu->read_half(s2);
    ra = 0x80139D68u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 1024;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139D68:
    { int64_t result = (int64_t)(int32_t)s5 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s5));
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139D70: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v1 = cpu->read_half(s3 + 2);
    t1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a0 = (int32_t)t1 >> 12;
    v1 = v1 - a0;
    cpu->write_half(v0 + 26, (uint16_t)v1);
    /* fall through to block_80139D98 */

block_80139D98:
    v1 = cpu->read_word(s4 + 12);
    a0 = 0x0900 << 16;  /* 0x09000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v1 = cpu->read_word(s4 + 8);
    v0 = v0 << 3;
    v1 = cpu->read_word(v1 + 12);
    v0 = v0 + s1;
    v1 = v1 | a0;
    cpu->write_word(v0, v1);
    v1 = cpu->read_word(s4 + 12);
    /* nop */  /* 0x80139DC4: 0x00000000 */
    v0 = v1 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = s1 + v0;
    v0 = v0 << 8;
    v1 = cpu->read_word(s4 + 8);
    v0 = (uint32_t)v0 >> 8;
    cpu->write_word(v1 + 12, v0);
    /* fall through to block_80139DE8 */

block_80139DE8:
    ra = cpu->read_word(sp + 56);
    s5 = cpu->read_word(sp + 52);
    s4 = cpu->read_word(sp + 48);
    s3 = cpu->read_word(sp + 44);
    s2 = cpu->read_word(sp + 40);
    s1 = cpu->read_word(sp + 36);
    s0 = cpu->read_word(sp + 32);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 64;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}
/* ======== ev24_fx_spawn_alias (alias body guest 0x801421C4) ======== */

/* Overlapping-alias body for br2eu_func_801421C4 (ev24_fx_spawn @0x801421C4): entries 0x801428D4/0x80142954/0x801429F4 plus 22 CPS blocks. Faithful CPS transliteration of generated/SLES_017.22_full_11.c br2eu_psx_alias_body_801421C4.
 * cpu->pc dispatch: continuation PCs hit the _cont switch; alias entry
 * addrs fall through to the entry switch (entry = cpu->pc). */
void ev24_fx_spawn_alias(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t entry;
  if (!cpu) return;
  entry = cpu->pc;
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801428F4u: goto block_801428F4;
            case 0x80142988u: goto block_80142988;
            case 0x80142998u: goto block_80142998;
            case 0x801429ECu: goto block_801429EC;
            case 0x80142A3Cu: goto block_80142A3C;
            case 0x80142908u: goto block_80142908;
            case 0x80142910u: goto block_80142910;
            case 0x80142918u: goto block_80142918;
            case 0x80142920u: goto block_80142920;
            case 0x80142928u: goto block_80142928;
            case 0x80142930u: goto block_80142930;
            case 0x80142944u: goto block_80142944;
            case 0x8014294Cu: goto block_8014294C;
            case 0x80142960u: goto block_80142960;
            case 0x80142968u: goto block_80142968;
            case 0x80142970u: goto block_80142970;
            case 0x80142990u: goto block_80142990;
            case 0x801429C4u: goto block_801429C4;
            case 0x80142A00u: goto block_80142A00;
            default: break;
        }
    }
    switch (entry) {
        case 0x801428D4u: goto block_801428D4;
        case 0x80142954u: goto block_80142954;
        case 0x801429F4u: goto block_801429F4;
        default: return;
    }

block_801428D4:
    v0 = 0x801F << 16;  /* 0x801F0000 */
    v1 = cpu->read_byte(s2 + 2);
    v0 = v0 + 1872;
    v1 = v1 << 2;
    v1 = v1 + v0;
    a0 = cpu->read_word(v1);
    ra = 0x801428F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801428F0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AD284u);
    cpu->pc = 0x801AD284u; return;  /* CPS jal -> 0x801AD284 */

block_801428F4:
    a0 = v0;
    v1 = s1 & 0x00FF;  /* 0x801428F8: 0x322300FF */
    v0 = 254;
    int cond_v1_eq_v0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 255) ? 1 : 0;  /* 0x80142904: 0x286200FF */
    if (cond_v1_eq_v0) {
        psx_check_interrupts_at(cpu, 0x8014294Cu);
        goto block_8014294C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142908u);
    }

block_80142908:;
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 255;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80142920u);
        goto block_80142920;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142910u);
    }

block_80142910:;
    int cond_v1_eq_zero = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 << 16;
    if (cond_v1_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80142930u);
        goto block_80142930;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142918u);
    }

block_80142918:
    /* delay slot (always executes) */
    /* nop */  /* 0x8014291C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_80142920:;
    int cond_v1_eq_v0_2 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v1_eq_v0_2) {
        psx_check_interrupts_at(cpu, 0x80142944u);
        goto block_80142944;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142928u);
    }

block_80142928:
    /* delay slot (always executes) */
    /* nop */  /* 0x8014292C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_80142930:
    v0 = (int32_t)v0 >> 24;
    cpu->write_word(a0 + 12, v0);
    v0 = s0 & 0x00FF;  /* 0x80142938: 0x320200FF */
    /* delay slot (always executes) */
    cpu->write_word(a0 + 16, v0);
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_80142944:
    /* delay slot (always executes) */
    cpu->write_word(a0 + 20, v0);
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_8014294C:
    /* delay slot (always executes) */
    cpu->write_word(a0 + 20, zero);
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_80142954:
    v1 = s1 & 0x00FF;  /* 0x80142954: 0x322300FF */
    int cond_v1_eq_zero_2 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v1_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80142970u);
        goto block_80142970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142960u);
    }

block_80142960:;
    int cond_v1_eq_v0_3 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    if (cond_v1_eq_v0_3) {
        psx_check_interrupts_at(cpu, 0x801429C4u);
        goto block_801429C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142968u);
    }

block_80142968:
    /* delay slot (always executes) */
    /* nop */  /* 0x8014296C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_80142970:
    a0 = 1;
    a1 = a0;
    a2 = cpu->read_byte(s2 + 2);
    a3 = 8;
    ra = 0x80142988u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a2 + a0;
    psx_check_interrupts_at(cpu, 0x801AD38Cu);
    cpu->pc = 0x801AD38Cu; return;  /* CPS jal -> 0x801AD38C */

block_80142988:;
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014298C: 0x00000000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80142B00u);
        cpu->pc = 0x80142B00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80142990u);
    }

block_80142990:
    ra = 0x80142998u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = v0;
    psx_check_interrupts_at(cpu, 0x801AD284u);
    cpu->pc = 0x801AD284u; return;  /* CPS jal -> 0x801AD284 */

block_80142998:
    v1 = cpu->read_byte(s2 + 2);
    a0 = s3 & 0x00FF;  /* 0x8014299C: 0x326400FF */
    cpu->write_word(v0 + 4, v1);
    v1 = a0 << 2;
    v1 = v1 + a0;
    a0 = cpu->read_word(s2 + 32);
    v1 = v1 << 4;
    a0 = a0 + v1;
    a0 = a0 + 52;
    /* delay slot (always executes) */
    cpu->write_word(v0 + 8, a0);
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_801429C4:
    a2 = s0 << 16;
    a2 = (int32_t)a2 >> 16;
    v1 = s3 & 0x00FF;  /* 0x801429CC: 0x326300FF */
    v0 = v1 << 2;
    v0 = v0 + v1;
    a1 = cpu->read_word(a0 + 32);
    v0 = v0 << 4;
    a1 = a1 + v0;
    ra = 0x801429ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 52;
    psx_check_interrupts_at(cpu, 0x8013E0F0u);
    cpu->pc = 0x8013E0F0u; return;  /* CPS jal -> 0x8013E0F0 */

block_801429EC:
    /* delay slot (always executes) */
    /* nop */  /* 0x801429F0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_801429F4:
    v0 = s1 & 0x00FF;  /* 0x801429F4: 0x322200FF */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x1F80 << 16;  /* 0x1F800000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80142B00u);
        cpu->pc = 0x80142B00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80142A00u);
    }

block_80142A00:
    v1 = v1 | 0x00E8;
    a0 = s0 << 16;
    a0 = (int32_t)a0 >> 27;
    v0 = cpu->read_byte(s2 + 2);
    a1 = s0 & 0x07FF;  /* 0x80142A10: 0x320507FF */
    a2 = v0 << 5;
    a2 = a2 + v0;
    a2 = a2 << 3;
    a2 = a2 + v1;
    v0 = v0 ^ 0x0001;  /* 0x80142A24: 0x38420001 */
    a3 = v0 << 5;
    a3 = a3 + v0;
    a3 = a3 << 3;
    ra = 0x80142A3Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = a3 + v1;
    psx_check_interrupts_at(cpu, 0x8013C9CCu);
    cpu->pc = 0x8013C9CCu; return;  /* CPS jal -> 0x8013C9CC */

block_80142A3C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80142A40: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */
    cpu->pc = 0x80142a40u; return;/* fallthrough to next function */
}