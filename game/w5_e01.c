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
/* w5_e01.c — clean CPS ports for five unnamed jal-target functions
 * (worktree orch-w5-e01):
 *   0x8019E600 gpu_state_patch_submit_3w
 *   0x801A0DDC cd_hw_chan_release_all
 *   0x801A2560 cd_cmd_rx_state_default
 *   0x801A2808 cd_cmd_rec_arm_extent
 *   0x801A2EC8 cd_cmd_rec_arm_op
 *
 * Each body is a faithful CPS transliteration of its generated
 * br2eu_func_* body (game/gen_cps_clean.py transform): cpu->pc
 * continuation switch + entry fallthrough, block labels + goto,
 * psx_check_interrupts_at at every yield/dispatch boundary, and
 * cpu->pc=<callee>; return tail-dispatches. Memory ops use the
 * MMIO-aware cpu->read/write accessors; named GPRs alias
 * cpu->gpr[]. Piece addrs are registered in game/clean_cps.map;
 * entry addrs in game/renames.map (except 0x801A2560 — see its
 * comment for the call_guest single-shot constraint).
 */

/* gpu_state_patch_submit_3w (guest 0x8019E600) — DI; B0 svc 0x56 (GetC0Table, live a0-a3) -> obj; p = *(obj+0x18); copy 3 words 0x801AE670..0x801AE67C -> p+0x70..; jal 0x801957F0 (gpu_state_patch_defer); EI; restore ra, jr ra.
 * Sibling of the bind.c FUN_8019E4C8/FUN_8019E55C patch-submit family. */
/* faithful CPS transliteration of br2eu_func_8019E600 (guest 0x8019e600)
 * from SLES_017.22_full_48.c. */
void gpu_state_patch_submit_3w(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019E610u: goto block_8019E610;
            case 0x8019E620u: goto block_8019E620;
            case 0x8019E654u: goto block_8019E654;
            case 0x8019E65Cu: goto block_8019E65C;
            case 0x8019E634u: goto block_8019E634;
            case 0x8019E64Cu: goto block_8019E64C;
            default: break;
        }
    }
    /* Address: 0x8019E600, Size: 112 bytes, Blocks: 7 */

block_8019E600:
    at = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(at + -5216, ra);
    ra = 0x8019E610u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E60C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019E610:
    t1 = 86;
    t2 = 176;
    uint32_t jr_target = t2;  /* latch indirect target before delay slot */
    ra = 0x8019E620u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E61C: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS jalr */

block_8019E620:
    v0 = cpu->read_word(v0 + 24);
    t2 = 0x801A << 16;  /* 0x801A0000 */
    t2 = t2 + -6544;
    t1 = 0x801A << 16;  /* 0x801A0000 */
    t1 = t1 + -6532;
    /* fall through to block_8019E634 */

block_8019E634:
    v1 = cpu->read_word(t2);
    /* nop */  /* 0x8019E638: 0x00000000 */
    cpu->write_word(v0 + 112, v1);
    t2 = t2 + 4;
    int cond_t2_ne_t1 = (t2 != t1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v0 + 4;
    if (cond_t2_ne_t1) {
        psx_check_interrupts_at(cpu, 0x8019E634u);
        goto block_8019E634;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019E64Cu);
        goto block_8019E64C;  /* not taken */
    }

block_8019E64C:
    ra = 0x8019E654u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E650: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801957F0u);
    cpu->pc = 0x801957F0u; return;  /* CPS jal -> 0x801957F0 */

block_8019E654:
    ra = 0x8019E65Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E658: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_8019E65C:
    ra = 0x8020 << 16;  /* 0x80200000 */
    ra = cpu->read_word(ra + -5216);
    /* nop */  /* 0x8019E664: 0x00000000 */
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E66C: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/* cd_hw_chan_release_all (guest 0x801A0DDC) — DI; for i=0..7 jal 0x801A11C0 (bios_tramp_b009) with a0 = *(0x8020EF70+4i) — the eight channel objects allocated by cd_hw_chan_init; EI only when the saved DI return == 1. Called by cd_block7_init (0x8019E6F0). */
/* faithful CPS transliteration of br2eu_func_801A0DDC (guest 0x801a0ddc)
 * from SLES_017.22_full_49.c. */
