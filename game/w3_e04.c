/* w3_e04.c — clean CPS piece-entry ports (orch-w3-e04 batch).
 *
 * All eleven guest addrs below are OVERLAPPING-ALIAS / jump-table piece
 * entries inside already-clean-ported parent functions (the parents are
 * plain clean C in menu.c/save.c, so these PCs only dispatch when the
 * parent runs generated — e.g. CLEAN_EXCLUDE bisection — or as CPS resume
 * targets). Each body is a faithful op-by-op transliteration of its
 * generated block(s): identical gpr ops/order, delay slots executed before
 * the branch resolves, and psx_check_interrupts_at + cpu->pc tail-dispatch
 * at every control boundary so continuation blocks resume inside the
 * generated parent body (its pc-switch covers every block addr).
 *
 * Dispatch addrs are registered in game/clean_cps.map (CLEAN_ENT_CPS):
 * clean_try_dispatch sets cpu->pc = the dispatched addr, this body's
 * entry-switch picks the block, and an unresolved cpu->pc is preserved
 * for the next dispatch (jr $ra / j / b<cond> targets).
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

/* ======== battle_state_gate (guest 0x801521A4) piece tail ========
 * Jump-table tail of battle_state_gate (clean port: menu.c). The parent's
 * JT @0x80120948 dispatches states {12-14} here: block 0x80152248 loads
 * s16(a0+0xa4) and returns (val>=0)?1:0 via the 0x80152264 `jr $ra` stub;
 * block 0x80152258 is the shared `jr $ra` with v0=0 delay slot. Disasm
 * generated/SLES_017.22_full_17.c br2eu_psx_alias_body_801521A4. */
void battle_state_gate_tail(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t pc;
  if (!cpu) return;
  pc = cpu->pc;
  if (pc != 0x80152248u && pc != 0x80152258u) return;  /* alias default: nop */

  if (pc == 0x80152248u) goto block_80152248;
  goto block_80152258;

block_80152248:
  cpu->gpr[2] = (int32_t)(int16_t)br2_ld16(cpu->gpr[4] + 0xa4); /* lh v0,0xa4(a0) */
  /* nop @0x8015224C */
  { int _bc = ((int32_t)cpu->gpr[2] >= 0);   /* bgez v0 — cond before delay */
    cpu->gpr[2] = 1;                          /* delay: li v0,1 */
    if (_bc) {
      psx_check_interrupts_at(cpu, 0x80152264u);
      cpu->pc = 0x80152264u; return;          /* CPS taken: split -> jr-ra stub */
    } else {
      psx_check_interrupts_at(cpu, 0x80152258u);
      goto block_80152258;                    /* not taken */
    } }

block_80152258:
  { uint32_t _jt = cpu->gpr[31];              /* jr $ra — latch before delay */
    cpu->gpr[2] = cpu->gpr[0];                /* delay: move v0,zero */
    psx_check_interrupts_at(cpu, _jt);
    cpu->pc = _jt; return; }                  /* CPS: jr $ra */
}

/* ======== mc_slot_action (guest 0x8015260C) case-0 piece ========
 * Jump-table case 0 of mc_slot_action's param_1[0x9b] dispatch (clean port:
 * save.c). Writes 0x100 to the slot's +0xb4 halfword then joins the shared
 * tail at 0x801526F0 (generated continuation — not clean-registered).
 * Disasm generated/SLES_017.22_full_17.c br2eu_psx_alias_body_8015260C. */
void mc_slot_action_case0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  if (cpu->pc != 0x80152698u) return;

  cpu->gpr[2] = cpu->gpr[16] << 1;             /* sll v0,s0,1 */
  cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[2];     /* addu v0,a0,v0 */
  cpu->gpr[3] = 0x100;                        /* addiu v1,zero,0x100 */
  br2_st16(cpu->gpr[2] + 0xb4, (uint16_t)cpu->gpr[3]); /* sh v1,0xb4(v0) — j delay */
  psx_check_interrupts_at(cpu, 0x801526F0u);
  cpu->pc = 0x801526F0u;                      /* j 0x801526F0 — shared tail */
}

/* ======== battle_config_eval (guest 0x80153544) state-0 case ========
 * JT case 0 of the phase-0x300/state sub-dispatch (table 0x801209f8,
 * clean port: menu.c battle_config_eval L_B0). Loads u16(B+0x92), masks
 * 0xf00, and branches: ==0x200 -> 0x80153774, else slti(v1,0x201) result
 * decides 0x80153748. Both targets stay in the generated parent body.
 * Disasm generated/SLES_017.22_full_18.c br2eu_psx_alias_body_80153544. */
void battle_config_eval_b0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  if (cpu->pc != 0x80153730u) return;

  cpu->gpr[2] = br2_ld16(cpu->gpr[17] + 0x92); /* lhu v0,0x92(s1) */
  /* nop @0x80153734 */
  cpu->gpr[3] = cpu->gpr[2] & 0x0f00;          /* andi v1,v0,0xf00 */
  cpu->gpr[2] = 0x200;                        /* addiu v0,zero,0x200 */
  { int _bc = (cpu->gpr[3] == cpu->gpr[2]);    /* beq v1,v0 — cond before delay */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 513) ? 1u : 0u; /* delay: slti v0,v1,0x201 */
    if (_bc) {
      psx_check_interrupts_at(cpu, 0x80153774u);
      cpu->pc = 0x80153774u; return;
    } else {
      psx_check_interrupts_at(cpu, 0x80153748u);
      cpu->pc = 0x80153748u; return;
    } }
}

