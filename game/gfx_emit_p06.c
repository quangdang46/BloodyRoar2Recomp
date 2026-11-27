/* gfx_emit_p06.c — clean ports for guest 0x8018E774..0x8018E85C.
 *
 * All nine addresses are CPS piece-entries inside printf_core
 * (guest 0x8018E590, 1684B / 125 blocks, already clean-ported as a
 * unit in game/gfx.c). The generated build exposes them as separate
 * br2eu_func_* trampololes into br2eu_psx_alias_body_8018E590; they
 * are the format-dispatch jump-table targets / CPS resume points for
 * printf's flag-set and argument-fetch blocks.
 *
 * Each port below transcribes the entry's own basic block(s) op-by-op
 * from generated/SLES_017.22_full_42.c (alias body). The blocks all
 * terminate by jumping into SHARED printf_core blocks (0x8018E60C
 * parse loop, 0x8018E7C8/0x8018E7B0 arg-width select, 0x8018E95C/
 * 0x8018E958 number-format core, 0x8018EBEC emit tail, 0x8018E9FC
 * char emit). Those shared blocks have no standalone clean-CPS body —
 * printf_core's clean port is a synchronous whole-function rewrite —
 * so the terminator is expressed as the same CPS tail-dispatch the
 * generated body uses: psx_check_interrupts_at(resume) + cpu->pc =
 * resume-block + return. The dispatcher then re-enters the generated
 * alias body at exactly that block (every block addr is in the
 * generated dispatch table), preserving guest $s0-$s7/$ra/$sp state
 * in cpu->gpr[]. Observably identical to the generated goto.
 *
 * Register map (MIPS): gpr[19]=s3 packed fmt flags (bit0='l' long,
 * bit2='h' short — matches printf_core's bVar5/bVar6), gpr[17]=s1 arg
 * cursor, gpr[18]=s2, gpr[22]=s6, gpr[30]=s8/fp, gpr[29]=sp,
 * gpr[2]=v0, gpr[3]=v1, gpr[5]=a1 base, gpr[7]=a3.
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
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))

/* swl_guest — unaligned word store: RMW of the aligned word,
 * byte lane selected by addr&3 (guest swl semantics). */
