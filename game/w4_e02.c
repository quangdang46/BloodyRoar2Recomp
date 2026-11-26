/* w4_e02.c — clean CPS ports (worktree orch/w4/e02).
 *
 * Six leftover guest entries not yet routed through clean_table:
 *
 *   0x80188200  prim_poly_emit_54a    — mode-0 heap-serialize poly emit,
 *                                      vertex stride 0x54 (tag 0x0100000a;
 *                                      mode-1 twin prim_poly_emit_54b).
 *                                      Per-vertex NCLIP/CC/AVSZ3 GTE loop.
 *   0x80188308  prim_poly_emit_5ca    — same family, stride 0x5c, reads
 *                                      a0+28/+32 (UV pair) — textured
 *                                      variant (tag 0x0100000e; twin
 *                                      prim_poly_emit_5c).
 *   0x80188434  prim_point_emit_48a   — stride 0x48 point/degenerate-poly
 *                                      emit (tag 0x01000010; twin
 *                                      prim_point_emit_48).
 *   0x80194470  gte_screen_init_defer_mfhi — 2-insn alt entry: mfhi $t6;
 *                                      nop; falls through into
 *                                      gte_screen_init_defer (0x80194478).
 *   0x80194614  fixed_point_ret_neg1  — shared error tail: li $v0,-1;
 *                                      jr $ra (reached from
 *                                      fixed_rsqrt_q12_ex LZCR guards).
 *   0x80194940  fixed_recip_neg_exp   — negative-exponent tail of
 *                                      fixed_recip_lut: $t1=-$t1;
 *                                      $v0=$t5>>($t1&31); jr $ra.
 *
 * Bodies are faithful op-by-op CPS transliterations (same transform as
 * game/cps_clean.c / gen_cps_clean.py): identical guest ops/order/yield
 * points, cycle/icache/cosim/debug instrumentation dropped, MMIO-aware
 * cpu->read_x/cpu->write_x accessors. Dispatch addrs are registered in
 * game/clean_cps.map; renames.map carries the addr->name authority.
 *
 * NOT in this file: 0x80196370 (cd_response_process) and 0x80199440
 * (cd_mc_state_machine) are CD-ROM functions already delegated to
 * generated per AGENTS.md §4 via call_guest4 wrappers in game/cd.c —
 * their CPS continuation PCs must stay on the generated resume path,
 * so they get no clean_cps.map entries here.
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
extern void     psx_muldiv_stall(CPUState *cpu);
extern void     psx_gte_stall(CPUState *cpu);
extern void     psx_gte_read(CPUState *cpu, uint32_t rt);
extern void     gte_execute(CPUState *cpu, uint32_t cmd);
extern uint32_t gte_read_data(CPUState *cpu, uint8_t reg);
extern void     gte_write_data(CPUState *cpu, uint8_t reg, uint32_t val);
extern void     gte_precision_store_word(uint32_t addr, uint8_t reg);

#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability),
 * same convention as game/cps_clean.c. */
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


/* ======== 0x80188200 ======== */
void prim_poly_emit_54a(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80188208u: goto block_80188208;
            case 0x80188210u: goto block_80188210;
            case 0x80188214u: goto block_80188214;
            case 0x80188260u: goto block_80188260;
            case 0x801882F8u: goto block_801882F8;
            case 0x80188300u: goto block_80188300;
            default: break;
        }
    }
    /* Address: 0x80188200, Size: 264 bytes, Blocks: 7 */

block_80188200:
    int _bc_80188200 = ((int32_t)a3 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80188204: 0x00000000 */
    if (_bc_80188200) {
        psx_check_interrupts_at(cpu, 0x80188300u);
        goto block_80188300;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188208u);
        goto block_80188208;  /* not taken */
    }

block_80188208:
    int _bc_80188208 = (a2 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t4 = 28;
    if (_bc_80188208) {
        psx_check_interrupts_at(cpu, 0x80188214u);
        goto block_80188214;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188210u);
        goto block_80188210;  /* not taken */
    }

block_80188210:
    t4 = t4 + 28;
    /* fall through to block_80188214 */

block_80188214:
    t6 = cpu->read_word(a0);
    t7 = cpu->read_word(a0 + 4);
    t8 = cpu->read_word(a0 + 8);
    t0 = cpu->read_word(t6);
    t1 = cpu->read_word(t7);
    t2 = cpu->read_word(t8);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 12, t0);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 13, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 14, t2);  /* mtc2 */
    a3 = a3 + -1;
    t3 = a0 + t4;
    gte_execute(cpu, 0x1400006);  /* gte cmd 0x06 */  /* 0x80188240: 0x4B400006 */
    cpu->write_word(t3 + 8, t0);
    cpu->write_word(t3 + 16, t1);
    cpu->write_word(t3 + 24, t2);
    
    psx_gte_read(cpu, 13);
    t5 = cpu->gte_data[24];  /* mfc2 */
    /* nop */  /* 0x80188254: 0x00000000 */
    int _bc_80188258 = ((int32_t)t5 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018825C: 0x00000000 */
    if (_bc_80188258) {
        psx_check_interrupts_at(cpu, 0x801882F8u);
        goto block_801882F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188260u);
        goto block_80188260;  /* not taken */
    }

