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
void cps_80195be0(void);
void cps_80195bf0(void);
void cps_80195c10(void);
void cps_80196370(void);
void cps_801985c4(void);
void cps_80198e2c(void);
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
void cps_8019fb9c(void);
void cps_8015ace4(void);
void cps_801a972c(void);
void cps_80168838(void);

/* cps_80195be0 — clean CPS transliteration of br2eu_func_80195BE0 (guest 0x80195be0)
 * from SLES_017.22_full_45.c. */
void cps_80195be0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80195BE0, Size: 16 bytes, Blocks: 1 */

block_80195BE0:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + -14252);
    uint32_t _jt_80195BE8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80195BEC: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80195BE8);
    cpu->pc = _jt_80195BE8; return;  /* CPS: jr $ra */
}

/* cps_80195bf0 — clean CPS transliteration of br2eu_func_80195BF0 (guest 0x80195bf0)
 * from SLES_017.22_full_45.c. */
void cps_80195bf0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80195BF0, Size: 12 bytes, Blocks: 1 */

block_80195BF0:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + -14236);
    uint32_t _jt_80195BF8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80195BFC: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80195BF8);
    cpu->pc = _jt_80195BF8; return;  /* CPS: jr $ra */
}

/* cps_80195c10 — clean CPS transliteration of br2eu_func_80195C10 (guest 0x80195c10)
 * from SLES_017.22_full_45.c. */
void cps_80195c10(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80195C10, Size: 12 bytes, Blocks: 1 */

block_80195C10:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    uint32_t _jt_80195C14 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + -14240;
    psx_check_interrupts_at(cpu, _jt_80195C14);
    cpu->pc = _jt_80195C14; return;  /* CPS: jr $ra */
}

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

/* cps_8019fb9c — clean CPS transliteration of br2eu_func_8019FB9C (guest 0x8019fb9c)
 * from SLES_017.22_full_48.c. */
void cps_8019fb9c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019FBE8u: goto block_8019FBE8;
            case 0x8019FBFCu: goto block_8019FBFC;
            case 0x8019FC08u: goto block_8019FC08;
            case 0x8019FC2Cu: goto block_8019FC2C;
            case 0x8019FC3Cu: goto block_8019FC3C;
            case 0x8019FC5Cu: goto block_8019FC5C;
            case 0x8019FC6Cu: goto block_8019FC6C;
            case 0x8019FC88u: goto block_8019FC88;
            case 0x8019FCB0u: goto block_8019FCB0;
            case 0x8019FD24u: goto block_8019FD24;
            case 0x8019FD74u: goto block_8019FD74;
            case 0x8019FBDCu: goto block_8019FBDC;
            case 0x8019FBF0u: goto block_8019FBF0;
            case 0x8019FC34u: goto block_8019FC34;
            case 0x8019FC44u: goto block_8019FC44;
            case 0x8019FC4Cu: goto block_8019FC4C;
            case 0x8019FC50u: goto block_8019FC50;
            case 0x8019FC54u: goto block_8019FC54;
            case 0x8019FC64u: goto block_8019FC64;
            case 0x8019FC7Cu: goto block_8019FC7C;
            case 0x8019FC90u: goto block_8019FC90;
            case 0x8019FCC0u: goto block_8019FCC0;
            case 0x8019FCD0u: goto block_8019FCD0;
            case 0x8019FCE8u: goto block_8019FCE8;
            case 0x8019FCF0u: goto block_8019FCF0;
            case 0x8019FD00u: goto block_8019FD00;
            case 0x8019FD14u: goto block_8019FD14;
            case 0x8019FD34u: goto block_8019FD34;
            case 0x8019FD3Cu: goto block_8019FD3C;
            case 0x8019FD44u: goto block_8019FD44;
            case 0x8019FD54u: goto block_8019FD54;
            case 0x8019FD5Cu: goto block_8019FD5C;
            case 0x8019FD60u: goto block_8019FD60;
            case 0x8019FD6Cu: goto block_8019FD6C;
            case 0x8019FD78u: goto block_8019FD78;
            default: break;
        }
    }
    /* Address: 0x8019FB9C, Size: 508 bytes, Blocks: 36 */

block_8019FB9C:
    cpu->gpr[29] = cpu->gpr[29] + -80;
    cpu->write_word(cpu->gpr[29] + 68, cpu->gpr[19]);
    cpu->gpr[19] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 60, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 72, cpu->gpr[20]);
    cpu->gpr[20] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[16]);
    cpu->gpr[16] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[16] = cpu->gpr[16] + -5152;
    cpu->write_word(cpu->gpr[29] + 76, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 64, cpu->gpr[18]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16]);
    /* nop */  /* 0x8019FBD0: 0x00000000 */
    int _bc_8019FBD4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[0];
    if (_bc_8019FBD4) {
        psx_check_interrupts_at(cpu, 0x8019FBF0u);
        goto block_8019FBF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FBDCu);
        goto block_8019FBDC;  /* not taken */
    }

block_8019FBDC:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x8019FBE8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 24204;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_8019FBE8:
    /* delay slot (always executes) */
    cpu->gpr[2] = -1;
    psx_check_interrupts_at(cpu, 0x8019FD78u);
    goto block_8019FD78;  /* j */

block_8019FBF0:
    cpu->gpr[4] = cpu->gpr[19];
    cpu->gpr[31] = 0x8019FBFCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[29] + 16;
    psx_check_interrupts_at(cpu, 0x801A01BCu);
    cpu->pc = 0x801A01BCu; return;  /* CPS jal -> 0x801A01BC */

block_8019FBFC:
    cpu->gpr[4] = cpu->gpr[29] + 16;
    cpu->gpr[31] = 0x8019FC08u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[17];
    psx_check_interrupts_at(cpu, 0x801A0530u);
    cpu->pc = 0x801A0530u; return;  /* CPS jal -> 0x801A0530 */

