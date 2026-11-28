/* recon_8013.c — clean-room reconstructions of guest functions in the
 * 0x8013xxxx band (FX/emit task cluster + sprite emit + impact FX).
 *
 * Ports are mechanical-but-readable transliterations of the cps_<addr>
 * bodies in game/cps_clean.c, converted to the recon_8015.c atomic-port
 * shape: guest `jal`s run synchronously through r3_jal (clean dispatch +
 * br2_clean_call_drive for CPS callees), intra-piece edges stay plain
 * gotos behind the same psx_check_interrupts_at resume points, and
 * cross-piece edges (sibling cps_ pieces, jr $ra / jr $v0 tails) remain
 * cpu->pc tail-dispatches — generated/sibling CPS stays authoritative for
 * continuations. All mutable state lives in cpu->gpr[]/guest RAM so IRQ
 * resume through the generated piece at resume_pc is faithful.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern int      br2_bail_pending(void);
extern int      clean_try_dispatch(CPUState *cpu, uint32_t addr);
extern void     psx_dispatch_call(CPUState *cpu, uint32_t target_addr,
                                  uint32_t return_addr);
extern int      psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);
extern void     psx_restore_state_escape(void);

/* r3_jal — faithful guest `jal` from clean C: dispatch the callee
 * synchronously (clean port when covered, generated body otherwise, full
 * nested dispatch so per-block IRQ checks + coroutine yields inside
 * callees are preserved), return its v0 in gpr[2]. Same contract as
 * recon_8015.c's rs_jal / ev.c's br2_jal / recon_80138.c's wb_jal. */
static uint32_t r3_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
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

/* MIPS unaligned word accessors — same contract as cps_clean.c /
 * recon_80138.c br2_lwl/lwr/swl/swr (MMIO-aware cpu read/write path). */
static inline uint32_t r3_lwl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return (rt_value & 0x00FFFFFFu) | (word << 24);
        case 1: return (rt_value & 0x0000FFFFu) | (word << 16);
        case 2: return (rt_value & 0x000000FFu) | (word << 8);
        default: return word;
    }
}
static inline uint32_t r3_lwr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return word;
        case 1: return (rt_value & 0xFF000000u) | (word >> 8);
        case 2: return (rt_value & 0xFFFF0000u) | (word >> 16);
        default: return (rt_value & 0xFFFFFF00u) | (word >> 24);
    }
}
static inline void r3_swl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
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
static inline void r3_swr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
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

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability),
 * identical to cps_clean.c / recon_80138.c so the transliterated bodies
 * read the same. */
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

/* ======== sprite_kind_emit_dispatch (guest 0x80134648) ========
 * Sprite-kind emit select: runs prim_task_run on the record's base prim
 * slots (record+0 / +8 variants gated on lh +12 and the +0x26 kind field
 * at rec+38), then binary-searches the 5-bit kind field
 * ((lh(rec+38)+20) & 0x1F00) down a compare tree to pick sprite_slot_commit
 * args (a0 = 0/0x120, a1 = slot id 480..495, a2 = 16, a3 = 1, stack args
 * 288/480) and tail-jals sprite_slot_commit. 101 CPS blocks / 1260 bytes.
 * Port of cps_80134648 (SLES_017.22_full_06.c br2eu_func_80134648). */
void sprite_kind_emit_dispatch(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;

/* Address: 0x80134648, Size: 1260 bytes, Blocks: 101 */

block_80134648:
    sp -= 48;  /* 0x80134648: 0x27BDFFD0 */
    cpu->write_word(sp + 36, s3);  /* 0x8013464C: 0xAFB30024 */
    s3 = a0;  /* 0x80134650: 0x00809821 */
    cpu->write_word(sp + 32, s2);  /* 0x80134654: 0xAFB20020 */
    s2 = a1;  /* 0x80134658: 0x00A09021 */
    cpu->write_word(sp + 28, s1);  /* 0x8013465C: 0xAFB1001C */
    s1 = a2;  /* 0x80134660: 0x00C08821 */
    cpu->write_word(sp + 24, s0);  /* 0x80134664: 0xAFB00018 */
    s0 = s3 + 8264;  /* 0x80134668: 0x26702048 */
    a1 = s0;  /* 0x8013466C: 0x02002821 */
    a2 = s2;  /* 0x80134670: 0x02403021 */
    cpu->write_word(sp + 40, ra);  /* 0x80134674: 0xAFBF0028 */
    a0 = cpu->read_word(s1);  /* 0x80134678: 0x8E240000 */
    /* delay slot (always executes) */
    a3 = 1;  /* 0x80134680: 0x24070001 */
    r3_jal(0x8018A4F8u, 0x80134684u);  /* jal prim_task_run */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x80134684u);
    goto block_80134684;

block_80134684:
    a1 = s0;  /* 0x80134684: 0x02002821 */
    a2 = s2;  /* 0x80134688: 0x02403021 */
    a0 = cpu->read_word(s1 + 8);  /* 0x8013468C: 0x8E240008 */
    a3 = 1;  /* 0x80134690: 0x24070001 */
    /* delay slot (always executes) */
    a0 += 160;  /* 0x80134698: 0x248400A0 */
    r3_jal(0x8018A4F8u, 0x8013469Cu);  /* jal prim_task_run */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013469Cu);
    goto block_8013469C;

block_8013469C:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 12);  /* 0x8013469C: 0x8622000C */
    /* nop */  /* 0x801346A0: 0x00000000 */
    a3 = v0 + -5;  /* 0x801346A4: 0x2447FFFB */
    int cond_801346A8 = ((int32_t)a3 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s3 + 8308;  /* 0x801346AC: 0x26652074 */
    if (cond_801346A8) {
        psx_check_interrupts_at(cpu, 0x801346C0u);
        goto block_801346C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801346B0u);
    }

block_801346B0:
    a0 = cpu->read_word(s1 + 8);  /* 0x801346B0: 0x8E240008 */
    a2 = s2;  /* 0x801346B4: 0x02403021 */
    /* delay slot (always executes) */
    a0 += 200;  /* 0x801346BC: 0x248400C8 */
    r3_jal(0x8018A4F8u, 0x801346C0u);  /* jal prim_task_run */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x801346C0u);
    goto block_801346C0;

block_801346C0:
    v0 = cpu->read_half(s1 + 38);  /* 0x801346C0: 0x96220026 */
    /* nop */  /* 0x801346C4: 0x00000000 */
    v0 += 20;  /* 0x801346C8: 0x24420014 */
    v1 = v0 & 0x1F00;  /* 0x801346CC: 0x30431F00 */
    cpu->write_half(s1 + 38, (uint16_t)v0);  /* 0x801346D0: 0xA6220026 */
    v0 = 4096;  /* 0x801346D4: 0x24021000 */
    int cond_801346D8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 480;  /* 0x801346DC: 0x240501E0 */
    if (cond_801346D8) {
        psx_check_interrupts_at(cpu, 0x80134A50u);
        goto block_80134A50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801346E0u);
    }

block_801346E0:
    v0 = (int32_t)v1 < 4097;  /* 0x801346E0: 0x28621001 */
    int cond_801346E4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2048;  /* 0x801346E8: 0x24020800 */
    if (cond_801346E4) {
        psx_check_interrupts_at(cpu, 0x801347F8u);
        goto block_801347F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801346ECu);
    }

block_801346EC:
    ;
    int cond_801346EC = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 2049;  /* 0x801346F0: 0x28620801 */
    if (cond_801346EC) {
        psx_check_interrupts_at(cpu, 0x801349A0u);
        goto block_801349A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801346F4u);
    }

block_801346F4:
    ;
    int cond_801346F4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;  /* 0x801346F8: 0x24020400 */
    if (cond_801346F4) {
        psx_check_interrupts_at(cpu, 0x80134778u);
        goto block_80134778;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801346FCu);
    }

block_801346FC:
    ;
    int cond_801346FC = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 1025;  /* 0x80134700: 0x28620401 */
    if (cond_801346FC) {
        psx_check_interrupts_at(cpu, 0x80134948u);
        goto block_80134948;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134704u);
    }

block_80134704:
    ;
    int cond_80134704 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 512;  /* 0x80134708: 0x24020200 */
    if (cond_80134704) {
        psx_check_interrupts_at(cpu, 0x80134740u);
        goto block_80134740;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013470Cu);
    }

block_8013470C:
    ;
    int cond_8013470C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 513;  /* 0x80134710: 0x28620201 */
    if (cond_8013470C) {
        psx_check_interrupts_at(cpu, 0x8013491Cu);
        goto block_8013491C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134714u);
    }

block_80134714:
    ;
    int cond_80134714 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 256;  /* 0x80134718: 0x24020100 */
    if (cond_80134714) {
        psx_check_interrupts_at(cpu, 0x8013472Cu);
        goto block_8013472C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013471Cu);
    }

block_8013471C:
    ;
    int cond_8013471C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x80134720: 0x24040120 */
    if (cond_8013471C) {
        psx_check_interrupts_at(cpu, 0x80134908u);
        goto block_80134908;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134724u);
    }

block_80134724:
    /* delay slot (always executes) */
    /* nop */  /* 0x80134728: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_8013472C:
    v0 = 768;  /* 0x8013472C: 0x24020300 */
    int cond_80134730 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x80134734: 0x24040120 */
    if (cond_80134730) {
        psx_check_interrupts_at(cpu, 0x80134934u);
        goto block_80134934;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134738u);
    }

block_80134738:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013473C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_80134740:
    v0 = 1536;  /* 0x80134740: 0x24020600 */
    int cond_80134744 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 1537;  /* 0x80134748: 0x28620601 */
    if (cond_80134744) {
        psx_check_interrupts_at(cpu, 0x80134974u);
        goto block_80134974;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013474Cu);
    }

block_8013474C:
    ;
    int cond_8013474C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1280;  /* 0x80134750: 0x24020500 */
    if (cond_8013474C) {
        psx_check_interrupts_at(cpu, 0x80134764u);
        goto block_80134764;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134754u);
    }

block_80134754:
    ;
    int cond_80134754 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x80134758: 0x24040120 */
    if (cond_80134754) {
        psx_check_interrupts_at(cpu, 0x80134960u);
        goto block_80134960;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013475Cu);
    }

block_8013475C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80134760: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_80134764:
    v0 = 1792;  /* 0x80134764: 0x24020700 */
    int cond_80134768 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x8013476C: 0x24040120 */
    if (cond_80134768) {
        psx_check_interrupts_at(cpu, 0x8013498Cu);
        goto block_8013498C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134770u);
    }

block_80134770:
    /* delay slot (always executes) */
    /* nop */  /* 0x80134774: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_80134778:
    v0 = 3072;  /* 0x80134778: 0x24020C00 */
    int cond_8013477C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 3073;  /* 0x80134780: 0x28620C01 */
    if (cond_8013477C) {
        psx_check_interrupts_at(cpu, 0x801349F8u);
        goto block_801349F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134784u);
    }

block_80134784:
    ;
    int cond_80134784 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2560;  /* 0x80134788: 0x24020A00 */
    if (cond_80134784) {
        psx_check_interrupts_at(cpu, 0x801347C0u);
        goto block_801347C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013478Cu);
    }

block_8013478C:
    ;
    int cond_8013478C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 2561;  /* 0x80134790: 0x28620A01 */
    if (cond_8013478C) {
        psx_check_interrupts_at(cpu, 0x801349CCu);
        goto block_801349CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134794u);
    }

block_80134794:
    ;
    int cond_80134794 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2304;  /* 0x80134798: 0x24020900 */
    if (cond_80134794) {
        psx_check_interrupts_at(cpu, 0x801347ACu);
        goto block_801347AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013479Cu);
    }

block_8013479C:
    ;
    int cond_8013479C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x801347A0: 0x24040120 */
    if (cond_8013479C) {
        psx_check_interrupts_at(cpu, 0x801349B8u);
        goto block_801349B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801347A4u);
    }

block_801347A4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801347A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_801347AC:
    v0 = 2816;  /* 0x801347AC: 0x24020B00 */
    int cond_801347B0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x801347B4: 0x24040120 */
    if (cond_801347B0) {
        psx_check_interrupts_at(cpu, 0x801349E4u);
        goto block_801349E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801347B8u);
    }

block_801347B8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801347BC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_801347C0:
    v0 = 3584;  /* 0x801347C0: 0x24020E00 */
    int cond_801347C4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 3585;  /* 0x801347C8: 0x28620E01 */
    if (cond_801347C4) {
        psx_check_interrupts_at(cpu, 0x80134A24u);
        goto block_80134A24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801347CCu);
    }

block_801347CC:
    ;
    int cond_801347CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3328;  /* 0x801347D0: 0x24020D00 */
    if (cond_801347CC) {
        psx_check_interrupts_at(cpu, 0x801347E4u);
        goto block_801347E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801347D4u);
    }

block_801347D4:
    ;
    int cond_801347D4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x801347D8: 0x24040120 */
    if (cond_801347D4) {
        psx_check_interrupts_at(cpu, 0x80134A10u);
        goto block_80134A10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801347DCu);
    }

block_801347DC:
    /* delay slot (always executes) */
    /* nop */  /* 0x801347E0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_801347E4:
    v0 = 3840;  /* 0x801347E4: 0x24020F00 */
    int cond_801347E8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 288;  /* 0x801347EC: 0x24040120 */
    if (cond_801347E8) {
        psx_check_interrupts_at(cpu, 0x80134A3Cu);
        goto block_80134A3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801347F0u);
    }

block_801347F0:
    /* delay slot (always executes) */
    /* nop */  /* 0x801347F4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_801347F8:
    v0 = 6144;  /* 0x801347F8: 0x24021800 */
    int cond_801347FC = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 6145;  /* 0x80134800: 0x28621801 */
    if (cond_801347FC) {
        psx_check_interrupts_at(cpu, 0x80134AB0u);
        goto block_80134AB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134804u);
    }

block_80134804:
    ;
    int cond_80134804 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5120;  /* 0x80134808: 0x24021400 */
    if (cond_80134804) {
        psx_check_interrupts_at(cpu, 0x80134888u);
        goto block_80134888;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013480Cu);
    }

block_8013480C:
    ;
    int cond_8013480C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 5121;  /* 0x80134810: 0x28621401 */
    if (cond_8013480C) {
        psx_check_interrupts_at(cpu, 0x80134A88u);
        goto block_80134A88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134814u);
    }

block_80134814:
    ;
    int cond_80134814 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4608;  /* 0x80134818: 0x24021200 */
    if (cond_80134814) {
        psx_check_interrupts_at(cpu, 0x80134850u);
        goto block_80134850;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013481Cu);
    }

block_8013481C:
    ;
    int cond_8013481C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 4609;  /* 0x80134820: 0x28621201 */
    if (cond_8013481C) {
        psx_check_interrupts_at(cpu, 0x80134A74u);
        goto block_80134A74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134824u);
    }

block_80134824:
    ;
    int cond_80134824 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4352;  /* 0x80134828: 0x24021100 */
    if (cond_80134824) {
        psx_check_interrupts_at(cpu, 0x8013483Cu);
        goto block_8013483C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013482Cu);
    }

block_8013482C:
    ;
    int cond_8013482C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x80134830: 0x24040130 */
    if (cond_8013482C) {
        psx_check_interrupts_at(cpu, 0x80134A6Cu);
        goto block_80134A6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134834u);
    }

block_80134834:
    /* delay slot (always executes) */
    /* nop */  /* 0x80134838: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_8013483C:
    v0 = 4864;  /* 0x8013483C: 0x24021300 */
    int cond_80134840 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x80134844: 0x24040130 */
    if (cond_80134840) {
        psx_check_interrupts_at(cpu, 0x80134A80u);
        goto block_80134A80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134848u);
    }

block_80134848:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013484C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_80134850:
    v0 = 5632;  /* 0x80134850: 0x24021600 */
    int cond_80134854 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 5633;  /* 0x80134858: 0x28621601 */
    if (cond_80134854) {
        psx_check_interrupts_at(cpu, 0x80134A9Cu);
        goto block_80134A9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013485Cu);
    }

block_8013485C:
    ;
    int cond_8013485C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5376;  /* 0x80134860: 0x24021500 */
    if (cond_8013485C) {
        psx_check_interrupts_at(cpu, 0x80134874u);
        goto block_80134874;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134864u);
    }

block_80134864:
    ;
    int cond_80134864 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x80134868: 0x24040130 */
    if (cond_80134864) {
        psx_check_interrupts_at(cpu, 0x80134A94u);
        goto block_80134A94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013486Cu);
    }

block_8013486C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80134870: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_80134874:
    v0 = 5888;  /* 0x80134874: 0x24021700 */
    int cond_80134878 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x8013487C: 0x24040130 */
    if (cond_80134878) {
        psx_check_interrupts_at(cpu, 0x80134AA8u);
        goto block_80134AA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134880u);
    }

block_80134880:
    /* delay slot (always executes) */
    /* nop */  /* 0x80134884: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_80134888:
    v0 = 7168;  /* 0x80134888: 0x24021C00 */
    int cond_8013488C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 7169;  /* 0x80134890: 0x28621C01 */
    if (cond_8013488C) {
        psx_check_interrupts_at(cpu, 0x80134AD8u);
        goto block_80134AD8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134894u);
    }

block_80134894:
    ;
    int cond_80134894 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 6656;  /* 0x80134898: 0x24021A00 */
    if (cond_80134894) {
        psx_check_interrupts_at(cpu, 0x801348D0u);
        goto block_801348D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013489Cu);
    }

block_8013489C:
    ;
    int cond_8013489C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 6657;  /* 0x801348A0: 0x28621A01 */
    if (cond_8013489C) {
        psx_check_interrupts_at(cpu, 0x80134AC4u);
        goto block_80134AC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801348A4u);
    }

block_801348A4:
    ;
    int cond_801348A4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 6400;  /* 0x801348A8: 0x24021900 */
    if (cond_801348A4) {
        psx_check_interrupts_at(cpu, 0x801348BCu);
        goto block_801348BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801348ACu);
    }

block_801348AC:
    ;
    int cond_801348AC = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x801348B0: 0x24040130 */
    if (cond_801348AC) {
        psx_check_interrupts_at(cpu, 0x80134ABCu);
        goto block_80134ABC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801348B4u);
    }

block_801348B4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801348B8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_801348BC:
    v0 = 6912;  /* 0x801348BC: 0x24021B00 */
    int cond_801348C0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x801348C4: 0x24040130 */
    if (cond_801348C0) {
        psx_check_interrupts_at(cpu, 0x80134AD0u);
        goto block_80134AD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801348C8u);
    }

block_801348C8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801348CC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_801348D0:
    v0 = 7680;  /* 0x801348D0: 0x24021E00 */
    int cond_801348D4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 7681;  /* 0x801348D8: 0x28621E01 */
    if (cond_801348D4) {
        psx_check_interrupts_at(cpu, 0x80134AECu);
        goto block_80134AEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801348DCu);
    }

block_801348DC:
    ;
    int cond_801348DC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 7424;  /* 0x801348E0: 0x24021D00 */
    if (cond_801348DC) {
        psx_check_interrupts_at(cpu, 0x801348F4u);
        goto block_801348F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801348E4u);
    }

block_801348E4:
    ;
    int cond_801348E4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x801348E8: 0x24040130 */
    if (cond_801348E4) {
        psx_check_interrupts_at(cpu, 0x80134AE4u);
        goto block_80134AE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801348ECu);
    }

block_801348EC:
    /* delay slot (always executes) */
    /* nop */  /* 0x801348F0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_801348F4:
    v0 = 7936;  /* 0x801348F4: 0x24021F00 */
    int cond_801348F8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 304;  /* 0x801348FC: 0x24040130 */
    if (cond_801348F8) {
        psx_check_interrupts_at(cpu, 0x80134AF8u);
        goto block_80134AF8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80134900u);
    }

block_80134900:
    /* delay slot (always executes) */
    /* nop */  /* 0x80134904: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;  /* j */

block_80134908:
    a1 = 481;  /* 0x80134908: 0x240501E1 */
    a2 = 16;  /* 0x8013490C: 0x24060010 */
    a3 = 1;  /* 0x80134910: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134918: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_8013491C:
    a0 = 288;  /* 0x8013491C: 0x24040120 */
    a1 = 482;  /* 0x80134920: 0x240501E2 */
    a2 = 16;  /* 0x80134924: 0x24060010 */
    a3 = 1;  /* 0x80134928: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134930: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134934:
    a1 = 483;  /* 0x80134934: 0x240501E3 */
    a2 = 16;  /* 0x80134938: 0x24060010 */
    a3 = 1;  /* 0x8013493C: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134944: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134948:
    a0 = 288;  /* 0x80134948: 0x24040120 */
    a1 = 484;  /* 0x8013494C: 0x240501E4 */
    a2 = 16;  /* 0x80134950: 0x24060010 */
    a3 = 1;  /* 0x80134954: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x8013495C: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134960:
    a1 = 485;  /* 0x80134960: 0x240501E5 */
    a2 = 16;  /* 0x80134964: 0x24060010 */
    a3 = 1;  /* 0x80134968: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134970: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134974:
    a0 = 288;  /* 0x80134974: 0x24040120 */
    a1 = 486;  /* 0x80134978: 0x240501E6 */
    a2 = 16;  /* 0x8013497C: 0x24060010 */
    a3 = 1;  /* 0x80134980: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134988: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_8013498C:
    a1 = 487;  /* 0x8013498C: 0x240501E7 */
    a2 = 16;  /* 0x80134990: 0x24060010 */
    a3 = 1;  /* 0x80134994: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x8013499C: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_801349A0:
    a0 = 288;  /* 0x801349A0: 0x24040120 */
    a1 = 488;  /* 0x801349A4: 0x240501E8 */
    a2 = 16;  /* 0x801349A8: 0x24060010 */
    a3 = 1;  /* 0x801349AC: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x801349B4: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_801349B8:
    a1 = 489;  /* 0x801349B8: 0x240501E9 */
    a2 = 16;  /* 0x801349BC: 0x24060010 */
    a3 = 1;  /* 0x801349C0: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x801349C8: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_801349CC:
    a0 = 288;  /* 0x801349CC: 0x24040120 */
    a1 = 490;  /* 0x801349D0: 0x240501EA */
    a2 = 16;  /* 0x801349D4: 0x24060010 */
    a3 = 1;  /* 0x801349D8: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x801349E0: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_801349E4:
    a1 = 491;  /* 0x801349E4: 0x240501EB */
    a2 = 16;  /* 0x801349E8: 0x24060010 */
    a3 = 1;  /* 0x801349EC: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x801349F4: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_801349F8:
    a0 = 288;  /* 0x801349F8: 0x24040120 */
    a1 = 492;  /* 0x801349FC: 0x240501EC */
    a2 = 16;  /* 0x80134A00: 0x24060010 */
    a3 = 1;  /* 0x80134A04: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134A0C: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134A10:
    a1 = 493;  /* 0x80134A10: 0x240501ED */
    a2 = 16;  /* 0x80134A14: 0x24060010 */
    a3 = 1;  /* 0x80134A18: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134A20: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134A24:
    a0 = 288;  /* 0x80134A24: 0x24040120 */
    a1 = 494;  /* 0x80134A28: 0x240501EE */
    a2 = 16;  /* 0x80134A2C: 0x24060010 */
    a3 = 1;  /* 0x80134A30: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134A38: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134A3C:
    a1 = 495;  /* 0x80134A3C: 0x240501EF */
    a2 = 16;  /* 0x80134A40: 0x24060010 */
    a3 = 1;  /* 0x80134A44: 0x24070001 */
    /* delay slot (always executes) */
    v0 = a0;  /* 0x80134A4C: 0x00801021 */
    psx_check_interrupts_at(cpu, 0x80134B08u);
    goto block_80134B08;  /* j */

