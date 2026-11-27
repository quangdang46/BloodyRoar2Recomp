/* w2_e03.c — clean CPS ports of the ScriptVM Bra (op01) sub-operation
 * pieces (guest 0x80183008-0x8018310C + shared tails to 0x8018322C,
 * SLES_017.22_full_38.c, inside the 0x80182F48 coroutine body).
 *
 * These addrs are CPS block leaders / continuation PCs of the Bra
 * handler, NOT standalone `jr $ra` functions: the generated code keeps
 * them as `br2eu_psx_alias_body_80182F48` entries reached via
 * cpu->pc dispatch (IRQ resume, branch splits). They share the caller's
 * 0x48-byte frame (s0=script pc, s1=ctx, s3=ctrl; scratch u16 table at
 * sp+0x10) and the common epilogue at 0x8018322C.
 *
 * Semantics (matching the generated blocks op-for-op):
 *   0x80183008 — sub1 if/else: cmp=ctx+0x38 s32; cmp>0 -> sel=lhu(pc+2),
 *                cmp<=0 -> ctx+0x38=0, sel=lhu(pc+6); sel!=0 -> pc=
 *                base+sel else pc+=8.
 *   0x8018304C..0x801830EC — relational jumps on ctx+0x38: ==0,!=0,<=0,
 *                <0,>=0,>0. Condition met -> pc+=4; else pc=base+lhu(pc+2).
 *   0x8018310C — weighted-random pick: n=u8(pc+2) weight bytes @pc+3
 *                stored as u16 on the stack frame, stream byte
 *                b=stream[idx++], X=(b*sum)>>8 - w[0]; subtract w[1..]
 *                until X<0 counting pick; sel=lhu(pc+((n+4)&0x1fe)+pick*2);
 *                sel!=0 -> pc=base+sel else pc+=(3n+4)&0x7fe (absolute).
 *   tails 0x80183218/0x80183224/0x80183228 — base+sel -> ctx+0x24,
 *                v0=1, epilogue jr $ra.
 */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);

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

/* ======== scriptop_bra_subops — Bra sub-op pieces ========
 * CPS piece-group covering guest block leaders 0x80183008/0x8018304C/
 * 0x8018306C/0x8018308C/0x801830AC/0x801830CC/0x801830EC/0x8018310C and
 * their internal blocks + shared tails through the 0x8018322C epilogue.
 * Register contract (set by the 0x80182F48 prologue): s0 = ctx->pc
 * (script pc), s1 = ctx, s3 = ctrl (base = lw(s3+0x30)); sp frame with
 * s0-s3/ra saved at +0x30..+0x40 and a u16 scratch table at +0x10.
 * Faithful to generated br2eu_psx_alias_body_80182F48. */
void scriptop_bra_subops(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80183008u: goto block_80183008;
            case 0x80183018u: goto block_80183018;
            case 0x80183024u: goto block_80183024;
            case 0x80183034u: goto block_80183034;
            case 0x80183044u: goto block_80183044;
            case 0x8018304Cu: goto block_8018304C;
            case 0x8018305Cu: goto block_8018305C;
            case 0x8018306Cu: goto block_8018306C;
            case 0x8018307Cu: goto block_8018307C;
            case 0x8018308Cu: goto block_8018308C;
            case 0x8018309Cu: goto block_8018309C;
            case 0x801830ACu: goto block_801830AC;
            case 0x801830BCu: goto block_801830BC;
            case 0x801830CCu: goto block_801830CC;
            case 0x801830DCu: goto block_801830DC;
            case 0x801830ECu: goto block_801830EC;
            case 0x801830FCu: goto block_801830FC;
            case 0x8018310Cu: goto block_8018310C;
            case 0x80183124u: goto block_80183124;
            case 0x80183128u: goto block_80183128;
            case 0x80183148u: goto block_80183148;
            case 0x80183188u: goto block_80183188;
            case 0x8018318Cu: goto block_8018318C;
            case 0x801831A4u: goto block_801831A4;
            case 0x801831C8u: goto block_801831C8;
            case 0x80183218u: goto block_80183218;
            case 0x80183224u: goto block_80183224;
            case 0x80183228u: goto block_80183228;
            case 0x8018322Cu: goto block_8018322C;
            default: break;
        }
    }