void cd_hw_chan_release_all(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A0DECu: goto block_801A0DEC;
            case 0x801A0DFCu: goto block_801A0DFC;
            case 0x801A0E0Cu: goto block_801A0E0C;
            case 0x801A0E1Cu: goto block_801A0E1C;
            case 0x801A0E2Cu: goto block_801A0E2C;
            case 0x801A0E3Cu: goto block_801A0E3C;
            case 0x801A0E4Cu: goto block_801A0E4C;
            case 0x801A0E5Cu: goto block_801A0E5C;
            case 0x801A0E6Cu: goto block_801A0E6C;
            case 0x801A0E80u: goto block_801A0E80;
            case 0x801A0E78u: goto block_801A0E78;
            default: break;
        }
    }
    /* Address: 0x801A0DDC, Size: 180 bytes, Blocks: 12 */

block_801A0DDC:
    sp = sp + -24;
    cpu->write_word(sp + 20, ra);
    ra = 0x801A0DECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, s0);
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_801A0DEC:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4240);
    ra = 0x801A0DFCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    s0 = v0;
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0DFC:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4236);
    ra = 0x801A0E0Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E08: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0E0C:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4232);
    ra = 0x801A0E1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E18: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0E1C:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4228);
    ra = 0x801A0E2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E28: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0E2C:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4224);
    ra = 0x801A0E3Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0E3C:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4220);
    ra = 0x801A0E4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E48: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0E4C:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4216);
    ra = 0x801A0E5Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E58: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0E5C:
    a0 = 0x8020 << 16;  /* 0x80200000 */
    a0 = cpu->read_word(a0 + -4212);
    ra = 0x801A0E6Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E68: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A11C0u);
    cpu->pc = 0x801A11C0u; return;  /* CPS jal -> 0x801A11C0 */

block_801A0E6C:
    v0 = 1;
    int cond_s0_ne_v0 = (s0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E74: 0x00000000 */
    if (cond_s0_ne_v0) {
        psx_check_interrupts_at(cpu, 0x801A0E80u);
        goto block_801A0E80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0E78u);
        goto block_801A0E78;  /* not taken */
    }

block_801A0E78:
    ra = 0x801A0E80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0E7C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_801A0E80:
    ra = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 24;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}

/* cd_cmd_rx_state_default (guest 0x801A2560) — default rec[24] RX-state handler invoked by cd_cmd_state_rx when the record has no custom fn. Dispatches on rec[70] (b70): state handlers compute extent offsets (cd_extent_len 0x801A27D0 family), arm handler pairs via cd_cmd_rec_arm_extent 0x801A2808, or jalr the *(0x801BE784) notify cell. NOTE: entry addr 0x801A2560 is NOT clean-registered — clean cd_cmd_state_rx reaches it through call_guest(), which can only drive a single CPS piece; the generated entry chains into these clean continuation pieces via cpu->pc. */
/* faithful CPS transliteration of br2eu_func_801A2560 (guest 0x801a2560)
 * from SLES_017.22_full_49.c. */
