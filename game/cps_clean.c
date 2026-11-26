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
void cps_80138cec(void);
void cps_801393b8(void);
void cps_80139834(void);
void cps_80139954(void);
void cps_80139a0c(void);
void cps_80139d98(void);
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
void cps_80198af0(void);
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
void cps_8019a00c(void);
void cps_8019a09c(void);
void cps_8019a378(void);
void cps_8019a3f0(void);
void cps_8019a5d8(void);
void cps_8019aabc(void);
void cps_8019e330(void);
void cps_8019e39c(void);
void cps_8019fb9c(void);
void cps_801a0660(void);
void cps_801a0be0(void);
void cps_801a0c68(void);
void cps_801a1a40(void);
void cps_801a36f0(void);
void cps_801a972c(void);
void cps_801aed3c(void);
void cps_801aedbc(void);
void cps_801aef18(void);
void cps_80172a2c(void);
void cps_80175a18(void);
void cps_801796e4(void);
void cps_80179a5c(void);
void cps_80179a60(void);
void cps_8017a6e0(void);
void cps_8017a990(void);
void cps_8017a9b8(void);
void cps_8017a9c4(void);
void cps_8017a9d0(void);
void cps_8017a9fc(void);
void cps_8017aa04(void);
void cps_8017ab30(void);
void cps_8017ab5c(void);
void cps_8017ac44(void);
void cps_8017ac4c(void);
void cps_8017ad78(void);
void cps_8017adb8(void);
void cps_8017ae00(void);
void cps_8017ae10(void);
void cps_8017b5f4(void);
void cps_8017b648(void);
void cps_8017b650(void);
void cps_8017b664(void);
void cps_8017b668(void);
void cps_8017b860(void);
void cps_8017b8b4(void);
void cps_8017b8bc(void);
void cps_8017b8d0(void);
void cps_8017b8d4(void);
void cps_8017ba3c(void);
void cps_8017baac(void);
void cps_8017bab4(void);
void cps_8017bac8(void);
void cps_8017bad0(void);
void cps_8017bad4(void);
void cps_8017bd8c(void);
void cps_8017c35c(void);
void cps_8017c3b4(void);
void cps_8017c3bc(void);
void cps_8017c3d0(void);
void cps_8017c5b8(void);
void cps_8017c600(void);
void cps_8017c608(void);
void cps_8017c61c(void);
void cps_8017c7b0(void);
void cps_8017c818(void);
void cps_8017c840(void);
void cps_8017c850(void);
void cps_8017c898(void);
void cps_8017c900(void);
void cps_8017c928(void);
void cps_8017c938(void);
void cps_8017c9bc(void);
void cps_8017ca84(void);
void cps_8017cad0(void);
void cps_8017cad8(void);
void cps_8017caec(void);
void cps_8017caf4(void);
void cps_8017d1d0(void);
void cps_8017d23c(void);
void cps_8017d244(void);
void cps_8017d258(void);
void cps_8017d25c(void);
void cps_8017d354(void);
void cps_8017d55c(void);
void cps_8017d5c0(void);
void cps_8017d5c8(void);
void cps_8017d5dc(void);
void cps_8017d5e0(void);
void cps_8017d6ac(void);
void cps_8017d71c(void);
void cps_8017d724(void);
void cps_8017d72c(void);
void cps_8017d754(void);
void cps_8017d758(void);
void cps_8017d7ec(void);
void cps_8017d9dc(void);
void cps_8017da44(void);
void cps_8017da4c(void);
void cps_8017da60(void);
void cps_8017da64(void);
void cps_8017da68(void);
void cps_8017e4cc(void);
void cps_8017e4d4(void);
void cps_8017e628(void);
void cps_8017e68c(void);
void cps_8017e694(void);
void cps_8017e6a8(void);
void cps_8017e6ac(void);
void cps_8017e6b0(void);
void cps_8017f97c(void);
void cps_8017fae4(void);
void cps_8017faf8(void);
void cps_8017fb00(void);

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

/* cps_80138cec — clean CPS transliteration of br2eu_func_80138CEC (guest 0x80138cec)
 * from SLES_017.22_full_07.c. */
void cps_80138cec(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80138D6Cu: goto block_80138D6C;
            case 0x80138FC4u: goto block_80138FC4;
            case 0x80138D34u: goto block_80138D34;
            case 0x80138D54u: goto block_80138D54;
            case 0x80138D64u: goto block_80138D64;
            case 0x80138D74u: goto block_80138D74;
            case 0x80138D84u: goto block_80138D84;
            case 0x80138DA0u: goto block_80138DA0;
            case 0x80138DCCu: goto block_80138DCC;
            case 0x80138DD4u: goto block_80138DD4;
            case 0x80138DD8u: goto block_80138DD8;
            case 0x80138E04u: goto block_80138E04;
            case 0x80138E0Cu: goto block_80138E0C;
            case 0x80138E10u: goto block_80138E10;
            case 0x80138E3Cu: goto block_80138E3C;
            case 0x80138E44u: goto block_80138E44;
            case 0x80138E54u: goto block_80138E54;
            case 0x80138E70u: goto block_80138E70;
            case 0x80138E9Cu: goto block_80138E9C;
            case 0x80138EA4u: goto block_80138EA4;
            case 0x80138EA8u: goto block_80138EA8;
            case 0x80138ED4u: goto block_80138ED4;
            case 0x80138EDCu: goto block_80138EDC;
            case 0x80138EE0u: goto block_80138EE0;
            case 0x80138F0Cu: goto block_80138F0C;
            case 0x80138F14u: goto block_80138F14;
            case 0x80138F18u: goto block_80138F18;
            case 0x80138F24u: goto block_80138F24;
            case 0x80138F2Cu: goto block_80138F2C;
            case 0x80138F48u: goto block_80138F48;
            case 0x80138F8Cu: goto block_80138F8C;
            case 0x801390D0u: goto block_801390D0;
            case 0x801390D8u: goto block_801390D8;
            case 0x801390DCu: goto block_801390DC;
            case 0x8013914Cu: goto block_8013914C;
            case 0x80139154u: goto block_80139154;
            case 0x80139158u: goto block_80139158;
            case 0x80139160u: goto block_80139160;
            case 0x801391D4u: goto block_801391D4;
            case 0x801391ECu: goto block_801391EC;
            case 0x801392DCu: goto block_801392DC;
            case 0x801392E4u: goto block_801392E4;
            case 0x801392E8u: goto block_801392E8;
            case 0x80139358u: goto block_80139358;
            case 0x80139360u: goto block_80139360;
            case 0x80139364u: goto block_80139364;
            case 0x8013936Cu: goto block_8013936C;
            case 0x8013937Cu: goto block_8013937C;
            default: break;
        }
    }
    /* Address: 0x80138CEC, Size: 1740 bytes, Blocks: 49 */

block_80138CEC:
    cpu->gpr[29] = cpu->gpr[29] + -64;
    cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[19]);
    cpu->gpr[19] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[19] = cpu->gpr[19] | 0x03FC;
    cpu->write_word(cpu->gpr[29] + 52, cpu->gpr[21]);
    cpu->gpr[21] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[21] = cpu->gpr[21] | 0x03F8;
    cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[20]);
    cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[18]);
    cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[16]);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 100);
    /* nop */  /* 0x80138D24: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)19) ? 1 : 0;  /* 0x80138D28: 0x2C620013 */
    int _bc_80138D2C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[20] = 0x1F80 << 16;  /* 0x1F800000 */
    if (_bc_80138D2C) {
        psx_check_interrupts_at(cpu, 0x80138F24u);
        goto block_80138F24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D34u);
        goto block_80138D34;  /* not taken */
    }

block_80138D34:
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    cpu->gpr[2] = cpu->gpr[2] + 104;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x80138D48: 0x00000000 */
    uint32_t _jt_80138D4C = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D50: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80138D4C);
    cpu->pc = _jt_80138D4C; return;  /* CPS: jr cpu->gpr[2] */

block_80138D54:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[17] + 80);
    /* nop */  /* 0x80138D58: 0x00000000 */
    int _bc_80138D5C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D60: 0x00000000 */
    if (_bc_80138D5C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* not taken */
    }

block_80138D64:
    cpu->gpr[31] = 0x80138D6Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D68: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801A482Cu);
    cpu->pc = 0x801A482Cu; return;  /* CPS jal -> 0x801A482C */

block_80138D6C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D70: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x80139DE8u);
    cpu->pc = 0x80139DE8u; return;  /* CPS j: split */

block_80138D74:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 130);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 96);
    int _bc_80138D7C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D80: 0x00000000 */
    if (_bc_80138D7C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138D84u);
        goto block_80138D84;  /* not taken */
    }

block_80138D84:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3]);
    /* nop */  /* 0x80138D88: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + -1;
    cpu->write_half(cpu->gpr[3], (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    int _bc_80138D98 = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138D9C: 0x00000000 */
    if (_bc_80138D98) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DA0u);
        goto block_80138DA0;  /* not taken */
    }

block_80138DA0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138DA4: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[17] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[3] + 4);
    /* nop */  /* 0x80138DBC: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80138DC0: 0x2C820008 */
    int _bc_80138DC4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + -8;
    if (_bc_80138DC4) {
        psx_check_interrupts_at(cpu, 0x80138DD4u);
        goto block_80138DD4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138DCCu);
        goto block_80138DCC;  /* not taken */
    }

block_80138DCC:
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3] + 4, (uint8_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80138DD8u);
    goto block_80138DD8;  /* j */

block_80138DD4:
    cpu->write_byte(cpu->gpr[3] + 4, (uint8_t)cpu->gpr[2]);
    /* fall through to block_80138DD8 */

block_80138DD8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138DDC: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[17] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[3] + 5);
    /* nop */  /* 0x80138DF4: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80138DF8: 0x2C820008 */
    int _bc_80138DFC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + -8;
    if (_bc_80138DFC) {
        psx_check_interrupts_at(cpu, 0x80138E0Cu);
        goto block_80138E0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E04u);
        goto block_80138E04;  /* not taken */
    }

block_80138E04:
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3] + 5, (uint8_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80138E10u);
    goto block_80138E10;  /* j */

block_80138E0C:
    cpu->write_byte(cpu->gpr[3] + 5, (uint8_t)cpu->gpr[2]);
    /* fall through to block_80138E10 */

block_80138E10:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138E14: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[17] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[3] + 6);
    /* nop */  /* 0x80138E2C: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80138E30: 0x2C820008 */
    int _bc_80138E34 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + -8;
    if (_bc_80138E34) {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
        goto block_80138F0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E3Cu);
        goto block_80138E3C;  /* not taken */
    }

block_80138E3C:
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3] + 6, (uint8_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80138F18u);
    goto block_80138F18;  /* j */

block_80138E44:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 130);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 96);
    int _bc_80138E4C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E50: 0x00000000 */
    if (_bc_80138E4C) {
        psx_check_interrupts_at(cpu, 0x80138F18u);
        goto block_80138F18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E54u);
        goto block_80138E54;  /* not taken */
    }

block_80138E54:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[3] + 8);
    /* nop */  /* 0x80138E58: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + -1;
    cpu->write_half(cpu->gpr[3] + 8, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    int _bc_80138E68 = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138E6C: 0x00000000 */
    if (_bc_80138E68) {
        psx_check_interrupts_at(cpu, 0x80138D64u);
        goto block_80138D64;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E70u);
        goto block_80138E70;  /* not taken */
    }

block_80138E70:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138E74: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[17] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[3] + 4);
    /* nop */  /* 0x80138E8C: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80138E90: 0x2C820008 */
    int _bc_80138E94 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + -8;
    if (_bc_80138E94) {
        psx_check_interrupts_at(cpu, 0x80138EA4u);
        goto block_80138EA4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138E9Cu);
        goto block_80138E9C;  /* not taken */
    }

block_80138E9C:
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3] + 4, (uint8_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80138EA8u);
    goto block_80138EA8;  /* j */

block_80138EA4:
    cpu->write_byte(cpu->gpr[3] + 4, (uint8_t)cpu->gpr[2]);
    /* fall through to block_80138EA8 */

block_80138EA8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138EAC: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[17] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[3] + 5);
    /* nop */  /* 0x80138EC4: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80138EC8: 0x2C820008 */
    int _bc_80138ECC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + -8;
    if (_bc_80138ECC) {
        psx_check_interrupts_at(cpu, 0x80138EDCu);
        goto block_80138EDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138ED4u);
        goto block_80138ED4;  /* not taken */
    }

block_80138ED4:
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3] + 5, (uint8_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80138EE0u);
    goto block_80138EE0;  /* j */

block_80138EDC:
    cpu->write_byte(cpu->gpr[3] + 5, (uint8_t)cpu->gpr[2]);
    /* fall through to block_80138EE0 */

block_80138EE0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138EE4: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[17] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[3] + 6);
    /* nop */  /* 0x80138EFC: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)8) ? 1 : 0;  /* 0x80138F00: 0x2C820008 */
    int _bc_80138F04 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] + -8;
    if (_bc_80138F04) {
        psx_check_interrupts_at(cpu, 0x80138F14u);
        goto block_80138F14;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F0Cu);
        goto block_80138F0C;  /* not taken */
    }

block_80138F0C:
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[3] + 6, (uint8_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80138F18u);
    goto block_80138F18;  /* j */

block_80138F14:
    cpu->write_byte(cpu->gpr[3] + 6, (uint8_t)cpu->gpr[2]);
    /* fall through to block_80138F18 */

block_80138F18:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 100);
    /* nop */  /* 0x80138F1C: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)19) ? 1 : 0;  /* 0x80138F20: 0x2C620013 */
    /* fall through to block_80138F24 */

block_80138F24:
    int _bc_80138F24 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_80138F24) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F2Cu);
        goto block_80138F2C;  /* not taken */
    }

block_80138F2C:
    cpu->gpr[2] = cpu->gpr[2] + 184;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x80138F3C: 0x00000000 */
    uint32_t _jt_80138F40 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F44: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80138F40);
    cpu->pc = _jt_80138F40; return;  /* CPS: jr cpu->gpr[2] */

block_80138F48:
    cpu->gpr[2] = cpu->gpr[17] + 88;
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((cpu->gpr[2])));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((cpu->gpr[2] + 4))));
    /* nop */  /* 0x80138F54: 0x00000000 */
    /* nop */  /* 0x80138F58: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80138F5C: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[19]), (cpu->gte_data[14]));
    gte_precision_store_word((cpu->gpr[19]), 14);
    
    psx_gte_read(cpu, 12);
    cpu->gpr[12] = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80138F68: 0x00000000 */
    cpu->gpr[12] = (int32_t)cpu->gpr[12] >> 2;
    cpu->write_word(cpu->gpr[21], cpu->gpr[12]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[21]);
    /* nop */  /* 0x80138F78: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[4] + -4;
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)2059) ? 1 : 0;  /* 0x80138F80: 0x2C42080B */
    int _bc_80138F84 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80138F88: 0x00000000 */
    if (_bc_80138F84) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80138F8Cu);
        goto block_80138F8C;  /* not taken */
    }

block_80138F8C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 36);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 84);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[4] << 2;
    /* nop */  /* 0x80138FA8: 0x00000000 */
    if (cpu->gpr[2] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[3] / (int32_t)cpu->gpr[2]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[3] % (int32_t)cpu->gpr[2]); } else { cpu->lo = ((int32_t)cpu->gpr[3] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[3]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[16] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2] + 664);
    cpu->gpr[31] = 0x80138FC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 160;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80138FC4:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138FC8: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[4] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[17];
    cpu->gpr[4] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[5] = (uint32_t)cpu->gpr[16] >> 31;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[16] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[16]));
    cpu->gpr[5] = cpu->gpr[16] + cpu->gpr[5];
    cpu->gpr[5] = (int32_t)cpu->gpr[5] >> 1;
    cpu->gpr[4] = cpu->gpr[4] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[3] + 8, (uint16_t)cpu->gpr[4]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80138FFC: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[4] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[17];
    cpu->gpr[4] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[7] = (int32_t)cpu->gpr[2] >> 12;
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 31;
    cpu->gpr[2] = cpu->gpr[7] + cpu->gpr[2];
    cpu->gpr[6] = (int32_t)cpu->gpr[2] >> 1;
    cpu->gpr[4] = cpu->gpr[4] - cpu->gpr[6];
    cpu->write_half(cpu->gpr[3] + 10, (uint16_t)cpu->gpr[4]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139034: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[16];
    cpu->write_half(cpu->gpr[2] + 16, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x8013905C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[6];
    cpu->write_half(cpu->gpr[2] + 18, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139080: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 24, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x801390A4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[4] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_801390C8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[6];
    if (_bc_801390C8) {
        psx_check_interrupts_at(cpu, 0x801390D8u);
        goto block_801390D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801390D0u);
        goto block_801390D0;  /* not taken */
    }

block_801390D0:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[6];
    psx_check_interrupts_at(cpu, 0x801390DCu);
    goto block_801390DC;  /* j */

block_801390D8:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[7];
    /* fall through to block_801390DC */

block_801390DC:
    cpu->write_half(cpu->gpr[4] + 26, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[4] = cpu->gpr[2] << 2;
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[4] = cpu->gpr[4] << 3;
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[17];
    cpu->gpr[2] = (uint32_t)cpu->gpr[16] >> 31;
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[16];
    cpu->write_half(cpu->gpr[4] + 32, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139114: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[5] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[2] = (uint32_t)cpu->gpr[7] >> 31;
    cpu->gpr[2] = cpu->gpr[7] + cpu->gpr[2];
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 1;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_80139144 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[4];
    if (_bc_80139144) {
        psx_check_interrupts_at(cpu, 0x80139154u);
        goto block_80139154;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013914Cu);
        goto block_8013914C;  /* not taken */
    }

block_8013914C:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[4];
    psx_check_interrupts_at(cpu, 0x80139158u);
    goto block_80139158;  /* j */

block_80139154:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[7];
    /* fall through to block_80139158 */

block_80139158:
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 34, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80139A0Cu);
    cpu->pc = 0x80139A0Cu; return;  /* CPS j: split */

block_80139160:
    cpu->gpr[2] = cpu->gpr[17] + 88;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 96);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((cpu->gpr[2])));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((cpu->gpr[2] + 4))));
    /* nop */  /* 0x80139170: 0x00000000 */
    /* nop */  /* 0x80139174: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139178: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[19]), (cpu->gte_data[14]));
    gte_precision_store_word((cpu->gpr[19]), 14);
    
    psx_gte_read(cpu, 12);
    cpu->gpr[12] = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139184: 0x00000000 */
    cpu->gpr[12] = (int32_t)cpu->gpr[12] >> 2;
    cpu->write_word(cpu->gpr[21], cpu->gpr[12]);
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((cpu->gpr[3])));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((cpu->gpr[3] + 4))));
    /* nop */  /* 0x80139198: 0x00000000 */
    /* nop */  /* 0x8013919C: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x801391A0: 0x4A180001 */
    cpu->gpr[2] = cpu->gpr[29] + 24;
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[2]), (cpu->gte_data[14]));
    gte_precision_store_word((cpu->gpr[2]), 14);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[19]);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 24);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[6] = cpu->gpr[4] - cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[29] + 26);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[19]);
    int _bc_801391CC = ((int32_t)cpu->gpr[6] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[2] - cpu->gpr[3];
    if (_bc_801391CC) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x801391D4u);
        goto block_801391D4;  /* not taken */
    }

block_801391D4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[21]);
    /* nop */  /* 0x801391D8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + -4;
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)2059) ? 1 : 0;  /* 0x801391E0: 0x2C42080B */
    int _bc_801391E4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = (uint32_t)cpu->gpr[6] >> 31;
    if (_bc_801391E4) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x801391ECu);
        goto block_801391EC;  /* not taken */
    }

block_801391EC:
    cpu->gpr[5] = cpu->gpr[6] + cpu->gpr[5];
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[5] = (int32_t)cpu->gpr[5] >> 1;
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[4] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 8, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = (uint32_t)cpu->gpr[8] >> 31;
    cpu->gpr[3] = cpu->gpr[8] + cpu->gpr[3];
    cpu->gpr[7] = (int32_t)cpu->gpr[3] >> 1;
    cpu->gpr[4] = cpu->gpr[4] - cpu->gpr[7];
    cpu->write_half(cpu->gpr[2] + 10, (uint16_t)cpu->gpr[4]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139240: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[6];
    cpu->write_half(cpu->gpr[2] + 16, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139268: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[7];
    cpu->write_half(cpu->gpr[2] + 18, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x8013928C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 24, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x801392B0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[4] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_801392D4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[7];
    if (_bc_801392D4) {
        psx_check_interrupts_at(cpu, 0x801392E4u);
        goto block_801392E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801392DCu);
        goto block_801392DC;  /* not taken */
    }

block_801392DC:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[7];
    psx_check_interrupts_at(cpu, 0x801392E8u);
    goto block_801392E8;  /* j */

block_801392E4:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[8];
    /* fall through to block_801392E8 */

block_801392E8:
    cpu->write_half(cpu->gpr[4] + 26, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[4] = cpu->gpr[2] << 2;
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[4] = cpu->gpr[4] << 3;
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[17];
    cpu->gpr[2] = (uint32_t)cpu->gpr[6] >> 31;
    cpu->gpr[2] = cpu->gpr[6] + cpu->gpr[2];
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[6];
    cpu->write_half(cpu->gpr[4] + 32, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139320: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[5] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[2] = (uint32_t)cpu->gpr[8] >> 31;
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[2];
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 1;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_80139350 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[4];
    if (_bc_80139350) {
        psx_check_interrupts_at(cpu, 0x80139360u);
        goto block_80139360;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139358u);
        goto block_80139358;  /* not taken */
    }

block_80139358:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[4];
    psx_check_interrupts_at(cpu, 0x80139364u);
    goto block_80139364;  /* j */

block_80139360:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[8];
    /* fall through to block_80139364 */

block_80139364:
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 34, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80139A0Cu);
    cpu->pc = 0x80139A0Cu; return;  /* CPS j: split */

block_8013936C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 130);
    /* nop */  /* 0x80139370: 0x00000000 */
    int _bc_80139374 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[17] + 88;
    if (_bc_80139374) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        cpu->pc = 0x80139834u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8013937Cu);
        goto block_8013937C;  /* not taken */
    }

block_8013937C:
    cpu->gpr[5] = cpu->read_word(cpu->gpr[17] + 96);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 88);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[5]);
    /* nop */  /* 0x80139388: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 88, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 90);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[5] + 2);
    /* nop */  /* 0x8013939C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 90, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 92);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[5] + 4);
    /* nop */  /* 0x801393B0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cps_801393b8(); return;  /* fallthrough to split piece */
}

/* cps_801393b8 — clean CPS transliteration of br2eu_func_801393B8 (guest 0x801393b8)
 * from SLES_017.22_full_08.c. */
void cps_801393b8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8013946Cu: goto block_8013946C;
            case 0x801393CCu: goto block_801393CC;
            case 0x801393D0u: goto block_801393D0;
            case 0x801393E8u: goto block_801393E8;
            case 0x801393ECu: goto block_801393EC;
            case 0x80139404u: goto block_80139404;
            case 0x80139408u: goto block_80139408;
            case 0x80139424u: goto block_80139424;
            case 0x80139434u: goto block_80139434;
            case 0x80139450u: goto block_80139450;
            case 0x801394B8u: goto block_801394B8;
            case 0x801394BCu: goto block_801394BC;
            case 0x801394D4u: goto block_801394D4;
            case 0x801394D8u: goto block_801394D8;
            case 0x801394F0u: goto block_801394F0;
            case 0x801394F4u: goto block_801394F4;
            case 0x80139504u: goto block_80139504;
            case 0x80139514u: goto block_80139514;
            case 0x80139560u: goto block_80139560;
            case 0x80139570u: goto block_80139570;
            case 0x801395D4u: goto block_801395D4;
            case 0x801395D8u: goto block_801395D8;
            case 0x801395E8u: goto block_801395E8;
            case 0x80139608u: goto block_80139608;
            case 0x8013960Cu: goto block_8013960C;
            case 0x80139620u: goto block_80139620;
            case 0x801397ACu: goto block_801397AC;
            case 0x801397B4u: goto block_801397B4;
            case 0x801397B8u: goto block_801397B8;
            case 0x8013981Cu: goto block_8013981C;
            case 0x80139824u: goto block_80139824;
            case 0x80139828u: goto block_80139828;
            case 0x80139830u: goto block_80139830;
            default: break;
        }
    }
    /* Address: 0x801393B8, Size: 1148 bytes, Blocks: 34 */

block_801393B8:
    cpu->write_half(cpu->gpr[17] + 92, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5]);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[5]);
    int _bc_801393C4 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801393C8: 0x00000000 */
    if (_bc_801393C4) {
        psx_check_interrupts_at(cpu, 0x801393D0u);
        goto block_801393D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801393CCu);
        goto block_801393CC;  /* not taken */
    }

block_801393CC:
    cpu->gpr[2] = cpu->gpr[2] + 7;
    /* fall through to block_801393D0 */

block_801393D0:
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 3;
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 2);
    cpu->gpr[6] = cpu->read_half(cpu->gpr[5] + 2);
    cpu->gpr[2] = cpu->gpr[3] - cpu->gpr[2];
    int _bc_801393E0 = ((int32_t)cpu->gpr[4] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[2]);
    if (_bc_801393E0) {
        psx_check_interrupts_at(cpu, 0x801393ECu);
        goto block_801393EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801393E8u);
        goto block_801393E8;  /* not taken */
    }

block_801393E8:
    cpu->gpr[4] = cpu->gpr[4] + 7;
    /* fall through to block_801393EC */

block_801393EC:
    cpu->gpr[2] = (int32_t)cpu->gpr[4] >> 3;
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 4);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[5] + 4);
    cpu->gpr[2] = cpu->gpr[6] - cpu->gpr[2];
    int _bc_801393FC = ((int32_t)cpu->gpr[3] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 2, (uint16_t)cpu->gpr[2]);
    if (_bc_801393FC) {
        psx_check_interrupts_at(cpu, 0x80139408u);
        goto block_80139408;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139404u);
        goto block_80139404;  /* not taken */
    }

block_80139404:
    cpu->gpr[3] = cpu->gpr[3] + 7;
    /* fall through to block_80139408 */

block_80139408:
    cpu->gpr[2] = (int32_t)cpu->gpr[3] >> 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[5] + 2);
    cpu->gpr[2] = cpu->gpr[4] - cpu->gpr[2];
    cpu->write_half(cpu->gpr[5] + 4, (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = cpu->gpr[3] + 1;
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 2, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139424:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 130);
    /* nop */  /* 0x80139428: 0x00000000 */
    int _bc_8013942C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[17] + 88;
    if (_bc_8013942C) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        cpu->pc = 0x80139834u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80139434u);
        goto block_80139434;  /* not taken */
    }

block_80139434:
    cpu->gpr[16] = cpu->read_word(cpu->gpr[17] + 96);
    /* nop */  /* 0x80139438: 0x00000000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 12);
    cpu->gpr[3] = 3;
    cpu->gpr[2] = cpu->gpr[2] + 1;
    int _bc_80139448 = (cpu->gpr[2] != cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16] + 12, cpu->gpr[2]);
    if (_bc_80139448) {
        psx_check_interrupts_at(cpu, 0x8013946Cu);
        goto block_8013946C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139450u);
        goto block_80139450;  /* not taken */
    }

block_80139450:
    cpu->gpr[4] = cpu->gpr[17] + 88;
    cpu->gpr[5] = 15;
    cpu->gpr[6] = cpu->gpr[0];
    cpu->gpr[7] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[16] + 12, cpu->gpr[0]);
    cpu->gpr[31] = 0x8013946Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x80139FD0u);
    cpu->pc = 0x80139FD0u; return;  /* CPS jal -> 0x80139FD0 */

block_8013946C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 88);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16]);
    /* nop */  /* 0x80139474: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 88, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 90);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 2);
    /* nop */  /* 0x80139488: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 90, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 92);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 4);
    /* nop */  /* 0x8013949C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 92, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16]);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16]);
    int _bc_801394B0 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801394B4: 0x00000000 */
    if (_bc_801394B0) {
        psx_check_interrupts_at(cpu, 0x801394BCu);
        goto block_801394BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394B8u);
        goto block_801394B8;  /* not taken */
    }

block_801394B8:
    cpu->gpr[2] = cpu->gpr[2] + 3;
    /* fall through to block_801394BC */

