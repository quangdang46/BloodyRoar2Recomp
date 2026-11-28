/* task_packet_transform.c — clean CPS port of guest 0x8013DBD8.
 *
 * This is one guest coroutine with 25 registered block-entry PCs. Its
 * continuation selector and all live state stay in CPUState/guest RAM so
 * IRQ checks and nested guest calls can resume at the original block.
 * The named calls at 0x801A482C, 0x80194840, 0x801954D0, 0x80142B3C,
 * 0x8018D298, and 0x8018D29C route through clean dispatch; the wrappers
 * marshal pointer-typed GTE arguments from guest addresses to host pointers.
 */
#include <stdint.h>
#include <stdlib.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

static inline void mips_div(CPUState *cpu, int32_t dividend, int32_t divisor)
{
  if (divisor == 0) {
    cpu->lo = dividend < 0 ? 1u : 0xffffffffu;
    cpu->hi = (uint32_t)dividend;
  } else if (dividend == INT32_MIN && divisor == -1) {
    cpu->lo = 0x80000000u;
    cpu->hi = 0;
  } else {
    cpu->lo = (uint32_t)(dividend / divisor);
    cpu->hi = (uint32_t)(dividend % divisor);
  }
}

static inline uint32_t br2_lwl(CPUState *cpu, uint32_t addr, uint32_t rt_value)
{
  uint32_t word = cpu->read_word(addr & ~3u);
  switch (addr & 3u) {
    case 0: return (rt_value & 0x00ffffffu) | (word << 24);
    case 1: return (rt_value & 0x0000ffffu) | (word << 16);
    case 2: return (rt_value & 0x000000ffu) | (word << 8);
    default: return word;
  }
}

static inline uint32_t br2_lwr(CPUState *cpu, uint32_t addr, uint32_t rt_value)
{
  uint32_t word = cpu->read_word(addr & ~3u);
  switch (addr & 3u) {
    case 0: return word;
    case 1: return (rt_value & 0xff000000u) | (word >> 8);
    case 2: return (rt_value & 0xffff0000u) | (word >> 16);
    default: return (rt_value & 0xffffff00u) | (word >> 24);
  }
}

static inline void br2_swl(CPUState *cpu, uint32_t addr, uint32_t rt_value)
{
  uint32_t aligned = addr & ~3u;
  uint32_t word = cpu->read_word(aligned);
  switch (addr & 3u) {
    case 0: word = (word & 0xffffff00u) | (rt_value >> 24); break;
    case 1: word = (word & 0xffff0000u) | (rt_value >> 16); break;
    case 2: word = (word & 0xff000000u) | (rt_value >> 8); break;
    default: word = rt_value; break;
  }
  cpu->write_word(aligned, word);
}

static inline void br2_swr(CPUState *cpu, uint32_t addr, uint32_t rt_value)
{
  uint32_t aligned = addr & ~3u;
  uint32_t word = cpu->read_word(aligned);
  switch (addr & 3u) {
    case 0: word = rt_value; break;
    case 1: word = (word & 0x000000ffu) | (rt_value << 8); break;
    case 2: word = (word & 0x0000ffffu) | (rt_value << 16); break;
    default: word = (word & 0x00ffffffu) | (rt_value << 24); break;
  }
  cpu->write_word(aligned, word);
}

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

void task_packet_transform(void)
{
  CPUState *cpu = clean_cpu_state();
  if (cpu == NULL) abort();

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8013DCA4u: goto block_8013DCA4;
            case 0x8013DCE8u: goto block_8013DCE8;
            case 0x8013DD5Cu: goto block_8013DD5C;
            case 0x8013DDC4u: goto block_8013DDC4;
            case 0x8013DED0u: goto block_8013DED0;
            case 0x8013DEFCu: goto block_8013DEFC;
            case 0x8013DF28u: goto block_8013DF28;
            case 0x8013DF54u: goto block_8013DF54;
            case 0x8013DF8Cu: goto block_8013DF8C;
            case 0x8013DFD0u: goto block_8013DFD0;
            case 0x8013DFFCu: goto block_8013DFFC;
            case 0x8013E028u: goto block_8013E028;
            case 0x8013DC80u: goto block_8013DC80;
            case 0x8013DC9Cu: goto block_8013DC9C;
            case 0x8013DCACu: goto block_8013DCAC;
            case 0x8013DD34u: goto block_8013DD34;
            case 0x8013DD48u: goto block_8013DD48;
            case 0x8013DD78u: goto block_8013DD78;
            case 0x8013DD84u: goto block_8013DD84;
            case 0x8013DDDCu: goto block_8013DDDC;
            case 0x8013DE9Cu: goto block_8013DE9C;
            case 0x8013DEC4u: goto block_8013DEC4;
            case 0x8013E094u: goto block_8013E094;
            case 0x8013E0C0u: goto block_8013E0C0;
            default: break;
        }
    }
    /* Address: 0x8013DBD8, Size: 1304 bytes, Blocks: 25 */

