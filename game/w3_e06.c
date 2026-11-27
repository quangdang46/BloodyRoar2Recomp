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
/* confirm_code_entry — dialog-code thunk family (alias body
 * 0x8015787C). Polls mc_confirm_dispatch; CPS piece entries. */
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
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157918:
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157920:
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157928:
    /* delay slot (always executes) */
    a1 = 5;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157930:
    /* delay slot (always executes) */
    a1 = 3;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157938:
    /* delay slot (always executes) */
    a1 = 6;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157940:
    /* delay slot (always executes) */
    a1 = 4;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157948:
    /* delay slot (always executes) */
    a1 = 7;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157950:
    /* delay slot (always executes) */
    a1 = 8;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157958:
    /* delay slot (always executes) */
    a1 = 10;
    psx_check_interrupts_at(cpu, 0x80157964u);
    goto block_80157964;  /* j */

block_80157960:
    a1 = 9;
    /* fall through to block_80157964 */

block_80157964:
    /* delay slot (always executes) */
    a1 = a1 + 20;
    psx_check_interrupts_at(cpu, 0x801579E4u);
    goto block_801579E4;  /* j */

block_8015799C:
    /* delay slot (always executes) */
    a1 = 4;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579A4:
    /* delay slot (always executes) */
    a1 = 5;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579AC:
    /* delay slot (always executes) */
    a1 = 6;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579B4:
    /* delay slot (always executes) */
    a1 = 7;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579BC:
    /* delay slot (always executes) */
    a1 = 8;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579C4:
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579CC:
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579D4:
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x801579E0u);
    goto block_801579E0;  /* j */

block_801579DC:
    a1 = 3;
    /* fall through to block_801579E0 */

block_801579E0:
    a1 = a1 + 10;
    /* fall through to block_801579E4 */

block_801579E4:
    a0 = a1;
    /* fall through to block_801579E8 */

block_801579E8:
    ra = 0x801579F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x801577BCu);
    cpu->pc = 0x801577BCu; return;  /* CPS jal -> 0x801577BC (mc_confirm_dispatch) */

block_801579F0:
    ra = cpu->read_word(sp + 16);
    /* nop */  /* 0x801579F4: 0x00000000 */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 24;
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */}

/* memcard_dialog_cps — clean CPS transliteration of the overlapping-alias
 * body br2eu_psx_alias_body_80157A00 (host 0x80157A00),
 * entries are memcard dialog/state handlers
 * from generated/SLES_017.22_full_20.c. Registered addrs live
 * in game/clean_cps.map; continuation PCs resume via cpu->pc. */
/* memcard_tick dialog/state handler (alias body 0x80157A00).
 * Memory-card file browser: cd_dir_read_wrap -> cd_file_open ->
 * cd_stamp_to_sector reads, cd_sync_wait/result waits,
 * cd_cmd_issue_async writes, brightness_packet_send chrome.
 * CPS piece entries. */
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
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 (brightness_packet_send) */

block_80157AD8:
    a0 = 14;
    a1 = s0;
    v0 = 7;
    ra = 0x80157AECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157AEC:
    v1 = 0x801C << 16;  /* 0x801C0000 */
    v0 = cpu->read_byte(v1 + 17792);
    /* nop */  /* 0x80157AF4: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157AFC: 0x00000000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80157B10u);
        goto block_80157B10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B00u);
        goto block_80157B00;  /* not taken */
    }

block_80157B00:
    v0 = cpu->read_byte(s0 + 13);
    /* nop */  /* 0x80157B04: 0x00000000 */
    v0 = v0 + 3;
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    /* fall through to block_80157B10 */

block_80157B10:
    cpu->write_byte(v1 + 17792, (uint8_t)zero);
    /* fall through to block_80157B14 */

block_80157B14:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157B20:
    a0 = 1;
    ra = 0x80157B2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157B2C:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157B38: 0x00000000 */
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80157B60u);
        goto block_80157B60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B3Cu);
        goto block_80157B3C;  /* not taken */
    }

block_80157B3C:
    a0 = cpu->read_word(s0 + 32);
    ra = 0x80157B48u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;
    psx_check_interrupts_at(cpu, 0x801961ECu);
    cpu->pc = 0x801961ECu; return;  /* CPS jal -> 0x801961EC (cd_file_open) */

