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

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability), same
 * convention as game/cps_clean.c. */
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


/* cps_8016fcc4_alias — clean CPS transliteration of the generated
 * br2eu_psx_alias_body_8016FCC4 alias-entry piece (guest addrs
 * 0x80170144/0x80170260/0x80170960/0x80170980/0x80170998 share this body).
 * `entry` is the dispatched guest addr (cpu->pc), matching the generated
 * alias body's entry parameter. */
void cps_8016fcc4_alias(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  uint32_t entry = cpu->pc;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8016FD7Cu: goto block_8016FD7C;
            case 0x8016FD84u: goto block_8016FD84;
            case 0x8016FDA0u: goto block_8016FDA0;
            case 0x8016FDA8u: goto block_8016FDA8;
            case 0x8016FDB0u: goto block_8016FDB0;
            case 0x8016FDC0u: goto block_8016FDC0;
            case 0x8016FDD4u: goto block_8016FDD4;
            case 0x8016FDDCu: goto block_8016FDDC;
            case 0x8016FE44u: goto block_8016FE44;
            case 0x8016FE68u: goto block_8016FE68;
            case 0x8016FE9Cu: goto block_8016FE9C;
            case 0x8016FED0u: goto block_8016FED0;
            case 0x8016FEF4u: goto block_8016FEF4;
            case 0x8016FF20u: goto block_8016FF20;
            case 0x8016FF28u: goto block_8016FF28;
            case 0x8016FF44u: goto block_8016FF44;
            case 0x8016FF80u: goto block_8016FF80;
            case 0x8016FF98u: goto block_8016FF98;
            case 0x8016FFA0u: goto block_8016FFA0;
            case 0x8016FFF0u: goto block_8016FFF0;
            case 0x8016FFF8u: goto block_8016FFF8;
            case 0x80170008u: goto block_80170008;
            case 0x80170018u: goto block_80170018;
            case 0x8017002Cu: goto block_8017002C;
            case 0x8017003Cu: goto block_8017003C;
            case 0x80170044u: goto block_80170044;
            case 0x80170058u: goto block_80170058;
            case 0x80170080u: goto block_80170080;
            case 0x801700D0u: goto block_801700D0;
            case 0x801700FCu: goto block_801700FC;
            case 0x80170104u: goto block_80170104;
            case 0x80170110u: goto block_80170110;
            case 0x80170118u: goto block_80170118;
            case 0x80170120u: goto block_80170120;
            case 0x80170130u: goto block_80170130;
            case 0x8017013Cu: goto block_8017013C;
            case 0x80170158u: goto block_80170158;
            case 0x80170168u: goto block_80170168;
            case 0x80170170u: goto block_80170170;
            case 0x80170178u: goto block_80170178;
            case 0x80170180u: goto block_80170180;
            case 0x80170188u: goto block_80170188;
            case 0x80170190u: goto block_80170190;
            case 0x80170198u: goto block_80170198;
            case 0x801701ACu: goto block_801701AC;
            case 0x801701C8u: goto block_801701C8;
            case 0x801701E0u: goto block_801701E0;
            case 0x801701E8u: goto block_801701E8;
            case 0x801701F0u: goto block_801701F0;
            case 0x801701F8u: goto block_801701F8;
            case 0x80170208u: goto block_80170208;
            case 0x80170218u: goto block_80170218;
            case 0x8017022Cu: goto block_8017022C;
            case 0x80170234u: goto block_80170234;
            case 0x8017023Cu: goto block_8017023C;
            case 0x80170244u: goto block_80170244;
            case 0x80170254u: goto block_80170254;
            case 0x8017027Cu: goto block_8017027C;
            case 0x80170290u: goto block_80170290;
            case 0x801702B8u: goto block_801702B8;
            case 0x801702FCu: goto block_801702FC;
            case 0x8017030Cu: goto block_8017030C;
            case 0x80170318u: goto block_80170318;
            case 0x80170324u: goto block_80170324;
            case 0x80170334u: goto block_80170334;
            case 0x80170380u: goto block_80170380;
            case 0x80170394u: goto block_80170394;
            case 0x801703A0u: goto block_801703A0;
            case 0x801703D8u: goto block_801703D8;
            case 0x801703FCu: goto block_801703FC;
            case 0x80170420u: goto block_80170420;
            case 0x80170468u: goto block_80170468;
            case 0x80170494u: goto block_80170494;
            case 0x801704BCu: goto block_801704BC;
            case 0x80170544u: goto block_80170544;
            case 0x8017058Cu: goto block_8017058C;
            case 0x801705C8u: goto block_801705C8;
            case 0x801705ECu: goto block_801705EC;
            case 0x80170610u: goto block_80170610;
            case 0x80170658u: goto block_80170658;
            case 0x80170684u: goto block_80170684;
            case 0x801706ACu: goto block_801706AC;
            case 0x80170734u: goto block_80170734;
            case 0x8017077Cu: goto block_8017077C;
            case 0x801707ACu: goto block_801707AC;
            case 0x80170808u: goto block_80170808;
            case 0x80170830u: goto block_80170830;
            case 0x80170878u: goto block_80170878;
            case 0x8017089Cu: goto block_8017089C;
            case 0x801708C0u: goto block_801708C0;
            case 0x801708D8u: goto block_801708D8;
            case 0x801708F0u: goto block_801708F0;
            case 0x80170900u: goto block_80170900;
            case 0x80170908u: goto block_80170908;
            case 0x80170910u: goto block_80170910;
            case 0x80170918u: goto block_80170918;
            case 0x80170928u: goto block_80170928;
            case 0x80170934u: goto block_80170934;
            case 0x80170944u: goto block_80170944;
            case 0x80170954u: goto block_80170954;
            case 0x80170968u: goto block_80170968;
            case 0x80170988u: goto block_80170988;
            case 0x801709A0u: goto block_801709A0;
            case 0x801709C8u: goto block_801709C8;
            case 0x8016FD38u: goto block_8016FD38;
            case 0x8016FD50u: goto block_8016FD50;
            case 0x8016FD8Cu: goto block_8016FD8C;
            case 0x8016FDF0u: goto block_8016FDF0;
            case 0x8016FE24u: goto block_8016FE24;
            case 0x8016FE3Cu: goto block_8016FE3C;
            case 0x8016FE4Cu: goto block_8016FE4C;
            case 0x8016FE58u: goto block_8016FE58;
            case 0x8016FE60u: goto block_8016FE60;
            case 0x8016FE70u: goto block_8016FE70;
            case 0x8016FE90u: goto block_8016FE90;
            case 0x8016FE94u: goto block_8016FE94;
            case 0x8016FEA4u: goto block_8016FEA4;
            case 0x8016FED8u: goto block_8016FED8;
            case 0x8016FEFCu: goto block_8016FEFC;
            case 0x8016FF64u: goto block_8016FF64;
            case 0x8016FF78u: goto block_8016FF78;
            case 0x8016FF88u: goto block_8016FF88;
            case 0x8016FF90u: goto block_8016FF90;
            case 0x8016FFB4u: goto block_8016FFB4;
            case 0x8016FFC4u: goto block_8016FFC4;
            case 0x8016FFD0u: goto block_8016FFD0;
            case 0x8016FFD8u: goto block_8016FFD8;
            case 0x80170060u: goto block_80170060;
            case 0x80170070u: goto block_80170070;
            case 0x80170088u: goto block_80170088;
            case 0x8017008Cu: goto block_8017008C;
            case 0x801700BCu: goto block_801700BC;
            case 0x801700C0u: goto block_801700C0;
            case 0x801700D8u: goto block_801700D8;
            case 0x8017025Cu: goto block_8017025C;
            case 0x80170270u: goto block_80170270;
            case 0x80170284u: goto block_80170284;
            case 0x801702A8u: goto block_801702A8;
            case 0x801702B0u: goto block_801702B0;
            case 0x801702C0u: goto block_801702C0;
            case 0x801702E0u: goto block_801702E0;
            case 0x801702E4u: goto block_801702E4;
            case 0x801702F4u: goto block_801702F4;
            case 0x8017034Cu: goto block_8017034C;
            case 0x8017035Cu: goto block_8017035C;
            case 0x8017036Cu: goto block_8017036C;
            case 0x80170370u: goto block_80170370;
            case 0x80170384u: goto block_80170384;
            case 0x80170388u: goto block_80170388;
            case 0x801703B4u: goto block_801703B4;
            case 0x801703C8u: goto block_801703C8;
            case 0x801703ECu: goto block_801703EC;
            case 0x801703F4u: goto block_801703F4;
            case 0x80170428u: goto block_80170428;
            case 0x80170448u: goto block_80170448;
            case 0x80170458u: goto block_80170458;
            case 0x80170460u: goto block_80170460;
            case 0x8017048Cu: goto block_8017048C;
            case 0x801704B4u: goto block_801704B4;
            case 0x801704D4u: goto block_801704D4;
            case 0x801704E4u: goto block_801704E4;
            case 0x80170500u: goto block_80170500;
            case 0x80170508u: goto block_80170508;
            case 0x80170510u: goto block_80170510;
            case 0x80170518u: goto block_80170518;
            case 0x80170524u: goto block_80170524;
            case 0x8017054Cu: goto block_8017054C;
            case 0x80170550u: goto block_80170550;
            case 0x801705A4u: goto block_801705A4;
            case 0x801705B8u: goto block_801705B8;
            case 0x801705DCu: goto block_801705DC;
            case 0x801705E4u: goto block_801705E4;
            case 0x80170618u: goto block_80170618;
            case 0x80170638u: goto block_80170638;
            case 0x80170648u: goto block_80170648;
            case 0x80170650u: goto block_80170650;
            case 0x8017067Cu: goto block_8017067C;
            case 0x801706A4u: goto block_801706A4;
            case 0x801706C4u: goto block_801706C4;
            case 0x801706D4u: goto block_801706D4;
            case 0x801706F0u: goto block_801706F0;
            case 0x801706F8u: goto block_801706F8;
            case 0x80170700u: goto block_80170700;
            case 0x80170708u: goto block_80170708;
            case 0x80170714u: goto block_80170714;
            case 0x8017073Cu: goto block_8017073C;
            case 0x80170740u: goto block_80170740;
            case 0x80170794u: goto block_80170794;
            case 0x8017079Cu: goto block_8017079C;
            case 0x801707C0u: goto block_801707C0;
            case 0x801707D4u: goto block_801707D4;
            case 0x801707E0u: goto block_801707E0;
            case 0x801707E4u: goto block_801707E4;
            case 0x80170820u: goto block_80170820;
            case 0x80170840u: goto block_80170840;
            case 0x80170850u: goto block_80170850;
            case 0x80170868u: goto block_80170868;
            case 0x80170888u: goto block_80170888;
            case 0x801708ACu: goto block_801708AC;
            case 0x801708D0u: goto block_801708D0;
            case 0x801708E8u: goto block_801708E8;
            case 0x80170978u: goto block_80170978;
            case 0x80170990u: goto block_80170990;
            case 0x801709B0u: goto block_801709B0;
            case 0x801709C0u: goto block_801709C0;
            default: break;
        }
    }
    switch (entry) {
        case 0x8016FD6Cu: goto block_8016FD6C;
        case 0x8016FD74u: goto block_8016FD74;
        case 0x8016FE10u: goto block_8016FE10;
        case 0x8016FE78u: goto block_8016FE78;
        case 0x8016FEBCu: goto block_8016FEBC;
        case 0x8016FEE0u: goto block_8016FEE0;
        case 0x8016FF04u: goto block_8016FF04;
        case 0x8016FFE0u: goto block_8016FFE0;
        case 0x80170050u: goto block_80170050;
        case 0x801700ACu: goto block_801700AC;
        case 0x801700E8u: goto block_801700E8;
        case 0x80170144u: goto block_80170144;
        case 0x8017024Cu: goto block_8017024C;
        case 0x80170260u: goto block_80170260;
        case 0x801708F8u: goto block_801708F8;
        case 0x80170960u: goto block_80170960;
        case 0x80170980u: goto block_80170980;
        case 0x80170998u: goto block_80170998;
        default: return;
    }

