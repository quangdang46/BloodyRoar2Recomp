/* w3_e07.c — clean CPS ports for guest piece-entries
 * 0x80160008-0x80160514 (SLES_017.22_full_24.c) and
 * 0x80163744-0x80163780 (SLES_017.22_full_25.c).
 *
 * All eleven addrs are CPS piece/jump-table-case entries inside two large
 * coroutine bodies, NOT standalone functions:
 *
 *   - 0x8015F6A0 body (memcard_file_flow, clean-ported in game/save.c):
 *     the 0x80160008/24/40 entries are cases of the `jr $v0` dispatch at
 *     0x8015FFE4 (table 0x80121B40, s0 < 5) — each loads a different
 *     "SAVE/FORMAT/CHECKING MEMORY CARD %s." format string and joins the
 *     shared sprintf_core_guest (0x8018EF60) call at 0x80160058.
 *     0x80160184 is a stat-byte percent probe (magic /10) ending in
 *     `jal 0x80156048`. 0x80160220/0x80160298/0x80160410 are classifier
 *     pieces that all merge on the shared poll node 0x80160514, which
 *     itself ends in `jal 0x8012C374` / `jal 0x8012AEEC`.
 *   - 0x80163634 body (mode_name_announce, clean-ported in game/menu.c):
 *     0x80163744/58/80 are cases of the `jr $v0` dispatch at 0x8016373C
 *     (table 0x80121EC4, lbu 0x92(t0) < 0x11) selecting mode-name strings
 *     ("V.S./WATCH/PRACTICE/CUSTOM MODE") and joining the shared
 *     announce call `jal 0x8012E078` at 0x80163820.
 *
 * Each body below is a faithful op-by-op transcription of the generated
 * br2eu_func_* / br2eu_psx_alias_body_* piece: same guest ops, same order,
 * same delay slots, same yield/continuation points (psx_check_interrupts_at
 * + cpu->pc CPS dispatch), minus cycle/icache/cosim/debug instrumentation.
 * Continuations not covered here stay generated-authoritative.
 */
#include <stdint.h>
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

/* ======== 0x80160008 — memcard_op_fmt_save ========
 * Jump-table case (table 0x80121B40 idx 1/3) of the memcard_file_flow
 * `jr $v0` dispatch at 0x8015FFE4: sprintf(sp+0x80, "SAVE TO MEMORY CARD
 * %s.", table_entry) then joins the shared jal sprintf_core_guest
 * (0x8018EF60) at 0x80160058. Faithful to br2eu_func_80160008. */
void memcard_op_fmt_save(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_80160008:
  a0 = sp + 128;
  v0 = s3 << 2;
  v0 = v0 + s7;
  a2 = cpu->read_word(v0);
  a1 = 0x8012 << 16;  /* lui a1,0x8012 */
  /* j 0x80160058 — delay slot */
  a1 = a1 + 6408;  /* 0x80160020: addiu a1,a1,0x1908 -> 0x80121908 "SAVE TO MEMORY CARD %s." */
  psx_check_interrupts_at(cpu, 0x80160058u);

block_80160058:
  ra = 0x80160060u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  /* nop */  /* 0x8016005C: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x8018EF60u);
  cpu->pc = 0x8018EF60u; return;  /* CPS jal -> sprintf_core_guest */
}

/* ======== 0x80160024 — memcard_op_fmt_format ========
 * Same dispatch case shape as 0x80160008 with format string
 * 0x80121920 "FORMAT MEMORY CARD %s.". Faithful to
 * br2eu_func_80160024. */
void memcard_op_fmt_format(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_80160024:
  a0 = sp + 128;
  v0 = s3 << 2;
  v0 = v0 + s7;
  a2 = cpu->read_word(v0);
  a1 = 0x8012 << 16;  /* lui a1,0x8012 */
  /* j 0x80160058 — delay slot */
  a1 = a1 + 6432;  /* 0x8016003C: addiu a1,a1,0x1920 -> 0x80121920 "FORMAT MEMORY CARD %s." */
  psx_check_interrupts_at(cpu, 0x80160058u);

block_80160058:
  ra = 0x80160060u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  /* nop */  /* 0x8016005C: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x8018EF60u);
  cpu->pc = 0x8018EF60u; return;  /* CPS jal -> sprintf_core_guest */
}

