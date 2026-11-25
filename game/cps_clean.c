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

/* MIPS unaligned word accessors — faithful to psx_lwl/lwr/swl/swr but on the
 * MMIO-aware cpu read/write accessor path (no per-access cycle charge). */
static inline uint32_t br2_lwl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return (rt_value & 0x00FFFFFFu) | (word << 24);
        case 1: return (rt_value & 0x0000FFFFu) | (word << 16);
        case 2: return (rt_value & 0x000000FFu) | (word << 8);
        default: return word;
    }
}
static inline uint32_t br2_lwr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return word;
        case 1: return (rt_value & 0xFF000000u) | (word >> 8);
        case 2: return (rt_value & 0xFFFF0000u) | (word >> 16);
        default: return (rt_value & 0xFFFFFF00u) | (word >> 24);
    }
}
static inline void br2_swl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = (word & 0xFFFFFF00u) | (rt_value >> 24); break;
        case 1: word = (word & 0xFFFF0000u) | (rt_value >> 16); break;
        case 2: word = (word & 0xFF000000u) | (rt_value >> 8); break;
        default: word = rt_value; break;
    }
    cpu->write_word(aligned, word);
}
static inline void br2_swr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = rt_value; break;
        case 1: word = (word & 0x000000FFu) | (rt_value << 8); break;
        case 2: word = (word & 0x0000FFFFu) | (rt_value << 16); break;
        default: word = (word & 0x00FFFFFFu) | (rt_value << 24); break;
    }
    cpu->write_word(aligned, word);
}
void cps_80196370(void);
void cps_80199440(void);
void cps_80199aa8(void);
void cps_80199ab0(void);
void cps_80199aec(void);
void cps_80199afc(void);
void cps_80199b10(void);
void cps_80199b30(void);
void cps_80199b74(void);
void cps_80199b9c(void);
void cps_80199bb8(void);
void cps_80199bc8(void);
void cps_80199c0c(void);
void cps_80199c30(void);
void cps_80199c54(void);
void cps_80199c78(void);
void cps_80199c94(void);
void cps_80199ccc(void);
void cps_80199cf0(void);
void cps_80199d4c(void);
void cps_80198e2c(void);
void cps_801985c4(void);
void cps_8018d300(void);
void cps_8018f7e0(void);
void cps_8018e490(void);
void cps_8018eca0(void);
void cps_80194db0(void);
void cps_8018da2c(void);
void cps_8018dac0(void);
void cps_8018b050(void);
void cps_8018d4fc(void);
void cps_8018d484(void);
void cps_8018b0a0(void);
void cps_8018de6c(void);
void cps_8018df00(void);
void cps_8018e028(void);
void cps_80133aa0(void);

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

/* cps_80199440 — clean CPS transliteration of br2eu_func_80199440 (guest 0x80199440)
 * from SLES_017.22_full_46.c. */
void cps_80199440(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801994CCu: goto block_801994CC;
            case 0x8019966Cu: goto block_8019966C;
            case 0x80199698u: goto block_80199698;
            case 0x8019986Cu: goto block_8019986C;
            case 0x80199938u: goto block_80199938;
            case 0x80199974u: goto block_80199974;
            case 0x80199A18u: goto block_80199A18;
            case 0x80199458u: goto block_80199458;
            case 0x8019946Cu: goto block_8019946C;
            case 0x8019948Cu: goto block_8019948C;
            case 0x801994A0u: goto block_801994A0;
            case 0x801994B8u: goto block_801994B8;
            case 0x801994C4u: goto block_801994C4;
            case 0x801994D8u: goto block_801994D8;
            case 0x801994FCu: goto block_801994FC;
            case 0x80199508u: goto block_80199508;
            case 0x80199538u: goto block_80199538;
            case 0x8019954Cu: goto block_8019954C;
            case 0x80199568u: goto block_80199568;
            case 0x80199574u: goto block_80199574;
            case 0x801995E8u: goto block_801995E8;
            case 0x801995ECu: goto block_801995EC;
            case 0x80199610u: goto block_80199610;
            case 0x80199620u: goto block_80199620;
            case 0x80199634u: goto block_80199634;
            case 0x80199648u: goto block_80199648;
            case 0x80199674u: goto block_80199674;
            case 0x801996B8u: goto block_801996B8;
            case 0x801996BCu: goto block_801996BC;
            case 0x801996D0u: goto block_801996D0;
            case 0x80199724u: goto block_80199724;
            case 0x80199738u: goto block_80199738;
            case 0x80199754u: goto block_80199754;
            case 0x8019976Cu: goto block_8019976C;
            case 0x80199788u: goto block_80199788;
            case 0x80199790u: goto block_80199790;
            case 0x801997ACu: goto block_801997AC;
            case 0x801997C8u: goto block_801997C8;
            case 0x801997DCu: goto block_801997DC;
            case 0x801997E8u: goto block_801997E8;
            case 0x801997ECu: goto block_801997EC;
            case 0x80199808u: goto block_80199808;
            case 0x80199820u: goto block_80199820;
            case 0x80199834u: goto block_80199834;
            case 0x80199844u: goto block_80199844;
            case 0x8019989Cu: goto block_8019989C;
            case 0x801998B8u: goto block_801998B8;
            case 0x801998C4u: goto block_801998C4;
            case 0x801998E0u: goto block_801998E0;
            case 0x80199908u: goto block_80199908;
            case 0x80199910u: goto block_80199910;
            case 0x8019996Cu: goto block_8019996C;
            case 0x80199988u: goto block_80199988;
            case 0x801999A4u: goto block_801999A4;
            case 0x801999B0u: goto block_801999B0;
            case 0x801999E0u: goto block_801999E0;
            case 0x801999F4u: goto block_801999F4;
            case 0x80199A10u: goto block_80199A10;
            case 0x80199A2Cu: goto block_80199A2C;
            case 0x80199A48u: goto block_80199A48;
            case 0x80199A54u: goto block_80199A54;
            case 0x80199A70u: goto block_80199A70;
            case 0x80199A88u: goto block_80199A88;
            case 0x80199AA4u: goto block_80199AA4;
            default: break;
        }
    }
    /* Address: 0x80199440, Size: 1640 bytes, Blocks: 65 */

block_80199440:
    cpu->gpr[29] = cpu->gpr[29] + -64;
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14692);
    cpu->gpr[4] = 1;
    int _bc_80199450 = (cpu->gpr[2] == cpu->gpr[4]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[31]);
    if (_bc_80199450) {
        psx_check_interrupts_at(cpu, 0x80199D4Cu);
        cpu->pc = 0x80199D4Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199458u);
        goto block_80199458;  /* not taken */
    }

block_80199458:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14712);
    /* nop */  /* 0x80199460: 0x00000000 */
    int _bc_80199464 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199468: 0x00000000 */
    if (_bc_80199464) {
        psx_check_interrupts_at(cpu, 0x801994C4u);
        goto block_801994C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019946Cu);
        goto block_8019946C;  /* not taken */
    }

block_8019946C:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13216);
    /* nop */  /* 0x80199474: 0x00000000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[3] = 0x0100 << 16;  /* 0x01000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x80199480: 0x00431024 */
    int _bc_80199484 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199488: 0x00000000 */
    if (_bc_80199484) {
        psx_check_interrupts_at(cpu, 0x801994C4u);
        goto block_801994C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019948Cu);
        goto block_8019948C;  /* not taken */
    }

block_8019948C:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    int _bc_80199498 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14708, cpu->gpr[4]);
    if (_bc_80199498) {
        psx_check_interrupts_at(cpu, 0x801994B8u);
        goto block_801994B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801994A0u);
        goto block_801994A0;  /* not taken */
    }

block_801994A0:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x801994A8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    /* fall through to block_801994B8 */

block_801994B8:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[4]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_801994C4:
    cpu->gpr[31] = 0x801994CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 48;
    psx_check_interrupts_at(cpu, 0x80195D44u);
    cpu->pc = 0x80195D44u; return;  /* CPS jal -> 0x80195D44 */

block_801994CC:
    cpu->gpr[3] = 5;
    int _bc_801994D0 = (cpu->gpr[2] == cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801994D4: 0x00000000 */
    if (_bc_801994D0) {
        psx_check_interrupts_at(cpu, 0x80199D4Cu);
        cpu->pc = 0x80199D4Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x801994D8u);
        goto block_801994D8;  /* not taken */
    }

block_801994D8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[29] + 48);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[29] + 49);
    cpu->write_half(cpu->gpr[29] + 34, (uint16_t)cpu->gpr[2]);
    cpu->write_half(cpu->gpr[29] + 36, (uint16_t)cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[29] + 34);
    /* nop */  /* 0x801994EC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0004;  /* 0x801994F0: 0x30420004 */
    int _bc_801994F4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 3;
    if (_bc_801994F4) {
        psx_check_interrupts_at(cpu, 0x80199508u);
        goto block_80199508;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801994FCu);
        goto block_801994FC;  /* not taken */
    }

block_801994FC:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_80199508:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14684);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14656);
    cpu->gpr[2] = cpu->gpr[2] << 5;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -5352, cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3]);
    /* nop */  /* 0x8019952C: 0x00000000 */
    int _bc_80199530 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199534: 0x00000000 */
    if (_bc_80199530) {
        psx_check_interrupts_at(cpu, 0x80199574u);
        goto block_80199574;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199538u);
        goto block_80199538;  /* not taken */
    }

block_80199538:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x80199540: 0x00000000 */
    int _bc_80199544 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    if (_bc_80199544) {
        psx_check_interrupts_at(cpu, 0x80199568u);
        goto block_80199568;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019954Cu);
        goto block_8019954C;  /* not taken */
    }

block_8019954C:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x80199554: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    cpu->gpr[2] = 4;
    /* fall through to block_80199568 */

block_80199568:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_80199574:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13248);
    /* nop */  /* 0x8019957C: 0x00000000 */
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[0]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13236);
    /* nop */  /* 0x8019958C: 0x00000000 */
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[0]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13248);
    cpu->gpr[4] = 0x0002 << 16;  /* 0x00020000 */
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[0]);
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13236);
    cpu->gpr[2] = 128;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13232);
    cpu->gpr[4] = cpu->gpr[4] | 0x0943;
    cpu->write_word(cpu->gpr[2], cpu->gpr[4]);
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13228);
    cpu->gpr[2] = 4899;
    cpu->write_word(cpu->gpr[3], cpu->gpr[2]);
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14600);
    /* nop */  /* 0x801995DC: 0x00000000 */
    int _bc_801995E0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    if (_bc_801995E0) {
        psx_check_interrupts_at(cpu, 0x80199634u);
        goto block_80199634;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801995E8u);
        goto block_801995E8;  /* not taken */
    }

block_801995E8:
    cpu->gpr[5] = cpu->gpr[29] + 40;
    /* fall through to block_801995EC */

block_801995EC:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13240);
    cpu->gpr[3] = cpu->gpr[5] + cpu->gpr[4];
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2]);
    cpu->gpr[4] = cpu->gpr[4] + 1;
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)4) ? 1 : 0;  /* 0x80199604: 0x2C820004 */
    int _bc_80199608 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019960C: 0x00000000 */
    if (_bc_80199608) {
        psx_check_interrupts_at(cpu, 0x801995ECu);
        goto block_801995EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199610u);
        goto block_80199610;  /* not taken */
    }

block_80199610:
    cpu->gpr[4] = cpu->gpr[0];
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13240);
    /* nop */  /* 0x8019961C: 0x00000000 */
    /* fall through to block_80199620 */

block_80199620:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[3]);
    cpu->gpr[4] = cpu->gpr[4] + 1;
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80199628: 0x2C820008 */
    int _bc_8019962C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199630: 0x00000000 */
    if (_bc_8019962C) {
        psx_check_interrupts_at(cpu, 0x80199620u);
        goto block_80199620;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199634u);
        goto block_80199634;  /* not taken */
    }

block_80199634:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x8019963C: 0x00000000 */
    int _bc_80199640 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = 0x1100 << 16;  /* 0x11000000 */
    if (_bc_80199640) {
        psx_check_interrupts_at(cpu, 0x80199674u);
        goto block_80199674;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199648u);
        goto block_80199648;  /* not taken */
    }

block_80199648:
    cpu->gpr[6] = 8;
    cpu->gpr[7] = cpu->gpr[0];
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14688);
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -5352);
    cpu->gpr[5] = cpu->gpr[5] << 11;
    cpu->gpr[31] = 0x8019966Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[2] + cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x80199D5Cu);
    cpu->pc = 0x80199D5Cu; return;  /* CPS jal -> 0x80199D5C */

block_8019966C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80199670: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80199698u);
    goto block_80199698;  /* j */

block_80199674:
    cpu->gpr[4] = 3;
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -5352);
    cpu->gpr[7] = 8;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[8]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[0]);
    cpu->gpr[31] = 0x80199698u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80199D88u);
    cpu->pc = 0x80199D88u; return;  /* CPS jal -> 0x80199D88 */

block_80199698:
    cpu->gpr[4] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -13200);
    /* nop */  /* 0x801996A0: 0x00000000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4]);
    cpu->gpr[3] = 0x0100 << 16;  /* 0x01000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x801996AC: 0x00431024 */
    int _bc_801996B0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[4];
    if (_bc_801996B0) {
        psx_check_interrupts_at(cpu, 0x801996D0u);
        goto block_801996D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801996B8u);
        goto block_801996B8;  /* not taken */
    }

block_801996B8:
    cpu->gpr[4] = 0x0100 << 16;  /* 0x01000000 */
    /* fall through to block_801996BC */

block_801996BC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x801996C0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x801996C4: 0x00441024 */
    int _bc_801996C8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801996CC: 0x00000000 */
    if (_bc_801996C8) {
        psx_check_interrupts_at(cpu, 0x801996BCu);
        goto block_801996BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801996D0u);
        goto block_801996D0;  /* not taken */
    }

