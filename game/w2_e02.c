/* w2_e02.c — clean CPS ports of fighter_script_fsm / scriptop_bra
 * piece-entries (guest 0x80182A58-0x80182FF0, SLES_017.22_full_38.c).
 *
 * All eight addrs are CPS piece-entries of the fighter AI/script FSM
 * (host bodies br2eu_psx_alias_body_80181F20 / _80182F48): reached via
 * `cpu->pc = <addr>` coroutine dispatches and the FSM inner-switch jump
 * table, NOT ordinary `jr $ra` ABI calls. They are routed through
 * game/clean_cps.map (CLEAN_ENT_CPS wrappers handing the fn
 * cpu->pc == the dispatched addr) exactly like the cps_clean.c bodies.
 * Each body is a faithful op-by-op transcription of the generated
 * br2eu_func_* piece: same guest ops, same order, same yield/continuation
 * points, minus cycle/icache/cosim/debug instrumentation.
 *
 * Shared ctx convention (same script-VM ctx as w2_e01's
 * fighter_script_fsm decode): s0=script ctx (+36 stream pc, +40
 * state halfword, +6/+10 flag halfwords, +42 sub-state), s1=script
 * descriptor (+88/+92 params, +204 ext ptr), t0=fighter/anim state
 * (+88/+100/+118). The fsm_mode* bodies are the mode-dispatch arms:
 * each evaluates ctx/fighter conditions and commits the next script
 * pc via ctx[36]; fsm_flags_commit / scriptop_bra_pc_commit are the
 * shared tail pieces. The ~750 blocks are per-condition eval arms —
 * annotate per-arm only when a specific mode needs RE work.
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

/* ======== 0x80182a58 ======== */
/* fsm_script_vm_gate — fighter_script_fsm CPS piece (guest 0x80182A58).
 * Tests foe fighter +0x60 bit15 (s1): when clear, CPS-jals script_op_dispatch
 * (0x80184F28) with (a0=s1 ctrl, a1=s3 param, a2=s0 script ctx, a3=s2 fight
 * ctx), then falls into the shared flags-commit tail. When set, skips the VM
 * call and goes straight to the tail. Faithful CPS transliteration of
 * generated br2eu_func_80182A58 (alias body br2eu_psx_alias_body_80181F20). */
void fsm_script_vm_gate(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182A6Cu: goto block_80182A6C;
            case 0x80182EE4u: goto block_80182EE4;
            case 0x80182EF8u: goto block_80182EF8;
            case 0x80182EFCu: goto block_80182EFC;
            case 0x80182F10u: goto block_80182F10;
            case 0x80182F20u: goto block_80182F20;
            default: break;
        }
    }

block_80182A58:
    v0 = cpu->read_word(s1 + 96);
    /* nop */  /* 0x80182A5C: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x80182A60: 0x30428000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182A68: 0x00000000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80182EE4u);
        goto block_80182EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182A6Cu);
        goto block_80182A6C;  /* not taken */
    }

block_80182A6C:
    a0 = s1;
    a1 = s3;
    a2 = s0;
    ra = 0x80182A80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x80184F28u);
    cpu->pc = 0x80184F28u; return;  /* CPS jal -> 0x80184F28 */

block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero) {
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
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_2) {
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
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80182ac0 ======== */
/* fsm_combat_mode_eval — fighter_script_fsm CPS piece (guest 0x80182AC0),
 * inner-switch mode-4 body: ctx+0x28 |= 0x18, then the melee/approach
 * evaluation (foe +0x58/+0x76/+0xba/+0xbc/+0x88/+0x74, self +0x5c/+0xcc
 * flags, script-byte tests) that picks the next AI decision flags. Shares the
 * post-switch eval tail with the other mode pieces. Faithful CPS
 * transliteration of generated br2eu_func_80182AC0. */
void fsm_combat_mode_eval(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182118u: goto block_80182118;
            case 0x8018212Cu: goto block_8018212C;
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
            case 0x80182D94u: goto block_80182D94;
            case 0x80182DECu: goto block_80182DEC;
            case 0x80182DF0u: goto block_80182DF0;
            case 0x80182E9Cu: goto block_80182E9C;
            case 0x80182EA8u: goto block_80182EA8;
            case 0x80182EC4u: goto block_80182EC4;
            case 0x80182EE4u: goto block_80182EE4;
            case 0x80182EF8u: goto block_80182EF8;
            case 0x80182EFCu: goto block_80182EFC;
            case 0x80182F10u: goto block_80182F10;
            case 0x80182F20u: goto block_80182F20;
            default: break;
        }
    }