/* ---- sub1 if/else: cmp>0 -> sel@pc+2 ; cmp<=0 -> clear cmp, sel@pc+6 */

block_80183008:
    v0 = cpu->read_word(s1 + 56);          /* 0x80183008 lw v0,0x38(s1) — cmp */
    /* nop */  /* 0x8018300C: 0x00000000 */
    int cond_v0_le_0 = ((int32_t)v0 <= 0);  /* 0x80183010 blez v0 */
    /* delay slot (always executes) */
    /* nop */  /* 0x80183014: 0x00000000 */
    if (cond_v0_le_0) {
        psx_check_interrupts_at(cpu, 0x80183024u);
        goto block_80183024;  /* taken: cmp<=0 -> else operand */
    } else {
        psx_check_interrupts_at(cpu, 0x80183018u);
        goto block_80183018;  /* not taken: cmp>0 -> then operand */
    }

block_80183018:
    v0 = cpu->read_half(s0 + 2);           /* 0x80183018 lhu v0,2(s0) — sel */
    /* j 0x80183034 — delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);          /* 0x80183020 sw v0,0x24(s1) */
    psx_check_interrupts_at(cpu, 0x80183034u);
    goto block_80183034;  /* j */

block_80183024:
    cpu->write_word(s1 + 56, zero);        /* 0x80183024 sw zero,0x38(s1) — cmp=0 */
    v0 = cpu->read_half(s0 + 6);           /* 0x80183028 lhu v0,6(s0) — sel */
    /* nop */  /* 0x8018302C: 0x00000000 */
    cpu->write_word(s1 + 36, v0);          /* 0x80183030 sw v0,0x24(s1) */
    /* fall through to block_80183034 */

block_80183034:
    v1 = cpu->read_word(s1 + 36);          /* 0x80183034 lw v1,0x24(s1) — sel */
    /* nop */  /* 0x80183038: 0x00000000 */
    int cond_v1_ne_zero = (v1 != zero);       /* 0x8018303C bne v1,zero */
    /* delay slot (always executes) */
    v0 = s0 + 8;                           /* 0x80183040 addiu v0,s0,8 — pc+8 */
    if (cond_v1_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183218u);
        goto block_80183218;  /* taken: sel!=0 -> base+sel */
    } else {
        psx_check_interrupts_at(cpu, 0x80183044u);
        goto block_80183044;  /* not taken: sel==0 -> pc+8 */
    }

block_80183044:
    /* j 0x80183228 — delay slot (always executes) */
    cpu->write_word(s1 + 36, v0);          /* 0x80183048 sw v0,0x24(s1) — pc+8 */
    psx_check_interrupts_at(cpu, 0x80183228u);
    goto block_80183228;  /* j */

/* ---- relational jumps on ctx+0x38: cond met -> pc+4, else base+lhu(pc+2) */

block_8018304C:
    v0 = cpu->read_word(s1 + 56);          /* 0x8018304C lw v0,0x38(s1) — cmp */
    /* nop */  /* 0x80183050: 0x00000000 */
    int cond_v0_eq_zero = (v0 == zero);       /* 0x80183054 beq v0,zero — jump if ==0 */
    /* delay slot (always executes) */
    v0 = s0 + 4;                           /* 0x80183058 addiu v0,s0,4 — pc+4 */
    if (cond_v0_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken: cond met -> pc+4 */
    } else {
        psx_check_interrupts_at(cpu, 0x8018305Cu);
        goto block_8018305C;  /* not taken -> base+lhu(pc+2) */
    }

