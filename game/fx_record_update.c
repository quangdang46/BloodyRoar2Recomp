/* Clean CPS port of the FX record update cluster at 0x8013AE1C.
 * The canonical entry owns the 48-byte guest frame; jump-table variants enter
 * their original blocks. Guest registers and stack args survive every yield. */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* fx_record_update @0x8013AE1C. CPS control flow is retained so every
 * generated resume PC and guest-visible register survives an interrupt yield. */
void fx_record_update(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8013AE1Cu: goto block_8013AE1C;
            case 0x8013AE54u: goto block_8013AE54;
            case 0x8013AED8u: goto block_8013AED8;
            case 0x8013AF0Cu: goto block_8013AF0C;
            case 0x8013AF20u: goto block_8013AF20;
            case 0x8013AF58u: goto block_8013AF58;
            case 0x8013AFB4u: goto block_8013AFB4;
            case 0x8013B034u: goto block_8013B034;
            case 0x8013B074u: goto block_8013B074;
            case 0x8013AE44u: goto block_8013AE44;
            case 0x8013AE4Cu: goto block_8013AE4C;
            case 0x8013AE6Cu: goto block_8013AE6C;
            case 0x8013AE88u: goto block_8013AE88;
            case 0x8013AE9Cu: goto block_8013AE9C;
            case 0x8013AEA4u: goto block_8013AEA4;
            case 0x8013AEA8u: goto block_8013AEA8;
            case 0x8013AED0u: goto block_8013AED0;
            case 0x8013AEFCu: goto block_8013AEFC;
            case 0x8013AF04u: goto block_8013AF04;
            case 0x8013AF18u: goto block_8013AF18;
            case 0x8013AF4Cu: goto block_8013AF4C;
            case 0x8013AF80u: goto block_8013AF80;
            case 0x8013AFD0u: goto block_8013AFD0;
            case 0x8013B050u: goto block_8013B050;
            case 0x8013B060u: goto block_8013B060;
            case 0x8013B06Cu: goto block_8013B06C;
            default: break;
        }
    }
    /* Address: 0x8013AE1C, Size: 620 bytes, Blocks: 26 */

block_8013AE1C:
    sp -= 48;  /* 0x8013AE1C: 0x27BDFFD0 */
    cpu->write_word(sp + 36, s1);  /* 0x8013AE20: 0xAFB10024 */
    s1 = a0;  /* 0x8013AE24: 0x00808821 */
    cpu->write_word(sp + 40, ra);  /* 0x8013AE28: 0xAFBF0028 */
    cpu->write_word(sp + 32, s0);  /* 0x8013AE2C: 0xAFB00020 */
    v0 = cpu->read_word(s1 + 20);  /* 0x8013AE30: 0x8E220014 */
    /* nop */  /* 0x8013AE34: 0x00000000 */
    v0 &= 0x0080;  /* 0x8013AE38: 0x30420080 */
    int cond_8013AE3C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AE40: 0x00000000 */
    if (cond_8013AE3C) {
        psx_check_interrupts_at(cpu, 0x8013AE54u);
        goto block_8013AE54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013AE44u);
    }

block_8013AE44:
    ;
    int cond_8013AE44 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AE48: 0x00000000 */
    if (cond_8013AE44) {
        psx_check_interrupts_at(cpu, 0x8013AE54u);
        goto block_8013AE54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013AE4Cu);
    }

block_8013AE4C:
    ra = 0x8013AE54u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AE50: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013ADB4u);
    cpu->pc = 0x8013ADB4u; return;  /* CPS jal -> fx_record_init (0x8013ADB4) */

block_8013AE54:
    v0 = cpu->read_word(s1 + 20);  /* 0x8013AE54: 0x8E220014 */
    /* nop */  /* 0x8013AE58: 0x00000000 */
    v1 = v0 & 0x007F;  /* 0x8013AE5C: 0x3043007F */
    v0 = v1 < (uint32_t)32;  /* 0x8013AE60: 0x2C620020 */
    int cond_8013AE64 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x8013AE68: 0x3C028012 */
    if (cond_8013AE64) {
        psx_check_interrupts_at(cpu, 0x8013B074u);
        goto block_8013B074;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013AE6Cu);
    }

block_8013AE6C:
    v0 += 344;  /* 0x8013AE6C: 0x24420158 */
    v1 <<= 2;  /* 0x8013AE70: 0x00031880 */
    v1 += v0;  /* 0x8013AE74: 0x00621821 */
    v0 = cpu->read_word(v1);  /* 0x8013AE78: 0x8C620000 */
    /* nop */  /* 0x8013AE7C: 0x00000000 */
    uint32_t jt_8013AE80 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AE84: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_8013AE80);
    cpu->pc = jt_8013AE80; return;  /* CPS: jr v0 */

