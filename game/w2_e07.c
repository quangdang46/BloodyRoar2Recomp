#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* scriptop_wait_cps_body — faithful clean CPS transliteration of the
 * generated br2eu_func_80183664 body (guest 0x80183664-0x80183BB4,
 * "scriptop_wait"), produced by game/gen_cps_clean.py and hand-checked
 * block-for-block. The six guest addresses below are NOT standalone
 * functions: they are alias piece-entries the recompiler exposes inside
 * this one CPS body (br2eu_psx_alias_body_80183664's entry switch — the
 * canonical function plus ten sibling entries at 0x8018371C/0x801837B0/
 * 0x801838B0/0x801838E8/0x80183920/0x80183948 that belong to other
 * workstreams). Each enters the shared body mid-stream — after the
 * prologue has built the 40-byte guest frame — so they are only ever
 * reached as CPS continuation/branch-target PCs. clean_cps.map registers
 * them with CLEAN_ENT_CPS so clean_try_dispatch sets cpu->pc = <addr>
 * and the _cont switch picks the right block, exactly like the generated
 * alias body. The canonical entry 0x80183664 stays with the vm.c
 * scriptop_wait port; this body only serves the piece-entry addrs. */
/* scriptop_wait (guest 0x80183664 region) — script WAIT opcode
 * CPS body. Args: s2=a0 owner obj, s0=a2 script ctx; s3=ctx[36]
 * stream, ctx[110] mode halfword. ~70 blocks: operand fetches from
 * the stream select wait-until conditions (frame count / flag /
 * timer); 5 sprintf_core_guest calls format trace strings. */
static void scriptop_wait_cps_body(CPUState *cpu)
{

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801836D8u: goto block_801836D8;
            case 0x80183774u: goto block_80183774;
            case 0x80183878u: goto block_80183878;
            case 0x80183A0Cu: goto block_80183A0C;
            case 0x80183B6Cu: goto block_80183B6C;
            case 0x80183698u: goto block_80183698;
            case 0x801836B4u: goto block_801836B4;
            case 0x801836E8u: goto block_801836E8;
            case 0x80183700u: goto block_80183700;
            case 0x8018371Cu: goto block_8018371C;
            case 0x8018372Cu: goto block_8018372C;
            case 0x80183738u: goto block_80183738;
            case 0x80183748u: goto block_80183748;
            case 0x80183764u: goto block_80183764;
            case 0x80183784u: goto block_80183784;
            case 0x801837A0u: goto block_801837A0;
            case 0x801837B0u: goto block_801837B0;
            case 0x801837C0u: goto block_801837C0;
            case 0x80183830u: goto block_80183830;
            case 0x80183834u: goto block_80183834;
            case 0x8018383Cu: goto block_8018383C;
            case 0x8018384Cu: goto block_8018384C;
            case 0x80183868u: goto block_80183868;
            case 0x80183888u: goto block_80183888;
            case 0x801838A4u: goto block_801838A4;
            case 0x801838B0u: goto block_801838B0;
            case 0x801838C0u: goto block_801838C0;
            case 0x801838D0u: goto block_801838D0;
            case 0x801838E0u: goto block_801838E0;
            case 0x801838E8u: goto block_801838E8;
            case 0x801838F8u: goto block_801838F8;
            case 0x80183908u: goto block_80183908;
            case 0x80183918u: goto block_80183918;
            case 0x80183920u: goto block_80183920;
            case 0x80183930u: goto block_80183930;
            case 0x80183934u: goto block_80183934;
            case 0x80183940u: goto block_80183940;
            case 0x80183948u: goto block_80183948;
            case 0x80183958u: goto block_80183958;
            case 0x80183964u: goto block_80183964;
            case 0x80183968u: goto block_80183968;
            case 0x80183974u: goto block_80183974;
            case 0x8018397Cu: goto block_8018397C;
            case 0x80183988u: goto block_80183988;
            case 0x80183998u: goto block_80183998;
            case 0x801839D0u: goto block_801839D0;
            case 0x801839E0u: goto block_801839E0;
            case 0x801839FCu: goto block_801839FC;
            case 0x80183A1Cu: goto block_80183A1C;
            case 0x80183A38u: goto block_80183A38;
            case 0x80183A48u: goto block_80183A48;
            case 0x80183A58u: goto block_80183A58;
            case 0x80183A6Cu: goto block_80183A6C;
            case 0x80183A74u: goto block_80183A74;
            case 0x80183A84u: goto block_80183A84;
            case 0x80183A98u: goto block_80183A98;
            case 0x80183A9Cu: goto block_80183A9C;
            case 0x80183AA4u: goto block_80183AA4;
            case 0x80183AB4u: goto block_80183AB4;
            case 0x80183B24u: goto block_80183B24;
            case 0x80183B28u: goto block_80183B28;
            case 0x80183B30u: goto block_80183B30;
            case 0x80183B40u: goto block_80183B40;
            case 0x80183B5Cu: goto block_80183B5C;
            case 0x80183B7Cu: goto block_80183B7C;
            case 0x80183B98u: goto block_80183B98;
            case 0x80183BA4u: goto block_80183BA4;
            case 0x80183BACu: goto block_80183BAC;
            case 0x80183BB4u: goto block_80183BB4;
            default: break;
        }
    }
    /* Address: 0x80183664, Size: 1388 bytes, Blocks: 70 */

block_80183664:
    sp = sp + -40;
    cpu->write_word(sp + 24, s2);
    s2 = a0;
    cpu->write_word(sp + 16, s0);
    s0 = a2;
    cpu->write_word(sp + 32, ra);
    cpu->write_word(sp + 28, s3);
    cpu->write_word(sp + 20, s1);
    s3 = cpu->read_word(s0 + 36);
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 110);
    s1 = cpu->read_word(s2 + 44);
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183694: 0x00000000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801836E8u);
        goto block_801836E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183698u);
        goto block_80183698;  /* not taken */
    }

block_80183698:
    a0 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x8018369C: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s0;
    v0 = ((int32_t)v0 < 184) ? 1 : 0;  /* 0x801836A8: 0x284200B8 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x801B << 16;  /* 0x801B0000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x801836E8u);
        goto block_801836E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801836B4u);
        goto block_801836B4;  /* not taken */
    }

block_801836B4:
    v1 = v1 + 21520;
    v0 = cpu->read_half(s3);
    a1 = 0x8012 << 16;  /* 0x80120000 */
    v0 = (uint32_t)v0 >> 8;
    v0 = v0 << 2;
    v0 = v0 + v1;
    a2 = cpu->read_word(v0);
    ra = 0x801836D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 12560;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_801836D8:
    v1 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x801836DC: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s0 + 120, v1);
    /* fall through to block_801836E8 */

block_801836E8:
    v0 = cpu->read_half(s3);
    /* nop */  /* 0x801836EC: 0x00000000 */
    v1 = (uint32_t)v0 >> 8;
    v0 = (v1 < (uint32_t)12) ? 1 : 0;  /* 0x801836F4: 0x2C62000C */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x8018371Cu);
        goto block_8018371C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183700u);
        goto block_80183700;  /* not taken */
    }

