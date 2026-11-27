/* w4_e00.c — clean CPS ports of scriptop_wait / scriptop_chk
 * piece-entry groups (SLES_017.22_full_38.c / _full_39.c, alias bodies
 * br2eu_psx_alias_body_80183664 / _80183C30).
 *
 * All eleven guest addresses are CPS split-piece entries, NOT standalone
 * functions: the recompiler exposes mid-body blocks of two shared CPS
 * bodies as separate dispatch addrs. They are only ever reached via
 * `cpu->pc = <addr>` coroutine dispatches (cond-table `jr $v0` targets and
 * IRQ/resume PCs), so they are routed through game/clean_cps.map
 * (CLEAN_ENT_CPS wrappers that hand the fn cpu->pc == the dispatched
 * addr) and the body's pc-switch picks the right block — exactly like
 * the generated alias body's entry switch.
 *
 * scriptop_wait pieces (0x80183920/0x80183948): cond cases 4/5 —
 *   self (s2): state0 >= 0 || atkwin3 > 0  -> pc+=2, ret 1
 *   foe  (s1): same predicate on foe ctrl
 *   shared tail: 80183934 branch, 80183940 ret-0, 80183A98 pc+2 store,
 *   80183A9C ret-1, 80183BB4 epilogue (pop s0-s3/ra, sp+=40, jr ra).
 *
 * scriptop_chk pieces (0x80184018-0x80184250): cond-table cases of
 *   chk_state20     0x80184018/0x80184024 — state0 & 0x20 -> mask store
 *   chk_atk3_free   0x80184040/0x80184074 — state0>=0 || atkwin3>0
 *   chk_mask_op     0x80184110/0x80184150/0x80184190/0x80184250 —
 *                   ctrl field & operand, debug-trace gated
 *   chk_distx       0x80184210 — |distX| >= 1707 test
 *   shared tails: 0x80184354 (pc+2 store) / 0x80184358 (epilogue,
 *   pop s0-s4/ra, sp+=40, jr ra) — already routed to chk_atk2_self /
 *   scriptop_chk_state_bits in clean_cps.map; the blocks below are
 *   entry-independent faithful copies reached by direct goto.
 *
 * Faithful op-by-op transcription of the generated blocks: same guest
 * ops, same order, same yield/continuation points, minus
 * cycle/icache/cosim/debug instrumentation. Memory ops use the
 * MMIO-aware cpu->read_word / cpu->write_word accessors.
 */
#include <stdint.h>
#include <stdlib.h>
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

/* ======== scriptop_wait cond4/cond5 — attack-win wait ========
 * CPS piece-group: piece-entries 0x80183920 (self, s2) and 0x80183948
 * (foe, s1) of scriptop_wait (guest 0x80183664), plus the shared
 * continuation blocks they reach. Register roles inside the live
 * scriptop_wait frame: s2 = self ctrl, s1 = foe ctrl, s0 = ctx,
 * s3 = script pc value (ctx+0x24). On success the blocks store s3+2
 * back to ctx+0x24 and return v0 = 1 through the shared epilogue;
 * on failure v0 = 0 -> epilogue (wait continues). */
static void scriptop_wait_atk3_body(CPUState *cpu)
{

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183920u: goto block_80183920;
            case 0x80183930u: goto block_80183930;
            case 0x80183934u: goto block_80183934;
            case 0x80183940u: goto block_80183940;
            case 0x80183948u: goto block_80183948;
            case 0x80183958u: goto block_80183958;
            case 0x80183A98u: goto block_80183A98;
            case 0x80183A9Cu: goto block_80183A9C;
            case 0x80183BB4u: goto block_80183BB4;
            default: break;
        }
    }

block_80183920:
    v0 = cpu->read_word(s2 + 88);
    /* nop */  /* 0x80183924: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183930u);
        goto block_80183930;  /* not taken */
    }

block_80183930:
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 188);
    /* fall through to block_80183934 */

block_80183934:;
    /* nop */  /* 0x80183934: 0x00000000 */
    int cond_v0_gt_0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_gt_0) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183940u);
        goto block_80183940;  /* not taken */
    }

block_80183940:
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, 0x80183BB4u);
    goto block_80183BB4;  /* j */

block_80183948:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018394C: 0x00000000 */
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183958u);
        goto block_80183958;  /* not taken */
    }

block_80183958:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 188);
    /* delay slot (always executes) */
    /* nop */  /* 0x80183960: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80183934u);
    goto block_80183934;  /* j */

