/* w2_e01.c — clean CPS ports of fighter_script_fsm piece-entries
 * (guest 0x80181FF8/0x80182090/0x801822D0/0x801823BC/0x80182690/
 *  0x80182740/0x8018287C/0x80182960, SLES_017.22_full_38.c).
 *
 * All eight addrs are CPS piece-entries inside the fighter_script_fsm
 * (guest 0x80181F20) continuation surface — reached via `cpu->pc = <addr>`
 * coroutine/branch dispatches, NOT ordinary `jr $ra` function entries.
 * They share ONE generated body (br2eu_func_80181F20 /
 * br2eu_psx_alias_body_80181F20, 215 blocks), so they share ONE clean
 * body below: fighter_script_fsm_cps() carries the full block_* / cpu->pc
 * continuation switch and is faithful op-by-op to the generated
 * transliteration (same guest ops, order, yield points; minus cycle/
 * icache/cosim/debug instrumentation). The eight guest addrs are routed
 * to it through game/clean_cps.map as CLEAN_ENT_CPS entries.
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

/* fighter_script_fsm_cps — clean CPS transliteration of br2eu_func_80181F20 (guest 0x80181f20)
 * from SLES_017.22_full_37.c. */
void fighter_script_fsm_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801825F8u: goto block_801825F8;
            case 0x80182A80u: goto block_80182A80;
            case 0x80181F70u: goto block_80181F70;
            case 0x80181F88u: goto block_80181F88;
            case 0x80181F94u: goto block_80181F94;
            case 0x80181FACu: goto block_80181FAC;
            case 0x80181FB0u: goto block_80181FB0;
            case 0x80181FDCu: goto block_80181FDC;
            case 0x80181FF8u: goto block_80181FF8;
            case 0x80182040u: goto block_80182040;
            case 0x80182044u: goto block_80182044;
            case 0x80182090u: goto block_80182090;
            case 0x801820A4u: goto block_801820A4;
            case 0x801820B0u: goto block_801820B0;
            case 0x801820C0u: goto block_801820C0;
            case 0x801820D8u: goto block_801820D8;
            case 0x801820E0u: goto block_801820E0;
            case 0x80182118u: goto block_80182118;
            case 0x8018212Cu: goto block_8018212C;
            case 0x80182134u: goto block_80182134;
            case 0x8018213Cu: goto block_8018213C;
            case 0x80182144u: goto block_80182144;
            case 0x80182154u: goto block_80182154;
            case 0x80182158u: goto block_80182158;
            case 0x80182164u: goto block_80182164;
            case 0x80182170u: goto block_80182170;
            case 0x80182178u: goto block_80182178;
            case 0x80182180u: goto block_80182180;
            case 0x80182190u: goto block_80182190;
            case 0x801821A0u: goto block_801821A0;
            case 0x801821B4u: goto block_801821B4;
            case 0x801821BCu: goto block_801821BC;
            case 0x801821CCu: goto block_801821CC;
            case 0x801821DCu: goto block_801821DC;
            case 0x80182218u: goto block_80182218;
            case 0x80182248u: goto block_80182248;
            case 0x80182250u: goto block_80182250;
            case 0x80182258u: goto block_80182258;
            case 0x8018226Cu: goto block_8018226C;
            case 0x80182274u: goto block_80182274;
            case 0x801822A4u: goto block_801822A4;
            case 0x801822B8u: goto block_801822B8;
            case 0x801822BCu: goto block_801822BC;
            case 0x801822C8u: goto block_801822C8;
            case 0x801822D0u: goto block_801822D0;
            case 0x801822E4u: goto block_801822E4;
            case 0x801822F4u: goto block_801822F4;
            case 0x801822F8u: goto block_801822F8;
            case 0x8018230Cu: goto block_8018230C;
            case 0x80182318u: goto block_80182318;
            case 0x8018233Cu: goto block_8018233C;
            case 0x80182350u: goto block_80182350;
            case 0x80182370u: goto block_80182370;
            case 0x801823A0u: goto block_801823A0;
            case 0x801823A8u: goto block_801823A8;
            case 0x801823B0u: goto block_801823B0;
            case 0x801823B8u: goto block_801823B8;
            case 0x801823BCu: goto block_801823BC;
            case 0x801823D0u: goto block_801823D0;
            case 0x801823E4u: goto block_801823E4;
            case 0x801823ECu: goto block_801823EC;
            case 0x80182408u: goto block_80182408;
            case 0x80182420u: goto block_80182420;
            case 0x80182444u: goto block_80182444;
            case 0x80182448u: goto block_80182448;
            case 0x80182458u: goto block_80182458;
            case 0x8018246Cu: goto block_8018246C;
            case 0x80182474u: goto block_80182474;
            case 0x80182480u: goto block_80182480;
            case 0x80182494u: goto block_80182494;
            case 0x801824A8u: goto block_801824A8;
            case 0x801824B4u: goto block_801824B4;
            case 0x801824BCu: goto block_801824BC;
            case 0x801824C8u: goto block_801824C8;
            case 0x801824DCu: goto block_801824DC;
            case 0x801824E4u: goto block_801824E4;
            case 0x801824E8u: goto block_801824E8;
            case 0x801824F8u: goto block_801824F8;
            case 0x80182500u: goto block_80182500;
            case 0x80182518u: goto block_80182518;
            case 0x8018252Cu: goto block_8018252C;
            case 0x80182530u: goto block_80182530;
            case 0x80182544u: goto block_80182544;
            case 0x80182564u: goto block_80182564;
            case 0x80182590u: goto block_80182590;
            case 0x801825CCu: goto block_801825CC;
            case 0x801825E8u: goto block_801825E8;
            case 0x80182608u: goto block_80182608;
            case 0x80182678u: goto block_80182678;
            case 0x8018267Cu: goto block_8018267C;
            case 0x80182690u: goto block_80182690;
            case 0x801826A4u: goto block_801826A4;
            case 0x801826B0u: goto block_801826B0;
            case 0x801826C0u: goto block_801826C0;
            case 0x801826D4u: goto block_801826D4;
            case 0x801826DCu: goto block_801826DC;
            case 0x801826ECu: goto block_801826EC;
            case 0x801826F4u: goto block_801826F4;
            case 0x80182724u: goto block_80182724;
            case 0x80182730u: goto block_80182730;
            case 0x8018273Cu: goto block_8018273C;
            case 0x80182740u: goto block_80182740;
            case 0x80182754u: goto block_80182754;
            case 0x80182768u: goto block_80182768;
            case 0x80182774u: goto block_80182774;
            case 0x80182788u: goto block_80182788;
            case 0x8018278Cu: goto block_8018278C;
            case 0x80182794u: goto block_80182794;
            case 0x801827A0u: goto block_801827A0;
            case 0x801827B0u: goto block_801827B0;
            case 0x801827B8u: goto block_801827B8;
            case 0x801827E8u: goto block_801827E8;
            case 0x801827F4u: goto block_801827F4;
            case 0x80182800u: goto block_80182800;
            case 0x8018280Cu: goto block_8018280C;
            case 0x80182814u: goto block_80182814;
            case 0x8018284Cu: goto block_8018284C;
            case 0x80182860u: goto block_80182860;
            case 0x80182868u: goto block_80182868;
            case 0x80182874u: goto block_80182874;
            case 0x8018287Cu: goto block_8018287C;
            case 0x8018288Cu: goto block_8018288C;
            case 0x80182894u: goto block_80182894;
            case 0x801828A4u: goto block_801828A4;
            case 0x801828ACu: goto block_801828AC;
            case 0x801828B8u: goto block_801828B8;
            case 0x801828C0u: goto block_801828C0;
            case 0x801828C8u: goto block_801828C8;
            case 0x801828D0u: goto block_801828D0;
            case 0x80182900u: goto block_80182900;
            case 0x80182904u: goto block_80182904;
            case 0x80182908u: goto block_80182908;
            case 0x80182944u: goto block_80182944;
            case 0x80182958u: goto block_80182958;
            case 0x80182960u: goto block_80182960;
            case 0x80182974u: goto block_80182974;
            case 0x80182988u: goto block_80182988;
            case 0x8018298Cu: goto block_8018298C;
            case 0x8018299Cu: goto block_8018299C;
            case 0x801829B4u: goto block_801829B4;
            case 0x801829ECu: goto block_801829EC;
            case 0x801829FCu: goto block_801829FC;
            case 0x80182A18u: goto block_80182A18;
            case 0x80182A4Cu: goto block_80182A4C;
            case 0x80182A50u: goto block_80182A50;
            case 0x80182A58u: goto block_80182A58;
            case 0x80182A6Cu: goto block_80182A6C;
            case 0x80182A88u: goto block_80182A88;
            case 0x80182AA4u: goto block_80182AA4;
            case 0x80182AC0u: goto block_80182AC0;
            case 0x80182AE0u: goto block_80182AE0;
            case 0x80182AF4u: goto block_80182AF4;
            case 0x80182B18u: goto block_80182B18;
            case 0x80182B28u: goto block_80182B28;
            case 0x80182B3Cu: goto block_80182B3C;
            case 0x80182B6Cu: goto block_80182B6C;
            case 0x80182B74u: goto block_80182B74;
            case 0x80182B88u: goto block_80182B88;
            case 0x80182B9Cu: goto block_80182B9C;
            case 0x80182BCCu: goto block_80182BCC;
            case 0x80182BECu: goto block_80182BEC;
            case 0x80182BF0u: goto block_80182BF0;
            case 0x80182BFCu: goto block_80182BFC;
            case 0x80182C04u: goto block_80182C04;
            case 0x80182C14u: goto block_80182C14;
            case 0x80182C28u: goto block_80182C28;
            case 0x80182C38u: goto block_80182C38;
            case 0x80182C40u: goto block_80182C40;
            case 0x80182C4Cu: goto block_80182C4C;
            case 0x80182C60u: goto block_80182C60;
            case 0x80182C74u: goto block_80182C74;
            case 0x80182C84u: goto block_80182C84;
            case 0x80182C90u: goto block_80182C90;
            case 0x80182C98u: goto block_80182C98;
            case 0x80182CA4u: goto block_80182CA4;
            case 0x80182CACu: goto block_80182CAC;
            case 0x80182CBCu: goto block_80182CBC;
            case 0x80182CD4u: goto block_80182CD4;
            case 0x80182CE8u: goto block_80182CE8;
            case 0x80182CF8u: goto block_80182CF8;
            case 0x80182D0Cu: goto block_80182D0C;
            case 0x80182D14u: goto block_80182D14;
            case 0x80182D1Cu: goto block_80182D1C;
            case 0x80182D20u: goto block_80182D20;
            case 0x80182D24u: goto block_80182D24;
            case 0x80182D3Cu: goto block_80182D3C;
            case 0x80182D50u: goto block_80182D50;
            case 0x80182D64u: goto block_80182D64;
            case 0x80182D94u: goto block_80182D94;
            case 0x80182D9Cu: goto block_80182D9C;
            case 0x80182DB4u: goto block_80182DB4;
            case 0x80182DD4u: goto block_80182DD4;
            case 0x80182DD8u: goto block_80182DD8;
            case 0x80182DE4u: goto block_80182DE4;
            case 0x80182DECu: goto block_80182DEC;
            case 0x80182DF0u: goto block_80182DF0;
            case 0x80182DF8u: goto block_80182DF8;
            case 0x80182E08u: goto block_80182E08;
            case 0x80182E1Cu: goto block_80182E1C;
            case 0x80182E28u: goto block_80182E28;
            case 0x80182E3Cu: goto block_80182E3C;
            case 0x80182E50u: goto block_80182E50;
            case 0x80182E64u: goto block_80182E64;
            case 0x80182E70u: goto block_80182E70;
            case 0x80182E84u: goto block_80182E84;
            case 0x80182E98u: goto block_80182E98;
            case 0x80182E9Cu: goto block_80182E9C;
            case 0x80182EA4u: goto block_80182EA4;
            case 0x80182EA8u: goto block_80182EA8;
            case 0x80182EB0u: goto block_80182EB0;
            case 0x80182EC4u: goto block_80182EC4;
            case 0x80182ECCu: goto block_80182ECC;
            case 0x80182ED4u: goto block_80182ED4;
            case 0x80182EE0u: goto block_80182EE0;
            case 0x80182EE4u: goto block_80182EE4;
            case 0x80182EF8u: goto block_80182EF8;
            case 0x80182EFCu: goto block_80182EFC;
            case 0x80182F10u: goto block_80182F10;
            case 0x80182F20u: goto block_80182F20;
            default: break;
        }
    }
    /* Address: 0x80181F20, Size: 4124 bytes, Blocks: 215 */

    /* Entry — args: s1=a0 script descriptor, s3=a1,
     * s0=a2 script context, s2=a3 aux object. Init ctx[120]=&ctx[124],
     * ctx[124]=0; compute mode flag a1 (0/1) from descriptor byte+2
     * and aux-object state halfwords (+4/+6 == 4). */