block_8013DBD8:
    sp -= 160;  /* 0x8013DBD8: 0x27BDFF60 */
    cpu->write_word(sp + 132, s3);  /* 0x8013DBDC: 0xAFB30084 */
    s3 = a0;  /* 0x8013DBE0: 0x00809821 */
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013DBE4: 0x3C041F80 */
    cpu->write_word(sp + 156, ra);  /* 0x8013DBE8: 0xAFBF009C */
    cpu->write_word(sp + 152, fp);  /* 0x8013DBEC: 0xAFBE0098 */
    cpu->write_word(sp + 148, s7);  /* 0x8013DBF0: 0xAFB70094 */
    cpu->write_word(sp + 144, s6);  /* 0x8013DBF4: 0xAFB60090 */
    cpu->write_word(sp + 140, s5);  /* 0x8013DBF8: 0xAFB5008C */
    cpu->write_word(sp + 136, s4);  /* 0x8013DBFC: 0xAFB40088 */
    cpu->write_word(sp + 128, s2);  /* 0x8013DC00: 0xAFB20080 */
    cpu->write_word(sp + 124, s1);  /* 0x8013DC04: 0xAFB1007C */
    cpu->write_word(sp + 120, s0);  /* 0x8013DC08: 0xAFB00078 */
    v1 = cpu->read_word(a0 + 12);  /* 0x8013DC0C: 0x8C83000C */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x8013DC10: 0x3C028012 */
    t5 = v0 + 472;  /* 0x8013DC14: 0x244D01D8 */
    t2 = br2_lwl(cpu, t5 + 3, t2);  /* 0x8013DC18: 0x89AA0003 */
    t2 = br2_lwr(cpu, t5, t2);  /* 0x8013DC1C: 0x99AA0000 */
    t3 = br2_lwl(cpu, t5 + 7, t3);  /* 0x8013DC20: 0x89AB0007 */
    t3 = br2_lwr(cpu, t5 + 4, t3);  /* 0x8013DC24: 0x99AB0004 */
    t4 = br2_lwl(cpu, t5 + 11, t4);  /* 0x8013DC28: 0x89AC000B */
    t4 = br2_lwr(cpu, t5 + 8, t4);  /* 0x8013DC2C: 0x99AC0008 */
    br2_swl(cpu, sp + 83, t2);  /* 0x8013DC30: 0xABAA0053 */
    br2_swr(cpu, sp + 80, t2);  /* 0x8013DC34: 0xBBAA0050 */
    br2_swl(cpu, sp + 87, t3);  /* 0x8013DC38: 0xABAB0057 */
    br2_swr(cpu, sp + 84, t3);  /* 0x8013DC3C: 0xBBAB0054 */
    br2_swl(cpu, sp + 91, t4);  /* 0x8013DC40: 0xABAC005B */
    br2_swr(cpu, sp + 88, t4);  /* 0x8013DC44: 0xBBAC0058 */
    t2 = br2_lwl(cpu, t5 + 15, t2);  /* 0x8013DC48: 0x89AA000F */
    t2 = br2_lwr(cpu, t5 + 12, t2);  /* 0x8013DC4C: 0x99AA000C */
    /* nop */  /* 0x8013DC50: 0x00000000 */
    br2_swl(cpu, sp + 95, t2);  /* 0x8013DC54: 0xABAA005F */
    br2_swr(cpu, sp + 92, t2);  /* 0x8013DC58: 0xBBAA005C */
    v0 = v1 << 2;  /* 0x8013DC5C: 0x00031080 */
    v0 += v1;  /* 0x8013DC60: 0x00431021 */
    v0 <<= 4;  /* 0x8013DC64: 0x00021100 */
    v0 = s3 + v0;  /* 0x8013DC68: 0x02621021 */
    cpu->write_word(sp + 100, v0);  /* 0x8013DC6C: 0xAFA20064 */
    v0 = cpu->read_half(a0 + 130);  /* 0x8013DC70: 0x94820082 */
    /* nop */  /* 0x8013DC74: 0x00000000 */
    int cond_8013DC78 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 80;  /* 0x8013DC7C: 0x27A40050 */
    if (cond_8013DC78) {
        psx_check_interrupts_at(cpu, 0x8013DD34u);
        goto block_8013DD34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013DC80u);
    }