/* ======== 0x80160040 — memcard_op_fmt_check ========
 * Same dispatch case shape, format string 0x80121938 "CHECKING MEMORY
 * CARD %s."; falls through into the shared jal block (no j). Faithful
 * to br2eu_func_80160040. */
void memcard_op_fmt_check(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_80160040:
  a0 = sp + 128;
  v0 = s3 << 2;
  v0 = v0 + s7;
  a2 = cpu->read_word(v0);
  a1 = 0x8012 << 16;  /* lui a1,0x8012 */
  a1 = a1 + 6456;  /* 0x80160054: addiu a1,a1,0x1938 -> 0x80121938 "CHECKING MEMORY CARD %s." */
  /* fall through to block_80160058 */

block_80160058:
  ra = 0x80160060u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  /* nop */  /* 0x8016005C: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x8018EF60u);
  cpu->pc = 0x8018EF60u; return;  /* CPS jal -> sprintf_core_guest */
}

/* ======== 0x80160184 — memcard_stat_pct_call ========
 * Reads the stat sub-record at s1+0xc0, extracts bytes +0xe/+0xf, computes
 * (byte*8 * 0x66666667)>>32-based percent via mult/mfhi, then
 * `jal 0x80156048`. Faithful to br2eu_func_80160184. */
void memcard_stat_pct_call(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_80160184:
  v0 = cpu->read_word(s1 + 192);          /* 0x80160184: lw v0,0xc0(s1) */
  v1 = 0x6666 << 16;                      /* 0x80160188: lui v1,0x6666 */
  a1 = cpu->read_byte(v0 + 15);           /* 0x8016018C: lbu a1,0xf(v0) */
  v1 = v1 | 0x6667;                       /* 0x80160190: ori v1,v1,0x6667 */
  a1 = a1 << 3;                           /* 0x80160194: sll a1,a1,3 */
  { int64_t result = (int64_t)(int32_t)a1 * (int64_t)(int32_t)v1;
    cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
  psx_muldiv_set(cpu, psx_mult_latency_s(a1));  /* 0x80160198: mult a1,v1 */
  a0 = cpu->read_byte(v0 + 14);           /* 0x8016019C: lbu a0,0xe(v0) */
  a1 = (int32_t)a1 >> 31;                 /* 0x801601A0: sra a1,a1,31 */
  t0 = cpu->hi;                           /* 0x801601A4: mfhi t0 */
  psx_muldiv_stall(cpu);
  v0 = (int32_t)t0 >> 2;                  /* 0x801601A8: sra v0,t0,2 */
  ra = 0x801601B4u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  a1 = v0 - a1;                           /* 0x801601B0: subu a1,v0,a1 */
  psx_check_interrupts_at(cpu, 0x80156048u);
  cpu->pc = 0x80156048u; return;  /* CPS jal -> 0x80156048 */
}

/* ======== 0x80160220 — memcard_flow_triage ========
 * Classifier piece of memcard_file_flow: reads the sp+0xf8 flag word and
 * the sp+0x40 halfword, walks the compare ladder, sets s0/s3/s4, then
 * merges on the shared poll node 0x80160514. Faithful to
 * br2eu_func_80160220. */
void memcard_flow_triage(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  int cond;

block_80160220:
  t3 = cpu->read_word(sp + 248);          /* 0x80160220: lw t3,0xf8(sp) */
  /* nop */  /* 0x80160224: 0x00000000 */
  cond = (t3 != zero);       /* bnez t3 */
  /* delay slot (always executes) */
  s4 = 0xFFFFFFFF;                        /* 0x8016022C: addiu s4,zero,-1 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x8016026Cu);
    goto block_8016026C;
  } else {
    psx_check_interrupts_at(cpu, 0x80160230u);
  }

block_80160230:
  v1 = cpu->read_half(sp + 64);           /* 0x80160230: lhu v1,0x40(sp) */
  /* nop */  /* 0x80160234: 0x00000000 */
  cond = (v1 == fp);          /* beq v1,fp */
  /* delay slot (always executes) */
  v0 = ((int32_t)v1 < 3) ? 1 : 0;         /* 0x8016023C: slti v0,v1,3 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160278u);
    goto block_80160278;
  } else {
    psx_check_interrupts_at(cpu, 0x80160240u);
  }

block_80160240:
  cond = (v0 == zero);        /* beqz v0 */
  /* delay slot (always executes) */
  t0 = 1;                                 /* 0x80160244: addiu t0,zero,1 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160258u);
    goto block_80160258;
  } else {
    psx_check_interrupts_at(cpu, 0x80160248u);
  }

block_80160248:
  cond = (v1 == t0);          /* beq v1,t0 */
  /* delay slot (always executes) */
  t1 = 17;                                /* 0x8016024C: addiu t1,zero,0x11 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x8016026Cu);
    goto block_8016026C;
  } else {
    psx_check_interrupts_at(cpu, 0x80160250u);
  }

block_80160250:
  /* j 0x80160514 — delay slot */
  cpu->write_byte(sp + 0x3b, (uint8_t)t1);  /* 0x80160254: sb t1,0x3b(sp) */
  psx_check_interrupts_at(cpu, 0x80160514u);
  cpu->pc = 0x80160514u; return;  /* CPS j: split */

block_80160258:
  v0 = 3;                                 /* 0x80160258: addiu v0,zero,3 */
  cond = (v1 == v0);          /* beq v1,v0 */
  /* delay slot (always executes) */
  t1 = 17;                                /* 0x80160260: addiu t1,zero,0x11 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160284u);
    goto block_80160284;
  } else {
    psx_check_interrupts_at(cpu, 0x80160264u);
  }

block_80160264:
  /* j 0x80160514 — delay slot */
  cpu->write_byte(sp + 0x3b, (uint8_t)t1);  /* 0x80160268: sb t1,0x3b(sp) */
  psx_check_interrupts_at(cpu, 0x80160514u);
  cpu->pc = 0x80160514u; return;  /* CPS j: split */

block_8016026C:
  s0 = 1;                                 /* 0x8016026C: addiu s0,zero,1 */
  /* j 0x8016028C — delay slot */
  s3 = zero;                              /* 0x80160274: move s3,zero */
  psx_check_interrupts_at(cpu, 0x8016028Cu);
  goto block_8016028C;

block_80160278:
  s3 = (int32_t)(int16_t)cpu->read_half(sp + 0x4e);  /* 0x80160278: lh s3,0x4e(sp) */
  /* j 0x8016028C — delay slot */
  s0 = zero;                              /* 0x80160280: move s0,zero */
  psx_check_interrupts_at(cpu, 0x8016028Cu);
  goto block_8016028C;

block_80160284:
  s0 = 1;                                 /* 0x80160284: addiu s0,zero,1 */
  s3 = (int32_t)(int16_t)cpu->read_half(sp + 0x50);  /* 0x80160288: lh s3,0x50(sp) */
  /* fall through to block_8016028C */

block_8016028C:
  t1 = 17;                                /* 0x8016028C: addiu t1,zero,0x11 */
  /* j 0x80160514 — delay slot */
  cpu->write_byte(sp + 0x3b, (uint8_t)t1);  /* 0x80160294: sb t1,0x3b(sp) */
  psx_check_interrupts_at(cpu, 0x80160514u);
  cpu->pc = 0x80160514u; return;  /* CPS j: split */
}

/* ======== 0x80160298 — memcard_flow_arm_flag ========
 * Arms the op flag (s4=1, s0=s4) then joins block_80160464 which stores
 * 0x11 to sp+0x3b and merges on 0x80160514. Faithful to
 * br2eu_func_80160298. */
void memcard_flow_arm_flag(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_80160298:
  s4 = 1;                                 /* 0x80160298: addiu s4,zero,1 */
  /* j 0x80160464 — delay slot */
  s0 = s4;                                /* 0x801602A0: move s0,s4 */
  psx_check_interrupts_at(cpu, 0x80160464u);

block_80160464:
  t2 = 17;                                /* 0x80160464: addiu t2,zero,0x11 */
  /* j 0x80160514 — delay slot */
  cpu->write_byte(sp + 0x3b, (uint8_t)t2);  /* 0x8016046C: sb t2,0x3b(sp) */
  psx_check_interrupts_at(cpu, 0x80160514u);
  cpu->pc = 0x80160514u; return;  /* CPS j: split */
}

/* ======== 0x80160410 — memcard_flow_state_dispatch ========
 * Largest classifier piece of memcard_file_flow: walks the s0/s2 compare
 * ladder over the sp+0xf8 flag and sp+0x40 halfword, then either stores
 * the op code and merges on 0x80160514, bails to 0x8015F8C0, or clears
 * sp+0x3b / the s1+0xc0 sub-record byte +0x22. Faithful to
 * br2eu_func_80160410. */
void memcard_flow_state_dispatch(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  int cond;

block_80160410:
  cond = (s0 == zero);        /* beqz s0 */
  /* delay slot (always executes) */
  v0 = 5;                                 /* 0x80160414: addiu v0,zero,5 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x801604B4u);
    goto block_801604B4;
  } else {
    psx_check_interrupts_at(cpu, 0x80160418u);
  }

block_80160418:
  cond = (s2 == v0);          /* beq s2,v0 */
  /* delay slot (always executes) */
  v0 = ((int32_t)s2 < 6) ? 1 : 0;         /* 0x8016041C: slti v0,s2,6 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160494u);
    goto block_80160494;
  } else {
    psx_check_interrupts_at(cpu, 0x80160420u);
  }

block_80160420:
  cond = (v0 == zero);        /* beqz v0 */
  /* delay slot (always executes) */
  v0 = ((int32_t)s2 < 4) ? 1 : 0;         /* 0x80160424: slti v0,s2,4 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160448u);
    goto block_80160448;
  } else {
    psx_check_interrupts_at(cpu, 0x80160428u);
  }

block_80160428:
  cond = (v0 == zero);        /* beqz v0 */
  /* delay slot (always executes) */
  /* nop */  /* 0x8016042C: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x8016045Cu);
    goto block_8016045C;
  } else {
    psx_check_interrupts_at(cpu, 0x80160430u);
  }

block_80160430:
  cond = ((int32_t)s2 > 0);   /* bgtz s2 */
  /* delay slot (always executes) */
  /* nop */  /* 0x80160434: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160470u);
    goto block_80160470;
  } else {
    psx_check_interrupts_at(cpu, 0x80160438u);
  }

block_80160438:
  cond = (s2 == zero);        /* beqz s2 */
  /* delay slot (always executes) */
  /* nop */  /* 0x8016043C: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160494u);
    goto block_80160494;
  } else {
    psx_check_interrupts_at(cpu, 0x80160440u);
  }

block_80160440:
  /* j 0x80160470 — delay slot */
  /* nop */  /* 0x80160444: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x80160470u);
  goto block_80160470;

block_80160448:
  v0 = 15;                                /* 0x80160448: addiu v0,zero,0xf */
  cond = (s2 == v0);          /* beq s2,v0 */
  /* delay slot (always executes) */
  /* nop */  /* 0x80160450: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160494u);
    goto block_80160494;
  } else {
    psx_check_interrupts_at(cpu, 0x80160454u);
  }

block_80160454:
  /* j 0x80160470 — delay slot */
  /* nop */  /* 0x80160458: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x80160470u);
  goto block_80160470;

block_8016045C:
  s4 = 1;                                 /* 0x8016045C: addiu s4,zero,1 */
  s0 = 2;                                 /* 0x80160460: addiu s0,zero,2 */
  /* fall through to block_80160464 */

block_80160464:
  t2 = 17;                                /* 0x80160464: addiu t2,zero,0x11 */
  /* j 0x80160514 — delay slot */
  cpu->write_byte(sp + 0x3b, (uint8_t)t2);  /* 0x8016046C: sb t2,0x3b(sp) */
  psx_check_interrupts_at(cpu, 0x80160514u);
  cpu->pc = 0x80160514u; return;  /* CPS j: split */

block_80160470:
  t3 = cpu->read_word(sp + 248);          /* 0x80160470: lw t3,0xf8(sp) */
  /* nop */  /* 0x80160474: 0x00000000 */
  cond = (t3 == zero);        /* beqz t3 */
  /* delay slot (always executes) */
  /* nop */  /* 0x8016047C: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x801604A4u);
    goto block_801604A4;
  } else {
    psx_check_interrupts_at(cpu, 0x80160480u);
  }

block_80160480:
  s4 = 1;                                 /* 0x80160480: addiu s4,zero,1 */
  s0 = s4;                                /* 0x80160484: move s0,s4 */
  /* fall through to block_80160488 */

block_80160488:
  t0 = 17;                                /* 0x80160488: addiu t0,zero,0x11 */
  /* j 0x80160514 — delay slot */
  cpu->write_byte(sp + 0x3b, (uint8_t)t0);  /* 0x80160490: sb t0,0x3b(sp) */
  psx_check_interrupts_at(cpu, 0x80160514u);
  cpu->pc = 0x80160514u; return;  /* CPS j: split */

block_80160494:
  t1 = cpu->read_word(sp + 248);          /* 0x80160494: lw t1,0xf8(sp) */
  /* nop */  /* 0x80160498: 0x00000000 */
  cond = (t1 != zero);        /* bnez t1 */
  /* delay slot (always executes) */
  /* nop */  /* 0x801604A0: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x8015F8C0u);
    cpu->pc = 0x8015F8C0u; return;  /* CPS taken: split */
  } else {
    psx_check_interrupts_at(cpu, 0x801604A4u);
  }

block_801604A4:
  cond = (s2 == zero);        /* beqz s2 */
  /* delay slot (always executes) */
  /* nop */  /* 0x801604A8: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160510u);
    goto block_80160510;
  } else {
    psx_check_interrupts_at(cpu, 0x801604ACu);
  }

block_801604AC:
  /* j 0x801604F4 — delay slot */
  /* nop */  /* 0x801604B0: 0x00000000 */
  psx_check_interrupts_at(cpu, 0x801604F4u);
  goto block_801604F4;

block_801604B4:
  t3 = cpu->read_word(sp + 248);          /* 0x801604B4: lw t3,0xf8(sp) */
  /* nop */  /* 0x801604B8: 0x00000000 */
  cond = (t3 != zero);        /* bnez t3 */
  /* delay slot (always executes) */
  /* nop */  /* 0x801604C0: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x8015F8C0u);
    cpu->pc = 0x8015F8C0u; return;  /* CPS taken: split */
  } else {
    psx_check_interrupts_at(cpu, 0x801604C4u);
  }

