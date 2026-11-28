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

/* ======== screen_menu_stepper_cb (guest 0x80160A64) ========
 * Screen-menu stepper row callback (133 CPS blocks / 3020 bytes):
 * installed as blk+0x0c by screen_flow_state_machine; a0 = stepper desc
 * guest addr (host ptr on the fn_lookup leg -- normalised through
 * br2_gaddr), a1 = row selector. Two bounded jr-v0 dispatches on
 * s16@(desc+0x14) via guest tables 0x80121C88/0x80121CA0 (verified
 * against live guest RAM; all ten targets are intra-function blocks),
 * plus scroll offsets, HUD text, announce lines, sprite submits,
 * banner stripe scroll and the display invalidate/commit tail. */
uint32_t screen_menu_stepper_cb(intptr_t a0_, uint32_t a1_)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;
    cpu->gpr[4] = br2_gaddr((const void *)a0_);
    cpu->gpr[5] = a1_;

block_80160A64:
    sp -= 192;  /* 0x80160A64: 0x27BDFF40 */
    cpu->write_word(sp + 180, s7);  /* 0x80160A68: 0xAFB700B4 */
    s7 = a0;  /* 0x80160A6C: 0x0080B821 */
    a0 = 32;  /* 0x80160A70: 0x24040020 */
    cpu->write_word(sp + 152, s0);  /* 0x80160A74: 0xAFB00098 */
    s0 = a1;  /* 0x80160A78: 0x00A08021 */
    a1 = -1;  /* 0x80160A7C: 0x2405FFFF */
    cpu->write_word(sp + 188, ra);  /* 0x80160A80: 0xAFBF00BC */
    cpu->write_word(sp + 184, fp);  /* 0x80160A84: 0xAFBE00B8 */
    cpu->write_word(sp + 176, s6);  /* 0x80160A88: 0xAFB600B0 */
    cpu->write_word(sp + 172, s5);  /* 0x80160A8C: 0xAFB500AC */
    cpu->write_word(sp + 168, s4);  /* 0x80160A90: 0xAFB400A8 */
    cpu->write_word(sp + 164, s3);  /* 0x80160A94: 0xAFB300A4 */
    cpu->write_word(sp + 160, s2);  /* 0x80160A98: 0xAFB200A0 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 156, s1);  /* 0x80160AA0: 0xAFB1009C */
    rs_jal(0x8012CD9Cu, 0x80160AA4u);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160AA4u);
    goto block_80160AA4;

block_80160AA4:
    v0 = cpu->read_half(s7 + 8);  /* 0x80160AA4: 0x96E20008 */
    v1 = s0 & 0xFFFF;  /* 0x80160AA8: 0x3203FFFF */
    v0 += 256;  /* 0x80160AAC: 0x24420100 */
    int cond_80160AB0 = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = 1;  /* 0x80160AB4: 0x24150001 */
    if (cond_80160AB0) {
        psx_check_interrupts_at(cpu, 0x80160AE4u);
        goto block_80160AE4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160AB8u);
    }

block_80160AB8:
    s5 = zero;  /* 0x80160AB8: 0x0000A821 */
    a2 = 0x80121270u;  /* 0x80120000 4720 */  /* 0x80120000 */  /* 0x80160ABC: 0x3C068012 */  /* 0x80160AC0: 0x24C61270 */
    a3 = 4096;  /* 0x80160AC4: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80160AC8: 0x3C021F80 */
    a0 = cpu->read_word(v0 + 8);  /* 0x80160ACC: 0x8C440008 */
    a1 = cpu->read_word(v0 + 12);  /* 0x80160AD0: 0x8C45000C */
    v0 = a3;  /* 0x80160AD4: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x80160AD8: 0xAFA20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x80160AE0: 0xAFA00014 */
    psx_check_interrupts_at(cpu, 0x80160B08u);
    goto block_80160B08;  /* j */

block_80160AE4:
    a2 = 0x80121274u;  /* 0x80120000 4724 */  /* 0x80120000 */  /* 0x80160AE4: 0x3C068012 */  /* 0x80160AE8: 0x24C61274 */
    a3 = 4096;  /* 0x80160AEC: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80160AF0: 0x3C021F80 */
    a0 = cpu->read_word(v0 + 8);  /* 0x80160AF4: 0x8C440008 */
    a1 = cpu->read_word(v0 + 12);  /* 0x80160AF8: 0x8C45000C */
    v0 = a3;  /* 0x80160AFC: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x80160B00: 0xAFA20010 */
    cpu->write_word(sp + 20, s5);  /* 0x80160B04: 0xAFB50014 */
    /* fall through to block_80160B08 */

block_80160B08:
    /* delay slot (always executes) */
    /* nop */  /* 0x80160B0C: 0x00000000 */
    rs_jal(0x8012E61Cu, 0x80160B10u);  /* jal hud_text_b */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160B10u);
    goto block_80160B10;

block_80160B10:
    v1 = s0 & 0xFFFF;  /* 0x80160B10: 0x3203FFFF */
    v0 = 256;  /* 0x80160B14: 0x24020100 */
    int cond_80160B18 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 257;  /* 0x80160B1C: 0x28620101 */
    if (cond_80160B18) {
        psx_check_interrupts_at(cpu, 0x80160B54u);
        goto block_80160B54;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160B20u);
    }

block_80160B20:
    ;
    int cond_80160B20 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;  /* 0x80160B24: 0x24020001 */
    if (cond_80160B20) {
        psx_check_interrupts_at(cpu, 0x80160B38u);
        goto block_80160B38;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160B28u);
    }

block_80160B28:
    ;
    int cond_80160B28 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s7 + 20;  /* 0x80160B2C: 0x26E40014 */
    if (cond_80160B28) {
        psx_check_interrupts_at(cpu, 0x80161558u);
        goto block_80161558;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160B30u);
    }

block_80160B30:
    /* delay slot (always executes) */
    /* nop */  /* 0x80160B34: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801615FCu);
    goto block_801615FC;  /* j */

block_80160B38:
    v0 = 258;  /* 0x80160B38: 0x24020102 */
    int cond_80160B3C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < (int32_t)v0;  /* 0x80160B40: 0x0062102A */
    if (cond_80160B3C) {
        psx_check_interrupts_at(cpu, 0x801615C4u);
        goto block_801615C4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160B44u);
    }

block_80160B44:
    ;
    int cond_80160B44 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x80160B48: 0x3C068012 */
    if (cond_80160B44) {
        psx_check_interrupts_at(cpu, 0x8016156Cu);
        goto block_8016156C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160B4Cu);
    }

block_80160B4C:
    /* delay slot (always executes) */
    /* nop */  /* 0x80160B50: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801615FCu);
    goto block_801615FC;  /* j */

block_80160B54:
    a0 = -1;  /* 0x80160B54: 0x2404FFFF */
    /* delay slot (always executes) */
    a1 = 24;  /* 0x80160B5C: 0x24050018 */
    rs_jal(0x8012CD9Cu, 0x80160B60u);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160B60u);
    goto block_80160B60;

block_80160B60:
    a2 = 0x80121404u;  /* 0x80120000 5124 */  /* 0x80120000 */  /* 0x80160B60: 0x3C068012 */  /* 0x80160B64: 0x24C61404 */
    a3 = 4096;  /* 0x80160B68: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80160B6C: 0x3C021F80 */
    a0 = cpu->read_word(v0 + 8);  /* 0x80160B70: 0x8C440008 */
    a1 = cpu->read_word(v0 + 12);  /* 0x80160B74: 0x8C45000C */
    v0 = a3;  /* 0x80160B78: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x80160B7C: 0xAFA20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x80160B84: 0xAFA00014 */
    rs_jal(0x8012E58Cu, 0x80160B88u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160B88u);
    goto block_80160B88;

block_80160B88:
    v1 = (int32_t)(int16_t)cpu->read_half(s7 + 20);  /* 0x80160B88: 0x86E30014 */
    /* nop */  /* 0x80160B8C: 0x00000000 */
    v0 = v1 < (uint32_t)5;  /* 0x80160B90: 0x2C620005 */
    int cond_80160B94 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x80160B98: 0x3C028012 */
    if (cond_80160B94) {
        psx_check_interrupts_at(cpu, 0x80160BDCu);
        goto block_80160BDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160B9Cu);
    }

block_80160B9C:
    /* jr v0 via guest jump table @0x80121C88, idx = s16@(s7+0x14)
     * (guard above proves 0..4). Table verified against live guest RAM:
     * {BB8,BC0,BC8,BD0,BD8} -- all intra-function s3-set blocks that
     * rejoin at 0x80160BDC. */
    switch (v1) {
    case 0: psx_check_interrupts_at(cpu, 0x80160BB8u); goto block_80160BB8;
    case 1: psx_check_interrupts_at(cpu, 0x80160BC0u); goto block_80160BC0;
    case 2: psx_check_interrupts_at(cpu, 0x80160BC8u); goto block_80160BC8;
    case 3: psx_check_interrupts_at(cpu, 0x80160BD0u); goto block_80160BD0;
    default: psx_check_interrupts_at(cpu, 0x80160BD8u); goto block_80160BD8;
    }