block_8013DC80:
    v0 = cpu->read_half(s3 + 194);  /* 0x8013DC80: 0x966200C2 */
    /* nop */  /* 0x8013DC84: 0x00000000 */
    v0--;  /* 0x8013DC88: 0x2442FFFF */
    cpu->write_half(s3 + 194, (uint16_t)v0);  /* 0x8013DC8C: 0xA66200C2 */
    v0 <<= 16;  /* 0x8013DC90: 0x00021400 */
    int cond_8013DC94 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 88;  /* 0x8013DC98: 0x27A50058 */
    if (cond_8013DC94) {
        psx_check_interrupts_at(cpu, 0x8013DCACu);
        goto block_8013DCAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013DC9Cu);
    }

block_8013DC9C:
    ra = 0x8013DCA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DCA0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A482Cu);
    cpu->pc = 0x801A482Cu; return;  /* CPS jal -> task_slot_step (0x801A482C) */

block_8013DCA4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DCA8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013E0C0u);
    goto block_8013E0C0;  /* j */

block_8013DCAC:
    a3 = (int32_t)(int16_t)cpu->read_half(s3 + 194);  /* 0x8013DCAC: 0x866700C2 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 192);  /* 0x8013DCB0: 0x866200C0 */
    a3 <<= 12;  /* 0x8013DCB4: 0x00073B00 */
    mips_div(cpu, (int32_t)a3, (int32_t)v0);
    psx_muldiv_set(cpu, 37u);  /* 0x8013DCB8: 0x00E2001A */
    a3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DCBC: 0x00003812 */
    a2 = 1;  /* 0x8013DCC0: 0x24060001 */
    t0 = cpu->read_half(s3 + 184);  /* 0x8013DCC4: 0x966800B8 */
    v1 = cpu->read_half(s3 + 180);  /* 0x8013DCC8: 0x966300B4 */
    t1 = cpu->read_half(s3 + 188);  /* 0x8013DCCC: 0x966900BC */
    v0 = cpu->read_half(s3 + 176);  /* 0x8013DCD0: 0x966200B0 */
    v1 += t1;  /* 0x8013DCD4: 0x00691821 */
    cpu->write_half(s3 + 180, (uint16_t)v1);  /* 0x8013DCD8: 0xA66300B4 */
    v0 += t0;  /* 0x8013DCDC: 0x00481021 */
    ra = 0x8013DCE8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s3 + 176, (uint16_t)v0);  /* 0x8013DCE4: 0xA66200B0 */
    psx_check_interrupts_at(cpu, 0x80194840u);
    cpu->pc = 0x80194840u; return;  /* CPS jal -> gte_xform_accum (0x80194840) */

block_8013DCE8:
    v0 = cpu->read_byte(sp + 80);  /* 0x8013DCE8: 0x93A20050 */
    t2 = cpu->read_word(sp + 100);  /* 0x8013DCEC: 0x8FAA0064 */
    /* nop */  /* 0x8013DCF0: 0x00000000 */
    cpu->write_byte(t2 + 4, (uint8_t)v0);  /* 0x8013DCF4: 0xA1420004 */
    v0 = cpu->read_byte(sp + 82);  /* 0x8013DCF8: 0x93A20052 */
    /* nop */  /* 0x8013DCFC: 0x00000000 */
    cpu->write_byte(t2 + 5, (uint8_t)v0);  /* 0x8013DD00: 0xA1420005 */
    v0 = cpu->read_byte(sp + 84);  /* 0x8013DD04: 0x93A20054 */
    /* nop */  /* 0x8013DD08: 0x00000000 */
    cpu->write_byte(t2 + 6, (uint8_t)v0);  /* 0x8013DD0C: 0xA1420006 */
    v0 = cpu->read_byte(sp + 80);  /* 0x8013DD10: 0x93A20050 */
    /* nop */  /* 0x8013DD14: 0x00000000 */
    cpu->write_byte(t2 + 44, (uint8_t)v0);  /* 0x8013DD18: 0xA142002C */
    v0 = cpu->read_byte(sp + 82);  /* 0x8013DD1C: 0x93A20052 */
    /* nop */  /* 0x8013DD20: 0x00000000 */
    cpu->write_byte(t2 + 45, (uint8_t)v0);  /* 0x8013DD24: 0xA142002D */
    v0 = cpu->read_byte(sp + 84);  /* 0x8013DD28: 0x93A20054 */
    /* nop */  /* 0x8013DD2C: 0x00000000 */
    cpu->write_byte(t2 + 46, (uint8_t)v0);  /* 0x8013DD30: 0xA142002E */
    /* fall through to block_8013DD34 */