block_80183700:
    v0 = v0 + 13332;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80183710: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183718: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

block_8018371C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    /* nop */  /* 0x80183720: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183728: 0x00000000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183738u);
        goto block_80183738;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018372Cu);
        goto block_8018372C;  /* not taken */
    }

block_8018372C:
    v0 = cpu->read_half(s3 + 2);
    /* nop */  /* 0x80183730: 0x00000000 */
    cpu->write_half(s0 + 60, (uint16_t)v0);
    /* fall through to block_80183738 */

block_80183738:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 110);
    /* nop */  /* 0x8018373C: 0x00000000 */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183744: 0x00000000 */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x80183784u);
        goto block_80183784;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183748u);
        goto block_80183748;  /* not taken */
    }

block_80183748:
    a0 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x8018374C: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s0;
    v0 = ((int32_t)v0 < 195) ? 1 : 0;  /* 0x80183758: 0x284200C3 */
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183760: 0x00000000 */
    if (cond_v0_eq_zero_5) {
        psx_check_interrupts_at(cpu, 0x80183784u);
        goto block_80183784;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183764u);
        goto block_80183764;  /* not taken */
    }

block_80183764:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a2 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    ra = 0x80183774u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 13320;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80183774:
    v1 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x80183778: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s0 + 120, v1);
    /* fall through to block_80183784 */

block_80183784:
    v0 = cpu->read_half(s0 + 60);
    /* nop */  /* 0x80183788: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(s0 + 60, (uint16_t)v0);
    v0 = v0 << 16;
    int cond_v0_gt_0 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_gt_0) {
        psx_check_interrupts_at(cpu, 0x80183BB4u);
        goto block_80183BB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801837A0u);
        goto block_801837A0;  /* not taken */
    }

block_801837A0:
    v0 = s3 + 4;
    cpu->write_word(s0 + 36, v0);
    /* delay slot (always executes) */
    cpu->write_half(s0 + 60, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x80183940u);
    goto block_80183940;  /* j */

block_801837B0:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    /* nop */  /* 0x801837B4: 0x00000000 */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801837BC: 0x00000000 */
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x8018383Cu);
        goto block_8018383C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801837C0u);
        goto block_801837C0;  /* not taken */
    }

block_801837C0:
    a0 = cpu->read_byte(s0 + 4);
    v0 = cpu->read_word(s0);
    a1 = a0 + 1;
    v1 = a0 & 0x00FF;  /* 0x801837CC: 0x308300FF */
    v0 = v0 + v1;
    v1 = a0 + 2;
    cpu->write_byte(s0 + 4, (uint8_t)a1);
    a1 = a1 & 0x00FF;  /* 0x801837DC: 0x30A500FF */
    a2 = cpu->read_byte(v0);
    v0 = cpu->read_word(s0);
    a0 = a0 + 3;
    cpu->write_byte(s0 + 4, (uint8_t)v1);
    v0 = v0 + a1;
    a1 = cpu->read_byte(v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x801837FC: 0x306300FF */
    cpu->write_byte(s0 + 4, (uint8_t)a0);
    v0 = v0 + v1;
    a2 = a2 + a1;
    v0 = cpu->read_byte(v0);
    v1 = cpu->read_half(s0 + 8);
    a2 = a2 + v0;
    v0 = 256;
    v0 = v0 - v1;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a2; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018382C: 0x00000000 */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183834u);
        goto block_80183834;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183830u);
        goto block_80183830;  /* not taken */
    }

block_80183830:
    v0 = v0 + 8191;
    /* fall through to block_80183834 */

block_80183834:
    v0 = (int32_t)v0 >> 13;
    cpu->write_half(s0 + 60, (uint16_t)v0);
    /* fall through to block_8018383C */

block_8018383C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 110);
    /* nop */  /* 0x80183840: 0x00000000 */
    int cond_v0_eq_zero_6 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183848: 0x00000000 */
    if (cond_v0_eq_zero_6) {
        psx_check_interrupts_at(cpu, 0x80183888u);
        goto block_80183888;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018384Cu);
        goto block_8018384C;  /* not taken */
    }

block_8018384C:
    a0 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x80183850: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s0;
    v0 = ((int32_t)v0 < 195) ? 1 : 0;  /* 0x8018385C: 0x284200C3 */
    int cond_v0_eq_zero_7 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183864: 0x00000000 */
    if (cond_v0_eq_zero_7) {
        psx_check_interrupts_at(cpu, 0x80183888u);
        goto block_80183888;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183868u);
        goto block_80183868;  /* not taken */
    }

block_80183868:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a2 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    ra = 0x80183878u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 13320;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80183878:
    v1 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x8018387C: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s0 + 120, v1);
    /* fall through to block_80183888 */

block_80183888:
    v0 = cpu->read_half(s0 + 60);
    /* nop */  /* 0x8018388C: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(s0 + 60, (uint16_t)v0);
    v0 = v0 << 16;
    int cond_v0_gt_0_2 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_gt_0_2) {
        psx_check_interrupts_at(cpu, 0x80183BB4u);
        goto block_80183BB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801838A4u);
        goto block_801838A4;  /* not taken */
    }

block_801838A4:
    v0 = 1;
    /* delay slot (always executes) */
    v1 = s3 + 2;
    psx_check_interrupts_at(cpu, 0x80183BACu);
    goto block_80183BAC;  /* j */

block_801838B0:
    v0 = cpu->read_word(s2 + 88);
    /* nop */  /* 0x801838B4: 0x00000000 */
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801838C0u);
        goto block_801838C0;  /* not taken */
    }

block_801838C0:
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 186);
    /* nop */  /* 0x801838C4: 0x00000000 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x80183BB4u);
        goto block_80183BB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801838D0u);
        goto block_801838D0;  /* not taken */
    }

block_801838D0:
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 184);
    /* nop */  /* 0x801838D4: 0x00000000 */
    int cond_v0_eq_zero_8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_eq_zero_8) {
        psx_check_interrupts_at(cpu, 0x80183940u);
        goto block_80183940;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801838E0u);
        goto block_801838E0;  /* not taken */
    }

block_801838E0:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183A9Cu);
    goto block_80183A9C;  /* j */

block_801838E8:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x801838EC: 0x00000000 */
    int cond_v0_ge_0_3 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_3) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801838F8u);
        goto block_801838F8;  /* not taken */
    }

block_801838F8:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 186);
    /* nop */  /* 0x801838FC: 0x00000000 */
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero_4) {
        psx_check_interrupts_at(cpu, 0x80183BB4u);
        goto block_80183BB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183908u);
        goto block_80183908;  /* not taken */
    }

block_80183908:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 184);
    /* nop */  /* 0x8018390C: 0x00000000 */
    int cond_v0_eq_zero_9 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_eq_zero_9) {
        psx_check_interrupts_at(cpu, 0x80183940u);
        goto block_80183940;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183918u);
        goto block_80183918;  /* not taken */
    }

