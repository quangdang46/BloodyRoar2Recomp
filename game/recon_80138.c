/* recon_80138.c — clean-room reconstruction of guest range
 * 0x80138000-0x8013FFFF (unowned gap between w5e01's 0x80137 band and
 * w6e00's 0x80134-0x80136 / w5e04's 0x80140+ bands).
 *
 * Faithful clean-C ports of functions still covered by cps_ transliterations
 * in game/cps_clean.c. Each port takes over its guest entry addr via
 * game/renames.map + game/clean_cps.map + gen_clean_table.py; the cps_<addr>
 * defs remain as unreachable shadow code.
 *
 * Coroutine-style ports keep the guest `psx_check_interrupts_at` yield points
 * and the cpu->pc continuation switch exactly like the cps_ bodies they
 * replace; the CLEAN_ENT_CPS wrapper hands the fn cpu->pc == the dispatched
 * addr on entry and converts an untouched cpu->pc back to $ra on return.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern int      br2_bail_pending(void);
extern int      clean_try_dispatch(CPUState *cpu, uint32_t addr);
extern void     psx_dispatch_call(CPUState *cpu, uint32_t target_addr,
                                  uint32_t return_addr);

/* MIPS unaligned word accessors — same contract as cps_clean.c's br2_lwl/lwr/
 * swl/swr (MMIO-aware cpu read/write path, no per-access cycle charge). */
static inline uint32_t wb_lwl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return (rt_value & 0x00FFFFFFu) | (word << 24);
        case 1: return (rt_value & 0x0000FFFFu) | (word << 16);
        case 2: return (rt_value & 0x000000FFu) | (word << 8);
        default: return word;
    }
}
static inline uint32_t wb_lwr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return word;
        case 1: return (rt_value & 0xFF000000u) | (word >> 8);
        case 2: return (rt_value & 0xFFFF0000u) | (word >> 16);
        default: return (rt_value & 0xFFFFFF00u) | (word >> 24);
    }
}
static inline void wb_swl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = (word & 0xFFFFFF00u) | (rt_value >> 24); break;
        case 1: word = (word & 0xFFFF0000u) | (rt_value >> 16); break;
        case 2: word = (word & 0xFF000000u) | (rt_value >> 8); break;
        default: word = rt_value; break;
    }
    cpu->write_word(aligned, word);
}
static inline void wb_swr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = rt_value; break;
        case 1: word = (word & 0x000000FFu) | (rt_value << 8); break;
        case 2: word = (word & 0x0000FFFFu) | (rt_value << 16); break;
        default: word = (word & 0x00FFFFFFu) | (rt_value << 24); break;
    }
    cpu->write_word(aligned, word);
}

/* wb_jal — faithful guest `jal` from clean C: dispatch the callee
 * synchronously (clean port when one exists, generated body otherwise),
 * return its v0. Same contract as ev.c's br2_jal. */
static uint32_t wb_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
        /* CPS callees tail-dispatch mid-chain: keep driving cpu->pc until
         * the coroutine returns to the link (see br2_clean_call_drive). */
        extern void br2_clean_call_drive(CPUState *, uint32_t);
        br2_clean_call_drive(cpu, link);
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

/* MIPS GPR names — file-local aliases for cpu->gpr[N], identical to
 * cps_clean.c's block so the transliterated bodies below read the same. */
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

/* ======== popup_field_update (guest 0x8013AB40) ========
 * br2eu_func_8013AB40 — 3-piece CPS coroutine (entry + continuation addrs
 * 0x8013AB48/50/64 inside the entry def, split pieces 0x8013AB68/7C).
 * Halfword field updates on the s0 record:
 *   sh v1, 0x14(s0)                 0x8013AB44 (delay slot, always)
 *   if (v0 != 0) sh 160, 0x18(s0)   via block 0x8013AB48
 *   h = lh 0x0C(s0); if (h < 15) sh 15, 0x0C(s0)   (delay: v0 = 15)
 *   sh (lhu 0x06(s0) + lhu 0x08(s0)), 0x06(s0)
 * epilogue: restore ra/s0, sp += 32, jr ra.
 * s0/ra/sp frame is owned by the caller-side prologue outside this cluster;
 * only the epilogue ops shown here are guest-faithful. */
uint32_t popup_field_update_cps(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;
    uint32_t pc = cpu->pc;
    if (pc != 0u && pc != 0x8013ab40u) {
        switch (pc) {
        case 0x8013ab48u: goto b48;
        case 0x8013ab50u: goto b50;
        case 0x8013ab64u: goto b64;
        case 0x8013ab68u: goto b68;
        case 0x8013ab7cu: goto b7c;
        default: break;
        }
    }

    /* block_8013AB40 — cond = (v0 == 0); delay: sh v1, 0x14(s0) */
    {
        int cond = (cpu->gpr[2] == cpu->gpr[0]);
        cpu->write_half(cpu->gpr[16] + 20u, (uint16_t)cpu->gpr[3]);
        if (cond) {
            psx_check_interrupts_at(cpu, 0x8013ab50u);
            goto b50;
        } else {
            psx_check_interrupts_at(cpu, 0x8013ab48u);
            goto b48;
        }
    }

b48:
    cpu->gpr[2] = 160u;
    cpu->write_half(cpu->gpr[16] + 24u, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8013AB50 */

b50:
    /* h = lh 0x0C(s0); cond = !(h < 15); delay: v0 = 15 */
    {
        int32_t h = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 12u);
        int cond = !(h < 15);
        cpu->gpr[2] = 15u;
        if (cond) {
            psx_check_interrupts_at(cpu, 0x8013ab68u);
            goto b68;
        } else {
            psx_check_interrupts_at(cpu, 0x8013ab64u);
            goto b64;
        }
    }

b64:
    cpu->write_half(cpu->gpr[16] + 12u, (uint16_t)cpu->gpr[2]);
    /* cps_ edge: cpu->pc = 0x8013ab68; return — intra-function, goto */

b68:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 6u);   /* lhu 0x06(s0) */
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 8u);   /* lhu 0x08(s0) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];           /* v0 += v1 */
    cpu->write_half(cpu->gpr[16] + 6u, (uint16_t)cpu->gpr[2]);
    /* cps_ edge: cpu->pc = 0x8013ab7c; return — intra-function, goto */

