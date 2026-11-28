/* recon_8019.c — clean-room reconstructions of guest functions in the
 * 0x8019xxxx band (SLES_017.22_full_43.c): the text-window GPU emit path
 * and the text-format engine.
 *
 * Both are atomic clean ports derived from the timing-faithful cpst_
 * transliterations in game/cd_irq_probe.c (tools/cps_to_recon.py): the
 * cpu->pc continuation switch and psx_slice_block yields are removed —
 * every guest `jal` runs synchronously through rs_jal (clean port when
 * covered, generated body otherwise), intra-function edges stay plain
 * gotos behind the same psx_check_interrupts_at resume points the CPS
 * pieces used, and per-instruction psx_cyc_step / psx_cyc_load_* /
 * psx_icache_fetch accounting is kept verbatim so device deadlines
 * service inside the body exactly as in the CPS twin. An IRQ taken at a
 * check resumes through the generated block at resume_pc (the retired
 * clean_cps.map rows now fall through to generated dispatch), matching
 * the CPS coroutine contract. All mutable state lives in cpu->gpr[]/
 * guest RAM so scheduler longjmps preserve it.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern int      br2_bail_pending(void);
extern int      clean_try_dispatch(CPUState *cpu, uint32_t addr);
extern void     psx_dispatch_call(CPUState *cpu, uint32_t target_addr,
                                  uint32_t return_addr);

/* rs_jal — faithful guest `jal` from clean C: dispatch the callee
 * synchronously (clean port when covered, generated body otherwise),
 * return its v0. Same contract as recon_8015.c's rs_jal / ev.c's br2_jal.
 * Defined before the GPR #defines so its locals may use ra/pc/sp names. */
static uint32_t rs_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
    psx_dispatch_call(cpu, fn, link);
    cpu->gpr[29] = sp;
    {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
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


void text_win_emit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

/* Address: 0x801906E8, Size: 796 bytes, Blocks: 39 */

block_801906E8:
    psx_icache_fetch(cpu, 0x801906E8u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 80;  /* 0x801906E8: 0x27BDFFB0 */
    psx_cyc_step(cpu, 0x41u);
    a2 = 128;  /* 0x801906EC: 0x24060080 */
    psx_icache_fetch(cpu, 0x801906F0u);
    psx_cyc_step(cpu, 0x20000040u);
    cpu->write_word(sp + 28, a2);  /* 0x801906F0: 0xAFA6001C */
    psx_cyc_step(cpu, 0x20000040u);
    cpu->write_word(sp + 32, a2);  /* 0x801906F4: 0xAFA60020 */
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 76, ra);  /* 0x801906F8: 0xAFBF004C */
    psx_cyc_step(cpu, 0x60000000u);
    cpu->write_word(sp + 72, fp);  /* 0x801906FC: 0xAFBE0048 */
    psx_icache_fetch(cpu, 0x80190700u);
    psx_cyc_step(cpu, 0x20800000u);
    cpu->write_word(sp + 68, s7);  /* 0x80190700: 0xAFB70044 */
    psx_cyc_step(cpu, 0x20400000u);
    cpu->write_word(sp + 64, s6);  /* 0x80190704: 0xAFB60040 */
    psx_cyc_step(cpu, 0x20200000u);
    cpu->write_word(sp + 60, s5);  /* 0x80190708: 0xAFB5003C */
    psx_cyc_step(cpu, 0x20100000u);
    cpu->write_word(sp + 56, s4);  /* 0x8019070C: 0xAFB40038 */
    psx_icache_fetch(cpu, 0x80190710u);
    psx_cyc_step(cpu, 0x20080000u);
    cpu->write_word(sp + 52, s3);  /* 0x80190710: 0xAFB30034 */
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 48, s2);  /* 0x80190714: 0xAFB20030 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 44, s1);  /* 0x80190718: 0xAFB1002C */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 40, s0);  /* 0x8019071C: 0xAFB00028 */
    psx_icache_fetch(cpu, 0x80190720u);
    psx_cyc_step(cpu, 0x20000001u);
    cpu->write_word(sp + 20, zero);  /* 0x80190720: 0xAFA00014 */
    psx_cyc_step(cpu, 0x10u);
    int cond_80190724 = ((int32_t)a0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000040u);
    cpu->write_word(sp + 36, a2);  /* 0x80190728: 0xAFA60024 */
    if (cond_80190724) {
        psx_check_interrupts_at(cpu, 0x80190744u);
        goto block_80190744;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019072Cu);
    }

block_8019072C:
    psx_icache_fetch(cpu, 0x8019072Cu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x8019072C: 0x3C02801B */
    psx_icache_fetch(cpu, 0x80190730u);
    v0 = psx_cyc_load_word(cpu, v0 + 30912, 2, 0x4u);  /* 0x80190730: 0x8C4278C0 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190734: 0x00000000 */
    psx_cyc_step(cpu, 0x14u);
    v0 = (int32_t)a0 < (int32_t)v0;  /* 0x80190738: 0x0082102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_8019073C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190740u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 1;  /* 0x80190740: 0x00041040 */
    if (cond_8019073C) {
        psx_check_interrupts_at(cpu, 0x8019077Cu);
        goto block_8019077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190744u);
    }

block_80190744:
    psx_icache_fetch(cpu, 0x80190744u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80190744: 0x3C04801B */
    a0 = psx_cyc_load_word(cpu, a0 + 30916, 4, 0x10u);  /* 0x80190748: 0x8C8478C4 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019074C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80190750u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 1;  /* 0x80190750: 0x00041040 */
    psx_cyc_step(cpu, 0x14u);
    v0 += a0;  /* 0x80190754: 0x00441021 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 4;  /* 0x80190758: 0x00021100 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801B0000u;  /* 0x801B0000 */  /* 0x8019075C: 0x3C01801B */
    psx_icache_fetch(cpu, 0x80190760u);
    psx_cyc_step(cpu, 0x6u);
    at += v0;  /* 0x80190760: 0x00220821 */
    v0 = psx_cyc_load_word(cpu, at + 30564, 2, 0x2u);  /* 0x80190764: 0x8C227764 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190768: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_8019076C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190770u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 1;  /* 0x80190770: 0x00041040 */
    if (cond_8019076C) {
        psx_check_interrupts_at(cpu, 0x8019077Cu);
        goto block_8019077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190774u);
    }