block_80183918:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80183A9Cu);
    goto block_80183A9C;  /* j */

block_80183920:
    v0 = cpu->read_word(s2 + 88);
    /* nop */  /* 0x80183924: 0x00000000 */
    int cond_v0_ge_0_4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_4) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183930u);
        goto block_80183930;  /* not taken */
    }

block_80183930:
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 188);
    /* fall through to block_80183934 */

block_80183934:;
    /* nop */  /* 0x80183934: 0x00000000 */
    int cond_v0_gt_0_3 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_gt_0_3) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183940u);
        goto block_80183940;  /* not taken */
    }

block_80183940:
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, 0x80183BB4u);
    goto block_80183BB4;  /* j */

block_80183948:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x8018394C: 0x00000000 */
    int cond_v0_ge_0_5 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_5) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183958u);
        goto block_80183958;  /* not taken */
    }

block_80183958:
    v0 = (int32_t)(int16_t)cpu->read_half(s1 + 188);
    /* delay slot (always executes) */
    /* nop */  /* 0x80183960: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80183934u);
    goto block_80183934;  /* j */

block_80183964:
    v0 = cpu->read_word(s2 + 88);
    /* fall through to block_80183968 */

block_80183968:;
    /* nop */  /* 0x80183968: 0x00000000 */
    int cond_v0_ge_0_6 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_6) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183974u);
        goto block_80183974;  /* not taken */
    }

block_80183974:
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, 0x80183BB4u);
    goto block_80183BB4;  /* j */

block_8018397C:
    v0 = cpu->read_word(s1 + 88);
    /* delay slot (always executes) */
    /* nop */  /* 0x80183984: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80183968u);
    goto block_80183968;  /* j */

block_80183988:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    /* nop */  /* 0x8018398C: 0x00000000 */
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183994: 0x00000000 */
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x801839D0u);
        goto block_801839D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183998u);
        goto block_80183998;  /* not taken */
    }

block_80183998:
    v1 = cpu->read_byte(s0 + 4);
    a0 = cpu->read_half(s3 + 2);
    v0 = v1 + 1;
    cpu->write_byte(s0 + 4, (uint8_t)v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x801839AC: 0x306300FF */
    v0 = v0 + v1;
    v0 = cpu->read_byte(v0);
    /* nop */  /* 0x801839B8: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)a0 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a0));
    a3 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = (int32_t)a3 >> 8;
    v0 = v0 + 1;
    cpu->write_half(s0 + 60, (uint16_t)v0);
    /* fall through to block_801839D0 */

block_801839D0:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 110);
    /* nop */  /* 0x801839D4: 0x00000000 */
    int cond_v0_eq_zero_10 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801839DC: 0x00000000 */
    if (cond_v0_eq_zero_10) {
        psx_check_interrupts_at(cpu, 0x80183A1Cu);
        goto block_80183A1C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801839E0u);
        goto block_801839E0;  /* not taken */
    }

block_801839E0:
    a0 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x801839E4: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s0;
    v0 = ((int32_t)v0 < 195) ? 1 : 0;  /* 0x801839F0: 0x284200C3 */
    int cond_v0_eq_zero_11 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801839F8: 0x00000000 */
    if (cond_v0_eq_zero_11) {
        psx_check_interrupts_at(cpu, 0x80183A1Cu);
        goto block_80183A1C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801839FCu);
        goto block_801839FC;  /* not taken */
    }

block_801839FC:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a2 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    ra = 0x80183A0Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 13320;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80183A0C:
    v1 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x80183A10: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s0 + 120, v1);
    /* fall through to block_80183A1C */

block_80183A1C:
    v0 = cpu->read_half(s0 + 60);
    /* nop */  /* 0x80183A20: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(s0 + 60, (uint16_t)v0);
    v0 = v0 << 16;
    int cond_v0_gt_0_4 = ((int32_t)v0 > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_gt_0_4) {
        psx_check_interrupts_at(cpu, 0x80183BB4u);
        goto block_80183BB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183A38u);
        goto block_80183A38;  /* not taken */
    }

block_80183A38:
    v0 = s3 + 4;
    cpu->write_word(s0 + 36, v0);
    /* delay slot (always executes) */
    cpu->write_half(s0 + 60, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x80183940u);
    goto block_80183940;  /* j */

block_80183A48:
    v0 = cpu->read_word(s2 + 88);
    /* nop */  /* 0x80183A4C: 0x00000000 */
    int cond_v0_ge_0_7 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_7) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183A58u);
        goto block_80183A58;  /* not taken */
    }

block_80183A58:
    v0 = cpu->read_word(s2 + 108);
    v1 = 0x0100 << 16;  /* 0x01000000 */
    v0 = v0 & v1;  /* 0x80183A60: 0x00431024 */
    int cond_v0_eq_zero_12 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_eq_zero_12) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183A6Cu);
        goto block_80183A6C;  /* not taken */
    }

block_80183A6C:
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, 0x80183BB4u);
    goto block_80183BB4;  /* j */

block_80183A74:
    v0 = cpu->read_word(s1 + 88);
    /* nop */  /* 0x80183A78: 0x00000000 */
    int cond_v0_ge_0_8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ge_0_8) {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183A84u);
        goto block_80183A84;  /* not taken */
    }

block_80183A84:
    v0 = cpu->read_word(s1 + 108);
    v1 = 0x0100 << 16;  /* 0x01000000 */
    v0 = v0 & v1;  /* 0x80183A8C: 0x00431024 */
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = s3 + 2;
    if (cond_v0_ne_zero_6) {
        psx_check_interrupts_at(cpu, 0x80183940u);
        goto block_80183940;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183A98u);
        goto block_80183A98;  /* not taken */
    }

block_80183A98:
    cpu->write_word(s0 + 36, v0);
    /* fall through to block_80183A9C */

block_80183A9C:
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183BB4u);
    goto block_80183BB4;  /* j */

block_80183AA4:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    /* nop */  /* 0x80183AA8: 0x00000000 */
    int cond_v0_ne_zero_7 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183AB0: 0x00000000 */
    if (cond_v0_ne_zero_7) {
        psx_check_interrupts_at(cpu, 0x80183B30u);
        goto block_80183B30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183AB4u);
        goto block_80183AB4;  /* not taken */
    }