block_80181F20:
    sp = sp + -40;
    cpu->write_word(sp + 20, s1);
    s1 = a0;
    cpu->write_word(sp + 28, s3);
    s3 = a1;
    cpu->write_word(sp + 16, s0);
    s0 = a2;
    cpu->write_word(sp + 24, s2);
    s2 = a3;
    cpu->write_word(sp + 32, ra);
    t0 = cpu->read_word(s1 + 44);
    v0 = s0 + 124;
    cpu->write_word(s0 + 120, v0);
    v0 = cpu->read_half(s0 + 8);
    a1 = zero;
    cpu->write_byte(s0 + 124, (uint8_t)zero);
    a0 = cpu->read_byte(s1 + 2);
    /* nop */  /* 0x80181F64: 0x00000000 */
    int cond_a0_ne_zero = (a0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t1 = (uint32_t)v0 >> 4;
    if (cond_a0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80181F88u);
        goto block_80181F88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80181F70u);
        goto block_80181F70;  /* not taken */
    }

    /* Mode-flag compute: aux = *(s2+200); flag = 1 iff
     * descriptor[2]!=0 XOR-state checks on aux+4/aux+6 halfwords. */
block_80181F70:
    v0 = cpu->read_word(s2 + 200);
    /* nop */  /* 0x80181F74: 0x00000000 */
    v1 = (int32_t)(int16_t)cpu->read_half(v0 + 4);
    v0 = 4;
    int cond_v1_eq_v0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80181F84: 0x00000000 */
    if (cond_v1_eq_v0) {
        psx_check_interrupts_at(cpu, 0x80181FACu);
        goto block_80181FAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80181F88u);
        goto block_80181F88;  /* not taken */
    }

block_80181F88:
    v0 = 1;
    int cond_a0_ne_v0 = (a0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80181F90: 0x00000000 */
    if (cond_a0_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80181FB0u);
        goto block_80181FB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80181F94u);
        goto block_80181F94;  /* not taken */
    }

block_80181F94:
    v0 = cpu->read_word(s2 + 200);
    /* nop */  /* 0x80181F98: 0x00000000 */
    v1 = (int32_t)(int16_t)cpu->read_half(v0 + 6);
    v0 = 4;
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80181FA8: 0x00000000 */
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80181FB0u);
        goto block_80181FB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80181FACu);
        goto block_80181FAC;  /* not taken */
    }

block_80181FAC:
    a1 = 1;
    /* fall through to block_80181FB0 */

    /* Seed ctx: [110]=mode flag, [112]=desc[88],
     * [116]=desc[108], [6]=0. Then state check: ctx[40] < 10 ->
     * primary opcode dispatch; >=10 -> secondary class dispatch. */
block_80181FB0:
    cpu->write_half(s0 + 110, (uint16_t)a1);
    v0 = cpu->read_word(s1 + 88);
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 40);
    a0 = cpu->read_half(s0 + 40);
    cpu->write_word(s0 + 112, v0);
    v0 = cpu->read_word(s1 + 108);
    cpu->write_half(s0 + 6, (uint16_t)zero);
    cpu->write_word(s0 + 116, v0);
    v0 = (v1 < (uint32_t)10) ? 1 : 0;  /* 0x80181FD0: 0x2C62000A */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80182A88u);
        goto block_80182A88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80181FDCu);
        goto block_80181FDC;  /* not taken */
    }

    /* Primary dispatch: opcode = *(s0+4); jump table
     * @0x801232B4 (0x80120000+12916) -> jr v0 opcode handler. */
block_80181FDC:
    v0 = v0 + 12916;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80181FEC: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80181FF4: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

    /* Opcode handler entry: a2 = sub-index<<2,
     * table base = ctx[16]; fetch operand via ctx[4]. */
block_80181FF8:
    a2 = t1 << 2;
    v0 = cpu->read_word(s0 + 16);
    v1 = cpu->read_byte(s0 + 4);
    v0 = v0 + a2;
    a0 = cpu->read_half(v0 + 2);
    a1 = cpu->read_half(v0);
    v0 = v1 + 1;
    v1 = v1 & 0x00FF;  /* 0x80182014: 0x306300FF */
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    a0 = a0 - a1;
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    a0 = a0 + 1;
    { int64_t result = (int64_t)(int32_t)a0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a0));
    a1 = cpu->lo;
    psx_muldiv_stall(cpu);
    int cond_a1_ge_0 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018203C: 0x00000000 */
    if (cond_a1_ge_0) {
        psx_check_interrupts_at(cpu, 0x80182044u);
        goto block_80182044;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182040u);
        goto block_80182040;  /* not taken */
    }

block_80182040:
    a1 = a1 + 255;
    /* fall through to block_80182044 */

block_80182044:
    v0 = cpu->read_word(s0 + 16);
    v1 = cpu->read_half(s0 + 10);
    v0 = v0 + a2;
    v1 = v1 | 0x0001;
    a0 = cpu->read_half(v0);
    v0 = -1;
    cpu->write_word(s0 + 48, v0);
    v0 = 1;
    cpu->write_half(s0 + 40, (uint16_t)v0);
    v0 = (int32_t)a1 >> 8;
    cpu->write_half(s0 + 10, (uint16_t)v1);
    a0 = a0 + v0;
    cpu->write_half(s0 + 42, (uint16_t)a0);
    a0 = a0 << 16;
    v0 = (int32_t)a0 >> 16;
    a0 = (uint32_t)a0 >> 31;
    v0 = v0 + a0;
    v0 = (int32_t)v0 >> 1;
    cpu->write_half(s0 + 44, (uint16_t)v0);
block_80182090:
    a3 = cpu->read_word(s1 + 88);
    v0 = 0xC000 << 16;  /* 0xC0000000 */
    v0 = a3 & v0;  /* 0x80182098: 0x00E21024 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x1000 << 16;  /* 0x10000000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801820A4u);
        goto block_801820A4;  /* not taken */
    }

block_801820A4:
    v0 = a3 & v0;  /* 0x801820A4: 0x00E21024 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a3 & 0x8000;  /* 0x801820AC: 0x30E28000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182134u);
        goto block_80182134;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801820B0u);
        goto block_801820B0;  /* not taken */
    }

block_801820B0:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x801820B4: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801820BC: 0x00000000 */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80182118u);
        goto block_80182118;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801820C0u);
        goto block_801820C0;  /* not taken */
    }

block_801820C0:
    v1 = (int32_t)(int16_t)cpu->read_half(t0 + 186);
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 136);
    /* nop */  /* 0x801820C8: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x801820CC: 0x0043102A */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801820D4: 0x00000000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182118u);
        goto block_80182118;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801820D8u);
        goto block_801820D8;  /* not taken */
    }

block_801820D8:;
    int cond_v1_lt_0 = ((int32_t)v1 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801820DC: 0x00000000 */
    if (cond_v1_lt_0) {
        psx_check_interrupts_at(cpu, 0x80182118u);
        goto block_80182118;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801820E0u);
        goto block_801820E0;  /* not taken */
    }