block_8016FD38:
    v1 = cpu->read_byte(s0 + 1);
    s4 = cpu->read_word(s0 + 12);
    s5 = cpu->read_word(s0 + 8);
    v0 = (v1 < (uint32_t)13) ? 1 : 0;  /* 0x8016FD44: 0x2C62000D */
    int _bc_8016FD48 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8016FD48) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FD50u);
        goto block_8016FD50;  /* not taken */
    }

block_8016FD50:
    v0 = v0 + 9680;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x8016FD60: 0x00000000 */
    uint32_t _jt_8016FD64 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD68: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8016FD64);
    cpu->pc = _jt_8016FD64; return;  /* CPS: jr v0 */

block_8016FD6C:
    v0 = 1;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_8016FD74 */

block_8016FD74:
    ra = 0x8016FD7Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD78: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80136DDCu);
    cpu->pc = 0x80136DDCu; return;  /* CPS jal -> 0x80136DDC */

block_8016FD7C:
    ra = 0x8016FD84u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD80: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_8016FD84:
    int _bc_8016FD84 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD88: 0x00000000 */
    if (_bc_8016FD84) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FD8Cu);
        goto block_8016FD8C;  /* not taken */
    }

block_8016FD8C:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x8016FD90: 0x00000000 */
    a0 = cpu->read_byte(v0 + 11);
    ra = 0x8016FDA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FD9C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012708Cu);
    cpu->pc = 0x8012708Cu; return;  /* CPS jal -> 0x8012708C */

block_8016FDA0:
    ra = 0x8016FDA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FDA4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012748Cu);
    cpu->pc = 0x8012748Cu; return;  /* CPS jal -> 0x8012748C */

block_8016FDA8:
    ra = 0x8016FDB0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x801560F8u);
    cpu->pc = 0x801560F8u; return;  /* CPS jal -> 0x801560F8 */

block_8016FDB0:
    a0 = zero;
    a1 = a0;
    ra = 0x8016FDC0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a0;
    psx_check_interrupts_at(cpu, 0x801274FCu);
    cpu->pc = 0x801274FCu; return;  /* CPS jal -> 0x801274FC */

block_8016FDC0:
    a0 = s0;
    cpu->write_byte(s0 + 16, (uint8_t)zero);
    cpu->write_byte(s0 + 158, (uint8_t)zero);
    ra = 0x8016FDD4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x8015113Cu);
    cpu->pc = 0x8015113Cu; return;  /* CPS jal -> 0x8015113C */

block_8016FDD4:
    ra = 0x8016FDDCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8015A1E4u);
    cpu->pc = 0x8015A1E4u; return;  /* CPS jal -> 0x8015A1E4 */

block_8016FDDC:
    v1 = cpu->read_byte(s0 + 156);
    /* nop */  /* 0x8016FDE0: 0x00000000 */
    v0 = (v1 < (uint32_t)6) ? 1 : 0;  /* 0x8016FDE4: 0x2C620006 */
    int _bc_8016FDE8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 36, zero);
    if (_bc_8016FDE8) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FDF0u);
        goto block_8016FDF0;  /* not taken */
    }

block_8016FDF0:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 9736;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x8016FE04: 0x00000000 */
    uint32_t _jt_8016FE08 = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE0C: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8016FE08);
    cpu->pc = _jt_8016FE08; return;  /* CPS: jr v0 */

block_8016FE10:
    v0 = cpu->read_word(s0 + 192);
    v1 = cpu->read_byte(s0 + 155);
    v0 = v0 + 44;
    int _bc_8016FE1C = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    if (_bc_8016FE1C) {
        psx_check_interrupts_at(cpu, 0x8016FE60u);
        goto block_8016FE60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE24u);
        goto block_8016FE24;  /* not taken */
    }

block_8016FE24:
    v0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x8016FE28: 0x00000000 */
    v1 = cpu->read_byte(v0 + 17);
    v0 = 1;
    int _bc_8016FE34 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE38: 0x00000000 */
    if (_bc_8016FE34) {
        psx_check_interrupts_at(cpu, 0x8016FE60u);
        goto block_8016FE60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE3Cu);
        goto block_8016FE3C;  /* not taken */
    }

block_8016FE3C:
    ra = 0x8016FE44u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE40: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014CDD0u);
    cpu->pc = 0x8014CDD0u; return;  /* CPS jal -> 0x8014CDD0 */

block_8016FE44:
    int _bc_8016FE44 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 1;
    if (_bc_8016FE44) {
        psx_check_interrupts_at(cpu, 0x8016FE58u);
        goto block_8016FE58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE4Cu);
        goto block_8016FE4C;  /* not taken */
    }

block_8016FE4C:
    t0 = 255;
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    t0 = 1;
    /* fall through to block_8016FE58 */

block_8016FE58:
    /* delay slot (always executes) */
    cpu->write_word(sp + 36, t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FE60:
    ra = 0x8016FE68u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FE68:
    int _bc_8016FE68 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FE68) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE70u);
        goto block_8016FE70;  /* not taken */
    }

block_8016FE70:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FE78:
    v0 = cpu->read_word(s0 + 192);
    v1 = cpu->read_byte(s0 + 155);
    v0 = v0 + 56;
    cpu->write_word(s0 + 196, v0);
    int _bc_8016FE88 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    if (_bc_8016FE88) {
        psx_check_interrupts_at(cpu, 0x8016FE94u);
        goto block_8016FE94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FE90u);
        goto block_8016FE90;  /* not taken */
    }

block_8016FE90:
    a0 = 3;
    /* fall through to block_8016FE94 */

block_8016FE94:
    ra = 0x8016FE9Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FE98: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FE9C:
    int _bc_8016FE9C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 11;
    if (_bc_8016FE9C) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FEA4u);
        goto block_8016FEA4;  /* not taken */
    }

block_8016FEA4:
    cpu->write_byte(s0, (uint8_t)v0);
    v0 = 4;
    t0 = 255;
    cpu->write_byte(s0 + 2, (uint8_t)v0);
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FEBC:
    v0 = cpu->read_word(s0 + 192);
    a0 = 1;
    v0 = v0 + 44;
    ra = 0x8016FED0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FED0:
    int _bc_8016FED0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FED0) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FED8u);
        goto block_8016FED8;  /* not taken */
    }

