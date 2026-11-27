/* w2_e09.c — clean CPS ports of scriptop_chk (guest 0x80183C30, op04 Chk)
 * case piece-entries 0x80183DC8-0x80183EFC (SLES_017.22_full_39.c,
 * br2eu_psx_alias_body_80183C30).
 *
 * All eight addrs are CPS piece-entries into the 45-cond Chk dispatch
 * table (@0x801235C4): the guest `jr v0` lands mid-function on a per-cond
 * case block, each of which computes ctx+0x38 (result) and ctx+0x24
 * (pc += 2, or the FDC/FE4 pair which stores pc+2 itself and skips the
 * shared 0x80184354 store), then runs the shared epilogue at
 * 0x80184358 (restore s0-s4/ra, sp += 40, jr ra). They are reached via
 * `cpu->pc = <addr>` coroutine dispatches, NOT ordinary `jr $ra`
 * functions, so they route through game/clean_cps.map (CLEAN_ENT_CPS
 * wrappers hand the fn cpu->pc == the dispatched addr) exactly like the
 * cps_clean.c bodies. Register roles (from the 0x80183C30 prologue):
 *   s3 = self ctrl, s0 = foe ctrl (self+0x2c), s2 = script ctx,
 *   s1 = script pc (ctx+0x24), s4 = cond, v0 = return value.
 *
 * Each body below is a faithful op-by-op transcription of the generated
 * block subgraph reachable from its entry: same guest ops, same order,
 * same yield/continuation points, minus cycle/icache/cosim/debug
 * instrumentation. Shared tails (0x80184354/0x80184358 store+epilogue,
 * 0x80183FDC/0x80183FE4 pc-advance+mask store) are duplicated per
 * function exactly as the guest executes them.
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

/* ======== 0x80183dc8 — chk_atk2_self ========
 * Chk cond 10 (self): res = state0<0 && atkwin2(+0xba) != 0.
 * CPS piece-group: entry 0x80183DC8 + continuations 0x80183DD8/0x80183DE4
 * + shared tail 0x80184354/0x80184358. Faithful to generated
 * br2eu_func_80183DC8 (alias body br2eu_psx_alias_body_80183C30). */
void chk_atk2_self(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183DC8u: goto block_80183DC8;
            case 0x80183DD8u: goto block_80183DD8;
            case 0x80183DE4u: goto block_80183DE4;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183DC8:
    v0 = cpu->read_word(s3 + 88);  /* lw v0,0x58(s3) — state0 */
    /* nop */  /* 0x80183DCC: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* move v1,zero */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183DE4u);
        goto block_80183DE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183DD8u);
        goto block_80183DD8;  /* not taken */
    }

block_80183DD8:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 186);  /* lh v0,0xba(s3) — atkwin2 */
    /* nop */  /* 0x80183DDC: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183DE0: sltu v1,v1,v0 */
    /* fall through to block_80183DE4 */

block_80183DE4:
    v0 = 1;  /* li v0,1 — return value */
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    /* delay slot (always executes) */
    v1 = s1 + 2;  /* addiu v1,s1,2 — pc advance */
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184354:
    cpu->write_word(s2 + 36, v1);  /* sw v1,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80183df4 — chk_atk2_foe ========
 * Chk cond 11 (foe): res = foe state0<0 && foe atkwin2 != 0.
 * Same shape as chk_atk2_self with s0 (foe) in place of s3 (self).
 * Faithful to generated br2eu_func_80183DF4. */
void chk_atk2_foe(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183DF4u: goto block_80183DF4;
            case 0x80183E04u: goto block_80183E04;
            case 0x80183E10u: goto block_80183E10;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183DF4:
    v0 = cpu->read_word(s0 + 88);  /* lw v0,0x58(s0) — foe state0 */
    /* nop */  /* 0x80183DF8: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* move v1,zero */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183E10u);
        goto block_80183E10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E04u);
        goto block_80183E04;  /* not taken */
    }

block_80183E04:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 186);  /* lh v0,0xba(s0) — foe atkwin2 */
    /* nop */  /* 0x80183E08: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183E0C: sltu v1,v1,v0 */
    /* fall through to block_80183E10 */

block_80183E10:
    v0 = 1;  /* li v0,1 — return value */
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    /* delay slot (always executes) */
    v1 = s1 + 2;  /* addiu v1,s1,2 — pc advance */
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184354:
    cpu->write_word(s2 + 36, v1);  /* sw v1,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80183e20 — chk_atk1_self ========
 * Chk cond 12 (self): res = state0<0 && atkwin2==0 && atkwin1(+0xb8)!=0.
 * Two-level check: bgez state0 -> store 0; else bne atkwin2 -> store 0;
 * else sltu vs atkwin1. CPS piece-group 0x80183E20/30/40/4C/50 + tail.
 * Faithful to generated br2eu_func_80183E20. */