block_80190774:
    psx_icache_fetch(cpu, 0x80190774u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = zero;  /* 0x80190778: 0x00001021 */
    psx_check_interrupts_at(cpu, 0x801909D4u);
    goto block_801909D4;  /* j */

block_8019077C:
    psx_icache_fetch(cpu, 0x8019077Cu);
    psx_cyc_step(cpu, 0x14u);
    v0 += a0;  /* 0x8019077C: 0x00441021 */
    psx_icache_fetch(cpu, 0x80190780u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 4;  /* 0x80190780: 0x00021100 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80190784: 0x3C03801B */
    psx_cyc_step(cpu, 0x8u);
    v1 += 30528;  /* 0x80190788: 0x24637740 */
    psx_cyc_step(cpu, 0x8000Cu);
    s3 = v0 + v1;  /* 0x8019078C: 0x00439821 */
    psx_icache_fetch(cpu, 0x80190790u);
    psx_cyc_step(cpu, 0x80040u);
    a2 = s3 + 16;  /* 0x80190790: 0x26660010 */
    psx_cyc_step(cpu, 0x20000040u);
    cpu->write_word(sp + 16, a2);  /* 0x80190794: 0xAFA60010 */
    a0 = psx_cyc_load_word(cpu, sp + 16, 4, 0x20000000u);  /* 0x80190798: 0x8FA40010 */
    s0 = psx_cyc_load_word(cpu, s3 + 36, 16, 0x80000u);  /* 0x8019079C: 0x8E700024 */
    psx_icache_fetch(cpu, 0x801907A0u);
    fp = psx_cyc_load_word(cpu, s3 + 28, 30, 0x80000u);  /* 0x801907A0: 0x8E7E001C */
    s1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s3 + 8, 17, 0x80000u);  /* 0x801907A4: 0x86710008 */
    s5 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s3 + 10, 21, 0x80000u);  /* 0x801907A8: 0x8675000A */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s3 + 12, 2, 0x80000u);  /* 0x801907AC: 0x8662000C */
    psx_icache_fetch(cpu, 0x801907B0u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s3 + 14, 3, 0x80000u);  /* 0x801907B0: 0x8663000E */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801907B4: 0x00000000 */
    psx_cyc_step(cpu, 0x200008u);
    v1 = s5 + v1;  /* 0x801907B8: 0x02A31821 */
    psx_cyc_step(cpu, 0x20000008u);
    cpu->write_word(sp + 24, v1);  /* 0x801907BC: 0xAFA30018 */
    psx_icache_fetch(cpu, 0x801907C0u);
    s2 = psx_cyc_load_word(cpu, s3 + 32, 18, 0x80000u);  /* 0x801907C0: 0x8E720020 */
    s6 = psx_cyc_load_word(cpu, s3 + 44, 22, 0x80000u);  /* 0x801907C4: 0x8E76002C */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801907D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x820004u);
    s7 = s1 + v0;  /* 0x801907CC: 0x0222B821 */
    psx_check_interrupts_at(cpu, 0x80194040u);
    rs_jal(0x80194040u, 0x801907D0u);
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x801907D0u);
    goto block_801907D0;

block_801907D0:
    psx_icache_fetch(cpu, 0x801907D0u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801907D4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80190964u);
    goto block_80190964;  /* j */

block_801907D8:
    psx_icache_fetch(cpu, 0x801907D8u);
    psx_cyc_step(cpu, 0x40000001u);
    int cond_801907D8 = (fp == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 24;  /* 0x801907DC: 0x00031600 */
    if (cond_801907D8) {
        psx_check_interrupts_at(cpu, 0x80190974u);
        goto block_80190974;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801907E0u);
    }

block_801907E0:
    psx_icache_fetch(cpu, 0x801907E0u);
    psx_cyc_step(cpu, 0xCu);
    v1 = (int32_t)v0 >> 24;  /* 0x801907E0: 0x00021E03 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 32;  /* 0x801907E4: 0x24020020 */
    psx_cyc_step(cpu, 0xCu);
    int cond_801907E8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x100001u);
    s4 = zero;  /* 0x801907EC: 0x0000A021 */
    if (cond_801907E8) {
        psx_check_interrupts_at(cpu, 0x80190908u);
        goto block_80190908;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801907F0u);
    }

block_801907F0:
    psx_icache_fetch(cpu, 0x801907F0u);
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v1 < 33;  /* 0x801907F0: 0x28620021 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801907F4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 9;  /* 0x801907F8: 0x24020009 */
    if (cond_801907F4) {
        psx_check_interrupts_at(cpu, 0x80190814u);
        goto block_80190814;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801907FCu);
    }

block_801907FC:
    psx_icache_fetch(cpu, 0x801907FCu);
    psx_cyc_step(cpu, 0xCu);
    int cond_801907FC = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190800u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 10;  /* 0x80190800: 0x2402000A */
    if (cond_801907FC) {
        psx_check_interrupts_at(cpu, 0x80190874u);
        goto block_80190874;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190804u);
    }

block_80190804:
    psx_icache_fetch(cpu, 0x80190804u);
    psx_cyc_step(cpu, 0xCu);
    int cond_80190804 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190808: 0x00000000 */
    if (cond_80190804) {
        psx_check_interrupts_at(cpu, 0x80190920u);
        goto block_80190920;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019080Cu);
    }

block_8019080C:
    psx_icache_fetch(cpu, 0x8019080Cu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190810u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190810: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019087Cu);
    goto block_8019087C;  /* j */

block_80190814:
    psx_icache_fetch(cpu, 0x80190814u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 126;  /* 0x80190814: 0x2402007E */
    psx_cyc_step(cpu, 0xCu);
    int cond_80190818 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 99;  /* 0x8019081C: 0x24020063 */
    if (cond_80190818) {
        psx_check_interrupts_at(cpu, 0x8019087Cu);
        goto block_8019087C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190820u);
    }

block_80190820:
    psx_icache_fetch(cpu, 0x80190820u);
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x80190820: 0x26100001 */
    v1 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s0, 3, 0x10000u);  /* 0x80190824: 0x82030000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190828: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    int cond_8019082C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190830u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190830: 0x00000000 */
    if (cond_8019082C) {
        psx_check_interrupts_at(cpu, 0x80190924u);
        goto block_80190924;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190834u);
    }

block_80190834:
    psx_icache_fetch(cpu, 0x80190834u);
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x80190834: 0x26100001 */
    v0 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s0, 2, 0x10000u);  /* 0x80190838: 0x82020000 */
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x8019083C: 0x26100001 */
    psx_icache_fetch(cpu, 0x80190840u);
    v1 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s0, 3, 0x10000u);  /* 0x80190840: 0x82030000 */
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x80190844: 0x26100001 */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 48;  /* 0x80190848: 0x2442FFD0 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 4;  /* 0x8019084C: 0x00021100 */
    psx_icache_fetch(cpu, 0x80190850u);
    psx_cyc_step(cpu, 0x8u);
    v1 -= 48;  /* 0x80190850: 0x2463FFD0 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 28, v0);  /* 0x80190854: 0xAFA2001C */
    v0 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s0, 2, 0x10000u);  /* 0x80190858: 0x82020000 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 4;  /* 0x8019085C: 0x00031900 */
    psx_icache_fetch(cpu, 0x80190860u);
    psx_cyc_step(cpu, 0x20000008u);
    cpu->write_word(sp + 32, v1);  /* 0x80190860: 0xAFA30020 */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 48;  /* 0x80190864: 0x2442FFD0 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 4;  /* 0x80190868: 0x00021100 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190870u);
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 36, v0);  /* 0x80190870: 0xAFA20024 */
    psx_check_interrupts_at(cpu, 0x80190924u);
    goto block_80190924;  /* j */

block_80190874:
    psx_icache_fetch(cpu, 0x80190874u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000u);
    s1 += 32;  /* 0x80190878: 0x26310020 */
    psx_check_interrupts_at(cpu, 0x8019090Cu);
    goto block_8019090C;  /* j */

block_8019087C:
    psx_icache_fetch(cpu, 0x8019087Cu);
    v1 = psx_cyc_load_byte(cpu, s0, 3, 0x10000u);  /* 0x8019087C: 0x92030000 */
    psx_icache_fetch(cpu, 0x80190880u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190880: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 + -97;  /* 0x80190884: 0x2462FF9F */
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)26;  /* 0x80190888: 0x2C42001A */
    psx_cyc_step(cpu, 0x5u);
    int cond_8019088C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190890u);
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 24;  /* 0x80190890: 0x00031600 */
    if (cond_8019088C) {
        psx_check_interrupts_at(cpu, 0x801908A0u);
        goto block_801908A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190894u);
    }

block_80190894:
    psx_icache_fetch(cpu, 0x80190894u);
    psx_cyc_step(cpu, 0x4u);
    v0 = (int32_t)v0 >> 24;  /* 0x80190894: 0x00021603 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0xCu);
    v1 = v0 + -64;  /* 0x8019089C: 0x2443FFC0 */
    psx_check_interrupts_at(cpu, 0x801908A8u);
    goto block_801908A8;  /* j */

