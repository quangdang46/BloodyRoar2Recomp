#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern CPUState *clean_cpu_state(void);
extern uint32_t br2_ld32(uint32_t addr);
extern void     br2_st32(uint32_t addr, uint32_t val);
extern uint8_t  br2_ld8(uint32_t addr);
extern uint16_t br2_ld16(uint32_t addr);
extern void     br2_st8(uint32_t addr, uint8_t val);
extern void     br2_st16(uint32_t addr, uint16_t val);
extern int      psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);
extern uint32_t br2_call_generated(uint32_t addr, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))

static void swl_guest(uint32_t addr, uint32_t v)
{
  uint32_t al = addr & ~3u;
  uint32_t w = DB32(al);
  switch (addr & 3u) {
  case 0: w = (w & 0xFFFFFF00u) | (v >> 24); break;
  case 1: w = (w & 0xFFFF0000u) | (v >> 16); break;
  case 2: w = (w & 0xFF000000u) | (v >> 8);  break;
  default: w = v; break;
  }
  DB32(al) = w;
}

static void prim_ot_link_indexed(uint32_t ot, uint32_t out, uint32_t otz)
{
  uint32_t slot = ot + (otz & 0x1ffcu);
  uint32_t old  = DB32(slot);
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
}

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
    int _bc_801373E0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (_bc_801373E0) {
        psx_check_interrupts_at(cpu, 0x8013742Cu);
        goto block_8013742C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801373E8u);
        goto block_801373E8;  /* not taken */
    }

block_801373E8:
    int _bc_801373E8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    if (_bc_801373E8) {
        psx_check_interrupts_at(cpu, 0x80137448u);
        goto block_80137448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801373F0u);
        goto block_801373F0;  /* not taken */
    }

block_801373F0:
    a1 = a0;
    a2 = s1 + a0;
    ra = 0x80137400u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = v0;
    psx_check_interrupts_at(cpu, 0x801AD38Cu);
    cpu->pc = 0x801AD38Cu; return;  /* CPS jal -> 0x801AD38C */

block_80137400:
    int _bc_80137400 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s2, v0);
    if (_bc_80137400) {
        psx_check_interrupts_at(cpu, 0x80137448u);
        goto block_80137448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80137408u);
        goto block_80137408;  /* not taken */
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
    int _bc_80137434 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    if (_bc_80137434) {
        psx_check_interrupts_at(cpu, 0x80137448u);
        goto block_80137448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013743Cu);
        goto block_8013743C;  /* not taken */
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
    int _bc_80137454 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s3 = s3 + 264;
    if (_bc_80137454) {
        psx_check_interrupts_at(cpu, 0x801373CCu);
        goto block_801373CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013745Cu);
        goto block_8013745C;  /* not taken */
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
    int _bc_801374C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 4;
    if (_bc_801374C8) {
        psx_check_interrupts_at(cpu, 0x80137480u);
        goto block_80137480;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801374D0u);
        goto block_801374D0;  /* not taken */
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
    uint32_t _jt_80137568 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 56;
    psx_check_interrupts_at(cpu, _jt_80137568);
    cpu->pc = _jt_80137568; return;  /* CPS: jr $ra */
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
    int _bc_80138D5C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D60: 0x00000000 */
    if (_bc_80138D5C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* not taken */
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
    int _bc_80138D7C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D80: 0x00000000 */
    if (_bc_80138D7C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D84u);
        goto block_80138D84;  /* not taken */
    }

block_80138D84:
    v0 = cpu->read_half(v1);
    /* nop */  /* 0x80138D88: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(v1, (uint16_t)v0);
    v0 = v0 << 16;
    int _bc_80138D98 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D9C: 0x00000000 */
    if (_bc_80138D98) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DA0u);
        goto block_80138DA0;  /* not taken */
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
    int _bc_80138DC4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (_bc_80138DC4) {
        psx_check_interrupts_at(cpu, 0x80138DD4u);
        goto block_80138DD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DCCu);
        goto block_80138DCC;  /* not taken */
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
    int _bc_80138DFC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (_bc_80138DFC) {
        psx_check_interrupts_at(cpu, 0x80138E0Cu);
        goto block_80138E0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E04u);
        goto block_80138E04;  /* not taken */
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
    int _bc_80138E34 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (_bc_80138E34) {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
        goto block_80138F0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E3Cu);
        goto block_80138E3C;  /* not taken */
    }

block_80138E3C:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 6, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80138F18u);
    goto block_80138F18;  /* j */

block_80138E44:
    v0 = cpu->read_half(s4 + 130);
    v1 = cpu->read_word(s1 + 96);
    int _bc_80138E4C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E50: 0x00000000 */
    if (_bc_80138E4C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E54u);
        goto block_80138E54;  /* not taken */
    }

block_80138E54:
    v0 = cpu->read_half(v1 + 8);
    /* nop */  /* 0x80138E58: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(v1 + 8, (uint16_t)v0);
    v0 = v0 << 16;
    int _bc_80138E68 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E6C: 0x00000000 */
    if (_bc_80138E68) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E70u);
        goto block_80138E70;  /* not taken */
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
    int _bc_80138E94 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (_bc_80138E94) {
        psx_check_interrupts_at(cpu, 0x80138EA4u);
        goto block_80138EA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E9Cu);
        goto block_80138E9C;  /* not taken */
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
    int _bc_80138ECC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (_bc_80138ECC) {
        psx_check_interrupts_at(cpu, 0x80138EDCu);
        goto block_80138EDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138ED4u);
        goto block_80138ED4;  /* not taken */
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
    int _bc_80138F04 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;
    if (_bc_80138F04) {
        psx_check_interrupts_at(cpu, 0x80138F14u);
        goto block_80138F14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
        goto block_80138F0C;  /* not taken */
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

block_80138F24:
    int _bc_80138F24 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_80138F24) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F2Cu);
        goto block_80138F2C;  /* not taken */
    }

block_80138F2C:
    v0 = v0 + 184;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80138F3C: 0x00000000 */
    uint32_t _jt_80138F40 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F44: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80138F40);
    cpu->pc = _jt_80138F40; return;  /* CPS: jr v0 */

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
    int _bc_80138F84 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F88: 0x00000000 */
    if (_bc_80138F84) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F8Cu);
        goto block_80138F8C;  /* not taken */
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
    int _bc_801390C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a2;
    if (_bc_801390C8) {
        psx_check_interrupts_at(cpu, 0x801390D8u);
        goto block_801390D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801390D0u);
        goto block_801390D0;  /* not taken */
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
    int _bc_80139144 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a0;
    if (_bc_80139144) {
        psx_check_interrupts_at(cpu, 0x80139154u);
        goto block_80139154;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013914Cu);
        goto block_8013914C;  /* not taken */
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
    int _bc_801391CC = ((int32_t)a2 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = v0 - v1;
    if (_bc_801391CC) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801391D4u);
        goto block_801391D4;  /* not taken */
    }

block_801391D4:
    v0 = cpu->read_word(s5);
    /* nop */  /* 0x801391D8: 0x00000000 */
    v0 = v0 + -4;
    v0 = (v0 < (uint32_t)2059) ? 1 : 0;  /* 0x801391E0: 0x2C42080B */
    int _bc_801391E4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (uint32_t)a2 >> 31;
    if (_bc_801391E4) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801391ECu);
        goto block_801391EC;  /* not taken */
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
    int _bc_801392D4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a3;
    if (_bc_801392D4) {
        psx_check_interrupts_at(cpu, 0x801392E4u);
        goto block_801392E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801392DCu);
        goto block_801392DC;  /* not taken */
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
    int _bc_80139350 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a0;
    if (_bc_80139350) {
        psx_check_interrupts_at(cpu, 0x80139360u);
        goto block_80139360;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139358u);
        goto block_80139358;  /* not taken */
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
    int _bc_80139374 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (_bc_80139374) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013937Cu);
        goto block_8013937C;  /* not taken */
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
    int _bc_801393C4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801393C8: 0x00000000 */
    if (_bc_801393C4) {
        psx_check_interrupts_at(cpu, 0x801393D0u);
        goto block_801393D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801393CCu);
        goto block_801393CC;  /* not taken */
    }

block_801393CC:
    v0 = v0 + 7;
    /* fall through to block_801393D0 */

