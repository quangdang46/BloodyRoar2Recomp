#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern CPUState *clean_cpu_state(void);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st32(uint32_t a, uint32_t v);
extern uint8_t  br2_ld8(uint32_t a);
extern uint16_t br2_ld16(uint32_t a);
extern void     br2_st8(uint32_t a, uint8_t v);
extern void     br2_st16(uint32_t a, uint16_t v);
extern int      psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);
extern uint32_t br2_call_generated(uint32_t addr, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
extern void     psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern void     gte_precision_store_word(uint32_t addr, uint8_t reg);
extern uint32_t prim_persp_ot_emit(uint32_t cur, uint32_t ot, uint32_t count,
                                   uint32_t delta, uint32_t in_stride,
                                   uint32_t out_stride);

#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))

/* swl_guest — unaligned word store: RMW of the aligned word,
 * byte lane selected by addr&3 (guest swl semantics). */
static void swl_guest(uint32_t addr, uint32_t v)
{
  uint32_t al = addr & ~3u;
  uint32_t w = DB32(al);
  switch (addr & 3u) {
  case 0: w = (w & 0xFFFFFF00u) | (v >> 24); break;
  case 1: w = (w & 0xFFFF0000u) | (v >> 16); break;
  case 2: w = (w & 0xFF000000u) | (v >> 8);  break;
  default: w = v; break;
  }
  DB32(al) = w;
}

/* prim_ot_link_indexed — ordering-table link: slot = ot +
 * (otz&0x1ffc); writes out<<8 into the slot and back-links the
 * old head into out+2. */
static void prim_ot_link_indexed(uint32_t ot, uint32_t out, uint32_t otz)
{
  uint32_t slot = ot + (otz & 0x1ffcu);
  uint32_t old  = DB32(slot);
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
}

/* ======== 0x801870A4 ======== */

/* prim_xform3_task_run_b — sibling of prim_xform3_task_run (0x8018B100):
 * dual-buffer primitive-list transform + task runner (guest 0x801870A4,
 * 280B, 14 CPS blocks — all 14 addrs registered in clean_cps.map).
 * Same skeleton: saves caller s0-s6+ra to scratchpad 0x1F8003E0..0x3FC,
 * ctc2 ctrl29=1365 / ctrl30=1024 up front (the sibling has no ctrl setup),
 * walks `count` 0x28-byte records at a0. Per record with lh(rec+0xC)==0
 * and lw(rec)!=0 it calls 0x80186E64 (a1=prim+0x20) then 0x80186F5C
 * (a0=s4), loads V0.xy from rec+4/+8, runs GTE cmd 0x0480012, reads
 * MAC1-3 into gte_ctrl[5..7] (the sibling stashes them to SPAD instead),
 * computes a1 = s1 + lh(rec+0xE), then jalr's the record's 16-byte task
 * entries (count at [s2], entries at s2+4 stride 16: a3=ent[1],
 * a0=ent[2], a1=s1+lh, fn=ent[3]). Epilogue restores SPAD regs and
 * returns v0 = final record pointer (a0 + 0x28*count), or a0 when
 * count <= 0.
 *
 * All three call sites are CPS tail-dispatches ($ra = continuation,
 * psx_check_interrupts_at, cpu->pc = callee, return) exactly like the
 * generated body — a yield inside any callee resumes via clean_cps.map
 * -> this body's pc-switch. a4/a5 arrive on the guest stack
 * (sp+16/sp+20 -> s4/s5) exactly as generated reads them.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801870A4. */
uint32_t prim_xform3_task_run_b(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  if (!c) return 0;
  pc = c->pc;
  if (pc != 0u && pc != 0x801870A4u) {
    switch (pc) {
    case 0x801870ACu: goto b0AC;
    case 0x801870F0u: goto b0F0;
    case 0x80187100u: goto b100;
    case 0x80187108u: goto b108;
    case 0x80187110u: goto b110;
    case 0x80187118u: goto b118;
    case 0x80187144u: goto b144;
    case 0x8018714Cu: goto b14C;
    case 0x80187164u: goto b164;
    case 0x80187178u: goto b178;
    case 0x80187180u: goto b180;
    case 0x8018718Cu: goto b18C;
    case 0x801871B4u: goto b1B4;
    default: break;
    }
  }

  /* block_801870A4 — entry: blez a3 -> shared epilogue; delay t0 = SPAD */
  { int _bc = ((int32_t)c->gpr[7] <= 0);
    c->gpr[8] = 0x1F800000u;                        /* delay: t0 = 0x1F800000 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x801871B4u);
      goto b1B4;
    } else {
      psx_check_interrupts_at(c, 0x801870ACu);
      goto b0AC;
    } }

b0AC:
  /* save caller s0-s6 + ra into scratchpad 0x1F8003E0..0x3FC */
  br2_st32(c->gpr[8] + 1020, c->gpr[31]);            /* ra  -> 0x1F8003FC */
  br2_st32(c->gpr[8] + 1016, c->gpr[16]);            /* s0  -> 0x1F8003F8 */
  br2_st32(c->gpr[8] + 1012, c->gpr[17]);            /* s1  -> 0x1F8003F4 */
  br2_st32(c->gpr[8] + 1008, c->gpr[18]);            /* s2  -> 0x1F8003F0 */
  br2_st32(c->gpr[8] + 1004, c->gpr[19]);            /* s3  -> 0x1F8003EC */
  br2_st32(c->gpr[8] + 1000, c->gpr[20]);            /* s4  -> 0x1F8003E8 */
  br2_st32(c->gpr[8] +  996, c->gpr[21]);            /* s5  -> 0x1F8003E4 */
  br2_st32(c->gpr[8] +  992, c->gpr[22]);            /* s6  -> 0x1F8003E0 */
  c->gpr[8]  = 1365;
  gte_write_ctrl(c, 29, c->gpr[8]);                  /* ctc2 ctrl29 = 1365 */
  c->gpr[8]  = 1024;
  gte_write_ctrl(c, 30, c->gpr[8]);                  /* ctc2 ctrl30 = 1024 */
  c->gpr[16] = c->gpr[4];                            /* s0 = a0 */
  c->gpr[17] = c->gpr[5];                            /* s1 = a1 */
  c->gpr[19] = c->gpr[7];                            /* s3 = a3 */
  c->gpr[20] = br2_ld32(c->gpr[29] + 16);            /* s4 = lw sp+16 (a4) */
  c->gpr[21] = br2_ld32(c->gpr[29] + 20);            /* s5 = lw sp+20 (a5) */
  psx_check_interrupts_at(c, 0x801870F0u);
  goto b0F0;

b0F0:                                               /* record loop head */
  c->gpr[8] = (uint32_t)(int32_t)(int16_t)br2_ld16(c->gpr[16] + 12); /* lh sx16 */
  c->gpr[5] = br2_ld32(c->gpr[16]);                  /* a1 = lw(s0) prim */
  { int _bc = (c->gpr[8] != c->gpr[0]);              /* bne t0,zero -> skip */
    c->gpr[4] = c->gpr[21];                          /* delay: a0 = s5 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x80187180u);
      goto b180;
    } else {
      psx_check_interrupts_at(c, 0x80187100u);
      goto b100;
    } }

b100:
  { int _bc = (c->gpr[5] == c->gpr[0]);              /* beq a1,zero -> skip */
    /* nop delay slot */
    if (_bc) {
      psx_check_interrupts_at(c, 0x80187180u);
      goto b180;
    } else {
      psx_check_interrupts_at(c, 0x80187108u);
      goto b108;
    } }

b108:
  c->gpr[31] = 0x80187110u;                          /* jal link */
  c->gpr[5]  = c->gpr[5] + 32;                       /* delay: a1 = prim+0x20 */
  psx_check_interrupts_at(c, 0x80186E64u);           /* jal -> 0x80186E64 */
  c->pc = 0x80186E64u;
  return c->gpr[2];

b110:
  c->gpr[31] = 0x80187118u;                          /* jal link */
  c->gpr[4]  = c->gpr[20];                            /* delay: a0 = s4 */
  psx_check_interrupts_at(c, 0x80186F5Cu);           /* jal -> 0x80186F5C */
  c->pc = 0x80186F5Cu;
  return c->gpr[2];

b118:
  c->gte_data[0] = psx_cyc_lwc2_read(c, c->gpr[16] + 4);    /* lwc2 gte[0] */
  gte_write_data(c, 1, psx_cyc_lwc2_read(c, c->gpr[16] + 8)); /* lwc2 gte[1] */
  /* nop */
  gte_execute(c, 0x0480012u);                        /* GTE cmd 0x12 */
  c->gpr[18] = br2_ld32(c->gpr[16] + 28);             /* s2 = lw(s0+0x1C) tasks */
  /* nop */
  c->gpr[22] = br2_ld32(c->gpr[18]);                  /* s6 = lw(s2) count */
  c->gpr[10] = c->gte_data[27];                       /* mfc2 t2, data27 */
  c->gpr[9]  = c->gte_data[26];                       /* mfc2 t1, data26 */
  { int _bc = ((int32_t)c->gpr[10] <= 0);            /* blez t2 -> skip rec */
    /* nop delay slot */
    if (_bc) {
      psx_check_interrupts_at(c, 0x80187180u);
      goto b180;
    } else {
      psx_check_interrupts_at(c, 0x80187144u);
      goto b144;
    } }

b144:
  { int _bc = (c->gpr[22] == c->gpr[0]);              /* beq s6,zero -> skip */
    c->gpr[18] = c->gpr[18] + 4;                      /* delay: s2 += 4 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x80187180u);
      goto b180;
    } else {
      psx_check_interrupts_at(c, 0x8018714Cu);
      goto b14C;
    } }

b14C:
  c->gpr[8]  = c->gte_data[25];                       /* mfc2 t0, data25 */
  c->gpr[11] = (uint32_t)(int32_t)(int16_t)br2_ld16(c->gpr[16] + 14); /* lh sx16 */
  c->gte_ctrl[5] = c->gpr[8];                         /* ctc2 ctrl5 = MAC1 */
  c->gte_ctrl[6] = c->gpr[9];                         /* ctc2 ctrl6 = MAC2 */
  c->gte_ctrl[7] = c->gpr[10];                        /* ctc2 ctrl7 = MAC3 */
  c->gpr[5]  = c->gpr[17] + c->gpr[11];               /* a1 = s1 + lh(s0+0xE) */
  psx_check_interrupts_at(c, 0x80187164u);
  goto b164;

b164:                                               /* task-entry loop */
  c->gpr[7] = br2_ld32(c->gpr[18] + 4);               /* a3 = ent[1] */
  c->gpr[8] = br2_ld32(c->gpr[18] + 12);              /* t0 = ent[3] fn */
  c->gpr[4] = br2_ld32(c->gpr[18] + 8);               /* a0 = ent[2] */
  c->gpr[31] = 0x80187178u;                           /* jalr link */
  c->gpr[22] = c->gpr[22] + 0xFFFFFFFFu;              /* delay: s6-- */
  { uint32_t _jt = c->gpr[8];                         /* jalr $ra,$t0 */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt;
    return c->gpr[2]; }

b178:
  { int _bc = ((int32_t)c->gpr[22] > 0);              /* bgtz s6 -> next entry */
    c->gpr[18] = c->gpr[18] + 16;                     /* delay: s2 += 16 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x80187164u);
      goto b164;
    } else {
      psx_check_interrupts_at(c, 0x80187180u);
      goto b180;
    } }

b180:                                               /* record loop tail */
  c->gpr[19] = c->gpr[19] + 0xFFFFFFFFu;              /* s3-- */
  { int _bc = ((int32_t)c->gpr[19] > 0);              /* bgtz s3 -> next record */
    c->gpr[16] = c->gpr[16] + 40;                     /* delay: s0 += 0x28 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x801870F0u);
      goto b0F0;
    } else {
      psx_check_interrupts_at(c, 0x8018718Cu);
      goto b18C;
    } }

b18C:                                               /* epilogue: restore SPAD */
  c->gpr[4]  = c->gpr[16];                            /* a0 = final record ptr */
  c->gpr[8]  = 0x1F800000u;                           /* t0 = 0x1F800000 */
  c->gpr[31] = br2_ld32(c->gpr[8] + 1020);            /* ra  <- 0x1F8003FC */
  c->gpr[16] = br2_ld32(c->gpr[8] + 1016);            /* s0  <- 0x1F8003F8 */
  c->gpr[17] = br2_ld32(c->gpr[8] + 1012);            /* s1  <- 0x1F8003F4 */
  c->gpr[18] = br2_ld32(c->gpr[8] + 1008);            /* s2  <- 0x1F8003F0 */
  c->gpr[19] = br2_ld32(c->gpr[8] + 1004);            /* s3  <- 0x1F8003EC */
  c->gpr[20] = br2_ld32(c->gpr[8] + 1000);            /* s4  <- 0x1F8003E8 */
  c->gpr[21] = br2_ld32(c->gpr[8] +  996);            /* s5  <- 0x1F8003E4 */
  c->gpr[22] = br2_ld32(c->gpr[8] +  992);            /* s6  <- 0x1F8003E0 */
  psx_check_interrupts_at(c, 0x801871B4u);
  goto b1B4;

b1B4:
  { uint32_t _jt = c->gpr[31];                        /* jr $ra */
    c->gpr[2] = c->gpr[4];                            /* delay: v0 = a0 */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt;
    return c->gpr[2]; }
}

/* ======== 0x80188200 ======== */

/* prim_quad_emit_54 — quad GTE emitter over vertex-pointer records,
 * input stride 84 (0x54). a0 = record cursor (returned in v0), a1 = OT
 * base, a2 = out-delta select (0 -> t4=28, else 28+28=56), a3 = signed
 * count (<=0 -> return cursor unchanged).
 *
 * Per record: t6/t7/t8 = three vertex pointers at a0+0/+4/+8; their xyz
 * words load raw into GTE V0/V1/V2 (mtc2 data 12/13/14) and are stored
 * raw at out+8/+16/+24; NCLIP writes the MAC0-class flag into data[24];
 * MAC0<=0 skips the transform body. Otherwise: v1 attr record ptr =
 * ld32(a0+24), its *ptr word and lhu(ptr+4) split into (x&0xffff,
 * x>>16) -> mtc2 data9/10 with lwc2 data11 = ptr+4, lwc2 data17 =
 * t6+4, gte[6] attr word @a0+12, RTPS, lwc2 data18 = t7+4, lagged
 * swc2 gte[22]->out+4; the same mtc2 9/10/11 triple repeats verbatim
 * for vertices 2 and 3 (guest reloads the unchanged regs), each with
 * its own gte[6] attr word @a0+16/+20 and gte[19] = t8+4 before the
 * third RTPS; OP 0x158002D runs between vertex 2 and 3; final vertex
 * reads OTZ (data7), RTPS, then the indexed OT link
 * (slot = a1 + (otz & 0x1ffc), swl pair) and swc2 gte[22]->out+20.
 * a0 += 84 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188200. */
uint32_t prim_quad_emit_54(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_80188200:
  if ((int32_t)cpu->gpr[7] <= 0) goto block_80188300;   /* blez a3 */

block_80188208:
  { int _bc = (cpu->gpr[6] == cpu->gpr[0]);            /* beq a2,zero */
    cpu->gpr[12] = 28;                               /* delay slot: t4=28 */
    if (_bc) goto block_80188214; else goto block_80188210; }

block_80188210:
  cpu->gpr[12] = cpu->gpr[12] + 28;                   /* t4 = 56 */
  /* fall through */

block_80188214:
  cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = vertex ptr0 */
  cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = vertex ptr1 */
  cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = vertex ptr2 */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6 (xyz) */
  cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7 */
  cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8 */
  gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 V0.xy */
  gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 V0.z  */
  gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2        */
  cpu->gpr[7]  = cpu->gpr[7] + 0xFFFFFFFFu;           /* a3-- */
  cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
  gte_execute(cpu, 0x1400006u);                       /* NCLIP */
  br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
  br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);
  br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);
  cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
  if ((int32_t)cpu->gpr[13] <= 0) goto block_801882F8;/* blez t5 */

block_80188260:
  cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 24);          /* v1 attr ptr */
  cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12); /* lwc2 gte[6] */
  cpu->gpr[9]  = br2_ld32(cpu->gpr[8]);               /* *attrptr */
  cpu->gpr[10] = br2_ld16(cpu->gpr[8] + 4);           /* lhu attrptr+4 */
  cpu->gpr[8]  = cpu->gpr[9] << 16;
  cpu->gpr[8]  = (uint32_t)cpu->gpr[8] >> 16;         /* *ptr low16 */
  cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;         /* *ptr high16 */
  gte_write_data(cpu, 9,  cpu->gpr[8]);               /* mtc2 */
  gte_write_data(cpu, 10, cpu->gpr[9]);               /* mtc2 */
  gte_write_data(cpu, 11, cpu->gpr[10]);              /* mtc2 */
  gte_write_data(cpu, 17, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 */
  gte_execute(cpu, 0x138041Cu);                       /* RTPS */
  gte_write_data(cpu, 18, psx_cyc_lwc2_read(cpu, cpu->gpr[15] + 4)); /* lwc2 */
  br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);      /* swc2 gte[22] */
  gte_precision_store_word(cpu->gpr[11] + 4, 22);
  gte_write_data(cpu, 9,  cpu->gpr[8]);               /* mtc2 (repeat) */
  gte_write_data(cpu, 10, cpu->gpr[9]);               /* mtc2 (repeat) */
  gte_write_data(cpu, 11, cpu->gpr[10]);              /* mtc2 (repeat) */
  cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] */
  gte_execute(cpu, 0x138041Cu);                       /* RTPS */
  gte_write_data(cpu, 19, psx_cyc_lwc2_read(cpu, cpu->gpr[24] + 4)); /* lwc2 */
  gte_execute(cpu, 0x158002Du);                       /* OP */
  br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);     /* swc2 gte[22] */
  gte_precision_store_word(cpu->gpr[11] + 12, 22);
  gte_write_data(cpu, 9,  cpu->gpr[8]);               /* mtc2 (repeat) */
  gte_write_data(cpu, 10, cpu->gpr[9]);               /* mtc2 (repeat) */
  gte_write_data(cpu, 11, cpu->gpr[10]);              /* mtc2 (repeat) */
  cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20); /* lwc2 gte[6] */
  cpu->gpr[8]  = gte_read_data(cpu, 7);               /* mfc2 t0, OTZ */
  gte_execute(cpu, 0x138041Cu);                       /* RTPS */
  cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;               /* otz & 0x1ffc */
  cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];           /* OT slot */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);               /* old = *slot */
  cpu->gpr[9]  = cpu->gpr[11] << 8;                   /* out << 8 */
  swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);            /* swl slot+2 */
  cpu->gpr[8]  = cpu->gpr[8] << 8;                    /* old << 8 */
  swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);           /* swl out+2 */
  br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);     /* swc2 gte[22] */
  gte_precision_store_word(cpu->gpr[11] + 20, 22);
  /* fall through */

