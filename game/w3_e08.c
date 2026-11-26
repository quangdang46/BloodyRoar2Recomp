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
static inline void br2_swl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
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
static inline void br2_swr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
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

/*  hud_time_score_draw (guest body 0x80163E38, renames.map) — HUD */
/*  time+score draw routine. This file carries its CPS piece/alias */
/*  entries (clean_cps.map 0x80163F04..0x80164638); all blocks route */
/*  here via cpu->pc. Named GPR aliases (v0/a0../sp/fp/ra) mirror the */
/*  generated transliteration 1:1. */

void hud_score_draw_alt(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80163F2Cu: goto block_80163F2C;
            case 0x80164010u: goto block_80164010;
            case 0x8016401Cu: goto block_8016401C;
            case 0x80164040u: goto block_80164040;
            case 0x80164068u: goto block_80164068;
            case 0x8016408Cu: goto block_8016408C;
            case 0x801640A0u: goto block_801640A0;
            case 0x801640D8u: goto block_801640D8;
            case 0x801640E4u: goto block_801640E4;
            case 0x80164108u: goto block_80164108;
            case 0x80164124u: goto block_80164124;
            case 0x80164130u: goto block_80164130;
            case 0x80164138u: goto block_80164138;
            case 0x80164154u: goto block_80164154;
            case 0x80164190u: goto block_80164190;
            case 0x801641BCu: goto block_801641BC;
            case 0x801641C8u: goto block_801641C8;
            case 0x801641ECu: goto block_801641EC;
            case 0x80164210u: goto block_80164210;
            case 0x8016422Cu: goto block_8016422C;
            case 0x80164268u: goto block_80164268;
            case 0x801642A0u: goto block_801642A0;
            case 0x801642ACu: goto block_801642AC;
            case 0x801642D0u: goto block_801642D0;
            case 0x801642F4u: goto block_801642F4;
            case 0x80164310u: goto block_80164310;
            case 0x8016434Cu: goto block_8016434C;
            case 0x80164384u: goto block_80164384;
            case 0x80164390u: goto block_80164390;
            case 0x801643B4u: goto block_801643B4;
            case 0x801643DCu: goto block_801643DC;
            case 0x80164450u: goto block_80164450;
            case 0x80164480u: goto block_80164480;
            case 0x801644C8u: goto block_801644C8;
            case 0x80164514u: goto block_80164514;
            case 0x80164538u: goto block_80164538;
            case 0x80164544u: goto block_80164544;
            case 0x80164564u: goto block_80164564;
            case 0x80164584u: goto block_80164584;
            case 0x801645B0u: goto block_801645B0;
            case 0x801645E0u: goto block_801645E0;
            case 0x80164604u: goto block_80164604;
            case 0x80164610u: goto block_80164610;
            case 0x80164638u: goto block_80164638;
            case 0x80163F04u: goto block_80163F04;
            case 0x80163F10u: goto block_80163F10;
            case 0x80163FC0u: goto block_80163FC0;
            case 0x80163FE0u: goto block_80163FE0;
            case 0x80164070u: goto block_80164070;
            case 0x801640A8u: goto block_801640A8;
            case 0x80164110u: goto block_80164110;
            case 0x80164140u: goto block_80164140;
            case 0x80164218u: goto block_80164218;
            case 0x801642FCu: goto block_801642FC;
            case 0x801643BCu: goto block_801643BC;
            case 0x80164400u: goto block_80164400;
            case 0x80164418u: goto block_80164418;
            case 0x80164434u: goto block_80164434;
            case 0x80164448u: goto block_80164448;
            case 0x80164458u: goto block_80164458;
            case 0x8016446Cu: goto block_8016446C;
            case 0x80164498u: goto block_80164498;
            case 0x801644E8u: goto block_801644E8;
            case 0x80164530u: goto block_80164530;
            case 0x8016456Cu: goto block_8016456C;
            case 0x8016457Cu: goto block_8016457C;
            case 0x801645F8u: goto block_801645F8;
            default: break;
        }
    }
    /* Address: 0x80163E38, Size: 2096 bytes, Blocks: 68 */

block_80163E38:
    sp = sp + -96;
    cpu->write_word(sp + 88, fp);
    fp = cpu->read_word(sp + 112);
    v0 = 0x51EB << 16;  /* 0x51EB0000 */
    cpu->write_word(sp + 92, ra);
    cpu->write_word(sp + 84, s7);
    cpu->write_word(sp + 80, s6);
    cpu->write_word(sp + 76, s5);
    cpu->write_word(sp + 72, s4);
    cpu->write_word(sp + 68, s3);
    cpu->write_word(sp + 64, s2);
    cpu->write_word(sp + 60, s1);
    cpu->write_word(sp + 56, s0);
    t0 = cpu->read_word(fp + 8);
    v0 = v0 | 0x851F;
    { int64_t result = (int64_t)(int32_t)t0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(t0));
    s4 = a0;
    a0 = 0x8888 << 16;  /* 0x88880000 */
    a0 = a0 | 0x8889;
    v0 = (int32_t)t0 >> 31;
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    v1 = (int32_t)t1 >> 4;
    s0 = v1 - v0;
    v1 = s0;
    v0 = v1 << 1;
    v0 = v0 + v1;
    v0 = v0 << 3;
    v0 = v0 + v1;
    v0 = v0 << 1;
    s0 = t0 - v0;
    t0 = v1;
    { int64_t result = (int64_t)(int32_t)t0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(t0));
    s5 = a1;
    s6 = a2;
    s7 = a3;
    s3 = cpu->read_word(sp + 116);
    v1 = (int32_t)t0 >> 31;
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);
    v0 = t1 + t0;
    v0 = (int32_t)v0 >> 5;
    s1 = v0 - v1;
    v1 = s1;
    v0 = v1 << 4;
    v0 = v0 - v1;
    v0 = v0 << 2;
    s1 = t0 - v0;
    t0 = v1;
    s2 = t0;
    v0 = ((int32_t)s2 < 100) ? 1 : 0;  /* 0x80163EF8: 0x2A420064 */
    int _bc_80163EFC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = s0 << 1;
    if (_bc_80163EFC) {
        psx_check_interrupts_at(cpu, 0x80163F10u);
        goto block_80163F10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80163F04u);
        goto block_80163F04;  /* not taken */
    }

block_80163F04:
    s0 = 99;
    s2 = s0;
    s1 = 59;
    /* fall through to block_80163F10 */

block_80163F10:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8104;
    a2 = s2;
    a3 = s1;
    ra = 0x80163F2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, s0);
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80163F2C:
    v1 = 0x1062 << 16;  /* 0x10620000 */
    v0 = cpu->read_half(fp + 12);
    v1 = v1 | 0x4DD3;
    v0 = v0 << 16;
    a0 = (int32_t)v0 >> 16;
    { int64_t result = (int64_t)(int32_t)a0 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a0));
    a1 = cpu->read_half(fp + 14);
    /* nop */  /* 0x80163F48: 0x00000000 */
    a1 = a1 << 16;
    t0 = cpu->hi;
    psx_muldiv_stall(cpu);
    a2 = (int32_t)a1 >> 16;
    v0 = (int32_t)v0 >> 31;
    { int64_t result = (int64_t)(int32_t)a2 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a2));
    a1 = (int32_t)a1 >> 31;
    v1 = (int32_t)t0 >> 6;
    v1 = v1 - v0;
    v0 = v1 << 5;
    v0 = v0 - v1;
    v0 = v0 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a0 = a0 - v0;
    a0 = a0 << 16;
    t0 = (int32_t)a0 >> 16;
    a3 = cpu->hi;
    psx_muldiv_stall(cpu);
    v1 = (int32_t)a3 >> 6;
    v1 = v1 - a1;
    v0 = v1 << 5;
    v0 = v0 - v1;
    v0 = v0 << 2;
    v0 = v0 + v1;
    v0 = v0 << 3;
    a2 = a2 - v0;
    a2 = a2 << 16;
    v0 = (s3 < (uint32_t)23) ? 1 : 0;  /* 0x80163FB4: 0x2E620017 */
    int _bc_80163FB8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)a2 >> 16;
    if (_bc_80163FB8) {
        psx_check_interrupts_at(cpu, 0x80164638u);
        goto block_80164638;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80163FC0u);
        goto block_80163FC0;  /* not taken */
    }

block_80163FC0:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 8172;
    v1 = s3 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80163FD4: 0x00000000 */
    uint32_t _jt_80163FD8 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80163FDC: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80163FD8);
    cpu->pc = _jt_80163FD8; return;  /* CPS: jr v0 */

block_80163FE0:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 16;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s0 = s7 << 16;
    s0 = (int32_t)s0 >> 16;
    s1 = 1;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164010u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80164010:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x8016401Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_8016401C:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 160;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164040u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80164040:
    a0 = s4;
    a1 = s5;
    a2 = fp;
    a3 = s6 + 304;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    v0 = 4;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164068u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80164068:
    /* delay slot (always executes) */
    /* nop */  /* 0x8016406C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164638u);
    goto block_80164638;  /* j */

block_80164070:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8120;
    a2 = s2;
    a3 = s1;
    ra = 0x8016408Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, s0);
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_8016408C:
    a0 = 0x8012 << 16;  /* 0x80120000 */
    a0 = a0 + 8136;
    a1 = fp;
    ra = 0x801640A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = sp + 24;
    psx_check_interrupts_at(cpu, 0x8012CC6Cu);
    cpu->pc = 0x8012CC6Cu; return;  /* CPS jal -> 0x8012CC6C */

block_801640A0:
    /* delay slot (always executes) */
    /* nop */  /* 0x801640A4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164124u);
    goto block_80164124;  /* j */

block_801640A8:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 16;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s0 = s7 << 16;
    s0 = (int32_t)s0 >> 16;
    s1 = 1;
    cpu->write_word(sp + 16, s0);
    ra = 0x801640D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_801640D8:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x801640E4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_801640E4:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 160;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164108u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80164108:
    /* delay slot (always executes) */
    /* nop */  /* 0x8016410C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164638u);
    goto block_80164638;  /* j */

block_80164110:
    a0 = 0x8012 << 16;  /* 0x80120000 */
    a0 = a0 + 8148;
    a1 = fp;
    ra = 0x80164124u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = t0;
    psx_check_interrupts_at(cpu, 0x8012CC6Cu);
    cpu->pc = 0x8012CC6Cu; return;  /* CPS jal -> 0x8012CC6C */

block_80164124:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x80164130u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_80164130:
    ra = 0x80164138u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x8012CC6Cu);
    cpu->pc = 0x8012CC6Cu; return;  /* CPS jal -> 0x8012CC6C */

block_80164138:
    /* delay slot (always executes) */
    /* nop */  /* 0x8016413C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164638u);
    goto block_80164638;  /* j */

block_80164140:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8160;
    ra = 0x80164154u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = t0;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80164154:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = 3;
    a3 = a3 - v0;
    a3 = a3 << 4;
    a3 = s6 + a3;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s0 = s7 << 16;
    s0 = (int32_t)s0 >> 16;
    s2 = 4;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164190u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s2);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80164190:
    a0 = s4;
    a1 = s5;
    a2 = 0x8012 << 16;  /* 0x80120000 */
    a2 = a2 + 8164;
    a3 = s6 + 48;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s1 = 1;
    cpu->write_word(sp + 16, s0);
    ra = 0x801641BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_801641BC:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x801641C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_801641C8:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 160;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    cpu->write_word(sp + 16, s0);
    ra = 0x801641ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_801641EC:
    a0 = s4;
    a1 = s5;
    a2 = fp;
    a3 = s6 + 304;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164210u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s2);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80164210:
    /* delay slot (always executes) */
    /* nop */  /* 0x80164214: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164638u);
    goto block_80164638;  /* j */

block_80164218:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8160;
    ra = 0x8016422Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a3;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_8016422C:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = 3;
    a3 = a3 - v0;
    a3 = a3 << 4;
    a3 = s6 + a3;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s1 = s7 << 16;
    s1 = (int32_t)s1 >> 16;
    s3 = 4;
    cpu->write_word(sp + 16, s1);
    ra = 0x80164268u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s3);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_80164268:
    a0 = s4;
    a1 = s5;
    a2 = 0x8012 << 16;  /* 0x80120000 */
    a2 = a2 + 8164;
    a3 = s6 + 48;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s0 = s7 + 10;
    s0 = s0 << 16;
    s0 = (int32_t)s0 >> 16;
    s2 = 1;
    cpu->write_word(sp + 16, s0);
    ra = 0x801642A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s2);
    psx_check_interrupts_at(cpu, 0x8012E184u);
    cpu->pc = 0x8012E184u; return;  /* CPS jal -> 0x8012E184 */

block_801642A0:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x801642ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_801642AC:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 116;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    cpu->write_word(sp + 16, s0);
    ra = 0x801642D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s2);
    psx_check_interrupts_at(cpu, 0x8012E184u);
    cpu->pc = 0x8012E184u; return;  /* CPS jal -> 0x8012E184 */

block_801642D0:
    a0 = s4;
    a1 = s5;
    a2 = fp;
    a3 = s6 + 180;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    cpu->write_word(sp + 16, s1);
    ra = 0x801642F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s3);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_801642F4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801642F8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164638u);
    goto block_80164638;  /* j */

block_801642FC:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8160;
    ra = 0x80164310u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a3;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80164310:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = 3;
    a3 = a3 - v0;
    a3 = a3 << 4;
    a3 = s6 + a3;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    v0 = s7 << 16;
    v0 = (int32_t)v0 >> 16;
    cpu->write_word(sp + 16, v0);
    v0 = 4;
    ra = 0x8016434Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);
    psx_check_interrupts_at(cpu, 0x8012E334u);
    cpu->pc = 0x8012E334u; return;  /* CPS jal -> 0x8012E334 */

block_8016434C:
    a0 = s4;
    a1 = s5;
    a2 = 0x8012 << 16;  /* 0x80120000 */
    a2 = a2 + 8164;
    a3 = s6 + 48;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s0 = s7 + 10;
    s0 = s0 << 16;
    s0 = (int32_t)s0 >> 16;
    s1 = 1;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164384u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E184u);
    cpu->pc = 0x8012E184u; return;  /* CPS jal -> 0x8012E184 */

block_80164384:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x80164390u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_80164390:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 116;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    cpu->write_word(sp + 16, s0);
    ra = 0x801643B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E184u);
    cpu->pc = 0x8012E184u; return;  /* CPS jal -> 0x8012E184 */

block_801643B4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801643B8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164638u);
    goto block_80164638;  /* j */

block_801643BC:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 16;
    v0 = 5;
    cpu->write_word(sp + 16, s7);
    ra = 0x801643DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);
    psx_check_interrupts_at(cpu, 0x8012E61Cu);
    cpu->pc = 0x8012E61Cu; return;  /* CPS jal -> 0x8012E61C */

block_801643DC:
    a0 = s4;
    a1 = s5;
    a2 = fp;
    a3 = s6 + 168;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s0 = s7 << 16;
    /* delay slot (always executes) */
    s0 = (int32_t)s0 >> 16;
    psx_check_interrupts_at(cpu, 0x801645F8u);
    goto block_801645F8;  /* j */

block_80164400:
    v0 = 0x1F80 << 16;  /* 0x1F800000 */
    v0 = cpu->read_word(v0 + 220);
    /* nop */  /* 0x80164408: 0x00000000 */
    v0 = v0 & 0x0004;  /* 0x8016440C: 0x30420004 */
    int _bc_80164410 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s4;
    if (_bc_80164410) {
        psx_check_interrupts_at(cpu, 0x80164434u);
        goto block_80164434;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164418u);
        goto block_80164418;  /* not taken */
    }

block_80164418:
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 16;
    v0 = 5;
    cpu->write_word(sp + 16, s7);
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);
    psx_check_interrupts_at(cpu, 0x80164448u);
    goto block_80164448;  /* j */

block_80164434:
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 16;
    cpu->write_word(sp + 16, s7);
    cpu->write_word(sp + 20, zero);
    /* fall through to block_80164448 */

block_80164448:
    ra = 0x80164450u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016444C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012E73Cu);
    cpu->pc = 0x8012E73Cu; return;  /* CPS jal -> 0x8012E73C */

block_80164450:
    /* delay slot (always executes) */
    /* nop */  /* 0x80164454: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164538u);
    goto block_80164538;  /* j */

block_80164458:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8160;
    /* delay slot (always executes) */
    a2 = a3;
    psx_check_interrupts_at(cpu, 0x8016457Cu);
    goto block_8016457C;  /* j */

block_8016446C:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8160;
    ra = 0x80164480u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a3;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80164480:
    v1 = 0x1F80 << 16;  /* 0x1F800000 */
    v1 = cpu->read_word(v1 + 220);
    /* nop */  /* 0x80164488: 0x00000000 */
    v1 = v1 & 0x0004;  /* 0x8016448C: 0x30630004 */
    int _bc_80164490 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = v0;
    if (_bc_80164490) {
        psx_check_interrupts_at(cpu, 0x801644E8u);
        goto block_801644E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164498u);
        goto block_80164498;  /* not taken */
    }

block_80164498:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = 3;
    a3 = a3 - t0;
    a3 = a3 << 4;
    a3 = s6 + a3;
    a3 = a3 + 16;
    s0 = 5;
    cpu->write_word(sp + 16, s7);
    ra = 0x801644C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);
    psx_check_interrupts_at(cpu, 0x8012E73Cu);
    cpu->pc = 0x8012E73Cu; return;  /* CPS jal -> 0x8012E73C */