b7c:
    /* epilogue: lw ra,0x1C(sp); lw s0,0x18(sp); delay: addiu sp,sp,0x20 */
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 28u);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 24u);
    {
        uint32_t jt = cpu->gpr[31];
        cpu->gpr[29] = cpu->gpr[29] + 32u;
        psx_check_interrupts_at(cpu, jt);
        cpu->pc = jt;               /* jr ra — explicit so a stale
                                       continuation pc can't survive */
    }
    return cpu->gpr[2];
}

/* ======== anim_workblock_tick (guest 0x8013F828) ========
 * Per-frame task callback spawned by anim_workblock_init
 * (task_node_alloc(1,2,0xc,0x8013f828,0,0x978) in menu.c) — advances the
 * 0x978-byte animation work-block record in a0 through its GTE vertex
 * transform / halfword channel-shuffle states.
 *
 * Atomic clean port of the whole 9-def CPS cluster
 * (0x8013F828..0x801401C8: cps_8013f828/f888/f904/f914/fc38/fd2c/fd48/
 * fea0/80140198, 65 clean_cps.map rows). The guest body is one function
 * with a 64-byte frame (s0-s7/fp/ra saved at sp+24..60, scratch spills at
 * sp+16/sp+20) — reproduced literally so live guest state at every
 * psx_check_interrupts_at edge is identical to generated execution; an
 * IRQ there resumes through the generated piece at resume_pc, exactly
 * like the CPS coroutine did. The four guest `jal`s run synchronously
 * through wb_jal (all four callees have clean ports: task_slot_step,
 * gte_rtps3_sxy_pack_ctrl x2, gte_rtps3_sxy_pack, gte_xform_accum).
 * Entry-only: no cpu->pc continuation switch — the ordinary clean
 * wrapper forces pc=$ra on return, matching the guest `jr $ra` epilogue. */
uint32_t anim_workblock_tick(uint32_t a0_, uint32_t a1_, uint32_t a2_, uint32_t a3_)
{
    (void)a0_; (void)a1_; (void)a2_; (void)a3_;
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;

block_8013F828:
    v0 = 0x1F800000u;
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 130);      /* lh 0x1F800082 gate */
    sp -= 64;
    cpu->write_word(sp + 48, s6);
    s6 = a0;
    cpu->write_word(sp + 60, ra);
    cpu->write_word(sp + 56, fp);
    cpu->write_word(sp + 52, s7);
    cpu->write_word(sp + 44, s5);
    cpu->write_word(sp + 40, s4);
    cpu->write_word(sp + 36, s3);
    cpu->write_word(sp + 32, s2);
    cpu->write_word(sp + 28, s1);
    int cond_8013F85C = (v0 != zero);
    cpu->write_word(sp + 24, s0);                        /* delay slot */
    if (cond_8013F85C) {
        psx_check_interrupts_at(cpu, 0x8013FD48u);
        goto block_8013FD48;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F864u);
        goto block_8013F864;
    }

block_8013F864:
    v0 = 0x801F0000u;
    v1 = (int32_t)(int16_t)cpu->read_half(s6 + 2414);    /* lh +0x96E */
    v0 += 2776;                                          /* -> 0x801F0AD8 */
    v1 += v0;
    v0 = cpu->read_byte(v1);
    int cond_8013F87C = (v0 != zero);
    if (cond_8013F87C) {
        psx_check_interrupts_at(cpu, 0x8013F904u);
        goto block_8013F904;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F884u);
        goto block_8013F884;
    }

block_8013F884:
    v0 = (int32_t)(int16_t)cpu->read_half(s6 + 2410);    /* lh +0x96A */
    goto block_8013F888;                                 /* piece fallthrough */

block_8013F888:
    ;
    int cond_8013F88C = (v0 != zero);
    if (cond_8013F88C) {
        psx_check_interrupts_at(cpu, 0x8013F8CCu);
        goto block_8013F8CC;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F894u);
        goto block_8013F894;
    }

block_8013F894:
    s5 = zero;
    v1 = s6;
    /* fall through */

block_8013F89C:
    t2 = wb_lwl(cpu, v1 + 1743, t2);
    t2 = wb_lwr(cpu, v1 + 1740, t2);
    t7 = wb_lwl(cpu, v1 + 1747, t7);
    t7 = wb_lwr(cpu, v1 + 1744, t7);
    wb_swl(cpu, v1 + 1735, t2);
    wb_swr(cpu, v1 + 1732, t2);
    wb_swl(cpu, v1 + 1739, t7);
    wb_swr(cpu, v1 + 1736, t7);
    s5++;
    v0 = (int32_t)s5 < 3;
    int cond_8013F8C4 = (v0 != zero);
    v1 += 144;                                          /* delay slot */
    if (cond_8013F8C4) {
        psx_check_interrupts_at(cpu, 0x8013F89Cu);
        goto block_8013F89C;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F8CCu);
        goto block_8013F8CC;
    }

block_8013F8CC:
    v0 = cpu->read_half(s6 + 2410);                     /* lhu +0x96A */
    v0--;
    cpu->write_half(s6 + 2410, (uint16_t)v0);
    v0 <<= 16;
    int cond_8013F8E0 = ((int32_t)v0 > 0);
    if (cond_8013F8E0) {
        psx_check_interrupts_at(cpu, 0x8013F914u);
        goto block_8013F914;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F8E8u);
        goto block_8013F8E8;
    }

