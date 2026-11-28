/* recon_8015.c — clean-room reconstructions of guest functions in the
 * 0x8015xxxx band: the three menu-stepper row callbacks installed into
 * stepper descriptors by save_flow_state_machine (0x8015D420 -> cb
 * 0x8015D69C), options_state_machine (0x8015EBF8 -> cb 0x8015EE40) and
 * screen_flow_state_machine (0x8016088C -> cb 0x80160A64).
 *
 * These are invoked TWO ways (menu_stepper_state_advance /
 * menu_stepper_masked_sweep / the button-wait variant):
 *   - fn_lookup direct C call cb(desc_host_ptr, sel) — the primary path;
 *   - call_guest -> clean_try_dispatch when fn_lookup misses.
 * The guest contract is a plain `jalr` call that runs to completion, so
 * the ports are ATOMIC clean ports (recon_80138.c anim_workblock_tick
 * style): every guest `jal` runs synchronously through rs_jal (clean
 * port when covered, generated body otherwise, full nested dispatch so
 * per-block IRQ checks + coroutine yields inside callees are preserved),
 * intra-function edges stay plain gotos behind the same
 * psx_check_interrupts_at resume points the CPS pieces used, and the
 * whole clean_cps.map cluster is retired — an IRQ taken at a check
 * resumes through the generated block at resume_pc, exactly like the
 * CPS coroutine did. All mutable state lives in cpu->gpr[]/guest RAM
 * so scheduler longjmps preserve it.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern int      br2_bail_pending(void);
extern int      clean_try_dispatch(CPUState *cpu, uint32_t addr);
extern void     psx_dispatch_call(CPUState *cpu, uint32_t target_addr,
                                  uint32_t return_addr);
extern uint32_t br2_gaddr(const void *);

/* rs_jal — faithful guest `jal` from clean C: dispatch the callee
 * synchronously (clean port when one exists, generated body otherwise),
 * return its v0. Same contract as recon_80138.c's wb_jal / ev.c's br2_jal.
 * Defined before the GPR #defines so its locals may use ra/pc/sp names. */
static uint32_t rs_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
    psx_dispatch_call(cpu, fn, link);
    cpu->gpr[29] = sp;
    {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
}

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

/* ======== options_menu_stepper_cb (guest 0x8015EE40) ========
 * Options-menu stepper row callback (96 CPS blocks / 1872 bytes):
 * installed as blk+0x0c by options_state_machine; a0 = stepper desc
 * guest addr (host ptr on the fn_lookup leg — normalised through
 * br2_gaddr), a1 = row selector (u16 @+8, or 0x100+/0x1ff sweep codes).
 * Scroll offsets, HUD text, announce lines, sprite attr allocs, gauge
 * scale, display invalidate/commit pair, frame_tick/frame_present. */
uint32_t options_menu_stepper_cb(intptr_t a0_, uint32_t a1_)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;
    cpu->gpr[4] = br2_gaddr((const void *)a0_);
    cpu->gpr[5] = a1_;

block_8015EE40:
    sp -= 72;  /* 0x8015EE40: 0x27BDFFB8 */
    cpu->write_word(sp + 60, s3);  /* 0x8015EE44: 0xAFB3003C */
    s3 = a0;  /* 0x8015EE48: 0x00809821 */
    a0 = 32;  /* 0x8015EE4C: 0x24040020 */
    cpu->write_word(sp + 48, s0);  /* 0x8015EE50: 0xAFB00030 */
    s0 = a1;  /* 0x8015EE54: 0x00A08021 */
    a1 = -1;  /* 0x8015EE58: 0x2405FFFF */
    cpu->write_word(sp + 68, ra);  /* 0x8015EE5C: 0xAFBF0044 */
    cpu->write_word(sp + 64, s4);  /* 0x8015EE60: 0xAFB40040 */
    cpu->write_word(sp + 56, s2);  /* 0x8015EE64: 0xAFB20038 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 52, s1);  /* 0x8015EE6C: 0xAFB10034 */
    rs_jal(0x8012CD9Cu, 0x8015EE70u);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015EE70u);
    goto block_8015EE70;

block_8015EE70:
    v0 = cpu->read_half(s3 + 8);  /* 0x8015EE70: 0x96620008 */
    v1 = s0 & 0xFFFF;  /* 0x8015EE74: 0x3203FFFF */
    v0 += 256;  /* 0x8015EE78: 0x24420100 */
    int cond_8015EE7C = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s4 = 320;  /* 0x8015EE80: 0x24140140 */
    if (cond_8015EE7C) {
        psx_check_interrupts_at(cpu, 0x8015EEB0u);
        goto block_8015EEB0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EE84u);
    }

block_8015EE84:
    s1 = zero;  /* 0x8015EE84: 0x00008821 */
    a2 = 0x80121270u;  /* 0x80120000 4720 */  /* 0x80120000 */  /* 0x8015EE88: 0x3C068012 */  /* 0x8015EE8C: 0x24C61270 */
    a3 = 4096;  /* 0x8015EE90: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015EE94: 0x3C021F80 */
    a1 = cpu->read_word(v0 + 12);  /* 0x8015EE98: 0x8C45000C */
    a0 = cpu->read_word(v0 + 8);  /* 0x8015EE9C: 0x8C440008 */
    v0 = a3;  /* 0x8015EEA0: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015EEA4: 0xAFA20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x8015EEAC: 0xAFA00014 */
    psx_check_interrupts_at(cpu, 0x8015EED8u);
    goto block_8015EED8;  /* j */

block_8015EEB0:
    s1 = 1;  /* 0x8015EEB0: 0x24110001 */
    a2 = 0x80121274u;  /* 0x80120000 4724 */  /* 0x80120000 */  /* 0x8015EEB4: 0x3C068012 */  /* 0x8015EEB8: 0x24C61274 */
    a3 = 4096;  /* 0x8015EEBC: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015EEC0: 0x3C021F80 */
    a1 = cpu->read_word(v0 + 12);  /* 0x8015EEC4: 0x8C45000C */
    a0 = cpu->read_word(v0 + 8);  /* 0x8015EEC8: 0x8C440008 */
    v0 = a3;  /* 0x8015EECC: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015EED0: 0xAFA20010 */
    cpu->write_word(sp + 20, s1);  /* 0x8015EED4: 0xAFB10014 */
    /* fall through to block_8015EED8 */

