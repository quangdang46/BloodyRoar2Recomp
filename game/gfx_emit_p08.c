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
extern void     cpst_8018ef6c(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* GPU display-list command case-entries of the packet interpreter at guest
 * 0x8018EF6C. The function's opcode dispatcher tail-jumps (`jr v0` through a
 * 45-entry table at 0x80120000) into per-command blocks; each of these guest
 * addresses is one such jump-table target, also used as an IRQ resume pc.
 *
 * The whole shared body already exists as the clean CPS transliteration
 * cpst_8018ef6c (game/cps_clean.c): clean_try_dispatch hands the entry addr
 * in cpu->pc and the body's pc-switch resumes at the matching block, with a
 * psx_check_interrupts_at at every block boundary exactly like the
 * generated alias body. These wrappers are the named clean entries; the
 * dispatch addrs are registered in game/clean_cps.map so they get CPS
 * semantics (cpu->pc preserved across yields / tail-dispatch). */

/* 0x8018F22C — attr word (sp+0x210) |= 0x40, then common tail at 0x8018F244. */
void gpu_pkt_cmd_f22c(void)
{
    cpst_8018ef6c();
}

/* 0x8018F238 — attr word (sp+0x210) |= 0x80, then common tail at 0x8018F244. */
void gpu_pkt_cmd_f238(void)
{
    cpst_8018ef6c();
}

/* 0x8018F264 — command bitfield decode (>>5&1, sign test), writes flag byte
 * at sp+0x211, joins shared decode tail. */
void gpu_pkt_cmd_f264(void)
{
    cpst_8018ef6c();
}

/* 0x8018F2BC — command bitfield decode (>>5&1, >>4&1, >>3&1), flag byte. */
void gpu_pkt_cmd_f2bc(void)
{
    cpst_8018ef6c();
}

/* 0x8018F3DC — command bitfield decode (>>5&1, >>4&1, >>3&1), count at
 * sp+0x218. */
void gpu_pkt_cmd_f3dc(void)
{
    cpst_8018ef6c();
}

/* 0x8018F4DC — count=8 at sp+0x218, attr |= 0x50, a3=0x80127208, j 0x8018F504.
 * In the shared CPS body this whole sequence is block_8018F4DC (the F4F0
 * insns are inline there), so the plain entry works. */
void gpu_pkt_cmd_f4dc(void)
{
    cpst_8018ef6c();
}

/* 0x8018F4F0 — `lui a3,0x8012` @F4F0, `j 0x8018F504` @F4F4 with delay-slot
 * `addiu a3,a3,0x5208` @F4F8. This address is a jump-table entry but NOT a
 * block leader in the 0x8018EF6C CFG (it sits mid-block inside
 * block_8018F4DC's region), so cpst_8018ef6c has no case for it. Replay the
 * two guest insns exactly, then re-enter the shared body at F504 — the same
 * resume pc the generated `j` uses for its interrupt check. */
void gpu_pkt_cmd_f4f0(void)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return;
    cpu->gpr[7] = 0x8012u << 16;        /* lui a3,0x8012 */
    cpu->gpr[7] = cpu->gpr[7] + 21000;  /* addiu a3,a3,0x5208 (delay slot) */
    psx_check_interrupts_at(cpu, 0x8018F504u);
    cpu->pc = 0x8018F504u;
    cpst_8018ef6c();
}

/* 0x8018F4FC — a3=0x8012721C (0x80120000+21020), falls into 0x8018F504. */
void gpu_pkt_cmd_f4fc(void)
{
    cpst_8018ef6c();
}

/* 0x8018F608 — byte-stream copy command (s1-- store, cursor += 4), tail at
 * 0x8018F700. */
void gpu_pkt_cmd_f608(void)
{
    cpst_8018ef6c();
}
