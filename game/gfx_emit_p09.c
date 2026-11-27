/* gfx_emit_p09.c — clean-room ports of the 0x8018F6xx-0x8018FFxx emit
 * family (generated/SLES_017.22_full_43.c). MDEC param-block commit /
 * restore pair, a GPU packet field getter, a VRAM span cell update
 * (CPS-split pair), and three CPS piece-entries into the 0x8018EF6C
 * text-engine body (routed to the existing cps_8018ef6c transliteration
 * via clean_cps.map — the generated alias stubs dispatch on `entry`
 * exactly like the clean CPS contract dispatches on cpu->pc).
 *
 * Disasm verified insn-by-insn against generated/SLES_017.22_full_43.c
 * (br2eu_func_8018F884 / _F910 / _F9A8 / _FEE0 / _FF00) and the
 * br2eu_psx_alias_body_8018EF6C entry switch. */

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

#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))

/* swl_guest — unaligned word store: RMW of the aligned word,
 * byte lane selected by addr&3 (guest swl semantics). */
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

/* prim_ot_link_indexed — ordering-table link: slot = ot +
 * (otz&0x1ffc); writes out<<8 into the slot and back-links the
 * old head into out+2. */
static void prim_ot_link_indexed(uint32_t ot, uint32_t out, uint32_t otz)
{
  uint32_t slot = ot + (otz & 0x1ffcu);
  uint32_t old  = DB32(slot);
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
}

/* ======== 0x8018f884 ======== */

/* mdec_parambuf_restore — read back the 256-byte MDEC parameter state
 * that mdec_parambuf_commit (0x8018F910) saved, into the caller's a0
 * buffer; returns a0.
 *
 * Three copy loops, all `lw/sw` word streams with the loop counter in
 * $v1 (15 -> -1 = 16 words, then 31 -> -1 = 32 words) against the -1
 * sentinel in $a3:
 *   dst a0+0    <- 0x801B67B4, 16 words (64B)
 *   dst a0+64   <- 0x801B67F4, 16 words (64B)
 *   dst a0+128  <- 0x801B6838, 32 words (128B)
 * The third source skips the 4-byte command-block header at 0x801B6834
 * (the second mdec_decode_issue block address). Leaf: internal loop
 * branches stay inside the function — no CPS splits, no jal. */
uint32_t mdec_parambuf_restore(uint32_t a0)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0;

block_8018F884:
  cpu->gpr[6] = cpu->gpr[4];                          /* a2 = a0 (dst) */
  cpu->gpr[5] = 0x801B << 16;                         /* a1 = 0x801B0000 */
  cpu->gpr[5] = cpu->gpr[5] + 26548u;                 /* a1 = 0x801B67B4 */
  cpu->gpr[3] = 15u;                                  /* v1 = 15 */
  cpu->gpr[7] = 0xFFFFFFFFu;                          /* a3 = -1 */
  /* fall through */

block_8018F898:
  cpu->gpr[2] = br2_ld32(cpu->gpr[5]);                /* lw v0,0(a1) */
  cpu->gpr[5] = cpu->gpr[5] + 4u;                     /* a1 += 4 */
  cpu->gpr[3] = cpu->gpr[3] + 0xFFFFFFFFu;            /* v1-- */
  br2_st32(cpu->gpr[6], cpu->gpr[2]);                 /* sw v0,0(a2) */
  { int _bc = (cpu->gpr[3] != cpu->gpr[7]);           /* bne v1,a3 */
    cpu->gpr[6] = cpu->gpr[6] + 4u;                   /* delay: a2 += 4 */
    if (_bc) goto block_8018F898; else goto block_8018F8B0; }

block_8018F8B0:
  cpu->gpr[6] = cpu->gpr[4] + 64u;                    /* a2 = a0+64 */
  cpu->gpr[5] = 0x801B << 16;
  cpu->gpr[5] = cpu->gpr[5] + 26612u;                 /* a1 = 0x801B67F4 */
  cpu->gpr[3] = 15u;
  cpu->gpr[7] = 0xFFFFFFFFu;
  /* fall through */

