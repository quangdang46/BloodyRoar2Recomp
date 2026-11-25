/* cps_clean.c — AUTO-GENERATED clean CPS transliterations.
 * Regenerate:  python3 game/gen_cps_clean.py --file 0xADDR [0xADDR...]
 * Do NOT hand-edit — fix game/gen_cps_clean.py and regenerate.
 *
 * Each cps_<addr> is a faithful CPS transliteration of the generated
 * br2eu_func_* body: identical guest ops/order/yield points, minus cycle/
 * icache/cosim/debug instrumentation. Memory ops use the cpu->read_word /
 * cpu->write_word accessor family (MMIO-aware) so both main RAM and
 * 0x1F801xxx hardware regs resolve correctly. Dispatch addrs are registered
 * in game/clean_cps.map.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

/* cps_80196370 — clean CPS transliteration of br2eu_func_80196370 (guest 0x80196370)
 * from SLES_017.22_full_45.c. */
void cps_80196370(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196554u: goto block_80196554;
            case 0x8019659Cu: goto block_8019659C;
            case 0x801968A4u: goto block_801968A4;
            case 0x801968B4u: goto block_801968B4;
            case 0x801963BCu: goto block_801963BC;
            case 0x801963C4u: goto block_801963C4;
            case 0x801963D4u: goto block_801963D4;
            case 0x801963E8u: goto block_801963E8;
            case 0x801963ECu: goto block_801963EC;
            case 0x8019640Cu: goto block_8019640C;
            case 0x80196430u: goto block_80196430;
            case 0x8019643Cu: goto block_8019643C;
            case 0x80196444u: goto block_80196444;
            case 0x80196458u: goto block_80196458;
            case 0x80196498u: goto block_80196498;
            case 0x801964C0u: goto block_801964C0;
            case 0x801964D8u: goto block_801964D8;
            case 0x801964ECu: goto block_801964EC;
            case 0x80196504u: goto block_80196504;
            case 0x80196524u: goto block_80196524;
            case 0x80196534u: goto block_80196534;
            case 0x80196548u: goto block_80196548;
            case 0x80196568u: goto block_80196568;
            case 0x801965B4u: goto block_801965B4;
            case 0x801965CCu: goto block_801965CC;
            case 0x801965D4u: goto block_801965D4;
            case 0x801965F0u: goto block_801965F0;
            case 0x801965F8u: goto block_801965F8;
            case 0x80196610u: goto block_80196610;
            case 0x80196618u: goto block_80196618;
            case 0x80196640u: goto block_80196640;
            case 0x8019665Cu: goto block_8019665C;
            case 0x80196664u: goto block_80196664;
            case 0x8019667Cu: goto block_8019667C;
            case 0x80196684u: goto block_80196684;
            case 0x801966A4u: goto block_801966A4;
            case 0x801966ACu: goto block_801966AC;
            case 0x801966C4u: goto block_801966C4;
            case 0x801966CCu: goto block_801966CC;
            case 0x801966D4u: goto block_801966D4;
            case 0x801966D8u: goto block_801966D8;
            case 0x801966F0u: goto block_801966F0;
            case 0x801966F8u: goto block_801966F8;
            case 0x80196710u: goto block_80196710;
            case 0x80196718u: goto block_80196718;
            case 0x80196720u: goto block_80196720;
            case 0x80196728u: goto block_80196728;
            case 0x8019672Cu: goto block_8019672C;
            case 0x80196734u: goto block_80196734;
            case 0x80196738u: goto block_80196738;
            case 0x80196754u: goto block_80196754;
            case 0x8019675Cu: goto block_8019675C;
            case 0x80196774u: goto block_80196774;
            case 0x80196798u: goto block_80196798;
            case 0x801967C4u: goto block_801967C4;
            case 0x801967C8u: goto block_801967C8;
            case 0x801967E0u: goto block_801967E0;
            case 0x801967F0u: goto block_801967F0;
            case 0x801967F8u: goto block_801967F8;
            case 0x80196810u: goto block_80196810;
            case 0x80196818u: goto block_80196818;
            case 0x80196844u: goto block_80196844;
            case 0x80196848u: goto block_80196848;
            case 0x80196860u: goto block_80196860;
            case 0x80196870u: goto block_80196870;
            case 0x80196878u: goto block_80196878;
            case 0x80196890u: goto block_80196890;
            case 0x80196898u: goto block_80196898;
            case 0x801968B8u: goto block_801968B8;
            default: break;
        }
    }
    /* Address: 0x80196370, Size: 1372 bytes, Blocks: 70 */

