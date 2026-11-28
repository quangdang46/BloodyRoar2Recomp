/* recon_8017b.c — clean-room reconstruction of guest range
 * 0x80178000-0x8017FFFF (worktree orch-w5-e05; w5e02 owns 0x80170-0x80177).
 *
 * Faithful clean-C ports of functions still covered by cps_ transliterations
 * in game/cps_clean.c. Each port takes over its guest entry addr via
 * game/renames.map + gen_clean_table.py; the cps_<addr> def remains as
 * unreachable shadow code (expected — same class as the shadowed delegates
 * in proof/generated_zero_audit.md).
 *
 * Leaf ports keep the guest `jr $ra` interrupt check
 * (psx_check_interrupts_at with resume = $ra) exactly like the cps_ body
 * they replace; the CLEAN_ENT_CPS wrapper converts the untouched cpu->pc
 * back to $ra on return.
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

/* ======== part_field4c_clear (guest 0x80179A60) ========
 * br2eu_func_80179A60 — single block, 12 bytes:
 *   sw zero, 0x4C(a1)        0x80179A60: 0xACA0004C
 *   jr ra  (delay: nop)      0x80179A64/68
 * Clears the +0x4C word of the a1 record and returns. */
void part_field4c_clear(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;
    cpu->write_word(cpu->gpr[5] + 76u, cpu->gpr[0]);  /* sw zero,0x4C(a1) */
    psx_check_interrupts_at(cpu, cpu->gpr[31]);        /* jr $ra resume check */
}

/* ======== desc_u16pair_dispatch (guest 0x8017B8D4) ========
 * Descriptor-script op tail shared by the 0x8017B8xx walkers: v1 = op
 * index (0-10, callers mask the opcode nibble); a2 = source record cursor,
 * a0 = destination record. Jump table @0x8012028C[v1] selects one of the
 * u16-pair field writes into a0+0x8A..0x98 (raw copy from a2+2/+4, sum with
 * a0+0x70/+0x74, scaled (v1<<7)/a2[4], sign-extended adds with clamps
 * +/-0x500..0x1000 and *src>>7); v1>=11 and the fallthrough ops return
 * unmodified. Epilogue: v0 = a2+6 (3-halfword op consumed), jr $ra.
 * Sibling leaves: desc_ret_a2p6 (0x8017B8B4) / desc_ret_a2p6_b.
 * Ported 1:1 from the cps_8017b8d4 CPS transliteration (19 CPS blocks);
 * clean_cps.map routes all 19 continuation addrs here. */
void desc_u16pair_dispatch(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017B8E0u: goto block_8017B8E0;
            case 0x8017B8FCu: goto block_8017B8FC;
            case 0x8017B914u: goto block_8017B914;
            case 0x8017B92Cu: goto block_8017B92C;
            case 0x8017B944u: goto block_8017B944;
            case 0x8017B950u: goto block_8017B950;
            case 0x8017B970u: goto block_8017B970;
            case 0x8017B974u: goto block_8017B974;
            case 0x8017B98Cu: goto block_8017B98C;
            case 0x8017B9B4u: goto block_8017B9B4;
            case 0x8017B9C0u: goto block_8017B9C0;
            case 0x8017B9CCu: goto block_8017B9CC;
            case 0x8017B9DCu: goto block_8017B9DC;
            case 0x8017BA04u: goto block_8017BA04;
            case 0x8017BA10u: goto block_8017BA10;
            case 0x8017BA18u: goto block_8017BA18;
            case 0x8017BA30u: goto block_8017BA30;
            case 0x8017BA34u: goto block_8017BA34;
            default: break;
        }
    }
    /* Address: 0x8017B8D4, Size: 360 bytes, Blocks: 19 */

block_8017B8D4:
    v0 = v1 < (uint32_t)11;  /* 0x8017B8D4: 0x2C62000B */
    int cond_8017B8D8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x8017B8DC: 0x3C028012 */
    if (cond_8017B8D8) {
        psx_check_interrupts_at(cpu, 0x8017BA34u);
        goto block_8017BA34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B8E0u);
    }

block_8017B8E0:
    v0 += 10284;  /* 0x8017B8E0: 0x2442282C */
    v1 <<= 2;  /* 0x8017B8E4: 0x00031880 */
    v1 += v0;  /* 0x8017B8E8: 0x00621821 */
    v0 = cpu->read_word(v1);  /* 0x8017B8EC: 0x8C620000 */
    /* nop */  /* 0x8017B8F0: 0x00000000 */
    uint32_t jt_8017B8F4 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B8F8: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_8017B8F4);
    cpu->pc = jt_8017B8F4; return;  /* CPS: jr v0 */