block_8013AE88:
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8013AE88: 0x3C02801C */
    v1 = cpu->read_word(v0 + 17784);  /* 0x8013AE8C: 0x8C434578 */
    t0 = cpu->read_word(s1 + 12);  /* 0x8013AE90: 0x8E28000C */
    int cond_8013AE94 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3;  /* 0x8013AE98: 0x24020003 */
    if (cond_8013AE94) {
        psx_check_interrupts_at(cpu, 0x8013AEA4u);
        goto block_8013AEA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013AE9Cu);
    }

block_8013AE9C:
    ;
    int cond_8013AE9C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x00280000u;  /* 0x00280000 */  /* 0x8013AEA0: 0x3C030028 */
    if (cond_8013AE9C) {
        psx_check_interrupts_at(cpu, 0x8013AEA8u);
        goto block_8013AEA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013AEA4u);
    }

block_8013AEA4:
    v1 = 0x00200000u;  /* 0x00200000 */  /* 0x8013AEA4: 0x3C030020 */
    /* fall through to block_8013AEA8 */

block_8013AEA8:
    a0 = s1;  /* 0x8013AEA8: 0x02202021 */
    v0 = 0x40000000u;  /* 0x40000000 */  /* 0x8013AEAC: 0x3C024000 */
    a1 = cpu->read_word(a0 + 16);  /* 0x8013AEB0: 0x8C850010 */
    a2 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013AEB4: 0x85060000 */
    a1 += v0;  /* 0x8013AEB8: 0x00A22821 */
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 4);  /* 0x8013AEBC: 0x85020004 */
    a3 = (int32_t)(int16_t)cpu->read_half(t0 + 2);  /* 0x8013AEC0: 0x85070002 */
    v0 = v1 | v0;  /* 0x8013AEC4: 0x00621025 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, v0);  /* 0x8013AECC: 0xAFA20010 */
    psx_check_interrupts_at(cpu, 0x8013B06Cu);
    goto block_8013B06C;  /* j */

block_8013AED0:
    ra = 0x8013AED8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AED4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E4D0u);
    cpu->pc = 0x8018E4D0u; return;  /* CPS jal -> rand_lcg (0x8018E4D0) */

block_8013AED8:
    v1 = cpu->read_half(s1);  /* 0x8013AED8: 0x96230000 */
    v0 &= 0x00FF;  /* 0x8013AEDC: 0x304200FF */
    v1 -= 128;  /* 0x8013AEE0: 0x2463FF80 */
    v1 += v0;  /* 0x8013AEE4: 0x00621821 */
    cpu->write_half(sp + 24, (uint16_t)v1);  /* 0x8013AEE8: 0xA7A30018 */
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 2);  /* 0x8013AEEC: 0x86220002 */
    v1 = cpu->read_half(s1 + 2);  /* 0x8013AEF0: 0x96230002 */
    int cond_8013AEF4 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AEF8: 0x00000000 */
    if (cond_8013AEF4) {
        psx_check_interrupts_at(cpu, 0x8013AF04u);
        goto block_8013AF04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013AEFCu);
    }

block_8013AEFC:
    /* delay slot (always executes) */
    cpu->write_half(sp + 26, (uint16_t)v1);  /* 0x8013AF00: 0xA7A3001A */
    psx_check_interrupts_at(cpu, 0x8013AF18u);
    goto block_8013AF18;  /* j */

block_8013AF04:
    ra = 0x8013AF0Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AF08: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E4D0u);
    cpu->pc = 0x8018E4D0u; return;  /* CPS jal -> rand_lcg (0x8018E4D0) */

block_8013AF0C:
    v0 &= 0x007F;  /* 0x8013AF0C: 0x3042007F */
    v0 = zero - v0;  /* 0x8013AF10: 0x00021023 */
    cpu->write_half(sp + 26, (uint16_t)v0);  /* 0x8013AF14: 0xA7A2001A */
    /* fall through to block_8013AF18 */

block_8013AF18:
    ra = 0x8013AF20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AF1C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E4D0u);
    cpu->pc = 0x8018E4D0u; return;  /* CPS jal -> rand_lcg (0x8018E4D0) */