block_8016FED8:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FEE0:
    v0 = cpu->read_word(s0 + 192);
    a0 = 3;
    v0 = v0 + 44;
    ra = 0x8016FEF4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FEF4:
    int _bc_8016FEF4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FEF4) {
        psx_check_interrupts_at(cpu, 0x8016FFA0u);
        goto block_8016FFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FEFCu);
        goto block_8016FEFC;  /* not taken */
    }

block_8016FEFC:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FF04:
    a0 = 1027;
    a1 = zero;
    v0 = cpu->read_word(s0 + 192);
    a2 = a1;
    v0 = v0 + 44;
    ra = 0x8016FF20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(s0 + 196, v0);
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8016FF20:
    ra = 0x8016FF28u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_8016FF28:
    v0 = cpu->read_word(s0 + 192);
    a0 = 0x8001 << 16;  /* 0x80010000 */
    a2 = cpu->read_word(v0 + 40);
    a1 = 1;
    a2 = (uint32_t)a2 >> (a1 & 0x1F);
    ra = 0x8016FF44u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a2 & a1;  /* 0x8016FF40: 0x00C53024 */
    psx_check_interrupts_at(cpu, 0x80159028u);
    cpu->pc = 0x80159028u; return;  /* CPS jal -> 0x80159028 */

block_8016FF44:
    a0 = cpu->read_word(s0 + 192);
    /* nop */  /* 0x8016FF48: 0x00000000 */
    v1 = cpu->read_word(a0 + 40);
    /* nop */  /* 0x8016FF50: 0x00000000 */
    v0 = (uint32_t)v1 >> 1;
    v0 = v0 & 0x0001;  /* 0x8016FF58: 0x30420001 */
    int _bc_8016FF5C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 | 0x0002;
    if (_bc_8016FF5C) {
        psx_check_interrupts_at(cpu, 0x8016FF78u);
        goto block_8016FF78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FF64u);
        goto block_8016FF64;  /* not taken */
    }

block_8016FF64:
    cpu->write_word(a0 + 40, v0);
    v0 = 1;
    t0 = 0x801C << 16;  /* 0x801C0000 */
    t0 = t0 + 4576;
    cpu->write_word(t0 + 1040, v0);
    /* fall through to block_8016FF78 */

block_8016FF78:
    ra = 0x8016FF80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 2;
    psx_check_interrupts_at(cpu, 0x8014F260u);
    cpu->pc = 0x8014F260u; return;  /* CPS jal -> 0x8014F260 */

block_8016FF80:
    int _bc_8016FF80 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_8016FF80) {
        psx_check_interrupts_at(cpu, 0x8016FF90u);
        goto block_8016FF90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FF88u);
        goto block_8016FF88;  /* not taken */
    }

block_8016FF88:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x8016FFA0u);
    goto block_8016FFA0;  /* j */

block_8016FF90:
    ra = 0x8016FF98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8016DDF8u);
    cpu->pc = 0x8016DDF8u; return;  /* CPS jal -> 0x8016DDF8 */

block_8016FF98:
    ra = 0x8016FFA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8016E21Cu);
    cpu->pc = 0x8016E21Cu; return;  /* CPS jal -> 0x8016E21C */

block_8016FFA0:
    v0 = cpu->read_byte(s0 + 1);
    /* nop */  /* 0x8016FFA4: 0x00000000 */
    a0 = v0 & 0x00FF;  /* 0x8016FFA8: 0x304400FF */
    int _bc_8016FFAC = (a0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 13;
    if (_bc_8016FFAC) {
        psx_check_interrupts_at(cpu, 0x8016FFD8u);
        goto block_8016FFD8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FFB4u);
        goto block_8016FFB4;  /* not taken */
    }

block_8016FFB4:
    v1 = cpu->read_byte(s0);
    /* nop */  /* 0x8016FFB8: 0x00000000 */
    int _bc_8016FFBC = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_8016FFBC) {
        psx_check_interrupts_at(cpu, 0x8016FFD0u);
        goto block_8016FFD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FFC4u);
        goto block_8016FFC4;  /* not taken */
    }

block_8016FFC4:
    t0 = 255;
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_8016FFD0:
    int _bc_8016FFD0 = (a0 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FFD4: 0x00000000 */
    if (_bc_8016FFD0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016FFD8u);
        goto block_8016FFD8;  /* not taken */
    }

block_8016FFD8:
    v0 = 2;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_8016FFE0 */

block_8016FFE0:
    a0 = 516;
    a1 = zero;
    ra = 0x8016FFF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8016FFF0:
    ra = 0x8016FFF8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016FFF4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80136DDCu);
    cpu->pc = 0x80136DDCu; return;  /* CPS jal -> 0x80136DDC */

block_8016FFF8:
    a0 = zero;
    a1 = a0;
    ra = 0x80170008u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a0;
    psx_check_interrupts_at(cpu, 0x801274FCu);
    cpu->pc = 0x801274FCu; return;  /* CPS jal -> 0x801274FC */

block_80170008:
    a0 = 2;
    a1 = zero;
    ra = 0x80170018u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x8012CBC0u);
    cpu->pc = 0x8012CBC0u; return;  /* CPS jal -> 0x8012CBC0 */

block_80170018:
    a0 = s3;
    a1 = s2;
    a3 = cpu->read_word(s0 + 192);
    ra = 0x8017002Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x8015A69Cu);
    cpu->pc = 0x8015A69Cu; return;  /* CPS jal -> 0x8015A69C */

block_8017002C:
    a0 = s3;
    a1 = s2;
    ra = 0x8017003Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80151328u);
    cpu->pc = 0x80151328u; return;  /* CPS jal -> 0x80151328 */

block_8017003C:
    ra = 0x80170044u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170040: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8016FBFCu);
    cpu->pc = 0x8016FBFCu; return;  /* CPS jal -> 0x8016FBFC */

block_80170044:
    v0 = 3;
    cpu->write_byte(s0 + 158, (uint8_t)zero);
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_80170050 */

block_80170050:
    ra = 0x80170058u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170054: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_80170058:
    int _bc_80170058 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017005C: 0x00000000 */
    if (_bc_80170058) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170060u);
        goto block_80170060;  /* not taken */
    }

block_80170060:
    t0 = cpu->read_word(sp + 36);
    /* nop */  /* 0x80170064: 0x00000000 */
    int _bc_80170068 = (t0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 3;
    if (_bc_80170068) {
        psx_check_interrupts_at(cpu, 0x80170088u);
        goto block_80170088;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170070u);
        goto block_80170070;  /* not taken */
    }

block_80170070:
    a0 = s0;
    a1 = s3;
    ra = 0x80170080u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s2;
    psx_check_interrupts_at(cpu, 0x801506D0u);
    cpu->pc = 0x801506D0u; return;  /* CPS jal -> 0x801506D0 */

block_80170080:
    /* delay slot (always executes) */
    v0 = 3;
    psx_check_interrupts_at(cpu, 0x8017008Cu);
    goto block_8017008C;  /* j */

block_80170088:
    cpu->write_word(sp + 36, zero);
    /* fall through to block_8017008C */

block_8017008C:
    cpu->write_byte(s0 + 158, (uint8_t)v0);
    v0 = 4;
    cpu->write_half(s0 + 136, (uint16_t)zero);
    cpu->write_half(s0 + 140, (uint16_t)zero);
    cpu->write_half(s0 + 142, (uint16_t)zero);
    cpu->write_half(s0 + 144, (uint16_t)zero);
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_801700AC:
    v0 = cpu->read_half(s0 + 144);
    /* nop */  /* 0x801700B0: 0x00000000 */
    int _bc_801700B4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    if (_bc_801700B4) {
        psx_check_interrupts_at(cpu, 0x801700C0u);
        goto block_801700C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801700BCu);
        goto block_801700BC;  /* not taken */
    }

block_801700BC:
    cpu->write_half(s0 + 144, (uint16_t)zero);
    /* fall through to block_801700C0 */

block_801700C0:
    a1 = s3;
    a2 = s2;
    ra = 0x801700D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = zero;
    psx_check_interrupts_at(cpu, 0x801709FCu);
    cpu->pc = 0x801709FCu; return;  /* CPS jal -> 0x801709FC */

block_801700D0:
    int _bc_801700D0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801700D4: 0x00000000 */
    if (_bc_801700D0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801700D8u);
        goto block_801700D8;  /* not taken */
    }

block_801700D8:
    v0 = cpu->read_half(s0 + 144);
    /* nop */  /* 0x801700DC: 0x00000000 */
    int _bc_801700E0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801700E4: 0x00000000 */
    if (_bc_801700E0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801700E8u);
        goto block_801700E8;  /* not taken */
    }

block_801700E8:
    a0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x801700EC: 0x00000000 */
    a0 = a0 << 24;
    ra = 0x801700FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x80155EDCu);
    cpu->pc = 0x80155EDCu; return;  /* CPS jal -> 0x80155EDC */