void cd_cmd_rx_state_default(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A273Cu: goto block_801A273C;
            case 0x801A275Cu: goto block_801A275C;
            case 0x801A27B0u: goto block_801A27B0;
            case 0x801A2580u: goto block_801A2580;
            case 0x801A2588u: goto block_801A2588;
            case 0x801A2590u: goto block_801A2590;
            case 0x801A2598u: goto block_801A2598;
            case 0x801A25A4u: goto block_801A25A4;
            case 0x801A25ACu: goto block_801A25AC;
            case 0x801A25C4u: goto block_801A25C4;
            case 0x801A25D8u: goto block_801A25D8;
            case 0x801A25ECu: goto block_801A25EC;
            case 0x801A2600u: goto block_801A2600;
            case 0x801A2614u: goto block_801A2614;
            case 0x801A261Cu: goto block_801A261C;
            case 0x801A2620u: goto block_801A2620;
            case 0x801A2664u: goto block_801A2664;
            case 0x801A266Cu: goto block_801A266C;
            case 0x801A2684u: goto block_801A2684;
            case 0x801A2694u: goto block_801A2694;
            case 0x801A26B4u: goto block_801A26B4;
            case 0x801A26B8u: goto block_801A26B8;
            case 0x801A26C0u: goto block_801A26C0;
            case 0x801A26D4u: goto block_801A26D4;
            case 0x801A26ECu: goto block_801A26EC;
            case 0x801A26FCu: goto block_801A26FC;
            case 0x801A2734u: goto block_801A2734;
            case 0x801A2748u: goto block_801A2748;
            case 0x801A2770u: goto block_801A2770;
            case 0x801A2784u: goto block_801A2784;
            case 0x801A2794u: goto block_801A2794;
            case 0x801A27BCu: goto block_801A27BC;
            case 0x801A27C0u: goto block_801A27C0;
            default: break;
        }
    }
    /* Address: 0x801A2560, Size: 624 bytes, Blocks: 34 */

block_801A2560:
    sp = sp + -24;
    cpu->write_word(sp + 16, s0);
    s0 = a0;
    cpu->write_word(sp + 20, ra);
    v1 = cpu->read_byte(s0 + 70);
    v0 = 3;
    int cond_v1_eq_v0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 4) ? 1 : 0;  /* 0x801A257C: 0x28620004 */
    if (cond_v1_eq_v0) {
        psx_check_interrupts_at(cpu, 0x801A266Cu);
        goto block_801A266C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2580u);
        goto block_801A2580;  /* not taken */
    }

block_801A2580:;
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801A2598u);
        goto block_801A2598;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2588u);
        goto block_801A2588;  /* not taken */
    }

block_801A2588:;
    int cond_v1_eq_v0_2 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v1_eq_v0_2) {
        psx_check_interrupts_at(cpu, 0x801A25ACu);
        goto block_801A25AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2590u);
        goto block_801A2590;  /* not taken */
    }

block_801A2590:
    /* delay slot (always executes) */
    /* nop */  /* 0x801A2594: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A27C0u);
    goto block_801A27C0;  /* j */

block_801A2598:
    v0 = 4;
    int cond_v1_eq_v0_3 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v1_eq_v0_3) {
        psx_check_interrupts_at(cpu, 0x801A26D4u);
        goto block_801A26D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A25A4u);
        goto block_801A25A4;  /* not taken */
    }

block_801A25A4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801A25A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A27C0u);
    goto block_801A27C0;  /* j */

block_801A25AC:
    a0 = cpu->read_word(s0 + 60);
    /* nop */  /* 0x801A25B0: 0x00000000 */
    v0 = cpu->read_byte(a0 + 7);
    /* nop */  /* 0x801A25B8: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801A27C0u);
        goto block_801A27C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A25C4u);
        goto block_801A25C4;  /* not taken */
    }

block_801A25C4:
    v1 = cpu->read_byte(s0 + 227);
    v0 = cpu->read_byte(a0 + 3);
    /* nop */  /* 0x801A25CC: 0x00000000 */
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xFFFF;
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x801A261Cu);
        goto block_801A261C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A25D8u);
        goto block_801A25D8;  /* not taken */
    }