block_801882F8:
  { int _bc = ((int32_t)cpu->gpr[7] > 0);             /* bgtz a3 */
    cpu->gpr[4] = cpu->gpr[4] + 84;                   /* delay slot: a0 += 0x54 */
    if (_bc) goto block_80188214; else goto block_80188300; }

block_80188300:
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188308 ======== */

/* prim_quad_emit_5c — quad GTE emitter over vertex-pointer records,
 * input stride 92 (0x5c). Same prologue/NCLIP/raw-store skeleton as
 * prim_quad_emit_54 but out-delta t4 = 36 (a2==0) else 36+28=64, and a
 * DIFFERENT vertex-1 body: each vertex gets its own attr record ptr
 * (ld32 a0+24/+28/+32), lwc2 gte[17/18/19] = t6/t7/t8+4 interleaved
 * BEFORE that vertex's attr word, gte[6] attr words @a0+12/+16/+20,
 * lwc2 gte[11] = attrptr+4, and the lagged swc2 gte[22] -> out+4/+12
 * lands inside the NEXT vertex's section. OP 0x158002D runs inside
 * vertex 3's section (after its swc2, before the attr split). Final
 * vertex reads OTZ (data7), RTPS, indexed OT link, swc2 gte[22]->out+20.
 * a0 += 92 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188308. */
