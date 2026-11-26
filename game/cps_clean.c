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
void cps_80126234(void);
void cps_8013c9cc(void);
void cps_801468ec(void);
void cps_801565ec(void);
void cps_801587c0(void);
void cps_8015ace4(void);
void cps_80161630(void);
void cps_80168838(void);
void cps_8018b100(void);
void cps_8018cfdc(void);
void cps_8018d150(void);
void cps_80195be0(void);
void cps_80195bf0(void);
void cps_80195c10(void);
void cps_80196370(void);
void cps_80196e14(void);
void cps_801985c4(void);
void cps_80198c90(void);
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
void cps_8019a5d8(void);
void cps_8019aabc(void);
void cps_8019fb9c(void);
void cps_801a0660(void);
void cps_801a972c(void);
void cps_80198af0(void);
void cps_8019a00c(void);
void cps_8019a09c(void);
void cps_8019a378(void);
void cps_8019a3f0(void);
void cps_8019e330(void);
void cps_8019e39c(void);
void cps_801a0be0(void);
void cps_801a0c68(void);
void cps_801a1a40(void);
void cps_801a36f0(void);

/* cps_80126234 — clean CPS transliteration of br2eu_func_80126234 (guest 0x80126234)
 * from SLES_017.22_full_00.c. */
void cps_80126234(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80126258u: goto block_80126258;
            default: break;
        }
    }
    /* Address: 0x80126234, Size: 40 bytes, Blocks: 2 */

block_80126234:
    cpu->gpr[29] = cpu->gpr[29] + -64;
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 52, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[17]);
    cpu->gpr[31] = 0x80126258u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[16]);
    psx_check_interrupts_at(cpu, 0x80126170u);
    cpu->pc = 0x80126170u; return;  /* CPS jal -> 0x80126170 */

block_80126258:
    cpu->gpr[31] = 0x80126260u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[19] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80126F04u);
    cpu->pc = 0x80126F04u; return;  /* CPS jal -> 0x80126F04 */
}

/* cps_8013c9cc — clean CPS transliteration of br2eu_func_8013C9CC (guest 0x8013c9cc)
 * from SLES_017.22_full_09.c. */
void cps_8013c9cc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8013CA38u: goto block_8013CA38;
            case 0x8013CB28u: goto block_8013CB28;
            case 0x8013CC1Cu: goto block_8013CC1C;
            case 0x8013CC4Cu: goto block_8013CC4C;
            case 0x8013CD58u: goto block_8013CD58;
            case 0x8013CDB8u: goto block_8013CDB8;
            case 0x8013CA14u: goto block_8013CA14;
            case 0x8013CA40u: goto block_8013CA40;
            case 0x8013CA74u: goto block_8013CA74;
            case 0x8013CA90u: goto block_8013CA90;
            case 0x8013CAA4u: goto block_8013CAA4;
            case 0x8013CABCu: goto block_8013CABC;
            case 0x8013CAC0u: goto block_8013CAC0;
            case 0x8013CAE0u: goto block_8013CAE0;
            case 0x8013CB64u: goto block_8013CB64;
            case 0x8013CB70u: goto block_8013CB70;
            case 0x8013CB78u: goto block_8013CB78;
            case 0x8013CBBCu: goto block_8013CBBC;
            case 0x8013CBD0u: goto block_8013CBD0;
            case 0x8013CBE4u: goto block_8013CBE4;
            case 0x8013CC04u: goto block_8013CC04;
            case 0x8013CC08u: goto block_8013CC08;
            case 0x8013CC28u: goto block_8013CC28;
            case 0x8013CC40u: goto block_8013CC40;
            case 0x8013CC50u: goto block_8013CC50;
            case 0x8013CC60u: goto block_8013CC60;
            case 0x8013CC6Cu: goto block_8013CC6C;
            case 0x8013CCA4u: goto block_8013CCA4;
            case 0x8013CCD8u: goto block_8013CCD8;
            case 0x8013CCE4u: goto block_8013CCE4;
            case 0x8013CCF4u: goto block_8013CCF4;
            case 0x8013CD30u: goto block_8013CD30;
            case 0x8013CD44u: goto block_8013CD44;
            case 0x8013CD50u: goto block_8013CD50;
            case 0x8013CD90u: goto block_8013CD90;
            case 0x8013CDA4u: goto block_8013CDA4;
            case 0x8013CDACu: goto block_8013CDAC;
            default: break;
        }
    }
    /* Address: 0x8013C9CC, Size: 1052 bytes, Blocks: 38 */

block_8013C9CC:
    cpu->gpr[2] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[2] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[2] + 177);
    cpu->gpr[29] = cpu->gpr[29] + -96;
    cpu->write_word(cpu->gpr[29] + 60, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 64, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[7];
    cpu->write_word(cpu->gpr[29] + 92, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[30]);
    cpu->write_word(cpu->gpr[29] + 84, cpu->gpr[23]);
    cpu->write_word(cpu->gpr[29] + 80, cpu->gpr[22]);
    cpu->write_word(cpu->gpr[29] + 76, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 72, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 68, cpu->gpr[19]);
    int _bc_8013CA0C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 104, cpu->gpr[6]);
    if (_bc_8013CA0C) {
        psx_check_interrupts_at(cpu, 0x8013CDB8u);
        goto block_8013CDB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CA14u);
        goto block_8013CA14;  /* not taken */
    }

block_8013CA14:
    cpu->gpr[4] = 1;
    cpu->gpr[5] = 2;
    cpu->gpr[6] = 10;
    cpu->gpr[7] = 0x8014 << 16;  /* 0x80140000 */
    cpu->gpr[7] = cpu->gpr[7] + -18644;
    cpu->gpr[2] = 7200;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[0]);
    cpu->gpr[31] = 0x8013CA38u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x801A4308u);
    cpu->pc = 0x801A4308u; return;  /* CPS jal -> 0x801A4308 */

block_8013CA38:
    int _bc_8013CA38 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[2]);
    if (_bc_8013CA38) {
        psx_check_interrupts_at(cpu, 0x8013CDB8u);
        goto block_8013CDB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CA40u);
        goto block_8013CA40;  /* not taken */
    }

block_8013CA40:
    cpu->gpr[30] = cpu->gpr[0];
    cpu->gpr[4] = 4992;
    cpu->gpr[22] = cpu->read_word(cpu->gpr[2] + 16);
    cpu->gpr[2] = -1;
    cpu->write_half(cpu->gpr[22] + 7192, (uint16_t)cpu->gpr[16]);
    cpu->write_half(cpu->gpr[22] + 7194, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[22] + 7196, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[22] + 7198, (uint16_t)cpu->gpr[17]);
    cpu->gpr[11] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->gpr[3] = cpu->gpr[22];
    cpu->write_word(cpu->gpr[22] + 5768, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[22] + 5756, cpu->gpr[2]);
    cpu->write_word(cpu->gpr[22] + 5764, cpu->gpr[11]);
    /* fall through to block_8013CA74 */

block_8013CA74:
    cpu->gpr[2] = cpu->gpr[22] + cpu->gpr[4];
    cpu->write_word(cpu->gpr[3] + 5536, cpu->gpr[2]);
    cpu->gpr[4] = cpu->gpr[4] + 136;
    cpu->gpr[30] = cpu->gpr[30] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[30] < 4) ? 1 : 0;  /* 0x8013CA84: 0x2BC20004 */
    int _bc_8013CA88 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 4;
    if (_bc_8013CA88) {
        psx_check_interrupts_at(cpu, 0x8013CA74u);
        goto block_8013CA74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CA90u);
        goto block_8013CA90;  /* not taken */
    }

block_8013CA90:
    cpu->gpr[30] = cpu->gpr[0];
    cpu->gpr[10] = 32;
    cpu->gpr[9] = 33;
    cpu->gpr[8] = cpu->gpr[22];
    cpu->gpr[7] = cpu->gpr[30];
    /* fall through to block_8013CAA4 */

block_8013CAA4:
    cpu->gpr[11] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->gpr[23] = cpu->gpr[22] + cpu->gpr[7];
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[11] + 4);
    /* nop */  /* 0x8013CAB0: 0x00000000 */
    int _bc_8013CAB4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 31002;
    if (_bc_8013CAB4) {
        psx_check_interrupts_at(cpu, 0x8013CAC0u);
        goto block_8013CAC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CABCu);
        goto block_8013CABC;  /* not taken */
    }

block_8013CABC:
    cpu->gpr[3] = 31514;
    /* fall through to block_8013CAC0 */

block_8013CAC0:
    cpu->gpr[18] = cpu->gpr[0];
    cpu->gpr[11] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->gpr[17] = cpu->gpr[8];
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[11] + 2);
    cpu->gpr[19] = cpu->gpr[7];
    cpu->gpr[2] = (cpu->gpr[18] < cpu->gpr[2]) ? 1 : 0;  /* 0x8013CAD4: 0x0242102B */
    cpu->gpr[20] = cpu->gpr[2] << 6;
    cpu->gpr[21] = cpu->gpr[20] | 0x0001;
    /* fall through to block_8013CAE0 */

block_8013CAE0:
    cpu->gpr[4] = cpu->gpr[17];
    cpu->gpr[16] = cpu->gpr[22] + cpu->gpr[19];
    cpu->gpr[2] = 63;
    cpu->write_half(cpu->gpr[16] + 26, (uint16_t)cpu->gpr[2]);
    cpu->write_byte(cpu->gpr[17] + 12, (uint8_t)cpu->gpr[10]);
    cpu->write_byte(cpu->gpr[17] + 13, (uint8_t)cpu->gpr[20]);
    cpu->write_byte(cpu->gpr[17] + 24, (uint8_t)cpu->gpr[9]);
    cpu->write_byte(cpu->gpr[17] + 25, (uint8_t)cpu->gpr[20]);
    cpu->write_byte(cpu->gpr[17] + 36, (uint8_t)cpu->gpr[10]);
    cpu->write_byte(cpu->gpr[17] + 37, (uint8_t)cpu->gpr[21]);
    cpu->write_byte(cpu->gpr[17] + 48, (uint8_t)cpu->gpr[9]);
    cpu->write_byte(cpu->gpr[17] + 49, (uint8_t)cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[3]);
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[7]);
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[8]);
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[9]);
    cpu->gpr[31] = 0x8013CB28u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[10]);
    psx_check_interrupts_at(cpu, 0x80194100u);
    cpu->pc = 0x80194100u; return;  /* CPS jal -> 0x80194100 */

block_8013CB28:
    cpu->gpr[19] = cpu->gpr[19] + 52;
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[17] + 7);
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[2] = cpu->gpr[2] & 0x00FE;  /* 0x8013CB34: 0x304200FE */
    cpu->gpr[2] = cpu->gpr[2] | 0x0002;
    cpu->write_byte(cpu->gpr[17] + 7, (uint8_t)cpu->gpr[2]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < 48) ? 1 : 0;  /* 0x8013CB44: 0x2A420030 */
    cpu->write_half(cpu->gpr[16] + 14, (uint16_t)cpu->gpr[3]);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[9] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 48);
    int _bc_8013CB5C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = cpu->gpr[17] + 52;
    if (_bc_8013CB5C) {
        psx_check_interrupts_at(cpu, 0x8013CAE0u);
        goto block_8013CAE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CB64u);
        goto block_8013CB64;  /* not taken */
    }

block_8013CB64:
    cpu->gpr[5] = 160;
    cpu->gpr[4] = 120;
    cpu->gpr[18] = cpu->gpr[0];
    /* fall through to block_8013CB70 */

block_8013CB70:
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[3] = cpu->gpr[23] + 42;
    /* fall through to block_8013CB78 */

block_8013CB78:
    cpu->write_byte(cpu->gpr[3] + -38, (uint8_t)cpu->gpr[5]);
    cpu->write_byte(cpu->gpr[3] + -37, (uint8_t)cpu->gpr[5]);
    cpu->write_byte(cpu->gpr[3] + -36, (uint8_t)cpu->gpr[5]);
    cpu->write_byte(cpu->gpr[3] + -14, (uint8_t)cpu->gpr[5]);
    cpu->write_byte(cpu->gpr[3] + -13, (uint8_t)cpu->gpr[5]);
    cpu->write_byte(cpu->gpr[3] + -12, (uint8_t)cpu->gpr[5]);
    cpu->write_byte(cpu->gpr[3] + -26, (uint8_t)cpu->gpr[4]);
    cpu->write_byte(cpu->gpr[3] + -25, (uint8_t)cpu->gpr[4]);
    cpu->write_byte(cpu->gpr[3] + -24, (uint8_t)cpu->gpr[4]);
    cpu->write_byte(cpu->gpr[3] + -2, (uint8_t)cpu->gpr[4]);
    cpu->write_byte(cpu->gpr[3] + -1, (uint8_t)cpu->gpr[4]);
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[4]);
    cpu->gpr[3] = cpu->gpr[3] + 52;
    cpu->gpr[6] = cpu->gpr[6] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[6] < 16) ? 1 : 0;  /* 0x8013CBB0: 0x28C20010 */
    int _bc_8013CBB4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[23] = cpu->gpr[23] + 52;
    if (_bc_8013CBB4) {
        psx_check_interrupts_at(cpu, 0x8013CB78u);
        goto block_8013CB78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CBBCu);
        goto block_8013CBBC;  /* not taken */
    }

block_8013CBBC:
    cpu->gpr[5] = cpu->gpr[5] + -60;
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < 3) ? 1 : 0;  /* 0x8013CBC4: 0x2A420003 */
    int _bc_8013CBC8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + -60;
    if (_bc_8013CBC8) {
        psx_check_interrupts_at(cpu, 0x8013CB70u);
        goto block_8013CB70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CBD0u);
        goto block_8013CBD0;  /* not taken */
    }

block_8013CBD0:
    cpu->gpr[8] = cpu->gpr[8] + 2496;
    cpu->gpr[30] = cpu->gpr[30] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[30] < 2) ? 1 : 0;  /* 0x8013CBD8: 0x2BC20002 */
    int _bc_8013CBDC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[7] + 2496;
    if (_bc_8013CBDC) {
        psx_check_interrupts_at(cpu, 0x8013CAA4u);
        goto block_8013CAA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CBE4u);
        goto block_8013CBE4;  /* not taken */
    }

block_8013CBE4:
    cpu->gpr[5] = 240;
    cpu->gpr[4] = cpu->gpr[22] + 5772;
    cpu->gpr[11] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[11] + 2);
    /* nop */  /* 0x8013CBF8: 0x00000000 */
    int _bc_8013CBFC = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + 17320;
    if (_bc_8013CBFC) {
        psx_check_interrupts_at(cpu, 0x8013CC08u);
        goto block_8013CC08;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CC04u);
        goto block_8013CC04;  /* not taken */
    }

block_8013CC04:
    cpu->gpr[5] = 160;
    /* fall through to block_8013CC08 */

block_8013CC08:
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[2] = cpu->gpr[5] + cpu->gpr[2];
    cpu->gpr[5] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[31] = 0x8013CC1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 1;
    psx_check_interrupts_at(cpu, 0x801A6BD0u);
    cpu->pc = 0x801A6BD0u; return;  /* CPS jal -> 0x801A6BD0 */

block_8013CC1C:
    cpu->gpr[4] = cpu->gpr[2];
    int _bc_8013CC20 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[22] + 5772, cpu->gpr[4]);
    if (_bc_8013CC20) {
        psx_check_interrupts_at(cpu, 0x8013CDACu);
        goto block_8013CDAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CC28u);
        goto block_8013CC28;  /* not taken */
    }

block_8013CC28:
    cpu->gpr[11] = cpu->read_word(cpu->gpr[29] + 104);
    /* nop */  /* 0x8013CC2C: 0x00000000 */
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[11] + 2);
    cpu->gpr[2] = 1;
    int _bc_8013CC38 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[30] = cpu->gpr[0];
    if (_bc_8013CC38) {
        psx_check_interrupts_at(cpu, 0x8013CC50u);
        goto block_8013CC50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CC40u);
        goto block_8013CC40;  /* not taken */
    }

block_8013CC40:
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[31] = 0x8013CC4Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 64;
    psx_check_interrupts_at(cpu, 0x801ACB08u);
    cpu->pc = 0x801ACB08u; return;  /* CPS jal -> 0x801ACB08 */

block_8013CC4C:
    cpu->gpr[30] = cpu->gpr[0];
    /* fall through to block_8013CC50 */

block_8013CC50:
    cpu->gpr[20] = 32;
    cpu->gpr[19] = 39;
    cpu->gpr[23] = cpu->gpr[22];
    cpu->gpr[21] = cpu->gpr[30];
    /* fall through to block_8013CC60 */

block_8013CC60:
    cpu->gpr[18] = cpu->gpr[0];
    cpu->gpr[16] = cpu->gpr[23];
    cpu->gpr[17] = cpu->gpr[21];
    /* fall through to block_8013CC6C */

block_8013CC6C:
    cpu->gpr[4] = cpu->gpr[22] + cpu->gpr[17];
    cpu->gpr[2] = 31;
    cpu->write_half(cpu->gpr[4] + 5798, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = 0x0900 << 16;  /* 0x09000000 */
    cpu->gpr[3] = cpu->gpr[16] + 5776;
    cpu->write_word(cpu->gpr[16] + 5776, cpu->gpr[2]);
    cpu->gpr[2] = 44;
    cpu->write_byte(cpu->gpr[3] + 7, (uint8_t)cpu->gpr[2]);
    cpu->gpr[11] = cpu->read_word(cpu->gpr[29] + 104);
    /* nop */  /* 0x8013CC90: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[11] + 2);
    /* nop */  /* 0x8013CC98: 0x00000000 */
    int _bc_8013CC9C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 64;
    if (_bc_8013CC9C) {
        psx_check_interrupts_at(cpu, 0x8013CCF4u);
        goto block_8013CCF4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CCA4u);
        goto block_8013CCA4;  /* not taken */
    }

block_8013CCA4:
    cpu->gpr[2] = 15;
    cpu->write_byte(cpu->gpr[3] + 12, (uint8_t)cpu->gpr[20]);
    cpu->write_byte(cpu->gpr[3] + 13, (uint8_t)cpu->gpr[0]);
    cpu->write_byte(cpu->gpr[3] + 20, (uint8_t)cpu->gpr[19]);
    cpu->write_byte(cpu->gpr[3] + 21, (uint8_t)cpu->gpr[0]);
    cpu->write_byte(cpu->gpr[3] + 28, (uint8_t)cpu->gpr[20]);
    cpu->write_byte(cpu->gpr[3] + 29, (uint8_t)cpu->gpr[2]);
    cpu->write_byte(cpu->gpr[3] + 36, (uint8_t)cpu->gpr[19]);
    cpu->write_byte(cpu->gpr[3] + 37, (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[11] + 4);
    /* nop */  /* 0x8013CCCC: 0x00000000 */
    int _bc_8013CCD0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 30790;
    if (_bc_8013CCD0) {
        psx_check_interrupts_at(cpu, 0x8013CCE4u);
        goto block_8013CCE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CCD8u);
        goto block_8013CCD8;  /* not taken */
    }

block_8013CCD8:
    cpu->gpr[2] = 30726;
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 5790, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8013CD58u);
    goto block_8013CD58;  /* j */

block_8013CCE4:
    cpu->write_half(cpu->gpr[4] + 5790, (uint16_t)cpu->gpr[2]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[22] + 5772);
    /* delay slot (always executes) */
    cpu->gpr[5] = 30790;
    psx_check_interrupts_at(cpu, 0x8013CD50u);
    goto block_8013CD50;  /* j */

block_8013CCF4:
    cpu->write_byte(cpu->gpr[3] + 13, (uint8_t)cpu->gpr[2]);
    cpu->write_byte(cpu->gpr[3] + 21, (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = 79;
    cpu->write_byte(cpu->gpr[3] + 12, (uint8_t)cpu->gpr[20]);
    cpu->write_byte(cpu->gpr[3] + 20, (uint8_t)cpu->gpr[19]);
    cpu->write_byte(cpu->gpr[3] + 28, (uint8_t)cpu->gpr[20]);
    cpu->write_byte(cpu->gpr[3] + 29, (uint8_t)cpu->gpr[2]);
    cpu->write_byte(cpu->gpr[3] + 36, (uint8_t)cpu->gpr[19]);
    cpu->write_byte(cpu->gpr[3] + 37, (uint8_t)cpu->gpr[2]);
    cpu->gpr[11] = cpu->read_word(cpu->gpr[29] + 104);
    /* nop */  /* 0x8013CD1C: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[11] + 4);
    /* nop */  /* 0x8013CD24: 0x00000000 */
    int _bc_8013CD28 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 30791;
    if (_bc_8013CD28) {
        psx_check_interrupts_at(cpu, 0x8013CD44u);
        goto block_8013CD44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CD30u);
        goto block_8013CD30;  /* not taken */
    }

block_8013CD30:
    cpu->gpr[2] = 30727;
    cpu->write_half(cpu->gpr[4] + 5790, (uint16_t)cpu->gpr[2]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[22] + 5772);
    /* delay slot (always executes) */
    cpu->gpr[5] = 30727;
    psx_check_interrupts_at(cpu, 0x8013CD50u);
    goto block_8013CD50;  /* j */

block_8013CD44:
    cpu->write_half(cpu->gpr[4] + 5790, (uint16_t)cpu->gpr[2]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[22] + 5772);
    cpu->gpr[5] = 30791;
    /* fall through to block_8013CD50 */

block_8013CD50:
    cpu->gpr[31] = 0x8013CD58u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013CD54: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801AC9B8u);
    cpu->pc = 0x801AC9B8u; return;  /* CPS jal -> 0x801AC9B8 */

block_8013CD58:
    cpu->gpr[4] = cpu->gpr[16] + 5776;
    cpu->gpr[16] = cpu->gpr[16] + 40;
    cpu->gpr[17] = cpu->gpr[17] + 40;
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[4] + 7);
    cpu->gpr[2] = 128;
    cpu->write_byte(cpu->gpr[4] + 4, (uint8_t)cpu->gpr[2]);
    cpu->write_byte(cpu->gpr[4] + 5, (uint8_t)cpu->gpr[2]);
    cpu->write_byte(cpu->gpr[4] + 6, (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < 16) ? 1 : 0;  /* 0x8013CD7C: 0x2A420010 */
    cpu->gpr[3] = cpu->gpr[3] | 0x0001;
    cpu->gpr[3] = cpu->gpr[3] & 0x00FD;  /* 0x8013CD84: 0x306300FD */
    int _bc_8013CD88 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[4] + 7, (uint8_t)cpu->gpr[3]);
    if (_bc_8013CD88) {
        psx_check_interrupts_at(cpu, 0x8013CC6Cu);
        goto block_8013CC6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CD90u);
        goto block_8013CD90;  /* not taken */
    }

block_8013CD90:
    cpu->gpr[23] = cpu->gpr[23] + 640;
    cpu->gpr[30] = cpu->gpr[30] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[30] < 2) ? 1 : 0;  /* 0x8013CD98: 0x2BC20002 */
    int _bc_8013CD9C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[21] = cpu->gpr[21] + 640;
    if (_bc_8013CD9C) {
        psx_check_interrupts_at(cpu, 0x8013CC60u);
        goto block_8013CC60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013CDA4u);
        goto block_8013CDA4;  /* not taken */
    }

block_8013CDA4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8013CDA8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8013CDB8u);
    goto block_8013CDB8;  /* j */

block_8013CDAC:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[31] = 0x8013CDB8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8013CDB4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A48FCu);
    cpu->pc = 0x801A48FCu; return;  /* CPS jal -> 0x801A48FC */

block_8013CDB8:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 92);
    cpu->gpr[30] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[23] = cpu->read_word(cpu->gpr[29] + 84);
    cpu->gpr[22] = cpu->read_word(cpu->gpr[29] + 80);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 76);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 72);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 68);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 64);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 60);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 56);
    uint32_t _jt_8013CDE0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 96;
    psx_check_interrupts_at(cpu, _jt_8013CDE0);
    cpu->pc = _jt_8013CDE0; return;  /* CPS: jr $ra */
}

/* cps_801468ec — clean CPS transliteration of br2eu_func_801468EC (guest 0x801468ec)
 * from SLES_017.22_full_13.c. */