/* ======== post_battle_dispatch (guest 0x80154930) select pieces ========
 * Two jump-table case entries of post_battle_dispatch (clean port:
 * menu.c). Both read s16(s1+0x8c); case @0x80155678 continues the
 * <30/select flow, case @0x80155708 writes a flag byte. Targets stay in
 * the generated parent body. Disasm generated/SLES_017.22_full_19.c
 * br2eu_psx_alias_body_80154930. */
void post_battle_dispatch_sel(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t pc;
  if (!cpu) return;
  pc = cpu->pc;
  if (pc != 0x80155678u && pc != 0x80155708u) return;

  if (pc == 0x80155678u) goto block_80155678;
  goto block_80155708;

block_80155678:
  cpu->gpr[2] = (int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 0x8c); /* lh v0,0x8c(s1) */
  /* nop @0x8015567C */
  cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 30) ? 1u : 0u;  /* slti v0,v0,0x1e */
  { int _bc = (cpu->gpr[2] != cpu->gpr[0]);   /* bnez v0 — cond before delay */
    cpu->gpr[3] = 0x80020000u;                /* delay: lui v1,0x8002 */
    if (_bc) {
      psx_check_interrupts_at(cpu, 0x801556ACu);
      cpu->pc = 0x801556ACu; return;
    } else {
      psx_check_interrupts_at(cpu, 0x8015568Cu);
      cpu->pc = 0x8015568Cu; return;
    } }

block_80155708:
  cpu->gpr[3] = (int32_t)(int16_t)br2_ld16(cpu->gpr[17] + 0x8c); /* lh v1,0x8c(s1) */
  cpu->gpr[2] = 1;                            /* addiu v0,zero,1 */
  { int _bc = (cpu->gpr[3] != cpu->gpr[0]);   /* bnez v1 — cond before delay */
    br2_st8(cpu->gpr[17] + 0xb1, (uint8_t)cpu->gpr[2]); /* delay: sb v0,0xb1(s1) */
    if (_bc) {
      psx_check_interrupts_at(cpu, 0x80155720u);
      cpu->pc = 0x80155720u; return;
    } else {
      psx_check_interrupts_at(cpu, 0x80155718u);
      cpu->pc = 0x80155718u; return;
    } }
}

/* ======== page_table_ptr_set (guest 0x801566D4) case pieces ========
 * Five jump-table case entries of page_table_ptr_set's (param_2,
 * param_1) table select (clean port: menu.c). Each loads the selected
 * 0x801Bxxxx table pointer into a2 (or, for 0x80156784, bounds-checks
 * a0<0x14 first) and joins the shared store tail at 0x801568E4, which
 * stays in the generated parent body. Disasm
 * generated/SLES_017.22_full_19.c br2eu_psx_alias_body_801566D4. */
void page_table_ptr_set_sel(void)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t pc;
  if (!cpu) return;
  pc = cpu->pc;
  if (pc != 0x80156784u && pc != 0x80156800u && pc != 0x80156818u &&
      pc != 0x80156824u && pc != 0x80156830u) return;

  if (pc == 0x80156784u) goto block_80156784;
  if (pc == 0x80156800u) goto block_80156800;
  if (pc == 0x80156818u) goto block_80156818;
  if (pc == 0x80156824u) goto block_80156824;
  goto block_80156830;

block_80156784:
  cpu->gpr[2] = (cpu->gpr[4] < 20u) ? 1u : 0u;  /* sltiu v0,a0,0x14 */
  { int _bc = (cpu->gpr[2] == cpu->gpr[0]);    /* beqz v0 — cond before delay */
    cpu->gpr[2] = 0x80120000u;                 /* delay: lui v0,0x8012 */
    if (_bc) {
      psx_check_interrupts_at(cpu, 0x801568E4u);
      cpu->pc = 0x801568E4u; return;
    } else {
      psx_check_interrupts_at(cpu, 0x80156790u);
      cpu->pc = 0x80156790u; return;
    } }

block_80156800:
  cpu->gpr[2] = 0x801B0000u;                   /* lui v0,0x801b */
  cpu->gpr[6] = cpu->gpr[2] + 0x38ac;          /* delay: addiu a2,v0,0x38ac */
  psx_check_interrupts_at(cpu, 0x801568E4u);
  cpu->pc = 0x801568E4u; return;               /* j 0x801568E4 */

block_80156818:
  cpu->gpr[2] = 0x801B0000u;                   /* lui v0,0x801b */
  cpu->gpr[6] = cpu->gpr[2] + 0x35cc;          /* delay: addiu a2,v0,0x35cc */
  psx_check_interrupts_at(cpu, 0x801568E4u);
  cpu->pc = 0x801568E4u; return;               /* j 0x801568E4 */

block_80156824:
  cpu->gpr[2] = 0x801B0000u;                   /* lui v0,0x801b */
  cpu->gpr[6] = cpu->gpr[2] + 0x3758;          /* delay: addiu a2,v0,0x3758 */
  psx_check_interrupts_at(cpu, 0x801568E4u);
  cpu->pc = 0x801568E4u; return;               /* j 0x801568E4 */

block_80156830:
  cpu->gpr[2] = 0x801B0000u;                   /* lui v0,0x801b */
  cpu->gpr[6] = cpu->gpr[2] + 0x377c;          /* delay: addiu a2,v0,0x377c */
  psx_check_interrupts_at(cpu, 0x801568E4u);
  cpu->pc = 0x801568E4u; return;               /* j 0x801568E4 */
}