uint32_t prim_quad_emit_5c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_80188308:
  if ((int32_t)cpu->gpr[7] <= 0) goto block_8018842C;   /* blez a3 */

block_80188310:
  { int _bc = (cpu->gpr[6] == cpu->gpr[0]);            /* beq a2,zero */
    cpu->gpr[12] = 36;                               /* delay slot: t4=36 */
    if (_bc) goto block_8018831C; else goto block_80188318; }

block_80188318:
  cpu->gpr[12] = cpu->gpr[12] + 28;                   /* t4 = 64 */
  /* fall through */

block_8018831C:
  cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = vertex ptr0 */
  cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = vertex ptr1 */
  cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = vertex ptr2 */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6 (xyz) */
  cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7 */
  cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8 */
  gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 V0.xy */
  gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 V0.z  */
  gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2        */
  cpu->gpr[7]  = cpu->gpr[7] + 0xFFFFFFFFu;           /* a3-- */
  cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
  gte_execute(cpu, 0x1400006u);                       /* NCLIP */
  br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
  br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);
  br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);
  cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
  if ((int32_t)cpu->gpr[13] <= 0) goto block_80188424;/* blez t5 */

block_80188368:
  cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 24);          /* v1 attr ptr */
  gte_write_data(cpu, 17, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 */
  cpu->gpr[10] = br2_ld32(cpu->gpr[8]);               /* *attrptr */
  cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12); /* lwc2 gte[6] */
  cpu->gpr[9]  = cpu->gpr[10] << 16;
  cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;         /* *ptr low16 */
  cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;        /* *ptr high16 */
  gte_write_data(cpu, 9,  cpu->gpr[9]);               /* mtc2 */
  gte_write_data(cpu, 10, cpu->gpr[10]);              /* mtc2 */
  gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 */
  gte_execute(cpu, 0x138041Cu);                       /* RTPS (v1) */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 28);          /* v2 attr ptr */
  gte_write_data(cpu, 18, psx_cyc_lwc2_read(cpu, cpu->gpr[15] + 4)); /* lwc2 */
  cpu->gpr[10] = br2_ld32(cpu->gpr[8]);               /* *attrptr */
  br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);      /* swc2 gte[22] (v1) */
  gte_precision_store_word(cpu->gpr[11] + 4, 22);
  cpu->gpr[9]  = cpu->gpr[10] << 16;
  cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
  cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
  gte_write_data(cpu, 9,  cpu->gpr[9]);               /* mtc2 */
  gte_write_data(cpu, 10, cpu->gpr[10]);              /* mtc2 */
  gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 */
  cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] */
  gte_execute(cpu, 0x138041Cu);                       /* RTPS (v2) */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 32);          /* v3 attr ptr */
  gte_write_data(cpu, 19, psx_cyc_lwc2_read(cpu, cpu->gpr[24] + 4)); /* lwc2 */
  cpu->gpr[10] = br2_ld32(cpu->gpr[8]);               /* *attrptr */
  br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);     /* swc2 gte[22] (v2) */
  gte_precision_store_word(cpu->gpr[11] + 12, 22);
  gte_execute(cpu, 0x158002Du);                       /* OP */
  cpu->gpr[9]  = cpu->gpr[10] << 16;
  cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
  cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
  gte_write_data(cpu, 9,  cpu->gpr[9]);               /* mtc2 */
  gte_write_data(cpu, 10, cpu->gpr[10]);              /* mtc2 */
  gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 */
  cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20); /* lwc2 gte[6] */
  cpu->gpr[8]  = gte_read_data(cpu, 7);               /* mfc2 t0, OTZ */
  gte_execute(cpu, 0x138041Cu);                       /* RTPS (v3) */
  cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;               /* otz & 0x1ffc */
  cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];           /* OT slot */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);               /* old = *slot */
  cpu->gpr[9]  = cpu->gpr[11] << 8;                   /* out << 8 */
  swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);            /* swl slot+2 */
  cpu->gpr[8]  = cpu->gpr[8] << 8;                    /* old << 8 */
  swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);           /* swl out+2 */
  br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);     /* swc2 gte[22] */
  gte_precision_store_word(cpu->gpr[11] + 20, 22);
  /* fall through */