block_801996D0:
    cpu->gpr[4] = 0x0002 << 16;  /* 0x00020000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0843;
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -5352);
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13232);
    cpu->gpr[5] = br2_lwl(cpu, cpu->gpr[29] + 43, cpu->gpr[5]);
    cpu->gpr[5] = br2_lwr(cpu, cpu->gpr[29] + 40, cpu->gpr[5]);
    /* nop */  /* 0x801996F0: 0x00000000 */
    cpu->write_word(cpu->gpr[3], cpu->gpr[4]);
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13228);
    cpu->gpr[2] = 4901;
    cpu->write_word(cpu->gpr[3], cpu->gpr[2]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14664);
    cpu->gpr[2] = 1;
    int _bc_8019971C = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199720: 0x00000000 */
    if (_bc_8019971C) {
        psx_check_interrupts_at(cpu, 0x80199790u);
        goto block_80199790;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199724u);
        goto block_80199724;  /* not taken */
    }

block_80199724:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -14700);
    /* nop */  /* 0x8019972C: 0x00000000 */
    int _bc_80199730 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199734: 0x00000000 */
    if (_bc_80199730) {
        psx_check_interrupts_at(cpu, 0x80199790u);
        goto block_80199790;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199738u);
        goto block_80199738;  /* not taken */
    }

block_80199738:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5352);
    /* nop */  /* 0x80199740: 0x00000000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3] + 8);
    /* nop */  /* 0x80199748: 0x00000000 */
    int _bc_8019974C = (cpu->gpr[4] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199750: 0x00000000 */
    if (_bc_8019974C) {
        psx_check_interrupts_at(cpu, 0x80199788u);
        goto block_80199788;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199754u);
        goto block_80199754;  /* not taken */
    }

block_80199754:
    cpu->write_half(cpu->gpr[3], (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x80199760: 0x00000000 */
    int _bc_80199764 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199768: 0x00000000 */
    if (_bc_80199764) {
        psx_check_interrupts_at(cpu, 0x80199D4Cu);
        cpu->pc = 0x80199D4Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8019976Cu);
        goto block_8019976C;  /* not taken */
    }

block_8019976C:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x80199774: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_80199788:
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14664, cpu->gpr[0]);
    /* fall through to block_80199790 */

block_80199790:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -5352);
    /* nop */  /* 0x80199798: 0x00000000 */
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4]);
    cpu->gpr[2] = 352;
    int _bc_801997A4 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801997A8: 0x00000000 */
    if (_bc_801997A4) {
        psx_check_interrupts_at(cpu, 0x801997C8u);
        goto block_801997C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801997ACu);
        goto block_801997AC;  /* not taken */
    }

block_801997AC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 2);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14696);
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 10;
    cpu->gpr[2] = cpu->gpr[2] & 0x001F;  /* 0x801997BC: 0x3042001F */
    int _bc_801997C0 = (cpu->gpr[2] == cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801997C4: 0x00000000 */
    if (_bc_801997C0) {
        psx_check_interrupts_at(cpu, 0x80199808u);
        goto block_80199808;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801997C8u);
        goto block_801997C8;  /* not taken */
    }

block_801997C8:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x801997D0: 0x00000000 */
    int _bc_801997D4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801997D8: 0x00000000 */
    if (_bc_801997D4) {
        psx_check_interrupts_at(cpu, 0x801997E8u);
        goto block_801997E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801997DCu);
        goto block_801997DC;  /* not taken */
    }

block_801997DC:
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x801997ECu);
    goto block_801997EC;  /* j */

block_801997E8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4]);
    /* fall through to block_801997EC */

block_801997EC:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5352);
    cpu->gpr[2] = 5;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[3], (uint16_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_80199808:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[3] + -14716);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 4);
    /* nop */  /* 0x80199814: 0x00000000 */
    int _bc_80199818 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019981C: 0x00000000 */
    if (_bc_80199818) {
        psx_check_interrupts_at(cpu, 0x80199844u);
        goto block_80199844;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199820u);
        goto block_80199820;  /* not taken */
    }

block_80199820:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14720);
    /* nop */  /* 0x80199828: 0x00000000 */
    int _bc_8019982C = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199830: 0x00000000 */
    if (_bc_8019982C) {
        psx_check_interrupts_at(cpu, 0x801998C4u);
        goto block_801998C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199834u);
        goto block_80199834;  /* not taken */
    }

block_80199834:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 8);
    /* nop */  /* 0x80199838: 0x00000000 */
    int _bc_8019983C = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199840: 0x00000000 */
    if (_bc_8019983C) {
        psx_check_interrupts_at(cpu, 0x801998C4u);
        goto block_801998C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199844u);
        goto block_80199844;  /* not taken */
    }

block_80199844:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -14680);
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14684);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14720, cpu->gpr[0]);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_half(cpu->gpr[1] + -14716, (uint16_t)cpu->gpr[0]);
    cpu->gpr[31] = 0x8019986Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] - cpu->gpr[4];
    psx_check_interrupts_at(cpu, 0x80199320u);
    cpu->pc = 0x80199320u; return;  /* CPS jal -> 0x80199320 */

block_8019986C:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14680);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5352);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14684, cpu->gpr[2]);
    cpu->write_half(cpu->gpr[3], (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x80199890: 0x00000000 */
    int _bc_80199894 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 6;
    if (_bc_80199894) {
        psx_check_interrupts_at(cpu, 0x801998B8u);
        goto block_801998B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019989Cu);
        goto block_8019989C;  /* not taken */
    }

block_8019989C:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x801998A4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    cpu->gpr[2] = 6;
    /* fall through to block_801998B8 */

block_801998B8:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_801998C4:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5352);
    /* nop */  /* 0x801998CC: 0x00000000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3] + 4);
    /* nop */  /* 0x801998D4: 0x00000000 */
    int _bc_801998D8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 10;
    if (_bc_801998D8) {
        psx_check_interrupts_at(cpu, 0x80199B10u);
        cpu->pc = 0x80199B10u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x801998E0u);
        goto block_801998E0;  /* not taken */
    }

block_801998E0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3] + 8);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14668);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_half(cpu->gpr[1] + -14716, (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = cpu->gpr[2] & 0xFFFF;  /* 0x801998F4: 0x3042FFFF */
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14720, cpu->gpr[2]);
    int _bc_80199900 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (cpu->gpr[2] < cpu->gpr[3]) ? 1 : 0;  /* 0x80199904: 0x0043102B */
    if (_bc_80199900) {
        psx_check_interrupts_at(cpu, 0x801999B0u);
        goto block_801999B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199908u);
        goto block_80199908;  /* not taken */
    }

block_80199908:
    int _bc_80199908 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019990C: 0x00000000 */
    if (_bc_80199908) {
        psx_check_interrupts_at(cpu, 0x801999B0u);
        goto block_801999B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199910u);
        goto block_80199910;  /* not taken */
    }

block_80199910:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -14680);
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14684);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14720, cpu->gpr[0]);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_half(cpu->gpr[1] + -14716, (uint16_t)cpu->gpr[0]);
    cpu->gpr[31] = 0x80199938u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] - cpu->gpr[4];
    psx_check_interrupts_at(cpu, 0x80199320u);
    cpu->pc = 0x80199320u; return;  /* CPS jal -> 0x80199320 */

block_80199938:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14680);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5352);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14684, cpu->gpr[2]);
    cpu->write_half(cpu->gpr[3], (uint16_t)cpu->gpr[0]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5356);
    cpu->gpr[2] = 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    int _bc_80199964 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14664, cpu->gpr[2]);
    if (_bc_80199964) {
        psx_check_interrupts_at(cpu, 0x80199974u);
        goto block_80199974;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019996Cu);
        goto block_8019996C;  /* not taken */
    }

block_8019996C:
    uint32_t _jt_8019996C = cpu->gpr[3];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x80199974u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199970: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019996C);
    cpu->pc = _jt_8019996C; return;  /* CPS jalr */

block_80199974:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x8019997C: 0x00000000 */
    int _bc_80199980 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 7;
    if (_bc_80199980) {
        psx_check_interrupts_at(cpu, 0x801999A4u);
        goto block_801999A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199988u);
        goto block_80199988;  /* not taken */
    }

block_80199988:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x80199990: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    cpu->gpr[2] = 7;
    /* fall through to block_801999A4 */

block_801999A4:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_801999B0:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14652);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14684);
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -5352);
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4] + 6);
    cpu->gpr[2] = cpu->gpr[2] + -1;
    cpu->gpr[2] = (cpu->gpr[2] < cpu->gpr[3]) ? 1 : 0;  /* 0x801999D4: 0x0043102B */
    int _bc_801999D8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801999DC: 0x00000000 */
    if (_bc_801999D8) {
        psx_check_interrupts_at(cpu, 0x80199AFCu);
        cpu->pc = 0x80199AFCu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x801999E0u);
        goto block_801999E0;  /* not taken */
    }

block_801999E0:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14668);
    /* nop */  /* 0x801999E8: 0x00000000 */
    int _bc_801999EC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_801999EC) {
        psx_check_interrupts_at(cpu, 0x80199A54u);
        goto block_80199A54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801999F4u);
        goto block_801999F4;  /* not taken */
    }

block_801999F4:
    cpu->write_half(cpu->gpr[4], (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5356);
    cpu->gpr[2] = 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    int _bc_80199A08 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14664, cpu->gpr[2]);
    if (_bc_80199A08) {
        psx_check_interrupts_at(cpu, 0x80199A18u);
        goto block_80199A18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199A10u);
        goto block_80199A10;  /* not taken */
    }

block_80199A10:
    uint32_t _jt_80199A10 = cpu->gpr[3];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x80199A18u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199A14: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80199A10);
    cpu->pc = _jt_80199A10; return;  /* CPS jalr */

block_80199A18:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x80199A20: 0x00000000 */
    int _bc_80199A24 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 8;
    if (_bc_80199A24) {
        psx_check_interrupts_at(cpu, 0x80199A48u);
        goto block_80199A48;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199A2Cu);
        goto block_80199A2C;  /* not taken */
    }

block_80199A2C:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x80199A34: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    cpu->gpr[2] = 8;
    /* fall through to block_80199A48 */

block_80199A48:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */

block_80199A54:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14656);
    /* nop */  /* 0x80199A5C: 0x00000000 */
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2]);
    /* nop */  /* 0x80199A64: 0x00000000 */
    int _bc_80199A68 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_80199A68) {
        psx_check_interrupts_at(cpu, 0x80199AB0u);
        cpu->pc = 0x80199AB0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199A70u);
        goto block_80199A70;  /* not taken */
    }

block_80199A70:
    cpu->write_half(cpu->gpr[4], (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x80199A7C: 0x00000000 */
    int _bc_80199A80 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 9;
    if (_bc_80199A80) {
        psx_check_interrupts_at(cpu, 0x80199AA4u);
        goto block_80199AA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199A88u);
        goto block_80199A88;  /* not taken */
    }

block_80199A88:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x80199A90: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    cpu->gpr[2] = 9;
    /* fall through to block_80199AA4 */

block_80199AA4:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cps_80199aa8(); return;  /* fallthrough to split piece */
}

/* cps_80199aa8 — clean CPS transliteration of br2eu_func_80199AA8 (guest 0x80199aa8)
 * from SLES_017.22_full_46.c. */
void cps_80199aa8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199AA8, Size: 8 bytes, Blocks: 1 */

block_80199AA8:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199D4Cu);
    cpu->pc = 0x80199D4Cu; return;  /* CPS j: split */
    cps_80199ab0();  /* fallthrough to next function */
}

/* cps_80199ab0 — clean CPS transliteration of br2eu_func_80199AB0 (guest 0x80199ab0)
 * from SLES_017.22_full_46.c. */
void cps_80199ab0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80199AD0u: goto block_80199AD0;
            default: break;
        }
    }
    /* Address: 0x80199AB0, Size: 60 bytes, Blocks: 2 */

block_80199AB0:
    cpu->write_half(cpu->gpr[4], (uint16_t)cpu->gpr[2]);
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14656);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5352);
    cpu->gpr[4] = cpu->gpr[0];
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14684, cpu->gpr[0]);
    /* fall through to block_80199AD0 */

block_80199AD0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    cpu->gpr[3] = cpu->gpr[3] + 4;
    cpu->gpr[4] = cpu->gpr[4] + 1;
    cpu->write_word(cpu->gpr[5], cpu->gpr[2]);
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80199AE0: 0x2C820008 */
    int _bc_80199AE4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + 4;
    if (_bc_80199AE4) {
        psx_check_interrupts_at(cpu, 0x80199AD0u);
        goto block_80199AD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80199AECu);
        cpu->pc = 0x80199AECu; return;  /* CPS not taken: split */
    }
}

/* cps_80199aec — clean CPS transliteration of br2eu_func_80199AEC (guest 0x80199aec)
 * from SLES_017.22_full_46.c. */
void cps_80199aec(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199AEC, Size: 16 bytes, Blocks: 1 */

block_80199AEC:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14656);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -5352, cpu->gpr[2]);
    cps_80199afc(); return;  /* fallthrough to split piece */
    cps_80199afc();  /* fallthrough to next function */
}

/* cps_80199afc — clean CPS transliteration of br2eu_func_80199AFC (guest 0x80199afc)
 * from SLES_017.22_full_46.c. */
void cps_80199afc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199AFC, Size: 20 bytes, Blocks: 1 */

block_80199AFC:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14684);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14680, cpu->gpr[2]);
    cpu->gpr[2] = 10;
    cps_80199b10(); return;  /* fallthrough to split piece */
    cps_80199b10();  /* fallthrough to next function */
}

/* cps_80199b10 — clean CPS transliteration of br2eu_func_80199B10 (guest 0x80199b10)
 * from SLES_017.22_full_46.c. */
void cps_80199b10(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199B10, Size: 32 bytes, Blocks: 1 */

block_80199B10:
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -13176, cpu->gpr[2]);
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2] + -14716);
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -14652);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14656);
    cps_80199b30(); return;  /* fallthrough to split piece */
    cps_80199b30();  /* fallthrough to next function */
}

/* cps_80199b30 — clean CPS transliteration of br2eu_func_80199B30 (guest 0x80199b30)
 * from SLES_017.22_full_46.c. */