block_80183AB4:
    a0 = cpu->read_byte(s0 + 4);
    v0 = cpu->read_word(s0);
    a1 = a0 + 1;
    v1 = a0 & 0x00FF;  /* 0x80183AC0: 0x308300FF */
    v0 = v0 + v1;
    v1 = a0 + 2;
    cpu->write_byte(s0 + 4, (uint8_t)a1);
    a1 = a1 & 0x00FF;  /* 0x80183AD0: 0x30A500FF */
    a2 = cpu->read_byte(v0);
    v0 = cpu->read_word(s0);
    a0 = a0 + 3;
    cpu->write_byte(s0 + 4, (uint8_t)v1);
    v0 = v0 + a1;
    a1 = cpu->read_byte(v0);
    v0 = cpu->read_word(s0);
    v1 = v1 & 0x00FF;  /* 0x80183AF0: 0x306300FF */
    cpu->write_byte(s0 + 4, (uint8_t)a0);
    v0 = v0 + v1;
    a2 = a2 + a1;
    v0 = cpu->read_byte(v0);
    v1 = cpu->read_half(s0 + 8);
    a2 = a2 + v0;
    v0 = 256;
    v0 = v0 - v1;
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a2; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);
    int cond_v0_ge_0_9 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183B20: 0x00000000 */
    if (cond_v0_ge_0_9) {
        psx_check_interrupts_at(cpu, 0x80183B28u);
        goto block_80183B28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183B24u);
        goto block_80183B24;  /* not taken */
    }

block_80183B24:
    v0 = v0 + 8191;
    /* fall through to block_80183B28 */

block_80183B28:
    v0 = (int32_t)v0 >> 13;
    cpu->write_half(s0 + 60, (uint16_t)v0);
    /* fall through to block_80183B30 */

block_80183B30:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 110);
    /* nop */  /* 0x80183B34: 0x00000000 */
    int cond_v0_eq_zero_13 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183B3C: 0x00000000 */
    if (cond_v0_eq_zero_13) {
        psx_check_interrupts_at(cpu, 0x80183B7Cu);
        goto block_80183B7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183B40u);
        goto block_80183B40;  /* not taken */
    }

block_80183B40:
    a0 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x80183B44: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s0;
    v0 = ((int32_t)v0 < 195) ? 1 : 0;  /* 0x80183B50: 0x284200C3 */
    int cond_v0_eq_zero_14 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183B58: 0x00000000 */
    if (cond_v0_eq_zero_14) {
        psx_check_interrupts_at(cpu, 0x80183B7Cu);
        goto block_80183B7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183B5Cu);
        goto block_80183B5C;  /* not taken */
    }

block_80183B5C:
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a2 = (int32_t)(int16_t)cpu->read_half(s0 + 60);
    ra = 0x80183B6Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 13320;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80183B6C:
    v1 = cpu->read_word(s0 + 120);
    /* nop */  /* 0x80183B70: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s0 + 120, v1);
    /* fall through to block_80183B7C */

block_80183B7C:
    v0 = cpu->read_half(s0 + 60);
    /* nop */  /* 0x80183B80: 0x00000000 */
    v0 = v0 + -1;
    cpu->write_half(s0 + 60, (uint16_t)v0);
    v0 = v0 << 16;
    int cond_v0_le_0 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_le_0) {
        psx_check_interrupts_at(cpu, 0x80183BA4u);
        goto block_80183BA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183B98u);
        goto block_80183B98;  /* not taken */
    }

block_80183B98:
    v1 = cpu->read_half(s3 + 2);
    /* delay slot (always executes) */
    cpu->write_half(s0 + 6, (uint16_t)v1);
    psx_check_interrupts_at(cpu, 0x80183BB4u);
    goto block_80183BB4;  /* j */

block_80183BA4:
    v0 = 1;
    v1 = s3 + 4;
    /* fall through to block_80183BAC */

block_80183BAC:
    cpu->write_word(s0 + 36, v1);
    cpu->write_half(s0 + 60, (uint16_t)zero);
    /* fall through to block_80183BB4 */

block_80183BB4:
    ra = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/* scriptop_chk_cps_body — faithful clean CPS transliteration of the
 * generated br2eu_func_80183C30 body (guest 0x80183C30-0x80184378,
 * "scriptop_chk"), produced by game/gen_cps_clean.py and hand-checked
 * block-for-block. 0x80183CE4 and 0x80183CF0 are NOT standalone
 * functions: they are continuation PCs inside this one CPS body — the
 * generated cpu->pc switch has `case 0x80183CE4` / `case 0x80183CF0`
 * labels that rejoin the shared epilogue at block_80183CF8 ->
 * block_80184358. clean_cps.map registers them with CLEAN_ENT_CPS so
 * clean_try_dispatch sets cpu->pc = <addr> and the _cont switch picks
 * the right block. The canonical entry 0x80183C30 stays with the vm.c
 * scriptop_chk port; this body only serves the two piece-entry addrs. */
/* scriptop_chk (guest 0x80183C30 region) — script CHK opcode
 * CPS body. Same ctx convention (s0=ctx, s2=owner, s3=ctx[36]
 * stream). Condition-eval opcode: tests script flags/vars and
 * branches the stream; 2 sprintf_core_guest trace calls. */
