/* recon_8016.c — clean-room reconstructions of guest functions in the
 * 0x8016xxxx band, ported from the cps_clean.c CPS transliterations.
 *
 * Ports keep the CPS coroutine contract (cd_response_pump style): the body
 * resumes on cpu->pc via the entry switch, every psx_check_interrupts_at is
 * a hard IRQ delivery point, and external jal targets tail-dispatch through
 * cpu->pc so clean_try_dispatch re-enters the callee. clean_cps.map routes
 * the entry + all continuation addrs here; the cps_80166a60 def in
 * cps_clean.c remains as unreachable shadow.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* battle_slot_draw — per-slot battle scene submit (guest 0x80166A60,
 * 1104 bytes / 53 blocks). a0 = combatant slot state record, a1 = option
 * flag bitfield. Submits one fighter's scene contribution: HUD variant
 * (battle_hud_draw @0x801737EC, two a0 forms selected by slot->8 +16/+8248
 * on flag (a1>>2)&1), conditional player_light_apply (@0x8012ECBC, modes
 * 1/2), prim_list_gte_invoke (@0x801871BC) prim batches, and
 * prim_list_persp_step (@0x8018B208) perspective passes — twice, once per
 * opponent slot s1/s2 base 0x1F8000E8/0x1F8001F0. Also calls
 * battle_intro_fsm (@0x80137098), fighter_trail_emit (@0x80135180), and
 * slot_height_proj (@0x80136B64); indexes the 0x801F4D70 stride-188 table
 * and writes halfwords into the 0x801F4CB0 area. Frame: sp-=64, saves
 * s0-s6+ra; all mutable state lives in cpu->gpr[]/guest RAM so scheduler
 * longjmps are safe. 69 IRQ yield points — collapsing them would defer
 * VBlank/IRQ delivery, so the CPS continuation form is kept. */
uint32_t battle_slot_draw(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80166B08u: goto block_80166B08;
            case 0x80166B9Cu: goto block_80166B9C;
            case 0x80166BC8u: goto block_80166BC8;
            case 0x80166BF4u: goto block_80166BF4;
            case 0x80166C58u: goto block_80166C58;
            case 0x80166CF0u: goto block_80166CF0;
            case 0x80166D1Cu: goto block_80166D1C;
            case 0x80166D48u: goto block_80166D48;
            case 0x80166DACu: goto block_80166DAC;
            case 0x80166DD4u: goto block_80166DD4;
            case 0x80166E48u: goto block_80166E48;
            case 0x80166E88u: goto block_80166E88;
            case 0x80166ABCu: goto block_80166ABC;
            case 0x80166ACCu: goto block_80166ACC;
            case 0x80166AE8u: goto block_80166AE8;
            case 0x80166B00u: goto block_80166B00;
            case 0x80166B14u: goto block_80166B14;
            case 0x80166B28u: goto block_80166B28;
            case 0x80166B3Cu: goto block_80166B3C;
            case 0x80166B6Cu: goto block_80166B6C;
            case 0x80166B7Cu: goto block_80166B7C;
            case 0x80166B94u: goto block_80166B94;
            case 0x80166BC0u: goto block_80166BC0;
            case 0x80166BECu: goto block_80166BEC;
            case 0x80166BF8u: goto block_80166BF8;
            case 0x80166C04u: goto block_80166C04;
            case 0x80166C14u: goto block_80166C14;
            case 0x80166C5Cu: goto block_80166C5C;
            case 0x80166C68u: goto block_80166C68;
            case 0x80166C7Cu: goto block_80166C7C;
            case 0x80166C90u: goto block_80166C90;
            case 0x80166CC0u: goto block_80166CC0;
            case 0x80166CD0u: goto block_80166CD0;
            case 0x80166CE8u: goto block_80166CE8;
            case 0x80166D14u: goto block_80166D14;
            case 0x80166D40u: goto block_80166D40;
            case 0x80166D4Cu: goto block_80166D4C;
            case 0x80166D58u: goto block_80166D58;
            case 0x80166D68u: goto block_80166D68;
            case 0x80166DB0u: goto block_80166DB0;
            case 0x80166DBCu: goto block_80166DBC;
            case 0x80166DC4u: goto block_80166DC4;
            case 0x80166DE4u: goto block_80166DE4;
            case 0x80166DF8u: goto block_80166DF8;
            case 0x80166E0Cu: goto block_80166E0C;
            case 0x80166E18u: goto block_80166E18;
            case 0x80166E20u: goto block_80166E20;
            case 0x80166E2Cu: goto block_80166E2C;
            case 0x80166E38u: goto block_80166E38;
            case 0x80166E58u: goto block_80166E58;
            case 0x80166E68u: goto block_80166E68;
            case 0x80166E74u: goto block_80166E74;
            default: break;
        }
    }
    /* Address: 0x80166A60, Size: 1104 bytes, Blocks: 53 */