void cps_801468ec(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80146A00u: goto block_80146A00;
            case 0x80146AD8u: goto block_80146AD8;
            case 0x80146BB8u: goto block_80146BB8;
            case 0x80146C24u: goto block_80146C24;
            case 0x80146D20u: goto block_80146D20;
            case 0x80146D80u: goto block_80146D80;
            case 0x80146E00u: goto block_80146E00;
            case 0x80146E08u: goto block_80146E08;
            case 0x80146998u: goto block_80146998;
            case 0x801469B4u: goto block_801469B4;
            case 0x801469BCu: goto block_801469BC;
            case 0x801469C4u: goto block_801469C4;
            case 0x801469CCu: goto block_801469CC;
            case 0x801469D4u: goto block_801469D4;
            case 0x801469DCu: goto block_801469DC;
            case 0x801469E0u: goto block_801469E0;
            case 0x801469E8u: goto block_801469E8;
            case 0x80146A0Cu: goto block_80146A0C;
            case 0x80146A20u: goto block_80146A20;
            case 0x80146A34u: goto block_80146A34;
            case 0x80146A44u: goto block_80146A44;
            case 0x80146A60u: goto block_80146A60;
            case 0x80146A6Cu: goto block_80146A6C;
            case 0x80146A70u: goto block_80146A70;
            case 0x80146A78u: goto block_80146A78;
            case 0x80146A80u: goto block_80146A80;
            case 0x80146A88u: goto block_80146A88;
            case 0x80146A98u: goto block_80146A98;
            case 0x80146AA0u: goto block_80146AA0;
            case 0x80146AE4u: goto block_80146AE4;
            case 0x80146AE8u: goto block_80146AE8;
            case 0x80146AF4u: goto block_80146AF4;
            case 0x80146B04u: goto block_80146B04;
            case 0x80146B18u: goto block_80146B18;
            case 0x80146B28u: goto block_80146B28;
            case 0x80146B34u: goto block_80146B34;
            case 0x80146B4Cu: goto block_80146B4C;
            case 0x80146B50u: goto block_80146B50;
            case 0x80146B68u: goto block_80146B68;
            case 0x80146B70u: goto block_80146B70;
            case 0x80146B78u: goto block_80146B78;
            case 0x80146B80u: goto block_80146B80;
            case 0x80146BC4u: goto block_80146BC4;
            case 0x80146BC8u: goto block_80146BC8;
            case 0x80146BD4u: goto block_80146BD4;
            case 0x80146BE8u: goto block_80146BE8;
            case 0x80146C28u: goto block_80146C28;
            case 0x80146C34u: goto block_80146C34;
            case 0x80146C44u: goto block_80146C44;
            case 0x80146C58u: goto block_80146C58;
            case 0x80146C68u: goto block_80146C68;
            case 0x80146C74u: goto block_80146C74;
            case 0x80146C90u: goto block_80146C90;
            case 0x80146C94u: goto block_80146C94;
            case 0x80146C98u: goto block_80146C98;
            case 0x80146CACu: goto block_80146CAC;
            case 0x80146CC8u: goto block_80146CC8;
            case 0x80146CD0u: goto block_80146CD0;
            case 0x80146CD8u: goto block_80146CD8;
            case 0x80146CE0u: goto block_80146CE0;
            case 0x80146CE8u: goto block_80146CE8;
            case 0x80146CF0u: goto block_80146CF0;
            case 0x80146D34u: goto block_80146D34;
            case 0x80146D94u: goto block_80146D94;
            case 0x80146DA4u: goto block_80146DA4;
            case 0x80146DB4u: goto block_80146DB4;
            default: break;
        }
    }
    /* Address: 0x801468EC, Size: 1356 bytes, Blocks: 67 */

block_801468EC:
    cpu->gpr[29] = cpu->gpr[29] + -96;
    cpu->write_word(cpu->gpr[29] + 72, cpu->gpr[20]);
    cpu->gpr[20] = cpu->gpr[7];
    cpu->write_word(cpu->gpr[29] + 92, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[30]);
    cpu->write_word(cpu->gpr[29] + 84, cpu->gpr[23]);
    cpu->write_word(cpu->gpr[29] + 80, cpu->gpr[22]);
    cpu->write_word(cpu->gpr[29] + 76, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 68, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 64, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 60, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[16]);
    cpu->write_word(cpu->gpr[29] + 96, cpu->gpr[4]);
    cpu->write_word(cpu->gpr[29] + 100, cpu->gpr[5]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[20] + 44);
    cpu->gpr[22] = cpu->read_word(cpu->gpr[29] + 112);
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[8]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[20] + 20);
    cpu->gpr[30] = cpu->read_word(cpu->gpr[20] + 32);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[8]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[20] + 16);
    /* nop */  /* 0x80146944: 0x00000000 */
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[8]);
    cpu->gpr[12] = cpu->read_word(cpu->gpr[6]);
    cpu->gpr[13] = cpu->read_word(cpu->gpr[6] + 4);
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[0] = cpu->gpr[12];  /* ctc2 */  /* 0x80146954: 0x48CC0000 */
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[1] = cpu->gpr[13];  /* ctc2 */  /* 0x80146958: 0x48CD0800 */
    cpu->gpr[12] = cpu->read_word(cpu->gpr[6] + 8);
    cpu->gpr[13] = cpu->read_word(cpu->gpr[6] + 12);
    cpu->gpr[14] = cpu->read_word(cpu->gpr[6] + 16);
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[2] = cpu->gpr[12];  /* ctc2 */  /* 0x80146968: 0x48CC1000 */
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[3] = cpu->gpr[13];  /* ctc2 */  /* 0x8014696C: 0x48CD1800 */
    
    psx_gte_stall(cpu);
    gte_write_ctrl(cpu, 4, cpu->gpr[14]);  /* ctc2 */  /* 0x80146970: 0x48CE2000 */
    cpu->gpr[12] = cpu->read_word(cpu->gpr[6] + 20);
    cpu->gpr[13] = cpu->read_word(cpu->gpr[6] + 24);
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[5] = cpu->gpr[12];  /* ctc2 */  /* 0x8014697C: 0x48CC2800 */
    cpu->gpr[14] = cpu->read_word(cpu->gpr[6] + 28);
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[6] = cpu->gpr[13];  /* ctc2 */  /* 0x80146984: 0x48CD3000 */
    
    psx_gte_stall(cpu);
    cpu->gte_ctrl[7] = cpu->gpr[14];  /* ctc2 */  /* 0x80146988: 0x48CE3800 */
    cpu->gpr[2] = (cpu->gpr[22] < (uint32_t)6) ? 1 : 0;  /* 0x8014698C: 0x2EC20006 */
    int _bc_80146990 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_80146990) {
        psx_check_interrupts_at(cpu, 0x801469E0u);
        goto block_801469E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146998u);
        goto block_80146998;  /* not taken */
    }

block_80146998:
    cpu->gpr[2] = cpu->gpr[2] + 716;
    cpu->gpr[3] = cpu->gpr[22] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x801469A8: 0x00000000 */
    uint32_t _jt_801469AC = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801469B0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801469AC);
    cpu->pc = _jt_801469AC; return;  /* CPS: jr cpu->gpr[2] */

block_801469B4:
    /* delay slot (always executes) */
    cpu->gpr[22] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801469E0u);
    goto block_801469E0;  /* j */

block_801469BC:
    /* delay slot (always executes) */
    cpu->gpr[22] = 4;
    psx_check_interrupts_at(cpu, 0x801469E0u);
    goto block_801469E0;  /* j */

block_801469C4:
    /* delay slot (always executes) */
    cpu->gpr[22] = 2;
    psx_check_interrupts_at(cpu, 0x801469E0u);
    goto block_801469E0;  /* j */

block_801469CC:
    /* delay slot (always executes) */
    cpu->gpr[22] = 6;
    psx_check_interrupts_at(cpu, 0x801469E0u);
    goto block_801469E0;  /* j */

block_801469D4:
    /* delay slot (always executes) */
    cpu->gpr[22] = 1;
    psx_check_interrupts_at(cpu, 0x801469E0u);
    goto block_801469E0;  /* j */

block_801469DC:
    cpu->gpr[22] = 7;
    /* fall through to block_801469E0 */

block_801469E0:
    int _bc_801469E0 = (cpu->gpr[22] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 26;
    if (_bc_801469E0) {
        psx_check_interrupts_at(cpu, 0x80146A00u);
        goto block_80146A00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801469E8u);
        goto block_801469E8;  /* not taken */
    }

block_801469E8:
    cpu->gpr[6] = cpu->gpr[30] + 52;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[7] = cpu->gpr[0];
    cpu->gpr[31] = 0x80146A00u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80136F70u);
    cpu->pc = 0x80136F70u; return;  /* CPS jal -> 0x80136F70 */

block_80146A00:
    cpu->gpr[2] = cpu->gpr[22] & 0x0001;  /* 0x80146A00: 0x32C20001 */
    int _bc_80146A04 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x0020 << 16;  /* 0x00200000 */
    if (_bc_80146A04) {
        psx_check_interrupts_at(cpu, 0x80146AE4u);
        goto block_80146AE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A0Cu);
        goto block_80146A0C;  /* not taken */
    }

block_80146A0C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 96);
    /* nop */  /* 0x80146A10: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x80146A14: 0x00431024 */
    int _bc_80146A18 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[22] >> 1;
    if (_bc_80146A18) {
        psx_check_interrupts_at(cpu, 0x80146AE8u);
        goto block_80146AE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A20u);
        goto block_80146A20;  /* not taken */
    }

block_80146A20:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 88);
    cpu->gpr[4] = 0x0003 << 16;  /* 0x00030000 */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[4];  /* 0x80146A28: 0x00641024 */
    int _bc_80146A2C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[0];
    if (_bc_80146A2C) {
        psx_check_interrupts_at(cpu, 0x80146A44u);
        goto block_80146A44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A34u);
        goto block_80146A34;  /* not taken */
    }

block_80146A34:
    cpu->gpr[2] = 0x0800 << 16;  /* 0x08000000 */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146A38: 0x00621024 */
    int _bc_80146A3C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146A40: 0x00000000 */
    if (_bc_80146A3C) {
        psx_check_interrupts_at(cpu, 0x80146A6Cu);
        goto block_80146A6C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A44u);
        goto block_80146A44;  /* not taken */
    }

block_80146A44:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 40);
    /* nop */  /* 0x80146A48: 0x00000000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[8] + 88);
    /* nop */  /* 0x80146A50: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[4];  /* 0x80146A54: 0x00641024 */
    int _bc_80146A58 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_80146A58) {
        psx_check_interrupts_at(cpu, 0x80146A70u);
        goto block_80146A70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A60u);
        goto block_80146A60;  /* not taken */
    }

block_80146A60:
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146A60: 0x00621024 */
    int _bc_80146A64 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146A68: 0x00000000 */
    if (_bc_80146A64) {
        psx_check_interrupts_at(cpu, 0x80146A70u);
        goto block_80146A70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A6Cu);
        goto block_80146A6C;  /* not taken */
    }

block_80146A6C:
    cpu->gpr[18] = 1;
    /* fall through to block_80146A70 */

block_80146A70:
    int _bc_80146A70 = (cpu->gpr[18] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[0];
    if (_bc_80146A70) {
        psx_check_interrupts_at(cpu, 0x80146A80u);
        goto block_80146A80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A78u);
        goto block_80146A78;  /* not taken */
    }

block_80146A78:
    /* delay slot (always executes) */
    cpu->gpr[19] = 1;
    psx_check_interrupts_at(cpu, 0x80146A88u);
    goto block_80146A88;  /* j */

block_80146A80:
    cpu->gpr[18] = 1;
    cpu->gpr[19] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[20] + 9);
    /* fall through to block_80146A88 */

block_80146A88:
    /* nop */  /* 0x80146A88: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < (int32_t)cpu->gpr[19]) ? 1 : 0;  /* 0x80146A8C: 0x0253102A */
    int _bc_80146A90 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[18] << 2;
    if (_bc_80146A90) {
        psx_check_interrupts_at(cpu, 0x80146AE4u);
        goto block_80146AE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146A98u);
        goto block_80146A98;  /* not taken */
    }

block_80146A98:
    cpu->gpr[17] = cpu->gpr[2] + cpu->gpr[16];
    cpu->gpr[7] = 2;
    /* fall through to block_80146AA0 */

block_80146AA0:
    cpu->gpr[16] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[17] + 1);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 2);
    cpu->gpr[17] = cpu->gpr[17] + 4;
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[6] = cpu->gpr[16] << (cpu->gpr[7] & 0x1F);
    cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[16];
    cpu->gpr[6] = cpu->gpr[6] << 4;
    cpu->gpr[6] = cpu->gpr[30] + cpu->gpr[6];
    cpu->gpr[6] = cpu->gpr[6] + 52;
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[31] = 0x80146AD8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80136F70u);
    cpu->pc = 0x80136F70u; return;  /* CPS jal -> 0x80136F70 */

block_80146AD8:
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < (int32_t)cpu->gpr[19]) ? 1 : 0;  /* 0x80146AD8: 0x0253102A */
    int _bc_80146ADC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 2;
    if (_bc_80146ADC) {
        psx_check_interrupts_at(cpu, 0x80146AA0u);
        goto block_80146AA0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146AE4u);
        goto block_80146AE4;  /* not taken */
    }

block_80146AE4:
    cpu->gpr[2] = (int32_t)cpu->gpr[22] >> 1;
    /* fall through to block_80146AE8 */

block_80146AE8:
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80146AE8: 0x30420001 */
    int _bc_80146AEC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[22] >> 2;
    if (_bc_80146AEC) {
        psx_check_interrupts_at(cpu, 0x80146C28u);
        goto block_80146C28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146AF4u);
        goto block_80146AF4;  /* not taken */
    }

block_80146AF4:
    cpu->gpr[2] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[20] + 39);
    /* nop */  /* 0x80146AF8: 0x00000000 */
    int _bc_80146AFC = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x0004 << 16;  /* 0x00040000 */
    if (_bc_80146AFC) {
        psx_check_interrupts_at(cpu, 0x80146BC4u);
        goto block_80146BC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B04u);
        goto block_80146B04;  /* not taken */
    }

block_80146B04:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 88);
    /* nop */  /* 0x80146B08: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146B0C: 0x00621024 */
    int _bc_80146B10 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[23] = cpu->gpr[0];
    if (_bc_80146B10) {
        psx_check_interrupts_at(cpu, 0x80146B4Cu);
        goto block_80146B4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B18u);
        goto block_80146B18;  /* not taken */
    }

block_80146B18:
    cpu->gpr[2] = 0x0003 << 16;  /* 0x00030000 */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146B1C: 0x00621024 */
    int _bc_80146B20 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_80146B20) {
        psx_check_interrupts_at(cpu, 0x80146B50u);
        goto block_80146B50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B28u);
        goto block_80146B28;  /* not taken */
    }

block_80146B28:
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146B28: 0x00621024 */
    int _bc_80146B2C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8000 << 16;  /* 0x80000000 */
    if (_bc_80146B2C) {
        psx_check_interrupts_at(cpu, 0x80146B50u);
        goto block_80146B50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B34u);
        goto block_80146B34;  /* not taken */
    }

block_80146B34:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 92);
    cpu->gpr[2] = cpu->gpr[2] | 0x0080;
    cpu->gpr[3] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146B3C: 0x00621824 */
    cpu->gpr[2] = 128;
    int _bc_80146B44 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146B48: 0x00000000 */
    if (_bc_80146B44) {
        psx_check_interrupts_at(cpu, 0x80146B50u);
        goto block_80146B50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B4Cu);
        goto block_80146B4C;  /* not taken */
    }

block_80146B4C:
    cpu->gpr[23] = 1;
    /* fall through to block_80146B50 */

block_80146B50:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 40);
    /* nop */  /* 0x80146B54: 0x00000000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[8] + 100);
    cpu->gpr[19] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[20] + 10);
    int _bc_80146B60 = (cpu->gpr[23] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[21] = cpu->gpr[2] & 0x1000;  /* 0x80146B64: 0x30551000 */
    if (_bc_80146B60) {
        psx_check_interrupts_at(cpu, 0x80146B70u);
        goto block_80146B70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B68u);
        goto block_80146B68;  /* not taken */
    }

block_80146B68:
    int _bc_80146B68 = (cpu->gpr[21] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[22] >> 1;
    if (_bc_80146B68) {
        psx_check_interrupts_at(cpu, 0x80146BC8u);
        goto block_80146BC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B70u);
        goto block_80146B70;  /* not taken */
    }

block_80146B70:
    int _bc_80146B70 = ((int32_t)cpu->gpr[19] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[0];
    if (_bc_80146B70) {
        psx_check_interrupts_at(cpu, 0x80146BC4u);
        goto block_80146BC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146B78u);
        goto block_80146B78;  /* not taken */
    }

block_80146B78:
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[7] = 1;
    /* fall through to block_80146B80 */

block_80146B80:
    cpu->gpr[18] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[17] + 1);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 2);
    cpu->gpr[17] = cpu->gpr[17] + 4;
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[7];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[6] = cpu->gpr[18] << 2;
    cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[18];
    cpu->gpr[6] = cpu->gpr[6] << 4;
    cpu->gpr[6] = cpu->gpr[30] + cpu->gpr[6];
    cpu->gpr[6] = cpu->gpr[6] + 52;
    cpu->gpr[2] = cpu->gpr[2] << (cpu->gpr[7] & 0x1F);
    cpu->gpr[31] = 0x80146BB8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80136F70u);
    cpu->pc = 0x80136F70u; return;  /* CPS jal -> 0x80136F70 */

block_80146BB8:
    cpu->gpr[2] = ((int32_t)cpu->gpr[16] < (int32_t)cpu->gpr[19]) ? 1 : 0;  /* 0x80146BB8: 0x0213102A */
    int _bc_80146BBC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 1;
    if (_bc_80146BBC) {
        psx_check_interrupts_at(cpu, 0x80146B80u);
        goto block_80146B80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146BC4u);
        goto block_80146BC4;  /* not taken */
    }

block_80146BC4:
    cpu->gpr[2] = (int32_t)cpu->gpr[22] >> 1;
    /* fall through to block_80146BC8 */

block_80146BC8:
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80146BC8: 0x30420001 */
    int _bc_80146BCC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x2000 << 16;  /* 0x20000000 */
    if (_bc_80146BCC) {
        psx_check_interrupts_at(cpu, 0x80146C24u);
        goto block_80146C24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146BD4u);
        goto block_80146BD4;  /* not taken */
    }

block_80146BD4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 92);
    /* nop */  /* 0x80146BD8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x80146BDC: 0x00431024 */
    int _bc_80146BE0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1024;
    if (_bc_80146BE0) {
        psx_check_interrupts_at(cpu, 0x80146C24u);
        goto block_80146C24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146BE8u);
        goto block_80146BE8;  /* not taken */
    }

block_80146BE8:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[6] = cpu->read_word(cpu->gpr[20] + 32);
    cpu->gpr[7] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[20] + 196);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[20] + 78);
    cpu->gpr[6] = cpu->gpr[6] + 52;
    cpu->gpr[7] = cpu->gpr[7] + -1024;
    cpu->gpr[7] = cpu->gpr[7] << 20;
    cpu->gpr[7] = (int32_t)cpu->gpr[7] >> 20;
    cpu->gpr[2] = cpu->gpr[2] + -1024;
    cpu->gpr[2] = cpu->gpr[2] << 20;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    cpu->gpr[31] = 0x80146C24u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80136F80u);
    cpu->pc = 0x80136F80u; return;  /* CPS jal -> 0x80136F80 */

block_80146C24:
    cpu->gpr[2] = (int32_t)cpu->gpr[22] >> 2;
    /* fall through to block_80146C28 */

block_80146C28:
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80146C28: 0x30420001 */
    int _bc_80146C2C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146C30: 0x00000000 */
    if (_bc_80146C2C) {
        psx_check_interrupts_at(cpu, 0x80146E00u);
        goto block_80146E00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146C34u);
        goto block_80146C34;  /* not taken */
    }

block_80146C34:
    cpu->gpr[2] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[20] + 38);
    /* nop */  /* 0x80146C38: 0x00000000 */
    int _bc_80146C3C = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x0004 << 16;  /* 0x00040000 */
    if (_bc_80146C3C) {
        psx_check_interrupts_at(cpu, 0x80146D94u);
        goto block_80146D94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146C44u);
        goto block_80146C44;  /* not taken */
    }

block_80146C44:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 88);
    /* nop */  /* 0x80146C48: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146C4C: 0x00621024 */
    int _bc_80146C50 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[23] = cpu->gpr[0];
    if (_bc_80146C50) {
        psx_check_interrupts_at(cpu, 0x80146C90u);
        goto block_80146C90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146C58u);
        goto block_80146C58;  /* not taken */
    }

block_80146C58:
    cpu->gpr[2] = 0x0003 << 16;  /* 0x00030000 */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146C5C: 0x00621024 */
    int _bc_80146C60 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x0800 << 16;  /* 0x08000000 */
    if (_bc_80146C60) {
        psx_check_interrupts_at(cpu, 0x80146C94u);
        goto block_80146C94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146C68u);
        goto block_80146C68;  /* not taken */
    }

block_80146C68:
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146C68: 0x00621024 */
    int _bc_80146C6C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[0];
    if (_bc_80146C6C) {
        psx_check_interrupts_at(cpu, 0x80146C98u);
        goto block_80146C98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146C74u);
        goto block_80146C74;  /* not taken */
    }

block_80146C74:
    cpu->gpr[2] = 0x8000 << 16;  /* 0x80000000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 92);
    cpu->gpr[2] = cpu->gpr[2] | 0x0080;
    cpu->gpr[3] = cpu->gpr[3] & cpu->gpr[2];  /* 0x80146C80: 0x00621824 */
    cpu->gpr[2] = 128;
    int _bc_80146C88 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146C8C: 0x00000000 */
    if (_bc_80146C88) {
        psx_check_interrupts_at(cpu, 0x80146C98u);
        goto block_80146C98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146C90u);
        goto block_80146C90;  /* not taken */
    }

block_80146C90:
    cpu->gpr[23] = 1;
    /* fall through to block_80146C94 */

block_80146C94:
    cpu->gpr[18] = cpu->gpr[0];
    /* fall through to block_80146C98 */

block_80146C98:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[19] = cpu->gpr[18];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[8] + 100);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 48);
    cpu->gpr[21] = cpu->gpr[2] & 0x1000;  /* 0x80146CA8: 0x30551000 */
    /* fall through to block_80146CAC */

block_80146CAC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 20);
    /* nop */  /* 0x80146CB0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[19] + cpu->gpr[2];
    cpu->gpr[16] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[2]);
    cpu->gpr[2] = -1;
    int _bc_80146CC0 = (cpu->gpr[16] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = -2;
    if (_bc_80146CC0) {
        psx_check_interrupts_at(cpu, 0x80146D94u);
        goto block_80146D94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146CC8u);
        goto block_80146CC8;  /* not taken */
    }

block_80146CC8:
    int _bc_80146CC8 = (cpu->gpr[16] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146CCC: 0x00000000 */
    if (_bc_80146CC8) {
        psx_check_interrupts_at(cpu, 0x80146D80u);
        goto block_80146D80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146CD0u);
        goto block_80146CD0;  /* not taken */
    }

block_80146CD0:
    int _bc_80146CD0 = (cpu->gpr[23] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146CD4: 0x00000000 */
    if (_bc_80146CD0) {
        psx_check_interrupts_at(cpu, 0x80146CE8u);
        goto block_80146CE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146CD8u);
        goto block_80146CD8;  /* not taken */
    }

block_80146CD8:
    int _bc_80146CD8 = (cpu->gpr[21] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 3;
    if (_bc_80146CD8) {
        psx_check_interrupts_at(cpu, 0x80146D80u);
        goto block_80146D80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146CE0u);
        goto block_80146CE0;  /* not taken */
    }

block_80146CE0:
    /* delay slot (always executes) */
    /* nop */  /* 0x80146CE4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80146CF0u);
    goto block_80146CF0;  /* j */

block_80146CE8:
    int _bc_80146CE8 = (cpu->gpr[21] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 3;
    if (_bc_80146CE8) {
        psx_check_interrupts_at(cpu, 0x80146D80u);
        goto block_80146D80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146CF0u);
        goto block_80146CF0;  /* not taken */
    }

block_80146CF0:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[16] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[17] + 1);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 2);
    cpu->gpr[6] = cpu->gpr[16] << 2;
    cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[16];
    cpu->gpr[6] = cpu->gpr[6] << 4;
    cpu->gpr[6] = cpu->gpr[30] + cpu->gpr[6];
    cpu->gpr[6] = cpu->gpr[6] + 52;
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[31] = 0x80146D20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80136F70u);
    cpu->pc = 0x80136F70u; return;  /* CPS jal -> 0x80136F70 */

block_80146D20:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 108);
    /* nop */  /* 0x80146D24: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0800;  /* 0x80146D28: 0x30420800 */
    int _bc_80146D2C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[16] << 3;
    if (_bc_80146D2C) {
        psx_check_interrupts_at(cpu, 0x80146D80u);
        goto block_80146D80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146D34u);
        goto block_80146D34;  /* not taken */
    }

block_80146D34:
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[16];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 64);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[3] + 14);
    /* nop */  /* 0x80146D4C: 0x00000000 */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[3] + 16);
    cpu->gpr[6] = cpu->gpr[29] + 24;
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[3] + 18);
    cpu->gpr[7] = 5;
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 2);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[31] = 0x80146D80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80136F70u);
    cpu->pc = 0x80136F70u; return;  /* CPS jal -> 0x80136F70 */

block_80146D80:
    cpu->gpr[17] = cpu->gpr[17] + 4;
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < 8) ? 1 : 0;  /* 0x80146D88: 0x2A420008 */
    int _bc_80146D8C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[19] = cpu->gpr[19] + 4;
    if (_bc_80146D8C) {
        psx_check_interrupts_at(cpu, 0x80146CACu);
        goto block_80146CAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146D94u);
        goto block_80146D94;  /* not taken */
    }

block_80146D94:
    cpu->gpr[2] = (int32_t)cpu->gpr[22] >> 2;
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80146D98: 0x30420001 */
    int _bc_80146D9C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146DA0: 0x00000000 */
    if (_bc_80146D9C) {
        psx_check_interrupts_at(cpu, 0x80146E00u);
        goto block_80146E00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146DA4u);
        goto block_80146DA4;  /* not taken */
    }