block_80157B48:
    a0 = 2;
    ra = 0x80157B54u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157B54:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157B60:
    v0 = 5;
    int cond_v1_ne_v0_2 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_2) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B6Cu);
        goto block_80157B6C;  /* not taken */
    }

block_80157B6C:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157B74:
    a0 = 1;
    ra = 0x80157B80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157B80:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_3 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157B8C: 0x00000000 */
    if (cond_v1_ne_v0_3) {
        psx_check_interrupts_at(cpu, 0x80157BB0u);
        goto block_80157BB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157B90u);
        goto block_80157B90;  /* not taken */
    }

block_80157B90:
    ra = 0x80157B98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = -1;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 (brightness_packet_send) */

block_80157B98:
    a0 = 3;
    ra = 0x80157BA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157BA4:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157BB0:
    v0 = 5;
    int cond_v1_ne_v0_4 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_4) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157BBCu);
        goto block_80157BBC;  /* not taken */
    }

block_80157BBC:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157BC4:
    a0 = 1;
    ra = 0x80157BD0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157BD0:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_5 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157BDC: 0x00000000 */
    if (cond_v1_ne_v0_5) {
        psx_check_interrupts_at(cpu, 0x80157BECu);
        goto block_80157BEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157BE0u);
        goto block_80157BE0;  /* not taken */
    }

block_80157BE0:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157BEC:
    v0 = 5;
    int cond_v1_ne_v0_6 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_6) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157BF8u);
        goto block_80157BF8;  /* not taken */
    }

block_80157BF8:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157C00:
    a0 = 1;
    ra = 0x80157C0Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157C0C:
    a0 = 1;
    v0 = cpu->read_byte(s0 + 13);
    a1 = s0 + 4;
    v0 = v0 + a0;
    ra = 0x80157C24u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80195D44u);
    cpu->pc = 0x80195D44u; return;  /* CPS jal -> 0x80195D44 (cd_result_wait_b_fwd) */

block_80157C24:
    v1 = v0;
    v0 = 1;
    int cond_v1_ne_v0_7 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;
    if (cond_v1_ne_v0_7) {
        psx_check_interrupts_at(cpu, 0x80157C6Cu);
        goto block_80157C6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C34u);
        goto block_80157C34;  /* not taken */
    }

block_80157C34:
    v0 = cpu->read_byte(s0 + 8);
    /* nop */  /* 0x80157C38: 0x00000000 */
    v0 = v0 & 0x0080;  /* 0x80157C3C: 0x30420080 */
    int cond_v0_ne_zero_2 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 16;
    if (cond_v0_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C48u);
        goto block_80157C48;  /* not taken */
    }

block_80157C48:
    v0 = cpu->read_byte(s0 + 7);
    v1 = cpu->read_byte(s0 + 8);
    a1 = cpu->read_byte(s0 + 9);
    cpu->write_byte(sp + 16, (uint8_t)v0);
    cpu->write_byte(sp + 17, (uint8_t)v1);
    ra = 0x80157C64u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(sp + 18, (uint8_t)a1);
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return;  /* CPS jal -> 0x801962F0 (cd_stamp_to_sector) */

block_80157C64:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157C6C:;
    int cond_v1_ne_v0_8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_ne_v0_8) {
        psx_check_interrupts_at(cpu, 0x80157C90u);
        goto block_80157C90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C74u);
        goto block_80157C74;  /* not taken */
    }

block_80157C74:
    v0 = cpu->read_byte(s0 + 14);
    /* nop */  /* 0x80157C78: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80157C7C: 0x30420010 */
    int cond_v0_ne_zero_3 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16;
    if (cond_v0_ne_zero_3) {
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
    int cond_v1_ne_v0_9 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_9) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157C98u);
        goto block_80157C98;  /* not taken */
    }

block_80157C98:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157CA0:
    a0 = 1;
    ra = 0x80157CACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157CAC:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_10 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157CB8: 0x00000000 */
    if (cond_v1_ne_v0_10) {
        psx_check_interrupts_at(cpu, 0x80157CC8u);
        goto block_80157CC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157CBCu);
        goto block_80157CBC;  /* not taken */
    }