block_80134A50:
    a0 = 304;  /* 0x80134A50: 0x24040130 */
    a2 = 16;  /* 0x80134A54: 0x24060010 */
    a3 = 1;  /* 0x80134A58: 0x24070001 */
    v0 = 288;  /* 0x80134A5C: 0x24020120 */
    cpu->write_word(sp + 16, v0);  /* 0x80134A60: 0xAFA20010 */
    /* delay slot (always executes) */
    v0 = a1;  /* 0x80134A68: 0x00A01021 */
    psx_check_interrupts_at(cpu, 0x80134B10u);
    goto block_80134B10;  /* j */

block_80134A6C:
    /* delay slot (always executes) */
    a1 = 481;  /* 0x80134A70: 0x240501E1 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134A74:
    a0 = 304;  /* 0x80134A74: 0x24040130 */
    /* delay slot (always executes) */
    a1 = 482;  /* 0x80134A7C: 0x240501E2 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134A80:
    /* delay slot (always executes) */
    a1 = 483;  /* 0x80134A84: 0x240501E3 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134A88:
    a0 = 304;  /* 0x80134A88: 0x24040130 */
    /* delay slot (always executes) */
    a1 = 484;  /* 0x80134A90: 0x240501E4 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134A94:
    /* delay slot (always executes) */
    a1 = 485;  /* 0x80134A98: 0x240501E5 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134A9C:
    a0 = 304;  /* 0x80134A9C: 0x24040130 */
    /* delay slot (always executes) */
    a1 = 486;  /* 0x80134AA4: 0x240501E6 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AA8:
    /* delay slot (always executes) */
    a1 = 487;  /* 0x80134AAC: 0x240501E7 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AB0:
    a0 = 304;  /* 0x80134AB0: 0x24040130 */
    /* delay slot (always executes) */
    a1 = 488;  /* 0x80134AB8: 0x240501E8 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134ABC:
    /* delay slot (always executes) */
    a1 = 489;  /* 0x80134AC0: 0x240501E9 */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AC4:
    a0 = 304;  /* 0x80134AC4: 0x24040130 */
    /* delay slot (always executes) */
    a1 = 490;  /* 0x80134ACC: 0x240501EA */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AD0:
    /* delay slot (always executes) */
    a1 = 491;  /* 0x80134AD4: 0x240501EB */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AD8:
    a0 = 304;  /* 0x80134AD8: 0x24040130 */
    /* delay slot (always executes) */
    a1 = 492;  /* 0x80134AE0: 0x240501EC */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AE4:
    /* delay slot (always executes) */
    a1 = 493;  /* 0x80134AE8: 0x240501ED */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AEC:
    a0 = 304;  /* 0x80134AEC: 0x24040130 */
    /* delay slot (always executes) */
    a1 = 494;  /* 0x80134AF4: 0x240501EE */
    psx_check_interrupts_at(cpu, 0x80134AFCu);
    goto block_80134AFC;  /* j */

block_80134AF8:
    a1 = 495;  /* 0x80134AF8: 0x240501EF */
    /* fall through to block_80134AFC */

block_80134AFC:
    a2 = 16;  /* 0x80134AFC: 0x24060010 */
    a3 = 1;  /* 0x80134B00: 0x24070001 */
    v0 = 288;  /* 0x80134B04: 0x24020120 */
    /* fall through to block_80134B08 */

block_80134B08:
    cpu->write_word(sp + 16, v0);  /* 0x80134B08: 0xAFA20010 */
    v0 = 480;  /* 0x80134B0C: 0x240201E0 */
    /* fall through to block_80134B10 */

block_80134B10:
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);  /* 0x80134B14: 0xAFA20014 */
    r3_jal(0x8012A504u, 0x80134B18u);  /* jal sprite_slot_commit */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x80134B18u);
    goto block_80134B18;

block_80134B18:
    ra = cpu->read_word(sp + 40);  /* 0x80134B18: 0x8FBF0028 */
    s3 = cpu->read_word(sp + 36);  /* 0x80134B1C: 0x8FB30024 */
    s2 = cpu->read_word(sp + 32);  /* 0x80134B20: 0x8FB20020 */
    s1 = cpu->read_word(sp + 28);  /* 0x80134B24: 0x8FB1001C */
    s0 = cpu->read_word(sp + 24);  /* 0x80134B28: 0x8FB00018 */
    uint32_t jt_80134B2C = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 48;  /* 0x80134B30: 0x27BD0030 */
    psx_check_interrupts_at(cpu, jt_80134B2C);
    cpu->pc = jt_80134B2C; return;  /* CPS: jr $ra */
}


/* ======== stage_env_fx_task_xform (guest 0x8013B844) ========
 * Continuation piece of the stage-env FX task callback (task cb
 * 0x8013B72C, spawned by stage_env_task_spawn @0x8013C9CC with a 7200-byte
 * payload): computes endpoint deltas, angle_from_xy, a lut_cos_q12 fan,
 * node_tree_free + task_slot_step, then gte_rtps_xform1. No prologue —
 * entered mid-frame via cpu->pc from cps_8013b72c's tail; all state is
 * live cpu->gpr/guest RAM. 74 CPS blocks / 1408 bytes.
 * Port of cps_8013b844 (SLES_017.22_full_09.c br2eu_func_8013B844). */
void stage_env_fx_task_xform(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;

/* Address: 0x8013B844, Size: 1408 bytes, Blocks: 74 */

block_8013B844:
    cpu->write_half(sp + 56, (uint16_t)v1);  /* 0x8013B844: 0xA7A30038 */
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 7198);  /* 0x8013B848: 0x86831C1E */
    a0 = cpu->read_word(s4 + 5768);  /* 0x8013B84C: 0x8E841688 */
    v0 = v1 << 2;  /* 0x8013B850: 0x00031080 */
    v0 += v1;  /* 0x8013B854: 0x00431021 */
    v1 = cpu->read_word(a0 + 32);  /* 0x8013B858: 0x8C830020 */
    v0 <<= 4;  /* 0x8013B85C: 0x00021100 */
    v0 += v1;  /* 0x8013B860: 0x00431021 */
    v1 = cpu->read_half(sp + 58);  /* 0x8013B864: 0x97A3003A */
    v0 = cpu->read_half(v0 + 56);  /* 0x8013B868: 0x94420038 */
    t3 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013B86C: 0x3C0B1F80 */
    v1 += v0;  /* 0x8013B870: 0x00621821 */
    cpu->write_half(sp + 58, (uint16_t)v1);  /* 0x8013B874: 0xA7A3003A */
    a0 = (int32_t)(int16_t)cpu->read_half(s4 + 7198);  /* 0x8013B878: 0x86841C1E */
    v1 = cpu->read_word(s4 + 5768);  /* 0x8013B87C: 0x8E831688 */
    v0 = a0 << 2;  /* 0x8013B880: 0x00041080 */
    v0 += a0;  /* 0x8013B884: 0x00441021 */
    v1 = cpu->read_word(v1 + 32);  /* 0x8013B888: 0x8C630020 */
    v0 <<= 4;  /* 0x8013B88C: 0x00021100 */
    v0 += v1;  /* 0x8013B890: 0x00431021 */
    v1 = cpu->read_half(sp + 60);  /* 0x8013B894: 0x97A3003C */
    v0 = cpu->read_half(v0 + 60);  /* 0x8013B898: 0x9442003C */
    t3 |= 0x03BC;  /* 0x8013B89C: 0x356B03BC */
    cpu->write_word(sp + 312, t3);  /* 0x8013B8A0: 0xAFAB0138 */
    v1 += v0;  /* 0x8013B8A4: 0x00621821 */
    cpu->write_half(sp + 60, (uint16_t)v1);  /* 0x8013B8A8: 0xA7A3003C */
    t5 = r3_lwl(cpu, sp + 59, t5);  /* 0x8013B8AC: 0x8BAD003B */
    t5 = r3_lwr(cpu, sp + 56, t5);  /* 0x8013B8B0: 0x9BAD0038 */
    t6 = r3_lwl(cpu, sp + 63, t6);  /* 0x8013B8B4: 0x8BAE003F */
    t6 = r3_lwr(cpu, sp + 60, t6);  /* 0x8013B8B8: 0x9BAE003C */
    r3_swl(cpu, sp + 75, t5);  /* 0x8013B8BC: 0xABAD004B */
    r3_swr(cpu, sp + 72, t5);  /* 0x8013B8C0: 0xBBAD0048 */
    r3_swl(cpu, sp + 79, t6);  /* 0x8013B8C4: 0xABAE004F */
    r3_swr(cpu, sp + 76, t6);  /* 0x8013B8C8: 0xBBAE004C */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 56);  /* 0x8013B8CC: 0x87A30038 */
    a0 = (int32_t)(int16_t)cpu->read_half(sp + 32);  /* 0x8013B8D0: 0x87A40020 */
    v0 = (int32_t)(int16_t)cpu->read_half(sp + 60);  /* 0x8013B8D4: 0x87A2003C */
    a1 = (int32_t)(int16_t)cpu->read_half(sp + 36);  /* 0x8013B8D8: 0x87A50024 */
    a0 = v1 - a0;  /* 0x8013B8DC: 0x00642023 */
    /* delay slot (always executes) */
    a1 = v0 - a1;  /* 0x8013B8E4: 0x00452823 */
    r3_jal(0x80142B3Cu, 0x8013B8E8u);  /* jal angle_from_xy */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B8E8u);
    goto block_8013B8E8;

block_8013B8E8:
    a0 = (int32_t)(int16_t)cpu->read_half(sp + 56);  /* 0x8013B8E8: 0x87A40038 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 32);  /* 0x8013B8EC: 0x87A30020 */
    s1 = v0 + 1024;  /* 0x8013B8F0: 0x24510400 */
    v0 = a0 - v1;  /* 0x8013B8F4: 0x00831023 */
    int cond_8013B8F8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v0 << 12;  /* 0x8013B8FC: 0x00028300 */
    if (cond_8013B8F8) {
        psx_check_interrupts_at(cpu, 0x8013B908u);
        goto block_8013B908;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B900u);
    }

block_8013B900:
    v0 = v1 - a0;  /* 0x8013B900: 0x00641023 */
    s0 = v0 << 12;  /* 0x8013B904: 0x00028300 */
    /* fall through to block_8013B908 */

block_8013B908:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013B90C: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013B910u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B910u);
    goto block_8013B910;

block_8013B910:
    ;
    int cond_8013B910 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013B914: 0x00000000 */
    if (cond_8013B910) {
        psx_check_interrupts_at(cpu, 0x8013B93Cu);
        goto block_8013B93C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B918u);
    }

block_8013B918:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013B91C: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013B920u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B920u);
    goto block_8013B920;

block_8013B920:
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)s0 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)s0 % (int32_t)v0); } else { cpu->lo = ((int32_t)s0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)s0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013B920: 0x0202001A */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013B924: 0x00001012 */
    /* nop */  /* 0x8013B928: 0x00000000 */
    int cond_8013B92C = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013B930: 0x00000000 */
    if (cond_8013B92C) {
        psx_check_interrupts_at(cpu, 0x8013B98Cu);
        goto block_8013B98C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B934u);
    }

block_8013B934:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013B938: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013B944u);
    goto block_8013B944;  /* j */

block_8013B93C:
    ;
    int cond_8013B93C = ((int32_t)s0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013B940: 0x00000000 */
    if (cond_8013B93C) {
        psx_check_interrupts_at(cpu, 0x8013B98Cu);
        goto block_8013B98C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B944u);
    }

block_8013B944:
    a0 = (int32_t)(int16_t)cpu->read_half(sp + 56);  /* 0x8013B944: 0x87A40038 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 32);  /* 0x8013B948: 0x87A30020 */
    /* nop */  /* 0x8013B94C: 0x00000000 */
    v0 = a0 - v1;  /* 0x8013B950: 0x00831023 */
    int cond_8013B954 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v0 << 12;  /* 0x8013B958: 0x00028300 */
    if (cond_8013B954) {
        psx_check_interrupts_at(cpu, 0x8013B964u);
        goto block_8013B964;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B95Cu);
    }

block_8013B95C:
    v0 = v1 - a0;  /* 0x8013B95C: 0x00641023 */
    s0 = v0 << 12;  /* 0x8013B960: 0x00028300 */
    /* fall through to block_8013B964 */

block_8013B964:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013B968: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013B96Cu);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B96Cu);
    goto block_8013B96C;

block_8013B96C:
    ;
    int cond_8013B96C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 = s0;  /* 0x8013B970: 0x02009021 */
    if (cond_8013B96C) {
        psx_check_interrupts_at(cpu, 0x8013B9D4u);
        goto block_8013B9D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B974u);
    }

block_8013B974:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013B978: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013B97Cu);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B97Cu);
    goto block_8013B97C;

block_8013B97C:
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)s0 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)s0 % (int32_t)v0); } else { cpu->lo = ((int32_t)s0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)s0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013B97C: 0x0202001A */
    s2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013B980: 0x00009012 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013B988: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013B9D4u);
    goto block_8013B9D4;  /* j */

block_8013B98C:
    a0 = (int32_t)(int16_t)cpu->read_half(sp + 56);  /* 0x8013B98C: 0x87A40038 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 32);  /* 0x8013B990: 0x87A30020 */
    /* nop */  /* 0x8013B994: 0x00000000 */
    v0 = a0 - v1;  /* 0x8013B998: 0x00831023 */
    int cond_8013B99C = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v0 << 12;  /* 0x8013B9A0: 0x00028300 */
    if (cond_8013B99C) {
        psx_check_interrupts_at(cpu, 0x8013B9ACu);
        goto block_8013B9AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B9A4u);
    }

block_8013B9A4:
    v0 = v1 - a0;  /* 0x8013B9A4: 0x00641023 */
    s0 = v0 << 12;  /* 0x8013B9A8: 0x00028300 */
    /* fall through to block_8013B9AC */

block_8013B9AC:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013B9B0: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013B9B4u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B9B4u);
    goto block_8013B9B4;

block_8013B9B4:
    ;
    int cond_8013B9B4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 = zero - s0;  /* 0x8013B9B8: 0x00109023 */
    if (cond_8013B9B4) {
        psx_check_interrupts_at(cpu, 0x8013B9D4u);
        goto block_8013B9D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013B9BCu);
    }

block_8013B9BC:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013B9C0: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013B9C4u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B9C4u);
    goto block_8013B9C4;

block_8013B9C4:
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)s0 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)s0 % (int32_t)v0); } else { cpu->lo = ((int32_t)s0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)s0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013B9C4: 0x0202001A */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013B9C8: 0x00001012 */
    /* nop */  /* 0x8013B9CC: 0x00000000 */
    s2 = zero - v0;  /* 0x8013B9D0: 0x00029023 */
    /* fall through to block_8013B9D4 */

block_8013B9D4:
    v0 = (int32_t)(int16_t)cpu->read_half(sp + 58);  /* 0x8013B9D4: 0x87A2003A */
    a1 = (int32_t)(int16_t)cpu->read_half(sp + 34);  /* 0x8013B9D8: 0x87A50022 */
    a0 = s2;  /* 0x8013B9DC: 0x02402021 */
    /* delay slot (always executes) */
    a1 = v0 - a1;  /* 0x8013B9E4: 0x00452823 */
    r3_jal(0x80142B3Cu, 0x8013B9E8u);  /* jal angle_from_xy */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B9E8u);
    goto block_8013B9E8;

block_8013B9E8:
    s1 = v0 + 1024;  /* 0x8013B9E8: 0x24510400 */
    a0 = s1;  /* 0x8013B9EC: 0x02202021 */
    s0 = s2 << 12;  /* 0x8013B9F0: 0x00128300 */
    /* delay slot (always executes) */
    s3 = s0;  /* 0x8013B9F8: 0x02009821 */
    r3_jal(0x8018D298u, 0x8013B9FCu);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013B9FCu);
    goto block_8013B9FC;

block_8013B9FC:
    ;
    int cond_8013B9FC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BA00: 0x00000000 */
    if (cond_8013B9FC) {
        psx_check_interrupts_at(cpu, 0x8013BA28u);
        goto block_8013BA28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BA04u);
    }

block_8013BA04:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013BA08: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013BA0Cu);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BA0Cu);
    goto block_8013BA0C;

block_8013BA0C:
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)s0 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)s0 % (int32_t)v0); } else { cpu->lo = ((int32_t)s0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)s0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013BA0C: 0x0202001A */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BA10: 0x00001012 */
    /* nop */  /* 0x8013BA14: 0x00000000 */
    int cond_8013BA18 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013BA1C: 0x02202021 */
    if (cond_8013BA18) {
        psx_check_interrupts_at(cpu, 0x8013BA30u);
        goto block_8013BA30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BA20u);
    }

block_8013BA20:
    /* delay slot (always executes) */
    s0 = s2 << 12;  /* 0x8013BA24: 0x00128300 */
    psx_check_interrupts_at(cpu, 0x8013BA60u);
    goto block_8013BA60;  /* j */

block_8013BA28:
    ;
    int cond_8013BA28 = ((int32_t)s3 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013BA2C: 0x02202021 */
    if (cond_8013BA28) {
        psx_check_interrupts_at(cpu, 0x8013BA5Cu);
        goto block_8013BA5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BA30u);
    }

block_8013BA30:
    s0 = s2 << 12;  /* 0x8013BA30: 0x00128300 */
    /* delay slot (always executes) */
    s2 = s0;  /* 0x8013BA38: 0x02009021 */
    r3_jal(0x8018D298u, 0x8013BA3Cu);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BA3Cu);
    goto block_8013BA3C;

block_8013BA3C:
    ;
    int cond_8013BA3C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s2;  /* 0x8013BA40: 0x02402021 */
    if (cond_8013BA3C) {
        psx_check_interrupts_at(cpu, 0x8013BA88u);
        goto block_8013BA88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BA44u);
    }

block_8013BA44:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013BA48: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013BA4Cu);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BA4Cu);
    goto block_8013BA4C;

block_8013BA4C:
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)s0 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)s0 % (int32_t)v0); } else { cpu->lo = ((int32_t)s0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)s0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013BA4C: 0x0202001A */
    a0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BA50: 0x00002012 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BA58: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013BA88u);
    goto block_8013BA88;  /* j */

block_8013BA5C:
    s0 = s2 << 12;  /* 0x8013BA5C: 0x00128300 */
    /* fall through to block_8013BA60 */

block_8013BA60:
    /* delay slot (always executes) */
    s2 = s0;  /* 0x8013BA64: 0x02009021 */
    r3_jal(0x8018D298u, 0x8013BA68u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BA68u);
    goto block_8013BA68;

block_8013BA68:
    ;
    int cond_8013BA68 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = zero - s2;  /* 0x8013BA6C: 0x00122023 */
    if (cond_8013BA68) {
        psx_check_interrupts_at(cpu, 0x8013BA88u);
        goto block_8013BA88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BA70u);
    }

block_8013BA70:
    /* delay slot (always executes) */
    a0 = s1;  /* 0x8013BA74: 0x02202021 */
    r3_jal(0x8018D298u, 0x8013BA78u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BA78u);
    goto block_8013BA78;

block_8013BA78:
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)s0 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)s0 % (int32_t)v0); } else { cpu->lo = ((int32_t)s0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)s0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013BA78: 0x0202001A */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BA7C: 0x00001012 */
    /* nop */  /* 0x8013BA80: 0x00000000 */
    a0 = zero - v0;  /* 0x8013BA84: 0x00022023 */
    /* fall through to block_8013BA88 */

block_8013BA88:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013BA88: 0x3C021F80 */
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 130);  /* 0x8013BA8C: 0x84420082 */
    /* nop */  /* 0x8013BA90: 0x00000000 */
    int cond_8013BA94 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 = a0;  /* 0x8013BA98: 0x00809021 */
    if (cond_8013BA94) {
        psx_check_interrupts_at(cpu, 0x8013BB94u);
        goto block_8013BB94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BA9Cu);
    }

block_8013BA9C:
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 7194);  /* 0x8013BA9C: 0x86831C1A */
    v0 = 1;  /* 0x8013BAA0: 0x24020001 */
    int cond_8013BAA4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 2;  /* 0x8013BAA8: 0x28620002 */
    if (cond_8013BAA4) {
        psx_check_interrupts_at(cpu, 0x8013BB24u);
        goto block_8013BB24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BAACu);
    }

block_8013BAAC:
    ;
    int cond_8013BAAC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;  /* 0x8013BAB0: 0x24020002 */
    if (cond_8013BAAC) {
        psx_check_interrupts_at(cpu, 0x8013BAC4u);
        goto block_8013BAC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BAB4u);
    }

block_8013BAB4:
    ;
    int cond_8013BAB4 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x55550000u;  /* 0x55550000 */  /* 0x8013BAB8: 0x3C025555 */
    if (cond_8013BAB4) {
        psx_check_interrupts_at(cpu, 0x8013BAD4u);
        goto block_8013BAD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BABCu);
    }

block_8013BABC:
    /* delay slot (always executes) */
    v0 |= 0x5556;  /* 0x8013BAC0: 0x34425556 */
    psx_check_interrupts_at(cpu, 0x8013BB9Cu);
    goto block_8013BB9C;  /* j */

block_8013BAC4:
    ;
    int cond_8013BAC4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x55550000u;  /* 0x55550000 */  /* 0x8013BAC8: 0x3C025555 */
    if (cond_8013BAC4) {
        psx_check_interrupts_at(cpu, 0x8013BB4Cu);
        goto block_8013BB4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BACCu);
    }

block_8013BACC:
    /* delay slot (always executes) */
    v0 |= 0x5556;  /* 0x8013BAD0: 0x34425556 */
    psx_check_interrupts_at(cpu, 0x8013BB9Cu);
    goto block_8013BB9C;  /* j */

block_8013BAD4:
    v0 = (int32_t)s2 < 512;  /* 0x8013BAD4: 0x2A420200 */
    int cond_8013BAD8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BADC: 0x00000000 */
    if (cond_8013BAD8) {
        psx_check_interrupts_at(cpu, 0x8013BAECu);
        goto block_8013BAEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BAE0u);
    }

block_8013BAE0:
    v0 = cpu->read_half(s4 + 7196);  /* 0x8013BAE0: 0x96821C1C */
    /* delay slot (always executes) */
    v0 += 3;  /* 0x8013BAE8: 0x24420003 */
    psx_check_interrupts_at(cpu, 0x8013BAF8u);
    goto block_8013BAF8;  /* j */

block_8013BAEC:
    v0 = cpu->read_half(s4 + 7196);  /* 0x8013BAEC: 0x96821C1C */
    /* nop */  /* 0x8013BAF0: 0x00000000 */
    v0 += 2;  /* 0x8013BAF4: 0x24420002 */
    /* fall through to block_8013BAF8 */

