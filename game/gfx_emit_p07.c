/* gfx_emit_p07.c — clean-room ports for guest 0x8018E86C-0x8018F220.
 *
 * NOTE: despite the emit-worklist label, this range is NOT GTE/OT emitter
 * code — it is the tail of the printf/sprintf engine. All nine addrs are
 * CPS piece/alias entries into two large coroutine bodies:
 *
 *   - 0x8018E86C/0x8018E8CC/0x8018E8D0/0x8018E904/0x8018E910/0x8018EBC0/
 *     0x8018EBD4  -> br2eu_psx_alias_body_8018E590, alternate entries into
 *     printf_core (guest 0x8018E590). Each lands mid-body on live s-regs
 *     and sp-frame state (e.g. 0x8018E8CC is a lone `ori s3,s3,1` that
 *     falls into the flag-parse blocks).
 *   - 0x8018EF6C  -> br2eu_func_8018EF6C, the sprintf engine core
 *     (prologue `addiu sp,sp,-584`, 121 blocks / 2164 bytes).
 *     sprintf_core_guest (0x8018EF60) spills a1-a3 and falls into it.
 *   - 0x8018F220  -> br2eu_psx_alias_body_8018EF6C, a mid-piece alias
 *     entry into the same engine (`lw v0,528(sp)` — live frame state).
 *
 * Every basic-block boundary in these bodies carries psx_check_interrupts_at
 * yield checks, and the pieces read/write live coroutine registers — the
 * generated CPS frame supplies the resume-PC + register contract that clean
 * C cannot reproduce (AGENTS.md §4). Each body is therefore delegated whole
 * to the generated implementation; gen_clean_table.py skips self-delegating
 * thunks so dispatch continues to route these addrs to generated code
 * unchanged. Named here per the delegation rule so the addresses have a
 * clean-tree identity.
 */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

/* ---- printf_core (0x8018E590) CPS piece entries — delegated ---- */

/* 0x8018E86C: mid-body entry — s1+=4; s2=lw -4(s1); flag test chain. */
uint32_t printf_core_piece_e86c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018e86cu, a0, a1, a2, a3);
}

/* 0x8018E8CC: single-insn piece — ori s3,s3,0x1 then flag-parse fallthrough. */
uint32_t printf_core_piece_e8cc(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018e8ccu, a0, a1, a2, a3);
}

/* 0x8018E8D0: andi v0,s3,1 / s3,4 flag tests — alternate entry one insn later. */
uint32_t printf_core_piece_e8d0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018e8d0u, a0, a1, a2, a3);
}

/* 0x8018E904: lui/addiu a3 = 0x80124FFC-class constant store to sp+72. */
uint32_t printf_core_piece_e904(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018e904u, a0, a1, a2, a3);
}

/* 0x8018E910: andi v0,s3,1 flag test — alternate entry into same parse. */
uint32_t printf_core_piece_e910(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018e910u, a0, a1, a2, a3);
}

/* 0x8018EBC0: jal console_pct(0x8018EDEC) piece — emit-percent tail. */
uint32_t printf_core_piece_ebc0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018ebc0u, a0, a1, a2, a3);
}

/* 0x8018EBD4: lw a3,56(sp); lb a0,0(s7) — format-char advance piece. */
uint32_t printf_core_piece_ebd4(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018ebd4u, a0, a1, a2, a3);
}

/* ---- sprintf engine core (0x8018EF6C, 121 blocks) — delegated ---- */

/* 0x8018EF6C: engine entry — sp-=584 frame, s3=a0(dst), a1=fmt char walk. */
uint32_t sprintf_core_engine(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018ef6cu, a0, a1, a2, a3);
}

/* 0x8018F220: mid-piece alias entry — lw v0,528(sp); ori v0,v0,32. */
uint32_t sprintf_core_engine_piece_f220(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x8018f220u, a0, a1, a2, a3);
}