block_8013F8E8:
    v0 = cpu->read_half(s6 + 2418);                     /* lhu +0x972 */
    v0--;
    cpu->write_half(s6 + 2418, (uint16_t)v0);
    v0 <<= 16;
    int cond_8013F8FC = ((int32_t)v0 >= 0);
    if (cond_8013F8FC) {
        psx_check_interrupts_at(cpu, 0x8013FEA0u);
        goto block_8013FEA0;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F904u);
        goto block_8013F904;
    }

block_8013F904:
    wb_jal(0x801A482Cu, 0x8013F90Cu);                   /* jal task_slot_step */
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8013F90C;

block_8013F90C:
    psx_check_interrupts_at(cpu, 0x80140198u);          /* j epilogue */
    goto block_80140198;

block_8013F914:
    v0 = (int32_t)(int16_t)cpu->read_half(s6 + 2412);   /* lh +0x96C */
    v1 = cpu->read_half(s6 + 2412);                     /* lhu +0x96C */
    int cond_8013F91C = ((int32_t)v0 <= 0);
    v0 = v1 + -1;                                       /* delay slot */
    if (cond_8013F91C) {
        psx_check_interrupts_at(cpu, 0x8013F92Cu);
        goto block_8013F92C;
    } else {
        cpu->write_half(s6 + 2412, (uint16_t)v0);       /* block_8013F924 */
        psx_check_interrupts_at(cpu, 0x80140198u);      /* j epilogue */
        goto block_80140198;
    }

block_8013F92C:
    v0 = cpu->read_half(s6 + 2408);                     /* lhu +0x968 */
    v1 = (int32_t)(int16_t)cpu->read_half(s6 + 2412);   /* lh +0x96C */
    v0++;
    int cond_8013F938 = (v1 != zero);
    cpu->write_half(s6 + 2408, (uint16_t)v0);           /* delay slot */
    if (cond_8013F938) {
        psx_check_interrupts_at(cpu, 0x8013FA44u);
        goto block_8013FA44;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F940u);
        goto block_8013F940;
    }

block_8013F940:
    s0 = 0x1F800000u;
    v0 = (int32_t)(int16_t)cpu->read_half(s6 + 2416);   /* lh +0x970 */
    int cond_8013F94C = (v0 == zero);
    s0 |= 0x03F4;                                       /* delay slot -> 0x1F8003F4 */
    if (cond_8013F94C) {
        psx_check_interrupts_at(cpu, 0x8013F968u);
        goto block_8013F968;
    } else {
        psx_check_interrupts_at(cpu, 0x8013F954u);
        goto block_8013F954;
    }

block_8013F954:
    a0 = cpu->read_word(s6 + 2364);                     /* lw +0x93C */
    a1 = s6 + 2376;                                     /* delay slot +0x948 */
    wb_jal(0x80194F40u, 0x8013F960u);                   /* jal gte_rtps3_sxy_pack_ctrl */
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8013F960;

block_8013F960:
    s2 = s6 + 2164;                                     /* +0x874 */
    psx_check_interrupts_at(cpu, 0x8013F99Cu);          /* j */
    goto block_8013F99C;

block_8013F968:
    t2 = cpu->read_word(s6 + 2364);                     /* lw +0x93C */
    t4 = cpu->read_word(t2);
    t5 = cpu->read_word(t2 + 4);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[0] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[1] = t5;
    t4 = cpu->read_word(t2 + 8);
    t5 = cpu->read_word(t2 + 12);
    t6 = cpu->read_word(t2 + 16);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[2] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[3] = t5;
    psx_gte_stall(cpu);
    gte_write_ctrl(cpu, 4, t6);
    s2 = s6 + 2164;                                     /* +0x874 */
    /* fall through */

block_8013F99C:
    s4 = cpu->read_word(s6 + 2368);                     /* lw +0x940 */
    s5 = zero;
    /* fall through */

block_8013F9A4:
    s3 = zero;
    a0 = s2 + 4;
    /* fall through */

block_8013F9AC:
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((s4)));
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((s4 + 4))));
    gte_execute(cpu, 0x0486012);                        /* gte cmd 0x12 RTPS */
    psx_gte_stall(cpu);
    cpu->write_word((s0), (cpu->gte_data[25]));
    gte_precision_store_word((s0), 25);
    psx_gte_stall(cpu);
    cpu->write_word((s0 + 4), (cpu->gte_data[26]));
    gte_precision_store_word((s0 + 4), 26);
    psx_gte_stall(cpu);
    cpu->write_word((s0 + 8), (cpu->gte_data[27]));
    gte_precision_store_word((s0 + 8), 27);
    v1 = cpu->read_word(s6 + 2364);
    v0 = cpu->read_half(s0);
    v1 = cpu->read_half(v1 + 20);
    s4 += 8;
    v0 += v1;
    cpu->write_half(a0 + 12, (uint16_t)v0);
    cpu->write_half(s2, (uint16_t)v0);
    v1 = cpu->read_word(s6 + 2364);
    v0 = cpu->read_half(s0 + 4);
    v1 = cpu->read_half(v1 + 24);
    s3++;
    v0 += v1;
    cpu->write_half(a0 + 14, (uint16_t)v0);
    cpu->write_half(a0 + -2, (uint16_t)v0);
    v1 = cpu->read_word(s6 + 2364);
    v0 = cpu->read_half(s0 + 8);
    v1 = cpu->read_half(v1 + 28);
    s2 += 8;
    v0 += v1;
    cpu->write_half(a0 + 16, (uint16_t)v0);
    cpu->write_half(a0, (uint16_t)v0);
    v0 = (int32_t)s3 < 2;
    int cond_8013FA24 = (v0 != zero);
    a0 += 8;                                            /* delay slot */
    if (cond_8013FA24) {
        psx_check_interrupts_at(cpu, 0x8013F9ACu);
        goto block_8013F9AC;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FA2Cu);
        goto block_8013FA2C;
    }