void cps_80199b30(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199B30, Size: 68 bytes, Blocks: 1 */

block_80199B30:
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14684);
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[4] = cpu->gpr[4] << 5;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_half(cpu->gpr[1] + -14716, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[5] << 6;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[5];
    cpu->gpr[2] = cpu->gpr[2] << 5;
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -14712);
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14660, cpu->gpr[3]);
    int _bc_80199B6C = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = 0x1100 << 16;  /* 0x11000000 */
    if (_bc_80199B6C) {
        psx_check_interrupts_at(cpu, 0x80199B9Cu);
        cpu->pc = 0x80199B9Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199B74u);
        cpu->pc = 0x80199B74u; return;  /* CPS not taken: split */
    }
    cps_80199b74();  /* fallthrough to next function */
}

/* cps_80199b74 — clean CPS transliteration of br2eu_func_80199B74 (guest 0x80199b74)
 * from SLES_017.22_full_46.c. */
void cps_80199b74(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199B74, Size: 40 bytes, Blocks: 1 */

block_80199B74:
    cpu->gpr[3] = 0x0002 << 16;  /* 0x00020000 */
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13232);
    cpu->gpr[3] = cpu->gpr[3] | 0x0943;
    cpu->write_word(cpu->gpr[2], cpu->gpr[3]);
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13228);
    cpu->gpr[2] = 4899;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[3], cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199BB8u);
    cpu->pc = 0x80199BB8u; return;  /* CPS j: split */
    cps_80199b9c();  /* fallthrough to next function */
}

/* cps_80199b9c — clean CPS transliteration of br2eu_func_80199B9C (guest 0x80199b9c)
 * from SLES_017.22_full_46.c. */
void cps_80199b9c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199B9C, Size: 28 bytes, Blocks: 1 */

block_80199B9C:
    cpu->gpr[3] = 0x2102 << 16;  /* 0x21020000 */
    cpu->gpr[3] = cpu->gpr[3] | 0x0843;
    cpu->gpr[8] = 0x1140 << 16;  /* 0x11400000 */
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13232);
    cpu->gpr[8] = cpu->gpr[8] | 0x0100;
    cpu->write_word(cpu->gpr[2], cpu->gpr[3]);
    cps_80199bb8(); return;  /* fallthrough to split piece */
    cps_80199bb8();  /* fallthrough to next function */
}

/* cps_80199bb8 — clean CPS transliteration of br2eu_func_80199BB8 (guest 0x80199bb8)
 * from SLES_017.22_full_46.c. */
void cps_80199bb8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199BB8, Size: 16 bytes, Blocks: 1 */

block_80199BB8:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -5352);
    /* nop */  /* 0x80199BC0: 0x00000000 */
    cpu->gpr[3] = cpu->read_half(cpu->gpr[2] + 6);
    cps_80199bc8(); return;  /* fallthrough to split piece */
    cps_80199bc8();  /* fallthrough to next function */
}

/* cps_80199bc8 — clean CPS transliteration of br2eu_func_80199BC8 (guest 0x80199bc8)
 * from SLES_017.22_full_46.c. */
void cps_80199bc8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80199BD8u: goto block_80199BD8;
            case 0x80199BF0u: goto block_80199BF0;
            default: break;
        }
    }
    /* Address: 0x80199BC8, Size: 68 bytes, Blocks: 3 */

block_80199BC8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2] + 4);
    cpu->gpr[3] = cpu->gpr[3] + -1;
    int _bc_80199BD0 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 1;
    if (_bc_80199BD0) {
        psx_check_interrupts_at(cpu, 0x80199C78u);
        cpu->pc = 0x80199C78u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199BD8u);
        goto block_80199BD8;  /* not taken */
    }

block_80199BD8:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14692, cpu->gpr[3]);
    int _bc_80199BE8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 504;
    if (_bc_80199BE8) {
        psx_check_interrupts_at(cpu, 0x80199C30u);
        cpu->pc = 0x80199C30u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199BF0u);
        goto block_80199BF0;  /* not taken */
    }

block_80199BF0:
    cpu->gpr[7] = 1;
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14688);
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -14660);
    cpu->gpr[5] = cpu->gpr[5] << 11;
    cpu->gpr[5] = cpu->gpr[2] + cpu->gpr[5];
    cps_80199c0c(); return;  /* fallthrough to split piece */
}

/* cps_80199c0c — clean CPS transliteration of br2eu_func_80199C0C (guest 0x80199c0c)
 * from SLES_017.22_full_46.c. */
void cps_80199c0c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80199C14u: goto block_80199C14;
            default: break;
        }
    }
    /* Address: 0x80199C0C, Size: 36 bytes, Blocks: 2 */

block_80199C0C:
    cpu->gpr[31] = 0x80199C14u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + 32;
    psx_check_interrupts_at(cpu, 0x80199D5Cu);
    cpu->pc = 0x80199D5Cu; return;  /* CPS jal -> 0x80199D5C */

block_80199C14:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x80199C1C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199C54u);
    cpu->pc = 0x80199C54u; return;  /* CPS j: split */
}

/* cps_80199c30 — clean CPS transliteration of br2eu_func_80199C30 (guest 0x80199c30)
 * from SLES_017.22_full_46.c. */
void cps_80199c30(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199C30, Size: 32 bytes, Blocks: 1 */

block_80199C30:
    cpu->gpr[4] = 3;
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14660);
    cpu->gpr[7] = 504;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[8]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[3]);
    cpu->gpr[31] = 0x80199C54u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80199D88u);
    cpu->pc = 0x80199D88u; return;  /* CPS jal -> 0x80199D88 */
}

/* cps_80199c54 — clean CPS transliteration of br2eu_func_80199C54 (guest 0x80199c54)
 * from SLES_017.22_full_46.c. */
void cps_80199c54(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199C54, Size: 36 bytes, Blocks: 1 */

block_80199C54:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14704);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_half(cpu->gpr[1] + -14716, (uint16_t)cpu->gpr[0]);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14720, cpu->gpr[0]);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14696, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199CF0u);
    cpu->pc = 0x80199CF0u; return;  /* CPS j: split */
    cps_80199c78();  /* fallthrough to next function */
}

/* cps_80199c78 — clean CPS transliteration of br2eu_func_80199C78 (guest 0x80199c78)
 * from SLES_017.22_full_46.c. */
void cps_80199c78(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80199C8Cu: goto block_80199C8C;
            default: break;
        }
    }
    /* Address: 0x80199C78, Size: 28 bytes, Blocks: 2 */

block_80199C78:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14672);
    /* nop */  /* 0x80199C80: 0x00000000 */
    int _bc_80199C84 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 504;
    if (_bc_80199C84) {
        psx_check_interrupts_at(cpu, 0x80199CCCu);
        cpu->pc = 0x80199CCCu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199C8Cu);
        goto block_80199C8C;  /* not taken */
    }

block_80199C8C:
    cpu->gpr[7] = cpu->gpr[0];
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cps_80199c94(); return;  /* fallthrough to split piece */
}

/* cps_80199c94 — clean CPS transliteration of br2eu_func_80199C94 (guest 0x80199c94)
 * from SLES_017.22_full_46.c. */
void cps_80199c94(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80199CB0u: goto block_80199CB0;
            default: break;
        }
    }
    /* Address: 0x80199C94, Size: 56 bytes, Blocks: 2 */

block_80199C94:
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14688);
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -14660);
    cpu->gpr[5] = cpu->gpr[5] << 11;
    cpu->gpr[5] = cpu->gpr[2] + cpu->gpr[5];
    cpu->gpr[31] = 0x80199CB0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + 32;
    psx_check_interrupts_at(cpu, 0x80199D5Cu);
    cpu->pc = 0x80199D5Cu; return;  /* CPS jal -> 0x80199D5C */

block_80199CB0:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14688);
    /* nop */  /* 0x80199CB8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14688, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80199CF0u);
    cpu->pc = 0x80199CF0u; return;  /* CPS j: split */
}

/* cps_80199ccc — clean CPS transliteration of br2eu_func_80199CCC (guest 0x80199ccc)
 * from SLES_017.22_full_46.c. */
void cps_80199ccc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199CCC, Size: 36 bytes, Blocks: 1 */

block_80199CCC:
    cpu->gpr[4] = 3;
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14660);
    cpu->gpr[7] = 504;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[8]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[0]);
    cpu->gpr[31] = 0x80199CF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80199D88u);
    cpu->pc = 0x80199D88u; return;  /* CPS jal -> 0x80199D88 */
}

/* cps_80199cf0 — clean CPS transliteration of br2eu_func_80199CF0 (guest 0x80199cf0)
 * from SLES_017.22_full_46.c. */
void cps_80199cf0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80199D30u: goto block_80199D30;
            case 0x80199D44u: goto block_80199D44;
            default: break;
        }
    }
    /* Address: 0x80199CF0, Size: 92 bytes, Blocks: 3 */

block_80199CF0:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13228);
    cpu->gpr[2] = 4901;
    cpu->write_word(cpu->gpr[3], cpu->gpr[2]);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -5352);
    cpu->gpr[2] = 3;
    cpu->write_half(cpu->gpr[3], (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14684);
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14672);
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    int _bc_80199D28 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -14684, cpu->gpr[2]);
    if (_bc_80199D28) {
        psx_check_interrupts_at(cpu, 0x80199D4Cu);
        cpu->pc = 0x80199D4Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199D30u);
        goto block_80199D30;  /* not taken */
    }

block_80199D30:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14692);
    /* nop */  /* 0x80199D38: 0x00000000 */
    int _bc_80199D3C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199D40: 0x00000000 */
    if (_bc_80199D3C) {
        psx_check_interrupts_at(cpu, 0x80199D4Cu);
        cpu->pc = 0x80199D4Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80199D44u);
        goto block_80199D44;  /* not taken */
    }

block_80199D44:
    cpu->gpr[31] = 0x80199D4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199D48: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801990F0u);
    cpu->pc = 0x801990F0u; return;  /* CPS jal -> 0x801990F0 */
}

/* cps_80199d4c — clean CPS transliteration of br2eu_func_80199D4C (guest 0x80199d4c)
 * from SLES_017.22_full_46.c. */
void cps_80199d4c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80199D4C, Size: 16 bytes, Blocks: 1 */

block_80199D4C:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 56);
    cpu->gpr[29] = cpu->gpr[29] + 64;
    uint32_t _jt_80199D54 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80199D58: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80199D54);
    cpu->pc = _jt_80199D54; return;  /* CPS: jr $ra */
}

/* cps_80198e2c — clean CPS transliteration of br2eu_func_80198E2C (guest 0x80198e2c)
 * from SLES_017.22_full_46.c. */
void cps_80198e2c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80198E64u: goto block_80198E64;
            case 0x80198E94u: goto block_80198E94;
            case 0x80198EB4u: goto block_80198EB4;
            case 0x80198EF4u: goto block_80198EF4;
            case 0x80198E5Cu: goto block_80198E5C;
            case 0x80198E7Cu: goto block_80198E7C;
            case 0x80198E8Cu: goto block_80198E8C;
            case 0x80198EACu: goto block_80198EAC;
            case 0x80198EC0u: goto block_80198EC0;
            case 0x80198EC4u: goto block_80198EC4;
            case 0x80198ECCu: goto block_80198ECC;
            case 0x80198EDCu: goto block_80198EDC;
            case 0x80198EE4u: goto block_80198EE4;
            case 0x80198EECu: goto block_80198EEC;
            case 0x80198F04u: goto block_80198F04;
            case 0x80198F0Cu: goto block_80198F0C;
            case 0x80198F10u: goto block_80198F10;
            default: break;
        }
    }
    /* Address: 0x80198E2C, Size: 260 bytes, Blocks: 18 */

block_80198E2C:
    cpu->gpr[29] = cpu->gpr[29] + -40;
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[19]);
    cpu->gpr[19] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[20]);
    cpu->gpr[20] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[17] = cpu->gpr[17] + -13412;
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[17] + 8;
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    /* fall through to block_80198E5C */

block_80198E5C:
    cpu->gpr[31] = 0x80198E64u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198E64:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17]);
    /* nop */  /* 0x80198E68: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] + 1200;
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x80198E70: 0x0062182A */
    int _bc_80198E74 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = -1;
    if (_bc_80198E74) {
        psx_check_interrupts_at(cpu, 0x80198EC4u);
        goto block_80198EC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198E7Cu);
        goto block_80198E7C;  /* not taken */
    }

block_80198E7C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[17] + -8);
    /* nop */  /* 0x80198E80: 0x00000000 */
    int _bc_80198E84 = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198E88: 0x00000000 */
    if (_bc_80198E84) {
        psx_check_interrupts_at(cpu, 0x80198EACu);
        goto block_80198EAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198E8Cu);
        goto block_80198E8C;  /* not taken */
    }

block_80198E8C:
    cpu->gpr[31] = 0x80198E94u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198E94:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + -4);
    /* nop */  /* 0x80198E98: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] + 60;
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x80198EA0: 0x0062182A */
    int _bc_80198EA4 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198EA8: 0x00000000 */
    if (_bc_80198EA4) {
        psx_check_interrupts_at(cpu, 0x80198EC0u);
        goto block_80198EC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198EACu);
        goto block_80198EAC;  /* not taken */
    }

block_80198EAC:
    cpu->gpr[31] = 0x80198EB4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1;
    psx_check_interrupts_at(cpu, 0x80198908u);
    cpu->pc = 0x80198908u; return;  /* CPS jal -> 0x80198908 */

block_80198EB4:
    cpu->gpr[16] = cpu->read_word(cpu->gpr[17] + -28);
    /* delay slot (always executes) */
    /* nop */  /* 0x80198EBC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198EC4u);
    goto block_80198EC4;  /* j */

block_80198EC0:
    cpu->gpr[16] = cpu->read_word(cpu->gpr[17] + -8);
    /* fall through to block_80198EC4 */

block_80198EC4:
    int _bc_80198EC4 = (cpu->gpr[19] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1;
    if (_bc_80198EC4) {
        psx_check_interrupts_at(cpu, 0x80198EECu);
        goto block_80198EEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198ECCu);
        goto block_80198ECC;  /* not taken */
    }

block_80198ECC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[18]);
    /* nop */  /* 0x80198ED0: 0x00000000 */
    int _bc_80198ED4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198ED8: 0x00000000 */
    if (_bc_80198ED4) {
        psx_check_interrupts_at(cpu, 0x80198EE4u);
        goto block_80198EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198EDCu);
        goto block_80198EDC;  /* not taken */
    }

