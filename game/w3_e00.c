/* w3_e00.c — clean ports for leftover generated coverage in the
 * 0x80122170 / 0x80126344 / 0x801271xx range (boot/init + display setup).
 *
 * All ten guest addrs are CPS piece-entries / alternate entries, not
 * standalone functions: each performs a few guest ops then tail-dispatches
 * to the next guest block addr via cpu->pc (the clean-CPS idiom — see
 * cps_clean.c). They are registered in game/clean_cps.map so
 * clean_try_dispatch hands the body cpu->pc == the dispatched addr and
 * preserves the tail-dispatch pc on return.
 *
 *   0x80122170  pointer-table data addr (NOT code) — the generated body is a
 *               fail-closed psx_unknown_dispatch stub; reproduced faithfully.
 *   0x80126344  alternate entry into main() (entry_0x80126234): reloads the
 *               state byte lbu v1,0(s1) and falls into the g_game_state
 *               dispatch chain at 0x80126348 (cps_80126348, already clean).
 *   0x80127100..0x80127148  display_mode_setup switch-case trampolines:
 *               li v0,<width> then j/fallthrough into the 0xf0-height
 *               (0x80127134) or 0x1e0-height (0x8012714C) commit blocks.
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
extern void     psx_unknown_dispatch(CPUState *cpu, uint32_t addr, uint32_t phys);

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

/* ------------------------------------------------------------------ */
/* 0x80122170 — NOT code: guest bytes are a pointer table
 * (0x8016C0B8 repeated / 0x8016C094). The generated stub is the
 * fail-closed psx_unknown_dispatch body; reproduce it exactly. */
void ptr_table_80122170(void)
{
    CPUState *cpu = clean_cpu_state();
    psx_unknown_dispatch(cpu, 0x80122170u, 0x00122170u);
}

/* ------------------------------------------------------------------ */
/* 0x80126344 — alternate entry into main()'s state dispatch.
 * Guest block_80126344:  lbu v1, 0(s1)   (0x80126344: 0x92230000)
 * then fall through into block_80126348 (sltiu v0,v1,14 -> state
 * table 0x8012E800 dispatch), already covered by cps_80126348. */
void main_state_byte_gate(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[17]);   /* lbu v1,0(s1) */
    cpu->pc = 0x80126348u; return;               /* fallthrough to split piece */
}

/* ------------------------------------------------------------------ */
/* display_mode_setup (0x8012708C) switch-case trampolines.
 * Each entry is `j TARGET` + delay-slot `li v0,<width>` (or a bare
 * `li v0,<width>` falling into the next block for the last two cases).
 * TARGET 0x80127134 = 0xf0-height commit path; 0x8012714C = 0x1e0-height. */

/* 0x80127100: j 0x80127134 ; li v0,0x140 */
void display_mode_w320_hf0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x140;
    psx_check_interrupts_at(cpu, 0x80127134u);
    cpu->pc = 0x80127134u; return;               /* CPS j */
}

/* 0x80127108: j 0x8012714C ; li v0,0x140 */
void display_mode_w320_h1e0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x140;
    psx_check_interrupts_at(cpu, 0x8012714Cu);
    cpu->pc = 0x8012714Cu; return;               /* CPS j */
}

/* 0x80127110: j 0x80127134 ; li v0,0x180 */
void display_mode_w384_hf0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x180;
    psx_check_interrupts_at(cpu, 0x80127134u);
    cpu->pc = 0x80127134u; return;               /* CPS j */
}

/* 0x80127118: j 0x8012714C ; li v0,0x180 */
void display_mode_w384_h1e0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x180;
    psx_check_interrupts_at(cpu, 0x8012714Cu);
    cpu->pc = 0x8012714Cu; return;               /* CPS j */
}

/* 0x80127120: j 0x80127134 ; li v0,0x200 */
void display_mode_w512_hf0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x200;
    psx_check_interrupts_at(cpu, 0x80127134u);
    cpu->pc = 0x80127134u; return;               /* CPS j */
}

/* 0x80127128: j 0x8012714C ; li v0,0x200 */
void display_mode_w512_h1e0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x200;
    psx_check_interrupts_at(cpu, 0x8012714Cu);
    cpu->pc = 0x8012714Cu; return;               /* CPS j */
}

/* 0x80127130: li v0,0x280 — bare fallthrough into block_80127134 */
void display_mode_w640_hf0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x280;
    cpu->pc = 0x80127134u; return;               /* fallthrough to split piece */
}

/* 0x80127148: li v0,0x280 — bare fallthrough into block_8012714C */
void display_mode_w640_h1e0(void)
{
    CPUState *cpu = clean_cpu_state();
    cpu->gpr[2] = 0x280;
    cpu->pc = 0x8012714Cu; return;               /* fallthrough to split piece */
}