static void scriptop_chk_cps_body(CPUState *cpu)
{

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183CACu: goto block_80183CAC;
            case 0x801842A4u: goto block_801842A4;
            case 0x80183C74u: goto block_80183C74;
            case 0x80183C90u: goto block_80183C90;
            case 0x80183CBCu: goto block_80183CBC;
            case 0x80183CC0u: goto block_80183CC0;
            case 0x80183CC8u: goto block_80183CC8;
            case 0x80183CE4u: goto block_80183CE4;
            case 0x80183CF0u: goto block_80183CF0;
            case 0x80183CF8u: goto block_80183CF8;
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
            case 0x80183DC8u: goto block_80183DC8;
            case 0x80183DD8u: goto block_80183DD8;
            case 0x80183DE4u: goto block_80183DE4;
            case 0x80183DF4u: goto block_80183DF4;
            case 0x80183E04u: goto block_80183E04;
            case 0x80183E10u: goto block_80183E10;
            case 0x80183E20u: goto block_80183E20;
            case 0x80183E30u: goto block_80183E30;
            case 0x80183E40u: goto block_80183E40;
            case 0x80183E4Cu: goto block_80183E4C;
            case 0x80183E50u: goto block_80183E50;
            case 0x80183E5Cu: goto block_80183E5C;
            case 0x80183E6Cu: goto block_80183E6C;
            case 0x80183E7Cu: goto block_80183E7C;
            case 0x80183E88u: goto block_80183E88;
            case 0x80183E8Cu: goto block_80183E8C;
            case 0x80183E98u: goto block_80183E98;
            case 0x80183EA8u: goto block_80183EA8;
            case 0x80183EB4u: goto block_80183EB4;
            case 0x80183EC4u: goto block_80183EC4;
            case 0x80183ED4u: goto block_80183ED4;
            case 0x80183EE0u: goto block_80183EE0;
            case 0x80183EF0u: goto block_80183EF0;
            case 0x80183EFCu: goto block_80183EFC;
            case 0x80183F08u: goto block_80183F08;
            case 0x80183F18u: goto block_80183F18;
            case 0x80183F28u: goto block_80183F28;
            case 0x80183F38u: goto block_80183F38;
            case 0x80183F48u: goto block_80183F48;
            case 0x80183F58u: goto block_80183F58;
            case 0x80183F68u: goto block_80183F68;
            case 0x80183F78u: goto block_80183F78;
            case 0x80183F88u: goto block_80183F88;
            case 0x80183F98u: goto block_80183F98;
            case 0x80183FA8u: goto block_80183FA8;
            case 0x80183FB8u: goto block_80183FB8;
            case 0x80183FC8u: goto block_80183FC8;
            case 0x80183FD4u: goto block_80183FD4;
            case 0x80183FDCu: goto block_80183FDC;
            case 0x80183FE4u: goto block_80183FE4;
            case 0x80183FF0u: goto block_80183FF0;
            case 0x80183FFCu: goto block_80183FFC;
            case 0x80184004u: goto block_80184004;
            case 0x80184018u: goto block_80184018;
            case 0x80184024u: goto block_80184024;
            case 0x8018402Cu: goto block_8018402C;
            case 0x80184040u: goto block_80184040;
            case 0x80184050u: goto block_80184050;
            case 0x80184060u: goto block_80184060;
            case 0x80184068u: goto block_80184068;
            case 0x80184074u: goto block_80184074;
            case 0x80184084u: goto block_80184084;
            case 0x80184094u: goto block_80184094;
            case 0x8018409Cu: goto block_8018409C;
            case 0x801840A8u: goto block_801840A8;
            case 0x801840BCu: goto block_801840BC;
            case 0x801840CCu: goto block_801840CC;
            case 0x801840DCu: goto block_801840DC;
            case 0x801840F0u: goto block_801840F0;
            case 0x80184100u: goto block_80184100;
            case 0x80184110u: goto block_80184110;
            case 0x8018413Cu: goto block_8018413C;
            case 0x80184150u: goto block_80184150;
            case 0x8018417Cu: goto block_8018417C;
            case 0x80184190u: goto block_80184190;
            case 0x801841BCu: goto block_801841BC;
            case 0x801841D0u: goto block_801841D0;
            case 0x801841FCu: goto block_801841FC;
            case 0x80184210u: goto block_80184210;
            case 0x80184228u: goto block_80184228;
            case 0x8018422Cu: goto block_8018422C;
            case 0x8018423Cu: goto block_8018423C;
            case 0x80184250u: goto block_80184250;
            case 0x80184260u: goto block_80184260;
            case 0x80184278u: goto block_80184278;
            case 0x80184280u: goto block_80184280;
            case 0x8018429Cu: goto block_8018429C;
            case 0x801842B4u: goto block_801842B4;
            case 0x801842BCu: goto block_801842BC;
            case 0x801842CCu: goto block_801842CC;
            case 0x801842DCu: goto block_801842DC;
            case 0x801842F0u: goto block_801842F0;
            case 0x801842FCu: goto block_801842FC;
            case 0x8018430Cu: goto block_8018430C;
            case 0x8018431Cu: goto block_8018431C;
            case 0x80184330u: goto block_80184330;
            case 0x80184338u: goto block_80184338;
            case 0x80184348u: goto block_80184348;
            case 0x8018434Cu: goto block_8018434C;
            case 0x80184354u: goto block_80184354;
            case 0x80184358u: goto block_80184358;
            default: break;
        }
    }
    /* Address: 0x80183C30, Size: 1864 bytes, Blocks: 113 */

block_80183C30:
    sp = sp + -40;
    cpu->write_word(sp + 24, s2);
    s2 = a2;
    cpu->write_word(sp + 36, ra);
    cpu->write_word(sp + 32, s4);
    cpu->write_word(sp + 28, s3);
    cpu->write_word(sp + 20, s1);
    cpu->write_word(sp + 16, s0);
    s1 = cpu->read_word(s2 + 36);
    /* nop */  /* 0x80183C54: 0x00000000 */
    v0 = cpu->read_half(s1);
    s3 = a0;
    s4 = (uint32_t)v0 >> 8;
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    s0 = cpu->read_word(s3 + 44);
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (s4 < (uint32_t)45) ? 1 : 0;  /* 0x80183C70: 0x2E82002D */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80183CC0u);
        goto block_80183CC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183C74u);
        goto block_80183C74;  /* not taken */
    }

block_80183C74:
    a0 = cpu->read_word(s2 + 120);
    /* nop */  /* 0x80183C78: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s2;
    v0 = ((int32_t)v0 < 188) ? 1 : 0;  /* 0x80183C84: 0x284200BC */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x801B << 16;  /* 0x801B0000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80183CBCu);
        goto block_80183CBC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183C90u);
        goto block_80183C90;  /* not taken */
    }

block_80183C90:
    v1 = v1 + 21568;
    v0 = s4 << 2;
    v0 = v0 + v1;
    a1 = 0x8012 << 16;  /* 0x80120000 */
    a2 = cpu->read_word(v0);
    ra = 0x80183CACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 13752;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_80183CAC:
    v1 = cpu->read_word(s2 + 120);
    /* nop */  /* 0x80183CB0: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s2 + 120, v1);
    /* fall through to block_80183CBC */

block_80183CBC:
    v0 = (s4 < (uint32_t)45) ? 1 : 0;  /* 0x80183CBC: 0x2E82002D */
    /* fall through to block_80183CC0 */

block_80183CC0:;
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x80183CE4u);
        goto block_80183CE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183CC8u);
        goto block_80183CC8;  /* not taken */
    }

block_80183CC8:
    v0 = v0 + 13764;
    v1 = s4 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80183CD8: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183CE0: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

block_80183CE4:
    v1 = cpu->read_word(s3 + 88);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183CF8u);
    goto block_80183CF8;  /* j */

block_80183CF0:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    /* fall through to block_80183CF8 */

block_80183CF8:
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = v1 & 0x8000;  /* 0x80183D00: 0x30638000 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

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
    v1 = v1 & 0x0800;  /* 0x80183D58: 0x30630800 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80183D64:
    v0 = 1;
    v1 = cpu->read_word(s3 + 88);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = (uint32_t)v1 >> 11;
    /* delay slot (always executes) */
    v1 = v1 ^ v0;  /* 0x80183D7C: 0x00621826 */
    psx_check_interrupts_at(cpu, 0x80183FE4u);
    goto block_80183FE4;  /* j */

block_80183D80:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = (uint32_t)v1 >> 11;
    /* delay slot (always executes) */
    v1 = v1 ^ v0;  /* 0x80183D98: 0x00621826 */
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
    v1 = v1 & a0;  /* 0x80183DBC: 0x00641824 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80183DC8:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x80183DCC: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183DE4u);
        goto block_80183DE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183DD8u);
        goto block_80183DD8;  /* not taken */
    }

block_80183DD8:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 186);
    /* nop */  /* 0x80183DDC: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183DE0: 0x0062182B */
    /* fall through to block_80183DE4 */

block_80183DE4:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183DF4:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80183DF8: 0x00000000 */
    int cond_v0_ge_0_2 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_2) {
        psx_check_interrupts_at(cpu, 0x80183E10u);
        goto block_80183E10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E04u);
        goto block_80183E04;  /* not taken */
    }