block_80198EDC:
    int _bc_80198EDC = (cpu->gpr[16] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198EE0: 0x00000000 */
    if (_bc_80198EDC) {
        psx_check_interrupts_at(cpu, 0x80198E5Cu);
        goto block_80198E5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198EE4u);
        goto block_80198EE4;  /* not taken */
    }

block_80198EE4:
    int _bc_80198EE4 = ((int32_t)cpu->gpr[16] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1;
    if (_bc_80198EE4) {
        psx_check_interrupts_at(cpu, 0x80198E5Cu);
        goto block_80198E5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198EECu);
        goto block_80198EEC;  /* not taken */
    }

block_80198EEC:
    cpu->gpr[31] = 0x80198EF4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[20];
    psx_check_interrupts_at(cpu, 0x80195D44u);
    cpu->pc = 0x80195D44u; return;  /* CPS jal -> 0x80195D44 */

block_80198EF4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[18]);
    /* nop */  /* 0x80198EF8: 0x00000000 */
    int _bc_80198EFC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[16];
    if (_bc_80198EFC) {
        psx_check_interrupts_at(cpu, 0x80198F10u);
        goto block_80198F10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198F04u);
        goto block_80198F04;  /* not taken */
    }

block_80198F04:
    int _bc_80198F04 = (cpu->gpr[16] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = 1;
    if (_bc_80198F04) {
        psx_check_interrupts_at(cpu, 0x80198F10u);
        goto block_80198F10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198F0Cu);
        goto block_80198F0C;  /* not taken */
    }

block_80198F0C:
    cpu->gpr[2] = cpu->gpr[16];
    /* fall through to block_80198F10 */

block_80198F10:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_80198F28 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 40;
    psx_check_interrupts_at(cpu, _jt_80198F28);
    cpu->pc = _jt_80198F28; return;  /* CPS: jr $ra */
}

/* cps_801985c4 — clean CPS transliteration of br2eu_func_801985C4 (guest 0x801985c4)
 * from SLES_017.22_full_46.c. */
void cps_801985c4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80198634u: goto block_80198634;
            case 0x80198640u: goto block_80198640;
            case 0x80198648u: goto block_80198648;
            case 0x80198654u: goto block_80198654;
            case 0x80198664u: goto block_80198664;
            case 0x8019866Cu: goto block_8019866C;
            case 0x80198690u: goto block_80198690;
            case 0x801986CCu: goto block_801986CC;
            case 0x801986ECu: goto block_801986EC;
            case 0x80198734u: goto block_80198734;
            case 0x80198758u: goto block_80198758;
            case 0x80198760u: goto block_80198760;
            case 0x80198794u: goto block_80198794;
            case 0x801987B8u: goto block_801987B8;
            case 0x801987DCu: goto block_801987DC;
            case 0x801987E8u: goto block_801987E8;
            case 0x801987F4u: goto block_801987F4;
            case 0x80198824u: goto block_80198824;
            case 0x801985F4u: goto block_801985F4;
            case 0x80198604u: goto block_80198604;
            case 0x80198614u: goto block_80198614;
            case 0x8019862Cu: goto block_8019862C;
            case 0x8019865Cu: goto block_8019865C;
            case 0x80198684u: goto block_80198684;
            case 0x8019869Cu: goto block_8019869C;
            case 0x801986B4u: goto block_801986B4;
            case 0x801986D4u: goto block_801986D4;
            case 0x80198728u: goto block_80198728;
            case 0x8019872Cu: goto block_8019872C;
            case 0x80198750u: goto block_80198750;
            case 0x80198778u: goto block_80198778;
            case 0x8019877Cu: goto block_8019877C;
            case 0x8019878Cu: goto block_8019878C;
            case 0x801987ACu: goto block_801987AC;
            case 0x801987D0u: goto block_801987D0;
            case 0x80198808u: goto block_80198808;
            case 0x80198818u: goto block_80198818;
            case 0x8019881Cu: goto block_8019881C;
            default: break;
        }
    }
    /* Address: 0x801985C4, Size: 628 bytes, Blocks: 39 */

block_801985C4:
    cpu->gpr[29] = cpu->gpr[29] + -48;
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[5];
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->gpr[3] + -13440;
    cpu->gpr[4] = cpu->gpr[4] & 0x00FF;  /* 0x801985D8: 0x308400FF */
    cpu->gpr[2] = 1;
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[16]);
    cpu->write_word(cpu->gpr[3] + 52, cpu->gpr[17]);
    int _bc_801985EC = (cpu->gpr[4] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = -1;
    if (_bc_801985EC) {
        psx_check_interrupts_at(cpu, 0x80198728u);
        goto block_80198728;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801985F4u);
        goto block_801985F4;  /* not taken */
    }

block_801985F4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3] + 20);
    /* nop */  /* 0x801985F8: 0x00000000 */
    int _bc_801985FC = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198600: 0x00000000 */
    if (_bc_801985FC) {
        psx_check_interrupts_at(cpu, 0x8019872Cu);
        goto block_8019872C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198604u);
        goto block_80198604;  /* not taken */
    }

block_80198604:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + 16);
    cpu->gpr[2] = 512;
    int _bc_8019860C = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198610: 0x00000000 */
    if (_bc_8019860C) {
        psx_check_interrupts_at(cpu, 0x8019869Cu);
        goto block_8019869C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198614u);
        goto block_80198614;  /* not taken */
    }

block_80198614:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14412);
    /* nop */  /* 0x8019861C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80198620: 0x30420001 */
    int _bc_80198624 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[29] + 16;
    if (_bc_80198624) {
        psx_check_interrupts_at(cpu, 0x8019865Cu);
        goto block_8019865C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019862Cu);
        goto block_8019862C;  /* not taken */
    }

block_8019862C:
    cpu->gpr[31] = 0x80198634u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return;  /* CPS jal -> 0x801961A8 */

block_80198634:
    cpu->gpr[4] = cpu->gpr[29] + 16;
    cpu->gpr[31] = 0x80198640u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = 3;
    psx_check_interrupts_at(cpu, 0x80196188u);
    cpu->pc = 0x80196188u; return;  /* CPS jal -> 0x80196188 */

block_80198640:
    cpu->gpr[31] = 0x80198648u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801961CCu);
    cpu->pc = 0x801961CCu; return;  /* CPS jal -> 0x801961CC */

block_80198648:
    cpu->gpr[4] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[31] = 0x80198654u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + -30664;
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return;  /* CPS jal -> 0x801961A8 */

block_80198654:
    /* delay slot (always executes) */
    /* nop */  /* 0x80198658: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198664u);
    goto block_80198664;  /* j */

block_8019865C:
    cpu->gpr[31] = 0x80198664u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = 3;
    psx_check_interrupts_at(cpu, 0x80196168u);
    cpu->pc = 0x80196168u; return;  /* CPS jal -> 0x80196168 */

block_80198664:
    cpu->gpr[31] = 0x8019866Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[29] + 16;
    psx_check_interrupts_at(cpu, 0x801962F0u);
    cpu->pc = 0x801962F0u; return;  /* CPS jal -> 0x801962F0 */

block_8019866C:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13408;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16]);
    /* nop */  /* 0x80198678: 0x00000000 */
    int _bc_8019867C = (cpu->gpr[2] == cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198680: 0x00000000 */
    if (_bc_8019867C) {
        psx_check_interrupts_at(cpu, 0x8019869Cu);
        goto block_8019869C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198684u);
        goto block_80198684;  /* not taken */
    }

block_80198684:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x80198690u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 23240;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u; return;  /* CPS jal -> 0x80197AD0 */

block_80198690:
    cpu->gpr[3] = cpu->gpr[16] + -32;
    cpu->gpr[2] = -1;
    cpu->write_word(cpu->gpr[3] + 20, cpu->gpr[2]);
    /* fall through to block_8019869C */

block_8019869C:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14412);
    /* nop */  /* 0x801986A4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x801986A8: 0x30420001 */
    int _bc_801986AC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801986B0: 0x00000000 */
    if (_bc_801986AC) {
        psx_check_interrupts_at(cpu, 0x801986D4u);
        goto block_801986D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801986B4u);
        goto block_801986B4;  /* not taken */
    }

block_801986B4:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->gpr[2] + -13432;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[2] + 8);
    cpu->gpr[31] = 0x801986CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801986C8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80196188u);
    cpu->pc = 0x80196188u; return;  /* CPS jal -> 0x80196188 */

block_801986CC:
    /* delay slot (always executes) */
    /* nop */  /* 0x801986D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019872Cu);
    goto block_8019872C;  /* j */

block_801986D4:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13432;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[16]);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[16] + 8);
    cpu->gpr[31] = 0x801986ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801986E8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80196168u);
    cpu->pc = 0x80196168u; return;  /* CPS jal -> 0x80196168 */

block_801986EC:
    cpu->gpr[4] = cpu->gpr[16] + -8;
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 8);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16]);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->write_word(cpu->gpr[4] + 8, cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 20);
    /* nop */  /* 0x80198708: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + -1;
    cpu->write_word(cpu->gpr[4] + 20, cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 32);
    /* nop */  /* 0x80198718: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 1;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[4] + 32, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8019872Cu);
    goto block_8019872C;  /* j */

block_80198728:
    cpu->write_word(cpu->gpr[3] + 20, cpu->gpr[2]);
    /* fall through to block_8019872C */

block_8019872C:
    cpu->gpr[31] = 0x80198734u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198734:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13440;
    cpu->write_word(cpu->gpr[16] + 24, cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 20);
    /* nop */  /* 0x80198744: 0x00000000 */
    int _bc_80198748 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019874C: 0x00000000 */
    if (_bc_80198748) {
        psx_check_interrupts_at(cpu, 0x80198758u);
        goto block_80198758;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198750u);
        goto block_80198750;  /* not taken */
    }

block_80198750:
    cpu->gpr[31] = 0x80198758u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1;
    psx_check_interrupts_at(cpu, 0x80198908u);
    cpu->pc = 0x80198908u; return;  /* CPS jal -> 0x80198908 */

block_80198758:
    cpu->gpr[31] = 0x80198760u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198760:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16] + 28);
    /* nop */  /* 0x80198764: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] + 1200;
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x8019876C: 0x0062182A */
    int _bc_80198770 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = -1;
    if (_bc_80198770) {
        psx_check_interrupts_at(cpu, 0x8019877Cu);
        goto block_8019877C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198778u);
        goto block_80198778;  /* not taken */
    }

block_80198778:
    cpu->write_word(cpu->gpr[16] + 20, cpu->gpr[2]);
    /* fall through to block_8019877C */

block_8019877C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 20);
    /* nop */  /* 0x80198780: 0x00000000 */
    int _bc_80198784 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198788: 0x00000000 */
    if (_bc_80198784) {
        psx_check_interrupts_at(cpu, 0x801987ACu);
        goto block_801987AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019878Cu);
        goto block_8019878C;  /* not taken */
    }

block_8019878C:
    cpu->gpr[31] = 0x80198794u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198794:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16] + 28);
    /* nop */  /* 0x80198798: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] + 1200;
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x801987A0: 0x0062182A */
    int _bc_801987A4 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801987A8: 0x00000000 */
    if (_bc_801987A4) {
        psx_check_interrupts_at(cpu, 0x80198824u);
        goto block_80198824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801987ACu);
        goto block_801987AC;  /* not taken */
    }

block_801987AC:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[16] + 44);
    cpu->gpr[31] = 0x801987B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801987B4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80195D78u);
    cpu->pc = 0x80195D78u; return;  /* CPS jal -> 0x80195D78 */

block_801987B8:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14412);
    /* nop */  /* 0x801987C0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x801987C4: 0x30420001 */
    int _bc_801987C8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801987CC: 0x00000000 */
    if (_bc_801987C8) {
        psx_check_interrupts_at(cpu, 0x801987DCu);
        goto block_801987DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801987D0u);
        goto block_801987D0;  /* not taken */
    }

block_801987D0:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[16] + 48);
    cpu->gpr[31] = 0x801987DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801987D8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return;  /* CPS jal -> 0x801961A8 */

block_801987DC:
    cpu->gpr[4] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[31] = 0x801987E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + -31344;
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> 0x80195D64 */

block_801987E8:
    cpu->gpr[4] = 9;
    cpu->gpr[31] = 0x801987F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 */

block_801987F4:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14416);
    cpu->gpr[2] = 1;
    int _bc_80198800 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16] + 36, cpu->gpr[2]);
    if (_bc_80198800) {
        psx_check_interrupts_at(cpu, 0x80198824u);
        goto block_80198824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198808u);
        goto block_80198808;  /* not taken */
    }

block_80198808:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 20);
    /* nop */  /* 0x8019880C: 0x00000000 */
    int _bc_80198810 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 5;
    if (_bc_80198810) {
        psx_check_interrupts_at(cpu, 0x8019881Cu);
        goto block_8019881C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198818u);
        goto block_80198818;  /* not taken */
    }

block_80198818:
    cpu->gpr[4] = 2;
    /* fall through to block_8019881C */

block_8019881C:
    uint32_t _jt_8019881C = cpu->gpr[3];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x80198824u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[17];
    psx_check_interrupts_at(cpu, _jt_8019881C);
    cpu->pc = _jt_8019881C; return;  /* CPS jalr */

block_80198824:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 32);
    uint32_t _jt_80198830 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 48;
    psx_check_interrupts_at(cpu, _jt_80198830);
    cpu->pc = _jt_80198830; return;  /* CPS: jr $ra */
}

/* cps_8018d300 — clean CPS transliteration of br2eu_func_8018D300 (guest 0x8018d300)
 * from SLES_017.22_full_42.c. */
void cps_8018d300(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018D308u: goto block_8018D308;
            case 0x8018D30Cu: goto block_8018D30C;
            case 0x8018D314u: goto block_8018D314;
            case 0x8018D318u: goto block_8018D318;
            case 0x8018D324u: goto block_8018D324;
            case 0x8018D330u: goto block_8018D330;
            case 0x8018D338u: goto block_8018D338;
            case 0x8018D33Cu: goto block_8018D33C;
            case 0x8018D3ACu: goto block_8018D3AC;
            default: break;
        }
    }
    /* Address: 0x8018D300, Size: 180 bytes, Blocks: 10 */