block_801908A0:
    psx_icache_fetch(cpu, 0x801908A0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = (int32_t)v0 >> 24;  /* 0x801908A0: 0x00021603 */
    psx_cyc_step(cpu, 0xCu);
    v1 = v0 + -32;  /* 0x801908A4: 0x2443FFE0 */
    /* fall through to block_801908A8 */

block_801908A8:
    psx_icache_fetch(cpu, 0x801908A8u);
    psx_cyc_step(cpu, 0x8u);
    int cond_801908A8 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0xDu);
    v0 = v1;  /* 0x801908AC: 0x00601021 */
    if (cond_801908A8) {
        psx_check_interrupts_at(cpu, 0x801908B4u);
        goto block_801908B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801908B0u);
    }

block_801908B0:
    psx_icache_fetch(cpu, 0x801908B0u);
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 + 15;  /* 0x801908B0: 0x2462000F */
    /* fall through to block_801908B4 */

block_801908B4:
    psx_icache_fetch(cpu, 0x801908B4u);
    psx_cyc_step(cpu, 0x14u);
    a0 = (int32_t)v0 >> 4;  /* 0x801908B4: 0x00022103 */
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 4;  /* 0x801908B8: 0x00041100 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 - v0;  /* 0x801908BC: 0x00621023 */
    psx_icache_fetch(cpu, 0x801908C0u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801908C0: 0x000210C0 */
    psx_cyc_step(cpu, 0x40004u);
    cpu->write_byte(s2 + 12, (uint8_t)v0);  /* 0x801908C4: 0xA242000C */
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 3;  /* 0x801908C8: 0x000410C0 */
    psx_cyc_step(cpu, 0x40004u);
    cpu->write_byte(s2 + 13, (uint8_t)v0);  /* 0x801908CC: 0xA242000D */
    psx_icache_fetch(cpu, 0x801908D0u);
    psx_cyc_step(cpu, 0x60000u);
    cpu->write_half(s2 + 8, (uint16_t)s1);  /* 0x801908D0: 0xA6510008 */
    psx_cyc_step(cpu, 0x240000u);
    cpu->write_half(s2 + 10, (uint16_t)s5);  /* 0x801908D4: 0xA655000A */
    a2 = psx_cyc_load_byte(cpu, sp + 28, 6, 0x20000000u);  /* 0x801908D8: 0x93A6001C */
    psx_cyc_step(cpu, 0x40021u);
    a1 = s2;  /* 0x801908DC: 0x02402821 */
    psx_icache_fetch(cpu, 0x801908E0u);
    psx_cyc_step(cpu, 0x40040u);
    cpu->write_byte(s2 + 4, (uint8_t)a2);  /* 0x801908E0: 0xA2460004 */
    a2 = psx_cyc_load_byte(cpu, sp + 32, 6, 0x20000000u);  /* 0x801908E4: 0x93A60020 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801908E8: 0x00000000 */
    psx_cyc_step(cpu, 0x40040u);
    cpu->write_byte(s2 + 5, (uint8_t)a2);  /* 0x801908EC: 0xA2460005 */
    psx_icache_fetch(cpu, 0x801908F0u);
    a2 = psx_cyc_load_byte(cpu, sp + 36, 6, 0x20000000u);  /* 0x801908F0: 0x93A60024 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801908F4: 0x00000000 */
    psx_cyc_step(cpu, 0x40040u);
    cpu->write_byte(s2 + 6, (uint8_t)a2);  /* 0x801908F8: 0xA2460006 */
    a0 = psx_cyc_load_word(cpu, sp + 16, 4, 0x20000000u);  /* 0x801908FC: 0x8FA40010 */
    psx_icache_fetch(cpu, 0x80190900u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80190908u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40000u);
    s2 += 16;  /* 0x80190904: 0x26520010 */
    psx_check_interrupts_at(cpu, 0x80194000u);
    rs_jal(0x80194000u, 0x80190908u);
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x80190908u);
    goto block_80190908;

block_80190908:
    psx_icache_fetch(cpu, 0x80190908u);
    psx_cyc_step(cpu, 0x20000u);
    s1 += 8;  /* 0x80190908: 0x26310008 */
    /* fall through to block_8019090C */

block_8019090C:
    psx_icache_fetch(cpu, 0x8019090Cu);
    psx_cyc_step(cpu, 0x820004u);
    v0 = (int32_t)s1 < (int32_t)s7;  /* 0x8019090C: 0x0237102A */
    psx_icache_fetch(cpu, 0x80190910u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80190910 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190914: 0x00000000 */
    if (cond_80190910) {
        psx_check_interrupts_at(cpu, 0x80190924u);
        goto block_80190924;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190918u);
    }

block_80190918:
    psx_icache_fetch(cpu, 0x80190918u);
    psx_cyc_step(cpu, 0x400001u);
    int cond_80190918 = (s6 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019091C: 0x00000000 */
    if (cond_80190918) {
        psx_check_interrupts_at(cpu, 0x80190924u);
        goto block_80190924;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190920u);
    }

block_80190920:
    psx_icache_fetch(cpu, 0x80190920u);
    psx_cyc_step(cpu, 0x100001u);
    s4 = 1;  /* 0x80190920: 0x24140001 */
    /* fall through to block_80190924 */

block_80190924:
    psx_icache_fetch(cpu, 0x80190924u);
    psx_cyc_step(cpu, 0x100001u);
    int cond_80190924 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190928: 0x00000000 */
    if (cond_80190924) {
        psx_check_interrupts_at(cpu, 0x8019095Cu);
        goto block_8019095C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019092Cu);
    }

block_8019092C:
    psx_icache_fetch(cpu, 0x8019092Cu);
    a2 = psx_cyc_load_word(cpu, sp + 20, 6, 0x20000000u);  /* 0x8019092C: 0x8FA60014 */
    psx_icache_fetch(cpu, 0x80190930u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190930: 0x00000000 */
    psx_cyc_step(cpu, 0x20044u);
    v0 = (int32_t)a2 < (int32_t)s1;  /* 0x80190934: 0x00D1102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_80190938 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019093C: 0x00000000 */
    if (cond_80190938) {
        psx_check_interrupts_at(cpu, 0x80190944u);
        goto block_80190944;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190940u);
    }

block_80190940:
    psx_icache_fetch(cpu, 0x80190940u);
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 20, s1);  /* 0x80190940: 0xAFB10014 */
    /* fall through to block_80190944 */

block_80190944:
    psx_icache_fetch(cpu, 0x80190944u);
    psx_cyc_step(cpu, 0x200000u);
    s5 += 8;  /* 0x80190944: 0x26B50008 */
    a2 = psx_cyc_load_word(cpu, sp + 24, 6, 0x20000000u);  /* 0x80190948: 0x8FA60018 */
    s1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s3 + 8, 17, 0x80000u);  /* 0x8019094C: 0x86710008 */
    psx_icache_fetch(cpu, 0x80190950u);
    psx_cyc_step(cpu, 0x200044u);
    v0 = (int32_t)s5 < (int32_t)a2;  /* 0x80190950: 0x02A6102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_80190954 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190958: 0x00000000 */
    if (cond_80190954) {
        psx_check_interrupts_at(cpu, 0x80190974u);
        goto block_80190974;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019095Cu);
    }

block_8019095C:
    psx_icache_fetch(cpu, 0x8019095Cu);
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x8019095C: 0x26100001 */
    psx_icache_fetch(cpu, 0x80190960u);
    psx_cyc_step(cpu, 0x40000000u);
    fp--;  /* 0x80190960: 0x27DEFFFF */
    /* fall through to block_80190964 */

block_80190964:
    psx_icache_fetch(cpu, 0x80190964u);
    v0 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s0, 2, 0x10000u);  /* 0x80190964: 0x82020000 */
    v1 = psx_cyc_load_byte(cpu, s0, 3, 0x10000u);  /* 0x80190968: 0x92030000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_8019096C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190970u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190970: 0x00000000 */
    if (cond_8019096C) {
        psx_check_interrupts_at(cpu, 0x801907D8u);
        goto block_801907D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190974u);
    }