block_80157CBC:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157CC8:
    v0 = 5;
    int cond_v1_ne_v0_11 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_11) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157CD4u);
        goto block_80157CD4;  /* not taken */
    }

block_80157CD4:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157CDC:
    ra = 0x80157CE4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 (brightness_packet_send) */

block_80157CE4:
    v0 = cpu->read_half(s0 + 20);
    v1 = cpu->read_byte(s0 + 4);
    v0 = v0 + 1;
    v1 = v1 & 0x0010;  /* 0x80157CF0: 0x30630010 */
    int cond_v1_ne_zero = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 20, (uint16_t)v0);
    if (cond_v1_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80157D04u);
        goto block_80157D04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157CFCu);
        goto block_80157CFC;  /* not taken */
    }

block_80157CFC:
    /* delay slot (always executes) */
    v0 = 35;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D04:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D10:
    a0 = 1;
    ra = 0x80157D1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157D1C:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D28:
    a0 = 1;
    ra = 0x80157D34u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157D34:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_12 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_ne_v0_12) {
        psx_check_interrupts_at(cpu, 0x80157D70u);
        goto block_80157D70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157D44u);
        goto block_80157D44;  /* not taken */
    }

block_80157D44:
    v0 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x80157D48: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x80157D4C: 0x30420010 */
    int cond_v0_ne_zero_4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157D54: 0x00000000 */
    if (cond_v0_ne_zero_4) {
        psx_check_interrupts_at(cpu, 0x80157D64u);
        goto block_80157D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157D58u);
        goto block_80157D58;  /* not taken */
    }

block_80157D58:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D64:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D70:;
    int cond_v1_ne_v0_13 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157D74: 0x00000000 */
    if (cond_v1_ne_v0_13) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157D78u);
        goto block_80157D78;  /* not taken */
    }

block_80157D78:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D84:
    a0 = 19;
    ra = 0x80157D90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157D90:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157D9C:
    a0 = 1;
    ra = 0x80157DA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157DA8:
    v1 = v0;
    v0 = 2;
    int cond_v1_eq_v0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_eq_v0) {
        psx_check_interrupts_at(cpu, 0x80158228u);
        goto block_80158228;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157DB8u);
        goto block_80157DB8;  /* not taken */
    }

block_80157DB8:;
    int cond_v1_ne_v0_14 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_14) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157DC0u);
        goto block_80157DC0;  /* not taken */
    }

block_80157DC0:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157DC8:
    a0 = 1;
    ra = 0x80157DD4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157DD4:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157DE0:
    a0 = 1;
    ra = 0x80157DECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157DEC:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_15 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_ne_v0_15) {
        psx_check_interrupts_at(cpu, 0x80157E84u);
        goto block_80157E84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157DFCu);
        goto block_80157DFC;  /* not taken */
    }

block_80157DFC:
    ra = 0x80157E04u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0 + 44;
    psx_check_interrupts_at(cpu, 0x80195830u);
    cpu->pc = 0x80195830u; return;  /* CPS jal -> 0x80195830 (cd_dir_read_wrap) */

block_80157E04:
    v1 = v0;
    int cond_v1_le_0 = ((int32_t)v1 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157E0C: 0x00000000 */
    if (cond_v1_le_0) {
        psx_check_interrupts_at(cpu, 0x80157E78u);
        goto block_80157E78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E10u);
        goto block_80157E10;  /* not taken */
    }

block_80157E10:
    cpu->write_byte(s0 + 12, (uint8_t)v1);
    v1 = (int32_t)(int16_t)cpu->read_half(s0 + 18);
    a0 = cpu->read_byte(s0 + 12);
    /* nop */  /* 0x80157E1C: 0x00000000 */
    v0 = ((int32_t)a0 < (int32_t)v1) ? 1 : 0;  /* 0x80157E20: 0x0083102A */
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157E28: 0x00000000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80157E3Cu);
        goto block_80157E3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E2Cu);
        goto block_80157E2C;  /* not taken */
    }

