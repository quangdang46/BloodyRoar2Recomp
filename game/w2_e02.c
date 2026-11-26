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
 */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef int8_t   byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef int      int32_t_ghidra;
#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern CPUState *clean_cpu_state(void);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st32(uint32_t a, uint32_t v);
extern uint8_t  br2_ld8(uint32_t a);
extern uint16_t br2_ld16(uint32_t a);
extern void     br2_st8(uint32_t a, uint8_t v);
extern void     br2_st16(uint32_t a, uint16_t v);
extern int      psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);
extern uint32_t br2_call_generated(uint32_t addr, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))

static void swl_guest(uint32_t addr, uint32_t v)
{
  uint32_t al = addr & ~3u;
  uint32_t w = DB32(al);
  switch (addr & 3u) {
  case 0: w = (w & 0xFFFFFF00u) | (v >> 24); break;
  case 1: w = (w & 0xFFFF0000u) | (v >> 16); break;
  case 2: w = (w & 0xFF000000u) | (v >> 8);  break;
  default: w = v; break;
  }
  DB32(al) = w;
}

static void prim_ot_link_indexed(uint32_t ot, uint32_t out, uint32_t otz)
{
  uint32_t slot = ot + (otz & 0x1ffcu);
  uint32_t old  = DB32(slot);
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
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
    int _bc_80182A64 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182A68: 0x00000000 */
    if (_bc_80182A64) {
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
    int _bc_80182EF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (_bc_80182EF0) {
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
    int _bc_80182F08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (_bc_80182F08) {
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
    uint32_t _jt_80182F34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80182F34);
    cpu->pc = _jt_80182F34; return;  /* CPS: jr $ra */
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
    int _bc_80182AD8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182ADC: 0x00000000 */
    if (_bc_80182AD8) {
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
    int _bc_80182124 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182124) {
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
    int _bc_80182AEC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182AF0: 0x00000000 */
    if (_bc_80182AEC) {
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
    int _bc_80182B10 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B14: 0x00000000 */
    if (_bc_80182B10) {
        psx_check_interrupts_at(cpu, 0x80182B74u);
        goto block_80182B74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182B18u);
        goto block_80182B18;  /* not taken */
    }

block_80182B18:
    v0 = (int32_t)(int16_t)cpu->read_half(t0 + 188);
    /* nop */  /* 0x80182B1C: 0x00000000 */
    int _bc_80182B20 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B24: 0x00000000 */
    if (_bc_80182B20) {
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
    int _bc_80182B34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B38: 0x00000000 */
    if (_bc_80182B34) {
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
    int _bc_80182B64 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4096;
    if (_bc_80182B64) {
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
    int _bc_80182B80 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B84: 0x00000000 */
    if (_bc_80182B80) {
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
    int _bc_80182B94 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182B98: 0x00000000 */
    if (_bc_80182B94) {
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
    int _bc_80182BC4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (_bc_80182BC4) {
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
    int _bc_80182BE4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (_bc_80182BE4) {
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
    int _bc_80182BF4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182BF8: 0x28C20201 */
    if (_bc_80182BF4) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182BFCu);
        goto block_80182BFC;  /* not taken */
    }

block_80182BFC:
    int _bc_80182BFC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xC000;
    if (_bc_80182BFC) {
        psx_check_interrupts_at(cpu, 0x80182DF0u);
        goto block_80182DF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C04u);
        goto block_80182C04;  /* not taken */
    }

block_80182C04:
    v0 = cpu->read_word(t0 + 88);
    /* nop */  /* 0x80182C08: 0x00000000 */
    int _bc_80182C0C = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_80182C0C) {
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
    int _bc_80182C20 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182C20) {
        psx_check_interrupts_at(cpu, 0x80182C40u);
        goto block_80182C40;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* not taken */
    }

block_80182C28:
    v0 = cpu->read_half(s0 + 6);
    /* nop */  /* 0x80182C2C: 0x00000000 */
    int _bc_80182C30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (_bc_80182C30) {
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
    int _bc_80182C44 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C48: 0x00000000 */
    if (_bc_80182C44) {
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
    int _bc_80182C58 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C5C: 0x00000000 */
    if (_bc_80182C58) {
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
    int _bc_80182C6C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (_bc_80182C6C) {
        psx_check_interrupts_at(cpu, 0x80182C84u);
        goto block_80182C84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C74u);
        goto block_80182C74;  /* not taken */
    }

block_80182C74:
    v1 = a0 & 0x0180;  /* 0x80182C74: 0x30830180 */
    v0 = 256;
    int _bc_80182C7C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0002 << 16;  /* 0x00020000 */
    if (_bc_80182C7C) {
        psx_check_interrupts_at(cpu, 0x80182C98u);
        goto block_80182C98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182C84u);
        goto block_80182C84;  /* not taken */
    }

block_80182C84:
    v0 = a0 & v0;  /* 0x80182C84: 0x00821024 */
    int _bc_80182C88 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 17408;
    if (_bc_80182C88) {
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
    int _bc_80182C9C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (_bc_80182C9C) {
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
    int _bc_80182EF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (_bc_80182EF0) {
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
    int _bc_80182F08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (_bc_80182F08) {
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
    uint32_t _jt_80182F34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80182F34);
    cpu->pc = _jt_80182F34; return;  /* CPS: jr $ra */
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
    int _bc_80182CB4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;
    if (_bc_80182CB4) {
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
    int _bc_80182124 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182124) {
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
    int _bc_80182C30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (_bc_80182C30) {
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
    int _bc_80182CCC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182CD0: 0x00000000 */
    if (_bc_80182CCC) {
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
    int _bc_80182CF0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 + -1;
    if (_bc_80182CF0) {
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
    int _bc_80182D34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182D34) {
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
    int _bc_80182D48 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (_bc_80182D48) {
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
    int _bc_80182D5C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (_bc_80182D5C) {
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
    int _bc_80182D8C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (_bc_80182D8C) {
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
    int _bc_80182DAC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (_bc_80182DAC) {
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
    int _bc_80182DCC = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (_bc_80182DCC) {
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
    int _bc_80182DDC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (_bc_80182DDC) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:
    int _bc_80182DE4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (_bc_80182DE4) {
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
    int _bc_80182E00 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_80182E00) {
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
    int _bc_80182E14 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182E14) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int _bc_80182E20 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (_bc_80182E20) {
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
    int _bc_80182E34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (_bc_80182E34) {
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
    int _bc_80182E48 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (_bc_80182E48) {
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
    int _bc_80182E5C = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a0 & 0x0180;  /* 0x80182E60: 0x30830180 */
    if (_bc_80182E5C) {
        psx_check_interrupts_at(cpu, 0x80182E84u);
        goto block_80182E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E64u);
        goto block_80182E64;  /* not taken */
    }

block_80182E64:
    v0 = 256;
    int _bc_80182E68 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (_bc_80182E68) {
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
    int _bc_80182E7C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (_bc_80182E7C) {
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
    int _bc_80182E90 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182E90) {
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
    int _bc_80182EBC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (_bc_80182EBC) {
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
    int _bc_80182EF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (_bc_80182EF0) {
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
    int _bc_80182F08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (_bc_80182F08) {
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
    uint32_t _jt_80182F34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80182F34);
    cpu->pc = _jt_80182F34; return;  /* CPS: jr $ra */
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
    int _bc_80182124 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182124) {
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
    int _bc_80182C30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (_bc_80182C30) {
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
    int _bc_80182D34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182D34) {
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
    int _bc_80182D48 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (_bc_80182D48) {
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
    int _bc_80182D5C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (_bc_80182D5C) {
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
    int _bc_80182D8C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (_bc_80182D8C) {
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
    int _bc_80182DAC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (_bc_80182DAC) {
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
    int _bc_80182DCC = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (_bc_80182DCC) {
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
    int _bc_80182DDC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (_bc_80182DDC) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:
    int _bc_80182DE4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (_bc_80182DE4) {
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
    int _bc_80182E00 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_80182E00) {
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
    int _bc_80182E14 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182E14) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int _bc_80182E20 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (_bc_80182E20) {
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
    int _bc_80182E34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (_bc_80182E34) {
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
    int _bc_80182E48 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (_bc_80182E48) {
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
    int _bc_80182E5C = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a0 & 0x0180;  /* 0x80182E60: 0x30830180 */
    if (_bc_80182E5C) {
        psx_check_interrupts_at(cpu, 0x80182E84u);
        goto block_80182E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E64u);
        goto block_80182E64;  /* not taken */
    }

block_80182E64:
    v0 = 256;
    int _bc_80182E68 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (_bc_80182E68) {
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
    int _bc_80182E7C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (_bc_80182E7C) {
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
    int _bc_80182E90 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182E90) {
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
    int _bc_80182EBC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (_bc_80182EBC) {
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
    int _bc_80182EF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (_bc_80182EF0) {
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
    int _bc_80182F08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (_bc_80182F08) {
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
    uint32_t _jt_80182F34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80182F34);
    cpu->pc = _jt_80182F34; return;  /* CPS: jr $ra */
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
    int _bc_80182124 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182124) {
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
    int _bc_80182C30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (_bc_80182C30) {
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
    int _bc_80182D34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182D34) {
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
    int _bc_80182D48 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (_bc_80182D48) {
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
    int _bc_80182D5C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (_bc_80182D5C) {
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
    int _bc_80182D8C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (_bc_80182D8C) {
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
    int _bc_80182DAC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (_bc_80182DAC) {
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
    int _bc_80182DCC = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (_bc_80182DCC) {
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
    int _bc_80182DDC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (_bc_80182DDC) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:
    int _bc_80182DE4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (_bc_80182DE4) {
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
    int _bc_80182E00 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_80182E00) {
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
    int _bc_80182E14 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182E14) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int _bc_80182E20 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (_bc_80182E20) {
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
    int _bc_80182E34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (_bc_80182E34) {
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
    int _bc_80182E48 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (_bc_80182E48) {
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
    int _bc_80182E5C = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a0 & 0x0180;  /* 0x80182E60: 0x30830180 */
    if (_bc_80182E5C) {
        psx_check_interrupts_at(cpu, 0x80182E84u);
        goto block_80182E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E64u);
        goto block_80182E64;  /* not taken */
    }

block_80182E64:
    v0 = 256;
    int _bc_80182E68 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (_bc_80182E68) {
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
    int _bc_80182E7C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (_bc_80182E7C) {
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
    int _bc_80182E90 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182E90) {
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
    int _bc_80182EBC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (_bc_80182EBC) {
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
    int _bc_80182EF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (_bc_80182EF0) {
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
    int _bc_80182F08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (_bc_80182F08) {
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
    uint32_t _jt_80182F34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80182F34);
    cpu->pc = _jt_80182F34; return;  /* CPS: jr $ra */
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
    int _bc_80182D34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182D34) {
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
    int _bc_80182124 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182124) {
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
    int _bc_80182C30 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182C34: 0x00000000 */
    if (_bc_80182C30) {
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
    int _bc_80182D48 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D4C: 0x00000000 */
    if (_bc_80182D48) {
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
    int _bc_80182D5C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182D60: 0x00000000 */
    if (_bc_80182D5C) {
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
    int _bc_80182D8C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8192;
    if (_bc_80182D8C) {
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
    int _bc_80182DAC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DB0: 0x00000000 */
    if (_bc_80182DAC) {
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
    int _bc_80182DCC = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = v0;
    if (_bc_80182DCC) {
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
    int _bc_80182DDC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)a2 < 513) ? 1 : 0;  /* 0x80182DE0: 0x28C20201 */
    if (_bc_80182DDC) {
        psx_check_interrupts_at(cpu, 0x80182DECu);
        goto block_80182DEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182DE4u);
        goto block_80182DE4;  /* not taken */
    }

block_80182DE4:
    int _bc_80182DE4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182DE8: 0x00000000 */
    if (_bc_80182DE4) {
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
    int _bc_80182E00 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x0200 << 16;  /* 0x02000000 */
    if (_bc_80182E00) {
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
    int _bc_80182E14 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_80182E14) {
        psx_check_interrupts_at(cpu, 0x80182C28u);
        goto block_80182C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E1Cu);
        goto block_80182E1C;  /* not taken */
    }

block_80182E1C:
    v0 = v1 & v0;  /* 0x80182E1C: 0x00621024 */
    int _bc_80182E20 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E24: 0x00000000 */
    if (_bc_80182E20) {
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
    int _bc_80182E34 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E38: 0x00000000 */
    if (_bc_80182E34) {
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
    int _bc_80182E48 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 66;
    if (_bc_80182E48) {
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
    int _bc_80182E5C = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a0 & 0x0180;  /* 0x80182E60: 0x30830180 */
    if (_bc_80182E5C) {
        psx_check_interrupts_at(cpu, 0x80182E84u);
        goto block_80182E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80182E64u);
        goto block_80182E64;  /* not taken */
    }

block_80182E64:
    v0 = 256;
    int _bc_80182E68 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E6C: 0x00000000 */
    if (_bc_80182E68) {
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
    int _bc_80182E7C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182E80: 0x00000000 */
    if (_bc_80182E7C) {
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
    int _bc_80182E90 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16384;
    if (_bc_80182E90) {
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
    int _bc_80182EBC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1024;
    if (_bc_80182EBC) {
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
    int _bc_80182EF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (_bc_80182EF0) {
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
    int _bc_80182F08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (_bc_80182F08) {
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
    uint32_t _jt_80182F34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80182F34);
    cpu->pc = _jt_80182F34; return;  /* CPS: jr $ra */
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
    int _bc_80182EF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0008;
    if (_bc_80182EF0) {
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
    int _bc_80182F08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80182F0C: 0x00000000 */
    if (_bc_80182F08) {
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
    uint32_t _jt_80182F34 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80182F34);
    cpu->pc = _jt_80182F34; return;  /* CPS: jr $ra */
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
    uint32_t _jt_80183240 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 72;
    psx_check_interrupts_at(cpu, _jt_80183240);
    cpu->pc = _jt_80183240; return;  /* CPS: jr $ra */
}
