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


/* ======== save_menu_stepper_cb (guest 0x8015D69C) ========
 * Save-menu stepper row callback (332 CPS blocks / 4576 bytes):
 * installed as blk+0x0c by save_flow_state_machine; a0 = stepper desc
 * guest addr (host ptr on the fn_lookup leg -- normalised through
 * br2_gaddr), a1 = row selector. Largest of the three menu steppers:
 * two bounded jr-v0 dispatches on s16@(desc+0x1e) via guest tables
 * 0x80121610/0x801216A0 (36 entries each, verified against live guest
 * RAM; all targets intra-function), 36 menu_sprite_load_sm +
 * 36 cd_timer_read calls, memcard confirm/state helpers, sprite attr
 * allocs, sprintf_core_guest row text. */
uint32_t save_menu_stepper_cb(intptr_t a0_, uint32_t a1_)
{
    CPUState *cpu = clean_cpu_state();
    if (!cpu) return 0;
    cpu->gpr[4] = br2_gaddr((const void *)a0_);
    cpu->gpr[5] = a1_;



block_8015D69C:
    sp -= 96;  /* 0x8015D69C: 0x27BDFFA0 */
    cpu->write_word(sp + 76, s5);  /* 0x8015D6A0: 0xAFB5004C */
    s5 = a0;  /* 0x8015D6A4: 0x0080A821 */
    a0 = 32;  /* 0x8015D6A8: 0x24040020 */
    cpu->write_word(sp + 56, s0);  /* 0x8015D6AC: 0xAFB00038 */
    s0 = a1;  /* 0x8015D6B0: 0x00A08021 */
    a1 = -1;  /* 0x8015D6B4: 0x2405FFFF */
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8015D6B8: 0x3C02801C */
    cpu->write_word(sp + 84, s7);  /* 0x8015D6BC: 0xAFB70054 */
    s7 = v0 + 4576;  /* 0x8015D6C0: 0x245711E0 */
    v0 = 0x801C0000u;  /* 0x801C0000 */  /* 0x8015D6C4: 0x3C02801C */
    cpu->write_word(sp + 88, fp);  /* 0x8015D6C8: 0xAFBE0058 */
    fp = v0 + -4256;  /* 0x8015D6CC: 0x245EEF60 */
    cpu->write_word(sp + 92, ra);  /* 0x8015D6D0: 0xAFBF005C */
    cpu->write_word(sp + 80, s6);  /* 0x8015D6D4: 0xAFB60050 */
    cpu->write_word(sp + 72, s4);  /* 0x8015D6D8: 0xAFB40048 */
    cpu->write_word(sp + 68, s3);  /* 0x8015D6DC: 0xAFB30044 */
    cpu->write_word(sp + 64, s2);  /* 0x8015D6E0: 0xAFB20040 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 60, s1);  /* 0x8015D6E8: 0xAFB1003C */
    rs_jal(0x8012CD9Cu, 0x8015D6ECu);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015D6ECu);
    goto block_8015D6EC;

block_8015D6EC:
    v1 = (int32_t)(int16_t)cpu->read_half(s5 + 30);  /* 0x8015D6EC: 0x86A3001E */
    /* nop */  /* 0x8015D6F0: 0x00000000 */
    v0 = v1 < (uint32_t)36;  /* 0x8015D6F4: 0x2C620024 */
    int cond_8015D6F8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s6 = 312;  /* 0x8015D6FC: 0x24160138 */
    if (cond_8015D6F8) {
        psx_check_interrupts_at(cpu, 0x8015D7D0u);
        goto block_8015D7D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D700u);
    }

block_8015D700:
    /* jr v0 via guest jump table @0x80121610, idx = s16@(s5+0x1e)
     * (guard above proves 0..35). 36 entries verified against live
     * guest RAM; every target is an intra-function block. */
    switch (v1) {
    case 0: psx_check_interrupts_at(cpu, 0x8015D720u); goto block_8015D720;
    case 1: psx_check_interrupts_at(cpu, 0x8015D72Cu); goto block_8015D72C;
    case 2: psx_check_interrupts_at(cpu, 0x8015D7BCu); goto block_8015D7BC;
    case 3: psx_check_interrupts_at(cpu, 0x8015D738u); goto block_8015D738;
    case 4: psx_check_interrupts_at(cpu, 0x8015D744u); goto block_8015D744;
    case 5: psx_check_interrupts_at(cpu, 0x8015D7C8u); goto block_8015D7C8;
    case 6: psx_check_interrupts_at(cpu, 0x8015D750u); goto block_8015D750;
    case 7: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 8: psx_check_interrupts_at(cpu, 0x8015D750u); goto block_8015D750;
    case 9: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 10: psx_check_interrupts_at(cpu, 0x8015D75Cu); goto block_8015D75C;
    case 11: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 12: psx_check_interrupts_at(cpu, 0x8015D75Cu); goto block_8015D75C;
    case 13: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 14: psx_check_interrupts_at(cpu, 0x8015D768u); goto block_8015D768;
    case 15: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 16: psx_check_interrupts_at(cpu, 0x8015D780u); goto block_8015D780;
    case 17: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 18: psx_check_interrupts_at(cpu, 0x8015D75Cu); goto block_8015D75C;
    case 19: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 20: psx_check_interrupts_at(cpu, 0x8015D768u); goto block_8015D768;
    case 21: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 22: psx_check_interrupts_at(cpu, 0x8015D774u); goto block_8015D774;
    case 23: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 24: psx_check_interrupts_at(cpu, 0x8015D780u); goto block_8015D780;
    case 25: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 26: psx_check_interrupts_at(cpu, 0x8015D78Cu); goto block_8015D78C;
    case 27: psx_check_interrupts_at(cpu, 0x8015D798u); goto block_8015D798;
    case 28: psx_check_interrupts_at(cpu, 0x8015D7A4u); goto block_8015D7A4;
    case 29: psx_check_interrupts_at(cpu, 0x8015D7BCu); goto block_8015D7BC;
    case 30: psx_check_interrupts_at(cpu, 0x8015D7BCu); goto block_8015D7BC;
    case 31: psx_check_interrupts_at(cpu, 0x8015D7BCu); goto block_8015D7BC;
    case 32: psx_check_interrupts_at(cpu, 0x8015D7B0u); goto block_8015D7B0;
    case 33: psx_check_interrupts_at(cpu, 0x8015D7C8u); goto block_8015D7C8;
    case 34: psx_check_interrupts_at(cpu, 0x8015D7BCu); goto block_8015D7BC;
    default: psx_check_interrupts_at(cpu, 0x8015D7C8u); goto block_8015D7C8;
    }

block_8015D720:
    s2 = zero;  /* 0x8015D720: 0x00009021 */
    /* delay slot (always executes) */
    s1 = 17;  /* 0x8015D728: 0x24110011 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D72C:
    s2 = 64;  /* 0x8015D72C: 0x24120040 */
    /* delay slot (always executes) */
    s1 = 50;  /* 0x8015D734: 0x24110032 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D738:
    s2 = 64;  /* 0x8015D738: 0x24120040 */
    /* delay slot (always executes) */
    s1 = 2;  /* 0x8015D740: 0x24110002 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D744:
    s2 = 128;  /* 0x8015D744: 0x24120080 */
    /* delay slot (always executes) */
    s1 = 62;  /* 0x8015D74C: 0x2411003E */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D750:
    s2 = 192;  /* 0x8015D750: 0x241200C0 */
    /* delay slot (always executes) */
    s1 = 24;  /* 0x8015D758: 0x24110018 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D75C:
    s2 = 192;  /* 0x8015D75C: 0x241200C0 */
    /* delay slot (always executes) */
    s1 = 21;  /* 0x8015D764: 0x24110015 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D768:
    s2 = 192;  /* 0x8015D768: 0x241200C0 */
    /* delay slot (always executes) */
    s1 = 22;  /* 0x8015D770: 0x24110016 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D774:
    s2 = 192;  /* 0x8015D774: 0x241200C0 */
    /* delay slot (always executes) */
    s1 = 20;  /* 0x8015D77C: 0x24110014 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D780:
    s2 = 192;  /* 0x8015D780: 0x241200C0 */
    /* delay slot (always executes) */
    s1 = 23;  /* 0x8015D788: 0x24110017 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D78C:
    s2 = 192;  /* 0x8015D78C: 0x241200C0 */
    /* delay slot (always executes) */
    s1 = 17;  /* 0x8015D794: 0x24110011 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D798:
    s2 = 128;  /* 0x8015D798: 0x24120080 */
    /* delay slot (always executes) */
    s1 = zero;  /* 0x8015D7A0: 0x00008821 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D7A4:
    s2 = 128;  /* 0x8015D7A4: 0x24120080 */
    /* delay slot (always executes) */
    s1 = 3;  /* 0x8015D7AC: 0x24110003 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D7B0:
    s2 = 128;  /* 0x8015D7B0: 0x24120080 */
    /* delay slot (always executes) */
    s1 = 2;  /* 0x8015D7B8: 0x24110002 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D7BC:
    s2 = 128;  /* 0x8015D7BC: 0x24120080 */
    /* delay slot (always executes) */
    s1 = 4;  /* 0x8015D7C4: 0x24110004 */
    psx_check_interrupts_at(cpu, 0x8015D7D0u);
    goto block_8015D7D0;  /* j */

block_8015D7C8:
    s2 = 128;  /* 0x8015D7C8: 0x24120080 */
    s1 = 5;  /* 0x8015D7CC: 0x24110005 */
    /* fall through to block_8015D7D0 */

block_8015D7D0:
    v0 = cpu->read_half(s5 + 8);  /* 0x8015D7D0: 0x96A20008 */
    v1 = s0 & 0xFFFF;  /* 0x8015D7D4: 0x3203FFFF */
    v0 += 256;  /* 0x8015D7D8: 0x24420100 */
    int cond_8015D7DC = (v1 != v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    s4 = 1;  /* 0x8015D7E0: 0x24140001 */
    if (cond_8015D7DC) {
        psx_check_interrupts_at(cpu, 0x8015D810u);
        goto block_8015D810;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D7E4u);
    }

block_8015D7E4:
    s4 = zero;  /* 0x8015D7E4: 0x0000A021 */
    a2 = 0x80121270u;  /* 0x80120000 4720 */  /* 0x80120000 */  /* 0x8015D7E8: 0x3C068012 */  /* 0x8015D7EC: 0x24C61270 */
    a3 = 4096;  /* 0x8015D7F0: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015D7F4: 0x3C021F80 */
    a0 = cpu->read_word(v0 + 8);  /* 0x8015D7F8: 0x8C440008 */
    a1 = cpu->read_word(v0 + 12);  /* 0x8015D7FC: 0x8C45000C */
    v0 = a3;  /* 0x8015D800: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015D804: 0xAFA20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x8015D80C: 0xAFA00014 */
    psx_check_interrupts_at(cpu, 0x8015D834u);
    goto block_8015D834;  /* j */

block_8015D810:
    a2 = 0x80121274u;  /* 0x80120000 4724 */  /* 0x80120000 */  /* 0x8015D810: 0x3C068012 */  /* 0x8015D814: 0x24C61274 */
    a3 = 4096;  /* 0x8015D818: 0x24071000 */
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015D81C: 0x3C021F80 */
    a0 = cpu->read_word(v0 + 8);  /* 0x8015D820: 0x8C440008 */
    a1 = cpu->read_word(v0 + 12);  /* 0x8015D824: 0x8C45000C */
    v0 = a3;  /* 0x8015D828: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015D82C: 0xAFA20010 */
    cpu->write_word(sp + 20, s4);  /* 0x8015D830: 0xAFB40014 */
    /* fall through to block_8015D834 */

block_8015D834:
    /* delay slot (always executes) */
    s3 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015D838: 0x3C131F80 */
    rs_jal(0x8012E61Cu, 0x8015D83Cu);  /* jal hud_text_b */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015D83Cu);
    goto block_8015D83C;

block_8015D83C:
    s0 &= 0xFFFF;  /* 0x8015D83C: 0x3210FFFF */
    v0 = cpu->read_word(s3 + 192);  /* 0x8015D840: 0x8E6200C0 */
    v1 = 256;  /* 0x8015D844: 0x24030100 */
    a0 = cpu->read_byte(v0 + 13);  /* 0x8015D848: 0x9044000D */
    int cond_8015D84C = (s0 == v1);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s0 < 257;  /* 0x8015D850: 0x2A020101 */
    if (cond_8015D84C) {
        psx_check_interrupts_at(cpu, 0x8015D950u);
        goto block_8015D950;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D854u);
    }