block_80166A60:
    sp -= 64;  /* 0x80166A60: 0x27BDFFC0 */
    cpu->write_word(sp + 32, s0);  /* 0x80166A64: 0xAFB00020 */
    s0 = a0;  /* 0x80166A68: 0x00808021 */
    cpu->write_word(sp + 48, s4);  /* 0x80166A6C: 0xAFB40030 */
    s4 = a1;  /* 0x80166A70: 0x00A0A021 */
    cpu->write_word(sp + 36, s1);  /* 0x80166A74: 0xAFB10024 */
    s1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166A78: 0x3C111F80 */
    s1 |= 0x00E8;  /* 0x80166A7C: 0x363100E8 */
    cpu->write_word(sp + 40, s2);  /* 0x80166A80: 0xAFB20028 */
    s2 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166A84: 0x3C121F80 */
    s2 |= 0x01F0;  /* 0x80166A88: 0x365201F0 */
    cpu->write_word(sp + 52, s5);  /* 0x80166A8C: 0xAFB50034 */
    s5 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166A90: 0x3C151F80 */
    s5 |= 0x033C;  /* 0x80166A94: 0x36B5033C */
    cpu->write_word(sp + 56, s6);  /* 0x80166A98: 0xAFB60038 */
    s6 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166A9C: 0x3C161F80 */
    s6 |= 0x035C;  /* 0x80166AA0: 0x36D6035C */
    cpu->write_word(sp + 44, s3);  /* 0x80166AA4: 0xAFB3002C */
    s3 = s0 + 40;  /* 0x80166AA8: 0x26130028 */
    v0 = (int32_t)s4 >> 2;  /* 0x80166AAC: 0x00141083 */
    v0 &= 0x0001;  /* 0x80166AB0: 0x30420001 */
    int cond_80166AB4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 60, ra);  /* 0x80166AB8: 0xAFBF003C */
    if (cond_80166AB4) {
        psx_check_interrupts_at(cpu, 0x80166B08u);
        goto block_80166B08;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166ABCu);
    }

block_80166ABC:
    v0 = cpu->read_byte(s0 + 157);  /* 0x80166ABC: 0x9202009D */
    /* nop */  /* 0x80166AC0: 0x00000000 */
    int cond_80166AC4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s0;  /* 0x80166AC8: 0x02002821 */
    if (cond_80166AC4) {
        psx_check_interrupts_at(cpu, 0x80166AE8u);
        goto block_80166AE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166ACCu);
    }

block_80166ACC:
    a2 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166ACC: 0x3C061F80 */
    a2 |= 0x00E8;  /* 0x80166AD0: 0x34C600E8 */
    a3 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166AD4: 0x3C071F80 */
    a0 = cpu->read_word(s0 + 8);  /* 0x80166AD8: 0x8E040008 */
    a3 |= 0x01F0;  /* 0x80166ADC: 0x34E701F0 */
    /* delay slot (always executes) */
    a0 += 16;  /* 0x80166AE4: 0x24840010 */
    psx_check_interrupts_at(cpu, 0x80166B00u);
    goto block_80166B00;  /* j */

block_80166AE8:
    a2 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166AE8: 0x3C061F80 */
    a2 |= 0x00E8;  /* 0x80166AEC: 0x34C600E8 */
    a3 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166AF0: 0x3C071F80 */
    a0 = cpu->read_word(s0 + 8);  /* 0x80166AF4: 0x8E040008 */
    a3 |= 0x01F0;  /* 0x80166AF8: 0x34E701F0 */
    a0 += 8248;  /* 0x80166AFC: 0x24842038 */
    /* fall through to block_80166B00 */