block_80183E04:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 186);
    /* nop */  /* 0x80183E08: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183E0C: 0x0062182B */
    /* fall through to block_80183E10 */

block_80183E10:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183E20:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x80183E24: 0x00000000 */
    int cond_v0_ge_0_3 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_3) {
        psx_check_interrupts_at(cpu, 0x80183E4Cu);
        goto block_80183E4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E30u);
        goto block_80183E30;  /* not taken */
    }

block_80183E30:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 186);
    /* nop */  /* 0x80183E34: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183E50u);
        goto block_80183E50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E40u);
        goto block_80183E40;  /* not taken */
    }

block_80183E40:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 184);
    /* nop */  /* 0x80183E44: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183E48: 0x0062182B */
    /* fall through to block_80183E4C */

block_80183E4C:
    v0 = 1;
    /* fall through to block_80183E50 */

block_80183E50:
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183E5C:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80183E60: 0x00000000 */
    int cond_v0_ge_0_4 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_4) {
        psx_check_interrupts_at(cpu, 0x80183E88u);
        goto block_80183E88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E6Cu);
        goto block_80183E6C;  /* not taken */
    }

block_80183E6C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 186);
    /* nop */  /* 0x80183E70: 0x00000000 */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80183E8Cu);
        goto block_80183E8C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183E7Cu);
        goto block_80183E7C;  /* not taken */
    }

block_80183E7C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 184);
    /* nop */  /* 0x80183E80: 0x00000000 */
    v1 = (v1 < v0) ? 1 : 0;  /* 0x80183E84: 0x0062182B */
    /* fall through to block_80183E88 */

block_80183E88:
    v0 = 1;
    /* fall through to block_80183E8C */

block_80183E8C:
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183E98:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x80183E9C: 0x00000000 */
    int cond_v0_ge_0_5 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_5) {
        psx_check_interrupts_at(cpu, 0x80183EB4u);
        goto block_80183EB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183EA8u);
        goto block_80183EA8;  /* not taken */
    }

block_80183EA8:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 188);
    /* nop */  /* 0x80183EAC: 0x00000000 */
    v1 = ((int32_t)v1 < (int32_t)v0) ? 1 : 0;  /* 0x80183EB0: 0x0062182A */
    /* fall through to block_80183EB4 */

block_80183EB4:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183EC4:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80183EC8: 0x00000000 */
    int cond_v0_ge_0_6 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_6) {
        psx_check_interrupts_at(cpu, 0x80183EE0u);
        goto block_80183EE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183ED4u);
        goto block_80183ED4;  /* not taken */
    }

block_80183ED4:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 188);
    /* nop */  /* 0x80183ED8: 0x00000000 */
    v1 = ((int32_t)v1 < (int32_t)v0) ? 1 : 0;  /* 0x80183EDC: 0x0062182A */
    /* fall through to block_80183EE0 */

block_80183EE0:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183EF0:
    v1 = cpu->read_byte(s3 + 5);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183FDCu);
    goto block_80183FDC;  /* j */

block_80183EFC:
    v1 = cpu->read_byte(s0 + 5);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183FDCu);
    goto block_80183FDC;  /* j */

block_80183F08:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x80183F0C: 0x00000000 */
    int cond_v0_ge_0_7 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_7) {
        psx_check_interrupts_at(cpu, 0x80183F28u);
        goto block_80183F28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183F18u);
        goto block_80183F18;  /* not taken */
    }

block_80183F18:
    v0 = cpu->read_word(s3 + 108);
    v1 = 0x0100 << 16;  /* 0x01000000 */
    v0 = v0 & v1;  /* 0x80183F20: 0x00431024 */
    v1 = (zero < v0) ? 1 : 0;  /* 0x80183F24: 0x0002182B */
    /* fall through to block_80183F28 */

block_80183F28:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183F38:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80183F3C: 0x00000000 */
    int cond_v0_ge_0_8 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_8) {
        psx_check_interrupts_at(cpu, 0x80183F58u);
        goto block_80183F58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183F48u);
        goto block_80183F48;  /* not taken */
    }

block_80183F48:
    v0 = cpu->read_word(s0 + 108);
    v1 = 0x0100 << 16;  /* 0x01000000 */
    v0 = v0 & v1;  /* 0x80183F50: 0x00431024 */
    v1 = (zero < v0) ? 1 : 0;  /* 0x80183F54: 0x0002182B */
    /* fall through to block_80183F58 */

block_80183F58:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183F68:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x80183F6C: 0x00000000 */
    int cond_v0_ge_0_9 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_9) {
        psx_check_interrupts_at(cpu, 0x80183F88u);
        goto block_80183F88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183F78u);
        goto block_80183F78;  /* not taken */
    }

block_80183F78:
    v0 = cpu->read_word(s3 + 108);
    v1 = 0x0200 << 16;  /* 0x02000000 */
    v0 = v0 & v1;  /* 0x80183F80: 0x00431024 */
    v1 = (zero < v0) ? 1 : 0;  /* 0x80183F84: 0x0002182B */
    /* fall through to block_80183F88 */

block_80183F88:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183F98:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80183F9C: 0x00000000 */
    int cond_v0_ge_0_10 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_10) {
        psx_check_interrupts_at(cpu, 0x80183FB8u);
        goto block_80183FB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80183FA8u);
        goto block_80183FA8;  /* not taken */
    }

block_80183FA8:
    v0 = cpu->read_word(s0 + 108);
    v1 = 0x0200 << 16;  /* 0x02000000 */
    v0 = v0 & v1;  /* 0x80183FB0: 0x00431024 */
    v1 = (zero < v0) ? 1 : 0;  /* 0x80183FB4: 0x0002182B */
    /* fall through to block_80183FB8 */

block_80183FB8:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80183FC8:
    v1 = cpu->read_word(s3 + 88);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80183FDCu);
    goto block_80183FDC;  /* j */

block_80183FD4:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    /* fall through to block_80183FDC */

block_80183FDC:
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    /* fall through to block_80183FE4 */

block_80183FE4:
    v1 = v1 & v0;  /* 0x80183FE4: 0x00621824 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80183FF0:
    v1 = cpu->read_word(s3 + 88);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80184004u);
    goto block_80184004;  /* j */

block_80183FFC:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    /* fall through to block_80184004 */

block_80184004:
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = v1 & 0x0010;  /* 0x8018400C: 0x30630010 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184018:
    v1 = cpu->read_word(s3 + 88);
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x8018402Cu);
    goto block_8018402C;  /* j */

block_80184024:
    v0 = 1;
    v1 = cpu->read_word(s0 + 88);
    /* fall through to block_8018402C */

block_8018402C:
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    v1 = v1 & 0x0020;  /* 0x80184034: 0x30630020 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_80184040:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x80184044: 0x00000000 */
    int cond_v0_ge_0_11 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_11) {
        psx_check_interrupts_at(cpu, 0x80184060u);
        goto block_80184060;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184050u);
        goto block_80184050;  /* not taken */
    }