block_8013FA2C:
    s5++;
    v0 = (int32_t)s5 < 3;
    int cond_8013FA34 = (v0 != zero);
    s2 += 32;                                           /* delay slot */
    if (cond_8013FA34) {
        psx_check_interrupts_at(cpu, 0x8013F9A4u);
        goto block_8013F9A4;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FA3Cu);
        goto block_8013FA3C;
    }

block_8013FA3C:
    v0 = -1;
    cpu->write_half(s6 + 2412, (uint16_t)v0);           /* +0x96C = 0xFFFF */
    /* fall through */

block_8013FA44:
    s5 = zero;
    a2 = s6;
    /* fall through */

block_8013FA4C:
    a1 = cpu->read_word(a2 + 1696);                     /* +0x6A0 */
    v0 = cpu->read_word(a2 + 1692);                     /* +0x69C */
    v1 = cpu->read_word(a2 + 1688);                     /* +0x698 */
    a0 = cpu->read_word(a2 + 1684);                     /* +0x694 */
    s5++;
    cpu->write_word(a2 + 1696, v0);
    cpu->write_word(a2 + 1692, v1);
    cpu->write_word(a2 + 1688, a0);
    cpu->write_word(a2 + 1684, a1);                     /* 4-word rotate */
    v0 = (int32_t)s5 < 3;
    int cond_8013FA74 = (v0 != zero);
    a2 += 16;                                           /* delay slot */
    if (cond_8013FA74) {
        psx_check_interrupts_at(cpu, 0x8013FA4Cu);
        goto block_8013FA4C;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FA7Cu);
        goto block_8013FA7C;
    }

block_8013FA7C:
    s2 = s6 + 2164;                                     /* +0x874 */
    s5 = (int32_t)(int16_t)cpu->read_half(s6 + 2362);   /* lh +0x93A */
    v0 = cpu->read_half(s6 + 2356);                     /* lhu +0x934 */
    v1 = cpu->read_half(s6 + 2360);                     /* lhu +0x938 */
    a0 = cpu->read_half(s6 + 2358);                     /* lhu +0x936 */
    a1 = s6 + 2188;                                     /* +0x88C */
    cpu->write_half(s6 + 2356, (uint16_t)s5);           /* rotate 4 halfword ctrs */
    s5 = zero;
    cpu->write_half(s6 + 2362, (uint16_t)v1);
    cpu->write_half(s6 + 2360, (uint16_t)a0);
    cpu->write_half(s6 + 2358, (uint16_t)v0);
    /* fall through */

block_8013FAA8:
    t7 = wb_lwl(cpu, a1 + -5, t7);
    t7 = wb_lwr(cpu, a1 + -8, t7);
    t8 = wb_lwl(cpu, a1 + -1, t8);
    t8 = wb_lwr(cpu, a1 + -4, t8);
    wb_swl(cpu, a1 + 11, t7);
    wb_swr(cpu, a1 + 8, t7);
    wb_swl(cpu, a1 + 15, t8);
    wb_swr(cpu, a1 + 12, t8);
    t7 = wb_lwl(cpu, a1 + 3, t7);
    t7 = wb_lwr(cpu, a1, t7);
    t8 = wb_lwl(cpu, a1 + 7, t8);
    t8 = wb_lwr(cpu, a1 + 4, t8);
    wb_swl(cpu, a1 + 19, t7);
    wb_swr(cpu, a1 + 16, t7);
    wb_swl(cpu, a1 + 23, t8);
    wb_swr(cpu, a1 + 20, t8);
    t7 = wb_lwl(cpu, s2 + 3, t7);
    t7 = wb_lwr(cpu, s2, t7);
    t8 = wb_lwl(cpu, s2 + 7, t8);
    t8 = wb_lwr(cpu, s2 + 4, t8);
    wb_swl(cpu, a1 + -5, t7);
    wb_swr(cpu, a1 + -8, t7);
    wb_swl(cpu, a1 + -1, t8);
    wb_swr(cpu, a1 + -4, t8);
    t7 = wb_lwl(cpu, a1 + -13, t7);
    t7 = wb_lwr(cpu, a1 + -16, t7);
    t8 = wb_lwl(cpu, a1 + -9, t8);
    t8 = wb_lwr(cpu, a1 + -12, t8);
    wb_swl(cpu, a1 + 3, t7);
    wb_swr(cpu, a1, t7);
    wb_swl(cpu, a1 + 7, t8);
    wb_swr(cpu, a1 + 4, t8);
    a1 += 48;
    s5++;
    v0 = (int32_t)s5 < 3;
    int cond_8013FB34 = (v0 != zero);
    s2 += 48;                                           /* delay slot */
    if (cond_8013FB34) {
        psx_check_interrupts_at(cpu, 0x8013FAA8u);
        goto block_8013FAA8;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FB3Cu);
        goto block_8013FB3C;
    }

block_8013FB3C:
    v0 = (int32_t)(int16_t)cpu->read_half(s6 + 2416);   /* lh +0x970 */
    s4 = cpu->read_word(s6 + 2368);                     /* lw +0x940 */
    int cond_8013FB44 = (v0 == zero);
    s2 = s6 + 2164;                                     /* delay slot +0x874 */
    if (cond_8013FB44) {
        psx_check_interrupts_at(cpu, 0x8013FB60u);
        goto block_8013FB60;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FB4Cu);
        goto block_8013FB4C;
    }