block_801820E0:
    v1 = cpu->read_byte(s0 + 4);
    a0 = cpu->read_word(s0 + 28);
    v0 = v1 + 1;
    a0 = t1 + a0;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x801820F8: 0x306300FF */
    v0 = v0 + v1;
    v1 = cpu->read_byte(a0);
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182108: 0x00000000 */
    v0 = (v0 < v1) ? 1 : 0;  /* 0x8018210C: 0x0043102B */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3;
    if (cond_v0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x801823B8u);
        goto block_801823B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182118u);
        goto block_80182118;  /* not taken */
    }

block_80182118:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018211C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182120: 0x30420010 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018212Cu);
        goto block_8018212C;  /* not taken */
    }

block_8018212C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182134:;
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182138: 0x00000000 */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x80182144u);
        goto block_80182144;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018213Cu);
        goto block_8018213C;  /* not taken */
    }

block_8018213C:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 48, zero);
    psx_check_interrupts_at(cpu, 0x80182544u);
    goto block_80182544;  /* j */

block_80182144:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 116);
    /* nop */  /* 0x80182148: 0x00000000 */
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x80182158u);
        goto block_80182158;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182154u);
        goto block_80182154;  /* not taken */
    }

block_80182154:
    a2 = zero - a2;
    /* fall through to block_80182158 */

block_80182158:
    v0 = ((int32_t)a2 < 1707) ? 1 : 0;  /* 0x80182158: 0x28C206AB */
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182160: 0x28C20201 */
    if (cond_v0_ne_zero_4) {
        psx_check_interrupts_at(cpu, 0x80182170u);
        goto block_80182170;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182164u);
        goto block_80182164;  /* not taken */
    }

block_80182164:
    v0 = 1;
    /* delay slot (always executes) */
    cpu->write_word(s0 + 48, v0);
    psx_check_interrupts_at(cpu, 0x80182544u);
    goto block_80182544;  /* j */

block_80182170:;
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x80182180u);
        goto block_80182180;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182178u);
        goto block_80182178;  /* not taken */
    }

block_80182178:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 48, v0);
    psx_check_interrupts_at(cpu, 0x80182544u);
    goto block_80182544;  /* j */

block_80182180:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 42);
    v1 = cpu->read_half(s0 + 42);
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;
    if (cond_v0_eq_zero_5) {
        psx_check_interrupts_at(cpu, 0x801821A0u);
        goto block_801821A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182190u);
        goto block_80182190;  /* not taken */
    }

block_80182190:
    cpu->write_half(s0 + 42, (uint16_t)v0);
    v0 = v0 << 16;
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018219C: 0x00000000 */
    if (cond_v0_ne_zero_6) {
        psx_check_interrupts_at(cpu, 0x801821BCu);
        goto block_801821BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801821A0u);
        goto block_801821A0;  /* not taken */
    }

block_801821A0:
    v0 = cpu->read_word(s1 + 96);
    /* nop */  /* 0x801821A4: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801821A8: 0x30428000 */
    int cond_v0_ne_zero_7 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3;
    if (cond_v0_ne_zero_7) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801821B4u);
        goto block_801821B4;  /* not taken */
    }

block_801821B4:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 48, v0);
    psx_check_interrupts_at(cpu, 0x80182544u);
    goto block_80182544;  /* j */

block_801821BC:
    a3 = cpu->read_word(t0 + 88);
    a0 = cpu->read_word(s0 + 24);
    int cond_a3_ge_0 = ((int32_t)a3 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 6;
    if (cond_a3_ge_0) {
        psx_check_interrupts_at(cpu, 0x80182258u);
        goto block_80182258;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801821CCu);
        goto block_801821CC;  /* not taken */
    }

block_801821CC:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 188);
    /* nop */  /* 0x801821D0: 0x00000000 */
    int cond_v0_gt_0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v0_gt_0) {
        psx_check_interrupts_at(cpu, 0x80182250u);
        goto block_80182250;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801821DCu);
        goto block_801821DC;  /* not taken */
    }

block_801821DC:
    a2 = t1 & 0xFFFF;  /* 0x801821DC: 0x3126FFFF */
    a1 = cpu->read_byte(s0 + 4);
    a0 = cpu->read_word(s0 + 28);
    v0 = cpu->read_word(s0);
    a3 = a1 + 1;
    a0 = a0 + a2;
    v1 = a1 & 0x00FF;  /* 0x801821F4: 0x30A300FF */
    v0 = v0 + v1;
    cpu->write_byte(s0 + 4, (uint8_t)a3);
    v1 = cpu->read_byte(a0);
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182208: 0x00000000 */
    v0 = (v0 < v1) ? 1 : 0;  /* 0x8018220C: 0x0043102B */
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a1 + 2;
    if (cond_v0_eq_zero_6) {
        psx_check_interrupts_at(cpu, 0x801822C8u);
        goto block_801822C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182218u);
        goto block_80182218;  /* not taken */
    }

block_80182218:
    a0 = cpu->read_word(s0 + 32);
    v1 = a3 & 0x00FF;  /* 0x8018221C: 0x30E300FF */
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    a0 = a0 + a2;
    v0 = v0 + v1;
    v1 = cpu->read_byte(a0);
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182238: 0x00000000 */
    v0 = (v0 < v1) ? 1 : 0;  /* 0x8018223C: 0x0043102B */
    int cond_v0_ne_zero_8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3;
    if (cond_v0_ne_zero_8) {
        psx_check_interrupts_at(cpu, 0x801823B8u);
        goto block_801823B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182248u);
        goto block_80182248;  /* not taken */
    }

block_80182248:
    /* delay slot (always executes) */
    v0 = 4;
    psx_check_interrupts_at(cpu, 0x8018278Cu);
    goto block_8018278C;  /* j */

block_80182250:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 48, v0);
    psx_check_interrupts_at(cpu, 0x80182544u);
    goto block_80182544;  /* j */

block_80182258:
    cpu->write_word(s0 + 48, v0);
    a2 = cpu->read_word(a0 + 72);
    /* nop */  /* 0x80182260: 0x00000000 */
    int cond_a2_eq_zero = (a2 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a3 & a2;  /* 0x80182268: 0x00E61024 */
    if (cond_a2_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801822A4u);
        goto block_801822A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018226Cu);
        goto block_8018226C;  /* not taken */
    }

block_8018226C:;
    int cond_v0_eq_a2 = (v0 == a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182270: 0x00000000 */
    if (cond_v0_eq_a2) {
        psx_check_interrupts_at(cpu, 0x80182500u);
        goto block_80182500;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182274u);
        goto block_80182274;  /* not taken */
    }

block_80182274:
    v1 = cpu->read_word(s0 + 48);
    /* nop */  /* 0x80182278: 0x00000000 */
    v1 = v1 + 1;
    v0 = v1 << 1;
    v0 = v0 + v1;
    v0 = v0 << 2;
    v0 = v0 + a0;
    cpu->write_word(s0 + 48, v1);
    a2 = cpu->read_word(v0);
    /* nop */  /* 0x80182298: 0x00000000 */
    int cond_a2_ne_zero = (a2 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a3 & a2;  /* 0x801822A0: 0x00E61024 */
    if (cond_a2_ne_zero) {
        psx_check_interrupts_at(cpu, 0x8018226Cu);
        goto block_8018226C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801822A4u);
        goto block_801822A4;  /* not taken */
    }

block_801822A4:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x801822A8: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x801822AC: 0x30420010 */
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_7) {
        psx_check_interrupts_at(cpu, 0x801822BCu);
        goto block_801822BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801822B8u);
        goto block_801822B8;  /* not taken */
    }

block_801822B8:
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_801822BC */

block_801822BC:
    v0 = -1;
    /* delay slot (always executes) */
    cpu->write_word(s0 + 48, v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_801822C8:
    v0 = 2;
    cpu->write_half(s0 + 40, (uint16_t)v0);
block_801822D0:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x801822D4: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801822D8: 0x30428000 */
    int cond_v0_ne_zero_9 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801822E0: 0x00000000 */
    if (cond_v0_ne_zero_9) {
        psx_check_interrupts_at(cpu, 0x801822F4u);
        goto block_801822F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801822E4u);
        goto block_801822E4;  /* not taken */
    }

block_801822E4:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 184);
    /* nop */  /* 0x801822E8: 0x00000000 */
    int cond_v0_ne_zero_10 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801822F0: 0x00000000 */
    if (cond_v0_ne_zero_10) {
        psx_check_interrupts_at(cpu, 0x801822F8u);
        goto block_801822F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801822F4u);
        goto block_801822F4;  /* not taken */
    }

block_801822F4:
    cpu->write_half(s0 + 40, (uint16_t)zero);
    /* fall through to block_801822F8 */

block_801822F8:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x801822FC: 0x00000000 */
    v0 = v0 & 0x0800;  /* 0x80182300: 0x30420800 */
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182308: 0x00000000 */
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x80182318u);
        goto block_80182318;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018230Cu);
        goto block_8018230C;  /* not taken */
    }

block_8018230C:
    v0 = cpu->read_half(s0 + 10);
    /* delay slot (always executes) */
    v0 = v0 | 0x0001;
    psx_check_interrupts_at(cpu, 0x80182EE0u);
    goto block_80182EE0;  /* j */

block_80182318:
    v0 = cpu->read_half(s0 + 10);
    /* nop */  /* 0x8018231C: 0x00000000 */
    v0 = v0 & 0xFFFE;  /* 0x80182320: 0x3042FFFE */
    cpu->write_half(s0 + 10, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 118);
    /* nop */  /* 0x8018232C: 0x00000000 */
    v0 = ((int32_t)v0 < 1025) ? 1 : 0;  /* 0x80182330: 0x28420401 */
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182338: 0x00000000 */
    if (cond_v0_eq_zero_9) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018233Cu);
        goto block_8018233C;  /* not taken */
    }

block_8018233C:
    v0 = cpu->read_word(t0 + 100);
    /* nop */  /* 0x80182340: 0x00000000 */
    v0 = v0 & 0x0080;  /* 0x80182344: 0x30420080 */
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018234C: 0x00000000 */
    if (cond_v0_eq_zero_10) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182350u);
        goto block_80182350;  /* not taken */
    }