block_8019FC08:
    cpu->gpr[4] = cpu->gpr[29] + 16;
    cpu->gpr[5] = 1;
    cpu->gpr[2] = 1;
    cpu->gpr[6] = cpu->read_word(cpu->gpr[16] + 16);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16] + 12);
    cpu->gpr[2] = cpu->gpr[2] << (cpu->gpr[6] & 0x1F);
    cpu->gpr[3] = cpu->gpr[3] | cpu->gpr[2];
    cpu->gpr[31] = 0x8019FC2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16] + 12, cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x801A0220u);
    cpu->pc = 0x801A0220u; return;  /* CPS jal -> 0x801A0220 */

block_8019FC2C:
    int _bc_8019FC2C = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[20] = cpu->gpr[20] << 16;
    if (_bc_8019FC2C) {
        psx_check_interrupts_at(cpu, 0x8019FC4Cu);
        goto block_8019FC4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FC34u);
        goto block_8019FC34;  /* not taken */
    }

block_8019FC34:
    cpu->gpr[31] = 0x8019FC3Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801A0250u);
    cpu->pc = 0x801A0250u; return;  /* CPS jal -> 0x801A0250 */

block_8019FC3C:
    /* delay slot (always executes) */
    cpu->gpr[2] = 6;
    psx_check_interrupts_at(cpu, 0x8019FD78u);
    goto block_8019FD78;  /* j */

block_8019FC44:
    /* delay slot (always executes) */
    cpu->gpr[2] = 7;
    psx_check_interrupts_at(cpu, 0x8019FD78u);
    goto block_8019FD78;  /* j */

block_8019FC4C:
    cpu->gpr[17] = 2;
    /* fall through to block_8019FC50 */

block_8019FC50:
    cpu->gpr[4] = cpu->gpr[29] + 16;
    /* fall through to block_8019FC54 */

block_8019FC54:
    cpu->gpr[31] = 0x8019FC5Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[20] | 0x0200;
    psx_check_interrupts_at(cpu, 0x801A0220u);
    cpu->pc = 0x801A0220u; return;  /* CPS jal -> 0x801A0220 */

block_8019FC5C:
    int _bc_8019FC5C = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FC60: 0x00000000 */
    if (_bc_8019FC5C) {
        psx_check_interrupts_at(cpu, 0x8019FD6Cu);
        goto block_8019FD6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FC64u);
        goto block_8019FC64;  /* not taken */
    }

block_8019FC64:
    cpu->gpr[31] = 0x8019FC6Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x8019FA6Cu);
    cpu->pc = 0x8019FA6Cu; return;  /* CPS jal -> 0x8019FA6C */

block_8019FC6C:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16]);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    int _bc_8019FC74 = ((int32_t)cpu->gpr[3] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -5064, cpu->gpr[2]);
    if (_bc_8019FC74) {
        psx_check_interrupts_at(cpu, 0x8019FC90u);
        goto block_8019FC90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FC7Cu);
        goto block_8019FC7C;  /* not taken */
    }

block_8019FC7C:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x8019FC88u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 23784;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_8019FC88:
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FC8C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019FCB0u);
    goto block_8019FCB0;  /* j */

block_8019FC90:
    cpu->gpr[4] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[4] = cpu->gpr[4] + -5376;
    cpu->write_word(cpu->gpr[16], cpu->gpr[17]);
    cpu->write_word(cpu->gpr[16] + 4, cpu->gpr[0]);
    cpu->write_word(cpu->gpr[16] + 8, cpu->gpr[0]);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[31] = 0x8019FCB0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -5136, cpu->gpr[19]);
    psx_check_interrupts_at(cpu, 0x801A0A10u);
    cpu->pc = 0x801A0A10u; return;  /* CPS jal -> 0x801A0A10 */

block_8019FCB0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16]);
    /* nop */  /* 0x8019FCB4: 0x00000000 */
    int _bc_8019FCB8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FCBC: 0x00000000 */
    if (_bc_8019FCB8) {
        psx_check_interrupts_at(cpu, 0x8019FCD0u);
        goto block_8019FCD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FCC0u);
        goto block_8019FCC0;  /* not taken */
    }

block_8019FCC0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 8);
    /* nop */  /* 0x8019FCC4: 0x00000000 */
    int _bc_8019FCC8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FCCC: 0x00000000 */
    if (_bc_8019FCC8) {
        psx_check_interrupts_at(cpu, 0x8019FD14u);
        goto block_8019FD14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FCD0u);
        goto block_8019FCD0;  /* not taken */
    }

block_8019FCD0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 4);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 8);
    /* nop */  /* 0x8019FCDC: 0x00000000 */
    int _bc_8019FCE0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FCE4: 0x00000000 */
    if (_bc_8019FCE0) {
        psx_check_interrupts_at(cpu, 0x8019FD00u);
        goto block_8019FD00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FCE8u);
        goto block_8019FCE8;  /* not taken */
    }

block_8019FCE8:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->gpr[3] + -5144;
    /* fall through to block_8019FCF0 */

block_8019FCF0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8019FCF4: 0x00000000 */
    int _bc_8019FCF8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FCFC: 0x00000000 */
    if (_bc_8019FCF8) {
        psx_check_interrupts_at(cpu, 0x8019FCF0u);
        goto block_8019FCF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FD00u);
        goto block_8019FD00;  /* not taken */
    }

block_8019FD00:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->gpr[2] + -5164;
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2]);
    cpu->write_word(cpu->gpr[16] + 8, cpu->gpr[0]);
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[2]);
    /* fall through to block_8019FD14 */

block_8019FD14:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -5064);
    cpu->gpr[31] = 0x8019FD24u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FD20: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019FA6Cu);
    cpu->pc = 0x8019FA6Cu; return;  /* CPS jal -> 0x8019FA6C */

block_8019FD24:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[29] + 48);
    /* nop */  /* 0x8019FD28: 0x00000000 */
    int _bc_8019FD2C = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 3;
    if (_bc_8019FD2C) {
        psx_check_interrupts_at(cpu, 0x8019FC44u);
        goto block_8019FC44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FD34u);
        goto block_8019FD34;  /* not taken */
    }

