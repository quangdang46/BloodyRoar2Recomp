/* w2_e06.c — clean-room ports for the 0x80183630-0x80183948 piece-entry
 * range (SLES_017.22_full_38.c).
 *
 * All eight addrs are CPS split-piece / alias entries, NOT standalone
 * functions — the generated dispatch aliases each into its host body with
 * cpu->pc preset to the piece addr:
 *
 *   0x80183630, 0x80183648  -> host fn 0x801835D0
 *   0x8018371C, 0x801837B0, 0x801838B0, 0x801838E8, 0x80183920, 0x80183948
 *                          -> host fn 0x80183664 (scriptop_wait, vm.c)
 *
 * Group A (0x80183630/0x80183648): continuation blocks of guest fn
 * 0x801835D0. That function already has a faithful clean CPS
 * transliteration, cps_801835d0 (game/cps_clean.c), whose pc-switch
 * contains block_80183630 and block_80183648 verbatim. Both addrs are
 * routed to it through game/clean_cps.map — no new body needed here.
 *
 * Group B (the six 0x801837xx/0x801838xx/0x801839xx addrs): mid-body CPS
 * resume PCs inside guest fn 0x80183664 (1388 bytes, 70 blocks). The
 * function entry is already clean-ported as scriptop_wait (game/vm.c),
 * a synchronous readable implementation with no CPS yield points — so
 * these generated-only resume states cannot be produced while the clean
 * body runs. Faithfully porting them would mean duplicating the entire
 * 70-block CPS frame (live s-regs, sp-frame spills, per-block
 * psx_check_interrupts_at resume PCs) that clean C cannot reproduce
 * (AGENTS.md §4). They are therefore explicit br2_call_generated
 * delegates: if a resume PC ever lands on one, the generated alias body
 * reproduces the exact guest semantics. gen_clean_table skips these
 * self-delegates, so dispatch is unchanged.
 */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

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

/* ======== scriptop_wait CPS resume fragments (guest 0x80183664) ========
 * Each addr is a mid-body continuation of the generated 70-block CPS
 * function; the generated alias entry presets cpu->pc to the piece addr
 * and resumes the block. Delegation preserves that contract exactly. */

uint32_t scriptop_wait_fragment_8018371c(uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018371cu, a0, a1, a2, a3);
}

uint32_t scriptop_wait_fragment_801837b0(uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x801837b0u, a0, a1, a2, a3);
}

uint32_t scriptop_wait_fragment_801838b0(uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x801838b0u, a0, a1, a2, a3);
}

uint32_t scriptop_wait_fragment_801838e8(uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x801838e8u, a0, a1, a2, a3);
}

uint32_t scriptop_wait_fragment_80183920(uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x80183920u, a0, a1, a2, a3);
}

uint32_t scriptop_wait_fragment_80183948(uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x80183948u, a0, a1, a2, a3);
}