block_801A25D8:
    v1 = cpu->read_byte(s0 + 228);
    v0 = cpu->read_byte(a0 + 4);
    /* nop */  /* 0x801A25E0: 0x00000000 */
    int cond_v1_ne_v0_2 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xFFFF;
    if (cond_v1_ne_v0_2) {
        psx_check_interrupts_at(cpu, 0x801A261Cu);
        goto block_801A261C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A25ECu);
        goto block_801A25EC;  /* not taken */
    }

block_801A25EC:
    v1 = cpu->read_byte(s0 + 233);
    v0 = cpu->read_byte(a0 + 5);
    /* nop */  /* 0x801A25F4: 0x00000000 */
    int cond_v1_ne_v0_3 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xFFFF;
    if (cond_v1_ne_v0_3) {
        psx_check_interrupts_at(cpu, 0x801A261Cu);
        goto block_801A261C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2600u);
        goto block_801A2600;  /* not taken */
    }

block_801A2600:
    v1 = cpu->read_byte(s0 + 234);
    v0 = cpu->read_byte(a0 + 6);
    /* nop */  /* 0x801A2608: 0x00000000 */
    int cond_v1_ne_v0_4 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xFFFF;
    if (cond_v1_ne_v0_4) {
        psx_check_interrupts_at(cpu, 0x801A261Cu);
        goto block_801A261C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2614u);
        goto block_801A2614;  /* not taken */
    }

block_801A2614:
    /* delay slot (always executes) */
    cpu->write_half(s0 + 238, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x801A2620u);
    goto block_801A2620;  /* j */

block_801A261C:
    cpu->write_half(s0 + 238, (uint16_t)v0);
    /* fall through to block_801A2620 */

block_801A2620:
    v0 = cpu->read_word(s0 + 60);
    v1 = cpu->read_word(s0 + 60);
    v0 = cpu->read_byte(v0 + 3);
    /* nop */  /* 0x801A262C: 0x00000000 */
    cpu->write_byte(s0 + 227, (uint8_t)v0);
    v0 = cpu->read_byte(v1 + 4);
    v1 = cpu->read_word(s0 + 60);
    cpu->write_half(s0 + 230, (uint16_t)zero);
    cpu->write_byte(s0 + 228, (uint8_t)v0);
    v0 = cpu->read_byte(v1 + 5);
    v1 = cpu->read_word(s0 + 60);
    cpu->write_byte(s0 + 233, (uint8_t)v0);
    v0 = cpu->read_byte(v1 + 6);
    v1 = cpu->read_half(s0 + 238);
    cpu->write_half(s0 + 236, (uint16_t)zero);
    int cond_v1_ne_zero = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 234, (uint8_t)v0);
    if (cond_v1_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801A26B8u);
        goto block_801A26B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2664u);
        goto block_801A2664;  /* not taken */
    }

block_801A2664:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 235, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x801A27BCu);
    goto block_801A27BC;  /* j */

block_801A266C:
    v1 = cpu->read_word(s0 + 60);
    /* nop */  /* 0x801A2670: 0x00000000 */
    v0 = cpu->read_byte(v1 + 2);
    /* nop */  /* 0x801A2678: 0x00000000 */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x801A27C0u);
        goto block_801A27C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2684u);
        goto block_801A2684;  /* not taken */
    }

block_801A2684:
    v0 = cpu->read_byte(v1 + 3);
    /* nop */  /* 0x801A2688: 0x00000000 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x801A27C0u);
        goto block_801A27C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2694u);
        goto block_801A2694;  /* not taken */
    }

block_801A2694:
    v0 = cpu->read_byte(v1 + 4);
    v1 = cpu->read_byte(v1 + 5);
    v0 = v0 << 8;
    a0 = v1 + v0;
    v1 = cpu->read_half(s0 + 238);
    v0 = a0 & 0xFFFF;  /* 0x801A26A8: 0x3082FFFF */
    int cond_v1_eq_v0_4 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 230, (uint16_t)a0);
    if (cond_v1_eq_v0_4) {
        psx_check_interrupts_at(cpu, 0x801A26C0u);
        goto block_801A26C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A26B4u);
        goto block_801A26B4;  /* not taken */
    }