block_8015EED8:
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015EEDC: 0x24840004 */
    rs_jal(0x8012E61Cu, 0x8015EEE0u);  /* jal hud_text_b */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015EEE0u);
    goto block_8015EEE0;

block_8015EEE0:
    v1 = s0 & 0xFFFF;  /* 0x8015EEE0: 0x3203FFFF */
    v0 = 256;  /* 0x8015EEE4: 0x24020100 */
    int cond_8015EEE8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 257;  /* 0x8015EEEC: 0x28620101 */
    if (cond_8015EEE8) {
        psx_check_interrupts_at(cpu, 0x8015EF40u);
        goto block_8015EF40;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EEF0u);
    }

block_8015EEF0:
    ;
    int cond_8015EEF0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;  /* 0x8015EEF4: 0x24020002 */
    if (cond_8015EEF0) {
        psx_check_interrupts_at(cpu, 0x8015EF18u);
        goto block_8015EF18;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EEF8u);
    }

block_8015EEF8:
    ;
    int cond_8015EEF8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 3;  /* 0x8015EEFC: 0x28620003 */
    if (cond_8015EEF8) {
        psx_check_interrupts_at(cpu, 0x8015F20Cu);
        goto block_8015F20C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EF00u);
    }

block_8015EF00:
    ;
    int cond_8015EF00 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;  /* 0x8015EF04: 0x24020001 */
    if (cond_8015EF00) {
        psx_check_interrupts_at(cpu, 0x8015F56Cu);
        goto block_8015F56C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EF08u);
    }

block_8015EF08:
    ;
    int cond_8015EF08 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015EF0C: 0x3C041F80 */
    if (cond_8015EF08) {
        psx_check_interrupts_at(cpu, 0x8015EF98u);
        goto block_8015EF98;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EF10u);
    }

block_8015EF10:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015EF14: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015F56Cu);
    goto block_8015F56C;  /* j */

block_8015EF18:
    v0 = 258;  /* 0x8015EF18: 0x24020102 */
    int cond_8015EF1C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < (int32_t)v0;  /* 0x8015EF20: 0x0062102A */
    if (cond_8015EF1C) {
        psx_check_interrupts_at(cpu, 0x8015F39Cu);
        goto block_8015F39C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EF24u);
    }

block_8015EF24:
    ;
    int cond_8015EF24 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015EF28: 0x3C068012 */
    if (cond_8015EF24) {
        psx_check_interrupts_at(cpu, 0x8015F1B4u);
        goto block_8015F1B4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EF2Cu);
    }

block_8015EF2C:
    v0 = 259;  /* 0x8015EF2C: 0x24020103 */
    int cond_8015EF30 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015EF34: 0x3C068012 */
    if (cond_8015EF30) {
        psx_check_interrupts_at(cpu, 0x8015F534u);
        goto block_8015F534;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EF38u);
    }

block_8015EF38:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015EF3C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015F56Cu);
    goto block_8015F56C;  /* j */

block_8015EF40:
    a0 = -1;  /* 0x8015EF40: 0x2404FFFF */
    /* delay slot (always executes) */
    a1 = 24;  /* 0x8015EF48: 0x24050018 */
    rs_jal(0x8012CD9Cu, 0x8015EF4Cu);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015EF4Cu);
    goto block_8015EF4C;

block_8015EF4C:
    a2 = 0x801213F0u;  /* 0x80120000 5104 */  /* 0x80120000 */  /* 0x8015EF4C: 0x3C068012 */  /* 0x8015EF50: 0x24C613F0 */
    a3 = 4096;  /* 0x8015EF54: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015EF58: 0x3C021F80 */
    a1 = cpu->read_word(v0 + 12);  /* 0x8015EF5C: 0x8C45000C */
    a0 = cpu->read_word(v0 + 8);  /* 0x8015EF60: 0x8C440008 */
    v0 = a3;  /* 0x8015EF64: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015EF68: 0xAFA20010 */
    cpu->write_word(sp + 20, zero);  /* 0x8015EF6C: 0xAFA00014 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015EF74: 0x24840004 */
    rs_jal(0x8012E58Cu, 0x8015EF78u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015EF78u);
    goto block_8015EF78;

block_8015EF78:
    a0 = 7;  /* 0x8015EF78: 0x24040007 */
    a1 = zero;  /* 0x8015EF7C: 0x00002821 */
    a2 = 4;  /* 0x8015EF80: 0x24060004 */
    /* delay slot (always executes) */
    a3 = a0;  /* 0x8015EF88: 0x00803821 */
    rs_jal(0x8012BF0Cu, 0x8015EF8Cu);  /* jal banner_stripe_scroll */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015EF8Cu);
    goto block_8015EF8C;

block_8015EF8C:
    a0 = zero;  /* 0x8015EF8C: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 16;  /* 0x8015EF94: 0x24050010 */
    psx_check_interrupts_at(cpu, 0x8015F564u);
    goto block_8015F564;  /* j */

block_8015EF98:
    v1 = cpu->read_half(a0 + 126);  /* 0x8015EF98: 0x9483007E */
    /* nop */  /* 0x8015EF9C: 0x00000000 */
    v0 = v1 & 0xA000;  /* 0x8015EFA0: 0x3062A000 */
    int cond_8015EFA4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x2000;  /* 0x8015EFA8: 0x30622000 */
    if (cond_8015EFA4) {
        psx_check_interrupts_at(cpu, 0x8015F0B0u);
        goto block_8015F0B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EFACu);
    }

block_8015EFAC:
    a3 = (int32_t)(int16_t)cpu->read_half(s3 + 20);  /* 0x8015EFAC: 0x86670014 */
    v1 = cpu->read_half(s3 + 20);  /* 0x8015EFB0: 0x96630014 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015EFB8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015EFC8u);
    goto block_8015EFC8;  /* j */

block_8015EFBC:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 20);  /* 0x8015EFBC: 0x86620014 */
    v1 = cpu->read_half(s3 + 20);  /* 0x8015EFC0: 0x96630014 */
    v0 = (int32_t)v0 < 3;  /* 0x8015EFC4: 0x28420003 */
    /* fall through to block_8015EFC8 */