block_8018D300:
    int _bc_8018D300 = ((int32_t)cpu->gpr[4] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4];
    if (_bc_8018D300) {
        psx_check_interrupts_at(cpu, 0x8018D30Cu);
        goto block_8018D30C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D308u);
        goto block_8018D308;  /* not taken */
    }

block_8018D308:
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[4];
    /* fall through to block_8018D30C */

block_8018D30C:
    int _bc_8018D30C = ((int32_t)cpu->gpr[5] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[5];
    if (_bc_8018D30C) {
        psx_check_interrupts_at(cpu, 0x8018D318u);
        goto block_8018D318;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D314u);
        goto block_8018D314;  /* not taken */
    }

block_8018D314:
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[5];
    /* fall through to block_8018D318 */

block_8018D318:
    cpu->gpr[8] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x8018D318: 0x0043402A */
    int _bc_8018D31C = (cpu->gpr[8] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[10] = 0x0002;
    if (_bc_8018D31C) {
        psx_check_interrupts_at(cpu, 0x8018D330u);
        goto block_8018D330;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D324u);
        goto block_8018D324;  /* not taken */
    }

block_8018D324:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018D324: 0x00431026 */
    cpu->gpr[3] = cpu->gpr[3] ^ cpu->gpr[2];  /* 0x8018D328: 0x00621826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018D32C: 0x00431026 */
    /* fall through to block_8018D330 */

block_8018D330:
    int _bc_8018D330 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[11] = cpu->gpr[2] << 15;
    if (_bc_8018D330) {
        psx_check_interrupts_at(cpu, 0x8018D3ACu);
        goto block_8018D3AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D338u);
        goto block_8018D338;  /* not taken */
    }

block_8018D338:
    cpu->gpr[3] = cpu->gpr[3] << 15;
    /* fall through to block_8018D33C */

block_8018D33C:
    if (cpu->gpr[2] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[3] / (int32_t)cpu->gpr[2]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[3] % (int32_t)cpu->gpr[2]); } else { cpu->lo = ((int32_t)cpu->gpr[3] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[3]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    /* nop */  /* 0x8018D344: 0x00000000 */
    /* nop */  /* 0x8018D348: 0x00000000 */
    { uint64_t result = (uint64_t)cpu->gpr[8] * (uint64_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(cpu->gpr[8]));
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[9] = (uint32_t)cpu->gpr[8] >> 15;
    cpu->gpr[1] = 0x0002 << 16;  /* 0x00020000 */
    cpu->gpr[9] = cpu->gpr[9] + cpu->gpr[1];
    if (cpu->gpr[9] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[8] / (int32_t)cpu->gpr[9]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[8] % (int32_t)cpu->gpr[9]); } else { cpu->lo = ((int32_t)cpu->gpr[8] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[8]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[9] = cpu->gpr[11] << 1;
    cpu->gpr[10] = cpu->gpr[10] + -1;
    { uint64_t result = (uint64_t)cpu->gpr[9] * (uint64_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(cpu->gpr[9]));
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[9] = (uint32_t)cpu->gpr[9] >> 15;
    cpu->gpr[2] = cpu->gpr[2] << 17;
    { uint64_t result = (uint64_t)cpu->gpr[3] * (uint64_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(cpu->gpr[3]));
    cpu->gpr[9] = cpu->gpr[2] | cpu->gpr[9];
    cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[9];
    cpu->gpr[2] = (uint32_t)cpu->gpr[11] >> 15;
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[9] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (uint32_t)cpu->gpr[3] >> 15;
    cpu->gpr[9] = cpu->gpr[9] << 17;
    int _bc_8018D3A4 = ((int32_t)cpu->gpr[10] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[9] | cpu->gpr[3];
    if (_bc_8018D3A4) {
        psx_check_interrupts_at(cpu, 0x8018D33Cu);
        goto block_8018D33C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D3ACu);
        goto block_8018D3AC;  /* not taken */
    }

block_8018D3AC:
    uint32_t _jt_8018D3AC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D3B0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8018D3AC);
    cpu->pc = _jt_8018D3AC; return;  /* CPS: jr $ra */
}

/* cps_8018f7e0 — clean CPS transliteration of br2eu_func_8018F7E0 (guest 0x8018f7e0)
 * from SLES_017.22_full_43.c. */
void cps_8018f7e0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018F7F0u: goto block_8018F7F0;
            case 0x8018F7F8u: goto block_8018F7F8;
            case 0x8018F814u: goto block_8018F814;
            case 0x8018F81Cu: goto block_8018F81C;
            case 0x8018F824u: goto block_8018F824;
            case 0x8018F840u: goto block_8018F840;
            case 0x8018F844u: goto block_8018F844;
            default: break;
        }
    }
    /* Address: 0x8018F7E0, Size: 108 bytes, Blocks: 8 */

block_8018F7E0:
    cpu->gpr[7] = cpu->gpr[4];
    cpu->gpr[2] = (cpu->gpr[7] < cpu->gpr[5]) ? 1 : 0;  /* 0x8018F7E4: 0x00E5102B */
    int _bc_8018F7E8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6];
    if (_bc_8018F7E8) {
        psx_check_interrupts_at(cpu, 0x8018F81Cu);
        goto block_8018F81C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018F7F0u);
        goto block_8018F7F0;  /* not taken */
    }

block_8018F7F0:
    int _bc_8018F7F0 = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[6] + -1;
    if (_bc_8018F7F0) {
        psx_check_interrupts_at(cpu, 0x8018F840u);
        goto block_8018F840;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018F7F8u);
        goto block_8018F7F8;  /* not taken */
    }

block_8018F7F8:
    cpu->gpr[4] = cpu->gpr[7] + cpu->gpr[6];
    cpu->gpr[2] = cpu->gpr[5] + cpu->gpr[6];
    cpu->gpr[3] = cpu->gpr[6];
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2]);
    cpu->gpr[6] = cpu->gpr[6] + -1;
    int _bc_8018F80C = ((int32_t)cpu->gpr[3] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[4], (uint8_t)cpu->gpr[2]);
    if (_bc_8018F80C) {
        psx_check_interrupts_at(cpu, 0x8018F7F8u);
        goto block_8018F7F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018F814u);
        goto block_8018F814;  /* not taken */
    }

block_8018F814:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[7];
    psx_check_interrupts_at(cpu, 0x8018F844u);
    goto block_8018F844;  /* j */

block_8018F81C:
    int _bc_8018F81C = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[6] + -1;
    if (_bc_8018F81C) {
        psx_check_interrupts_at(cpu, 0x8018F840u);
        goto block_8018F840;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018F824u);
        goto block_8018F824;  /* not taken */
    }

block_8018F824:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[3] = cpu->gpr[6];
    cpu->gpr[6] = cpu->gpr[6] + -1;
    cpu->write_byte(cpu->gpr[7], (uint8_t)cpu->gpr[2]);
    int _bc_8018F838 = ((int32_t)cpu->gpr[3] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[7] + 1;
    if (_bc_8018F838) {
        psx_check_interrupts_at(cpu, 0x8018F824u);
        goto block_8018F824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018F840u);
        goto block_8018F840;  /* not taken */
    }

block_8018F840:
    cpu->gpr[2] = cpu->gpr[7];
    /* fall through to block_8018F844 */

block_8018F844:
    uint32_t _jt_8018F844 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018F848: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8018F844);
    cpu->pc = _jt_8018F844; return;  /* CPS: jr $ra */
}

/* cps_8018e490 — clean CPS transliteration of br2eu_func_8018E490 (guest 0x8018e490)
 * from SLES_017.22_full_42.c. */
void cps_8018e490(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018E498u: goto block_8018E498;
            case 0x8018E4A0u: goto block_8018E4A0;
            case 0x8018E4B8u: goto block_8018E4B8;
            case 0x8018E4BCu: goto block_8018E4BC;
            default: break;
        }
    }
    /* Address: 0x8018E490, Size: 52 bytes, Blocks: 5 */

block_8018E490:
    int _bc_8018E490 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_8018E490) {
        psx_check_interrupts_at(cpu, 0x8018E4BCu);
        goto block_8018E4BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E498u);
        goto block_8018E498;  /* not taken */
    }

block_8018E498:
    int _bc_8018E498 = ((int32_t)cpu->gpr[6] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[4];
    if (_bc_8018E498) {
        psx_check_interrupts_at(cpu, 0x8018E4B8u);
        goto block_8018E4B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E4A0u);
        goto block_8018E4A0;  /* not taken */
    }

block_8018E4A0:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5]);
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[6] = cpu->gpr[6] + -1;
    cpu->write_byte(cpu->gpr[4], (uint8_t)cpu->gpr[2]);
    int _bc_8018E4B0 = ((int32_t)cpu->gpr[6] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1;
    if (_bc_8018E4B0) {
        psx_check_interrupts_at(cpu, 0x8018E4A0u);
        goto block_8018E4A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E4B8u);
        goto block_8018E4B8;  /* not taken */
    }

block_8018E4B8:
    cpu->gpr[2] = cpu->gpr[3];
    /* fall through to block_8018E4BC */

block_8018E4BC:
    uint32_t _jt_8018E4BC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018E4C0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8018E4BC);
    cpu->pc = _jt_8018E4BC; return;  /* CPS: jr $ra */
}

/* cps_8018eca0 — clean CPS transliteration of br2eu_func_8018ECA0 (guest 0x8018eca0)
 * from SLES_017.22_full_42.c. */
void cps_8018eca0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018ECA8u: goto block_8018ECA8;
            case 0x8018ECB0u: goto block_8018ECB0;
            case 0x8018ECB8u: goto block_8018ECB8;
            case 0x8018ECC0u: goto block_8018ECC0;
            case 0x8018ECC8u: goto block_8018ECC8;
            case 0x8018ECCCu: goto block_8018ECCC;
            case 0x8018ECDCu: goto block_8018ECDC;
            case 0x8018ECE8u: goto block_8018ECE8;
            default: break;
        }
    }
    /* Address: 0x8018ECA0, Size: 80 bytes, Blocks: 9 */

block_8018ECA0:
    int _bc_8018ECA0 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_8018ECA0) {
        psx_check_interrupts_at(cpu, 0x8018ECE8u);
        goto block_8018ECE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018ECA8u);
        goto block_8018ECA8;  /* not taken */
    }

block_8018ECA8:
    int _bc_8018ECA8 = ((int32_t)cpu->gpr[6] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018ECAC: 0x00000000 */
    if (_bc_8018ECA8) {
        psx_check_interrupts_at(cpu, 0x8018ECE8u);
        goto block_8018ECE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018ECB0u);
        goto block_8018ECB0;  /* not taken */
    }

block_8018ECB0:
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[6] + -1;
    psx_check_interrupts_at(cpu, 0x8018ECC0u);
    goto block_8018ECC0;  /* j */

block_8018ECB8:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + -1;
    psx_check_interrupts_at(cpu, 0x8018ECE8u);
    goto block_8018ECE8;  /* j */

block_8018ECC0:
    int _bc_8018ECC0 = ((int32_t)cpu->gpr[6] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_8018ECC0) {
        psx_check_interrupts_at(cpu, 0x8018ECE8u);
        goto block_8018ECE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018ECC8u);
        goto block_8018ECC8;  /* not taken */
    }

block_8018ECC8:
    cpu->gpr[5] = cpu->gpr[5] & 0x00FF;  /* 0x8018ECC8: 0x30A500FF */
    /* fall through to block_8018ECCC */

block_8018ECCC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4]);
    /* nop */  /* 0x8018ECD0: 0x00000000 */
    int _bc_8018ECD4 = (cpu->gpr[2] == cpu->gpr[5]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1;
    if (_bc_8018ECD4) {
        psx_check_interrupts_at(cpu, 0x8018ECB8u);
        goto block_8018ECB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018ECDCu);
        goto block_8018ECDC;  /* not taken */
    }

block_8018ECDC:
    cpu->gpr[6] = cpu->gpr[6] + -1;
    int _bc_8018ECE0 = ((int32_t)cpu->gpr[6] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_8018ECE0) {
        psx_check_interrupts_at(cpu, 0x8018ECCCu);
        goto block_8018ECCC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018ECE8u);
        goto block_8018ECE8;  /* not taken */
    }

block_8018ECE8:
    uint32_t _jt_8018ECE8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018ECEC: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8018ECE8);
    cpu->pc = _jt_8018ECE8; return;  /* CPS: jr $ra */
}

/* cps_80194db0 — clean CPS transliteration of br2eu_func_80194DB0 (guest 0x80194db0)
 * from SLES_017.22_full_44.c. */
void cps_80194db0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80194DECu: goto block_80194DEC;
            case 0x80194E04u: goto block_80194E04;
            case 0x80194E0Cu: goto block_80194E0C;
            case 0x80194E14u: goto block_80194E14;
            case 0x80194E2Cu: goto block_80194E2C;
            case 0x80194E34u: goto block_80194E34;
            case 0x80194E3Cu: goto block_80194E3C;
            case 0x80194E54u: goto block_80194E54;
            case 0x80194E5Cu: goto block_80194E5C;
            case 0x80194E98u: goto block_80194E98;
            case 0x80194EA8u: goto block_80194EA8;
            case 0x80194EACu: goto block_80194EAC;
            case 0x80194EB4u: goto block_80194EB4;
            case 0x80194EC4u: goto block_80194EC4;
            case 0x80194EC8u: goto block_80194EC8;
            case 0x80194ED0u: goto block_80194ED0;
            case 0x80194EE0u: goto block_80194EE0;
            case 0x80194EE4u: goto block_80194EE4;
            default: break;
        }
    }
    /* Address: 0x80194DB0, Size: 352 bytes, Blocks: 19 */