block_8013BAF8:
    cpu->write_half(s4 + 7196, (uint16_t)v0);  /* 0x8013BAF8: 0xA6821C1C */
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 7196);  /* 0x8013BAFC: 0x86821C1C */
    /* nop */  /* 0x8013BB00: 0x00000000 */
    v0 = (int32_t)v0 < 16;  /* 0x8013BB04: 0x28420010 */
    int cond_8013BB08 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x55550000u;  /* 0x55550000 */  /* 0x8013BB0C: 0x3C025555 */
    if (cond_8013BB08) {
        psx_check_interrupts_at(cpu, 0x8013BB98u);
        goto block_8013BB98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BB10u);
    }

block_8013BB10:
    v0 = 16;  /* 0x8013BB10: 0x24020010 */
    cpu->write_half(s4 + 7196, (uint16_t)v0);  /* 0x8013BB14: 0xA6821C1C */
    v0 = 1;  /* 0x8013BB18: 0x24020001 */
    /* delay slot (always executes) */
    cpu->write_half(s4 + 7194, (uint16_t)v0);  /* 0x8013BB20: 0xA6821C1A */
    psx_check_interrupts_at(cpu, 0x8013BB94u);
    goto block_8013BB94;  /* j */

block_8013BB24:
    v0 = cpu->read_half(s4 + 7192);  /* 0x8013BB24: 0x96821C18 */
    /* nop */  /* 0x8013BB28: 0x00000000 */
    v0--;  /* 0x8013BB2C: 0x2442FFFF */
    cpu->write_half(s4 + 7192, (uint16_t)v0);  /* 0x8013BB30: 0xA6821C18 */
    v0 <<= 16;  /* 0x8013BB34: 0x00021400 */
    int cond_8013BB38 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x55550000u;  /* 0x55550000 */  /* 0x8013BB3C: 0x3C025555 */
    if (cond_8013BB38) {
        psx_check_interrupts_at(cpu, 0x8013BB98u);
        goto block_8013BB98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BB40u);
    }

block_8013BB40:
    v0 = 2;  /* 0x8013BB40: 0x24020002 */
    /* delay slot (always executes) */
    cpu->write_half(s4 + 7194, (uint16_t)v0);  /* 0x8013BB48: 0xA6821C1A */
    psx_check_interrupts_at(cpu, 0x8013BB94u);
    goto block_8013BB94;  /* j */

block_8013BB4C:
    v0 = cpu->read_half(s4 + 7196);  /* 0x8013BB4C: 0x96821C1C */
    /* nop */  /* 0x8013BB50: 0x00000000 */
    v0--;  /* 0x8013BB54: 0x2442FFFF */
    cpu->write_half(s4 + 7196, (uint16_t)v0);  /* 0x8013BB58: 0xA6821C1C */
    v0 = cpu->read_half(s4 + 7196);  /* 0x8013BB5C: 0x96821C1C */
    /* nop */  /* 0x8013BB60: 0x00000000 */
    v0 -= 2;  /* 0x8013BB64: 0x2442FFFE */
    cpu->write_half(s4 + 7196, (uint16_t)v0);  /* 0x8013BB68: 0xA6821C1C */
    v0 <<= 16;  /* 0x8013BB6C: 0x00021400 */
    int cond_8013BB70 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x55550000u;  /* 0x55550000 */  /* 0x8013BB74: 0x3C025555 */
    if (cond_8013BB70) {
        psx_check_interrupts_at(cpu, 0x8013BB98u);
        goto block_8013BB98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BB78u);
    }

block_8013BB78:
    a0 = cpu->read_word(s4 + 5772);  /* 0x8013BB78: 0x8E84168C */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BB80: 0x00000000 */
    r3_jal(0x801AC758u, 0x8013BB84u);  /* jal node_tree_free */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BB84u);
    goto block_8013BB84;

block_8013BB84:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BB88: 0x00000000 */
    r3_jal(0x801A482Cu, 0x8013BB8Cu);  /* jal task_slot_step */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BB8Cu);
    goto block_8013BB8C;

block_8013BB8C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BB90: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013C99Cu);
    cpu->pc = 0x8013C99Cu; return;  /* CPS j: split -> cps_8013c99c */

block_8013BB94:
    v0 = 0x55550000u;  /* 0x55550000 */  /* 0x8013BB94: 0x3C025555 */
    /* fall through to block_8013BB98 */

block_8013BB98:
    v0 |= 0x5556;  /* 0x8013BB98: 0x34425556 */
    /* fall through to block_8013BB9C */

block_8013BB9C:
    { int64_t result = (int64_t)(int32_t)s2 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s2));  /* 0x8013BB9C: 0x02420018 */
    a1 = sp + 64;  /* 0x8013BBA0: 0x27A50040 */
    v0 = (int32_t)s2 >> 31;  /* 0x8013BBA4: 0x001217C3 */
    cpu->write_half(sp + 64, (uint16_t)zero);  /* 0x8013BBA8: 0xA7A00040 */
    cpu->write_half(sp + 66, (uint16_t)zero);  /* 0x8013BBAC: 0xA7A00042 */
    t3 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8013BBB0: 0x00005810 */
    v0 = t3 - v0;  /* 0x8013BBB4: 0x01621023 */
    v0 = zero - v0;  /* 0x8013BBB8: 0x00021023 */
    cpu->write_half(sp + 68, (uint16_t)v0);  /* 0x8013BBBC: 0xA7A20044 */
    v0 = cpu->read_word(s4 + 5764);  /* 0x8013BBC0: 0x8E821684 */
    a2 = a1;  /* 0x8013BBC4: 0x00A03021 */
    a0 = cpu->read_word(v0 + 32);  /* 0x8013BBC8: 0x8C440020 */
    s1 = zero;  /* 0x8013BBCC: 0x00008821 */
    /* delay slot (always executes) */
    a0 += 192;  /* 0x8013BBD4: 0x248400C0 */
    r3_jal(0x801951B0u, 0x8013BBD8u);  /* jal gte_rtps_xform1 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BBD8u);
    goto block_8013BBD8;

block_8013BBD8:
    t1 = s1;  /* 0x8013BBD8: 0x02204821 */
    t2 = s4;  /* 0x8013BBDC: 0x02805021 */
    a1 = cpu->read_half(sp + 32);  /* 0x8013BBE0: 0x97A50020 */
    t0 = cpu->read_half(sp + 64);  /* 0x8013BBE4: 0x97A80040 */
    v1 = cpu->read_half(sp + 34);  /* 0x8013BBE8: 0x97A30022 */
    a2 = cpu->read_half(sp + 66);  /* 0x8013BBEC: 0x97A60042 */
    a0 = cpu->read_half(sp + 36);  /* 0x8013BBF0: 0x97A40024 */
    a3 = cpu->read_half(sp + 68);  /* 0x8013BBF4: 0x97A70044 */
    v0 = a1 + t0;  /* 0x8013BBF8: 0x00A81021 */
    cpu->write_half(sp + 40, (uint16_t)v0);  /* 0x8013BBFC: 0xA7A20028 */
    v0 = v1 + a2;  /* 0x8013BC00: 0x00661021 */
    cpu->write_half(sp + 42, (uint16_t)v0);  /* 0x8013BC04: 0xA7A2002A */
    v0 = a0 + a3;  /* 0x8013BC08: 0x00871021 */
    a1 -= t0;  /* 0x8013BC0C: 0x00A82823 */
    v1 -= a2;  /* 0x8013BC10: 0x00661823 */
    a0 -= a3;  /* 0x8013BC14: 0x00872023 */
    cpu->write_half(sp + 44, (uint16_t)v0);  /* 0x8013BC18: 0xA7A2002C */
    cpu->write_half(sp + 32, (uint16_t)a1);  /* 0x8013BC1C: 0xA7A50020 */
    cpu->write_half(sp + 34, (uint16_t)v1);  /* 0x8013BC20: 0xA7A30022 */
    cpu->write_half(sp + 36, (uint16_t)a0);  /* 0x8013BC24: 0xA7A40024 */
    t5 = r3_lwl(cpu, sp + 59, t5);  /* 0x8013BC28: 0x8BAD003B */
    t5 = r3_lwr(cpu, sp + 56, t5);  /* 0x8013BC2C: 0x9BAD0038 */
    t6 = r3_lwl(cpu, sp + 63, t6);  /* 0x8013BC30: 0x8BAE003F */
    t6 = r3_lwr(cpu, sp + 60, t6);  /* 0x8013BC34: 0x9BAE003C */
    r3_swl(cpu, sp + 51, t5);  /* 0x8013BC38: 0xABAD0033 */
    r3_swr(cpu, sp + 48, t5);  /* 0x8013BC3C: 0xBBAD0030 */
    r3_swl(cpu, sp + 55, t6);  /* 0x8013BC40: 0xABAE0037 */
    r3_swr(cpu, sp + 52, t6);  /* 0x8013BC44: 0xBBAE0034 */
    /* fall through to block_8013BC48 */

block_8013BC48:
    v0 = 4096;  /* 0x8013BC48: 0x24021000 */
    v0 -= t1;  /* 0x8013BC4C: 0x00491023 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BC50: 0x00420018 */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BC54: 0x00001812 */
    /* nop */  /* 0x8013BC58: 0x00000000 */
    int cond_8013BC5C = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)t1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BC60: 0x00490018 */
    if (cond_8013BC5C) {
        psx_check_interrupts_at(cpu, 0x8013BC68u);
        goto block_8013BC68;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BC64u);
    }

block_8013BC64:
    v1 += 8191;  /* 0x8013BC64: 0x24631FFF */
    /* fall through to block_8013BC68 */

block_8013BC68:
    a1 = (int32_t)v1 >> 13;  /* 0x8013BC68: 0x00032B43 */
    v0 = 0x00800000u;  /* 0x00800000 */  /* 0x8013BC6C: 0x3C020080 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BC70: 0x00005812 */
    v0 = t3 + v0;  /* 0x8013BC74: 0x01621021 */
    int cond_8013BC78 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, a1);  /* 0x8013BC7C: 0xAFA50010 */
    if (cond_8013BC78) {
        psx_check_interrupts_at(cpu, 0x8013BC84u);
        goto block_8013BC84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BC80u);
    }

block_8013BC80:
    v0 += 4095;  /* 0x8013BC80: 0x24420FFF */
    /* fall through to block_8013BC84 */

block_8013BC84:
    { int64_t result = (int64_t)(int32_t)t1 * (int64_t)(int32_t)t1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(t1));  /* 0x8013BC84: 0x01290018 */
    v1 = (int32_t)v0 >> 12;  /* 0x8013BC88: 0x00021B03 */
    a0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BC8C: 0x00002012 */
    int cond_8013BC90 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v1);  /* 0x8013BC94: 0xAFA30014 */
    if (cond_8013BC90) {
        psx_check_interrupts_at(cpu, 0x8013BC9Cu);
        goto block_8013BC9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BC98u);
    }

block_8013BC98:
    a0 += 8191;  /* 0x8013BC98: 0x24841FFF */
    /* fall through to block_8013BC9C */

block_8013BC9C:
    v0 = (int32_t)(int16_t)cpu->read_half(sp + 32);  /* 0x8013BC9C: 0x87A20020 */
    /* nop */  /* 0x8013BCA0: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));  /* 0x8013BCA4: 0x00A20018 */
    a2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BCA8: 0x00003012 */
    v0 = (int32_t)(int16_t)cpu->read_half(sp + 40);  /* 0x8013BCAC: 0x87A20028 */
    /* nop */  /* 0x8013BCB0: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));  /* 0x8013BCB4: 0x00620018 */
    a1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BCB8: 0x00002812 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 48);  /* 0x8013BCBC: 0x87A30030 */
    v0 = (int32_t)a0 >> 13;  /* 0x8013BCC0: 0x00041343 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BCC4: 0x00430018 */
    cpu->write_word(sp + 24, v0);  /* 0x8013BCC8: 0xAFA20018 */
    v0 = a2 + a1;  /* 0x8013BCCC: 0x00C51021 */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BCD0: 0x00001812 */
    v0 += v1;  /* 0x8013BCD4: 0x00431021 */
    int cond_8013BCD8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BCDC: 0x00000000 */
    if (cond_8013BCD8) {
        psx_check_interrupts_at(cpu, 0x8013BCE4u);
        goto block_8013BCE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BCE0u);
    }

block_8013BCE0:
    v0 += 4095;  /* 0x8013BCE0: 0x24420FFF */
    /* fall through to block_8013BCE4 */

block_8013BCE4:
    v0 = (int32_t)v0 >> 12;  /* 0x8013BCE4: 0x00021303 */
    cpu->write_half(t2 + 7056, (uint16_t)v0);  /* 0x8013BCE8: 0xA5421B90 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 34);  /* 0x8013BCEC: 0x87A30022 */
    v0 = cpu->read_word(sp + 16);  /* 0x8013BCF0: 0x8FA20010 */
    /* nop */  /* 0x8013BCF4: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BCF8: 0x00430018 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 42);  /* 0x8013BCFC: 0x87A3002A */
    a1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BD00: 0x00002812 */
    v0 = cpu->read_word(sp + 20);  /* 0x8013BD04: 0x8FA20014 */
    /* nop */  /* 0x8013BD08: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BD0C: 0x00430018 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 50);  /* 0x8013BD10: 0x87A30032 */
    a0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BD14: 0x00002012 */
    v0 = cpu->read_word(sp + 24);  /* 0x8013BD18: 0x8FA20018 */
    /* nop */  /* 0x8013BD1C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BD20: 0x00430018 */
    v0 = a1 + a0;  /* 0x8013BD24: 0x00A41021 */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BD28: 0x00001812 */
    v0 += v1;  /* 0x8013BD2C: 0x00431021 */
    int cond_8013BD30 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BD34: 0x00000000 */
    if (cond_8013BD30) {
        psx_check_interrupts_at(cpu, 0x8013BD3Cu);
        goto block_8013BD3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BD38u);
    }

block_8013BD38:
    v0 += 4095;  /* 0x8013BD38: 0x24420FFF */
    /* fall through to block_8013BD3C */

block_8013BD3C:
    v0 = (int32_t)v0 >> 12;  /* 0x8013BD3C: 0x00021303 */
    cpu->write_half(t2 + 7058, (uint16_t)v0);  /* 0x8013BD40: 0xA5421B92 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 36);  /* 0x8013BD44: 0x87A30024 */
    v0 = cpu->read_word(sp + 16);  /* 0x8013BD48: 0x8FA20010 */
    /* nop */  /* 0x8013BD4C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BD50: 0x00430018 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 44);  /* 0x8013BD54: 0x87A3002C */
    a1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BD58: 0x00002812 */
    v0 = cpu->read_word(sp + 20);  /* 0x8013BD5C: 0x8FA20014 */
    /* nop */  /* 0x8013BD60: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BD64: 0x00430018 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 52);  /* 0x8013BD68: 0x87A30034 */
    a0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BD6C: 0x00002012 */
    v0 = cpu->read_word(sp + 24);  /* 0x8013BD70: 0x8FA20018 */
    /* nop */  /* 0x8013BD74: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013BD78: 0x00430018 */
    v0 = a1 + a0;  /* 0x8013BD7C: 0x00A41021 */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BD80: 0x00001812 */
    v0 += v1;  /* 0x8013BD84: 0x00431021 */
    int cond_8013BD88 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BD8C: 0x00000000 */
    if (cond_8013BD88) {
        psx_check_interrupts_at(cpu, 0x8013BD94u);
        goto block_8013BD94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BD90u);
    }

block_8013BD90:
    v0 += 4095;  /* 0x8013BD90: 0x24420FFF */
    /* fall through to block_8013BD94 */

block_8013BD94:
    v0 = (int32_t)v0 >> 12;  /* 0x8013BD94: 0x00021303 */
    cpu->write_half(t2 + 7060, (uint16_t)v0);  /* 0x8013BD98: 0xA5421B94 */
    t1 += 512;  /* 0x8013BD9C: 0x25290200 */
    s1++;  /* 0x8013BDA0: 0x26310001 */
    v0 = (int32_t)s1 < 8;  /* 0x8013BDA4: 0x2A220008 */
    int cond_8013BDA8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t2 += 8;  /* 0x8013BDAC: 0x254A0008 */
    if (cond_8013BDA8) {
        psx_check_interrupts_at(cpu, 0x8013BC48u);
        goto block_8013BC48;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BDB0u);
    }

block_8013BDB0:
    v0 = (int32_t)s1 < 17;  /* 0x8013BDB0: 0x2A220011 */
    int cond_8013BDB4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t1 = zero;  /* 0x8013BDB8: 0x00004821 */
    if (cond_8013BDB4) {
        psx_check_interrupts_at(cpu, 0x8013BF2Cu);
        cpu->pc = 0x8013BF2Cu; return;  /* CPS taken: split -> cps_8013bf2c */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BDBCu);
    }

block_8013BDBC:
    v0 = s1 << 3;  /* 0x8013BDBC: 0x001110C0 */
    a3 = v0 + s4;  /* 0x8013BDC0: 0x00543821 */
    cpu->pc = 0x8013bdc4u; return;  /* fallthrough to split piece -> cps_8013bdc4 */
}


/* ======== stage_env_fx_task_emit (guest 0x8013BF2C) ========
 * Continuation piece of the same stage-env FX task callback: GTE
 * rot/trans forms (gte_rot_load/gte_tr_load + GTE cmd stream),
 * angle_from_xy + lut_cos_q12/lut_sin_q12 fan for per-particle rotation,
 * tails into sibling pieces cps_8013c5e0/c750/c540 and cps_8013c99c.
 * No prologue — entered via cpu->pc. 51 CPS blocks / 1508 bytes.
 * Port of cps_8013bf2c (SLES_017.22_full_09.c br2eu_func_8013BF2C). */
void stage_env_fx_task_emit(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;

/* Address: 0x8013BF2C, Size: 1504 bytes, Blocks: 51 */

block_8013BF2C:
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8013BF2C: 0x3C10801C */
    a0 = cpu->read_word(s0 + -6088);  /* 0x8013BF30: 0x8E04E838 */
    /* delay slot (always executes) */
    s1 = zero;  /* 0x8013BF38: 0x00008821 */
    r3_jal(0x80195210u, 0x8013BF3Cu);  /* jal gte_rot_load */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BF3Cu);
    goto block_8013BF3C;

block_8013BF3C:
    a0 = cpu->read_word(s0 + -6088);  /* 0x8013BF3C: 0x8E04E838 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013BF44: 0x00000000 */
    r3_jal(0x80195270u, 0x8013BF48u);  /* jal gte_tr_load */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BF48u);
    goto block_8013BF48;

block_8013BF48:
    a1 = sp + 80;  /* 0x8013BF48: 0x27A50050 */
    v1 = 7056;  /* 0x8013BF4C: 0x24031B90 */
    a0 = cpu->read_word(sp + 312);  /* 0x8013BF50: 0x8FA40138 */
    /* fall through to block_8013BF54 */

block_8013BF54:
    v0 = s4 + v1;  /* 0x8013BF54: 0x02831021 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));  /* 0x8013BF58: 0xC8400000 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));  /* 0x8013BF5C: 0xC8410004 */
    /* nop */  /* 0x8013BF60: 0x00000000 */
    /* nop */  /* 0x8013BF64: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x8013BF68: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((a0), (cpu->gte_data[14]));  /* 0x8013BF6C: 0xE88E0000 */
    gte_precision_store_word((a0), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */  /* 0x8013BF70: 0x480C9800 */
    /* nop */  /* 0x8013BF74: 0x00000000 */
    t4 = (int32_t)t4 >> 2;  /* 0x8013BF78: 0x000C6083 */
    cpu->write_word(a1, t4);  /* 0x8013BF7C: 0xACAC0000 */
    a1 += 4;  /* 0x8013BF80: 0x24A50004 */
    a0 += 4;  /* 0x8013BF84: 0x24840004 */
    s1++;  /* 0x8013BF88: 0x26310001 */
    v0 = (int32_t)s1 < 17;  /* 0x8013BF8C: 0x2A220011 */
    int cond_8013BF90 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 += 8;  /* 0x8013BF94: 0x24630008 */
    if (cond_8013BF90) {
        psx_check_interrupts_at(cpu, 0x8013BF54u);
        goto block_8013BF54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013BF98u);
    }

block_8013BF98:
    s1 = zero;  /* 0x8013BF98: 0x00008821 */
    a1 = cpu->read_word(sp + 312);  /* 0x8013BF9C: 0x8FA50138 */
    /* fall through to block_8013BFA0 */

block_8013BFA0:
    /* nop */  /* 0x8013BFA0: 0x00000000 */
    s0 = a1 + 4;  /* 0x8013BFA4: 0x24B00004 */
    v1 = (int32_t)(int16_t)cpu->read_half(a1 + 4);  /* 0x8013BFA8: 0x84A30004 */
    a0 = (int32_t)(int16_t)cpu->read_half(a1);  /* 0x8013BFAC: 0x84A40000 */
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 2);  /* 0x8013BFB0: 0x86020002 */
    a1 = (int32_t)(int16_t)cpu->read_half(a1 + 2);  /* 0x8013BFB4: 0x84A50002 */
    a0 = v1 - a0;  /* 0x8013BFB8: 0x00642023 */
    /* delay slot (always executes) */
    a1 = v0 - a1;  /* 0x8013BFC0: 0x00452823 */
    r3_jal(0x80142B3Cu, 0x8013BFC4u);  /* jal angle_from_xy */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013BFC4u);
    goto block_8013BFC4;

block_8013BFC4:
    a0 = s1 << 2;  /* 0x8013BFC4: 0x00112080 */
    a2 = sp + 152;  /* 0x8013BFC8: 0x27A60098 */
    v1 = a2 + a0;  /* 0x8013BFCC: 0x00C41821 */
    cpu->write_word(v1, v0);  /* 0x8013BFD0: 0xAC620000 */
    v0 = sp + a0;  /* 0x8013BFD4: 0x03A41021 */
    v1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013BFD8: 0x3C031F80 */
    v1 = (int32_t)(int16_t)cpu->read_half(v1 + 36);  /* 0x8013BFDC: 0x84630024 */
    v0 = cpu->read_word(v0 + 80);  /* 0x8013BFE0: 0x8C420050 */
    v1 <<= 5;  /* 0x8013BFE4: 0x00031940 */
    v0 <<= 2;  /* 0x8013BFE8: 0x00021080 */
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)v0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013BFEC: 0x0062001A */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013BFF0: 0x00001812 */
    s1++;  /* 0x8013BFF4: 0x26310001 */
    v0 = sp + a0;  /* 0x8013BFF8: 0x03A41021 */
    cpu->write_word(v0 + 224, v1);  /* 0x8013BFFC: 0xAC4300E0 */
    v0 = (int32_t)s1 < 16;  /* 0x8013C000: 0x2A220010 */
    int cond_8013C004 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s0;  /* 0x8013C008: 0x02002821 */
    if (cond_8013C004) {
        psx_check_interrupts_at(cpu, 0x8013BFA0u);
        goto block_8013BFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C00Cu);
    }

block_8013C00C:
    a3 = a2;  /* 0x8013C00C: 0x00C03821 */
    a1 = sp + 156;  /* 0x8013C010: 0x27A5009C */
    a2 = zero;  /* 0x8013C014: 0x00003021 */
    v0 = cpu->read_word(sp + 212);  /* 0x8013C018: 0x8FA200D4 */
    v1 = cpu->read_word(sp + 284);  /* 0x8013C01C: 0x8FA3011C */
    t0 = sp + 220;  /* 0x8013C020: 0x27A800DC */
    cpu->write_word(sp + 216, v0);  /* 0x8013C024: 0xAFA200D8 */
    cpu->write_word(sp + 288, v1);  /* 0x8013C028: 0xAFA30120 */
    /* fall through to block_8013C02C */