block_80196370:
    cpu->gpr[29] = cpu->gpr[29] + -48;
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13556);
    cpu->gpr[2] = 1;
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[16]);
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[4] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -13544);
    /* nop */  /* 0x80196398: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4]);
    /* nop */  /* 0x801963A0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0007;  /* 0x801963A4: 0x30420007 */
    cpu->write_byte(cpu->gpr[29] + 16, (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[29] + 16);
    /* nop */  /* 0x801963B0: 0x00000000 */
    int _bc_801963B4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = cpu->gpr[0];
    if (_bc_801963B4) {
        psx_check_interrupts_at(cpu, 0x801968B4u);
        goto block_801968B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801963BCu);
        goto block_801963BC;  /* not taken */
    }

block_801963BC:
    /* delay slot (always executes) */
    /* nop */  /* 0x801963C0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801963D4u);
    goto block_801963D4;  /* j */

block_801963C4:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4]);
    /* nop */  /* 0x801963C8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0007;  /* 0x801963CC: 0x30420007 */
    cpu->write_byte(cpu->gpr[29] + 16, (uint8_t)cpu->gpr[2]);
    /* fall through to block_801963D4 */

block_801963D4:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4]);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[29] + 16);
    cpu->gpr[2] = cpu->gpr[2] & 0x0007;  /* 0x801963DC: 0x30420007 */
    int _bc_801963E0 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[0];
    if (_bc_801963E0) {
        psx_check_interrupts_at(cpu, 0x801963C4u);
        goto block_801963C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801963E8u);
        goto block_801963E8;  /* not taken */
    }

block_801963E8:
    cpu->gpr[4] = cpu->gpr[29] + 24;
    /* fall through to block_801963EC */

block_801963EC:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13556);
    /* nop */  /* 0x801963F4: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2]);
    /* nop */  /* 0x801963FC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0020;  /* 0x80196400: 0x30420020 */
    int _bc_80196404 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[4] + cpu->gpr[16];
    if (_bc_80196404) {
        psx_check_interrupts_at(cpu, 0x80196430u);
        goto block_80196430;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019640Cu);
        goto block_8019640C;  /* not taken */
    }

block_8019640C:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13552);
    /* nop */  /* 0x80196414: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2]);
    cpu->gpr[16] = cpu->gpr[16] + 1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = ((int32_t)cpu->gpr[16] < 8) ? 1 : 0;  /* 0x80196424: 0x2A020008 */
    int _bc_80196428 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019642C: 0x00000000 */
    if (_bc_80196428) {
        psx_check_interrupts_at(cpu, 0x801963ECu);
        goto block_801963EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196430u);
        goto block_80196430;  /* not taken */
    }

block_80196430:
    cpu->gpr[2] = ((int32_t)cpu->gpr[16] < 8) ? 1 : 0;  /* 0x80196430: 0x2A020008 */
    int _bc_80196434 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[16];
    if (_bc_80196434) {
        psx_check_interrupts_at(cpu, 0x80196458u);
        goto block_80196458;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019643Cu);
        goto block_8019643C;  /* not taken */
    }

block_8019643C:
    cpu->gpr[4] = cpu->gpr[29] + 24;
    cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[3];
    /* fall through to block_80196444 */

block_80196444:
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[0]);
    cpu->gpr[3] = cpu->gpr[3] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 8) ? 1 : 0;  /* 0x8019644C: 0x28620008 */
    int _bc_80196450 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[3];
    if (_bc_80196450) {
        psx_check_interrupts_at(cpu, 0x80196444u);
        goto block_80196444;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196458u);
        goto block_80196458;  /* not taken */
    }

block_80196458:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13556);
    cpu->gpr[2] = 1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13544);
    cpu->gpr[3] = 7;
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[3]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13548);
    /* nop */  /* 0x80196480: 0x00000000 */
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[29] + 16);
    cpu->gpr[2] = 3;
    int _bc_80196490 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80196494: 0x00000000 */
    if (_bc_80196490) {
        psx_check_interrupts_at(cpu, 0x801964C0u);
        goto block_801964C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196498u);
        goto block_80196498;  /* not taken */
    }

block_80196498:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + -14235);
    /* nop */  /* 0x801964A0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[1] = cpu->gpr[1] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[1] + -13812);
    /* nop */  /* 0x801964B4: 0x00000000 */
    int _bc_801964B8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801964BC: 0x00000000 */
    if (_bc_801964B8) {
        psx_check_interrupts_at(cpu, 0x80196524u);
        goto block_80196524;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801964C0u);
        goto block_801964C0;  /* not taken */
    }