block_8013AF20:
    a0 = sp + 24;  /* 0x8013AF20: 0x27A40018 */
    a2 = zero;  /* 0x8013AF24: 0x00003021 */
    v0 &= 0x00FF;  /* 0x8013AF28: 0x304200FF */
    v1 = cpu->read_half(s1 + 4);  /* 0x8013AF2C: 0x96230004 */
    /* nop */  /* 0x8013AF30: 0x00000000 */
    v1 -= 128;  /* 0x8013AF34: 0x2463FF80 */
    v1 += v0;  /* 0x8013AF38: 0x00621821 */
    cpu->write_half(sp + 28, (uint16_t)v1);  /* 0x8013AF3C: 0xA7A3001C */
    v0 = cpu->read_word(s1 + 16);  /* 0x8013AF40: 0x8E220010 */
    /* delay slot (always executes) */
    a3 = a2;  /* 0x8013AF48: 0x00C03821 */
    psx_check_interrupts_at(cpu, 0x8013B060u);
    goto block_8013B060;  /* j */

block_8013AF4C:
    s0 = cpu->read_word(s1 + 12);  /* 0x8013AF4C: 0x8E30000C */
    ra = 0x8013AF58u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013AF54: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E4D0u);
    cpu->pc = 0x8018E4D0u; return;  /* CPS jal -> rand_lcg (0x8018E4D0) */

block_8013AF58:
    a0 = s1;  /* 0x8013AF58: 0x02202021 */
    v0 &= 0x0003;  /* 0x8013AF5C: 0x30420003 */
    v0 += 10;  /* 0x8013AF60: 0x2442000A */
    a1 = 0x40000000u;  /* 0x40000000 */  /* 0x8013AF64: 0x3C054000 */
    v1 = cpu->read_word(a0 + 16);  /* 0x8013AF68: 0x8C830010 */
    a2 = (int32_t)(int16_t)cpu->read_half(s0);  /* 0x8013AF6C: 0x86060000 */
    a3 = (int32_t)(int16_t)cpu->read_half(s0 + 2);  /* 0x8013AF70: 0x86070002 */
    a1 = v0 | a1;  /* 0x8013AF74: 0x00452825 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, v1);  /* 0x8013AF7C: 0xAFA30010 */
    psx_check_interrupts_at(cpu, 0x8013B06Cu);
    goto block_8013B06C;  /* j */

block_8013AF80:
    a0 = s1;  /* 0x8013AF80: 0x02202021 */
    a1 = 0x40000000u;  /* 0x40000000 */  /* 0x8013AF84: 0x3C054000 */
    s0 = cpu->read_word(a0 + 12);  /* 0x8013AF88: 0x8C90000C */
    a1 |= 0x000E;  /* 0x8013AF8C: 0x34A5000E */
    a2 = (int32_t)(int16_t)cpu->read_half(s0);  /* 0x8013AF90: 0x86060000 */
    v0 = cpu->read_half(s0 + 4);  /* 0x8013AF94: 0x96020004 */
    a3 = (int32_t)(int16_t)cpu->read_half(s0 + 2);  /* 0x8013AF98: 0x86070002 */
    v0 += 888;  /* 0x8013AF9C: 0x24420378 */
    v0 &= 0x0BFF;  /* 0x8013AFA0: 0x30420BFF */
    cpu->write_half(s0 + 4, (uint16_t)v0);  /* 0x8013AFA4: 0xA6020004 */
    v0 = 4096;  /* 0x8013AFA8: 0x24021000 */
    ra = 0x8013AFB4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, v0);  /* 0x8013AFB0: 0xAFA20010 */
    psx_check_interrupts_at(cpu, 0x80139FD0u);
    cpu->pc = 0x80139FD0u; return;  /* CPS jal -> impact_fx_spawn (0x80139FD0) */

block_8013AFB4:
    v0 = cpu->read_half(s0);  /* 0x8013AFB4: 0x96020000 */
    v1 = cpu->read_half(s0 + 2);  /* 0x8013AFB8: 0x96030002 */
    v0 += 682;  /* 0x8013AFBC: 0x244202AA */
    v1 += 1365;  /* 0x8013AFC0: 0x24630555 */
    cpu->write_half(s0, (uint16_t)v0);  /* 0x8013AFC4: 0xA6020000 */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 2, (uint16_t)v1);  /* 0x8013AFCC: 0xA6030002 */
    psx_check_interrupts_at(cpu, 0x8013B074u);
    goto block_8013B074;  /* j */