block_801604C4:
  cond = (s2 == zero);        /* beqz s2 */
  /* delay slot (always executes) */
  t0 = 1;                                 /* 0x801604C8: addiu t0,zero,1 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160510u);
    goto block_80160510;
  } else {
    psx_check_interrupts_at(cpu, 0x801604CCu);
  }

block_801604CC:
  v0 = cpu->read_half(sp + 64);           /* 0x801604CC: lhu v0,0x40(sp) */
  /* nop */  /* 0x801604D0: 0x00000000 */
  cond = (v0 == t0);          /* beq v0,t0 */
  /* delay slot (always executes) */
  /* nop */  /* 0x801604D8: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160504u);
    goto block_80160504;
  } else {
    psx_check_interrupts_at(cpu, 0x801604DCu);
  }

block_801604DC:
  /* j 0x80160514 — delay slot */
  cpu->write_byte(sp + 0x3b, (uint8_t)zero);  /* 0x801604E0: sb zero,0x3b(sp) */
  psx_check_interrupts_at(cpu, 0x80160514u);
  cpu->pc = 0x80160514u; return;  /* CPS j: split */

block_801604F4:
  v0 = cpu->read_half(sp + 64);           /* 0x801604F4: lhu v0,0x40(sp) */
  t2 = 1;                                 /* 0x801604F8: addiu t2,zero,1 */
  cond = (v0 != t2);          /* bne v0,t2 */
  /* delay slot (always executes) */
  /* nop */  /* 0x80160500: 0x00000000 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160510u);
    goto block_80160510;
  } else {
    psx_check_interrupts_at(cpu, 0x80160504u);
  }

block_80160504:
  v0 = cpu->read_word(s1 + 192);          /* 0x80160504: lw v0,0xc0(s1) */
  /* nop */  /* 0x80160508: 0x00000000 */
  cpu->write_byte(v0 + 0x22, (uint8_t)zero);  /* 0x8016050C: sb zero,0x22(v0) */
  /* fall through to block_80160510 */