block_801394BC:
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 2;
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 2);
    cpu->gpr[5] = cpu->read_half(cpu->gpr[16] + 2);
    cpu->gpr[2] = cpu->gpr[3] - cpu->gpr[2];
    int _bc_801394CC = ((int32_t)cpu->gpr[4] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16], (uint16_t)cpu->gpr[2]);
    if (_bc_801394CC) {
        psx_check_interrupts_at(cpu, 0x801394D8u);
        goto block_801394D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394D4u);
        goto block_801394D4;  /* not taken */
    }

block_801394D4:
    cpu->gpr[4] = cpu->gpr[4] + 3;
    /* fall through to block_801394D8 */

block_801394D8:
    cpu->gpr[2] = (int32_t)cpu->gpr[4] >> 2;
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 4);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[16] + 4);
    cpu->gpr[2] = cpu->gpr[5] - cpu->gpr[2];
    int _bc_801394E8 = ((int32_t)cpu->gpr[3] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16] + 2, (uint16_t)cpu->gpr[2]);
    if (_bc_801394E8) {
        psx_check_interrupts_at(cpu, 0x801394F4u);
        goto block_801394F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801394F0u);
        goto block_801394F0;  /* not taken */
    }

block_801394F0:
    cpu->gpr[3] = cpu->gpr[3] + 3;
    /* fall through to block_801394F4 */

block_801394F4:
    cpu->gpr[2] = (int32_t)cpu->gpr[3] >> 2;
    cpu->gpr[2] = cpu->gpr[4] - cpu->gpr[2];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16] + 4, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139504:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 130);
    /* nop */  /* 0x80139508: 0x00000000 */
    int _bc_8013950C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[17] + 88;
    if (_bc_8013950C) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        cpu->pc = 0x80139834u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80139514u);
        goto block_80139514;  /* not taken */
    }

block_80139514:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[17] + 96);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 88);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4]);
    /* nop */  /* 0x80139520: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 88, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 90);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4] + 2);
    /* nop */  /* 0x80139534: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[17] + 92);
    cpu->write_half(cpu->gpr[17] + 90, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[17] + 84);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[4] + 4);
    cpu->gpr[2] = cpu->gpr[2] + 16;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->write_word(cpu->gpr[17] + 84, cpu->gpr[2]);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[17] + 92, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139560:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 130);
    /* nop */  /* 0x80139564: 0x00000000 */
    int _bc_80139568 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[17] + 88;
    if (_bc_80139568) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        cpu->pc = 0x80139834u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80139570u);
        goto block_80139570;  /* not taken */
    }

block_80139570:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[17] + 96);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 88);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4]);
    /* nop */  /* 0x8013957C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 88, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 90);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4] + 2);
    /* nop */  /* 0x80139590: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 90, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17] + 92);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4] + 4);
    /* nop */  /* 0x801395A4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[17] + 92, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 2);
    /* nop */  /* 0x801395B4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 2;
    cpu->write_half(cpu->gpr[4] + 2, (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 84);
    /* nop */  /* 0x801395C4: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 51) ? 1 : 0;  /* 0x801395C8: 0x28620033 */
    int _bc_801395CC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + -1;
    if (_bc_801395CC) {
        psx_check_interrupts_at(cpu, 0x801395D8u);
        goto block_801395D8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801395D4u);
        goto block_801395D4;  /* not taken */
    }

block_801395D4:
    cpu->write_word(cpu->gpr[17] + 84, cpu->gpr[2]);
    /* fall through to block_801395D8 */

block_801395D8:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[17] + 90);
    /* nop */  /* 0x801395DC: 0x00000000 */
    int _bc_801395E0 = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[17] + 88;
    if (_bc_801395E0) {
        psx_check_interrupts_at(cpu, 0x80139834u);
        cpu->pc = 0x80139834u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x801395E8u);
        goto block_801395E8;  /* not taken */
    }

block_801395E8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 2);
    /* nop */  /* 0x801395EC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 31;
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    int _bc_80139600 = ((int32_t)cpu->gpr[3] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 1;
    if (_bc_80139600) {
        psx_check_interrupts_at(cpu, 0x8013960Cu);
        goto block_8013960C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139608u);
        goto block_80139608;  /* not taken */
    }

block_80139608:
    cpu->gpr[3] = cpu->gpr[3] + 3;
    /* fall through to block_8013960C */

block_8013960C:
    cpu->gpr[2] = (int32_t)cpu->gpr[3] >> 2;
    cpu->gpr[2] = cpu->gpr[5] + cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 2, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80139830u);
    goto block_80139830;  /* j */

block_80139620:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 96);
    /* nop */  /* 0x80139624: 0x00000000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8013962C: 0x00000000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2]);
    /* nop */  /* 0x80139634: 0x00000000 */
    cpu->write_half(cpu->gpr[17] + 88, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x80139640: 0x00000000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2] + 4);
    /* nop */  /* 0x80139648: 0x00000000 */
    cpu->write_half(cpu->gpr[17] + 90, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x80139654: 0x00000000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2] + 8);
    /* nop */  /* 0x8013965C: 0x00000000 */
    cpu->write_half(cpu->gpr[17] + 92, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[17] + 88;
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((cpu->gpr[2])));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((cpu->gpr[2] + 4))));
    /* nop */  /* 0x80139670: 0x00000000 */
    /* nop */  /* 0x80139674: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139678: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[19]), (cpu->gte_data[14]));
    gte_precision_store_word((cpu->gpr[19]), 14);
    
    psx_gte_read(cpu, 12);
    cpu->gpr[12] = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139684: 0x00000000 */
    cpu->gpr[12] = (int32_t)cpu->gpr[12] >> 2;
    cpu->write_word(cpu->gpr[21], cpu->gpr[12]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 36);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 84);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[2] = cpu->read_word(cpu->gpr[21]);
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    /* nop */  /* 0x801396B0: 0x00000000 */
    if (cpu->gpr[2] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[3] / (int32_t)cpu->gpr[2]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[3] % (int32_t)cpu->gpr[2]); } else { cpu->lo = ((int32_t)cpu->gpr[3] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[3]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[16] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = (uint32_t)cpu->gpr[16] >> 31;
    cpu->gpr[3] = cpu->gpr[16] + cpu->gpr[3];
    cpu->gpr[5] = (int32_t)cpu->gpr[3] >> 1;
    cpu->gpr[4] = cpu->gpr[4] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 8, (uint16_t)cpu->gpr[4]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x801396EC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 10, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139710: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[16];
    cpu->write_half(cpu->gpr[2] + 16, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139738: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 18, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x8013975C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 24, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139780: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[4] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_801397A4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    if (_bc_801397A4) {
        psx_check_interrupts_at(cpu, 0x801397B4u);
        goto block_801397B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801397ACu);
        goto block_801397AC;  /* not taken */
    }

block_801397AC:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x801397B8u);
    goto block_801397B8;  /* j */

block_801397B4:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[16];
    /* fall through to block_801397B8 */

block_801397B8:
    cpu->gpr[3] = (uint32_t)cpu->gpr[16] >> 31;
    cpu->gpr[3] = cpu->gpr[16] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[4] + 26, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[5] = (int32_t)cpu->gpr[3] >> 1;
    cpu->gpr[4] = cpu->gpr[2] << 2;
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[4] = cpu->gpr[4] << 3;
    cpu->gpr[2] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[17];
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[5];
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[16];
    cpu->write_half(cpu->gpr[4] + 32, (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x801397F0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[4] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_80139814 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    if (_bc_80139814) {
        psx_check_interrupts_at(cpu, 0x80139824u);
        goto block_80139824;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013981Cu);
        goto block_8013981C;  /* not taken */
    }

block_8013981C:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x80139828u);
    goto block_80139828;  /* j */

block_80139824:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[16];
    /* fall through to block_80139828 */

block_80139828:
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 34, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80139D98u);
    cpu->pc = 0x80139D98u; return;  /* CPS j: split */

block_80139830:
    cpu->gpr[2] = cpu->gpr[17] + 88;
    cps_80139834(); return;  /* fallthrough to split piece */
}

/* cps_80139834 — clean CPS transliteration of br2eu_func_80139834 (guest 0x80139834)
 * from SLES_017.22_full_08.c. */
void cps_80139834(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80139874u: goto block_80139874;
            default: break;
        }
    }
    /* Address: 0x80139834, Size: 288 bytes, Blocks: 2 */

block_80139834:
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((cpu->gpr[2])));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((cpu->gpr[2] + 4))));
    /* nop */  /* 0x8013983C: 0x00000000 */
    /* nop */  /* 0x80139840: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139844: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[19]), (cpu->gte_data[14]));
    gte_precision_store_word((cpu->gpr[19]), 14);
    
    psx_gte_read(cpu, 12);
    cpu->gpr[12] = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139850: 0x00000000 */
    cpu->gpr[12] = (int32_t)cpu->gpr[12] >> 2;
    cpu->write_word(cpu->gpr[21], cpu->gpr[12]);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[21]);
    /* nop */  /* 0x80139860: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[4] + -4;
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)2059) ? 1 : 0;  /* 0x80139868: 0x2C42080B */
    int _bc_8013986C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139870: 0x00000000 */
    if (_bc_8013986C) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80139874u);
        goto block_80139874;  /* not taken */
    }

block_80139874:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 36);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 84);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[3] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[4] << 2;
    /* nop */  /* 0x80139890: 0x00000000 */
    if (cpu->gpr[2] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[3] / (int32_t)cpu->gpr[2]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[3] % (int32_t)cpu->gpr[2]); } else { cpu->lo = ((int32_t)cpu->gpr[3] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[3]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[16] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = (uint32_t)cpu->gpr[16] >> 31;
    cpu->gpr[3] = cpu->gpr[16] + cpu->gpr[3];
    cpu->gpr[5] = (int32_t)cpu->gpr[3] >> 1;
    cpu->gpr[4] = cpu->gpr[4] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 8, (uint16_t)cpu->gpr[4]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x801398CC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 10, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x801398F0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[16];
    cpu->write_half(cpu->gpr[2] + 16, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139918: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 18, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x8013993C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cps_80139954(); return;  /* fallthrough to split piece */
}

/* cps_80139954 — clean CPS transliteration of br2eu_func_80139954 (guest 0x80139954)
 * from SLES_017.22_full_08.c. */
void cps_80139954(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8013998Cu: goto block_8013998C;
            case 0x80139994u: goto block_80139994;
            case 0x80139998u: goto block_80139998;
            case 0x801399FCu: goto block_801399FC;
            case 0x80139A04u: goto block_80139A04;
            case 0x80139A08u: goto block_80139A08;
            default: break;
        }
    }
    /* Address: 0x80139954, Size: 184 bytes, Blocks: 7 */

block_80139954:
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 24, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139960: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[4] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_80139984 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    if (_bc_80139984) {
        psx_check_interrupts_at(cpu, 0x80139994u);
        goto block_80139994;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8013998Cu);
        goto block_8013998C;  /* not taken */
    }

block_8013998C:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x80139998u);
    goto block_80139998;  /* j */

block_80139994:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[16];
    /* fall through to block_80139998 */

block_80139998:
    cpu->gpr[3] = (uint32_t)cpu->gpr[16] >> 31;
    cpu->gpr[3] = cpu->gpr[16] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[4] + 26, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[5] = (int32_t)cpu->gpr[3] >> 1;
    cpu->gpr[4] = cpu->gpr[2] << 2;
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[4] = cpu->gpr[4] << 3;
    cpu->gpr[2] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[17];
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[5];
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[16];
    cpu->write_half(cpu->gpr[4] + 32, (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x801399D0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[4] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    int _bc_801399F4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[5];
    if (_bc_801399F4) {
        psx_check_interrupts_at(cpu, 0x80139A04u);
        goto block_80139A04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801399FCu);
        goto block_801399FC;  /* not taken */
    }

block_801399FC:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[5];
    psx_check_interrupts_at(cpu, 0x80139A08u);
    goto block_80139A08;  /* j */

block_80139A04:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[16];
    /* fall through to block_80139A08 */

block_80139A08:
    cpu->write_half(cpu->gpr[4] + 34, (uint16_t)cpu->gpr[2]);
    cps_80139a0c(); return;  /* fallthrough to split piece */
}

/* cps_80139a0c — clean CPS transliteration of br2eu_func_80139A0C (guest 0x80139a0c)
 * from SLES_017.22_full_08.c. */
void cps_80139a0c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80139BC4u: goto block_80139BC4;
            case 0x80139C00u: goto block_80139C00;
            case 0x80139C3Cu: goto block_80139C3C;
            case 0x80139C78u: goto block_80139C78;
            case 0x80139CB4u: goto block_80139CB4;
            case 0x80139CF0u: goto block_80139CF0;
            case 0x80139D2Cu: goto block_80139D2C;
            case 0x80139D68u: goto block_80139D68;
            case 0x80139A74u: goto block_80139A74;
            case 0x80139AB8u: goto block_80139AB8;
            case 0x80139AC8u: goto block_80139AC8;
            case 0x80139AECu: goto block_80139AEC;
            case 0x80139AF0u: goto block_80139AF0;
            case 0x80139B78u: goto block_80139B78;
            case 0x80139BACu: goto block_80139BAC;
            case 0x80139BB8u: goto block_80139BB8;
            default: break;
        }
    }
    /* Address: 0x80139A0C, Size: 908 bytes, Blocks: 17 */

block_80139A0C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[20] + 8);
    cpu->gpr[3] = cpu->gpr[2] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] << 3;
    cpu->gpr[2] = cpu->read_word(cpu->gpr[21]);
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[17];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[4] = 0x0900 << 16;  /* 0x09000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    cpu->write_word(cpu->gpr[3], cpu->gpr[2]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[21]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[20] + 8);
    cpu->gpr[4] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[4] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[2] << 8;
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 8;
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[3], cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80139DE8u);
    cpu->pc = 0x80139DE8u; return;  /* CPS j: split */

block_80139A74:
    cpu->gpr[2] = cpu->gpr[17] + 88;
    
    psx_gte_stall(cpu);
    cpu->gte_data[0] = (cpu->read_word((cpu->gpr[2])));
    
    psx_gte_stall(cpu);
    gte_write_data(cpu, 1, (cpu->read_word((cpu->gpr[2] + 4))));
    /* nop */  /* 0x80139A80: 0x00000000 */
    /* nop */  /* 0x80139A84: 0x00000000 */
    gte_execute(cpu, 0x0180001);  /* gte cmd 0x01 */  /* 0x80139A88: 0x4A180001 */
    
    psx_gte_stall(cpu);
    cpu->write_word((cpu->gpr[19]), (cpu->gte_data[14]));
    gte_precision_store_word((cpu->gpr[19]), 14);
    
    psx_gte_read(cpu, 12);
    cpu->gpr[12] = gte_read_data(cpu, 19);  /* mfc2 */
    /* nop */  /* 0x80139A94: 0x00000000 */
    cpu->gpr[12] = (int32_t)cpu->gpr[12] >> 2;
    cpu->write_word(cpu->gpr[21], cpu->gpr[12]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[21]);
    /* nop */  /* 0x80139AA4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + -4;
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)2059) ? 1 : 0;  /* 0x80139AAC: 0x2C42080B */
    int _bc_80139AB0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139AB4: 0x00000000 */
    if (_bc_80139AB0) {
        psx_check_interrupts_at(cpu, 0x80139DE8u);
        cpu->pc = 0x80139DE8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AB8u);
        goto block_80139AB8;  /* not taken */
    }

block_80139AB8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 130);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[17] + 96);
    int _bc_80139AC0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139AC4: 0x00000000 */
    if (_bc_80139AC0) {
        psx_check_interrupts_at(cpu, 0x80139B78u);
        goto block_80139B78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AC8u);
        goto block_80139AC8;  /* not taken */
    }

block_80139AC8:
    cpu->gpr[4] = cpu->read_half(cpu->gpr[18] + 2);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[18]);
    cpu->gpr[3] = cpu->gpr[4];
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[3] = cpu->gpr[3] + -50;
    cpu->write_half(cpu->gpr[18] + 2, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    int _bc_80139AE4 = ((int32_t)cpu->gpr[3] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[18], (uint16_t)cpu->gpr[2]);
    if (_bc_80139AE4) {
        psx_check_interrupts_at(cpu, 0x80139AF0u);
        goto block_80139AF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139AECu);
        goto block_80139AEC;  /* not taken */
    }

block_80139AEC:
    cpu->write_half(cpu->gpr[18] + 2, (uint16_t)cpu->gpr[0]);
    /* fall through to block_80139AF0 */

block_80139AF0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[17] + 84);
    /* nop */  /* 0x80139AF4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 4;
    cpu->write_word(cpu->gpr[17] + 84, cpu->gpr[2]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139B04: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[2] + 4);
    /* nop */  /* 0x80139B1C: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] + -24;
    cpu->write_byte(cpu->gpr[2] + 4, (uint8_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139B2C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[2] + 5);
    /* nop */  /* 0x80139B44: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] + -24;
    cpu->write_byte(cpu->gpr[2] + 5, (uint8_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139B54: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[2] + 6);
    /* nop */  /* 0x80139B6C: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] + -24;
    cpu->write_byte(cpu->gpr[2] + 6, (uint8_t)cpu->gpr[3]);
    /* fall through to block_80139B78 */

block_80139B78:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[20] + 36);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[17] + 84);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[2] = cpu->read_word(cpu->gpr[21]);
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[20] + 21);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    if (cpu->gpr[2] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[8] / (int32_t)cpu->gpr[2]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[8] % (int32_t)cpu->gpr[2]); } else { cpu->lo = ((int32_t)cpu->gpr[8] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[8]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[16] = cpu->lo;
    psx_muldiv_stall(cpu);
    int _bc_80139BA4 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[21] = cpu->gpr[16];
    if (_bc_80139BA4) {
        psx_check_interrupts_at(cpu, 0x80139BB8u);
        goto block_80139BB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80139BACu);
        goto block_80139BAC;  /* not taken */
    }

block_80139BAC:
    cpu->gpr[2] = (uint32_t)cpu->gpr[16] >> 31;
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
    cpu->gpr[21] = (int32_t)cpu->gpr[2] >> 1;
    /* fall through to block_80139BB8 */

block_80139BB8:
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139BC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139BC0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139BC4:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[16] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[16]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139BCC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 8, (uint16_t)cpu->gpr[3]);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139C00u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139BFC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139C00:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[16] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[16]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139C08: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 32, (uint16_t)cpu->gpr[3]);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139C3Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139C38: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139C3C:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[21] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[21]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139C44: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 10, (uint16_t)cpu->gpr[3]);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139C78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80139C74: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139C78:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[21] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[21]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139C80: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 34, (uint16_t)cpu->gpr[3]);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139CB4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139CB4:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[16] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[16]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139CBC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 16, (uint16_t)cpu->gpr[3]);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139CF0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    psx_check_interrupts_at(cpu, 0x8018D298u);
    cpu->pc = 0x8018D298u; return;  /* CPS jal -> 0x8018D298 */

block_80139CF0:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[16] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[16]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139CF8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19]);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 24, (uint16_t)cpu->gpr[3]);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139D2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139D2C:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[21] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[21]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139D34: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 18, (uint16_t)cpu->gpr[3]);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[18]);
    cpu->gpr[31] = 0x80139D68u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[4] + 1024;
    psx_check_interrupts_at(cpu, 0x8018D29Cu);
    cpu->pc = 0x8018D29Cu; return;  /* CPS jal -> 0x8018D29C */

block_80139D68:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[21] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[21]));
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139D70: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[19] + 2);
    cpu->gpr[9] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[4] = (int32_t)cpu->gpr[9] >> 12;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[4];
    cpu->write_half(cpu->gpr[2] + 26, (uint16_t)cpu->gpr[3]);
    cps_80139d98(); return;  /* fallthrough to split piece */
}

/* cps_80139d98 — clean CPS transliteration of br2eu_func_80139D98 (guest 0x80139d98)
 * from SLES_017.22_full_08.c. */
void cps_80139d98(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80139D98, Size: 116 bytes, Blocks: 1 */

block_80139D98:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    cpu->gpr[4] = 0x0900 << 16;  /* 0x09000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 8);
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[3] + 12);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];
    cpu->gpr[3] = cpu->gpr[3] | cpu->gpr[4];
    cpu->write_word(cpu->gpr[2], cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 12);
    /* nop */  /* 0x80139DC4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[2] = cpu->gpr[17] + cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[2] << 8;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[20] + 8);
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 8;
    cpu->write_word(cpu->gpr[3] + 12, cpu->gpr[2]);
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 56);
    cpu->gpr[21] = cpu->read_word(cpu->gpr[29] + 52);
    cpu->gpr[20] = cpu->read_word(cpu->gpr[29] + 48);
    cpu->gpr[19] = cpu->read_word(cpu->gpr[29] + 44);
    cpu->gpr[18] = cpu->read_word(cpu->gpr[29] + 40);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 36);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 32);
    uint32_t _jt_80139E04 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 64;
    psx_check_interrupts_at(cpu, _jt_80139E04);
    cpu->pc = _jt_80139E04; return;  /* CPS: jr $ra */
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

/* cps_801aed3c — clean CPS transliteration of br2eu_func_801AED3C (guest 0x801aed3c)
 * from SLES_017.22_full_53.c. */
void cps_801aed3c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801AED80u: goto block_801AED80;
            case 0x801AEDACu: goto block_801AEDAC;
            case 0x801AED58u: goto block_801AED58;
            case 0x801AED88u: goto block_801AED88;
            default: break;
        }
    }
    /* Address: 0x801AED3C, Size: 124 bytes, Blocks: 5 */

block_801AED3C:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[6] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x801AED4C: 0x00000000 */
    int _bc_801AED50 = (cpu->gpr[6] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801AED54: 0x00000000 */
    if (_bc_801AED50) {
        psx_check_interrupts_at(cpu, 0x801AED88u);
        goto block_801AED88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AED58u);
        goto block_801AED58;  /* not taken */
    }

block_801AED58:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 4);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[28] + 76);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[3] + 8);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5]);
    /* nop */  /* 0x801AED74: 0x00000000 */
    uint32_t _jt_801AED78 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801AED80u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_801AED78);
    cpu->pc = _jt_801AED78; return;  /* CPS jalr */

block_801AED80:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16], cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x801AEDACu);
    goto block_801AEDAC;  /* j */

block_801AED88:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 4);
    /* nop */  /* 0x801AED8C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5]);
    /* nop */  /* 0x801AEDA0: 0x00000000 */
    uint32_t _jt_801AEDA4 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801AEDACu;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801AEDA8: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801AEDA4);
    cpu->pc = _jt_801AEDA4; return;  /* CPS jalr */

block_801AEDAC:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_801AEDB4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 24;
    psx_check_interrupts_at(cpu, _jt_801AEDB4);
    cpu->pc = _jt_801AEDB4; return;  /* CPS: jr $ra */
}

/* cps_801aedbc — clean CPS transliteration of br2eu_func_801AEDBC (guest 0x801aedbc)
 * from SLES_017.22_full_53.c. */
void cps_801aedbc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801AEE10u: goto block_801AEE10;
            case 0x801AEE48u: goto block_801AEE48;
            case 0x801AEDD8u: goto block_801AEDD8;
            case 0x801AEE18u: goto block_801AEE18;
            default: break;
        }
    }
    /* Address: 0x801AEDBC, Size: 156 bytes, Blocks: 5 */

block_801AEDBC:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[6] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x801AEDCC: 0x00000000 */
    int _bc_801AEDD0 = (cpu->gpr[6] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[5];
    if (_bc_801AEDD0) {
        psx_check_interrupts_at(cpu, 0x801AEE18u);
        goto block_801AEE18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AEDD8u);
        goto block_801AEDD8;  /* not taken */
    }

block_801AEDD8:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 4);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 6);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[7];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[28] + 76);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[3]);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[2] + 8);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[8]);
    /* nop */  /* 0x801AEE04: 0x00000000 */
    uint32_t _jt_801AEE08 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801AEE10u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_801AEE08);
    cpu->pc = _jt_801AEE08; return;  /* CPS jalr */

block_801AEE10:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16], cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x801AEE48u);
    goto block_801AEE48;  /* j */

block_801AEE18:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 4);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 6);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[7];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[3]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[8]);
    /* nop */  /* 0x801AEE3C: 0x00000000 */
    uint32_t _jt_801AEE40 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801AEE48u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801AEE44: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801AEE40);
    cpu->pc = _jt_801AEE40; return;  /* CPS jalr */

block_801AEE48:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_801AEE50 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 24;
    psx_check_interrupts_at(cpu, _jt_801AEE50);
    cpu->pc = _jt_801AEE50; return;  /* CPS: jr $ra */
}

/* cps_801aef18 — clean CPS transliteration of br2eu_func_801AEF18 (guest 0x801aef18)
 * from SLES_017.22_full_53.c. */
void cps_801aef18(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801AEF90u: goto block_801AEF90;
            case 0x801AEFE8u: goto block_801AEFE8;
            case 0x801AEF38u: goto block_801AEF38;
            case 0x801AEF98u: goto block_801AEF98;
            default: break;
        }
    }
    /* Address: 0x801AEF18, Size: 224 bytes, Blocks: 5 */

block_801AEF18:
    cpu->gpr[29] = cpu->gpr[29] + -24;
    cpu->gpr[8] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[9] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x801AEF2C: 0x00000000 */
    int _bc_801AEF30 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[10] = cpu->gpr[5];
    if (_bc_801AEF30) {
        psx_check_interrupts_at(cpu, 0x801AEF98u);
        goto block_801AEF98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801AEF38u);
        goto block_801AEF38;  /* not taken */
    }

block_801AEF38:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 4);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 6);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[7];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 8);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[3]);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 10);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[7];
    cpu->gpr[6] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[28] + 76);
    cpu->gpr[7] = cpu->read_word(cpu->gpr[3]);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[2] + 8);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[10]);
    /* nop */  /* 0x801AEF84: 0x00000000 */
    uint32_t _jt_801AEF88 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801AEF90u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[9];
    psx_check_interrupts_at(cpu, _jt_801AEF88);
    cpu->pc = _jt_801AEF88; return;  /* CPS jalr */

block_801AEF90:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[16], cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x801AEFE8u);
    goto block_801AEFE8;  /* j */

block_801AEF98:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 4);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 6);
    cpu->gpr[6] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 8);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[7];
    cpu->gpr[6] = cpu->gpr[6] << 2;
    cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[7];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[3]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[8] + 10);
    cpu->gpr[6] = cpu->read_word(cpu->gpr[6]);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[7];
    cpu->gpr[7] = cpu->read_word(cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[10]);
    /* nop */  /* 0x801AEFDC: 0x00000000 */
    uint32_t _jt_801AEFE0 = cpu->gpr[2];  /* latch indirect target before delay slot */
    cpu->gpr[31] = 0x801AEFE8u;  /* jalr link before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801AEFE4: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801AEFE0);
    cpu->pc = _jt_801AEFE0; return;  /* CPS jalr */

block_801AEFE8:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_801AEFF0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 24;
    psx_check_interrupts_at(cpu, _jt_801AEFF0);
    cpu->pc = _jt_801AEFF0; return;  /* CPS: jr $ra */
}

/* cps_80172a2c — clean CPS transliteration of br2eu_func_80172A2C (guest 0x80172a2c)
 * from SLES_017.22_full_32.c. */
void cps_80172a2c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80172A2C, Size: 8 bytes, Blocks: 1 */

block_80172A2C:
    uint32_t _jt_80172A2C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    psx_check_interrupts_at(cpu, _jt_80172A2C);
    cpu->pc = _jt_80172A2C; return;  /* CPS: jr $ra */
}

/* cps_80175a18 — clean CPS transliteration of br2eu_func_80175A18 (guest 0x80175a18)
 * from SLES_017.22_full_32.c. */
void cps_80175a18(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80175A18, Size: 100 bytes, Blocks: 1 */

block_80175A18:
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 176);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 222);
    /* nop */  /* 0x80175A20: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    cpu->gpr[4] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = 0x51EB << 16;  /* 0x51EB0000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x851F;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    cpu->gpr[3] = 0x0020 << 16;  /* 0x00200000 */
    cpu->write_half(cpu->gpr[6] + 216, (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = 7680;
    cpu->write_half(cpu->gpr[6] + 214, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[6] + 92);
    cpu->gpr[4] = (int32_t)cpu->gpr[4] >> 31;
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];
    cpu->write_word(cpu->gpr[6] + 92, cpu->gpr[2]);
    cpu->gpr[8] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = (int32_t)cpu->gpr[8] >> 5;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[4];
    cpu->write_half(cpu->gpr[6] + 176, (uint16_t)cpu->gpr[2]);
    cpu->write_half(cpu->gpr[6] + 172, (uint16_t)cpu->gpr[2]);
    cpu->write_half(cpu->gpr[6] + 170, (uint16_t)cpu->gpr[2]);
    cpu->write_half(cpu->gpr[6] + 174, (uint16_t)cpu->gpr[2]);
    uint32_t _jt_80175A74 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80175A78: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80175A74);
    cpu->pc = _jt_80175A74; return;  /* CPS: jr $ra */
}