block_80160BB8:
    /* delay slot (always executes) */
    s3 = 16;  /* 0x80160BBC: 0x24130010 */
    psx_check_interrupts_at(cpu, 0x80160BDCu);
    goto block_80160BDC;  /* j */

block_80160BC0:
    /* delay slot (always executes) */
    s3 = 17;  /* 0x80160BC4: 0x24130011 */
    psx_check_interrupts_at(cpu, 0x80160BDCu);
    goto block_80160BDC;  /* j */

block_80160BC8:
    /* delay slot (always executes) */
    s3 = 18;  /* 0x80160BCC: 0x24130012 */
    psx_check_interrupts_at(cpu, 0x80160BDCu);
    goto block_80160BDC;  /* j */

block_80160BD0:
    /* delay slot (always executes) */
    s3 = 8;  /* 0x80160BD4: 0x24130008 */
    psx_check_interrupts_at(cpu, 0x80160BDCu);
    goto block_80160BDC;  /* j */

block_80160BD8:
    s3 = 19;  /* 0x80160BD8: 0x24130013 */
    /* fall through to block_80160BDC */

block_80160BDC:
    a0 = s3;  /* 0x80160BDC: 0x02602021 */
    a1 = zero;  /* 0x80160BE0: 0x00002821 */
    a2 = 4;  /* 0x80160BE4: 0x24060004 */
    /* delay slot (always executes) */
    a3 = 7;  /* 0x80160BEC: 0x24070007 */
    rs_jal(0x8012BF0Cu, 0x80160BF0u);  /* jal banner_stripe_scroll */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160BF0u);
    goto block_80160BF0;

block_80160BF0:
    a0 = zero;  /* 0x80160BF0: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 24;  /* 0x80160BF8: 0x24050018 */
    rs_jal(0x8012CDD8u, 0x80160BFCu);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160BFCu);
    goto block_80160BFC;

block_80160BFC:
    v1 = (int32_t)(int16_t)cpu->read_half(s7 + 20);  /* 0x80160BFC: 0x86E30014 */
    /* nop */  /* 0x80160C00: 0x00000000 */
    v0 = v1 < (uint32_t)5;  /* 0x80160C04: 0x2C620005 */
    int cond_80160C08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x80160C0C: 0x3C028012 */
    if (cond_80160C08) {
        psx_check_interrupts_at(cpu, 0x801615FCu);
        goto block_801615FC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160C10u);
    }

block_80160C10:
    /* jr v0 via guest jump table @0x80121CA0, same idx field
     * s16@(s7+0x14) guarded 0..4. Verified against live guest RAM:
     * {C2C,C38,C44,D58,10CC} -- intra-function s4-set blocks rejoining
     * at 0x80160C4C. */
    switch (v1) {
    case 0: psx_check_interrupts_at(cpu, 0x80160C2Cu); goto block_80160C2C;
    case 1: psx_check_interrupts_at(cpu, 0x80160C38u); goto block_80160C38;
    case 2: psx_check_interrupts_at(cpu, 0x80160C44u); goto block_80160C44;
    case 3: psx_check_interrupts_at(cpu, 0x80160D58u); goto block_80160D58;
    default: psx_check_interrupts_at(cpu, 0x801610CCu); goto block_801610CC;
    }

block_80160C2C:
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80160C2C: 0x3C02801C */
    /* delay slot (always executes) */
    s4 = v0 + 4708;  /* 0x80160C34: 0x24541264 */
    psx_check_interrupts_at(cpu, 0x80160C4Cu);
    goto block_80160C4C;  /* j */

block_80160C38:
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80160C38: 0x3C02801C */
    /* delay slot (always executes) */
    s4 = v0 + 4868;  /* 0x80160C40: 0x24541304 */
    psx_check_interrupts_at(cpu, 0x80160C4Cu);
    goto block_80160C4C;  /* j */

block_80160C44:
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x80160C44: 0x3C02801C */
    s4 = v0 + 5028;  /* 0x80160C48: 0x245413A4 */
    /* fall through to block_80160C4C */

block_80160C4C:
    s6 = 32;  /* 0x80160C4C: 0x24160020 */
    a0 = s6;  /* 0x80160C50: 0x02C02021 */
    /* delay slot (always executes) */
    a1 = 48;  /* 0x80160C58: 0x24050030 */
    rs_jal(0x8012CD9Cu, 0x80160C5Cu);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160C5Cu);
    goto block_80160C5C;

block_80160C5C:
    s3 = zero;  /* 0x80160C5C: 0x00009821 */
    s0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80160C60: 0x3C101F80 */
    s5 = 4096;  /* 0x80160C64: 0x24151000 */
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80160C68: 0x3C02801B */
    s2 = v0 + 16980;  /* 0x80160C6C: 0x24524254 */
    a0 = sp + 24;  /* 0x80160C70: 0x27A40018 */
    /* fall through to block_80160C74 */

block_80160C74:
    a1 = 0x80121BF8u;  /* 0x80120000 7160 */  /* 0x80120000 */  /* 0x80160C74: 0x3C058012 */  /* 0x80160C78: 0x24A51BF8 */
    /* delay slot (always executes) */
    a2 = s3 + 1;  /* 0x80160C80: 0x26660001 */
    rs_jal(0x8018EF60u, 0x80160C84u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160C84u);
    goto block_80160C84;

block_80160C84:
    a2 = sp + 24;  /* 0x80160C84: 0x27A60018 */
    a3 = s6;  /* 0x80160C88: 0x02C03821 */
    a0 = cpu->read_word(s0 + 8);  /* 0x80160C8C: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x80160C90: 0x8E05000C */
    v0 = 3;  /* 0x80160C94: 0x24020003 */
    cpu->write_word(sp + 16, s5);  /* 0x80160C98: 0xAFB50010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);  /* 0x80160CA0: 0xAFA20014 */
    rs_jal(0x8012E58Cu, 0x80160CA4u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160CA4u);
    goto block_80160CA4;

block_80160CA4:
    a0 = cpu->read_word(s0 + 8);  /* 0x80160CA4: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x80160CA8: 0x8E05000C */
    cpu->write_word(sp + 16, s5);  /* 0x80160CAC: 0xAFB50010 */
    cpu->write_word(sp + 20, zero);  /* 0x80160CB0: 0xAFA00014 */
    a2 = cpu->read_word(s2);  /* 0x80160CB4: 0x8E460000 */
    /* delay slot (always executes) */
    a3 = 4096;  /* 0x80160CBC: 0x24071000 */
    rs_jal(0x8012E58Cu, 0x80160CC0u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160CC0u);
    goto block_80160CC0;

block_80160CC0:
    v1 = (int32_t)(int16_t)cpu->read_half(s7 + 20);  /* 0x80160CC0: 0x86E30014 */
    v0 = 1;  /* 0x80160CC4: 0x24020001 */
    int cond_80160CC8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)v1 < 2;  /* 0x80160CCC: 0x28620002 */
    if (cond_80160CC8) {
        psx_check_interrupts_at(cpu, 0x80160D00u);
        goto block_80160D00;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160CD0u);
    }

block_80160CD0:
    ;
    int cond_80160CD0 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;  /* 0x80160CD4: 0x24020002 */
    if (cond_80160CD0) {
        psx_check_interrupts_at(cpu, 0x80160CE8u);
        goto block_80160CE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160CD8u);
    }

block_80160CD8:
    ;
    int cond_80160CD8 = (v1 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = s6 + 64;  /* 0x80160CDC: 0x26C60040 */
    if (cond_80160CD8) {
        psx_check_interrupts_at(cpu, 0x80160CF8u);
        goto block_80160CF8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160CE0u);
    }

block_80160CE0:
    /* delay slot (always executes) */
    a3 = 4096;  /* 0x80160CE4: 0x24071000 */
    psx_check_interrupts_at(cpu, 0x80160D14u);
    goto block_80160D14;  /* j */

block_80160CE8:
    ;
    int cond_80160CE8 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = s6 + 64;  /* 0x80160CEC: 0x26C60040 */
    if (cond_80160CE8) {
        psx_check_interrupts_at(cpu, 0x80160D08u);
        goto block_80160D08;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160CF0u);
    }

block_80160CF0:
    /* delay slot (always executes) */
    a3 = 4096;  /* 0x80160CF4: 0x24071000 */
    psx_check_interrupts_at(cpu, 0x80160D14u);
    goto block_80160D14;  /* j */

block_80160CF8:
    /* delay slot (always executes) */
    s1 = 18;  /* 0x80160CFC: 0x24110012 */
    psx_check_interrupts_at(cpu, 0x80160D0Cu);
    goto block_80160D0C;  /* j */

block_80160D00:
    /* delay slot (always executes) */
    s1 = 22;  /* 0x80160D04: 0x24110016 */
    psx_check_interrupts_at(cpu, 0x80160D0Cu);
    goto block_80160D0C;  /* j */

block_80160D08:
    s1 = 20;  /* 0x80160D08: 0x24110014 */
    /* fall through to block_80160D0C */