block_801393D0:
    v0 = (int32_t)v0 >> 3;
    a0 = (int32_t)(int16_t)cpu->read_half(a1 + 2);
    a2 = cpu->read_half(a1 + 2);
    v0 = v1 - v0;
    int _bc_801393E0 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a1, (uint16_t)v0);
    if (_bc_801393E0) {
        psx_check_interrupts_at(cpu, 0x801393ECu);
        goto block_801393EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801393E8u);
        goto block_801393E8;  /* not taken */
    }

block_801393E8:
    a0 = a0 + 7;
    /* fall through to block_801393EC */

block_801393EC:
    v0 = (int32_t)a0 >> 3;
    v1 = (int32_t)(int16_t)cpu->read_half(a1 + 4);
    a0 = cpu->read_half(a1 + 4);
    v0 = a2 - v0;
    int _bc_801393FC = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a1 + 2, (uint16_t)v0);
    if (_bc_801393FC) {
        psx_check_interrupts_at(cpu, 0x80139408u);
        goto block_80139408;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139404u);
        goto block_80139404;  /* not taken */
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
    int _bc_8013942C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (_bc_8013942C) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139434u);
        goto block_80139434;  /* not taken */
    }

block_80139434:
    s0 = cpu->read_word(s1 + 96);
    /* nop */  /* 0x80139438: 0x00000000 */
    v0 = cpu->read_word(s0 + 12);
    v1 = 3;
    v0 = v0 + 1;
    int _bc_80139448 = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 12, v0);
    if (_bc_80139448) {
        psx_check_interrupts_at(cpu, 0x8013946Cu);
        goto block_8013946C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139450u);
        goto block_80139450;  /* not taken */
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
    int _bc_801394B0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801394B4: 0x00000000 */
    if (_bc_801394B0) {
        psx_check_interrupts_at(cpu, 0x801394BCu);
        goto block_801394BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394B8u);
        goto block_801394B8;  /* not taken */
    }

block_801394B8:
    v0 = v0 + 3;
    /* fall through to block_801394BC */

block_801394BC:
    v0 = (int32_t)v0 >> 2;
    a0 = (int32_t)(int16_t)cpu->read_half(s0 + 2);
    a1 = cpu->read_half(s0 + 2);
    v0 = v1 - v0;
    int _bc_801394CC = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0, (uint16_t)v0);
    if (_bc_801394CC) {
        psx_check_interrupts_at(cpu, 0x801394D8u);
        goto block_801394D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394D4u);
        goto block_801394D4;  /* not taken */
    }

block_801394D4:
    a0 = a0 + 3;
    /* fall through to block_801394D8 */

block_801394D8:
    v0 = (int32_t)a0 >> 2;
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 4);
    a0 = cpu->read_half(s0 + 4);
    v0 = a1 - v0;
    int _bc_801394E8 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 2, (uint16_t)v0);
    if (_bc_801394E8) {
        psx_check_interrupts_at(cpu, 0x801394F4u);
        goto block_801394F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394F0u);
        goto block_801394F0;  /* not taken */
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
    int _bc_8013950C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (_bc_8013950C) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139514u);
        goto block_80139514;  /* not taken */
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
    int _bc_80139568 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (_bc_80139568) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139570u);
        goto block_80139570;  /* not taken */
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
    int _bc_801395CC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;
    if (_bc_801395CC) {
        psx_check_interrupts_at(cpu, 0x801395D8u);
        goto block_801395D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801395D4u);
        goto block_801395D4;  /* not taken */
    }

block_801395D4:
    cpu->write_word(s1 + 84, v0);
    /* fall through to block_801395D8 */

block_801395D8:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 90);
    /* nop */  /* 0x801395DC: 0x00000000 */
    int _bc_801395E0 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;
    if (_bc_801395E0) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        goto block_80139834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801395E8u);
        goto block_801395E8;  /* not taken */
    }

block_801395E8:
    v0 = cpu->read_half(a0 + 2);
    /* nop */  /* 0x801395EC: 0x00000000 */
    v0 = v0 << 16;
    v1 = (int32_t)v0 >> 16;
    v0 = (uint32_t)v0 >> 31;
    v0 = v1 + v0;
    int _bc_80139600 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)v0 >> 1;
    if (_bc_80139600) {
        psx_check_interrupts_at(cpu, 0x8013960Cu);
        goto block_8013960C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139608u);
        goto block_80139608;  /* not taken */
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
    int _bc_801397A4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (_bc_801397A4) {
        psx_check_interrupts_at(cpu, 0x801397B4u);
        goto block_801397B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801397ACu);
        goto block_801397AC;  /* not taken */
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
    int _bc_80139814 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (_bc_80139814) {
        psx_check_interrupts_at(cpu, 0x80139824u);
        goto block_80139824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013981Cu);
        goto block_8013981C;  /* not taken */
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
    int _bc_8013986C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139870: 0x00000000 */
    if (_bc_8013986C) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139874u);
        goto block_80139874;  /* not taken */
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
    int _bc_80139984 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (_bc_80139984) {
        psx_check_interrupts_at(cpu, 0x80139994u);
        goto block_80139994;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013998Cu);
        goto block_8013998C;  /* not taken */
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
    int _bc_801399F4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = v1 - a1;
    if (_bc_801399F4) {
        psx_check_interrupts_at(cpu, 0x80139A04u);
        goto block_80139A04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801399FCu);
        goto block_801399FC;  /* not taken */
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
    int _bc_80139AB0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139AB4: 0x00000000 */
    if (_bc_80139AB0) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        goto block_80139DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AB8u);
        goto block_80139AB8;  /* not taken */
    }

block_80139AB8:
    v0 = cpu->read_half(s4 + 130);
    s2 = cpu->read_word(s1 + 96);
    int _bc_80139AC0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139AC4: 0x00000000 */
    if (_bc_80139AC0) {
        psx_check_interrupts_at(cpu, 0x80139B78u);
        goto block_80139B78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AC8u);
        goto block_80139AC8;  /* not taken */
    }

block_80139AC8:
    a0 = cpu->read_half(s2 + 2);
    v0 = cpu->read_half(s2);
    v1 = a0;
    v0 = v0 + a0;
    v1 = v1 + -50;
    cpu->write_half(s2 + 2, (uint16_t)v1);
    v1 = v1 << 16;
    int _bc_80139AE4 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s2, (uint16_t)v0);
    if (_bc_80139AE4) {
        psx_check_interrupts_at(cpu, 0x80139AF0u);
        goto block_80139AF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AECu);
        goto block_80139AEC;  /* not taken */
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
    int _bc_80139BA4 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = s0;
    if (_bc_80139BA4) {
        psx_check_interrupts_at(cpu, 0x80139BB8u);
        goto block_80139BB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139BACu);
        goto block_80139BAC;  /* not taken */
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
    uint32_t _jt_80139E04 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 64;
    psx_check_interrupts_at(cpu, _jt_80139E04);
    cpu->pc = _jt_80139E04; return;  /* CPS: jr $ra */
}

/* ======== ev_cmd_dispatch_alias (alias body guest 0x80141D2C) ======== */

/* Overlapping-alias body for br2eu_func_80141D2C (ev_cmd_dispatch @0x80141D2C): 16 event-command entries (0x80141D90..0x801420E0) plus 48 CPS blocks. Faithful CPS transliteration of generated/SLES_017.22_full_11.c br2eu_psx_alias_body_80141D2C.
 * cpu->pc dispatch: continuation PCs hit the _cont switch; alias entry
 * addrs fall through to the entry switch (entry = cpu->pc). */