block_80166B00:
    ra = 0x80166B08u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80166B04: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801737ECu);
    cpu->pc = 0x801737ECu; return cpu->gpr[2];  /* CPS jal -> battle_hud_draw (0x801737EC) */

block_80166B08:
    v0 = s4 & 0x0001;  /* 0x80166B08: 0x32820001 */
    int cond_80166B0C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 1;  /* 0x80166B10: 0x00141043 */
    if (cond_80166B0C) {
        psx_check_interrupts_at(cpu, 0x80166C5Cu);
        goto block_80166C5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166B14u);
    }

block_80166B14:
    v0 = cpu->read_word(s1 + 96);  /* 0x80166B14: 0x8E220060 */
    /* nop */  /* 0x80166B18: 0x00000000 */
    v0 &= 0x0002;  /* 0x80166B1C: 0x30420002 */
    int cond_80166B20 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 1;  /* 0x80166B24: 0x00141043 */
    if (cond_80166B20) {
        psx_check_interrupts_at(cpu, 0x80166C5Cu);
        goto block_80166C5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166B28u);
    }

block_80166B28:
    a1 = cpu->read_word(s1 + 108);  /* 0x80166B28: 0x8E25006C */
    /* nop */  /* 0x80166B2C: 0x00000000 */
    v0 = a1 & 0x0400;  /* 0x80166B30: 0x30A20400 */
    int cond_80166B34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x801F0000u;  /* 0x801F0000 */  /* 0x80166B38: 0x3C04801F */
    if (cond_80166B34) {
        psx_check_interrupts_at(cpu, 0x80166B6Cu);
        goto block_80166B6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166B3Cu);
    }

block_80166B3C:
    v1 = cpu->read_byte(s1 + 2);  /* 0x80166B3C: 0x92230002 */
    a0 += 19824;  /* 0x80166B40: 0x24844D70 */
    v0 = v1 << 1;  /* 0x80166B44: 0x00031040 */
    v0 += v1;  /* 0x80166B48: 0x00431021 */
    v0 <<= 4;  /* 0x80166B4C: 0x00021100 */
    v0 -= v1;  /* 0x80166B50: 0x00431023 */
    v0 <<= 2;  /* 0x80166B54: 0x00021080 */
    v0 += a0;  /* 0x80166B58: 0x00441021 */
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 2);  /* 0x80166B5C: 0x84420002 */
    /* nop */  /* 0x80166B60: 0x00000000 */
    int cond_80166B64 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 6;  /* 0x80166B68: 0x00141183 */
    if (cond_80166B64) {
        psx_check_interrupts_at(cpu, 0x80166BF8u);
        goto block_80166BF8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166B6Cu);
    }

block_80166B6C:
    v0 = 0x08000000u;  /* 0x08000000 */  /* 0x80166B6C: 0x3C020800 */
    v0 = a1 & v0;  /* 0x80166B70: 0x00A21024 */
    int cond_80166B74 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s1;  /* 0x80166B78: 0x02202021 */
    if (cond_80166B74) {
        psx_check_interrupts_at(cpu, 0x80166BC0u);
        goto block_80166BC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166B7Cu);
    }

block_80166B7C:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166B7C: 0x3C021F80 */
    v0 = cpu->read_word(v0 + 220);  /* 0x80166B80: 0x8C4200DC */
    /* nop */  /* 0x80166B84: 0x00000000 */
    v0 &= 0x0001;  /* 0x80166B88: 0x30420001 */
    int cond_80166B8C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80166B90: 0x00000000 */
    if (cond_80166B8C) {
        psx_check_interrupts_at(cpu, 0x80166BC0u);
        goto block_80166BC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166B94u);
    }

block_80166B94:
    ra = 0x80166B9Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;  /* 0x80166B98: 0x24050001 */
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return cpu->gpr[2];  /* CPS jal -> player_light_apply (0x8012ECBC) */