block_8013FB4C:
    a0 = cpu->read_word(s6 + 2364);                     /* lw +0x93C */
    a1 = s6 + 2376;                                     /* delay slot +0x948 */
    wb_jal(0x80194F40u, 0x8013FB58u);                   /* jal gte_rtps3_sxy_pack_ctrl */
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8013FB58;

block_8013FB58:
    a1 = 0x1F800000u;                                   /* delay slot */
    psx_check_interrupts_at(cpu, 0x8013FB94u);          /* j */
    goto block_8013FB94;

block_8013FB60:
    t7 = cpu->read_word(s6 + 2364);                     /* lw +0x93C */
    t4 = cpu->read_word(t7);
    t5 = cpu->read_word(t7 + 4);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[0] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[1] = t5;
    t4 = cpu->read_word(t7 + 8);
    t5 = cpu->read_word(t7 + 12);
    t6 = cpu->read_word(t7 + 16);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[2] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[3] = t5;
    psx_gte_stall(cpu);
    gte_write_ctrl(cpu, 4, t6);
    a1 = 0x1F800000u;
    /* fall through */

block_8013FB94:
    a1 |= 0x03F4;                                       /* -> 0x1F8003F4 */
    s5 = zero;
    /* fall through */

block_8013FB9C:
    s3 = zero;
    a0 = s2 + 4;
    /* fall through */

block_8013FBA4:
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((s4)));
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((s4 + 4))));
    gte_execute(cpu, 0x0486012);                        /* gte cmd 0x12 RTPS */
    psx_gte_stall(cpu);
    cpu->write_word((a1), (cpu->gte_data[25]));
    gte_precision_store_word((a1), 25);
    psx_gte_stall(cpu);
    cpu->write_word((a1 + 4), (cpu->gte_data[26]));
    gte_precision_store_word((a1 + 4), 26);
    psx_gte_stall(cpu);
    cpu->write_word((a1 + 8), (cpu->gte_data[27]));
    gte_precision_store_word((a1 + 8), 27);
    v1 = cpu->read_word(s6 + 2364);
    v0 = cpu->read_half(a1);
    v1 = cpu->read_half(v1 + 20);
    s4 += 8;
    v0 += v1;
    cpu->write_half(s2, (uint16_t)v0);
    v1 = cpu->read_word(s6 + 2364);
    v0 = cpu->read_half(a1 + 4);
    v1 = cpu->read_half(v1 + 24);
    s3++;
    v0 += v1;
    cpu->write_half(a0 + -2, (uint16_t)v0);
    v1 = cpu->read_word(s6 + 2364);
    v0 = cpu->read_half(a1 + 8);
    v1 = cpu->read_half(v1 + 28);
    s2 += 8;
    v0 += v1;
    cpu->write_half(a0, (uint16_t)v0);
    v0 = (int32_t)s3 < 2;
    int cond_8013FC10 = (v0 != zero);
    a0 += 8;                                            /* delay slot */
    if (cond_8013FC10) {
        psx_check_interrupts_at(cpu, 0x8013FBA4u);
        goto block_8013FBA4;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FC18u);
        goto block_8013FC18;
    }

block_8013FC18:
    s5++;
    v0 = (int32_t)s5 < 3;
    int cond_8013FC20 = (v0 != zero);
    s2 += 32;                                           /* delay slot */
    if (cond_8013FC20) {
        psx_check_interrupts_at(cpu, 0x8013FB9Cu);
        goto block_8013FB9C;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FC28u);
        goto block_8013FC28;
    }

block_8013FC28:
    s4 = s6 + 2164;                                     /* +0x874 */
    s5 = zero;
    a1 = s6;
    a0 = s6 + 2192;                                     /* +0x890 */
    /* fall through */

block_8013FC38:
    s2 = cpu->read_word(a1 + 1684);                     /* lw +0x694 ptr */
    t8 = wb_lwl(cpu, s4 + 3, t8);
    t8 = wb_lwr(cpu, s4, t8);
    t9 = wb_lwl(cpu, s4 + 7, t9);
    t9 = wb_lwr(cpu, s4 + 4, t9);
    wb_swl(cpu, s2 + 3, t8);
    wb_swr(cpu, s2, t8);
    wb_swl(cpu, s2 + 7, t9);
    wb_swr(cpu, s2 + 4, t9);
    t8 = wb_lwl(cpu, a0 + -17, t8);
    t8 = wb_lwr(cpu, a0 + -20, t8);
    t9 = wb_lwl(cpu, a0 + -13, t9);
    t9 = wb_lwr(cpu, a0 + -16, t9);
    wb_swl(cpu, s2 + 11, t8);
    wb_swr(cpu, s2 + 8, t8);
    wb_swl(cpu, s2 + 15, t9);
    wb_swr(cpu, s2 + 12, t9);
    v0 = (int32_t)(int16_t)cpu->read_half(s4);
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + -12);
    v0 += v1;
    v1 = (uint32_t)v0 >> 31;
    v0 += v1;
    v0 = (int32_t)v0 >> 1;                              /* avg -> s2+16 */
    cpu->write_half(s2 + 16, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + -26);
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + -10);
    v0 += v1;
    v1 = (uint32_t)v0 >> 31;
    v0 += v1;
    v0 = (int32_t)v0 >> 1;
    cpu->write_half(s2 + 18, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + -24);
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + -8);
    a1 += 16;
    v0 += v1;
    v1 = (uint32_t)v0 >> 31;
    v0 += v1;
    v0 = (int32_t)v0 >> 1;
    cpu->write_half(s2 + 20, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + -20);
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + -4);
    s5++;
    v0 += v1;
    v1 = (uint32_t)v0 >> 31;
    v0 += v1;
    v0 = (int32_t)v0 >> 1;
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + -18);
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + -2);
    s4 += 48;
    v0 += v1;
    v1 = (uint32_t)v0 >> 31;
    v0 += v1;
    v0 = (int32_t)v0 >> 1;
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = (int32_t)(int16_t)cpu->read_half(a0 + -16);
    v1 = (int32_t)(int16_t)cpu->read_half(a0);
    v0 += v1;
    /* block_8013FD2C */
    v1 = (uint32_t)v0 >> 31;
    v0 += v1;
    v0 = (int32_t)v0 >> 1;
    cpu->write_half(s2 + 28, (uint16_t)v0);
    v0 = (int32_t)s5 < 3;
    int cond_8013FD40 = (v0 != zero);
    a0 += 48;                                           /* delay slot */
    if (cond_8013FD40) {
        psx_check_interrupts_at(cpu, 0x8013FC38u);
        goto block_8013FC38;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FD48u);
        goto block_8013FD48;
    }