block_8015D854:
    ;
    int cond_8015D854 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 5;  /* 0x8015D858: 0x24020005 */
    if (cond_8015D854) {
        psx_check_interrupts_at(cpu, 0x8015D8D0u);
        goto block_8015D8D0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D85Cu);
    }

block_8015D85C:
    ;
    int cond_8015D85C = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s0 < 6;  /* 0x8015D860: 0x2A020006 */
    if (cond_8015D85C) {
        psx_check_interrupts_at(cpu, 0x8015DCD0u);
        goto block_8015DCD0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D864u);
    }

block_8015D864:
    ;
    int cond_8015D864 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 2;  /* 0x8015D868: 0x24020002 */
    if (cond_8015D864) {
        psx_check_interrupts_at(cpu, 0x8015D8A8u);
        goto block_8015D8A8;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D86Cu);
    }

block_8015D86C:
    ;
    int cond_8015D86C = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s0 < 3;  /* 0x8015D870: 0x2A020003 */
    if (cond_8015D86C) {
        psx_check_interrupts_at(cpu, 0x8015DA34u);
        goto block_8015DA34;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D874u);
    }

block_8015D874:
    ;
    int cond_8015D874 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 1;  /* 0x8015D878: 0x24020001 */
    if (cond_8015D874) {
        psx_check_interrupts_at(cpu, 0x8015D88Cu);
        goto block_8015D88C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D87Cu);
    }

block_8015D87C:
    ;
    int cond_8015D87C = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 20;  /* 0x8015D880: 0x26A40014 */
    if (cond_8015D87C) {
        psx_check_interrupts_at(cpu, 0x8015D9A0u);
        goto block_8015D9A0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D884u);
    }

block_8015D884:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D888: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015D88C:
    v0 = 3;  /* 0x8015D88C: 0x24020003 */
    int cond_8015D890 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 4;  /* 0x8015D894: 0x24020004 */
    if (cond_8015D890) {
        psx_check_interrupts_at(cpu, 0x8015DACCu);
        goto block_8015DACC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D898u);
    }

block_8015D898:
    ;
    int cond_8015D898 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 26;  /* 0x8015D89C: 0x26A4001A */
    if (cond_8015D898) {
        psx_check_interrupts_at(cpu, 0x8015DBDCu);
        goto block_8015DBDC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8A0u);
    }

block_8015D8A0:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D8A4: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015D8A8:
    v0 = 7;  /* 0x8015D8A8: 0x24020007 */
    int cond_8015D8AC = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s0 < (int32_t)v0;  /* 0x8015D8B0: 0x0202102A */
    if (cond_8015D8AC) {
        psx_check_interrupts_at(cpu, 0x8015E66Cu);
        goto block_8015E66C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8B4u);
    }

block_8015D8B4:
    ;
    int cond_8015D8B4 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 30;  /* 0x8015D8B8: 0x26A4001E */
    if (cond_8015D8B4) {
        psx_check_interrupts_at(cpu, 0x8015DEE0u);
        goto block_8015DEE0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8BCu);
    }

block_8015D8BC:
    v0 = 8;  /* 0x8015D8BC: 0x24020008 */
    int cond_8015D8C0 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D8C4: 0x00000000 */
    if (cond_8015D8C0) {
        psx_check_interrupts_at(cpu, 0x8015E704u);
        goto block_8015E704;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8C8u);
    }

block_8015D8C8:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D8CC: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015D8D0:
    v0 = 261;  /* 0x8015D8D0: 0x24020105 */
    int cond_8015D8D4 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s0 < 262;  /* 0x8015D8D8: 0x2A020106 */
    if (cond_8015D8D4) {
        psx_check_interrupts_at(cpu, 0x8015DE08u);
        goto block_8015DE08;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8DCu);
    }

block_8015D8DC:
    ;
    int cond_8015D8DC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 258;  /* 0x8015D8E0: 0x24020102 */
    if (cond_8015D8DC) {
        psx_check_interrupts_at(cpu, 0x8015D910u);
        goto block_8015D910;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8E4u);
    }

block_8015D8E4:
    ;
    int cond_8015D8E4 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s0 < (int32_t)v0;  /* 0x8015D8E8: 0x0202102A */
    if (cond_8015D8E4) {
        psx_check_interrupts_at(cpu, 0x8015DA74u);
        goto block_8015DA74;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8ECu);
    }

block_8015D8EC:
    ;
    int cond_8015D8EC = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015D8F0: 0x3C068012 */
    if (cond_8015D8EC) {
        psx_check_interrupts_at(cpu, 0x8015D9E0u);
        goto block_8015D9E0;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D8F4u);
    }

block_8015D8F4:
    v0 = 259;  /* 0x8015D8F4: 0x24020103 */
    int cond_8015D8F8 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 260;  /* 0x8015D8FC: 0x24020104 */
    if (cond_8015D8F8) {
        psx_check_interrupts_at(cpu, 0x8015DB08u);
        goto block_8015DB08;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D900u);
    }

block_8015D900:
    ;
    int cond_8015D900 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015D904: 0x3C068012 */
    if (cond_8015D900) {
        psx_check_interrupts_at(cpu, 0x8015DBF4u);
        goto block_8015DBF4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D908u);
    }

block_8015D908:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D90C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015D910:
    v0 = 264;  /* 0x8015D910: 0x24020108 */
    int cond_8015D914 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = (int32_t)s0 < 265;  /* 0x8015D918: 0x2A020109 */
    if (cond_8015D914) {
        psx_check_interrupts_at(cpu, 0x8015E808u);
        goto block_8015E808;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D91Cu);
    }

block_8015D91C:
    ;
    int cond_8015D91C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 262;  /* 0x8015D920: 0x24020106 */
    if (cond_8015D91C) {
        psx_check_interrupts_at(cpu, 0x8015D93Cu);
        goto block_8015D93C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D924u);
    }

block_8015D924:
    ;
    int cond_8015D924 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 263;  /* 0x8015D928: 0x24020107 */
    if (cond_8015D924) {
        psx_check_interrupts_at(cpu, 0x8015E610u);
        goto block_8015E610;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D92Cu);
    }

block_8015D92C:
    ;
    int cond_8015D92C = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015D930: 0x3C068012 */
    if (cond_8015D92C) {
        psx_check_interrupts_at(cpu, 0x8015E6ACu);
        goto block_8015E6AC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D934u);
    }

block_8015D934:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D938: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015D93C:
    v0 = 265;  /* 0x8015D93C: 0x24020109 */
    int cond_8015D940 = (s0 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015D944: 0x3C068012 */
    if (cond_8015D940) {
        psx_check_interrupts_at(cpu, 0x8015E814u);
        goto block_8015E814;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D948u);
    }

block_8015D948:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D94C: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015D950:
    a0 = -1;  /* 0x8015D950: 0x2404FFFF */
    /* delay slot (always executes) */
    a1 = 24;  /* 0x8015D958: 0x24050018 */
    rs_jal(0x8012CD9Cu, 0x8015D95Cu);  /* jal scroll_offset_set */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015D95Cu);
    goto block_8015D95C;

block_8015D95C:
    a2 = 0x801213E0u;  /* 0x80120000 5088 */  /* 0x80120000 */  /* 0x8015D95C: 0x3C068012 */  /* 0x8015D960: 0x24C613E0 */
    a3 = 4096;  /* 0x8015D964: 0x24071000 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015D968: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015D96C: 0x8E65000C */
    v0 = a3;  /* 0x8015D970: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015D974: 0xAFA20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, zero);  /* 0x8015D97C: 0xAFA00014 */
    rs_jal(0x8012E58Cu, 0x8015D980u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015D980u);
    goto block_8015D980;

block_8015D980:
    a0 = 6;  /* 0x8015D980: 0x24040006 */
    a1 = zero;  /* 0x8015D984: 0x00002821 */
    a2 = 4;  /* 0x8015D988: 0x24060004 */
    /* delay slot (always executes) */
    a3 = 7;  /* 0x8015D990: 0x24070007 */
    rs_jal(0x8012BF0Cu, 0x8015D994u);  /* jal banner_stripe_scroll */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015D994u);
    goto block_8015D994;

block_8015D994:
    a0 = zero;  /* 0x8015D994: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 16;  /* 0x8015D99C: 0x24050010 */
    psx_check_interrupts_at(cpu, 0x8015E840u);
    goto block_8015E840;  /* j */

block_8015D9A0:
    a1 = 1;  /* 0x8015D9A0: 0x24050001 */
    /* delay slot (always executes) */
    a2 = a1;  /* 0x8015D9A8: 0x00A03021 */
    rs_jal(0x80129978u, 0x8015D9ACu);  /* jal u16_counter_step_pad_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015D9ACu);
    goto block_8015D9AC;

block_8015D9AC:
    ;
    int cond_8015D9AC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015D9B0: 0x00000000 */
    if (cond_8015D9AC) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015D9B4u);
    }

block_8015D9B4:
    v1 = cpu->read_word(s3 + 192);  /* 0x8015D9B4: 0x8E6300C0 */
    v0 = cpu->read_byte(s5 + 20);  /* 0x8015D9B8: 0x92A20014 */
    /* nop */  /* 0x8015D9BC: 0x00000000 */
    cpu->write_byte(v1 + 12, (uint8_t)v0);  /* 0x8015D9C0: 0xA062000C */
    v0 = cpu->read_word(s3 + 192);  /* 0x8015D9C4: 0x8E6200C0 */
    /* nop */  /* 0x8015D9C8: 0x00000000 */
    a0 = cpu->read_byte(v0 + 12);  /* 0x8015D9CC: 0x9044000C */
    /* delay slot (always executes) */
    a0 ^= 0x0001;  /* 0x8015D9D4: 0x38840001 */
    rs_jal(0x801561C8u, 0x8015D9D8u);  /* jal brightness_commit */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015D9D8u);
    goto block_8015D9D8;

block_8015D9D8:
    /* delay slot (always executes) */
    cpu->write_word(s7 + 1040, s0);  /* 0x8015D9DC: 0xAEF00410 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015D9E0:
    a2 += 5552;  /* 0x8015D9E0: 0x24C615B0 */
    a3 = 4096;  /* 0x8015D9E4: 0x24071000 */
    s1 = a3;  /* 0x8015D9E8: 0x00E08821 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015D9EC: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015D9F0: 0x8E65000C */
    s0 = s4;  /* 0x8015D9F4: 0x02808021 */
    cpu->write_word(sp + 16, s1);  /* 0x8015D9F8: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DA00: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DA04u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DA04u);
    goto block_8015DA04;

block_8015DA04:
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x8015DA04: 0x3C02801B */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DA08: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DA0C: 0x8E65000C */
    v1 = (int32_t)(int16_t)cpu->read_half(s5 + 20);  /* 0x8015DA10: 0x86A30014 */
    v0 += 16524;  /* 0x8015DA14: 0x2442408C */
    cpu->write_word(sp + 16, s1);  /* 0x8015DA18: 0xAFB10010 */
    cpu->write_word(sp + 20, s0);  /* 0x8015DA1C: 0xAFB00014 */
    v1 <<= 2;  /* 0x8015DA20: 0x00031880 */
    v1 += v0;  /* 0x8015DA24: 0x00621821 */
    a2 = cpu->read_word(v1);  /* 0x8015DA28: 0x8C660000 */
    /* delay slot (always executes) */
    a3 = s6;  /* 0x8015DA30: 0x02C03821 */
    psx_check_interrupts_at(cpu, 0x8015E830u);
    goto block_8015E830;  /* j */

block_8015DA34:
    a0 = s5 + 22;  /* 0x8015DA34: 0x26A40016 */
    a1 = 1;  /* 0x8015DA38: 0x24050001 */
    /* delay slot (always executes) */
    a2 = a1;  /* 0x8015DA40: 0x00A03021 */
    rs_jal(0x80129978u, 0x8015DA44u);  /* jal u16_counter_step_pad_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DA44u);
    goto block_8015DA44;

block_8015DA44:
    ;
    int cond_8015DA44 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DA48: 0x00000000 */
    if (cond_8015DA44) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DA4Cu);
    }