block_80166B9C:
    v0 = cpu->read_word(s1 + 40);  /* 0x80166B9C: 0x8E220028 */
    a1 = cpu->read_word(s0 + 8);  /* 0x80166BA0: 0x8E050008 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);  /* 0x80166BA4: 0x80470024 */
    cpu->write_word(sp + 16, s3);  /* 0x80166BA8: 0xAFB30010 */
    cpu->write_word(sp + 20, s5);  /* 0x80166BAC: 0xAFB50014 */
    a0 = cpu->read_word(v0 + 52);  /* 0x80166BB0: 0x8C440034 */
    a2 = cpu->read_word(s0 + 12);  /* 0x80166BB4: 0x8E06000C */
    /* delay slot (always executes) */
    a1 += 64;  /* 0x80166BBC: 0x24A50040 */
    psx_check_interrupts_at(cpu, 0x80166BECu);
    goto block_80166BEC;  /* j */

block_80166BC0:
    ra = 0x80166BC8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;  /* 0x80166BC4: 0x24050001 */
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return cpu->gpr[2];  /* CPS jal -> player_light_apply (0x8012ECBC) */

block_80166BC8:
    a0 = cpu->read_word(s1 + 24);  /* 0x80166BC8: 0x8E240018 */
    a1 = cpu->read_word(s0 + 8);  /* 0x80166BCC: 0x8E050008 */
    cpu->write_word(sp + 16, s3);  /* 0x80166BD0: 0xAFB30010 */
    cpu->write_word(sp + 20, s5);  /* 0x80166BD4: 0xAFB50014 */
    a2 = cpu->read_word(s0 + 12);  /* 0x80166BD8: 0x8E06000C */
    a3 = cpu->read_byte(s1 + 6);  /* 0x80166BDC: 0x92270006 */
    a1 += 64;  /* 0x80166BE0: 0x24A50040 */
    a3 <<= 24;  /* 0x80166BE4: 0x00073E00 */
    a3 = (int32_t)a3 >> 24;  /* 0x80166BE8: 0x00073E03 */
    /* fall through to block_80166BEC */

block_80166BEC:
    ra = 0x80166BF4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80166BF0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return cpu->gpr[2];  /* CPS jal -> prim_list_gte_invoke (0x801871BC) */

block_80166BF4:
    v0 = (int32_t)s4 >> 6;  /* 0x80166BF4: 0x00141183 */
    /* fall through to block_80166BF8 */

block_80166BF8:
    v0 &= 0x0001;  /* 0x80166BF8: 0x30420001 */
    int cond_80166BFC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 1;  /* 0x80166C00: 0x00141043 */
    if (cond_80166BFC) {
        psx_check_interrupts_at(cpu, 0x80166C5Cu);
        goto block_80166C5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166C04u);
    }

block_80166C04:
    v0 = cpu->read_byte(s0 + 159);  /* 0x80166C04: 0x9202009F */
    /* nop */  /* 0x80166C08: 0x00000000 */
    int cond_80166C0C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 1;  /* 0x80166C10: 0x00141043 */
    if (cond_80166C0C) {
        psx_check_interrupts_at(cpu, 0x80166C5Cu);
        goto block_80166C5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166C14u);
    }

block_80166C14:
    a3 = 15;  /* 0x80166C14: 0x2407000F */
    a1 = cpu->read_word(s0 + 8);  /* 0x80166C18: 0x8E050008 */
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80166C1C: 0x3C02801B */
    cpu->write_word(sp + 16, s3);  /* 0x80166C20: 0xAFB30010 */
    cpu->write_word(sp + 20, s6);  /* 0x80166C24: 0xAFB60014 */
    v1 = (int32_t)(int8_t)cpu->read_byte(s0 + 152);  /* 0x80166C28: 0x82030098 */
    v0 += 5812;  /* 0x80166C2C: 0x244216B4 */
    v1 <<= 3;  /* 0x80166C30: 0x000318C0 */
    v1 += v0;  /* 0x80166C34: 0x00621821 */
    cpu->write_word(sp + 24, v1);  /* 0x80166C38: 0xAFA30018 */
    v0 = cpu->read_word(s0 + 8);  /* 0x80166C3C: 0x8E020008 */
    a0 = cpu->read_word(s1 + 28);  /* 0x80166C40: 0x8E24001C */
    v0 += 8300;  /* 0x80166C44: 0x2442206C */
    cpu->write_word(sp + 28, v0);  /* 0x80166C48: 0xAFA2001C */
    a2 = cpu->read_word(s0 + 12);  /* 0x80166C4C: 0x8E06000C */
    ra = 0x80166C58u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 += 8256;  /* 0x80166C54: 0x24A52040 */
    psx_check_interrupts_at(cpu, 0x8018B208u);
    cpu->pc = 0x8018B208u; return cpu->gpr[2];  /* CPS jal -> prim_list_persp_step (0x8018B208) */