block_801700FC:
    ra = 0x80170104u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8015787Cu);
    cpu->pc = 0x8015787Cu; return;  /* CPS jal -> 0x8015787C */

block_80170104:
    a0 = cpu->read_word(s0 + 188);
    ra = 0x80170110u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x8012F024u);
    cpu->pc = 0x8012F024u; return;  /* CPS jal -> 0x8012F024 */

block_80170110:
    ra = 0x80170118u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x80136FB0u);
    cpu->pc = 0x80136FB0u; return;  /* CPS jal -> 0x80136FB0 */

block_80170118:
    ra = 0x80170120u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = zero;
    psx_check_interrupts_at(cpu, 0x8012D090u);
    cpu->pc = 0x8012D090u; return;  /* CPS jal -> 0x8012D090 */

block_80170120:
    a0 = 2;
    a1 = zero;
    ra = 0x80170130u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x8012CBC0u);
    cpu->pc = 0x8012CBC0u; return;  /* CPS jal -> 0x8012CBC0 */

block_80170130:
    a0 = s0;
    ra = 0x8017013Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = -1;
    psx_check_interrupts_at(cpu, 0x8012AE3Cu);
    cpu->pc = 0x8012AE3Cu; return;  /* CPS jal -> 0x8012AE3C */

block_8017013C:
    v0 = 6;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_80170144 */

block_80170144:
    a0 = s3;
    a1 = s2;
    a3 = cpu->read_word(s0 + 192);
    ra = 0x80170158u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x8015A69Cu);
    cpu->pc = 0x8015A69Cu; return;  /* CPS jal -> 0x8015A69C */

block_80170158:
    a0 = s3;
    a1 = s2;
    ra = 0x80170168u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80151328u);
    cpu->pc = 0x80151328u; return;  /* CPS jal -> 0x80151328 */

block_80170168:
    ra = 0x80170170u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017016C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80129F78u);
    cpu->pc = 0x80129F78u; return;  /* CPS jal -> 0x80129F78 */

block_80170170:
    ra = 0x80170178u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = v0;
    psx_check_interrupts_at(cpu, 0x8012A030u);
    cpu->pc = 0x8012A030u; return;  /* CPS jal -> 0x8012A030 */

block_80170178:
    ra = 0x80170180u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x80127BE4u);
    cpu->pc = 0x80127BE4u; return;  /* CPS jal -> 0x80127BE4 */

block_80170180:
    ra = 0x80170188u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x80136FB0u);
    cpu->pc = 0x80136FB0u; return;  /* CPS jal -> 0x80136FB0 */

block_80170188:
    ra = 0x80170190u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x8012B49Cu);
    cpu->pc = 0x8012B49Cu; return;  /* CPS jal -> 0x8012B49C */

block_80170190:
    ra = 0x80170198u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 127;
    psx_check_interrupts_at(cpu, 0x80156114u);
    cpu->pc = 0x80156114u; return;  /* CPS jal -> 0x80156114 */

block_80170198:
    a0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x8017019C: 0x00000000 */
    a0 = a0 << 24;
    ra = 0x801701ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x80136D28u);
    cpu->pc = 0x80136D28u; return;  /* CPS jal -> 0x80136D28 */

block_801701AC:
    a0 = cpu->read_byte(s0 + 152);
    a1 = cpu->read_byte(s0 + 153);
    a0 = a0 << 24;
    a0 = (int32_t)a0 >> 24;
    a1 = a1 << 24;
    ra = 0x801701C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = (int32_t)a1 >> 24;
    psx_check_interrupts_at(cpu, 0x801358E8u);
    cpu->pc = 0x801358E8u; return;  /* CPS jal -> 0x801358E8 */

block_801701C8:
    a1 = s3;
    a0 = cpu->read_byte(s0 + 152);
    a2 = s2;
    a0 = a0 << 24;
    ra = 0x801701E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x8012E97Cu);
    cpu->pc = 0x8012E97Cu; return;  /* CPS jal -> 0x8012E97C */

block_801701E0:
    ra = 0x801701E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    psx_check_interrupts_at(cpu, 0x8016F2C4u);
    cpu->pc = 0x8016F2C4u; return;  /* CPS jal -> 0x8016F2C4 */

block_801701E8:
    ra = 0x801701F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    psx_check_interrupts_at(cpu, 0x8016F2C4u);
    cpu->pc = 0x8016F2C4u; return;  /* CPS jal -> 0x8016F2C4 */

block_801701F0:
    ra = 0x801701F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801701F4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80142D58u);
    cpu->pc = 0x80142D58u; return;  /* CPS jal -> 0x80142D58 */

block_801701F8:
    a0 = s0;
    a2 = cpu->read_byte(s0 + 154);
    ra = 0x80170208u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s3;
    psx_check_interrupts_at(cpu, 0x80175FECu);
    cpu->pc = 0x80175FECu; return;  /* CPS jal -> 0x80175FEC */

block_80170208:
    a0 = s0;
    a2 = cpu->read_byte(s0 + 154);
    ra = 0x80170218u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s2;
    psx_check_interrupts_at(cpu, 0x80175FECu);
    cpu->pc = 0x80175FECu; return;  /* CPS jal -> 0x80175FEC */

block_80170218:
    a0 = cpu->read_byte(s0 + 153);
    /* nop */  /* 0x8017021C: 0x00000000 */
    a0 = a0 << 24;
    ra = 0x8017022Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = (int32_t)a0 >> 24;
    psx_check_interrupts_at(cpu, 0x80172A34u);
    cpu->pc = 0x80172A34u; return;  /* CPS jal -> 0x80172A34 */

block_8017022C:
    ra = 0x80170234u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8016D478u);
    cpu->pc = 0x8016D478u; return;  /* CPS jal -> 0x8016D478 */

block_80170234:
    ra = 0x8017023Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170238: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801360CCu);
    cpu->pc = 0x801360CCu; return;  /* CPS jal -> 0x801360CC */

block_8017023C:
    ra = 0x80170244u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_80170244:
    v0 = 7;
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_8017024C */

block_8017024C:
    ra = 0x80170254u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170250: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_80170254:
    int _bc_80170254 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (_bc_80170254) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017025Cu);
        goto block_8017025C;  /* not taken */
    }

block_8017025C:
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    /* fall through to block_80170260 */

block_80170260:
    v1 = cpu->read_byte(s0 + 155);
    v0 = 4;
    int _bc_80170268 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    if (_bc_80170268) {
        psx_check_interrupts_at(cpu, 0x80170284u);
        goto block_80170284;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170270u);
        goto block_80170270;  /* not taken */
    }

block_80170270:
    a1 = s2;
    ra = 0x8017027Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80154930u);
    cpu->pc = 0x80154930u; return;  /* CPS jal -> 0x80154930 */

block_8017027C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80170280: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80170290u);
    goto block_80170290;  /* j */

block_80170284:
    a1 = s2;
    ra = 0x80170290u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x80155C14u);
    cpu->pc = 0x80155C14u; return;  /* CPS jal -> 0x80155C14 */

block_80170290:
    v0 = cpu->read_byte(s0 + 1);
    /* nop */  /* 0x80170294: 0x00000000 */
    v1 = v0 & 0x00FF;  /* 0x80170298: 0x304300FF */
    v0 = 6;
    int _bc_801702A0 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 8;
    if (_bc_801702A0) {
        psx_check_interrupts_at(cpu, 0x801702C0u);
        goto block_801702C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702A8u);
        goto block_801702A8;  /* not taken */
    }

block_801702A8:
    int _bc_801702A8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801702AC: 0x00000000 */
    if (_bc_801702A8) {
        psx_check_interrupts_at(cpu, 0x801702C0u);
        goto block_801702C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702B0u);
        goto block_801702B0;  /* not taken */
    }

block_801702B0:
    ra = 0x801702B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012B23Cu);
    cpu->pc = 0x8012B23Cu; return;  /* CPS jal -> 0x8012B23C */

block_801702B8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801702BC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801709A0u);
    goto block_801709A0;  /* j */

block_801702C0:
    v0 = cpu->read_word(s0 + 196);
    /* nop */  /* 0x801702C4: 0x00000000 */
    v0 = cpu->read_word(v0 + 8);
    /* nop */  /* 0x801702CC: 0x00000000 */
    v0 = (uint32_t)v0 >> 11;
    v0 = v0 & 0x0001;  /* 0x801702D4: 0x30420001 */
    int _bc_801702D8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_801702D8) {
        psx_check_interrupts_at(cpu, 0x801702E4u);
        goto block_801702E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702E0u);
        goto block_801702E0;  /* not taken */
    }

block_801702E0:
    cpu->write_byte(s0 + 158, (uint8_t)v0);
    /* fall through to block_801702E4 */

block_801702E4:
    v0 = cpu->read_half(s0 + 130);
    /* nop */  /* 0x801702E8: 0x00000000 */
    int _bc_801702EC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801702F0: 0x00000000 */
    if (_bc_801702EC) {
        psx_check_interrupts_at(cpu, 0x80170334u);
        goto block_80170334;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801702F4u);
        goto block_801702F4;  /* not taken */
    }