block_8013C02C:
    v0 = a3 + a2;  /* 0x8013C02C: 0x00E61021 */
    a0 = cpu->read_word(v0);  /* 0x8013C030: 0x8C440000 */
    v1 = cpu->read_word(a1);  /* 0x8013C034: 0x8CA30000 */
    /* nop */  /* 0x8013C038: 0x00000000 */
    v0 = a0 - v1;  /* 0x8013C03C: 0x00831023 */
    int cond_8013C040 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v0 < 2048;  /* 0x8013C044: 0x28420800 */
    if (cond_8013C040) {
        psx_check_interrupts_at(cpu, 0x8013C058u);
        goto block_8013C058;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C048u);
    }

block_8013C048:
    ;
    int cond_8013C048 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a3 + a2;  /* 0x8013C04C: 0x00E61021 */
    if (cond_8013C048) {
        psx_check_interrupts_at(cpu, 0x8013C068u);
        goto block_8013C068;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C050u);
    }

block_8013C050:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C054: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013C090u);
    goto block_8013C090;  /* j */

block_8013C058:
    v0 = v1 - a0;  /* 0x8013C058: 0x00641023 */
    v0 = (int32_t)v0 < 2048;  /* 0x8013C05C: 0x28420800 */
    int cond_8013C060 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a3 + a2;  /* 0x8013C064: 0x00E61021 */
    if (cond_8013C060) {
        psx_check_interrupts_at(cpu, 0x8013C090u);
        goto block_8013C090;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C068u);
    }

block_8013C068:
    v0 = cpu->read_word(v0);  /* 0x8013C068: 0x8C420000 */
    v1 = cpu->read_word(a1);  /* 0x8013C06C: 0x8CA30000 */
    /* nop */  /* 0x8013C070: 0x00000000 */
    v0 += v1;  /* 0x8013C074: 0x00431021 */
    v1 = (uint32_t)v0 >> 31;  /* 0x8013C078: 0x00021FC2 */
    v0 += v1;  /* 0x8013C07C: 0x00431021 */
    v0 = (int32_t)v0 >> 1;  /* 0x8013C080: 0x00021043 */
    v0 += 2048;  /* 0x8013C084: 0x24420800 */
    /* delay slot (always executes) */
    v0 &= 0x0FFF;  /* 0x8013C08C: 0x30420FFF */
    psx_check_interrupts_at(cpu, 0x8013C0ACu);
    goto block_8013C0AC;  /* j */

block_8013C090:
    v0 = cpu->read_word(v0);  /* 0x8013C090: 0x8C420000 */
    v1 = cpu->read_word(a1);  /* 0x8013C094: 0x8CA30000 */
    /* nop */  /* 0x8013C098: 0x00000000 */
    v0 += v1;  /* 0x8013C09C: 0x00431021 */
    v1 = (uint32_t)v0 >> 31;  /* 0x8013C0A0: 0x00021FC2 */
    v0 += v1;  /* 0x8013C0A4: 0x00431021 */
    v0 = (int32_t)v0 >> 1;  /* 0x8013C0A8: 0x00021043 */
    /* fall through to block_8013C0AC */

block_8013C0AC:
    cpu->write_word(a1, v0);  /* 0x8013C0AC: 0xACA20000 */
    a1 += 4;  /* 0x8013C0B0: 0x24A50004 */
    v0 = (int32_t)a1 < (int32_t)t0;  /* 0x8013C0B4: 0x00A8102A */
    int cond_8013C0B8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 += 4;  /* 0x8013C0BC: 0x24C60004 */
    if (cond_8013C0B8) {
        psx_check_interrupts_at(cpu, 0x8013C02Cu);
        goto block_8013C02C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C0C0u);
    }

block_8013C0C0:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013C0C0: 0x3C021F80 */
    v0 = cpu->read_word(v0 + 12);  /* 0x8013C0C4: 0x8C42000C */
    s1 = zero;  /* 0x8013C0C8: 0x00008821 */
    v1 = v0 << 2;  /* 0x8013C0CC: 0x00021880 */
    v1 += v0;  /* 0x8013C0D0: 0x00621821 */
    v1 <<= 7;  /* 0x8013C0D4: 0x000319C0 */
    v1 += 5776;  /* 0x8013C0D8: 0x24631690 */
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 7196);  /* 0x8013C0DC: 0x86821C1C */
    v1 = s4 + v1;  /* 0x8013C0E0: 0x02831821 */
    int cond_8013C0E4 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 316, v1);  /* 0x8013C0E8: 0xAFA3013C */
    if (cond_8013C0E4) {
        psx_check_interrupts_at(cpu, 0x8013C414u);
        goto block_8013C414;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C0ECu);
    }

block_8013C0EC:
    t3 = sp + 224;  /* 0x8013C0EC: 0x27AB00E0 */
    fp = t3;  /* 0x8013C0F0: 0x0160F021 */
    t5 = sp + 152;  /* 0x8013C0F4: 0x27AD0098 */
    s7 = t5;  /* 0x8013C0F8: 0x01A0B821 */
    t6 = 4;  /* 0x8013C0FC: 0x240E0004 */
    s2 = cpu->read_word(sp + 312);  /* 0x8013C100: 0x8FB20138 */
    s0 = v1 + 34;  /* 0x8013C104: 0x24700022 */
    cpu->write_word(sp + 328, zero);  /* 0x8013C108: 0xAFA00148 */
    cpu->write_word(sp + 324, t3);  /* 0x8013C10C: 0xAFAB0144 */
    cpu->write_word(sp + 320, t5);  /* 0x8013C110: 0xAFAD0140 */
    cpu->write_word(sp + 332, t6);  /* 0x8013C114: 0xAFAE014C */
    /* fall through to block_8013C118 */

block_8013C118:
    t7 = cpu->read_word(sp + 328);  /* 0x8013C118: 0x8FAF0148 */
    /* nop */  /* 0x8013C11C: 0x00000000 */
    v0 = sp + t7;  /* 0x8013C120: 0x03AF1021 */
    v0 = cpu->read_word(v0 + 80);  /* 0x8013C124: 0x8C420050 */
    /* nop */  /* 0x8013C128: 0x00000000 */
    v0 -= 4;  /* 0x8013C12C: 0x2442FFFC */
    v0 = v0 < (uint32_t)2059;  /* 0x8013C130: 0x2C42080B */
    int cond_8013C134 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C138: 0x00000000 */
    if (cond_8013C134) {
        psx_check_interrupts_at(cpu, 0x8013C3D0u);
        goto block_8013C3D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C13Cu);
    }

block_8013C13C:
    a0 = cpu->read_word(s7);  /* 0x8013C13C: 0x8EE40000 */
    /* delay slot (always executes) */
    s6 = s2 + 4;  /* 0x8013C144: 0x26560004 */
    r3_jal(0x8018D298u, 0x8013C148u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C148u);
    goto block_8013C148;

block_8013C148:
    v1 = cpu->read_word(fp);  /* 0x8013C148: 0x8FC30000 */
    /* nop */  /* 0x8013C14C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C150: 0x00430018 */
    v0 = cpu->read_half(s2);  /* 0x8013C154: 0x96420000 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C158: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C15C: 0x000B1B03 */
    v0 += v1;  /* 0x8013C160: 0x00431021 */
    cpu->write_half(s0 + -26, (uint16_t)v0);  /* 0x8013C164: 0xA602FFE6 */
    a0 = cpu->read_word(s7);  /* 0x8013C168: 0x8EE40000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C170: 0x00000000 */
    r3_jal(0x8018D298u, 0x8013C174u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C174u);
    goto block_8013C174;

block_8013C174:
    v1 = cpu->read_word(fp);  /* 0x8013C174: 0x8FC30000 */
    /* nop */  /* 0x8013C178: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C17C: 0x00430018 */
    v0 = cpu->read_half(s2);  /* 0x8013C180: 0x96420000 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C184: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C188: 0x000B1B03 */
    v0 -= v1;  /* 0x8013C18C: 0x00431023 */
    cpu->write_half(s0 + -10, (uint16_t)v0);  /* 0x8013C190: 0xA602FFF6 */
    t5 = cpu->read_word(sp + 320);  /* 0x8013C194: 0x8FAD0140 */
    t6 = cpu->read_word(sp + 332);  /* 0x8013C198: 0x8FAE014C */
    /* nop */  /* 0x8013C19C: 0x00000000 */
    s3 = t5 + t6;  /* 0x8013C1A0: 0x01AE9821 */
    a0 = cpu->read_word(s3);  /* 0x8013C1A4: 0x8E640000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C1AC: 0x00000000 */
    r3_jal(0x8018D298u, 0x8013C1B0u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C1B0u);
    goto block_8013C1B0;

block_8013C1B0:
    t7 = cpu->read_word(sp + 324);  /* 0x8013C1B0: 0x8FAF0144 */
    t3 = cpu->read_word(sp + 332);  /* 0x8013C1B4: 0x8FAB014C */
    /* nop */  /* 0x8013C1B8: 0x00000000 */
    s5 = t7 + t3;  /* 0x8013C1BC: 0x01EBA821 */
    v1 = cpu->read_word(s5);  /* 0x8013C1C0: 0x8EA30000 */
    /* nop */  /* 0x8013C1C4: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C1C8: 0x00430018 */
    v0 = cpu->read_half(s2 + 4);  /* 0x8013C1CC: 0x96420004 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C1D0: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C1D4: 0x000B1B03 */
    v0 += v1;  /* 0x8013C1D8: 0x00431021 */
    cpu->write_half(s0 + -18, (uint16_t)v0);  /* 0x8013C1DC: 0xA602FFEE */
    a0 = cpu->read_word(s3);  /* 0x8013C1E0: 0x8E640000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C1E8: 0x00000000 */
    r3_jal(0x8018D298u, 0x8013C1ECu);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C1ECu);
    goto block_8013C1EC;

block_8013C1EC:
    v1 = cpu->read_word(s5);  /* 0x8013C1EC: 0x8EA30000 */
    /* nop */  /* 0x8013C1F0: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C1F4: 0x00430018 */
    v0 = cpu->read_half(s2 + 4);  /* 0x8013C1F8: 0x96420004 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C1FC: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C200: 0x000B1B03 */
    v0 -= v1;  /* 0x8013C204: 0x00431023 */
    cpu->write_half(s0 + -2, (uint16_t)v0);  /* 0x8013C208: 0xA602FFFE */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013C20C: 0x3C021F80 */
    v0 = cpu->read_byte(v0 + 21);  /* 0x8013C210: 0x90420015 */
    /* nop */  /* 0x8013C214: 0x00000000 */
    int cond_8013C218 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C21C: 0x00000000 */
    if (cond_8013C218) {
        psx_check_interrupts_at(cpu, 0x8013C2CCu);
        goto block_8013C2CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C220u);
    }

block_8013C220:
    a0 = cpu->read_word(s7);  /* 0x8013C220: 0x8EE40000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C228: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C22Cu);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C22Cu);
    goto block_8013C22C;

block_8013C22C:
    v1 = cpu->read_word(fp);  /* 0x8013C22C: 0x8FC30000 */
    /* nop */  /* 0x8013C230: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C234: 0x00430018 */
    v0 = cpu->read_half(s2 + 2);  /* 0x8013C238: 0x96420002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C23C: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C240: 0x000B1B03 */
    v0 += v1;  /* 0x8013C244: 0x00431021 */
    cpu->write_half(s0 + -24, (uint16_t)v0);  /* 0x8013C248: 0xA602FFE8 */
    a0 = cpu->read_word(s7);  /* 0x8013C24C: 0x8EE40000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C254: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C258u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C258u);
    goto block_8013C258;

block_8013C258:
    v1 = cpu->read_word(fp);  /* 0x8013C258: 0x8FC30000 */
    /* nop */  /* 0x8013C25C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C260: 0x00430018 */
    v0 = cpu->read_half(s2 + 2);  /* 0x8013C264: 0x96420002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C268: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C26C: 0x000B1B03 */
    v0 -= v1;  /* 0x8013C270: 0x00431023 */
    cpu->write_half(s0 + -8, (uint16_t)v0);  /* 0x8013C274: 0xA602FFF8 */
    a0 = cpu->read_word(s3);  /* 0x8013C278: 0x8E640000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C280: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C284u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C284u);
    goto block_8013C284;

block_8013C284:
    v1 = cpu->read_word(s5);  /* 0x8013C284: 0x8EA30000 */
    /* nop */  /* 0x8013C288: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C28C: 0x00430018 */
    v0 = cpu->read_half(s6 + 2);  /* 0x8013C290: 0x96C20002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C294: 0x00005812 */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C298: 0x000B1B03 */
    v0 += v1;  /* 0x8013C29C: 0x00431021 */
    cpu->write_half(s0 + -16, (uint16_t)v0);  /* 0x8013C2A0: 0xA602FFF0 */
    a0 = cpu->read_word(s3);  /* 0x8013C2A4: 0x8E640000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C2AC: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C2B0u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C2B0u);
    goto block_8013C2B0;

block_8013C2B0:
    v1 = cpu->read_word(s5);  /* 0x8013C2B0: 0x8EA30000 */
    /* nop */  /* 0x8013C2B4: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C2B8: 0x00430018 */
    v0 = cpu->read_half(s6 + 2);  /* 0x8013C2BC: 0x96C20002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C2C0: 0x00005812 */
    /* delay slot (always executes) */
    v1 = (int32_t)t3 >> 12;  /* 0x8013C2C8: 0x000B1B03 */
    psx_check_interrupts_at(cpu, 0x8013C374u);
    goto block_8013C374;  /* j */

block_8013C2CC:
    a0 = cpu->read_word(s7);  /* 0x8013C2CC: 0x8EE40000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C2D4: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C2D8u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C2D8u);
    goto block_8013C2D8;

block_8013C2D8:
    v1 = cpu->read_word(fp);  /* 0x8013C2D8: 0x8FC30000 */
    /* nop */  /* 0x8013C2DC: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C2E0: 0x00430018 */
    v0 = cpu->read_half(s2 + 2);  /* 0x8013C2E4: 0x96420002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C2E8: 0x00005812 */
    v1 = (int32_t)t3 >> 13;  /* 0x8013C2EC: 0x000B1B43 */
    v0 += v1;  /* 0x8013C2F0: 0x00431021 */
    cpu->write_half(s0 + -24, (uint16_t)v0);  /* 0x8013C2F4: 0xA602FFE8 */
    a0 = cpu->read_word(s7);  /* 0x8013C2F8: 0x8EE40000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C300: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C304u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C304u);
    goto block_8013C304;

block_8013C304:
    v1 = cpu->read_word(fp);  /* 0x8013C304: 0x8FC30000 */
    /* nop */  /* 0x8013C308: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C30C: 0x00430018 */
    v0 = cpu->read_half(s2 + 2);  /* 0x8013C310: 0x96420002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C314: 0x00005812 */
    v1 = (int32_t)t3 >> 13;  /* 0x8013C318: 0x000B1B43 */
    v0 -= v1;  /* 0x8013C31C: 0x00431023 */
    cpu->write_half(s0 + -8, (uint16_t)v0);  /* 0x8013C320: 0xA602FFF8 */
    a0 = cpu->read_word(s3);  /* 0x8013C324: 0x8E640000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C32C: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C330u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C330u);
    goto block_8013C330;

block_8013C330:
    v1 = cpu->read_word(s5);  /* 0x8013C330: 0x8EA30000 */
    /* nop */  /* 0x8013C334: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C338: 0x00430018 */
    v0 = cpu->read_half(s6 + 2);  /* 0x8013C33C: 0x96C20002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C340: 0x00005812 */
    v1 = (int32_t)t3 >> 13;  /* 0x8013C344: 0x000B1B43 */
    v0 += v1;  /* 0x8013C348: 0x00431021 */
    cpu->write_half(s0 + -16, (uint16_t)v0);  /* 0x8013C34C: 0xA602FFF0 */
    a0 = cpu->read_word(s3);  /* 0x8013C350: 0x8E640000 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C358: 0x00000000 */
    r3_jal(0x8018D29Cu, 0x8013C35Cu);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C35Cu);
    goto block_8013C35C;

block_8013C35C:
    v1 = cpu->read_word(s5);  /* 0x8013C35C: 0x8EA30000 */
    /* nop */  /* 0x8013C360: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013C364: 0x00430018 */
    v0 = cpu->read_half(s6 + 2);  /* 0x8013C368: 0x96C20002 */
    t3 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C36C: 0x00005812 */
    v1 = (int32_t)t3 >> 13;  /* 0x8013C370: 0x000B1B43 */
    /* fall through to block_8013C374 */

block_8013C374:
    v0 -= v1;  /* 0x8013C374: 0x00431023 */
    cpu->write_half(s0, (uint16_t)v0);  /* 0x8013C378: 0xA6020000 */
    a0 = sp + 80;  /* 0x8013C37C: 0x27A40050 */
    t5 = cpu->read_word(sp + 328);  /* 0x8013C380: 0x8FAD0148 */
    v1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013C384: 0x3C031F80 */
    v1 = cpu->read_word(v1 + 8);  /* 0x8013C388: 0x8C630008 */
    a0 += t5;  /* 0x8013C38C: 0x008D2021 */
    v0 = cpu->read_word(a0);  /* 0x8013C390: 0x8C820000 */
    t6 = cpu->read_word(sp + 316);  /* 0x8013C394: 0x8FAE013C */
    v0 <<= 2;  /* 0x8013C398: 0x00021080 */
    v0 += v1;  /* 0x8013C39C: 0x00431021 */
    v0 = cpu->read_word(v0);  /* 0x8013C3A0: 0x8C420000 */
    v1 = 0x09000000u;  /* 0x09000000 */  /* 0x8013C3A4: 0x3C030900 */
    v0 |= v1;  /* 0x8013C3A8: 0x00431025 */
    cpu->write_word(t6, v0);  /* 0x8013C3AC: 0xADC20000 */
    v1 = cpu->read_word(a0);  /* 0x8013C3B0: 0x8C830000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013C3B4: 0x3C021F80 */
    v0 = cpu->read_word(v0 + 8);  /* 0x8013C3B8: 0x8C420008 */
    v1 <<= 2;  /* 0x8013C3BC: 0x00031880 */
    v1 += v0;  /* 0x8013C3C0: 0x00621821 */
    v0 = t6 << 8;  /* 0x8013C3C4: 0x000E1200 */
    v0 = (uint32_t)v0 >> 8;  /* 0x8013C3C8: 0x00021202 */
    cpu->write_word(v1, v0);  /* 0x8013C3CC: 0xAC620000 */
    /* fall through to block_8013C3D0 */

block_8013C3D0:
    s0 += 40;  /* 0x8013C3D0: 0x26100028 */
    fp += 4;  /* 0x8013C3D4: 0x27DE0004 */
    s7 += 4;  /* 0x8013C3D8: 0x26F70004 */
    t7 = cpu->read_word(sp + 316);  /* 0x8013C3DC: 0x8FAF013C */
    t3 = cpu->read_word(sp + 328);  /* 0x8013C3E0: 0x8FAB0148 */
    t5 = cpu->read_word(sp + 332);  /* 0x8013C3E4: 0x8FAD014C */
    t7 += 40;  /* 0x8013C3E8: 0x25EF0028 */
    t3 += 4;  /* 0x8013C3EC: 0x256B0004 */
    t5 += 4;  /* 0x8013C3F0: 0x25AD0004 */
    cpu->write_word(sp + 316, t7);  /* 0x8013C3F4: 0xAFAF013C */
    cpu->write_word(sp + 328, t3);  /* 0x8013C3F8: 0xAFAB0148 */
    cpu->write_word(sp + 332, t5);  /* 0x8013C3FC: 0xAFAD014C */
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 7196);  /* 0x8013C400: 0x86821C1C */
    s1++;  /* 0x8013C404: 0x26310001 */
    v0 = (int32_t)s1 < (int32_t)v0;  /* 0x8013C408: 0x0222102A */
    int cond_8013C40C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 += 4;  /* 0x8013C410: 0x26520004 */
    if (cond_8013C40C) {
        psx_check_interrupts_at(cpu, 0x8013C118u);
        goto block_8013C118;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C414u);
    }

block_8013C414:
    v1 = (int32_t)(int16_t)cpu->read_half(s4 + 7194);  /* 0x8013C414: 0x86831C1A */
    v0 = 1;  /* 0x8013C418: 0x24020001 */
    int cond_8013C41C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 2;  /* 0x8013C420: 0x28620002 */
    if (cond_8013C41C) {
        psx_check_interrupts_at(cpu, 0x8013C5E0u);
        cpu->pc = 0x8013C5E0u; return;  /* CPS taken: split -> cps_8013c5e0 */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C424u);
    }

block_8013C424:
    ;
    int cond_8013C424 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;  /* 0x8013C428: 0x24020002 */
    if (cond_8013C424) {
        psx_check_interrupts_at(cpu, 0x8013C43Cu);
        goto block_8013C43C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C42Cu);
    }

block_8013C42C:
    ;
    int cond_8013C42C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C430: 0x00000000 */
    if (cond_8013C42C) {
        psx_check_interrupts_at(cpu, 0x8013C444u);
        goto block_8013C444;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C434u);
    }

block_8013C434:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C438: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013C750u);
    cpu->pc = 0x8013C750u; return;  /* CPS j: split -> cps_8013c750 */

block_8013C43C:
    ;
    int cond_8013C43C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C440: 0x00000000 */
    if (cond_8013C43C) {
        psx_check_interrupts_at(cpu, 0x8013C750u);
        cpu->pc = 0x8013C750u; return;  /* CPS taken: split -> cps_8013c750 */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C444u);
    }

block_8013C444:
    v0 = (int32_t)(int16_t)cpu->read_half(s4 + 7196);  /* 0x8013C444: 0x86821C1C */
    v1 = cpu->read_word(s4 + 5768);  /* 0x8013C448: 0x8E831688 */
    v0 <<= 3;  /* 0x8013C44C: 0x000210C0 */
    v0 = s4 + v0;  /* 0x8013C450: 0x02821021 */
    v1 = cpu->read_word(v1 + 32);  /* 0x8013C454: 0x8C630020 */
    a2 = (int32_t)(int16_t)cpu->read_half(v0 + 7056);  /* 0x8013C458: 0x84461B90 */
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 7060);  /* 0x8013C45C: 0x84421B94 */
    a0 = cpu->read_word(v1 + 212);  /* 0x8013C460: 0x8C6400D4 */
    a1 = cpu->read_word(v1 + 220);  /* 0x8013C464: 0x8C6500DC */
    a0 -= a2;  /* 0x8013C468: 0x00862023 */
    /* delay slot (always executes) */
    a1 -= v0;  /* 0x8013C470: 0x00A22823 */
    r3_jal(0x80142B3Cu, 0x8013C474u);  /* jal angle_from_xy */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C474u);
    goto block_8013C474;

block_8013C474:
    s0 = v0;  /* 0x8013C474: 0x00408021 */
    /* delay slot (always executes) */
    a0 = s0;  /* 0x8013C47C: 0x02002021 */
    r3_jal(0x8018D298u, 0x8013C480u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C480u);
    goto block_8013C480;

