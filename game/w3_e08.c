/* w3_e08.c — clean CPS ports of shared-body alias/piece entries
 * (worktree orch/w3/e08).
 *
 * All eleven assigned addrs are CPS piece-entries / overlapping-alias
 * entries into five shared generated bodies; each body below is a faithful
 * op-by-op transliteration (gen_cps_clean.py transform) of the generated
 * CPS code: same guest ops, order, delay slots, yield/continuation points,
 * minus cycle/icache/cosim/debug instrumentation. Memory ops use the
 * MMIO-aware cpu read/write accessors. Dispatch addrs are registered
 * in game/clean_cps.map; cpu->pc selects the entry/continuation block.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

/* MIPS unaligned word accessors — faithful to psx_lwl/lwr/swl/swr but on the
 * MMIO-aware cpu read/write accessor path (no per-access cycle charge). */
static inline uint32_t br2_lwl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return (rt_value & 0x00FFFFFFu) | (word << 24);
        case 1: return (rt_value & 0x0000FFFFu) | (word << 16);
        case 2: return (rt_value & 0x000000FFu) | (word << 8);
        default: return word;
    }
}
static inline uint32_t br2_lwr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return word;
        case 1: return (rt_value & 0xFF000000u) | (word >> 8);
        case 2: return (rt_value & 0xFFFF0000u) | (word >> 16);
        default: return (rt_value & 0xFFFFFF00u) | (word >> 24);
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
/*  mc_op_result_poll (guest body 0x80168424, renames.map) — memory-card */
/*  op result poll. CPS piece/alias entries; small 11-block body. */

void mc_op_result_poll_alt(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80168450u: goto block_80168450;
            case 0x80168464u: goto block_80168464;
            case 0x80168448u: goto block_80168448;
            case 0x80168458u: goto block_80168458;
            case 0x8016846Cu: goto block_8016846C;
            case 0x80168480u: goto block_80168480;
            case 0x80168494u: goto block_80168494;
            case 0x801684A4u: goto block_801684A4;
            case 0x801684ACu: goto block_801684AC;
            case 0x801684B0u: goto block_801684B0;
            default: break;
        }
    }
    /* Address: 0x80168424, Size: 164 bytes, Blocks: 11 */
    /* mc_op_result_poll: submit a job on channel s1 (a0), then poll
     * cd_job_result until the result index at sp+20 stabilizes.
     * Result classes <5 dispatch through jump table @0x8012212C;
     * classes >=5 are "in flight" — re-poll until the index stops
     * changing, then return it in v0. */

    /* memcard op-result poll: retry 0x8019EA98 until v0!=0, then retry
     * 0x8019FA80; index word(sp+20) (<5) dispatches via table s2, else
     * re-poll while the value keeps changing; v0 = word(sp+20) on return */
block_80168424:
    sp = sp + -40;
    cpu->write_word(sp + 28, s1);
    s1 = a0;
    cpu->write_word(sp + 24, s0);
    s0 = zero;                       /* s0 = previous result index (stable-detect) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    cpu->write_word(sp + 32, s2);
    s2 = v0 + 8492;                  /* s2 = jump table @0x8012212C */
    cpu->write_word(sp + 36, ra);
    /* fall through to block_80168448 */

block_80168448:
    ra = 0x80168450u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x8019EA98u);
    cpu->pc = 0x8019EA98u; return;  /* CPS jal -> cd_chan_job_submit */

block_80168450:
    /* delay slot (always executes) */
    a0 = zero;
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80168448u);
        goto block_80168448;  /* taken: poll again */
    } else {
        psx_check_interrupts_at(cpu, 0x80168458u);
    }
    psx_check_interrupts_at(cpu, 0x80168458u);
    /* fall through to block_80168458 */

block_80168458:
    a1 = sp + 16;                    /* &result_word */
    ra = 0x80168464u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = sp + 20;                    /* &result_index */
    psx_check_interrupts_at(cpu, 0x8019FA80u);
    cpu->pc = 0x8019FA80u; return;  /* CPS jal -> cd_job_result */

block_80168464:
    /* delay slot (always executes) */
    a0 = zero;
    if (v0 == zero) {
        psx_check_interrupts_at(cpu, 0x80168458u);
        goto block_80168458;  /* taken: poll again */
    } else {
        psx_check_interrupts_at(cpu, 0x8016846Cu);
    }
    psx_check_interrupts_at(cpu, 0x8016846Cu);
    /* fall through to block_8016846C */

block_8016846C:
    v1 = cpu->read_word(sp + 20);    /* v1 = result index/class */
    /* nop */  /* 0x80168470: 0x00000000 */
    v0 = (v1 < (uint32_t)5) ? 1 : 0;  /* 0x80168474: 0x2C620005 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80168494u);
        goto block_80168494;  /* taken: index >=5 -> stability check */
    } else {
        psx_check_interrupts_at(cpu, 0x80168480u);
    }
    psx_check_interrupts_at(cpu, 0x80168480u);
    /* fall through to block_80168480 */

