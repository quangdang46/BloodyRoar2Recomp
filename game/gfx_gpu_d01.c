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

/*
 * These entry points are split pieces of guest CPS routines. Their live
 * register state and interrupt resume PCs are part of the calling contract,
 * so dispatch the complete authoritative body rather than flattening a block
 * into an ordinary C call.
 */
uint32_t text_format_copy_byte_fragment(uint32_t a0, uint32_t a1,
                                        uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x80190ca4u, a0, a1, a2, a3);
}

uint32_t text_format_measure_string_fragment(uint32_t a0, uint32_t a1,
                                             uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x80190cc4u, a0, a1, a2, a3);
}

uint32_t text_format_width_check_fragment(uint32_t a0, uint32_t a1,
                                          uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x80190ce4u, a0, a1, a2, a3);
}

uint32_t draw_env_field_compare_fragment(uint32_t a0, uint32_t a1,
                                         uint32_t a2, uint32_t a3)
{
  return br2_call_generated(0x80191a78u, a0, a1, a2, a3);
}