block_8015EFC8:
    ;
    int cond_8015EFC8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1++;  /* 0x8015EFCC: 0x24630001 */
    if (cond_8015EFC8) {
        psx_check_interrupts_at(cpu, 0x8015EFF0u);
        goto block_8015EFF0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EFD0u);
    }

block_8015EFD0:
    v0 = cpu->read_word(a0 + 192);  /* 0x8015EFD0: 0x8C8200C0 */
    cpu->write_half(s3 + 20, (uint16_t)v1);  /* 0x8015EFD4: 0xA6630014 */
    v0 = cpu->read_word(v0 + 40);  /* 0x8015EFD8: 0x8C420028 */
    /* nop */  /* 0x8015EFDC: 0x00000000 */
    v0 = (uint32_t)v0 >> (v1 & 0x1F);  /* 0x8015EFE0: 0x00621006 */
    v0 &= 0x0001;  /* 0x8015EFE4: 0x30420001 */
    int cond_8015EFE8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015EFEC: 0x00000000 */
    if (cond_8015EFE8) {
        psx_check_interrupts_at(cpu, 0x8015EFBCu);
        goto block_8015EFBC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015EFF0u);
    }

block_8015EFF0:
    v1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015EFF0: 0x3C031F80 */
    v0 = cpu->read_half(v1 + 126);  /* 0x8015EFF4: 0x9462007E */
    /* nop */  /* 0x8015EFF8: 0x00000000 */
    v0 &= 0x8000;  /* 0x8015EFFC: 0x30428000 */
    int cond_8015F000 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F004: 0x00000000 */
    if (cond_8015F000) {
        psx_check_interrupts_at(cpu, 0x8015F064u);
        goto block_8015F064;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F008u);
    }

block_8015F008:
    v0 = cpu->read_half(s3 + 20);  /* 0x8015F008: 0x96620014 */
    v1 = cpu->read_word(v1 + 192);  /* 0x8015F00C: 0x8C6300C0 */
    v0--;  /* 0x8015F010: 0x2442FFFF */
    cpu->write_half(s3 + 20, (uint16_t)v0);  /* 0x8015F014: 0xA6620014 */
    v1 = cpu->read_word(v1 + 40);  /* 0x8015F018: 0x8C630028 */
    /* nop */  /* 0x8015F01C: 0x00000000 */
    v1 = (uint32_t)v1 >> (v0 & 0x1F);  /* 0x8015F020: 0x00431806 */
    v1 &= 0x0001;  /* 0x8015F024: 0x30630001 */
    int cond_8015F028 = (v1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F02C: 0x00000000 */
    if (cond_8015F028) {
        psx_check_interrupts_at(cpu, 0x8015F064u);
        goto block_8015F064;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F030u);
    }

block_8015F030:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F030: 0x3C041F80 */
    /* fall through to block_8015F034 */

block_8015F034:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 20);  /* 0x8015F034: 0x86620014 */
    v1 = cpu->read_half(s3 + 20);  /* 0x8015F038: 0x96630014 */
    int cond_8015F03C = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1--;  /* 0x8015F040: 0x2463FFFF */
    if (cond_8015F03C) {
        psx_check_interrupts_at(cpu, 0x8015F064u);
        goto block_8015F064;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F044u);
    }

block_8015F044:
    v0 = cpu->read_word(a0 + 192);  /* 0x8015F044: 0x8C8200C0 */
    cpu->write_half(s3 + 20, (uint16_t)v1);  /* 0x8015F048: 0xA6630014 */
    v0 = cpu->read_word(v0 + 40);  /* 0x8015F04C: 0x8C420028 */
    /* nop */  /* 0x8015F050: 0x00000000 */
    v0 = (uint32_t)v0 >> (v1 & 0x1F);  /* 0x8015F054: 0x00621006 */
    v0 &= 0x0001;  /* 0x8015F058: 0x30420001 */
    int cond_8015F05C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F060: 0x00000000 */
    if (cond_8015F05C) {
        psx_check_interrupts_at(cpu, 0x8015F034u);
        goto block_8015F034;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F064u);
    }

block_8015F064:
    v1 = cpu->read_half(s3 + 20);  /* 0x8015F064: 0x96630014 */
    /* nop */  /* 0x8015F068: 0x00000000 */
    v0 = v1 < (uint32_t)4;  /* 0x8015F06C: 0x2C620004 */
    int cond_8015F070 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F074: 0x00000000 */
    if (cond_8015F070) {
        psx_check_interrupts_at(cpu, 0x8015F09Cu);
        goto block_8015F09C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F078u);
    }

block_8015F078:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F078: 0x3C021F80 */
    v0 = cpu->read_word(v0 + 192);  /* 0x8015F07C: 0x8C4200C0 */
    /* nop */  /* 0x8015F080: 0x00000000 */
    v0 = cpu->read_word(v0 + 40);  /* 0x8015F084: 0x8C420028 */
    /* nop */  /* 0x8015F088: 0x00000000 */
    v0 = (uint32_t)v0 >> (v1 & 0x1F);  /* 0x8015F08C: 0x00621006 */
    v0 &= 0x0001;  /* 0x8015F090: 0x30420001 */
    int cond_8015F094 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;  /* 0x8015F098: 0x24040400 */
    if (cond_8015F094) {
        psx_check_interrupts_at(cpu, 0x8015F0A4u);
        goto block_8015F0A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F09Cu);
    }

block_8015F09C:
    /* delay slot (always executes) */
    cpu->write_half(s3 + 20, (uint16_t)a3);  /* 0x8015F0A0: 0xA6670014 */
    psx_check_interrupts_at(cpu, 0x8015F0B0u);
    goto block_8015F0B0;  /* j */

block_8015F0A4:
    a1 = 11;  /* 0x8015F0A4: 0x2405000B */
    /* delay slot (always executes) */
    a2 = zero;  /* 0x8015F0AC: 0x00003021 */
    rs_jal(0x80156EF4u, 0x8015F0B0u);  /* jal deferred_cmd_post */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F0B0u);
    goto block_8015F0B0;