void ev_cmd_dispatch_alias(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t entry;
  if (!cpu) return;
  entry = cpu->pc;
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80141DCCu: goto block_80141DCC;
            case 0x80141DE8u: goto block_80141DE8;
            case 0x80141E10u: goto block_80141E10;
            case 0x80141E38u: goto block_80141E38;
            case 0x80141E68u: goto block_80141E68;
            case 0x80141E7Cu: goto block_80141E7C;
            case 0x80141E8Cu: goto block_80141E8C;
            case 0x80141EA4u: goto block_80141EA4;
            case 0x80141EBCu: goto block_80141EBC;
            case 0x80141EF0u: goto block_80141EF0;
            case 0x80141F08u: goto block_80141F08;
            case 0x80141F4Cu: goto block_80141F4C;
            case 0x80141F78u: goto block_80141F78;
            case 0x80141F90u: goto block_80141F90;
            case 0x80141FBCu: goto block_80141FBC;
            case 0x8014207Cu: goto block_8014207C;
            case 0x801420ACu: goto block_801420AC;
            case 0x801420ECu: goto block_801420EC;
            case 0x80141DA8u: goto block_80141DA8;
            case 0x80141DB0u: goto block_80141DB0;
            case 0x80141DB8u: goto block_80141DB8;
            case 0x80141DC0u: goto block_80141DC0;
            case 0x80141E74u: goto block_80141E74;
            case 0x80141EE0u: goto block_80141EE0;
            case 0x80141F28u: goto block_80141F28;
            case 0x80141F38u: goto block_80141F38;
            case 0x80141F54u: goto block_80141F54;
            case 0x80141FE8u: goto block_80141FE8;
            case 0x80141FF4u: goto block_80141FF4;
            case 0x80142000u: goto block_80142000;
            case 0x801420B8u: goto block_801420B8;
            case 0x801420F0u: goto block_801420F0;
            default: break;
        }
    }
    switch (entry) {
        case 0x80141D90u: goto block_80141D90;
        case 0x80141DD4u: goto block_80141DD4;
        case 0x80141DF0u: goto block_80141DF0;
        case 0x80141E18u: goto block_80141E18;
        case 0x80141E40u: goto block_80141E40;
        case 0x80141E84u: goto block_80141E84;
        case 0x80141E94u: goto block_80141E94;
        case 0x80141EACu: goto block_80141EAC;
        case 0x80141EC4u: goto block_80141EC4;
        case 0x80141EF8u: goto block_80141EF8;
        case 0x80141F10u: goto block_80141F10;
        case 0x80141F80u: goto block_80141F80;
        case 0x80141F98u: goto block_80141F98;
        case 0x80141FC4u: goto block_80141FC4;
        case 0x80142084u: goto block_80142084;
        case 0x801420E0u: goto block_801420E0;
        default: return;
    }

block_80141D90:
    v0 = cpu->read_word(s1 + 56);
    /* nop */  /* 0x80141D94: 0x00000000 */
    v0 = cpu->read_byte(v0 + 256);
    /* nop */  /* 0x80141D9C: 0x00000000 */
    int _bc_80141DA0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a3 & 0x0080;  /* 0x80141DA4: 0x30E20080 */
    if (_bc_80141DA0) {
        psx_check_interrupts_at(cpu, 0x80141DB8u);
        goto block_80141DB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141DA8u);
        goto block_80141DA8;  /* not taken */
    }

block_80141DA8:
    int _bc_80141DA8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s1;
    if (_bc_80141DA8) {
        psx_check_interrupts_at(cpu, 0x80141DB8u);
        goto block_80141DB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141DB0u);
        goto block_80141DB0;  /* not taken */
    }

block_80141DB0:
    /* delay slot (always executes) */
    a1 = a3 & 0x00FF;  /* 0x80141DB4: 0x30E500FF */
    psx_check_interrupts_at(cpu, 0x80141DC0u);
    goto block_80141DC0;  /* j */

block_80141DB8:
    a0 = s1;
    a1 = a3 & 0x007F;  /* 0x80141DBC: 0x30E5007F */
    /* fall through to block_80141DC0 */

block_80141DC0:
    a2 = a3 << 16;
    ra = 0x80141DCCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = (int32_t)a2 >> 24;
    psx_check_interrupts_at(cpu, 0x80140710u);
    cpu->pc = 0x80140710u; return;  /* CPS jal -> 0x80140710 */

block_80141DCC:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141DD4:
    a0 = cpu->read_byte(s1 + 2);
    a1 = a3 & 0x00FF;  /* 0x80141DD8: 0x30E500FF */
    a2 = a3 << 16;
    ra = 0x80141DE8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = (int32_t)a2 >> 24;
    psx_check_interrupts_at(cpu, 0x80138A24u);
    cpu->pc = 0x80138A24u; return;  /* CPS jal -> 0x80138A24 */

block_80141DE8:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141DF0:
    a2 = a3 << 16;
    a2 = (int32_t)a2 >> 24;
    a3 = a3 & 0x00FF;  /* 0x80141DF8: 0x30E700FF */
    a0 = cpu->read_word(s1 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s1 + 78);
    a0 = a0 + 292;
    ra = 0x80141E10u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + -1024;
    psx_check_interrupts_at(cpu, 0x8013F178u);
    cpu->pc = 0x8013F178u; return;  /* CPS jal -> 0x8013F178 */

block_80141E10:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141E18:
    a2 = a3 << 16;
    a2 = (int32_t)a2 >> 24;
    a3 = a3 & 0x00FF;  /* 0x80141E20: 0x30E700FF */
    a0 = cpu->read_word(s1 + 32);
    a1 = (int32_t)(int16_t)cpu->read_half(s1 + 78);
    a0 = a0 + 292;
    ra = 0x80141E38u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 1024;
    psx_check_interrupts_at(cpu, 0x8013F178u);
    cpu->pc = 0x8013F178u; return;  /* CPS jal -> 0x8013F178 */

block_80141E38:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141E40:
    a1 = 6;
    a2 = 0x8000 << 16;  /* 0x80000000 */
    a2 = a2 | 0x0010;
    a3 = 2;
    v0 = 0x801B << 16;  /* 0x801B0000 */
    a0 = cpu->read_word(s1 + 32);
    v0 = cpu->read_word(v0 + 7492);
    a0 = a0 + 292;
    ra = 0x80141E68u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, v0);
    psx_check_interrupts_at(cpu, 0x8013B088u);
    cpu->pc = 0x8013B088u; return;  /* CPS jal -> 0x8013B088 */

block_80141E68:
    s0 = v0;
    int _bc_80141E6C = (s0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s2 + 4;
    if (_bc_80141E6C) {
        psx_check_interrupts_at(cpu, 0x801420F0u);
        goto block_801420F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141E74u);
        goto block_80141E74;  /* not taken */
    }

block_80141E74:
    ra = 0x80141E7Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80141E78: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E4D0u);
    cpu->pc = 0x8018E4D0u; return;  /* CPS jal -> 0x8018E4D0 */

block_80141E7C:
    /* delay slot (always executes) */
    cpu->write_half(s0, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801420ECu);
    goto block_801420EC;  /* j */

block_80141E84:
    ra = 0x80141E8Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x8013F5F8u);
    cpu->pc = 0x8013F5F8u; return;  /* CPS jal -> 0x8013F5F8 */

block_80141E8C:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141E94:
    a0 = s1;
    a1 = a3 << 16;
    ra = 0x80141EA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)a1 >> 16;
    psx_check_interrupts_at(cpu, 0x80140B18u);
    cpu->pc = 0x80140B18u; return;  /* CPS jal -> 0x80140B18 */

block_80141EA4:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141EAC:
    a0 = s1;
    a1 = a3 << 16;
    ra = 0x80141EBCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)a1 >> 16;
    psx_check_interrupts_at(cpu, 0x8013DA2Cu);
    cpu->pc = 0x8013DA2Cu; return;  /* CPS jal -> 0x8013DA2C */

block_80141EBC:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141EC4:
    v0 = 0x1F80 << 16;  /* 0x1F800000 */
    v0 = cpu->read_word(v0 + 196);
    /* nop */  /* 0x80141ECC: 0x00000000 */
    v0 = cpu->read_byte(v0 + 2);
    /* nop */  /* 0x80141ED4: 0x00000000 */
    int _bc_80141ED8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s1;
    if (_bc_80141ED8) {
        psx_check_interrupts_at(cpu, 0x801420ECu);
        goto block_801420EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141EE0u);
        goto block_80141EE0;  /* not taken */
    }

block_80141EE0:
    a1 = a3 << 16;
    a1 = (int32_t)a1 >> 24;
    ra = 0x80141EF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a3 & 0x00FF;  /* 0x80141EEC: 0x30E600FF */
    psx_check_interrupts_at(cpu, 0x8013AB8Cu);
    cpu->pc = 0x8013AB8Cu; return;  /* CPS jal -> 0x8013AB8C */

block_80141EF0:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141EF8:
    a0 = s1;
    a1 = a3 << 16;
    ra = 0x80141F08u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)a1 >> 16;
    psx_check_interrupts_at(cpu, 0x8013B5A8u);
    cpu->pc = 0x8013B5A8u; return;  /* CPS jal -> 0x8013B5A8 */