block_80182350:
    v1 = (int32_t)(int16_t)cpu->read_half(t0 + 186);
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 136);
    /* nop */  /* 0x80182358: 0x00000000 */
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)v0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);
    a3 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = 5;
    int cond_a3_ne_v0 = (a3 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a3 < 5) ? 1 : 0;  /* 0x8018236C: 0x28E20005 */
    if (cond_a3_ne_v0) {
        psx_check_interrupts_at(cpu, 0x801823A8u);
        goto block_801823A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182370u);
        goto block_80182370;  /* not taken */
    }

block_80182370:
    v1 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x80182374: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x80182384: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182390: 0x00000000 */
    v0 = (v0 < (uint32_t)64) ? 1 : 0;  /* 0x80182394: 0x2C420040 */
    int cond_v0_eq_zero_11 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_11) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801823A0u);
        goto block_801823A0;  /* not taken */
    }

block_801823A0:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_801823A8:;
    int cond_v0_eq_zero_12 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801823AC: 0x00000000 */
    if (cond_v0_eq_zero_12) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801823B0u);
        goto block_801823B0;  /* not taken */
    }

block_801823B0:
    /* delay slot (always executes) */
    /* nop */  /* 0x801823B4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80182118u);
    goto block_80182118;  /* j */

block_801823B8:
    cpu->write_half(s0 + 40, (uint16_t)v0);
block_801823BC:
    v1 = cpu->read_word(s1 + 92);
    v0 = 0x0200 << 16;  /* 0x02000000 */
    v0 = v1 & v0;  /* 0x801823C4: 0x00621024 */
    int cond_v0_eq_zero_13 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero_13) {
        psx_check_interrupts_at(cpu, 0x80182474u);
        goto block_80182474;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801823D0u);
        goto block_801823D0;  /* not taken */
    }

block_801823D0:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 204);
    /* nop */  /* 0x801823D4: 0x00000000 */
    v0 = ((int32_t)v0 < 97) ? 1 : 0;  /* 0x801823D8: 0x28420061 */
    int cond_v0_eq_zero_14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x0080;  /* 0x801823E0: 0x30620080 */
    if (cond_v0_eq_zero_14) {
        psx_check_interrupts_at(cpu, 0x80182448u);
        goto block_80182448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801823E4u);
        goto block_801823E4;  /* not taken */
    }

block_801823E4:;
    int cond_v0_eq_zero_15 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801823E8: 0x00000000 */
    if (cond_v0_eq_zero_15) {
        psx_check_interrupts_at(cpu, 0x80182408u);
        goto block_80182408;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801823ECu);
        goto block_801823EC;  /* not taken */
    }

block_801823EC:
    a0 = cpu->read_byte(s0 + 4);
    v1 = cpu->read_word(s0);
    v0 = a0 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = 0x801B << 16;  /* 0x801B0000 */
    /* delay slot (always executes) */
    v0 = v0 + 21160;
    psx_check_interrupts_at(cpu, 0x80182420u);
    goto block_80182420;  /* j */

block_80182408:
    a0 = cpu->read_byte(s0 + 4);
    v1 = cpu->read_word(s0);
    v0 = a0 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = 0x801B << 16;  /* 0x801B0000 */
    v0 = v0 + 21144;
    /* fall through to block_80182420 */

block_80182420:
    v0 = t1 + v0;
    a0 = a0 & 0x00FF;  /* 0x80182424: 0x308400FF */
    v1 = v1 + a0;
    a0 = cpu->read_byte(v0);
    v0 = cpu->read_byte(v1);
    /* nop */  /* 0x80182434: 0x00000000 */
    v0 = (v0 < a0) ? 1 : 0;  /* 0x80182438: 0x0044102B */
    int cond_v0_eq_zero_16 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_eq_zero_16) {
        psx_check_interrupts_at(cpu, 0x80182448u);
        goto block_80182448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182444u);
        goto block_80182444;  /* not taken */
    }

block_80182444:
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182448 */

block_80182448:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x8018244C: 0x00000000 */
    int cond_v0_ne_zero_11 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182454: 0x00000000 */
    if (cond_v0_ne_zero_11) {
        psx_check_interrupts_at(cpu, 0x801824E8u);
        goto block_801824E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182458u);
        goto block_80182458;  /* not taken */
    }

block_80182458:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018245C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182460: 0x30420010 */
    int cond_v0_eq_zero_17 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_17) {
        psx_check_interrupts_at(cpu, 0x801824E8u);
        goto block_801824E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018246Cu);
        goto block_8018246C;  /* not taken */
    }

block_8018246C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801824E8u);
    goto block_801824E8;  /* j */

block_80182474:
    v0 = v1 & v0;  /* 0x80182474: 0x00621024 */
    int cond_v0_ne_zero_12 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018247C: 0x00000000 */
    if (cond_v0_ne_zero_12) {
        psx_check_interrupts_at(cpu, 0x80182458u);
        goto block_80182458;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182480u);
        goto block_80182480;  /* not taken */
    }

block_80182480:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 118);
    /* nop */  /* 0x80182484: 0x00000000 */
    v0 = ((int32_t)v0 < 1024) ? 1 : 0;  /* 0x80182488: 0x28420400 */
    int cond_v0_eq_zero_18 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182490: 0x00000000 */
    if (cond_v0_eq_zero_18) {
        psx_check_interrupts_at(cpu, 0x801824C8u);
        goto block_801824C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182494u);
        goto block_80182494;  /* not taken */
    }

block_80182494:
    v1 = cpu->read_word(t0 + 100);
    /* nop */  /* 0x80182498: 0x00000000 */
    v0 = v1 & 0x0040;  /* 0x8018249C: 0x30620040 */
    int cond_v0_eq_zero_19 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (cond_v0_eq_zero_19) {
        psx_check_interrupts_at(cpu, 0x801824C8u);
        goto block_801824C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801824A8u);
        goto block_801824A8;  /* not taken */
    }

block_801824A8:
    v0 = v1 & v0;  /* 0x801824A8: 0x00621024 */
    int cond_v0_eq_zero_20 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 17408;
    if (cond_v0_eq_zero_20) {
        psx_check_interrupts_at(cpu, 0x801824BCu);
        goto block_801824BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801824B4u);
        goto block_801824B4;  /* not taken */
    }

block_801824B4:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801824E8u);
    goto block_801824E8;  /* j */

block_801824BC:
    v0 = 16384;
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801824E8u);
    goto block_801824E8;  /* j */

block_801824C8:
    v0 = cpu->read_word(t0 + 100);
    v1 = 0x0002 << 16;  /* 0x00020000 */
    v0 = v0 & v1;  /* 0x801824D0: 0x00431024 */
    int cond_v0_eq_zero_21 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_eq_zero_21) {
        psx_check_interrupts_at(cpu, 0x801824E4u);
        goto block_801824E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801824DCu);
        goto block_801824DC;  /* not taken */
    }

block_801824DC:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801824E8u);
    goto block_801824E8;  /* j */

block_801824E4:
    cpu->write_half(s0 + 6, (uint16_t)zero);
    /* fall through to block_801824E8 */

block_801824E8:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 184);
    /* nop */  /* 0x801824EC: 0x00000000 */
    int cond_v0_ne_zero_13 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801824F4: 0x00000000 */
    if (cond_v0_ne_zero_13) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801824F8u);
        goto block_801824F8;  /* not taken */
    }

block_801824F8:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 40, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182500:
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 42);
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 44);
    /* nop */  /* 0x80182508: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x8018250C: 0x0043102A */
    int cond_v0_eq_zero_22 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182514: 0x00000000 */
    if (cond_v0_eq_zero_22) {
        psx_check_interrupts_at(cpu, 0x80182544u);
        goto block_80182544;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182518u);
        goto block_80182518;  /* not taken */
    }

block_80182518:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018251C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182520: 0x30420010 */
    int cond_v0_eq_zero_23 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_23) {
        psx_check_interrupts_at(cpu, 0x80182530u);
        goto block_80182530;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018252Cu);
        goto block_8018252C;  /* not taken */
    }

block_8018252C:
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182530 */

block_80182530:
    v0 = -1;
    cpu->write_word(s0 + 48, v0);
    v0 = 1;
    /* delay slot (always executes) */
    cpu->write_half(s0 + 40, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182544:
    a3 = (int32_t)(int16_t)cpu->read_half(s1 + 118);
    v1 = cpu->read_word(s0 + 20);
    cpu->write_word(s0 + 52, zero);
    v0 = cpu->read_half(v1);
    /* nop */  /* 0x80182554: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)a3) ? 1 : 0;  /* 0x80182558: 0x0047102A */
    int cond_v0_eq_zero_24 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182560: 0x00000000 */
    if (cond_v0_eq_zero_24) {
        psx_check_interrupts_at(cpu, 0x80182590u);
        goto block_80182590;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182564u);
        goto block_80182564;  /* not taken */
    }

block_80182564:
    v0 = cpu->read_word(s0 + 52);
    /* nop */  /* 0x80182568: 0x00000000 */
    v0 = v0 + 1;
    cpu->write_word(s0 + 52, v0);
    v0 = v0 << 1;
    v0 = v0 + v1;
    v0 = cpu->read_half(v0);
    /* nop */  /* 0x80182580: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)a3) ? 1 : 0;  /* 0x80182584: 0x0047102A */
    int cond_v0_ne_zero_14 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018258C: 0x00000000 */
    if (cond_v0_ne_zero_14) {
        psx_check_interrupts_at(cpu, 0x80182564u);
        goto block_80182564;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182590u);
        goto block_80182590;  /* not taken */
    }

block_80182590:
    v1 = cpu->read_word(s0 + 48);
    a0 = cpu->read_word(s0 + 24);
    v0 = v1 << 1;
    v0 = v0 + v1;
    v0 = v0 << 2;
    v1 = cpu->read_word(s0 + 52);
    v0 = v0 + a0;
    v1 = v1 << 1;
    v0 = v0 + v1;
    v0 = cpu->read_half(v0 + 4);
    v1 = cpu->read_word(s1 + 48);
    a0 = (int32_t)(int16_t)cpu->read_half(s0 + 110);
    v0 = v0 + v1;
    int cond_a0_eq_zero = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 36, v0);
    if (cond_a0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80182608u);
        goto block_80182608;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801825CCu);
        goto block_801825CC;  /* not taken */
    }