block_8015F0B0:
    s0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F0B0: 0x3C101F80 */
    v0 = cpu->read_half(s0 + 124);  /* 0x8015F0B4: 0x9602007C */
    /* nop */  /* 0x8015F0B8: 0x00000000 */
    v0 &= 0x00E0;  /* 0x8015F0BC: 0x304200E0 */
    int cond_8015F0C0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;  /* 0x8015F0C4: 0x24040400 */
    if (cond_8015F0C0) {
        psx_check_interrupts_at(cpu, 0x8015F56Cu);
        goto block_8015F56C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F0C8u);
    }

block_8015F0C8:
    a1 = 12;  /* 0x8015F0C8: 0x2405000C */
    /* delay slot (always executes) */
    a2 = zero;  /* 0x8015F0D0: 0x00003021 */
    rs_jal(0x80156EF4u, 0x8015F0D4u);  /* jal deferred_cmd_post */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F0D4u);
    goto block_8015F0D4;

block_8015F0D4:
    /* delay slot (always executes) */
    a0 = s0;  /* 0x8015F0D8: 0x02002021 */
    rs_jal(0x8012AEECu, 0x8015F0DCu);  /* jal frame_tick */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F0DCu);
    goto block_8015F0DC;

block_8015F0DC:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 20);  /* 0x8015F0DC: 0x86620014 */
    /* nop */  /* 0x8015F0E0: 0x00000000 */
    int cond_8015F0E4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 0x80010000u;  /* 0x80010000 */  /* 0x8015F0E8: 0x3C048001 */
    if (cond_8015F0E4) {
        psx_check_interrupts_at(cpu, 0x8015F150u);
        goto block_8015F150;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F0ECu);
    }

block_8015F0EC:
    a2 = cpu->read_word(s0 + 192);  /* 0x8015F0EC: 0x8E0600C0 */
    a0 = 0x51EB0000u;  /* 0x51EB0000 */  /* 0x8015F0F0: 0x3C0451EB */
    v1 = cpu->read_byte(a2 + 15);  /* 0x8015F0F4: 0x90C3000F */
    a0 |= 0x851F;  /* 0x8015F0F8: 0x3484851F */
    v0 = v1 << 5;  /* 0x8015F0FC: 0x00031140 */
    v0 += v1;  /* 0x8015F100: 0x00431021 */
    v0 <<= 2;  /* 0x8015F104: 0x00021080 */
    v0 += v1;  /* 0x8015F108: 0x00431021 */
    v0 <<= 1;  /* 0x8015F10C: 0x00021040 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8015F110: 0x00440018 */
    v1 = 0x66660000u;  /* 0x66660000 */  /* 0x8015F114: 0x3C036666 */
    v1 |= 0x6667;  /* 0x8015F118: 0x34636667 */
    v0 = (int32_t)v0 >> 31;  /* 0x8015F11C: 0x000217C3 */
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8015F120: 0x00004810 */
    a1 = (int32_t)t1 >> 5;  /* 0x8015F124: 0x00092943 */
    a1 -= v0;  /* 0x8015F128: 0x00A22823 */
    a1 <<= 3;  /* 0x8015F12C: 0x000528C0 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));  /* 0x8015F130: 0x00A30018 */
    a0 = cpu->read_byte(a2 + 14);  /* 0x8015F134: 0x90C4000E */
    a1 = (int32_t)a1 >> 31;  /* 0x8015F138: 0x00052FC3 */
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8015F13C: 0x00004810 */
    v0 = (int32_t)t1 >> 2;  /* 0x8015F140: 0x00091083 */
    /* delay slot (always executes) */
    a1 = v0 - a1;  /* 0x8015F148: 0x00452823 */
    rs_jal(0x80156048u, 0x8015F14Cu);  /* jal gauge_scale_fixed */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F14Cu);
    goto block_8015F14C;

block_8015F14C:
    a0 = 0x80010000u;  /* 0x80010000 */  /* 0x8015F14C: 0x3C048001 */
    /* fall through to block_8015F150 */

block_8015F150:
    a1 = (int32_t)(int16_t)cpu->read_half(s3 + 20);  /* 0x8015F150: 0x86650014 */
    /* delay slot (always executes) */
    a2 = 1;  /* 0x8015F158: 0x24060001 */
    rs_jal(0x80159028u, 0x8015F15Cu);  /* jal scene_entry */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F15Cu);
    goto block_8015F15C;

block_8015F15C:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 20);  /* 0x8015F15C: 0x86620014 */
    /* nop */  /* 0x8015F160: 0x00000000 */
    int cond_8015F164 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = 0x66660000u;  /* 0x66660000 */  /* 0x8015F168: 0x3C036666 */
    if (cond_8015F164) {
        psx_check_interrupts_at(cpu, 0x8015F19Cu);
        goto block_8015F19C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F16Cu);
    }

block_8015F16C:
    v0 = cpu->read_word(s0 + 192);  /* 0x8015F16C: 0x8E0200C0 */
    /* nop */  /* 0x8015F170: 0x00000000 */
    a1 = cpu->read_byte(v0 + 15);  /* 0x8015F174: 0x9045000F */
    v1 |= 0x6667;  /* 0x8015F178: 0x34636667 */
    a1 <<= 3;  /* 0x8015F17C: 0x000528C0 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));  /* 0x8015F180: 0x00A30018 */
    a0 = cpu->read_byte(v0 + 14);  /* 0x8015F184: 0x9044000E */
    a1 = (int32_t)a1 >> 31;  /* 0x8015F188: 0x00052FC3 */
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8015F18C: 0x00004810 */
    v0 = (int32_t)t1 >> 2;  /* 0x8015F190: 0x00091083 */
    /* delay slot (always executes) */
    a1 = v0 - a1;  /* 0x8015F198: 0x00452823 */
    rs_jal(0x80156048u, 0x8015F19Cu);  /* jal gauge_scale_fixed */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F19Cu);
    goto block_8015F19C;

block_8015F19C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F1A0: 0x00000000 */
    rs_jal(0x8015E87Cu, 0x8015F1A4u);  /* jal display_cur_invalidate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F1A4u);
    goto block_8015F1A4;

block_8015F1A4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F1A8: 0x00000000 */
    rs_jal(0x8015E88Cu, 0x8015F1ACu);  /* jal display_pair_invalidate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F1ACu);
    goto block_8015F1AC;

block_8015F1AC:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F1B0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015F56Cu);
    goto block_8015F56C;  /* j */