block_80166C58:
    v0 = (int32_t)s4 >> 1;  /* 0x80166C58: 0x00141043 */
    /* fall through to block_80166C5C */

block_80166C5C:
    v0 &= 0x0001;  /* 0x80166C5C: 0x30420001 */
    int cond_80166C60 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 4;  /* 0x80166C64: 0x00141103 */
    if (cond_80166C60) {
        psx_check_interrupts_at(cpu, 0x80166DB0u);
        goto block_80166DB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166C68u);
    }

block_80166C68:
    v0 = cpu->read_word(s2 + 96);  /* 0x80166C68: 0x8E420060 */
    /* nop */  /* 0x80166C6C: 0x00000000 */
    v0 &= 0x0002;  /* 0x80166C70: 0x30420002 */
    int cond_80166C74 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 4;  /* 0x80166C78: 0x00141103 */
    if (cond_80166C74) {
        psx_check_interrupts_at(cpu, 0x80166DB0u);
        goto block_80166DB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166C7Cu);
    }

block_80166C7C:
    a1 = cpu->read_word(s2 + 108);  /* 0x80166C7C: 0x8E45006C */
    /* nop */  /* 0x80166C80: 0x00000000 */
    v0 = a1 & 0x0400;  /* 0x80166C84: 0x30A20400 */
    int cond_80166C88 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x801F0000u;  /* 0x801F0000 */  /* 0x80166C8C: 0x3C04801F */
    if (cond_80166C88) {
        psx_check_interrupts_at(cpu, 0x80166CC0u);
        goto block_80166CC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166C90u);
    }

block_80166C90:
    v1 = cpu->read_byte(s2 + 2);  /* 0x80166C90: 0x92430002 */
    a0 += 19824;  /* 0x80166C94: 0x24844D70 */
    v0 = v1 << 1;  /* 0x80166C98: 0x00031040 */
    v0 += v1;  /* 0x80166C9C: 0x00431021 */
    v0 <<= 4;  /* 0x80166CA0: 0x00021100 */
    v0 -= v1;  /* 0x80166CA4: 0x00431023 */
    v0 <<= 2;  /* 0x80166CA8: 0x00021080 */
    v0 += a0;  /* 0x80166CAC: 0x00441021 */
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 2);  /* 0x80166CB0: 0x84420002 */
    /* nop */  /* 0x80166CB4: 0x00000000 */
    int cond_80166CB8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 6;  /* 0x80166CBC: 0x00141183 */
    if (cond_80166CB8) {
        psx_check_interrupts_at(cpu, 0x80166D4Cu);
        goto block_80166D4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166CC0u);
    }

block_80166CC0:
    v0 = 0x08000000u;  /* 0x08000000 */  /* 0x80166CC0: 0x3C020800 */
    v0 = a1 & v0;  /* 0x80166CC4: 0x00A21024 */
    int cond_80166CC8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s2;  /* 0x80166CCC: 0x02402021 */
    if (cond_80166CC8) {
        psx_check_interrupts_at(cpu, 0x80166D14u);
        goto block_80166D14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166CD0u);
    }

block_80166CD0:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80166CD0: 0x3C021F80 */
    v0 = cpu->read_word(v0 + 220);  /* 0x80166CD4: 0x8C4200DC */
    /* nop */  /* 0x80166CD8: 0x00000000 */
    v0 &= 0x0001;  /* 0x80166CDC: 0x30420001 */
    int cond_80166CE0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80166CE4: 0x00000000 */
    if (cond_80166CE0) {
        psx_check_interrupts_at(cpu, 0x80166D14u);
        goto block_80166D14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166CE8u);
    }