/* cps_801796e4 — clean CPS transliteration of br2eu_func_801796E4 (guest 0x801796e4)
 * from SLES_017.22_full_34.c. */
void cps_801796e4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x801796E4, Size: 8 bytes, Blocks: 1 */

block_801796E4:
    uint32_t _jt_801796E4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801796E8: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_801796E4);
    cpu->pc = _jt_801796E4; return;  /* CPS: jr $ra */
}

/* cps_80179a5c — clean CPS transliteration of br2eu_func_80179A5C (guest 0x80179a5c)
 * from SLES_017.22_full_34.c. */
void cps_80179a5c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80179A5C, Size: 4 bytes, Blocks: 1 */

block_80179A5C:
    cpu->write_word(cpu->gpr[4] + 68, cpu->gpr[5]);
    cps_80179a60(); return;  /* fallthrough to split piece */
    cps_80179a60();  /* fallthrough to next function */
}

/* cps_80179a60 — clean CPS transliteration of br2eu_func_80179A60 (guest 0x80179a60)
 * from SLES_017.22_full_34.c. */
void cps_80179a60(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x80179A60, Size: 12 bytes, Blocks: 1 */

block_80179A60:
    cpu->write_word(cpu->gpr[5] + 76, cpu->gpr[0]);
    uint32_t _jt_80179A64 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80179A68: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_80179A64);
    cpu->pc = _jt_80179A64; return;  /* CPS: jr $ra */
}

/* cps_8017a6e0 — clean CPS transliteration of br2eu_func_8017A6E0 (guest 0x8017a6e0)
 * from SLES_017.22_full_34.c. */
void cps_8017a6e0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017A6E0, Size: 8 bytes, Blocks: 1 */

block_8017A6E0:
    uint32_t _jt_8017A6E0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A6E4: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017A6E0);
    cpu->pc = _jt_8017A6E0; return;  /* CPS: jr $ra */
}

/* cps_8017a990 — clean CPS transliteration of br2eu_func_8017A990 (guest 0x8017a990)
 * from SLES_017.22_full_34.c. */
void cps_8017a990(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017A9A8u: goto block_8017A9A8;
            case 0x8017A9B0u: goto block_8017A9B0;
            default: break;
        }
    }
    /* Address: 0x8017A990, Size: 40 bytes, Blocks: 3 */

block_8017A990:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017A994: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 24;
    int _bc_8017A9A0 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_8017A9A0) {
        psx_check_interrupts_at(cpu, 0x8017A9B8u);
        cpu->pc = 0x8017A9B8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017A9A8u);
        goto block_8017A9A8;  /* not taken */
    }

block_8017A9A8:
    int _bc_8017A9A8 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A9AC: 0x00000000 */
    if (_bc_8017A9A8) {
        psx_check_interrupts_at(cpu, 0x8017A9C4u);
        cpu->pc = 0x8017A9C4u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017A9B0u);
        goto block_8017A9B0;  /* not taken */
    }

block_8017A9B0:
    uint32_t _jt_8017A9B0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A9B4: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017A9B0);
    cpu->pc = _jt_8017A9B0; return;  /* CPS: jr $ra */
}

/* cps_8017a9b8 — clean CPS transliteration of br2eu_func_8017A9B8 (guest 0x8017a9b8)
 * from SLES_017.22_full_34.c. */
void cps_8017a9b8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017A9B8, Size: 12 bytes, Blocks: 1 */

block_8017A9B8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 136);
    uint32_t _jt_8017A9BC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A9C0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017A9BC);
    cpu->pc = _jt_8017A9BC; return;  /* CPS: jr $ra */
}

/* cps_8017a9c4 — clean CPS transliteration of br2eu_func_8017A9C4 (guest 0x8017a9c4)
 * from SLES_017.22_full_34.c. */
void cps_8017a9c4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017A9C4, Size: 12 bytes, Blocks: 1 */

block_8017A9C4:
    cpu->gpr[2] = cpu->gpr[6] + 2;
    uint32_t _jt_8017A9C8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[5] + 136, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, _jt_8017A9C8);
    cpu->pc = _jt_8017A9C8; return;  /* CPS: jr $ra */
}

/* cps_8017a9d0 — clean CPS transliteration of br2eu_func_8017A9D0 (guest 0x8017a9d0)
 * from SLES_017.22_full_34.c. */
void cps_8017a9d0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017A9E8u: goto block_8017A9E8;
            default: break;
        }
    }
    /* Address: 0x8017A9D0, Size: 44 bytes, Blocks: 2 */

block_8017A9D0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017A9D4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 24;
    int _bc_8017A9E0 = ((int32_t)cpu->gpr[3] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A9E4: 0x00000000 */
    if (_bc_8017A9E0) {
        psx_check_interrupts_at(cpu, 0x8017AA0Cu);
        cpu->pc = 0x8017AA0Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017A9E8u);
        goto block_8017A9E8;  /* not taken */
    }

block_8017A9E8:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 198);
    /* nop */  /* 0x8017A9EC: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x8017A9F0: 0x0043102A */
    int _bc_8017A9F4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017A9F8: 0x00000000 */
    if (_bc_8017A9F4) {
        psx_check_interrupts_at(cpu, 0x8017AA04u);
        cpu->pc = 0x8017AA04u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017A9FCu);
        cpu->pc = 0x8017A9FCu; return;  /* CPS not taken: split */
    }
}

/* cps_8017a9fc — clean CPS transliteration of br2eu_func_8017A9FC (guest 0x8017a9fc)
 * from SLES_017.22_full_34.c. */
void cps_8017a9fc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017A9FC, Size: 8 bytes, Blocks: 1 */

block_8017A9FC:
    uint32_t _jt_8017A9FC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 2;
    psx_check_interrupts_at(cpu, _jt_8017A9FC);
    cpu->pc = _jt_8017A9FC; return;  /* CPS: jr $ra */
}

/* cps_8017aa04 — clean CPS transliteration of br2eu_func_8017AA04 (guest 0x8017aa04)
 * from SLES_017.22_full_34.c. */
void cps_8017aa04(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017AA04, Size: 8 bytes, Blocks: 1 */

block_8017AA04:
    uint32_t _jt_8017AA04 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, _jt_8017AA04);
    cpu->pc = _jt_8017AA04; return;  /* CPS: jr $ra */
}

/* cps_8017ab30 — clean CPS transliteration of br2eu_func_8017AB30 (guest 0x8017ab30)
 * from SLES_017.22_full_34.c. */
void cps_8017ab30(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017AB48u: goto block_8017AB48;
            default: break;
        }
    }
    /* Address: 0x8017AB30, Size: 44 bytes, Blocks: 2 */

block_8017AB30:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017AB34: 0x00000000 */
    cpu->gpr[3] = (uint32_t)cpu->gpr[2] >> 8;
    cpu->gpr[2] = 255;
    int _bc_8017AB40 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0];
    if (_bc_8017AB40) {
        psx_check_interrupts_at(cpu, 0x8017AB5Cu);
        cpu->pc = 0x8017AB5Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017AB48u);
        goto block_8017AB48;  /* not taken */
    }

block_8017AB48:
    cpu->gpr[3] = cpu->gpr[3] << 4;
    cpu->write_half(cpu->gpr[5] + 144, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->gpr[6] + 2;
    uint32_t _jt_8017AB54 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[5] + 132, cpu->gpr[3]);
    psx_check_interrupts_at(cpu, _jt_8017AB54);
    cpu->pc = _jt_8017AB54; return;  /* CPS: jr $ra */
}

/* cps_8017ab5c — clean CPS transliteration of br2eu_func_8017AB5C (guest 0x8017ab5c)
 * from SLES_017.22_full_34.c. */
void cps_8017ab5c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017AB5C, Size: 24 bytes, Blocks: 1 */

block_8017AB5C:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 190);
    cpu->gpr[4] = cpu->gpr[6] + 2;
    cpu->write_word(cpu->gpr[5] + 132, cpu->gpr[4]);
    cpu->gpr[3] = cpu->gpr[3] << 4;
    uint32_t _jt_8017AB6C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 144, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, _jt_8017AB6C);
    cpu->pc = _jt_8017AB6C; return;  /* CPS: jr $ra */
}

/* cps_8017ac44 — clean CPS transliteration of br2eu_func_8017AC44 (guest 0x8017ac44)
 * from SLES_017.22_full_34.c. */
void cps_8017ac44(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017AC44, Size: 8 bytes, Blocks: 1 */

block_8017AC44:
    uint32_t _jt_8017AC44 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017AC44);
    cpu->pc = _jt_8017AC44; return;  /* CPS: jr $ra */
}

/* cps_8017ac4c — clean CPS transliteration of br2eu_func_8017AC4C (guest 0x8017ac4c)
 * from SLES_017.22_full_34.c. */
void cps_8017ac4c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017AC5Cu: goto block_8017AC5C;
            default: break;
        }
    }
    /* Address: 0x8017AC4C, Size: 24 bytes, Blocks: 2 */

block_8017AC4C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 148);
    /* nop */  /* 0x8017AC50: 0x00000000 */
    int _bc_8017AC54 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AC58: 0x00000000 */
    if (_bc_8017AC54) {
        psx_check_interrupts_at(cpu, 0x8017AC44u);
        cpu->pc = 0x8017AC44u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017AC5Cu);
        goto block_8017AC5C;  /* not taken */
    }

block_8017AC5C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AC60: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017AE00u);
    cpu->pc = 0x8017AE00u; return;  /* CPS j: split */
}

/* cps_8017ad78 — clean CPS transliteration of br2eu_func_8017AD78 (guest 0x8017ad78)
 * from SLES_017.22_full_34.c. */
void cps_8017ad78(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017AD80u: goto block_8017AD80;
            case 0x8017AD88u: goto block_8017AD88;
            case 0x8017AD94u: goto block_8017AD94;
            case 0x8017ADA0u: goto block_8017ADA0;
            case 0x8017ADB0u: goto block_8017ADB0;
            default: break;
        }
    }
    /* Address: 0x8017AD78, Size: 64 bytes, Blocks: 6 */

block_8017AD78:
    int _bc_8017AD78 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AD7C: 0x00000000 */
    if (_bc_8017AD78) {
        psx_check_interrupts_at(cpu, 0x8017AE00u);
        cpu->pc = 0x8017AE00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017AD80u);
        goto block_8017AD80;  /* not taken */
    }

block_8017AD80:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AD84: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017AC44u);
    cpu->pc = 0x8017AC44u; return;  /* CPS j: split */

block_8017AD88:
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 3);
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    psx_check_interrupts_at(cpu, 0x8017AD78u);
    goto block_8017AD78;  /* j */

block_8017AD94:
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 3);
    /* delay slot (always executes) */
    cpu->gpr[2] = 3;
    psx_check_interrupts_at(cpu, 0x8017AD78u);
    goto block_8017AD78;  /* j */

block_8017ADA0:
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 3);
    cpu->gpr[2] = 4;
    int _bc_8017ADA8 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + cpu->gpr[2];
    if (_bc_8017ADA8) {
        psx_check_interrupts_at(cpu, 0x8017AE00u);
        cpu->pc = 0x8017AE00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017ADB0u);
        goto block_8017ADB0;  /* not taken */
    }

block_8017ADB0:
    uint32_t _jt_8017ADB0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017ADB4: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017ADB0);
    cpu->pc = _jt_8017ADB0; return;  /* CPS: jr $ra */
}

/* cps_8017adb8 — clean CPS transliteration of br2eu_func_8017ADB8 (guest 0x8017adb8)
 * from SLES_017.22_full_34.c. */
void cps_8017adb8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017ADB8, Size: 12 bytes, Blocks: 1 */

block_8017ADB8:
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 3);
    /* delay slot (always executes) */
    cpu->gpr[2] = 5;
    psx_check_interrupts_at(cpu, 0x8017AD78u);
    cpu->pc = 0x8017AD78u; return;  /* CPS j: split */
    cpu->pc = 0x8017ADC4u; return;  /* image-edge fallthrough: tail-transfer */
}

/* cps_8017ae00 — clean CPS transliteration of br2eu_func_8017AE00 (guest 0x8017ae00)
 * from SLES_017.22_full_34.c. */
void cps_8017ae00(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017AE00, Size: 24 bytes, Blocks: 1 */

block_8017AE00:
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 48);
    /* nop */  /* 0x8017AE08: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    uint32_t _jt_8017AE10 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AE14: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017AE10);
    cpu->pc = _jt_8017AE10; return;  /* CPS: jr $ra */
}

/* cps_8017ae10 — clean CPS transliteration of br2eu_func_8017AE10 (guest 0x8017ae10)
 * from SLES_017.22_full_34.c. */
void cps_8017ae10(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017AE10, Size: 8 bytes, Blocks: 1 */

block_8017AE10:
    uint32_t _jt_8017AE10 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017AE14: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017AE10);
    cpu->pc = _jt_8017AE10; return;  /* CPS: jr $ra */
}

/* cps_8017b5f4 — clean CPS transliteration of br2eu_func_8017B5F4 (guest 0x8017b5f4)
 * from SLES_017.22_full_35.c. */
void cps_8017b5f4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017B61Cu: goto block_8017B61C;
            case 0x8017B624u: goto block_8017B624;
            case 0x8017B62Cu: goto block_8017B62C;
            case 0x8017B634u: goto block_8017B634;
            default: break;
        }
    }
    /* Address: 0x8017B5F4, Size: 84 bytes, Blocks: 5 */

block_8017B5F4:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017B5F8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[7] = (int32_t)cpu->gpr[2] >> 20;
    cpu->gpr[2] = 1;
    int _bc_8017B614 = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[7] < 2) ? 1 : 0;  /* 0x8017B618: 0x28E20002 */
    if (_bc_8017B614) {
        psx_check_interrupts_at(cpu, 0x8017B634u);
        goto block_8017B634;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B61Cu);
        goto block_8017B61C;  /* not taken */
    }

block_8017B61C:
    int _bc_8017B61C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017B61C) {
        psx_check_interrupts_at(cpu, 0x8017B664u);
        cpu->pc = 0x8017B664u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B624u);
        goto block_8017B624;  /* not taken */
    }

block_8017B624:
    int _bc_8017B624 = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B628: 0x00000000 */
    if (_bc_8017B624) {
        psx_check_interrupts_at(cpu, 0x8017B650u);
        cpu->pc = 0x8017B650u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B62Cu);
        goto block_8017B62C;  /* not taken */
    }

block_8017B62C:
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017B630: 0x3063000F */
    psx_check_interrupts_at(cpu, 0x8017B668u);
    cpu->pc = 0x8017B668u; return;  /* CPS j: split */

block_8017B634:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017B638: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017B63C: 0x30420001 */
    int _bc_8017B640 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017B644: 0x3063000F */
    if (_bc_8017B640) {
        psx_check_interrupts_at(cpu, 0x8017B668u);
        cpu->pc = 0x8017B668u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B648u);
        cpu->pc = 0x8017B648u; return;  /* CPS not taken: split */
    }
}

/* cps_8017b648 — clean CPS transliteration of br2eu_func_8017B648 (guest 0x8017b648)
 * from SLES_017.22_full_35.c. */
void cps_8017b648(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017B648, Size: 8 bytes, Blocks: 1 */

block_8017B648:
    uint32_t _jt_8017B648 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017B648);
    cpu->pc = _jt_8017B648; return;  /* CPS: jr $ra */
}

/* cps_8017b650 — clean CPS transliteration of br2eu_func_8017B650 (guest 0x8017b650)
 * from SLES_017.22_full_35.c. */
void cps_8017b650(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017B650, Size: 20 bytes, Blocks: 1 */

block_8017B650:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017B654: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017B658: 0x30420001 */
    int _bc_8017B65C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B660: 0x00000000 */
    if (_bc_8017B65C) {
        psx_check_interrupts_at(cpu, 0x8017B648u);
        cpu->pc = 0x8017B648u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B664u);
        cpu->pc = 0x8017B664u; return;  /* CPS not taken: split */
    }
    cps_8017b664();  /* fallthrough to next function */
}

/* cps_8017b664 — clean CPS transliteration of br2eu_func_8017B664 (guest 0x8017b664)
 * from SLES_017.22_full_35.c. */
void cps_8017b664(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017B664, Size: 4 bytes, Blocks: 1 */

block_8017B664:
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017B664: 0x3063000F */
    cps_8017b668(); return;  /* fallthrough to split piece */
    cps_8017b668();  /* fallthrough to next function */
}

/* cps_8017b668 — clean CPS transliteration of br2eu_func_8017B668 (guest 0x8017b668)
 * from SLES_017.22_full_35.c. */
void cps_8017b668(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017B674u: goto block_8017B674;
            case 0x8017B690u: goto block_8017B690;
            case 0x8017B69Cu: goto block_8017B69C;
            case 0x8017B6ACu: goto block_8017B6AC;
            case 0x8017B6C0u: goto block_8017B6C0;
            case 0x8017B6D0u: goto block_8017B6D0;
            case 0x8017B6DCu: goto block_8017B6DC;
            case 0x8017B704u: goto block_8017B704;
            case 0x8017B710u: goto block_8017B710;
            case 0x8017B724u: goto block_8017B724;
            case 0x8017B728u: goto block_8017B728;
            case 0x8017B734u: goto block_8017B734;
            case 0x8017B738u: goto block_8017B738;
            case 0x8017B740u: goto block_8017B740;
            case 0x8017B744u: goto block_8017B744;
            case 0x8017B74Cu: goto block_8017B74C;
            case 0x8017B750u: goto block_8017B750;
            case 0x8017B758u: goto block_8017B758;
            case 0x8017B75Cu: goto block_8017B75C;
            case 0x8017B764u: goto block_8017B764;
            case 0x8017B768u: goto block_8017B768;
            case 0x8017B770u: goto block_8017B770;
            case 0x8017B774u: goto block_8017B774;
            case 0x8017B77Cu: goto block_8017B77C;
            case 0x8017B780u: goto block_8017B780;
            case 0x8017B79Cu: goto block_8017B79C;
            case 0x8017B7B0u: goto block_8017B7B0;
            case 0x8017B7C8u: goto block_8017B7C8;
            case 0x8017B7D4u: goto block_8017B7D4;
            case 0x8017B7E0u: goto block_8017B7E0;
            case 0x8017B7ECu: goto block_8017B7EC;
            case 0x8017B7F8u: goto block_8017B7F8;
            case 0x8017B83Cu: goto block_8017B83C;
            case 0x8017B840u: goto block_8017B840;
            case 0x8017B844u: goto block_8017B844;
            case 0x8017B84Cu: goto block_8017B84C;
            case 0x8017B858u: goto block_8017B858;
            default: break;
        }
    }
    /* Address: 0x8017B668, Size: 504 bytes, Blocks: 38 */

block_8017B668:
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)11) ? 1 : 0;  /* 0x8017B668: 0x2C62000B */
    int _bc_8017B66C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8017B66C) {
        psx_check_interrupts_at(cpu, 0x8017B858u);
        goto block_8017B858;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B674u);
        goto block_8017B674;  /* not taken */
    }

block_8017B674:
    cpu->gpr[2] = cpu->gpr[2] + 10236;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8017B684: 0x00000000 */
    uint32_t _jt_8017B688 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B68C: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017B688);
    cpu->pc = _jt_8017B688; return;  /* CPS: jr cpu->gpr[2] */

block_8017B690:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 208, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017B858u);
    goto block_8017B858;  /* j */

block_8017B69C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 208);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017B6C0u);
    goto block_8017B6C0;  /* j */

block_8017B6AC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 44);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2] + 78);
    /* nop */  /* 0x8017B6B8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    /* fall through to block_8017B6C0 */

block_8017B6C0:
    cpu->gpr[2] = cpu->gpr[2] << 20;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 208, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017B858u);
    goto block_8017B858;  /* j */

block_8017B6D0:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 116);
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B6D8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017B840u);
    goto block_8017B840;  /* j */

block_8017B6DC:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 78);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 196);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->gpr[2] + -2048;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    cpu->gpr[3] = cpu->gpr[3] << 20;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 20;
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 228, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x8017B858u);
    goto block_8017B858;  /* j */

block_8017B704:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 112);
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B70C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017B840u);
    goto block_8017B840;  /* j */

block_8017B710:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[4] + 108);
    /* nop */  /* 0x8017B714: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] & 0x0010;  /* 0x8017B718: 0x30620010 */
    int _bc_8017B71C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[0];
    if (_bc_8017B71C) {
        psx_check_interrupts_at(cpu, 0x8017B728u);
        goto block_8017B728;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B724u);
        goto block_8017B724;  /* not taken */
    }

block_8017B724:
    cpu->gpr[7] = -1024;
    /* fall through to block_8017B728 */

block_8017B728:
    cpu->gpr[2] = cpu->gpr[3] & 0x0020;  /* 0x8017B728: 0x30620020 */
    int _bc_8017B72C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & 0x0040;  /* 0x8017B730: 0x30620040 */
    if (_bc_8017B72C) {
        psx_check_interrupts_at(cpu, 0x8017B738u);
        goto block_8017B738;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B734u);
        goto block_8017B734;  /* not taken */
    }

block_8017B734:
    cpu->gpr[7] = 1024;
    /* fall through to block_8017B738 */

block_8017B738:
    int _bc_8017B738 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & 0x0080;  /* 0x8017B73C: 0x30620080 */
    if (_bc_8017B738) {
        psx_check_interrupts_at(cpu, 0x8017B744u);
        goto block_8017B744;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B740u);
        goto block_8017B740;  /* not taken */
    }

block_8017B740:
    cpu->gpr[7] = cpu->gpr[0];
    /* fall through to block_8017B744 */

block_8017B744:
    int _bc_8017B744 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & 0x0001;  /* 0x8017B748: 0x30620001 */
    if (_bc_8017B744) {
        psx_check_interrupts_at(cpu, 0x8017B750u);
        goto block_8017B750;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B74Cu);
        goto block_8017B74C;  /* not taken */
    }

block_8017B74C:
    cpu->gpr[7] = -2048;
    /* fall through to block_8017B750 */

block_8017B750:
    int _bc_8017B750 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & 0x0002;  /* 0x8017B754: 0x30620002 */
    if (_bc_8017B750) {
        psx_check_interrupts_at(cpu, 0x8017B75Cu);
        goto block_8017B75C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B758u);
        goto block_8017B758;  /* not taken */
    }

block_8017B758:
    cpu->gpr[7] = -1024;
    /* fall through to block_8017B75C */

block_8017B75C:
    int _bc_8017B75C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & 0x0004;  /* 0x8017B760: 0x30620004 */
    if (_bc_8017B75C) {
        psx_check_interrupts_at(cpu, 0x8017B768u);
        goto block_8017B768;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B764u);
        goto block_8017B764;  /* not taken */
    }

block_8017B764:
    cpu->gpr[7] = 1024;
    /* fall through to block_8017B768 */

block_8017B768:
    int _bc_8017B768 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & 0x0008;  /* 0x8017B76C: 0x30620008 */
    if (_bc_8017B768) {
        psx_check_interrupts_at(cpu, 0x8017B774u);
        goto block_8017B774;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B770u);
        goto block_8017B770;  /* not taken */
    }

block_8017B770:
    cpu->gpr[7] = cpu->gpr[0];
    /* fall through to block_8017B774 */

block_8017B774:
    int _bc_8017B774 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B778: 0x00000000 */
    if (_bc_8017B774) {
        psx_check_interrupts_at(cpu, 0x8017B780u);
        goto block_8017B780;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B77Cu);
        goto block_8017B77C;  /* not taken */
    }

block_8017B77C:
    cpu->gpr[7] = -2048;
    /* fall through to block_8017B780 */

block_8017B780:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 78);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 228);
    /* nop */  /* 0x8017B788: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[7] - cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8017B844u);
    goto block_8017B844;  /* j */

block_8017B79C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 92);
    cpu->gpr[3] = 0x0010 << 16;  /* 0x00100000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017B7A4: 0x00431024 */
    int _bc_8017B7A8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B7AC: 0x00000000 */
    if (_bc_8017B7A8) {
        psx_check_interrupts_at(cpu, 0x8017B858u);
        goto block_8017B858;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B7B0u);
        goto block_8017B7B0;  /* not taken */
    }

block_8017B7B0:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 116);
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[3];
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x8017B7BC: 0x0082102A */
    int _bc_8017B7C0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B7C4: 0x00000000 */
    if (_bc_8017B7C0) {
        psx_check_interrupts_at(cpu, 0x8017B7D4u);
        goto block_8017B7D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B7C8u);
        goto block_8017B7C8;  /* not taken */
    }

block_8017B7C8:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 228);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017B84Cu);
    goto block_8017B84C;  /* j */

block_8017B7D4:
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[4]) ? 1 : 0;  /* 0x8017B7D4: 0x0064102A */
    int _bc_8017B7D8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B7DC: 0x00000000 */
    if (_bc_8017B7D8) {
        psx_check_interrupts_at(cpu, 0x8017B7ECu);
        goto block_8017B7EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B7E0u);
        goto block_8017B7E0;  /* not taken */
    }

block_8017B7E0:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 228);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017B84Cu);
    goto block_8017B84C;  /* j */

block_8017B7EC:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 228);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    psx_check_interrupts_at(cpu, 0x8017B84Cu);
    goto block_8017B84C;  /* j */

block_8017B7F8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 32);
    /* nop */  /* 0x8017B7FC: 0x00000000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2] + 372);
    /* nop */  /* 0x8017B804: 0x00000000 */
    cpu->write_half(cpu->gpr[5] + 212, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 32);
    /* nop */  /* 0x8017B810: 0x00000000 */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[2] + 380);
    /* nop */  /* 0x8017B818: 0x00000000 */
    cpu->write_half(cpu->gpr[5] + 216, (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 78);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 196);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[3] = cpu->gpr[3] + -2048;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    psx_check_interrupts_at(cpu, 0x8017B84Cu);
    goto block_8017B84C;  /* j */

block_8017B83C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 228);
    /* fall through to block_8017B840 */

block_8017B840:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* fall through to block_8017B844 */

block_8017B844:
    /* nop */  /* 0x8017B844: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    /* fall through to block_8017B84C */

block_8017B84C:
    cpu->gpr[2] = cpu->gpr[2] << 20;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    cpu->write_half(cpu->gpr[5] + 228, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017B858 */

block_8017B858:
    uint32_t _jt_8017B858 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017B858);
    cpu->pc = _jt_8017B858; return;  /* CPS: jr $ra */
}

/* cps_8017b860 — clean CPS transliteration of br2eu_func_8017B860 (guest 0x8017b860)
 * from SLES_017.22_full_35.c. */
void cps_8017b860(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017B888u: goto block_8017B888;
            case 0x8017B890u: goto block_8017B890;
            case 0x8017B898u: goto block_8017B898;
            case 0x8017B8A0u: goto block_8017B8A0;
            default: break;
        }
    }
    /* Address: 0x8017B860, Size: 84 bytes, Blocks: 5 */

block_8017B860:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017B864: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 20;
    cpu->gpr[2] = 1;
    int _bc_8017B880 = (cpu->gpr[5] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[5] < 2) ? 1 : 0;  /* 0x8017B884: 0x28A20002 */
    if (_bc_8017B880) {
        psx_check_interrupts_at(cpu, 0x8017B8A0u);
        goto block_8017B8A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B888u);
        goto block_8017B888;  /* not taken */
    }

block_8017B888:
    int _bc_8017B888 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017B888) {
        psx_check_interrupts_at(cpu, 0x8017B8D0u);
        cpu->pc = 0x8017B8D0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B890u);
        goto block_8017B890;  /* not taken */
    }

block_8017B890:
    int _bc_8017B890 = (cpu->gpr[5] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B894: 0x00000000 */
    if (_bc_8017B890) {
        psx_check_interrupts_at(cpu, 0x8017B8BCu);
        cpu->pc = 0x8017B8BCu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B898u);
        goto block_8017B898;  /* not taken */
    }

block_8017B898:
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017B89C: 0x3063000F */
    psx_check_interrupts_at(cpu, 0x8017B8D4u);
    cpu->pc = 0x8017B8D4u; return;  /* CPS j: split */