block_8015F1B4:
    a3 = 4096;  /* 0x8015F1B4: 0x24071000 */
    s0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F1B8: 0x3C101F80 */
    a1 = cpu->read_word(s0 + 12);  /* 0x8015F1BC: 0x8E05000C */
    s2 = a3;  /* 0x8015F1C0: 0x00E09021 */
    cpu->write_word(sp + 16, s2);  /* 0x8015F1C4: 0xAFB20010 */
    a0 = cpu->read_word(s0 + 8);  /* 0x8015F1C8: 0x8E040008 */
    a2 += 5968;  /* 0x8015F1CC: 0x24C61750 */
    cpu->write_word(sp + 20, s1);  /* 0x8015F1D0: 0xAFB10014 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015F1D8: 0x24840004 */
    rs_jal(0x8012E58Cu, 0x8015F1DCu);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F1DCu);
    goto block_8015F1DC;

block_8015F1DC:
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x8015F1DC: 0x3C02801B */
    a1 = cpu->read_word(s0 + 12);  /* 0x8015F1E0: 0x8E05000C */
    v1 = (int32_t)(int16_t)cpu->read_half(s3 + 20);  /* 0x8015F1E4: 0x86630014 */
    v0 += 16684;  /* 0x8015F1E8: 0x2442412C */
    cpu->write_word(sp + 16, s2);  /* 0x8015F1EC: 0xAFB20010 */
    cpu->write_word(sp + 20, s1);  /* 0x8015F1F0: 0xAFB10014 */
    a0 = cpu->read_word(s0 + 8);  /* 0x8015F1F4: 0x8E040008 */
    v1 <<= 2;  /* 0x8015F1F8: 0x00031880 */
    v1 += v0;  /* 0x8015F1FC: 0x00621821 */
    a2 = cpu->read_word(v1);  /* 0x8015F200: 0x8C660000 */
    /* delay slot (always executes) */
    a3 = s4;  /* 0x8015F208: 0x02803821 */
    psx_check_interrupts_at(cpu, 0x8015F554u);
    goto block_8015F554;  /* j */

block_8015F20C:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 26);  /* 0x8015F20C: 0x8662001A */
    /* nop */  /* 0x8015F210: 0x00000000 */
    int cond_8015F214 = ((int32_t)v0 <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F218: 0x00000000 */
    if (cond_8015F214) {
        psx_check_interrupts_at(cpu, 0x8015F2B0u);
        goto block_8015F2B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F21Cu);
    }

block_8015F21C:
    v1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F21C: 0x3C031F80 */
    v1 = cpu->read_half(v1 + 126);  /* 0x8015F220: 0x9463007E */
    /* nop */  /* 0x8015F224: 0x00000000 */
    v0 = v1 & 0xA000;  /* 0x8015F228: 0x3062A000 */
    int cond_8015F22C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x2000;  /* 0x8015F230: 0x30622000 */
    if (cond_8015F22C) {
        psx_check_interrupts_at(cpu, 0x8015F2B0u);
        goto block_8015F2B0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F234u);
    }

block_8015F234:
    ;
    int cond_8015F234 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = v1 & 0x8000;  /* 0x8015F238: 0x30628000 */
    if (cond_8015F234) {
        psx_check_interrupts_at(cpu, 0x8015F26Cu);
        goto block_8015F26C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F23Cu);
    }

block_8015F23C:
    v0 = cpu->read_half(s3 + 22);  /* 0x8015F23C: 0x96620016 */
    v1 = (int32_t)(int16_t)cpu->read_half(s3 + 26);  /* 0x8015F240: 0x8663001A */
    v0++;  /* 0x8015F244: 0x24420001 */
    cpu->write_half(s3 + 22, (uint16_t)v0);  /* 0x8015F248: 0xA6620016 */
    v0 = (int32_t)(int16_t)v0;  /* 0x8015F24C: 0x00021400 */  /* 0x8015F250: 0x00021403 */
    v1--;  /* 0x8015F254: 0x2463FFFF */
    v1 = (int32_t)v1 < (int32_t)v0;  /* 0x8015F258: 0x0062182A */
    int cond_8015F25C = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;  /* 0x8015F260: 0x24040400 */
    if (cond_8015F25C) {
        psx_check_interrupts_at(cpu, 0x8015F2A4u);
        goto block_8015F2A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F264u);
    }

block_8015F264:
    /* delay slot (always executes) */
    cpu->write_half(s3 + 22, (uint16_t)zero);  /* 0x8015F268: 0xA6600016 */
    psx_check_interrupts_at(cpu, 0x8015F2A4u);
    goto block_8015F2A4;  /* j */

block_8015F26C:
    ;
    int cond_8015F26C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;  /* 0x8015F270: 0x24040400 */
    if (cond_8015F26C) {
        psx_check_interrupts_at(cpu, 0x8015F2A4u);
        goto block_8015F2A4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F274u);
    }

block_8015F274:
    v0 = cpu->read_half(s3 + 22);  /* 0x8015F274: 0x96620016 */
    /* nop */  /* 0x8015F278: 0x00000000 */
    v0--;  /* 0x8015F27C: 0x2442FFFF */
    cpu->write_half(s3 + 22, (uint16_t)v0);  /* 0x8015F280: 0xA6620016 */
    v0 <<= 16;  /* 0x8015F284: 0x00021400 */
    int cond_8015F288 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = 11;  /* 0x8015F28C: 0x2405000B */
    if (cond_8015F288) {
        psx_check_interrupts_at(cpu, 0x8015F2A8u);
        goto block_8015F2A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F290u);
    }

block_8015F290:
    v0 = cpu->read_half(s3 + 26);  /* 0x8015F290: 0x9662001A */
    /* nop */  /* 0x8015F294: 0x00000000 */
    v0--;  /* 0x8015F298: 0x2442FFFF */
    cpu->write_half(s3 + 22, (uint16_t)v0);  /* 0x8015F29C: 0xA6620016 */
    a0 = 1024;  /* 0x8015F2A0: 0x24040400 */
    /* fall through to block_8015F2A4 */