block_80190974:
    psx_icache_fetch(cpu, 0x80190974u);
    v0 = psx_cyc_load_byte(cpu, s3 + 7, 2, 0x80000u);  /* 0x80190974: 0x92620007 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190978: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_8019097C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190980u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190980: 0x00000000 */
    if (cond_8019097C) {
        psx_check_interrupts_at(cpu, 0x801909B8u);
        goto block_801909B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190984u);
    }

block_80190984:
    psx_icache_fetch(cpu, 0x80190984u);
    a0 = psx_cyc_load_word(cpu, sp + 16, 4, 0x20000000u);  /* 0x80190984: 0x8FA40010 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80190990u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x80021u);
    a1 = s3;  /* 0x8019098C: 0x02602821 */
    psx_check_interrupts_at(cpu, 0x80194000u);
    rs_jal(0x80194000u, 0x80190990u);
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x80190990u);
    goto block_80190990;

block_80190990:
    psx_icache_fetch(cpu, 0x80190990u);
    psx_cyc_step(cpu, 0x400001u);
    int cond_80190990 = (s6 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190994: 0x00000000 */
    if (cond_80190990) {
        psx_check_interrupts_at(cpu, 0x801909B8u);
        goto block_801909B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190998u);
    }

block_80190998:
    psx_icache_fetch(cpu, 0x80190998u);
    v0 = psx_cyc_load_half(cpu, s3 + 8, 2, 0x80000u);  /* 0x80190998: 0x96620008 */
    a2 = psx_cyc_load_word(cpu, sp + 20, 6, 0x20000000u);  /* 0x8019099C: 0x8FA60014 */
    psx_icache_fetch(cpu, 0x801909A0u);
    v1 = psx_cyc_load_half(cpu, s3 + 10, 3, 0x80000u);  /* 0x801909A0: 0x9663000A */
    psx_cyc_step(cpu, 0x44u);
    v0 = a2 - v0;  /* 0x801909A4: 0x00C21023 */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 8;  /* 0x801909A8: 0x2463FFF8 */
    psx_cyc_step(cpu, 0x200008u);
    v1 = s5 - v1;  /* 0x801909AC: 0x02A31823 */
    psx_icache_fetch(cpu, 0x801909B0u);
    psx_cyc_step(cpu, 0x80004u);
    cpu->write_half(s3 + 12, (uint16_t)v0);  /* 0x801909B0: 0xA662000C */
    psx_cyc_step(cpu, 0x80008u);
    cpu->write_half(s3 + 14, (uint16_t)v1);  /* 0x801909B4: 0xA663000E */
    /* fall through to block_801909B8 */

block_801909B8:
    psx_icache_fetch(cpu, 0x801909B8u);
    a0 = psx_cyc_load_word(cpu, sp + 16, 4, 0x20000000u);  /* 0x801909B8: 0x8FA40010 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801909C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801909C0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801909C0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80191770u);
    rs_jal(0x80191770u, 0x801909C4u);
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x801909C4u);
    goto block_801909C4;

block_801909C4:
    psx_icache_fetch(cpu, 0x801909C4u);
    v1 = psx_cyc_load_word(cpu, s3 + 36, 3, 0x80000u);  /* 0x801909C4: 0x8E630024 */
    v0 = psx_cyc_load_word(cpu, sp + 16, 2, 0x20000000u);  /* 0x801909C8: 0x8FA20010 */
    psx_cyc_step(cpu, 0x80001u);
    cpu->write_word(s3 + 40, zero);  /* 0x801909CC: 0xAE600028 */
    psx_icache_fetch(cpu, 0x801909D0u);
    psx_cyc_step(cpu, 0x9u);
    cpu->write_byte(v1, (uint8_t)zero);  /* 0x801909D0: 0xA0600000 */
    /* fall through to block_801909D4 */

block_801909D4:
    psx_icache_fetch(cpu, 0x801909D4u);
    ra = psx_cyc_load_word(cpu, sp + 76, 31, 0x20000000u);  /* 0x801909D4: 0x8FBF004C */
    fp = psx_cyc_load_word(cpu, sp + 72, 30, 0x20000000u);  /* 0x801909D8: 0x8FBE0048 */
    s7 = psx_cyc_load_word(cpu, sp + 68, 23, 0x20000000u);  /* 0x801909DC: 0x8FB70044 */
    psx_icache_fetch(cpu, 0x801909E0u);
    s6 = psx_cyc_load_word(cpu, sp + 64, 22, 0x20000000u);  /* 0x801909E0: 0x8FB60040 */
    s5 = psx_cyc_load_word(cpu, sp + 60, 21, 0x20000000u);  /* 0x801909E4: 0x8FB5003C */
    s4 = psx_cyc_load_word(cpu, sp + 56, 20, 0x20000000u);  /* 0x801909E8: 0x8FB40038 */
    s3 = psx_cyc_load_word(cpu, sp + 52, 19, 0x20000000u);  /* 0x801909EC: 0x8FB30034 */
    psx_icache_fetch(cpu, 0x801909F0u);
    s2 = psx_cyc_load_word(cpu, sp + 48, 18, 0x20000000u);  /* 0x801909F0: 0x8FB20030 */
    s1 = psx_cyc_load_word(cpu, sp + 44, 17, 0x20000000u);  /* 0x801909F4: 0x8FB1002C */
    s0 = psx_cyc_load_word(cpu, sp + 40, 16, 0x20000000u);  /* 0x801909F8: 0x8FB00028 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_801909FC = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190A00u);
    psx_cyc_step(cpu, 0x20000000u);
    sp += 80;  /* 0x80190A00: 0x27BD0050 */
    psx_check_interrupts_at(cpu, jt_801909FC);
    cpu->pc = jt_801909FC; return;  /* CPS: jr $ra */
}

void text_format_run(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

/* Address: 0x80190A14, Size: 944 bytes, Blocks: 49 */

block_80190A14:
    psx_icache_fetch(cpu, 0x80190A14u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 568;  /* 0x80190A14: 0x27BDFDC8 */
    psx_cyc_step(cpu, 0x20000004u);
    v0 = sp + 572;  /* 0x80190A18: 0x27A2023C */
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 560, ra);  /* 0x80190A1C: 0xAFBF0230 */
    psx_icache_fetch(cpu, 0x80190A20u);
    psx_cyc_step(cpu, 0x20200000u);
    cpu->write_word(sp + 556, s5);  /* 0x80190A20: 0xAFB5022C */
    psx_cyc_step(cpu, 0x20100000u);
    cpu->write_word(sp + 552, s4);  /* 0x80190A24: 0xAFB40228 */
    psx_cyc_step(cpu, 0x20080000u);
    cpu->write_word(sp + 548, s3);  /* 0x80190A28: 0xAFB30224 */
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 544, s2);  /* 0x80190A2C: 0xAFB20220 */
    psx_icache_fetch(cpu, 0x80190A30u);
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 540, s1);  /* 0x80190A30: 0xAFB1021C */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 536, s0);  /* 0x80190A34: 0xAFB00218 */
    psx_cyc_step(cpu, 0x20000010u);
    cpu->write_word(sp + 568, a0);  /* 0x80190A38: 0xAFA40238 */
    psx_cyc_step(cpu, 0x10u);
    int cond_80190A3C = ((int32_t)a0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190A40u);
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 528, v0);  /* 0x80190A40: 0xAFA20210 */
    if (cond_80190A3C) {
        psx_check_interrupts_at(cpu, 0x80190A5Cu);
        goto block_80190A5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190A44u);
    }