block_801702F4:
    ra = 0x801702FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x80128898u);
    cpu->pc = 0x80128898u; return;  /* CPS jal -> 0x80128898 */

block_801702FC:
    a0 = s3;
    a1 = s2;
    ra = 0x8017030Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x8017F398u);
    cpu->pc = 0x8017F398u; return;  /* CPS jal -> 0x8017F398 */

block_8017030C:
    a0 = s3;
    ra = 0x80170318u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x801787DCu);
    cpu->pc = 0x801787DCu; return;  /* CPS jal -> 0x801787DC */

block_80170318:
    a0 = s2;
    ra = 0x80170324u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x801787DCu);
    cpu->pc = 0x801787DCu; return;  /* CPS jal -> 0x801787DC */

block_80170324:
    a0 = s3;
    a1 = s2;
    ra = 0x80170334u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s0;
    psx_check_interrupts_at(cpu, 0x801809C8u);
    cpu->pc = 0x801809C8u; return;  /* CPS jal -> 0x801809C8 */

block_80170334:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170338: 0x00000000 */
    v0 = (uint32_t)v0 >> 2;
    v0 = v0 & 0x0001;  /* 0x80170340: 0x30420001 */
    int _bc_80170344 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s3;
    if (_bc_80170344) {
        psx_check_interrupts_at(cpu, 0x80170384u);
        goto block_80170384;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017034Cu);
        goto block_8017034C;  /* not taken */
    }

block_8017034C:
    v0 = cpu->read_byte(s0 + 176);
    /* nop */  /* 0x80170350: 0x00000000 */
    int _bc_80170354 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = s2;
    if (_bc_80170354) {
        psx_check_interrupts_at(cpu, 0x80170388u);
        goto block_80170388;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017035Cu);
        goto block_8017035C;  /* not taken */
    }

block_8017035C:
    v0 = cpu->read_byte(s0 + 157);
    /* nop */  /* 0x80170360: 0x00000000 */
    int _bc_80170364 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 16;
    if (_bc_80170364) {
        psx_check_interrupts_at(cpu, 0x80170370u);
        goto block_80170370;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017036Cu);
        goto block_8017036C;  /* not taken */
    }

block_8017036C:
    a0 = s5 + 8248;
    /* fall through to block_80170370 */

block_80170370:
    a1 = s0;
    a2 = s3;
    ra = 0x80170380u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x801737ECu);
    cpu->pc = 0x801737ECu; return;  /* CPS jal -> 0x801737EC */

block_80170380:
    a1 = s3;
    /* fall through to block_80170384 */

block_80170384:
    a2 = s2;
    /* fall through to block_80170388 */

block_80170388:
    a0 = cpu->read_word(s0 + 188);
    ra = 0x80170394u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x8012FB10u);
    cpu->pc = 0x8012FB10u; return;  /* CPS jal -> 0x8012FB10 */

block_80170394:
    a0 = cpu->read_word(s0 + 188);
    ra = 0x801703A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = s0;
    psx_check_interrupts_at(cpu, 0x8012EE48u);
    cpu->pc = 0x8012EE48u; return;  /* CPS jal -> 0x8012EE48 */

block_801703A0:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x801703A4: 0x00000000 */
    v0 = v0 & 0x0001;  /* 0x801703A8: 0x30420001 */
    int _bc_801703AC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801703B0: 0x00000000 */
    if (_bc_801703AC) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703B4u);
        goto block_801703B4;  /* not taken */
    }

block_801703B4:
    v0 = cpu->read_word(s3 + 96);
    /* nop */  /* 0x801703B8: 0x00000000 */
    v0 = v0 & 0x0002;  /* 0x801703BC: 0x30420002 */
    int _bc_801703C0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_801703C0) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703C8u);
        goto block_801703C8;  /* not taken */
    }

block_801703C8:
    a1 = s4;
    a2 = s0;
    ra = 0x801703D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s3;
    psx_check_interrupts_at(cpu, 0x8016CD8Cu);
    cpu->pc = 0x8016CD8Cu; return;  /* CPS jal -> 0x8016CD8C */

block_801703D8:
    v1 = cpu->read_word(s3 + 108);
    /* nop */  /* 0x801703DC: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x801703E0: 0x30620400 */
    int _bc_801703E4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x0200;  /* 0x801703E8: 0x30620200 */
    if (_bc_801703E4) {
        psx_check_interrupts_at(cpu, 0x801704BCu);
        goto block_801704BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703ECu);
        goto block_801703EC;  /* not taken */
    }

block_801703EC:
    int _bc_801703EC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    if (_bc_801703EC) {
        psx_check_interrupts_at(cpu, 0x80170448u);
        goto block_80170448;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801703F4u);
        goto block_801703F4;  /* not taken */
    }

block_801703F4:
    ra = 0x801703FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_801703FC:
    v0 = cpu->read_word(s3 + 40);
    s1 = s5 + 64;
    a3 = (int32_t)(int8_t)cpu->read_byte(v0);
    a1 = s1;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 16);
    ra = 0x80170420u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_80170420:
    int _bc_80170420 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    if (_bc_80170420) {
        psx_check_interrupts_at(cpu, 0x801704BCu);
        goto block_801704BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170428u);
        goto block_80170428;  /* not taken */
    }

block_80170428:
    v0 = cpu->read_word(s3 + 40);
    /* nop */  /* 0x8017042C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801704B4u);
    goto block_801704B4;  /* j */

block_80170448:
    v0 = 0x0800 << 16;  /* 0x08000000 */
    v0 = v1 & v0;  /* 0x8017044C: 0x00621024 */
    int _bc_80170450 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170454: 0x00000000 */
    if (_bc_80170450) {
        psx_check_interrupts_at(cpu, 0x8017048Cu);
        goto block_8017048C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170458u);
        goto block_80170458;  /* not taken */
    }

block_80170458:
    int _bc_80170458 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017045C: 0x00000000 */
    if (_bc_80170458) {
        psx_check_interrupts_at(cpu, 0x8017048Cu);
        goto block_8017048C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170460u);
        goto block_80170460;  /* not taken */
    }

block_80170460:
    ra = 0x80170468u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170468:
    v0 = cpu->read_word(s3 + 40);
    /* nop */  /* 0x8017046C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    a1 = s5 + 64;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801704B4u);
    goto block_801704B4;  /* j */

block_8017048C:
    ra = 0x80170494u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 1;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170494:
    a1 = s5 + 64;
    a0 = cpu->read_word(s3 + 24);
    a3 = cpu->read_byte(s3 + 6);
    a2 = s4;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a3 = a3 << 24;
    a3 = (int32_t)a3 >> 24;
    /* fall through to block_801704B4 */

block_801704B4:
    ra = 0x801704BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801704B8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_801704BC:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x801704C0: 0x00000000 */
    v0 = (uint32_t)v0 >> 6;
    v0 = v0 & 0x0001;  /* 0x801704C8: 0x30420001 */
    int _bc_801704CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801704D0: 0x00000000 */
    if (_bc_801704CC) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801704D4u);
        goto block_801704D4;  /* not taken */
    }

block_801704D4:
    v0 = cpu->read_byte(s0 + 159);
    /* nop */  /* 0x801704D8: 0x00000000 */
    int _bc_801704DC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801704E0: 0x00000000 */
    if (_bc_801704DC) {
        psx_check_interrupts_at(cpu, 0x8017058Cu);
        goto block_8017058C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801704E4u);
        goto block_801704E4;  /* not taken */
    }

block_801704E4:
    v0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x801704E8: 0x00000000 */
    v0 = v0 << 24;
    v1 = (int32_t)v0 >> 24;
    v0 = 4;
    int _bc_801704F8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 5) ? 1 : 0;  /* 0x801704FC: 0x28620005 */
    if (_bc_801704F8) {
        psx_check_interrupts_at(cpu, 0x80170524u);
        goto block_80170524;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170500u);
        goto block_80170500;  /* not taken */
    }

block_80170500:
    int _bc_80170500 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_80170500) {
        psx_check_interrupts_at(cpu, 0x80170518u);
        goto block_80170518;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170508u);
        goto block_80170508;  /* not taken */
    }

block_80170508:
    int _bc_80170508 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_80170508) {
        psx_check_interrupts_at(cpu, 0x80170524u);
        goto block_80170524;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170510u);
        goto block_80170510;  /* not taken */
    }

block_80170510:
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x80170550u);
    goto block_80170550;  /* j */

block_80170518:
    v0 = 6;
    int _bc_8017051C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_8017051C) {
        psx_check_interrupts_at(cpu, 0x8017054Cu);
        goto block_8017054C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170524u);
        goto block_80170524;  /* not taken */
    }

block_80170524:
    a1 = s5 + 8256;
    a2 = s4;
    a0 = cpu->read_word(s3 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    ra = 0x80170544u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, t0);
    psx_check_interrupts_at(cpu, 0x8018B100u);
    cpu->pc = 0x8018B100u; return;  /* CPS jal -> 0x8018B100 */

block_80170544:
    /* delay slot (always executes) */
    /* nop */  /* 0x80170548: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017058Cu);
    goto block_8017058C;  /* j */