block_8019FD34:
    int _bc_8019FD34 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[29] + 16;
    if (_bc_8019FD34) {
        psx_check_interrupts_at(cpu, 0x8019FC54u);
        goto block_8019FC54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FD3Cu);
        goto block_8019FD3C;  /* not taken */
    }

block_8019FD3C:
    int _bc_8019FD3C = (cpu->gpr[3] != cpu->gpr[17]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FD40: 0x00000000 */
    if (_bc_8019FD3C) {
        psx_check_interrupts_at(cpu, 0x8019FD54u);
        goto block_8019FD54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FD44u);
        goto block_8019FD44;  /* not taken */
    }

block_8019FD44:
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < 4) ? 1 : 0;  /* 0x8019FD48: 0x2A420004 */
    int _bc_8019FD4C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FD50: 0x00000000 */
    if (_bc_8019FD4C) {
        psx_check_interrupts_at(cpu, 0x8019FC50u);
        goto block_8019FC50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FD54u);
        goto block_8019FD54;  /* not taken */
    }

block_8019FD54:
    int _bc_8019FD54 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 5;
    if (_bc_8019FD54) {
        psx_check_interrupts_at(cpu, 0x8019FD60u);
        goto block_8019FD60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019FD5Cu);
        goto block_8019FD5C;  /* not taken */
    }

block_8019FD5C:
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[2]);
    /* fall through to block_8019FD60 */

block_8019FD60:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 48);
    /* delay slot (always executes) */
    /* nop */  /* 0x8019FD68: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019FD78u);
    goto block_8019FD78;  /* j */

block_8019FD6C:
    cpu->gpr[31] = 0x8019FD74u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801A0250u);
    cpu->pc = 0x801A0250u; return;  /* CPS jal -> 0x801A0250 */

block_8019FD74:
    cpu->gpr[2] = cpu->gpr[0];
    /* fall through to block_8019FD78 */

block_8019FD78:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 76);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 72);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 68);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 64);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 60);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 56);
    uint32_t _jt_8019FD90 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 80;
    psx_check_interrupts_at(cpu, _jt_8019FD90);
    cpu->pc = _jt_8019FD90; return;  /* CPS: jr $ra */
}

/* cps_8015ace4 — clean CPS transliteration of br2eu_func_8015ACE4 (guest 0x8015ace4)
 * from SLES_017.22_full_21.c. */
void cps_8015ace4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8015AD54u: goto block_8015AD54;
            case 0x8015AD68u: goto block_8015AD68;
            case 0x8015AF4Cu: goto block_8015AF4C;
            case 0x8015AF70u: goto block_8015AF70;
            case 0x8015AF98u: goto block_8015AF98;
            case 0x8015AFA8u: goto block_8015AFA8;
            case 0x8015B164u: goto block_8015B164;
            case 0x8015AD14u: goto block_8015AD14;
            case 0x8015AD1Cu: goto block_8015AD1C;
            case 0x8015AD24u: goto block_8015AD24;
            case 0x8015AD2Cu: goto block_8015AD2C;
            case 0x8015AD34u: goto block_8015AD34;
            case 0x8015AD3Cu: goto block_8015AD3C;
            case 0x8015AD44u: goto block_8015AD44;
            case 0x8015AD4Cu: goto block_8015AD4C;
            case 0x8015AD5Cu: goto block_8015AD5C;
            case 0x8015ADD0u: goto block_8015ADD0;
            case 0x8015ADD4u: goto block_8015ADD4;
            case 0x8015ADECu: goto block_8015ADEC;
            case 0x8015ADF0u: goto block_8015ADF0;
            case 0x8015AE08u: goto block_8015AE08;
            case 0x8015AE0Cu: goto block_8015AE0C;
            case 0x8015AE24u: goto block_8015AE24;
            case 0x8015AE28u: goto block_8015AE28;
            case 0x8015AE40u: goto block_8015AE40;
            case 0x8015AE44u: goto block_8015AE44;
            case 0x8015AE58u: goto block_8015AE58;
            case 0x8015AE5Cu: goto block_8015AE5C;
            case 0x8015AEA4u: goto block_8015AEA4;
            case 0x8015AEA8u: goto block_8015AEA8;
            case 0x8015AEC0u: goto block_8015AEC0;
            case 0x8015AEC4u: goto block_8015AEC4;
            case 0x8015AF08u: goto block_8015AF08;
            case 0x8015AF0Cu: goto block_8015AF0C;
            case 0x8015AF24u: goto block_8015AF24;
            case 0x8015AF28u: goto block_8015AF28;
            case 0x8015AF40u: goto block_8015AF40;
            case 0x8015AF44u: goto block_8015AF44;
            case 0x8015AF50u: goto block_8015AF50;
            case 0x8015AF64u: goto block_8015AF64;
            case 0x8015AF78u: goto block_8015AF78;
            case 0x8015AF8Cu: goto block_8015AF8C;
            case 0x8015AFA0u: goto block_8015AFA0;
            case 0x8015AFB0u: goto block_8015AFB0;
            case 0x8015AFB8u: goto block_8015AFB8;
            case 0x8015AFCCu: goto block_8015AFCC;
            case 0x8015AFD0u: goto block_8015AFD0;
            case 0x8015AFE0u: goto block_8015AFE0;
            case 0x8015AFF0u: goto block_8015AFF0;
            case 0x8015B000u: goto block_8015B000;
            case 0x8015B010u: goto block_8015B010;
            case 0x8015B020u: goto block_8015B020;
            case 0x8015B030u: goto block_8015B030;
            case 0x8015B040u: goto block_8015B040;
            case 0x8015B050u: goto block_8015B050;
            case 0x8015B060u: goto block_8015B060;
            case 0x8015B070u: goto block_8015B070;
            case 0x8015B0B0u: goto block_8015B0B0;
            case 0x8015B0C0u: goto block_8015B0C0;
            case 0x8015B0D0u: goto block_8015B0D0;
            case 0x8015B0E0u: goto block_8015B0E0;
            case 0x8015B104u: goto block_8015B104;
            case 0x8015B114u: goto block_8015B114;
            case 0x8015B124u: goto block_8015B124;
            case 0x8015B134u: goto block_8015B134;
            case 0x8015B144u: goto block_8015B144;
            case 0x8015B154u: goto block_8015B154;
            case 0x8015B15Cu: goto block_8015B15C;
            case 0x8015B170u: goto block_8015B170;
            default: break;
        }
    }
    /* Address: 0x8015ACE4, Size: 1196 bytes, Blocks: 70 */

