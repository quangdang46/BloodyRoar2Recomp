/* w5_e00.c — clean CPS ports for unnamed jal-target functions in the
 * CD/kernel-object region (0x8019A350 / 0x8019A364 / 0x8019E4C8 /
 * 0x8019E55C / 0x8019E5CC).
 *
 * All five are CPS piece-entries: they perform guest ops then tail-dispatch
 * via cpu->pc (the clean-CPS idiom — see cps_clean.c). Registered in
 * game/renames.map (canonical names) and game/clean_cps.map (entry +
 * continuation addrs -> CLEAN_ENT_CPS so cpu->pc is preserved across
 * yields and tail dispatches).
 *
 *   0x8019a350 / 0x8019a364 — kobj method tail-calls: `lw t1, mem[0x801FEB58
 *               /0x801FEB5C]; jr t1`. The cells hold OpenBIOS kernel-object
 *               method pointers installed by cd_kobj_init_b (sub+0x884 /
 *               sub+0x894). Called from cd_chan_reset_seq.
 *   0x8019e4c8 — DI; B0:0x57-family kobj call (t1=0x56, jr 0xB0 gate) with
 *               live a0-a3 -> obj; dst = (*(obj+0x18)+0x70/0x74 packed)
 *               +0x28; copy 5 words 0x801AE4A0..0x801AE4B4 -> dst;
 *               *(0x0000DFFC) = dst_end; jal 0x801957F0 (bios_tramp_a044).
 *               No EI — faithful to the guest epilogue (see bind.c
 *               FUN_8019e4c8 / proof/game_gfx_bind_audit.md).
 *   0x8019e55c — same shape, t1=0x57: p = *(obj+0x16C); copy 5 words
 *               0x801AE4B4..0x801AE4C8 -> p+0x9C8..; jal bios_tramp_a044.
 *   0x8019e5cc — pure copy: 28 words 0x801AE430..0x801AE4A0 -> 0x0000DF80,
 *               jr ra. No calls.
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

/* ======== cd_kobj_mcall_58 (guest 0x8019a350) ======== */

/* Faithful CPS transliteration of generated br2eu_func_8019A350
 * (SLES_017.22_full_46.c): lui t1,0x8020; lw t1,-5288(t1) -> *0x801FEB58;
 * jr t1. Indirect tail-call through the kobj method cell — the loaded
 * target is left in cpu->pc for the dispatcher. */
void cd_kobj_mcall_58(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8019A350:
    t1 = 0x8020 << 16;  /* 0x80200000 */
    t1 = cpu->read_word(t1 + -5288);
    /* nop */  /* 0x8019A358: 0x00000000 */
    uint32_t _jt_8019A35C = t1;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A360: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019A35C);
    cpu->pc = _jt_8019A35C; return;  /* CPS: jr t1 */
}

/* ======== cd_kobj_mcall_5c (guest 0x8019a364) ======== */

/* Faithful CPS transliteration of generated br2eu_func_8019A364:
 * same shape, loads *0x801FEB5C (sub+0x894 method ptr). */
void cd_kobj_mcall_5c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8019A364:
    t1 = 0x8020 << 16;  /* 0x80200000 */
    t1 = cpu->read_word(t1 + -5284);
    /* nop */  /* 0x8019A36C: 0x00000000 */
    uint32_t _jt_8019A370 = t1;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A374: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019A370);
    cpu->pc = _jt_8019A370; return;  /* CPS: jr t1 */
}

/* ======== cd_kobj_b56_commit (guest 0x8019e4c8) ======== */

/* Faithful CPS transliteration of generated br2eu_func_8019E4C8
 * (SLES_017.22_full_48.c): save $ra -> 0x801FEB90, jal cpu_critical_enter
 * (DI), kobj call B0:0x56 via the 0xB0 gate with live a0-a3, copy 5 words
 * 0x801AE4A0..0x801AE4B4 -> computed dst, store dst_end -> 0x0000DFFC,
 * jal bios_tramp_a044, restore $ra, jr $ra. cpu->pc continuation switch
 * covers the jal/jalr return addrs and the copy-loop yield points. */
void cd_kobj_b56_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019E4D8u: goto block_8019E4D8;
            case 0x8019E4E8u: goto block_8019E4E8;
            case 0x8019E548u: goto block_8019E548;
            case 0x8019E524u: goto block_8019E524;
            case 0x8019E53Cu: goto block_8019E53C;
            default: break;
        }
    }
    /* Address: 0x8019E4C8, Size: 148 bytes, Blocks: 6 */

block_8019E4C8:
    at = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(at + -5232, ra);
    ra = 0x8019E4D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E4D4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019E4D8:
    t1 = 86;
    t2 = 176;
    uint32_t _jt_8019E4E0 = t2;  /* latch indirect target before delay slot */
    ra = 0x8019E4E8u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E4E4: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019E4E0);
    cpu->pc = _jt_8019E4E0; return;  /* CPS jalr */

block_8019E4E8:
    v0 = cpu->read_word(v0 + 24);
    /* nop */  /* 0x8019E4EC: 0x00000000 */
    v1 = cpu->read_word(v0 + 112);
    /* nop */  /* 0x8019E4F4: 0x00000000 */
    t1 = v1 & 0xFFFF;  /* 0x8019E4F8: 0x3069FFFF */
    t1 = t1 << 16;
    v1 = cpu->read_word(v0 + 116);
    /* nop */  /* 0x8019E504: 0x00000000 */
    t2 = v1 & 0xFFFF;  /* 0x8019E508: 0x306AFFFF */
    v1 = t1 + t2;
    v0 = v1 + 40;
    t2 = 0x801A << 16;  /* 0x801A0000 */
    t2 = t2 + -7008;
    t1 = 0x801A << 16;  /* 0x801A0000 */
    t1 = t1 + -6988;
    /* fall through to block_8019E524 */