block_8015DA4C:
    v1 = cpu->read_word(s3 + 192);  /* 0x8015DA4C: 0x8E6300C0 */
    v0 = cpu->read_byte(s5 + 22);  /* 0x8015DA50: 0x92A20016 */
    /* nop */  /* 0x8015DA54: 0x00000000 */
    cpu->write_byte(v1 + 13, (uint8_t)v0);  /* 0x8015DA58: 0xA062000D */
    v0 = -1;  /* 0x8015DA5C: 0x2402FFFF */
    cpu->write_half(s5 + 38, (uint16_t)v0);  /* 0x8015DA60: 0xA6A20026 */
    v0 = 1;  /* 0x8015DA64: 0x24020001 */
    cpu->write_half(s5 + 28, (uint16_t)zero);  /* 0x8015DA68: 0xA6A0001C */
    /* delay slot (always executes) */
    cpu->write_word(s7 + 1040, v0);  /* 0x8015DA70: 0xAEE20410 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015DA74:
    a2 = 0x801215C0u;  /* 0x80120000 5568 */  /* 0x80120000 */  /* 0x8015DA74: 0x3C068012 */  /* 0x8015DA78: 0x24C615C0 */
    a3 = 4096;  /* 0x8015DA7C: 0x24071000 */
    s1 = a3;  /* 0x8015DA80: 0x00E08821 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DA84: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DA88: 0x8E65000C */
    s0 = s4;  /* 0x8015DA8C: 0x02808021 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DA90: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DA98: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DA9Cu);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DA9Cu);
    goto block_8015DA9C;

block_8015DA9C:
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x8015DA9C: 0x3C02801B */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DAA0: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DAA4: 0x8E65000C */
    v1 = (int32_t)(int16_t)cpu->read_half(s5 + 22);  /* 0x8015DAA8: 0x86A30016 */
    v0 += 16532;  /* 0x8015DAAC: 0x24424094 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DAB0: 0xAFB10010 */
    cpu->write_word(sp + 20, s0);  /* 0x8015DAB4: 0xAFB00014 */
    v1 <<= 2;  /* 0x8015DAB8: 0x00031880 */
    v1 += v0;  /* 0x8015DABC: 0x00621821 */
    a2 = cpu->read_word(v1);  /* 0x8015DAC0: 0x8C660000 */
    /* delay slot (always executes) */
    a3 = s6;  /* 0x8015DAC8: 0x02C03821 */
    psx_check_interrupts_at(cpu, 0x8015E830u);
    goto block_8015E830;  /* j */

block_8015DACC:
    a0 = s5 + 24;  /* 0x8015DACC: 0x26A40018 */
    /* delay slot (always executes) */
    a1 = 100;  /* 0x8015DAD4: 0x24050064 */
    rs_jal(0x80129B48u, 0x8015DAD8u);  /* jal u16_counter_step_pad_b */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DAD8u);
    goto block_8015DAD8;

block_8015DAD8:
    ;
    int cond_8015DAD8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DADC: 0x00000000 */
    if (cond_8015DAD8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DAE0u);
    }

block_8015DAE0:
    v1 = cpu->read_word(s3 + 192);  /* 0x8015DAE0: 0x8E6300C0 */
    v0 = cpu->read_byte(s5 + 24);  /* 0x8015DAE4: 0x92A20018 */
    /* nop */  /* 0x8015DAE8: 0x00000000 */
    cpu->write_byte(v1 + 14, (uint8_t)v0);  /* 0x8015DAEC: 0xA062000E */
    v0 = cpu->read_word(s3 + 192);  /* 0x8015DAF0: 0x8E6200C0 */
    /* nop */  /* 0x8015DAF4: 0x00000000 */
    a0 = cpu->read_byte(v0 + 14);  /* 0x8015DAF8: 0x9044000E */
    a1 = cpu->read_byte(v0 + 15);  /* 0x8015DAFC: 0x9045000F */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DB04: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E7F4u);
    goto block_8015E7F4;  /* j */

block_8015DB08:
    a2 = 0x801215CCu;  /* 0x80120000 5580 */  /* 0x80120000 */  /* 0x8015DB08: 0x3C068012 */  /* 0x8015DB0C: 0x24C615CC */
    a3 = 4096;  /* 0x8015DB10: 0x24071000 */
    s1 = a3;  /* 0x8015DB14: 0x00E08821 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DB18: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DB1C: 0x8E65000C */
    s0 = s4;  /* 0x8015DB20: 0x02808021 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DB24: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DB2C: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DB30u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DB30u);
    goto block_8015DB30;

block_8015DB30:
    a0 = sp + 40;  /* 0x8015DB30: 0x27A40028 */
    a1 = 0x80120000u;  /* 0x80120000 */  /* 0x8015DB34: 0x3C058012 */
    a2 = (int32_t)(int16_t)cpu->read_half(s5 + 24);  /* 0x8015DB38: 0x86A60018 */
    /* delay slot (always executes) */
    a1 += 4816;  /* 0x8015DB40: 0x24A512D0 */
    rs_jal(0x8018EF60u, 0x8015DB44u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DB44u);
    goto block_8015DB44;

block_8015DB44:
    a2 = sp + 40;  /* 0x8015DB44: 0x27A60028 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DB48: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DB4C: 0x8E65000C */
    a3 = s6;  /* 0x8015DB50: 0x02C03821 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DB54: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DB5C: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DB60u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DB60u);
    goto block_8015DB60;

block_8015DB60:
    /* delay slot (always executes) */
    s0 = s6 + 32;  /* 0x8015DB64: 0x26D00020 */
    rs_jal(0x8012CE20u, 0x8015DB68u);  /* jal scroll_offset_y_get */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DB68u);
    goto block_8015DB68;

block_8015DB68:
    a1 = s0;  /* 0x8015DB68: 0x02002821 */
    a2 = v0;  /* 0x8015DB6C: 0x00403021 */
    s1 = 16;  /* 0x8015DB70: 0x24110010 */
    a3 = (int32_t)(int16_t)cpu->read_half(s5 + 24);  /* 0x8015DB74: 0x86A70018 */
    v0 = 21600;  /* 0x8015DB78: 0x24025460 */
    cpu->write_word(sp + 24, v0);  /* 0x8015DB7C: 0xAFA20018 */
    v0 = 14400;  /* 0x8015DB80: 0x24023840 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DB84: 0xAFB10010 */
    cpu->write_word(sp + 20, zero);  /* 0x8015DB88: 0xAFA00014 */
    cpu->write_word(sp + 28, v0);  /* 0x8015DB8C: 0xAFA2001C */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DB90: 0x8E640008 */
    v0 = 0xE0FF;  /* 0x8015DB94: 0x3402E0FF */
    cpu->write_word(sp + 32, v0);  /* 0x8015DB98: 0xAFA20020 */
    cpu->write_word(sp + 36, zero);  /* 0x8015DB9C: 0xAFA00024 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015DBA4: 0x24840004 */
    rs_jal(0x8018CE70u, 0x8015DBA8u);  /* jal sprite_node_emit_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DBA8u);
    goto block_8015DBA8;

block_8015DBA8:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DBAC: 0x00000000 */
    rs_jal(0x8012CE20u, 0x8015DBB0u);  /* jal scroll_offset_y_get */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DBB0u);
    goto block_8015DBB0;

block_8015DBB0:
    a1 = s0;  /* 0x8015DBB0: 0x02002821 */
    a2 = v0;  /* 0x8015DBB4: 0x00403021 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DBB8: 0x8E640008 */
    a3 = 100;  /* 0x8015DBBC: 0x24070064 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DBC0: 0xAFB10010 */
    cpu->write_word(sp + 20, zero);  /* 0x8015DBC4: 0xAFA00014 */
    cpu->write_word(sp + 24, zero);  /* 0x8015DBC8: 0xAFA00018 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015DBD0: 0x24840004 */
    rs_jal(0x8018CC6Cu, 0x8015DBD4u);  /* jal spr_attr_alloc_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DBD4u);
    goto block_8015DBD4;

block_8015DBD4:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015DBD8: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x8015E83Cu);
    goto block_8015E83C;  /* j */

block_8015DBDC:
    /* delay slot (always executes) */
    a1 = 100;  /* 0x8015DBE0: 0x24050064 */
    rs_jal(0x80129B48u, 0x8015DBE4u);  /* jal u16_counter_step_pad_b */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DBE4u);
    goto block_8015DBE4;

block_8015DBE4:
    ;
    int cond_8015DBE4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DBE8: 0x00000000 */
    if (cond_8015DBE4) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DBECu);
    }

block_8015DBEC:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DBF0: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E7B8u);
    goto block_8015E7B8;  /* j */

block_8015DBF4:
    a2 += 5596;  /* 0x8015DBF4: 0x24C615DC */
    a3 = 4096;  /* 0x8015DBF8: 0x24071000 */
    s1 = a3;  /* 0x8015DBFC: 0x00E08821 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DC00: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DC04: 0x8E65000C */
    s0 = s4;  /* 0x8015DC08: 0x02808021 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DC0C: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DC14: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DC18u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DC18u);
    goto block_8015DC18;

block_8015DC18:
    a0 = sp + 40;  /* 0x8015DC18: 0x27A40028 */
    a1 = 0x80120000u;  /* 0x80120000 */  /* 0x8015DC1C: 0x3C058012 */
    a2 = (int32_t)(int16_t)cpu->read_half(s5 + 26);  /* 0x8015DC20: 0x86A6001A */
    /* delay slot (always executes) */
    a1 += 4816;  /* 0x8015DC28: 0x24A512D0 */
    rs_jal(0x8018EF60u, 0x8015DC2Cu);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DC2Cu);
    goto block_8015DC2C;

block_8015DC2C:
    a2 = sp + 40;  /* 0x8015DC2C: 0x27A60028 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DC30: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DC34: 0x8E65000C */
    a3 = s6;  /* 0x8015DC38: 0x02C03821 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DC3C: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DC44: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DC48u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DC48u);
    goto block_8015DC48;

block_8015DC48:
    /* delay slot (always executes) */
    s1 = s6 + 32;  /* 0x8015DC4C: 0x26D10020 */
    rs_jal(0x8012CE20u, 0x8015DC50u);  /* jal scroll_offset_y_get */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DC50u);
    goto block_8015DC50;

block_8015DC50:
    t1 = 0x00540000u;  /* 0x00540000 */  /* 0x8015DC50: 0x3C090054 */
    t1 |= 0x6000;  /* 0x8015DC54: 0x35296000 */
    t0 = 0x00380000u;  /* 0x00380000 */  /* 0x8015DC58: 0x3C080038 */
    t0 |= 0x4000;  /* 0x8015DC5C: 0x35084000 */
    v1 = 0x00E00000u;  /* 0x00E00000 */  /* 0x8015DC60: 0x3C0300E0 */
    v1 |= 0xFF00;  /* 0x8015DC64: 0x3463FF00 */
    a1 = s1;  /* 0x8015DC68: 0x02202821 */
    a2 = v0;  /* 0x8015DC6C: 0x00403021 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DC70: 0x8E640008 */
    a3 = (int32_t)(int16_t)cpu->read_half(s5 + 26);  /* 0x8015DC74: 0x86A7001A */
    s0 = 16;  /* 0x8015DC78: 0x24100010 */
    cpu->write_word(sp + 16, s0);  /* 0x8015DC7C: 0xAFB00010 */
    cpu->write_word(sp + 20, zero);  /* 0x8015DC80: 0xAFA00014 */
    cpu->write_word(sp + 24, t1);  /* 0x8015DC84: 0xAFA90018 */
    cpu->write_word(sp + 28, t0);  /* 0x8015DC88: 0xAFA8001C */
    cpu->write_word(sp + 32, v1);  /* 0x8015DC8C: 0xAFA30020 */
    cpu->write_word(sp + 36, zero);  /* 0x8015DC90: 0xAFA00024 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015DC98: 0x24840004 */
    rs_jal(0x8018CE70u, 0x8015DC9Cu);  /* jal sprite_node_emit_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DC9Cu);
    goto block_8015DC9C;

block_8015DC9C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DCA0: 0x00000000 */
    rs_jal(0x8012CE20u, 0x8015DCA4u);  /* jal scroll_offset_y_get */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DCA4u);
    goto block_8015DCA4;