block_8015ACE4:
    cpu->gpr[29] = cpu->gpr[29] + -112;
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 100, cpu->gpr[19]);
    cpu->gpr[19] = cpu->gpr[0];
    cpu->write_word(cpu->gpr[29] + 92, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[19];
    cpu->write_word(cpu->gpr[29] + 96, cpu->gpr[18]);
    cpu->gpr[18] = 1;
    cpu->write_word(cpu->gpr[29] + 104, cpu->gpr[20]);
    cpu->gpr[20] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->write_word(cpu->gpr[29] + 108, cpu->gpr[31]);
    /* fall through to block_8015AD14 */

block_8015AD14:
    int _bc_8015AD14 = (cpu->gpr[17] == cpu->gpr[18]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[17] < 2) ? 1 : 0;  /* 0x8015AD18: 0x2A220002 */
    if (_bc_8015AD14) {
        psx_check_interrupts_at(cpu, 0x8015AF50u);
        goto block_8015AF50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AD1Cu);
        goto block_8015AD1C;  /* not taken */
    }

block_8015AD1C:
    int _bc_8015AD1C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 240;
    if (_bc_8015AD1C) {
        psx_check_interrupts_at(cpu, 0x8015AD34u);
        goto block_8015AD34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AD24u);
        goto block_8015AD24;  /* not taken */
    }

block_8015AD24:
    int _bc_8015AD24 = (cpu->gpr[17] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AD28: 0x00000000 */
    if (_bc_8015AD24) {
        psx_check_interrupts_at(cpu, 0x8015AD4Cu);
        goto block_8015AD4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AD2Cu);
        goto block_8015AD2C;  /* not taken */
    }

block_8015AD2C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AD30: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015B15Cu);
    goto block_8015B15C;  /* j */

block_8015AD34:
    int _bc_8015AD34 = (cpu->gpr[17] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 241;
    if (_bc_8015AD34) {
        psx_check_interrupts_at(cpu, 0x8015AFB0u);
        goto block_8015AFB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AD3Cu);
        goto block_8015AD3C;  /* not taken */
    }

block_8015AD3C:
    int _bc_8015AD3C = (cpu->gpr[17] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AD40: 0x00000000 */
    if (_bc_8015AD3C) {
        psx_check_interrupts_at(cpu, 0x8015B154u);
        goto block_8015B154;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AD44u);
        goto block_8015AD44;  /* not taken */
    }

block_8015AD44:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AD48: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015B15Cu);
    goto block_8015B15C;  /* j */

block_8015AD4C:
    cpu->gpr[31] = 0x8015AD54u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AD50: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_8015AD54:
    int _bc_8015AD54 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 2;
    if (_bc_8015AD54) {
        psx_check_interrupts_at(cpu, 0x8015B15Cu);
        goto block_8015B15C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AD5Cu);
        goto block_8015AD5C;  /* not taken */
    }

block_8015AD5C:
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[31] = 0x8015AD68u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x8012CBC0u);
    cpu->pc = 0x8012CBC0u; return;  /* CPS jal -> 0x8012CBC0 */

