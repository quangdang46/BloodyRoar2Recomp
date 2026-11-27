/* w3_e06.c — clean CPS ports for the 0x80157948-0x80158230 alias-entry
 * family (worktree orch-w3-e06).
 *
 * All eleven requested addrs are entries into two overlapping-alias CPS
 * bodies in generated/SLES_017.22_full_20.c, NOT standalone functions:
 *
 *   0x80157948/50/58/60 -> br2eu_psx_alias_body_8015787C  (confirm_code_select
 *                          dialog-code thunk family)
 *   0x80158000/58/70, 0x80158170/88, 0x80158228/30
 *                        -> br2eu_psx_alias_body_80157A00  (memcard_tick
 *                          dialog/state-handler family)
 *
 * Each clean body keeps the generated cpu->pc continuation switch and the
 * entry switch (driven by _sel = the dispatched addr the clean_table CPS
 * wrapper installs). Multi-addr routing lives in game/clean_cps.map. Bodies
 * were produced by the same transform as game/cps_clean.c
 * (game/gen_cps_clean.py): instrumentation stripped, cpu->read/write_*
 * MMIO-aware accessors, br2_lwX/br2_swX for unaligned ops, named GPRs.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern void psx_muldiv_stall(CPUState *cpu);

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

/* confirm_code_entry_cps — clean CPS transliteration of the overlapping-alias
 * body br2eu_psx_alias_body_8015787C (host 0x8015787C),
 * entries are the confirm-code dialog select thunks
 * from generated/SLES_017.22_full_20.c. Registered addrs live
 * in game/clean_cps.map; continuation PCs resume via cpu->pc. */
void confirm_code_entry_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  uint32_t _sel = cpu->pc;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801579F0u: goto block_801579F0;
            case 0x80157964u: goto block_80157964;
            case 0x801579E0u: goto block_801579E0;
            case 0x801579E4u: goto block_801579E4;
            case 0x801579E8u: goto block_801579E8;
            default: break;
        }
    }
    switch (_sel) {
        case 0x80157910u: goto block_80157910;
        case 0x80157918u: goto block_80157918;
        case 0x80157920u: goto block_80157920;
        case 0x80157928u: goto block_80157928;
        case 0x80157930u: goto block_80157930;
        case 0x80157938u: goto block_80157938;
        case 0x80157940u: goto block_80157940;
        case 0x80157948u: goto block_80157948;
        case 0x80157950u: goto block_80157950;
        case 0x80157958u: goto block_80157958;
        case 0x80157960u: goto block_80157960;
        case 0x8015799Cu: goto block_8015799C;
        case 0x801579A4u: goto block_801579A4;
        case 0x801579ACu: goto block_801579AC;
        case 0x801579B4u: goto block_801579B4;
        case 0x801579BCu: goto block_801579BC;
        case 0x801579C4u: goto block_801579C4;
        case 0x801579CCu: goto block_801579CC;
        case 0x801579D4u: goto block_801579D4;
        case 0x801579DCu: goto block_801579DC;
        default: return;
    }

block_80157910:
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157914: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157918:
    /* delay slot (always executes) */
    a1 = 1;  /* 0x8015791C: 0x24050001 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157920:
    /* delay slot (always executes) */
    a1 = 2;  /* 0x80157924: 0x24050002 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157928:
    /* delay slot (always executes) */
    a1 = 5;  /* 0x8015792C: 0x24050005 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157930:
    /* delay slot (always executes) */
    a1 = 3;  /* 0x80157934: 0x24050003 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157938:
    /* delay slot (always executes) */
    a1 = 6;  /* 0x8015793C: 0x24050006 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157940:
    /* delay slot (always executes) */
    a1 = 4;  /* 0x80157944: 0x24050004 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157948:
    /* delay slot (always executes) */
    a1 = 7;  /* 0x8015794C: 0x24050007 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157950:
    /* delay slot (always executes) */
    a1 = 8;  /* 0x80157954: 0x24050008 */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157958:
    /* delay slot (always executes) */
    a1 = 10;  /* 0x8015795C: 0x2405000A */
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157960:
    a1 = 9;  /* 0x80157960: 0x24050009 */
    /* fall through to block_80157964 */

block_80157964:
    /* delay slot (always executes) */
    a1 += 20;  /* 0x80157968: 0x24A50014 */
    psx_check_interrupts_at(cpu, 0x801579E4u);
    goto block_801579E4;  /* j */

block_8015799C:
    /* delay slot (always executes) */
    a1 = 4;  /* 0x801579A0: 0x24050004 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579A4:
    /* delay slot (always executes) */
    a1 = 5;  /* 0x801579A8: 0x24050005 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579AC:
    /* delay slot (always executes) */
    a1 = 6;  /* 0x801579B0: 0x24050006 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579B4:
    /* delay slot (always executes) */
    a1 = 7;  /* 0x801579B8: 0x24050007 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579BC:
    /* delay slot (always executes) */
    a1 = 8;  /* 0x801579C0: 0x24050008 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579C4:
    /* delay slot (always executes) */
    a1 = zero;  /* 0x801579C8: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579CC:
    /* delay slot (always executes) */
    a1 = 1;  /* 0x801579D0: 0x24050001 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579D4:
    /* delay slot (always executes) */
    a1 = 2;  /* 0x801579D8: 0x24050002 */
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579DC:
    a1 = 3;  /* 0x801579DC: 0x24050003 */
    /* fall through to block_801579E0 */

block_801579E0:
    a1 += 10;  /* 0x801579E0: 0x24A5000A */
    /* fall through to block_801579E4 */

block_801579E4:
    a0 = a1;  /* 0x801579E4: 0x00A02021 */
    /* fall through to block_801579E8 */

block_801579E8:
    ra = 0x801579F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;  /* 0x801579EC: 0x24050001 */
    psx_check_interrupts_at(cpu, 0x801577BCu);
    cpu->pc = 0x801577BCu; return;  /* CPS jal -> mc_confirm_dispatch (0x801577BC) */