block_801644C8:
    a0 = s4;
    a1 = s5;
    a2 = 0x8012 << 16;  /* 0x80120000 */
    a2 = a2 + 8164;
    a3 = s6 + 64;
    cpu->write_word(sp + 16, s7);
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);
    psx_check_interrupts_at(cpu, 0x80164530u);
    goto block_80164530;  /* j */

block_801644E8:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = 3;
    a3 = a3 - t0;
    a3 = a3 << 4;
    a3 = s6 + a3;
    a3 = a3 + 16;
    cpu->write_word(sp + 16, s7);
    ra = 0x80164514u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);
    psx_check_interrupts_at(cpu, 0x8012E73Cu);
    cpu->pc = 0x8012E73Cu; return;  /* CPS jal -> 0x8012E73C */

block_80164514:
    a0 = s4;
    a1 = s5;
    a2 = 0x8012 << 16;  /* 0x80120000 */
    a2 = a2 + 8164;
    a3 = s6 + 64;
    cpu->write_word(sp + 16, s7);
    cpu->write_word(sp + 20, zero);
    /* fall through to block_80164530 */

block_80164530:
    ra = 0x80164538u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80164534: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012E6A8u);
    cpu->pc = 0x8012E6A8u; return;  /* CPS jal -> 0x8012E6A8 */

block_80164538:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x80164544u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_80164544:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 248;
    v0 = 2;
    cpu->write_word(sp + 16, s7);
    ra = 0x80164564u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);
    psx_check_interrupts_at(cpu, 0x8012E6A8u);
    cpu->pc = 0x8012E6A8u; return;  /* CPS jal -> 0x8012E6A8 */

block_80164564:
    /* delay slot (always executes) */
    /* nop */  /* 0x80164568: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80164638u);
    goto block_80164638;  /* j */

block_8016456C:
    a0 = sp + 24;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8160;
    a2 = t0;
    /* fall through to block_8016457C */

block_8016457C:
    ra = 0x80164584u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s1 = 5;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80164584:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = 3;
    a3 = a3 - v0;
    a3 = a3 << 4;
    a3 = s6 + a3;
    a3 = a3 + 16;
    cpu->write_word(sp + 16, s7);
    ra = 0x801645B0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E61Cu);
    cpu->pc = 0x8012E61Cu; return;  /* CPS jal -> 0x8012E61C */

block_801645B0:
    a0 = s4;
    a1 = s5;
    a2 = 0x8012 << 16;  /* 0x80120000 */
    a2 = a2 + 8164;
    a3 = s6 + 64;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    s0 = s7 << 16;
    s0 = (int32_t)s0 >> 16;
    cpu->write_word(sp + 16, s0);
    ra = 0x801645E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_801645E0:
    a0 = s4;
    a1 = s5;
    a2 = fp;
    a3 = s6 + 168;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    /* fall through to block_801645F8 */

block_801645F8:
    cpu->write_word(sp + 16, s0);
    ra = 0x80164604u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_80164604:
    a1 = cpu->read_byte(fp + 4);
    ra = 0x80164610u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801513FCu);
    cpu->pc = 0x801513FCu; return;  /* CPS jal -> 0x801513FC */

block_80164610:
    a0 = s4;
    a1 = s5;
    a2 = sp + 24;
    a3 = s6 + 248;
    a3 = a3 << 16;
    a3 = (int32_t)a3 >> 16;
    v0 = 2;
    cpu->write_word(sp + 16, s0);
    ra = 0x80164638u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_80164638:
    ra = cpu->read_word(sp + 92);
    fp = cpu->read_word(sp + 88);
    s7 = cpu->read_word(sp + 84);
    s6 = cpu->read_word(sp + 80);
    s5 = cpu->read_word(sp + 76);
    s4 = cpu->read_word(sp + 72);
    s3 = cpu->read_word(sp + 68);
    s2 = cpu->read_word(sp + 64);
    s1 = cpu->read_word(sp + 60);
    s0 = cpu->read_word(sp + 56);
    uint32_t _jt_80164660 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 96;
    psx_check_interrupts_at(cpu, _jt_80164660);
    cpu->pc = _jt_80164660; return;  /* CPS: jr $ra */
}


/*  player_name_entry (guest body 0x80164EA4, renames.map) — player name */
/*  entry UI coroutine. CPS piece/alias entries 0x80164F7C+ all share */
/*  this body; 152 basic blocks, continuation dispatch via cpu->pc. */

void player_name_entry_alt(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80165054u: goto block_80165054;
            case 0x80165064u: goto block_80165064;
            case 0x80165074u: goto block_80165074;
            case 0x80165084u: goto block_80165084;
            case 0x801650ACu: goto block_801650AC;
            case 0x801651C0u: goto block_801651C0;
            case 0x801651DCu: goto block_801651DC;
            case 0x80165240u: goto block_80165240;
            case 0x80165290u: goto block_80165290;
            case 0x801652BCu: goto block_801652BC;
            case 0x801652D0u: goto block_801652D0;
            case 0x80165300u: goto block_80165300;
            case 0x80165324u: goto block_80165324;
            case 0x801653FCu: goto block_801653FC;
            case 0x8016541Cu: goto block_8016541C;
            case 0x80165430u: goto block_80165430;
            case 0x80165444u: goto block_80165444;
            case 0x801654A8u: goto block_801654A8;
            case 0x801654CCu: goto block_801654CC;
            case 0x801654F0u: goto block_801654F0;
            case 0x8016550Cu: goto block_8016550C;
            case 0x80165530u: goto block_80165530;
            case 0x80165550u: goto block_80165550;
            case 0x80165580u: goto block_80165580;
            case 0x801655F0u: goto block_801655F0;
            case 0x80165660u: goto block_80165660;
            case 0x8016568Cu: goto block_8016568C;
            case 0x801656ACu: goto block_801656AC;
            case 0x801656DCu: goto block_801656DC;
            case 0x801656ECu: goto block_801656EC;
            case 0x80165708u: goto block_80165708;
            case 0x8016572Cu: goto block_8016572C;
            case 0x80165740u: goto block_80165740;
            case 0x80165750u: goto block_80165750;
            case 0x8016578Cu: goto block_8016578C;
            case 0x8016579Cu: goto block_8016579C;
            case 0x801657ACu: goto block_801657AC;
            case 0x801657BCu: goto block_801657BC;
            case 0x80165828u: goto block_80165828;
            case 0x80165838u: goto block_80165838;
            case 0x80165844u: goto block_80165844;
            case 0x80165854u: goto block_80165854;
            case 0x80165864u: goto block_80165864;
            case 0x8016586Cu: goto block_8016586C;
            case 0x80165878u: goto block_80165878;
            case 0x80165888u: goto block_80165888;
            case 0x801658A4u: goto block_801658A4;
            case 0x801658BCu: goto block_801658BC;
            case 0x80164F7Cu: goto block_80164F7C;
            case 0x80164F88u: goto block_80164F88;
            case 0x80164F90u: goto block_80164F90;
            case 0x80164F98u: goto block_80164F98;
            case 0x80164FA0u: goto block_80164FA0;
            case 0x80164FA8u: goto block_80164FA8;
            case 0x80164FB0u: goto block_80164FB0;
            case 0x80164FB4u: goto block_80164FB4;
            case 0x80164FCCu: goto block_80164FCC;
            case 0x80164FDCu: goto block_80164FDC;
            case 0x80164FECu: goto block_80164FEC;
            case 0x80164FF4u: goto block_80164FF4;
            case 0x80164FFCu: goto block_80164FFC;
            case 0x80165004u: goto block_80165004;
            case 0x8016500Cu: goto block_8016500C;
            case 0x80165014u: goto block_80165014;
            case 0x80165020u: goto block_80165020;
            case 0x80165028u: goto block_80165028;
            case 0x80165030u: goto block_80165030;
            case 0x80165038u: goto block_80165038;
            case 0x80165044u: goto block_80165044;
            case 0x8016504Cu: goto block_8016504C;
            case 0x8016505Cu: goto block_8016505C;
            case 0x8016506Cu: goto block_8016506C;
            case 0x80165090u: goto block_80165090;
            case 0x801650B4u: goto block_801650B4;
            case 0x801650FCu: goto block_801650FC;
            case 0x80165110u: goto block_80165110;
            case 0x8016511Cu: goto block_8016511C;
            case 0x80165130u: goto block_80165130;
            case 0x8016513Cu: goto block_8016513C;
            case 0x80165154u: goto block_80165154;
            case 0x80165174u: goto block_80165174;
            case 0x80165194u: goto block_80165194;
            case 0x801651A4u: goto block_801651A4;
            case 0x801651ACu: goto block_801651AC;
            case 0x801651C8u: goto block_801651C8;
            case 0x801651E4u: goto block_801651E4;
            case 0x801651E8u: goto block_801651E8;
            case 0x801651ECu: goto block_801651EC;
            case 0x80165204u: goto block_80165204;
            case 0x8016520Cu: goto block_8016520C;
            case 0x8016521Cu: goto block_8016521C;
            case 0x80165234u: goto block_80165234;
            case 0x80165238u: goto block_80165238;
            case 0x80165258u: goto block_80165258;
            case 0x80165278u: goto block_80165278;
            case 0x801652A4u: goto block_801652A4;
            case 0x801652B0u: goto block_801652B0;
            case 0x801652F4u: goto block_801652F4;
            case 0x80165304u: goto block_80165304;
            case 0x80165318u: goto block_80165318;
            case 0x80165328u: goto block_80165328;
            case 0x80165370u: goto block_80165370;
            case 0x8016537Cu: goto block_8016537C;
            case 0x80165384u: goto block_80165384;
            case 0x8016538Cu: goto block_8016538C;
            case 0x80165390u: goto block_80165390;
            case 0x801653CCu: goto block_801653CC;
            case 0x801653DCu: goto block_801653DC;
            case 0x801653E4u: goto block_801653E4;
            case 0x801653ECu: goto block_801653EC;
            case 0x80165404u: goto block_80165404;
            case 0x80165410u: goto block_80165410;
            case 0x801654A0u: goto block_801654A0;
            case 0x801654B8u: goto block_801654B8;
            case 0x801654BCu: goto block_801654BC;
            case 0x801654C0u: goto block_801654C0;
            case 0x801654DCu: goto block_801654DC;
            case 0x80165500u: goto block_80165500;
            case 0x80165560u: goto block_80165560;
            case 0x80165564u: goto block_80165564;
            case 0x80165590u: goto block_80165590;
            case 0x80165598u: goto block_80165598;
            case 0x8016559Cu: goto block_8016559C;
            case 0x801655A4u: goto block_801655A4;
            case 0x801655ACu: goto block_801655AC;
            case 0x801655C4u: goto block_801655C4;
            case 0x80165600u: goto block_80165600;
            case 0x80165608u: goto block_80165608;
            case 0x80165620u: goto block_80165620;
            case 0x8016563Cu: goto block_8016563C;
            case 0x80165658u: goto block_80165658;
            case 0x80165664u: goto block_80165664;
            case 0x8016566Cu: goto block_8016566C;
            case 0x801656BCu: goto block_801656BC;
            case 0x801656C0u: goto block_801656C0;
            case 0x801656E4u: goto block_801656E4;
            case 0x801656FCu: goto block_801656FC;
            case 0x80165758u: goto block_80165758;
            case 0x80165768u: goto block_80165768;
            case 0x80165780u: goto block_80165780;
            case 0x80165784u: goto block_80165784;
            case 0x801657D0u: goto block_801657D0;
            case 0x801657E8u: goto block_801657E8;
            case 0x801657ECu: goto block_801657EC;
            case 0x80165800u: goto block_80165800;
            case 0x8016580Cu: goto block_8016580C;
            case 0x80165830u: goto block_80165830;
            case 0x8016589Cu: goto block_8016589C;
            case 0x801658ACu: goto block_801658AC;
            case 0x801658B4u: goto block_801658B4;
            case 0x801658D0u: goto block_801658D0;
            default: break;
        }
    }
    /* Address: 0x80164EA4, Size: 2660 bytes, Blocks: 152 */

block_80164EA4:
    sp = sp + -144;
    t0 = 0x1F80 << 16;  /* 0x1F800000 */
    t0 = t0 | 0x00E8;
    t1 = 0x1F80 << 16;  /* 0x1F800000 */
    t1 = t1 | 0x01F0;
    t2 = 0x1F80 << 16;  /* 0x1F800000 */
    t2 = t2 | 0x00E8;
    cpu->write_word(sp + 120, s4);
    s4 = zero;
    cpu->write_word(sp + 136, fp);
    v1 = 0x1F80 << 16;  /* 0x1F800000 */
    v1 = cpu->read_byte(v1 + 156);
    v0 = 0x8012 << 16;  /* 0x80120000 */
    cpu->write_word(sp + 140, ra);
    cpu->write_word(sp + 132, s7);
    cpu->write_word(sp + 128, s6);
    cpu->write_word(sp + 124, s5);
    cpu->write_word(sp + 116, s3);
    cpu->write_word(sp + 112, s2);
    cpu->write_word(sp + 108, s1);
    cpu->write_word(sp + 104, s0);
    cpu->write_word(sp + 68, t0);
    cpu->write_word(sp + 72, t1);
    cpu->write_word(sp + 76, t2);
    cpu->write_word(sp + 84, zero);
    t2 = v0 + 7984;
    t3 = br2_lwl(cpu, t2 + 3, t3);
    t3 = br2_lwr(cpu, t2, t3);
    /* nop */  /* 0x80164F14: 0x00000000 */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    t2 = v0 + 8308;
    t3 = (int32_t)(int8_t)cpu->read_byte(t2);
    t0 = (int32_t)(int8_t)cpu->read_byte(t2 + 1);
    t1 = (int32_t)(int8_t)cpu->read_byte(t2 + 2);
    cpu->write_byte(sp + 32, (uint8_t)t3);
    cpu->write_byte(sp + 33, (uint8_t)t0);
    cpu->write_byte(sp + 34, (uint8_t)t1);
    v0 = 0x8012 << 16;  /* 0x80120000 */
    t2 = v0 + 8312;
    t3 = (int32_t)(int8_t)cpu->read_byte(t2);
    t0 = (int32_t)(int8_t)cpu->read_byte(t2 + 1);
    cpu->write_byte(sp + 40, (uint8_t)t3);
    cpu->write_byte(sp + 41, (uint8_t)t0);
    v0 = 0x8012 << 16;  /* 0x80120000 */
    t2 = v0 + 8316;
    t3 = (int32_t)(int8_t)cpu->read_byte(t2);
    t0 = (int32_t)(int8_t)cpu->read_byte(t2 + 1);
    cpu->write_byte(sp + 48, (uint8_t)t3);
    cpu->write_byte(sp + 49, (uint8_t)t0);
    v0 = 3;
    int _bc_80164F74 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    fp = s4;
    if (_bc_80164F74) {
        psx_check_interrupts_at(cpu, 0x80164FA8u);
        goto block_80164FA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164F7Cu);
        goto block_80164F7C;  /* not taken */
    }

block_80164F7C:
    v0 = ((int32_t)v1 < 4) ? 1 : 0;  /* 0x80164F7C: 0x28620004 */
    int _bc_80164F80 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (_bc_80164F80) {
        psx_check_interrupts_at(cpu, 0x80164F98u);
        goto block_80164F98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164F88u);
        goto block_80164F88;  /* not taken */
    }

block_80164F88:
    int _bc_80164F88 = ((int32_t)v1 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801C << 16;  /* 0x801C0000 */
    if (_bc_80164F88) {
        psx_check_interrupts_at(cpu, 0x80164FB4u);
        goto block_80164FB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164F90u);
        goto block_80164F90;  /* not taken */
    }

block_80164F90:
    /* delay slot (always executes) */
    v0 = v0 + 4708;
    psx_check_interrupts_at(cpu, 0x80164FB0u);
    goto block_80164FB0;  /* j */

block_80164F98:
    int _bc_80164F98 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801C << 16;  /* 0x801C0000 */
    if (_bc_80164F98) {
        psx_check_interrupts_at(cpu, 0x80164FB4u);
        goto block_80164FB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164FA0u);
        goto block_80164FA0;  /* not taken */
    }

block_80164FA0:
    /* delay slot (always executes) */
    v0 = v0 + 4708;
    psx_check_interrupts_at(cpu, 0x80164FB0u);
    goto block_80164FB0;  /* j */

block_80164FA8:
    v0 = 0x801C << 16;  /* 0x801C0000 */
    v0 = v0 + 5028;
    /* fall through to block_80164FB0 */

block_80164FB0:
    cpu->write_word(sp + 88, v0);
    /* fall through to block_80164FB4 */

block_80164FB4:
    v0 = 0x1F80 << 16;  /* 0x1F800000 */
    v1 = cpu->read_byte(v0 + 1);
    cpu->write_half(v0 + 140, (uint16_t)zero);
    v0 = 255;
    int _bc_80164FC4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s2 = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_80164FC4) {
        psx_check_interrupts_at(cpu, 0x801658D0u);
        goto block_801658D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164FCCu);
        goto block_80164FCC;  /* not taken */
    }

block_80164FCC:
    t3 = sp + 24;
    t0 = sp + 56;
    cpu->write_word(sp + 96, t3);
    cpu->write_word(sp + 92, t0);
    /* fall through to block_80164FDC */

block_80164FDC:
    v1 = cpu->read_byte(s2 + 1);
    t1 = 3;
    int _bc_80164FE4 = (v1 == t1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 4) ? 1 : 0;  /* 0x80164FE8: 0x28620004 */
    if (_bc_80164FE4) {
        psx_check_interrupts_at(cpu, 0x801652BCu);
        goto block_801652BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164FECu);
        goto block_80164FEC;  /* not taken */
    }