block_8013DD34:
    s4 = zero;  /* 0x8013DD34: 0x0000A021 */
    s5 = sp + 96;  /* 0x8013DD38: 0x27B50060 */
    s2 = sp + 48;  /* 0x8013DD3C: 0x27B20030 */
    s1 = sp + 64;  /* 0x8013DD40: 0x27B10040 */
    s0 = 160;  /* 0x8013DD44: 0x241000A0 */
    /* fall through to block_8013DD48 */

block_8013DD48:
    a0 = s3 + s0;  /* 0x8013DD48: 0x02702021 */
    a1 = s1;  /* 0x8013DD4C: 0x02202821 */
    a2 = s5;  /* 0x8013DD50: 0x02A03021 */
    ra = 0x8013DD5Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s5;  /* 0x8013DD58: 0x02A03821 */
    psx_check_interrupts_at(cpu, 0x801954D0u);
    cpu->pc = 0x801954D0u; return;  /* CPS jal -> gte_rtps_vec3 (0x801954D0) */

block_8013DD5C:
    cpu->write_word(s2, v0);  /* 0x8013DD5C: 0xAE420000 */
    s2 += 4;  /* 0x8013DD60: 0x26520004 */
    s1 += 4;  /* 0x8013DD64: 0x26310004 */
    s4++;  /* 0x8013DD68: 0x26940001 */
    v0 = (int32_t)s4 < 3;  /* 0x8013DD6C: 0x2A820003 */
    int cond_8013DD70 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 += 8;  /* 0x8013DD74: 0x26100008 */
    if (cond_8013DD70) {
        psx_check_interrupts_at(cpu, 0x8013DD48u);
        goto block_8013DD48;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013DD78u);
    }

block_8013DD78:
    s4 = zero;  /* 0x8013DD78: 0x0000A021 */
    s1 = sp + 64;  /* 0x8013DD7C: 0x27B10040 */
    v0 = s4 + 1;  /* 0x8013DD80: 0x26820001 */
    /* fall through to block_8013DD84 */

block_8013DD84:
    s0 = s4 << 2;  /* 0x8013DD84: 0x00148080 */
    s4 = v0;  /* 0x8013DD88: 0x0040A021 */
    v0 = s4 << 2;  /* 0x8013DD8C: 0x00141080 */
    v0 = s1 + v0;  /* 0x8013DD90: 0x02221021 */
    a1 = s1 + s0;  /* 0x8013DD94: 0x02302821 */
    v1 = (int32_t)(int16_t)cpu->read_half(v0);  /* 0x8013DD98: 0x84430000 */
    a0 = (int32_t)(int16_t)cpu->read_half(a1);  /* 0x8013DD9C: 0x84A40000 */
    v0 = cpu->read_half(v0 + 2);  /* 0x8013DDA0: 0x94420002 */
    a1 = cpu->read_half(a1 + 2);  /* 0x8013DDA4: 0x94A50002 */
    a0 = v1 - a0;  /* 0x8013DDA8: 0x00642023 */
    v0 = (int32_t)(int16_t)v0;  /* 0x8013DDAC: 0x00021400 */  /* 0x8013DDB0: 0x00021403 */
    a1 = (int32_t)(int16_t)a1;  /* 0x8013DDB4: 0x00052C00 */  /* 0x8013DDB8: 0x00052C03 */
    ra = 0x8013DDC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = v0 - a1;  /* 0x8013DDC0: 0x00452823 */
    psx_check_interrupts_at(cpu, 0x80142B3Cu);
    cpu->pc = 0x80142B3Cu; return;  /* CPS jal -> angle_from_xy (0x80142B3C) */