block_801825CC:
    a0 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x801825D0: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s0;
    v0 = ((int32_t)v0 < 187) ? 1 : 0;  /* 0x801825DC: 0x284200BB */
    int cond_v0_eq_zero_25 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_25) {
        psx_check_interrupts_at(cpu, 0x80182608u);
        goto block_80182608;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801825E8u);
        goto block_801825E8;  /* not taken */
    }

    /* sprintf_core_guest(buf, fmt@0x801232A4, ctx[48],
     * ctx[52]) — format two ctx fields (coords/values). */
block_801825E8:
    a2 = cpu->read_word(s0 + 48);
    a3 = cpu->read_word(s0 + 52);
    ra = 0x801825F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 12900;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_801825F8:
    v1 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x801825FC: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s0 + 120, v1);
    /* fall through to block_80182608 */

block_80182608:
    v1 = 256;
    v0 = cpu->read_half(s0 + 8);
    a1 = cpu->read_byte(s0 + 4);
    a3 = v1 - v0;
    a0 = a1 + 1;
    v0 = cpu->read_word(s0);
    v1 = a1 & 0x00FF;  /* 0x80182620: 0x30A300FF */
    cpu->write_byte(s0 + 4, (uint8_t)a0);
    a0 = a0 & 0x00FF;  /* 0x80182628: 0x308400FF */
    v0 = v0 + v1;
    v1 = a1 + 2;
    a2 = cpu->read_byte(v0);
    v0 = cpu->read_word(s0);
    a1 = a1 + 3;
    cpu->write_byte(s0 + 4, (uint8_t)v1);
    v0 = v0 + a0;
    a0 = cpu->read_byte(v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x80182650: 0x306300FF */
    cpu->write_byte(s0 + 4, (uint8_t)a1);
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    a2 = a2 + a0;
    a2 = a2 + v0;
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)a2; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    int cond_v0_ge_0_3 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182674: 0x00000000 */
    if (cond_v0_ge_0_3) {
        psx_check_interrupts_at(cpu, 0x8018267Cu);
        goto block_8018267C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182678u);
        goto block_80182678;  /* not taken */
    }

block_80182678:
    v0 = v0 + 8191;
    /* fall through to block_8018267C */

block_8018267C:
    v0 = (int32_t)v0 >> 13;
    cpu->write_half(s0 + 42, (uint16_t)v0);
    v0 = 4;
    cpu->write_half(s0 + 108, (uint16_t)zero);
    cpu->write_half(s0 + 40, (uint16_t)v0);
block_80182690:
    v1 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x80182694: 0x00000000 */
    a0 = v1 & 0x8000;  /* 0x80182698: 0x30648000 */
    int cond_a0_eq_zero_2 = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0003 << 16;  /* 0x00030000 */
    if (cond_a0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x801826B0u);
        goto block_801826B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801826A4u);
        goto block_801826A4;  /* not taken */
    }

block_801826A4:
    v0 = v1 & v0;  /* 0x801826A4: 0x00621024 */
    int cond_v0_ne_zero_15 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v0_ne_zero_15) {
        psx_check_interrupts_at(cpu, 0x801826F4u);
        goto block_801826F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801826B0u);
        goto block_801826B0;  /* not taken */
    }

block_801826B0:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 210);
    /* nop */  /* 0x801826B4: 0x00000000 */
    int cond_v0_ne_zero_16 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 6;
    if (cond_v0_ne_zero_16) {
        psx_check_interrupts_at(cpu, 0x80182768u);
        goto block_80182768;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801826C0u);
        goto block_801826C0;  /* not taken */
    }

block_801826C0:
    v0 = cpu->read_word(s0 + 112);
    /* nop */  /* 0x801826C4: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801826C8: 0x30428000 */
    int cond_v0_ne_zero_17 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801826D0: 0x00000000 */
    if (cond_v0_ne_zero_17) {
        psx_check_interrupts_at(cpu, 0x801826DCu);
        goto block_801826DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801826D4u);
        goto block_801826D4;  /* not taken */
    }

block_801826D4:;
    int cond_a0_ne_zero_2 = (a0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801826D8: 0x00000000 */
    if (cond_a0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x801824F8u);
        goto block_801824F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801826DCu);
        goto block_801826DC;  /* not taken */
    }

block_801826DC:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 42);
    v1 = cpu->read_half(s0 + 42);
    int cond_v0_eq_zero_26 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;
    if (cond_v0_eq_zero_26) {
        psx_check_interrupts_at(cpu, 0x80182A6Cu);
        goto block_80182A6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801826ECu);
        goto block_801826EC;  /* not taken */
    }

block_801826EC:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 42, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_801826F4:
    v1 = cpu->read_byte(s0 + 4);
    cpu->write_half(s0 + 40, (uint16_t)v0);
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x80182708: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182714: 0x00000000 */
    v0 = (v0 < (uint32_t)32) ? 1 : 0;  /* 0x80182718: 0x2C420020 */
    int cond_v0_eq_zero_27 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182720: 0x00000000 */
    if (cond_v0_eq_zero_27) {
        psx_check_interrupts_at(cpu, 0x80182730u);
        goto block_80182730;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182724u);
        goto block_80182724;  /* not taken */
    }

block_80182724:
    v0 = cpu->read_half(s0 + 10);
    /* delay slot (always executes) */
    v0 = v0 | 0x0004;
    psx_check_interrupts_at(cpu, 0x8018273Cu);
    goto block_8018273C;  /* j */

block_80182730:
    v0 = cpu->read_half(s0 + 10);
    /* nop */  /* 0x80182734: 0x00000000 */
    v0 = v0 & 0xFFFB;  /* 0x80182738: 0x3042FFFB */
    /* fall through to block_8018273C */

block_8018273C:
    cpu->write_half(s0 + 10, (uint16_t)v0);
block_80182740:
    v0 = cpu->read_word(s1 + 88);
    v1 = 0x0200 << 16;  /* 0x02000000 */
    v0 = v0 & v1;  /* 0x80182748: 0x00431024 */
    int cond_v0_eq_zero_28 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0080 << 16;  /* 0x00800000 */
    if (cond_v0_eq_zero_28) {
        psx_check_interrupts_at(cpu, 0x80182774u);
        goto block_80182774;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182754u);
        goto block_80182754;  /* not taken */
    }

block_80182754:
    v0 = cpu->read_word(s1 + 104);
    /* nop */  /* 0x80182758: 0x00000000 */
    v0 = v0 & v1;  /* 0x8018275C: 0x00431024 */
    int cond_v0_eq_zero_29 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 7;
    if (cond_v0_eq_zero_29) {
        psx_check_interrupts_at(cpu, 0x80182774u);
        goto block_80182774;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182768u);
        goto block_80182768;  /* not taken */
    }

block_80182768:
    cpu->write_half(s0 + 40, (uint16_t)v0);
    /* delay slot (always executes) */
    cpu->write_half(s0 + 42, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182774:
    v1 = cpu->read_word(s1 + 88);
    v0 = 0x0003 << 16;  /* 0x00030000 */
    v0 = v1 & v0;  /* 0x8018277C: 0x00621024 */
    int cond_v0_ne_zero_18 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0004 << 16;  /* 0x00040000 */
    if (cond_v0_ne_zero_18) {
        psx_check_interrupts_at(cpu, 0x80182794u);
        goto block_80182794;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182788u);
        goto block_80182788;  /* not taken */
    }

block_80182788:
    v0 = 4;
    /* fall through to block_8018278C */

block_8018278C:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 48, v0);
    psx_check_interrupts_at(cpu, 0x80182544u);
    goto block_80182544;  /* j */

block_80182794:
    v0 = v1 & v0;  /* 0x80182794: 0x00621024 */
    int cond_v0_ne_zero_19 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018279C: 0x00000000 */
    if (cond_v0_ne_zero_19) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801827A0u);
        goto block_801827A0;  /* not taken */
    }

block_801827A0:
    a0 = (int32_t)(int16_t)cpu->read_half(s1 + 192);
    /* nop */  /* 0x801827A4: 0x00000000 */
    int cond_a0_lt_0 = ((int32_t)a0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801827AC: 0x00000000 */
    if (cond_a0_lt_0) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801827B0u);
        goto block_801827B0;  /* not taken */
    }

block_801827B0:;
    int cond_a0_ne_zero_3 = (a0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801827B4: 0x00000000 */
    if (cond_a0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x80182814u);
        goto block_80182814;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801827B8u);
        goto block_801827B8;  /* not taken */
    }

block_801827B8:
    v1 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x801827BC: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x801827CC: 0x306300FF */
    v0 = v0 + v1;
    a3 = cpu->read_byte(v0);
    /* nop */  /* 0x801827D8: 0x00000000 */
    v0 = ((int32_t)a3 < 48) ? 1 : 0;  /* 0x801827DC: 0x28E20030 */
    int cond_v0_ne_zero_20 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_ne_zero_20) {
        psx_check_interrupts_at(cpu, 0x80182D94u);
        goto block_80182D94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801827E8u);
        goto block_801827E8;  /* not taken */
    }

block_801827E8:
    v0 = ((int32_t)a3 < 96) ? 1 : 0;  /* 0x801827E8: 0x28E20060 */
    int cond_v0_ne_zero_21 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8000;
    if (cond_v0_ne_zero_21) {
        psx_check_interrupts_at(cpu, 0x80182874u);
        goto block_80182874;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801827F4u);
        goto block_801827F4;  /* not taken */
    }

block_801827F4:
    v0 = ((int32_t)a3 < 144) ? 1 : 0;  /* 0x801827F4: 0x28E20090 */
    int cond_v0_ne_zero_22 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4096;
    if (cond_v0_ne_zero_22) {
        psx_check_interrupts_at(cpu, 0x80182B6Cu);
        goto block_80182B6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182800u);
        goto block_80182800;  /* not taken */
    }

block_80182800:
    v0 = ((int32_t)a3 < 192) ? 1 : 0;  /* 0x80182800: 0x28E200C0 */
    int cond_v0_eq_zero_30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_30) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018280Cu);
        goto block_8018280C;  /* not taken */
    }