block_80164FEC:
    int _bc_80164FEC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t2 = 1;
    if (_bc_80164FEC) {
        psx_check_interrupts_at(cpu, 0x80165014u);
        goto block_80165014;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164FF4u);
        goto block_80164FF4;  /* not taken */
    }

block_80164FF4:
    int _bc_80164FF4 = (v1 == t2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 2) ? 1 : 0;  /* 0x80164FF8: 0x28620002 */
    if (_bc_80164FF4) {
        psx_check_interrupts_at(cpu, 0x80165090u);
        goto block_80165090;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80164FFCu);
        goto block_80164FFC;  /* not taken */
    }

block_80164FFC:
    int _bc_80164FFC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165000: 0x00000000 */
    if (_bc_80164FFC) {
        psx_check_interrupts_at(cpu, 0x801650FCu);
        goto block_801650FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165004u);
        goto block_80165004;  /* not taken */
    }

block_80165004:
    int _bc_80165004 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165008: 0x00000000 */
    if (_bc_80165004) {
        psx_check_interrupts_at(cpu, 0x8016504Cu);
        goto block_8016504C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016500Cu);
        goto block_8016500C;  /* not taken */
    }

block_8016500C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80165010: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801658B4u);
    goto block_801658B4;  /* j */

block_80165014:
    v0 = 240;
    int _bc_80165018 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 241) ? 1 : 0;  /* 0x8016501C: 0x286200F1 */
    if (_bc_80165018) {
        psx_check_interrupts_at(cpu, 0x80165830u);
        goto block_80165830;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165020u);
        goto block_80165020;  /* not taken */
    }

block_80165020:
    int _bc_80165020 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;
    if (_bc_80165020) {
        psx_check_interrupts_at(cpu, 0x80165038u);
        goto block_80165038;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165028u);
        goto block_80165028;  /* not taken */
    }

block_80165028:
    int _bc_80165028 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016502C: 0x00000000 */
    if (_bc_80165028) {
        psx_check_interrupts_at(cpu, 0x80165758u);
        goto block_80165758;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165030u);
        goto block_80165030;  /* not taken */
    }

block_80165030:
    /* delay slot (always executes) */
    /* nop */  /* 0x80165034: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801658B4u);
    goto block_801658B4;  /* j */

block_80165038:
    v0 = 241;
    int _bc_8016503C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165040: 0x00000000 */
    if (_bc_8016503C) {
        psx_check_interrupts_at(cpu, 0x8016589Cu);
        goto block_8016589C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165044u);
        goto block_80165044;  /* not taken */
    }

block_80165044:
    /* delay slot (always executes) */
    /* nop */  /* 0x80165048: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801658B4u);
    goto block_801658B4;  /* j */

block_8016504C:
    ra = 0x80165054u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165050: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_80165054:
    int _bc_80165054 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165058: 0x00000000 */
    if (_bc_80165054) {
        psx_check_interrupts_at(cpu, 0x801658B4u);
        goto block_801658B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016505Cu);
        goto block_8016505C;  /* not taken */
    }

block_8016505C:
    ra = 0x80165064u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165060: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012C0B4u);
    cpu->pc = 0x8012C0B4u; return;  /* CPS jal -> 0x8012C0B4 */

block_80165064:
    int _bc_80165064 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165068: 0x00000000 */
    if (_bc_80165064) {
        psx_check_interrupts_at(cpu, 0x801658B4u);
        goto block_801658B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016506Cu);
        goto block_8016506C;  /* not taken */
    }

block_8016506C:
    ra = 0x80165074u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x8012D090u);
    cpu->pc = 0x8012D090u; return;  /* CPS jal -> 0x8012D090 */

block_80165074:
    a0 = 2;
    a1 = zero;
    ra = 0x80165084u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x8012CBC0u);
    cpu->pc = 0x8012CBC0u; return;  /* CPS jal -> 0x8012CBC0 */

block_80165084:
    cpu->write_half(s2 + 140, (uint16_t)zero);
    t3 = 1;
    cpu->write_byte(s2 + 1, (uint8_t)t3);
    /* fall through to block_80165090 */

block_80165090:
    t0 = 1;
    cpu->write_word(sp + 16, t0);
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    a1 = 141;
    a2 = 18;
    ra = 0x801650ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 2;
    psx_check_interrupts_at(cpu, 0x8015ABC0u);
    cpu->pc = 0x8015ABC0u; return;  /* CPS jal -> 0x8015ABC0 */

block_801650AC:
    int _bc_801650AC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_801650AC) {
        psx_check_interrupts_at(cpu, 0x801658B4u);
        goto block_801658B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801650B4u);
        goto block_801650B4;  /* not taken */
    }

block_801650B4:
    a0 = 0x801F << 16;  /* 0x801F0000 */
    cpu->write_word(v0 + -31480, zero);
    v0 = 48;
    cpu->write_word(a0 + -31464, v0);
    v0 = 2;
    t1 = 0x801F << 16;  /* 0x801F0000 */
    v1 = cpu->read_half(s2 + 32);
    t1 = t1 + -31480;
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    cpu->write_word(t1 + 4, v1);
    v0 = cpu->read_half(s2 + 34);
    a0 = a0 + -31464;
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 16;
    v0 = v0 + -60;
    cpu->write_word(a0 + 4, v0);
    /* fall through to block_801650FC */

block_801650FC:
    v0 = cpu->read_half(s2 + 180);
    v1 = 2560;
    v0 = v0 & 0x0F00;  /* 0x80165104: 0x30420F00 */
    int _bc_80165108 = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016510C: 0x00000000 */
    if (_bc_80165108) {
        psx_check_interrupts_at(cpu, 0x8016511Cu);
        goto block_8016511C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165110u);
        goto block_80165110;  /* not taken */
    }

block_80165110:
    t2 = cpu->read_word(sp + 68);
    /* nop */  /* 0x80165114: 0x00000000 */
    cpu->write_word(sp + 76, t2);
    /* fall through to block_8016511C */

block_8016511C:
    v0 = cpu->read_half(s2 + 182);
    /* nop */  /* 0x80165120: 0x00000000 */
    v0 = v0 & 0x0F00;  /* 0x80165124: 0x30420F00 */
    int _bc_80165128 = (v0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016512C: 0x00000000 */
    if (_bc_80165128) {
        psx_check_interrupts_at(cpu, 0x8016513Cu);
        goto block_8016513C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165130u);
        goto block_80165130;  /* not taken */
    }

block_80165130:
    t3 = cpu->read_word(sp + 72);
    /* nop */  /* 0x80165134: 0x00000000 */
    cpu->write_word(sp + 76, t3);
    /* fall through to block_8016513C */

block_8016513C:
    t0 = cpu->read_word(sp + 76);
    v1 = cpu->read_byte(s2 + 156);
    t0 = cpu->read_byte(t0 + 2);
    v0 = (v1 < (uint32_t)6) ? 1 : 0;  /* 0x80165148: 0x2C620006 */
    int _bc_8016514C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 84, t0);
    if (_bc_8016514C) {
        psx_check_interrupts_at(cpu, 0x801651ECu);
        goto block_801651EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165154u);
        goto block_80165154;  /* not taken */
    }

block_80165154:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 8332;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80165168: 0x00000000 */
    uint32_t _jt_8016516C = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165170: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8016516C);
    cpu->pc = _jt_8016516C; return;  /* CPS: jr v0 */

block_80165174:
    v1 = 0x1F80 << 16;  /* 0x1F800000 */
    v1 = cpu->read_word(v1 + 192);
    /* nop */  /* 0x8016517C: 0x00000000 */
    v0 = cpu->read_byte(v1 + 9);
    /* nop */  /* 0x80165184: 0x00000000 */
    v0 = (v0 < (uint32_t)2) ? 1 : 0;  /* 0x80165188: 0x2C420002 */
    int _bc_8016518C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t1 = -1;
    if (_bc_8016518C) {
        psx_check_interrupts_at(cpu, 0x801651E8u);
        goto block_801651E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165194u);
        goto block_80165194;  /* not taken */
    }

block_80165194:
    v0 = cpu->read_byte(v1 + 6);
    /* nop */  /* 0x80165198: 0x00000000 */
    int _bc_8016519C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t2 = -1;
    if (_bc_8016519C) {
        psx_check_interrupts_at(cpu, 0x801651ACu);
        goto block_801651AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801651A4u);
        goto block_801651A4;  /* not taken */
    }

block_801651A4:
    /* delay slot (always executes) */
    cpu->write_word(sp + 80, t2);
    psx_check_interrupts_at(cpu, 0x801651ECu);
    goto block_801651EC;  /* j */

block_801651AC:
    t3 = cpu->read_word(sp + 76);
    a0 = cpu->read_word(sp + 88);
    a1 = cpu->read_word(t3 + 240);
    ra = 0x801651C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = 10;
    psx_check_interrupts_at(cpu, 0x80163B08u);
    cpu->pc = 0x80163B08u; return;  /* CPS jal -> 0x80163B08 */

block_801651C0:
    /* delay slot (always executes) */
    cpu->write_word(sp + 80, v0);
    psx_check_interrupts_at(cpu, 0x801651ECu);
    goto block_801651EC;  /* j */

block_801651C8:
    t0 = cpu->read_word(sp + 76);
    a0 = cpu->read_word(sp + 88);
    a1 = cpu->read_half(t0 + 230);
    ra = 0x801651DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = 10;
    psx_check_interrupts_at(cpu, 0x80163B84u);
    cpu->pc = 0x80163B84u; return;  /* CPS jal -> 0x80163B84 */

block_801651DC:
    /* delay slot (always executes) */
    cpu->write_word(sp + 80, v0);
    psx_check_interrupts_at(cpu, 0x801651ECu);
    goto block_801651EC;  /* j */

block_801651E4:
    t1 = -1;
    /* fall through to block_801651E8 */

block_801651E8:
    cpu->write_word(sp + 80, t1);
    /* fall through to block_801651EC */

block_801651EC:
    v0 = cpu->read_word(s2 + 196);
    /* nop */  /* 0x801651F0: 0x00000000 */
    v0 = cpu->read_word(v0 + 8);
    /* nop */  /* 0x801651F8: 0x00000000 */
    int _bc_801651FC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165200: 0x00000000 */
    if (_bc_801651FC) {
        psx_check_interrupts_at(cpu, 0x8016520Cu);
        goto block_8016520C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165204u);
        goto block_80165204;  /* not taken */
    }

block_80165204:
    t2 = -1;
    cpu->write_word(sp + 80, t2);
    /* fall through to block_8016520C */

block_8016520C:
    t3 = cpu->read_word(sp + 80);
    /* nop */  /* 0x80165210: 0x00000000 */
    int _bc_80165214 = ((int32_t)t3 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;
    if (_bc_80165214) {
        psx_check_interrupts_at(cpu, 0x801652A4u);
        goto block_801652A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016521Cu);
        goto block_8016521C;  /* not taken */
    }

block_8016521C:
    v0 = cpu->read_word(s2 + 192);
    /* nop */  /* 0x80165220: 0x00000000 */
    v0 = cpu->read_byte(v0 + 13);
    /* nop */  /* 0x80165228: 0x00000000 */
    int _bc_8016522C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 9;
    if (_bc_8016522C) {
        psx_check_interrupts_at(cpu, 0x80165238u);
        goto block_80165238;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165234u);
        goto block_80165234;  /* not taken */
    }

block_80165234:
    a0 = 41;
    /* fall through to block_80165238 */

block_80165238:
    ra = 0x80165240u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x801577BCu);
    cpu->pc = 0x801577BCu; return;  /* CPS jal -> 0x801577BC */

block_80165240:
    v1 = cpu->read_word(s2 + 192);
    /* nop */  /* 0x80165244: 0x00000000 */
    v0 = cpu->read_byte(v1);
    /* nop */  /* 0x8016524C: 0x00000000 */
    int _bc_80165250 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a3 = sp + 24;
    if (_bc_80165250) {
        psx_check_interrupts_at(cpu, 0x80165278u);
        goto block_80165278;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165258u);
        goto block_80165258;  /* not taken */
    }

block_80165258:
    cpu->write_byte(sp + 24, (uint8_t)v0);
    v0 = cpu->read_byte(v1 + 1);
    fp = 3;
    cpu->write_byte(sp + 25, (uint8_t)v0);
    v0 = cpu->read_byte(v1 + 2);
    s4 = 32;
    cpu->write_byte(sp + 27, (uint8_t)zero);
    cpu->write_byte(sp + 26, (uint8_t)v0);
    /* fall through to block_80165278 */

block_80165278:
    a0 = cpu->read_word(sp + 88);
    a1 = cpu->read_word(sp + 80);
    a2 = cpu->read_word(sp + 76);
    v0 = 10;
    ra = 0x80165290u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, v0);
    psx_check_interrupts_at(cpu, 0x80163BF4u);
    cpu->pc = 0x80163BF4u; return;  /* CPS jal -> 0x80163BF4 */

block_80165290:
    t0 = -1;
    t1 = 3;
    cpu->write_word(sp + 64, t0);
    /* delay slot (always executes) */
    cpu->write_byte(s2 + 1, (uint8_t)t1);
    psx_check_interrupts_at(cpu, 0x801652B0u);
    goto block_801652B0;  /* j */

block_801652A4:
    cpu->write_word(sp + 64, zero);
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    cpu->write_byte(sp + 24, (uint8_t)zero);
    /* fall through to block_801652B0 */

block_801652B0:
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    ra = 0x801652BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = -1;
    psx_check_interrupts_at(cpu, 0x8012AE3Cu);
    cpu->pc = 0x8012AE3Cu; return;  /* CPS jal -> 0x8012AE3C */

block_801652BC:
    a0 = 2;
    a1 = 1;
    a2 = 3;
    ra = 0x801652D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = zero;
    psx_check_interrupts_at(cpu, 0x8012BF0Cu);
    cpu->pc = 0x8012BF0Cu; return;  /* CPS jal -> 0x8012BF0C */

block_801652D0:
    t2 = cpu->read_word(sp + 84);
    /* nop */  /* 0x801652D4: 0x00000000 */
    v0 = t2 << 1;
    s0 = v0 + s2;
    v0 = cpu->read_half(s0 + 116);
    /* nop */  /* 0x801652E4: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801652E8: 0x30428000 */
    int _bc_801652EC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;
    if (_bc_801652EC) {
        psx_check_interrupts_at(cpu, 0x80165304u);
        goto block_80165304;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801652F4u);
        goto block_801652F4;  /* not taken */
    }

block_801652F4:
    a1 = 11;
    ra = 0x80165300u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = zero;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_80165300:
    s4 = s4 + -1;
    /* fall through to block_80165304 */

block_80165304:
    v0 = cpu->read_half(s0 + 116);
    /* nop */  /* 0x80165308: 0x00000000 */
    v0 = v0 & 0x2000;  /* 0x8016530C: 0x30422000 */
    int _bc_80165310 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;
    if (_bc_80165310) {
        psx_check_interrupts_at(cpu, 0x80165328u);
        goto block_80165328;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165318u);
        goto block_80165318;  /* not taken */
    }

block_80165318:
    a1 = 11;
    ra = 0x80165324u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = zero;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_80165324:
    s4 = s4 + 1;
    /* fall through to block_80165328 */

block_80165328:
    v0 = 0x2FA0 << 16;  /* 0x2FA00000 */
    v0 = v0 | 0xBE83;
    a0 = s4 + 43;
    { int64_t result = (int64_t)(int32_t)a0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a0));
    v0 = (int32_t)a0 >> 31;
    t0 = cpu->hi;
    psx_muldiv_stall(cpu);
    v1 = (int32_t)t0 >> 3;
    s4 = v1 - v0;
    v0 = s4 << 1;
    v0 = v0 + s4;
    v0 = v0 << 2;
    v0 = v0 - s4;
    v0 = v0 << 2;
    v0 = v0 - s4;
    s4 = a0 - v0;
    v0 = ((int32_t)fp < 3) ? 1 : 0;  /* 0x80165364: 0x2BC20003 */
    int _bc_80165368 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (_bc_80165368) {
        psx_check_interrupts_at(cpu, 0x80165390u);
        goto block_80165390;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165370u);
        goto block_80165370;  /* not taken */
    }

block_80165370:
    v0 = ((int32_t)s4 < 31) ? 1 : 0;  /* 0x80165370: 0x2A82001F */
    int _bc_80165374 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)s4 < 33) ? 1 : 0;  /* 0x80165378: 0x2A820021 */
    if (_bc_80165374) {
        psx_check_interrupts_at(cpu, 0x80165384u);
        goto block_80165384;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016537Cu);
        goto block_8016537C;  /* not taken */
    }

block_8016537C:
    s4 = 31;
    v0 = ((int32_t)s4 < 33) ? 1 : 0;  /* 0x80165380: 0x2A820021 */
    /* fall through to block_80165384 */

block_80165384:
    int _bc_80165384 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x801B << 16;  /* 0x801B0000 */
    if (_bc_80165384) {
        psx_check_interrupts_at(cpu, 0x80165390u);
        goto block_80165390;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016538Cu);
        goto block_8016538C;  /* not taken */
    }

block_8016538C:
    s4 = 32;
    /* fall through to block_80165390 */

block_80165390:
    v0 = v0 + 17340;
    v0 = s4 + v0;
    t1 = cpu->read_word(sp + 96);
    v0 = cpu->read_byte(v0);
    s0 = t1 + fp;
    cpu->write_byte(s0, (uint8_t)v0);
    t2 = cpu->read_word(sp + 84);
    /* nop */  /* 0x801653AC: 0x00000000 */
    v0 = t2 << 1;
    v0 = v0 + s2;
    v0 = cpu->read_half(v0 + 104);
    /* nop */  /* 0x801653BC: 0x00000000 */
    v0 = v0 & 0x00E0;  /* 0x801653C0: 0x304200E0 */
    int _bc_801653C4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 60;
    if (_bc_801653C4) {
        psx_check_interrupts_at(cpu, 0x801654BCu);
        goto block_801654BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801653CCu);
        goto block_801653CC;  /* not taken */
    }