block_8015F2A4:
    a1 = 11;  /* 0x8015F2A4: 0x2405000B */
    /* fall through to block_8015F2A8 */

block_8015F2A8:
    /* delay slot (always executes) */
    a2 = zero;  /* 0x8015F2AC: 0x00003021 */
    rs_jal(0x80156EF4u, 0x8015F2B0u);  /* jal deferred_cmd_post */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F2B0u);
    goto block_8015F2B0;

block_8015F2B0:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F2B0: 0x3C021F80 */
    v0 = cpu->read_half(v0 + 124);  /* 0x8015F2B4: 0x9442007C */
    /* nop */  /* 0x8015F2B8: 0x00000000 */
    v0 &= 0x00E0;  /* 0x8015F2BC: 0x304200E0 */
    int cond_8015F2C0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;  /* 0x8015F2C4: 0x24040400 */
    if (cond_8015F2C0) {
        psx_check_interrupts_at(cpu, 0x8015F36Cu);
        goto block_8015F36C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F2C8u);
    }

block_8015F2C8:
    a1 = 12;  /* 0x8015F2C8: 0x2405000C */
    /* delay slot (always executes) */
    a2 = zero;  /* 0x8015F2D0: 0x00003021 */
    rs_jal(0x80156EF4u, 0x8015F2D4u);  /* jal deferred_cmd_post */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F2D4u);
    goto block_8015F2D4;

block_8015F2D4:
    /* delay slot (always executes) */
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F2D8: 0x3C041F80 */
    rs_jal(0x8012B23Cu, 0x8015F2DCu);  /* jal frame_present */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F2DCu);
    goto block_8015F2DC;

block_8015F2DC:
    a3 = zero;  /* 0x8015F2DC: 0x00003821 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 22);  /* 0x8015F2E0: 0x86620016 */
    t0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F2E4: 0x3C081F80 */
    a1 = v0 + 1;  /* 0x8015F2E8: 0x24450001 */
    /* fall through to block_8015F2EC */

block_8015F2EC:
    ;
    int cond_8015F2EC = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015F2F0: 0x00002021 */
    if (cond_8015F2EC) {
        psx_check_interrupts_at(cpu, 0x8015F34Cu);
        goto block_8015F34C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F2F4u);
    }

block_8015F2F4:
    a2 = a3 << 5;  /* 0x8015F2F4: 0x00073140 */
    v0 = cpu->read_word(t0 + 192);  /* 0x8015F2F8: 0x8D0200C0 */
    v1 = a3 << 2;  /* 0x8015F2FC: 0x00071880 */
    v0 += v1;  /* 0x8015F300: 0x00431021 */
    v1 = cpu->read_word(v0 + 84);  /* 0x8015F304: 0x8C430054 */
    /* fall through to block_8015F308 */

block_8015F308:
    ;
    int cond_8015F308 = (a1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (uint32_t)v1 >> (a0 & 0x1F);  /* 0x8015F30C: 0x00831006 */
    if (cond_8015F308) {
        psx_check_interrupts_at(cpu, 0x8015F33Cu);
        goto block_8015F33C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F310u);
    }

block_8015F310:
    v0 &= 0x0001;  /* 0x8015F310: 0x30420001 */
    int cond_8015F314 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F318: 0x00000000 */
    if (cond_8015F314) {
        psx_check_interrupts_at(cpu, 0x8015F32Cu);
        goto block_8015F32C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F31Cu);
    }

block_8015F31C:
    a1--;  /* 0x8015F31C: 0x24A5FFFF */
    int cond_8015F320 = (a1 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a2 + a0;  /* 0x8015F324: 0x00C41021 */
    if (cond_8015F320) {
        psx_check_interrupts_at(cpu, 0x8015F32Cu);
        goto block_8015F32C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F328u);
    }

block_8015F328:
    cpu->write_half(s3 + 34, (uint16_t)v0);  /* 0x8015F328: 0xA6620022 */
    /* fall through to block_8015F32C */

block_8015F32C:
    a0++;  /* 0x8015F32C: 0x24840001 */
    v0 = (int32_t)a0 < 32;  /* 0x8015F330: 0x28820020 */
    int cond_8015F334 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F338: 0x00000000 */
    if (cond_8015F334) {
        psx_check_interrupts_at(cpu, 0x8015F308u);
        goto block_8015F308;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F33Cu);
    }

block_8015F33C:
    a3++;  /* 0x8015F33C: 0x24E70001 */
    v0 = (int32_t)a3 < 3;  /* 0x8015F340: 0x28E20003 */
    int cond_8015F344 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F348: 0x00000000 */
    if (cond_8015F344) {
        psx_check_interrupts_at(cpu, 0x8015F2ECu);
        goto block_8015F2EC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F34Cu);
    }

block_8015F34C:
    v1 = (int32_t)(int16_t)cpu->read_half(s3 + 24);  /* 0x8015F34C: 0x86630018 */
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 34);  /* 0x8015F350: 0x86620022 */
    a0 = cpu->read_half(s3 + 34);  /* 0x8015F354: 0x96640022 */
    int cond_8015F358 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F35C: 0x00000000 */
    if (cond_8015F358) {
        psx_check_interrupts_at(cpu, 0x8015F36Cu);
        goto block_8015F36C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F360u);
    }

block_8015F360:
    cpu->write_half(s3 + 24, (uint16_t)a0);  /* 0x8015F360: 0xA6640018 */
    at = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F364: 0x3C011F80 */
    cpu->write_half(at + 140, (uint16_t)zero);  /* 0x8015F368: 0xA420008C */
    /* fall through to block_8015F36C */

block_8015F36C:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F36C: 0x3C021F80 */
    v0 = (int32_t)(int16_t)cpu->read_half(v0 + 140);  /* 0x8015F370: 0x8442008C */
    /* nop */  /* 0x8015F374: 0x00000000 */
    int cond_8015F378 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F37C: 0x00000000 */
    if (cond_8015F378) {
        psx_check_interrupts_at(cpu, 0x8015F56Cu);
        goto block_8015F56C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F380u);
    }