block_80160510:
  cpu->write_byte(sp + 0x3b, (uint8_t)zero);  /* 0x80160510: sb zero,0x3b(sp) */
  /* fall through to block_80160514 — split piece exit */
  cpu->pc = 0x80160514u; return;
}

/* ======== 0x80160514 — memcard_flow_poll_node ========
 * The shared poll/dispatch node every classifier piece merges on: reads
 * the sp+0xf8 flag and either `jal 0x8012C374` (flag clear) or
 * `jal 0x8012AEEC` (flag set). Faithful to br2eu_func_80160514. */
void memcard_flow_poll_node(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  int cond;

block_80160514:
  t3 = cpu->read_word(sp + 248);          /* 0x80160514: lw t3,0xf8(sp) */
  /* nop */  /* 0x80160518: 0x00000000 */
  cond = (t3 != zero);        /* bnez t3 */
  /* delay slot (always executes) */
  a0 = s1;                                /* 0x80160520: move a0,s1 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x80160550u);
    goto block_80160550;
  } else {
    psx_check_interrupts_at(cpu, 0x80160524u);
  }

block_80160524:
  a1 = cpu->read_word(s1 + 8);            /* 0x80160524: lw a1,8(s1) */
  ra = 0x80160530u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  a2 = 7;                                 /* 0x8016052C: addiu a2,zero,7 */
  psx_check_interrupts_at(cpu, 0x8012C374u);
  cpu->pc = 0x8012C374u; return;  /* CPS jal -> 0x8012C374 */

block_80160550:
  ra = 0x80160558u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  a0 = s1;                                /* 0x80160554: move a0,s1 */
  psx_check_interrupts_at(cpu, 0x8012AEECu);
  cpu->pc = 0x8012AEECu; return;  /* CPS jal -> 0x8012AEEC */
}