block_8013C480:
    a1 = (int32_t)(int16_t)cpu->read_half(sp + 56);  /* 0x8013C480: 0x87A50038 */
    a0 = (int32_t)(int16_t)cpu->read_half(sp + 32);  /* 0x8013C484: 0x87A40020 */
    v1 = v0;  /* 0x8013C488: 0x00401821 */
    v0 = a1 - a0;  /* 0x8013C48C: 0x00A41023 */
    int cond_8013C490 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 <<= 12;  /* 0x8013C494: 0x00021300 */
    if (cond_8013C490) {
        psx_check_interrupts_at(cpu, 0x8013C4B4u);
        goto block_8013C4B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C498u);
    }

block_8013C498:
    if (v1 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)v1); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)v1); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013C498: 0x0043001A */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C49C: 0x00001012 */
    /* nop */  /* 0x8013C4A0: 0x00000000 */
    int cond_8013C4A4 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C4A8: 0x00000000 */
    if (cond_8013C4A4) {
        psx_check_interrupts_at(cpu, 0x8013C508u);
        goto block_8013C508;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C4ACu);
    }

block_8013C4AC:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C4B0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013C4D0u);
    goto block_8013C4D0;  /* j */

block_8013C4B4:
    v0 = a0 - a1;  /* 0x8013C4B4: 0x00851023 */
    v0 <<= 12;  /* 0x8013C4B8: 0x00021300 */
    if (v1 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)v1); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)v1); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013C4BC: 0x0043001A */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C4C0: 0x00001012 */
    /* nop */  /* 0x8013C4C4: 0x00000000 */
    int cond_8013C4C8 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C4CC: 0x00000000 */
    if (cond_8013C4C8) {
        psx_check_interrupts_at(cpu, 0x8013C508u);
        goto block_8013C508;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C4D0u);
    }

block_8013C4D0:
    /* delay slot (always executes) */
    a0 = s0;  /* 0x8013C4D4: 0x02002021 */
    r3_jal(0x8018D298u, 0x8013C4D8u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C4D8u);
    goto block_8013C4D8;

block_8013C4D8:
    a1 = (int32_t)(int16_t)cpu->read_half(sp + 56);  /* 0x8013C4D8: 0x87A50038 */
    a0 = (int32_t)(int16_t)cpu->read_half(sp + 32);  /* 0x8013C4DC: 0x87A40020 */
    v1 = v0;  /* 0x8013C4E0: 0x00401821 */
    v0 = a1 - a0;  /* 0x8013C4E4: 0x00A41023 */
    int cond_8013C4E8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 <<= 12;  /* 0x8013C4EC: 0x00021300 */
    if (cond_8013C4E8) {
        psx_check_interrupts_at(cpu, 0x8013C4F8u);
        goto block_8013C4F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013C4F0u);
    }

block_8013C4F0:
    v0 = a0 - a1;  /* 0x8013C4F0: 0x00851023 */
    v0 <<= 12;  /* 0x8013C4F4: 0x00021300 */
    /* fall through to block_8013C4F8 */

block_8013C4F8:
    if (v1 != 0) { cpu->lo = (uint32_t)((int32_t)v0 / (int32_t)v1); cpu->hi = (uint32_t)((int32_t)v0 % (int32_t)v1); } else { cpu->lo = ((int32_t)v0 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x8013C4F8: 0x0043001A */
    s2 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013C4FC: 0x00009012 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013C504: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013C540u);
    cpu->pc = 0x8013C540u; return;  /* CPS j: split -> cps_8013c540 */

block_8013C508:
    /* delay slot (always executes) */
    a0 = s0;  /* 0x8013C50C: 0x02002021 */
    r3_jal(0x8018D298u, 0x8013C510u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013C510u);
    cpu->pc = 0x8013C510u; return;  /* resume in sibling piece */
}


/* ======== emit_fx_task_update (guest 0x8013CE68) ========
 * Per-tick update for the FX emit task spawned by emit_task_spawn
 * (task_node_alloc cb 0x8013CE68, 0x62*kind+0x2c payload): advances the
 * emit record (rand_lcg, lut_cos/sin, task_slot_step) and tail-jals
 * emit_task_spawn's sibling. Real entry — own 64-byte frame
 * (s0-s7/fp/ra). 62 CPS blocks / 2120 bytes.
 * Port of cps_8013ce68 (SLES_017.22_full_09.c br2eu_func_8013CE68). */
void emit_fx_task_update(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;

/* Address: 0x8013CE68, Size: 2120 bytes, Blocks: 62 */

block_8013CE68:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013CE68: 0x3C021F80 */
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 130);  /* 0x8013CE6C: 0x84420082 */
    sp -= 64;  /* 0x8013CE70: 0x27BDFFC0 */
    cpu->write_word(sp + 36, s3);  /* 0x8013CE74: 0xAFB30024 */
    s3 = a0;  /* 0x8013CE78: 0x00809821 */
    cpu->write_word(sp + 60, ra);  /* 0x8013CE7C: 0xAFBF003C */
    cpu->write_word(sp + 56, fp);  /* 0x8013CE80: 0xAFBE0038 */
    cpu->write_word(sp + 52, s7);  /* 0x8013CE84: 0xAFB70034 */
    cpu->write_word(sp + 48, s6);  /* 0x8013CE88: 0xAFB60030 */
    cpu->write_word(sp + 44, s5);  /* 0x8013CE8C: 0xAFB5002C */
    cpu->write_word(sp + 40, s4);  /* 0x8013CE90: 0xAFB40028 */
    cpu->write_word(sp + 32, s2);  /* 0x8013CE94: 0xAFB20020 */
    cpu->write_word(sp + 28, s1);  /* 0x8013CE98: 0xAFB1001C */
    int cond_8013CE9C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, s0);  /* 0x8013CEA0: 0xAFB00018 */
    if (cond_8013CE9C) {
        psx_check_interrupts_at(cpu, 0x8013D2E0u);
        goto block_8013D2E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CEA4u);
    }

block_8013CEA4:
    a0 = cpu->read_half(s3 + 32);  /* 0x8013CEA4: 0x96640020 */
    v0 = cpu->read_half(s3 + 28);  /* 0x8013CEA8: 0x9662001C */
    a1 = (int32_t)(int16_t)cpu->read_half(s3 + 32);  /* 0x8013CEAC: 0x86650020 */
    v1 = (int32_t)(int16_t)cpu->read_half(s3 + 30);  /* 0x8013CEB0: 0x8663001E */
    v0++;  /* 0x8013CEB4: 0x24420001 */
    v1 = (int32_t)a1 < (int32_t)v1;  /* 0x8013CEB8: 0x00A3182A */
    int cond_8013CEBC = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s3 + 28, (uint16_t)v0);  /* 0x8013CEC0: 0xA662001C */
    if (cond_8013CEBC) {
        psx_check_interrupts_at(cpu, 0x8013D2E0u);
        goto block_8013D2E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CEC4u);
    }

block_8013CEC4:
    v0 = 1;  /* 0x8013CEC4: 0x24020001 */
    int cond_8013CEC8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = a1;  /* 0x8013CECC: 0x00A0A821 */
    if (cond_8013CEC8) {
        psx_check_interrupts_at(cpu, 0x8013D2E0u);
        goto block_8013D2E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CED0u);
    }

block_8013CED0:
    s6 = 128;  /* 0x8013CED0: 0x24160080 */
    v0 = s5 << 2;  /* 0x8013CED4: 0x00151080 */
    v0 += s5;  /* 0x8013CED8: 0x00551021 */
    fp = v0 << 3;  /* 0x8013CEDC: 0x0002F0C0 */
    /* fall through to block_8013CEE0 */

block_8013CEE0:
    v0 = a0 << 16;  /* 0x8013CEE0: 0x00041400 */
    v1 = (int32_t)(int16_t)cpu->read_half(s3 + 30);  /* 0x8013CEE4: 0x8663001E */
    v0 = (int32_t)v0 >> 16;  /* 0x8013CEE8: 0x00021403 */
    v0 = (int32_t)v0 < (int32_t)v1;  /* 0x8013CEEC: 0x0043102A */
    int cond_8013CEF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t5 = zero;  /* 0x8013CEF4: 0x00006821 */
    if (cond_8013CEF0) {
        psx_check_interrupts_at(cpu, 0x8013D2C4u);
        goto block_8013D2C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CEF8u);
    }

block_8013CEF8:
    a1 = fp;  /* 0x8013CEF8: 0x03C02821 */
    t1 = 72;  /* 0x8013CEFC: 0x24090048 */
    a2 = 136;  /* 0x8013CF00: 0x24060088 */
    t0 = 96;  /* 0x8013CF04: 0x24080060 */
    a3 = 104;  /* 0x8013CF08: 0x24070068 */
    a0 = s3;  /* 0x8013CF0C: 0x02602021 */
    /* fall through to block_8013CF10 */

block_8013CF10:
    v0 = cpu->read_word(a0);  /* 0x8013CF10: 0x8C820000 */
    v1 = 0x09000000u;  /* 0x09000000 */  /* 0x8013CF14: 0x3C030900 */
    v0 = a1 + v0;  /* 0x8013CF18: 0x00A21021 */
    cpu->write_word(v0, v1);  /* 0x8013CF1C: 0xAC430000 */
    v0 = cpu->read_word(a0);  /* 0x8013CF20: 0x8C820000 */
    v1 = 44;  /* 0x8013CF24: 0x2403002C */
    v0 = a1 + v0;  /* 0x8013CF28: 0x00A21021 */
    cpu->write_byte(v0 + 7, (uint8_t)v1);  /* 0x8013CF2C: 0xA0430007 */
    v0 = cpu->read_word(a0);  /* 0x8013CF30: 0x8C820000 */
    v1 = 63;  /* 0x8013CF34: 0x2403003F */
    v0 = a1 + v0;  /* 0x8013CF38: 0x00A21021 */
    cpu->write_half(v0 + 22, (uint16_t)v1);  /* 0x8013CF3C: 0xA4430016 */
    v0 = cpu->read_word(a0);  /* 0x8013CF40: 0x8C820000 */
    /* nop */  /* 0x8013CF44: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013CF48: 0x00A21021 */
    t6 = 127;  /* 0x8013CF4C: 0x240E007F */
    cpu->write_byte(v0 + 4, (uint8_t)t6);  /* 0x8013CF50: 0xA04E0004 */
    v0 = cpu->read_word(a0);  /* 0x8013CF54: 0x8C820000 */
    /* nop */  /* 0x8013CF58: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013CF5C: 0x00A21021 */
    s7 = t6;  /* 0x8013CF60: 0x01C0B821 */
    cpu->write_byte(v0 + 5, (uint8_t)s7);  /* 0x8013CF64: 0xA0570005 */
    v0 = cpu->read_word(a0);  /* 0x8013CF68: 0x8C820000 */
    /* nop */  /* 0x8013CF6C: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013CF70: 0x00A21021 */
    t8 = t6;  /* 0x8013CF74: 0x01C0C021 */
    cpu->write_byte(v0 + 6, (uint8_t)t8);  /* 0x8013CF78: 0xA0580006 */
    v1 = cpu->read_word(a0);  /* 0x8013CF7C: 0x8C830000 */
    /* nop */  /* 0x8013CF80: 0x00000000 */
    v1 = a1 + v1;  /* 0x8013CF84: 0x00A31821 */
    v0 = cpu->read_byte(v1 + 7);  /* 0x8013CF88: 0x90620007 */
    /* nop */  /* 0x8013CF8C: 0x00000000 */
    v0 &= 0x00FE;  /* 0x8013CF90: 0x304200FE */
    cpu->write_byte(v1 + 7, (uint8_t)v0);  /* 0x8013CF94: 0xA0620007 */
    v1 = cpu->read_word(a0);  /* 0x8013CF98: 0x8C830000 */
    /* nop */  /* 0x8013CF9C: 0x00000000 */
    v1 = a1 + v1;  /* 0x8013CFA0: 0x00A31821 */
    v0 = cpu->read_byte(v1 + 7);  /* 0x8013CFA4: 0x90620007 */
    /* nop */  /* 0x8013CFA8: 0x00000000 */
    v0 |= 0x0002;  /* 0x8013CFAC: 0x34420002 */
    cpu->write_byte(v1 + 7, (uint8_t)v0);  /* 0x8013CFB0: 0xA0620007 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 40);  /* 0x8013CFB4: 0x86620028 */
    /* nop */  /* 0x8013CFB8: 0x00000000 */
    int cond_8013CFBC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 31000;  /* 0x8013CFC0: 0x24037918 */
    if (cond_8013CFBC) {
        psx_check_interrupts_at(cpu, 0x8013D050u);
        goto block_8013D050;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CFC4u);
    }

block_8013CFC4:
    v0 = cpu->read_word(a0);  /* 0x8013CFC4: 0x8C820000 */
    v1 = 30744;  /* 0x8013CFC8: 0x24037818 */
    v0 = a1 + v0;  /* 0x8013CFCC: 0x00A21021 */
    cpu->write_half(v0 + 14, (uint16_t)v1);  /* 0x8013CFD0: 0xA443000E */
    v0 = cpu->read_word(a0);  /* 0x8013CFD4: 0x8C820000 */
    /* nop */  /* 0x8013CFD8: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013CFDC: 0x00A21021 */
    t9 = 64;  /* 0x8013CFE0: 0x24190040 */
    cpu->write_byte(v0 + 12, (uint8_t)t9);  /* 0x8013CFE4: 0xA059000C */
    v0 = cpu->read_word(a0);  /* 0x8013CFE8: 0x8C820000 */
    /* nop */  /* 0x8013CFEC: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013CFF0: 0x00A21021 */
    cpu->write_byte(v0 + 13, (uint8_t)s6);  /* 0x8013CFF4: 0xA056000D */
    v0 = cpu->read_word(a0);  /* 0x8013CFF8: 0x8C820000 */
    /* nop */  /* 0x8013CFFC: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D000: 0x00A21021 */
    cpu->write_byte(v0 + 20, (uint8_t)t1);  /* 0x8013D004: 0xA0490014 */
    v0 = cpu->read_word(a0);  /* 0x8013D008: 0x8C820000 */
    /* nop */  /* 0x8013D00C: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D010: 0x00A21021 */
    cpu->write_byte(v0 + 21, (uint8_t)s6);  /* 0x8013D014: 0xA0560015 */
    v0 = cpu->read_word(a0);  /* 0x8013D018: 0x8C820000 */
    /* nop */  /* 0x8013D01C: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D020: 0x00A21021 */
    t6 = t9;  /* 0x8013D024: 0x03207021 */
    cpu->write_byte(v0 + 28, (uint8_t)t6);  /* 0x8013D028: 0xA04E001C */
    v0 = cpu->read_word(a0);  /* 0x8013D02C: 0x8C820000 */
    /* nop */  /* 0x8013D030: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D034: 0x00A21021 */
    cpu->write_byte(v0 + 29, (uint8_t)a2);  /* 0x8013D038: 0xA046001D */
    v0 = cpu->read_word(a0);  /* 0x8013D03C: 0x8C820000 */
    /* nop */  /* 0x8013D040: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D044: 0x00A21021 */
    /* delay slot (always executes) */
    cpu->write_byte(v0 + 36, (uint8_t)t1);  /* 0x8013D04C: 0xA0490024 */
    psx_check_interrupts_at(cpu, 0x8013D0D0u);
    goto block_8013D0D0;  /* j */

block_8013D050:
    v0 = cpu->read_word(a0);  /* 0x8013D050: 0x8C820000 */
    /* nop */  /* 0x8013D054: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D058: 0x00A21021 */
    cpu->write_half(v0 + 14, (uint16_t)v1);  /* 0x8013D05C: 0xA443000E */
    v0 = cpu->read_word(a0);  /* 0x8013D060: 0x8C820000 */
    /* nop */  /* 0x8013D064: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D068: 0x00A21021 */
    cpu->write_byte(v0 + 12, (uint8_t)t0);  /* 0x8013D06C: 0xA048000C */
    v0 = cpu->read_word(a0);  /* 0x8013D070: 0x8C820000 */
    /* nop */  /* 0x8013D074: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D078: 0x00A21021 */
    cpu->write_byte(v0 + 13, (uint8_t)s6);  /* 0x8013D07C: 0xA056000D */
    v0 = cpu->read_word(a0);  /* 0x8013D080: 0x8C820000 */
    /* nop */  /* 0x8013D084: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D088: 0x00A21021 */
    cpu->write_byte(v0 + 20, (uint8_t)a3);  /* 0x8013D08C: 0xA0470014 */
    v0 = cpu->read_word(a0);  /* 0x8013D090: 0x8C820000 */
    /* nop */  /* 0x8013D094: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D098: 0x00A21021 */
    cpu->write_byte(v0 + 21, (uint8_t)s6);  /* 0x8013D09C: 0xA0560015 */
    v0 = cpu->read_word(a0);  /* 0x8013D0A0: 0x8C820000 */
    /* nop */  /* 0x8013D0A4: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D0A8: 0x00A21021 */
    cpu->write_byte(v0 + 28, (uint8_t)t0);  /* 0x8013D0AC: 0xA048001C */
    v0 = cpu->read_word(a0);  /* 0x8013D0B0: 0x8C820000 */
    /* nop */  /* 0x8013D0B4: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D0B8: 0x00A21021 */
    cpu->write_byte(v0 + 29, (uint8_t)a2);  /* 0x8013D0BC: 0xA046001D */
    v0 = cpu->read_word(a0);  /* 0x8013D0C0: 0x8C820000 */
    /* nop */  /* 0x8013D0C4: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D0C8: 0x00A21021 */
    cpu->write_byte(v0 + 36, (uint8_t)a3);  /* 0x8013D0CC: 0xA0470024 */
    /* fall through to block_8013D0D0 */

block_8013D0D0:
    v0 = cpu->read_word(a0);  /* 0x8013D0D0: 0x8C820000 */
    /* nop */  /* 0x8013D0D4: 0x00000000 */
    v0 = a1 + v0;  /* 0x8013D0D8: 0x00A21021 */
    cpu->write_byte(v0 + 37, (uint8_t)a2);  /* 0x8013D0DC: 0xA0460025 */
    t5++;  /* 0x8013D0E0: 0x25AD0001 */
    v0 = (int32_t)t5 < 2;  /* 0x8013D0E4: 0x29A20002 */
    int cond_8013D0E8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8013D0EC: 0x24840004 */
    if (cond_8013D0E8) {
        psx_check_interrupts_at(cpu, 0x8013CF10u);
        goto block_8013CF10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D0F0u);
    }

block_8013D0F0:
    /* delay slot (always executes) */
    s2 = s5 << 3;  /* 0x8013D0F4: 0x001590C0 */
    r3_jal(0x8018E4D0u, 0x8013D0F8u);  /* jal rand_lcg */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013D0F8u);
    goto block_8013D0F8;

block_8013D0F8:
    s1 = (int32_t)(int16_t)cpu->read_half(s3 + 36);  /* 0x8013D0F8: 0x86710024 */
    v0 &= 0x03FF;  /* 0x8013D0FC: 0x304203FF */
    s1 -= 512;  /* 0x8013D100: 0x2631FE00 */
    /* delay slot (always executes) */
    s1 += v0;  /* 0x8013D108: 0x02228821 */
    r3_jal(0x8018E4D0u, 0x8013D10Cu);  /* jal rand_lcg */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013D10Cu);
    goto block_8013D10C;

block_8013D10C:
    s0 = (int32_t)(int16_t)cpu->read_half(s3 + 34);  /* 0x8013D10C: 0x86700022 */
    v0 &= 0x03FF;  /* 0x8013D110: 0x304203FF */
    s0 -= 512;  /* 0x8013D114: 0x2610FE00 */
    /* delay slot (always executes) */
    s0 += v0;  /* 0x8013D11C: 0x02028021 */
    r3_jal(0x8018E4D0u, 0x8013D120u);  /* jal rand_lcg */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013D120u);
    goto block_8013D120;

block_8013D120:
    a0 = s0;  /* 0x8013D120: 0x02002021 */
    v1 = cpu->read_word(s3 + 8);  /* 0x8013D124: 0x8E630008 */
    /* nop */  /* 0x8013D128: 0x00000000 */
    v1 = s2 + v1;  /* 0x8013D12C: 0x02431821 */
    s7 = r3_lwl(cpu, s3 + 23, s7);  /* 0x8013D130: 0x8A770017 */
    s7 = r3_lwr(cpu, s3 + 20, s7);  /* 0x8013D134: 0x9A770014 */
    t8 = r3_lwl(cpu, s3 + 27, t8);  /* 0x8013D138: 0x8A78001B */
    t8 = r3_lwr(cpu, s3 + 24, t8);  /* 0x8013D13C: 0x9A780018 */
    r3_swl(cpu, v1 + 3, s7);  /* 0x8013D140: 0xA8770003 */
    r3_swr(cpu, v1, s7);  /* 0x8013D144: 0xB8770000 */
    r3_swl(cpu, v1 + 7, t8);  /* 0x8013D148: 0xA8780007 */
    r3_swr(cpu, v1 + 4, t8);  /* 0x8013D14C: 0xB8780004 */
    /* delay slot (always executes) */
    s4 = v0 & 0x0BFF;  /* 0x8013D154: 0x30540BFF */
    r3_jal(0x8018D298u, 0x8013D158u);  /* jal lut_cos_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013D158u);
    goto block_8013D158;

block_8013D158:
    a0 = s1;  /* 0x8013D158: 0x02202021 */
    v1 = cpu->read_word(s3 + 12);  /* 0x8013D15C: 0x8E63000C */
    v0 = (int32_t)v0 >> 8;  /* 0x8013D160: 0x00021203 */
    v1 = s2 + v1;  /* 0x8013D164: 0x02431821 */
    /* delay slot (always executes) */
    cpu->write_half(v1, (uint16_t)v0);  /* 0x8013D16C: 0xA4620000 */
    r3_jal(0x8018D29Cu, 0x8013D170u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013D170u);
    goto block_8013D170;

block_8013D170:
    a0 = s0;  /* 0x8013D170: 0x02002021 */
    v1 = cpu->read_word(s3 + 12);  /* 0x8013D174: 0x8E63000C */
    v0 = (int32_t)v0 >> 8;  /* 0x8013D178: 0x00021203 */
    v1 = s2 + v1;  /* 0x8013D17C: 0x02431821 */
    /* delay slot (always executes) */
    cpu->write_half(v1 + 2, (uint16_t)v0);  /* 0x8013D184: 0xA4620002 */
    r3_jal(0x8018D29Cu, 0x8013D188u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013D188u);
    goto block_8013D188;

block_8013D188:
    v1 = cpu->read_word(s3 + 12);  /* 0x8013D188: 0x8E63000C */
    v0 = (int32_t)v0 >> 8;  /* 0x8013D18C: 0x00021203 */
    v1 = s2 + v1;  /* 0x8013D190: 0x02431821 */
    cpu->write_half(v1 + 4, (uint16_t)v0);  /* 0x8013D194: 0xA4620004 */
    v0 = cpu->read_word(s3 + 12);  /* 0x8013D198: 0x8E62000C */
    /* nop */  /* 0x8013D19C: 0x00000000 */
    a0 = s2 + v0;  /* 0x8013D1A0: 0x02422021 */
    v1 = (int32_t)(int16_t)cpu->read_half(a0);  /* 0x8013D1A4: 0x84830000 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 38);  /* 0x8013D1A8: 0x86620026 */
    /* nop */  /* 0x8013D1AC: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));  /* 0x8013D1B0: 0x00620018 */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013D1B4: 0x00001012 */
    int cond_8013D1B8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D1BC: 0x00000000 */
    if (cond_8013D1B8) {
        psx_check_interrupts_at(cpu, 0x8013D1C4u);
        goto block_8013D1C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D1C0u);
    }

