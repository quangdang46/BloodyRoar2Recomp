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

/* ======== dbg_marker_mode_set CPS pieces (parent 0x8018BABC, already
 * clean-ported as dbg_marker_mode_set in game/save.c) ======== */

/* dbg_marker_mode_set_clear — guest 0x8018BB4C. Interior clear-loop piece:
 * zeroes the 2x2 countdown byte pairs at 0x801f596c / 0x801f5970 (outer
 * slot index a2 in {0,1} -> byte offset a2*2, inner index a1 in {0,1}),
 * then stores a0 to the flag byte 0x801b55d0 and returns with
 * v0 = 0x801b0000. Reached via CPS splits from 0x8018BAE0 (a0==0) and
 * 0x8018BB48 (a0==2, which first clears a2). Disasm
 * generated/SLES_017.22_full_41.c br2eu_func_8018BB4C. */
uint32_t dbg_marker_mode_set_clear(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_8018BB4C:
  cpu->gpr[2] = 0x801F0000;                          /* lui v0,0x801f */
  cpu->gpr[9] = cpu->gpr[2] + 22892;                 /* t1 = 0x801f596c */
  cpu->gpr[2] = 0x801F0000;                          /* lui v0,0x801f */
  cpu->gpr[8] = cpu->gpr[2] + 22896;                 /* t0 = 0x801f5970 */
  cpu->gpr[5] = cpu->gpr[0];                         /* move a1,zero */
  /* fall through */

block_8018BB60:
  cpu->gpr[7] = cpu->gpr[6] << 1;                    /* s3 = a2 << 1 */
  cpu->gpr[2] = cpu->gpr[5] + cpu->gpr[7];           /* v0 = a1 + a2*2 */
  /* fall through */

block_8018BB68:
  cpu->gpr[5] = cpu->gpr[5] + 1;                     /* addiu a1,a1,1 */
  cpu->gpr[3] = cpu->gpr[2] + cpu->gpr[9];           /* v1 = 596c + idx */
  cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[8];           /* v0 = 5970 + idx */
  br2_st8(cpu->gpr[3], (uint8_t)cpu->gpr[0]);        /* sb zero,(v1) */
  br2_st8(cpu->gpr[2], (uint8_t)cpu->gpr[0]);        /* sb zero,(v0) */
  cpu->gpr[2] = ((int32_t)cpu->gpr[5] < 2) ? 1 : 0;  /* slti v0,a1,2 */
  { int _bc = (cpu->gpr[2] != cpu->gpr[0]);          /* bnez -> BB68 */
    cpu->gpr[2] = cpu->gpr[5] + cpu->gpr[7];         /* delay: v0 = a1+a2*2 */
    if (_bc) goto block_8018BB68; else goto block_8018BB88; }

block_8018BB88:
  cpu->gpr[6] = cpu->gpr[6] + 1;                     /* addiu a2,a2,1 */
  cpu->gpr[2] = ((int32_t)cpu->gpr[6] < 2) ? 1 : 0;  /* slti v0,a2,2 */
  { int _bc = (cpu->gpr[2] != cpu->gpr[0]);          /* bnez -> BB60 */
    cpu->gpr[5] = cpu->gpr[0];                       /* delay: move a1,zero */
    if (_bc) goto block_8018BB60; else goto block_8018BB98; }

block_8018BB98:
  cpu->gpr[2] = 0x801B0000;                          /* lui v0,0x801b */
  br2_st8(cpu->gpr[2] + 21968, (uint8_t)cpu->gpr[4]);/* sb a0,0x55d0(v0) */
  return cpu->gpr[2];                                /* jr ra (nop delay) */
}

/* dbg_marker_mode_set_clr_gate — guest 0x8018BB48. Single-instruction CPS
 * piece: move a2,zero then falls through into 0x8018BB4C. Disasm
 * generated/SLES_017.22_full_41.c br2eu_func_8018BB48. */