block_80160D0C:
    a2 = s6 + 64;  /* 0x80160D0C: 0x26C60040 */
    a3 = 4096;  /* 0x80160D10: 0x24071000 */
    /* fall through to block_80160D14 */

block_80160D14:
    s2 += 4;  /* 0x80160D14: 0x26520004 */
    v0 = s3 << 4;  /* 0x80160D18: 0x00131100 */
    s3++;  /* 0x80160D1C: 0x26730001 */
    a0 = cpu->read_word(s0 + 8);  /* 0x80160D20: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x80160D24: 0x8E05000C */
    v0 = s4 + v0;  /* 0x80160D28: 0x02821021 */
    cpu->write_word(sp + 16, v0);  /* 0x80160D2C: 0xAFA20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);  /* 0x80160D34: 0xAFB10014 */
    rs_jal(0x80163E38u, 0x80160D38u);  /* jal hud_time_score_draw */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160D38u);
    goto block_80160D38;

block_80160D38:
    a0 = zero;  /* 0x80160D38: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 12;  /* 0x80160D40: 0x2405000C */
    rs_jal(0x8012CDD8u, 0x80160D44u);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160D44u);
    goto block_80160D44;

block_80160D44:
    v0 = (int32_t)s3 < 10;  /* 0x80160D44: 0x2A62000A */
    int cond_80160D48 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;  /* 0x80160D4C: 0x27A40018 */
    if (cond_80160D48) {
        psx_check_interrupts_at(cpu, 0x80160C74u);
        goto block_80160C74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160D50u);
    }

block_80160D50:
    /* delay slot (always executes) */
    a0 = -1;  /* 0x80160D54: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x80161544u);
    goto block_80161544;  /* j */

block_80160D58:
    s3 = 10;  /* 0x80160D58: 0x2413000A */
    v0 = sp + 128;  /* 0x80160D5C: 0x27A20080 */
    /* fall through to block_80160D60 */

block_80160D60:
    cpu->write_word(v0, s3);  /* 0x80160D60: 0xAC530000 */
    s3--;  /* 0x80160D64: 0x2673FFFF */
    int cond_80160D68 = ((int32_t)s3 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 -= 4;  /* 0x80160D6C: 0x2442FFFC */
    if (cond_80160D68) {
        psx_check_interrupts_at(cpu, 0x80160D60u);
        goto block_80160D60;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160D70u);
    }

block_80160D70:
    s3 = zero;  /* 0x80160D70: 0x00009821 */
    t1 = sp + 88;  /* 0x80160D74: 0x27A90058 */
    s1 = zero;  /* 0x80160D78: 0x00008821 */
    /* fall through to block_80160D7C */

block_80160D7C:
    t0 = s1 + 1;  /* 0x80160D7C: 0x26280001 */
    /* fall through to block_80160D80 */

block_80160D80:
    v0 = t0 << 2;  /* 0x80160D80: 0x00081080 */
    a3 = t1 + v0;  /* 0x80160D84: 0x01223821 */
    v0 = s1 << 2;  /* 0x80160D88: 0x00111080 */
    a1 = t1 + v0;  /* 0x80160D8C: 0x01222821 */
    t4 = 0x801C1444u;  /* 0x801C0000 5188 */  /* 0x801C0000 */  /* 0x80160D90: 0x3C0C801C */  /* 0x80160D94: 0x258C1444 */
    a2 = cpu->read_word(a3);  /* 0x80160D98: 0x8CE60000 */
    a0 = cpu->read_word(a1);  /* 0x80160D9C: 0x8CA40000 */
    v1 = a2 << 3;  /* 0x80160DA0: 0x000618C0 */
    v1 += a2;  /* 0x80160DA4: 0x00661821 */
    v1 <<= 2;  /* 0x80160DA8: 0x00031880 */
    v1 += t4;  /* 0x80160DAC: 0x006C1821 */
    v0 = a0 << 3;  /* 0x80160DB0: 0x000410C0 */
    v0 += a0;  /* 0x80160DB4: 0x00441021 */
    v0 <<= 2;  /* 0x80160DB8: 0x00021080 */
    t4 = 0x801C1444u;  /* 0x801C0000 5188 */  /* 0x801C0000 */  /* 0x80160DBC: 0x3C0C801C */  /* 0x80160DC0: 0x258C1444 */
    v0 += t4;  /* 0x80160DC4: 0x004C1021 */
    v1 = cpu->read_half(v1);  /* 0x80160DC8: 0x94630000 */
    v0 = cpu->read_half(v0);  /* 0x80160DCC: 0x94420000 */
    /* nop */  /* 0x80160DD0: 0x00000000 */
    v0 = v0 < v1;  /* 0x80160DD4: 0x0043102B */
    int cond_80160DD8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s1 = t0;  /* 0x80160DDC: 0x01008821 */
    if (cond_80160DD8) {
        psx_check_interrupts_at(cpu, 0x80160DE8u);
        goto block_80160DE8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160DE0u);
    }

block_80160DE0:
    cpu->write_word(a3, a0);  /* 0x80160DE0: 0xACE40000 */
    cpu->write_word(a1, a2);  /* 0x80160DE4: 0xACA60000 */
    /* fall through to block_80160DE8 */

block_80160DE8:
    v0 = (int32_t)s1 < 10;  /* 0x80160DE8: 0x2A22000A */
    int cond_80160DEC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = s1 + 1;  /* 0x80160DF0: 0x26280001 */
    if (cond_80160DEC) {
        psx_check_interrupts_at(cpu, 0x80160D80u);
        goto block_80160D80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160DF4u);
    }

block_80160DF4:
    s3++;  /* 0x80160DF4: 0x26730001 */
    v0 = (int32_t)s3 < 10;  /* 0x80160DF8: 0x2A62000A */
    int cond_80160DFC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s1 = zero;  /* 0x80160E00: 0x00008821 */
    if (cond_80160DFC) {
        psx_check_interrupts_at(cpu, 0x80160D7Cu);
        goto block_80160D7C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160E04u);
    }

block_80160E04:
    a2 = 0x80121BFCu;  /* 0x80120000 7164 */  /* 0x80120000 */  /* 0x80160E04: 0x3C068012 */  /* 0x80160E08: 0x24C61BFC */
    a3 = 104;  /* 0x80160E0C: 0x24070068 */
    s2 = zero;  /* 0x80160E10: 0x00009021 */
    s3 = s2;  /* 0x80160E14: 0x02409821 */
    s0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80160E18: 0x3C101F80 */
    a0 = cpu->read_word(s0 + 8);  /* 0x80160E1C: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x80160E20: 0x8E05000C */
    s1 = 4096;  /* 0x80160E24: 0x24111000 */
    cpu->write_word(sp + 16, s1);  /* 0x80160E28: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x80160E30: 0xAFA00014 */
    rs_jal(0x8012E7CCu, 0x80160E34u);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160E34u);
    goto block_80160E34;

block_80160E34:
    a2 = 0x80121C24u;  /* 0x80120000 7204 */  /* 0x80120000 */  /* 0x80160E34: 0x3C068012 */  /* 0x80160E38: 0x24C61C24 */
    a0 = cpu->read_word(s0 + 8);  /* 0x80160E3C: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x80160E40: 0x8E05000C */
    a3 = 96;  /* 0x80160E44: 0x24070060 */
    cpu->write_word(sp + 16, s1);  /* 0x80160E48: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x80160E50: 0xAFA00014 */
    rs_jal(0x8012E7CCu, 0x80160E54u);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160E54u);
    goto block_80160E54;

block_80160E54:
    v1 = 0x801C1444u;  /* 0x801C0000 5188 */  /* 0x801C0000 */  /* 0x80160E54: 0x3C03801C */  /* 0x80160E58: 0x24631444 */
    /* fall through to block_80160E5C */

block_80160E5C:
    v0 = cpu->read_half(v1);  /* 0x80160E5C: 0x94620000 */
    s3++;  /* 0x80160E60: 0x26730001 */
    s2 += v0;  /* 0x80160E64: 0x02429021 */
    v0 = (int32_t)s3 < 11;  /* 0x80160E68: 0x2A62000B */
    int cond_80160E6C = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 += 36;  /* 0x80160E70: 0x24630024 */
    if (cond_80160E6C) {
        psx_check_interrupts_at(cpu, 0x80160E5Cu);
        goto block_80160E5C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160E74u);
    }

block_80160E74:
    ;
    int cond_80160E74 = (s2 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s3 = zero;  /* 0x80160E78: 0x00009821 */
    if (cond_80160E74) {
        psx_check_interrupts_at(cpu, 0x80160E80u);
        goto block_80160E80;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160E7Cu);
    }

block_80160E7C:
    s2 = 1;  /* 0x80160E7C: 0x24120001 */
    /* fall through to block_80160E80 */