block_801A26B4:
    cpu->write_half(s0 + 238, (uint16_t)a0);
    /* fall through to block_801A26B8 */

block_801A26B8:
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, 0x801A27C0u);
    goto block_801A27C0;  /* j */

block_801A26C0:
    v0 = 0xFFFF;
    cpu->write_half(s0 + 238, (uint16_t)v0);
    cpu->write_byte(s0 + 235, (uint8_t)zero);
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 71, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x801A27BCu);
    goto block_801A27BC;  /* j */

block_801A26D4:
    v1 = cpu->read_word(s0 + 60);
    /* nop */  /* 0x801A26D8: 0x00000000 */
    v0 = cpu->read_byte(v1 + 2);
    /* nop */  /* 0x801A26E0: 0x00000000 */
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero_4) {
        psx_check_interrupts_at(cpu, 0x801A27C0u);
        goto block_801A27C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A26ECu);
        goto block_801A26EC;  /* not taken */
    }

block_801A26EC:
    v0 = cpu->read_byte(v1 + 3);
    /* nop */  /* 0x801A26F0: 0x00000000 */
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x801A27C0u);
        goto block_801A27C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A26FCu);
        goto block_801A26FC;  /* not taken */
    }

block_801A26FC:
    a0 = cpu->read_half(s0 + 236);
    v1 = cpu->read_byte(v1 + 4);
    v0 = cpu->read_byte(s0 + 71);
    a0 = a0 + 8;
    v1 = v1 + 3;
    v1 = v1 & 0x01FC;  /* 0x801A2710: 0x306301FC */
    a0 = a0 + v1;
    v1 = cpu->read_byte(s0 + 234);
    v0 = v0 + 1;
    cpu->write_byte(s0 + 71, (uint8_t)v0);
    v0 = v0 & 0x00FF;  /* 0x801A2724: 0x304200FF */
    v0 = (v0 < v1) ? 1 : 0;  /* 0x801A2728: 0x0043102B */
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 236, (uint16_t)a0);
    if (cond_v0_ne_zero_6) {
        psx_check_interrupts_at(cpu, 0x801A26B8u);
        goto block_801A26B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2734u);
        goto block_801A2734;  /* not taken */
    }

block_801A2734:
    ra = 0x801A273Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x801A27D0u);
    cpu->pc = 0x801A27D0u; return;  /* CPS jal -> 0x801A27D0 */

block_801A273C:
    v0 = ((int32_t)v0 < 129) ? 1 : 0;  /* 0x801A273C: 0x28420081 */
    int cond_v0_ne_zero_7 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A2744: 0x00000000 */
    if (cond_v0_ne_zero_7) {
        psx_check_interrupts_at(cpu, 0x801A2770u);
        goto block_801A2770;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2748u);
        goto block_801A2748;  /* not taken */
    }

block_801A2748:
    v0 = 0x801C << 16;  /* 0x801C0000 */
    v0 = cpu->read_word(v0 + -6268);
    /* nop */  /* 0x801A2750: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    ra = 0x801A275Cu;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS jalr */

block_801A275C:
    v0 = 254;
    cpu->write_byte(s0 + 70, (uint8_t)v0);
    v0 = 2;
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 73, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x801A26B8u);
    goto block_801A26B8;  /* j */

block_801A2770:
    v1 = cpu->read_half(s0 + 236);
    v0 = cpu->read_half(s0 + 238);
    /* nop */  /* 0x801A2778: 0x00000000 */
    int cond_v0_eq_v1 = (v0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_eq_v1) {
        psx_check_interrupts_at(cpu, 0x801A2794u);
        goto block_801A2794;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2784u);
        goto block_801A2784;  /* not taken */
    }

