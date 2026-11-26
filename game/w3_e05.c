/* w3_e05.c — clean CPS ports of the page_table_ptr_set / confirm_code_select
 * alias piece-entries (worktree orch/w3/e05).
 *
 * All eleven guest addrs are ALTERNATE ENTRIES into shared generated bodies
 * ("overlapping-alias" entries), not standalone functions:
 *
 *  - 0x80156864/0x80156870/0x80156888/0x80156894 enter the body of
 *    page_table_ptr_set (guest 0x801566D4, already clean in menu.c). Each
 *    stub loads a2 = 0x801B0000 + <DAT offset> then joins the shared tail
 *    block_801568E4 -> block_801568EC which stores a2 into the pointer table
 *    at guest 0x801F35E0 + a1*4 and returns (jr $ra). Faithful CPS
 *    transliteration of generated/SLES_017.22_full_19.c
 *    br2eu_psx_alias_body_801566D4.
 *
 *  - 0x80157910/18/20/28/30/38/40 enter the body of confirm_code_select
 *    (guest 0x8015787C, already clean in save.c). Each stub loads a1 = N
 *    then joins block_80157964 (a1 += 20) -> block_801579E4 (a0 = a1) ->
 *    block_801579E8 (ra = 0x801579F0, a1 = 1, jal 0x801577BC =
 *    mc_confirm_dispatch) -> block_801579F0 epilogue (lw ra,16(sp); jr ra;
 *    sp += 24). Faithful CPS transliteration of
 *    generated/SLES_017.22_full_20.c br2eu_psx_alias_body_8015787C.
 *
 * Continuation addrs 0x801568E4/0x801568EC and 0x80157964/0x801579E4/
 * 0x801579E0/0x801579F0 are registered in game/clean_cps.map so a yield inside
 * psx_check_interrupts_at re-dispatches back into these bodies with
 * cpu->pc == that addr (the CLEAN_ENT_CPS contract: cpu->pc preset to the
 * dispatched addr, tail-dispatch preserved, no forced jr-$ra epilogue).
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

/* ======== ptab_ptr_case_set ========
 * Shared CPS body for the four page_table_ptr_set alias entries
 * (0x80156864/0x80156870/0x80156888/0x80156894) plus the shared-tail
 * continuation addrs 0x801568E4/0x801568EC.
 *
 * Each entry:  lui   v0,0x801B            ; v0 = 0x801B0000
 *              j     0x801568E4
 *              addiu a2,v0,<off>         ; delay slot: a2 = &DAT_801b<off>
 * block_801568E4: lui v1,0x801F ; addiu v1,v1,0x35E0 (delay) -> v1=0x801F35E0
 * block_801568EC: sll v0,a1,2 ; addu v0,v0,v1 ; jr ra
 *              sw   a2,0(v0)            ; delay slot: table[a1] = a2
 * Offsets: 0x80156864->0x38D0, 0x80156870->0x3968, 0x80156888->0x3A84,
 *          0x80156894->0x3B08 (the DAT_801b38d0/3968/3a84/3b08 case arms). */
uint32_t ptab_ptr_case_set(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  if (!c) return 0;
  pc = c->pc;
  switch (pc) {
  case 0x80156864u: goto b864;
  case 0x80156870u: goto b870;
  case 0x80156888u: goto b888;
  case 0x80156894u: goto b894;
  case 0x801568E4u: goto bE4;
  case 0x801568ECu: goto bEC;
  default: return c->gpr[2];
  }

b864:
  c->gpr[2] = 0x801B0000u;                     /* lui v0,0x801B */
  c->gpr[6] = c->gpr[2] + 0x38D0u;             /* delay: a2 = &DAT_801b38d0 */
  goto chkE4;

b870:
  c->gpr[2] = 0x801B0000u;
  c->gpr[6] = c->gpr[2] + 0x3968u;             /* delay: a2 = &DAT_801b3968 */
  goto chkE4;

b888:
  c->gpr[2] = 0x801B0000u;
  c->gpr[6] = c->gpr[2] + 0x3A84u;             /* delay: a2 = &DAT_801b3a84 */
  goto chkE4;

b894:
  c->gpr[2] = 0x801B0000u;
  c->gpr[6] = c->gpr[2] + 0x3B08u;             /* delay: a2 = &DAT_801b3b08 */
  goto chkE4;

chkE4:
  psx_check_interrupts_at(c, 0x801568E4u);
  goto bE4;

bE4:
  c->gpr[3] = 0x801F0000u;                     /* lui v1,0x801F */
  c->gpr[3] = c->gpr[3] + 0x35E0u;             /* delay: v1 = 0x801F35E0 */
  /* fall through — generated has no interrupt check between E4 and EC */

bEC:
  c->gpr[2] = c->gpr[5] << 2;                  /* sll v0,a1,2 */
  c->gpr[2] = c->gpr[2] + c->gpr[3];           /* addu v0,v0,v1 */
  { uint32_t _jt = c->gpr[31];                 /* latch jr target */
    c->write_word(c->gpr[2], c->gpr[6]);       /* delay: sw a2,0(v0) */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt;
    return c->gpr[2]; }
}