block_80166CE8:
    ra = 0x80166CF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;  /* 0x80166CEC: 0x24050001 */
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return cpu->gpr[2];  /* CPS jal -> player_light_apply (0x8012ECBC) */

block_80166CF0:
    v0 = cpu->read_word(s2 + 40);  /* 0x80166CF0: 0x8E420028 */
    a1 = cpu->read_word(s0 + 8);  /* 0x80166CF4: 0x8E050008 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);  /* 0x80166CF8: 0x80470024 */
    cpu->write_word(sp + 16, s3);  /* 0x80166CFC: 0xAFB30010 */
    cpu->write_word(sp + 20, s5);  /* 0x80166D00: 0xAFB50014 */
    a0 = cpu->read_word(v0 + 52);  /* 0x80166D04: 0x8C440034 */
    a2 = cpu->read_word(s0 + 12);  /* 0x80166D08: 0x8E06000C */
    /* delay slot (always executes) */
    a1 += 64;  /* 0x80166D10: 0x24A50040 */
    psx_check_interrupts_at(cpu, 0x80166D40u);
    goto block_80166D40;  /* j */

block_80166D14:
    ra = 0x80166D1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;  /* 0x80166D18: 0x24050002 */
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return cpu->gpr[2];  /* CPS jal -> player_light_apply (0x8012ECBC) */

block_80166D1C:
    a0 = cpu->read_word(s2 + 24);  /* 0x80166D1C: 0x8E440018 */
    a1 = cpu->read_word(s0 + 8);  /* 0x80166D20: 0x8E050008 */
    cpu->write_word(sp + 16, s3);  /* 0x80166D24: 0xAFB30010 */
    cpu->write_word(sp + 20, s5);  /* 0x80166D28: 0xAFB50014 */
    a2 = cpu->read_word(s0 + 12);  /* 0x80166D2C: 0x8E06000C */
    a3 = cpu->read_byte(s2 + 6);  /* 0x80166D30: 0x92470006 */
    a1 += 64;  /* 0x80166D34: 0x24A50040 */
    a3 <<= 24;  /* 0x80166D38: 0x00073E00 */
    a3 = (int32_t)a3 >> 24;  /* 0x80166D3C: 0x00073E03 */
    /* fall through to block_80166D40 */

block_80166D40:
    ra = 0x80166D48u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80166D44: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return cpu->gpr[2];  /* CPS jal -> prim_list_gte_invoke (0x801871BC) */

block_80166D48:
    v0 = (int32_t)s4 >> 6;  /* 0x80166D48: 0x00141183 */
    /* fall through to block_80166D4C */

block_80166D4C:
    v0 &= 0x0001;  /* 0x80166D4C: 0x30420001 */
    int cond_80166D50 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 4;  /* 0x80166D54: 0x00141103 */
    if (cond_80166D50) {
        psx_check_interrupts_at(cpu, 0x80166DB0u);
        goto block_80166DB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166D58u);
    }

block_80166D58:
    v0 = cpu->read_byte(s0 + 159);  /* 0x80166D58: 0x9202009F */
    /* nop */  /* 0x80166D5C: 0x00000000 */
    int cond_80166D60 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 4;  /* 0x80166D64: 0x00141103 */
    if (cond_80166D60) {
        psx_check_interrupts_at(cpu, 0x80166DB0u);
        goto block_80166DB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166D68u);
    }

block_80166D68:
    a3 = 15;  /* 0x80166D68: 0x2407000F */
    a1 = cpu->read_word(s0 + 8);  /* 0x80166D6C: 0x8E050008 */
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80166D70: 0x3C02801B */
    cpu->write_word(sp + 16, s3);  /* 0x80166D74: 0xAFB30010 */
    cpu->write_word(sp + 20, s6);  /* 0x80166D78: 0xAFB60014 */
    v1 = (int32_t)(int8_t)cpu->read_byte(s0 + 152);  /* 0x80166D7C: 0x82030098 */
    v0 += 5812;  /* 0x80166D80: 0x244216B4 */
    v1 <<= 3;  /* 0x80166D84: 0x000318C0 */
    v1 += v0;  /* 0x80166D88: 0x00621821 */
    cpu->write_word(sp + 24, v1);  /* 0x80166D8C: 0xAFA30018 */
    v0 = cpu->read_word(s0 + 8);  /* 0x80166D90: 0x8E020008 */
    a0 = cpu->read_word(s2 + 28);  /* 0x80166D94: 0x8E44001C */
    v0 += 8300;  /* 0x80166D98: 0x2442206C */
    cpu->write_word(sp + 28, v0);  /* 0x80166D9C: 0xAFA2001C */
    a2 = cpu->read_word(s0 + 12);  /* 0x80166DA0: 0x8E06000C */
    ra = 0x80166DACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 += 8256;  /* 0x80166DA8: 0x24A52040 */
    psx_check_interrupts_at(cpu, 0x8018B208u);
    cpu->pc = 0x8018B208u; return cpu->gpr[2];  /* CPS jal -> prim_list_persp_step (0x8018B208) */