/* ======== 0x80163744 — mode_name_case_vs ========
 * Jump-table case (table 0x80121EC4) of mode_name_announce's `jr $v0`
 * dispatch at 0x8016373C: selects the "V.S. MODE" announce string
 * (0x80121E54), then joins the shared announce tail (0x80163814/0x80163820)
 * ending in `jal 0x8012E078`. Faithful to br2eu_func_80163744. */
void mode_name_case_vs(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_80163744:
  a2 = 0x8012 << 16;                      /* 0x80163744: lui a2,0x8012 */
  a2 = a2 + 7764;                         /* 0x80163748: addiu a2,a2,0x1e54 -> 0x80121E54 "V.S. MODE" */
  v0 = 4;                                 /* 0x8016374C: addiu v0,zero,4 */
  /* j 0x80163814 — delay slot */
  a3 = a3 - 27;                           /* 0x80163754: addiu a3,a3,-0x1b */
  psx_check_interrupts_at(cpu, 0x80163814u);

block_80163814:
  a3 = a3 << 16;                          /* 0x80163814: sll a3,a3,0x10 */
  cpu->write_word(sp + 16, a0);           /* 0x80163818: sw a0,0x10(sp) */
  cpu->write_word(sp + 20, v0);           /* 0x8016381C: sw v0,0x14(sp) */
  /* fall through to block_80163820 */

block_80163820:
  a0 = cpu->read_word(t0 + 8);            /* 0x80163820: lw a0,8(t0) */
  a1 = cpu->read_word(t0 + 12);           /* 0x80163824: lw a1,0xc(t0) */
  ra = 0x80163830u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  a3 = (int32_t)a3 >> 16;                 /* 0x8016382C: sra a3,a3,0x10 */
  psx_check_interrupts_at(cpu, 0x8012E078u);
  cpu->pc = 0x8012E078u; return;  /* CPS jal -> 0x8012E078 */
}