block_8018305C:
    v0 = cpu->read_half(s0 + 2);           /* 0x8018305C lhu v0,2(s0) — target off */
    v1 = cpu->read_word(s3 + 48);          /* 0x80183060 lw v1,0x30(s3) — base */
    /* j 0x80183224 — delay slot (always executes) */
    v0 = v1 + v0;                          /* 0x80183068 addu v0,v1,v0 — base+off */
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_8018306C:
    v0 = cpu->read_word(s1 + 56);          /* 0x8018306C lw v0,0x38(s1) — cmp */
    /* nop */  /* 0x80183070: 0x00000000 */
    int cond_v0_ne_zero = (v0 != zero);       /* 0x80183074 bne v0,zero — jump if !=0 */
    /* delay slot (always executes) */
    v0 = s0 + 4;                           /* 0x80183078 addiu v0,s0,4 — pc+4 */
    if (cond_v0_ne_zero) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken: cond met -> pc+4 */
    } else {
        psx_check_interrupts_at(cpu, 0x8018307Cu);
        goto block_8018307C;  /* not taken -> base+lhu(pc+2) */
    }

block_8018307C:
    v0 = cpu->read_half(s0 + 2);           /* 0x8018307C lhu v0,2(s0) — target off */
    v1 = cpu->read_word(s3 + 48);          /* 0x80183080 lw v1,0x30(s3) — base */
    /* j 0x80183224 — delay slot (always executes) */
    v0 = v1 + v0;                          /* 0x80183088 addu v0,v1,v0 — base+off */
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_8018308C:
    v0 = cpu->read_word(s1 + 56);          /* 0x8018308C lw v0,0x38(s1) — cmp */
    /* nop */  /* 0x80183090: 0x00000000 */
    int cond_v0_le_0_2 = ((int32_t)v0 <= 0);  /* 0x80183094 blez v0 — jump if <=0 */
    /* delay slot (always executes) */
    v0 = s0 + 4;                           /* 0x80183098 addiu v0,s0,4 — pc+4 */
    if (cond_v0_le_0_2) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken: cond met -> pc+4 */
    } else {
        psx_check_interrupts_at(cpu, 0x8018309Cu);
        goto block_8018309C;  /* not taken -> base+lhu(pc+2) */
    }

block_8018309C:
    v0 = cpu->read_half(s0 + 2);           /* 0x8018309C lhu v0,2(s0) — target off */
    v1 = cpu->read_word(s3 + 48);          /* 0x801830A0 lw v1,0x30(s3) — base */
    /* j 0x80183224 — delay slot (always executes) */
    v0 = v1 + v0;                          /* 0x801830A8 addu v0,v1,v0 — base+off */
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_801830AC:
    v0 = cpu->read_word(s1 + 56);          /* 0x801830AC lw v0,0x38(s1) — cmp */
    /* nop */  /* 0x801830B0: 0x00000000 */
    int cond_v0_lt_0 = ((int32_t)v0 < 0);   /* 0x801830B4 bltz v0 — jump if <0 */
    /* delay slot (always executes) */
    v0 = s0 + 4;                           /* 0x801830B8 addiu v0,s0,4 — pc+4 */
    if (cond_v0_lt_0) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken: cond met -> pc+4 */
    } else {
        psx_check_interrupts_at(cpu, 0x801830BCu);
        goto block_801830BC;  /* not taken -> base+lhu(pc+2) */
    }