block_80141F08:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141F10:
    v0 = cpu->read_word(s1 + 56);
    v1 = a3 << 16;
    v0 = cpu->read_byte(v0 + 256);
    /* nop */  /* 0x80141F1C: 0x00000000 */
    int _bc_80141F20 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = (int32_t)v1 >> 16;
    if (_bc_80141F20) {
        psx_check_interrupts_at(cpu, 0x80141F38u);
        goto block_80141F38;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141F28u);
        goto block_80141F28;  /* not taken */
    }

block_80141F28:
    v0 = 0x801B << 16;  /* 0x801B0000 */
    v0 = v0 + 6156;
    v0 = s0 + v0;
    s0 = cpu->read_byte(v0);
    /* fall through to block_80141F38 */

block_80141F38:
    a0 = 1;
    a1 = a0;
    a2 = zero;
    ra = 0x80141F4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 15;
    psx_check_interrupts_at(cpu, 0x801AD38Cu);
    cpu->pc = 0x801AD38Cu; return;  /* CPS jal -> 0x801AD38C */

block_80141F4C:
    int _bc_80141F4C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = v0;
    if (_bc_80141F4C) {
        psx_check_interrupts_at(cpu, 0x801420ECu);
        goto block_801420EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141F54u);
        goto block_80141F54;  /* not taken */
    }

block_80141F54:
    a2 = 0x801F << 16;  /* 0x801F0000 */
    a2 = a2 + 1888;
    v0 = s0 << 2;
    v0 = v0 + s0;
    a1 = cpu->read_word(s1 + 32);
    v0 = v0 << 4;
    a1 = a1 + v0;
    ra = 0x80141F78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 52;
    psx_check_interrupts_at(cpu, 0x801AD224u);
    cpu->pc = 0x801AD224u; return;  /* CPS jal -> 0x801AD224 */

block_80141F78:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141F80:
    a0 = cpu->read_byte(s1 + 3);
    a1 = a3 << 16;
    ra = 0x80141F90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)a1 >> 16;
    psx_check_interrupts_at(cpu, 0x8014140Cu);
    cpu->pc = 0x8014140Cu; return;  /* CPS jal -> 0x8014140C */

block_80141F90:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141F98:
    v0 = a3 << 16;
    v0 = (int32_t)v0 >> 16;
    v1 = v0 << 2;
    v1 = v1 + v0;
    a0 = cpu->read_word(s1 + 32);
    v1 = v1 << 4;
    a0 = a0 + v1;
    ra = 0x80141FBCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 52;
    psx_check_interrupts_at(cpu, 0x80141050u);
    cpu->pc = 0x80141050u; return;  /* CPS jal -> 0x80141050 */

block_80141FBC:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80141FC4:
    v1 = a3 << 16;
    v0 = (int32_t)v1 >> 24;
    a0 = v0 & 0x007F;  /* 0x80141FCC: 0x3044007F */
    v0 = cpu->read_word(s1 + 56);
    v1 = (int32_t)v1 >> 16;
    v0 = cpu->read_byte(v0 + 256);
    /* nop */  /* 0x80141FDC: 0x00000000 */
    int _bc_80141FE0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = a3 & 0x00FF;  /* 0x80141FE4: 0x30E500FF */
    if (_bc_80141FE0) {
        psx_check_interrupts_at(cpu, 0x80142000u);
        goto block_80142000;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141FE8u);
        goto block_80141FE8;  /* not taken */
    }

block_80141FE8:
    v0 = v1 & 0x8000;  /* 0x80141FE8: 0x30628000 */
    int _bc_80141FEC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (_bc_80141FEC) {
        psx_check_interrupts_at(cpu, 0x80142000u);
        goto block_80142000;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80141FF4u);
        goto block_80141FF4;  /* not taken */
    }

block_80141FF4:
    v0 = v0 + 6156;
    v0 = a0 + v0;
    a0 = cpu->read_byte(v0);
    /* fall through to block_80142000 */

block_80142000:
    /* nop */  /* 0x80142000: 0x00000000 */
    v1 = a0 << 2;
    v1 = v1 + a0;
    v0 = cpu->read_word(s1 + 32);
    v1 = v1 << 4;
    v0 = v1 + v0;
    v0 = cpu->read_half(v0 + 52);
    /* nop */  /* 0x8014201C: 0x00000000 */
    cpu->write_half(sp + 32, (uint16_t)v0);
    v0 = cpu->read_word(s1 + 32);
    /* nop */  /* 0x80142028: 0x00000000 */
    v0 = v1 + v0;
    v0 = cpu->read_half(v0 + 56);
    /* nop */  /* 0x80142034: 0x00000000 */
    cpu->write_half(sp + 34, (uint16_t)v0);
    v0 = cpu->read_word(s1 + 32);
    a0 = sp + 32;
    v1 = v1 + v0;
    v0 = cpu->read_half(v1 + 60);
    v1 = a1 << 2;
    cpu->write_half(sp + 36, (uint16_t)v0);
    v0 = 0x801F << 16;  /* 0x801F0000 */
    v0 = v0 + 1936;
    v0 = v1 + v0;
    a1 = cpu->read_word(v0);
    v0 = 0x801F << 16;  /* 0x801F0000 */
    v0 = v0 + 1920;
    v1 = v1 + v0;
    a2 = cpu->read_word(v1);
    ra = 0x8014207Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = zero;
    psx_check_interrupts_at(cpu, 0x8013ED98u);
    cpu->pc = 0x8013ED98u; return;  /* CPS jal -> 0x8013ED98 */

block_8014207C:
    /* delay slot (always executes) */
    v0 = s2 + 4;
    psx_check_interrupts_at(cpu, 0x801420F0u);
    goto block_801420F0;  /* j */

block_80142084:
    a1 = 12;
    a2 = 0x8000 << 16;  /* 0x80000000 */
    a2 = a2 | 0x0012;
    a3 = 142;
    v0 = 0x801B << 16;  /* 0x801B0000 */
    a0 = cpu->read_word(s1 + 32);
    v0 = v0 + 7516;
    cpu->write_word(sp + 16, v0);
    ra = 0x801420ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 1092;
    psx_check_interrupts_at(cpu, 0x8013B088u);
    cpu->pc = 0x8013B088u; return;  /* CPS jal -> 0x8013B088 */

block_801420AC:
    a1 = v0;
    int _bc_801420B0 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_801420B0) {
        psx_check_interrupts_at(cpu, 0x801420ECu);
        goto block_801420EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801420B8u);
        goto block_801420B8;  /* not taken */
    }

block_801420B8:
    cpu->write_word(a1, s1);
    v1 = cpu->read_byte(s1 + 2);
    a0 = a0 | 0x00E8;
    v1 = v1 ^ 0x0001;  /* 0x801420C4: 0x38630001 */
    v0 = v1 << 5;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = v0 + a0;
    /* delay slot (always executes) */
    cpu->write_word(a1 + 4, v0);
    psx_check_interrupts_at(cpu, 0x801420ECu);
    goto block_801420EC;  /* j */

block_801420E0:
    a0 = 1;
    ra = 0x801420ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    psx_check_interrupts_at(cpu, 0x80140CA0u);
    cpu->pc = 0x80140CA0u; return;  /* CPS jal -> 0x80140CA0 */

block_801420EC:
    v0 = s2 + 4;
    /* fall through to block_801420F0 */

block_801420F0:
    ra = cpu->read_word(sp + 52);
    s2 = cpu->read_word(sp + 48);
    s1 = cpu->read_word(sp + 44);
    s0 = cpu->read_word(sp + 40);
    uint32_t _jt_80142100 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 56;
    psx_check_interrupts_at(cpu, _jt_80142100);
    cpu->pc = _jt_80142100; return;  /* CPS: jr $ra */
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
    int _bc_80142900 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 255) ? 1 : 0;  /* 0x80142904: 0x286200FF */
    if (_bc_80142900) {
        psx_check_interrupts_at(cpu, 0x8014294Cu);
        goto block_8014294C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142908u);
        goto block_80142908;  /* not taken */
    }

block_80142908:
    int _bc_80142908 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 255;
    if (_bc_80142908) {
        psx_check_interrupts_at(cpu, 0x80142920u);
        goto block_80142920;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142910u);
        goto block_80142910;  /* not taken */
    }