block_801964C0:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14252);
    /* nop */  /* 0x801964C8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x801964CC: 0x30420010 */
    int _bc_801964D0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801964D4: 0x00000000 */
    if (_bc_801964D0) {
        psx_check_interrupts_at(cpu, 0x80196504u);
        goto block_80196504;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801964D8u);
        goto block_801964D8;  /* not taken */
    }

block_801964D8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[29] + 24);
    /* nop */  /* 0x801964DC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x801964E0: 0x30420010 */
    int _bc_801964E4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801964E8: 0x00000000 */
    if (_bc_801964E4) {
        psx_check_interrupts_at(cpu, 0x80196504u);
        goto block_80196504;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801964ECu);
        goto block_801964EC;  /* not taken */
    }

block_801964EC:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14244);
    /* nop */  /* 0x801964F4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -14244, cpu->gpr[2]);
    /* fall through to block_80196504 */

block_80196504:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[29] + 24);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[29] + 25);
    cpu->gpr[2] = cpu->gpr[2] & 0x00FF;  /* 0x8019650C: 0x304200FF */
    cpu->gpr[17] = cpu->gpr[2] & 0x001D;  /* 0x80196510: 0x3051001D */
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -14252, cpu->gpr[2]);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -14248, cpu->gpr[3]);
    /* fall through to block_80196524 */

block_80196524:
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[29] + 16);
    cpu->gpr[2] = 5;
    int _bc_8019652C = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80196530: 0x00000000 */
    if (_bc_8019652C) {
        psx_check_interrupts_at(cpu, 0x8019659Cu);
        goto block_8019659C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196534u);
        goto block_80196534;  /* not taken */
    }

block_80196534:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14256);
    /* nop */  /* 0x8019653C: 0x00000000 */
    int _bc_80196540 = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80196544: 0x00000000 */
    if (_bc_80196540) {
        psx_check_interrupts_at(cpu, 0x8019659Cu);
        goto block_8019659C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196548u);
        goto block_80196548;  /* not taken */
    }

block_80196548:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x80196554u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22480;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_80196554:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14256);
    /* nop */  /* 0x8019655C: 0x00000000 */
    int _bc_80196560 = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80196564: 0x00000000 */
    if (_bc_80196560) {
        psx_check_interrupts_at(cpu, 0x8019659Cu);
        goto block_8019659C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196568u);
        goto block_80196568;  /* not taken */
    }

block_80196568:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + -14235);
    cpu->gpr[6] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[6] = cpu->read_word(cpu->gpr[6] + -14252);
    cpu->gpr[7] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[7] = cpu->read_word(cpu->gpr[7] + -14248);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[5] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14228);
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x8019659Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22492;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_8019659C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[29] + 16);
    /* nop */  /* 0x801965A0: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] + -1;
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)5) ? 1 : 0;  /* 0x801965A8: 0x2C620005 */
    int _bc_801965AC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    if (_bc_801965AC) {
        psx_check_interrupts_at(cpu, 0x80196898u);
        goto block_80196898;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801965B4u);
        goto block_801965B4;  /* not taken */
    }

block_801965B4:
    cpu->gpr[1] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[1] = cpu->gpr[1] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[1] + 22552);
    /* nop */  /* 0x801965C0: 0x00000000 */
    uint32_t _jt_801965C4 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801965C8: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801965C4);
    cpu->pc = _jt_801965C4; return;  /* CPS: jr cpu->gpr[2] */

block_801965CC:
    int _bc_801965CC = (cpu->gpr[17] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 5;
    if (_bc_801965CC) {
        psx_check_interrupts_at(cpu, 0x80196618u);
        goto block_80196618;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801965D4u);
        goto block_801965D4;  /* not taken */
    }

block_801965D4:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->gpr[3] + -13532;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -14648;
    int _bc_801965E8 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 24;
    if (_bc_801965E8) {
        psx_check_interrupts_at(cpu, 0x80196710u);
        goto block_80196710;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801965F0u);
        goto block_801965F0;  /* not taken */
    }

block_801965F0:
    cpu->gpr[4] = 7;
    cpu->gpr[6] = -1;
    /* fall through to block_801965F8 */

block_801965F8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    int _bc_80196608 = (cpu->gpr[4] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 1;
    if (_bc_80196608) {
        psx_check_interrupts_at(cpu, 0x801965F8u);
        goto block_801965F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196610u);
        goto block_80196610;  /* not taken */
    }

block_80196610:
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196618:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + -14235);
    /* nop */  /* 0x80196620: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[1] = cpu->gpr[1] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[1] + -14068);
    /* nop */  /* 0x80196634: 0x00000000 */
    int _bc_80196638 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 3;
    if (_bc_80196638) {
        psx_check_interrupts_at(cpu, 0x80196684u);
        goto block_80196684;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196640u);
        goto block_80196640;  /* not taken */
    }