block_8018280C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182814:
    v0 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x80182818: 0x00000000 */
    v1 = v0 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v1);
    v1 = cpu->read_word(s0);
    v0 = v0 & 0x00FF;  /* 0x80182828: 0x304200FF */
    v1 = v1 + v0;
    v0 = cpu->read_half(s0 + 8);
    a3 = cpu->read_byte(v1);
    v0 = v0 + 32;
    v0 = (int32_t)v0 >> 1;
    v0 = ((int32_t)a3 < (int32_t)v0) ? 1 : 0;  /* 0x80182840: 0x00E2102A */
    int cond_v0_eq_zero_31 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182848: 0x00000000 */
    if (cond_v0_eq_zero_31) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018284Cu);
        goto block_8018284C;  /* not taken */
    }

block_8018284C:
    v0 = cpu->read_half(s0 + 10);
    /* nop */  /* 0x80182850: 0x00000000 */
    v0 = v0 & 0x0004;  /* 0x80182854: 0x30420004 */
    int cond_v0_eq_zero_32 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 64;
    if (cond_v0_eq_zero_32) {
        psx_check_interrupts_at(cpu, 0x80182868u);
        goto block_80182868;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182860u);
        goto block_80182860;  /* not taken */
    }

block_80182860:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182868:
    v0 = a3 & 0x0001;  /* 0x80182868: 0x30E20001 */
    int cond_v0_eq_zero_33 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8000;
    if (cond_v0_eq_zero_33) {
        psx_check_interrupts_at(cpu, 0x80182EA4u);
        goto block_80182EA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182874u);
        goto block_80182874;  /* not taken */
    }

block_80182874:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_8018287C:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 210);
    /* nop */  /* 0x80182880: 0x00000000 */
    int cond_v0_ne_zero_23 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_ne_zero_23) {
        psx_check_interrupts_at(cpu, 0x80182894u);
        goto block_80182894;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018288Cu);
        goto block_8018288C;  /* not taken */
    }

block_8018288C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801824F8u);
    goto block_801824F8;  /* j */

block_80182894:
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 42);
    /* nop */  /* 0x80182898: 0x00000000 */
    int cond_v1_eq_zero = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801828A0: 0x00000000 */
    if (cond_v1_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801828D0u);
        goto block_801828D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801828A4u);
        goto block_801828A4;  /* not taken */
    }

block_801828A4:;
    int cond_v1_gt_0 = ((int32_t)v1 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v1_gt_0) {
        psx_check_interrupts_at(cpu, 0x801828C0u);
        goto block_801828C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801828ACu);
        goto block_801828AC;  /* not taken */
    }

block_801828AC:
    v0 = -1;
    int cond_v1_eq_v0_2 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801828B4: 0x00000000 */
    if (cond_v1_eq_v0_2) {
        psx_check_interrupts_at(cpu, 0x80182944u);
        goto block_80182944;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801828B8u);
        goto block_801828B8;  /* not taken */
    }

block_801828B8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801828BC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_801828C0:;
    int cond_v1_eq_v0_3 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801828C4: 0x00000000 */
    if (cond_v1_eq_v0_3) {
        psx_check_interrupts_at(cpu, 0x80182908u);
        goto block_80182908;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801828C8u);
        goto block_801828C8;  /* not taken */
    }

block_801828C8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801828CC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_801828D0:
    v1 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x801828D4: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x801828E4: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x801828F0: 0x00000000 */
    v0 = (v0 < (uint32_t)64) ? 1 : 0;  /* 0x801828F4: 0x2C420040 */
    int cond_v0_ne_zero_24 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = -1;
    if (cond_v0_ne_zero_24) {
        psx_check_interrupts_at(cpu, 0x80182904u);
        goto block_80182904;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182900u);
        goto block_80182900;  /* not taken */
    }

block_80182900:
    v0 = 1;
    /* fall through to block_80182904 */

block_80182904:
    cpu->write_half(s0 + 42, (uint16_t)v0);
    /* fall through to block_80182908 */

block_80182908:
    a0 = cpu->read_byte(s0 + 4);
    v1 = cpu->read_word(s0);
    v0 = a0 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = 0x801B << 16;  /* 0x801B0000 */
    v0 = v0 + 21176;
    v0 = t1 + v0;
    a0 = a0 & 0x00FF;  /* 0x80182924: 0x308400FF */
    v1 = v1 + a0;
    v0 = cpu->read_byte(v0);
    v1 = cpu->read_byte(v1);
    /* nop */  /* 0x80182934: 0x00000000 */
    v0 = (v0 < v1) ? 1 : 0;  /* 0x80182938: 0x0043102B */
    int cond_v0_ne_zero_25 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182940: 0x00000000 */
    if (cond_v0_ne_zero_25) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182944u);
        goto block_80182944;  /* not taken */
    }

block_80182944:
    v0 = cpu->read_half(s1 + 210);
    /* nop */  /* 0x80182948: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x8018294C: 0x30420010 */
    int cond_v0_eq_zero_34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8400;
    if (cond_v0_eq_zero_34) {
        psx_check_interrupts_at(cpu, 0x80182E98u);
        goto block_80182E98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182958u);
        goto block_80182958;  /* not taken */
    }

block_80182958:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182960:
    v0 = cpu->read_word(s1 + 88);
    v1 = 0x0200 << 16;  /* 0x02000000 */
    v0 = v0 & v1;  /* 0x80182968: 0x00431024 */
    int cond_v0_eq_zero_35 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0400 << 16;  /* 0x04000000 */
    if (cond_v0_eq_zero_35) {
        psx_check_interrupts_at(cpu, 0x801824F8u);
        goto block_801824F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182974u);
        goto block_80182974;  /* not taken */
    }

block_80182974:
    v0 = cpu->read_word(s1 + 92);
    /* nop */  /* 0x80182978: 0x00000000 */
    v0 = v0 & v1;  /* 0x8018297C: 0x00431024 */
    int cond_v0_eq_zero_36 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182984: 0x00000000 */
    if (cond_v0_eq_zero_36) {
        psx_check_interrupts_at(cpu, 0x8018298Cu);
        goto block_8018298C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182988u);
        goto block_80182988;  /* not taken */
    }

block_80182988:
    cpu->write_half(s0 + 42, (uint16_t)zero);
    /* fall through to block_8018298C */

block_8018298C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 42);
    /* nop */  /* 0x80182990: 0x00000000 */
    int cond_v0_ne_zero_26 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182998: 0x00000000 */
    if (cond_v0_ne_zero_26) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018299Cu);
        goto block_8018299C;  /* not taken */
    }

block_8018299C:
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 204);
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 134);
    /* nop */  /* 0x801829A4: 0x00000000 */
    v0 = ((int32_t)v0 < (int32_t)v1) ? 1 : 0;  /* 0x801829A8: 0x0043102A */
    int cond_v0_ne_zero_27 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (cond_v0_ne_zero_27) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801829B4u);
        goto block_801829B4;  /* not taken */
    }

block_801829B4:
    v0 = v0 + 21128;
    v0 = t1 + v0;
    a1 = cpu->read_byte(s0 + 4);
    v1 = cpu->read_word(s0);
    a2 = a1 + 1;
    a0 = a1 & 0x00FF;  /* 0x801829C8: 0x30A400FF */
    v1 = v1 + a0;
    cpu->write_byte(s0 + 4, (uint8_t)a2);
    a0 = cpu->read_byte(v0);
    v0 = cpu->read_byte(v1);
    /* nop */  /* 0x801829DC: 0x00000000 */
    v0 = (v0 < a0) ? 1 : 0;  /* 0x801829E0: 0x0044102B */
    int cond_v0_eq_zero_37 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = -1;
    if (cond_v0_eq_zero_37) {
        psx_check_interrupts_at(cpu, 0x80182A50u);
        goto block_80182A50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801829ECu);
        goto block_801829EC;  /* not taken */
    }

block_801829EC:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x801829F0: 0x00000000 */
    int cond_v0_ge_0_4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a1 + 2;
    if (cond_v0_ge_0_4) {
        psx_check_interrupts_at(cpu, 0x80182A18u);
        goto block_80182A18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801829FCu);
        goto block_801829FC;  /* not taken */
    }

block_801829FC:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 134);
    a0 = (int32_t)(int16_t)cpu->read_half(t0 + 186);
    v1 = v0 << 2;
    v1 = v1 + v0;
    v1 = ((int32_t)v1 < (int32_t)a0) ? 1 : 0;  /* 0x80182A0C: 0x0064182A */
    int cond_v1_eq_zero_2 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a1 + 2;
    if (cond_v1_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182A4Cu);
        goto block_80182A4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182A18u);
        goto block_80182A18;  /* not taken */
    }

block_80182A18:
    a0 = 0x801B << 16;  /* 0x801B0000 */
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = a2 & 0x00FF;  /* 0x80182A24: 0x30C300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    a0 = a0 + 21360;
    v0 = v0 & 0x0007;  /* 0x80182A34: 0x30420007 */
    v0 = v0 << 1;
    v0 = v0 + a0;
    v0 = cpu->read_half(v0);
    cpu->write_half(s0 + 40, (uint16_t)zero);
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182A4C */

block_80182A4C:
    v0 = -1;
    /* fall through to block_80182A50 */

block_80182A50:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 42, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182A58:
    v0 = cpu->read_word(s1 + 96);
    /* nop */  /* 0x80182A5C: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x80182A60: 0x30428000 */
    int cond_v0_ne_zero_28 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182A68: 0x00000000 */
    if (cond_v0_ne_zero_28) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182A6Cu);
        goto block_80182A6C;  /* not taken */
    }

    /* script_op_dispatch (0x80184F28) — nested script-op
     * handler for extended opcodes. */
block_80182A6C:
    a0 = s1;
    a1 = s3;
    a2 = s0;
    ra = 0x80182A80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x80184F28u);
    cpu->pc = 0x80184F28u; return;  /* CPS jal -> 0x80184F28 */