block_8015AD68:
    cpu->gpr[2] = 0x8016 << 16;  /* 0x80160000 */
    cpu->gpr[2] = cpu->gpr[2] + -20080;
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[2]);
    cpu->gpr[2] = 17;
    cpu->write_half(cpu->gpr[29] + 22, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = -1;
    cpu->write_half(cpu->gpr[29] + 16, (uint16_t)cpu->gpr[0]);
    cpu->write_byte(cpu->gpr[29] + 18, (uint8_t)cpu->gpr[0]);
    cpu->write_byte(cpu->gpr[29] + 19, (uint8_t)cpu->gpr[0]);
    cpu->write_byte(cpu->gpr[29] + 20, (uint8_t)cpu->gpr[0]);
    cpu->write_byte(cpu->gpr[29] + 21, (uint8_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 24, (uint16_t)cpu->gpr[18]);
    cpu->write_half(cpu->gpr[29] + 32, (uint16_t)cpu->gpr[2]);
    cpu->write_half(cpu->gpr[29] + 34, (uint16_t)cpu->gpr[2]);
    cpu->write_half(cpu->gpr[29] + 36, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 38, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 40, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 42, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 44, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 46, (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015ADBC: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 2;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015ADC4: 0x30420001 */
    int _bc_8015ADC8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015ADCC: 0x00000000 */
    if (_bc_8015ADC8) {
        psx_check_interrupts_at(cpu, 0x8015ADD4u);
        goto block_8015ADD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015ADD0u);
        goto block_8015ADD0;  /* not taken */
    }

block_8015ADD0:
    cpu->write_half(cpu->gpr[29] + 36, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015ADD4 */

block_8015ADD4:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015ADD8: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 3;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015ADE0: 0x30420001 */
    int _bc_8015ADE4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015ADE8: 0x00000000 */
    if (_bc_8015ADE4) {
        psx_check_interrupts_at(cpu, 0x8015ADF0u);
        goto block_8015ADF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015ADECu);
        goto block_8015ADEC;  /* not taken */
    }

block_8015ADEC:
    cpu->write_half(cpu->gpr[29] + 38, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015ADF0 */

block_8015ADF0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015ADF4: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 4;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015ADFC: 0x30420001 */
    int _bc_8015AE00 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AE04: 0x00000000 */
    if (_bc_8015AE00) {
        psx_check_interrupts_at(cpu, 0x8015AE0Cu);
        goto block_8015AE0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AE08u);
        goto block_8015AE08;  /* not taken */
    }

block_8015AE08:
    cpu->write_half(cpu->gpr[29] + 40, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AE0C */

block_8015AE0C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AE10: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 5;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015AE18: 0x30420001 */
    int _bc_8015AE1C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AE20: 0x00000000 */
    if (_bc_8015AE1C) {
        psx_check_interrupts_at(cpu, 0x8015AE28u);
        goto block_8015AE28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AE24u);
        goto block_8015AE24;  /* not taken */
    }

block_8015AE24:
    cpu->write_half(cpu->gpr[29] + 42, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AE28 */

block_8015AE28:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AE2C: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 6;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015AE34: 0x30420001 */
    int _bc_8015AE38 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AE3C: 0x00000000 */
    if (_bc_8015AE38) {
        psx_check_interrupts_at(cpu, 0x8015AE44u);
        goto block_8015AE44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AE40u);
        goto block_8015AE40;  /* not taken */
    }

block_8015AE40:
    cpu->write_half(cpu->gpr[29] + 44, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AE44 */

block_8015AE44:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AE48: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 7;
    int _bc_8015AE50 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AE54: 0x00000000 */
    if (_bc_8015AE50) {
        psx_check_interrupts_at(cpu, 0x8015AE5Cu);
        goto block_8015AE5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AE58u);
        goto block_8015AE58;  /* not taken */
    }

block_8015AE58:
    cpu->write_half(cpu->gpr[29] + 46, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AE5C */

block_8015AE5C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 4);
    /* nop */  /* 0x8015AE60: 0x00000000 */
    cpu->write_half(cpu->gpr[29] + 48, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 6);
    /* nop */  /* 0x8015AE6C: 0x00000000 */
    cpu->write_half(cpu->gpr[29] + 50, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 8);
    /* nop */  /* 0x8015AE78: 0x00000000 */
    cpu->write_half(cpu->gpr[29] + 52, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 10);
    cpu->write_half(cpu->gpr[29] + 56, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 58, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 54, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AE94: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015AE98: 0x30420001 */
    int _bc_8015AE9C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AEA0: 0x00000000 */
    if (_bc_8015AE9C) {
        psx_check_interrupts_at(cpu, 0x8015AEA8u);
        goto block_8015AEA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AEA4u);
        goto block_8015AEA4;  /* not taken */
    }

block_8015AEA4:
    cpu->write_half(cpu->gpr[29] + 56, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AEA8 */

block_8015AEA8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AEAC: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015AEB4: 0x30420001 */
    int _bc_8015AEB8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AEBC: 0x00000000 */
    if (_bc_8015AEB8) {
        psx_check_interrupts_at(cpu, 0x8015AEC4u);
        goto block_8015AEC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AEC0u);
        goto block_8015AEC0;  /* not taken */
    }

block_8015AEC0:
    cpu->write_half(cpu->gpr[29] + 58, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AEC4 */

block_8015AEC4:
    cpu->gpr[3] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + 192);
    /* nop */  /* 0x8015AECC: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[3] + 37);
    /* nop */  /* 0x8015AED4: 0x00000000 */
    cpu->write_half(cpu->gpr[29] + 60, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[3] + 11);
    cpu->write_half(cpu->gpr[29] + 64, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 66, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 68, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 62, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AEF4: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 10;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015AEFC: 0x30420001 */
    int _bc_8015AF00 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AF04: 0x00000000 */
    if (_bc_8015AF00) {
        psx_check_interrupts_at(cpu, 0x8015AF0Cu);
        goto block_8015AF0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AF08u);
        goto block_8015AF08;  /* not taken */
    }

block_8015AF08:
    cpu->write_half(cpu->gpr[29] + 64, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AF0C */

block_8015AF0C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AF10: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 11;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015AF18: 0x30420001 */
    int _bc_8015AF1C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AF20: 0x00000000 */
    if (_bc_8015AF1C) {
        psx_check_interrupts_at(cpu, 0x8015AF28u);
        goto block_8015AF28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AF24u);
        goto block_8015AF24;  /* not taken */
    }

block_8015AF24:
    cpu->write_half(cpu->gpr[29] + 66, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AF28 */

block_8015AF28:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AF2C: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 12;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8015AF34: 0x30420001 */
    int _bc_8015AF38 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_8015AF38) {
        psx_check_interrupts_at(cpu, 0x8015AF44u);
        goto block_8015AF44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AF40u);
        goto block_8015AF40;  /* not taken */
    }

block_8015AF40:
    cpu->write_half(cpu->gpr[29] + 68, (uint16_t)cpu->gpr[18]);
    /* fall through to block_8015AF44 */

block_8015AF44:
    cpu->gpr[31] = 0x8015AF4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = -1;
    psx_check_interrupts_at(cpu, 0x8012AE3Cu);
    cpu->pc = 0x8012AE3Cu; return;  /* CPS jal -> 0x8012AE3C */

block_8015AF4C:
    cpu->gpr[17] = cpu->gpr[17] + 1;
    /* fall through to block_8015AF50 */

block_8015AF50:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 124);
    /* nop */  /* 0x8015AF54: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x8015AF58: 0x30420010 */
    int _bc_8015AF5C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1024;
    if (_bc_8015AF5C) {
        psx_check_interrupts_at(cpu, 0x8015AF78u);
        goto block_8015AF78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AF64u);
        goto block_8015AF64;  /* not taken */
    }

block_8015AF64:
    cpu->gpr[5] = 13;
    cpu->gpr[31] = 0x8015AF70u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8015AF70:
    cpu->gpr[17] = 240;
    cpu->gpr[19] = 1;
    /* fall through to block_8015AF78 */