block_80146DA4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 88);
    /* nop */  /* 0x80146DA8: 0x00000000 */
    int _bc_80146DAC = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146DB0: 0x00000000 */
    if (_bc_80146DAC) {
        psx_check_interrupts_at(cpu, 0x80146E00u);
        goto block_80146E00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80146DB4u);
        goto block_80146DB4;  /* not taken */
    }

block_80146DB4:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[6] = cpu->read_word(cpu->gpr[20] + 32);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[7] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[20] + 78);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 219);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[20] + 198);
    cpu->gpr[6] = cpu->gpr[6] + 52;
    cpu->gpr[2] = cpu->gpr[2] << 4;
    cpu->gpr[7] = cpu->gpr[7] + cpu->gpr[3];
    cpu->gpr[7] = cpu->gpr[7] + -1024;
    cpu->gpr[7] = cpu->gpr[7] << 20;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[20] + 78);
    cpu->gpr[7] = (int32_t)cpu->gpr[7] >> 20;
    cpu->gpr[2] = cpu->gpr[2] + -1024;
    cpu->gpr[2] = cpu->gpr[2] << 20;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    cpu->gpr[31] = 0x80146E00u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80136F80u);
    cpu->pc = 0x80136F80u; return;  /* CPS jal -> 0x80136F80 */

block_80146E00:
    cpu->gpr[31] = 0x80146E08u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80146E04: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80136F78u);
    cpu->pc = 0x80136F78u; return;  /* CPS jal -> 0x80136F78 */

block_80146E08:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 92);
    cpu->gpr[30] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[23] = cpu->read_word(cpu->gpr[29] + 84);
    cpu->gpr[22] = cpu->read_word(cpu->gpr[29] + 80);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 76);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 72);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 68);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 64);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 60);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 56);
    uint32_t _jt_80146E30 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 96;
    psx_check_interrupts_at(cpu, _jt_80146E30);
    cpu->pc = _jt_80146E30; return;  /* CPS: jr $ra */
}

/* cps_801565ec — clean CPS transliteration of br2eu_func_801565EC (guest 0x801565ec)
 * from SLES_017.22_full_19.c. */
void cps_801565ec(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80156614u: goto block_80156614;
            case 0x8015661Cu: goto block_8015661C;
            case 0x80156628u: goto block_80156628;
            case 0x80156630u: goto block_80156630;
            case 0x80156638u: goto block_80156638;
            default: break;
        }
    }
    /* Address: 0x801565EC, Size: 100 bytes, Blocks: 6 */

block_801565EC:
    cpu->gpr[29] = cpu->gpr[29] + -32;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[31]);
    cpu->gpr[31] = 0x80156614u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x8019D4D0u);
    cpu->pc = 0x8019D4D0u; return;  /* CPS jal -> 0x8019D4D0 */

block_80156614:
    cpu->gpr[31] = 0x8015661Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x8019D470u);
    cpu->pc = 0x8019D470u; return;  /* CPS jal -> 0x8019D470 */

block_8015661C:
    cpu->gpr[4] = cpu->gpr[17];
    cpu->gpr[31] = 0x80156628u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[18];
    psx_check_interrupts_at(cpu, 0x8019D410u);
    cpu->pc = 0x8019D410u; return;  /* CPS jal -> 0x8019D410 */

block_80156628:
    cpu->gpr[31] = 0x80156630u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x8019D500u);
    cpu->pc = 0x8019D500u; return;  /* CPS jal -> 0x8019D500 */

block_80156630:
    int _bc_80156630 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[18];
    if (_bc_80156630) {
        psx_check_interrupts_at(cpu, 0x80156628u);
        goto block_80156628;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80156638u);
        goto block_80156638;  /* not taken */
    }

block_80156638:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_80156648 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 32;
    psx_check_interrupts_at(cpu, _jt_80156648);
    cpu->pc = _jt_80156648; return;  /* CPS: jr $ra */
}

/* cps_801587c0 — clean CPS transliteration of br2eu_func_801587C0 (guest 0x801587c0)
 * from SLES_017.22_full_20.c. */
void cps_801587c0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8015895Cu: goto block_8015895C;
            case 0x80158994u: goto block_80158994;
            case 0x80158A1Cu: goto block_80158A1C;
            case 0x80158A5Cu: goto block_80158A5C;
            case 0x80158A84u: goto block_80158A84;
            case 0x80158AA4u: goto block_80158AA4;
            case 0x80158AFCu: goto block_80158AFC;
            case 0x80158B58u: goto block_80158B58;
            case 0x80158B9Cu: goto block_80158B9C;
            case 0x80158BD8u: goto block_80158BD8;
            case 0x80158C44u: goto block_80158C44;
            case 0x80158CB0u: goto block_80158CB0;
            case 0x80158D00u: goto block_80158D00;
            case 0x80158D48u: goto block_80158D48;
            case 0x80158808u: goto block_80158808;
            case 0x80158814u: goto block_80158814;
            case 0x8015881Cu: goto block_8015881C;
            case 0x80158824u: goto block_80158824;
            case 0x80158828u: goto block_80158828;
            case 0x8015882Cu: goto block_8015882C;
            case 0x801588A4u: goto block_801588A4;
            case 0x801588C4u: goto block_801588C4;
            case 0x801588D4u: goto block_801588D4;
            case 0x801588E4u: goto block_801588E4;
            case 0x801588F4u: goto block_801588F4;
            case 0x80158904u: goto block_80158904;
            case 0x80158910u: goto block_80158910;
            case 0x8015891Cu: goto block_8015891C;
            case 0x801589A4u: goto block_801589A4;
            case 0x801589ACu: goto block_801589AC;
            case 0x801589B4u: goto block_801589B4;
            case 0x801589BCu: goto block_801589BC;
            case 0x801589C8u: goto block_801589C8;
            case 0x801589D0u: goto block_801589D0;
            case 0x80158AACu: goto block_80158AAC;
            case 0x80158B08u: goto block_80158B08;
            case 0x80158B64u: goto block_80158B64;
            case 0x80158BA8u: goto block_80158BA8;
            case 0x80158BE4u: goto block_80158BE4;
            case 0x80158BFCu: goto block_80158BFC;
            case 0x80158C50u: goto block_80158C50;
            case 0x80158CBCu: goto block_80158CBC;
            case 0x80158D0Cu: goto block_80158D0C;
            case 0x80158D54u: goto block_80158D54;
            case 0x80158D78u: goto block_80158D78;
            default: break;
        }
    }
    /* Address: 0x801587C0, Size: 1512 bytes, Blocks: 46 */

block_801587C0:
    cpu->gpr[29] = cpu->gpr[29] + -88;
    cpu->write_word(cpu->gpr[29] + 64, cpu->gpr[20]);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->write_word(cpu->gpr[29] + 60, cpu->gpr[19]);
    cpu->gpr[19] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 84, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 80, cpu->gpr[30]);
    cpu->write_word(cpu->gpr[29] + 76, cpu->gpr[23]);
    cpu->write_word(cpu->gpr[29] + 72, cpu->gpr[22]);
    cpu->write_word(cpu->gpr[29] + 68, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 52, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[16]);
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[4]);
    cpu->write_word(cpu->gpr[29] + 96, cpu->gpr[6]);
    cpu->gpr[3] = cpu->gpr[20] & 0x00F0;  /* 0x801587FC: 0x328300F0 */
    int _bc_80158800 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 100, cpu->gpr[7]);
    if (_bc_80158800) {
        psx_check_interrupts_at(cpu, 0x8015881Cu);
        goto block_8015881C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158808u);
        goto block_80158808;  /* not taken */
    }

block_80158808:
    cpu->gpr[2] = 16;
    int _bc_8015880C = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_8015880C) {
        psx_check_interrupts_at(cpu, 0x80158824u);
        goto block_80158824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158814u);
        goto block_80158814;  /* not taken */
    }

block_80158814:
    /* delay slot (always executes) */
    /* nop */  /* 0x80158818: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015882Cu);
    goto block_8015882C;  /* j */

block_8015881C:
    /* delay slot (always executes) */
    cpu->gpr[8] = 128;
    psx_check_interrupts_at(cpu, 0x80158828u);
    goto block_80158828;  /* j */

block_80158824:
    cpu->gpr[8] = 208;
    /* fall through to block_80158828 */

block_80158828:
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    /* fall through to block_8015882C */

block_8015882C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 18168);
    /* nop */  /* 0x80158830: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[5] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 14056);
    /* nop */  /* 0x80158844: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[4] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 14052);
    /* nop */  /* 0x80158858: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 18160);
    /* nop */  /* 0x8015886C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[5] = (uint32_t)cpu->gpr[5] >> 15;
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[5]);
    cpu->gpr[4] = (uint32_t)cpu->gpr[4] >> 15;
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[4]);
    cpu->gpr[3] = (uint32_t)cpu->gpr[3] >> 15;
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[3]);
    cpu->gpr[3] = cpu->gpr[20] & 0x000F;  /* 0x8015888C: 0x3283000F */
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)5) ? 1 : 0;  /* 0x80158890: 0x2C620005 */
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[8] = (uint32_t)cpu->gpr[8] >> 15;
    int _bc_8015889C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[8]);
    if (_bc_8015889C) {
        psx_check_interrupts_at(cpu, 0x8015891Cu);
        goto block_8015891C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801588A4u);
        goto block_801588A4;  /* not taken */
    }

block_801588A4:
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[2] = cpu->gpr[2] + 3532;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x801588B8: 0x00000000 */
    uint32_t _jt_801588BC = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801588C0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801588BC);
    cpu->pc = _jt_801588BC; return;  /* CPS: jr cpu->gpr[2] */

block_801588C4:
    cpu->gpr[17] = 255;
    cpu->gpr[21] = cpu->gpr[17];
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80158910u);
    goto block_80158910;  /* j */

block_801588D4:
    cpu->gpr[17] = 128;
    cpu->gpr[21] = 255;
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[17];
    psx_check_interrupts_at(cpu, 0x80158910u);
    goto block_80158910;  /* j */

block_801588E4:
    cpu->gpr[17] = 255;
    cpu->gpr[21] = 128;
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[21];
    psx_check_interrupts_at(cpu, 0x80158910u);
    goto block_80158910;  /* j */

block_801588F4:
    cpu->gpr[17] = 128;
    cpu->gpr[21] = cpu->gpr[17];
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[17];
    psx_check_interrupts_at(cpu, 0x80158910u);
    goto block_80158910;  /* j */

block_80158904:
    cpu->gpr[17] = 128;
    cpu->gpr[21] = cpu->gpr[17];
    cpu->gpr[18] = 255;
    /* fall through to block_80158910 */

block_80158910:
    cpu->gpr[23] = 96;
    cpu->gpr[22] = cpu->gpr[0];
    cpu->gpr[30] = cpu->gpr[0];
    /* fall through to block_8015891C */

block_8015891C:
    cpu->gpr[5] = cpu->gpr[19];
    cpu->gpr[6] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[6] = cpu->gpr[6] + 3520;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[16] = cpu->gpr[10] << 16;
    cpu->gpr[16] = (int32_t)cpu->gpr[16] >> 16;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[7] = cpu->gpr[16];
    cpu->gpr[2] = cpu->gpr[10] << 16;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->gpr[2] = (cpu->gpr[10] < (uint32_t)1) ? 1 : 0;  /* 0x80158950: 0x2D420001 */
    cpu->gpr[31] = 0x8015895Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8012E7CCu);
    cpu->pc = 0x8012E7CCu; return;  /* CPS jal -> 0x8012E7CC */

block_8015895C:
    cpu->gpr[5] = cpu->gpr[19];
    cpu->gpr[6] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[6] = cpu->gpr[6] + 3524;
    cpu->gpr[7] = cpu->gpr[16];
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[19] = cpu->gpr[10] + 9;
    cpu->gpr[2] = cpu->gpr[19] << 16;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->gpr[2] = (cpu->gpr[10] < (uint32_t)1) ? 1 : 0;  /* 0x80158988: 0x2D420001 */
    cpu->gpr[31] = 0x80158994u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8012E7CCu);
    cpu->pc = 0x8012E7CCu; return;  /* CPS jal -> 0x8012E7CC */

block_80158994:
    cpu->gpr[3] = cpu->gpr[20] & 0x00F0;  /* 0x80158994: 0x328300F0 */
    cpu->gpr[2] = 16;
    int _bc_8015899C = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 17) ? 1 : 0;  /* 0x801589A0: 0x28620011 */
    if (_bc_8015899C) {
        psx_check_interrupts_at(cpu, 0x80158AACu);
        goto block_80158AAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801589A4u);
        goto block_801589A4;  /* not taken */
    }

block_801589A4:
    int _bc_801589A4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801589A8: 0x00000000 */
    if (_bc_801589A4) {
        psx_check_interrupts_at(cpu, 0x801589BCu);
        goto block_801589BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801589ACu);
        goto block_801589AC;  /* not taken */
    }

block_801589AC:
    int _bc_801589AC = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[19] = 4;
    if (_bc_801589AC) {
        psx_check_interrupts_at(cpu, 0x801589D0u);
        goto block_801589D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801589B4u);
        goto block_801589B4;  /* not taken */
    }

block_801589B4:
    /* delay slot (always executes) */
    /* nop */  /* 0x801589B8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80158D78u);
    goto block_80158D78;  /* j */

block_801589BC:
    cpu->gpr[2] = 32;
    int _bc_801589C0 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[19];
    if (_bc_801589C0) {
        psx_check_interrupts_at(cpu, 0x80158BFCu);
        goto block_80158BFC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801589C8u);
        goto block_801589C8;  /* not taken */
    }

block_801589C8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801589CC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80158D78u);
    goto block_80158D78;  /* j */

block_801589D0:
    cpu->gpr[17] = cpu->gpr[17] & 0x00FF;  /* 0x801589D0: 0x323100FF */
    cpu->gpr[2] = cpu->gpr[21] & 0x00FF;  /* 0x801589D4: 0x32A200FF */
    cpu->gpr[2] = cpu->gpr[2] << 8;
    cpu->gpr[17] = cpu->gpr[17] | cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[18] & 0x00FF;  /* 0x801589E0: 0x324200FF */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[17] = cpu->gpr[17] | cpu->gpr[2];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    cpu->gpr[20] = cpu->gpr[10] + 16;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[16] = cpu->gpr[10] + 2;
    cpu->gpr[31] = 0x80158A1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x8018CC6Cu);
    cpu->pc = 0x8018CC6Cu; return;  /* CPS jal -> 0x8018CC6C */

block_80158A1C:
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[6] = cpu->gpr[16];
    cpu->gpr[16] = cpu->gpr[23] & 0x00FF;  /* 0x80158A24: 0x32F000FF */
    cpu->gpr[2] = cpu->gpr[22] & 0x00FF;  /* 0x80158A28: 0x32C200FF */
    cpu->gpr[2] = cpu->gpr[2] << 8;
    cpu->gpr[16] = cpu->gpr[16] | cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[30] & 0x00FF;  /* 0x80158A34: 0x33C200FF */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[16] = cpu->gpr[16] | cpu->gpr[2];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[21] = 2;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[16]);
    cpu->gpr[31] = 0x80158A5Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[21]);
    psx_check_interrupts_at(cpu, 0x8018CC6Cu);
    cpu->pc = 0x8018CC6Cu; return;  /* CPS jal -> 0x8018CC6C */

block_80158A5C:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[5] = cpu->gpr[20];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    cpu->gpr[18] = cpu->gpr[10] + 11;
    cpu->gpr[31] = 0x80158A84u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[18];
    psx_check_interrupts_at(cpu, 0x8018CC6Cu);
    cpu->pc = 0x8018CC6Cu; return;  /* CPS jal -> 0x8018CC6C */

block_80158A84:
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[6] = cpu->gpr[18];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[16]);
    cpu->gpr[31] = 0x80158AA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[21]);
    psx_check_interrupts_at(cpu, 0x8018CC6Cu);
    cpu->pc = 0x8018CC6Cu; return;  /* CPS jal -> 0x8018CC6C */

block_80158AA4:
    /* delay slot (always executes) */
    /* nop */  /* 0x80158AA8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80158D78u);
    goto block_80158D78;  /* j */

block_80158AAC:
    cpu->gpr[19] = 4;
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x80158AB0: 0x322200FF */
    cpu->gpr[3] = cpu->gpr[21] & 0x00FF;  /* 0x80158AB4: 0x32A300FF */
    cpu->gpr[3] = cpu->gpr[3] << 8;
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];
    cpu->gpr[3] = cpu->gpr[18] & 0x00FF;  /* 0x80158AC0: 0x324300FF */
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[21] = cpu->gpr[2] | cpu->gpr[3];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    cpu->gpr[20] = cpu->gpr[10] + 16;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[16] = cpu->gpr[10] + 2;
    cpu->gpr[31] = 0x80158AFCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158AFC:
    cpu->gpr[8] = cpu->gpr[2];
    int _bc_80158B00 = (cpu->gpr[8] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[20];
    if (_bc_80158B00) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158B08u);
        goto block_80158B08;  /* not taken */
    }

block_80158B08:
    cpu->gpr[6] = cpu->gpr[16];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[8] + 14);
    cpu->gpr[3] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[3] = cpu->gpr[23] & 0x00FF;  /* 0x80158B24: 0x32E300FF */
    cpu->write_half(cpu->gpr[8] + 14, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[22] & 0x00FF;  /* 0x80158B2C: 0x32C200FF */
    cpu->gpr[2] = cpu->gpr[2] << 8;
    cpu->gpr[3] = cpu->gpr[3] | cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[30] & 0x00FF;  /* 0x80158B38: 0x33C200FF */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[17] = cpu->gpr[3] | cpu->gpr[2];
    cpu->gpr[22] = 2;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[31] = 0x80158B58u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[22]);
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158B58:
    cpu->gpr[8] = cpu->gpr[2];
    int _bc_80158B5C = (cpu->gpr[8] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[20];
    if (_bc_80158B5C) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158B64u);
        goto block_80158B64;  /* not taken */
    }

block_80158B64:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[8] + 14);
    cpu->gpr[16] = cpu->gpr[10] + 11;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[6] = cpu->gpr[16];
    cpu->gpr[3] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->write_half(cpu->gpr[8] + 14, (uint16_t)cpu->gpr[2]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[21]);
    cpu->gpr[31] = 0x80158B9Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158B9C:
    cpu->gpr[8] = cpu->gpr[2];
    int _bc_80158BA0 = (cpu->gpr[8] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[20];
    if (_bc_80158BA0) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158BA8u);
        goto block_80158BA8;  /* not taken */
    }

block_80158BA8:
    cpu->gpr[6] = cpu->gpr[16];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[8] + 22);
    cpu->gpr[3] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[8] + 22, (uint16_t)cpu->gpr[2]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[31] = 0x80158BD8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[22]);
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158BD8:
    cpu->gpr[8] = cpu->gpr[2];
    int _bc_80158BDC = (cpu->gpr[8] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80158BE0: 0x00000000 */
    if (_bc_80158BDC) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158BE4u);
        goto block_80158BE4;  /* not taken */
    }

block_80158BE4:
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[8] + 22);
    cpu->gpr[3] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[8] + 22, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80158D78u);
    goto block_80158D78;  /* j */

block_80158BFC:
    cpu->gpr[20] = 4;
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x80158C00: 0x322200FF */
    cpu->gpr[3] = cpu->gpr[21] & 0x00FF;  /* 0x80158C04: 0x32A300FF */
    cpu->gpr[3] = cpu->gpr[3] << 8;
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];
    cpu->gpr[3] = cpu->gpr[18] & 0x00FF;  /* 0x80158C10: 0x324300FF */
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[18] = cpu->gpr[2] | cpu->gpr[3];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    cpu->gpr[16] = cpu->gpr[10] + -2;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[5] = cpu->gpr[16];
    cpu->gpr[31] = 0x80158C44u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[0] - cpu->gpr[10];
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158C44:
    cpu->gpr[9] = cpu->gpr[2];
    int _bc_80158C48 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[16];
    if (_bc_80158C48) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158C50u);
        goto block_80158C50;  /* not taken */
    }

block_80158C50:
    cpu->gpr[6] = cpu->gpr[19];
    cpu->gpr[21] = 2;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[9] + 14);
    cpu->gpr[7] = cpu->gpr[0] - cpu->gpr[10];
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[9] + 22);
    cpu->gpr[8] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[8];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[8];
    cpu->write_half(cpu->gpr[9] + 22, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->gpr[23] & 0x00FF;  /* 0x80158C80: 0x32E300FF */
    cpu->write_half(cpu->gpr[9] + 14, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[22] & 0x00FF;  /* 0x80158C88: 0x32C200FF */
    cpu->gpr[2] = cpu->gpr[2] << 8;
    cpu->gpr[3] = cpu->gpr[3] | cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[30] & 0x00FF;  /* 0x80158C94: 0x33C200FF */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[17] = cpu->gpr[3] | cpu->gpr[2];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[31] = 0x80158CB0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[21]);
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158CB0:
    cpu->gpr[9] = cpu->gpr[2];
    int _bc_80158CB4 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[19];
    if (_bc_80158CB4) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158CBCu);
        goto block_80158CBC;  /* not taken */
    }

block_80158CBC:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[9] + 14);
    cpu->gpr[8] = cpu->read_half(cpu->gpr[9] + 22);
    cpu->gpr[16] = cpu->gpr[10] + 16;
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[5] = cpu->gpr[16];
    cpu->gpr[2] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[2];
    cpu->write_half(cpu->gpr[9] + 14, (uint16_t)cpu->gpr[3]);
    cpu->write_half(cpu->gpr[9] + 22, (uint16_t)cpu->gpr[8]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[18]);
    cpu->gpr[31] = 0x80158D00u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158D00:
    cpu->gpr[9] = cpu->gpr[2];
    int _bc_80158D04 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[16];
    if (_bc_80158D04) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158D0Cu);
        goto block_80158D0C;  /* not taken */
    }

block_80158D0C:
    cpu->gpr[6] = cpu->gpr[19];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[9] + 14);
    cpu->gpr[8] = cpu->read_half(cpu->gpr[9] + 22);
    cpu->gpr[2] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[2];
    cpu->write_half(cpu->gpr[9] + 14, (uint16_t)cpu->gpr[3]);
    cpu->write_half(cpu->gpr[9] + 22, (uint16_t)cpu->gpr[8]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[31] = 0x80158D48u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[21]);
    psx_check_interrupts_at(cpu, 0x80158698u);
    cpu->pc = 0x80158698u; return;  /* CPS jal -> 0x80158698 */

block_80158D48:
    cpu->gpr[9] = cpu->gpr[2];
    int _bc_80158D4C = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80158D50: 0x00000000 */
    if (_bc_80158D4C) {
        psx_check_interrupts_at(cpu, 0x80158D78u);
        goto block_80158D78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80158D54u);
        goto block_80158D54;  /* not taken */
    }

block_80158D54:
    cpu->gpr[10] = cpu->read_word(cpu->gpr[29] + 44);
    /* nop */  /* 0x80158D58: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[10] >> 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[9] + 14);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[9] + 22);
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    cpu->write_half(cpu->gpr[9] + 14, (uint16_t)cpu->gpr[3]);
    cpu->write_half(cpu->gpr[9] + 22, (uint16_t)cpu->gpr[4]);
    /* fall through to block_80158D78 */

block_80158D78:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 84);
    cpu->gpr[30] = cpu->read_word(cpu->gpr[29] + 80);
    cpu->gpr[23] = cpu->read_word(cpu->gpr[29] + 76);
    cpu->gpr[22] = cpu->read_word(cpu->gpr[29] + 72);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 68);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 64);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 60);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 56);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 52);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 48);
    uint32_t _jt_80158DA0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 88;
    psx_check_interrupts_at(cpu, _jt_80158DA0);
    cpu->pc = _jt_80158DA0; return;  /* CPS: jr $ra */
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

/* cps_80161630 — clean CPS transliteration of br2eu_func_80161630 (guest 0x80161630)
 * from SLES_017.22_full_24.c. */