block_80142910:
    int _bc_80142910 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s0 << 16;
    if (_bc_80142910) {
        psx_check_interrupts_at(cpu, 0x80142930u);
        goto block_80142930;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142918u);
        goto block_80142918;  /* not taken */
    }

block_80142918:
    /* delay slot (always executes) */
    /* nop */  /* 0x8014291C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142B00u);
    cpu->pc = 0x80142B00u; return;  /* CPS j: split */

block_80142920:
    int _bc_80142920 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_80142920) {
        psx_check_interrupts_at(cpu, 0x80142944u);
        goto block_80142944;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142928u);
        goto block_80142928;  /* not taken */
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
    int _bc_80142958 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_80142958) {
        psx_check_interrupts_at(cpu, 0x80142970u);
        goto block_80142970;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142960u);
        goto block_80142960;  /* not taken */
    }

block_80142960:
    int _bc_80142960 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    if (_bc_80142960) {
        psx_check_interrupts_at(cpu, 0x801429C4u);
        goto block_801429C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142968u);
        goto block_80142968;  /* not taken */
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

block_80142988:
    int _bc_80142988 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014298C: 0x00000000 */
    if (_bc_80142988) {
        psx_check_interrupts_at(cpu, 0x80142B00u);
        cpu->pc = 0x80142B00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80142990u);
        goto block_80142990;  /* not taken */
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
    int _bc_801429F8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_801429F8) {
        psx_check_interrupts_at(cpu, 0x80142B00u);
        cpu->pc = 0x80142B00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80142A00u);
        goto block_80142A00;  /* not taken */
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

/* ======== action_from_input_alias (alias body guest 0x80142F3C) ======== */

/* Overlapping-alias body for br2eu_func_80142F3C (action_from_input @0x80142F3C): 8 command entries (0x80143004..0x80143384) plus 75 CPS blocks. Faithful CPS transliteration of generated/SLES_017.22_full_11.c br2eu_psx_alias_body_80142F3C.
 * cpu->pc dispatch: continuation PCs hit the _cont switch; alias entry
 * addrs fall through to the entry switch (entry = cpu->pc). */
void action_from_input_alias(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t entry;
  if (!cpu) return;
  entry = cpu->pc;
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80142F74u: goto block_80142F74;
            case 0x80142F84u: goto block_80142F84;
            case 0x80142F90u: goto block_80142F90;
            case 0x80142FA0u: goto block_80142FA0;
            case 0x80142FB4u: goto block_80142FB4;
            case 0x80142FBCu: goto block_80142FBC;
            case 0x80142FE8u: goto block_80142FE8;
            case 0x8014301Cu: goto block_8014301C;
            case 0x80143030u: goto block_80143030;
            case 0x80143038u: goto block_80143038;
            case 0x80143040u: goto block_80143040;
            case 0x80143054u: goto block_80143054;
            case 0x8014305Cu: goto block_8014305C;
            case 0x80143070u: goto block_80143070;
            case 0x80143084u: goto block_80143084;
            case 0x80143090u: goto block_80143090;
            case 0x80143098u: goto block_80143098;
            case 0x801430B4u: goto block_801430B4;
            case 0x801430C0u: goto block_801430C0;
            case 0x801430DCu: goto block_801430DC;
            case 0x801430F0u: goto block_801430F0;
            case 0x80143104u: goto block_80143104;
            case 0x80143118u: goto block_80143118;
            case 0x80143124u: goto block_80143124;
            case 0x80143130u: goto block_80143130;
            case 0x80143144u: goto block_80143144;
            case 0x80143158u: goto block_80143158;
            case 0x8014316Cu: goto block_8014316C;
            case 0x80143170u: goto block_80143170;
            case 0x80143198u: goto block_80143198;
            case 0x801431A4u: goto block_801431A4;
            case 0x801431B8u: goto block_801431B8;
            case 0x801431C4u: goto block_801431C4;
            case 0x801431CCu: goto block_801431CC;
            case 0x801431E4u: goto block_801431E4;
            case 0x80143220u: goto block_80143220;
            case 0x80143228u: goto block_80143228;
            case 0x80143238u: goto block_80143238;
            case 0x8014324Cu: goto block_8014324C;
            case 0x80143254u: goto block_80143254;
            case 0x80143260u: goto block_80143260;
            case 0x80143278u: goto block_80143278;
            case 0x801432B4u: goto block_801432B4;
            case 0x801432CCu: goto block_801432CC;
            case 0x801432D0u: goto block_801432D0;
            case 0x801432D8u: goto block_801432D8;
            case 0x801432ECu: goto block_801432EC;
            case 0x801432F0u: goto block_801432F0;
            case 0x801432FCu: goto block_801432FC;
            case 0x80143314u: goto block_80143314;
            case 0x8014339Cu: goto block_8014339C;
            case 0x801433A4u: goto block_801433A4;
            case 0x801433B8u: goto block_801433B8;
            case 0x801433CCu: goto block_801433CC;
            case 0x801433D0u: goto block_801433D0;
            case 0x801433E4u: goto block_801433E4;
            case 0x801433F4u: goto block_801433F4;
            case 0x80143404u: goto block_80143404;
            case 0x8014342Cu: goto block_8014342C;
            case 0x80143448u: goto block_80143448;
            case 0x80143458u: goto block_80143458;
            case 0x80143480u: goto block_80143480;
            case 0x80143484u: goto block_80143484;
            case 0x801434ACu: goto block_801434AC;
            case 0x801434B0u: goto block_801434B0;
            case 0x801434B4u: goto block_801434B4;
            case 0x801434CCu: goto block_801434CC;
            default: break;
        }
    }
    switch (entry) {
        case 0x80143004u: goto block_80143004;
        case 0x80143330u: goto block_80143330;
        case 0x8014333Cu: goto block_8014333C;
        case 0x80143348u: goto block_80143348;
        case 0x80143354u: goto block_80143354;
        case 0x80143360u: goto block_80143360;
        case 0x8014336Cu: goto block_8014336C;
        case 0x80143384u: goto block_80143384;
        default: return;
    }

block_80142F74:
    v1 = cpu->read_word(t5);
    /* nop */  /* 0x80142F78: 0x00000000 */
    int _bc_80142F7C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0010 << 16;  /* 0x00100000 */
    if (_bc_80142F7C) {
        psx_check_interrupts_at(cpu, 0x801434CCu);
        goto block_801434CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142F84u);
        goto block_80142F84;  /* not taken */
    }

block_80142F84:
    v0 = v1 & v0;  /* 0x80142F84: 0x00621024 */
    int _bc_80142F88 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801C << 16;  /* 0x801C0000 */
    if (_bc_80142F88) {
        psx_check_interrupts_at(cpu, 0x80142FA0u);
        goto block_80142FA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142F90u);
        goto block_80142F90;  /* not taken */
    }

block_80142F90:
    v0 = cpu->read_word(v0 + 17784);
    /* nop */  /* 0x80142F94: 0x00000000 */
    int _bc_80142F98 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80142F9C: 0x00000000 */
    if (_bc_80142F98) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142FA0u);
        goto block_80142FA0;  /* not taken */
    }

block_80142FA0:
    v0 = (int32_t)(int16_t)cpu->read_half(t1);
    /* nop */  /* 0x80142FA4: 0x00000000 */
    v1 = v0 + -1;
    int _bc_80142FAC = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (_bc_80142FAC) {
        psx_check_interrupts_at(cpu, 0x80142FBCu);
        goto block_80142FBC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142FB4u);
        goto block_80142FB4;  /* not taken */
    }

block_80142FB4:
    v1 = zero;
    v0 = v1 << 2;
    /* fall through to block_80142FBC */

block_80142FBC:
    v0 = v0 + v1;
    v0 = v0 << 1;
    v1 = cpu->read_word(a3 + 4);
    a0 = cpu->read_word(t2 + 48);
    a1 = v1 + v0;
    a1 = a1 + a0;
    v1 = (int32_t)(int16_t)cpu->read_half(a1 + 4);
    /* nop */  /* 0x80142FD8: 0x00000000 */
    v0 = (v1 < (uint32_t)8) ? 1 : 0;  /* 0x80142FDC: 0x2C620008 */
    int _bc_80142FE0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_80142FE0) {
        psx_check_interrupts_at(cpu, 0x801433A4u);
        goto block_801433A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80142FE8u);
        goto block_80142FE8;  /* not taken */
    }