/* ======== 0x80163758 — mode_name_case_watch ========
 * Same dispatch case shape with the "WATCH MODE" string (0x80121E60).
 * Faithful to br2eu_func_80163758. */
void mode_name_case_watch(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;

block_80163758:
  a2 = 0x8012 << 16;                      /* 0x80163758: lui a2,0x8012 */
  a2 = a2 + 7776;                         /* 0x8016375C: addiu a2,a2,0x1e60 -> 0x80121E60 "WATCH MODE" */
  v0 = 4;                                 /* 0x80163760: addiu v0,zero,4 */
  /* j 0x80163814 — delay slot */
  a3 = a3 - 30;                           /* 0x80163768: addiu a3,a3,-0x1e */
  psx_check_interrupts_at(cpu, 0x80163814u);

block_80163814:
  a3 = a3 << 16;                          /* 0x80163814: sll a3,a3,0x10 */
  cpu->write_word(sp + 16, a0);           /* 0x80163818: sw a0,0x10(sp) */
  cpu->write_word(sp + 20, v0);           /* 0x8016381C: sw v0,0x14(sp) */
  /* fall through to block_80163820 */

block_80163820:
  a0 = cpu->read_word(t0 + 8);            /* 0x80163820: lw a0,8(t0) */
  a1 = cpu->read_word(t0 + 12);           /* 0x80163824: lw a1,0xc(t0) */
  ra = 0x80163830u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  a3 = (int32_t)a3 >> 16;                 /* 0x8016382C: sra a3,a3,0x10 */
  psx_check_interrupts_at(cpu, 0x8012E078u);
  cpu->pc = 0x8012E078u; return;  /* CPS jal -> 0x8012E078 */
}