block_8015F380:
    a0 = (int32_t)(int16_t)cpu->read_half(s3 + 34);  /* 0x8015F380: 0x86640022 */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F388: 0x00000000 */
    rs_jal(0x8015E8A4u, 0x8015F38Cu);  /* jal display_page_commit_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F38Cu);
    goto block_8015F38C;

block_8015F38C:
    ;
    int cond_8015F38C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F390: 0x00000000 */
    if (cond_8015F38C) {
        psx_check_interrupts_at(cpu, 0x8015F380u);
        goto block_8015F380;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F394u);
    }

block_8015F394:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F398: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015F56Cu);
    goto block_8015F56C;  /* j */

block_8015F39C:
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015F39C: 0x3C068012 */
    a3 = 4096;  /* 0x8015F3A0: 0x24071000 */
    s0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F3A4: 0x3C101F80 */
    a1 = cpu->read_word(s0 + 12);  /* 0x8015F3A8: 0x8E05000C */
    s2 = a3;  /* 0x8015F3AC: 0x00E09021 */
    cpu->write_word(sp + 16, s2);  /* 0x8015F3B0: 0xAFB20010 */
    a0 = cpu->read_word(s0 + 8);  /* 0x8015F3B4: 0x8E040008 */
    a2 += 5976;  /* 0x8015F3B8: 0x24C61758 */
    cpu->write_word(sp + 20, s1);  /* 0x8015F3BC: 0xAFB10014 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015F3C4: 0x24840004 */
    rs_jal(0x8012E58Cu, 0x8015F3C8u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F3C8u);
    goto block_8015F3C8;

block_8015F3C8:
    a3 = (int32_t)(int16_t)cpu->read_half(s3 + 26);  /* 0x8015F3C8: 0x8667001A */
    /* nop */  /* 0x8015F3CC: 0x00000000 */
    int cond_8015F3D0 = (a3 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0xAC760000u;  /* 0xAC760000 */  /* 0x8015F3D4: 0x3C02AC76 */
    if (cond_8015F3D0) {
        psx_check_interrupts_at(cpu, 0x8015F3E4u);
        goto block_8015F3E4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F3D8u);
    }

block_8015F3D8:
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015F3D8: 0x3C068012 */
    /* delay slot (always executes) */
    a2 += 5988;  /* 0x8015F3E0: 0x24C61764 */
    psx_check_interrupts_at(cpu, 0x8015F434u);
    goto block_8015F434;  /* j */

block_8015F3E4:
    v0 |= 0x9185;  /* 0x8015F3E4: 0x34429185 */
    v1 = a3 << 1;  /* 0x8015F3E8: 0x00071840 */
    v1 += a3;  /* 0x8015F3EC: 0x00671821 */
    v1 <<= 3;  /* 0x8015F3F0: 0x000318C0 */
    v1 += a3;  /* 0x8015F3F4: 0x00671821 */
    v1 <<= 2;  /* 0x8015F3F8: 0x00031880 */
    { int64_t result = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v0; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v1));  /* 0x8015F3FC: 0x00620018 */
    a0 = sp + 32;  /* 0x8015F400: 0x27A40020 */
    a1 = 0x80120000u;  /* 0x80120000 */  /* 0x8015F404: 0x3C058012 */
    a2 = (int32_t)(int16_t)cpu->read_half(s3 + 22);  /* 0x8015F408: 0x86660016 */
    a1 += 5996;  /* 0x8015F40C: 0x24A5176C */
    a2++;  /* 0x8015F410: 0x24C60001 */
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8015F414: 0x00004810 */
    v0 = t1 + v1;  /* 0x8015F418: 0x01231021 */
    v0 = (int32_t)v0 >> 6;  /* 0x8015F41C: 0x00021183 */
    v1 = (int32_t)v1 >> 31;  /* 0x8015F420: 0x00031FC3 */
    v0 -= v1;  /* 0x8015F424: 0x00431023 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, v0);  /* 0x8015F42C: 0xAFA20010 */
    rs_jal(0x8018EF60u, 0x8015F430u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F430u);
    goto block_8015F430;

block_8015F430:
    a2 = sp + 32;  /* 0x8015F430: 0x27A60020 */
    /* fall through to block_8015F434 */

block_8015F434:
    a1 = cpu->read_word(s0 + 12);  /* 0x8015F434: 0x8E05000C */
    a0 = cpu->read_word(s0 + 8);  /* 0x8015F438: 0x8E040008 */
    a3 = s4;  /* 0x8015F43C: 0x02803821 */
    cpu->write_word(sp + 16, s2);  /* 0x8015F440: 0xAFB20010 */
    cpu->write_word(sp + 20, s1);  /* 0x8015F444: 0xAFB10014 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015F44C: 0x24840004 */
    rs_jal(0x8012E58Cu, 0x8015F450u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F450u);
    goto block_8015F450;

block_8015F450:
    v1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F450: 0x3C031F80 */
    v0 = cpu->read_half(v1 + 140);  /* 0x8015F454: 0x9462008C */
    /* nop */  /* 0x8015F458: 0x00000000 */
    v0 <<= 16;  /* 0x8015F45C: 0x00021400 */
    int cond_8015F460 = ((int32_t)v0 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F464: 0x00000000 */
    if (cond_8015F460) {
        psx_check_interrupts_at(cpu, 0x8015F4F0u);
        goto block_8015F4F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F468u);
    }

block_8015F468:
    v0 = (int32_t)(int16_t)cpu->read_half(s3 + 24);  /* 0x8015F468: 0x86620018 */
    /* nop */  /* 0x8015F46C: 0x00000000 */
    int cond_8015F470 = ((int32_t)v0 < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F474: 0x00000000 */
    if (cond_8015F470) {
        psx_check_interrupts_at(cpu, 0x8015F4F0u);
        goto block_8015F4F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F478u);
    }

block_8015F478:
    v0 = cpu->read_half(v1 + 128);  /* 0x8015F478: 0x94620080 */
    /* nop */  /* 0x8015F47C: 0x00000000 */
    v0 &= 0x000F;  /* 0x8015F480: 0x3042000F */
    int cond_8015F484 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x8015F488: 0x00002821 */
    if (cond_8015F484) {
        psx_check_interrupts_at(cpu, 0x8015F4C8u);
        goto block_8015F4C8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015F48Cu);
    }