block_8018F8C4:
  cpu->gpr[2] = br2_ld32(cpu->gpr[5]);
  cpu->gpr[5] = cpu->gpr[5] + 4u;
  cpu->gpr[3] = cpu->gpr[3] + 0xFFFFFFFFu;
  br2_st32(cpu->gpr[6], cpu->gpr[2]);
  { int _bc = (cpu->gpr[3] != cpu->gpr[7]);
    cpu->gpr[6] = cpu->gpr[6] + 4u;
    if (_bc) goto block_8018F8C4; else goto block_8018F8DC; }

block_8018F8DC:
  cpu->gpr[6] = cpu->gpr[4] + 128u;                   /* a2 = a0+128 */
  cpu->gpr[5] = 0x801B << 16;
  cpu->gpr[5] = cpu->gpr[5] + 26680u;                 /* a1 = 0x801B6838 */
  cpu->gpr[3] = 31u;                                  /* v1 = 31 (32 words) */
  cpu->gpr[7] = 0xFFFFFFFFu;
  /* fall through */

block_8018F8F0:
  cpu->gpr[2] = br2_ld32(cpu->gpr[5]);
  cpu->gpr[5] = cpu->gpr[5] + 4u;
  cpu->gpr[3] = cpu->gpr[3] + 0xFFFFFFFFu;
  br2_st32(cpu->gpr[6], cpu->gpr[2]);
  { int _bc = (cpu->gpr[3] != cpu->gpr[7]);
    cpu->gpr[6] = cpu->gpr[6] + 4u;
    if (_bc) goto block_8018F8F0; else goto block_8018F908; }

block_8018F908:
  cpu->gpr[2] = cpu->gpr[4];                          /* v0 = a0 (delay: ret) */
  return cpu->gpr[2];
}

/* ======== 0x8018f910 ======== */

/* mdec_parambuf_commit — store the caller's 128-byte parameter header
 * into the MDEC save area, then issue the two pending MDEC decodes.
 * Clean CPS coroutine (guest 0x8018F910-0x8018F9A7, 7 blocks); entry +
 * all 6 continuation addrs registered in clean_cps.map.
 *
 *   prologue : sp -= 24, save s0/ra to the guest frame (sp persists
 *              across yields — CPS entries own gpr[29] end-to-end)
 *   F930 loop: copy 16 words [s0+0..64)   -> 0x801B67B4
 *   F95C loop: copy 16 words [s0+64..128) -> 0x801B67F4
 *   F974     : a0=0x801B67B0, a1=32, ra=F984, jal mdec_decode_issue
 *   F984     : a0=0x801B6834, a1=32, ra=F994, jal mdec_decode_issue
 *   F994     : v0 = s0 (returns the caller's buffer), restore s0/ra,
 *              sp += 24, jr ra
 *
 * Both jals are CPS tail-dispatches ($ra = continuation,
 * psx_check_interrupts_at, cpu->pc = callee, return) exactly like the
 * generated body — a yield inside mdec_decode_issue resumes via
 * clean_cps.map -> this body's pc-switch. */