block_8015AF78:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 124);
    /* nop */  /* 0x8015AF7C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x00E0;  /* 0x8015AF80: 0x304200E0 */
    int _bc_8015AF84 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1024;
    if (_bc_8015AF84) {
        psx_check_interrupts_at(cpu, 0x8015AFA0u);
        goto block_8015AFA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AF8Cu);
        goto block_8015AF8C;  /* not taken */
    }

block_8015AF8C:
    cpu->gpr[5] = 12;
    cpu->gpr[31] = 0x8015AF98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8015AF98:
    cpu->gpr[17] = 240;
    cpu->gpr[19] = cpu->gpr[0];
    /* fall through to block_8015AFA0 */

block_8015AFA0:
    cpu->gpr[31] = 0x8015AFA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[29] + 16;
    psx_check_interrupts_at(cpu, 0x801294CCu);
    cpu->pc = 0x801294CCu; return;  /* CPS jal -> 0x801294CC */

block_8015AFA8:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AFAC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015B15Cu);
    goto block_8015B15C;  /* j */

block_8015AFB0:
    int _bc_8015AFB0 = (cpu->gpr[19] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = 255;
    if (_bc_8015AFB0) {
        psx_check_interrupts_at(cpu, 0x8015B164u);
        goto block_8015B164;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AFB8u);
        goto block_8015AFB8;  /* not taken */
    }

block_8015AFB8:
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 36);
    /* nop */  /* 0x8015AFC0: 0x00000000 */
    int _bc_8015AFC4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    if (_bc_8015AFC4) {
        psx_check_interrupts_at(cpu, 0x8015AFD0u);
        goto block_8015AFD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AFCCu);
        goto block_8015AFCC;  /* not taken */
    }

block_8015AFCC:
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015AFD0 */

block_8015AFD0:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 38);
    /* nop */  /* 0x8015AFD4: 0x00000000 */
    int _bc_8015AFD8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AFDC: 0x00000000 */
    if (_bc_8015AFD8) {
        psx_check_interrupts_at(cpu, 0x8015AFF0u);
        goto block_8015AFF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015AFE0u);
        goto block_8015AFE0;  /* not taken */
    }

block_8015AFE0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015AFE4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0008;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015AFF0 */

block_8015AFF0:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 40);
    /* nop */  /* 0x8015AFF4: 0x00000000 */
    int _bc_8015AFF8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015AFFC: 0x00000000 */
    if (_bc_8015AFF8) {
        psx_check_interrupts_at(cpu, 0x8015B010u);
        goto block_8015B010;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B000u);
        goto block_8015B000;  /* not taken */
    }

block_8015B000:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B004: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0010;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B010 */

block_8015B010:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 42);
    /* nop */  /* 0x8015B014: 0x00000000 */
    int _bc_8015B018 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015B01C: 0x00000000 */
    if (_bc_8015B018) {
        psx_check_interrupts_at(cpu, 0x8015B030u);
        goto block_8015B030;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B020u);
        goto block_8015B020;  /* not taken */
    }

block_8015B020:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B024: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0020;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B030 */

block_8015B030:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 44);
    /* nop */  /* 0x8015B034: 0x00000000 */
    int _bc_8015B038 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015B03C: 0x00000000 */
    if (_bc_8015B038) {
        psx_check_interrupts_at(cpu, 0x8015B050u);
        goto block_8015B050;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B040u);
        goto block_8015B040;  /* not taken */
    }

block_8015B040:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B044: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0040;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B050 */

block_8015B050:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 46);
    /* nop */  /* 0x8015B054: 0x00000000 */
    int _bc_8015B058 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015B05C: 0x00000000 */
    if (_bc_8015B058) {
        psx_check_interrupts_at(cpu, 0x8015B070u);
        goto block_8015B070;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B060u);
        goto block_8015B060;  /* not taken */
    }

block_8015B060:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B064: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0080;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B070 */

block_8015B070:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[29] + 48);
    /* nop */  /* 0x8015B074: 0x00000000 */
    cpu->write_half(cpu->gpr[16] + 4, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[29] + 50);
    /* nop */  /* 0x8015B080: 0x00000000 */
    cpu->write_half(cpu->gpr[16] + 6, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[29] + 52);
    /* nop */  /* 0x8015B08C: 0x00000000 */
    cpu->write_half(cpu->gpr[16] + 8, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[29] + 54);
    /* nop */  /* 0x8015B098: 0x00000000 */
    cpu->write_half(cpu->gpr[16] + 10, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 56);
    /* nop */  /* 0x8015B0A4: 0x00000000 */
    int _bc_8015B0A8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015B0AC: 0x00000000 */
    if (_bc_8015B0A8) {
        psx_check_interrupts_at(cpu, 0x8015B0C0u);
        goto block_8015B0C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B0B0u);
        goto block_8015B0B0;  /* not taken */
    }

block_8015B0B0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B0B4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0001;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B0C0 */

block_8015B0C0:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 58);
    /* nop */  /* 0x8015B0C4: 0x00000000 */
    int _bc_8015B0C8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015B0CC: 0x00000000 */
    if (_bc_8015B0C8) {
        psx_check_interrupts_at(cpu, 0x8015B0E0u);
        goto block_8015B0E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B0D0u);
        goto block_8015B0D0;  /* not taken */
    }

block_8015B0D0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B0D4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0002;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B0E0 */

block_8015B0E0:
    cpu->gpr[2] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 192);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[29] + 60);
    /* nop */  /* 0x8015B0EC: 0x00000000 */
    cpu->write_byte(cpu->gpr[2] + 37, (uint8_t)cpu->gpr[3]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 64);
    /* nop */  /* 0x8015B0F8: 0x00000000 */
    int _bc_8015B0FC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015B100: 0x00000000 */
    if (_bc_8015B0FC) {
        psx_check_interrupts_at(cpu, 0x8015B114u);
        goto block_8015B114;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B104u);
        goto block_8015B104;  /* not taken */
    }