block_80188424:
  { int _bc = ((int32_t)cpu->gpr[7] > 0);             /* bgtz a3 */
    cpu->gpr[4] = cpu->gpr[4] + 92;                   /* delay slot: a0 += 0x5c */
    if (_bc) goto block_8018831C; else goto block_8018842C; }

block_8018842C:
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188434 ======== */

/* prim_emit_48 — single-vertex GTE emitter, input stride 72 (0x48).
 * a0 = record cursor (returned in v0), a1 = OT base, a2 = out-delta
 * select (0 -> t4=24, else 24+24=48), a3 = signed count.
 *
 * Per record: same three-pointer prologue as the quad family (t6/t7/t8
 * = ptrs at a0+0/+4/+8, xyz -> mtc2 12/13/14, raw xyz stores at
 * out+8/+12/+16 — packed stride 4, NCLIP, MAC0<=0 skips). Otherwise:
 * t9 = ld32(a0+12) companion ptr, t0 = ld32(a0+20) attr ptr,
 * t5 = *t9 stored raw at out+20, lwc2 gte[16/17/18] = t6/t7/t8+4 and
 * gte[19] = t9+4, AVSZ4 (0x168002E) BEFORE the attr split; *t0 splits
 * into mtc2 data9/10 with lwc2 data11 = t0+4, gte[6] attr word @a0+16,
 * OTZ (data7) read, RTPS, indexed OT link, swc2 gte[22]->out+4.
 * a0 += 72 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188434. */