block_80196640:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->gpr[3] + -13532;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -14648;
    int _bc_80196654 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 24;
    if (_bc_80196654) {
        psx_check_interrupts_at(cpu, 0x8019667Cu);
        goto block_8019667C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019665Cu);
        goto block_8019665C;  /* not taken */
    }

block_8019665C:
    cpu->gpr[4] = 7;
    cpu->gpr[6] = -1;
    /* fall through to block_80196664 */

block_80196664:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    int _bc_80196674 = (cpu->gpr[4] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 1;
    if (_bc_80196674) {
        psx_check_interrupts_at(cpu, 0x80196664u);
        goto block_80196664;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019667Cu);
        goto block_8019667C;  /* not taken */
    }

block_8019667C:
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196684:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->gpr[3] + -13532;
    cpu->gpr[2] = 2;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -14648;
    int _bc_8019669C = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 24;
    if (_bc_8019669C) {
        psx_check_interrupts_at(cpu, 0x80196710u);
        goto block_80196710;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966A4u);
        goto block_801966A4;  /* not taken */
    }

block_801966A4:
    cpu->gpr[4] = 7;
    cpu->gpr[6] = -1;
    /* fall through to block_801966AC */

block_801966AC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    int _bc_801966BC = (cpu->gpr[4] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 1;
    if (_bc_801966BC) {
        psx_check_interrupts_at(cpu, 0x801966ACu);
        goto block_801966AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966C4u);
        goto block_801966C4;  /* not taken */
    }

block_801966C4:
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_801966CC:
    int _bc_801966CC = (cpu->gpr[17] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_801966CC) {
        psx_check_interrupts_at(cpu, 0x801966D8u);
        goto block_801966D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966D4u);
        goto block_801966D4;  /* not taken */
    }

block_801966D4:
    cpu->gpr[2] = 5;
    /* fall through to block_801966D8 */

block_801966D8:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_byte(cpu->gpr[1] + -13532, (uint8_t)cpu->gpr[2]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -14648;
    int _bc_801966E8 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 24;
    if (_bc_801966E8) {
        psx_check_interrupts_at(cpu, 0x80196710u);
        goto block_80196710;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801966F0u);
        goto block_801966F0;  /* not taken */
    }

block_801966F0:
    cpu->gpr[4] = 7;
    cpu->gpr[6] = -1;
    /* fall through to block_801966F8 */

block_801966F8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    int _bc_80196708 = (cpu->gpr[4] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 1;
    if (_bc_80196708) {
        psx_check_interrupts_at(cpu, 0x801966F8u);
        goto block_801966F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196710u);
        goto block_80196710;  /* not taken */
    }

block_80196710:
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196718:
    int _bc_80196718 = (cpu->gpr[17] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_80196718) {
        psx_check_interrupts_at(cpu, 0x8019672Cu);
        goto block_8019672C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196720u);
        goto block_80196720;  /* not taken */
    }

block_80196720:
    int _bc_80196720 = (cpu->gpr[16] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80196724: 0x00000000 */
    if (_bc_80196720) {
        psx_check_interrupts_at(cpu, 0x8019672Cu);
        goto block_8019672C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196728u);
        goto block_80196728;  /* not taken */
    }

block_80196728:
    cpu->gpr[17] = cpu->gpr[0];
    /* fall through to block_8019672C */

block_8019672C:
    int _bc_8019672C = (cpu->gpr[17] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 1;
    if (_bc_8019672C) {
        psx_check_interrupts_at(cpu, 0x80196738u);
        goto block_80196738;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196734u);
        goto block_80196734;  /* not taken */
    }

block_80196734:
    cpu->gpr[3] = 5;
    /* fall through to block_80196738 */

block_80196738:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->gpr[2] + -13532;
    cpu->write_byte(cpu->gpr[2] + 1, (uint8_t)cpu->gpr[3]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -14640;
    int _bc_8019674C = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 24;
    if (_bc_8019674C) {
        psx_check_interrupts_at(cpu, 0x80196774u);
        goto block_80196774;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196754u);
        goto block_80196754;  /* not taken */
    }

block_80196754:
    cpu->gpr[4] = 7;
    cpu->gpr[6] = -1;
    /* fall through to block_8019675C */

block_8019675C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    int _bc_8019676C = (cpu->gpr[4] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 1;
    if (_bc_8019676C) {
        psx_check_interrupts_at(cpu, 0x8019675Cu);
        goto block_8019675C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196774u);
        goto block_80196774;  /* not taken */
    }