block_8017B8A0:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017B8A4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017B8A8: 0x30420001 */
    int _bc_8017B8AC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017B8B0: 0x3063000F */
    if (_bc_8017B8AC) {
        psx_check_interrupts_at(cpu, 0x8017B8D4u);
        cpu->pc = 0x8017B8D4u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B8B4u);
        cpu->pc = 0x8017B8B4u; return;  /* CPS not taken: split */
    }
}

/* cps_8017b8b4 — clean CPS transliteration of br2eu_func_8017B8B4 (guest 0x8017b8b4)
 * from SLES_017.22_full_35.c. */
void cps_8017b8b4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017B8B4, Size: 8 bytes, Blocks: 1 */

block_8017B8B4:
    uint32_t _jt_8017B8B4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    psx_check_interrupts_at(cpu, _jt_8017B8B4);
    cpu->pc = _jt_8017B8B4; return;  /* CPS: jr $ra */
}

/* cps_8017b8bc — clean CPS transliteration of br2eu_func_8017B8BC (guest 0x8017b8bc)
 * from SLES_017.22_full_35.c. */
void cps_8017b8bc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017B8BC, Size: 20 bytes, Blocks: 1 */

block_8017B8BC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017B8C0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017B8C4: 0x30420001 */
    int _bc_8017B8C8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B8CC: 0x00000000 */
    if (_bc_8017B8C8) {
        psx_check_interrupts_at(cpu, 0x8017B8B4u);
        cpu->pc = 0x8017B8B4u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B8D0u);
        cpu->pc = 0x8017B8D0u; return;  /* CPS not taken: split */
    }
    cps_8017b8d0();  /* fallthrough to next function */
}

/* cps_8017b8d0 — clean CPS transliteration of br2eu_func_8017B8D0 (guest 0x8017b8d0)
 * from SLES_017.22_full_35.c. */
void cps_8017b8d0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017B8D0, Size: 4 bytes, Blocks: 1 */

block_8017B8D0:
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017B8D0: 0x3063000F */
    cps_8017b8d4(); return;  /* fallthrough to split piece */
    cps_8017b8d4();  /* fallthrough to next function */
}

/* cps_8017b8d4 — clean CPS transliteration of br2eu_func_8017B8D4 (guest 0x8017b8d4)
 * from SLES_017.22_full_35.c. */
void cps_8017b8d4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017B8E0u: goto block_8017B8E0;
            case 0x8017B8FCu: goto block_8017B8FC;
            case 0x8017B914u: goto block_8017B914;
            case 0x8017B92Cu: goto block_8017B92C;
            case 0x8017B944u: goto block_8017B944;
            case 0x8017B950u: goto block_8017B950;
            case 0x8017B970u: goto block_8017B970;
            case 0x8017B974u: goto block_8017B974;
            case 0x8017B98Cu: goto block_8017B98C;
            case 0x8017B9B4u: goto block_8017B9B4;
            case 0x8017B9C0u: goto block_8017B9C0;
            case 0x8017B9CCu: goto block_8017B9CC;
            case 0x8017B9DCu: goto block_8017B9DC;
            case 0x8017BA04u: goto block_8017BA04;
            case 0x8017BA10u: goto block_8017BA10;
            case 0x8017BA18u: goto block_8017BA18;
            case 0x8017BA30u: goto block_8017BA30;
            case 0x8017BA34u: goto block_8017BA34;
            default: break;
        }
    }
    /* Address: 0x8017B8D4, Size: 360 bytes, Blocks: 19 */

block_8017B8D4:
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)11) ? 1 : 0;  /* 0x8017B8D4: 0x2C62000B */
    int _bc_8017B8D8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8017B8D8) {
        psx_check_interrupts_at(cpu, 0x8017BA34u);
        goto block_8017BA34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B8E0u);
        goto block_8017B8E0;  /* not taken */
    }

block_8017B8E0:
    cpu->gpr[2] = cpu->gpr[2] + 10284;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8017B8F0: 0x00000000 */
    uint32_t _jt_8017B8F4 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B8F8: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017B8F4);
    cpu->pc = _jt_8017B8F4; return;  /* CPS: jr cpu->gpr[2] */

block_8017B8FC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017B900: 0x00000000 */
    cpu->write_half(cpu->gpr[4] + 138, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 4);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 140, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B914:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017B918: 0x00000000 */
    cpu->write_half(cpu->gpr[4] + 150, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 4);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 152, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B92C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017B930: 0x00000000 */
    cpu->write_half(cpu->gpr[4] + 144, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 4);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 146, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B944:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 142, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B950:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 116);
    cpu->gpr[5] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 118);
    cpu->write_half(cpu->gpr[4] + 150, (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017B960: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[5]) ? 1 : 0;  /* 0x8017B964: 0x0065102A */
    int _bc_8017B968 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017B96C: 0x00000000 */
    if (_bc_8017B968) {
        psx_check_interrupts_at(cpu, 0x8017B974u);
        goto block_8017B974;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017B970u);
        goto block_8017B970;  /* not taken */
    }

block_8017B970:
    cpu->gpr[5] = cpu->gpr[3];
    /* fall through to block_8017B974 */

block_8017B974:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 4);
    cpu->gpr[2] = cpu->gpr[5] << 7;
    if (cpu->gpr[3] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[2] / (int32_t)cpu->gpr[3]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[2] % (int32_t)cpu->gpr[3]); } else { cpu->lo = ((int32_t)cpu->gpr[2] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[2]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 152, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B98C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 112);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017B994: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 20;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    cpu->write_half(cpu->gpr[4] + 150, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 4);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 152, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B9B4:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 150, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B9C0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 146, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017B9CC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 152);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017BA30u);
    goto block_8017BA30;  /* j */

block_8017B9DC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 152);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017B9E4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[4] + 152, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 4097) ? 1 : 0;  /* 0x8017B9F8: 0x28621001 */
    int _bc_8017B9FC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 4096;
    if (_bc_8017B9FC) {
        psx_check_interrupts_at(cpu, 0x8017BA30u);
        goto block_8017BA30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA04u);
        goto block_8017BA04;  /* not taken */
    }

block_8017BA04:
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < -1280) ? 1 : 0;  /* 0x8017BA04: 0x2862FB00 */
    int _bc_8017BA08 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = -1280;
    if (_bc_8017BA08) {
        psx_check_interrupts_at(cpu, 0x8017BA34u);
        goto block_8017BA34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA10u);
        goto block_8017BA10;  /* not taken */
    }

block_8017BA10:
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4] + 152, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BA34u);
    goto block_8017BA34;  /* j */

block_8017BA18:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 152);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017BA20: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[7] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = (int32_t)cpu->gpr[7] >> 7;
    /* fall through to block_8017BA30 */

block_8017BA30:
    cpu->write_half(cpu->gpr[4] + 152, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017BA34 */

block_8017BA34:
    uint32_t _jt_8017BA34 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    psx_check_interrupts_at(cpu, _jt_8017BA34);
    cpu->pc = _jt_8017BA34; return;  /* CPS: jr $ra */
}

/* cps_8017ba3c — clean CPS transliteration of br2eu_func_8017BA3C (guest 0x8017ba3c)
 * from SLES_017.22_full_35.c. */
void cps_8017ba3c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017BA68u: goto block_8017BA68;
            case 0x8017BA70u: goto block_8017BA70;
            case 0x8017BA78u: goto block_8017BA78;
            case 0x8017BA80u: goto block_8017BA80;
            case 0x8017BA88u: goto block_8017BA88;
            case 0x8017BA90u: goto block_8017BA90;
            case 0x8017BA98u: goto block_8017BA98;
            default: break;
        }
    }
    /* Address: 0x8017BA3C, Size: 112 bytes, Blocks: 8 */

block_8017BA3C:
    cpu->gpr[8] = cpu->gpr[4];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 4);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[3];
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 8;
    cpu->gpr[3] = cpu->gpr[2] & 0x00C0;  /* 0x8017BA58: 0x304300C0 */
    cpu->gpr[2] = 64;
    int _bc_8017BA60 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 65) ? 1 : 0;  /* 0x8017BA64: 0x28620041 */
    if (_bc_8017BA60) {
        psx_check_interrupts_at(cpu, 0x8017BA98u);
        goto block_8017BA98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA68u);
        goto block_8017BA68;  /* not taken */
    }

block_8017BA68:
    int _bc_8017BA68 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 128;
    if (_bc_8017BA68) {
        psx_check_interrupts_at(cpu, 0x8017BA80u);
        goto block_8017BA80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA70u);
        goto block_8017BA70;  /* not taken */
    }

block_8017BA70:
    int _bc_8017BA70 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017BA74: 0x00000000 */
    if (_bc_8017BA70) {
        psx_check_interrupts_at(cpu, 0x8017BAC8u);
        cpu->pc = 0x8017BAC8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA78u);
        goto block_8017BA78;  /* not taken */
    }

block_8017BA78:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017BA7C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017BAD4u);
    cpu->pc = 0x8017BAD4u; return;  /* CPS j: split */

block_8017BA80:
    int _bc_8017BA80 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 192;
    if (_bc_8017BA80) {
        psx_check_interrupts_at(cpu, 0x8017BAB4u);
        cpu->pc = 0x8017BAB4u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA88u);
        goto block_8017BA88;  /* not taken */
    }

block_8017BA88:
    int _bc_8017BA88 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017BA8C: 0x00000000 */
    if (_bc_8017BA88) {
        psx_check_interrupts_at(cpu, 0x8017BAD0u);
        cpu->pc = 0x8017BAD0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BA90u);
        goto block_8017BA90;  /* not taken */
    }

block_8017BA90:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017BA94: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017BAD4u);
    cpu->pc = 0x8017BAD4u; return;  /* CPS j: split */

block_8017BA98:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[8] + 5);
    /* nop */  /* 0x8017BA9C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017BAA0: 0x30420001 */
    int _bc_8017BAA4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[8];
    if (_bc_8017BAA4) {
        psx_check_interrupts_at(cpu, 0x8017BAD4u);
        cpu->pc = 0x8017BAD4u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BAACu);
        cpu->pc = 0x8017BAACu; return;  /* CPS not taken: split */
    }
}

/* cps_8017baac — clean CPS transliteration of br2eu_func_8017BAAC (guest 0x8017baac)
 * from SLES_017.22_full_35.c. */
void cps_8017baac(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017BAAC, Size: 8 bytes, Blocks: 1 */

block_8017BAAC:
    uint32_t _jt_8017BAAC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    psx_check_interrupts_at(cpu, _jt_8017BAAC);
    cpu->pc = _jt_8017BAAC; return;  /* CPS: jr $ra */
}

/* cps_8017bab4 — clean CPS transliteration of br2eu_func_8017BAB4 (guest 0x8017bab4)
 * from SLES_017.22_full_35.c. */
void cps_8017bab4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017BAB4, Size: 20 bytes, Blocks: 1 */

block_8017BAB4:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[8] + 5);
    /* nop */  /* 0x8017BAB8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017BABC: 0x30420001 */
    int _bc_8017BAC0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017BAC4: 0x00000000 */
    if (_bc_8017BAC0) {
        psx_check_interrupts_at(cpu, 0x8017BAACu);
        cpu->pc = 0x8017BAACu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BAC8u);
        cpu->pc = 0x8017BAC8u; return;  /* CPS not taken: split */
    }
    cps_8017bac8();  /* fallthrough to next function */
}

/* cps_8017bac8 — clean CPS transliteration of br2eu_func_8017BAC8 (guest 0x8017bac8)
 * from SLES_017.22_full_35.c. */
void cps_8017bac8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017BAC8, Size: 8 bytes, Blocks: 1 */

block_8017BAC8:
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[8];
    psx_check_interrupts_at(cpu, 0x8017BAD4u);
    cpu->pc = 0x8017BAD4u; return;  /* CPS j: split */
    cps_8017bad0();  /* fallthrough to next function */
}

/* cps_8017bad0 — clean CPS transliteration of br2eu_func_8017BAD0 (guest 0x8017bad0)
 * from SLES_017.22_full_35.c. */
void cps_8017bad0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017BAD0, Size: 4 bytes, Blocks: 1 */

block_8017BAD0:
    cpu->gpr[7] = cpu->read_word(cpu->gpr[8] + 44);
    cps_8017bad4(); return;  /* fallthrough to split piece */
    cps_8017bad4();  /* fallthrough to next function */
}

/* cps_8017bad4 — clean CPS transliteration of br2eu_func_8017BAD4 (guest 0x8017bad4)
 * from SLES_017.22_full_35.c. */
void cps_8017bad4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017BAF0u: goto block_8017BAF0;
            case 0x8017BB0Cu: goto block_8017BB0C;
            case 0x8017BB24u: goto block_8017BB24;
            case 0x8017BB3Cu: goto block_8017BB3C;
            case 0x8017BB4Cu: goto block_8017BB4C;
            case 0x8017BB58u: goto block_8017BB58;
            case 0x8017BB60u: goto block_8017BB60;
            case 0x8017BB68u: goto block_8017BB68;
            case 0x8017BB70u: goto block_8017BB70;
            case 0x8017BB78u: goto block_8017BB78;
            case 0x8017BB8Cu: goto block_8017BB8C;
            case 0x8017BBA0u: goto block_8017BBA0;
            case 0x8017BBB4u: goto block_8017BBB4;
            case 0x8017BBC8u: goto block_8017BBC8;
            case 0x8017BBDCu: goto block_8017BBDC;
            case 0x8017BBF0u: goto block_8017BBF0;
            case 0x8017BC04u: goto block_8017BC04;
            case 0x8017BC18u: goto block_8017BC18;
            case 0x8017BC2Cu: goto block_8017BC2C;
            case 0x8017BC40u: goto block_8017BC40;
            case 0x8017BC54u: goto block_8017BC54;
            case 0x8017BC68u: goto block_8017BC68;
            case 0x8017BC7Cu: goto block_8017BC7C;
            case 0x8017BC90u: goto block_8017BC90;
            case 0x8017BCA4u: goto block_8017BCA4;
            case 0x8017BCB0u: goto block_8017BCB0;
            case 0x8017BCBCu: goto block_8017BCBC;
            case 0x8017BCC8u: goto block_8017BCC8;
            case 0x8017BCD4u: goto block_8017BCD4;
            case 0x8017BCE0u: goto block_8017BCE0;
            case 0x8017BCE8u: goto block_8017BCE8;
            case 0x8017BCFCu: goto block_8017BCFC;
            case 0x8017BD10u: goto block_8017BD10;
            case 0x8017BD24u: goto block_8017BD24;
            case 0x8017BD30u: goto block_8017BD30;
            case 0x8017BD38u: goto block_8017BD38;
            case 0x8017BD4Cu: goto block_8017BD4C;
            case 0x8017BD60u: goto block_8017BD60;
            case 0x8017BD74u: goto block_8017BD74;
            case 0x8017BD80u: goto block_8017BD80;
            case 0x8017BD84u: goto block_8017BD84;
            default: break;
        }
    }
    /* Address: 0x8017BAD4, Size: 696 bytes, Blocks: 42 */

block_8017BAD4:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017BAD8: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 8;
    cpu->gpr[3] = cpu->gpr[2] & 0x003F;  /* 0x8017BAE0: 0x3043003F */
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)35) ? 1 : 0;  /* 0x8017BAE4: 0x2C620023 */
    int _bc_8017BAE8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8017BAE8) {
        psx_check_interrupts_at(cpu, 0x8017BD84u);
        goto block_8017BD84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BAF0u);
        goto block_8017BAF0;  /* not taken */
    }

block_8017BAF0:
    cpu->gpr[2] = cpu->gpr[2] + 10332;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8017BB00: 0x00000000 */
    uint32_t _jt_8017BB04 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017BB08: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017BB04);
    cpu->pc = _jt_8017BB04; return;  /* CPS: jr cpu->gpr[2] */

block_8017BB0C:
    cpu->gpr[3] = cpu->read_half(cpu->gpr[7] + 134);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[7] + 5);
    cpu->write_word(cpu->gpr[7] + 88, cpu->gpr[4]);
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017BB18: 0x30420001 */
    int _bc_8017BB1C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[7] + 136, (uint16_t)cpu->gpr[3]);
    if (_bc_8017BB1C) {
        psx_check_interrupts_at(cpu, 0x8017BD84u);
        goto block_8017BD84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BB24u);
        goto block_8017BB24;  /* not taken */
    }

block_8017BB24:
    cpu->gpr[2] = cpu->gpr[4];
    cpu->gpr[3] = 0xC000 << 16;  /* 0xC0000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017BB2C: 0x00431024 */
    cpu->gpr[3] = 0x8000 << 16;  /* 0x80000000 */
    int _bc_8017BB34 = (cpu->gpr[2] != cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 190;
    if (_bc_8017BB34) {
        psx_check_interrupts_at(cpu, 0x8017BD84u);
        goto block_8017BD84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BB3Cu);
        goto block_8017BB3C;  /* not taken */
    }

block_8017BB3C:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 128);
    /* nop */  /* 0x8017BB40: 0x00000000 */
    int _bc_8017BB44 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017BB48: 0x00000000 */
    if (_bc_8017BB44) {
        psx_check_interrupts_at(cpu, 0x8017BD84u);
        goto block_8017BD84;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017BB4Cu);
        goto block_8017BB4C;  /* not taken */
    }

block_8017BB4C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[7] + 252);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[7] + 136, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BB58:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 96, cpu->gpr[4]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BB60:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 92, cpu->gpr[4]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BB68:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 100, cpu->gpr[4]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BB70:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 104, cpu->gpr[4]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BB78:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 88);
    /* nop */  /* 0x8017BB7C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BB80: 0x00441024 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 88, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BB8C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 96);
    /* nop */  /* 0x8017BB90: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BB94: 0x00441024 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 96, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BBA0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 92);
    /* nop */  /* 0x8017BBA4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BBA8: 0x00441024 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 92, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BBB4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 100);
    /* nop */  /* 0x8017BBB8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BBBC: 0x00441024 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 100, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BBC8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 104);
    /* nop */  /* 0x8017BBCC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BBD0: 0x00441024 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 104, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BBDC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 88);
    /* nop */  /* 0x8017BBE0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 88, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BBF0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 96);
    /* nop */  /* 0x8017BBF4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 96, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC04:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 92);
    /* nop */  /* 0x8017BC08: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 92, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC18:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 100);
    /* nop */  /* 0x8017BC1C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 100, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC2C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 104);
    /* nop */  /* 0x8017BC30: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 104, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC40:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 88);
    /* nop */  /* 0x8017BC44: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[4];  /* 0x8017BC48: 0x00441026 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 88, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC54:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 96);
    /* nop */  /* 0x8017BC58: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[4];  /* 0x8017BC5C: 0x00441026 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 96, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC68:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 92);
    /* nop */  /* 0x8017BC6C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[4];  /* 0x8017BC70: 0x00441026 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 92, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC7C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 100);
    /* nop */  /* 0x8017BC80: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[4];  /* 0x8017BC84: 0x00441026 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 100, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BC90:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 104);
    /* nop */  /* 0x8017BC94: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[4];  /* 0x8017BC98: 0x00441026 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 104, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BCA4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 88);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BCAC: 0x00441024 */
    psx_check_interrupts_at(cpu, 0x8017BD80u);
    goto block_8017BD80;  /* j */

block_8017BCB0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 96);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BCB8: 0x00441024 */
    psx_check_interrupts_at(cpu, 0x8017BD80u);
    goto block_8017BD80;  /* j */

block_8017BCBC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 92);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BCC4: 0x00441024 */
    psx_check_interrupts_at(cpu, 0x8017BD80u);
    goto block_8017BD80;  /* j */

block_8017BCC8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 100);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BCD0: 0x00441024 */
    psx_check_interrupts_at(cpu, 0x8017BD80u);
    goto block_8017BD80;  /* j */

block_8017BCD4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 104);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BCDC: 0x00441024 */
    psx_check_interrupts_at(cpu, 0x8017BD80u);
    goto block_8017BD80;  /* j */

block_8017BCE0:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 108, cpu->gpr[4]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BCE8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 108);
    /* nop */  /* 0x8017BCEC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BCF0: 0x00441024 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 108, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BCFC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 108);
    /* nop */  /* 0x8017BD00: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 108, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BD10:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 108);
    /* nop */  /* 0x8017BD14: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[4];  /* 0x8017BD18: 0x00441026 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 108, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BD24:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 108);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BD2C: 0x00441024 */
    psx_check_interrupts_at(cpu, 0x8017BD80u);
    goto block_8017BD80;  /* j */

block_8017BD30:
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 84, cpu->gpr[4]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BD38:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 84);
    /* nop */  /* 0x8017BD3C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BD40: 0x00441024 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 84, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BD4C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 84);
    /* nop */  /* 0x8017BD50: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[4];
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 84, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BD60:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 84);
    /* nop */  /* 0x8017BD64: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] ^ cpu->gpr[4];  /* 0x8017BD68: 0x00441026 */
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[7] + 84, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017BD84u);
    goto block_8017BD84;  /* j */

block_8017BD74:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 84);
    /* nop */  /* 0x8017BD78: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[4];  /* 0x8017BD7C: 0x00441024 */
    /* fall through to block_8017BD80 */

block_8017BD80:
    cpu->write_word(cpu->gpr[5] + 148, cpu->gpr[2]);
    /* fall through to block_8017BD84 */

block_8017BD84:
    uint32_t _jt_8017BD84 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    psx_check_interrupts_at(cpu, _jt_8017BD84);
    cpu->pc = _jt_8017BD84; return;  /* CPS: jr $ra */
}

/* cps_8017bd8c — clean CPS transliteration of br2eu_func_8017BD8C (guest 0x8017bd8c)
 * from SLES_017.22_full_35.c. */
void cps_8017bd8c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017BD8C, Size: 8 bytes, Blocks: 1 */

block_8017BD8C:
    uint32_t _jt_8017BD8C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 2;
    psx_check_interrupts_at(cpu, _jt_8017BD8C);
    cpu->pc = _jt_8017BD8C; return;  /* CPS: jr $ra */
}

/* cps_8017c35c — clean CPS transliteration of br2eu_func_8017C35C (guest 0x8017c35c)
 * from SLES_017.22_full_35.c. */
void cps_8017c35c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C384u: goto block_8017C384;
            case 0x8017C390u: goto block_8017C390;
            case 0x8017C398u: goto block_8017C398;
            case 0x8017C3A0u: goto block_8017C3A0;
            default: break;
        }
    }
    /* Address: 0x8017C35C, Size: 88 bytes, Blocks: 5 */

block_8017C35C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6]);
    cpu->gpr[8] = cpu->gpr[2] & 0x00FF;  /* 0x8017C364: 0x304800FF */
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[9] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 28;
    cpu->gpr[2] = 1;
    int _bc_8017C37C = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[4];
    if (_bc_8017C37C) {
        psx_check_interrupts_at(cpu, 0x8017C3A0u);
        goto block_8017C3A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C384u);
        goto block_8017C384;  /* not taken */
    }

block_8017C384:
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017C384: 0x28620002 */
    int _bc_8017C388 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017C388) {
        psx_check_interrupts_at(cpu, 0x8017C3D0u);
        cpu->pc = 0x8017C3D0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C390u);
        goto block_8017C390;  /* not taken */
    }

block_8017C390:
    int _bc_8017C390 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C394: 0x00000000 */
    if (_bc_8017C390) {
        psx_check_interrupts_at(cpu, 0x8017C3BCu);
        cpu->pc = 0x8017C3BCu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C398u);
        goto block_8017C398;  /* not taken */
    }

block_8017C398:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C39C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017C3D0u);
    cpu->pc = 0x8017C3D0u; return;  /* CPS j: split */

block_8017C3A0:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[7] + 5);
    /* nop */  /* 0x8017C3A4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017C3A8: 0x30420001 */
    int _bc_8017C3AC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C3B0: 0x00000000 */
    if (_bc_8017C3AC) {
        psx_check_interrupts_at(cpu, 0x8017C3D0u);
        cpu->pc = 0x8017C3D0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C3B4u);
        cpu->pc = 0x8017C3B4u; return;  /* CPS not taken: split */
    }
}

/* cps_8017c3b4 — clean CPS transliteration of br2eu_func_8017C3B4 (guest 0x8017c3b4)
 * from SLES_017.22_full_35.c. */
void cps_8017c3b4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C3B4, Size: 8 bytes, Blocks: 1 */

block_8017C3B4:
    uint32_t _jt_8017C3B4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C3B4);
    cpu->pc = _jt_8017C3B4; return;  /* CPS: jr $ra */
}

/* cps_8017c3bc — clean CPS transliteration of br2eu_func_8017C3BC (guest 0x8017c3bc)
 * from SLES_017.22_full_35.c. */
void cps_8017c3bc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C3BC, Size: 20 bytes, Blocks: 1 */

block_8017C3BC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[7] + 5);
    /* nop */  /* 0x8017C3C0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017C3C4: 0x30420001 */
    int _bc_8017C3C8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C3CC: 0x00000000 */
    if (_bc_8017C3C8) {
        psx_check_interrupts_at(cpu, 0x8017C3B4u);
        cpu->pc = 0x8017C3B4u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C3D0u);
        cpu->pc = 0x8017C3D0u; return;  /* CPS not taken: split */
    }
    cps_8017c3d0();  /* fallthrough to next function */
}

/* cps_8017c3d0 — clean CPS transliteration of br2eu_func_8017C3D0 (guest 0x8017c3d0)
 * from SLES_017.22_full_35.c. */
void cps_8017c3d0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C3ECu: goto block_8017C3EC;
            case 0x8017C3F4u: goto block_8017C3F4;
            case 0x8017C3FCu: goto block_8017C3FC;
            case 0x8017C404u: goto block_8017C404;
            case 0x8017C40Cu: goto block_8017C40C;
            case 0x8017C414u: goto block_8017C414;
            case 0x8017C41Cu: goto block_8017C41C;
            case 0x8017C438u: goto block_8017C438;
            case 0x8017C44Cu: goto block_8017C44C;
            case 0x8017C460u: goto block_8017C460;
            case 0x8017C478u: goto block_8017C478;
            case 0x8017C4A0u: goto block_8017C4A0;
            case 0x8017C4A8u: goto block_8017C4A8;
            case 0x8017C4CCu: goto block_8017C4CC;
            case 0x8017C4E0u: goto block_8017C4E0;
            case 0x8017C4F4u: goto block_8017C4F4;
            case 0x8017C50Cu: goto block_8017C50C;
            case 0x8017C518u: goto block_8017C518;
            case 0x8017C520u: goto block_8017C520;
            case 0x8017C528u: goto block_8017C528;
            case 0x8017C534u: goto block_8017C534;
            case 0x8017C540u: goto block_8017C540;
            case 0x8017C548u: goto block_8017C548;
            case 0x8017C554u: goto block_8017C554;
            case 0x8017C560u: goto block_8017C560;
            case 0x8017C56Cu: goto block_8017C56C;
            case 0x8017C580u: goto block_8017C580;
            case 0x8017C58Cu: goto block_8017C58C;
            case 0x8017C598u: goto block_8017C598;
            case 0x8017C5A4u: goto block_8017C5A4;
            case 0x8017C5ACu: goto block_8017C5AC;
            case 0x8017C5B0u: goto block_8017C5B0;
            default: break;
        }
    }
    /* Address: 0x8017C3D0, Size: 488 bytes, Blocks: 33 */

block_8017C3D0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017C3D4: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 8;
    cpu->gpr[3] = cpu->gpr[2] & 0x000F;  /* 0x8017C3DC: 0x3043000F */
    cpu->gpr[2] = 1;
    int _bc_8017C3E4 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017C3E8: 0x28620002 */
    if (_bc_8017C3E4) {
        psx_check_interrupts_at(cpu, 0x8017C41Cu);
        goto block_8017C41C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C3ECu);
        goto block_8017C3EC;  /* not taken */
    }

block_8017C3EC:
    int _bc_8017C3EC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017C3EC) {
        psx_check_interrupts_at(cpu, 0x8017C404u);
        goto block_8017C404;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C3F4u);
        goto block_8017C3F4;  /* not taken */
    }

block_8017C3F4:
    int _bc_8017C3F4 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C3F8: 0x00000000 */
    if (_bc_8017C3F4) {
        psx_check_interrupts_at(cpu, 0x8017C4A0u);
        goto block_8017C4A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C3FCu);
        goto block_8017C3FC;  /* not taken */
    }

block_8017C3FC:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C400: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017C4A8u);
    goto block_8017C4A8;  /* j */

