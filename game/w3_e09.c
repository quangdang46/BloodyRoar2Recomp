#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* reset_guest_cps — clean CPS transliteration of br2eu_func_80175898 (guest 0x80175964)
 * from SLES_017.22_full_32.c. */
void reset_guest_cps(void)
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
    int cond_v0_eq_zero = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F80 << 16;  /* 0x1F800000 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80175A74u);
        cpu->pc = 0x80175A74u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80175944u);
    }

block_80175944:
    v0 = 0x8012 << 16;  /* 0x80120000 */
    v0 = v0 + 9880;
    v1 = v1 << 2;
    v1 = v1 + v0;
    v0 = cpu->read_word(v1);
    /* nop */  /* 0x80175958: 0x00000000 */
    uint32_t jr_target = v0;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x80175960: 0x00000000 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr v0 */

block_80175964:
    v0 = 7680;
    cpu->write_half(a2 + 214, (uint16_t)v0);
    v1 = cpu->read_byte(a0 + 155);
    v0 = 4;
    int cond_v1_ne_v0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 7680;
    if (cond_v1_ne_v0) {
        psx_check_interrupts_at(cpu, 0x80175980u);
        goto block_80175980;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8017597Cu);
    }

block_8017597C:
    cpu->write_half(a2 + 214, (uint16_t)v0);
    /* fall through to block_80175980 */

block_80175980:
    v1 = cpu->read_byte(a0 + 156);
    v0 = 3;
    int cond_v1_ne_v0_2 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017598C: 0x00000000 */
    if (cond_v1_ne_v0_2) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80175990u);
    }

block_80175990:
    v0 = cpu->read_byte(a2 + 1);
    /* nop */  /* 0x80175994: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8017599C: 0x00000000 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801759A0u);
    }

block_801759A0:
    v0 = cpu->read_half(a0 + 162);
    /* nop */  /* 0x801759A4: 0x00000000 */
    int cond_v0_eq_zero_2 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x801759AC: 0x00000000 */
    if (cond_v0_eq_zero_2) {
        psx_check_interrupts_at(cpu, 0x801759C4u);
        goto block_801759C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801759B0u);
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
    int cond_a0_ge_0 = ((int32_t)a0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a2 + 176, (uint16_t)v0);
    if (cond_a0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80175A74u);
        cpu->pc = 0x80175A74u; return;  /* CPS taken: split */
    } else {
        psx_check_interrupts_at(cpu, 0x80175A10u);
    }

block_80175A10:;
    uint32_t jr_target_2 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    cpu->write_half(a2 + 214, (uint16_t)zero);
    psx_check_interrupts_at(cpu, jr_target_2);
    cpu->pc = jr_target_2; return;  /* CPS: jr $ra */
}