uint32_t prim_emit_48(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_80188434:
  if ((int32_t)cpu->gpr[7] <= 0) goto block_80188508;   /* blez a3 */

block_8018843C:
  { int _bc = (cpu->gpr[6] == cpu->gpr[0]);            /* beq a2,zero */
    cpu->gpr[12] = 24;                               /* delay slot: t4=24 */
    if (_bc) goto block_80188448; else goto block_80188444; }

block_80188444:
  cpu->gpr[12] = cpu->gpr[12] + 24;                   /* t4 = 48 */
  /* fall through */

block_80188448:
  cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = vertex ptr0 */
  cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = vertex ptr1 */
  cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = vertex ptr2 */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6 (xyz) */
  cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7 */
  cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8 */
  gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 V0.xy */
  gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 V0.z  */
  gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2        */
  cpu->gpr[7]  = cpu->gpr[7] + 0xFFFFFFFFu;           /* a3-- */
  cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
  gte_execute(cpu, 0x1400006u);                       /* NCLIP */
  br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
  br2_st32(cpu->gpr[11] + 12, cpu->gpr[9]);
  br2_st32(cpu->gpr[11] + 16, cpu->gpr[10]);
  cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
  if ((int32_t)cpu->gpr[13] <= 0) goto block_80188500;/* blez t5 */

block_80188494:
  cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);          /* t9 = *(a0+12) ptr */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 20);          /* t0 = *(a0+20) ptr */
  cpu->gpr[13] = br2_ld32(cpu->gpr[25]);              /* t5 = *t9 */
  gte_write_data(cpu, 16, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 */
  gte_write_data(cpu, 17, psx_cyc_lwc2_read(cpu, cpu->gpr[15] + 4)); /* lwc2 */
  gte_write_data(cpu, 18, psx_cyc_lwc2_read(cpu, cpu->gpr[24] + 4)); /* lwc2 */
  gte_write_data(cpu, 19, psx_cyc_lwc2_read(cpu, cpu->gpr[25] + 4)); /* lwc2 */
  br2_st32(cpu->gpr[11] + 20, cpu->gpr[13]);          /* sw t5,20(t3) */
  gte_execute(cpu, 0x168002Eu);                       /* AVSZ4 */
  cpu->gpr[10] = br2_ld32(cpu->gpr[8]);               /* *t0 */
  cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] */
  cpu->gpr[9]  = cpu->gpr[10] << 16;
  cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;         /* *t0 low16 */
  cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;        /* *t0 high16 */
  gte_write_data(cpu, 9,  cpu->gpr[9]);               /* mtc2 */
  gte_write_data(cpu, 10, cpu->gpr[10]);              /* mtc2 */
  gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 */
  cpu->gpr[8]  = gte_read_data(cpu, 7);               /* mfc2 t0, OTZ */
  gte_execute(cpu, 0x138041Cu);                       /* RTPS */
  cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;               /* otz & 0x1ffc */
  cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];           /* OT slot */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);               /* old = *slot */
  cpu->gpr[9]  = cpu->gpr[11] << 8;                   /* out << 8 */
  swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);            /* swl slot+2 */
  cpu->gpr[8]  = cpu->gpr[8] << 8;                    /* old << 8 */
  swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);           /* swl out+2 */
  br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);      /* swc2 gte[22] */
  gte_precision_store_word(cpu->gpr[11] + 4, 22);
  /* fall through */