void cps_80161630(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801616CCu: goto block_801616CC;
            case 0x801616E0u: goto block_801616E0;
            case 0x801616E8u: goto block_801616E8;
            case 0x801616FCu: goto block_801616FC;
            case 0x801617B8u: goto block_801617B8;
            case 0x801617CCu: goto block_801617CC;
            case 0x801617ECu: goto block_801617EC;
            case 0x8016183Cu: goto block_8016183C;
            case 0x80161860u: goto block_80161860;
            case 0x80161900u: goto block_80161900;
            case 0x801619A8u: goto block_801619A8;
            case 0x801619D4u: goto block_801619D4;
            case 0x80161A1Cu: goto block_80161A1C;
            case 0x80161AA8u: goto block_80161AA8;
            case 0x80161AE0u: goto block_80161AE0;
            case 0x80161B14u: goto block_80161B14;
            case 0x80161B44u: goto block_80161B44;
            case 0x80161B74u: goto block_80161B74;
            case 0x80161B88u: goto block_80161B88;
            case 0x80161C10u: goto block_80161C10;
            case 0x8016167Cu: goto block_8016167C;
            case 0x80161684u: goto block_80161684;
            case 0x8016168Cu: goto block_8016168C;
            case 0x80161694u: goto block_80161694;
            case 0x8016169Cu: goto block_8016169C;
            case 0x801616A4u: goto block_801616A4;
            case 0x801616ACu: goto block_801616AC;
            case 0x801616B4u: goto block_801616B4;
            case 0x801616C4u: goto block_801616C4;
            case 0x801616D4u: goto block_801616D4;
            case 0x801616F0u: goto block_801616F0;
            case 0x80161700u: goto block_80161700;
            case 0x80161708u: goto block_80161708;
            case 0x80161710u: goto block_80161710;
            case 0x8016172Cu: goto block_8016172C;
            case 0x8016173Cu: goto block_8016173C;
            case 0x8016174Cu: goto block_8016174C;
            case 0x80161750u: goto block_80161750;
            case 0x80161764u: goto block_80161764;
            case 0x80161774u: goto block_80161774;
            case 0x80161794u: goto block_80161794;
            case 0x801617E0u: goto block_801617E0;
            case 0x801617FCu: goto block_801617FC;
            case 0x80161828u: goto block_80161828;
            case 0x8016187Cu: goto block_8016187C;
            case 0x8016188Cu: goto block_8016188C;
            case 0x8016189Cu: goto block_8016189C;
            case 0x801618ACu: goto block_801618AC;
            case 0x801618BCu: goto block_801618BC;
            case 0x801618C0u: goto block_801618C0;
            case 0x801618D8u: goto block_801618D8;
            case 0x801618E0u: goto block_801618E0;
            case 0x80161918u: goto block_80161918;
            case 0x80161924u: goto block_80161924;
            case 0x80161938u: goto block_80161938;
            case 0x80161948u: goto block_80161948;
            case 0x8016194Cu: goto block_8016194C;
            case 0x80161974u: goto block_80161974;
            case 0x8016197Cu: goto block_8016197C;
            case 0x80161A30u: goto block_80161A30;
            case 0x80161A74u: goto block_80161A74;
            case 0x80161B54u: goto block_80161B54;
            case 0x80161B5Cu: goto block_80161B5C;
            case 0x80161B64u: goto block_80161B64;
            case 0x80161B7Cu: goto block_80161B7C;
            case 0x80161B90u: goto block_80161B90;
            case 0x80161BACu: goto block_80161BAC;
            case 0x80161BB0u: goto block_80161BB0;
            case 0x80161BECu: goto block_80161BEC;
            case 0x80161BFCu: goto block_80161BFC;
            case 0x80161C00u: goto block_80161C00;
            case 0x80161C08u: goto block_80161C08;
            case 0x80161C20u: goto block_80161C20;
            default: break;
        }
    }
    /* Address: 0x80161630, Size: 1572 bytes, Blocks: 74 */

block_80161630:
    cpu->gpr[29] = cpu->gpr[29] + -160;
    cpu->write_word(cpu->gpr[29] + 148, cpu->gpr[23]);
    cpu->gpr[23] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->write_word(cpu->gpr[29] + 156, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 152, cpu->gpr[30]);
    cpu->write_word(cpu->gpr[29] + 144, cpu->gpr[22]);
    cpu->write_word(cpu->gpr[29] + 140, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 136, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 132, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 128, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 124, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[29] + 120, cpu->gpr[16]);
    cpu->write_word(cpu->gpr[29] + 160, cpu->gpr[4]);
    cpu->write_word(cpu->gpr[29] + 80, cpu->gpr[0]);
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[0]);
    cpu->gpr[1] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->write_half(cpu->gpr[1] + 140, (uint16_t)cpu->gpr[0]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[2] = 1;
    /* fall through to block_8016167C */

block_8016167C:
    int _bc_8016167C = (cpu->gpr[8] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[8] < 2) ? 1 : 0;  /* 0x80161680: 0x29020002 */
    if (_bc_8016167C) {
        psx_check_interrupts_at(cpu, 0x80161794u);
        goto block_80161794;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161684u);
        goto block_80161684;  /* not taken */
    }

block_80161684:
    int _bc_80161684 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 240;
    if (_bc_80161684) {
        psx_check_interrupts_at(cpu, 0x8016169Cu);
        goto block_8016169C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016168Cu);
        goto block_8016168C;  /* not taken */
    }

block_8016168C:
    int _bc_8016168C = (cpu->gpr[8] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80161690: 0x00000000 */
    if (_bc_8016168C) {
        psx_check_interrupts_at(cpu, 0x801616B4u);
        goto block_801616B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161694u);
        goto block_80161694;  /* not taken */
    }

block_80161694:
    /* delay slot (always executes) */
    /* nop */  /* 0x80161698: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80161C08u);
    goto block_80161C08;  /* j */

block_8016169C:
    int _bc_8016169C = (cpu->gpr[8] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 241;
    if (_bc_8016169C) {
        psx_check_interrupts_at(cpu, 0x80161B90u);
        goto block_80161B90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801616A4u);
        goto block_801616A4;  /* not taken */
    }

block_801616A4:
    int _bc_801616A4 = (cpu->gpr[8] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = 255;
    if (_bc_801616A4) {
        psx_check_interrupts_at(cpu, 0x80161C00u);
        goto block_80161C00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801616ACu);
        goto block_801616AC;  /* not taken */
    }

block_801616AC:
    /* delay slot (always executes) */
    /* nop */  /* 0x801616B0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80161C08u);
    goto block_80161C08;  /* j */

block_801616B4:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 160);
    /* nop */  /* 0x801616B8: 0x00000000 */
    int _bc_801616BC = (cpu->gpr[8] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[21] = cpu->gpr[0];
    if (_bc_801616BC) {
        psx_check_interrupts_at(cpu, 0x80161700u);
        goto block_80161700;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801616C4u);
        goto block_801616C4;  /* not taken */
    }

block_801616C4:
    cpu->gpr[31] = 0x801616CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801616C8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8012AAA8u);
    cpu->pc = 0x8012AAA8u; return;  /* CPS jal -> 0x8012AAA8 */

block_801616CC:
    int _bc_801616CC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 2;
    if (_bc_801616CC) {
        psx_check_interrupts_at(cpu, 0x80161700u);
        goto block_80161700;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801616D4u);
        goto block_801616D4;  /* not taken */
    }

block_801616D4:
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[31] = 0x801616E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x8012CBC0u);
    cpu->pc = 0x8012CBC0u; return;  /* CPS jal -> 0x8012CBC0 */

block_801616E0:
    cpu->gpr[31] = 0x801616E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x8012C0B4u);
    cpu->pc = 0x8012C0B4u; return;  /* CPS jal -> 0x8012C0B4 */

block_801616E8:
    int _bc_801616E8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 515;
    if (_bc_801616E8) {
        psx_check_interrupts_at(cpu, 0x801616FCu);
        goto block_801616FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801616F0u);
        goto block_801616F0;  /* not taken */
    }

block_801616F0:
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[31] = 0x801616FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_801616FC:
    cpu->gpr[21] = cpu->gpr[0];
    /* fall through to block_80161700 */

block_80161700:
    cpu->write_half(cpu->gpr[29] + 26, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[29] + 24, (uint16_t)cpu->gpr[0]);
    /* fall through to block_80161708 */

block_80161708:
    cpu->gpr[17] = cpu->gpr[0];
    cpu->gpr[4] = cpu->gpr[21] << 4;
    /* fall through to block_80161710 */

block_80161710:
    cpu->gpr[18] = cpu->gpr[0];
    cpu->gpr[8] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[8] = cpu->gpr[8] + 4576;
    cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[8];
    cpu->gpr[5] = cpu->read_half(cpu->gpr[2] + 1008);
    cpu->gpr[3] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[3] = cpu->gpr[3] + 16852;
    /* fall through to block_8016172C */

block_8016172C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3]);
    /* nop */  /* 0x80161730: 0x00000000 */
    int _bc_80161734 = (cpu->gpr[5] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[29] + cpu->gpr[4];
    if (_bc_80161734) {
        psx_check_interrupts_at(cpu, 0x80161750u);
        goto block_80161750;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016173Cu);
        goto block_8016173C;  /* not taken */
    }

block_8016173C:
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < 9) ? 1 : 0;  /* 0x80161740: 0x2A420009 */
    int _bc_80161744 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 2;
    if (_bc_80161744) {
        psx_check_interrupts_at(cpu, 0x8016172Cu);
        goto block_8016172C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016174Cu);
        goto block_8016174C;  /* not taken */
    }

block_8016174C:
    cpu->gpr[2] = cpu->gpr[29] + cpu->gpr[4];
    /* fall through to block_80161750 */

block_80161750:
    cpu->write_half(cpu->gpr[2] + 48, (uint16_t)cpu->gpr[18]);
    cpu->gpr[17] = cpu->gpr[17] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[17] < 8) ? 1 : 0;  /* 0x80161758: 0x2A220008 */
    int _bc_8016175C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 2;
    if (_bc_8016175C) {
        psx_check_interrupts_at(cpu, 0x80161710u);
        goto block_80161710;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161764u);
        goto block_80161764;  /* not taken */
    }

block_80161764:
    cpu->gpr[21] = cpu->gpr[21] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[21] < 2) ? 1 : 0;  /* 0x80161768: 0x2AA20002 */
    int _bc_8016176C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_8016176C) {
        psx_check_interrupts_at(cpu, 0x80161708u);
        goto block_80161708;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161774u);
        goto block_80161774;  /* not taken */
    }

block_80161774:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 88);
    /* nop */  /* 0x80161778: 0x00000000 */
    cpu->gpr[8] = cpu->gpr[8] + 1;
    cpu->gpr[3] = 0x801F << 16;  /* 0x801F0000 */
    cpu->write_word(cpu->gpr[2] + -31480, cpu->gpr[0]);
    cpu->gpr[2] = 48;
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[8]);
    cpu->write_word(cpu->gpr[3] + -31464, cpu->gpr[2]);
    /* fall through to block_80161794 */

block_80161794:
    cpu->gpr[6] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[6] = cpu->gpr[6] + 4972;
    cpu->gpr[7] = 48;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[2] = cpu->gpr[7];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->gpr[31] = 0x801617B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_801617B8:
    cpu->gpr[4] = 5;
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[6] = 4;
    cpu->gpr[31] = 0x801617CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 7;
    psx_check_interrupts_at(cpu, 0x8012BF0Cu);
    cpu->pc = 0x8012BF0Cu; return;  /* CPS jal -> 0x8012BF0C */

block_801617CC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[23] + 124);
    /* nop */  /* 0x801617D0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0800;  /* 0x801617D4: 0x30420800 */
    int _bc_801617D8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1024;
    if (_bc_801617D8) {
        psx_check_interrupts_at(cpu, 0x801617FCu);
        goto block_801617FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801617E0u);
        goto block_801617E0;  /* not taken */
    }

block_801617E0:
    cpu->gpr[5] = 12;
    cpu->gpr[31] = 0x801617ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_801617EC:
    cpu->gpr[8] = 240;
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[8]);
    cpu->gpr[8] = 1;
    cpu->write_word(cpu->gpr[29] + 80, cpu->gpr[8]);
    /* fall through to block_801617FC */

block_801617FC:
    cpu->gpr[21] = cpu->gpr[0];
    cpu->gpr[8] = cpu->gpr[29] + 32;
    cpu->gpr[30] = cpu->gpr[21];
    cpu->write_word(cpu->gpr[29] + 92, cpu->gpr[8]);
    cpu->gpr[8] = 0x0030 << 16;  /* 0x00300000 */
    cpu->write_word(cpu->gpr[29] + 100, cpu->gpr[8]);
    cpu->gpr[8] = 0x0060 << 16;  /* 0x00600000 */
    cpu->write_word(cpu->gpr[29] + 104, cpu->gpr[8]);
    cpu->gpr[8] = 32;
    cpu->write_word(cpu->gpr[29] + 108, cpu->gpr[23]);
    cpu->write_word(cpu->gpr[29] + 112, cpu->gpr[8]);
    /* fall through to block_80161828 */

block_80161828:
    cpu->gpr[5] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[5] = cpu->gpr[5] + 7452;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[29] + 92);
    cpu->gpr[31] = 0x8016183Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[21] + 1;
    psx_check_interrupts_at(cpu, 0x8018EF60u);
    cpu->pc = 0x8018EF60u; return;  /* CPS jal -> 0x8018EF60 */

block_8016183C:
    cpu->gpr[6] = cpu->read_word(cpu->gpr[29] + 92);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[19] = 96;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[0]);
    cpu->gpr[31] = 0x80161860u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = (int32_t)cpu->gpr[8] >> 16;
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_80161860:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 108);
    /* nop */  /* 0x80161864: 0x00000000 */
    cpu->gpr[4] = cpu->read_half(cpu->gpr[8] + 108);
    cpu->gpr[2] = 12;
    cpu->gpr[3] = cpu->gpr[4] & 0x000C;  /* 0x80161870: 0x3083000C */
    int _bc_80161874 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[4];
    if (_bc_80161874) {
        psx_check_interrupts_at(cpu, 0x80161918u);
        goto block_80161918;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016187Cu);
        goto block_8016187C;  /* not taken */
    }

block_8016187C:
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[4] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[4] = cpu->gpr[4] + 16248;
    cpu->gpr[7] = cpu->gpr[4] + 16;
    /* fall through to block_8016188C */

block_8016188C:
    cpu->gpr[18] = cpu->gpr[0];
    cpu->gpr[5] = cpu->read_half(cpu->gpr[4]);
    cpu->gpr[3] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[3] = cpu->gpr[3] + 16852;
    /* fall through to block_8016189C */

block_8016189C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3]);
    /* nop */  /* 0x801618A0: 0x00000000 */
    int _bc_801618A4 = (cpu->gpr[5] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + cpu->gpr[30];
    if (_bc_801618A4) {
        psx_check_interrupts_at(cpu, 0x801618C0u);
        goto block_801618C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801618ACu);
        goto block_801618AC;  /* not taken */
    }

block_801618AC:
    cpu->gpr[18] = cpu->gpr[18] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[18] < 9) ? 1 : 0;  /* 0x801618B0: 0x2A420009 */
    int _bc_801618B4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + 2;
    if (_bc_801618B4) {
        psx_check_interrupts_at(cpu, 0x8016189Cu);
        goto block_8016189C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801618BCu);
        goto block_801618BC;  /* not taken */
    }

block_801618BC:
    cpu->gpr[2] = cpu->gpr[6] + cpu->gpr[30];
    /* fall through to block_801618C0 */

block_801618C0:
    cpu->gpr[2] = cpu->gpr[29] + cpu->gpr[2];
    cpu->write_half(cpu->gpr[2] + 48, (uint16_t)cpu->gpr[18]);
    cpu->gpr[4] = cpu->gpr[4] + 2;
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < (int32_t)cpu->gpr[7]) ? 1 : 0;  /* 0x801618CC: 0x0087102A */
    int _bc_801618D0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[6] + 2;
    if (_bc_801618D0) {
        psx_check_interrupts_at(cpu, 0x8016188Cu);
        goto block_8016188C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801618D8u);
        goto block_801618D8;  /* not taken */
    }

block_801618D8:
    /* delay slot (always executes) */
    cpu->gpr[18] = -1;
    psx_check_interrupts_at(cpu, 0x8016194Cu);
    goto block_8016194C;  /* j */

block_801618E0:
    cpu->gpr[5] = 9;
    cpu->gpr[6] = cpu->gpr[21];
    cpu->gpr[2] = cpu->gpr[7] + cpu->gpr[30];
    cpu->gpr[16] = cpu->gpr[29] + 48;
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[2];
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16]);
    cpu->gpr[31] = 0x80161900u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 1;
    psx_check_interrupts_at(cpu, 0x80129C10u);
    cpu->pc = 0x80129C10u; return;  /* CPS jal -> 0x80129C10 */

block_80161900:
    cpu->write_half(cpu->gpr[16], (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = 1;
    cpu->gpr[8] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[8] = cpu->gpr[8] + 4576;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[8] + 1040, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80161948u);
    goto block_80161948;  /* j */

block_80161918:
    cpu->gpr[17] = cpu->gpr[0];
    cpu->gpr[2] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[4] = cpu->gpr[2] + 16796;
    /* fall through to block_80161924 */

block_80161924:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4]);
    /* nop */  /* 0x80161928: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8016192C: 0x00431024 */
    int _bc_80161930 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[17] << 1;
    if (_bc_80161930) {
        psx_check_interrupts_at(cpu, 0x801618E0u);
        goto block_801618E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161938u);
        goto block_80161938;  /* not taken */
    }

block_80161938:
    cpu->gpr[17] = cpu->gpr[17] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[17] < 8) ? 1 : 0;  /* 0x8016193C: 0x2A220008 */
    int _bc_80161940 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 2;
    if (_bc_80161940) {
        psx_check_interrupts_at(cpu, 0x80161924u);
        goto block_80161924;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161948u);
        goto block_80161948;  /* not taken */
    }

block_80161948:
    cpu->gpr[18] = cpu->gpr[17];
    /* fall through to block_8016194C */

block_8016194C:
    cpu->gpr[19] = cpu->gpr[19] + 32;
    cpu->gpr[17] = cpu->gpr[0];
    cpu->gpr[2] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 112);
    cpu->gpr[22] = cpu->gpr[2] + 16764;
    cpu->gpr[8] = cpu->gpr[8] + 80;
    cpu->write_word(cpu->gpr[29] + 84, cpu->gpr[8]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->gpr[20] = cpu->gpr[19] << 16;
    cpu->write_word(cpu->gpr[29] + 96, cpu->gpr[8]);
    /* fall through to block_80161974 */

block_80161974:
    int _bc_80161974 = (cpu->gpr[17] != cpu->gpr[18]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[19] = cpu->gpr[0];
    if (_bc_80161974) {
        psx_check_interrupts_at(cpu, 0x801619A8u);
        goto block_801619A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8016197Cu);
        goto block_8016197C;  /* not taken */
    }

block_8016197C:
    cpu->gpr[19] = 3;
    cpu->gpr[6] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[6] = cpu->gpr[6] + 7464;
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 96);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[2] = (int32_t)cpu->gpr[20] >> 16;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[19]);
    cpu->gpr[31] = 0x801619A8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = (int32_t)cpu->gpr[8] >> 16;
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_801619A8:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 100);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[16] = (int32_t)cpu->gpr[20] >> 16;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[7] = (int32_t)cpu->gpr[8] >> 16;
    cpu->gpr[8] = 8;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[8]);
    cpu->gpr[6] = cpu->read_word(cpu->gpr[22]);
    cpu->gpr[31] = 0x801619D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[22] = cpu->gpr[22] + 4;
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_801619D4:
    cpu->gpr[2] = cpu->gpr[17] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[30];
    cpu->gpr[2] = cpu->gpr[29] + cpu->gpr[2];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 84);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2] + 48);
    cpu->gpr[17] = cpu->gpr[17] + 1;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[19]);
    cpu->gpr[7] = cpu->gpr[8] << 16;
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[8] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[8] = cpu->gpr[8] + 16812;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[8];
    cpu->gpr[6] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[31] = 0x80161A1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = (int32_t)cpu->gpr[7] >> 16;
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_80161A1C:
    cpu->gpr[2] = 0x0018 << 16;  /* 0x00180000 */
    cpu->gpr[20] = cpu->gpr[20] + cpu->gpr[2];
    cpu->gpr[2] = ((int32_t)cpu->gpr[17] < 8) ? 1 : 0;  /* 0x80161A24: 0x2A220008 */
    int _bc_80161A28 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x0100 << 16;  /* 0x01000000 */
    if (_bc_80161A28) {
        psx_check_interrupts_at(cpu, 0x80161974u);
        goto block_80161974;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161A30u);
        goto block_80161A30;  /* not taken */
    }

block_80161A30:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 100);
    /* nop */  /* 0x80161A34: 0x00000000 */
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[2];
    cpu->write_word(cpu->gpr[29] + 100, cpu->gpr[8]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 104);
    cpu->gpr[30] = cpu->gpr[30] + 16;
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[2];
    cpu->write_word(cpu->gpr[29] + 104, cpu->gpr[8]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 108);
    cpu->gpr[21] = cpu->gpr[21] + 1;
    cpu->gpr[8] = cpu->gpr[8] + 2;
    cpu->write_word(cpu->gpr[29] + 108, cpu->gpr[8]);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 112);
    cpu->gpr[2] = ((int32_t)cpu->gpr[21] < 2) ? 1 : 0;  /* 0x80161A64: 0x2AA20002 */
    cpu->gpr[8] = cpu->gpr[8] + 256;
    int _bc_80161A6C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 112, cpu->gpr[8]);
    if (_bc_80161A6C) {
        psx_check_interrupts_at(cpu, 0x80161828u);
        goto block_80161828;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161A74u);
        goto block_80161A74;  /* not taken */
    }

block_80161A74:
    cpu->gpr[6] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[6] = cpu->gpr[6] + 7480;
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[7] = 152;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[0]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[23] + 34);
    cpu->gpr[4] = cpu->gpr[4] + 4;
    cpu->gpr[2] = cpu->gpr[2] + -56;
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[31] = 0x80161AA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8012E7CCu);
    cpu->pc = 0x8012E7CCu; return;  /* CPS jal -> 0x8012E7CC */

block_80161AA8:
    cpu->gpr[6] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[6] = cpu->gpr[6] + 7496;
    cpu->gpr[7] = 220;
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[8] = 8;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[8]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[23] + 34);
    cpu->gpr[4] = cpu->gpr[4] + 4;
    cpu->gpr[2] = cpu->gpr[2] + -60;
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[31] = 0x80161AE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8012E58Cu);
    cpu->pc = 0x8012E58Cu; return;  /* CPS jal -> 0x8012E58C */

block_80161AE0:
    cpu->gpr[6] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[6] = cpu->gpr[6] + 4888;
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 12);
    cpu->gpr[7] = 288;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[0]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[23] + 34);
    cpu->gpr[4] = cpu->gpr[4] + 4;
    cpu->gpr[2] = cpu->gpr[2] + -56;
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[31] = 0x80161B14u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8012E7CCu);
    cpu->pc = 0x8012E7CCu; return;  /* CPS jal -> 0x8012E7CC */

block_80161B14:
    cpu->gpr[6] = 0x0F17 << 16;  /* 0x0F170000 */
    cpu->gpr[6] = cpu->gpr[6] | 0x001E;
    cpu->gpr[7] = 0x7A16 << 16;  /* 0x7A160000 */
    cpu->gpr[7] = cpu->gpr[7] | 0xD8E0;
    cpu->gpr[2] = 0xFFC4 << 16;  /* 0xFFC40000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[5] = cpu->read_half(cpu->gpr[23] + 34);
    cpu->gpr[4] = cpu->gpr[4] + 4;
    cpu->gpr[5] = cpu->gpr[5] << 16;
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];
    cpu->gpr[31] = 0x80161B44u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] | 0x0148;
    psx_check_interrupts_at(cpu, 0x8018C824u);
    cpu->pc = 0x8018C824u; return;  /* CPS jal -> 0x8018C824 */

block_80161B44:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 160);
    /* nop */  /* 0x80161B48: 0x00000000 */
    int _bc_80161B4C = (cpu->gpr[8] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_80161B4C) {
        psx_check_interrupts_at(cpu, 0x80161B64u);
        goto block_80161B64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161B54u);
        goto block_80161B54;  /* not taken */
    }

block_80161B54:
    int _bc_80161B54 = (cpu->gpr[8] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_80161B54) {
        psx_check_interrupts_at(cpu, 0x80161B7Cu);
        goto block_80161B7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161B5Cu);
        goto block_80161B5C;  /* not taken */
    }

block_80161B5C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80161B60: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80161C08u);
    goto block_80161C08;  /* j */

block_80161B64:
    cpu->gpr[4] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[31] = 0x80161B74u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 36;
    psx_check_interrupts_at(cpu, 0x8012C374u);
    cpu->pc = 0x8012C374u; return;  /* CPS jal -> 0x8012C374 */

block_80161B74:
    /* delay slot (always executes) */
    /* nop */  /* 0x80161B78: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80161C08u);
    goto block_80161C08;  /* j */

block_80161B7C:
    cpu->gpr[5] = cpu->read_word(cpu->gpr[23] + 8);
    cpu->gpr[31] = 0x80161B88u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 52;
    psx_check_interrupts_at(cpu, 0x8012C374u);
    cpu->pc = 0x8012C374u; return;  /* CPS jal -> 0x8012C374 */

block_80161B88:
    /* delay slot (always executes) */
    /* nop */  /* 0x80161B8C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80161C08u);
    goto block_80161C08;  /* j */

block_80161B90:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[23] + 192);
    cpu->gpr[21] = cpu->gpr[0];
    cpu->write_byte(cpu->gpr[2] + 23, (uint8_t)cpu->gpr[0]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[23] + 192);
    cpu->gpr[5] = cpu->gpr[29] + 48;
    cpu->write_byte(cpu->gpr[2] + 24, (uint8_t)cpu->gpr[0]);
    cpu->gpr[17] = cpu->gpr[0];
    /* fall through to block_80161BAC */

block_80161BAC:
    cpu->gpr[4] = cpu->gpr[21] << 4;
    /* fall through to block_80161BB0 */