uint32_t dbg_marker_mode_set_clr_gate(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;
  cpu->gpr[6] = cpu->gpr[0];                         /* move a2,zero */
  return dbg_marker_mode_set_clear(0, 0, 0, 0);      /* fallthrough -> BB4C */
}

/* dbg_marker_mode_set_dispatch — guest 0x8018BAF0. Mode dispatcher piece
 * of dbg_marker_mode_set: a0==2 -> 0x8018BB48 (clear with a2=0); a0==3 ->
 * 0x8018BB04 default-slot init then 0x8018BB38 flag set; anything else ->
 * 0x8018BAE8 (jr ra). The BB04 stores use live a2 (gpr[6]) for bytes
 * 0x801f5971/0x801f5973 — 1 when reached via the 0x8018BABC parent
 * (its prologue sets a2=1), caller-supplied otherwise. Disasm
 * generated/SLES_017.22_full_41.c br2eu_func_8018BAF0. */
uint32_t dbg_marker_mode_set_dispatch(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_8018BAF0:
  cpu->gpr[2] = 2;                                   /* li v0,2 */
  { int _bc = (cpu->gpr[4] == cpu->gpr[2]);          /* beq a0,v0 -> BB48 */
    cpu->gpr[2] = 3;                                 /* delay: li v0,3 */
    if (_bc) goto block_8018BB48; else goto block_8018BAFC; }

block_8018BB48:
  cpu->gpr[6] = cpu->gpr[0];                         /* move a2,zero */
  return dbg_marker_mode_set_clear(0, 0, 0, 0);      /* fallthrough -> BB4C */

block_8018BAFC:
  cpu->gpr[3] = 0x801F0000;                          /* delay: lui v1,0x801f */
  { int _bc = (cpu->gpr[4] != cpu->gpr[2]);          /* bne a0,v0 -> BAE8 */
    if (_bc) return cpu->gpr[2];                     /* BAE8: jr ra, v0=3 */
    else goto block_8018BB04; }

block_8018BB04:
  cpu->gpr[4] = 30;                                  /* li a0,30 */
  cpu->gpr[2] = 0x801F0000;                          /* lui v0,0x801f */
  cpu->gpr[5] = 64;                                  /* li a1,64 */
  br2_st8(cpu->gpr[3] + 22892, (uint8_t)cpu->gpr[4]);/* sb a0,0x596c(v1) */
  cpu->gpr[3] = cpu->gpr[3] + 22892;
  br2_st8(cpu->gpr[2] + 22896, (uint8_t)cpu->gpr[5]);/* sb a1,0x5970(v0) */
  cpu->gpr[2] = cpu->gpr[2] + 22896;
  br2_st8(cpu->gpr[3] + 1, (uint8_t)cpu->gpr[4]);    /* sb a0,1(v1) */
  br2_st8(cpu->gpr[2] + 1, (uint8_t)cpu->gpr[6]);    /* sb a2,1(v0) */
  br2_st8(cpu->gpr[3] + 2, (uint8_t)cpu->gpr[4]);    /* sb a0,2(v1) */
  br2_st8(cpu->gpr[2] + 2, (uint8_t)cpu->gpr[5]);    /* sb a1,2(v0) */
  br2_st8(cpu->gpr[3] + 3, (uint8_t)cpu->gpr[4]);    /* sb a0,3(v1) */
  br2_st8(cpu->gpr[2] + 3, (uint8_t)cpu->gpr[6]);    /* sb a2,3(v0) */
  /* fallthrough 0x8018BB38 (flag_801b55d0_set): sb 1 -> 0x801b55d0, jr ra */
  cpu->gpr[3] = 0x801B0000;                          /* lui v1,0x801b */
  cpu->gpr[2] = 1;                                   /* li v0,1 */
  br2_st8(cpu->gpr[3] + 21968, (uint8_t)cpu->gpr[2]);/* sb v0,0x55d0(v1) */
  return cpu->gpr[2];                                /* jr ra (nop delay) */
}