block_80188260:
    t0 = cpu->read_word(a0 + 24);
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (cpu->read_word((a0 + 12)));
    t1 = cpu->read_word(t0);
    t2 = cpu->read_half(t0 + 4);
    t0 = t1 << 16;
    t0 = (uint32_t)t0 >> 16;
    t1 = (uint32_t)t1 >> 16;
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, t0);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, t2);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 17, (cpu->read_word((t6 + 4))));
    gte_execute(cpu, 0x138041C);  /* gte cmd 0x1C */  /* 0x8018828C: 0x4B38041C */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 18, (cpu->read_word((t7 + 4))));
    
    psx_gte_stall(cpu);
    cpu->write_word((t3 + 4), (cpu->gte_data[22]));
    gte_precision_store_word((t3 + 4), 22);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, t0);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, t2);  /* mtc2 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (cpu->read_word((a0 + 16)));
    /* nop */  /* 0x801882A8: 0x00000000 */
    gte_execute(cpu, 0x138041C);  /* gte cmd 0x1C */  /* 0x801882AC: 0x4B38041C */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 19, (cpu->read_word((t8 + 4))));
    /* nop */  /* 0x801882B4: 0x00000000 */
    gte_execute(cpu, 0x158002D);  /* gte cmd 0x2D */  /* 0x801882B8: 0x4B58002D */
    
    psx_gte_stall(cpu);
    cpu->write_word((t3 + 12), (cpu->gte_data[22]));
    gte_precision_store_word((t3 + 12), 22);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, t0);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, t2);  /* mtc2 */
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (cpu->read_word((a0 + 20)));
    
    psx_gte_read(cpu, 8);
    t0 = gte_read_data(cpu, 7);  /* mfc2 */
    gte_execute(cpu, 0x138041C);  /* gte cmd 0x1C */  /* 0x801882D4: 0x4B38041C */
    v0 = t0 & 0x1FFC;  /* 0x801882D8: 0x31021FFC */
    v0 = a1 + v0;
    t0 = cpu->read_word(v0);
    t1 = t3 << 8;
    t0 = t0 << 8;
    
    psx_gte_stall(cpu);
    cpu->write_word((t3 + 20), (cpu->gte_data[22]));
    gte_precision_store_word((t3 + 20), 22);
    /* fall through to block_801882F8 */

block_801882F8:
    int _bc_801882F8 = ((int32_t)a3 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 84;
    if (_bc_801882F8) {
        psx_check_interrupts_at(cpu, 0x80188214u);
        goto block_80188214;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188300u);
        goto block_80188300;  /* not taken */
    }

block_80188300:
    uint32_t _jt_80188300 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a0;
    psx_check_interrupts_at(cpu, _jt_80188300);
    cpu->pc = _jt_80188300; return;  /* CPS: jr $ra */
}


/* ======== 0x80188308 ======== */
void prim_poly_emit_5ca(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80188310u: goto block_80188310;
            case 0x80188318u: goto block_80188318;
            case 0x8018831Cu: goto block_8018831C;
            case 0x80188368u: goto block_80188368;
            case 0x80188424u: goto block_80188424;
            case 0x8018842Cu: goto block_8018842C;
            default: break;
        }
    }
    /* Address: 0x80188308, Size: 300 bytes, Blocks: 7 */

block_80188308:
    int _bc_80188308 = ((int32_t)a3 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018830C: 0x00000000 */
    if (_bc_80188308) {
        psx_check_interrupts_at(cpu, 0x8018842Cu);
        goto block_8018842C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188310u);
        goto block_80188310;  /* not taken */
    }

block_80188310:
    int _bc_80188310 = (a2 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t4 = 36;
    if (_bc_80188310) {
        psx_check_interrupts_at(cpu, 0x8018831Cu);
        goto block_8018831C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188318u);
        goto block_80188318;  /* not taken */
    }

block_80188318:
    t4 = t4 + 28;
    /* fall through to block_8018831C */