block_8017054C:
    a2 = s4;
    /* fall through to block_80170550 */

block_80170550:
    a0 = cpu->read_word(s3 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, t0);
    v0 = cpu->read_byte(s0 + 152);
    v1 = s5 + 8300;
    cpu->write_word(sp + 28, v1);
    v1 = 0x801B << 16;  /* 0x801B0000 */
    v1 = v1 + 5812;
    v0 = v0 << 24;
    v0 = (int32_t)v0 >> 21;
    v0 = v0 + v1;
    ra = 0x8017058Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, v0);
    psx_check_interrupts_at(cpu, 0x8018B208u);
    cpu->pc = 0x8018B208u; return;  /* CPS jal -> 0x8018B208 */

block_8017058C:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170590: 0x00000000 */
    v0 = (uint32_t)v0 >> 1;
    v0 = v0 & 0x0001;  /* 0x80170598: 0x30420001 */
    int _bc_8017059C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801705A0: 0x00000000 */
    if (_bc_8017059C) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705A4u);
        goto block_801705A4;  /* not taken */
    }

block_801705A4:
    v0 = cpu->read_word(s2 + 96);
    /* nop */  /* 0x801705A8: 0x00000000 */
    v0 = v0 & 0x0002;  /* 0x801705AC: 0x30420002 */
    int _bc_801705B0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_801705B0) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705B8u);
        goto block_801705B8;  /* not taken */
    }

block_801705B8:
    a1 = s4;
    a2 = s0;
    ra = 0x801705C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x8016CD8Cu);
    cpu->pc = 0x8016CD8Cu; return;  /* CPS jal -> 0x8016CD8C */

block_801705C8:
    v1 = cpu->read_word(s2 + 108);
    /* nop */  /* 0x801705CC: 0x00000000 */
    v0 = v1 & 0x0400;  /* 0x801705D0: 0x30620400 */
    int _bc_801705D4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x0200;  /* 0x801705D8: 0x30620200 */
    if (_bc_801705D4) {
        psx_check_interrupts_at(cpu, 0x801706ACu);
        goto block_801706AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705DCu);
        goto block_801705DC;  /* not taken */
    }

block_801705DC:
    int _bc_801705DC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    if (_bc_801705DC) {
        psx_check_interrupts_at(cpu, 0x80170638u);
        goto block_80170638;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801705E4u);
        goto block_801705E4;  /* not taken */
    }

block_801705E4:
    ra = 0x801705ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_801705EC:
    v0 = cpu->read_word(s2 + 40);
    s1 = s5 + 64;
    a3 = (int32_t)(int8_t)cpu->read_byte(v0);
    a1 = s1;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 16);
    ra = 0x80170610u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_80170610:
    int _bc_80170610 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s1;
    if (_bc_80170610) {
        psx_check_interrupts_at(cpu, 0x801706ACu);
        goto block_801706AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170618u);
        goto block_80170618;  /* not taken */
    }

block_80170618:
    v0 = cpu->read_word(s2 + 40);
    /* nop */  /* 0x8017061C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801706A4u);
    goto block_801706A4;  /* j */

block_80170638:
    v0 = 0x0800 << 16;  /* 0x08000000 */
    v0 = v1 & v0;  /* 0x8017063C: 0x00621024 */
    int _bc_80170640 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170644: 0x00000000 */
    if (_bc_80170640) {
        psx_check_interrupts_at(cpu, 0x8017067Cu);
        goto block_8017067C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170648u);
        goto block_80170648;  /* not taken */
    }

block_80170648:
    int _bc_80170648 = (s4 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017064C: 0x00000000 */
    if (_bc_80170648) {
        psx_check_interrupts_at(cpu, 0x8017067Cu);
        goto block_8017067C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170650u);
        goto block_80170650;  /* not taken */
    }

block_80170650:
    ra = 0x80170658u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170658:
    v0 = cpu->read_word(s2 + 40);
    /* nop */  /* 0x8017065C: 0x00000000 */
    a3 = (int32_t)(int8_t)cpu->read_byte(v0 + 36);
    a1 = s5 + 64;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a0 = cpu->read_word(v0 + 52);
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x801706A4u);
    goto block_801706A4;  /* j */

block_8017067C:
    ra = 0x80170684u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 2;
    psx_check_interrupts_at(cpu, 0x8012ECBCu);
    cpu->pc = 0x8012ECBCu; return;  /* CPS jal -> 0x8012ECBC */

block_80170684:
    a1 = s5 + 64;
    a0 = cpu->read_word(s2 + 24);
    a3 = cpu->read_byte(s2 + 6);
    a2 = s4;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, s7);
    a3 = a3 << 24;
    a3 = (int32_t)a3 >> 24;
    /* fall through to block_801706A4 */

block_801706A4:
    ra = 0x801706ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801706A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801871BCu);
    cpu->pc = 0x801871BCu; return;  /* CPS jal -> 0x801871BC */

block_801706AC:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x801706B0: 0x00000000 */
    v0 = (uint32_t)v0 >> 6;
    v0 = v0 & 0x0001;  /* 0x801706B8: 0x30420001 */
    int _bc_801706BC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801706C0: 0x00000000 */
    if (_bc_801706BC) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706C4u);
        goto block_801706C4;  /* not taken */
    }

block_801706C4:
    v0 = cpu->read_byte(s0 + 159);
    /* nop */  /* 0x801706C8: 0x00000000 */
    int _bc_801706CC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801706D0: 0x00000000 */
    if (_bc_801706CC) {
        psx_check_interrupts_at(cpu, 0x8017077Cu);
        goto block_8017077C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706D4u);
        goto block_801706D4;  /* not taken */
    }

block_801706D4:
    v0 = cpu->read_byte(s0 + 152);
    /* nop */  /* 0x801706D8: 0x00000000 */
    v0 = v0 << 24;
    v1 = (int32_t)v0 >> 24;
    v0 = 4;
    int _bc_801706E8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = ((int32_t)v1 < 5) ? 1 : 0;  /* 0x801706EC: 0x28620005 */
    if (_bc_801706E8) {
        psx_check_interrupts_at(cpu, 0x80170714u);
        goto block_80170714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706F0u);
        goto block_801706F0;  /* not taken */
    }

block_801706F0:
    int _bc_801706F0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;
    if (_bc_801706F0) {
        psx_check_interrupts_at(cpu, 0x80170708u);
        goto block_80170708;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801706F8u);
        goto block_801706F8;  /* not taken */
    }

block_801706F8:
    int _bc_801706F8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_801706F8) {
        psx_check_interrupts_at(cpu, 0x80170714u);
        goto block_80170714;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170700u);
        goto block_80170700;  /* not taken */
    }

block_80170700:
    /* delay slot (always executes) */
    a2 = s4;
    psx_check_interrupts_at(cpu, 0x80170740u);
    goto block_80170740;  /* j */

block_80170708:
    v0 = 6;
    int _bc_8017070C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = s5 + 8256;
    if (_bc_8017070C) {
        psx_check_interrupts_at(cpu, 0x8017073Cu);
        goto block_8017073C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170714u);
        goto block_80170714;  /* not taken */
    }

block_80170714:
    a1 = s5 + 8256;
    a2 = s4;
    a0 = cpu->read_word(s2 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    ra = 0x80170734u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, t0);
    psx_check_interrupts_at(cpu, 0x8018B100u);
    cpu->pc = 0x8018B100u; return;  /* CPS jal -> 0x8018B100 */

block_80170734:
    /* delay slot (always executes) */
    /* nop */  /* 0x80170738: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017077Cu);
    goto block_8017077C;  /* j */

block_8017073C:
    a2 = s4;
    /* fall through to block_80170740 */

block_80170740:
    a0 = cpu->read_word(s2 + 28);
    t0 = cpu->read_word(sp + 32);
    a3 = 15;
    cpu->write_word(sp + 16, s6);
    cpu->write_word(sp + 20, t0);
    v0 = cpu->read_byte(s0 + 152);
    v1 = s5 + 8300;
    cpu->write_word(sp + 28, v1);
    v1 = 0x801B << 16;  /* 0x801B0000 */
    v1 = v1 + 5812;
    v0 = v0 << 24;
    v0 = (int32_t)v0 >> 21;
    v0 = v0 + v1;
    ra = 0x8017077Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(sp + 24, v0);
    psx_check_interrupts_at(cpu, 0x8018B208u);
    cpu->pc = 0x8018B208u; return;  /* CPS jal -> 0x8018B208 */

block_8017077C:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170780: 0x00000000 */
    v0 = (uint32_t)v0 >> 4;
    v0 = v0 & 0x0001;  /* 0x80170788: 0x30420001 */
    int _bc_8017078C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s6;
    if (_bc_8017078C) {
        psx_check_interrupts_at(cpu, 0x8017079Cu);
        goto block_8017079C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170794u);
        goto block_80170794;  /* not taken */
    }

block_80170794:
    v0 = 1;
    cpu->write_byte(s0 + 177, (uint8_t)v0);
    /* fall through to block_8017079C */

block_8017079C:
    a1 = s5;
    a2 = s4;
    ra = 0x801707ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x80137098u);
    cpu->pc = 0x80137098u; return;  /* CPS jal -> 0x80137098 */