block_8013D1C0:
    v0 += 15;  /* 0x8013D1C0: 0x2442000F */
    /* fall through to block_8013D1C4 */

block_8013D1C4:
    v0 = (int32_t)v0 >> 4;  /* 0x8013D1C4: 0x00021103 */
    cpu->write_half(a0, (uint16_t)v0);  /* 0x8013D1C8: 0xA4820000 */
    v0 = cpu->read_word(s3 + 12);  /* 0x8013D1CC: 0x8E62000C */
    /* nop */  /* 0x8013D1D0: 0x00000000 */
    a0 = s2 + v0;  /* 0x8013D1D4: 0x02422021 */
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + 2);  /* 0x8013D1D8: 0x84830002 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 38);  /* 0x8013D1DC: 0x86620026 */
    /* nop */  /* 0x8013D1E0: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));  /* 0x8013D1E4: 0x00620018 */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013D1E8: 0x00001012 */
    int cond_8013D1EC = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D1F0: 0x00000000 */
    if (cond_8013D1EC) {
        psx_check_interrupts_at(cpu, 0x8013D1F8u);
        goto block_8013D1F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D1F4u);
    }

block_8013D1F4:
    v0 += 15;  /* 0x8013D1F4: 0x2442000F */
    /* fall through to block_8013D1F8 */

block_8013D1F8:
    v0 = (int32_t)v0 >> 4;  /* 0x8013D1F8: 0x00021103 */
    cpu->write_half(a0 + 2, (uint16_t)v0);  /* 0x8013D1FC: 0xA4820002 */
    v0 = cpu->read_word(s3 + 12);  /* 0x8013D200: 0x8E62000C */
    /* nop */  /* 0x8013D204: 0x00000000 */
    a0 = s2 + v0;  /* 0x8013D208: 0x02422021 */
    v1 = (int32_t)(int16_t)cpu->read_half(a0 + 4);  /* 0x8013D20C: 0x84830004 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 38);  /* 0x8013D210: 0x86620026 */
    /* nop */  /* 0x8013D214: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));  /* 0x8013D218: 0x00620018 */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013D21C: 0x00001012 */
    int cond_8013D220 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D224: 0x00000000 */
    if (cond_8013D220) {
        psx_check_interrupts_at(cpu, 0x8013D22Cu);
        goto block_8013D22C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D228u);
    }

block_8013D228:
    v0 += 15;  /* 0x8013D228: 0x2442000F */
    /* fall through to block_8013D22C */

block_8013D22C:
    v0 = (int32_t)v0 >> 4;  /* 0x8013D22C: 0x00021103 */
    cpu->write_half(a0 + 4, (uint16_t)v0);  /* 0x8013D230: 0xA4820004 */
    v1 = cpu->read_word(s3 + 12);  /* 0x8013D234: 0x8E63000C */
    /* nop */  /* 0x8013D238: 0x00000000 */
    v1 = s2 + v1;  /* 0x8013D23C: 0x02431821 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1);  /* 0x8013D240: 0x84620000 */
    /* nop */  /* 0x8013D244: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)s4; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013D248: 0x00540018 */
    t6 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013D24C: 0x00007012 */
    v0 = (int32_t)t6 >> 12;  /* 0x8013D250: 0x000E1303 */
    cpu->write_half(v1, (uint16_t)v0);  /* 0x8013D254: 0xA4620000 */
    v1 = cpu->read_word(s3 + 12);  /* 0x8013D258: 0x8E63000C */
    /* nop */  /* 0x8013D25C: 0x00000000 */
    v1 = s2 + v1;  /* 0x8013D260: 0x02431821 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 2);  /* 0x8013D264: 0x84620002 */
    /* nop */  /* 0x8013D268: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)s4; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013D26C: 0x00540018 */
    t6 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013D270: 0x00007012 */
    v0 = (int32_t)t6 >> 12;  /* 0x8013D274: 0x000E1303 */
    cpu->write_half(v1 + 2, (uint16_t)v0);  /* 0x8013D278: 0xA4620002 */
    v1 = cpu->read_word(s3 + 12);  /* 0x8013D27C: 0x8E63000C */
    /* nop */  /* 0x8013D280: 0x00000000 */
    v1 = s2 + v1;  /* 0x8013D284: 0x02431821 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 4);  /* 0x8013D288: 0x84620004 */
    /* nop */  /* 0x8013D28C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)s4; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013D290: 0x00540018 */
    t6 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8013D294: 0x00007012 */
    v0 = (int32_t)t6 >> 12;  /* 0x8013D298: 0x000E1303 */
    cpu->write_half(v1 + 4, (uint16_t)v0);  /* 0x8013D29C: 0xA4620004 */
    v1 = cpu->read_word(s3 + 16);  /* 0x8013D2A0: 0x8E630010 */
    v0 = s5 << 1;  /* 0x8013D2A4: 0x00151040 */
    v0 += v1;  /* 0x8013D2A8: 0x00431021 */
    v1 = 60;  /* 0x8013D2AC: 0x2403003C */
    cpu->write_half(v0, (uint16_t)v1);  /* 0x8013D2B0: 0xA4430000 */
    v0 = cpu->read_half(s3 + 32);  /* 0x8013D2B4: 0x96620020 */
    /* nop */  /* 0x8013D2B8: 0x00000000 */
    v0++;  /* 0x8013D2BC: 0x24420001 */
    cpu->write_half(s3 + 32, (uint16_t)v0);  /* 0x8013D2C0: 0xA6620020 */
    /* fall through to block_8013D2C4 */

block_8013D2C4:
    s5++;  /* 0x8013D2C4: 0x26B50001 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 32);  /* 0x8013D2C8: 0x86620020 */
    a0 = cpu->read_half(s3 + 32);  /* 0x8013D2CC: 0x96640020 */
    v0 += 2;  /* 0x8013D2D0: 0x24420002 */
    v0 = (int32_t)s5 < (int32_t)v0;  /* 0x8013D2D4: 0x02A2102A */
    int cond_8013D2D8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    fp += 40;  /* 0x8013D2DC: 0x27DE0028 */
    if (cond_8013D2D8) {
        psx_check_interrupts_at(cpu, 0x8013CEE0u);
        goto block_8013CEE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D2E0u);
    }

block_8013D2E0:
    t3 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013D2E0: 0x3C0B1F80 */
    t3 |= 0x03F4;  /* 0x8013D2E4: 0x356B03F4 */
    a0 = 0x55550000u;  /* 0x55550000 */  /* 0x8013D2E8: 0x3C045555 */
    v1 = cpu->read_half(s3 + 32);  /* 0x8013D2EC: 0x96630020 */
    a0 |= 0x5556;  /* 0x8013D2F0: 0x34845556 */
    v1 <<= 16;  /* 0x8013D2F4: 0x00031C00 */
    v0 = (int32_t)v1 >> 16;  /* 0x8013D2F8: 0x00031403 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8013D2FC: 0x00440018 */
    s2 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013D300: 0x3C121F80 */
    s2 |= 0x03E8;  /* 0x8013D304: 0x365203E8 */
    t4 = cpu->read_word(s3 + 8);  /* 0x8013D308: 0x8E6C0008 */
    s1 = cpu->read_word(s3 + 12);  /* 0x8013D30C: 0x8E71000C */
    t0 = cpu->read_word(s3 + 16);  /* 0x8013D310: 0x8E680010 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013D314: 0x3C021F80 */
    v0 = cpu->read_word(v0 + 12);  /* 0x8013D318: 0x8C42000C */
    v1 = (int32_t)v1 >> 31;  /* 0x8013D31C: 0x00031FC3 */
    v0 <<= 2;  /* 0x8013D320: 0x00021080 */
    v0 = s3 + v0;  /* 0x8013D324: 0x02621021 */
    t7 = cpu->read_word(v0);  /* 0x8013D328: 0x8C4F0000 */
    t6 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8013D32C: 0x00007010 */
    v1 = t6 - v1;  /* 0x8013D330: 0x01C31823 */
    v1 <<= 16;  /* 0x8013D334: 0x00031C00 */
    s6 = (int32_t)v1 >> 16;  /* 0x8013D338: 0x0003B403 */
    int cond_8013D33C = ((int32_t)s6 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = zero;  /* 0x8013D340: 0x0000A821 */
    if (cond_8013D33C) {
        psx_check_interrupts_at(cpu, 0x8013D650u);
        goto block_8013D650;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D344u);
    }

block_8013D344:
    s4 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8013D344: 0x3C141F80 */
    v1 = t4 + 8;  /* 0x8013D348: 0x25830008 */
    /* fall through to block_8013D34C */

block_8013D34C:
    v0 = t4 + 16;  /* 0x8013D34C: 0x25820010 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((t4)));  /* 0x8013D350: 0xC9800000 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((t4 + 4))));  /* 0x8013D354: 0xC9810004 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (cpu->read_word((v1)));  /* 0x8013D358: 0xC8620000 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (cpu->read_word((v1 + 4))));  /* 0x8013D35C: 0xC8630004 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (cpu->read_word((v0)));  /* 0x8013D360: 0xC8440000 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (cpu->read_word((v0 + 4))));  /* 0x8013D364: 0xC8450004 */
    /* nop */  /* 0x8013D368: 0x00000000 */
    /* nop */  /* 0x8013D36C: 0x00000000 */
    gte_execute(cpu, 0x0280030);  /* gte cmd 0x30 */  /* 0x8013D370: 0x4A280030 */
    v1 = s2 + 4;  /* 0x8013D374: 0x26430004 */
    v0 = s2 + 8;  /* 0x8013D378: 0x26420008 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s2), (cpu->gte_data[12]));  /* 0x8013D37C: 0xEA4C0000 */
    gte_precision_store_word((s2), 12);
    
    psx_gte_stall(cpu);
    cpu->write_word((v1), (cpu->gte_data[13]));  /* 0x8013D380: 0xE86D0000 */
    gte_precision_store_word((v1), 13);
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[14]));  /* 0x8013D384: 0xE84E0000 */
    gte_precision_store_word((v0), 14);
    v1 = t3 + 4;  /* 0x8013D388: 0x25630004 */
    v0 = t3 + 8;  /* 0x8013D38C: 0x25620008 */
    
    psx_gte_stall(cpu);
    cpu->write_word((t3), (gte_read_data(cpu, 17)));  /* 0x8013D390: 0xE9710000 */
    gte_precision_store_word((t3), 17);
    
    psx_gte_stall(cpu);
    cpu->write_word((v1), (gte_read_data(cpu, 18)));  /* 0x8013D394: 0xE8720000 */
    gte_precision_store_word((v1), 18);
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (gte_read_data(cpu, 19)));  /* 0x8013D398: 0xE8530000 */
    gte_precision_store_word((v0), 19);
    t5 = zero;  /* 0x8013D39C: 0x00006821 */
    t1 = s2;  /* 0x8013D3A0: 0x02404821 */
    s0 = t3;  /* 0x8013D3A4: 0x01608021 */
    a3 = t7 + 34;  /* 0x8013D3A8: 0x25E70022 */
    s7 = s1 + 2;  /* 0x8013D3AC: 0x26370002 */
    t2 = t4 + 2;  /* 0x8013D3B0: 0x258A0002 */
    cpu->write_word(sp + 16, s7);  /* 0x8013D3B4: 0xAFB70010 */
    v0 = cpu->read_word(t3);  /* 0x8013D3B8: 0x8D620000 */
    v1 = cpu->read_word(t3 + 8);  /* 0x8013D3BC: 0x8D630008 */
    v0 = (int32_t)v0 >> 2;  /* 0x8013D3C0: 0x00021083 */
    cpu->write_word(t3, v0);  /* 0x8013D3C4: 0xAD620000 */
    v0 = cpu->read_word(t3 + 4);  /* 0x8013D3C8: 0x8D620004 */
    v1 = (int32_t)v1 >> 2;  /* 0x8013D3CC: 0x00031883 */
    cpu->write_word(t3 + 8, v1);  /* 0x8013D3D0: 0xAD630008 */
    v0 = (int32_t)v0 >> 2;  /* 0x8013D3D4: 0x00021083 */
    cpu->write_word(t3 + 4, v0);  /* 0x8013D3D8: 0xAD620004 */
    /* fall through to block_8013D3DC */

block_8013D3DC:
    v0 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D3DC: 0x85020000 */
    v1 = cpu->read_half(t0);  /* 0x8013D3E0: 0x95030000 */
    int cond_8013D3E4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D3E8: 0x00000000 */
    if (cond_8013D3E4) {
        psx_check_interrupts_at(cpu, 0x8013D608u);
        goto block_8013D608;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D3ECu);
    }

block_8013D3EC:
    v0 = cpu->read_half(s4 + 130);  /* 0x8013D3EC: 0x96820082 */
    /* nop */  /* 0x8013D3F0: 0x00000000 */
    int cond_8013D3F4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;  /* 0x8013D3F8: 0x2462FFFF */
    if (cond_8013D3F4) {
        psx_check_interrupts_at(cpu, 0x8013D440u);
        goto block_8013D440;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D3FCu);
    }

block_8013D3FC:
    cpu->write_half(t0, (uint16_t)v0);  /* 0x8013D3FC: 0xA5020000 */
    v0 = cpu->read_half(t4);  /* 0x8013D400: 0x95820000 */
    v1 = cpu->read_half(s1);  /* 0x8013D404: 0x96230000 */
    /* nop */  /* 0x8013D408: 0x00000000 */
    v0 += v1;  /* 0x8013D40C: 0x00431021 */
    cpu->write_half(t4, (uint16_t)v0);  /* 0x8013D410: 0xA5820000 */
    t8 = cpu->read_word(sp + 16);  /* 0x8013D414: 0x8FB80010 */
    v0 = cpu->read_half(t2 + 2);  /* 0x8013D418: 0x95420002 */
    v1 = cpu->read_half(t8 + 2);  /* 0x8013D41C: 0x97030002 */
    /* nop */  /* 0x8013D420: 0x00000000 */
    v0 += v1;  /* 0x8013D424: 0x00431021 */
    cpu->write_half(t2 + 2, (uint16_t)v0);  /* 0x8013D428: 0xA5420002 */
    v0 = cpu->read_half(t2);  /* 0x8013D42C: 0x95420000 */
    v1 = cpu->read_half(t8);  /* 0x8013D430: 0x97030000 */
    /* nop */  /* 0x8013D434: 0x00000000 */
    v0 += v1;  /* 0x8013D438: 0x00431021 */
    cpu->write_half(t2, (uint16_t)v0);  /* 0x8013D43C: 0xA5420000 */
    /* fall through to block_8013D440 */

block_8013D440:
    v0 = cpu->read_word(s0);  /* 0x8013D440: 0x8E020000 */
    /* nop */  /* 0x8013D444: 0x00000000 */
    v0 -= 10;  /* 0x8013D448: 0x2442FFF6 */
    v0 = v0 < (uint32_t)2038;  /* 0x8013D44C: 0x2C4207F6 */
    int cond_8013D450 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = t5 << 2;  /* 0x8013D454: 0x000D2080 */
    if (cond_8013D450) {
        psx_check_interrupts_at(cpu, 0x8013D608u);
        goto block_8013D608;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D458u);
    }

block_8013D458:
    v1 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D458: 0x85030000 */
    /* nop */  /* 0x8013D45C: 0x00000000 */
    int cond_8013D460 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D464: 0x00000000 */
    if (cond_8013D460) {
        psx_check_interrupts_at(cpu, 0x8013D46Cu);
        goto block_8013D46C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D468u);
    }

block_8013D468:
    v1 += 7;  /* 0x8013D468: 0x24630007 */
    /* fall through to block_8013D46C */

block_8013D46C:
    v0 = cpu->read_half(t1);  /* 0x8013D46C: 0x95220000 */
    v1 = (int32_t)v1 >> 3;  /* 0x8013D470: 0x000318C3 */
    v0 -= v1;  /* 0x8013D474: 0x00431023 */
    cpu->write_half(a3 + -26, (uint16_t)v0);  /* 0x8013D478: 0xA4E2FFE6 */
    v1 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D47C: 0x85030000 */
    /* nop */  /* 0x8013D480: 0x00000000 */
    int cond_8013D484 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D488: 0x00000000 */
    if (cond_8013D484) {
        psx_check_interrupts_at(cpu, 0x8013D490u);
        goto block_8013D490;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D48Cu);
    }

block_8013D48C:
    v1 += 7;  /* 0x8013D48C: 0x24630007 */
    /* fall through to block_8013D490 */

block_8013D490:
    v0 = cpu->read_half(t1 + 2);  /* 0x8013D490: 0x95220002 */
    v1 = (int32_t)v1 >> 3;  /* 0x8013D494: 0x000318C3 */
    v0 -= v1;  /* 0x8013D498: 0x00431023 */
    cpu->write_half(a3 + -24, (uint16_t)v0);  /* 0x8013D49C: 0xA4E2FFE8 */
    v0 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D4A0: 0x85020000 */
    /* nop */  /* 0x8013D4A4: 0x00000000 */
    int cond_8013D4A8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;  /* 0x8013D4AC: 0x00403021 */
    if (cond_8013D4A8) {
        psx_check_interrupts_at(cpu, 0x8013D4B4u);
        goto block_8013D4B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D4B0u);
    }

block_8013D4B0:
    a2 = v0 + 7;  /* 0x8013D4B0: 0x24460007 */
    /* fall through to block_8013D4B4 */

block_8013D4B4:
    a1 = v0;  /* 0x8013D4B4: 0x00402821 */
    v1 = cpu->read_half(t1);  /* 0x8013D4B8: 0x95230000 */
    v0 = (int32_t)a2 >> 3;  /* 0x8013D4BC: 0x000610C3 */
    int cond_8013D4C0 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= v0;  /* 0x8013D4C4: 0x00621823 */
    if (cond_8013D4C0) {
        psx_check_interrupts_at(cpu, 0x8013D4CCu);
        goto block_8013D4CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D4C8u);
    }

block_8013D4C8:
    a1 += 3;  /* 0x8013D4C8: 0x24A50003 */
    /* fall through to block_8013D4CC */

block_8013D4CC:
    v0 = (int32_t)a1 >> 2;  /* 0x8013D4CC: 0x00051083 */
    v0 = v1 + v0;  /* 0x8013D4D0: 0x00621021 */
    cpu->write_half(a3 + -18, (uint16_t)v0);  /* 0x8013D4D4: 0xA4E2FFEE */
    v1 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D4D8: 0x85030000 */
    /* nop */  /* 0x8013D4DC: 0x00000000 */
    int cond_8013D4E0 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D4E4: 0x00000000 */
    if (cond_8013D4E0) {
        psx_check_interrupts_at(cpu, 0x8013D4ECu);
        goto block_8013D4EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D4E8u);
    }

block_8013D4E8:
    v1 += 7;  /* 0x8013D4E8: 0x24630007 */
    /* fall through to block_8013D4EC */

block_8013D4EC:
    v0 = cpu->read_half(t1 + 2);  /* 0x8013D4EC: 0x95220002 */
    v1 = (int32_t)v1 >> 3;  /* 0x8013D4F0: 0x000318C3 */
    v0 -= v1;  /* 0x8013D4F4: 0x00431023 */
    cpu->write_half(a3 + -16, (uint16_t)v0);  /* 0x8013D4F8: 0xA4E2FFF0 */
    v1 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D4FC: 0x85030000 */
    /* nop */  /* 0x8013D500: 0x00000000 */
    int cond_8013D504 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D508: 0x00000000 */
    if (cond_8013D504) {
        psx_check_interrupts_at(cpu, 0x8013D510u);
        goto block_8013D510;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D50Cu);
    }

block_8013D50C:
    v1 += 7;  /* 0x8013D50C: 0x24630007 */
    /* fall through to block_8013D510 */

block_8013D510:
    v0 = cpu->read_half(t1);  /* 0x8013D510: 0x95220000 */
    v1 = (int32_t)v1 >> 3;  /* 0x8013D514: 0x000318C3 */
    v0 -= v1;  /* 0x8013D518: 0x00431023 */
    cpu->write_half(a3 + -10, (uint16_t)v0);  /* 0x8013D51C: 0xA4E2FFF6 */
    v0 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D520: 0x85020000 */
    /* nop */  /* 0x8013D524: 0x00000000 */
    int cond_8013D528 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;  /* 0x8013D52C: 0x00403021 */
    if (cond_8013D528) {
        psx_check_interrupts_at(cpu, 0x8013D534u);
        goto block_8013D534;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D530u);
    }

block_8013D530:
    a2 = v0 + 7;  /* 0x8013D530: 0x24460007 */
    /* fall through to block_8013D534 */

block_8013D534:
    a1 = v0;  /* 0x8013D534: 0x00402821 */
    v1 = cpu->read_half(t1 + 2);  /* 0x8013D538: 0x95230002 */
    v0 = (int32_t)a2 >> 3;  /* 0x8013D53C: 0x000610C3 */
    int cond_8013D540 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= v0;  /* 0x8013D544: 0x00621823 */
    if (cond_8013D540) {
        psx_check_interrupts_at(cpu, 0x8013D54Cu);
        goto block_8013D54C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D548u);
    }

block_8013D548:
    a1 += 3;  /* 0x8013D548: 0x24A50003 */
    /* fall through to block_8013D54C */

block_8013D54C:
    v0 = (int32_t)a1 >> 2;  /* 0x8013D54C: 0x00051083 */
    v0 = v1 + v0;  /* 0x8013D550: 0x00621021 */
    cpu->write_half(a3 + -8, (uint16_t)v0);  /* 0x8013D554: 0xA4E2FFF8 */
    v0 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D558: 0x85020000 */
    /* nop */  /* 0x8013D55C: 0x00000000 */
    int cond_8013D560 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;  /* 0x8013D564: 0x00403021 */
    if (cond_8013D560) {
        psx_check_interrupts_at(cpu, 0x8013D56Cu);
        goto block_8013D56C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D568u);
    }

block_8013D568:
    a2 = v0 + 7;  /* 0x8013D568: 0x24460007 */
    /* fall through to block_8013D56C */

block_8013D56C:
    a1 = v0;  /* 0x8013D56C: 0x00402821 */
    v1 = cpu->read_half(t1);  /* 0x8013D570: 0x95230000 */
    v0 = (int32_t)a2 >> 3;  /* 0x8013D574: 0x000610C3 */
    int cond_8013D578 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= v0;  /* 0x8013D57C: 0x00621823 */
    if (cond_8013D578) {
        psx_check_interrupts_at(cpu, 0x8013D584u);
        goto block_8013D584;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D580u);
    }

block_8013D580:
    a1 += 3;  /* 0x8013D580: 0x24A50003 */
    /* fall through to block_8013D584 */

