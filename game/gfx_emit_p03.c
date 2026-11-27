/* gfx_emit_p03.c — clean CPS ports of render/GTE emit piece-entries
 * (guest 0x8018D3B4-0x8018DAB4, SLES_017.22_full_42.c).
 *
 * All nine addrs are CPS split-piece entries of the dir_rotate / vec2 /
 * range-circle / rotmat emit family — reached via `cpu->pc = <addr>`
 * coroutine dispatches (jal links, branch splits, fallthrough pieces),
 * NOT ordinary `jr $ra` functions. They are therefore routed through
 * game/clean_cps.map (CLEAN_ENT_CPS wrappers that hand the fn
 * cpu->pc == the dispatched addr), exactly like the cps_clean.c bodies.
 * Each body below is a faithful op-by-op transcription of the generated
 * br2eu_func_* body: same guest ops, same order, same yield/continuation
 * points, minus cycle/icache/cosim/debug instrumentation.
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

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability). */
#define zero cpu->gpr[0]
#define at   cpu->gpr[1]
#define v0   cpu->gpr[2]
#define v1   cpu->gpr[3]
#define a0   cpu->gpr[4]
#define a1   cpu->gpr[5]
#define a2   cpu->gpr[6]
#define a3   cpu->gpr[7]
#define t0   cpu->gpr[8]
#define t1   cpu->gpr[9]
#define t2   cpu->gpr[10]
#define t3   cpu->gpr[11]
#define t4   cpu->gpr[12]
#define t5   cpu->gpr[13]
#define t6   cpu->gpr[14]
#define t7   cpu->gpr[15]
#define s0   cpu->gpr[16]
#define s1   cpu->gpr[17]
#define s2   cpu->gpr[18]
#define s3   cpu->gpr[19]
#define s4   cpu->gpr[20]
#define s5   cpu->gpr[21]
#define s6   cpu->gpr[22]
#define s7   cpu->gpr[23]
#define t8   cpu->gpr[24]
#define t9   cpu->gpr[25]
#define k0   cpu->gpr[26]
#define k1   cpu->gpr[27]
#define gp   cpu->gpr[28]
#define sp   cpu->gpr[29]
#define fp   cpu->gpr[30]
#define ra   cpu->gpr[31]

/* ======== 0x8018d3b4 — dir_rotate_emit ========
 * CPS piece-group (6 dispatch addrs: entries 0x8018D3B4/0x8018D3D0/
 * 0x8018D3EC + jal-return continuations 0x8018D3C0/0x8018D3DC/0x8018D3F8).
 * Each entry saves ra in t1, CPS-jals dir_rotate_pair (0x8018D408), and
 * the return block stores the rotated v0/v1 pair through a3 in one of
 * three formats, then `jr t1`:
 *   d3b4 -> d3c0: sw v1,0(a3); sw v0,8(a3); v0=a3; jr t1
 *   d3d0 -> d3dc: sh v1,0(a3); sh v0,4(a3); v0=a3; jr t1
 *   d3ec -> d3f8: v0=(v0<<16)|(v1&0xFFFF);            jr t1
 * Faithful to generated br2eu_func_8018D3B4. */
void dir_rotate_emit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018D3C0u: goto block_8018D3C0;
            case 0x8018D3DCu: goto block_8018D3DC;
            case 0x8018D3F8u: goto block_8018D3F8;
            case 0x8018D3D0u: goto block_8018D3D0;
            case 0x8018D3ECu: goto block_8018D3EC;
            default: break;
        }
    }

block_8018D3B4:
    t1 = ra;
    ra = 0x8018D3C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D3BC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D408u);
    cpu->pc = 0x8018D408u; return;  /* CPS jal -> dir_rotate_pair */

block_8018D3C0:
    cpu->write_word(a3, v1);
    cpu->write_word(a3 + 8, v0);
    uint32_t jr_target = t1;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a3;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr t1 */

block_8018D3D0:
    t1 = ra;
    ra = 0x8018D3DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D3D8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D408u);
    cpu->pc = 0x8018D408u; return;  /* CPS jal -> dir_rotate_pair */

block_8018D3DC:
    cpu->write_half(a3, (uint16_t)v1);
    cpu->write_half(a3 + 4, (uint16_t)v0);
    uint32_t jr_target_2 = t1;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = a3;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr t1 */

block_8018D3EC:
    t1 = ra;
    ra = 0x8018D3F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D3F4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D408u);
    cpu->pc = 0x8018D408u; return;  /* CPS jal -> dir_rotate_pair */

block_8018D3F8:
    v0 = v0 << 16;
    v1 = v1 & 0xFFFF;  /* 0x8018D3FC: 0x3063FFFF */
    uint32_t jr_target_3 = t1;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = v0 | v1;
    psx_check_interrupts_at(cpu, jr_target_3);
    cpu->pc = jr_target_3; return;  /* CPS: jr t1 */
}

/* ======== 0x8018d474 — dir_emit_hpair ========
 * Store half of a rotated/emit pair then fall through to the shared
 * `jr t5` epilogue piece at 0x8018D47C. Faithful to
 * br2eu_func_8018D474 (sh v1,0(a3); sh t4,4(a3); -> 0x8018D47C). */