uint32_t mdec_parambuf_commit(void)
{
  extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  if (!c) return 0;
  pc = c->pc;
  if (pc != 0u && pc != 0x8018F910u) {
    switch (pc) {
    case 0x8018F930u: goto b930;
    case 0x8018F948u: goto b948;
    case 0x8018F95Cu: goto b95C;
    case 0x8018F974u: goto b974;
    case 0x8018F984u: goto b984;
    case 0x8018F994u: goto b994;
    default: break;
    }
  }

  /* block_8018F910 — entry: build frame, prime first copy loop */
  c->gpr[29] = c->gpr[29] - 24u;                     /* sp -= 24 */
  br2_st32(c->gpr[29] + 16u, c->gpr[16]);            /* save s0 */
  c->gpr[16] = c->gpr[4];                            /* s0 = a0 */
  c->gpr[5]  = 0x801B << 16;                         /* a1 = 0x801B0000 */
  c->gpr[5]  = c->gpr[5] + 26548u;                   /* a1 = 0x801B67B4 */
  c->gpr[3]  = 15u;                                  /* v1 = 15 */
  c->gpr[6]  = 0xFFFFFFFFu;                          /* a2 = -1 */
  br2_st32(c->gpr[29] + 20u, c->gpr[31]);            /* save ra */
  /* fall through */

b930:
  c->gpr[2] = br2_ld32(c->gpr[4]);                   /* lw v0,0(a0) */
  c->gpr[4] = c->gpr[4] + 4u;                        /* a0 += 4 */
  c->gpr[3] = c->gpr[3] + 0xFFFFFFFFu;               /* v1-- */
  br2_st32(c->gpr[5], c->gpr[2]);                    /* sw v0,0(a1) */
  { int _bc = (c->gpr[3] != c->gpr[6]);              /* bne v1,a2 */
    c->gpr[5] = c->gpr[5] + 4u;                      /* delay: a1 += 4 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018F930u);
      goto b930;
    } else {
      psx_check_interrupts_at(c, 0x8018F948u);
      goto b948;
    } }

b948:
  c->gpr[5] = 0x801B << 16;
  c->gpr[5] = c->gpr[5] + 26612u;                    /* a1 = 0x801B67F4 */
  c->gpr[4] = c->gpr[16] + 64u;                      /* a0 = s0+64 */
  c->gpr[3] = 15u;
  c->gpr[6] = 0xFFFFFFFFu;
  /* fall through */

b95C:
  c->gpr[2] = br2_ld32(c->gpr[4]);                   /* lw v0,0(a0) */
  c->gpr[4] = c->gpr[4] + 4u;
  c->gpr[3] = c->gpr[3] + 0xFFFFFFFFu;
  br2_st32(c->gpr[5], c->gpr[2]);                    /* sw v0,0(a1) */
  { int _bc = (c->gpr[3] != c->gpr[6]);
    c->gpr[5] = c->gpr[5] + 4u;
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018F95Cu);
      goto b95C;
    } else {
      psx_check_interrupts_at(c, 0x8018F974u);
      goto b974;
    } }

b974:
  c->gpr[4]  = 0x801B << 16;
  c->gpr[4]  = c->gpr[4] + 26544u;                   /* a0 = 0x801B67B0 */
  c->gpr[31] = 0x8018F984u;                          /* jal link */
  c->gpr[5]  = 32u;                                  /* delay: a1 = 32 */
  psx_check_interrupts_at(c, 0x8018FC00u);           /* jal mdec_decode_issue */
  c->pc = 0x8018FC00u;
  return c->gpr[2];

b984:
  c->gpr[4]  = 0x801B << 16;
  c->gpr[4]  = c->gpr[4] + 26676u;                   /* a0 = 0x801B6834 */
  c->gpr[31] = 0x8018F994u;                          /* jal link */
  c->gpr[5]  = 32u;                                  /* delay: a1 = 32 */
  psx_check_interrupts_at(c, 0x8018FC00u);           /* jal mdec_decode_issue */
  c->pc = 0x8018FC00u;
  return c->gpr[2];

b994:
  c->gpr[2]  = c->gpr[16];                           /* v0 = s0 (ret buf) */
  c->gpr[31] = br2_ld32(c->gpr[29] + 20u);           /* restore ra */
  c->gpr[16] = br2_ld32(c->gpr[29] + 16u);           /* restore s0 */
  { uint32_t _jt = c->gpr[31];                       /* jr $ra */
    c->gpr[29] = c->gpr[29] + 24u;                   /* delay: sp += 24 */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt;
    return c->gpr[2]; }
}