block_801653CC:
    v1 = cpu->read_byte(s0);
    /* nop */  /* 0x801653D0: 0x00000000 */
    int _bc_801653D4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 94;
    if (_bc_801653D4) {
        psx_check_interrupts_at(cpu, 0x801653ECu);
        goto block_801653EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801653DCu);
        goto block_801653DC;  /* not taken */
    }

block_801653DC:
    int _bc_801653DC = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;
    if (_bc_801653DC) {
        psx_check_interrupts_at(cpu, 0x80165410u);
        goto block_80165410;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801653E4u);
        goto block_801653E4;  /* not taken */
    }

block_801653E4:
    /* delay slot (always executes) */
    a1 = 12;
    psx_check_interrupts_at(cpu, 0x801654A0u);
    goto block_801654A0;  /* j */

block_801653EC:
    a0 = 1024;
    a1 = 13;
    ra = 0x801653FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = zero;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_801653FC:
    int _bc_801653FC = ((int32_t)fp <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (_bc_801653FC) {
        psx_check_interrupts_at(cpu, 0x801654BCu);
        goto block_801654BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165404u);
        goto block_80165404;  /* not taken */
    }

block_80165404:
    cpu->write_byte(s0, (uint8_t)v0);
    /* delay slot (always executes) */
    fp = fp + -1;
    psx_check_interrupts_at(cpu, 0x801654BCu);
    goto block_801654BC;  /* j */

block_80165410:
    a1 = 12;
    ra = 0x8016541Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = zero;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8016541C:
    a0 = cpu->read_word(sp + 96);
    v0 = 32;
    cpu->write_byte(s0, (uint8_t)v0);
    ra = 0x80165430u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(sp + 27, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80163D7Cu);
    cpu->pc = 0x80163D7Cu; return;  /* CPS jal -> 0x80163D7C */

block_80165430:
    a0 = cpu->read_word(sp + 88);
    a1 = cpu->read_word(sp + 80);
    a2 = cpu->read_word(sp + 96);
    ra = 0x80165444u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s7 = 32;
    psx_check_interrupts_at(cpu, 0x80163BC0u);
    cpu->pc = 0x80163BC0u; return;  /* CPS jal -> 0x80163BC0 */

block_80165444:
    v1 = cpu->read_word(s2 + 192);
    v0 = cpu->read_byte(sp + 24);
    /* nop */  /* 0x8016544C: 0x00000000 */
    cpu->write_byte(v1, (uint8_t)v0);
    v1 = cpu->read_word(s2 + 192);
    v0 = cpu->read_byte(sp + 25);
    /* nop */  /* 0x8016545C: 0x00000000 */
    cpu->write_byte(v1 + 1, (uint8_t)v0);
    v1 = cpu->read_word(s2 + 192);
    v0 = cpu->read_byte(sp + 26);
    t3 = 0x801F << 16;  /* 0x801F0000 */
    cpu->write_byte(v1 + 2, (uint8_t)v0);
    v0 = cpu->read_word(s2 + 192);
    t3 = t3 + -31480;
    cpu->write_word(sp + 64, zero);
    cpu->write_byte(v0 + 3, (uint8_t)zero);
    v1 = cpu->read_half(s2 + 32);
    v0 = 4;
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    v1 = v1 << 16;
    v1 = (int32_t)v1 >> 16;
    /* delay slot (always executes) */
    cpu->write_word(t3 + 4, v1);
    psx_check_interrupts_at(cpu, 0x801654C0u);
    goto block_801654C0;  /* j */

block_801654A0:
    ra = 0x801654A8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = zero;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_801654A8:
    fp = fp + 1;
    v0 = ((int32_t)fp < 3) ? 1 : 0;  /* 0x801654AC: 0x2BC20003 */
    int _bc_801654B0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s7 = 32;
    if (_bc_801654B0) {
        psx_check_interrupts_at(cpu, 0x801654C0u);
        goto block_801654C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801654B8u);
        goto block_801654B8;  /* not taken */
    }

block_801654B8:
    s4 = 32;
    /* fall through to block_801654BC */

block_801654BC:
    s7 = 32;
    /* fall through to block_801654C0 */

block_801654C0:
    a0 = s7;
    ra = 0x801654CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 48;
    psx_check_interrupts_at(cpu, 0x8012CD9Cu);
    cpu->pc = 0x8012CD9Cu; return;  /* CPS jal -> 0x8012CD9C */

block_801654CC:
    s3 = zero;
    s6 = cpu->read_word(sp + 88);
    v0 = 0x801B << 16;  /* 0x801B0000 */
    s5 = v0 + 16980;
    /* fall through to block_801654DC */

block_801654DC:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a1 = a1 + 8160;
    a0 = cpu->read_word(sp + 92);
    ra = 0x801654F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s3 + 1;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_801654F0:
    t0 = cpu->read_word(sp + 80);
    /* nop */  /* 0x801654F4: 0x00000000 */
    int _bc_801654F8 = (t0 != s3);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t3 = 4096;
    if (_bc_801654F8) {
        psx_check_interrupts_at(cpu, 0x8016566Cu);
        goto block_8016566C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165500u);
        goto block_80165500;  /* not taken */
    }

block_80165500:
    a0 = zero;
    ra = 0x8016550Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = -4;
    psx_check_interrupts_at(cpu, 0x8012CDD8u);
    cpu->pc = 0x8012CDD8u; return;  /* CPS jal -> 0x8012CDD8 */

block_8016550C:
    a3 = s7;
    t1 = 4096;
    a2 = cpu->read_word(sp + 92);
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    t2 = 3;
    cpu->write_word(sp + 16, t1);
    ra = 0x80165530u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, t2);
    psx_check_interrupts_at(cpu, 0x8012E6A8u);
    cpu->pc = 0x8012E6A8u; return;  /* CPS jal -> 0x8012E6A8 */

block_80165530:
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    t3 = 4096;
    cpu->write_word(sp + 16, t3);
    cpu->write_word(sp + 20, zero);
    a2 = cpu->read_word(s5);
    ra = 0x80165550u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = t3;
    psx_check_interrupts_at(cpu, 0x8012E6A8u);
    cpu->pc = 0x8012E6A8u; return;  /* CPS jal -> 0x8012E6A8 */

block_80165550:
    v0 = cpu->read_byte(s2 + 156);
    t0 = 3;
    int _bc_80165558 = (v0 != t0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = 19;
    if (_bc_80165558) {
        psx_check_interrupts_at(cpu, 0x80165564u);
        goto block_80165564;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165560u);
        goto block_80165560;  /* not taken */
    }

block_80165560:
    s0 = 21;
    /* fall through to block_80165564 */

block_80165564:
    a2 = s7 + 64;
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    a3 = 4096;
    cpu->write_word(sp + 16, s6);
    ra = 0x80165580u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);
    psx_check_interrupts_at(cpu, 0x80163E38u);
    cpu->pc = 0x80163E38u; return;  /* CPS jal -> 0x80163E38 */

block_80165580:
    v0 = cpu->read_byte(s2 + 1);
    t1 = 3;
    int _bc_80165588 = (v0 != t1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s1 = s7 + 232;
    if (_bc_80165588) {
        psx_check_interrupts_at(cpu, 0x80165608u);
        goto block_80165608;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165590u);
        goto block_80165590;  /* not taken */
    }

block_80165590:
    int _bc_80165590 = ((int32_t)fp < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = zero;
    if (_bc_80165590) {
        psx_check_interrupts_at(cpu, 0x80165660u);
        goto block_80165660;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165598u);
        goto block_80165598;  /* not taken */
    }

block_80165598:
    v0 = ((int32_t)s0 < 4) ? 1 : 0;  /* 0x80165598: 0x2A020004 */
    /* fall through to block_8016559C */

block_8016559C:
    int _bc_8016559C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    if (_bc_8016559C) {
        psx_check_interrupts_at(cpu, 0x80165664u);
        goto block_80165664;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801655A4u);
        goto block_801655A4;  /* not taken */
    }

block_801655A4:
    int _bc_801655A4 = (s0 != fp);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = sp + 48;
    if (_bc_801655A4) {
        psx_check_interrupts_at(cpu, 0x801655C4u);
        goto block_801655C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801655ACu);
        goto block_801655AC;  /* not taken */
    }

block_801655AC:
    v0 = 0x1F80 << 16;  /* 0x1F800000 */
    v0 = cpu->read_word(v0 + 220);
    /* nop */  /* 0x801655B4: 0x00000000 */
    v0 = v0 & 0x0008;  /* 0x801655B8: 0x30420008 */
    int _bc_801655BC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801655C0: 0x00000000 */
    if (_bc_801655BC) {
        psx_check_interrupts_at(cpu, 0x801655F0u);
        goto block_801655F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801655C4u);
        goto block_801655C4;  /* not taken */
    }

block_801655C4:
    a0 = cpu->read_word(s2 + 8);
    t2 = cpu->read_word(sp + 96);
    a1 = cpu->read_word(s2 + 12);
    v0 = t2 + s0;
    v0 = cpu->read_byte(v0);
    a3 = s1;
    cpu->write_byte(sp + 48, (uint8_t)v0);
    t3 = 4096;
    cpu->write_word(sp + 16, t3);
    ra = 0x801655F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);
    psx_check_interrupts_at(cpu, 0x8012E6A8u);
    cpu->pc = 0x8012E6A8u; return;  /* CPS jal -> 0x8012E6A8 */

block_801655F0:
    s0 = s0 + 1;
    v0 = ((int32_t)fp < (int32_t)s0) ? 1 : 0;  /* 0x801655F4: 0x03D0102A */
    int _bc_801655F8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s1 = s1 + 16;
    if (_bc_801655F8) {
        psx_check_interrupts_at(cpu, 0x80165660u);
        goto block_80165660;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165600u);
        goto block_80165600;  /* not taken */
    }

block_80165600:
    /* delay slot (always executes) */
    v0 = ((int32_t)s0 < 4) ? 1 : 0;  /* 0x80165604: 0x2A020004 */
    psx_check_interrupts_at(cpu, 0x8016559Cu);
    goto block_8016559C;  /* j */

block_80165608:
    v0 = 0x1F80 << 16;  /* 0x1F800000 */
    v0 = cpu->read_word(v0 + 220);
    /* nop */  /* 0x80165610: 0x00000000 */
    v0 = v0 & 0x0004;  /* 0x80165614: 0x30420004 */
    int _bc_80165618 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = sp + 24;
    if (_bc_80165618) {
        psx_check_interrupts_at(cpu, 0x8016563Cu);
        goto block_8016563C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165620u);
        goto block_80165620;  /* not taken */
    }

block_80165620:
    a3 = s1;
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    t0 = 4096;
    cpu->write_word(sp + 16, t0);
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);
    psx_check_interrupts_at(cpu, 0x80165658u);
    goto block_80165658;  /* j */

block_8016563C:
    a3 = s1;
    t1 = 4096;
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    t2 = 3;
    cpu->write_word(sp + 16, t1);
    cpu->write_word(sp + 20, t2);
    /* fall through to block_80165658 */

block_80165658:
    ra = 0x80165660u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016565C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012E6A8u);
    cpu->pc = 0x8012E6A8u; return;  /* CPS jal -> 0x8012E6A8 */

block_80165660:
    a0 = zero;
    /* fall through to block_80165664 */

block_80165664:
    /* delay slot (always executes) */
    a1 = 20;
    psx_check_interrupts_at(cpu, 0x801656E4u);
    goto block_801656E4;  /* j */

block_8016566C:
    a3 = s7;
    a2 = cpu->read_word(sp + 92);
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    t0 = 3;
    cpu->write_word(sp + 16, t3);
    ra = 0x8016568Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, t0);
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_8016568C:
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    t1 = 4096;
    cpu->write_word(sp + 16, t1);
    cpu->write_word(sp + 20, zero);
    a2 = cpu->read_word(s5);
    ra = 0x801656ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = t1;
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_801656AC:
    v0 = cpu->read_byte(s2 + 156);
    t2 = 3;
    int _bc_801656B4 = (v0 != t2);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = 18;
    if (_bc_801656B4) {
        psx_check_interrupts_at(cpu, 0x801656C0u);
        goto block_801656C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801656BCu);
        goto block_801656BC;  /* not taken */
    }

block_801656BC:
    s0 = 20;
    /* fall through to block_801656C0 */

block_801656C0:
    a2 = s7 + 64;
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    a3 = 4096;
    cpu->write_word(sp + 16, s6);
    ra = 0x801656DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);
    psx_check_interrupts_at(cpu, 0x80163E38u);
    cpu->pc = 0x80163E38u; return;  /* CPS jal -> 0x80163E38 */

block_801656DC:
    a0 = zero;
    a1 = 16;
    /* fall through to block_801656E4 */

block_801656E4:
    ra = 0x801656ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s6 = s6 + 16;
    psx_check_interrupts_at(cpu, 0x8012CDD8u);
    cpu->pc = 0x8012CDD8u; return;  /* CPS jal -> 0x8012CDD8 */

block_801656EC:
    s3 = s3 + 1;
    v0 = ((int32_t)s3 < 10) ? 1 : 0;  /* 0x801656F0: 0x2A62000A */
    int _bc_801656F4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = s5 + 4;
    if (_bc_801656F4) {
        psx_check_interrupts_at(cpu, 0x801654DCu);
        goto block_801654DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801656FCu);
        goto block_801656FC;  /* not taken */
    }

block_801656FC:
    a0 = 32;
    ra = 0x80165708u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 24;
    psx_check_interrupts_at(cpu, 0x8012CD9Cu);
    cpu->pc = 0x8012CD9Cu; return;  /* CPS jal -> 0x8012CD9C */

block_80165708:
    a2 = 0x8012 << 16;  /* 0x80120000 */
    a2 = a2 + 8320;
    a3 = 4096;
    a0 = cpu->read_word(s2 + 8);
    a1 = cpu->read_word(s2 + 12);
    t3 = a3;
    cpu->write_word(sp + 16, t3);
    ra = 0x8016572Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_8016572C:
    a0 = zero;
    a1 = a0;
    a2 = 4;
    ra = 0x80165740u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = 7;
    psx_check_interrupts_at(cpu, 0x8012BF0Cu);
    cpu->pc = 0x8012BF0Cu; return;  /* CPS jal -> 0x8012BF0C */

block_80165740:
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    a1 = cpu->read_word(s2 + 8);
    ra = 0x80165750u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = 35;
    psx_check_interrupts_at(cpu, 0x8012C374u);
    cpu->pc = 0x8012C374u; return;  /* CPS jal -> 0x8012C374 */

block_80165750:
    /* delay slot (always executes) */
    /* nop */  /* 0x80165754: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801658B4u);
    goto block_801658B4;  /* j */

block_80165758:
    t0 = cpu->read_word(sp + 64);
    /* nop */  /* 0x8016575C: 0x00000000 */
    int _bc_80165760 = (t0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80165764: 0x00000000 */
    if (_bc_80165760) {
        psx_check_interrupts_at(cpu, 0x801657BCu);
        goto block_801657BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165768u);
        goto block_80165768;  /* not taken */
    }

block_80165768:
    v0 = cpu->read_word(s2 + 192);
    /* nop */  /* 0x8016576C: 0x00000000 */
    v0 = cpu->read_byte(v0 + 13);
    /* nop */  /* 0x80165774: 0x00000000 */
    int _bc_80165778 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 7;
    if (_bc_80165778) {
        psx_check_interrupts_at(cpu, 0x80165784u);
        goto block_80165784;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165780u);
        goto block_80165780;  /* not taken */
    }

block_80165780:
    a0 = 40;
    /* fall through to block_80165784 */

block_80165784:
    ra = 0x8016578Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x801577BCu);
    cpu->pc = 0x801577BCu; return;  /* CPS jal -> 0x801577BC */

block_8016578C:
    a0 = 1024;
    a1 = 131;
    ra = 0x8016579Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = 30;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8016579C:
    a0 = 1024;
    a1 = 132;
    ra = 0x801657ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = 35;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_801657AC:
    a0 = 1024;
    a1 = 133;
    ra = 0x801657BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = 40;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_801657BC:
    t1 = cpu->read_word(sp + 64);
    /* nop */  /* 0x801657C0: 0x00000000 */
    v0 = ((int32_t)t1 < 101) ? 1 : 0;  /* 0x801657C4: 0x29220065 */
    int _bc_801657C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801657CC: 0x00000000 */
    if (_bc_801657C8) {
        psx_check_interrupts_at(cpu, 0x801657ECu);
        goto block_801657EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801657D0u);
        goto block_801657D0;  /* not taken */
    }

block_801657D0:
    v0 = 0x1F80 << 16;  /* 0x1F800000 */
    v0 = cpu->read_half(v0 + 124);
    /* nop */  /* 0x801657D8: 0x00000000 */
    v0 = v0 & 0x08E0;  /* 0x801657DC: 0x304208E0 */
    int _bc_801657E0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t2 = 4950;
    if (_bc_801657E0) {
        psx_check_interrupts_at(cpu, 0x801657ECu);
        goto block_801657EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801657E8u);
        goto block_801657E8;  /* not taken */
    }

block_801657E8:
    cpu->write_word(sp + 64, t2);
    /* fall through to block_801657EC */