block_8015DCA4:
    a1 = s1;  /* 0x8015DCA4: 0x02202821 */
    a2 = v0;  /* 0x8015DCA8: 0x00403021 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DCAC: 0x8E640008 */
    a3 = 100;  /* 0x8015DCB0: 0x24070064 */
    cpu->write_word(sp + 16, s0);  /* 0x8015DCB4: 0xAFB00010 */
    cpu->write_word(sp + 20, zero);  /* 0x8015DCB8: 0xAFA00014 */
    cpu->write_word(sp + 24, zero);  /* 0x8015DCBC: 0xAFA00018 */
    /* delay slot (always executes) */
    a0 += 4;  /* 0x8015DCC4: 0x24840004 */
    rs_jal(0x8018CC6Cu, 0x8015DCC8u);  /* jal spr_attr_alloc_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DCC8u);
    goto block_8015DCC8;

block_8015DCC8:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015DCCC: 0x00002021 */
    psx_check_interrupts_at(cpu, 0x8015E83Cu);
    goto block_8015E83C;  /* j */

block_8015DCD0:
    v0 = a0 & 0x00FF;  /* 0x8015DCD0: 0x308200FF */
    int cond_8015DCD4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = s5 + 28;  /* 0x8015DCD8: 0x26A4001C */
    if (cond_8015DCD4) {
        psx_check_interrupts_at(cpu, 0x8015DD4Cu);
        goto block_8015DD4C;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DCDCu);
    }

block_8015DCDC:
    a1 = 18;  /* 0x8015DCDC: 0x24050012 */
    /* delay slot (always executes) */
    a2 = 1;  /* 0x8015DCE4: 0x24060001 */
    rs_jal(0x80129978u, 0x8015DCE8u);  /* jal u16_counter_step_pad_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DCE8u);
    goto block_8015DCE8;

block_8015DCE8:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015DCE8: 0x3C021F80 */
    v0 = cpu->read_half(v0 + 124);  /* 0x8015DCEC: 0x9442007C */
    /* nop */  /* 0x8015DCF0: 0x00000000 */
    v0 &= 0x00E0;  /* 0x8015DCF4: 0x304200E0 */
    int cond_8015DCF8 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DCFC: 0x00000000 */
    if (cond_8015DCF8) {
        psx_check_interrupts_at(cpu, 0x8015DDC4u);
        goto block_8015DDC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DD00u);
    }

block_8015DD00:
    v0 = cpu->read_byte(fp + 13);  /* 0x8015DD00: 0x93C2000D */
    /* nop */  /* 0x8015DD04: 0x00000000 */
    int cond_8015DD08 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DD0C: 0x00000000 */
    if (cond_8015DD08) {
        psx_check_interrupts_at(cpu, 0x8015DD24u);
        goto block_8015DD24;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DD10u);
    }

block_8015DD10:
    v1 = (int32_t)(int16_t)cpu->read_half(s5 + 28);  /* 0x8015DD10: 0x86A3001C */
    v0 = (int32_t)(int16_t)cpu->read_half(s5 + 38);  /* 0x8015DD14: 0x86A20026 */
    /* nop */  /* 0x8015DD18: 0x00000000 */
    int cond_8015DD1C = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DD20: 0x00000000 */
    if (cond_8015DD1C) {
        psx_check_interrupts_at(cpu, 0x8015DDBCu);
        goto block_8015DDBC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DD24u);
    }

block_8015DD24:
    v0 = cpu->read_half(s5 + 28);  /* 0x8015DD24: 0x96A2001C */
    /* delay slot (always executes) */
    cpu->write_half(s5 + 38, (uint16_t)v0);  /* 0x8015DD2C: 0xA6A20026 */
    rs_jal(0x801573ECu, 0x8015DD30u);  /* jal mc_state_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DD30u);
    goto block_8015DD30;

block_8015DD30:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DD34: 0x00000000 */
    rs_jal(0x801582C0u, 0x8015DD38u);  /* jal chan_reset_register */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DD38u);
    goto block_8015DD38;

block_8015DD38:
    a0 = (int32_t)(int16_t)cpu->read_half(s5 + 28);  /* 0x8015DD38: 0x86A4001C */
    /* delay slot (always executes) */
    a1 = zero;  /* 0x8015DD40: 0x00002821 */
    rs_jal(0x801577BCu, 0x8015DD44u);  /* jal mc_confirm_dispatch */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DD44u);
    goto block_8015DD44;

block_8015DD44:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DD48: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015DDC4u);
    goto block_8015DDC4;  /* j */

block_8015DD4C:
    a1 = 23;  /* 0x8015DD4C: 0x24050017 */
    /* delay slot (always executes) */
    a2 = 1;  /* 0x8015DD54: 0x24060001 */
    rs_jal(0x80129978u, 0x8015DD58u);  /* jal u16_counter_step_pad_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DD58u);
    goto block_8015DD58;

block_8015DD58:
    v0 = cpu->read_half(s3 + 124);  /* 0x8015DD58: 0x9662007C */
    /* nop */  /* 0x8015DD5C: 0x00000000 */
    v0 &= 0x00E0;  /* 0x8015DD60: 0x304200E0 */
    int cond_8015DD64 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DD68: 0x00000000 */
    if (cond_8015DD64) {
        psx_check_interrupts_at(cpu, 0x8015DDC4u);
        goto block_8015DDC4;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DD6Cu);
    }

block_8015DD6C:
    v0 = cpu->read_byte(fp + 13);  /* 0x8015DD6C: 0x93C2000D */
    /* nop */  /* 0x8015DD70: 0x00000000 */
    int cond_8015DD74 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DD78: 0x00000000 */
    if (cond_8015DD74) {
        psx_check_interrupts_at(cpu, 0x8015DD90u);
        goto block_8015DD90;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DD7Cu);
    }

block_8015DD7C:
    v1 = (int32_t)(int16_t)cpu->read_half(s5 + 28);  /* 0x8015DD7C: 0x86A3001C */
    v0 = (int32_t)(int16_t)cpu->read_half(s5 + 38);  /* 0x8015DD80: 0x86A20026 */
    /* nop */  /* 0x8015DD84: 0x00000000 */
    int cond_8015DD88 = (v1 == v0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DD8C: 0x00000000 */
    if (cond_8015DD88) {
        psx_check_interrupts_at(cpu, 0x8015DDBCu);
        goto block_8015DDBC;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DD90u);
    }

block_8015DD90:
    v0 = cpu->read_half(s5 + 28);  /* 0x8015DD90: 0x96A2001C */
    /* delay slot (always executes) */
    cpu->write_half(s5 + 38, (uint16_t)v0);  /* 0x8015DD98: 0xA6A20026 */
    rs_jal(0x801573ECu, 0x8015DD9Cu);  /* jal mc_state_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DD9Cu);
    goto block_8015DD9C;

block_8015DD9C:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DDA0: 0x00000000 */
    rs_jal(0x801582C0u, 0x8015DDA4u);  /* jal chan_reset_register */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DDA4u);
    goto block_8015DDA4;

block_8015DDA4:
    a0 = (int32_t)(int16_t)cpu->read_half(s5 + 28);  /* 0x8015DDA4: 0x86A4001C */
    a1 = zero;  /* 0x8015DDA8: 0x00002821 */
    /* delay slot (always executes) */
    a0 += 19;  /* 0x8015DDB0: 0x24840013 */
    rs_jal(0x801577BCu, 0x8015DDB4u);  /* jal mc_confirm_dispatch */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DDB4u);
    goto block_8015DDB4;

block_8015DDB4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DDB8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015DDC4u);
    goto block_8015DDC4;  /* j */

block_8015DDBC:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DDC0: 0x00000000 */
    rs_jal(0x801573ECu, 0x8015DDC4u);  /* jal mc_state_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DDC4u);
    goto block_8015DDC4;

block_8015DDC4:
    v0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015DDC4: 0x3C021F80 */
    v0 = cpu->read_half(v0 + 124);  /* 0x8015DDC8: 0x9442007C */
    /* nop */  /* 0x8015DDCC: 0x00000000 */
    v0 &= 0x000A;  /* 0x8015DDD0: 0x3042000A */
    int cond_8015DDD4 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DDD8: 0x00000000 */
    if (cond_8015DDD4) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DDDCu);
    }

block_8015DDDC:
    v0 = cpu->read_half(s5 + 34);  /* 0x8015DDDC: 0x96A20022 */
    /* nop */  /* 0x8015DDE0: 0x00000000 */
    v0++;  /* 0x8015DDE4: 0x24420001 */
    cpu->write_half(s5 + 34, (uint16_t)v0);  /* 0x8015DDE8: 0xA6A20022 */
    v0 = (int32_t)(int16_t)v0;  /* 0x8015DDEC: 0x00021400 */  /* 0x8015DDF0: 0x00021403 */
    v0 = (int32_t)v0 < 5;  /* 0x8015DDF4: 0x28420005 */
    int cond_8015DDF8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DDFC: 0x00000000 */
    if (cond_8015DDF8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DE00u);
    }

block_8015DE00:
    /* delay slot (always executes) */
    cpu->write_half(s5 + 34, (uint16_t)zero);  /* 0x8015DE04: 0xA6A00022 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015DE08:
    a2 = 0x801215E8u;  /* 0x80120000 5608 */  /* 0x80120000 */  /* 0x8015DE08: 0x3C068012 */  /* 0x8015DE0C: 0x24C615E8 */
    a3 = 4096;  /* 0x8015DE10: 0x24071000 */
    s1 = a3;  /* 0x8015DE14: 0x00E08821 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DE18: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DE1C: 0x8E65000C */
    s0 = s4;  /* 0x8015DE20: 0x02808021 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DE24: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DE2C: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DE30u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DE30u);
    goto block_8015DE30;

block_8015DE30:
    a0 = sp + 40;  /* 0x8015DE30: 0x27A40028 */
    a1 = 0x80120000u;  /* 0x80120000 */  /* 0x8015DE34: 0x3C058012 */
    a2 = (int32_t)(int16_t)cpu->read_half(s5 + 28);  /* 0x8015DE38: 0x86A6001C */
    a1 += 5620;  /* 0x8015DE3C: 0x24A515F4 */
    /* delay slot (always executes) */
    a2++;  /* 0x8015DE44: 0x24C60001 */
    rs_jal(0x8018EF60u, 0x8015DE48u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DE48u);
    goto block_8015DE48;

block_8015DE48:
    a2 = sp + 40;  /* 0x8015DE48: 0x27A60028 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DE4C: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DE50: 0x8E65000C */
    a3 = s6;  /* 0x8015DE54: 0x02C03821 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DE58: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DE60: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015DE64u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DE64u);
    goto block_8015DE64;

block_8015DE64:
    a0 = zero;  /* 0x8015DE64: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 12;  /* 0x8015DE6C: 0x2405000C */
    rs_jal(0x8012CDD8u, 0x8015DE70u);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DE70u);
    goto block_8015DE70;

block_8015DE70:
    /* delay slot (always executes) */
    a0 = sp + 40;  /* 0x8015DE74: 0x27A40028 */
    rs_jal(0x80158DA8u, 0x8015DE78u);  /* jal bcd_time_frame_print */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DE78u);
    goto block_8015DE78;

block_8015DE78:
    a2 = sp + 40;  /* 0x8015DE78: 0x27A60028 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DE7C: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DE80: 0x8E65000C */
    a3 = s6;  /* 0x8015DE84: 0x02C03821 */
    cpu->write_word(sp + 16, s1);  /* 0x8015DE88: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015DE90: 0xAFB00014 */
    rs_jal(0x8012E61Cu, 0x8015DE94u);  /* jal hud_text_b */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DE94u);
    goto block_8015DE94;

block_8015DE94:
    a0 = zero;  /* 0x8015DE94: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 13;  /* 0x8015DE9C: 0x2405000D */
    rs_jal(0x8012CDD8u, 0x8015DEA0u);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DEA0u);
    goto block_8015DEA0;

block_8015DEA0:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DEA4: 0x00000000 */
    rs_jal(0x8012CE20u, 0x8015DEA8u);  /* jal scroll_offset_y_get */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DEA8u);
    goto block_8015DEA8;

block_8015DEA8:
    a2 = s6;  /* 0x8015DEA8: 0x02C03021 */
    a3 = v0 + -3;  /* 0x8015DEAC: 0x2447FFFD */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015DEB0: 0x8E640008 */
    v0 = cpu->read_half(s5 + 34);  /* 0x8015DEB4: 0x96A20022 */
    v1 = cpu->read_half(s5 + 36);  /* 0x8015DEB8: 0x96A30024 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015DEBC: 0x8E65000C */
    v0 |= v1;  /* 0x8015DEC0: 0x00431025 */
    v0 = (int32_t)(int16_t)v0;  /* 0x8015DEC4: 0x00021400 */  /* 0x8015DEC8: 0x00021403 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, v0);  /* 0x8015DED0: 0xAFA20010 */
    rs_jal(0x801587C0u, 0x8015DED4u);  /* jal hud_gauge_frame_emit */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DED4u);
    goto block_8015DED4;