/* ======== 0x8018f9a8 ======== */

/* gpu_pkt_field2_get — return the unsigned halfword at a0+2 (lhu,
 * zero-extended into $v0). Sits between gpu_pkt_attr_set (0x8018F9B4)
 * and the file-op family; the packet's +2 field is read here while
 * attr_set consumes the +0 halfword as the decode size. Leaf: single
 * load + jr ra. */
uint32_t gpu_pkt_field2_get(uint32_t a0)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0;

  cpu->gpr[2] = (uint32_t)br2_ld16(cpu->gpr[4] + 2u); /* lhu v0,2(a0) */
  return cpu->gpr[2];
}

/* ======== 0x8018fee0 ======== */

/* vram_span_set — read-modify-write the span cell at 0x801B6900 and
 * return its PREVIOUS value in $v0 (the `lw v0,0(t0)` delay-slot load
 * runs on both paths, so v0 is always the old cell content).
 *
 *   t0 = 0x801B6900
 *   at = a0 - 1
 *   v0 = *t0                     (delay slot of blez)
 *   if (a0 - 1 <= 0)  -> CPS split to 0x8018FF00:
 *                        *t0 = 0x00FFFFFF
 *   else              -> *t0 = a0 << 1
 *
 * The taken branch is a generated CPS split (cpu->pc = 0x8018FF00,
 * return), not a jal — entry + continuation 0x8018FEF4 are registered
 * in clean_cps.map so a yield resumes inside this body and the split
 * target re-dispatches to vram_span_set_max. */
uint32_t vram_span_set(uint32_t a0)
{
  extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  (void)a0;
  if (!c) return 0;
  pc = c->pc;
  if (pc != 0u && pc != 0x8018FEE0u) {
    switch (pc) {
    case 0x8018FEF4u: goto bEF4;
    default: break;
    }
  }

  /* block_8018FEE0 — entry */
  c->gpr[8] = 0x801B << 16;                         /* t0 = 0x801B0000 */
  c->gpr[8] = c->gpr[8] + 26880u;                   /* t0 = 0x801B6900 */
  c->gpr[1] = c->gpr[4] + 0xFFFFFFFFu;              /* at = a0 - 1 */
  c->gpr[2] = br2_ld32(c->gpr[8]);                  /* delay: v0 = *t0 (old) */
  { int _bc = ((int32_t)c->gpr[1] <= 0);            /* blez at */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018FF00u);
      c->pc = 0x8018FF00u;                          /* CPS split -> ff00 */
      return c->gpr[2];
    } else {
      psx_check_interrupts_at(c, 0x8018FEF4u);
      goto bEF4;
    } }

bEF4:
  c->gpr[1] = c->gpr[4] << 1;                       /* at = a0 << 1 */
  br2_st32(c->gpr[8], c->gpr[1]);                   /* delay: sw at,0(t0) */
  return c->gpr[2];                                 /* v0 = old cell value */
}

/* ======== 0x8018ff00 ======== */

/* vram_span_set_max — the a0 <= 1 arm of vram_span_set reached by CPS
 * split: store 0x00FFFFFF into the span cell at [t0] ($t0 must hold
 * 0x801B6900 — set by vram_span_set's prologue or by the caller when
 * this addr is dispatched directly) and return. $v0 keeps whatever
 * the caller left (vram_span_set's delay-slot load already filled it
 * with the old cell value). Leaf: jr ra only. */
uint32_t vram_span_set_max(void)
{
  CPUState *cpu = clean_cpu_state();

  cpu->gpr[1] = 0x00FFu << 16;                      /* at = 0x00FF0000 */
  cpu->gpr[1] = cpu->gpr[1] | 0xFFFFu;              /* at = 0x00FFFFFF */
  br2_st32(cpu->gpr[8], cpu->gpr[1]);               /* delay: sw at,0(t0) */
  return cpu->gpr[2];
}