block_8017B8FC:
    v0 = cpu->read_half(a2 + 2);  /* 0x8017B8FC: 0x94C20002 */
    /* nop */  /* 0x8017B900: 0x00000000 */
    cpu->write_half(a0 + 138, (uint16_t)v0);  /* 0x8017B904: 0xA482008A */
    v0 = cpu->read_half(a2 + 4);  /* 0x8017B908: 0x94C20004 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 140, (uint16_t)v0);  /* 0x8017B910: 0xA482008C */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B914:
    v0 = cpu->read_half(a2 + 2);  /* 0x8017B914: 0x94C20002 */
    /* nop */  /* 0x8017B918: 0x00000000 */
    cpu->write_half(a0 + 150, (uint16_t)v0);  /* 0x8017B91C: 0xA4820096 */
    v0 = cpu->read_half(a2 + 4);  /* 0x8017B920: 0x94C20004 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 152, (uint16_t)v0);  /* 0x8017B928: 0xA4820098 */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B92C:
    v0 = cpu->read_half(a2 + 2);  /* 0x8017B92C: 0x94C20002 */
    /* nop */  /* 0x8017B930: 0x00000000 */
    cpu->write_half(a0 + 144, (uint16_t)v0);  /* 0x8017B934: 0xA4820090 */
    v0 = cpu->read_half(a2 + 4);  /* 0x8017B938: 0x94C20004 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 146, (uint16_t)v0);  /* 0x8017B940: 0xA4820092 */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B944:
    v0 = cpu->read_half(a2 + 2);  /* 0x8017B944: 0x94C20002 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 142, (uint16_t)v0);  /* 0x8017B94C: 0xA482008E */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B950:
    v0 = cpu->read_half(a0 + 116);  /* 0x8017B950: 0x94820074 */
    a1 = (int32_t)(int16_t)cpu->read_half(a0 + 118);  /* 0x8017B954: 0x84850076 */
    cpu->write_half(a0 + 150, (uint16_t)v0);  /* 0x8017B958: 0xA4820096 */
    v1 = (int32_t)(int16_t)cpu->read_half(a2 + 2);  /* 0x8017B95C: 0x84C30002 */
    /* nop */  /* 0x8017B960: 0x00000000 */
    v0 = (int32_t)v1 < (int32_t)a1;  /* 0x8017B964: 0x0065102A */
    int cond_8017B968 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B96C: 0x00000000 */
    if (cond_8017B968) {
        psx_check_interrupts_at(cpu, 0x8017B974u);
        goto block_8017B974;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B970u);
    }

block_8017B970:
    a1 = v1;  /* 0x8017B970: 0x00602821 */
    /* fall through to block_8017B974 */

block_8017B974:
    v1 = (int32_t)(int16_t)cpu->read_half(a2 + 4);  /* 0x8017B974: 0x84C30004 */
    v0 = a1 << 7;  /* 0x8017B978: 0x000511C0 */
    if (v1 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)v1); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)v1); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8017B97C: 0x0043001A */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8017B980: 0x00001012 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 152, (uint16_t)v0);  /* 0x8017B988: 0xA4820098 */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B98C:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 112);  /* 0x8017B98C: 0x84820070 */
    v1 = (int32_t)(int16_t)cpu->read_half(a2 + 2);  /* 0x8017B990: 0x84C30002 */
    /* nop */  /* 0x8017B994: 0x00000000 */
    v0 += v1;  /* 0x8017B998: 0x00431021 */
    v0 <<= 20;  /* 0x8017B99C: 0x00021500 */
    v0 = (int32_t)v0 >> 20;  /* 0x8017B9A0: 0x00021503 */
    cpu->write_half(a0 + 150, (uint16_t)v0);  /* 0x8017B9A4: 0xA4820096 */
    v0 = cpu->read_half(a2 + 4);  /* 0x8017B9A8: 0x94C20004 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 152, (uint16_t)v0);  /* 0x8017B9B0: 0xA4820098 */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B9B4:
    v0 = cpu->read_half(a2 + 2);  /* 0x8017B9B4: 0x94C20002 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 150, (uint16_t)v0);  /* 0x8017B9BC: 0xA4820096 */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B9C0:
    v0 = cpu->read_half(a2 + 2);  /* 0x8017B9C0: 0x94C20002 */
    /* delay slot (always executes) */
    cpu->write_half(a0 + 146, (uint16_t)v0);  /* 0x8017B9C8: 0xA4820092 */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B9CC:
    v0 = cpu->read_half(a0 + 152);  /* 0x8017B9CC: 0x94820098 */
    v1 = cpu->read_half(a2 + 2);  /* 0x8017B9D0: 0x94C30002 */
    /* delay slot (always executes) */
    v0 += v1;  /* 0x8017B9D8: 0x00431021 */
    psx_check_interrupts_at(cpu, 0x8017BA30u);
    goto block_8017BA30;  /* j */