block_80161BB0:
    cpu->gpr[8] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[8] = cpu->gpr[8] + 4576;
    cpu->gpr[3] = cpu->gpr[4] + cpu->gpr[8];
    cpu->gpr[2] = cpu->gpr[5] + cpu->gpr[4];
    cpu->gpr[8] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2]);
    cpu->gpr[8] = cpu->gpr[8] + 16852;
    cpu->gpr[2] = cpu->gpr[2] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[8];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2]);
    cpu->gpr[17] = cpu->gpr[17] + 1;
    cpu->write_half(cpu->gpr[3] + 1008, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = ((int32_t)cpu->gpr[17] < 8) ? 1 : 0;  /* 0x80161BE0: 0x2A220008 */
    int _bc_80161BE4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 2;
    if (_bc_80161BE4) {
        psx_check_interrupts_at(cpu, 0x80161BB0u);
        goto block_80161BB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161BECu);
        goto block_80161BEC;  /* not taken */
    }

block_80161BEC:
    cpu->gpr[21] = cpu->gpr[21] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[21] < 2) ? 1 : 0;  /* 0x80161BF0: 0x2AA20002 */
    int _bc_80161BF4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = cpu->gpr[0];
    if (_bc_80161BF4) {
        psx_check_interrupts_at(cpu, 0x80161BACu);
        goto block_80161BAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161BFCu);
        goto block_80161BFC;  /* not taken */
    }

block_80161BFC:
    cpu->gpr[8] = 255;
    /* fall through to block_80161C00 */

block_80161C00:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 88, cpu->gpr[8]);
    psx_check_interrupts_at(cpu, 0x80161C10u);
    goto block_80161C10;  /* j */

block_80161C08:
    cpu->gpr[31] = 0x80161C10u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 0x1F80 << 16;  /* 0x1F800000 */
    psx_check_interrupts_at(cpu, 0x8012AEECu);
    cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */

block_80161C10:
    cpu->gpr[8] = cpu->read_word(cpu->gpr[29] + 88);
    cpu->gpr[2] = 255;
    int _bc_80161C18 = (cpu->gpr[8] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_80161C18) {
        psx_check_interrupts_at(cpu, 0x8016167Cu);
        goto block_8016167C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161C20u);
        goto block_80161C20;  /* not taken */
    }

block_80161C20:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 80);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 156);
    cpu->gpr[30] = cpu->read_word(cpu->gpr[29] + 152);
    cpu->gpr[23] = cpu->read_word(cpu->gpr[29] + 148);
    cpu->gpr[22] = cpu->read_word(cpu->gpr[29] + 144);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 140);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 136);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 132);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 128);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 124);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 120);
    uint32_t _jt_80161C4C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 160;
    psx_check_interrupts_at(cpu, _jt_80161C4C);
    cpu->pc = _jt_80161C4C; return;  /* CPS: jr $ra */
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

/* cps_8018b100 — clean CPS transliteration of br2eu_func_8018B100 (guest 0x8018b100)
 * from SLES_017.22_full_41.c. */
void cps_8018b100(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018B15Cu: goto block_8018B15C;
            case 0x8018B198u: goto block_8018B198;
            case 0x8018B1C4u: goto block_8018B1C4;
            case 0x8018B108u: goto block_8018B108;
            case 0x8018B13Cu: goto block_8018B13C;
            case 0x8018B14Cu: goto block_8018B14C;
            case 0x8018B154u: goto block_8018B154;
            case 0x8018B1B0u: goto block_8018B1B0;
            case 0x8018B1CCu: goto block_8018B1CC;
            case 0x8018B1D8u: goto block_8018B1D8;
            case 0x8018B200u: goto block_8018B200;
            default: break;
        }
    }
    /* Address: 0x8018B100, Size: 264 bytes, Blocks: 12 */

block_8018B100:
    int _bc_8018B100 = ((int32_t)cpu->gpr[7] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_8018B100) {
        psx_check_interrupts_at(cpu, 0x8018B200u);
        goto block_8018B200;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018B108u);
        goto block_8018B108;  /* not taken */
    }

block_8018B108:
    cpu->write_word(cpu->gpr[8] + 1020, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[8] + 1016, cpu->gpr[16]);
    cpu->write_word(cpu->gpr[8] + 1012, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[8] + 1008, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[8] + 1004, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[8] + 1000, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[8] + 996, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[8] + 992, cpu->gpr[22]);
    cpu->gpr[16] = cpu->gpr[4];
    cpu->gpr[17] = cpu->gpr[5];
    cpu->gpr[19] = cpu->gpr[7];
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 16);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 20);
    /* fall through to block_8018B13C */

block_8018B13C:
    cpu->gpr[8] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 12);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[16]);
    int _bc_8018B144 = (cpu->gpr[8] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[21];
    if (_bc_8018B144) {
        psx_check_interrupts_at(cpu, 0x8018B1CCu);
        goto block_8018B1CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018B14Cu);
        goto block_8018B14C;  /* not taken */
    }

block_8018B14C:
    int _bc_8018B14C = (cpu->gpr[5] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018B150: 0x00000000 */
    if (_bc_8018B14C) {
        psx_check_interrupts_at(cpu, 0x8018B1CCu);
        goto block_8018B1CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018B154u);
        goto block_8018B154;  /* not taken */
    }

block_8018B154:
    cpu->gpr[31] = 0x8018B15Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + 32;
    psx_check_interrupts_at(cpu, 0x80186F5Cu);
    cpu->pc = 0x80186F5Cu; return;  /* CPS jal -> 0x80186F5C */

block_8018B15C:
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((cpu->gpr[16] + 4)));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((cpu->gpr[16] + 8))));
    cpu->gpr[11] = 0x1F80 << 16;  /* 0x1F800000 */
    gte_execute(cpu, 0x0480012);  /* gte cmd 0x12 */  /* 0x8018B168: 0x4A480012 */
    cpu->write_word(cpu->gpr[11] + 960, cpu->gpr[14]);
    cpu->write_word(cpu->gpr[11] + 964, cpu->gpr[12]);
    cpu->write_word(cpu->gpr[11] + 968, cpu->gpr[15]);
    cpu->write_word(cpu->gpr[11] + 972, cpu->gpr[24]);
    cpu->write_word(cpu->gpr[11] + 976, cpu->gpr[13]);
    cpu->gpr[5] = cpu->gpr[11] + 960;
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[11] + 980), (cpu->gte_data[25]));
    gte_precision_store_word((cpu->gpr[11] + 980), 25);
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[11] + 984), (cpu->gte_data[26]));
    gte_precision_store_word((cpu->gpr[11] + 984), 26);
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[11] + 988), (cpu->gte_data[27]));
    gte_precision_store_word((cpu->gpr[11] + 988), 27);
    cpu->gpr[31] = 0x8018B198u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[20];
    psx_check_interrupts_at(cpu, 0x80186F5Cu);
    cpu->pc = 0x80186F5Cu; return;  /* CPS jal -> 0x80186F5C */

block_8018B198:
    cpu->gpr[18] = cpu->read_word(cpu->gpr[16] + 28);
    cpu->gpr[5] = cpu->gpr[17];
    cpu->gpr[22] = cpu->read_word(cpu->gpr[18]);
    /* nop */  /* 0x8018B1A4: 0x00000000 */
    int _bc_8018B1A8 = (cpu->gpr[22] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[18] + 4;
    if (_bc_8018B1A8) {
        psx_check_interrupts_at(cpu, 0x8018B1CCu);
        goto block_8018B1CC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018B1B0u);
        goto block_8018B1B0;  /* not taken */
    }

block_8018B1B0:
    cpu->gpr[7] = cpu->read_word(cpu->gpr[18] + 4);
    cpu->gpr[8] = cpu->read_word(cpu->gpr[18] + 12);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[18] + 8);
    uint32_t _jt_8018B1BC = cpu->gpr[8];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x8018B1C4u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[22] = cpu->gpr[22] + -1;
    psx_check_interrupts_at(cpu, _jt_8018B1BC);
    cpu->pc = _jt_8018B1BC; return;  /* CPS jalr */

block_8018B1C4:
    int _bc_8018B1C4 = ((int32_t)cpu->gpr[22] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[18] + 16;
    if (_bc_8018B1C4) {
        psx_check_interrupts_at(cpu, 0x8018B1B0u);
        goto block_8018B1B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018B1CCu);
        goto block_8018B1CC;  /* not taken */
    }

block_8018B1CC:
    cpu->gpr[19] = cpu->gpr[19] + -1;
    int _bc_8018B1D0 = ((int32_t)cpu->gpr[19] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + 40;
    if (_bc_8018B1D0) {
        psx_check_interrupts_at(cpu, 0x8018B13Cu);
        goto block_8018B13C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018B1D8u);
        goto block_8018B1D8;  /* not taken */
    }

block_8018B1D8:
    cpu->gpr[4] = cpu->gpr[16];
    cpu->gpr[8] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[31] = cpu->read_word(cpu->gpr[8] + 1020);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[8] + 1016);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[8] + 1012);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[8] + 1008);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[8] + 1004);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[8] + 1000);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[8] + 996);
    cpu->gpr[22] = cpu->read_word(cpu->gpr[8] + 992);
    /* fall through to block_8018B200 */

block_8018B200:
    uint32_t _jt_8018B200 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4];
    psx_check_interrupts_at(cpu, _jt_8018B200);
    cpu->pc = _jt_8018B200; return;  /* CPS: jr $ra */
}

/* cps_8018cfdc — clean CPS transliteration of br2eu_func_8018CFDC (guest 0x8018cfdc)
 * from SLES_017.22_full_41.c. */
void cps_8018cfdc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018D020u: goto block_8018D020;
            case 0x8018D028u: goto block_8018D028;
            case 0x8018D040u: goto block_8018D040;
            case 0x8018D0CCu: goto block_8018D0CC;
            case 0x8018D0F4u: goto block_8018D0F4;
            case 0x8018D118u: goto block_8018D118;
            case 0x8018D018u: goto block_8018D018;
            case 0x8018D054u: goto block_8018D054;
            case 0x8018D064u: goto block_8018D064;
            case 0x8018D068u: goto block_8018D068;
            case 0x8018D074u: goto block_8018D074;
            case 0x8018D084u: goto block_8018D084;
            case 0x8018D08Cu: goto block_8018D08C;
            case 0x8018D09Cu: goto block_8018D09C;
            case 0x8018D0A4u: goto block_8018D0A4;
            case 0x8018D0BCu: goto block_8018D0BC;
            case 0x8018D0D8u: goto block_8018D0D8;
            case 0x8018D0E4u: goto block_8018D0E4;
            case 0x8018D0ECu: goto block_8018D0EC;
            case 0x8018D104u: goto block_8018D104;
            case 0x8018D10Cu: goto block_8018D10C;
            case 0x8018D120u: goto block_8018D120;
            case 0x8018D12Cu: goto block_8018D12C;
            case 0x8018D130u: goto block_8018D130;
            default: break;
        }
    }
    /* Address: 0x8018CFDC, Size: 372 bytes, Blocks: 25 */

block_8018CFDC:
    cpu->gpr[29] = cpu->gpr[29] + -48;
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[20]);
    cpu->gpr[3] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[3] = cpu->gpr[3] + 23444;
    cpu->gpr[2] = cpu->gpr[4] << 3;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[16]);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[2] + 4);
    int _bc_8018D010 = ((int32_t)cpu->gpr[4] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[20] = cpu->gpr[5];
    if (_bc_8018D010) {
        psx_check_interrupts_at(cpu, 0x8018D074u);
        goto block_8018D074;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D018u);
        goto block_8018D018;  /* not taken */
    }

block_8018D018:
    cpu->gpr[31] = 0x8018D020u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D01C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80157368u);
    cpu->pc = 0x80157368u; return;  /* CPS jal -> 0x80157368 */

block_8018D020:
    cpu->gpr[31] = 0x8018D028u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8018D024: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198AF0u);
    cpu->pc = 0x80198AF0u; return;  /* CPS jal -> 0x80198AF0 */

block_8018D028:
    cpu->gpr[4] = 14;
    cpu->gpr[5] = cpu->gpr[29] + 16;
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[2] = 128;
    cpu->gpr[31] = 0x8018D040u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[29] + 16, (uint8_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80195D8Cu);
    cpu->pc = 0x80195D8Cu; return;  /* CPS jal -> 0x80195D8C */

block_8018D040:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + 17792);
    /* nop */  /* 0x8018D048: 0x00000000 */
    int _bc_8018D04C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_8018D04C) {
        psx_check_interrupts_at(cpu, 0x8018D064u);
        goto block_8018D064;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D054u);
        goto block_8018D054;  /* not taken */
    }

block_8018D054:
    cpu->gpr[3] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[2] = 3;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[3] + 26840, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8018D068u);
    goto block_8018D068;  /* j */

block_8018D064:
    cpu->write_word(cpu->gpr[2] + 26840, cpu->gpr[0]);
    /* fall through to block_8018D068 */

block_8018D068:
    cpu->gpr[2] = 0x801B << 16;  /* 0x801B0000 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[2] + 21996, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x8018D12Cu);
    goto block_8018D12C;  /* j */

block_8018D074:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[17]);
    /* nop */  /* 0x8018D078: 0x00000000 */
    int _bc_8018D07C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_8018D07C) {
        psx_check_interrupts_at(cpu, 0x8018D08Cu);
        goto block_8018D08C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D084u);
        goto block_8018D084;  /* not taken */
    }

block_8018D084:
    /* delay slot (always executes) */
    cpu->gpr[2] = -1;
    psx_check_interrupts_at(cpu, 0x8018D130u);
    goto block_8018D130;  /* j */

block_8018D08C:
    cpu->gpr[16] = cpu->read_word(cpu->gpr[3] + 26840);
    /* nop */  /* 0x8018D090: 0x00000000 */
    int _bc_8018D094 = (cpu->gpr[16] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[16] + -1;
    if (_bc_8018D094) {
        psx_check_interrupts_at(cpu, 0x8018D0A4u);
        goto block_8018D0A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D09Cu);
        goto block_8018D09C;  /* not taken */
    }

block_8018D09C:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[3] + 26840, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8018D12Cu);
    goto block_8018D12C;  /* j */

block_8018D0A4:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[18] = 0x801B << 16;  /* 0x801B0000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[18] + 21996);
    cpu->gpr[2] = 1;
    int _bc_8018D0B4 = (cpu->gpr[4] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3] + 17792, (uint8_t)cpu->gpr[2]);
    if (_bc_8018D0B4) {
        psx_check_interrupts_at(cpu, 0x8018D0D8u);
        goto block_8018D0D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D0BCu);
        goto block_8018D0BC;  /* not taken */
    }

block_8018D0BC:
    cpu->gpr[4] = cpu->gpr[17];
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[31] = 0x8018D0CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[19];
    psx_check_interrupts_at(cpu, 0x8018D150u);
    cpu->pc = 0x8018D150u; return;  /* CPS jal -> 0x8018D150 */

block_8018D0CC:
    cpu->gpr[2] = 1;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[18] + 21996, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8018D12Cu);
    goto block_8018D12C;  /* j */

block_8018D0D8:
    cpu->gpr[2] = 2;
    int _bc_8018D0DC = (cpu->gpr[4] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1;
    if (_bc_8018D0DC) {
        psx_check_interrupts_at(cpu, 0x8018D0ECu);
        goto block_8018D0EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D0E4u);
        goto block_8018D0E4;  /* not taken */
    }

block_8018D0E4:
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    psx_check_interrupts_at(cpu, 0x8018D130u);
    goto block_8018D130;  /* j */

block_8018D0EC:
    cpu->gpr[31] = 0x8018D0F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80198E2Cu);
    cpu->pc = 0x80198E2Cu; return;  /* CPS jal -> 0x80198E2C */

block_8018D0F4:
    cpu->gpr[3] = cpu->gpr[2];
    cpu->gpr[2] = ((int32_t)cpu->gpr[16] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x8018D0F8: 0x0203102A */
    int _bc_8018D0FC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_8018D0FC) {
        psx_check_interrupts_at(cpu, 0x8018D130u);
        goto block_8018D130;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D104u);
        goto block_8018D104;  /* not taken */
    }

block_8018D104:
    int _bc_8018D104 = ((int32_t)cpu->gpr[3] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[17];
    if (_bc_8018D104) {
        psx_check_interrupts_at(cpu, 0x8018D120u);
        goto block_8018D120;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D10Cu);
        goto block_8018D10C;  /* not taken */
    }

block_8018D10C:
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[31] = 0x8018D118u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[19];
    psx_check_interrupts_at(cpu, 0x8018D150u);
    cpu->pc = 0x8018D150u; return;  /* CPS jal -> 0x8018D150 */

block_8018D118:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x8018D130u);
    goto block_8018D130;  /* j */

block_8018D120:
    cpu->write_word(cpu->gpr[18] + 21996, cpu->gpr[0]);
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    psx_check_interrupts_at(cpu, 0x8018D130u);
    goto block_8018D130;  /* j */

block_8018D12C:
    cpu->gpr[2] = cpu->gpr[0];
    /* fall through to block_8018D130 */

block_8018D130:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 24);
    uint32_t _jt_8018D148 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 48;
    psx_check_interrupts_at(cpu, _jt_8018D148);
    cpu->pc = _jt_8018D148; return;  /* CPS: jr $ra */
}

/* cps_8018d150 — clean CPS transliteration of br2eu_func_8018D150 (guest 0x8018d150)
 * from SLES_017.22_full_41.c. */
void cps_8018d150(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8018D180u: goto block_8018D180;
            case 0x8018D198u: goto block_8018D198;
            case 0x8018D174u: goto block_8018D174;
            case 0x8018D188u: goto block_8018D188;
            case 0x8018D18Cu: goto block_8018D18C;
            case 0x8018D1A0u: goto block_8018D1A0;
            default: break;
        }
    }
    /* Address: 0x8018D150, Size: 108 bytes, Blocks: 7 */

block_8018D150:
    cpu->gpr[29] = cpu->gpr[29] + -32;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[31]);
    cpu->gpr[4] = 2;
    /* fall through to block_8018D174 */

block_8018D174:
    cpu->gpr[5] = cpu->gpr[17];
    cpu->gpr[31] = 0x8018D180u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80195D8Cu);
    cpu->pc = 0x80195D8Cu; return;  /* CPS jal -> 0x80195D8C */

block_8018D180:
    int _bc_8018D180 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 2;
    if (_bc_8018D180) {
        psx_check_interrupts_at(cpu, 0x8018D174u);
        goto block_8018D174;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D188u);
        goto block_8018D188;  /* not taken */
    }

block_8018D188:
    cpu->gpr[4] = cpu->gpr[16];
    /* fall through to block_8018D18C */

block_8018D18C:
    cpu->gpr[5] = cpu->gpr[18];
    cpu->gpr[31] = 0x8018D198u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 128;
    psx_check_interrupts_at(cpu, 0x80198C90u);
    cpu->pc = 0x80198C90u; return;  /* CPS jal -> 0x80198C90 */

block_8018D198:
    int _bc_8018D198 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[16];
    if (_bc_8018D198) {
        psx_check_interrupts_at(cpu, 0x8018D18Cu);
        goto block_8018D18C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8018D1A0u);
        goto block_8018D1A0;  /* not taken */
    }

block_8018D1A0:
    cpu->gpr[2] = cpu->gpr[16];
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_8018D1B4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 32;
    psx_check_interrupts_at(cpu, _jt_8018D1B4);
    cpu->pc = _jt_8018D1B4; return;  /* CPS: jr $ra */
}

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

/* cps_80196e14 — clean CPS transliteration of br2eu_func_80196E14 (guest 0x80196e14)
 * from SLES_017.22_full_45.c. */
void cps_80196e14(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80196E78u: goto block_80196E78;
            case 0x80196ECCu: goto block_80196ECC;
            case 0x80196EDCu: goto block_80196EDC;
            case 0x80196FD8u: goto block_80196FD8;
            case 0x80197030u: goto block_80197030;
            case 0x80197080u: goto block_80197080;
            case 0x801970CCu: goto block_801970CC;
            case 0x801970D4u: goto block_801970D4;
            case 0x801970F0u: goto block_801970F0;
            case 0x80197118u: goto block_80197118;
            case 0x80197150u: goto block_80197150;
            case 0x80197180u: goto block_80197180;
            case 0x80196E58u: goto block_80196E58;
            case 0x80196E98u: goto block_80196E98;
            case 0x80196EA0u: goto block_80196EA0;
            case 0x80196EB4u: goto block_80196EB4;
            case 0x80196ED4u: goto block_80196ED4;
            case 0x80196EECu: goto block_80196EEC;
            case 0x80196EF4u: goto block_80196EF4;
            case 0x80196F14u: goto block_80196F14;
            case 0x80196F1Cu: goto block_80196F1C;
            case 0x80196F24u: goto block_80196F24;
            case 0x80196F30u: goto block_80196F30;
            case 0x80196F58u: goto block_80196F58;
            case 0x80196F5Cu: goto block_80196F5C;
            case 0x80196F84u: goto block_80196F84;
            case 0x80196F8Cu: goto block_80196F8C;
            case 0x80196FB4u: goto block_80196FB4;
            case 0x80196FD0u: goto block_80196FD0;
            case 0x80197010u: goto block_80197010;
            case 0x80197028u: goto block_80197028;
            case 0x80197048u: goto block_80197048;
            case 0x80197074u: goto block_80197074;
            case 0x801970DCu: goto block_801970DC;
            case 0x801970E0u: goto block_801970E0;
            case 0x801970E8u: goto block_801970E8;
            case 0x801970F8u: goto block_801970F8;
            case 0x80197110u: goto block_80197110;
            case 0x80197124u: goto block_80197124;
            case 0x8019712Cu: goto block_8019712C;
            case 0x80197140u: goto block_80197140;
            case 0x80197154u: goto block_80197154;
            case 0x8019715Cu: goto block_8019715C;
            case 0x80197170u: goto block_80197170;
            case 0x80197188u: goto block_80197188;
            case 0x80197198u: goto block_80197198;
            case 0x801971A8u: goto block_801971A8;
            case 0x801971B8u: goto block_801971B8;
            case 0x801971BCu: goto block_801971BC;
            case 0x801971D4u: goto block_801971D4;
            case 0x801971F0u: goto block_801971F0;
            case 0x801971F8u: goto block_801971F8;
            default: break;
        }
    }
    /* Address: 0x80196E14, Size: 1036 bytes, Blocks: 53 */

block_80196E14:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14256);
    cpu->gpr[29] = cpu->gpr[29] + -56;
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[22]);
    cpu->gpr[22] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[7];
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 52, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[20]);
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 2) ? 1 : 0;  /* 0x80196E4C: 0x28420002 */
    int _bc_80196E50 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[19]);
    if (_bc_80196E50) {
        psx_check_interrupts_at(cpu, 0x80196E78u);
        goto block_80196E78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196E58u);
        goto block_80196E58;  /* not taken */
    }

block_80196E58:
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x80196E58: 0x322200FF */
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[5] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14228);
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x80196E78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22592;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_80196E78:
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x80196E78: 0x322200FF */
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13684);
    /* nop */  /* 0x80196E8C: 0x00000000 */
    int _bc_80196E90 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    if (_bc_80196E90) {
        psx_check_interrupts_at(cpu, 0x80196ED4u);
        goto block_80196ED4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196E98u);
        goto block_80196E98;  /* not taken */
    }

block_80196E98:
    int _bc_80196E98 = (cpu->gpr[16] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80196E9C: 0x00000000 */
    if (_bc_80196E98) {
        psx_check_interrupts_at(cpu, 0x80196ED4u);
        goto block_80196ED4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196EA0u);
        goto block_80196EA0;  /* not taken */
    }

block_80196EA0:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14256);
    /* nop */  /* 0x80196EA8: 0x00000000 */
    int _bc_80196EAC = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = -2;
    if (_bc_80196EAC) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196EB4u);
        goto block_80196EB4;  /* not taken */
    }

block_80196EB4:
    cpu->gpr[5] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[3];
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14228);
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x80196ECCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22600;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_80196ECC:
    /* delay slot (always executes) */
    cpu->gpr[2] = -2;
    psx_check_interrupts_at(cpu, 0x801971F8u);
    goto block_801971F8;  /* j */

block_80196ED4:
    cpu->gpr[31] = 0x80196EDCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801968CCu);
    cpu->pc = 0x801968CCu; return;  /* CPS jal -> 0x801968CC */

block_80196EDC:
    cpu->gpr[3] = cpu->gpr[17] & 0x00FF;  /* 0x80196EDC: 0x322300FF */
    cpu->gpr[2] = 2;
    int _bc_80196EE4 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 14;
    if (_bc_80196EE4) {
        psx_check_interrupts_at(cpu, 0x80196F1Cu);
        goto block_80196F1C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196EECu);
        goto block_80196EEC;  /* not taken */
    }

block_80196EEC:
    cpu->gpr[4] = cpu->gpr[0];
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[4];
    /* fall through to block_80196EF4 */

block_80196EF4:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2]);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[1] = cpu->gpr[1] + cpu->gpr[4];
    cpu->write_byte(cpu->gpr[1] + -14240, (uint8_t)cpu->gpr[2]);
    cpu->gpr[4] = cpu->gpr[4] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < 4) ? 1 : 0;  /* 0x80196F08: 0x28820004 */
    int _bc_80196F0C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[4];
    if (_bc_80196F0C) {
        psx_check_interrupts_at(cpu, 0x80196EF4u);
        goto block_80196EF4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F14u);
        goto block_80196F14;  /* not taken */
    }