block_80142FE8:
    v0 = v0 + 588;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80142FF8: 0x00000000 */
    uint32_t _jt_80142FFC = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143000: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80142FFC);
    cpu->pc = _jt_80142FFC; return;  /* CPS: jr v0 */

block_80143004:
    cpu->write_half(t1, (uint16_t)zero);
    v1 = cpu->read_word(t5);
    v0 = 0x0008 << 16;  /* 0x00080000 */
    v0 = v1 & v0;  /* 0x80143010: 0x00621024 */
    int _bc_80143014 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x0040;  /* 0x80143018: 0x30620040 */
    if (_bc_80143014) {
        psx_check_interrupts_at(cpu, 0x80143038u);
        goto block_80143038;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014301Cu);
        goto block_8014301C;  /* not taken */
    }

block_8014301C:
    v0 = cpu->read_byte(t2 + 5);
    /* nop */  /* 0x80143020: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x80143024: 0x30420001 */
    int _bc_80143028 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014302C: 0x00000000 */
    if (_bc_80143028) {
        psx_check_interrupts_at(cpu, 0x80143070u);
        goto block_80143070;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143030u);
        goto block_80143030;  /* not taken */
    }

block_80143030:
    /* delay slot (always executes) */
    t0 = t0 + 8;
    psx_check_interrupts_at(cpu, 0x801434B4u);
    goto block_801434B4;  /* j */

block_80143038:
    int _bc_80143038 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014303C: 0x00000000 */
    if (_bc_80143038) {
        psx_check_interrupts_at(cpu, 0x80143070u);
        goto block_80143070;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143040u);
        goto block_80143040;  /* not taken */
    }

block_80143040:
    v0 = cpu->read_byte(t2 + 5);
    /* nop */  /* 0x80143044: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x80143048: 0x30420001 */
    int _bc_8014304C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x0080;  /* 0x80143050: 0x30620080 */
    if (_bc_8014304C) {
        psx_check_interrupts_at(cpu, 0x80143070u);
        goto block_80143070;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143054u);
        goto block_80143054;  /* not taken */
    }

block_80143054:
    int _bc_80143054 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143058: 0x00000000 */
    if (_bc_80143054) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014305Cu);
        goto block_8014305C;  /* not taken */
    }

block_8014305C:
    v0 = (int32_t)(int16_t)cpu->read_half(t2 + 214);
    /* nop */  /* 0x80143060: 0x00000000 */
    v0 = ((int32_t)v0 < 7680) ? 1 : 0;  /* 0x80143064: 0x28421E00 */
    int _bc_80143068 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014306C: 0x00000000 */
    if (_bc_80143068) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143070u);
        goto block_80143070;  /* not taken */
    }

block_80143070:
    a1 = cpu->read_word(t5);
    /* nop */  /* 0x80143074: 0x00000000 */
    v0 = a1 & 0x0080;  /* 0x80143078: 0x30A20080 */
    int _bc_8014307C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a1 & 0x3000;  /* 0x80143080: 0x30A33000 */
    if (_bc_8014307C) {
        psx_check_interrupts_at(cpu, 0x80143124u);
        goto block_80143124;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143084u);
        goto block_80143084;  /* not taken */
    }

block_80143084:
    v0 = 12288;
    int _bc_80143088 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a1 & 0x1000;  /* 0x8014308C: 0x30A21000 */
    if (_bc_80143088) {
        psx_check_interrupts_at(cpu, 0x801430DCu);
        goto block_801430DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143090u);
        goto block_80143090;  /* not taken */
    }

block_80143090:
    int _bc_80143090 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0040 << 16;  /* 0x00400000 */
    if (_bc_80143090) {
        psx_check_interrupts_at(cpu, 0x801430B4u);
        goto block_801430B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143098u);
        goto block_80143098;  /* not taken */
    }

block_80143098:
    v0 = cpu->read_word(t2 + 44);
    /* nop */  /* 0x8014309C: 0x00000000 */
    v0 = cpu->read_word(v0 + 92);
    /* nop */  /* 0x801430A4: 0x00000000 */
    v0 = v0 & v1;  /* 0x801430A8: 0x00431024 */
    int _bc_801430AC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801430B0: 0x00000000 */
    if (_bc_801430AC) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801430B4u);
        goto block_801430B4;  /* not taken */
    }

block_801430B4:
    v0 = a1 & 0x2000;  /* 0x801430B4: 0x30A22000 */
    int _bc_801430B8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0040 << 16;  /* 0x00400000 */
    if (_bc_801430B8) {
        psx_check_interrupts_at(cpu, 0x801430DCu);
        goto block_801430DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801430C0u);
        goto block_801430C0;  /* not taken */
    }

block_801430C0:
    v0 = cpu->read_word(t2 + 44);
    /* nop */  /* 0x801430C4: 0x00000000 */
    v0 = cpu->read_word(v0 + 92);
    /* nop */  /* 0x801430CC: 0x00000000 */
    v0 = v0 & v1;  /* 0x801430D0: 0x00431024 */
    int _bc_801430D4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801430D8: 0x00000000 */
    if (_bc_801430D4) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801430DCu);
        goto block_801430DC;  /* not taken */
    }

block_801430DC:
    v0 = cpu->read_word(t2 + 88);
    /* nop */  /* 0x801430E0: 0x00000000 */
    v0 = v0 & 0x0800;  /* 0x801430E4: 0x30420800 */
    int _bc_801430E8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801430EC: 0x00000000 */
    if (_bc_801430E8) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801430F0u);
        goto block_801430F0;  /* not taken */
    }

block_801430F0:
    v0 = cpu->read_word(a2 + 32);
    /* nop */  /* 0x801430F4: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801430F8: 0x30428000 */
    int _bc_801430FC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143100: 0x00000000 */
    if (_bc_801430FC) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143104u);
        goto block_80143104;  /* not taken */
    }

block_80143104:
    v0 = (int32_t)(int16_t)cpu->read_half(a2 + 42);
    /* nop */  /* 0x80143108: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)t4) ? 1 : 0;  /* 0x8014310C: 0x004C102A */
    int _bc_80143110 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143114: 0x00000000 */
    if (_bc_80143110) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143118u);
        goto block_80143118;  /* not taken */
    }

block_80143118:
    v0 = cpu->read_word(t5);
    /* delay slot (always executes) */
    cpu->write_word(a2 + 32, v0);
    psx_check_interrupts_at(cpu, 0x80143170u);
    goto block_80143170;  /* j */

block_80143124:
    v0 = a1 & 0x0100;  /* 0x80143124: 0x30A20100 */
    int _bc_80143128 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_80143128) {
        psx_check_interrupts_at(cpu, 0x80143198u);
        goto block_80143198;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143130u);
        goto block_80143130;  /* not taken */
    }

block_80143130:
    v0 = cpu->read_word(t2 + 88);
    /* nop */  /* 0x80143134: 0x00000000 */
    v0 = v0 & 0x0800;  /* 0x80143138: 0x30420800 */
    int _bc_8014313C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143140: 0x00000000 */
    if (_bc_8014313C) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143144u);
        goto block_80143144;  /* not taken */
    }

block_80143144:
    v0 = cpu->read_word(a2 + 32);
    /* nop */  /* 0x80143148: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x8014314C: 0x30428000 */
    int _bc_80143150 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143154: 0x00000000 */
    if (_bc_80143150) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143158u);
        goto block_80143158;  /* not taken */
    }

block_80143158:
    v0 = (int32_t)(int16_t)cpu->read_half(a2 + 42);
    /* nop */  /* 0x8014315C: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)t4) ? 1 : 0;  /* 0x80143160: 0x004C102A */
    int _bc_80143164 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143168: 0x00000000 */
    if (_bc_80143164) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014316Cu);
        goto block_8014316C;  /* not taken */
    }

block_8014316C:
    cpu->write_word(a2 + 32, a1);
    /* fall through to block_80143170 */

block_80143170:
    v0 = cpu->read_half(a3 + -4);
    /* nop */  /* 0x80143174: 0x00000000 */
    cpu->write_half(a2 + 36, (uint16_t)v0);
    v0 = cpu->read_half(a3 + -2);
    /* nop */  /* 0x80143180: 0x00000000 */
    cpu->write_half(a2 + 38, (uint16_t)v0);
    v0 = cpu->read_half(a3 + 2);
    cpu->write_half(a2 + 42, (uint16_t)t4);
    /* delay slot (always executes) */
    cpu->write_half(a2 + 40, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801434B0u);
    goto block_801434B0;  /* j */