block_80168480:
    v0 = v0 + s2;                    /* table_8012212C[class] */
    v0 = cpu->read_word(v0);
    /* nop */  /* 0x80168488: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80168490: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 — tail-dispatch class handler */

block_80168494:
    v0 = cpu->read_word(sp + 20);    /* current index again */
    /* nop */  /* 0x80168498: 0x00000000 */
    int cond_s0_eq_v0 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v0;                         /* remember this index */
    if (cond_s0_eq_v0) {
        psx_check_interrupts_at(cpu, 0x801684B0u);
        goto block_801684B0;  /* taken: unchanged -> done */
    } else {
        psx_check_interrupts_at(cpu, 0x801684A4u);
    }
    psx_check_interrupts_at(cpu, 0x801684A4u);
    /* fall through to block_801684A4 */

block_801684A4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801684A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80168448u);
    goto block_80168448;  /* j — back to submit/poll loop */

block_801684AC:
    v0 = cpu->read_word(sp + 20);
    /* fall through to block_801684B0 */

block_801684B0:
    ra = cpu->read_word(sp + 36);
    s2 = cpu->read_word(sp + 32);
    s1 = cpu->read_word(sp + 28);
    s0 = cpu->read_word(sp + 24);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/*  mc_file_cmd_stable_poll (guest body 0x801685B4, renames.map) — */
/*  memory-card file-command stable poll. CPS piece/alias entries; */
/*  6-block body. */

void mc_file_cmd_stable_poll_alt(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801685F8u: goto block_801685F8;
            case 0x801685E8u: goto block_801685E8;
            case 0x80168608u: goto block_80168608;
            case 0x8016861Cu: goto block_8016861C;
            case 0x80168624u: goto block_80168624;
            default: break;
        }
    }
    /* Address: 0x801685B4, Size: 148 bytes, Blocks: 6 */
    /* mc_file_cmd_stable_poll: issue mc_file_cmd(s2,s3,s4) = (a0,a1,a2)
     * and poll until the result stabilizes. Result classes <8 dispatch
     * through jump table @0x80122144; classes >=8 mean "command still
     * in flight" — re-issue until the result stops changing, then
     * return it in v0. */

    /* stable-result poll: retry 0x8019FB9C until v0!=0; index v0 (<8)
     * dispatches via table s1, else re-poll while the value keeps
     * changing; v0 = last index on return */
block_801685B4:
    sp = sp + -40;
    cpu->write_word(sp + 24, s2);
    s2 = a0;                         /* s2 = cmd arg0 */
    cpu->write_word(sp + 28, s3);
    s3 = a1;                         /* s3 = cmd arg1 */
    cpu->write_word(sp + 32, s4);
    s4 = a2;                         /* s4 = cmd arg2 */
    cpu->write_word(sp + 16, s0);
    s0 = zero;                       /* s0 = previous result (stable-detect) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    cpu->write_word(sp + 20, s1);
    s1 = v0 + 8516;                  /* s1 = jump table @0x80122144 */
    cpu->write_word(sp + 36, ra);
    /* fall through to block_801685E8 */

block_801685E8:
    a0 = s2;
    a1 = s3;
    ra = 0x801685F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x8019FB9Cu);
    cpu->pc = 0x8019FB9Cu; return;  /* CPS jal -> mc_file_cmd */

block_801685F8:
    v1 = v0;                         /* v1 = mc_file_cmd result */
    v0 = (v1 < (uint32_t)8) ? 1 : 0;  /* 0x801685FC: 0x2C620008 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x8016861Cu);
        goto block_8016861C;  /* taken: index >=8 -> stability check */
    } else {
        psx_check_interrupts_at(cpu, 0x80168608u);
    }
    psx_check_interrupts_at(cpu, 0x80168608u);
    /* fall through to block_80168608 */

block_80168608:
    v0 = v0 + s1;                    /* table_80122144[class] */
    v0 = cpu->read_word(v0);
    /* nop */  /* 0x80168610: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80168618: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 — tail-dispatch class handler */

block_8016861C:;
    int cond_s0_ne_v1 = (s0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v1;                         /* remember this result */
    if (cond_s0_ne_v1) {
        psx_check_interrupts_at(cpu, 0x801685E8u);
        goto block_801685E8;  /* taken: changed -> repoll */
    } else {
        psx_check_interrupts_at(cpu, 0x80168624u);
    }
    psx_check_interrupts_at(cpu, 0x80168624u);
    /* fall through to block_80168624 */

block_80168624:
    v0 = v1;                         /* return stable result */
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}