block_8013DDC4:
    s2 = sp + 16;  /* 0x8013DDC4: 0x27B20010 */
    s0 = s2 + s0;  /* 0x8013DDC8: 0x02508021 */
    cpu->write_word(s0, v0);  /* 0x8013DDCC: 0xAE020000 */
    v0 = (int32_t)s4 < 2;  /* 0x8013DDD0: 0x2A820002 */
    int cond_8013DDD4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s4 + 1;  /* 0x8013DDD8: 0x26820001 */
    if (cond_8013DDD4) {
        psx_check_interrupts_at(cpu, 0x8013DD84u);
        goto block_8013DD84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013DDDCu);
    }

block_8013DDDC:
    a1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013DDDC: 0x3C051F80 */
    a1 = (int32_t)(int16_t)cpu->read_half(a1 + 36);  /* 0x8013DDE0: 0x84A50024 */
    v0 = cpu->read_word(sp + 48);  /* 0x8013DDE4: 0x8FA20030 */
    v1 = a1 << 1;  /* 0x8013DDE8: 0x00051840 */
    v1 += a1;  /* 0x8013DDEC: 0x00651821 */
    v1 <<= 2;  /* 0x8013DDF0: 0x00031880 */
    v1 -= a1;  /* 0x8013DDF4: 0x00651823 */
    v1 <<= 3;  /* 0x8013DDF8: 0x000318C0 */
    v1 += a1;  /* 0x8013DDFC: 0x00651821 */
    v1 <<= 2;  /* 0x8013DE00: 0x00031880 */
    v0 <<= 2;  /* 0x8013DE04: 0x00021080 */
    mips_div(cpu, (int32_t)v1, (int32_t)v0);
    psx_muldiv_set(cpu, 37u);  /* 0x8013DE08: 0x0062001A */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DE0C: 0x00001812 */
    a2 = a1 << 3;  /* 0x8013DE10: 0x000530C0 */
    a2 -= a1;  /* 0x8013DE14: 0x00C53023 */
    a2 <<= 3;  /* 0x8013DE18: 0x000630C0 */
    a2 += a1;  /* 0x8013DE1C: 0x00C53021 */
    v0 = cpu->read_word(sp + 52);  /* 0x8013DE20: 0x8FA20034 */
    a2 <<= 2;  /* 0x8013DE24: 0x00063080 */
    v0 <<= 2;  /* 0x8013DE28: 0x00021080 */
    mips_div(cpu, (int32_t)a2, (int32_t)v0);
    psx_muldiv_set(cpu, 37u);  /* 0x8013DE2C: 0x00C2001A */
    a2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DE30: 0x00003012 */
    a0 = a1 << 4;  /* 0x8013DE34: 0x00052100 */
    a0 -= a1;  /* 0x8013DE38: 0x00852023 */
    a0 <<= 2;  /* 0x8013DE3C: 0x00042080 */
    a0 -= a1;  /* 0x8013DE40: 0x00852023 */
    v0 = cpu->read_word(sp + 56);  /* 0x8013DE44: 0x8FA20038 */
    a0 <<= 1;  /* 0x8013DE48: 0x00042040 */
    v0 <<= 2;  /* 0x8013DE4C: 0x00021080 */
    mips_div(cpu, (int32_t)a0, (int32_t)v0);
    psx_muldiv_set(cpu, 37u);  /* 0x8013DE50: 0x0082001A */
    a0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DE54: 0x00002012 */
    s4 = zero;  /* 0x8013DE58: 0x0000A021 */
    fp = 4;  /* 0x8013DE5C: 0x241E0004 */
    t2 = sp + 32;  /* 0x8013DE60: 0x27AA0020 */
    s7 = t2;  /* 0x8013DE64: 0x0140B821 */
    cpu->write_word(sp + 116, s2);  /* 0x8013DE68: 0xAFB20074 */
    s6 = s2;  /* 0x8013DE6C: 0x0240B021 */
    t3 = sp + 64;  /* 0x8013DE70: 0x27AB0040 */
    t4 = cpu->read_word(sp + 100);  /* 0x8013DE74: 0x8FAC0064 */
    s5 = t3;  /* 0x8013DE78: 0x0160A821 */
    cpu->write_word(sp + 112, t2);  /* 0x8013DE7C: 0xAFAA0070 */
    cpu->write_word(sp + 108, t3);  /* 0x8013DE80: 0xAFAB006C */
    v0 = cpu->read_word(sp + 20);  /* 0x8013DE84: 0x8FA20014 */
    s3 = t4 + 34;  /* 0x8013DE88: 0x25930022 */
    cpu->write_word(sp + 24, v0);  /* 0x8013DE8C: 0xAFA20018 */
    cpu->write_word(sp + 32, v1);  /* 0x8013DE90: 0xAFA30020 */
    cpu->write_word(sp + 36, a2);  /* 0x8013DE94: 0xAFA60024 */
    cpu->write_word(sp + 40, a0);  /* 0x8013DE98: 0xAFA40028 */
    /* fall through to block_8013DE9C */

