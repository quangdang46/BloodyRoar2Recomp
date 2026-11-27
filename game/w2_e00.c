/* w2_e00.c — clean CPS ports of anim/script/render-boundary piece-entries
 * (guest 0x801800A4-0x80181F18, SLES_017.22_full_37.c).
 *
 * All eight addrs are CPS split-piece entries/continuations of logical
 * functions already clean-ported at their entries:
 *   0x801800A4/0x801802CC/0x8018098C  -> hitbox_corners_xform (0x8017FE60)
 *   0x80181000                        -> fighter_pair_update (0x801809C8)
 *   0x80181EE8/EF0/F10/F18            -> ani_fixup_guest   (0x80181DB8)
 * They are reached via `cpu->pc = <addr>` coroutine dispatches (jal links,
 * branch splits, fallthrough pieces) — NOT ordinary `jr $ra` calls — so
 * every dispatch addr (entries + continuation PCs) is routed through
 * game/clean_cps.map (CLEAN_ENT_CPS wrappers that hand the fn
 * cpu->pc == the dispatched addr). Each body is a faithful op-by-op
 * transcription of the generated br2eu_func_* body: same guest ops,
 * same order, same yield/continuation points, minus cycle/icache/cosim/
 * debug instrumentation.
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

/* ======== 0x801800a4 — hitbox_xform_node_commit ========
 * CPS piece-group of hitbox_corners_xform (guest entry 0x8017FE60,
 * game/battle.c). Reached as the fallthrough split-piece after the
 * parent's first node_xform_apply section, and via CPS jal-return
 * continuations 0x801800D4/134/140/184/190. Applies node_xform_apply
 * (*(node+0x44), node+0x20, 0), commits the rotated s3+0xd4..0xd8
 * corner deltas into the s2 record (+0x18..0x1c) and s4+0x9a..0x9e
 * accumulators, mirrors the node's +4..+0x14 fields to +0x20..+0x30
 * and +0x18..+0x1c (lh) to +0x34..+0x3c, then `j 0x8018098C` into the
 * shared epilogue piece. Faithful to generated br2eu_func_801800A4. */
void hitbox_xform_node_commit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801800D4u: goto block_801800D4;
            case 0x80180134u: goto block_80180134;
            case 0x80180140u: goto block_80180140;
            case 0x80180184u: goto block_80180184;
            case 0x80180190u: goto block_80180190;
            default: break;
        }
    }
    /* Address: 0x801800A4, Size: 552 bytes, Blocks: 6 */

block_801800A4:
    /* nop */  /* 0x801800A4: 0x00000000 */
    cpu->write_word(v1 + 56, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x801800B0: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 28);
    /* nop */  /* 0x801800B8: 0x00000000 */
    cpu->write_word(v1 + 60, v0);
    a1 = cpu->read_word(s7 + 32);
    a2 = zero;
    a0 = cpu->read_word(a1 + 68);
    ra = 0x801800D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 32;
    psx_check_interrupts_at(cpu, 0x8018D788u);
    cpu->pc = 0x8018D788u; return;  /* CPS jal -> 0x8018D788 */