static void swl_guest(uint32_t addr, uint32_t v)
{
  uint32_t al = addr & ~3u;
  switch (addr & 3u) {
  case 0: w = (w & 0xFFFFFF00u) | (v >> 24); break;
  default: w = v; break;
  }
  DB32(al) = w;
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
/* CPS tail-dispatch into a shared printf_core block: identical to the
 * generated `psx_check_interrupts_at(cpu, T); goto block_T` — the check
 * runs, then control leaves this host frame with cpu->pc = T so the
 * dispatcher resumes the generated alias body at that exact block. */
static uint32_t printf_cps_dispatch(CPUState *cpu, uint32_t target)
{
  psx_check_interrupts_at(cpu, target);
  cpu->pc = target;
  return cpu->gpr[2];
}

/* ======== 0x8018e774 ======== */

/* printf_flags_h_set — 'h' length-modifier piece-entry: block is a
 * single `j 0x8018E60C` whose delay slot is `ori s3,s3,4` (set the 'h'
 * flag bit in the packed fmt-flags reg, then resume the format parse
 * loop). Disasm generated/SLES_017.22_full_42.c block_8018E774. */
uint32_t printf_flags_h_set(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[19] = cpu->gpr[19] | 0x0004;      /* delay: ori s3,s3,4 */
  return printf_cps_dispatch(cpu, 0x8018E60Cu);
}

/* ======== 0x8018e77c ======== */

/* printf_flags_l_set — 'l' length-modifier piece-entry: `j 0x8018E60C`
 * with delay `ori s3,s3,1` (set the 'l' flag bit, resume parse loop).
 * Disasm block_8018E77C. */
uint32_t printf_flags_l_set(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[19] = cpu->gpr[19] | 0x0001;      /* delay: ori s3,s3,1 */
  return printf_cps_dispatch(cpu, 0x8018E60Cu);
}

/* ======== 0x8018e784 ======== */

/* printf_arg_byte — byte-argument fetch piece-entry (7 insns):
 *   s2 = sp+16; s1 += 4; s6 = 1; v0 = lbu -4(s1);
 *   s8 = 0; [j 0x8018E9FC delay:] sb v0,16(sp)
 * Fetches the next vararg as a byte into a stack local and continues
 * at the shared char-emit block. Disasm block_8018E784. */
uint32_t printf_arg_byte(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[18] = cpu->gpr[29] + 16;          /* s2 = sp+16 */
  cpu->gpr[17] = cpu->gpr[17] + 4;           /* s1 += 4 */
  cpu->gpr[22] = 1;                          /* s6 = 1 */
  cpu->gpr[2]  = br2_ld8(cpu->gpr[17] - 4);  /* v0 = lbu -4(s1) */
  cpu->gpr[30] = cpu->gpr[0];                /* s8 = 0 (move fp,zero) */
  br2_st8(cpu->gpr[29] + 16, (uint8_t)cpu->gpr[2]); /* delay: sb v0,16(sp) */
  return printf_cps_dispatch(cpu, 0x8018E9FCu);
}

/* ======== 0x8018e7a0 ======== */

/* printf_arg_dec_l — 'l'-flagged signed-decimal argument fetch. Block
 * 0x8018E7A0 is `ori s3,s3,1` falling through into block_8018E7A4 (no
 * interrupt check on fallthrough — replicated by inlining). The shared
 * block tests the packed flags: v0 = s3&1 (saved cond), delay v0 =
 * s3&4; bne -> 0x8018E7C8 (lw arg) else -> 0x8018E7B0 (second width
 * test). Disasm blocks_8018E7A0/8018E7A4. */
uint32_t printf_arg_dec_l(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc;
  cpu->gpr[19] = cpu->gpr[19] | 0x0001;      /* ori s3,s3,1 (E7A0) */
  /* fall through to block_8018E7A4 */
  cpu->gpr[2] = cpu->gpr[19] & 0x0001;       /* andi v0,s3,1 */
  _bc = (cpu->gpr[2] != cpu->gpr[0]);        /* save branch cond */
  cpu->gpr[2] = cpu->gpr[19] & 0x0004;       /* delay: andi v0,s3,4 */
  if (_bc) {
    psx_check_interrupts_at(cpu, 0x8018E7C8u);
    cpu->pc = 0x8018E7C8u;
  } else {
    psx_check_interrupts_at(cpu, 0x8018E7B0u);
    cpu->pc = 0x8018E7B0u;
  }
  return cpu->gpr[2];
}

/* ======== 0x8018e7a4 ======== */

/* printf_arg_dec — signed-decimal argument fetch (the 'd'/'i' handler
 * block): identical flag test as printf_arg_dec_l minus the leading
 * `ori s3,s3,1`. Disasm block_8018E7A4. */
uint32_t printf_arg_dec(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc;
  cpu->gpr[2] = cpu->gpr[19] & 0x0001;       /* andi v0,s3,1 */
  _bc = (cpu->gpr[2] != cpu->gpr[0]);        /* save branch cond */
  cpu->gpr[2] = cpu->gpr[19] & 0x0004;       /* delay: andi v0,s3,4 */
  if (_bc) {
    psx_check_interrupts_at(cpu, 0x8018E7C8u);
    cpu->pc = 0x8018E7C8u;
  } else {
    psx_check_interrupts_at(cpu, 0x8018E7B0u);
    cpu->pc = 0x8018E7B0u;
  }
  return cpu->gpr[2];
}

/* ======== 0x8018e7e8 ======== */

/* printf_arg_word — word-argument fetch: same packed-flags width test
 * as printf_arg_dec but the taken/not-taken arms are 0x8018E810 (lw
 * arg) / 0x8018E7F4 (second test -> lh/lw arg), then both load the arg
 * word and a stack half/word at sp+56 before j 0x8018EBEC. Only this
 * entry block is ported; continuations tail-dispatch. Disasm
 * block_8018E7E8. */
uint32_t printf_arg_word(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc;
  cpu->gpr[2] = cpu->gpr[19] & 0x0001;       /* andi v0,s3,1 */
  _bc = (cpu->gpr[2] != cpu->gpr[0]);        /* save branch cond */
  cpu->gpr[2] = cpu->gpr[19] & 0x0004;       /* delay: andi v0,s3,4 */
  if (_bc) {
    psx_check_interrupts_at(cpu, 0x8018E810u);
    cpu->pc = 0x8018E810u;
  } else {
    psx_check_interrupts_at(cpu, 0x8018E7F4u);
    cpu->pc = 0x8018E7F4u;
  }
  return cpu->gpr[2];
}

/* ======== 0x8018e824 ======== */

/* printf_arg_oct_l — 'l'-flagged octal argument fetch: `ori s3,s3,1`
 * (0x8018E824) falls through into block_8018E828 (no check on
 * fallthrough — inlined). The shared block runs the same flags width
 * test, branching to 0x8018E84C (lw arg, base 8) / 0x8018E834
 * (second test). Disasm blocks_8018E824/8018E828. */
uint32_t printf_arg_oct_l(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc;
  cpu->gpr[19] = cpu->gpr[19] | 0x0001;      /* ori s3,s3,1 (E824) */
  /* fall through to block_8018E828 */
  cpu->gpr[2] = cpu->gpr[19] & 0x0001;       /* andi v0,s3,1 */
  _bc = (cpu->gpr[2] != cpu->gpr[0]);        /* save branch cond */
  cpu->gpr[2] = cpu->gpr[19] & 0x0004;       /* delay: andi v0,s3,4 */
  if (_bc) {
    psx_check_interrupts_at(cpu, 0x8018E84Cu);
    cpu->pc = 0x8018E84Cu;
  } else {
    psx_check_interrupts_at(cpu, 0x8018E834u);
    cpu->pc = 0x8018E834u;
  }
  return cpu->gpr[2];
}

/* ======== 0x8018e828 ======== */

/* printf_arg_oct — octal argument fetch ('o' handler block): same
 * flags width test as printf_arg_dec, branching to 0x8018E84C /
 * 0x8018E834 where a1=8 selects base 8. Disasm block_8018E828. */
uint32_t printf_arg_oct(void)
{
  CPUState *cpu = clean_cpu_state();
  int _bc;
  cpu->gpr[2] = cpu->gpr[19] & 0x0001;       /* andi v0,s3,1 */
  _bc = (cpu->gpr[2] != cpu->gpr[0]);        /* save branch cond */
  cpu->gpr[2] = cpu->gpr[19] & 0x0004;       /* delay: andi v0,s3,4 */
  if (_bc) {
    psx_check_interrupts_at(cpu, 0x8018E84Cu);
    cpu->pc = 0x8018E84Cu;
  } else {
    psx_check_interrupts_at(cpu, 0x8018E834u);
    cpu->pc = 0x8018E834u;
  }
  return cpu->gpr[2];
}

/* ======== 0x8018e85c ======== */

/* printf_arg_hex — hex argument fetch ('x'/'X' handler block, 5
 * insns): s1 += 4; a3(v1) = lw -4(s1); a1 = 16; j 0x8018E958 — always
 * fetches the full word arg and selects base 16, no flags width test.
 * Disasm block_8018E85C. */
uint32_t printf_arg_hex(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[17] = cpu->gpr[17] + 4;           /* s1 += 4 */
  cpu->gpr[3]  = br2_ld32(cpu->gpr[17] - 4); /* v1 = lw -4(s1) */
  cpu->gpr[5]  = 16;                         /* a1 = 16 (base) */
  return printf_cps_dispatch(cpu, 0x8018E958u);
}
