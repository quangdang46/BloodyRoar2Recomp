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
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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
  case 0x80156864u: goto block_80156864;
  case 0x80156870u: goto block_80156870;
  case 0x80156888u: goto block_80156888;
  case 0x80156894u: goto block_80156894;
  case 0x801568E4u: goto block_801568E4;
  case 0x801568ECu: goto block_801568EC;
  default: return c->gpr[2];
  }

block_80156864:
  c->gpr[2] = 0x801B0000u;                     /* lui v0,0x801B */
  c->gpr[6] = c->gpr[2] + 0x38D0u;             /* delay: a2 = &DAT_801b38d0 */
  goto dispatch_801568E4;

block_80156870:
  c->gpr[2] = 0x801B0000u;
  c->gpr[6] = c->gpr[2] + 0x3968u;             /* delay: a2 = &DAT_801b3968 */
  goto dispatch_801568E4;

block_80156888:
  c->gpr[2] = 0x801B0000u;
  c->gpr[6] = c->gpr[2] + 0x3A84u;             /* delay: a2 = &DAT_801b3a84 */
  goto dispatch_801568E4;

block_80156894:
  c->gpr[2] = 0x801B0000u;
  c->gpr[6] = c->gpr[2] + 0x3B08u;             /* delay: a2 = &DAT_801b3b08 */
  goto dispatch_801568E4;

dispatch_801568E4:
  psx_check_interrupts_at(c, 0x801568E4u);
  goto block_801568E4;

block_801568E4:
  c->gpr[3] = 0x801F0000u;                     /* lui v1,0x801F */
  c->gpr[3] = c->gpr[3] + 0x35E0u;             /* delay: v1 = 0x801F35E0 */
  /* fall through — generated has no interrupt check between E4 and EC */

block_801568EC:
  c->gpr[2] = c->gpr[5] << 2;                  /* sll v0,a1,2 */
  c->gpr[2] = c->gpr[2] + c->gpr[3];           /* addu v0,v0,v1 */
  { uint32_t jr_target = c->gpr[31];                 /* latch jr target */
    c->write_word(c->gpr[2], c->gpr[6]);       /* delay: sw a2,0(v0) */
    psx_check_interrupts_at(c, jr_target);
    c->pc = jr_target;
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
  case 0x80157910u: goto block_80157910;
  case 0x80157918u: goto block_80157918;
  case 0x80157920u: goto block_80157920;
  case 0x80157928u: goto block_80157928;
  case 0x80157930u: goto block_80157930;
  case 0x80157938u: goto block_80157938;
  case 0x80157940u: goto block_80157940;
  case 0x80157964u: goto block_80157964;
  case 0x801579E0u: goto block_801579E0;
  case 0x801579E4u: goto block_801579E4;
  case 0x801579F0u: goto block_801579F0;
  default: return c->gpr[2];
  }

block_80157910:
  c->gpr[5] = c->gpr[0];                     /* delay: move a1,zero */
  goto dispatch_80157964;
block_80157918:
  c->gpr[5] = 1;                             /* delay: li a1,1 */
  goto dispatch_80157964;
block_80157920:
  c->gpr[5] = 2;
  goto dispatch_80157964;
block_80157928:
  c->gpr[5] = 5;
  goto dispatch_80157964;
block_80157930:
  c->gpr[5] = 3;
  goto dispatch_80157964;
block_80157938:
  c->gpr[5] = 6;
  goto dispatch_80157964;
block_80157940:
  c->gpr[5] = 4;
  goto dispatch_80157964;

dispatch_80157964:
  psx_check_interrupts_at(c, 0x80157964u);
  goto block_80157964;

block_80157964:
  c->gpr[5] = c->gpr[5] + 20;                /* delay: addiu a1,a1,0x14 */
  psx_check_interrupts_at(c, 0x801579E4u);
  goto block_801579E4;

block_801579E0:
  c->gpr[5] = c->gpr[5] + 10;                /* addiu a1,a1,0xa */
  /* fall through — generated has no interrupt check between E0 and E4 */

block_801579E4:
  c->gpr[4] = c->gpr[5];                     /* move a0,a1 */
  /* fall through — generated has no interrupt check between E4 and E8 */

block_801579E8:
  c->gpr[31] = 0x801579F0u;                  /* jal link */
  c->gpr[5] = 1;                             /* delay: li a1,1 */
  psx_check_interrupts_at(c, 0x801577BCu);
  c->pc = 0x801577BCu;                       /* CPS jal -> mc_confirm_dispatch */
  return c->gpr[2];

block_801579F0:
  c->gpr[31] = c->read_word(c->gpr[29] + 16);/* lw ra,16(sp) */
  /* nop @0x801579F4 */
  { uint32_t jr_target = c->gpr[31];               /* latch jr target */
    c->gpr[29] = c->gpr[29] + 24;            /* delay: addiu sp,sp,0x18 */
    psx_check_interrupts_at(c, jr_target);
    c->pc = jr_target;
    return c->gpr[2]; }
}