block_801579F0:
    ra = cpu->read_word(sp + 16);  /* 0x801579F0: 0x8FBF0010 */
    /* nop */  /* 0x801579F4: 0x00000000 */
    uint32_t jt_801579F8 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 24;  /* 0x801579FC: 0x27BD0018 */
    psx_check_interrupts_at(cpu, jt_801579F8);
    cpu->pc = jt_801579F8; return;  /* CPS: jr $ra */}

/* memcard_dialog_cps — clean CPS transliteration of the overlapping-alias
 * body br2eu_psx_alias_body_80157A00 (host 0x80157A00),
 * entries are memcard dialog/state handlers
 * from generated/SLES_017.22_full_20.c. Registered addrs live
 * in game/clean_cps.map; continuation PCs resume via cpu->pc. */
void memcard_dialog_cps(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  uint32_t _sel = cpu->pc;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80157AD8u: goto block_80157AD8;
            case 0x80157AECu: goto block_80157AEC;
            case 0x80157B2Cu: goto block_80157B2C;
            case 0x80157B48u: goto block_80157B48;
            case 0x80157B54u: goto block_80157B54;
            case 0x80157B80u: goto block_80157B80;
            case 0x80157B98u: goto block_80157B98;
            case 0x80157BA4u: goto block_80157BA4;
            case 0x80157BD0u: goto block_80157BD0;
            case 0x80157C0Cu: goto block_80157C0C;
            case 0x80157C24u: goto block_80157C24;
            case 0x80157C64u: goto block_80157C64;
            case 0x80157CACu: goto block_80157CAC;
            case 0x80157CE4u: goto block_80157CE4;
            case 0x80157D1Cu: goto block_80157D1C;
            case 0x80157D34u: goto block_80157D34;
            case 0x80157D90u: goto block_80157D90;
            case 0x80157DA8u: goto block_80157DA8;
            case 0x80157DD4u: goto block_80157DD4;
            case 0x80157DECu: goto block_80157DEC;
            case 0x80157E04u: goto block_80157E04;
            case 0x80157E70u: goto block_80157E70;
            case 0x80157ECCu: goto block_80157ECC;
            case 0x80157EE0u: goto block_80157EE0;
            case 0x80157F24u: goto block_80157F24;
            case 0x80157F40u: goto block_80157F40;
            case 0x80157F4Cu: goto block_80157F4C;
            case 0x80157F78u: goto block_80157F78;
            case 0x80157F90u: goto block_80157F90;
            case 0x80157F9Cu: goto block_80157F9C;
            case 0x80157FC8u: goto block_80157FC8;
            case 0x80157FE0u: goto block_80157FE0;
            case 0x8015800Cu: goto block_8015800C;
            case 0x80158064u: goto block_80158064;
            case 0x80158080u: goto block_80158080;
            case 0x80158098u: goto block_80158098;
            case 0x801580FCu: goto block_801580FC;
            case 0x80158108u: goto block_80158108;
            case 0x80158144u: goto block_80158144;
            case 0x8015817Cu: goto block_8015817C;
            case 0x80158194u: goto block_80158194;
            case 0x801581F0u: goto block_801581F0;
            case 0x80158208u: goto block_80158208;
            case 0x80157B00u: goto block_80157B00;
            case 0x80157B10u: goto block_80157B10;
            case 0x80157B3Cu: goto block_80157B3C;
            case 0x80157B60u: goto block_80157B60;
            case 0x80157B6Cu: goto block_80157B6C;
            case 0x80157B90u: goto block_80157B90;
            case 0x80157BB0u: goto block_80157BB0;
            case 0x80157BBCu: goto block_80157BBC;
            case 0x80157BE0u: goto block_80157BE0;
            case 0x80157BECu: goto block_80157BEC;
            case 0x80157BF8u: goto block_80157BF8;
            case 0x80157C34u: goto block_80157C34;
            case 0x80157C48u: goto block_80157C48;
            case 0x80157C6Cu: goto block_80157C6C;
            case 0x80157C74u: goto block_80157C74;
            case 0x80157C88u: goto block_80157C88;
            case 0x80157C90u: goto block_80157C90;
            case 0x80157C98u: goto block_80157C98;
            case 0x80157CBCu: goto block_80157CBC;
            case 0x80157CC8u: goto block_80157CC8;
            case 0x80157CD4u: goto block_80157CD4;
            case 0x80157CFCu: goto block_80157CFC;
            case 0x80157D04u: goto block_80157D04;
            case 0x80157D44u: goto block_80157D44;
            case 0x80157D58u: goto block_80157D58;
            case 0x80157D64u: goto block_80157D64;
            case 0x80157D70u: goto block_80157D70;
            case 0x80157D78u: goto block_80157D78;
            case 0x80157DB8u: goto block_80157DB8;
            case 0x80157DC0u: goto block_80157DC0;
            case 0x80157DFCu: goto block_80157DFC;
            case 0x80157E10u: goto block_80157E10;
            case 0x80157E2Cu: goto block_80157E2C;
            case 0x80157E3Cu: goto block_80157E3C;
            case 0x80157E4Cu: goto block_80157E4C;
            case 0x80157E58u: goto block_80157E58;
            case 0x80157E78u: goto block_80157E78;
            case 0x80157E84u: goto block_80157E84;
            case 0x80157E8Cu: goto block_80157E8C;
            case 0x80157EF4u: goto block_80157EF4;
            case 0x80157F04u: goto block_80157F04;
            case 0x80157F34u: goto block_80157F34;
            case 0x80157F58u: goto block_80157F58;
            case 0x80157F64u: goto block_80157F64;
            case 0x80157F88u: goto block_80157F88;
            case 0x80157FA8u: goto block_80157FA8;
            case 0x80157FB4u: goto block_80157FB4;
            case 0x80157FD8u: goto block_80157FD8;
            case 0x80157FECu: goto block_80157FEC;
            case 0x80157FF8u: goto block_80157FF8;
            case 0x8015801Cu: goto block_8015801C;
            case 0x80158030u: goto block_80158030;
            case 0x8015803Cu: goto block_8015803C;
            case 0x80158048u: goto block_80158048;
            case 0x80158050u: goto block_80158050;
            case 0x80158090u: goto block_80158090;
            case 0x801580ACu: goto block_801580AC;
            case 0x801580C4u: goto block_801580C4;
            case 0x801580D8u: goto block_801580D8;
            case 0x801580F4u: goto block_801580F4;
            case 0x80158120u: goto block_80158120;
            case 0x8015812Cu: goto block_8015812C;
            case 0x80158134u: goto block_80158134;
            case 0x8015815Cu: goto block_8015815C;
            case 0x80158164u: goto block_80158164;
            case 0x801581A4u: goto block_801581A4;
            case 0x801581B8u: goto block_801581B8;
            case 0x801581C4u: goto block_801581C4;
            case 0x801581D0u: goto block_801581D0;
            case 0x801581D8u: goto block_801581D8;
            case 0x80158218u: goto block_80158218;
            case 0x80158220u: goto block_80158220;
            case 0x8015822Cu: goto block_8015822C;
            default: break;
        }
    }
    switch (_sel) {
        case 0x80157AD0u: goto block_80157AD0;
        case 0x80157B14u: goto block_80157B14;
        case 0x80157B20u: goto block_80157B20;
        case 0x80157B74u: goto block_80157B74;
        case 0x80157BC4u: goto block_80157BC4;
        case 0x80157C00u: goto block_80157C00;
        case 0x80157CA0u: goto block_80157CA0;
        case 0x80157CDCu: goto block_80157CDC;
        case 0x80157D10u: goto block_80157D10;
        case 0x80157D28u: goto block_80157D28;
        case 0x80157D84u: goto block_80157D84;
        case 0x80157D9Cu: goto block_80157D9C;
        case 0x80157DC8u: goto block_80157DC8;
        case 0x80157DE0u: goto block_80157DE0;
        case 0x80157EC4u: goto block_80157EC4;
        case 0x80157F0Cu: goto block_80157F0C;
        case 0x80157F18u: goto block_80157F18;
        case 0x80157F6Cu: goto block_80157F6C;
        case 0x80157FBCu: goto block_80157FBC;
        case 0x80158000u: goto block_80158000;
        case 0x80158058u: goto block_80158058;
        case 0x80158070u: goto block_80158070;
        case 0x8015813Cu: goto block_8015813C;
        case 0x80158170u: goto block_80158170;
        case 0x80158188u: goto block_80158188;
        case 0x801581E4u: goto block_801581E4;
        case 0x801581FCu: goto block_801581FC;
        case 0x80158228u: goto block_80158228;
        case 0x80158230u: goto block_80158230;
        default: return;
    }