block_80183A98:
    cpu->write_word(s0 + 36, v0);
    /* fall through to block_80183A9C */

block_80183A9C:
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183BB4u);
    goto block_80183BB4;  /* j */

block_80183BB4:
    ra = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* scriptop_wait_atk3_self — CPS piece-entry wrapper: resume scriptop_wait_atk3_body */
void scriptop_wait_atk3_self(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  scriptop_wait_atk3_body(cpu);
}

/* scriptop_wait_atk3_foe — CPS piece-entry wrapper: resume scriptop_wait_atk3_body */
void scriptop_wait_atk3_foe(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  scriptop_wait_atk3_body(cpu);
}

/* ======== scriptop_chk cond26/cond27 — state0 & 0x20 ========
 * Piece-entries 0x80184018 (self, s3) / 0x80184024 (foe, s0): load the
 * state word at ctrl+0x58, advance script pc (s1+2 -> ctx+0x24 via
 * s2+0x24), mask with 0x20 into ctx+0x38 (s2+0x38), then the shared
 * `jr $ra` epilogue at 0x80184358. */
void chk_state20(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80184018u: goto block_80184018;
            case 0x80184024u: goto block_80184024;
            case 0x8018402Cu: goto block_8018402C;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80184018:
    v1 = cpu->read_word(s3 + 88);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x8018402Cu);
    goto block_8018402C;  /* j */

block_80184024:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    /* fall through to block_8018402C */

block_8018402C:
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = v1 & 0x0020;  /* 0x80184034: 0x30630020 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184358:
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== scriptop_chk cond28/cond29 — free || atkwin3 ========
 * Piece-entries 0x80184040 (self, s3) / 0x80184074 (foe, s0):
 *   state0 >= 0  -> result 1
 *   else atkwin3 (ctrl+0xbc, lh) > 0 -> result 1
 *   else          -> result 0
 * Result v1 stored to ctx+0x38 (s2+0x38) via shared tail 0x80184068 /
 * 0x8018409C, then pc+2 store at 0x80184354 and the shared epilogue
 * 0x80184358. v0 = 1 on every path (return value). */
void chk_atk3_free(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80184040u: goto block_80184040;
            case 0x80184050u: goto block_80184050;
            case 0x80184060u: goto block_80184060;
            case 0x80184068u: goto block_80184068;
            case 0x80184074u: goto block_80184074;
            case 0x80184084u: goto block_80184084;
            case 0x80184094u: goto block_80184094;
            case 0x8018409Cu: goto block_8018409C;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80184040:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x80184044: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80184060u);
        goto block_80184060;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184050u);
        goto block_80184050;  /* not taken */
    }

block_80184050:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 188);
    /* nop */  /* 0x80184054: 0x00000000 */
    int cond_v0_le_0 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_le_0) {
        psx_check_interrupts_at(cpu, 0x80184068u);
        goto block_80184068;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184060u);
        goto block_80184060;  /* not taken */
    }

block_80184060:
    v1 = 1;
    v0 = 1;
    /* fall through to block_80184068 */

block_80184068:
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184074:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80184078: 0x00000000 */
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x80184094u);
        goto block_80184094;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184084u);
        goto block_80184084;  /* not taken */
    }

block_80184084:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 188);
    /* nop */  /* 0x80184088: 0x00000000 */
    int cond_v0_le_0_2 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_le_0_2) {
        psx_check_interrupts_at(cpu, 0x8018409Cu);
        goto block_8018409C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184094u);
        goto block_80184094;  /* not taken */
    }

block_80184094:
    v1 = 1;
    v0 = 1;
    /* fall through to block_8018409C */

block_8018409C:
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184354:
    cpu->write_word(s2 + 36, v1);
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== scriptop_chk mask-op family + distX ========
 * chk_mask_op piece-entries: 0x80184110 (self +0x64), 0x80184150 (self
 * +0x5c), 0x80184190 (self +0x58), 0x80184250 (foe +0x6c). Each reads
 * the 32-bit operand (lo half at s1+2, hi half at s1+4), advances the
 * script pc by 6 (s1+6 -> ctx+0x24), ANDs the ctrl field with the
 * operand, stores the masked result to ctx+0x38 (s2+0x38), then joins
 * the shared debug-trace tail at 0x80184278: if the trace flag
 * (ctx+0x6e, lh at s2+0x6e) is zero -> epilogue; else range-check the
 * trace cursor (ctx+0x78) and sprintf "%8X" via jal 0x8018EF60
 * (return addr 0x801842A4), advance the cursor by the return length,
 * and return 1 through the epilogue.
 *
 * chk_distx piece-entry 0x80184210: tests |self distX (s3+0x74, lh)|
 * >= 1707, stores the boolean to ctx+0x38, advances pc by 2, returns 1
 * through the epilogue. */