block_8017B9DC:
    v0 = cpu->read_half(a0 + 152);  /* 0x8017B9DC: 0x94820098 */
    v1 = cpu->read_half(a2 + 2);  /* 0x8017B9E0: 0x94C30002 */
    /* nop */  /* 0x8017B9E4: 0x00000000 */
    v0 += v1;  /* 0x8017B9E8: 0x00431021 */
    cpu->write_half(a0 + 152, (uint16_t)v0);  /* 0x8017B9EC: 0xA4820098 */
    v0 <<= 16;  /* 0x8017B9F0: 0x00021400 */
    v1 = (int32_t)v0 >> 16;  /* 0x8017B9F4: 0x00021C03 */
    v0 = (int32_t)v1 < 4097;  /* 0x8017B9F8: 0x28621001 */
    int cond_8017B9FC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4096;  /* 0x8017BA00: 0x24021000 */
    if (cond_8017B9FC) {
        psx_check_interrupts_at(cpu, 0x8017BA30u);
        goto block_8017BA30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA04u);
    }

block_8017BA04:
    v0 = (int32_t)v1 < -1280;  /* 0x8017BA04: 0x2862FB00 */
    int cond_8017BA08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = -1280;  /* 0x8017BA0C: 0x2402FB00 */
    if (cond_8017BA08) {
        psx_check_interrupts_at(cpu, 0x8017BA34u);
        goto block_8017BA34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA10u);
    }

block_8017BA10:
    /* delay slot (always executes) */
    cpu->write_half(a0 + 152, (uint16_t)v0);  /* 0x8017BA14: 0xA4820098 */
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017BA18:
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + 152);  /* 0x8017BA18: 0x84830098 */
    v0 = (int32_t)(int16_t)cpu->read_half(a2 + 2);  /* 0x8017BA1C: 0x84C20002 */
    /* nop */  /* 0x8017BA20: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));  /* 0x8017BA24: 0x00620018 */
    a3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8017BA28: 0x00003812 */
    v0 = (int32_t)a3 >> 7;  /* 0x8017BA2C: 0x000711C3 */
    /* fall through to block_8017BA30 */

block_8017BA30:
    cpu->write_half(a0 + 152, (uint16_t)v0);  /* 0x8017BA30: 0xA4820098 */
    /* fall through to block_8017BA34 */

block_8017BA34:
    ;
    uint32_t jt_8017BA34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 6;  /* 0x8017BA38: 0x24C20006 */
    psx_check_interrupts_at(cpu, jt_8017BA34);
    cpu->pc = jt_8017BA34; return;  /* CPS: jr $ra */
}



/* ======== desc_flagcond_store (guest 0x8017E6B0) ========
 * Descriptor-script condition op tail: v1 selects the predicate over the
 * a0 record — flag word a0+0x58 sign/bit tests and signed-half compares
 * against a0+0xB8/+0xBA/+0xBC (plus the 0x0800 / 0x08030000 masks) —
 * producing v1 = 0/1 stored as a word at a1+0x94. Reached via CPS j: split
 * from cps_8017e628's opcode decode (v0=1 preset for the default arm).
 * Epilogue: v0 = a2+2 (2-byte op consumed), jr $ra.
 * Ported 1:1 from the cps_8017e6b0 CPS transliteration (18 CPS blocks);
 * clean_cps.map routes all 18 continuation addrs here. */
void desc_flagcond_store(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017E6B8u: goto block_8017E6B8;
            case 0x8017E6C0u: goto block_8017E6C0;
            case 0x8017E6C8u: goto block_8017E6C8;
            case 0x8017E6D0u: goto block_8017E6D0;
            case 0x8017E6D8u: goto block_8017E6D8;
            case 0x8017E6E0u: goto block_8017E6E0;
            case 0x8017E6E8u: goto block_8017E6E8;
            case 0x8017E6F8u: goto block_8017E6F8;
            case 0x8017E704u: goto block_8017E704;
            case 0x8017E714u: goto block_8017E714;
            case 0x8017E724u: goto block_8017E724;
            case 0x8017E730u: goto block_8017E730;
            case 0x8017E740u: goto block_8017E740;
            case 0x8017E74Cu: goto block_8017E74C;
            case 0x8017E760u: goto block_8017E760;
            case 0x8017E76Cu: goto block_8017E76C;
            case 0x8017E774u: goto block_8017E774;
            default: break;
        }
    }
    /* Address: 0x8017E6B0, Size: 204 bytes, Blocks: 18 */

block_8017E6B0:
    ;
    int cond_8017E6B0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 2;  /* 0x8017E6B4: 0x28620002 */
    if (cond_8017E6B0) {
        psx_check_interrupts_at(cpu, 0x8017E704u);
        goto block_8017E704;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6B8u);
    }