block_8013DE9C:
    v1 = s4 << 2;  /* 0x8013DE9C: 0x00141880 */
    v0 = sp + 48;  /* 0x8013DEA0: 0x27A20030 */
    v0 += v1;  /* 0x8013DEA4: 0x00431021 */
    cpu->write_word(sp + 104, v0);  /* 0x8013DEA8: 0xAFA20068 */
    v0 = cpu->read_word(v0);  /* 0x8013DEAC: 0x8C420000 */
    /* nop */  /* 0x8013DEB0: 0x00000000 */
    v0 -= 4;  /* 0x8013DEB4: 0x2442FFFC */
    v0 = v0 < (uint32_t)2059;  /* 0x8013DEB8: 0x2C42080B */
    int cond_8013DEBC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DEC0: 0x00000000 */
    if (cond_8013DEBC) {
        psx_check_interrupts_at(cpu, 0x8013E094u);
        goto block_8013E094;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013DEC4u);
    }

block_8013DEC4:
    a0 = cpu->read_word(s6);  /* 0x8013DEC4: 0x8EC40000 */
    ra = 0x8013DED0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DECC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> lut_cos_q12 (0x8018D298) */

block_8013DED0:
    v1 = cpu->read_word(s7);  /* 0x8013DED0: 0x8EE30000 */
    /* nop */  /* 0x8013DED4: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013DED8: 0x00430018 */
    v0 = cpu->read_half(s5);  /* 0x8013DEDC: 0x96A20000 */
    t2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DEE0: 0x00005012 */
    v1 = (int32_t)t2 >> 12;  /* 0x8013DEE4: 0x000A1B03 */
    v0 += v1;  /* 0x8013DEE8: 0x00431021 */
    cpu->write_half(s3 + -26, (uint16_t)v0);  /* 0x8013DEEC: 0xA662FFE6 */
    a0 = cpu->read_word(s6);  /* 0x8013DEF0: 0x8EC40000 */
    ra = 0x8013DEFCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DEF8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> lut_sin_q12 (0x8018D29C) */

block_8013DEFC:
    v1 = cpu->read_word(s7);  /* 0x8013DEFC: 0x8EE30000 */
    /* nop */  /* 0x8013DF00: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013DF04: 0x00430018 */
    v0 = cpu->read_half(s5 + 2);  /* 0x8013DF08: 0x96A20002 */
    t2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DF0C: 0x00005012 */
    v1 = (int32_t)t2 >> 12;  /* 0x8013DF10: 0x000A1B03 */
    v0 += v1;  /* 0x8013DF14: 0x00431021 */
    cpu->write_half(s3 + -24, (uint16_t)v0);  /* 0x8013DF18: 0xA662FFE8 */
    a0 = cpu->read_word(s6);  /* 0x8013DF1C: 0x8EC40000 */
    ra = 0x8013DF28u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DF24: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> lut_cos_q12 (0x8018D298) */

block_8013DF28:
    v1 = cpu->read_word(s7);  /* 0x8013DF28: 0x8EE30000 */
    /* nop */  /* 0x8013DF2C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013DF30: 0x00430018 */
    v0 = cpu->read_half(s5);  /* 0x8013DF34: 0x96A20000 */
    t2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DF38: 0x00005012 */
    v1 = (int32_t)t2 >> 12;  /* 0x8013DF3C: 0x000A1B03 */
    v0 -= v1;  /* 0x8013DF40: 0x00431023 */
    cpu->write_half(s3 + -10, (uint16_t)v0);  /* 0x8013DF44: 0xA662FFF6 */
    a0 = cpu->read_word(s6);  /* 0x8013DF48: 0x8EC40000 */
    ra = 0x8013DF54u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DF50: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> lut_sin_q12 (0x8018D29C) */