block_80184050:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 188);
    /* nop */  /* 0x80184054: 0x00000000 */
    int cond_v0_le_0 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_le_0) {
        psx_check_interrupts_at(cpu, 0x80184068u);
        goto block_80184068;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184060u);
        goto block_80184060;  /* not taken */
    }

block_80184060:
    v1 = 1;
    v0 = 1;
    /* fall through to block_80184068 */

block_80184068:
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184074:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80184078: 0x00000000 */
    int cond_v0_ge_0_12 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ge_0_12) {
        psx_check_interrupts_at(cpu, 0x80184094u);
        goto block_80184094;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184084u);
        goto block_80184084;  /* not taken */
    }

block_80184084:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 188);
    /* nop */  /* 0x80184088: 0x00000000 */
    int cond_v0_le_0_2 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_le_0_2) {
        psx_check_interrupts_at(cpu, 0x8018409Cu);
        goto block_8018409C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184094u);
        goto block_80184094;  /* not taken */
    }

block_80184094:
    v1 = 1;
    v0 = 1;
    /* fall through to block_8018409C */

block_8018409C:
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_801840A8:
    v0 = cpu->read_byte(s3 + 5);
    /* nop */  /* 0x801840AC: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x801840B0: 0x30420001 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x801840CCu);
        goto block_801840CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801840BCu);
        goto block_801840BC;  /* not taken */
    }

block_801840BC:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 214);
    /* nop */  /* 0x801840C0: 0x00000000 */
    v0 = ((int32_t)v0 < 7680) ? 1 : 0;  /* 0x801840C4: 0x28421E00 */
    v1 = v0 ^ 0x0001;  /* 0x801840C8: 0x38430001 */
    /* fall through to block_801840CC */

block_801840CC:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_801840DC:
    v0 = cpu->read_byte(s0 + 5);
    /* nop */  /* 0x801840E0: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x801840E4: 0x30420001 */
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = zero;
    if (cond_v0_ne_zero_4) {
        psx_check_interrupts_at(cpu, 0x80184100u);
        goto block_80184100;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801840F0u);
        goto block_801840F0;  /* not taken */
    }

block_801840F0:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 214);
    /* nop */  /* 0x801840F4: 0x00000000 */
    v0 = ((int32_t)v0 < 7680) ? 1 : 0;  /* 0x801840F8: 0x28421E00 */
    v1 = v0 ^ 0x0001;  /* 0x801840FC: 0x38430001 */
    /* fall through to block_80184100 */

block_80184100:
    v0 = 1;
    cpu->write_word(s2 + 56, v1);
    /* delay slot (always executes) */
    v1 = s1 + 2;
    psx_check_interrupts_at(cpu, 0x80184354u);
    goto block_80184354;  /* j */

block_80184110:
    a1 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a0 = cpu->read_word(s3 + 100);
    v0 = s1 + 6;
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a1 + v1;
    a0 = a0 & a2;  /* 0x80184130: 0x00862024 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, a0);
    psx_check_interrupts_at(cpu, 0x80184278u);
    goto block_80184278;  /* j */

block_8018413C:
    a0 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a1 = cpu->read_word(s0 + 100);
    /* delay slot (always executes) */
    v0 = s1 + 6;
    psx_check_interrupts_at(cpu, 0x80184260u);
    goto block_80184260;  /* j */

block_80184150:
    a1 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a0 = cpu->read_word(s3 + 92);
    v0 = s1 + 6;
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a1 + v1;
    a0 = a0 & a2;  /* 0x80184170: 0x00862024 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, a0);
    psx_check_interrupts_at(cpu, 0x80184278u);
    goto block_80184278;  /* j */

block_8018417C:
    a0 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a1 = cpu->read_word(s0 + 92);
    /* delay slot (always executes) */
    v0 = s1 + 6;
    psx_check_interrupts_at(cpu, 0x80184260u);
    goto block_80184260;  /* j */

block_80184190:
    a1 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a0 = cpu->read_word(s3 + 88);
    v0 = s1 + 6;
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a1 + v1;
    a0 = a0 & a2;  /* 0x801841B0: 0x00862024 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, a0);
    psx_check_interrupts_at(cpu, 0x80184278u);
    goto block_80184278;  /* j */

block_801841BC:
    a0 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a1 = cpu->read_word(s0 + 88);
    /* delay slot (always executes) */
    v0 = s1 + 6;
    psx_check_interrupts_at(cpu, 0x80184260u);
    goto block_80184260;  /* j */

block_801841D0:
    a1 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a0 = cpu->read_word(s3 + 104);
    v0 = s1 + 6;
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a1 + v1;
    a0 = a0 & a2;  /* 0x801841F0: 0x00862024 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, a0);
    psx_check_interrupts_at(cpu, 0x80184278u);
    goto block_80184278;  /* j */

block_801841FC:
    a0 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a1 = cpu->read_word(s0 + 104);
    /* delay slot (always executes) */
    v0 = s1 + 6;
    psx_check_interrupts_at(cpu, 0x80184260u);
    goto block_80184260;  /* j */

block_80184210:
    v0 = 1;
    v1 = (int32_t)(int16_t)cpu->read_half(s3 + 116);
    a0 = s1 + 2;
    cpu->write_word(s2 + 36, a0);
    int cond_v1_ge_0 = ((int32_t)v1 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80184224: 0x00000000 */
    if (cond_v1_ge_0) {
        psx_check_interrupts_at(cpu, 0x8018422Cu);
        goto block_8018422C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184228u);
        goto block_80184228;  /* not taken */
    }

block_80184228:
    v1 = zero - v1;
    /* fall through to block_8018422C */

block_8018422C:
    v1 = ((int32_t)v1 < 1707) ? 1 : 0;  /* 0x8018422C: 0x286306AB */
    v1 = v1 ^ v0;  /* 0x80184230: 0x00621826 */
    /* delay slot (always executes) */
    cpu->write_word(s2 + 56, v1);
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_8018423C:
    a0 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a1 = cpu->read_word(s3 + 108);
    /* delay slot (always executes) */
    v0 = s1 + 6;
    psx_check_interrupts_at(cpu, 0x80184260u);
    goto block_80184260;  /* j */

block_80184250:
    a0 = cpu->read_half(s1 + 2);
    v1 = cpu->read_half(s1 + 4);
    a1 = cpu->read_word(s0 + 108);
    v0 = s1 + 6;
    /* fall through to block_80184260 */

block_80184260:
    cpu->write_word(s2 + 36, v0);
    v0 = (int32_t)(int16_t)cpu->read_half(s2 + 110);
    v1 = v1 << 16;
    a2 = a0 + v1;
    a2 = a2 & a1;  /* 0x80184270: 0x00C53024 */
    cpu->write_word(s2 + 56, a2);
    /* fall through to block_80184278 */

block_80184278:;
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x80184358u);
        goto block_80184358;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184280u);
        goto block_80184280;  /* not taken */
    }