block_80194DB0:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[4]);
    cpu->gpr[9] = cpu->read_word(cpu->gpr[4] + 4);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[4] + 8);
    cpu->gpr[11] = cpu->read_word(cpu->gpr[4] + 12);
    cpu->gpr[12] = cpu->read_word(cpu->gpr[4] + 16);
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[0] = cpu->gpr[8];  /* ctc2 */  /* 0x80194DC4: 0x48C80000 */
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[1] = cpu->gpr[9];  /* ctc2 */  /* 0x80194DC8: 0x48C90800 */
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[2] = cpu->gpr[10];  /* ctc2 */  /* 0x80194DCC: 0x48CA1000 */
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[3] = cpu->gpr[11];  /* ctc2 */  /* 0x80194DD0: 0x48CB1800 */
    
    psx_gte_stall(cpu);
    gte_write_ctrl(cpu, 4, cpu->gpr[12]);  /* ctc2 */  /* 0x80194DD4: 0x48CC2000 */
    cpu->gpr[8] = cpu->read_word(cpu->gpr[5]);
    cpu->gpr[9] = cpu->read_word(cpu->gpr[5] + 4);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[5] + 8);
    int _bc_80194DE4 = ((int32_t)cpu->gpr[8] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80194DE8: 0x00000000 */
    if (_bc_80194DE4) {
        psx_check_interrupts_at(cpu, 0x80194E04u);
        goto block_80194E04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194DECu);
        goto block_80194DEC;  /* not taken */
    }

block_80194DEC:
    cpu->gpr[8] = cpu->gpr[0] - cpu->gpr[8];
    cpu->gpr[11] = (int32_t)cpu->gpr[8] >> 15;
    cpu->gpr[11] = cpu->gpr[0] - cpu->gpr[11];
    cpu->gpr[8] = cpu->gpr[8] & 0x7FFF;  /* 0x80194DF8: 0x31087FFF */
    int _bc_80194DFC = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[0] - cpu->gpr[8];
    if (_bc_80194DFC) {
        psx_check_interrupts_at(cpu, 0x80194E0Cu);
        goto block_80194E0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194E04u);
        goto block_80194E04;  /* not taken */
    }

block_80194E04:
    cpu->gpr[11] = (int32_t)cpu->gpr[8] >> 15;
    cpu->gpr[8] = cpu->gpr[8] & 0x7FFF;  /* 0x80194E08: 0x31087FFF */
    /* fall through to block_80194E0C */

block_80194E0C:
    int _bc_80194E0C = ((int32_t)cpu->gpr[9] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80194E10: 0x00000000 */
    if (_bc_80194E0C) {
        psx_check_interrupts_at(cpu, 0x80194E2Cu);
        goto block_80194E2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194E14u);
        goto block_80194E14;  /* not taken */
    }

block_80194E14:
    cpu->gpr[9] = cpu->gpr[0] - cpu->gpr[9];
    cpu->gpr[12] = (int32_t)cpu->gpr[9] >> 15;
    cpu->gpr[12] = cpu->gpr[0] - cpu->gpr[12];
    cpu->gpr[9] = cpu->gpr[9] & 0x7FFF;  /* 0x80194E20: 0x31297FFF */
    int _bc_80194E24 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[9] = cpu->gpr[0] - cpu->gpr[9];
    if (_bc_80194E24) {
        psx_check_interrupts_at(cpu, 0x80194E34u);
        goto block_80194E34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194E2Cu);
        goto block_80194E2C;  /* not taken */
    }

block_80194E2C:
    cpu->gpr[12] = (int32_t)cpu->gpr[9] >> 15;
    cpu->gpr[9] = cpu->gpr[9] & 0x7FFF;  /* 0x80194E30: 0x31297FFF */
    /* fall through to block_80194E34 */

block_80194E34:
    int _bc_80194E34 = ((int32_t)cpu->gpr[10] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80194E38: 0x00000000 */
    if (_bc_80194E34) {
        psx_check_interrupts_at(cpu, 0x80194E54u);
        goto block_80194E54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194E3Cu);
        goto block_80194E3C;  /* not taken */
    }

block_80194E3C:
    cpu->gpr[10] = cpu->gpr[0] - cpu->gpr[10];
    cpu->gpr[13] = (int32_t)cpu->gpr[10] >> 15;
    cpu->gpr[13] = cpu->gpr[0] - cpu->gpr[13];
    cpu->gpr[10] = cpu->gpr[10] & 0x7FFF;  /* 0x80194E48: 0x314A7FFF */
    int _bc_80194E4C = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[10] = cpu->gpr[0] - cpu->gpr[10];
    if (_bc_80194E4C) {
        psx_check_interrupts_at(cpu, 0x80194E5Cu);
        goto block_80194E5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194E54u);
        goto block_80194E54;  /* not taken */
    }

block_80194E54:
    cpu->gpr[13] = (int32_t)cpu->gpr[10] >> 15;
    cpu->gpr[10] = cpu->gpr[10] & 0x7FFF;  /* 0x80194E58: 0x314A7FFF */
    /* fall through to block_80194E5C */

block_80194E5C:
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, cpu->gpr[11]);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, cpu->gpr[12]);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, cpu->gpr[13]);  /* mtc2 */
    /* nop */  /* 0x80194E68: 0x00000000 */
    gte_execute(cpu, 0x041E012);  /* gte cmd 0x12 */  /* 0x80194E6C: 0x4A41E012 */
    
    psx_gte_read(cpu, 11);
    cpu->gpr[11] = cpu->gte_data[25];  /* mfc2 */
    
    psx_gte_read(cpu, 12);
    cpu->gpr[12] = cpu->gte_data[26];  /* mfc2 */
    
    psx_gte_read(cpu, 13);
    cpu->gpr[13] = cpu->gte_data[27];  /* mfc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 9, cpu->gpr[8]);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 10, cpu->gpr[9]);  /* mtc2 */
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 11, cpu->gpr[10]);  /* mtc2 */
    /* nop */  /* 0x80194E88: 0x00000000 */
    gte_execute(cpu, 0x049E012);  /* gte cmd 0x12 */  /* 0x80194E8C: 0x4A49E012 */
    int _bc_80194E90 = ((int32_t)cpu->gpr[11] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80194E94: 0x00000000 */
    if (_bc_80194E90) {
        psx_check_interrupts_at(cpu, 0x80194EA8u);
        goto block_80194EA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194E98u);
        goto block_80194E98;  /* not taken */
    }

block_80194E98:
    cpu->gpr[11] = cpu->gpr[0] - cpu->gpr[11];
    cpu->gpr[11] = cpu->gpr[11] << 3;
    int _bc_80194EA0 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[11] = cpu->gpr[0] - cpu->gpr[11];
    if (_bc_80194EA0) {
        psx_check_interrupts_at(cpu, 0x80194EACu);
        goto block_80194EAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194EA8u);
        goto block_80194EA8;  /* not taken */
    }

block_80194EA8:
    cpu->gpr[11] = cpu->gpr[11] << 3;
    /* fall through to block_80194EAC */

block_80194EAC:
    int _bc_80194EAC = ((int32_t)cpu->gpr[12] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80194EB0: 0x00000000 */
    if (_bc_80194EAC) {
        psx_check_interrupts_at(cpu, 0x80194EC4u);
        goto block_80194EC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194EB4u);
        goto block_80194EB4;  /* not taken */
    }

block_80194EB4:
    cpu->gpr[12] = cpu->gpr[0] - cpu->gpr[12];
    cpu->gpr[12] = cpu->gpr[12] << 3;
    int _bc_80194EBC = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[12] = cpu->gpr[0] - cpu->gpr[12];
    if (_bc_80194EBC) {
        psx_check_interrupts_at(cpu, 0x80194EC8u);
        goto block_80194EC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194EC4u);
        goto block_80194EC4;  /* not taken */
    }

block_80194EC4:
    cpu->gpr[12] = cpu->gpr[12] << 3;
    /* fall through to block_80194EC8 */

block_80194EC8:
    int _bc_80194EC8 = ((int32_t)cpu->gpr[13] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80194ECC: 0x00000000 */
    if (_bc_80194EC8) {
        psx_check_interrupts_at(cpu, 0x80194EE0u);
        goto block_80194EE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194ED0u);
        goto block_80194ED0;  /* not taken */
    }

block_80194ED0:
    cpu->gpr[13] = cpu->gpr[0] - cpu->gpr[13];
    cpu->gpr[13] = cpu->gpr[13] << 3;
    int _bc_80194ED8 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[13] = cpu->gpr[0] - cpu->gpr[13];
    if (_bc_80194ED8) {
        psx_check_interrupts_at(cpu, 0x80194EE4u);
        goto block_80194EE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80194EE0u);
        goto block_80194EE0;  /* not taken */
    }

block_80194EE0:
    cpu->gpr[13] = cpu->gpr[13] << 3;
    /* fall through to block_80194EE4 */

block_80194EE4:
    
    psx_gte_read(cpu, 8);
    cpu->gpr[8] = cpu->gte_data[25];  /* mfc2 */
    
    psx_gte_read(cpu, 9);
    cpu->gpr[9] = cpu->gte_data[26];  /* mfc2 */
    
    psx_gte_read(cpu, 10);
    cpu->gpr[10] = cpu->gte_data[27];  /* mfc2 */
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[11];
    cpu->gpr[9] = cpu->gpr[9] + cpu->gpr[12];
    cpu->gpr[10] = cpu->gpr[10] + cpu->gpr[13];
    cpu->write_word(cpu->gpr[6], cpu->gpr[8]);
    cpu->write_word(cpu->gpr[6] + 4, cpu->gpr[9]);
    cpu->write_word(cpu->gpr[6] + 8, cpu->gpr[10]);
    uint32_t _jt_80194F08 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_80194F08);
    cpu->pc = _jt_80194F08; return;  /* CPS: jr $ra */
}

/* cps_8018da2c — clean CPS transliteration of br2eu_func_8018DA2C (guest 0x8018da2c)
 * from SLES_017.22_full_42.c. */
void cps_8018da2c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018DA54u: goto block_8018DA54;
            case 0x8018DA60u: goto block_8018DA60;
            case 0x8018DA78u: goto block_8018DA78;
            case 0x8018DA7Cu: goto block_8018DA7C;
            case 0x8018DA90u: goto block_8018DA90;
            case 0x8018DA98u: goto block_8018DA98;
            default: break;
        }
    }
    /* Address: 0x8018DA2C, Size: 136 bytes, Blocks: 7 */

block_8018DA2C:
    cpu->gpr[8] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[8] = cpu->gpr[8] + 18344;
    cpu->gpr[9] = 0x1000;
    cpu->write_word(cpu->gpr[5], cpu->gpr[9]);
    cpu->gpr[2] = cpu->gpr[4] & 0x03FF;  /* 0x8018DA3C: 0x308203FF */
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[4] & 0x0400;  /* 0x8018DA48: 0x30890400 */
    int _bc_8018DA4C = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 2048;
    if (_bc_8018DA4C) {
        psx_check_interrupts_at(cpu, 0x8018DA60u);
        goto block_8018DA60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DA54u);
        goto block_8018DA54;  /* not taken */
    }

block_8018DA54:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DA54: 0x00431026 */
    cpu->gpr[3] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DA58: 0x00431826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DA5C: 0x00431026 */
    /* fall through to block_8018DA60 */

block_8018DA60:
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[8] + cpu->gpr[3];
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DA6C: 0x30890800 */
    int _bc_8018DA70 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    if (_bc_8018DA70) {
        psx_check_interrupts_at(cpu, 0x8018DA7Cu);
        goto block_8018DA7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DA78u);
        goto block_8018DA78;  /* not taken */
    }

block_8018DA78:
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    /* fall through to block_8018DA7C */

block_8018DA7C:
    cpu->gpr[3] = cpu->read_half(cpu->gpr[3]);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DA84: 0x30890800 */
    int _bc_8018DA88 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[0] - cpu->gpr[2];
    if (_bc_8018DA88) {
        psx_check_interrupts_at(cpu, 0x8018DA98u);
        goto block_8018DA98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DA90u);
        goto block_8018DA90;  /* not taken */
    }

block_8018DA90:
    cpu->gpr[9] = cpu->gpr[0] - cpu->gpr[3];
    cpu->gpr[3] = cpu->gpr[9] & 0xFFFF;  /* 0x8018DA94: 0x3123FFFF */
    /* fall through to block_8018DA98 */

block_8018DA98:
    cpu->write_word(cpu->gpr[5] + 4, cpu->gpr[0]);
    cpu->gpr[9] = cpu->gpr[3] | cpu->gpr[8];
    cpu->write_word(cpu->gpr[5] + 8, cpu->gpr[9]);
    cpu->write_word(cpu->gpr[5] + 12, cpu->gpr[2]);
    cpu->write_half(cpu->gpr[5] + 16, (uint16_t)cpu->gpr[3]);
    uint32_t _jt_8018DAAC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, _jt_8018DAAC);
    cpu->pc = _jt_8018DAAC; return;  /* CPS: jr $ra */
}

/* cps_8018dac0 — clean CPS transliteration of br2eu_func_8018DAC0 (guest 0x8018dac0)
 * from SLES_017.22_full_42.c. */
void cps_8018dac0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018DAE0u: goto block_8018DAE0;
            case 0x8018DAECu: goto block_8018DAEC;
            case 0x8018DB04u: goto block_8018DB04;
            case 0x8018DB08u: goto block_8018DB08;
            case 0x8018DB20u: goto block_8018DB20;
            case 0x8018DB2Cu: goto block_8018DB2C;
            case 0x8018DB44u: goto block_8018DB44;
            case 0x8018DB48u: goto block_8018DB48;
            case 0x8018DB60u: goto block_8018DB60;
            case 0x8018DB64u: goto block_8018DB64;
            case 0x8018DB88u: goto block_8018DB88;
            case 0x8018DB8Cu: goto block_8018DB8C;
            default: break;
        }
    }
    /* Address: 0x8018DAC0, Size: 292 bytes, Blocks: 13 */

block_8018DAC0:
    cpu->gpr[8] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[8] = cpu->gpr[8] + 18344;
    cpu->gpr[2] = cpu->gpr[4] & 0x03FF;  /* 0x8018DAC8: 0x308203FF */
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[15] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[4] & 0x0400;  /* 0x8018DAD4: 0x30890400 */
    int _bc_8018DAD8 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[15] = cpu->gpr[15] + 2048;
    if (_bc_8018DAD8) {
        psx_check_interrupts_at(cpu, 0x8018DAECu);
        goto block_8018DAEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DAE0u);
        goto block_8018DAE0;  /* not taken */
    }