block_8013DF54:
    v1 = cpu->read_word(s7);  /* 0x8013DF54: 0x8EE30000 */
    /* nop */  /* 0x8013DF58: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013DF5C: 0x00430018 */
    v0 = cpu->read_half(s5 + 2);  /* 0x8013DF60: 0x96A20002 */
    t2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DF64: 0x00005012 */
    v1 = (int32_t)t2 >> 12;  /* 0x8013DF68: 0x000A1B03 */
    v0 -= v1;  /* 0x8013DF6C: 0x00431023 */
    cpu->write_half(s3 + -8, (uint16_t)v0);  /* 0x8013DF70: 0xA662FFF8 */
    t3 = cpu->read_word(sp + 116);  /* 0x8013DF74: 0x8FAB0074 */
    /* nop */  /* 0x8013DF78: 0x00000000 */
    s2 = t3 + fp;  /* 0x8013DF7C: 0x017E9021 */
    a0 = cpu->read_word(s2);  /* 0x8013DF80: 0x8E440000 */
    ra = 0x8013DF8Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DF88: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> lut_cos_q12 (0x8018D298) */

block_8013DF8C:
    t4 = cpu->read_word(sp + 112);  /* 0x8013DF8C: 0x8FAC0070 */
    /* nop */  /* 0x8013DF90: 0x00000000 */
    s1 = t4 + fp;  /* 0x8013DF94: 0x019E8821 */
    v1 = cpu->read_word(s1);  /* 0x8013DF98: 0x8E230000 */
    /* nop */  /* 0x8013DF9C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013DFA0: 0x00430018 */
    t2 = cpu->read_word(sp + 108);  /* 0x8013DFA4: 0x8FAA006C */
    /* nop */  /* 0x8013DFA8: 0x00000000 */
    s0 = t2 + fp;  /* 0x8013DFAC: 0x015E8021 */
    v0 = cpu->read_half(s0);  /* 0x8013DFB0: 0x96020000 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DFB4: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013DFB8: 0x000B1B03 */
    v0 += v1;  /* 0x8013DFBC: 0x00431021 */
    cpu->write_half(s3 + -18, (uint16_t)v0);  /* 0x8013DFC0: 0xA662FFEE */
    a0 = cpu->read_word(s2);  /* 0x8013DFC4: 0x8E440000 */
    ra = 0x8013DFD0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DFCC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> lut_sin_q12 (0x8018D29C) */

block_8013DFD0:
    v1 = cpu->read_word(s1);  /* 0x8013DFD0: 0x8E230000 */
    /* nop */  /* 0x8013DFD4: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013DFD8: 0x00430018 */
    v0 = cpu->read_half(s0 + 2);  /* 0x8013DFDC: 0x96020002 */
    t2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013DFE0: 0x00005012 */
    v1 = (int32_t)t2 >> 12;  /* 0x8013DFE4: 0x000A1B03 */
    v0 += v1;  /* 0x8013DFE8: 0x00431021 */
    cpu->write_half(s3 + -16, (uint16_t)v0);  /* 0x8013DFEC: 0xA662FFF0 */
    a0 = cpu->read_word(s2);  /* 0x8013DFF0: 0x8E440000 */
    ra = 0x8013DFFCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013DFF8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> lut_cos_q12 (0x8018D298) */

block_8013DFFC:
    v1 = cpu->read_word(s1);  /* 0x8013DFFC: 0x8E230000 */
    /* nop */  /* 0x8013E000: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013E004: 0x00430018 */
    v0 = cpu->read_half(s0);  /* 0x8013E008: 0x96020000 */
    t2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013E00C: 0x00005012 */
    v1 = (int32_t)t2 >> 12;  /* 0x8013E010: 0x000A1B03 */
    v0 -= v1;  /* 0x8013E014: 0x00431023 */
    cpu->write_half(s3 + -2, (uint16_t)v0);  /* 0x8013E018: 0xA662FFFE */
    a0 = cpu->read_word(s2);  /* 0x8013E01C: 0x8E440000 */
    ra = 0x8013E028u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013E024: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> lut_sin_q12 (0x8018D29C) */