block_801657EC:
    t3 = cpu->read_word(sp + 64);
    /* nop */  /* 0x801657F0: 0x00000000 */
    v0 = ((int32_t)t3 < 751) ? 1 : 0;  /* 0x801657F4: 0x296202EF */
    int _bc_801657F8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    if (_bc_801657F8) {
        psx_check_interrupts_at(cpu, 0x8016580Cu);
        goto block_8016580C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80165800u);
        goto block_80165800;  /* not taken */
    }

block_80165800:
    v0 = 240;
    at = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->write_byte(at + 1, (uint8_t)v0);
    /* fall through to block_8016580C */

block_8016580C:
    a1 = a0;
    a2 = 3;
    t0 = cpu->read_word(sp + 64);
    a3 = a2;
    t0 = t0 + 1;
    ra = 0x80165828u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 64, t0);
    psx_check_interrupts_at(cpu, 0x8012BF0Cu);
    cpu->pc = 0x8012BF0Cu; return;  /* CPS jal -> 0x8012BF0C */

block_80165828:
    /* delay slot (always executes) */
    s7 = 32;
    psx_check_interrupts_at(cpu, 0x801654C0u);
    goto block_801654C0;  /* j */

block_80165830:
    ra = 0x80165838u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    psx_check_interrupts_at(cpu, 0x801525C4u);
    cpu->pc = 0x801525C4u; return;  /* CPS jal -> 0x801525C4 */

block_80165838:
    a0 = cpu->read_word(sp + 76);
    ra = 0x80165844u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 24;
    psx_check_interrupts_at(cpu, 0x80163CBCu);
    cpu->pc = 0x80163CBCu; return;  /* CPS jal -> 0x80163CBC */

block_80165844:
    a1 = 1;
    a0 = cpu->read_word(sp + 76);
    ra = 0x80165854u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = sp + 24;
    psx_check_interrupts_at(cpu, 0x801279D0u);
    cpu->pc = 0x801279D0u; return;  /* CPS jal -> 0x801279D0 */

block_80165854:
    a0 = 516;
    a1 = zero;
    ra = 0x80165864u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_80165864:
    ra = 0x8016586Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    psx_check_interrupts_at(cpu, 0x8012B23Cu);
    cpu->pc = 0x8012B23Cu; return;  /* CPS jal -> 0x8012B23C */

block_8016586C:
    a0 = 1;
    ra = 0x80165878u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 8;
    psx_check_interrupts_at(cpu, 0x8012AA6Cu);
    cpu->pc = 0x8012AA6Cu; return;  /* CPS jal -> 0x8012AA6C */

block_80165878:
    a0 = zero;
    a1 = a0;
    ra = 0x80165888u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a0;
    psx_check_interrupts_at(cpu, 0x801274FCu);
    cpu->pc = 0x801274FCu; return;  /* CPS jal -> 0x801274FC */

block_80165888:
    v0 = cpu->read_byte(s2 + 1);
    /* nop */  /* 0x8016588C: 0x00000000 */
    v0 = v0 + 1;
    /* delay slot (always executes) */
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x801658B4u);
    goto block_801658B4;  /* j */

block_8016589C:
    ra = 0x801658A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801658A0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_801658A4:
    int _bc_801658A4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 255;
    if (_bc_801658A4) {
        psx_check_interrupts_at(cpu, 0x801658B4u);
        goto block_801658B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801658ACu);
        goto block_801658AC;  /* not taken */
    }

block_801658AC:
    /* delay slot (always executes) */
    cpu->write_byte(s2 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x801658BCu);
    goto block_801658BC;  /* j */

block_801658B4:
    ra = 0x801658BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_801658BC:
    v1 = 0x1F80 << 16;  /* 0x1F800000 */
    v1 = cpu->read_byte(v1 + 1);
    v0 = 255;
    int _bc_801658C8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801658CC: 0x00000000 */
    if (_bc_801658C8) {
        psx_check_interrupts_at(cpu, 0x80164FDCu);
        goto block_80164FDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801658D0u);
        goto block_801658D0;  /* not taken */
    }

block_801658D0:
    ra = cpu->read_word(sp + 140);
    fp = cpu->read_word(sp + 136);
    s7 = cpu->read_word(sp + 132);
    s6 = cpu->read_word(sp + 128);
    s5 = cpu->read_word(sp + 124);
    s4 = cpu->read_word(sp + 120);
    s3 = cpu->read_word(sp + 116);
    s2 = cpu->read_word(sp + 112);
    s1 = cpu->read_word(sp + 108);
    s0 = cpu->read_word(sp + 104);
    at = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->write_byte(at + 1, (uint8_t)zero);
    uint32_t _jt_80165900 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 144;
    psx_check_interrupts_at(cpu, _jt_80165900);
    cpu->pc = _jt_80165900; return;  /* CPS: jr $ra */
}


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

block_80168424:
    sp = sp + -40;
    cpu->write_word(sp + 28, s1);
    s1 = a0;
    cpu->write_word(sp + 24, s0);
    s0 = zero;
    v0 = 0x8012 << 16;  /* 0x80120000 */
    cpu->write_word(sp + 32, s2);
    s2 = v0 + 8492;
    cpu->write_word(sp + 36, ra);
    /* fall through to block_80168448 */

block_80168448:
    ra = 0x80168450u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x8019EA98u);
    cpu->pc = 0x8019EA98u; return;  /* CPS jal -> 0x8019EA98 */

block_80168450:
    int _bc_80168450 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    if (_bc_80168450) {
        psx_check_interrupts_at(cpu, 0x80168448u);
        goto block_80168448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80168458u);
        goto block_80168458;  /* not taken */
    }

block_80168458:
    a1 = sp + 16;
    ra = 0x80168464u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = sp + 20;
    psx_check_interrupts_at(cpu, 0x8019FA80u);
    cpu->pc = 0x8019FA80u; return;  /* CPS jal -> 0x8019FA80 */

block_80168464:
    int _bc_80168464 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    if (_bc_80168464) {
        psx_check_interrupts_at(cpu, 0x80168458u);
        goto block_80168458;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016846Cu);
        goto block_8016846C;  /* not taken */
    }

block_8016846C:
    v1 = cpu->read_word(sp + 20);
    /* nop */  /* 0x80168470: 0x00000000 */
    v0 = (v1 < (uint32_t)5) ? 1 : 0;  /* 0x80168474: 0x2C620005 */
    int _bc_80168478 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (_bc_80168478) {
        psx_check_interrupts_at(cpu, 0x80168494u);
        goto block_80168494;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80168480u);
        goto block_80168480;  /* not taken */
    }

block_80168480:
    v0 = v0 + s2;
    v0 = cpu->read_word(v0);
    /* nop */  /* 0x80168488: 0x00000000 */
    uint32_t _jt_8016848C = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80168490: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8016848C);
    cpu->pc = _jt_8016848C; return;  /* CPS: jr v0 */

block_80168494:
    v0 = cpu->read_word(sp + 20);
    /* nop */  /* 0x80168498: 0x00000000 */
    int _bc_8016849C = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    if (_bc_8016849C) {
        psx_check_interrupts_at(cpu, 0x801684B0u);
        goto block_801684B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801684A4u);
        goto block_801684A4;  /* not taken */
    }

block_801684A4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801684A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80168448u);
    goto block_80168448;  /* j */

block_801684AC:
    v0 = cpu->read_word(sp + 20);
    /* fall through to block_801684B0 */

block_801684B0:
    ra = cpu->read_word(sp + 36);
    s2 = cpu->read_word(sp + 32);
    s1 = cpu->read_word(sp + 28);
    s0 = cpu->read_word(sp + 24);
    uint32_t _jt_801684C0 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_801684C0);
    cpu->pc = _jt_801684C0; return;  /* CPS: jr $ra */
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

block_801685B4:
    sp = sp + -40;
    cpu->write_word(sp + 24, s2);
    s2 = a0;
    cpu->write_word(sp + 28, s3);
    s3 = a1;
    cpu->write_word(sp + 32, s4);
    s4 = a2;
    cpu->write_word(sp + 16, s0);
    s0 = zero;
    v0 = 0x8012 << 16;  /* 0x80120000 */
    cpu->write_word(sp + 20, s1);
    s1 = v0 + 8516;
    cpu->write_word(sp + 36, ra);
    /* fall through to block_801685E8 */

block_801685E8:
    a0 = s2;
    a1 = s3;
    ra = 0x801685F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x8019FB9Cu);
    cpu->pc = 0x8019FB9Cu; return;  /* CPS jal -> 0x8019FB9C */

block_801685F8:
    v1 = v0;
    v0 = (v1 < (uint32_t)8) ? 1 : 0;  /* 0x801685FC: 0x2C620008 */
    int _bc_80168600 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 << 2;
    if (_bc_80168600) {
        psx_check_interrupts_at(cpu, 0x8016861Cu);
        goto block_8016861C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80168608u);
        goto block_80168608;  /* not taken */
    }

block_80168608:
    v0 = v0 + s1;
    v0 = cpu->read_word(v0);
    /* nop */  /* 0x80168610: 0x00000000 */
    uint32_t _jt_80168614 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80168618: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80168614);
    cpu->pc = _jt_80168614; return;  /* CPS: jr v0 */

block_8016861C:
    int _bc_8016861C = (s0 != v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s0 = v1;
    if (_bc_8016861C) {
        psx_check_interrupts_at(cpu, 0x801685E8u);
        goto block_801685E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80168624u);
        goto block_80168624;  /* not taken */
    }

block_80168624:
    v0 = v1;
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t _jt_80168640 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, _jt_80168640);
    cpu->pc = _jt_80168640; return;  /* CPS: jr $ra */
}


/*  battle_flow_state_machine (guest body ~0x8016FCC4, renames.map) — */
/*  battle flow state machine. Largest body in this file (~2450 lines, */
/*  piece entries 0x8016FD38..0x801709C8); continuation dispatch via */
/*  cpu->pc. */

void battle_flow_state_machine_alt(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  uint32_t entry = cpu->pc;  /* dispatched addr: CLEAN_ENT_CPS sets cpu->pc */

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8016FD7Cu: goto block_8016FD7C;
            case 0x8016FD84u: goto block_8016FD84;
            case 0x8016FDA0u: goto block_8016FDA0;
            case 0x8016FDA8u: goto block_8016FDA8;
            case 0x8016FDB0u: goto block_8016FDB0;
            case 0x8016FDC0u: goto block_8016FDC0;
            case 0x8016FDD4u: goto block_8016FDD4;
            case 0x8016FDDCu: goto block_8016FDDC;
            case 0x8016FE44u: goto block_8016FE44;
            case 0x8016FE68u: goto block_8016FE68;
            case 0x8016FE9Cu: goto block_8016FE9C;
            case 0x8016FED0u: goto block_8016FED0;
            case 0x8016FEF4u: goto block_8016FEF4;
            case 0x8016FF20u: goto block_8016FF20;
            case 0x8016FF28u: goto block_8016FF28;
            case 0x8016FF44u: goto block_8016FF44;
            case 0x8016FF80u: goto block_8016FF80;
            case 0x8016FF98u: goto block_8016FF98;
            case 0x8016FFA0u: goto block_8016FFA0;
            case 0x8016FFF0u: goto block_8016FFF0;
            case 0x8016FFF8u: goto block_8016FFF8;
            case 0x80170008u: goto block_80170008;
            case 0x80170018u: goto block_80170018;
            case 0x8017002Cu: goto block_8017002C;
            case 0x8017003Cu: goto block_8017003C;
            case 0x80170044u: goto block_80170044;
            case 0x80170058u: goto block_80170058;
            case 0x80170080u: goto block_80170080;
            case 0x801700D0u: goto block_801700D0;
            case 0x801700FCu: goto block_801700FC;
            case 0x80170104u: goto block_80170104;
            case 0x80170110u: goto block_80170110;
            case 0x80170118u: goto block_80170118;
            case 0x80170120u: goto block_80170120;
            case 0x80170130u: goto block_80170130;
            case 0x8017013Cu: goto block_8017013C;
            case 0x80170158u: goto block_80170158;
            case 0x80170168u: goto block_80170168;
            case 0x80170170u: goto block_80170170;
            case 0x80170178u: goto block_80170178;
            case 0x80170180u: goto block_80170180;
            case 0x80170188u: goto block_80170188;
            case 0x80170190u: goto block_80170190;
            case 0x80170198u: goto block_80170198;
            case 0x801701ACu: goto block_801701AC;
            case 0x801701C8u: goto block_801701C8;
            case 0x801701E0u: goto block_801701E0;
            case 0x801701E8u: goto block_801701E8;
            case 0x801701F0u: goto block_801701F0;
            case 0x801701F8u: goto block_801701F8;
            case 0x80170208u: goto block_80170208;
            case 0x80170218u: goto block_80170218;
            case 0x8017022Cu: goto block_8017022C;
            case 0x80170234u: goto block_80170234;
            case 0x8017023Cu: goto block_8017023C;
            case 0x80170244u: goto block_80170244;
            case 0x80170254u: goto block_80170254;
            case 0x8017027Cu: goto block_8017027C;
            case 0x80170290u: goto block_80170290;
            case 0x801702B8u: goto block_801702B8;
            case 0x801702FCu: goto block_801702FC;
            case 0x8017030Cu: goto block_8017030C;
            case 0x80170318u: goto block_80170318;
            case 0x80170324u: goto block_80170324;
            case 0x80170334u: goto block_80170334;
            case 0x80170380u: goto block_80170380;
            case 0x80170394u: goto block_80170394;
            case 0x801703A0u: goto block_801703A0;
            case 0x801703D8u: goto block_801703D8;
            case 0x801703FCu: goto block_801703FC;
            case 0x80170420u: goto block_80170420;
            case 0x80170468u: goto block_80170468;
            case 0x80170494u: goto block_80170494;
            case 0x801704BCu: goto block_801704BC;
            case 0x80170544u: goto block_80170544;
            case 0x8017058Cu: goto block_8017058C;
            case 0x801705C8u: goto block_801705C8;
            case 0x801705ECu: goto block_801705EC;
            case 0x80170610u: goto block_80170610;
            case 0x80170658u: goto block_80170658;
            case 0x80170684u: goto block_80170684;
            case 0x801706ACu: goto block_801706AC;
            case 0x80170734u: goto block_80170734;
            case 0x8017077Cu: goto block_8017077C;
            case 0x801707ACu: goto block_801707AC;
            case 0x80170808u: goto block_80170808;
            case 0x80170830u: goto block_80170830;
            case 0x80170878u: goto block_80170878;
            case 0x8017089Cu: goto block_8017089C;
            case 0x801708C0u: goto block_801708C0;
            case 0x801708D8u: goto block_801708D8;
            case 0x801708F0u: goto block_801708F0;
            case 0x80170900u: goto block_80170900;
            case 0x80170908u: goto block_80170908;
            case 0x80170910u: goto block_80170910;
            case 0x80170918u: goto block_80170918;
            case 0x80170928u: goto block_80170928;
            case 0x80170934u: goto block_80170934;
            case 0x80170944u: goto block_80170944;
            case 0x80170954u: goto block_80170954;
            case 0x80170968u: goto block_80170968;
            case 0x80170988u: goto block_80170988;
            case 0x801709A0u: goto block_801709A0;
            case 0x801709C8u: goto block_801709C8;
            case 0x8016FD38u: goto block_8016FD38;
            case 0x8016FD50u: goto block_8016FD50;
            case 0x8016FD8Cu: goto block_8016FD8C;
            case 0x8016FDF0u: goto block_8016FDF0;
            case 0x8016FE24u: goto block_8016FE24;
            case 0x8016FE3Cu: goto block_8016FE3C;
            case 0x8016FE4Cu: goto block_8016FE4C;
            case 0x8016FE58u: goto block_8016FE58;
            case 0x8016FE60u: goto block_8016FE60;
            case 0x8016FE70u: goto block_8016FE70;
            case 0x8016FE90u: goto block_8016FE90;
            case 0x8016FE94u: goto block_8016FE94;
            case 0x8016FEA4u: goto block_8016FEA4;
            case 0x8016FED8u: goto block_8016FED8;
            case 0x8016FEFCu: goto block_8016FEFC;
            case 0x8016FF64u: goto block_8016FF64;
            case 0x8016FF78u: goto block_8016FF78;
            case 0x8016FF88u: goto block_8016FF88;
            case 0x8016FF90u: goto block_8016FF90;
            case 0x8016FFB4u: goto block_8016FFB4;
            case 0x8016FFC4u: goto block_8016FFC4;
            case 0x8016FFD0u: goto block_8016FFD0;
            case 0x8016FFD8u: goto block_8016FFD8;
            case 0x80170060u: goto block_80170060;
            case 0x80170070u: goto block_80170070;
            case 0x80170088u: goto block_80170088;
            case 0x8017008Cu: goto block_8017008C;
            case 0x801700BCu: goto block_801700BC;
            case 0x801700C0u: goto block_801700C0;
            case 0x801700D8u: goto block_801700D8;
            case 0x8017025Cu: goto block_8017025C;
            case 0x80170270u: goto block_80170270;
            case 0x80170284u: goto block_80170284;
            case 0x801702A8u: goto block_801702A8;
            case 0x801702B0u: goto block_801702B0;
            case 0x801702C0u: goto block_801702C0;
            case 0x801702E0u: goto block_801702E0;
            case 0x801702E4u: goto block_801702E4;
            case 0x801702F4u: goto block_801702F4;
            case 0x8017034Cu: goto block_8017034C;
            case 0x8017035Cu: goto block_8017035C;
            case 0x8017036Cu: goto block_8017036C;
            case 0x80170370u: goto block_80170370;
            case 0x80170384u: goto block_80170384;
            case 0x80170388u: goto block_80170388;
            case 0x801703B4u: goto block_801703B4;
            case 0x801703C8u: goto block_801703C8;
            case 0x801703ECu: goto block_801703EC;
            case 0x801703F4u: goto block_801703F4;
            case 0x80170428u: goto block_80170428;
            case 0x80170448u: goto block_80170448;
            case 0x80170458u: goto block_80170458;
            case 0x80170460u: goto block_80170460;
            case 0x8017048Cu: goto block_8017048C;
            case 0x801704B4u: goto block_801704B4;
            case 0x801704D4u: goto block_801704D4;
            case 0x801704E4u: goto block_801704E4;
            case 0x80170500u: goto block_80170500;
            case 0x80170508u: goto block_80170508;
            case 0x80170510u: goto block_80170510;
            case 0x80170518u: goto block_80170518;
            case 0x80170524u: goto block_80170524;
            case 0x8017054Cu: goto block_8017054C;
            case 0x80170550u: goto block_80170550;
            case 0x801705A4u: goto block_801705A4;
            case 0x801705B8u: goto block_801705B8;
            case 0x801705DCu: goto block_801705DC;
            case 0x801705E4u: goto block_801705E4;
            case 0x80170618u: goto block_80170618;
            case 0x80170638u: goto block_80170638;
            case 0x80170648u: goto block_80170648;
            case 0x80170650u: goto block_80170650;
            case 0x8017067Cu: goto block_8017067C;
            case 0x801706A4u: goto block_801706A4;
            case 0x801706C4u: goto block_801706C4;
            case 0x801706D4u: goto block_801706D4;
            case 0x801706F0u: goto block_801706F0;
            case 0x801706F8u: goto block_801706F8;
            case 0x80170700u: goto block_80170700;
            case 0x80170708u: goto block_80170708;
            case 0x80170714u: goto block_80170714;
            case 0x8017073Cu: goto block_8017073C;
            case 0x80170740u: goto block_80170740;
            case 0x80170794u: goto block_80170794;
            case 0x8017079Cu: goto block_8017079C;
            case 0x801707C0u: goto block_801707C0;
            case 0x801707D4u: goto block_801707D4;
            case 0x801707E0u: goto block_801707E0;
            case 0x801707E4u: goto block_801707E4;
            case 0x80170820u: goto block_80170820;
            case 0x80170840u: goto block_80170840;
            case 0x80170850u: goto block_80170850;
            case 0x80170868u: goto block_80170868;
            case 0x80170888u: goto block_80170888;
            case 0x801708ACu: goto block_801708AC;
            case 0x801708D0u: goto block_801708D0;
            case 0x801708E8u: goto block_801708E8;
            case 0x80170978u: goto block_80170978;
            case 0x80170990u: goto block_80170990;
            case 0x801709B0u: goto block_801709B0;
            case 0x801709C0u: goto block_801709C0;
            default: break;
        }
    }
    switch (entry) {
        case 0x8016FD6Cu: goto block_8016FD6C;
        case 0x8016FD74u: goto block_8016FD74;
        case 0x8016FE10u: goto block_8016FE10;
        case 0x8016FE78u: goto block_8016FE78;
        case 0x8016FEBCu: goto block_8016FEBC;
        case 0x8016FEE0u: goto block_8016FEE0;
        case 0x8016FF04u: goto block_8016FF04;
        case 0x8016FFE0u: goto block_8016FFE0;
        case 0x80170050u: goto block_80170050;
        case 0x801700ACu: goto block_801700AC;
        case 0x801700E8u: goto block_801700E8;
        case 0x80170144u: goto block_80170144;
        case 0x8017024Cu: goto block_8017024C;
        case 0x80170260u: goto block_80170260;
        case 0x801708F8u: goto block_801708F8;
        case 0x80170960u: goto block_80170960;
        case 0x80170980u: goto block_80170980;
        case 0x80170998u: goto block_80170998;
        default: return;
    }