block_80182A80:
    /* delay slot (always executes) */
    /* nop */  /* 0x80182A84: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

    /* Secondary dispatch (ctx[40] >= 10): class =
     * (a0 & 0x47) - 64; <5 -> jump table @0x801232DC
     * (0x80120000+12956); else bail to 0x80182EE4. */
block_80182A88:
    v0 = a0 & 0x0047;  /* 0x80182A88: 0x30820047 */
    v0 = v0 + -64;
    v0 = v0 << 16;
    v1 = (int32_t)v0 >> 16;
    v0 = (v1 < (uint32_t)5) ? 1 : 0;  /* 0x80182A98: 0x2C620005 */
    int cond_v0_eq_zero_38 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_38) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182AA4u);
        goto block_80182AA4;  /* not taken */
    }

    /* Secondary jr v0 — dispatch class handler. */
block_80182AA4:
    v0 = v0 + 12956;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80182AB4: 0x00000000 */
    uint32_t jr_target_2 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182ABC: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr v0 */

block_80182AC0:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182AC4: 0x00000000 */
    v0 = v0 | 0x0018;
    cpu->write_half(s0 + 40, (uint16_t)v0);
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182AD4: 0x00000000 */
    int cond_v0_ge_0_5 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182ADC: 0x00000000 */
    if (cond_v0_ge_0_5) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182AE0u);
        goto block_80182AE0;  /* not taken */
    }

block_80182AE0:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 118);
    /* nop */  /* 0x80182AE4: 0x00000000 */
    v0 = ((int32_t)v0 < 1024) ? 1 : 0;  /* 0x80182AE8: 0x28420400 */
    int cond_v0_eq_zero_39 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182AF0: 0x00000000 */
    if (cond_v0_eq_zero_39) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182AF4u);
        goto block_80182AF4;  /* not taken */
    }

block_80182AF4:
    v1 = (int32_t)(int16_t)cpu->read_half(t0 + 186);
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 136);
    /* nop */  /* 0x80182AFC: 0x00000000 */
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)v0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);
    a3 = cpu->lo;
    psx_muldiv_stall(cpu);
    /* nop */  /* 0x80182B08: 0x00000000 */
    v0 = ((int32_t)a3 < 12) ? 1 : 0;  /* 0x80182B0C: 0x28E2000C */
    int cond_v0_ne_zero_29 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B14: 0x00000000 */
    if (cond_v0_ne_zero_29) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B18u);
        goto block_80182B18;  /* not taken */
    }

block_80182B18:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 188);
    /* nop */  /* 0x80182B1C: 0x00000000 */
    int cond_v0_gt_0_2 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B24: 0x00000000 */
    if (cond_v0_gt_0_2) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B28u);
        goto block_80182B28;  /* not taken */
    }

block_80182B28:
    v0 = cpu->read_word(t0 + 100);
    /* nop */  /* 0x80182B2C: 0x00000000 */
    v0 = v0 & 0x0040;  /* 0x80182B30: 0x30420040 */
    int cond_v0_eq_zero_40 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B38: 0x00000000 */
    if (cond_v0_eq_zero_40) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B3Cu);
        goto block_80182B3C;  /* not taken */
    }

block_80182B3C:
    v1 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x80182B40: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x80182B50: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182B5C: 0x00000000 */
    v0 = v0 & 0x0003;  /* 0x80182B60: 0x30420003 */
    int cond_v0_ne_zero_30 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4096;
    if (cond_v0_ne_zero_30) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B6Cu);
        goto block_80182B6C;  /* not taken */
    }

block_80182B6C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182B74:
    v0 = cpu->read_word(s1 + 92);
    v1 = 0x4000 << 16;  /* 0x40000000 */
    v0 = v0 & v1;  /* 0x80182B7C: 0x00431024 */
    int cond_v0_eq_zero_41 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B84: 0x00000000 */
    if (cond_v0_eq_zero_41) {
        psx_check_interrupts_at(cpu, 0x80182BCCu);
        goto block_80182BCC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B88u);
        goto block_80182B88;  /* not taken */
    }

block_80182B88:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 204);
    /* nop */  /* 0x80182B8C: 0x00000000 */
    v0 = ((int32_t)v0 < 97) ? 1 : 0;  /* 0x80182B90: 0x28420061 */
    int cond_v0_eq_zero_42 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B98: 0x00000000 */
    if (cond_v0_eq_zero_42) {
        psx_check_interrupts_at(cpu, 0x80182BCCu);
        goto block_80182BCC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B9Cu);
        goto block_80182B9C;  /* not taken */
    }

block_80182B9C:
    v1 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x80182BA0: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x80182BB0: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182BBC: 0x00000000 */
    v0 = (v0 < (uint32_t)21) ? 1 : 0;  /* 0x80182BC0: 0x2C420015 */
    int cond_v0_ne_zero_31 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_ne_zero_31) {
        psx_check_interrupts_at(cpu, 0x80182D94u);
        goto block_80182D94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182BCCu);
        goto block_80182BCC;  /* not taken */
    }

block_80182BCC:
    v0 = cpu->read_half(s0 + 10);
    /* nop */  /* 0x80182BD0: 0x00000000 */
    v0 = v0 | 0x0001;
    cpu->write_half(s0 + 10, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 116);
    /* nop */  /* 0x80182BE0: 0x00000000 */
    int cond_v0_ge_0_6 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0_6) {
        psx_check_interrupts_at(cpu, 0x80182BF0u);
        goto block_80182BF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182BECu);
        goto block_80182BEC;  /* not taken */
    }

block_80182BEC:
    a2 = zero - a2;
    /* fall through to block_80182BF0 */

block_80182BF0:
    v0 = ((int32_t)a2 < 1707) ? 1 : 0;  /* 0x80182BF0: 0x28C206AB */
    int cond_v0_eq_zero_43 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182BF8: 0x28C20201 */
    if (cond_v0_eq_zero_43) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182BFCu);
        goto block_80182BFC;  /* not taken */
    }

block_80182BFC:;
    int cond_v0_eq_zero_44 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xC000;
    if (cond_v0_eq_zero_44) {
        psx_check_interrupts_at(cpu, 0x80182DF0u);
        goto block_80182DF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C04u);
        goto block_80182C04;  /* not taken */
    }

block_80182C04:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182C08: 0x00000000 */
    int cond_v0_ge_0_7 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (cond_v0_ge_0_7) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C14u);
        goto block_80182C14;  /* not taken */
    }

block_80182C14:
    v1 = cpu->read_word(s1 + 92);
    /* nop */  /* 0x80182C18: 0x00000000 */
    v0 = v1 & v0;  /* 0x80182C1C: 0x00621024 */
    int cond_v0_eq_zero_45 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero_45) {
        psx_check_interrupts_at(cpu, 0x80182C40u);
        goto block_80182C40;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* not taken */
    }

block_80182C28:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182C2C: 0x00000000 */
    int cond_v0_eq_zero_46 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (cond_v0_eq_zero_46) {
        psx_check_interrupts_at(cpu, 0x80182118u);
        goto block_80182118;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C38u);
        goto block_80182C38;  /* not taken */
    }

block_80182C38:
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C3C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182C40:
    v0 = v1 & v0;  /* 0x80182C40: 0x00621024 */
    int cond_v0_ne_zero_32 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C48: 0x00000000 */
    if (cond_v0_ne_zero_32) {
        psx_check_interrupts_at(cpu, 0x80182118u);
        goto block_80182118;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C4Cu);
        goto block_80182C4C;  /* not taken */
    }

block_80182C4C:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 118);
    /* nop */  /* 0x80182C50: 0x00000000 */
    v0 = ((int32_t)v0 < 1024) ? 1 : 0;  /* 0x80182C54: 0x28420400 */
    int cond_v0_eq_zero_47 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C5C: 0x00000000 */
    if (cond_v0_eq_zero_47) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C60u);
        goto block_80182C60;  /* not taken */
    }

block_80182C60:
    a0 = cpu->read_word(t0 + 100);
    /* nop */  /* 0x80182C64: 0x00000000 */
    v0 = a0 & 0x0040;  /* 0x80182C68: 0x30820040 */
    int cond_v0_ne_zero_33 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (cond_v0_ne_zero_33) {
        psx_check_interrupts_at(cpu, 0x80182C84u);
        goto block_80182C84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C74u);
        goto block_80182C74;  /* not taken */
    }

block_80182C74:
    v1 = a0 & 0x0180;  /* 0x80182C74: 0x30830180 */
    v0 = 256;
    int cond_v1_ne_v0_2 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (cond_v1_ne_v0_2) {
        psx_check_interrupts_at(cpu, 0x80182C98u);
        goto block_80182C98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C84u);
        goto block_80182C84;  /* not taken */
    }

block_80182C84:
    v0 = a0 & v0;  /* 0x80182C84: 0x00821024 */
    int cond_v0_ne_zero_34 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 17408;
    if (cond_v0_ne_zero_34) {
        psx_check_interrupts_at(cpu, 0x80182E9Cu);
        goto block_80182E9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C90u);
        goto block_80182C90;  /* not taken */
    }

block_80182C90:
    /* delay slot (always executes) */
    v0 = 16384;
    psx_check_interrupts_at(cpu, 0x80182EA8u);
    goto block_80182EA8;  /* j */

block_80182C98:
    v0 = a0 & v0;  /* 0x80182C98: 0x00821024 */
    int cond_v0_ne_zero_35 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_ne_zero_35) {
        psx_check_interrupts_at(cpu, 0x80182EC4u);
        goto block_80182EC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182CA4u);
        goto block_80182CA4;  /* not taken */
    }

block_80182CA4:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182CAC:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182CB0: 0x00000000 */
    int cond_v0_ge_0_8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;
    if (cond_v0_ge_0_8) {
        psx_check_interrupts_at(cpu, 0x80182CE8u);
        goto block_80182CE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182CBCu);
        goto block_80182CBC;  /* not taken */
    }