block_80157AD0:
    ra = 0x80157AD8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;  /* 0x80157AD4: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> brightness_packet_send (0x80156114) */

block_80157AD8:
    a0 = 14;  /* 0x80157AD8: 0x2404000E */
    a1 = s0;  /* 0x80157ADC: 0x02002821 */
    v0 = 7;  /* 0x80157AE0: 0x24020007 */
    ra = 0x80157AECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0, (uint8_t)v0);  /* 0x80157AE8: 0xA2020000 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157AEC:
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80157AEC: 0x3C03801C */
    v0 = cpu->read_byte(v1 + 17792);  /* 0x80157AF0: 0x90624580 */
    /* nop */  /* 0x80157AF4: 0x00000000 */
    int cond_80157AF8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157AFC: 0x00000000 */
    if (cond_80157AF8) {
        psx_check_interrupts_at(cpu, 0x80157B10u);
        goto block_80157B10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B00u);
        goto block_80157B00;  /* not taken */
    }

block_80157B00:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157B00: 0x9202000D */
    /* nop */  /* 0x80157B04: 0x00000000 */
    v0 += 3;  /* 0x80157B08: 0x24420003 */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157B0C: 0xA202000D */
    /* fall through to block_80157B10 */

block_80157B10:
    cpu->write_byte(v1 + 17792, (uint8_t)zero);  /* 0x80157B10: 0xA0604580 */
    /* fall through to block_80157B14 */

block_80157B14:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157B14: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157B1C: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157B20:
    a0 = 1;  /* 0x80157B20: 0x24040001 */
    ra = 0x80157B2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157B28: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157B2C:
    v1 = v0;  /* 0x80157B2C: 0x00401821 */
    v0 = 2;  /* 0x80157B30: 0x24020002 */
    int cond_80157B34 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157B38: 0x00000000 */
    if (cond_80157B34) {
        psx_check_interrupts_at(cpu, 0x80157B60u);
        goto block_80157B60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B3Cu);
        goto block_80157B3C;  /* not taken */
    }

block_80157B3C:
    a0 = cpu->read_word(s0 + 32);  /* 0x80157B3C: 0x8E040020 */
    ra = 0x80157B48u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;  /* 0x80157B44: 0x27A50010 */
    psx_check_interrupts_at(cpu, 0x801961ECu);
    cpu->pc = 0x801961ECu; return;  /* CPS jal -> cd_file_open (0x801961EC) */

block_80157B48:
    a0 = 2;  /* 0x80157B48: 0x24040002 */
    ra = 0x80157B54u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;  /* 0x80157B50: 0x27A50010 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157B54:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157B54: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157B5C: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157B60:
    v0 = 5;  /* 0x80157B60: 0x24020005 */
    int cond_80157B64 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157B68: 0x24020020 */
    if (cond_80157B64) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B6Cu);
        goto block_80157B6C;  /* not taken */
    }