block_80166DAC:
    v0 = (int32_t)s4 >> 4;  /* 0x80166DAC: 0x00141103 */
    /* fall through to block_80166DB0 */

block_80166DB0:
    v0 &= 0x0001;  /* 0x80166DB0: 0x30420001 */
    int cond_80166DB4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;  /* 0x80166DB8: 0x02602021 */
    if (cond_80166DB4) {
        psx_check_interrupts_at(cpu, 0x80166DC4u);
        goto block_80166DC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166DBCu);
    }

block_80166DBC:
    v0 = 1;  /* 0x80166DBC: 0x24020001 */
    cpu->write_byte(s0 + 177, (uint8_t)v0);  /* 0x80166DC0: 0xA20200B1 */
    /* fall through to block_80166DC4 */

block_80166DC4:
    a1 = cpu->read_word(s0 + 8);  /* 0x80166DC4: 0x8E050008 */
    a2 = cpu->read_word(s0 + 12);  /* 0x80166DC8: 0x8E06000C */
    ra = 0x80166DD4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;  /* 0x80166DD0: 0x02003821 */
    psx_check_interrupts_at(cpu, 0x80137098u);
    cpu->pc = 0x80137098u; return cpu->gpr[2];  /* CPS jal -> battle_intro_fsm (0x80137098) */

block_80166DD4:
    v0 = (int32_t)s4 >> 8;  /* 0x80166DD4: 0x00141203 */
    v0 &= 0x0001;  /* 0x80166DD8: 0x30420001 */
    int cond_80166DDC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 3;  /* 0x80166DE0: 0x001410C3 */
    if (cond_80166DDC) {
        psx_check_interrupts_at(cpu, 0x80166E2Cu);
        goto block_80166E2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166DE4u);
    }

block_80166DE4:
    v0 = cpu->read_word(s1 + 108);  /* 0x80166DE4: 0x8E22006C */
    /* nop */  /* 0x80166DE8: 0x00000000 */
    v0 &= 0x8000;  /* 0x80166DEC: 0x30428000 */
    int cond_80166DF0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801F0000u;  /* 0x801F0000 */  /* 0x80166DF4: 0x3C02801F */
    if (cond_80166DF0) {
        psx_check_interrupts_at(cpu, 0x80166E0Cu);
        goto block_80166E0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166DF8u);
    }

block_80166DF8:
    v0 = cpu->read_word(s2 + 108);  /* 0x80166DF8: 0x8E42006C */
    /* nop */  /* 0x80166DFC: 0x00000000 */
    v0 &= 0x8000;  /* 0x80166E00: 0x30428000 */
    int cond_80166E04 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801F0000u;  /* 0x801F0000 */  /* 0x80166E08: 0x3C02801F */
    if (cond_80166E04) {
        psx_check_interrupts_at(cpu, 0x80166E18u);
        goto block_80166E18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166E0Cu);
    }

block_80166E0C:
    v0 += 19632;  /* 0x80166E0C: 0x24424CB0 */
    /* delay slot (always executes) */
    v1 = 16;  /* 0x80166E14: 0x24030010 */
    psx_check_interrupts_at(cpu, 0x80166E20u);
    goto block_80166E20;  /* j */

block_80166E18:
    v0 += 19632;  /* 0x80166E18: 0x24424CB0 */
    v1 = -8;  /* 0x80166E1C: 0x2403FFF8 */
    /* fall through to block_80166E20 */