block_801A2784:
    cpu->write_half(s0 + 238, (uint16_t)v1);
    cpu->write_byte(s0 + 71, (uint8_t)zero);
    /* delay slot (always executes) */
    cpu->write_half(s0 + 236, (uint16_t)zero);
    psx_check_interrupts_at(cpu, 0x801A27C0u);
    goto block_801A27C0;  /* j */

block_801A2794:
    a0 = s0;
    v0 = 255;
    a1 = s0 + 99;
    cpu->write_half(s0 + 238, (uint16_t)zero);
    cpu->write_byte(s0 + 235, (uint8_t)zero);
    ra = 0x801A27B0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 70, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x801A2808u);
    cpu->pc = 0x801A2808u; return;  /* CPS jal -> 0x801A2808 */

block_801A27B0:
    v0 = 2;
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 70, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x801A26B8u);
    goto block_801A26B8;  /* j */

block_801A27BC:
    v0 = 1;
    /* fall through to block_801A27C0 */

block_801A27C0:
    ra = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 24;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/* cd_cmd_rec_arm_extent (guest 0x801A2808) — gate jalr *(*(0x801BE79C)) must return 0; then rec[73]=4, rec[70]=1, rec[20]=0x801A28F4 (mid br2eu_func_801A28DC state-commit cb), rec[24]=0x801A298C (mid br2eu_func_801A2984 rx-state cb), and the extent table rec[0]/[4]/[8] is laid out from rec[227]/rec[233] (same math family as cd_extent_len). Returns 1 on arm, 0 when the gate reports busy. Called from cd_cmd_rx_state_default state-3. */
/* faithful CPS transliteration of br2eu_func_801A2808 (guest 0x801a2808)
 * from SLES_017.22_full_49.c. */
void cd_cmd_rec_arm_extent(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A2848u: goto block_801A2848;
            case 0x801A2824u: goto block_801A2824;
            case 0x801A2834u: goto block_801A2834;
            case 0x801A2850u: goto block_801A2850;
            case 0x801A2858u: goto block_801A2858;
            case 0x801A28C8u: goto block_801A28C8;
            default: break;
        }
    }
    /* Address: 0x801A2808, Size: 212 bytes, Blocks: 7 */

block_801A2808:
    sp = sp + -32;
    cpu->write_word(sp + 16, s0);
    s0 = a0;
    cpu->write_word(sp + 20, s1);
    s1 = a1;
    int cond_s1_eq_zero = (s1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, ra);
    if (cond_s1_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801A2850u);
        goto block_801A2850;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2824u);
        goto block_801A2824;  /* not taken */
    }

block_801A2824:
    v0 = cpu->read_word(s0 + 4);
    /* nop */  /* 0x801A2828: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801A28C8u);
        goto block_801A28C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2834u);
        goto block_801A2834;  /* not taken */
    }

block_801A2834:
    v0 = 0x801C << 16;  /* 0x801C0000 */
    v0 = cpu->read_word(v0 + -6244);
    /* nop */  /* 0x801A283C: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    ra = 0x801A2848u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A2844: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS jalr */

block_801A2848:;
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x801A2858u);
        goto block_801A2858;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2850u);
        goto block_801A2850;  /* not taken */
    }

block_801A2850:
    /* delay slot (always executes) */
    v0 = zero;
    psx_check_interrupts_at(cpu, 0x801A28C8u);
    goto block_801A28C8;  /* j */