block_80157B6C:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157B70: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157B74:
    a0 = 1;  /* 0x80157B74: 0x24040001 */
    ra = 0x80157B80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157B7C: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157B80:
    v1 = v0;  /* 0x80157B80: 0x00401821 */
    v0 = 2;  /* 0x80157B84: 0x24020002 */
    int cond_80157B88 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157B8C: 0x00000000 */
    if (cond_80157B88) {
        psx_check_interrupts_at(cpu, 0x80157BB0u);
        goto block_80157BB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B90u);
        goto block_80157B90;  /* not taken */
    }

block_80157B90:
    ra = 0x80157B98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = -1;  /* 0x80157B94: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> brightness_packet_send (0x80156114) */

block_80157B98:
    a0 = 3;  /* 0x80157B98: 0x24040003 */
    ra = 0x80157BA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157BA0: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157BA4:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157BA4: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157BAC: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157BB0:
    v0 = 5;  /* 0x80157BB0: 0x24020005 */
    int cond_80157BB4 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157BB8: 0x24020020 */
    if (cond_80157BB4) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157BBCu);
        goto block_80157BBC;  /* not taken */
    }

block_80157BBC:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157BC0: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157BC4:
    a0 = 1;  /* 0x80157BC4: 0x24040001 */
    ra = 0x80157BD0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157BCC: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157BD0:
    v1 = v0;  /* 0x80157BD0: 0x00401821 */
    v0 = 2;  /* 0x80157BD4: 0x24020002 */
    int cond_80157BD8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157BDC: 0x00000000 */
    if (cond_80157BD8) {
        psx_check_interrupts_at(cpu, 0x80157BECu);
        goto block_80157BEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157BE0u);
        goto block_80157BE0;  /* not taken */
    }

block_80157BE0:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157BE0: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157BE8: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157BEC:
    v0 = 5;  /* 0x80157BEC: 0x24020005 */
    int cond_80157BF0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157BF4: 0x24020020 */
    if (cond_80157BF0) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157BF8u);
        goto block_80157BF8;  /* not taken */
    }

block_80157BF8:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157BFC: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157C00:
    a0 = 1;  /* 0x80157C00: 0x24040001 */
    ra = 0x80157C0Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157C08: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157C0C:
    a0 = 1;  /* 0x80157C0C: 0x24040001 */
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157C10: 0x9202000D */
    a1 = s0 + 4;  /* 0x80157C14: 0x26050004 */
    v0 += a0;  /* 0x80157C18: 0x00441021 */
    ra = 0x80157C24u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157C20: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80195D44u);
    cpu->pc = 0x80195D44u; return;  /* CPS jal -> cd_result_wait_b_fwd (0x80195D44) */

block_80157C24:
    v1 = v0;  /* 0x80157C24: 0x00401821 */
    v0 = 1;  /* 0x80157C28: 0x24020001 */
    int cond_80157C2C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;  /* 0x80157C30: 0x24020004 */
    if (cond_80157C2C) {
        psx_check_interrupts_at(cpu, 0x80157C6Cu);
        goto block_80157C6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C34u);
        goto block_80157C34;  /* not taken */
    }

block_80157C34:
    v0 = cpu->read_byte(s0 + 8);  /* 0x80157C34: 0x92020008 */
    /* nop */  /* 0x80157C38: 0x00000000 */
    v0 &= 0x0080;  /* 0x80157C3C: 0x30420080 */
    int cond_80157C40 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 16;  /* 0x80157C44: 0x27A40010 */
    if (cond_80157C40) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C48u);
        goto block_80157C48;  /* not taken */
    }

block_80157C48:
    v0 = cpu->read_byte(s0 + 7);  /* 0x80157C48: 0x92020007 */
    v1 = cpu->read_byte(s0 + 8);  /* 0x80157C4C: 0x92030008 */
    a1 = cpu->read_byte(s0 + 9);  /* 0x80157C50: 0x92050009 */
    cpu->write_byte(sp + 16, (uint8_t)v0);  /* 0x80157C54: 0xA3A20010 */
    cpu->write_byte(sp + 17, (uint8_t)v1);  /* 0x80157C58: 0xA3A30011 */
    ra = 0x80157C64u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(sp + 18, (uint8_t)a1);  /* 0x80157C60: 0xA3A50012 */
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return;  /* CPS jal -> cd_stamp_to_sector (0x801962F0) */

block_80157C64:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, v0);  /* 0x80157C68: 0xAE020020 */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157C6C:;
    int cond_80157C6C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x80157C70: 0x24020005 */
    if (cond_80157C6C) {
        psx_check_interrupts_at(cpu, 0x80157C90u);
        goto block_80157C90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C74u);
        goto block_80157C74;  /* not taken */
    }

block_80157C74:
    v0 = cpu->read_byte(s0 + 14);  /* 0x80157C74: 0x9202000E */
    /* nop */  /* 0x80157C78: 0x00000000 */
    v0 &= 0x0010;  /* 0x80157C7C: 0x30420010 */
    int cond_80157C80 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16;  /* 0x80157C84: 0x24020010 */
    if (cond_80157C80) {
        psx_check_interrupts_at(cpu, 0x801580D8u);
        goto block_801580D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C88u);
        goto block_80157C88;  /* not taken */
    }

block_80157C88:
    /* delay slot (always executes) */
    /* nop */  /* 0x80157C8C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801580F4u);
    goto block_801580F4;  /* j */

block_80157C90:;
    int cond_80157C90 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157C94: 0x24020020 */
    if (cond_80157C90) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C98u);
        goto block_80157C98;  /* not taken */
    }

block_80157C98:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157C9C: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157CA0:
    a0 = 1;  /* 0x80157CA0: 0x24040001 */
    ra = 0x80157CACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157CA8: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157CAC:
    v1 = v0;  /* 0x80157CAC: 0x00401821 */
    v0 = 2;  /* 0x80157CB0: 0x24020002 */
    int cond_80157CB4 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157CB8: 0x00000000 */
    if (cond_80157CB4) {
        psx_check_interrupts_at(cpu, 0x80157CC8u);
        goto block_80157CC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157CBCu);
        goto block_80157CBC;  /* not taken */
    }