block_80143198:
    v0 = a1 & v0;  /* 0x80143198: 0x00A21024 */
    int _bc_8014319C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801431A0: 0x00000000 */
    if (_bc_8014319C) {
        psx_check_interrupts_at(cpu, 0x801432FCu);
        goto block_801432FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801431A4u);
        goto block_801431A4;  /* not taken */
    }

block_801431A4:
    a0 = cpu->read_word(t2 + 88);
    /* nop */  /* 0x801431A8: 0x00000000 */
    v0 = a0 & 0x1000;  /* 0x801431AC: 0x30821000 */
    int _bc_801431B0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (_bc_801431B0) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801431B8u);
        goto block_801431B8;  /* not taken */
    }

block_801431B8:
    v0 = a1 & v0;  /* 0x801431B8: 0x00A21024 */
    int _bc_801431BC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 & 0x2800;  /* 0x801431C0: 0x30822800 */
    if (_bc_801431BC) {
        psx_check_interrupts_at(cpu, 0x80143228u);
        goto block_80143228;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801431C4u);
        goto block_801431C4;  /* not taken */
    }

block_801431C4:
    int _bc_801431C4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801431C8: 0x00000000 */
    if (_bc_801431C4) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801431CCu);
        goto block_801431CC;  /* not taken */
    }

block_801431CC:
    v0 = (int32_t)(int16_t)cpu->read_half(a2 + 52);
    v1 = (int32_t)(int16_t)cpu->read_half(a3 + -4);
    /* nop */  /* 0x801431D4: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x801431D8: 0x0043102A */
    int _bc_801431DC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801431E0: 0x00000000 */
    if (_bc_801431DC) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801431E4u);
        goto block_801431E4;  /* not taken */
    }

block_801431E4:
    cpu->write_word(a2 + 48, a1);
    v0 = cpu->read_half(a3 + -4);
    /* nop */  /* 0x801431EC: 0x00000000 */
    cpu->write_half(a2 + 52, (uint16_t)v0);
    v0 = cpu->read_half(a3 + -2);
    /* nop */  /* 0x801431F8: 0x00000000 */
    cpu->write_half(a2 + 54, (uint16_t)v0);
    a0 = cpu->read_half(a3 + 2);
    /* nop */  /* 0x80143204: 0x00000000 */
    cpu->write_half(a2 + 56, (uint16_t)a0);
    v1 = cpu->read_word(t2 + 84);
    /* nop */  /* 0x80143210: 0x00000000 */
    v0 = v1 & 0x0008;  /* 0x80143214: 0x30620008 */
    int _bc_80143218 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 << 16;
    if (_bc_80143218) {
        psx_check_interrupts_at(cpu, 0x801432B4u);
        goto block_801432B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143220u);
        goto block_80143220;  /* not taken */
    }

block_80143220:
    /* delay slot (always executes) */
    v0 = v1 & 0x0010;  /* 0x80143224: 0x30620010 */
    psx_check_interrupts_at(cpu, 0x801432D0u);
    goto block_801432D0;  /* j */

block_80143228:
    v0 = 0x0004 << 16;  /* 0x00040000 */
    v0 = a1 & v0;  /* 0x8014322C: 0x00A21024 */
    int _bc_80143230 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x8000 << 16;  /* 0x80000000 */
    if (_bc_80143230) {
        psx_check_interrupts_at(cpu, 0x80143254u);
        goto block_80143254;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143238u);
        goto block_80143238;  /* not taken */
    }

block_80143238:
    v1 = v1 | 0x2010;
    v1 = a0 & v1;  /* 0x8014323C: 0x00831824 */
    v0 = 8192;
    int _bc_80143244 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143248: 0x00000000 */
    if (_bc_80143244) {
        psx_check_interrupts_at(cpu, 0x801431CCu);
        goto block_801431CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014324Cu);
        goto block_8014324C;  /* not taken */
    }

block_8014324C:
    /* delay slot (always executes) */
    t0 = t0 + 8;
    psx_check_interrupts_at(cpu, 0x801434B4u);
    goto block_801434B4;  /* j */

block_80143254:
    v0 = a0 & 0x0800;  /* 0x80143254: 0x30820800 */
    int _bc_80143258 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014325C: 0x00000000 */
    if (_bc_80143258) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143260u);
        goto block_80143260;  /* not taken */
    }

block_80143260:
    v0 = (int32_t)(int16_t)cpu->read_half(a2 + 52);
    v1 = (int32_t)(int16_t)cpu->read_half(a3 + -4);
    /* nop */  /* 0x80143268: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x8014326C: 0x0043102A */
    int _bc_80143270 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143274: 0x00000000 */
    if (_bc_80143270) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143278u);
        goto block_80143278;  /* not taken */
    }

block_80143278:
    cpu->write_word(a2 + 48, a1);
    v0 = cpu->read_half(a3 + -4);
    /* nop */  /* 0x80143280: 0x00000000 */
    cpu->write_half(a2 + 52, (uint16_t)v0);
    v0 = cpu->read_half(a3 + -2);
    /* nop */  /* 0x8014328C: 0x00000000 */
    cpu->write_half(a2 + 54, (uint16_t)v0);
    a0 = cpu->read_half(a3 + 2);
    /* nop */  /* 0x80143298: 0x00000000 */
    cpu->write_half(a2 + 56, (uint16_t)a0);
    v1 = cpu->read_word(t2 + 84);
    /* nop */  /* 0x801432A4: 0x00000000 */
    v0 = v1 & 0x0008;  /* 0x801432A8: 0x30620008 */
    int _bc_801432AC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 << 16;
    if (_bc_801432AC) {
        psx_check_interrupts_at(cpu, 0x801432CCu);
        goto block_801432CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801432B4u);
        goto block_801432B4;  /* not taken */
    }

block_801432B4:
    v1 = (int32_t)v0 >> 16;
    v0 = (uint32_t)v0 >> 31;
    v1 = v1 + v0;
    v1 = (int32_t)v1 >> 1;
    /* delay slot (always executes) */
    cpu->write_half(a2 + 56, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x801434B0u);
    goto block_801434B0;  /* j */

block_801432CC:
    v0 = v1 & 0x0010;  /* 0x801432CC: 0x30620010 */
    /* fall through to block_801432D0 */

block_801432D0:
    int _bc_801432D0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 << 16;
    if (_bc_801432D0) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801432D8u);
        goto block_801432D8;  /* not taken */
    }

block_801432D8:
    v0 = (int32_t)v0 >> 16;
    v1 = v0 << 3;
    v0 = v1 - v0;
    int _bc_801432E4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801432E8: 0x00000000 */
    if (_bc_801432E4) {
        psx_check_interrupts_at(cpu, 0x801432F0u);
        goto block_801432F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801432ECu);
        goto block_801432EC;  /* not taken */
    }

block_801432EC:
    v0 = v0 + 7;
    /* fall through to block_801432F0 */

block_801432F0:
    v0 = (int32_t)v0 >> 3;
    /* delay slot (always executes) */
    cpu->write_half(a2 + 56, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801434B0u);
    goto block_801434B0;  /* j */

block_801432FC:
    v0 = (int32_t)(int16_t)cpu->read_half(a2 + 28);
    v1 = (int32_t)(int16_t)cpu->read_half(a3 + -4);
    /* nop */  /* 0x80143304: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x80143308: 0x0043102A */
    int _bc_8014330C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143310: 0x00000000 */
    if (_bc_8014330C) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143314u);
        goto block_80143314;  /* not taken */
    }

block_80143314:
    cpu->write_word(a2 + 24, a1);
    v0 = cpu->read_half(a3 + -4);
    /* nop */  /* 0x8014331C: 0x00000000 */
    cpu->write_half(a2 + 28, (uint16_t)v0);
    v0 = cpu->read_half(a3 + -2);
    /* delay slot (always executes) */
    cpu->write_half(a2 + 30, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801434B0u);
    goto block_801434B0;  /* j */

block_80143330:
    t3 = cpu->read_half(t2 + 124);
    /* delay slot (always executes) */
    /* nop */  /* 0x80143338: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801433A4u);
    goto block_801433A4;  /* j */