block_8015DED4:
    a0 = zero;  /* 0x8015DED4: 0x00002021 */
    /* delay slot (always executes) */
    a1 = 8;  /* 0x8015DEDC: 0x24050008 */
    psx_check_interrupts_at(cpu, 0x8015E840u);
    goto block_8015E840;  /* j */

block_8015DEE0:
    a1 = 35;  /* 0x8015DEE0: 0x24050023 */
    /* delay slot (always executes) */
    a2 = zero;  /* 0x8015DEE8: 0x00003021 */
    rs_jal(0x80129978u, 0x8015DEECu);  /* jal u16_counter_step_pad_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DEECu);
    goto block_8015DEEC;

block_8015DEEC:
    ;
    int cond_8015DEEC = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DEF0: 0x00000000 */
    if (cond_8015DEEC) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DEF4u);
    }

block_8015DEF4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DEF8: 0x00000000 */
    rs_jal(0x80156220u, 0x8015DEFCu);  /* jal rec_slot_scan */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DEFCu);
    goto block_8015DEFC;

block_8015DEFC:
    a0 = 3;  /* 0x8015DEFC: 0x24040003 */
    /* delay slot (always executes) */
    a1 = 24;  /* 0x8015DF04: 0x24050018 */
    rs_jal(0x80156344u, 0x8015DF08u);  /* jal rec_clear_irq */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DF08u);
    goto block_8015DF08;

block_8015DF08:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DF0C: 0x00000000 */
    rs_jal(0x80158358u, 0x8015DF10u);  /* jal chan_reset_clear */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DF10u);
    goto block_8015DF10;

block_8015DF10:
    /* delay slot (always executes) */
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015DF14: 0x3C041F80 */
    rs_jal(0x8012B23Cu, 0x8015DF18u);  /* jal frame_present */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DF18u);
    goto block_8015DF18;

block_8015DF18:
    cpu->write_half(s5 + 32, (uint16_t)zero);  /* 0x8015DF18: 0xA6A00020 */
    cpu->write_half(s3 + 140, (uint16_t)zero);  /* 0x8015DF1C: 0xA660008C */
    v1 = (int32_t)(int16_t)cpu->read_half(s5 + 30);  /* 0x8015DF20: 0x86A3001E */
    /* nop */  /* 0x8015DF24: 0x00000000 */
    v0 = v1 < (uint32_t)36;  /* 0x8015DF28: 0x2C620024 */
    int cond_8015DF2C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    v0 = 0x80120000u;  /* 0x80120000 */  /* 0x8015DF30: 0x3C028012 */
    if (cond_8015DF2C) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DF34u);
    }

block_8015DF34:
    /* jr v0 via guest jump table @0x801216A0, same idx field
     * s16@(s5+0x1e) guarded 0..35. 36 entries verified against live
     * guest RAM; every target is an intra-function block (the 36
     * per-row paint arms, stride 0x30). */
    switch (v1) {
    case 0: psx_check_interrupts_at(cpu, 0x8015DF50u); goto block_8015DF50;
    case 1: psx_check_interrupts_at(cpu, 0x8015DF80u); goto block_8015DF80;
    case 2: psx_check_interrupts_at(cpu, 0x8015DFB0u); goto block_8015DFB0;
    case 3: psx_check_interrupts_at(cpu, 0x8015DFE0u); goto block_8015DFE0;
    case 4: psx_check_interrupts_at(cpu, 0x8015E010u); goto block_8015E010;
    case 5: psx_check_interrupts_at(cpu, 0x8015E040u); goto block_8015E040;
    case 6: psx_check_interrupts_at(cpu, 0x8015E070u); goto block_8015E070;
    case 7: psx_check_interrupts_at(cpu, 0x8015E0A0u); goto block_8015E0A0;
    case 8: psx_check_interrupts_at(cpu, 0x8015E0D0u); goto block_8015E0D0;
    case 9: psx_check_interrupts_at(cpu, 0x8015E100u); goto block_8015E100;
    case 10: psx_check_interrupts_at(cpu, 0x8015E130u); goto block_8015E130;
    case 11: psx_check_interrupts_at(cpu, 0x8015E160u); goto block_8015E160;
    case 12: psx_check_interrupts_at(cpu, 0x8015E190u); goto block_8015E190;
    case 13: psx_check_interrupts_at(cpu, 0x8015E1C0u); goto block_8015E1C0;
    case 14: psx_check_interrupts_at(cpu, 0x8015E1F0u); goto block_8015E1F0;
    case 15: psx_check_interrupts_at(cpu, 0x8015E220u); goto block_8015E220;
    case 16: psx_check_interrupts_at(cpu, 0x8015E250u); goto block_8015E250;
    case 17: psx_check_interrupts_at(cpu, 0x8015E280u); goto block_8015E280;
    case 18: psx_check_interrupts_at(cpu, 0x8015E2B0u); goto block_8015E2B0;
    case 19: psx_check_interrupts_at(cpu, 0x8015E2E0u); goto block_8015E2E0;
    case 20: psx_check_interrupts_at(cpu, 0x8015E310u); goto block_8015E310;
    case 21: psx_check_interrupts_at(cpu, 0x8015E340u); goto block_8015E340;
    case 22: psx_check_interrupts_at(cpu, 0x8015E370u); goto block_8015E370;
    case 23: psx_check_interrupts_at(cpu, 0x8015E3A0u); goto block_8015E3A0;
    case 24: psx_check_interrupts_at(cpu, 0x8015E3D0u); goto block_8015E3D0;
    case 25: psx_check_interrupts_at(cpu, 0x8015E400u); goto block_8015E400;
    case 26: psx_check_interrupts_at(cpu, 0x8015E430u); goto block_8015E430;
    case 27: psx_check_interrupts_at(cpu, 0x8015E460u); goto block_8015E460;
    case 28: psx_check_interrupts_at(cpu, 0x8015E490u); goto block_8015E490;
    case 29: psx_check_interrupts_at(cpu, 0x8015E4C0u); goto block_8015E4C0;
    case 30: psx_check_interrupts_at(cpu, 0x8015E4F0u); goto block_8015E4F0;
    case 31: psx_check_interrupts_at(cpu, 0x8015E520u); goto block_8015E520;
    case 32: psx_check_interrupts_at(cpu, 0x8015E550u); goto block_8015E550;
    case 33: psx_check_interrupts_at(cpu, 0x8015E580u); goto block_8015E580;
    case 34: psx_check_interrupts_at(cpu, 0x8015E5B0u); goto block_8015E5B0;
    default: psx_check_interrupts_at(cpu, 0x8015E5E0u); goto block_8015E5E0;
    }

block_8015DF50:
    cpu->write_word(sp + 16, zero);  /* 0x8015DF50: 0xAFA00010 */
    /* fall through to block_8015DF54 */

block_8015DF54:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015DF54: 0x3C041F80 */
    a1 = 137;  /* 0x8015DF58: 0x24050089 */
    a2 = zero;  /* 0x8015DF5C: 0x00003021 */
    /* delay slot (always executes) */
    a3 = a2;  /* 0x8015DF64: 0x00C03821 */
    rs_jal(0x8015ABC0u, 0x8015DF68u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DF68u);
    goto block_8015DF68;

block_8015DF68:
    ;
    int cond_8015DF68 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DF6C: 0x00000000 */
    if (cond_8015DF68) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DF70u);
    }

block_8015DF70:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015DF74: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015DF78u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DF78u);
    goto block_8015DF78;

block_8015DF78:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015DF7C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015DF54u);
    goto block_8015DF54;  /* j */

block_8015DF80:
    cpu->write_word(sp + 16, zero);  /* 0x8015DF80: 0xAFA00010 */
    /* fall through to block_8015DF84 */

block_8015DF84:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015DF84: 0x3C041F80 */
    a1 = 136;  /* 0x8015DF88: 0x24050088 */
    a2 = zero;  /* 0x8015DF8C: 0x00003021 */
    /* delay slot (always executes) */
    a3 = 1;  /* 0x8015DF94: 0x24070001 */
    rs_jal(0x8015ABC0u, 0x8015DF98u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DF98u);
    goto block_8015DF98;

block_8015DF98:
    ;
    int cond_8015DF98 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DF9C: 0x00000000 */
    if (cond_8015DF98) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DFA0u);
    }

block_8015DFA0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015DFA4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015DFA8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DFA8u);
    goto block_8015DFA8;

block_8015DFA8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015DFAC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015DF84u);
    goto block_8015DF84;  /* j */

block_8015DFB0:
    cpu->write_word(sp + 16, zero);  /* 0x8015DFB0: 0xAFA00010 */
    /* fall through to block_8015DFB4 */

block_8015DFB4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015DFB4: 0x3C041F80 */
    a1 = 138;  /* 0x8015DFB8: 0x2405008A */
    a2 = 16;  /* 0x8015DFBC: 0x24060010 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015DFC4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015DFC8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DFC8u);
    goto block_8015DFC8;

block_8015DFC8:
    ;
    int cond_8015DFC8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DFCC: 0x00000000 */
    if (cond_8015DFC8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015DFD0u);
    }

block_8015DFD0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015DFD4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015DFD8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DFD8u);
    goto block_8015DFD8;

block_8015DFD8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015DFDC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015DFB4u);
    goto block_8015DFB4;  /* j */

block_8015DFE0:
    cpu->write_word(sp + 16, zero);  /* 0x8015DFE0: 0xAFA00010 */
    /* fall through to block_8015DFE4 */

block_8015DFE4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015DFE4: 0x3C041F80 */
    a1 = 139;  /* 0x8015DFE8: 0x2405008B */
    a2 = 1;  /* 0x8015DFEC: 0x24060001 */
    /* delay slot (always executes) */
    a3 = a2;  /* 0x8015DFF4: 0x00C03821 */
    rs_jal(0x8015ABC0u, 0x8015DFF8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015DFF8u);
    goto block_8015DFF8;

block_8015DFF8:
    ;
    int cond_8015DFF8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015DFFC: 0x00000000 */
    if (cond_8015DFF8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E000u);
    }

block_8015E000:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E004: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E008u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E008u);
    goto block_8015E008;

block_8015E008:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E00C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015DFE4u);
    goto block_8015DFE4;  /* j */

block_8015E010:
    cpu->write_word(sp + 16, zero);  /* 0x8015E010: 0xAFA00010 */
    /* fall through to block_8015E014 */

block_8015E014:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E014: 0x3C041F80 */
    a1 = 140;  /* 0x8015E018: 0x2405008C */
    a2 = 17;  /* 0x8015E01C: 0x24060011 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E024: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E028u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E028u);
    goto block_8015E028;

block_8015E028:
    ;
    int cond_8015E028 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E02C: 0x00000000 */
    if (cond_8015E028) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E030u);
    }

block_8015E030:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E034: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E038u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E038u);
    goto block_8015E038;

block_8015E038:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E03C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E014u);
    goto block_8015E014;  /* j */

block_8015E040:
    cpu->write_word(sp + 16, zero);  /* 0x8015E040: 0xAFA00010 */
    /* fall through to block_8015E044 */

block_8015E044:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E044: 0x3C041F80 */
    a1 = 141;  /* 0x8015E048: 0x2405008D */
    a2 = 18;  /* 0x8015E04C: 0x24060012 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E054: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E058u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E058u);
    goto block_8015E058;

block_8015E058:
    ;
    int cond_8015E058 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E05C: 0x00000000 */
    if (cond_8015E058) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E060u);
    }

block_8015E060:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E064: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E068u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E068u);
    goto block_8015E068;

block_8015E068:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E06C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E044u);
    goto block_8015E044;  /* j */

block_8015E070:
    cpu->write_word(sp + 16, zero);  /* 0x8015E070: 0xAFA00010 */
    /* fall through to block_8015E074 */

block_8015E074:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E074: 0x3C041F80 */
    a1 = 196;  /* 0x8015E078: 0x240500C4 */
    a2 = zero;  /* 0x8015E07C: 0x00003021 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E084: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E088u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E088u);
    goto block_8015E088;

block_8015E088:
    ;
    int cond_8015E088 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E08C: 0x00000000 */
    if (cond_8015E088) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E090u);
    }

block_8015E090:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E094: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E098u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E098u);
    goto block_8015E098;

block_8015E098:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E09C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E074u);
    goto block_8015E074;  /* j */