block_8018831C:
    t6 = cpu->read_word(a0);
    t7 = cpu->read_word(a0 + 4);
    t8 = cpu->read_word(a0 + 8);
    t0 = cpu->read_word(t6);
    t1 = cpu->read_word(t7);
    t2 = cpu->read_word(t8);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 12, t0);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 13, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 14, t2);  /* mtc2 */
    a3 = a3 + -1;
    t3 = a0 + t4;
    gte_execute(cpu, 0x1400006);  /* gte cmd 0x06 */  /* 0x80188348: 0x4B400006 */
    cpu->write_word(t3 + 8, t0);
    cpu->write_word(t3 + 16, t1);
    cpu->write_word(t3 + 24, t2);
    
    psx_gte_read(cpu, 13);
    t5 = cpu->gte_data[24];  /* mfc2 */
    /* nop */  /* 0x8018835C: 0x00000000 */
    int _bc_80188360 = ((int32_t)t5 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80188364: 0x00000000 */
    if (_bc_80188360) {
        psx_check_interrupts_at(cpu, 0x80188424u);
        goto block_80188424;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188368u);
        goto block_80188368;  /* not taken */
    }

block_80188368:
    t0 = cpu->read_word(a0 + 24);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 17, (cpu->read_word((t6 + 4))));
    t2 = cpu->read_word(t0);
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (cpu->read_word((a0 + 12)));
    t1 = t2 << 16;
    t1 = (uint32_t)t1 >> 16;
    t2 = (uint32_t)t2 >> 16;
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, t2);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, (cpu->read_word((t0 + 4))));
    /* nop */  /* 0x80188390: 0x00000000 */
    gte_execute(cpu, 0x138041C);  /* gte cmd 0x1C */  /* 0x80188394: 0x4B38041C */
    t0 = cpu->read_word(a0 + 28);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 18, (cpu->read_word((t7 + 4))));
    t2 = cpu->read_word(t0);
    
    psx_gte_stall(cpu);
    cpu->write_word((t3 + 4), (cpu->gte_data[22]));
    gte_precision_store_word((t3 + 4), 22);
    t1 = t2 << 16;
    t1 = (uint32_t)t1 >> 16;
    t2 = (uint32_t)t2 >> 16;
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, t2);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, (cpu->read_word((t0 + 4))));
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (cpu->read_word((a0 + 16)));
    /* nop */  /* 0x801883C4: 0x00000000 */
    gte_execute(cpu, 0x138041C);  /* gte cmd 0x1C */  /* 0x801883C8: 0x4B38041C */
    t0 = cpu->read_word(a0 + 32);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 19, (cpu->read_word((t8 + 4))));
    t2 = cpu->read_word(t0);
    
    psx_gte_stall(cpu);
    cpu->write_word((t3 + 12), (cpu->gte_data[22]));
    gte_precision_store_word((t3 + 12), 22);
    gte_execute(cpu, 0x158002D);  /* gte cmd 0x2D */  /* 0x801883DC: 0x4B58002D */
    t1 = t2 << 16;
    t1 = (uint32_t)t1 >> 16;
    t2 = (uint32_t)t2 >> 16;
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, t2);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, (cpu->read_word((t0 + 4))));
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (cpu->read_word((a0 + 20)));
    
    psx_gte_read(cpu, 8);
    t0 = gte_read_data(cpu, 7);  /* mfc2 */
    gte_execute(cpu, 0x138041C);  /* gte cmd 0x1C */  /* 0x80188400: 0x4B38041C */
    v0 = t0 & 0x1FFC;  /* 0x80188404: 0x31021FFC */
    v0 = a1 + v0;
    t0 = cpu->read_word(v0);
    t1 = t3 << 8;
    t0 = t0 << 8;
    
    psx_gte_stall(cpu);
    cpu->write_word((t3 + 20), (cpu->gte_data[22]));
    gte_precision_store_word((t3 + 20), 22);
    /* fall through to block_80188424 */

block_80188424:
    int _bc_80188424 = ((int32_t)a3 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 92;
    if (_bc_80188424) {
        psx_check_interrupts_at(cpu, 0x8018831Cu);
        goto block_8018831C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018842Cu);
        goto block_8018842C;  /* not taken */
    }

block_8018842C:
    uint32_t _jt_8018842C = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a0;
    psx_check_interrupts_at(cpu, _jt_8018842C);
    cpu->pc = _jt_8018842C; return;  /* CPS: jr $ra */
}


/* ======== 0x80188434 ======== */
void prim_point_emit_48a(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018843Cu: goto block_8018843C;
            case 0x80188444u: goto block_80188444;
            case 0x80188448u: goto block_80188448;
            case 0x80188494u: goto block_80188494;
            case 0x80188500u: goto block_80188500;
            case 0x80188508u: goto block_80188508;
            default: break;
        }
    }
    /* Address: 0x80188434, Size: 220 bytes, Blocks: 7 */

block_80188434:
    int _bc_80188434 = ((int32_t)a3 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80188438: 0x00000000 */
    if (_bc_80188434) {
        psx_check_interrupts_at(cpu, 0x80188508u);
        goto block_80188508;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018843Cu);
        goto block_8018843C;  /* not taken */
    }