block_8019E524:
    v1 = cpu->read_word(t2);
    /* nop */  /* 0x8019E528: 0x00000000 */
    cpu->write_word(v0, v1);
    t2 = t2 + 4;
    int _bc_8019E534 = (t2 != t1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v0 + 4;
    if (_bc_8019E534) {
        psx_check_interrupts_at(cpu, 0x8019E524u);
        goto block_8019E524;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019E53Cu);
        goto block_8019E53C;  /* not taken */
    }

block_8019E53C:
    at = 0x0001 << 16;  /* 0x00010000 */
    ra = 0x8019E548u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(at + -8196, v0);
    psx_check_interrupts_at(cpu, 0x801957F0u);
    cpu->pc = 0x801957F0u; return;  /* CPS jal -> 0x801957F0 */

block_8019E548:
    ra = 0x8020 << 16;  /* 0x80200000 */
    ra = cpu->read_word(ra + -5232);
    /* nop */  /* 0x8019E550: 0x00000000 */
    uint32_t _jt_8019E554 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E558: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019E554);
    cpu->pc = _jt_8019E554; return;  /* CPS: jr $ra */
}

/* ======== cd_kobj_b57_commit (guest 0x8019e55c) ======== */

/* Faithful CPS transliteration of generated br2eu_func_8019E55C
 * (SLES_017.22_full_48.c): same prologue/kobj-call shape with t1=0x57;
 * p = *(obj+0x16C); copy 5 words 0x801AE4B4..0x801AE4C8 -> p+0x9C8..;
 * jal bios_tramp_a044; restore $ra; jr $ra. */
void cd_kobj_b57_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019E56Cu: goto block_8019E56C;
            case 0x8019E57Cu: goto block_8019E57C;
            case 0x8019E5B8u: goto block_8019E5B8;
            case 0x8019E598u: goto block_8019E598;
            case 0x8019E5B0u: goto block_8019E5B0;
            default: break;
        }
    }
    /* Address: 0x8019E55C, Size: 112 bytes, Blocks: 6 */

block_8019E55C:
    at = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(at + -5232, ra);
    ra = 0x8019E56Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E568: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019E56C:
    t1 = 87;
    t2 = 176;
    uint32_t _jt_8019E574 = t2;  /* latch indirect target before delay slot */
    ra = 0x8019E57Cu;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E578: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019E574);
    cpu->pc = _jt_8019E574; return;  /* CPS jalr */

block_8019E57C:
    v0 = cpu->read_word(v0 + 364);
    /* nop */  /* 0x8019E580: 0x00000000 */
    v1 = cpu->read_word(v0 + 2504);
    t2 = 0x801A << 16;  /* 0x801A0000 */
    t2 = t2 + -6988;
    t1 = 0x801A << 16;  /* 0x801A0000 */
    t1 = t1 + -6968;
    /* fall through to block_8019E598 */

block_8019E598:
    t0 = cpu->read_word(t2);
    /* nop */  /* 0x8019E59C: 0x00000000 */
    cpu->write_word(v0 + 2504, t0);
    t2 = t2 + 4;
    int _bc_8019E5A8 = (t2 != t1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v0 + 4;
    if (_bc_8019E5A8) {
        psx_check_interrupts_at(cpu, 0x8019E598u);
        goto block_8019E598;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019E5B0u);
        goto block_8019E5B0;  /* not taken */
    }

block_8019E5B0:
    ra = 0x8019E5B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E5B4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801957F0u);
    cpu->pc = 0x801957F0u; return;  /* CPS jal -> 0x801957F0 */

block_8019E5B8:
    ra = 0x8020 << 16;  /* 0x80200000 */
    ra = cpu->read_word(ra + -5232);
    /* nop */  /* 0x8019E5C0: 0x00000000 */
    uint32_t _jt_8019E5C4 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E5C8: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019E5C4);
    cpu->pc = _jt_8019E5C4; return;  /* CPS: jr $ra */
}

/* ======== cd_kobj_param_copy (guest 0x8019e5cc) ======== */

/* Faithful CPS transliteration of generated br2eu_func_8019E5CC
 * (SLES_017.22_full_48.c): v0=0xDF80; copy 28 words
 * 0x801AE430..0x801AE4A0 -> 0x0000DF80..; jr ra. No calls — pure copy. */
void cd_kobj_param_copy(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019E5E0u: goto block_8019E5E0;
            case 0x8019E5F8u: goto block_8019E5F8;
            default: break;
        }
    }
    /* Address: 0x8019E5CC, Size: 52 bytes, Blocks: 3 */

block_8019E5CC:
    v0 = 0xDF80;
    t2 = 0x801A << 16;  /* 0x801A0000 */
    t2 = t2 + -7120;
    t1 = 0x801A << 16;  /* 0x801A0000 */
    t1 = t1 + -7008;
    /* fall through to block_8019E5E0 */

block_8019E5E0:
    v1 = cpu->read_word(t2);
    /* nop */  /* 0x8019E5E4: 0x00000000 */
    cpu->write_word(v0, v1);
    t2 = t2 + 4;
    int _bc_8019E5F0 = (t2 != t1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v0 + 4;
    if (_bc_8019E5F0) {
        psx_check_interrupts_at(cpu, 0x8019E5E0u);
        goto block_8019E5E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019E5F8u);
        goto block_8019E5F8;  /* not taken */
    }

block_8019E5F8:
    uint32_t _jt_8019E5F8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E5FC: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019E5F8);
    cpu->pc = _jt_8019E5F8; return;  /* CPS: jr $ra */
}