block_80196F14:
    cpu->gpr[3] = cpu->gpr[17] & 0x00FF;  /* 0x80196F14: 0x322300FF */
    cpu->gpr[2] = 14;
    /* fall through to block_80196F1C */

block_80196F1C:
    int _bc_80196F1C = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80196F20: 0x00000000 */
    if (_bc_80196F1C) {
        psx_check_interrupts_at(cpu, 0x80196F30u);
        goto block_80196F30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F24u);
        goto block_80196F24;  /* not taken */
    }

block_80196F24:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[16]);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_byte(cpu->gpr[1] + -14236, (uint8_t)cpu->gpr[2]);
    /* fall through to block_80196F30 */

block_80196F30:
    cpu->gpr[5] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[5] = cpu->gpr[5] + -13532;
    cpu->gpr[4] = cpu->gpr[3] << 2;
    cpu->write_byte(cpu->gpr[5], (uint8_t)cpu->gpr[0]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13940);
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    int _bc_80196F50 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] + -13940;
    if (_bc_80196F50) {
        psx_check_interrupts_at(cpu, 0x80196F5Cu);
        goto block_80196F5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F58u);
        goto block_80196F58;  /* not taken */
    }

block_80196F58:
    cpu->write_byte(cpu->gpr[5] + 1, (uint8_t)cpu->gpr[0]);
    /* fall through to block_80196F5C */

block_80196F5C:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13556);
    /* nop */  /* 0x80196F64: 0x00000000 */
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[0]);
    cpu->gpr[2] = cpu->gpr[3] + 256;
    cpu->gpr[3] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x80196F78: 0x00000000 */
    int _bc_80196F7C = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    if (_bc_80196F7C) {
        psx_check_interrupts_at(cpu, 0x80196FB4u);
        goto block_80196FB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196F84u);
        goto block_80196F84;  /* not taken */
    }

block_80196F84:
    cpu->gpr[5] = cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[4];
    /* fall through to block_80196F8C */

block_80196F8C:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -13548);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2]);
    /* nop */  /* 0x80196F98: 0x00000000 */
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5]);
    cpu->gpr[4] = cpu->gpr[4] + 1;
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x80196FA8: 0x0082102A */
    int _bc_80196FAC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[4];
    if (_bc_80196FAC) {
        psx_check_interrupts_at(cpu, 0x80196F8Cu);
        goto block_80196F8C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196FB4u);
        goto block_80196FB4;  /* not taken */
    }

block_80196FB4:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13552);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_byte(cpu->gpr[1] + -14235, (uint8_t)cpu->gpr[17]);
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[17]);
    int _bc_80196FC8 = (cpu->gpr[18] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_80196FC8) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80196FD0u);
        goto block_80196FD0;  /* not taken */
    }

block_80196FD0:
    cpu->gpr[31] = 0x80196FD8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80196FD8:
    cpu->gpr[2] = cpu->gpr[2] + 960;
    cpu->gpr[4] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[4] = cpu->gpr[4] + -13532;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14624, cpu->gpr[2]);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14620, cpu->gpr[0]);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[4]);
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[2] = cpu->gpr[2] + 22616;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14616, cpu->gpr[2]);
    int _bc_80197008 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[22];
    if (_bc_80197008) {
        psx_check_interrupts_at(cpu, 0x801971A8u);
        goto block_801971A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197010u);
        goto block_80197010;  /* not taken */
    }

block_80197010:
    cpu->gpr[21] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[21] = cpu->gpr[21] + -14228;
    cpu->gpr[19] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[19] = cpu->gpr[19] + -14100;
    cpu->gpr[18] = cpu->gpr[4];
    cpu->gpr[20] = cpu->gpr[18] + 1;
    /* fall through to block_80197028 */

block_80197028:
    cpu->gpr[31] = 0x80197030u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80197030:
    cpu->gpr[3] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14624);
    /* nop */  /* 0x80197038: 0x00000000 */
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x8019703C: 0x0062182A */
    int _bc_80197040 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80197044: 0x00000000 */
    if (_bc_80197040) {
        psx_check_interrupts_at(cpu, 0x80197074u);
        goto block_80197074;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197048u);
        goto block_80197048;  /* not taken */
    }

block_80197048:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14620);
    /* nop */  /* 0x80197050: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[2] + 1;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -14620, cpu->gpr[2]);
    cpu->gpr[2] = 0x003C << 16;  /* 0x003C0000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x80197068: 0x0043102A */
    int _bc_8019706C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80197070: 0x00000000 */
    if (_bc_8019706C) {
        psx_check_interrupts_at(cpu, 0x801970DCu);
        goto block_801970DC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197074u);
        goto block_80197074;  /* not taken */
    }

block_80197074:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x80197080u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22436;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u; return;  /* CPS jal -> 0x80197AD0 */

block_80197080:
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[18]);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[18] + 1);
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[5] = cpu->read_word(cpu->gpr[5] + -14616);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[19];
    cpu->gpr[4] = cpu->gpr[4] << 2;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + -14235);
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[19];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[21];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[3]);
    cpu->gpr[6] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[4]);
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x801970CCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 22452;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_801970CC:
    cpu->gpr[31] = 0x801970D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801970D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801972A8u);
    cpu->pc = 0x801972A8u; return;  /* CPS jal -> 0x801972A8 */

block_801970D4:
    /* delay slot (always executes) */
    cpu->gpr[2] = -1;
    psx_check_interrupts_at(cpu, 0x801970E0u);
    goto block_801970E0;  /* j */

block_801970DC:
    cpu->gpr[2] = cpu->gpr[0];
    /* fall through to block_801970E0 */

block_801970E0:
    int _bc_801970E0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = -1;
    if (_bc_801970E0) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801970E8u);
        goto block_801970E8;  /* not taken */
    }

block_801970E8:
    cpu->gpr[31] = 0x801970F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801970EC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019A7D4u);
    cpu->pc = 0x8019A7D4u; return;  /* CPS jal -> 0x8019A7D4 */

block_801970F0:
    int _bc_801970F0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801970F4: 0x00000000 */
    if (_bc_801970F0) {
        psx_check_interrupts_at(cpu, 0x80197198u);
        goto block_80197198;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801970F8u);
        goto block_801970F8;  /* not taken */
    }

block_801970F8:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13556);
    /* nop */  /* 0x80197100: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2]);
    /* nop */  /* 0x80197108: 0x00000000 */
    cpu->gpr[17] = cpu->gpr[2] & 0x0003;  /* 0x8019710C: 0x30510003 */
    /* fall through to block_80197110 */

block_80197110:
    cpu->gpr[31] = 0x80197118u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80197114: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80196370u);
    cpu->pc = 0x80196370u; return;  /* CPS jal -> 0x80196370 */

block_80197118:
    cpu->gpr[16] = cpu->gpr[2];
    int _bc_8019711C = (cpu->gpr[16] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[16] & 0x0004;  /* 0x80197120: 0x32020004 */
    if (_bc_8019711C) {
        psx_check_interrupts_at(cpu, 0x80197188u);
        goto block_80197188;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197124u);
        goto block_80197124;  /* not taken */
    }

block_80197124:
    int _bc_80197124 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[16] & 0x0002;  /* 0x80197128: 0x32020002 */
    if (_bc_80197124) {
        psx_check_interrupts_at(cpu, 0x80197154u);
        goto block_80197154;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019712Cu);
        goto block_8019712C;  /* not taken */
    }

block_8019712C:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14260);
    /* nop */  /* 0x80197134: 0x00000000 */
    int _bc_80197138 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019713C: 0x00000000 */
    if (_bc_80197138) {
        psx_check_interrupts_at(cpu, 0x80197150u);
        goto block_80197150;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197140u);
        goto block_80197140;  /* not taken */
    }

block_80197140:
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[20]);
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    uint32_t _jt_80197148 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x80197150u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + -14640;
    psx_check_interrupts_at(cpu, _jt_80197148);
    cpu->pc = _jt_80197148; return;  /* CPS jalr */

block_80197150:
    cpu->gpr[2] = cpu->gpr[16] & 0x0002;  /* 0x80197150: 0x32020002 */
    /* fall through to block_80197154 */

block_80197154:
    int _bc_80197154 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80197158: 0x00000000 */
    if (_bc_80197154) {
        psx_check_interrupts_at(cpu, 0x80197110u);
        goto block_80197110;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019715Cu);
        goto block_8019715C;  /* not taken */
    }

block_8019715C:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14264);
    /* nop */  /* 0x80197164: 0x00000000 */
    int _bc_80197168 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019716C: 0x00000000 */
    if (_bc_80197168) {
        psx_check_interrupts_at(cpu, 0x80197110u);
        goto block_80197110;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80197170u);
        goto block_80197170;  /* not taken */
    }

block_80197170:
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[18]);
    cpu->gpr[5] = 0x8020 << 16;  /* 0x80200000 */
    uint32_t _jt_80197178 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x80197180u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + -14648;
    psx_check_interrupts_at(cpu, _jt_80197178);
    cpu->pc = _jt_80197178; return;  /* CPS jalr */

block_80197180:
    /* delay slot (always executes) */
    /* nop */  /* 0x80197184: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80197110u);
    goto block_80197110;  /* j */

block_80197188:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -13556);
    /* nop */  /* 0x80197190: 0x00000000 */
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[17]);
    /* fall through to block_80197198 */

block_80197198:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[18]);
    /* nop */  /* 0x8019719C: 0x00000000 */
    int _bc_801971A0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[22];
    if (_bc_801971A0) {
        psx_check_interrupts_at(cpu, 0x80197028u);
        goto block_80197028;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971A8u);
        goto block_801971A8;  /* not taken */
    }

block_801971A8:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->gpr[4] + -14648;
    int _bc_801971B0 = (cpu->gpr[6] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 7;
    if (_bc_801971B0) {
        psx_check_interrupts_at(cpu, 0x801971D4u);
        goto block_801971D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971B8u);
        goto block_801971B8;  /* not taken */
    }

block_801971B8:
    cpu->gpr[5] = -1;
    /* fall through to block_801971BC */

block_801971BC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4]);
    cpu->gpr[4] = cpu->gpr[4] + 1;
    cpu->gpr[3] = cpu->gpr[3] + -1;
    cpu->write_byte(cpu->gpr[6], (uint8_t)cpu->gpr[2]);
    int _bc_801971CC = (cpu->gpr[3] != cpu->gpr[5]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[6] + 1;
    if (_bc_801971CC) {
        psx_check_interrupts_at(cpu, 0x801971BCu);
        goto block_801971BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971D4u);
        goto block_801971D4;  /* not taken */
    }

block_801971D4:
    cpu->gpr[4] = cpu->gpr[0];
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->gpr[2] + -13532;
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[2]);
    cpu->gpr[2] = 5;
    int _bc_801971E8 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4];
    if (_bc_801971E8) {
        psx_check_interrupts_at(cpu, 0x801971F8u);
        goto block_801971F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801971F0u);
        goto block_801971F0;  /* not taken */
    }

block_801971F0:
    cpu->gpr[4] = -1;
    cpu->gpr[2] = cpu->gpr[4];
    /* fall through to block_801971F8 */

block_801971F8:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 52);
    cpu->gpr[22] = cpu->read_word(cpu->gpr[29] + 48);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 24);
    uint32_t _jt_80197218 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 56;
    psx_check_interrupts_at(cpu, _jt_80197218);
    cpu->pc = _jt_80197218; return;  /* CPS: jr $ra */
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

/* cps_80198c90 — clean CPS transliteration of br2eu_func_80198C90 (guest 0x80198c90)
 * from SLES_017.22_full_46.c. */
void cps_80198c90(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80198CD4u: goto block_80198CD4;
            case 0x80198CECu: goto block_80198CEC;
            case 0x80198D50u: goto block_80198D50;
            case 0x80198DA4u: goto block_80198DA4;
            case 0x80198DB0u: goto block_80198DB0;
            case 0x80198DD0u: goto block_80198DD0;
            case 0x80198DDCu: goto block_80198DDC;
            case 0x80198DE8u: goto block_80198DE8;
            case 0x80198E00u: goto block_80198E00;
            case 0x80198E08u: goto block_80198E08;
            case 0x80198CCCu: goto block_80198CCC;
            case 0x80198CE4u: goto block_80198CE4;
            case 0x80198CFCu: goto block_80198CFC;
            case 0x80198D0Cu: goto block_80198D0C;
            case 0x80198D2Cu: goto block_80198D2C;
            case 0x80198D34u: goto block_80198D34;
            case 0x80198D3Cu: goto block_80198D3C;
            case 0x80198D5Cu: goto block_80198D5C;
            case 0x80198D68u: goto block_80198D68;
            case 0x80198D74u: goto block_80198D74;
            case 0x80198D80u: goto block_80198D80;
            case 0x80198DC8u: goto block_80198DC8;
            case 0x80198DD4u: goto block_80198DD4;
            case 0x80198DF4u: goto block_80198DF4;
            default: break;
        }
    }
    /* Address: 0x80198C90, Size: 412 bytes, Blocks: 25 */

block_80198C90:
    cpu->gpr[29] = cpu->gpr[29] + -40;
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[20]);
    cpu->gpr[20] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[19]);
    cpu->gpr[19] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13404;
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16]);
    /* nop */  /* 0x80198CC0: 0x00000000 */
    int _bc_80198CC4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[6];
    if (_bc_80198CC4) {
        psx_check_interrupts_at(cpu, 0x80198D0Cu);
        goto block_80198D0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198CCCu);
        goto block_80198CCC;  /* not taken */
    }

block_80198CCC:
    cpu->gpr[31] = 0x80198CD4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198CD4:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16]);
    /* nop */  /* 0x80198CD8: 0x00000000 */
    int _bc_80198CDC = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = cpu->gpr[2];
    if (_bc_80198CDC) {
        psx_check_interrupts_at(cpu, 0x80198D0Cu);
        goto block_80198D0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198CE4u);
        goto block_80198CE4;  /* not taken */
    }

block_80198CE4:
    cpu->gpr[31] = 0x80198CECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198CEC:
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[17];
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)121) ? 1 : 0;  /* 0x80198CF0: 0x2C420079 */
    int _bc_80198CF4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198CF8: 0x00000000 */
    if (_bc_80198CF4) {
        psx_check_interrupts_at(cpu, 0x80198D3Cu);
        goto block_80198D3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198CFCu);
        goto block_80198CFC;  /* not taken */
    }

block_80198CFC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16]);
    /* nop */  /* 0x80198D00: 0x00000000 */
    int _bc_80198D04 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198D08: 0x00000000 */
    if (_bc_80198D04) {
        psx_check_interrupts_at(cpu, 0x80198CE4u);
        goto block_80198CE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198D0Cu);
        goto block_80198D0C;  /* not taken */
    }

block_80198D0C:
    cpu->gpr[4] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[4] = cpu->gpr[4] + -13440;
    cpu->write_word(cpu->gpr[4] + 12, cpu->gpr[18]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 12);
    /* nop */  /* 0x80198D1C: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] & 0x0030;  /* 0x80198D20: 0x30430030 */
    int _bc_80198D24 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 32;
    if (_bc_80198D24) {
        psx_check_interrupts_at(cpu, 0x80198D5Cu);
        goto block_80198D5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198D2Cu);
        goto block_80198D2C;  /* not taken */
    }

block_80198D2C:
    int _bc_80198D2C = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 582;
    if (_bc_80198D2C) {
        psx_check_interrupts_at(cpu, 0x80198D68u);
        goto block_80198D68;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198D34u);
        goto block_80198D34;  /* not taken */
    }

block_80198D34:
    /* delay slot (always executes) */
    /* nop */  /* 0x80198D38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198D74u);
    goto block_80198D74;  /* j */

block_80198D3C:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13400;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[16]);
    cpu->gpr[31] = 0x80198D50u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + -40;
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> 0x80195D64 */

block_80198D50:
    cpu->write_word(cpu->gpr[16] + 36, cpu->gpr[0]);
    /* delay slot (always executes) */
    /* nop */  /* 0x80198D58: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198D0Cu);
    goto block_80198D0C;  /* j */

block_80198D5C:
    cpu->gpr[2] = 512;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[4] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80198D80u);
    goto block_80198D80;  /* j */

block_80198D68:
    cpu->gpr[2] = 585;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[4] + 16, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80198D80u);
    goto block_80198D80;  /* j */

block_80198D74:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->gpr[3] + -13440;
    cpu->write_word(cpu->gpr[3] + 16, cpu->gpr[2]);
    /* fall through to block_80198D80 */

block_80198D80:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13440;
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 12);
    cpu->gpr[4] = cpu->gpr[0];
    cpu->gpr[2] = cpu->gpr[2] | 0x0020;
    cpu->write_word(cpu->gpr[16] + 12, cpu->gpr[2]);
    cpu->write_word(cpu->gpr[16] + 4, cpu->gpr[19]);
    cpu->gpr[31] = 0x80198DA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16], cpu->gpr[20]);
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> 0x80195D64 */

block_80198DA4:
    cpu->gpr[4] = cpu->gpr[0];
    cpu->gpr[31] = 0x80198DB0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16] + 40, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80195D78u);
    cpu->pc = 0x80195D78u; return;  /* CPS jal -> 0x80195D78 */

block_80198DB0:
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -14412);
    cpu->write_word(cpu->gpr[16] + 44, cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[3] & 0x0001;  /* 0x80198DBC: 0x30630001 */
    int _bc_80198DC0 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198DC4: 0x00000000 */
    if (_bc_80198DC0) {
        psx_check_interrupts_at(cpu, 0x80198DD4u);
        goto block_80198DD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198DC8u);
        goto block_80198DC8;  /* not taken */
    }

block_80198DC8:
    cpu->gpr[31] = 0x80198DD0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return;  /* CPS jal -> 0x801961A8 */

block_80198DD0:
    cpu->write_word(cpu->gpr[16] + 48, cpu->gpr[2]);
    /* fall through to block_80198DD4 */

block_80198DD4:
    cpu->gpr[31] = 0x80198DDCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198DDC:
    cpu->write_word(cpu->gpr[16] + 28, cpu->gpr[2]);
    cpu->gpr[31] = 0x80198DE8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198DE4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80195BE0u);
    cpu->pc = 0x80195BE0u; return;  /* CPS jal -> 0x80195BE0 */

block_80198DE8:
    cpu->gpr[2] = cpu->gpr[2] & 0x00E0;  /* 0x80198DE8: 0x304200E0 */
    int _bc_80198DEC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 9;
    if (_bc_80198DEC) {
        psx_check_interrupts_at(cpu, 0x80198E00u);
        goto block_80198E00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198DF4u);
        goto block_80198DF4;  /* not taken */
    }

block_80198DF4:
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[31] = 0x80198E00u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80195FFCu);
    cpu->pc = 0x80195FFCu; return;  /* CPS jal -> 0x80195FFC */

block_80198E00:
    cpu->gpr[31] = 0x80198E08u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80198908u);
    cpu->pc = 0x80198908u; return;  /* CPS jal -> 0x80198908 */

block_80198E08:
    cpu->gpr[2] = ((int32_t)cpu->gpr[0] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x80198E08: 0x0002102A */
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_80198E24 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 40;
    psx_check_interrupts_at(cpu, _jt_80198E24);
    cpu->pc = _jt_80198E24; return;  /* CPS: jr $ra */
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

/* cps_8019a5d8 — clean CPS transliteration of br2eu_func_8019A5D8 (guest 0x8019a5d8)
 * from SLES_017.22_full_46.c. */
void cps_8019a5d8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019A62Cu: goto block_8019A62C;
            case 0x8019A634u: goto block_8019A634;
            case 0x8019A640u: goto block_8019A640;
            case 0x8019A5FCu: goto block_8019A5FC;
            case 0x8019A600u: goto block_8019A600;
            case 0x8019A620u: goto block_8019A620;
            case 0x8019A648u: goto block_8019A648;
            case 0x8019A660u: goto block_8019A660;
            default: break;
        }
    }
    /* Address: 0x8019A5D8, Size: 152 bytes, Blocks: 9 */

block_8019A5D8:
    cpu->gpr[29] = cpu->gpr[29] + -32;
    cpu->gpr[5] = cpu->gpr[5] << 15;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[5]);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -8736);
    /* nop */  /* 0x8019A5EC: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[4]) ? 1 : 0;  /* 0x8019A5F0: 0x0044102A */
    int _bc_8019A5F4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[31]);
    if (_bc_8019A5F4) {
        psx_check_interrupts_at(cpu, 0x8019A660u);
        goto block_8019A660;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019A5FCu);
        goto block_8019A5FC;  /* not taken */
    }

block_8019A5FC:
    cpu->gpr[3] = -1;
    /* fall through to block_8019A600 */

block_8019A600:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 16);
    /* nop */  /* 0x8019A604: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + -1;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[29] + 16);
    /* nop */  /* 0x8019A614: 0x00000000 */
    int _bc_8019A618 = (cpu->gpr[2] != cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A61C: 0x00000000 */
    if (_bc_8019A618) {
        psx_check_interrupts_at(cpu, 0x8019A648u);
        goto block_8019A648;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019A620u);
        goto block_8019A620;  /* not taken */
    }

block_8019A620:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x8019A62Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 23352;
    psx_check_interrupts_at(cpu, 0x80197AD0u);
    cpu->pc = 0x80197AD0u; return;  /* CPS jal -> 0x80197AD0 */

block_8019A62C:
    cpu->gpr[31] = 0x8019A634u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80199FE0u);
    cpu->pc = 0x80199FE0u; return;  /* CPS jal -> 0x80199FE0 */

block_8019A634:
    cpu->gpr[4] = 3;
    cpu->gpr[31] = 0x8019A640u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x8019A670u);
    cpu->pc = 0x8019A670u; return;  /* CPS jal -> 0x8019A670 */

block_8019A640:
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A644: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019A660u);
    goto block_8019A660;  /* j */

block_8019A648:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -8736);
    /* nop */  /* 0x8019A650: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[4]) ? 1 : 0;  /* 0x8019A654: 0x0044102A */
    int _bc_8019A658 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A65C: 0x00000000 */
    if (_bc_8019A658) {
        psx_check_interrupts_at(cpu, 0x8019A600u);
        goto block_8019A600;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019A660u);
        goto block_8019A660;  /* not taken */
    }

block_8019A660:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[29] = cpu->gpr[29] + 32;
    uint32_t _jt_8019A668 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A66C: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019A668);
    cpu->pc = _jt_8019A668; return;  /* CPS: jr $ra */
}

/* cps_8019aabc — clean CPS transliteration of br2eu_func_8019AABC (guest 0x8019aabc)
 * from SLES_017.22_full_46.c. */
void cps_8019aabc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019AB80u: goto block_8019AB80;
            case 0x8019AB8Cu: goto block_8019AB8C;
            case 0x8019ABA4u: goto block_8019ABA4;
            case 0x8019ABBCu: goto block_8019ABBC;
            case 0x8019ABD0u: goto block_8019ABD0;
            case 0x8019AB00u: goto block_8019AB00;
            case 0x8019AB10u: goto block_8019AB10;
            case 0x8019AB2Cu: goto block_8019AB2C;
            case 0x8019AB4Cu: goto block_8019AB4C;
            case 0x8019AB6Cu: goto block_8019AB6C;
            case 0x8019AB74u: goto block_8019AB74;
            case 0x8019AB90u: goto block_8019AB90;
            case 0x8019AB98u: goto block_8019AB98;
            case 0x8019ABA8u: goto block_8019ABA8;
            case 0x8019ABB0u: goto block_8019ABB0;
            case 0x8019ABC0u: goto block_8019ABC0;
            case 0x8019ABC8u: goto block_8019ABC8;
            case 0x8019ABE0u: goto block_8019ABE0;
            case 0x8019ABE4u: goto block_8019ABE4;
            default: break;
        }
    }
    /* Address: 0x8019AABC, Size: 328 bytes, Blocks: 20 */

block_8019AABC:
    cpu->gpr[29] = cpu->gpr[29] + -40;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[5];
    cpu->gpr[5] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[5] = cpu->gpr[5] + -13028;
    cpu->gpr[2] = cpu->gpr[17] << 2;
    cpu->gpr[4] = cpu->gpr[2] + cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[4]);
    /* nop */  /* 0x8019AAF4: 0x00000000 */
    int _bc_8019AAF8 = (cpu->gpr[18] == cpu->gpr[20]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[20];
    if (_bc_8019AAF8) {
        psx_check_interrupts_at(cpu, 0x8019ABE4u);
        goto block_8019ABE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019AB00u);
        goto block_8019AB00;  /* not taken */
    }

block_8019AB00:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[5] + -4);
    /* nop */  /* 0x8019AB04: 0x00000000 */
    int _bc_8019AB08 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[5] + -4;
    if (_bc_8019AB08) {
        psx_check_interrupts_at(cpu, 0x8019ABE0u);
        goto block_8019ABE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019AB10u);
        goto block_8019AB10;  /* not taken */
    }

block_8019AB10:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -8792);
    /* nop */  /* 0x8019AB18: 0x00000000 */
    cpu->gpr[3] = cpu->read_half(cpu->gpr[2]);
    cpu->write_half(cpu->gpr[2], (uint16_t)cpu->gpr[0]);
    int _bc_8019AB24 = (cpu->gpr[18] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[19] = cpu->gpr[3] & 0xFFFF;  /* 0x8019AB28: 0x3073FFFF */
    if (_bc_8019AB24) {
        psx_check_interrupts_at(cpu, 0x8019AB4Cu);
        goto block_8019AB4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019AB2Cu);
        goto block_8019AB2C;  /* not taken */
    }