void chk_atk1_self(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183E20u: goto block_80183E20;
            case 0x80183E30u: goto block_80183E30;
            case 0x80183E40u: goto block_80183E40;
            case 0x80183E4Cu: goto block_80183E4C;
            case 0x80183E50u: goto block_80183E50;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183E20:
    v0 = cpu->read_word(s3 + 88);  /* lw v0,0x58(s3) — state0 */
    /* nop */  /* 0x80183E24: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* move v1,zero */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183E4Cu);
        goto block_80183E4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E30u);
        goto block_80183E30;  /* not taken */
    }

block_80183E30:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 186);  /* lh v0,0xba(s3) — atkwin2 */
    /* nop */  /* 0x80183E34: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;  /* li v0,1 — return value (both paths) */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183E50u);
        goto block_80183E50;  /* taken — atkwin2 != 0 -> res 0 */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E40u);
        goto block_80183E40;  /* not taken */
    }

block_80183E40:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 184);  /* lh v0,0xb8(s3) — atkwin1 */
    /* nop */  /* 0x80183E44: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183E48: sltu v1,v1,v0 */
    /* fall through to block_80183E4C */

block_80183E4C:
    v0 = 1;  /* li v0,1 — return value */
    /* fall through to block_80183E50 */

block_80183E50:
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    /* delay slot (always executes) */
    v1 = s1 + 2;  /* addiu v1,s1,2 — pc advance */
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184354:
    cpu->write_word(s2 + 36, v1);  /* sw v1,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80183e5c — chk_atk1_foe ========
 * Chk cond 13 (foe): res = foe state0<0 && foe atkwin2==0 && foe
 * atkwin1 != 0. Same two-level shape as chk_atk1_self on s0.
 * Faithful to generated br2eu_func_80183E5C. */
void chk_atk1_foe(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183E5Cu: goto block_80183E5C;
            case 0x80183E6Cu: goto block_80183E6C;
            case 0x80183E7Cu: goto block_80183E7C;
            case 0x80183E88u: goto block_80183E88;
            case 0x80183E8Cu: goto block_80183E8C;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183E5C:
    v0 = cpu->read_word(s0 + 88);  /* lw v0,0x58(s0) — foe state0 */
    /* nop */  /* 0x80183E60: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* move v1,zero */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183E88u);
        goto block_80183E88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E6Cu);
        goto block_80183E6C;  /* not taken */
    }

block_80183E6C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 186);  /* lh v0,0xba(s0) — foe atkwin2 */
    /* nop */  /* 0x80183E70: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;  /* li v0,1 — return value (both paths) */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183E8Cu);
        goto block_80183E8C;  /* taken — atkwin2 != 0 -> res 0 */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E7Cu);
        goto block_80183E7C;  /* not taken */
    }

block_80183E7C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 184);  /* lh v0,0xb8(s0) — foe atkwin1 */
    /* nop */  /* 0x80183E80: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183E84: sltu v1,v1,v0 */
    /* fall through to block_80183E88 */

block_80183E88:
    v0 = 1;  /* li v0,1 — return value */
    /* fall through to block_80183E8C */

block_80183E8C:
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    /* delay slot (always executes) */
    v1 = s1 + 2;  /* addiu v1,s1,2 — pc advance */
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184354:
    cpu->write_word(s2 + 36, v1);  /* sw v1,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80183e98 — chk_atk3_self ========
 * Chk cond 14 (self): res = state0<0 && 0 <s atkwin3(+0xbc).
 * Signed slt vs the unsigned-sltu pairs above. CPS piece-group
 * 0x80183E98/EA8/EB4 + tail. Faithful to generated br2eu_func_80183E98. */
void chk_atk3_self(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183E98u: goto block_80183E98;
            case 0x80183EA8u: goto block_80183EA8;
            case 0x80183EB4u: goto block_80183EB4;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183E98:
    v0 = cpu->read_word(s3 + 88);  /* lw v0,0x58(s3) — state0 */
    /* nop */  /* 0x80183E9C: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* move v1,zero */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183EB4u);
        goto block_80183EB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183EA8u);
        goto block_80183EA8;  /* not taken */
    }

block_80183EA8:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 188);  /* lh v0,0xbc(s3) — atkwin3 */
    /* nop */  /* 0x80183EAC: 0x00000000 */
    v1 = ((int32_t)v1 < (int32_t)v0) ? 1 : 0;  /* 0x80183EB0: slt v1,v1,v0 */
    /* fall through to block_80183EB4 */