block_8017C404:
    int _bc_8017C404 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 3;
    if (_bc_8017C404) {
        psx_check_interrupts_at(cpu, 0x8017C41Cu);
        goto block_8017C41C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C40Cu);
        goto block_8017C40C;  /* not taken */
    }

block_8017C40C:
    int _bc_8017C40C = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C410: 0x00000000 */
    if (_bc_8017C40C) {
        psx_check_interrupts_at(cpu, 0x8017C438u);
        goto block_8017C438;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C414u);
        goto block_8017C414;  /* not taken */
    }

block_8017C414:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C418: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017C4A8u);
    goto block_8017C4A8;  /* j */

block_8017C41C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[7] + 218);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 220);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[8];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[9];
    cpu->write_byte(cpu->gpr[7] + 218, (uint8_t)cpu->gpr[2]);
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[7] + 220, (uint8_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x8017C4A8u);
    goto block_8017C4A8;  /* j */

block_8017C438:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 108);
    /* nop */  /* 0x8017C43C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0100;  /* 0x8017C440: 0x30420100 */
    int _bc_8017C444 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x0001 << 16;  /* 0x00010000 */
    if (_bc_8017C444) {
        psx_check_interrupts_at(cpu, 0x8017C460u);
        goto block_8017C460;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C44Cu);
        goto block_8017C44C;  /* not taken */
    }

block_8017C44C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[7] + 214);
    cpu->gpr[3] = cpu->gpr[3] | 0x2BFB;
    cpu->gpr[2] = cpu->gpr[2] << 1;
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017C478u);
    goto block_8017C478;  /* j */

block_8017C460:
    cpu->gpr[4] = 0x0001 << 16;  /* 0x00010000 */
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[7] + 214);
    cpu->gpr[4] = cpu->gpr[4] | 0xA3F9;
    cpu->gpr[2] = cpu->gpr[3] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    /* fall through to block_8017C478 */

block_8017C478:
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[8]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = 0xDA77 << 16;  /* 0xDA770000 */
    cpu->gpr[3] = cpu->gpr[3] | 0xB1C9;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[3]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[3] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 17;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 31;
    cpu->gpr[8] = cpu->gpr[3] - cpu->gpr[2];
    /* fall through to block_8017C4A0 */

block_8017C4A0:
    cpu->write_byte(cpu->gpr[7] + 218, (uint8_t)cpu->gpr[8]);
    cpu->write_byte(cpu->gpr[7] + 220, (uint8_t)cpu->gpr[9]);
    /* fall through to block_8017C4A8 */

block_8017C4A8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[5] + 58);
    /* nop */  /* 0x8017C4AC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[8];
    cpu->write_half(cpu->gpr[5] + 58, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 100);
    cpu->gpr[3] = 0x1000 << 16;  /* 0x10000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017C4C0: 0x00431024 */
    int _bc_8017C4C4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C4C8: 0x00000000 */
    if (_bc_8017C4C4) {
        psx_check_interrupts_at(cpu, 0x8017C4E0u);
        goto block_8017C4E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C4CCu);
        goto block_8017C4CC;  /* not taken */
    }

block_8017C4CC:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 44);
    /* nop */  /* 0x8017C4D0: 0x00000000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 56);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[2] + 274, (uint16_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x8017C4F4u);
    goto block_8017C4F4;  /* j */

block_8017C4E0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 44);
    /* nop */  /* 0x8017C4E4: 0x00000000 */
    cpu->gpr[3] = cpu->read_word(cpu->gpr[2] + 56);
    cpu->gpr[2] = 64;
    cpu->write_half(cpu->gpr[3] + 274, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017C4F4 */

block_8017C4F4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[7] + 100);
    /* nop */  /* 0x8017C4F8: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[2] & 0x0038;  /* 0x8017C4FC: 0x30430038 */
    cpu->gpr[2] = 8;
    int _bc_8017C504 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C508: 0x00000000 */
    if (_bc_8017C504) {
        psx_check_interrupts_at(cpu, 0x8017C5A4u);
        goto block_8017C5A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C50Cu);
        goto block_8017C50C;  /* not taken */
    }

block_8017C50C:
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 9) ? 1 : 0;  /* 0x8017C50C: 0x28620009 */
    int _bc_8017C510 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C514: 0x00000000 */
    if (_bc_8017C510) {
        psx_check_interrupts_at(cpu, 0x8017C528u);
        goto block_8017C528;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C518u);
        goto block_8017C518;  /* not taken */
    }

block_8017C518:
    int _bc_8017C518 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C51C: 0x00000000 */
    if (_bc_8017C518) {
        psx_check_interrupts_at(cpu, 0x8017C560u);
        goto block_8017C560;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C520u);
        goto block_8017C520;  /* not taken */
    }

block_8017C520:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C524: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017C56Cu);
    goto block_8017C56C;  /* j */

block_8017C528:
    cpu->gpr[2] = 16;
    int _bc_8017C52C = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C530: 0x00000000 */
    if (_bc_8017C52C) {
        psx_check_interrupts_at(cpu, 0x8017C548u);
        goto block_8017C548;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C534u);
        goto block_8017C534;  /* not taken */
    }

block_8017C534:
    cpu->gpr[2] = 32;
    int _bc_8017C538 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C53C: 0x00000000 */
    if (_bc_8017C538) {
        psx_check_interrupts_at(cpu, 0x8017C554u);
        goto block_8017C554;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C540u);
        goto block_8017C540;  /* not taken */
    }

block_8017C540:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C544: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017C56Cu);
    goto block_8017C56C;  /* j */

block_8017C548:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[7] + 56);
    /* delay slot (always executes) */
    cpu->gpr[2] = 76;
    psx_check_interrupts_at(cpu, 0x8017C5ACu);
    goto block_8017C5AC;  /* j */

block_8017C554:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[7] + 56);
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    psx_check_interrupts_at(cpu, 0x8017C5ACu);
    goto block_8017C5AC;  /* j */

block_8017C560:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[7] + 56);
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    psx_check_interrupts_at(cpu, 0x8017C5ACu);
    goto block_8017C5AC;  /* j */

block_8017C56C:
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 218);
    /* nop */  /* 0x8017C570: 0x00000000 */
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)26) ? 1 : 0;  /* 0x8017C574: 0x2C62001A */
    int _bc_8017C578 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    if (_bc_8017C578) {
        psx_check_interrupts_at(cpu, 0x8017C58Cu);
        goto block_8017C58C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C580u);
        goto block_8017C580;  /* not taken */
    }

block_8017C580:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[7] + 56);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[3] + 276, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017C5B0u);
    goto block_8017C5B0;  /* j */

block_8017C58C:
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)21) ? 1 : 0;  /* 0x8017C58C: 0x2C620015 */
    int _bc_8017C590 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C594: 0x00000000 */
    if (_bc_8017C590) {
        psx_check_interrupts_at(cpu, 0x8017C5A4u);
        goto block_8017C5A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C598u);
        goto block_8017C598;  /* not taken */
    }

block_8017C598:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[7] + 56);
    /* delay slot (always executes) */
    cpu->gpr[2] = 76;
    psx_check_interrupts_at(cpu, 0x8017C5ACu);
    goto block_8017C5AC;  /* j */

block_8017C5A4:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[7] + 56);
    cpu->gpr[2] = 75;
    /* fall through to block_8017C5AC */

block_8017C5AC:
    cpu->write_half(cpu->gpr[3] + 276, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017C5B0 */

block_8017C5B0:
    uint32_t _jt_8017C5B0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C5B0);
    cpu->pc = _jt_8017C5B0; return;  /* CPS: jr $ra */
}

/* cps_8017c5b8 — clean CPS transliteration of br2eu_func_8017C5B8 (guest 0x8017c5b8)
 * from SLES_017.22_full_35.c. */
void cps_8017c5b8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C5D4u: goto block_8017C5D4;
            case 0x8017C5DCu: goto block_8017C5DC;
            case 0x8017C5E4u: goto block_8017C5E4;
            case 0x8017C5ECu: goto block_8017C5EC;
            default: break;
        }
    }
    /* Address: 0x8017C5B8, Size: 72 bytes, Blocks: 5 */

block_8017C5B8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017C5BC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 28;
    cpu->gpr[2] = 1;
    int _bc_8017C5CC = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017C5D0: 0x28620002 */
    if (_bc_8017C5CC) {
        psx_check_interrupts_at(cpu, 0x8017C5ECu);
        goto block_8017C5EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C5D4u);
        goto block_8017C5D4;  /* not taken */
    }

block_8017C5D4:
    int _bc_8017C5D4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017C5D4) {
        psx_check_interrupts_at(cpu, 0x8017C61Cu);
        cpu->pc = 0x8017C61Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C5DCu);
        goto block_8017C5DC;  /* not taken */
    }

block_8017C5DC:
    int _bc_8017C5DC = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C5E0: 0x00000000 */
    if (_bc_8017C5DC) {
        psx_check_interrupts_at(cpu, 0x8017C608u);
        cpu->pc = 0x8017C608u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C5E4u);
        goto block_8017C5E4;  /* not taken */
    }

block_8017C5E4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C5E8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017C61Cu);
    cpu->pc = 0x8017C61Cu; return;  /* CPS j: split */

block_8017C5EC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017C5F0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017C5F4: 0x30420001 */
    int _bc_8017C5F8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C5FC: 0x00000000 */
    if (_bc_8017C5F8) {
        psx_check_interrupts_at(cpu, 0x8017C61Cu);
        cpu->pc = 0x8017C61Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C600u);
        cpu->pc = 0x8017C600u; return;  /* CPS not taken: split */
    }
}

/* cps_8017c600 — clean CPS transliteration of br2eu_func_8017C600 (guest 0x8017c600)
 * from SLES_017.22_full_35.c. */
void cps_8017c600(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C600, Size: 8 bytes, Blocks: 1 */

block_8017C600:
    uint32_t _jt_8017C600 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C600);
    cpu->pc = _jt_8017C600; return;  /* CPS: jr $ra */
}

/* cps_8017c608 — clean CPS transliteration of br2eu_func_8017C608 (guest 0x8017c608)
 * from SLES_017.22_full_35.c. */
void cps_8017c608(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C608, Size: 20 bytes, Blocks: 1 */

block_8017C608:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017C60C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017C610: 0x30420001 */
    int _bc_8017C614 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C618: 0x00000000 */
    if (_bc_8017C614) {
        psx_check_interrupts_at(cpu, 0x8017C600u);
        cpu->pc = 0x8017C600u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C61Cu);
        cpu->pc = 0x8017C61Cu; return;  /* CPS not taken: split */
    }
    cps_8017c61c();  /* fallthrough to next function */
}

/* cps_8017c61c — clean CPS transliteration of br2eu_func_8017C61C (guest 0x8017c61c)
 * from SLES_017.22_full_35.c. */
void cps_8017c61c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C638u: goto block_8017C638;
            case 0x8017C650u: goto block_8017C650;
            case 0x8017C660u: goto block_8017C660;
            default: break;
        }
    }
    /* Address: 0x8017C61C, Size: 80 bytes, Blocks: 4 */

block_8017C61C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017C620: 0x00000000 */
    cpu->write_byte(cpu->gpr[4] + 219, (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5] + 256);
    /* nop */  /* 0x8017C62C: 0x00000000 */
    int _bc_8017C630 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C634: 0x00000000 */
    if (_bc_8017C630) {
        psx_check_interrupts_at(cpu, 0x8017C650u);
        goto block_8017C650;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C638u);
        goto block_8017C638;  /* not taken */
    }

block_8017C638:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017C63C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8017C660u);
    goto block_8017C660;  /* j */

block_8017C650:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017C654: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    /* fall through to block_8017C660 */

block_8017C660:
    cpu->write_half(cpu->gpr[4] + 198, (uint16_t)cpu->gpr[2]);
    uint32_t _jt_8017C664 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C664);
    cpu->pc = _jt_8017C664; return;  /* CPS: jr $ra */
}

/* cps_8017c7b0 — clean CPS transliteration of br2eu_func_8017C7B0 (guest 0x8017c7b0)
 * from SLES_017.22_full_35.c. */
void cps_8017c7b0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C7C8u: goto block_8017C7C8;
            case 0x8017C7DCu: goto block_8017C7DC;
            case 0x8017C7E0u: goto block_8017C7E0;
            case 0x8017C800u: goto block_8017C800;
            case 0x8017C804u: goto block_8017C804;
            case 0x8017C810u: goto block_8017C810;
            default: break;
        }
    }
    /* Address: 0x8017C7B0, Size: 104 bytes, Blocks: 7 */

block_8017C7B0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017C7B4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    int _bc_8017C7C0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C7C4: 0x00000000 */
    if (_bc_8017C7C0) {
        psx_check_interrupts_at(cpu, 0x8017C840u);
        cpu->pc = 0x8017C840u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C7C8u);
        goto block_8017C7C8;  /* not taken */
    }

block_8017C7C8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 92);
    /* nop */  /* 0x8017C7CC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0040;  /* 0x8017C7D0: 0x30420040 */
    int _bc_8017C7D4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 1024;
    if (_bc_8017C7D4) {
        psx_check_interrupts_at(cpu, 0x8017C7E0u);
        goto block_8017C7E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C7DCu);
        goto block_8017C7DC;  /* not taken */
    }

block_8017C7DC:
    cpu->gpr[7] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* fall through to block_8017C7E0 */

block_8017C7E0:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 116);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 210);
    /* nop */  /* 0x8017C7E8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 20;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    int _bc_8017C7F8 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C7FC: 0x00000000 */
    if (_bc_8017C7F8) {
        psx_check_interrupts_at(cpu, 0x8017C804u);
        goto block_8017C804;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C800u);
        goto block_8017C800;  /* not taken */
    }

block_8017C800:
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    /* fall through to block_8017C804 */

block_8017C804:
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[7]) ? 1 : 0;  /* 0x8017C804: 0x0047102A */
    int _bc_8017C808 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0xB40B << 16;  /* 0xB40B0000 */
    if (_bc_8017C808) {
        psx_check_interrupts_at(cpu, 0x8017C818u);
        cpu->pc = 0x8017C818u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C810u);
        goto block_8017C810;  /* not taken */
    }

block_8017C810:
    uint32_t _jt_8017C810 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C810);
    cpu->pc = _jt_8017C810; return;  /* CPS: jr $ra */
}

/* cps_8017c818 — clean CPS transliteration of br2eu_func_8017C818 (guest 0x8017c818)
 * from SLES_017.22_full_35.c. */
void cps_8017c818(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C818, Size: 40 bytes, Blocks: 1 */

block_8017C818:
    cpu->gpr[2] = cpu->gpr[2] | 0x40B5;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[7] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[7]));
    cpu->gpr[3] = (int32_t)cpu->gpr[7] >> 31;
    cpu->write_half(cpu->gpr[5] + 268, (uint16_t)cpu->gpr[7]);
    cpu->gpr[8] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[7];
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 7;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 266, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017C850u);
    cpu->pc = 0x8017C850u; return;  /* CPS j: split */
    cps_8017c840();  /* fallthrough to next function */
}

/* cps_8017c840 — clean CPS transliteration of br2eu_func_8017C840 (guest 0x8017c840)
 * from SLES_017.22_full_35.c. */
void cps_8017c840(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C840, Size: 16 bytes, Blocks: 1 */

block_8017C840:
    cpu->write_half(cpu->gpr[5] + 266, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017C848: 0x00000000 */
    cpu->write_half(cpu->gpr[5] + 268, (uint16_t)cpu->gpr[2]);
    cps_8017c850(); return;  /* fallthrough to split piece */
    cps_8017c850();  /* fallthrough to next function */
}

/* cps_8017c850 — clean CPS transliteration of br2eu_func_8017C850 (guest 0x8017c850)
 * from SLES_017.22_full_35.c. */
void cps_8017c850(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C874u: goto block_8017C874;
            case 0x8017C884u: goto block_8017C884;
            case 0x8017C88Cu: goto block_8017C88C;
            default: break;
        }
    }
    /* Address: 0x8017C850, Size: 72 bytes, Blocks: 4 */

block_8017C850:
    cpu->gpr[3] = 0xF7FF << 16;  /* 0xF7FF0000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 92);
    cpu->gpr[3] = cpu->gpr[3] | 0xFFFF;
    cpu->gpr[3] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017C85C: 0x00431824 */
    cpu->write_word(cpu->gpr[4] + 92, cpu->gpr[3]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 266);
    /* nop */  /* 0x8017C868: 0x00000000 */
    int _bc_8017C86C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C870: 0x00000000 */
    if (_bc_8017C86C) {
        psx_check_interrupts_at(cpu, 0x8017C884u);
        goto block_8017C884;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C874u);
        goto block_8017C874;  /* not taken */
    }

block_8017C874:
    cpu->gpr[2] = 0xFFFD << 16;  /* 0xFFFD0000 */
    cpu->gpr[2] = cpu->gpr[2] | 0xFFFF;
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x8017C880: 0x00621024 */
    psx_check_interrupts_at(cpu, 0x8017C88Cu);
    goto block_8017C88C;  /* j */

block_8017C884:
    cpu->gpr[2] = 0x0002 << 16;  /* 0x00020000 */
    cpu->gpr[2] = cpu->gpr[3] | cpu->gpr[2];
    /* fall through to block_8017C88C */

block_8017C88C:
    cpu->write_word(cpu->gpr[4] + 92, cpu->gpr[2]);
    uint32_t _jt_8017C890 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C890);
    cpu->pc = _jt_8017C890; return;  /* CPS: jr $ra */
}

/* cps_8017c898 — clean CPS transliteration of br2eu_func_8017C898 (guest 0x8017c898)
 * from SLES_017.22_full_35.c. */
void cps_8017c898(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C8B0u: goto block_8017C8B0;
            case 0x8017C8C4u: goto block_8017C8C4;
            case 0x8017C8C8u: goto block_8017C8C8;
            case 0x8017C8E8u: goto block_8017C8E8;
            case 0x8017C8ECu: goto block_8017C8EC;
            case 0x8017C8F8u: goto block_8017C8F8;
            default: break;
        }
    }
    /* Address: 0x8017C898, Size: 104 bytes, Blocks: 7 */

block_8017C898:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017C89C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    int _bc_8017C8A8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C8AC: 0x00000000 */
    if (_bc_8017C8A8) {
        psx_check_interrupts_at(cpu, 0x8017C928u);
        cpu->pc = 0x8017C928u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C8B0u);
        goto block_8017C8B0;  /* not taken */
    }

block_8017C8B0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 92);
    /* nop */  /* 0x8017C8B4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0040;  /* 0x8017C8B8: 0x30420040 */
    int _bc_8017C8BC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = 1024;
    if (_bc_8017C8BC) {
        psx_check_interrupts_at(cpu, 0x8017C8C8u);
        goto block_8017C8C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C8C4u);
        goto block_8017C8C4;  /* not taken */
    }

block_8017C8C4:
    cpu->gpr[7] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* fall through to block_8017C8C8 */

block_8017C8C8:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 116);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 210);
    /* nop */  /* 0x8017C8D0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 20;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
    int _bc_8017C8E0 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C8E4: 0x00000000 */
    if (_bc_8017C8E0) {
        psx_check_interrupts_at(cpu, 0x8017C8ECu);
        goto block_8017C8EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C8E8u);
        goto block_8017C8E8;  /* not taken */
    }

block_8017C8E8:
    cpu->gpr[2] = cpu->gpr[0] - cpu->gpr[2];
    /* fall through to block_8017C8EC */

block_8017C8EC:
    cpu->gpr[2] = ((int32_t)cpu->gpr[7] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x8017C8EC: 0x00E2102A */
    int _bc_8017C8F0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0xB40B << 16;  /* 0xB40B0000 */
    if (_bc_8017C8F0) {
        psx_check_interrupts_at(cpu, 0x8017C900u);
        cpu->pc = 0x8017C900u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C8F8u);
        goto block_8017C8F8;  /* not taken */
    }

block_8017C8F8:
    uint32_t _jt_8017C8F8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C8F8);
    cpu->pc = _jt_8017C8F8; return;  /* CPS: jr $ra */
}

/* cps_8017c900 — clean CPS transliteration of br2eu_func_8017C900 (guest 0x8017c900)
 * from SLES_017.22_full_35.c. */
void cps_8017c900(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C900, Size: 40 bytes, Blocks: 1 */

block_8017C900:
    cpu->gpr[2] = cpu->gpr[2] | 0x40B5;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[7] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[7]));
    cpu->gpr[3] = (int32_t)cpu->gpr[7] >> 31;
    cpu->write_half(cpu->gpr[5] + 268, (uint16_t)cpu->gpr[7]);
    cpu->gpr[8] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[8] + cpu->gpr[7];
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 7;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 266, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017C938u);
    cpu->pc = 0x8017C938u; return;  /* CPS j: split */
    cps_8017c928();  /* fallthrough to next function */
}

/* cps_8017c928 — clean CPS transliteration of br2eu_func_8017C928 (guest 0x8017c928)
 * from SLES_017.22_full_35.c. */
void cps_8017c928(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C928, Size: 16 bytes, Blocks: 1 */

block_8017C928:
    cpu->write_half(cpu->gpr[5] + 266, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017C930: 0x00000000 */
    cpu->write_half(cpu->gpr[5] + 268, (uint16_t)cpu->gpr[2]);
    cps_8017c938(); return;  /* fallthrough to split piece */
    cps_8017c938();  /* fallthrough to next function */
}

/* cps_8017c938 — clean CPS transliteration of br2eu_func_8017C938 (guest 0x8017c938)
 * from SLES_017.22_full_35.c. */
void cps_8017c938(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017C958u: goto block_8017C958;
            case 0x8017C968u: goto block_8017C968;
            case 0x8017C970u: goto block_8017C970;
            default: break;
        }
    }
    /* Address: 0x8017C938, Size: 68 bytes, Blocks: 4 */

block_8017C938:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 92);
    cpu->gpr[3] = 0x0800 << 16;  /* 0x08000000 */
    cpu->gpr[3] = cpu->gpr[2] | cpu->gpr[3];
    cpu->write_word(cpu->gpr[4] + 92, cpu->gpr[3]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 266);
    /* nop */  /* 0x8017C94C: 0x00000000 */
    int _bc_8017C950 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017C954: 0x00000000 */
    if (_bc_8017C950) {
        psx_check_interrupts_at(cpu, 0x8017C968u);
        goto block_8017C968;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017C958u);
        goto block_8017C958;  /* not taken */
    }

block_8017C958:
    cpu->gpr[2] = 0xFFFD << 16;  /* 0xFFFD0000 */
    cpu->gpr[2] = cpu->gpr[2] | 0xFFFF;
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] & cpu->gpr[2];  /* 0x8017C964: 0x00621024 */
    psx_check_interrupts_at(cpu, 0x8017C970u);
    goto block_8017C970;  /* j */

block_8017C968:
    cpu->gpr[2] = 0x0002 << 16;  /* 0x00020000 */
    cpu->gpr[2] = cpu->gpr[3] | cpu->gpr[2];
    /* fall through to block_8017C970 */

block_8017C970:
    cpu->write_word(cpu->gpr[4] + 92, cpu->gpr[2]);
    uint32_t _jt_8017C974 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017C974);
    cpu->pc = _jt_8017C974; return;  /* CPS: jr $ra */
}

/* cps_8017c9bc — clean CPS transliteration of br2eu_func_8017C9BC (guest 0x8017c9bc)
 * from SLES_017.22_full_35.c. */
void cps_8017c9bc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017C9BC, Size: 24 bytes, Blocks: 1 */

block_8017C9BC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017C9C0: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 8;
    cpu->write_byte(cpu->gpr[4] + 39, (uint8_t)cpu->gpr[2]);
    uint32_t _jt_8017C9CC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 2;
    psx_check_interrupts_at(cpu, _jt_8017C9CC);
    cpu->pc = _jt_8017C9CC; return;  /* CPS: jr $ra */
}

/* cps_8017ca84 — clean CPS transliteration of br2eu_func_8017CA84 (guest 0x8017ca84)
 * from SLES_017.22_full_35.c. */
void cps_8017ca84(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017CAA0u: goto block_8017CAA0;
            case 0x8017CAA8u: goto block_8017CAA8;
            case 0x8017CAB4u: goto block_8017CAB4;
            case 0x8017CABCu: goto block_8017CABC;
            default: break;
        }
    }
    /* Address: 0x8017CA84, Size: 76 bytes, Blocks: 5 */

block_8017CA84:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017CA88: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 28;
    cpu->gpr[2] = 1;
    int _bc_8017CA98 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017CA9C: 0x28620002 */
    if (_bc_8017CA98) {
        psx_check_interrupts_at(cpu, 0x8017CABCu);
        goto block_8017CABC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CAA0u);
        goto block_8017CAA0;  /* not taken */
    }

block_8017CAA0:
    int _bc_8017CAA0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    if (_bc_8017CAA0) {
        psx_check_interrupts_at(cpu, 0x8017CAECu);
        cpu->pc = 0x8017CAECu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CAA8u);
        goto block_8017CAA8;  /* not taken */
    }

block_8017CAA8:
    cpu->gpr[2] = 2;
    int _bc_8017CAAC = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    if (_bc_8017CAAC) {
        psx_check_interrupts_at(cpu, 0x8017CAD8u);
        cpu->pc = 0x8017CAD8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CAB4u);
        goto block_8017CAB4;  /* not taken */
    }

block_8017CAB4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CAB8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017CAECu);
    cpu->pc = 0x8017CAECu; return;  /* CPS j: split */

block_8017CABC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017CAC0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017CAC4: 0x30420001 */
    int _bc_8017CAC8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    if (_bc_8017CAC8) {
        psx_check_interrupts_at(cpu, 0x8017CAECu);
        cpu->pc = 0x8017CAECu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CAD0u);
        cpu->pc = 0x8017CAD0u; return;  /* CPS not taken: split */
    }
}

/* cps_8017cad0 — clean CPS transliteration of br2eu_func_8017CAD0 (guest 0x8017cad0)
 * from SLES_017.22_full_35.c. */
void cps_8017cad0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017CAD0, Size: 8 bytes, Blocks: 1 */

block_8017CAD0:
    uint32_t _jt_8017CAD0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    psx_check_interrupts_at(cpu, _jt_8017CAD0);
    cpu->pc = _jt_8017CAD0; return;  /* CPS: jr $ra */
}

/* cps_8017cad8 — clean CPS transliteration of br2eu_func_8017CAD8 (guest 0x8017cad8)
 * from SLES_017.22_full_35.c. */
void cps_8017cad8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017CAD8, Size: 20 bytes, Blocks: 1 */

block_8017CAD8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017CADC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017CAE0: 0x30420001 */
    int _bc_8017CAE4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    if (_bc_8017CAE4) {
        psx_check_interrupts_at(cpu, 0x8017CAD0u);
        cpu->pc = 0x8017CAD0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CAECu);
        cpu->pc = 0x8017CAECu; return;  /* CPS not taken: split */
    }
    cps_8017caec();  /* fallthrough to next function */
}

/* cps_8017caec — clean CPS transliteration of br2eu_func_8017CAEC (guest 0x8017caec)
 * from SLES_017.22_full_35.c. */
void cps_8017caec(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017CAEC, Size: 8 bytes, Blocks: 1 */

block_8017CAEC:
    uint32_t _jt_8017CAEC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CAF0: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017CAEC);
    cpu->pc = _jt_8017CAEC; return;  /* CPS: jr $ra */
}

/* cps_8017caf4 — clean CPS transliteration of br2eu_func_8017CAF4 (guest 0x8017caf4)
 * from SLES_017.22_full_35.c. */