block_8013D584:
    v0 = (int32_t)a1 >> 2;  /* 0x8013D584: 0x00051083 */
    v0 = v1 + v0;  /* 0x8013D588: 0x00621021 */
    cpu->write_half(a3 + -2, (uint16_t)v0);  /* 0x8013D58C: 0xA4E2FFFE */
    v0 = (int32_t)(int16_t)cpu->read_half(t0);  /* 0x8013D590: 0x85020000 */
    v1 = a0 + s2;  /* 0x8013D594: 0x00921821 */
    int cond_8013D598 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;  /* 0x8013D59C: 0x00403021 */
    if (cond_8013D598) {
        psx_check_interrupts_at(cpu, 0x8013D5A4u);
        goto block_8013D5A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D5A0u);
    }

block_8013D5A0:
    a2 = v0 + 7;  /* 0x8013D5A0: 0x24460007 */
    /* fall through to block_8013D5A4 */

block_8013D5A4:
    a1 = v0;  /* 0x8013D5A4: 0x00402821 */
    v1 = cpu->read_half(v1 + 2);  /* 0x8013D5A8: 0x94630002 */
    v0 = (int32_t)a2 >> 3;  /* 0x8013D5AC: 0x000610C3 */
    int cond_8013D5B0 = ((int32_t)a1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= v0;  /* 0x8013D5B4: 0x00621823 */
    if (cond_8013D5B0) {
        psx_check_interrupts_at(cpu, 0x8013D5BCu);
        goto block_8013D5BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D5B8u);
    }

block_8013D5B8:
    a1 += 3;  /* 0x8013D5B8: 0x24A50003 */
    /* fall through to block_8013D5BC */

block_8013D5BC:
    v0 = (int32_t)a1 >> 2;  /* 0x8013D5BC: 0x00051083 */
    v0 = v1 + v0;  /* 0x8013D5C0: 0x00621021 */
    a0 += t3;  /* 0x8013D5C4: 0x008B2021 */
    cpu->write_half(a3, (uint16_t)v0);  /* 0x8013D5C8: 0xA4E20000 */
    v0 = cpu->read_word(a0);  /* 0x8013D5CC: 0x8C820000 */
    v1 = cpu->read_word(s4 + 8);  /* 0x8013D5D0: 0x8E830008 */
    v0 <<= 2;  /* 0x8013D5D4: 0x00021080 */
    v0 += v1;  /* 0x8013D5D8: 0x00431021 */
    v0 = cpu->read_word(v0);  /* 0x8013D5DC: 0x8C420000 */
    v1 = 0x09000000u;  /* 0x09000000 */  /* 0x8013D5E0: 0x3C030900 */
    v0 |= v1;  /* 0x8013D5E4: 0x00431025 */
    cpu->write_word(t7, v0);  /* 0x8013D5E8: 0xADE20000 */
    v1 = cpu->read_word(a0);  /* 0x8013D5EC: 0x8C830000 */
    v0 = cpu->read_word(s4 + 8);  /* 0x8013D5F0: 0x8E820008 */
    v1 <<= 2;  /* 0x8013D5F4: 0x00031880 */
    v1 += v0;  /* 0x8013D5F8: 0x00621821 */
    v0 = t7 << 8;  /* 0x8013D5FC: 0x000F1200 */
    v0 = (uint32_t)v0 >> 8;  /* 0x8013D600: 0x00021202 */
    cpu->write_word(v1, v0);  /* 0x8013D604: 0xAC620000 */
    /* fall through to block_8013D608 */

block_8013D608:
    t2 += 8;  /* 0x8013D608: 0x254A0008 */
    t4 += 8;  /* 0x8013D60C: 0x258C0008 */
    s1 += 8;  /* 0x8013D610: 0x26310008 */
    t0 += 2;  /* 0x8013D614: 0x25080002 */
    a3 += 40;  /* 0x8013D618: 0x24E70028 */
    t7 += 40;  /* 0x8013D61C: 0x25EF0028 */
    t1 += 4;  /* 0x8013D620: 0x25290004 */
    s0 += 4;  /* 0x8013D624: 0x26100004 */
    t5++;  /* 0x8013D628: 0x25AD0001 */
    t9 = cpu->read_word(sp + 16);  /* 0x8013D62C: 0x8FB90010 */
    v0 = (int32_t)t5 < 3;  /* 0x8013D630: 0x29A20003 */
    t9 += 8;  /* 0x8013D634: 0x27390008 */
    int cond_8013D638 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, t9);  /* 0x8013D63C: 0xAFB90010 */
    if (cond_8013D638) {
        psx_check_interrupts_at(cpu, 0x8013D3DCu);
        goto block_8013D3DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D640u);
    }

block_8013D640:
    s5++;  /* 0x8013D640: 0x26B50001 */
    v0 = (int32_t)s5 < (int32_t)s6;  /* 0x8013D644: 0x02B6102A */
    int cond_8013D648 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = t4 + 8;  /* 0x8013D64C: 0x25830008 */
    if (cond_8013D648) {
        psx_check_interrupts_at(cpu, 0x8013D34Cu);
        goto block_8013D34C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D650u);
    }

block_8013D650:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 28);  /* 0x8013D650: 0x8662001C */
    /* nop */  /* 0x8013D654: 0x00000000 */
    v0 = (int32_t)v0 < 56;  /* 0x8013D658: 0x28420038 */
    int cond_8013D65C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D660: 0x00000000 */
    if (cond_8013D65C) {
        psx_check_interrupts_at(cpu, 0x8013D680u);
        goto block_8013D680;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013D664u);
    }

block_8013D664:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013D668: 0x00000000 */
    r3_jal(0x801A482Cu, 0x8013D66Cu);  /* jal task_slot_step */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x8013D66Cu);
    goto block_8013D66C;

block_8013D66C:
    v1 = 0x801F0000u;  /* 0x801F0000 */  /* 0x8013D66C: 0x3C03801F */
    v0 = cpu->read_word(v1 + 1844);  /* 0x8013D670: 0x8C620734 */
    /* nop */  /* 0x8013D674: 0x00000000 */
    v0 -= 3;  /* 0x8013D678: 0x2442FFFD */
    cpu->write_word(v1 + 1844, v0);  /* 0x8013D67C: 0xAC620734 */
    /* fall through to block_8013D680 */

block_8013D680:
    ra = cpu->read_word(sp + 60);  /* 0x8013D680: 0x8FBF003C */
    fp = cpu->read_word(sp + 56);  /* 0x8013D684: 0x8FBE0038 */
    s7 = cpu->read_word(sp + 52);  /* 0x8013D688: 0x8FB70034 */
    s6 = cpu->read_word(sp + 48);  /* 0x8013D68C: 0x8FB60030 */
    s5 = cpu->read_word(sp + 44);  /* 0x8013D690: 0x8FB5002C */
    s4 = cpu->read_word(sp + 40);  /* 0x8013D694: 0x8FB40028 */
    s3 = cpu->read_word(sp + 36);  /* 0x8013D698: 0x8FB30024 */
    s2 = cpu->read_word(sp + 32);  /* 0x8013D69C: 0x8FB20020 */
    s1 = cpu->read_word(sp + 28);  /* 0x8013D6A0: 0x8FB1001C */
    s0 = cpu->read_word(sp + 24);  /* 0x8013D6A4: 0x8FB00018 */
    uint32_t jt_8013D6A8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 64;  /* 0x8013D6AC: 0x27BD0040 */
    psx_check_interrupts_at(cpu, jt_8013D6A8);
    cpu->pc = jt_8013D6A8; return;  /* CPS: jr $ra */
}


/* ======== impact_fx_update_cps (guest 0x80138CEC) ========
 * Per-tick update for an impact FX record — clean port of the
 * cps_80138cec cluster (49 clean_cps.map rows, br2eu_func_80138CEC in
 * SLES_017.22_full_07.c). Covers the prologue (64-byte frame, s0-s5+ra),
 * the type-field read, the computed-goto dispatch heads (jr $v0 tails stay
 * cpu->pc dispatches into the sibling CPS pieces / generated alias body,
 * which stay authoritative for the type continuations), the inline type
 * cases, task_slot_step + lut_sin_q12 jals via r3_jal, and the shared
 * tail pieces. battle.c's impact_fx_update(int rec) is the C-ABI shim:
 * it stages gpr[4]=rec and calls this.
 */
void impact_fx_update_cps(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;

/* Address: 0x80138CEC, Size: 1740 bytes, Blocks: 49 */

block_80138CEC:
    sp -= 64;  /* 0x80138CEC: 0x27BDFFC0 */
    cpu->write_word(sp + 44, s3);  /* 0x80138CF0: 0xAFB3002C */
    s3 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80138CF4: 0x3C131F80 */
    s3 |= 0x03FC;  /* 0x80138CF8: 0x367303FC */
    cpu->write_word(sp + 52, s5);  /* 0x80138CFC: 0xAFB50034 */
    s5 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80138D00: 0x3C151F80 */
    s5 |= 0x03F8;  /* 0x80138D04: 0x36B503F8 */
    cpu->write_word(sp + 36, s1);  /* 0x80138D08: 0xAFB10024 */
    s1 = a0;  /* 0x80138D0C: 0x00808821 */
    cpu->write_word(sp + 56, ra);  /* 0x80138D10: 0xAFBF0038 */
    cpu->write_word(sp + 48, s4);  /* 0x80138D14: 0xAFB40030 */
    cpu->write_word(sp + 40, s2);  /* 0x80138D18: 0xAFB20028 */
    cpu->write_word(sp + 32, s0);  /* 0x80138D1C: 0xAFB00020 */
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 100);  /* 0x80138D20: 0x86230064 */
    /* nop */  /* 0x80138D24: 0x00000000 */
    v0 = v1 < (uint32_t)19;  /* 0x80138D28: 0x2C620013 */
    int cond_80138D2C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s4 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80138D30: 0x3C141F80 */
    if (cond_80138D2C) {
        psx_check_interrupts_at(cpu, 0x80138F24u);
        goto block_80138F24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D34u);
    }

block_80138D34:
    v0 = 0x80120068u;  /* 0x80120000 104 */  /* 0x80120000 */  /* 0x80138D34: 0x3C028012 */  /* 0x80138D38: 0x24420068 */
    v1 <<= 2;  /* 0x80138D3C: 0x00031880 */
    v1 += v0;  /* 0x80138D40: 0x00621821 */
    v0 = cpu->read_word(v1);  /* 0x80138D44: 0x8C620000 */
    /* nop */  /* 0x80138D48: 0x00000000 */
    uint32_t jt_80138D4C = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D50: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_80138D4C);
    cpu->pc = jt_80138D4C; return;  /* CPS: jr v0 */

block_80138D54:
    v0 = cpu->read_word(s1 + 80);  /* 0x80138D54: 0x8E220050 */
    /* nop */  /* 0x80138D58: 0x00000000 */
    int cond_80138D5C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D60: 0x00000000 */
    if (cond_80138D5C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D64u);
    }

block_80138D64:
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D68: 0x00000000 */
    r3_jal(0x801A482Cu, 0x80138D6Cu);  /* jal task_slot_step */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x80138D6Cu);
    goto block_80138D6C;

block_80138D6C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D70: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80139DE8u);
    cpu->pc = 0x80139DE8u; return;  /* CPS j: split -> impact_fx_type_alias (0x80139DE8) */

block_80138D74:
    v0 = cpu->read_half(s4 + 130);  /* 0x80138D74: 0x96820082 */
    v1 = cpu->read_word(s1 + 96);  /* 0x80138D78: 0x8E230060 */
    int cond_80138D7C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D80: 0x00000000 */
    if (cond_80138D7C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D84u);
    }

block_80138D84:
    v0 = cpu->read_half(v1);  /* 0x80138D84: 0x94620000 */
    /* nop */  /* 0x80138D88: 0x00000000 */
    v0--;  /* 0x80138D8C: 0x2442FFFF */
    cpu->write_half(v1, (uint16_t)v0);  /* 0x80138D90: 0xA4620000 */
    v0 <<= 16;  /* 0x80138D94: 0x00021400 */
    int cond_80138D98 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D9C: 0x00000000 */
    if (cond_80138D98) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DA0u);
    }

block_80138DA0:
    v0 = cpu->read_word(s4 + 12);  /* 0x80138DA0: 0x8E82000C */
    /* nop */  /* 0x80138DA4: 0x00000000 */
    v1 = v0 << 2;  /* 0x80138DA8: 0x00021880 */
    v1 += v0;  /* 0x80138DAC: 0x00621821 */
    v1 <<= 3;  /* 0x80138DB0: 0x000318C0 */
    v1 = s1 + v1;  /* 0x80138DB4: 0x02231821 */
    a0 = cpu->read_byte(v1 + 4);  /* 0x80138DB8: 0x90640004 */
    /* nop */  /* 0x80138DBC: 0x00000000 */
    v0 = a0 < (uint32_t)8;  /* 0x80138DC0: 0x2C820008 */
    int cond_80138DC4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;  /* 0x80138DC8: 0x2482FFF8 */
    if (cond_80138DC4) {
        psx_check_interrupts_at(cpu, 0x80138DD4u);
        goto block_80138DD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DCCu);
    }

block_80138DCC:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 4, (uint8_t)zero);  /* 0x80138DD0: 0xA0600004 */
    psx_check_interrupts_at(cpu, 0x80138DD8u);
    goto block_80138DD8;  /* j */

block_80138DD4:
    cpu->write_byte(v1 + 4, (uint8_t)v0);  /* 0x80138DD4: 0xA0620004 */
    /* fall through to block_80138DD8 */

block_80138DD8:
    v0 = cpu->read_word(s4 + 12);  /* 0x80138DD8: 0x8E82000C */
    /* nop */  /* 0x80138DDC: 0x00000000 */
    v1 = v0 << 2;  /* 0x80138DE0: 0x00021880 */
    v1 += v0;  /* 0x80138DE4: 0x00621821 */
    v1 <<= 3;  /* 0x80138DE8: 0x000318C0 */
    v1 = s1 + v1;  /* 0x80138DEC: 0x02231821 */
    a0 = cpu->read_byte(v1 + 5);  /* 0x80138DF0: 0x90640005 */
    /* nop */  /* 0x80138DF4: 0x00000000 */
    v0 = a0 < (uint32_t)8;  /* 0x80138DF8: 0x2C820008 */
    int cond_80138DFC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;  /* 0x80138E00: 0x2482FFF8 */
    if (cond_80138DFC) {
        psx_check_interrupts_at(cpu, 0x80138E0Cu);
        goto block_80138E0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E04u);
    }

block_80138E04:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 5, (uint8_t)zero);  /* 0x80138E08: 0xA0600005 */
    psx_check_interrupts_at(cpu, 0x80138E10u);
    goto block_80138E10;  /* j */

block_80138E0C:
    cpu->write_byte(v1 + 5, (uint8_t)v0);  /* 0x80138E0C: 0xA0620005 */
    /* fall through to block_80138E10 */

block_80138E10:
    v0 = cpu->read_word(s4 + 12);  /* 0x80138E10: 0x8E82000C */
    /* nop */  /* 0x80138E14: 0x00000000 */
    v1 = v0 << 2;  /* 0x80138E18: 0x00021880 */
    v1 += v0;  /* 0x80138E1C: 0x00621821 */
    v1 <<= 3;  /* 0x80138E20: 0x000318C0 */
    v1 = s1 + v1;  /* 0x80138E24: 0x02231821 */
    a0 = cpu->read_byte(v1 + 6);  /* 0x80138E28: 0x90640006 */
    /* nop */  /* 0x80138E2C: 0x00000000 */
    v0 = a0 < (uint32_t)8;  /* 0x80138E30: 0x2C820008 */
    int cond_80138E34 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;  /* 0x80138E38: 0x2482FFF8 */
    if (cond_80138E34) {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
        goto block_80138F0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E3Cu);
    }

block_80138E3C:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 6, (uint8_t)v0);  /* 0x80138E40: 0xA0620006 */
    psx_check_interrupts_at(cpu, 0x80138F18u);
    goto block_80138F18;  /* j */

block_80138E44:
    v0 = cpu->read_half(s4 + 130);  /* 0x80138E44: 0x96820082 */
    v1 = cpu->read_word(s1 + 96);  /* 0x80138E48: 0x8E230060 */
    int cond_80138E4C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E50: 0x00000000 */
    if (cond_80138E4C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E54u);
    }

block_80138E54:
    v0 = cpu->read_half(v1 + 8);  /* 0x80138E54: 0x94620008 */
    /* nop */  /* 0x80138E58: 0x00000000 */
    v0--;  /* 0x80138E5C: 0x2442FFFF */
    cpu->write_half(v1 + 8, (uint16_t)v0);  /* 0x80138E60: 0xA4620008 */
    v0 <<= 16;  /* 0x80138E64: 0x00021400 */
    int cond_80138E68 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E6C: 0x00000000 */
    if (cond_80138E68) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E70u);
    }

block_80138E70:
    v0 = cpu->read_word(s4 + 12);  /* 0x80138E70: 0x8E82000C */
    /* nop */  /* 0x80138E74: 0x00000000 */
    v1 = v0 << 2;  /* 0x80138E78: 0x00021880 */
    v1 += v0;  /* 0x80138E7C: 0x00621821 */
    v1 <<= 3;  /* 0x80138E80: 0x000318C0 */
    v1 = s1 + v1;  /* 0x80138E84: 0x02231821 */
    a0 = cpu->read_byte(v1 + 4);  /* 0x80138E88: 0x90640004 */
    /* nop */  /* 0x80138E8C: 0x00000000 */
    v0 = a0 < (uint32_t)8;  /* 0x80138E90: 0x2C820008 */
    int cond_80138E94 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;  /* 0x80138E98: 0x2482FFF8 */
    if (cond_80138E94) {
        psx_check_interrupts_at(cpu, 0x80138EA4u);
        goto block_80138EA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E9Cu);
    }

block_80138E9C:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 4, (uint8_t)zero);  /* 0x80138EA0: 0xA0600004 */
    psx_check_interrupts_at(cpu, 0x80138EA8u);
    goto block_80138EA8;  /* j */

block_80138EA4:
    cpu->write_byte(v1 + 4, (uint8_t)v0);  /* 0x80138EA4: 0xA0620004 */
    /* fall through to block_80138EA8 */

block_80138EA8:
    v0 = cpu->read_word(s4 + 12);  /* 0x80138EA8: 0x8E82000C */
    /* nop */  /* 0x80138EAC: 0x00000000 */
    v1 = v0 << 2;  /* 0x80138EB0: 0x00021880 */
    v1 += v0;  /* 0x80138EB4: 0x00621821 */
    v1 <<= 3;  /* 0x80138EB8: 0x000318C0 */
    v1 = s1 + v1;  /* 0x80138EBC: 0x02231821 */
    a0 = cpu->read_byte(v1 + 5);  /* 0x80138EC0: 0x90640005 */
    /* nop */  /* 0x80138EC4: 0x00000000 */
    v0 = a0 < (uint32_t)8;  /* 0x80138EC8: 0x2C820008 */
    int cond_80138ECC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;  /* 0x80138ED0: 0x2482FFF8 */
    if (cond_80138ECC) {
        psx_check_interrupts_at(cpu, 0x80138EDCu);
        goto block_80138EDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138ED4u);
    }

block_80138ED4:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 5, (uint8_t)zero);  /* 0x80138ED8: 0xA0600005 */
    psx_check_interrupts_at(cpu, 0x80138EE0u);
    goto block_80138EE0;  /* j */

block_80138EDC:
    cpu->write_byte(v1 + 5, (uint8_t)v0);  /* 0x80138EDC: 0xA0620005 */
    /* fall through to block_80138EE0 */

block_80138EE0:
    v0 = cpu->read_word(s4 + 12);  /* 0x80138EE0: 0x8E82000C */
    /* nop */  /* 0x80138EE4: 0x00000000 */
    v1 = v0 << 2;  /* 0x80138EE8: 0x00021880 */
    v1 += v0;  /* 0x80138EEC: 0x00621821 */
    v1 <<= 3;  /* 0x80138EF0: 0x000318C0 */
    v1 = s1 + v1;  /* 0x80138EF4: 0x02231821 */
    a0 = cpu->read_byte(v1 + 6);  /* 0x80138EF8: 0x90640006 */
    /* nop */  /* 0x80138EFC: 0x00000000 */
    v0 = a0 < (uint32_t)8;  /* 0x80138F00: 0x2C820008 */
    int cond_80138F04 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a0 + -8;  /* 0x80138F08: 0x2482FFF8 */
    if (cond_80138F04) {
        psx_check_interrupts_at(cpu, 0x80138F14u);
        goto block_80138F14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
    }

block_80138F0C:
    /* delay slot (always executes) */
    cpu->write_byte(v1 + 6, (uint8_t)zero);  /* 0x80138F10: 0xA0600006 */
    psx_check_interrupts_at(cpu, 0x80138F18u);
    goto block_80138F18;  /* j */

block_80138F14:
    cpu->write_byte(v1 + 6, (uint8_t)v0);  /* 0x80138F14: 0xA0620006 */
    /* fall through to block_80138F18 */

block_80138F18:
    v1 = (int32_t)(int16_t)cpu->read_half(s1 + 100);  /* 0x80138F18: 0x86230064 */
    /* nop */  /* 0x80138F1C: 0x00000000 */
    v0 = v1 < (uint32_t)19;  /* 0x80138F20: 0x2C620013 */
    /* fall through to block_80138F24 */

block_80138F24:
    ;
    int cond_80138F24 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x80138F28: 0x3C028012 */
    if (cond_80138F24) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split -> impact_fx_type_alias (0x80139DE8) */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F2Cu);
    }

block_80138F2C:
    v0 += 184;  /* 0x80138F2C: 0x244200B8 */
    v1 <<= 2;  /* 0x80138F30: 0x00031880 */
    v1 += v0;  /* 0x80138F34: 0x00621821 */
    v0 = cpu->read_word(v1);  /* 0x80138F38: 0x8C620000 */
    /* nop */  /* 0x80138F3C: 0x00000000 */
    uint32_t jt_80138F40 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F44: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_80138F40);
    cpu->pc = jt_80138F40; return;  /* CPS: jr v0 */

block_80138F48:
    v0 = s1 + 88;  /* 0x80138F48: 0x26220058 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));  /* 0x80138F4C: 0xC8400000 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));  /* 0x80138F50: 0xC8410004 */
    /* nop */  /* 0x80138F54: 0x00000000 */
    /* nop */  /* 0x80138F58: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80138F5C: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s3), (cpu->gte_data[14]));  /* 0x80138F60: 0xEA6E0000 */
    gte_precision_store_word((s3), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */  /* 0x80138F64: 0x480C9800 */
    /* nop */  /* 0x80138F68: 0x00000000 */
    t4 = (int32_t)t4 >> 2;  /* 0x80138F6C: 0x000C6083 */
    cpu->write_word(s5, t4);  /* 0x80138F70: 0xAEAC0000 */
    a0 = cpu->read_word(s5);  /* 0x80138F74: 0x8EA40000 */
    /* nop */  /* 0x80138F78: 0x00000000 */
    v0 = a0 + -4;  /* 0x80138F7C: 0x2482FFFC */
    v0 = v0 < (uint32_t)2059;  /* 0x80138F80: 0x2C42080B */
    int cond_80138F84 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F88: 0x00000000 */
    if (cond_80138F84) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split -> impact_fx_type_alias (0x80139DE8) */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F8Cu);
    }