block_8015E0A0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E0A0: 0xAFA00010 */
    /* fall through to block_8015E0A4 */

block_8015E0A4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E0A4: 0x3C041F80 */
    a1 = 197;  /* 0x8015E0A8: 0x240500C5 */
    a2 = 19;  /* 0x8015E0AC: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E0B4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E0B8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E0B8u);
    goto block_8015E0B8;

block_8015E0B8:
    ;
    int cond_8015E0B8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E0BC: 0x00000000 */
    if (cond_8015E0B8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E0C0u);
    }

block_8015E0C0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E0C4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E0C8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E0C8u);
    goto block_8015E0C8;

block_8015E0C8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E0CC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E0A4u);
    goto block_8015E0A4;  /* j */

block_8015E0D0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E0D0: 0xAFA00010 */
    /* fall through to block_8015E0D4 */

block_8015E0D4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E0D4: 0x3C041F80 */
    a1 = 204;  /* 0x8015E0D8: 0x240500CC */
    a2 = 1;  /* 0x8015E0DC: 0x24060001 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E0E4: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E0E8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E0E8u);
    goto block_8015E0E8;

block_8015E0E8:
    ;
    int cond_8015E0E8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E0EC: 0x00000000 */
    if (cond_8015E0E8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E0F0u);
    }

block_8015E0F0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E0F4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E0F8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E0F8u);
    goto block_8015E0F8;

block_8015E0F8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E0FC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E0D4u);
    goto block_8015E0D4;  /* j */

block_8015E100:
    cpu->write_word(sp + 16, zero);  /* 0x8015E100: 0xAFA00010 */
    /* fall through to block_8015E104 */

block_8015E104:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E104: 0x3C041F80 */
    a1 = 205;  /* 0x8015E108: 0x240500CD */
    a2 = 19;  /* 0x8015E10C: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E114: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E118u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E118u);
    goto block_8015E118;

block_8015E118:
    ;
    int cond_8015E118 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E11C: 0x00000000 */
    if (cond_8015E118) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E120u);
    }

block_8015E120:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E124: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E128u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E128u);
    goto block_8015E128;

block_8015E128:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E12C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E104u);
    goto block_8015E104;  /* j */

block_8015E130:
    cpu->write_word(sp + 16, zero);  /* 0x8015E130: 0xAFA00010 */
    /* fall through to block_8015E134 */

block_8015E134:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E134: 0x3C041F80 */
    a1 = 212;  /* 0x8015E138: 0x240500D4 */
    a2 = 2;  /* 0x8015E13C: 0x24060002 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E144: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E148u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E148u);
    goto block_8015E148;

block_8015E148:
    ;
    int cond_8015E148 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E14C: 0x00000000 */
    if (cond_8015E148) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E150u);
    }

block_8015E150:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E154: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E158u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E158u);
    goto block_8015E158;

block_8015E158:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E15C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E134u);
    goto block_8015E134;  /* j */

block_8015E160:
    cpu->write_word(sp + 16, zero);  /* 0x8015E160: 0xAFA00010 */
    /* fall through to block_8015E164 */

block_8015E164:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E164: 0x3C041F80 */
    a1 = 213;  /* 0x8015E168: 0x240500D5 */
    a2 = 19;  /* 0x8015E16C: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E174: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E178u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E178u);
    goto block_8015E178;

block_8015E178:
    ;
    int cond_8015E178 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E17C: 0x00000000 */
    if (cond_8015E178) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E180u);
    }

block_8015E180:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E184: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E188u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E188u);
    goto block_8015E188;

block_8015E188:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E18C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E164u);
    goto block_8015E164;  /* j */

block_8015E190:
    cpu->write_word(sp + 16, zero);  /* 0x8015E190: 0xAFA00010 */
    /* fall through to block_8015E194 */

block_8015E194:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E194: 0x3C041F80 */
    a1 = 220;  /* 0x8015E198: 0x240500DC */
    a2 = 3;  /* 0x8015E19C: 0x24060003 */
    /* delay slot (always executes) */
    a3 = a2;  /* 0x8015E1A4: 0x00C03821 */
    rs_jal(0x8015ABC0u, 0x8015E1A8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E1A8u);
    goto block_8015E1A8;

block_8015E1A8:
    ;
    int cond_8015E1A8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E1AC: 0x00000000 */
    if (cond_8015E1A8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E1B0u);
    }

block_8015E1B0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E1B4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E1B8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E1B8u);
    goto block_8015E1B8;

block_8015E1B8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E1BC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E194u);
    goto block_8015E194;  /* j */

block_8015E1C0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E1C0: 0xAFA00010 */
    /* fall through to block_8015E1C4 */

block_8015E1C4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E1C4: 0x3C041F80 */
    a1 = 221;  /* 0x8015E1C8: 0x240500DD */
    a2 = 19;  /* 0x8015E1CC: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E1D4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E1D8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E1D8u);
    goto block_8015E1D8;

block_8015E1D8:
    ;
    int cond_8015E1D8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E1DC: 0x00000000 */
    if (cond_8015E1D8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E1E0u);
    }

block_8015E1E0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E1E4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E1E8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E1E8u);
    goto block_8015E1E8;

block_8015E1E8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E1EC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E1C4u);
    goto block_8015E1C4;  /* j */

block_8015E1F0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E1F0: 0xAFA00010 */
    /* fall through to block_8015E1F4 */

block_8015E1F4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E1F4: 0x3C041F80 */
    a1 = 228;  /* 0x8015E1F8: 0x240500E4 */
    a2 = 4;  /* 0x8015E1FC: 0x24060004 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E204: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E208u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E208u);
    goto block_8015E208;

block_8015E208:
    ;
    int cond_8015E208 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E20C: 0x00000000 */
    if (cond_8015E208) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E210u);
    }

block_8015E210:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E214: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E218u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E218u);
    goto block_8015E218;

block_8015E218:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E21C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E1F4u);
    goto block_8015E1F4;  /* j */

block_8015E220:
    cpu->write_word(sp + 16, zero);  /* 0x8015E220: 0xAFA00010 */
    /* fall through to block_8015E224 */

block_8015E224:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E224: 0x3C041F80 */
    a1 = 229;  /* 0x8015E228: 0x240500E5 */
    a2 = 19;  /* 0x8015E22C: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E234: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E238u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E238u);
    goto block_8015E238;

block_8015E238:
    ;
    int cond_8015E238 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E23C: 0x00000000 */
    if (cond_8015E238) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E240u);
    }

block_8015E240:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E244: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E248u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E248u);
    goto block_8015E248;

block_8015E248:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E24C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E224u);
    goto block_8015E224;  /* j */

block_8015E250:
    cpu->write_word(sp + 16, zero);  /* 0x8015E250: 0xAFA00010 */
    /* fall through to block_8015E254 */

block_8015E254:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E254: 0x3C041F80 */
    a1 = 236;  /* 0x8015E258: 0x240500EC */
    a2 = 5;  /* 0x8015E25C: 0x24060005 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E264: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E268u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E268u);
    goto block_8015E268;

block_8015E268:
    ;
    int cond_8015E268 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E26C: 0x00000000 */
    if (cond_8015E268) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E270u);
    }

block_8015E270:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E274: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E278u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E278u);
    goto block_8015E278;

block_8015E278:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E27C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E254u);
    goto block_8015E254;  /* j */

block_8015E280:
    cpu->write_word(sp + 16, zero);  /* 0x8015E280: 0xAFA00010 */
    /* fall through to block_8015E284 */

block_8015E284:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E284: 0x3C041F80 */
    a1 = 237;  /* 0x8015E288: 0x240500ED */
    a2 = 19;  /* 0x8015E28C: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E294: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E298u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E298u);
    goto block_8015E298;

block_8015E298:
    ;
    int cond_8015E298 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E29C: 0x00000000 */
    if (cond_8015E298) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E2A0u);
    }

block_8015E2A0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E2A4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E2A8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E2A8u);
    goto block_8015E2A8;

block_8015E2A8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E2AC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E284u);
    goto block_8015E284;  /* j */

block_8015E2B0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E2B0: 0xAFA00010 */
    /* fall through to block_8015E2B4 */

block_8015E2B4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E2B4: 0x3C041F80 */
    a1 = 244;  /* 0x8015E2B8: 0x240500F4 */
    a2 = 6;  /* 0x8015E2BC: 0x24060006 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E2C4: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E2C8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E2C8u);
    goto block_8015E2C8;

block_8015E2C8:
    ;
    int cond_8015E2C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E2CC: 0x00000000 */
    if (cond_8015E2C8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E2D0u);
    }

block_8015E2D0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E2D4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E2D8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E2D8u);
    goto block_8015E2D8;

block_8015E2D8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E2DC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E2B4u);
    goto block_8015E2B4;  /* j */

block_8015E2E0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E2E0: 0xAFA00010 */
    /* fall through to block_8015E2E4 */

block_8015E2E4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E2E4: 0x3C041F80 */
    a1 = 245;  /* 0x8015E2E8: 0x240500F5 */
    a2 = 19;  /* 0x8015E2EC: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E2F4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E2F8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E2F8u);
    goto block_8015E2F8;

block_8015E2F8:
    ;
    int cond_8015E2F8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E2FC: 0x00000000 */
    if (cond_8015E2F8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E300u);
    }

block_8015E300:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E304: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E308u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E308u);
    goto block_8015E308;

block_8015E308:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E30C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E2E4u);
    goto block_8015E2E4;  /* j */

block_8015E310:
    cpu->write_word(sp + 16, zero);  /* 0x8015E310: 0xAFA00010 */
    /* fall through to block_8015E314 */

block_8015E314:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E314: 0x3C041F80 */
    a1 = 252;  /* 0x8015E318: 0x240500FC */
    a2 = 7;  /* 0x8015E31C: 0x24060007 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E324: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E328u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E328u);
    goto block_8015E328;

block_8015E328:
    ;
    int cond_8015E328 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E32C: 0x00000000 */
    if (cond_8015E328) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E330u);
    }

block_8015E330:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E334: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E338u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E338u);
    goto block_8015E338;

block_8015E338:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E33C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E314u);
    goto block_8015E314;  /* j */

block_8015E340:
    cpu->write_word(sp + 16, zero);  /* 0x8015E340: 0xAFA00010 */
    /* fall through to block_8015E344 */

block_8015E344:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E344: 0x3C041F80 */
    a1 = 253;  /* 0x8015E348: 0x240500FD */
    a2 = 19;  /* 0x8015E34C: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E354: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E358u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E358u);
    goto block_8015E358;

block_8015E358:
    ;
    int cond_8015E358 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E35C: 0x00000000 */
    if (cond_8015E358) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E360u);
    }

block_8015E360:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E364: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E368u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E368u);
    goto block_8015E368;

block_8015E368:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E36C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E344u);
    goto block_8015E344;  /* j */

block_8015E370:
    cpu->write_word(sp + 16, zero);  /* 0x8015E370: 0xAFA00010 */
    /* fall through to block_8015E374 */

block_8015E374:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E374: 0x3C041F80 */
    a1 = 260;  /* 0x8015E378: 0x24050104 */
    a2 = 8;  /* 0x8015E37C: 0x24060008 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E384: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E388u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E388u);
    goto block_8015E388;

block_8015E388:
    ;
    int cond_8015E388 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E38C: 0x00000000 */
    if (cond_8015E388) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E390u);
    }

block_8015E390:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E394: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E398u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E398u);
    goto block_8015E398;

block_8015E398:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E39C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E374u);
    goto block_8015E374;  /* j */

block_8015E3A0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E3A0: 0xAFA00010 */
    /* fall through to block_8015E3A4 */

block_8015E3A4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E3A4: 0x3C041F80 */
    a1 = 261;  /* 0x8015E3A8: 0x24050105 */
    a2 = 19;  /* 0x8015E3AC: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E3B4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E3B8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E3B8u);
    goto block_8015E3B8;

block_8015E3B8:
    ;
    int cond_8015E3B8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E3BC: 0x00000000 */
    if (cond_8015E3B8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E3C0u);
    }

block_8015E3C0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E3C4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E3C8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E3C8u);
    goto block_8015E3C8;

block_8015E3C8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E3CC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E3A4u);
    goto block_8015E3A4;  /* j */

block_8015E3D0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E3D0: 0xAFA00010 */
    /* fall through to block_8015E3D4 */

block_8015E3D4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E3D4: 0x3C041F80 */
    a1 = 268;  /* 0x8015E3D8: 0x2405010C */
    a2 = 9;  /* 0x8015E3DC: 0x24060009 */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E3E4: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E3E8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E3E8u);
    goto block_8015E3E8;