block_80166E20:
    cpu->write_half(v0 + 2, (uint16_t)zero);  /* 0x80166E20: 0xA4400002 */
    cpu->write_half(v0 + 6, (uint16_t)v1);  /* 0x80166E24: 0xA4430006 */
    v0 = (int32_t)s4 >> 3;  /* 0x80166E28: 0x001410C3 */
    /* fall through to block_80166E2C */

block_80166E2C:
    v0 &= 0x0001;  /* 0x80166E2C: 0x30420001 */
    int cond_80166E30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;  /* 0x80166E34: 0x02602021 */
    if (cond_80166E30) {
        psx_check_interrupts_at(cpu, 0x80166E48u);
        goto block_80166E48;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166E38u);
    }

block_80166E38:
    a1 = s5;  /* 0x80166E38: 0x02A02821 */
    a2 = cpu->read_word(s0 + 188);  /* 0x80166E3C: 0x8E0600BC */
    ra = 0x80166E48u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;  /* 0x80166E44: 0x02003821 */
    psx_check_interrupts_at(cpu, 0x80135180u);
    cpu->pc = 0x80135180u; return cpu->gpr[2];  /* CPS jal -> fighter_trail_emit (0x80135180) */

block_80166E48:
    v0 = cpu->read_byte(s0 + 159);  /* 0x80166E48: 0x9202009F */
    /* nop */  /* 0x80166E4C: 0x00000000 */
    int cond_80166E50 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80166E54: 0x00000000 */
    if (cond_80166E50) {
        psx_check_interrupts_at(cpu, 0x80166E88u);
        goto block_80166E88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166E58u);
    }

block_80166E58:
    v0 = cpu->read_byte(s0 + 158);  /* 0x80166E58: 0x9202009E */
    /* nop */  /* 0x80166E5C: 0x00000000 */
    int cond_80166E60 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s4 >> 5;  /* 0x80166E64: 0x00141143 */
    if (cond_80166E60) {
        psx_check_interrupts_at(cpu, 0x80166E88u);
        goto block_80166E88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166E68u);
    }

block_80166E68:
    v0 &= 0x0001;  /* 0x80166E68: 0x30420001 */
    int cond_80166E6C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = s0;  /* 0x80166E70: 0x02003821 */
    if (cond_80166E6C) {
        psx_check_interrupts_at(cpu, 0x80166E88u);
        goto block_80166E88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80166E74u);
    }

block_80166E74:
    a1 = cpu->read_word(a3 + 188);  /* 0x80166E74: 0x8CE500BC */
    a0 = cpu->read_word(a3 + 8);  /* 0x80166E78: 0x8CE40008 */
    a2 = cpu->read_word(a3 + 12);  /* 0x80166E7C: 0x8CE6000C */
    ra = 0x80166E88u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 += 8316;  /* 0x80166E84: 0x2484207C */
    psx_check_interrupts_at(cpu, 0x80136B64u);
    cpu->pc = 0x80136B64u; return cpu->gpr[2];  /* CPS jal -> slot_height_proj (0x80136B64) */

block_80166E88:
    ra = cpu->read_word(sp + 60);  /* 0x80166E88: 0x8FBF003C */
    s6 = cpu->read_word(sp + 56);  /* 0x80166E8C: 0x8FB60038 */
    s5 = cpu->read_word(sp + 52);  /* 0x80166E90: 0x8FB50034 */
    s4 = cpu->read_word(sp + 48);  /* 0x80166E94: 0x8FB40030 */
    s3 = cpu->read_word(sp + 44);  /* 0x80166E98: 0x8FB3002C */
    s2 = cpu->read_word(sp + 40);  /* 0x80166E9C: 0x8FB20028 */
    s1 = cpu->read_word(sp + 36);  /* 0x80166EA0: 0x8FB10024 */
    s0 = cpu->read_word(sp + 32);  /* 0x80166EA4: 0x8FB00020 */
    uint32_t jt_80166EA8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 64;  /* 0x80166EAC: 0x27BD0040 */
    psx_check_interrupts_at(cpu, jt_80166EA8);
    cpu->pc = jt_80166EA8; return cpu->gpr[2];  /* CPS: jr $ra */
}