block_8016FD38:
    v1 = cpu->read_byte(s0 + 1);
    s4 = cpu->read_word(s0 + 12);
    s5 = cpu->read_word(s0 + 8);
    v0 = (v1 < (uint32_t)13) ? 1 : 0;  /* 0x8016FD44: 0x2C62000D */
    int _bc_8016FD48 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8016FD48) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FD50u);
        goto block_8016FD50;  /* not taken */
    }

block_8016FD50:
    v0 = v0 + 9680;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x8016FD60: 0x00000000 */
    uint32_t _jt_8016FD64 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD68: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8016FD64);
    cpu->pc = _jt_8016FD64; return;  /* CPS: jr v0 */

block_8016FD6C:
    v0 = 1;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_8016FD74 */

block_8016FD74:
    ra = 0x8016FD7Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD78: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80136DDCu);
    cpu->pc = 0x80136DDCu; return;  /* CPS jal -> 0x80136DDC */

block_8016FD7C:
    ra = 0x8016FD84u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD80: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_8016FD84:
    int _bc_8016FD84 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD88: 0x00000000 */
    if (_bc_8016FD84) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FD8Cu);
        goto block_8016FD8C;  /* not taken */
    }

block_8016FD8C:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x8016FD90: 0x00000000 */
    a0 = cpu->read_byte(v0 + 11);
    ra = 0x8016FDA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD9C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012708Cu);
    cpu->pc = 0x8012708Cu; return;  /* CPS jal -> 0x8012708C */

block_8016FDA0:
    ra = 0x8016FDA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FDA4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012748Cu);
    cpu->pc = 0x8012748Cu; return;  /* CPS jal -> 0x8012748C */

block_8016FDA8:
    ra = 0x8016FDB0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x801560F8u);
    cpu->pc = 0x801560F8u; return;  /* CPS jal -> 0x801560F8 */

block_8016FDB0:
    a0 = zero;
    a1 = a0;
    ra = 0x8016FDC0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a0;
    psx_check_interrupts_at(cpu, 0x801274FCu);
    cpu->pc = 0x801274FCu; return;  /* CPS jal -> 0x801274FC */

block_8016FDC0:
    a0 = s0;
    cpu->write_byte(s0 + 16, (uint8_t)zero);
    cpu->write_byte(s0 + 158, (uint8_t)zero);
    ra = 0x8016FDD4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x8015113Cu);
    cpu->pc = 0x8015113Cu; return;  /* CPS jal -> 0x8015113C */

block_8016FDD4:
    ra = 0x8016FDDCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8015A1E4u);
    cpu->pc = 0x8015A1E4u; return;  /* CPS jal -> 0x8015A1E4 */

block_8016FDDC:
    v1 = cpu->read_byte(s0 + 156);
    /* nop */  /* 0x8016FDE0: 0x00000000 */
    v0 = (v1 < (uint32_t)6) ? 1 : 0;  /* 0x8016FDE4: 0x2C620006 */
    int _bc_8016FDE8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 36, zero);
    if (_bc_8016FDE8) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FDF0u);
        goto block_8016FDF0;  /* not taken */
    }

block_8016FDF0:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 9736;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x8016FE04: 0x00000000 */
    uint32_t _jt_8016FE08 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE0C: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8016FE08);
    cpu->pc = _jt_8016FE08; return;  /* CPS: jr v0 */

block_8016FE10:
    v0 = cpu->read_word(s0 + 192);
    v1 = cpu->read_byte(s0 + 155);
    v0 = v0 + 44;
    int _bc_8016FE1C = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    if (_bc_8016FE1C) {
        psx_check_interrupts_at(cpu, 0x8016FE60u);
        goto block_8016FE60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE24u);
        goto block_8016FE24;  /* not taken */
    }

block_8016FE24:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x8016FE28: 0x00000000 */
    v1 = cpu->read_byte(v0 + 17);
    v0 = 1;
    int _bc_8016FE34 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE38: 0x00000000 */
    if (_bc_8016FE34) {
        psx_check_interrupts_at(cpu, 0x8016FE60u);
        goto block_8016FE60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE3Cu);
        goto block_8016FE3C;  /* not taken */
    }

block_8016FE3C:
    ra = 0x8016FE44u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE40: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014CDD0u);
    cpu->pc = 0x8014CDD0u; return;  /* CPS jal -> 0x8014CDD0 */

block_8016FE44:
    int _bc_8016FE44 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 1;
    if (_bc_8016FE44) {
        psx_check_interrupts_at(cpu, 0x8016FE58u);
        goto block_8016FE58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE4Cu);
        goto block_8016FE4C;  /* not taken */
    }

block_8016FE4C:
    t0 = 255;
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    t0 = 1;
    /* fall through to block_8016FE58 */

block_8016FE58:
    /* delay slot (always executes) */
    cpu->write_word(sp + 36, t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FE60:
    ra = 0x8016FE68u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FE68:
    int _bc_8016FE68 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FE68) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE70u);
        goto block_8016FE70;  /* not taken */
    }

block_8016FE70:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FE78:
    v0 = cpu->read_word(s0 + 192);
    v1 = cpu->read_byte(s0 + 155);
    v0 = v0 + 56;
    cpu->write_word(s0 + 196, v0);
    int _bc_8016FE88 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    if (_bc_8016FE88) {
        psx_check_interrupts_at(cpu, 0x8016FE94u);
        goto block_8016FE94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE90u);
        goto block_8016FE90;  /* not taken */
    }

block_8016FE90:
    a0 = 3;
    /* fall through to block_8016FE94 */

block_8016FE94:
    ra = 0x8016FE9Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE98: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FE9C:
    int _bc_8016FE9C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 11;
    if (_bc_8016FE9C) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FEA4u);
        goto block_8016FEA4;  /* not taken */
    }

block_8016FEA4:
    cpu->write_byte(s0, (uint8_t)v0);
    v0 = 4;
    t0 = 255;
    cpu->write_byte(s0 + 2, (uint8_t)v0);
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FEBC:
    v0 = cpu->read_word(s0 + 192);
    a0 = 1;
    v0 = v0 + 44;
    ra = 0x8016FED0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FED0:
    int _bc_8016FED0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FED0) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FED8u);
        goto block_8016FED8;  /* not taken */
    }

block_8016FED8:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FEE0:
    v0 = cpu->read_word(s0 + 192);
    a0 = 3;
    v0 = v0 + 44;
    ra = 0x8016FEF4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FEF4:
    int _bc_8016FEF4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FEF4) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FEFCu);
        goto block_8016FEFC;  /* not taken */
    }

block_8016FEFC:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FF04:
    a0 = 1027;
    a1 = zero;
    v0 = cpu->read_word(s0 + 192);
    a2 = a1;
    v0 = v0 + 44;
    ra = 0x8016FF20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8016FF20:
    ra = 0x8016FF28u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_8016FF28:
    v0 = cpu->read_word(s0 + 192);
    a0 = 0x8001 << 16;  /* 0x80010000 */
    a2 = cpu->read_word(v0 + 40);
    a1 = 1;
    a2 = (uint32_t)a2 >> (a1 & 0x1F);
    ra = 0x8016FF44u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a2 & a1;  /* 0x8016FF40: 0x00C53024 */
    psx_check_interrupts_at(cpu, 0x80159028u);
    cpu->pc = 0x80159028u; return;  /* CPS jal -> 0x80159028 */

block_8016FF44:
    a0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x8016FF48: 0x00000000 */
    v1 = cpu->read_word(a0 + 40);
    /* nop */  /* 0x8016FF50: 0x00000000 */
    v0 = (uint32_t)v1 >> 1;
    v0 = v0 & 0x0001;  /* 0x8016FF58: 0x30420001 */
    int _bc_8016FF5C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0002;
    if (_bc_8016FF5C) {
        psx_check_interrupts_at(cpu, 0x8016FF78u);
        goto block_8016FF78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FF64u);
        goto block_8016FF64;  /* not taken */
    }

block_8016FF64:
    cpu->write_word(a0 + 40, v0);
    v0 = 1;
    t0 = 0x801C << 16;  /* 0x801C0000 */
    t0 = t0 + 4576;
    cpu->write_word(t0 + 1040, v0);
    /* fall through to block_8016FF78 */

block_8016FF78:
    ra = 0x8016FF80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 2;
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FF80:
    int _bc_8016FF80 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FF80) {
        psx_check_interrupts_at(cpu, 0x8016FF90u);
        goto block_8016FF90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FF88u);
        goto block_8016FF88;  /* not taken */
    }

block_8016FF88:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FF90:
    ra = 0x8016FF98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8016DDF8u);
    cpu->pc = 0x8016DDF8u; return;  /* CPS jal -> 0x8016DDF8 */

block_8016FF98:
    ra = 0x8016FFA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8016E21Cu);
    cpu->pc = 0x8016E21Cu; return;  /* CPS jal -> 0x8016E21C */

block_8016FFA0:
    v0 = cpu->read_byte(s0 + 1);
    /* nop */  /* 0x8016FFA4: 0x00000000 */
    a0 = v0 & 0x00FF;  /* 0x8016FFA8: 0x304400FF */
    int _bc_8016FFAC = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 13;
    if (_bc_8016FFAC) {
        psx_check_interrupts_at(cpu, 0x8016FFD8u);
        goto block_8016FFD8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FFB4u);
        goto block_8016FFB4;  /* not taken */
    }

block_8016FFB4:
    v1 = cpu->read_byte(s0);
    /* nop */  /* 0x8016FFB8: 0x00000000 */
    int _bc_8016FFBC = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_8016FFBC) {
        psx_check_interrupts_at(cpu, 0x8016FFD0u);
        goto block_8016FFD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FFC4u);
        goto block_8016FFC4;  /* not taken */
    }

block_8016FFC4:
    t0 = 255;
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_8016FFD0:
    int _bc_8016FFD0 = (a0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FFD4: 0x00000000 */
    if (_bc_8016FFD0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FFD8u);
        goto block_8016FFD8;  /* not taken */
    }

block_8016FFD8:
    v0 = 2;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_8016FFE0 */

block_8016FFE0:
    a0 = 516;
    a1 = zero;
    ra = 0x8016FFF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8016FFF0:
    ra = 0x8016FFF8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FFF4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80136DDCu);
    cpu->pc = 0x80136DDCu; return;  /* CPS jal -> 0x80136DDC */

block_8016FFF8:
    a0 = zero;
    a1 = a0;
    ra = 0x80170008u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a0;
    psx_check_interrupts_at(cpu, 0x801274FCu);
    cpu->pc = 0x801274FCu; return;  /* CPS jal -> 0x801274FC */

block_80170008:
    a0 = 2;
    a1 = zero;
    ra = 0x80170018u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x8012CBC0u);
    cpu->pc = 0x8012CBC0u; return;  /* CPS jal -> 0x8012CBC0 */

block_80170018:
    a0 = s3;
    a1 = s2;
    a3 = cpu->read_word(s0 + 192);
    ra = 0x8017002Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x8015A69Cu);
    cpu->pc = 0x8015A69Cu; return;  /* CPS jal -> 0x8015A69C */

block_8017002C:
    a0 = s3;
    a1 = s2;
    ra = 0x8017003Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80151328u);
    cpu->pc = 0x80151328u; return;  /* CPS jal -> 0x80151328 */

block_8017003C:
    ra = 0x80170044u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170040: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8016FBFCu);
    cpu->pc = 0x8016FBFCu; return;  /* CPS jal -> 0x8016FBFC */

block_80170044:
    v0 = 3;
    cpu->write_byte(s0 + 158, (uint8_t)zero);
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_80170050 */

block_80170050:
    ra = 0x80170058u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170054: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_80170058:
    int _bc_80170058 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017005C: 0x00000000 */
    if (_bc_80170058) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170060u);
        goto block_80170060;  /* not taken */
    }

block_80170060:
    t0 = cpu->read_word(sp + 36);
    /* nop */  /* 0x80170064: 0x00000000 */
    int _bc_80170068 = (t0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3;
    if (_bc_80170068) {
        psx_check_interrupts_at(cpu, 0x80170088u);
        goto block_80170088;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170070u);
        goto block_80170070;  /* not taken */
    }

block_80170070:
    a0 = s0;
    a1 = s3;
    ra = 0x80170080u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s2;
    psx_check_interrupts_at(cpu, 0x801506D0u);
    cpu->pc = 0x801506D0u; return;  /* CPS jal -> 0x801506D0 */

block_80170080:
    /* delay slot (always executes) */
    v0 = 3;
    psx_check_interrupts_at(cpu, 0x8017008Cu);
    goto block_8017008C;  /* j */

block_80170088:
    cpu->write_word(sp + 36, zero);
    /* fall through to block_8017008C */

block_8017008C:
    cpu->write_byte(s0 + 158, (uint8_t)v0);
    v0 = 4;
    cpu->write_half(s0 + 136, (uint16_t)zero);
    cpu->write_half(s0 + 140, (uint16_t)zero);
    cpu->write_half(s0 + 142, (uint16_t)zero);
    cpu->write_half(s0 + 144, (uint16_t)zero);
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_801700AC:
    v0 = cpu->read_half(s0 + 144);
    /* nop */  /* 0x801700B0: 0x00000000 */
    int _bc_801700B4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    if (_bc_801700B4) {
        psx_check_interrupts_at(cpu, 0x801700C0u);
        goto block_801700C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801700BCu);
        goto block_801700BC;  /* not taken */
    }

block_801700BC:
    cpu->write_half(s0 + 144, (uint16_t)zero);
    /* fall through to block_801700C0 */

block_801700C0:
    a1 = s3;
    a2 = s2;
    ra = 0x801700D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = zero;
    psx_check_interrupts_at(cpu, 0x801709FCu);
    cpu->pc = 0x801709FCu; return;  /* CPS jal -> 0x801709FC */

block_801700D0:
    int _bc_801700D0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801700D4: 0x00000000 */
    if (_bc_801700D0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801700D8u);
        goto block_801700D8;  /* not taken */
    }