block_80138F8C:
    v0 = cpu->read_half(s4 + 36);  /* 0x80138F8C: 0x96820024 */
    v1 = cpu->read_word(s1 + 84);  /* 0x80138F90: 0x8E230054 */
    v0 = (int32_t)(int16_t)v0;  /* 0x80138F94: 0x00021400 */  /* 0x80138F98: 0x00021403 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));  /* 0x80138F9C: 0x00620018 */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80138FA0: 0x00001812 */
    v0 = a0 << 2;  /* 0x80138FA4: 0x00041080 */
    /* nop */  /* 0x80138FA8: 0x00000000 */
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)v0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);  /* 0x80138FAC: 0x0062001A */
    s0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80138FB0: 0x00008012 */
    v0 = 0x801F0000u;  /* 0x801F0000 */  /* 0x80138FB4: 0x3C02801F */
    a0 = (int32_t)(int16_t)cpu->read_half(v0 + 664);  /* 0x80138FB8: 0x84440298 */
    /* delay slot (always executes) */
    a0 += 160;  /* 0x80138FC0: 0x248400A0 */
    r3_jal(0x8018D29Cu, 0x80138FC4u);  /* jal lut_sin_q12 */
    if (br2_bail_pending()) return;
    psx_check_interrupts_at(cpu, 0x80138FC4u);
    goto block_80138FC4;

block_80138FC4:
    a0 = cpu->read_word(s4 + 12);  /* 0x80138FC4: 0x8E84000C */
    /* nop */  /* 0x80138FC8: 0x00000000 */
    v1 = a0 << 2;  /* 0x80138FCC: 0x00041880 */
    v1 += a0;  /* 0x80138FD0: 0x00641821 */
    v1 <<= 3;  /* 0x80138FD4: 0x000318C0 */
    v1 += s1;  /* 0x80138FD8: 0x00711821 */
    a0 = cpu->read_half(s3);  /* 0x80138FDC: 0x96640000 */
    a1 = (uint32_t)s0 >> 31;  /* 0x80138FE0: 0x00102FC2 */
    { int64_t result = (int64_t)(int32_t)s0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(s0));  /* 0x80138FE4: 0x02020018 */
    a1 = s0 + a1;  /* 0x80138FE8: 0x02052821 */
    a1 = (int32_t)a1 >> 1;  /* 0x80138FEC: 0x00052843 */
    a0 -= a1;  /* 0x80138FF0: 0x00852023 */
    cpu->write_half(v1 + 8, (uint16_t)a0);  /* 0x80138FF4: 0xA4640008 */
    a0 = cpu->read_word(s4 + 12);  /* 0x80138FF8: 0x8E84000C */
    /* nop */  /* 0x80138FFC: 0x00000000 */
    v1 = a0 << 2;  /* 0x80139000: 0x00041880 */
    v1 += a0;  /* 0x80139004: 0x00641821 */
    v1 <<= 3;  /* 0x80139008: 0x000318C0 */
    v1 += s1;  /* 0x8013900C: 0x00711821 */
    a0 = cpu->read_half(s3 + 2);  /* 0x80139010: 0x96640002 */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80139014: 0x00001012 */
    a3 = (int32_t)v0 >> 12;  /* 0x80139018: 0x00023B03 */
    v0 = (uint32_t)v0 >> 31;  /* 0x8013901C: 0x000217C2 */
    v0 = a3 + v0;  /* 0x80139020: 0x00E21021 */
    a2 = (int32_t)v0 >> 1;  /* 0x80139024: 0x00023043 */
    a0 -= a2;  /* 0x80139028: 0x00862023 */
    cpu->write_half(v1 + 10, (uint16_t)a0);  /* 0x8013902C: 0xA464000A */
    v1 = cpu->read_word(s4 + 12);  /* 0x80139030: 0x8E83000C */
    /* nop */  /* 0x80139034: 0x00000000 */
    v0 = v1 << 2;  /* 0x80139038: 0x00031080 */
    v0 += v1;  /* 0x8013903C: 0x00431021 */
    v0 <<= 3;  /* 0x80139040: 0x000210C0 */
    v1 = cpu->read_half(s3);  /* 0x80139044: 0x96630000 */
    v0 += s1;  /* 0x80139048: 0x00511021 */
    v1 -= a1;  /* 0x8013904C: 0x00651823 */
    v1 += s0;  /* 0x80139050: 0x00701821 */
    cpu->write_half(v0 + 16, (uint16_t)v1);  /* 0x80139054: 0xA4430010 */
    v1 = cpu->read_word(s4 + 12);  /* 0x80139058: 0x8E83000C */
    /* nop */  /* 0x8013905C: 0x00000000 */
    v0 = v1 << 2;  /* 0x80139060: 0x00031080 */
    v0 += v1;  /* 0x80139064: 0x00431021 */
    v0 <<= 3;  /* 0x80139068: 0x000210C0 */
    v1 = cpu->read_half(s3 + 2);  /* 0x8013906C: 0x96630002 */
    v0 += s1;  /* 0x80139070: 0x00511021 */
    v1 -= a2;  /* 0x80139074: 0x00661823 */
    cpu->write_half(v0 + 18, (uint16_t)v1);  /* 0x80139078: 0xA4430012 */
    v1 = cpu->read_word(s4 + 12);  /* 0x8013907C: 0x8E83000C */
    /* nop */  /* 0x80139080: 0x00000000 */
    v0 = v1 << 2;  /* 0x80139084: 0x00031080 */
    v0 += v1;  /* 0x80139088: 0x00431021 */
    v0 <<= 3;  /* 0x8013908C: 0x000210C0 */
    v1 = cpu->read_half(s3);  /* 0x80139090: 0x96630000 */
    v0 += s1;  /* 0x80139094: 0x00511021 */
    v1 -= a1;  /* 0x80139098: 0x00651823 */
    cpu->write_half(v0 + 24, (uint16_t)v1);  /* 0x8013909C: 0xA4430018 */
    v1 = cpu->read_word(s4 + 12);  /* 0x801390A0: 0x8E83000C */
    /* nop */  /* 0x801390A4: 0x00000000 */
    v0 = v1 << 2;  /* 0x801390A8: 0x00031080 */
    v0 += v1;  /* 0x801390AC: 0x00431021 */
    v0 <<= 3;  /* 0x801390B0: 0x000210C0 */
    a0 = v0 + s1;  /* 0x801390B4: 0x00512021 */
    v1 = cpu->read_half(s3 + 2);  /* 0x801390B8: 0x96630002 */
    v0 = cpu->read_byte(s4 + 21);  /* 0x801390BC: 0x92820015 */
    v1 = (int32_t)(int16_t)v1;  /* 0x801390C0: 0x00031C00 */  /* 0x801390C4: 0x00031C03 */
    int cond_801390C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= a2;  /* 0x801390CC: 0x00661823 */
    if (cond_801390C8) {
        psx_check_interrupts_at(cpu, 0x801390D8u);
        goto block_801390D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801390D0u);
    }

block_801390D0:
    /* delay slot (always executes) */
    v0 = v1 + a2;  /* 0x801390D4: 0x00661021 */
    psx_check_interrupts_at(cpu, 0x801390DCu);
    goto block_801390DC;  /* j */

block_801390D8:
    v0 = v1 + a3;  /* 0x801390D8: 0x00671021 */
    /* fall through to block_801390DC */

block_801390DC:
    cpu->write_half(a0 + 26, (uint16_t)v0);  /* 0x801390DC: 0xA482001A */
    v0 = cpu->read_word(s4 + 12);  /* 0x801390E0: 0x8E82000C */
    v1 = cpu->read_half(s3);  /* 0x801390E4: 0x96630000 */
    a0 = v0 << 2;  /* 0x801390E8: 0x00022080 */
    a0 += v0;  /* 0x801390EC: 0x00822021 */
    a0 <<= 3;  /* 0x801390F0: 0x000420C0 */
    a0 += s1;  /* 0x801390F4: 0x00912021 */
    v0 = (uint32_t)s0 >> 31;  /* 0x801390F8: 0x001017C2 */
    v0 = s0 + v0;  /* 0x801390FC: 0x02021021 */
    v0 = (int32_t)v0 >> 1;  /* 0x80139100: 0x00021043 */
    v1 -= v0;  /* 0x80139104: 0x00621823 */
    v1 += s0;  /* 0x80139108: 0x00701821 */
    cpu->write_half(a0 + 32, (uint16_t)v1);  /* 0x8013910C: 0xA4830020 */
    v1 = cpu->read_word(s4 + 12);  /* 0x80139110: 0x8E83000C */
    /* nop */  /* 0x80139114: 0x00000000 */
    v0 = v1 << 2;  /* 0x80139118: 0x00031080 */
    v0 += v1;  /* 0x8013911C: 0x00431021 */
    v0 <<= 3;  /* 0x80139120: 0x000210C0 */
    a1 = v0 + s1;  /* 0x80139124: 0x00512821 */
    v0 = (uint32_t)a3 >> 31;  /* 0x80139128: 0x000717C2 */
    v0 = a3 + v0;  /* 0x8013912C: 0x00E21021 */
    a0 = (int32_t)v0 >> 1;  /* 0x80139130: 0x00022043 */
    v1 = cpu->read_half(s3 + 2);  /* 0x80139134: 0x96630002 */
    v0 = cpu->read_byte(s4 + 21);  /* 0x80139138: 0x92820015 */
    v1 = (int32_t)(int16_t)v1;  /* 0x8013913C: 0x00031C00 */  /* 0x80139140: 0x00031C03 */
    int cond_80139144 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= a0;  /* 0x80139148: 0x00641823 */
    if (cond_80139144) {
        psx_check_interrupts_at(cpu, 0x80139154u);
        goto block_80139154;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013914Cu);
    }

block_8013914C:
    /* delay slot (always executes) */
    v0 = v1 + a0;  /* 0x80139150: 0x00641021 */
    psx_check_interrupts_at(cpu, 0x80139158u);
    goto block_80139158;  /* j */

block_80139154:
    v0 = v1 + a3;  /* 0x80139154: 0x00671021 */
    /* fall through to block_80139158 */

block_80139158:
    /* delay slot (always executes) */
    cpu->write_half(a1 + 34, (uint16_t)v0);  /* 0x8013915C: 0xA4A20022 */
    psx_check_interrupts_at(cpu, 0x80139A0Cu);
    cpu->pc = 0x80139A0Cu; return;  /* CPS j: split -> cps_80139a0c */

block_80139160:
    v0 = s1 + 88;  /* 0x80139160: 0x26220058 */
    v1 = cpu->read_word(s1 + 96);  /* 0x80139164: 0x8E230060 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v0)));  /* 0x80139168: 0xC8400000 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v0 + 4))));  /* 0x8013916C: 0xC8410004 */
    /* nop */  /* 0x80139170: 0x00000000 */
    /* nop */  /* 0x80139174: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139178: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((s3), (cpu->gte_data[14]));  /* 0x8013917C: 0xEA6E0000 */
    gte_precision_store_word((s3), 14);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */  /* 0x80139180: 0x480C9800 */
    /* nop */  /* 0x80139184: 0x00000000 */
    t4 = (int32_t)t4 >> 2;  /* 0x80139188: 0x000C6083 */
    cpu->write_word(s5, t4);  /* 0x8013918C: 0xAEAC0000 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((v1)));  /* 0x80139190: 0xC8600000 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((v1 + 4))));  /* 0x80139194: 0xC8610004 */
    /* nop */  /* 0x80139198: 0x00000000 */
    /* nop */  /* 0x8013919C: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x801391A0: 0x4A180001 */
    v0 = sp + 24;  /* 0x801391A4: 0x27A20018 */
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[14]));  /* 0x801391A8: 0xE84E0000 */
    gte_precision_store_word((v0), 14);
    a0 = (int32_t)(int16_t)cpu->read_half(s3);  /* 0x801391AC: 0x86640000 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 24);  /* 0x801391B0: 0x87A30018 */
    v0 = cpu->read_half(s3 + 2);  /* 0x801391B4: 0x96620002 */
    a2 = a0 - v1;  /* 0x801391B8: 0x00833023 */
    v0 = (int32_t)(int16_t)v0;  /* 0x801391BC: 0x00021400 */  /* 0x801391C0: 0x00021403 */
    v1 = (int32_t)(int16_t)cpu->read_half(sp + 26);  /* 0x801391C4: 0x87A3001A */
    a0 = cpu->read_half(s3);  /* 0x801391C8: 0x96640000 */
    int cond_801391CC = ((int32_t)a2 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = v0 - v1;  /* 0x801391D0: 0x00434023 */
    if (cond_801391CC) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split -> impact_fx_type_alias (0x80139DE8) */
    } else {
        psx_check_interrupts_at(cpu, 0x801391D4u);
    }

block_801391D4:
    v0 = cpu->read_word(s5);  /* 0x801391D4: 0x8EA20000 */
    /* nop */  /* 0x801391D8: 0x00000000 */
    v0 -= 4;  /* 0x801391DC: 0x2442FFFC */
    v0 = v0 < (uint32_t)2059;  /* 0x801391E0: 0x2C42080B */
    int cond_801391E4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = (uint32_t)a2 >> 31;  /* 0x801391E8: 0x00062FC2 */
    if (cond_801391E4) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split -> impact_fx_type_alias (0x80139DE8) */
    } else {
        psx_check_interrupts_at(cpu, 0x801391ECu);
    }

block_801391EC:
    a1 = a2 + a1;  /* 0x801391EC: 0x00C52821 */
    v1 = cpu->read_word(s4 + 12);  /* 0x801391F0: 0x8E83000C */
    a1 = (int32_t)a1 >> 1;  /* 0x801391F4: 0x00052843 */
    v0 = v1 << 2;  /* 0x801391F8: 0x00031080 */
    v0 += v1;  /* 0x801391FC: 0x00431021 */
    v0 <<= 3;  /* 0x80139200: 0x000210C0 */
    v0 += s1;  /* 0x80139204: 0x00511021 */
    v1 = a0 - a1;  /* 0x80139208: 0x00851823 */
    cpu->write_half(v0 + 8, (uint16_t)v1);  /* 0x8013920C: 0xA4430008 */
    v1 = cpu->read_word(s4 + 12);  /* 0x80139210: 0x8E83000C */
    a0 = cpu->read_half(s3 + 2);  /* 0x80139214: 0x96640002 */
    v0 = v1 << 2;  /* 0x80139218: 0x00031080 */
    v0 += v1;  /* 0x8013921C: 0x00431021 */
    v0 <<= 3;  /* 0x80139220: 0x000210C0 */
    v0 += s1;  /* 0x80139224: 0x00511021 */
    v1 = (uint32_t)t0 >> 31;  /* 0x80139228: 0x00081FC2 */
    v1 = t0 + v1;  /* 0x8013922C: 0x01031821 */
    a3 = (int32_t)v1 >> 1;  /* 0x80139230: 0x00033843 */
    a0 -= a3;  /* 0x80139234: 0x00872023 */
    cpu->write_half(v0 + 10, (uint16_t)a0);  /* 0x80139238: 0xA444000A */
    v1 = cpu->read_word(s4 + 12);  /* 0x8013923C: 0x8E83000C */
    /* nop */  /* 0x80139240: 0x00000000 */
    v0 = v1 << 2;  /* 0x80139244: 0x00031080 */
    v0 += v1;  /* 0x80139248: 0x00431021 */
    v0 <<= 3;  /* 0x8013924C: 0x000210C0 */
    v1 = cpu->read_half(s3);  /* 0x80139250: 0x96630000 */
    v0 += s1;  /* 0x80139254: 0x00511021 */
    v1 -= a1;  /* 0x80139258: 0x00651823 */
    v1 += a2;  /* 0x8013925C: 0x00661821 */
    cpu->write_half(v0 + 16, (uint16_t)v1);  /* 0x80139260: 0xA4430010 */
    v1 = cpu->read_word(s4 + 12);  /* 0x80139264: 0x8E83000C */
    /* nop */  /* 0x80139268: 0x00000000 */
    v0 = v1 << 2;  /* 0x8013926C: 0x00031080 */
    v0 += v1;  /* 0x80139270: 0x00431021 */
    v0 <<= 3;  /* 0x80139274: 0x000210C0 */
    v1 = cpu->read_half(s3 + 2);  /* 0x80139278: 0x96630002 */
    v0 += s1;  /* 0x8013927C: 0x00511021 */
    v1 -= a3;  /* 0x80139280: 0x00671823 */
    cpu->write_half(v0 + 18, (uint16_t)v1);  /* 0x80139284: 0xA4430012 */
    v1 = cpu->read_word(s4 + 12);  /* 0x80139288: 0x8E83000C */
    /* nop */  /* 0x8013928C: 0x00000000 */
    v0 = v1 << 2;  /* 0x80139290: 0x00031080 */
    v0 += v1;  /* 0x80139294: 0x00431021 */
    v0 <<= 3;  /* 0x80139298: 0x000210C0 */
    v1 = cpu->read_half(s3);  /* 0x8013929C: 0x96630000 */
    v0 += s1;  /* 0x801392A0: 0x00511021 */
    v1 -= a1;  /* 0x801392A4: 0x00651823 */
    cpu->write_half(v0 + 24, (uint16_t)v1);  /* 0x801392A8: 0xA4430018 */
    v1 = cpu->read_word(s4 + 12);  /* 0x801392AC: 0x8E83000C */
    /* nop */  /* 0x801392B0: 0x00000000 */
    v0 = v1 << 2;  /* 0x801392B4: 0x00031080 */
    v0 += v1;  /* 0x801392B8: 0x00431021 */
    v0 <<= 3;  /* 0x801392BC: 0x000210C0 */
    a0 = v0 + s1;  /* 0x801392C0: 0x00512021 */
    v1 = cpu->read_half(s3 + 2);  /* 0x801392C4: 0x96630002 */
    v0 = cpu->read_byte(s4 + 21);  /* 0x801392C8: 0x92820015 */
    v1 = (int32_t)(int16_t)v1;  /* 0x801392CC: 0x00031C00 */  /* 0x801392D0: 0x00031C03 */
    int cond_801392D4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= a3;  /* 0x801392D8: 0x00671823 */
    if (cond_801392D4) {
        psx_check_interrupts_at(cpu, 0x801392E4u);
        goto block_801392E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801392DCu);
    }

block_801392DC:
    /* delay slot (always executes) */
    v0 = v1 + a3;  /* 0x801392E0: 0x00671021 */
    psx_check_interrupts_at(cpu, 0x801392E8u);
    goto block_801392E8;  /* j */

block_801392E4:
    v0 = v1 + t0;  /* 0x801392E4: 0x00681021 */
    /* fall through to block_801392E8 */

block_801392E8:
    cpu->write_half(a0 + 26, (uint16_t)v0);  /* 0x801392E8: 0xA482001A */
    v0 = cpu->read_word(s4 + 12);  /* 0x801392EC: 0x8E82000C */
    v1 = cpu->read_half(s3);  /* 0x801392F0: 0x96630000 */
    a0 = v0 << 2;  /* 0x801392F4: 0x00022080 */
    a0 += v0;  /* 0x801392F8: 0x00822021 */
    a0 <<= 3;  /* 0x801392FC: 0x000420C0 */
    a0 += s1;  /* 0x80139300: 0x00912021 */
    v0 = (uint32_t)a2 >> 31;  /* 0x80139304: 0x000617C2 */
    v0 = a2 + v0;  /* 0x80139308: 0x00C21021 */
    v0 = (int32_t)v0 >> 1;  /* 0x8013930C: 0x00021043 */
    v1 -= v0;  /* 0x80139310: 0x00621823 */
    v1 += a2;  /* 0x80139314: 0x00661821 */
    cpu->write_half(a0 + 32, (uint16_t)v1);  /* 0x80139318: 0xA4830020 */
    v1 = cpu->read_word(s4 + 12);  /* 0x8013931C: 0x8E83000C */
    /* nop */  /* 0x80139320: 0x00000000 */
    v0 = v1 << 2;  /* 0x80139324: 0x00031080 */
    v0 += v1;  /* 0x80139328: 0x00431021 */
    v0 <<= 3;  /* 0x8013932C: 0x000210C0 */
    a1 = v0 + s1;  /* 0x80139330: 0x00512821 */
    v0 = (uint32_t)t0 >> 31;  /* 0x80139334: 0x000817C2 */
    v0 = t0 + v0;  /* 0x80139338: 0x01021021 */
    a0 = (int32_t)v0 >> 1;  /* 0x8013933C: 0x00022043 */
    v1 = cpu->read_half(s3 + 2);  /* 0x80139340: 0x96630002 */
    v0 = cpu->read_byte(s4 + 21);  /* 0x80139344: 0x92820015 */
    v1 = (int32_t)(int16_t)v1;  /* 0x80139348: 0x00031C00 */  /* 0x8013934C: 0x00031C03 */
    int cond_80139350 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 -= a0;  /* 0x80139354: 0x00641823 */
    if (cond_80139350) {
        psx_check_interrupts_at(cpu, 0x80139360u);
        goto block_80139360;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139358u);
    }

block_80139358:
    /* delay slot (always executes) */
    v0 = v1 + a0;  /* 0x8013935C: 0x00641021 */
    psx_check_interrupts_at(cpu, 0x80139364u);
    goto block_80139364;  /* j */

block_80139360:
    v0 = v1 + t0;  /* 0x80139360: 0x00681021 */
    /* fall through to block_80139364 */

block_80139364:
    /* delay slot (always executes) */
    cpu->write_half(a1 + 34, (uint16_t)v0);  /* 0x80139368: 0xA4A20022 */
    psx_check_interrupts_at(cpu, 0x80139A0Cu);
    cpu->pc = 0x80139A0Cu; return;  /* CPS j: split -> cps_80139a0c */

block_8013936C:
    v0 = cpu->read_half(s4 + 130);  /* 0x8013936C: 0x96820082 */
    /* nop */  /* 0x80139370: 0x00000000 */
    int cond_80139374 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s1 + 88;  /* 0x80139378: 0x26220058 */
    if (cond_80139374) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        cpu->pc = 0x80139834u; return;  /* CPS taken: split -> cps_80139834 */
    } else {
        psx_check_interrupts_at(cpu, 0x8013937Cu);
    }

block_8013937C:
    a1 = cpu->read_word(s1 + 96);  /* 0x8013937C: 0x8E250060 */
    v0 = cpu->read_half(s1 + 88);  /* 0x80139380: 0x96220058 */
    v1 = cpu->read_half(a1);  /* 0x80139384: 0x94A30000 */
    /* nop */  /* 0x80139388: 0x00000000 */
    v0 += v1;  /* 0x8013938C: 0x00431021 */
    cpu->write_half(s1 + 88, (uint16_t)v0);  /* 0x80139390: 0xA6220058 */
    v0 = cpu->read_half(s1 + 90);  /* 0x80139394: 0x9622005A */
    v1 = cpu->read_half(a1 + 2);  /* 0x80139398: 0x94A30002 */
    /* nop */  /* 0x8013939C: 0x00000000 */
    v0 += v1;  /* 0x801393A0: 0x00431021 */
    cpu->write_half(s1 + 90, (uint16_t)v0);  /* 0x801393A4: 0xA622005A */
    v0 = cpu->read_half(s1 + 92);  /* 0x801393A8: 0x9622005C */
    v1 = cpu->read_half(a1 + 4);  /* 0x801393AC: 0x94A30004 */
    /* nop */  /* 0x801393B0: 0x00000000 */
    v0 += v1;  /* 0x801393B4: 0x00431021 */
    cpu->pc = 0x801393b8u; return;  /* fallthrough to split piece -> cps_801393b8 */
}