block_80157E2C:
    if (a0 != 0) { cpu->lo = (uint32_t)((int32_t)v1 / (int32_t)a0); cpu->hi = (uint32_t)((int32_t)v1 % (int32_t)a0); } else { cpu->lo = ((int32_t)v1 >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)v1; }
    psx_muldiv_set(cpu, 37u);
    v0 = cpu->hi;
    psx_muldiv_stall(cpu);
    /* nop */  /* 0x80157E34: 0x00000000 */
    cpu->write_half(s0 + 18, (uint16_t)v0);
    /* fall through to block_80157E3C */

block_80157E3C:
    v0 = (int32_t)(int16_t)cpu->read_half(s0 + 18);
    /* nop */  /* 0x80157E40: 0x00000000 */
    int cond_v0_ne_zero_5 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157E48: 0x00000000 */
    if (cond_v0_ne_zero_5) {
        psx_check_interrupts_at(cpu, 0x80157E58u);
        goto block_80157E58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E4Cu);
        goto block_80157E4C;  /* not taken */
    }

block_80157E4C:
    v0 = cpu->read_byte(s0 + 12);
    /* nop */  /* 0x80157E50: 0x00000000 */
    cpu->write_half(s0 + 18, (uint16_t)v0);
    /* fall through to block_80157E58 */

block_80157E58:
    a0 = (int32_t)(int16_t)cpu->read_half(s0 + 18);
    /* nop */  /* 0x80157E5C: 0x00000000 */
    a0 = a0 << 2;
    a0 = a0 + 44;
    ra = 0x80157E70u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0 + a0;
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return;  /* CPS jal -> 0x801962F0 (cd_stamp_to_sector) */

block_80157E70:
    /* delay slot (always executes) */
    cpu->write_word(s0 + 36, v0);
    psx_check_interrupts_at(cpu, 0x80158228u);
    goto block_80158228;  /* j */

block_80157E78:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157E84:;
    int cond_v1_ne_v0_16 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_16) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157E8Cu);
        goto block_80157E8C;  /* not taken */
    }

block_80157E8C:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157EC4:
    ra = 0x80157ECCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 (brightness_packet_send) */

block_80157ECC:
    a0 = 14;
    a1 = s0;
    v0 = 202;
    ra = 0x80157EE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157EE0:
    v1 = 0x801C << 16;  /* 0x801C0000 */
    v0 = cpu->read_byte(v1 + 17792);
    /* nop */  /* 0x80157EE8: 0x00000000 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157EF0: 0x00000000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x80157F04u);
        goto block_80157F04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157EF4u);
        goto block_80157EF4;  /* not taken */
    }

block_80157EF4:
    v0 = cpu->read_byte(s0 + 13);
    /* nop */  /* 0x80157EF8: 0x00000000 */
    v0 = v0 + 3;
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    /* fall through to block_80157F04 */

block_80157F04:
    v0 = 1;
    cpu->write_byte(v1 + 17792, (uint8_t)v0);
    /* fall through to block_80157F0C */

block_80157F0C:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157F18:
    a0 = 1;
    ra = 0x80157F24u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157F24:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_17 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157F30: 0x00000000 */
    if (cond_v1_ne_v0_17) {
        psx_check_interrupts_at(cpu, 0x80157F58u);
        goto block_80157F58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157F34u);
        goto block_80157F34;  /* not taken */
    }

block_80157F34:
    a0 = cpu->read_word(s0 + 32);
    ra = 0x80157F40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;
    psx_check_interrupts_at(cpu, 0x801961ECu);
    cpu->pc = 0x801961ECu; return;  /* CPS jal -> 0x801961EC (cd_file_open) */

block_80157F40:
    a0 = 2;
    ra = 0x80157F4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = sp + 16;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157F4C:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157F58:
    v0 = 5;
    int cond_v1_ne_v0_18 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_18) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157F64u);
        goto block_80157F64;  /* not taken */
    }

block_80157F64:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157F6C:
    a0 = 1;
    ra = 0x80157F78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157F78:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_19 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80157F84: 0x00000000 */
    if (cond_v1_ne_v0_19) {
        psx_check_interrupts_at(cpu, 0x80157FA8u);
        goto block_80157FA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157F88u);
        goto block_80157F88;  /* not taken */
    }