block_80182AC0:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182AC4: 0x00000000 */
    v0 = v0 | 0x0018;
    cpu->write_half(s0 + 40, (uint16_t)v0);
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182AD4: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182ADC: 0x00000000 */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182AE0u);
        goto block_80182AE0;  /* not taken */
    }

block_80182118:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018211C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182120: 0x30420010 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero) {
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

block_80182AE0:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 118);
    /* nop */  /* 0x80182AE4: 0x00000000 */
    v0 = ((int32_t)v0 < 1024) ? 1 : 0;  /* 0x80182AE8: 0x28420400 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182AF0: 0x00000000 */
    if (cond_v0_eq_zero_2) {
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
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B14: 0x00000000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B18u);
        goto block_80182B18;  /* not taken */
    }

block_80182B18:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 188);
    /* nop */  /* 0x80182B1C: 0x00000000 */
    int cond_v0_gt_0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B24: 0x00000000 */
    if (cond_v0_gt_0) {
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
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B38: 0x00000000 */
    if (cond_v0_eq_zero_3) {
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
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4096;
    if (cond_v0_ne_zero_2) {
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
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B84: 0x00000000 */
    if (cond_v0_eq_zero_4) {
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
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B98: 0x00000000 */
    if (cond_v0_eq_zero_5) {
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
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_ne_zero_3) {
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
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0_2) {
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
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182BF8: 0x28C20201 */
    if (cond_v0_eq_zero_6) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182BFCu);
        goto block_80182BFC;  /* not taken */
    }

block_80182BFC:;
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xC000;
    if (cond_v0_eq_zero_7) {
        psx_check_interrupts_at(cpu, 0x80182DF0u);
        goto block_80182DF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C04u);
        goto block_80182C04;  /* not taken */
    }

block_80182C04:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182C08: 0x00000000 */
    int cond_v0_ge_0_3 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (cond_v0_ge_0_3) {
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
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x80182C40u);
        goto block_80182C40;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* not taken */
    }

block_80182C28:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182C2C: 0x00000000 */
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (cond_v0_eq_zero_9) {
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
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C48: 0x00000000 */
    if (cond_v0_ne_zero_4) {
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
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C5C: 0x00000000 */
    if (cond_v0_eq_zero_10) {
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
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x80182C84u);
        goto block_80182C84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C74u);
        goto block_80182C74;  /* not taken */
    }

block_80182C74:
    v1 = a0 & 0x0180;  /* 0x80182C74: 0x30830180 */
    v0 = 256;
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80182C98u);
        goto block_80182C98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C84u);
        goto block_80182C84;  /* not taken */
    }

block_80182C84:
    v0 = a0 & v0;  /* 0x80182C84: 0x00821024 */
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 17408;
    if (cond_v0_ne_zero_6) {
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
    int cond_v0_ne_zero_7 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_ne_zero_7) {
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

block_80182D94:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182DEC:
    v0 = 0xC000;
    /* fall through to block_80182DF0 */

block_80182DF0:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182E9C:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EA8:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EC4:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero_11 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero_11) {
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
    int cond_v0_eq_zero_12 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_12) {
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
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80182cac ======== */
/* fsm_mode1_eval — fighter_script_fsm CPS piece (guest 0x80182CAC),
 * inner-switch mode-1 body: foe +0x58 sign test -> ctx+0x2a = 4 path and the
 * shared mode-eval tail (param_3[10] bit tests, foe state fields, ctx+6 flag
 * commit). Faithful CPS transliteration of generated br2eu_func_80182CAC. */
void fsm_mode1_eval(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182118u: goto block_80182118;
            case 0x8018212Cu: goto block_8018212C;
            case 0x80182C28u: goto block_80182C28;
            case 0x80182C38u: goto block_80182C38;
            case 0x80182CBCu: goto block_80182CBC;
            case 0x80182CD4u: goto block_80182CD4;
            case 0x80182CE8u: goto block_80182CE8;
            case 0x80182CF8u: goto block_80182CF8;
            case 0x80182D0Cu: goto block_80182D0C;
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

block_80182CAC:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182CB0: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80182CE8u);
        goto block_80182CE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182CBCu);
        goto block_80182CBC;  /* not taken */
    }