block_80160E80:
    s1 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80160E80: 0x3C111F80 */
    s6 = 4096;  /* 0x80160E84: 0x24161000 */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x80160E88: 0x3C028012 */
    fp = v0 + 7252;  /* 0x80160E8C: 0x245E1C54 */
    s7 = 0xCCCC0000u;  /* 0xCCCC0000 */  /* 0x80160E90: 0x3C17CCCC */
    s7 |= 0xCCCD;  /* 0x80160E94: 0x36F7CCCD */
    a0 = sp + 24;  /* 0x80160E98: 0x27A40018 */
    /* fall through to block_80160E9C */

block_80160E9C:
    v0 = s3 << 2;  /* 0x80160E9C: 0x00131080 */
    v0 = sp + v0;  /* 0x80160EA0: 0x03A21021 */
    a1 = 0x80121C4Cu;  /* 0x80120000 7244 */  /* 0x80120000 */  /* 0x80160EA4: 0x3C058012 */  /* 0x80160EA8: 0x24A51C4C */
    a3 = cpu->read_word(v0 + 88);  /* 0x80160EAC: 0x8C470058 */
    v0 = 0x801B47ECu;  /* 0x801B0000 18412 */  /* 0x801B0000 */  /* 0x80160EB0: 0x3C02801B */  /* 0x80160EB4: 0x244247EC */
    t4 = 0x801C1444u;  /* 0x801C0000 5188 */  /* 0x801C0000 */  /* 0x80160EB8: 0x3C0C801C */  /* 0x80160EBC: 0x258C1444 */
    v1 = a3 << 3;  /* 0x80160EC0: 0x000718C0 */
    v0 = v1 + v0;  /* 0x80160EC4: 0x00621021 */
    v1 += a3;  /* 0x80160EC8: 0x00671821 */
    v1 <<= 2;  /* 0x80160ECC: 0x00031880 */
    a2 = cpu->read_word(v0);  /* 0x80160ED0: 0x8C460000 */
    /* delay slot (always executes) */
    s4 = t4 + v1;  /* 0x80160ED8: 0x0183A021 */
    rs_jal(0x8018EF60u, 0x80160EDCu);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160EDCu);
    goto block_80160EDC;

block_80160EDC:
    a2 = sp + 24;  /* 0x80160EDC: 0x27A60018 */
    a0 = cpu->read_word(s1 + 8);  /* 0x80160EE0: 0x8E240008 */
    a1 = cpu->read_word(s1 + 12);  /* 0x80160EE4: 0x8E25000C */
    a3 = 16;  /* 0x80160EE8: 0x24070010 */
    cpu->write_word(sp + 16, s6);  /* 0x80160EEC: 0xAFB60010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x80160EF4: 0xAFA00014 */
    rs_jal(0x8012E7CCu, 0x80160EF8u);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160EF8u);
    goto block_80160EF8;

block_80160EF8:
    a0 = sp + 24;  /* 0x80160EF8: 0x27A40018 */
    a1 = 0x80120000u;  /* 0x80120000 */  /* 0x80160EFC: 0x3C058012 */
    v0 = cpu->read_half(s4);  /* 0x80160F00: 0x96820000 */
    a1 += 7248;  /* 0x80160F04: 0x24A51C50 */
    a2 = v0;  /* 0x80160F08: 0x00403021 */
    /* delay slot (always executes) */
    s5 = v0 < (uint32_t)1;  /* 0x80160F10: 0x2C550001 */
    rs_jal(0x8018EF60u, 0x80160F14u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160F14u);
    goto block_80160F14;