block_8015B104:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B108: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0400;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B114 */

block_8015B114:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 66);
    /* nop */  /* 0x8015B118: 0x00000000 */
    int _bc_8015B11C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015B120: 0x00000000 */
    if (_bc_8015B11C) {
        psx_check_interrupts_at(cpu, 0x8015B134u);
        goto block_8015B134;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B124u);
        goto block_8015B124;  /* not taken */
    }

block_8015B124:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B128: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x0800;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B134 */

block_8015B134:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 68);
    /* nop */  /* 0x8015B138: 0x00000000 */
    int _bc_8015B13C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = 255;
    if (_bc_8015B13C) {
        psx_check_interrupts_at(cpu, 0x8015B164u);
        goto block_8015B164;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B144u);
        goto block_8015B144;  /* not taken */
    }

block_8015B144:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x8015B148: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x1000;
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8015B154 */

block_8015B154:
    /* delay slot (always executes) */
    cpu->gpr[17] = 255;
    psx_check_interrupts_at(cpu, 0x8015B164u);
    goto block_8015B164;  /* j */

block_8015B15C:
    cpu->gpr[31] = 0x8015B164u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 0x1F80 << 16;  /* 0x1F800000 */
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_8015B164:
    cpu->gpr[2] = 255;
    int _bc_8015B168 = (cpu->gpr[17] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[19];
    if (_bc_8015B168) {
        psx_check_interrupts_at(cpu, 0x8015AD14u);
        goto block_8015AD14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015B170u);
        goto block_8015B170;  /* not taken */
    }

block_8015B170:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 108);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 92);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 88);
    uint32_t _jt_8015B188 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 112;
    psx_check_interrupts_at(cpu, _jt_8015B188);
    cpu->pc = _jt_8015B188; return;  /* CPS: jr $ra */
}

/* cps_801a972c — clean CPS transliteration of br2eu_func_801A972C (guest 0x801a972c)
 * from SLES_017.22_full_52.c. */
void cps_801a972c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A983Cu: goto block_801A983C;
            case 0x801A9740u: goto block_801A9740;
            case 0x801A9748u: goto block_801A9748;
            case 0x801A975Cu: goto block_801A975C;
            case 0x801A976Cu: goto block_801A976C;
            case 0x801A9770u: goto block_801A9770;
            case 0x801A9778u: goto block_801A9778;
            case 0x801A9784u: goto block_801A9784;
            case 0x801A97A0u: goto block_801A97A0;
            case 0x801A97C4u: goto block_801A97C4;
            case 0x801A97DCu: goto block_801A97DC;
            case 0x801A97E8u: goto block_801A97E8;
            case 0x801A97F0u: goto block_801A97F0;
            case 0x801A9810u: goto block_801A9810;
            case 0x801A9820u: goto block_801A9820;
            case 0x801A9828u: goto block_801A9828;
            case 0x801A9830u: goto block_801A9830;
            default: break;
        }
    }
    /* Address: 0x801A972C, Size: 288 bytes, Blocks: 18 */

block_801A972C:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[28] + 68);
    cpu->gpr[2] = 1;
    int _bc_801A9738 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);
    if (_bc_801A9738) {
        psx_check_interrupts_at(cpu, 0x801A983Cu);
        goto block_801A983C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9740u);
        goto block_801A9740;  /* not taken */
    }

block_801A9740:
    int _bc_801A9740 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A9744: 0x00000000 */
    if (_bc_801A9740) {
        psx_check_interrupts_at(cpu, 0x801A983Cu);
        goto block_801A983C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9748u);
        goto block_801A9748;  /* not taken */
    }

block_801A9748:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 10);
    cpu->gpr[6] = cpu->read_word(cpu->gpr[4] + 44);
    /* nop */  /* 0x801A9750: 0x00000000 */
    int _bc_801A9754 = (cpu->gpr[6] == cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[2];
    if (_bc_801A9754) {
        psx_check_interrupts_at(cpu, 0x801A983Cu);
        goto block_801A983C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A975Cu);
        goto block_801A975C;  /* not taken */
    }

block_801A975C:
    cpu->gpr[5] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 14);
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[6]) ? 1 : 0;  /* 0x801A9760: 0x0066102A */
    int _bc_801A9764 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[0];
    if (_bc_801A9764) {
        psx_check_interrupts_at(cpu, 0x801A9770u);
        goto block_801A9770;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A976Cu);
        goto block_801A976C;  /* not taken */
    }

block_801A976C:
    cpu->gpr[8] = -1;
    /* fall through to block_801A9770 */

block_801A9770:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] - cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x801A9820u);
    goto block_801A9820;  /* j */

block_801A9778:
    cpu->gpr[5] = cpu->gpr[0];
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801A9830u);
    goto block_801A9830;  /* j */

block_801A9784:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 10);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[4] + 44);
    /* nop */  /* 0x801A978C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[2] = ((int32_t)cpu->gpr[7] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x801A9794: 0x00E2102A */
    int _bc_801A9798 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[5] << 2;
    if (_bc_801A9798) {
        psx_check_interrupts_at(cpu, 0x801A9830u);
        goto block_801A9830;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A97A0u);
        goto block_801A97A0;  /* not taken */
    }

block_801A97A0:
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[5];
    cpu->gpr[3] = cpu->read_word(cpu->gpr[4] + 24);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 4);
    cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[8];
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[6]) ? 1 : 0;  /* 0x801A97B8: 0x0046102A */
    int _bc_801A97BC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A97C0: 0x00000000 */
    if (_bc_801A97BC) {
        psx_check_interrupts_at(cpu, 0x801A97E8u);
        goto block_801A97E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A97C4u);
        goto block_801A97C4;  /* not taken */
    }