block_801700D8:
    v0 = cpu->read_half(s0 + 144);
    /* nop */  /* 0x801700DC: 0x00000000 */
    int _bc_801700E0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801700E4: 0x00000000 */
    if (_bc_801700E0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801700E8u);
        goto block_801700E8;  /* not taken */
    }

block_801700E8:
    a0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x801700EC: 0x00000000 */
    a0 = a0 << 24;
    ra = 0x801700FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x80155EDCu);
    cpu->pc = 0x80155EDCu; return;  /* CPS jal -> 0x80155EDC */

block_801700FC:
    ra = 0x80170104u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8015787Cu);
    cpu->pc = 0x8015787Cu; return;  /* CPS jal -> 0x8015787C */

block_80170104:
    a0 = cpu->read_word(s0 + 188);
    ra = 0x80170110u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x8012F024u);
    cpu->pc = 0x8012F024u; return;  /* CPS jal -> 0x8012F024 */

block_80170110:
    ra = 0x80170118u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x80136FB0u);
    cpu->pc = 0x80136FB0u; return;  /* CPS jal -> 0x80136FB0 */

block_80170118:
    ra = 0x80170120u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8012D090u);
    cpu->pc = 0x8012D090u; return;  /* CPS jal -> 0x8012D090 */

block_80170120:
    a0 = 2;
    a1 = zero;
    ra = 0x80170130u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x8012CBC0u);
    cpu->pc = 0x8012CBC0u; return;  /* CPS jal -> 0x8012CBC0 */

block_80170130:
    a0 = s0;
    ra = 0x8017013Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = -1;
    psx_check_interrupts_at(cpu, 0x8012AE3Cu);
    cpu->pc = 0x8012AE3Cu; return;  /* CPS jal -> 0x8012AE3C */

block_8017013C:
    v0 = 6;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_80170144 */

block_80170144:
    a0 = s3;
    a1 = s2;
    a3 = cpu->read_word(s0 + 192);
    ra = 0x80170158u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x8015A69Cu);
    cpu->pc = 0x8015A69Cu; return;  /* CPS jal -> 0x8015A69C */

block_80170158:
    a0 = s3;
    a1 = s2;
    ra = 0x80170168u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80151328u);
    cpu->pc = 0x80151328u; return;  /* CPS jal -> 0x80151328 */

block_80170168:
    ra = 0x80170170u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017016C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80129F78u);
    cpu->pc = 0x80129F78u; return;  /* CPS jal -> 0x80129F78 */

block_80170170:
    ra = 0x80170178u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = v0;
    psx_check_interrupts_at(cpu, 0x8012A030u);
    cpu->pc = 0x8012A030u; return;  /* CPS jal -> 0x8012A030 */

block_80170178:
    ra = 0x80170180u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x80127BE4u);
    cpu->pc = 0x80127BE4u; return;  /* CPS jal -> 0x80127BE4 */

block_80170180:
    ra = 0x80170188u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x80136FB0u);
    cpu->pc = 0x80136FB0u; return;  /* CPS jal -> 0x80136FB0 */

block_80170188:
    ra = 0x80170190u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x8012B49Cu);
    cpu->pc = 0x8012B49Cu; return;  /* CPS jal -> 0x8012B49C */

block_80170190:
    ra = 0x80170198u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 127;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 */

block_80170198:
    a0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x8017019C: 0x00000000 */
    a0 = a0 << 24;
    ra = 0x801701ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x80136D28u);
    cpu->pc = 0x80136D28u; return;  /* CPS jal -> 0x80136D28 */

block_801701AC:
    a0 = cpu->read_byte(s0 + 152);
    a1 = cpu->read_byte(s0 + 153);
    a0 = a0 << 24;
    a0 = (int32_t)a0 >> 24;
    a1 = a1 << 24;
    ra = 0x801701C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)a1 >> 24;
    psx_check_interrupts_at(cpu, 0x801358E8u);
    cpu->pc = 0x801358E8u; return;  /* CPS jal -> 0x801358E8 */

block_801701C8:
    a1 = s3;
    a0 = cpu->read_byte(s0 + 152);
    a2 = s2;
    a0 = a0 << 24;
    ra = 0x801701E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x8012E97Cu);
    cpu->pc = 0x8012E97Cu; return;  /* CPS jal -> 0x8012E97C */

block_801701E0:
    ra = 0x801701E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    psx_check_interrupts_at(cpu, 0x8016F2C4u);
    cpu->pc = 0x8016F2C4u; return;  /* CPS jal -> 0x8016F2C4 */

block_801701E8:
    ra = 0x801701F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    psx_check_interrupts_at(cpu, 0x8016F2C4u);
    cpu->pc = 0x8016F2C4u; return;  /* CPS jal -> 0x8016F2C4 */

block_801701F0:
    ra = 0x801701F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801701F4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142D58u);
    cpu->pc = 0x80142D58u; return;  /* CPS jal -> 0x80142D58 */

block_801701F8:
    a0 = s0;
    a2 = cpu->read_byte(s0 + 154);
    ra = 0x80170208u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s3;
    psx_check_interrupts_at(cpu, 0x80175FECu);
    cpu->pc = 0x80175FECu; return;  /* CPS jal -> 0x80175FEC */

block_80170208:
    a0 = s0;
    a2 = cpu->read_byte(s0 + 154);
    ra = 0x80170218u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s2;
    psx_check_interrupts_at(cpu, 0x80175FECu);
    cpu->pc = 0x80175FECu; return;  /* CPS jal -> 0x80175FEC */

block_80170218:
    a0 = cpu->read_byte(s0 + 153);
    /* nop */  /* 0x8017021C: 0x00000000 */
    a0 = a0 << 24;
    ra = 0x8017022Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x80172A34u);
    cpu->pc = 0x80172A34u; return;  /* CPS jal -> 0x80172A34 */

block_8017022C:
    ra = 0x80170234u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8016D478u);
    cpu->pc = 0x8016D478u; return;  /* CPS jal -> 0x8016D478 */

block_80170234:
    ra = 0x8017023Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170238: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801360CCu);
    cpu->pc = 0x801360CCu; return;  /* CPS jal -> 0x801360CC */

block_8017023C:
    ra = 0x80170244u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_80170244:
    v0 = 7;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_8017024C */

block_8017024C:
    ra = 0x80170254u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170250: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_80170254:
    int _bc_80170254 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (_bc_80170254) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017025Cu);
        goto block_8017025C;  /* not taken */
    }

block_8017025C:
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_80170260 */

block_80170260:
    v1 = cpu->read_byte(s0 + 155);
    v0 = 4;
    int _bc_80170268 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    if (_bc_80170268) {
        psx_check_interrupts_at(cpu, 0x80170284u);
        goto block_80170284;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170270u);
        goto block_80170270;  /* not taken */
    }

block_80170270:
    a1 = s2;
    ra = 0x8017027Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80154930u);
    cpu->pc = 0x80154930u; return;  /* CPS jal -> 0x80154930 */

block_8017027C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80170280: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80170290u);
    goto block_80170290;  /* j */

block_80170284:
    a1 = s2;
    ra = 0x80170290u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80155C14u);
    cpu->pc = 0x80155C14u; return;  /* CPS jal -> 0x80155C14 */

block_80170290:
    v0 = cpu->read_byte(s0 + 1);
    /* nop */  /* 0x80170294: 0x00000000 */
    v1 = v0 & 0x00FF;  /* 0x80170298: 0x304300FF */
    v0 = 6;
    int _bc_801702A0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (_bc_801702A0) {
        psx_check_interrupts_at(cpu, 0x801702C0u);
        goto block_801702C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702A8u);
        goto block_801702A8;  /* not taken */
    }

block_801702A8:
    int _bc_801702A8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801702AC: 0x00000000 */
    if (_bc_801702A8) {
        psx_check_interrupts_at(cpu, 0x801702C0u);
        goto block_801702C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702B0u);
        goto block_801702B0;  /* not taken */
    }

block_801702B0:
    ra = 0x801702B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012B23Cu);
    cpu->pc = 0x8012B23Cu; return;  /* CPS jal -> 0x8012B23C */

block_801702B8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801702BC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801709A0u);
    goto block_801709A0;  /* j */

block_801702C0:
    v0 = cpu->read_word(s0 + 196);
    /* nop */  /* 0x801702C4: 0x00000000 */
    v0 = cpu->read_word(v0 + 8);
    /* nop */  /* 0x801702CC: 0x00000000 */
    v0 = (uint32_t)v0 >> 11;
    v0 = v0 & 0x0001;  /* 0x801702D4: 0x30420001 */
    int _bc_801702D8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_801702D8) {
        psx_check_interrupts_at(cpu, 0x801702E4u);
        goto block_801702E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702E0u);
        goto block_801702E0;  /* not taken */
    }

block_801702E0:
    cpu->write_byte(s0 + 158, (uint8_t)v0);
    /* fall through to block_801702E4 */

block_801702E4:
    v0 = cpu->read_half(s0 + 130);
    /* nop */  /* 0x801702E8: 0x00000000 */
    int _bc_801702EC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801702F0: 0x00000000 */
    if (_bc_801702EC) {
        psx_check_interrupts_at(cpu, 0x80170334u);
        goto block_80170334;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702F4u);
        goto block_801702F4;  /* not taken */
    }

block_801702F4:
    ra = 0x801702FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x80128898u);
    cpu->pc = 0x80128898u; return;  /* CPS jal -> 0x80128898 */

block_801702FC:
    a0 = s3;
    a1 = s2;
    ra = 0x8017030Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x8017F398u);
    cpu->pc = 0x8017F398u; return;  /* CPS jal -> 0x8017F398 */

block_8017030C:
    a0 = s3;
    ra = 0x80170318u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x801787DCu);
    cpu->pc = 0x801787DCu; return;  /* CPS jal -> 0x801787DC */

block_80170318:
    a0 = s2;
    ra = 0x80170324u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x801787DCu);
    cpu->pc = 0x801787DCu; return;  /* CPS jal -> 0x801787DC */

block_80170324:
    a0 = s3;
    a1 = s2;
    ra = 0x80170334u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x801809C8u);
    cpu->pc = 0x801809C8u; return;  /* CPS jal -> 0x801809C8 */

block_80170334:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170338: 0x00000000 */
    v0 = (uint32_t)v0 >> 2;
    v0 = v0 & 0x0001;  /* 0x80170340: 0x30420001 */
    int _bc_80170344 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s3;
    if (_bc_80170344) {
        psx_check_interrupts_at(cpu, 0x80170384u);
        goto block_80170384;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017034Cu);
        goto block_8017034C;  /* not taken */
    }

block_8017034C:
    v0 = cpu->read_byte(s0 + 176);
    /* nop */  /* 0x80170350: 0x00000000 */
    int _bc_80170354 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = s2;
    if (_bc_80170354) {
        psx_check_interrupts_at(cpu, 0x80170388u);
        goto block_80170388;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017035Cu);
        goto block_8017035C;  /* not taken */
    }

block_8017035C:
    v0 = cpu->read_byte(s0 + 157);
    /* nop */  /* 0x80170360: 0x00000000 */
    int _bc_80170364 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 16;
    if (_bc_80170364) {
        psx_check_interrupts_at(cpu, 0x80170370u);
        goto block_80170370;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017036Cu);
        goto block_8017036C;  /* not taken */
    }

block_8017036C:
    a0 = s5 + 8248;
    /* fall through to block_80170370 */

block_80170370:
    a1 = s0;
    a2 = s3;
    ra = 0x80170380u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x801737ECu);
    cpu->pc = 0x801737ECu; return;  /* CPS jal -> 0x801737EC */

block_80170380:
    a1 = s3;
    /* fall through to block_80170384 */

block_80170384:
    a2 = s2;
    /* fall through to block_80170388 */

block_80170388:
    a0 = cpu->read_word(s0 + 188);
    ra = 0x80170394u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x8012FB10u);
    cpu->pc = 0x8012FB10u; return;  /* CPS jal -> 0x8012FB10 */

block_80170394:
    a0 = cpu->read_word(s0 + 188);
    ra = 0x801703A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x8012EE48u);
    cpu->pc = 0x8012EE48u; return;  /* CPS jal -> 0x8012EE48 */

block_801703A0:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x801703A4: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x801703A8: 0x30420001 */
    int _bc_801703AC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801703B0: 0x00000000 */
    if (_bc_801703AC) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703B4u);
        goto block_801703B4;  /* not taken */
    }

block_801703B4:
    v0 = cpu->read_word(s3 + 96);
    /* nop */  /* 0x801703B8: 0x00000000 */
    v0 = v0 & 0x0002;  /* 0x801703BC: 0x30420002 */
    int _bc_801703C0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_801703C0) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703C8u);
        goto block_801703C8;  /* not taken */
    }

block_801703C8:
    a1 = s4;
    a2 = s0;
    ra = 0x801703D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s3;
    psx_check_interrupts_at(cpu, 0x8016CD8Cu);
    cpu->pc = 0x8016CD8Cu; return;  /* CPS jal -> 0x8016CD8C */

block_801703D8:
    v1 = cpu->read_word(s3 + 108);
    /* nop */  /* 0x801703DC: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x801703E0: 0x30620400 */
    int _bc_801703E4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x0200;  /* 0x801703E8: 0x30620200 */
    if (_bc_801703E4) {
        psx_check_interrupts_at(cpu, 0x801704BCu);
        goto block_801704BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703ECu);
        goto block_801703EC;  /* not taken */
    }

block_801703EC:
    int _bc_801703EC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    if (_bc_801703EC) {
        psx_check_interrupts_at(cpu, 0x80170448u);
        goto block_80170448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703F4u);
        goto block_801703F4;  /* not taken */
    }

block_801703F4:
    ra = 0x801703FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_801703FC:
    v0 = cpu->read_word(s3 + 40);
    s1 = s5 + 64;
    a3 = (int32_t)(int8_t)cpu->read_byte(v0);
    a1 = s1;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 16);
    ra = 0x80170420u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_80170420:
    int _bc_80170420 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    if (_bc_80170420) {
        psx_check_interrupts_at(cpu, 0x801704BCu);
        goto block_801704BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170428u);
        goto block_80170428;  /* not taken */
    }

block_80170428:
    v0 = cpu->read_word(s3 + 40);
    /* nop */  /* 0x8017042C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801704B4u);
    goto block_801704B4;  /* j */

block_80170448:
    v0 = 0x0800 << 16;  /* 0x08000000 */
    v0 = v1 & v0;  /* 0x8017044C: 0x00621024 */
    int _bc_80170450 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170454: 0x00000000 */
    if (_bc_80170450) {
        psx_check_interrupts_at(cpu, 0x8017048Cu);
        goto block_8017048C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170458u);
        goto block_80170458;  /* not taken */
    }

block_80170458:
    int _bc_80170458 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017045C: 0x00000000 */
    if (_bc_80170458) {
        psx_check_interrupts_at(cpu, 0x8017048Cu);
        goto block_8017048C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170460u);
        goto block_80170460;  /* not taken */
    }

block_80170460:
    ra = 0x80170468u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170468:
    v0 = cpu->read_word(s3 + 40);
    /* nop */  /* 0x8017046C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    a1 = s5 + 64;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801704B4u);
    goto block_801704B4;  /* j */

block_8017048C:
    ra = 0x80170494u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170494:
    a1 = s5 + 64;
    a0 = cpu->read_word(s3 + 24);
    a3 = cpu->read_byte(s3 + 6);
    a2 = s4;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a3 = a3 << 24;
    a3 = (int32_t)a3 >> 24;
    /* fall through to block_801704B4 */

block_801704B4:
    ra = 0x801704BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801704B8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_801704BC:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x801704C0: 0x00000000 */
    v0 = (uint32_t)v0 >> 6;
    v0 = v0 & 0x0001;  /* 0x801704C8: 0x30420001 */
    int _bc_801704CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801704D0: 0x00000000 */
    if (_bc_801704CC) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801704D4u);
        goto block_801704D4;  /* not taken */
    }

block_801704D4:
    v0 = cpu->read_byte(s0 + 159);
    /* nop */  /* 0x801704D8: 0x00000000 */
    int _bc_801704DC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801704E0: 0x00000000 */
    if (_bc_801704DC) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801704E4u);
        goto block_801704E4;  /* not taken */
    }

block_801704E4:
    v0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x801704E8: 0x00000000 */
    v0 = v0 << 24;
    v1 = (int32_t)v0 >> 24;
    v0 = 4;
    int _bc_801704F8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 5) ? 1 : 0;  /* 0x801704FC: 0x28620005 */
    if (_bc_801704F8) {
        psx_check_interrupts_at(cpu, 0x80170524u);
        goto block_80170524;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170500u);
        goto block_80170500;  /* not taken */
    }

block_80170500:
    int _bc_80170500 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_80170500) {
        psx_check_interrupts_at(cpu, 0x80170518u);
        goto block_80170518;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170508u);
        goto block_80170508;  /* not taken */
    }

block_80170508:
    int _bc_80170508 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_80170508) {
        psx_check_interrupts_at(cpu, 0x80170524u);
        goto block_80170524;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170510u);
        goto block_80170510;  /* not taken */
    }

block_80170510:
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x80170550u);
    goto block_80170550;  /* j */

block_80170518:
    v0 = 6;
    int _bc_8017051C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_8017051C) {
        psx_check_interrupts_at(cpu, 0x8017054Cu);
        goto block_8017054C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170524u);
        goto block_80170524;  /* not taken */
    }

block_80170524:
    a1 = s5 + 8256;
    a2 = s4;
    a0 = cpu->read_word(s3 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    ra = 0x80170544u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, t0);
    psx_check_interrupts_at(cpu, 0x8018B100u);
    cpu->pc = 0x8018B100u; return;  /* CPS jal -> 0x8018B100 */