void cps_8017caf4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017CEB8u: goto block_8017CEB8;
            case 0x8017CEC4u: goto block_8017CEC4;
            case 0x8017CEE4u: goto block_8017CEE4;
            case 0x8017CEECu: goto block_8017CEEC;
            case 0x8017CEF4u: goto block_8017CEF4;
            case 0x8017CEFCu: goto block_8017CEFC;
            case 0x8017CB2Cu: goto block_8017CB2C;
            case 0x8017CB34u: goto block_8017CB34;
            case 0x8017CB3Cu: goto block_8017CB3C;
            case 0x8017CB44u: goto block_8017CB44;
            case 0x8017CB4Cu: goto block_8017CB4C;
            case 0x8017CB54u: goto block_8017CB54;
            case 0x8017CB5Cu: goto block_8017CB5C;
            case 0x8017CB88u: goto block_8017CB88;
            case 0x8017CB90u: goto block_8017CB90;
            case 0x8017CB94u: goto block_8017CB94;
            case 0x8017CBC8u: goto block_8017CBC8;
            case 0x8017CBD0u: goto block_8017CBD0;
            case 0x8017CC04u: goto block_8017CC04;
            case 0x8017CC0Cu: goto block_8017CC0C;
            case 0x8017CC2Cu: goto block_8017CC2C;
            case 0x8017CC34u: goto block_8017CC34;
            case 0x8017CC38u: goto block_8017CC38;
            case 0x8017CC74u: goto block_8017CC74;
            case 0x8017CC7Cu: goto block_8017CC7C;
            case 0x8017CCB0u: goto block_8017CCB0;
            case 0x8017CCB8u: goto block_8017CCB8;
            case 0x8017CCE0u: goto block_8017CCE0;
            case 0x8017CCECu: goto block_8017CCEC;
            case 0x8017CCF8u: goto block_8017CCF8;
            case 0x8017CD04u: goto block_8017CD04;
            case 0x8017CD0Cu: goto block_8017CD0C;
            case 0x8017CD20u: goto block_8017CD20;
            case 0x8017CD2Cu: goto block_8017CD2C;
            case 0x8017CD38u: goto block_8017CD38;
            case 0x8017CD44u: goto block_8017CD44;
            case 0x8017CD4Cu: goto block_8017CD4C;
            case 0x8017CD68u: goto block_8017CD68;
            case 0x8017CD70u: goto block_8017CD70;
            case 0x8017CD94u: goto block_8017CD94;
            case 0x8017CDA8u: goto block_8017CDA8;
            case 0x8017CDC8u: goto block_8017CDC8;
            case 0x8017CDDCu: goto block_8017CDDC;
            case 0x8017CE10u: goto block_8017CE10;
            case 0x8017CE28u: goto block_8017CE28;
            case 0x8017CE38u: goto block_8017CE38;
            case 0x8017CE40u: goto block_8017CE40;
            case 0x8017CE50u: goto block_8017CE50;
            case 0x8017CE64u: goto block_8017CE64;
            case 0x8017CE78u: goto block_8017CE78;
            case 0x8017CE98u: goto block_8017CE98;
            case 0x8017CEA8u: goto block_8017CEA8;
            case 0x8017CEACu: goto block_8017CEAC;
            case 0x8017CF00u: goto block_8017CF00;
            default: break;
        }
    }
    /* Address: 0x8017CAF4, Size: 1056 bytes, Blocks: 55 */

block_8017CAF4:
    cpu->gpr[29] = cpu->gpr[29] + -32;
    cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);
    cpu->gpr[16] = cpu->gpr[4];
    cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);
    cpu->gpr[17] = cpu->gpr[6];
    cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[31]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[17]);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[17] + 2);
    cpu->gpr[5] = cpu->read_word(cpu->gpr[16] + 44);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[2] = 1;
    int _bc_8017CB24 = (cpu->gpr[4] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < 2) ? 1 : 0;  /* 0x8017CB28: 0x28820002 */
    if (_bc_8017CB24) {
        psx_check_interrupts_at(cpu, 0x8017CC0Cu);
        goto block_8017CC0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CB2Cu);
        goto block_8017CB2C;  /* not taken */
    }

block_8017CB2C:
    int _bc_8017CB2C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017CB2C) {
        psx_check_interrupts_at(cpu, 0x8017CB44u);
        goto block_8017CB44;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CB34u);
        goto block_8017CB34;  /* not taken */
    }

block_8017CB34:
    int _bc_8017CB34 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CB38: 0x00000000 */
    if (_bc_8017CB34) {
        psx_check_interrupts_at(cpu, 0x8017CB5Cu);
        goto block_8017CB5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CB3Cu);
        goto block_8017CB3C;  /* not taken */
    }

block_8017CB3C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CB40: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017CE50u);
    goto block_8017CE50;  /* j */

block_8017CB44:
    int _bc_8017CB44 = (cpu->gpr[4] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 3;
    if (_bc_8017CB44) {
        psx_check_interrupts_at(cpu, 0x8017CCB8u);
        goto block_8017CCB8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CB4Cu);
        goto block_8017CB4C;  /* not taken */
    }

block_8017CB4C:
    int _bc_8017CB4C = (cpu->gpr[4] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] << 16;
    if (_bc_8017CB4C) {
        psx_check_interrupts_at(cpu, 0x8017CE28u);
        goto block_8017CE28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CB54u);
        goto block_8017CB54;  /* not taken */
    }

block_8017CB54:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CB58: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017CE50u);
    goto block_8017CE50;  /* j */

block_8017CB5C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 204);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 200);
    cpu->gpr[2] = cpu->gpr[2] + 32;
    cpu->write_half(cpu->gpr[16] + 204, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 172);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 176);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x8017CB78: 0x0062182A */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 200);
    int _bc_8017CB80 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CB84: 0x00000000 */
    if (_bc_8017CB80) {
        psx_check_interrupts_at(cpu, 0x8017CE50u);
        goto block_8017CE50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CB88u);
        goto block_8017CB88;  /* not taken */
    }

block_8017CB88:
    int _bc_8017CB88 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 0x6666 << 16;  /* 0x66660000 */
    if (_bc_8017CB88) {
        psx_check_interrupts_at(cpu, 0x8017CB94u);
        goto block_8017CB94;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CB90u);
        goto block_8017CB90;  /* not taken */
    }

block_8017CB90:
    cpu->write_half(cpu->gpr[16] + 202, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CB94 */

block_8017CB94:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 202);
    cpu->gpr[4] = cpu->gpr[4] | 0x6667;
    cpu->gpr[2] = cpu->gpr[3] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[4]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 31;
    cpu->gpr[7] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[3] = (int32_t)cpu->gpr[7] >> 2;
    cpu->gpr[4] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 192);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 192);
    int _bc_8017CBC0 = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] << 4;
    if (_bc_8017CBC0) {
        psx_check_interrupts_at(cpu, 0x8017CBD0u);
        goto block_8017CBD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CBC8u);
        goto block_8017CBC8;  /* not taken */
    }

block_8017CBC8:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    cpu->write_half(cpu->gpr[16] + 192, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CBD0 */

block_8017CBD0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 200);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 172);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[16] + 200, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 176);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 176);
    cpu->gpr[4] = ((int32_t)cpu->gpr[4] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x8017CBF4: 0x0083202A */
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 172);
    int _bc_8017CBFC = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    if (_bc_8017CBFC) {
        psx_check_interrupts_at(cpu, 0x8017CE50u);
        goto block_8017CE50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CC04u);
        goto block_8017CC04;  /* not taken */
    }

block_8017CC04:
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16] + 200, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017CE50u);
    goto block_8017CE50;  /* j */

block_8017CC0C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 172);
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 200);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 176);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x8017CC1C: 0x0062182A */
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 200);
    int _bc_8017CC24 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CC28: 0x00000000 */
    if (_bc_8017CC24) {
        psx_check_interrupts_at(cpu, 0x8017CE50u);
        goto block_8017CE50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CC2Cu);
        goto block_8017CC2C;  /* not taken */
    }

block_8017CC2C:
    int _bc_8017CC2C = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 0x6666 << 16;  /* 0x66660000 */
    if (_bc_8017CC2C) {
        psx_check_interrupts_at(cpu, 0x8017CC38u);
        goto block_8017CC38;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CC34u);
        goto block_8017CC34;  /* not taken */
    }

block_8017CC34:
    cpu->write_half(cpu->gpr[16] + 202, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CC38 */

block_8017CC38:
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 202);
    cpu->gpr[4] = cpu->gpr[4] | 0x6667;
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[3] >> 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[4]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 31;
    cpu->gpr[7] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = (int32_t)cpu->gpr[7] >> 2;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 192);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 192);
    int _bc_8017CC6C = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] << 4;
    if (_bc_8017CC6C) {
        psx_check_interrupts_at(cpu, 0x8017CC7Cu);
        goto block_8017CC7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CC74u);
        goto block_8017CC74;  /* not taken */
    }

block_8017CC74:
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    cpu->write_half(cpu->gpr[16] + 192, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CC7C */

block_8017CC7C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 200);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 172);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[16] + 200, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[4] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 176);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 176);
    cpu->gpr[4] = ((int32_t)cpu->gpr[4] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x8017CCA0: 0x0083202A */
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 172);
    int _bc_8017CCA8 = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    if (_bc_8017CCA8) {
        psx_check_interrupts_at(cpu, 0x8017CE50u);
        goto block_8017CE50;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CCB0u);
        goto block_8017CCB0;  /* not taken */
    }

block_8017CCB0:
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16] + 200, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017CE50u);
    goto block_8017CE50;  /* j */

block_8017CCB8:
    cpu->gpr[3] = cpu->gpr[3] << 16;
    cpu->gpr[4] = cpu->read_byte(cpu->gpr[5] + 218);
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 16;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[3]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5] + 5);
    /* nop */  /* 0x8017CCCC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x8017CCD0: 0x30420010 */
    cpu->gpr[4] = cpu->lo;
    psx_muldiv_stall(cpu);
    int _bc_8017CCD8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CCDC: 0x00000000 */
    if (_bc_8017CCD8) {
        psx_check_interrupts_at(cpu, 0x8017CCECu);
        goto block_8017CCEC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CCE0u);
        goto block_8017CCE0;  /* not taken */
    }

block_8017CCE0:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 256);
    /* delay slot (always executes) */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    psx_check_interrupts_at(cpu, 0x8017CCF8u);
    goto block_8017CCF8;  /* j */

block_8017CCEC:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 254);
    /* nop */  /* 0x8017CCF0: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    /* fall through to block_8017CCF8 */

block_8017CCF8:
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    int _bc_8017CCFC = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 7;
    if (_bc_8017CCFC) {
        psx_check_interrupts_at(cpu, 0x8017CD0Cu);
        goto block_8017CD0C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CD04u);
        goto block_8017CD04;  /* not taken */
    }

block_8017CD04:
    cpu->gpr[2] = cpu->gpr[2] + 127;
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 7;
    /* fall through to block_8017CD0C */

block_8017CD0C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[16] + 5);
    /* nop */  /* 0x8017CD10: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x8017CD14: 0x30420010 */
    int _bc_8017CD18 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CD1C: 0x00000000 */
    if (_bc_8017CD18) {
        psx_check_interrupts_at(cpu, 0x8017CD2Cu);
        goto block_8017CD2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CD20u);
        goto block_8017CD20;  /* not taken */
    }

block_8017CD20:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 260);
    /* delay slot (always executes) */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    psx_check_interrupts_at(cpu, 0x8017CD38u);
    goto block_8017CD38;  /* j */

block_8017CD2C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 258);
    /* nop */  /* 0x8017CD30: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    /* fall through to block_8017CD38 */

block_8017CD38:
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    int _bc_8017CD3C = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 7;
    if (_bc_8017CD3C) {
        psx_check_interrupts_at(cpu, 0x8017CD4Cu);
        goto block_8017CD4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CD44u);
        goto block_8017CD44;  /* not taken */
    }

block_8017CD44:
    cpu->gpr[2] = cpu->gpr[2] + 127;
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 7;
    /* fall through to block_8017CD4C */

block_8017CD4C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[16] + 262);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 192);
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[2] = cpu->gpr[4] & 0x007F;  /* 0x8017CD58: 0x3082007F */
    cpu->gpr[4] = (int32_t)cpu->gpr[4] >> 7;
    int _bc_8017CD60 = ((int32_t)cpu->gpr[3] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[16] + 262, (uint8_t)cpu->gpr[2]);
    if (_bc_8017CD60) {
        psx_check_interrupts_at(cpu, 0x8017CD70u);
        goto block_8017CD70;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CD68u);
        goto block_8017CD68;  /* not taken */
    }

block_8017CD68:
    cpu->gpr[2] = cpu->gpr[4] << 4;
    cpu->write_half(cpu->gpr[16] + 192, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CD70 */

block_8017CD70:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 200);
    cpu->write_half(cpu->gpr[16] + 204, (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[16] + 200, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[5] + 5);
    /* nop */  /* 0x8017CD84: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x8017CD88: 0x30420010 */
    int _bc_8017CD8C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CD90: 0x00000000 */
    if (_bc_8017CD8C) {
        psx_check_interrupts_at(cpu, 0x8017CDC8u);
        goto block_8017CDC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CD94u);
        goto block_8017CD94;  /* not taken */
    }

block_8017CD94:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[16] + 108);
    /* nop */  /* 0x8017CD98: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x8000;  /* 0x8017CD9C: 0x30428000 */
    int _bc_8017CDA0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (uint32_t)cpu->gpr[4] >> 31;
    if (_bc_8017CDA0) {
        psx_check_interrupts_at(cpu, 0x8017CDC8u);
        goto block_8017CDC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CDA8u);
        goto block_8017CDA8;  /* not taken */
    }

block_8017CDA8:
    cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;
    cpu->gpr[3] = cpu->gpr[2] << 4;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[5] + 216);
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->write_half(cpu->gpr[5] + 216, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CDC8 */

block_8017CDC8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[16] + 5);
    /* nop */  /* 0x8017CDCC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x8017CDD0: 0x30420010 */
    int _bc_8017CDD4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[4] << 4;
    if (_bc_8017CDD4) {
        psx_check_interrupts_at(cpu, 0x8017CE10u);
        goto block_8017CE10;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CDDCu);
        goto block_8017CDDC;  /* not taken */
    }

block_8017CDDC:
    cpu->gpr[2] = 0x5555 << 16;  /* 0x55550000 */
    cpu->gpr[2] = cpu->gpr[2] | 0x5556;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
    cpu->gpr[2] = (int32_t)cpu->gpr[4] >> 31;
    cpu->gpr[7] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[7] - cpu->gpr[2];
    cpu->gpr[3] = cpu->gpr[2] << 4;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 216);
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16] + 216, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017CE50u);
    goto block_8017CE50;  /* j */

block_8017CE10:
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[4];
    cpu->gpr[3] = cpu->read_half(cpu->gpr[16] + 216);
    cpu->gpr[2] = cpu->gpr[2] << 3;
    cpu->gpr[3] = cpu->gpr[3] - cpu->gpr[2];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16] + 216, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x8017CE50u);
    goto block_8017CE50;  /* j */

block_8017CE28:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 192);
    /* nop */  /* 0x8017CE2C: 0x00000000 */
    int _bc_8017CE30 = ((int32_t)cpu->gpr[3] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 16;
    if (_bc_8017CE30) {
        psx_check_interrupts_at(cpu, 0x8017CE40u);
        goto block_8017CE40;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CE38u);
        goto block_8017CE38;  /* not taken */
    }

block_8017CE38:
    cpu->gpr[2] = cpu->gpr[4] << 4;
    cpu->write_half(cpu->gpr[16] + 192, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CE40 */

block_8017CE40:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 200);
    cpu->write_half(cpu->gpr[16] + 204, (uint16_t)cpu->gpr[0]);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->write_half(cpu->gpr[16] + 200, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017CE50 */

block_8017CE50:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[16] + 5);
    /* nop */  /* 0x8017CE54: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017CE58: 0x30420001 */
    int _bc_8017CE5C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[17] + 4;
    if (_bc_8017CE5C) {
        psx_check_interrupts_at(cpu, 0x8017CF00u);
        goto block_8017CF00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CE64u);
        goto block_8017CE64;  /* not taken */
    }

block_8017CE64:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[16]);
    /* nop */  /* 0x8017CE68: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0080;  /* 0x8017CE6C: 0x30420080 */
    int _bc_8017CE70 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017CE74: 0x00000000 */
    if (_bc_8017CE70) {
        psx_check_interrupts_at(cpu, 0x8017CE98u);
        goto block_8017CE98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CE78u);
        goto block_8017CE78;  /* not taken */
    }

block_8017CE78:
    cpu->gpr[2] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 192);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[16] + 229);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + 4);
    /* nop */  /* 0x8017CE88: 0x00000000 */
    cpu->gpr[3] = (cpu->gpr[3] < cpu->gpr[2]) ? 1 : 0;  /* 0x8017CE8C: 0x0062182B */
    int _bc_8017CE90 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 1024;
    if (_bc_8017CE90) {
        psx_check_interrupts_at(cpu, 0x8017CEACu);
        goto block_8017CEAC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CE98u);
        goto block_8017CE98;  /* not taken */
    }

block_8017CE98:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[16] + 214);
    /* nop */  /* 0x8017CE9C: 0x00000000 */
    int _bc_8017CEA0 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[17] + 4;
    if (_bc_8017CEA0) {
        psx_check_interrupts_at(cpu, 0x8017CF00u);
        goto block_8017CF00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017CEA8u);
        goto block_8017CEA8;  /* not taken */
    }

block_8017CEA8:
    cpu->gpr[4] = 1024;
    /* fall through to block_8017CEAC */

block_8017CEAC:
    cpu->gpr[5] = 7;
    cpu->gpr[31] = 0x8017CEB8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[6] = cpu->gpr[0];
    psx_check_interrupts_at(cpu, 0x80156EF4u);
    cpu->pc = 0x80156EF4u; return;  /* CPS jal -> 0x80156EF4 */

block_8017CEB8:
    cpu->gpr[4] = 1;
    cpu->gpr[31] = 0x8017CEC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[5] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x80140CA0u);
    cpu->pc = 0x80140CA0u; return;  /* CPS jal -> 0x80140CA0 */

block_8017CEC4:
    cpu->gpr[4] = cpu->gpr[16];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[16] + 172);
    cpu->gpr[5] = cpu->gpr[0];
    cpu->write_half(cpu->gpr[16] + 214, (uint16_t)cpu->gpr[0]);
    cpu->write_half(cpu->gpr[16] + 216, (uint16_t)cpu->gpr[0]);
    cpu->write_byte(cpu->gpr[16] + 5, (uint8_t)cpu->gpr[0]);
    cpu->gpr[31] = 0x8017CEE4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[16] + 174, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x80175E08u);
    cpu->pc = 0x80175E08u; return;  /* CPS jal -> 0x80175E08 */

block_8017CEE4:
    cpu->gpr[31] = 0x8017CEECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x80142D20u);
    cpu->pc = 0x80142D20u; return;  /* CPS jal -> 0x80142D20 */

block_8017CEEC:
    cpu->gpr[31] = 0x8017CEF4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x80142DA0u);
    cpu->pc = 0x80142DA0u; return;  /* CPS jal -> 0x80142DA0 */

block_8017CEF4:
    cpu->gpr[31] = 0x8017CEFCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[16];
    psx_check_interrupts_at(cpu, 0x80142DD8u);
    cpu->pc = 0x80142DD8u; return;  /* CPS jal -> 0x80142DD8 */

block_8017CEFC:
    cpu->gpr[2] = cpu->gpr[17] + 4;
    /* fall through to block_8017CF00 */

block_8017CF00:
    cpu->gpr[31] = cpu->read_word(cpu->gpr[29] + 24);
    cpu->gpr[17] = cpu->read_word(cpu->gpr[29] + 20);
    cpu->gpr[16] = cpu->read_word(cpu->gpr[29] + 16);
    uint32_t _jt_8017CF0C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[29] = cpu->gpr[29] + 32;
    psx_check_interrupts_at(cpu, _jt_8017CF0C);
    cpu->pc = _jt_8017CF0C; return;  /* CPS: jr $ra */
}

/* cps_8017d1d0 — clean CPS transliteration of br2eu_func_8017D1D0 (guest 0x8017d1d0)
 * from SLES_017.22_full_35.c. */
void cps_8017d1d0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017D1F8u: goto block_8017D1F8;
            case 0x8017D1FCu: goto block_8017D1FC;
            case 0x8017D210u: goto block_8017D210;
            case 0x8017D218u: goto block_8017D218;
            case 0x8017D220u: goto block_8017D220;
            case 0x8017D228u: goto block_8017D228;
            default: break;
        }
    }
    /* Address: 0x8017D1D0, Size: 108 bytes, Blocks: 7 */

block_8017D1D0:
    cpu->gpr[5] = cpu->gpr[4];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017D1D8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[4] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[4] >> 16;
    cpu->gpr[2] = cpu->gpr[3] & 0x0040;  /* 0x8017D1EC: 0x30620040 */
    int _bc_8017D1F0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[7] = cpu->gpr[5];
    if (_bc_8017D1F0) {
        psx_check_interrupts_at(cpu, 0x8017D1FCu);
        goto block_8017D1FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D1F8u);
        goto block_8017D1F8;  /* not taken */
    }

block_8017D1F8:
    cpu->gpr[7] = cpu->read_word(cpu->gpr[5] + 44);
    /* fall through to block_8017D1FC */

block_8017D1FC:
    cpu->gpr[2] = (int32_t)cpu->gpr[4] >> 20;
    cpu->gpr[4] = cpu->gpr[2] & 0x0003;  /* 0x8017D200: 0x30440003 */
    cpu->gpr[2] = 1;
    int _bc_8017D208 = (cpu->gpr[4] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < 2) ? 1 : 0;  /* 0x8017D20C: 0x28820002 */
    if (_bc_8017D208) {
        psx_check_interrupts_at(cpu, 0x8017D228u);
        goto block_8017D228;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D210u);
        goto block_8017D210;  /* not taken */
    }

block_8017D210:
    int _bc_8017D210 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017D210) {
        psx_check_interrupts_at(cpu, 0x8017D258u);
        cpu->pc = 0x8017D258u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D218u);
        goto block_8017D218;  /* not taken */
    }

block_8017D218:
    int _bc_8017D218 = (cpu->gpr[4] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D21C: 0x00000000 */
    if (_bc_8017D218) {
        psx_check_interrupts_at(cpu, 0x8017D244u);
        cpu->pc = 0x8017D244u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D220u);
        goto block_8017D220;  /* not taken */
    }

block_8017D220:
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D224: 0x3063000F */
    psx_check_interrupts_at(cpu, 0x8017D25Cu);
    cpu->pc = 0x8017D25Cu; return;  /* CPS j: split */

block_8017D228:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[7] + 5);
    /* nop */  /* 0x8017D22C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017D230: 0x30420001 */
    int _bc_8017D234 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D238: 0x3063000F */
    if (_bc_8017D234) {
        psx_check_interrupts_at(cpu, 0x8017D25Cu);
        cpu->pc = 0x8017D25Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D23Cu);
        cpu->pc = 0x8017D23Cu; return;  /* CPS not taken: split */
    }
}

/* cps_8017d23c — clean CPS transliteration of br2eu_func_8017D23C (guest 0x8017d23c)
 * from SLES_017.22_full_35.c. */
void cps_8017d23c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D23C, Size: 8 bytes, Blocks: 1 */

block_8017D23C:
    uint32_t _jt_8017D23C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017D23C);
    cpu->pc = _jt_8017D23C; return;  /* CPS: jr $ra */
}

/* cps_8017d244 — clean CPS transliteration of br2eu_func_8017D244 (guest 0x8017d244)
 * from SLES_017.22_full_35.c. */
void cps_8017d244(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D244, Size: 20 bytes, Blocks: 1 */

block_8017D244:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[7] + 5);
    /* nop */  /* 0x8017D248: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017D24C: 0x30420001 */
    int _bc_8017D250 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D254: 0x00000000 */
    if (_bc_8017D250) {
        psx_check_interrupts_at(cpu, 0x8017D23Cu);
        cpu->pc = 0x8017D23Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D258u);
        cpu->pc = 0x8017D258u; return;  /* CPS not taken: split */
    }
    cps_8017d258();  /* fallthrough to next function */
}

/* cps_8017d258 — clean CPS transliteration of br2eu_func_8017D258 (guest 0x8017d258)
 * from SLES_017.22_full_35.c. */
void cps_8017d258(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D258, Size: 4 bytes, Blocks: 1 */

block_8017D258:
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D258: 0x3063000F */
    cps_8017d25c(); return;  /* fallthrough to split piece */
    cps_8017d25c();  /* fallthrough to next function */
}

/* cps_8017d25c — clean CPS transliteration of br2eu_func_8017D25C (guest 0x8017d25c)
 * from SLES_017.22_full_35.c. */
void cps_8017d25c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017D268u: goto block_8017D268;
            case 0x8017D284u: goto block_8017D284;
            case 0x8017D2A0u: goto block_8017D2A0;
            case 0x8017D2B8u: goto block_8017D2B8;
            case 0x8017D2D4u: goto block_8017D2D4;
            case 0x8017D2F0u: goto block_8017D2F0;
            case 0x8017D2F8u: goto block_8017D2F8;
            case 0x8017D2FCu: goto block_8017D2FC;
            case 0x8017D308u: goto block_8017D308;
            case 0x8017D314u: goto block_8017D314;
            case 0x8017D330u: goto block_8017D330;
            case 0x8017D344u: goto block_8017D344;
            case 0x8017D34Cu: goto block_8017D34C;
            default: break;
        }
    }
    /* Address: 0x8017D25C, Size: 248 bytes, Blocks: 14 */

block_8017D25C:
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)7) ? 1 : 0;  /* 0x8017D25C: 0x2C620007 */
    int _bc_8017D260 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8017D260) {
        psx_check_interrupts_at(cpu, 0x8017D34Cu);
        goto block_8017D34C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D268u);
        goto block_8017D268;  /* not taken */
    }

block_8017D268:
    cpu->gpr[2] = cpu->gpr[2] + 10596;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8017D278: 0x00000000 */
    uint32_t _jt_8017D27C = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D280: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017D27C);
    cpu->pc = _jt_8017D27C; return;  /* CPS: jr cpu->gpr[2] */

block_8017D284:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 178);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017D28C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[8] >> 7;
    psx_check_interrupts_at(cpu, 0x8017D344u);
    goto block_8017D344;  /* j */

block_8017D2A0:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[5] + 78);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017D2A8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 196, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017D34Cu);
    goto block_8017D34C;  /* j */

block_8017D2B8:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[5] + 78);
    cpu->gpr[4] = cpu->read_half(cpu->gpr[5] + 112);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 196, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x8017D34Cu);
    goto block_8017D34C;  /* j */

block_8017D2D4:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017D2D8: 0x00000000 */
    cpu->write_half(cpu->gpr[5] + 180, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 152);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 178);
    int _bc_8017D2E8 = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D2EC: 0x00000000 */
    if (_bc_8017D2E8) {
        psx_check_interrupts_at(cpu, 0x8017D2F8u);
        goto block_8017D2F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D2F0u);
        goto block_8017D2F0;  /* not taken */
    }

block_8017D2F0:
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8017D2FCu);
    goto block_8017D2FC;  /* j */

block_8017D2F8:
    cpu->gpr[2] = cpu->gpr[3] - cpu->gpr[2];
    /* fall through to block_8017D2FC */

block_8017D2FC:
    cpu->write_half(cpu->gpr[5] + 178, (uint16_t)cpu->gpr[2]);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 152, (uint16_t)cpu->gpr[0]);
    psx_check_interrupts_at(cpu, 0x8017D34Cu);
    goto block_8017D34C;  /* j */

block_8017D308:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 180, (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017D34Cu);
    goto block_8017D34C;  /* j */

block_8017D314:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 114);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017D31C: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[8] = cpu->lo;
    psx_muldiv_stall(cpu);
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[8] >> 7;
    psx_check_interrupts_at(cpu, 0x8017D344u);
    goto block_8017D344;  /* j */

block_8017D330:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 114);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->gpr[2] << 7;
    if (cpu->gpr[3] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[2] / (int32_t)cpu->gpr[3]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[2] % (int32_t)cpu->gpr[3]); } else { cpu->lo = ((int32_t)cpu->gpr[2] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[2]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[2] = cpu->lo;
    psx_muldiv_stall(cpu);
    /* fall through to block_8017D344 */

block_8017D344:
    /* nop */  /* 0x8017D344: 0x00000000 */
    cpu->write_half(cpu->gpr[5] + 178, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017D34C */

block_8017D34C:
    uint32_t _jt_8017D34C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017D34C);
    cpu->pc = _jt_8017D34C; return;  /* CPS: jr $ra */
}

/* cps_8017d354 — clean CPS transliteration of br2eu_func_8017D354 (guest 0x8017d354)
 * from SLES_017.22_full_35.c. */
void cps_8017d354(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D354, Size: 36 bytes, Blocks: 1 */

block_8017D354:
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = 0x8000;
    cpu->write_word(cpu->gpr[5] + 32, cpu->gpr[2]);
    cpu->gpr[2] = 4095;
    cpu->write_half(cpu->gpr[5] + 42, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->gpr[6] + 4;
    cpu->gpr[3] = cpu->gpr[3] + 208;
    uint32_t _jt_8017D370 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[5] + 36, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, _jt_8017D370);
    cpu->pc = _jt_8017D370; return;  /* CPS: jr $ra */
}

/* cps_8017d55c — clean CPS transliteration of br2eu_func_8017D55C (guest 0x8017d55c)
 * from SLES_017.22_full_35.c. */
void cps_8017d55c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017D580u: goto block_8017D580;
            case 0x8017D584u: goto block_8017D584;
            case 0x8017D594u: goto block_8017D594;
            case 0x8017D59Cu: goto block_8017D59C;
            case 0x8017D5A4u: goto block_8017D5A4;
            case 0x8017D5ACu: goto block_8017D5AC;
            default: break;
        }
    }
    /* Address: 0x8017D55C, Size: 100 bytes, Blocks: 7 */