block_80196774:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13556);
    /* nop */  /* 0x8019677C: 0x00000000 */
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[0]);
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13544);
    cpu->gpr[2] = 4;
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196798:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->gpr[4] + -14632;
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->gpr[2] + -13532;
    cpu->gpr[3] = 4;
    cpu->write_byte(cpu->gpr[2] + 2, (uint8_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[2] + 2);
    cpu->gpr[5] = cpu->gpr[29] + 24;
    cpu->write_byte(cpu->gpr[2] + 1, (uint8_t)cpu->gpr[3]);
    int _bc_801967BC = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 7;
    if (_bc_801967BC) {
        psx_check_interrupts_at(cpu, 0x801967E0u);
        goto block_801967E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801967C4u);
        goto block_801967C4;  /* not taken */
    }

block_801967C4:
    cpu->gpr[6] = -1;
    /* fall through to block_801967C8 */

block_801967C8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[3] = cpu->gpr[3] + -1;
    cpu->write_byte(cpu->gpr[4], (uint8_t)cpu->gpr[2]);
    int _bc_801967D8 = (cpu->gpr[3] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1;
    if (_bc_801967D8) {
        psx_check_interrupts_at(cpu, 0x801967C8u);
        goto block_801967C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801967E0u);
        goto block_801967E0;  /* not taken */
    }

block_801967E0:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -14640;
    int _bc_801967E8 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 24;
    if (_bc_801967E8) {
        psx_check_interrupts_at(cpu, 0x80196810u);
        goto block_80196810;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801967F0u);
        goto block_801967F0;  /* not taken */
    }

block_801967F0:
    cpu->gpr[4] = 7;
    cpu->gpr[6] = -1;
    /* fall through to block_801967F8 */

block_801967F8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    int _bc_80196808 = (cpu->gpr[4] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 1;
    if (_bc_80196808) {
        psx_check_interrupts_at(cpu, 0x801967F8u);
        goto block_801967F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196810u);
        goto block_80196810;  /* not taken */
    }

block_80196810:
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196818:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->gpr[4] + -14648;
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->gpr[2] + -13532;
    cpu->gpr[3] = 5;
    cpu->write_byte(cpu->gpr[2] + 1, (uint8_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[2] + 1);
    cpu->gpr[5] = cpu->gpr[29] + 24;
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[3]);
    int _bc_8019683C = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 7;
    if (_bc_8019683C) {
        psx_check_interrupts_at(cpu, 0x80196860u);
        goto block_80196860;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196844u);
        goto block_80196844;  /* not taken */
    }

block_80196844:
    cpu->gpr[6] = -1;
    /* fall through to block_80196848 */

block_80196848:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[3] = cpu->gpr[3] + -1;
    cpu->write_byte(cpu->gpr[4], (uint8_t)cpu->gpr[2]);
    int _bc_80196858 = (cpu->gpr[3] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1;
    if (_bc_80196858) {
        psx_check_interrupts_at(cpu, 0x80196848u);
        goto block_80196848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196860u);
        goto block_80196860;  /* not taken */
    }

block_80196860:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -14640;
    int _bc_80196868 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 24;
    if (_bc_80196868) {
        psx_check_interrupts_at(cpu, 0x80196890u);
        goto block_80196890;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196870u);
        goto block_80196870;  /* not taken */
    }

block_80196870:
    cpu->gpr[4] = 7;
    cpu->gpr[6] = -1;
    /* fall through to block_80196878 */

block_80196878:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    int _bc_80196888 = (cpu->gpr[4] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 1;
    if (_bc_80196888) {
        psx_check_interrupts_at(cpu, 0x80196878u);
        goto block_80196878;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196890u);
        goto block_80196890;  /* not taken */
    }

block_80196890:
    /* delay slot (always executes) */
    cpu->gpr[2] = 6;
    psx_check_interrupts_at(cpu, 0x801968B8u);
    goto block_801968B8;  /* j */

block_80196898:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x801968A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22520;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u; return;  /* CPS jal -> 0x80197AD0 */

block_801968A4:
    cpu->gpr[5] = cpu->read_byte(cpu->gpr[29] + 16);
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x801968B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22540;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_801968B4:
    cpu->gpr[2] = cpu->gpr[0];
    /* fall through to block_801968B8 */

block_801968B8:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 32);
    uint32_t _jt_801968C4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 48;
    psx_check_interrupts_at(cpu, _jt_801968C4);
    cpu->pc = _jt_801968C4; return;  /* CPS: jr $ra */
}