block_8017E6B8:
    ;
    int cond_8017E6B8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;  /* 0x8017E6BC: 0x24020002 */
    if (cond_8017E6B8) {
        psx_check_interrupts_at(cpu, 0x8017E6D0u);
        goto block_8017E6D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6C0u);
    }

block_8017E6C0:
    ;
    int cond_8017E6C0 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 2;  /* 0x8017E6C4: 0x24C20002 */
    if (cond_8017E6C0) {
        psx_check_interrupts_at(cpu, 0x8017E6E8u);
        goto block_8017E6E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6C8u);
    }

block_8017E6C8:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E6CC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017E774u);
    goto block_8017E774;  /* j */

block_8017E6D0:
    ;
    int cond_8017E6D0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3;  /* 0x8017E6D4: 0x24020003 */
    if (cond_8017E6D0) {
        psx_check_interrupts_at(cpu, 0x8017E730u);
        goto block_8017E730;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6D8u);
    }

block_8017E6D8:
    ;
    int cond_8017E6D8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + 2;  /* 0x8017E6DC: 0x24C20002 */
    if (cond_8017E6D8) {
        psx_check_interrupts_at(cpu, 0x8017E74Cu);
        goto block_8017E74C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6E0u);
    }

block_8017E6E0:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E6E4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017E774u);
    goto block_8017E774;  /* j */

block_8017E6E8:
    v0 = cpu->read_word(a0 + 88);  /* 0x8017E6E8: 0x8C820058 */
    /* nop */  /* 0x8017E6EC: 0x00000000 */
    int cond_8017E6F0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* 0x8017E6F4: 0x00001821 */
    if (cond_8017E6F0) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6F8u);
    }

block_8017E6F8:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 186);  /* 0x8017E6F8: 0x848200BA */
    /* delay slot (always executes) */
    v1 = v1 < v0;  /* 0x8017E700: 0x0062182B */
    psx_check_interrupts_at(cpu, 0x8017E76Cu);
    goto block_8017E76C;  /* j */

block_8017E704:
    v0 = cpu->read_word(a0 + 88);  /* 0x8017E704: 0x8C820058 */
    /* nop */  /* 0x8017E708: 0x00000000 */
    int cond_8017E70C = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* 0x8017E710: 0x00001821 */
    if (cond_8017E70C) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E714u);
    }

block_8017E714:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 186);  /* 0x8017E714: 0x848200BA */
    /* nop */  /* 0x8017E718: 0x00000000 */
    int cond_8017E71C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E720: 0x00000000 */
    if (cond_8017E71C) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E724u);
    }

block_8017E724:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 184);  /* 0x8017E724: 0x848200B8 */
    /* delay slot (always executes) */
    v1 = v1 < v0;  /* 0x8017E72C: 0x0062182B */
    psx_check_interrupts_at(cpu, 0x8017E76Cu);
    goto block_8017E76C;  /* j */

block_8017E730:
    v0 = cpu->read_word(a0 + 88);  /* 0x8017E730: 0x8C820058 */
    /* nop */  /* 0x8017E734: 0x00000000 */
    int cond_8017E738 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* 0x8017E73C: 0x00001821 */
    if (cond_8017E738) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E740u);
    }

block_8017E740:
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + 188);  /* 0x8017E740: 0x848200BC */
    /* delay slot (always executes) */
    v1 = (int32_t)v1 < (int32_t)v0;  /* 0x8017E748: 0x0062182A */
    psx_check_interrupts_at(cpu, 0x8017E76Cu);
    goto block_8017E76C;  /* j */

block_8017E74C:
    a0 = cpu->read_word(a0 + 88);  /* 0x8017E74C: 0x8C840058 */
    /* nop */  /* 0x8017E750: 0x00000000 */
    v0 = a0 & 0x0800;  /* 0x8017E754: 0x30820800 */
    int cond_8017E758 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* 0x8017E75C: 0x00001821 */
    if (cond_8017E758) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E760u);
    }

block_8017E760:
    v0 = 0x08030000u;  /* 0x08030000 */  /* 0x8017E760: 0x3C020803 */
    v0 = a0 & v0;  /* 0x8017E764: 0x00821024 */
    v1 = v1 < v0;  /* 0x8017E768: 0x0062182B */
    /* fall through to block_8017E76C */

block_8017E76C:
    cpu->write_word(a1 + 148, v1);  /* 0x8017E76C: 0xACA30094 */
    v0 = a2 + 2;  /* 0x8017E770: 0x24C20002 */
    /* fall through to block_8017E774 */

block_8017E774:
    ;
    uint32_t jt_8017E774 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E778: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_8017E774);
    cpu->pc = jt_8017E774; return;  /* CPS: jr $ra */
}