block_8017D55C:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017D560: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[7] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[7] >> 16;
    cpu->gpr[2] = cpu->gpr[3] & 0x0040;  /* 0x8017D574: 0x30620040 */
    int _bc_8017D578 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[7] >> 20;
    if (_bc_8017D578) {
        psx_check_interrupts_at(cpu, 0x8017D584u);
        goto block_8017D584;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D580u);
        goto block_8017D580;  /* not taken */
    }

block_8017D580:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + 44);
    /* fall through to block_8017D584 */

block_8017D584:
    cpu->gpr[7] = cpu->gpr[2] & 0x0003;  /* 0x8017D584: 0x30470003 */
    cpu->gpr[2] = 1;
    int _bc_8017D58C = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[7] < 2) ? 1 : 0;  /* 0x8017D590: 0x28E20002 */
    if (_bc_8017D58C) {
        psx_check_interrupts_at(cpu, 0x8017D5ACu);
        goto block_8017D5AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D594u);
        goto block_8017D594;  /* not taken */
    }

block_8017D594:
    int _bc_8017D594 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017D594) {
        psx_check_interrupts_at(cpu, 0x8017D5DCu);
        cpu->pc = 0x8017D5DCu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D59Cu);
        goto block_8017D59C;  /* not taken */
    }

block_8017D59C:
    int _bc_8017D59C = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D5A0: 0x00000000 */
    if (_bc_8017D59C) {
        psx_check_interrupts_at(cpu, 0x8017D5C8u);
        cpu->pc = 0x8017D5C8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D5A4u);
        goto block_8017D5A4;  /* not taken */
    }

block_8017D5A4:
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D5A8: 0x3063000F */
    psx_check_interrupts_at(cpu, 0x8017D5E0u);
    cpu->pc = 0x8017D5E0u; return;  /* CPS j: split */

block_8017D5AC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017D5B0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017D5B4: 0x30420001 */
    int _bc_8017D5B8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D5BC: 0x3063000F */
    if (_bc_8017D5B8) {
        psx_check_interrupts_at(cpu, 0x8017D5E0u);
        cpu->pc = 0x8017D5E0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D5C0u);
        cpu->pc = 0x8017D5C0u; return;  /* CPS not taken: split */
    }
}

/* cps_8017d5c0 — clean CPS transliteration of br2eu_func_8017D5C0 (guest 0x8017d5c0)
 * from SLES_017.22_full_35.c. */
void cps_8017d5c0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D5C0, Size: 8 bytes, Blocks: 1 */

block_8017D5C0:
    uint32_t _jt_8017D5C0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017D5C0);
    cpu->pc = _jt_8017D5C0; return;  /* CPS: jr $ra */
}

/* cps_8017d5c8 — clean CPS transliteration of br2eu_func_8017D5C8 (guest 0x8017d5c8)
 * from SLES_017.22_full_35.c. */
void cps_8017d5c8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D5C8, Size: 20 bytes, Blocks: 1 */

block_8017D5C8:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017D5CC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017D5D0: 0x30420001 */
    int _bc_8017D5D4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D5D8: 0x00000000 */
    if (_bc_8017D5D4) {
        psx_check_interrupts_at(cpu, 0x8017D5C0u);
        cpu->pc = 0x8017D5C0u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D5DCu);
        cpu->pc = 0x8017D5DCu; return;  /* CPS not taken: split */
    }
    cps_8017d5dc();  /* fallthrough to next function */
}

/* cps_8017d5dc — clean CPS transliteration of br2eu_func_8017D5DC (guest 0x8017d5dc)
 * from SLES_017.22_full_35.c. */
void cps_8017d5dc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D5DC, Size: 4 bytes, Blocks: 1 */

block_8017D5DC:
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D5DC: 0x3063000F */
    cps_8017d5e0(); return;  /* fallthrough to split piece */
    cps_8017d5e0();  /* fallthrough to next function */
}

/* cps_8017d5e0 — clean CPS transliteration of br2eu_func_8017D5E0 (guest 0x8017d5e0)
 * from SLES_017.22_full_35.c. */
void cps_8017d5e0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017D5ECu: goto block_8017D5EC;
            case 0x8017D608u: goto block_8017D608;
            case 0x8017D61Cu: goto block_8017D61C;
            case 0x8017D628u: goto block_8017D628;
            case 0x8017D63Cu: goto block_8017D63C;
            case 0x8017D650u: goto block_8017D650;
            case 0x8017D660u: goto block_8017D660;
            case 0x8017D670u: goto block_8017D670;
            case 0x8017D67Cu: goto block_8017D67C;
            case 0x8017D690u: goto block_8017D690;
            case 0x8017D694u: goto block_8017D694;
            case 0x8017D6A0u: goto block_8017D6A0;
            case 0x8017D6A4u: goto block_8017D6A4;
            default: break;
        }
    }
    /* Address: 0x8017D5E0, Size: 204 bytes, Blocks: 14 */

block_8017D5E0:
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)9) ? 1 : 0;  /* 0x8017D5E0: 0x2C620009 */
    int _bc_8017D5E4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8017D5E4) {
        psx_check_interrupts_at(cpu, 0x8017D6A4u);
        goto block_8017D6A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D5ECu);
        goto block_8017D5EC;  /* not taken */
    }

block_8017D5EC:
    cpu->gpr[2] = cpu->gpr[2] + 10692;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8017D5FC: 0x00000000 */
    uint32_t _jt_8017D600 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D604: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017D600);
    cpu->pc = _jt_8017D600; return;  /* CPS: jr cpu->gpr[2] */

block_8017D608:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 32);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[2] + 56);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017D6A0u);
    goto block_8017D6A0;  /* j */

block_8017D61C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 144);
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D624: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017D694u);
    goto block_8017D694;  /* j */

block_8017D628:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 56);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2] + 128);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017D6A0u);
    goto block_8017D6A0;  /* j */

block_8017D63C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 56);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2] + 162);
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    psx_check_interrupts_at(cpu, 0x8017D6A0u);
    goto block_8017D6A0;  /* j */

block_8017D650:
    cpu->gpr[2] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[2] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[2] + 153);
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D65C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017D694u);
    goto block_8017D694;  /* j */

block_8017D660:
    cpu->gpr[2] = 0x1F80 << 16;  /* 0x1F800000 */
    cpu->gpr[2] = (int32_t)(int8_t)cpu->read_byte(cpu->gpr[2] + 152);
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D66C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017D694u);
    goto block_8017D694;  /* j */

block_8017D670:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 152);
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D678: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017D694u);
    goto block_8017D694;  /* j */

block_8017D67C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 56);
    /* nop */  /* 0x8017D680: 0x00000000 */
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[2] + 256);
    /* delay slot (always executes) */
    cpu->write_word(cpu->gpr[5] + 148, cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017D6A4u);
    goto block_8017D6A4;  /* j */

block_8017D690:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 210);
    /* fall through to block_8017D694 */

block_8017D694:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017D698: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    /* fall through to block_8017D6A0 */

block_8017D6A0:
    cpu->write_word(cpu->gpr[5] + 148, cpu->gpr[2]);
    /* fall through to block_8017D6A4 */

block_8017D6A4:
    uint32_t _jt_8017D6A4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017D6A4);
    cpu->pc = _jt_8017D6A4; return;  /* CPS: jr $ra */
}

/* cps_8017d6ac — clean CPS transliteration of br2eu_func_8017D6AC (guest 0x8017d6ac)
 * from SLES_017.22_full_35.c. */
void cps_8017d6ac(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017D6D0u: goto block_8017D6D0;
            case 0x8017D6D4u: goto block_8017D6D4;
            case 0x8017D6E4u: goto block_8017D6E4;
            case 0x8017D6ECu: goto block_8017D6EC;
            case 0x8017D6F4u: goto block_8017D6F4;
            case 0x8017D6FCu: goto block_8017D6FC;
            case 0x8017D710u: goto block_8017D710;
            default: break;
        }
    }
    /* Address: 0x8017D6AC, Size: 112 bytes, Blocks: 8 */

block_8017D6AC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017D6B0: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[7] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[7] >> 16;
    cpu->gpr[2] = cpu->gpr[3] & 0x0040;  /* 0x8017D6C4: 0x30620040 */
    int _bc_8017D6C8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[7] >> 20;
    if (_bc_8017D6C8) {
        psx_check_interrupts_at(cpu, 0x8017D6D4u);
        goto block_8017D6D4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D6D0u);
        goto block_8017D6D0;  /* not taken */
    }

block_8017D6D0:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + 44);
    /* fall through to block_8017D6D4 */

block_8017D6D4:
    cpu->gpr[7] = cpu->gpr[2] & 0x0003;  /* 0x8017D6D4: 0x30470003 */
    cpu->gpr[2] = 1;
    int _bc_8017D6DC = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[7] < 2) ? 1 : 0;  /* 0x8017D6E0: 0x28E20002 */
    if (_bc_8017D6DC) {
        psx_check_interrupts_at(cpu, 0x8017D6FCu);
        goto block_8017D6FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D6E4u);
        goto block_8017D6E4;  /* not taken */
    }

block_8017D6E4:
    int _bc_8017D6E4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017D6E4) {
        psx_check_interrupts_at(cpu, 0x8017D754u);
        cpu->pc = 0x8017D754u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D6ECu);
        goto block_8017D6EC;  /* not taken */
    }

block_8017D6EC:
    int _bc_8017D6EC = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D6F0: 0x00000000 */
    if (_bc_8017D6EC) {
        psx_check_interrupts_at(cpu, 0x8017D72Cu);
        cpu->pc = 0x8017D72Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D6F4u);
        goto block_8017D6F4;  /* not taken */
    }

block_8017D6F4:
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D6F8: 0x3063000F */
    psx_check_interrupts_at(cpu, 0x8017D758u);
    cpu->pc = 0x8017D758u; return;  /* CPS j: split */

block_8017D6FC:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017D700: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017D704: 0x30420001 */
    int _bc_8017D708 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    if (_bc_8017D708) {
        psx_check_interrupts_at(cpu, 0x8017D754u);
        cpu->pc = 0x8017D754u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D710u);
        goto block_8017D710;  /* not taken */
    }

block_8017D710:
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D710: 0x3063000F */
    int _bc_8017D714 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D718: 0x00000000 */
    if (_bc_8017D714) {
        psx_check_interrupts_at(cpu, 0x8017D724u);
        cpu->pc = 0x8017D724u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D71Cu);
        cpu->pc = 0x8017D71Cu; return;  /* CPS not taken: split */
    }
}

/* cps_8017d71c — clean CPS transliteration of br2eu_func_8017D71C (guest 0x8017d71c)
 * from SLES_017.22_full_35.c. */
void cps_8017d71c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D71C, Size: 8 bytes, Blocks: 1 */

block_8017D71C:
    uint32_t _jt_8017D71C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    psx_check_interrupts_at(cpu, _jt_8017D71C);
    cpu->pc = _jt_8017D71C; return;  /* CPS: jr $ra */
}

/* cps_8017d724 — clean CPS transliteration of br2eu_func_8017D724 (guest 0x8017d724)
 * from SLES_017.22_full_35.c. */
void cps_8017d724(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D724, Size: 8 bytes, Blocks: 1 */

block_8017D724:
    uint32_t _jt_8017D724 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017D724);
    cpu->pc = _jt_8017D724; return;  /* CPS: jr $ra */
}

/* cps_8017d72c — clean CPS transliteration of br2eu_func_8017D72C (guest 0x8017d72c)
 * from SLES_017.22_full_35.c. */
void cps_8017d72c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017D740u: goto block_8017D740;
            case 0x8017D74Cu: goto block_8017D74C;
            default: break;
        }
    }
    /* Address: 0x8017D72C, Size: 40 bytes, Blocks: 3 */

block_8017D72C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017D730: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017D734: 0x30420001 */
    int _bc_8017D738 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 4;
    if (_bc_8017D738) {
        psx_check_interrupts_at(cpu, 0x8017D754u);
        cpu->pc = 0x8017D754u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D740u);
        goto block_8017D740;  /* not taken */
    }

block_8017D740:
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D740: 0x3063000F */
    int _bc_8017D744 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D748: 0x00000000 */
    if (_bc_8017D744) {
        psx_check_interrupts_at(cpu, 0x8017D71Cu);
        cpu->pc = 0x8017D71Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D74Cu);
        goto block_8017D74C;  /* not taken */
    }

block_8017D74C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D750: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017D724u);
    cpu->pc = 0x8017D724u; return;  /* CPS j: split */
}

/* cps_8017d754 — clean CPS transliteration of br2eu_func_8017D754 (guest 0x8017d754)
 * from SLES_017.22_full_35.c. */
void cps_8017d754(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D754, Size: 4 bytes, Blocks: 1 */

block_8017D754:
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x8017D754: 0x3063000F */
    cps_8017d758(); return;  /* fallthrough to split piece */
    cps_8017d758();  /* fallthrough to next function */
}

/* cps_8017d758 — clean CPS transliteration of br2eu_func_8017D758 (guest 0x8017d758)
 * from SLES_017.22_full_35.c. */
void cps_8017d758(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017D764u: goto block_8017D764;
            case 0x8017D780u: goto block_8017D780;
            case 0x8017D78Cu: goto block_8017D78C;
            case 0x8017D798u: goto block_8017D798;
            case 0x8017D7A4u: goto block_8017D7A4;
            case 0x8017D7B0u: goto block_8017D7B0;
            default: break;
        }
    }
    /* Address: 0x8017D758, Size: 148 bytes, Blocks: 7 */

block_8017D758:
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)5) ? 1 : 0;  /* 0x8017D758: 0x2C620005 */
    int _bc_8017D75C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (_bc_8017D75C) {
        psx_check_interrupts_at(cpu, 0x8017D7ECu);
        cpu->pc = 0x8017D7ECu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017D764u);
        goto block_8017D764;  /* not taken */
    }

block_8017D764:
    cpu->gpr[2] = cpu->gpr[2] + 10732;
    cpu->gpr[3] = cpu->gpr[3] << 2;
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    cpu->gpr[2] = cpu->read_word(cpu->gpr[3]);
    /* nop */  /* 0x8017D774: 0x00000000 */
    uint32_t _jt_8017D778 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017D77C: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017D778);
    cpu->pc = _jt_8017D778; return;  /* CPS: jr cpu->gpr[2] */

block_8017D780:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[5] + 257, (uint8_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017D7ECu);
    cpu->pc = 0x8017D7ECu; return;  /* CPS j: split */

block_8017D78C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[5] + 258, (uint8_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017D7ECu);
    cpu->pc = 0x8017D7ECu; return;  /* CPS j: split */

block_8017D798:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[5] + 259, (uint8_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017D7ECu);
    cpu->pc = 0x8017D7ECu; return;  /* CPS j: split */

block_8017D7A4:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 2);
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[5] + 260, (uint8_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017D7ECu);
    cpu->pc = 0x8017D7ECu; return;  /* CPS j: split */

block_8017D7B0:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017D7B4: 0x00000000 */
    cpu->write_byte(cpu->gpr[5] + 257, (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017D7C0: 0x00000000 */
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 8;
    cpu->write_byte(cpu->gpr[5] + 258, (uint8_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[6] + 4);
    /* nop */  /* 0x8017D7D0: 0x00000000 */
    cpu->write_byte(cpu->gpr[5] + 259, (uint8_t)cpu->gpr[2]);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 4);
    cpu->gpr[2] = cpu->gpr[6] + 6;
    cpu->gpr[3] = (uint32_t)cpu->gpr[3] >> 8;
    uint32_t _jt_8017D7E4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_byte(cpu->gpr[5] + 260, (uint8_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, _jt_8017D7E4);
    cpu->pc = _jt_8017D7E4; return;  /* CPS: jr $ra */
}

/* cps_8017d7ec — clean CPS transliteration of br2eu_func_8017D7EC (guest 0x8017d7ec)
 * from SLES_017.22_full_35.c. */
void cps_8017d7ec(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017D7EC, Size: 8 bytes, Blocks: 1 */

block_8017D7EC:
    uint32_t _jt_8017D7EC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 4;
    psx_check_interrupts_at(cpu, _jt_8017D7EC);
    cpu->pc = _jt_8017D7EC; return;  /* CPS: jr $ra */
}

/* cps_8017d9dc — clean CPS transliteration of br2eu_func_8017D9DC (guest 0x8017d9dc)
 * from SLES_017.22_full_36.c. */
void cps_8017d9dc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017DA00u: goto block_8017DA00;
            case 0x8017DA04u: goto block_8017DA04;
            case 0x8017DA18u: goto block_8017DA18;
            case 0x8017DA20u: goto block_8017DA20;
            case 0x8017DA28u: goto block_8017DA28;
            case 0x8017DA30u: goto block_8017DA30;
            default: break;
        }
    }
    /* Address: 0x8017D9DC, Size: 104 bytes, Blocks: 7 */

block_8017D9DC:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    cpu->gpr[7] = cpu->read_byte(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[3] = cpu->gpr[2] << 16;
    cpu->gpr[5] = (int32_t)cpu->gpr[3] >> 16;
    cpu->gpr[2] = cpu->gpr[5] & 0x0040;  /* 0x8017D9F4: 0x30A20040 */
    int _bc_8017D9F8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[8] = cpu->gpr[4];
    if (_bc_8017D9F8) {
        psx_check_interrupts_at(cpu, 0x8017DA04u);
        goto block_8017DA04;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA00u);
        goto block_8017DA00;  /* not taken */
    }

block_8017DA00:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[8] + 44);
    /* fall through to block_8017DA04 */

block_8017DA04:
    cpu->gpr[2] = (int32_t)cpu->gpr[3] >> 20;
    cpu->gpr[3] = cpu->gpr[2] & 0x0003;  /* 0x8017DA08: 0x30430003 */
    cpu->gpr[2] = 1;
    int _bc_8017DA10 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017DA14: 0x28620002 */
    if (_bc_8017DA10) {
        psx_check_interrupts_at(cpu, 0x8017DA30u);
        goto block_8017DA30;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA18u);
        goto block_8017DA18;  /* not taken */
    }

block_8017DA18:
    int _bc_8017DA18 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017DA18) {
        psx_check_interrupts_at(cpu, 0x8017DA60u);
        cpu->pc = 0x8017DA60u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA20u);
        goto block_8017DA20;  /* not taken */
    }

block_8017DA20:
    int _bc_8017DA20 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[5] & 0x000F;  /* 0x8017DA24: 0x30A3000F */
    if (_bc_8017DA20) {
        psx_check_interrupts_at(cpu, 0x8017DA4Cu);
        cpu->pc = 0x8017DA4Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA28u);
        goto block_8017DA28;  /* not taken */
    }

block_8017DA28:
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    psx_check_interrupts_at(cpu, 0x8017DA68u);
    cpu->pc = 0x8017DA68u; return;  /* CPS j: split */

block_8017DA30:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017DA34: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017DA38: 0x30420001 */
    int _bc_8017DA3C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[5] & 0x000F;  /* 0x8017DA40: 0x30A3000F */
    if (_bc_8017DA3C) {
        psx_check_interrupts_at(cpu, 0x8017DA64u);
        cpu->pc = 0x8017DA64u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA44u);
        cpu->pc = 0x8017DA44u; return;  /* CPS not taken: split */
    }
}

/* cps_8017da44 — clean CPS transliteration of br2eu_func_8017DA44 (guest 0x8017da44)
 * from SLES_017.22_full_36.c. */
void cps_8017da44(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017DA44, Size: 8 bytes, Blocks: 1 */

block_8017DA44:
    uint32_t _jt_8017DA44 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 8;
    psx_check_interrupts_at(cpu, _jt_8017DA44);
    cpu->pc = _jt_8017DA44; return;  /* CPS: jr $ra */
}

/* cps_8017da4c — clean CPS transliteration of br2eu_func_8017DA4C (guest 0x8017da4c)
 * from SLES_017.22_full_36.c. */
void cps_8017da4c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017DA4C, Size: 20 bytes, Blocks: 1 */

block_8017DA4C:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017DA50: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017DA54: 0x30420001 */
    int _bc_8017DA58 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017DA5C: 0x00000000 */
    if (_bc_8017DA58) {
        psx_check_interrupts_at(cpu, 0x8017DA44u);
        cpu->pc = 0x8017DA44u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA60u);
        cpu->pc = 0x8017DA60u; return;  /* CPS not taken: split */
    }
    cps_8017da60();  /* fallthrough to next function */
}

/* cps_8017da60 — clean CPS transliteration of br2eu_func_8017DA60 (guest 0x8017da60)
 * from SLES_017.22_full_36.c. */
void cps_8017da60(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017DA60, Size: 4 bytes, Blocks: 1 */

block_8017DA60:
    cpu->gpr[3] = cpu->gpr[5] & 0x000F;  /* 0x8017DA60: 0x30A3000F */
    cps_8017da64(); return;  /* fallthrough to split piece */
    cps_8017da64();  /* fallthrough to next function */
}

/* cps_8017da64 — clean CPS transliteration of br2eu_func_8017DA64 (guest 0x8017da64)
 * from SLES_017.22_full_36.c. */
void cps_8017da64(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017DA64, Size: 4 bytes, Blocks: 1 */

block_8017DA64:
    cpu->gpr[2] = 1;
    cps_8017da68(); return;  /* fallthrough to split piece */
    cps_8017da68();  /* fallthrough to next function */
}

/* cps_8017da68 — clean CPS transliteration of br2eu_func_8017DA68 (guest 0x8017da68)
 * from SLES_017.22_full_36.c. */
void cps_8017da68(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017DA70u: goto block_8017DA70;
            case 0x8017DA78u: goto block_8017DA78;
            case 0x8017DA80u: goto block_8017DA80;
            case 0x8017DA88u: goto block_8017DA88;
            case 0x8017DA90u: goto block_8017DA90;
            case 0x8017DA98u: goto block_8017DA98;
            case 0x8017DAC8u: goto block_8017DAC8;
            case 0x8017DAF8u: goto block_8017DAF8;
            case 0x8017DB7Cu: goto block_8017DB7C;
            case 0x8017DB80u: goto block_8017DB80;
            default: break;
        }
    }
    /* Address: 0x8017DA68, Size: 288 bytes, Blocks: 11 */

block_8017DA68:
    int _bc_8017DA68 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017DA6C: 0x28620002 */
    if (_bc_8017DA68) {
        psx_check_interrupts_at(cpu, 0x8017DAC8u);
        goto block_8017DAC8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA70u);
        goto block_8017DA70;  /* not taken */
    }

block_8017DA70:
    int _bc_8017DA70 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017DA70) {
        psx_check_interrupts_at(cpu, 0x8017DA88u);
        goto block_8017DA88;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA78u);
        goto block_8017DA78;  /* not taken */
    }

block_8017DA78:
    int _bc_8017DA78 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 8;
    if (_bc_8017DA78) {
        psx_check_interrupts_at(cpu, 0x8017DA98u);
        goto block_8017DA98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA80u);
        goto block_8017DA80;  /* not taken */
    }

block_8017DA80:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017DA84: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017DB80u);
    goto block_8017DB80;  /* j */

block_8017DA88:
    int _bc_8017DA88 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 8;
    if (_bc_8017DA88) {
        psx_check_interrupts_at(cpu, 0x8017DAF8u);
        goto block_8017DAF8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017DA90u);
        goto block_8017DA90;  /* not taken */
    }

block_8017DA90:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017DA94: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017DB80u);
    goto block_8017DB80;  /* j */

block_8017DA98:
    cpu->gpr[2] = cpu->gpr[7] << 3;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[7];
    cpu->gpr[3] = cpu->read_word(cpu->gpr[8] + 64);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2] + 24);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[6] + 4);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4]);
    /* nop */  /* 0x8017DAB8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4], (uint16_t)cpu->gpr[2]);
    psx_check_interrupts_at(cpu, 0x8017DB7Cu);
    goto block_8017DB7C;  /* j */

block_8017DAC8:
    cpu->gpr[2] = cpu->gpr[7] << 3;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[7];
    cpu->gpr[3] = cpu->read_word(cpu->gpr[8] + 64);
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[4] = cpu->read_word(cpu->gpr[2] + 24);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 4);
    cpu->gpr[3] = cpu->read_half(cpu->gpr[4]);
    cpu->gpr[2] = ~cpu->gpr[2];  /* not */  /* 0x8017DAE8: 0x00021027 */
    cpu->gpr[3] = cpu->gpr[3] & cpu->gpr[2];  /* 0x8017DAEC: 0x00621824 */
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[4], (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x8017DB7Cu);
    goto block_8017DB7C;  /* j */

block_8017DAF8:
    cpu->gpr[5] = 0xB60B << 16;  /* 0xB60B0000 */
    cpu->gpr[4] = cpu->gpr[7] << 3;
    cpu->gpr[4] = cpu->gpr[4] - cpu->gpr[7];
    cpu->gpr[4] = cpu->gpr[4] << 2;
    cpu->gpr[3] = cpu->read_word(cpu->gpr[8] + 64);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[3] = cpu->gpr[4] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->write_half(cpu->gpr[3] + 14, (uint16_t)cpu->gpr[2]);
    cpu->gpr[2] = cpu->read_word(cpu->gpr[8] + 64);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[6] + 4);
    cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[2];
    cpu->write_half(cpu->gpr[2] + 16, (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_word(cpu->gpr[8] + 64);
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6] + 4);
    cpu->gpr[3] = cpu->gpr[4] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->write_half(cpu->gpr[3] + 18, (uint16_t)cpu->gpr[2]);
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 6);
    cpu->gpr[5] = cpu->gpr[5] | 0x60B7;
    cpu->gpr[3] = cpu->gpr[3] << 12;
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[5]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
    cpu->gpr[2] = cpu->read_word(cpu->gpr[8] + 64);
    /* nop */  /* 0x8017DB5C: 0x00000000 */
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    cpu->gpr[9] = cpu->hi;
    psx_muldiv_stall(cpu);
    cpu->gpr[2] = cpu->gpr[9] + cpu->gpr[3];
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 8;
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 31;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->write_half(cpu->gpr[4] + 20, (uint16_t)cpu->gpr[2]);
    /* fall through to block_8017DB7C */

block_8017DB7C:
    cpu->gpr[2] = cpu->gpr[6] + 8;
    /* fall through to block_8017DB80 */

block_8017DB80:
    uint32_t _jt_8017DB80 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017DB84: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017DB80);
    cpu->pc = _jt_8017DB80; return;  /* CPS: jr $ra */
}

/* cps_8017e4cc — clean CPS transliteration of br2eu_func_8017E4CC (guest 0x8017e4cc)
 * from SLES_017.22_full_36.c. */
void cps_8017e4cc(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017E4CC, Size: 8 bytes, Blocks: 1 */

block_8017E4CC:
    uint32_t _jt_8017E4CC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6];
    psx_check_interrupts_at(cpu, _jt_8017E4CC);
    cpu->pc = _jt_8017E4CC; return;  /* CPS: jr $ra */
}

/* cps_8017e4d4 — clean CPS transliteration of br2eu_func_8017E4D4 (guest 0x8017e4d4)
 * from SLES_017.22_full_36.c. */
void cps_8017e4d4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017E4F0u: goto block_8017E4F0;
            case 0x8017E4F8u: goto block_8017E4F8;
            case 0x8017E500u: goto block_8017E500;
            case 0x8017E508u: goto block_8017E508;
            case 0x8017E510u: goto block_8017E510;
            case 0x8017E518u: goto block_8017E518;
            case 0x8017E528u: goto block_8017E528;
            case 0x8017E530u: goto block_8017E530;
            case 0x8017E540u: goto block_8017E540;
            case 0x8017E548u: goto block_8017E548;
            case 0x8017E554u: goto block_8017E554;
            case 0x8017E588u: goto block_8017E588;
            case 0x8017E598u: goto block_8017E598;
            case 0x8017E59Cu: goto block_8017E59C;
            case 0x8017E61Cu: goto block_8017E61C;
            case 0x8017E620u: goto block_8017E620;
            default: break;
        }
    }
    /* Address: 0x8017E4D4, Size: 340 bytes, Blocks: 17 */