block_801707AC:
    v0 = cpu->read_word(s3 + 108);
    /* nop */  /* 0x801707B0: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801707B4: 0x30428000 */
    int _bc_801707B8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_801707B8) {
        psx_check_interrupts_at(cpu, 0x801707D4u);
        goto block_801707D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801707C0u);
        goto block_801707C0;  /* not taken */
    }

block_801707C0:
    v0 = cpu->read_word(s2 + 108);
    /* nop */  /* 0x801707C4: 0x00000000 */
    v0 = v0 & 0x8000;  /* 0x801707C8: 0x30428000 */
    int _bc_801707CC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = -8;
    if (_bc_801707CC) {
        psx_check_interrupts_at(cpu, 0x801707E0u);
        goto block_801707E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801707D4u);
        goto block_801707D4;  /* not taken */
    }

block_801707D4:
    t0 = t0 + 19632;
    /* delay slot (always executes) */
    v0 = 16;
    psx_check_interrupts_at(cpu, 0x801707E4u);
    goto block_801707E4;  /* j */

block_801707E0:
    t0 = t0 + 19632;
    /* fall through to block_801707E4 */

block_801707E4:
    cpu->write_half(t0 + 2, (uint16_t)zero);
    cpu->write_half(t0 + 6, (uint16_t)v0);
    a0 = s5;
    a1 = s4;
    a3 = cpu->read_byte(s0 + 152);
    a2 = s0;
    a3 = a3 << 24;
    ra = 0x80170808u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = (int32_t)a3 >> 24;
    psx_check_interrupts_at(cpu, 0x8016D244u);
    cpu->pc = 0x8016D244u; return;  /* CPS jal -> 0x8016D244 */

block_80170808:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x8017080C: 0x00000000 */
    v0 = (uint32_t)v0 >> 3;
    v0 = v0 & 0x0001;  /* 0x80170814: 0x30420001 */
    int _bc_80170818 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s6;
    if (_bc_80170818) {
        psx_check_interrupts_at(cpu, 0x80170830u);
        goto block_80170830;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170820u);
        goto block_80170820;  /* not taken */
    }

block_80170820:
    a1 = s7;
    a2 = cpu->read_word(s0 + 188);
    ra = 0x80170830u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x80135180u);
    cpu->pc = 0x80135180u; return;  /* CPS jal -> 0x80135180 */

block_80170830:
    v0 = cpu->read_byte(s0 + 159);
    /* nop */  /* 0x80170834: 0x00000000 */
    int _bc_80170838 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017083C: 0x00000000 */
    if (_bc_80170838) {
        psx_check_interrupts_at(cpu, 0x80170878u);
        goto block_80170878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170840u);
        goto block_80170840;  /* not taken */
    }

block_80170840:
    v0 = cpu->read_byte(s0 + 158);
    /* nop */  /* 0x80170844: 0x00000000 */
    int _bc_80170848 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017084C: 0x00000000 */
    if (_bc_80170848) {
        psx_check_interrupts_at(cpu, 0x80170878u);
        goto block_80170878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170850u);
        goto block_80170850;  /* not taken */
    }

block_80170850:
    v0 = cpu->read_half(fp + 2);
    /* nop */  /* 0x80170854: 0x00000000 */
    v0 = (uint32_t)v0 >> 5;
    v0 = v0 & 0x0001;  /* 0x8017085C: 0x30420001 */
    int _bc_80170860 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 8316;
    if (_bc_80170860) {
        psx_check_interrupts_at(cpu, 0x80170878u);
        goto block_80170878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170868u);
        goto block_80170868;  /* not taken */
    }

block_80170868:
    a1 = cpu->read_word(s0 + 188);
    a2 = s4;
    ra = 0x80170878u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s0;
    psx_check_interrupts_at(cpu, 0x80136B64u);
    cpu->pc = 0x80136B64u; return;  /* CPS jal -> 0x80136B64 */

block_80170878:
    v0 = (int32_t)(int16_t)cpu->read_half(fp + 8);
    /* nop */  /* 0x8017087C: 0x00000000 */
    int _bc_80170880 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_80170880) {
        psx_check_interrupts_at(cpu, 0x8017089Cu);
        goto block_8017089C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170888u);
        goto block_80170888;  /* not taken */
    }

block_80170888:
    cpu->write_word(sp + 16, v0);
    a1 = s4;
    a2 = s6;
    ra = 0x8017089Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s3;
    psx_check_interrupts_at(cpu, 0x801468ECu);
    cpu->pc = 0x801468ECu; return;  /* CPS jal -> 0x801468EC */

block_8017089C:
    v0 = (int32_t)(int16_t)cpu->read_half(fp + 10);
    /* nop */  /* 0x801708A0: 0x00000000 */
    int _bc_801708A4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5;
    if (_bc_801708A4) {
        psx_check_interrupts_at(cpu, 0x801708C0u);
        goto block_801708C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801708ACu);
        goto block_801708AC;  /* not taken */
    }

block_801708AC:
    cpu->write_word(sp + 16, v0);
    a1 = s4;
    a2 = s6;
    ra = 0x801708C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a3 = s2;
    psx_check_interrupts_at(cpu, 0x801468ECu);
    cpu->pc = 0x801468ECu; return;  /* CPS jal -> 0x801468EC */

block_801708C0:
    a1 = (int32_t)(int16_t)cpu->read_half(fp + 4);
    /* nop */  /* 0x801708C4: 0x00000000 */
    int _bc_801708C8 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801708CC: 0x00000000 */
    if (_bc_801708C8) {
        psx_check_interrupts_at(cpu, 0x801708D8u);
        goto block_801708D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801708D0u);
        goto block_801708D0;  /* not taken */
    }

block_801708D0:
    ra = 0x801708D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s3;
    psx_check_interrupts_at(cpu, 0x8016F588u);
    cpu->pc = 0x8016F588u; return;  /* CPS jal -> 0x8016F588 */

block_801708D8:
    a1 = (int32_t)(int16_t)cpu->read_half(fp + 6);
    /* nop */  /* 0x801708DC: 0x00000000 */
    int _bc_801708E0 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801708E4: 0x00000000 */
    if (_bc_801708E0) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801708E8u);
        goto block_801708E8;  /* not taken */
    }

block_801708E8:
    ra = 0x801708F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s2;
    psx_check_interrupts_at(cpu, 0x8016F588u);
    cpu->pc = 0x8016F588u; return;  /* CPS jal -> 0x8016F588 */

block_801708F0:
    /* delay slot (always executes) */
    /* nop */  /* 0x801708F4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_801708F8:
    ra = 0x80170900u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012B23Cu);
    cpu->pc = 0x8012B23Cu; return;  /* CPS jal -> 0x8012B23C */

block_80170900:
    ra = 0x80170908u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170904: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80136DDCu);
    cpu->pc = 0x80136DDCu; return;  /* CPS jal -> 0x80136DDC */

block_80170908:
    ra = 0x80170910u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x801560F8u);
    cpu->pc = 0x801560F8u; return;  /* CPS jal -> 0x801560F8 */

block_80170910:
    ra = 0x80170918u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170914: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80155E30u);
    cpu->pc = 0x80155E30u; return;  /* CPS jal -> 0x80155E30 */

block_80170918:
    a0 = 516;
    a1 = zero;
    ra = 0x80170928u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a1;
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_80170928:
    a0 = 1;
    ra = 0x80170934u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a1 = 8;
    psx_check_interrupts_at(cpu, 0x8012AA6Cu);
    cpu->pc = 0x8012AA6Cu; return;  /* CPS jal -> 0x8012AA6C */

block_80170934:
    a0 = 1;
    cpu->write_byte(s0 + 16, (uint8_t)zero);
    ra = 0x80170944u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 158, (uint8_t)zero);
    psx_check_interrupts_at(cpu, 0x80136FB0u);
    cpu->pc = 0x80136FB0u; return;  /* CPS jal -> 0x80136FB0 */

block_80170944:
    a0 = zero;
    a1 = a0;
    ra = 0x80170954u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a2 = a0;
    psx_check_interrupts_at(cpu, 0x801274FCu);
    cpu->pc = 0x801274FCu; return;  /* CPS jal -> 0x801274FC */

block_80170954:
    v0 = 12;
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_80170960:
    ra = 0x80170968u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = 1;
    psx_check_interrupts_at(cpu, 0x8016E21Cu);
    cpu->pc = 0x8016E21Cu; return;  /* CPS jal -> 0x8016E21C */

block_80170968:
    v1 = cpu->read_byte(s0 + 1);
    v0 = 11;
    int _bc_80170970 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;
    if (_bc_80170970) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170978u);
        goto block_80170978;  /* not taken */
    }

block_80170978:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)v0);
    psx_check_interrupts_at(cpu, 0x80170998u);
    goto block_80170998;  /* j */

block_80170980:
    ra = 0x80170988u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80170984: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_80170988:
    int _bc_80170988 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = 255;
    if (_bc_80170988) {
        psx_check_interrupts_at(cpu, 0x80170998u);
        goto block_80170998;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80170990u);
        goto block_80170990;  /* not taken */
    }