block_8015F48C:
    a2 = 0xEFFE0000u;  /* 0xEFFE0000 */  /* 0x8015F48C: 0x3C06EFFE */
    a2 |= 0x0088;  /* 0x8015F490: 0x34C60088 */
    a3 = 0x7FC00000u;  /* 0x7FC00000 */  /* 0x8015F494: 0x3C077FC0 */
    v0 = cpu->read_half(v1 + 32);  /* 0x8015F498: 0x94620020 */
    a0 = cpu->read_word(v1 + 8);  /* 0x8015F49C: 0x8C640008 */
    v1 = cpu->read_half(v1 + 34);  /* 0x8015F4A0: 0x94630022 */
    v0 = (int32_t)(int16_t)v0;  /* 0x8015F4A4: 0x00021400 */  /* 0x8015F4A8: 0x00021403 */
    v1 = (int32_t)(int16_t)v1;  /* 0x8015F4AC: 0x00031C00 */  /* 0x8015F4B0: 0x00031C03 */
    cpu->write_word(sp + 16, v0);  /* 0x8015F4B4: 0xAFA20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v1);  /* 0x8015F4BC: 0xAFA30014 */
    rs_jal(0x8018C71Cu, 0x8015F4C0u);  /* jal spr_attr_alloc_b */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F4C0u);
    goto block_8015F4C0;

block_8015F4C0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015F4C4: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x8015F52Cu);
    goto block_8015F52C;  /* j */

block_8015F4C8:
    a1 = 0x00800000u;  /* 0x00800000 */  /* 0x8015F4C8: 0x3C050080 */
    a1 |= 0x0080;  /* 0x8015F4CC: 0x34A50080 */
    a2 = 0xEFFE0000u;  /* 0xEFFE0000 */  /* 0x8015F4D0: 0x3C06EFFE */
    a2 |= 0x0088;  /* 0x8015F4D4: 0x34C60088 */
    a0 = cpu->read_word(v1 + 8);  /* 0x8015F4D8: 0x8C640008 */
    a3 = 0x7FC00000u;  /* 0x7FC00000 */  /* 0x8015F4DC: 0x3C077FC0 */
    /* delay slot (always executes) */
    a0 += 8;  /* 0x8015F4E4: 0x24840008 */
    rs_jal(0x8018C824u, 0x8015F4E8u);  /* jal spr_slot_pkt_submit */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F4E8u);
    goto block_8015F4E8;

block_8015F4E8:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015F4EC: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x8015F52Cu);
    goto block_8015F52C;  /* j */

block_8015F4F0:
    v1 = 0x00800000u;  /* 0x00800000 */  /* 0x8015F4F0: 0x3C030080 */
    v1 |= 0x8080;  /* 0x8015F4F4: 0x34638080 */
    a1 = 128;  /* 0x8015F4F8: 0x24050080 */
    a2 = a1;  /* 0x8015F4FC: 0x00A03021 */
    a3 = 255;  /* 0x8015F500: 0x240700FF */
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F504: 0x3C041F80 */
    a0 = cpu->read_word(a0 + 8);  /* 0x8015F508: 0x8C840008 */
    v0 = 240;  /* 0x8015F50C: 0x240200F0 */
    cpu->write_word(sp + 16, v0);  /* 0x8015F510: 0xAFA20010 */
    v0 = 4;  /* 0x8015F514: 0x24020004 */
    cpu->write_word(sp + 20, v1);  /* 0x8015F518: 0xAFA30014 */
    cpu->write_word(sp + 24, v0);  /* 0x8015F51C: 0xAFA20018 */
    /* delay slot (always executes) */
    a0 += 8;  /* 0x8015F524: 0x24840008 */
    rs_jal(0x8018CC6Cu, 0x8015F528u);  /* jal spr_attr_alloc_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F528u);
    goto block_8015F528;

block_8015F528:
    a0 = zero;  /* 0x8015F528: 0x00002021 */
    /* fall through to block_8015F52C */

block_8015F52C:
    /* delay slot (always executes) */
    a1 = 136;  /* 0x8015F530: 0x24050088 */
    psx_check_interrupts_at(cpu, 0x8015F564u);
    goto block_8015F564;  /* j */

block_8015F534:
    a2 += 4888;  /* 0x8015F534: 0x24C61318 */
    a3 = 4096;  /* 0x8015F538: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015F53C: 0x3C021F80 */
    a1 = cpu->read_word(v0 + 12);  /* 0x8015F540: 0x8C45000C */
    a0 = cpu->read_word(v0 + 8);  /* 0x8015F544: 0x8C440008 */
    v0 = a3;  /* 0x8015F548: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015F54C: 0xAFA20010 */
    cpu->write_word(sp + 20, s1);  /* 0x8015F550: 0xAFB10014 */
    /* fall through to block_8015F554 */

block_8015F554:
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015F558: 0x24840004 */
    rs_jal(0x8012E58Cu, 0x8015F55Cu);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F55Cu);
    goto block_8015F55C;

block_8015F55C:
    a0 = zero;  /* 0x8015F55C: 0x00002021 */
    a1 = 12;  /* 0x8015F560: 0x2405000C */
    /* fall through to block_8015F564 */

block_8015F564:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015F568: 0x00000000 */
    rs_jal(0x8012CDD8u, 0x8015F56Cu);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015F56Cu);
    goto block_8015F56C;

block_8015F56C:
    ra = cpu->read_word(sp + 68);  /* 0x8015F56C: 0x8FBF0044 */
    s4 = cpu->read_word(sp + 64);  /* 0x8015F570: 0x8FB40040 */
    s3 = cpu->read_word(sp + 60);  /* 0x8015F574: 0x8FB3003C */
    s2 = cpu->read_word(sp + 56);  /* 0x8015F578: 0x8FB20038 */
    s1 = cpu->read_word(sp + 52);  /* 0x8015F57C: 0x8FB10034 */
    s0 = cpu->read_word(sp + 48);  /* 0x8015F580: 0x8FB00030 */
    v0 = zero;  /* 0x8015F584: 0x00001021 */
    uint32_t jt_8015F588 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 72;  /* 0x8015F58C: 0x27BD0048 */
    psx_check_interrupts_at(cpu, jt_8015F588);
    cpu->pc = jt_8015F588; return cpu->gpr[2];  /* CPS: jr $ra */
}