block_801800D4:
    a3 = (int32_t)(int16_t)cpu->read_half(s3 + 224);
    fp = (int32_t)(int16_t)cpu->read_half(s3 + 220);
    cpu->write_word(sp + 16, a3);
    v0 = cpu->read_half(s5 + 212);
    s6 = (int32_t)(int16_t)cpu->read_half(s3 + 208);
    s1 = cpu->read_half(s3 + 222);
    cpu->write_half(s3 + 212, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 214);
    /* nop */  /* 0x801800F4: 0x00000000 */
    cpu->write_half(s3 + 214, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 216);
    /* nop */  /* 0x80180100: 0x00000000 */
    cpu->write_half(s3 + 216, (uint16_t)v0);
    s2 = cpu->read_word(s4 + 32);
    v0 = cpu->read_half(s3 + 212);
    /* nop */  /* 0x80180110: 0x00000000 */
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 214);
    a0 = s6;
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 216);
    s1 = s1 << 16;
    ra = 0x80180134u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s2 + 28, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80180134:
    a0 = s6;
    ra = 0x80180140u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80180140:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x8018014C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    a0 = s6;
    v0 = cpu->read_half(s4 + 156);
    s1 = (int32_t)s1 >> 20;
    v0 = v0 + s1;
    cpu->write_half(s4 + 156, (uint16_t)v0);
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 - t0;
    v1 = cpu->read_half(s4 + 154);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    ra = 0x80180184u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s4 + 154, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80180184:
    a0 = s6;
    ra = 0x80180190u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80180190:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x8018019C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 + t0;
    v1 = cpu->read_half(s4 + 158);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    cpu->write_half(s4 + 158, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 24);
    v1 = cpu->read_half(s4 + 154);
    /* nop */  /* 0x801801C4: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s4 + 156);
    a1 = cpu->read_half(s2 + 52);
    v0 = v0 + v1;
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    a0 = cpu->read_half(s4 + 158);
    v1 = cpu->read_half(s2 + 24);
    v0 = v0 + a0;
    v1 = v1 - a1;
    cpu->write_half(s2 + 28, (uint16_t)v0);
    cpu->write_half(s4 + 154, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s2 + 56);
    /* nop */  /* 0x80180208: 0x00000000 */
    v0 = v0 - v1;
    cpu->write_half(s4 + 156, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    v1 = cpu->read_half(s2 + 60);
    a0 = cpu->read_word(s4 + 32);
    v0 = v0 - v1;
    cpu->write_half(s4 + 158, (uint16_t)v0);
    v0 = cpu->read_word(a0 + 4);
    /* nop */  /* 0x8018022C: 0x00000000 */
    cpu->write_word(a0 + 32, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180238: 0x00000000 */
    v0 = cpu->read_word(v1 + 8);
    /* nop */  /* 0x80180240: 0x00000000 */
    cpu->write_word(v1 + 36, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x8018024C: 0x00000000 */
    v0 = cpu->read_word(v1 + 12);
    /* nop */  /* 0x80180254: 0x00000000 */
    cpu->write_word(v1 + 40, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180260: 0x00000000 */
    v0 = cpu->read_word(v1 + 16);
    /* nop */  /* 0x80180268: 0x00000000 */
    cpu->write_word(v1 + 44, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180274: 0x00000000 */
    v0 = cpu->read_word(v1 + 20);
    /* nop */  /* 0x8018027C: 0x00000000 */
    cpu->write_word(v1 + 48, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180288: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 24);
    /* nop */  /* 0x80180290: 0x00000000 */
    cpu->write_word(v1 + 52, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x8018029C: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 26);
    /* nop */  /* 0x801802A4: 0x00000000 */
    cpu->write_word(v1 + 56, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x801802B0: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 28);
    /* nop */  /* 0x801802B8: 0x00000000 */
    cpu->write_word(v1 + 60, v0);
    a1 = cpu->read_word(s4 + 32);
    /* delay slot (always executes) */
    a2 = zero;
    psx_check_interrupts_at(cpu, 0x8018098Cu);
    cpu->pc = 0x8018098Cu; return;  /* CPS j: split */
}

/* ======== 0x801802cc — hitbox_xform_flag_pair ========
 * CPS piece-group of hitbox_corners_xform: the param_2 flag-0x60000
 * test and BOTH transform variants (the flag-clear `&= 0xfffdffff`
 * path at block_801802E0 and the plain path at block_80180564), each
 * ending in the node_xform_apply + corner-commit sequence, then
 * `j 0x8018098C` into the shared epilogue. Continuations:
 * 0x801802E0/364/370/3B4/3C0/504/564/5DC/5E8/62C/638/77C/7F0/7F8/
 * 804/848/854. Faithful to generated br2eu_func_801802CC. */
void hitbox_xform_flag_pair(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80180364u: goto block_80180364;
            case 0x80180370u: goto block_80180370;
            case 0x801803B4u: goto block_801803B4;
            case 0x801803C0u: goto block_801803C0;
            case 0x80180504u: goto block_80180504;
            case 0x801805DCu: goto block_801805DC;
            case 0x801805E8u: goto block_801805E8;
            case 0x8018062Cu: goto block_8018062C;
            case 0x80180638u: goto block_80180638;
            case 0x8018077Cu: goto block_8018077C;
            case 0x801807F8u: goto block_801807F8;
            case 0x80180804u: goto block_80180804;
            case 0x80180848u: goto block_80180848;
            case 0x80180854u: goto block_80180854;
            case 0x801802E0u: goto block_801802E0;
            case 0x80180564u: goto block_80180564;
            case 0x801807F0u: goto block_801807F0;
            default: break;
        }
    }
    /* Address: 0x801802CC, Size: 1728 bytes, Blocks: 18 */

block_801802CC:
    v1 = cpu->read_word(s7 + 108);
    /* nop */  /* 0x801802D0: 0x00000000 */
    v0 = v1 & a0;  /* 0x801802D4: 0x00641024 */
    int _bc_801802D8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xFFFD << 16;  /* 0xFFFD0000 */
    if (_bc_801802D8) {
        psx_check_interrupts_at(cpu, 0x80180564u);
        goto block_80180564;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801802E0u);
        goto block_801802E0;  /* not taken */
    }

block_801802E0:
    v0 = v0 | 0xFFFF;
    v0 = v1 & v0;  /* 0x801802E4: 0x00621024 */
    cpu->write_word(s7 + 108, v0);
    a3 = (int32_t)(int16_t)cpu->read_half(s3 + 224);
    s2 = cpu->read_word(s4 + 32);
    fp = (int32_t)(int16_t)cpu->read_half(s3 + 220);
    cpu->write_word(sp + 16, a3);
    v0 = cpu->read_half(s3 + 212);
    s6 = (int32_t)(int16_t)cpu->read_half(s3 + 208);
    s1 = cpu->read_half(s3 + 222);
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 214);
    /* nop */  /* 0x80180310: 0x00000000 */
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 216);
    /* nop */  /* 0x8018031C: 0x00000000 */
    cpu->write_half(s2 + 28, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 212);
    v1 = cpu->read_half(s4 + 154);
    /* nop */  /* 0x8018032C: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s3 + 212, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 214);
    v1 = cpu->read_half(s4 + 156);
    a0 = s6;
    v0 = v0 + v1;
    cpu->write_half(s3 + 214, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 216);
    v1 = cpu->read_half(s4 + 158);
    s1 = s1 << 16;
    v0 = v0 + v1;
    ra = 0x80180364u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s3 + 216, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80180364:
    a0 = s6;
    ra = 0x80180370u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80180370:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x8018037C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    a0 = s6;
    v0 = cpu->read_half(s4 + 156);
    s1 = (int32_t)s1 >> 20;
    v0 = v0 + s1;
    cpu->write_half(s4 + 156, (uint16_t)v0);
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 - t0;
    v1 = cpu->read_half(s4 + 154);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    ra = 0x801803B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s4 + 154, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_801803B4:
    a0 = s6;
    ra = 0x801803C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_801803C0:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x801803CC: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 + t0;
    v1 = cpu->read_half(s4 + 158);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    cpu->write_half(s4 + 158, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 24);
    v1 = cpu->read_half(s4 + 154);
    /* nop */  /* 0x801803F4: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s4 + 156);
    a1 = cpu->read_half(s2 + 52);
    v0 = v0 + v1;
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    a0 = cpu->read_half(s4 + 158);
    v1 = cpu->read_half(s2 + 24);
    v0 = v0 + a0;
    v1 = v1 - a1;
    cpu->write_half(s2 + 28, (uint16_t)v0);
    cpu->write_half(s4 + 154, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s2 + 56);
    /* nop */  /* 0x80180438: 0x00000000 */
    v0 = v0 - v1;
    cpu->write_half(s4 + 156, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    v1 = cpu->read_half(s2 + 60);
    a0 = cpu->read_word(s4 + 32);
    v0 = v0 - v1;
    cpu->write_half(s4 + 158, (uint16_t)v0);
    v0 = cpu->read_word(a0 + 4);
    /* nop */  /* 0x8018045C: 0x00000000 */
    cpu->write_word(a0 + 32, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180468: 0x00000000 */
    v0 = cpu->read_word(v1 + 8);
    /* nop */  /* 0x80180470: 0x00000000 */
    cpu->write_word(v1 + 36, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x8018047C: 0x00000000 */
    v0 = cpu->read_word(v1 + 12);
    /* nop */  /* 0x80180484: 0x00000000 */
    cpu->write_word(v1 + 40, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180490: 0x00000000 */
    v0 = cpu->read_word(v1 + 16);
    /* nop */  /* 0x80180498: 0x00000000 */
    cpu->write_word(v1 + 44, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x801804A4: 0x00000000 */
    v0 = cpu->read_word(v1 + 20);
    /* nop */  /* 0x801804AC: 0x00000000 */
    cpu->write_word(v1 + 48, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x801804B8: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 24);
    /* nop */  /* 0x801804C0: 0x00000000 */
    cpu->write_word(v1 + 52, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x801804CC: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 26);
    /* nop */  /* 0x801804D4: 0x00000000 */
    cpu->write_word(v1 + 56, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x801804E0: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 28);
    /* nop */  /* 0x801804E8: 0x00000000 */
    cpu->write_word(v1 + 60, v0);
    a1 = cpu->read_word(s4 + 32);
    a2 = zero;
    a0 = cpu->read_word(a1 + 68);
    ra = 0x80180504u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 32;
    psx_check_interrupts_at(cpu, 0x8018D788u);
    cpu->pc = 0x8018D788u; return;  /* CPS jal -> 0x8018D788 */

block_80180504:
    a3 = (int32_t)(int16_t)cpu->read_half(s5 + 224);
    fp = (int32_t)(int16_t)cpu->read_half(s5 + 220);
    cpu->write_word(sp + 16, a3);
    v0 = cpu->read_half(s3 + 212);
    s6 = (int32_t)(int16_t)cpu->read_half(s5 + 208);
    s1 = cpu->read_half(s5 + 222);
    cpu->write_half(s5 + 212, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 214);
    /* nop */  /* 0x80180524: 0x00000000 */
    cpu->write_half(s5 + 214, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 216);
    /* nop */  /* 0x80180530: 0x00000000 */
    cpu->write_half(s5 + 216, (uint16_t)v0);
    s2 = cpu->read_word(s7 + 32);
    v0 = cpu->read_half(s5 + 212);
    /* nop */  /* 0x80180540: 0x00000000 */
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 214);
    a0 = s6;
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 216);
    s1 = s1 << 16;
    /* delay slot (always executes) */
    cpu->write_half(s2 + 28, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801807F0u);
    goto block_801807F0;  /* j */

block_80180564:
    a3 = (int32_t)(int16_t)cpu->read_half(s3 + 224);
    s2 = cpu->read_word(s4 + 32);
    fp = (int32_t)(int16_t)cpu->read_half(s3 + 220);
    cpu->write_word(sp + 16, a3);
    v0 = cpu->read_half(s3 + 212);
    s6 = (int32_t)(int16_t)cpu->read_half(s3 + 208);
    s1 = cpu->read_half(s3 + 222);
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 214);
    /* nop */  /* 0x80180588: 0x00000000 */
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 216);
    /* nop */  /* 0x80180594: 0x00000000 */
    cpu->write_half(s2 + 28, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 212);
    v1 = cpu->read_half(s4 + 154);
    /* nop */  /* 0x801805A4: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s3 + 212, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 214);
    v1 = cpu->read_half(s4 + 156);
    a0 = s6;
    v0 = v0 + v1;
    cpu->write_half(s3 + 214, (uint16_t)v0);
    v0 = cpu->read_half(s3 + 216);
    v1 = cpu->read_half(s4 + 158);
    s1 = s1 << 16;
    v0 = v0 + v1;
    ra = 0x801805DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s3 + 216, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_801805DC:
    a0 = s6;
    ra = 0x801805E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_801805E8:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x801805F4: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    a0 = s6;
    v0 = cpu->read_half(s4 + 156);
    s1 = (int32_t)s1 >> 20;
    v0 = v0 + s1;
    cpu->write_half(s4 + 156, (uint16_t)v0);
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 - t0;
    v1 = cpu->read_half(s4 + 154);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    ra = 0x8018062Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s4 + 154, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_8018062C:
    a0 = s6;
    ra = 0x80180638u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80180638:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x80180644: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 + t0;
    v1 = cpu->read_half(s4 + 158);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    cpu->write_half(s4 + 158, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 24);
    v1 = cpu->read_half(s4 + 154);
    /* nop */  /* 0x8018066C: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s4 + 156);
    a1 = cpu->read_half(s2 + 52);
    v0 = v0 + v1;
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    a0 = cpu->read_half(s4 + 158);
    v1 = cpu->read_half(s2 + 24);
    v0 = v0 + a0;
    v1 = v1 - a1;
    cpu->write_half(s2 + 28, (uint16_t)v0);
    cpu->write_half(s4 + 154, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s2 + 56);
    /* nop */  /* 0x801806B0: 0x00000000 */
    v0 = v0 - v1;
    cpu->write_half(s4 + 156, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    v1 = cpu->read_half(s2 + 60);
    a0 = cpu->read_word(s4 + 32);
    v0 = v0 - v1;
    cpu->write_half(s4 + 158, (uint16_t)v0);
    v0 = cpu->read_word(a0 + 4);
    /* nop */  /* 0x801806D4: 0x00000000 */
    cpu->write_word(a0 + 32, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x801806E0: 0x00000000 */
    v0 = cpu->read_word(v1 + 8);
    /* nop */  /* 0x801806E8: 0x00000000 */
    cpu->write_word(v1 + 36, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x801806F4: 0x00000000 */
    v0 = cpu->read_word(v1 + 12);
    /* nop */  /* 0x801806FC: 0x00000000 */
    cpu->write_word(v1 + 40, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180708: 0x00000000 */
    v0 = cpu->read_word(v1 + 16);
    /* nop */  /* 0x80180710: 0x00000000 */
    cpu->write_word(v1 + 44, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x8018071C: 0x00000000 */
    v0 = cpu->read_word(v1 + 20);
    /* nop */  /* 0x80180724: 0x00000000 */
    cpu->write_word(v1 + 48, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180730: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 24);
    /* nop */  /* 0x80180738: 0x00000000 */
    cpu->write_word(v1 + 52, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180744: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 26);
    /* nop */  /* 0x8018074C: 0x00000000 */
    cpu->write_word(v1 + 56, v0);
    v1 = cpu->read_word(s4 + 32);
    /* nop */  /* 0x80180758: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 28);
    /* nop */  /* 0x80180760: 0x00000000 */
    cpu->write_word(v1 + 60, v0);
    a1 = cpu->read_word(s4 + 32);
    a2 = zero;
    a0 = cpu->read_word(a1 + 68);
    ra = 0x8018077Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 32;
    psx_check_interrupts_at(cpu, 0x8018D788u);
    cpu->pc = 0x8018D788u; return;  /* CPS jal -> 0x8018D788 */

block_8018077C:
    a3 = (int32_t)(int16_t)cpu->read_half(s5 + 224);
    s2 = cpu->read_word(s7 + 32);
    fp = (int32_t)(int16_t)cpu->read_half(s5 + 220);
    cpu->write_word(sp + 16, a3);
    v0 = cpu->read_half(s5 + 212);
    s6 = (int32_t)(int16_t)cpu->read_half(s5 + 208);
    s1 = cpu->read_half(s5 + 222);
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 214);
    /* nop */  /* 0x801807A0: 0x00000000 */
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 216);
    /* nop */  /* 0x801807AC: 0x00000000 */
    cpu->write_half(s2 + 28, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 212);
    v1 = cpu->read_half(s7 + 154);
    /* nop */  /* 0x801807BC: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s5 + 212, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 214);
    v1 = cpu->read_half(s7 + 156);
    a0 = s6;
    v0 = v0 + v1;
    cpu->write_half(s5 + 214, (uint16_t)v0);
    v0 = cpu->read_half(s5 + 216);
    v1 = cpu->read_half(s7 + 158);
    s1 = s1 << 16;
    v0 = v0 + v1;
    cpu->write_half(s5 + 216, (uint16_t)v0);
    /* fall through to block_801807F0 */

block_801807F0:
    ra = 0x801807F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s1 = (int32_t)s1 >> 20;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_801807F8:
    a0 = s6;
    ra = 0x80180804u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80180804:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x80180810: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    a0 = s6;
    v0 = cpu->read_half(s7 + 156);
    /* nop */  /* 0x80180820: 0x00000000 */
    v0 = v0 + s1;
    cpu->write_half(s7 + 156, (uint16_t)v0);
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 - t0;
    v1 = cpu->read_half(s7 + 154);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    ra = 0x80180848u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s7 + 154, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80180848:
    a0 = s6;
    ra = 0x80180854u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80180854:
    { int64_t result = (int64_t)(int32_t)fp * (int64_t)(int32_t)s0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(fp));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    a3 = cpu->read_word(sp + 16);
    /* nop */  /* 0x80180860: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a3 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a3));
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = v1 + t0;
    v1 = cpu->read_half(s7 + 158);
    v0 = (int32_t)v0 >> 16;
    v1 = v1 + v0;
    cpu->write_half(s7 + 158, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 24);
    v1 = cpu->read_half(s7 + 154);
    /* nop */  /* 0x80180888: 0x00000000 */
    v0 = v0 + v1;
    cpu->write_half(s2 + 24, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s7 + 156);
    a1 = cpu->read_half(s2 + 52);
    v0 = v0 + v1;
    cpu->write_half(s2 + 26, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    a0 = cpu->read_half(s7 + 158);
    v1 = cpu->read_half(s2 + 24);
    v0 = v0 + a0;
    v1 = v1 - a1;
    cpu->write_half(s2 + 28, (uint16_t)v0);
    cpu->write_half(s7 + 154, (uint16_t)v1);
    v0 = cpu->read_half(s2 + 26);
    v1 = cpu->read_half(s2 + 56);
    /* nop */  /* 0x801808CC: 0x00000000 */
    v0 = v0 - v1;
    cpu->write_half(s7 + 156, (uint16_t)v0);
    v0 = cpu->read_half(s2 + 28);
    v1 = cpu->read_half(s2 + 60);
    a0 = cpu->read_word(s7 + 32);
    v0 = v0 - v1;
    cpu->write_half(s7 + 158, (uint16_t)v0);
    v0 = cpu->read_word(a0 + 4);
    /* nop */  /* 0x801808F0: 0x00000000 */
    cpu->write_word(a0 + 32, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x801808FC: 0x00000000 */
    v0 = cpu->read_word(v1 + 8);
    /* nop */  /* 0x80180904: 0x00000000 */
    cpu->write_word(v1 + 36, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x80180910: 0x00000000 */
    v0 = cpu->read_word(v1 + 12);
    /* nop */  /* 0x80180918: 0x00000000 */
    cpu->write_word(v1 + 40, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x80180924: 0x00000000 */
    v0 = cpu->read_word(v1 + 16);
    /* nop */  /* 0x8018092C: 0x00000000 */
    cpu->write_word(v1 + 44, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x80180938: 0x00000000 */
    v0 = cpu->read_word(v1 + 20);
    /* nop */  /* 0x80180940: 0x00000000 */
    cpu->write_word(v1 + 48, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x8018094C: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 24);
    /* nop */  /* 0x80180954: 0x00000000 */
    cpu->write_word(v1 + 52, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x80180960: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 26);
    /* nop */  /* 0x80180968: 0x00000000 */
    cpu->write_word(v1 + 56, v0);
    v1 = cpu->read_word(s7 + 32);
    /* nop */  /* 0x80180974: 0x00000000 */
    v0 = (int32_t)(int16_t)cpu->read_half(v1 + 28);
    /* nop */  /* 0x8018097C: 0x00000000 */
    cpu->write_word(v1 + 60, v0);
    a1 = cpu->read_word(s7 + 32);
    a2 = zero;
    cpu->pc = 0x8018098cu; return;  /* fallthrough to split piece */
}

/* ======== 0x8018098c — hitbox_xform_epilogue ========
 * Shared tail piece of hitbox_corners_xform: final
 * node_xform_apply(*(a1+0x44), a1+0x20, 0) with a1 = *(s7/s4+0x20)
 * handed over by the predecessor pieces, then the s0-s7/fp/ra
 * epilogue (sp += 0x40) and `jr $ra`. Continuation 0x80180998.
 * Faithful to generated br2eu_func_8018098C. */
void hitbox_xform_epilogue(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80180998u: goto block_80180998;
            default: break;
        }
    }
    /* Address: 0x8018098C, Size: 60 bytes, Blocks: 2 */

block_8018098C:
    a0 = cpu->read_word(a1 + 68);
    ra = 0x80180998u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 32;
    psx_check_interrupts_at(cpu, 0x8018D788u);
    cpu->pc = 0x8018D788u; return;  /* CPS jal -> 0x8018D788 */

block_80180998:
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
    uint32_t _jt_801809C0 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 64;
    psx_check_interrupts_at(cpu, _jt_801809C0);
    cpu->pc = _jt_801809C0; return;  /* CPS: jr $ra */
}

/* ======== 0x80181000 — pair_update_attach_tail ========
 * CPS tail piece-group of fighter_pair_update (guest entry 0x801809C8,
 * game/battle.c) — the split piece the parent falls through into.
 * Resolves the attach angle via atan2_lut, the distance via
 * fixed_point_divide, stores the results through s5+0x70/0x72, then
 * calls fighter_attach_run for both fighters before the sp += 0x50
 * epilogue. Continuations 0x80181008/18/34/3C. Faithful to generated
 * br2eu_func_80181000. */
void pair_update_attach_tail(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80181008u: goto block_80181008;
            case 0x80181018u: goto block_80181018;
            case 0x80181034u: goto block_80181034;
            case 0x8018103Cu: goto block_8018103C;
            default: break;
        }
    }
    /* Address: 0x80181000, Size: 108 bytes, Blocks: 5 */

block_80181000:
    ra = 0x80181008u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero - s7;
    psx_check_interrupts_at(cpu, 0x8018D1BCu);
    cpu->pc = 0x8018D1BCu; return;  /* CPS jal -> 0x8018D1BC */

block_80181008:
    fp = v0;
    a0 = s6;
    ra = 0x80181018u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s7;
    psx_check_interrupts_at(cpu, 0x8018D300u);
    cpu->pc = 0x8018D300u; return;  /* CPS jal -> 0x8018D300 */

block_80181018:
    a0 = s4;
    s1 = fp - s1;
    s1 = s1 << 20;
    s1 = (int32_t)s1 >> 20;
    cpu->write_half(s5 + 114, (uint16_t)v0);
    ra = 0x80181034u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s5 + 112, (uint16_t)s1);
    psx_check_interrupts_at(cpu, 0x80186B2Cu);
    cpu->pc = 0x80186B2Cu; return;  /* CPS jal -> 0x80186B2C */

block_80181034:
    ra = 0x8018103Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    psx_check_interrupts_at(cpu, 0x80186B2Cu);
    cpu->pc = 0x80186B2Cu; return;  /* CPS jal -> 0x80186B2C */

block_8018103C:
    ra = cpu->read_word(sp + 76);
    fp = cpu->read_word(sp + 72);
    s7 = cpu->read_word(sp + 68);
    s6 = cpu->read_word(sp + 64);
    s5 = cpu->read_word(sp + 60);
    s4 = cpu->read_word(sp + 56);
    s3 = cpu->read_word(sp + 52);
    s2 = cpu->read_word(sp + 48);
    s1 = cpu->read_word(sp + 44);
    s0 = cpu->read_word(sp + 40);
    uint32_t _jt_80181064 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 80;
    psx_check_interrupts_at(cpu, _jt_80181064);
    cpu->pc = _jt_80181064; return;  /* CPS: jr $ra */
}

/* ======== 0x80181ee8 — ani_cat_tail_zero ========
 * Split tail of ani_fixup_guest (guest 0x80181DB8, game/action.c):
 * cat+0x28 = 0 then `jr $ra`. Faithful to br2eu_func_80181EE8. */
void ani_cat_tail_zero(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80181EE8, Size: 8 bytes, Blocks: 1 */

block_80181EE8:;
    uint32_t _jt_80181EE8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a2 + 40, (uint16_t)zero);
    psx_check_interrupts_at(cpu, _jt_80181EE8);
    cpu->pc = _jt_80181EE8; return;  /* CPS: jr $ra */
}

/* ======== 0x80181ef0 — ani_cat_tail_v2 ========
 * Split tail of ani_fixup_guest: variant==2 path — cat+0x28 = 8 and
 * cat+0x24 = *(*(cat+0xc)+0x14) + *(t1+0x30), then `jr $ra`.
 * Faithful to br2eu_func_80181EF0. */
void ani_cat_tail_v2(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80181EF0, Size: 32 bytes, Blocks: 1 */

block_80181EF0:
    v0 = cpu->read_word(a2 + 12);
    a0 = cpu->read_word(t1 + 48);
    v1 = cpu->read_word(v0 + 20);
    v0 = 8;
    cpu->write_half(a2 + 40, (uint16_t)v0);
    v1 = v1 + a0;
    uint32_t _jt_80181F08 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(a2 + 36, v1);
    psx_check_interrupts_at(cpu, _jt_80181F08);
    cpu->pc = _jt_80181F08; return;  /* CPS: jr $ra */
}

/* ======== 0x80181f10 — ani_cat_tail_nine ========
 * Split tail of ani_fixup_guest: cat+0x28 = 9 then CPS-fallthrough
 * to the plain-return piece 0x80181F18. Faithful to
 * br2eu_func_80181F10. */
void ani_cat_tail_nine(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80181F10, Size: 8 bytes, Blocks: 1 */

block_80181F10:
    v0 = 9;
    cpu->write_half(a2 + 40, (uint16_t)v0);
    cpu->pc = 0x80181f18u; return;  /* fallthrough to split piece */
}

/* ======== 0x80181f18 — ani_cat_tail_ret ========
 * Split tail of ani_fixup_guest: plain `jr $ra` (nop delay slot).
 * Faithful to br2eu_func_80181F18. */
void ani_cat_tail_ret(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80181F18, Size: 8 bytes, Blocks: 1 */

block_80181F18:;
    uint32_t _jt_80181F18 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80181F1C: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80181F18);
    cpu->pc = _jt_80181F18; return;  /* CPS: jr $ra */
}