block_80157CBC:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157CBC: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x80157CC4: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157CC8:
    v0 = 5;  /* 0x80157CC8: 0x24020005 */
    int cond_80157CCC = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157CD0: 0x24020020 */
    if (cond_80157CCC) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157CD4u);
        goto block_80157CD4;  /* not taken */
    }

block_80157CD4:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157CD8: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157CDC:
    ra = 0x80157CE4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;  /* 0x80157CE0: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> brightness_packet_send (0x80156114) */

block_80157CE4:
    v0 = cpu->read_half(s0 + 20);  /* 0x80157CE4: 0x96020014 */
    v1 = cpu->read_byte(s0 + 4);  /* 0x80157CE8: 0x92030004 */
    v0++;  /* 0x80157CEC: 0x24420001 */
    v1 &= 0x0010;  /* 0x80157CF0: 0x30630010 */
    int cond_80157CF4 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 20, (uint16_t)v0);  /* 0x80157CF8: 0xA6020014 */
    if (cond_80157CF4) {
        psx_check_interrupts_at(cpu, 0x80157D04u);
        goto block_80157D04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157CFCu);
        goto block_80157CFC;  /* not taken */
    }

block_80157CFC:
    /* delay slot (always executes) */
    v0 = 35;  /* 0x80157D00: 0x24020023 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D04:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157D04: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157D0C: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D10:
    a0 = 1;  /* 0x80157D10: 0x24040001 */
    ra = 0x80157D1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157D18: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157D1C:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157D1C: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157D24: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D28:
    a0 = 1;  /* 0x80157D28: 0x24040001 */
    ra = 0x80157D34u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157D30: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157D34:
    v1 = v0;  /* 0x80157D34: 0x00401821 */
    v0 = 2;  /* 0x80157D38: 0x24020002 */
    int cond_80157D3C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x80157D40: 0x24020005 */
    if (cond_80157D3C) {
        psx_check_interrupts_at(cpu, 0x80157D70u);
        goto block_80157D70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157D44u);
        goto block_80157D44;  /* not taken */
    }

block_80157D44:
    v0 = cpu->read_byte(s0 + 4);  /* 0x80157D44: 0x92020004 */
    /* nop */  /* 0x80157D48: 0x00000000 */
    v0 &= 0x0010;  /* 0x80157D4C: 0x30420010 */
    int cond_80157D50 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157D54: 0x00000000 */
    if (cond_80157D50) {
        psx_check_interrupts_at(cpu, 0x80157D64u);
        goto block_80157D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157D58u);
        goto block_80157D58;  /* not taken */
    }

block_80157D58:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157D58: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157D60: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D64:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157D64: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x80157D6C: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D70:;
    int cond_80157D70 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157D74: 0x00000000 */
    if (cond_80157D70) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157D78u);
        goto block_80157D78;  /* not taken */
    }

block_80157D78:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157D78: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x80157D80: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D84:
    a0 = 19;  /* 0x80157D84: 0x24040013 */
    ra = 0x80157D90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157D8C: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157D90:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157D90: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157D98: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D9C:
    a0 = 1;  /* 0x80157D9C: 0x24040001 */
    ra = 0x80157DA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157DA4: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157DA8:
    v1 = v0;  /* 0x80157DA8: 0x00401821 */
    v0 = 2;  /* 0x80157DAC: 0x24020002 */
    int cond_80157DB0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x80157DB4: 0x24020005 */
    if (cond_80157DB0) {
        psx_check_interrupts_at(cpu, 0x80158228u);
        goto block_80158228;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157DB8u);
        goto block_80157DB8;  /* not taken */
    }

block_80157DB8:;
    int cond_80157DB8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157DBC: 0x24020020 */
    if (cond_80157DB8) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157DC0u);
        goto block_80157DC0;  /* not taken */
    }

block_80157DC0:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157DC4: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157DC8:
    a0 = 1;  /* 0x80157DC8: 0x24040001 */
    ra = 0x80157DD4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157DD0: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157DD4:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157DD4: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157DDC: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157DE0:
    a0 = 1;  /* 0x80157DE0: 0x24040001 */
    ra = 0x80157DECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157DE8: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157DEC:
    v1 = v0;  /* 0x80157DEC: 0x00401821 */
    v0 = 2;  /* 0x80157DF0: 0x24020002 */
    int cond_80157DF4 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x80157DF8: 0x24020005 */
    if (cond_80157DF4) {
        psx_check_interrupts_at(cpu, 0x80157E84u);
        goto block_80157E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157DFCu);
        goto block_80157DFC;  /* not taken */
    }

block_80157DFC:
    ra = 0x80157E04u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0 + 44;  /* 0x80157E00: 0x2604002C */
    psx_check_interrupts_at(cpu, 0x80195830u);
    cpu->pc = 0x80195830u; return;  /* CPS jal -> cd_dir_read_wrap (0x80195830) */

block_80157E04:
    v1 = v0;  /* 0x80157E04: 0x00401821 */
    int cond_80157E08 = ((int32_t)v1 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157E0C: 0x00000000 */
    if (cond_80157E08) {
        psx_check_interrupts_at(cpu, 0x80157E78u);
        goto block_80157E78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E10u);
        goto block_80157E10;  /* not taken */
    }

block_80157E10:
    cpu->write_byte(s0 + 12, (uint8_t)v1);  /* 0x80157E10: 0xA203000C */
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 18);  /* 0x80157E14: 0x86030012 */
    a0 = cpu->read_byte(s0 + 12);  /* 0x80157E18: 0x9204000C */
    /* nop */  /* 0x80157E1C: 0x00000000 */
    v0 = (int32_t)a0 < (int32_t)v1;  /* 0x80157E20: 0x0083102A */
    int cond_80157E24 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157E28: 0x00000000 */
    if (cond_80157E24) {
        psx_check_interrupts_at(cpu, 0x80157E3Cu);
        goto block_80157E3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E2Cu);
        goto block_80157E2C;  /* not taken */
    }