block_8018DAE0:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[15];  /* 0x8018DAE0: 0x004F1026 */
    cpu->gpr[15] = cpu->gpr[2] ^ cpu->gpr[15];  /* 0x8018DAE4: 0x004F7826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[15];  /* 0x8018DAE8: 0x004F1026 */
    /* fall through to block_8018DAEC */

block_8018DAEC:
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[10] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2]);
    cpu->gpr[15] = cpu->gpr[8] + cpu->gpr[15];
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DAF8: 0x30890800 */
    int _bc_8018DAFC = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    if (_bc_8018DAFC) {
        psx_check_interrupts_at(cpu, 0x8018DB08u);
        goto block_8018DB08;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DB04u);
        goto block_8018DB04;  /* not taken */
    }

block_8018DB04:
    cpu->gpr[10] = cpu->gpr[0] - cpu->gpr[10];
    /* fall through to block_8018DB08 */

block_8018DB08:
    cpu->gpr[2] = cpu->gpr[5] & 0x03FF;  /* 0x8018DB08: 0x30A203FF */
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[5] & 0x0400;  /* 0x8018DB14: 0x30A90400 */
    int _bc_8018DB18 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 2048;
    if (_bc_8018DB18) {
        psx_check_interrupts_at(cpu, 0x8018DB2Cu);
        goto block_8018DB2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DB20u);
        goto block_8018DB20;  /* not taken */
    }

block_8018DB20:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DB20: 0x00431026 */
    cpu->gpr[3] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DB24: 0x00431826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DB28: 0x00431026 */
    /* fall through to block_8018DB2C */

block_8018DB2C:
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[12] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[8] + cpu->gpr[3];
    cpu->gpr[9] = cpu->gpr[5] & 0x0800;  /* 0x8018DB38: 0x30A90800 */
    int _bc_8018DB3C = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + 1024;
    if (_bc_8018DB3C) {
        psx_check_interrupts_at(cpu, 0x8018DB48u);
        goto block_8018DB48;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DB44u);
        goto block_8018DB44;  /* not taken */
    }

block_8018DB44:
    cpu->gpr[12] = cpu->gpr[0] - cpu->gpr[12];
    /* fall through to block_8018DB48 */

block_8018DB48:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[12] * (int64_t)(int32_t)cpu->gpr[10]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[12]));
    cpu->gpr[13] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[3]);
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[12];
    cpu->gpr[9] = cpu->gpr[5] & 0x0800;  /* 0x8018DB54: 0x30A90800 */
    int _bc_8018DB58 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[14] = cpu->gpr[2] & 0xFFFF;  /* 0x8018DB5C: 0x304EFFFF */
    if (_bc_8018DB58) {
        psx_check_interrupts_at(cpu, 0x8018DB64u);
        goto block_8018DB64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DB60u);
        goto block_8018DB60;  /* not taken */
    }

block_8018DB60:
    cpu->gpr[13] = cpu->gpr[0] - cpu->gpr[13];
    /* fall through to block_8018DB64 */

block_8018DB64:
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 12;
    cpu->gpr[8] = cpu->gpr[2] << 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[13] * (int64_t)(int32_t)cpu->gpr[10]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[13]));
    cpu->gpr[11] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[15]);
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[10];
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DB7C: 0x30890800 */
    int _bc_8018DB80 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[10] = cpu->gpr[2] << 16;
    if (_bc_8018DB80) {
        psx_check_interrupts_at(cpu, 0x8018DB8Cu);
        goto block_8018DB8C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DB88u);
        goto block_8018DB88;  /* not taken */
    }

block_8018DB88:
    cpu->gpr[11] = cpu->gpr[0] - cpu->gpr[11];
    /* fall through to block_8018DB8C */

block_8018DB8C:
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 12;
    cpu->gpr[9] = cpu->gpr[2] << 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[12] * (int64_t)(int32_t)cpu->gpr[11]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[12]));
    cpu->gpr[3] = cpu->gpr[14] | cpu->gpr[9];
    cpu->write_word(cpu->gpr[6] + 12, cpu->gpr[3]);
    cpu->gpr[2] = cpu->gpr[13] & 0xFFFF;  /* 0x8018DBA4: 0x31A2FFFF */
    cpu->gpr[3] = cpu->gpr[2] | cpu->gpr[8];
    cpu->write_word(cpu->gpr[6], cpu->gpr[3]);
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 12;
    cpu->gpr[2] = cpu->gpr[3] & 0xFFFF;  /* 0x8018DBB8: 0x3062FFFF */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[13] * (int64_t)(int32_t)cpu->gpr[11]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[13]));
    cpu->write_word(cpu->gpr[6] + 4, cpu->gpr[2]);
    cpu->gpr[9] = cpu->gpr[11] & 0xFFFF;  /* 0x8018DBC4: 0x3169FFFF */
    cpu->gpr[3] = cpu->gpr[9] | cpu->gpr[10];
    cpu->write_word(cpu->gpr[6] + 8, cpu->gpr[3]);
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 12;
    cpu->write_half(cpu->gpr[6] + 16, (uint16_t)cpu->gpr[3]);
    uint32_t _jt_8018DBDC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_8018DBDC);
    cpu->pc = _jt_8018DBDC; return;  /* CPS: jr $ra */
}

/* cps_8018b050 — clean CPS transliteration of br2eu_func_8018B050 (guest 0x8018b050)
 * from SLES_017.22_full_41.c. */
void cps_8018b050(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018B05Cu: goto block_8018B05C;
            case 0x8018B064u: goto block_8018B064;
            case 0x8018B070u: goto block_8018B070;
            case 0x8018B07Cu: goto block_8018B07C;
            default: break;
        }
    }
    /* Address: 0x8018B050, Size: 80 bytes, Blocks: 5 */

block_8018B050:
    cpu->gpr[8] = cpu->gpr[31];
    cpu->gpr[31] = 0x8018B05Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018B058: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_8018B05C:
    cpu->gpr[31] = 0x8018B064u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[9] = cpu->gpr[0] - cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_8018B064:
    cpu->write_half(cpu->gpr[6] + 2, (uint16_t)cpu->gpr[2]);
    cpu->gpr[31] = 0x8018B070u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_8018B070:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[9] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[9]));
    cpu->gpr[31] = 0x8018B07Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018B078: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_8018B07C:
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 12;
    cpu->write_half(cpu->gpr[6], (uint16_t)cpu->gpr[3]);
    { int64_t result = (int64_t)(int32_t)cpu->gpr[9] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[9]));
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 12;
    cpu->write_half(cpu->gpr[6] + 4, (uint16_t)cpu->gpr[3]);
    uint32_t _jt_8018B098 = cpu->gpr[8];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_8018B098);
    cpu->pc = _jt_8018B098; return;  /* CPS: jr cpu->gpr[8] */
}

/* cps_8018d4fc — clean CPS transliteration of br2eu_func_8018D4FC (guest 0x8018d4fc)
 * from SLES_017.22_full_42.c. */
void cps_8018d4fc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018D508u: goto block_8018D508;
            case 0x8018D514u: goto block_8018D514;
            default: break;
        }
    }
    /* Address: 0x8018D4FC, Size: 60 bytes, Blocks: 3 */

block_8018D4FC:
    cpu->gpr[8] = cpu->gpr[31];
    cpu->gpr[31] = 0x8018D508u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D504: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_8018D508:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[5] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[5]));
    cpu->gpr[31] = 0x8018D514u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D510: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_8018D514:
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = cpu->gpr[3] + 2048;
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    { int64_t result = (int64_t)(int32_t)cpu->gpr[5] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[5]));
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 12;
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[2] + 2048;
    uint32_t _jt_8018D530 = cpu->gpr[8];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 12;
    psx_check_interrupts_at(cpu, _jt_8018D530);
    cpu->pc = _jt_8018D530; return;  /* CPS: jr cpu->gpr[8] */
}

/* cps_8018d484 — clean CPS transliteration of br2eu_func_8018D484 (guest 0x8018d484)
 * from SLES_017.22_full_42.c. */
void cps_8018d484(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018D48Cu: goto block_8018D48C;
            default: break;
        }
    }
    /* Address: 0x8018D484, Size: 24 bytes, Blocks: 2 */

block_8018D484:
    int _bc_8018D484 = (cpu->gpr[5] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[9] = cpu->gpr[31];
    if (_bc_8018D484) {
        psx_check_interrupts_at(cpu, 0x8018D49Cu);
        cpu->pc = 0x8018D49Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D48Cu);
        goto block_8018D48C;  /* not taken */
    }

block_8018D48C:
    cpu->gpr[12] = cpu->gpr[0];
    cpu->gpr[3] = cpu->gpr[0];
    uint32_t _jt_8018D494 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, _jt_8018D494);
    cpu->pc = _jt_8018D494; return;  /* CPS: jr $ra */
}

/* cps_8018b0a0 — clean CPS transliteration of br2eu_func_8018B0A0 (guest 0x8018b0a0)
 * from SLES_017.22_full_41.c. */
void cps_8018b0a0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018B0ACu: goto block_8018B0AC;
            case 0x8018B0B8u: goto block_8018B0B8;
            case 0x8018B0C4u: goto block_8018B0C4;
            default: break;
        }
    }
    /* Address: 0x8018B0A0, Size: 96 bytes, Blocks: 4 */

block_8018B0A0:
    cpu->gpr[8] = cpu->gpr[31];
    cpu->gpr[31] = 0x8018B0ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[9] = 4096;
    psx_check_interrupts_at(cpu, 0x8018D244u);
    cpu->pc = 0x8018D244u; return;  /* CPS jal -> 0x8018D244 */

block_8018B0AC:
    cpu->gpr[10] = cpu->gpr[2];
    cpu->gpr[31] = 0x8018B0B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_8018B0B8:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[10] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[10]));
    cpu->gpr[31] = 0x8018B0C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018B0C0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_8018B0C4:
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 12;
    cpu->gpr[3] = cpu->gpr[3] << 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[10] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[10]));
    cpu->gpr[3] = cpu->gpr[9] | cpu->gpr[3];
    cpu->write_word(cpu->gpr[6], cpu->gpr[3]);
    cpu->write_word(cpu->gpr[6] + 4, cpu->gpr[0]);
    cpu->write_word(cpu->gpr[6] + 8, cpu->gpr[0]);
    cpu->write_half(cpu->gpr[6] + 16, (uint16_t)cpu->gpr[9]);
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 12;
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->write_word(cpu->gpr[6] + 12, cpu->gpr[3]);
    uint32_t _jt_8018B0F8 = cpu->gpr[8];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_8018B0F8);
    cpu->pc = _jt_8018B0F8; return;  /* CPS: jr cpu->gpr[8] */
}

/* cps_8018de6c — clean CPS transliteration of br2eu_func_8018DE6C (guest 0x8018de6c)
 * from SLES_017.22_full_42.c. */
void cps_8018de6c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018DE94u: goto block_8018DE94;
            case 0x8018DEA0u: goto block_8018DEA0;
            case 0x8018DEB8u: goto block_8018DEB8;
            case 0x8018DEC0u: goto block_8018DEC0;
            case 0x8018DED4u: goto block_8018DED4;
            case 0x8018DEDCu: goto block_8018DEDC;
            default: break;
        }
    }
    /* Address: 0x8018DE6C, Size: 136 bytes, Blocks: 7 */

block_8018DE6C:
    cpu->gpr[8] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[8] = cpu->gpr[8] + 18344;
    cpu->gpr[9] = 0x1000;
    cpu->write_word(cpu->gpr[5] + 8, cpu->gpr[9]);
    cpu->gpr[2] = cpu->gpr[4] & 0x03FF;  /* 0x8018DE7C: 0x308203FF */
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[4] & 0x0400;  /* 0x8018DE88: 0x30890400 */
    int _bc_8018DE8C = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 2048;
    if (_bc_8018DE8C) {
        psx_check_interrupts_at(cpu, 0x8018DEA0u);
        goto block_8018DEA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DE94u);
        goto block_8018DE94;  /* not taken */
    }

block_8018DE94:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DE94: 0x00431026 */
    cpu->gpr[3] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DE98: 0x00431826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DE9C: 0x00431026 */
    /* fall through to block_8018DEA0 */

block_8018DEA0:
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[8] + cpu->gpr[3];
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DEAC: 0x30890800 */
    int _bc_8018DEB0 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    if (_bc_8018DEB0) {
        psx_check_interrupts_at(cpu, 0x8018DEC0u);
        goto block_8018DEC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DEB8u);
        goto block_8018DEB8;  /* not taken */
    }

block_8018DEB8:
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[2] & 0xFFFF;  /* 0x8018DEBC: 0x3042FFFF */
    /* fall through to block_8018DEC0 */

block_8018DEC0:
    cpu->gpr[3] = cpu->read_half(cpu->gpr[3]);
    cpu->gpr[8] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DEC8: 0x30890800 */
    int _bc_8018DECC = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[8] & 0xFFFF;  /* 0x8018DED0: 0x3108FFFF */
    if (_bc_8018DECC) {
        psx_check_interrupts_at(cpu, 0x8018DEDCu);
        goto block_8018DEDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DED4u);
        goto block_8018DED4;  /* not taken */
    }

block_8018DED4:
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[3];
    cpu->gpr[3] = cpu->gpr[3] & 0xFFFF;  /* 0x8018DED8: 0x3063FFFF */
    /* fall through to block_8018DEDC */

block_8018DEDC:
    cpu->write_word(cpu->gpr[5], cpu->gpr[3]);
    cpu->write_word(cpu->gpr[5] + 4, cpu->gpr[2]);
    cpu->write_word(cpu->gpr[5] + 12, cpu->gpr[8]);
    cpu->write_half(cpu->gpr[5] + 16, (uint16_t)cpu->gpr[3]);
    uint32_t _jt_8018DEEC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, _jt_8018DEEC);
    cpu->pc = _jt_8018DEEC; return;  /* CPS: jr $ra */
}

/* cps_8018df00 — clean CPS transliteration of br2eu_func_8018DF00 (guest 0x8018df00)
 * from SLES_017.22_full_42.c. */