block_8015E3E8:
    ;
    int cond_8015E3E8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E3EC: 0x00000000 */
    if (cond_8015E3E8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E3F0u);
    }

block_8015E3F0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E3F4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E3F8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E3F8u);
    goto block_8015E3F8;

block_8015E3F8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E3FC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E3D4u);
    goto block_8015E3D4;  /* j */

block_8015E400:
    cpu->write_word(sp + 16, zero);  /* 0x8015E400: 0xAFA00010 */
    /* fall through to block_8015E404 */

block_8015E404:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E404: 0x3C041F80 */
    a1 = 269;  /* 0x8015E408: 0x2405010D */
    a2 = 19;  /* 0x8015E40C: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E414: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E418u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E418u);
    goto block_8015E418;

block_8015E418:
    ;
    int cond_8015E418 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E41C: 0x00000000 */
    if (cond_8015E418) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E420u);
    }

block_8015E420:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E424: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E428u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E428u);
    goto block_8015E428;

block_8015E428:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E42C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E404u);
    goto block_8015E404;  /* j */

block_8015E430:
    cpu->write_word(sp + 16, zero);  /* 0x8015E430: 0xAFA00010 */
    /* fall through to block_8015E434 */

block_8015E434:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E434: 0x3C041F80 */
    a1 = 276;  /* 0x8015E438: 0x24050114 */
    a2 = 10;  /* 0x8015E43C: 0x2406000A */
    /* delay slot (always executes) */
    a3 = 3;  /* 0x8015E444: 0x24070003 */
    rs_jal(0x8015ABC0u, 0x8015E448u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E448u);
    goto block_8015E448;

block_8015E448:
    ;
    int cond_8015E448 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E44C: 0x00000000 */
    if (cond_8015E448) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E450u);
    }

block_8015E450:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E454: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E458u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E458u);
    goto block_8015E458;

block_8015E458:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E45C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E434u);
    goto block_8015E434;  /* j */

block_8015E460:
    cpu->write_word(sp + 16, zero);  /* 0x8015E460: 0xAFA00010 */
    /* fall through to block_8015E464 */

block_8015E464:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E464: 0x3C041F80 */
    a1 = 277;  /* 0x8015E468: 0x24050115 */
    a2 = 19;  /* 0x8015E46C: 0x24060013 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E474: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E478u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E478u);
    goto block_8015E478;

block_8015E478:
    ;
    int cond_8015E478 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E47C: 0x00000000 */
    if (cond_8015E478) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E480u);
    }

block_8015E480:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E484: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E488u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E488u);
    goto block_8015E488;

block_8015E488:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E48C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E464u);
    goto block_8015E464;  /* j */

block_8015E490:
    cpu->write_word(sp + 16, zero);  /* 0x8015E490: 0xAFA00010 */
    /* fall through to block_8015E494 */

block_8015E494:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E494: 0x3C041F80 */
    a1 = 176;  /* 0x8015E498: 0x240500B0 */
    a2 = zero;  /* 0x8015E49C: 0x00003021 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E4A4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E4A8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E4A8u);
    goto block_8015E4A8;

block_8015E4A8:
    ;
    int cond_8015E4A8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E4AC: 0x00000000 */
    if (cond_8015E4A8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E4B0u);
    }

block_8015E4B0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E4B4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E4B8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E4B8u);
    goto block_8015E4B8;

block_8015E4B8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E4BC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E494u);
    goto block_8015E494;  /* j */

block_8015E4C0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E4C0: 0xAFA00010 */
    /* fall through to block_8015E4C4 */

block_8015E4C4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E4C4: 0x3C041F80 */
    a1 = 177;  /* 0x8015E4C8: 0x240500B1 */
    a2 = 1;  /* 0x8015E4CC: 0x24060001 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E4D4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E4D8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E4D8u);
    goto block_8015E4D8;

block_8015E4D8:
    ;
    int cond_8015E4D8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E4DC: 0x00000000 */
    if (cond_8015E4D8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E4E0u);
    }

block_8015E4E0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E4E4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E4E8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E4E8u);
    goto block_8015E4E8;

block_8015E4E8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E4EC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E4C4u);
    goto block_8015E4C4;  /* j */

block_8015E4F0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E4F0: 0xAFA00010 */
    /* fall through to block_8015E4F4 */

block_8015E4F4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E4F4: 0x3C041F80 */
    a1 = 178;  /* 0x8015E4F8: 0x240500B2 */
    a2 = 2;  /* 0x8015E4FC: 0x24060002 */
    /* delay slot (always executes) */
    a3 = a2;  /* 0x8015E504: 0x00C03821 */
    rs_jal(0x8015ABC0u, 0x8015E508u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E508u);
    goto block_8015E508;

block_8015E508:
    ;
    int cond_8015E508 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E50C: 0x00000000 */
    if (cond_8015E508) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E510u);
    }

block_8015E510:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E514: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E518u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E518u);
    goto block_8015E518;

block_8015E518:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E51C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E4F4u);
    goto block_8015E4F4;  /* j */

block_8015E520:
    cpu->write_word(sp + 16, zero);  /* 0x8015E520: 0xAFA00010 */
    /* fall through to block_8015E524 */

block_8015E524:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E524: 0x3C041F80 */
    a1 = 179;  /* 0x8015E528: 0x240500B3 */
    a2 = 3;  /* 0x8015E52C: 0x24060003 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E534: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E538u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E538u);
    goto block_8015E538;

block_8015E538:
    ;
    int cond_8015E538 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E53C: 0x00000000 */
    if (cond_8015E538) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E540u);
    }

block_8015E540:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E544: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E548u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E548u);
    goto block_8015E548;

block_8015E548:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E54C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E524u);
    goto block_8015E524;  /* j */

block_8015E550:
    cpu->write_word(sp + 16, zero);  /* 0x8015E550: 0xAFA00010 */
    /* fall through to block_8015E554 */

block_8015E554:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E554: 0x3C041F80 */
    a1 = 180;  /* 0x8015E558: 0x240500B4 */
    a2 = 4;  /* 0x8015E55C: 0x24060004 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E564: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E568u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E568u);
    goto block_8015E568;

block_8015E568:
    ;
    int cond_8015E568 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E56C: 0x00000000 */
    if (cond_8015E568) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E570u);
    }

block_8015E570:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E574: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E578u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E578u);
    goto block_8015E578;

block_8015E578:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E57C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E554u);
    goto block_8015E554;  /* j */

block_8015E580:
    cpu->write_word(sp + 16, zero);  /* 0x8015E580: 0xAFA00010 */
    /* fall through to block_8015E584 */

block_8015E584:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E584: 0x3C041F80 */
    a1 = 181;  /* 0x8015E588: 0x240500B5 */
    a2 = 5;  /* 0x8015E58C: 0x24060005 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E594: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E598u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E598u);
    goto block_8015E598;

block_8015E598:
    ;
    int cond_8015E598 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E59C: 0x00000000 */
    if (cond_8015E598) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E5A0u);
    }

block_8015E5A0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E5A4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E5A8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E5A8u);
    goto block_8015E5A8;

block_8015E5A8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E5AC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E584u);
    goto block_8015E584;  /* j */

block_8015E5B0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E5B0: 0xAFA00010 */
    /* fall through to block_8015E5B4 */

block_8015E5B4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E5B4: 0x3C041F80 */
    a1 = 182;  /* 0x8015E5B8: 0x240500B6 */
    a2 = 6;  /* 0x8015E5BC: 0x24060006 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E5C4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E5C8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E5C8u);
    goto block_8015E5C8;

block_8015E5C8:
    ;
    int cond_8015E5C8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E5CC: 0x00000000 */
    if (cond_8015E5C8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E5D0u);
    }

block_8015E5D0:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E5D4: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E5D8u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E5D8u);
    goto block_8015E5D8;

block_8015E5D8:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E5DC: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E5B4u);
    goto block_8015E5B4;  /* j */

block_8015E5E0:
    cpu->write_word(sp + 16, zero);  /* 0x8015E5E0: 0xAFA00010 */
    /* fall through to block_8015E5E4 */

block_8015E5E4:
    a0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E5E4: 0x3C041F80 */
    a1 = 183;  /* 0x8015E5E8: 0x240500B7 */
    a2 = 7;  /* 0x8015E5EC: 0x24060007 */
    /* delay slot (always executes) */
    a3 = 2;  /* 0x8015E5F4: 0x24070002 */
    rs_jal(0x8015ABC0u, 0x8015E5F8u);  /* jal menu_sprite_load_sm */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E5F8u);
    goto block_8015E5F8;

block_8015E5F8:
    ;
    int cond_8015E5F8 = (v0 != zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E5FC: 0x00000000 */
    if (cond_8015E5F8) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E600u);
    }

block_8015E600:
    /* delay slot (always executes) */
    a0 = zero;  /* 0x8015E604: 0x00002021 */
    rs_jal(0x8019A460u, 0x8015E608u);  /* jal cpst_8019a460 */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E608u);
    goto block_8015E608;

block_8015E608:
    /* delay slot (always executes) */
    cpu->write_word(sp + 16, zero);  /* 0x8015E60C: 0xAFA00010 */
    psx_check_interrupts_at(cpu, 0x8015E5E4u);
    goto block_8015E5E4;  /* j */

block_8015E610:
    a2 = 0x801215FCu;  /* 0x80120000 5628 */  /* 0x80120000 */  /* 0x8015E610: 0x3C068012 */  /* 0x8015E614: 0x24C615FC */
    a3 = 4096;  /* 0x8015E618: 0x24071000 */
    s0 = 0x1F800000u;  /* 0x1F800000 */  /* 0x8015E61C: 0x3C101F80 */
    s2 = a3;  /* 0x8015E620: 0x00E09021 */
    a0 = cpu->read_word(s0 + 8);  /* 0x8015E624: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x8015E628: 0x8E05000C */
    s1 = s4;  /* 0x8015E62C: 0x02808821 */
    cpu->write_word(sp + 16, s2);  /* 0x8015E630: 0xAFB20010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s1);  /* 0x8015E638: 0xAFB10014 */
    rs_jal(0x8012E58Cu, 0x8015E63Cu);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E63Cu);
    goto block_8015E63C;

block_8015E63C:
    v0 = 0x801B0000u;  /* 0x801B0000 */  /* 0x8015E63C: 0x3C02801B */
    a0 = cpu->read_word(s0 + 8);  /* 0x8015E640: 0x8E040008 */
    a1 = cpu->read_word(s0 + 12);  /* 0x8015E644: 0x8E05000C */
    v1 = (int32_t)(int16_t)cpu->read_half(s5 + 30);  /* 0x8015E648: 0x86A3001E */
    v0 += 16540;  /* 0x8015E64C: 0x2442409C */
    cpu->write_word(sp + 16, s2);  /* 0x8015E650: 0xAFB20010 */
    cpu->write_word(sp + 20, s1);  /* 0x8015E654: 0xAFB10014 */
    v1 <<= 2;  /* 0x8015E658: 0x00031880 */
    v1 += v0;  /* 0x8015E65C: 0x00621821 */
    a2 = cpu->read_word(v1);  /* 0x8015E660: 0x8C660000 */
    /* delay slot (always executes) */
    a3 = s6;  /* 0x8015E668: 0x02C03821 */
    psx_check_interrupts_at(cpu, 0x8015E830u);
    goto block_8015E830;  /* j */

block_8015E66C:
    a0 = s5 + 32;  /* 0x8015E66C: 0x26A40020 */
    a1 = s1 << 16;  /* 0x8015E670: 0x00112C00 */
    a1 = (int32_t)a1 >> 16;  /* 0x8015E674: 0x00052C03 */
    /* delay slot (always executes) */
    a2 = 1;  /* 0x8015E67C: 0x24060001 */
    rs_jal(0x80129978u, 0x8015E680u);  /* jal u16_counter_step_pad_a */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E680u);
    goto block_8015E680;

block_8015E680:
    v0 = cpu->read_half(s3 + 124);  /* 0x8015E680: 0x9662007C */
    /* nop */  /* 0x8015E684: 0x00000000 */
    v0 &= 0x00E0;  /* 0x8015E688: 0x304200E0 */
    int cond_8015E68C = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;  /* 0x8015E690: 0x24040400 */
    if (cond_8015E68C) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E694u);
    }