block_8017E4D4:
    cpu->gpr[7] = cpu->gpr[4];
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    cpu->gpr[4] = 1;
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 24;
    int _bc_8017E4E8 = (cpu->gpr[3] == cpu->gpr[4]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017E4EC: 0x28620002 */
    if (_bc_8017E4E8) {
        psx_check_interrupts_at(cpu, 0x8017E588u);
        goto block_8017E588;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E4F0u);
        goto block_8017E4F0;  /* not taken */
    }

block_8017E4F0:
    int _bc_8017E4F0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017E4F0) {
        psx_check_interrupts_at(cpu, 0x8017E508u);
        goto block_8017E508;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E4F8u);
        goto block_8017E4F8;  /* not taken */
    }

block_8017E4F8:
    int _bc_8017E4F8 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    if (_bc_8017E4F8) {
        psx_check_interrupts_at(cpu, 0x8017E518u);
        goto block_8017E518;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E500u);
        goto block_8017E500;  /* not taken */
    }

block_8017E500:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E504: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017E620u);
    goto block_8017E620;  /* j */

block_8017E508:
    int _bc_8017E508 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    if (_bc_8017E508) {
        psx_check_interrupts_at(cpu, 0x8017E554u);
        goto block_8017E554;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E510u);
        goto block_8017E510;  /* not taken */
    }

block_8017E510:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E514: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017E620u);
    goto block_8017E620;  /* j */

block_8017E518:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    /* nop */  /* 0x8017E51C: 0x00000000 */
    int _bc_8017E520 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E524: 0x00000000 */
    if (_bc_8017E520) {
        psx_check_interrupts_at(cpu, 0x8017E530u);
        goto block_8017E530;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E528u);
        goto block_8017E528;  /* not taken */
    }

block_8017E528:
    int _bc_8017E528 = (cpu->gpr[2] == cpu->gpr[4]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    if (_bc_8017E528) {
        psx_check_interrupts_at(cpu, 0x8017E540u);
        goto block_8017E540;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E530u);
        goto block_8017E530;  /* not taken */
    }

block_8017E530:
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[2] = cpu->gpr[2] + 19632;
    /* delay slot (always executes) */
    cpu->gpr[3] = -4;
    psx_check_interrupts_at(cpu, 0x8017E548u);
    goto block_8017E548;  /* j */

block_8017E540:
    cpu->gpr[2] = cpu->gpr[2] + 19632;
    cpu->gpr[3] = 8;
    /* fall through to block_8017E548 */

block_8017E548:
    cpu->write_half(cpu->gpr[2] + 2, (uint16_t)cpu->gpr[0]);
    /* delay slot (always executes) */
    cpu->write_half(cpu->gpr[2] + 6, (uint16_t)cpu->gpr[3]);
    psx_check_interrupts_at(cpu, 0x8017E61Cu);
    goto block_8017E61C;  /* j */

block_8017E554:
    cpu->gpr[4] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 2);
    cpu->gpr[4] = cpu->gpr[4] + 19824;
    cpu->gpr[2] = cpu->gpr[3] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 4;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[2] + 6);
    /* nop */  /* 0x8017E57C: 0x00000000 */
    int _bc_8017E580 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 6;
    if (_bc_8017E580) {
        psx_check_interrupts_at(cpu, 0x8017E620u);
        goto block_8017E620;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E588u);
        goto block_8017E588;  /* not taken */
    }

block_8017E588:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 4);
    /* nop */  /* 0x8017E58C: 0x00000000 */
    int _bc_8017E590 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = cpu->gpr[2];
    if (_bc_8017E590) {
        psx_check_interrupts_at(cpu, 0x8017E59Cu);
        goto block_8017E59C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E598u);
        goto block_8017E598;  /* not taken */
    }

block_8017E598:
    cpu->gpr[4] = cpu->gpr[0] - cpu->gpr[4];
    /* fall through to block_8017E59C */

block_8017E59C:
    cpu->gpr[4] = cpu->gpr[4] + 255;
    if (cpu->gpr[2] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[4] / (int32_t)cpu->gpr[2]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[4] % (int32_t)cpu->gpr[2]); } else { cpu->lo = ((int32_t)cpu->gpr[4] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[4]; }
    psx_muldiv_set(cpu, 37u);
    cpu->gpr[4] = cpu->lo;
    psx_muldiv_stall(cpu);
    cpu->gpr[5] = 0x801F << 16;  /* 0x801F0000 */
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 2);
    cpu->gpr[5] = cpu->gpr[5] + 19824;
    cpu->gpr[2] = cpu->gpr[3] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 4;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 2);
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[5];
    cpu->write_half(cpu->gpr[2], (uint16_t)cpu->gpr[3]);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 2);
    /* nop */  /* 0x8017E5D8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 4;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 2, (uint16_t)cpu->gpr[0]);
    cpu->gpr[3] = cpu->read_byte(cpu->gpr[7] + 2);
    /* nop */  /* 0x8017E5FC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[3] << 1;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 4;
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
    cpu->gpr[2] = cpu->gpr[2] << 2;
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[5];
    cpu->write_half(cpu->gpr[2] + 6, (uint16_t)cpu->gpr[4]);
    /* fall through to block_8017E61C */

block_8017E61C:
    cpu->gpr[2] = cpu->gpr[6] + 6;
    /* fall through to block_8017E620 */

block_8017E620:
    uint32_t _jt_8017E620 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E624: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017E620);
    cpu->pc = _jt_8017E620; return;  /* CPS: jr $ra */
}

/* cps_8017e628 — clean CPS transliteration of br2eu_func_8017E628 (guest 0x8017e628)
 * from SLES_017.22_full_36.c. */
void cps_8017e628(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017E64Cu: goto block_8017E64C;
            case 0x8017E650u: goto block_8017E650;
            case 0x8017E660u: goto block_8017E660;
            case 0x8017E668u: goto block_8017E668;
            case 0x8017E670u: goto block_8017E670;
            case 0x8017E678u: goto block_8017E678;
            default: break;
        }
    }
    /* Address: 0x8017E628, Size: 100 bytes, Blocks: 7 */

block_8017E628:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[6]);
    /* nop */  /* 0x8017E62C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] << 16;
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    cpu->gpr[3] = cpu->gpr[2] << 16;
    cpu->gpr[7] = (int32_t)cpu->gpr[3] >> 16;
    cpu->gpr[2] = cpu->gpr[7] & 0x0040;  /* 0x8017E640: 0x30E20040 */
    int _bc_8017E644 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = (int32_t)cpu->gpr[3] >> 20;
    if (_bc_8017E644) {
        psx_check_interrupts_at(cpu, 0x8017E650u);
        goto block_8017E650;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E64Cu);
        goto block_8017E64C;  /* not taken */
    }

block_8017E64C:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + 44);
    /* fall through to block_8017E650 */

block_8017E650:
    cpu->gpr[3] = cpu->gpr[2] & 0x0003;  /* 0x8017E650: 0x30430003 */
    cpu->gpr[2] = 1;
    int _bc_8017E658 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017E65C: 0x28620002 */
    if (_bc_8017E658) {
        psx_check_interrupts_at(cpu, 0x8017E678u);
        goto block_8017E678;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E660u);
        goto block_8017E660;  /* not taken */
    }

block_8017E660:
    int _bc_8017E660 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017E660) {
        psx_check_interrupts_at(cpu, 0x8017E6A8u);
        cpu->pc = 0x8017E6A8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E668u);
        goto block_8017E668;  /* not taken */
    }

block_8017E668:
    int _bc_8017E668 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[7] & 0x000F;  /* 0x8017E66C: 0x30E3000F */
    if (_bc_8017E668) {
        psx_check_interrupts_at(cpu, 0x8017E694u);
        cpu->pc = 0x8017E694u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E670u);
        goto block_8017E670;  /* not taken */
    }

block_8017E670:
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    psx_check_interrupts_at(cpu, 0x8017E6B0u);
    cpu->pc = 0x8017E6B0u; return;  /* CPS j: split */

block_8017E678:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017E67C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017E680: 0x30420001 */
    int _bc_8017E684 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[7] & 0x000F;  /* 0x8017E688: 0x30E3000F */
    if (_bc_8017E684) {
        psx_check_interrupts_at(cpu, 0x8017E6ACu);
        cpu->pc = 0x8017E6ACu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E68Cu);
        cpu->pc = 0x8017E68Cu; return;  /* CPS not taken: split */
    }
}

/* cps_8017e68c — clean CPS transliteration of br2eu_func_8017E68C (guest 0x8017e68c)
 * from SLES_017.22_full_36.c. */
void cps_8017e68c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017E68C, Size: 8 bytes, Blocks: 1 */

block_8017E68C:
    uint32_t _jt_8017E68C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 2;
    psx_check_interrupts_at(cpu, _jt_8017E68C);
    cpu->pc = _jt_8017E68C; return;  /* CPS: jr $ra */
}

/* cps_8017e694 — clean CPS transliteration of br2eu_func_8017E694 (guest 0x8017e694)
 * from SLES_017.22_full_36.c. */
void cps_8017e694(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017E694, Size: 20 bytes, Blocks: 1 */

block_8017E694:
    cpu->gpr[2] = cpu->read_byte(cpu->gpr[4] + 5);
    /* nop */  /* 0x8017E698: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017E69C: 0x30420001 */
    int _bc_8017E6A0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E6A4: 0x00000000 */
    if (_bc_8017E6A0) {
        psx_check_interrupts_at(cpu, 0x8017E68Cu);
        cpu->pc = 0x8017E68Cu; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6A8u);
        cpu->pc = 0x8017E6A8u; return;  /* CPS not taken: split */
    }
    cps_8017e6a8();  /* fallthrough to next function */
}

/* cps_8017e6a8 — clean CPS transliteration of br2eu_func_8017E6A8 (guest 0x8017e6a8)
 * from SLES_017.22_full_36.c. */
void cps_8017e6a8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017E6A8, Size: 4 bytes, Blocks: 1 */

block_8017E6A8:
    cpu->gpr[3] = cpu->gpr[7] & 0x000F;  /* 0x8017E6A8: 0x30E3000F */
    cps_8017e6ac(); return;  /* fallthrough to split piece */
    cps_8017e6ac();  /* fallthrough to next function */
}

/* cps_8017e6ac — clean CPS transliteration of br2eu_func_8017E6AC (guest 0x8017e6ac)
 * from SLES_017.22_full_36.c. */
void cps_8017e6ac(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017E6AC, Size: 4 bytes, Blocks: 1 */

block_8017E6AC:
    cpu->gpr[2] = 1;
    cps_8017e6b0(); return;  /* fallthrough to split piece */
    cps_8017e6b0();  /* fallthrough to next function */
}

/* cps_8017e6b0 — clean CPS transliteration of br2eu_func_8017E6B0 (guest 0x8017e6b0)
 * from SLES_017.22_full_36.c. */
void cps_8017e6b0(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017E6B8u: goto block_8017E6B8;
            case 0x8017E6C0u: goto block_8017E6C0;
            case 0x8017E6C8u: goto block_8017E6C8;
            case 0x8017E6D0u: goto block_8017E6D0;
            case 0x8017E6D8u: goto block_8017E6D8;
            case 0x8017E6E0u: goto block_8017E6E0;
            case 0x8017E6E8u: goto block_8017E6E8;
            case 0x8017E6F8u: goto block_8017E6F8;
            case 0x8017E704u: goto block_8017E704;
            case 0x8017E714u: goto block_8017E714;
            case 0x8017E724u: goto block_8017E724;
            case 0x8017E730u: goto block_8017E730;
            case 0x8017E740u: goto block_8017E740;
            case 0x8017E74Cu: goto block_8017E74C;
            case 0x8017E760u: goto block_8017E760;
            case 0x8017E76Cu: goto block_8017E76C;
            case 0x8017E774u: goto block_8017E774;
            default: break;
        }
    }
    /* Address: 0x8017E6B0, Size: 204 bytes, Blocks: 18 */

block_8017E6B0:
    int _bc_8017E6B0 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 2) ? 1 : 0;  /* 0x8017E6B4: 0x28620002 */
    if (_bc_8017E6B0) {
        psx_check_interrupts_at(cpu, 0x8017E704u);
        goto block_8017E704;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6B8u);
        goto block_8017E6B8;  /* not taken */
    }

block_8017E6B8:
    int _bc_8017E6B8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 2;
    if (_bc_8017E6B8) {
        psx_check_interrupts_at(cpu, 0x8017E6D0u);
        goto block_8017E6D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6C0u);
        goto block_8017E6C0;  /* not taken */
    }

block_8017E6C0:
    int _bc_8017E6C0 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 2;
    if (_bc_8017E6C0) {
        psx_check_interrupts_at(cpu, 0x8017E6E8u);
        goto block_8017E6E8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6C8u);
        goto block_8017E6C8;  /* not taken */
    }

block_8017E6C8:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E6CC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017E774u);
    goto block_8017E774;  /* j */

block_8017E6D0:
    int _bc_8017E6D0 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 3;
    if (_bc_8017E6D0) {
        psx_check_interrupts_at(cpu, 0x8017E730u);
        goto block_8017E730;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6D8u);
        goto block_8017E6D8;  /* not taken */
    }

block_8017E6D8:
    int _bc_8017E6D8 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = cpu->gpr[6] + 2;
    if (_bc_8017E6D8) {
        psx_check_interrupts_at(cpu, 0x8017E74Cu);
        goto block_8017E74C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6E0u);
        goto block_8017E6E0;  /* not taken */
    }

block_8017E6E0:
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E6E4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8017E774u);
    goto block_8017E774;  /* j */

block_8017E6E8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 88);
    /* nop */  /* 0x8017E6EC: 0x00000000 */
    int _bc_8017E6F0 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[0];
    if (_bc_8017E6F0) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E6F8u);
        goto block_8017E6F8;  /* not taken */
    }

block_8017E6F8:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 186);
    /* delay slot (always executes) */
    cpu->gpr[3] = (cpu->gpr[3] < cpu->gpr[2]) ? 1 : 0;  /* 0x8017E700: 0x0062182B */
    psx_check_interrupts_at(cpu, 0x8017E76Cu);
    goto block_8017E76C;  /* j */

block_8017E704:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 88);
    /* nop */  /* 0x8017E708: 0x00000000 */
    int _bc_8017E70C = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[0];
    if (_bc_8017E70C) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E714u);
        goto block_8017E714;  /* not taken */
    }

block_8017E714:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 186);
    /* nop */  /* 0x8017E718: 0x00000000 */
    int _bc_8017E71C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E720: 0x00000000 */
    if (_bc_8017E71C) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E724u);
        goto block_8017E724;  /* not taken */
    }

block_8017E724:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 184);
    /* delay slot (always executes) */
    cpu->gpr[3] = (cpu->gpr[3] < cpu->gpr[2]) ? 1 : 0;  /* 0x8017E72C: 0x0062182B */
    psx_check_interrupts_at(cpu, 0x8017E76Cu);
    goto block_8017E76C;  /* j */

block_8017E730:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[4] + 88);
    /* nop */  /* 0x8017E734: 0x00000000 */
    int _bc_8017E738 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[0];
    if (_bc_8017E738) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E740u);
        goto block_8017E740;  /* not taken */
    }

block_8017E740:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 188);
    /* delay slot (always executes) */
    cpu->gpr[3] = ((int32_t)cpu->gpr[3] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x8017E748: 0x0062182A */
    psx_check_interrupts_at(cpu, 0x8017E76Cu);
    goto block_8017E76C;  /* j */

block_8017E74C:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[4] + 88);
    /* nop */  /* 0x8017E750: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[4] & 0x0800;  /* 0x8017E754: 0x30820800 */
    int _bc_8017E758 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = cpu->gpr[0];
    if (_bc_8017E758) {
        psx_check_interrupts_at(cpu, 0x8017E76Cu);
        goto block_8017E76C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017E760u);
        goto block_8017E760;  /* not taken */
    }

block_8017E760:
    cpu->gpr[2] = 0x0803 << 16;  /* 0x08030000 */
    cpu->gpr[2] = cpu->gpr[4] & cpu->gpr[2];  /* 0x8017E764: 0x00821024 */
    cpu->gpr[3] = (cpu->gpr[3] < cpu->gpr[2]) ? 1 : 0;  /* 0x8017E768: 0x0062182B */
    /* fall through to block_8017E76C */

block_8017E76C:
    cpu->write_word(cpu->gpr[5] + 148, cpu->gpr[3]);
    cpu->gpr[2] = cpu->gpr[6] + 2;
    /* fall through to block_8017E774 */

block_8017E774:
    uint32_t _jt_8017E774 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017E778: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017E774);
    cpu->pc = _jt_8017E774; return;  /* CPS: jr $ra */
}

/* cps_8017f97c — clean CPS transliteration of br2eu_func_8017F97C (guest 0x8017f97c)
 * from SLES_017.22_full_36.c. */
void cps_8017f97c(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017F98Cu: goto block_8017F98C;
            case 0x8017F9A0u: goto block_8017F9A0;
            case 0x8017F9B4u: goto block_8017F9B4;
            case 0x8017F9C8u: goto block_8017F9C8;
            case 0x8017F9D8u: goto block_8017F9D8;
            case 0x8017F9ECu: goto block_8017F9EC;
            case 0x8017F9FCu: goto block_8017F9FC;
            case 0x8017FA14u: goto block_8017FA14;
            case 0x8017FA30u: goto block_8017FA30;
            case 0x8017FA44u: goto block_8017FA44;
            case 0x8017FA58u: goto block_8017FA58;
            case 0x8017FA64u: goto block_8017FA64;
            case 0x8017FA78u: goto block_8017FA78;
            case 0x8017FA8Cu: goto block_8017FA8C;
            case 0x8017FAA0u: goto block_8017FAA0;
            case 0x8017FAB4u: goto block_8017FAB4;
            case 0x8017FAC8u: goto block_8017FAC8;
            case 0x8017FADCu: goto block_8017FADC;
            default: break;
        }
    }
    /* Address: 0x8017F97C, Size: 360 bytes, Blocks: 19 */

block_8017F97C:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 208);
    /* nop */  /* 0x8017F980: 0x00000000 */
    int _bc_8017F984 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x4000 << 16;  /* 0x40000000 */
    if (_bc_8017F984) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017F98Cu);
        goto block_8017F98C;  /* not taken */
    }

block_8017F98C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 96);
    /* nop */  /* 0x8017F990: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017F994: 0x00431024 */
    int _bc_8017F998 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x0040 << 16;  /* 0x00400000 */
    if (_bc_8017F998) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017F9A0u);
        goto block_8017F9A0;  /* not taken */
    }

block_8017F9A0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 92);
    /* nop */  /* 0x8017F9A4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017F9A8: 0x00431024 */
    int _bc_8017F9AC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x8008 << 16;  /* 0x80080000 */
    if (_bc_8017F9AC) {
        psx_check_interrupts_at(cpu, 0x8017F9C8u);
        goto block_8017F9C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017F9B4u);
        goto block_8017F9B4;  /* not taken */
    }

block_8017F9B4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 88);
    /* nop */  /* 0x8017F9B8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017F9BC: 0x00431024 */
    int _bc_8017F9C0 = (cpu->gpr[2] != cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017F9C4: 0x00000000 */
    if (_bc_8017F9C0) {
        psx_check_interrupts_at(cpu, 0x8017F9FCu);
        goto block_8017F9FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017F9C8u);
        goto block_8017F9C8;  /* not taken */
    }

block_8017F9C8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 88);
    /* nop */  /* 0x8017F9CC: 0x00000000 */
    int _bc_8017F9D0 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017F9D4: 0x00000000 */
    if (_bc_8017F9D0) {
        psx_check_interrupts_at(cpu, 0x8017F9FCu);
        goto block_8017F9FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017F9D8u);
        goto block_8017F9D8;  /* not taken */
    }

block_8017F9D8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 100);
    /* nop */  /* 0x8017F9DC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x8000;  /* 0x8017F9E0: 0x30428000 */
    int _bc_8017F9E4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017F9E8: 0x00000000 */
    if (_bc_8017F9E4) {
        psx_check_interrupts_at(cpu, 0x8017F9FCu);
        goto block_8017F9FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017F9ECu);
        goto block_8017F9EC;  /* not taken */
    }

block_8017F9EC:
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[5] + 188);
    /* nop */  /* 0x8017F9F0: 0x00000000 */
    int _bc_8017F9F4 = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017F9F8: 0x00000000 */
    if (_bc_8017F9F4) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017F9FCu);
        goto block_8017F9FC;  /* not taken */
    }

block_8017F9FC:
    cpu->gpr[3] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[4] + 118);
    cpu->gpr[2] = (int32_t)(int16_t)cpu->read_half(cpu->gpr[6] + 40);
    /* nop */  /* 0x8017FA04: 0x00000000 */
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x8017FA08: 0x0043102A */
    int _bc_8017FA0C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FA10: 0x00000000 */
    if (_bc_8017FA0C) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FA14u);
        goto block_8017FA14;  /* not taken */
    }

block_8017FA14:
    cpu->gpr[2] = cpu->read_half(cpu->gpr[4] + 116);
    /* nop */  /* 0x8017FA18: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] + 170;
    cpu->gpr[2] = cpu->gpr[2] & 0xFFFF;  /* 0x8017FA20: 0x3042FFFF */
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)341) ? 1 : 0;  /* 0x8017FA24: 0x2C420155 */
    int _bc_8017FA28 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FA2C: 0x00000000 */
    if (_bc_8017FA28) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FA30u);
        goto block_8017FA30;  /* not taken */
    }

block_8017FA30:
    cpu->gpr[4] = cpu->read_word(cpu->gpr[6] + 32);
    /* nop */  /* 0x8017FA34: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[4] & 0x1000;  /* 0x8017FA38: 0x30821000 */
    int _bc_8017FA3C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x0040 << 16;  /* 0x00400000 */
    if (_bc_8017FA3C) {
        psx_check_interrupts_at(cpu, 0x8017FA58u);
        goto block_8017FA58;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FA44u);
        goto block_8017FA44;  /* not taken */
    }

block_8017FA44:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 92);
    /* nop */  /* 0x8017FA48: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017FA4C: 0x00431024 */
    int _bc_8017FA50 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FA54: 0x00000000 */
    if (_bc_8017FA50) {
        psx_check_interrupts_at(cpu, 0x8017FA78u);
        goto block_8017FA78;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FA58u);
        goto block_8017FA58;  /* not taken */
    }

block_8017FA58:
    cpu->gpr[2] = cpu->gpr[4] & 0x2000;  /* 0x8017FA58: 0x30822000 */
    int _bc_8017FA5C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x0040 << 16;  /* 0x00400000 */
    if (_bc_8017FA5C) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FA64u);
        goto block_8017FA64;  /* not taken */
    }

block_8017FA64:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 92);
    /* nop */  /* 0x8017FA68: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017FA6C: 0x00431024 */
    int _bc_8017FA70 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FA74: 0x00000000 */
    if (_bc_8017FA70) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FA78u);
        goto block_8017FA78;  /* not taken */
    }

block_8017FA78:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[6] + 32);
    /* nop */  /* 0x8017FA7C: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0200;  /* 0x8017FA80: 0x30420200 */
    int _bc_8017FA84 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x0004 << 16;  /* 0x00040000 */
    if (_bc_8017FA84) {
        psx_check_interrupts_at(cpu, 0x8017FAB4u);
        goto block_8017FAB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FA8Cu);
        goto block_8017FA8C;  /* not taken */
    }

block_8017FA8C:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 92);
    /* nop */  /* 0x8017FA90: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017FA94: 0x00431024 */
    int _bc_8017FA98 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FA9C: 0x00000000 */
    if (_bc_8017FA98) {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FAA0u);
        goto block_8017FAA0;  /* not taken */
    }

block_8017FAA0:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 88);
    /* nop */  /* 0x8017FAA4: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x8017FAA8: 0x30420001 */
    int _bc_8017FAAC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FAB0: 0x00000000 */
    if (_bc_8017FAAC) {
        psx_check_interrupts_at(cpu, 0x8017FAF8u);
        cpu->pc = 0x8017FAF8u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FAB4u);
        goto block_8017FAB4;  /* not taken */
    }

block_8017FAB4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[6] + 32);
    /* nop */  /* 0x8017FAB8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0400;  /* 0x8017FABC: 0x30420400 */
    int _bc_8017FAC0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[3] = 0x0004 << 16;  /* 0x00040000 */
    if (_bc_8017FAC0) {
        psx_check_interrupts_at(cpu, 0x8017FB00u);
        cpu->pc = 0x8017FB00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FAC8u);
        goto block_8017FAC8;  /* not taken */
    }

block_8017FAC8:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 92);
    /* nop */  /* 0x8017FACC: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8017FAD0: 0x00431024 */
    int _bc_8017FAD4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FAD8: 0x00000000 */
    if (_bc_8017FAD4) {
        psx_check_interrupts_at(cpu, 0x8017FAE4u);
        cpu->pc = 0x8017FAE4u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FADCu);
        goto block_8017FADC;  /* not taken */
    }

block_8017FADC:
    uint32_t _jt_8017FADC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = -1;
    psx_check_interrupts_at(cpu, _jt_8017FADC);
    cpu->pc = _jt_8017FADC; return;  /* CPS: jr $ra */
}

/* cps_8017fae4 — clean CPS transliteration of br2eu_func_8017FAE4 (guest 0x8017fae4)
 * from SLES_017.22_full_36.c. */
void cps_8017fae4(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017FAE4, Size: 20 bytes, Blocks: 1 */

block_8017FAE4:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[5] + 88);
    /* nop */  /* 0x8017FAE8: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0030;  /* 0x8017FAEC: 0x30420030 */
    int _bc_8017FAF0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FAF4: 0x00000000 */
    if (_bc_8017FAF0) {
        psx_check_interrupts_at(cpu, 0x8017FB00u);
        cpu->pc = 0x8017FB00u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FAF8u);
        cpu->pc = 0x8017FAF8u; return;  /* CPS not taken: split */
    }
    cps_8017faf8();  /* fallthrough to next function */
}

/* cps_8017faf8 — clean CPS transliteration of br2eu_func_8017FAF8 (guest 0x8017faf8)
 * from SLES_017.22_full_36.c. */
void cps_8017faf8(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    /* Address: 0x8017FAF8, Size: 8 bytes, Blocks: 1 */

block_8017FAF8:
    uint32_t _jt_8017FAF8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    psx_check_interrupts_at(cpu, _jt_8017FAF8);
    cpu->pc = _jt_8017FAF8; return;  /* CPS: jr $ra */
}

/* cps_8017fb00 — clean CPS transliteration of br2eu_func_8017FB00 (guest 0x8017fb00)
 * from SLES_017.22_full_36.c. */
void cps_8017fb00(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8017FB14u: goto block_8017FB14;
            case 0x8017FB28u: goto block_8017FB28;
            case 0x8017FB2Cu: goto block_8017FB2C;
            default: break;
        }
    }
    /* Address: 0x8017FB00, Size: 52 bytes, Blocks: 4 */

block_8017FB00:
    cpu->gpr[2] = cpu->read_word(cpu->gpr[6] + 32);
    /* nop */  /* 0x8017FB04: 0x00000000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0800;  /* 0x8017FB08: 0x30420800 */
    int _bc_8017FB0C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[4] = 0x0004 << 16;  /* 0x00040000 */
    if (_bc_8017FB0C) {
        psx_check_interrupts_at(cpu, 0x8017FB28u);
        goto block_8017FB28;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FB14u);
        goto block_8017FB14;  /* not taken */
    }

block_8017FB14:
    cpu->gpr[3] = cpu->read_word(cpu->gpr[5] + 92);
    /* nop */  /* 0x8017FB18: 0x00000000 */
    cpu->gpr[3] = cpu->gpr[3] & cpu->gpr[4];  /* 0x8017FB1C: 0x00641824 */
    int _bc_8017FB20 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[2] = 1;
    if (_bc_8017FB20) {
        psx_check_interrupts_at(cpu, 0x8017FB2Cu);
        goto block_8017FB2C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017FB28u);
        goto block_8017FB28;  /* not taken */
    }

block_8017FB28:
    cpu->gpr[2] = -1;
    /* fall through to block_8017FB2C */

block_8017FB2C:
    uint32_t _jt_8017FB2C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017FB30: 0x00000000 */
    psx_check_interrupts_at(cpu, _jt_8017FB2C);
    cpu->pc = _jt_8017FB2C; return;  /* CPS: jr $ra */
}