block_8013FD48:
    s2 = s6 + 2164;                                     /* +0x874 */
    t8 = 0x801B1860u;
    t4 = cpu->read_word(t8);
    t5 = cpu->read_word(t8 + 4);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[0] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[1] = t5;
    t4 = cpu->read_word(t8 + 8);
    t5 = cpu->read_word(t8 + 12);
    t6 = cpu->read_word(t8 + 16);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[2] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[3] = t5;
    psx_gte_stall(cpu);
    gte_write_ctrl(cpu, 4, t6);
    s0 = 0x1F800000u;
    s0 |= 0x03E0;                                       /* -> 0x1F8003E0 */
    s5 = zero;
    a1 = s6;
    /* fall through */

block_8013FD8C:
    s3 = zero;
    s1 = s2 + 36;
    fp = cpu->read_word(a1 + 1688);                     /* lw +0x698 */
    s4 = cpu->read_word(a1 + 1684);                     /* lw +0x694 */
    s7 = fp + 4;
    s4 += 36;
    /* fall through */

block_8013FDA4:
    v0 = cpu->read_half(s2);
    cpu->write_half(s0, (uint16_t)v0);
    v0 = cpu->read_half(s1 + -20);
    cpu->write_half(s0 + 6, (uint16_t)v0);
    v0 = cpu->read_half(s1 + -4);
    cpu->write_half(s0 + 12, (uint16_t)v0);
    v0 = cpu->read_half(s1 + -34);
    cpu->write_half(s0 + 2, (uint16_t)v0);
    v0 = cpu->read_half(s1 + -18);
    cpu->write_half(s0 + 8, (uint16_t)v0);
    v0 = cpu->read_half(s1 + -2);
    cpu->write_half(s0 + 14, (uint16_t)v0);
    v0 = cpu->read_half(s1 + -32);
    cpu->write_half(s0 + 4, (uint16_t)v0);
    v0 = cpu->read_half(s1 + -16);
    cpu->write_half(s0 + 10, (uint16_t)v0);
    v0 = cpu->read_half(s1);
    a0 = s0;
    cpu->write_half(s0 + 16, (uint16_t)v0);
    cpu->write_word(sp + 16, a1);                       /* delay slot spill */
    wb_jal(0x80194A70u, 0x8013FE18u);                   /* jal gte_rtps3_sxy_pack */
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8013FE18;

block_8013FE18:
    v0 = cpu->read_half(s0);
    cpu->write_half(s4 + -4, (uint16_t)v0);
    v0 = cpu->read_half(s0 + 2);
    s2 += 8;
    cpu->write_half(s4 + -2, (uint16_t)v0);
    v0 = cpu->read_half(s0 + 4);
    cpu->write_half(s4, (uint16_t)v0);
    v0 = 12;
    v1 = (int32_t)(int16_t)cpu->read_half(s6 + 2356);   /* lh +0x934 */
    a1 = cpu->read_word(sp + 16);                       /* reload spill */
    int cond_8013FE48 = (v1 != v0);
    s1 += 8;                                            /* delay slot */
    if (cond_8013FE48) {
        psx_check_interrupts_at(cpu, 0x8013FE74u);
        goto block_8013FE74;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FE50u);
        goto block_8013FE50;
    }

block_8013FE50:
    v0 = cpu->read_half(s0);
    cpu->write_half(fp, (uint16_t)v0);
    v0 = cpu->read_half(s0 + 2);
    cpu->write_half(s7 + -2, (uint16_t)v0);
    v0 = cpu->read_half(s0 + 4);
    cpu->write_half(s7, (uint16_t)v0);
    /* fall through */

block_8013FE74:
    s4 += 8;
    s7 += 8;
    s3++;
    v0 = (int32_t)s3 < 2;
    int cond_8013FE84 = (v0 != zero);
    fp += 8;                                            /* delay slot */
    if (cond_8013FE84) {
        psx_check_interrupts_at(cpu, 0x8013FDA4u);
        goto block_8013FDA4;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FE8Cu);
        goto block_8013FE8C;
    }

block_8013FE8C:
    s2 += 32;
    s5++;
    v0 = (int32_t)s5 < 3;
    int cond_8013FE98 = (v0 != zero);
    a1 += 16;                                           /* delay slot */
    if (cond_8013FE98) {
        psx_check_interrupts_at(cpu, 0x8013FD8Cu);
        goto block_8013FD8C;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FEA0u);
        goto block_8013FEA0;
    }

block_8013FEA0:
    v0 = 0x1F800000u;
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 130);    /* lh 0x1F800082 gate */
    int cond_8013FEAC = (v0 != zero);
    v0 = 0x801C0000u;                                   /* delay slot */
    if (cond_8013FEAC) {
        psx_check_interrupts_at(cpu, 0x8013FF80u);
        goto block_8013FF80;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FEB4u);
        goto block_8013FEB4;
    }