block_801830BC:
    v0 = cpu->read_half(s0 + 2);           /* 0x801830BC lhu v0,2(s0) — target off */
    v1 = cpu->read_word(s3 + 48);          /* 0x801830C0 lw v1,0x30(s3) — base */
    /* j 0x80183224 — delay slot (always executes) */
    v0 = v1 + v0;                          /* 0x801830C8 addu v0,v1,v0 — base+off */
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_801830CC:
    v0 = cpu->read_word(s1 + 56);          /* 0x801830CC lw v0,0x38(s1) — cmp */
    /* nop */  /* 0x801830D0: 0x00000000 */
    int cond_v0_ge_0 = ((int32_t)v0 >= 0);  /* 0x801830D4 bgez v0 — jump if >=0 */
    /* delay slot (always executes) */
    v0 = s0 + 4;                           /* 0x801830D8 addiu v0,s0,4 — pc+4 */
    if (cond_v0_ge_0) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken: cond met -> pc+4 */
    } else {
        psx_check_interrupts_at(cpu, 0x801830DCu);
        goto block_801830DC;  /* not taken -> base+lhu(pc+2) */
    }

block_801830DC:
    v0 = cpu->read_half(s0 + 2);           /* 0x801830DC lhu v0,2(s0) — target off */
    v1 = cpu->read_word(s3 + 48);          /* 0x801830E0 lw v1,0x30(s3) — base */
    /* j 0x80183224 — delay slot (always executes) */
    v0 = v1 + v0;                          /* 0x801830E8 addu v0,v1,v0 — base+off */
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

block_801830EC:
    v0 = cpu->read_word(s1 + 56);          /* 0x801830EC lw v0,0x38(s1) — cmp */
    /* nop */  /* 0x801830F0: 0x00000000 */
    int cond_v0_gt_0 = ((int32_t)v0 > 0);   /* 0x801830F4 bgtz v0 — jump if >0 */
    /* delay slot (always executes) */
    v0 = s0 + 4;                           /* 0x801830F8 addiu v0,s0,4 — pc+4 */
    if (cond_v0_gt_0) {
        psx_check_interrupts_at(cpu, 0x80183224u);
        goto block_80183224;  /* taken: cond met -> pc+4 */
    } else {
        psx_check_interrupts_at(cpu, 0x801830FCu);
        goto block_801830FC;  /* not taken -> base+lhu(pc+2) */
    }

block_801830FC:
    v0 = cpu->read_half(s0 + 2);           /* 0x801830FC lhu v0,2(s0) — target off */
    v1 = cpu->read_word(s3 + 48);          /* 0x80183100 lw v1,0x30(s3) — base */
    /* j 0x80183224 — delay slot (always executes) */
    v0 = v1 + v0;                          /* 0x80183108 addu v0,v1,v0 — base+off */
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

/* ---- weighted-random pick (sub10): n weight bytes, stream-byte roll ---- */

block_8018310C:
    a1 = s0 + 3;                           /* 0x8018310C addiu a1,s0,3 — weight bytes */
    a2 = zero;                             /* 0x80183110 move a2,zero — sum = 0 */
    a3 = cpu->read_byte(s0 + 2);           /* 0x80183114 lbu a3,2(s0) — n */
    /* nop */  /* 0x80183118: 0x00000000 */
    int cond_a3_eq_zero = (a3 == zero);       /* 0x8018311C beq a3,zero — n==0 skip */
    /* delay slot (always executes) */
    v1 = a2;                               /* 0x80183120 move v1,a2 — i = 0 */
    if (cond_a3_eq_zero) {
        psx_check_interrupts_at(cpu, 0x80183148u);
        goto block_80183148;  /* taken: n==0 -> skip weight table build */
    } else {
        psx_check_interrupts_at(cpu, 0x80183124u);
        goto block_80183124;  /* not taken -> build weight table */
    }

block_80183124:
    a0 = sp + 16;                          /* 0x80183124 addiu a0,sp,16 — u16 table */
    /* fall through to block_80183128 */