block_8013E028:
    v1 = cpu->read_word(s1);  /* 0x8013E028: 0x8E230000 */
    /* nop */  /* 0x8013E02C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013E030: 0x00430018 */
    v0 = cpu->read_half(s0 + 2);  /* 0x8013E034: 0x96020002 */
    t2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013E038: 0x00005012 */
    v1 = (int32_t)t2 >> 12;  /* 0x8013E03C: 0x000A1B03 */
    v0 -= v1;  /* 0x8013E040: 0x00431023 */
    cpu->write_half(s3, (uint16_t)v0);  /* 0x8013E044: 0xA6620000 */
    t3 = cpu->read_word(sp + 104);  /* 0x8013E048: 0x8FAB0068 */
    v1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013E04C: 0x3C031F80 */
    v1 = cpu->read_word(v1 + 8);  /* 0x8013E050: 0x8C630008 */
    v0 = cpu->read_word(t3);  /* 0x8013E054: 0x8D620000 */
    t4 = cpu->read_word(sp + 100);  /* 0x8013E058: 0x8FAC0064 */
    v0 <<= 2;  /* 0x8013E05C: 0x00021080 */
    v0 += v1;  /* 0x8013E060: 0x00431021 */
    v0 = cpu->read_word(v0);  /* 0x8013E064: 0x8C420000 */
    v1 = 0x09000000u;  /* 0x09000000 */  /* 0x8013E068: 0x3C030900 */
    v0 |= v1;  /* 0x8013E06C: 0x00431025 */
    cpu->write_word(t4, v0);  /* 0x8013E070: 0xAD820000 */
    v1 = cpu->read_word(t3);  /* 0x8013E074: 0x8D630000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013E078: 0x3C021F80 */
    v0 = cpu->read_word(v0 + 8);  /* 0x8013E07C: 0x8C420008 */
    v1 <<= 2;  /* 0x8013E080: 0x00031880 */
    v1 += v0;  /* 0x8013E084: 0x00621821 */
    v0 = t4 << 8;  /* 0x8013E088: 0x000C1200 */
    v0 = (uint32_t)v0 >> 8;  /* 0x8013E08C: 0x00021202 */
    cpu->write_word(v1, v0);  /* 0x8013E090: 0xAC620000 */
    /* fall through to block_8013E094 */

block_8013E094:
    s3 += 40;  /* 0x8013E094: 0x26730028 */
    fp += 4;  /* 0x8013E098: 0x27DE0004 */
    s7 += 4;  /* 0x8013E09C: 0x26F70004 */
    s6 += 4;  /* 0x8013E0A0: 0x26D60004 */
    s5 += 4;  /* 0x8013E0A4: 0x26B50004 */
    s4++;  /* 0x8013E0A8: 0x26940001 */
    t5 = cpu->read_word(sp + 100);  /* 0x8013E0AC: 0x8FAD0064 */
    v0 = (int32_t)s4 < 2;  /* 0x8013E0B0: 0x2A820002 */
    t5 += 40;  /* 0x8013E0B4: 0x25AD0028 */
    int cond_8013E0B8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 100, t5);  /* 0x8013E0BC: 0xAFAD0064 */
    if (cond_8013E0B8) {
        psx_check_interrupts_at(cpu, 0x8013DE9Cu);
        goto block_8013DE9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013E0C0u);
    }

block_8013E0C0:
    ra = cpu->read_word(sp + 156);  /* 0x8013E0C0: 0x8FBF009C */
    fp = cpu->read_word(sp + 152);  /* 0x8013E0C4: 0x8FBE0098 */
    s7 = cpu->read_word(sp + 148);  /* 0x8013E0C8: 0x8FB70094 */
    s6 = cpu->read_word(sp + 144);  /* 0x8013E0CC: 0x8FB60090 */
    s5 = cpu->read_word(sp + 140);  /* 0x8013E0D0: 0x8FB5008C */
    s4 = cpu->read_word(sp + 136);  /* 0x8013E0D4: 0x8FB40088 */
    s3 = cpu->read_word(sp + 132);  /* 0x8013E0D8: 0x8FB30084 */
    s2 = cpu->read_word(sp + 128);  /* 0x8013E0DC: 0x8FB20080 */
    s1 = cpu->read_word(sp + 124);  /* 0x8013E0E0: 0x8FB1007C */
    s0 = cpu->read_word(sp + 120);  /* 0x8013E0E4: 0x8FB00078 */
    uint32_t jt_8013E0E8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 160;  /* 0x8013E0EC: 0x27BD00A0 */
    psx_check_interrupts_at(cpu, jt_8013E0E8);
    cpu->pc = jt_8013E0E8; return;  /* CPS: jr $ra */
}