block_80182CBC:
    cpu->write_half(s0 + 42, (uint16_t)v0);
    v0 = cpu->read_word(s1 + 92);
    v1 = 0x6000 << 16;  /* 0x60000000 */
    v0 = v0 & v1;  /* 0x80182CC8: 0x00431024 */
    int cond_v0_eq_zero_48 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182CD0: 0x00000000 */
    if (cond_v0_eq_zero_48) {
        psx_check_interrupts_at(cpu, 0x80182D24u);
        goto block_80182D24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182CD4u);
        goto block_80182CD4;  /* not taken */
    }

block_80182CD4:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182CD8: 0x00000000 */
    v0 = v0 | 0x0008;
    /* delay slot (always executes) */
    cpu->write_half(s0 + 40, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182D24u);
    goto block_80182D24;  /* j */

block_80182CE8:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 42);
    v1 = cpu->read_half(s0 + 42);
    int cond_v0_gt_0_3 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;
    if (cond_v0_gt_0_3) {
        psx_check_interrupts_at(cpu, 0x80182D0Cu);
        goto block_80182D0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182CF8u);
        goto block_80182CF8;  /* not taken */
    }

block_80182CF8:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182CFC: 0x00000000 */
    v0 = v0 & 0xFFF7;  /* 0x80182D00: 0x3042FFF7 */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 40, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182D24u);
    goto block_80182D24;  /* j */

block_80182D0C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 42, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182D24u);
    goto block_80182D24;  /* j */

block_80182D14:
    /* delay slot (always executes) */
    v0 = 4096;
    psx_check_interrupts_at(cpu, 0x80182D20u);
    goto block_80182D20;  /* j */

block_80182D1C:
    v0 = 16384;
    /* fall through to block_80182D20 */

block_80182D20:
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182D24 */

block_80182D24:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182D28: 0x00000000 */
    v0 = (uint32_t)v0 >> 4;
    v0 = v0 & 0x0001;  /* 0x80182D30: 0x30420001 */
    int cond_v0_eq_zero_49 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero_49) {
        psx_check_interrupts_at(cpu, 0x80182D9Cu);
        goto block_80182D9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182D3Cu);
        goto block_80182D3C;  /* not taken */
    }

block_80182D3C:
    v0 = cpu->read_word(s1 + 92);
    /* nop */  /* 0x80182D40: 0x00000000 */
    v0 = v0 & v1;  /* 0x80182D44: 0x00431024 */
    int cond_v0_eq_zero_50 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (cond_v0_eq_zero_50) {
        psx_check_interrupts_at(cpu, 0x80182D9Cu);
        goto block_80182D9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182D50u);
        goto block_80182D50;  /* not taken */
    }

block_80182D50:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 204);
    /* nop */  /* 0x80182D54: 0x00000000 */
    v0 = ((int32_t)v0 < 97) ? 1 : 0;  /* 0x80182D58: 0x28420061 */
    int cond_v0_eq_zero_51 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (cond_v0_eq_zero_51) {
        psx_check_interrupts_at(cpu, 0x80182D9Cu);
        goto block_80182D9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182D64u);
        goto block_80182D64;  /* not taken */
    }

block_80182D64:
    v1 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x80182D68: 0x00000000 */
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x80182D78: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x80182D84: 0x00000000 */
    v0 = (v0 < (uint32_t)21) ? 1 : 0;  /* 0x80182D88: 0x2C420015 */
    int cond_v0_eq_zero_52 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_eq_zero_52) {
        psx_check_interrupts_at(cpu, 0x80182D9Cu);
        goto block_80182D9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182D94u);
        goto block_80182D94;  /* not taken */
    }

block_80182D94:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182D9C:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182DA0: 0x00000000 */
    v0 = (uint32_t)v0 >> 3;
    v0 = v0 & 0x0001;  /* 0x80182DA8: 0x30420001 */
    int cond_v0_eq_zero_53 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (cond_v0_eq_zero_53) {
        psx_check_interrupts_at(cpu, 0x80182ED4u);
        goto block_80182ED4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DB4u);
        goto block_80182DB4;  /* not taken */
    }

block_80182DB4:
    v0 = cpu->read_half(s0 + 10);
    /* nop */  /* 0x80182DB8: 0x00000000 */
    v0 = v0 | 0x0001;
    cpu->write_half(s0 + 10, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 116);
    /* nop */  /* 0x80182DC8: 0x00000000 */
    int cond_v0_ge_0_9 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0_9) {
        psx_check_interrupts_at(cpu, 0x80182DD8u);
        goto block_80182DD8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DD4u);
        goto block_80182DD4;  /* not taken */
    }

block_80182DD4:
    a2 = zero - a2;
    /* fall through to block_80182DD8 */

block_80182DD8:
    v0 = ((int32_t)a2 < 1707) ? 1 : 0;  /* 0x80182DD8: 0x28C206AB */
    int cond_v0_eq_zero_54 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (cond_v0_eq_zero_54) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:;
    int cond_v0_ne_zero_36 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (cond_v0_ne_zero_36) {
        psx_check_interrupts_at(cpu, 0x80182DF8u);
        goto block_80182DF8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* not taken */
    }

block_80182DEC:
    v0 = 0xC000;
    /* fall through to block_80182DF0 */

block_80182DF0:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182DF8:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182DFC: 0x00000000 */
    int cond_v0_ge_0_10 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (cond_v0_ge_0_10) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E08u);
        goto block_80182E08;  /* not taken */
    }

block_80182E08:
    v1 = cpu->read_word(s1 + 92);
    /* nop */  /* 0x80182E0C: 0x00000000 */
    v0 = v1 & v0;  /* 0x80182E10: 0x00621024 */
    int cond_v0_ne_zero_37 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_ne_zero_37) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int cond_v0_ne_zero_38 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (cond_v0_ne_zero_38) {
        psx_check_interrupts_at(cpu, 0x80182118u);
        goto block_80182118;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E28u);
        goto block_80182E28;  /* not taken */
    }

block_80182E28:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 118);
    /* nop */  /* 0x80182E2C: 0x00000000 */
    v0 = ((int32_t)v0 < 1024) ? 1 : 0;  /* 0x80182E30: 0x28420400 */
    int cond_v0_eq_zero_55 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (cond_v0_eq_zero_55) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E3Cu);
        goto block_80182E3C;  /* not taken */
    }

block_80182E3C:
    a0 = cpu->read_word(t0 + 100);
    /* nop */  /* 0x80182E40: 0x00000000 */
    v0 = a0 & 0x0040;  /* 0x80182E44: 0x30820040 */
    int cond_v0_ne_zero_39 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (cond_v0_ne_zero_39) {
        psx_check_interrupts_at(cpu, 0x80182E84u);
        goto block_80182E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E50u);
        goto block_80182E50;  /* not taken */
    }

block_80182E50:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182E54: 0x00000000 */
    v0 = v0 & 0x0047;  /* 0x80182E58: 0x30420047 */
    int cond_v0_eq_v1 = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a0 & 0x0180;  /* 0x80182E60: 0x30830180 */
    if (cond_v0_eq_v1) {
        psx_check_interrupts_at(cpu, 0x80182E84u);
        goto block_80182E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E64u);
        goto block_80182E64;  /* not taken */
    }

block_80182E64:
    v0 = 256;
    int cond_v1_ne_v0_3 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (cond_v1_ne_v0_3) {
        psx_check_interrupts_at(cpu, 0x80182EB0u);
        goto block_80182EB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E70u);
        goto block_80182E70;  /* not taken */
    }

block_80182E70:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x80182E74: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182E78: 0x30420010 */
    int cond_v0_eq_zero_56 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (cond_v0_eq_zero_56) {
        psx_check_interrupts_at(cpu, 0x80182EB0u);
        goto block_80182EB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E84u);
        goto block_80182E84;  /* not taken */
    }

block_80182E84:
    v0 = cpu->read_word(t0 + 100);
    v1 = 0x0002 << 16;  /* 0x00020000 */
    v0 = v0 & v1;  /* 0x80182E8C: 0x00431024 */
    int cond_v0_eq_zero_57 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_57) {
        psx_check_interrupts_at(cpu, 0x80182EA8u);
        goto block_80182EA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E98u);
        goto block_80182E98;  /* not taken */
    }

block_80182E98:
    v0 = 17408;
    /* fall through to block_80182E9C */

block_80182E9C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EA4:
    v0 = 16384;
    /* fall through to block_80182EA8 */

block_80182EA8:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EB0:
    v0 = cpu->read_word(t0 + 100);
    v1 = 0x0002 << 16;  /* 0x00020000 */
    v0 = v0 & v1;  /* 0x80182EB8: 0x00431024 */
    int cond_v0_eq_zero_58 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_eq_zero_58) {
        psx_check_interrupts_at(cpu, 0x80182ECCu);
        goto block_80182ECC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182EC4u);
        goto block_80182EC4;  /* not taken */
    }

block_80182EC4:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182ECC:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182ED4:
    v0 = cpu->read_half(s0 + 10);
    /* nop */  /* 0x80182ED8: 0x00000000 */
    v0 = v0 & 0xFFFE;  /* 0x80182EDC: 0x3042FFFE */
    /* fall through to block_80182EE0 */

block_80182EE0:
    cpu->write_half(s0 + 10, (uint16_t)v0);
    /* fall through to block_80182EE4 */

    /* Script-end/teardown: set ctx[6] flag bits (0x8 if 0x400
     * clear; 0x800 if ctx[10]&1) then jr $ra epilogue. */
block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero_59 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero_59) {
        psx_check_interrupts_at(cpu, 0x80182EFCu);
        goto block_80182EFC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182EF8u);
        goto block_80182EF8;  /* not taken */
    }

block_80182EF8:
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182EFC */

block_80182EFC:
    v0 = cpu->read_half(s0 + 10);
    /* nop */  /* 0x80182F00: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x80182F04: 0x30420001 */
    int cond_v0_eq_zero_60 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_60) {
        psx_check_interrupts_at(cpu, 0x80182F20u);
        goto block_80182F20;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182F10u);
        goto block_80182F10;  /* not taken */
    }

block_80182F10:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182F14: 0x00000000 */
    v0 = v0 | 0x0800;
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182F20 */

block_80182F20:
    ra = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target_3 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target_3);
    cpu->pc = jr_target_3; return;  /* CPS: jr $ra */
}