/* ======== confirm_code_tail_set ========
 * Shared CPS body for the seven confirm_code_select alias entries
 * (0x80157910/18/20/28/30/38/40) plus continuation addrs
 * 0x80157964/0x801579E0/0x801579E4/0x801579F0.
 *
 * Each entry:  j     0x80157964
 *              li    a1,N                  ; delay slot
 * block_80157964: j 0x801579E4 ; addiu a1,a1,20 (delay) -> check -> E4
 * block_801579E0: addiu a1,a1,10           ; sibling +10 path (generated
 *              entries 0x8015799C-DC only; kept for yield-resume fidelity)
 * block_801579E4: move a0,a1               ; fallthrough, no IRQ check
 * block_801579E8: ra = 0x801579F0 (jal link); li a1,1 (delay);
 *              jal 0x801577BC = mc_confirm_dispatch
 * block_801579F0: lw ra,16(sp) ; nop ; jr ra ; addiu sp,sp,24 (delay)
 * a1 constants: 910->0, 918->1, 920->2, 928->5, 930->3, 938->6, 940->4
 * (dispatch arg a0 = N + 20, matching confirm_code_select's code += 0x14). */
uint32_t confirm_code_tail_set(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  if (!c) return 0;
  pc = c->pc;
  switch (pc) {
  case 0x80157910u: goto b910;
  case 0x80157918u: goto b918;
  case 0x80157920u: goto b920;
  case 0x80157928u: goto b928;
  case 0x80157930u: goto b930;
  case 0x80157938u: goto b938;
  case 0x80157940u: goto b940;
  case 0x80157964u: goto b964;
  case 0x801579E0u: goto b9E0;
  case 0x801579E4u: goto b9E4;
  case 0x801579F0u: goto b9F0;
  default: return c->gpr[2];
  }

b910:
  c->gpr[5] = c->gpr[0];                     /* delay: move a1,zero */
  goto chk964;
b918:
  c->gpr[5] = 1;                             /* delay: li a1,1 */
  goto chk964;
b920:
  c->gpr[5] = 2;
  goto chk964;
b928:
  c->gpr[5] = 5;
  goto chk964;
b930:
  c->gpr[5] = 3;
  goto chk964;
b938:
  c->gpr[5] = 6;
  goto chk964;
b940:
  c->gpr[5] = 4;
  goto chk964;

chk964:
  psx_check_interrupts_at(c, 0x80157964u);
  goto b964;

b964:
  c->gpr[5] = c->gpr[5] + 20;                /* delay: addiu a1,a1,0x14 */
  psx_check_interrupts_at(c, 0x801579E4u);
  goto b9E4;

b9E0:
  c->gpr[5] = c->gpr[5] + 10;                /* addiu a1,a1,0xa */
  /* fall through — generated has no interrupt check between E0 and E4 */

b9E4:
  c->gpr[4] = c->gpr[5];                     /* move a0,a1 */
  /* fall through — generated has no interrupt check between E4 and E8 */

b9E8:
  c->gpr[31] = 0x801579F0u;                  /* jal link */
  c->gpr[5] = 1;                             /* delay: li a1,1 */
  psx_check_interrupts_at(c, 0x801577BCu);
  c->pc = 0x801577BCu;                       /* CPS jal -> mc_confirm_dispatch */
  return c->gpr[2];

b9F0:
  c->gpr[31] = c->read_word(c->gpr[29] + 16);/* lw ra,16(sp) */
  /* nop @0x801579F4 */
  { uint32_t _jt = c->gpr[31];               /* latch jr target */
    c->gpr[29] = c->gpr[29] + 24;            /* delay: addiu sp,sp,0x18 */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt;
    return c->gpr[2]; }
}