block_801A97C4:
    cpu->gpr[5] = cpu->gpr[5] + 1;
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 6);
    /* nop */  /* 0x801A97CC: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[5] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x801A97D0: 0x00A3102A */
    int _bc_801A97D4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 1;
    if (_bc_801A97D4) {
        psx_check_interrupts_at(cpu, 0x801A9810u);
        goto block_801A9810;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A97DCu);
        goto block_801A97DC;  /* not taken */
    }

block_801A97DC:
    cpu->gpr[5] = cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801A9830u);
    goto block_801A9830;  /* j */

block_801A97E8:
    int _bc_801A97E8 = ((int32_t)cpu->gpr[6] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A97EC: 0x00000000 */
    if (_bc_801A97E8) {
        psx_check_interrupts_at(cpu, 0x801A9810u);
        goto block_801A9810;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A97F0u);
        goto block_801A97F0;  /* not taken */
    }

block_801A97F0:
    cpu->gpr[5] = cpu->gpr[5] + -1;
    cpu->gpr[2] = cpu->gpr[5] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[5];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 4);
    int _bc_801A9808 = ((int32_t)cpu->gpr[5] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[2] + -1;
    if (_bc_801A9808) {
        psx_check_interrupts_at(cpu, 0x801A9778u);
        goto block_801A9778;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9810u);
        goto block_801A9810;  /* not taken */
    }

block_801A9810:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 10);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 44);
    cpu->gpr[7] = cpu->gpr[7] + 1;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    /* fall through to block_801A9820 */

block_801A9820:
    int _bc_801A9820 = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[7] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x801A9824: 0x00E2102A */
    if (_bc_801A9820) {
        psx_check_interrupts_at(cpu, 0x801A9784u);
        goto block_801A9784;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9828u);
        goto block_801A9828;  /* not taken */
    }

block_801A9828:
    int _bc_801A9828 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[5] << 2;
    if (_bc_801A9828) {
        psx_check_interrupts_at(cpu, 0x801A97A0u);
        goto block_801A97A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A9830u);
        goto block_801A9830;  /* not taken */
    }

block_801A9830:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 44);
    cpu->gpr[31] = 0x801A983Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 10, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x801A9C24u);
    cpu->pc = 0x801A9C24u; return;  /* CPS jal -> 0x801A9C24 */

block_801A983C:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 16);
    /* nop */  /* 0x801A9840: 0x00000000 */
    uint32_t _jt_801A9844 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 24;
    psx_check_interrupts_at(cpu, _jt_801A9844);
    cpu->pc = _jt_801A9844; return;  /* CPS: jr $ra */
}

/* cps_80168838 — clean CPS transliteration of br2eu_func_80168838 (guest 0x80168838)
 * from SLES_017.22_full_27.c. */
void cps_80168838(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80168888u: goto block_80168888;
            case 0x8016889Cu: goto block_8016889C;
            case 0x80168870u: goto block_80168870;
            case 0x80168890u: goto block_80168890;
            case 0x801688A4u: goto block_801688A4;
            case 0x801688B8u: goto block_801688B8;
            case 0x801688C0u: goto block_801688C0;
            case 0x801688C8u: goto block_801688C8;
            default: break;
        }
    }
    /* Address: 0x80168838, Size: 184 bytes, Blocks: 9 */

block_80168838:
    cpu->gpr[29] = cpu->gpr[29] + -64;
    cpu->write_word(cpu->gpr[29] + 52, cpu->gpr[21]);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 80);
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[19]);
    cpu->gpr[19] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[20]);
    cpu->gpr[20] = cpu->gpr[7];
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[0];
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[31]);
    /* fall through to block_80168870 */

block_80168870:
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[21]);
    cpu->gpr[4] = cpu->gpr[17];
    cpu->gpr[5] = cpu->gpr[18];
    cpu->gpr[6] = cpu->gpr[19];
    cpu->gpr[31] = 0x80168888u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[20];
    psx_check_interrupts_at(cpu, 0x8019F35Cu);
    cpu->pc = 0x8019F35Cu; return;  /* CPS jal -> 0x8019F35C */

block_80168888:
    int _bc_80168888 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    if (_bc_80168888) {
        psx_check_interrupts_at(cpu, 0x80168870u);
        goto block_80168870;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80168890u);
        goto block_80168890;  /* not taken */
    }

block_80168890:
    cpu->gpr[5] = cpu->gpr[29] + 24;
    cpu->gpr[31] = 0x8016889Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[29] + 28;
    psx_check_interrupts_at(cpu, 0x8019FA80u);
    cpu->pc = 0x8019FA80u; return;  /* CPS jal -> 0x8019FA80 */

block_8016889C:
    int _bc_8016889C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    if (_bc_8016889C) {
        psx_check_interrupts_at(cpu, 0x80168890u);
        goto block_80168890;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801688A4u);
        goto block_801688A4;  /* not taken */
    }

block_801688A4:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[29] + 28);
    /* nop */  /* 0x801688A8: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)4) ? 1 : 0;  /* 0x801688AC: 0x2C620004 */
    int _bc_801688B0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801688B4: 0x00000000 */
    if (_bc_801688B0) {
        psx_check_interrupts_at(cpu, 0x801688C0u);
        goto block_801688C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801688B8u);
        goto block_801688B8;  /* not taken */
    }

block_801688B8:
    int _bc_801688B8 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801688BC: 0x00000000 */
    if (_bc_801688B8) {
        psx_check_interrupts_at(cpu, 0x801688C8u);
        goto block_801688C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801688C0u);
        goto block_801688C0;  /* not taken */
    }

block_801688C0:
    int _bc_801688C0 = (cpu->gpr[16] != cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[3];
    if (_bc_801688C0) {
        psx_check_interrupts_at(cpu, 0x80168870u);
        goto block_80168870;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801688C8u);
        goto block_801688C8;  /* not taken */
    }

block_801688C8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 56);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 52);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 48);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 32);
    uint32_t _jt_801688E8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 64;
    psx_check_interrupts_at(cpu, _jt_801688E8);
    cpu->pc = _jt_801688E8; return;  /* CPS: jr $ra */
}