block_801A2858:
    v1 = 4;
    a1 = s1 + 3;
    a1 = (int32_t)a1 >> 2;
    cpu->write_byte(s0 + 73, (uint8_t)v1);
    v1 = 1;
    cpu->write_byte(s0 + 70, (uint8_t)v1);
    v1 = 0x801A << 16;  /* 0x801A0000 */
    v1 = v1 + 10460;
    cpu->write_word(s0 + 20, v1);
    v1 = cpu->read_byte(s0 + 227);
    a0 = 0x801A << 16;  /* 0x801A0000 */
    a0 = a0 + 10628;
    cpu->write_word(s0 + 24, a0);
    a0 = cpu->read_byte(s0 + 233);
    a1 = a1 << 2;
    cpu->write_word(s0, a1);
    cpu->write_byte(s0 + 71, (uint8_t)zero);
    v1 = v1 + 1;
    v1 = (int32_t)v1 >> 1;
    v1 = v1 << 2;
    a1 = a1 + v1;
    v1 = a0 << 2;
    v1 = v1 + a0;
    v1 = v1 + 3;
    v1 = v1 & 0x0FFC;  /* 0x801A28B8: 0x30630FFC */
    cpu->write_word(s0 + 4, a1);
    a1 = a1 + v1;
    cpu->write_word(s0 + 8, a1);
    /* fall through to block_801A28C8 */

block_801A28C8:
    ra = cpu->read_word(sp + 24);
    s1 = cpu->read_word(sp + 20);
    s0 = cpu->read_word(sp + 16);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 32;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}

/* cd_cmd_rec_arm_op (guest 0x801A2EC8) — gate jalr *(*(0x801BE79C)) must return 0; then rec[70]=1, rec[20]=0x801A2F70 (mid br2eu_func_801A2F60), rec[24]=0x801A2FC4 (mid op_state_dispatch 0x801A2FB4), rec[81]=a1, rec[82]=a2, rec[83]=((a1&0xFF)^rec[228])==0. Returns 1 on arm, 0 when busy. Called by 0x801A1654 with a0=gate-result record, a1/a2 = byte params. */
/* faithful CPS transliteration of br2eu_func_801A2EC8 (guest 0x801a2ec8)
 * from SLES_017.22_full_49.c. */
void cd_cmd_rec_arm_op(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A2EFCu: goto block_801A2EFC;
            case 0x801A2F04u: goto block_801A2F04;
            case 0x801A2F44u: goto block_801A2F44;
            default: break;
        }
    }
    /* Address: 0x801A2EC8, Size: 152 bytes, Blocks: 4 */

block_801A2EC8:
    sp = sp + -40;
    cpu->write_word(sp + 16, s0);
    s0 = a0;
    cpu->write_word(sp + 20, s1);
    s1 = a1;
    cpu->write_word(sp + 24, s2);
    s2 = a2;
    cpu->write_word(sp + 28, s3);
    v0 = 0x801C << 16;  /* 0x801C0000 */
    v0 = cpu->read_word(v0 + -6244);
    cpu->write_word(sp + 32, ra);
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    ra = 0x801A2EFCu;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    s3 = s1;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS jalr */

block_801A2EFC:;
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = zero;
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801A2F44u);
        goto block_801A2F44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A2F04u);
        goto block_801A2F04;  /* not taken */
    }

block_801A2F04:
    v0 = 1;
    a0 = cpu->read_byte(s0 + 228);
    v1 = 1;
    cpu->write_byte(s0 + 70, (uint8_t)v1);
    v1 = 0x801A << 16;  /* 0x801A0000 */
    v1 = v1 + 12128;
    cpu->write_word(s0 + 20, v1);
    v1 = 0x801A << 16;  /* 0x801A0000 */
    v1 = v1 + 12212;
    cpu->write_word(s0 + 24, v1);
    v1 = s3 & 0x00FF;  /* 0x801A2F2C: 0x326300FF */
    cpu->write_byte(s0 + 81, (uint8_t)s1);
    cpu->write_byte(s0 + 82, (uint8_t)s2);
    v1 = v1 ^ a0;  /* 0x801A2F38: 0x00641826 */
    v1 = (v1 < (uint32_t)1) ? 1 : 0;  /* 0x801A2F3C: 0x2C630001 */
    cpu->write_byte(s0 + 83, (uint8_t)v1);
    /* fall through to block_801A2F44 */

block_801A2F44:
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