block_80190A44:
    psx_icache_fetch(cpu, 0x80190A44u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80190A44: 0x3C02801B */
    v0 = psx_cyc_load_word(cpu, v0 + 30912, 2, 0x4u);  /* 0x80190A48: 0x8C4278C0 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190A4C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80190A50u);
    psx_cyc_step(cpu, 0x14u);
    v0 = (int32_t)a0 < (int32_t)v0;  /* 0x80190A50: 0x0082102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_80190A54 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000004u);
    v0 = sp + 576;  /* 0x80190A58: 0x27A20240 */
    if (cond_80190A54) {
        psx_check_interrupts_at(cpu, 0x80190A94u);
        goto block_80190A94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190A5Cu);
    }

block_80190A5C:
    psx_icache_fetch(cpu, 0x80190A5Cu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80190A5C: 0x3C02801B */
    psx_icache_fetch(cpu, 0x80190A60u);
    v0 = psx_cyc_load_word(cpu, v0 + 30916, 2, 0x4u);  /* 0x80190A60: 0x8C4278C4 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190A64: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v1 = v0 << 1;  /* 0x80190A68: 0x00021840 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x80190A6C: 0x00621821 */
    psx_icache_fetch(cpu, 0x80190A70u);
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 4;  /* 0x80190A70: 0x00031900 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801B0000u;  /* 0x801B0000 */  /* 0x80190A74: 0x3C01801B */
    psx_cyc_step(cpu, 0xAu);
    at += v1;  /* 0x80190A78: 0x00230821 */
    v1 = psx_cyc_load_word(cpu, at + 30564, 3, 0x2u);  /* 0x80190A7C: 0x8C237764 */
    psx_icache_fetch(cpu, 0x80190A80u);
    psx_cyc_step(cpu, 0x80011u);
    s3 = a0;  /* 0x80190A80: 0x00809821 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80190A84 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 568, v0);  /* 0x80190A88: 0xAFA20238 */
    if (cond_80190A84) {
        psx_check_interrupts_at(cpu, 0x80190A9Cu);
        goto block_80190A9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190A8Cu);
    }

block_80190A8C:
    psx_icache_fetch(cpu, 0x80190A8Cu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190A90u);
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x80190A90: 0x2402FFFF */
    psx_check_interrupts_at(cpu, 0x80190DA0u);
    goto block_80190DA0;  /* j */

block_80190A94:
    psx_icache_fetch(cpu, 0x80190A94u);
    s3 = psx_cyc_load_word(cpu, sp + 572, 19, 0x20000000u);  /* 0x80190A94: 0x8FB3023C */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 528, v0);  /* 0x80190A98: 0xAFA20210 */
    /* fall through to block_80190A9C */

block_80190A9C:
    psx_icache_fetch(cpu, 0x80190A9Cu);
    v1 = psx_cyc_load_word(cpu, sp + 568, 3, 0x20000000u);  /* 0x80190A9C: 0x8FA30238 */
    psx_icache_fetch(cpu, 0x80190AA0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190AA0: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 1;  /* 0x80190AA4: 0x00031040 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x80190AA8: 0x00431021 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 4;  /* 0x80190AAC: 0x00021100 */
    psx_icache_fetch(cpu, 0x80190AB0u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80190AB0: 0x3C03801B */
    psx_cyc_step(cpu, 0x8u);
    v1 += 30528;  /* 0x80190AB4: 0x24637740 */
    psx_cyc_step(cpu, 0x2000Cu);
    s1 = v0 + v1;  /* 0x80190AB8: 0x00438821 */
    v1 = psx_cyc_load_word(cpu, s1 + 40, 3, 0x20000u);  /* 0x80190ABC: 0x8E230028 */
    psx_icache_fetch(cpu, 0x80190AC0u);
    v0 = psx_cyc_load_word(cpu, s1 + 28, 2, 0x20000u);  /* 0x80190AC0: 0x8E22001C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190AC4: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v0 < (int32_t)v1;  /* 0x80190AC8: 0x0043102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_80190ACC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190AD0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190AD0: 0x00000000 */
    if (cond_80190ACC) {
        psx_check_interrupts_at(cpu, 0x80190A8Cu);
        goto block_80190A8C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190AD4u);
    }

block_80190AD4:
    psx_icache_fetch(cpu, 0x80190AD4u);
    a0 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s3, 4, 0x80000u);  /* 0x80190AD4: 0x82640000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190AD8: 0x00000000 */
    psx_cyc_step(cpu, 0x11u);
    int cond_80190ADC = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190AE0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190AE0: 0x00000000 */
    if (cond_80190ADC) {
        psx_check_interrupts_at(cpu, 0x80190D88u);
        goto block_80190D88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190AE4u);
    }

block_80190AE4:
    psx_icache_fetch(cpu, 0x80190AE4u);
    psx_cyc_step(cpu, 0x100001u);
    s4 = 37;  /* 0x80190AE4: 0x24140025 */
    psx_cyc_step(cpu, 0x200000u);
    s5 = 0xCCCC0000u;  /* 0xCCCC0000 */  /* 0x80190AE8: 0x3C15CCCC */
    psx_cyc_step(cpu, 0x200000u);
    s5 |= 0xCCCD;  /* 0x80190AEC: 0x36B5CCCD */
    /* fall through to block_80190AF0 */

block_80190AF0:
    psx_icache_fetch(cpu, 0x80190AF0u);
    psx_cyc_step(cpu, 0x100010u);
    int cond_80190AF0 = (a0 != s4);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190AF4: 0x00000000 */
    if (cond_80190AF0) {
        psx_check_interrupts_at(cpu, 0x80190B0Cu);
        goto block_80190B0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190AF8u);
    }

block_80190AF8:
    psx_icache_fetch(cpu, 0x80190AF8u);
    psx_cyc_step(cpu, 0x80000u);
    s3++;  /* 0x80190AF8: 0x26730001 */
    a0 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s3, 4, 0x80000u);  /* 0x80190AFC: 0x82640000 */
    psx_icache_fetch(cpu, 0x80190B00u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190B00: 0x00000000 */
    psx_cyc_step(cpu, 0x100010u);
    int cond_80190B04 = (a0 != s4);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40001u);
    s2 = zero;  /* 0x80190B08: 0x00009021 */
    if (cond_80190B04) {
        psx_check_interrupts_at(cpu, 0x80190B3Cu);
        goto block_80190B3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190B0Cu);
    }

block_80190B0C:
    psx_icache_fetch(cpu, 0x80190B0Cu);
    v0 = psx_cyc_load_word(cpu, s1 + 40, 2, 0x20000u);  /* 0x80190B0C: 0x8E220028 */
    psx_icache_fetch(cpu, 0x80190B10u);
    v1 = psx_cyc_load_word(cpu, s1 + 36, 3, 0x20000u);  /* 0x80190B10: 0x8E230024 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190B14: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x80190B18: 0x00621821 */
    psx_cyc_step(cpu, 0x18u);
    cpu->write_byte(v1, (uint8_t)a0);  /* 0x80190B1C: 0xA0640000 */
    psx_icache_fetch(cpu, 0x80190B20u);
    v1 = psx_cyc_load_word(cpu, s1 + 28, 3, 0x20000u);  /* 0x80190B20: 0x8E23001C */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x80190B24: 0x24420001 */
    psx_cyc_step(cpu, 0xCu);
    v1 = (int32_t)v1 < (int32_t)v0;  /* 0x80190B28: 0x0062182A */
    psx_cyc_step(cpu, 0x9u);
    int cond_80190B2C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190B30u);
    psx_cyc_step(cpu, 0x20004u);
    cpu->write_word(s1 + 40, v0);  /* 0x80190B30: 0xAE220028 */
    if (cond_80190B2C) {
        psx_check_interrupts_at(cpu, 0x80190D74u);
        goto block_80190D74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190B34u);
    }