block_80157E2C:
    if (a0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)a0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)a0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);  /* 0x80157E2C: 0x0064001A */
    v0 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x80157E30: 0x00001010 */
    /* nop */  /* 0x80157E34: 0x00000000 */
    cpu->write_half(s0 + 18, (uint16_t)v0);  /* 0x80157E38: 0xA6020012 */
    /* fall through to block_80157E3C */

block_80157E3C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 18);  /* 0x80157E3C: 0x86020012 */
    /* nop */  /* 0x80157E40: 0x00000000 */
    int cond_80157E44 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157E48: 0x00000000 */
    if (cond_80157E44) {
        psx_check_interrupts_at(cpu, 0x80157E58u);
        goto block_80157E58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E4Cu);
        goto block_80157E4C;  /* not taken */
    }

block_80157E4C:
    v0 = cpu->read_byte(s0 + 12);  /* 0x80157E4C: 0x9202000C */
    /* nop */  /* 0x80157E50: 0x00000000 */
    cpu->write_half(s0 + 18, (uint16_t)v0);  /* 0x80157E54: 0xA6020012 */
    /* fall through to block_80157E58 */

block_80157E58:
    a0 = (int32_t)(int16_t)cpu->read_half(s0 + 18);  /* 0x80157E58: 0x86040012 */
    /* nop */  /* 0x80157E5C: 0x00000000 */
    a0 <<= 2;  /* 0x80157E60: 0x00042080 */
    a0 += 44;  /* 0x80157E64: 0x2484002C */
    ra = 0x80157E70u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0 + a0;  /* 0x80157E6C: 0x02042021 */
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return;  /* CPS jal -> cd_stamp_to_sector (0x801962F0) */

block_80157E70:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 36, v0);  /* 0x80157E74: 0xAE020024 */
    psx_check_interrupts_at(cpu, 0x80158228u);
    goto block_80158228;  /* j */

block_80157E78:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157E78: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x80157E80: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157E84:;
    int cond_80157E84 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157E88: 0x24020020 */
    if (cond_80157E84) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E8Cu);
        goto block_80157E8C;  /* not taken */
    }

block_80157E8C:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157E90: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157EC4:
    ra = 0x80157ECCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;  /* 0x80157EC8: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> brightness_packet_send (0x80156114) */

block_80157ECC:
    a0 = 14;  /* 0x80157ECC: 0x2404000E */
    a1 = s0;  /* 0x80157ED0: 0x02002821 */
    v0 = 202;  /* 0x80157ED4: 0x240200CA */
    ra = 0x80157EE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0, (uint8_t)v0);  /* 0x80157EDC: 0xA2020000 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157EE0:
    v1 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80157EE0: 0x3C03801C */
    v0 = cpu->read_byte(v1 + 17792);  /* 0x80157EE4: 0x90624580 */
    /* nop */  /* 0x80157EE8: 0x00000000 */
    int cond_80157EEC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157EF0: 0x00000000 */
    if (cond_80157EEC) {
        psx_check_interrupts_at(cpu, 0x80157F04u);
        goto block_80157F04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157EF4u);
        goto block_80157EF4;  /* not taken */
    }

block_80157EF4:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157EF4: 0x9202000D */
    /* nop */  /* 0x80157EF8: 0x00000000 */
    v0 += 3;  /* 0x80157EFC: 0x24420003 */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157F00: 0xA202000D */
    /* fall through to block_80157F04 */

block_80157F04:
    v0 = 1;  /* 0x80157F04: 0x24020001 */
    cpu->write_byte(v1 + 17792, (uint8_t)v0);  /* 0x80157F08: 0xA0624580 */
    /* fall through to block_80157F0C */

block_80157F0C:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157F0C: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157F14: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157F18:
    a0 = 1;  /* 0x80157F18: 0x24040001 */
    ra = 0x80157F24u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157F20: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157F24:
    v1 = v0;  /* 0x80157F24: 0x00401821 */
    v0 = 2;  /* 0x80157F28: 0x24020002 */
    int cond_80157F2C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157F30: 0x00000000 */
    if (cond_80157F2C) {
        psx_check_interrupts_at(cpu, 0x80157F58u);
        goto block_80157F58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157F34u);
        goto block_80157F34;  /* not taken */
    }

block_80157F34:
    a0 = cpu->read_word(s0 + 32);  /* 0x80157F34: 0x8E040020 */
    ra = 0x80157F40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;  /* 0x80157F3C: 0x27A50010 */
    psx_check_interrupts_at(cpu, 0x801961ECu);
    cpu->pc = 0x801961ECu; return;  /* CPS jal -> cd_file_open (0x801961EC) */

block_80157F40:
    a0 = 2;  /* 0x80157F40: 0x24040002 */
    ra = 0x80157F4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;  /* 0x80157F48: 0x27A50010 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157F4C:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157F4C: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157F54: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157F58:
    v0 = 5;  /* 0x80157F58: 0x24020005 */
    int cond_80157F5C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157F60: 0x24020020 */
    if (cond_80157F5C) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157F64u);
        goto block_80157F64;  /* not taken */
    }

block_80157F64:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157F68: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157F6C:
    a0 = 1;  /* 0x80157F6C: 0x24040001 */
    ra = 0x80157F78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157F74: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157F78:
    v1 = v0;  /* 0x80157F78: 0x00401821 */
    v0 = 2;  /* 0x80157F7C: 0x24020002 */
    int cond_80157F80 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157F84: 0x00000000 */
    if (cond_80157F80) {
        psx_check_interrupts_at(cpu, 0x80157FA8u);
        goto block_80157FA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157F88u);
        goto block_80157F88;  /* not taken */
    }

block_80157F88:
    ra = 0x80157F90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = -1;  /* 0x80157F8C: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> brightness_packet_send (0x80156114) */

