/* recon_8019.c — clean-room reconstructions of guest functions in the
 * 0x8019xxxx/0x801Axxxx band, ported from the cpst_ timing-faithful CPS
 * transliterations in cd_irq_probe.c.
 *
 * TIMING CONTRACT (proof/cps_fallthrough_cycle_wedge.md): this band is
 * CD/IRQ/kernel + GTE render-pipeline code. Unlike the game-logic cps_
 * bodies, these ports MUST keep the per-instruction psx_cyc_step /
 * psx_cyc_load_* / psx_cyc_lwc2_read / psx_icache_fetch / psx_slice_block
 * calls and the psx_gte_stall / psx_muldiv_* / gte_execute timing hooks —
 * device deadlines (CD IRQ, DMA, VBlank, GTE/MUL-DIV pipes) service
 * inside a piece only through them. clean_cost supplies only the
 * per-dispatch estimate; it does NOT replace intra-piece servicing.
 *
 * Coroutine contract (cd_irq_init_seq / cd_response_pump style): the body
 * resumes on cpu->pc via the entry switch, every psx_check_interrupts_at
 * is a hard IRQ/yield delivery point, and external jal targets tail-
 * dispatch through cpu->pc so clean_try_dispatch re-enters the callee.
 * clean_cps.map routes the entry + all continuation addrs here; the
 * cpst_* bodies in cd_irq_probe.c remain as unreachable shadow.
 *
 * The text-window ports at the bottom (text_win_emit / text_format_run)
 * use the ATOMIC variant: no cpu->pc resume switch; jal edges go through
 * rs_jal and IRQ resume falls through to generated dispatch.
 * All mutable state lives in cpu->gpr[]/guest RAM so scheduler longjmps
 * are safe.
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

/* ======== cd_init_session (guest 0x80198AF0) ========
 * CD read-session init + async-Pause handshake (29 CPS blocks / 416
 * bytes). Probes the CD-init flag cell, busy-waits, walks the mc_session
 * channel table, issues CD cmd 9 via cd_cmd_issue_async, then polls the
 * session timers through cpst_8019a460 (cd_timer_read family). Ports the
 * cpst_80198af0 timing-faithful body 1:1 — the BIOS-trampoline /
 * mc_session / cd_cmd_issue_async jals are kernel+device boundaries that
 * need the generated CPS live-resume contract. */

void cd_init_session(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80198B18u: goto block_80198B18;
            case 0x80198B50u: goto block_80198B50;
            case 0x80198B70u: goto block_80198B70;
            case 0x80198B84u: goto block_80198B84;
            case 0x80198BA0u: goto block_80198BA0;
            case 0x80198BE0u: goto block_80198BE0;
            case 0x80198C04u: goto block_80198C04;
            case 0x80198C10u: goto block_80198C10;
            case 0x80198C1Cu: goto block_80198C1C;
            case 0x80198C40u: goto block_80198C40;
            case 0x80198C5Cu: goto block_80198C5C;
            case 0x80198B10u: goto block_80198B10;
            case 0x80198B34u: goto block_80198B34;
            case 0x80198B3Cu: goto block_80198B3C;
            case 0x80198B5Cu: goto block_80198B5C;
            case 0x80198B7Cu: goto block_80198B7C;
            case 0x80198B94u: goto block_80198B94;
            case 0x80198B98u: goto block_80198B98;
            case 0x80198BB0u: goto block_80198BB0;
            case 0x80198BC0u: goto block_80198BC0;
            case 0x80198BC4u: goto block_80198BC4;
            case 0x80198BCCu: goto block_80198BCC;
            case 0x80198BF8u: goto block_80198BF8;
            case 0x80198C38u: goto block_80198C38;
            case 0x80198C50u: goto block_80198C50;
            case 0x80198C54u: goto block_80198C54;
            case 0x80198C6Cu: goto block_80198C6C;
            case 0x80198C7Cu: goto block_80198C7C;
            default: break;
        }
    }
    /* Address: 0x80198AF0, Size: 416 bytes, Blocks: 29 */

block_80198AF0:
    if (psx_slice_block(cpu, 0x80198AF0u, 8u, 0)) return;
    psx_icache_fetch(cpu, 0x80198AF0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198AF0: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14412, 2, 0x4u);  /* 0x80198AF4: 0x8C42C7B4 */
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 32;  /* 0x80198AF8: 0x27BDFFE0 */
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 24, ra);  /* 0x80198AFC: 0xAFBF0018 */
    psx_icache_fetch(cpu, 0x80198B00u);
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 20, s1);  /* 0x80198B00: 0xAFB10014 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0001;  /* 0x80198B04: 0x30420001 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198B08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 16, s0);  /* 0x80198B0C: 0xAFB00010 */
    if (cond_80198B08) {
        psx_check_interrupts_at(cpu, 0x80198B18u);
        goto block_80198B18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198B10u);
    }