block_80190B34:
    psx_icache_fetch(cpu, 0x80190B34u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x80190B38: 0x2402FFFF */
    psx_check_interrupts_at(cpu, 0x80190DA0u);
    goto block_80190DA0;  /* j */

block_80190B3C:
    psx_icache_fetch(cpu, 0x80190B3Cu);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 ^ 0x0030;  /* 0x80190B3C: 0x38820030 */
    psx_icache_fetch(cpu, 0x80190B40u);
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)1;  /* 0x80190B40: 0x2C420001 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x45u);
    a2 = v0;  /* 0x80190B48: 0x00403021 */
    psx_check_interrupts_at(cpu, 0x80190B68u);
    goto block_80190B68;  /* j */

block_80190B4C:
    psx_icache_fetch(cpu, 0x80190B4Cu);
    psx_cyc_step(cpu, 0x40004u);
    v0 += s2;  /* 0x80190B4C: 0x00521021 */
    psx_icache_fetch(cpu, 0x80190B50u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 1;  /* 0x80190B50: 0x00021040 */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 48;  /* 0x80190B54: 0x2442FFD0 */
    psx_cyc_step(cpu, 0x40014u);
    s2 = v0 + a0;  /* 0x80190B58: 0x00449021 */
    psx_cyc_step(cpu, 0x80000u);
    s3++;  /* 0x80190B5C: 0x26730001 */
    psx_icache_fetch(cpu, 0x80190B60u);
    a0 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s3, 4, 0x80000u);  /* 0x80190B60: 0x82640000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190B64: 0x00000000 */
    /* fall through to block_80190B68 */

block_80190B68:
    psx_icache_fetch(cpu, 0x80190B68u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 + -48;  /* 0x80190B68: 0x2482FFD0 */
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)10;  /* 0x80190B6C: 0x2C42000A */
    psx_icache_fetch(cpu, 0x80190B70u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80190B70 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40004u);
    v0 = s2 << 2;  /* 0x80190B74: 0x00121080 */
    if (cond_80190B70) {
        psx_check_interrupts_at(cpu, 0x80190B4Cu);
        goto block_80190B4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190B78u);
    }

block_80190B78:
    psx_icache_fetch(cpu, 0x80190B78u);
    psx_cyc_step(cpu, 0x40000u);
    int cond_80190B78 = ((int32_t)s2 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 + -88;  /* 0x80190B7C: 0x2483FFA8 */
    if (cond_80190B78) {
        psx_check_interrupts_at(cpu, 0x80190B84u);
        goto block_80190B84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190B80u);
    }

block_80190B80:
    psx_icache_fetch(cpu, 0x80190B80u);
    psx_cyc_step(cpu, 0x40001u);
    s2 = 1;  /* 0x80190B80: 0x24120001 */
    /* fall through to block_80190B84 */

block_80190B84:
    psx_icache_fetch(cpu, 0x80190B84u);
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 < (uint32_t)33;  /* 0x80190B84: 0x2C620021 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80190B88 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20010000u);
    s0 = sp + 528;  /* 0x80190B8C: 0x27B00210 */
    if (cond_80190B88) {
        psx_check_interrupts_at(cpu, 0x80190CE4u);
        goto block_80190CE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190B90u);
    }

block_80190B90:
    psx_icache_fetch(cpu, 0x80190B90u);
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x80190B90: 0x00031080 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x80120000u;  /* 0x80120000 */  /* 0x80190B94: 0x3C018012 */
    psx_cyc_step(cpu, 0x6u);
    at += v0;  /* 0x80190B98: 0x00220821 */
    v0 = psx_cyc_load_word(cpu, at + 21328, 2, 0x2u);  /* 0x80190B9C: 0x8C225350 */
    psx_icache_fetch(cpu, 0x80190BA0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190BA0: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    uint32_t jt_80190BA4 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190BA8: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_80190BA4);
    cpu->pc = jt_80190BA4; return;  /* CPS: jr v0 */

block_80190BAC:
    psx_icache_fetch(cpu, 0x80190BACu);
    v0 = psx_cyc_load_word(cpu, sp + 528, 2, 0x20000000u);  /* 0x80190BAC: 0x8FA20210 */
    psx_icache_fetch(cpu, 0x80190BB0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190BB0: 0x00000000 */
    a0 = psx_cyc_load_word(cpu, v0, 4, 0x4u);  /* 0x80190BB4: 0x8C440000 */
    psx_cyc_step(cpu, 0x4u);
    v0 += 4;  /* 0x80190BB8: 0x24420004 */
    psx_cyc_step(cpu, 0x10u);
    int cond_80190BBC = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190BC0u);
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 528, v0);  /* 0x80190BC0: 0xAFA20210 */
    if (cond_80190BBC) {
        psx_check_interrupts_at(cpu, 0x80190BD0u);
        goto block_80190BD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190BC4u);
    }

block_80190BC4:
    psx_icache_fetch(cpu, 0x80190BC4u);
    psx_cyc_step(cpu, 0x11u);
    a0 = zero - a0;  /* 0x80190BC4: 0x00042023 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x41u);
    a2 = 45;  /* 0x80190BCC: 0x2406002D */
    psx_check_interrupts_at(cpu, 0x80190BD4u);
    goto block_80190BD4;  /* j */

block_80190BD0:
    psx_icache_fetch(cpu, 0x80190BD0u);
    psx_cyc_step(cpu, 0x41u);
    a2 = zero;  /* 0x80190BD0: 0x00003021 */
    /* fall through to block_80190BD4 */

block_80190BD4:
    psx_icache_fetch(cpu, 0x80190BD4u);
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x80190BD4: 0x00002821 */
    /* fall through to block_80190BD8 */

block_80190BD8:
    psx_icache_fetch(cpu, 0x80190BD8u);
    psx_cyc_step(cpu, 0x200010u);
    { uint64_t result = (uint64_t)a0 * (uint64_t)s5; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(a0));  /* 0x80190BD8: 0x00950019 */
    psx_cyc_step(cpu, 0x10000u);
    s0--;  /* 0x80190BDC: 0x2610FFFF */
    psx_icache_fetch(cpu, 0x80190BE0u);
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x80190BE0: 0x24A50001 */
    psx_cyc_step(cpu, 0x80u);
    a3 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x80190BE4: 0x00003810 */
    psx_cyc_step(cpu, 0x88u);
    v1 = (uint32_t)a3 >> 3;  /* 0x80190BE8: 0x000718C2 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x80190BEC: 0x00031080 */
    psx_icache_fetch(cpu, 0x80190BF0u);
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x80190BF0: 0x00431021 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 1;  /* 0x80190BF4: 0x00021040 */
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 - v0;  /* 0x80190BF8: 0x00821023 */
    psx_cyc_step(cpu, 0x4u);
    v0 += 48;  /* 0x80190BFC: 0x24420030 */
    psx_icache_fetch(cpu, 0x80190C00u);
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_byte(s0, (uint8_t)v0);  /* 0x80190C00: 0xA2020000 */
    psx_cyc_step(cpu, 0x21u);
    int cond_80190C04 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x19u);
    a0 = v1;  /* 0x80190C08: 0x00602021 */
    if (cond_80190C04) {
        psx_check_interrupts_at(cpu, 0x80190BD8u);
        goto block_80190BD8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C0Cu);
    }

block_80190C0C:
    psx_icache_fetch(cpu, 0x80190C0Cu);
    psx_cyc_step(cpu, 0x11u);
    int cond_80190C0C = (a0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190C10u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190C10: 0x00000000 */
    if (cond_80190C0C) {
        psx_check_interrupts_at(cpu, 0x80190BD8u);
        goto block_80190BD8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C14u);
    }

block_80190C14:
    psx_icache_fetch(cpu, 0x80190C14u);
    psx_cyc_step(cpu, 0x41u);
    int cond_80190C14 = (a2 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40024u);
    v0 = (int32_t)a1 < (int32_t)s2;  /* 0x80190C18: 0x00B2102A */
    if (cond_80190C14) {
        psx_check_interrupts_at(cpu, 0x80190CE8u);
        goto block_80190CE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C1Cu);
    }