block_8019AB2C:
    cpu->gpr[3] = 1;
    cpu->gpr[3] = cpu->gpr[3] << (cpu->gpr[17] & 0x1F);
    cpu->write_word(cpu->gpr[4], cpu->gpr[18]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 48);
    cpu->gpr[19] = cpu->gpr[19] | cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[6] + 48, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8019AB6Cu);
    goto block_8019AB6C;  /* j */

block_8019AB4C:
    cpu->gpr[2] = 1;
    cpu->gpr[2] = cpu->gpr[2] << (cpu->gpr[17] & 0x1F);
    cpu->gpr[2] = ~cpu->gpr[2];  /* not */  /* 0x8019AB54: 0x00021027 */
    cpu->write_word(cpu->gpr[4], cpu->gpr[0]);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[5] + 44);
    cpu->gpr[19] = cpu->gpr[19] & cpu->gpr[2];  /* 0x8019AB60: 0x02629824 */
    cpu->gpr[3] = cpu->gpr[3] & cpu->gpr[2];  /* 0x8019AB64: 0x00621824 */
    cpu->write_half(cpu->gpr[5] + 44, (uint16_t)cpu->gpr[3]);
    /* fall through to block_8019AB6C */

block_8019AB6C:
    int _bc_8019AB6C = (cpu->gpr[17] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    if (_bc_8019AB6C) {
        psx_check_interrupts_at(cpu, 0x8019AB90u);
        goto block_8019AB90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019AB74u);
        goto block_8019AB74;  /* not taken */
    }

block_8019AB74:
    cpu->gpr[16] = (cpu->gpr[18] < (uint32_t)1) ? 1 : 0;  /* 0x8019AB74: 0x2E500001 */
    cpu->gpr[31] = 0x8019AB80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x80199FE0u);
    cpu->pc = 0x80199FE0u; return;  /* CPS jal -> 0x80199FE0 */

block_8019AB80:
    cpu->gpr[4] = 3;
    cpu->gpr[31] = 0x8019AB8Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x8019A670u);
    cpu->pc = 0x8019A670u; return;  /* CPS jal -> 0x8019A670 */

block_8019AB8C:
    cpu->gpr[2] = 4;
    /* fall through to block_8019AB90 */

block_8019AB90:
    int _bc_8019AB90 = (cpu->gpr[17] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 5;
    if (_bc_8019AB90) {
        psx_check_interrupts_at(cpu, 0x8019ABA8u);
        goto block_8019ABA8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019AB98u);
        goto block_8019AB98;  /* not taken */
    }

block_8019AB98:
    cpu->gpr[4] = cpu->gpr[0];
    cpu->gpr[31] = 0x8019ABA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = (cpu->gpr[18] < (uint32_t)1) ? 1 : 0;  /* 0x8019ABA0: 0x2E450001 */
    psx_check_interrupts_at(cpu, 0x8019A670u);
    cpu->pc = 0x8019A670u; return;  /* CPS jal -> 0x8019A670 */

block_8019ABA4:
    cpu->gpr[2] = 5;
    /* fall through to block_8019ABA8 */

block_8019ABA8:
    int _bc_8019ABA8 = (cpu->gpr[17] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 6;
    if (_bc_8019ABA8) {
        psx_check_interrupts_at(cpu, 0x8019ABC0u);
        goto block_8019ABC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019ABB0u);
        goto block_8019ABB0;  /* not taken */
    }

block_8019ABB0:
    cpu->gpr[4] = 1;
    cpu->gpr[31] = 0x8019ABBCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = (cpu->gpr[18] < (uint32_t)1) ? 1 : 0;  /* 0x8019ABB8: 0x2E450001 */
    psx_check_interrupts_at(cpu, 0x8019A670u);
    cpu->pc = 0x8019A670u; return;  /* CPS jal -> 0x8019A670 */

block_8019ABBC:
    cpu->gpr[2] = 6;
    /* fall through to block_8019ABC0 */

block_8019ABC0:
    int _bc_8019ABC0 = (cpu->gpr[17] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 2;
    if (_bc_8019ABC0) {
        psx_check_interrupts_at(cpu, 0x8019ABD0u);
        goto block_8019ABD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019ABC8u);
        goto block_8019ABC8;  /* not taken */
    }

block_8019ABC8:
    cpu->gpr[31] = 0x8019ABD0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = (cpu->gpr[18] < (uint32_t)1) ? 1 : 0;  /* 0x8019ABCC: 0x2E450001 */
    psx_check_interrupts_at(cpu, 0x8019A670u);
    cpu->pc = 0x8019A670u; return;  /* CPS jal -> 0x8019A670 */

block_8019ABD0:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -8792);
    /* nop */  /* 0x8019ABD8: 0x00000000 */
    cpu->write_half(cpu->gpr[2], (uint16_t)cpu->gpr[19]);
    /* fall through to block_8019ABE0 */

block_8019ABE0:
    cpu->gpr[2] = cpu->gpr[20];
    /* fall through to block_8019ABE4 */

block_8019ABE4:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_8019ABFC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 40;
    psx_check_interrupts_at(cpu, _jt_8019ABFC);
    cpu->pc = _jt_8019ABFC; return;  /* CPS: jr $ra */
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

/* cps_801a0660 — clean CPS transliteration of br2eu_func_801A0660 (guest 0x801a0660)
 * from SLES_017.22_full_49.c. */
void cps_801a0660(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A06D4u: goto block_801A06D4;
            case 0x801A06E4u: goto block_801A06E4;
            case 0x801A06F4u: goto block_801A06F4;
            case 0x801A0708u: goto block_801A0708;
            case 0x801A0710u: goto block_801A0710;
            case 0x801A0720u: goto block_801A0720;
            case 0x801A0734u: goto block_801A0734;
            case 0x801A0744u: goto block_801A0744;
            case 0x801A069Cu: goto block_801A069C;
            case 0x801A06B4u: goto block_801A06B4;
            case 0x801A06C0u: goto block_801A06C0;
            case 0x801A06CCu: goto block_801A06CC;
            case 0x801A06ECu: goto block_801A06EC;
            case 0x801A0700u: goto block_801A0700;
            case 0x801A0728u: goto block_801A0728;
            case 0x801A073Cu: goto block_801A073C;
            case 0x801A0750u: goto block_801A0750;
            case 0x801A0758u: goto block_801A0758;
            case 0x801A076Cu: goto block_801A076C;
            case 0x801A077Cu: goto block_801A077C;
            case 0x801A0780u: goto block_801A0780;
            default: break;
        }
    }
    /* Address: 0x801A0660, Size: 324 bytes, Blocks: 22 */

block_801A0660:
    cpu->gpr[29] = cpu->gpr[29] + -176;
    cpu->write_word(cpu->gpr[29] + 144, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 160, cpu->gpr[20]);
    cpu->gpr[20] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 156, cpu->gpr[19]);
    cpu->gpr[19] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 148, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[0];
    cpu->gpr[3] = cpu->gpr[19];
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[4] = 126;
    cpu->write_word(cpu->gpr[29] + 168, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 164, cpu->gpr[21]);
    cpu->write_word(cpu->gpr[29] + 152, cpu->gpr[18]);
    /* fall through to block_801A069C */

block_801A069C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[3]);
    cpu->gpr[3] = cpu->gpr[3] + 1;
    cpu->gpr[4] = cpu->gpr[4] + -1;
    cpu->gpr[2] = cpu->gpr[5] ^ cpu->gpr[2];  /* 0x801A06A8: 0x00A21026 */
    int _bc_801A06AC = ((int32_t)cpu->gpr[4] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[2];
    if (_bc_801A06AC) {
        psx_check_interrupts_at(cpu, 0x801A069Cu);
        goto block_801A069C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A06B4u);
        goto block_801A06B4;  /* not taken */
    }

block_801A06B4:
    cpu->write_byte(cpu->gpr[3], (uint8_t)cpu->gpr[2]);
    cpu->gpr[21] = 1;
    cpu->gpr[18] = cpu->gpr[29] + 16;
    /* fall through to block_801A06C0 */

block_801A06C0:
    cpu->gpr[2] = ((int32_t)cpu->gpr[17] < 8) ? 1 : 0;  /* 0x801A06C0: 0x2A220008 */
    int _bc_801A06C4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_801A06C4) {
        psx_check_interrupts_at(cpu, 0x801A0780u);
        goto block_801A0780;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A06CCu);
        goto block_801A06CC;  /* not taken */
    }

block_801A06CC:
    cpu->gpr[31] = 0x801A06D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A06D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A0610u);
    cpu->pc = 0x801A0610u; return;  /* CPS jal -> 0x801A0610 */

block_801A06D4:
    cpu->gpr[4] = cpu->gpr[16];
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[31] = 0x801A06E4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[19];
    psx_check_interrupts_at(cpu, 0x801A0600u);
    cpu->pc = 0x801A0600u; return;  /* CPS jal -> 0x801A0600 */

block_801A06E4:
    int _bc_801A06E4 = (cpu->gpr[2] != cpu->gpr[21]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_801A06E4) {
        psx_check_interrupts_at(cpu, 0x801A0780u);
        goto block_801A0780;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A06ECu);
        goto block_801A06EC;  /* not taken */
    }

block_801A06EC:
    cpu->gpr[31] = 0x801A06F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = (int32_t)cpu->gpr[16] >> 4;
    psx_check_interrupts_at(cpu, 0x801A09F0u);
    cpu->pc = 0x801A09F0u; return;  /* CPS jal -> 0x801A09F0 */

block_801A06F4:
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x801A06F4: 0x30420001 */
    int _bc_801A06F8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[18];
    if (_bc_801A06F8) {
        psx_check_interrupts_at(cpu, 0x801A06ECu);
        goto block_801A06EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0700u);
        goto block_801A0700;  /* not taken */
    }

block_801A0700:
    cpu->gpr[31] = 0x801A0708u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = 128;
    psx_check_interrupts_at(cpu, 0x801A09B0u);
    cpu->pc = 0x801A09B0u; return;  /* CPS jal -> 0x801A09B0 */

block_801A0708:
    cpu->gpr[31] = 0x801A0710u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A070C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A0610u);
    cpu->pc = 0x801A0610u; return;  /* CPS jal -> 0x801A0610 */

block_801A0710:
    cpu->gpr[4] = cpu->gpr[16];
    cpu->gpr[5] = cpu->gpr[20];
    cpu->gpr[31] = 0x801A0720u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[18];
    psx_check_interrupts_at(cpu, 0x801A09E0u);
    cpu->pc = 0x801A09E0u; return;  /* CPS jal -> 0x801A09E0 */

block_801A0720:
    int _bc_801A0720 = (cpu->gpr[2] == cpu->gpr[21]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0724: 0x00000000 */
    if (_bc_801A0720) {
        psx_check_interrupts_at(cpu, 0x801A073Cu);
        goto block_801A073C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0728u);
        goto block_801A0728;  /* not taken */
    }

block_801A0728:
    cpu->gpr[4] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[31] = 0x801A0734u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 24440;
    psx_check_interrupts_at(cpu, 0x8018E550u);
    cpu->pc = 0x8018E550u; return;  /* CPS jal -> 0x8018E550 */

block_801A0734:
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[29] + 16;
    psx_check_interrupts_at(cpu, 0x801A0750u);
    goto block_801A0750;  /* j */

block_801A073C:
    cpu->gpr[31] = 0x801A0744u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = (int32_t)cpu->gpr[16] >> 4;
    psx_check_interrupts_at(cpu, 0x801A09F0u);
    cpu->pc = 0x801A09F0u; return;  /* CPS jal -> 0x801A09F0 */

block_801A0744:
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x801A0744: 0x30420001 */
    int _bc_801A0748 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[29] + 16;
    if (_bc_801A0748) {
        psx_check_interrupts_at(cpu, 0x801A073Cu);
        goto block_801A073C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0750u);
        goto block_801A0750;  /* not taken */
    }

block_801A0750:
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[3] = 126;
    /* fall through to block_801A0758 */

block_801A0758:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4]);
    cpu->gpr[4] = cpu->gpr[4] + 1;
    cpu->gpr[3] = cpu->gpr[3] + -1;
    int _bc_801A0764 = ((int32_t)cpu->gpr[3] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] ^ cpu->gpr[2];  /* 0x801A0768: 0x00A22826 */
    if (_bc_801A0764) {
        psx_check_interrupts_at(cpu, 0x801A0758u);
        goto block_801A0758;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A076Cu);
        goto block_801A076C;  /* not taken */
    }

block_801A076C:
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[19] + 127);
    cpu->gpr[2] = cpu->gpr[5] & 0x00FF;  /* 0x801A0770: 0x30A200FF */
    int _bc_801A0774 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = cpu->gpr[17] + 1;
    if (_bc_801A0774) {
        psx_check_interrupts_at(cpu, 0x801A06C0u);
        goto block_801A06C0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A077Cu);
        goto block_801A077C;  /* not taken */
    }

block_801A077C:
    cpu->gpr[2] = 1;
    /* fall through to block_801A0780 */

block_801A0780:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 168);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 164);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 160);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 156);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 152);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 148);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 144);
    uint32_t _jt_801A079C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 176;
    psx_check_interrupts_at(cpu, _jt_801A079C);
    cpu->pc = _jt_801A079C; return;  /* CPS: jr $ra */
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

/* cps_80198af0 — clean CPS transliteration of br2eu_func_80198AF0 (guest 0x80198af0)
 * from SLES_017.22_full_46.c. */
void cps_80198af0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80198B18u: goto block_80198B18;
            case 0x80198B50u: goto block_80198B50;
            case 0x80198B70u: goto block_80198B70;
            case 0x80198B84u: goto block_80198B84;
            case 0x80198BA0u: goto block_80198BA0;
            case 0x80198BE0u: goto block_80198BE0;
            case 0x80198C04u: goto block_80198C04;
            case 0x80198C10u: goto block_80198C10;
            case 0x80198C1Cu: goto block_80198C1C;
            case 0x80198C40u: goto block_80198C40;
            case 0x80198C5Cu: goto block_80198C5C;
            case 0x80198B10u: goto block_80198B10;
            case 0x80198B34u: goto block_80198B34;
            case 0x80198B3Cu: goto block_80198B3C;
            case 0x80198B5Cu: goto block_80198B5C;
            case 0x80198B7Cu: goto block_80198B7C;
            case 0x80198B94u: goto block_80198B94;
            case 0x80198B98u: goto block_80198B98;
            case 0x80198BB0u: goto block_80198BB0;
            case 0x80198BC0u: goto block_80198BC0;
            case 0x80198BC4u: goto block_80198BC4;
            case 0x80198BCCu: goto block_80198BCC;
            case 0x80198BF8u: goto block_80198BF8;
            case 0x80198C38u: goto block_80198C38;
            case 0x80198C50u: goto block_80198C50;
            case 0x80198C54u: goto block_80198C54;
            case 0x80198C6Cu: goto block_80198C6C;
            case 0x80198C7Cu: goto block_80198C7C;
            default: break;
        }
    }
    /* Address: 0x80198AF0, Size: 416 bytes, Blocks: 29 */

block_80198AF0:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14412);
    cpu->gpr[29] = cpu->gpr[29] + -32;
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80198B04: 0x30420001 */
    int _bc_80198B08 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    if (_bc_80198B08) {
        psx_check_interrupts_at(cpu, 0x80198B18u);
        goto block_80198B18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198B10u);
        goto block_80198B10;  /* not taken */
    }

block_80198B10:
    cpu->gpr[31] = 0x80198B18u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x801961CCu);
    cpu->pc = 0x801961CCu; return;  /* CPS jal -> 0x801961CC */

block_80198B18:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13440;
    cpu->write_word(cpu->gpr[16] + 20, cpu->gpr[0]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 36);
    /* nop */  /* 0x80198B28: 0x00000000 */
    int _bc_80198B2C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_80198B2C) {
        psx_check_interrupts_at(cpu, 0x80198B7Cu);
        goto block_80198B7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198B34u);
        goto block_80198B34;  /* not taken */
    }

block_80198B34:
    /* delay slot (always executes) */
    /* nop */  /* 0x80198B38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198BC4u);
    goto block_80198BC4;  /* j */

block_80198B3C:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13400;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[16]);
    cpu->gpr[31] = 0x80198B50u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + -40;
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> 0x80195D64 */

block_80198B50:
    cpu->gpr[2] = -1;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16] + 36, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80198BC4u);
    goto block_80198BC4;  /* j */

block_80198B5C:
    cpu->gpr[16] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[16] = cpu->gpr[16] + -13400;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[16]);
    cpu->gpr[31] = 0x80198B70u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + -40;
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> 0x80195D64 */

block_80198B70:
    cpu->write_word(cpu->gpr[16] + 36, cpu->gpr[0]);
    /* delay slot (always executes) */
    /* nop */  /* 0x80198B78: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80198C7Cu);
    goto block_80198C7C;  /* j */

block_80198B7C:
    cpu->gpr[31] = 0x80198B84u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198B84:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[16] + 36);
    /* nop */  /* 0x80198B88: 0x00000000 */
    int _bc_80198B8C = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[17] = cpu->gpr[2];
    if (_bc_80198B8C) {
        psx_check_interrupts_at(cpu, 0x80198BC0u);
        goto block_80198BC0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198B94u);
        goto block_80198B94;  /* not taken */
    }

block_80198B94:
    cpu->gpr[16] = cpu->gpr[16] + 36;
    /* fall through to block_80198B98 */

block_80198B98:
    cpu->gpr[31] = 0x80198BA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198BA0:
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[17];
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)121) ? 1 : 0;  /* 0x80198BA4: 0x2C420079 */
    int _bc_80198BA8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198BAC: 0x00000000 */
    if (_bc_80198BA8) {
        psx_check_interrupts_at(cpu, 0x80198B3Cu);
        goto block_80198B3C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BB0u);
        goto block_80198BB0;  /* not taken */
    }

block_80198BB0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16]);
    /* nop */  /* 0x80198BB4: 0x00000000 */
    int _bc_80198BB8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198BBC: 0x00000000 */
    if (_bc_80198BB8) {
        psx_check_interrupts_at(cpu, 0x80198B98u);
        goto block_80198B98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BC0u);
        goto block_80198BC0;  /* not taken */
    }

block_80198BC0:
    cpu->gpr[2] = cpu->gpr[0];
    /* fall through to block_80198BC4 */

block_80198BC4:
    int _bc_80198BC4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198BC8: 0x00000000 */
    if (_bc_80198BC4) {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
        goto block_80198C7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BCCu);
        goto block_80198BCC;  /* not taken */
    }

block_80198BCC:
    cpu->gpr[17] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[17] = cpu->gpr[17] + -13396;
    cpu->gpr[4] = cpu->read_word(cpu->gpr[17]);
    cpu->gpr[31] = 0x80198BE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198BDC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80195D78u);
    cpu->pc = 0x80195D78u; return;  /* CPS jal -> 0x80195D78 */

block_80198BE0:
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -14412);
    /* nop */  /* 0x80198BE8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80198BEC: 0x30420001 */
    int _bc_80198BF0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198BF4: 0x00000000 */
    if (_bc_80198BF0) {
        psx_check_interrupts_at(cpu, 0x80198C04u);
        goto block_80198C04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198BF8u);
        goto block_80198BF8;  /* not taken */
    }

block_80198BF8:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[17] + 4);
    cpu->gpr[31] = 0x80198C04u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198C00: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801961A8u);
    cpu->pc = 0x801961A8u; return;  /* CPS jal -> 0x801961A8 */

block_80198C04:
    cpu->gpr[4] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[31] = 0x80198C10u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + -31344;
    psx_check_interrupts_at(cpu, 0x80195D64u);
    cpu->pc = 0x80195D64u; return;  /* CPS jal -> 0x80195D64 */

block_80198C10:
    cpu->gpr[4] = 9;
    cpu->gpr[31] = 0x80198C1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80195EC8u);
    cpu->pc = 0x80195EC8u; return;  /* CPS jal -> 0x80195EC8 */

block_80198C1C:
    cpu->gpr[2] = cpu->gpr[17] + -44;
    cpu->gpr[3] = 1;
    cpu->write_word(cpu->gpr[2] + 36, cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[17] + -8);
    /* nop */  /* 0x80198C2C: 0x00000000 */
    int _bc_80198C30 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198C34: 0x00000000 */
    if (_bc_80198C30) {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
        goto block_80198C7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C38u);
        goto block_80198C38;  /* not taken */
    }

block_80198C38:
    cpu->gpr[31] = 0x80198C40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198C40:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + -8);
    /* nop */  /* 0x80198C44: 0x00000000 */
    int _bc_80198C48 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[2];
    if (_bc_80198C48) {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
        goto block_80198C7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C50u);
        goto block_80198C50;  /* not taken */
    }

block_80198C50:
    cpu->gpr[17] = cpu->gpr[17] + -8;
    /* fall through to block_80198C54 */

block_80198C54:
    cpu->gpr[31] = 0x80198C5Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = -1;
    psx_check_interrupts_at(cpu, 0x8019A460u);
    cpu->pc = 0x8019A460u; return;  /* CPS jal -> 0x8019A460 */

block_80198C5C:
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[16];
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)121) ? 1 : 0;  /* 0x80198C60: 0x2C420079 */
    int _bc_80198C64 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198C68: 0x00000000 */
    if (_bc_80198C64) {
        psx_check_interrupts_at(cpu, 0x80198B5Cu);
        goto block_80198B5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C6Cu);
        goto block_80198C6C;  /* not taken */
    }

block_80198C6C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[17]);
    /* nop */  /* 0x80198C70: 0x00000000 */
    int _bc_80198C74 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80198C78: 0x00000000 */
    if (_bc_80198C74) {
        psx_check_interrupts_at(cpu, 0x80198C54u);
        goto block_80198C54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80198C7Cu);
        goto block_80198C7C;  /* not taken */
    }

block_80198C7C:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_80198C88 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 32;
    psx_check_interrupts_at(cpu, _jt_80198C88);
    cpu->pc = _jt_80198C88; return;  /* CPS: jr $ra */
}

/* cps_8019a00c — clean CPS transliteration of br2eu_func_8019A00C (guest 0x8019a00c)
 * from SLES_017.22_full_46.c. */
void cps_8019a00c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019A038u: goto block_8019A038;
            case 0x8019A040u: goto block_8019A040;
            case 0x8019A048u: goto block_8019A048;
            case 0x8019A050u: goto block_8019A050;
            case 0x8019A058u: goto block_8019A058;
            case 0x8019A060u: goto block_8019A060;
            case 0x8019A074u: goto block_8019A074;
            default: break;
        }
    }
    /* Address: 0x8019A00C, Size: 144 bytes, Blocks: 8 */

block_8019A00C:
    cpu->gpr[29] = cpu->gpr[29] + -40;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[31]);
    cpu->gpr[31] = 0x8019A038u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[19] = cpu->gpr[7];
    psx_check_interrupts_at(cpu, 0x8019A3F0u);
    cpu->pc = 0x8019A3F0u; return;  /* CPS jal -> 0x8019A3F0 */

block_8019A038:
    cpu->gpr[31] = 0x8019A040u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A03C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019A040:
    cpu->gpr[31] = 0x8019A048u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A044: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019A378u);
    cpu->pc = 0x8019A378u; return;  /* CPS jal -> 0x8019A378 */

block_8019A048:
    cpu->gpr[31] = 0x8019A050u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A04C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_8019A050:
    cpu->gpr[31] = 0x8019A058u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80199FE0u);
    cpu->pc = 0x80199FE0u; return;  /* CPS jal -> 0x80199FE0 */

block_8019A058:
    cpu->gpr[31] = 0x8019A060u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A05C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019A194u);
    cpu->pc = 0x8019A194u; return;  /* CPS jal -> 0x8019A194 */

block_8019A060:
    cpu->gpr[4] = cpu->gpr[16];
    cpu->gpr[5] = cpu->gpr[17];
    cpu->gpr[6] = cpu->gpr[18];
    cpu->gpr[31] = 0x8019A074u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[19];
    psx_check_interrupts_at(cpu, 0x8019A320u);
    cpu->pc = 0x8019A320u; return;  /* CPS jal -> 0x8019A320 */

block_8019A074:
    cpu->gpr[2] = 1;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -13168, cpu->gpr[2]);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_8019A094 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 40;
    psx_check_interrupts_at(cpu, _jt_8019A094);
    cpu->pc = _jt_8019A094; return;  /* CPS: jr $ra */
}

/* cps_8019a09c — clean CPS transliteration of br2eu_func_8019A09C (guest 0x8019a09c)
 * from SLES_017.22_full_46.c. */
void cps_8019a09c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019A0C8u: goto block_8019A0C8;
            case 0x8019A0D0u: goto block_8019A0D0;
            case 0x8019A0D8u: goto block_8019A0D8;
            case 0x8019A0E0u: goto block_8019A0E0;
            case 0x8019A0E8u: goto block_8019A0E8;
            case 0x8019A0F0u: goto block_8019A0F0;
            case 0x8019A104u: goto block_8019A104;
            default: break;
        }
    }
    /* Address: 0x8019A09C, Size: 144 bytes, Blocks: 8 */