block_8018843C:
    int _bc_8018843C = (a2 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t4 = 24;
    if (_bc_8018843C) {
        psx_check_interrupts_at(cpu, 0x80188448u);
        goto block_80188448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188444u);
        goto block_80188444;  /* not taken */
    }

block_80188444:
    t4 = t4 + 24;
    /* fall through to block_80188448 */

block_80188448:
    t6 = cpu->read_word(a0);
    t7 = cpu->read_word(a0 + 4);
    t8 = cpu->read_word(a0 + 8);
    t0 = cpu->read_word(t6);
    t1 = cpu->read_word(t7);
    t2 = cpu->read_word(t8);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 12, t0);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 13, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 14, t2);  /* mtc2 */
    a3 = a3 + -1;
    t3 = a0 + t4;
    gte_execute(cpu, 0x1400006);  /* gte cmd 0x06 */  /* 0x80188474: 0x4B400006 */
    cpu->write_word(t3 + 8, t0);
    cpu->write_word(t3 + 12, t1);
    cpu->write_word(t3 + 16, t2);
    
    psx_gte_read(cpu, 13);
    t5 = cpu->gte_data[24];  /* mfc2 */
    /* nop */  /* 0x80188488: 0x00000000 */
    int _bc_8018848C = ((int32_t)t5 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80188490: 0x00000000 */
    if (_bc_8018848C) {
        psx_check_interrupts_at(cpu, 0x80188500u);
        goto block_80188500;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188494u);
        goto block_80188494;  /* not taken */
    }

block_80188494:
    t9 = cpu->read_word(a0 + 12);
    t0 = cpu->read_word(a0 + 20);
    t5 = cpu->read_word(t9);
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 16, (cpu->read_word((t6 + 4))));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 17, (cpu->read_word((t7 + 4))));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 18, (cpu->read_word((t8 + 4))));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 19, (cpu->read_word((t9 + 4))));
    cpu->write_word(t3 + 20, t5);
    gte_execute(cpu, 0x168002E);  /* gte cmd 0x2E */  /* 0x801884B4: 0x4B68002E */
    t2 = cpu->read_word(t0);
    
    psx_gte_stall(cpu);
    cpu->gte_data[6] = (cpu->read_word((a0 + 16)));
    t1 = t2 << 16;
    t1 = (uint32_t)t1 >> 16;
    t2 = (uint32_t)t2 >> 16;
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, t1);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, t2);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, (cpu->read_word((t0 + 4))));
    
    psx_gte_read(cpu, 8);
    t0 = gte_read_data(cpu, 7);  /* mfc2 */
    gte_execute(cpu, 0x138041C);  /* gte cmd 0x1C */  /* 0x801884DC: 0x4B38041C */
    v0 = t0 & 0x1FFC;  /* 0x801884E0: 0x31021FFC */
    v0 = a1 + v0;
    t0 = cpu->read_word(v0);
    t1 = t3 << 8;
    t0 = t0 << 8;
    
    psx_gte_stall(cpu);
    cpu->write_word((t3 + 4), (cpu->gte_data[22]));
    gte_precision_store_word((t3 + 4), 22);
    /* fall through to block_80188500 */

block_80188500:
    int _bc_80188500 = ((int32_t)a3 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = a0 + 72;
    if (_bc_80188500) {
        psx_check_interrupts_at(cpu, 0x80188448u);
        goto block_80188448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80188508u);
        goto block_80188508;  /* not taken */
    }

block_80188508:
    uint32_t _jt_80188508 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a0;
    psx_check_interrupts_at(cpu, _jt_80188508);
    cpu->pc = _jt_80188508; return;  /* CPS: jr $ra */
}

/* ======== 0x80194470 ======== */
void gte_screen_init_defer_mfhi(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80194470, Size: 8 bytes, Blocks: 1 */

block_80194470:
    t6 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* nop: write to $zero */  /* 0x80194474: 0x00440000 */
    cpu->pc = 0x80194478u; return;  /* fallthrough to split piece */
    cpu->pc = 0x80194478u; return;/* fallthrough to next function */
}


/* ======== 0x80194614 ======== */
void fixed_point_ret_neg1(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80194614, Size: 8 bytes, Blocks: 1 */

block_80194614:
    uint32_t _jt_80194614 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = -1;
    psx_check_interrupts_at(cpu, _jt_80194614);
    cpu->pc = _jt_80194614; return;  /* CPS: jr $ra */
}

/* ======== 0x80194940 ======== */
void fixed_recip_neg_exp(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80194940, Size: 12 bytes, Blocks: 1 */

block_80194940:
    t1 = zero - t1;
    uint32_t _jt_80194944 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = (uint32_t)t5 >> (t1 & 0x1F);
    psx_check_interrupts_at(cpu, _jt_80194944);
    cpu->pc = _jt_80194944; return;  /* CPS: jr $ra */
}

