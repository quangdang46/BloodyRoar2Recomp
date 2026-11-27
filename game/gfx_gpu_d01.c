#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

extern uint32_t br2_call_generated(uint32_t addr, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);

#define DB16(a) (*(uint16_t *)guest_at(a))
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