void chk_mask_op(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80184110u: goto block_80184110;
            case 0x80184150u: goto block_80184150;
            case 0x80184190u: goto block_80184190;
            case 0x80184250u: goto block_80184250;
            case 0x80184260u: goto block_80184260;
            case 0x80184278u: goto block_80184278;
            case 0x80184280u: goto block_80184280;
            case 0x8018429Cu: goto block_8018429C;
            case 0x801842A4u: goto block_801842A4;
            case 0x801842B4u: goto block_801842B4;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80184110:
    a1 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a0 = cpu->read_word(s3 + 100);
    v0 = s1 + 6;
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a1 + v1;
    a0 = a0 & a2;  /* 0x80184130: 0x00862024 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, a0);
    psx_check_interrupts_at(cpu, 0x80184278u);
    goto block_80184278;  /* j */

block_80184150:
    a1 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a0 = cpu->read_word(s3 + 92);
    v0 = s1 + 6;
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a1 + v1;
    a0 = a0 & a2;  /* 0x80184170: 0x00862024 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, a0);
    psx_check_interrupts_at(cpu, 0x80184278u);
    goto block_80184278;  /* j */

block_80184190:
    a1 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a0 = cpu->read_word(s3 + 88);
    v0 = s1 + 6;
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a1 + v1;
    a0 = a0 & a2;  /* 0x801841B0: 0x00862024 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, a0);
    psx_check_interrupts_at(cpu, 0x80184278u);
    goto block_80184278;  /* j */

block_80184250:
    a0 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a1 = cpu->read_word(s0 + 108);
    v0 = s1 + 6;
    /* fall through to block_80184260 */

block_80184260:
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a0 + v1;
    a2 = a2 & a1;  /* 0x80184270: 0x00C53024 */
    cpu->write_word(s2 + 56, a2);
    /* fall through to block_80184278 */

block_80184278:;
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80184358u);
        goto block_80184358;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184280u);
        goto block_80184280;  /* not taken */
    }

block_80184280:
    a0 = cpu->read_word(s2 + 120);
    /* nop */  /* 0x80184284: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s2;
    v0 = ((int32_t)v0 < 191) ? 1 : 0;  /* 0x80184290: 0x284200BF */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x801842B4u);
        goto block_801842B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018429Cu);
        goto block_8018429C;  /* not taken */
    }

block_8018429C:
    ra = 0x801842A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 13760;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_801842A4:
    v1 = cpu->read_word(s2 + 120);
    /* nop */  /* 0x801842A8: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s2 + 120, v1);
    /* fall through to block_801842B4 */

block_801842B4:
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184358:
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== scriptop_chk cond40 — |distX| >= 1707 ========
 * Piece-entry 0x80184210: load self distX (s3+0x74, lh), advance the
 * script pc by 2 (s1+2 -> ctx+0x24), negate when negative, compare
 * against 1707 (0x6ab), XOR with v0=1 so the stored boolean is
 * "|distX| >= 1707", store to ctx+0x38 (s2+0x38), return 1 through
 * the shared epilogue 0x80184358. */
void chk_distx(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80184210u: goto block_80184210;
            case 0x80184228u: goto block_80184228;
            case 0x8018422Cu: goto block_8018422C;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80184210:
    v0 = 1;
    v1 = (int32_t)(int16_t)cpu->read_half(s3 + 116);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    int cond_v1_ge_0 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80184224: 0x00000000 */
    if (cond_v1_ge_0) {
        psx_check_interrupts_at(cpu, 0x8018422Cu);
        goto block_8018422C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184228u);
        goto block_80184228;  /* not taken */
    }

block_80184228:
    v1 = zero - v1;
    /* fall through to block_8018422C */

block_8018422C:
    v1 = ((int32_t)v1 < 1707) ? 1 : 0;  /* 0x8018422C: 0x286306AB */
    v1 = v1 ^ v0;  /* 0x80184230: 0x00621826 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184358:
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}