void dir_emit_hpair(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8018D474:
    cpu->write_half(a3, (uint16_t)v1);
    cpu->write_half(a3 + 4, (uint16_t)t4);
    cpu->pc = 0x8018d47cu; return;  /* fallthrough to split piece */
}

/* ======== 0x8018d47c — dir_emit_ret_t5 ========
 * Shared `jr t5` return piece (nop delay slot). Faithful to
 * br2eu_func_8018D47C. */
void dir_emit_ret_t5(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8018D47C:;
    uint32_t jr_target = t5;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D480: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr t5 */
}

/* ======== 0x8018d49c — vec2_rot_emit ========
 * CPS piece (entries 0x8018D49C + jal-return continuation 0x8018D4AC).
 * Loads the s16 pair at a2+0/a2+4 into t2/t3, CPS-jals vec2_rot_lut
 * (0x8018D4FC — returns the packed v0:v1 pair), then on return recentres
 * both results by +64, stores the low-7-bit-minus-64 deltas back through
 * a2 as halfwords, and returns v0 = (sra(v0,7)<<16) | (sra(v1,7)&0xFFFF)
 * via `jr t1`. Faithful to br2eu_func_8018D49C. */
void vec2_rot_emit(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018D4ACu: goto block_8018D4AC;
            default: break;
        }
    }

block_8018D49C:
    t2 = (int32_t)(int16_t)cpu->read_half(a2);
    t3 = (int32_t)(int16_t)cpu->read_half(a2 + 4);
    ra = 0x8018D4ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D4A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D4FCu);
    cpu->pc = 0x8018D4FCu; return;  /* CPS jal -> vec2_rot_lut */

block_8018D4AC:
    v1 = v1 + t2;
    v0 = v0 + t3;
    v1 = v1 + 64;
    v0 = v0 + 64;
    at = -128;
    t2 = v1 & at;  /* 0x8018D4C0: 0x00615024 */
    at = -128;
    t3 = v0 & at;  /* 0x8018D4C8: 0x00415824 */
    t2 = v1 - t2;
    t3 = v0 - t3;
    t2 = t2 + -64;
    t3 = t3 + -64;
    v1 = (int32_t)v1 >> 7;
    t4 = (int32_t)v0 >> 7;
    cpu->write_half(a2, (uint16_t)t2);
    cpu->write_half(a2 + 4, (uint16_t)t3);
    v0 = t4 << 16;
    v1 = v1 & 0xFFFF;  /* 0x8018D4F0: 0x3063FFFF */
    uint32_t jr_target = t1;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = v0 | v1;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr t1 */
}

/* ======== 0x8018d668 — emit_tail_ra ========
 * Shared `jr $ra` return piece (nop delay slot) — the "inside range"
 * epilogue of the range-circle emit tail at 0x8018D670. Faithful to
 * br2eu_func_8018D668. */
void emit_tail_ra(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8018D668:;
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D66C: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x8018d670 — range_circle_test_tail ========
 * Tail of range_circle_test (0x8018D618): accumulates t1 -= a0*a0 then
 * t1 -= a1*a1 (mult -> lo -> mflo into t0, stall-accurate), then splits:
 *   t1 < 0  -> 0x8018D668 (jr $ra, $v0 untouched)
 *   t1 >= 0 -> 0x8018D694 (v0=0 in delay slot, jr $ra)
 * Faithful to br2eu_func_8018D670. */
void range_circle_test_tail(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8018D670:
    { int64_t result = (int64_t)(int32_t)a0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a0));
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    t1 = t1 - t0;
    /* nop */  /* 0x8018D67C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)a1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);
    t1 = t1 - t0;
    int cond_t1_lt_0 = ((int32_t)t1 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D690: 0x00000000 */
    if (cond_t1_lt_0) {
        psx_check_interrupts_at(cpu, 0x8018D668u);
        cpu->pc = 0x8018D668u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D694u);
        cpu->pc = 0x8018D694u; return;  /* CPS not taken: split */
    }
}

/* ======== 0x8018d694 — emit_tail_zero ========
 * `v0 = 0` in the jr delay slot, then `jr $ra` — the "outside range"
 * epilogue paired with emit_tail_ra. Faithful to br2eu_func_8018D694. */
void emit_tail_zero(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8018D694:;
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* ======== 0x8018da28 — rotmat2_build_s16 ========
 * Alternate entry ahead of rotmat2_build (0x8018DA2C): replaces a0 with
 * the sign-extended halfword at (a0), then falls through into
 * rotmat2_build. Faithful to br2eu_func_8018DA28. */
void rotmat2_build_s16(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8018DA28:
    a0 = (int32_t)(int16_t)cpu->read_half(a0);
    cpu->pc = 0x8018da2cu; return;  /* fallthrough to split piece */
}

/* ======== 0x8018dab4 — angle_pair_rotmat_unpack ========
 * Entry piece ahead of angle_pair_rotmat (0x8018DAC0): a0 = lw(a0)
 * (deref the packed angle-pair pointer), a2 = a1, a1 = sra(a0,16)
 * (hi 16 of the loaded word), then falls through into
 * angle_pair_rotmat. Faithful to br2eu_func_8018DAB4. */
void angle_pair_rotmat_unpack(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_8018DAB4:
    a0 = cpu->read_word(a0);
    a2 = a1;
    a1 = (int32_t)a0 >> 16;
    cpu->pc = 0x8018dac0u; return;  /* fallthrough to split piece */
}