block_8014333C:
    t3 = cpu->read_half(t2 + 122);
    /* delay slot (always executes) */
    /* nop */  /* 0x80143344: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801433A4u);
    goto block_801433A4;  /* j */

block_80143348:
    t3 = cpu->read_half(t2 + 128);
    /* delay slot (always executes) */
    /* nop */  /* 0x80143350: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801433A4u);
    goto block_801433A4;  /* j */

block_80143354:
    t3 = cpu->read_half(t2 + 132);
    /* delay slot (always executes) */
    /* nop */  /* 0x8014335C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801433A4u);
    goto block_801433A4;  /* j */

block_80143360:
    t3 = cpu->read_half(t2 + 130);
    /* delay slot (always executes) */
    /* nop */  /* 0x80143368: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801433A4u);
    goto block_801433A4;  /* j */

block_8014336C:
    v1 = cpu->read_half(t2 + 124);
    v0 = cpu->read_half(t2 + 122);
    v1 = v1 & 0xF000;  /* 0x80143374: 0x3063F000 */
    v0 = v0 & 0x00E8;  /* 0x80143378: 0x304200E8 */
    /* delay slot (always executes) */
    t3 = v1 | v0;
    psx_check_interrupts_at(cpu, 0x801433A4u);
    goto block_801433A4;  /* j */

block_80143384:
    v0 = cpu->read_half(t2 + 122);
    t3 = cpu->read_half(t2 + 128);
    v1 = v0 & 0x00E8;  /* 0x8014338C: 0x304300E8 */
    v0 = 192;
    int _bc_80143394 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 136;
    if (_bc_80143394) {
        psx_check_interrupts_at(cpu, 0x801433A4u);
        goto block_801433A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014339Cu);
        goto block_8014339C;  /* not taken */
    }

block_8014339C:
    int _bc_8014339C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801433A0: 0x00000000 */
    if (_bc_8014339C) {
        psx_check_interrupts_at(cpu, 0x80143448u);
        goto block_80143448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801433A4u);
        goto block_801433A4;  /* not taken */
    }

block_801433A4:
    v0 = cpu->read_byte(t2 + 5);
    /* nop */  /* 0x801433A8: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x801433AC: 0x30420001 */
    int _bc_801433B0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801433B4: 0x00000000 */
    if (_bc_801433B0) {
        psx_check_interrupts_at(cpu, 0x801433D0u);
        goto block_801433D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801433B8u);
        goto block_801433B8;  /* not taken */
    }

block_801433B8:
    v0 = (int32_t)(int16_t)cpu->read_half(t2 + 214);
    /* nop */  /* 0x801433BC: 0x00000000 */
    v0 = ((int32_t)v0 < 7680) ? 1 : 0;  /* 0x801433C0: 0x28421E00 */
    int _bc_801433C4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801433C8: 0x00000000 */
    if (_bc_801433C4) {
        psx_check_interrupts_at(cpu, 0x801433D0u);
        goto block_801433D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801433CCu);
        goto block_801433CC;  /* not taken */
    }

block_801433CC:
    t3 = t3 & 0xFFDF;  /* 0x801433CC: 0x316BFFDF */
    /* fall through to block_801433D0 */

block_801433D0:
    v0 = cpu->read_half(a1 + 6);
    v1 = cpu->read_half(a1 + 8);
    v0 = t3 & v0;  /* 0x801433D8: 0x01621024 */
    int _bc_801433DC = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801433E0: 0x00000000 */
    if (_bc_801433DC) {
        psx_check_interrupts_at(cpu, 0x80143448u);
        goto block_80143448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801433E4u);
        goto block_801433E4;  /* not taken */
    }

block_801433E4:
    v0 = (int32_t)(int16_t)cpu->read_half(t1);
    v1 = cpu->read_half(t1);
    int _bc_801433EC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + 1;
    if (_bc_801433EC) {
        psx_check_interrupts_at(cpu, 0x80143404u);
        goto block_80143404;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801433F4u);
        goto block_801433F4;  /* not taken */
    }

block_801433F4:
    cpu->write_half(t1, (uint16_t)v0);
    cpu->write_half(t0, (uint16_t)zero);
    cpu->write_half(t0 + -2, (uint16_t)zero);
    cpu->write_half(t0 + -4, (uint16_t)zero);
    /* fall through to block_80143404 */

block_80143404:
    v0 = cpu->read_half(t0 + -4);
    /* nop */  /* 0x80143408: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_half(t0 + -4, (uint16_t)v0);
    v0 = v0 << 16;
    v1 = (int32_t)(int16_t)cpu->read_half(a1);
    v0 = (int32_t)v0 >> 16;
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x80143420: 0x0043102A */
    int _bc_80143424 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143428: 0x00000000 */
    if (_bc_80143424) {
        psx_check_interrupts_at(cpu, 0x80143484u);
        goto block_80143484;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8014342Cu);
        goto block_8014342C;  /* not taken */
    }

block_8014342C:
    v0 = cpu->read_half(t1);
    /* nop */  /* 0x80143430: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_half(t1, (uint16_t)v0);
    cpu->write_half(t0 + -4, (uint16_t)zero);
    /* delay slot (always executes) */
    cpu->write_half(t0 + -2, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x80142FA0u);
    goto block_80142FA0;  /* j */

block_80143448:
    v0 = (int32_t)(int16_t)cpu->read_half(t1);
    /* nop */  /* 0x8014344C: 0x00000000 */
    int _bc_80143450 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80143454: 0x00000000 */
    if (_bc_80143450) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143458u);
        goto block_80143458;  /* not taken */
    }

block_80143458:
    v0 = cpu->read_half(t0 + -2);
    /* nop */  /* 0x8014345C: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_half(t0 + -2, (uint16_t)v0);
    v0 = v0 << 16;
    v1 = (int32_t)(int16_t)cpu->read_half(a1 + 2);
    v0 = (int32_t)v0 >> 16;
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x80143474: 0x0043102A */
    int _bc_80143478 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8014347C: 0x00000000 */
    if (_bc_80143478) {
        psx_check_interrupts_at(cpu, 0x80143484u);
        goto block_80143484;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80143480u);
        goto block_80143480;  /* not taken */
    }

block_80143480:
    cpu->write_half(t1, (uint16_t)zero);
    /* fall through to block_80143484 */

block_80143484:
    v0 = cpu->read_half(t0);
    /* nop */  /* 0x80143488: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_half(t0, (uint16_t)v0);
    v0 = v0 << 16;
    v1 = (int32_t)(int16_t)cpu->read_half(a3);
    v0 = (int32_t)v0 >> 16;
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x801434A0: 0x0043102A */
    int _bc_801434A4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801434A8: 0x00000000 */
    if (_bc_801434A4) {
        psx_check_interrupts_at(cpu, 0x801434B0u);
        goto block_801434B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801434ACu);
        goto block_801434AC;  /* not taken */
    }

block_801434AC:
    cpu->write_half(t1, (uint16_t)zero);
    /* fall through to block_801434B0 */

block_801434B0:
    t0 = t0 + 8;
    /* fall through to block_801434B4 */

block_801434B4:
    t1 = t1 + 8;
    a3 = a3 + 16;
    t4 = t4 + 1;
    v0 = ((int32_t)t4 < 32) ? 1 : 0;  /* 0x801434C0: 0x29820020 */
    int _bc_801434C4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t5 = t5 + 16;
    if (_bc_801434C4) {
        psx_check_interrupts_at(cpu, 0x80142F74u);
        goto block_80142F74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801434CCu);
        goto block_801434CC;  /* not taken */
    }

block_801434CC:
    uint32_t _jt_801434CC = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801434D0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801434CC);
    cpu->pc = _jt_801434CC; return;  /* CPS: jr $ra */
}

/* ======== cmd_input_parse_alias (alias body guest 0x801434D4) ======== */

/* Overlapping-alias body for br2eu_func_801434D4 (cmd_input_parse @0x801434D4): 12 input-command entries (0x801435F0..0x80143978) plus 67 CPS blocks. Faithful CPS transliteration of generated/SLES_017.22_full_11.c br2eu_psx_alias_body_801434D4.
 * cpu->pc dispatch: continuation PCs hit the _cont switch; alias entry
 * addrs fall through to the entry switch (entry = cpu->pc). */
void cmd_input_parse_alias(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t entry;
  if (!cpu) return;
  entry = cpu->pc;
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