block_8013AFD0:
    v1 = cpu->read_word(s1 + 12);  /* 0x8013AFD0: 0x8E23000C */
    /* nop */  /* 0x8013AFD4: 0x00000000 */
    v0 = cpu->read_half(v1 + 8);  /* 0x8013AFD8: 0x94620008 */
    s0 = cpu->read_half(v1 + 10);  /* 0x8013AFDC: 0x9470000A */
    a0 = cpu->read_word(v1);  /* 0x8013AFE0: 0x8C640000 */
    v0++;  /* 0x8013AFE4: 0x24420001 */
    s0 += 128;  /* 0x8013AFE8: 0x26100080 */
    cpu->write_half(v1 + 10, (uint16_t)s0);  /* 0x8013AFEC: 0xA470000A */
    s0 <<= 16;  /* 0x8013AFF0: 0x00108400 */
    cpu->write_half(v1 + 8, (uint16_t)v0);  /* 0x8013AFF4: 0xA4620008 */
    v0 = cpu->read_word(v1 + 4);  /* 0x8013AFF8: 0x8C620004 */
    a1 = cpu->read_word(a0 + 32);  /* 0x8013AFFC: 0x8C850020 */
    v0 = cpu->read_word(v0 + 32);  /* 0x8013B000: 0x8C420020 */
    a0 = cpu->read_word(a1 + 292);  /* 0x8013B004: 0x8CA40124 */
    a1 = cpu->read_word(a1 + 300);  /* 0x8013B008: 0x8CA5012C */
    a2 = cpu->read_word(v0 + 292);  /* 0x8013B00C: 0x8C460124 */
    v0 = cpu->read_word(v0 + 300);  /* 0x8013B010: 0x8C42012C */
    s0 = (int32_t)s0 >> 16;  /* 0x8013B014: 0x00108403 */
    a1 = v0 - a1;  /* 0x8013B018: 0x00452823 */
    v0 = cpu->read_half(v1 + 8);  /* 0x8013B01C: 0x94620008 */
    a0 = a2 - a0;  /* 0x8013B020: 0x00C42023 */
    v0 &= 0x0003;  /* 0x8013B024: 0x30420003 */
    v0 <<= 10;  /* 0x8013B028: 0x00021280 */
    ra = 0x8013B034u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 += v0;  /* 0x8013B030: 0x02028021 */
    psx_check_interrupts_at(cpu, 0x80142B3Cu);
    cpu->pc = 0x80142B3Cu; return;  /* CPS jal -> angle_from_xy (0x80142B3C) */

block_8013B034:
    a0 = s1;  /* 0x8013B034: 0x02202021 */
    a1 = 0x40000000u;  /* 0x40000000 */  /* 0x8013B038: 0x3C054000 */
    a1 |= 0x0012;  /* 0x8013B03C: 0x34A50012 */
    a2 = s0;  /* 0x8013B040: 0x02003021 */
    a3 = v0 + -1024;  /* 0x8013B044: 0x2447FC00 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8013B04C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8013B06Cu);
    goto block_8013B06C;  /* j */

block_8013B050:
    a0 = s1;  /* 0x8013B050: 0x02202021 */
    a2 = zero;  /* 0x8013B054: 0x00003021 */
    a3 = a2;  /* 0x8013B058: 0x00C03821 */
    v0 = cpu->read_word(a0 + 16);  /* 0x8013B05C: 0x8C820010 */
    /* fall through to block_8013B060 */

block_8013B060:
    a1 = 0x40000000u;  /* 0x40000000 */  /* 0x8013B060: 0x3C054000 */
    cpu->write_word(sp + 16, zero);  /* 0x8013B064: 0xAFA00010 */
    a1 = v0 | a1;  /* 0x8013B068: 0x00452825 */
    /* fall through to block_8013B06C */

block_8013B06C:
    ra = 0x8013B074u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013B070: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80139FD0u);
    cpu->pc = 0x80139FD0u; return;  /* CPS jal -> impact_fx_spawn (0x80139FD0) */

block_8013B074:
    ra = cpu->read_word(sp + 40);  /* 0x8013B074: 0x8FBF0028 */
    s1 = cpu->read_word(sp + 36);  /* 0x8013B078: 0x8FB10024 */
    s0 = cpu->read_word(sp + 32);  /* 0x8013B07C: 0x8FB00020 */
    uint32_t jt_8013B080 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 48;  /* 0x8013B084: 0x27BD0030 */
    psx_check_interrupts_at(cpu, jt_8013B080);
    cpu->pc = jt_8013B080; return;  /* CPS: jr $ra */
}