block_80160F14:
    a2 = sp + 24;  /* 0x80160F14: 0x27A60018 */
    a0 = cpu->read_word(s1 + 8);  /* 0x80160F18: 0x8E240008 */
    a1 = cpu->read_word(s1 + 12);  /* 0x80160F1C: 0x8E25000C */
    a3 = 96;  /* 0x80160F20: 0x24070060 */
    cpu->write_word(sp + 16, s6);  /* 0x80160F24: 0xAFB60010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s5);  /* 0x80160F2C: 0xAFB50014 */
    rs_jal(0x8012E85Cu, 0x80160F30u);  /* jal descblk_variant_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160F30u);
    goto block_80160F30;

block_80160F30:
    v1 = cpu->read_half(s4);  /* 0x80160F30: 0x96830000 */
    /* nop */  /* 0x80160F34: 0x00000000 */
    v0 = v1 << 5;  /* 0x80160F38: 0x00031140 */
    v0 -= v1;  /* 0x80160F3C: 0x00431023 */
    v0 <<= 2;  /* 0x80160F40: 0x00021080 */
    v0 += v1;  /* 0x80160F44: 0x00431021 */
    v0 <<= 3;  /* 0x80160F48: 0x000210C0 */
    if (s2 != 0) { cpu->lo = v0 / s2; cpu->hi = v0 % s2; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x80160F4C: 0x0052001B */
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80160F50: 0x00004012 */
    /* nop */  /* 0x80160F54: 0x00000000 */
    /* nop */  /* 0x80160F58: 0x00000000 */
    { uint64_t result = (uint64_t)t0 * (uint64_t)s7; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(t0));  /* 0x80160F5C: 0x01170019 */
    a0 = sp + 24;  /* 0x80160F60: 0x27A40018 */
    a1 = fp;  /* 0x80160F64: 0x03C02821 */
    v1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x80160F68: 0x00001810 */
    a2 = (uint32_t)v1 >> 3;  /* 0x80160F6C: 0x000330C2 */
    a3 = a2 << 2;  /* 0x80160F70: 0x00063880 */
    a3 += a2;  /* 0x80160F74: 0x00E63821 */
    a3 <<= 1;  /* 0x80160F78: 0x00073840 */
    /* delay slot (always executes) */
    a3 = t0 - a3;  /* 0x80160F80: 0x01073823 */
    rs_jal(0x8018EF60u, 0x80160F84u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160F84u);
    goto block_80160F84;

block_80160F84:
    a2 = sp + 24;  /* 0x80160F84: 0x27A60018 */
    a0 = cpu->read_word(s1 + 8);  /* 0x80160F88: 0x8E240008 */
    a1 = cpu->read_word(s1 + 12);  /* 0x80160F8C: 0x8E25000C */
    a3 = 160;  /* 0x80160F90: 0x240700A0 */
    cpu->write_word(sp + 16, s6);  /* 0x80160F94: 0xAFB60010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s5);  /* 0x80160F9C: 0xAFB50014 */
    rs_jal(0x8012E85Cu, 0x80160FA0u);  /* jal descblk_variant_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80160FA0u);
    goto block_80160FA0;

block_80160FA0:
    s0 = cpu->read_half(s4 + 6);  /* 0x80160FA0: 0x96900006 */
    /* nop */  /* 0x80160FA4: 0x00000000 */
    int cond_80160FA8 = (s0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = s0 < (uint32_t)1;  /* 0x80160FAC: 0x2E150001 */
    if (cond_80160FA8) {
        psx_check_interrupts_at(cpu, 0x80160FB4u);
        goto block_80160FB4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80160FB0u);
    }

block_80160FB0:
    s0 = 1;  /* 0x80160FB0: 0x24100001 */
    /* fall through to block_80160FB4 */

block_80160FB4:
    v1 = cpu->read_half(s4 + 4);  /* 0x80160FB4: 0x96830004 */
    /* nop */  /* 0x80160FB8: 0x00000000 */
    v0 = v1 << 5;  /* 0x80160FBC: 0x00031140 */
    v0 -= v1;  /* 0x80160FC0: 0x00431023 */
    v0 <<= 2;  /* 0x80160FC4: 0x00021080 */
    v0 += v1;  /* 0x80160FC8: 0x00431021 */
    v0 <<= 3;  /* 0x80160FCC: 0x000210C0 */
    if (s0 != 0) { cpu->lo = v0 / s0; cpu->hi = v0 % s0; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x80160FD0: 0x0050001B */
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80160FD4: 0x00004012 */
    /* nop */  /* 0x80160FD8: 0x00000000 */
    /* nop */  /* 0x80160FDC: 0x00000000 */
    { uint64_t result = (uint64_t)t0 * (uint64_t)s7; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(t0));  /* 0x80160FE0: 0x01170019 */
    a0 = sp + 24;  /* 0x80160FE4: 0x27A40018 */
    a1 = fp;  /* 0x80160FE8: 0x03C02821 */
    v1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x80160FEC: 0x00001810 */
    a2 = (uint32_t)v1 >> 3;  /* 0x80160FF0: 0x000330C2 */
    a3 = a2 << 2;  /* 0x80160FF4: 0x00063880 */
    a3 += a2;  /* 0x80160FF8: 0x00E63821 */
    a3 <<= 1;  /* 0x80160FFC: 0x00073840 */
    /* delay slot (always executes) */
    a3 = t0 - a3;  /* 0x80161004: 0x01073823 */
    rs_jal(0x8018EF60u, 0x80161008u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161008u);
    goto block_80161008;

block_80161008:
    a2 = sp + 24;  /* 0x80161008: 0x27A60018 */
    a0 = cpu->read_word(s1 + 8);  /* 0x8016100C: 0x8E240008 */
    a1 = cpu->read_word(s1 + 12);  /* 0x80161010: 0x8E25000C */
    a3 = 224;  /* 0x80161014: 0x240700E0 */
    cpu->write_word(sp + 16, s6);  /* 0x80161018: 0xAFB60010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s5);  /* 0x80161020: 0xAFB50014 */
    rs_jal(0x8012E85Cu, 0x80161024u);  /* jal descblk_variant_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161024u);
    goto block_80161024;

block_80161024:
    s0 = cpu->read_half(s4 + 10);  /* 0x80161024: 0x9690000A */
    /* nop */  /* 0x80161028: 0x00000000 */
    int cond_8016102C = (s0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = s0 < (uint32_t)1;  /* 0x80161030: 0x2E150001 */
    if (cond_8016102C) {
        psx_check_interrupts_at(cpu, 0x80161038u);
        goto block_80161038;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161034u);
    }

block_80161034:
    s0 = 1;  /* 0x80161034: 0x24100001 */
    /* fall through to block_80161038 */

block_80161038:
    v1 = cpu->read_half(s4 + 8);  /* 0x80161038: 0x96830008 */
    /* nop */  /* 0x8016103C: 0x00000000 */
    v0 = v1 << 5;  /* 0x80161040: 0x00031140 */
    v0 -= v1;  /* 0x80161044: 0x00431023 */
    v0 <<= 2;  /* 0x80161048: 0x00021080 */
    v0 += v1;  /* 0x8016104C: 0x00431021 */
    v0 <<= 3;  /* 0x80161050: 0x000210C0 */
    if (s0 != 0) { cpu->lo = v0 / s0; cpu->hi = v0 % s0; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x80161054: 0x0050001B */
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80161058: 0x00004012 */
    /* nop */  /* 0x8016105C: 0x00000000 */
    /* nop */  /* 0x80161060: 0x00000000 */
    { uint64_t result = (uint64_t)t0 * (uint64_t)s7; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(t0));  /* 0x80161064: 0x01170019 */
    a0 = sp + 24;  /* 0x80161068: 0x27A40018 */
    a1 = fp;  /* 0x8016106C: 0x03C02821 */
    s3++;  /* 0x80161070: 0x26730001 */
    v1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x80161074: 0x00001810 */
    a2 = (uint32_t)v1 >> 3;  /* 0x80161078: 0x000330C2 */
    a3 = a2 << 2;  /* 0x8016107C: 0x00063880 */
    a3 += a2;  /* 0x80161080: 0x00E63821 */
    a3 <<= 1;  /* 0x80161084: 0x00073840 */
    /* delay slot (always executes) */
    a3 = t0 - a3;  /* 0x8016108C: 0x01073823 */
    rs_jal(0x8018EF60u, 0x80161090u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161090u);
    goto block_80161090;

block_80161090:
    a2 = sp + 24;  /* 0x80161090: 0x27A60018 */
    a0 = cpu->read_word(s1 + 8);  /* 0x80161094: 0x8E240008 */
    a1 = cpu->read_word(s1 + 12);  /* 0x80161098: 0x8E25000C */
    a3 = 288;  /* 0x8016109C: 0x24070120 */
    cpu->write_word(sp + 16, s6);  /* 0x801610A0: 0xAFB60010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s5);  /* 0x801610A8: 0xAFB50014 */
    rs_jal(0x8012E85Cu, 0x801610ACu);  /* jal descblk_variant_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801610ACu);
    goto block_801610AC;

block_801610AC:
    a0 = zero;  /* 0x801610AC: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 8;  /* 0x801610B4: 0x24050008 */
    rs_jal(0x8012CDD8u, 0x801610B8u);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801610B8u);
    goto block_801610B8;

block_801610B8:
    v0 = (int32_t)s3 < 11;  /* 0x801610B8: 0x2A62000B */
    int cond_801610BC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;  /* 0x801610C0: 0x27A40018 */
    if (cond_801610BC) {
        psx_check_interrupts_at(cpu, 0x80160E9Cu);
        goto block_80160E9C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801610C4u);
    }

block_801610C4:
    /* delay slot (always executes) */
    a0 = -1;  /* 0x801610C8: 0x2404FFFF */
    psx_check_interrupts_at(cpu, 0x80161544u);
    goto block_80161544;  /* j */

block_801610CC:
    s3 = zero;  /* 0x801610CC: 0x00009821 */
    t3 = 0xCCCC0000u;  /* 0xCCCC0000 */  /* 0x801610D0: 0x3C0BCCCC */
    t3 |= 0xCCCD;  /* 0x801610D4: 0x356BCCCD */
    t1 = 0x801C1444u;  /* 0x801C0000 5188 */  /* 0x801C0000 */  /* 0x801610D8: 0x3C09801C */  /* 0x801610DC: 0x25291444 */
    /* fall through to block_801610E0 */

block_801610E0:
    s0 = zero;  /* 0x801610E0: 0x00008021 */
    s1 = s0;  /* 0x801610E4: 0x02008821 */
    t2 = s3 << 1;  /* 0x801610E8: 0x00135040 */
    a3 = 0x801C1444u;  /* 0x801C0000 5188 */  /* 0x801C0000 */  /* 0x801610EC: 0x3C07801C */  /* 0x801610F0: 0x24E71444 */
    a2 = t1;  /* 0x801610F4: 0x01203021 */
    /* fall through to block_801610F8 */

block_801610F8:
    ;
    int cond_801610F8 = (s3 == s1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = a3 + t2;  /* 0x801610FC: 0x00EA1021 */
    if (cond_801610F8) {
        psx_check_interrupts_at(cpu, 0x80161198u);
        goto block_80161198;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161100u);
    }

block_80161100:
    a0 = cpu->read_half(a2 + 14);  /* 0x80161100: 0x94C4000E */
    a1 = cpu->read_half(v0 + 14);  /* 0x80161104: 0x9445000E */
    /* nop */  /* 0x80161108: 0x00000000 */
    s2 = a0 + a1;  /* 0x8016110C: 0x00859021 */
    int cond_80161110 = (s2 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v1 = a0 << 1;  /* 0x80161114: 0x00041840 */
    if (cond_80161110) {
        psx_check_interrupts_at(cpu, 0x8016111Cu);
        goto block_8016111C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161118u);
    }

block_80161118:
    s2 = 1;  /* 0x80161118: 0x24120001 */
    /* fall through to block_8016111C */

block_8016111C:
    v1 += a0;  /* 0x8016111C: 0x00641821 */
    v1 <<= 3;  /* 0x80161120: 0x000318C0 */
    v1 += a0;  /* 0x80161124: 0x00641821 */
    v1 <<= 2;  /* 0x80161128: 0x00031880 */
    if (s2 != 0) { cpu->lo = v1 / s2; cpu->hi = v1 % s2; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v1; }
    psx_muldiv_set(cpu, 37u);  /* 0x8016112C: 0x0072001B */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80161130: 0x00001812 */
    v0 = a1 << 1;  /* 0x80161134: 0x00051040 */
    v0 += a1;  /* 0x80161138: 0x00451021 */
    v0 <<= 3;  /* 0x8016113C: 0x000210C0 */
    v0 += a1;  /* 0x80161140: 0x00451021 */
    v0 <<= 2;  /* 0x80161144: 0x00021080 */
    if (s2 != 0) { cpu->lo = v0 / s2; cpu->hi = v0 % s2; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x80161148: 0x0052001B */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8016114C: 0x00001012 */
    v1 += 5;  /* 0x80161150: 0x24630005 */
    /* nop */  /* 0x80161154: 0x00000000 */
    { uint64_t result = (uint64_t)v1 * (uint64_t)t3; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(v1));  /* 0x80161158: 0x006B0019 */
    v1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8016115C: 0x00001810 */
    v0 += 5;  /* 0x80161160: 0x24420005 */
    /* nop */  /* 0x80161164: 0x00000000 */
    { uint64_t result = (uint64_t)v0 * (uint64_t)t3; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(v0));  /* 0x80161168: 0x004B0019 */
    t0 = (uint32_t)v1 >> 3;  /* 0x8016116C: 0x000340C2 */
    t5 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x80161170: 0x00006810 */
    v1 = (uint32_t)t5 >> 3;  /* 0x80161174: 0x000D18C2 */
    v0 = t0 + v1;  /* 0x80161178: 0x01031021 */
    v0 = v0 < (uint32_t)11;  /* 0x8016117C: 0x2C42000B */
    int cond_80161180 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = t0 < v1;  /* 0x80161184: 0x0103102B */
    if (cond_80161180) {
        psx_check_interrupts_at(cpu, 0x80161194u);
        goto block_80161194;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161188u);
    }

block_80161188:
    ;
    int cond_80161188 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016118C: 0x00000000 */
    if (cond_80161188) {
        psx_check_interrupts_at(cpu, 0x80161194u);
        goto block_80161194;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161190u);
    }

block_80161190:
    t0--;  /* 0x80161190: 0x2508FFFF */
    /* fall through to block_80161194 */

block_80161194:
    s0 += t0;  /* 0x80161194: 0x02088021 */
    /* fall through to block_80161198 */