block_8015E694:
    a1 = (int32_t)(int16_t)cpu->read_half(s5 + 32);  /* 0x8015E694: 0x86A50020 */
    a2 = zero;  /* 0x8015E698: 0x00003021 */
    /* delay slot (always executes) */
    a1 += s2;  /* 0x8015E6A0: 0x00B22821 */
    rs_jal(0x80156EF4u, 0x8015E6A4u);  /* jal deferred_cmd_post */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E6A4u);
    goto block_8015E6A4;

block_8015E6A4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E6A8: 0x00000000 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015E6AC:
    a2 += 5636;  /* 0x8015E6AC: 0x24C61604 */
    a3 = 4096;  /* 0x8015E6B0: 0x24071000 */
    s1 = a3;  /* 0x8015E6B4: 0x00E08821 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015E6B8: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015E6BC: 0x8E65000C */
    s0 = s4;  /* 0x8015E6C0: 0x02808021 */
    cpu->write_word(sp + 16, s1);  /* 0x8015E6C4: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015E6CC: 0xAFB00014 */
    rs_jal(0x8012E58Cu, 0x8015E6D0u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E6D0u);
    goto block_8015E6D0;

block_8015E6D0:
    a0 = sp + 40;  /* 0x8015E6D0: 0x27A40028 */
    a1 = 0x80120000u;  /* 0x80120000 */  /* 0x8015E6D4: 0x3C058012 */
    a2 = (int32_t)(int16_t)cpu->read_half(s5 + 32);  /* 0x8015E6D8: 0x86A60020 */
    a1 += 5620;  /* 0x8015E6DC: 0x24A515F4 */
    /* delay slot (always executes) */
    a2++;  /* 0x8015E6E4: 0x24C60001 */
    rs_jal(0x8018EF60u, 0x8015E6E8u);  /* jal sprintf_core_guest */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E6E8u);
    goto block_8015E6E8;

block_8015E6E8:
    a2 = sp + 40;  /* 0x8015E6E8: 0x27A60028 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015E6EC: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015E6F0: 0x8E65000C */
    a3 = s6;  /* 0x8015E6F4: 0x02C03821 */
    cpu->write_word(sp + 16, s1);  /* 0x8015E6F8: 0xAFB10010 */
    /* delay slot (always executes) */
    cpu->write_word(sp + 20, s0);  /* 0x8015E700: 0xAFB00014 */
    psx_check_interrupts_at(cpu, 0x8015E830u);
    goto block_8015E830;  /* j */

block_8015E704:
    v0 = cpu->read_half(s3 + 124);  /* 0x8015E704: 0x9662007C */
    /* nop */  /* 0x8015E708: 0x00000000 */
    v0 &= 0x00E0;  /* 0x8015E70C: 0x304200E0 */
    int cond_8015E710 = (v0 == zero);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    a0 = 1024;  /* 0x8015E714: 0x24040400 */
    if (cond_8015E710) {
        psx_check_interrupts_at(cpu, 0x8015E848u);
        goto block_8015E848;  /* taken */
    } else {
        psx_check_interrupts_at(cpu, 0x8015E718u);
    }

block_8015E718:
    a1 = 12;  /* 0x8015E718: 0x2405000C */
    /* delay slot (always executes) */
    a2 = zero;  /* 0x8015E720: 0x00003021 */
    rs_jal(0x80156EF4u, 0x8015E724u);  /* jal deferred_cmd_post */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E724u);
    goto block_8015E724;

block_8015E724:
    a0 = 515;  /* 0x8015E724: 0x24040203 */
    a1 = zero;  /* 0x8015E728: 0x00002821 */
    /* delay slot (always executes) */
    a2 = a1;  /* 0x8015E730: 0x00A03021 */
    rs_jal(0x80156EF4u, 0x8015E734u);  /* jal deferred_cmd_post */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E734u);
    goto block_8015E734;

block_8015E734:
    v1 = 0x801B1280u;  /* 0x801B0000 4736 */  /* 0x801B0000 */  /* 0x8015E734: 0x3C03801B */  /* 0x8015E738: 0x24631280 */
    v0 = cpu->read_byte(v1 + 12);  /* 0x8015E73C: 0x9062000C */
    /* nop */  /* 0x8015E740: 0x00000000 */
    cpu->write_half(s5 + 20, (uint16_t)v0);  /* 0x8015E744: 0xA6A20014 */
    v0 = cpu->read_byte(v1 + 13);  /* 0x8015E748: 0x9062000D */
    /* nop */  /* 0x8015E74C: 0x00000000 */
    cpu->write_half(s5 + 22, (uint16_t)v0);  /* 0x8015E750: 0xA6A20016 */
    v0 = cpu->read_byte(v1 + 14);  /* 0x8015E754: 0x9062000E */
    /* nop */  /* 0x8015E758: 0x00000000 */
    cpu->write_half(s5 + 24, (uint16_t)v0);  /* 0x8015E75C: 0xA6A20018 */
    v0 = cpu->read_byte(v1 + 15);  /* 0x8015E760: 0x9062000F */
    cpu->write_half(s5 + 28, (uint16_t)zero);  /* 0x8015E764: 0xA6A0001C */
    cpu->write_half(s5 + 30, (uint16_t)zero);  /* 0x8015E768: 0xA6A0001E */
    cpu->write_half(s5 + 32, (uint16_t)zero);  /* 0x8015E76C: 0xA6A00020 */
    cpu->write_half(s5 + 26, (uint16_t)v0);  /* 0x8015E770: 0xA6A2001A */
    v1 = cpu->read_word(s3 + 192);  /* 0x8015E774: 0x8E6300C0 */
    v0 = cpu->read_byte(s5 + 22);  /* 0x8015E778: 0x92A20016 */
    /* nop */  /* 0x8015E77C: 0x00000000 */
    cpu->write_byte(v1 + 13, (uint8_t)v0);  /* 0x8015E780: 0xA062000D */
    v1 = cpu->read_word(s3 + 192);  /* 0x8015E784: 0x8E6300C0 */
    v0 = cpu->read_byte(s5 + 20);  /* 0x8015E788: 0x92A20014 */
    /* nop */  /* 0x8015E78C: 0x00000000 */
    cpu->write_byte(v1 + 12, (uint8_t)v0);  /* 0x8015E790: 0xA062000C */
    v0 = cpu->read_word(s3 + 192);  /* 0x8015E794: 0x8E6200C0 */
    /* nop */  /* 0x8015E798: 0x00000000 */
    a0 = cpu->read_byte(v0 + 12);  /* 0x8015E79C: 0x9044000C */
    /* delay slot (always executes) */
    a0 ^= 0x0001;  /* 0x8015E7A4: 0x38840001 */
    rs_jal(0x801561C8u, 0x8015E7A8u);  /* jal brightness_commit */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E7A8u);
    goto block_8015E7A8;

block_8015E7A8:
    v1 = cpu->read_word(s3 + 192);  /* 0x8015E7A8: 0x8E6300C0 */
    v0 = cpu->read_byte(s5 + 24);  /* 0x8015E7AC: 0x92A20018 */
    /* nop */  /* 0x8015E7B0: 0x00000000 */
    cpu->write_byte(v1 + 14, (uint8_t)v0);  /* 0x8015E7B4: 0xA062000E */
    /* fall through to block_8015E7B8 */

block_8015E7B8:
    v1 = cpu->read_word(s3 + 192);  /* 0x8015E7B8: 0x8E6300C0 */
    v0 = cpu->read_byte(s5 + 26);  /* 0x8015E7BC: 0x92A2001A */
    /* nop */  /* 0x8015E7C0: 0x00000000 */
    cpu->write_byte(v1 + 15, (uint8_t)v0);  /* 0x8015E7C4: 0xA062000F */
    v0 = cpu->read_word(s3 + 192);  /* 0x8015E7C8: 0x8E6200C0 */
    v1 = 0x66660000u;  /* 0x66660000 */  /* 0x8015E7CC: 0x3C036666 */
    a1 = cpu->read_byte(v0 + 15);  /* 0x8015E7D0: 0x9045000F */
    v1 |= 0x6667;  /* 0x8015E7D4: 0x34636667 */
    a1 <<= 3;  /* 0x8015E7D8: 0x000528C0 */
    { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v1; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(a1));  /* 0x8015E7DC: 0x00A30018 */
    a0 = cpu->read_byte(v0 + 14);  /* 0x8015E7E0: 0x9044000E */
    a1 = (int32_t)a1 >> 31;  /* 0x8015E7E4: 0x00052FC3 */
    t2 = cpu->hi;
    psx_muldiv_stall(cpu);  /* 0x8015E7E8: 0x00005010 */
    v0 = (int32_t)t2 >> 2;  /* 0x8015E7EC: 0x000A1083 */
    a1 = v0 - a1;  /* 0x8015E7F0: 0x00452823 */
    /* fall through to block_8015E7F4 */

block_8015E7F4:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E7F8: 0x00000000 */
    rs_jal(0x80156048u, 0x8015E7FCu);  /* jal gauge_scale_fixed */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E7FCu);
    goto block_8015E7FC;

block_8015E7FC:
    v0 = 1;  /* 0x8015E7FC: 0x24020001 */
    /* delay slot (always executes) */
    cpu->write_word(s7 + 1040, v0);  /* 0x8015E804: 0xAEE20410 */
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;  /* j */

block_8015E808:
    a2 = 0x80120000u;  /* 0x80120000 */  /* 0x8015E808: 0x3C068012 */
    /* delay slot (always executes) */
    a2 += 4880;  /* 0x8015E810: 0x24C61310 */
    psx_check_interrupts_at(cpu, 0x8015E818u);
    goto block_8015E818;  /* j */

block_8015E814:
    a2 += 4888;  /* 0x8015E814: 0x24C61318 */
    /* fall through to block_8015E818 */

block_8015E818:
    a3 = 4096;  /* 0x8015E818: 0x24071000 */
    a0 = cpu->read_word(s3 + 8);  /* 0x8015E81C: 0x8E640008 */
    a1 = cpu->read_word(s3 + 12);  /* 0x8015E820: 0x8E65000C */
    v0 = a3;  /* 0x8015E824: 0x00E01021 */
    cpu->write_word(sp + 16, v0);  /* 0x8015E828: 0xAFA20010 */
    cpu->write_word(sp + 20, s4);  /* 0x8015E82C: 0xAFB40014 */
    /* fall through to block_8015E830 */

block_8015E830:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E834: 0x00000000 */
    rs_jal(0x8012E58Cu, 0x8015E838u);  /* jal announce_param_block_init */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E838u);
    goto block_8015E838;

block_8015E838:
    a0 = zero;  /* 0x8015E838: 0x00002021 */
    /* fall through to block_8015E83C */

block_8015E83C:
    a1 = 12;  /* 0x8015E83C: 0x2405000C */
    /* fall through to block_8015E840 */

block_8015E840:
    /* delay slot (always executes) */
    /* nop */  /* 0x8015E844: 0x00000000 */
    rs_jal(0x8012CDD8u, 0x8015E848u);  /* jal scroll_offset_accumulate */
    if (br2_bail_pending()) return cpu->gpr[2];
    psx_check_interrupts_at(cpu, 0x8015E848u);
    goto block_8015E848;

block_8015E848:
    ra = cpu->read_word(sp + 92);  /* 0x8015E848: 0x8FBF005C */
    fp = cpu->read_word(sp + 88);  /* 0x8015E84C: 0x8FBE0058 */
    s7 = cpu->read_word(sp + 84);  /* 0x8015E850: 0x8FB70054 */
    s6 = cpu->read_word(sp + 80);  /* 0x8015E854: 0x8FB60050 */
    s5 = cpu->read_word(sp + 76);  /* 0x8015E858: 0x8FB5004C */
    s4 = cpu->read_word(sp + 72);  /* 0x8015E85C: 0x8FB40048 */
    s3 = cpu->read_word(sp + 68);  /* 0x8015E860: 0x8FB30044 */
    s2 = cpu->read_word(sp + 64);  /* 0x8015E864: 0x8FB20040 */
    s1 = cpu->read_word(sp + 60);  /* 0x8015E868: 0x8FB1003C */
    s0 = cpu->read_word(sp + 56);  /* 0x8015E86C: 0x8FB00038 */
    v0 = zero;  /* 0x8015E870: 0x00001021 */
    uint32_t jt_8015E874 = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp += 96;  /* 0x8015E878: 0x27BD0060 */
    psx_check_interrupts_at(cpu, jt_8015E874);
    cpu->pc = jt_8015E874; return cpu->gpr[2];  /* CPS: jr $ra */
}