block_80183EB4:
    v0 = 1;  /* li v0,1 — return value */
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    /* delay slot (always executes) */
    v1 = s1 + 2;  /* addiu v1,s1,2 — pc advance */
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184354:
    cpu->write_word(s2 + 36, v1);  /* sw v1,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80183ec4 — chk_atk3_foe ========
 * Chk cond 15 (foe): res = foe state0<0 && 0 <s foe atkwin3.
 * Faithful to generated br2eu_func_80183EC4. */
void chk_atk3_foe(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183EC4u: goto block_80183EC4;
            case 0x80183ED4u: goto block_80183ED4;
            case 0x80183EE0u: goto block_80183EE0;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183EC4:
    v0 = cpu->read_word(s0 + 88);  /* lw v0,0x58(s0) — foe state0 */
    /* nop */  /* 0x80183EC8: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;  /* move v1,zero */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183EE0u);
        goto block_80183EE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183ED4u);
        goto block_80183ED4;  /* not taken */
    }

block_80183ED4:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 188);  /* lh v0,0xbc(s0) — foe atkwin3 */
    /* nop */  /* 0x80183ED8: 0x00000000 */
    v1 = ((int32_t)v1 < (int32_t)v0) ? 1 : 0;  /* 0x80183EDC: slt v1,v1,v0 */
    /* fall through to block_80183EE0 */

block_80183EE0:
    v0 = 1;  /* li v0,1 — return value */
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    /* delay slot (always executes) */
    v1 = s1 + 2;  /* addiu v1,s1,2 — pc advance */
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184354:
    cpu->write_word(s2 + 36, v1);  /* sw v1,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80183ef0 — chk_mode1_self ========
 * Chk cond 16 (self): res = mode5(+5) & 1. Uses the alternate tail:
 * FDC stores ctx+0x24 = s1+2 itself (via a0) and FE4 stores the masked
 * result, then j 0x80184358 — skipping the shared 0x80184354 store.
 * Faithful to generated br2eu_func_80183EF0. */
void chk_mode1_self(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183EF0u: goto block_80183EF0;
            case 0x80183FDCu: goto block_80183FDC;
            case 0x80183FE4u: goto block_80183FE4;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183EF0:
    v1 = cpu->read_byte(s3 + 5);  /* lbu v1,5(s3) — mode5 */
    /* delay slot (always executes) */
    v0 = 1;  /* li v0,1 — mask + return value */
    psx_check_interrupts_at(cpu, 0x80183FDCu);
    goto block_80183FDC;  /* j */

block_80183FDC:
    a0 = s1 + 2;  /* addiu a0,s1,2 — pc advance */
    cpu->write_word(s2 + 36, a0);  /* sw a0,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80183FE4 */

block_80183FE4:
    v1 = v1 & v0;  /* 0x80183FE4: and v1,v1,v0 — mode5 & 1 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80183efc — chk_mode1_foe ========
 * Chk cond 17 (foe): res = foe mode5 & 1. Same FDC/FE4 alternate tail
 * as chk_mode1_self on s0. Faithful to generated br2eu_func_80183EFC. */
void chk_mode1_foe(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183EFCu: goto block_80183EFC;
            case 0x80183FDCu: goto block_80183FDC;
            case 0x80183FE4u: goto block_80183FE4;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183EFC:
    v1 = cpu->read_byte(s0 + 5);  /* lbu v1,5(s0) — foe mode5 */
    /* delay slot (always executes) */
    v0 = 1;  /* li v0,1 — mask + return value */
    psx_check_interrupts_at(cpu, 0x80183FDCu);
    goto block_80183FDC;  /* j */

block_80183FDC:
    a0 = s1 + 2;  /* addiu a0,s1,2 — pc advance */
    cpu->write_word(s2 + 36, a0);  /* sw a0,0x24(s2) — ctx+0x24 pc */
    /* fall through to block_80183FE4 */

block_80183FE4:
    v1 = v1 & v0;  /* 0x80183FE4: and v1,v1,v0 — foe mode5 & 1 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);  /* sw v1,0x38(s2) — ctx+0x38 result */
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184358:
    ra = cpu->read_word(sp + 36);  /* lw ra,0x24(sp) */
    s4 = cpu->read_word(sp + 32);  /* lw s4,0x20(sp) */
    s3 = cpu->read_word(sp + 28);  /* lw s3,0x1c(sp) */
    s2 = cpu->read_word(sp + 24);  /* lw s2,0x18(sp) */
    s1 = cpu->read_word(sp + 20);  /* lw s1,0x14(sp) */
    s0 = cpu->read_word(sp + 16);  /* lw s0,0x10(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;  /* addiu sp,sp,0x28 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}
