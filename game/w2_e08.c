/* w2_e08.c — clean CPS ports of scriptop_chk state-bit condition
 * piece-entries (guest 0x80183D0C-0x80183DA8 + shared tails
 * 0x80183D50/0x80183DB0/0x80183DBC/0x80183FE4/0x80184358,
 * SLES_017.22_full_39.c, alias body br2eu_psx_alias_body_80183C30).
 *
 * These eight addrs are CPS split-piece entries of scriptop_chk
 * (guest 0x80183C30, clean-ported in vm.c): the cond-table cases 2-9
 * that test bits of the fighter state word at ctrl+0x58. Each entry
 * loads the state word (s3 = self ctrl, s0 = foe ctrl), advances the
 * script pc (ctx+0x24 via s1+2 store to s2+0x24), computes the result
 * (raw mask / shifted-xor boolean), and stores it to ctx+0x38 (s2+0x38)
 * through one of four shared tail blocks, then falls into the shared
 * `jr $ra` epilogue at 0x80184358.
 *
 * They are reached via `cpu->pc = <addr>` coroutine dispatches, NOT
 * ordinary `jr $ra` calls, so they are routed through game/clean_cps.map
 * (CLEAN_ENT_CPS wrappers that hand the fn cpu->pc == the dispatched
 * addr). The shared tail blocks are also CPS resume PCs (every
 * psx_check_interrupts_at in this region passes one as resume_pc), so
 * they are registered in clean_cps.map as well — the blocks are
 * entry-independent, so resuming there from any scriptop_chk cond case
 * runs identical code.
 *
 * Faithful op-by-op transcription of the generated blocks: same guest
 * ops, same order, same yield/continuation points, minus
 * cycle/icache/cosim/debug instrumentation. Memory ops use the
 * MMIO-aware cpu->read_word / cpu->write_word accessors.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* ======== scriptop_chk_state_bits ========
 * CPS piece-group (13 dispatch addrs): cond cases 2-9 of scriptop_chk —
 *   D0C/D24: state & 0x10000000 (self s3 / foe s0) -> mask tail DBC
 *   D3C/D48: state & 0x00000800 (self s3 / foe s0) -> mask tail D50
 *   D64/D80: ((state >> 11) ^ 1) & 1        (self s3 / foe s0) -> xor tail FE4
 *   D9C/DA8: state & 0x80000000 (self s3 / foe s0) -> mask tail DB0->DBC
 * Shared tails D50/DB0/DBC/FE4 store ctx+0x38 and jump to the shared
 * epilogue 0x80184358 (pop s0-s4/ra, sp+=40, jr ra). Faithful to the
 * generated br2eu_psx_alias_body_80183C30 blocks. */
void scriptop_chk_state_bits(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183D0Cu: goto block_80183D0C;
            case 0x80183D24u: goto block_80183D24;
            case 0x80183D3Cu: goto block_80183D3C;
            case 0x80183D48u: goto block_80183D48;
            case 0x80183D50u: goto block_80183D50;
            case 0x80183D64u: goto block_80183D64;
            case 0x80183D80u: goto block_80183D80;
            case 0x80183D9Cu: goto block_80183D9C;
            case 0x80183DA8u: goto block_80183DA8;
            case 0x80183DB0u: goto block_80183DB0;
            case 0x80183DBCu: goto block_80183DBC;
            case 0x80183FE4u: goto block_80183FE4;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }

block_80183D0C:
    v0 = 1;
    v1 = cpu->read_word(s3 + 88);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    /* delay slot (always executes) */
    a0 = 0x1000 << 16;  /* 0x10000000 */
    psx_check_interrupts_at(cpu, 0x80183DBCu);
    goto block_80183DBC;  /* j */

block_80183D24:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    /* delay slot (always executes) */
    a0 = 0x1000 << 16;  /* 0x10000000 */
    psx_check_interrupts_at(cpu, 0x80183DBCu);
    goto block_80183DBC;  /* j */

block_80183D3C:
    v1 = cpu->read_word(s3 + 88);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183D50u);
    goto block_80183D50;  /* j */

block_80183D48:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    /* fall through to block_80183D50 */

block_80183D50:
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = v1 & 0x800;
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80183D64:
    v0 = 1;
    v1 = cpu->read_word(s3 + 88);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = v1 >> 11;
    /* delay slot (always executes) */
    v1 = v1 ^ v0;
    psx_check_interrupts_at(cpu, 0x80183FE4u);
    goto block_80183FE4;  /* j */

block_80183D80:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = v1 >> 11;
    /* delay slot (always executes) */
    v1 = v1 ^ v0;
    psx_check_interrupts_at(cpu, 0x80183FE4u);
    goto block_80183FE4;  /* j */

block_80183D9C:
    v1 = cpu->read_word(s3 + 88);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183DB0u);
    goto block_80183DB0;  /* j */

block_80183DA8:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    /* fall through to block_80183DB0 */

block_80183DB0:
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    a0 = 0x8000 << 16;  /* 0x80000000 */
    /* fall through to block_80183DBC */

block_80183DBC:
    v1 = v1 & a0;
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80183FE4:
    v1 = v1 & v0;
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184358:
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}