block_80170544:
    /* delay slot (always executes) */
    /* nop */  /* 0x80170548: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017058Cu);
    goto block_8017058C;  /* j */

block_8017054C:
    a2 = s4;
    /* fall through to block_80170550 */

block_80170550:
    a0 = cpu->read_word(s3 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, t0);
    v0 = cpu->read_byte(s0 + 152);
    v1 = s5 + 8300;
    cpu->write_word(sp + 28, v1);
    v1 = 0x801B << 16;  /* 0x801B0000 */
    v1 = v1 + 5812;
    v0 = v0 << 24;
    v0 = (int32_t)v0 >> 21;
    v0 = v0 + v1;
    ra = 0x8017058Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, v0);
    psx_check_interrupts_at(cpu, 0x8018B208u);
    cpu->pc = 0x8018B208u; return;  /* CPS jal -> 0x8018B208 */

block_8017058C:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170590: 0x00000000 */
    v0 = (uint32_t)v0 >> 1;
    v0 = v0 & 0x0001;  /* 0x80170598: 0x30420001 */
    int _bc_8017059C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801705A0: 0x00000000 */
    if (_bc_8017059C) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705A4u);
        goto block_801705A4;  /* not taken */
    }

block_801705A4:
    v0 = cpu->read_word(s2 + 96);
    /* nop */  /* 0x801705A8: 0x00000000 */
    v0 = v0 & 0x0002;  /* 0x801705AC: 0x30420002 */
    int _bc_801705B0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_801705B0) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705B8u);
        goto block_801705B8;  /* not taken */
    }

block_801705B8:
    a1 = s4;
    a2 = s0;
    ra = 0x801705C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x8016CD8Cu);
    cpu->pc = 0x8016CD8Cu; return;  /* CPS jal -> 0x8016CD8C */

block_801705C8:
    v1 = cpu->read_word(s2 + 108);
    /* nop */  /* 0x801705CC: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x801705D0: 0x30620400 */
    int _bc_801705D4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x0200;  /* 0x801705D8: 0x30620200 */
    if (_bc_801705D4) {
        psx_check_interrupts_at(cpu, 0x801706ACu);
        goto block_801706AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705DCu);
        goto block_801705DC;  /* not taken */
    }

block_801705DC:
    int _bc_801705DC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    if (_bc_801705DC) {
        psx_check_interrupts_at(cpu, 0x80170638u);
        goto block_80170638;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705E4u);
        goto block_801705E4;  /* not taken */
    }

block_801705E4:
    ra = 0x801705ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_801705EC:
    v0 = cpu->read_word(s2 + 40);
    s1 = s5 + 64;
    a3 = (int32_t)(int8_t)cpu->read_byte(v0);
    a1 = s1;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 16);
    ra = 0x80170610u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_80170610:
    int _bc_80170610 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    if (_bc_80170610) {
        psx_check_interrupts_at(cpu, 0x801706ACu);
        goto block_801706AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170618u);
        goto block_80170618;  /* not taken */
    }

block_80170618:
    v0 = cpu->read_word(s2 + 40);
    /* nop */  /* 0x8017061C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801706A4u);
    goto block_801706A4;  /* j */

block_80170638:
    v0 = 0x0800 << 16;  /* 0x08000000 */
    v0 = v1 & v0;  /* 0x8017063C: 0x00621024 */
    int _bc_80170640 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170644: 0x00000000 */
    if (_bc_80170640) {
        psx_check_interrupts_at(cpu, 0x8017067Cu);
        goto block_8017067C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170648u);
        goto block_80170648;  /* not taken */
    }

block_80170648:
    int _bc_80170648 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017064C: 0x00000000 */
    if (_bc_80170648) {
        psx_check_interrupts_at(cpu, 0x8017067Cu);
        goto block_8017067C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170650u);
        goto block_80170650;  /* not taken */
    }

block_80170650:
    ra = 0x80170658u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170658:
    v0 = cpu->read_word(s2 + 40);
    /* nop */  /* 0x8017065C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    a1 = s5 + 64;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801706A4u);
    goto block_801706A4;  /* j */

block_8017067C:
    ra = 0x80170684u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170684:
    a1 = s5 + 64;
    a0 = cpu->read_word(s2 + 24);
    a3 = cpu->read_byte(s2 + 6);
    a2 = s4;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a3 = a3 << 24;
    a3 = (int32_t)a3 >> 24;
    /* fall through to block_801706A4 */

block_801706A4:
    ra = 0x801706ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801706A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_801706AC:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x801706B0: 0x00000000 */
    v0 = (uint32_t)v0 >> 6;
    v0 = v0 & 0x0001;  /* 0x801706B8: 0x30420001 */
    int _bc_801706BC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801706C0: 0x00000000 */
    if (_bc_801706BC) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706C4u);
        goto block_801706C4;  /* not taken */
    }

block_801706C4:
    v0 = cpu->read_byte(s0 + 159);
    /* nop */  /* 0x801706C8: 0x00000000 */
    int _bc_801706CC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801706D0: 0x00000000 */
    if (_bc_801706CC) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706D4u);
        goto block_801706D4;  /* not taken */
    }

block_801706D4:
    v0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x801706D8: 0x00000000 */
    v0 = v0 << 24;
    v1 = (int32_t)v0 >> 24;
    v0 = 4;
    int _bc_801706E8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 5) ? 1 : 0;  /* 0x801706EC: 0x28620005 */
    if (_bc_801706E8) {
        psx_check_interrupts_at(cpu, 0x80170714u);
        goto block_80170714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706F0u);
        goto block_801706F0;  /* not taken */
    }

block_801706F0:
    int _bc_801706F0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_801706F0) {
        psx_check_interrupts_at(cpu, 0x80170708u);
        goto block_80170708;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706F8u);
        goto block_801706F8;  /* not taken */
    }

block_801706F8:
    int _bc_801706F8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_801706F8) {
        psx_check_interrupts_at(cpu, 0x80170714u);
        goto block_80170714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170700u);
        goto block_80170700;  /* not taken */
    }

block_80170700:
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x80170740u);
    goto block_80170740;  /* j */

block_80170708:
    v0 = 6;
    int _bc_8017070C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_8017070C) {
        psx_check_interrupts_at(cpu, 0x8017073Cu);
        goto block_8017073C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170714u);
        goto block_80170714;  /* not taken */
    }

block_80170714:
    a1 = s5 + 8256;
    a2 = s4;
    a0 = cpu->read_word(s2 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    ra = 0x80170734u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, t0);
    psx_check_interrupts_at(cpu, 0x8018B100u);
    cpu->pc = 0x8018B100u; return;  /* CPS jal -> 0x8018B100 */

block_80170734:
    /* delay slot (always executes) */
    /* nop */  /* 0x80170738: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017077Cu);
    goto block_8017077C;  /* j */

block_8017073C:
    a2 = s4;
    /* fall through to block_80170740 */

block_80170740:
    a0 = cpu->read_word(s2 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, t0);
    v0 = cpu->read_byte(s0 + 152);
    v1 = s5 + 8300;
    cpu->write_word(sp + 28, v1);
    v1 = 0x801B << 16;  /* 0x801B0000 */
    v1 = v1 + 5812;
    v0 = v0 << 24;
    v0 = (int32_t)v0 >> 21;
    v0 = v0 + v1;
    ra = 0x8017077Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, v0);
    psx_check_interrupts_at(cpu, 0x8018B208u);
    cpu->pc = 0x8018B208u; return;  /* CPS jal -> 0x8018B208 */

block_8017077C:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170780: 0x00000000 */
    v0 = (uint32_t)v0 >> 4;
    v0 = v0 & 0x0001;  /* 0x80170788: 0x30420001 */
    int _bc_8017078C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s6;
    if (_bc_8017078C) {
        psx_check_interrupts_at(cpu, 0x8017079Cu);
        goto block_8017079C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170794u);
        goto block_80170794;  /* not taken */
    }

block_80170794:
    v0 = 1;
    cpu->write_byte(s0 + 177, (uint8_t)v0);
    /* fall through to block_8017079C */

block_8017079C:
    a1 = s5;
    a2 = s4;
    ra = 0x801707ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x80137098u);
    cpu->pc = 0x80137098u; return;  /* CPS jal -> 0x80137098 */

block_801707AC:
    v0 = cpu->read_word(s3 + 108);
    /* nop */  /* 0x801707B0: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801707B4: 0x30428000 */
    int _bc_801707B8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_801707B8) {
        psx_check_interrupts_at(cpu, 0x801707D4u);
        goto block_801707D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801707C0u);
        goto block_801707C0;  /* not taken */
    }

block_801707C0:
    v0 = cpu->read_word(s2 + 108);
    /* nop */  /* 0x801707C4: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801707C8: 0x30428000 */
    int _bc_801707CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = -8;
    if (_bc_801707CC) {
        psx_check_interrupts_at(cpu, 0x801707E0u);
        goto block_801707E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801707D4u);
        goto block_801707D4;  /* not taken */
    }

block_801707D4:
    t0 = t0 + 19632;
    /* delay slot (always executes) */
    v0 = 16;
    psx_check_interrupts_at(cpu, 0x801707E4u);
    goto block_801707E4;  /* j */

block_801707E0:
    t0 = t0 + 19632;
    /* fall through to block_801707E4 */

block_801707E4:
    cpu->write_half(t0 + 2, (uint16_t)zero);
    cpu->write_half(t0 + 6, (uint16_t)v0);
    a0 = s5;
    a1 = s4;
    a3 = cpu->read_byte(s0 + 152);
    a2 = s0;
    a3 = a3 << 24;
    ra = 0x80170808u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)a3 >> 24;
    psx_check_interrupts_at(cpu, 0x8016D244u);
    cpu->pc = 0x8016D244u; return;  /* CPS jal -> 0x8016D244 */

block_80170808:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x8017080C: 0x00000000 */
    v0 = (uint32_t)v0 >> 3;
    v0 = v0 & 0x0001;  /* 0x80170814: 0x30420001 */
    int _bc_80170818 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s6;
    if (_bc_80170818) {
        psx_check_interrupts_at(cpu, 0x80170830u);
        goto block_80170830;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170820u);
        goto block_80170820;  /* not taken */
    }

block_80170820:
    a1 = s7;
    a2 = cpu->read_word(s0 + 188);
    ra = 0x80170830u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x80135180u);
    cpu->pc = 0x80135180u; return;  /* CPS jal -> 0x80135180 */

block_80170830:
    v0 = cpu->read_byte(s0 + 159);
    /* nop */  /* 0x80170834: 0x00000000 */
    int _bc_80170838 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017083C: 0x00000000 */
    if (_bc_80170838) {
        psx_check_interrupts_at(cpu, 0x80170878u);
        goto block_80170878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170840u);
        goto block_80170840;  /* not taken */
    }

block_80170840:
    v0 = cpu->read_byte(s0 + 158);
    /* nop */  /* 0x80170844: 0x00000000 */
    int _bc_80170848 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017084C: 0x00000000 */
    if (_bc_80170848) {
        psx_check_interrupts_at(cpu, 0x80170878u);
        goto block_80170878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170850u);
        goto block_80170850;  /* not taken */
    }

block_80170850:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170854: 0x00000000 */
    v0 = (uint32_t)v0 >> 5;
    v0 = v0 & 0x0001;  /* 0x8017085C: 0x30420001 */
    int _bc_80170860 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 8316;
    if (_bc_80170860) {
        psx_check_interrupts_at(cpu, 0x80170878u);
        goto block_80170878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170868u);
        goto block_80170868;  /* not taken */
    }

block_80170868:
    a1 = cpu->read_word(s0 + 188);
    a2 = s4;
    ra = 0x80170878u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x80136B64u);
    cpu->pc = 0x80136B64u; return;  /* CPS jal -> 0x80136B64 */

block_80170878:
    v0 = (int32_t)(int16_t)cpu->read_half(fp + 8);
    /* nop */  /* 0x8017087C: 0x00000000 */
    int _bc_80170880 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_80170880) {
        psx_check_interrupts_at(cpu, 0x8017089Cu);
        goto block_8017089C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170888u);
        goto block_80170888;  /* not taken */
    }

block_80170888:
    cpu->write_word(sp + 16, v0);
    a1 = s4;
    a2 = s6;
    ra = 0x8017089Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s3;
    psx_check_interrupts_at(cpu, 0x801468ECu);
    cpu->pc = 0x801468ECu; return;  /* CPS jal -> 0x801468EC */

block_8017089C:
    v0 = (int32_t)(int16_t)cpu->read_half(fp + 10);
    /* nop */  /* 0x801708A0: 0x00000000 */
    int _bc_801708A4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_801708A4) {
        psx_check_interrupts_at(cpu, 0x801708C0u);
        goto block_801708C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801708ACu);
        goto block_801708AC;  /* not taken */
    }

block_801708AC:
    cpu->write_word(sp + 16, v0);
    a1 = s4;
    a2 = s6;
    ra = 0x801708C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x801468ECu);
    cpu->pc = 0x801468ECu; return;  /* CPS jal -> 0x801468EC */

block_801708C0:
    a1 = (int32_t)(int16_t)cpu->read_half(fp + 4);
    /* nop */  /* 0x801708C4: 0x00000000 */
    int _bc_801708C8 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801708CC: 0x00000000 */
    if (_bc_801708C8) {
        psx_check_interrupts_at(cpu, 0x801708D8u);
        goto block_801708D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801708D0u);
        goto block_801708D0;  /* not taken */
    }

block_801708D0:
    ra = 0x801708D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    psx_check_interrupts_at(cpu, 0x8016F588u);
    cpu->pc = 0x8016F588u; return;  /* CPS jal -> 0x8016F588 */

block_801708D8:
    a1 = (int32_t)(int16_t)cpu->read_half(fp + 6);
    /* nop */  /* 0x801708DC: 0x00000000 */
    int _bc_801708E0 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801708E4: 0x00000000 */
    if (_bc_801708E0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801708E8u);
        goto block_801708E8;  /* not taken */
    }

block_801708E8:
    ra = 0x801708F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    psx_check_interrupts_at(cpu, 0x8016F588u);
    cpu->pc = 0x8016F588u; return;  /* CPS jal -> 0x8016F588 */

block_801708F0:
    /* delay slot (always executes) */
    /* nop */  /* 0x801708F4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_801708F8:
    ra = 0x80170900u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012B23Cu);
    cpu->pc = 0x8012B23Cu; return;  /* CPS jal -> 0x8012B23C */

block_80170900:
    ra = 0x80170908u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170904: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80136DDCu);
    cpu->pc = 0x80136DDCu; return;  /* CPS jal -> 0x80136DDC */

block_80170908:
    ra = 0x80170910u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x801560F8u);
    cpu->pc = 0x801560F8u; return;  /* CPS jal -> 0x801560F8 */

block_80170910:
    ra = 0x80170918u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170914: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80155E30u);
    cpu->pc = 0x80155E30u; return;  /* CPS jal -> 0x80155E30 */

block_80170918:
    a0 = 516;
    a1 = zero;
    ra = 0x80170928u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_80170928:
    a0 = 1;
    ra = 0x80170934u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 8;
    psx_check_interrupts_at(cpu, 0x8012AA6Cu);
    cpu->pc = 0x8012AA6Cu; return;  /* CPS jal -> 0x8012AA6C */

block_80170934:
    a0 = 1;
    cpu->write_byte(s0 + 16, (uint8_t)zero);
    ra = 0x80170944u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 158, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80136FB0u);
    cpu->pc = 0x80136FB0u; return;  /* CPS jal -> 0x80136FB0 */

block_80170944:
    a0 = zero;
    a1 = a0;
    ra = 0x80170954u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a0;
    psx_check_interrupts_at(cpu, 0x801274FCu);
    cpu->pc = 0x801274FCu; return;  /* CPS jal -> 0x801274FC */

block_80170954:
    v0 = 12;
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_80170960:
    ra = 0x80170968u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x8016E21Cu);
    cpu->pc = 0x8016E21Cu; return;  /* CPS jal -> 0x8016E21C */

block_80170968:
    v1 = cpu->read_byte(s0 + 1);
    v0 = 11;
    int _bc_80170970 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;
    if (_bc_80170970) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170978u);
        goto block_80170978;  /* not taken */
    }

block_80170978:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_80170980:
    ra = 0x80170988u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170984: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_80170988:
    int _bc_80170988 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_80170988) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170990u);
        goto block_80170990;  /* not taken */
    }

block_80170990:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x801709A0u);
    goto block_801709A0;  /* j */

block_80170998:
    ra = 0x801709A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_801709A0:
    v1 = cpu->read_byte(s0 + 1);
    v0 = 255;
    int _bc_801709A8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801709AC: 0x00000000 */
    if (_bc_801709A8) {
        psx_check_interrupts_at(cpu, 0x8016FD38u);
        goto block_8016FD38;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801709B0u);
        goto block_801709B0;  /* not taken */
    }

block_801709B0:
    v1 = cpu->read_byte(s0);
    v0 = 9;
    int _bc_801709B8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801709BC: 0x00000000 */
    if (_bc_801709B8) {
        psx_check_interrupts_at(cpu, 0x801709C8u);
        goto block_801709C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801709C0u);
        goto block_801709C0;  /* not taken */
    }

block_801709C0:
    ra = 0x801709C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x801525C4u);
    cpu->pc = 0x801525C4u; return;  /* CPS jal -> 0x801525C4 */

block_801709C8:
    cpu->write_byte(s0 + 1, (uint8_t)zero);
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
    uint32_t _jt_801709F4 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 80;
    psx_check_interrupts_at(cpu, _jt_801709F4);
    cpu->pc = _jt_801709F4; return;  /* CPS: jr $ra */}