block_80170990:
    /* delay slot (always executes) */
    cpu->write_byte(s0 + 1, (uint8_t)t0);
    psx_check_interrupts_at(cpu, 0x801709A0u);
    goto block_801709A0;  /* j */

block_80170998:
    ra = 0x801709A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_801709A0:
    v1 = cpu->read_byte(s0 + 1);
    v0 = 255;
    int _bc_801709A8 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801709AC: 0x00000000 */
    if (_bc_801709A8) {
        psx_check_interrupts_at(cpu, 0x8016FD38u);
        goto block_8016FD38;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801709B0u);
        goto block_801709B0;  /* not taken */
    }

block_801709B0:
    v1 = cpu->read_byte(s0);
    v0 = 9;
    int _bc_801709B8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801709BC: 0x00000000 */
    if (_bc_801709B8) {
        psx_check_interrupts_at(cpu, 0x801709C8u);
        goto block_801709C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801709C0u);
        goto block_801709C0;  /* not taken */
    }

block_801709C0:
    ra = 0x801709C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    a0 = s0;
    psx_check_interrupts_at(cpu, 0x801525C4u);
    cpu->pc = 0x801525C4u; return;  /* CPS jal -> 0x801525C4 */

block_801709C8:
    cpu->write_byte(s0 + 1, (uint8_t)zero);
    ra = cpu->read_word(sp + 76);
    fp = cpu->read_word(sp + 72);
    s7 = cpu->read_word(sp + 68);
    s6 = cpu->read_word(sp + 64);
    s5 = cpu->read_word(sp + 60);
    s4 = cpu->read_word(sp + 56);
    s3 = cpu->read_word(sp + 52);
    s2 = cpu->read_word(sp + 48);
    s1 = cpu->read_word(sp + 44);
    s0 = cpu->read_word(sp + 40);
    uint32_t _jt_801709F4 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 80;
    psx_check_interrupts_at(cpu, _jt_801709F4);
    cpu->pc = _jt_801709F4; return;  /* CPS: jr $ra */
}


/* cps_80175964 — clean CPS transliteration of br2eu_func_80175898 (guest 0x80175964)
 * from SLES_017.22_full_32.c. */
void cps_80175964(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80175944u: goto block_80175944;
            case 0x80175964u: goto block_80175964;
            case 0x8017597Cu: goto block_8017597C;
            case 0x80175980u: goto block_80175980;
            case 0x80175990u: goto block_80175990;
            case 0x801759A0u: goto block_801759A0;
            case 0x801759B0u: goto block_801759B0;
            case 0x801759C4u: goto block_801759C4;
            case 0x801759D0u: goto block_801759D0;
            case 0x80175A10u: goto block_80175A10;
            default: break;
        }
    }
    /* Address: 0x80175898, Size: 384 bytes, Blocks: 11 */

block_80175898:
    a2 = a0;
    v0 = 480;
    cpu->write_half(a2 + 206, (uint16_t)v0);
    v0 = 0x801F << 16;  /* 0x801F0000 */
    v1 = cpu->read_byte(a2 + 2);
    a0 = cpu->read_byte(a2 + 228);
    v0 = v0 + 21224;
    cpu->write_half(a2 + 124, (uint16_t)zero);
    cpu->write_half(a2 + 122, (uint16_t)zero);
    cpu->write_half(a2 + 138, (uint16_t)zero);
    cpu->write_half(a2 + 140, (uint16_t)zero);
    cpu->write_half(a2 + 142, (uint16_t)zero);
    cpu->write_half(a2 + 144, (uint16_t)zero);
    cpu->write_half(a2 + 146, (uint16_t)zero);
    cpu->write_half(a2 + 148, (uint16_t)zero);
    cpu->write_half(a2 + 150, (uint16_t)zero);
    cpu->write_half(a2 + 152, (uint16_t)zero);
    cpu->write_byte(a2 + 218, (uint8_t)zero);
    cpu->write_half(a2 + 198, (uint16_t)zero);
    cpu->write_byte(a2 + 219, (uint8_t)zero);
    cpu->write_half(a2 + 250, (uint16_t)zero);
    cpu->write_half(a2 + 200, (uint16_t)zero);
    cpu->write_byte(a2 + 262, (uint8_t)zero);
    cpu->write_half(a2 + 192, (uint16_t)zero);
    cpu->write_half(a2 + 196, (uint16_t)zero);
    cpu->write_half(a2 + 178, (uint16_t)zero);
    cpu->write_half(a2 + 208, (uint16_t)zero);
    cpu->write_byte(a2 + 221, (uint8_t)zero);
    cpu->write_half(a2 + 202, (uint16_t)zero);
    cpu->write_half(a2 + 154, (uint16_t)zero);
    cpu->write_half(a2 + 156, (uint16_t)zero);
    cpu->write_half(a2 + 158, (uint16_t)zero);
    cpu->write_half(a2 + 162, (uint16_t)zero);
    cpu->write_half(a2 + 164, (uint16_t)zero);
    cpu->write_half(a2 + 166, (uint16_t)zero);
    v1 = v1 << 5;
    v1 = v1 + v0;
    cpu->write_byte(a2 + 229, (uint8_t)a0);
    cpu->write_word(a2 + 20, v1);
    v1 = a1 & 0x000F;  /* 0x80175934: 0x30A3000F */
    v0 = (v1 < (uint32_t)5) ? 1 : 0;  /* 0x80175938: 0x2C620005 */
    int _bc_8017593C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_8017593C) {
        psx_check_interrupts_at(cpu, 0x80175A74u);
        cpu->pc = 0x80175A74u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80175944u);
        goto block_80175944;  /* not taken */
    }

block_80175944:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 9880;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80175958: 0x00000000 */
    uint32_t _jt_8017595C = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80175960: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017595C);
    cpu->pc = _jt_8017595C; return;  /* CPS: jr v0 */

block_80175964:
    v0 = 7680;
    cpu->write_half(a2 + 214, (uint16_t)v0);
    v1 = cpu->read_byte(a0 + 155);
    v0 = 4;
    int _bc_80175974 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 7680;
    if (_bc_80175974) {
        psx_check_interrupts_at(cpu, 0x80175980u);
        goto block_80175980;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017597Cu);
        goto block_8017597C;  /* not taken */
    }

block_8017597C:
    cpu->write_half(a2 + 214, (uint16_t)v0);
    /* fall through to block_80175980 */

block_80175980:
    v1 = cpu->read_byte(a0 + 156);
    v0 = 3;
    int _bc_80175988 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017598C: 0x00000000 */
    if (_bc_80175988) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80175990u);
        goto block_80175990;  /* not taken */
    }

block_80175990:
    v0 = cpu->read_byte(a2 + 1);
    /* nop */  /* 0x80175994: 0x00000000 */
    int _bc_80175998 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017599C: 0x00000000 */
    if (_bc_80175998) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801759A0u);
        goto block_801759A0;  /* not taken */
    }

block_801759A0:
    v0 = cpu->read_half(a0 + 162);
    /* nop */  /* 0x801759A4: 0x00000000 */
    int _bc_801759A8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801759AC: 0x00000000 */
    if (_bc_801759A8) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801759B0u);
        goto block_801759B0;  /* not taken */
    }

block_801759B0:
    v0 = cpu->read_half(a2 + 172);
    /* nop */  /* 0x801759B4: 0x00000000 */
    cpu->write_half(a2 + 170, (uint16_t)v0);
    /* delay slot (always executes) */
    cpu->write_half(a2 + 174, (uint16_t)v0);
    psx_check_interrupts_at(cpu, 0x801759D0u);
    goto block_801759D0;  /* j */

block_801759C4:
    cpu->write_half(a2 + 172, (uint16_t)zero);
    cpu->write_half(a2 + 170, (uint16_t)zero);
    cpu->write_half(a2 + 174, (uint16_t)zero);
    /* fall through to block_801759D0 */

block_801759D0:
    v1 = (int32_t)(int16_t)cpu->read_half(a2 + 176);
    v0 = cpu->read_byte(a2 + 222);
    /* nop */  /* 0x801759D8: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);
    v0 = 0x51EB << 16;  /* 0x51EB0000 */
    v0 = v0 | 0x851F;
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));
    a0 = (int32_t)(int16_t)cpu->read_half(a2 + 214);
    cpu->write_half(a2 + 216, (uint16_t)zero);
    v1 = (int32_t)v1 >> 31;
    v0 = cpu->hi;
    psx_muldiv_stall(cpu);
    v0 = (int32_t)v0 >> 5;
    v0 = v0 - v1;
    int _bc_80175A08 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a2 + 176, (uint16_t)v0);
    if (_bc_80175A08) {
        psx_check_interrupts_at(cpu, 0x80175A74u);
        cpu->pc = 0x80175A74u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80175A10u);
        goto block_80175A10;  /* not taken */
    }

block_80175A10:
    uint32_t _jt_80175A10 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a2 + 214, (uint16_t)zero);
    psx_check_interrupts_at(cpu, _jt_80175A10);
    cpu->pc = _jt_80175A10; return;  /* CPS: jr $ra */
}