block_80157F88:
    ra = 0x80157F90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = -1;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 (brightness_packet_send) */

block_80157F90:
    a0 = 27;
    ra = 0x80157F9Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157F9C:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157FA8:
    v0 = 5;
    int cond_v1_ne_v0_20 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_20) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157FB4u);
        goto block_80157FB4;  /* not taken */
    }

block_80157FB4:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80157FBC:
    a0 = 1;
    ra = 0x80157FC8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80157FC8:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_21 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    if (cond_v1_ne_v0_21) {
        psx_check_interrupts_at(cpu, 0x80157FECu);
        goto block_80157FEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157FD8u);
        goto block_80157FD8;  /* not taken */
    }

block_80157FD8:
    ra = 0x80157FE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80157FE0:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80157FEC:
    v0 = 5;
    int cond_v1_ne_v0_22 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_22) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80157FF8u);
        goto block_80157FF8;  /* not taken */
    }

block_80157FF8:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158000:
    a0 = 1;
    ra = 0x8015800Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_8015800C:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_23 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_ne_v0_23) {
        psx_check_interrupts_at(cpu, 0x80158048u);
        goto block_80158048;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015801Cu);
        goto block_8015801C;  /* not taken */
    }

block_8015801C:
    v0 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x80158020: 0x00000000 */
    v0 = v0 & 0x0020;  /* 0x80158024: 0x30420020 */
    int cond_v0_eq_zero_3 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015802C: 0x00000000 */
    if (cond_v0_eq_zero_3) {
        psx_check_interrupts_at(cpu, 0x8015803Cu);
        goto block_8015803C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158030u);
        goto block_80158030;  /* not taken */
    }

block_80158030:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_8015803C:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158048:;
    int cond_v1_ne_v0_24 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_24) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158050u);
        goto block_80158050;  /* not taken */
    }

block_80158050:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158058:
    a0 = 16;
    ra = 0x80158064u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80158064:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158070:
    a0 = 1;
    s1 = s0 + 4;
    ra = 0x80158080u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80158080:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_25 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_ne_v0_25) {
        psx_check_interrupts_at(cpu, 0x8015812Cu);
        goto block_8015812C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158090u);
        goto block_80158090;  /* not taken */
    }

block_80158090:
    ra = 0x80158098u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s1;
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return;  /* CPS jal -> 0x801962F0 (cd_stamp_to_sector) */

block_80158098:
    v1 = cpu->read_word(s0 + 40);
    a0 = v0;
    v1 = ((int32_t)v1 < (int32_t)a0) ? 1 : 0;  /* 0x801580A0: 0x0064182A */
    int cond_v1_ne_zero_2 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, a0);
    if (cond_v1_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x801580C4u);
        goto block_801580C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801580ACu);
        goto block_801580AC;  /* not taken */
    }

block_801580AC:
    v0 = cpu->read_word(s0 + 36);
    /* nop */  /* 0x801580B0: 0x00000000 */
    v0 = v0 + -300;
    v0 = ((int32_t)a0 < (int32_t)v0) ? 1 : 0;  /* 0x801580B8: 0x0082102A */
    int cond_v0_eq_zero_4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801580C0: 0x00000000 */
    if (cond_v0_eq_zero_4) {
        psx_check_interrupts_at(cpu, 0x80158120u);
        goto block_80158120;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801580C4u);
        goto block_801580C4;  /* not taken */
    }

block_801580C4:
    v0 = cpu->read_byte(s0 + 14);
    /* nop */  /* 0x801580C8: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x801580CC: 0x30420010 */
    int cond_v0_eq_zero_5 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 16;
    if (cond_v0_eq_zero_5) {
        psx_check_interrupts_at(cpu, 0x801580F4u);
        goto block_801580F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801580D8u);
        goto block_801580D8;  /* not taken */
    }

block_801580D8:
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    v0 = cpu->read_word(s0 + 24);
    v1 = cpu->read_word(s0 + 36);
    v0 = v0 + 1;
    cpu->write_word(s0 + 24, v0);
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, v1);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_801580F4:
    ra = 0x801580FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 (brightness_packet_send) */