block_80198B10:
    if (psx_slice_block(cpu, 0x80198B10u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B10u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198B18u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80198B14: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x801961CCu);
    cpu->pc = 0x801961CCu; return;  /* CPS jal -> cd_busy_wait_fwd (0x801961CC) */

block_80198B18:
    if (psx_slice_block(cpu, 0x80198B18u, 7u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B18u);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198B18: 0x3C10801C */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13440;  /* 0x80198B1C: 0x2610CB80 */
    psx_icache_fetch(cpu, 0x80198B20u);
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_word(s0 + 20, zero);  /* 0x80198B20: 0xAE000014 */
    v0 = psx_cyc_load_word(cpu, s0 + 36, 2, 0x10000u);  /* 0x80198B24: 0x8E020024 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198B28: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198B2C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198B30u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x80198B30: 0x24020001 */
    if (cond_80198B2C) {
        psx_check_interrupts_at(cpu, 0x80198B7Cu);
        goto block_80198B7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198B34u);
    }

block_80198B34:
    if (psx_slice_block(cpu, 0x80198B34u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B34u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198B38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198BC4u);
    goto block_80198BC4;  /* j */

block_80198B3C:
    if (psx_slice_block(cpu, 0x80198B3Cu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B3Cu);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198B3C: 0x3C10801C */
    psx_icache_fetch(cpu, 0x80198B40u);
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13400;  /* 0x80198B40: 0x2610CBA8 */
    a0 = psx_cyc_load_word(cpu, s0, 4, 0x10000u);  /* 0x80198B44: 0x8E040000 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198B50u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 40;  /* 0x80198B4C: 0x2610FFD8 */
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> mc_session (0x80195D64) */

block_80198B50:
    if (psx_slice_block(cpu, 0x80198B50u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B50u);
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x80198B50: 0x2402FFFF */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_word(s0 + 36, zero);  /* 0x80198B58: 0xAE000024 */
    psx_check_interrupts_at(cpu, 0x80198BC4u);
    goto block_80198BC4;  /* j */

block_80198B5C:
    if (psx_slice_block(cpu, 0x80198B5Cu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B5Cu);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198B5C: 0x3C10801C */
    psx_icache_fetch(cpu, 0x80198B60u);
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13400;  /* 0x80198B60: 0x2610CBA8 */
    a0 = psx_cyc_load_word(cpu, s0, 4, 0x10000u);  /* 0x80198B64: 0x8E040000 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198B70u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 40;  /* 0x80198B6C: 0x2610FFD8 */
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> mc_session (0x80195D64) */

block_80198B70:
    if (psx_slice_block(cpu, 0x80198B70u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B70u);
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_word(s0 + 36, zero);  /* 0x80198B70: 0xAE000024 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198B78: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198C7Cu);
    goto block_80198C7C;  /* j */

block_80198B7C:
    if (psx_slice_block(cpu, 0x80198B7Cu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B7Cu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198B84u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198B80u);
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198B80: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> cpst_8019a460 */

block_80198B84:
    if (psx_slice_block(cpu, 0x80198B84u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B84u);
    v1 = psx_cyc_load_word(cpu, s0 + 36, 3, 0x10000u);  /* 0x80198B84: 0x8E030024 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198B88: 0x00000000 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80198B8C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198B90u);
    psx_cyc_step(cpu, 0x20005u);
    s1 = v0;  /* 0x80198B90: 0x00408821 */
    if (cond_80198B8C) {
        psx_check_interrupts_at(cpu, 0x80198BC0u);
        goto block_80198BC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198B94u);
    }

block_80198B94:
    if (psx_slice_block(cpu, 0x80198B94u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B94u);
    psx_cyc_step(cpu, 0x10000u);
    s0 += 36;  /* 0x80198B94: 0x26100024 */
    /* fall through to block_80198B98 */

block_80198B98:
    if (psx_slice_block(cpu, 0x80198B98u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198B98u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198BA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198B9C: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> cpst_8019a460 */

block_80198BA0:
    if (psx_slice_block(cpu, 0x80198BA0u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198BA0u);
    psx_cyc_step(cpu, 0x20004u);
    v0 -= s1;  /* 0x80198BA0: 0x00511023 */
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)121;  /* 0x80198BA4: 0x2C420079 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198BA8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198BAC: 0x00000000 */
    if (cond_80198BA8) {
        psx_check_interrupts_at(cpu, 0x80198B3Cu);
        goto block_80198B3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BB0u);
    }

block_80198BB0:
    if (psx_slice_block(cpu, 0x80198BB0u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198BB0u);
    v0 = psx_cyc_load_word(cpu, s0, 2, 0x10000u);  /* 0x80198BB0: 0x8E020000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198BB4: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198BB8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198BBC: 0x00000000 */
    if (cond_80198BB8) {
        psx_check_interrupts_at(cpu, 0x80198B98u);
        goto block_80198B98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BC0u);
    }

block_80198BC0:
    if (psx_slice_block(cpu, 0x80198BC0u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x80198BC0u);
    psx_cyc_step(cpu, 0x5u);
    v0 = zero;  /* 0x80198BC0: 0x00001021 */
    /* fall through to block_80198BC4 */

block_80198BC4:
    if (psx_slice_block(cpu, 0x80198BC4u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198BC4u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80198BC4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198BC8: 0x00000000 */
    if (cond_80198BC4) {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
        goto block_80198C7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BCCu);
    }

block_80198BCC:
    if (psx_slice_block(cpu, 0x80198BCCu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x80198BCCu);
    psx_cyc_step(cpu, 0x20000u);
    s1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198BCC: 0x3C11801C */
    psx_icache_fetch(cpu, 0x80198BD0u);
    psx_cyc_step(cpu, 0x20000u);
    s1 -= 13396;  /* 0x80198BD0: 0x2631CBAC */
    a0 = psx_cyc_load_word(cpu, s1, 4, 0x20000u);  /* 0x80198BD4: 0x8E240000 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198BE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198BDC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80195D78u);
    cpu->pc = 0x80195D78u; return;  /* CPS jal -> mc_session_b (0x80195D78) */

block_80198BE0:
    if (psx_slice_block(cpu, 0x80198BE0u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x80198BE0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198BE0: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14412, 2, 0x4u);  /* 0x80198BE4: 0x8C42C7B4 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198BE8: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0001;  /* 0x80198BEC: 0x30420001 */
    psx_icache_fetch(cpu, 0x80198BF0u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80198BF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198BF4: 0x00000000 */
    if (cond_80198BF0) {
        psx_check_interrupts_at(cpu, 0x80198C04u);
        goto block_80198C04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BF8u);
    }

block_80198BF8:
    if (psx_slice_block(cpu, 0x80198BF8u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198BF8u);
    a0 = psx_cyc_load_word(cpu, s1 + 4, 4, 0x20000u);  /* 0x80198BF8: 0x8E240004 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198C04u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198C00u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198C00: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return;  /* CPS jal -> cd_cmd_dispatch3 (0x801961A8) */

block_80198C04:
    if (psx_slice_block(cpu, 0x80198C04u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C04u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x801A0000u;  /* 0x801A0000 */  /* 0x80198C04: 0x3C04801A */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198C10u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 -= 31344;  /* 0x80198C0C: 0x24848590 */
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> mc_session (0x80195D64) */

block_80198C10:
    if (psx_slice_block(cpu, 0x80198C10u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C10u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 9;  /* 0x80198C10: 0x24040009 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198C1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x80198C18: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80198C1C:
    if (psx_slice_block(cpu, 0x80198C1Cu, 7u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C1Cu);
    psx_cyc_step(cpu, 0x20004u);
    v0 = s1 + -44;  /* 0x80198C1C: 0x2622FFD4 */
    psx_icache_fetch(cpu, 0x80198C20u);
    psx_cyc_step(cpu, 0x9u);
    v1 = 1;  /* 0x80198C20: 0x24030001 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_word(v0 + 36, v1);  /* 0x80198C24: 0xAC430024 */
    v0 = psx_cyc_load_word(cpu, s1 + -8, 2, 0x20000u);  /* 0x80198C28: 0x8E22FFF8 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198C2C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80198C30u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80198C30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198C34: 0x00000000 */
    if (cond_80198C30) {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
        goto block_80198C7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C38u);
    }

block_80198C38:
    if (psx_slice_block(cpu, 0x80198C38u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C38u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198C40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198C3C: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> cpst_8019a460 */

block_80198C40:
    if (psx_slice_block(cpu, 0x80198C40u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C40u);
    v1 = psx_cyc_load_word(cpu, s1 + -8, 3, 0x20000u);  /* 0x80198C40: 0x8E23FFF8 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198C44: 0x00000000 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80198C48 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10005u);
    s0 = v0;  /* 0x80198C4C: 0x00408021 */
    if (cond_80198C48) {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
        goto block_80198C7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C50u);
    }

block_80198C50:
    if (psx_slice_block(cpu, 0x80198C50u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C50u);
    psx_cyc_step(cpu, 0x20000u);
    s1 -= 8;  /* 0x80198C50: 0x2631FFF8 */
    /* fall through to block_80198C54 */

block_80198C54:
    if (psx_slice_block(cpu, 0x80198C54u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C54u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198C5Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198C58: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> cpst_8019a460 */

block_80198C5C:
    if (psx_slice_block(cpu, 0x80198C5Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C5Cu);
    psx_cyc_step(cpu, 0x10004u);
    v0 -= s0;  /* 0x80198C5C: 0x00501023 */
    psx_icache_fetch(cpu, 0x80198C60u);
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)121;  /* 0x80198C60: 0x2C420079 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198C64 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198C68: 0x00000000 */
    if (cond_80198C64) {
        psx_check_interrupts_at(cpu, 0x80198B5Cu);
        goto block_80198B5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C6Cu);
    }

block_80198C6C:
    if (psx_slice_block(cpu, 0x80198C6Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C6Cu);
    v0 = psx_cyc_load_word(cpu, s1, 2, 0x20000u);  /* 0x80198C6C: 0x8E220000 */
    psx_icache_fetch(cpu, 0x80198C70u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198C70: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198C74 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198C78: 0x00000000 */
    if (cond_80198C74) {
        psx_check_interrupts_at(cpu, 0x80198C54u);
        goto block_80198C54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
    }

block_80198C7C:
    if (psx_slice_block(cpu, 0x80198C7Cu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C7Cu);
    ra = psx_cyc_load_word(cpu, sp + 24, 31, 0x20000000u);  /* 0x80198C7C: 0x8FBF0018 */
    psx_icache_fetch(cpu, 0x80198C80u);
    s1 = psx_cyc_load_word(cpu, sp + 20, 17, 0x20000000u);  /* 0x80198C80: 0x8FB10014 */
    s0 = psx_cyc_load_word(cpu, sp + 16, 16, 0x20000000u);  /* 0x80198C84: 0x8FB00010 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_80198C88 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 32;  /* 0x80198C8C: 0x27BD0020 */
    psx_check_interrupts_at(cpu, jt_80198C88);
    cpu->pc = jt_80198C88; return;  /* CPS: jr $ra */
}
/* ======== cd_read_setup_cps (guest 0x80198C90) ========
 * CD read-setup entry (25 CPS blocks). Reads the pending command block,
 * calls cd_read_open / cd_cmd_issue_sync / cd_cmd_dispatch3 and the
 * mc_session pair; timing-faithful port of cpst_80198c90. The cd.c
 * cd_read_setup(count,pos,mode) 3-arg host wrapper is unchanged — it
 * forwards through call_guest4 into this body. */

void cd_read_setup_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80198CD4u: goto block_80198CD4;
            case 0x80198CECu: goto block_80198CEC;
            case 0x80198D50u: goto block_80198D50;
            case 0x80198DA4u: goto block_80198DA4;
            case 0x80198DB0u: goto block_80198DB0;
            case 0x80198DD0u: goto block_80198DD0;
            case 0x80198DDCu: goto block_80198DDC;
            case 0x80198DE8u: goto block_80198DE8;
            case 0x80198E00u: goto block_80198E00;
            case 0x80198E08u: goto block_80198E08;
            case 0x80198CCCu: goto block_80198CCC;
            case 0x80198CE4u: goto block_80198CE4;
            case 0x80198CFCu: goto block_80198CFC;
            case 0x80198D0Cu: goto block_80198D0C;
            case 0x80198D2Cu: goto block_80198D2C;
            case 0x80198D34u: goto block_80198D34;
            case 0x80198D3Cu: goto block_80198D3C;
            case 0x80198D5Cu: goto block_80198D5C;
            case 0x80198D68u: goto block_80198D68;
            case 0x80198D74u: goto block_80198D74;
            case 0x80198D80u: goto block_80198D80;
            case 0x80198DC8u: goto block_80198DC8;
            case 0x80198DD4u: goto block_80198DD4;
            case 0x80198DF4u: goto block_80198DF4;
            default: break;
        }
    }
    /* Address: 0x80198C90, Size: 412 bytes, Blocks: 25 */

block_80198C90:
    if (psx_slice_block(cpu, 0x80198C90u, 15u, 0)) return;
    psx_icache_fetch(cpu, 0x80198C90u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 40;  /* 0x80198C90: 0x27BDFFD8 */
    psx_cyc_step(cpu, 0x20100000u);
    cpu->write_word(sp + 32, s4);  /* 0x80198C94: 0xAFB40020 */
    psx_cyc_step(cpu, 0x100011u);
    s4 = a0;  /* 0x80198C98: 0x0080A021 */
    psx_cyc_step(cpu, 0x20080000u);
    cpu->write_word(sp + 28, s3);  /* 0x80198C9C: 0xAFB3001C */
    psx_icache_fetch(cpu, 0x80198CA0u);
    psx_cyc_step(cpu, 0x80021u);
    s3 = a1;  /* 0x80198CA0: 0x00A09821 */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 16, s0);  /* 0x80198CA4: 0xAFB00010 */
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198CA8: 0x3C10801C */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13404;  /* 0x80198CAC: 0x2610CBA4 */
    psx_icache_fetch(cpu, 0x80198CB0u);
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 36, ra);  /* 0x80198CB0: 0xAFBF0024 */
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 24, s2);  /* 0x80198CB4: 0xAFB20018 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 20, s1);  /* 0x80198CB8: 0xAFB10014 */
    v0 = psx_cyc_load_word(cpu, s0, 2, 0x10000u);  /* 0x80198CBC: 0x8E020000 */
    psx_icache_fetch(cpu, 0x80198CC0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198CC0: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198CC4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40041u);
    s2 = a2;  /* 0x80198CC8: 0x00C09021 */
    if (cond_80198CC4) {
        psx_check_interrupts_at(cpu, 0x80198D0Cu);
        goto block_80198D0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198CCCu);
    }

block_80198CCC:
    if (psx_slice_block(cpu, 0x80198CCCu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198CCCu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198CD4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198CD0u);
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198CD0: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> cpst_8019a460 */

block_80198CD4:
    if (psx_slice_block(cpu, 0x80198CD4u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198CD4u);
    v1 = psx_cyc_load_word(cpu, s0, 3, 0x10000u);  /* 0x80198CD4: 0x8E030000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198CD8: 0x00000000 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80198CDC = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198CE0u);
    psx_cyc_step(cpu, 0x20005u);
    s1 = v0;  /* 0x80198CE0: 0x00408821 */
    if (cond_80198CDC) {
        psx_check_interrupts_at(cpu, 0x80198D0Cu);
        goto block_80198D0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198CE4u);
    }

block_80198CE4:
    if (psx_slice_block(cpu, 0x80198CE4u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198CE4u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198CECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198CE8: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> cpst_8019a460 */

block_80198CEC:
    if (psx_slice_block(cpu, 0x80198CECu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198CECu);
    psx_cyc_step(cpu, 0x20004u);
    v0 -= s1;  /* 0x80198CEC: 0x00511023 */
    psx_icache_fetch(cpu, 0x80198CF0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)121;  /* 0x80198CF0: 0x2C420079 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198CF4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198CF8: 0x00000000 */
    if (cond_80198CF4) {
        psx_check_interrupts_at(cpu, 0x80198D3Cu);
        goto block_80198D3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198CFCu);
    }

block_80198CFC:
    if (psx_slice_block(cpu, 0x80198CFCu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x80198CFCu);
    v0 = psx_cyc_load_word(cpu, s0, 2, 0x10000u);  /* 0x80198CFC: 0x8E020000 */
    psx_icache_fetch(cpu, 0x80198D00u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198D00: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198D04 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198D08: 0x00000000 */
    if (cond_80198D04) {
        psx_check_interrupts_at(cpu, 0x80198CE4u);
        goto block_80198CE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198D0Cu);
    }

block_80198D0C:
    if (psx_slice_block(cpu, 0x80198D0Cu, 8u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D0Cu);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198D0C: 0x3C04801C */
    psx_icache_fetch(cpu, 0x80198D10u);
    psx_cyc_step(cpu, 0x10u);
    a0 -= 13440;  /* 0x80198D10: 0x2484CB80 */
    psx_cyc_step(cpu, 0x40010u);
    cpu->write_word(a0 + 12, s2);  /* 0x80198D14: 0xAC92000C */
    v0 = psx_cyc_load_word(cpu, a0 + 12, 2, 0x10u);  /* 0x80198D18: 0x8C82000C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198D1C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80198D20u);
    psx_cyc_step(cpu, 0xCu);
    v1 = v0 & 0x0030;  /* 0x80198D20: 0x30430030 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80198D24 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 32;  /* 0x80198D28: 0x24020020 */
    if (cond_80198D24) {
        psx_check_interrupts_at(cpu, 0x80198D5Cu);
        goto block_80198D5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198D2Cu);
    }

block_80198D2C:
    if (psx_slice_block(cpu, 0x80198D2Cu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D2Cu);
    psx_cyc_step(cpu, 0xCu);
    int cond_80198D2C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198D30u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 582;  /* 0x80198D30: 0x24020246 */
    if (cond_80198D2C) {
        psx_check_interrupts_at(cpu, 0x80198D68u);
        goto block_80198D68;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198D34u);
    }

block_80198D34:
    if (psx_slice_block(cpu, 0x80198D34u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D34u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198D38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198D74u);
    goto block_80198D74;  /* j */

block_80198D3C:
    if (psx_slice_block(cpu, 0x80198D3Cu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D3Cu);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198D3C: 0x3C10801C */
    psx_icache_fetch(cpu, 0x80198D40u);
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13400;  /* 0x80198D40: 0x2610CBA8 */
    a0 = psx_cyc_load_word(cpu, s0, 4, 0x10000u);  /* 0x80198D44: 0x8E040000 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198D50u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 40;  /* 0x80198D4C: 0x2610FFD8 */
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> mc_session (0x80195D64) */

block_80198D50:
    if (psx_slice_block(cpu, 0x80198D50u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D50u);
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_word(s0 + 36, zero);  /* 0x80198D50: 0xAE000024 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198D58: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198D0Cu);
    goto block_80198D0C;  /* j */

block_80198D5C:
    if (psx_slice_block(cpu, 0x80198D5Cu, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D5Cu);
    psx_cyc_step(cpu, 0x5u);
    v0 = 512;  /* 0x80198D5C: 0x24020200 */
    psx_icache_fetch(cpu, 0x80198D60u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x14u);
    cpu->write_word(a0 + 16, v0);  /* 0x80198D64: 0xAC820010 */
    psx_check_interrupts_at(cpu, 0x80198D80u);
    goto block_80198D80;  /* j */

block_80198D68:
    if (psx_slice_block(cpu, 0x80198D68u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D68u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 585;  /* 0x80198D68: 0x24020249 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198D70u);
    psx_cyc_step(cpu, 0x14u);
    cpu->write_word(a0 + 16, v0);  /* 0x80198D70: 0xAC820010 */
    psx_check_interrupts_at(cpu, 0x80198D80u);
    goto block_80198D80;  /* j */

block_80198D74:
    if (psx_slice_block(cpu, 0x80198D74u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D74u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198D74: 0x3C03801C */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 13440;  /* 0x80198D78: 0x2463CB80 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_word(v1 + 16, v0);  /* 0x80198D7C: 0xAC620010 */
    /* fall through to block_80198D80 */

block_80198D80:
    if (psx_slice_block(cpu, 0x80198D80u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x80198D80u);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198D80: 0x3C10801C */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13440;  /* 0x80198D84: 0x2610CB80 */
    v0 = psx_cyc_load_word(cpu, s0 + 12, 2, 0x10000u);  /* 0x80198D88: 0x8E02000C */
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80198D8C: 0x00002021 */
    psx_icache_fetch(cpu, 0x80198D90u);
    psx_cyc_step(cpu, 0x4u);
    v0 |= 0x0020;  /* 0x80198D90: 0x34420020 */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 12, v0);  /* 0x80198D94: 0xAE02000C */
    psx_cyc_step(cpu, 0x90000u);
    cpu->write_word(s0 + 4, s3);  /* 0x80198D98: 0xAE130004 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198DA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198DA0u);
    psx_cyc_step(cpu, 0x110000u);
    cpu->write_word(s0, s4);  /* 0x80198DA0: 0xAE140000 */
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> mc_session (0x80195D64) */

block_80198DA4:
    if (psx_slice_block(cpu, 0x80198DA4u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DA4u);
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80198DA4: 0x00002021 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198DB0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 40, v0);  /* 0x80198DAC: 0xAE020028 */
    psx_check_interrupts_at(cpu, 0x80195D78u);
    cpu->pc = 0x80195D78u; return;  /* CPS jal -> mc_session_b (0x80195D78) */

block_80198DB0:
    if (psx_slice_block(cpu, 0x80198DB0u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DB0u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198DB0: 0x3C03801C */
    v1 = psx_cyc_load_word(cpu, v1 + -14412, 3, 0x8u);  /* 0x80198DB4: 0x8C63C7B4 */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 44, v0);  /* 0x80198DB8: 0xAE02002C */
    psx_cyc_step(cpu, 0x8u);
    v1 &= 0x0001;  /* 0x80198DBC: 0x30630001 */
    psx_icache_fetch(cpu, 0x80198DC0u);
    psx_cyc_step(cpu, 0x9u);
    int cond_80198DC0 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198DC4: 0x00000000 */
    if (cond_80198DC0) {
        psx_check_interrupts_at(cpu, 0x80198DD4u);
        goto block_80198DD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198DC8u);
    }

block_80198DC8:
    if (psx_slice_block(cpu, 0x80198DC8u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DC8u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198DD0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80198DCC: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return;  /* CPS jal -> cd_cmd_dispatch3 (0x801961A8) */

block_80198DD0:
    if (psx_slice_block(cpu, 0x80198DD0u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DD0u);
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 48, v0);  /* 0x80198DD0: 0xAE020030 */
    /* fall through to block_80198DD4 */

block_80198DD4:
    if (psx_slice_block(cpu, 0x80198DD4u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DD4u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198DDCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198DD8: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> cpst_8019a460 */

block_80198DDC:
    if (psx_slice_block(cpu, 0x80198DDCu, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DDCu);
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 28, v0);  /* 0x80198DDC: 0xAE02001C */
    psx_icache_fetch(cpu, 0x80198DE0u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198DE8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198DE4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80195BE0u);
    cpu->pc = 0x80195BE0u; return;  /* CPS jal -> cd_status_raw (0x80195BE0) */

block_80198DE8:
    if (psx_slice_block(cpu, 0x80198DE8u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DE8u);
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x00E0;  /* 0x80198DE8: 0x304200E0 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198DEC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198DF0u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 9;  /* 0x80198DF0: 0x24040009 */
    if (cond_80198DEC) {
        psx_check_interrupts_at(cpu, 0x80198E00u);
        goto block_80198E00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198DF4u);
    }

block_80198DF4:
    if (psx_slice_block(cpu, 0x80198DF4u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x80198DF4u);
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x80198DF4: 0x00002821 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198E00u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x41u);
    a2 = zero;  /* 0x80198DFC: 0x00003021 */
    psx_check_interrupts_at(cpu, 0x80195FFCu);
    cpu->pc = 0x80195FFCu; return;  /* CPS jal -> cd_cmd_issue_sync (0x80195FFC) */

block_80198E00:
    if (psx_slice_block(cpu, 0x80198E00u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x80198E00u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198E08u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80198E04: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x80198908u);
    cpu->pc = 0x80198908u; return;  /* CPS jal -> cd_read_open (0x80198908) */

block_80198E08:
    if (psx_slice_block(cpu, 0x80198E08u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x80198E08u);
    psx_cyc_step(cpu, 0x5u);
    v0 = (int32_t)zero < (int32_t)v0;  /* 0x80198E08: 0x0002102A */
    ra = psx_cyc_load_word(cpu, sp + 36, 31, 0x20000000u);  /* 0x80198E0C: 0x8FBF0024 */
    psx_icache_fetch(cpu, 0x80198E10u);
    s4 = psx_cyc_load_word(cpu, sp + 32, 20, 0x20000000u);  /* 0x80198E10: 0x8FB40020 */
    s3 = psx_cyc_load_word(cpu, sp + 28, 19, 0x20000000u);  /* 0x80198E14: 0x8FB3001C */
    s2 = psx_cyc_load_word(cpu, sp + 24, 18, 0x20000000u);  /* 0x80198E18: 0x8FB20018 */
    s1 = psx_cyc_load_word(cpu, sp + 20, 17, 0x20000000u);  /* 0x80198E1C: 0x8FB10014 */
    psx_icache_fetch(cpu, 0x80198E20u);
    s0 = psx_cyc_load_word(cpu, sp + 16, 16, 0x20000000u);  /* 0x80198E20: 0x8FB00010 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_80198E24 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 40;  /* 0x80198E28: 0x27BD0028 */
    psx_check_interrupts_at(cpu, jt_80198E24);
    cpu->pc = jt_80198E24; return;  /* CPS: jr $ra */
}
/* ======== cd_chan_cmd_pump_cps (guest 0x801A0660) ========
 * CD command pump, up to 8 retries (22 CPS blocks / 324 bytes): XOR
 * checksum over the 126-byte command image at a2, then the
 * cd_svc_50/4e/5c/4f + cd_bzero service calls with retry-on-mismatch.
 * The cd.c cd_chan_cmd_pump(a0,a1,a2) 3-arg host wrapper is unchanged —
 * it forwards through call_guest4 into this body. */

void cd_chan_cmd_pump_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A06D4u: goto block_801A06D4;
            case 0x801A06E4u: goto block_801A06E4;
            case 0x801A06F4u: goto block_801A06F4;
            case 0x801A0708u: goto block_801A0708;
            case 0x801A0710u: goto block_801A0710;
            case 0x801A0720u: goto block_801A0720;
            case 0x801A0734u: goto block_801A0734;
            case 0x801A0744u: goto block_801A0744;
            case 0x801A069Cu: goto block_801A069C;
            case 0x801A06B4u: goto block_801A06B4;
            case 0x801A06C0u: goto block_801A06C0;
            case 0x801A06CCu: goto block_801A06CC;
            case 0x801A06ECu: goto block_801A06EC;
            case 0x801A0700u: goto block_801A0700;
            case 0x801A0728u: goto block_801A0728;
            case 0x801A073Cu: goto block_801A073C;
            case 0x801A0750u: goto block_801A0750;
            case 0x801A0758u: goto block_801A0758;
            case 0x801A076Cu: goto block_801A076C;
            case 0x801A077Cu: goto block_801A077C;
            case 0x801A0780u: goto block_801A0780;
            default: break;
        }
    }
    /* Address: 0x801A0660, Size: 324 bytes, Blocks: 22 */

block_801A0660:
    if (psx_slice_block(cpu, 0x801A0660u, 15u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0660u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 176;  /* 0x801A0660: 0x27BDFF50 */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 144, s0);  /* 0x801A0664: 0xAFB00090 */
    psx_cyc_step(cpu, 0x10011u);
    s0 = a0;  /* 0x801A0668: 0x00808021 */
    psx_cyc_step(cpu, 0x20100000u);
    cpu->write_word(sp + 160, s4);  /* 0x801A066C: 0xAFB400A0 */
    psx_icache_fetch(cpu, 0x801A0670u);
    psx_cyc_step(cpu, 0x100021u);
    s4 = a1;  /* 0x801A0670: 0x00A0A021 */
    psx_cyc_step(cpu, 0x20080000u);
    cpu->write_word(sp + 156, s3);  /* 0x801A0674: 0xAFB3009C */
    psx_cyc_step(cpu, 0x80041u);
    s3 = a2;  /* 0x801A0678: 0x00C09821 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 148, s1);  /* 0x801A067C: 0xAFB10094 */
    psx_icache_fetch(cpu, 0x801A0680u);
    psx_cyc_step(cpu, 0x20001u);
    s1 = zero;  /* 0x801A0680: 0x00008821 */
    psx_cyc_step(cpu, 0x80009u);
    v1 = s3;  /* 0x801A0684: 0x02601821 */
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x801A0688: 0x00002821 */
    psx_cyc_step(cpu, 0x11u);
    a0 = 126;  /* 0x801A068C: 0x2404007E */
    psx_icache_fetch(cpu, 0x801A0690u);
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 168, ra);  /* 0x801A0690: 0xAFBF00A8 */
    psx_cyc_step(cpu, 0x20200000u);
    cpu->write_word(sp + 164, s5);  /* 0x801A0694: 0xAFB500A4 */
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 152, s2);  /* 0x801A0698: 0xAFB20098 */
    /* fall through to block_801A069C */

block_801A069C:
    if (psx_slice_block(cpu, 0x801A069Cu, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801A069Cu);
    v0 = psx_cyc_load_byte(cpu, v1, 2, 0x8u);  /* 0x801A069C: 0x90620000 */
    psx_icache_fetch(cpu, 0x801A06A0u);
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x801A06A0: 0x24630001 */
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x801A06A4: 0x2484FFFF */
    psx_cyc_step(cpu, 0x24u);
    v0 = a1 ^ v0;  /* 0x801A06A8: 0x00A21026 */
    psx_cyc_step(cpu, 0x10u);
    int cond_801A06AC = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A06B0u);
    psx_cyc_step(cpu, 0x25u);
    a1 = v0;  /* 0x801A06B0: 0x00402821 */
    if (cond_801A06AC) {
        psx_check_interrupts_at(cpu, 0x801A069Cu);
        goto block_801A069C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A06B4u);
    }

block_801A06B4:
    if (psx_slice_block(cpu, 0x801A06B4u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801A06B4u);
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x801A06B4: 0xA0620000 */
    psx_cyc_step(cpu, 0x200001u);
    s5 = 1;  /* 0x801A06B8: 0x24150001 */
    psx_cyc_step(cpu, 0x20040000u);
    s2 = sp + 16;  /* 0x801A06BC: 0x27B20010 */
    /* fall through to block_801A06C0 */

block_801A06C0:
    if (psx_slice_block(cpu, 0x801A06C0u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801A06C0u);
    psx_cyc_step(cpu, 0x20004u);
    v0 = (int32_t)s1 < 8;  /* 0x801A06C0: 0x2A220008 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A06C4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = zero;  /* 0x801A06C8: 0x00001021 */
    if (cond_801A06C4) {
        psx_check_interrupts_at(cpu, 0x801A0780u);
        goto block_801A0780;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A06CCu);
    }

block_801A06CC:
    if (psx_slice_block(cpu, 0x801A06CCu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A06CCu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A06D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A06D0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A06D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A0610u);
    cpu->pc = 0x801A0610u; return;  /* CPS jal -> cd_svc_50 (0x801A0610) */

block_801A06D4:
    if (psx_slice_block(cpu, 0x801A06D4u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A06D4u);
    psx_cyc_step(cpu, 0x10011u);
    a0 = s0;  /* 0x801A06D4: 0x02002021 */
    psx_cyc_step(cpu, 0x100021u);
    a1 = s4;  /* 0x801A06D8: 0x02802821 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A06E4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A06E0u);
    psx_cyc_step(cpu, 0x80041u);
    a2 = s3;  /* 0x801A06E0: 0x02603021 */
    psx_check_interrupts_at(cpu, 0x801A0600u);
    cpu->pc = 0x801A0600u; return;  /* CPS jal -> cd_svc_4e (0x801A0600) */

block_801A06E4:
    if (psx_slice_block(cpu, 0x801A06E4u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A06E4u);
    psx_cyc_step(cpu, 0x200004u);
    int cond_801A06E4 = (v0 != s5);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = zero;  /* 0x801A06E8: 0x00001021 */
    if (cond_801A06E4) {
        psx_check_interrupts_at(cpu, 0x801A0780u);
        goto block_801A0780;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A06ECu);
    }

block_801A06EC:
    if (psx_slice_block(cpu, 0x801A06ECu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A06ECu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A06F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A06F0u);
    psx_cyc_step(cpu, 0x10010u);
    a0 = (int32_t)s0 >> 4;  /* 0x801A06F0: 0x00102103 */
    psx_check_interrupts_at(cpu, 0x801A09F0u);
    cpu->pc = 0x801A09F0u; return;  /* CPS jal -> cd_svc_5c (0x801A09F0) */

block_801A06F4:
    if (psx_slice_block(cpu, 0x801A06F4u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801A06F4u);
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0001;  /* 0x801A06F4: 0x30420001 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A06F8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40011u);
    a0 = s2;  /* 0x801A06FC: 0x02402021 */
    if (cond_801A06F8) {
        psx_check_interrupts_at(cpu, 0x801A06ECu);
        goto block_801A06EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0700u);
    }

block_801A0700:
    if (psx_slice_block(cpu, 0x801A0700u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0700u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A0708u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x21u);
    a1 = 128;  /* 0x801A0704: 0x24050080 */
    psx_check_interrupts_at(cpu, 0x801A09B0u);
    cpu->pc = 0x801A09B0u; return;  /* CPS jal -> cd_bzero (0x801A09B0) */

block_801A0708:
    if (psx_slice_block(cpu, 0x801A0708u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0708u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A0710u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A070C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A0610u);
    cpu->pc = 0x801A0610u; return;  /* CPS jal -> cd_svc_50 (0x801A0610) */

block_801A0710:
    if (psx_slice_block(cpu, 0x801A0710u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0710u);
    psx_cyc_step(cpu, 0x10011u);
    a0 = s0;  /* 0x801A0710: 0x02002021 */
    psx_cyc_step(cpu, 0x100021u);
    a1 = s4;  /* 0x801A0714: 0x02802821 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A0720u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x40041u);
    a2 = s2;  /* 0x801A071C: 0x02403021 */
    psx_check_interrupts_at(cpu, 0x801A09E0u);
    cpu->pc = 0x801A09E0u; return;  /* CPS jal -> cd_svc_4f (0x801A09E0) */

block_801A0720:
    if (psx_slice_block(cpu, 0x801A0720u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0720u);
    psx_cyc_step(cpu, 0x200004u);
    int cond_801A0720 = (v0 == s5);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A0724: 0x00000000 */
    if (cond_801A0720) {
        psx_check_interrupts_at(cpu, 0x801A073Cu);
        goto block_801A073C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0728u);
    }

block_801A0728:
    if (psx_slice_block(cpu, 0x801A0728u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0728u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x801A0728: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A0734u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A0730u);
    psx_cyc_step(cpu, 0x10u);
    a0 += 24440;  /* 0x801A0730: 0x24845F78 */
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> printf_forward_guest (0x8018E550) */

block_801A0734:
    if (psx_slice_block(cpu, 0x801A0734u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0734u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000010u);
    a0 = sp + 16;  /* 0x801A0738: 0x27A40010 */
    psx_check_interrupts_at(cpu, 0x801A0750u);
    goto block_801A0750;  /* j */

block_801A073C:
    if (psx_slice_block(cpu, 0x801A073Cu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A073Cu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801A0744u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A0740u);
    psx_cyc_step(cpu, 0x10010u);
    a0 = (int32_t)s0 >> 4;  /* 0x801A0740: 0x00102103 */
    psx_check_interrupts_at(cpu, 0x801A09F0u);
    cpu->pc = 0x801A09F0u; return;  /* CPS jal -> cd_svc_5c (0x801A09F0) */

block_801A0744:
    if (psx_slice_block(cpu, 0x801A0744u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0744u);
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0001;  /* 0x801A0744: 0x30420001 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A0748 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000010u);
    a0 = sp + 16;  /* 0x801A074C: 0x27A40010 */
    if (cond_801A0748) {
        psx_check_interrupts_at(cpu, 0x801A073Cu);
        goto block_801A073C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0750u);
    }

block_801A0750:
    if (psx_slice_block(cpu, 0x801A0750u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0750u);
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x801A0750: 0x00002821 */
    psx_cyc_step(cpu, 0x9u);
    v1 = 126;  /* 0x801A0754: 0x2403007E */
    /* fall through to block_801A0758 */

block_801A0758:
    if (psx_slice_block(cpu, 0x801A0758u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0758u);
    v0 = psx_cyc_load_byte(cpu, a0, 2, 0x10u);  /* 0x801A0758: 0x90820000 */
    psx_cyc_step(cpu, 0x10u);
    a0++;  /* 0x801A075C: 0x24840001 */
    psx_icache_fetch(cpu, 0x801A0760u);
    psx_cyc_step(cpu, 0x8u);
    v1--;  /* 0x801A0760: 0x2463FFFF */
    psx_cyc_step(cpu, 0x8u);
    int cond_801A0764 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x24u);
    a1 ^= v0;  /* 0x801A0768: 0x00A22826 */
    if (cond_801A0764) {
        psx_check_interrupts_at(cpu, 0x801A0758u);
        goto block_801A0758;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A076Cu);
    }

block_801A076C:
    if (psx_slice_block(cpu, 0x801A076Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A076Cu);
    v1 = psx_cyc_load_byte(cpu, s3 + 127, 3, 0x80000u);  /* 0x801A076C: 0x9263007F */
    psx_icache_fetch(cpu, 0x801A0770u);
    psx_cyc_step(cpu, 0x24u);
    v0 = a1 & 0x00FF;  /* 0x801A0770: 0x30A200FF */
    psx_cyc_step(cpu, 0xCu);
    int cond_801A0774 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000u);
    s1++;  /* 0x801A0778: 0x26310001 */
    if (cond_801A0774) {
        psx_check_interrupts_at(cpu, 0x801A06C0u);
        goto block_801A06C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A077Cu);
    }

block_801A077C:
    if (psx_slice_block(cpu, 0x801A077Cu, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801A077Cu);
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x801A077C: 0x24020001 */
    /* fall through to block_801A0780 */

block_801A0780:
    if (psx_slice_block(cpu, 0x801A0780u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801A0780u);
    ra = psx_cyc_load_word(cpu, sp + 168, 31, 0x20000000u);  /* 0x801A0780: 0x8FBF00A8 */
    s5 = psx_cyc_load_word(cpu, sp + 164, 21, 0x20000000u);  /* 0x801A0784: 0x8FB500A4 */
    s4 = psx_cyc_load_word(cpu, sp + 160, 20, 0x20000000u);  /* 0x801A0788: 0x8FB400A0 */
    s3 = psx_cyc_load_word(cpu, sp + 156, 19, 0x20000000u);  /* 0x801A078C: 0x8FB3009C */
    psx_icache_fetch(cpu, 0x801A0790u);
    s2 = psx_cyc_load_word(cpu, sp + 152, 18, 0x20000000u);  /* 0x801A0790: 0x8FB20098 */
    s1 = psx_cyc_load_word(cpu, sp + 148, 17, 0x20000000u);  /* 0x801A0794: 0x8FB10094 */
    s0 = psx_cyc_load_word(cpu, sp + 144, 16, 0x20000000u);  /* 0x801A0798: 0x8FB00090 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_801A079C = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A07A0u);
    psx_cyc_step(cpu, 0x20000000u);
    sp += 176;  /* 0x801A07A0: 0x27BD00B0 */
    psx_check_interrupts_at(cpu, jt_801A079C);
    cpu->pc = jt_801A079C; return;  /* CPS: jr $ra */
}
/* ======== op_verts_a618 (guest 0x801AA618) ========
 * Piece of the unnamed vertex-op sequence function spanning
 * 0x801AA558-0x801AA968 (between tween_seq_step and data_relocate):
 * reads the s0+52/+56/+58 attribute tables and dispatches per-vertex
 * halfword fixups (MUL/DIV pipe timed via psx_muldiv_*). This port
 * covers the 0x801AA618-0x801AA7E7 segment (24 CPS blocks); it tail-
 * dispatches into op_verts_a7e8 exactly like the cpst_ split did. */

void op_verts_a618(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801AA628u: goto block_801AA628;
            case 0x801AA630u: goto block_801AA630;
            case 0x801AA644u: goto block_801AA644;
            case 0x801AA658u: goto block_801AA658;
            case 0x801AA65Cu: goto block_801AA65C;
            case 0x801AA664u: goto block_801AA664;
            case 0x801AA670u: goto block_801AA670;
            case 0x801AA674u: goto block_801AA674;
            case 0x801AA678u: goto block_801AA678;
            case 0x801AA740u: goto block_801AA740;
            case 0x801AA750u: goto block_801AA750;
            case 0x801AA768u: goto block_801AA768;
            case 0x801AA76Cu: goto block_801AA76C;
            case 0x801AA778u: goto block_801AA778;
            case 0x801AA780u: goto block_801AA780;
            case 0x801AA784u: goto block_801AA784;
            case 0x801AA790u: goto block_801AA790;
            case 0x801AA7A8u: goto block_801AA7A8;
            case 0x801AA7ACu: goto block_801AA7AC;
            case 0x801AA7B8u: goto block_801AA7B8;
            case 0x801AA7C0u: goto block_801AA7C0;
            case 0x801AA7C4u: goto block_801AA7C4;
            case 0x801AA7D0u: goto block_801AA7D0;
            default: break;
        }
    }
    /* Address: 0x801AA618, Size: 464 bytes, Blocks: 24 */

block_801AA618:
    if (psx_slice_block(cpu, 0x801AA618u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA618u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 68, 2, 0x10000u);  /* 0x801AA618: 0x86020044 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA61C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AA620u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801AA620 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA624: 0x00000000 */
    if (cond_801AA620) {
        psx_check_interrupts_at(cpu, 0x801AA658u);
        goto block_801AA658;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA628u);
    }

block_801AA628:
    if (psx_slice_block(cpu, 0x801AA628u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA628u);
    psx_cyc_step(cpu, 0x84u);
    int cond_801AA628 = (v0 != a3);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 << 24;  /* 0x801AA62C: 0x00041E00 */
    if (cond_801AA628) {
        psx_check_interrupts_at(cpu, 0x801AA678u);
        goto block_801AA678;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA630u);
    }

block_801AA630:
    if (psx_slice_block(cpu, 0x801AA630u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA630u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 24;  /* 0x801AA630: 0x00041600 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 70, 3, 0x10000u);  /* 0x801AA634: 0x86030046 */
    psx_cyc_step(cpu, 0x4u);
    v0 = (int32_t)v0 >> 24;  /* 0x801AA638: 0x00021603 */
    psx_cyc_step(cpu, 0xCu);
    int cond_801AA63C = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AA640u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 24;  /* 0x801AA640: 0x00041600 */
    if (cond_801AA63C) {
        psx_check_interrupts_at(cpu, 0x801AA65Cu);
        goto block_801AA65C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA644u);
    }

block_801AA644:
    if (psx_slice_block(cpu, 0x801AA644u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA644u);
    v0 = psx_cyc_load_half(cpu, s0 + 72, 2, 0x10000u);  /* 0x801AA644: 0x96020048 */
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_half(s0 + 58, (uint16_t)zero);  /* 0x801AA648: 0xA600003A */
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_half(s0 + 68, (uint16_t)zero);  /* 0x801AA64C: 0xA6000044 */
    psx_icache_fetch(cpu, 0x801AA650u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 56, (uint16_t)v0);  /* 0x801AA654: 0xA6020038 */
    psx_check_interrupts_at(cpu, 0x801AA55Cu);
    cpu->pc = 0x801AA55Cu; return;  /* CPS j: split -> cpst_801aa55c */

block_801AA658:
    if (psx_slice_block(cpu, 0x801AA658u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA658u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 << 24;  /* 0x801AA658: 0x00041600 */
    /* fall through to block_801AA65C */

block_801AA65C:
    if (psx_slice_block(cpu, 0x801AA65Cu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA65Cu);
    psx_cyc_step(cpu, 0x4u);
    int cond_801AA65C = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AA660u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA660: 0x00000000 */
    if (cond_801AA65C) {
        psx_check_interrupts_at(cpu, 0x801AA670u);
        goto block_801AA670;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA664u);
    }

block_801AA664:
    if (psx_slice_block(cpu, 0x801AA664u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA664u);
    psx_cyc_step(cpu, 0x10020u);
    cpu->write_half(s0 + 62, (uint16_t)a1);  /* 0x801AA664: 0xA605003E */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = zero - a0;  /* 0x801AA66C: 0x00042023 */
    psx_check_interrupts_at(cpu, 0x801AA674u);
    goto block_801AA674;  /* j */

block_801AA670:
    if (psx_slice_block(cpu, 0x801AA670u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA670u);
    psx_cyc_step(cpu, 0x10080u);
    cpu->write_half(s0 + 62, (uint16_t)a3);  /* 0x801AA670: 0xA607003E */
    /* fall through to block_801AA674 */

block_801AA674:
    if (psx_slice_block(cpu, 0x801AA674u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA674u);
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 << 24;  /* 0x801AA674: 0x00041E00 */
    /* fall through to block_801AA678 */

block_801AA678:
    if (psx_slice_block(cpu, 0x801AA678u, 50u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA678u);
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v1 >> 24;  /* 0x801AA678: 0x00031603 */
    psx_cyc_step(cpu, 0xDu);
    v1 = v0;  /* 0x801AA67C: 0x00401821 */
    psx_icache_fetch(cpu, 0x801AA680u);
    psx_cyc_step(cpu, 0x8u);
    v1--;  /* 0x801AA680: 0x2463FFFF */
    psx_cyc_step(cpu, 0x10008u);
    cpu->write_half(s0 + 14, (uint16_t)v1);  /* 0x801AA684: 0xA603000E */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 16;  /* 0x801AA688: 0x00031C00 */
    psx_cyc_step(cpu, 0x8u);
    v1 = (int32_t)v1 >> 16;  /* 0x801AA68C: 0x00031C03 */
    psx_icache_fetch(cpu, 0x801AA690u);
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 60, (uint16_t)v0);  /* 0x801AA690: 0xA602003C */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801AA694: 0x00031080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA698: 0x00431021 */
    v1 = psx_cyc_load_word(cpu, s0 + 24, 3, 0x10000u);  /* 0x801AA69C: 0x8E030018 */
    psx_icache_fetch(cpu, 0x801AA6A0u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801AA6A0: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA6A4: 0x00431021 */
    v0 = psx_cyc_load_half(cpu, v0 + 4, 2, 0x4u);  /* 0x801AA6A8: 0x94420004 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 14, 3, 0x10000u);  /* 0x801AA6AC: 0x8603000E */
    psx_icache_fetch(cpu, 0x801AA6B0u);
    psx_cyc_step(cpu, 0x4u);
    v0--;  /* 0x801AA6B0: 0x2442FFFF */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 10, (uint16_t)v0);  /* 0x801AA6B4: 0xA602000A */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801AA6B8: 0x00031080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA6BC: 0x00431021 */
    psx_icache_fetch(cpu, 0x801AA6C0u);
    v1 = psx_cyc_load_word(cpu, s0 + 24, 3, 0x10000u);  /* 0x801AA6C0: 0x8E030018 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801AA6C4: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA6C8: 0x00431021 */
    v0 = psx_cyc_load_word(cpu, v0 + 16, 2, 0x4u);  /* 0x801AA6CC: 0x8C420010 */
    psx_icache_fetch(cpu, 0x801AA6D0u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 14, 3, 0x10000u);  /* 0x801AA6D0: 0x8603000E */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 20, v0);  /* 0x801AA6D4: 0xAE020014 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801AA6D8: 0x00031080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA6DC: 0x00431021 */
    psx_icache_fetch(cpu, 0x801AA6E0u);
    v1 = psx_cyc_load_word(cpu, s0 + 24, 3, 0x10000u);  /* 0x801AA6E0: 0x8E030018 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801AA6E4: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA6E8: 0x00431021 */
    v0 = psx_cyc_load_half(cpu, v0, 2, 0x4u);  /* 0x801AA6EC: 0x94420000 */
    psx_icache_fetch(cpu, 0x801AA6F0u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 14, 3, 0x10000u);  /* 0x801AA6F0: 0x8603000E */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 16, (uint16_t)v0);  /* 0x801AA6F4: 0xA6020010 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801AA6F8: 0x00031080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA6FC: 0x00431021 */
    psx_icache_fetch(cpu, 0x801AA700u);
    v1 = psx_cyc_load_word(cpu, s0 + 24, 3, 0x10000u);  /* 0x801AA700: 0x8E030018 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801AA704: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA708: 0x00431021 */
    v0 = psx_cyc_load_word(cpu, v0 + 8, 2, 0x4u);  /* 0x801AA70C: 0x8C420008 */
    psx_icache_fetch(cpu, 0x801AA710u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 14, 3, 0x10000u);  /* 0x801AA710: 0x8603000E */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 36, v0);  /* 0x801AA714: 0xAE020024 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801AA718: 0x00031080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA71C: 0x00431021 */
    psx_icache_fetch(cpu, 0x801AA720u);
    v1 = psx_cyc_load_word(cpu, s0 + 24, 3, 0x10000u);  /* 0x801AA720: 0x8E030018 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801AA724: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA728: 0x00431021 */
    v0 = psx_cyc_load_word(cpu, v0 + 12, 2, 0x4u);  /* 0x801AA72C: 0x8C42000C */
    psx_icache_fetch(cpu, 0x801AA730u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 2, 3, 0x10000u);  /* 0x801AA730: 0x86030002 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA734: 0x00000000 */
    psx_cyc_step(cpu, 0x88u);
    int cond_801AA738 = (v1 != a3);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 40, v0);  /* 0x801AA73C: 0xAE020028 */
    if (cond_801AA738) {
        psx_check_interrupts_at(cpu, 0x801AA7E8u);
        cpu->pc = 0x801AA7E8u; return;  /* CPS taken: split -> cpst_801aa7e8 */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA740u);
    }

block_801AA740:
    if (psx_slice_block(cpu, 0x801AA740u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA740u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 62, 2, 0x10000u);  /* 0x801AA740: 0x8602003E */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA744: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_801AA748 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA74C: 0x00000000 */
    if (cond_801AA748) {
        psx_check_interrupts_at(cpu, 0x801AA790u);
        goto block_801AA790;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA750u);
    }

block_801AA750:
    if (psx_slice_block(cpu, 0x801AA750u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA750u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 10, 2, 0x10000u);  /* 0x801AA750: 0x8602000A */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA754: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x801AA758: 0x24420001 */
    psx_cyc_step(cpu, 0x44u);
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)a2 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)a2 % (int32_t)v0); } else { cpu->lo = ((int32_t)a2 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)a2; }
    psx_muldiv_set(cpu, 37u);  /* 0x801AA75C: 0x00C2001A */
    psx_icache_fetch(cpu, 0x801AA760u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801AA760 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA764: 0x00000000 */
    if (cond_801AA760) {
        psx_check_interrupts_at(cpu, 0x801AA76Cu);
        goto block_801AA76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA768u);
    }

block_801AA768:
    if (psx_slice_block(cpu, 0x801AA768u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA768u);
    psx_cyc_step(cpu, 0x0u);
    /* break(7168) — trap, no-op in recompiler */  /* 0x801AA768: 0x0007000D */
    /* fall through to block_801AA76C */

block_801AA76C:
    if (psx_slice_block(cpu, 0x801AA76Cu, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA76Cu);
    psx_cyc_step(cpu, 0x3u);
    at = -1;  /* 0x801AA76C: 0x2401FFFF */
    psx_icache_fetch(cpu, 0x801AA770u);
    psx_cyc_step(cpu, 0x6u);
    int cond_801AA770 = (v0 != at);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x2u);
    at = 0x80000000u;  /* 0x80000000 */  /* 0x801AA774: 0x3C018000 */
    if (cond_801AA770) {
        psx_check_interrupts_at(cpu, 0x801AA784u);
        goto block_801AA784;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA778u);
    }

block_801AA778:
    if (psx_slice_block(cpu, 0x801AA778u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA778u);
    psx_cyc_step(cpu, 0x42u);
    int cond_801AA778 = (a2 != at);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA77C: 0x00000000 */
    if (cond_801AA778) {
        psx_check_interrupts_at(cpu, 0x801AA784u);
        goto block_801AA784;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA780u);
    }

block_801AA780:
    if (psx_slice_block(cpu, 0x801AA780u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA780u);
    psx_cyc_step(cpu, 0x0u);
    /* break(6144) — trap, no-op in recompiler */  /* 0x801AA780: 0x0006000D */
    /* fall through to block_801AA784 */

block_801AA784:
    if (psx_slice_block(cpu, 0x801AA784u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA784u);
    psx_cyc_step(cpu, 0x4u);
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x801AA784: 0x00001012 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_half(s0 + 64, (uint16_t)zero);  /* 0x801AA78C: 0xA6000040 */
    psx_check_interrupts_at(cpu, 0x801AA7D0u);
    goto block_801AA7D0;  /* j */

block_801AA790:
    if (psx_slice_block(cpu, 0x801AA790u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA790u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 10, 2, 0x10000u);  /* 0x801AA790: 0x8602000A */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA794: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x801AA798: 0x24420001 */
    psx_cyc_step(cpu, 0x44u);
    if (v0 != 0) { cpu->lo = (uint32_t)((int32_t)a2 / (int32_t)v0); cpu->hi = (uint32_t)((int32_t)a2 % (int32_t)v0); } else { cpu->lo = ((int32_t)a2 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)a2; }
    psx_muldiv_set(cpu, 37u);  /* 0x801AA79C: 0x00C2001A */
    psx_icache_fetch(cpu, 0x801AA7A0u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801AA7A0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA7A4: 0x00000000 */
    if (cond_801AA7A0) {
        psx_check_interrupts_at(cpu, 0x801AA7ACu);
        goto block_801AA7AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA7A8u);
    }

block_801AA7A8:
    if (psx_slice_block(cpu, 0x801AA7A8u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7A8u);
    psx_cyc_step(cpu, 0x0u);
    /* break(7168) — trap, no-op in recompiler */  /* 0x801AA7A8: 0x0007000D */
    /* fall through to block_801AA7AC */

block_801AA7AC:
    if (psx_slice_block(cpu, 0x801AA7ACu, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7ACu);
    psx_cyc_step(cpu, 0x3u);
    at = -1;  /* 0x801AA7AC: 0x2401FFFF */
    psx_icache_fetch(cpu, 0x801AA7B0u);
    psx_cyc_step(cpu, 0x6u);
    int cond_801AA7B0 = (v0 != at);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x2u);
    at = 0x80000000u;  /* 0x80000000 */  /* 0x801AA7B4: 0x3C018000 */
    if (cond_801AA7B0) {
        psx_check_interrupts_at(cpu, 0x801AA7C4u);
        goto block_801AA7C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA7B8u);
    }

block_801AA7B8:
    if (psx_slice_block(cpu, 0x801AA7B8u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7B8u);
    psx_cyc_step(cpu, 0x42u);
    int cond_801AA7B8 = (a2 != at);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA7BC: 0x00000000 */
    if (cond_801AA7B8) {
        psx_check_interrupts_at(cpu, 0x801AA7C4u);
        goto block_801AA7C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA7C0u);
    }

block_801AA7C0:
    if (psx_slice_block(cpu, 0x801AA7C0u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7C0u);
    psx_cyc_step(cpu, 0x0u);
    /* break(6144) — trap, no-op in recompiler */  /* 0x801AA7C0: 0x0006000D */
    /* fall through to block_801AA7C4 */

block_801AA7C4:
    if (psx_slice_block(cpu, 0x801AA7C4u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7C4u);
    psx_cyc_step(cpu, 0x4u);
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x801AA7C4: 0x00001012 */
    psx_cyc_step(cpu, 0x10040u);
    cpu->write_half(s0 + 64, (uint16_t)a2);  /* 0x801AA7C8: 0xA6060040 */
    psx_cyc_step(cpu, 0x5u);
    v0 = zero - v0;  /* 0x801AA7CC: 0x00021023 */
    /* fall through to block_801AA7D0 */

block_801AA7D0:
    if (psx_slice_block(cpu, 0x801AA7D0u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7D0u);
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 66, (uint16_t)v0);  /* 0x801AA7D0: 0xA6020042 */
    v0 = psx_cyc_load_half(cpu, s0 + 64, 2, 0x10000u);  /* 0x801AA7D4: 0x96020040 */
    v1 = psx_cyc_load_half(cpu, s0 + 66, 3, 0x10000u);  /* 0x801AA7D8: 0x96030042 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA7DC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AA7E0u);
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA7E0: 0x00431021 */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 64, (uint16_t)v0);  /* 0x801AA7E4: 0xA6020040 */
    cpu->pc = 0x801aa7e8u; return;  /* fallthrough to split piece -> cpst_801aa7e8 */
}
/* ======== op_verts_a7e8 (guest 0x801AA7E8) ========
 * Second piece of the vertex-op sequence function
 * 0x801AA558-0x801AA968: applies verts_offset_apply / verts_rgb_apply /
 * verts_add_apply / verts_sub_apply per vertex record (21 CPS blocks).
 * Tail-dispatches into cpst_801aa95c (unported piece — stays
 * cpst_-authoritative via clean_cps.map). */

void op_verts_a7e8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801AA820u: goto block_801AA820;
            case 0x801AA840u: goto block_801AA840;
            case 0x801AA8C4u: goto block_801AA8C4;
            case 0x801AA938u: goto block_801AA938;
            case 0x801AA7F8u: goto block_801AA7F8;
            case 0x801AA808u: goto block_801AA808;
            case 0x801AA828u: goto block_801AA828;
            case 0x801AA848u: goto block_801AA848;
            case 0x801AA858u: goto block_801AA858;
            case 0x801AA868u: goto block_801AA868;
            case 0x801AA870u: goto block_801AA870;
            case 0x801AA874u: goto block_801AA874;
            case 0x801AA884u: goto block_801AA884;
            case 0x801AA88Cu: goto block_801AA88C;
            case 0x801AA894u: goto block_801AA894;
            case 0x801AA8CCu: goto block_801AA8CC;
            case 0x801AA8FCu: goto block_801AA8FC;
            case 0x801AA910u: goto block_801AA910;
            case 0x801AA914u: goto block_801AA914;
            case 0x801AA94Cu: goto block_801AA94C;
            default: break;
        }
    }
    /* Address: 0x801AA7E8, Size: 372 bytes, Blocks: 21 */

block_801AA7E8:
    if (psx_slice_block(cpu, 0x801AA7E8u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7E8u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 2, 2, 0x10000u);  /* 0x801AA7E8: 0x86020002 */
    psx_cyc_step(cpu, 0x9u);
    v1 = 2;  /* 0x801AA7EC: 0x24030002 */
    psx_icache_fetch(cpu, 0x801AA7F0u);
    psx_cyc_step(cpu, 0xCu);
    int cond_801AA7F0 = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA7F4: 0x00000000 */
    if (cond_801AA7F0) {
        psx_check_interrupts_at(cpu, 0x801AA848u);
        goto block_801AA848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA7F8u);
    }

block_801AA7F8:
    if (psx_slice_block(cpu, 0x801AA7F8u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA7F8u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 62, 2, 0x10000u);  /* 0x801AA7F8: 0x8602003E */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA7FC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AA800u);
    psx_cyc_step(cpu, 0x4u);
    int cond_801AA800 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA804: 0x00000000 */
    if (cond_801AA800) {
        psx_check_interrupts_at(cpu, 0x801AA828u);
        goto block_801AA828;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA808u);
    }

block_801AA808:
    if (psx_slice_block(cpu, 0x801AA808u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA808u);
    a0 = psx_cyc_load_word(cpu, s0 + 28, 4, 0x10000u);  /* 0x801AA808: 0x8E04001C */
    a1 = psx_cyc_load_word(cpu, s0 + 36, 5, 0x10000u);  /* 0x801AA80C: 0x8E050024 */
    psx_icache_fetch(cpu, 0x801AA810u);
    a2 = psx_cyc_load_word(cpu, s0 + 20, 6, 0x10000u);  /* 0x801AA810: 0x8E060014 */
    a3 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 16, 7, 0x10000u);  /* 0x801AA814: 0x86070010 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801AA820u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA81C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AD11Cu);
    cpu->pc = 0x801AD11Cu; return;  /* CPS jal -> verts_add_apply (0x801AD11C) */

block_801AA820:
    if (psx_slice_block(cpu, 0x801AA820u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA820u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA824: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AA94Cu);
    goto block_801AA94C;  /* j */

block_801AA828:
    if (psx_slice_block(cpu, 0x801AA828u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA828u);
    a0 = psx_cyc_load_word(cpu, s0 + 28, 4, 0x10000u);  /* 0x801AA828: 0x8E04001C */
    a1 = psx_cyc_load_word(cpu, s0 + 36, 5, 0x10000u);  /* 0x801AA82C: 0x8E050024 */
    psx_icache_fetch(cpu, 0x801AA830u);
    a2 = psx_cyc_load_word(cpu, s0 + 20, 6, 0x10000u);  /* 0x801AA830: 0x8E060014 */
    a3 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 16, 7, 0x10000u);  /* 0x801AA834: 0x86070010 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801AA840u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA83C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AD180u);
    cpu->pc = 0x801AD180u; return;  /* CPS jal -> verts_sub_apply (0x801AD180) */

block_801AA840:
    if (psx_slice_block(cpu, 0x801AA840u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA840u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA844: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AA94Cu);
    goto block_801AA94C;  /* j */

block_801AA848:
    if (psx_slice_block(cpu, 0x801AA848u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA848u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 64, 2, 0x10000u);  /* 0x801AA848: 0x86020040 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA84C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AA850u);
    psx_cyc_step(cpu, 0x4u);
    int cond_801AA850 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x4u);
    v0 = (int32_t)v0 < 4097;  /* 0x801AA854: 0x28421001 */
    if (cond_801AA850) {
        psx_check_interrupts_at(cpu, 0x801AA868u);
        goto block_801AA868;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA858u);
    }

block_801AA858:
    if (psx_slice_block(cpu, 0x801AA858u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA858u);
    psx_cyc_step(cpu, 0x10001u);
    cpu->write_half(s0 + 64, (uint16_t)zero);  /* 0x801AA858: 0xA6000040 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 64, 2, 0x10000u);  /* 0x801AA85C: 0x86020040 */
    psx_icache_fetch(cpu, 0x801AA860u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA860: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 = (int32_t)v0 < 4097;  /* 0x801AA864: 0x28421001 */
    /* fall through to block_801AA868 */

block_801AA868:
    if (psx_slice_block(cpu, 0x801AA868u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA868u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801AA868 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 4096;  /* 0x801AA86C: 0x24021000 */
    if (cond_801AA868) {
        psx_check_interrupts_at(cpu, 0x801AA874u);
        goto block_801AA874;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA870u);
    }

block_801AA870:
    if (psx_slice_block(cpu, 0x801AA870u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA870u);
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 64, (uint16_t)v0);  /* 0x801AA870: 0xA6020040 */
    /* fall through to block_801AA874 */

block_801AA874:
    if (psx_slice_block(cpu, 0x801AA874u, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA874u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0, 2, 0x10000u);  /* 0x801AA874: 0x86020000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA878: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801AA87C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AA880u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA880: 0x00000000 */
    if (cond_801AA87C) {
        psx_check_interrupts_at(cpu, 0x801AA894u);
        goto block_801AA894;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA884u);
    }

block_801AA884:
    if (psx_slice_block(cpu, 0x801AA884u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA884u);
    psx_cyc_step(cpu, 0xCu);
    int cond_801AA884 = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA888: 0x00000000 */
    if (cond_801AA884) {
        psx_check_interrupts_at(cpu, 0x801AA8CCu);
        goto block_801AA8CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA88Cu);
    }

block_801AA88C:
    if (psx_slice_block(cpu, 0x801AA88Cu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA88Cu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AA890u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA890: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AA938u);
    goto block_801AA938;  /* j */

block_801AA894:
    if (psx_slice_block(cpu, 0x801AA894u, 12u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA894u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 16, 2, 0x10000u);  /* 0x801AA894: 0x86020010 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA898: 0x00000000 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 16, v0);  /* 0x801AA89C: 0xAFA20010 */
    psx_icache_fetch(cpu, 0x801AA8A0u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 64, 2, 0x10000u);  /* 0x801AA8A0: 0x86020040 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA8A4: 0x00000000 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 20, v0);  /* 0x801AA8A8: 0xAFA20014 */
    a0 = psx_cyc_load_word(cpu, s0 + 36, 4, 0x10000u);  /* 0x801AA8AC: 0x8E040024 */
    psx_icache_fetch(cpu, 0x801AA8B0u);
    a1 = psx_cyc_load_word(cpu, s0 + 40, 5, 0x10000u);  /* 0x801AA8B0: 0x8E050028 */
    a2 = psx_cyc_load_word(cpu, s0 + 28, 6, 0x10000u);  /* 0x801AA8B4: 0x8E06001C */
    a3 = psx_cyc_load_word(cpu, s0 + 20, 7, 0x10000u);  /* 0x801AA8B8: 0x8E070014 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801AA8C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AA8C0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA8C0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801ACFE4u);
    cpu->pc = 0x801ACFE4u; return;  /* CPS jal -> verts_offset_apply (0x801ACFE4) */

block_801AA8C4:
    if (psx_slice_block(cpu, 0x801AA8C4u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA8C4u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA8C8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AA938u);
    goto block_801AA938;  /* j */

block_801AA8CC:
    if (psx_slice_block(cpu, 0x801AA8CCu, 12u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA8CCu);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 14, 3, 0x10000u);  /* 0x801AA8CC: 0x8603000E */
    psx_icache_fetch(cpu, 0x801AA8D0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA8D0: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801AA8D4: 0x00031080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA8D8: 0x00431021 */
    v1 = psx_cyc_load_word(cpu, s0 + 24, 3, 0x10000u);  /* 0x801AA8DC: 0x8E030018 */
    psx_icache_fetch(cpu, 0x801AA8E0u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801AA8E0: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA8E4: 0x00431021 */
    a0 = psx_cyc_load_word(cpu, v0 + 8, 4, 0x4u);  /* 0x801AA8E8: 0x8C440008 */
    v1 = psx_cyc_load_word(cpu, gp + 36, 3, 0x10000000u);  /* 0x801AA8EC: 0x8F830024 */
    psx_icache_fetch(cpu, 0x801AA8F0u);
    a1 = psx_cyc_load_word(cpu, v0 + 12, 5, 0x4u);  /* 0x801AA8F0: 0x8C45000C */
    psx_cyc_step(cpu, 0x9u);
    int cond_801AA8F4 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA8F8: 0x00000000 */
    if (cond_801AA8F4) {
        psx_check_interrupts_at(cpu, 0x801AA910u);
        goto block_801AA910;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AA8FCu);
    }

block_801AA8FC:
    if (psx_slice_block(cpu, 0x801AA8FCu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA8FCu);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 8, 2, 0x10000u);  /* 0x801AA8FC: 0x86020008 */
    psx_icache_fetch(cpu, 0x801AA900u);
    v1 = psx_cyc_load_word(cpu, s0 + 32, 3, 0x10000u);  /* 0x801AA900: 0x8E030020 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801AA904: 0x00021080 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x4Cu);
    a2 = v1 + v0;  /* 0x801AA90C: 0x00623021 */
    psx_check_interrupts_at(cpu, 0x801AA914u);
    goto block_801AA914;  /* j */

block_801AA910:
    if (psx_slice_block(cpu, 0x801AA910u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA910u);
    a2 = psx_cyc_load_word(cpu, s0 + 32, 6, 0x10000u);  /* 0x801AA910: 0x8E060020 */
    /* fall through to block_801AA914 */

block_801AA914:
    if (psx_slice_block(cpu, 0x801AA914u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA914u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 16, 2, 0x10000u);  /* 0x801AA914: 0x86020010 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA918: 0x00000000 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 16, v0);  /* 0x801AA91C: 0xAFA20010 */
    psx_icache_fetch(cpu, 0x801AA920u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s0 + 64, 2, 0x10000u);  /* 0x801AA920: 0x86020040 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA924: 0x00000000 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_word(sp + 20, v0);  /* 0x801AA928: 0xAFA20014 */
    a3 = psx_cyc_load_word(cpu, s0 + 20, 7, 0x10000u);  /* 0x801AA92C: 0x8E070014 */
    psx_icache_fetch(cpu, 0x801AA930u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801AA938u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA934: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AD070u);
    cpu->pc = 0x801AD070u; return;  /* CPS jal -> verts_rgb_apply (0x801AD070) */

block_801AA938:
    if (psx_slice_block(cpu, 0x801AA938u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA938u);
    v0 = psx_cyc_load_half(cpu, s0 + 64, 2, 0x10000u);  /* 0x801AA938: 0x96020040 */
    v1 = psx_cyc_load_half(cpu, s0 + 66, 3, 0x10000u);  /* 0x801AA93C: 0x96030042 */
    psx_icache_fetch(cpu, 0x801AA940u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA940: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AA944: 0x00431021 */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 64, (uint16_t)v0);  /* 0x801AA948: 0xA6020040 */
    /* fall through to block_801AA94C */

block_801AA94C:
    if (psx_slice_block(cpu, 0x801AA94Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801AA94Cu);
    v0 = psx_cyc_load_half(cpu, s0 + 10, 2, 0x10000u);  /* 0x801AA94C: 0x9602000A */
    psx_icache_fetch(cpu, 0x801AA950u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AA950: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0--;  /* 0x801AA954: 0x2442FFFF */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_half(s0 + 10, (uint16_t)v0);  /* 0x801AA958: 0xA602000A */
    cpu->pc = 0x801aa95cu; return;  /* fallthrough to split piece -> cpst_801aa95c */
}
/* ======== prim_emit_xform_a (guest 0x801A8EE0) ========
 * GTE transformed-vertex emitter, variant A (21 CPS blocks / 768
 * bytes): iterates the t2+48/50/52 vertex counts, runs the GTE pipe
 * (psx_gte_stall / gte_execute / gte_read_data / gte_write_data /
 * psx_cyc_lwc2_read) and stores RTPS results into the vertex buffers.
 * Sibling of prim_emit_xform_b (0x801A91E0) — near-identical body with
 * extra precision-store hooks. Timing-faithful port of cpst_801a8ee0. */

void prim_emit_xform_a(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A8F04u: goto block_801A8F04;
            case 0x801A8F28u: goto block_801A8F28;
            case 0x801A8F2Cu: goto block_801A8F2C;
            case 0x801A8F3Cu: goto block_801A8F3C;
            case 0x801A8F50u: goto block_801A8F50;
            case 0x801A8F6Cu: goto block_801A8F6C;
            case 0x801A8F7Cu: goto block_801A8F7C;
            case 0x801A900Cu: goto block_801A900C;
            case 0x801A9074u: goto block_801A9074;
            case 0x801A9088u: goto block_801A9088;
            case 0x801A909Cu: goto block_801A909C;
            case 0x801A90ACu: goto block_801A90AC;
            case 0x801A90C0u: goto block_801A90C0;
            case 0x801A90D4u: goto block_801A90D4;
            case 0x801A9184u: goto block_801A9184;
            case 0x801A918Cu: goto block_801A918C;
            case 0x801A9194u: goto block_801A9194;
            case 0x801A9198u: goto block_801A9198;
            case 0x801A919Cu: goto block_801A919C;
            case 0x801A91CCu: goto block_801A91CC;
            default: break;
        }
    }
    /* Address: 0x801A8EE0, Size: 768 bytes, Blocks: 21 */

block_801A8EE0:
    if (psx_slice_block(cpu, 0x801A8EE0u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8EE0u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 24;  /* 0x801A8EE0: 0x27BDFFE8 */
    psx_cyc_step(cpu, 0x411u);
    t2 = a0;  /* 0x801A8EE4: 0x00805021 */
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 16, s2);  /* 0x801A8EE8: 0xAFB20010 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 12, s1);  /* 0x801A8EEC: 0xAFB1000C */
    psx_icache_fetch(cpu, 0x801A8EF0u);
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 8, s0);  /* 0x801A8EF0: 0xAFB00008 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 52, 2, 0x400u);  /* 0x801A8EF4: 0x85420034 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8EF8: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A8EFC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A8F00u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8F00: 0x00000000 */
    if (cond_801A8EFC) {
        psx_check_interrupts_at(cpu, 0x801A91CCu);
        goto block_801A91CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A8F04u);
    }

block_801A8F04:
    if (psx_slice_block(cpu, 0x801A8F04u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8F04u);
    t9 = psx_cyc_load_word(cpu, t2 + 24, 25, 0x400u);  /* 0x801A8F04: 0x8D590018 */
    v0 = psx_cyc_load_word(cpu, gp + 36, 2, 0x10000000u);  /* 0x801A8F08: 0x8F820024 */
    s0 = psx_cyc_load_word(cpu, t2 + 36, 16, 0x400u);  /* 0x801A8F0C: 0x8D500024 */
    psx_icache_fetch(cpu, 0x801A8F10u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 50, 3, 0x400u);  /* 0x801A8F10: 0x85430032 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801A8F14: 0x00021080 */
    psx_cyc_step(cpu, 0x404u);
    v0 = t2 + v0;  /* 0x801A8F18: 0x01421021 */
    t1 = psx_cyc_load_word(cpu, v0, 9, 0x4u);  /* 0x801A8F1C: 0x8C490000 */
    psx_icache_fetch(cpu, 0x801A8F20u);
    psx_cyc_step(cpu, 0x9u);
    int cond_801A8F20 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8F24: 0x00000000 */
    if (cond_801A8F20) {
        psx_check_interrupts_at(cpu, 0x801A8F2Cu);
        goto block_801A8F2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A8F28u);
    }

block_801A8F28:
    if (psx_slice_block(cpu, 0x801A8F28u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8F28u);
    psx_cyc_step(cpu, 0x20000008u);
    cpu->write_word(sp + 4, v1);  /* 0x801A8F28: 0xAFA30004 */
    /* fall through to block_801A8F2C */

block_801A8F2C:
    if (psx_slice_block(cpu, 0x801A8F2Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8F2Cu);
    v0 = psx_cyc_load_word(cpu, t2 + 28, 2, 0x400u);  /* 0x801A8F2C: 0x8D42001C */
    psx_icache_fetch(cpu, 0x801A8F30u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8F30: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A8F34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8F38: 0x00000000 */
    if (cond_801A8F34) {
        psx_check_interrupts_at(cpu, 0x801A91CCu);
        goto block_801A91CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A8F3Cu);
    }

block_801A8F3C:
    if (psx_slice_block(cpu, 0x801A8F3Cu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8F3Cu);
    t0 = psx_cyc_load_word(cpu, t2 + 20, 8, 0x400u);  /* 0x801A8F3C: 0x8D480014 */
    psx_icache_fetch(cpu, 0x801A8F40u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 48, 2, 0x400u);  /* 0x801A8F40: 0x85420030 */
    a3 = psx_cyc_load_word(cpu, t2 + 32, 7, 0x400u);  /* 0x801A8F44: 0x8D470020 */
    psx_cyc_step(cpu, 0x4u);
    int cond_801A8F48 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20001u);
    s1 = zero;  /* 0x801A8F4C: 0x00008821 */
    if (cond_801A8F48) {
        psx_check_interrupts_at(cpu, 0x801A91CCu);
        goto block_801A91CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A8F50u);
    }

block_801A8F50:
    if (psx_slice_block(cpu, 0x801A8F50u, 7u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8F50u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x801A8F50: 0x3C028012 */
    psx_cyc_step(cpu, 0x40004u);
    s2 = v0 + 24832;  /* 0x801A8F54: 0x24526100 */
    psx_cyc_step(cpu, 0x2000800u);
    t3 = t9 + 4;  /* 0x801A8F58: 0x272B0004 */
    psx_cyc_step(cpu, 0x1000200u);
    t8 = t1 + 7;  /* 0x801A8F5C: 0x25380007 */
    psx_icache_fetch(cpu, 0x801A8F60u);
    psx_cyc_step(cpu, 0x8200u);
    t7 = t1 + 32;  /* 0x801A8F60: 0x252F0020 */
    psx_cyc_step(cpu, 0x4200u);
    t6 = t1 + 20;  /* 0x801A8F64: 0x252E0014 */
    psx_cyc_step(cpu, 0x2200u);
    t5 = t1 + 8;  /* 0x801A8F68: 0x252D0008 */
    /* fall through to block_801A8F6C */

block_801A8F6C:
    if (psx_slice_block(cpu, 0x801A8F6Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8F6Cu);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 50, 2, 0x400u);  /* 0x801A8F6C: 0x85420032 */
    psx_icache_fetch(cpu, 0x801A8F70u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8F70: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A8F74 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8F78: 0x00000000 */
    if (cond_801A8F74) {
        psx_check_interrupts_at(cpu, 0x801A900Cu);
        goto block_801A900C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A8F7Cu);
    }

block_801A8F7C:
    if (psx_slice_block(cpu, 0x801A8F7Cu, 36u, 0)) return;
    psx_icache_fetch(cpu, 0x801A8F7Cu);
    a1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t9, 5, 0x2000000u);  /* 0x801A8F7C: 0x87250000 */
    psx_icache_fetch(cpu, 0x801A8F80u);
    a0 = psx_cyc_load_word(cpu, t2 + 16, 4, 0x400u);  /* 0x801A8F80: 0x8D440010 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3 + -2, 3, 0x800u);  /* 0x801A8F84: 0x8563FFFE */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3, 2, 0x800u);  /* 0x801A8F88: 0x85620000 */
    psx_cyc_step(cpu, 0x20u);
    a1 <<= 3;  /* 0x801A8F8C: 0x000528C0 */
    psx_icache_fetch(cpu, 0x801A8F90u);
    psx_cyc_step(cpu, 0x30u);
    a1 = a0 + a1;  /* 0x801A8F90: 0x00852821 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 3;  /* 0x801A8F94: 0x000318C0 */
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 + v1;  /* 0x801A8F98: 0x00831821 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A8F9C: 0x000210C0 */
    psx_icache_fetch(cpu, 0x801A8FA0u);
    psx_cyc_step(cpu, 0x14u);
    a0 += v0;  /* 0x801A8FA0: 0x00822021 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (a1)));  /* 0x801A8FA4: 0xC8A00000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (a1 + 4))));  /* 0x801A8FA8: 0xC8A10004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A8FAC: 0xC8620000 */
    psx_icache_fetch(cpu, 0x801A8FB0u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A8FB0: 0xC8630004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (psx_cyc_lwc2_read(cpu, (a0)));  /* 0x801A8FB4: 0xC8840000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (psx_cyc_lwc2_read(cpu, (a0 + 4))));  /* 0x801A8FB8: 0xC8850004 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8FBC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A8FC0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8FC0: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x0280030);  /* gte cmd 0x30 */  /* 0x801A8FC4: 0x4A280030 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8FC8: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8FCC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A8FD0u);
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x1400006);  /* gte cmd 0x06 */  /* 0x801A8FD0: 0x4B400006 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((sp), (cpu->gte_data[24]));  /* 0x801A8FD4: 0xEBB80000 */
    gte_precision_store_word((sp), 24);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t5), (cpu->gte_data[12]));  /* 0x801A8FD8: 0xE9AC0000 */
    gte_precision_store_word((t5), 12);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t6), (cpu->gte_data[13]));  /* 0x801A8FDC: 0xE9CD0000 */
    gte_precision_store_word((t6), 13);
    psx_icache_fetch(cpu, 0x801A8FE0u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t7), (cpu->gte_data[14]));  /* 0x801A8FE0: 0xE9EE0000 */
    gte_precision_store_word((t7), 14);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8FE4: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8FE8: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x158002D);  /* gte cmd 0x2D */  /* 0x801A8FEC: 0x4B58002D */
    psx_icache_fetch(cpu, 0x801A8FF0u);
    psx_cyc_step(cpu, 0x20000004u);
    v0 = sp + 4;  /* 0x801A8FF0: 0x27A20004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */  /* 0x801A8FF4: 0x480C9800 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A8FF8: 0x00000000 */
    psx_cyc_step(cpu, 0x1000u);
    t4 = (int32_t)t4 >> 2;  /* 0x801A8FFC: 0x000C6083 */
    psx_icache_fetch(cpu, 0x801A9000u);
    psx_cyc_step(cpu, 0x1004u);
    cpu->write_word(v0, t4);  /* 0x801A9000: 0xAC4C0000 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9008: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A9074u);
    goto block_801A9074;  /* j */

block_801A900C:
    if (psx_slice_block(cpu, 0x801A900Cu, 26u, 0)) return;
    psx_icache_fetch(cpu, 0x801A900Cu);
    a1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t9, 5, 0x2000000u);  /* 0x801A900C: 0x87250000 */
    psx_icache_fetch(cpu, 0x801A9010u);
    a0 = psx_cyc_load_word(cpu, t2 + 16, 4, 0x400u);  /* 0x801A9010: 0x8D440010 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3 + -2, 3, 0x800u);  /* 0x801A9014: 0x8563FFFE */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3, 2, 0x800u);  /* 0x801A9018: 0x85620000 */
    psx_cyc_step(cpu, 0x20u);
    a1 <<= 3;  /* 0x801A901C: 0x000528C0 */
    psx_icache_fetch(cpu, 0x801A9020u);
    psx_cyc_step(cpu, 0x30u);
    a1 = a0 + a1;  /* 0x801A9020: 0x00852821 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 3;  /* 0x801A9024: 0x000318C0 */
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 + v1;  /* 0x801A9028: 0x00831821 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A902C: 0x000210C0 */
    psx_icache_fetch(cpu, 0x801A9030u);
    psx_cyc_step(cpu, 0x14u);
    a0 += v0;  /* 0x801A9030: 0x00822021 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (a1)));  /* 0x801A9034: 0xC8A00000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (a1 + 4))));  /* 0x801A9038: 0xC8A10004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A903C: 0xC8620000 */
    psx_icache_fetch(cpu, 0x801A9040u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A9040: 0xC8630004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (psx_cyc_lwc2_read(cpu, (a0)));  /* 0x801A9044: 0xC8840000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (psx_cyc_lwc2_read(cpu, (a0 + 4))));  /* 0x801A9048: 0xC8850004 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A904C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A9050u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9050: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x0280030);  /* gte cmd 0x30 */  /* 0x801A9054: 0x4A280030 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9058: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A905C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A9060u);
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x1400006);  /* gte cmd 0x06 */  /* 0x801A9060: 0x4B400006 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((sp), (cpu->gte_data[24]));  /* 0x801A9064: 0xEBB80000 */
    gte_precision_store_word((sp), 24);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t5), (cpu->gte_data[12]));  /* 0x801A9068: 0xE9AC0000 */
    gte_precision_store_word((t5), 12);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t6), (cpu->gte_data[13]));  /* 0x801A906C: 0xE9CD0000 */
    gte_precision_store_word((t6), 13);
    psx_icache_fetch(cpu, 0x801A9070u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t7), (cpu->gte_data[14]));  /* 0x801A9070: 0xE9EE0000 */
    gte_precision_store_word((t7), 14);
    /* fall through to block_801A9074 */

block_801A9074:
    if (psx_slice_block(cpu, 0x801A9074u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9074u);
    v1 = psx_cyc_load_byte(cpu, s0, 3, 0x10000u);  /* 0x801A9074: 0x92030000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9078: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 < (uint32_t)5;  /* 0x801A907C: 0x2C620005 */
    psx_icache_fetch(cpu, 0x801A9080u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801A9080 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801A9084: 0x00031080 */
    if (cond_801A9080) {
        psx_check_interrupts_at(cpu, 0x801A919Cu);
        goto block_801A919C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9088u);
    }

block_801A9088:
    if (psx_slice_block(cpu, 0x801A9088u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9088u);
    psx_cyc_step(cpu, 0x40004u);
    v0 += s2;  /* 0x801A9088: 0x00521021 */
    v0 = psx_cyc_load_word(cpu, v0, 2, 0x4u);  /* 0x801A908C: 0x8C420000 */
    psx_icache_fetch(cpu, 0x801A9090u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9090: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    uint32_t jt_801A9094 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9098: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_801A9094);
    cpu->pc = jt_801A9094; return;  /* CPS: jr v0 */

block_801A909C:
    if (psx_slice_block(cpu, 0x801A909Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A909Cu);
    v0 = psx_cyc_load_word(cpu, sp, 2, 0x20000000u);  /* 0x801A909C: 0x8FA20000 */
    psx_icache_fetch(cpu, 0x801A90A0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A90A0: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_801A90A4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A90A8: 0x00000000 */
    if (cond_801A90A4) {
        psx_check_interrupts_at(cpu, 0x801A90C0u);
        goto block_801A90C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A90ACu);
    }

block_801A90AC:
    if (psx_slice_block(cpu, 0x801A90ACu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A90ACu);
    v0 = psx_cyc_load_half(cpu, s0, 2, 0x10000u);  /* 0x801A90AC: 0x96020000 */
    psx_icache_fetch(cpu, 0x801A90B0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A90B0: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0200;  /* 0x801A90B4: 0x30420200 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A90B8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A90BC: 0x00000000 */
    if (cond_801A90B8) {
        psx_check_interrupts_at(cpu, 0x801A9194u);
        goto block_801A9194;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A90C0u);
    }

block_801A90C0:
    if (psx_slice_block(cpu, 0x801A90C0u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A90C0u);
    v0 = psx_cyc_load_word(cpu, sp + 4, 2, 0x20000000u);  /* 0x801A90C0: 0x8FA20004 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A90C4: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)2048;  /* 0x801A90C8: 0x2C420800 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A90CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A90D0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A90D0: 0x00000000 */
    if (cond_801A90CC) {
        psx_check_interrupts_at(cpu, 0x801A9194u);
        goto block_801A9194;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A90D4u);
    }

block_801A90D4:
    if (psx_slice_block(cpu, 0x801A90D4u, 44u, 0)) return;
    psx_icache_fetch(cpu, 0x801A90D4u);
    a1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a3, 5, 0x80u);  /* 0x801A90D4: 0x84E50000 */
    a0 = psx_cyc_load_word(cpu, t2 + 28, 4, 0x400u);  /* 0x801A90D8: 0x8D44001C */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a3 + 2, 3, 0x80u);  /* 0x801A90DC: 0x84E30002 */
    psx_icache_fetch(cpu, 0x801A90E0u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a3 + 4, 2, 0x80u);  /* 0x801A90E0: 0x84E20004 */
    a2 = psx_cyc_load_byte(cpu, t8, 6, 0x1000000u);  /* 0x801A90E4: 0x93060000 */
    psx_cyc_step(cpu, 0x20u);
    a1 <<= 3;  /* 0x801A90E8: 0x000528C0 */
    psx_cyc_step(cpu, 0x30u);
    a1 = a0 + a1;  /* 0x801A90EC: 0x00852821 */
    psx_icache_fetch(cpu, 0x801A90F0u);
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 3;  /* 0x801A90F0: 0x000318C0 */
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 + v1;  /* 0x801A90F4: 0x00831821 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A90F8: 0x000210C0 */
    psx_cyc_step(cpu, 0x14u);
    a0 += v0;  /* 0x801A90FC: 0x00822021 */
    psx_icache_fetch(cpu, 0x801A9100u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (a1)));  /* 0x801A9100: 0xC8A00000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (a1 + 4))));  /* 0x801A9104: 0xC8A10004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A9108: 0xC8620000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A910C: 0xC8630004 */
    psx_icache_fetch(cpu, 0x801A9110u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (psx_cyc_lwc2_read(cpu, (a0)));  /* 0x801A9110: 0xC8840000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (psx_cyc_lwc2_read(cpu, (a0 + 4))));  /* 0x801A9114: 0xC8850004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (psx_cyc_lwc2_read(cpu, (t0)));  /* 0x801A9118: 0xC9060000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A911C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A9120u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9120: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x118043F);  /* gte cmd 0x3F */  /* 0x801A9124: 0x4B18043F */
    psx_cyc_step(cpu, 0x210u);
    a0 = t1 + 4;  /* 0x801A9128: 0x25240004 */
    psx_cyc_step(cpu, 0x208u);
    v1 = t1 + 16;  /* 0x801A912C: 0x25230010 */
    psx_icache_fetch(cpu, 0x801A9130u);
    psx_cyc_step(cpu, 0x204u);
    v0 = t1 + 28;  /* 0x801A9130: 0x2522001C */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((a0), (cpu->gte_data[20]));  /* 0x801A9134: 0xE8940000 */
    gte_precision_store_word((a0), 20);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((v1), (cpu->gte_data[21]));  /* 0x801A9138: 0xE8750000 */
    gte_precision_store_word((v1), 21);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[22]));  /* 0x801A913C: 0xE8560000 */
    gte_precision_store_word((v0), 22);
    psx_icache_fetch(cpu, 0x801A9140u);
    psx_cyc_step(cpu, 0x1000040u);
    cpu->write_byte(t8, (uint8_t)a2);  /* 0x801A9140: 0xA3060000 */
    v0 = psx_cyc_load_word(cpu, sp + 4, 2, 0x20000000u);  /* 0x801A9144: 0x8FA20004 */
    v1 = psx_cyc_load_word(cpu, gp + 72, 3, 0x10000000u);  /* 0x801A9148: 0x8F830048 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801A914C: 0x00021080 */
    psx_icache_fetch(cpu, 0x801A9150u);
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801A9150: 0x00431021 */
    v0 = psx_cyc_load_word(cpu, v0, 2, 0x4u);  /* 0x801A9154: 0x8C420000 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x09000000u;  /* 0x09000000 */  /* 0x801A9158: 0x3C030900 */
    psx_cyc_step(cpu, 0xCu);
    v0 |= v1;  /* 0x801A915C: 0x00431025 */
    psx_icache_fetch(cpu, 0x801A9160u);
    psx_cyc_step(cpu, 0x204u);
    cpu->write_word(t1, v0);  /* 0x801A9160: 0xAD220000 */
    v1 = psx_cyc_load_word(cpu, sp + 4, 3, 0x20000000u);  /* 0x801A9164: 0x8FA30004 */
    v0 = psx_cyc_load_word(cpu, gp + 72, 2, 0x10000000u);  /* 0x801A9168: 0x8F820048 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 2;  /* 0x801A916C: 0x00031880 */
    psx_icache_fetch(cpu, 0x801A9170u);
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x801A9170: 0x00621821 */
    psx_cyc_step(cpu, 0x204u);
    v0 = t1 << 8;  /* 0x801A9174: 0x00091200 */
    psx_cyc_step(cpu, 0x4u);
    v0 = (uint32_t)v0 >> 8;  /* 0x801A9178: 0x00021202 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A9180u);
    psx_cyc_step(cpu, 0xCu);
    cpu->write_word(v1, v0);  /* 0x801A9180: 0xAC620000 */
    psx_check_interrupts_at(cpu, 0x801A9194u);
    goto block_801A9194;  /* j */

block_801A9184:
    if (psx_slice_block(cpu, 0x801A9184u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9184u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x80u);
    a3 += 2;  /* 0x801A9188: 0x24E70002 */
    psx_check_interrupts_at(cpu, 0x801A9198u);
    goto block_801A9198;  /* j */

block_801A918C:
    if (psx_slice_block(cpu, 0x801A918Cu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A918Cu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A9190u);
    psx_cyc_step(cpu, 0x80u);
    a3 += 2;  /* 0x801A9190: 0x24E70002 */
    psx_check_interrupts_at(cpu, 0x801A9198u);
    goto block_801A9198;  /* j */

block_801A9194:
    if (psx_slice_block(cpu, 0x801A9194u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9194u);
    psx_cyc_step(cpu, 0x80u);
    a3 += 6;  /* 0x801A9194: 0x24E70006 */
    /* fall through to block_801A9198 */

block_801A9198:
    if (psx_slice_block(cpu, 0x801A9198u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9198u);
    psx_cyc_step(cpu, 0x100u);
    t0 += 12;  /* 0x801A9198: 0x2508000C */
    /* fall through to block_801A919C */

block_801A919C:
    if (psx_slice_block(cpu, 0x801A919Cu, 12u, 0)) return;
    psx_icache_fetch(cpu, 0x801A919Cu);
    psx_cyc_step(cpu, 0x1000000u);
    t8 += 40;  /* 0x801A919C: 0x27180028 */
    psx_icache_fetch(cpu, 0x801A91A0u);
    psx_cyc_step(cpu, 0x8000u);
    t7 += 40;  /* 0x801A91A0: 0x25EF0028 */
    psx_cyc_step(cpu, 0x4000u);
    t6 += 40;  /* 0x801A91A4: 0x25CE0028 */
    psx_cyc_step(cpu, 0x2000u);
    t5 += 40;  /* 0x801A91A8: 0x25AD0028 */
    psx_cyc_step(cpu, 0x200u);
    t1 += 40;  /* 0x801A91AC: 0x25290028 */
    psx_icache_fetch(cpu, 0x801A91B0u);
    psx_cyc_step(cpu, 0x800u);
    t3 += 6;  /* 0x801A91B0: 0x256B0006 */
    psx_cyc_step(cpu, 0x2000000u);
    t9 += 6;  /* 0x801A91B4: 0x27390006 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 48, 2, 0x400u);  /* 0x801A91B8: 0x85420030 */
    psx_cyc_step(cpu, 0x20000u);
    s1++;  /* 0x801A91BC: 0x26310001 */
    psx_icache_fetch(cpu, 0x801A91C0u);
    psx_cyc_step(cpu, 0x20004u);
    v0 = (int32_t)s1 < (int32_t)v0;  /* 0x801A91C0: 0x0222102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A91C4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10000u);
    s0 += 2;  /* 0x801A91C8: 0x26100002 */
    if (cond_801A91C4) {
        psx_check_interrupts_at(cpu, 0x801A8F6Cu);
        goto block_801A8F6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A91CCu);
    }

block_801A91CC:
    if (psx_slice_block(cpu, 0x801A91CCu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A91CCu);
    s2 = psx_cyc_load_word(cpu, sp + 16, 18, 0x20000000u);  /* 0x801A91CC: 0x8FB20010 */
    psx_icache_fetch(cpu, 0x801A91D0u);
    s1 = psx_cyc_load_word(cpu, sp + 12, 17, 0x20000000u);  /* 0x801A91D0: 0x8FB1000C */
    s0 = psx_cyc_load_word(cpu, sp + 8, 16, 0x20000000u);  /* 0x801A91D4: 0x8FB00008 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_801A91D8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 24;  /* 0x801A91DC: 0x27BD0018 */
    psx_check_interrupts_at(cpu, jt_801A91D8);
    cpu->pc = jt_801A91D8; return;  /* CPS: jr $ra */
}
/* ======== prim_emit_xform_b (guest 0x801A91E0) ========
 * GTE transformed-vertex emitter, variant B (21 CPS blocks / 860
 * bytes): same vertex-count walk as prim_emit_xform_a plus
 * gte_precision_store_word / psx_gte_stall hooks on the swc2 paths.
 * Timing-faithful port of cpst_801a91e0. */

void prim_emit_xform_b(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A9204u: goto block_801A9204;
            case 0x801A9228u: goto block_801A9228;
            case 0x801A922Cu: goto block_801A922C;
            case 0x801A923Cu: goto block_801A923C;
            case 0x801A9250u: goto block_801A9250;
            case 0x801A926Cu: goto block_801A926C;
            case 0x801A927Cu: goto block_801A927C;
            case 0x801A930Cu: goto block_801A930C;
            case 0x801A9374u: goto block_801A9374;
            case 0x801A9388u: goto block_801A9388;
            case 0x801A939Cu: goto block_801A939C;
            case 0x801A93ACu: goto block_801A93AC;
            case 0x801A93C0u: goto block_801A93C0;
            case 0x801A93D4u: goto block_801A93D4;
            case 0x801A94E0u: goto block_801A94E0;
            case 0x801A94E8u: goto block_801A94E8;
            case 0x801A94F0u: goto block_801A94F0;
            case 0x801A94F4u: goto block_801A94F4;
            case 0x801A94F8u: goto block_801A94F8;
            case 0x801A9528u: goto block_801A9528;
            default: break;
        }
    }
    /* Address: 0x801A91E0, Size: 860 bytes, Blocks: 21 */

block_801A91E0:
    if (psx_slice_block(cpu, 0x801A91E0u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801A91E0u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 24;  /* 0x801A91E0: 0x27BDFFE8 */
    psx_cyc_step(cpu, 0x411u);
    t2 = a0;  /* 0x801A91E4: 0x00805021 */
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 16, s2);  /* 0x801A91E8: 0xAFB20010 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 12, s1);  /* 0x801A91EC: 0xAFB1000C */
    psx_icache_fetch(cpu, 0x801A91F0u);
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 8, s0);  /* 0x801A91F0: 0xAFB00008 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 52, 2, 0x400u);  /* 0x801A91F4: 0x85420034 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A91F8: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A91FC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A9200u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9200: 0x00000000 */
    if (cond_801A91FC) {
        psx_check_interrupts_at(cpu, 0x801A9528u);
        goto block_801A9528;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9204u);
    }

block_801A9204:
    if (psx_slice_block(cpu, 0x801A9204u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9204u);
    t9 = psx_cyc_load_word(cpu, t2 + 24, 25, 0x400u);  /* 0x801A9204: 0x8D590018 */
    v0 = psx_cyc_load_word(cpu, gp + 36, 2, 0x10000000u);  /* 0x801A9208: 0x8F820024 */
    s0 = psx_cyc_load_word(cpu, t2 + 36, 16, 0x400u);  /* 0x801A920C: 0x8D500024 */
    psx_icache_fetch(cpu, 0x801A9210u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 50, 3, 0x400u);  /* 0x801A9210: 0x85430032 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801A9214: 0x00021080 */
    psx_cyc_step(cpu, 0x404u);
    v0 = t2 + v0;  /* 0x801A9218: 0x01421021 */
    t1 = psx_cyc_load_word(cpu, v0, 9, 0x4u);  /* 0x801A921C: 0x8C490000 */
    psx_icache_fetch(cpu, 0x801A9220u);
    psx_cyc_step(cpu, 0x9u);
    int cond_801A9220 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9224: 0x00000000 */
    if (cond_801A9220) {
        psx_check_interrupts_at(cpu, 0x801A922Cu);
        goto block_801A922C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9228u);
    }

block_801A9228:
    if (psx_slice_block(cpu, 0x801A9228u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9228u);
    psx_cyc_step(cpu, 0x20000008u);
    cpu->write_word(sp + 4, v1);  /* 0x801A9228: 0xAFA30004 */
    /* fall through to block_801A922C */

block_801A922C:
    if (psx_slice_block(cpu, 0x801A922Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A922Cu);
    v0 = psx_cyc_load_word(cpu, t2 + 28, 2, 0x400u);  /* 0x801A922C: 0x8D42001C */
    psx_icache_fetch(cpu, 0x801A9230u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9230: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A9234 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9238: 0x00000000 */
    if (cond_801A9234) {
        psx_check_interrupts_at(cpu, 0x801A9528u);
        goto block_801A9528;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A923Cu);
    }

block_801A923C:
    if (psx_slice_block(cpu, 0x801A923Cu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A923Cu);
    t0 = psx_cyc_load_word(cpu, t2 + 20, 8, 0x400u);  /* 0x801A923C: 0x8D480014 */
    psx_icache_fetch(cpu, 0x801A9240u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 48, 2, 0x400u);  /* 0x801A9240: 0x85420030 */
    a3 = psx_cyc_load_word(cpu, t2 + 32, 7, 0x400u);  /* 0x801A9244: 0x8D470020 */
    psx_cyc_step(cpu, 0x4u);
    int cond_801A9248 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20001u);
    s1 = zero;  /* 0x801A924C: 0x00008821 */
    if (cond_801A9248) {
        psx_check_interrupts_at(cpu, 0x801A9528u);
        goto block_801A9528;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9250u);
    }

block_801A9250:
    if (psx_slice_block(cpu, 0x801A9250u, 7u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9250u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x801A9250: 0x3C028012 */
    psx_cyc_step(cpu, 0x40004u);
    s2 = v0 + 24856;  /* 0x801A9254: 0x24526118 */
    psx_cyc_step(cpu, 0x2000800u);
    t3 = t9 + 6;  /* 0x801A9258: 0x272B0006 */
    psx_cyc_step(cpu, 0x1000200u);
    t8 = t1 + 7;  /* 0x801A925C: 0x25380007 */
    psx_icache_fetch(cpu, 0x801A9260u);
    psx_cyc_step(cpu, 0x8200u);
    t7 = t1 + 32;  /* 0x801A9260: 0x252F0020 */
    psx_cyc_step(cpu, 0x4200u);
    t6 = t1 + 20;  /* 0x801A9264: 0x252E0014 */
    psx_cyc_step(cpu, 0x2200u);
    t5 = t1 + 8;  /* 0x801A9268: 0x252D0008 */
    /* fall through to block_801A926C */

block_801A926C:
    if (psx_slice_block(cpu, 0x801A926Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A926Cu);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 50, 2, 0x400u);  /* 0x801A926C: 0x85420032 */
    psx_icache_fetch(cpu, 0x801A9270u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9270: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A9274 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9278: 0x00000000 */
    if (cond_801A9274) {
        psx_check_interrupts_at(cpu, 0x801A930Cu);
        goto block_801A930C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A927Cu);
    }

block_801A927C:
    if (psx_slice_block(cpu, 0x801A927Cu, 36u, 0)) return;
    psx_icache_fetch(cpu, 0x801A927Cu);
    a1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t9, 5, 0x2000000u);  /* 0x801A927C: 0x87250000 */
    psx_icache_fetch(cpu, 0x801A9280u);
    a0 = psx_cyc_load_word(cpu, t2 + 16, 4, 0x400u);  /* 0x801A9280: 0x8D440010 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3 + -4, 3, 0x800u);  /* 0x801A9284: 0x8563FFFC */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3 + -2, 2, 0x800u);  /* 0x801A9288: 0x8562FFFE */
    psx_cyc_step(cpu, 0x20u);
    a1 <<= 3;  /* 0x801A928C: 0x000528C0 */
    psx_icache_fetch(cpu, 0x801A9290u);
    psx_cyc_step(cpu, 0x30u);
    a1 = a0 + a1;  /* 0x801A9290: 0x00852821 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 3;  /* 0x801A9294: 0x000318C0 */
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 + v1;  /* 0x801A9298: 0x00831821 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A929C: 0x000210C0 */
    psx_icache_fetch(cpu, 0x801A92A0u);
    psx_cyc_step(cpu, 0x14u);
    a0 += v0;  /* 0x801A92A0: 0x00822021 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (a1)));  /* 0x801A92A4: 0xC8A00000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (a1 + 4))));  /* 0x801A92A8: 0xC8A10004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A92AC: 0xC8620000 */
    psx_icache_fetch(cpu, 0x801A92B0u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A92B0: 0xC8630004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (psx_cyc_lwc2_read(cpu, (a0)));  /* 0x801A92B4: 0xC8840000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (psx_cyc_lwc2_read(cpu, (a0 + 4))));  /* 0x801A92B8: 0xC8850004 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A92BC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A92C0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A92C0: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x0280030);  /* gte cmd 0x30 */  /* 0x801A92C4: 0x4A280030 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A92C8: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A92CC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A92D0u);
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x1400006);  /* gte cmd 0x06 */  /* 0x801A92D0: 0x4B400006 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((sp), (cpu->gte_data[24]));  /* 0x801A92D4: 0xEBB80000 */
    gte_precision_store_word((sp), 24);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t5), (cpu->gte_data[12]));  /* 0x801A92D8: 0xE9AC0000 */
    gte_precision_store_word((t5), 12);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t6), (cpu->gte_data[13]));  /* 0x801A92DC: 0xE9CD0000 */
    gte_precision_store_word((t6), 13);
    psx_icache_fetch(cpu, 0x801A92E0u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t7), (cpu->gte_data[14]));  /* 0x801A92E0: 0xE9EE0000 */
    gte_precision_store_word((t7), 14);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A92E4: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A92E8: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x158002D);  /* gte cmd 0x2D */  /* 0x801A92EC: 0x4B58002D */
    psx_icache_fetch(cpu, 0x801A92F0u);
    psx_cyc_step(cpu, 0x20000004u);
    v0 = sp + 4;  /* 0x801A92F0: 0x27A20004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_read(cpu, 12);
    t4 = gte_read_data(cpu, 19);  /* mfc2 */  /* 0x801A92F4: 0x480C9800 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A92F8: 0x00000000 */
    psx_cyc_step(cpu, 0x1000u);
    t4 = (int32_t)t4 >> 2;  /* 0x801A92FC: 0x000C6083 */
    psx_icache_fetch(cpu, 0x801A9300u);
    psx_cyc_step(cpu, 0x1004u);
    cpu->write_word(v0, t4);  /* 0x801A9300: 0xAC4C0000 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9308: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A9374u);
    goto block_801A9374;  /* j */

block_801A930C:
    if (psx_slice_block(cpu, 0x801A930Cu, 26u, 0)) return;
    psx_icache_fetch(cpu, 0x801A930Cu);
    a1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t9, 5, 0x2000000u);  /* 0x801A930C: 0x87250000 */
    psx_icache_fetch(cpu, 0x801A9310u);
    a0 = psx_cyc_load_word(cpu, t2 + 16, 4, 0x400u);  /* 0x801A9310: 0x8D440010 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3 + -4, 3, 0x800u);  /* 0x801A9314: 0x8563FFFC */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3 + -2, 2, 0x800u);  /* 0x801A9318: 0x8562FFFE */
    psx_cyc_step(cpu, 0x20u);
    a1 <<= 3;  /* 0x801A931C: 0x000528C0 */
    psx_icache_fetch(cpu, 0x801A9320u);
    psx_cyc_step(cpu, 0x30u);
    a1 = a0 + a1;  /* 0x801A9320: 0x00852821 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 3;  /* 0x801A9324: 0x000318C0 */
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 + v1;  /* 0x801A9328: 0x00831821 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A932C: 0x000210C0 */
    psx_icache_fetch(cpu, 0x801A9330u);
    psx_cyc_step(cpu, 0x14u);
    a0 += v0;  /* 0x801A9330: 0x00822021 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (a1)));  /* 0x801A9334: 0xC8A00000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (a1 + 4))));  /* 0x801A9338: 0xC8A10004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A933C: 0xC8620000 */
    psx_icache_fetch(cpu, 0x801A9340u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A9340: 0xC8630004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (psx_cyc_lwc2_read(cpu, (a0)));  /* 0x801A9344: 0xC8840000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (psx_cyc_lwc2_read(cpu, (a0 + 4))));  /* 0x801A9348: 0xC8850004 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A934C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A9350u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9350: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x0280030);  /* gte cmd 0x30 */  /* 0x801A9354: 0x4A280030 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9358: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A935C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A9360u);
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x1400006);  /* gte cmd 0x06 */  /* 0x801A9360: 0x4B400006 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((sp), (cpu->gte_data[24]));  /* 0x801A9364: 0xEBB80000 */
    gte_precision_store_word((sp), 24);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t5), (cpu->gte_data[12]));  /* 0x801A9368: 0xE9AC0000 */
    gte_precision_store_word((t5), 12);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t6), (cpu->gte_data[13]));  /* 0x801A936C: 0xE9CD0000 */
    gte_precision_store_word((t6), 13);
    psx_icache_fetch(cpu, 0x801A9370u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((t7), (cpu->gte_data[14]));  /* 0x801A9370: 0xE9EE0000 */
    gte_precision_store_word((t7), 14);
    /* fall through to block_801A9374 */

block_801A9374:
    if (psx_slice_block(cpu, 0x801A9374u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9374u);
    v1 = psx_cyc_load_byte(cpu, s0, 3, 0x10000u);  /* 0x801A9374: 0x92030000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9378: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 < (uint32_t)5;  /* 0x801A937C: 0x2C620005 */
    psx_icache_fetch(cpu, 0x801A9380u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801A9380 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801A9384: 0x00031080 */
    if (cond_801A9380) {
        psx_check_interrupts_at(cpu, 0x801A94F8u);
        goto block_801A94F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9388u);
    }

block_801A9388:
    if (psx_slice_block(cpu, 0x801A9388u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9388u);
    psx_cyc_step(cpu, 0x40004u);
    v0 += s2;  /* 0x801A9388: 0x00521021 */
    v0 = psx_cyc_load_word(cpu, v0, 2, 0x4u);  /* 0x801A938C: 0x8C420000 */
    psx_icache_fetch(cpu, 0x801A9390u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9390: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    uint32_t jt_801A9394 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9398: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_801A9394);
    cpu->pc = jt_801A9394; return;  /* CPS: jr v0 */

block_801A939C:
    if (psx_slice_block(cpu, 0x801A939Cu, 4u, 0)) return;
    psx_icache_fetch(cpu, 0x801A939Cu);
    v0 = psx_cyc_load_word(cpu, sp, 2, 0x20000000u);  /* 0x801A939C: 0x8FA20000 */
    psx_icache_fetch(cpu, 0x801A93A0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93A0: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_801A93A4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93A8: 0x00000000 */
    if (cond_801A93A4) {
        psx_check_interrupts_at(cpu, 0x801A93C0u);
        goto block_801A93C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A93ACu);
    }

block_801A93AC:
    if (psx_slice_block(cpu, 0x801A93ACu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A93ACu);
    v0 = psx_cyc_load_half(cpu, s0, 2, 0x10000u);  /* 0x801A93AC: 0x96020000 */
    psx_icache_fetch(cpu, 0x801A93B0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93B0: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0200;  /* 0x801A93B4: 0x30420200 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A93B8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93BC: 0x00000000 */
    if (cond_801A93B8) {
        psx_check_interrupts_at(cpu, 0x801A94F0u);
        goto block_801A94F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A93C0u);
    }

block_801A93C0:
    if (psx_slice_block(cpu, 0x801A93C0u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A93C0u);
    v0 = psx_cyc_load_word(cpu, sp + 4, 2, 0x20000000u);  /* 0x801A93C0: 0x8FA20004 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93C4: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 = v0 < (uint32_t)2048;  /* 0x801A93C8: 0x2C420800 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801A93CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801A93D0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93D0: 0x00000000 */
    if (cond_801A93CC) {
        psx_check_interrupts_at(cpu, 0x801A94F0u);
        goto block_801A94F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A93D4u);
    }

block_801A93D4:
    if (psx_slice_block(cpu, 0x801A93D4u, 67u, 0)) return;
    psx_icache_fetch(cpu, 0x801A93D4u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t3, 2, 0x800u);  /* 0x801A93D4: 0x85620000 */
    v1 = psx_cyc_load_word(cpu, t2 + 16, 3, 0x400u);  /* 0x801A93D8: 0x8D430010 */
    a2 = psx_cyc_load_byte(cpu, t8, 6, 0x1000000u);  /* 0x801A93DC: 0x93060000 */
    psx_icache_fetch(cpu, 0x801A93E0u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A93E0: 0x000210C0 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x801A93E4: 0x00621821 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A93E8: 0xC8600000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A93EC: 0xC8610004 */
    psx_icache_fetch(cpu, 0x801A93F0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93F0: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A93F4: 0x00000000 */
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x801A93F8: 0x4A180001 */
    psx_cyc_step(cpu, 0x204u);
    v0 = t1 + 44;  /* 0x801A93FC: 0x2522002C */
    psx_icache_fetch(cpu, 0x801A9400u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[14]));  /* 0x801A9400: 0xE84E0000 */
    gte_precision_store_word((v0), 14);
    a1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a3, 5, 0x80u);  /* 0x801A9404: 0x84E50000 */
    a0 = psx_cyc_load_word(cpu, t2 + 28, 4, 0x400u);  /* 0x801A9408: 0x8D44001C */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a3 + 2, 3, 0x80u);  /* 0x801A940C: 0x84E30002 */
    psx_icache_fetch(cpu, 0x801A9410u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a3 + 4, 2, 0x80u);  /* 0x801A9410: 0x84E20004 */
    psx_cyc_step(cpu, 0x20u);
    a1 <<= 3;  /* 0x801A9414: 0x000528C0 */
    psx_cyc_step(cpu, 0x30u);
    a1 = a0 + a1;  /* 0x801A9418: 0x00852821 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 3;  /* 0x801A941C: 0x000318C0 */
    psx_icache_fetch(cpu, 0x801A9420u);
    psx_cyc_step(cpu, 0x18u);
    v1 = a0 + v1;  /* 0x801A9420: 0x00831821 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A9424: 0x000210C0 */
    psx_cyc_step(cpu, 0x14u);
    a0 += v0;  /* 0x801A9428: 0x00822021 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (a1)));  /* 0x801A942C: 0xC8A00000 */
    psx_icache_fetch(cpu, 0x801A9430u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (a1 + 4))));  /* 0x801A9430: 0xC8A10004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[2] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A9434: 0xC8620000 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 3, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A9438: 0xC8630004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[4] = (psx_cyc_lwc2_read(cpu, (a0)));  /* 0x801A943C: 0xC8840000 */
    psx_icache_fetch(cpu, 0x801A9440u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 5, (psx_cyc_lwc2_read(cpu, (a0 + 4))));  /* 0x801A9440: 0xC8850004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (psx_cyc_lwc2_read(cpu, (t0)));  /* 0x801A9444: 0xC9060000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9448: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A944C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A9450u);
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x118043F);  /* gte cmd 0x3F */  /* 0x801A9450: 0x4B18043F */
    psx_cyc_step(cpu, 0x210u);
    a0 = t1 + 4;  /* 0x801A9454: 0x25240004 */
    psx_cyc_step(cpu, 0x208u);
    v1 = t1 + 16;  /* 0x801A9458: 0x25230010 */
    psx_cyc_step(cpu, 0x204u);
    v0 = t1 + 28;  /* 0x801A945C: 0x2522001C */
    psx_icache_fetch(cpu, 0x801A9460u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((a0), (cpu->gte_data[20]));  /* 0x801A9460: 0xE8940000 */
    gte_precision_store_word((a0), 20);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((v1), (cpu->gte_data[21]));  /* 0x801A9464: 0xE8750000 */
    gte_precision_store_word((v1), 21);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[22]));  /* 0x801A9468: 0xE8560000 */
    gte_precision_store_word((v0), 22);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a3 + 6, 2, 0x80u);  /* 0x801A946C: 0x84E20006 */
    psx_icache_fetch(cpu, 0x801A9470u);
    v1 = psx_cyc_load_word(cpu, t2 + 28, 3, 0x400u);  /* 0x801A9470: 0x8D43001C */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 3;  /* 0x801A9474: 0x000210C0 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x801A9478: 0x00621821 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (psx_cyc_lwc2_read(cpu, (v1)));  /* 0x801A947C: 0xC8600000 */
    psx_icache_fetch(cpu, 0x801A9480u);
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (psx_cyc_lwc2_read(cpu, (v1 + 4))));  /* 0x801A9480: 0xC8610004 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (psx_cyc_lwc2_read(cpu, (t0)));  /* 0x801A9484: 0xC9060000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A9488: 0x00000000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801A948C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801A9490u);
    psx_cyc_step(cpu, 0x0u);
    gte_execute(cpu, 0x108041B);  /* gte cmd 0x1B */  /* 0x801A9490: 0x4B08041B */
    psx_cyc_step(cpu, 0x204u);
    v0 = t1 + 40;  /* 0x801A9494: 0x25220028 */
    psx_cyc_step(cpu, 0x0u);
    
    psx_gte_stall(cpu);
    cpu->write_word((v0), (cpu->gte_data[22]));  /* 0x801A9498: 0xE8560000 */
    gte_precision_store_word((v0), 22);
    psx_cyc_step(cpu, 0x1000040u);
    cpu->write_byte(t8, (uint8_t)a2);  /* 0x801A949C: 0xA3060000 */
    psx_icache_fetch(cpu, 0x801A94A0u);
    v0 = psx_cyc_load_word(cpu, sp + 4, 2, 0x20000000u);  /* 0x801A94A0: 0x8FA20004 */
    v1 = psx_cyc_load_word(cpu, gp + 72, 3, 0x10000000u);  /* 0x801A94A4: 0x8F830048 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801A94A8: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801A94AC: 0x00431021 */
    psx_icache_fetch(cpu, 0x801A94B0u);
    v0 = psx_cyc_load_word(cpu, v0, 2, 0x4u);  /* 0x801A94B0: 0x8C420000 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x0C000000u;  /* 0x0C000000 */  /* 0x801A94B4: 0x3C030C00 */
    psx_cyc_step(cpu, 0xCu);
    v0 |= v1;  /* 0x801A94B8: 0x00431025 */
    psx_cyc_step(cpu, 0x204u);
    cpu->write_word(t1, v0);  /* 0x801A94BC: 0xAD220000 */
    psx_icache_fetch(cpu, 0x801A94C0u);
    v1 = psx_cyc_load_word(cpu, sp + 4, 3, 0x20000000u);  /* 0x801A94C0: 0x8FA30004 */
    v0 = psx_cyc_load_word(cpu, gp + 72, 2, 0x10000000u);  /* 0x801A94C4: 0x8F820048 */
    psx_cyc_step(cpu, 0x8u);
    v1 <<= 2;  /* 0x801A94C8: 0x00031880 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x801A94CC: 0x00621821 */
    psx_icache_fetch(cpu, 0x801A94D0u);
    psx_cyc_step(cpu, 0x204u);
    v0 = t1 << 8;  /* 0x801A94D0: 0x00091200 */
    psx_cyc_step(cpu, 0x4u);
    v0 = (uint32_t)v0 >> 8;  /* 0x801A94D4: 0x00021202 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_word(v1, v0);  /* 0x801A94DC: 0xAC620000 */
    psx_check_interrupts_at(cpu, 0x801A94F0u);
    goto block_801A94F0;  /* j */

block_801A94E0:
    if (psx_slice_block(cpu, 0x801A94E0u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A94E0u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x80u);
    a3 += 2;  /* 0x801A94E4: 0x24E70002 */
    psx_check_interrupts_at(cpu, 0x801A94F4u);
    goto block_801A94F4;  /* j */

block_801A94E8:
    if (psx_slice_block(cpu, 0x801A94E8u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801A94E8u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x80u);
    a3 += 2;  /* 0x801A94EC: 0x24E70002 */
    psx_check_interrupts_at(cpu, 0x801A94F4u);
    goto block_801A94F4;  /* j */

block_801A94F0:
    if (psx_slice_block(cpu, 0x801A94F0u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801A94F0u);
    psx_cyc_step(cpu, 0x80u);
    a3 += 8;  /* 0x801A94F0: 0x24E70008 */
    /* fall through to block_801A94F4 */

block_801A94F4:
    if (psx_slice_block(cpu, 0x801A94F4u, 1u, 0)) return;
    psx_icache_fetch(cpu, 0x801A94F4u);
    psx_cyc_step(cpu, 0x100u);
    t0 += 16;  /* 0x801A94F4: 0x25080010 */
    /* fall through to block_801A94F8 */

block_801A94F8:
    if (psx_slice_block(cpu, 0x801A94F8u, 12u, 0)) return;
    psx_icache_fetch(cpu, 0x801A94F8u);
    psx_cyc_step(cpu, 0x1000000u);
    t8 += 52;  /* 0x801A94F8: 0x27180034 */
    psx_cyc_step(cpu, 0x8000u);
    t7 += 52;  /* 0x801A94FC: 0x25EF0034 */
    psx_icache_fetch(cpu, 0x801A9500u);
    psx_cyc_step(cpu, 0x4000u);
    t6 += 52;  /* 0x801A9500: 0x25CE0034 */
    psx_cyc_step(cpu, 0x2000u);
    t5 += 52;  /* 0x801A9504: 0x25AD0034 */
    psx_cyc_step(cpu, 0x200u);
    t1 += 52;  /* 0x801A9508: 0x25290034 */
    psx_cyc_step(cpu, 0x800u);
    t3 += 8;  /* 0x801A950C: 0x256B0008 */
    psx_icache_fetch(cpu, 0x801A9510u);
    psx_cyc_step(cpu, 0x2000000u);
    t9 += 8;  /* 0x801A9510: 0x27390008 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, t2 + 48, 2, 0x400u);  /* 0x801A9514: 0x85420030 */
    psx_cyc_step(cpu, 0x20000u);
    s1++;  /* 0x801A9518: 0x26310001 */
    psx_cyc_step(cpu, 0x20004u);
    v0 = (int32_t)s1 < (int32_t)v0;  /* 0x801A951C: 0x0222102A */
    psx_icache_fetch(cpu, 0x801A9520u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801A9520 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10000u);
    s0 += 2;  /* 0x801A9524: 0x26100002 */
    if (cond_801A9520) {
        psx_check_interrupts_at(cpu, 0x801A926Cu);
        goto block_801A926C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9528u);
    }

block_801A9528:
    if (psx_slice_block(cpu, 0x801A9528u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801A9528u);
    s2 = psx_cyc_load_word(cpu, sp + 16, 18, 0x20000000u);  /* 0x801A9528: 0x8FB20010 */
    s1 = psx_cyc_load_word(cpu, sp + 12, 17, 0x20000000u);  /* 0x801A952C: 0x8FB1000C */
    psx_icache_fetch(cpu, 0x801A9530u);
    s0 = psx_cyc_load_word(cpu, sp + 8, 16, 0x20000000u);  /* 0x801A9530: 0x8FB00008 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_801A9534 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 24;  /* 0x801A9538: 0x27BD0018 */
    psx_check_interrupts_at(cpu, jt_801A9534);
    cpu->pc = jt_801A9534; return;  /* CPS: jr $ra */
}
/* ======== scene_prim_tag_dispatch (guest 0x801AFB28) ========
 * Scene/prim tag dispatcher (24 CPS blocks / 528 bytes): loads three
 * u16 indices at a0+0/2/4 against the node base, then dispatches on the
 * prim tag class (0x1000/0x2000/0x3000/0x4000 ranges) into the
 * scene_node_walk (0x801B0DB8) chain. Timing-faithful port of
 * cpst_801afb28. */

void scene_prim_tag_dispatch(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801AFCB8u: goto block_801AFCB8;
            case 0x801AFB60u: goto block_801AFB60;
            case 0x801AFB78u: goto block_801AFB78;
            case 0x801AFB80u: goto block_801AFB80;
            case 0x801AFB88u: goto block_801AFB88;
            case 0x801AFB90u: goto block_801AFB90;
            case 0x801AFB9Cu: goto block_801AFB9C;
            case 0x801AFBA4u: goto block_801AFBA4;
            case 0x801AFBACu: goto block_801AFBAC;
            case 0x801AFBC4u: goto block_801AFBC4;
            case 0x801AFBD8u: goto block_801AFBD8;
            case 0x801AFBE0u: goto block_801AFBE0;
            case 0x801AFBF8u: goto block_801AFBF8;
            case 0x801AFC1Cu: goto block_801AFC1C;
            case 0x801AFC28u: goto block_801AFC28;
            case 0x801AFC34u: goto block_801AFC34;
            case 0x801AFC58u: goto block_801AFC58;
            case 0x801AFC64u: goto block_801AFC64;
            case 0x801AFC7Cu: goto block_801AFC7C;
            case 0x801AFC90u: goto block_801AFC90;
            case 0x801AFCD4u: goto block_801AFCD4;
            case 0x801AFCF8u: goto block_801AFCF8;
            case 0x801AFD1Cu: goto block_801AFD1C;
            default: break;
        }
    }
    /* Address: 0x801AFB28, Size: 528 bytes, Blocks: 24 */

block_801AFB28:
    if (psx_slice_block(cpu, 0x801AFB28u, 14u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFB28u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 40;  /* 0x801AFB28: 0x27BDFFD8 */
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 32, ra);  /* 0x801AFB2C: 0xAFBF0020 */
    psx_icache_fetch(cpu, 0x801AFB30u);
    psx_cyc_step(cpu, 0x20080000u);
    cpu->write_word(sp + 28, s3);  /* 0x801AFB30: 0xAFB3001C */
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 24, s2);  /* 0x801AFB34: 0xAFB20018 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 20, s1);  /* 0x801AFB38: 0xAFB10014 */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 16, s0);  /* 0x801AFB3C: 0xAFB00010 */
    psx_icache_fetch(cpu, 0x801AFB40u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0, 3, 0x10u);  /* 0x801AFB40: 0x84830000 */
    v0 = psx_cyc_load_word(cpu, v0 + 8, 2, 0x4u);  /* 0x801AFB44: 0x8C420008 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFB48: 0x00000000 */
    psx_cyc_step(cpu, 0x2000Cu);
    s1 = v0 + v1;  /* 0x801AFB4C: 0x00438821 */
    psx_icache_fetch(cpu, 0x801AFB50u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0 + 2, 3, 0x10u);  /* 0x801AFB50: 0x84830002 */
    a0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0 + 4, 4, 0x10u);  /* 0x801AFB54: 0x84840004 */
    psx_cyc_step(cpu, 0x4000Cu);
    s2 = v0 + v1;  /* 0x801AFB58: 0x00439021 */
    psx_cyc_step(cpu, 0x80014u);
    s3 = v0 + a0;  /* 0x801AFB5C: 0x00449821 */
    /* fall through to block_801AFB60 */

block_801AFB60:
    if (psx_slice_block(cpu, 0x801AFB60u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFB60u);
    a0 = psx_cyc_load_word(cpu, s2, 4, 0x40000u);  /* 0x801AFB60: 0x8E440000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFB64: 0x00000000 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0, 3, 0x10u);  /* 0x801AFB68: 0x84830000 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 8192;  /* 0x801AFB6C: 0x24022000 */
    psx_icache_fetch(cpu, 0x801AFB70u);
    psx_cyc_step(cpu, 0xCu);
    int cond_801AFB70 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v1 < 8193;  /* 0x801AFB74: 0x28622001 */
    if (cond_801AFB70) {
        psx_check_interrupts_at(cpu, 0x801AFBE0u);
        goto block_801AFBE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFB78u);
    }

block_801AFB78:
    if (psx_slice_block(cpu, 0x801AFB78u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFB78u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801AFB78 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 4096;  /* 0x801AFB7C: 0x24021000 */
    if (cond_801AFB78) {
        psx_check_interrupts_at(cpu, 0x801AFB90u);
        goto block_801AFB90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFB80u);
    }

block_801AFB80:
    if (psx_slice_block(cpu, 0x801AFB80u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFB80u);
    psx_cyc_step(cpu, 0xCu);
    int cond_801AFB80 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFB84: 0x00000000 */
    if (cond_801AFB80) {
        psx_check_interrupts_at(cpu, 0x801AFBACu);
        goto block_801AFBAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFB88u);
    }

block_801AFB88:
    if (psx_slice_block(cpu, 0x801AFB88u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFB88u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFB8C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AFC64u);
    goto block_801AFC64;  /* j */

block_801AFB90:
    if (psx_slice_block(cpu, 0x801AFB90u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFB90u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 12288;  /* 0x801AFB90: 0x24023000 */
    psx_cyc_step(cpu, 0xCu);
    int cond_801AFB94 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 16384;  /* 0x801AFB98: 0x24024000 */
    if (cond_801AFB94) {
        psx_check_interrupts_at(cpu, 0x801AFC34u);
        goto block_801AFC34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFB9Cu);
    }

block_801AFB9C:
    if (psx_slice_block(cpu, 0x801AFB9Cu, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFB9Cu);
    psx_cyc_step(cpu, 0xCu);
    int cond_801AFB9C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AFBA0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFBA0: 0x00000000 */
    if (cond_801AFB9C) {
        psx_check_interrupts_at(cpu, 0x801AFC58u);
        goto block_801AFC58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFBA4u);
    }

block_801AFBA4:
    if (psx_slice_block(cpu, 0x801AFBA4u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFBA4u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFBA8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AFC64u);
    goto block_801AFC64;  /* j */

block_801AFBAC:
    if (psx_slice_block(cpu, 0x801AFBACu, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFBACu);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0 + 2, 3, 0x10u);  /* 0x801AFBAC: 0x84830002 */
    psx_icache_fetch(cpu, 0x801AFBB0u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s1 + 2, 2, 0x20000u);  /* 0x801AFBB0: 0x86220002 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFBB4: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v0 < (int32_t)v1;  /* 0x801AFBB8: 0x0043102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_801AFBBC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AFBC0u);
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 + -6;  /* 0x801AFBC0: 0x2482FFFA */
    if (cond_801AFBBC) {
        psx_check_interrupts_at(cpu, 0x801AFBD8u);
        goto block_801AFBD8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFBC4u);
    }

block_801AFBC4:
    if (psx_slice_block(cpu, 0x801AFBC4u, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFBC4u);
    psx_cyc_step(cpu, 0x40004u);
    cpu->write_word(s2, v0);  /* 0x801AFBC4: 0xAE420000 */
    v0 = psx_cyc_load_half(cpu, s1 + 2, 2, 0x20000u);  /* 0x801AFBC8: 0x96220002 */
    v1 = psx_cyc_load_half(cpu, a0 + -6, 3, 0x10u);  /* 0x801AFBCC: 0x9483FFFA */
    psx_icache_fetch(cpu, 0x801AFBD0u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x801AFBD4: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x801AFC1Cu);
    goto block_801AFC1C;  /* j */

block_801AFBD8:
    if (psx_slice_block(cpu, 0x801AFBD8u, 2u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFBD8u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 + 4;  /* 0x801AFBDC: 0x24820004 */
    psx_check_interrupts_at(cpu, 0x801AFC28u);
    goto block_801AFC28;  /* j */

block_801AFBE0:
    if (psx_slice_block(cpu, 0x801AFBE0u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFBE0u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0 + 2, 3, 0x10u);  /* 0x801AFBE0: 0x84830002 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s1 + 2, 2, 0x20000u);  /* 0x801AFBE4: 0x86220002 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFBE8: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v0 < (int32_t)v1;  /* 0x801AFBEC: 0x0043102A */
    psx_icache_fetch(cpu, 0x801AFBF0u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801AFBF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x14u);
    v0 = a0 + 6;  /* 0x801AFBF4: 0x24820006 */
    if (cond_801AFBF0) {
        psx_check_interrupts_at(cpu, 0x801AFC28u);
        goto block_801AFC28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFBF8u);
    }

block_801AFBF8:
    if (psx_slice_block(cpu, 0x801AFBF8u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFBF8u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0 + 4, 3, 0x10u);  /* 0x801AFBF8: 0x84830004 */
    v0 = psx_cyc_load_word(cpu, gp + 80, 2, 0x10000000u);  /* 0x801AFBFC: 0x8F820050 */
    psx_icache_fetch(cpu, 0x801AFC00u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFC00: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AFC04: 0x00431021 */
    psx_cyc_step(cpu, 0x40004u);
    cpu->write_word(s2, v0);  /* 0x801AFC08: 0xAE420000 */
    v1 = psx_cyc_load_half(cpu, v0, 3, 0x4u);  /* 0x801AFC0C: 0x94430000 */
    psx_icache_fetch(cpu, 0x801AFC10u);
    v0 = psx_cyc_load_half(cpu, s1 + 2, 2, 0x20000u);  /* 0x801AFC10: 0x96220002 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFC14: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x801AFC18: 0x24420001 */
    /* fall through to block_801AFC1C */

block_801AFC1C:
    if (psx_slice_block(cpu, 0x801AFC1Cu, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFC1Cu);
    psx_cyc_step(cpu, 0x20008u);
    cpu->write_half(s1, (uint16_t)v1);  /* 0x801AFC1C: 0xA6230000 */
    psx_icache_fetch(cpu, 0x801AFC20u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20004u);
    cpu->write_half(s1 + 2, (uint16_t)v0);  /* 0x801AFC24: 0xA6220002 */
    psx_check_interrupts_at(cpu, 0x801AFB60u);
    goto block_801AFB60;  /* j */

block_801AFC28:
    if (psx_slice_block(cpu, 0x801AFC28u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFC28u);
    psx_cyc_step(cpu, 0x40004u);
    cpu->write_word(s2, v0);  /* 0x801AFC28: 0xAE420000 */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AFC30u);
    psx_cyc_step(cpu, 0x20001u);
    cpu->write_half(s1 + 2, (uint16_t)zero);  /* 0x801AFC30: 0xA6200002 */
    psx_check_interrupts_at(cpu, 0x801AFB60u);
    goto block_801AFB60;  /* j */

block_801AFC34:
    if (psx_slice_block(cpu, 0x801AFC34u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFC34u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a0 + 2, 2, 0x10u);  /* 0x801AFC34: 0x84820002 */
    v1 = psx_cyc_load_word(cpu, gp + 80, 3, 0x10000000u);  /* 0x801AFC38: 0x8F830050 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFC3C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AFC40u);
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x801AFC40: 0x00621821 */
    psx_cyc_step(cpu, 0x40008u);
    cpu->write_word(s2, v1);  /* 0x801AFC44: 0xAE430000 */
    v0 = psx_cyc_load_half(cpu, v1, 2, 0x8u);  /* 0x801AFC48: 0x94620000 */
    psx_cyc_step(cpu, 0x20001u);
    cpu->write_half(s1 + 2, (uint16_t)zero);  /* 0x801AFC4C: 0xA6200002 */
    psx_icache_fetch(cpu, 0x801AFC50u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20004u);
    cpu->write_half(s1, (uint16_t)v0);  /* 0x801AFC54: 0xA6220000 */
    psx_check_interrupts_at(cpu, 0x801AFB60u);
    goto block_801AFB60;  /* j */

block_801AFC58:
    if (psx_slice_block(cpu, 0x801AFC58u, 3u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFC58u);
    v0 = psx_cyc_load_word(cpu, gp + 76, 2, 0x10000000u);  /* 0x801AFC58: 0x8F82004C */
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AFC60u);
    psx_cyc_step(cpu, 0x5u);
    cpu->write_word(v0 + 20, zero);  /* 0x801AFC60: 0xAC400014 */
    psx_check_interrupts_at(cpu, 0x801AFD1Cu);
    goto block_801AFD1C;  /* j */

block_801AFC64:
    if (psx_slice_block(cpu, 0x801AFC64u, 6u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFC64u);
    a1 = psx_cyc_load_word(cpu, s2, 5, 0x40000u);  /* 0x801AFC64: 0x8E450000 */
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s1, 3, 0x20000u);  /* 0x801AFC68: 0x86230000 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, a1, 2, 0x20u);  /* 0x801AFC6C: 0x84A20000 */
    psx_icache_fetch(cpu, 0x801AFC70u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFC70: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    int cond_801AFC74 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFC78: 0x00000000 */
    if (cond_801AFC74) {
        psx_check_interrupts_at(cpu, 0x801AFCF8u);
        goto block_801AFCF8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFC7Cu);
    }

block_801AFC7C:
    if (psx_slice_block(cpu, 0x801AFC7Cu, 5u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFC7Cu);
    psx_cyc_step(cpu, 0x29u);
    v1 = a1;  /* 0x801AFC7C: 0x00A01821 */
    psx_icache_fetch(cpu, 0x801AFC80u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, v1 + 2, 2, 0x8u);  /* 0x801AFC80: 0x84620002 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFC84: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_801AFC88 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10001u);
    s0 = zero;  /* 0x801AFC8C: 0x00008021 */
    if (cond_801AFC88) {
        psx_check_interrupts_at(cpu, 0x801AFCD4u);
        goto block_801AFCD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFC90u);
    }

block_801AFC90:
    if (psx_slice_block(cpu, 0x801AFC90u, 10u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFC90u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, v1 + 4, 2, 0x8u);  /* 0x801AFC90: 0x84620004 */
    v1 = psx_cyc_load_word(cpu, s3, 3, 0x80000u);  /* 0x801AFC94: 0x8E630000 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 1;  /* 0x801AFC98: 0x00021040 */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AFC9C: 0x00431021 */
    psx_icache_fetch(cpu, 0x801AFCA0u);
    v1 = (int32_t)(int16_t)psx_cyc_load_half(cpu, v0, 3, 0x4u);  /* 0x801AFCA0: 0x84430000 */
    v0 = psx_cyc_load_word(cpu, gp + 80, 2, 0x10000000u);  /* 0x801AFCA4: 0x8F820050 */
    a0 = psx_cyc_load_word(cpu, gp + 76, 4, 0x10000000u);  /* 0x801AFCA8: 0x8F84004C */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x801AFCAC: 0x00431021 */
    psx_icache_fetch(cpu, 0x801AFCB0u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801AFCB8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x14u);
    cpu->write_word(a0 + 12, v0);  /* 0x801AFCB4: 0xAC82000C */
    psx_check_interrupts_at(cpu, 0x801B0DB8u);
    cpu->pc = 0x801B0DB8u; return;  /* CPS jal -> cpst_801b0db8 */

block_801AFCB8:
    if (psx_slice_block(cpu, 0x801AFCB8u, 7u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFCB8u);
    v1 = psx_cyc_load_word(cpu, s2, 3, 0x40000u);  /* 0x801AFCB8: 0x8E430000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFCBC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AFCC0u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, v1 + 2, 2, 0x8u);  /* 0x801AFCC0: 0x84620002 */
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x801AFCC4: 0x26100001 */
    psx_cyc_step(cpu, 0x10004u);
    v0 = (int32_t)s0 < (int32_t)v0;  /* 0x801AFCC8: 0x0202102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_801AFCCC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801AFCD0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFCD0: 0x00000000 */
    if (cond_801AFCCC) {
        psx_check_interrupts_at(cpu, 0x801AFC90u);
        goto block_801AFC90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFCD4u);
    }

block_801AFCD4:
    if (psx_slice_block(cpu, 0x801AFCD4u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFCD4u);
    v0 = psx_cyc_load_word(cpu, s2, 2, 0x40000u);  /* 0x801AFCD4: 0x8E420000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFCD8: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v1 = v0 + 6;  /* 0x801AFCDC: 0x24430006 */
    psx_icache_fetch(cpu, 0x801AFCE0u);
    psx_cyc_step(cpu, 0x40008u);
    cpu->write_word(s2, v1);  /* 0x801AFCE0: 0xAE430000 */
    a0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, s1, 4, 0x20000u);  /* 0x801AFCE4: 0x86240000 */
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, v0 + 6, 2, 0x4u);  /* 0x801AFCE8: 0x84420006 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFCEC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AFCF0u);
    psx_cyc_step(cpu, 0x14u);
    int cond_801AFCF0 = (a0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x29u);
    a1 = v1;  /* 0x801AFCF4: 0x00602821 */
    if (cond_801AFCF0) {
        psx_check_interrupts_at(cpu, 0x801AFC7Cu);
        goto block_801AFC7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AFCF8u);
    }

block_801AFCF8:
    if (psx_slice_block(cpu, 0x801AFCF8u, 9u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFCF8u);
    v0 = psx_cyc_load_half(cpu, s1, 2, 0x20000u);  /* 0x801AFCF8: 0x96220000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFCFC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801AFD00u);
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x801AFD00: 0x24420001 */
    psx_cyc_step(cpu, 0x20004u);
    cpu->write_half(s1, (uint16_t)v0);  /* 0x801AFD04: 0xA6220000 */
    v0 = psx_cyc_load_word(cpu, s2, 2, 0x40000u);  /* 0x801AFD08: 0x8E420000 */
    v1 = psx_cyc_load_word(cpu, gp + 76, 3, 0x10000000u);  /* 0x801AFD0C: 0x8F83004C */
    psx_icache_fetch(cpu, 0x801AFD10u);
    v0 = (int32_t)(int16_t)psx_cyc_load_half(cpu, v0, 2, 0x4u);  /* 0x801AFD10: 0x84420000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801AFD14: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_word(v1 + 20, v0);  /* 0x801AFD18: 0xAC620014 */
    /* fall through to block_801AFD1C */

block_801AFD1C:
    if (psx_slice_block(cpu, 0x801AFD1Cu, 7u, 0)) return;
    psx_icache_fetch(cpu, 0x801AFD1Cu);
    ra = psx_cyc_load_word(cpu, sp + 32, 31, 0x20000000u);  /* 0x801AFD1C: 0x8FBF0020 */
    psx_icache_fetch(cpu, 0x801AFD20u);
    s3 = psx_cyc_load_word(cpu, sp + 28, 19, 0x20000000u);  /* 0x801AFD20: 0x8FB3001C */
    s2 = psx_cyc_load_word(cpu, sp + 24, 18, 0x20000000u);  /* 0x801AFD24: 0x8FB20018 */
    s1 = psx_cyc_load_word(cpu, sp + 20, 17, 0x20000000u);  /* 0x801AFD28: 0x8FB10014 */
    s0 = psx_cyc_load_word(cpu, sp + 16, 16, 0x20000000u);  /* 0x801AFD2C: 0x8FB00010 */
    psx_icache_fetch(cpu, 0x801AFD30u);
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_801AFD30 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 40;  /* 0x801AFD34: 0x27BD0028 */
    psx_check_interrupts_at(cpu, jt_801AFD30);
    cpu->pc = jt_801AFD30; return;  /* CPS: jr $ra */
}

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
/* ======== cd_result_wait_a_cps (guest 0x801968CC) ========
 * CD synchronous command-result wait (33 CPS blocks / 640 bytes).
 * a0 = wait mode (s6; 0 = retry until response, nonzero = single pass),
 * a1 = optional 8-byte result buffer guest addr (s7).
 *
 * Arms the shared busy-wait triple @0x801FC6E0/E4/E8 (deadline =
 * cd_timer_read(-1)+960, iteration counter, message ptr 0x8012582C),
 * then loops: cd_timer_read -> deadline/counter timeout check ->
 * cd_intc_busy_read -> while busy run the cd_response_process pump
 * (status bits 4->handlerA @0x801BC84C arg state@0x801BCB25 buf
 * 0x801FC6D0, bit 2->handlerB @0x801BC848 arg state@0x801BCB24 buf
 * 0x801FC6C8 — the same handler cells cd_response_pump dispatches) ->
 * when idle check the response byte @0x801BCB24: 2 or 5 = success
 * (commit 2, copy 8 bytes 0x801FC6C8 -> out_buf, return the byte),
 * else loop (mode 0) or return 0 (mode != 0). Timeout prints
 * "CD timeout"-class strings via cd_str_print + printf_forward_guest
 * then cd_cmd_state_reset and returns -1.
 *
 * Guest frame: sp-=64, saves s0-s7+fp+ra; all state in gpr[]/guest RAM.
 * 32 continuation addrs registered in clean_cps.map. */
uint32_t cd_result_wait_a_cps(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196908u: goto block_80196908;
            case 0x80196954u: goto block_80196954;
            case 0x801969A4u: goto block_801969A4;
            case 0x801969F0u: goto block_801969F0;
            case 0x801969F8u: goto block_801969F8;
            case 0x80196A14u: goto block_80196A14;
            case 0x80196A3Cu: goto block_80196A3C;
            case 0x80196A74u: goto block_80196A74;
            case 0x80196AA4u: goto block_80196AA4;
            case 0x8019694Cu: goto block_8019694C;
            case 0x8019696Cu: goto block_8019696C;
            case 0x80196998u: goto block_80196998;
            case 0x80196A00u: goto block_80196A00;
            case 0x80196A04u: goto block_80196A04;
            case 0x80196A0Cu: goto block_80196A0C;
            case 0x80196A1Cu: goto block_80196A1C;
            case 0x80196A34u: goto block_80196A34;
            case 0x80196A48u: goto block_80196A48;
            case 0x80196A50u: goto block_80196A50;
            case 0x80196A64u: goto block_80196A64;
            case 0x80196A78u: goto block_80196A78;
            case 0x80196A80u: goto block_80196A80;
            case 0x80196A94u: goto block_80196A94;
            case 0x80196AACu: goto block_80196AAC;
            case 0x80196ABCu: goto block_80196ABC;
            case 0x80196AD0u: goto block_80196AD0;
            case 0x80196AD8u: goto block_80196AD8;
            case 0x80196AF0u: goto block_80196AF0;
            case 0x80196AF4u: goto block_80196AF4;
            case 0x80196B0Cu: goto block_80196B0C;
            case 0x80196B14u: goto block_80196B14;
            case 0x80196B1Cu: goto block_80196B1C;
            default: break;
        }
    }
    /* Address: 0x801968CC, Size: 640 bytes, Blocks: 33 */

block_801968CC:
    /* entry: build 0x40 frame, save s0-s7/fp/ra, arm a0=-1 for the
     * cd_timer_read tail-call (jal link = block_80196908). */
    sp -= 64;
    cpu->write_word(sp + 48, s6);
    s6 = a0;
    cpu->write_word(sp + 52, s7);
    s7 = a1;
    a0 = 0xFFFFFFFFu;
    cpu->write_word(sp + 60, ra);
    cpu->write_word(sp + 56, fp);
    cpu->write_word(sp + 44, s5);
    cpu->write_word(sp + 40, s4);
    cpu->write_word(sp + 36, s3);
    cpu->write_word(sp + 32, s2);
    cpu->write_word(sp + 28, s1);
    ra = 0x80196908u;                       /* jal link */
    cpu->write_word(sp + 24, s0);           /* delay slot */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;                  /* tail-call cd_timer_read */
    return cpu->gpr[2];

block_80196908:
    /* cd_timer_read(-1) returned v0 = now. Arm the busy-wait triple and
     * precompute the cell/table base registers. */
    fp = 0x801BC86Cu;                       /* 0x801C0000-14228: name tbl2 base */
    s4 = 0x801BC8ECu;                       /* 0x801C0000-14100: name tbl1 base */
    s2 = 0x801BCB24u;                       /* 0x801C0000-13532: resp/state byte */
    s5 = s2 + 1;                            /* 0x801BCB25: state byte B */
    s3 = 2;                                 /* expected response / commit value */
    v0 += 960;                              /* deadline = now + 960 */
    cpu->write_word(0x801FC6E0, v0);        /* deadline cell */
    cpu->write_word(0x801FC6E4, zero);      /* iteration counter */
    cpu->write_word(0x801FC6E8, 0x8012582Cu); /* message ptr */
    /* fall through */

block_8019694C:
    /* outer wait-loop head: re-arm a0=-1 and tail-call cd_timer_read. */
    a0 = 0xFFFFFFFFu;
    ra = 0x80196954u;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;                  /* tail-call cd_timer_read */
    return cpu->gpr[2];

block_80196954:
    /* deadline < now ? (signed slt) -> timeout */
    v1 = cpu->read_word(0x801FC6E0);        /* deadline */
    v1 = (uint32_t)((int32_t)v1 < (int32_t)v0);
    if (v1 != zero) {
        psx_check_interrupts_at(cpu, 0x80196998u);
        goto block_80196998;                /* taken: timeout */
    }
    psx_check_interrupts_at(cpu, 0x8019696Cu);

block_8019696C:
    /* iteration counter++ ; cap 0x3C0000 exceeded -> timeout */
    v0 = cpu->read_word(0x801FC6E4);
    v1 = v0;
    v0 = v0 + 1;
    cpu->write_word(0x801FC6E4, v0);
    v0 = 0x003C0000u;
    v0 = (uint32_t)((int32_t)v0 < (int32_t)v1);
    if (v0 == zero) {                       /* cnt <= cap: continue */
        psx_check_interrupts_at(cpu, 0x80196A00u);
        goto block_80196A00;
    }
    psx_check_interrupts_at(cpu, 0x80196998u);
    goto block_80196998;                    /* timeout */

block_80196998:
    /* timeout: cd_str_print("CD timeout"-class msg 0x801257A4) */
    a0 = 0x801257A4u;
    ra = 0x801969A4u;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u;                  /* tail-call cd_str_print */
    return cpu->gpr[2];

block_801969A4:
    /* printf args: fmt 0x801257B4, a1=*0x801FC6E8, a2=name-tbl1[resp],
     * a3=name-tbl2[mode], stack arg = name-tbl1[byte2]. */
    a0 = cpu->read_byte(s2);                /* response byte */
    v0 = cpu->read_byte(s2 + 1);            /* state byte B */
    a1 = cpu->read_word(0x801FC6E8);        /* stored message ptr */
    v0 = v0 << 2;
    v0 = v0 + s4;
    a0 = a0 << 2;
    v1 = cpu->read_word(v0);                /* tbl1[byte2] -> stack arg */
    v0 = cpu->read_byte(0x801BC865u);       /* mode byte */
    a0 = a0 + s4;
    v0 = v0 << 2;
    v0 = v0 + fp;
    cpu->write_word(sp + 16, v1);           /* 5th printf arg on stack */
    a2 = cpu->read_word(a0);                /* tbl1[resp] */
    a3 = cpu->read_word(v0);                /* tbl2[mode] */
    a0 = 0x801257B4u;                       /* fmt */
    ra = 0x801969F0u;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u;                  /* tail-call printf_forward_guest */
    return cpu->gpr[2];

block_801969F0:
    /* cd_cmd_state_reset() then return -1 via block_80196A04. */
    ra = 0x801969F8u;
    psx_check_interrupts_at(cpu, 0x801972A8u);
    cpu->pc = 0x801972A8u;                  /* tail-call cd_cmd_state_reset */
    return cpu->gpr[2];

block_801969F8:
    /* j block_80196A04 with delay-slot v0 = -1 -> return -1. */
    v0 = 0xFFFFFFFFu;
    psx_check_interrupts_at(cpu, 0x80196A04u);
    goto block_80196A04;

block_80196A00:
    /* counter-under-cap path marker: v0 = 0 -> continue at 80196A04. */
    v0 = zero;
    /* fall through */

block_80196A04:
    /* v0 != 0 -> epilogue (return v0=-1); else -> intc busy check. */
    { int _bc = (v0 != zero);
      v0 = 0xFFFFFFFFu;                     /* delay slot */
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196B1Cu);
          goto block_80196B1C;
      }
      psx_check_interrupts_at(cpu, 0x80196A0Cu);
    }

block_80196A0C:
    /* cd_intc_busy_read() -> v0 */
    ra = 0x80196A14u;
    psx_check_interrupts_at(cpu, 0x8019A7D4u);
    cpu->pc = 0x8019A7D4u;                  /* tail-call cd_intc_busy_read */
    return cpu->gpr[2];

block_80196A14:
    /* not busy -> check response byte; busy -> snapshot state&3 + pump. */
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196ABCu);
        goto block_80196ABC;
    }
    psx_check_interrupts_at(cpu, 0x80196A1Cu);

block_80196A1C:
    /* s1 = *(*0x801BCB0C) & 3 — saved low bits of the CD state byte. */
    v0 = cpu->read_word(0x801BCB0Cu);
    v0 = cpu->read_byte(v0);
    s1 = v0 & 3u;
    /* fall through */

block_80196A34:
    /* response-pump loop head: cd_response_process() -> status in v0. */
    ra = 0x80196A3Cu;
    psx_check_interrupts_at(cpu, 0x80196370u);
    cpu->pc = 0x80196370u;                  /* tail-call cd_response_process */
    return cpu->gpr[2];

block_80196A3C:
    /* status dispatch: 0 -> commit state byte; else bits 4/2 -> handlers. */
    s0 = v0;
    { int _bc = (s0 == zero);
      v0 = s0 & 4u;                         /* delay slot */
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196AACu);
          goto block_80196AAC;
      }
      psx_check_interrupts_at(cpu, 0x80196A48u);
    }

block_80196A48:
    /* status & 4 ? -> handlerA arm check */
    { int _bc = (v0 == zero);               /* (s0&4)==0 */
      v0 = s0 & 2u;                         /* delay slot */
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196A78u);
          goto block_80196A78;
      }
      psx_check_interrupts_at(cpu, 0x80196A50u);
    }

block_80196A50:
    /* handlerA cell @0x801BC84C (0x801C0000-14260) null ? -> bit-2 test */
    v0 = cpu->read_word(0x801BC84Cu);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196A74u);
        goto block_80196A74;
    }
    psx_check_interrupts_at(cpu, 0x80196A64u);

block_80196A64:
    /* jalr handlerA(state@0x801BCB25, 0x801FC6D0) -> ret 0x80196A74 */
    a0 = cpu->read_byte(s5);
    a1 = 0x801FC6D0u;
    { uint32_t _jt = v0;                    /* handlerA fn-ptr */
      ra = 0x80196A74u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196A74:
    v0 = s0 & 2u;
    /* fall through */

block_80196A78:
    /* status & 2 clear -> pump again; set -> handlerB arm check */
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196A34u);
        goto block_80196A34;                /* loop: response_process */
    }
    psx_check_interrupts_at(cpu, 0x80196A80u);

block_80196A80:
    /* handlerB cell @0x801BC848 (0x801C0000-14264) null ? -> pump again */
    v0 = cpu->read_word(0x801BC848u);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196A34u);
        goto block_80196A34;                /* loop */
    }
    psx_check_interrupts_at(cpu, 0x80196A94u);

block_80196A94:
    /* jalr handlerB(state@0x801BCB24, 0x801FC6C8) -> ret 0x80196AA4 */
    a0 = cpu->read_byte(s2);
    a1 = 0x801FC6C8u;
    { uint32_t _jt = v0;                    /* handlerB fn-ptr */
      ra = 0x80196AA4u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196AA4:
    /* j block_80196A34 — pump loop. */
    psx_check_interrupts_at(cpu, 0x80196A34u);
    goto block_80196A34;

block_80196AAC:
    /* response_process returned 0: commit saved low bits s1 back to
     * *(*0x801BCB0C), then check the response byte. */
    v0 = cpu->read_word(0x801BCB0Cu);
    cpu->write_byte(v0, (uint8_t)s1);
    /* fall through */

block_80196ABC:
    /* response byte @0x801BCB24 in {2,5} -> success; else give-up test. */
    v0 = cpu->read_byte(s2);
    a2 = v0 & 0x00FFu;
    v0 = 5;
    if (a2 == s3) {                         /* == 2 */
        psx_check_interrupts_at(cpu, 0x80196AD8u);
        goto block_80196AD8;
    }
    psx_check_interrupts_at(cpu, 0x80196AD0u);

block_80196AD0:
    /* == 5 ? -> success; else mode test */
    if (a2 != v0) {                         /* != 5 */
        psx_check_interrupts_at(cpu, 0x80196B14u);
        goto block_80196B14;
    }
    psx_check_interrupts_at(cpu, 0x80196AD8u);
    goto block_80196AD8;

block_80196AD8:
    /* success: mark resp=2, copy 8-byte result 0x801FC6C8 -> out_buf
     * when a1 != 0, return the response byte in v0 (=a2). */
    cpu->write_byte(s2, (uint8_t)s3);       /* *0x801BCB24 = 2 */
    a1 = s7;
    a0 = 0x801FC6C8u;
    v1 = 7;                                 /* delay slot: count = 7 */
    if (a1 == zero) {
        psx_check_interrupts_at(cpu, 0x80196B0Cu);
        goto block_80196B0C;
    }
    psx_check_interrupts_at(cpu, 0x80196AF0u);
    goto block_80196AF0;

block_80196AF0:
    a3 = 0xFFFFFFFFu;                       /* loop sentinel -1 */
    /* fall through */

block_80196AF4:
    /* 8-byte copy loop: *a1++ = *a0++ while --v1 != -1. */
    v0 = cpu->read_byte(a0);
    a0 = a0 + 1;
    v1 = v1 - 1;
    cpu->write_byte(a1, (uint8_t)v0);
    a1 = a1 + 1;                            /* delay slot */
    if (v1 != a3) {
        psx_check_interrupts_at(cpu, 0x80196AF4u);
        goto block_80196AF4;
    }
    psx_check_interrupts_at(cpu, 0x80196B0Cu);
    goto block_80196B0C;

block_80196B0C:
    v0 = a2;                                /* return response byte */
    psx_check_interrupts_at(cpu, 0x80196B1Cu);
    goto block_80196B1C;

block_80196B14:
    /* response byte unexpected: mode==0 -> outer wait loop; else ret 0. */
    v0 = zero;                              /* delay slot */
    if (s6 == zero) {
        psx_check_interrupts_at(cpu, 0x8019694Cu);
        goto block_8019694C;
    }
    psx_check_interrupts_at(cpu, 0x80196B1Cu);
    goto block_80196B1C;

block_80196B1C:
    /* epilogue: restore s0-s7/fp/ra, sp+=64, jr ra. */
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
    sp += 64;                               /* delay slot */
    { uint32_t _jt = ra;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }
}

/* ======== cd_result_wait_b_cps (guest 0x80196B4C) ========
 * CD synchronous two-slot result wait (37 CPS blocks / 712 bytes).
 * a0 = wait mode (s7; 0 = retry until a result lands, nonzero = single
 * pass), a1 = optional out_buf guest addr (s3).
 *
 * Same busy-wait triple @0x801FC6E0/E4/E8 as wait_a (deadline +960,
 * counter cap 0x3C0000, message ptr 0x80125834) and the same
 * cd_response_process pump (status bit4 -> handlerA @0x801BC84C arg
 * state@0x801BCB25 buf 0x801FC6D0, bit2 -> handlerB @0x801BC848 arg
 * state@0x801BCB24 buf 0x801FC6C8). On idle it polls TWO response
 * slots: *0x801BCB26 != 0 -> consume (write 0), copy 8 bytes
 * 0x801FC6D8 -> out_buf, return the slot byte; elif *0x801BCB25 != 0 ->
 * consume, copy 8 bytes 0x801FC6D0 -> out_buf, return the byte; else
 * mode==0 loops / mode!=0 returns 0. Timeout path identical shape to
 * wait_a (cd_str_print + printf_forward_guest + cd_cmd_state_reset,
 * return -1).
 * Guest frame sp-=64 saves s0-s7+fp+ra; 36 continuation addrs. */
uint32_t cd_result_wait_b_cps(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196B88u: goto block_80196B88;
            case 0x80196BD4u: goto block_80196BD4;
            case 0x80196C24u: goto block_80196C24;
            case 0x80196C70u: goto block_80196C70;
            case 0x80196C78u: goto block_80196C78;
            case 0x80196C94u: goto block_80196C94;
            case 0x80196CBCu: goto block_80196CBC;
            case 0x80196CF4u: goto block_80196CF4;
            case 0x80196D24u: goto block_80196D24;
            case 0x80196BCCu: goto block_80196BCC;
            case 0x80196BECu: goto block_80196BEC;
            case 0x80196C18u: goto block_80196C18;
            case 0x80196C80u: goto block_80196C80;
            case 0x80196C84u: goto block_80196C84;
            case 0x80196C8Cu: goto block_80196C8C;
            case 0x80196C9Cu: goto block_80196C9C;
            case 0x80196CB4u: goto block_80196CB4;
            case 0x80196CC8u: goto block_80196CC8;
            case 0x80196CD0u: goto block_80196CD0;
            case 0x80196CE4u: goto block_80196CE4;
            case 0x80196CF8u: goto block_80196CF8;
            case 0x80196D00u: goto block_80196D00;
            case 0x80196D14u: goto block_80196D14;
            case 0x80196D2Cu: goto block_80196D2C;
            case 0x80196D3Cu: goto block_80196D3C;
            case 0x80196D50u: goto block_80196D50;
            case 0x80196D64u: goto block_80196D64;
            case 0x80196D6Cu: goto block_80196D6C;
            case 0x80196D84u: goto block_80196D84;
            case 0x80196D8Cu: goto block_80196D8C;
            case 0x80196DA0u: goto block_80196DA0;
            case 0x80196DB8u: goto block_80196DB8;
            case 0x80196DBCu: goto block_80196DBC;
            case 0x80196DD4u: goto block_80196DD4;
            case 0x80196DDCu: goto block_80196DDC;
            case 0x80196DE4u: goto block_80196DE4;
            default: break;
        }
    }
    /* Address: 0x80196B4C, Size: 712 bytes, Blocks: 37 */

block_80196B4C:
    /* entry: frame + save s0-s7/fp/ra; s7=a0 mode, s3=a1 out_buf. */
    sp -= 64;
    cpu->write_word(sp + 52, s7);
    s7 = a0;
    cpu->write_word(sp + 36, s3);
    s3 = a1;
    a0 = 0xFFFFFFFFu;
    cpu->write_word(sp + 60, ra);
    cpu->write_word(sp + 56, fp);
    cpu->write_word(sp + 48, s6);
    cpu->write_word(sp + 44, s5);
    cpu->write_word(sp + 40, s4);
    cpu->write_word(sp + 32, s2);
    cpu->write_word(sp + 28, s1);
    ra = 0x80196B88u;                       /* jal link */
    cpu->write_word(sp + 24, s0);           /* delay slot */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;                  /* tail-call cd_timer_read */
    return cpu->gpr[2];

block_80196B88:
    /* arm wait triple; s2=resp base 0x801BCB24, s6=+1, s4=+2,
     * s5=name tbl1 0x801BC8EC, fp=name tbl2 0x801BC86C. */
    fp = 0x801BC86Cu;
    s5 = 0x801BC8ECu;
    s2 = 0x801BCB24u;
    s6 = s2 + 1;                            /* 0x801BCB25 slot1 */
    s4 = s2 + 2;                            /* 0x801BCB26 slot2 */
    v0 += 960;
    cpu->write_word(0x801FC6E0, v0);        /* deadline */
    cpu->write_word(0x801FC6E4, zero);      /* counter */
    cpu->write_word(0x801FC6E8, 0x80125834u); /* message ptr */
    /* fall through */

block_80196BCC:
    /* outer wait-loop head: cd_timer_read(-1). */
    a0 = 0xFFFFFFFFu;
    ra = 0x80196BD4u;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u;
    return cpu->gpr[2];

block_80196BD4:
    /* deadline < now -> timeout */
    v1 = cpu->read_word(0x801FC6E0);
    v1 = (uint32_t)((int32_t)v1 < (int32_t)v0);
    if (v1 != zero) {
        psx_check_interrupts_at(cpu, 0x80196C18u);
        goto block_80196C18;
    }
    psx_check_interrupts_at(cpu, 0x80196BECu);

block_80196BEC:
    /* counter++ ; cap -> timeout */
    v0 = cpu->read_word(0x801FC6E4);
    v1 = v0;
    v0 = v0 + 1;
    cpu->write_word(0x801FC6E4, v0);
    v0 = 0x003C0000u;
    v0 = (uint32_t)((int32_t)v0 < (int32_t)v1);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196C80u);
        goto block_80196C80;
    }
    psx_check_interrupts_at(cpu, 0x80196C18u);
    goto block_80196C18;

block_80196C18:
    /* timeout: cd_str_print(0x801257A4) */
    a0 = 0x801257A4u;
    ra = 0x80196C24u;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u;
    return cpu->gpr[2];

block_80196C24:
    /* printf args (same shape as wait_a; tbl1 base = s5). */
    a0 = cpu->read_byte(s2);
    v0 = cpu->read_byte(s2 + 1);
    a1 = cpu->read_word(0x801FC6E8);
    v0 = v0 << 2;
    v0 = v0 + s5;
    a0 = a0 << 2;
    v1 = cpu->read_word(v0);
    v0 = cpu->read_byte(0x801BC865u);
    a0 = a0 + s5;
    v0 = v0 << 2;
    v0 = v0 + fp;
    cpu->write_word(sp + 16, v1);
    a2 = cpu->read_word(a0);
    a3 = cpu->read_word(v0);
    a0 = 0x801257B4u;
    ra = 0x80196C70u;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u;
    return cpu->gpr[2];

block_80196C70:
    /* cd_cmd_state_reset() then return -1 via 0x80196C84. */
    ra = 0x80196C78u;
    psx_check_interrupts_at(cpu, 0x801972A8u);
    cpu->pc = 0x801972A8u;
    return cpu->gpr[2];

block_80196C78:
    v0 = 0xFFFFFFFFu;                       /* j 0x80196C84, v0=-1 */
    psx_check_interrupts_at(cpu, 0x80196C84u);
    goto block_80196C84;

block_80196C80:
    v0 = zero;                              /* continue marker */
    /* fall through */

block_80196C84:
    /* v0 != 0 -> epilogue (ret -1); else intc busy check. */
    { int _bc = (v0 != zero);
      v0 = 0xFFFFFFFFu;
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196DE4u);
          goto block_80196DE4;
      }
      psx_check_interrupts_at(cpu, 0x80196C8Cu);
    }

block_80196C8C:
    /* cd_intc_busy_read() */
    ra = 0x80196C94u;
    psx_check_interrupts_at(cpu, 0x8019A7D4u);
    cpu->pc = 0x8019A7D4u;
    return cpu->gpr[2];

block_80196C94:
    /* not busy -> poll result slots; busy -> snapshot + pump. */
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196D3Cu);
        goto block_80196D3C;
    }
    psx_check_interrupts_at(cpu, 0x80196C9Cu);

block_80196C9C:
    /* s1 = *(*0x801BCB0C) & 3 */
    v0 = cpu->read_word(0x801BCB0Cu);
    v0 = cpu->read_byte(v0);
    s1 = v0 & 3u;
    /* fall through */

block_80196CB4:
    /* pump head: cd_response_process() */
    ra = 0x80196CBCu;
    psx_check_interrupts_at(cpu, 0x80196370u);
    cpu->pc = 0x80196370u;
    return cpu->gpr[2];

block_80196CBC:
    /* status dispatch (same bit layout as wait_a). */
    s0 = v0;
    { int _bc = (s0 == zero);
      v0 = s0 & 4u;
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196D2Cu);
          goto block_80196D2C;
      }
      psx_check_interrupts_at(cpu, 0x80196CC8u);
    }

block_80196CC8:
    { int _bc = (v0 == zero);               /* (s0&4)==0 */
      v0 = s0 & 2u;
      if (_bc) {
          psx_check_interrupts_at(cpu, 0x80196CF8u);
          goto block_80196CF8;
      }
      psx_check_interrupts_at(cpu, 0x80196CD0u);
    }

block_80196CD0:
    /* handlerA cell @0x801BC84C (0x801C0000-14260) */
    v0 = cpu->read_word(0x801BC84Cu);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196CF4u);
        goto block_80196CF4;
    }
    psx_check_interrupts_at(cpu, 0x80196CE4u);

block_80196CE4:
    /* jalr handlerA(*0x801BCB25, 0x801FC6D0) -> ret 0x80196CF4 */
    a0 = cpu->read_byte(s6);
    a1 = 0x801FC6D0u;
    { uint32_t _jt = v0;
      ra = 0x80196CF4u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196CF4:
    v0 = s0 & 2u;
    /* fall through */

block_80196CF8:
    if (v0 == zero) {                       /* (s0&2)==0 -> pump again */
        psx_check_interrupts_at(cpu, 0x80196CB4u);
        goto block_80196CB4;
    }
    psx_check_interrupts_at(cpu, 0x80196D00u);

block_80196D00:
    /* handlerB cell @0x801BC848 (0x801C0000-14264) */
    v0 = cpu->read_word(0x801BC848u);
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80196CB4u);
        goto block_80196CB4;
    }
    psx_check_interrupts_at(cpu, 0x80196D14u);

block_80196D14:
    /* jalr handlerB(*0x801BCB24, 0x801FC6C8) -> ret 0x80196D24 */
    a0 = cpu->read_byte(s2);
    a1 = 0x801FC6C8u;
    { uint32_t _jt = v0;
      ra = 0x80196D24u;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }

block_80196D24:
    psx_check_interrupts_at(cpu, 0x80196CB4u);
    goto block_80196CB4;                    /* pump loop */

block_80196D2C:
    /* status==0: commit s1 -> *(*0x801BCB0C), then poll slots. */
    v0 = cpu->read_word(0x801BCB0Cu);
    cpu->write_byte(v0, (uint8_t)s1);
    /* fall through */

block_80196D3C:
    /* slot2 *0x801BCB26 nonzero? -> consume + copy 0x801FC6D8 */
    v0 = cpu->read_byte(s4);
    a2 = v0 & 0x00FFu;
    if (a2 == zero) {
        psx_check_interrupts_at(cpu, 0x80196D8Cu);
        goto block_80196D8C;
    }
    psx_check_interrupts_at(cpu, 0x80196D50u);

block_80196D50:
    /* consume slot2, copy 8B 0x801FC6D8 -> s3 (if s3 != 0). */
    cpu->write_byte(s2 + 2, (uint8_t)zero); /* *0x801BCB26 = 0 */
    a0 = 0x801FC6D8u;
    a1 = s3;
    if (s3 == zero) {
        psx_check_interrupts_at(cpu, 0x80196DD4u);
        goto block_80196DD4;
    }
    psx_check_interrupts_at(cpu, 0x80196D64u);
    goto block_80196D64;

block_80196D64:
    v1 = 7;                                 /* 0x80196D64: count = 7 */
    a3 = 0xFFFFFFFFu;                       /* 0x80196D68: sentinel -1 */
    /* fall through */

block_80196D6C:
    /* 8-byte copy loop: *a1++ = *a0++ while --v1 != -1. */
    v0 = cpu->read_byte(a0);
    a0 = a0 + 1;
    v1 = v1 - 1;
    cpu->write_byte(a1, (uint8_t)v0);
    a1 = a1 + 1;                            /* delay slot */
    if (v1 != a3) {
        psx_check_interrupts_at(cpu, 0x80196D6Cu);
        goto block_80196D6C;
    }
    psx_check_interrupts_at(cpu, 0x80196D84u);
    goto block_80196D84;

block_80196D84:
    v0 = a2;                                /* return slot byte */
    psx_check_interrupts_at(cpu, 0x80196DE4u);
    goto block_80196DE4;

block_80196D8C:
    /* slot1 *0x801BCB25 nonzero? -> consume + copy 0x801FC6D0 */
    v0 = cpu->read_byte(s4 - 1);
    a2 = v0 & 0x00FFu;
    if (a2 == zero) {
        psx_check_interrupts_at(cpu, 0x80196DDCu);
        goto block_80196DDC;
    }
    psx_check_interrupts_at(cpu, 0x80196DA0u);

block_80196DA0:
    /* consume slot1, copy 8B 0x801FC6D0 -> s3 (if s3 != 0). */
    cpu->write_byte(s2 + 1, (uint8_t)zero); /* *0x801BCB25 = 0 */
    a1 = s3;
    a0 = 0x801FC6D0u;
    v1 = 7;                                 /* delay slot */
    if (a1 == zero) {
        psx_check_interrupts_at(cpu, 0x80196DD4u);
        goto block_80196DD4;
    }
    psx_check_interrupts_at(cpu, 0x80196DB8u);
    goto block_80196DB8;

block_80196DB8:
    a3 = 0xFFFFFFFFu;
    /* fall through */

block_80196DBC:
    /* 8-byte copy loop: *a1++ = *a0++ while --v1 != -1. */
    v0 = cpu->read_byte(a0);
    a0 = a0 + 1;
    v1 = v1 - 1;
    cpu->write_byte(a1, (uint8_t)v0);
    a1 = a1 + 1;                            /* delay slot */
    if (v1 != a3) {
        psx_check_interrupts_at(cpu, 0x80196DBCu);
        goto block_80196DBC;
    }
    psx_check_interrupts_at(cpu, 0x80196DD4u);
    goto block_80196DD4;

block_80196DD4:
    v0 = a2;                                /* return slot byte */
    psx_check_interrupts_at(cpu, 0x80196DE4u);
    goto block_80196DE4;

block_80196DDC:
    /* no slot pending: mode==0 -> outer wait loop; else ret 0. */
    v0 = zero;                              /* delay slot */
    if (s7 == zero) {
        psx_check_interrupts_at(cpu, 0x80196BCCu);
        goto block_80196BCC;
    }
    psx_check_interrupts_at(cpu, 0x80196DE4u);
    goto block_80196DE4;

block_80196DE4:
    /* epilogue: restore s0-s7/fp/ra, sp+=64, jr ra. */
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
    sp += 64;                               /* delay slot */
    { uint32_t _jt = ra;
      psx_check_interrupts_at(cpu, _jt);
      cpu->pc = _jt; return cpu->gpr[2]; }
}

/* ======== cd_read_step_cb_cps (guest 0x801985C4) ========
 * CD read-step state-machine step callback (39 CPS blocks / 628 bytes).
 * a0 = step selector (only 1 continues; other -> mark chan +20 = -1 and
 * bail), a1 = user cookie (s1, passed through to the completion cb).
 *
 * Timing-faithful port of cpst_801985c4 (cd_irq_probe.c): keeps every
 * psx_slice_block / psx_icache_fetch / psx_cyc_step / psx_cyc_load_word
 * call verbatim -- this per-sector hot path arms CD command deadlines and
 * the +1200 chan+28 polls depend on real guest-cycle accounting (the
 * stripped variant livelocked the 0x801A1Fxx queue service). Channel
 * struct base 0x801BCB80 (alt 0x801BCB88/0x801BCBA0 ring cells), flag
 * 0x801BC7B4 bit0 selects sync_wait_c vs sync_wait_b, tail jalr through
 * *0x801BC7B0 cb(a0=5|2, s1). Guest frame sp-=48 saves s0/s1/ra;
 * 38 continuation addrs. */
uint32_t cd_read_step_cb_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80198634u: goto block_80198634;
            case 0x80198640u: goto block_80198640;
            case 0x80198648u: goto block_80198648;
            case 0x80198654u: goto block_80198654;
            case 0x80198664u: goto block_80198664;
            case 0x8019866Cu: goto block_8019866C;
            case 0x80198690u: goto block_80198690;
            case 0x801986CCu: goto block_801986CC;
            case 0x801986ECu: goto block_801986EC;
            case 0x80198734u: goto block_80198734;
            case 0x80198758u: goto block_80198758;
            case 0x80198760u: goto block_80198760;
            case 0x80198794u: goto block_80198794;
            case 0x801987B8u: goto block_801987B8;
            case 0x801987DCu: goto block_801987DC;
            case 0x801987E8u: goto block_801987E8;
            case 0x801987F4u: goto block_801987F4;
            case 0x80198824u: goto block_80198824;
            case 0x801985F4u: goto block_801985F4;
            case 0x80198604u: goto block_80198604;
            case 0x80198614u: goto block_80198614;
            case 0x8019862Cu: goto block_8019862C;
            case 0x8019865Cu: goto block_8019865C;
            case 0x80198684u: goto block_80198684;
            case 0x8019869Cu: goto block_8019869C;
            case 0x801986B4u: goto block_801986B4;
            case 0x801986D4u: goto block_801986D4;
            case 0x80198728u: goto block_80198728;
            case 0x8019872Cu: goto block_8019872C;
            case 0x80198750u: goto block_80198750;
            case 0x80198778u: goto block_80198778;
            case 0x8019877Cu: goto block_8019877C;
            case 0x8019878Cu: goto block_8019878C;
            case 0x801987ACu: goto block_801987AC;
            case 0x801987D0u: goto block_801987D0;
            case 0x80198808u: goto block_80198808;
            case 0x80198818u: goto block_80198818;
            case 0x8019881Cu: goto block_8019881C;
            default: break;
        }
    }
    /* Address: 0x801985C4, Size: 628 bytes, Blocks: 39 */

block_801985C4:
    if (psx_slice_block(cpu, 0x801985C4u, 12u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801985C4u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 48;  /* 0x801985C4: 0x27BDFFD0 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 36, s1);  /* 0x801985C8: 0xAFB10024 */
    psx_cyc_step(cpu, 0x20021u);
    s1 = a1;  /* 0x801985CC: 0x00A08821 */
    psx_icache_fetch(cpu, 0x801985D0u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801985D0: 0x3C03801C */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 13440;  /* 0x801985D4: 0x2463CB80 */
    psx_cyc_step(cpu, 0x10u);
    a0 &= 0x00FF;  /* 0x801985D8: 0x308400FF */
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x801985DC: 0x24020001 */
    psx_icache_fetch(cpu, 0x801985E0u);
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 40, ra);  /* 0x801985E0: 0xAFBF0028 */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 32, s0);  /* 0x801985E4: 0xAFB00020 */
    psx_cyc_step(cpu, 0x20008u);
    cpu->write_word(v1 + 52, s1);  /* 0x801985E8: 0xAC710034 */
    psx_cyc_step(cpu, 0x14u);
    int cond_801985EC = (a0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801985F0u);
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x801985F0: 0x2402FFFF */
    if (cond_801985EC) {
        psx_check_interrupts_at(cpu, 0x80198728u);
        goto block_80198728;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801985F4u);
    }

block_801985F4:
    if (psx_slice_block(cpu, 0x801985F4u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801985F4u);
    v0 = psx_cyc_load_word(cpu, v1 + 20, 2, 0x8u);  /* 0x801985F4: 0x8C620014 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801985F8: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_801985FC = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198600u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198600: 0x00000000 */
    if (cond_801985FC) {
        psx_check_interrupts_at(cpu, 0x8019872Cu);
        goto block_8019872C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198604u);
    }

block_80198604:
    if (psx_slice_block(cpu, 0x80198604u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198604u);
    v1 = psx_cyc_load_word(cpu, v1 + 16, 3, 0x8u);  /* 0x80198604: 0x8C630010 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 512;  /* 0x80198608: 0x24020200 */
    psx_cyc_step(cpu, 0xCu);
    int cond_8019860C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198610u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198610: 0x00000000 */
    if (cond_8019860C) {
        psx_check_interrupts_at(cpu, 0x8019869Cu);
        goto block_8019869C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198614u);
    }

block_80198614:
    if (psx_slice_block(cpu, 0x80198614u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198614u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198614: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14412, 2, 0x4u);  /* 0x80198618: 0x8C42C7B4 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019861C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80198620u);
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0001;  /* 0x80198620: 0x30420001 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198624 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000010u);
    a0 = sp + 16;  /* 0x80198628: 0x27A40010 */
    if (cond_80198624) {
        psx_check_interrupts_at(cpu, 0x8019865Cu);
        goto block_8019865C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019862Cu);
    }

block_8019862C:
    if (psx_slice_block(cpu, 0x8019862Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019862Cu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198634u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198630u);
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80198630: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return cpu->gpr[2];  /* CPS jal -> cd_cmd_dispatch3 (0x801961A8) */

block_80198634:
    if (psx_slice_block(cpu, 0x80198634u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198634u);
    psx_cyc_step(cpu, 0x20000010u);
    a0 = sp + 16;  /* 0x80198634: 0x27A40010 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198640u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x21u);
    a1 = 3;  /* 0x8019863C: 0x24050003 */
    psx_check_interrupts_at(cpu, 0x80196188u);
    cpu->pc = 0x80196188u; return cpu->gpr[2];  /* CPS jal -> cd_sync_wait_c (0x80196188) */

block_80198640:
    if (psx_slice_block(cpu, 0x80198640u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198640u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198648u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80198644: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x801961CCu);
    cpu->pc = 0x801961CCu; return cpu->gpr[2];  /* CPS jal -> cd_busy_wait_fwd (0x801961CC) */

block_80198648:
    if (psx_slice_block(cpu, 0x80198648u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198648u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x801A0000u;  /* 0x801A0000 */  /* 0x80198648: 0x3C04801A */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198654u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198650u);
    psx_cyc_step(cpu, 0x10u);
    a0 -= 30664;  /* 0x80198650: 0x24848838 */
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return cpu->gpr[2];  /* CPS jal -> cd_cmd_dispatch3 (0x801961A8) */

block_80198654:
    if (psx_slice_block(cpu, 0x80198654u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198654u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198658: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198664u);
    goto block_80198664;  /* j */

block_8019865C:
    if (psx_slice_block(cpu, 0x8019865Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019865Cu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198664u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198660u);
    psx_cyc_step(cpu, 0x21u);
    a1 = 3;  /* 0x80198660: 0x24050003 */
    psx_check_interrupts_at(cpu, 0x80196168u);
    cpu->pc = 0x80196168u; return cpu->gpr[2];  /* CPS jal -> cd_sync_wait_b (0x80196168) */

block_80198664:
    if (psx_slice_block(cpu, 0x80198664u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198664u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x8019866Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000010u);
    a0 = sp + 16;  /* 0x80198668: 0x27A40010 */
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return cpu->gpr[2];  /* CPS jal -> cd_stamp_to_sector (0x801962F0) */

block_8019866C:
    if (psx_slice_block(cpu, 0x8019866Cu, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019866Cu);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8019866C: 0x3C10801C */
    psx_icache_fetch(cpu, 0x80198670u);
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13408;  /* 0x80198670: 0x2610CBA0 */
    v1 = psx_cyc_load_word(cpu, s0, 3, 0x10000u);  /* 0x80198674: 0x8E030000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198678: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    int cond_8019867C = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198680u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198680: 0x00000000 */
    if (cond_8019867C) {
        psx_check_interrupts_at(cpu, 0x8019869Cu);
        goto block_8019869C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198684u);
    }

block_80198684:
    if (psx_slice_block(cpu, 0x80198684u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198684u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x80198684: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198690u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 += 23240;  /* 0x8019868C: 0x24845AC8 */
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u; return cpu->gpr[2];  /* CPS jal -> cd_str_print (0x80197AD0) */

block_80198690:
    if (psx_slice_block(cpu, 0x80198690u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198690u);
    psx_cyc_step(cpu, 0x10008u);
    v1 = s0 + -32;  /* 0x80198690: 0x2603FFE0 */
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x80198694: 0x2402FFFF */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_word(v1 + 20, v0);  /* 0x80198698: 0xAC620014 */
    /* fall through to block_8019869C */

block_8019869C:
    if (psx_slice_block(cpu, 0x8019869Cu, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019869Cu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8019869C: 0x3C02801C */
    psx_icache_fetch(cpu, 0x801986A0u);
    v0 = psx_cyc_load_word(cpu, v0 + -14412, 2, 0x4u);  /* 0x801986A0: 0x8C42C7B4 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801986A4: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0001;  /* 0x801986A8: 0x30420001 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801986AC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801986B0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801986B0: 0x00000000 */
    if (cond_801986AC) {
        psx_check_interrupts_at(cpu, 0x801986D4u);
        goto block_801986D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801986B4u);
    }

block_801986B4:
    if (psx_slice_block(cpu, 0x801986B4u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801986B4u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801986B4: 0x3C02801C */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 13432;  /* 0x801986B8: 0x2442CB88 */
    a0 = psx_cyc_load_word(cpu, v0, 4, 0x4u);  /* 0x801986BC: 0x8C440000 */
    psx_icache_fetch(cpu, 0x801986C0u);
    a1 = psx_cyc_load_word(cpu, v0 + 8, 5, 0x4u);  /* 0x801986C0: 0x8C450008 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801986CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801986C8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80196188u);
    cpu->pc = 0x80196188u; return cpu->gpr[2];  /* CPS jal -> cd_sync_wait_c (0x80196188) */

block_801986CC:
    if (psx_slice_block(cpu, 0x801986CCu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801986CCu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801986D0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801986D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019872Cu);
    goto block_8019872C;  /* j */

block_801986D4:
    if (psx_slice_block(cpu, 0x801986D4u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801986D4u);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801986D4: 0x3C10801C */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13432;  /* 0x801986D8: 0x2610CB88 */
    a0 = psx_cyc_load_word(cpu, s0, 4, 0x10000u);  /* 0x801986DC: 0x8E040000 */
    psx_icache_fetch(cpu, 0x801986E0u);
    a1 = psx_cyc_load_word(cpu, s0 + 8, 5, 0x10000u);  /* 0x801986E0: 0x8E050008 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801986ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801986E8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80196168u);
    cpu->pc = 0x80196168u; return cpu->gpr[2];  /* CPS jal -> cd_sync_wait_b (0x80196168) */

block_801986EC:
    if (psx_slice_block(cpu, 0x801986ECu, 15u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801986ECu);
    psx_cyc_step(cpu, 0x10010u);
    a0 = s0 + -8;  /* 0x801986EC: 0x2604FFF8 */
    psx_icache_fetch(cpu, 0x801986F0u);
    v0 = psx_cyc_load_word(cpu, s0 + 8, 2, 0x10000u);  /* 0x801986F0: 0x8E020008 */
    v1 = psx_cyc_load_word(cpu, s0, 3, 0x10000u);  /* 0x801986F4: 0x8E030000 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801986F8: 0x00021080 */
    psx_cyc_step(cpu, 0xCu);
    v1 += v0;  /* 0x801986FC: 0x00621821 */
    psx_icache_fetch(cpu, 0x80198700u);
    psx_cyc_step(cpu, 0x18u);
    cpu->write_word(a0 + 8, v1);  /* 0x80198700: 0xAC830008 */
    v0 = psx_cyc_load_word(cpu, a0 + 20, 2, 0x10u);  /* 0x80198704: 0x8C820014 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198708: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0--;  /* 0x8019870C: 0x2442FFFF */
    psx_icache_fetch(cpu, 0x80198710u);
    psx_cyc_step(cpu, 0x14u);
    cpu->write_word(a0 + 20, v0);  /* 0x80198710: 0xAC820014 */
    v0 = psx_cyc_load_word(cpu, a0 + 32, 2, 0x10u);  /* 0x80198714: 0x8C820020 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198718: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x8019871C: 0x24420001 */
    psx_icache_fetch(cpu, 0x80198720u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x14u);
    cpu->write_word(a0 + 32, v0);  /* 0x80198724: 0xAC820020 */
    psx_check_interrupts_at(cpu, 0x8019872Cu);
    goto block_8019872C;  /* j */

block_80198728:
    if (psx_slice_block(cpu, 0x80198728u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198728u);
    psx_cyc_step(cpu, 0xCu);
    cpu->write_word(v1 + 20, v0);  /* 0x80198728: 0xAC620014 */
    /* fall through to block_8019872C */

block_8019872C:
    if (psx_slice_block(cpu, 0x8019872Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019872Cu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198734u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198730u);
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198730: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return cpu->gpr[2];  /* CPS jal -> cpst_8019a460 */

block_80198734:
    if (psx_slice_block(cpu, 0x80198734u, 7u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198734u);
    psx_cyc_step(cpu, 0x10000u);
    s0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80198734: 0x3C10801C */
    psx_cyc_step(cpu, 0x10000u);
    s0 -= 13440;  /* 0x80198738: 0x2610CB80 */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 24, v0);  /* 0x8019873C: 0xAE020018 */
    psx_icache_fetch(cpu, 0x80198740u);
    v0 = psx_cyc_load_word(cpu, s0 + 20, 2, 0x10000u);  /* 0x80198740: 0x8E020014 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198744: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_80198748 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019874C: 0x00000000 */
    if (cond_80198748) {
        psx_check_interrupts_at(cpu, 0x80198758u);
        goto block_80198758;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198750u);
    }

block_80198750:
    if (psx_slice_block(cpu, 0x80198750u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198750u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198758u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = 1;  /* 0x80198754: 0x24040001 */
    psx_check_interrupts_at(cpu, 0x80198908u);
    cpu->pc = 0x80198908u; return cpu->gpr[2];  /* CPS jal -> cd_read_open (0x80198908) */

block_80198758:
    if (psx_slice_block(cpu, 0x80198758u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198758u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198760u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x8019875C: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return cpu->gpr[2];  /* CPS jal -> cpst_8019a460 */

block_80198760:
    if (psx_slice_block(cpu, 0x80198760u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198760u);
    v1 = psx_cyc_load_word(cpu, s0 + 28, 3, 0x10000u);  /* 0x80198760: 0x8E03001C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198764: 0x00000000 */
    psx_cyc_step(cpu, 0x8u);
    v1 += 1200;  /* 0x80198768: 0x246304B0 */
    psx_cyc_step(cpu, 0xCu);
    v1 = (int32_t)v1 < (int32_t)v0;  /* 0x8019876C: 0x0062182A */
    psx_icache_fetch(cpu, 0x80198770u);
    psx_cyc_step(cpu, 0x9u);
    int cond_80198770 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x80198774: 0x2402FFFF */
    if (cond_80198770) {
        psx_check_interrupts_at(cpu, 0x8019877Cu);
        goto block_8019877C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198778u);
    }

block_80198778:
    if (psx_slice_block(cpu, 0x80198778u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198778u);
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 20, v0);  /* 0x80198778: 0xAE020014 */
    /* fall through to block_8019877C */

block_8019877C:
    if (psx_slice_block(cpu, 0x8019877Cu, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019877Cu);
    v0 = psx_cyc_load_word(cpu, s0 + 20, 2, 0x10000u);  /* 0x8019877C: 0x8E020014 */
    psx_icache_fetch(cpu, 0x80198780u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198780: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80198784 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198788: 0x00000000 */
    if (cond_80198784) {
        psx_check_interrupts_at(cpu, 0x801987ACu);
        goto block_801987AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019878Cu);
    }

block_8019878C:
    if (psx_slice_block(cpu, 0x8019878Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019878Cu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80198794u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198790u);
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80198790: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return cpu->gpr[2];  /* CPS jal -> cpst_8019a460 */

block_80198794:
    if (psx_slice_block(cpu, 0x80198794u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198794u);
    v1 = psx_cyc_load_word(cpu, s0 + 28, 3, 0x10000u);  /* 0x80198794: 0x8E03001C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80198798: 0x00000000 */
    psx_cyc_step(cpu, 0x8u);
    v1 += 1200;  /* 0x8019879C: 0x246304B0 */
    psx_icache_fetch(cpu, 0x801987A0u);
    psx_cyc_step(cpu, 0xCu);
    v1 = (int32_t)v1 < (int32_t)v0;  /* 0x801987A0: 0x0062182A */
    psx_cyc_step(cpu, 0x9u);
    int cond_801987A4 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801987A8: 0x00000000 */
    if (cond_801987A4) {
        psx_check_interrupts_at(cpu, 0x80198824u);
        goto block_80198824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801987ACu);
    }

block_801987AC:
    if (psx_slice_block(cpu, 0x801987ACu, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801987ACu);
    a0 = psx_cyc_load_word(cpu, s0 + 44, 4, 0x10000u);  /* 0x801987AC: 0x8E04002C */
    psx_icache_fetch(cpu, 0x801987B0u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801987B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801987B4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80195D78u);
    cpu->pc = 0x80195D78u; return cpu->gpr[2];  /* CPS jal -> mc_session_b (0x80195D78) */

block_801987B8:
    if (psx_slice_block(cpu, 0x801987B8u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801987B8u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801987B8: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14412, 2, 0x4u);  /* 0x801987BC: 0x8C42C7B4 */
    psx_icache_fetch(cpu, 0x801987C0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801987C0: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0001;  /* 0x801987C4: 0x30420001 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801987C8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801987CC: 0x00000000 */
    if (cond_801987C8) {
        psx_check_interrupts_at(cpu, 0x801987DCu);
        goto block_801987DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801987D0u);
    }

block_801987D0:
    if (psx_slice_block(cpu, 0x801987D0u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801987D0u);
    a0 = psx_cyc_load_word(cpu, s0 + 48, 4, 0x10000u);  /* 0x801987D0: 0x8E040030 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801987DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801987D8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return cpu->gpr[2];  /* CPS jal -> cd_cmd_dispatch3 (0x801961A8) */

block_801987DC:
    if (psx_slice_block(cpu, 0x801987DCu, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801987DCu);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x801A0000u;  /* 0x801A0000 */  /* 0x801987DC: 0x3C04801A */
    psx_icache_fetch(cpu, 0x801987E0u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801987E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 -= 31344;  /* 0x801987E4: 0x24848590 */
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return cpu->gpr[2];  /* CPS jal -> mc_session (0x80195D64) */

block_801987E8:
    if (psx_slice_block(cpu, 0x801987E8u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801987E8u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 9;  /* 0x801987E8: 0x24040009 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801987F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801987F0u);
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x801987F0: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return cpu->gpr[2];  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_801987F4:
    if (psx_slice_block(cpu, 0x801987F4u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801987F4u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801987F4: 0x3C03801C */
    v1 = psx_cyc_load_word(cpu, v1 + -14416, 3, 0x8u);  /* 0x801987F8: 0x8C63C7B0 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x801987FC: 0x24020001 */
    psx_icache_fetch(cpu, 0x80198800u);
    psx_cyc_step(cpu, 0x9u);
    int cond_80198800 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10004u);
    cpu->write_word(s0 + 36, v0);  /* 0x80198804: 0xAE020024 */
    if (cond_80198800) {
        psx_check_interrupts_at(cpu, 0x80198824u);
        goto block_80198824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198808u);
    }

block_80198808:
    if (psx_slice_block(cpu, 0x80198808u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198808u);
    v0 = psx_cyc_load_word(cpu, s0 + 20, 2, 0x10000u);  /* 0x80198808: 0x8E020014 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019880C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80198810u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80198810 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = 5;  /* 0x80198814: 0x24040005 */
    if (cond_80198810) {
        psx_check_interrupts_at(cpu, 0x8019881Cu);
        goto block_8019881C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198818u);
    }

block_80198818:
    if (psx_slice_block(cpu, 0x80198818u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198818u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 2;  /* 0x80198818: 0x24040002 */
    /* fall through to block_8019881C */

block_8019881C:
    if (psx_slice_block(cpu, 0x8019881Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019881Cu);
    psx_cyc_step(cpu, 0x80000008u);
    uint32_t jt_8019881C = v1;  /* latch indirect target before delay slot */
    ra = 0x80198824u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80198820u);
    psx_cyc_step(cpu, 0x20021u);
    a1 = s1;  /* 0x80198820: 0x02202821 */
    psx_check_interrupts_at(cpu, jt_8019881C);
    cpu->pc = jt_8019881C; return cpu->gpr[2];  /* CPS jalr */

block_80198824:
    if (psx_slice_block(cpu, 0x80198824u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80198824u);
    ra = psx_cyc_load_word(cpu, sp + 40, 31, 0x20000000u);  /* 0x80198824: 0x8FBF0028 */
    s1 = psx_cyc_load_word(cpu, sp + 36, 17, 0x20000000u);  /* 0x80198828: 0x8FB10024 */
    s0 = psx_cyc_load_word(cpu, sp + 32, 16, 0x20000000u);  /* 0x8019882C: 0x8FB00020 */
    psx_icache_fetch(cpu, 0x80198830u);
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_80198830 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 48;  /* 0x80198834: 0x27BD0030 */
    psx_check_interrupts_at(cpu, jt_80198830);
    cpu->pc = jt_80198830; return cpu->gpr[2];  /* CPS: jr $ra */
}
/* ======== cd_response_process_cps (guest 0x80196370) ========
 * CD response-int8_t state machine — IRQ2-context leaf pumped by
 * cd_response_pump / cd_result_wait_a/b. Timing-faithful port of the
 * cpst_80196370 CPS transliteration: every psx_cyc_step /
 * psx_cyc_load_* / psx_icache_fetch / psx_slice_block /
 * psx_check_interrupts_at kept verbatim; the cpu->pc resume-switch
 * coroutine contract is unchanged. The cd.c cd_response_process()
 * wrapper still forwards here via call_guest4 -> clean dispatch.
 * 70 CPS continuation addrs registered in clean_cps.map. */

uint32_t cd_response_process_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196554u: goto block_80196554;
            case 0x8019659Cu: goto block_8019659C;
            case 0x801968A4u: goto block_801968A4;
            case 0x801968B4u: goto block_801968B4;
            case 0x801963BCu: goto block_801963BC;
            case 0x801963C4u: goto block_801963C4;
            case 0x801963D4u: goto block_801963D4;
            case 0x801963E8u: goto block_801963E8;
            case 0x801963ECu: goto block_801963EC;
            case 0x8019640Cu: goto block_8019640C;
            case 0x80196430u: goto block_80196430;
            case 0x8019643Cu: goto block_8019643C;
            case 0x80196444u: goto block_80196444;
            case 0x80196458u: goto block_80196458;
            case 0x80196498u: goto block_80196498;
            case 0x801964C0u: goto block_801964C0;
            case 0x801964D8u: goto block_801964D8;
            case 0x801964ECu: goto block_801964EC;
            case 0x80196504u: goto block_80196504;
            case 0x80196524u: goto block_80196524;
            case 0x80196534u: goto block_80196534;
            case 0x80196548u: goto block_80196548;
            case 0x80196568u: goto block_80196568;
            case 0x801965B4u: goto block_801965B4;
            case 0x801965CCu: goto block_801965CC;
            case 0x801965D4u: goto block_801965D4;
            case 0x801965F0u: goto block_801965F0;
            case 0x801965F8u: goto block_801965F8;
            case 0x80196610u: goto block_80196610;
            case 0x80196618u: goto block_80196618;
            case 0x80196640u: goto block_80196640;
            case 0x8019665Cu: goto block_8019665C;
            case 0x80196664u: goto block_80196664;
            case 0x8019667Cu: goto block_8019667C;
            case 0x80196684u: goto block_80196684;
            case 0x801966A4u: goto block_801966A4;
            case 0x801966ACu: goto block_801966AC;
            case 0x801966C4u: goto block_801966C4;
            case 0x801966CCu: goto block_801966CC;
            case 0x801966D4u: goto block_801966D4;
            case 0x801966D8u: goto block_801966D8;
            case 0x801966F0u: goto block_801966F0;
            case 0x801966F8u: goto block_801966F8;
            case 0x80196710u: goto block_80196710;
            case 0x80196718u: goto block_80196718;
            case 0x80196720u: goto block_80196720;
            case 0x80196728u: goto block_80196728;
            case 0x8019672Cu: goto block_8019672C;
            case 0x80196734u: goto block_80196734;
            case 0x80196738u: goto block_80196738;
            case 0x80196754u: goto block_80196754;
            case 0x8019675Cu: goto block_8019675C;
            case 0x80196774u: goto block_80196774;
            case 0x80196798u: goto block_80196798;
            case 0x801967C4u: goto block_801967C4;
            case 0x801967C8u: goto block_801967C8;
            case 0x801967E0u: goto block_801967E0;
            case 0x801967F0u: goto block_801967F0;
            case 0x801967F8u: goto block_801967F8;
            case 0x80196810u: goto block_80196810;
            case 0x80196818u: goto block_80196818;
            case 0x80196844u: goto block_80196844;
            case 0x80196848u: goto block_80196848;
            case 0x80196860u: goto block_80196860;
            case 0x80196870u: goto block_80196870;
            case 0x80196878u: goto block_80196878;
            case 0x80196890u: goto block_80196890;
            case 0x80196898u: goto block_80196898;
            case 0x801968B8u: goto block_801968B8;
            default: break;
        }
    }
    /* Address: 0x80196370, Size: 1372 bytes, Blocks: 70 */

block_80196370:
    if (psx_slice_block(cpu, 0x80196370u, 19u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196370u);
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 48;  /* 0x80196370: 0x27BDFFD0 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196374: 0x3C03801C */
    v1 = psx_cyc_load_word(cpu, v1 + -13556, 3, 0x8u);  /* 0x80196378: 0x8C63CB0C */
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x8019637C: 0x24020001 */
    psx_icache_fetch(cpu, 0x80196380u);
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 40, ra);  /* 0x80196380: 0xAFBF0028 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 36, s1);  /* 0x80196384: 0xAFB10024 */
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 32, s0);  /* 0x80196388: 0xAFB00020 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x8019638C: 0xA0620000 */
    psx_icache_fetch(cpu, 0x80196390u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196390: 0x3C04801C */
    a0 = psx_cyc_load_word(cpu, a0 + -13544, 4, 0x10u);  /* 0x80196394: 0x8C84CB18 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196398: 0x00000000 */
    v0 = psx_cyc_load_byte(cpu, a0, 2, 0x10u);  /* 0x8019639C: 0x90820000 */
    psx_icache_fetch(cpu, 0x801963A0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801963A0: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0007;  /* 0x801963A4: 0x30420007 */
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_byte(sp + 16, (uint8_t)v0);  /* 0x801963A8: 0xA3A20010 */
    v0 = psx_cyc_load_byte(cpu, sp + 16, 2, 0x20000000u);  /* 0x801963AC: 0x93A20010 */
    psx_icache_fetch(cpu, 0x801963B0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801963B0: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801963B4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20001u);
    s1 = zero;  /* 0x801963B8: 0x00008821 */
    if (cond_801963B4) {
        psx_check_interrupts_at(cpu, 0x801968B4u);
        goto block_801968B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801963BCu);
    }

block_801963BC:
    if (psx_slice_block(cpu, 0x801963BCu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801963BCu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801963C0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801963C0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801963D4u);
    goto block_801963D4;  /* j */

block_801963C4:
    if (psx_slice_block(cpu, 0x801963C4u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801963C4u);
    v0 = psx_cyc_load_byte(cpu, a0, 2, 0x10u);  /* 0x801963C4: 0x90820000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801963C8: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0007;  /* 0x801963CC: 0x30420007 */
    psx_icache_fetch(cpu, 0x801963D0u);
    psx_cyc_step(cpu, 0x20000004u);
    cpu->write_byte(sp + 16, (uint8_t)v0);  /* 0x801963D0: 0xA3A20010 */
    /* fall through to block_801963D4 */

block_801963D4:
    if (psx_slice_block(cpu, 0x801963D4u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801963D4u);
    v0 = psx_cyc_load_byte(cpu, a0, 2, 0x10u);  /* 0x801963D4: 0x90820000 */
    v1 = psx_cyc_load_byte(cpu, sp + 16, 3, 0x20000000u);  /* 0x801963D8: 0x93A30010 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0007;  /* 0x801963DC: 0x30420007 */
    psx_icache_fetch(cpu, 0x801963E0u);
    psx_cyc_step(cpu, 0xCu);
    int cond_801963E0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10001u);
    s0 = zero;  /* 0x801963E4: 0x00008021 */
    if (cond_801963E0) {
        psx_check_interrupts_at(cpu, 0x801963C4u);
        goto block_801963C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801963E8u);
    }

block_801963E8:
    if (psx_slice_block(cpu, 0x801963E8u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801963E8u);
    psx_cyc_step(cpu, 0x20000010u);
    a0 = sp + 24;  /* 0x801963E8: 0x27A40018 */
    /* fall through to block_801963EC */

block_801963EC:
    if (psx_slice_block(cpu, 0x801963ECu, 8u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801963ECu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801963EC: 0x3C02801C */
    psx_icache_fetch(cpu, 0x801963F0u);
    v0 = psx_cyc_load_word(cpu, v0 + -13556, 2, 0x4u);  /* 0x801963F0: 0x8C42CB0C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801963F4: 0x00000000 */
    v0 = psx_cyc_load_byte(cpu, v0, 2, 0x4u);  /* 0x801963F8: 0x90420000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801963FC: 0x00000000 */
    psx_icache_fetch(cpu, 0x80196400u);
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0020;  /* 0x80196400: 0x30420020 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80196404 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10018u);
    v1 = a0 + s0;  /* 0x80196408: 0x00901821 */
    if (cond_80196404) {
        psx_check_interrupts_at(cpu, 0x80196430u);
        goto block_80196430;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019640Cu);
    }

block_8019640C:
    if (psx_slice_block(cpu, 0x8019640Cu, 9u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019640Cu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8019640C: 0x3C02801C */
    psx_icache_fetch(cpu, 0x80196410u);
    v0 = psx_cyc_load_word(cpu, v0 + -13552, 2, 0x4u);  /* 0x80196410: 0x8C42CB10 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196414: 0x00000000 */
    v0 = psx_cyc_load_byte(cpu, v0, 2, 0x4u);  /* 0x80196418: 0x90420000 */
    psx_cyc_step(cpu, 0x10000u);
    s0++;  /* 0x8019641C: 0x26100001 */
    psx_icache_fetch(cpu, 0x80196420u);
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196420: 0xA0620000 */
    psx_cyc_step(cpu, 0x10004u);
    v0 = (int32_t)s0 < 8;  /* 0x80196424: 0x2A020008 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80196428 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019642C: 0x00000000 */
    if (cond_80196428) {
        psx_check_interrupts_at(cpu, 0x801963ECu);
        goto block_801963EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196430u);
    }

block_80196430:
    if (psx_slice_block(cpu, 0x80196430u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196430u);
    psx_cyc_step(cpu, 0x10004u);
    v0 = (int32_t)s0 < 8;  /* 0x80196430: 0x2A020008 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80196434 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10009u);
    v1 = s0;  /* 0x80196438: 0x02001821 */
    if (cond_80196434) {
        psx_check_interrupts_at(cpu, 0x80196458u);
        goto block_80196458;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019643Cu);
    }

block_8019643C:
    if (psx_slice_block(cpu, 0x8019643Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019643Cu);
    psx_cyc_step(cpu, 0x20000010u);
    a0 = sp + 24;  /* 0x8019643C: 0x27A40018 */
    psx_icache_fetch(cpu, 0x80196440u);
    psx_cyc_step(cpu, 0x1Cu);
    v0 = a0 + v1;  /* 0x80196440: 0x00831021 */
    /* fall through to block_80196444 */

block_80196444:
    if (psx_slice_block(cpu, 0x80196444u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196444u);
    psx_cyc_step(cpu, 0x5u);
    cpu->write_byte(v0, (uint8_t)zero);  /* 0x80196444: 0xA0400000 */
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x80196448: 0x24630001 */
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v1 < 8;  /* 0x8019644C: 0x28620008 */
    psx_icache_fetch(cpu, 0x80196450u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80196450 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1Cu);
    v0 = a0 + v1;  /* 0x80196454: 0x00831021 */
    if (cond_80196450) {
        psx_check_interrupts_at(cpu, 0x80196444u);
        goto block_80196444;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196458u);
    }

block_80196458:
    if (psx_slice_block(cpu, 0x80196458u, 16u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196458u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196458: 0x3C03801C */
    v1 = psx_cyc_load_word(cpu, v1 + -13556, 3, 0x8u);  /* 0x8019645C: 0x8C63CB0C */
    psx_icache_fetch(cpu, 0x80196460u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x80196460: 0x24020001 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196464: 0xA0620000 */
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196468: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -13544, 2, 0x4u);  /* 0x8019646C: 0x8C42CB18 */
    psx_icache_fetch(cpu, 0x80196470u);
    psx_cyc_step(cpu, 0x9u);
    v1 = 7;  /* 0x80196470: 0x24030007 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v0, (uint8_t)v1);  /* 0x80196474: 0xA0430000 */
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196478: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -13548, 2, 0x4u);  /* 0x8019647C: 0x8C42CB14 */
    psx_icache_fetch(cpu, 0x80196480u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196480: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v0, (uint8_t)v1);  /* 0x80196484: 0xA0430000 */
    v1 = psx_cyc_load_byte(cpu, sp + 16, 3, 0x20000000u);  /* 0x80196488: 0x93A30010 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 3;  /* 0x8019648C: 0x24020003 */
    psx_icache_fetch(cpu, 0x80196490u);
    psx_cyc_step(cpu, 0xCu);
    int cond_80196490 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196494: 0x00000000 */
    if (cond_80196490) {
        psx_check_interrupts_at(cpu, 0x801964C0u);
        goto block_801964C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196498u);
    }

block_80196498:
    if (psx_slice_block(cpu, 0x80196498u, 10u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196498u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196498: 0x3C02801C */
    v0 = psx_cyc_load_byte(cpu, v0 + -14235, 2, 0x4u);  /* 0x8019649C: 0x9042C865 */
    psx_icache_fetch(cpu, 0x801964A0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964A0: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801964A4: 0x00021080 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x801964A8: 0x3C01801C */
    psx_cyc_step(cpu, 0x6u);
    at += v0;  /* 0x801964AC: 0x00220821 */
    psx_icache_fetch(cpu, 0x801964B0u);
    v0 = psx_cyc_load_word(cpu, at + -13812, 2, 0x2u);  /* 0x801964B0: 0x8C22CA0C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964B4: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801964B8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964BC: 0x00000000 */
    if (cond_801964B8) {
        psx_check_interrupts_at(cpu, 0x80196524u);
        goto block_80196524;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801964C0u);
    }

block_801964C0:
    if (psx_slice_block(cpu, 0x801964C0u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801964C0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801964C0: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14252, 2, 0x4u);  /* 0x801964C4: 0x8C42C854 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964C8: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0010;  /* 0x801964CC: 0x30420010 */
    psx_icache_fetch(cpu, 0x801964D0u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801964D0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964D4: 0x00000000 */
    if (cond_801964D0) {
        psx_check_interrupts_at(cpu, 0x80196504u);
        goto block_80196504;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801964D8u);
    }

block_801964D8:
    if (psx_slice_block(cpu, 0x801964D8u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801964D8u);
    v0 = psx_cyc_load_byte(cpu, sp + 24, 2, 0x20000000u);  /* 0x801964D8: 0x93A20018 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964DC: 0x00000000 */
    psx_icache_fetch(cpu, 0x801964E0u);
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x0010;  /* 0x801964E0: 0x30420010 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801964E4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964E8: 0x00000000 */
    if (cond_801964E4) {
        psx_check_interrupts_at(cpu, 0x80196504u);
        goto block_80196504;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801964ECu);
    }

block_801964EC:
    if (psx_slice_block(cpu, 0x801964ECu, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801964ECu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801964EC: 0x3C02801C */
    psx_icache_fetch(cpu, 0x801964F0u);
    v0 = psx_cyc_load_word(cpu, v0 + -14244, 2, 0x4u);  /* 0x801964F0: 0x8C42C85C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801964F4: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x801964F8: 0x24420001 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x801964FC: 0x3C01801C */
    psx_icache_fetch(cpu, 0x80196500u);
    psx_cyc_step(cpu, 0x6u);
    cpu->write_word(at + -14244, v0);  /* 0x80196500: 0xAC22C85C */
    /* fall through to block_80196504 */

block_80196504:
    if (psx_slice_block(cpu, 0x80196504u, 8u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196504u);
    v0 = psx_cyc_load_byte(cpu, sp + 24, 2, 0x20000000u);  /* 0x80196504: 0x93A20018 */
    v1 = psx_cyc_load_byte(cpu, sp + 25, 3, 0x20000000u);  /* 0x80196508: 0x93A30019 */
    psx_cyc_step(cpu, 0x4u);
    v0 &= 0x00FF;  /* 0x8019650C: 0x304200FF */
    psx_icache_fetch(cpu, 0x80196510u);
    psx_cyc_step(cpu, 0x20004u);
    s1 = v0 & 0x001D;  /* 0x80196510: 0x3051001D */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196514: 0x3C01801C */
    psx_cyc_step(cpu, 0x6u);
    cpu->write_word(at + -14252, v0);  /* 0x80196518: 0xAC22C854 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x8019651C: 0x3C01801C */
    psx_icache_fetch(cpu, 0x80196520u);
    psx_cyc_step(cpu, 0xAu);
    cpu->write_word(at + -14248, v1);  /* 0x80196520: 0xAC23C858 */
    /* fall through to block_80196524 */

block_80196524:
    if (psx_slice_block(cpu, 0x80196524u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196524u);
    v1 = psx_cyc_load_byte(cpu, sp + 16, 3, 0x20000000u);  /* 0x80196524: 0x93A30010 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 5;  /* 0x80196528: 0x24020005 */
    psx_cyc_step(cpu, 0xCu);
    int cond_8019652C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196530u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196530: 0x00000000 */
    if (cond_8019652C) {
        psx_check_interrupts_at(cpu, 0x8019659Cu);
        goto block_8019659C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196534u);
    }

block_80196534:
    if (psx_slice_block(cpu, 0x80196534u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196534u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196534: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14256, 2, 0x4u);  /* 0x80196538: 0x8C42C850 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019653C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80196540u);
    psx_cyc_step(cpu, 0x4u);
    int cond_80196540 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196544: 0x00000000 */
    if (cond_80196540) {
        psx_check_interrupts_at(cpu, 0x8019659Cu);
        goto block_8019659C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196548u);
    }

block_80196548:
    if (psx_slice_block(cpu, 0x80196548u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196548u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x80196548: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80196554u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196550u);
    psx_cyc_step(cpu, 0x10u);
    a0 += 22480;  /* 0x80196550: 0x248457D0 */
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return cpu->gpr[2];  /* CPS jal -> printf_forward_guest (0x8018E550) */

block_80196554:
    if (psx_slice_block(cpu, 0x80196554u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196554u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196554: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14256, 2, 0x4u);  /* 0x80196558: 0x8C42C850 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019655C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80196560u);
    psx_cyc_step(cpu, 0x4u);
    int cond_80196560 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196564: 0x00000000 */
    if (cond_80196560) {
        psx_check_interrupts_at(cpu, 0x8019659Cu);
        goto block_8019659C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196568u);
    }

block_80196568:
    if (psx_slice_block(cpu, 0x80196568u, 13u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196568u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196568: 0x3C02801C */
    v0 = psx_cyc_load_byte(cpu, v0 + -14235, 2, 0x4u);  /* 0x8019656C: 0x9042C865 */
    psx_icache_fetch(cpu, 0x80196570u);
    psx_cyc_step(cpu, 0x40u);
    a2 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196570: 0x3C06801C */
    a2 = psx_cyc_load_word(cpu, a2 + -14252, 6, 0x40u);  /* 0x80196574: 0x8CC6C854 */
    psx_cyc_step(cpu, 0x80u);
    a3 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196578: 0x3C07801C */
    a3 = psx_cyc_load_word(cpu, a3 + -14248, 7, 0x80u);  /* 0x8019657C: 0x8CE7C858 */
    psx_icache_fetch(cpu, 0x80196580u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x80196580: 0x00021080 */
    psx_cyc_step(cpu, 0x20u);
    a1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196584: 0x3C05801C */
    psx_cyc_step(cpu, 0x24u);
    a1 += v0;  /* 0x80196588: 0x00A22821 */
    a1 = psx_cyc_load_word(cpu, a1 + -14228, 5, 0x20u);  /* 0x8019658C: 0x8CA5C86C */
    psx_icache_fetch(cpu, 0x80196590u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x80196590: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x8019659Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 += 22492;  /* 0x80196598: 0x248457DC */
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return cpu->gpr[2];  /* CPS jal -> printf_forward_guest (0x8018E550) */

block_8019659C:
    if (psx_slice_block(cpu, 0x8019659Cu, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019659Cu);
    v0 = psx_cyc_load_byte(cpu, sp + 16, 2, 0x20000000u);  /* 0x8019659C: 0x93A20010 */
    psx_icache_fetch(cpu, 0x801965A0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801965A0: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v1 = v0 + -1;  /* 0x801965A4: 0x2443FFFF */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 < (uint32_t)5;  /* 0x801965A8: 0x2C620005 */
    psx_cyc_step(cpu, 0x5u);
    int cond_801965AC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801965B0u);
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 << 2;  /* 0x801965B0: 0x00031080 */
    if (cond_801965AC) {
        psx_check_interrupts_at(cpu, 0x80196898u);
        goto block_80196898;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801965B4u);
    }

block_801965B4:
    if (psx_slice_block(cpu, 0x801965B4u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801965B4u);
    psx_cyc_step(cpu, 0x2u);
    at = 0x80120000u;  /* 0x80120000 */  /* 0x801965B4: 0x3C018012 */
    psx_cyc_step(cpu, 0x6u);
    at += v0;  /* 0x801965B8: 0x00220821 */
    v0 = psx_cyc_load_word(cpu, at + 22552, 2, 0x2u);  /* 0x801965BC: 0x8C225818 */
    psx_icache_fetch(cpu, 0x801965C0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801965C0: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    uint32_t jt_801965C4 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801965C8: 0x00000000 */
    psx_check_interrupts_at(cpu, jt_801965C4);
    cpu->pc = jt_801965C4; return cpu->gpr[2];  /* CPS: jr v0 */

block_801965CC:
    if (psx_slice_block(cpu, 0x801965CCu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801965CCu);
    psx_cyc_step(cpu, 0x20001u);
    int cond_801965CC = (s1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801965D0u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 5;  /* 0x801965D0: 0x24020005 */
    if (cond_801965CC) {
        psx_check_interrupts_at(cpu, 0x80196618u);
        goto block_80196618;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801965D4u);
    }

block_801965D4:
    if (psx_slice_block(cpu, 0x801965D4u, 7u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801965D4u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801965D4: 0x3C03801C */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 13532;  /* 0x801965D8: 0x2463CB24 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x801965DC: 0xA0620000 */
    psx_icache_fetch(cpu, 0x801965E0u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x801965E0: 0x3C038020 */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 14648;  /* 0x801965E4: 0x2463C6C8 */
    psx_cyc_step(cpu, 0x9u);
    int cond_801965E8 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x801965EC: 0x27A50018 */
    if (cond_801965E8) {
        psx_check_interrupts_at(cpu, 0x80196710u);
        goto block_80196710;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801965F0u);
    }

block_801965F0:
    if (psx_slice_block(cpu, 0x801965F0u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801965F0u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 7;  /* 0x801965F0: 0x24040007 */
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x801965F4: 0x2406FFFF */
    /* fall through to block_801965F8 */

block_801965F8:
    if (psx_slice_block(cpu, 0x801965F8u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801965F8u);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x801965F8: 0x90A20000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x801965FC: 0x24A50001 */
    psx_icache_fetch(cpu, 0x80196600u);
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x80196600: 0x2484FFFF */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196604: 0xA0620000 */
    psx_cyc_step(cpu, 0x50u);
    int cond_80196608 = (a0 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x8019660C: 0x24630001 */
    if (cond_80196608) {
        psx_check_interrupts_at(cpu, 0x801965F8u);
        goto block_801965F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196610u);
    }

block_80196610:
    if (psx_slice_block(cpu, 0x80196610u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196610u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 2;  /* 0x80196614: 0x24020002 */
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196618:
    if (psx_slice_block(cpu, 0x80196618u, 10u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196618u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196618: 0x3C02801C */
    v0 = psx_cyc_load_byte(cpu, v0 + -14235, 2, 0x4u);  /* 0x8019661C: 0x9042C865 */
    psx_icache_fetch(cpu, 0x80196620u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196620: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x80196624: 0x00021080 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196628: 0x3C01801C */
    psx_cyc_step(cpu, 0x6u);
    at += v0;  /* 0x8019662C: 0x00220821 */
    psx_icache_fetch(cpu, 0x80196630u);
    v0 = psx_cyc_load_word(cpu, at + -14068, 2, 0x2u);  /* 0x80196630: 0x8C22C90C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196634: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80196638 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 3;  /* 0x8019663C: 0x24020003 */
    if (cond_80196638) {
        psx_check_interrupts_at(cpu, 0x80196684u);
        goto block_80196684;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196640u);
    }

block_80196640:
    if (psx_slice_block(cpu, 0x80196640u, 7u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196640u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196640: 0x3C03801C */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 13532;  /* 0x80196644: 0x2463CB24 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196648: 0xA0620000 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x8019664C: 0x3C038020 */
    psx_icache_fetch(cpu, 0x80196650u);
    psx_cyc_step(cpu, 0x8u);
    v1 -= 14648;  /* 0x80196650: 0x2463C6C8 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80196654 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x80196658: 0x27A50018 */
    if (cond_80196654) {
        psx_check_interrupts_at(cpu, 0x8019667Cu);
        goto block_8019667C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019665Cu);
    }

block_8019665C:
    if (psx_slice_block(cpu, 0x8019665Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019665Cu);
    psx_cyc_step(cpu, 0x11u);
    a0 = 7;  /* 0x8019665C: 0x24040007 */
    psx_icache_fetch(cpu, 0x80196660u);
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x80196660: 0x2406FFFF */
    /* fall through to block_80196664 */

block_80196664:
    if (psx_slice_block(cpu, 0x80196664u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196664u);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x80196664: 0x90A20000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x80196668: 0x24A50001 */
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x8019666C: 0x2484FFFF */
    psx_icache_fetch(cpu, 0x80196670u);
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196670: 0xA0620000 */
    psx_cyc_step(cpu, 0x50u);
    int cond_80196674 = (a0 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x80196678: 0x24630001 */
    if (cond_80196674) {
        psx_check_interrupts_at(cpu, 0x80196664u);
        goto block_80196664;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019667Cu);
    }

block_8019667C:
    if (psx_slice_block(cpu, 0x8019667Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019667Cu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196680u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x80196680: 0x24020001 */
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196684:
    if (psx_slice_block(cpu, 0x80196684u, 8u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196684u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196684: 0x3C03801C */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 13532;  /* 0x80196688: 0x2463CB24 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 2;  /* 0x8019668C: 0x24020002 */
    psx_icache_fetch(cpu, 0x80196690u);
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196690: 0xA0620000 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x80196694: 0x3C038020 */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 14648;  /* 0x80196698: 0x2463C6C8 */
    psx_cyc_step(cpu, 0x9u);
    int cond_8019669C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801966A0u);
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x801966A0: 0x27A50018 */
    if (cond_8019669C) {
        psx_check_interrupts_at(cpu, 0x80196710u);
        goto block_80196710;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966A4u);
    }

block_801966A4:
    if (psx_slice_block(cpu, 0x801966A4u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966A4u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 7;  /* 0x801966A4: 0x24040007 */
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x801966A8: 0x2406FFFF */
    /* fall through to block_801966AC */

block_801966AC:
    if (psx_slice_block(cpu, 0x801966ACu, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966ACu);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x801966AC: 0x90A20000 */
    psx_icache_fetch(cpu, 0x801966B0u);
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x801966B0: 0x24A50001 */
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x801966B4: 0x2484FFFF */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x801966B8: 0xA0620000 */
    psx_cyc_step(cpu, 0x50u);
    int cond_801966BC = (a0 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801966C0u);
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x801966C0: 0x24630001 */
    if (cond_801966BC) {
        psx_check_interrupts_at(cpu, 0x801966ACu);
        goto block_801966AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966C4u);
    }

block_801966C4:
    if (psx_slice_block(cpu, 0x801966C4u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966C4u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 2;  /* 0x801966C8: 0x24020002 */
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_801966CC:
    if (psx_slice_block(cpu, 0x801966CCu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966CCu);
    psx_cyc_step(cpu, 0x20001u);
    int cond_801966CC = (s1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801966D0u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 2;  /* 0x801966D0: 0x24020002 */
    if (cond_801966CC) {
        psx_check_interrupts_at(cpu, 0x801966D8u);
        goto block_801966D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966D4u);
    }

block_801966D4:
    if (psx_slice_block(cpu, 0x801966D4u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966D4u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 5;  /* 0x801966D4: 0x24020005 */
    /* fall through to block_801966D8 */

block_801966D8:
    if (psx_slice_block(cpu, 0x801966D8u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966D8u);
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x801966D8: 0x3C01801C */
    psx_cyc_step(cpu, 0x6u);
    cpu->write_byte(at + -13532, (uint8_t)v0);  /* 0x801966DC: 0xA022CB24 */
    psx_icache_fetch(cpu, 0x801966E0u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x801966E0: 0x3C038020 */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 14648;  /* 0x801966E4: 0x2463C6C8 */
    psx_cyc_step(cpu, 0x9u);
    int cond_801966E8 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x801966EC: 0x27A50018 */
    if (cond_801966E8) {
        psx_check_interrupts_at(cpu, 0x80196710u);
        goto block_80196710;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966F0u);
    }

block_801966F0:
    if (psx_slice_block(cpu, 0x801966F0u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966F0u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 7;  /* 0x801966F0: 0x24040007 */
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x801966F4: 0x2406FFFF */
    /* fall through to block_801966F8 */

block_801966F8:
    if (psx_slice_block(cpu, 0x801966F8u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801966F8u);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x801966F8: 0x90A20000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x801966FC: 0x24A50001 */
    psx_icache_fetch(cpu, 0x80196700u);
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x80196700: 0x2484FFFF */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196704: 0xA0620000 */
    psx_cyc_step(cpu, 0x50u);
    int cond_80196708 = (a0 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x8019670C: 0x24630001 */
    if (cond_80196708) {
        psx_check_interrupts_at(cpu, 0x801966F8u);
        goto block_801966F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196710u);
    }

block_80196710:
    if (psx_slice_block(cpu, 0x80196710u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196710u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 2;  /* 0x80196714: 0x24020002 */
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196718:
    if (psx_slice_block(cpu, 0x80196718u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196718u);
    psx_cyc_step(cpu, 0x20001u);
    int cond_80196718 = (s1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 1;  /* 0x8019671C: 0x24020001 */
    if (cond_80196718) {
        psx_check_interrupts_at(cpu, 0x8019672Cu);
        goto block_8019672C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196720u);
    }

block_80196720:
    if (psx_slice_block(cpu, 0x80196720u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196720u);
    psx_cyc_step(cpu, 0x10004u);
    int cond_80196720 = (s0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196724: 0x00000000 */
    if (cond_80196720) {
        psx_check_interrupts_at(cpu, 0x8019672Cu);
        goto block_8019672C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196728u);
    }

block_80196728:
    if (psx_slice_block(cpu, 0x80196728u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196728u);
    psx_cyc_step(cpu, 0x20001u);
    s1 = zero;  /* 0x80196728: 0x00008821 */
    /* fall through to block_8019672C */

block_8019672C:
    if (psx_slice_block(cpu, 0x8019672Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019672Cu);
    psx_cyc_step(cpu, 0x20001u);
    int cond_8019672C = (s1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196730u);
    psx_cyc_step(cpu, 0x9u);
    v1 = 1;  /* 0x80196730: 0x24030001 */
    if (cond_8019672C) {
        psx_check_interrupts_at(cpu, 0x80196738u);
        goto block_80196738;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196734u);
    }

block_80196734:
    if (psx_slice_block(cpu, 0x80196734u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196734u);
    psx_cyc_step(cpu, 0x9u);
    v1 = 5;  /* 0x80196734: 0x24030005 */
    /* fall through to block_80196738 */

block_80196738:
    if (psx_slice_block(cpu, 0x80196738u, 7u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196738u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196738: 0x3C02801C */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 13532;  /* 0x8019673C: 0x2442CB24 */
    psx_icache_fetch(cpu, 0x80196740u);
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v0 + 1, (uint8_t)v1);  /* 0x80196740: 0xA0430001 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x80196744: 0x3C038020 */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 14640;  /* 0x80196748: 0x2463C6D0 */
    psx_cyc_step(cpu, 0x9u);
    int cond_8019674C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196750u);
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x80196750: 0x27A50018 */
    if (cond_8019674C) {
        psx_check_interrupts_at(cpu, 0x80196774u);
        goto block_80196774;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196754u);
    }

block_80196754:
    if (psx_slice_block(cpu, 0x80196754u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196754u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 7;  /* 0x80196754: 0x24040007 */
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x80196758: 0x2406FFFF */
    /* fall through to block_8019675C */

block_8019675C:
    if (psx_slice_block(cpu, 0x8019675Cu, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019675Cu);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x8019675C: 0x90A20000 */
    psx_icache_fetch(cpu, 0x80196760u);
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x80196760: 0x24A50001 */
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x80196764: 0x2484FFFF */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196768: 0xA0620000 */
    psx_cyc_step(cpu, 0x50u);
    int cond_8019676C = (a0 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196770u);
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x80196770: 0x24630001 */
    if (cond_8019676C) {
        psx_check_interrupts_at(cpu, 0x8019675Cu);
        goto block_8019675C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196774u);
    }

block_80196774:
    if (psx_slice_block(cpu, 0x80196774u, 9u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196774u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196774: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -13556, 2, 0x4u);  /* 0x80196778: 0x8C42CB0C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019677C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80196780u);
    psx_cyc_step(cpu, 0x5u);
    cpu->write_byte(v0, (uint8_t)zero);  /* 0x80196780: 0xA0400000 */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196784: 0x3C03801C */
    v1 = psx_cyc_load_word(cpu, v1 + -13544, 3, 0x8u);  /* 0x80196788: 0x8C63CB18 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 4;  /* 0x8019678C: 0x24020004 */
    psx_icache_fetch(cpu, 0x80196790u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x9u);
    cpu->write_byte(v1, (uint8_t)zero);  /* 0x80196794: 0xA0600000 */
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196798:
    if (psx_slice_block(cpu, 0x80196798u, 11u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196798u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80200000u;  /* 0x80200000 */  /* 0x80196798: 0x3C048020 */
    psx_cyc_step(cpu, 0x10u);
    a0 -= 14632;  /* 0x8019679C: 0x2484C6D8 */
    psx_icache_fetch(cpu, 0x801967A0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801967A0: 0x3C02801C */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 13532;  /* 0x801967A4: 0x2442CB24 */
    psx_cyc_step(cpu, 0x9u);
    v1 = 4;  /* 0x801967A8: 0x24030004 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v0 + 2, (uint8_t)v1);  /* 0x801967AC: 0xA0430002 */
    psx_icache_fetch(cpu, 0x801967B0u);
    v1 = psx_cyc_load_byte(cpu, v0 + 2, 3, 0x4u);  /* 0x801967B0: 0x90430002 */
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x801967B4: 0x27A50018 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v0 + 1, (uint8_t)v1);  /* 0x801967B8: 0xA0430001 */
    psx_cyc_step(cpu, 0x11u);
    int cond_801967BC = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801967C0u);
    psx_cyc_step(cpu, 0x9u);
    v1 = 7;  /* 0x801967C0: 0x24030007 */
    if (cond_801967BC) {
        psx_check_interrupts_at(cpu, 0x801967E0u);
        goto block_801967E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801967C4u);
    }

block_801967C4:
    if (psx_slice_block(cpu, 0x801967C4u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801967C4u);
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x801967C4: 0x2406FFFF */
    /* fall through to block_801967C8 */

block_801967C8:
    if (psx_slice_block(cpu, 0x801967C8u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801967C8u);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x801967C8: 0x90A20000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x801967CC: 0x24A50001 */
    psx_icache_fetch(cpu, 0x801967D0u);
    psx_cyc_step(cpu, 0x8u);
    v1--;  /* 0x801967D0: 0x2463FFFF */
    psx_cyc_step(cpu, 0x14u);
    cpu->write_byte(a0, (uint8_t)v0);  /* 0x801967D4: 0xA0820000 */
    psx_cyc_step(cpu, 0x48u);
    int cond_801967D8 = (v1 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0++;  /* 0x801967DC: 0x24840001 */
    if (cond_801967D8) {
        psx_check_interrupts_at(cpu, 0x801967C8u);
        goto block_801967C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801967E0u);
    }

block_801967E0:
    if (psx_slice_block(cpu, 0x801967E0u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801967E0u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x801967E0: 0x3C038020 */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 14640;  /* 0x801967E4: 0x2463C6D0 */
    psx_cyc_step(cpu, 0x9u);
    int cond_801967E8 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x801967EC: 0x27A50018 */
    if (cond_801967E8) {
        psx_check_interrupts_at(cpu, 0x80196810u);
        goto block_80196810;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801967F0u);
    }

block_801967F0:
    if (psx_slice_block(cpu, 0x801967F0u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801967F0u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 7;  /* 0x801967F0: 0x24040007 */
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x801967F4: 0x2406FFFF */
    /* fall through to block_801967F8 */

block_801967F8:
    if (psx_slice_block(cpu, 0x801967F8u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801967F8u);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x801967F8: 0x90A20000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x801967FC: 0x24A50001 */
    psx_icache_fetch(cpu, 0x80196800u);
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x80196800: 0x2484FFFF */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196804: 0xA0620000 */
    psx_cyc_step(cpu, 0x50u);
    int cond_80196808 = (a0 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x8019680C: 0x24630001 */
    if (cond_80196808) {
        psx_check_interrupts_at(cpu, 0x801967F8u);
        goto block_801967F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196810u);
    }

block_80196810:
    if (psx_slice_block(cpu, 0x80196810u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196810u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 4;  /* 0x80196814: 0x24020004 */
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196818:
    if (psx_slice_block(cpu, 0x80196818u, 11u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196818u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80200000u;  /* 0x80200000 */  /* 0x80196818: 0x3C048020 */
    psx_cyc_step(cpu, 0x10u);
    a0 -= 14648;  /* 0x8019681C: 0x2484C6C8 */
    psx_icache_fetch(cpu, 0x80196820u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196820: 0x3C02801C */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 13532;  /* 0x80196824: 0x2442CB24 */
    psx_cyc_step(cpu, 0x9u);
    v1 = 5;  /* 0x80196828: 0x24030005 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v0 + 1, (uint8_t)v1);  /* 0x8019682C: 0xA0430001 */
    psx_icache_fetch(cpu, 0x80196830u);
    v1 = psx_cyc_load_byte(cpu, v0 + 1, 3, 0x4u);  /* 0x80196830: 0x90430001 */
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x80196834: 0x27A50018 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v0, (uint8_t)v1);  /* 0x80196838: 0xA0430000 */
    psx_cyc_step(cpu, 0x11u);
    int cond_8019683C = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196840u);
    psx_cyc_step(cpu, 0x9u);
    v1 = 7;  /* 0x80196840: 0x24030007 */
    if (cond_8019683C) {
        psx_check_interrupts_at(cpu, 0x80196860u);
        goto block_80196860;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196844u);
    }

block_80196844:
    if (psx_slice_block(cpu, 0x80196844u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196844u);
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x80196844: 0x2406FFFF */
    /* fall through to block_80196848 */

block_80196848:
    if (psx_slice_block(cpu, 0x80196848u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196848u);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x80196848: 0x90A20000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x8019684C: 0x24A50001 */
    psx_icache_fetch(cpu, 0x80196850u);
    psx_cyc_step(cpu, 0x8u);
    v1--;  /* 0x80196850: 0x2463FFFF */
    psx_cyc_step(cpu, 0x14u);
    cpu->write_byte(a0, (uint8_t)v0);  /* 0x80196854: 0xA0820000 */
    psx_cyc_step(cpu, 0x48u);
    int cond_80196858 = (v1 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0++;  /* 0x8019685C: 0x24840001 */
    if (cond_80196858) {
        psx_check_interrupts_at(cpu, 0x80196848u);
        goto block_80196848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196860u);
    }

block_80196860:
    if (psx_slice_block(cpu, 0x80196860u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196860u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x80196860: 0x3C038020 */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 14640;  /* 0x80196864: 0x2463C6D0 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80196868 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000020u);
    a1 = sp + 24;  /* 0x8019686C: 0x27A50018 */
    if (cond_80196868) {
        psx_check_interrupts_at(cpu, 0x80196890u);
        goto block_80196890;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196870u);
    }

block_80196870:
    if (psx_slice_block(cpu, 0x80196870u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196870u);
    psx_cyc_step(cpu, 0x11u);
    a0 = 7;  /* 0x80196870: 0x24040007 */
    psx_cyc_step(cpu, 0x41u);
    a2 = -1;  /* 0x80196874: 0x2406FFFF */
    /* fall through to block_80196878 */

block_80196878:
    if (psx_slice_block(cpu, 0x80196878u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196878u);
    v0 = psx_cyc_load_byte(cpu, a1, 2, 0x20u);  /* 0x80196878: 0x90A20000 */
    psx_cyc_step(cpu, 0x20u);
    a1++;  /* 0x8019687C: 0x24A50001 */
    psx_icache_fetch(cpu, 0x80196880u);
    psx_cyc_step(cpu, 0x10u);
    a0--;  /* 0x80196880: 0x2484FFFF */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196884: 0xA0620000 */
    psx_cyc_step(cpu, 0x50u);
    int cond_80196888 = (a0 != a2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x8u);
    v1++;  /* 0x8019688C: 0x24630001 */
    if (cond_80196888) {
        psx_check_interrupts_at(cpu, 0x80196878u);
        goto block_80196878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196890u);
    }

block_80196890:
    if (psx_slice_block(cpu, 0x80196890u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196890u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 6;  /* 0x80196894: 0x24020006 */
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196898:
    if (psx_slice_block(cpu, 0x80196898u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196898u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x80196898: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801968A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801968A0u);
    psx_cyc_step(cpu, 0x10u);
    a0 += 22520;  /* 0x801968A0: 0x248457F8 */
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u; return cpu->gpr[2];  /* CPS jal -> cd_str_print (0x80197AD0) */

block_801968A4:
    if (psx_slice_block(cpu, 0x801968A4u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801968A4u);
    a1 = psx_cyc_load_byte(cpu, sp + 16, 5, 0x20000000u);  /* 0x801968A4: 0x93A50010 */
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x801968A8: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801968B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801968B0u);
    psx_cyc_step(cpu, 0x10u);
    a0 += 22540;  /* 0x801968B0: 0x2484580C */
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return cpu->gpr[2];  /* CPS jal -> printf_forward_guest (0x8018E550) */

block_801968B4:
    if (psx_slice_block(cpu, 0x801968B4u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801968B4u);
    psx_cyc_step(cpu, 0x5u);
    v0 = zero;  /* 0x801968B4: 0x00001021 */
    /* fall through to block_801968B8 */

block_801968B8:
    if (psx_slice_block(cpu, 0x801968B8u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801968B8u);
    ra = psx_cyc_load_word(cpu, sp + 40, 31, 0x20000000u);  /* 0x801968B8: 0x8FBF0028 */
    s1 = psx_cyc_load_word(cpu, sp + 36, 17, 0x20000000u);  /* 0x801968BC: 0x8FB10024 */
    psx_icache_fetch(cpu, 0x801968C0u);
    s0 = psx_cyc_load_word(cpu, sp + 32, 16, 0x20000000u);  /* 0x801968C0: 0x8FB00020 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_801968C4 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 48;  /* 0x801968C8: 0x27BD0030 */
    psx_check_interrupts_at(cpu, jt_801968C4);
    cpu->pc = jt_801968C4; return cpu->gpr[2];  /* CPS: jr $ra */
}


/* ======== cd_cmd_issue_cps (guest 0x80196E14) ========
 * CD command issue + synchronous result wait — the load-bearing state
 * machine behind the cd.c cd_cmd_issue() thunk (which forwards via
 * call_guest4 -> clean dispatch). Timing-faithful port of the
 * cpst_80196e14 CPS transliteration: all psx_cyc_* / psx_icache_fetch /
 * psx_slice_block / psx_check_interrupts_at calls verbatim, cpu->pc
 * resume-switch contract preserved. 53 CPS continuation addrs
 * registered in clean_cps.map. */

uint32_t cd_cmd_issue_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return 0;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196E78u: goto block_80196E78;
            case 0x80196ECCu: goto block_80196ECC;
            case 0x80196EDCu: goto block_80196EDC;
            case 0x80196FD8u: goto block_80196FD8;
            case 0x80197030u: goto block_80197030;
            case 0x80197080u: goto block_80197080;
            case 0x801970CCu: goto block_801970CC;
            case 0x801970D4u: goto block_801970D4;
            case 0x801970F0u: goto block_801970F0;
            case 0x80197118u: goto block_80197118;
            case 0x80197150u: goto block_80197150;
            case 0x80197180u: goto block_80197180;
            case 0x80196E58u: goto block_80196E58;
            case 0x80196E98u: goto block_80196E98;
            case 0x80196EA0u: goto block_80196EA0;
            case 0x80196EB4u: goto block_80196EB4;
            case 0x80196ED4u: goto block_80196ED4;
            case 0x80196EECu: goto block_80196EEC;
            case 0x80196EF4u: goto block_80196EF4;
            case 0x80196F14u: goto block_80196F14;
            case 0x80196F1Cu: goto block_80196F1C;
            case 0x80196F24u: goto block_80196F24;
            case 0x80196F30u: goto block_80196F30;
            case 0x80196F58u: goto block_80196F58;
            case 0x80196F5Cu: goto block_80196F5C;
            case 0x80196F84u: goto block_80196F84;
            case 0x80196F8Cu: goto block_80196F8C;
            case 0x80196FB4u: goto block_80196FB4;
            case 0x80196FD0u: goto block_80196FD0;
            case 0x80197010u: goto block_80197010;
            case 0x80197028u: goto block_80197028;
            case 0x80197048u: goto block_80197048;
            case 0x80197074u: goto block_80197074;
            case 0x801970DCu: goto block_801970DC;
            case 0x801970E0u: goto block_801970E0;
            case 0x801970E8u: goto block_801970E8;
            case 0x801970F8u: goto block_801970F8;
            case 0x80197110u: goto block_80197110;
            case 0x80197124u: goto block_80197124;
            case 0x8019712Cu: goto block_8019712C;
            case 0x80197140u: goto block_80197140;
            case 0x80197154u: goto block_80197154;
            case 0x8019715Cu: goto block_8019715C;
            case 0x80197170u: goto block_80197170;
            case 0x80197188u: goto block_80197188;
            case 0x80197198u: goto block_80197198;
            case 0x801971A8u: goto block_801971A8;
            case 0x801971B8u: goto block_801971B8;
            case 0x801971BCu: goto block_801971BC;
            case 0x801971D4u: goto block_801971D4;
            case 0x801971F0u: goto block_801971F0;
            case 0x801971F8u: goto block_801971F8;
            default: break;
        }
    }
    /* Address: 0x80196E14, Size: 1036 bytes, Blocks: 53 */

block_80196E14:
    if (psx_slice_block(cpu, 0x80196E14u, 17u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196E14u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196E14: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14256, 2, 0x4u);  /* 0x80196E18: 0x8C42C850 */
    psx_cyc_step(cpu, 0x20000000u);
    sp -= 56;  /* 0x80196E1C: 0x27BDFFC8 */
    psx_icache_fetch(cpu, 0x80196E20u);
    psx_cyc_step(cpu, 0x20010000u);
    cpu->write_word(sp + 24, s0);  /* 0x80196E20: 0xAFB00018 */
    psx_cyc_step(cpu, 0x10021u);
    s0 = a1;  /* 0x80196E24: 0x00A08021 */
    psx_cyc_step(cpu, 0x20400000u);
    cpu->write_word(sp + 48, s6);  /* 0x80196E28: 0xAFB60030 */
    psx_cyc_step(cpu, 0x400041u);
    s6 = a2;  /* 0x80196E2C: 0x00C0B021 */
    psx_icache_fetch(cpu, 0x80196E30u);
    psx_cyc_step(cpu, 0x20040000u);
    cpu->write_word(sp + 32, s2);  /* 0x80196E30: 0xAFB20020 */
    psx_cyc_step(cpu, 0x40081u);
    s2 = a3;  /* 0x80196E34: 0x00E09021 */
    psx_cyc_step(cpu, 0x20020000u);
    cpu->write_word(sp + 28, s1);  /* 0x80196E38: 0xAFB1001C */
    psx_cyc_step(cpu, 0x20011u);
    s1 = a0;  /* 0x80196E3C: 0x00808821 */
    psx_icache_fetch(cpu, 0x80196E40u);
    psx_cyc_step(cpu, 0xA0000000u);
    cpu->write_word(sp + 52, ra);  /* 0x80196E40: 0xAFBF0034 */
    psx_cyc_step(cpu, 0x20200000u);
    cpu->write_word(sp + 44, s5);  /* 0x80196E44: 0xAFB5002C */
    psx_cyc_step(cpu, 0x20100000u);
    cpu->write_word(sp + 40, s4);  /* 0x80196E48: 0xAFB40028 */
    psx_cyc_step(cpu, 0x4u);
    v0 = (int32_t)v0 < 2;  /* 0x80196E4C: 0x28420002 */
    psx_icache_fetch(cpu, 0x80196E50u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80196E50 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20080000u);
    cpu->write_word(sp + 36, s3);  /* 0x80196E54: 0xAFB30024 */
    if (cond_80196E50) {
        psx_check_interrupts_at(cpu, 0x80196E78u);
        goto block_80196E78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196E58u);
    }

block_80196E58:
    if (psx_slice_block(cpu, 0x80196E58u, 8u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196E58u);
    psx_cyc_step(cpu, 0x20004u);
    v0 = s1 & 0x00FF;  /* 0x80196E58: 0x322200FF */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x80196E5C: 0x00021080 */
    psx_icache_fetch(cpu, 0x80196E60u);
    psx_cyc_step(cpu, 0x20u);
    a1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196E60: 0x3C05801C */
    psx_cyc_step(cpu, 0x24u);
    a1 += v0;  /* 0x80196E64: 0x00A22821 */
    a1 = psx_cyc_load_word(cpu, a1 + -14228, 5, 0x20u);  /* 0x80196E68: 0x8CA5C86C */
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x80196E6C: 0x3C048012 */
    psx_icache_fetch(cpu, 0x80196E70u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80196E78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 += 22592;  /* 0x80196E74: 0x24845840 */
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return cpu->gpr[2];  /* CPS jal -> printf_forward_guest (0x8018E550) */

block_80196E78:
    if (psx_slice_block(cpu, 0x80196E78u, 8u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196E78u);
    psx_cyc_step(cpu, 0x20004u);
    v0 = s1 & 0x00FF;  /* 0x80196E78: 0x322200FF */
    psx_cyc_step(cpu, 0xCu);
    v1 = v0 << 2;  /* 0x80196E7C: 0x00021880 */
    psx_icache_fetch(cpu, 0x80196E80u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196E80: 0x3C02801C */
    psx_cyc_step(cpu, 0xCu);
    v0 += v1;  /* 0x80196E84: 0x00431021 */
    v0 = psx_cyc_load_word(cpu, v0 + -13684, 2, 0x4u);  /* 0x80196E88: 0x8C42CA8C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196E8C: 0x00000000 */
    psx_icache_fetch(cpu, 0x80196E90u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80196E90 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80196E94: 0x00002021 */
    if (cond_80196E90) {
        psx_check_interrupts_at(cpu, 0x80196ED4u);
        goto block_80196ED4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196E98u);
    }

block_80196E98:
    if (psx_slice_block(cpu, 0x80196E98u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196E98u);
    psx_cyc_step(cpu, 0x10001u);
    int cond_80196E98 = (s0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196E9C: 0x00000000 */
    if (cond_80196E98) {
        psx_check_interrupts_at(cpu, 0x80196ED4u);
        goto block_80196ED4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196EA0u);
    }

block_80196EA0:
    if (psx_slice_block(cpu, 0x80196EA0u, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196EA0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196EA0: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -14256, 2, 0x4u);  /* 0x80196EA4: 0x8C42C850 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196EA8: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_80196EAC = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196EB0u);
    psx_cyc_step(cpu, 0x5u);
    v0 = -2;  /* 0x80196EB0: 0x2402FFFE */
    if (cond_80196EAC) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196EB4u);
    }

block_80196EB4:
    if (psx_slice_block(cpu, 0x80196EB4u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196EB4u);
    psx_cyc_step(cpu, 0x20u);
    a1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196EB4: 0x3C05801C */
    psx_cyc_step(cpu, 0x28u);
    a1 += v1;  /* 0x80196EB8: 0x00A32821 */
    a1 = psx_cyc_load_word(cpu, a1 + -14228, 5, 0x20u);  /* 0x80196EBC: 0x8CA5C86C */
    psx_icache_fetch(cpu, 0x80196EC0u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x80196EC0: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80196ECCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 += 22600;  /* 0x80196EC8: 0x24845848 */
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return cpu->gpr[2];  /* CPS jal -> printf_forward_guest (0x8018E550) */

block_80196ECC:
    if (psx_slice_block(cpu, 0x80196ECCu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196ECCu);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196ED0u);
    psx_cyc_step(cpu, 0x5u);
    v0 = -2;  /* 0x80196ED0: 0x2402FFFE */
    psx_check_interrupts_at(cpu, 0x801971F8u);
    goto block_801971F8;  /* j */

block_80196ED4:
    if (psx_slice_block(cpu, 0x80196ED4u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196ED4u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80196EDCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x21u);
    a1 = zero;  /* 0x80196ED8: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x801968CCu);
    cpu->pc = 0x801968CCu; return cpu->gpr[2];  /* CPS jal -> cpst_801968cc */

block_80196EDC:
    if (psx_slice_block(cpu, 0x80196EDCu, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196EDCu);
    psx_cyc_step(cpu, 0x20008u);
    v1 = s1 & 0x00FF;  /* 0x80196EDC: 0x322300FF */
    psx_icache_fetch(cpu, 0x80196EE0u);
    psx_cyc_step(cpu, 0x5u);
    v0 = 2;  /* 0x80196EE0: 0x24020002 */
    psx_cyc_step(cpu, 0xCu);
    int cond_80196EE4 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = 14;  /* 0x80196EE8: 0x2402000E */
    if (cond_80196EE4) {
        psx_check_interrupts_at(cpu, 0x80196F1Cu);
        goto block_80196F1C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196EECu);
    }

block_80196EEC:
    if (psx_slice_block(cpu, 0x80196EECu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196EECu);
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80196EEC: 0x00002021 */
    psx_icache_fetch(cpu, 0x80196EF0u);
    psx_cyc_step(cpu, 0x10014u);
    v0 = s0 + a0;  /* 0x80196EF0: 0x02041021 */
    /* fall through to block_80196EF4 */

block_80196EF4:
    if (psx_slice_block(cpu, 0x80196EF4u, 8u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196EF4u);
    v0 = psx_cyc_load_byte(cpu, v0, 2, 0x4u);  /* 0x80196EF4: 0x90420000 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196EF8: 0x3C01801C */
    psx_cyc_step(cpu, 0x12u);
    at += a0;  /* 0x80196EFC: 0x00240821 */
    psx_icache_fetch(cpu, 0x80196F00u);
    psx_cyc_step(cpu, 0x6u);
    cpu->write_byte(at + -14240, (uint8_t)v0);  /* 0x80196F00: 0xA022C860 */
    psx_cyc_step(cpu, 0x10u);
    a0++;  /* 0x80196F04: 0x24840001 */
    psx_cyc_step(cpu, 0x14u);
    v0 = (int32_t)a0 < 4;  /* 0x80196F08: 0x28820004 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80196F0C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196F10u);
    psx_cyc_step(cpu, 0x10014u);
    v0 = s0 + a0;  /* 0x80196F10: 0x02041021 */
    if (cond_80196F0C) {
        psx_check_interrupts_at(cpu, 0x80196EF4u);
        goto block_80196EF4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F14u);
    }

block_80196F14:
    if (psx_slice_block(cpu, 0x80196F14u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F14u);
    psx_cyc_step(cpu, 0x20008u);
    v1 = s1 & 0x00FF;  /* 0x80196F14: 0x322300FF */
    psx_cyc_step(cpu, 0x5u);
    v0 = 14;  /* 0x80196F18: 0x2402000E */
    /* fall through to block_80196F1C */

block_80196F1C:
    if (psx_slice_block(cpu, 0x80196F1Cu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F1Cu);
    psx_cyc_step(cpu, 0xCu);
    int cond_80196F1C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196F20u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196F20: 0x00000000 */
    if (cond_80196F1C) {
        psx_check_interrupts_at(cpu, 0x80196F30u);
        goto block_80196F30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F24u);
    }

block_80196F24:
    if (psx_slice_block(cpu, 0x80196F24u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F24u);
    v0 = psx_cyc_load_byte(cpu, s0, 2, 0x10000u);  /* 0x80196F24: 0x92020000 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196F28: 0x3C01801C */
    psx_cyc_step(cpu, 0x6u);
    cpu->write_byte(at + -14236, (uint8_t)v0);  /* 0x80196F2C: 0xA022C864 */
    /* fall through to block_80196F30 */

block_80196F30:
    if (psx_slice_block(cpu, 0x80196F30u, 10u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F30u);
    psx_cyc_step(cpu, 0x20u);
    a1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196F30: 0x3C05801C */
    psx_cyc_step(cpu, 0x20u);
    a1 -= 13532;  /* 0x80196F34: 0x24A5CB24 */
    psx_cyc_step(cpu, 0x18u);
    a0 = v1 << 2;  /* 0x80196F38: 0x00032080 */
    psx_cyc_step(cpu, 0x21u);
    cpu->write_byte(a1, (uint8_t)zero);  /* 0x80196F3C: 0xA0A00000 */
    psx_icache_fetch(cpu, 0x80196F40u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196F40: 0x3C02801C */
    psx_cyc_step(cpu, 0x14u);
    v0 += a0;  /* 0x80196F44: 0x00441021 */
    v0 = psx_cyc_load_word(cpu, v0 + -13940, 2, 0x4u);  /* 0x80196F48: 0x8C42C98C */
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196F4C: 0x3C03801C */
    psx_icache_fetch(cpu, 0x80196F50u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80196F50 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x8u);
    v1 -= 13940;  /* 0x80196F54: 0x2463C98C */
    if (cond_80196F50) {
        psx_check_interrupts_at(cpu, 0x80196F5Cu);
        goto block_80196F5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F58u);
    }

block_80196F58:
    if (psx_slice_block(cpu, 0x80196F58u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F58u);
    psx_cyc_step(cpu, 0x21u);
    cpu->write_byte(a1 + 1, (uint8_t)zero);  /* 0x80196F58: 0xA0A00001 */
    /* fall through to block_80196F5C */

block_80196F5C:
    if (psx_slice_block(cpu, 0x80196F5Cu, 10u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F5Cu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196F5C: 0x3C02801C */
    psx_icache_fetch(cpu, 0x80196F60u);
    v0 = psx_cyc_load_word(cpu, v0 + -13556, 2, 0x4u);  /* 0x80196F60: 0x8C42CB0C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196F64: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    cpu->write_byte(v0, (uint8_t)zero);  /* 0x80196F68: 0xA0400000 */
    psx_cyc_step(cpu, 0xCu);
    v0 = v1 + 256;  /* 0x80196F6C: 0x24620100 */
    psx_icache_fetch(cpu, 0x80196F70u);
    psx_cyc_step(cpu, 0x1Cu);
    v1 = a0 + v0;  /* 0x80196F70: 0x00821821 */
    v0 = psx_cyc_load_word(cpu, v1, 2, 0x8u);  /* 0x80196F74: 0x8C620000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196F78: 0x00000000 */
    psx_cyc_step(cpu, 0x4u);
    int cond_80196F7C = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196F80u);
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x80196F80: 0x00002021 */
    if (cond_80196F7C) {
        psx_check_interrupts_at(cpu, 0x80196FB4u);
        goto block_80196FB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F84u);
    }

block_80196F84:
    if (psx_slice_block(cpu, 0x80196F84u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F84u);
    psx_cyc_step(cpu, 0x29u);
    a1 = v1;  /* 0x80196F84: 0x00602821 */
    psx_cyc_step(cpu, 0x10014u);
    v0 = s0 + a0;  /* 0x80196F88: 0x02041021 */
    /* fall through to block_80196F8C */

block_80196F8C:
    if (psx_slice_block(cpu, 0x80196F8Cu, 10u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196F8Cu);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196F8C: 0x3C03801C */
    psx_icache_fetch(cpu, 0x80196F90u);
    v1 = psx_cyc_load_word(cpu, v1 + -13548, 3, 0x8u);  /* 0x80196F90: 0x8C63CB14 */
    v0 = psx_cyc_load_byte(cpu, v0, 2, 0x4u);  /* 0x80196F94: 0x90420000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80196F98: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    cpu->write_byte(v1, (uint8_t)v0);  /* 0x80196F9C: 0xA0620000 */
    psx_icache_fetch(cpu, 0x80196FA0u);
    v0 = psx_cyc_load_word(cpu, a1, 2, 0x20u);  /* 0x80196FA0: 0x8CA20000 */
    psx_cyc_step(cpu, 0x10u);
    a0++;  /* 0x80196FA4: 0x24840001 */
    psx_cyc_step(cpu, 0x14u);
    v0 = (int32_t)a0 < (int32_t)v0;  /* 0x80196FA8: 0x0082102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_80196FAC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80196FB0u);
    psx_cyc_step(cpu, 0x10014u);
    v0 = s0 + a0;  /* 0x80196FB0: 0x02041021 */
    if (cond_80196FAC) {
        psx_check_interrupts_at(cpu, 0x80196F8Cu);
        goto block_80196F8C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196FB4u);
    }

block_80196FB4:
    if (psx_slice_block(cpu, 0x80196FB4u, 7u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196FB4u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196FB4: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -13552, 2, 0x4u);  /* 0x80196FB8: 0x8C42CB10 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196FBC: 0x3C01801C */
    psx_icache_fetch(cpu, 0x80196FC0u);
    psx_cyc_step(cpu, 0x20002u);
    cpu->write_byte(at + -14235, (uint8_t)s1);  /* 0x80196FC0: 0xA031C865 */
    psx_cyc_step(cpu, 0x20004u);
    cpu->write_byte(v0, (uint8_t)s1);  /* 0x80196FC4: 0xA0510000 */
    psx_cyc_step(cpu, 0x40001u);
    int cond_80196FC8 = (s2 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = zero;  /* 0x80196FCC: 0x00001021 */
    if (cond_80196FC8) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196FD0u);
    }

block_80196FD0:
    if (psx_slice_block(cpu, 0x80196FD0u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196FD0u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80196FD8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x80196FD4: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return cpu->gpr[2];  /* CPS jal -> cpst_8019a460 */

block_80196FD8:
    if (psx_slice_block(cpu, 0x80196FD8u, 14u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80196FD8u);
    psx_cyc_step(cpu, 0x4u);
    v0 += 960;  /* 0x80196FD8: 0x244203C0 */
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80196FDC: 0x3C04801C */
    psx_icache_fetch(cpu, 0x80196FE0u);
    psx_cyc_step(cpu, 0x10u);
    a0 -= 13532;  /* 0x80196FE0: 0x2484CB24 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x80200000u;  /* 0x80200000 */  /* 0x80196FE4: 0x3C018020 */
    psx_cyc_step(cpu, 0x6u);
    cpu->write_word(at + -14624, v0);  /* 0x80196FE8: 0xAC22C6E0 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x80200000u;  /* 0x80200000 */  /* 0x80196FEC: 0x3C018020 */
    psx_icache_fetch(cpu, 0x80196FF0u);
    psx_cyc_step(cpu, 0x3u);
    cpu->write_word(at + -14620, zero);  /* 0x80196FF0: 0xAC20C6E4 */
    v1 = psx_cyc_load_byte(cpu, a0, 3, 0x10u);  /* 0x80196FF4: 0x90830000 */
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x80196FF8: 0x3C028012 */
    psx_cyc_step(cpu, 0x4u);
    v0 += 22616;  /* 0x80196FFC: 0x24425858 */
    psx_icache_fetch(cpu, 0x80197000u);
    psx_cyc_step(cpu, 0x2u);
    at = 0x80200000u;  /* 0x80200000 */  /* 0x80197000: 0x3C018020 */
    psx_cyc_step(cpu, 0x6u);
    cpu->write_word(at + -14616, v0);  /* 0x80197004: 0xAC22C6E8 */
    psx_cyc_step(cpu, 0x9u);
    int cond_80197008 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x400041u);
    a2 = s6;  /* 0x8019700C: 0x02C03021 */
    if (cond_80197008) {
        psx_check_interrupts_at(cpu, 0x801971A8u);
        goto block_801971A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197010u);
    }

block_80197010:
    if (psx_slice_block(cpu, 0x80197010u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197010u);
    psx_cyc_step(cpu, 0x200000u);
    s5 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80197010: 0x3C15801C */
    psx_cyc_step(cpu, 0x200000u);
    s5 -= 14228;  /* 0x80197014: 0x26B5C86C */
    psx_cyc_step(cpu, 0x80000u);
    s3 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80197018: 0x3C13801C */
    psx_cyc_step(cpu, 0x80000u);
    s3 -= 14100;  /* 0x8019701C: 0x2673C8EC */
    psx_icache_fetch(cpu, 0x80197020u);
    psx_cyc_step(cpu, 0x40011u);
    s2 = a0;  /* 0x80197020: 0x00809021 */
    psx_cyc_step(cpu, 0x140000u);
    s4 = s2 + 1;  /* 0x80197024: 0x26540001 */
    /* fall through to block_80197028 */

block_80197028:
    if (psx_slice_block(cpu, 0x80197028u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197028u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80197030u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x8019702C: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return cpu->gpr[2];  /* CPS jal -> cpst_8019a460 */

block_80197030:
    if (psx_slice_block(cpu, 0x80197030u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197030u);
    psx_cyc_step(cpu, 0x8u);
    v1 = 0x80200000u;  /* 0x80200000 */  /* 0x80197030: 0x3C038020 */
    v1 = psx_cyc_load_word(cpu, v1 + -14624, 3, 0x8u);  /* 0x80197034: 0x8C63C6E0 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197038: 0x00000000 */
    psx_cyc_step(cpu, 0xCu);
    v1 = (int32_t)v1 < (int32_t)v0;  /* 0x8019703C: 0x0062182A */
    psx_icache_fetch(cpu, 0x80197040u);
    psx_cyc_step(cpu, 0x9u);
    int cond_80197040 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197044: 0x00000000 */
    if (cond_80197040) {
        psx_check_interrupts_at(cpu, 0x80197074u);
        goto block_80197074;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197048u);
    }

block_80197048:
    if (psx_slice_block(cpu, 0x80197048u, 11u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197048u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x80200000u;  /* 0x80200000 */  /* 0x80197048: 0x3C028020 */
    v0 = psx_cyc_load_word(cpu, v0 + -14620, 2, 0x4u);  /* 0x8019704C: 0x8C42C6E4 */
    psx_icache_fetch(cpu, 0x80197050u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197050: 0x00000000 */
    psx_cyc_step(cpu, 0xDu);
    v1 = v0;  /* 0x80197054: 0x00401821 */
    psx_cyc_step(cpu, 0x4u);
    v0++;  /* 0x80197058: 0x24420001 */
    psx_cyc_step(cpu, 0x2u);
    at = 0x80200000u;  /* 0x80200000 */  /* 0x8019705C: 0x3C018020 */
    psx_icache_fetch(cpu, 0x80197060u);
    psx_cyc_step(cpu, 0x6u);
    cpu->write_word(at + -14620, v0);  /* 0x80197060: 0xAC22C6E4 */
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x003C0000u;  /* 0x003C0000 */  /* 0x80197064: 0x3C02003C */
    psx_cyc_step(cpu, 0xCu);
    v0 = (int32_t)v0 < (int32_t)v1;  /* 0x80197068: 0x0043102A */
    psx_cyc_step(cpu, 0x5u);
    int cond_8019706C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80197070u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197070: 0x00000000 */
    if (cond_8019706C) {
        psx_check_interrupts_at(cpu, 0x801970DCu);
        goto block_801970DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197074u);
    }

block_80197074:
    if (psx_slice_block(cpu, 0x80197074u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197074u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x80197074: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80197080u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 += 22436;  /* 0x8019707C: 0x248457A4 */
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u; return cpu->gpr[2];  /* CPS jal -> cd_str_print (0x80197AD0) */

block_80197080:
    if (psx_slice_block(cpu, 0x80197080u, 19u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197080u);
    a0 = psx_cyc_load_byte(cpu, s2, 4, 0x40000u);  /* 0x80197080: 0x92440000 */
    v0 = psx_cyc_load_byte(cpu, s2 + 1, 2, 0x40000u);  /* 0x80197084: 0x92420001 */
    psx_cyc_step(cpu, 0x20u);
    a1 = 0x80200000u;  /* 0x80200000 */  /* 0x80197088: 0x3C058020 */
    a1 = psx_cyc_load_word(cpu, a1 + -14616, 5, 0x20u);  /* 0x8019708C: 0x8CA5C6E8 */
    psx_icache_fetch(cpu, 0x80197090u);
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x80197090: 0x00021080 */
    psx_cyc_step(cpu, 0x80004u);
    v0 += s3;  /* 0x80197094: 0x00531021 */
    psx_cyc_step(cpu, 0x10u);
    a0 <<= 2;  /* 0x80197098: 0x00042080 */
    v1 = psx_cyc_load_word(cpu, v0, 3, 0x4u);  /* 0x8019709C: 0x8C430000 */
    psx_icache_fetch(cpu, 0x801970A0u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801970A0: 0x3C02801C */
    v0 = psx_cyc_load_byte(cpu, v0 + -14235, 2, 0x4u);  /* 0x801970A4: 0x9042C865 */
    psx_cyc_step(cpu, 0x80010u);
    a0 += s3;  /* 0x801970A8: 0x00932021 */
    psx_cyc_step(cpu, 0x4u);
    v0 <<= 2;  /* 0x801970AC: 0x00021080 */
    psx_icache_fetch(cpu, 0x801970B0u);
    psx_cyc_step(cpu, 0x200004u);
    v0 += s5;  /* 0x801970B0: 0x00551021 */
    psx_cyc_step(cpu, 0x20000008u);
    cpu->write_word(sp + 16, v1);  /* 0x801970B4: 0xAFA30010 */
    a2 = psx_cyc_load_word(cpu, v0, 6, 0x4u);  /* 0x801970B8: 0x8C460000 */
    a3 = psx_cyc_load_word(cpu, a0, 7, 0x10u);  /* 0x801970BC: 0x8C870000 */
    psx_icache_fetch(cpu, 0x801970C0u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80120000u;  /* 0x80120000 */  /* 0x801970C0: 0x3C048012 */
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801970CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10u);
    a0 += 22452;  /* 0x801970C8: 0x248457B4 */
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return cpu->gpr[2];  /* CPS jal -> printf_forward_guest (0x8018E550) */

block_801970CC:
    if (psx_slice_block(cpu, 0x801970CCu, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801970CCu);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801970D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801970D0u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801970D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801972A8u);
    cpu->pc = 0x801972A8u; return cpu->gpr[2];  /* CPS jal -> cd_cmd_state_reset (0x801972A8) */

block_801970D4:
    if (psx_slice_block(cpu, 0x801970D4u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801970D4u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x801970D8: 0x2402FFFF */
    psx_check_interrupts_at(cpu, 0x801970E0u);
    goto block_801970E0;  /* j */

block_801970DC:
    if (psx_slice_block(cpu, 0x801970DCu, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801970DCu);
    psx_cyc_step(cpu, 0x5u);
    v0 = zero;  /* 0x801970DC: 0x00001021 */
    /* fall through to block_801970E0 */

block_801970E0:
    if (psx_slice_block(cpu, 0x801970E0u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801970E0u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801970E0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x5u);
    v0 = -1;  /* 0x801970E4: 0x2402FFFF */
    if (cond_801970E0) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801970E8u);
    }

block_801970E8:
    if (psx_slice_block(cpu, 0x801970E8u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801970E8u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x801970F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801970EC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019A7D4u);
    cpu->pc = 0x8019A7D4u; return cpu->gpr[2];  /* CPS jal -> cd_intc_busy_read (0x8019A7D4) */

block_801970F0:
    if (psx_slice_block(cpu, 0x801970F0u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801970F0u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801970F0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x801970F4: 0x00000000 */
    if (cond_801970F0) {
        psx_check_interrupts_at(cpu, 0x80197198u);
        goto block_80197198;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801970F8u);
    }

block_801970F8:
    if (psx_slice_block(cpu, 0x801970F8u, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801970F8u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801970F8: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -13556, 2, 0x4u);  /* 0x801970FC: 0x8C42CB0C */
    psx_icache_fetch(cpu, 0x80197100u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197100: 0x00000000 */
    v0 = psx_cyc_load_byte(cpu, v0, 2, 0x4u);  /* 0x80197104: 0x90420000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197108: 0x00000000 */
    psx_cyc_step(cpu, 0x20004u);
    s1 = v0 & 0x0003;  /* 0x8019710C: 0x30510003 */
    /* fall through to block_80197110 */

block_80197110:
    if (psx_slice_block(cpu, 0x80197110u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197110u);
    psx_cyc_step(cpu, 0x80000000u);
    ra = 0x80197118u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197114: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80196370u);
    cpu->pc = 0x80196370u; return cpu->gpr[2];  /* CPS jal -> cpst_80196370 */

block_80197118:
    if (psx_slice_block(cpu, 0x80197118u, 3u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197118u);
    psx_cyc_step(cpu, 0x10005u);
    s0 = v0;  /* 0x80197118: 0x00408021 */
    psx_cyc_step(cpu, 0x10001u);
    int cond_8019711C = (s0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x80197120u);
    psx_cyc_step(cpu, 0x10004u);
    v0 = s0 & 0x0004;  /* 0x80197120: 0x32020004 */
    if (cond_8019711C) {
        psx_check_interrupts_at(cpu, 0x80197188u);
        goto block_80197188;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197124u);
    }

block_80197124:
    if (psx_slice_block(cpu, 0x80197124u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197124u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80197124 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x10004u);
    v0 = s0 & 0x0002;  /* 0x80197128: 0x32020002 */
    if (cond_80197124) {
        psx_check_interrupts_at(cpu, 0x80197154u);
        goto block_80197154;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019712Cu);
    }

block_8019712C:
    if (psx_slice_block(cpu, 0x8019712Cu, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019712Cu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8019712C: 0x3C02801C */
    psx_icache_fetch(cpu, 0x80197130u);
    v0 = psx_cyc_load_word(cpu, v0 + -14260, 2, 0x4u);  /* 0x80197130: 0x8C42C84C */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197134: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80197138 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019713C: 0x00000000 */
    if (cond_80197138) {
        psx_check_interrupts_at(cpu, 0x80197150u);
        goto block_80197150;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197140u);
    }

block_80197140:
    if (psx_slice_block(cpu, 0x80197140u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197140u);
    a0 = psx_cyc_load_byte(cpu, s4, 4, 0x100000u);  /* 0x80197140: 0x92840000 */
    psx_cyc_step(cpu, 0x20u);
    a1 = 0x80200000u;  /* 0x80200000 */  /* 0x80197144: 0x3C058020 */
    psx_cyc_step(cpu, 0x80000004u);
    uint32_t jt_80197148 = v0;  /* latch indirect target before delay slot */
    ra = 0x80197150u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20u);
    a1 -= 14640;  /* 0x8019714C: 0x24A5C6D0 */
    psx_check_interrupts_at(cpu, jt_80197148);
    cpu->pc = jt_80197148; return cpu->gpr[2];  /* CPS jalr */

block_80197150:
    if (psx_slice_block(cpu, 0x80197150u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197150u);
    psx_cyc_step(cpu, 0x10004u);
    v0 = s0 & 0x0002;  /* 0x80197150: 0x32020002 */
    /* fall through to block_80197154 */

block_80197154:
    if (psx_slice_block(cpu, 0x80197154u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197154u);
    psx_cyc_step(cpu, 0x5u);
    int cond_80197154 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197158: 0x00000000 */
    if (cond_80197154) {
        psx_check_interrupts_at(cpu, 0x80197110u);
        goto block_80197110;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019715Cu);
    }

block_8019715C:
    if (psx_slice_block(cpu, 0x8019715Cu, 5u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x8019715Cu);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8019715C: 0x3C02801C */
    psx_icache_fetch(cpu, 0x80197160u);
    v0 = psx_cyc_load_word(cpu, v0 + -14264, 2, 0x4u);  /* 0x80197160: 0x8C42C848 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197164: 0x00000000 */
    psx_cyc_step(cpu, 0x5u);
    int cond_80197168 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019716C: 0x00000000 */
    if (cond_80197168) {
        psx_check_interrupts_at(cpu, 0x80197110u);
        goto block_80197110;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197170u);
    }

block_80197170:
    if (psx_slice_block(cpu, 0x80197170u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197170u);
    a0 = psx_cyc_load_byte(cpu, s2, 4, 0x40000u);  /* 0x80197170: 0x92440000 */
    psx_cyc_step(cpu, 0x20u);
    a1 = 0x80200000u;  /* 0x80200000 */  /* 0x80197174: 0x3C058020 */
    psx_cyc_step(cpu, 0x80000004u);
    uint32_t jt_80197178 = v0;  /* latch indirect target before delay slot */
    ra = 0x80197180u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20u);
    a1 -= 14648;  /* 0x8019717C: 0x24A5C6C8 */
    psx_check_interrupts_at(cpu, jt_80197178);
    cpu->pc = jt_80197178; return cpu->gpr[2];  /* CPS jalr */

block_80197180:
    if (psx_slice_block(cpu, 0x80197180u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197180u);
    psx_cyc_step(cpu, 0x0u);
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197184: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80197110u);
    goto block_80197110;  /* j */

block_80197188:
    if (psx_slice_block(cpu, 0x80197188u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197188u);
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80197188: 0x3C02801C */
    v0 = psx_cyc_load_word(cpu, v0 + -13556, 2, 0x4u);  /* 0x8019718C: 0x8C42CB0C */
    psx_icache_fetch(cpu, 0x80197190u);
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x80197190: 0x00000000 */
    psx_cyc_step(cpu, 0x20004u);
    cpu->write_byte(v0, (uint8_t)s1);  /* 0x80197194: 0xA0510000 */
    /* fall through to block_80197198 */

block_80197198:
    if (psx_slice_block(cpu, 0x80197198u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x80197198u);
    v0 = psx_cyc_load_byte(cpu, s2, 2, 0x40000u);  /* 0x80197198: 0x92420000 */
    psx_cyc_step(cpu, 0x1u);
    /* nop */  /* 0x8019719C: 0x00000000 */
    psx_icache_fetch(cpu, 0x801971A0u);
    psx_cyc_step(cpu, 0x5u);
    int cond_801971A0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x400041u);
    a2 = s6;  /* 0x801971A4: 0x02C03021 */
    if (cond_801971A0) {
        psx_check_interrupts_at(cpu, 0x80197028u);
        goto block_80197028;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971A8u);
    }

block_801971A8:
    if (psx_slice_block(cpu, 0x801971A8u, 4u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801971A8u);
    psx_cyc_step(cpu, 0x10u);
    a0 = 0x80200000u;  /* 0x80200000 */  /* 0x801971A8: 0x3C048020 */
    psx_cyc_step(cpu, 0x10u);
    a0 -= 14648;  /* 0x801971AC: 0x2484C6C8 */
    psx_icache_fetch(cpu, 0x801971B0u);
    psx_cyc_step(cpu, 0x41u);
    int cond_801971B0 = (a2 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x9u);
    v1 = 7;  /* 0x801971B4: 0x24030007 */
    if (cond_801971B0) {
        psx_check_interrupts_at(cpu, 0x801971D4u);
        goto block_801971D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971B8u);
    }

block_801971B8:
    if (psx_slice_block(cpu, 0x801971B8u, 1u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801971B8u);
    psx_cyc_step(cpu, 0x21u);
    a1 = -1;  /* 0x801971B8: 0x2405FFFF */
    /* fall through to block_801971BC */

block_801971BC:
    if (psx_slice_block(cpu, 0x801971BCu, 6u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801971BCu);
    v0 = psx_cyc_load_byte(cpu, a0, 2, 0x10u);  /* 0x801971BC: 0x90820000 */
    psx_icache_fetch(cpu, 0x801971C0u);
    psx_cyc_step(cpu, 0x10u);
    a0++;  /* 0x801971C0: 0x24840001 */
    psx_cyc_step(cpu, 0x8u);
    v1--;  /* 0x801971C4: 0x2463FFFF */
    psx_cyc_step(cpu, 0x44u);
    cpu->write_byte(a2, (uint8_t)v0);  /* 0x801971C8: 0xA0C20000 */
    psx_cyc_step(cpu, 0x28u);
    int cond_801971CC = (v1 != a1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_icache_fetch(cpu, 0x801971D0u);
    psx_cyc_step(cpu, 0x40u);
    a2++;  /* 0x801971D0: 0x24C60001 */
    if (cond_801971CC) {
        psx_check_interrupts_at(cpu, 0x801971BCu);
        goto block_801971BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971D4u);
    }

block_801971D4:
    if (psx_slice_block(cpu, 0x801971D4u, 7u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801971D4u);
    psx_cyc_step(cpu, 0x11u);
    a0 = zero;  /* 0x801971D4: 0x00002021 */
    psx_cyc_step(cpu, 0x4u);
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x801971D8: 0x3C02801C */
    psx_cyc_step(cpu, 0x4u);
    v0 -= 13532;  /* 0x801971DC: 0x2442CB24 */
    psx_icache_fetch(cpu, 0x801971E0u);
    v1 = psx_cyc_load_byte(cpu, v0, 3, 0x4u);  /* 0x801971E0: 0x90430000 */
    psx_cyc_step(cpu, 0x5u);
    v0 = 5;  /* 0x801971E4: 0x24020005 */
    psx_cyc_step(cpu, 0xCu);
    int cond_801971E8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x15u);
    v0 = a0;  /* 0x801971EC: 0x00801021 */
    if (cond_801971E8) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971F0u);
    }

block_801971F0:
    if (psx_slice_block(cpu, 0x801971F0u, 2u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801971F0u);
    psx_cyc_step(cpu, 0x11u);
    a0 = -1;  /* 0x801971F0: 0x2404FFFF */
    psx_cyc_step(cpu, 0x15u);
    v0 = a0;  /* 0x801971F4: 0x00801021 */
    /* fall through to block_801971F8 */

block_801971F8:
    if (psx_slice_block(cpu, 0x801971F8u, 10u, 0)) return cpu->gpr[2];
    psx_icache_fetch(cpu, 0x801971F8u);
    ra = psx_cyc_load_word(cpu, sp + 52, 31, 0x20000000u);  /* 0x801971F8: 0x8FBF0034 */
    s6 = psx_cyc_load_word(cpu, sp + 48, 22, 0x20000000u);  /* 0x801971FC: 0x8FB60030 */
    psx_icache_fetch(cpu, 0x80197200u);
    s5 = psx_cyc_load_word(cpu, sp + 44, 21, 0x20000000u);  /* 0x80197200: 0x8FB5002C */
    s4 = psx_cyc_load_word(cpu, sp + 40, 20, 0x20000000u);  /* 0x80197204: 0x8FB40028 */
    s3 = psx_cyc_load_word(cpu, sp + 36, 19, 0x20000000u);  /* 0x80197208: 0x8FB30024 */
    s2 = psx_cyc_load_word(cpu, sp + 32, 18, 0x20000000u);  /* 0x8019720C: 0x8FB20020 */
    psx_icache_fetch(cpu, 0x80197210u);
    s1 = psx_cyc_load_word(cpu, sp + 28, 17, 0x20000000u);  /* 0x80197210: 0x8FB1001C */
    s0 = psx_cyc_load_word(cpu, sp + 24, 16, 0x20000000u);  /* 0x80197214: 0x8FB00018 */
    psx_cyc_step(cpu, 0x80000001u);
    uint32_t jt_80197218 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    psx_cyc_step(cpu, 0x20000000u);
    sp += 56;  /* 0x8019721C: 0x27BD0038 */
    psx_check_interrupts_at(cpu, jt_80197218);
    cpu->pc = jt_80197218; return cpu->gpr[2];  /* CPS: jr $ra */
}