block_8013FEB4:
    v0 = (int32_t)(int16_t)cpu->read_half(s6 + 2410);   /* lh +0x96A */
    int cond_8013FEBC = ((int32_t)v0 >= 0);
    v0 = 0x801C0000u;                                   /* delay slot */
    if (cond_8013FEBC) {
        psx_check_interrupts_at(cpu, 0x8013FF80u);
        goto block_8013FF80;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FEC4u);
        goto block_8013FEC4;
    }

block_8013FEC4:
    s2 = 0x1F800000u;
    s2 |= 0x03F8;                                       /* -> 0x1F8003F8 */
    s4 = zero;
    v0 = 0x801C0000u;
    v0 = cpu->read_word(v0 + -4352);                    /* lw 0x801BEF00 idx cell */
    s3 = cpu->read_word(s6 + 2372);                     /* lw +0x944 */
    a0 = cpu->read_half(s6 + 2422);                     /* lhu +0x976 */
    v1 = v0 << 3;
    v1 -= v0;
    v0 = v1 << 4;
    v0 -= v1;
    v0 <<= 3;                                           /* idx*840 */
    v0 += 4;
    v0 = s6 + v0;
    v1 = cpu->read_half(s6 + 2420);                     /* lhu +0x974 */
    s1 = v0 + 564;                                      /* +0x234 -> record base */
    v1 -= a0;
    cpu->write_half(s6 + 2420, (uint16_t)v1);
    /* fall through */

block_8013FF0C:
    a0 = s2;
    a1 = s3;
    s3 += 8;
    s4++;
    cpu->write_half(s2, (uint16_t)zero);
    cpu->write_half(s2 + 2, (uint16_t)zero);
    cpu->write_half(s2 + 4, (uint16_t)zero);
    a3 = (int32_t)(int16_t)cpu->read_half(s6 + 2420);   /* lh +0x974 */
    s0 = cpu->read_word(s1 + -560);
    a2 = 1;                                             /* delay slot */
    wb_jal(0x80194840u, 0x8013FF38u);                   /* jal gte_xform_accum */
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8013FF38;

block_8013FF38:
    a0 = 0xFF000000u;
    a0 = s0 & a0;
    v0 = (int32_t)(int16_t)cpu->read_half(s2);
    v1 = cpu->read_half(s2 + 2);
    a0 += v0;
    v1 <<= 16;
    v1 = (int32_t)v1 >> 8;
    v0 = cpu->read_half(s2 + 4);
    a0 += v1;
    v0 <<= 16;
    s0 = a0 + v0;
    cpu->write_word(s1 + -560, s0);
    cpu->write_word(s1 + -280, s0);
    cpu->write_word(s1, s0);
    v0 = (int32_t)s4 < 7;
    int cond_8013FF74 = (v0 != zero);
    s1 += 40;                                           /* delay slot */
    if (cond_8013FF74) {
        psx_check_interrupts_at(cpu, 0x8013FF0Cu);
        goto block_8013FF0C;
    } else {
        psx_check_interrupts_at(cpu, 0x8013FF7Cu);
        goto block_8013FF7C;
    }

block_8013FF7C:
    v0 = 0x801C0000u;
    /* fall through */

block_8013FF80:
    v0 -= 6088;                                         /* -> 0x801BE838 ptr cell */
    t9 = cpu->read_word(v0);
    t4 = cpu->read_word(t9);
    t5 = cpu->read_word(t9 + 4);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[0] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[1] = t5;
    t4 = cpu->read_word(t9 + 8);
    t5 = cpu->read_word(t9 + 12);
    t6 = cpu->read_word(t9 + 16);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[2] = t4;
    psx_gte_stall(cpu);
    cpu->gte_ctrl[3] = t5;
    psx_gte_stall(cpu);
    gte_write_ctrl(cpu, 4, t6);
    t4 = cpu->read_word(t9 + 20);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[5] = t4;
    t5 = cpu->read_word(t9 + 24);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[6] = t5;
    t6 = cpu->read_word(t9 + 28);
    psx_gte_stall(cpu);
    cpu->gte_ctrl[7] = t6;
    s1 = 0x1F800000u;
    s1 |= 0x03B8;                                       /* -> 0x1F8003B8 */
    s2 = 0x1F800000u;
    s2 |= 0x03A0;                                       /* -> 0x1F8003A0 */
    t5 = s6 + 1732;                                     /* +0x6C4 */
    s7 = zero;
    s5 = 3;
    s4 = 2063;                                          /* clamp hi */
    s0 = 0x801C0000u;
    v0 = 0x801C0000u;
    v0 = cpu->read_word(v0 + -4352);                    /* lw 0x801BEF00 idx cell */
    s3 = 0x09000000u;
    v1 = v0 << (s5 & 0x1F);
    v1 -= v0;
    v0 = v1 << 4;
    v0 -= v1;
    v0 <<= (s5 & 0x1F);
    v0 += 4;
    t3 = s6 + v0;                                       /* idx*840+4 record ptr */
    /* fall through */

block_80140018:
    a3 = s1;
    t1 = zero;
    a0 = s2;
    /* fall through */

block_80140024:
    v1 = t5 + 8;
    v0 = t5 + 16;
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((t5)));
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((t5 + 4))));
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (cpu->read_word((v1)));
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (cpu->read_word((v1 + 4))));
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (cpu->read_word((v0)));
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (cpu->read_word((v0 + 4))));
    gte_execute(cpu, 0x0280030);                        /* gte cmd 0x30 */
    v1 = a3 + 4;
    v0 = a3 + 8;
    psx_gte_stall(cpu);
    cpu->write_word((a3), (cpu->gte_data[12]));
    gte_precision_store_word((a3), 12);
    psx_gte_stall(cpu);
    cpu->write_word((v1), (cpu->gte_data[13]));
    gte_precision_store_word((v1), 13);
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[14]));
    gte_precision_store_word((v0), 14);
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);                        /* mfc2 */
    t4 = (int32_t)t4 >> 2;
    cpu->write_word(a0, t4);
    v1 = cpu->read_word(a0);
    v0 = (int32_t)v1 < 3;
    int cond_80140080 = (v0 == zero);                   /* v1 >= 3 -> clamp check */
    v0 = (int32_t)v1 < 2063;                            /* delay slot */
    if (cond_80140080) {
        psx_check_interrupts_at(cpu, 0x80140090u);
        goto block_80140090;
    } else {
        cpu->write_word(a0, s5);                        /* block_80140088: store 3 */
        psx_check_interrupts_at(cpu, 0x8014009Cu);      /* j */
        goto block_8014009C;
    }