block_80184280:
    a0 = cpu->read_word(s2 + 120);
    /* nop */  /* 0x80184284: 0x00000000 */
    v0 = a0 + -124;
    v0 = v0 - s2;
    v0 = ((int32_t)v0 < 191) ? 1 : 0;  /* 0x80184290: 0x284200BF */
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_5) {
        psx_check_interrupts_at(cpu, 0x801842B4u);
        goto block_801842B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018429Cu);
        goto block_8018429C;  /* not taken */
    }

block_8018429C:
    ra = 0x801842A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = a1 + 13760;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_801842A4:
    v1 = cpu->read_word(s2 + 120);
    /* nop */  /* 0x801842A8: 0x00000000 */
    v1 = v1 + v0;
    cpu->write_word(s2 + 120, v1);
    /* fall through to block_801842B4 */

block_801842B4:
    /* delay slot (always executes) */
    v0 = 1;
    psx_check_interrupts_at(cpu, 0x80184358u);
    goto block_80184358;  /* j */

block_801842BC:
    v0 = cpu->read_word(s3 + 88);
    /* nop */  /* 0x801842C0: 0x00000000 */
    int cond_v0_ge_0_13 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    if (cond_v0_ge_0_13) {
        psx_check_interrupts_at(cpu, 0x80184348u);
        goto block_80184348;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801842CCu);
        goto block_801842CC;  /* not taken */
    }

block_801842CC:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 186);
    /* nop */  /* 0x801842D0: 0x00000000 */
    int cond_v0_le_0_3 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0004 << 16;  /* 0x00040000 */
    if (cond_v0_le_0_3) {
        psx_check_interrupts_at(cpu, 0x80184348u);
        goto block_80184348;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801842DCu);
        goto block_801842DC;  /* not taken */
    }

block_801842DC:
    v0 = cpu->read_word(s3 + 92);
    v1 = v1 | 0x0080;
    v0 = v0 & v1;  /* 0x801842E4: 0x00431024 */
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x8018434Cu);
        goto block_8018434C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801842F0u);
        goto block_801842F0;  /* not taken */
    }

block_801842F0:
    v0 = cpu->read_word(s3 + 100);
    /* delay slot (always executes) */
    v1 = 0x4000 << 16;  /* 0x40000000 */
    psx_check_interrupts_at(cpu, 0x80184338u);
    goto block_80184338;  /* j */

block_801842FC:
    v0 = cpu->read_word(s0 + 88);
    /* nop */  /* 0x80184300: 0x00000000 */
    int cond_v0_ge_0_14 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    if (cond_v0_ge_0_14) {
        psx_check_interrupts_at(cpu, 0x80184348u);
        goto block_80184348;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018430Cu);
        goto block_8018430C;  /* not taken */
    }

block_8018430C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 186);
    /* nop */  /* 0x80184310: 0x00000000 */
    int cond_v0_le_0_4 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x0004 << 16;  /* 0x00040000 */
    if (cond_v0_le_0_4) {
        psx_check_interrupts_at(cpu, 0x80184348u);
        goto block_80184348;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018431Cu);
        goto block_8018431C;  /* not taken */
    }

block_8018431C:
    v0 = cpu->read_word(s0 + 92);
    v1 = v1 | 0x0080;
    v0 = v0 & v1;  /* 0x80184324: 0x00431024 */
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_ne_zero_6) {
        psx_check_interrupts_at(cpu, 0x8018434Cu);
        goto block_8018434C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80184330u);
        goto block_80184330;  /* not taken */
    }

block_80184330:
    v1 = 0x4000 << 16;  /* 0x40000000 */
    v0 = cpu->read_word(s0 + 100);
    /* fall through to block_80184338 */

block_80184338:
    v1 = v1 | 0x2000;
    v0 = v0 & v1;  /* 0x8018433C: 0x00431024 */
    v0 = v0 ^ v1;  /* 0x80184340: 0x00431026 */
    a0 = (v0 < (uint32_t)1) ? 1 : 0;  /* 0x80184344: 0x2C440001 */
    /* fall through to block_80184348 */

block_80184348:
    v0 = 1;
    /* fall through to block_8018434C */

block_8018434C:
    v1 = s1 + 2;
    cpu->write_word(s2 + 56, a0);
    /* fall through to block_80184354 */

block_80184354:
    cpu->write_word(s2 + 36, v1);
    /* fall through to block_80184358 */

block_80184358:
    ra = cpu->read_word(sp + 36);
    s4 = cpu->read_word(sp + 32);
    s3 = cpu->read_word(sp + 28);
    s2 = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/* CPS piece-entry wrappers — each resumes the shared body at its
 * own block via cpu->pc (set by the CLEAN_ENT_CPS dispatch). */

/* scriptop_wait_cps_80183964 — guest script-op entry 0x80183964: runs the
 * shared scriptop_wait_cps_body on live CPU state. */
uint32_t scriptop_wait_cps_80183964(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_wait_cps_body(c);
  return c->gpr[2];
}

/* scriptop_wait_cps_8018397c — guest script-op entry 0x8018397C: runs the
 * shared scriptop_wait_cps_body on live CPU state. */
uint32_t scriptop_wait_cps_8018397c(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_wait_cps_body(c);
  return c->gpr[2];
}

/* scriptop_wait_cps_80183988 — guest script-op entry 0x80183988: runs the
 * shared scriptop_wait_cps_body on live CPU state. */
uint32_t scriptop_wait_cps_80183988(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_wait_cps_body(c);
  return c->gpr[2];
}

/* scriptop_wait_cps_80183a48 — guest script-op entry 0x80183A48: runs the
 * shared scriptop_wait_cps_body on live CPU state. */
uint32_t scriptop_wait_cps_80183a48(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_wait_cps_body(c);
  return c->gpr[2];
}

/* scriptop_wait_cps_80183a74 — guest script-op entry 0x80183A74: runs the
 * shared scriptop_wait_cps_body on live CPU state. */
uint32_t scriptop_wait_cps_80183a74(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_wait_cps_body(c);
  return c->gpr[2];
}

/* scriptop_wait_cps_80183aa4 — guest script-op entry 0x80183AA4: runs the
 * shared scriptop_wait_cps_body on live CPU state. */
uint32_t scriptop_wait_cps_80183aa4(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_wait_cps_body(c);
  return c->gpr[2];
}

/* scriptop_chk_cps_80183ce4 — guest script-op entry 0x80183CE4: runs the
 * shared scriptop_chk_cps_body on live CPU state. */
uint32_t scriptop_chk_cps_80183ce4(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_chk_cps_body(c);
  return c->gpr[2];
}

/* scriptop_chk_cps_80183cf0 — guest script-op entry 0x80183CF0: runs the
 * shared scriptop_chk_cps_body on live CPU state. */
uint32_t scriptop_chk_cps_80183cf0(void)
{
  CPUState *c = clean_cpu_state();
  if (!c) return 0;
  scriptop_chk_cps_body(c);
  return c->gpr[2];
}