block_80188500:
  { int _bc = ((int32_t)cpu->gpr[7] > 0);             /* bgtz a3 */
    cpu->gpr[4] = cpu->gpr[4] + 72;                   /* delay slot: a0 += 0x48 */
    if (_bc) goto block_80188448; else goto block_80188508; }

block_80188508:
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x801890DC ======== */

/* prim_persp_emit_78 — perspective GTE emitter, input stride 120
 * (0x78). a0 = record cursor (returned in v0), a1 = OT base, a2 =
 * out-delta select (0 -> t4=40, else 40+40=80), a3 = signed count.
 *
 * Per record: six packed vertex words lwc2 into GTE V0/V1/V2 (regs
 * 0-5, alternating raw gte_data[]/gte_write_data forms exactly as
 * generated); RTPT (0x0280030); t3 = a0+t4; attribute byte =
 * lbu(out+7)<<24 | 0x00808080 written DIRECT to gte_data[6]; companion
 * words t1 = ld32(a0+24), t2 = ld32(a0+28); cfc2 ctrl31 read (dead —
 * result unused, faithful); NCLIP; mtc2 gte[0] = t1 raw, gte[1] = t2
 * via gte_write_data; MAC0<=0 skips. Otherwise: gte cmd 0x0180001,
 * second pair t1 = ld32(a0+32), t2 = ld32(a0+36) -> gte[0]/[1],
 * swc2 gte[14]->out+32, sw SXY0 (data12 saved in t9) -> out+8,
 * gte cmd 0x108041B, FIXED OT link at a1 (lw a1, swl pair), swc2
 * gte[12]->out+16, swc2 gte[13]->out+24, swc2 gte[22]->out+4.
 * a0 += 120 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801890DC. */
uint32_t prim_persp_emit_78(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_801890DC:
  if ((int32_t)cpu->gpr[7] <= 0) goto block_8018919C;   /* blez a3 */

block_801890E4:
  { int _bc = (cpu->gpr[6] == cpu->gpr[0]);            /* beq a2,zero */
    cpu->gpr[12] = 40;                               /* delay slot: t4=40 */
    if (_bc) goto block_801890F0; else goto block_801890EC; }

block_801890EC:
  cpu->gpr[12] = cpu->gpr[12] + 40;                   /* t4 = 80 */
  /* fall through */

block_801890F0:
  cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
  gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
  cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
  gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
  cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
  gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
  cpu->gpr[7]  = cpu->gpr[7] + 0xFFFFFFFFu;           /* a3-- */
  gte_execute(cpu, 0x0280030u);                       /* RTPT */
  cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
  cpu->gpr[24] = br2_ld8(cpu->gpr[11] + 7);           /* lbu t8,7(t3) attr */
  cpu->gpr[25] = 0x0080u << 16;                       /* lui t9,0x80 */
  cpu->gpr[25] = cpu->gpr[25] | 0x8080u;              /* ori t9,0x8080 */
  cpu->gpr[24] = cpu->gpr[24] << 24;                  /* attr << 24 */
  cpu->gpr[24] = cpu->gpr[24] | cpu->gpr[25];         /* | 0x00808080 */
  cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 24);          /* t1 = *(a0+24) */
  cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 28);          /* t2 = *(a0+28) */
  cpu->gpr[2]  = gte_read_ctrl(cpu, 31);              /* cfc2 v0,ctrl31 */
  gte_execute(cpu, 0x1400006u);                       /* NCLIP */
  cpu->gte_data[6] = cpu->gpr[24];                    /* mtc2 gte[6] attr */
  cpu->gte_data[0] = cpu->gpr[9];                     /* mtc2 gte[0] */
  gte_write_data(cpu, 1, cpu->gpr[10]);               /* mtc2 gte[1] */
  cpu->gpr[8]  = cpu->gte_data[24];                   /* mfc2 t0, data24 */
  cpu->gpr[25] = cpu->gte_data[12];                   /* mfc2 t9, SXY0 */
  if ((int32_t)cpu->gpr[8] <= 0) goto block_80189194; /* blez t0 */