block_80140090:
    ;
    int cond_80140090 = (v0 != zero);                   /* v1 < 2063 */
    if (cond_80140090) {
        psx_check_interrupts_at(cpu, 0x8014009Cu);
        goto block_8014009C;
    } else {
        cpu->write_word(a0, s4);                        /* block_80140098: store 2063 */
        psx_check_interrupts_at(cpu, 0x8014009Cu);
        goto block_8014009C;
    }

block_8014009C:
    t5 += 24;
    a3 += 12;
    t1++;
    v0 = (int32_t)t1 < 6;
    int cond_801400AC = (v0 != zero);
    a0 += 4;                                            /* delay slot */
    if (cond_801400AC) {
        psx_check_interrupts_at(cpu, 0x80140024u);
        goto block_80140024;
    } else {
        psx_check_interrupts_at(cpu, 0x801400B4u);
        goto block_801400B4;
    }

block_801400B4:
    t1 = zero;
    t6 = s2;
    t4 = s6;
    /* fall through */

block_801400C0:
    v0 = (int32_t)(int16_t)cpu->read_half(s6 + 2408);   /* lh +0x968 count */
    v0 = (int32_t)t1 < (int32_t)v0;
    int cond_801400CC = (v0 == zero);                   /* t1 >= count -> exit */
    v1 = t1 ^ 0x0003;                                   /* delay slot */
    if (cond_801400CC) {
        psx_check_interrupts_at(cpu, 0x80140188u);
        goto block_80140188;
    } else {
        psx_check_interrupts_at(cpu, 0x801400D4u);
        goto block_801400D4;
    }

block_801400D4:
    v0 = (int32_t)(int16_t)cpu->read_half(t4 + 2356);   /* lh t4+0x934 */
    t0 = v1 < (uint32_t)1;                              /* sltiu: v1==0 (t1==3) */
    v0 <<= 2;
    a3 = s1 + v0;
    v0 = (int32_t)t0 < 2;
    int cond_801400E8 = (v0 == zero);
    if (cond_801400E8) {
        psx_check_interrupts_at(cpu, 0x80140174u);
        goto block_80140174;
    } else {
        psx_check_interrupts_at(cpu, 0x801400F0u);
        goto block_801400F0;
    }

block_801400F0:
    cpu->write_word(sp + 20, t6);                       /* spill */
    a1 = t3 + 32;
    a2 = a3 + 12;
    /* fall through */

block_801400FC:
    v0 = cpu->read_word(a3);
    a3 += 8;
    cpu->write_word(a1 + -24, v0);
    v0 = cpu->read_word(a2 + -4);
    t0++;
    cpu->write_word(a1 + -16, v0);
    v0 = cpu->read_word(a2 + -8);
    a0 = t3 << 8;
    cpu->write_word(a1 + -8, v0);
    v0 = cpu->read_word(a2);
    a0 = (uint32_t)a0 >> 8;                             /* t3 & 0xFFFFFF */
    cpu->write_word(a1, v0);
    t2 = cpu->read_word(sp + 20);                       /* reload spill */
    a2 += 8;
    v0 = cpu->read_word(t2);
    v1 = cpu->read_word(s0 + -4316);                    /* lw 0x801BEF2C */
    v0 <<= 2;
    v0 += v1;
    v0 = cpu->read_word(v0);
    a1 += 40;
    v0 |= s3;
    cpu->write_word(t3, v0);
    v0 = cpu->read_word(t2);
    v1 = cpu->read_word(s0 + -4316);
    v0 <<= 2;
    v0 += v1;
    cpu->write_word(v0, a0);
    v0 = (int32_t)t0 < 2;
    int cond_8014016C = (v0 != zero);
    t3 += 40;                                           /* delay slot */
    if (cond_8014016C) {
        psx_check_interrupts_at(cpu, 0x801400FCu);
        goto block_801400FC;
    } else {
        psx_check_interrupts_at(cpu, 0x80140174u);
        goto block_80140174;
    }

block_80140174:
    t6 += 4;
    t1++;
    v0 = (int32_t)t1 < 4;
    int cond_80140180 = (v0 != zero);
    t4 += 2;                                            /* delay slot */
    if (cond_80140180) {
        psx_check_interrupts_at(cpu, 0x801400C0u);
        goto block_801400C0;
    } else {
        psx_check_interrupts_at(cpu, 0x80140188u);
        goto block_80140188;
    }

block_80140188:
    s7++;
    v0 = (int32_t)s7 < 3;
    int cond_80140190 = (v0 != zero);
    if (cond_80140190) {
        psx_check_interrupts_at(cpu, 0x80140018u);
        goto block_80140018;
    } else {
        psx_check_interrupts_at(cpu, 0x80140198u);
        goto block_80140198;
    }

block_80140198:
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
    {
        uint32_t jt = ra;                               /* latch jr target */
        sp += 64;                                       /* delay slot */
        psx_check_interrupts_at(cpu, jt);
        cpu->pc = jt;                                   /* jr $ra */
    }
    return cpu->gpr[2];
}