block_80157F90:
    a0 = 27;  /* 0x80157F90: 0x2404001B */
    ra = 0x80157F9Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157F98: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157F9C:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157F9C: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157FA4: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157FA8:
    v0 = 5;  /* 0x80157FA8: 0x24020005 */
    int cond_80157FAC = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157FB0: 0x24020020 */
    if (cond_80157FAC) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157FB4u);
        goto block_80157FB4;  /* not taken */
    }

block_80157FB4:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157FB8: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157FBC:
    a0 = 1;  /* 0x80157FBC: 0x24040001 */
    ra = 0x80157FC8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80157FC4: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80157FC8:
    v1 = v0;  /* 0x80157FC8: 0x00401821 */
    v0 = 2;  /* 0x80157FCC: 0x24020002 */
    int cond_80157FD0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1;  /* 0x80157FD4: 0x24040001 */
    if (cond_80157FD0) {
        psx_check_interrupts_at(cpu, 0x80157FECu);
        goto block_80157FEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157FD8u);
        goto block_80157FD8;  /* not taken */
    }

block_80157FD8:
    ra = 0x80157FE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80157FDC: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80157FE0:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80157FE0: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80157FE8: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157FEC:
    v0 = 5;  /* 0x80157FEC: 0x24020005 */
    int cond_80157FF0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80157FF4: 0x24020020 */
    if (cond_80157FF0) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157FF8u);
        goto block_80157FF8;  /* not taken */
    }

block_80157FF8:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80157FFC: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158000:
    a0 = 1;  /* 0x80158000: 0x24040001 */
    ra = 0x8015800Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80158008: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_8015800C:
    v1 = v0;  /* 0x8015800C: 0x00401821 */
    v0 = 2;  /* 0x80158010: 0x24020002 */
    int cond_80158014 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x80158018: 0x24020005 */
    if (cond_80158014) {
        psx_check_interrupts_at(cpu, 0x80158048u);
        goto block_80158048;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015801Cu);
        goto block_8015801C;  /* not taken */
    }

block_8015801C:
    v0 = cpu->read_byte(s0 + 4);  /* 0x8015801C: 0x92020004 */
    /* nop */  /* 0x80158020: 0x00000000 */
    v0 &= 0x0020;  /* 0x80158024: 0x30420020 */
    int cond_80158028 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015802C: 0x00000000 */
    if (cond_80158028) {
        psx_check_interrupts_at(cpu, 0x8015803Cu);
        goto block_8015803C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158030u);
        goto block_80158030;  /* not taken */
    }

block_80158030:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80158030: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80158038: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_8015803C:
    v0 = cpu->read_byte(s0 + 13);  /* 0x8015803C: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x80158044: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158048:;
    int cond_80158048 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x8015804C: 0x24020020 */
    if (cond_80158048) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158050u);
        goto block_80158050;  /* not taken */
    }

block_80158050:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80158054: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158058:
    a0 = 16;  /* 0x80158058: 0x24040010 */
    ra = 0x80158064u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80158060: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80158064:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80158064: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x8015806C: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158070:
    a0 = 1;  /* 0x80158070: 0x24040001 */
    s1 = s0 + 4;  /* 0x80158074: 0x26110004 */
    ra = 0x80158080u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s1;  /* 0x8015807C: 0x02202821 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80158080:
    v1 = v0;  /* 0x80158080: 0x00401821 */
    v0 = 2;  /* 0x80158084: 0x24020002 */
    int cond_80158088 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x8015808C: 0x24020005 */
    if (cond_80158088) {
        psx_check_interrupts_at(cpu, 0x8015812Cu);
        goto block_8015812C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158090u);
        goto block_80158090;  /* not taken */
    }

block_80158090:
    ra = 0x80158098u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s1;  /* 0x80158094: 0x02202021 */
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return;  /* CPS jal -> cd_stamp_to_sector (0x801962F0) */

block_80158098:
    v1 = cpu->read_word(s0 + 40);  /* 0x80158098: 0x8E030028 */
    a0 = v0;  /* 0x8015809C: 0x00402021 */
    v1 = (int32_t)v1 < (int32_t)a0;  /* 0x801580A0: 0x0064182A */
    int cond_801580A4 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, a0);  /* 0x801580A8: 0xAE040020 */
    if (cond_801580A4) {
        psx_check_interrupts_at(cpu, 0x801580C4u);
        goto block_801580C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801580ACu);
        goto block_801580AC;  /* not taken */
    }

block_801580AC:
    v0 = cpu->read_word(s0 + 36);  /* 0x801580AC: 0x8E020024 */
    /* nop */  /* 0x801580B0: 0x00000000 */
    v0 -= 300;  /* 0x801580B4: 0x2442FED4 */
    v0 = (int32_t)a0 < (int32_t)v0;  /* 0x801580B8: 0x0082102A */
    int cond_801580BC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801580C0: 0x00000000 */
    if (cond_801580BC) {
        psx_check_interrupts_at(cpu, 0x80158120u);
        goto block_80158120;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801580C4u);
        goto block_801580C4;  /* not taken */
    }

block_801580C4:
    v0 = cpu->read_byte(s0 + 14);  /* 0x801580C4: 0x9202000E */
    /* nop */  /* 0x801580C8: 0x00000000 */
    v0 &= 0x0010;  /* 0x801580CC: 0x30420010 */
    int cond_801580D0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16;  /* 0x801580D4: 0x24020010 */
    if (cond_801580D0) {
        psx_check_interrupts_at(cpu, 0x801580F4u);
        goto block_801580F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801580D8u);
        goto block_801580D8;  /* not taken */
    }

block_801580D8:
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x801580D8: 0xA202000D */
    v0 = cpu->read_word(s0 + 24);  /* 0x801580DC: 0x8E020018 */
    v1 = cpu->read_word(s0 + 36);  /* 0x801580E0: 0x8E030024 */
    v0++;  /* 0x801580E4: 0x24420001 */
    cpu->write_word(s0 + 24, v0);  /* 0x801580E8: 0xAE020018 */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, v1);  /* 0x801580F0: 0xAE030020 */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_801580F4:
    ra = 0x801580FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;  /* 0x801580F8: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> brightness_packet_send (0x80156114) */