block_80161198:
    a3 += 36;  /* 0x80161198: 0x24E70024 */
    s1++;  /* 0x8016119C: 0x26310001 */
    v0 = (int32_t)s1 < 11;  /* 0x801611A0: 0x2A22000B */
    int cond_801611A4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 += 2;  /* 0x801611A8: 0x24C60002 */
    if (cond_801611A4) {
        psx_check_interrupts_at(cpu, 0x801610F8u);
        goto block_801610F8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801611ACu);
    }

block_801611AC:
    v0 = s3 << 2;  /* 0x801611AC: 0x00131080 */
    v0 = sp + v0;  /* 0x801611B0: 0x03A21021 */
    cpu->write_word(v0 + 40, s0);  /* 0x801611B4: 0xAC500028 */
    s3++;  /* 0x801611B8: 0x26730001 */
    v0 = (int32_t)s3 < 11;  /* 0x801611BC: 0x2A62000B */
    int cond_801611C0 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t1 += 36;  /* 0x801611C4: 0x25290024 */
    if (cond_801611C0) {
        psx_check_interrupts_at(cpu, 0x801610E0u);
        goto block_801610E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801611C8u);
    }

block_801611C8:
    s3 = 10;  /* 0x801611C8: 0x2413000A */
    v0 = sp + 128;  /* 0x801611CC: 0x27A20080 */
    /* fall through to block_801611D0 */

block_801611D0:
    cpu->write_word(v0, s3);  /* 0x801611D0: 0xAC530000 */
    s3--;  /* 0x801611D4: 0x2673FFFF */
    int cond_801611D8 = ((int32_t)s3 >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 -= 4;  /* 0x801611DC: 0x2442FFFC */
    if (cond_801611D8) {
        psx_check_interrupts_at(cpu, 0x801611D0u);
        goto block_801611D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801611E0u);
    }

block_801611E0:
    s3 = zero;  /* 0x801611E0: 0x00009821 */
    t2 = sp + 88;  /* 0x801611E4: 0x27AA0058 */
    t1 = sp + 40;  /* 0x801611E8: 0x27A90028 */
    s1 = zero;  /* 0x801611EC: 0x00008821 */
    /* fall through to block_801611F0 */

block_801611F0:
    t0 = s1 + 1;  /* 0x801611F0: 0x26280001 */
    /* fall through to block_801611F4 */

block_801611F4:
    v0 = t0 << 2;  /* 0x801611F4: 0x00081080 */
    a2 = t2 + v0;  /* 0x801611F8: 0x01423021 */
    v0 = s1 << 2;  /* 0x801611FC: 0x00111080 */
    a1 = t2 + v0;  /* 0x80161200: 0x01422821 */
    a3 = cpu->read_word(a2);  /* 0x80161204: 0x8CC70000 */
    a0 = cpu->read_word(a1);  /* 0x80161208: 0x8CA40000 */
    v1 = a3 << 2;  /* 0x8016120C: 0x00071880 */
    v1 = t1 + v1;  /* 0x80161210: 0x01231821 */
    v0 = a0 << 2;  /* 0x80161214: 0x00041080 */
    v0 = t1 + v0;  /* 0x80161218: 0x01221021 */
    v1 = cpu->read_word(v1);  /* 0x8016121C: 0x8C630000 */
    v0 = cpu->read_word(v0);  /* 0x80161220: 0x8C420000 */
    /* nop */  /* 0x80161224: 0x00000000 */
    v0 = (int32_t)v0 < (int32_t)v1;  /* 0x80161228: 0x0043102A */
    int cond_8016122C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s1 = t0;  /* 0x80161230: 0x01008821 */
    if (cond_8016122C) {
        psx_check_interrupts_at(cpu, 0x8016123Cu);
        goto block_8016123C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161234u);
    }

block_80161234:
    cpu->write_word(a2, a0);  /* 0x80161234: 0xACC40000 */
    cpu->write_word(a1, a3);  /* 0x80161238: 0xACA70000 */
    /* fall through to block_8016123C */

block_8016123C:
    v0 = (int32_t)s1 < 10;  /* 0x8016123C: 0x2A22000A */
    int cond_80161240 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t0 = s1 + 1;  /* 0x80161244: 0x26280001 */
    if (cond_80161240) {
        psx_check_interrupts_at(cpu, 0x801611F4u);
        goto block_801611F4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161248u);
    }

block_80161248:
    s3++;  /* 0x80161248: 0x26730001 */
    v0 = (int32_t)s3 < 10;  /* 0x8016124C: 0x2A62000A */
    int cond_80161250 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s1 = zero;  /* 0x80161254: 0x00008821 */
    if (cond_80161250) {
        psx_check_interrupts_at(cpu, 0x801611F0u);
        goto block_801611F0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161258u);
    }

block_80161258:
    s3 = zero;  /* 0x80161258: 0x00009821 */
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x8016125C: 0x3C02801B */
    s5 = v0 + 18412;  /* 0x80161260: 0x245547EC */
    s4 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80161264: 0x3C141F80 */
    s0 = 96;  /* 0x80161268: 0x24100060 */
    a0 = sp + 24;  /* 0x8016126C: 0x27A40018 */
    /* fall through to block_80161270 */

block_80161270:
    a1 = 0x80120000u;  /* 0x80120000 */  /* 0x80161270: 0x3C058012 */
    v0 = s3 << 2;  /* 0x80161274: 0x00131080 */
    s1 = sp + 88;  /* 0x80161278: 0x27B10058 */
    v0 = s1 + v0;  /* 0x8016127C: 0x02221021 */
    v0 = cpu->read_word(v0);  /* 0x80161280: 0x8C420000 */
    a1 += 7264;  /* 0x80161284: 0x24A51C60 */
    v0 <<= 3;  /* 0x80161288: 0x000210C0 */
    v0 += s5;  /* 0x8016128C: 0x00551021 */
    a2 = cpu->read_word(v0);  /* 0x80161290: 0x8C460000 */
    /* delay slot (always executes) */
    s3++;  /* 0x80161298: 0x26730001 */
    rs_jal(0x8018EF60u, 0x8016129Cu);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8016129Cu);
    goto block_8016129C;

block_8016129C:
    a2 = sp + 24;  /* 0x8016129C: 0x27A60018 */
    a3 = s0 << 16;  /* 0x801612A0: 0x00103C00 */
    a3 = (int32_t)a3 >> 16;  /* 0x801612A4: 0x00073C03 */
    s0 += 32;  /* 0x801612A8: 0x26100020 */
    a0 = cpu->read_word(s4 + 8);  /* 0x801612AC: 0x8E840008 */
    a1 = cpu->read_word(s4 + 12);  /* 0x801612B0: 0x8E85000C */
    s2 = 4096;  /* 0x801612B4: 0x24121000 */
    cpu->write_word(sp + 16, s2);  /* 0x801612B8: 0xAFB20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x801612C0: 0xAFA00014 */
    rs_jal(0x8012E7CCu, 0x801612C4u);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801612C4u);
    goto block_801612C4;

block_801612C4:
    v0 = (int32_t)s3 < 11;  /* 0x801612C4: 0x2A62000B */
    int cond_801612C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;  /* 0x801612CC: 0x27A40018 */
    if (cond_801612C8) {
        psx_check_interrupts_at(cpu, 0x80161270u);
        goto block_80161270;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801612D0u);
    }

block_801612D0:
    a2 = 0x80121C68u;  /* 0x80120000 7272 */  /* 0x80120000 */  /* 0x801612D0: 0x3C068012 */  /* 0x801612D4: 0x24C61C68 */
    a3 = 448;  /* 0x801612D8: 0x240701C0 */
    s3 = zero;  /* 0x801612DC: 0x00009821 */
    s7 = 0x1F800000u;  /* 0x1F800000 */  /* 0x801612E0: 0x3C171F80 */
    cpu->write_word(sp + 136, s1);  /* 0x801612E4: 0xAFB10088 */
    fp = s1;  /* 0x801612E8: 0x0220F021 */
    v0 = s7;  /* 0x801612EC: 0x02E01021 */
    a0 = cpu->read_word(v0 + 8);  /* 0x801612F0: 0x8C440008 */
    a1 = cpu->read_word(v0 + 12);  /* 0x801612F4: 0x8C45000C */
    t4 = 96;  /* 0x801612F8: 0x240C0060 */
    cpu->write_word(sp + 144, t4);  /* 0x801612FC: 0xAFAC0090 */
    cpu->write_word(sp + 16, s2);  /* 0x80161300: 0xAFB20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x80161308: 0xAFA00014 */
    rs_jal(0x8012E7CCu, 0x8016130Cu);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8016130Cu);
    goto block_8016130C;

block_8016130C:
    a0 = sp + 24;  /* 0x8016130C: 0x27A40018 */
    /* fall through to block_80161310 */