block_80183128:
    v0 = cpu->read_byte(a1);               /* 0x80183128 lbu v0,0(a1) — weight byte */
    a1 = a1 + 1;                           /* 0x8018312C addiu a1,a1,1 */
    v1 = v1 + 1;                           /* 0x80183130 addiu v1,v1,1 — i++ */
    a2 = v0 + a2;                          /* 0x80183134 addu a2,v0,a2 — sum += w */
    cpu->write_half(a0, (uint16_t)v0);     /* 0x80183138 sh v0,0(a0) — table[i] */
    v0 = ((int32_t)v1 < (int32_t)a3) ? 1 : 0;  /* 0x8018313C slt v0,v1,a3 — i<n */
    /* delay slot (always executes) */
    a0 = a0 + 2;                           /* 0x80183144 addiu a0,a0,2 */
    if (v0 != zero) {
        psx_check_interrupts_at(cpu, 0x80183128u);
        goto block_80183128;  /* taken: loop */
    } else {
        psx_check_interrupts_at(cpu, 0x80183148u);
        goto block_80183148;  /* not taken: table built */
    }

block_80183148:
    v1 = cpu->read_byte(s1 + 4);           /* 0x80183148 lbu v1,4(s1) — stream idx */
    /* nop */  /* 0x8018314C: 0x00000000 */
    v0 = v1 + 1;                           /* 0x80183150 addiu v0,v1,1 */
    cpu->write_byte(s1 + 4, (uint8_t)v0);  /* 0x80183154 sb v0,4(s1) — idx++ */
    v0 = cpu->read_word(s1);               /* 0x80183158 lw v0,0(s1) — stream base */
    v1 = v1 & 0x00FF;                      /* 0x8018315C andi v1,v1,0xff — idx&0xff */
    v0 = v0 + v1;                          /* 0x80183160 addu v0,v0,v1 — base+idx */
    v0 = cpu->read_byte(v0);               /* 0x80183164 lbu v0,0(v0) — stream byte */
    /* nop */  /* 0x80183168: 0x00000000 */
    { int64_t result = (int64_t)(int32_t)v0 * (int64_t)(int32_t)a2; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
    psx_muldiv_set(cpu, psx_mult_latency_s(v0));  /* 0x8018316C mult v0,a2 */
    v0 = cpu->read_half(sp + 16);          /* 0x80183170 lhu v0,16(sp) — w[0] */
    t0 = cpu->lo;
    psx_muldiv_stall(cpu);                 /* 0x80183174 mflo t0 */
    a0 = (int32_t)t0 >> 8;                 /* 0x80183178 sra a0,t0,8 — (b*sum)>>8 */
    a0 = a0 - v0;                          /* 0x8018317C subu a0,a0,v0 — X -= w[0] */
    int cond_a0_lt_0 = ((int32_t)a0 < 0);   /* 0x80183180 bltz a0 — X<0 done */
    /* delay slot (always executes) */
    v1 = zero;                             /* 0x80183184 move v1,zero — pick = 0 */
    if (cond_a0_lt_0) {
        psx_check_interrupts_at(cpu, 0x801831A4u);
        goto block_801831A4;  /* taken: X<0 -> pick stays 0 */
    } else {
        psx_check_interrupts_at(cpu, 0x80183188u);
        goto block_80183188;  /* not taken -> subtract loop */
    }

block_80183188:
    a1 = sp + 16;                          /* 0x80183188 addiu a1,sp,16 — &w[0] */
    /* fall through to block_8018318C */

block_8018318C:
    a1 = a1 + 2;                           /* 0x8018318C addiu a1,a1,2 — next weight */
    v0 = cpu->read_half(a1);               /* 0x80183190 lhu v0,0(a1) — w[i] */
    /* nop */  /* 0x80183194: 0x00000000 */
    a0 = a0 - v0;                          /* 0x80183198 subu a0,a0,v0 — X -= w[i] */
    int cond_a0_ge_0 = ((int32_t)a0 >= 0);  /* 0x8018319C bgez a0 — keep going */
    /* delay slot (always executes) */
    v1 = v1 + 1;                           /* 0x801831A0 addiu v1,v1,1 — pick++ */
    if (cond_a0_ge_0) {
        psx_check_interrupts_at(cpu, 0x8018318Cu);
        goto block_8018318C;  /* taken: loop */
    } else {
        psx_check_interrupts_at(cpu, 0x801831A4u);
        goto block_801831A4;  /* not taken: X<0 -> pick final */
    }

block_801831A4:
    v0 = a3 + 4;                           /* 0x801831A4 addiu v0,a3,4 — n+4 */
    v1 = v1 << 1;                          /* 0x801831A8 sll v1,v1,1 — pick*2 */
    v0 = v0 & 0x01FE;                      /* 0x801831AC andi v0,v0,0x1fe */
    v0 = v0 + s0;                          /* 0x801831B0 addu v0,v0,s0 — pc+off */
    v1 = v1 + v0;                          /* 0x801831B4 addu v1,v1,v0 — +pick*2 */
    v1 = cpu->read_half(v1);               /* 0x801831B8 lhu v1,0(v1) — sel */
    /* nop */  /* 0x801831BC: 0x00000000 */
    int cond_v1_ne_zero_2 = (v1 != zero);       /* 0x801831C0 bne v1,zero */
    /* delay slot (always executes) */
    cpu->write_word(s1 + 36, v1);          /* 0x801831C4 sw v1,0x24(s1) — sel */
    if (cond_v1_ne_zero_2) {
        psx_check_interrupts_at(cpu, 0x80183218u);
        goto block_80183218;  /* taken: sel!=0 -> base+sel */
    } else {
        psx_check_interrupts_at(cpu, 0x801831C8u);
        goto block_801831C8;  /* not taken: sel==0 -> pc+((3n+4)&0x7fe) */
    }

block_801831C8:
    v0 = a3 << 1;                          /* 0x801831C8 sll v0,a3,1 — 2n */
    v0 = v0 + a3;                          /* 0x801831CC addu v0,v0,a3 — 3n */
    v0 = v0 + 4;                           /* 0x801831D0 addiu v0,v0,4 */
    v0 = v0 & 0x07FE;                      /* 0x801831D4 andi v0,v0,0x7fe */
    /* j 0x80183224 — delay slot (always executes) */
    v0 = s0 + v0;                          /* 0x801831DC addu v0,s0,v0 — pc+off */
    psx_check_interrupts_at(cpu, 0x80183224u);
    goto block_80183224;  /* j */

/* ---- shared tails ---- */

block_80183218:
    v0 = cpu->read_word(s3 + 48);          /* 0x80183218 lw v0,0x30(s3) — base */
    /* nop */  /* 0x8018321C: 0x00000000 */
    v0 = v1 + v0;                          /* 0x80183220 addu v0,v1,v0 — base+sel */
    /* fall through to block_80183224 */

block_80183224:
    cpu->write_word(s1 + 36, v0);          /* 0x80183224 sw v0,0x24(s1) — next pc */
    /* fall through to block_80183228 */

block_80183228:
    v0 = 1;                                /* 0x80183228 li v0,1 — continue */
    /* fall through to block_8018322C */

block_8018322C:
    ra = cpu->read_word(sp + 64);          /* 0x8018322C lw ra,0x40(sp) */
    s3 = cpu->read_word(sp + 60);          /* 0x80183230 lw s3,0x3c(sp) */
    s2 = cpu->read_word(sp + 56);          /* 0x80183234 lw s2,0x38(sp) */
    s1 = cpu->read_word(sp + 52);          /* 0x80183238 lw s1,0x34(sp) */
    s0 = cpu->read_word(sp + 48);          /* 0x8018323C lw s0,0x30(sp) */
    uint32_t jr_target = ra;  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
    sp = sp + 72;                          /* 0x80183244 addiu sp,sp,0x48 */
    psx_check_interrupts_at(cpu, jr_target);
    cpu->pc = jr_target; return;  /* CPS: jr $ra */
}