block_80189154:
  gte_execute(cpu, 0x0180001u);                       /* gte cmd 0x01 */
  cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 32);          /* t1 = *(a0+32) */
  cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 36);          /* t2 = *(a0+36) */
  br2_st32(cpu->gpr[11] + 32, cpu->gte_data[14]);     /* swc2 gte[14] */
  gte_precision_store_word(cpu->gpr[11] + 32, 14);
  cpu->gte_data[0] = cpu->gpr[9];                     /* mtc2 gte[0] */
  gte_write_data(cpu, 1, cpu->gpr[10]);               /* mtc2 gte[1] */
  br2_st32(cpu->gpr[11] + 8, cpu->gpr[25]);           /* sw t9,SXY0 -> out+8 */
  gte_execute(cpu, 0x108041Bu);                       /* gte cmd 0x1B */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);               /* old = *a1 (OT) */
  br2_st32(cpu->gpr[11] + 16, cpu->gte_data[12]);     /* swc2 gte[12] */
  gte_precision_store_word(cpu->gpr[11] + 16, 12);
  br2_st32(cpu->gpr[11] + 24, cpu->gte_data[13]);     /* swc2 gte[13] */
  gte_precision_store_word(cpu->gpr[11] + 24, 13);
  cpu->gpr[9]  = cpu->gpr[11] << 8;                   /* out << 8 */
  swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);            /* swl a1+2 */
  cpu->gpr[8]  = cpu->gpr[8] << 8;                    /* old << 8 */
  swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);           /* swl out+2 */
  br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);      /* swc2 gte[22] */
  gte_precision_store_word(cpu->gpr[11] + 4, 22);
  /* fall through */

block_80189194:
  { int _bc = ((int32_t)cpu->gpr[7] > 0);             /* bgtz a3 */
    cpu->gpr[4] = cpu->gpr[4] + 120;                  /* delay slot: a0 += 0x78 */
    if (_bc) goto block_801890F0; else goto block_8018919C; }

block_8018919C:
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018B738 ======== */

/* prim_persp_ot_emit_var — parameterized prologue of the
 * prim_persp_ot_emit family (guest 0x8018B738, 52B, 5 CPS blocks).
 * The 0x8018B748 alternate entry (t6/t7/t8 = 40/112/8) is a mid-function
 * CPS piece with no fnmap row — it stays generated. This port covers
 * the 0x8018B738 entry (t6/t7/t8 = 24/80/4):
 *   a3 <= 0  -> tail to 0x8018B88C epilogue (v0 = a0)
 *   a2 == 0  -> tail to 0x8018B76C with t4 = 32
 *   a2 != 0  -> t4 = 32 + t6 = 56, tail to 0x8018B76C
 * The callee consumes the live cursor/OT/count/delta/strides from
 * gpr[4]/[5]/[7]/[12]/[15]/[24] exactly like the generated fallthrough.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018B738. */