block_80190C1C:
    psx_icache_fetch(cpu, 0x80190C1Cu);
    psx_cyc_step(cpu, 0x10000u);
    s0--;  /* 0x80190C1C: 0x2610FFFF */
    psx_icache_fetch(cpu, 0x80190C20u);
    psx_cyc_step(cpu, 0x10040u);
    cpu->write_byte(s0, (uint8_t)a2);  /* 0x80190C20: 0xA2060000 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x80190C28: 0x24A50001 */
    psx_check_interrupts_at(cpu, 0x80190CE4u);
    goto block_80190CE4;  /* j */

block_80190C2C:
    psx_icache_fetch(cpu, 0x80190C2Cu);
    v0 = psx_cyc_load_word(cpu, sp + 528, 2, 0x20000000u);  /* 0x80190C2C: 0x8FA20210 */
    psx_icache_fetch(cpu, 0x80190C30u);
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x80190C30: 0x00002821 */
    a0 = psx_cyc_load_word(cpu, v0, 4, 0x4u);  /* 0x80190C34: 0x8C440000 */
    psx_cyc_step(cpu, 0x4u);
    v0 += 4;  /* 0x80190C38: 0x24420004 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 528, v0);  /* 0x80190C3C: 0xAFA20210 */
    /* fall through to block_80190C40 */

block_80190C40:
    psx_icache_fetch(cpu, 0x80190C40u);
    psx_cyc_step(cpu, 0x10000u);
    s0--;  /* 0x80190C40: 0x2610FFFF */
    /* fall through to block_80190C44 */

block_80190C44:
    psx_icache_fetch(cpu, 0x80190C44u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 & 0x000F;  /* 0x80190C44: 0x3082000F */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80190C48: 0x3C03801C */
    v1 = psx_cyc_load_word(cpu, v1 + -32052, 3, 0x8u);  /* 0x80190C4C: 0x8C6382CC */
    psx_icache_fetch(cpu, 0x80190C50u);
    psx_cyc_step(cpu, 0x10u);
    a0 = (uint32_t)a0 >> 4;  /* 0x80190C50: 0x00042102 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x80190C54: 0x00621821 */
    v0 = psx_cyc_load_byte(cpu, v1, 2, 0x8u);  /* 0x80190C58: 0x90620000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x80190C5C: 0x24A50001 */
    psx_icache_fetch(cpu, 0x80190C60u);
    psx_cyc_step(cpu, 0x21u);
    int cond_80190C60 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_byte(s0, (uint8_t)v0);  /* 0x80190C64: 0xA2020000 */
    if (cond_80190C60) {
        psx_check_interrupts_at(cpu, 0x80190C40u);
        goto block_80190C40;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C68u);
    }

block_80190C68:
    psx_icache_fetch(cpu, 0x80190C68u);
    psx_cyc_step(cpu, 0x11u);
    int cond_80190C68 = (a0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10000u);
    s0--;  /* 0x80190C6C: 0x2610FFFF */
    if (cond_80190C68) {
        psx_check_interrupts_at(cpu, 0x80190C44u);
        goto block_80190C44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C70u);
    }

block_80190C70:
    psx_icache_fetch(cpu, 0x80190C70u);
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x80190C70: 0x26100001 */
    psx_cyc_step(cpu, 0x45u);
    v0 = a2;  /* 0x80190C74: 0x00C01021 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80190C78 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40024u);
    v0 = (int32_t)a1 < (int32_t)s2;  /* 0x80190C7C: 0x00B2102A */
    if (cond_80190C78) {
        psx_check_interrupts_at(cpu, 0x80190CE4u);
        goto block_80190CE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C80u);
    }

block_80190C80:
    psx_icache_fetch(cpu, 0x80190C80u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80190C80 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x9u);
    v1 = 48;  /* 0x80190C84: 0x24030030 */
    if (cond_80190C80) {
        psx_check_interrupts_at(cpu, 0x80190D28u);
        goto block_80190D28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C88u);
    }

block_80190C88:
    psx_icache_fetch(cpu, 0x80190C88u);
    psx_cyc_step(cpu, 0x10000u);
    s0--;  /* 0x80190C88: 0x2610FFFF */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x80190C8C: 0x24A50001 */
    psx_icache_fetch(cpu, 0x80190C90u);
    psx_cyc_step(cpu, 0x40024u);
    v0 = (int32_t)a1 < (int32_t)s2;  /* 0x80190C90: 0x00B2102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_80190C94 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10008u);
    cpu->write_byte(s0, (uint8_t)v1);  /* 0x80190C98: 0xA2030000 */
    if (cond_80190C94) {
        psx_check_interrupts_at(cpu, 0x80190C88u);
        goto block_80190C88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190C9Cu);
    }

block_80190C9C:
    psx_icache_fetch(cpu, 0x80190C9Cu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190CA0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190CA0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80190CE8u);
    goto block_80190CE8;  /* j */

block_80190CA4:
    psx_icache_fetch(cpu, 0x80190CA4u);
    psx_cyc_step(cpu, 0x10000u);
    s0--;  /* 0x80190CA4: 0x2610FFFF */
    v0 = psx_cyc_load_word(cpu, sp + 528, 2, 0x20000000u);  /* 0x80190CA8: 0x8FA20210 */
    psx_cyc_step(cpu, 0x21u);
    a1 = 1;  /* 0x80190CAC: 0x24050001 */
    psx_icache_fetch(cpu, 0x80190CB0u);
    v1 = psx_cyc_load_byte(cpu, v0, 3, 0x4u);  /* 0x80190CB0: 0x90430000 */
    psx_cyc_step(cpu, 0x4u);
    v0 += 4;  /* 0x80190CB4: 0x24420004 */
    psx_cyc_step(cpu, 0x10008u);
    cpu->write_byte(s0, (uint8_t)v1);  /* 0x80190CB8: 0xA2030000 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190CC0u);
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 528, v0);  /* 0x80190CC0: 0xAFA20210 */
    psx_check_interrupts_at(cpu, 0x80190CE4u);
    goto block_80190CE4;  /* j */

block_80190CC4:
    psx_icache_fetch(cpu, 0x80190CC4u);
    v0 = psx_cyc_load_word(cpu, sp + 528, 2, 0x20000000u);  /* 0x80190CC4: 0x8FA20210 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190CC8: 0x00000000 */
    s0 = psx_cyc_load_word(cpu, v0, 16, 0x4u);  /* 0x80190CCC: 0x8C500000 */
    psx_icache_fetch(cpu, 0x80190CD0u);
    psx_cyc_step(cpu, 0x4u);
    v0 += 4;  /* 0x80190CD0: 0x24420004 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 528, v0);  /* 0x80190CD4: 0xAFA20210 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80190CE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10011u);
    a0 = s0;  /* 0x80190CDC: 0x02002021 */
    psx_check_interrupts_at(cpu, 0x8018E510u);
    rs_jal(0x8018E510u, 0x80190CE0u);
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x80190CE0u);
    goto block_80190CE0;

block_80190CE0:
    psx_icache_fetch(cpu, 0x80190CE0u);
    psx_cyc_step(cpu, 0x25u);
    a1 = v0;  /* 0x80190CE0: 0x00402821 */
    /* fall through to block_80190CE4 */

block_80190CE4:
    psx_icache_fetch(cpu, 0x80190CE4u);
    psx_cyc_step(cpu, 0x40024u);
    v0 = (int32_t)a1 < (int32_t)s2;  /* 0x80190CE4: 0x00B2102A */
    /* fall through to block_80190CE8 */

block_80190CE8:
    psx_icache_fetch(cpu, 0x80190CE8u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80190CE8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = 32;  /* 0x80190CEC: 0x24040020 */
    if (cond_80190CE8) {
        psx_check_interrupts_at(cpu, 0x80190D28u);
        goto block_80190D28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190CF0u);
    }