block_801580FC:
    a0 = 9;  /* 0x801580FC: 0x24040009 */
    ra = 0x80158108u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80158104: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_80158108:
    v1 = cpu->read_word(s0 + 36);  /* 0x80158108: 0x8E030024 */
    v0 = 255;  /* 0x8015810C: 0x240200FF */
    cpu->write_byte(s0 + 13, (uint8_t)zero);  /* 0x80158110: 0xA200000D */
    cpu->write_byte(s0 + 15, (uint8_t)v0);  /* 0x80158114: 0xA202000F */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, v1);  /* 0x8015811C: 0xAE030020 */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158120:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80158120: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x80158128: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_8015812C:;
    int cond_8015812C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x80158130: 0x24020020 */
    if (cond_8015812C) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158134u);
        goto block_80158134;  /* not taken */
    }

block_80158134:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80158138: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_8015813C:
    ra = 0x80158144u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;  /* 0x80158140: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> brightness_packet_send (0x80156114) */

block_80158144:
    v0 = cpu->read_half(s0 + 20);  /* 0x80158144: 0x96020014 */
    v1 = cpu->read_byte(s0 + 4);  /* 0x80158148: 0x92030004 */
    v0++;  /* 0x8015814C: 0x24420001 */
    v1 &= 0x0010;  /* 0x80158150: 0x30630010 */
    int cond_80158154 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 20, (uint16_t)v0);  /* 0x80158158: 0xA6020014 */
    if (cond_80158154) {
        psx_check_interrupts_at(cpu, 0x80158164u);
        goto block_80158164;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015815Cu);
        goto block_8015815C;  /* not taken */
    }

block_8015815C:
    /* delay slot (always executes) */
    v0 = 35;  /* 0x80158160: 0x24020023 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158164:
    v0 = cpu->read_byte(s0 + 13);  /* 0x80158164: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x8015816C: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158170:
    a0 = 1;  /* 0x80158170: 0x24040001 */
    ra = 0x8015817Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x80158178: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_8015817C:
    v0 = cpu->read_byte(s0 + 13);  /* 0x8015817C: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x80158184: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158188:
    a0 = 1;  /* 0x80158188: 0x24040001 */
    ra = 0x80158194u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80158190: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80158194:
    v1 = v0;  /* 0x80158194: 0x00401821 */
    v0 = 2;  /* 0x80158198: 0x24020002 */
    int cond_8015819C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x801581A0: 0x24020005 */
    if (cond_8015819C) {
        psx_check_interrupts_at(cpu, 0x801581D0u);
        goto block_801581D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801581A4u);
        goto block_801581A4;  /* not taken */
    }

block_801581A4:
    v0 = cpu->read_byte(s0 + 4);  /* 0x801581A4: 0x92020004 */
    /* nop */  /* 0x801581A8: 0x00000000 */
    v0 &= 0x0010;  /* 0x801581AC: 0x30420010 */
    int cond_801581B0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801581B4: 0x00000000 */
    if (cond_801581B0) {
        psx_check_interrupts_at(cpu, 0x801581C4u);
        goto block_801581C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801581B8u);
        goto block_801581B8;  /* not taken */
    }

block_801581B8:
    v0 = cpu->read_byte(s0 + 13);  /* 0x801581B8: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x801581C0: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581C4:
    v0 = cpu->read_byte(s0 + 13);  /* 0x801581C4: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x801581CC: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581D0:;
    int cond_801581D0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801581D4: 0x00000000 */
    if (cond_801581D0) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801581D8u);
        goto block_801581D8;  /* not taken */
    }

block_801581D8:
    v0 = cpu->read_byte(s0 + 13);  /* 0x801581D8: 0x9202000D */
    /* delay slot (always executes) */
    v0--;  /* 0x801581E0: 0x2442FFFF */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581E4:
    a0 = 19;  /* 0x801581E4: 0x24040013 */
    ra = 0x801581F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x801581EC: 0x00002821 */
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> cd_cmd_issue_async (0x80195EC8) */

block_801581F0:
    v0 = cpu->read_byte(s0 + 13);  /* 0x801581F0: 0x9202000D */
    /* delay slot (always executes) */
    v0++;  /* 0x801581F8: 0x24420001 */
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581FC:
    a0 = 1;  /* 0x801581FC: 0x24040001 */
    ra = 0x80158208u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;  /* 0x80158204: 0x26050004 */
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> cd_sync_wait_a (0x80195D24) */

block_80158208:
    v1 = v0;  /* 0x80158208: 0x00401821 */
    v0 = 2;  /* 0x8015820C: 0x24020002 */
    int cond_80158210 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x80158214: 0x24020005 */
    if (cond_80158210) {
        psx_check_interrupts_at(cpu, 0x80158228u);
        goto block_80158228;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158218u);
        goto block_80158218;  /* not taken */
    }

block_80158218:;
    int cond_80158218 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;  /* 0x8015821C: 0x24020020 */
    if (cond_80158218) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158220u);
        goto block_80158220;  /* not taken */
    }

block_80158220:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x80158224: 0xA202000D */
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158228:
    v0 = 16;  /* 0x80158228: 0x24020010 */
    /* fall through to block_8015822C */

block_8015822C:
    cpu->write_byte(s0 + 13, (uint8_t)v0);  /* 0x8015822C: 0xA202000D */
    /* fall through to block_80158230 */

block_80158230:
    ra = cpu->read_word(sp + 32);  /* 0x80158230: 0x8FBF0020 */
    s1 = cpu->read_word(sp + 28);  /* 0x80158234: 0x8FB1001C */
    s0 = cpu->read_word(sp + 24);  /* 0x80158238: 0x8FB00018 */
    uint32_t jt_8015823C = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 40;  /* 0x80158240: 0x27BD0028 */
    psx_check_interrupts_at(cpu, jt_8015823C);
    cpu->pc = jt_8015823C; return;  /* CPS: jr $ra */}
