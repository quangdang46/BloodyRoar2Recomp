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

extern undefined2 *euler_rotmat(uint a0, uint a1, uint a2, undefined2 *a3);
extern uint *rotmat_rtps_build(uint a0, uint *a1);
extern uint *rotmat2_pair_build(uint a0, uint a1, uint *a2);
extern uint *angle_vec_build(uint a0, uint *a1);

undefined2 *euler_rotmat_unpack(uint32_t src, undefined2 *out)
{
  uint32_t w = br2_ld32(src);
  int32_t a3 = (int32_t)(int16_t)br2_ld16(src + 4);
  return euler_rotmat(w, (uint32_t)((int32_t)w >> 16), (uint32_t)a3, out);
}

undefined2 *euler_rotmat_neg(uint32_t src, undefined2 *out)
{
  uint32_t w = br2_ld32(src);
  int32_t a3 = (int32_t)(int16_t)br2_ld16(src + 4);
  int32_t a2 = (int32_t)w >> 16;
  return euler_rotmat(w, (uint32_t)(0 - a2), (uint32_t)(0 - a3), out);
}

undefined2 *euler_rotmat_core(uint32_t a0, uint32_t a1, uint32_t a2, undefined2 *out)
{
  return euler_rotmat(a0, a1, a2, out);
}

uint *rotmat_rtps_build_half(uint32_t src, uint *out)
{
  uint32_t a0 = (uint32_t)(int32_t)(int16_t)br2_ld16(src + 2);
  return rotmat_rtps_build(a0, out);
}

uint *rotmat2_pair_build_word(uint32_t src, uint *out)
{
  uint32_t w = br2_ld32(src);
  return rotmat2_pair_build(w, (uint32_t)((int32_t)w >> 16), out);
}

uint *angle_vec_build_half(uint32_t src, uint *out)
{
  uint32_t a0 = (uint32_t)(int32_t)(int16_t)br2_ld16(src + 4);
  return angle_vec_build(a0, out);
}

static int32_t lut_half(uint32_t off)
{
  return (int32_t)(int16_t)br2_ld16(0x801247a8u + off);
}

static void rotmat_lut_pair(uint32_t x, int32_t *first, int32_t *second)
{
  uint32_t i0 = (x & 0x3ffu) * 2u;
  uint32_t i1 = 0u - i0 + 0x800u;
  uint32_t raw = x - 0x400u;
  int32_t a;
  int32_t b;
  if (x & 0x400u) {
    uint32_t t = i0;
    i0 = i1;
    i1 = t;
  }
  a = lut_half(i0);
  if (x & 0x800u) a = -a;
  b = lut_half(i1);
  if (raw & 0x800u) b = -b;
  *first = a;
  *second = b;
}

static int32_t mul_q12(int32_t a, int32_t b)
{
  return (int32_t)((int64_t)a * (int64_t)b) >> 12;
}

uint *rotmat_half_euler_a(uint32_t src, uint *out)
{
  int32_t raw1 = (int32_t)(int16_t)br2_ld16(src);
  int32_t raw2 = (int32_t)(int16_t)br2_ld16(src + 4);
  uint32_t x1 = (uint32_t)raw1 + 0x400u;
  uint32_t x2 = (uint32_t)raw2 + 0x400u;
  int32_t t2, t3, t4, t5;
  uint16_t *h = (uint16_t *)out;
  rotmat_lut_pair(x1, &t2, &t3);
  rotmat_lut_pair(x2, &t4, &t5);
  h[8] = (uint16_t)t2;
  h[3] = (uint16_t)t5;
  h[4] = (uint16_t)mul_q12(t4, t2);
  h[0] = (uint16_t)t4;
  h[7] = (uint16_t)t3;
  {
    int32_t n5 = -t5;
    int32_t n3 = -t3;
    h[1] = (uint16_t)mul_q12(n5, t2);
    h[5] = (uint16_t)mul_q12(t4, n3);
    h[6] = 0;
    h[2] = (uint16_t)mul_q12(n5, n3);
  }
  return out;
}

uint *rotmat_half_euler_b(uint32_t src, uint *out)
{
  int32_t raw1 = (int32_t)(int16_t)br2_ld16(src);
  int32_t raw2 = (int32_t)(int16_t)br2_ld16(src + 4);
  uint32_t x1 = (uint32_t)raw1 + 0x400u;
  uint32_t x2 = (uint32_t)raw2 + 0x400u;
  int32_t t2, t3, t4, t5;
  uint16_t *h = (uint16_t *)out;
  rotmat_lut_pair(x1, &t2, &t3);
  rotmat_lut_pair(x2, &t4, &t5);
  h[8] = (uint16_t)t2;
  h[4] = (uint16_t)mul_q12(t4, t2);
  h[0] = (uint16_t)t4;
  h[3] = (uint16_t)mul_q12(t5, t2);
  h[2] = 0;
  h[7] = (uint16_t)mul_q12(t4, t3);
  h[1] = (uint16_t)-t5;
  h[5] = (uint16_t)-t3;
  h[6] = (uint16_t)mul_q12(t5, t3);
  return out;
}

uint32_t u16_lut_slot_set(uint32_t a0, uint32_t a1, uint32_t a2)
{
  uint32_t idx = a0 & 0xffffu;
  uint32_t flags = 72;
  uint32_t rec;
  if (idx >= 3) return 0;
  rec = br2_ld32(0x801b66e4u) + (idx << 4);
  br2_st16(rec + 4, 0);
  br2_st16(rec + 8, (uint16_t)a1);
  if (idx < 2) {
    if (a2 & 0x10u) flags = 73;
    if (!(a2 & 1u)) flags |= 0x100u;
  } else {
    if (!(a2 & 1u)) flags = 584;
  }
  if (a2 & 0x1000u) flags |= 0x10u;
  br2_st16(rec + 4, (uint16_t)flags);
  return 1;
}