uint32_t prim_persp_ot_emit_var(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  cpu->gpr[14] = 24;                                /* t6 = 24 */
  cpu->gpr[15] = 80;                                /* t7 = 80 */
  cpu->gpr[24] = 4;                                 /* t8 = 4  */
  if ((int32_t)cpu->gpr[7] <= 0) {                  /* blez a3 -> 0x8018B88C */
    cpu->gpr[2] = cpu->gpr[4];                      /* v0 = a0 */
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                                /* delay slot: t4 = 32 */
  if (cpu->gpr[6] != cpu->gpr[0])                   /* bnez a2 -> B768 */
    cpu->gpr[12] = cpu->gpr[12] + cpu->gpr[14];     /* t4 += t6 (56) */
  /* CPS fallthrough into 0x8018B76C — direct clean call on live regs. */
  return prim_persp_ot_emit(cpu->gpr[4], cpu->gpr[5], cpu->gpr[7],
                            cpu->gpr[12], cpu->gpr[15], cpu->gpr[24]);
}

/* ======== 0x8018B88C ======== */

/* prim_emit_ret_a0 — shared `move v0,a0; jr ra` epilogue piece of the
 * prim_persp_ot_emit family (guest 0x8018B88C, 8B). Reached by CPS
 * tail-dispatch from the family prologues when count <= 0.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018B88C. */
uint32_t prim_emit_ret_a0(void)
{
  CPUState *cpu = clean_cpu_state();
  cpu->gpr[2] = cpu->gpr[4];                        /* delay slot: v0 = a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018B894 ======== */

/* prim_persp_ot_emit_58 — perspective OT emitter, input stride 88
 * (0x58 = guest t7), out-stride 8 (guest t8), out-delta t4 = 24
 * (a2==0) else 24+32=56 (guest t6=32). a0 = record cursor (returned in
 * v0), a1 = OT base, a2 = delta select, a3 = signed count.
 *
 * Per record: six packed vertex words lwc2 into GTE V0/V1/V2 (regs
 * 0-5, alternating raw gte_data[]/gte_write_data forms); RTPT
 * (0x0280030); NCLIP; t9 = cfc2-ctrl31 & 0x7FC40000 — nonzero skips the
 * record entirely (no stores). Otherwise t3 = a0+t4+8; MAC0<=0 exits;
 * SXY0..2 screen-cull accumulate (sx via psx_ws_cull_sltiu on the low
 * halves, sy via arith>>16 < 480), SXY0 stored at out+8 in the sy-branch
 * DELAY SLOT (unconditional once past the MAC0 gate); on pass,
 * SXY1/SXY2 store at out+16/+24 via t3 += t8 steps, t3 recomputed to
 * base, FIXED OT link at a1 (lw a1, swl pair). a0 += 88 per element.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018B894. */
uint32_t prim_persp_ot_emit_58(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

block_8018B894:
  cpu->gpr[14] = 32;                                /* t6 = 32 */
  cpu->gpr[15] = 88;                                /* t7 = 88 */
  cpu->gpr[24] = 8;                                 /* t8 = 8  */
  /* fall into blez a3 */

block_8018B8A0:
  if ((int32_t)cpu->gpr[7] <= 0) goto block_8018B984;/* blez a3 */

block_8018B8A8:
  { int _bc = (cpu->gpr[6] == cpu->gpr[0]);            /* beq a2,zero */
    cpu->gpr[12] = 24;                               /* delay slot: t4=24 */
    if (_bc) goto block_8018B8B4; else goto block_8018B8B0; }

block_8018B8B0:
  cpu->gpr[12] = cpu->gpr[12] + cpu->gpr[14];       /* t4 += t6 (56) */
  /* fall through */

block_8018B8B4:                                     /* record loop head */
  cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
  gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
  cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
  gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
  cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
  gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
  cpu->gpr[7]  = cpu->gpr[7] + 0xFFFFFFFFu;           /* a3-- */
  gte_execute(cpu, 0x0280030u);                       /* RTPT */
  cpu->gpr[25] = 0x7FC4u << 16;                       /* t9 = 0x7FC40000 */
  cpu->gpr[2]  = gte_read_ctrl(cpu, 31);              /* cfc2 v0,ctrl31 */
  gte_execute(cpu, 0x1400006u);                       /* NCLIP */
  /* nop */
  cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];          /* t9 = flag & mask */
  { int _bc = (cpu->gpr[25] != cpu->gpr[0]);          /* bne t9,zero -> skip */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* delay: t3 = a0+t4 */
    if (_bc) goto block_8018B984; else goto block_8018B8F0; }

block_8018B8F0:
  cpu->gpr[11] = cpu->gpr[11] + 8;                    /* t3 += 8 (out) */
  cpu->gpr[9]  = cpu->gte_data[24];                   /* mfc2 t1, MAC0 */
  cpu->gpr[8]  = cpu->gte_data[12];                   /* mfc2 t0, SXY0 */
  cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;               /* delay: t9 = sxy0 lo */
  if ((int32_t)cpu->gpr[9] <= 0) goto block_8018B984; /* blez t1 */

block_8018B904:
  cpu->gpr[9]  = cpu->gte_data[13];                   /* mfc2 t1, SXY1 */
  cpu->gpr[10] = cpu->gte_data[14];                   /* mfc2 t2, SXY2 */
  cpu->gpr[25] = psx_ws_cull_sltiu(cpu->gpr[25], 512); /* sx cull sxy0 */
  cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
  cpu->gpr[13] = psx_ws_cull_sltiu(cpu->gpr[13], 512); /* sx cull sxy1 */
  cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
  cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
  cpu->gpr[13] = psx_ws_cull_sltiu(cpu->gpr[13], 512); /* sx cull sxy2 */
  cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
  { int _bc = (cpu->gpr[25] == cpu->gpr[0]);          /* beq sx_ok,zero */
    cpu->gpr[25] = (uint32_t)((int32_t)cpu->gpr[8] >> 16); /* delay: sxy0 hi */
    if (_bc) goto block_8018B97C; else goto block_8018B930; }

block_8018B930:
  cpu->gpr[25] = (cpu->gpr[25] < (uint32_t)480) ? 1u : 0u;  /* sltiu t9,480 */
  cpu->gpr[13] = (uint32_t)((int32_t)cpu->gpr[9] >> 16);
  cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
  cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
  cpu->gpr[13] = (uint32_t)((int32_t)cpu->gpr[10] >> 16);
  cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
  cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
  { int _bc = (cpu->gpr[25] == cpu->gpr[0]);          /* beq sy_ok,zero */
    br2_st32(cpu->gpr[11], cpu->gpr[8]);              /* delay: sw SXY0,(t3) */
    if (_bc) goto block_8018B97C; else goto block_8018B954; }

block_8018B954:
  cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];        /* t3 += t8 */
  br2_st32(cpu->gpr[11], cpu->gpr[9]);               /* sw SXY1,(t3) */
  cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];        /* t3 += t8 */
  br2_st32(cpu->gpr[11], cpu->gpr[10]);              /* sw SXY2,(t3) */
  cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];         /* t3 = a0+t4 (base) */
  cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);              /* old = *a1 (OT) */
  cpu->gpr[9]  = cpu->gpr[11] << 8;                  /* out << 8 */
  swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);           /* swl a1+2 */
  cpu->gpr[8]  = cpu->gpr[8] << 8;                   /* old << 8 */
  swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);          /* swl out+2 */
  /* fall through */

block_8018B97C:
  { int _bc = ((int32_t)cpu->gpr[7] > 0);            /* bgtz a3 */
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];        /* delay: a0 += t7 (88) */
    if (_bc) goto block_8018B8B4; else goto block_8018B984; }

block_8018B984:
  cpu->gpr[2] = cpu->gpr[4];                         /* delay: v0 = a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018BAE8 ======== */

/* prim_emit_ret_v0 — bare `nop; jr ra` piece (guest 0x8018BAE8, 8B):
 * returns v0 unchanged. Sibling epilogue piece of the emit family.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018BAE8. */
uint32_t prim_emit_ret_v0(void)
{
  CPUState *cpu = clean_cpu_state();
  return cpu->gpr[2];
}