void cps_8018df00(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018DF20u: goto block_8018DF20;
            case 0x8018DF2Cu: goto block_8018DF2C;
            case 0x8018DF44u: goto block_8018DF44;
            case 0x8018DF48u: goto block_8018DF48;
            case 0x8018DF60u: goto block_8018DF60;
            case 0x8018DF6Cu: goto block_8018DF6C;
            case 0x8018DF84u: goto block_8018DF84;
            case 0x8018DF88u: goto block_8018DF88;
            case 0x8018DFA0u: goto block_8018DFA0;
            case 0x8018DFA4u: goto block_8018DFA4;
            case 0x8018DFC8u: goto block_8018DFC8;
            case 0x8018DFCCu: goto block_8018DFCC;
            default: break;
        }
    }
    /* Address: 0x8018DF00, Size: 292 bytes, Blocks: 13 */

block_8018DF00:
    cpu->gpr[8] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[8] = cpu->gpr[8] + 18344;
    cpu->gpr[2] = cpu->gpr[4] & 0x03FF;  /* 0x8018DF08: 0x308203FF */
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[15] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[4] & 0x0400;  /* 0x8018DF14: 0x30890400 */
    int _bc_8018DF18 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[15] = cpu->gpr[15] + 2048;
    if (_bc_8018DF18) {
        psx_check_interrupts_at(cpu, 0x8018DF2Cu);
        goto block_8018DF2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DF20u);
        goto block_8018DF20;  /* not taken */
    }

block_8018DF20:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[15];  /* 0x8018DF20: 0x004F1026 */
    cpu->gpr[15] = cpu->gpr[2] ^ cpu->gpr[15];  /* 0x8018DF24: 0x004F7826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[15];  /* 0x8018DF28: 0x004F1026 */
    /* fall through to block_8018DF2C */

block_8018DF2C:
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[10] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2]);
    cpu->gpr[15] = cpu->gpr[8] + cpu->gpr[15];
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DF38: 0x30890800 */
    int _bc_8018DF3C = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    if (_bc_8018DF3C) {
        psx_check_interrupts_at(cpu, 0x8018DF48u);
        goto block_8018DF48;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DF44u);
        goto block_8018DF44;  /* not taken */
    }

block_8018DF44:
    cpu->gpr[10] = cpu->gpr[0] - cpu->gpr[10];
    /* fall through to block_8018DF48 */

block_8018DF48:
    cpu->gpr[2] = cpu->gpr[5] & 0x03FF;  /* 0x8018DF48: 0x30A203FF */
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[5] & 0x0400;  /* 0x8018DF54: 0x30A90400 */
    int _bc_8018DF58 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 2048;
    if (_bc_8018DF58) {
        psx_check_interrupts_at(cpu, 0x8018DF6Cu);
        goto block_8018DF6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DF60u);
        goto block_8018DF60;  /* not taken */
    }

block_8018DF60:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DF60: 0x00431026 */
    cpu->gpr[3] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DF64: 0x00431826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018DF68: 0x00431026 */
    /* fall through to block_8018DF6C */

block_8018DF6C:
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[12] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[8] + cpu->gpr[3];
    cpu->gpr[9] = cpu->gpr[5] & 0x0800;  /* 0x8018DF78: 0x30A90800 */
    int _bc_8018DF7C = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + 1024;
    if (_bc_8018DF7C) {
        psx_check_interrupts_at(cpu, 0x8018DF88u);
        goto block_8018DF88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DF84u);
        goto block_8018DF84;  /* not taken */
    }

block_8018DF84:
    cpu->gpr[12] = cpu->gpr[0] - cpu->gpr[12];
    /* fall through to block_8018DF88 */

block_8018DF88:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[12] * (int64_t)(int32_t)cpu->gpr[10]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[12]));
    cpu->gpr[13] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[3]);
    cpu->gpr[14] = cpu->gpr[10] << 16;
    cpu->gpr[9] = cpu->gpr[5] & 0x0800;  /* 0x8018DF94: 0x30A90800 */
    int _bc_8018DF98 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[10] = cpu->gpr[0] - cpu->gpr[10];
    if (_bc_8018DF98) {
        psx_check_interrupts_at(cpu, 0x8018DFA4u);
        goto block_8018DFA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DFA0u);
        goto block_8018DFA0;  /* not taken */
    }

block_8018DFA0:
    cpu->gpr[13] = cpu->gpr[0] - cpu->gpr[13];
    /* fall through to block_8018DFA4 */

block_8018DFA4:
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 12;
    cpu->gpr[8] = cpu->gpr[2] << 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[13] * (int64_t)(int32_t)cpu->gpr[10]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[13]));
    cpu->gpr[11] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[15]);
    cpu->gpr[10] = cpu->gpr[12] & 0xFFFF;  /* 0x8018DFB8: 0x318AFFFF */
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018DFBC: 0x30890800 */
    int _bc_8018DFC0 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[10] | cpu->gpr[8];
    if (_bc_8018DFC0) {
        psx_check_interrupts_at(cpu, 0x8018DFCCu);
        goto block_8018DFCC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018DFC8u);
        goto block_8018DFC8;  /* not taken */
    }

block_8018DFC8:
    cpu->gpr[11] = cpu->gpr[0] - cpu->gpr[11];
    /* fall through to block_8018DFCC */

block_8018DFCC:
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 12;
    cpu->gpr[9] = cpu->gpr[2] << 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[12] * (int64_t)(int32_t)cpu->gpr[11]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[12]));
    cpu->write_word(cpu->gpr[6] + 4, cpu->gpr[3]);
    cpu->gpr[2] = cpu->gpr[11] & 0xFFFF;  /* 0x8018DFE0: 0x3162FFFF */
    cpu->gpr[3] = cpu->gpr[2] | cpu->gpr[9];
    cpu->write_word(cpu->gpr[6] + 8, cpu->gpr[3]);
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 12;
    cpu->gpr[2] = cpu->gpr[13] & 0xFFFF;  /* 0x8018DFF4: 0x31A2FFFF */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[13] * (int64_t)(int32_t)cpu->gpr[11]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[13]));
    cpu->write_word(cpu->gpr[6], cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[3] & 0xFFFF;  /* 0x8018E004: 0x3062FFFF */
    cpu->gpr[3] = cpu->gpr[14] | cpu->gpr[2];
    cpu->write_word(cpu->gpr[6] + 12, cpu->gpr[3]);
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 12;
    cpu->write_half(cpu->gpr[6] + 16, (uint16_t)cpu->gpr[3]);
    uint32_t _jt_8018E01C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_8018E01C);
    cpu->pc = _jt_8018E01C; return;  /* CPS: jr $ra */
}

/* cps_8018e028 — clean CPS transliteration of br2eu_func_8018E028 (guest 0x8018e028)
 * from SLES_017.22_full_42.c. */
void cps_8018e028(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018E050u: goto block_8018E050;
            case 0x8018E05Cu: goto block_8018E05C;
            case 0x8018E074u: goto block_8018E074;
            case 0x8018E078u: goto block_8018E078;
            case 0x8018E08Cu: goto block_8018E08C;
            case 0x8018E094u: goto block_8018E094;
            default: break;
        }
    }
    /* Address: 0x8018E028, Size: 136 bytes, Blocks: 7 */

block_8018E028:
    cpu->gpr[8] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[8] = cpu->gpr[8] + 18344;
    cpu->gpr[9] = 0x1000;
    cpu->write_half(cpu->gpr[5] + 16, (uint16_t)cpu->gpr[9]);
    cpu->gpr[2] = cpu->gpr[4] & 0x03FF;  /* 0x8018E038: 0x308203FF */
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[2];
    cpu->gpr[9] = cpu->gpr[4] & 0x0400;  /* 0x8018E044: 0x30890400 */
    int _bc_8018E048 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 2048;
    if (_bc_8018E048) {
        psx_check_interrupts_at(cpu, 0x8018E05Cu);
        goto block_8018E05C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E050u);
        goto block_8018E050;  /* not taken */
    }

block_8018E050:
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018E050: 0x00431026 */
    cpu->gpr[3] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018E054: 0x00431826 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[3];  /* 0x8018E058: 0x00431026 */
    /* fall through to block_8018E05C */

block_8018E05C:
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[8] + cpu->gpr[3];
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018E068: 0x30890800 */
    int _bc_8018E06C = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    if (_bc_8018E06C) {
        psx_check_interrupts_at(cpu, 0x8018E078u);
        goto block_8018E078;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E074u);
        goto block_8018E074;  /* not taken */
    }

block_8018E074:
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    /* fall through to block_8018E078 */

block_8018E078:
    cpu->gpr[3] = cpu->read_half(cpu->gpr[3]);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[9] = cpu->gpr[4] & 0x0800;  /* 0x8018E080: 0x30890800 */
    int _bc_8018E084 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[0] - cpu->gpr[2];
    if (_bc_8018E084) {
        psx_check_interrupts_at(cpu, 0x8018E094u);
        goto block_8018E094;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018E08Cu);
        goto block_8018E08C;  /* not taken */
    }

block_8018E08C:
    cpu->gpr[3] = cpu->gpr[0] - cpu->gpr[3];
    cpu->gpr[3] = cpu->gpr[3] & 0xFFFF;  /* 0x8018E090: 0x3063FFFF */
    /* fall through to block_8018E094 */

block_8018E094:
    cpu->gpr[9] = cpu->gpr[3] | cpu->gpr[8];
    cpu->write_word(cpu->gpr[5], cpu->gpr[9]);
    cpu->write_word(cpu->gpr[5] + 4, cpu->gpr[2]);
    cpu->write_word(cpu->gpr[5] + 8, cpu->gpr[3]);
    cpu->write_word(cpu->gpr[5] + 12, cpu->gpr[0]);
    uint32_t _jt_8018E0A8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, _jt_8018E0A8);
    cpu->pc = _jt_8018E0A8; return;  /* CPS: jr $ra */
}

/* cps_80133aa0 — clean CPS transliteration of br2eu_func_80133AA0 (guest 0x80133aa0)
 * from SLES_017.22_full_06.c. */
void cps_80133aa0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80133B18u: goto block_80133B18;
            case 0x80133B20u: goto block_80133B20;
            case 0x80133B00u: goto block_80133B00;
            case 0x80133B04u: goto block_80133B04;
            case 0x80133B2Cu: goto block_80133B2C;
            case 0x80133B50u: goto block_80133B50;
            case 0x80133BC8u: goto block_80133BC8;
            default: break;
        }
    }
    /* Address: 0x80133AA0, Size: 360 bytes, Blocks: 8 */

block_80133AA0:
    cpu->gpr[29] = cpu->gpr[29] + -80;
    cpu->write_word(cpu->gpr[29] + 60, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 64, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 72, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 68, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[16]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17]);
    cpu->gpr[19] = cpu->gpr[4];
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18] + 2);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 2);
    cpu->gpr[5] = cpu->gpr[7];
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18] + 4);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 4);
    cpu->gpr[7] = cpu->gpr[7] << 16;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    int _bc_80133AF8 = (cpu->gpr[7] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[2]);
    if (_bc_80133AF8) {
        psx_check_interrupts_at(cpu, 0x80133B04u);
        goto block_80133B04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80133B00u);
        goto block_80133B00;  /* not taken */
    }

block_80133B00:
    cpu->gpr[5] = 1;
    /* fall through to block_80133B04 */

block_80133B04:
    cpu->gpr[4] = cpu->gpr[29] + 16;
    cpu->gpr[2] = cpu->gpr[5] << 16;
    cpu->gpr[16] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[31] = 0x80133B18u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 32;
    psx_check_interrupts_at(cpu, 0x80194630u);
    cpu->pc = 0x80194630u; return;  /* CPS jal -> 0x80194630 */

block_80133B18:
    cpu->gpr[31] = 0x80133B20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80194500u);
    cpu->pc = 0x80194500u; return;  /* CPS jal -> 0x80194500 */

block_80133B20:
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[16]) ? 1 : 0;  /* 0x80133B20: 0x0050102A */
    int _bc_80133B24 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80133B28: 0x00000000 */
    if (_bc_80133B24) {
        psx_check_interrupts_at(cpu, 0x80133B50u);
        goto block_80133B50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80133B2Cu);
        goto block_80133B2C;  /* not taken */
    }

block_80133B2C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[18]);
    /* nop */  /* 0x80133B30: 0x00000000 */
    cpu->write_half(cpu->gpr[17], (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[18] + 2);
    /* nop */  /* 0x80133B3C: 0x00000000 */
    cpu->write_half(cpu->gpr[17] + 2, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[18] + 4);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[17] + 4, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80133BC8u);
    goto block_80133BC8;  /* j */

block_80133B50:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 32);
    /* nop */  /* 0x80133B54: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[16]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[8] >> 12;
    cpu->write_half(cpu->gpr[29] + 48, (uint16_t)cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17]);
    /* nop */  /* 0x80133B6C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17], (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 36);
    /* nop */  /* 0x80133B7C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[16]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[8] >> 12;
    cpu->write_half(cpu->gpr[29] + 50, (uint16_t)cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 2);
    /* nop */  /* 0x80133B94: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 2, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 40);
    /* nop */  /* 0x80133BA4: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[16]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[8] >> 12;
    cpu->write_half(cpu->gpr[29] + 52, (uint16_t)cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 4);
    /* nop */  /* 0x80133BBC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 4, (uint16_t)cpu->gpr[2]);
    /* fall through to block_80133BC8 */

block_80133BC8:
    cpu->gpr[2] = cpu->gpr[19];
    cpu->gpr[9] = br2_lwl(cpu, cpu->gpr[29] + 51, cpu->gpr[9]);
    cpu->gpr[9] = br2_lwr(cpu, cpu->gpr[29] + 48, cpu->gpr[9]);
    cpu->gpr[10] = br2_lwl(cpu, cpu->gpr[29] + 55, cpu->gpr[10]);
    cpu->gpr[10] = br2_lwr(cpu, cpu->gpr[29] + 52, cpu->gpr[10]);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 72);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 68);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 64);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 60);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 56);
    uint32_t _jt_80133C00 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 80;
    psx_check_interrupts_at(cpu, _jt_80133C00);
    cpu->pc = _jt_80133C00; return;  /* CPS: jr $ra */
}