block_80161310:
    a1 = 0x80121C4Cu;  /* 0x80120000 7244 */  /* 0x80120000 */  /* 0x80161310: 0x3C058012 */  /* 0x80161314: 0x24A51C4C */
    s0 = zero;  /* 0x80161318: 0x00008021 */
    v0 = 0x801B47ECu;  /* 0x801B0000 18412 */  /* 0x801B0000 */  /* 0x8016131C: 0x3C02801B */  /* 0x80161320: 0x244247EC */
    t4 = 0x801C1444u;  /* 0x801C0000 5188 */  /* 0x801C0000 */  /* 0x80161324: 0x3C0C801C */  /* 0x80161328: 0x258C1444 */
    a3 = cpu->read_word(fp);  /* 0x8016132C: 0x8FC70000 */
    s6 = cpu->read_word(sp + 136);  /* 0x80161330: 0x8FB60088 */
    v1 = a3 << 3;  /* 0x80161334: 0x000718C0 */
    v0 = v1 + v0;  /* 0x80161338: 0x00621021 */
    v1 += a3;  /* 0x8016133C: 0x00671821 */
    v1 <<= 2;  /* 0x80161340: 0x00031880 */
    a2 = cpu->read_word(v0);  /* 0x80161344: 0x8C460000 */
    /* delay slot (always executes) */
    s4 = t4 + v1;  /* 0x8016134C: 0x0183A021 */
    rs_jal(0x8018EF60u, 0x80161350u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161350u);
    goto block_80161350;

block_80161350:
    a2 = sp + 24;  /* 0x80161350: 0x27A60018 */
    a3 = 16;  /* 0x80161354: 0x24070010 */
    a0 = cpu->read_word(s7 + 8);  /* 0x80161358: 0x8EE40008 */
    a1 = cpu->read_word(s7 + 12);  /* 0x8016135C: 0x8EE5000C */
    t4 = 4096;  /* 0x80161360: 0x240C1000 */
    cpu->write_word(sp + 16, t4);  /* 0x80161364: 0xAFAC0010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x8016136C: 0xAFA00014 */
    rs_jal(0x8012E7CCu, 0x80161370u);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161370u);
    goto block_80161370;

block_80161370:
    t4 = cpu->read_word(sp + 144);  /* 0x80161370: 0x8FAC0090 */
    s1 = s0;  /* 0x80161374: 0x02008821 */
    v0 = s0 + t4;  /* 0x80161378: 0x020C1021 */
    v0 += 10;  /* 0x8016137C: 0x2442000A */
    v0 <<= 16;  /* 0x80161380: 0x00021400 */
    cpu->write_word(sp + 140, v0);  /* 0x80161384: 0xAFA2008C */
    /* fall through to block_80161388 */

block_80161388:
    ;
    int cond_80161388 = (s3 != s1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    t4 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8016138C: 0x3C0C801C */
    if (cond_80161388) {
        psx_check_interrupts_at(cpu, 0x801613BCu);
        goto block_801613BC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161390u);
    }

block_80161390:
    a2 = 0x80121C74u;  /* 0x80120000 7284 */  /* 0x80120000 */  /* 0x80161390: 0x3C068012 */  /* 0x80161394: 0x24C61C74 */
    a0 = cpu->read_word(s7 + 8);  /* 0x80161398: 0x8EE40008 */
    a1 = cpu->read_word(s7 + 12);  /* 0x8016139C: 0x8EE5000C */
    t4 = cpu->read_word(sp + 140);  /* 0x801613A0: 0x8FAC008C */
    v0 = 1;  /* 0x801613A4: 0x24020001 */
    cpu->write_word(sp + 20, v0);  /* 0x801613A8: 0xAFA20014 */
    a3 = (int32_t)t4 >> 16;  /* 0x801613AC: 0x000C3C03 */
    t4 = 4096;  /* 0x801613B0: 0x240C1000 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, t4);  /* 0x801613B8: 0xAFAC0010 */
    psx_check_interrupts_at(cpu, 0x801614C8u);
    goto block_801614C8;  /* j */

block_801613BC:
    v1 = cpu->read_word(s6);  /* 0x801613BC: 0x8EC30000 */
    t4 += 5188;  /* 0x801613C0: 0x258C1444 */
    a0 = v1 << 1;  /* 0x801613C4: 0x00032040 */
    a0 = s4 + a0;  /* 0x801613C8: 0x02842021 */
    v0 = v1 << 3;  /* 0x801613CC: 0x000310C0 */
    v0 += v1;  /* 0x801613D0: 0x00431021 */
    v0 <<= 2;  /* 0x801613D4: 0x00021080 */
    v0 += t4;  /* 0x801613D8: 0x004C1021 */
    v1 = cpu->read_word(fp);  /* 0x801613DC: 0x8FC30000 */
    a0 = cpu->read_half(a0 + 14);  /* 0x801613E0: 0x9484000E */
    v1 <<= 1;  /* 0x801613E4: 0x00031840 */
    v0 += v1;  /* 0x801613E8: 0x00431021 */
    a1 = cpu->read_half(v0 + 14);  /* 0x801613EC: 0x9445000E */
    /* nop */  /* 0x801613F0: 0x00000000 */
    s2 = a0 + a1;  /* 0x801613F4: 0x00859021 */
    int cond_801613F8 = (s2 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s5 = s2 < (uint32_t)1;  /* 0x801613FC: 0x2E550001 */
    if (cond_801613F8) {
        psx_check_interrupts_at(cpu, 0x80161404u);
        goto block_80161404;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161400u);
    }

block_80161400:
    s2 = 1;  /* 0x80161400: 0x24120001 */
    /* fall through to block_80161404 */

block_80161404:
    v1 = a0 << 1;  /* 0x80161404: 0x00041840 */
    v1 += a0;  /* 0x80161408: 0x00641821 */
    v1 <<= 3;  /* 0x8016140C: 0x000318C0 */
    v1 += a0;  /* 0x80161410: 0x00641821 */
    v1 <<= 2;  /* 0x80161414: 0x00031880 */
    if (s2 != 0) { cpu->lo = v1 / s2; cpu->hi = v1 % s2; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v1; }
    psx_muldiv_set(cpu, 37u);  /* 0x80161418: 0x0072001B */
    v1 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x8016141C: 0x00001812 */
    v0 = a1 << 1;  /* 0x80161420: 0x00051040 */
    v0 += a1;  /* 0x80161424: 0x00451021 */
    v0 <<= 3;  /* 0x80161428: 0x000210C0 */
    v0 += a1;  /* 0x8016142C: 0x00451021 */
    v0 <<= 2;  /* 0x80161430: 0x00021080 */
    if (s2 != 0) { cpu->lo = v0 / s2; cpu->hi = v0 % s2; } else { cpu->lo = 0xFFFFFFFF; cpu->hi = v0; }
    psx_muldiv_set(cpu, 37u);  /* 0x80161434: 0x0052001B */
    v0 = cpu->lo;
    psx_muldiv_stall(cpu);  /* 0x80161438: 0x00001012 */
    t4 = 0xCCCC0000u;  /* 0xCCCC0000 */  /* 0x8016143C: 0x3C0CCCCC */
    t4 |= 0xCCCD;  /* 0x80161440: 0x358CCCCD */
    v1 += 5;  /* 0x80161444: 0x24630005 */
    { uint64_t result = (uint64_t)v1 * (uint64_t)t4; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(v1));  /* 0x80161448: 0x006C0019 */
    v1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8016144C: 0x00001810 */
    v0 += 5;  /* 0x80161450: 0x24420005 */
    /* nop */  /* 0x80161454: 0x00000000 */
    { uint64_t result = (uint64_t)v0 * (uint64_t)t4; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_u(v0));  /* 0x80161458: 0x004C0019 */
    t0 = (uint32_t)v1 >> 3;  /* 0x8016145C: 0x000340C2 */
    t1 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x80161460: 0x00004810 */
    v1 = (uint32_t)t1 >> 3;  /* 0x80161464: 0x000918C2 */
    v0 = t0 + v1;  /* 0x80161468: 0x01031021 */
    v0 = v0 < (uint32_t)11;  /* 0x8016146C: 0x2C42000B */
    int cond_80161470 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = t0 < v1;  /* 0x80161474: 0x0103102B */
    if (cond_80161470) {
        psx_check_interrupts_at(cpu, 0x80161484u);
        goto block_80161484;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161478u);
    }

block_80161478:
    ;
    int cond_80161478 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8016147C: 0x00000000 */
    if (cond_80161478) {
        psx_check_interrupts_at(cpu, 0x80161484u);
        goto block_80161484;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161480u);
    }

block_80161480:
    t0--;  /* 0x80161480: 0x2508FFFF */
    /* fall through to block_80161484 */

block_80161484:
    s0 += t0;  /* 0x80161484: 0x02088021 */
    a0 = sp + 24;  /* 0x80161488: 0x27A40018 */
    a1 = 0x80121C78u;  /* 0x80120000 7288 */  /* 0x80120000 */  /* 0x8016148C: 0x3C058012 */  /* 0x80161490: 0x24A51C78 */
    /* delay slot (always executes) */
    a2 = t0;  /* 0x80161498: 0x01003021 */
    rs_jal(0x8018EF60u, 0x8016149Cu);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8016149Cu);
    goto block_8016149C;