/* ======== 0x80163780 — mode_name_case_practice ========
 * Dispatch case that probes t0+0x9b: value 4 selects "PRACTICE MODE"
 * (0x80121E7C), anything else "CUSTOM MODE" (0x80121E8C); both arms join
 * the shared announce jal at 0x80163820. Faithful to
 * br2eu_func_80163780. */
void mode_name_case_practice(void)
{
  CPUState *cpu = clean_cpu_state();
  if (!cpu) return;
  int cond;

block_80163780:
  v0 = cpu->read_byte(t0 + 155);          /* 0x80163780: lbu v0,0x9b(t0) */
  v1 = 4;                                 /* 0x80163784: addiu v1,zero,4 */
  cond = (v0 != v1);          /* bne v0,v1 */
  /* delay slot (always executes) */
  a2 = 0x8012 << 16;                      /* 0x8016378C: lui a2,0x8012 */
  if (cond) {
    psx_check_interrupts_at(cpu, 0x801637ACu);
    goto block_801637AC;
  } else {
    psx_check_interrupts_at(cpu, 0x80163790u);
  }

block_80163790:
  a2 = 0x8012 << 16;                      /* 0x80163790: lui a2,0x8012 */
  a2 = a2 + 7804;                         /* 0x80163794: addiu a2,a2,0x1e7c -> 0x80121E7C "PRACTICE MODE" */
  a3 = a3 - 39;                           /* 0x80163798: addiu a3,a3,-0x27 */
  a3 = a3 << 16;                          /* 0x8016379C: sll a3,a3,0x10 */
  cpu->write_word(sp + 16, a0);           /* 0x801637A0: sw a0,0x10(sp) */
  /* j 0x80163820 — delay slot */
  cpu->write_word(sp + 20, v1);           /* 0x801637A8: sw v1,0x14(sp) */
  psx_check_interrupts_at(cpu, 0x80163820u);
  goto block_80163820;

block_801637AC:
  a2 = a2 + 7820;                         /* 0x801637AC: addiu a2,a2,0x1e8c -> 0x80121E8C "CUSTOM MODE" */
  a3 = a3 - 33;                           /* 0x801637B0: addiu a3,a3,-0x21 */
  a3 = a3 << 16;                          /* 0x801637B4: sll a3,a3,0x10 */
  cpu->write_word(sp + 16, a0);           /* 0x801637B8: sw a0,0x10(sp) */
  /* j 0x80163820 — delay slot */
  cpu->write_word(sp + 20, v1);           /* 0x801637C0: sw v1,0x14(sp) */
  psx_check_interrupts_at(cpu, 0x80163820u);

block_80163820:
  a0 = cpu->read_word(t0 + 8);            /* 0x80163820: lw a0,8(t0) */
  a1 = cpu->read_word(t0 + 12);           /* 0x80163824: lw a1,0xc(t0) */
  ra = 0x80163830u;  /* jal link before delay slot */
  /* delay slot (always executes) */
  a3 = (int32_t)a3 >> 16;                 /* 0x8016382C: sra a3,a3,0x10 */
  psx_check_interrupts_at(cpu, 0x8012E078u);
  cpu->pc = 0x8012E078u; return;  /* CPS jal -> 0x8012E078 */
}