/* ======== pad_slot_state_walk CPS piece entries (parent 0x8018BBA8,
 * already clean-ported as pad_slot_state_walk in game/menu.c) ========
 *
 * The five addresses below are interior CPS resume points inside
 * pad_slot_state_walk: the generated alias stubs
 * (br2eu_psx_alias_body_8018BBA8) re-enter the parent body at that block
 * with the live CPS register/frame image. The clean pad_slot_state_walk
 * has no CPS frame — it runs the whole switch atomically — so these PCs
 * are unreachable while the parent stays clean, and no external jal/j
 * targets them (verified: only the parent's own psx_check_interrupts_at
 * splits reference these addresses). Faithful treatment per AGENTS.md §4:
 * self-delegate so gen_clean_table leaves them generated-authoritative;
 * if the generated parent ever resumes at one of these PCs the exact
 * generated piece body runs. */

/* 0x8018BC2C — pad_slot_state_walk case-6 body (flag==0 -> field_get /
 * set_fwd / cmd_fwd init sequence, then flag=1). */
uint32_t pad_slot_walk_case6_resume(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018BC2Cu, a0, a1, a2, a3);
}

/* 0x8018BC78 — pad_slot_state_walk case4/5 body (first countdown byte
 * of the s0a/s1a pair: nonzero -> decrement, zero -> clear pair flag). */
uint32_t pad_slot_walk_decay_resume(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018BC78u, a0, a1, a2, a3);
}

/* 0x8018BCB4 — pad_slot_state_walk case-2 body (clear all four bytes,
 * flag=1). */
uint32_t pad_slot_walk_case2_resume(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018BCB4u, a0, a1, a2, a3);
}

/* 0x8018BCD0 — pad_slot_state_walk case0/1 body (clear all four bytes,
 * flag=0). */
uint32_t pad_slot_walk_case01_resume(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018BCD0u, a0, a1, a2, a3);
}

/* 0x8018BCE4 — pad_slot_state_walk loop tail (s3 += 0x10, s2a/s0a/s1a +=
 * stride, loop while s2a < 0x801f596a, else epilogue). */
uint32_t pad_slot_walk_tail_resume(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018BCE4u, a0, a1, a2, a3);
}

/* ======== 0x8018C0F8 ======== */

/* spr_pool_b_commit — guest 0x8018C0F8. CPS tail-split piece of
 * spr_pool_b_alloc (0x8018C0A8): reached when the sentinel scan finds a
 * free 0x2c-stride slot in the 0x801f5ed8 pool (a3 = 0x801b0000 base).
 * v0 = (v1 < 32); a0 = a2 (slot address); if v1 >= 32 the stored index is
 * cleared to 0. Then DAT_801b55dc (a3+0x55dc) = v1, *a0 = a1 (sentinel),
 * and v0 = a0 + 4 is returned — the caller-visible slot+4 pointer.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018C0F8. */
uint32_t spr_pool_b_commit(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_8018C0F8:
  cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 32) ? 1 : 0;  /* slti v0,v1,32 */
  cpu->gpr[4] = cpu->gpr[6];                        /* delay: move a0,a2 */
  if (cpu->gpr[2] != cpu->gpr[0]) goto block_8018C108;
  /* else fall into */

block_8018C104:
  cpu->gpr[3] = cpu->gpr[0];                        /* move v1,zero */
  /* fall through */

block_8018C108:
  cpu->gpr[2] = cpu->gpr[4] + 4;                    /* addiu v0,a0,4 */
  br2_st32(cpu->gpr[7] + 21980, cpu->gpr[3]);       /* sw v1,0x55dc(a3) */
  br2_st32(cpu->gpr[4], cpu->gpr[5]);               /* jr delay: sw a1,0(a0) */
  return cpu->gpr[2];                               /* jr ra */
}