block_801580FC:
    a0 = 9;
    ra = 0x80158108u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_80158108:
    v1 = cpu->read_word(s0 + 36);
    v0 = 255;
    cpu->write_byte(s0 + 13, (uint8_t)zero);
    cpu->write_byte(s0 + 15, (uint8_t)v0);
    /* delay slot (always executes) */
    cpu->write_word(s0 + 32, v1);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158120:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_8015812C:;
    int cond_v1_ne_v0_26 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_26) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158134u);
        goto block_80158134;  /* not taken */
    }

block_80158134:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_8015813C:
    ra = 0x80158144u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 (brightness_packet_send) */

block_80158144:
    v0 = cpu->read_half(s0 + 20);
    v1 = cpu->read_byte(s0 + 4);
    v0 = v0 + 1;
    v1 = v1 & 0x0010;  /* 0x80158150: 0x30630010 */
    int cond_v1_ne_zero_3 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(s0 + 20, (uint16_t)v0);
    if (cond_v1_ne_zero_3) {
        psx_check_interrupts_at(cpu, 0x80158164u);
        goto block_80158164;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015815Cu);
        goto block_8015815C;  /* not taken */
    }

block_8015815C:
    /* delay slot (always executes) */
    v0 = 35;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158164:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158170:
    a0 = 1;
    ra = 0x8015817Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_8015817C:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_80158188:
    a0 = 1;
    ra = 0x80158194u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80158194:
    v1 = v0;
    v0 = 2;
    int cond_v1_ne_v0_27 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_ne_v0_27) {
        psx_check_interrupts_at(cpu, 0x801581D0u);
        goto block_801581D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801581A4u);
        goto block_801581A4;  /* not taken */
    }

block_801581A4:
    v0 = cpu->read_byte(s0 + 4);
    /* nop */  /* 0x801581A8: 0x00000000 */
    v0 = v0 & 0x0010;  /* 0x801581AC: 0x30420010 */
    int cond_v0_ne_zero_6 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801581B4: 0x00000000 */
    if (cond_v0_ne_zero_6) {
        psx_check_interrupts_at(cpu, 0x801581C4u);
        goto block_801581C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801581B8u);
        goto block_801581B8;  /* not taken */
    }

block_801581B8:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581C4:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581D0:;
    int cond_v1_ne_v0_28 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801581D4: 0x00000000 */
    if (cond_v1_ne_v0_28) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801581D8u);
        goto block_801581D8;  /* not taken */
    }

block_801581D8:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + -1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581E4:
    a0 = 19;
    ra = 0x801581F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = zero;
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 (cd_cmd_issue_async) */

block_801581F0:
    v0 = cpu->read_byte(s0 + 13);
    /* delay slot (always executes) */
    v0 = v0 + 1;
    psx_check_interrupts_at(cpu, 0x8015822Cu);
    goto block_8015822C;  /* j */

block_801581FC:
    a0 = 1;
    ra = 0x80158208u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0 + 4;
    psx_check_interrupts_at(cpu, 0x80195D24u);
    cpu->pc = 0x80195D24u; return;  /* CPS jal -> 0x80195D24 (cd_sync_wait_a) */

block_80158208:
    v1 = v0;
    v0 = 2;
    int cond_v1_eq_v0_2 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;
    if (cond_v1_eq_v0_2) {
        psx_check_interrupts_at(cpu, 0x80158228u);
        goto block_80158228;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158218u);
        goto block_80158218;  /* not taken */
    }

block_80158218:;
    int cond_v1_ne_v0_29 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 32;
    if (cond_v1_ne_v0_29) {
        psx_check_interrupts_at(cpu, 0x80158230u);
        goto block_80158230;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158220u);
        goto block_80158220;  /* not taken */
    }

block_80158220:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80158230u);
    goto block_80158230;  /* j */

block_80158228:
    v0 = 16;
    /* fall through to block_8015822C */

block_8015822C:
    cpu->write_byte(s0 + 13, (uint8_t)v0);
    /* fall through to block_80158230 */

block_80158230:
    ra = cpu->read_word(sp + 32);
    s1 = cpu->read_word(sp + 28);
    s0 = cpu->read_word(sp + 24);
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 40;
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */}