block_80182118:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018211C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182120: 0x30420010 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero) {
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

block_80182C28:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182C2C: 0x00000000 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (cond_v0_eq_zero_2) {
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

block_80182CBC:
    cpu->write_half(s0 + 42, (uint16_t)v0);
    v0 = cpu->read_word(s1 + 92);
    v1 = 0x6000 << 16;  /* 0x60000000 */
    v0 = v0 & v1;  /* 0x80182CC8: 0x00431024 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182CD0: 0x00000000 */
    if (cond_v0_eq_zero_3) {
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
    int cond_v0_gt_0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;
    if (cond_v0_gt_0) {
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

block_80182D24:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182D28: 0x00000000 */
    v0 = (uint32_t)v0 >> 4;
    v0 = v0 & 0x0001;  /* 0x80182D30: 0x30420001 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero_4) {
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
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (cond_v0_eq_zero_5) {
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
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (cond_v0_eq_zero_6) {
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
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_eq_zero_7) {
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
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (cond_v0_eq_zero_8) {
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
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0_2) {
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
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (cond_v0_eq_zero_9) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:;
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (cond_v0_ne_zero) {
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
    int cond_v0_ge_0_3 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (cond_v0_ge_0_3) {
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
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (cond_v0_ne_zero_3) {
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
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (cond_v0_eq_zero_10) {
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
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (cond_v0_ne_zero_4) {
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
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (cond_v1_ne_v0) {
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
    int cond_v0_eq_zero_11 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (cond_v0_eq_zero_11) {
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
    int cond_v0_eq_zero_12 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_12) {
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

block_80182EA8:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EB0:
    v0 = cpu->read_word(t0 + 100);
    v1 = 0x0002 << 16;  /* 0x00020000 */
    v0 = v0 & v1;  /* 0x80182EB8: 0x00431024 */
    int cond_v0_eq_zero_13 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_eq_zero_13) {
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

block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero_14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero_14) {
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
    int cond_v0_eq_zero_15 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_15) {
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
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80182d14 ======== */
/* fsm_mode3_flag1000 — fighter_script_fsm CPS piece (guest 0x80182D14),
 * inner-switch mode-3 entry: ctx+6 = 0x1000 then joins the shared mode-eval
 * tail at 0x80182D20/0x80182D24. Faithful CPS transliteration of generated
 * br2eu_func_80182D14. */
void fsm_mode3_flag1000(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182118u: goto block_80182118;
            case 0x8018212Cu: goto block_8018212C;
            case 0x80182C28u: goto block_80182C28;
            case 0x80182C38u: goto block_80182C38;
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

block_80182D14:
    /* delay slot (always executes) */
    v0 = 4096;
    psx_check_interrupts_at(cpu, 0x80182D20u);
    goto block_80182D20;  /* j */

block_80182118:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018211C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182120: 0x30420010 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero) {
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

block_80182C28:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182C2C: 0x00000000 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (cond_v0_eq_zero_2) {
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

block_80182D20:
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182D24 */

block_80182D24:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182D28: 0x00000000 */
    v0 = (uint32_t)v0 >> 4;
    v0 = v0 & 0x0001;  /* 0x80182D30: 0x30420001 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero_3) {
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
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (cond_v0_eq_zero_4) {
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
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (cond_v0_eq_zero_5) {
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
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_eq_zero_6) {
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
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (cond_v0_eq_zero_7) {
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
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0) {
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
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:;
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (cond_v0_ne_zero) {
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
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (cond_v0_ge_0_2) {
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
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (cond_v0_ne_zero_3) {
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
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (cond_v0_eq_zero_9) {
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
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (cond_v0_ne_zero_4) {
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
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (cond_v1_ne_v0) {
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
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (cond_v0_eq_zero_10) {
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
    int cond_v0_eq_zero_11 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_11) {
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

block_80182EA8:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EB0:
    v0 = cpu->read_word(t0 + 100);
    v1 = 0x0002 << 16;  /* 0x00020000 */
    v0 = v0 & v1;  /* 0x80182EB8: 0x00431024 */
    int cond_v0_eq_zero_12 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_eq_zero_12) {
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

block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero_13 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero_13) {
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
    int cond_v0_eq_zero_14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_14) {
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
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80182d1c ======== */
/* fsm_mode2_flag4000 — fighter_script_fsm CPS piece (guest 0x80182D1C),
 * inner-switch mode-2 entry: ctx+6 = 0x4000 then joins the shared mode-eval
 * tail at 0x80182D20/0x80182D24. Faithful CPS transliteration of generated
 * br2eu_func_80182D1C. */
void fsm_mode2_flag4000(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182118u: goto block_80182118;
            case 0x8018212Cu: goto block_8018212C;
            case 0x80182C28u: goto block_80182C28;
            case 0x80182C38u: goto block_80182C38;
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

block_80182D1C:
    v0 = 16384;
    /* fall through to block_80182D20 */

block_80182118:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018211C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182120: 0x30420010 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero) {
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

block_80182C28:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182C2C: 0x00000000 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (cond_v0_eq_zero_2) {
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

block_80182D20:
    cpu->write_half(s0 + 6, (uint16_t)v0);
    /* fall through to block_80182D24 */

block_80182D24:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182D28: 0x00000000 */
    v0 = (uint32_t)v0 >> 4;
    v0 = v0 & 0x0001;  /* 0x80182D30: 0x30420001 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero_3) {
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
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (cond_v0_eq_zero_4) {
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
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (cond_v0_eq_zero_5) {
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
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_eq_zero_6) {
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
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (cond_v0_eq_zero_7) {
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
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0) {
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
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:;
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (cond_v0_ne_zero) {
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
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (cond_v0_ge_0_2) {
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
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (cond_v0_ne_zero_3) {
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
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (cond_v0_eq_zero_9) {
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
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (cond_v0_ne_zero_4) {
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
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (cond_v1_ne_v0) {
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
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (cond_v0_eq_zero_10) {
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
    int cond_v0_eq_zero_11 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_11) {
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

block_80182EA8:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EB0:
    v0 = cpu->read_word(t0 + 100);
    v1 = 0x0002 << 16;  /* 0x00020000 */
    v0 = v0 & v1;  /* 0x80182EB8: 0x00431024 */
    int cond_v0_eq_zero_12 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_eq_zero_12) {
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

block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero_13 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero_13) {
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
    int cond_v0_eq_zero_14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_14) {
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
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80182d24 ======== */
/* fsm_mode_eval — fighter_script_fsm CPS piece (guest 0x80182D24), the
 * post-switch shared mode evaluation: ctx+0x28 bit4 test, foe +0x58/+0x76/
 * +0x64/+0xd4/+0xcc/+0xba/+0xbc/+0x88 field checks and the ctx+6 decision-flag
 * writes (0x400/0x4400/0xc000/0) before the flags-commit tail. Faithful CPS
 * transliteration of generated br2eu_func_80182D24. */
void fsm_mode_eval(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182118u: goto block_80182118;
            case 0x8018212Cu: goto block_8018212C;
            case 0x80182C28u: goto block_80182C28;
            case 0x80182C38u: goto block_80182C38;
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

block_80182D24:
    v0 = cpu->read_half(s0 + 40);
    /* nop */  /* 0x80182D28: 0x00000000 */
    v0 = (uint32_t)v0 >> 4;
    v0 = v0 & 0x0001;  /* 0x80182D30: 0x30420001 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80182D9Cu);
        goto block_80182D9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182D3Cu);
        goto block_80182D3C;  /* not taken */
    }

block_80182118:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018211C: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80182120: 0x30420010 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_2) {
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

block_80182C28:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182C2C: 0x00000000 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (cond_v0_eq_zero_3) {
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

block_80182D3C:
    v0 = cpu->read_word(s1 + 92);
    /* nop */  /* 0x80182D40: 0x00000000 */
    v0 = v0 & v1;  /* 0x80182D44: 0x00431024 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (cond_v0_eq_zero_4) {
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
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (cond_v0_eq_zero_5) {
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
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (cond_v0_eq_zero_6) {
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
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (cond_v0_eq_zero_7) {
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
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (cond_v0_ge_0) {
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
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:;
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (cond_v0_ne_zero) {
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
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (cond_v0_ge_0_2) {
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
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (cond_v0_ne_zero_3) {
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
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (cond_v0_eq_zero_9) {
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
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (cond_v0_ne_zero_4) {
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
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (cond_v1_ne_v0) {
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
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (cond_v0_eq_zero_10) {
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
    int cond_v0_eq_zero_11 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (cond_v0_eq_zero_11) {
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

block_80182EA8:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x80182EE4u);
    goto block_80182EE4;  /* j */

block_80182EB0:
    v0 = cpu->read_word(t0 + 100);
    v1 = 0x0002 << 16;  /* 0x00020000 */
    v0 = v0 & v1;  /* 0x80182EB8: 0x00431024 */
    int cond_v0_eq_zero_12 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (cond_v0_eq_zero_12) {
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

block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero_13 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero_13) {
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
    int cond_v0_eq_zero_14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_14) {
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
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80182ee4 ======== */
/* fsm_flags_commit — fighter_script_fsm CPS piece (guest 0x80182EE4), the
 * shared decision-flags commit tail: folds ctx+6 bit10 into bit3, tests
 * ctx+0xA bit0, sets ctx+6 bit11, then runs the standard epilogue
 * (restore s0-s3+ra, sp += 40, jr ra). Reached from every mode-eval exit.
 * Faithful CPS transliteration of generated br2eu_func_80182EE4. */
void fsm_flags_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80182EF8u: goto block_80182EF8;
            case 0x80182EFCu: goto block_80182EFC;
            case 0x80182F10u: goto block_80182F10;
            case 0x80182F20u: goto block_80182F20;
            default: break;
        }
    }

block_80182EE4:
    v1 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182EE8: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x80182EEC: 0x30620400 */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (cond_v0_eq_zero) {
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
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (cond_v0_eq_zero_2) {
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
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x80182ff0 ======== */
/* scriptop_bra_pc_commit — scriptop_bra CPS piece (guest 0x80182FF0,
 * alias body br2eu_psx_alias_body_80182F48): the taken-branch commit —
 * ctx->pc (s1+0x24) = script_base (s3+0x30) + operand (lh s0+2), returns
 * v0 = 1 (continue), then the shared 0x8018322C epilogue. Faithful CPS
 * transliteration of generated br2eu_func_80182FF0. */
void scriptop_bra_pc_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018322Cu: goto block_8018322C;
            default: break;
        }
    }

block_80182FF0:
    v1 = cpu->read_half(s0 + 2);
    a0 = cpu->read_word(s3 + 48);
    v0 = 1;
    v1 = v1 + a0;
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v1);
    psx_check_interrupts_at(cpu, 0x8018322Cu);
    goto block_8018322C;  /* j */

block_8018322C:
    ra = cpu->read_word(sp + 64);
    s3 = cpu->read_word(sp + 60);
    s2 = cpu->read_word(sp + 56);
    s1 = cpu->read_word(sp + 52);
    s0 = cpu->read_word(sp + 48);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 72;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}