block_8019A09C:
    cpu->gpr[29] = cpu->gpr[29] + -40;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[5];
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->gpr[18] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[19]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[31]);
    cpu->gpr[31] = 0x8019A0C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[19] = cpu->gpr[7];
    psx_check_interrupts_at(cpu, 0x8019A3F0u);
    cpu->pc = 0x8019A3F0u; return;  /* CPS jal -> 0x8019A3F0 */

block_8019A0C8:
    cpu->gpr[31] = 0x8019A0D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A0CC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019A0D0:
    cpu->gpr[31] = 0x8019A0D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A0D4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019A378u);
    cpu->pc = 0x8019A378u; return;  /* CPS jal -> 0x8019A378 */

block_8019A0D8:
    cpu->gpr[31] = 0x8019A0E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A0DC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_8019A0E0:
    cpu->gpr[31] = 0x8019A0E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80199FE0u);
    cpu->pc = 0x80199FE0u; return;  /* CPS jal -> 0x80199FE0 */

block_8019A0E8:
    cpu->gpr[31] = 0x8019A0F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A0EC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019A194u);
    cpu->pc = 0x8019A194u; return;  /* CPS jal -> 0x8019A194 */

block_8019A0F0:
    cpu->gpr[4] = cpu->gpr[16];
    cpu->gpr[5] = cpu->gpr[17];
    cpu->gpr[6] = cpu->gpr[18];
    cpu->gpr[31] = 0x8019A104u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[19];
    psx_check_interrupts_at(cpu, 0x8019A2F0u);
    cpu->pc = 0x8019A2F0u; return;  /* CPS jal -> 0x8019A2F0 */

block_8019A104:
    cpu->gpr[2] = 1;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -13168, cpu->gpr[2]);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 32);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_8019A124 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 40;
    psx_check_interrupts_at(cpu, _jt_8019A124);
    cpu->pc = _jt_8019A124; return;  /* CPS: jr $ra */
}

/* cps_8019a378 — clean CPS transliteration of br2eu_func_8019A378 (guest 0x8019a378)
 * from SLES_017.22_full_46.c. */
void cps_8019a378(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019A388u: goto block_8019A388;
            case 0x8019A398u: goto block_8019A398;
            case 0x8019A3D4u: goto block_8019A3D4;
            case 0x8019A3B8u: goto block_8019A3B8;
            case 0x8019A3CCu: goto block_8019A3CC;
            default: break;
        }
    }
    /* Address: 0x8019A378, Size: 112 bytes, Blocks: 6 */

block_8019A378:
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -5296, cpu->gpr[31]);
    cpu->gpr[31] = 0x8019A388u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A384: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019A388:
    cpu->gpr[9] = 87;
    cpu->gpr[10] = 176;
    uint32_t _jt_8019A390 = cpu->gpr[10];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x8019A398u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A394: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019A390);
    cpu->pc = _jt_8019A390; return;  /* CPS jalr */

block_8019A398:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 364);
    cpu->gpr[9] = 11;
    cpu->gpr[3] = cpu->gpr[2] + 2180;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -5288, cpu->gpr[3]);
    cpu->gpr[3] = cpu->gpr[2] + 2196;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -5284, cpu->gpr[3]);
    /* fall through to block_8019A3B8 */

block_8019A3B8:
    cpu->write_word(cpu->gpr[2] + 1428, cpu->gpr[0]);
    cpu->gpr[2] = cpu->gpr[2] + 4;
    cpu->gpr[9] = cpu->gpr[9] + -1;
    int _bc_8019A3C4 = (cpu->gpr[9] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A3C8: 0x00000000 */
    if (_bc_8019A3C4) {
        psx_check_interrupts_at(cpu, 0x8019A3B8u);
        goto block_8019A3B8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019A3CCu);
        goto block_8019A3CC;  /* not taken */
    }

block_8019A3CC:
    cpu->gpr[31] = 0x8019A3D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A3D0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801957F0u);
    cpu->pc = 0x801957F0u; return;  /* CPS jal -> 0x801957F0 */

block_8019A3D4:
    cpu->gpr[31] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[31] = cpu->read_word(cpu->gpr[31] + -5296);
    /* nop */  /* 0x8019A3DC: 0x00000000 */
    uint32_t _jt_8019A3E0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A3E4: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019A3E0);
    cpu->pc = _jt_8019A3E0; return;  /* CPS: jr $ra */
}

/* cps_8019a3f0 — clean CPS transliteration of br2eu_func_8019A3F0 (guest 0x8019a3f0)
 * from SLES_017.22_full_46.c. */
void cps_8019a3f0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019A400u: goto block_8019A400;
            case 0x8019A410u: goto block_8019A410;
            case 0x8019A43Cu: goto block_8019A43C;
            case 0x8019A444u: goto block_8019A444;
            case 0x8019A420u: goto block_8019A420;
            case 0x8019A434u: goto block_8019A434;
            default: break;
        }
    }
    /* Address: 0x8019A3F0, Size: 104 bytes, Blocks: 7 */

block_8019A3F0:
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -5280, cpu->gpr[31]);
    cpu->gpr[31] = 0x8019A400u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A3FC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019A400:
    cpu->gpr[9] = 87;
    cpu->gpr[10] = 176;
    uint32_t _jt_8019A408 = cpu->gpr[10];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x8019A410u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A40C: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019A408);
    cpu->pc = _jt_8019A408; return;  /* CPS jalr */

block_8019A410:
    cpu->gpr[10] = 9;
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 364);
    /* nop */  /* 0x8019A418: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] + 1580;
    /* fall through to block_8019A420 */

block_8019A420:
    cpu->write_word(cpu->gpr[3], cpu->gpr[0]);
    cpu->gpr[3] = cpu->gpr[3] + 4;
    cpu->gpr[10] = cpu->gpr[10] + -1;
    int _bc_8019A42C = (cpu->gpr[10] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A430: 0x00000000 */
    if (_bc_8019A42C) {
        psx_check_interrupts_at(cpu, 0x8019A420u);
        goto block_8019A420;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019A434u);
        goto block_8019A434;  /* not taken */
    }

block_8019A434:
    cpu->gpr[31] = 0x8019A43Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A438: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801957F0u);
    cpu->pc = 0x801957F0u; return;  /* CPS jal -> 0x801957F0 */

block_8019A43C:
    cpu->gpr[31] = 0x8019A444u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A440: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_8019A444:
    cpu->gpr[31] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[31] = cpu->read_word(cpu->gpr[31] + -5280);
    /* nop */  /* 0x8019A44C: 0x00000000 */
    uint32_t _jt_8019A450 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019A454: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019A450);
    cpu->pc = _jt_8019A450; return;  /* CPS: jr $ra */
}

/* cps_8019e330 — clean CPS transliteration of br2eu_func_8019E330 (guest 0x8019e330)
 * from SLES_017.22_full_48.c. */
void cps_8019e330(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019E348u: goto block_8019E348;
            case 0x8019E350u: goto block_8019E350;
            case 0x8019E358u: goto block_8019E358;
            case 0x8019E36Cu: goto block_8019E36C;
            case 0x8019E374u: goto block_8019E374;
            case 0x8019E37Cu: goto block_8019E37C;
            case 0x8019E384u: goto block_8019E384;
            case 0x8019E38Cu: goto block_8019E38C;
            case 0x8019E360u: goto block_8019E360;
            case 0x8019E364u: goto block_8019E364;
            default: break;
        }
    }
    /* Address: 0x8019E330, Size: 108 bytes, Blocks: 11 */

block_8019E330:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);
    cpu->gpr[31] = 0x8019E348u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80199FE0u);
    cpu->pc = 0x80199FE0u; return;  /* CPS jal -> 0x80199FE0 */

block_8019E348:
    cpu->gpr[31] = 0x8019E350u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E34C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019E350:
    cpu->gpr[31] = 0x8019E358u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E354: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80199FFCu);
    cpu->pc = 0x80199FFCu; return;  /* CPS jal -> 0x80199FFC */

block_8019E358:
    int _bc_8019E358 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E35C: 0x00000000 */
    if (_bc_8019E358) {
        psx_check_interrupts_at(cpu, 0x8019E364u);
        goto block_8019E364;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8019E360u);
        goto block_8019E360;  /* not taken */
    }

block_8019E360:
    cpu->gpr[16] = cpu->gpr[0];
    /* fall through to block_8019E364 */

block_8019E364:
    cpu->gpr[31] = 0x8019E36Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x8019E400u);
    cpu->pc = 0x8019E400u; return;  /* CPS jal -> 0x8019E400 */

block_8019E36C:
    cpu->gpr[31] = 0x8019E374u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E370: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019E5CCu);
    cpu->pc = 0x8019E5CCu; return;  /* CPS jal -> 0x8019E5CC */

block_8019E374:
    cpu->gpr[31] = 0x8019E37Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E378: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019E4C8u);
    cpu->pc = 0x8019E4C8u; return;  /* CPS jal -> 0x8019E4C8 */

block_8019E37C:
    cpu->gpr[31] = 0x8019E384u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E380: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019E55Cu);
    cpu->pc = 0x8019E55Cu; return;  /* CPS jal -> 0x8019E55C */

block_8019E384:
    cpu->gpr[31] = 0x8019E38Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E388: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_8019E38C:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_8019E394 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 24;
    psx_check_interrupts_at(cpu, _jt_8019E394);
    cpu->pc = _jt_8019E394; return;  /* CPS: jr $ra */
}

/* cps_8019e39c — clean CPS transliteration of br2eu_func_8019E39C (guest 0x8019e39c)
 * from SLES_017.22_full_48.c. */
void cps_8019e39c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8019E3ACu: goto block_8019E3AC;
            case 0x8019E3B4u: goto block_8019E3B4;
            case 0x8019E3BCu: goto block_8019E3BC;
            case 0x8019E3C4u: goto block_8019E3C4;
            default: break;
        }
    }
    /* Address: 0x8019E39C, Size: 56 bytes, Blocks: 5 */

block_8019E39C:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);
    cpu->gpr[31] = 0x8019E3ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E3A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_8019E3AC:
    cpu->gpr[31] = 0x8019E3B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E3B0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019E410u);
    cpu->pc = 0x8019E410u; return;  /* CPS jal -> 0x8019E410 */

block_8019E3B4:
    cpu->gpr[31] = 0x8019E3BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80199FE0u);
    cpu->pc = 0x80199FE0u; return;  /* CPS jal -> 0x80199FE0 */

block_8019E3BC:
    cpu->gpr[31] = 0x8019E3C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E3C0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_8019E3C4:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 16);
    cpu->gpr[29] = cpu->gpr[29] + 24;
    uint32_t _jt_8019E3CC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8019E3D0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8019E3CC);
    cpu->pc = _jt_8019E3CC; return;  /* CPS: jr $ra */
}

/* cps_801a0be0 — clean CPS transliteration of br2eu_func_801A0BE0 (guest 0x801a0be0)
 * from SLES_017.22_full_49.c. */
void cps_801a0be0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A0BF0u: goto block_801A0BF0;
            case 0x801A0C10u: goto block_801A0C10;
            case 0x801A0C34u: goto block_801A0C34;
            case 0x801A0C58u: goto block_801A0C58;
            default: break;
        }
    }
    /* Address: 0x801A0BE0, Size: 136 bytes, Blocks: 5 */

block_801A0BE0:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);
    cpu->gpr[31] = 0x801A0BF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_801A0BF0:
    cpu->gpr[4] = 0xF400 << 16;  /* 0xF4000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0001;
    cpu->gpr[5] = 4;
    cpu->gpr[6] = 4096;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[7] = cpu->gpr[7] + 2832;
    cpu->gpr[31] = 0x801A0C10u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0C10:
    cpu->gpr[4] = 0xF400 << 16;  /* 0xF4000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0001;
    cpu->gpr[5] = 0x8000;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[7] = cpu->gpr[7] + 2852;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -4240, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A0C34u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 4096;
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0C34:
    cpu->gpr[4] = 0xF400 << 16;  /* 0xF4000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0001;
    cpu->gpr[5] = 256;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[7] = cpu->gpr[7] + 2872;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -4236, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A0C58u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 4096;
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0C58:
    cpu->gpr[4] = 0xF400 << 16;  /* 0xF4000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0001;
    cpu->gpr[5] = 8192;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cps_801a0c68(); return;  /* fallthrough to split piece */
}

/* cps_801a0c68 — clean CPS transliteration of br2eu_func_801A0C68 (guest 0x801a0c68)
 * from SLES_017.22_full_49.c. */
void cps_801a0c68(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A0C7Cu: goto block_801A0C7C;
            case 0x801A0CA0u: goto block_801A0CA0;
            case 0x801A0CC4u: goto block_801A0CC4;
            case 0x801A0CE8u: goto block_801A0CE8;
            case 0x801A0D0Cu: goto block_801A0D0C;
            case 0x801A0D20u: goto block_801A0D20;
            case 0x801A0D30u: goto block_801A0D30;
            case 0x801A0D40u: goto block_801A0D40;
            case 0x801A0D50u: goto block_801A0D50;
            case 0x801A0D60u: goto block_801A0D60;
            case 0x801A0D70u: goto block_801A0D70;
            case 0x801A0D80u: goto block_801A0D80;
            case 0x801A0D90u: goto block_801A0D90;
            case 0x801A0D98u: goto block_801A0D98;
            case 0x801A0DACu: goto block_801A0DAC;
            case 0x801A0DA4u: goto block_801A0DA4;
            default: break;
        }
    }
    /* Address: 0x801A0C68, Size: 340 bytes, Blocks: 17 */

block_801A0C68:
    cpu->gpr[7] = cpu->gpr[7] + 2892;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -4232, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A0C7Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 4096;
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0C7C:
    cpu->gpr[4] = 0xF000 << 16;  /* 0xF0000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0011;
    cpu->gpr[5] = 4;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[7] = cpu->gpr[7] + 2912;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -4228, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A0CA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 4096;
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0CA0:
    cpu->gpr[4] = 0xF000 << 16;  /* 0xF0000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0011;
    cpu->gpr[5] = 0x8000;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[7] = cpu->gpr[7] + 2932;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -4224, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A0CC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 4096;
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0CC4:
    cpu->gpr[4] = 0xF000 << 16;  /* 0xF0000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0011;
    cpu->gpr[5] = 256;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[7] = cpu->gpr[7] + 2952;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -4220, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A0CE8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 4096;
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0CE8:
    cpu->gpr[4] = 0xF000 << 16;  /* 0xF0000000 */
    cpu->gpr[4] = cpu->gpr[4] | 0x0011;
    cpu->gpr[5] = 8192;
    cpu->gpr[7] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[7] = cpu->gpr[7] + 2972;
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->write_word(cpu->gpr[1] + -4216, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A0D0Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = 4096;
    psx_check_interrupts_at(cpu, 0x8019B380u);
    cpu->pc = 0x8019B380u; return;  /* CPS jal -> 0x8019B380 */

block_801A0D0C:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4240);
    cpu->gpr[1] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[31] = 0x801A0D20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -4212, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D20:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4236);
    cpu->gpr[31] = 0x801A0D30u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D2C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D30:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4232);
    cpu->gpr[31] = 0x801A0D40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D3C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D40:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4228);
    cpu->gpr[31] = 0x801A0D50u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D4C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D50:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4224);
    cpu->gpr[31] = 0x801A0D60u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D5C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D60:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4220);
    cpu->gpr[31] = 0x801A0D70u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D6C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D70:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4216);
    cpu->gpr[31] = 0x801A0D80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D7C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D80:
    cpu->gpr[4] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -4212);
    cpu->gpr[31] = 0x801A0D90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D8C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8019B390u);
    cpu->pc = 0x8019B390u; return;  /* CPS jal -> 0x8019B390 */

block_801A0D90:
    cpu->gpr[31] = 0x801A0D98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0D94: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A0E90u);
    cpu->pc = 0x801A0E90u; return;  /* CPS jal -> 0x801A0E90 */

block_801A0D98:
    cpu->gpr[2] = 1;
    int _bc_801A0D9C = (cpu->gpr[16] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0DA0: 0x00000000 */
    if (_bc_801A0D9C) {
        psx_check_interrupts_at(cpu, 0x801A0DACu);
        goto block_801A0DAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A0DA4u);
        goto block_801A0DA4;  /* not taken */
    }

block_801A0DA4:
    cpu->gpr[31] = 0x801A0DACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A0DA8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_801A0DAC:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_801A0DB4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 24;
    psx_check_interrupts_at(cpu, _jt_801A0DB4);
    cpu->pc = _jt_801A0DB4; return;  /* CPS: jr $ra */
}

/* cps_801a1a40 — clean CPS transliteration of br2eu_func_801A1A40 (guest 0x801a1a40)
 * from SLES_017.22_full_49.c. */
void cps_801a1a40(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A1A58u: goto block_801A1A58;
            case 0x801A1A6Cu: goto block_801A1A6C;
            case 0x801A1A78u: goto block_801A1A78;
            case 0x801A1AA0u: goto block_801A1AA0;
            case 0x801A1AA8u: goto block_801A1AA8;
            case 0x801A1AC4u: goto block_801A1AC4;
            case 0x801A1AE0u: goto block_801A1AE0;
            default: break;
        }
    }
    /* Address: 0x801A1A40, Size: 204 bytes, Blocks: 8 */

block_801A1A40:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[31] = 0x801A1A58u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[1] + -6216, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x8018E300u);
    cpu->pc = 0x8018E300u; return;  /* CPS jal -> 0x8018E300 */

block_801A1A58:
    cpu->gpr[4] = 2;
    cpu->gpr[16] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[16] = cpu->gpr[16] + -4160;
    cpu->gpr[31] = 0x801A1A6Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x8019A340u);
    cpu->pc = 0x8019A340u; return;  /* CPS jal -> 0x8019A340 */

block_801A1A6C:
    cpu->gpr[4] = 2;
    cpu->gpr[31] = 0x801A1A78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x8019A330u);
    cpu->pc = 0x8019A330u; return;  /* CPS jal -> 0x8019A330 */

block_801A1A78:
    cpu->gpr[4] = 3;
    cpu->gpr[3] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + -6176);
    cpu->gpr[2] = -2;
    cpu->write_word(cpu->gpr[3], cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3] + 4);
    cpu->gpr[5] = cpu->gpr[0];
    cpu->gpr[2] = cpu->gpr[2] | 0x0001;
    cpu->gpr[31] = 0x801A1AA0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[3] + 4, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8019A670u);
    cpu->pc = 0x8019A670u; return;  /* CPS jal -> 0x8019A670 */

block_801A1AA0:
    cpu->gpr[31] = 0x801A1AA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A1AA4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018E310u);
    cpu->pc = 0x8018E310u; return;  /* CPS jal -> 0x8018E310 */

block_801A1AA8:
    cpu->gpr[4] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -6220);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -6268);
    /* nop */  /* 0x801A1AB8: 0x00000000 */
    uint32_t _jt_801A1ABC = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801A1AC4u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A1AC0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801A1ABC);
    cpu->pc = _jt_801A1ABC; return;  /* CPS jalr */

block_801A1AC4:
    cpu->gpr[4] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + -6220);
    cpu->gpr[2] = 0x801C << 16;  /* 0x801C0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + -6268);
    /* nop */  /* 0x801A1AD4: 0x00000000 */
    uint32_t _jt_801A1AD8 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801A1AE0u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 240;
    psx_check_interrupts_at(cpu, _jt_801A1AD8);
    cpu->pc = _jt_801A1AD8; return;  /* CPS jalr */

block_801A1AE0:
    cpu->gpr[2] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[2] = cpu->gpr[2] + -4144;
    cpu->write_word(cpu->gpr[2] + 4, cpu->gpr[0]);
    cpu->write_word(cpu->gpr[2], cpu->gpr[0]);
    cpu->gpr[2] = 1;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6216, cpu->gpr[2]);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_801A1B04 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 24;
    psx_check_interrupts_at(cpu, _jt_801A1B04);
    cpu->pc = _jt_801A1B04; return;  /* CPS: jr $ra */
}

/* cps_801a36f0 — clean CPS transliteration of br2eu_func_801A36F0 (guest 0x801a36f0)
 * from SLES_017.22_full_50.c. */
void cps_801a36f0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801A3720u: goto block_801A3720;
            case 0x801A37ACu: goto block_801A37AC;
            case 0x801A382Cu: goto block_801A382C;
            case 0x801A37D0u: goto block_801A37D0;
            case 0x801A37F8u: goto block_801A37F8;
            case 0x801A3808u: goto block_801A3808;
            case 0x801A3824u: goto block_801A3824;
            default: break;
        }
    }
    /* Address: 0x801A36F0, Size: 352 bytes, Blocks: 8 */

block_801A36F0:
    cpu->gpr[29] = cpu->gpr[29] + -32;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6216, cpu->gpr[0]);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6196, cpu->gpr[0]);
    cpu->gpr[31] = 0x801A3720u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[18] = cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x801A3D50u);
    cpu->pc = 0x801A3D50u; return;  /* CPS jal -> 0x801A3D50 */

block_801A3720:
    cpu->gpr[16] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[16] = cpu->gpr[16] + -3968;
    cpu->gpr[2] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[2] = cpu->gpr[2] + 14520;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6272, cpu->gpr[2]);
    cpu->gpr[2] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[2] = cpu->gpr[2] + 14416;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6268, cpu->gpr[2]);
    cpu->gpr[2] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[2] = cpu->gpr[2] + 14780;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6264, cpu->gpr[2]);
    cpu->gpr[2] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[2] = cpu->gpr[2] + 14968;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6260, cpu->gpr[2]);
    cpu->gpr[2] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[2] = cpu->gpr[2] + 15604;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6256, cpu->gpr[2]);
    cpu->gpr[2] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[2] = cpu->gpr[2] + 15660;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6252, cpu->gpr[2]);
    cpu->gpr[2] = 0x801A << 16;  /* 0x801A0000 */
    cpu->gpr[2] = cpu->gpr[2] + 14764;
    cpu->gpr[4] = cpu->gpr[16];
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6220, cpu->gpr[16]);
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6236, cpu->gpr[2]);
    cpu->gpr[31] = 0x801A37ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = 480;
    psx_check_interrupts_at(cpu, 0x801A09B0u);
    cpu->pc = 0x801A09B0u; return;  /* CPS jal -> 0x801A09B0 */

block_801A37AC:
    cpu->gpr[8] = cpu->gpr[0];
    cpu->gpr[9] = 255;
    cpu->gpr[4] = cpu->gpr[16] + 64;
    cpu->gpr[7] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[7] = cpu->gpr[7] + -4040;
    cpu->gpr[6] = 0x8020 << 16;  /* 0x80200000 */
    cpu->gpr[6] = cpu->gpr[6] + -4112;
    cpu->write_word(cpu->gpr[16] + 48, cpu->gpr[17]);
    cpu->write_word(cpu->gpr[16] + 288, cpu->gpr[18]);
    /* fall through to block_801A37D0 */

block_801A37D0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + -16);
    cpu->gpr[5] = cpu->gpr[16] + 93;
    cpu->write_word(cpu->gpr[4] + -52, cpu->gpr[0]);
    cpu->write_word(cpu->gpr[4] + -48, cpu->gpr[16]);
    cpu->write_byte(cpu->gpr[2], (uint8_t)cpu->gpr[9]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + -16);
    cpu->gpr[3] = 5;
    cpu->write_byte(cpu->gpr[2] + 1, (uint8_t)cpu->gpr[0]);
    cpu->write_word(cpu->gpr[4] + -4, cpu->gpr[6]);
    cpu->write_word(cpu->gpr[4], cpu->gpr[7]);
    /* fall through to block_801A37F8 */

block_801A37F8:
    cpu->write_byte(cpu->gpr[5], (uint8_t)cpu->gpr[9]);
    cpu->gpr[3] = cpu->gpr[3] + -1;
    int _bc_801A3800 = ((int32_t)cpu->gpr[3] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[5] + 1;
    if (_bc_801A3800) {
        psx_check_interrupts_at(cpu, 0x801A37F8u);
        goto block_801A37F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A3808u);
        goto block_801A3808;  /* not taken */
    }

block_801A3808:
    cpu->gpr[7] = cpu->gpr[7] + 35;
    cpu->gpr[6] = cpu->gpr[6] + 35;
    cpu->gpr[8] = cpu->gpr[8] + 1;
    cpu->gpr[4] = cpu->gpr[4] + 240;
    cpu->gpr[2] = ((int32_t)cpu->gpr[8] < 2) ? 1 : 0;  /* 0x801A3818: 0x29020002 */
    int _bc_801A381C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + 240;
    if (_bc_801A381C) {
        psx_check_interrupts_at(cpu, 0x801A37D0u);
        goto block_801A37D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801A3824u);
        goto block_801A3824;  /* not taken */
    }

block_801A3824:
    cpu->gpr[31] = 0x801A382Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801A3828: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A1808u);
    cpu->pc = 0x801A1808u; return;  /* CPS jal -> 0x801A1808 */

block_801A382C:
    cpu->gpr[2] = 1;
    cpu->gpr[1] = 0x801C << 16;  /* 0x801C0000 */
    cpu->write_word(cpu->gpr[1] + -6216, cpu->gpr[2]);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 28);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_801A3848 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 32;
    psx_check_interrupts_at(cpu, _jt_801A3848);
    cpu->pc = _jt_801A3848; return;  /* CPS: jr $ra */
}