block_80190CF0:
    psx_icache_fetch(cpu, 0x80190CF0u);
    v0 = psx_cyc_load_word(cpu, s1 + 40, 2, 0x20000u);  /* 0x80190CF0: 0x8E220028 */
    v1 = psx_cyc_load_word(cpu, s1 + 36, 3, 0x20000u);  /* 0x80190CF4: 0x8E230024 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190CF8: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x80190CFC: 0x00621821 */
    psx_icache_fetch(cpu, 0x80190D00u);
    psx_cyc_step(cpu, 0x18u);
    cpu->write_byte(v1, (uint8_t)a0);  /* 0x80190D00: 0xA0640000 */
    v1 = psx_cyc_load_word(cpu, s1 + 28, 3, 0x20000u);  /* 0x80190D04: 0x8E23001C */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x80190D08: 0x24420001 */
    psx_cyc_step(cpu, 0xCu);
    v1 = (int32_t)v1 < (int32_t)v0;  /* 0x80190D0C: 0x0062182A */
    psx_icache_fetch(cpu, 0x80190D10u);
    psx_cyc_step(cpu, 0x9u);
    int cond_80190D10 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20004u);
    cpu->write_word(s1 + 40, v0);  /* 0x80190D14: 0xAE220028 */
    if (cond_80190D10) {
        psx_check_interrupts_at(cpu, 0x80190A8Cu);
        goto block_80190A8C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190D18u);
    }

block_80190D18:
    psx_icache_fetch(cpu, 0x80190D18u);
    psx_cyc_step(cpu, 0x40000u);
    s2--;  /* 0x80190D18: 0x2652FFFF */
    psx_cyc_step(cpu, 0x40024u);
    v0 = (int32_t)a1 < (int32_t)s2;  /* 0x80190D1C: 0x00B2102A */
    psx_icache_fetch(cpu, 0x80190D20u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80190D20 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190D24: 0x00000000 */
    if (cond_80190D20) {
        psx_check_interrupts_at(cpu, 0x80190CF0u);
        goto block_80190CF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190D28u);
    }

block_80190D28:
    psx_icache_fetch(cpu, 0x80190D28u);
    psx_cyc_step(cpu, 0x20u);
    a1--;  /* 0x80190D28: 0x24A5FFFF */
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x80190D2C: 0x2402FFFF */
    psx_icache_fetch(cpu, 0x80190D30u);
    psx_cyc_step(cpu, 0x24u);
    int cond_80190D30 = (a1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190D34: 0x00000000 */
    if (cond_80190D30) {
        psx_check_interrupts_at(cpu, 0x80190D74u);
        goto block_80190D74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190D38u);
    }

block_80190D38:
    psx_icache_fetch(cpu, 0x80190D38u);
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x80190D38: 0x2406FFFF */
    /* fall through to block_80190D3C */

block_80190D3C:
    psx_icache_fetch(cpu, 0x80190D3Cu);
    v1 = psx_cyc_load_word(cpu, s1 + 40, 3, 0x20000u);  /* 0x80190D3C: 0x8E230028 */
    psx_icache_fetch(cpu, 0x80190D40u);
    v0 = psx_cyc_load_word(cpu, s1 + 36, 2, 0x20000u);  /* 0x80190D40: 0x8E220024 */
    a0 = psx_cyc_load_byte(cpu, s0, 4, 0x10000u);  /* 0x80190D44: 0x92040000 */
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x80190D48: 0x26100001 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x80190D4C: 0x00431021 */
    psx_icache_fetch(cpu, 0x80190D50u);
    psx_cyc_step(cpu, 0x14u);
    cpu->write_byte(v0, (uint8_t)a0);  /* 0x80190D50: 0xA0440000 */
    v0 = psx_cyc_load_word(cpu, s1 + 28, 2, 0x20000u);  /* 0x80190D54: 0x8E22001C */
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x80190D58: 0x24630001 */
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v0 < (int32_t)v1;  /* 0x80190D5C: 0x0043102A */
    psx_icache_fetch(cpu, 0x80190D60u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80190D60 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20008u);
    cpu->write_word(s1 + 40, v1);  /* 0x80190D64: 0xAE230028 */
    if (cond_80190D60) {
        psx_check_interrupts_at(cpu, 0x80190A8Cu);
        goto block_80190A8C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190D68u);
    }

block_80190D68:
    psx_icache_fetch(cpu, 0x80190D68u);
    psx_cyc_step(cpu, 0x20u);
    a1--;  /* 0x80190D68: 0x24A5FFFF */
    psx_cyc_step(cpu, 0x60u);
    int cond_80190D6C = (a1 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190D70u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190D70: 0x00000000 */
    if (cond_80190D6C) {
        psx_check_interrupts_at(cpu, 0x80190D3Cu);
        goto block_80190D3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190D74u);
    }

block_80190D74:
    psx_icache_fetch(cpu, 0x80190D74u);
    psx_cyc_step(cpu, 0x80000u);
    s3++;  /* 0x80190D74: 0x26730001 */
    a0 = (int32_t)(int8_t)psx_cyc_load_byte(cpu, s3, 4, 0x80000u);  /* 0x80190D78: 0x82640000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190D7C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80190D80u);
    psx_cyc_step(cpu, 0x11u);
    int cond_80190D80 = (a0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190D84: 0x00000000 */
    if (cond_80190D80) {
        psx_check_interrupts_at(cpu, 0x80190AF0u);
        goto block_80190AF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80190D88u);
    }

block_80190D88:
    psx_icache_fetch(cpu, 0x80190D88u);
    v0 = psx_cyc_load_word(cpu, s1 + 36, 2, 0x20000u);  /* 0x80190D88: 0x8E220024 */
    v1 = psx_cyc_load_word(cpu, s1 + 40, 3, 0x20000u);  /* 0x80190D8C: 0x8E230028 */
    psx_icache_fetch(cpu, 0x80190D90u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80190D90: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x80190D94: 0x00431021 */
    psx_cyc_step(cpu, 0x5u);
    cpu->write_byte(v0, (uint8_t)zero);  /* 0x80190D98: 0xA0400000 */
    v0 = psx_cyc_load_word(cpu, s1 + 40, 2, 0x20000u);  /* 0x80190D9C: 0x8E220028 */
    /* fall through to block_80190DA0 */

block_80190DA0:
    psx_icache_fetch(cpu, 0x80190DA0u);
    ra = psx_cyc_load_word(cpu, sp + 560, 31, 0x20000000u);  /* 0x80190DA0: 0x8FBF0230 */
    s5 = psx_cyc_load_word(cpu, sp + 556, 21, 0x20000000u);  /* 0x80190DA4: 0x8FB5022C */
    s4 = psx_cyc_load_word(cpu, sp + 552, 20, 0x20000000u);  /* 0x80190DA8: 0x8FB40228 */
    s3 = psx_cyc_load_word(cpu, sp + 548, 19, 0x20000000u);  /* 0x80190DAC: 0x8FB30224 */
    psx_icache_fetch(cpu, 0x80190DB0u);
    s2 = psx_cyc_load_word(cpu, sp + 544, 18, 0x20000000u);  /* 0x80190DB0: 0x8FB20220 */
    s1 = psx_cyc_load_word(cpu, sp + 540, 17, 0x20000000u);  /* 0x80190DB4: 0x8FB1021C */
    s0 = psx_cyc_load_word(cpu, sp + 536, 16, 0x20000000u);  /* 0x80190DB8: 0x8FB00218 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_80190DBC = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80190DC0u);
    psx_cyc_step(cpu, 0x20000000u);
    sp += 568;  /* 0x80190DC0: 0x27BD0238 */
    psx_check_interrupts_at(cpu, jt_80190DBC);
    cpu->pc = jt_80190DBC; return;  /* CPS: jr $ra */
}