block_8016149C:
    a2 = sp + 24;  /* 0x8016149C: 0x27A60018 */
    a3 = s1 << 5;  /* 0x801614A0: 0x00113940 */
    a3 += 0;  /* 0x801614A4: 0x24E70000 */
    a3 += 100;  /* 0x801614A8: 0x24E70064 */
    a3 = (int32_t)(int16_t)a3;  /* 0x801614AC: 0x00073C00 */  /* 0x801614B0: 0x00073C03 */
    a0 = cpu->read_word(s7 + 8);  /* 0x801614B4: 0x8EE40008 */
    a1 = cpu->read_word(s7 + 12);  /* 0x801614B8: 0x8EE5000C */
    t4 = 4096;  /* 0x801614BC: 0x240C1000 */
    cpu->write_word(sp + 16, t4);  /* 0x801614C0: 0xAFAC0010 */
    cpu->write_word(sp + 20, s5);  /* 0x801614C4: 0xAFB50014 */
    /* fall through to block_801614C8 */

block_801614C8:
    /* delay slot (always executes) */
    s6 += 4;  /* 0x801614CC: 0x26D60004 */
    rs_jal(0x8012E7CCu, 0x801614D0u);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801614D0u);
    goto block_801614D0;

block_801614D0:
    s1++;  /* 0x801614D0: 0x26310001 */
    v0 = (int32_t)s1 < 11;  /* 0x801614D4: 0x2A22000B */
    int cond_801614D8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;  /* 0x801614DC: 0x27A40018 */
    if (cond_801614D8) {
        psx_check_interrupts_at(cpu, 0x80161388u);
        goto block_80161388;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x801614E0u);
    }

block_801614E0:
    a1 = 0x80121C7Cu;  /* 0x80120000 7292 */  /* 0x80120000 */  /* 0x801614E0: 0x3C058012 */  /* 0x801614E4: 0x24A51C7C */
    /* delay slot (always executes) */
    a2 = s0;  /* 0x801614EC: 0x02003021 */
    rs_jal(0x8018EF60u, 0x801614F0u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801614F0u);
    goto block_801614F0;

block_801614F0:
    a2 = sp + 24;  /* 0x801614F0: 0x27A60018 */
    a3 = 456;  /* 0x801614F4: 0x240701C8 */
    fp += 4;  /* 0x801614F8: 0x27DE0004 */
    s3++;  /* 0x801614FC: 0x26730001 */
    t4 = cpu->read_word(sp + 144);  /* 0x80161500: 0x8FAC0090 */
    v0 = 3;  /* 0x80161504: 0x24020003 */
    t4 += 32;  /* 0x80161508: 0x258C0020 */
    cpu->write_word(sp + 144, t4);  /* 0x8016150C: 0xAFAC0090 */
    a0 = cpu->read_word(s7 + 8);  /* 0x80161510: 0x8EE40008 */
    a1 = cpu->read_word(s7 + 12);  /* 0x80161514: 0x8EE5000C */
    t4 = 4096;  /* 0x80161518: 0x240C1000 */
    cpu->write_word(sp + 16, t4);  /* 0x8016151C: 0xAFAC0010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, v0);  /* 0x80161524: 0xAFA20014 */
    rs_jal(0x8012E7CCu, 0x80161528u);  /* jal hud_text_d */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161528u);
    goto block_80161528;

block_80161528:
    a0 = zero;  /* 0x80161528: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 8;  /* 0x80161530: 0x24050008 */
    rs_jal(0x8012CDD8u, 0x80161534u);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161534u);
    goto block_80161534;

block_80161534:
    v0 = (int32_t)s3 < 11;  /* 0x80161534: 0x2A62000B */
    int cond_80161538 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = sp + 24;  /* 0x8016153C: 0x27A40018 */
    if (cond_80161538) {
        psx_check_interrupts_at(cpu, 0x80161310u);
        goto block_80161310;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x80161540u);
    }

block_80161540:
    a0 = -1;  /* 0x80161540: 0x2404FFFF */
    /* fall through to block_80161544 */

block_80161544:
    t4 = 24;  /* 0x80161544: 0x240C0018 */
    /* delay slot (always executes) */
    a1 = t4 + 152;  /* 0x8016154C: 0x25850098 */
    rs_jal(0x8012CD9Cu, 0x80161550u);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161550u);
    goto block_80161550;

block_80161550:
    /* delay slot (always executes) */
    /* nop */  /* 0x80161554: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801615FCu);
    goto block_801615FC;  /* j */

block_80161558:
    a1 = 4;  /* 0x80161558: 0x24050004 */
    /* delay slot (always executes) */
    a2 = 1;  /* 0x80161560: 0x24060001 */
    rs_jal(0x80129978u, 0x80161564u);  /* jal u16_counter_step_pad_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161564u);
    goto block_80161564;

block_80161564:
    /* delay slot (always executes) */
    /* nop */  /* 0x80161568: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x801615FCu);
    goto block_801615FC;  /* j */

block_8016156C:
    a2 += 7296;  /* 0x8016156C: 0x24C61C80 */
    a3 = 4096;  /* 0x80161570: 0x24071000 */
    s0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x80161574: 0x3C101F80 */
    s2 = a3;  /* 0x80161578: 0x00E09021 */
    a0 = cpu->read_word(s0 + 8);  /* 0x8016157C: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x80161580: 0x8E05000C */
    s1 = s5;  /* 0x80161584: 0x02A08821 */
    cpu->write_word(sp + 16, s2);  /* 0x80161588: 0xAFB20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);  /* 0x80161590: 0xAFB10014 */
    rs_jal(0x8012E58Cu, 0x80161594u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x80161594u);
    goto block_80161594;

block_80161594:
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x80161594: 0x3C02801B */
    a0 = cpu->read_word(s0 + 8);  /* 0x80161598: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x8016159C: 0x8E05000C */
    v1 = (int32_t)(int16_t)cpu->read_half(s7 + 20);  /* 0x801615A0: 0x86E30014 */
    v0 += 16744;  /* 0x801615A4: 0x24424168 */
    cpu->write_word(sp + 16, s2);  /* 0x801615A8: 0xAFB20010 */
    cpu->write_word(sp + 20, s1);  /* 0x801615AC: 0xAFB10014 */
    v1 <<= 2;  /* 0x801615B0: 0x00031880 */
    v1 += v0;  /* 0x801615B4: 0x00621821 */
    a2 = cpu->read_word(v1);  /* 0x801615B8: 0x8C660000 */
    /* delay slot (always executes) */
    a3 = 192;  /* 0x801615C0: 0x240700C0 */
    psx_check_interrupts_at(cpu, 0x801615E8u);
    goto block_801615E8;  /* j */

block_801615C4:
    a2 = 0x80121318u;  /* 0x80120000 4888 */  /* 0x80120000 */  /* 0x801615C4: 0x3C068012 */  /* 0x801615C8: 0x24C61318 */
    a3 = 4096;  /* 0x801615CC: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x801615D0: 0x3C021F80 */
    a0 = cpu->read_word(v0 + 8);  /* 0x801615D4: 0x8C440008 */
    a1 = cpu->read_word(v0 + 12);  /* 0x801615D8: 0x8C45000C */
    v0 = a3;  /* 0x801615DC: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x801615E0: 0xAFA20010 */
    cpu->write_word(sp + 20, s5);  /* 0x801615E4: 0xAFB50014 */
    /* fall through to block_801615E8 */

block_801615E8:
    /* delay slot (always executes) */
    /* nop */  /* 0x801615EC: 0x00000000 */
    rs_jal(0x8012E58Cu, 0x801615F0u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801615F0u);
    goto block_801615F0;

block_801615F0:
    a0 = zero;  /* 0x801615F0: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 12;  /* 0x801615F8: 0x2405000C */
    rs_jal(0x8012CDD8u, 0x801615FCu);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x801615FCu);
    goto block_801615FC;

block_801615FC:
    ra = cpu->read_word(sp + 188);  /* 0x801615FC: 0x8FBF00BC */
    fp = cpu->read_word(sp + 184);  /* 0x80161600: 0x8FBE00B8 */
    s7 = cpu->read_word(sp + 180);  /* 0x80161604: 0x8FB700B4 */
    s6 = cpu->read_word(sp + 176);  /* 0x80161608: 0x8FB600B0 */
    s5 = cpu->read_word(sp + 172);  /* 0x8016160C: 0x8FB500AC */
    s4 = cpu->read_word(sp + 168);  /* 0x80161610: 0x8FB400A8 */
    s3 = cpu->read_word(sp + 164);  /* 0x80161614: 0x8FB300A4 */
    s2 = cpu->read_word(sp + 160);  /* 0x80161618: 0x8FB200A0 */
    s1 = cpu->read_word(sp + 156);  /* 0x8016161C: 0x8FB1009C */
    s0 = cpu->read_word(sp + 152);  /* 0x80161620: 0x8FB00098 */
    v0 = zero;  /* 0x80161624: 0x00001021 */
    uint32_t jt_80161628 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 192;  /* 0x8016162C: 0x27BD00C0 */
    psx_check_interrupts_at(cpu, jt_80161628);
    cpu->pc = jt_80161628; return cpu->gpr[2];  /* CPS: jr $ra */
}
