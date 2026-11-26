/* gfx.c — OAM/sprite attribute cluster (verbatim Ghidra ports).
 *
 * Guest globals bound via g_* arena pointers (bind.c).
 * Pointer-compressed fields (& 0xffffff tags) round-trip through
 * guest_of(); spr_desc_slot_alloc returns a host pointer (signature
 * widened from the guest's 32-bit return — sole caller is
 * spr_slot_pkt_submit in this file). */
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint32_t guest_sp(void);
extern CPUState *clean_cpu_state(void);
extern void setCopReg(int,int,uint32_t);
extern int  getCopReg(int,int);
extern void setCopControlWord(int,int,uint32_t);
extern int  getCopControlWord(int,int);
extern void copFunction(int,int);
extern void gte_vec3_normalize(void);
extern uint32_t br2_ld32p(uint32_t cell);
extern void     br2_st32p(uint32_t cell, uint32_t v);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st32(uint32_t a, uint32_t v);
extern uint8_t  br2_ld8(uint32_t a);
extern uint16_t br2_ld16(uint32_t a);
extern void     br2_st8(uint32_t a, uint8_t v);
extern void     br2_st16(uint32_t a, uint16_t v);

void gpu_load_image_cmd(uint32_t,uint32_t);
void prim_packet_init_tex(int,int,int,uint32_t,int8_t *);
uint8_t * guest_memcpy(uint32_t *,uint32_t *,int);
void printf_forward(const char *, ...);
void  guest_memset(uint32_t *,uint8_t,int);
void  cd_dispatch_tblc(void);
void  cd_timeout_arm(void);
int   cd_pump_watchdog(void);
void  bios_call_a0(uint32_t);
uint32_t gpu_mode_regs_apply(uint32_t);
void  gpu_prim_pkt_build(int,uint16_t *);
uint32_t gp_bde20_read(void);
uint32_t gpu_invoke(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void  gpu_rect_bounds_dbg(uint32_t,short *);

/* ---- guest globals ---- */
extern uint8_t *g_DAT_801b55dc;
extern uint8_t *g_DAT_801b55e0;
extern uint8_t *g_DAT_801b55e4;
extern uint8_t *g_DAT_801f642c;
extern uint8_t *g_DAT_801f68b4;
extern uint8_t *g__DAT_1f8000dc;
extern uint8_t *g_DAT_801e40b6;
extern uint8_t *g_DAT_801e40b7;
extern uint8_t *g_DAT_801b8320;
extern uint8_t *g_DAT_801b8321;
extern uint8_t *g_DAT_801b8322;
extern uint8_t *g_DAT_801b8323;
extern uint8_t *g_DAT_801b8324;
extern uint8_t *g_DAT_801b8326;
extern uint8_t *g_DAT_801b8330;
extern uint8_t *g_DAT_801b838c;
extern uint8_t *g_DAT_801b839c;
extern uint8_t *g_DAT_801b83e0;
extern char *s_LoadImage_80125504;
extern char *s_ClearImage_801254ec;
extern char *s_DrawOTag__08x_____80125558;
extern char *s_PutDispEnv__08x_____801255a0;
extern char *s_ClearImage2_801254f8;
extern char *s_ClearOTagR__08x__d_____80125540;
extern char *s_PutDrawEnv__08x_____8012556c;
extern char *s_ResetGraph_jtb__08x_env__08x_80125410;
extern char *s_ResetGraph__d_____80125430;
extern char *s_SetDispMask__d_____801254a0;
extern char *s_DrawSync__d_____801254b4;
extern char *s__Id__sys_c_v_1_140_1998_01_12_07_801b82d8;

#define spr_pool_b_next   (*(int32_t *)g_DAT_801b55dc)
#define spr_slot_next   (*(int32_t *)g_DAT_801b55e0)
#define spr_pool_a_next   (*(int32_t *)g_DAT_801b55e4)
#define spr_desc_tbl_a   (*(uint32_t *)g_DAT_801f642c)
#define spr_desc_tbl_b   (*(uint32_t *)g_DAT_801f68b4)
#define frame_tag  (*(uint32_t *)g__DAT_1f8000dc)
#define DAT_801e40b6   (*(uint8_t *)g_DAT_801e40b6)
#define DAT_801e40b7   (*(uint8_t *)g_DAT_801e40b7)
#define gpu_mode   (*(uint8_t  *)g_DAT_801b8320)
#define gpu_mode_state   (*(uint8_t  *)g_DAT_801b8321)
#define gpu_dbg_level   (*(uint8_t  *)g_DAT_801b8322)
#define DAT_801b8323   (*(uint8_t  *)g_DAT_801b8323)
#define disp_width   (*(uint16_t *)g_DAT_801b8324)
#define disp_height   (*(uint16_t *)g_DAT_801b8326)
#define draw_env   ((uint32_t *)guest_at(0x801b8330))
#define disp_env_shadow   ((uint32_t *)guest_at(0x801b838c))
#define disp_env_tail   (*(int32_t  *)guest_at(0x801b839c))
#define otag_r_tag   (*(uint32_t *)guest_at(0x801b83e0))

/* GPU func table: PTR_PTR_801b8318 -> 0x801b82d8 (verified in
 * payload). Entries are guest addrs of libgpu syscall impls;
 * dispatch via gpu_invoke (weak no-op until ported). */
#define GPU_TBL()  ((uint32_t *)guest_at(*(uint32_t *)guest_at(0x801b8318)))
static uint32_t gpu_call(uint32_t off, uint32_t a0, uint32_t a1,
                         uint32_t a2, uint32_t a3)
{
    uint32_t *t = GPU_TBL();
    return gpu_invoke(t[off >> 2], a0, a1, a2, a3);
}
#define GPU_ADDR(p) guest_of((const void *)(uintptr_t)(p))
/* PSX callers hand gpu_load_image_cmd a stack RECT (x,y,w,h s16 x4); the
 * port has no guest stack, so stage the 8-int8_t rect in the arg-build
 * area below the guest stack pointer (o32 sp-0x10..sp-1) — scratchpad
 * is only 0x400 bytes, so no fixed 0x1f8xxxx stand-in is valid. */
uint32_t gpu_rect(const void *p)
{
    memcpy(guest_at((uint32_t)(guest_sp() - 0x10)), p, 8);
    return (uint32_t)(guest_sp() - 0x10);
}
#define DB16(a) (*(uint16_t *)guest_at(a))
#define DB32(a) (*(uint32_t *)guest_at(a))
static void gpu_dbg(const char *fmt, ...)
{
    /* PTR_FUN_801b831c -> debug/trace printf (guest func addr) */
    uint32_t fn = *(uint32_t *)guest_at(0x801b831c);
    (void)fn; (void)fmt;
}
#define GPU_DBG(...) gpu_dbg(__VA_ARGS__)

/* ======== 0x8018bd50 ======== */

void gpu_pkt_unlink_n(uint32_t *head,int count)

{
  uint32_t word;
  uint32_t expect;
  uint32_t *prev;
  uint32_t *hp;
  uint32_t p1g, p1;
  
  /* head arrives as a host pointer from the clean_table wrapper
   * (guest_at(gpr4)) but as a raw guest-address value from the game.c/
   * menu.c callers — resolve either form. The link value walked below
   * is the guest address's low 24 bits. */
  p1g = guest_of(head);
  p1 = p1g ? p1g : (uint32_t)(uintptr_t)head;
  hp = (uint32_t *)guest_at(p1);
  expect = p1 & 0xffffff;
  do {
    word = *hp;
    expect = expect - 4;
    prev = hp;
    if (word == expect) {
      do {
        prev = prev + -1;
        word = *prev;
        count = count + -1;
        expect = expect - 4;
        if (count < 1) break;
      } while (word == expect);
      *hp = word;
    }
    count = count + -1;
    if (word == 0xffffff) {
      return;
    }
    hp = prev + -1;
    if (count < 1) {
      return;
    }
  } while( true );
}

/* ======== 0x8018bf40 ======== */

void spr_draw_off(int spr,short x,short y,int abs_coords)

{
  /* Guest builds one contiguous 8-int8_t RECT at sp+0x10..0x17
   * (sh @0x10,@0x12,@0x14,@0x16, disasm 0x8018bf40: frame 0x20) and
   * gpu_load_image_cmd reads it through generated GPU frames — stage it
   * in a reserved guest frame (sp+0x10 -> frm+0x10) or the callee's own
   * frame lands on the old sp-0x10 scratch window. */
  uint16_t rect[4];
  uint8_t *desc;
  
  /* spr is a guest address (callers pass the heap cursor
   * _DAT_801e3cfc); resolve before dereferencing. spr + 0x14
   * stays a guest address for gpu_load_image_cmd's a1. */
  desc = (uint8_t *)guest_at((uint32_t)spr);
  if ((*(uint32_t *)(desc + 4) & 8) != 0) {
    spr = spr + *(int *)(desc + 8);
    desc = (uint8_t *)guest_at((uint32_t)spr);
  }
  rect[0] = (uint16_t)x;
  rect[1] = (uint16_t)y;
  if (abs_coords == 0) {
    rect[0] = (uint16_t)(*(short *)(desc + 0xc) + x);
    rect[1] = (uint16_t)(*(short *)(desc + 0xe) + y);
  }
  rect[2] = *(uint16_t *)(desc + 0x10);
  rect[3] = *(uint16_t *)(desc + 0x12);
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t frm = br2_guest_frame(0x20);
    memcpy(guest_at(frm + 0x10), rect, 8);
    gpu_load_image_cmd(frm + 0x10,(uint32_t)(spr + 0x14));
    br2_guest_frame_free();
  }
  return;
}

/* ======== 0x8018bec0 ======== */

void spr_draw_cond(int spr,short x,short y,int abs_coords)

{
  uint16_t rect[4];
  uint8_t *desc;
  
  /* spr is a guest address; see spr_draw_off. */
  desc = (uint8_t *)guest_at((uint32_t)spr);
  if ((*(uint32_t *)(desc + 4) & 8) != 0) {
    rect[0] = (uint16_t)x;
    rect[1] = (uint16_t)y;
    if (abs_coords == 0) {
      rect[0] = (uint16_t)(*(short *)(desc + 0xc) + x);
      rect[1] = (uint16_t)(*(short *)(desc + 0xe) + y);
    }
    rect[2] = *(uint16_t *)(desc + 0x10);
    rect[3] = *(uint16_t *)(desc + 0x12);
    /* Same guest frame as spr_draw_off (0x8018bec0: frame 0x20, rect at
     * sp+0x10) — reserved frame staging, see spr_draw_off. */
    {
      extern uint32_t br2_guest_frame(uint32_t);
      extern void br2_guest_frame_free(void);
      uint32_t frm = br2_guest_frame(0x20);
      memcpy(guest_at(frm + 0x10), rect, 8);
      gpu_load_image_cmd(frm + 0x10,(uint32_t)(spr + 0x14));
      br2_guest_frame_free();
    }
  }
  return;
}

/* ======== 0x8018bfcc ======== */

void spr_draw_pair(uint32_t spr,uint32_t cx,uint32_t cy,uint32_t cflag,
                 uint32_t ox,uint32_t oy,uint32_t oflag)

{
  spr_draw_off(spr,ox,oy,oflag);
  spr_draw_cond(spr,cx,cy,cflag);
  return;
}

/* ======== 0x8018c034 ======== */

void spr_desc_tbl_a_init(void)

{
  int i;
  uint8_t *rec;
  
  if (spr_pool_b_next < 0) {
    i = 0x1f;
    rec = (uint8_t *)&spr_desc_tbl_a;
    do {
      i = i + -1;
      *(uint32_t *)rec = (uint32_t)(frame_tag - 2);
      rec[7] = 9;
      rec[0xb] = 0x2f;
      rec[8] = 0x80;
      rec[9] = 0x80;
      rec[10] = 0x80;
      rec = rec + -0x2c;
    } while (-1 < i);
    spr_pool_b_next = 0;
  }
  return;
}

/* ======== 0x8018c118 ======== */

void spr_desc_tbl_b_init(void)

{
  int i;
  uint8_t *rec;
  
  if (spr_slot_next < 0) {
    i = 0x1f;
    rec = (uint8_t *)&spr_desc_tbl_b;
    do {
      i = i + -1;
      *(uint32_t *)rec = (uint32_t)(frame_tag - 2);
      rec[7] = 4;
      rec[0xb] = 0x67;
      rec[8] = 0x80;
      rec[9] = 0x80;
      rec[10] = 0x80;
      rec = rec + -0x24;
    } while (-1 < i);
    spr_slot_next = 0;
  }
  return;
}

/* ======== 0x8018c18c ======== */

/* Guest returned a 32-bit guest pointer; widened to host pointer —
 * sole caller spr_slot_pkt_submit is in this file. */
void *spr_desc_slot_alloc(void)

{
  uint32_t off;
  int32_t owner;
  uint32_t *slot;
  int32_t next;
  
  off = (uint32_t)spr_slot_next * 0x24;
  slot = (uint32_t *)guest_at(0x801f6458 + off);
  owner = (int32_t)*slot;
  if ((owner != (int32_t)frame_tag) &&
      (next = spr_slot_next + 1, owner != (int32_t)(frame_tag - 1))) {
    if (0x1f < next) {
      next = 0;
    }
    spr_slot_next = next;
    *slot = frame_tag;
    return guest_at(0x801f645c + off);
  }
  return (void *)0;
}

/* ======== 0x8018c824 ======== */

uint32_t spr_slot_pkt_submit(uint32_t *ot_cell,uint32_t pos,uint32_t col,uint32_t attr)

{
  uint32_t *pkt;
  uint32_t ret;
  uint32_t pkt_g;
  uint32_t *cell;
  
  /* ot_cell carries either a pkt_g address value (direct clean callers
   * pass _DAT_1f800008+n) or a host pointer (clean_table.c wrapper passes
   * guest_at(gpr4)). guest_of() resolves the host form; fall back to the
   * raw value otherwise. */
  {
    uint32_t p1g = guest_of(ot_cell);
    cell = (uint32_t *)guest_at(p1g ? p1g : (uint32_t)(uintptr_t)ot_cell);
  }
  pkt = (uint32_t *)spr_desc_slot_alloc();
  if (pkt == (uint32_t *)0x0) {
    ret = 0xffffffff;
  }
  else {
    pkt_g = guest_of(pkt);
    *(uint16_t *)(pkt + 4) = ((uint16_t)((uint32_t)col >> 0x10) & 0xff) + 1;
    pkt[3] = attr;
    *(uint16_t *)((char *)pkt + 0x12) = (int8_t)((uint32_t)col >> 0x18) + 1;
    *(short *)(pkt + 2) = (short)pos;
    *(short *)((char *)pkt + 10) = (short)((uint32_t)pos >> 0x10);
    *(int8_t *)((char *)pkt + 7) = *(int8_t *)((char *)pkt + 7) & 0xfd | 1;
    pkt[1] = 0x64808080;
    prim_packet_init_tex((int)guest_of(pkt + 5),DAT_801e40b7,DAT_801e40b6,(int)(short)col,0);
    ret = 0;
    *pkt = *cell | 0x4000000;
    *cell = pkt_g & 0xffffff;
    pkt[5] = pkt_g & 0xffffff | 0x2000000;
    *cell = (pkt_g + 0x14) & 0xffffff;
  }
  return ret;
}

/* ======== GPU command wrappers (libgpu-style dispatch) ======== */

/* ======== 0x80191428 ======== */

void gpu_load_image_cmd(uint32_t rect,uint32_t src)

{
  gpu_rect_bounds_dbg(guest_of(s_LoadImage_80125504),(short *)(uintptr_t)rect);
  gpu_call(8,GPU_TBL()[8],rect,8,src);
  return;
}

/* ======== 0x80191300 ======== */

void gpu_clear_image_cmd(uint32_t rect,uint32_t r,uint32_t g,uint32_t b)

{
  gpu_rect_bounds_dbg(guest_of(s_ClearImage_801254ec),(short *)(uintptr_t)rect);
  gpu_call(8,GPU_TBL()[3],rect,8,
               (b & 0xff) << 0x10 | (g & 0xff) << 8 | r & 0xff);
  return;
}

/* ======== 0x80191390 ======== */

void gpu_clear_image_cmd2(uint32_t rect,uint32_t r,uint32_t g,uint32_t b)

{
  gpu_rect_bounds_dbg(guest_of(s_ClearImage2_801254f8),(short *)(uintptr_t)rect);
  gpu_call(8,GPU_TBL()[3],rect,8,
               (b & 0xff) << 0x10 | (g & 0xff) << 8 | 0x80000000 | r & 0xff);
  return;
}

/* ======== 0x80191668 ======== */

void gpu_clear_otag_r(uint32_t addr,uint32_t tag)

{
  if (1 < gpu_dbg_level) {
    GPU_DBG(s_ClearOTagR__08x__d_____80125540,addr,tag);
  }
  /* jalr t[11] with a0=addr,a1=tag (a2/a3 = caller leftovers in
   * guest; 0 here — ClearOTagR impl reads only a0/a1). */
  gpu_call(0x2c,addr,tag,0,0);
  otag_r_tag = 0x41b83cc;
  *(uint32_t *)guest_at(addr) = 0x1b83e0;
  return;
}

/* ======== 0x80191770 ======== */

void gpu_draw_otag(uint32_t otag)

{
  if (1 < gpu_dbg_level) {
    GPU_DBG(s_DrawOTag__08x_____80125558,otag);
  }
  gpu_call(8,GPU_TBL()[6],otag,0,0);
  return;
}

/* ======== 0x801917e0 ======== */

int gpu_draw_env_put(int env_g)

{
  uint8_t *envp;
  intptr_t hp;
  
  hp = (intptr_t)guest_at((uint32_t)env_g);
  if (1 < gpu_dbg_level) {
    GPU_DBG(s_PutDrawEnv__08x_____8012556c,env_g);
  }
  gpu_prim_pkt_build(env_g + 0x1c,(uint16_t *)(uintptr_t)env_g);
  *(uint32_t *)((char *)hp + 0x1c) = *(uint32_t *)((char *)hp + 0x1c) | 0xffffff;
  gpu_call(8,GPU_TBL()[6],env_g + 0x1c,0x40,0);
  guest_memcpy((uint32_t *)0x801b8330,(uint32_t *)env_g,0x5c);
  return env_g;
}

/* ======== 0x80191ed8 ======== */

uint32_t gpu_stat_sign(void)

{
  uint32_t stat;
  
  stat = gpu_call(0x38,0,0,0,0);
  return stat >> 0x1f;
}

/* ======== 0x801910e4 ======== */

void gpu_disp_mask_set(int enable)

{
  uint32_t cmd;
  
  if (1 < gpu_dbg_level) {
    GPU_DBG(s_SetDispMask__d_____801254a0,enable);
  }
  if (enable == 0) {
    guest_memset((uint32_t *)0x801b838c,0xff,0x14);
  }
  cmd = 0x3000001;
  if (enable != 0) {
    cmd = 0x3000000;
  }
  gpu_call(0x10,cmd,0,0,0);
  return;
}

/* ======== 0x8019117c ======== */

void gpu_draw_sync(uint32_t n)

{
  if (1 < gpu_dbg_level) {
    GPU_DBG(s_DrawSync__d_____801254b4,n);
  }
  /* jalr t[15] with a0=n (delay slot 0x801911D0: move a0,s0). */
  gpu_call(0x3c,n,0,0,0);
  return;
}

/* ======== 0x80190e00 ======== */

uint32_t gpu_graph_reset(uint32_t arg)

{
  uint32_t v;
  
  v = arg & 7;
  if (v == 3) {
LAB_80190e4c:
    printf_forward(s_ResetGraph_jtb__08x_env__08x_80125410,
                 (uint32_t *)0x801b82d8,(uint32_t *)0x801b8320);
LAB_80190e68:
    guest_memset((uint32_t *)0x801b8320,0,0x80);
    cd_dispatch_tblc();
    bios_call_a0((*(uint32_t *)guest_at(0x801b8318)) & 0xffffff);
    gpu_mode = (uint8_t)gpu_mode_regs_apply(arg);
    gpu_mode_state = 1;
    disp_width = *(uint16_t *)((char *)guest_at(0x801b83a0) + (uint32_t)gpu_mode * 4);
    disp_height = *(uint16_t *)((char *)guest_at(0x801b83ac) + (uint32_t)gpu_mode * 4);
    guest_memset((uint32_t *)0x801b8330,0xff,0x5c);
    guest_memset((uint32_t *)0x801b838c,0xff,0x14);
    v = (uint32_t)gpu_mode;
  }
  else {
    if (v < 4) {
      if (v == 0) goto LAB_80190e4c;
    }
    else if (v == 5) goto LAB_80190e68;
    if (1 < gpu_dbg_level) {
      GPU_DBG(s_ResetGraph__d_____80125430,arg);
    }
    /* jalr t[13] with a0=1 (delay slot 0x80190F5C: addiu a0,zero,1);
     * a1 = leftover (s1=arg on debug path, caller a1 otherwise). */
    v = gpu_call(0x34,1,0,0,0);
  }
  return v;
}

/* ======== 0x801919ac ======== */

/* Draw-env -> GPU display timing. param_1 is a guest address of the
 * 20-int8_t env struct (callers pass 0x801e40fc/0x801e624c-style addrs);
 * bridged to host here. Returns the host env pointer. */
uint32_t draw_env_timing(uint32_t env_g)

{
  uint16_t hgt;
  bool narrow;
  char gp_type;
  int tbl_off;
  int delta;
  uint32_t td;
  uint32_t ta;
  uint32_t tb;
  int std_idx;
  uint32_t tc;
  uint32_t tot;
  uint16_t *env;
  uint16_t *timing_tbl;
  uint8_t  *timing_b;
  
  env = (uint16_t *)guest_at(env_g);
  if (1 < gpu_dbg_level) {
    GPU_DBG(s_PutDispEnv__08x_____801255a0,env_g);
  }
  gpu_call(0x10,((env[1] & 0x3ff) << 10 | env[0] & 0x3ff | 0x5000000),0,0,0);
  if ((((DB32(0x801b839c) != *(int *)(env + 8)) || (DB16(0x801b838c) != env[0])) ||
      (DB16(0x801b838e) != env[1])) ||
     ((DB16(0x801b8390) != env[2] || (DB16(0x801b8392) != env[3])))) {
    gp_type = gp_bde20_read();
    *(char *)(env + 9) = gp_type;
    /* s0 = 0x08000000 unconditionally (lui @0x801919DC delay slot);
     * ori s0,s0,8 @0x80191AC0 only when gp_type==1. */
    tc = 0x08000000;
    if (gp_type == '\x01') {
      tc |= 8;
    }
    if (((uint8_t *)env)[0x11] != 0) {   /* lbu @env+0x11 (0x80191AC4) */
      tc = tc | 0x10;
    }
    if ((char)env[8] != '\0') {
      tc = tc | 0x20;
    }
    if (DAT_801b8323 != '\0') {
      tc = tc | 0x80;
    }
    hgt = env[2];
    if (0x118 < (short)hgt) {
      if ((short)hgt < 0x161) {
        tc = tc | 1;
      }
      else if ((short)hgt < 0x191) {
        tc = tc | 0x40;
      }
      else if ((short)hgt < 0x231) {
        tc = tc | 2;
      }
      else {
        tc = tc | 3;
      }
    }
    narrow = (short)env[3] < 0x121;
    if ((char)env[9] == '\0') {
      narrow = (short)env[3] < 0x101;
    }
    if (!narrow) {
      tc = tc | 0x24;
    }
    gpu_call(0x10,tc,0,0,0);
    *(uint8_t *)(env + 9) = 8;
  }
  if (((DB16(0x801b8394) == env[4]) && (DB16(0x801b8396) == env[5])) &&
     ((DB16(0x801b8398) == env[6] && ((DB16(0x801b839a) == env[7] && ((char)env[9] != '\b'))))))
   goto LAB_80191e74;
  gp_type = gp_bde20_read();
  *(char *)(env + 9) = gp_type;
  tc = (int)(short)env[5] + 0x13;
  if (gp_type == '\0') {
    tc = (int)(short)env[5] + 0x10;
  }
  tot = tc + (int)(short)env[7];
  if ((short)env[7] == 0) {
    tot = tc + 0xf0;
  }
  hgt = env[2];
  std_idx = 0;
  if ((((0x118 < (short)hgt) && (std_idx = 1, 0x160 < (short)hgt)) &&
      (std_idx = 2, 400 < (short)hgt)) && (std_idx = 4, (short)hgt < 0x231)) {
    std_idx = 3;
  }
  tbl_off = ((uint32_t)(int8_t)env[9] * 5 + std_idx) * 4;
  timing_tbl = (uint16_t *)guest_at(0x801b83f4);
  /* tbl_off is a BYTE offset into the u16 timing table (MIPS: lhu @base+tbl_off),
   * not a u16 element index — divide by 2 for timing_tbl[] indexing. */
  delta = (int)timing_tbl[tbl_off / 2 + 1] - (int)timing_tbl[tbl_off / 2];
  ta = (uint32_t)timing_tbl[tbl_off / 2] +
          (int)(short)env[4] * (uint32_t)(int8_t)((uint8_t *)guest_at(0x801b841c))[std_idx];
  if ((short)env[6] != 0) {
    delta = delta * (short)env[6] >> 8;
  }
  td = ta + delta;
  if ((char)env[9] == '\0') {
    tb = 500;
    if ((499 < (int)ta) && (tb = 0xcb2, (int)ta < 0xcb3)) {
      tb = ta;
    }
    ta = tb + (uint32_t)(int8_t)((uint8_t *)guest_at(0x801b841c))[std_idx] * 4;
    if (((int)ta <= (int)td) && (ta = 0xcda, (int)td < 0xcdb)) {
      ta = td;
    }
    if ((int)tc < 0x10) {
      td = 0x10;
    }
    else {
      td = 0x101;
      if ((int)tc < 0x102) {
        td = tc;
      }
    }
    tc = td + 2;
    narrow = (int)tot < 0x103;
    if ((int)tc <= (int)tot) {
      tc = 0x102;
      goto LAB_80191e0c;
    }
  }
  else {
    tb = 0x21c;
    if ((0x21b < (int)ta) && (tb = 0xc94, (int)ta < 0xc95)) {
      tb = ta;
    }
    ta = tb + (uint32_t)(int8_t)((uint8_t *)guest_at(0x801b841c))[std_idx] * 4;
    if (((int)ta <= (int)td) && (ta = 0xcbc, (int)td < 0xcbd)) {
      ta = td;
    }
    if ((int)tc < 0x13) {
      td = 0x13;
    }
    else {
      td = 0x12f;
      if ((int)tc < 0x130) {
        td = tc;
      }
    }
    narrow = (int)tot < 0x132;
    tc = td + 2;
    if ((int)(td + 2) <= (int)tot) {
      tc = 0x131;
LAB_80191e0c:
      if (narrow) {
        tc = tot;
      }
    }
  }
  gpu_call(0x10,(ta & 0xfff) << 0xc | tb & 0xfff | 0x6000000,0,0,0);
  gpu_call(0x10,(tc & 0x3ff) << 10 | td & 0x3ff | 0x7000000,0,0,0);
LAB_80191e74:
  guest_memcpy((uint32_t *)0x801b838c,(uint32_t *)env_g,0x14);
  return env_g;
}

/* ======== batch27: GPU/env helpers ======== */

extern uint32_t gpu_coord_clamp_a(uint32_t,uint32_t);
extern uint32_t gpu_coord_clamp_b(uint32_t,uint32_t);
extern uint32_t gpu_coord_pack(int,int);
extern uint32_t gpu_pkt_pack(uint8_t,char,uint16_t);
extern uint32_t gpu_texpage_pack(uint16_t *);
extern uint32_t cd_u16_swap_store(int);
extern uint32_t gpu_status_cmd(uint32_t);
extern void cd_chan_reg_init(int);
extern void cd_ext_init_once(void);
extern void cd_u16_table_store(int,uint32_t,int);
uint32_t bios_call(uint32_t,uint32_t);

extern char *s__s_bad_RECT_801254c8;
extern char *s___d__d____d__d__801254d4;
extern char *s__Id__sys_c_v_1_140_1998_01_12_07_801b82d8;

#define GP32(a)   (*(uint32_t *)guest_at(a))
#define GP8(a)    (*(uint8_t  *)guest_at(a))
#define GP16(a)   (*(uint16_t *)guest_at(a))
#define GPP(a)    ((uint32_t *)guest_at(a))
#define GPTRW(a)  (*(uint32_t *)guest_at(GP32(a)))

/* ======== 0x80193d08 ======== */

void guest_memset(uint32_t *dst,uint8_t val,int len)

{
  int i;
  /* dst: host pointer from clean_table wrapper, guest-address value
   * from in-file callers (0x801b83xx constants) — resolve either form. */
  uint32_t g1 = guest_of(dst);
  uint8_t *d = (uint8_t *)guest_at(g1 ? g1 : (uint32_t)(uintptr_t)dst);
  
  i = len + -1;
  if (len != 0) {
    do {
      *d = val;
      i = i + -1;
      d = d + 1;
    } while (i != -1);
  }
  return;
}

/* ======== 0x8018e490 ======== */

uint8_t * guest_memcpy(uint32_t *dst,uint32_t *src,int len)

{
  uint8_t c;
  uint8_t *wp;
  uint8_t *ret;
  uint8_t *d, *s;
  
  ret = (uint8_t *)0x0;
  /* params arrive as host pointers from the clean_table wrapper but as
   * guest-address values from cd.c callers — resolve either form. */
  {
    uint32_t g1 = guest_of(dst);
    uint32_t g2 = guest_of(src);
    d = (uint8_t *)guest_at(g1 ? g1 : (uint32_t)(uintptr_t)dst);
    s = (uint8_t *)guest_at(g2 ? g2 : (uint32_t)(uintptr_t)src);
  }
  wp = d;
  if (dst != (uint32_t *)0x0) {
    for (; ret = (uint8_t *)d, 0 < len; len = len + -1) {
      c = *s;
      s = s + 1;
      *wp = c;
      wp = (uint8_t *)(wp + 1);
    }
  }
  return ret;
}

/* ======== 0x801921c0 ======== */

void gpu_prim_pkt_build(int pkt_g,uint16_t *env_p)

{
  uint16_t h_raw;
  uint16_t w;
  uint16_t h;
  uint32_t word;
  char tag;
  uint32_t dxy;
  uint32_t wh;
  uint8_t *dst;
  uint16_t *env;
  
  /* env_p: host pointer from clean_table wrapper, guest-address value
   * from gpu_draw_env_put — resolve either form. pkt_g stays a guest
   * address value in both paths. */
  uint32_t p2g = guest_of(env_p);
  dst = (uint8_t *)guest_at((uint32_t)pkt_g);
  env = (uint16_t *)guest_at(p2g ? p2g : (uint32_t)(uintptr_t)env_p);
  word = gpu_coord_clamp_a((int)(short)env[0],(int)(short)env[1]);
  *(uint32_t *)(dst + 4) = word;
  word = gpu_coord_clamp_b((int)(((uint32_t)env[2] + (uint32_t)env[0] + -1) * 0x10000) >> 0x10,
                       (int)(((uint32_t)env[1] + (uint32_t)env[3] + -1) * 0x10000) >> 0x10);
  *(uint32_t *)(dst + 8) = word;
  word = gpu_coord_pack((int)(short)env[4],(int)(short)env[5]);
  *(uint32_t *)(dst + 0xc) = word;
  word = gpu_pkt_pack(*(uint8_t *)((char *)env + 0x17),(char)env[0xb],env[10]);
  *(uint32_t *)(dst + 0x10) = word;
  word = gpu_texpage_pack(env + 6);
  *(uint32_t *)(dst + 0x14) = word;
  *(uint32_t *)(dst + 0x18) = 0xe6000000;
  tag = '\a';
  if ((char)env[0xc] != '\0') {
    word = *(uint32_t *)env;
    h = env[2];
    h_raw = env[3];
    w = 0;
    if ((-1 < (short)h) && (w = disp_width - 1, (int)(short)h <= disp_width + -1)) {
      w = h;
    }
    if ((short)h_raw < 0) {
      h = 0;
    }
    else {
      h = disp_height - 1;
      if ((int)(short)h_raw <= disp_height + -1) {
        h = h_raw;
      }
    }
    wh = ((uint32_t)h << 16) | w;
    if (((env[0] & 0x3f) == 0) && ((w & 0x3f) == 0)) {
      tag = '\n';
      *(uint32_t *)(dst + 0x1c) =
           (uint32_t)*(uint8_t *)((char *)env + 0x1b) << 0x10 | (uint32_t)(uint8_t)env[0xd] << 8 | 0x2000000
           | (uint32_t)*(uint8_t *)((char *)env + 0x19);
      *(uint32_t *)(dst + 0x20) = word;
      *(uint32_t *)(dst + 0x24) = wh;
    }
    else {
      tag = '\n';
      dxy = ((uint32_t)(uint16_t)(env[1] - env[5]) << 16) | (uint16_t)(env[0] - env[4]);
      *(uint32_t *)(dst + 0x1c) =
           (uint32_t)*(uint8_t *)((char *)env + 0x1b) << 0x10 | (uint32_t)(uint8_t)env[0xd] << 8 | 0x60000000
           | (uint32_t)*(uint8_t *)((char *)env + 0x19);
      *(uint32_t *)(dst + 0x20) = dxy;
      *(uint32_t *)(dst + 0x24) = wh;
    }
  }
  *(char *)(dst + 3) = tag + -1;
  return;
}

/* ======== 0x80193424 ======== */

/* gpu_mode_regs_apply — GPU mode/register apply @0x80193424 (336 bytes,
 * 15 blocks). Faithful clean port; every callee is already clean:
 * cd_u16_swap_store (0x8019A7FC), guest_memset (0x80193D08),
 * gpu_status_cmd (0x80193828). All values are guest u32s.
 *
 * Prologue side effects (all modes): *0x801B845C = 0; first
 * cd_u16_swap_store(0) result saved @0x801B8468; *0x801B8458 =
 * *0x801B845C (the just-zeroed cell -> 0).
 *
 * mode = a0 & 7 dispatch (verified against the slti/beq tree):
 *   {0,3,5}: **(u32*)0x801B8444 = 0x401; **(u32*)0x801B8454 |= 0x800;
 *            **(u32*)0x801B8438 = 0 (in the jal delay slot);
 *            guest_memset(0x801FAD68,0,0x100);
 *            guest_memset(0x801FAE70,0,0x1800).
 *   {1}:     same two register writes; **(u32*)0x801B8438 = 0x02000000
 *            then = 0x01000000 (first store is dead — faithful).
 *   {2,4,6,7}: no register writes.
 * Tail (all modes): v0 = cd_u16_swap_store(*0x801B8468); if mode==0
 * v0 = gpu_status_cmd(a0). Returns last call's $v0. */
uint32_t gpu_mode_regs_apply(uint32_t arg)
{
  uint32_t mode = arg & 7u;
  uint32_t v0 = cd_u16_swap_store(0);
  uint32_t p;
  uint32_t r;
  GP32(0x801B845C) = 0;
  GP32(0x801B8468) = v0;
  GP32(0x801B8458) = GP32(0x801B845C);
  if (mode == 0 || mode == 3 || mode == 5) {
    p = GP32(0x801B8444); GP32(p) = 0x401;
    p = GP32(0x801B8454); GP32(p) = GP32(p) | 0x800;
    p = GP32(0x801B8438); GP32(p) = 0;
    guest_memset((uint32_t *)0x801FAD68, 0, 0x100);
    guest_memset((uint32_t *)0x801FAE70, 0, 0x1800);
  } else if (mode == 1) {
    p = GP32(0x801B8444); GP32(p) = 0x401;
    p = GP32(0x801B8454); GP32(p) = GP32(p) | 0x800;
    p = GP32(0x801B8438); GP32(p) = 0x02000000;
    p = GP32(0x801B8438); GP32(p) = 0x01000000;
  }
  r = cd_u16_swap_store((int)GP32(0x801B8468));
  if (mode == 0) r = gpu_status_cmd(arg);
  return r;
}

/* ======== 0x80193d30 ======== */

void bios_call_a0(uint32_t arg)

{
  /* guest 0x80193d30: `li t2,0xa0; jr t2; li t1,0x49` — BIOS svc A0/0x49
   * with a0 = arg (caller's arg register). */
  extern uint32_t bios_svc6(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
  bios_svc6(0xa0,0x49,arg,0,0,0);
  return;
}

/* ======== 0x801911e4 ======== */

void gpu_rect_bounds_dbg(uint32_t name,short *rect)

{
  int y;
  char *msg;
  int x;
  /* rect arrives as a host pointer from the clean_table wrapper
   * (guest_at(gpr5)) but as a guest-address value from gpu_load_image_cmd
   * family callers — resolve either form. */
  uint32_t p2g = guest_of(rect);
  short *rc = (short *)guest_at(p2g ? p2g : (uint32_t)(uintptr_t)rect);
  
  if (gpu_dbg_level == '\x01') {
    x = (int)rc[2];
    if ((x <= disp_width) && (x + rc[0] <= (int)disp_width)) {
      y = (int)rc[1];
      if (((y <= disp_height) &&
          (((y + rc[3] <= (int)disp_height && (0 < x)) && (-1 < rc[0])))) &&
         ((-1 < y && (0 < rc[3])))) {
        return;
      }
    }
    msg = s__s_bad_RECT_801254c8;
  }
  else {
    if (gpu_dbg_level != '\x02') {
      return;
    }
    msg = (char *)guest_at(0x801254e8);
  }
  gpu_dbg(msg,name);
  gpu_dbg(s___d__d____d__d__801254d4,(int)rc[0],(int)rc[1],(int)rc[2],
              (int)rc[3]);
  return;
}

/* ======== 0x8019b1e4 ======== */

uint32_t gp_bde20_read(void)

{
  return GP32(0x801bde20);
}

/* ======== 0x8019b220 ======== */

void cd_chan_boot_init(int chan)

{
  /* Keep clean: it must reach cd_ext_init_once() so the CD
   * OpenEvent(0xF0000009) init path runs. Delegating this body (and
   * cd_chan_boot_fwd) to generated was tried; the generated CPS path did not
   * dispatch cd_ext_init_once either, leaving the guard unset. */
  uint16_t *w;
  int i;
  uint16_t *base;
  
  cd_dispatch_tblc();
  cd_chan_reg_init(chan);
  if (chan == 0) {
    i = 0x17;
    base = (uint16_t *)guest_at(0x801bde9a);
    w = base;
    do {
      *w = 0xc000;
      i = i + -1;
      w = w + -1;
    } while (-1 < i);
  }
  cd_ext_init_once();
  GP32(0x801bde44) = 0;
  GP32(0x801bde48) = 0;
  GP32(0x801bde54) = 0;
  GP32(0x801bde58) = 0;
  GP16(0x801bde5a) = 0;
  GP32(0x801bde5c) = 0;
  GP32(0x801bde60) = 0;
  GP32(0x801bde4c) = GP32(0x801be440);
  cd_u16_table_store(0xd1,GP32(0x801be440),0);
  GP32(0x801bdf10) = 0;
  GP32(0x801bdf14) = 0;
  GP32(0x801bdf18) = 0;
  GP32(0x801bde40) = 0;
  GP32(0x801bdecc) = 0;
  GP32(0x801bde3c) = 0;
  GP32(0x801bde68) = 0;
  GP32(0x801bde64) = 0;
  GP32(0x801bde9c) = 0;
  return;
}

/* ---- batch28: printf-forwarder + sprite-attr inits ---- */
extern uint32_t printf_core(uint32_t,const uint8_t *,const uint32_t *);

/* printf_forward — printf-style forwarder: packs up to 3 varargs and
 * calls printf_core(1, fmt, &args). Verbatim from decompile. */
void printf_forward(const char *fmt, ...)
{
  uint32_t args[8];
  va_list ap;
  int i;
  va_start(ap,fmt);
  for (i = 0; i < 8; i++) {
    uintptr_t v = va_arg(ap,uintptr_t);
    uint32_t g = guest_of((const void *)v);
    args[i] = g ? g : (uint32_t)v;
  }
  va_end(ap);
  printf_core(1,fmt,args);
}

/* sprite_attr_init_a — sprite/OAM attribute block init (u16 fields + tail
 * bytes). param_1 = host pointer to the attr struct. */
void sprite_attr_init_a(uint32_t attr_g,uint16_t x,uint16_t y,
                  uint16_t w,int32_t h)
{
  int sys;
  uint16_t *p1 = (uint16_t *)guest_at(attr_g);
  uint8_t *b = (uint8_t *)p1;
  sys = gp_bde20_read();
  p1[0] = x;
  p1[1] = y;
  p1[2] = w;
  p1[6] = 0;
  p1[7] = 0;
  p1[8] = 0;
  p1[9] = 0;
  b[0x19] = 0;
  b[0x1a] = 0;
  b[0x1b] = 0;
  b[0x16] = 1;
  p1[3] = (int16_t)h;
  b[0x17] = (uint8_t)(sys == 0 ? h < 0x101 : h < 0x121);
  p1[4] = x;
  p1[5] = y;
  p1[10] = 10;
  b[0x18] = 0;
}

/* sprite_attr_init_b — second sprite-attr variant init. */
void sprite_attr_init_b(uint32_t attr_g,uint16_t x,uint16_t y,
                  uint16_t w,uint16_t aux)
{
  uint16_t *p1 = (uint16_t *)guest_at(attr_g);
  uint8_t *b = (uint8_t *)p1;
  p1[0] = x;
  p1[1] = y;
  p1[2] = w;
  p1[4] = 0;
  p1[5] = 0;
  p1[6] = 0;
  p1[7] = 0;
  b[0x11] = 0;
  b[0x10] = 0;
  b[0x13] = 0;
  b[0x12] = 0;
  p1[3] = aux;
}

/* ---- batch29: sprite-attr bit helpers (guest-addr convention) ---- */

/* bitfield_or_write — unaligned bitfield OR-write: inserts param_2 (8-bit
 * value) at bit offset derived from param_1 alignment. param_1 is a
 * host pointer (sole caller hud_sprite_desc_init passes arena ptrs). */
void bitfield_or_write(const void *p,int val)
{
  uintptr_t a = (uintptr_t)p;
  uint32_t align = (uint32_t)(a + 2) & 3;
  uint32_t *wp = (uint32_t *)((a + 2) - align);
  *wp = *wp & (0xffffffffu << ((align + 1) * 8)) |
           (uint32_t)((uint32_t)val << 8) >> ((3 - align) * 8);
}

/* spr_desc_a/4060/4090 — per-slot sprite descriptor helpers.
 * param_1 = guest address of a 0x28-int8_t slot record. */
void spr_desc_a(uint32_t slot_g)
{
  uint8_t *p = (uint8_t *)guest_at(slot_g);
  p[3] = 9;
  p[7] = 0x2c;
}

void spr_desc_b(uint32_t slot_g,uint32_t on)
{
  uint8_t *p = (uint8_t *)guest_at(slot_g);
  if (on == 0) p[7] &= 0xfd;
  else               p[7] |= 2;
}

void spr_desc_c(uint32_t slot_g,uint32_t on)
{
  uint8_t *p = (uint8_t *)guest_at(slot_g);
  if (on == 0) p[7] &= 0xfe;
  else               p[7] |= 1;
}

/* ---- batch29: text-window/string-widget cluster ----
 * All struct fields addressed by guest offset via guest_at. */
#define GB8(a)   (*(uint8_t  *)guest_at(a))
#define GB16(a)  (*(uint16_t *)guest_at(a))
#define GB32(a)  (*(uint32_t *)guest_at(a))
#define GB16S(a) (*(int16_t  *)guest_at(a))
#define GB32S(a) (*(int32_t   *)guest_at(a))

extern uint32_t gpu_sprite_rect_emit(uint32_t,uint32_t,uint32_t);
extern uint32_t gpu_sprite_rect_emit_mode(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void     guest_memset_b(uint32_t,uint32_t,uint32_t);
extern int32_t  text_win_printf(int32_t,uint32_t,uint32_t,uint32_t,...);
extern void     gpu_pkt_tag_60(uint32_t);
extern void     gpu_pkt_tag_74(uint32_t);
extern void    *PTR_FUN_801b831c;

/* text_win_init — text window init: two draw-env installs + clear. */
void text_win_init(uint32_t w,int32_t h)
{
  GB32(0x801fad12) = gpu_sprite_rect_emit(0x801b78c8,w,(uint32_t)(h + 0x80));
  GB32(0x801fad10) = gpu_sprite_rect_emit_mode(0x801b7ac8,0,0,w,(uint32_t)h,0x80,0x20);
  GB8(0x801b78c0) = 0;
  guest_memset_b(0x801b7740,0,0x180);
}

/* text_line_select — select active text line (bounds vs DAT_801b78c0). */
void text_line_select(int32_t line)
{
  if ((0 <= line) && (line <= (int32_t)GB8(0x801b78c0))) {
    PTR_FUN_801b831c = (void *)text_win_printf;
    GB8(0x801b78c4) = (uint8_t)line;
  }
}

/* text_widget_alloc — allocate one 0x30-int8_t text-string widget slot.
 * Returns slot index or -1 when DAT_801b78c0 >= 8. */
int32_t text_widget_alloc(uint16_t x,uint16_t y,int32_t attr,
                     uint16_t aux,int32_t mode,int32_t count)
{
  int32_t i, tmp;
  /* guest sp-0x30..-0x28 is one contiguous 8-int8_t texcoord buffer read by
   * prim_packet_init_tex as bytes 0..7 (mode[0..2] + shorts +4/+6).
   * Ghidra split the four halfwords into scalars; keep them contiguous. */
  uint16_t uv[4];
  uint32_t slot;

  if ((int32_t)GB8(0x801b78c0) < 8) {
    if (GB8(0x801b78c0) == 0) GB32(0x801b82c8) = 0;
    slot = GB8(0x801b78c0) * 0x30;
    GB32(0x801b7740 + slot + 0x2c) = (attr == 0);
    tmp = (int32_t)GB32(0x801b82c8);
    if (0x400 < tmp + count) count = 0x400 - tmp;
    uv[2] = 0x100; uv[3] = 0x100;
    uv[0] = 0;     uv[1] = 0;
    prim_packet_init_tex((int)(0x801b7750 + slot),0,0,(uint32_t)GB32(0x801fad10),
                 (int8_t *)uv);
    if (mode != 0) {
      gpu_pkt_tag_60(0x801b7740 + slot);
      GB8(0x801b7744 + slot) = 0;
      GB8(0x801b7745 + slot) = 0;
      GB8(0x801b7746 + slot) = 0;
      spr_desc_b(0x801b7740 + slot,(mode == 2));
    }
    tmp = (int32_t)GB32(0x801b82c8);
    slot = GB8(0x801b78c0) * 0x30;
    GB16(0x801b7748 + slot) = x;
    GB16(0x801b774a + slot) = y;
    GB16S(0x801b774c + slot) = (int16_t)attr;
    GB16(0x801b774e + slot) = aux;
    GB32S(0x801b775c + slot) = count;
    GB32(0x801b7768 + slot) = 0;
    GB32S(0x801b7764 + slot) = tmp + (int32_t)0x801f6910;
    GB32S(0x801b7760 + slot) = tmp * 0x10 + (int32_t)0x801f6d10;
    *(uint8_t *)guest_at(GB32(0x801b7764 + slot)) = 0;
    tmp = GB32S(0x801b7760 + GB8(0x801b78c0) * 0x30);
    i = 0;
    if (0 < count) {
      do {
        gpu_pkt_tag_74((uint32_t)tmp);
        i = i + 1;
        GB16((uint32_t)tmp + 0xe) = GB16(0x801fad12);
        tmp += 0x10;
      } while (i < count);
    }
    GB32(0x801b82c8) = GB32(0x801b82c8) + (uint32_t)count;
    tmp = GB8(0x801b78c0);
    GB8(0x801b78c0) = GB8(0x801b78c0) + 1;
  } else {
    tmp = -1;
  }
  return tmp;
}

/* spr_attr_alloc_a — sprite-attr slot alloc with 0x5000000/0x28000000|
 * 0x2a000000 tag packing (same cluster as spr_desc_slot_alloc/c824).
 * param_1 = guest address of the head pointer cell. */
extern void *spr_pool_a_alloc(void);
uint32_t spr_attr_alloc_a(uint32_t head_g,int16_t x0,int16_t y0,
                      int16_t w,int16_t h,uint32_t tag,
                      int32_t variant)
{
  uint32_t *pkt;
  uint32_t ret;
  int32_t tex_ofs;
  pkt = (uint32_t *)spr_pool_a_alloc();
  if (pkt == (uint32_t *)0x0) {
    ret = 0xffffffff;
  } else {
    uint8_t *b = (uint8_t *)pkt;
    *(int16_t *)(b + 8) = x0;
    *(int16_t *)(b + 10) = y0;
    *(int16_t *)(b + 12) = (int16_t)(x0 + w);
    *(int16_t *)(b + 14) = y0;
    *(int16_t *)(b + 16) = x0;
    *(int16_t *)(b + 18) = (int16_t)(x0 + w);
    *(int16_t *)(b + 22) = (int16_t)(y0 + h);
    *(int16_t *)(b + 26) = (int16_t)(y0 + h);
    if (variant == 0) {
      pkt[1] = tag | 0x28000000;
      tex_ofs = 0;
    } else {
      pkt[1] = tag | 0x2a000000;
      tex_ofs = (variant + -1) * 0x20;
    }
    prim_packet_init_tex((int)guest_of(pkt + 6),GB8(0x801e40b7),GB8(0x801e40b6),tex_ofs,0);
    ret = 0;
    uint32_t *head = (uint32_t *)guest_at(head_g);
    /* OT link word goes into the packet's first word (node[0] =
     * old_head | 0x05000000), NOT into the head cell — writing *head
     * here leaves node[0] uninitialised and the GPU linked-list DMA
     * follows a garbage next-pointer (versus/attract OT runaway). */
    pkt[0] = *head | 0x05000000u;
    *head = guest_of(pkt) & 0xffffff;
    pkt[6] = guest_of(pkt) & 0xffffff | 0x2000000;
    *head = guest_of(pkt + 6) & 0xffffff;
  }
  return ret;
}

/* spr_attr_alloc_b — sprite-attr slot alloc variant: fixed tag
 * 0x2c808080, *param_1 head cell gets 0x9000000 tag. Same
 * guest-addr + pointer-tag conventions as spr_attr_alloc_a. */
extern void *spr_pool_b_alloc(void);
uint32_t spr_attr_alloc_b(uint32_t head_g,uint32_t pxy,uint32_t pc,
                      uint32_t prgb,uint16_t tu,uint16_t tv)
{
  uint32_t *pkt;
  uint32_t ret;
  uint16_t yh;
  int8_t ca, cb;
  pkt = (uint32_t *)spr_pool_b_alloc();
  if (pkt == (uint32_t *)0x0) {
    ret = 0xffffffff;
  } else {
    uint8_t *b = (uint8_t *)pkt;
    ret = 0;
    cb = (int8_t)prgb + (int8_t)(pc >> 16) + 1;
    ca = (int8_t)(prgb >> 8);
    b[0x1c] = (uint8_t)prgb;
    b[0x15] = (uint8_t)ca;
    ca = (int8_t)(ca + (pc >> 24) + 1);
    pkt[3] = prgb;                    /* int8_t 0x0c */
    *(int16_t *)(b + 0x16) = (int16_t)pc;
    b[0x14] = (uint8_t)cb;
    b[0x1d] = (uint8_t)ca;
    b[0x24] = (uint8_t)cb;
    b[0x25] = (uint8_t)ca;
    *(int16_t *)(b + 0x08) = (int16_t)pxy;
    yh = (uint16_t)(pxy >> 16);
    *(uint16_t *)(b + 0x0a) = yh;
    *(uint16_t *)(b + 0x10) = tu;
    *(uint16_t *)(b + 0x12) = yh;
    *(int16_t *)(b + 0x18) = (int16_t)pxy;
    *(uint16_t *)(b + 0x1a) = tv;
    *(uint16_t *)(b + 0x20) = tu;
    *(uint16_t *)(b + 0x22) = tv;
    pkt[1] = 0x2c808080;
    uint32_t *head = (uint32_t *)guest_at(head_g);
    /* node[0] = old_head | 0x09000000 (link word into the packet, not
     * the head cell) — same fix as spr_attr_alloc_a. */
    pkt[0] = *head | 0x09000000u;
    *head = guest_of(pkt) & 0xffffff;
  }
  return ret;
}

/* hud_gauge_render — HUD gauge/frame sprite renderer. param_1 = guest
 * addr of fighter-state block (+0xc attr u32, +0x20/+0x22 s16 x/y);
 * param_2 = guest addr of sprite-list head cell; param_3 low nibble =
 * palette set, bits 4-7 = layout variant. Emits spr_attr_alloc_b/8c824/
 * 8ce70/8c92c sprite records + hud_text_d tile blits. */
extern void hud_text_d(uint32_t,uint32_t,const void *,int32_t,int32_t,int32_t);
extern void sprite_node_emit_a(uint32_t,int32_t,int32_t,int32_t,int32_t,uint32_t,uint32_t,uint32_t,uint32_t,int32_t);
extern void sprite_node_emit_b(uint32_t,uint32_t,uint32_t,uint32_t,int32_t,int32_t);
void hud_gauge_render(uint32_t st_g,uint32_t head_g,uint32_t variant)
{
  int16_t x;
  int32_t cell, v;
  uint32_t attr, col_a, col_b, col_c, col_d;
  uint8_t *st = (uint8_t *)guest_at(st_g);
  attr = *(uint32_t *)(st + 0xc);
  switch (variant & 0xf) {
  default:
    col_d = 0xc; col_c = 0x20; col_a = 0x30; col_b = 0x40;
    goto LAB_8012c4f8;
  case 1:
    col_d = 0; col_c = 0x1000; col_a = 0x3000; col_b = 0x3000;
    v = 1;
    break;
  case 2:
    col_d = 0x100000; col_c = 0x300000; col_a = 0x400000; col_b = 0x600000;
    v = 2;
    break;
  case 3:
    col_d = 0x101010; col_c = 0x202020; col_a = 0x303030; col_b = 0x404040;
    v = 3;
    break;
  case 4:
    col_d = 0x8121c; col_c = 0x121c26; col_a = 0x1c2630; col_b = 0x26303a;
    v = 4;
    break;
  case 5:
    col_d = 0xc000c; col_c = 0x200020; col_a = 0x300030; col_b = 0x400040;
    v = 5;
    break;
  case 6:
    col_d = 0x408; col_c = 0xc18; col_a = 0x1020; col_b = 0x1830;
    v = 6;
    break;
  case 7:
    col_d = 0x80805; col_c = 0x18180f; col_a = 0x202014; col_b = 0x30301c;
    v = 7;
    break;
  case 8:
    col_d = 0; col_c = 0x80; col_a = 0x800000; col_b = 0x200000;
    goto LAB_8012c4f8;
  case 9:
    col_d = 0x800000; col_c = 0; col_a = 0x80; col_b = 0x20;
LAB_8012c4f8:
    v = 0;
  }
  variant = variant & 0xf0;
  if (variant == 0x20) {
LAB_8012c64c:
    cell = head_g + 4;
    spr_attr_alloc_b(cell,0x280000,0x177e001e,0x7ad68080,0x80,0x48);
    spr_attr_alloc_b(cell,0x280080,0x177e001e,0x7a968080,0x100,0x48);
    spr_attr_alloc_b(cell,0x280100,0x177e001e,0x7a568080,0x180,0x48);
    spr_attr_alloc_b(cell,0x280180,0x1780001e,0x7a5680ff,0x200,0x48);
    spr_slot_pkt_submit((uint32_t *)guest_at(cell),(*(int16_t *)(st + 0x22) + -0x54) * 0x10000 |
                 ((uint32_t)*(int16_t *)(st + 0x20) - 0x90U) & 0xffff,0x277e001e,
                 ((uint32_t)(v << 6) | 0x7e16U) << 0x10 | 0x9880);
    sprite_node_emit_a(cell,0,*(int16_t *)(st + 0x22) + -0x40,(int32_t)*(int16_t *)(st + 0x20),0x18,
                 col_a,0,col_a,0,2);
    sprite_node_emit_b(cell,((uint32_t)(*(int16_t *)(st + 0x22) + -0x13f) * 0x10000) |
                 ((uint32_t)*(int16_t *)(st + 0x20) - 0xffU) & 0xffff,0xfefe001f,
                 ((uint32_t)(v << 6) | 0x7c16U) << 0x10,0x80,1);
LAB_8012cb64:
    x = *(int16_t *)(st + 0x20);
    v = (int32_t)*(int16_t *)(st + 0x22);
    cell = 0;
  } else {
    if (variant < 0x21) {
      if (variant == 0) {
        cell = head_g + 4;
        hud_text_d(cell,attr,guest_at(0x8011f10c),0xc0,
                     (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x38) * 0x10000) >> 0x10,0);
        spr_slot_pkt_submit((uint32_t *)guest_at(cell),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0xd8,0xf0f001e,0x7a16d830);
        spr_slot_pkt_submit((uint32_t *)guest_at(cell),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0xe8,0xf0f001e,0x7a16d820);
        spr_slot_pkt_submit((uint32_t *)guest_at(cell),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0xf8,0xf0f001e,0x7a16d810);
        hud_text_d(cell,attr,guest_at(0x8011f110),0x120,
                     (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x38) * 0x10000) >> 0x10,0);
        spr_slot_pkt_submit((uint32_t *)guest_at(cell),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0x148,0xf0f001e,0x7a16d800);
        goto LAB_8012c64c;
      }
      if (variant != 0x10) return;
      hud_text_d(head_g + 4,attr,guest_at(0x8011f110),0x120,
                   (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x30) * 0x10000) >> 0x10,0);
      spr_slot_pkt_submit((uint32_t *)guest_at(head_g + 4),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0x148,0xf0f001e,0x7a16d800);
    } else {
      if (variant == 0x30) {
        cell = head_g + 4;
        spr_attr_alloc_b(cell,0x280000,0x177e001e,0x7ad68080,0x80,0x48);
        spr_attr_alloc_b(cell,0x280080,0x177e001e,0x7a968080,0x100,0x48);
        spr_attr_alloc_b(cell,0x280100,0x177e001e,0x7a568080,0x180,0x48);
        spr_attr_alloc_b(cell,0x280180,0x1780001e,0x7a5680ff,0x200,0x48);
        spr_slot_pkt_submit((uint32_t *)guest_at(cell),((uint32_t)(*(int16_t *)(st + 0x22) + -0x54) * 0x10000) |
                     ((uint32_t)*(int16_t *)(st + 0x20) - 0x90U) & 0xffff,0x277e001e,
                     ((uint32_t)(v << 6) | 0x7e16U) << 0x10 | 0x9880);
        sprite_node_emit_a(cell,0,*(int16_t *)(st + 0x22) + -0x40,(int32_t)*(int16_t *)(st + 0x20),0x18,
                     col_a,0,col_a,0,2);
        goto LAB_8012cb64;
      }
      if (variant != 0x40) return;
    }
    v = head_g + 4;
    hud_text_d(v,attr,guest_at(0x8011f10c),0xc0,
                 (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x30) * 0x10000) >> 0x10,0);
    spr_slot_pkt_submit((uint32_t *)guest_at(v),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0xd8,0xf0f001e,0x7a16d830);
    spr_slot_pkt_submit((uint32_t *)guest_at(v),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0xe8,0xf0f001e,0x7a16d820);
    spr_slot_pkt_submit((uint32_t *)guest_at(v),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0xf8,0xf0f001e,0x7a16d810);
    spr_attr_alloc_b(head_g,0x200000,0x1780001e,0x7a5680ff,0x80,0x40);
    spr_attr_alloc_b(head_g,0x200080,0x1780001e,0x7a9680ff,0x100,0x40);
    spr_attr_alloc_b(head_g,0x200100,0x177e001e,0x7a968080,0x180,0x40);
    spr_attr_alloc_b(head_g,0x200180,0x177e001e,0x7a568080,0x200,0x40);
    spr_attr_alloc_a(v,0,*(int16_t *)(st + 0x22) + -0x38,(int32_t)*(int16_t *)(st + 0x20),0x18,
                 col_b,2);
    cell = (*(int16_t *)(st + 0x22) * 3) / 5 + -8;
    sprite_node_emit_a(v,0,0,(int32_t)*(int16_t *)(st + 0x20),cell,(int32_t)col_d,(int32_t)col_d,(int32_t)col_c,(int32_t)col_c,0);
    x = *(int16_t *)(st + 0x20);
    col_d = 0;
    v = *(int16_t *)(st + 0x22) - cell;
    col_c = col_a;
    col_b = col_a;
  }
  sprite_node_emit_a(head_g + 4,0,cell,(int32_t)x,v,(int32_t)col_c,(int32_t)col_b,0,(int32_t)col_d,0);
}

/* ======== 0x8018cbfc ======== */

/* Sprite-attr pool A allocator: 0x18 slots x 0x28 stride at
 * 0x801f5978, round-robin counter spr_pool_a_next, sentinel
 * frame_tag. Slot word0 == sentinel or sentinel-1 -> NULL;
 * else mark sentinel, bump counter (wrap at 0x18), return slot+4. */
void *spr_pool_a_alloc(void)
{
  int32_t owner;
  uint32_t *slot;
  int32_t next;
  int32_t v1 = spr_pool_a_next;
  uint32_t sent = frame_tag;
  if (v1 < 0) {
    slot = (uint32_t *)guest_at(0x801f5978);
    v1 = 0;
  } else {
    slot = (uint32_t *)guest_at(0x801f5978 + (uint32_t)v1 * 0x28);
  }
  owner = (int32_t)*slot;
  if (owner == (int32_t)sent || owner == (int32_t)(sent - 1))
    return (void *)0;
  next = v1 + 1;
  if (0x17 < next) next = 0;
  spr_pool_a_next = next;
  *slot = sent;
  return guest_at(0x801f5978 + (uint32_t)v1 * 0x28 + 4);
}

/* ======== 0x8018c0a8 ======== */

/* Sprite-attr pool B allocator: 0x20 slots x 0x2c stride at
 * 0x801f5ed8, counter spr_pool_b_next. Same sentinel protocol. */
void *spr_pool_b_alloc(void)
{
  int32_t owner;
  uint32_t *slot;
  int32_t next;
  int32_t v1 = spr_pool_b_next;
  uint32_t sent = frame_tag;
  slot = (uint32_t *)guest_at(0x801f5ed8 + (uint32_t)v1 * 0x2c);
  owner = (int32_t)*slot;
  if (owner == (int32_t)sent || owner == (int32_t)(sent - 1))
    return (void *)0;
  next = v1 + 1;
  if (0x1f < next) next = 0;
  spr_pool_b_next = next;
  *slot = sent;
  return guest_at(0x801f5ed8 + (uint32_t)v1 * 0x2c + 4);
}

/* ======== 0x8018e590 ======== */

/* printf core: emits formatted text via console_emit (putc) /
 * console_pct ('%'), returns char count. param_1 = channel (unused
 * here — callee globals), param_2 = fmt, param_3 = u32 arg array.
 * Flags: '-' left, '+' sign, ' ' space, '#' alt, '0' pad, digit width,
 * '.' precision, '*' width/precision from args, l/h length, d/i/u/o/x/X/p/c/s/n.
 * Elided-arg sites verified against disasm: sign emit passes iVar21,
 * space pads pass 0x20. */
extern void console_emit(uint32_t);
extern void console_pct(void);
extern int str_len(char *);
extern char * strchr_bounded(char *,char,int);
extern char s_0123456789abcdef_80124fe8[];
extern char s_0123456789ABCDEF_80125004[];
extern char s__null__80124ffc[];
extern uint8_t ctype_bitmap_801b6701[];
uint32_t printf_core(uint32_t chan,const uint8_t *fmt,const uint32_t *args)
{
  uint8_t c1, c2;
  char dig;
  int pfx, lng, sht, alt;
  uint32_t v, base, flags, quo;
  uint32_t *apn, *ap;
  char *txt;
  uint32_t *need, *fldw, *left;
  const uint8_t *fp;
  int32_t sign;
  intptr_t count, prec, zpad, width;
  char dbuf[40];
  intptr_t prec_raw;
  char *dcur;
  (void)chan;
  if (fmt == 0) return 0;
  const char *digits = s_0123456789abcdef_80124fe8;
  count = 0;
LAB_8018e5e0:
  v = (uint32_t)*fmt;
  if (v != 0) {
    flags = 0;
    pfx = 0; alt = 0; sht = 0; lng = 0;
    if (v == 0x25) {
      prec_raw = -1;
      sign = 0;
      prec = 0;
      zpad = 0;
      width = 0;
      ap = (uint32_t *)args;
      fp = fmt;
LAB_8018e60c:
      fmt = fp + 1;
      apn = ap;
      switch (*fmt) {
      case 0:
        goto switchD_8018e634_caseD_0;
      default:
        v = (uint32_t)(char)*fmt;
        count = count + 1;
        args = (const uint32_t *)ap;
        break;
      case 0x20:
        fp = fmt;
        if (sign == 0) sign = 0x20;
        goto LAB_8018e60c;
      case 0x23:
        alt = 1;
        fp = fmt;
        goto LAB_8018e60c;
      case 0x2a:
        apn = ap + 1;
        width = (intptr_t)*ap;
        ap = apn;
        fp = fmt;
        if (-1 < (int)width) goto LAB_8018e60c;
        width = -width;
        /* fallthrough */
      case 0x2d:
        flags |= 0x10;
        ap = apn;
        fp = fmt;
        goto LAB_8018e60c;
      case 0x2b:
        sign = 0x2b;
        fp = fmt;
        goto LAB_8018e60c;
      case 0x2e:
        fp = fp + 2;
        c1 = *fp;
        if (c1 == 0x2a) {
          prec_raw = (intptr_t)*ap;
          ap = ap + 1;
        } else {
          prec_raw = 0;
          while (c1 < 0x80 && (c2 = *fp, (ctype_bitmap_801b6701[c2] & 4) != 0)) {
            fp = fp + 1;
            c1 = *fp;
            prec_raw = (intptr_t)((intptr_t)prec_raw * 10 + -0x30 + (uint32_t)c2);
          }
          fp = fp - 1;
        }
        if ((int)prec_raw < 0) prec_raw = -1;
        goto LAB_8018e60c;
      case 0x30:
        flags |= 0x20;
        fp = fmt;
        goto LAB_8018e60c;
      case 0x31: case 0x32: case 0x33: case 0x34:
      case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
        width = 0;
        do {
          fp = fmt;
          fmt = fp + 1;
          width = (intptr_t)((intptr_t)width * 10 + -0x30 + (uint32_t)*fp);
          if (0x7f < *fmt) break;
        } while ((ctype_bitmap_801b6701[*fmt] & 4) != 0);
        goto LAB_8018e60c;
      case 0x44:
        lng = 1;
        /* fallthrough */
      case 100:
      case 0x69:
        if (lng || !sht) v = *ap;
        else v = (uint32_t)(int16_t)*ap;
        base = 10;
        if ((int32_t)v < 0) { v = -v; sign = 0x2d; }
        goto LAB_8018e95c_d;
      case 0x4c:
        fp = fmt;
        goto LAB_8018e60c;
      case 0x4f:
        lng = 1;
        /* fallthrough */
      case 0x6f:
        if (lng || (base = 8, !sht)) {
          v = *ap;
          base = 8;
          goto LAB_8018e958_d;
        }
        v = (uint32_t)(int16_t)*ap;
        sign = 0;
        goto LAB_8018e95c_d;
      case 0x55:
        lng = 1;
        /* fallthrough */
      case 0x75:
        if (lng || (base = 10, !sht)) {
          v = *ap;
          base = 10;
          goto LAB_8018e958_d;
        }
        v = (uint32_t)(int16_t)*ap;
        sign = 0;
        goto LAB_8018e95c_d;
      case 0x58:
        digits = s_0123456789ABCDEF_80125004;
        /* fallthrough */
      case 0x78:
        if (lng || !sht) v = *ap;
        else v = (uint32_t)(int16_t)*ap;
        base = 0x10;
        sign = 0;
        if (alt && v != 0) { pfx = 1; goto LAB_8018e958_d; }
        goto LAB_8018e95c_d;
      case 99:
        txt = (char *)dbuf;
        left = (uint32_t *)1;
        sign = 0;
        dbuf[0] = (char)*ap;
        goto LAB_8018e9fc_d;
      case 0x68:
        sht = 1;
        fp = fmt;
        goto LAB_8018e60c;
      case 0x6c:
        lng = 1;
        fp = fmt;
        goto LAB_8018e60c;
      case 0x6e:
        if (lng || !sht) {
          args = (const uint32_t *)(ap + 1);
          *(uint32_t *)guest_at(*ap) = (uint32_t)count;
        } else {
          args = (const uint32_t *)(ap + 1);
          *(uint16_t *)guest_at(*ap) = (uint16_t)count;
        }
        goto LAB_8018ebec_d;
      case 0x70:
        v = *ap;
        base = 0x10;
        goto LAB_8018e958_d;
      case 0x73:
        txt = *ap == 0 ? (char *)s__null__80124ffc
                                : (char *)guest_at(*ap);
        if ((int)prec_raw < 0) {
          prec_raw = str_len(txt);
        } else {
          char *hit = strchr_bounded(txt,0,(int)prec_raw);
          if (hit != 0 &&
              (sign = 0, left = (uint32_t *)(intptr_t)(hit - txt),
               hit - txt <= (intptr_t)prec_raw))
            goto LAB_8018e9fc_d;
        }
        sign = 0;
        left = (uint32_t *)prec_raw;
        goto LAB_8018e9fc_d;
      }
      console_emit(v);
      goto LAB_8018ebec_d;
    }
switchD_8018e634_caseD_0:
    console_pct();
  }
  return (uint32_t)count;
LAB_8018e958_d:
  sign = 0;
LAB_8018e95c_d:
  prec = (intptr_t)prec_raw;
  if (-1 < (int)prec_raw) flags &= ~0x20;
  {
    /* Guest digit scratch = sp+0x10..0x37, i.e. dbuf[40]; the cursor
     * s2 starts one past the top (sp+0x38, adjacent to the count char
     * counter) and walks down storing digits. The emitted string starts
     * at the final cursor and its length is top - cursor. Ghidra split
     * the scratch into &count/dbuf scalars and emitted pointer
     * arithmetic that wrote digits below the dcur variable itself and
     * never assigned txt — rebuild it as one real buffer. */
    dcur = dbuf + sizeof(dbuf);
    if (v != 0 || prec_raw != 0)
      goto LAB_8018e984_d;
    txt = dcur;
    left = (uint32_t *)0;
    goto LAB_8018e9fc_d;
  }
LAB_8018e984_d:
  do {
    quo = v / base;
    if (base == 0) { /* guest break 7 — div-by-zero */ }
    dig = digits[v % base];
    dcur = dcur - 1;
    *dcur = dig;
    v = quo;
  } while (quo != 0);
  digits = s_0123456789abcdef_80124fe8;
  if (alt && base == 8 && dig != '0') {
    dcur = dcur - 1;
    *dcur = '0';
  }
  txt = dcur;
  left = (uint32_t *)(uintptr_t)((dbuf + sizeof(dbuf)) - dcur);
LAB_8018e9fc_d:
  args = (const uint32_t *)((const uint32_t *)ap + 1);
  need = (uint32_t *)((intptr_t)left + zpad);
  if (sign != 0) need = (uint32_t *)((intptr_t)need + 1);
  if (pfx) need = (uint32_t *)((intptr_t)need + 2);
  fldw = (uint32_t *)prec;
  if (prec < (intptr_t)need) fldw = need;
  if (flags == 0 && width != 0 && (intptr_t)fldw < width) {
    intptr_t pad = (intptr_t)fldw;
    do { console_emit(0x20); pad++; } while (pad < width);
  }
  if (sign != 0) console_emit((uint32_t)sign);
  if (pfx) { console_emit(0x30); console_emit((uint32_t)(char)*fmt); }
  if (flags == 0x20 && (intptr_t)fldw < width) {
    intptr_t pad = (intptr_t)fldw;
    do { console_emit(0x30); pad++; } while (pad < width);
  }
  if ((intptr_t)need < prec) {
    do { console_emit(0x30); need = (uint32_t *)((intptr_t)need + 1); }
    while ((intptr_t)need < prec);
  }
  while ((left = (uint32_t *)((intptr_t)left - 1)), (intptr_t)left >= 0) {
    dig = *txt;
    txt = (char *)((intptr_t)txt + 1);
    console_emit((uint32_t)dig);
  }
  while ((zpad = zpad - 1), zpad >= 0) console_emit(0x30);
  if ((flags & 0x10) != 0 && (intptr_t)fldw < width) {
    intptr_t pad = (intptr_t)fldw;
    do { console_emit(0x20); pad++; } while (pad < width);
  }
  need = (uint32_t *)width;
  if (width < (intptr_t)fldw) need = fldw;
  count = count + (intptr_t)need;
LAB_8018ebec_d:
  fmt = fmt + 1;
  goto LAB_8018e5e0;
}

/* ======== debug console emit layer ========
 * 32-int8_t line buffer @0x801f68f0, col @0x801b6790, count @0x801b6794,
 * flushed via bios_tramp_b035(1, buf, count). */
extern int32_t bios_tramp_b035(int32_t,const void *,int32_t);

void console_emit(uint32_t c)
{
  int8_t ch = (int8_t)(c << 24 >> 24);
  if (ch == 9) {
    do { console_emit(0x20); }
    while ((*(int32_t *)guest_at(0x801b6790) & 7) != 0);
    return;
  }
  if (ch == 0xa) {
    console_emit(0xd);
    *(int32_t *)guest_at(0x801b6790) = 0;
  } else {
    if ((ctype_bitmap_801b6701[ch & 0xff] & 0x97) != 0)
      *(int32_t *)guest_at(0x801b6790) += 1;
  }
  int32_t cnt = *(int32_t *)guest_at(0x801b6794);
  if (0x1f < cnt) {
    bios_tramp_b035(1,guest_at(0x801f68f0),cnt);
    *(int32_t *)guest_at(0x801b6794) = 0;
    cnt = 0;
  }
  *(uint8_t *)guest_at(0x801f68f0u + (uint32_t)cnt) = (uint8_t)c;
  *(int32_t *)guest_at(0x801b6794) = cnt + 1;
}

void console_pct(void)
{
  int32_t cnt = *(int32_t *)guest_at(0x801b6794);
  if (0 < cnt) {
    bios_tramp_b035(1,guest_at(0x801f68f0),cnt);
    *(int32_t *)guest_at(0x801b6794) = 0;
  }
}

/* ======== 0x8018c1f4 ======== */

/* 9-slice/frame sprite builder: allocates from pool B
 * (spr_pool_b_alloc), writes corner/edge UVs derived from param_3 (w/h
 * packed bytes) and param_2 (pos), param_4 color at +0xc, tag
 * 0x2e808080 at +4, links slot[0] = old_head|0x9000000 and stores
 * compressed slot ptr into *param_1 head cell. Disasm-verified. */
uint32_t sprite_9slice_build(uint32_t head_g,uint32_t pos,uint32_t wh,uint32_t col)
{
  uint32_t *pkt;
  uint8_t *b;
  int32_t a2, a3, a0, v1, v0_;
  pkt = (uint32_t *)spr_pool_b_alloc();
  if (pkt == 0) return 0xffffffff;
  b = (uint8_t *)pkt;
  a2 = (int32_t)(wh >> 16) & 0xff; a2 += 1;
  a0 = (int32_t)col & 0xff;
  int32_t a1 = a0 + a2;
  v1 = (int32_t)(col >> 8) & 0xff;
  a3 = (int32_t)(wh >> 24) + 1;
  b[0x15] = (uint8_t)v1;
  v1 += a3;
  b[0x14] = (uint8_t)a1;
  b[0x24] = (uint8_t)a1;
  b[0x1d] = (uint8_t)v1;
  b[0x25] = (uint8_t)v1;
  b[0x1c] = (uint8_t)a0;
  a1 = -a2;
  a1 = (a1 + ((uint32_t)a1 >> 31)) >> 1;   /* -(a2)/2 rounding */
  a1 += (int32_t)pos;
  v1 = -a3;
  v1 = (v1 + ((uint32_t)v1 >> 31)) >> 1;
  a0 = (int32_t)pos >> 16;
  v1 += a0;
  a2 >>= 1; a2 += (int32_t)pos;
  a3 >>= 1; a0 += a3;
  pkt[3] = col;                          /* +0xc */
  *(int16_t *)(b + 0x16) = (int16_t)wh;
  *(int16_t *)(b + 8) = (int16_t)a1;
  *(int16_t *)(b + 0xa) = (int16_t)v1;
  *(int16_t *)(b + 0x10) = (int16_t)a2;
  *(int16_t *)(b + 0x12) = (int16_t)v1;
  *(int16_t *)(b + 0x18) = (int16_t)a1;
  *(int16_t *)(b + 0x1a) = (int16_t)a0;
  *(int16_t *)(b + 0x20) = (int16_t)a2;
  *(int16_t *)(b + 0x22) = (int16_t)a0;
  pkt[1] = 0x2e808080;
  uint32_t *head = (uint32_t *)guest_at(head_g);
  uint32_t old = *head;
  *pkt = old | 0x9000000;
  *head = guest_of(pkt) & 0xffffff;
  return 0;
}

/* ======== batch31: text-window printf + MDEC pump + memcard helpers ========
 * All ports verified against psx_payload.bin @0x8011e800 via capstone.
 * Guest addrs -> guest_at; pointers stored into guest cells stay raw
 * guest values (reg cells at 0x801b68xx hold guest pointers). */

/* ---- shared externs for this batch ---- */
extern int      str_len(char *);
void     mdec_decode_issue(uint32_t,uint32_t);
void     mdec_error_reset(uint32_t);
int32_t  mdec_stat_wait_a(void);
int32_t  mdec_stat_wait_b(void);
void     gp_20_store(uint32_t);

/* MDEC reg cell: 0x801b68xx holds a guest pointer to a status reg. */
/* The 0x801b68xx cells hold guest pointers into the 0x1F801xxx MDEC
 * register file — accesses must go through the pointer-cell bridge
 * (br2_ld32p/br2_st32p) so MMIO side effects fire; a raw guest_at()
 * deref would only touch the open-bus mirror. */
#define MDECREG(C) br2_ld32p(C)
#define MDECREG_ST(C,V) br2_st32p(C,V)

/* ======== 0x8018fc00 ======== */

void mdec_decode_issue(uint32_t bfr_g,uint32_t size)

{
  uint32_t sz;
  
  mdec_stat_wait_a();
  MDECREG_ST(0x801b68f8, MDECREG(0x801b68f8) | 0x88);
  MDECREG_ST(0x801b68c0, bfr_g + 4);
  sz = ((size >> 5) << 0x10) | 0x20;
  MDECREG_ST(0x801b68c4, sz);
  MDECREG_ST(0x801b68f0, *(uint32_t *)guest_at(bfr_g));
  MDECREG_ST(0x801b68c8, 0x1000201);
  return;
}

/* ======== 0x8018fd1c ======== */

int32_t mdec_stat_wait_a(void)

{
  int32_t cnt = 0x10;
  
  if ((MDECREG(0x801b68f4) & 0x20000000) != 0) {
    for (;;) {
      cnt = cnt - 1;
      if (cnt == -1) {
        mdec_error_reset(0x80125304);
        return -1;
      }
      if ((MDECREG(0x801b68f4) & 0x20000000) == 0) return 0;
    }
  }
  return 0;
}

/* ======== 0x8018fdb0 ======== */

int32_t mdec_stat_wait_b(void)

{
  int32_t cnt = 0x10;
  
  if ((MDECREG(0x801b68d4) & 0x100) != 0) {
    for (;;) {
      cnt = cnt - 1;
      if (cnt == -1) {
        mdec_error_reset(0x80125314);
        return -1;
      }
      if ((MDECREG(0x801b68d4) & 0x100) == 0) return 0;
    }
  }
  return 0;
}

/* ======== 0x8018fe5c ======== */

void mdec_error_reset(uint32_t msg_g)

{
  printf_forward((const char *)guest_at(0x80125324),
               (const char *)guest_at(msg_g));
  MDECREG_ST(0x801b68f4, 0x80000000);
  MDECREG_ST(0x801b68c8, 0);
  MDECREG_ST(0x801b68d4, 0);
  MDECREG_ST(0x801b68f4, 0x60000000);
  return;
}

/* ======== 0x8018fb10 ======== */

void mdec_pump_a(int32_t code)

{
  if (code == 0) {
    MDECREG_ST(0x801b68f4, 0x80000000);
    MDECREG_ST(0x801b68c8, 0);
    MDECREG_ST(0x801b68d4, 0);
    MDECREG_ST(0x801b68f4, 0x60000000);
    mdec_decode_issue(0x801b67b0,0x20);
    mdec_decode_issue(0x801b6834,0x20);
  }
  else if (code == 1) {
    MDECREG_ST(0x801b68f4, 0x80000000);
    MDECREG_ST(0x801b68c8, 0);
    MDECREG_ST(0x801b68d4, 0);
    MDECREG_ST(0x801b68f4, 0x60000000);
  }
  else {
    printf_forward((const char *)guest_at(0x801252e8),code);
  }
  return;
}

/* ======== 0x8018fc90 ======== */

void mdec_pump_b(uint32_t bfr_g,uint32_t size)

{
  uint32_t sz;
  
  mdec_stat_wait_b();
  MDECREG_ST(0x801b68f8, MDECREG(0x801b68f8) | 0x88);
  MDECREG_ST(0x801b68d4, 0);
  MDECREG_ST(0x801b68cc, bfr_g);
  sz = ((size >> 5) << 0x10) | 0x20;
  MDECREG_ST(0x801b68d0, sz);
  MDECREG_ST(0x801b68d4, 0x1000200);
  return;
}

/* ======== 0x80193fa0 ======== */

uint32_t gpu_bits_pack_a(uint32_t a0,uint32_t a1,uint32_t a2,
                      uint32_t a3)

{
  return ((a0 & 3) << 7) | ((a1 & 3) << 5) |
         ((a3 & 0x100) >> 4) | ((a2 & 0x3ff) >> 6) |
         ((a3 & 0x200) << 2);
}

/* ======== 0x80193fe0 ======== */

uint32_t gpu_coord_pack_b(uint32_t a0,uint32_t a1)

{
  return (((a1 << 6) | ((a0 >> 4) & 0x3f)) & 0xffff);
}

/* ======== 0x80193e30 ======== */

uint32_t gpu_sprite_rect_emit(uint32_t src,uint32_t x,uint32_t y)

{
  int16_t rect[4];
  
  rect[0] = (int16_t)x;
  rect[1] = (int16_t)y;
  rect[2] = 0x10;
  rect[3] = 1;
  /* guest rect = $sp+0x10 (frame 0x28, disasm 0x80193e30) -> frm+0x10;
   * reserved frame staging, see spr_draw_off. */
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t gre_frm = br2_guest_frame(0x28);
    memcpy(guest_at(gre_frm + 0x10), rect, 8);
    gpu_load_image_cmd(gre_frm + 0x10,src);
    br2_guest_frame_free();
  }
  return gpu_coord_pack_b(x,y) & 0xffff;
}

/* ======== 0x80193d40 ======== */

uint32_t gpu_sprite_rect_emit_mode(uint32_t src,uint32_t mode,uint32_t arg3,
                      uint32_t x,uint32_t y,uint32_t v,
                      uint32_t h)

{
  int16_t rect[4];
  int16_t dim;                          /* guest leaves +0x14 uninit for
                                         * mode > 2 / negative mode */
  int32_t mode_i = (int32_t)mode;
  int32_t v_i = (int32_t)v;
  
  rect[0] = (int16_t)x;
  rect[1] = (int16_t)y;
  rect[3] = (int16_t)h;
  if (mode_i == 1) {
    dim = (int16_t)(v_i / 2);
  }
  else if (mode_i < 2) {
    if (mode_i == 0) dim = (int16_t)(v_i / 4);
  }
  else if (mode_i == 2) {
    dim = (int16_t)v_i;
  }
  rect[2] = dim;
  /* guest rect = $sp+0x10 (frame 0x30, disasm 0x80193d40) -> frm+0x10;
   * reserved frame staging, see spr_draw_off. */
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t grem_frm = br2_guest_frame(0x30);
    memcpy(guest_at(grem_frm + 0x10), rect, 8);
    gpu_load_image_cmd(grem_frm + 0x10,src);
    br2_guest_frame_free();
  }
  return gpu_bits_pack_a(mode,arg3,x,y) & 0xffff;
}

/* ======== 0x80194120 ======== */

void gpu_pkt_tag_74(uint32_t pkt_g)

{
  uint8_t *p = (uint8_t *)guest_at(pkt_g);
  p[3] = 3;
  p[7] = 0x74;                        /* jr delay slot */
  return;
}

/* ======== 0x80194140 ======== */

void gpu_pkt_tag_60(uint32_t pkt_g)

{
  uint8_t *p = (uint8_t *)guest_at(pkt_g);
  p[3] = 3;
  p[7] = 0x60;                        /* jr delay slot */
  return;
}

/* ======== 0x80190dd0 ======== */

void guest_memset_b(uint32_t dst,uint32_t val,uint32_t len)

{
  uint8_t *d;
  int32_t n = (int32_t)len;
  
  if (dst == 0) return;
  d = (uint8_t *)guest_at(dst);
  if (n > 0) {
    do {
      *d = (uint8_t)val;
      n = n - 1;
      d = d + 1;
    } while (n > 0);
  }
  return;
}

/* ======== 0x80190a04 ======== */

/* Text-window printf: vsprintf into the 0x30-int8_t window slot at
 * 0x801b7740 + win*0x30 (fields: +0x1c limit, +0x24 line buffer guest
 * ptr, +0x28 cursor). a0 selects the window; if a0 is out of range it
 * is itself the format string and DAT_801b78c4 picks the window.
 * Supported specs: %d %x %X %c %s %% plus decimal width; '0' flag only
 * affects %x (zero-pad into the digit buffer). Faithful quirks kept:
 * unknown specs emit `count` bytes from a stale pointer, return is the
 * final cursor pos, -1 on overflow. */
static int32_t win_putc(uint32_t slot,int32_t c)
{
  uint32_t pos = GB32(slot + 0x28);
  uint8_t *buf = (uint8_t *)guest_at(GB32(slot + 0x24));
  
  buf[pos] = (uint8_t)c;
  pos = pos + 1;
  GB32(slot + 0x28) = pos;             /* stored even on overflow */
  if (GB32S(slot + 0x1c) < (int32_t)pos) return -1;
  return 0;
}

int32_t text_win_printf(int32_t sel,uint32_t a1,uint32_t a2,
                     uint32_t a3,...)

{
  uint32_t iargs[3];
  va_list ap;
  int ai;
  int32_t win;
  const char *fmt;
  uint32_t slot;
  int32_t count = (int32_t)a1;    /* live $a1: stale until a spec
                                        * handler sets it */
  int32_t c;
  uint8_t dbuf[0x210];
  uint8_t *dtop = dbuf + 0x210;
  const uint8_t *ep = dtop;            /* guest $s0: stale across specs */
  
  iargs[0] = a1;
  iargs[1] = a2;
  iargs[2] = a3;
  va_start(ap,a3);
  if ((sel < 0) || ((int32_t)GB32(0x801b78c0) <= sel)) {
    /* out-of-range a0: it is the format string; window = DAT_801b78c4 */
    win = GB32(0x801b78c4);
    if (GB32(0x801b7740 + win * 0x30 + 0x24) == 0) {
      va_end(ap);
      return -1;
    }
    fmt = (const char *)guest_at((uint32_t)sel);
    ai = 0;                            /* varargs start at a1 */
  }
  else {
    win = (uint32_t)sel;
    fmt = (const char *)guest_at(a1);
    ai = 1;                            /* varargs start at a2 */
  }
  slot = 0x801b7740 + win * 0x30;
  if (GB32S(slot + 0x1c) < GB32S(slot + 0x28)) {
    va_end(ap);
    return -1;
  }
#define BR2_ARG() (ai < 3 ? iargs[ai++] : (uint32_t)(int32_t)va_arg(ap,int32_t))
  for (;;) {
    c = (int8_t)*fmt;
    if (c == 0) break;
    if (c != 0x25) {                   /* '%' */
      if (win_putc(slot,c) < 0) { va_end(ap); return -1; }
      fmt = fmt + 1;
      continue;
    }
    fmt = fmt + 1;
    c = (int8_t)*fmt;
    if (c == 0x25) {                   /* '%%' */
      if (win_putc(slot,c) < 0) { va_end(ap); return -1; }
      fmt = fmt + 1;
      continue;
    }
    {
      int32_t zflag = (c == 0x30);    /* '0' after '%' */
      int32_t width = 0;
      int32_t spec;
      
      while ((uint32_t)(c - 0x30) < 10) {
        width = width * 10 + (c - 0x30);
        fmt = fmt + 1;
        c = (int8_t)*fmt;
      }
      if (width <= 0) width = 1;
      ep = dtop;                        /* $s0 reset (br delay slot) */
      spec = (uint32_t)(c - 0x58);
      if (spec >= 0x21) goto emit_tail;   /* unknown: stale count/ep */
      if (c == 0x64) {                 /* 'd' */
        int32_t av = (int32_t)BR2_ARG();
        uint32_t ua;
        uint8_t *sp = dtop;
        int32_t neg = 0;
        
        if (av < 0) {
          ua = 0u - (uint32_t)av;
          neg = 0x2d;
        }
        else {
          ua = (uint32_t)av;
        }
        count = 0;
        do {
          uint32_t hi = (uint32_t)(((uint64_t)ua * 0xcccccccdull) >> 32);
          uint32_t q = hi >> 3;
          *--sp = (uint8_t)(ua - q * 10 + 0x30);
          count = count + 1;
          ua = q;
        } while (ua != 0);
        if (neg != 0) {
          *--sp = (uint8_t)neg;
          count = count + 1;
        }
        ep = sp;
        goto pad_tail;
      }
      if ((c == 0x78) || (c == 0x58)) { /* 'x'/'X' */
        uint32_t hv = BR2_ARG();
        uint8_t *hp = dtop;
        static const char hexd[] = "0123456789abcdef";
        
        hp = hp - 1;
        count = 0;
        for (;;) {
          uint32_t nib = hv & 0xf;
          hv = hv >> 4;
          *hp = (uint8_t)hexd[nib];
          count = count + 1;
          if (hv == 0) break;
          hp = hp - 1;
        }
        hp = hp + 1;
        ep = hp;
        if (zflag != 0) {
          if (count < width) {
            do {
              hp = hp - 1;
              count = count + 1;
              *hp = 0x30;
            } while (count < width);
          }
          goto emit_only;
        }
        goto pad_tail;
      }
      if (c == 0x63) {                 /* 'c' */
        uint32_t av = BR2_ARG();
        uint8_t *hp = dtop - 1;        /* s0 reset to sp+0x210 per spec */
        *hp = (uint8_t)av;
        ep = hp;
        count = 1;
        goto pad_tail;
      }
      if (c == 0x73) {                 /* 's' */
        ep = (const uint8_t *)guest_at(BR2_ARG());
        count = str_len((char *)(uintptr_t)ep);
        goto pad_tail;
      }
      goto pad_tail;               /* unreachable (spec<0x21 all mapped) */
emit_tail:
      ;
pad_tail:
      if (count < width) {
        do {
          if (win_putc(slot,0x20) < 0) { va_end(ap); return -1; }
          width = width - 1;
        } while (count < width);
      }
emit_only:
      count = count - 1;
      if (count != -1) {
        do {
          int32_t ch = *ep;
          ep = ep + 1;
          if (win_putc(slot,ch) < 0) { va_end(ap); return -1; }
          count = count - 1;
        } while (count != -1);
      }
      fmt = fmt + 1;                   /* consume spec char */
    }
  }
  /* NUL-terminate at cursor (no pos update, no bounds check) */
  {
    uint8_t *buf = (uint8_t *)guest_at(GB32(slot + 0x24));
    buf[GB32(slot + 0x28)] = 0;
  }
  va_end(ap);
  return GB32(slot + 0x28);
#undef BR2_ARG
}

/* ======== 0x801a4aa0 ======== */

void gp_20_store(uint32_t val)

{
  /* $gp = 0x801beedc (set at 0x801261f8: lui/addiu $gp); +0x20 cell */
  *(uint32_t *)guest_at(0x801beefc) = val;
  return;
}

/* ======== 0x801a6ae8 ======== */

void gp_20_store_default(void)

{
  gp_20_store(0x801b17b4);
  return;
}

/* ======== 0x801ad59c ======== */

int32_t aux_tbl_store_a(uint32_t val,int32_t idx)

{
  if (idx < 5) {
    *(uint32_t *)guest_at(0x801ff638 + idx * 4) = val;
    return 1;
  }
  return 0;
}

/* ======== 0x801ad1e4 ======== */

int32_t aux_tbl_store_b(uint32_t val,int32_t idx)

{
  if (idx < 5) {
    *(uint32_t *)guest_at(0x801ff620 + idx * 4) = val;
    return 1;
  }
  return 0;
}

/* gpu_status_cmd — GPU status/command helper. Writes 0x10000007 through
 * the pointer cell @0x801b8428; reads *(*(u32*)0x801b8424) & 0xffffff.
 * If ==2: a0&8 -> write 0x9000001 via 0x801b8428, return 2; else return 1.
 * If !=2: read *T8428, write (*T8428 & 0x3fff) | 0xe1001000 through the
 * 0x801b8424 cell, re-read *T8424 (status side effect), return 0.
 * Disasm-verified @0x80193828 (cell addrs 0x801B8428/0x801B8424, store
 * base a1 = ptr from 0x801B8424 at 0x8019387C). */
uint32_t gpu_status_cmd(uint32_t flags)
{
  uint32_t v;
  br2_st32p(0x801b8428, 0x10000007);
  v = br2_ld32p(0x801b8424) & 0xffffff;
  if (v == 2) {
    if ((flags & 8) != 0) {
      br2_st32p(0x801b8428, 0x9000001);
      return 2;
    }
    return 1;
  }
  br2_st32p(0x801b8424, (br2_ld32p(0x801b8428) & 0x3fff) | 0xe1001000);
  (void)br2_ld32p(0x801b8424);
  return 0;
}

/* spr_tbl_init_a — one-shot sprite-table bulk init: when flag @0x801b55e4
 * negative, fill 24 records descending from 0x801f5d10 stride -0x28:
 * +0 = (*(u32*)0x1f8000dc)-2, +7 = 5, +0xb = 0x28; clear flag.
 * Disasm 0x8018cb98-0x8018cbf4. */
void spr_tbl_init_a(void)
{
  uint8_t *rec;
  uint32_t dc;
  int i;
  if (*(int32_t *)guest_at(0x801b55e4) >= 0) return;
  rec = (uint8_t *)guest_at(0x801f5d10);
  dc = *(uint32_t *)guest_at(0x1f8000dc) - 2;
  for (i = 0; i < 0x18; i++) {
    *(uint32_t *)rec = dc;
    rec[7] = 5;
    rec[0xb] = 0x28;
    rec -= 0x28;
  }
  *(uint32_t *)guest_at(0x801b55e4) = 0;
}

/* spr_tbl_init_b — same pattern for second table: flag @0x801b55e8,
 * 8 records from 0x801f5ea4 stride -0x34, +7 = 8, +0xb = 0x38.
 * Disasm 0x8018cd94-0x8018cdf0. */
void spr_tbl_init_b(void)
{
  uint8_t *rec;
  uint32_t dc;
  int i;
  if (*(int32_t *)guest_at(0x801b55e8) >= 0) return;
  rec = (uint8_t *)guest_at(0x801f5ea4);
  dc = *(uint32_t *)guest_at(0x1f8000dc) - 2;
  for (i = 0; i < 8; i++) {
    *(uint32_t *)rec = dc;
    rec[7] = 8;
    rec[0xb] = 0x38;
    rec -= 0x34;
  }
  *(uint32_t *)guest_at(0x801b55e8) = 0;
}

/* gpu_pkt_attr_set — GPU packet attr bit setter on host word *a0:
 * a1 bit0: set->clear bit27 (0x08000000), clear->set bit27;
 * a1 bit1: set->set bit25 (0x02000000), clear->clear bit25
 * (mask 0xfdffffff); then mdec_decode_issue(a0, lhu(*a0)).
 * Disasm 0x8018f9b4-0x8018fa28. */
void gpu_pkt_attr_set(uint32_t pkt_g,int flags)
{
  uint32_t *w = (uint32_t *)guest_at(pkt_g);
  if (flags & 1) *w &= 0xf7ffffff;
  else             *w |= 0x08000000;
  if (flags & 2) *w |= 0x02000000;
  else             *w &= 0xfdffffff;
  mdec_decode_issue(pkt_g,*(uint16_t *)w);
}

/* vram_tile_decode — VRAM tile RLE decoder + XOR descramble. Control bytes
 * from guest stream 0x801b6930: <0xf0 = run of n+1 bytes (n<0x80 only;
 * dist==0 copies literals, else backrefs from dst-dist); ==0xf0 resets
 * dist; >0xf0 packs dist = ((n<<8)|next) - 0xf0ff, dist==0xf00 ends.
 * Then u16 buf[i] ^= buf[i-4] for i in [4,0x87ff]. dst = host buffer.
 * Disasm 0x80190260-0x8019033c. */
void vram_tile_decode(uint8_t *vram)
{
  const uint8_t *s = (const uint8_t *)guest_at(0x801b6930);
  /* vram: host pointer from the clean_table wrapper, guest-address
   * value from game.c scene_entry (_DAT_801f4758 = 0x80010000+0x6f000) —
   * resolve either form. */
  uint8_t *base;
  {
    uint32_t g = guest_of(vram);
    base = (uint8_t *)guest_at(g ? g : (uint32_t)(uintptr_t)vram);
  }
  uint8_t *d = base;
  uint32_t dist = 0;
  uint32_t i;
  uint16_t *w;
  if (s == NULL || base == NULL) return;
  for (;;) {
    int32_t n = (int32_t)(s[0] & 0xff);
    s++;
    if (n < 0xf0) {
      if (n >= 0) {
        if (dist == 0) {
          while (n-- >= 0) { *d++ = *s++; }
        } else {
          while (n-- >= 0) { *d = d[-(int32_t)dist]; d++; }
        }
      }
    } else if (n == 0xf0) {
      dist = 0;
    } else {
      dist = (((uint32_t)n << 8) | s[0]) + 0xffff0f01;
      s++;
    }
    if (dist == 0xf00) break;
  }
  w = (uint16_t *)(base + 8);
  for (i = 4; i <= 0x87ff; i++) {
    w[0] ^= ((uint16_t *)base)[i - 4];
    w++;
  }
}

/* spr_prim_tag_a — sprite-prim tag init: int8_t+3=5, int8_t+7=0x28.
 * a0 is a guest address. Disasm 0x801940c0-0x801940d0. */
void spr_prim_tag_a(uint32_t pkt_g)
{
  uint8_t *p = (uint8_t *)guest_at(pkt_g);
  if (p == NULL) return;
  p[3] = 5;
  p[7] = 0x28;
}

/* spr_prim_tag_b — sprite-prim tag init: int8_t+3=3, int8_t+7=0x40.
 * a0 is a guest address. Disasm 0x80194160-0x80194170. */
void spr_prim_tag_b(uint32_t pkt_g)
{
  uint8_t *p = (uint8_t *)guest_at(pkt_g);
  if (p == NULL) return;
  p[3] = 3;
  p[7] = 0x40;
}

/* gpu_pkt_pack — GPU packet word pack: a0?0x400:0, a1?0xe100200:0,
 * base (a2 & 0x9ff). Disasm 0x80192430-0x80192448. */
uint32_t gpu_pkt_pack(uint8_t a0,char a1,uint16_t a2)
{
  uint32_t v0 = a2 & 0x9ff;
  if (a1) v0 |= 0xe100200;
  if (a0) v0 |= 0x400;
  return v0;
}

/* gpu_coord_pack — GPU coord pack: (a0 & 0x7ff) | 0xe5000000 |
 * ((a1 & 0x7ff) << 11). Disasm 0x80192580-0x80192594. */
uint32_t gpu_coord_pack(int a0,int a1)
{
  return ((uint32_t)a0 & 0x7ff) | 0xe5000000 | (((uint32_t)a1 & 0x7ff) << 11);
}

/* gpu_texpage_pack — GPU texpage/coord pack from 8-int8_t descriptor:
 * returns 0 on NULL; else packed (clut<<22)|(page<<12)|((u8+2)>>3)<<15|
 * ((-s16+6 &0xff)>>3)<<5 | ((-s16+4 &0xff)>>3). Disasm verified. */
uint32_t gpu_texpage_pack(uint16_t *a0_)
{
  uint8_t *a0 = (uint8_t *)a0_;
  uint32_t r, t;
  int32_t a2, v1;
  if (a0 == NULL) return 0;
  r = (uint32_t)(a0[0] >> 3) << 10;
  a2 = (int32_t)((uint32_t)(-(int32_t)*(int16_t *)(a0 + 4)) & 0xff) >> 3;
  t = (uint32_t)(a0[2] >> 3);
  r |= 0xe2000000;
  r |= t << 15;
  v1 = (int32_t)((uint32_t)(-(int32_t)*(int16_t *)(a0 + 6)) & 0xff) >> 3;
  r |= (uint32_t)v1 << 5;
  r |= (uint32_t)a2;
  return r;
}

/* gpu_coord_clamp_a/801924e8 — coord pack with viewport clamp:
 * low16 of each arg sign-extended, clamped to [0,dim-1] where dim is the
 * signed/unsigned hword @0x801b8324 (x) / @0x801b8326 (y); negative -> 0,
 * >=dim -> dim-1, in-range keeps original arg. Result
 * 0xe3000000/0xe4000000 | (x&0x3ff) | ((y&0x3ff)<<10).
 * Disasm 0x80192450-0x80192578. */
static uint32_t coord_clamp(uint32_t v, uint32_t dimaddr)
{
  uint8_t *p = (uint8_t *)guest_at(dimaddr);
  int32_t x = (int16_t)v;
  int32_t n = (int16_t)*(uint16_t *)p;
  uint32_t nu = *(uint16_t *)p;
  if (x < 0) return 0;
  if (n - 1 < x) return nu - 1;
  return v;
}
uint32_t gpu_coord_clamp_a(uint32_t a0,uint32_t a1)
{
  uint32_t x = coord_clamp(a0,0x801b8324);
  uint32_t y = coord_clamp(a1,0x801b8326);
  return ((y & 0x3ff) << 10) | (x & 0x3ff) | 0xe3000000;
}
uint32_t gpu_coord_clamp_b(uint32_t a0,uint32_t a1)
{
  uint32_t x = coord_clamp(a0,0x801b8324);
  uint32_t y = coord_clamp(a1,0x801b8326);
  return ((y & 0x3ff) << 10) | (x & 0x3ff) | 0xe4000000;
}

/* ======== 0x8018c31c ======== */
/* Rotated sprite-quad emitter. a0=link-out ptr (sp+38), a1=s7 base pos
 * (16.16), a2=s0 packed (w-1/h-1 nibbles + color?), a3=s4 attr word,
 * stack args s5=scale-x?, s6=scale-y?, fp=angle. Alloc via pool-B
 * spr_pool_b_alloc; fail -> -1. sin/cos via lut_sin_q12/8d298; corner
 * offsets rounded /8192; writes 4 corner xy pairs + tag 0x2c808080,
 * links node into *a0. Disasm 0x8018c31c-0x8018c574. */
extern void *spr_pool_b_alloc(void);
extern int lut_cos_q12(int);
extern int lut_sin_q12(uint32_t);
int sprite_quad_emit(uint32_t a0g, int32_t s7, uint32_t s0, uint32_t s4,
                int32_t s5, int32_t s6, int32_t fp)
{
  int32_t *s1 = (int32_t *)spr_pool_b_alloc();
  int32_t s2, s3, a1r, v0r, v1r;
  int32_t t2, t1, a3r, t0, t6, t5, t4, t3, a2r, a0r;
  int32_t sn, cs;
  uint32_t *out = (uint32_t *)guest_at(a0g);
  if (s1 == 0)
    return -1;
  s2 = (int32_t)(((int32_t)s0 >> 16) & 0xff) + 1;
  s3 = (int32_t)((s0 >> 24) & 0xff) + 1;
  a1r = (s4 & 0xff) + s2;
  ((uint8_t *)s1)[0x14] = (uint8_t)a1r;
  ((uint8_t *)s1)[0x24] = (uint8_t)a1r;
  ((uint8_t *)s1)[0x1c] = (uint8_t)(s4 & 0xff);
  v0r = (int32_t)((s4 >> 8) & 0xff);
  ((uint8_t *)s1)[0x15] = (uint8_t)v0r;
  v1r = v0r + s3;
  ((uint8_t *)s1)[0x1d] = (uint8_t)v1r;
  ((uint8_t *)s1)[0x25] = (uint8_t)v1r;
  s1[3] = (int32_t)s4;              /* +0xc */
  ((int16_t *)s1)[0xb] = (int16_t)s0; /* +0x16 */
  a1r = s2 * (int16_t)s5;
  if (a1r < 0) a1r += 0xfff;
  s2 = a1r >> 12;
  v0r = s3 * (int16_t)s6;
  if (v0r < 0) v0r += 0xfff;
  s3 = v0r >> 12;
  {
    /* jal delay slots: s0 = lut_sin_q12(angle) (f1),
     * v0 = lut_cos_q12(angle) (f2). */
    int32_t ang = (int16_t)fp;
    sn = lut_sin_q12((uint32_t)ang);
    cs = lut_cos_q12(ang);
  }
  t2 = (-s2) * cs;          /* -s2 * f2 */
  a3r = s3 * sn;            /* s3 * f1 */
  a0r = t2 + a3r; if (a0r < 0) a0r += 0x1fff;
  t1 = (-s2) * sn;          /* -s2 * f1 */
  v1r = s3 * cs;            /* s3 * f2 */
  a1r = t1 - v1r; if (a1r < 0) a1r += 0x1fff;
  t6 = a0r >> 13;
  v0r = s2 * cs;            /* s2 * f2 */
  a2r = v0r + a3r; if (a2r < 0) a2r += 0x1fff;
  t5 = a1r >> 13;
  t4 = a2r >> 13;
  t0 = s2 * sn;             /* s2 * f1 */
  a0r = t0 - v1r; if (a0r < 0) a0r += 0x1fff;
  t3 = a0r >> 13;
  a1r = t2 - a3r; if (a1r < 0) a1r += 0x1fff;
  t2 = a1r >> 13;
  a0r = t1 + v1r; if (a0r < 0) a0r += 0x1fff;
  t1 = a0r >> 13;
  v0r = v0r - a3r; if (v0r < 0) v0r += 0x1fff;
  a3r = v0r >> 13;
  a2r = t0 + v1r; if (a2r < 0) a2r += 0x1fff;
  {
    int32_t yb = s7 >> 16;
    int32_t xb = s7;
    ((int16_t *)s1)[4] = (int16_t)(xb + t6);   /* +8  */
    ((int16_t *)s1)[5] = (int16_t)(yb + t5);   /* +0xa */
    ((int16_t *)s1)[8] = (int16_t)(xb + t4);   /* +0x10 */
    ((int16_t *)s1)[9] = (int16_t)(yb + t3);   /* +0x12 */
    ((int16_t *)s1)[0xc] = (int16_t)(xb + t2); /* +0x18 */
    ((int16_t *)s1)[0xd] = (int16_t)(yb + t1); /* +0x1a */
    ((int16_t *)s1)[0x10] = (int16_t)(xb + a3r);/* +0x20 */
    ((int16_t *)s1)[0x11] = (int16_t)(yb + (a2r >> 13)); /* +0x22 */
  }
  s1[1] = 0x2c808080;                        /* +4 tag */
  s1[0] = (int32_t)(*out | 0x09000000);
  *out = guest_of(s1) & 0xffffff;            /* link: low24 of guest node */
  return 0;
}

/* ======== 0x801863ac ======== */
/* Empty stub in the payload: jr ra; nop. Faithful no-op. */
void guest_noop(void) { }

/* ======== 0x80191f08 ======== */
/* Sprite-rect packet fill: pkt+3=2, pkt+4=coord-pack(lh@r+0,lh@r+2)
 * via gpu_coord_clamp_a, pkt+8=gpu_coord_clamp_b(s16(u16@0+u16@4-1),
 * s16(u16@2+u16@6-1)). a0=pkt host, a1=rect host. */
void sprite_rect_pkt_fill(uint32_t *pkt, uint16_t *r)
{
  /* pkt/r: host pointers from the clean_table wrapper and menu.c/save.c
   * guest_at() callers, but raw guest-address values from save.c
   * (0x801f4xxx constants) — resolve either form. */
  uint32_t g1 = guest_of(pkt);
  uint32_t g2 = guest_of(r);
  pkt = (uint32_t *)guest_at(g1 ? g1 : (uint32_t)(uintptr_t)pkt);
  r = (uint16_t *)guest_at(g2 ? g2 : (uint32_t)(uintptr_t)r);
  ((uint8_t *)pkt)[3] = 2;
  pkt[1] = gpu_coord_clamp_a((uint32_t)(int16_t)r[0],
                        (uint32_t)(int16_t)r[1]);
  pkt[2] = gpu_coord_clamp_b((uint32_t)(int16_t)(r[0] + r[2] - 1),
                        (uint32_t)(int16_t)(r[1] + r[3] - 1));
}

/* ======== 0x80194180 ======== */
/* GPU texpage/clut packet: mode = (lh@a1+4 && lh@a1+6) ? 5 : 0;
 * +4=0x1000000, +8=0x80000000, +0x10=(a3<<16)|(a2&0xffff),
 * +0xc/+0x14 = a1[0]/a1[1]. Returns a1[1]. a0=host pkt. */
uint32_t gpu_texclut_pkt(uint32_t *t0, uint32_t *a1, uint32_t a2, uint32_t a3)
{
  uint32_t mode = 5;
  /* t0/a1: host pointers from the clean_table wrapper/menu.c, raw
   * guest-address values from save.c — resolve either form. */
  {
    uint32_t g1 = guest_of(t0);
    uint32_t g2 = guest_of(a1);
    t0 = (uint32_t *)guest_at(g1 ? g1 : (uint32_t)(uintptr_t)t0);
    a1 = (uint32_t *)guest_at(g2 ? g2 : (uint32_t)(uintptr_t)a1);
  }
  if (*(int16_t *)((uint8_t *)a1 + 4) == 0 ||
      *(int16_t *)((uint8_t *)a1 + 6) == 0)
    mode = 0;
  t0[1] = 0x1000000;
  t0[2] = 0x80000000;
  ((uint8_t *)t0)[3] = (uint8_t)mode;
  t0[4] = (a3 << 16) | (a2 & 0xffff);
  t0[3] = a1[0];
  t0[5] = a1[1];
  return a1[1];
}

/* =====================================================================
 * GTE / libgpu leaf helpers (0x80194xxx-0x80195xxx cluster).
 *
 * These are the game's embedded GTE library wrappers: lwc2/swc2 vector
 * loads/stores around raw COP2 commands, AVSZ/RTPS/OP/INTPL/SQR/DPCL/
 * DPCT entry points, and the fixed-point reciprocal LUT helpers. All are
 * pure computation (no IRQ, no blocking waits, no live guest sp) so they
 * port faithfully. COP2 register moves go through the bind.c helpers
 * (setCopReg/getCopReg/setCopControlWord/getCopControlWord/copFunction),
 * which route to gte_write_data/gte_read_data/gte_write_ctrl/
 * gte_read_ctrl/gte_execute — the same helpers the generated code uses
 * for every architecturally-visible COP2 access.
 * ===================================================================== */

#define GTE_DREG(n) ((n) * 0x800)   /* COP2 data-reg field encoding (bind.c) */
#define GTE_CREG(n) ((n) * 0x800)   /* COP2 ctrl-reg field encoding        */

/* lwc2 gte[reg], off(gaddr): guest word load into a GTE data register.
 * setCopReg routes through gte_write_data which is exactly what the
 * generated emitter uses for every MTC2/LWC2 (helper or raw write). */
static uint32_t gte_lwc2(uint32_t gaddr, int reg)
{
  uint32_t v = DB32(gaddr);
  setCopReg(2, GTE_DREG(reg), v);
  return v;
}

/* swc2 gte[reg], off(gaddr): GTE data register store to guest RAM.
 * getCopReg routes through gte_read_data — same value the generated
 * emitter produces for both raw and helper-classified registers. */
static void gte_swc2(uint32_t gaddr, int reg)
{
  DB32(gaddr) = (uint32_t)getCopReg(2, GTE_DREG(reg));
}

/* ======== 0x80190228 ======== */
/* Debug/profiler snapshot: stores gpr[4],gpr[5],gpr[2],gpr[3],gpr[12],
 * gpr[13],gpr[15],gpr[24],gpr[25] into the 9-word block @0x801b6904 and
 * returns 1. Needs the live CPUState for the non-arg registers.
 * Disasm 0x80190228-0x8019025C. */
uint32_t cpu_regs_snapshot(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t *d;
  if (c == NULL) return 1;
  d = (uint32_t *)guest_at(0x801b6904);
  d[0] = c->gpr[4];   /* a0 */
  d[1] = c->gpr[5];   /* a1 */
  d[2] = c->gpr[2];   /* v0 */
  d[3] = c->gpr[3];   /* v1 */
  d[4] = c->gpr[12];  /* t4 */
  d[5] = c->gpr[13];  /* t5 */
  d[6] = c->gpr[15];  /* t7 */
  d[7] = c->gpr[24];  /* t8 */
  d[8] = c->gpr[25];  /* t9 */
  return 1;
}

/* ======== 0x80191074 ======== */
/* Single-int8_t flag read @0x801c8322 (adjacent to the 0x801c8324/26
 * screen-dimension words used by gpu_coord_clamp_*). Disasm
 * 0x80191074-0x80191080. */
uint32_t gpu_flag_8322_read(void)
{
  return (uint32_t)*(uint8_t *)guest_at(0x801c8322);
}

/* ======== 0x80192944 ======== */
/* GPU display-environment DMA submit. a0 points at the env word pair
 * (+0 tag word, +4 packed x/y hwords), a1 at the payload word list.
 * Clamps +4/+6 against the 0x801c8324/26 dimension hwords, derives the
 * transfer split from x*y, pumps the GPU status bit through
 * cd_pump_watchdog, streams env+payload through the 0x801c8424 reg cell,
 * then kicks the 0x801c8428/842c/8430/8434 DMA cells when the high chunk
 * count is nonzero. Disasm 0x80192944-0x80192B7C. */
uint32_t gpu_draw_env_dma(uint32_t envg, uint32_t listg)
{
  int32_t xs, ys, xv, yv, prod, p1, half, q5, rem;
  uint32_t xu, yu, chunks, n, cell;
  uint32_t w;

  cd_timeout_arm();

  xs = (int32_t)(int16_t)br2_ld16(envg + 4);
  if (xs < 0) {
    xu = 0;
  } else {
    int32_t bound = (int32_t)(int16_t)br2_ld16(0x801c8324u);
    xu = br2_ld16(envg + 4);
    if (bound < xs)
      xu = br2_ld16(0x801c8324u);
  }
  br2_st16(envg + 4, (uint16_t)xu);

  ys = (int32_t)(int16_t)br2_ld16(envg + 6);
  if (ys < 0) {
    yu = 0;
  } else {
    int32_t bound = (int32_t)(int16_t)br2_ld16(0x801c8326u);
    yu = br2_ld16(envg + 6);
    if (bound < ys)
      yu = br2_ld16(0x801c8326u);
  }
  br2_st16(envg + 6, (uint16_t)yu);

  xv = (int32_t)(int16_t)xu;
  yv = (int32_t)(int16_t)yu;
  prod = xv * yv;
  p1 = prod + 1;
  half = (p1 + (p1 >> 31)) >> 1;
  if (half <= 0)
    return 0xffffffffu;
  q5 = (p1 + (p1 >> 31)) >> 5;
  chunks = (uint32_t)q5;
  rem = half - (q5 << 4);

  for (;;) {
    cell = br2_ld32(0x801c8428u);
    w = br2_ld32(cell);
    if ((w & 0x04000000u) != 0)
      break;
    if (cd_pump_watchdog() != 0)
      return 0xffffffffu;
  }

  br2_st32(br2_ld32(0x801c8428u), 0x04000000u);
  br2_st32(br2_ld32(0x801c8424u), 0x01000000u);
  br2_st32(br2_ld32(0x801c8424u), 0xa0000000u);
  br2_st32(br2_ld32(0x801c8424u), br2_ld32(envg));
  br2_st32(br2_ld32(0x801c8424u), br2_ld32(envg + 4));

  for (n = (uint32_t)rem; n != 0xffffffffu; n = (uint32_t)((int32_t)n - 1)) {
    cell = br2_ld32(0x801c8424u);
    br2_st32(cell, br2_ld32(listg));
    listg += 4;
  }

  if (chunks != 0) {
    uint32_t st;
    cell = br2_ld32(0x801c8428u);
    st = br2_ld32(cell);
    br2_st32(cell, st | 0x00000002u);
    br2_st32(br2_ld32(0x801c842cu), listg);
    br2_st32(br2_ld32(0x801c8430u), (chunks << 16) | 0x00000010u);
    br2_st32(br2_ld32(0x801c8434u), 0x01000201u);
  }
  return 0;
}

/* ======== 0x80194000 ======== */
/* Swap the low-24 payload fields of the two packet words while keeping
 * each word's own top int8_t, then tag *a0 with the compressed a1 pointer:
 *   *a1 = (*a1 & 0xff000000) | (*a0 & 0x00ffffff)
 *   *a0 = (*a0 & 0xff000000) | (a1_guest & 0x00ffffff)
 * The second store ORs the pointer-compressed form of a1 itself (the
 * & 0xffffff tag convention used throughout the OT packet code), not
 * *a1's payload — the guest ANDs the a1 argument register, verified
 * against the generated body (and $a1,$a1,$t2 @0x80194028).
 * Disasm 0x80194000-0x8019403C. */
void gpu_pkt_merge24(uint32_t a0g, uint32_t a1g)
{
  uint32_t *a0 = (uint32_t *)guest_at(a0g);
  uint32_t *a1 = (uint32_t *)guest_at(a1g);
  uint32_t v1 = a1[0];
  uint32_t v0;
  a1[0] = (v1 & 0xff000000u) | (a0[0] & 0x00ffffffu);
  v0 = a0[0];
  a0[0] = (v0 & 0xff000000u) | (a1g & 0x00ffffffu);
}

/* ======== 0x80194040 ======== */
/* *a0 |= 0x00ffffff — set the low 24 payload bits of a packet word.
 * Disasm 0x80194040-0x80194058. */
void gpu_pkt_low24_or(uint32_t a0g)
{
  uint32_t *a0 = (uint32_t *)guest_at(a0g);
  a0[0] |= 0x00ffffffu;
}

/* ======== 0x80194100 ======== */
/* Packet tag int8_t pair: p[3]=0x0c, p[7]=0x3c. Sits between the
 * spr_prim_tag_a/b helpers (0x801940c0/0x80194160). Disasm
 * 0x80194100-0x80194114. */
void gpu_pkt_tag_0c3c(uint32_t a0g)
{
  uint8_t *p = (uint8_t *)guest_at(a0g);
  p[3] = 0x0c;
  p[7] = 0x3c;
}

/* ======== 0x801948c0 ======== */
/* Fixed-point reciprocal via the 0x801B8480 LUT — sibling of
 * ani.c's fixed_rsqrt_q12 (0x80194500) but with a 0x13 (not 0x1F)
 * exponent bias and a signed result: returns table_val << e for e>=0,
 * table_val >> -e for e<0 (the 0x80194940 negative-exponent tail is
 * inlined here; LZCR==32 returns 0 via the 0x8019494C tail).
 * a0 is the LZCS source AND the value being normalized-shifted
 * (sllv/srav $t4,$a0,$t3) — faithful to the guest.
 * Disasm 0x801948c0-0x8019495C. */
uint32_t fixed_recip_lut(uint32_t a0)
{
  uint32_t lz, sh, m, idx, v;
  int32_t d;

  setCopReg(2, GTE_DREG(30), a0);          /* LZCS: latch source */
  lz = (uint32_t)getCopReg(2, GTE_DREG(31));
  if (lz == 32) return 0;
  lz &= 0xFFFFFFFEu;
  sh = (uint32_t)(((int32_t)(0x13u - lz)) >> 1);   /* sra, bias 0x13 */

  /* Mantissa extract (sllv/srav $t4,$a0,$t3): m = a0 << (lz-24) when
   * lz-24 >= 0, else m = a0 >> (24-lz) arithmetic — the VALUE a0 is
   * shifted by the exponent delta, same shape as ani.c's
   * fixed_rsqrt_q12. */
  d = (int32_t)lz - 24;
  if (d < 0) {
    m = (uint32_t)((int32_t)a0 >> ((24 - (int32_t)lz) & 31));  /* srav */
  } else {
    m = a0 << (d & 31);                                      /* sllv */
  }
  idx = (m - 64u) << 1;
  v = (uint32_t)*(const int16_t *)guest_at(0x801B8480u + idx);

  if ((int32_t)sh < 0) {
    uint32_t e = (uint32_t)(-(int32_t)sh);
    return v >> (e & 31);                  /* srlv — logical */
  }
  return v << (sh & 31);                   /* sllv */
}

/* ======== 0x80195290 ======== */
/* GTE control offset pair: ctrl[24] = a0 << 16, ctrl[25] = a1 << 16
 * (CTC2 writes; the <<16 happens in GPRs first). Disasm
 * 0x80195290-0x801952A8. */
void gte_ofs_set(int32_t a0, int32_t a1)
{
  setCopControlWord(2, GTE_CREG(24), (uint32_t)(a0 << 16));
  setCopControlWord(2, GTE_CREG(25), (uint32_t)(a1 << 16));
}

/* ======== 0x801952c0 ======== */
/* MVMVA vec3 wrapper: lwc2 gte[9..11] = in[0..2], GTE cmd 0x4DA412
 * (MVMVA), swc2 gte[9..11] -> out[0..2]. in/out are guest addresses.
 * Disasm 0x801952c0-0x801952E4. */
void gte_mvmva_vec3(uint32_t in, uint32_t out)
{
  gte_lwc2(in + 0, 9);
  gte_lwc2(in + 4, 10);
  gte_lwc2(in + 8, 11);
  copFunction(2, 0x4DA412);
  gte_swc2(out + 0, 9);
  gte_swc2(out + 4, 10);
  gte_swc2(out + 8, 11);
}

/* ======== 0x801952e8 ======== */
/* DPCL wrapper: lwc2 gte[9..11] = vec[0..2], lwc2 gte[6] = *scl_addr,
 * gte[8] = a2 (IR0), GTE cmd 0x680029 (DPCL), swc2 gte[22] -> *out.
 * Disasm 0x801952e8-0x8019530C. */
void gte_dpcl_vec3(uint32_t vec, uint32_t scl_addr, uint32_t ir0,
                   uint32_t out)
{
  gte_lwc2(vec + 0, 9);
  gte_lwc2(vec + 4, 10);
  gte_lwc2(vec + 8, 11);
  gte_lwc2(scl_addr + 0, 6);
  setCopReg(2, GTE_DREG(8), ir0);
  copFunction(2, 0x680029);
  gte_swc2(out + 0, 22);
}

/* ======== 0x80195310 ======== */
/* DPCT wrapper with three output pointers passed on the stack:
 *   lwc2 gte[20] = *a0, gte[21] = *a1, gte[22] = *a2, gte[6] = *a2,
 *   gte[8] = a3 (IR0 — the raw 4th arg, NOT dereferenced),
 *   GTE cmd 0x0F8002A (DPCT),
 *   swc2 gte[20..22] -> *stk0,*stk1,*stk2 (sp+16/20/24 args).
 * Generated 0x80195310-0x8019534C loads gte[6] from $a2 (gpr6), the
 * same pointer that fed gte[22]. */
void gte_dpct_vec3(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
                   uint32_t o0, uint32_t o1, uint32_t o2)
{
  gte_lwc2(a0 + 0, 20);
  gte_lwc2(a1 + 0, 21);
  gte_lwc2(a2 + 0, 22);
  gte_lwc2(a2 + 0, 6);
  setCopReg(2, GTE_DREG(8), a3);
  copFunction(2, 0x0F8002A);
  gte_swc2(o0 + 0, 20);
  gte_swc2(o1 + 0, 21);
  gte_swc2(o2 + 0, 22);
}

/* ======== 0x8019534c ======== */
/* INTPL wrapper: lwc2 gte[9..11] = vec[0..2], gte[8] = ir0,
 * GTE cmd 0x0980011 (INTPL), swc2 gte[22] -> *out.
 * Disasm 0x8019534C-0x80195370. */
void gte_intpl_vec3(uint32_t vec, uint32_t ir0, uint32_t out)
{
  gte_lwc2(vec + 0, 9);
  gte_lwc2(vec + 4, 10);
  gte_lwc2(vec + 8, 11);
  setCopReg(2, GTE_DREG(8), ir0);
  copFunction(2, 0x0980011);
  gte_swc2(out + 0, 22);
}

/* ======== 0x80195370 ======== */
/* SQR wrapper (cmd 0x0A80428 — V-reg fields differ from the
 * already-ported 0x80195398/0x80194xxx siblings): lwc2 gte[9..11] =
 * in[0..2], SQR, swc2 gte[25..27] (MAC1..3) -> out[0..2].
 * Returns out (guest addr) in v0 — `addu v0,a1,zr` in the delay slot.
 * Disasm 0x80195370-0x80195398. */
uint32_t gte_sqr_vec3_b(uint32_t in, uint32_t out)
{
  gte_lwc2(in + 0, 9);
  gte_lwc2(in + 4, 10);
  gte_lwc2(in + 8, 11);
  copFunction(2, 0x0A80428);
  gte_swc2(out + 0, 25);
  gte_swc2(out + 4, 26);
  gte_swc2(out + 8, 27);
  return out;
}

/* ======== 0x801953c0 ======== */
/* AVSZ3 wrapper: gte[17..19] = a0..a2, GTE cmd 0x158002D (AVSZ3),
 * returns gte[7] (the single accumulator read-back).
 * Disasm 0x801953C0-0x801953E0. */
uint32_t gte_avsz3_wrap(uint32_t a0, uint32_t a1, uint32_t a2)
{
  setCopReg(2, GTE_DREG(17), a0);
  setCopReg(2, GTE_DREG(18), a1);
  setCopReg(2, GTE_DREG(19), a2);
  copFunction(2, 0x158002D);
  return (uint32_t)getCopReg(2, GTE_DREG(7));
}

/* ======== 0x801953e0 ======== */
/* AVSZ4 wrapper: gte[16..19] = a0..a3, GTE cmd 0x168002E (AVSZ4),
 * returns gte[7]. Disasm 0x801953E0-0x80195404. */
uint32_t gte_avsz4_wrap(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  setCopReg(2, GTE_DREG(16), a0);
  setCopReg(2, GTE_DREG(17), a1);
  setCopReg(2, GTE_DREG(18), a2);
  setCopReg(2, GTE_DREG(19), a3);
  copFunction(2, 0x168002E);
  return (uint32_t)getCopReg(2, GTE_DREG(7));
}

/* ======== 0x80195404 ======== */
/* OP-with-matrix wrapper (variant A, cmd 0x178000C): saves ctrl[0,2,4],
 * loads them from mtx[0..2], lwc2 gte[9..11] = vec[8],vec[0],vec[4]
 * (note the +8/+0/+4 order), OP, swc2 MAC1..3 -> out[0..2], restores
 * ctrl[0,2,4]. Disasm 0x80195404-0x8019545C. */
void gte_op_vec3_a(uint32_t mtx, uint32_t vec, uint32_t out)
{
  int32_t c0 = getCopControlWord(2, GTE_CREG(0));
  int32_t c2 = getCopControlWord(2, GTE_CREG(2));
  int32_t c4 = getCopControlWord(2, GTE_CREG(4));
  setCopControlWord(2, GTE_CREG(0), DB32(mtx + 0));
  setCopControlWord(2, GTE_CREG(2), DB32(mtx + 4));
  setCopControlWord(2, GTE_CREG(4), DB32(mtx + 8));
  gte_lwc2(vec + 8, 11);
  gte_lwc2(vec + 0, 9);
  gte_lwc2(vec + 4, 10);
  copFunction(2, 0x178000C);
  gte_swc2(out + 0, 25);
  gte_swc2(out + 4, 26);
  gte_swc2(out + 8, 27);
  setCopControlWord(2, GTE_CREG(0), (uint32_t)c0);
  setCopControlWord(2, GTE_CREG(2), (uint32_t)c2);
  setCopControlWord(2, GTE_CREG(4), (uint32_t)c4);
}

/* ======== 0x8019545c ======== */
/* OP-with-matrix wrapper (variant B, cmd 0x170000C — different OP
 * operand fields than variant A). Identical ctrl save/set and
 * vec load order. Disasm 0x8019545C-0x801954B4. */
void gte_op_vec3_b(uint32_t mtx, uint32_t vec, uint32_t out)
{
  int32_t c0 = getCopControlWord(2, GTE_CREG(0));
  int32_t c2 = getCopControlWord(2, GTE_CREG(2));
  int32_t c4 = getCopControlWord(2, GTE_CREG(4));
  setCopControlWord(2, GTE_CREG(0), DB32(mtx + 0));
  setCopControlWord(2, GTE_CREG(2), DB32(mtx + 4));
  setCopControlWord(2, GTE_CREG(4), DB32(mtx + 8));
  gte_lwc2(vec + 8, 11);
  gte_lwc2(vec + 0, 9);
  gte_lwc2(vec + 4, 10);
  copFunction(2, 0x170000C);
  gte_swc2(out + 0, 25);
  gte_swc2(out + 4, 26);
  gte_swc2(out + 8, 27);
  setCopControlWord(2, GTE_CREG(0), (uint32_t)c0);
  setCopControlWord(2, GTE_CREG(2), (uint32_t)c2);
  setCopControlWord(2, GTE_CREG(4), (uint32_t)c4);
}

/* ======== 0x801954d0 ======== */
/* RTPS wrapper (the classic sceGteRTPS shape): lwc2 gte[0..1] =
 * in[0..1], GTE cmd 0x0180001 (RTPS), swc2 gte[14] -> *out_sxy,
 * gte[8] (sign-extended read) -> *out_mac, ctrl[31] -> *out_flag,
 * returns gte[19] >> 2 (arithmetic). in/out_* are guest addresses.
 * Disasm 0x801954D0-0x80195500. */
uint32_t gte_rtps_vec3(uint32_t in, uint32_t out_sxy, uint32_t out_mac,
                       uint32_t out_flag)
{
  gte_lwc2(in + 0, 0);
  gte_lwc2(in + 4, 1);
  copFunction(2, 0x0180001);
  gte_swc2(out_sxy + 0, 14);
  gte_swc2(out_mac + 0, 8);
  DB32(out_flag + 0) = (uint32_t)getCopControlWord(2, GTE_CREG(31));
  return (uint32_t)((int32_t)getCopReg(2, GTE_DREG(19)) >> 2);
}

/* ======== 0x80194a70 ======== */
/* Vertex screen-projection pack: three RTPS passes (cmd 0x0486012)
 * over the s16 coord fields of the 20-int8_t record at a0, with the
 * SXY read-backs (gte data 9/10/11) of each pass saved in GPRs and
 * repacked — as (hi<<16 | lo&0xffff) u32 words — into +0/+4/+8/+12,
 * then swc2 gte[11] -> +16. Returns a0 ($v0 = $a0 in the delay-slot
 * move). Verified against generated br2eu_func_80194A70:
 *   pass1 in : d0=(w@0&ffff)|(w@4&ffff0000) d1=w@12
 *   pass2 in : d0=s16@2|(w@8<<16)               d1=s16@14
 *   pass3 in : d0=s16@4|(w@8&ffff0000)          d1=w@16
 *   stores   : +0 =(d9_2<<16)|(d9_1&ffff)   +4 =(d9_3&ffff)|(d10_1<<16)
 *              +8 =(d10_3<<16)|(d10_2&ffff) +12=(d11_2<<16)|(d11_1&ffff)
 *              +16= d11_3 (swc2) */
uint32_t gte_rtps3_sxy_pack(uint32_t a0g)
{
  uint8_t *r = (uint8_t *)guest_at(a0g);
  uint32_t d9_1, d10_1, d11_1, d9_2, d10_2, d11_2;
  uint32_t d9_3, d10_3;

  setCopReg(2, GTE_DREG(0),
            (DB32(a0g + 0) & 0xffffu) | (DB32(a0g + 4) & 0xffff0000u));
  setCopReg(2, GTE_DREG(1), DB32(a0g + 12));
  copFunction(2, 0x0486012);                    /* RTPS */
  d9_1  = (uint32_t)getCopReg(2, GTE_DREG(9));
  d10_1 = (uint32_t)getCopReg(2, GTE_DREG(10));
  d11_1 = (uint32_t)getCopReg(2, GTE_DREG(11));

  setCopReg(2, GTE_DREG(0),
            (uint32_t)(int32_t)*(int16_t *)(r + 2) | (DB32(a0g + 8) << 16));
  setCopReg(2, GTE_DREG(1), (uint32_t)(int32_t)*(int16_t *)(r + 14));
  copFunction(2, 0x0486012);                    /* RTPS */
  d9_2  = (uint32_t)getCopReg(2, GTE_DREG(9));
  d10_2 = (uint32_t)getCopReg(2, GTE_DREG(10));
  d11_2 = (uint32_t)getCopReg(2, GTE_DREG(11));

  setCopReg(2, GTE_DREG(0),
            (uint32_t)(int32_t)*(int16_t *)(r + 4) |
            (DB32(a0g + 8) & 0xffff0000u));
  setCopReg(2, GTE_DREG(1), DB32(a0g + 16));
  copFunction(2, 0x0486012);                    /* RTPS */
  d9_3  = (uint32_t)getCopReg(2, GTE_DREG(9));
  d10_3 = (uint32_t)getCopReg(2, GTE_DREG(10));

  DB32(a0g + 0)  = (d9_2  << 16) | (d9_1  & 0xffffu);
  DB32(a0g + 12) = (d11_2 << 16) | (d11_1 & 0xffffu);
  DB32(a0g + 4)  = (d9_3  & 0xffffu) | (d10_1 << 16);
  DB32(a0g + 8)  = (d10_3 << 16) | (d10_2 & 0xffffu);
  gte_swc2(a0g + 16, 11);
  return a0g;
}

/* ======== 0x80194f40 ======== */
/* Ctrl-word variant of gte_rtps3_sxy_pack: a0 = ptr to five u32 GTE
 * control words (loaded into ctc2 ctrl 0-4 on entry), a1 = ptr to the
 * packed vertex block (same s16/u32 field layout as 0x80194a70's
 * record). Three RTPS passes (cop2 func 0x0486012) with SXY read-backs
 * (data 9/10/11) repacked as (hi<<16 | lo&0xffff) u32 words and written
 * back into ctc2 ctrl 0-4 — NOT to memory. Returns a0 ($v0 = $a0).
 * Verified against generated br2eu_func_80194F40 + EXE disasm:
 *   entry ctc2 : ctrl0..4 = a0[0..4]
 *   pass1 in   : d0=lhu@0|(w@4&ffff0000)   d1=w@0xc   (lhu = zero-ext)
 *   pass2 in   : d0=lhu@2|(w@8<<16)         d1=lh@0xe  (lh = sign-ext)
 *   pass3 in   : d0=lhu@4|(w@8&ffff0000)    d1=w@0x10
 *   exit ctc2  : ctrl0=(d9_2<<16)|(d9_1&ffff)  ctrl1=(d9_3&ffff)|(d10_1<<16)
 *                ctrl2=(d10_3<<16)|(d11_2&ffff) ctrl3=(d11_2<<16)|(d11_1&ffff)
 *                ctrl4=d11_3 (raw v2 SXY2) */
uint32_t gte_rtps3_sxy_pack_ctrl(uint32_t a0g, uint32_t a1g)
{
  uint8_t *v = (uint8_t *)guest_at(a1g);
  uint32_t d9_1, d10_1, d11_1, d9_2, d10_2, d11_2;
  uint32_t d9_3, d10_3, d11_3;

  /* entry: five control words a0[0..4] -> ctc2 ctrl 0..4 */
  setCopControlWord(2, GTE_CREG(0), DB32(a0g + 0));
  setCopControlWord(2, GTE_CREG(1), DB32(a0g + 4));
  setCopControlWord(2, GTE_CREG(2), DB32(a0g + 8));
  setCopControlWord(2, GTE_CREG(3), DB32(a0g + 12));
  setCopControlWord(2, GTE_CREG(4), DB32(a0g + 16));

  /* pass 1: vertex @a1+0 */
  setCopReg(2, GTE_DREG(0),
            (uint32_t)*(uint16_t *)(v + 0) | (DB32(a1g + 4) & 0xffff0000u));
  setCopReg(2, GTE_DREG(1), DB32(a1g + 12));
  copFunction(2, 0x0486012);                    /* RTPS */
  d9_1  = (uint32_t)getCopReg(2, GTE_DREG(9));
  d10_1 = (uint32_t)getCopReg(2, GTE_DREG(10));
  d11_1 = (uint32_t)getCopReg(2, GTE_DREG(11));

  /* pass 2: vertex @a1+2 (lh @+0xe sign-extends) */
  setCopReg(2, GTE_DREG(0),
            (uint32_t)*(uint16_t *)(v + 2) | (DB32(a1g + 8) << 16));
  setCopReg(2, GTE_DREG(1), (uint32_t)(int32_t)*(int16_t *)(v + 14));
  copFunction(2, 0x0486012);                    /* RTPS */
  d9_2  = (uint32_t)getCopReg(2, GTE_DREG(9));
  d10_2 = (uint32_t)getCopReg(2, GTE_DREG(10));
  d11_2 = (uint32_t)getCopReg(2, GTE_DREG(11));

  /* pass 3: vertex @a1+4 */
  setCopReg(2, GTE_DREG(0),
            (uint32_t)*(uint16_t *)(v + 4) | (DB32(a1g + 8) & 0xffff0000u));
  setCopReg(2, GTE_DREG(1), DB32(a1g + 16));
  copFunction(2, 0x0486012);                    /* RTPS */
  d9_3  = (uint32_t)getCopReg(2, GTE_DREG(9));
  d10_3 = (uint32_t)getCopReg(2, GTE_DREG(10));
  d11_3 = (uint32_t)getCopReg(2, GTE_DREG(11));

  /* repack SXY pairs -> ctc2 ctrl 0..4 (guest ctc2 order: t6,t0,t1,t8,t2) */
  setCopControlWord(2, GTE_CREG(0), (d9_2  << 16) | (d9_1  & 0xffffu));
  setCopControlWord(2, GTE_CREG(1), (d9_3  & 0xffffu) | (d10_1 << 16));
  setCopControlWord(2, GTE_CREG(2), (d10_3 << 16) | (d11_2 & 0xffffu));
  setCopControlWord(2, GTE_CREG(3), (d11_2 << 16) | (d11_1 & 0xffffu));
  setCopControlWord(2, GTE_CREG(4), d11_3);
  return a0g;
}

/* ======== 0x80194750 ======== */
/* Three-vector transform+normalize: a0 = 6 x s16 {t,r?}, a1 = 9 x s16 out.
 * Saves ctrl[0,2,4]; loads ctrl[0,2,4]=a0[0..2] (s16), gte[9..11]=
 * a0[3..5]; OP(0x178000C) -> MAC1 saved; ctrl[0,2,4]=a0[3..5];
 * gte[0..2]=a0[3..5]; OP -> MAC2->t0..2; OP again -> MAC3->t0..2;
 * restore ctrl; then three vec3_normalize passes (the shared
 * 0x80194690 core in ani.c) writing normalized s16 triples to
 * out[0..2], out[3..5], out[6..8] (last from the saved MAC1).
 * Verified against generated br2eu_func_80194750. */
void gte_xform_norm3pts(uint32_t a0g, uint32_t a1g)
{
  const int16_t *in = (const int16_t *)guest_at(a0g);
  int16_t *out = (int16_t *)guest_at(a1g);
  CPUState *c = clean_cpu_state();
  int32_t c0, c2, c4;
  uint32_t m0, m1, m2;
  int i;

  if (c == NULL) return;

  c0 = getCopControlWord(2, GTE_CREG(0));
  c2 = getCopControlWord(2, GTE_CREG(2));
  c4 = getCopControlWord(2, GTE_CREG(4));

  /* pass 1: ctrl = in[0..2], IR-vector = in[3..5] */
  setCopControlWord(2, GTE_CREG(0), (uint32_t)in[0]);
  setCopControlWord(2, GTE_CREG(2), (uint32_t)in[1]);
  setCopControlWord(2, GTE_CREG(4), (uint32_t)in[2]);
  setCopReg(2, GTE_DREG(9),  (uint32_t)in[3]);
  setCopReg(2, GTE_DREG(10), (uint32_t)in[4]);
  setCopReg(2, GTE_DREG(11), (uint32_t)in[5]);
  copFunction(2, 0x178000C);
  m0 = (uint32_t)getCopReg(2, GTE_DREG(25));
  m1 = (uint32_t)getCopReg(2, GTE_DREG(26));
  m2 = (uint32_t)getCopReg(2, GTE_DREG(27));

  /* pass 2: ctrl = in[3..5], OP again; MAC -> t0..t2; then
   * gte[0..2] = in[3..5] (IR0..2 for the later normalize read-back). */
  setCopControlWord(2, GTE_CREG(0), (uint32_t)in[3]);
  setCopControlWord(2, GTE_CREG(2), (uint32_t)in[4]);
  setCopControlWord(2, GTE_CREG(4), (uint32_t)in[5]);
  copFunction(2, 0x178000C);
  setCopReg(2, GTE_DREG(0), (uint32_t)in[3]);
  setCopReg(2, GTE_DREG(1), (uint32_t)in[4]);
  setCopReg(2, GTE_DREG(2), (uint32_t)in[5]);

  setCopControlWord(2, GTE_CREG(0), (uint32_t)c0);
  setCopControlWord(2, GTE_CREG(2), (uint32_t)c2);
  setCopControlWord(2, GTE_CREG(4), (uint32_t)c4);

  /* normalize OP#2 MAC -> out[0..2] */
  c->gpr[8]  = (uint32_t)getCopReg(2, GTE_DREG(25));
  c->gpr[9]  = (uint32_t)getCopReg(2, GTE_DREG(26));
  c->gpr[10] = (uint32_t)getCopReg(2, GTE_DREG(27));
  gte_vec3_normalize();
  for (i = 0; i < 3; i++) out[i] = (int16_t)c->gpr[8 + i];

  /* normalize gte[0..2] (IR written above) -> out[3..5] */
  c->gpr[8]  = (uint32_t)getCopReg(2, GTE_DREG(0));
  c->gpr[9]  = (uint32_t)getCopReg(2, GTE_DREG(1));
  c->gpr[10] = (uint32_t)getCopReg(2, GTE_DREG(2));
  gte_vec3_normalize();
  for (i = 0; i < 3; i++) out[3 + i] = (int16_t)c->gpr[8 + i];

  /* normalize saved MAC1 -> out[6..8] */
  c->gpr[8]  = m0;
  c->gpr[9]  = m1;
  c->gpr[10] = m2;
  gte_vec3_normalize();
  for (i = 0; i < 3; i++) out[6 + i] = (int16_t)c->gpr[8 + i];
}

/* ======== 0x80194478 / 0x80195710 ======== */
/* Init-time helpers. Both ported faithfully against the generated
 * bodies; every callee is already clean or a sanctioned bridge:
 *   0x80194478 gte_screen_init_defer: spill ra @0x801B8470, jal
 *     gpu_state_patch_defer, restore, SR |= 0x40000000 (cop0 r12),
 *     then GTE ctrl writes: ctc2 29=0x155, 30=0x100, 26=0x3E8,
 *     27=0xFFFFEF9E (s16 -4194), 28=0x01400000, 24=0, 25=0 — the
 *     generated code routes 26/27/29/30 through gte_write_ctrl
 *     (sign-extend/&0xffff semantics) and stores 24/25/28 directly.
 *   0x80195710 gpu_state_patch_defer: spill ra @0x801FC670, jal
 *     FUN_8018e300 (EnterCriticalSection), BIOS B0 svc 0x56 via the
 *     0xB0 vector, then a 6-word compare of *(v0+24)+40 against the
 *     table @0x801957BC; on match copies the table @0x801957D4 over
 *     it. Tail (both paths): BIOS A0 svc 0x44 (bios_tramp_a044 /
 *     br2_bios_tramp forwards live a0-a3) then FUN_8018e310
 *     (ExitCriticalSection), restore ra, return. */
void gpu_state_patch_defer(void);
uint32_t gte_screen_init_defer(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t ra;
  if (!c) return 0;
  ra = c->gpr[31];
  GP32(0x801B8470) = ra;               /* sw ra scratch slot */
  gpu_state_patch_defer();
  c->cop0[12] = (c->cop0[12] | 0x40000000u);
  gte_write_ctrl(c, 29, 0x155u);
  gte_write_ctrl(c, 30, 0x100u);
  gte_write_ctrl(c, 26, 0x3E8u);
  gte_write_ctrl(c, 27, 0xFFFFEF9Eu);
  c->gte_ctrl[28] = 0x01400000u;
  c->gte_ctrl[24] = 0;
  c->gte_ctrl[25] = 0;
  (void)GP32(0x801B8470);               /* lw ra restore — dead in C */
  return 0;
}

void gpu_state_patch_defer(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t v0, base;
  int i, same = 1;
  extern void FUN_8018e300(void);
  extern void FUN_8018e310(uint32_t);
  extern void br2_bios_tramp(uint32_t,uint32_t);
  if (!c) return;
  GP32(0x801FC670) = c->gpr[31];        /* sw ra scratch slot */
  FUN_8018e300();                       /* jal 0x8018E300: EnterCriticalSection */
  br2_bios_tramp(0xb0, 0x56);           /* jalr 0xB0, $t1=0x56 — B0 svc 0x56 */
  v0 = c->gpr[2];                       /* BIOS $v0 */
  base = GP32(v0 + 24) + 40;
  /* Generated compares TWELVE words: table cursor runs
   * 0x801957BC -> 0x801957EC (exclusive) against base+0..44. Only on a
   * full 12-word match does it copy the SIX words @0x801957D4
   * (-> 0x801957EC) over base+0..20 — the copy source overlaps the
   * tail of the compare table. */
  for (i = 0; i < 12; i++) {
    if (GP32(0x801957BCu + (uint32_t)i * 4u) != GP32(base + (uint32_t)i * 4u)) {
      same = 0;
      break;
    }
  }
  if (same) {
    for (i = 0; i < 6; i++)
      GP32(base + (uint32_t)i * 4u) = GP32(0x801957D4u + (uint32_t)i * 4u);
  }
  br2_bios_tramp(0xa0, 0x44);           /* jal 0x801957F0: BIOS A0 svc 0x44 */
  FUN_8018e310(0);                      /* jal 0x8018E310: ExitCriticalSection */
}

/* =====================================================================
 * 0x801AAE08 — GTE transform-packet executor (per-frame vertex path).
 *
 * Faithful clean port of the CPS-split generated family
 * br2eu_func_801AAE08 / _801AB470 / _801AB84C / _801ABD94 — ONE logical
 * guest function spanning 0x801AAE08..0x801AC06C (~1160 insns) that the
 * recompiler split at taken-branch boundaries. The fragments are internal
 * CPS continuations only (no external jal targets — verified against
 * generated/SLES_017.22_dispatch.c and every generated source file), so a single
 * clean body covering the whole CFG is correct.
 *
 * Structure: packet descriptor at a0. Flag words select which transform
 * stages run:
 *   lw  [a0+0x20]==0 -> stage-C entry (0x801AB84C)
 *   else copy [a0+0x14..0x1C] -> scratch 0x1F8003F4..FC, then
 *   lw  [a0+0x24]==0 -> stage-B entry (0x801AB470)
 *   else lh  [a0+0x28]==0 -> skip stage-A extra (join 0x801AB07C)
 * Each stage loads GTE ctrl matrices (CT0..4 / CT8..12 / CT0..7), runs
 * RTPS-family vertex transforms (SXY triplets via data 9/10/11), and
 * writes results back to the 0x1F8003E0 scratchpad / OT packet area.
 * ===================================================================== */
uint32_t gte_xform_packet(uint32_t a0)
{
    CPUState *c = clean_cpu_state();
    uint32_t g[32] = {0};
    g[4] = a0;
    g[28] = c ? c->gpr[28] : 0;   /* $gp — read by lw 52($gp) matrix ptr */
    g[3] = 528482304u;
    g[3] = g[3] | 992;
    g[2] = br2_ld32(g[4] + (int32_t)32); /* 801AAE10 */
    { int _bc1 = (g[2] == g[0]);
      g[5] = g[4] + g[0];
      if (_bc1) goto L_801AB84Cu; } /* beq 801AAE18 */
    g[2] = br2_ld32(g[4] + (int32_t)20); /* 801AAE20 */
    br2_st32(g[3] + (int32_t)20, g[2]); /* 801AAE28 */
    g[2] = br2_ld32(g[4] + (int32_t)24); /* 801AAE2C */
    br2_st32(g[3] + (int32_t)24, g[2]); /* 801AAE34 */
    g[2] = br2_ld32(g[4] + (int32_t)28); /* 801AAE38 */
    br2_st32(g[3] + (int32_t)28, g[2]); /* 801AAE40 */
    g[2] = br2_ld32(g[4] + (int32_t)36); /* 801AAE44 */
    { int _bc2 = (g[2] == g[0]);
      if (_bc2) goto L_801AB470u; } /* beq 801AAE4C */
    g[2] = (uint32_t)(int32_t)(int16_t)br2_ld16(g[4] + (int32_t)40); /* 801AAE54 */
    { int _bc3 = (g[2] == g[0]);
      if (_bc3) goto L_801AB07Cu; } /* beq 801AAE5C */
    g[6] = br2_ld32(g[5] + (int32_t)36); /* 801AAE64 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AAE6C */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AAE70 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AAE74 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AAE78 */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AAE7C */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AAE80 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AAE84 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AAE88 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AAE8C */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AAE90 */
    g[12] = br2_ld16(g[5] + (int32_t)0); /* 801AAE94 */
    g[13] = br2_ld16(g[5] + (int32_t)6); /* 801AAE98 */
    g[14] = br2_ld16(g[5] + (int32_t)12); /* 801AAE9C */
    gte_write_data(c, 9, g[12]); /* mtc2 801AAEA0 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AAEA4 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AAEA8 */
    gte_execute(c, 0x049E012u); /* cop2 801AAEB4 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AAEB8 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AAEBC */
    g[14] = gte_read_data(c, 11); /* mfc2 801AAEC0 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AAEC4 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AAEC8 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AAECC */
    g[2] = g[4] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AAED4 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AAED8 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AAEDC */
    gte_write_data(c, 9, g[12]); /* mtc2 801AAEE0 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AAEE4 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AAEE8 */
    gte_execute(c, 0x049E012u); /* cop2 801AAEF4 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801AAF00 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AAF04 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AAF08 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AAF0C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AAF10 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AAF14 */
    g[2] = g[4] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AAF1C */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AAF20 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AAF24 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AAF28 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AAF2C */
    gte_write_data(c, 11, g[14]); /* mtc2 801AAF30 */
    gte_execute(c, 0x049E012u); /* cop2 801AAF3C */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801AAF48 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AAF4C */
    g[14] = gte_read_data(c, 11); /* mfc2 801AAF50 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AAF54 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AAF58 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AAF5C */
    g[6] = br2_ld32(g[28] + (int32_t)52); /* 801AAF60 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AAF68 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AAF6C */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AAF70 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AAF74 */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AAF78 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AAF7C */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AAF80 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AAF84 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AAF88 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AAF8C */
    g[12] = br2_ld16(g[3] + (int32_t)0); /* 801AAF90 */
    g[13] = br2_ld16(g[3] + (int32_t)6); /* 801AAF94 */
    g[14] = br2_ld16(g[3] + (int32_t)12); /* 801AAF98 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AAF9C */
    gte_write_data(c, 10, g[13]); /* mtc2 801AAFA0 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AAFA4 */
    gte_execute(c, 0x049E012u); /* cop2 801AAFB0 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AAFB4 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AAFB8 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AAFBC */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AAFC0 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AAFC4 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AAFC8 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AAFD4 */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AAFD8 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AAFDC */
    gte_write_data(c, 9, g[12]); /* mtc2 801AAFE0 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AAFE4 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AAFE8 */
    gte_execute(c, 0x049E012u); /* cop2 801AAFF4 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AAFF8 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AAFFC */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB000 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB004 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB008 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB00C */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB018 */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB01C */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB020 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB024 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB028 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB02C */
    gte_execute(c, 0x049E012u); /* cop2 801AB038 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB03C */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB040 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB044 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB048 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB04C */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB050 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801AB054 */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801AB058 */
    c->gte_ctrl[8] = g[12]; /* ctc2 801AB05C */
    c->gte_ctrl[9] = g[13]; /* ctc2 801AB060 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801AB064 */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801AB068 */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801AB06C */
    c->gte_ctrl[10] = g[12]; /* ctc2 801AB070 */
    c->gte_ctrl[11] = g[13]; /* ctc2 801AB074 */
    gte_write_ctrl(c, 12, g[14]); /* ctc2 801AB078 */
L_801AB07Cu:
    g[12] = br2_ld32(g[5] + (int32_t)0); /* 801AB07C */
    g[13] = br2_ld32(g[5] + (int32_t)4); /* 801AB080 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB084 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB088 */
    g[12] = br2_ld32(g[5] + (int32_t)8); /* 801AB08C */
    g[13] = br2_ld32(g[5] + (int32_t)12); /* 801AB090 */
    g[14] = br2_ld32(g[5] + (int32_t)16); /* 801AB094 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB098 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB09C */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB0A0 */
    g[6] = br2_ld32(g[5] + (int32_t)32); /* 801AB0A4 */
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB0AC */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB0B0 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB0B4 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB0B8 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB0BC */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB0C0 */
    gte_execute(c, 0x049E012u); /* cop2 801AB0CC */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB0D0 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB0D4 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB0D8 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB0DC */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB0E0 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB0E4 */
    g[2] = br2_ld32(g[4] + (int32_t)32); /* 801AB0E8 */
    g[2] = g[2] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB0F4 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB0F8 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB0FC */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB100 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB104 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB108 */
    gte_execute(c, 0x049E012u); /* cop2 801AB114 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB120 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB124 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB128 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB12C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB130 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB134 */
    g[2] = br2_ld32(g[4] + (int32_t)32); /* 801AB138 */
    g[2] = g[2] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB144 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB148 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB14C */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB150 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB154 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB158 */
    gte_execute(c, 0x049E012u); /* cop2 801AB164 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB170 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB174 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB178 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB17C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB180 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB184 */
    g[6] = br2_ld32(g[5] + (int32_t)36); /* 801AB188 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AB190 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AB194 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB198 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB19C */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AB1A0 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AB1A4 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AB1A8 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB1AC */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB1B0 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB1B4 */
    g[12] = br2_ld16(g[3] + (int32_t)0); /* 801AB1B8 */
    g[13] = br2_ld16(g[3] + (int32_t)6); /* 801AB1BC */
    g[14] = br2_ld16(g[3] + (int32_t)12); /* 801AB1C0 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB1C4 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB1C8 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB1CC */
    gte_execute(c, 0x049E012u); /* cop2 801AB1D8 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB1DC */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB1E0 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB1E4 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB1E8 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB1EC */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB1F0 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB1FC */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB200 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB204 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB208 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB20C */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB210 */
    gte_execute(c, 0x049E012u); /* cop2 801AB21C */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB220 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB224 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB228 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB22C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB230 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB234 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB240 */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB244 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB248 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB24C */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB250 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB254 */
    gte_execute(c, 0x049E012u); /* cop2 801AB260 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB264 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB268 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB26C */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB270 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB274 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB278 */
    g[5] = br2_ld32(g[5] + (int32_t)36); /* 801AB27C */
    g[12] = br2_ld32(g[5] + (int32_t)20); /* 801AB284 */
    g[13] = br2_ld32(g[5] + (int32_t)24); /* 801AB288 */
    c->gte_ctrl[5] = g[12]; /* ctc2 801AB28C */
    g[14] = br2_ld32(g[5] + (int32_t)28); /* 801AB290 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801AB294 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801AB298 */
    g[6] = 528482304u;
    g[6] = g[6] | 1012;
    g[13] = br2_ld16(g[6] + (int32_t)4); /* 801AB2A4 */
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB2A8 */
    g[13] = g[13] << 16;
    g[12] = g[12] | g[13];
    c->gte_data[0] = g[12]; /* mtc2 801AB2B4 */
    gte_write_data(c, 1, br2_ld32(g[6] + (int32_t)8)); /* lwc2 801AB2B8 */
    gte_execute(c, 0x0480012u); /* cop2 801AB2C4 */
    br2_st32(g[6] + (int32_t)0, c->gte_data[25]); /* swc2 801AB2C8 */
    br2_st32(g[6] + (int32_t)4, c->gte_data[26]); /* swc2 801AB2CC */
    br2_st32(g[6] + (int32_t)8, c->gte_data[27]); /* swc2 801AB2D0 */
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801AB2D8 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AB2E0 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AB2E4 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB2E8 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB2EC */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AB2F0 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AB2F4 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AB2F8 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB2FC */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB300 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB304 */
    g[12] = br2_ld16(g[3] + (int32_t)0); /* 801AB308 */
    g[13] = br2_ld16(g[3] + (int32_t)6); /* 801AB30C */
    g[14] = br2_ld16(g[3] + (int32_t)12); /* 801AB310 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB314 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB318 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB31C */
    gte_execute(c, 0x049E012u); /* cop2 801AB328 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB32C */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB330 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB334 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB338 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB33C */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB340 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB34C */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB350 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB354 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB358 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB35C */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB360 */
    gte_execute(c, 0x049E012u); /* cop2 801AB36C */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB370 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB374 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB378 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB37C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB380 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB384 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB390 */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB394 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB398 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB39C */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB3A0 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB3A4 */
    gte_execute(c, 0x049E012u); /* cop2 801AB3B0 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB3B4 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB3B8 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB3BC */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB3C0 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB3C4 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB3C8 */
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801AB3D0 */
    g[12] = br2_ld32(g[6] + (int32_t)20); /* 801AB3D8 */
    g[13] = br2_ld32(g[6] + (int32_t)24); /* 801AB3DC */
    c->gte_ctrl[5] = g[12]; /* ctc2 801AB3E0 */
    g[14] = br2_ld32(g[6] + (int32_t)28); /* 801AB3E4 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801AB3E8 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801AB3EC */
    g[6] = 528482304u;
    g[6] = g[6] | 1012;
    g[13] = br2_ld16(g[6] + (int32_t)4); /* 801AB3F8 */
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB3FC */
    g[13] = g[13] << 16;
    g[12] = g[12] | g[13];
    c->gte_data[0] = g[12]; /* mtc2 801AB408 */
    gte_write_data(c, 1, br2_ld32(g[6] + (int32_t)8)); /* lwc2 801AB40C */
    gte_execute(c, 0x0480012u); /* cop2 801AB418 */
    br2_st32(g[6] + (int32_t)0, c->gte_data[25]); /* swc2 801AB41C */
    br2_st32(g[6] + (int32_t)4, c->gte_data[26]); /* swc2 801AB420 */
    br2_st32(g[6] + (int32_t)8, c->gte_data[27]); /* swc2 801AB424 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801AB428 */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801AB42C */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB430 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB434 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801AB438 */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801AB43C */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801AB440 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB444 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB448 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB44C */
    g[12] = br2_ld32(g[3] + (int32_t)20); /* 801AB450 */
    g[13] = br2_ld32(g[3] + (int32_t)24); /* 801AB454 */
    c->gte_ctrl[5] = g[12]; /* ctc2 801AB458 */
    g[14] = br2_ld32(g[3] + (int32_t)28); /* 801AB45C */
    c->gte_ctrl[6] = g[13]; /* ctc2 801AB460 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801AB464 */
    return g[2]; /* jr $ra 801AB468 */
L_801AB470u:
    g[2] = (uint32_t)(int32_t)(int16_t)br2_ld16(g[4] + (int32_t)40); /* 801AB470 */
    { int _bc4 = (g[2] == g[0]);
      if (_bc4) goto L_801AB5A4u; } /* beq 801AB478 */
    g[6] = br2_ld32(g[28] + (int32_t)52); /* 801AB480 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AB488 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AB48C */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB490 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB494 */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AB498 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AB49C */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AB4A0 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB4A4 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB4A8 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB4AC */
    g[12] = br2_ld16(g[5] + (int32_t)0); /* 801AB4B0 */
    g[13] = br2_ld16(g[5] + (int32_t)6); /* 801AB4B4 */
    g[14] = br2_ld16(g[5] + (int32_t)12); /* 801AB4B8 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB4BC */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB4C0 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB4C4 */
    gte_execute(c, 0x049E012u); /* cop2 801AB4D0 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB4D4 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB4D8 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB4DC */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB4E0 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB4E4 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB4E8 */
    g[2] = g[4] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB4F0 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB4F4 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB4F8 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB4FC */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB500 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB504 */
    gte_execute(c, 0x049E012u); /* cop2 801AB510 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB51C */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB520 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB524 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB528 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB52C */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB530 */
    g[2] = g[4] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB538 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB53C */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB540 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB544 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB548 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB54C */
    gte_execute(c, 0x049E012u); /* cop2 801AB558 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB564 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB568 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB56C */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB570 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB574 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB578 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801AB57C */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801AB580 */
    c->gte_ctrl[8] = g[12]; /* ctc2 801AB584 */
    c->gte_ctrl[9] = g[13]; /* ctc2 801AB588 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801AB58C */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801AB590 */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801AB594 */
    c->gte_ctrl[10] = g[12]; /* ctc2 801AB598 */
    c->gte_ctrl[11] = g[13]; /* ctc2 801AB59C */
    gte_write_ctrl(c, 12, g[14]); /* ctc2 801AB5A0 */
L_801AB5A4u:
    g[12] = br2_ld32(g[5] + (int32_t)0); /* 801AB5A4 */
    g[13] = br2_ld32(g[5] + (int32_t)4); /* 801AB5A8 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB5AC */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB5B0 */
    g[12] = br2_ld32(g[5] + (int32_t)8); /* 801AB5B4 */
    g[13] = br2_ld32(g[5] + (int32_t)12); /* 801AB5B8 */
    g[14] = br2_ld32(g[5] + (int32_t)16); /* 801AB5BC */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB5C0 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB5C4 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB5C8 */
    g[5] = br2_ld32(g[5] + (int32_t)32); /* 801AB5CC */
    g[12] = br2_ld16(g[5] + (int32_t)0); /* 801AB5D4 */
    g[13] = br2_ld16(g[5] + (int32_t)6); /* 801AB5D8 */
    g[14] = br2_ld16(g[5] + (int32_t)12); /* 801AB5DC */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB5E0 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB5E4 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB5E8 */
    gte_execute(c, 0x049E012u); /* cop2 801AB5F4 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB5F8 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB5FC */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB600 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB604 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB608 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB60C */
    g[2] = br2_ld32(g[4] + (int32_t)32); /* 801AB610 */
    g[2] = g[2] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB61C */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB620 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB624 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB628 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB62C */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB630 */
    gte_execute(c, 0x049E012u); /* cop2 801AB63C */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB648 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB64C */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB650 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB654 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB658 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB65C */
    g[2] = br2_ld32(g[4] + (int32_t)32); /* 801AB660 */
    g[2] = g[2] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB66C */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB670 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB674 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB678 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB67C */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB680 */
    gte_execute(c, 0x049E012u); /* cop2 801AB68C */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB698 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB69C */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB6A0 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB6A4 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB6A8 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB6AC */
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801AB6B4 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AB6BC */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AB6C0 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB6C4 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB6C8 */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AB6CC */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AB6D0 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AB6D4 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB6D8 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB6DC */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB6E0 */
    g[12] = br2_ld16(g[3] + (int32_t)0); /* 801AB6E4 */
    g[13] = br2_ld16(g[3] + (int32_t)6); /* 801AB6E8 */
    g[14] = br2_ld16(g[3] + (int32_t)12); /* 801AB6EC */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB6F0 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB6F4 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB6F8 */
    gte_execute(c, 0x049E012u); /* cop2 801AB704 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB708 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB70C */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB710 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB714 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB718 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB71C */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB728 */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB72C */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB730 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB734 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB738 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB73C */
    gte_execute(c, 0x049E012u); /* cop2 801AB748 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB74C */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB750 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB754 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB758 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB75C */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB760 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB76C */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB770 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB774 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB778 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB77C */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB780 */
    gte_execute(c, 0x049E012u); /* cop2 801AB78C */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB790 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB794 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB798 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB79C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB7A0 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB7A4 */
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801AB7AC */
    g[12] = br2_ld32(g[6] + (int32_t)20); /* 801AB7B4 */
    g[13] = br2_ld32(g[6] + (int32_t)24); /* 801AB7B8 */
    c->gte_ctrl[5] = g[12]; /* ctc2 801AB7BC */
    g[14] = br2_ld32(g[6] + (int32_t)28); /* 801AB7C0 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801AB7C4 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801AB7C8 */
    g[6] = 528482304u;
    g[6] = g[6] | 1012;
    g[13] = br2_ld16(g[6] + (int32_t)4); /* 801AB7D4 */
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB7D8 */
    g[13] = g[13] << 16;
    g[12] = g[12] | g[13];
    c->gte_data[0] = g[12]; /* mtc2 801AB7E4 */
    gte_write_data(c, 1, br2_ld32(g[6] + (int32_t)8)); /* lwc2 801AB7E8 */
    gte_execute(c, 0x0480012u); /* cop2 801AB7F4 */
    br2_st32(g[6] + (int32_t)0, c->gte_data[25]); /* swc2 801AB7F8 */
    br2_st32(g[6] + (int32_t)4, c->gte_data[26]); /* swc2 801AB7FC */
    br2_st32(g[6] + (int32_t)8, c->gte_data[27]); /* swc2 801AB800 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801AB804 */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801AB808 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB80C */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB810 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801AB814 */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801AB818 */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801AB81C */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB820 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB824 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB828 */
    g[12] = br2_ld32(g[3] + (int32_t)20); /* 801AB82C */
    g[13] = br2_ld32(g[3] + (int32_t)24); /* 801AB830 */
    c->gte_ctrl[5] = g[12]; /* ctc2 801AB834 */
    g[14] = br2_ld32(g[3] + (int32_t)28); /* 801AB838 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801AB83C */
    c->gte_ctrl[7] = g[14]; /* ctc2 801AB840 */
    return g[2]; /* jr $ra 801AB844 */
L_801AB84Cu:
    g[2] = br2_ld32(g[4] + (int32_t)36); /* 801AB84C */
    { int _bc5 = (g[2] == g[0]);
      if (_bc5) goto L_801ABD94u; } /* beq 801AB854 */
    g[2] = (uint32_t)(int32_t)(int16_t)br2_ld16(g[4] + (int32_t)40); /* 801AB85C */
    { int _bc6 = (g[2] == g[0]);
      if (_bc6) goto L_801ABA84u; } /* beq 801AB864 */
    g[6] = br2_ld32(g[5] + (int32_t)36); /* 801AB86C */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AB874 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AB878 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB87C */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB880 */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AB884 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AB888 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AB88C */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB890 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB894 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB898 */
    g[12] = br2_ld16(g[5] + (int32_t)0); /* 801AB89C */
    g[13] = br2_ld16(g[5] + (int32_t)6); /* 801AB8A0 */
    g[14] = br2_ld16(g[5] + (int32_t)12); /* 801AB8A4 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB8A8 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB8AC */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB8B0 */
    gte_execute(c, 0x049E012u); /* cop2 801AB8BC */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB8C0 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB8C4 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB8C8 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB8CC */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB8D0 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB8D4 */
    g[2] = g[4] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB8DC */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB8E0 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB8E4 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB8E8 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB8EC */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB8F0 */
    gte_execute(c, 0x049E012u); /* cop2 801AB8FC */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB908 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB90C */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB910 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB914 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB918 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB91C */
    g[2] = g[4] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801AB924 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801AB928 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801AB92C */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB930 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB934 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB938 */
    gte_execute(c, 0x049E012u); /* cop2 801AB944 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801AB950 */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB954 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB958 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801AB95C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801AB960 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801AB964 */
    g[6] = br2_ld32(g[28] + (int32_t)52); /* 801AB968 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801AB970 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801AB974 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AB978 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AB97C */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801AB980 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801AB984 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801AB988 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AB98C */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AB990 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AB994 */
    g[12] = br2_ld16(g[3] + (int32_t)0); /* 801AB998 */
    g[13] = br2_ld16(g[3] + (int32_t)6); /* 801AB99C */
    g[14] = br2_ld16(g[3] + (int32_t)12); /* 801AB9A0 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB9A4 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB9A8 */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB9AC */
    gte_execute(c, 0x049E012u); /* cop2 801AB9B8 */
    g[12] = gte_read_data(c, 9); /* mfc2 801AB9BC */
    g[13] = gte_read_data(c, 10); /* mfc2 801AB9C0 */
    g[14] = gte_read_data(c, 11); /* mfc2 801AB9C4 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801AB9C8 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801AB9CC */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801AB9D0 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801AB9DC */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801AB9E0 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801AB9E4 */
    gte_write_data(c, 9, g[12]); /* mtc2 801AB9E8 */
    gte_write_data(c, 10, g[13]); /* mtc2 801AB9EC */
    gte_write_data(c, 11, g[14]); /* mtc2 801AB9F0 */
    gte_execute(c, 0x049E012u); /* cop2 801AB9FC */
    g[12] = gte_read_data(c, 9); /* mfc2 801ABA00 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABA04 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABA08 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABA0C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABA10 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABA14 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801ABA20 */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801ABA24 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801ABA28 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABA2C */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABA30 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABA34 */
    gte_execute(c, 0x049E012u); /* cop2 801ABA40 */
    g[12] = gte_read_data(c, 9); /* mfc2 801ABA44 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABA48 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABA4C */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABA50 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABA54 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABA58 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801ABA5C */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801ABA60 */
    c->gte_ctrl[8] = g[12]; /* ctc2 801ABA64 */
    c->gte_ctrl[9] = g[13]; /* ctc2 801ABA68 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801ABA6C */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801ABA70 */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801ABA74 */
    c->gte_ctrl[10] = g[12]; /* ctc2 801ABA78 */
    c->gte_ctrl[11] = g[13]; /* ctc2 801ABA7C */
    gte_write_ctrl(c, 12, g[14]); /* ctc2 801ABA80 */
L_801ABA84u:
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801ABA88 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801ABA90 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801ABA94 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801ABA98 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801ABA9C */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801ABAA0 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801ABAA4 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801ABAA8 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801ABAAC */
    c->gte_ctrl[3] = g[13]; /* ctc2 801ABAB0 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801ABAB4 */
    g[6] = br2_ld32(g[5] + (int32_t)36); /* 801ABAB8 */
    g[12] = br2_ld16(g[6] + (int32_t)0); /* 801ABAC0 */
    g[13] = br2_ld16(g[6] + (int32_t)6); /* 801ABAC4 */
    g[14] = br2_ld16(g[6] + (int32_t)12); /* 801ABAC8 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABACC */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABAD0 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABAD4 */
    gte_execute(c, 0x049E012u); /* cop2 801ABAE0 */
    g[12] = gte_read_data(c, 9); /* mfc2 801ABAE4 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABAE8 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABAEC */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801ABAF0 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801ABAF4 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801ABAF8 */
    g[2] = br2_ld32(g[4] + (int32_t)36); /* 801ABAFC */
    g[2] = g[2] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABB08 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABB0C */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABB10 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABB14 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABB18 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABB1C */
    gte_execute(c, 0x049E012u); /* cop2 801ABB28 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABB34 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABB38 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABB3C */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABB40 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABB44 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABB48 */
    g[2] = br2_ld32(g[4] + (int32_t)36); /* 801ABB4C */
    g[2] = g[2] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABB58 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABB5C */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABB60 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABB64 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABB68 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABB6C */
    gte_execute(c, 0x049E012u); /* cop2 801ABB78 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABB84 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABB88 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABB8C */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABB90 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABB94 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABB98 */
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801ABBA0 */
    g[12] = br2_ld32(g[6] + (int32_t)20); /* 801ABBA8 */
    g[13] = br2_ld32(g[6] + (int32_t)24); /* 801ABBAC */
    c->gte_ctrl[5] = g[12]; /* ctc2 801ABBB0 */
    g[14] = br2_ld32(g[6] + (int32_t)28); /* 801ABBB4 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801ABBB8 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801ABBBC */
    g[2] = br2_ld32(g[4] + (int32_t)36); /* 801ABBC0 */
    g[2] = g[2] + (uint32_t)(int32_t)20;
    g[13] = br2_ld16(g[2] + (int32_t)4); /* 801ABBCC */
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABBD0 */
    g[13] = g[13] << 16;
    g[12] = g[12] | g[13];
    c->gte_data[0] = g[12]; /* mtc2 801ABBDC */
    gte_write_data(c, 1, br2_ld32(g[2] + (int32_t)8)); /* lwc2 801ABBE0 */
    gte_execute(c, 0x0480012u); /* cop2 801ABBEC */
    g[6] = 528482304u;
    g[6] = g[6] | 1012;
    br2_st32(g[6] + (int32_t)0, c->gte_data[25]); /* swc2 801ABBF8 */
    br2_st32(g[6] + (int32_t)4, c->gte_data[26]); /* swc2 801ABBFC */
    br2_st32(g[6] + (int32_t)8, c->gte_data[27]); /* swc2 801ABC00 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801ABC04 */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801ABC08 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801ABC0C */
    c->gte_ctrl[1] = g[13]; /* ctc2 801ABC10 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801ABC14 */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801ABC18 */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801ABC1C */
    c->gte_ctrl[2] = g[12]; /* ctc2 801ABC20 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801ABC24 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801ABC28 */
    g[12] = br2_ld16(g[5] + (int32_t)0); /* 801ABC2C */
    g[13] = br2_ld16(g[5] + (int32_t)6); /* 801ABC30 */
    g[14] = br2_ld16(g[5] + (int32_t)12); /* 801ABC34 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABC38 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABC3C */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABC40 */
    gte_execute(c, 0x049E012u); /* cop2 801ABC4C */
    g[12] = gte_read_data(c, 9); /* mfc2 801ABC50 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABC54 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABC58 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801ABC5C */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801ABC60 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801ABC64 */
    g[2] = g[4] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABC6C */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABC70 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABC74 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABC78 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABC7C */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABC80 */
    gte_execute(c, 0x049E012u); /* cop2 801ABC8C */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABC98 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABC9C */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABCA0 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABCA4 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABCA8 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABCAC */
    g[2] = g[4] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABCB4 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABCB8 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABCBC */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABCC0 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABCC4 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABCC8 */
    gte_execute(c, 0x049E012u); /* cop2 801ABCD4 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABCE0 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABCE4 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABCE8 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABCEC */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABCF0 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABCF4 */
    g[12] = br2_ld32(g[3] + (int32_t)20); /* 801ABCF8 */
    g[13] = br2_ld32(g[3] + (int32_t)24); /* 801ABCFC */
    c->gte_ctrl[5] = g[12]; /* ctc2 801ABD00 */
    g[14] = br2_ld32(g[3] + (int32_t)28); /* 801ABD04 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801ABD08 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801ABD0C */
    g[2] = g[4] + (uint32_t)(int32_t)20;
    g[13] = br2_ld16(g[2] + (int32_t)4); /* 801ABD14 */
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABD18 */
    g[13] = g[13] << 16;
    g[12] = g[12] | g[13];
    c->gte_data[0] = g[12]; /* mtc2 801ABD24 */
    gte_write_data(c, 1, br2_ld32(g[2] + (int32_t)8)); /* lwc2 801ABD28 */
    gte_execute(c, 0x0480012u); /* cop2 801ABD34 */
    g[6] = 528482304u;
    g[6] = g[6] | 1012;
    br2_st32(g[6] + (int32_t)0, c->gte_data[25]); /* swc2 801ABD40 */
    br2_st32(g[6] + (int32_t)4, c->gte_data[26]); /* swc2 801ABD44 */
    br2_st32(g[6] + (int32_t)8, c->gte_data[27]); /* swc2 801ABD48 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801ABD4C */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801ABD50 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801ABD54 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801ABD58 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801ABD5C */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801ABD60 */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801ABD64 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801ABD68 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801ABD6C */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801ABD70 */
    g[12] = br2_ld32(g[3] + (int32_t)20); /* 801ABD74 */
    g[13] = br2_ld32(g[3] + (int32_t)24); /* 801ABD78 */
    c->gte_ctrl[5] = g[12]; /* ctc2 801ABD7C */
    g[14] = br2_ld32(g[3] + (int32_t)28); /* 801ABD80 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801ABD84 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801ABD88 */
    return g[2]; /* jr $ra 801ABD8C */
L_801ABD94u:
    g[2] = (uint32_t)(int32_t)(int16_t)br2_ld16(g[4] + (int32_t)40); /* 801ABD94 */
    { int _bc7 = (g[2] == g[0]);
      if (_bc7) goto L_801ABEC8u; } /* beq 801ABD9C */
    g[6] = br2_ld32(g[28] + (int32_t)52); /* 801ABDA4 */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801ABDAC */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801ABDB0 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801ABDB4 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801ABDB8 */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801ABDBC */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801ABDC0 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801ABDC4 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801ABDC8 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801ABDCC */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801ABDD0 */
    g[12] = br2_ld16(g[5] + (int32_t)0); /* 801ABDD4 */
    g[13] = br2_ld16(g[5] + (int32_t)6); /* 801ABDD8 */
    g[14] = br2_ld16(g[5] + (int32_t)12); /* 801ABDDC */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABDE0 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABDE4 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABDE8 */
    gte_execute(c, 0x049E012u); /* cop2 801ABDF4 */
    g[12] = gte_read_data(c, 9); /* mfc2 801ABDF8 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABDFC */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABE00 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801ABE04 */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801ABE08 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801ABE0C */
    g[2] = g[4] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABE14 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABE18 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABE1C */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABE20 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABE24 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABE28 */
    gte_execute(c, 0x049E012u); /* cop2 801ABE34 */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABE40 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABE44 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABE48 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABE4C */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABE50 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABE54 */
    g[2] = g[4] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABE5C */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABE60 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABE64 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABE68 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABE6C */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABE70 */
    gte_execute(c, 0x049E012u); /* cop2 801ABE7C */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABE88 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABE8C */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABE90 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABE94 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABE98 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABE9C */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801ABEA0 */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801ABEA4 */
    c->gte_ctrl[8] = g[12]; /* ctc2 801ABEA8 */
    c->gte_ctrl[9] = g[13]; /* ctc2 801ABEAC */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801ABEB0 */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801ABEB4 */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801ABEB8 */
    c->gte_ctrl[10] = g[12]; /* ctc2 801ABEBC */
    c->gte_ctrl[11] = g[13]; /* ctc2 801ABEC0 */
    gte_write_ctrl(c, 12, g[14]); /* ctc2 801ABEC4 */
L_801ABEC8u:
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801ABECC */
    g[12] = br2_ld32(g[6] + (int32_t)0); /* 801ABED4 */
    g[13] = br2_ld32(g[6] + (int32_t)4); /* 801ABED8 */
    c->gte_ctrl[0] = g[12]; /* ctc2 801ABEDC */
    c->gte_ctrl[1] = g[13]; /* ctc2 801ABEE0 */
    g[12] = br2_ld32(g[6] + (int32_t)8); /* 801ABEE4 */
    g[13] = br2_ld32(g[6] + (int32_t)12); /* 801ABEE8 */
    g[14] = br2_ld32(g[6] + (int32_t)16); /* 801ABEEC */
    c->gte_ctrl[2] = g[12]; /* ctc2 801ABEF0 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801ABEF4 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801ABEF8 */
    g[12] = br2_ld16(g[5] + (int32_t)0); /* 801ABEFC */
    g[13] = br2_ld16(g[5] + (int32_t)6); /* 801ABF00 */
    g[14] = br2_ld16(g[5] + (int32_t)12); /* 801ABF04 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABF08 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABF0C */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABF10 */
    gte_execute(c, 0x049E012u); /* cop2 801ABF1C */
    g[12] = gte_read_data(c, 9); /* mfc2 801ABF20 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABF24 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABF28 */
    br2_st16(g[3] + (int32_t)0, (uint16_t)g[12]); /* 801ABF2C */
    br2_st16(g[3] + (int32_t)6, (uint16_t)g[13]); /* 801ABF30 */
    br2_st16(g[3] + (int32_t)12, (uint16_t)g[14]); /* 801ABF34 */
    g[2] = g[4] + (uint32_t)(int32_t)2;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABF3C */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABF40 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABF44 */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABF48 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABF4C */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABF50 */
    gte_execute(c, 0x049E012u); /* cop2 801ABF5C */
    g[6] = 528482304u;
    g[6] = g[6] | 994;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABF68 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABF6C */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABF70 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABF74 */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABF78 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABF7C */
    g[2] = g[4] + (uint32_t)(int32_t)4;
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABF84 */
    g[13] = br2_ld16(g[2] + (int32_t)6); /* 801ABF88 */
    g[14] = br2_ld16(g[2] + (int32_t)12); /* 801ABF8C */
    gte_write_data(c, 9, g[12]); /* mtc2 801ABF90 */
    gte_write_data(c, 10, g[13]); /* mtc2 801ABF94 */
    gte_write_data(c, 11, g[14]); /* mtc2 801ABF98 */
    gte_execute(c, 0x049E012u); /* cop2 801ABFA4 */
    g[6] = 528482304u;
    g[6] = g[6] | 996;
    g[12] = gte_read_data(c, 9); /* mfc2 801ABFB0 */
    g[13] = gte_read_data(c, 10); /* mfc2 801ABFB4 */
    g[14] = gte_read_data(c, 11); /* mfc2 801ABFB8 */
    br2_st16(g[6] + (int32_t)0, (uint16_t)g[12]); /* 801ABFBC */
    br2_st16(g[6] + (int32_t)6, (uint16_t)g[13]); /* 801ABFC0 */
    br2_st16(g[6] + (int32_t)12, (uint16_t)g[14]); /* 801ABFC4 */
    g[6] = 2149318656u;
    g[6] = br2_ld32(g[6] + (int32_t)-6088); /* 801ABFCC */
    g[12] = br2_ld32(g[6] + (int32_t)20); /* 801ABFD4 */
    g[13] = br2_ld32(g[6] + (int32_t)24); /* 801ABFD8 */
    c->gte_ctrl[5] = g[12]; /* ctc2 801ABFDC */
    g[14] = br2_ld32(g[6] + (int32_t)28); /* 801ABFE0 */
    c->gte_ctrl[6] = g[13]; /* ctc2 801ABFE4 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801ABFE8 */
    g[2] = g[4] + (uint32_t)(int32_t)20;
    g[13] = br2_ld16(g[2] + (int32_t)4); /* 801ABFF0 */
    g[12] = br2_ld16(g[2] + (int32_t)0); /* 801ABFF4 */
    g[13] = g[13] << 16;
    g[12] = g[12] | g[13];
    c->gte_data[0] = g[12]; /* mtc2 801AC000 */
    gte_write_data(c, 1, br2_ld32(g[2] + (int32_t)8)); /* lwc2 801AC004 */
    gte_execute(c, 0x0480012u); /* cop2 801AC010 */
    g[6] = 528482304u;
    g[6] = g[6] | 1012;
    br2_st32(g[6] + (int32_t)0, c->gte_data[25]); /* swc2 801AC01C */
    br2_st32(g[6] + (int32_t)4, c->gte_data[26]); /* swc2 801AC020 */
    br2_st32(g[6] + (int32_t)8, c->gte_data[27]); /* swc2 801AC024 */
    g[12] = br2_ld32(g[3] + (int32_t)0); /* 801AC028 */
    g[13] = br2_ld32(g[3] + (int32_t)4); /* 801AC02C */
    c->gte_ctrl[0] = g[12]; /* ctc2 801AC030 */
    c->gte_ctrl[1] = g[13]; /* ctc2 801AC034 */
    g[12] = br2_ld32(g[3] + (int32_t)8); /* 801AC038 */
    g[13] = br2_ld32(g[3] + (int32_t)12); /* 801AC03C */
    g[14] = br2_ld32(g[3] + (int32_t)16); /* 801AC040 */
    c->gte_ctrl[2] = g[12]; /* ctc2 801AC044 */
    c->gte_ctrl[3] = g[13]; /* ctc2 801AC048 */
    gte_write_ctrl(c, 4, g[14]); /* ctc2 801AC04C */
    g[12] = br2_ld32(g[3] + (int32_t)20); /* 801AC050 */
    g[13] = br2_ld32(g[3] + (int32_t)24); /* 801AC054 */
    c->gte_ctrl[5] = g[12]; /* ctc2 801AC058 */
    g[14] = br2_ld32(g[3] + (int32_t)28); /* 801AC05C */
    c->gte_ctrl[6] = g[13]; /* ctc2 801AC060 */
    c->gte_ctrl[7] = g[14]; /* ctc2 801AC064 */
    return g[2]; /* jr $ra 801AC068 */
    return g[2];
}

/* ======== 0x801B0DB8 ======== */

/* scene_node_walk — scene-graph node dispatcher / render-callback walker.
 * Faithful clean port of the CPS-split generated family br2eu_func_801B0DB8
 * plus its internal continuation fragments (0x801B0E60..0x801B0F4 / shared
 * tails 0x801B0E24,0x801B10CC,0x801B10FC,0x801B1124). The fragments are
 * internal CPS/jump-table continuations only: no generated source jal's to
 * them directly, so one clean CFG covers the whole logical function.
 *
 * Guest shape: a0 is the walk context; gp+0x4C/+0x50 cache a0/a0[1]. The
 * loop reads a 32-bit node word at s2, takes opcode = word&0xFF (must be
 * <17) and sub = word>>8, then either updates s2 through a per-opcode
 * {fn,delta} table or tail-dispatches through one of the 0x801CExxx
 * handler tables. Handler calls are guest jalr's reproduced with
 * call_guest(); their delay-slot register updates are applied BEFORE the
 * synchronous call, matching MIPS jalr semantics. */
void scene_node_walk(uint32_t a0)
{
  CPUState *c = clean_cpu_state();
  uint32_t sp, lt12;
  extern uint32_t call_guest(uint32_t,uint32_t,uint32_t);
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  extern void br2_guest_frame_abandon(void);
  extern int br2_bail_pending(void);
  extern void br2_irq_poll(void);
  if (!c) return;
  c->gpr[4] = a0;   /* guest $a0: ctx guest addr (task/save callers pass it raw) */
#define SCN_GPR(n)   (c->gpr[n])
#define SCN_LD32(a)  br2_ld32((uint32_t)(a))
#define SCN_LD16S(a) ((uint32_t)(int32_t)(int16_t)br2_ld16((uint32_t)(a)))
#define SCN_ST32(a,v) br2_st32((uint32_t)(a),(uint32_t)(v))
#define SCN_SRA16(x) ((uint32_t)((int32_t)(uint32_t)(x) >> 16))
#define SCN_CALL(fn) do { \
    (void)call_guest((uint32_t)(fn), SCN_GPR(4), SCN_GPR(5)); \
    if (br2_bail_pending()) { br2_guest_frame_abandon(); return; } \
  } while (0)

  /* addiu sp,sp,-0x30 + sw ra/s3/s2/s1/s0 (801B0DB8-0x801B0DCC). The guest
   * frame is real because the type-7/8/9/12 handlers receive sp+0x10 as an
   * out-param and nested guest calls must not land on it. */
  sp = br2_guest_frame(0x30);
  SCN_ST32(sp + 0x28, SCN_GPR(31));
  SCN_ST32(sp + 0x24, SCN_GPR(19));
  SCN_ST32(sp + 0x20, SCN_GPR(18));
  SCN_ST32(sp + 0x1c, SCN_GPR(17));
  SCN_ST32(sp + 0x18, SCN_GPR(16));

  SCN_GPR(5)  = SCN_LD32(SCN_GPR(4) + 4);            /* lw a1,4(a0) */
  SCN_GPR(2)  = SCN_LD32(0x801bef20u);               /* flag cell */
  SCN_GPR(19) = SCN_LD32(SCN_GPR(4) + 8);            /* s3 = a0[2] base */
  SCN_GPR(3)  = 1;
  SCN_ST32(SCN_GPR(28) + 0x4c, SCN_GPR(4));          /* gp cache: ctx */
  SCN_ST32(SCN_GPR(28) + 0x50, SCN_GPR(5));          /* gp cache: a0[1] */

  {
    int ne = (SCN_GPR(2) != SCN_GPR(3));
    SCN_GPR(2) = SCN_GPR(5);                          /* bne delay: v0=a1 */
    if (ne) {                                         /* 801B0E20 */
      SCN_GPR(18) = SCN_LD32(SCN_GPR(4) + 0xc);       /* s2 = a0[3] */
      goto L_loop;
    }
  }
  /* flag == 1: resolve the first node through a1's header (801B0DF4). */
  SCN_GPR(6) = SCN_LD32(SCN_GPR(2) + 0x14);           /* a2 = *(a1+0x14) */
  if (SCN_GPR(6) == 0) goto L_exit;                   /* beqz a2 -> 801B1124 */
  SCN_GPR(2) = SCN_LD32(SCN_GPR(2));                  /* v0 = *a1 */
  SCN_GPR(3) = SCN_LD32(SCN_GPR(4) + 0xc);            /* v1 = a0[3] */
  SCN_GPR(2) = SCN_GPR(5) + SCN_GPR(2);               /* v0 = a1 + *a1 */
  SCN_GPR(18) = SCN_GPR(5) + SCN_GPR(6);              /* delay: s2 = a1+a2 */
  if (SCN_GPR(3) == SCN_GPR(2)) goto L_exit;          /* beq v1,v0 */
  goto L_loop;                                        /* j 801B0E24 */

L_loop:                                               /* 801B0E24 */
  SCN_GPR(5)  = SCN_LD32(SCN_GPR(18));                /* a1 = *s2 node word */
  SCN_GPR(4)  = SCN_GPR(18) + 2;                      /* a0 = s2+2 (lh base) */
  SCN_GPR(3)  = SCN_GPR(5) & 0x00ff;                  /* opcode */
  SCN_GPR(2)  = (SCN_GPR(3) < 17u) ? 1u : 0u;         /* sltiu v0,17 */
  SCN_GPR(16) = SCN_GPR(5) >> 8;                      /* delay: s0 = word>>8 */
  if (SCN_GPR(2) == 0) {                              /* beqz -> self-loop */
    for (;;) br2_irq_poll();                          /* guest spins w/ IRQs */
  }
  switch (SCN_GPR(3)) {
  case 0x00:                                          /* 801B0E60 */
    SCN_GPR(5) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(3) = 0x801be8ccu;
    goto L_table_call;
  case 0x01:                                          /* 801B0E74 */
    SCN_GPR(2) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(5) = SCN_LD32(SCN_GPR(2));
    SCN_GPR(2) = (SCN_GPR(16) & 0xffu) << 3;
    SCN_GPR(3) = SCN_GPR(19) + SCN_LD16S(SCN_GPR(4) + 4);
    SCN_GPR(6) = SCN_LD32(SCN_GPR(3));                /* a2 */
    SCN_GPR(3) = 0x801bea1cu;
    SCN_GPR(2) += SCN_GPR(3);
    SCN_GPR(3) = SCN_LD32(SCN_GPR(2) + 4);            /* delta */
    SCN_GPR(2) = SCN_LD32(SCN_GPR(2));                /* fn */
    SCN_GPR(18) += SCN_GPR(3);                        /* jalr delay: s2 += d */
    SCN_CALL(SCN_GPR(2));
    goto L_loop;
  case 0x02:                                          /* 801B0EBC */
    SCN_GPR(2) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(5) = SCN_LD32(SCN_GPR(2));
    SCN_GPR(3) = 0x801beaf4u;
    goto L_table_call;
  case 0x03:                                          /* 801B0ED4 */
    SCN_GPR(2) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(5) = SCN_LD32(SCN_GPR(2));
    SCN_GPR(3) = 0x801bece4u;
    goto L_table_call;
  case 0x04:                                          /* 801B0EEC */
    SCN_GPR(3) = ((SCN_GPR(16) & 0xffu) << 3) + 0x801bedb4u;
    SCN_GPR(2) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(5) = SCN_LD32(SCN_GPR(2));
    SCN_GPR(6) = SCN_LD32(SCN_GPR(3) + 4);            /* s2 delta */
    SCN_GPR(18) += SCN_GPR(6);
    SCN_GPR(6) = SCN_GPR(19) + SCN_LD16S(SCN_GPR(4) + 2);
    SCN_GPR(2) = SCN_LD32(SCN_GPR(3));                /* fn */
    SCN_CALL(SCN_GPR(2));                             /* delay: a2 = s3+lh */
    goto L_loop;
  case 0x05:                                          /* 801B0F30 */
    SCN_GPR(5) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(3) = 0x801bee3cu;
    goto L_table_call;
  case 0x06:                                          /* 801B0F44 */
    SCN_GPR(2) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(5) = SCN_LD32(SCN_GPR(2));
    SCN_GPR(3) = 0x801bebecu;
    goto L_table_call;
  case 0x07:                                          /* 801B0F5C */
    SCN_GPR(3) = SCN_GPR(16) & 0xffu;
    if (SCN_GPR(3) == 11u) goto L_exit;               /* sub 0xB: done */
    lt12 = ((int32_t)SCN_GPR(3) < 12) ? 1u : 0u;      /* slti (delay) */
    SCN_GPR(2) = 4;                                   /* delay: v0=4 */
    if (lt12 == 0) goto L_f84;
    if (SCN_GPR(3) == 4u) goto L_f98;
    SCN_GPR(2) = 0x801be89cu;                         /* j FC4 table */
    goto L_fc_call;
L_f84:
    if (SCN_GPR(3) == 12u) {                          /* sub 0xC: jal helper */
      SCN_CALL(0x801a482cu);
      goto L_exit;
    }
    SCN_GPR(2) = 0x801be89cu;
    SCN_GPR(3) = SCN_GPR(16) & 0xffu;                 /* FC0 refresh */
    goto L_fc_call;
L_f98:                                                /* sub 4: commit s2 */
    SCN_GPR(2) = SCN_SRA16(SCN_GPR(5));
    SCN_GPR(3) = SCN_LD32(SCN_GPR(28) + 0x50);
    SCN_GPR(4) = SCN_LD32(SCN_GPR(28) + 0x4c);
    SCN_GPR(18) = SCN_GPR(3) + SCN_GPR(2);
    SCN_ST32(SCN_GPR(4) + 0xc, SCN_GPR(18));
    goto L_exit;
  case 0x08:                                          /* 801B0FE8 */
    SCN_GPR(5) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(3) = 0x801bebd4u;
    goto L_table_call;
  case 0x09:                                          /* 801B0FFC */
    SCN_GPR(2) = 0x801bec24u;
    SCN_GPR(3) = ((SCN_GPR(16) & 0xffu) << 3) + SCN_GPR(2);
    SCN_GPR(2) = SCN_LD32(SCN_GPR(3) + 4);            /* s2 delta */
    SCN_GPR(5) = SCN_LD16S(SCN_GPR(4) + 8);           /* a1 = lh a0+8 */
    SCN_GPR(18) += SCN_GPR(2);
    SCN_GPR(2) = SCN_LD32(SCN_GPR(3));                /* fn */
    SCN_GPR(5) = SCN_GPR(19) + SCN_GPR(5);           /* delay: a1=s3+lh */
    SCN_CALL(SCN_GPR(2));
    goto L_loop;
  case 0x0a:                                          /* 801B1034 */
    SCN_GPR(5) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(3) = 0x801bec44u;
    goto L_table_call;
  case 0x0b:                                          /* 801B1048 */
    SCN_GPR(3) = 0x801bec7cu;
    goto L_table_call;
  case 0x0c:                                          /* 801B1054 */
    SCN_GPR(5) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(6) = SCN_GPR(4) + 2;                      /* a2 = s2+4 */
    SCN_GPR(2) = 0x801bec8cu;
    SCN_GPR(16) = ((SCN_GPR(16) & 0xffu) << 3) + SCN_GPR(2);
    SCN_GPR(2) = SCN_LD32(SCN_GPR(16));               /* fn */
    SCN_GPR(17) = SCN_LD32(SCN_GPR(28) + 0x4c);       /* s1 = ctx */
    SCN_GPR(7)  = SCN_GPR(19);                        /* delay: a3 = s3 */
    SCN_CALL(SCN_GPR(2));
    SCN_GPR(2) = SCN_LD32(SCN_GPR(16) + 4);           /* s2 delta */
    SCN_GPR(3) = SCN_LD32(SCN_GPR(17) + 4);           /* ctx[1] refresh */
    SCN_ST32(SCN_GPR(28) + 0x4c, SCN_GPR(17));
    SCN_ST32(SCN_GPR(28) + 0x50, SCN_GPR(3));
    SCN_GPR(18) += SCN_GPR(2);                        /* j delay: s2 += d */
    goto L_loop;
  case 0x0d:                                          /* 801B109C */
    SCN_GPR(5) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(3) = 0x801becccu;
    goto L_table_call;
  case 0x0e:                                          /* 801B10B0 */
    SCN_GPR(3) = 0x801bee34u;
    goto L_table_call;
  case 0x0f:                                          /* 801B10BC */
    SCN_GPR(5) = SCN_GPR(19) + SCN_SRA16(SCN_GPR(5));
    SCN_GPR(3) = 0x801beeb4u;
    goto L_table_call;
  case 0x10:                                          /* 801B10F4 */
    SCN_GPR(3) = 0x801becb4u;
    goto L_table_call;
  }

L_table_call:                                         /* 801B10CC/10FC */
  SCN_GPR(2) = (SCN_GPR(16) & 0xffu) << 3;
  SCN_GPR(2) += SCN_GPR(3);                           /* entry = base + sub*8 */
  SCN_GPR(3) = SCN_LD32(SCN_GPR(2) + 4);              /* delta */
  SCN_GPR(2) = SCN_LD32(SCN_GPR(2));                  /* fn */
  SCN_GPR(18) += SCN_GPR(3);                          /* jalr delay: s2 += d */
  SCN_CALL(SCN_GPR(2));
  goto L_loop;

L_fc_call:                                            /* 801B0FC4 */
  SCN_GPR(3) = (SCN_GPR(3) << 2) + SCN_GPR(2);        /* base + sub*4 */
  SCN_GPR(2) = SCN_LD32(SCN_GPR(3));                  /* fn */
  SCN_GPR(5) = sp + 0x10;                             /* a1 = &stack_out */
  SCN_ST32(sp + 0x10, SCN_GPR(18));                   /* jalr delay: *out=s2 */
  SCN_CALL(SCN_GPR(2));
  SCN_GPR(18) = SCN_LD32(sp + 0x10);                  /* s2 = *out */
  goto L_loop;

L_exit:                                               /* 801B1124 epilogue */
  SCN_GPR(31) = SCN_LD32(sp + 0x28);
  SCN_GPR(19) = SCN_LD32(sp + 0x24);
  SCN_GPR(18) = SCN_LD32(sp + 0x20);
  SCN_GPR(17) = SCN_LD32(sp + 0x1c);
  SCN_GPR(16) = SCN_LD32(sp + 0x18);
  br2_guest_frame_free();                             /* addiu sp,+0x30; jr ra */
#undef SCN_GPR
#undef SCN_LD32
#undef SCN_LD16S
#undef SCN_ST32
#undef SCN_SRA16
#undef SCN_CALL
  return;
}

/* ======== 0x801B0240 ======== */

/* scene_emit_xform — GTE point transform + emit-record writer. Faithful
 * clean port of the CPS family br2eu_func_801B0240 (internal continuations
 * 0x801B02C4..0x801B03CC are part of the same logical function). Entry args
 * arrive in live regs, NOT a0-a3: $v0 = source vertex record, $v1 = packed
 * input halfword, $a1 = emit-context base (record table stride 40).
 * Allocates a real guest frame because the GTE result halfword pair and
 * the slot index live at sp+8/+16 across the signed divide.
 * Disasm-verified insn-by-insn vs generated/SLES_017.22_full_54.c. */
void scene_emit_xform(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t sp;
  int32_t dvd, dvs;
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  extern void gte_precision_store_word(uint32_t, uint8_t);
  if (!c) return;
#define XF_G(n)     (c->gpr[n])
#define XF_LD16S(a) ((uint32_t)(int32_t)(int16_t)br2_ld16((uint32_t)(a)))
#define XF_LDH(a)   ((uint32_t)br2_ld16((uint32_t)(a)))
#define XF_SH(a,v)  br2_st16((uint32_t)(a), (uint16_t)(v))

  sp = br2_guest_frame(24);                       /* addiu sp,sp,-24 */
  XF_SH(sp + 0, XF_G(3));                         /* sh v1,0(sp)      */
  XF_G(3) = XF_LD16S(XF_G(2) + 6);                /* lh v1,6(v0)      */
  XF_SH(sp + 2, XF_G(3));                         /* sh v1,2(sp)      */
  XF_G(2) = XF_LD16S(XF_G(2) + 10);               /* lh v0,10(v0)     */
  XF_G(3) = br2_ld32(0x801bef00u);                /* lw v1,0x801BEF00 */
  XF_G(7) = XF_G(5);                              /* move a3,a1       */
  XF_SH(sp + 4, XF_G(2));                         /* sh v0,4(sp)      */
  XF_G(2) = XF_G(3) << 2;                         /* sll v0,v1,2      */
  XF_G(2) += XF_G(3);                             /* addu v0,v0,v1    */
  XF_G(2) <<= 3;                                  /* sll v0,v0,3 (*40)*/
  XF_G(6) = XF_G(7) + XF_G(2);                    /* a2 = a1 + idx*40 */

  /* GTE sequence: lwc2 gte[0] <- word(sp) is a raw gte_data write in the
   * generated emitter; lwc2 gte[1] routes through gte_write_data which
   * masks &0xFFFF so only the lh10 halfword lands. cmd 0x0180001,
   * swc2 gte[14] -> sp+8, mfc2 gte[19] -> t4 = (sra 2) -> sp+16. */
  c->gte_data[0] = br2_ld32(sp);                  /* lwc2 gte[0] raw  */
  setCopReg(2, GTE_DREG(1), br2_ld32(sp + 4));    /* lwc2 gte[1]      */
  copFunction(2, 0x0180001u);                     /* gte cmd 0x01     */
  br2_st32(sp + 8, c->gte_data[14]);              /* swc2 gte[14]     */
  gte_precision_store_word(sp + 8, 14);
  XF_G(12) = (uint32_t)getCopReg(2, GTE_DREG(19));/* mfc2 gte[19]     */
  XF_G(12) = (uint32_t)((int32_t)XF_G(12) >> 2);  /* sra t4,t4,2      */
  br2_st32(sp + 16, XF_G(12));                    /* sw t4,16(sp)     */

  XF_G(2) = br2_ld32(sp + 16);                    /* lw v0,16(sp)     */
  XF_G(5) = (uint32_t)((int32_t)XF_G(2) >> 2);    /* sra a1,v0,2      */
  XF_G(2) = XF_G(5) - 17u;                        /* addiu v0,a1,-17  */
  XF_G(2) = (XF_G(2) < 2063u) ? 1u : 0u;          /* sltiu v0,v0,2063 */
  br2_st32(sp + 16, XF_G(5));                     /* delay: sw a1,16  */
  if (XF_G(2) == 0) goto L_emit_ret;              /* beqz -> 801B03D0 */

  /* 0x801B02C4: signed divide (lh(a3+0x50)<<9) / (a1<<2). The div-by-zero
   * and break(7168)/break(6144) trap paths are no-ops that all converge on
   * 0x801B02F8; the `at` compares leave $at = 0x80000000 (delay slot). */
  XF_G(4) = XF_LD16S(XF_G(7) + 0x50);             /* lh a0,0x50(a3)    */
  XF_G(5) <<= 2;                                  /* sll a1,a1,2       */
  XF_G(4) <<= 9;                                  /* sll a0,a0,9       */
  dvd = (int32_t)XF_G(4);
  dvs = (int32_t)XF_G(5);
  if (dvs != 0) {
    c->lo = (uint32_t)(dvd / dvs);
    c->hi = (uint32_t)(dvd % dvs);
  } else {
    c->lo = (dvd >= 0) ? 0xffffffffu : 1u;
    c->hi = (uint32_t)dvd;
  }
  XF_G(1) = 0x80000000u;                          /* delay: at=0x8000..*/
  XF_G(4) = c->lo;                                /* mflo a0           */
  XF_G(2) = XF_LDH(sp + 8);                       /* lhu v0,8(sp)      */
  XF_G(3) = XF_G(4) >> 31;                        /* srl v1,a0,31      */
  XF_G(3) = XF_G(4) + XF_G(3);                    /* addu v1,a0,v1     */
  XF_G(3) = (uint32_t)((int32_t)XF_G(3) >> 1);    /* sra v1,v1,1       */
  /* four corner pairs around the swc2 result halfwords; h = (a0+sign)>>1 */
  XF_G(2) -= XF_G(3); XF_SH(XF_G(6) + 8, XF_G(2));    /* +8  = u8 -h    */
  XF_G(2) = XF_LDH(sp + 10);
  XF_G(2) -= XF_G(3); XF_SH(XF_G(6) + 10, XF_G(2));   /* +10 = u10-h    */
  XF_G(2) = XF_LDH(sp + 8);
  XF_G(2) -= XF_G(3); XF_G(2) += XF_G(4);
  XF_SH(XF_G(6) + 16, XF_G(2));                        /* +16 = u8 -h+a0 */
  XF_G(2) = XF_LDH(sp + 10);
  XF_G(2) -= XF_G(3); XF_SH(XF_G(6) + 18, XF_G(2));   /* +18 = u10-h    */
  XF_G(2) = XF_LDH(sp + 8);
  XF_G(2) -= XF_G(3); XF_SH(XF_G(6) + 24, XF_G(2));   /* +24 = u8 -h    */
  XF_G(2) = XF_LDH(sp + 10);
  XF_G(2) -= XF_G(3); XF_G(2) += XF_G(4);
  XF_SH(XF_G(6) + 26, XF_G(2));                        /* +26 = u10-h+a0 */
  XF_G(2) = XF_LDH(sp + 8);
  XF_G(2) -= XF_G(3); XF_G(2) += XF_G(4);
  XF_SH(XF_G(6) + 32, XF_G(2));                        /* +32 = u8 -h+a0 */
  XF_G(2) = XF_LDH(sp + 10);
  XF_G(2) -= XF_G(3); XF_G(2) += XF_G(4);
  XF_SH(XF_G(6) + 34, XF_G(2));                        /* +34 = u10-h+a0 */

  /* tag word + slot pointer: tbl24 = lw 0x801BEF24; a1 += tbl24;
   * a2[0] = lw(a1) | 0x09000000; slot tbl24+idx*4 = a2 & 0xFFFFFF. */
  XF_G(3) = br2_ld32(0x801bef24u);                /* lw v1,0x801BEF24 */
  XF_G(5) += XF_G(3);                             /* a1 += tbl24      */
  XF_G(2) = br2_ld32(XF_G(5));                    /* lw v0,(a1)       */
  XF_G(3) = 0x09000000u;
  XF_G(2) |= XF_G(3);                             /* v0 |= 0x09000000 */
  br2_st32(XF_G(6), XF_G(2));                     /* sw v0,0(a2)      */
  XF_G(3) = br2_ld32(sp + 16);                    /* lw v1,16(sp) idx */
  XF_G(2) = br2_ld32(0x801bef24u);                /* lw v0,0x801BEF24 */
  XF_G(3) <<= 2;                                  /* sll v1,v1,2      */
  XF_G(3) += XF_G(2);                             /* v1 += tbl24      */
  XF_G(2) = XF_G(6) << 8;                         /* sll v0,a2,8      */
  XF_G(2) = XF_G(2) >> 8;                         /* srl v0,v0,8      */
  br2_st32(XF_G(3), XF_G(2));                     /* sw v0,(v1)       */

L_emit_ret:                                       /* 0x801B03D0 */
  br2_guest_frame_free();                         /* addiu sp,+24;jr  */
#undef XF_G
#undef XF_LD16S
#undef XF_LDH
#undef XF_SH
  return;
}

/* ======== 0x801B0228 ======== */

/* scene_emit_setup — arg-shuffle trampoline into scene_emit_xform
 * (0x801B0240, tail-jump). Guest: v0 = *(*(gp+0x4C)+8) + lh(a0+2);
 * v1 = lh(v0+2); then enters the emitter with $v0 = resolved record ptr,
 * $v1 = packed input half, $a1 passthrough. Disasm 0x801B0228-0x801B023C. */
void scene_emit_setup(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t v0, v1;
  if (!c) return;
  v0 = br2_ld32((uint32_t)(c->gpr[28] + 0x4c));    /* lw v0,0x4C(gp)    */
  v1 = (uint32_t)(int32_t)(int16_t)br2_ld16(c->gpr[4] + 2); /* lh v1,2(a0) */
  v0 = br2_ld32(v0 + 8);                           /* lw v0,8(v0)      */
  v0 += v1;                                        /* addu v0,v0,v1    */
  v1 = (uint32_t)(int32_t)(int16_t)br2_ld16(v0 + 2);/* lh v1,2(v0)     */
  c->gpr[2] = v0;
  c->gpr[3] = v1;
  scene_emit_xform();                              /* j 0x801B0240     */
}

/* ======== 0x801B07D0 ======== */

/* scene_leaf_ret — shared `jr $ra` thunk for the op-stream cluster.
 * Leaves every register untouched. Disasm 0x801B07D0-0x801B07D4. */
void scene_leaf_ret(void)
{
}

/* ======== 0x801B06A4 ======== */

/* scene_op_flush — op-stream terminator: copies the 4-int8_t record
 * {b0,b1,b2,b3} at *(a1+0x54) + lhu(cursor)*4 into the t0 output struct
 * (int8_t pairs at +0x0C/+0x14/+0x1C/+0x24/+0x34/+0x3C/+0x44/+0x4C with the
 * guest's exact per-field add pattern), then stores lhu(cursor+2) into
 * a1+0x60 and returns. Disasm-verified vs generated/SLES_017.22_full_54.c
 * (all loads are LBU; the final `lb v1,3(v1)` clobbers $v1 with b3). */
void scene_op_flush(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t a1, t0;
  if (!c) return;
  a1 = c->gpr[5];
  t0 = c->gpr[8];
  c->gpr[2] = (uint32_t)br2_ld16(br2_ld32(a1 + 0x58)); /* lhu(lw cur) */
  c->gpr[3] = br2_ld32(a1 + 0x54);                     /* lw v1,0x54(a1) */
  c->gpr[2] <<= 2;                                     /* sll v0,v0,2   */
  c->gpr[3] += c->gpr[2];                              /* v1 += v0=rec  */
#define FLUSH_LB(off) br2_ld8(c->gpr[3] + (off))
  /* stores interleaved with loads exactly as the guest emits them so an
   * aliased rec/t0 still observes identical ordering. */
  c->gpr[2] = FLUSH_LB(0); br2_st8(t0 + 0x0c, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); br2_st8(t0 + 0x0d, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(0); c->gpr[4] = FLUSH_LB(2); c->gpr[2] += c->gpr[4];
  br2_st8(t0 + 0x14, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); br2_st8(t0 + 0x15, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(0); br2_st8(t0 + 0x1c, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); c->gpr[4] = FLUSH_LB(3); c->gpr[2] += c->gpr[4];
  br2_st8(t0 + 0x1d, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(0); c->gpr[4] = FLUSH_LB(2); c->gpr[2] += c->gpr[4];
  br2_st8(t0 + 0x24, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); c->gpr[4] = FLUSH_LB(3); c->gpr[2] += c->gpr[4];
  br2_st8(t0 + 0x25, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(0); br2_st8(t0 + 0x34, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); br2_st8(t0 + 0x35, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(0); c->gpr[4] = FLUSH_LB(2); c->gpr[2] += c->gpr[4];
  br2_st8(t0 + 0x3c, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); br2_st8(t0 + 0x3d, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(0); br2_st8(t0 + 0x44, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); c->gpr[4] = FLUSH_LB(3); c->gpr[2] += c->gpr[4];
  br2_st8(t0 + 0x45, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(0); c->gpr[4] = FLUSH_LB(2); c->gpr[2] += c->gpr[4];
  br2_st8(t0 + 0x4c, (uint8_t)c->gpr[2]);
  c->gpr[2] = FLUSH_LB(1); c->gpr[3] = FLUSH_LB(3); c->gpr[2] += c->gpr[3];
  br2_st8(t0 + 0x4d, (uint8_t)c->gpr[2]);
#undef FLUSH_LB
  /* tail: a1[0x60] = lhu(cursor+2); j 0x801B07D0 (jr ra) */
  c->gpr[2] = br2_ld32(a1 + 0x58);                  /* lw v0,0x58(a1)   */
  c->gpr[2] = (uint32_t)br2_ld16(c->gpr[2] + 2);    /* lhu v0,2(v0)     */
  br2_st16(a1 + 0x60, (uint16_t)c->gpr[2]);         /* sh v0,0x60(a1)   */
}

/* ======== 0x801B05D8/0684/0688 ======== */

/* scene_op_loop — the op-stream step/next interpreter shared by three CPS
 * entry points (heads 0x801B05D8 step, 0x801B0684 cursor-store frag,
 * 0x801B0688 next-dispatch frag). The fragments loop back into each other
 * (step -> next -> step) once per stream record, so they are one clean CFG
 * with an entry selector — guest CPS resume addresses are internal-only.
 * $a1 = stream ctx (+0x54 record table, +0x58 cursor, +0x5C index table,
 * +0x60 counter, +0x62 int8_t index); $t0 = output struct (set by caller);
 * $t1/$t2 = tag bounds 0xFE00/0x8000 (set by scene_op_tick). */
static void scene_op_loop(uint32_t entry)
{
  CPUState *c = clean_cpu_state();
  uint32_t a1, a2;
  if (!c) return;
  a1 = c->gpr[5];
  if (entry == 0x801b0684u) goto L_cursor_set;      /* frag 0x801B0684  */
  if (entry == 0x801b0688u) goto L_op_next;         /* frag 0x801B0688  */

L_step_head:                                         /* 0x801B05D8 */
  a2 = br2_ld32(a1 + 0x58);                          /* lw a2,0x58(a1)  */
  c->gpr[6] = a2;
  c->gpr[2] = (uint32_t)br2_ld16(a2);                /* lhu v0,0(a2)    */
  c->gpr[3] = c->gpr[2] & 0xff00u;                   /* v1 = tag & FF00 */
  c->gpr[7] = c->gpr[2] & 0x00ffu;                   /* delay: a3=&FF  */
  if (c->gpr[3] == c->gpr[9]) goto L_idx;            /* beq v1,t1->0620 */
  c->gpr[2] = ((int32_t)c->gpr[9] < (int32_t)c->gpr[3]) ? 1u : 0u; /* slt */
  {
    uint32_t take = (c->gpr[2] != 0u);
    c->gpr[2] = 0xff00u;                             /* delay: v0=FF00  */
    if (take) goto L_ff00_chk;                       /* bnez -> 0610    */
  }
  if (c->gpr[3] == c->gpr[10]) goto L_cond_adv;      /* beq v1,t2->062C */
  goto L_op_next;                                    /* j 0x801B0688    */

L_ff00_chk:                                          /* 0x801B0610 */
  if (c->gpr[3] == c->gpr[2]) goto L_ff00_body;      /* beq v1,FF00    */
  goto L_op_next;                                    /* j 0x801B0688    */

L_idx:                                               /* 0x801B0620 */
  c->gpr[2] = (uint32_t)br2_ld16(a2 + 2);             /* lhu v0,2(a2)    */
  goto L_emit_idx;                                   /* j 0x801B0650    */

L_cond_adv:                                          /* 0x801B062C */
  c->gpr[4] = (uint32_t)br2_ld16(a1 + 0x62);          /* lhu a0,0x62(a1) */
  c->gpr[2] = (c->gpr[7] < c->gpr[4]) ? 1u : 0u;      /* sltu v0,a3,a0   */
  {
    uint32_t take = (c->gpr[2] != 0u);
    c->gpr[2] = a2 + 4u;                              /* delay: v0=a2+4  */
    if (take) goto L_cursor_set;                      /* -> 0x801B0684   */
  }
  /* 0x801B0640: index++ then emit via cursor+2 halfword */
  c->gpr[3] = br2_ld32(a1 + 0x58);                    /* lw v1,0x58(a1)  */
  c->gpr[2] = c->gpr[4] + 1u;                         /* v0 = a0+1       */
  br2_st16(a1 + 0x62, (uint16_t)c->gpr[2]);           /* sh v0,0x62(a1)  */
  c->gpr[2] = (uint32_t)br2_ld16(c->gpr[3] + 2);      /* lhu v0,2(v1)    */
  /* fall through to 0x801B0650 */
L_emit_idx:                                          /* 0x801B0650 */
  c->gpr[3] = br2_ld32(a1 + 0x5c);                    /* lw v1,0x5C(a1)  */
  c->gpr[2] <<= 2;                                    /* sll v0,v0,2     */
  c->gpr[3] += c->gpr[2];                             /* v1 += v0        */
  br2_st32(a1 + 0x58, c->gpr[3]);                     /* delay: sw cur   */
  goto L_op_next;                                    /* j 0x801B0688    */

L_ff00_body:                                         /* 0x801B0664 */
  c->gpr[2] = (uint32_t)br2_ld16(a1 + 0x62);          /* lhu v0,0x62(a1) */
  c->gpr[2] = (c->gpr[2] < c->gpr[7]) ? 1u : 0u;      /* sltu v0,v0,a3   */
  {
    uint32_t take = (c->gpr[2] != 0u);
    c->gpr[2] = a2 + 4u;                              /* delay: v0=a2+4  */
    if (take) goto L_cursor_set;                      /* -> 0x801B0684   */
  }
  /* 0x801B0678: *(gp_cache+0x14) = 0; jr ra */
  c->gpr[2] = br2_ld32((uint32_t)(c->gpr[28] + 0x4c));
  br2_st32(c->gpr[2] + 0x14, 0u);                     /* delay slot      */
  return;

L_cursor_set:                                        /* 0x801B0684 */
  br2_st32(a1 + 0x58, c->gpr[2]);                     /* sw v0,0x58(a1)  */
  /* fall through */
L_op_next:                                           /* 0x801B0688 */
  c->gpr[2] = br2_ld32(a1 + 0x58);                    /* lw v0,0x58(a1)  */
  c->gpr[2] = (uint32_t)br2_ld16(c->gpr[2]);          /* lhu v0,0(v0)    */
  c->gpr[2] &= 0xff00u;                               /* andi v0,FF00    */
  if (c->gpr[2] != 0u) goto L_step_head;              /* bnez -> 05D8    */
  scene_op_flush();                                  /* -> 0x801B06A4   */
  return;
}

/* scene_op_step — entry 0x801B05D8 (op-stream step dispatch). */
void scene_op_step(void)
{
  scene_op_loop(0x801b05d8u);
}

/* scene_cursor_set — entry 0x801B0684 (store $v0 as cursor, then next). */
void scene_cursor_set(void)
{
  scene_op_loop(0x801b0684u);
}

/* scene_op_next — entry 0x801B0688 (next-record dispatch). */
void scene_op_next(void)
{
  scene_op_loop(0x801b0688u);
}

/* ======== 0x801B0584 ======== */

/* scene_op_tick — op-stream per-frame tick + dispatch head. Decrements
 * the a1+0x60 counter (lhu-- / lh reload / bgtz early-out through the
 * 0x801B07D0 jr-ra thunk), publishes 1 into gp_cache+0x14, advances the
 * a1+0x58 cursor by 4, then either flushes (next tag&0xFF00 == 0) or arms
 * t1=0xFE00/t2=0x8000 and enters the step loop. $t0 = a1 in the delay
 * slot on every path. Disasm-verified vs generated/SLES_017.22_full_54.c. */
void scene_op_tick(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t a1, cur, tag;
  if (!c) return;
  a1 = c->gpr[5];
  {
    uint32_t t = (uint32_t)br2_ld16(a1 + 0x60);       /* lhu v0,0x60(a1) */
    uint32_t base = br2_ld32((uint32_t)(c->gpr[28] + 0x4c));
    br2_st16(a1 + 0x60, (uint16_t)(t - 1u));          /* sh v0-1,0x60    */
    br2_st32(base + 0x14, 1u);                        /* gp_cache[5]=1  */
    c->gpr[2] = (uint32_t)(int32_t)(int16_t)br2_ld16(a1 + 0x60); /* lh */
    if ((int32_t)c->gpr[2] > 0) return;               /* bgtz -> 07D0   */
  }
  cur = br2_ld32(a1 + 0x58);                          /* lw v1,0x58(a1) */
  br2_st32(a1 + 0x58, cur + 4u);                      /* cursor += 4    */
  tag = (uint32_t)br2_ld16(cur + 4u) & 0xff00u;      /* lhu cur+4&FF00 */
  c->gpr[8] = a1;                                      /* delay: t0=a1   */
  if (tag == 0) {                                     /* beqz -> 06A4   */
    scene_op_flush();
    return;
  }
  c->gpr[9]  = 0xfe00u;                                /* t1 = 0xFE00    */
  c->gpr[10] = 0x8000u;                                /* t2 = 0x8000    */
  scene_op_loop(0x801b05d8u);                         /* -> 0x801B05D8  */
}

/* ======== 0x8019261c ======== */

/* gpu_data_reg_read — read the register behind pointer cell
 * @0x801b8428: guest `lui v0,0x801c; lw v0,-0x7bd8(v0); nop; lw v0,(v0);
 * jr ra`. br2_ld32p resolves the cell then routes the pointed-to
 * access through br2_ld32 (MMIO-safe — the cell can hold a 0x1F80xxxx
 * hardware address). Disasm 0x8019261c-0x80192630. */
uint32_t gpu_data_reg_read(void)
{
  return br2_ld32p(0x801b8428);
}

/* ======== 0x80192e00 ======== */

/* gpu_tag_cmd_store — issue a0 through register cell @0x801b8428 and
 * record a0's low int8_t in the 256-entry tag table @0x801fad68 indexed
 * by a0>>24. Guest returns a0>>24 in $v0 (the srl result survives
 * into the jr delay slot which performs the sb). Disasm
 * 0x80192e00-0x80192e20. */
uint32_t gpu_tag_cmd_store(uint32_t cmd)
{
  uint32_t idx = cmd >> 24;
  br2_st32p(0x801b8428, cmd);
  GP8(0x801fad68 + idx) = (uint8_t)cmd;
  return idx;
}

/* ======== 0x80192e78 ======== */

/* gpu_reg_cells_init — quad register-cell init: writes 0x04000002,
 * a0, 0 and 0x01000401 through pointer cells 0x801b8428/2c/30/34.
 * Guest leaves $v0 = the last cell pointer (callers ignore it).
 * Disasm 0x80192e78-0x80192ebc. */
void gpu_reg_cells_init(uint32_t val)
{
  br2_st32p(0x801b8428, 0x04000002u);
  br2_st32p(0x801b842c, val);
  br2_st32p(0x801b8430, 0u);
  br2_st32p(0x801b8434, 0x01000401u);
}

/* ======== 0x80192ec0 ======== */

/* gpu_reg_cmd_commit — issue param_1 | 0x10000000 through the pointer
 * cell @0x801b8428, then return *(*0x801b8424) & 0x00FFFFFF. The cell
 * contents are device/register addresses (can be 0x1F80xxxx MMIO), so
 * both dereferences go through br2_ld32/br2_st32. Disasm
 * 0x80192EC0-0x80192EF0. */
uint32_t gpu_reg_cmd_commit(uint32_t cmd)
{
  uint32_t dst = br2_ld32(0x801b8428u);
  br2_st32(dst, cmd | 0x10000000u);
  return br2_ld32(br2_ld32(0x801b8424u)) & 0x00ffffffu;
}

/* ======== 0x8018d408 ======== */

/* dir_rotate_pair — Q12 rotate pair via the sin/cos LUTs.
 * Guest CPS chain (verified against generated br2eu_func_8018D408):
 *   t0 = ra; a0 += a1; jal lut_sin_q12        -> v0 = sin(a0+a1)
 *   LO = low32(a2 * v0)  (mult; LO survives the next jal — the LUT
 *   bodies contain no mult/div)
 *   jal lut_cos_q12      -> v0 = cos(a0+a1)   [0x8018D298 adds the
 *   +0x400 phase inside; clean lut_cos_q12 already folds it in]
 *   v1 = sra(LO + 0x800, 12)
 *   v0 = sra(low32(a2 * -v0) + 0x800, 12); jr t0
 * Live-outs: $v0 = return, $v1 = first product, $a0 = a0+a1+0x400
 * (the cos helper's addiu). */
uint32_t dir_rotate_pair(uint32_t a0, uint32_t a1, uint32_t a2)
{
  CPUState *c = clean_cpu_state();
  uint32_t ang = a0 + a1;
  uint32_t lo1 = a2 * (uint32_t)lut_sin_q12(ang);
  int32_t  cs  = lut_cos_q12((int32_t)ang);
  uint32_t lo2 = a2 * (0u - (uint32_t)cs);
  uint32_t v1o = (uint32_t)((int32_t)(lo1 + 0x800u) >> 12);
  uint32_t v0o = (uint32_t)((int32_t)(lo2 + 0x800u) >> 12);
  if (c) {
    c->gpr[3] = v1o;                    /* $v1 live-out */
    c->gpr[4] = ang + 0x400u;           /* $a0 post-call guest value */
  }
  return v0o;
}

/* ======== 0x8018d69c ======== */

/* prim_attr_repack — repack four u32 lanes from src into dst swapping
 * the hi/lo u16 halves cyclically (t0..t3 = src+0/4/8/c):
 *   dst+0  = (t1 hi)|(t0 lo)   dst+4  = (t0 hi)|(t3 lo)
 *   dst+8  = (t3 hi)|(t2 lo)   dst+c  = (t2 hi)|(t1 lo)
 * then copies the s16 at src+0x10 -> dst+0x10. The guest reads that
 * halfword AFTER the first two dst stores (kept for aliasing fidelity)
 * and returns dst in $v0. Disasm 0x8018d69c-0x8018d6fc. */
uint32_t prim_attr_repack(uint32_t src_a, uint32_t dst_a)
{
  uint8_t *src = (uint8_t *)guest_at(src_a);
  uint8_t *dst = (uint8_t *)guest_at(dst_a);
  uint32_t t0 = *(uint32_t *)(src + 0x00);
  uint32_t t1 = *(uint32_t *)(src + 0x04);
  uint32_t t2 = *(uint32_t *)(src + 0x08);
  uint32_t t3 = *(uint32_t *)(src + 0x0c);
  int16_t  h;
  *(uint32_t *)(dst + 0x00) = (t1 & 0xffff0000u) | (t0 & 0xffffu);
  *(uint32_t *)(dst + 0x04) = (t0 & 0xffff0000u) | (t3 & 0xffffu);
  h = *(int16_t *)(src + 0x10);
  *(uint32_t *)(dst + 0x08) = (t3 & 0xffff0000u) | (t2 & 0xffffu);
  *(uint32_t *)(dst + 0x0c) = (t2 & 0xffff0000u) | (t1 & 0xffffu);
  *(int16_t *)(dst + 0x10) = h;
  return dst_a;
}

/* ======== op_* leaf helpers (0x801ad3xx-0x801ad9xx) ======== */

extern uint8_t  br2_ld8 (uint32_t a);
extern uint16_t br2_ld16(uint32_t a);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st16(uint32_t a, uint16_t v);
extern void     br2_st32(uint32_t a, uint32_t v);
extern uint32_t guest_gp(void);

/* op_tbl_elem_copy — copy one u32 between two indexed tables.
 * Guest: i = s16(a0+2)<<2, j = s16(a0+6)<<2;
 *        v = *(u32*)(a2+j); *(u32*)(a1+i) = v; return v.
 * Params arrive as guest addresses; the br2_ldN/br2_stN pair keeps the
 * MMIO-capable access path. Disasm 0x801add38-0x801add58. */
uint32_t op_tbl_elem_copy(uint32_t a0, uint32_t a1, uint32_t a2)
{
  uint32_t i = (uint32_t)(((int32_t)(int16_t)br2_ld16(a0 + 2)) << 2);
  uint32_t j = (uint32_t)(((int32_t)(int16_t)br2_ld16(a0 + 6)) << 2);
  uint32_t v = br2_ld32(a2 + j);
  br2_st32(a1 + i, v);
  return v;
}

/* op_hword_pair_add — add two u16 table entries and store back.
 * Guest: da = a1 + s16(a0+2)*2, db = a2 + s16(a0+6)*2;
 *        s = lhu(da) + lhu(db); sh s,(da); return s.
 * Disasm 0x801add84-0x801addb0. */
uint32_t op_hword_pair_add(uint32_t a0, uint32_t a1, uint32_t a2)
{
  uint32_t da = a1 + ((uint32_t)(((int32_t)(int16_t)br2_ld16(a0 + 2)) << 1));
  uint32_t db = a2 + ((uint32_t)(((int32_t)(int16_t)br2_ld16(a0 + 6)) << 1));
  uint32_t s = (uint32_t)br2_ld16(da) + (uint32_t)br2_ld16(db);
  br2_st16(da, (uint16_t)s);
  return s;
}

/* op_gp_dword_sub — v = *a1 - *(u32*)(a0+2); store v at
 * *(*(u32*)(gp+0x4c)) + 0x14; return v. Disasm 0x801ad9d4-0x801ad9e8. */
uint32_t op_gp_dword_sub(uint32_t a0, uint32_t a1)
{
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  uint32_t v = br2_ld32(a1) - br2_ld32(a0 + 2);
  br2_st32(base + 0x14, v);
  return v;
}

/* op_gp_cond_store — gp-relative conditional store.
 * base = *(*(u32*)(gp+0x4c)); when *(base+0x14) != 0 the guest tail-
 * dispatches to 0x801ad698 (`*a1 += 4` — a separate function entry,
 * routed through call_guest to preserve the dispatch contract);
 * otherwise *a1 = s16(*a0) + *(u32*)(gp+0x50). Returns the stored
 * value. Disasm 0x801ad668-0x801ad694 + tail 0x801ad698-0x801ad6a8. */
uint32_t op_gp_cond_store(uint32_t a0, uint32_t a1)
{
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  uint32_t v;
  if (br2_ld32(base + 0x14) != 0) {
    extern uint32_t call_guest(uint32_t,uint32_t,uint32_t);
    return call_guest(0x801ad698u, a0, a1);
  }
  v = (uint32_t)(int32_t)(int16_t)br2_ld16(a0) +
      br2_ld32((uint32_t)(guest_gp() + 0x50));
  br2_st32(a1, v);
  return v;
}

/* ======== 0x801ad698 ======== */

/* op_dword_add4 — *a1 += 4; returns the stored value. Leaf tail shared by
 * the gp-relative conditional-store family. Disasm
 * 0x801ad698-0x801ad6a8 (lw/addiu/sw through gpr5). */
uint32_t op_dword_add4(uint32_t a0, uint32_t a1)
{
  uint32_t v;
  (void)a0;
  v = br2_ld32(a1) + 4;
  br2_st32(a1, v);
  return v;
}

/* ======== 0x801ad6ac ======== */

/* op_gp_cond_add — gp-relative conditional add-store, inverse polarity of
 * op_gp_cond_store. If *(*(gp+0x4c)+0x14) == 0 the tail path performs
 * *a1 += 4 (0x801ad6dc); otherwise *a1 = s16(*(u16*)a0) + *(gp+0x50).
 * Returns the stored value. Disasm 0x801ad6ac-0x801ad6d8 + tail
 * 0x801ad6dc-0x801ad6ec. */
uint32_t op_gp_cond_add(uint32_t a0, uint32_t a1)
{
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  uint32_t v;
  if (br2_ld32(base + 0x14) == 0) {
    v = br2_ld32(a1) + 4;
    br2_st32(a1, v);
    return v;
  }
  v = (uint32_t)(int32_t)(int16_t)br2_ld16(a0) +
      br2_ld32((uint32_t)(guest_gp() + 0x50));
  br2_st32(a1, v);
  return v;
}

/* ======== 0x801ad6dc ======== */

/* op_dword_add4_entry — standalone entry for the *a1 += 4 tail reached by
 * the conditional-store family. Identical body to op_dword_add4; kept as a
 * separate clean entry because the guest exposes it as its own jal target.
 * Disasm 0x801ad6dc-0x801ad6ec. */
uint32_t op_dword_add4_entry(uint32_t a0, uint32_t a1)
{
  return op_dword_add4(a0, a1);
}

/* ======== 0x801ad920 ======== */

/* op_dword_add_field2 — *a1 += *(u32*)(a0+2); returns the stored value.
 * Disasm 0x801ad920-0x801ad934 (lw v0,(a1); lw v1,2(a0); add; sw; jr). */
uint32_t op_dword_add_field2(uint32_t a0, uint32_t a1)
{
  uint32_t v = br2_ld32(a1) + br2_ld32(a0 + 2);
  br2_st32(a1, v);
  return v;
}

/* ======== 0x801ad994 ======== */

/* op_gp_deref_store14 — *(*(gp+0x4c)+0x14) = *a1; returns *a1.
 * Disasm 0x801ad994-0x801ad9a0 (lw v1,0x4c(gp); lw v0,(a1); sw v0,0x14(v1);
 * jr ra with v0 = loaded value). */
uint32_t op_gp_deref_store14(uint32_t a0, uint32_t a1)
{
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  uint32_t v = br2_ld32(a1);
  (void)a0;
  br2_st32(base + 0x14, v);
  return v;
}

/* ======== 0x801ae490 ======== */

/* op_gp_indexed_byte_store — idx = s16(*(u16*)(a0+2)); v = *(u8*)(a1+idx);
 * *(*(gp+0x4c)+0x14) = v; returns v. Disasm 0x801ae490-0x801ae4a4
 * (lh a0+2 -> add to a1 -> lbu -> sw into gp-deref +0x14). */
uint32_t op_gp_indexed_byte_store(uint32_t a0, uint32_t a1)
{
  int32_t idx = (int32_t)(int16_t)br2_ld16(a0 + 2);
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  uint32_t v = br2_ld8(a1 + (uint32_t)idx);
  br2_st32(base + 0x14, v);
  return v;
}

/* ======== 0x801AD984 ======== */

/* op_s16_store14 — *(gp_cache+0x14) = sext16(lh(a1)); returns the
 * sign-extended halfword (guest $v0). Disasm 0x801AD984-0x801AD990 —
 * the gp-cell store sits in the jr delay slot and always executes. */
uint32_t op_s16_store14(uint32_t a0, uint32_t a1)
{
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  uint32_t v = (uint32_t)(int32_t)(int16_t)br2_ld16(a1);
  (void)a0;
  br2_st32(base + 0x14, v);
  return v;
}

/* ======== 0x801AD964 ======== */

/* op_hword_clear — *(u16*)a1 = 0; $v0 passes through unchanged (the guest
 * never writes v0). Disasm 0x801AD964-0x801AD96C: jr $ra with the
 * delay-slot `sh $0,0(a1)` always executing. */
uint32_t op_hword_clear(uint32_t a0, uint32_t a1)
{
  CPUState *c = clean_cpu_state();
  (void)a0;
  br2_st16(a1, 0);
  return c ? c->gpr[2] : 0;
}

/* ======== 0x801AD96C ======== */

/* op_dword_clear — *(u32*)a1 = 0; $v0 passes through unchanged. Disasm
 * 0x801AD96C-0x801AD974: jr $ra with the delay-slot `sw $0,0(a1)`. */
uint32_t op_dword_clear(uint32_t a0, uint32_t a1)
{
  CPUState *c = clean_cpu_state();
  (void)a0;
  br2_st32(a1, 0);
  return c ? c->gpr[2] : 0;
}

/* ======== 0x801AE68C ======== */

/* op_gp_deref8_s16_add — *a1 = *(*(gp+0x4c)+8) + sext16(*(u16*)(a0+2));
 * returns the stored sum in $v0. Disasm 0x801AE68C-0x801AE6A8:
 * lw v0,0x4c(gp); lh v1,2(a0); lw v0,8(v0); addu v0,v0,v1; delay-slot
 * sw v0,0(a1); jr ra. */
uint32_t op_gp_deref8_s16_add(uint32_t a0, uint32_t a1)
{
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  int32_t idx  = (int32_t)(int16_t)br2_ld16(a0 + 2);
  uint32_t v   = br2_ld32(base + 8) + (uint32_t)idx;
  br2_st32(a1, v);
  return v;
}

/* ======== 0x801AE65C ======== */

/* op_gp_deref18_add_s16 — s = sext16(*(u16*)a0); *(*(gp+0x4c)+0x18) =
 * *(gp+0x50) + s; returns s in $v0. Disasm 0x801AE65C-0x801AE674:
 * lh v0,0(a0); lw v1,0x50(gp); lw a0,0x4c(gp); addu v1,v1,v0;
 * delay-slot sw v1,0x18(a0); jr ra. */
uint32_t op_gp_deref18_add_s16(uint32_t a0, uint32_t a1)
{
  uint32_t s    = (uint32_t)(int32_t)(int16_t)br2_ld16(a0);
  uint32_t acc  = br2_ld32((uint32_t)(guest_gp() + 0x50));
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  (void)a1;
  br2_st32(base + 0x18, acc + s);
  return s;
}

/* ======== 0x801ADA24 ======== */

/* op_dword_mul_store — *a1 = lo32(*(u32*)a1 * *(u32*)(a0+2)); the product
 * also lands in $a2 and $lo/$hi. Returns *(a0+2) (guest $v0 keeps the
 * multiplier loaded before the mult). Disasm 0x801ADA24-0x801ADA38. */
uint32_t op_dword_mul_store(uint32_t a0, uint32_t a1)
{
  CPUState *c = clean_cpu_state();
  uint32_t x = br2_ld32(a1);              /* v1 = *a1            */
  uint32_t y = br2_ld32(a0 + 2);          /* v0 = *(a0+2)        */
  int64_t p = (int64_t)(int32_t)x * (int64_t)(int32_t)y; /* mult */
  uint32_t lo = (uint32_t)p;
  if (c) {
    c->lo = lo;
    c->hi = (uint32_t)(p >> 32);
    c->gpr[6] = lo;                       /* mflo -> a2          */
    c->gpr[2] = y;                        /* v0 unchanged        */
  }
  br2_st32(a1, lo);                       /* delay: sw a2,(a1)   */
  return y;
}

/* ======== 0x801ADAA4 ======== */

/* op_hmask_and_store — *a1 = lhu(a1) & lhu(a0+2); gp_cache[5] =
 * sext16(result); returns sext16(result). Disasm 0x801ADAA4-0x801ADAC4 —
 * the sll16+sra16 sign-extend and the gp-cell store are the jr delay. */
uint32_t op_hmask_and_store(uint32_t a0, uint32_t a1)
{
  uint32_t m = (uint32_t)br2_ld16(a1);        /* lhu v0,(a1)     */
  uint32_t k = (uint32_t)br2_ld16(a0 + 2);    /* lhu v1,2(a0)    */
  uint32_t r = m & k;
  uint32_t sx = (uint32_t)(int32_t)(int16_t)r;/* sll16+sra16     */
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  br2_st16(a1, (uint16_t)r);                  /* sh v0,(a1)      */
  br2_st32(base + 0x14, sx);                  /* delay: sw gp+14 */
  return sx;
}

/* ======== 0x801ADAC8 ======== */

/* op_dword_and_store — *a1 &= *(u32*)(a0+2); gp_cache[5] = result;
 * returns result. Disasm 0x801ADAC8-0x801ADAE0 — gp-cell store is the
 * jr delay slot. */
uint32_t op_dword_and_store(uint32_t a0, uint32_t a1)
{
  uint32_t x = br2_ld32(a1);
  uint32_t y = br2_ld32(a0 + 2);
  uint32_t r = x & y;
  uint32_t base = br2_ld32((uint32_t)(guest_gp() + 0x4c));
  br2_st32(a1, r);
  br2_st32(base + 0x14, r);                   /* delay slot      */
  return r;
}

/* ======== 0x801ADC48 ======== */

/* op_dword_shl_field — *a1 = *a1 << (sext16(lh(a0+2)) & 0x1F);
 * returns the shifted value. Disasm 0x801ADC48-0x801ADC5C — the store
 * is the jr delay slot. */
uint32_t op_dword_shl_field(uint32_t a0, uint32_t a1)
{
  uint32_t sh = (uint32_t)(int32_t)(int16_t)br2_ld16(a0 + 2);
  uint32_t x = br2_ld32(a1);
  uint32_t r = x << (sh & 0x1fu);
  br2_st32(a1, r);                            /* delay slot      */
  return r;
}

/* ======== 0x801ADDB4 ======== */

/* op_dword_add_idx2 — p1 = a1 + sext16(lh(a0+2))*4;
 * p2 = a2 + sext16(lh(a0+6))*4; *p1 += *p2; returns the sum.
 * Disasm 0x801ADDB4-0x801ADDE0 — the *p1 store is the jr delay slot;
 * $a3 keeps p1 and $v1 keeps *p2 for callers that inspect them. */
uint32_t op_dword_add_idx2(uint32_t a0, uint32_t a1, uint32_t a2)
{
  CPUState *c = clean_cpu_state();
  uint32_t i1 = (uint32_t)(((int32_t)(int16_t)br2_ld16(a0 + 2)) << 2);
  uint32_t i2 = (uint32_t)(((int32_t)(int16_t)br2_ld16(a0 + 6)) << 2);
  uint32_t p1 = a1 + i1;
  uint32_t p2 = a2 + i2;
  uint32_t va = br2_ld32(p1);
  uint32_t vb = br2_ld32(p2);
  uint32_t s = va + vb;
  br2_st32(p1, s);                            /* delay slot      */
  if (c) {
    c->gpr[7] = p1;                           /* a3 = p1          */
    c->gpr[3] = vb;                           /* v1 = *p2         */
    c->gpr[2] = s;                            /* v0 = sum         */
  }
  return s;
}

/* ======== 0x801AEA94 ======== */

/* op_ctx_chain3 — ret = call_guest(0x801AD284 = noop_c, *a1, a1); then a
 * three-level pointer chase anchored at e0 = *(*(gp_cache+8) + lh(a0+2)):
 *   ret[1] = *(e0 + lh(a0+4)*4)
 *   ret[2] = *(e0 + ret[1]*4 + 4)
 *   ret[3] = *(e0 + ret[2]*4 + 8)
 * The guest saves/restores s0+ra around the jal and reloads lh(a0+4)
 * before each chase level (dead loads that still write $a1). Disasm
 * 0x801AEA94-0x801AEB20. */
uint32_t op_ctx_chain3(uint32_t a0, uint32_t a1)
{
  CPUState *c = clean_cpu_state();
  uint32_t s0, ctx, e0, i2, e1, e2, e3, ret;
  extern uint32_t call_guest(uint32_t, uint32_t, uint32_t);
  extern int br2_bail_pending(void);
  if (!c) return 0;
  s0 = a0;
  ret = call_guest(0x801ad284u, br2_ld32(a1), a1);
  c->gpr[2] = ret;                             /* jal result -> v0  */
  if (br2_bail_pending()) return ret;
  ctx = br2_ld32((uint32_t)(guest_gp() + 0x4c));/* lw v1,gp+0x4C    */
  i2  = (uint32_t)(int32_t)(int16_t)br2_ld16(s0 + 2);
  e0  = br2_ld32(br2_ld32(ctx + 8) + i2);      /* *(ctx8 + lh(s0+2))*/
  i2  = (uint32_t)(int32_t)(int16_t)br2_ld16(s0 + 4);
  e1  = br2_ld32(e0 + (i2 << 2));
  br2_st32(ret + 4, e1);                       /* ret[1] = e1       */
  i2  = (uint32_t)(int32_t)(int16_t)br2_ld16(s0 + 4); /* guest reload */
  e2  = br2_ld32(e0 + (e1 << 2) + 4);
  br2_st32(ret + 8, e2);                       /* ret[2] = e2       */
  i2  = (uint32_t)(int32_t)(int16_t)br2_ld16(s0 + 4); /* guest reload */
  e3  = br2_ld32(e0 + (e2 << 2) + 8);
  br2_st32(ret + 12, e3);                      /* ret[3] = e3       */
  c->gpr[5] = i2;                              /* last lh -> a1     */
  return ret;
}

/* ======== 0x801AFD38 ======== */

/* op_hpair_store_16_3e — a1[0x16] = a1[0x3E] = lhu(a0+2); the guest
 * reloads the source halfword for the delay-slot store. Returns the
 * halfword. Disasm 0x801AFD38-0x801AFD4C. */
uint32_t op_hpair_store_16_3e(uint32_t a0, uint32_t a1)
{
  uint32_t h = (uint32_t)br2_ld16(a0 + 2);     /* lhu v0,2(a0)      */
  br2_st16(a1 + 0x16, (uint16_t)h);
  h = (uint32_t)br2_ld16(a0 + 2);              /* reload for delay  */
  br2_st16(a1 + 0x3e, (uint16_t)h);            /* delay: sh ->+0x3E */
  return h;
}

/* ======== 0x801AFD50 ======== */

/* op_hpair_store_0e_36 — a1[0x0E] = a1[0x36] = lhu(a0+2); same shape as
 * op_hpair_store_16_3e with the sibling offset pair. Disasm
 * 0x801AFD50-0x801AFD64. */
uint32_t op_hpair_store_0e_36(uint32_t a0, uint32_t a1)
{
  uint32_t h = (uint32_t)br2_ld16(a0 + 2);     /* lhu v0,2(a0)      */
  br2_st16(a1 + 0x0e, (uint16_t)h);
  h = (uint32_t)br2_ld16(a0 + 2);              /* reload for delay  */
  br2_st16(a1 + 0x36, (uint16_t)h);            /* delay: sh ->+0x36 */
  return h;
}

/* ======== 0x801AFF20 ======== */

/* op_pair_rec_init — initialize two 40-int8_t records at a1 (and a1+40)
 * with the fixed tag bytes {+3=9, +7=46, +4..+6=128}, then copy
 * lhu(a0+2)->a1+0x50 and lhu(a0+4)->a1+0x52. The 2-iteration loop and
 * the jr-delay halfword load are reproduced exactly; $a2 ends at 2,
 * $a3=128, $t0=46, $t1=9, $v1=a1+80. Disasm 0x801AFF20-0x801AFF6C. */
uint32_t op_pair_rec_init(uint32_t a0, uint32_t a1)
{
  CPUState *c = clean_cpu_state();
  uint32_t v1, i;
  if (!c) return 0;
  c->gpr[6] = 0;                               /* a2 counter         */
  c->gpr[9] = 9;                               /* t1 = 9             */
  c->gpr[8] = 46;                              /* t0 = 46            */
  c->gpr[7] = 128;                             /* a3 = 128           */
  v1 = a1;
  for (i = 0; i < 2; i++) {
    br2_st8(v1 + 3, 9);
    br2_st8(v1 + 7, 46);
    br2_st8(v1 + 4, 128);
    br2_st8(v1 + 5, 128);
    br2_st8(v1 + 6, 128);
    v1 += 40;                                  /* delay: v1 += 0x28  */
  }
  c->gpr[6] = 2;                               /* a2 = 2             */
  c->gpr[3] = v1;                              /* v1 = a1+80         */
  c->gpr[2] = (uint32_t)br2_ld16(a0 + 2);      /* lhu v0,2(a0)       */
  br2_st16(a1 + 0x50, (uint16_t)c->gpr[2]);    /* sh -> +0x50        */
  c->gpr[2] = (uint32_t)br2_ld16(a0 + 4);      /* delay: lhu v0,4(a0)*/
  br2_st16(a1 + 0x52, (uint16_t)c->gpr[2]);    /* delay: sh -> +0x52 */
  return c->gpr[2];
}

/* ======== 0x80187dcc ======== */

/* gte_persp_batch — GTE batch transform over 16-int8_t records.
 * Guest loop (a3 = count, a0 = cursor, returns end cursor):
 *   per record: mtc2 gte[0]=rec[0], gte[1]=rec[1]; GTE cmd 0x4A6412;
 *   swc2 gte[9] -> rec+8, mfc2 gte[10] -> h, swc2 gte[11] -> rec+12,
 *   sh h -> rec+10; cursor += 0x10.
 * The guest software-pipelines the next record's mtc2 loads after the
 * GTE command — semantically identical to loading at loop top (the
 * stores never overlap the next record's +0/+4 source words).
 * Disasm 0x80187dcc-0x80187e14. */
uint32_t gte_persp_batch(uint32_t cur, uint32_t a1, uint32_t a2,
                         int32_t count)
{
  (void)a1; (void)a2;
  while (count > 0) {
    uint32_t t0 = DB32(cur);
    uint32_t t1 = DB32(cur + 4);
    uint32_t h;
    setCopReg(2, GTE_DREG(0), t0);
    setCopReg(2, GTE_DREG(1), t1);
    count--;
    copFunction(2, 0x4A6412u);
    DB32(cur + 8)  = (uint32_t)getCopReg(2, GTE_DREG(9));
    h = (uint32_t)getCopReg(2, GTE_DREG(10));
    DB32(cur + 12) = (uint32_t)getCopReg(2, GTE_DREG(11));
    GP16(cur + 10) = (uint16_t)h;
    cur += 0x10;
  }
  return cur;
}

/* ======== 0x8018d5fc ======== */

/* gte_lwc2_r0_r1 — paired GTE vector load: lwc2 gte[0] <- (u32*)a0,
 * lwc2 gte[1] <- (u32*)(a0+4). Leaf (jr ra, v0 untouched -> void).
 * Disasm 0x8018d5fc-0x8018d610 (lwc2 x2, jr ra). */
void gte_lwc2_r0_r1(uint32_t a0)
{
  gte_lwc2(a0 + 0, 0);
  gte_lwc2(a0 + 4, 1);
}

/* ======== 0x801A54F8 / 0x801A5698 / 0x801A5878 / 0x801A5A0C ======== */

/* RTPT/NCLIP primitive-record emitters — the four-entry GTE family that
 * prim-list walkers invoke per record batch. Shared prologue spills
 * ra/s0-s7 to scratchpad 0x1f8003dc-0x1f8003fc (same cells the generated
 * bodies use; epilogue reloads only feed callee-saved regs the clean body
 * never touches). a1 selects the output-record slot (a1 <<= 2 indexes a
 * 4-int8_t table at a2), a2 is the record descriptor:
 *   +0x10 vertex table (8-int8_t xy/z pairs), +0x18 index stream,
 *   +0x24 flags words, +0x30 signed record count (lh).
 * Per record: lwc2/mtc2 vertex loads -> RTPT -> NCLIP -> SXY stores ->
 * MAC0/flags gate -> ordering-table insert. Pure guest-RAM + COP2 state:
 * no calls, no blocking waits, no live guest sp -> portable per §4.
 * Verified insn-by-insn against disc/SLES_017.22 and
 * generated/SLES_017.22_full_50.c.
 *
 *   0x801A54F8 tri indexed OT, tag 0x07000000, rec 0x20
 *   0x801A5698 quad indexed OT, tag 0x09000000, rec 0x28
 *   0x801A5878 tri fixed OT head, tag 0x07000000, rec 0x20
 *   0x801A5A0C quad fixed OT head, tag 0x09000000, rec 0x28
 * Guest v0 at jr ra = 0x00ffffff (the mask constant) — returned. */
static void prim_emit_ctx_save(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t *b;
  if (c == NULL) return;
  b = (uint32_t *)guest_at(0x1f8003dc);
  b[0] = c->gpr[31];              /* ra  */
  b[1] = c->gpr[23];              /* s7  */
  b[2] = c->gpr[22];              /* s6  */
  b[3] = c->gpr[21];              /* s5  */
  b[4] = c->gpr[20];              /* s4  */
  b[5] = c->gpr[19];              /* s3  */
  b[6] = c->gpr[18];              /* s2  */
  b[7] = c->gpr[17];              /* s1  */
  b[8] = c->gpr[16];              /* s0  */
}

/* Shared descriptor preamble for all four emitters. */
static void prim_emit_desc(uint32_t a1,uint32_t a2,
                           uint32_t *s0,uint32_t *s4,uint32_t *s1,
                           uint32_t *s2,uint32_t *s3,int32_t *s5)
{
  a1 <<= 2;
  *s0 = DB32(a1 + a2);
  *s4 = DB32(a1 + a2 + 8);        /* dead in these bodies (no 0x100 chain) */
  *s1 = DB32(a2 + 0x10);
  *s2 = DB32(a2 + 0x18);
  *s3 = DB32(a2 + 0x24);
  *s5 = (int32_t)*(int16_t *)guest_at(a2 + 0x30);
}

/* 3-index record stream: packed u16 idx0/idx1 + lh idx2, stride 6.
 * Pre-loop reads idx0/idx1 as u16 (zero-extended) and idx2 as lh; the
 * in-loop stream re-reads all three as lh (sign-extended). idx0's vertex
 * pair is kept in GPRs (va0/va1) and mtc2'd into V0 on the next pass. */
uint32_t prim_emit_tri_ot_idx(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t s0,s1,s2,s3,s4,va0 = 0,va1 = 0;
  int32_t  s5;
  uint32_t w,i0,i1,i2,p,slot,old,sz3;
  int32_t  t1s,mac0,flags;
  int first = 1;
  (void)a3;

  prim_emit_ctx_save();
  prim_emit_desc(a1,a2,&s0,&s4,&s1,&s2,&s3,&s5);
  (void)s4;

  w  = DB32(s2);
  i0 = w & 0xffffu;
  i1 = w >> 16;
  i2 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2 + 4);
  p  = s1 + (i0 << 3); gte_lwc2(p,0);     gte_lwc2(p + 4,1);
  p  = s1 + (i1 << 3); gte_lwc2(p,2);     gte_lwc2(p + 4,3);
  p  = s1 + (i2 << 3); gte_lwc2(p,4);     gte_lwc2(p + 4,5);

  s5 -= 1;
  if (s5 > 0) s2 += 6; else s2 -= 6;

  for (;;) {
    if (!first) {
      setCopReg(2, GTE_DREG(0), va0);
      setCopReg(2, GTE_DREG(1), va1);
    }
    first = 0;
    s5 -= 1;
    copFunction(2, 0x0280030);                  /* RTPT */

    i0 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2);
    i1 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2 + 2);
    i2 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2 + 4);
    p  = s1 + (i0 << 3);
    va0 = DB32(p); va1 = DB32(p + 4);
    p  = s1 + (i1 << 3); gte_lwc2(p,2); gte_lwc2(p + 4,3);
    sz3 = (uint32_t)getCopReg(2, GTE_DREG(19));       /* SZ3 (pre-NCLIP) */
    copFunction(2, 0x0400006);                        /* NCLIP */
    t1s = (int32_t)sz3 >> 2;
    p  = s1 + (i2 << 3); gte_lwc2(p,4); gte_lwc2(p + 4,5);
    gte_swc2(s0 + 8, 12);
    gte_swc2(s0 + 0x10, 13);
    mac0   = (int32_t)getCopReg(2, GTE_DREG(24));
    flags  = (int32_t)*(int16_t *)guest_at(s3);
    if (mac0 >= 0 || (flags & 0x200) != 0) {
      slot = a0 + ((uint32_t)t1s << 2);
      if (t1s >= 10) {
        old = DB32(slot);               /* delay-slot load: t1s>=0x800 too */
        if (t1s < 0x800) {
          gte_swc2(s0 + 0x18, 14);
          DB32(s0)   = 0x07000000u | old;
          DB32(slot) = s0 & 0x00ffffffu;
        }
      }
    }
    s0 += 0x20; s2 += 6; s3 += 2;
    if (s5 > 0) continue;
    s2 -= 6;
    if (s5 >= 0) continue;
    break;
  }
  return 0x00ffffffu;
}

/* 0x801A5878 — same tri stream, fixed OT head at *a0 (no slot bounds). */
uint32_t prim_emit_tri_ot_fix(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t s0,s1,s2,s3,s4,va0 = 0,va1 = 0,old;
  int32_t  s5;
  uint32_t w,i0,i1,i2,p;
  int32_t  mac0,flags;
  int first = 1;
  (void)a3;

  prim_emit_ctx_save();
  prim_emit_desc(a1,a2,&s0,&s4,&s1,&s2,&s3,&s5);
  (void)s4;

  w  = DB32(s2);
  i0 = w & 0xffffu;
  i1 = w >> 16;
  i2 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2 + 4);
  p  = s1 + (i0 << 3); gte_lwc2(p,0);     gte_lwc2(p + 4,1);
  p  = s1 + (i1 << 3); gte_lwc2(p,2);     gte_lwc2(p + 4,3);
  p  = s1 + (i2 << 3); gte_lwc2(p,4);     gte_lwc2(p + 4,5);

  s5 -= 1;
  if (s5 > 0) s2 += 6; else s2 -= 6;

  for (;;) {
    if (!first) {
      setCopReg(2, GTE_DREG(0), va0);
      setCopReg(2, GTE_DREG(1), va1);
    }
    first = 0;
    s5 -= 1;
    copFunction(2, 0x0280030);                  /* RTPT */

    i0 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2);
    i1 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2 + 2);
    i2 = (uint32_t)(int32_t)*(int16_t *)guest_at(s2 + 4);
    p  = s1 + (i0 << 3);
    va0 = DB32(p); va1 = DB32(p + 4);
    p  = s1 + (i1 << 3); gte_lwc2(p,2); gte_lwc2(p + 4,3);
    (void)getCopReg(2, GTE_DREG(19));           /* SZ3 — dead here */
    copFunction(2, 0x0400006);                  /* NCLIP */
    p  = s1 + (i2 << 3); gte_lwc2(p,4); gte_lwc2(p + 4,5);
    gte_swc2(s0 + 8, 12);
    gte_swc2(s0 + 0x10, 13);
    mac0  = (int32_t)getCopReg(2, GTE_DREG(24));
    flags = (int32_t)*(int16_t *)guest_at(s3);
    old   = DB32(a0);                    /* guest loads *a0 on pass AND fail */
    if (mac0 >= 0 || (flags & 0x200) != 0) {
      gte_swc2(s0 + 0x18, 14);
      DB32(s0) = 0x07000000u | old;
      DB32(a0) = s0 & 0x00ffffffu;
    }
    s0 += 0x20; s2 += 6; s3 += 2;
    if (s5 > 0) continue;
    s2 -= 6;
    if (s5 >= 0) continue;
    break;
  }
  return 0x00ffffffu;
}

/* 4-index record stream: two packed u32 words (4 x u16, zero-extended),
 * stride 8. idx3's vertex ptr (t4) is rotated to t5 each pass and only
 * projected by the gated RTPS — its SXY2 lands at s0+0x20. */
uint32_t prim_emit_quad_ot_idx(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t s0,s1,s2,s3,s4,t4 = 0,t5,va0 = 0,va1 = 0;
  int32_t  s5;
  uint32_t w0,w1,i0,i1,i2,p,slot,old,sz3;
  int32_t  t1s,mac0,flags;
  int first = 1;
  (void)a3;

  prim_emit_ctx_save();
  prim_emit_desc(a1,a2,&s0,&s4,&s1,&s2,&s3,&s5);
  (void)s4;

  w0 = DB32(s2); w1 = DB32(s2 + 4);
  i0 = w0 & 0xffffu; i1 = w0 >> 16;
  i2 = w1 & 0xffffu;
  p  = s1 + (i0 << 3); gte_lwc2(p,0);     gte_lwc2(p + 4,1);
  p  = s1 + (i1 << 3); gte_lwc2(p,2);     gte_lwc2(p + 4,3);
  p  = s1 + (i2 << 3); gte_lwc2(p,4);     gte_lwc2(p + 4,5);
  t4 = s1 + ((w1 >> 16) << 3);

  s5 -= 1;
  if (s5 > 0) s2 += 8; else s2 -= 8;

  for (;;) {
    if (!first) {
      setCopReg(2, GTE_DREG(0), va0);
      setCopReg(2, GTE_DREG(1), va1);
    }
    first = 0;
    s5 -= 1;
    copFunction(2, 0x0280030);                  /* RTPT */

    w0 = DB32(s2); w1 = DB32(s2 + 4);
    i0 = w0 & 0xffffu;
    p  = s1 + (i0 << 3);
    va0 = DB32(p); va1 = DB32(p + 4);
    i1 = w0 >> 16;
    p  = s1 + (i1 << 3); gte_lwc2(p,2); gte_lwc2(p + 4,3);
    sz3 = (uint32_t)getCopReg(2, GTE_DREG(19));       /* SZ3 (pre-NCLIP) */
    copFunction(2, 0x0400006);                        /* NCLIP */
    t1s = (int32_t)sz3 >> 2;
    i2 = w1 & 0xffffu;
    p  = s1 + (i2 << 3); gte_lwc2(p,4); gte_lwc2(p + 4,5);
    t5 = s1 + ((w1 >> 16) << 3);
    gte_swc2(s0 + 8, 12);
    gte_swc2(s0 + 0x10, 13);
    gte_swc2(s0 + 0x18, 14);
    mac0  = (int32_t)getCopReg(2, GTE_DREG(24));
    flags = (int32_t)*(int16_t *)guest_at(s3);
    if (mac0 >= 0 || (flags & 0x200) != 0) {
      slot = a0 + ((uint32_t)t1s << 2);
      gte_lwc2(t4,0); gte_lwc2(t4 + 4,1);
      copFunction(2, 0x0180001);                /* RTPS — 4th vertex */
      old = DB32(slot);                 /* unconditional once gate passed */
      if (t1s >= 10 && t1s < 0x800) {
        DB32(s0)   = 0x09000000u | old;
        DB32(slot) = s0 & 0x00ffffffu;
        gte_swc2(s0 + 0x20, 14);
      }
    }
    t4 = t5;
    s0 += 0x28; s2 += 8; s3 += 2;
    if (s5 > 0) continue;
    s2 -= 8;
    if (s5 >= 0) continue;
    break;
  }
  return 0x00ffffffu;
}

/* 0x801A5A0C — same quad stream, fixed OT head. Unlike the indexed twin
 * the idx3 RTPS and the *a0 link word are computed BEFORE the MAC0/flags
 * gate every pass (guest order: lw *a0, lwc2 idx3, or tag, mfc2 MAC0,
 * RTPS, lh flags, then bgez/(flags+0x200)!=0). */
uint32_t prim_emit_quad_ot_fix(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  uint32_t s0,s1,s2,s3,s4,t4 = 0,t5,va0 = 0,va1 = 0;
  int32_t  s5;
  uint32_t w0,w1,i0,i1,i2,p,old,link;
  int32_t  mac0,flags;
  int first = 1;
  (void)a3;

  prim_emit_ctx_save();
  prim_emit_desc(a1,a2,&s0,&s4,&s1,&s2,&s3,&s5);
  (void)s4;

  w0 = DB32(s2); w1 = DB32(s2 + 4);
  i0 = w0 & 0xffffu; i1 = w0 >> 16;
  i2 = w1 & 0xffffu;
  p  = s1 + (i0 << 3); gte_lwc2(p,0);     gte_lwc2(p + 4,1);
  p  = s1 + (i1 << 3); gte_lwc2(p,2);     gte_lwc2(p + 4,3);
  p  = s1 + (i2 << 3); gte_lwc2(p,4);     gte_lwc2(p + 4,5);
  t4 = s1 + ((w1 >> 16) << 3);

  s5 -= 1;
  if (s5 > 0) s2 += 8; else s2 -= 8;

  for (;;) {
    if (!first) {
      setCopReg(2, GTE_DREG(0), va0);
      setCopReg(2, GTE_DREG(1), va1);
    }
    first = 0;
    s5 -= 1;
    copFunction(2, 0x0280030);                  /* RTPT */

    w0 = DB32(s2); w1 = DB32(s2 + 4);
    i0 = w0 & 0xffffu;
    p  = s1 + (i0 << 3);
    va0 = DB32(p); va1 = DB32(p + 4);
    i1 = w0 >> 16;
    p  = s1 + (i1 << 3); gte_lwc2(p,2); gte_lwc2(p + 4,3);
    (void)getCopReg(2, GTE_DREG(19));           /* SZ3 — dead here */
    copFunction(2, 0x0400006);                  /* NCLIP */
    i2 = w1 & 0xffffu;
    p  = s1 + (i2 << 3); gte_lwc2(p,4); gte_lwc2(p + 4,5);
    t5 = s1 + ((w1 >> 16) << 3);
    gte_swc2(s0 + 8, 12);
    gte_swc2(s0 + 0x10, 13);
    gte_swc2(s0 + 0x18, 14);

    old  = DB32(a0);
    gte_lwc2(t4,0); gte_lwc2(t4 + 4,1);
    link = 0x09000000u | old;
    mac0 = (int32_t)getCopReg(2, GTE_DREG(24)); /* pre-RTPS MAC0 */
    copFunction(2, 0x0180001);                  /* RTPS — unconditional */
    flags = (int32_t)*(int16_t *)guest_at(s3);
    /* Guest stores the link word unconditionally here: it sits in the
     * delay-slot path shared by the mac0>=0 pass and the (flags+0x200)!=0
     * pass, ahead of the *a0/swc2+0x20 stores that stay gated. */
    DB32(s0) = link;
    if (mac0 >= 0 || (flags + 0x200) != 0) {
      DB32(a0) = s0 & 0x00ffffffu;
      gte_swc2(s0 + 0x20, 14);
    }
    t4 = t5;
    s0 += 0x28; s2 += 8; s3 += 2;
    if (s5 > 0) continue;
    s2 -= 8;
    if (s5 >= 0) continue;
    break;
  }
  return 0x00ffffffu;
}

/* ======== 0x801A88DC / 0x801A8BC8 ======== */

/* Shared prologue for the textured tri/quad OT emitters. Returns the
 * descriptor fields the loop needs; *mode_out is the raw +0x32 hword and
 * *depth_out is the +0x14 stack cell (initialized to mode when nonzero,
 * rewritten by AVSZ3 on the mode==0 path). */
static int prim_emit_tex_desc(uint32_t desc, uint32_t *slotidx,
                              uint32_t *out, uint32_t *flags,
                              int32_t *mode_out, int32_t *depth_out)
{
  int32_t mode;
  if ((int32_t)(int16_t)GP16(desc + 0x34) == 0)
    return 0;
  mode = (int32_t)(int16_t)GP16(desc + 0x32);
  *slotidx = br2_ld32((uint32_t)(guest_gp() + 0x24));
  *out     = DB32(desc + (*slotidx << 2));
  *flags   = DB32(desc + 0x24);
  *mode_out = mode;
  *depth_out = mode;
  return 1;
}

/* Load three 8-int8_t vertex records (V0,V1,V2) by signed index. */
static void prim_emit_load_v012(int32_t i0, int32_t i1, int32_t i2,
                                uint32_t vtab)
{
  uint32_t p;
  p = vtab + ((uint32_t)i0 << 3); gte_lwc2(p,     0); gte_lwc2(p + 4, 1);
  p = vtab + ((uint32_t)i1 << 3); gte_lwc2(p,     2); gte_lwc2(p + 4, 3);
  p = vtab + ((uint32_t)i2 << 3); gte_lwc2(p,     4); gte_lwc2(p + 4, 5);
}

/* Indexed ordering-table insert used by both emitters: the OT slot is
 * otbase + depth*4, the record word gets tag|old, and the slot receives
 * the record's compressed 24-bit pointer. */
static void prim_emit_tag_insert(uint32_t otbase, uint32_t out,
                                 uint32_t depth, uint32_t tag)
{
  uint32_t slot = otbase + ((uint32_t)depth << 2);
  uint32_t old  = DB32(slot);
  DB32(out)  = tag | old;
  DB32(slot) = out & 0x00ffffffu;
}

/* 0x801A88DC — textured triangle emitter. Descriptor layout matches the
 * prim_emit_* family (+0x10 vertex table, +0x14 color stream, +0x18 index
 * stream of 3xh16 stride 6, +0x1c 8-int8_t color table, +0x20 secondary
 * index stream stride 2, +0x24 flag stream, +0x30 count, +0x32 mode,
 * +0x34 gate). Per record it RTPT/NCLIPs three vertices, optionally runs
 * AVSZ3 for the OT depth when mode==0, then emits a textured tri packet
 * for flag&0xff==2 records whose depth/depth-flag gate passes. Disasm
 * 0x801A88DC-0x801A8BC4. */
uint32_t prim_emit_tri_tex_ot(uint32_t desc)
{
  uint32_t slotidx, out, flags, vtab, idxs, cols, cidxs, ctab;
  int32_t  mode, depth, count, i;
  int32_t  mac;
  uint32_t otbase;

  if (!prim_emit_tex_desc(desc, &slotidx, &out, &flags, &mode, &depth))
    return 0;

  if (DB32(desc + 0x1c) == 0) {
    otbase = br2_ld32((uint32_t)(guest_gp() + 0x48));
    if (mode != 0) {
      uint32_t m = (uint32_t)mode;
      if ((uint32_t)(m - 1) < 0x7ffu)
        return prim_emit_tri_ot_fix(otbase + (m << 2), slotidx, desc, out);
      return 0;
    }
    return prim_emit_tri_ot_idx(otbase, slotidx, desc, out);
  }

  vtab  = DB32(desc + 0x10);
  idxs  = DB32(desc + 0x18);
  cols  = DB32(desc + 0x14);
  cidxs = desc + 0x20;
  count = (int32_t)(int16_t)GP16(desc + 0x30);
  if (count <= 0)
    return (uint32_t)count;

  for (i = 0; i < count; i++) {
    int32_t i0 = (int32_t)(int16_t)GP16(idxs);
    int32_t i1 = (int32_t)(int16_t)GP16(idxs + 2);
    int32_t i2 = (int32_t)(int16_t)GP16(idxs + 4);
    uint32_t f, attr;
    int adv_idx = 0, adv_col = 0;

    prim_emit_load_v012(i0, i1, i2, vtab);
    copFunction(2, 0x0280030u);              /* RTPT */
    copFunction(2, 0x1400006u);              /* NCLIP */
    mac = (int32_t)getCopReg(2, GTE_DREG(24));
    gte_swc2(out + 8,    12);
    gte_swc2(out + 0x10, 13);
    gte_swc2(out + 0x18, 14);
    if (mode == 0) {
      copFunction(2, 0x158002du);            /* AVSZ3 */
      depth = (int32_t)getCopReg(2, GTE_DREG(19)) >> 2;
    }

    f    = GP16(flags);
    attr = f & 0xffu;
    if (attr == 0) {
      cols += 4;
    } else if (attr == 2) {
      adv_idx = 1; adv_col = 1;
      if (mac >= 0 || (f & 0x200u) != 0) {
        if ((uint32_t)depth < 0x800u) {
          int32_t ci = (int32_t)(int16_t)GP16(cidxs);
          uint8_t ab;
          ctab = DB32(desc + 0x1c) + ((uint32_t)ci << 3);
          ab = GP8(out + 7);
          gte_lwc2(ctab,     0);
          gte_lwc2(ctab + 4, 1);
          gte_lwc2(cols,     6);
          copFunction(2, 0x108041bu);        /* NCCS */
          gte_swc2(out + 4, 22);
          GP8(out + 7) = ab;
          otbase = br2_ld32((uint32_t)(guest_gp() + 0x48));
          prim_emit_tag_insert(otbase, out, (uint32_t)depth, 0x07000000u);
        }
      }
    }

    if (adv_idx) cidxs += 2;
    if (adv_col) cols += 4;
    out += 0x20;
    idxs += 6;
    flags += 2;
  }
  return 0;
}

/* 0x801A8BC8 — textured quad emitter. Same descriptor contract as the
 * tri twin but the index stream is 4xh16 stride 8, output records are
 * 0x28 bytes, and each emitted quad first projects the fourth vertex
 * (RTPS -> out+0x20) before the NCCS color/OT-link tail uses tag
 * 0x09000000. Disasm 0x801A8BC8-0x801A8EDC. */
uint32_t prim_emit_quad_tex_ot(uint32_t desc)
{
  uint32_t slotidx, out, flags, vtab, idxs, cols, cidxs, ctab;
  int32_t  mode, depth, count, i;
  int32_t  mac;
  uint32_t otbase;

  if (!prim_emit_tex_desc(desc, &slotidx, &out, &flags, &mode, &depth))
    return 0;

  if (DB32(desc + 0x1c) == 0) {
    otbase = br2_ld32((uint32_t)(guest_gp() + 0x48));
    if (mode != 0) {
      uint32_t m = (uint32_t)mode;
      if ((uint32_t)(m - 1) < 0x7ffu)
        return prim_emit_quad_ot_fix(otbase + (m << 2), slotidx, desc, out);
      return 0;
    }
    return prim_emit_quad_ot_idx(otbase, slotidx, desc, out);
  }

  vtab  = DB32(desc + 0x10);
  idxs  = DB32(desc + 0x18);
  cols  = DB32(desc + 0x14);
  cidxs = desc + 0x20;
  count = (int32_t)(int16_t)GP16(desc + 0x30);
  if (count <= 0)
    return (uint32_t)count;

  for (i = 0; i < count; i++) {
    int32_t i0 = (int32_t)(int16_t)GP16(idxs);
    int32_t i1 = (int32_t)(int16_t)GP16(idxs + 2);
    int32_t i2 = (int32_t)(int16_t)GP16(idxs + 4);
    uint32_t f, attr;
    int adv_idx = 0, adv_col = 0;

    prim_emit_load_v012(i0, i1, i2, vtab);
    copFunction(2, 0x0280030u);              /* RTPT */
    copFunction(2, 0x1400006u);              /* NCLIP */
    mac = (int32_t)getCopReg(2, GTE_DREG(24));
    gte_swc2(out + 8,    12);
    gte_swc2(out + 0x10, 13);
    gte_swc2(out + 0x18, 14);
    if (mode == 0) {
      copFunction(2, 0x158002du);            /* AVSZ3 */
      depth = (int32_t)getCopReg(2, GTE_DREG(19)) >> 2;
    }

    f    = GP16(flags);
    attr = f & 0xffu;
    if (attr == 0) {
      cols += 4;
    } else if (attr == 2) {
      adv_idx = 1; adv_col = 1;
      if (mac >= 0 || (f & 0x200u) != 0) {
        if ((uint32_t)depth < 0x800u) {
          int32_t i3 = (int32_t)(int16_t)GP16(idxs + 6);
          int32_t ci;
          uint32_t p, ab;
          p = vtab + ((uint32_t)i3 << 3);
          gte_lwc2(p,     0);
          gte_lwc2(p + 4, 1);
          copFunction(2, 0x0180001u);        /* RTPS — 4th vertex */
          gte_swc2(out + 0x20, 14);
          ci = (int32_t)(int16_t)GP16(cidxs);
          ctab = DB32(desc + 0x1c) + ((uint32_t)ci << 3);
          ab = GP8(out + 7);
          gte_lwc2(ctab,     0);
          gte_lwc2(ctab + 4, 1);
          gte_lwc2(cols,     6);
          copFunction(2, 0x108041bu);        /* NCCS */
          gte_swc2(out + 4, 22);
          GP8(out + 7) = (uint8_t)ab;
          otbase = br2_ld32((uint32_t)(guest_gp() + 0x48));
          prim_emit_tag_insert(otbase, out, (uint32_t)depth, 0x09000000u);
        }
      }
    }

    if (adv_idx) cidxs += 2;
    if (adv_col) cols += 4;
    out += 0x28;
    idxs += 8;
    flags += 2;
  }
  return 0;
}

/* ======== 0x8018bb38 ======== */

/* flag_801b55d0_set — sets the flag word @0x801b55d0 to 1 and returns 1.
 * The flag is polled as (DAT_801b55d0 & 1) by the record/string emit
 * paths (battle.c). Disasm 0x8018bb38-0x8018bb44 (lui v1,0x801b;
 * li v0,1; sw v0,0x55d0(v1); jr ra). */
uint32_t flag_801b55d0_set(void)
{
  DB32(0x801b55d0) = 1;
  return 1;
}

/* ======== 0x8018bd24 ======== */

/* rec_hibyte_pair_swap — cross record high-int8_t store: t = *(u32*)a0;
 * *(u16*)(a0+2) = (u16)(a1 << 8); *(u16*)(a1+2) = (u16)(t << 8);
 * returns a0. Each sh keeps only the source low int8_t in the halfword's
 * high int8_t (low int8_t zeroed). Disasm 0x8018bd24-0x8018bd3c
 * (lw, sll x2, sh x2, jr ra with v0=a0 in the delay slot). */
uint32_t rec_hibyte_pair_swap(uint32_t a0, uint32_t a1)
{
  uint32_t t = DB32(a0);
  GP16(a0 + 2) = (uint16_t)(a1 << 8);
  GP16(a1 + 2) = (uint16_t)(t << 8);
  return a0;
}

/* ======== 0x8018b76c ======== */

/* swl_guest — faithful SWL (store-word-left) on a guest address: merges the
 * high bytes of v into the aligned word at addr&~3, exactly the merge table
 * in generated/SLES_017.22_decls.h psx_swl. Used by the OT-link tail below
 * (guest stores at +2 offsets are unaligned by design). */
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

/* prim_ot_link_indexed — shared OT-link tail of the 0x801872cc-0x8018786c
 * emit family: slot = ot + (OTZ & 0x1ffc); old = *slot; swl slot+2 <-
 * out<<8; swl out+2 <- old<<8. Disasm pattern at e.g. 0x80187344-0x80187364. */
static void prim_ot_link_indexed(uint32_t ot, uint32_t out, uint32_t otz)
{
  uint32_t slot = ot + (otz & 0x1ffcu);
  uint32_t old  = DB32(slot);
  swl_guest(slot + 2, out << 8);
  swl_guest(out + 2, old << 8);
}

/* ======== 0x801872cc ======== */

/* prim_emit_4c — record-fn GTE emitter, input stride 0x4c. a0=cursor,
 * a1=OT base, a2=variant select, a3=signed count; returns advanced cursor.
 * Per record: lwc2 V0-V2, RTPT, NCLIP; MAC0>0 gates AVSZ3 + one extra
 * transformed vector + NCCS color, then the indexed OT link. Disasm
 * 0x801872cc-0x80187374. */
uint32_t prim_emit_4c(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x24u : 0x38u;
    uint32_t out, w18, w1c, w20, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    w18 = DB32(cur + 0x18);
    w1c = DB32(cur + 0x1c);
    w20 = DB32(cur + 0x20);
    (void)getCopControlWord(2, GTE_CREG(31));    /* cfc2 FLAG (dead) */
    copFunction(2, 0x1400006u);                  /* NCLIP */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24)); /* MAC0 */
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      copFunction(2, 0x158002du);                /* AVSZ3 */
      setCopReg(2, GTE_DREG(6), w18);
      setCopReg(2, GTE_DREG(0), w1c);
      setCopReg(2, GTE_DREG(1), w20);
      DB32(out + 8) = (uint32_t)getCopReg(2, GTE_DREG(12));
      copFunction(2, 0x108041bu);                /* NCCS */
      DB32(out + 0x0c) = (uint32_t)getCopReg(2, GTE_DREG(13));
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(14));
      otz = (uint32_t)getCopReg(2, GTE_DREG(7)); /* OTZ */
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x4c;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187378 ======== */

/* prim_emit_5c — input stride 0x5c. Same RTPT/NCLIP/MAC0 gate as prim_emit_4c,
 * then stores SXY0, projects one extra vector (RTPS), runs AVSZ4 + NCCS and
 * links OT. Disasm 0x80187378-0x80187438. */
uint32_t prim_emit_5c(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x2cu : 0x44u;
    uint32_t out, w18, w1c, w20, w24, w28, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    w18 = DB32(cur + 0x18);
    w1c = DB32(cur + 0x1c);
    w20 = DB32(cur + 0x20);
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      DB32(out + 8) = (uint32_t)getCopReg(2, GTE_DREG(12));
      setCopReg(2, GTE_DREG(0), w18);
      setCopReg(2, GTE_DREG(1), w1c);
      setCopReg(2, GTE_DREG(6), w20);
      copFunction(2, 0x0180001u);                /* RTPS */
      w24 = DB32(cur + 0x24);
      w28 = DB32(cur + 0x28);
      copFunction(2, 0x168002eu);                /* AVSZ4 */
      setCopReg(2, GTE_DREG(0), w24);
      setCopReg(2, GTE_DREG(1), w28);
      DB32(out + 0x0c) = (uint32_t)getCopReg(2, GTE_DREG(12));
      copFunction(2, 0x108041bu);                /* NCCS */
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(13));
      DB32(out + 0x14) = (uint32_t)getCopReg(2, GTE_DREG(14));
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x5c;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x8018743c ======== */

/* prim_emit_6c — input stride 0x6c. RTPT/NCLIP/MAC0 gate, AVSZ3, loads a
 * second vertex triple into GTE 0-5 and runs NCCT; stores three color words.
 * Disasm 0x8018743c-0x80187500. */
uint32_t prim_emit_6c(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x34u : 0x50u;
    uint32_t out, w18, w1c, w20, w24, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    w18 = DB32(cur + 0x18);
    w1c = DB32(cur + 0x1c);
    w20 = DB32(cur + 0x20);
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    w24 = DB32(cur + 0x24);
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      copFunction(2, 0x158002du);                /* AVSZ3 */
      setCopReg(2, GTE_DREG(6), w18);
      setCopReg(2, GTE_DREG(0), w1c);
      setCopReg(2, GTE_DREG(1), w20);
      setCopReg(2, GTE_DREG(2), w24);
      gte_lwc2(cur + 0x28, 3);
      gte_lwc2(cur + 0x2c, 4);
      gte_lwc2(cur + 0x30, 5);
      DB32(out + 8) = (uint32_t)getCopReg(2, GTE_DREG(12));
      copFunction(2, 0x118043fu);                /* NCCT */
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(13));
      DB32(out + 0x18) = (uint32_t)getCopReg(2, GTE_DREG(14));
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 4)  = (uint32_t)getCopReg(2, GTE_DREG(20));
      DB32(out + 0x0c) = (uint32_t)getCopReg(2, GTE_DREG(21));
      DB32(out + 0x14) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x6c;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187504 ======== */

/* prim_emit_8c — input stride 0x8c. Longest member of the 0x801872cc family:
 * RTPT/NCLIP/MAC0 gate, two projected vectors, AVSZ4 + NCCT + NCCS color
 * groups and the indexed OT link. Disasm 0x80187504-0x801875f8. */
uint32_t prim_emit_8c(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x44u : 0x68u;
    uint32_t out, w18, w1c, w20, w2c, w3c, w40, g14, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    w20 = DB32(cur + 0x20);
    w18 = DB32(cur + 0x18);
    w1c = DB32(cur + 0x1c);
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    w2c = DB32(cur + 0x2c);
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      DB32(out + 8) = (uint32_t)getCopReg(2, GTE_DREG(12));
      setCopReg(2, GTE_DREG(0), w18);
      setCopReg(2, GTE_DREG(1), w1c);
      setCopReg(2, GTE_DREG(6), w20);
      copFunction(2, 0x0180001u);                /* RTPS */
      setCopReg(2, GTE_DREG(2), w2c);
      gte_lwc2(cur + 0x30, 3);
      gte_lwc2(cur + 0x34, 4);
      gte_lwc2(cur + 0x38, 5);
      gte_lwc2(cur + 0x24, 0);
      gte_lwc2(cur + 0x28, 1);
      g14 = (uint32_t)getCopReg(2, GTE_DREG(14));
      copFunction(2, 0x118043fu);                /* NCCT */
      w3c = DB32(cur + 0x3c);
      w40 = DB32(cur + 0x40);
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(12));
      DB32(out + 0x18) = (uint32_t)getCopReg(2, GTE_DREG(13));
      DB32(out + 0x20) = g14;
      copFunction(2, 0x168002eu);                /* AVSZ4 */
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(20));
      setCopReg(2, GTE_DREG(0), w3c);
      setCopReg(2, GTE_DREG(1), w40);
      copFunction(2, 0x108041bu);                /* NCCS */
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 0x0c) = (uint32_t)getCopReg(2, GTE_DREG(20));
      DB32(out + 0x14) = (uint32_t)getCopReg(2, GTE_DREG(21));
      DB32(out + 0x1c) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x8c;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x801875fc ======== */

/* prim_emit_60 — input stride 0x60. RTPT/NCLIP gate with the attribute int8_t
 * at out+7 folded into GTE6 ((b<<24)|0x808080), AVSZ3 + NCCS, OT link.
 * Disasm 0x801875fc-0x801876b8. */
uint32_t prim_emit_60(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x20u : 0x40u;
    uint32_t out, attr, w18, w1c, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    out  = cur + delta;
    attr = ((uint32_t)GP8(out + 7) << 24) | 0x00808080u;
    w18  = DB32(cur + 0x18);
    w1c  = DB32(cur + 0x1c);
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    setCopReg(2, GTE_DREG(6), attr);
    setCopReg(2, GTE_DREG(0), w18);
    setCopReg(2, GTE_DREG(1), w1c);
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    if ((int32_t)mac0 > 0) {
      copFunction(2, 0x158002du);                /* AVSZ3 */
      DB32(out + 8)  = (uint32_t)getCopReg(2, GTE_DREG(12));
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(13));
      DB32(out + 0x18) = (uint32_t)getCopReg(2, GTE_DREG(14));
      copFunction(2, 0x108041bu);                /* NCCS */
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x60;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x801876bc ======== */

/* prim_emit_78 — input stride 0x78. RTPT/NCLIP gate, attribute-int8_t GTE6,
 * one RTPS vector, AVSZ4 + NCCS, OT link; also stores the pre-branch SXY0
 * read (gte12) as a GPR word at out+8. Disasm 0x801876bc-0x80187790. */
uint32_t prim_emit_78(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x28u : 0x50u;
    uint32_t out, attr, w18, w1c, w20, w24, mac0, sxy0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    out  = cur + delta;
    attr = ((uint32_t)GP8(out + 7) << 24) | 0x00808080u;
    w18  = DB32(cur + 0x18);
    w1c  = DB32(cur + 0x1c);
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    setCopReg(2, GTE_DREG(6), attr);
    setCopReg(2, GTE_DREG(0), w18);
    setCopReg(2, GTE_DREG(1), w1c);
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12));
    if ((int32_t)mac0 > 0) {
      copFunction(2, 0x0180001u);                /* RTPS */
      w20 = DB32(cur + 0x20);
      w24 = DB32(cur + 0x24);
      DB32(out + 0x20) = (uint32_t)getCopReg(2, GTE_DREG(14));
      setCopReg(2, GTE_DREG(0), w20);
      setCopReg(2, GTE_DREG(1), w24);
      DB32(out + 8) = sxy0;
      copFunction(2, 0x168002eu);                /* AVSZ4 */
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(12));
      DB32(out + 0x18) = (uint32_t)getCopReg(2, GTE_DREG(13));
      copFunction(2, 0x108041bu);                /* NCCS */
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x78;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187794 ======== */

/* prim_emit_80 — input stride 0x80. RTPT/NCLIP gate, attribute-int8_t GTE6,
 * AVSZ3 + NCCT color triple, OT link. Disasm 0x80187794-0x80187868. */
uint32_t prim_emit_80(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x30u : 0x58u;
    uint32_t out, attr, w18, w1c, w20, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    out  = cur + delta;
    attr = ((uint32_t)GP8(out + 7) << 24) | 0x00808080u;
    w18  = DB32(cur + 0x18);
    w1c  = DB32(cur + 0x1c);
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    setCopReg(2, GTE_DREG(0), w18);
    setCopReg(2, GTE_DREG(1), w1c);
    w20 = DB32(cur + 0x20);
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    setCopReg(2, GTE_DREG(6), attr);
    if ((int32_t)mac0 > 0) {
      copFunction(2, 0x158002du);                /* AVSZ3 */
      setCopReg(2, GTE_DREG(2), w20);
      gte_lwc2(cur + 0x24, 3);
      gte_lwc2(cur + 0x28, 4);
      gte_lwc2(cur + 0x2c, 5);
      DB32(out + 8) = (uint32_t)getCopReg(2, GTE_DREG(12));
      copFunction(2, 0x118043fu);                /* NCCT */
      DB32(out + 0x14) = (uint32_t)getCopReg(2, GTE_DREG(13));
      DB32(out + 0x20) = (uint32_t)getCopReg(2, GTE_DREG(14));
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 4)   = (uint32_t)getCopReg(2, GTE_DREG(20));
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(21));
      DB32(out + 0x1c) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x80;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x8018786c ======== */

/* prim_emit_a8 — input stride 0xa8. Largest member of the family: RTPT/NCLIP
 * gate, attribute-int8_t GTE6, RTPS + NCCT + AVSZ4 + NCCS color groups and
 * the indexed OT link. Disasm 0x8018786c-0x8018797c. */
uint32_t prim_emit_a8(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x40u : 0x74u;
    uint32_t out, attr, w18, w1c, w28, w38, w3c, mac0, sxy0, g14, c0, c1, c2, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    out  = cur + delta;
    attr = ((uint32_t)GP8(out + 7) << 24) | 0x00808080u;
    w18  = DB32(cur + 0x18);
    w1c  = DB32(cur + 0x1c);
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    setCopReg(2, GTE_DREG(0), w18);
    setCopReg(2, GTE_DREG(1), w1c);
    w28 = DB32(cur + 0x28);
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12));
    if ((int32_t)mac0 > 0) {
      copFunction(2, 0x0180001u);                /* RTPS */
      setCopReg(2, GTE_DREG(6), attr);
      setCopReg(2, GTE_DREG(2), w28);
      gte_lwc2(cur + 0x2c, 3);
      gte_lwc2(cur + 0x30, 4);
      gte_lwc2(cur + 0x34, 5);
      gte_lwc2(cur + 0x20, 0);
      gte_lwc2(cur + 0x24, 1);
      g14 = (uint32_t)getCopReg(2, GTE_DREG(14));
      copFunction(2, 0x118043fu);                /* NCCT */
      w38 = DB32(cur + 0x38);
      w3c = DB32(cur + 0x3c);
      DB32(out + 8)    = sxy0;
      DB32(out + 0x2c) = g14;
      DB32(out + 0x14) = (uint32_t)getCopReg(2, GTE_DREG(12));
      DB32(out + 0x20) = (uint32_t)getCopReg(2, GTE_DREG(13));
      copFunction(2, 0x168002eu);                /* AVSZ4 */
      c0 = (uint32_t)getCopReg(2, GTE_DREG(20));
      c1 = (uint32_t)getCopReg(2, GTE_DREG(21));
      c2 = (uint32_t)getCopReg(2, GTE_DREG(22));
      setCopReg(2, GTE_DREG(0), w38);
      setCopReg(2, GTE_DREG(1), w3c);
      DB32(out + 4)    = c0;
      copFunction(2, 0x108041bu);                /* NCCS */
      DB32(out + 0x10) = c1;
      DB32(out + 0x1c) = c2;
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 0x28) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0xa8;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187980 ======== */

/* prim_emit_64 — input stride 0x64. Same RTPT/NCLIP/MAC0 gate as the rest
 * of the family; three NCCS color groups (cur+0x18/+0x1c/+0x20 attribute
 * bytes via gte6) interleaved with AVSZ3 and the indexed OT link.
 * a2==0 -> out delta 0x2c, else 0x48. Disasm 0x80187980-0x80187a46. */
uint32_t prim_emit_64(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  uint32_t delta;
  if (cnt <= 0) return cur;
  delta = (sel == 0) ? 0x2cu : 0x48u;
  do {
    uint32_t out, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      gte_lwc2(cur + 0x18, 6);
      gte_lwc2(cur + 0x24, 0);
      gte_lwc2(cur + 0x28, 1);
      copFunction(2, 0x108041Bu);                /* NCCS */
      gte_swc2(out + 0x08, 12);
      gte_swc2(out + 0x10, 13);
      gte_swc2(out + 0x18, 14);
      gte_swc2(out + 0x04, 22);
      gte_lwc2(cur + 0x1c, 6);
      copFunction(2, 0x108041Bu);                /* NCCS */
      copFunction(2, 0x158002Du);                /* AVSZ3 */
      gte_lwc2(cur + 0x20, 6);
      gte_swc2(out + 0x0c, 22);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x108041Bu);                /* NCCS */
      prim_ot_link_indexed(ot, out, otz);
      gte_swc2(out + 0x14, 22);
    }
    cur += 0x64;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187a48 ======== */

/* prim_emit_74 — input stride 0x74. Same skeleton as prim_emit_64 but each
 * NCCS group reloads gte6/gte0/gte1 from a wider set of record fields
 * (+0x18/+0x24/+0x28, +0x1c/+0x2c/+0x30, +0x20/+0x34/+0x38). a2==0 ->
 * delta 0x3c, else 0x58. Disasm 0x80187a48-0x80187b1e. */
uint32_t prim_emit_74(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  uint32_t delta;
  if (cnt <= 0) return cur;
  delta = (sel == 0) ? 0x3cu : 0x58u;
  do {
    uint32_t out, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      gte_lwc2(cur + 0x18, 6);
      gte_lwc2(cur + 0x24, 0);
      gte_lwc2(cur + 0x28, 1);
      copFunction(2, 0x108041Bu);                /* NCCS */
      gte_swc2(out + 0x08, 12);
      gte_swc2(out + 0x10, 13);
      gte_swc2(out + 0x18, 14);
      gte_swc2(out + 0x04, 22);
      gte_lwc2(cur + 0x1c, 6);
      gte_lwc2(cur + 0x2c, 0);
      gte_lwc2(cur + 0x30, 1);
      copFunction(2, 0x108041Bu);                /* NCCS */
      copFunction(2, 0x158002Du);                /* AVSZ3 */
      gte_lwc2(cur + 0x20, 6);
      gte_lwc2(cur + 0x34, 0);
      gte_lwc2(cur + 0x38, 1);
      gte_swc2(out + 0x0c, 22);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x108041Bu);                /* NCCS */
      prim_ot_link_indexed(ot, out, otz);
      gte_swc2(out + 0x14, 22);
    }
    cur += 0x74;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187b20 ======== */

/* prim_emit_80b — input stride 0x80, sibling of prim_emit_80 (0x80187794)
 * but with a richer tail: saves pre-RTPS SXY0 to out+8 via GPR, projects a
 * second vertex (RTPS on cur+0x18/+0x1c), AVSZ4 + four NCCS color groups
 * and the indexed OT link. a2==0 -> delta 0x38, else 0x5c.
 * Disasm 0x80187b20-0x80187c0e. */
uint32_t prim_emit_80b(uint32_t cur, uint32_t ot, uint32_t sel,
                       int32_t count)
{
  int32_t cnt = count;
  uint32_t delta;
  if (cnt <= 0) return cur;
  delta = (sel == 0) ? 0x38u : 0x5cu;
  do {
    uint32_t out, mac0, otz, sxy0;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12));
      gte_lwc2(cur + 0x18, 0);
      gte_lwc2(cur + 0x1c, 1);
      copFunction(2, 0x0180001u);                /* RTPS */
      DB32(out + 0x08) = sxy0;
      gte_swc2(out + 0x20, 14);
      gte_lwc2(cur + 0x20, 6);
      gte_lwc2(cur + 0x30, 0);
      gte_lwc2(cur + 0x34, 1);
      copFunction(2, 0x108041Bu);                /* NCCS */
      gte_swc2(out + 0x10, 12);
      gte_swc2(out + 0x18, 13);
      gte_swc2(out + 0x04, 22);
      gte_lwc2(cur + 0x24, 6);
      copFunction(2, 0x108041Bu);                /* NCCS */
      copFunction(2, 0x168002Eu);                /* AVSZ4 */
      gte_lwc2(cur + 0x28, 6);
      gte_swc2(out + 0x0c, 22);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x108041Bu);                /* NCCS */
      prim_ot_link_indexed(ot, out, otz);
      gte_swc2(out + 0x14, 22);
      gte_lwc2(cur + 0x2c, 6);
      copFunction(2, 0x108041Bu);                /* NCCS */
      gte_swc2(out + 0x1c, 22);
    }
    cur += 0x80;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187c10 ======== */

/* prim_emit_98 — input stride 0x98, largest of the cluster: stores the
 * pre-block SXY0 (gte12) to out+8 first, then RTPS on cur+0x18/+0x1c,
 * AVSZ4 and four NCCS groups whose gte0/gte1 pairs walk +0x30/+0x34,
 * +0x38/+0x3c, +0x40/+0x44, +0x48/+0x4c. a2==0 -> delta 0x50, else 0x74.
 * Disasm 0x80187c10-0x80187d12. */
uint32_t prim_emit_98(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  uint32_t delta;
  if (cnt <= 0) return cur;
  delta = (sel == 0) ? 0x50u : 0x74u;
  do {
    uint32_t out, mac0, otz;
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;
    copFunction(2, 0x0280030u);                  /* RTPT */
    (void)getCopControlWord(2, GTE_CREG(31));
    copFunction(2, 0x1400006u);                  /* NCLIP */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    out  = cur + delta;
    if ((int32_t)mac0 > 0) {
      gte_swc2(out + 0x08, 12);
      gte_lwc2(cur + 0x18, 0);
      gte_lwc2(cur + 0x1c, 1);
      copFunction(2, 0x0180001u);                /* RTPS */
      gte_lwc2(cur + 0x20, 6);
      gte_lwc2(cur + 0x30, 0);
      gte_lwc2(cur + 0x34, 1);
      copFunction(2, 0x108041Bu);                /* NCCS */
      gte_swc2(out + 0x10, 12);
      gte_swc2(out + 0x18, 13);
      gte_swc2(out + 0x20, 14);
      gte_swc2(out + 0x04, 22);
      gte_lwc2(cur + 0x24, 6);
      gte_lwc2(cur + 0x38, 0);
      gte_lwc2(cur + 0x3c, 1);
      copFunction(2, 0x108041Bu);                /* NCCS */
      copFunction(2, 0x168002Eu);                /* AVSZ4 */
      gte_lwc2(cur + 0x28, 6);
      gte_lwc2(cur + 0x40, 0);
      gte_lwc2(cur + 0x44, 1);
      gte_swc2(out + 0x0c, 22);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x108041Bu);                /* NCCS */
      prim_ot_link_indexed(ot, out, otz);
      gte_swc2(out + 0x14, 22);
      gte_lwc2(cur + 0x2c, 6);
      gte_lwc2(cur + 0x48, 0);
      gte_lwc2(cur + 0x4c, 1);
      copFunction(2, 0x108041Bu);                /* NCCS */
      gte_swc2(out + 0x1c, 22);
    }
    cur += 0x98;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187e18 ======== */

/* prim_emit_3c — input stride 0x3c. Per record three pointer words at
 * cur+0/+4/+8 are dereferenced into GTE SXY0-2 (regs 12-14) and NCLIP'd;
 * MAC0 gates a gouraud/color tail: second words of the three pointers
 * feed gte17-19, a fourth pointer at cur+16 supplies the packed color
 * (split lo/hi16 into gte9/gte10), AVSZ3, CC (func 0x1C) and the indexed
 * OT link. Stores the three raw pointer-deref words to out+8/+0xc/+0x10
 * unconditionally. a2==0 -> delta 0x14, else 0x28.
 * Disasm 0x80187e18-0x80187ede. */
uint32_t prim_emit_3c(uint32_t cur, uint32_t ot, uint32_t sel,
                      int32_t count)
{
  int32_t cnt = count;
  uint32_t delta;
  if (cnt <= 0) return cur;
  delta = (sel == 0) ? 0x14u : 0x28u;
  do {
    uint32_t out, pa, pb, pc, pd, mac0, otz, col, v8, v9, v10;
    pa = DB32(cur + 0x00);
    pb = DB32(cur + 0x04);
    pc = DB32(cur + 0x08);
    v8 = DB32(pa);
    v9 = DB32(pb);
    v10 = DB32(pc);
    setCopReg(2, GTE_DREG(12), v8);
    setCopReg(2, GTE_DREG(13), v9);
    setCopReg(2, GTE_DREG(14), v10);
    cnt--;
    out = cur + delta;
    copFunction(2, 0x1400006u);                  /* NCLIP */
    pd  = DB32(cur + 0x10);
    DB32(out + 0x08) = v8;
    DB32(out + 0x0c) = v9;
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24));
    DB32(out + 0x10) = v10;
    if ((int32_t)mac0 > 0) {
      gte_lwc2(pa + 4, 17);
      gte_lwc2(pb + 4, 18);
      gte_lwc2(pc + 4, 19);
      col = DB32(pd);
      copFunction(2, 0x158002Du);                /* AVSZ3 */
      gte_lwc2(cur + 0x0c, 6);
      setCopReg(2, GTE_DREG(9),  col & 0xffffu);
      setCopReg(2, GTE_DREG(10), col >> 16);
      gte_lwc2(pd + 4, 11);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x138041Cu);                /* CC */
      prim_ot_link_indexed(ot, out, otz);
      gte_swc2(out + 0x04, 22);
    }
    cur += 0x3c;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187d14 ======== */

/* prim_tri_batch_rtpt — stride-0x30 triangle batch RTPT. a0=record cursor,
 * a3=signed vertex count (consumed in groups of 3); returns advanced cursor.
 * Each record carries six input words at +0/+4/+0x10/+0x14/+0x20/+0x24
 * (loaded into GTE V0-V2) and receives the transformed SXY pairs at
 * +8/+0xc, +0x18/+0x1c, +0x28/+0x2c — partial tails store only the leading
 * pairs (count mod 3). Disasm 0x80187d14-0x80187dc8. */
uint32_t prim_tri_batch_rtpt(uint32_t cur, uint32_t a1, uint32_t a2,
                             int32_t count)
{
  int32_t rem = count;
  (void)a1; (void)a2;
  if (rem <= 0) return cur;
  do {
    uint32_t w0 = DB32(cur + 0x00);
    uint32_t w1 = DB32(cur + 0x04);
    uint32_t w2 = DB32(cur + 0x10);
    uint32_t w3 = DB32(cur + 0x14);
    uint32_t w4 = DB32(cur + 0x20);
    uint32_t w5 = DB32(cur + 0x24);
    int32_t verts = (rem < 3) ? rem : 3;
    setCopReg(2, GTE_DREG(0), w0);
    setCopReg(2, GTE_DREG(1), w1);
    setCopReg(2, GTE_DREG(2), w2);
    setCopReg(2, GTE_DREG(3), w3);
    setCopReg(2, GTE_DREG(4), w4);
    setCopReg(2, GTE_DREG(5), w5);
    rem -= 3;
    copFunction(2, 0x0280030u);                  /* RTPT */
    if (verts >= 1) {
      DB32(cur + 0x08) = (uint32_t)getCopReg(2, GTE_DREG(12));
      DB32(cur + 0x0c) = (uint32_t)getCopReg(2, GTE_DREG(17));
    }
    if (verts >= 2) {
      DB32(cur + 0x18) = (uint32_t)getCopReg(2, GTE_DREG(13));
      DB32(cur + 0x1c) = (uint32_t)getCopReg(2, GTE_DREG(18));
    }
    if (verts >= 3) {
      DB32(cur + 0x28) = (uint32_t)getCopReg(2, GTE_DREG(14));
      DB32(cur + 0x2c) = (uint32_t)getCopReg(2, GTE_DREG(19));
    }
    cur += 0x30;
  } while (rem > 0);
  return cur;
}

/* ======== 0x80187ee0 ======== */

/* prim_emit_ptr_54 — pointer-chase emit variant, stride 0x54. a0=cursor,
 * a1=OT base, a2=variant select, a3=count; returns advanced cursor. The
 * first three record words are vertex POINTERS: their pointees feed
 * NCLIP via GTE12-14 and are also copied to out+8/+0x10/+0x18. When MAC0>0
 * three pointer-pairs (cur+0x10/0x14/0x18 -> attr/UV words) drive the
 * 0x138041c shade ops with AVSZ3 depth before the indexed OT link.
 * Disasm 0x80187ee0-0x80187ff8. */
uint32_t prim_emit_ptr_54(uint32_t cur, uint32_t ot, uint32_t sel,
                          int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x1cu : 0x38u;
    uint32_t p0 = DB32(cur + 0x00);
    uint32_t p1 = DB32(cur + 0x04);
    uint32_t p2 = DB32(cur + 0x08);
    uint32_t w0 = DB32(p0);
    uint32_t w1 = DB32(p1);
    uint32_t w2 = DB32(p2);
    uint32_t out, pa, pb, pc, t2, otz;
    int32_t mac0;
    setCopReg(2, GTE_DREG(12), w0);
    setCopReg(2, GTE_DREG(13), w1);
    setCopReg(2, GTE_DREG(14), w2);
    cnt--;
    out = cur + delta;
    copFunction(2, 0x1400006u);                  /* NCLIP */
    pa = DB32(cur + 0x10);
    DB32(out + 0x08) = w0;
    DB32(out + 0x10) = w1;
    mac0 = (int32_t)getCopReg(2, GTE_DREG(24));
    DB32(out + 0x18) = w2;
    if (mac0 > 0) {
      t2 = DB32(pa);
      gte_lwc2(cur + 0x0c, 6);
      setCopReg(2, GTE_DREG(9),  t2 & 0xffffu);
      setCopReg(2, GTE_DREG(10), t2 >> 16);
      gte_lwc2(pa + 4, 11);
      gte_lwc2(p0 + 4, 17);
      copFunction(2, 0x138041cu);
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(22));

      pb = DB32(cur + 0x14);
      gte_lwc2(p1 + 4, 18);
      t2 = DB32(pb);
      setCopReg(2, GTE_DREG(9),  t2 & 0xffffu);
      setCopReg(2, GTE_DREG(10), t2 >> 16);
      gte_lwc2(pb + 4, 11);
      copFunction(2, 0x138041cu);
      DB32(out + 0x0c) = (uint32_t)getCopReg(2, GTE_DREG(22));

      pc = DB32(cur + 0x18);
      gte_lwc2(p2 + 4, 19);
      t2 = DB32(pc);
      copFunction(2, 0x158002du);                /* AVSZ3 */
      setCopReg(2, GTE_DREG(9),  t2 & 0xffffu);
      setCopReg(2, GTE_DREG(10), t2 >> 16);
      gte_lwc2(pc + 4, 11);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x138041cu);
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 0x14) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x54;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x80187ffc ======== */

/* prim_emit_ptr_50 — pointer-chase emit variant, stride 0x50. Same NCLIP
 * pointer preamble as prim_emit_ptr_54 but a single shade pointer at
 * cur+0xc; MAC0>0 folds the out+7 attribute int8_t into GTE6, runs AVSZ3 +
 * one 0x138041c shade and the indexed OT link. Disasm
 * 0x80187ffc-0x801880d0. */
uint32_t prim_emit_ptr_50(uint32_t cur, uint32_t ot, uint32_t sel,
                          int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x10u : 0x30u;
    uint32_t p0 = DB32(cur + 0x00);
    uint32_t p1 = DB32(cur + 0x04);
    uint32_t p2 = DB32(cur + 0x08);
    uint32_t w0 = DB32(p0);
    uint32_t w1 = DB32(p1);
    uint32_t w2 = DB32(p2);
    uint32_t out, pn, attr, t2, otz;
    int32_t mac0;
    setCopReg(2, GTE_DREG(12), w0);
    setCopReg(2, GTE_DREG(13), w1);
    setCopReg(2, GTE_DREG(14), w2);
    cnt--;
    out = cur + delta;
    copFunction(2, 0x1400006u);                  /* NCLIP */
    DB32(out + 0x08) = w0;
    DB32(out + 0x10) = w1;
    DB32(out + 0x18) = w2;
    mac0 = (int32_t)getCopReg(2, GTE_DREG(24));
    if (mac0 > 0) {
      pn   = DB32(cur + 0x0c);
      attr = ((uint32_t)GP8(out + 7) << 24) | 0x00808080u;
      t2   = DB32(pn);
      setCopReg(2, GTE_DREG(6), attr);
      gte_lwc2(p0 + 4, 17);
      gte_lwc2(p1 + 4, 18);
      gte_lwc2(p2 + 4, 19);
      copFunction(2, 0x158002du);                /* AVSZ3 */
      setCopReg(2, GTE_DREG(9),  t2 & 0xffffu);
      setCopReg(2, GTE_DREG(10), t2 >> 16);
      gte_lwc2(pn + 4, 11);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x138041cu);
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x50;
  } while (cnt > 0);
  return cur;
}

/* ======== 0x801880d4 ======== */

/* prim_emit_ptr_68 — pointer-chase emit variant, stride 0x68. prim_emit_ptr_50
 * widened to three shade pointers at cur+0xc/0x10/0x14 with wider output
 * slots (+8/+0x14/+0x20 stores, colors at +4/+0x10/+0x1c). Disasm
 * 0x801880d4-0x801881fc. */
uint32_t prim_emit_ptr_68(uint32_t cur, uint32_t ot, uint32_t sel,
                          int32_t count)
{
  int32_t cnt = count;
  if (cnt <= 0) return cur;
  do {
    uint32_t delta = (sel == 0) ? 0x18u : 0x40u;
    uint32_t p0 = DB32(cur + 0x00);
    uint32_t p1 = DB32(cur + 0x04);
    uint32_t p2 = DB32(cur + 0x08);
    uint32_t w0 = DB32(p0);
    uint32_t w1 = DB32(p1);
    uint32_t w2 = DB32(p2);
    uint32_t out, pa, pb, pc, attr, t2, otz;
    int32_t mac0;
    setCopReg(2, GTE_DREG(12), w0);
    setCopReg(2, GTE_DREG(13), w1);
    setCopReg(2, GTE_DREG(14), w2);
    cnt--;
    out = cur + delta;
    copFunction(2, 0x1400006u);                  /* NCLIP */
    DB32(out + 0x08) = w0;
    DB32(out + 0x14) = w1;
    DB32(out + 0x20) = w2;
    mac0 = (int32_t)getCopReg(2, GTE_DREG(24));
    if (mac0 > 0) {
      pa   = DB32(cur + 0x0c);
      attr = ((uint32_t)GP8(out + 7) << 24) | 0x00808080u;
      t2   = DB32(pa);
      setCopReg(2, GTE_DREG(6), attr);
      setCopReg(2, GTE_DREG(9),  t2 & 0xffffu);
      setCopReg(2, GTE_DREG(10), t2 >> 16);
      gte_lwc2(pa + 4, 11);
      gte_lwc2(p0 + 4, 17);
      copFunction(2, 0x138041cu);

      pb = DB32(cur + 0x10);
      gte_lwc2(p1 + 4, 18);
      t2 = DB32(pb);
      DB32(out + 4) = (uint32_t)getCopReg(2, GTE_DREG(22));
      setCopReg(2, GTE_DREG(9),  t2 & 0xffffu);
      setCopReg(2, GTE_DREG(10), t2 >> 16);
      gte_lwc2(pb + 4, 11);
      copFunction(2, 0x138041cu);

      pc = DB32(cur + 0x14);
      gte_lwc2(p2 + 4, 19);
      t2 = DB32(pc);
      DB32(out + 0x10) = (uint32_t)getCopReg(2, GTE_DREG(22));
      copFunction(2, 0x158002du);                /* AVSZ3 */
      setCopReg(2, GTE_DREG(9),  t2 & 0xffffu);
      setCopReg(2, GTE_DREG(10), t2 >> 16);
      gte_lwc2(pc + 4, 11);
      otz = (uint32_t)getCopReg(2, GTE_DREG(7));
      copFunction(2, 0x138041cu);
      prim_ot_link_indexed(ot, out, otz);
      DB32(out + 0x1c) = (uint32_t)getCopReg(2, GTE_DREG(22));
    }
    cur += 0x68;
  } while (cnt > 0);
  return cur;
}

extern int psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);

/* prim_persp_ot_emit — guest 0x8018B76C (hand-written leaf; args arrive in
 * a0/a1/a3 plus hidden temps $t4/$t7/$t8 — see ARG_GPRS). Per record of
 * `in_stride` bytes at `cur`: lwc2 V0..V2 (regs 0-5) <- cur[0..5], RTPT,
 * NCLIP; when the PREVIOUS iteration's FLAG (cfc2 ctrl31 read before this
 * NCLIP — faithful stale-by-one ordering) has no bits under 0x7FC40000 and
 * MAC0 > 0, emit four SXY words into the out record at cur+delta
 * (+8,+8+out_stride,+8+2*out_stride,+8+3*out_stride), RTPS the secondary
 * vector cur[6..7], accumulate the four screen-cull tests (sx via
 * psx_ws_cull_sltiu, sy via sltiu 480), AVSZ4 depth-check OTZ >= 2048, then
 * swl-link the OT entry. do-while on signed count; returns final cur.
 * Disasm generated/SLES_017.22_full_41.c 0x8018B76C-0x8018B890. */
uint32_t prim_persp_ot_emit(uint32_t cur, uint32_t ot, uint32_t count,
                            uint32_t delta, uint32_t in_stride,
                            uint32_t out_stride)
{
  int32_t cnt = (int32_t)count;
  uint32_t t1, t2, flag, out, sxy0, sxy1, sxy2, mac0;
  uint32_t sx_ok, sy_ok, old, base;

  do {
    /* lwc2 gte[0..5] <- cur[0..5] : V0/V1/V2 packed xy+z (3 verts). */
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;                                       /* addiu a3,-1 */
    copFunction(2, 0x0280030u);                  /* RTPT */
    t1   = DB32(cur + 24);                       /* lw t1,0x18(a0) */
    t2   = DB32(cur + 28);                       /* lw t2,0x1C(a0) */
    flag = (uint32_t)getCopControlWord(2, GTE_CREG(31)); /* cfc2 FLAG */
    copFunction(2, 0x1400006u);                  /* NCLIP */
    base = cur + delta;                          /* t3 = a0 + t4 */
    if ((flag & 0x7FC40000u) != 0) goto next;

    out = base + 8;                              /* addiu t3,8 */
    setCopReg(2, GTE_DREG(0), t1);               /* mtc2 V0.xy */
    setCopReg(2, GTE_DREG(1), t2);               /* mtc2 V0.z  */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24)); /* mfc2 MAC0 */
    sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12)); /* mfc2 SXY0 */
    DB32(out) = sxy0;   /* sw SXY0,(out) is the blez MAC0 DELAY SLOT —
                         * unconditional even when mac0 <= 0. */
    if ((int32_t)mac0 <= 0) goto next;
    sxy1 = (uint32_t)getCopReg(2, GTE_DREG(13)); /* mfc2 SXY1 */
    sxy2 = (uint32_t)getCopReg(2, GTE_DREG(14)); /* mfc2 SXY2 */
    out += out_stride; DB32(out) = sxy1;
    out += out_stride; DB32(out) = sxy2;
    copFunction(2, 0x180001u);                   /* RTPS: V0 -> SXY0 */

    /* Cull accumulation — same ops/order as the guest: sx tests ride the
     * widescreen-aware sltiu helper, sy tests are plain sltiu 480 on the
     * arithmetic-shifted high half. */
    sx_ok = (uint32_t)psx_ws_cull_sltiu(sxy0 & 0xFFFFu, 512u);
    sy_ok = ((uint32_t)((int32_t)sxy0 >> 16) < 480u) ? 1u : 0u;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy1 & 0xFFFFu, 512u);
    sy_ok += ((uint32_t)((int32_t)sxy1 >> 16) < 480u) ? 1u : 0u;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy2 & 0xFFFFu, 512u);
    sy_ok += ((uint32_t)((int32_t)sxy2 >> 16) < 480u) ? 1u : 0u;
    sxy2 = (uint32_t)getCopReg(2, GTE_DREG(14)); /* post-RTPS SXY2 */
    out += out_stride; DB32(out) = sxy2;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy2 & 0xFFFFu, 512u);
    if (sx_ok == 0) goto next;
    sy_ok += ((uint32_t)((int32_t)sxy2 >> 16) < 480u) ? 1u : 0u;
    if (sy_ok == 0) goto next;                   /* t3=a0+t4 recompute: dead */

    copFunction(2, 0x168002Eu);                  /* AVSZ4 -> OTZ */
    if ((int32_t)(uint32_t)getCopReg(2, GTE_DREG(7)) < 2048) goto next;

    /* OT link tail: t3 was recomputed to base (a0+t4) in the branch delay
     * slot, so the swl targets are base+2 / ot+2. */
    old = DB32(ot);                              /* lw t0,(a1) */
    swl_guest(ot + 2, base << 8);                /* swl (t3<<8),2(a1) */
    swl_guest(base + 2, old << 8);               /* swl (t0<<8),2(t3) */

  next:
    cur += in_stride;                           /* delay-slot a0 += t7 */
  } while (cnt > 0);                             /* bgtz a3 -> loop */
  return cur;                                   /* move v0,a0 */
}

/* prim_persp_ot_emit_112 — guest 0x8018B98C (heap_serialize tag 0x40051
 * mode-19 emit variant; sibling of prim_persp_ot_emit above). Identical
 * RTPT -> cfc2-FLAG -> NCLIP -> conditional RTPS -> SXY0..2 store ->
 * four-way screen cull -> AVSZ4/OTZ depth check -> swl OT link pipeline,
 * specialized to fixed geometry: in-stride 112 (guest $t7), out-stride 8
 * (guest $t8), and the out-record delta is 32 when a2==0 else 72
 * (guest $t4 = 32 + (a2 ? 40 : 0)). Signed count in a3; returns the
 * advanced input cursor in v0 (early-outs with the untouched a0 when
 * count <= 0). Disasm generated/SLES_017.22_full_41.c 0x8018B98C-0x8018BAB8. */
uint32_t prim_persp_ot_emit_112(uint32_t cur, uint32_t ot, uint32_t sel,
                                int32_t count)
{
  int32_t cnt = count;
  uint32_t delta, t1, t2, flag, out, sxy0, sxy1, sxy2, mac0;
  uint32_t sx_ok, sy_ok, old, base;

  if (cnt <= 0) return cur;                     /* blez a3 -> epilogue */
  delta = (sel == 0) ? 32u : 72u;               /* t4 = 32 (+40 if a2) */

  do {
    /* lwc2 gte[0..5] <- cur[0..5] : V0/V1/V2 packed xy+z (3 verts). */
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;                                       /* addiu a3,-1 */
    copFunction(2, 0x0280030u);                  /* RTPT */
    t1   = DB32(cur + 24);                       /* lw t1,0x18(a0) */
    t2   = DB32(cur + 28);                       /* lw t2,0x1C(a0) */
    flag = (uint32_t)getCopControlWord(2, GTE_CREG(31)); /* cfc2 FLAG */
    copFunction(2, 0x1400006u);                  /* NCLIP */
    base = cur + delta;                          /* t3 = a0 + t4 */
    if ((flag & 0x7FC40000u) != 0) goto next;

    out = base + 8;                              /* addiu t3,8 */
    setCopReg(2, GTE_DREG(0), t1);               /* mtc2 V0.xy */
    setCopReg(2, GTE_DREG(1), t2);               /* mtc2 V0.z  */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24)); /* mfc2 MAC0 */
    sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12)); /* mfc2 SXY0 */
    DB32(out) = sxy0;   /* sw SXY0,(out) is the blez MAC0 DELAY SLOT —
                         * unconditional even when mac0 <= 0. */
    if ((int32_t)mac0 <= 0) goto next;
    sxy1 = (uint32_t)getCopReg(2, GTE_DREG(13)); /* mfc2 SXY1 */
    sxy2 = (uint32_t)getCopReg(2, GTE_DREG(14)); /* mfc2 SXY2 */
    out += 8; DB32(out) = sxy1;
    out += 8; DB32(out) = sxy2;
    copFunction(2, 0x180001u);                   /* RTPS: V0 -> SXY0 */

    /* Cull accumulation — identical op order to the sibling emit:
     * sx tests ride the widescreen-aware sltiu helper, sy tests are
     * plain sltiu 480 on the arithmetic-shifted high half. */
    sx_ok = (uint32_t)psx_ws_cull_sltiu(sxy0 & 0xFFFFu, 512u);
    sy_ok = ((uint32_t)((int32_t)sxy0 >> 16) < 480u) ? 1u : 0u;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy1 & 0xFFFFu, 512u);
    sy_ok += ((uint32_t)((int32_t)sxy1 >> 16) < 480u) ? 1u : 0u;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy2 & 0xFFFFu, 512u);
    sy_ok += ((uint32_t)((int32_t)sxy2 >> 16) < 480u) ? 1u : 0u;
    sxy2 = (uint32_t)getCopReg(2, GTE_DREG(14)); /* post-RTPS SXY2 */
    out += 8; DB32(out) = sxy2;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy2 & 0xFFFFu, 512u);
    if (sx_ok == 0) goto next;
    sy_ok += ((uint32_t)((int32_t)sxy2 >> 16) < 480u) ? 1u : 0u;
    if (sy_ok == 0) goto next;                   /* t3=a0+t4 recompute: dead */

    copFunction(2, 0x168002Eu);                  /* AVSZ4 -> OTZ */
    if ((int32_t)(uint32_t)getCopReg(2, GTE_DREG(7)) < 2048) goto next;

    /* OT link tail: t3 was recomputed to base (a0+t4) in the branch delay
     * slot, so the swl targets are base+2 / ot+2. */
    old = DB32(ot);                              /* lw t0,(a1) */
    swl_guest(ot + 2, base << 8);                /* swl (t3<<8),2(a1) */
    swl_guest(base + 2, old << 8);               /* swl (t0<<8),2(t3) */

  next:
    cur += 112;                                  /* delay-slot a0 += t7 */
  } while (cnt > 0);                             /* bgtz a3 -> loop */
  return cur;                                   /* move v0,a0 */
}

/* prim_emit_tail_advance — guest 0x8018B884: shared loop tail of the
 * prim_persp_ot_emit family (the generated body is a CPS split piece —
 * see br2eu_func_8018B884 "fallthrough to split piece"). Semantics:
 * a0 += $t7 unconditionally (delay slot), then if a3 > 0 re-enter the
 * emit loop head 0x8018B76C with the live cursor/OT/count/delta/strides,
 * else fall into the 0x8018B88C epilogue (v0 = a0, jr $ra). Ported as a
 * tail call into prim_persp_ot_emit — identical observable behavior:
 * the callee consumes the advanced cursor and returns the final one.
 * Args arrive as (a0=cur, a1=ot, a3=count, $t4=delta, $t7=in_stride,
 * $t8=out_stride) — same register contract as prim_persp_ot_emit. */
uint32_t prim_emit_tail_advance(uint32_t cur, uint32_t ot, int32_t count,
                                uint32_t delta, uint32_t in_stride,
                                uint32_t out_stride)
{
  cur += in_stride;                             /* delay-slot a0 += t7 */
  if (count > 0)
    return prim_persp_ot_emit(cur, ot, (uint32_t)count, delta,
                              in_stride, out_stride);
  return cur;                                   /* move v0,a0 */
}

/* prim_depth_shade_emit — guest 0x8018A9E8 (heap_serialize tag 0x40051
 * mode-7 emit variant; alias entry into the 0x8018A9C8 body). Same
 * RTPT -> cfc2-FLAG -> NCLIP -> conditional RTPS -> SXY0..2 store ->
 * screen-cull pipeline as prim_persp_ot_emit_112, specialized to
 * in-stride 112 / out-stride 8 / delta 32|72, but the emit tail differs:
 * instead of the plain OTZ>=2048 reject it runs AVSZ4 then computes a
 * depth-shaded attribute word for base+12 from OTZ (GTE reg 7):
 *   z < 256          -> attr = mem & ~0x03C00000
 *   256 <= z < 1280  -> attr = (mem & ~0x03C00000) | (((z-256)<<16) & 0x03C00000)
 *   z >= 1280        -> attr = mem | 0x03C00000
 * then the usual swl OT link. Also wraps the loop in CTC2 ctrl30 writes:
 * 256 before iterating, 1024 on normal exit (skipped on count<=0 early
 * out). Disasm generated/SLES_017.22_full_41.c 0x8018A9E8-0x8018AB84. */
uint32_t prim_depth_shade_emit(uint32_t cur, uint32_t ot, uint32_t sel,
                               int32_t count)
{
  int32_t cnt = count;
  uint32_t delta, t1, t2, flag, out, sxy0, sxy1, sxy2, mac0;
  uint32_t sx_ok, sy_ok, old, base, z, mem, attr;

  if (cnt <= 0) return cur;                     /* blez a3 -> epilogue */
  setCopControlWord(2, GTE_CREG(30), 256u);     /* ctc2 ctrl30 = 256 */
  delta = (sel == 0) ? 32u : 72u;               /* t4 = 32 (+40 if a2) */

  do {
    /* lwc2 gte[0..5] <- cur[0..5] : V0/V1/V2 packed xy+z (3 verts). */
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;                                       /* addiu a3,-1 */
    copFunction(2, 0x0280030u);                  /* RTPT */
    t1   = DB32(cur + 24);                       /* lw t1,0x18(a0) */
    t2   = DB32(cur + 28);                       /* lw t2,0x1C(a0) */
    flag = (uint32_t)getCopControlWord(2, GTE_CREG(31)); /* cfc2 FLAG */
    copFunction(2, 0x1400006u);                  /* NCLIP */
    /* mtc2 gte[0]=t1 sits in the flag-branch DELAY SLOT — unconditional. */
    setCopReg(2, GTE_DREG(0), t1);               /* mtc2 V0.xy */
    if ((flag & 0x7FC40000u) != 0) goto next;

    setCopReg(2, GTE_DREG(1), t2);               /* mtc2 V0.z  */
    base = cur + delta;                          /* t3 = a0 + t4 */
    out  = base + 8;                             /* addiu t3,8 */
    mac0 = (uint32_t)getCopReg(2, GTE_DREG(24)); /* mfc2 MAC0 */
    sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12)); /* mfc2 SXY0 */
    DB32(out) = sxy0;   /* sw SXY0,(out) is the blez MAC0 DELAY SLOT —
                         * unconditional even when mac0 <= 0. */
    if ((int32_t)mac0 <= 0) goto next;
    sxy1 = (uint32_t)getCopReg(2, GTE_DREG(13)); /* mfc2 SXY1 */
    sxy2 = (uint32_t)getCopReg(2, GTE_DREG(14)); /* mfc2 SXY2 */
    out += 8; DB32(out) = sxy1;
    out += 8; DB32(out) = sxy2;
    copFunction(2, 0x180001u);                   /* RTPS: V0 -> SXY0 */

    /* Cull accumulation — identical op order to the sibling emit:
     * sx tests ride the widescreen-aware sltiu helper, sy tests are
     * plain sltiu 480 on the arithmetic-shifted high half. */
    sx_ok = (uint32_t)psx_ws_cull_sltiu(sxy0 & 0xFFFFu, 512u);
    sy_ok = ((uint32_t)((int32_t)sxy0 >> 16) < 480u) ? 1u : 0u;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy1 & 0xFFFFu, 512u);
    sy_ok += ((uint32_t)((int32_t)sxy1 >> 16) < 480u) ? 1u : 0u;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy2 & 0xFFFFu, 512u);
    sy_ok += ((uint32_t)((int32_t)sxy2 >> 16) < 480u) ? 1u : 0u;
    sxy2 = (uint32_t)getCopReg(2, GTE_DREG(14)); /* post-RTPS SXY2 */
    out += 8; DB32(out) = sxy2;
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy2 & 0xFFFFu, 512u);
    if (sx_ok == 0) goto next;
    sy_ok += ((uint32_t)((int32_t)sxy2 >> 16) < 480u) ? 1u : 0u;
    if (sy_ok == 0) goto next;                   /* t3=a0+t4 recompute: dead */

    copFunction(2, 0x168002Eu);                  /* AVSZ4 -> OTZ */

    /* Depth-shade attribute word at base+12 (lw t2,0xC(t3) — t3 was
     * recomputed to base in the sy-branch delay slot). */
    base = cur + delta;
    mem  = DB32(base + 12);
    z    = (uint32_t)getCopReg(2, GTE_DREG(7));  /* mfc2 OTZ (16-bit) */
    if (z < 256u) {
      attr = mem & ~0x03C00000u;
    } else if (z - 256u < 1024u) {
      attr = (mem & ~0x03C00000u) |
             (((z - 256u) << 16) & 0x03C00000u);
    } else {
      attr = mem | 0x03C00000u;
    }
    DB32(base + 12) = attr;

    /* OT link tail: identical swl pair to the sibling emit. */
    old = DB32(ot);                              /* lw t0,(a1) */
    swl_guest(ot + 2, base << 8);                /* swl (t3<<8),2(a1) */
    swl_guest(base + 2, old << 8);               /* swl (t0<<8),2(t3) */

  next:
    cur += 112;                                  /* delay-slot a0 += t7 */
  } while (cnt > 0);                             /* bgtz a3 -> loop */
  setCopControlWord(2, GTE_CREG(30), 1024u);    /* ctc2 ctrl30 = 1024 */
  return cur;                                   /* move v0,a0 */
}

/* ======== 0x8018ab84 / 0x8018ac54 family ======== */

/* prim_cull3_emit — shared body of the five-entry 0x8018AC54 stub family
 * (heap emit variants; each stub only loads $s6/$s7/$t8 then falls into
 * the common loop). Per record of `in_stride` bytes at `cur`: lwc2
 * V0..V2 (regs 0-5) <- cur[0..5], RTPT, then the SAME post-RTPT FLAG gate
 * as the prim_persp_ot_emit family — cfc2 ctrl31 & 0x7FC40000 rejects —
 * but NO NCLIP, NO MAC0 check, NO RTPS secondary vector, NO AVSZ depth
 * test. On pass it reads SXY0..2 (GTE data 12-14) and accumulates the
 * three screen culls independently: sx_ok = sum of
 * psx_ws_cull_sltiu(sxyN & 0xFFFF, 512), sy_ok = sum of
 * ((int32)sxyN >> 16) < 480. Emit requires sx_ok > 0 AND sy_ok > 0.
 * SXY0 is stored at out+0 unconditionally once sx_ok > 0 (it sits in the
 * sy-branch DELAY SLOT); SXY1/SXY2 land at out+out_stride /
 * out+2*out_stride only when sy_ok > 0. The OT link uses `ot` DIRECTLY
 * as the slot (no OTZ indexing — a1 is already the resolved slot):
 * old = *ot; swl ot+2 <- out_base<<8; swl out_base+2 <- old<<8.
 * out_base = cur + delta where delta = 24 (+ d_extra when sel != 0).
 * Signed count in a3; returns advanced cursor. Disasm
 * generated/SLES_017.22_full_41.c 0x8018AC54-0x8018AD80. */
static uint32_t prim_cull3_emit(uint32_t cur, uint32_t ot, uint32_t sel,
                                int32_t count, uint32_t d_extra,
                                uint32_t in_stride, uint32_t out_stride)
{
  int32_t cnt = count;
  uint32_t delta, base, out, sxy0, sxy1, sxy2;
  uint32_t sx_ok, sy_ok, old, flag;

  if (cnt <= 0) return cur;                     /* blez a3 -> epilogue */
  delta = (sel == 0) ? 24u : 24u + d_extra;     /* t4 = 24 (+s6 if a2) */

  do {
    /* lwc2 gte[0..5] <- cur[0..5] : V0/V1/V2 packed xy+z (3 verts). */
    gte_lwc2(cur +  0, 0);
    gte_lwc2(cur +  4, 1);
    gte_lwc2(cur +  8, 2);
    gte_lwc2(cur + 12, 3);
    gte_lwc2(cur + 16, 4);
    gte_lwc2(cur + 20, 5);
    cnt--;                                       /* addiu a3,-1 */
    copFunction(2, 0x0280030u);                  /* RTPT */
    flag = (uint32_t)getCopControlWord(2, GTE_CREG(31)); /* cfc2 FLAG */
    base = cur + delta;                          /* t3 = a0 + t4 */
    if ((flag & 0x7FC40000u) != 0) goto next;

    sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12)); /* mfc2 SXY0 */
    sxy1 = (uint32_t)getCopReg(2, GTE_DREG(13)); /* mfc2 SXY1 */
    sxy2 = (uint32_t)getCopReg(2, GTE_DREG(14)); /* mfc2 SXY2 */
    sx_ok  = (uint32_t)psx_ws_cull_sltiu(sxy0 & 0xFFFFu, 512u);
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy1 & 0xFFFFu, 512u);
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy2 & 0xFFFFu, 512u);
    if (sx_ok == 0) goto next;

    /* sy0 is computed in the sx-branch DELAY SLOT — live whenever the
     * branch is not taken. */
    sy_ok  = (((uint32_t)((int32_t)sxy0 >> 16)) < 480u) ? 1u : 0u;
    sy_ok += (((uint32_t)((int32_t)sxy1 >> 16)) < 480u) ? 1u : 0u;
    sy_ok += (((uint32_t)((int32_t)sxy2 >> 16)) < 480u) ? 1u : 0u;
    DB32(base) = sxy0;   /* sw SXY0,(t3) is the sy-cull DELAY SLOT —
                          * unconditional once sx_ok > 0. */
    if (sy_ok == 0) goto next;

    out = base + out_stride; DB32(out) = sxy1;
    out += out_stride;      DB32(out) = sxy2;

    /* OT link tail: t3 recomputed to base (a0+t4) in the guest, so the
     * swl targets are base+2 / ot+2 with ot used directly as the slot. */
    old = DB32(ot);                              /* lw t0,(a1) */
    swl_guest(ot + 2, base << 8);                /* swl (t3<<8),2(a1) */
    swl_guest(base + 2, old << 8);               /* swl (t0<<8),2(t3) */

  next:
    cur += in_stride;                           /* delay-slot a0 += s7 */
  } while (cnt > 0);                             /* bgtz a3 -> loop */
  return cur;                                   /* move v0,a0 */
}

/* The five stubs differ only in $s6/$s7/$t8 (d_extra/in_stride/out_stride):
 *   0x8018AC54: s6=24 s7=72  t8=4    0x8018AC74: s6=28 s7=80  t8=8
 *   0x8018AC64: s6=20 s7=64  t8=4    0x8018AC84: s6=32 s7=88  t8=8
 *   0x8018AC94: s6=40 s7=104 t8=12 */
uint32_t prim_cull3_emit_48(uint32_t cur, uint32_t ot, uint32_t sel,
                            int32_t count)
{ return prim_cull3_emit(cur, ot, sel, count, 24u, 72u, 4u); }
uint32_t prim_cull3_emit_40(uint32_t cur, uint32_t ot, uint32_t sel,
                            int32_t count)
{ return prim_cull3_emit(cur, ot, sel, count, 20u, 64u, 4u); }
uint32_t prim_cull3_emit_50(uint32_t cur, uint32_t ot, uint32_t sel,
                            int32_t count)
{ return prim_cull3_emit(cur, ot, sel, count, 28u, 80u, 8u); }
uint32_t prim_cull3_emit_58(uint32_t cur, uint32_t ot, uint32_t sel,
                            int32_t count)
{ return prim_cull3_emit(cur, ot, sel, count, 32u, 88u, 8u); }
uint32_t prim_cull3_emit_68(uint32_t cur, uint32_t ot, uint32_t sel,
                            int32_t count)
{ return prim_cull3_emit(cur, ot, sel, count, 40u, 104u, 12u); }

/* prim_cull2_emit_30 — guest 0x8018AB84: the two-vertex sibling of the
 * prim_cull3_emit family (fixed geometry: in-stride 48 = 0x30, out-stride
 * 4, delta = 16 when sel==0 else 32 — the guest loads t4=16 then adds
 * $s6=16 on the sel!=0 path). Input setup differs: V0.xy/V0.z come from
 * plain `lw` of cur[0]/cur[4] written via mtc2, V1.xy/V1.z via lwc2 of
 * cur[8]/cur[12], and V2 is an exact copy of V0 (mtc2 of the same two
 * loaded words). Same RTPT -> cfc2 FLAG & 0x7FC40000 gate -> two-way SXY
 * screen cull (sx_ok/sy_ok over SXY0/SXY1 only) -> SXY0 store (delay
 * slot, unconditional once sx_ok>0) + SXY1 at base+4 + direct-slot OT
 * link. Signed count in a3; returns advanced cursor. Disasm
 * generated/SLES_017.22_full_41.c 0x8018AB84-0x8018AC54. */
uint32_t prim_cull2_emit_30(uint32_t cur, uint32_t ot, uint32_t sel,
                            int32_t count)
{
  int32_t cnt = count;
  uint32_t delta, base, sxy0, sxy1;
  uint32_t sx_ok, sy_ok, old, flag, w0, w4;

  if (cnt <= 0) return cur;                     /* blez a3 -> epilogue */
  delta = (sel == 0) ? 16u : 32u;               /* t4 = 16 (+s6=16 if a2) */

  do {
    w0 = DB32(cur + 0);                         /* lw t0,(a0)   */
    w4 = DB32(cur + 4);                         /* lw t1,4(a0)   */
    setCopReg(2, GTE_DREG(0), w0);              /* mtc2 V0.xy    */
    setCopReg(2, GTE_DREG(1), w4);              /* mtc2 V0.z     */
    gte_lwc2(cur + 8, 2);                       /* lwc2 V1.xy    */
    gte_lwc2(cur + 12, 3);                      /* lwc2 V1.z     */
    setCopReg(2, GTE_DREG(4), w0);              /* mtc2 V2.xy = V0.xy */
    setCopReg(2, GTE_DREG(5), w4);              /* mtc2 V2.z  = V0.z  */
    cnt--;                                       /* addiu a3,-1 */
    copFunction(2, 0x0280030u);                  /* RTPT */
    flag = (uint32_t)getCopControlWord(2, GTE_CREG(31)); /* cfc2 FLAG */
    base = cur + delta;                          /* t3 = a0 + t4 */
    if ((flag & 0x7FC40000u) != 0) goto next;

    sxy0 = (uint32_t)getCopReg(2, GTE_DREG(12)); /* mfc2 SXY0 */
    sxy1 = (uint32_t)getCopReg(2, GTE_DREG(13)); /* mfc2 SXY1 */
    sx_ok  = (uint32_t)psx_ws_cull_sltiu(sxy0 & 0xFFFFu, 512u);
    sx_ok += (uint32_t)psx_ws_cull_sltiu(sxy1 & 0xFFFFu, 512u);
    if (sx_ok == 0) goto next;

    sy_ok  = (((uint32_t)((int32_t)sxy0 >> 16)) < 480u) ? 1u : 0u;
    sy_ok += (((uint32_t)((int32_t)sxy1 >> 16)) < 480u) ? 1u : 0u;
    DB32(base) = sxy0;   /* sw SXY0,(t3) is the sy-cull DELAY SLOT. */
    if (sy_ok == 0) goto next;

    DB32(base + 4) = sxy1;                       /* t3 += t8(4); sw */

    /* OT link tail: direct slot (a1), same swl pair as the cull3 family. */
    old = DB32(ot);                              /* lw t0,(a1) */
    swl_guest(ot + 2, base << 8);                /* swl (t3<<8),2(a1) */
    swl_guest(base + 2, old << 8);               /* swl (t0<<8),2(t3) */

  next:
    cur += 48;                                   /* delay-slot a0 += s7 */
  } while (cnt > 0);                             /* bgtz a3 -> loop */
  return cur;                                   /* move v0,a0 */
}

/* ======== 0x801864dc ======== */

/* part_skin_emit — skinned part renderer / OT primitive emitter.
 *
 * a0 = part record, a1 = skin/pose source, a2 = out vertex block,
 * a3 = scratch. Builds scratchpad work area at 0x1F800000+0x38C
 * (u16 triples: transformed xy + packed attr), transforms the part's
 * rtps point set via rtps_point_xform2, derives per-axis deltas with
 * (prod + rounding)>>8 fixed-point math, angle-dispatches on the s16
 * mode word at a1+2 (<6 / ==6 / ==9 / other) through atan2_lut +
 * lut_cos/sin_q12 to fold hinge angles into the scratchpad angles,
 * then runs part_matrix_compute + gte_rtps_xform1/xform3 to emit the
 * final transformed vertices back through a0/a1. Hot path (~2.5k calls).
 * Disasm 0x801864dc-0x8018694c. */

extern int *rtps_point_xform2(uint32_t *, uint32_t *, int *);
extern uint16_t *gte_rtps_xform1(uint32_t *, uint32_t *, uint16_t *);
extern uint32_t *gte_rtps_xform3(uint32_t *, uint16_t *, uint32_t *);
extern void part_matrix_compute(uint32_t *, int8_t *, short *, uint32_t);
extern int atan2_lut(int, int);
extern int lut_cos_q12(int);
extern int lut_sin_q12(uint32_t);

static void psk_mult_lo(CPUState *c, uint32_t x, uint32_t y)
{
  int64_t r = (int64_t)(int32_t)x * (int64_t)(int32_t)y;
  c->lo = (uint32_t)r;
  c->hi = (uint32_t)((uint64_t)r >> 32);
}

uint32_t part_skin_emit(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *c = clean_cpu_state();
  uint32_t g[32];
  uint32_t fr;
  int i;

  if (!c) return 0;
  for (i = 0; i < 32; i++) g[i] = c->gpr[i];
  g[0] = 0;
  g[4] = a0; g[5] = a1; g[6] = a2; g[7] = a3;
  fr = br2_guest_frame(0x70);

  /* ---- prologue: save s0-s7/ra/fp, s2 = scratchpad base ---- */
  br2_st32(fr + 96, g[22]);              /* sw s6 saved-incoming */
  g[22] = g[5];                          /* s6 = a1 */
  br2_st32(fr + 100, g[23]);             /* sw s7 saved-incoming */
  g[23] = g[6];                          /* s7 = a2 */
  br2_st32(fr + 80, g[18]);              /* sw s2 saved-incoming */
  g[18] = 0x1F800000u;
  g[5] = g[22] + 24;
  g[6] = g[22] + 52;
  br2_st32(fr + 108, g[31]);             /* ra */
  br2_st32(fr + 104, g[30]);             /* fp */
  br2_st32(fr + 92, g[21]);
  br2_st32(fr + 88, g[20]);
  br2_st32(fr + 84, g[19]);
  br2_st32(fr + 76, g[17]);
  br2_st32(fr + 72, g[16]);
  br2_st32(fr + 112, g[4]);              /* a0 */
  br2_st32(fr + 124, g[7]);              /* a3 */
  g[8] = br2_ld32(g[22] + 64);
  br2_st32(fr + 68, g[8]);
  g[21] = g[23] + 28;
  g[16] = br2_ld32(g[22] + 68);
  g[2] = br2_ld16(g[22] + 52);           /* lhu */
  g[30] = g[8] + 32;
  br2_st16(g[23] + 8, (uint16_t)g[2]);
  g[2] = br2_ld16(g[22] + 56);
  br2_st16(g[23] + 10, (uint16_t)g[2]);
  g[2] = br2_ld16(g[22] + 60);
  g[8] = g[23] + (uint32_t)-28;
  br2_st16(g[23] + 12, (uint16_t)g[2]);
  br2_st32(fr + 64, g[8]);
  g[20] = br2_ld32(g[23] + 24);

  /* jal 0x80186564 -> rtps_point_xform2(a0=g30, a1=a1+24, a2=a1+52) */
  g[4] = g[30];
  g[2] = (uint32_t)guest_of(rtps_point_xform2(
      (uint32_t *)guest_at(g[4]),
      (uint32_t *)guest_at(g[5]),
      (int *)guest_at(g[6])));

L_80186568:
  g[2] = br2_ld16(g[20]);                /* lhu */
  g[18] |= 0x38Cu;                       /* delay slot: s2 += 0x38C */
  g[2] &= 0x4000u;
  if (g[2] != 0) goto L_8018691C;        /* flag set -> epilogue */

  /* ---- axis delta pass 1: x (mult by lh(g20+4)) ---- */
  g[3] = (int32_t)(int16_t)br2_ld16(g[21] + 8);   /* lh */
  g[2] = br2_ld32(g[16] + 52);
  g[17] = (int32_t)(int16_t)br2_ld16(g[20] + 4);  /* lh */
  g[2] = g[2] - g[3];
  psk_mult_lo(c, g[2], g[17]);
  g[19] = (int32_t)(int16_t)br2_ld16(g[20] + 6);  /* lh */
  g[2] = c->lo;
  g[4] = (int32_t)g[2] >> 8;             /* delay slot sra */
  if ((int32_t)g[2] < 0) {               /* bgez not taken -> round up */
    g[2] += 0xFFu;
    g[4] = (int32_t)g[2] >> 8;
  }
  g[2] = br2_ld16(g[16] + 52);           /* lhu */
  g[3] = br2_ld16(g[22] + 52);           /* lhu */
  g[2] = g[2] + g[4];
  g[2] = g[2] - g[3];
  br2_st16(g[18], (uint16_t)g[2]);       /* sh s2+0 */
  g[3] = (int32_t)(int16_t)br2_ld16(g[21] + 10);  /* lh */
  g[2] = br2_ld32(g[16] + 56);
  /* 0x801865c4: nop */
  g[2] = g[2] - g[3];
  psk_mult_lo(c, g[2], g[17]);
  g[3] = c->lo;
  g[2] = g[19] << 2;                     /* delay slot */
  if ((int32_t)g[3] < 0) g[3] += 0xFFu;  /* bgez not taken */
  /* L_801865e0 */
  g[2] = g[2] + g[19];
  g[2] = g[2] << 3;
  g[2] = g[2] + g[19];
  g[4] = (int32_t)g[3] >> 8;             /* delay slot sra */
  if ((int32_t)g[2] >= 0) goto L_801865F8;
  g[2] += 0xFFu;
L_801865F8:
  g[3] = (int32_t)g[2] >> 8;
  g[3] = g[4] + g[3];
  g[2] = br2_ld16(g[16] + 56);           /* lhu */
  g[4] = br2_ld16(g[22] + 56);           /* lhu */
  g[2] = g[2] + g[3];
  g[5] = g[2] - g[4];
  br2_st16(g[18] + 2, (uint16_t)g[5]);   /* sh s2+2 */

  /* ---- axis delta pass 2: z (mult by lh(g20+4)) ---- */
  g[3] = (int32_t)(int16_t)br2_ld16(g[21] + 12);  /* lh */
  g[2] = br2_ld32(g[16] + 60);
  /* 0x8018661c: nop */
  g[2] = g[2] - g[3];
  psk_mult_lo(c, g[2], g[17]);
  g[2] = c->lo;
  g[4] = (int32_t)g[2] >> 8;             /* delay slot sra */
  if ((int32_t)g[2] < 0) {
    g[2] += 0xFFu;
    g[4] = (int32_t)g[2] >> 8;
  }
  g[2] = br2_ld16(g[16] + 60);           /* lhu */
  g[3] = br2_ld16(g[22] + 60);           /* lhu */
  g[2] = g[2] + g[4];
  g[2] = g[2] - g[3];
  g[3] = g[5] << 16;                     /* s16 of stored delta */
  g[3] = (int32_t)g[3] >> 16;
  br2_st16(g[18] + 4, (uint16_t)g[2]);   /* sh s2+4 */
  g[8] = br2_ld32(fr + 112);             /* reload saved a0 */
  g[2] = br2_ld32(g[22] + 56);
  g[4] = (int32_t)(int16_t)br2_ld16(g[8] + 148);  /* lh */
  g[2] = g[2] + g[3];
  g[17] = g[2] - g[4];
  g[2] = g[5] - g[17];                   /* delay slot */
  if ((int32_t)g[17] > 0)                /* blez not taken */
    br2_st16(g[18] + 2, (uint16_t)g[2]);

  /* jal 0x80186680 -> prim_attr_repack(a0=g30, a1=0x1F800394) */
  g[4] = g[30];
  g[5] = 0x1F800000u;
  g[5] |= 0x394u;                        /* delay slot */
  g[2] = prim_attr_repack(g[4], g[5]);

  /* jal 0x8018669c -> gte_rtps_xform1(0x1F800394, 0x1F80038C, 0x1F80038C) */
  g[4] = 0x1F800000u;
  g[4] |= 0x394u;
  g[5] = 0x1F800000u;
  g[5] |= 0x38Cu;
  g[6] = 0x1F800000u;
  g[6] |= 0x38Cu;                        /* delay slot */
  g[2] = (uint32_t)guest_of(gte_rtps_xform1(
      (uint32_t *)guest_at(g[4]),
      (uint32_t *)guest_at(g[5]),
      (uint16_t *)guest_at(g[6])));

  /* ---- flag halfword @ a2-part +0: 0x8000 -> zero out a2+6 ---- */
  g[2] = (int32_t)(int16_t)br2_ld16(g[20]);       /* lh */
  /* 0x801866a8/0x801866b4: nops */
  g[2] &= 0x8000u;
  if (g[2] != 0) br2_st16(g[23] + 6, 0);          /* beqz not taken */

  /* ---- pack s16 triples: s2 area -> sp+48/56, then part_matrix ---- */
  g[8] = br2_ld32(fr + 64);               /* saved a2-28 */
  /* 0x801866c4: nop */
  g[2] = br2_ld16(g[8] + 6);              /* lhu */
  /* 0x801866cc: nop */
  br2_st16(g[23] + 6, (uint16_t)g[2]);

  g[3] = br2_ld16(g[20] + 8);             /* lhu */
  g[2] = br2_ld16(g[18]);                 /* lhu */
  g[4] = fr + 16;
  g[2] = g[2] + g[3];
  br2_st16(fr + 56, (uint16_t)g[3]);
  br2_st16(fr + 48, (uint16_t)g[2]);
  g[3] = br2_ld16(g[20] + 10);            /* lhu */
  g[2] = br2_ld16(g[18] + 2);             /* lhu */
  g[6] = fr + 56;
  g[2] = g[2] + g[3];
  br2_st16(fr + 58, (uint16_t)g[3]);
  br2_st16(fr + 50, (uint16_t)g[2]);
  g[3] = br2_ld16(g[20] + 12);            /* lhu */
  g[2] = br2_ld16(g[18] + 4);             /* lhu */
  g[8] = br2_ld32(fr + 124);              /* saved a3 */
  g[2] = g[2] + g[3];
  br2_st16(fr + 52, (uint16_t)g[2]);
  g[2] = g[8] << 5;                       /* a3 << 5 */
  br2_st16(fr + 60, (uint16_t)g[3]);
  g[7] = br2_ld16(g[23] + 6);             /* lhu */
  g[8] = br2_ld32(fr + 112);              /* saved a0 */
  g[7] = g[7] << 16;
  g[5] = br2_ld32(g[8] + 72);
  g[7] = (int32_t)g[7] >> 17;             /* s16 << 16 >> 17 = s16 >> 1 */
  /* jal 0x80186734 -> part_matrix_compute(sp+16, g5+g2, sp+56, g7) */
  g[5] = g[5] + g[2];                     /* delay slot */
  part_matrix_compute((uint32_t *)guest_at(g[4]),
                      (int8_t *)guest_at(g[5]),
                      (short *)guest_at(g[6]), g[7]);

L_8018673C:
  g[4] = fr + 16;
  g[16] = g[18] + 8;                      /* s2 + 8 */
  /* jal 0x80186744 -> prim_attr_repack(sp+16, s2+8) */
  g[5] = g[16];                           /* delay slot */
  g[2] = prim_attr_repack(g[4], g[5]);

L_8018674C:
  g[4] = g[16];                           /* s2 + 8 */
  g[5] = fr + 48;
  /* jal 0x80186754 -> gte_rtps_xform1(s2+8, sp+48, s2) */
  g[6] = g[18];                           /* delay slot */
  g[2] = (uint32_t)guest_of(gte_rtps_xform1(
      (uint32_t *)guest_at(g[4]),
      (uint32_t *)guest_at(g[5]),
      (uint16_t *)guest_at(g[6])));

  /* ---- mode dispatch on lh(a1+2) ---- */
  g[3] = (int32_t)(int16_t)br2_ld16(g[20] + 2);   /* lh */
  /* 0x80186760: nop */
  g[2] = ((int32_t)g[3] < 6) ? 1u : 0u;
  g[2] = 6;                               /* delay slot */
  if (((int32_t)g[3] < 6)) goto L_801868D8;
  /* beq g3,6 : delay slot g2 = 9 always executes */
  g[2] = 9;
  if (g[3] == 6) goto L_801867F8;
  if (g[3] != 9) goto L_80186870;          /* _bc_80186778 */

  /* ================= mode == 9 ================= */
  /* atan2 pair #1: (s16)lhu(s2+2), lh(s2+0) */
  g[4] = br2_ld16(g[18] + 2);             /* lhu */
  g[5] = (int32_t)(int16_t)br2_ld16(g[18]);       /* lh */
  g[4] = g[4] << 16;
  /* jal atan2_lut, delay: g4 = (s32)g4 >> 16 */
  g[4] = (int32_t)g[4] >> 16;
  g[2] = (uint32_t)atan2_lut((int)g[4], (int)g[5]);
L_80186794:
  g[17] = g[2];
  /* atan2 pair #2: (s16)lhu(s2+2), lh(s2+4) */
  g[4] = br2_ld16(g[18] + 2);
  g[5] = (int32_t)(int16_t)br2_ld16(g[18] + 4);
  g[4] = g[4] << 16;
  g[4] = (int32_t)g[4] >> 16;             /* delay slot */
  g[2] = (uint32_t)atan2_lut((int)g[4], (int)g[5]);
L_801867AC:
  g[3] = (int32_t)(int16_t)br2_ld16(g[20] + 16);  /* lh */
  g[19] = g[2];
  g[2] = ((int32_t)g[3] < (int32_t)g[17]) ? 1u : 0u;
  if (g[2] != 0) goto L_801867D4;          /* _bc_801867B8 taken */
  g[3] = (int32_t)(int16_t)br2_ld16(g[20] + 14);  /* lh */
  /* 0x801867c4: nop */
  g[2] = ((int32_t)g[17] < (int32_t)g[3]) ? 1u : 0u;
  if (g[2] == 0) goto L_801867D8;          /* _bc_801867CC taken */
L_801867D4:
  g[17] = g[3];
L_801867D8:
  g[3] = (int32_t)(int16_t)br2_ld16(g[20] + 20);  /* lh */
  /* 0x801867dc: nop */
  g[2] = ((int32_t)g[3] < (int32_t)g[19]) ? 1u : 0u;
  if (g[2] != 0) goto L_8018686C;          /* _bc_801867E4 taken */
  g[3] = (int32_t)(int16_t)br2_ld16(g[20] + 18);  /* lh */
  /* 0x801867f0: j 0x80186864 ; delay slot slti */
  g[2] = ((int32_t)g[19] < (int32_t)g[3]) ? 1u : 0u;
  goto L_80186864;

  /* ================= mode == 6 ================= */
L_801867F8:
  g[4] = br2_ld16(g[18] + 2);             /* lhu */
  g[5] = (int32_t)(int16_t)br2_ld16(g[18]);       /* lh */
  g[4] = g[4] << 16;
  g[4] = (int32_t)g[4] >> 16;             /* delay slot */
  g[2] = (uint32_t)atan2_lut((int)g[4], (int)g[5]);
L_8018680C:
  g[17] = g[2];
  g[4] = br2_ld16(g[18] + 2);
  g[5] = (int32_t)(int16_t)br2_ld16(g[18] + 4);
  g[4] = g[4] << 16;
  g[4] = (int32_t)g[4] >> 16;             /* delay slot */
  g[2] = (uint32_t)atan2_lut((int)g[4], (int)g[5]);
L_80186824:
  g[19] = g[2];
  g[3] = (int32_t)(int16_t)br2_ld16(g[20] + 14);  /* lh */
  g[2] = ((int32_t)g[3] < (int32_t)g[17]) ? 1u : 0u;
  g[4] = g[0] - g[3];                     /* delay slot: g4 = -g3 */
  if (g[2] == 0) goto L_80186840;         /* _bc_80186830 taken */
  /* 0x80186838: */
  g[17] = g[3];
  goto L_80186850;                        /* j */
L_80186840:
  g[2] = ((int32_t)g[17] < (int32_t)g[4]) ? 1u : 0u;
  g[2] = ((int32_t)g[3] < (int32_t)g[19]) ? 1u : 0u;  /* delay slot */
  if (((int32_t)g[17] < (int32_t)g[4]) == 0) goto L_80186854;
  /* 0x8018684c: */
  g[17] = g[4];
L_80186850:
  g[2] = ((int32_t)g[3] < (int32_t)g[19]) ? 1u : 0u;
L_80186854:
  if (g[2] != 0) goto L_8018686C;         /* _bc_80186854 taken */
  /* 0x8018685c: */
  g[3] = g[0] - g[3];                     /* g3 = -g3 */
  g[2] = ((int32_t)g[19] < (int32_t)g[3]) ? 1u : 0u;
L_80186864:
  if (g[2] == 0) goto L_80186870;         /* _bc_80186864 taken */
  /* fallthrough */
L_8018686C:
  g[19] = g[3];

  /* ================= shared angle fold tail ================= */
L_80186870:
  /* jal -> lut_cos_q12(g17) */
  g[4] = g[17];                           /* delay slot */
  g[2] = (uint32_t)lut_cos_q12((int)g[4]);
L_80186878:
  g[4] = g[19];
  g[16] = g[2];                           /* delay slot: save cos(g17) */
  /* jal -> lut_cos_q12(g19) */
  g[2] = (uint32_t)lut_cos_q12((int)g[4]);
L_80186884:
  g[4] = g[17];
  g[16] = g[16] + g[2];
  g[2] = (uint32_t)g[16] >> 31;
  g[16] = g[16] + g[2];
  g[16] = (int32_t)g[16] >> 1;            /* rounded average */
  br2_st16(g[18] + 2, (uint16_t)g[16]);   /* delay slot of jal */
  /* jal -> lut_sin_q12(g17) */
  g[2] = (uint32_t)lut_sin_q12(g[4]);
L_801868A0:
  g[4] = g[19];
  br2_st16(g[18], (uint16_t)g[2]);        /* delay slot: sh s2+0 */
  /* jal -> lut_sin_q12(g19) */
  g[2] = (uint32_t)lut_sin_q12(g[4]);
L_801868AC:
  g[5] = (int32_t)(int16_t)br2_ld16(g[18]);       /* lh */
  g[4] = br2_ld16(g[18] + 2);             /* lhu */
  br2_st16(g[18] + 4, (uint16_t)g[2]);    /* sh s2+4 = sin(g19) */
  g[4] = g[4] << 16;
  g[4] = (int32_t)g[4] >> 16;             /* delay slot */
  g[2] = (uint32_t)atan2_lut((int)g[4], (int)g[5]);
L_801868C4:
  g[4] = br2_ld16(g[18] + 2);             /* lhu */
  g[5] = (int32_t)(int16_t)br2_ld16(g[18] + 4);   /* lh */
  g[4] = g[4] << 16;
  g[4] = (int32_t)g[4] >> 16;             /* delay slot */
  g[2] = (uint32_t)atan2_lut((int)g[4], (int)g[5]);

  /* ================= mode < 6 / tail ================= */
L_801868D8:
  g[16] = g[22] + 4;
  g[4] = g[16];
  g[2] = br2_ld16(g[18]);                 /* lhu */
  g[5] = fr + 16;
  br2_st16(g[23], (uint16_t)g[2]);        /* sh a2+0 */
  g[2] = br2_ld16(g[18] + 2);             /* lhu */
  g[6] = g[23];
  br2_st16(g[23] + 2, (uint16_t)g[2]);    /* sh a2+2 */
  g[2] = br2_ld16(g[18] + 4);             /* lhu */
  g[7] = 0;                               /* move zero */
  /* jal 0x80186900 -> part_matrix_compute(a1+4, sp+16, a2, 0) */
  br2_st16(g[23] + 4, (uint16_t)g[2]);    /* delay slot: sh a2+4 */
  part_matrix_compute((uint32_t *)guest_at(g[4]),
                      (int8_t *)guest_at(g[5]),
                      (short *)guest_at(g[6]), g[7]);

L_80186908:
  g[5] = g[16];                           /* a1 + 4 */
  g[8] = br2_ld32(fr + 68);               /* reload saved ptr */
  g[6] = g[22] + 32;
  /* jal 0x80186914 -> gte_rtps_xform3(g8+32, a1+4, a1+32) */
  g[4] = g[8] + 32;                       /* delay slot */
  g[2] = (uint32_t)guest_of(gte_rtps_xform3(
      (uint32_t *)guest_at(g[4]),
      (uint16_t *)guest_at(g[5]),
      (uint32_t *)guest_at(g[6])));

  /* ================= epilogue ================= */
L_8018691C:
  g[31] = br2_ld32(fr + 108);
  g[30] = br2_ld32(fr + 104);
  g[23] = br2_ld32(fr + 100);
  g[22] = br2_ld32(fr + 96);
  g[21] = br2_ld32(fr + 92);
  g[20] = br2_ld32(fr + 88);
  g[19] = br2_ld32(fr + 84);
  g[18] = br2_ld32(fr + 80);
  g[17] = br2_ld32(fr + 76);
  g[16] = br2_ld32(fr + 72);
  /* sp += 0x70 via frame_free */
  br2_guest_frame_free();
  c->gpr[2] = g[2];
  c->gpr[3] = g[3];
  return g[2];
}

/* ======== 0x80188510 ======== */

/* prim_poly_emit_6c — polygon/face GTE emitter, input stride 0x6c (108).
 * a0 = record cursor (advances by 108 per element, returned in v0),
 * a1 = OT base, a2 = out-size select (0 -> t4=36, else t4=72),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Per record: three vertex-pointer words @a0+0/+4/+8 are dereferenced and
 * loaded as raw xyz into GTE V0 (mtc2 data 12/13/14 = IR0? — the guest
 * writes data regs 12,13,14), NCLIP (cmd 0x06-family 0x1400006) writes
 * MAC0-class flag into data[24]; the three source xyz words are stored
 * raw at out+8/+16/+24. MAC0<=0 skips the transform body. Otherwise:
 * lwc2 attribute into gte[6] from a0+16, then four vertices — ptrs at
 * a0+20/+24/+28/+32 with companion data at t9=ld32(a0+12)+4 (regs
 * gte16..19) — each split into (x&0xffff, x>>16) -> data9/10 with
 * lwc2 data11 = ptr+4, RTPS (0x138041C), and the previous vertex's
 * gte_data[22] swc2'd to out+4/+12/+20/+28 (lagged by one); vertex1's
 * companion word *t9 goes to out+32 via plain sw before the first RTPS.
 * AVSZ3 (0x168002E) then a final RTPS yields OTZ in data[7]; the OT slot
 * a1 + (otz & 0x1ffc) is linked with the same swl pair as
 * prim_ot_link_indexed and gte_data[22] is stored at out+28.
 *
 * Faithful CPS transliteration: one clean body, cpu->gpr[] live mirrors,
 * block_* labels + goto for the two taken branches and the loop back-edge.
 * The generated pc-switch mid-entries (0x80188518/20/24/70/65C) are CPS
 * suspend continuations only — this port has no suspend points, so the
 * function-entry dispatch is the sole live entry (verified: no external
 * jal/jr targets the interior addresses). swc2 sites keep the generated
 * gte_precision_store_word side-call; lwc2 keeps the generated direct
 * gte_data[] write for reg 6 and gte_write_data for regs 16-19 (the
 * helper masks to 0xFFFF — different semantics, both replicated).
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188510. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_poly_emit_6c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 36;                                /* beq delay slot: t4=36 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36; /* t4 = 72 */
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                    /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                       /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);
    cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
    if ((int32_t)cpu->gpr[13] > 0) {                    /* blez t5 -> skip */
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);        /* t9 = *(a0+12) */
      cpu->gte_data[6] = br2_ld32(cpu->gpr[4] + 16);    /* lwc2 gte[6] */
      cpu->gpr[13] = br2_ld32(cpu->gpr[25]);            /* t5 = *t9 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 20);        /* v1 ptr */
      gte_write_data(cpu, 16, br2_ld32(cpu->gpr[14] + 4));/* lwc2 gte[16] */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;       /* x & 0xffff */
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;      /* x >> 16    */
      gte_write_data(cpu, 9, cpu->gpr[9]);              /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);            /* mtc2 */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[13]);        /* sw t5,32(t3) */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 24);        /* v2 ptr */
      gte_write_data(cpu, 17, br2_ld32(cpu->gpr[15] + 4));/* lwc2 gte[17] */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);    /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 28);        /* v3 ptr */
      gte_write_data(cpu, 18, br2_ld32(cpu->gpr[24] + 4));/* lwc2 gte[18] */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 32);        /* v4 ptr */
      gte_write_data(cpu, 19, br2_ld32(cpu->gpr[25] + 4));/* lwc2 gte[19] */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      gte_execute(cpu, 0x168002Eu);                     /* AVSZ3 */
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gpr[8]  = gte_read_data(cpu, 7);             /* mfc2 t0, data7 */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;             /* otz & 0x1ffc */
      cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];         /* OT slot */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);             /* old = *slot */
      cpu->gpr[9]  = cpu->gpr[11] << 8;                 /* out << 8 */
      swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);          /* swl slot+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                  /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);         /* swl out+2 */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 108;                    /* bgtz delay slot: a0 += 0x6c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018866c ======== */

/* prim_poly_emit_64 — polygon/face GTE emitter, input stride 0x64 (100).
 * Sibling of prim_poly_emit_6c (0x80188510) and prim_poly_emit_88
 * (0x80188754): a0 = record cursor (advances by 100 per element, returned
 * in v0), a1 = OT base, a2 = out-size select (0 -> t4=20, else t4=60),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Per record: three vertex-pointer words @a0+0/+4/+8 are dereferenced and
 * loaded as raw xyz into GTE V0 (mtc2 data 12/13/14), NCLIP
 * (0x1400006) writes the MAC0-class flag into data[24]; the three source
 * xyz words are stored raw at out+8/+16/+24. MAC0<=0 skips the transform
 * body. Otherwise: attribute int8_t = lbu(out+7)<<24 | 0x00808080 written
 * DIRECT to gte_data[6] (mtc2, no lwc2); companion t9=ld32(a0+12) and
 * v1=ld32(a0+16); all four lwc2 attribute words load up front —
 * gte16=t6+4, gte17=t7+4, gte18=t8+4, gte19=t9+4 — then AVSZ4
 * (0x168002E) runs BEFORE any RTPS. *t9 is stored raw at out+32; *v1 is
 * split into (x&0xffff, x>>16) -> data9/10 with lwc2 data11 = v1+4;
 * RTPS (0x138041C) yields OTZ in data[7]; the OT slot a1 + (otz & 0x1ffc)
 * is linked with the swl pair (slot+2 <- out<<8, out+2 <- old<<8) and
 * gte_data[22] is swc2'd to out+4 (the only lagged store — this variant
 * emits a single transformed vertex, not a quad).
 *
 * Faithful CPS transliteration in the same style as the 0x6c/0x88
 * siblings: cpu->gpr[] live mirrors, block_* labels + goto for the two
 * taken branches and the loop back-edge. lwc2 sites use the generated
 * psx_cyc_lwc2_read (charge + raw read); the gte[6] mtc2 keeps the
 * generated direct gte_data[] write (gte_write_data would mask to
 * 0xFFFF); swc2 keeps the generated gte_precision_store_word side-call.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_8018866C. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_poly_emit_64(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 20;                                /* beq delay slot: t4=20 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 40; /* t4 = 60 */
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                    /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                       /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);
    cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
    cpu->gpr[10] = 0x00800000u;                         /* lui t2,0x80  */
    cpu->gpr[10] = cpu->gpr[10] | 0x8080u;              /* ori t2,0x8080 */
    if ((int32_t)cpu->gpr[13] > 0) {                    /* blez t5 -> skip */
      cpu->gpr[9]  = br2_ld8(cpu->gpr[11] + 7);         /* lbu t1,7(t3) */
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);        /* t9 = *(a0+12) */
      cpu->gpr[9]  = cpu->gpr[9] << 24;                 /* attr << 24 */
      cpu->gpr[9]  = cpu->gpr[9] | cpu->gpr[10];        /* | 0x00808080 */
      cpu->gte_data[6] = cpu->gpr[9];                   /* mtc2 gte[6] */
      gte_write_data(cpu, 16, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4));/* lwc2 */
      gte_write_data(cpu, 17, psx_cyc_lwc2_read(cpu, cpu->gpr[15] + 4));/* lwc2 */
      gte_write_data(cpu, 18, psx_cyc_lwc2_read(cpu, cpu->gpr[24] + 4));/* lwc2 */
      gte_write_data(cpu, 19, psx_cyc_lwc2_read(cpu, cpu->gpr[25] + 4));/* lwc2 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 16);        /* v1 ptr */
      gte_execute(cpu, 0x168002Eu);                     /* AVSZ4 */
      cpu->gpr[13] = br2_ld32(cpu->gpr[25]);            /* t5 = *t9 */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);             /* *v1 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 */
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;       /* *v1 low16  */
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;      /* *v1 high16 */
      gte_write_data(cpu, 9, cpu->gpr[9]);              /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);            /* mtc2 */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[13]);        /* sw t5,32(t3) */
      cpu->gpr[8]  = gte_read_data(cpu, 7);             /* mfc2 t0, data7 */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;             /* otz & 0x1ffc */
      cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];         /* OT slot */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);             /* old = *slot */
      cpu->gpr[9]  = cpu->gpr[11] << 8;                 /* out << 8 */
      swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);          /* swl slot+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                  /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);         /* swl out+2 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);    /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 100;                    /* bgtz delay slot: a0 += 0x64 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188754 ======== */

/* prim_poly_emit_88 — sibling polygon GTE emitter, input stride 0x88
 * (136). Same skeleton as prim_poly_emit_6c with a wider output record:
 * out-delta t4 = 32 when a2==0 else 84 (32+52); NCLIP raw stores at
 * out+8/+20/+32; attribute int8_t = lbu(out+7)<<24 | 0x00808080 written
 * direct to gte_data[6] (no lwc2); vertex ptrs at a0+16/+20/+24/+28
 * (vs +20..+32 in the 0x6c variant); companion data t9=ld32(a0+12)+4 ->
 * gte16..19 as before; lagged swc2 gte[22] stores at out+4/+16/+28/+40;
 * *t9 stored at out+44 via plain sw (placed after the v2 vertex load,
 * before its swc2 — NOT before the first RTPS like the 0x6c variant);
 * same AVSZ3 -> OTZ -> swl OT-link tail. Record stride 136.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188754. */
uint32_t prim_poly_emit_88(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                                /* beq delay slot: t4=32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 52; /* t4 = 84 */
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                    /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                       /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
    br2_st32(cpu->gpr[11] + 20, cpu->gpr[9]);
    br2_st32(cpu->gpr[11] + 32, cpu->gpr[10]);
    cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
    cpu->gpr[10] = 0x00800000u;                         /* lui t2,0x80 */
    cpu->gpr[10] = cpu->gpr[10] | 0x8080u;              /* delay slot: t2 |= 0x8080 (unconditional) */
    if ((int32_t)cpu->gpr[13] > 0) {                    /* blez t5 -> skip */
      cpu->gpr[9]  = br2_ld8(cpu->gpr[11] + 7);         /* lbu t1,7(t3) */
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);        /* t9 = *(a0+12) */
      cpu->gpr[9]  = cpu->gpr[9] << 24;                 /* attr << 24 */
      cpu->gpr[9]  = cpu->gpr[9] | cpu->gpr[10];        /* | 0x00808080 */
      cpu->gte_data[6] = cpu->gpr[9];                   /* mtc2 gte[6] */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 16);        /* v1 ptr */
      cpu->gpr[13] = br2_ld32(cpu->gpr[25]);            /* t5 = *t9 */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      gte_write_data(cpu, 16, br2_ld32(cpu->gpr[14] + 4));/* lwc2 gte[16] */
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;       /* x & 0xffff */
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;      /* x >> 16    */
      gte_write_data(cpu, 9, cpu->gpr[9]);              /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);            /* mtc2 */
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 20);        /* v2 ptr */
      gte_write_data(cpu, 17, br2_ld32(cpu->gpr[15] + 4));/* lwc2 gte[17] */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      br2_st32(cpu->gpr[11] + 44, cpu->gpr[13]);        /* sw t5,44(t3) */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);    /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 24);        /* v3 ptr */
      gte_write_data(cpu, 18, br2_ld32(cpu->gpr[24] + 4));/* lwc2 gte[18] */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 16, 22);
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 28);        /* v4 ptr */
      gte_write_data(cpu, 19, br2_ld32(cpu->gpr[25] + 4));/* lwc2 gte[19] */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
      gte_execute(cpu, 0x168002Eu);                     /* AVSZ3 */
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, br2_ld32(cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gpr[8]  = gte_read_data(cpu, 7);             /* mfc2 t0, data7 */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;             /* otz & 0x1ffc */
      cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];         /* OT slot */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);             /* old = *slot */
      cpu->gpr[9]  = cpu->gpr[11] << 8;                 /* out << 8 */
      swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);          /* swl slot+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                  /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);         /* swl out+2 */
      br2_st32(cpu->gpr[11] + 40, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 40, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 136;                    /* bgtz delay slot: a0 += 0x88 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x801888c0 ======== */

/* prim_poly_emit_6c_tex — textured sibling of prim_poly_emit_6c, input
 * stride 0x6c (108), out-delta t4 = 36 when a2==0 else 72; a0 advances by
 * 108 per element and is returned in v0. Same NCLIP prologue and raw
 * stores at out+8/+16/+24, but the transform body differs: ONE shared
 * texcoord vertex at *(a0+32) feeds data9/10/11 for all four RTPS passes —
 * word0 split (x&0xffff -> data9, x>>16 -> data10) plus lhu(+4) -> data11
 * via mtc2 (the mtc2 triple is re-issued before every RTPS from the same
 * live gpr[8]/[9]/[10] values). Per-vertex attribute words load DIRECT to
 * gte_data[6] via lwc2 at a0+16/+20/+24/+28 (interleaved between the
 * RTPS passes, NOT up front like the 0x64 variant); position attrs
 * gte[16..19] = lwc2 *(t6/t7/t8/t9 + 4). *t9 stored raw at out+32 before
 * the first RTPS. Three RTPS (0x138041C) + one AVSZ4 (0x168002E); lagged
 * swc2 gte[22] at out+4/+12/+20/+28; same OTZ->swl tail.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801888C0. */
uint32_t prim_poly_emit_6c_tex(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 36;                                /* beq delay slot: t4=36 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36; /* t4 = 72 */
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                    /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                       /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);
    cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
    if ((int32_t)cpu->gpr[13] > 0) {                    /* blez t5 -> skip */
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);        /* t9 = *(a0+12) */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);  /* lwc2 gte[6] */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 32);        /* shared uv ptr */
      cpu->gpr[13] = br2_ld32(cpu->gpr[25]);            /* t5 = *t9 */
      cpu->gpr[9]  = br2_ld32(cpu->gpr[8]);             /* uv word0 */
      cpu->gpr[10] = br2_ld16(cpu->gpr[8] + 4);         /* lhu uv+4 -> t2 */
      cpu->gpr[8]  = cpu->gpr[9] << 16;
      cpu->gpr[8]  = (uint32_t)cpu->gpr[8] >> 16;       /* low16  -> data9 */
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;       /* high16 -> data10 */
      gte_write_data(cpu, 9, cpu->gpr[8]);              /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[9]);             /* mtc2 */
      gte_write_data(cpu, 11, cpu->gpr[10]);            /* mtc2 */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[13]);        /* sw t5,32(t3) */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v0 */
      gte_write_data(cpu, 16, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4));/* lwc2 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);    /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      gte_write_data(cpu, 9, cpu->gpr[8]);              /* mtc2 (same uv) */
      gte_write_data(cpu, 10, cpu->gpr[9]);
      gte_write_data(cpu, 11, cpu->gpr[10]);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20);  /* lwc2 gte[6] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v1 */
      gte_write_data(cpu, 17, psx_cyc_lwc2_read(cpu, cpu->gpr[15] + 4));/* lwc2 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      gte_write_data(cpu, 9, cpu->gpr[8]);              /* mtc2 (same uv) */
      gte_write_data(cpu, 10, cpu->gpr[9]);
      gte_write_data(cpu, 11, cpu->gpr[10]);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);  /* lwc2 gte[6] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v2 */
      gte_write_data(cpu, 18, psx_cyc_lwc2_read(cpu, cpu->gpr[24] + 4));/* lwc2 */
      gte_write_data(cpu, 19, psx_cyc_lwc2_read(cpu, cpu->gpr[25] + 4));/* lwc2 */
      gte_execute(cpu, 0x168002Eu);                     /* AVSZ4 */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      gte_write_data(cpu, 9, cpu->gpr[8]);              /* mtc2 (same uv) */
      gte_write_data(cpu, 10, cpu->gpr[9]);
      gte_write_data(cpu, 11, cpu->gpr[10]);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28);  /* lwc2 gte[6] */
      cpu->gpr[8]  = gte_read_data(cpu, 7);             /* mfc2 t0, data7 */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v3 */
      cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;             /* otz & 0x1ffc */
      cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];         /* OT slot */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);             /* old = *slot */
      cpu->gpr[9]  = cpu->gpr[11] << 8;                 /* out << 8 */
      swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);          /* swl slot+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                  /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);         /* swl out+2 */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 108;                    /* bgtz delay slot: a0 += 0x6c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x801889f4 ======== */

/* prim_poly_emit_78 — quad textured sibling, input stride 0x78 (120),
 * out-delta t4 = 48 when a2==0 else 84; a0 advances by 120 per element.
 * Same NCLIP prologue/raw stores. Transform body: per-vertex texcoords —
 * vertex ptrs at a0+32/+36/+40/+44; each vertex loads *vN into gpr[10],
 * splits (low16 -> data9 via gpr[9], high16 -> data10) and loads data11
 * via lwc2 *(vN+4) (v1's lwc2 precedes the split; v2-v4 follow mtc2 10).
 * Per-vertex attribute lwc2 gte[6] DIRECT at a0+16/+20/+24/+28,
 * interleaved before each RTPS; position attrs gte[16..19] =
 * lwc2 *(t6/t7/t8/t9 + 4). *t9 stored raw at out+32 before vertex 0's
 * RTPS; four RTPS + one AVSZ4; lagged swc2 gte[22] at out+4/+12/+20/+28;
 * same OTZ->swl tail.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801889F4. */
uint32_t prim_poly_emit_78(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 48;                                /* beq delay slot: t4=48 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36; /* t4 = 84 */
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                    /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                       /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);
    cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
    if ((int32_t)cpu->gpr[13] > 0) {                    /* blez t5 -> skip */
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);        /* t9 = *(a0+12) */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);  /* lwc2 gte[6] */
      cpu->gpr[13] = br2_ld32(cpu->gpr[25]);            /* t5 = *t9 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 32);        /* v1 ptr */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[13]);        /* sw t5,32(t3) */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);             /* *v1 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;       /* x & 0xffff */
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;      /* x >> 16    */
      gte_write_data(cpu, 9, cpu->gpr[9]);              /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);            /* mtc2 */
      gte_write_data(cpu, 16, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4));/* lwc2 */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v0 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 36);        /* v2 ptr */
      gte_write_data(cpu, 17, psx_cyc_lwc2_read(cpu, cpu->gpr[15] + 4));/* lwc2 */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);             /* *v2 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);    /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20);  /* lwc2 gte[6] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v1 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 40);        /* v3 ptr */
      gte_write_data(cpu, 18, psx_cyc_lwc2_read(cpu, cpu->gpr[24] + 4));/* lwc2 */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);             /* *v3 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);  /* lwc2 gte[6] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v2 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 44);        /* v4 ptr */
      gte_write_data(cpu, 19, psx_cyc_lwc2_read(cpu, cpu->gpr[25] + 4));/* lwc2 */
      cpu->gpr[10] = br2_ld32(cpu->gpr[8]);             /* *v4 */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      gte_execute(cpu, 0x168002Eu);                     /* AVSZ4 */
      cpu->gpr[9]  = cpu->gpr[10] << 16;
      cpu->gpr[9]  = (uint32_t)cpu->gpr[9] >> 16;
      cpu->gpr[10] = (uint32_t)cpu->gpr[10] >> 16;
      gte_write_data(cpu, 9, cpu->gpr[9]);
      gte_write_data(cpu, 10, cpu->gpr[10]);
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28);  /* lwc2 gte[6] */
      cpu->gpr[8]  = gte_read_data(cpu, 7);             /* mfc2 t0, data7 */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS v3 */
      cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;             /* otz & 0x1ffc */
      cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];         /* OT slot */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);             /* old = *slot */
      cpu->gpr[9]  = cpu->gpr[11] << 8;                 /* out << 8 */
      swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);          /* swl slot+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                  /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);         /* swl out+2 */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);   /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 120;                    /* bgtz delay slot: a0 += 0x78 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188b5c ======== */

/* prim_xform_emit — GTE transform/emit helper (NOT the poly family):
 * loads six packed words via lwc2 into gte data[0..5] (regs 0/2/4 DIRECT
 * gte_data[] writes, regs 1/3/5 via gte_write_data — the generated mix is
 * preserved), executes gte cmd 0x0280030, cfc2 ctrl31 -> v0, then NCLIP.
 * MAC0 (data24) <= 0 skips the emit body. Otherwise gte cmd 0x158002D,
 * then swc2 gte[12]/[13]/[14] -> out+8/+8+t8/+8+2*t8 (t8 = 4 for this
 * entry), mfc2 data7 -> OTZ, and the same OTZ->swl tail; a0 += t7 (64)
 * per element, a3 count, returns advanced a0.
 *
 * NOTE: generated br2eu_func_80188B5C has FOUR entry blocks setting
 * (t6 out-delta base, t7 record stride, t8 sub-stride):
 *   0x80188B5C: t6=20 t7=64  t8=4   <- this port (renames.map entry)
 *   0x80188B6C: t6=28 t7=80  t8=8   (stays generated — separate dispatch
 *   0x80188B7C: t6=32 t7=88  t8=8    entries; clean_table has no
 *   0x80188B8C: t6=40 t7=104 t8=12   multi-entry contract for them)
 * out-delta t4 = 24 (a2==0) else 24+t6.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188B5C. */
uint32_t prim_xform_emit(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x80188B5C: t6=20, t7=64, t8=4 */
  cpu->gpr[14] = 20;                                 /* t6 = 20 */
  cpu->gpr[15] = 64;                                 /* t7 = 64 */
  cpu->gpr[24] = 4;                                  /* t8 = 4 */

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 24;                                /* beq delay slot: t4=24 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];  /* t4 += t6 */
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);          /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));/* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);      /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));/* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);     /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));/* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                    /* a3-- */
    gte_execute(cpu, 0x0280030u);                       /* gte cmd 0x30 */
    cpu->gpr[13] = cpu->gpr[4] + cpu->gpr[12];          /* t5 = a0+t4 (out) */
    cpu->gpr[9]  = cpu->gpr[13] + 8;                    /* t1 = out+8 */
    cpu->gpr[10] = cpu->gpr[9] + cpu->gpr[24];          /* t2 = t1+t8 */
    cpu->gpr[11] = cpu->gpr[10] + cpu->gpr[24];         /* t3 = t2+t8 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);              /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                       /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                   /* mfc2 t0, data24 */
    if ((int32_t)cpu->gpr[8] > 0) {                     /* blez t0 -> skip */
      gte_execute(cpu, 0x158002Du);                     /* gte cmd 0x2D */
      br2_st32(cpu->gpr[9],  cpu->gte_data[12]);        /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[9], 12);
      br2_st32(cpu->gpr[10], cpu->gte_data[13]);        /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[10], 13);
      br2_st32(cpu->gpr[11], cpu->gte_data[14]);        /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11], 14);
      cpu->gpr[8]  = gte_read_data(cpu, 7);             /* mfc2 t0, data7 */
      cpu->gpr[9]  = cpu->gpr[13] << 8;                 /* out << 8 */
      cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;             /* otz & 0x1ffc */
      cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];         /* OT slot */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);             /* old = *slot */
      swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);          /* swl slot+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                  /* old << 8 */
      swl_guest(cpu->gpr[13] + 2, cpu->gpr[8]);         /* swl out+2 */
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];           /* bgtz delay slot: a0 += t7 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189e2c ======== */

/* prim_poly_emit_64_vs — versus-mode (mode==1) variant of
 * prim_poly_emit_64: same input stride 0x64 (100), same out-delta
 * t4 = 20/60, same NCLIP prologue and raw stores at out+8/+16/+24.
 * Differences from the 0x64 sibling: attribute word = lbu(out+7)<<24 |
 * 0x00808080 written DIRECT to gte_data[6] (no lwc2); texcoord pair =
 * TWO lhu loads (*(v+0) -> data9, *(v+2) -> data10) plus lwc2 data11 =
 * *(v+4) — no 32-bit split; NO OTZ/mfc2 — the OT link uses a1 DIRECTLY
 * as the slot (ld32(a1), swl a1+2 <- out<<8, swl out+2 <- old<<8); single
 * RTPS and a single lagged swc2 gte[22] at out+4. a0 += 100 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189E2C. */
uint32_t prim_poly_emit_64_vs(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 20;                                /* beq delay slot: t4=20 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 40; /* t4 = 60 */
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);               /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);           /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);           /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);              /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);              /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);              /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);               /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);               /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);              /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                    /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];          /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                       /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);
    cpu->gpr[13] = cpu->gte_data[24];                   /* mfc2 t5, data24 */
    if ((int32_t)cpu->gpr[13] > 0) {                    /* blez t5 -> skip */
      cpu->gpr[10] = 0x00800000u;                       /* lui t2,0x80 */
      cpu->gpr[9]  = br2_ld8(cpu->gpr[11] + 7);         /* lbu t1,7(t3) */
      cpu->gpr[10] = cpu->gpr[10] | 0x8080u;            /* ori t2,0x8080 */
      cpu->gpr[9]  = cpu->gpr[9] << 24;                 /* attr << 24 */
      cpu->gpr[9]  = cpu->gpr[9] | cpu->gpr[10];        /* | 0x00808080 */
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);        /* t9 = *(a0+12) */
      cpu->gte_data[6] = cpu->gpr[9];                   /* mtc2 gte[6] direct */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[25]);            /* t0 = *t9 */
      cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 16);        /* v ptr */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[8]);         /* sw t0,32(t3) */
      cpu->gpr[8]  = br2_ld16(cpu->gpr[10]);            /* lhu v+0 -> t0 */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[10] + 2);        /* lhu v+2 -> t1 */
      gte_write_data(cpu, 9, cpu->gpr[8]);              /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[9]);             /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[10] + 4));/* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                     /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);             /* old = *a1 (fixed OT slot) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;                 /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);          /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                  /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);         /* swl out+2 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);    /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 100;                    /* bgtz delay slot: a0 += 0x64 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188c34 ======== */

/* prim_nclip_emit_50 — NCLIP-gated GTE emitter, input record stride 80
 * (0x50). Sibling of prim_xform_emit (0x80188B5C): a0 = record cursor
 * (advances by t7=80 per element, returned in v0), a1 = OT base,
 * a2 = out-delta select (0 -> t4=32, else t4=32+t6), a3 = signed count
 * (<=0 -> return cursor unchanged). Entry 0x80188C34 sets t6=24, t7=80,
 * t8=4; the sibling entry blocks 0x80188C44/54/64 (t6=36/40/52,
 * t7=104/112/136, t8=8/8/12) are NOT separately registered and stay on
 * the generated dispatcher, same contract as prim_xform_emit.
 *
 * Per record: six packed attribute words lwc2'd straight from a0+0..+20
 * into gte data[0..5] (regs 0/2/4 DIRECT gte_data[] writes, 1/3/5 via
 * gte_write_data — generated mix preserved); a3--; gte cmd 0x0280030;
 * t3 = a0+t4 (out), t5 = out+8; two plain lw's a0+24 -> t1, a0+28 -> t2;
 * cfc2 ctrl31 -> v0; NCLIP (0x1400006); t0 = data24 (MAC0-class flag).
 * MAC0<=0 skips the emit body. Otherwise: swc2 gte[12] -> out+8; mtc2
 * gte[0]=t1 DIRECT + gte_write_data(1,t2); t5 += t8; gte cmd 0x0180001;
 * t9 = t5+t8, t1 = t9+t8; AVSZ3 (0x168002E); swc2 gte[12]/[13]/[14] ->
 * t5/t9/t1 (out+8+t8 / +2t8 / +3t8); mfc2 data7 -> OTZ; OT slot =
 * a1 + (otz & 0x1ffc) linked with the swl pair; loop a0 += t7.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188C34. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_nclip_emit_50(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  cpu->gpr[14] = 24;                                 /* t6 = 24 */
  cpu->gpr[15] = 80;                                 /* t7 = 80 */
  cpu->gpr[24] = 4;                                  /* t8 = 4 */

  if ((int32_t)cpu->gpr[7] <= 0) {                   /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                               /* beq delay slot: t4=32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14]; /* t4 += t6 */
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);          /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));/* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);      /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));/* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);     /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));/* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                 /* a3-- */
    gte_execute(cpu, 0x0280030u);                    /* gte cmd 0x30 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];       /* t3 = a0+t4 (out) */
    cpu->gpr[13] = cpu->gpr[11] + 8;                 /* t5 = out+8 */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 24);       /* t1 = *(a0+24) */
    cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 28);       /* t2 = *(a0+28) */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);           /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                    /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                /* mfc2 t0, data24 */
    if ((int32_t)cpu->gpr[8] > 0) {                  /* blez t0 -> skip */
      br2_st32(cpu->gpr[13], cpu->gte_data[12]);     /* swc2 gte[12] -> out+8 */
      gte_precision_store_word(cpu->gpr[13], 12);
      cpu->gte_data[0] = cpu->gpr[9];                /* mtc2 gte[0] = t1 */
      gte_write_data(cpu, 1, cpu->gpr[10]);          /* mtc2 gte[1] = t2 */
      cpu->gpr[13] = cpu->gpr[13] + cpu->gpr[24];    /* t5 += t8 */
      gte_execute(cpu, 0x0180001u);                  /* gte cmd 0x01 */
      cpu->gpr[25] = cpu->gpr[13] + cpu->gpr[24];    /* t9 = t5+t8 */
      cpu->gpr[9]  = cpu->gpr[25] + cpu->gpr[24];    /* t1 = t9+t8 */
      gte_execute(cpu, 0x168002Eu);                  /* AVSZ3 */
      br2_st32(cpu->gpr[13], cpu->gte_data[12]);     /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[13], 12);
      br2_st32(cpu->gpr[25], cpu->gte_data[13]);     /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[25], 13);
      br2_st32(cpu->gpr[9],  cpu->gte_data[14]);     /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[9], 14);
      cpu->gpr[8]  = gte_read_data(cpu, 7);          /* mfc2 t0, data7 */
      cpu->gpr[9]  = cpu->gpr[11] << 8;              /* out << 8 */
      cpu->gpr[2]  = cpu->gpr[8] & 0x1FFCu;          /* otz & 0x1ffc */
      cpu->gpr[2]  = cpu->gpr[5] + cpu->gpr[2];      /* OT slot */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[2]);          /* old = *slot */
      swl_guest(cpu->gpr[2] + 2, cpu->gpr[9]);       /* swl slot+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;               /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);      /* swl out+2 */
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];        /* bgtz delay slot: a0 += t7 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188d2c ======== */

/* prim_nclip_emit_4c — NCLIP-gated GTE emitter, input record stride 76
 * (0x4c). Same prologue as prim_nclip_emit_50: a0 cursor (+=76/elem,
 * returned), a1 = OT base, a2 out-delta select (0 -> t4=36, else 36+20=56),
 * a3 count. Six lwc2 words a0+0..+20 -> gte[0..5] (0/2/4 direct, 1/3/5
 * gte_write_data); a3--; gte cmd 0x0280030; cfc2 ctrl31; NCLIP;
 * data24<=0 skips. Otherwise: lwc2 gte[6]=a0+24, gte[0]=a0+28,
 * gte[1]=a0+32; gte cmd 0x108041B; swc2 gte[12]/[13]/[14] -> out+8/+12/
 * +16; OT link is FIXED at a1 (no OTZ index — ld32(a1), swl a1+2 <-
 * out<<8, swl out+2 <- old<<8); swc2 gte[22] -> out+4.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188D2C. */
uint32_t prim_nclip_emit_4c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80188D2C: out-delta t4 = 36 (a2==0) else 56, computed once;
   * per element NCLIP gates the emit on gte data24 (MAC0) > 0;
   * a3-- mid-body, a0 += 0x4c in the bgtz delay slot (runs on the
   * final iteration too), loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 36;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 20;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);          /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));/* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);      /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));/* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);     /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));/* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);   /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28);   /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 8,  cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 12, 13);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 16, 14);
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);  /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 76;                   /* bgtz delay slot: a0 += 0x4c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188dcc ======== */

/* prim_nclip_emit_5c — NCLIP-gated GTE emitter, input record stride 92
 * (0x5c). Same prologue; out-delta t4 = 44 (a2==0) else 44+24=68; a0 += 92
 * per element. MAC0<=0 skips. Otherwise a TWO-STAGE transform: stage1
 * lwc2 gte[0]=a0+24, gte[1]=a0+28 then gte cmd 0x0180001, plain sw of the
 * PRE-op mfc2'd data12 (t0) -> out+8; lwc2 gte[6]=a0+32; swc2 gte[14] ->
 * out+20; stage2 lwc2 gte[0]=a0+36, gte[1]=a0+40 then gte cmd 0x108041B;
 * swc2 gte[12]/[13] -> out+12/+16; FIXED OT link at a1 (no OTZ);
 * swc2 gte[22] -> out+4.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188DCC. */
uint32_t prim_nclip_emit_5c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80188DCC: out-delta t4 = 44 (a2==0) else 68, computed once;
   * per element NCLIP gates on gte data24 > 0; a3-- mid-body, a0 += 92
   * in the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 44;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 24;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);          /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));/* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);      /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));/* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);     /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));/* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gpr[8]  = cpu->gte_data[12];               /* mfc2 t0, data12 (pre-op) */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);   /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28));/* lwc2 gte[1] */
      gte_execute(cpu, 0x0180001u);                   /* gte cmd 0x01 */
      br2_st32(cpu->gpr[11] + 8, cpu->gpr[8]);        /* sw t0,8(t3) — plain sw */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);   /* lwc2 gte[6] */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 20, 14);
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36);   /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 12, 12);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 16, 13);
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);  /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 92;                   /* bgtz delay slot: a0 += 0x5c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188e88 ======== */

/* prim_nclip_emit_6c — NCLIP-gated GTE emitter, input record stride 108
 * (0x6c). Same prologue; out-delta t4 = 52 (a2==0) else 52+28=80; a0 += 108
 * per element. MAC0<=0 skips. Otherwise: SEVEN lwc2 words — gte[6]=a0+24,
 * gte[0..5]=a0+28..+48 — then gte cmd 0x118043F; swc2 gte[12]/[13]/[14]
 * -> out+8/+16/+24; FIXED OT link at a1 (no OTZ, no gte[22] store).
 * Distinct from prim_poly_emit_6c which dereferences vertex pointers and
 * runs RTPS passes — this variant feeds packed records straight to GTE.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188E88. */
uint32_t prim_nclip_emit_6c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80188E88: out-delta t4 = 52 (a2==0) else 80, computed once;
   * per element NCLIP gates on gte data24 > 0; a3-- mid-body, a0 += 108
   * in the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 52;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 28;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);          /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));/* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);      /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));/* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);     /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));/* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);   /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28);   /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32));/* lwc2 gte[1] */
      cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36);   /* lwc2 gte[2] */
      gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40));/* lwc2 gte[3] */
      cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 44);   /* lwc2 gte[4] */
      gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 48));/* lwc2 gte[5] */
      gte_execute(cpu, 0x118043Fu);                   /* gte cmd 0x3F */
      br2_st32(cpu->gpr[11] + 8,  cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 16, 13);
      br2_st32(cpu->gpr[11] + 24, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 24, 14);
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
    }
    cpu->gpr[4] = cpu->gpr[4] + 108;                  /* bgtz delay slot: a0 += 0x6c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80188f40 ======== */

/* prim_nclip_emit_8c — NCLIP-gated GTE emitter, input record stride 140
 * (0x8c). Same prologue; out-delta t4 = 68 (a2==0) else 68+36=104; a0 += 140
 * per element. MAC0<=0 skips. Otherwise a THREE-STAGE transform:
 * stage1 mfc2 data12 -> t0, lwc2 gte[0]=a0+24, gte[1]=a0+28, gte cmd
 * 0x0180001, plain sw t0 -> out+8; lwc2 gte[6]=a0+32, gte[2..5]=a0+44..
 * +56, gte[0]=a0+36, gte[1]=a0+40, swc2 gte[14] -> out+32 (interleaved
 * BEFORE the 0x118043F op), gte cmd 0x118043F, swc2 gte[12]/[13] ->
 * out+16/+24, swc2 gte[20] -> out+4; stage2 lwc2 gte[0]=a0+60,
 * gte[1]=a0+64, gte cmd 0x108041B; FIXED OT link at a1; swc2
 * gte[22]/[20]/[21] -> out+28/+12/+20.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80188F40. */
uint32_t prim_nclip_emit_8c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80188F40: out-delta t4 = 68 (a2==0) else 104, computed once;
   * per element NCLIP gates on gte data24 > 0; a3-- mid-body, a0 += 140
   * in the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 68;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);          /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));/* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);      /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));/* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);     /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));/* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gpr[8]  = cpu->gte_data[12];               /* mfc2 t0, data12 (pre-op) */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);   /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28));/* lwc2 gte[1] */
      gte_execute(cpu, 0x0180001u);                   /* gte cmd 0x01 */
      br2_st32(cpu->gpr[11] + 8, cpu->gpr[8]);        /* sw t0,8(t3) — plain sw */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);   /* lwc2 gte[6] */
      cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 44);   /* lwc2 gte[2] */
      gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 48));/* lwc2 gte[3] */
      cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 52);   /* lwc2 gte[4] */
      gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 56));/* lwc2 gte[5] */
      br2_st32(cpu->gpr[11] + 32, cpu->gte_data[14]); /* swc2 gte[14] -> out+32 */
      gte_precision_store_word(cpu->gpr[11] + 32, 14);
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36);   /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40));/* lwc2 gte[1] */
      gte_execute(cpu, 0x118043Fu);                   /* gte cmd 0x3F */
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 16, 12);
      br2_st32(cpu->gpr[11] + 24, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 24, 13);
      br2_st32(cpu->gpr[11] + 4,  cpu->gte_data[20]); /* swc2 gte[20] */
      gte_precision_store_word(cpu->gpr[11] + 4, 20);
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 60);   /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 64));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[20]); /* swc2 gte[20] */
      gte_precision_store_word(cpu->gpr[11] + 12, 20);
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[21]); /* swc2 gte[21] */
      gte_precision_store_word(cpu->gpr[11] + 20, 21);
    }
    cpu->gpr[4] = cpu->gpr[4] + 140;                  /* bgtz delay slot: a0 += 0x8c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018902c ======== */

/* prim_nclip_emit_60 — NCLIP-gated GTE emitter, input record stride 96
 * (0x60). Same six-word prologue; out-delta t4 = 32 (a2==0) else 32+32=64;
 * a0 += 96 per element. MAC0<=0 skips. Otherwise: t3 = out, attribute
 * int8_t = lbu(out+7)<<24 | 0x00808080 (built in gpr[24] via lui/ori/or);
 * t1 = lw(a0+24), t2 = lw(a0+28); cfc2; NCLIP; THEN mtc2 gte[6]=attr
 * (DIRECT gte_data[]), gte[0]=t1 (DIRECT), gte[1]=t2 (gte_write_data);
 * data24<=0 skips. Otherwise gte cmd 0x108041B; swc2 gte[12]/[13]/[14]
 * -> out+8/+16/+24; FIXED OT link at a1; swc2 gte[22] -> out+4.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_8018902C. */
uint32_t prim_nclip_emit_60(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x8018902C: out-delta t4 = 32 (a2==0) else 64; per element NCLIP
   * gates on gte data24 > 0; a3-- mid-body, a0 += 96 in the bgtz delay
   * slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 32;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);          /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));/* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);      /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));/* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);     /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));/* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    cpu->gpr[24] = br2_ld8(cpu->gpr[11] + 7);         /* lbu t8,7(t3) */
    cpu->gpr[25] = 0x00800000u;                       /* lui t9,0x80 */
    cpu->gpr[25] = cpu->gpr[25] | 0x8080u;            /* ori t9,0x8080 */
    cpu->gpr[24] = cpu->gpr[24] << 24;                /* attr << 24 */
    cpu->gpr[24] = cpu->gpr[24] | cpu->gpr[25];       /* | 0x00808080 */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 24);        /* t1 = *(a0+24) */
    cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 28);        /* t2 = *(a0+28) */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gte_data[6] = cpu->gpr[24];                  /* mtc2 gte[6] attr */
    cpu->gte_data[0] = cpu->gpr[9];                   /* mtc2 gte[0] = t1 */
    gte_write_data(cpu, 1, cpu->gpr[10]);             /* mtc2 gte[1] = t2 */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    if ((int32_t)cpu->gpr[8] > 0) {
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 */
      br2_st32(cpu->gpr[11] + 8,  cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 16, 13);
      br2_st32(cpu->gpr[11] + 24, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 24, 14);
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);  /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 96;                   /* bgtz delay slot: a0 += 0x60 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x801891a4 ======== */

/* prim_nclip_emit_80 — NCLIP-gated GTE emitter, input record stride 128
 * (0x80). Sibling of prim_nclip_emit_60 (0x8018902C): a0 = record cursor
 * (advances by 128 per element, returned in v0), a1 = OT base, a2 =
 * out-size select (0 -> t4=48, else t4=48+40=88), a3 = signed count
 * (<=0 -> return cursor unchanged).
 *
 * Per record: six-word prologue lwc2 gte[0..5] @a0+0..20 (direct gte_data[]
 * for regs 0/2/4, gte_write_data for 1/3/5); a3--; t3 = a0+t4 (out) BEFORE
 * the GTE ops; gte cmd 0x30; attribute int8_t = lbu(out+7)<<24 | 0x00808080
 * built in gpr[24]/gpr[25] and written DIRECT to gte_data[6]; cfc2;
 * NCLIP (0x1400006); lwc2 gte[0] @a0+24 (DIRECT); MAC0 (data24)<=0 skips.
 * Otherwise: lwc2 gte[1..5] @a0+28..44; gte cmd 0x118043F; swc2
 * gte[12]/[13]/[14] -> out+8/+20/+32; FIXED OT link at a1 (lw *a1, swl
 * pair); swc2 gte[20]/[21]/[22] -> out+4/+16/+28.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801891A4. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_nclip_emit_80(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x801891A4: out-delta t4 = 48 (a2==0) else 88; per element NCLIP
   * gates on gte data24 > 0; a3-- mid-body, a0 += 128 in the bgtz delay
   * slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 48;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 40;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[24] = br2_ld8(cpu->gpr[11] + 7);         /* lbu t8,7(t3) */
    cpu->gpr[25] = 0x00800000u;                       /* lui t9,0x80 */
    cpu->gpr[25] = cpu->gpr[25] | 0x8080u;            /* ori t9,0x8080 */
    cpu->gpr[24] = cpu->gpr[24] << 24;                /* attr << 24 */
    cpu->gpr[24] = cpu->gpr[24] | cpu->gpr[25];       /* | 0x00808080 */
    cpu->gte_data[6] = cpu->gpr[24];                  /* mtc2 gte[6] attr */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24); /* lwc2 gte[0] */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    if ((int32_t)cpu->gpr[8] > 0) {
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28)); /* lwc2 gte[1] */
      cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);      /* lwc2 gte[2] */
      gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36)); /* lwc2 gte[3] */
      cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40);      /* lwc2 gte[4] */
      gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 44)); /* lwc2 gte[5] */
      gte_execute(cpu, 0x118043Fu);                   /* gte cmd 0x3F */
      br2_st32(cpu->gpr[11] + 8,  cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 20, 13);
      br2_st32(cpu->gpr[11] + 32, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 32, 14);
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);          /* old = *a1 (OT) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;              /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);       /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;               /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);      /* swl out+2 */
      br2_st32(cpu->gpr[11] + 4,  cpu->gte_data[20]); /* swc2 gte[20] */
      gte_precision_store_word(cpu->gpr[11] + 4, 20);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[21]); /* swc2 gte[21] */
      gte_precision_store_word(cpu->gpr[11] + 16, 21);
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 128;                 /* bgtz delay slot: a0 += 0x80 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018926c ======== */

/* prim_nclip_emit_a8 — NCLIP-gated GTE emitter, input record stride 168
 * (0xa8). Same gated prologue as prim_nclip_emit_80 (a2==0 -> t4=64, else
 * t4=64+52=116; attr int8_t -> gte_data[6]; NCLIP gate on data24). The body
 * is a multi-stage transform: lwc2 gte[1] @a0+28 then swc2 gte[12]->out+8
 * BEFORE gte cmd 0x01; lwc2 gte[2..5] @a0+40..52; swc2 gte[14]->out+44;
 * lwc2 gte[0]/[1] @a0+32/+36; gte cmd 0x118043F; swc2 gte[12]/[13]->
 * out+20/+32 and gte[20]->out+4; lwc2 gte[0]/[1] @a0+56/+60; gte cmd
 * 0x108041B; FIXED OT link at a1; swc2 gte[22]/[20]/[21] -> out+40/+16/+28.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_8018926C. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_nclip_emit_a8(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x8018926C: out-delta t4 = 64 (a2==0) else 116; per element NCLIP
   * gates on gte data24 > 0; a3-- mid-body, a0 += 168 in the bgtz delay
   * slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 64;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 52;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[24] = br2_ld8(cpu->gpr[11] + 7);         /* lbu t8,7(t3) */
    cpu->gpr[25] = 0x00800000u;                       /* lui t9,0x80 */
    cpu->gpr[25] = cpu->gpr[25] | 0x8080u;            /* ori t9,0x8080 */
    cpu->gpr[24] = cpu->gpr[24] << 24;                /* attr << 24 */
    cpu->gpr[24] = cpu->gpr[24] | cpu->gpr[25];       /* | 0x00808080 */
    cpu->gte_data[6] = cpu->gpr[24];                  /* mtc2 gte[6] attr */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24); /* lwc2 gte[0] */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    if ((int32_t)cpu->gpr[8] > 0) {
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28)); /* lwc2 gte[1] */
      br2_st32(cpu->gpr[11] + 8, cpu->gte_data[12]);  /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      gte_execute(cpu, 0x0180001u);                   /* gte cmd 0x01 */
      cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40);      /* lwc2 gte[2] */
      gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 44)); /* lwc2 gte[3] */
      cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 48);      /* lwc2 gte[4] */
      gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 52)); /* lwc2 gte[5] */
      br2_st32(cpu->gpr[11] + 44, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 44, 14);
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);      /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36)); /* lwc2 gte[1] */
      gte_execute(cpu, 0x118043Fu);                   /* gte cmd 0x3F */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 20, 12);
      br2_st32(cpu->gpr[11] + 32, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 32, 13);
      br2_st32(cpu->gpr[11] + 4,  cpu->gte_data[20]); /* swc2 gte[20] */
      gte_precision_store_word(cpu->gpr[11] + 4, 20);
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 56);      /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 60)); /* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 (OT) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 40, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 40, 22);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[20]); /* swc2 gte[20] */
      gte_precision_store_word(cpu->gpr[11] + 16, 20);
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[21]); /* swc2 gte[21] */
      gte_precision_store_word(cpu->gpr[11] + 28, 21);
    }
    cpu->gpr[4] = cpu->gpr[4] + 168;                  /* bgtz delay slot: a0 += 0xa8 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189360 ======== */

/* prim_nclip_emit_64 — NCLIP-gated GTE emitter, input record stride 100
 * (0x64). Same six-word prologue and NCLIP gate as the family but NO
 * attribute int8_t: out-delta t4 = 44 (a2==0) else 44+28=72; t3 = a0+t4 is
 * computed AFTER the data24 mfc2. MAC0<=0 skips. Otherwise a three-vertex
 * RTPS-style body: vertex 1 gets lwc2 gte[6] @a0+24 (DIRECT), gte[0]/[1]
 * @a0+36/+40, gte cmd 0x108041B, swc2 gte[12]/[13]/[14]->out+8/+16/+24 and
 * gte[22]->out+4; vertex 2 gets gte[6] @a0+28, gte cmd 0x108041B, the FIXED
 * OT link at a1, swc2 gte[22]->out+12; vertex 3 gets gte[6] @a0+32, gte cmd
 * 0x108041B, swc2 gte[22]->out+20. a0 += 100 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189360. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_nclip_emit_64(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189360: out-delta t4 = 44 (a2==0) else 72; per element NCLIP
   * gates on gte data24 > 0; a3-- mid-body, a0 += 100 in the bgtz delay
   * slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 44;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 28;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);    /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36);    /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 8,  cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 16, 13);
      br2_st32(cpu->gpr[11] + 24, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 24, 14);
      br2_st32(cpu->gpr[11] + 4,  cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28);    /* lwc2 gte[6] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 (OT) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);    /* lwc2 gte[6] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 100;                  /* bgtz delay slot: a0 += 0x64 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189428 ======== */

/* prim_nclip_emit_74 — NCLIP-gated GTE emitter, input record stride 116
 * (0x74). Same gate as prim_nclip_emit_64 (t4 = 60, else 60+28=88; no
 * attribute int8_t; t3 after the data24 mfc2). Three-vertex body with wider
 * per-vertex data: v1 = gte[6] @a0+24 + gte[0]/[1] @a0+36/+40 + cmd 0x1B,
 * swc2 gte[12]/[13]/[14]->out+8/+16/+24, gte[22]->out+4; v2 = gte[6] @a0+28
 * + gte[0]/[1] @a0+44/+48 + cmd 0x1B, FIXED OT link at a1, gte[22]->out+12;
 * v3 = gte[6] @a0+32 + gte[0]/[1] @a0+52/+56 + cmd 0x1B, gte[22]->out+20.
 * a0 += 116 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189428. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_nclip_emit_74(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189428: out-delta t4 = 60 (a2==0) else 88; per element NCLIP
   * gates on gte data24 > 0; a3-- mid-body, a0 += 116 in the bgtz delay
   * slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 60;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 28;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);    /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36);    /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 8,  cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 16, 13);
      br2_st32(cpu->gpr[11] + 24, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 24, 14);
      br2_st32(cpu->gpr[11] + 4,  cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28);    /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 44);    /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 48));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 (OT) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);    /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 52);    /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 56));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 116;                  /* bgtz delay slot: a0 += 0x74 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189500 ======== */

/* prim_nclip_emit_80b — NCLIP-gated GTE emitter, input record stride 128
 * (0x80), second variant (sibling of prim_nclip_emit_80 at 0x801891A4).
 * Same gate (t4 = 56, else 56+36=92; no attribute int8_t; t3 after the
 * data24 mfc2). Quirky body: mfc2 gpr[8] = gte_data[12] FIRST (before any
 * loads — reads the previous AVSZ-class result), then lwc2 gte[0]/[1]
 * @a0+24/+28, gte cmd 0x0180001, and a PLAIN sw of gpr[8] -> out+8 (not
 * swc2 — no precision side-call); swc2 gte[14]->out+32. Then three
 * vertex stages: v2 = gte[6] @a0+32 + gte[0]/[1] @a0+48/+52 + cmd 0x1B,
 * swc2 gte[12]/[13]->out+16/+24, gte[22]->out+4; v3 = gte[6] @a0+36 + cmd
 * 0x1B, FIXED OT link at a1, gte[22]->out+12; v4 = gte[6] @a0+40 + cmd 0x1B,
 * gte[22]->out+20; v5 = gte[6] @a0+44 + cmd 0x1B, gte[22]->out+28.
 * a0 += 128 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189500. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_nclip_emit_80b(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189500: out-delta t4 = 56 (a2==0) else 92; per element NCLIP
   * gates on gte data24 > 0; a3-- mid-body, a0 += 128 in the bgtz delay
   * slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 56;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gpr[8]  = cpu->gte_data[12];              /* mfc2 t0, data12 */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);     /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28));/* lwc2 gte[1] */
      gte_execute(cpu, 0x0180001u);                   /* gte cmd 0x01 */
      br2_st32(cpu->gpr[11] + 8, cpu->gpr[8]);        /* sw t0,8(t3) */
      br2_st32(cpu->gpr[11] + 32, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 32, 14);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);     /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 48);     /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 52));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 16, 12);
      br2_st32(cpu->gpr[11] + 24, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 24, 13);
      br2_st32(cpu->gpr[11] + 4,  cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36);     /* lwc2 gte[6] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 (OT) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40);     /* lwc2 gte[6] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 44);     /* lwc2 gte[6] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 128;                  /* bgtz delay slot: a0 += 0x80 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x801895f8 ======== */

/* prim_nclip_emit_98 — NCLIP-gated GTE emitter, input record stride 152
 * (0x98). Same gate (t4 = 80, else 80+36=116; no attribute int8_t; t3 after
 * the data24 mfc2). Body leads with swc2 gte[12]->out+8 BEFORE any loads,
 * then a four-stage vertex transform: v1 = lwc2 gte[0]/[1] @a0+24/+28 +
 * gte[6] @a0+32 + gte[0]/[1] @a0+48/+52 + cmd 0x1B, swc2 gte[12]/[13]/[14]
 * ->out+16/+24/+32, gte[22]->out+4; v2 = gte[6] @a0+36 + gte[0]/[1]
 * @a0+56/+60 + cmd 0x1B, FIXED OT link at a1, gte[22]->out+12; v3 =
 * gte[6] @a0+40 + gte[0]/[1] @a0+64/+68 + cmd 0x1B, gte[22]->out+20; v4 =
 * gte[6] @a0+44 + gte[0]/[1] @a0+72/+76 + cmd 0x1B, gte[22]->out+28.
 * a0 += 152 per element.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801895F8. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_nclip_emit_98(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x801895F8: out-delta t4 = 80 (a2==0) else 116; per element NCLIP
   * gates on gte data24 > 0; a3-- mid-body, a0 += 152 in the bgtz delay
   * slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 80;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* gte cmd 0x30 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,ctrl31 */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];                 /* mfc2 t0, data24 */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* t3 = a0+t4 (out) */
    if ((int32_t)cpu->gpr[8] > 0) {
      br2_st32(cpu->gpr[11] + 8, cpu->gte_data[12]);  /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 8, 12);
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24);     /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28));/* lwc2 gte[1] */
      gte_execute(cpu, 0x0180001u);                   /* gte cmd 0x01 */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 32);     /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 48);     /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 52));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[12]); /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[11] + 16, 12);
      br2_st32(cpu->gpr[11] + 24, cpu->gte_data[13]); /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[11] + 24, 13);
      br2_st32(cpu->gpr[11] + 32, cpu->gte_data[14]); /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11] + 32, 14);
      br2_st32(cpu->gpr[11] + 4,  cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 36);     /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 56);     /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 60));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);           /* old = *a1 (OT) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;               /* out << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);        /* swl a1+2 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;                /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);       /* swl out+2 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 40);     /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 64);     /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 68));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 44);     /* lwc2 gte[6] */
      cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 72);     /* lwc2 gte[0] */
      gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 76));/* lwc2 gte[1] */
      gte_execute(cpu, 0x108041Bu);                   /* gte cmd 0x1B */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]); /* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 152;                  /* bgtz delay slot: a0 += 0x98 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189704 ======== */

/* prim_poly_emit_3c — polygon/face GTE emitter, input stride 60
 * (0x3c). Sibling of prim_poly_emit_64 (0x8018866C): a0 = record cursor
 * (advances by 60 per element, returned in v0), a1 = OT base,
 * a2 = out-delta select (0 -> t4=20, else t4=20+20=40), a3 = signed
 * count (<=0 -> return cursor unchanged).
 *
 * Per record: three vertex-pointer words @a0+0/+4/+8 are dereferenced and
 * loaded as raw xyz into GTE V0 (mtc2 data 12/13/14), NCLIP (0x1400006)
 * writes the MAC0-class flag into data[24]; the three source xyz words are
 * stored raw at out+8/+12/+16 and the vertex ptr lw(a0+16) is fetched
 * BEFORE the data24 mfc2. MAC0<=0 skips the emit body. Otherwise:
 * lwc2 gte[6] = a0+12 (DIRECT gte_data[] write); vertex packed as
 * lhu x @v+0 -> data9, lhu y @v+2 -> data10, lwc2 data11 = v+4; RTPS
 * (0x138041C); FIXED OT link at a1 (lw *a1, swl pair — NO OTZ index);
 * swc2 gte[22] -> out+4.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189704. */
uint32_t prim_poly_emit_3c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189704: out-delta t4 = 20 (a2==0) else 40; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 60 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 20;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 20;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 12, cpu->gpr[9]);       /* sw t1,12(t3) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 16);      /* v1 ptr      */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[10]);      /* sw t2,16(t3) */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12); /* lwc2 gte[6] */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[8]);         /* lhu t1,0(v)  */
      cpu->gpr[10] = br2_ld16(cpu->gpr[8] + 2);     /* lhu t2,2(v)  */
      gte_write_data(cpu, 9, cpu->gpr[9]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 60;                 /* bgtz delay slot: a0 += 0x3c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x801897b0 ======== */

/* prim_poly_emit_54 — polygon/face GTE emitter, input stride 84
 * (0x54). Sibling of prim_poly_emit_3c (0x80189704): a0 = record cursor
 * (advances by 84 per element, returned in v0), a1 = OT base,
 * a2 = out-delta select (0 -> t4=28, else t4=28+28=56), a3 = signed
 * count (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+16/+24 (8-int8_t spacing);
 * vertex ptrs at a0+16/+20/+24. MAC0<=0 skips. Otherwise a THREE-VERTEX
 * RTPS body: v1 gets lwc2 gte[6] = a0+12 then (lhu x @v+0, lhu y @v+2 ->
 * data9/10, lwc2 data11 = v+4); v2/v3 reuse the same format with ptrs
 * loaded one vertex ahead. Each RTPS is followed by swc2 gte[22] ->
 * out+4/+12/+20; the third vertex's swc2 lands after the FIXED OT link
 * at a1 (lw *a1, swl pair out+2 FIRST then a1+2 — generated order).
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801897B0. */
uint32_t prim_poly_emit_54(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x801897B0: out-delta t4 = 28 (a2==0) else 56; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 84 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 28;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 28;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);       /* sw t1,16(t3) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 16);      /* v1 ptr      */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);      /* sw t2,24(t3) */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12); /* lwc2 gte[6] */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[8]);         /* lhu t1,0(v1) */
      cpu->gpr[10] = br2_ld16(cpu->gpr[8] + 2);     /* lhu t2,2(v1) */
      gte_write_data(cpu, 9, cpu->gpr[9]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 20);    /* v2 ptr      */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v2) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v2) */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 24);    /* v3 ptr      */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v3) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v3) */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 84;                 /* bgtz delay slot: a0 += 0x54 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x801898a0 ======== */

/* prim_poly_emit_50 — polygon/face GTE emitter, input stride 80
 * (0x50). Sibling of prim_poly_emit_3c/54: a0 = record cursor (advances
 * by 80 per element, returned in v0), a1 = OT base, a2 = out-delta
 * select (0 -> t4=16, else t4=16+32=48), a3 = signed count
 * (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+16/+24; the attribute
 * constant t2 = 0x00800000 is built BEFORE the data24 blez (lui) and
 * OR'd with 0x8080 in the branch DELAY SLOT (always executes). Vertex
 * ptr = lw(a0+12); MAC0<=0 skips. Otherwise: lbu attr = lbu(out+7)
 * <<24 | 0x00808080 written DIRECT to gte_data[6]; vertex packed as
 * (lhu x @v+0 -> data9, lhu y @v+2 -> data10, lwc2 data11 = v+4); RTPS
 * (0x138041C); FIXED OT link at a1 (swl pair); swc2 gte[22] -> out+4.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_801898A0. */
uint32_t prim_poly_emit_50(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x801898A0: out-delta t4 = 16 (a2==0) else 48; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 80 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 16;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 32;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);       /* sw t1,16(t3) */
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);      /* sw t2,24(t3) */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    cpu->gpr[10] = 0x00800000u;                     /* lui t2,0x80  */
    cpu->gpr[10] = cpu->gpr[10] | 0x8080u;          /* blez delay slot: ori t2 */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gpr[9]  = br2_ld8(cpu->gpr[11] + 7);     /* lbu t1,7(t3) */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 12);    /* v1 ptr      */
      cpu->gpr[9]  = cpu->gpr[9] << 24;             /* attr << 24 */
      cpu->gpr[9]  = cpu->gpr[9] | cpu->gpr[10];    /* | 0x00808080 */
      cpu->gte_data[6] = cpu->gpr[9];               /* mtc2 gte[6] attr */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[8]);         /* lhu t1,0(v)  */
      cpu->gpr[10] = br2_ld16(cpu->gpr[8] + 2);     /* lhu t2,2(v)  */
      gte_write_data(cpu, 9, cpu->gpr[9]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 80;                 /* bgtz delay slot: a0 += 0x50 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018995c ======== */

/* prim_poly_emit_68 — polygon/face GTE emitter, input stride 104
 * (0x68). Sibling of prim_poly_emit_50: a0 = record cursor (advances
 * by 104 per element, returned in v0), a1 = OT base, a2 = out-delta
 * select (0 -> t4=24, else t4=24+40=64), a3 = signed count
 * (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+20/+32 (12-int8_t
 * spacing); lbu attr int8_t read BEFORE the data24 mfc2, attr constant
 * lui in the blez DELAY SLOT. Vertex ptrs at a0+12/+16/+20. MAC0<=0
 * skips. Otherwise a THREE-VERTEX RTPS body: v1's (lhu x @v+0, lhu y
 * @v+2 -> data9/10, lwc2 data11 = v+4) is set up first, then each
 * subsequent vertex's halves are loaded right after the previous
 * RTPS (interleaved before that vertex's swc2). gte_data[6] = attr
 * (lbu(out+7)<<24 | 0x00808080) written DIRECT once; swc2 gte[22]
 * -> out+4/+16/+28; FIXED OT link at a1 before the last swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_8018995C. */
uint32_t prim_poly_emit_68(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x8018995C: out-delta t4 = 24 (a2==0) else 64; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 104 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 24;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 40;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 20, cpu->gpr[9]);       /* sw t1,20(t3) */
    cpu->gpr[9]  = br2_ld8(cpu->gpr[11] + 7);       /* lbu t1,7(t3) */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 32, cpu->gpr[10]);      /* sw t2,32(t3) */
    cpu->gpr[10] = 0x00800000u;                     /* blez delay slot: lui t2,0x80 */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gpr[10] = cpu->gpr[10] | 0x8080u;        /* ori t2,0x8080 */
      cpu->gpr[9]  = cpu->gpr[9] << 24;             /* attr << 24 */
      cpu->gpr[9]  = cpu->gpr[9] | cpu->gpr[10];    /* | 0x00808080 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 12);    /* v1 ptr      */
      cpu->gte_data[6] = cpu->gpr[9];               /* mtc2 gte[6] attr */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[8]);         /* lhu t1,0(v1) */
      cpu->gpr[10] = br2_ld16(cpu->gpr[8] + 2);     /* lhu t2,2(v1) */
      gte_write_data(cpu, 9, cpu->gpr[9]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 16);    /* v2 ptr      */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v2) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v2) */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 20);    /* v3 ptr      */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v3) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v3) */
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 16, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 104;                /* bgtz delay slot: a0 += 0x68 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189a60 ======== */

/* prim_poly_emit_54b — polygon/face GTE emitter, input stride 84
 * (0x54), second variant (sibling of prim_poly_emit_54 at 0x801897B0).
 * a0 = record cursor (advances by 84 per element, returned in v0),
 * a1 = OT base, a2 = out-delta select (0 -> t4=28, else t4=28+28=56),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+16/+24. Vertex data
 * ptr = lw(a0+24) points at a packed 3xu16 position (x @+0, y @+2, z
 * @+4); per-vertex attribute words are INLINE in the record at
 * a0+12/+16/+20 (lwc2 gte[6], DIRECT each stage) — NOT behind the
 * vertex ptrs. The SAME x/y/z halves (t6/t7/t8) are mtc2'd into
 * data9/10/11 for all THREE RTPS passes (generated re-loads nothing);
 * swc2 gte[22] -> out+4/+12/+20; FIXED OT link at a1 before the last
 * swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189A60. */
uint32_t prim_poly_emit_54b(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189A60: out-delta t4 = 28 (a2==0) else 56; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 84 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 28;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 28;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);       /* sw t1,16(t3) */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);      /* sw t2,24(t3) */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 24);    /* t9 = packed vtx */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12); /* lwc2 gte[6] */
      cpu->gpr[14] = br2_ld16(cpu->gpr[25]);        /* lhu t6,0(t9)  */
      cpu->gpr[15] = br2_ld16(cpu->gpr[25] + 2);    /* lhu t7,2(t9)  */
      cpu->gpr[24] = br2_ld16(cpu->gpr[25] + 4);    /* lhu t8,4(t9)  */
      gte_write_data(cpu, 9, cpu->gpr[14]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[15]);        /* mtc2 */
      gte_write_data(cpu, 11, cpu->gpr[24]);        /* mtc2 */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] */
      gte_write_data(cpu, 9, cpu->gpr[14]);         /* mtc2 (same) */
      gte_write_data(cpu, 10, cpu->gpr[15]);        /* mtc2 (same) */
      gte_write_data(cpu, 11, cpu->gpr[24]);        /* mtc2 (same) */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20); /* lwc2 gte[6] */
      gte_write_data(cpu, 9, cpu->gpr[14]);         /* mtc2 (same) */
      gte_write_data(cpu, 10, cpu->gpr[15]);        /* mtc2 (same) */
      gte_write_data(cpu, 11, cpu->gpr[24]);        /* mtc2 (same) */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 84;                 /* bgtz delay slot: a0 += 0x54 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189b50 ======== */

/* prim_poly_emit_5c — polygon/face GTE emitter, input stride 92
 * (0x5c). Sibling of prim_poly_emit_54b: a0 = record cursor (advances
 * by 92 per element, returned in v0), a1 = OT base, a2 = out-delta
 * select (0 -> t4=36, else t4=36+28=64), a3 = signed count
 * (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+16/+24; vertex ptr
 * v1 = lw(a0+24) fetched BEFORE the data24 mfc2. MAC0<=0 skips.
 * Otherwise a THREE-VERTEX RTPS body mixing packed record attrs with
 * per-vertex ptrs: v1 = lwc2 gte[6] @a0+12 + (lhu x @v+0, lhu y @v+2
 * -> data9/10, lwc2 data11 = v+4); v2 ptr = lw(a0+28), v3 ptr =
 * lw(a0+32); each stage reloads lwc2 gte[6] DIRECT from a0+12/+16/+20
 * (inline record words) while position halves come from *ptr. swc2
 * gte[22] -> out+4/+12/+20; FIXED OT link at a1 before the last swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189B50. */
uint32_t prim_poly_emit_5c(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189B50: out-delta t4 = 36 (a2==0) else 64; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 92 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 36;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 28;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);       /* sw t1,16(t3) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[4] + 24);      /* v1 ptr      */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);      /* sw t2,24(t3) */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12); /* lwc2 gte[6] */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[8]);         /* lhu t1,0(v1) */
      cpu->gpr[10] = br2_ld16(cpu->gpr[8] + 2);     /* lhu t2,2(v1) */
      gte_write_data(cpu, 9, cpu->gpr[9]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[10]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[8] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 28);    /* v2 ptr      */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v2) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v2) */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] */
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 32);    /* v3 ptr      */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v3) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v3) */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20); /* lwc2 gte[6] */
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 92;                 /* bgtz delay slot: a0 += 0x5c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189c4c ======== */

/* prim_point_emit_48 — single-vertex (POINT) GTE emitter, input stride
 * 72 (0x48). a0 = record cursor (advances by 72 per element, returned
 * in v0), a1 = OT base, a2 = out-delta select (0 -> t4=24, else
 * t4=24+24=48), a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+12/+16 (4-int8_t
 * spacing). MAC0<=0 skips. Otherwise ONE RTPS vertex: attr word =
 * lwc2 gte[6] DIRECT from a0+16; companion raw word t0 = lw(*t9) with
 * t9 = lw(a0+12) stored plain-sw at out+20; position ptr t2 = lw(a0+20)
 * split into lhu x @+0 / lhu y @+2 -> data9/10, lwc2 data11 = t2+4.
 * swc2 gte[22] -> out+4; FIXED OT link at a1 before the swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189C4C. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_point_emit_48(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189C4C: out-delta t4 = 24 (a2==0) else 48; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 72 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 24;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 24;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 12, cpu->gpr[9]);       /* sw t1,12(t3) */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[10]);      /* sw t2,16(t3) */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);    /* t9 = raw ptr */
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] DIRECT */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[25]);        /* t0 = *t9     */
      cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 20);    /* vtx pos ptr */
      br2_st32(cpu->gpr[11] + 20, cpu->gpr[8]);     /* sw t0,20(t3) */
      cpu->gpr[8]  = br2_ld16(cpu->gpr[10]);        /* lhu t0,0(v)  */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[10] + 2);    /* lhu t1,2(v)  */
      gte_write_data(cpu, 9, cpu->gpr[8]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[9]);         /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[10] + 4)); /* lwc2 gte[11] */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 72;                 /* bgtz delay slot: a0 += 0x48 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189d08 ======== */

/* prim_poly_emit_6cb — polygon/face GTE emitter, input stride 0x6c
 * (108), fixed-link variant (sibling of prim_poly_emit_6c at
 * 0x80188510 which uses the x-split/AVSZ3/OTZ tail instead).
 * a0 = record cursor (advances by 108 per element, returned in v0),
 * a1 = OT base, a2 = out-delta select (0 -> t4=36, else t4=36+36=72),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+16/+24 (8-int8_t
 * spacing); companion t9 = lw(a0+12) fetched BEFORE the data24 mfc2,
 * *t9 stored plain-sw at out+32 inside the body. MAC0<=0 skips.
 * Otherwise a FOUR-VERTEX RTPS body with a SINGLE flat attribute:
 * gte_data[6] = lwc2 gte[6] DIRECT from a0+16 once; v1 pos ptr =
 * lw(a0+20) split lhu x @+0 / lhu y @+2 -> data9/10 with lwc2 data11 =
 * ptr+4; v2/v3/v4 pos ptrs = lw(a0+24/+28/+32), each stage reloading
 * lhu halves AFTER the previous RTPS (interleaved before that vertex's
 * swc2). swc2 gte[22] -> out+4/+12/+20/+28; FIXED OT link at a1 before
 * the last swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189D08. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_poly_emit_6cb(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189D08: out-delta t4 = 36 (a2==0) else 72; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 108 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 36;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);       /* sw t1,16(t3) */
    cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);      /* t9 = comp   */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);      /* sw t2,24(t3) */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] DIRECT */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[25]);        /* t0 = *t9     */
      cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 20);    /* v1 pos ptr  */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[8]);     /* sw t0,32(t3) */
      cpu->gpr[8]  = br2_ld16(cpu->gpr[10]);        /* lhu t0,0(v1) */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[10] + 2);    /* lhu t1,2(v1) */
      gte_write_data(cpu, 9, cpu->gpr[8]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[9]);         /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[10] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 24);    /* v2 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v2) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v2) */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 28);    /* v3 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v3) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v3) */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 32);    /* v4 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v4) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v4) */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v4 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 108;                /* bgtz delay slot: a0 += 0x6c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x80189efc ======== */

/* prim_poly_emit_88b — polygon/face GTE emitter, input stride 0x88
 * (136), second fixed-link variant (sibling of prim_poly_emit_88 at
 * 0x80188754 which uses the x-split/AVSZ3/OTZ tail instead).
 * a0 = record cursor (advances by 136 per element, returned in v0),
 * a1 = OT base, a2 = out-delta select (0 -> t4=32, else t4=32+52=84),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz stored at out+8/+20/+32 (24-int8_t
 * spacing); lbu attr int8_t = lbu(out+7) read BEFORE the data24 mfc2,
 * attr constant lui 0x0080 in the blez DELAY SLOT. Vertex ptrs at
 * a0+16/+20/+24/+28; companion t9 = lw(a0+12), *t9 = lw stored
 * plain-sw at out+44. FOUR RTPS vertices (lhu x @+0 / lhu y @+2 ->
 * data9/10, lwc2 data11 = ptr+4), attr word (attr<<24 | 0x00808080)
 * written DIRECT to gte_data[6] once; swc2 gte[22] ->
 * out+4/+16/+28/+40; FIXED OT link at a1 before the last swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_80189EFC. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_poly_emit_88b(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x80189EFC: out-delta t4 = 32 (a2==0) else 84; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 136 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 52;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 20, cpu->gpr[9]);       /* sw t1,20(t3) */
    cpu->gpr[9]  = br2_ld8(cpu->gpr[11] + 7);       /* lbu t1,7(t3) */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 32, cpu->gpr[10]);      /* sw t2,32(t3) */
    cpu->gpr[10] = 0x00800000u;                     /* blez delay slot: lui t2,0x80 */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gpr[10] = cpu->gpr[10] | 0x8080u;        /* ori t2,0x8080 */
      cpu->gpr[9]  = cpu->gpr[9] << 24;             /* attr << 24 */
      cpu->gpr[9]  = cpu->gpr[9] | cpu->gpr[10];    /* | 0x00808080 */
      cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);    /* t9 = comp   */
      cpu->gte_data[6] = cpu->gpr[9];               /* mtc2 gte[6] attr DIRECT */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[25]);        /* t0 = *t9     */
      cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 16);    /* v1 pos ptr  */
      br2_st32(cpu->gpr[11] + 44, cpu->gpr[8]);     /* sw t0,44(t3) */
      cpu->gpr[8]  = br2_ld16(cpu->gpr[10]);        /* lhu t0,0(v1) */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[10] + 2);    /* lhu t1,2(v1) */
      gte_write_data(cpu, 9, cpu->gpr[8]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[9]);         /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[10] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 20);    /* v2 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v2) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v2) */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 24);    /* v3 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v3) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v3) */
      br2_st32(cpu->gpr[11] + 16, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 16, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 28);    /* v4 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v4) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v4) */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v4 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 40, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 40, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 136;                /* bgtz delay slot: a0 += 0x88 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a030 ======== */

/* prim_poly_emit_6cc — polygon/face GTE emitter, input stride 0x6c
 * (108), packed-vertex variant (sibling of prim_poly_emit_6cb).
 * a0 = record cursor (advances by 108 per element, returned in v0),
 * a1 = OT base, a2 = out-delta select (0 -> t4=36, else t4=36+36=72),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz at out+8/+16/+24; companion t9 =
 * lw(a0+12), *t9 stored plain-sw at out+32. MAC0<=0 skips.
 * Otherwise a FOUR-RTPS body sharing ONE packed vertex position:
 * pos ptr = lw(a0+32) -> THREE lhu halves (x @+0, y @+2, z @+4) ->
 * data9/10/11 via mtc2 (no lwc2 for position); per-vertex attribute
 * lwc2 gte[6] DIRECT reloaded from a0+16/+20/+24/+28 before each RTPS
 * while the SAME xyz halves (t6/t7/t8) feed all four passes.
 * swc2 gte[22] -> out+4/+12/+20/+28; FIXED OT link at a1 before the
 * last swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_8018A030. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_poly_emit_6cc(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x8018A030: out-delta t4 = 36 (a2==0) else 72; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 108 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 36;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);       /* sw t1,16(t3) */
    cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);      /* t9 = comp   */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);      /* sw t2,24(t3) */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] DIRECT */
      cpu->gpr[13] = br2_ld32(cpu->gpr[25]);        /* t5 = *t9     */
      cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 32);    /* pos ptr     */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[13]);    /* sw t5,32(t3) */
      cpu->gpr[14] = br2_ld16(cpu->gpr[24]);        /* lhu t6,0(v)  */
      cpu->gpr[15] = br2_ld16(cpu->gpr[24] + 2);    /* lhu t7,2(v)  */
      cpu->gpr[24] = br2_ld16(cpu->gpr[24] + 4);    /* lhu t8,4(v)  */
      gte_write_data(cpu, 9, cpu->gpr[14]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[15]);        /* mtc2 */
      gte_write_data(cpu, 11, cpu->gpr[24]);        /* mtc2 */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20); /* lwc2 gte[6] DIRECT */
      gte_write_data(cpu, 9, cpu->gpr[14]);         /* mtc2 (same) */
      gte_write_data(cpu, 10, cpu->gpr[15]);        /* mtc2 (same) */
      gte_write_data(cpu, 11, cpu->gpr[24]);        /* mtc2 (same) */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24); /* lwc2 gte[6] DIRECT */
      gte_write_data(cpu, 9, cpu->gpr[14]);         /* mtc2 (same) */
      gte_write_data(cpu, 10, cpu->gpr[15]);        /* mtc2 (same) */
      gte_write_data(cpu, 11, cpu->gpr[24]);        /* mtc2 (same) */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28); /* lwc2 gte[6] DIRECT */
      gte_write_data(cpu, 9, cpu->gpr[14]);         /* mtc2 (same) */
      gte_write_data(cpu, 10, cpu->gpr[15]);        /* mtc2 (same) */
      gte_write_data(cpu, 11, cpu->gpr[24]);        /* mtc2 (same) */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v4 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 108;                /* bgtz delay slot: a0 += 0x6c */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a14c ======== */

/* prim_poly_emit_78b — polygon/face GTE emitter, input stride 0x78
 * (120), untextured fixed-link variant (sibling of the textured
 * prim_poly_emit_78 at 0x801889F4).
 * a0 = record cursor (advances by 120 per element, returned in v0),
 * a1 = OT base, a2 = out-delta select (0 -> t4=48, else t4=48+36=84),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Same NCLIP prologue; raw xyz at out+8/+16/+24; companion t9 =
 * lw(a0+12), *t9 stored plain-sw at out+32. MAC0<=0 skips.
 * Otherwise a FOUR-VERTEX RTPS body: v1 pos ptr = lw(a0+32) split
 * lhu x @+0 / lhu y @+2 -> data9/10, lwc2 data11 = ptr+4; v2/v3/v4
 * pos ptrs = lw(a0+36/+40/+44), halves reloaded after each RTPS;
 * per-vertex attribute lwc2 gte[6] DIRECT reloaded from
 * a0+16/+20/+24/+28 before each RTPS. swc2 gte[22] ->
 * out+4/+12/+20/+28; FIXED OT link at a1 before the last swc2.
 * Disasm generated/SLES_017.22_full_40.c br2eu_func_8018A14C. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_poly_emit_78b(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* Guest 0x8018A14C: out-delta t4 = 48 (a2==0) else 84; per element NCLIP
   * gates on gte data24 > 0 (kept in gpr[13]); a3-- mid-body, a0 += 120 in
   * the bgtz delay slot, loop while a3 > 0 signed. */
  if ((int32_t)cpu->gpr[7] <= 0) {
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 48;                                /* beq delay slot */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 36;
  do {
    cpu->gpr[14] = br2_ld32(cpu->gpr[4]);           /* t6 = *a0     */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 4);       /* t7 = *(a0+4) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 8);       /* t8 = *(a0+8) */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[14]);          /* t0 = *t6     */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[15]);          /* t1 = *t7     */
    cpu->gpr[10] = br2_ld32(cpu->gpr[24]);          /* t2 = *t8     */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    br2_st32(cpu->gpr[11] + 8,  cpu->gpr[8]);       /* sw t0,8(t3)  */
    br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);       /* sw t1,16(t3) */
    cpu->gpr[25] = br2_ld32(cpu->gpr[4] + 12);      /* t9 = comp   */
    cpu->gpr[13] = cpu->gte_data[24];               /* mfc2 t5,d24 */
    br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);      /* sw t2,24(t3) */
    if ((int32_t)cpu->gpr[13] > 0) {
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16); /* lwc2 gte[6] DIRECT */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[25]);        /* t0 = *t9     */
      cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 32);    /* v1 pos ptr  */
      br2_st32(cpu->gpr[11] + 32, cpu->gpr[8]);     /* sw t0,32(t3) */
      cpu->gpr[8]  = br2_ld16(cpu->gpr[10]);        /* lhu t0,0(v1) */
      cpu->gpr[9]  = br2_ld16(cpu->gpr[10] + 2);    /* lhu t1,2(v1) */
      gte_write_data(cpu, 9, cpu->gpr[8]);          /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[9]);         /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[10] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 36);    /* v2 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v1 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v2) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v2) */
      br2_st32(cpu->gpr[11] + 4, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 4, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20); /* lwc2 gte[6] DIRECT */
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 40);    /* v3 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v2 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v3) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v3) */
      br2_st32(cpu->gpr[11] + 12, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 12, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 24); /* lwc2 gte[6] DIRECT */
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      cpu->gpr[14] = br2_ld32(cpu->gpr[4] + 44);    /* v4 ptr      */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v3 */
      cpu->gpr[15] = br2_ld16(cpu->gpr[14]);        /* lhu t7,0(v4) */
      cpu->gpr[24] = br2_ld16(cpu->gpr[14] + 2);    /* lhu t8,2(v4) */
      br2_st32(cpu->gpr[11] + 20, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 20, 22);
      cpu->gte_data[6] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 28); /* lwc2 gte[6] DIRECT */
      gte_write_data(cpu, 9, cpu->gpr[15]);         /* mtc2 */
      gte_write_data(cpu, 10, cpu->gpr[24]);        /* mtc2 */
      gte_write_data(cpu, 11, psx_cyc_lwc2_read(cpu, cpu->gpr[14] + 4)); /* lwc2 gte[11] */
      /* nop */
      /* nop */
      gte_execute(cpu, 0x138041Cu);                 /* RTPS v4 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* old = *a1   */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* out << 8   */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2    */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8   */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl out+2   */
      br2_st32(cpu->gpr[11] + 28, cpu->gte_data[22]);/* swc2 gte[22] */
      gte_precision_store_word(cpu->gpr[11] + 28, 22);
    }
    cpu->gpr[4] = cpu->gpr[4] + 120;                /* bgtz delay slot: a0 += 0x78 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a27c ======== */

/* prim_xform_emit_64 — RTPT-based GTE transform/emit helper, input
 * record stride 64 (0x40). MULTI-ENTRY guest fn: only entry
 * 0x8018A27C is ported (sets t6=20 out-delta extra, t7=64 record
 * stride, t8=4 output spacing); sibling entries 0x8018A28C/29C/2AC
 * (stride 80/88/104) remain generated-authoritative.
 * a0 = record cursor (advances by t7=64 per element, returned in v0),
 * a1 = OT base, a2 = out-delta select (0 -> t4=24, else t4=24+t6=44),
 * a3 = signed count (<=0 -> return cursor unchanged).
 *
 * Per record: lwc2 the six input words into gte[0..5] — regs 0/2/4
 * DIRECT cpu->gte_data writes, regs 1/3/5 via gte_write_data (masked);
 * a3--; RTPT (0x0280030). Out record at a0+t4: t1=out+8, t2=t1+t8,
 * t3=t2+t8; old = lw(a1), out<<8 / old<<8 kept live; cfc2 $2,$31 then
 * NCLIP; mfc2 d24. swc2 gte[12] -> t1 ALWAYS (even when culled);
 * MAC0<=0 skips the rest. Otherwise swc2 gte[13]/[14] -> t2/t3, then
 * the OT-link swl pair in REVERSED order vs the poly emitters:
 * swl(out+2, old<<8) FIRST, swl(a1+2, out<<8) SECOND. NOTE: gpr[14]
 * (t6 param) is clobbered by `out << 8` inside the body — faithful,
 * the guest reuses t6 per iteration.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018A27C. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_xform_emit_64(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018A27C: t6=20 (out-delta extra), t7=64 (stride),
   * t8=4 (out-word spacing) — set once before the shared body. */
  cpu->gpr[14] = 20;
  cpu->gpr[15] = 64;
  cpu->gpr[24] = 4;

  if ((int32_t)cpu->gpr[7] <= 0) {                 /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 24;                                /* beq delay slot: t4=24 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] DIRECT */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] DIRECT */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] DIRECT */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    gte_execute(cpu, 0x0280030u);                   /* RTPT */
    cpu->gpr[25] = cpu->gpr[4] + cpu->gpr[12];      /* t9 = a0+t4 (out) */
    cpu->gpr[9]  = cpu->gpr[25] + 8;                /* t1 = out+8    */
    cpu->gpr[10] = cpu->gpr[9] + cpu->gpr[24];      /* t2 = t1+t8    */
    cpu->gpr[11] = cpu->gpr[10] + cpu->gpr[24];     /* t3 = t2+t8    */
    cpu->gpr[13] = br2_ld32(cpu->gpr[5]);           /* t5 = *a1     */
    cpu->gpr[14] = cpu->gpr[25] << 8;               /* t6 = out << 8 */
    cpu->gpr[13] = cpu->gpr[13] << 8;               /* old << 8     */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);          /* cfc2 $2,$31  */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];               /* mfc2 t0,d24  */
    br2_st32(cpu->gpr[9],  cpu->gte_data[12]);      /* swc2 gte[12] */
    gte_precision_store_word(cpu->gpr[9], 12);
    if ((int32_t)cpu->gpr[8] > 0) {
      br2_st32(cpu->gpr[10], cpu->gte_data[13]);    /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[10], 13);
      br2_st32(cpu->gpr[11], cpu->gte_data[14]);    /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11], 14);
      swl_guest(cpu->gpr[25] + 2, cpu->gpr[13]);    /* swl out+2, old<<8 */
      swl_guest(cpu->gpr[5] + 2,   cpu->gpr[14]);   /* swl a1+2, out<<8  */
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];       /* bgtz delay slot: a0 += t7 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a340 ======== */

/* prim_persp_ot_emit_50 — RTPT/NCLIP perspective OT emitter, input record
 * stride 80 (0x50). MULTI-ENTRY guest fn: only entry 0x8018A340 is ported
 * (t6=24 out-delta extra, t7=80 stride, t8=4 out-word spacing); sibling
 * entries 0x8018A350/A360/A370 (stride 104/112/136, t6 36/40/52) remain
 * generated-authoritative via br2eu_func_8018A340's CPS pc-switch.
 * a0 = record cursor (advances by t7, returned in v0), a1 = OT base,
 * a2 = out-delta select (0 -> t4=32, else t4=32+t6=56), a3 = signed count.
 *
 * Per record: lwc2 gte[0..5] <- a0[0..5] (regs 0/2/4 DIRECT
 * cpu->gte_data writes, regs 1/3/5 via masked gte_write_data); a3--; RTPT
 * (0x0280030); t3=a0+t4 (out base), t5=t3+8; t1=ld32(a0+24),
 * t2=ld32(a0+28); cfc2 $2,$31; NCLIP; mfc2 d24 -> t0. MAC0<=0 skips the
 * emit. Otherwise: swc2 gte[12] -> t5 ALWAYS first, mtc2 gte[0]=t1 /
 * gte[1]=t2 (V0.xy/z), t5+=t8, RTPS (0x180001), then the OT-link swl pair
 * swl(a1+2, base<<8) / swl(base+2, old<<8), then t9=t5+t8, t3=t9+t8 and
 * swc2 gte[12]/[13]/[14] -> t5/t9/t3 (SXY0',SXY1,SXY2 at base+12/+16/+20).
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018A340. */
extern void gte_precision_store_word(uint32_t addr, uint8_t reg);

uint32_t prim_persp_ot_emit_50(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018A340: t6=24, t7=80, t8=4 */
  cpu->gpr[14] = 24;
  cpu->gpr[15] = 80;
  cpu->gpr[24] = 4;

  if ((int32_t)cpu->gpr[7] <= 0) {                 /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                                /* beq delay slot: t4=32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] DIRECT */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] DIRECT */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] DIRECT */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    gte_execute(cpu, 0x0280030u);                   /* RTPT */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out base) */
    cpu->gpr[13] = cpu->gpr[11] + 8;                /* t5 = base+8 */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 24);      /* t1 = lw 0x18(a0) */
    cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 28);      /* t2 = lw 0x1C(a0) */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);          /* cfc2 $2,$31 */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    cpu->gpr[8]  = cpu->gte_data[24];               /* mfc2 t0,d24 (MAC0) */
    if ((int32_t)cpu->gpr[8] > 0) {
      br2_st32(cpu->gpr[13], cpu->gte_data[12]);    /* swc2 gte[12] -> base+8 */
      gte_precision_store_word(cpu->gpr[13], 12);
      cpu->gte_data[0] = cpu->gpr[9];               /* mtc2 gte[0] = t1 (V0.xy) */
      gte_write_data(cpu, 1, cpu->gpr[10]);         /* mtc2 gte[1] = t2 (V0.z) */
      cpu->gpr[13] = cpu->gpr[13] + cpu->gpr[24];   /* t5 += t8 */
      gte_execute(cpu, 0x0180001u);                 /* RTPS: V0 -> SXY0 */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* t0 = *a1 (OT head) */
      cpu->gpr[9]  = cpu->gpr[11] << 8;             /* base << 8 */
      swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl a1+2, base<<8 */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8 */
      swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl base+2, old<<8 */
      cpu->gpr[25] = cpu->gpr[13] + cpu->gpr[24];   /* t9 = t5+t8 */
      cpu->gpr[11] = cpu->gpr[25] + cpu->gpr[24];   /* t3 = t9+t8 */
      br2_st32(cpu->gpr[13], cpu->gte_data[12]);    /* swc2 gte[12] */
      gte_precision_store_word(cpu->gpr[13], 12);
      br2_st32(cpu->gpr[25], cpu->gte_data[13]);    /* swc2 gte[13] */
      gte_precision_store_word(cpu->gpr[25], 13);
      br2_st32(cpu->gpr[11], cpu->gte_data[14]);    /* swc2 gte[14] */
      gte_precision_store_word(cpu->gpr[11], 14);
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];       /* bgtz delay slot: a0 += t7 (80) */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a424 ======== */

/* prim_vert3_ot_emit_40 — three-vertex OT emitter, input record stride
 * 64 (0x40). MULTI-ENTRY guest fn: only entry 0x8018A424 is ported
 * (t6=24, t7=64, t8=4); sibling entries 0x8018A434/A444/A454 (stride
 * 88/96/120, t6 36/40/52) remain generated-authoritative.
 * a0 = record cursor (advances by t7, returned in v0), a1 = OT base,
 * a2 = out-delta select (0 -> t4=16, else t4=16+t6=40), a3 = count.
 *
 * Per record: THREE vertex pointers at a0+0/+4/+8 are each dereferenced
 * (*v0,*v1,*v2) and mtc2'd into gte[12],[13],[14] via gte_write_data;
 * a3--; NCLIP; out=t3=a0+t4, t9=t3+8; the three vertex words are stored
 * plain-sw at t9,+t8,+2*t8 (base+8,+12,+16); mfc2 d24 -> t0; t9+=t8
 * (base+20); base<<8 kept live in t1. MAC0<=0 skips the emit tail.
 * Otherwise: t5=ld32(a0+12) -> *t5 attr word, t0=ld32(a1)<<8 (OT head),
 * sw t5 -> t9 (base+20), then swl(a1+2, base<<8) / swl(base+2, old<<8).
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018A424. */
uint32_t prim_vert3_ot_emit_40(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018A424: t6=24, t7=64, t8=4 */
  cpu->gpr[14] = 24;
  cpu->gpr[15] = 64;
  cpu->gpr[24] = 4;

  if ((int32_t)cpu->gpr[7] <= 0) {                 /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 16;                                /* beq delay slot: t4=16 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];
  do {
    cpu->gpr[8]  = br2_ld32(cpu->gpr[4]);           /* t0 = *(a0+0)  v0 ptr */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 4);       /* t1 = *(a0+4)  v1 ptr */
    cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 8);       /* t2 = *(a0+8)  v2 ptr */
    cpu->gpr[8]  = br2_ld32(cpu->gpr[8]);           /* t0 = *v0 */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[9]);           /* t1 = *v1 */
    cpu->gpr[10] = br2_ld32(cpu->gpr[10]);          /* t2 = *v2 */
    gte_write_data(cpu, 12, cpu->gpr[8]);           /* mtc2 gte[12] */
    gte_write_data(cpu, 13, cpu->gpr[9]);           /* mtc2 gte[13] */
    gte_write_data(cpu, 14, cpu->gpr[10]);          /* mtc2 gte[14] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (out base) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    cpu->gpr[25] = cpu->gpr[11] + 8;                /* t9 = base+8 */
    br2_st32(cpu->gpr[25], cpu->gpr[8]);            /* sw t0 -> t9 */
    cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[24];     /* t9 += t8 */
    br2_st32(cpu->gpr[25], cpu->gpr[9]);            /* sw t1 */
    cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[24];     /* t9 += t8 */
    br2_st32(cpu->gpr[25], cpu->gpr[10]);           /* sw t2 */
    cpu->gpr[8]  = cpu->gte_data[24];               /* mfc2 t0,d24 (MAC0) */
    cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[24];     /* t9 += t8 -> base+20 */
    cpu->gpr[9]  = cpu->gpr[11] << 8;               /* base << 8 (delay slot) */
    if ((int32_t)cpu->gpr[8] > 0) {
      cpu->gpr[13] = br2_ld32(cpu->gpr[4] + 12);    /* t5 = *(a0+12) attr ptr */
      cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* t0 = *a1 (OT head) */
      cpu->gpr[13] = br2_ld32(cpu->gpr[13]);        /* t5 = *attr ptr */
      cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8 */
      br2_st32(cpu->gpr[25], cpu->gpr[13]);         /* sw t5 -> base+20 */
      swl_guest(cpu->gpr[5] + 2,   cpu->gpr[9]);    /* swl a1+2, base<<8 */
      swl_guest(cpu->gpr[11] + 2,  cpu->gpr[8]);    /* swl base+2, old<<8 */
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];       /* bgtz delay slot: a0 += t7 (64) */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a59c ======== */

/* prim_persp_cull_emit_48 — RTPT/NCLIP emitter with screen-edge cull and
 * OT link, input record stride 72 (0x48). MULTI-ENTRY guest fn: only
 * entry 0x8018A59C is ported (t6=24, t7=72, t8=4); sibling entries
 * 0x8018A5AC/A5BC/A5CC/A5DC remain generated-authoritative.
 * a0 = record cursor (advances by t7, returned in v0), a1 = OT base,
 * a2 = out-delta select (0 -> t4=24, else t4=24+t6=48), a3 = count.
 *
 * Per record: lwc2 gte[0..5] <- a0[0..5]; a3--; RTPT (0x0280030);
 * cfc2 $2,$31; NCLIP; t3=a0+t4 (delay slot,
 * unconditional); FLAG&0x7FC40000 != 0 skips. Then out=base+8;
 * mfc2 MAC0/SXY0 — MAC0<=0 skips (SXY0&0xFFFF pre-computed in delay
 * slot); SXY1/SXY2 = gte[13]/[14]; four-way screen cull: sx via
 * psx_ws_cull_sltiu(x&0xFFFF,512) on all four SXY words (the 4th uses a
 * POST-check reload pattern — see below), sy via (x>>16)<480; both
 * accumulators must be nonzero. SXY0 stored at out UNCONDITIONALLY in
 * the second beq delay slot; then SXY1/SXY2 at out+t8/+2*t8; base
 * recomputed; OT swl pair swl(a1+2, base<<8)/swl(base+2, old<<8).
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018A59C. */
uint32_t prim_persp_cull_emit_48(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018A59C: t6=24, t7=72, t8=4 */
  cpu->gpr[14] = 24;
  cpu->gpr[15] = 72;
  cpu->gpr[24] = 4;

  if ((int32_t)cpu->gpr[7] <= 0) {                 /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 24;                                /* beq delay slot: t4=24 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] DIRECT */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] DIRECT */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] DIRECT */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    gte_execute(cpu, 0x0280030u);                   /* RTPT */
    cpu->gpr[25] = 0x7FC4u << 16;                    /* 0x7FC40000 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);          /* cfc2 $2,$31 (FLAG) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];      /* FLAG & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (delay slot) */
    if (cpu->gpr[25] == cpu->gpr[0]) {              /* bne mask,0 -> culled */
      cpu->gpr[11] = cpu->gpr[11] + 8;              /* out = base+8 */
      cpu->gpr[9]  = cpu->gte_data[24];             /* mfc2 t1,d24 (MAC0) */
      cpu->gpr[8]  = cpu->gte_data[12];             /* mfc2 t0,d12 (SXY0) */
      cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;         /* sx0 (delay slot) */
      if ((int32_t)cpu->gpr[9] > 0) {               /* blez MAC0 -> skip */
        cpu->gpr[9]  = cpu->gte_data[13];           /* mfc2 SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];           /* mfc2 SXY2 */
        /* sx cull: 3x psx_ws_cull_sltiu(x&0xFFFF,512) accumulated in t9 */
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[25] = (int32_t)cpu->gpr[8] >> 16;   /* sy0 (beq delay slot) */
        if (cpu->gpr[25] != 0) {                    /* beq gpr[25],0 -> culled */
          cpu->gpr[25] = (cpu->gpr[25] < (uint32_t)480) ? 1u : 0u;   /* sy0<480 */
          cpu->gpr[13] = (int32_t)cpu->gpr[9] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;   /* sy1<480 */
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
          cpu->gpr[13] = (int32_t)cpu->gpr[10] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;   /* sy2<480 */
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
          br2_st32(cpu->gpr[11], cpu->gpr[8]);        /* sw SXY0 -> out (unconditional) */
          if (cpu->gpr[25] != 0) {
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[9]);          /* sw SXY1 */
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[10]);         /* sw SXY2 */
            cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];    /* t3 = a0+t4 (base) */
            cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* t0 = *a1 (OT head) */
            cpu->gpr[9]  = cpu->gpr[11] << 8;             /* base << 8 */
            swl_guest(cpu->gpr[5] + 2,   cpu->gpr[9]);    /* swl a1+2, base<<8 */
            cpu->gpr[8]  = cpu->gpr[8] << 8;              /* old << 8 */
            swl_guest(cpu->gpr[11] + 2,  cpu->gpr[8]);    /* swl base+2, old<<8 */
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];       /* bgtz delay slot: a0 += t7 (72) */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a6d4 ======== */

/* prim_persp_cull_emit_58 — perspective screen-cull OT emitter with a
 * secondary RTPS'd vertex, input record stride 88 (0x58). MULTI-ENTRY
 * guest fn: only entry 0x8018A6D4 is ported (t6=28, t7=88, t8=4);
 * sibling entries 0x8018A6E4/A6F4/A704/A714 remain generated.
 * a0 = record cursor, a1 = OT base, a2 = out-delta select
 * (0 -> t4=32, else t4=32+t6=60), a3 = count.
 *
 * Per record: lwc2 gte[0..5]; a3--; RTPT; t1=ld32(a0+24),
 * t2=ld32(a0+28); cfc2 FLAG; NCLIP; FLAG&0x7FC40000 != 0 skips
 * (t3=a0+t4 in delay slot). out=base+8; mtc2 gte[0]=t1 / gte[1]=t2
 * (unconditional, BEFORE the MAC0 test); mfc2 MAC0/SXY0 — MAC0<=0
 * skips but SXY0 is STILL stored at out in the blez delay slot.
 * Otherwise SXY1/SXY2 stored at out+t8/+2*t8, RTPS (0x180001)
 * transforms V0, then the four-way screen cull runs on the POST-RTPS
 * SXY0 plus the RTPT SXY1/SXY2 — with a twist: gte[14] is RELOADED
 * after RTPS and stored as a 4th word at out+3*t8, and the sx/sy
 * accumulators get a 4th term from that reloaded word. Both
 * accumulators must be nonzero; base recomputed; OT swl pair.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018A6D4. */
uint32_t prim_persp_cull_emit_58(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018A6D4: t6=28, t7=88, t8=4 */
  cpu->gpr[14] = 28;
  cpu->gpr[15] = 88;
  cpu->gpr[24] = 4;

  if ((int32_t)cpu->gpr[7] <= 0) {                 /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                                /* beq delay slot: t4=32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] DIRECT */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] DIRECT */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] DIRECT */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    gte_execute(cpu, 0x0280030u);                   /* RTPT */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 24);      /* t1 = lw 0x18(a0) */
    cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 28);      /* t2 = lw 0x1C(a0) */
    cpu->gpr[25] = 0x7FC4u << 16;                    /* 0x7FC40000 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);          /* cfc2 $2,$31 (FLAG) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];      /* FLAG & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (delay slot) */
    if (cpu->gpr[25] == cpu->gpr[0]) {              /* bne mask,0 -> culled */
      cpu->gpr[11] = cpu->gpr[11] + 8;              /* out = base+8 */
      cpu->gte_data[0] = cpu->gpr[9];               /* mtc2 gte[0] = t1 (unconditional) */
      gte_write_data(cpu, 1, cpu->gpr[10]);         /* mtc2 gte[1] = t2 */
      cpu->gpr[9]  = cpu->gte_data[24];             /* mfc2 t1,d24 (MAC0) */
      cpu->gpr[8]  = cpu->gte_data[12];             /* mfc2 t0,d12 (SXY0) */
      br2_st32(cpu->gpr[11], cpu->gpr[8]);          /* sw SXY0 -> out (unconditional) */
      if ((int32_t)cpu->gpr[9] > 0) {               /* blez MAC0 -> culled */
        cpu->gpr[9]  = cpu->gte_data[13];           /* mfc2 SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];           /* mfc2 SXY2 */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24]; /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[9]);        /* sw SXY1 */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24]; /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[10]);       /* sw SXY2 */
        gte_execute(cpu, 0x0180001u);               /* RTPS: V0 -> SXY0 */
        /* sx cull on SXY0 (post-RTPS) into t9, sy cull into t0 — guest order:
         * sx0 test, sy0 test, sx1+sy1, sx2+sy2 interleaved per component. */
        cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[8]  = (int32_t)cpu->gpr[8] >> 16;
        cpu->gpr[8]  = (cpu->gpr[8] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[9]  = (int32_t)cpu->gpr[9] >> 16;
        cpu->gpr[9]  = (cpu->gpr[9] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[9];
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16;
        cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10];
        cpu->gpr[10] = cpu->gte_data[14];           /* mfc2 SXY2 (post-RTPS reload) */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24]; /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[10]);       /* sw SXY2' -> out+3*t8 */
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16; /* sy2' (delay slot) */
        if (cpu->gpr[25] != 0) {                    /* beq sx_ok,0 -> skip */
          cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10];
          cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];/* t3 = a0+t4 (delay slot) */
          if (cpu->gpr[8] != 0) {                   /* beq sy_ok,0 -> skip */
            cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);   /* t0 = *a1 (OT head) */
            cpu->gpr[9]  = cpu->gpr[11] << 8;       /* base << 8 */
            swl_guest(cpu->gpr[5] + 2,   cpu->gpr[9]);  /* swl a1+2, base<<8 */
            cpu->gpr[8]  = cpu->gpr[8] << 8;        /* old << 8 */
            swl_guest(cpu->gpr[11] + 2,  cpu->gpr[8]);  /* swl base+2, old<<8 */
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];       /* bgtz delay slot: a0 += t7 (88) */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a844 ======== */

/* prim_depth_shade_emit_40 — screen-cull + depth-shaded attribute OT
 * emitter, input record stride 64 (0x40). MULTI-ENTRY guest fn: only
 * entry 0x8018A844 is ported (t6=20, t7=64, t8=4); sibling entries
 * 0x8018A854/A864/A874 remain generated-authoritative.
 * a0 = record cursor, a1 = OT base, a2 = out-delta select
 * (0 -> t4=24, else t4=24+t6=44), a3 = count.
 *
 * Wraps the loop in ctc2 ctrl29 writes: 341 before iterating (in the
 * blez delay slot path — skipped on count<=0 early out), 1365 on normal
 * exit. Per record: lwc2 gte[0..5]; a3--; RTPT; cfc2 FLAG; NCLIP;
 * FLAG&0x7FC40000 != 0 skips (t3=a0+t4 in delay slot). out=base+8;
 * mfc2 MAC0/SXY0 — MAC0<=0 skips (sx0 pre-computed in delay slot);
 * SXY1/SXY2 = gte[13]/[14]; four-way screen cull (sx via
 * psx_ws_cull_sltiu, sy via >>16 <480); SXY0 stored at out in the sy
 * beq delay slot (unconditional once past MAC0), SXY1/SXY2 at
 * out+t8/+2*t8; base recomputed; AVSZ3 (0x158002D); OTZ = gte_read_data(7);
 * depth-shade attr word at base+12 from OTZ (mask 0x03C00000):
 *   z<256 -> mem & ~mask; 256<=z<1280 -> (mem&~mask)|(((z-256)<<16)&mask);
 *   z>=1280 -> mem | mask.  Then OT swl pair swl(a1+2, base<<8) /
 * swl(base+2, old<<8).
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018A844. */
uint32_t prim_depth_shade_emit_40(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018A844: t6=20, t7=64, t8=4 */
  cpu->gpr[14] = 20;
  cpu->gpr[15] = 64;
  cpu->gpr[24] = 4;
  cpu->gpr[8] = 341;                                  /* delay slot: shade lo (unconditional) */

  if ((int32_t)cpu->gpr[7] <= 0) {                   /* blez a3 (skip ctc2) */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  gte_write_ctrl(cpu, 29, cpu->gpr[8]);               /* ctc2 ctrl29 = 341 */

  cpu->gpr[12] = 24;                                /* beq delay slot: t4=24 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] DIRECT */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] DIRECT */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] DIRECT */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    gte_execute(cpu, 0x0280030u);                   /* RTPT */
    cpu->gpr[25] = 0x7FC4u << 16;                    /* 0x7FC40000 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);          /* cfc2 $2,$31 (FLAG) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];      /* FLAG & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];      /* t3 = a0+t4 (delay slot) */
    if (cpu->gpr[25] == cpu->gpr[0]) {              /* bne mask,0 -> culled */
      cpu->gpr[11] = cpu->gpr[11] + 8;              /* out = base+8 */
      cpu->gpr[9]  = cpu->gte_data[24];             /* mfc2 t1,d24 (MAC0) */
      cpu->gpr[8]  = cpu->gte_data[12];             /* mfc2 t0,d12 (SXY0) */
      cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;         /* sx0 (delay slot) */
      if ((int32_t)cpu->gpr[9] > 0) {               /* blez MAC0 -> skip */
        cpu->gpr[9]  = cpu->gte_data[13];           /* mfc2 SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];           /* mfc2 SXY2 */
        /* sx cull: 3x sltiu512 into t9 via t5 */
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[25] = (int32_t)cpu->gpr[8] >> 16;   /* sy0 (beq delay slot) */
        if (cpu->gpr[25] != 0) {                    /* beq sx_ok,0 -> skip */
          cpu->gpr[25] = (cpu->gpr[25] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[13] = (int32_t)cpu->gpr[9] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
          cpu->gpr[13] = (int32_t)cpu->gpr[10] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
          br2_st32(cpu->gpr[11], cpu->gpr[8]);        /* sw SXY0 -> out (unconditional) */
          if (cpu->gpr[25] != 0) {                  /* beq sy_ok,0 -> skip */
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[9]);          /* sw SXY1 */
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[10]);         /* sw SXY2 */
            cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];    /* t3 = a0+t4 (base) */
            gte_execute(cpu, 0x158002Du);                 /* AVSZ3 -> OTZ */
            cpu->gpr[10] = br2_ld32(cpu->gpr[11] + 12);   /* t2 = lw 0xC(t3) attr mem */
            cpu->gpr[8]  = gte_read_data(cpu, 7);         /* mfc2 t0,d7 (OTZ) */
            cpu->gpr[9]  = 0x03C0u << 16;                 /* 0x03C00000 shade mask */
            cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)256) ? 1u : 0u;
            cpu->gpr[8]  = cpu->gpr[8] + 0xFFFFFF00u;     /* z -= 256 (delay slot) */
            if (cpu->gpr[25] != 0) {                  /* z < 256 */
              cpu->gpr[9]  = ~cpu->gpr[9];            /* ~0x03C00000 */
              cpu->gpr[9]  = cpu->gpr[9] & cpu->gpr[10];  /* attr = mem & ~mask */
            } else {                                  /* z >= 256 */
              cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)1024) ? 1u : 0u;  /* (z-256)<1024 */
              cpu->gpr[8]  = cpu->gpr[8] << 16;       /* (z-256)<<16 (delay slot) */
              if (cpu->gpr[25] != 0) {                /* 256 <= z < 1280 */
                cpu->gpr[8]  = cpu->gpr[8] & cpu->gpr[9];   /* ((z-256)<<16) & mask */
                cpu->gpr[9]  = ~cpu->gpr[9];          /* ~mask */
                cpu->gpr[10] = cpu->gpr[10] & cpu->gpr[9];  /* mem & ~mask */
                cpu->gpr[9]  = cpu->gpr[8] | cpu->gpr[10];  /* | shade bits */
              } else {                                /* z >= 1280 */
                cpu->gpr[9]  = cpu->gpr[10] | cpu->gpr[9];  /* attr = mem | mask */
              }
            }
            cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);     /* t0 = *a1 (OT head) */
            br2_st32(cpu->gpr[11] + 12, cpu->gpr[9]); /* sw attr -> base+12 */
            cpu->gpr[9]  = cpu->gpr[11] << 8;         /* base << 8 */
            swl_guest(cpu->gpr[5] + 2,   cpu->gpr[9]);/* swl a1+2, base<<8 */
            cpu->gpr[8]  = cpu->gpr[8] << 8;          /* old << 8 */
            swl_guest(cpu->gpr[11] + 2,  cpu->gpr[8]);/* swl base+2, old<<8 */
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];       /* bgtz delay slot: a0 += t7 (64) */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[8]  = 1365;                                /* shade hi */
  gte_write_ctrl(cpu, 29, cpu->gpr[8]);               /* ctc2 ctrl29 = 1365 */
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018a9c8 ======== */

/* prim_depth_shade_emit_50 — perspective depth-shaded attribute OT
 * emitter, input record stride 80 (0x50). Sibling of
 * prim_depth_shade_emit (0x8018A9E8, stride 112) sharing the SAME
 * generated body br2eu_func_8018A9C8; only entry 0x8018A9C8 is ported
 * here (t6=24, t7=80, t8=4 -> delta 32|56). Sibling entries
 * 0x8018A9D8/A9E8/A9F8 remain generated-authoritative — note 0x8018A9E8
 * is separately registered to prim_depth_shade_emit.
 * a0 = record cursor, a1 = OT base, a2 = out-delta select
 * (0 -> t4=32, else t4=32+t6=56), a3 = count.
 *
 * Wraps the loop in ctc2 ctrl30 writes: 256 before iterating (skipped
 * on count<=0 early out), 1024 on normal exit. Per record: lwc2
 * gte[0..5]; a3--; RTPT; t1=ld32(a0+24), t2=ld32(a0+28); cfc2 FLAG;
 * NCLIP; mtc2 gte[0]=t1 sits in the flag-branch DELAY SLOT
 * (unconditional); FLAG&0x7FC40000 != 0 skips. gte[1]=t2 then
 * out=base+8; MAC0<=0 skips but SXY0 is STILL stored at out in the blez
 * delay slot. SXY1/SXY2 stored at out+t8/+2*t8; RTPS; four-way screen
 * cull on post-RTPS SXY0 + gte[14]-reload as 4th word (stored at
 * out+3*t8); AVSZ4; OTZ depth-shade attr at base+12 (mask 0x03C00000,
 * z thresholds 256/1280 — identical math to prim_depth_shade_emit);
 * OT swl pair.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018A9C8. */
uint32_t prim_depth_shade_emit_50(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018A9C8: t6=24, t7=80, t8=4 */
  cpu->gpr[14] = 24;
  cpu->gpr[15] = 80;
  cpu->gpr[24] = 4;
  cpu->gpr[8] = 256;                                  /* delay slot: ctc2 val (unconditional) */

  if ((int32_t)cpu->gpr[7] <= 0) {                   /* blez a3 (skip ctc2) */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  gte_write_ctrl(cpu, 30, cpu->gpr[8]);               /* ctc2 ctrl30 = 256 */

  cpu->gpr[12] = 32;                                /* beq delay slot: t4=32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);           /* lwc2 gte[0] DIRECT */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));  /* lwc2 gte[1] */
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);       /* lwc2 gte[2] DIRECT */
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12)); /* lwc2 gte[3] */
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);      /* lwc2 gte[4] DIRECT */
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20)); /* lwc2 gte[5] */
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                /* a3-- */
    gte_execute(cpu, 0x0280030u);                   /* RTPT */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 24);      /* t1 = lw 0x18(a0) */
    cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 28);      /* t2 = lw 0x1C(a0) */
    cpu->gpr[25] = 0x7FC4u << 16;                    /* 0x7FC40000 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);          /* cfc2 $2,$31 (FLAG) */
    gte_execute(cpu, 0x1400006u);                   /* NCLIP */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];      /* FLAG & mask */
    cpu->gte_data[0] = cpu->gpr[9];                 /* mtc2 gte[0]=t1 (delay slot, unconditional) */
    if (cpu->gpr[25] == cpu->gpr[0]) {              /* bne mask,0 -> culled */
      gte_write_data(cpu, 1, cpu->gpr[10]);         /* mtc2 gte[1] = t2 */
      cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];    /* t3 = a0+t4 */
      cpu->gpr[11] = cpu->gpr[11] + 8;              /* out = base+8 */
      cpu->gpr[9]  = cpu->gte_data[24];             /* mfc2 t1,d24 (MAC0) */
      cpu->gpr[8]  = cpu->gte_data[12];             /* mfc2 t0,d12 (SXY0) */
      br2_st32(cpu->gpr[11], cpu->gpr[8]);          /* sw SXY0 -> out (unconditional) */
      if ((int32_t)cpu->gpr[9] > 0) {               /* blez MAC0 -> culled */
        cpu->gpr[9]  = cpu->gte_data[13];           /* mfc2 SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];           /* mfc2 SXY2 */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24]; /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[9]);        /* sw SXY1 */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24]; /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[10]);       /* sw SXY2 */
        gte_execute(cpu, 0x0180001u);               /* RTPS: V0 -> SXY0 */
        /* sx/sy cull — same interleaved order as prim_persp_cull_emit_58 */
        cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[8]  = (int32_t)cpu->gpr[8] >> 16;
        cpu->gpr[8]  = (cpu->gpr[8] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[9]  = (int32_t)cpu->gpr[9] >> 16;
        cpu->gpr[9]  = (cpu->gpr[9] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[9];
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16;
        cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10];
        cpu->gpr[10] = cpu->gte_data[14];           /* mfc2 SXY2 (post-RTPS reload) */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24]; /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[10]);       /* sw SXY2' -> out+3*t8 */
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16; /* sy2' (delay slot) */
        if (cpu->gpr[25] != 0) {                    /* beq sx_ok,0 -> skip */
          cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10];
          cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];/* t3 = a0+t4 (delay slot) */
          if (cpu->gpr[8] != 0) {                   /* beq sy_ok,0 -> skip */
            gte_execute(cpu, 0x168002Eu);           /* AVSZ4 -> OTZ */
            cpu->gpr[10] = br2_ld32(cpu->gpr[11] + 12);   /* t2 = lw 0xC(t3) attr mem */
            cpu->gpr[8]  = gte_read_data(cpu, 7);         /* mfc2 t0,d7 (OTZ) */
            cpu->gpr[9]  = 0x03C0u << 16;                 /* 0x03C00000 shade mask */
            cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)256) ? 1u : 0u;
            cpu->gpr[8]  = cpu->gpr[8] + 0xFFFFFF00u;     /* z -= 256 (delay slot) */
            if (cpu->gpr[25] != 0) {                  /* z < 256 */
              cpu->gpr[9]  = ~cpu->gpr[9];
              cpu->gpr[9]  = cpu->gpr[9] & cpu->gpr[10];  /* attr = mem & ~mask */
            } else {                                  /* z >= 256 */
              cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)1024) ? 1u : 0u;
              cpu->gpr[8]  = cpu->gpr[8] << 16;       /* (z-256)<<16 (delay slot) */
              if (cpu->gpr[25] != 0) {                /* 256 <= z < 1280 */
                cpu->gpr[8]  = cpu->gpr[8] & cpu->gpr[9];
                cpu->gpr[9]  = ~cpu->gpr[9];
                cpu->gpr[10] = cpu->gpr[10] & cpu->gpr[9];
                cpu->gpr[9]  = cpu->gpr[8] | cpu->gpr[10];
              } else {                                /* z >= 1280 */
                cpu->gpr[9]  = cpu->gpr[10] | cpu->gpr[9];
              }
            }
            cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);     /* t0 = *a1 (OT head) */
            br2_st32(cpu->gpr[11] + 12, cpu->gpr[9]); /* sw attr -> base+12 */
            cpu->gpr[9]  = cpu->gpr[11] << 8;         /* base << 8 */
            swl_guest(cpu->gpr[5] + 2,   cpu->gpr[9]);/* swl a1+2, base<<8 */
            cpu->gpr[8]  = cpu->gpr[8] << 8;          /* old << 8 */
            swl_guest(cpu->gpr[11] + 2,  cpu->gpr[8]);/* swl base+2, old<<8 */
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];       /* bgtz delay slot: a0 += t7 (80) */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[8]  = 1024;                                /* ctc2 restore val */
  gte_write_ctrl(cpu, 30, cpu->gpr[8]);               /* ctc2 ctrl30 = 1024 */
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018ad80 ======== */

/* prim_depth_shade_emit_70 — perspective depth-shaded attribute OT
 * emitter, input record stride 112 (0x70), out-record stride 8, out-delta
 * t4 = 32 when a2==0 else 72 (32+40). Sibling of prim_depth_shade_emit
 * (0x8018A9E8, same stride) but a DISTINCT generated body: here BOTH
 * secondary-vector mtc2s (gte[0]=lw(a0+24), gte[1]=lw(a0+28)) sit AFTER
 * the FLAG gate — fully conditional — whereas the A9C8-family body keeps
 * mtc2 gte[0] in the flag-branch delay slot (unconditional).
 * a0 = record cursor, a1 = OT base, a2 = out-delta select, a3 = count
 * (signed; <=0 -> v0=a0 epilogue).
 *
 * Wraps the loop in ctc2 ctrl30 writes: 256 before iterating (delay slot
 * of the entry blez — skipped value-wise on count<=0 early out), 1024 on
 * normal exit. Per record: lwc2 gte[0..5]; a3--; RTPT; t7=lw(a0+24),
 * t8=lw(a0+28); cfc2 FLAG; NCLIP; FLAG&0x7FC40000 != 0 skips (t3=a0+t4
 * in the branch delay slot). mtc2 gte[0]=t7, gte[1]=t8; MAC0<=0 skips
 * but SXY0 is STILL stored at out+8 in the blez delay slot. SXY1/SXY2 at
 * out+16/+24; RTPS; four-way screen cull on SXY0..2 + post-RTPS
 * gte[14]-reload as 4th word (stored at out+32); AVSZ4; OTZ depth-shade
 * attr at base+12 (mask 0x03C00000, z thresholds 256/1280 — identical
 * math to prim_depth_shade_emit); OT swl pair swl(a1+2, base<<8) /
 * swl(base+2, old<<8).
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018AD80. */
uint32_t prim_depth_shade_emit_70(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  cpu->gpr[8] = 256;                                    /* delay slot: t0 = 256 (unconditional) */
  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  gte_write_ctrl(cpu, 30, cpu->gpr[8]);                 /* ctc2 ctrl30 = 256 */

  cpu->gpr[12] = 32;                                  /* beq delay slot: t4 = 32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 40;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);        /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* RTPT */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 24);        /* lw t7,0x18(a0) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 28);        /* lw t8,0x1C(a0) */
    cpu->gpr[25] = 0x7FC40000u;                       /* lui t9,0x7FC4 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,FLAG */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];        /* t9 = flag & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* delay: t3 = a0+t4 (unconditional) */
    if (cpu->gpr[25] == cpu->gpr[0]) {                /* bne mask,0 -> skip emit */
      cpu->gte_data[0] = cpu->gpr[15];                /* mtc2 gte[0] = t7 */
      gte_write_data(cpu, 1, cpu->gpr[24]);           /* mtc2 gte[1] = t8 */
      cpu->gpr[9]  = cpu->gte_data[24];               /* mfc2 t1,MAC0 */
      cpu->gpr[8]  = cpu->gte_data[12];               /* mfc2 t0,SXY0 */
      br2_st32(cpu->gpr[11] + 8, cpu->gpr[8]);        /* sw SXY0,8(t3) (unconditional) */
      if ((int32_t)cpu->gpr[9] > 0) {                 /* blez t1 -> skip */
        cpu->gpr[9]  = cpu->gte_data[13];             /* mfc2 t1,SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];             /* mfc2 t2,SXY2 */
        br2_st32(cpu->gpr[11] + 16, cpu->gpr[9]);     /* sw SXY1,16(t3) */
        br2_st32(cpu->gpr[11] + 24, cpu->gpr[10]);    /* sw SXY2,24(t3) */
        gte_execute(cpu, 0x0180001u);                 /* RTPS */
        cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;         /* sx0 = SXY0 & 0xffff */
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[8]  = (int32_t)cpu->gpr[8] >> 16;    /* SXY0 >> 16 */
        cpu->gpr[8]  = (cpu->gpr[8] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok += */
        cpu->gpr[9]  = (int32_t)cpu->gpr[9] >> 16;
        cpu->gpr[9]  = (cpu->gpr[9] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[9];    /* sy_ok += */
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16;
        cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[9]  = cpu->gte_data[14];             /* post-RTPS SXY2 */
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10];    /* sy_ok += */
        br2_st32(cpu->gpr[11] + 32, cpu->gpr[9]);     /* sw SXY2',32(t3) */
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok 4th */
        cpu->gpr[9]  = (int32_t)cpu->gpr[9] >> 16;    /* delay: SXY2' >> 16 (unconditional) */
        if (cpu->gpr[25] != 0) {                      /* beq sx_ok,zero -> skip */
          cpu->gpr[9]  = (cpu->gpr[9] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[9];  /* sy_ok 4th */
          if (cpu->gpr[8] != 0) {                     /* beq sy_ok,zero -> skip */
            gte_execute(cpu, 0x168002Eu);             /* AVSZ4 -> OTZ */
            cpu->gpr[10] = br2_ld32(cpu->gpr[11] + 12);   /* lw t2,0xC(t3) attr */
            cpu->gpr[8]  = gte_read_data(cpu, 7);         /* mfc2 t0,OTZ */
            cpu->gpr[9]  = 0x03C00000u;                   /* lui t1,0x03C0 */
            cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)256) ? 1u : 0u;
            cpu->gpr[8]  = cpu->gpr[8] + 0xFFFFFF00u;     /* delay: t0 -= 256 (unconditional) */
            uint32_t shade_emit = 0;
            if (cpu->gpr[25] != 0) {                      /* z < 256 */
              cpu->gpr[9]  = ~cpu->gpr[9];
              cpu->gpr[9]  = cpu->gpr[9] & cpu->gpr[10];  /* t1 &= mem */
              shade_emit = 1;
            } else {                                      /* z >= 256 */
              cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)1024) ? 1u : 0u;
              cpu->gpr[8]  = cpu->gpr[8] << 16;           /* delay: t0 <<= 16 (unconditional) */
              if (cpu->gpr[25] != 0) {                    /* 256 <= z < 1280 */
                cpu->gpr[8]  = cpu->gpr[8] & cpu->gpr[9]; /* t0 &= 0x03C00000 */
                cpu->gpr[9]  = ~cpu->gpr[9];              /* not mask */
                cpu->gpr[10] = cpu->gpr[10] & cpu->gpr[9];/* mem &= ~mask */
                cpu->gpr[9]  = cpu->gpr[8] | cpu->gpr[10];/* attr = shade|mem */
                shade_emit = 1;
              }
              /* z >= 1280: beq -> loop tail, emit skipped entirely */
            }
            if (shade_emit) {
              br2_st32(cpu->gpr[11] + 12, cpu->gpr[9]); /* sw attr,0xC(t3) */
              cpu->gpr[9]  = cpu->gpr[11] << 8;         /* sll t1,t3,8 */
              cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);     /* lw t0,(a1) old OT */
              swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);  /* swl (t3<<8),2(a1) */
              cpu->gpr[8]  = cpu->gpr[8] << 8;          /* sll t0,8 */
              swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]); /* swl (t0<<8),2(t3) */
            }
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + 112;                  /* bgtz delay slot: a0 += 112 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[8] = 1024;                                 /* t0 = 1024 */
  gte_write_ctrl(cpu, 30, cpu->gpr[8]);               /* ctc2 ctrl30 = 1024 */
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018aee8 ======== */

/* prim_depth_shade_emit_88 — depth-shaded attribute OT emitter, input
 * record stride 136 (0x88), out-record stride 12, out-delta t4 = 32
 * when a2==0 else 84 (32+52). Same algorithm as
 * prim_depth_shade_emit_70 (0x8018AD80) with wider output spacing:
 * SXY0 at out+8, SXY1 at out+20, SXY2 at out+32, post-RTPS SXY2 at
 * out+44, depth attr at base+12, a0 += 136 per record.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018AEE8. */
uint32_t prim_depth_shade_emit_88(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  cpu->gpr[8] = 256;                                    /* delay slot: t0 = 256 (unconditional) */
  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  gte_write_ctrl(cpu, 30, cpu->gpr[8]);                 /* ctc2 ctrl30 = 256 */

  cpu->gpr[12] = 32;                                  /* beq delay slot: t4 = 32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += 52;
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);        /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* RTPT */
    cpu->gpr[15] = br2_ld32(cpu->gpr[4] + 24);        /* lw t7,0x18(a0) */
    cpu->gpr[24] = br2_ld32(cpu->gpr[4] + 28);        /* lw t8,0x1C(a0) */
    cpu->gpr[25] = 0x7FC40000u;                       /* lui t9,0x7FC4 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,FLAG */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];        /* t9 = flag & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* delay: t3 = a0+t4 (unconditional) */
    if (cpu->gpr[25] == cpu->gpr[0]) {                /* bne mask,0 -> skip emit */
      cpu->gte_data[0] = cpu->gpr[15];                /* mtc2 gte[0] = t7 */
      gte_write_data(cpu, 1, cpu->gpr[24]);           /* mtc2 gte[1] = t8 */
      cpu->gpr[9]  = cpu->gte_data[24];               /* mfc2 t1,MAC0 */
      cpu->gpr[8]  = cpu->gte_data[12];               /* mfc2 t0,SXY0 */
      br2_st32(cpu->gpr[11] + 8, cpu->gpr[8]);        /* sw SXY0,8(t3) (unconditional) */
      if ((int32_t)cpu->gpr[9] > 0) {                 /* blez t1 -> skip */
        cpu->gpr[9]  = cpu->gte_data[13];             /* mfc2 t1,SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];             /* mfc2 t2,SXY2 */
        br2_st32(cpu->gpr[11] + 20, cpu->gpr[9]);     /* sw SXY1,20(t3) */
        br2_st32(cpu->gpr[11] + 32, cpu->gpr[10]);    /* sw SXY2,32(t3) */
        gte_execute(cpu, 0x0180001u);                 /* RTPS */
        cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;         /* sx0 = SXY0 & 0xffff */
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[8]  = (int32_t)cpu->gpr[8] >> 16;
        cpu->gpr[8]  = (cpu->gpr[8] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok += */
        cpu->gpr[9]  = (int32_t)cpu->gpr[9] >> 16;
        cpu->gpr[9]  = (cpu->gpr[9] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[9];    /* sy_ok += */
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16;
        cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[9]  = cpu->gte_data[14];             /* post-RTPS SXY2 */
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10];    /* sy_ok += */
        br2_st32(cpu->gpr[11] + 44, cpu->gpr[9]);     /* sw SXY2',44(t3) */
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok 4th */
        cpu->gpr[9]  = (int32_t)cpu->gpr[9] >> 16;    /* delay: SXY2' >> 16 (unconditional) */
        if (cpu->gpr[25] != 0) {                      /* beq sx_ok,zero -> skip */
          cpu->gpr[9]  = (cpu->gpr[9] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[9];  /* sy_ok 4th */
          if (cpu->gpr[8] != 0) {                     /* beq sy_ok,zero -> skip */
            gte_execute(cpu, 0x168002Eu);             /* AVSZ4 -> OTZ */
            cpu->gpr[10] = br2_ld32(cpu->gpr[11] + 12);   /* lw t2,0xC(t3) attr */
            cpu->gpr[8]  = gte_read_data(cpu, 7);         /* mfc2 t0,OTZ */
            cpu->gpr[9]  = 0x03C00000u;                   /* lui t1,0x03C0 */
            cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)256) ? 1u : 0u;
            cpu->gpr[8]  = cpu->gpr[8] + 0xFFFFFF00u;     /* delay: t0 -= 256 (unconditional) */
            uint32_t shade_emit = 0;
            if (cpu->gpr[25] != 0) {                      /* z < 256 */
              cpu->gpr[9]  = ~cpu->gpr[9];
              cpu->gpr[9]  = cpu->gpr[9] & cpu->gpr[10];  /* t1 &= mem */
              shade_emit = 1;
            } else {                                      /* z >= 256 */
              cpu->gpr[25] = (cpu->gpr[8] < (uint32_t)1024) ? 1u : 0u;
              cpu->gpr[8]  = cpu->gpr[8] << 16;           /* delay: t0 <<= 16 (unconditional) */
              if (cpu->gpr[25] != 0) {                    /* 256 <= z < 1280 */
                cpu->gpr[8]  = cpu->gpr[8] & cpu->gpr[9]; /* t0 &= 0x03C00000 */
                cpu->gpr[9]  = ~cpu->gpr[9];              /* not mask */
                cpu->gpr[10] = cpu->gpr[10] & cpu->gpr[9];/* mem &= ~mask */
                cpu->gpr[9]  = cpu->gpr[8] | cpu->gpr[10];/* attr = shade|mem */
                shade_emit = 1;
              }
              /* z >= 1280: beq -> loop tail, emit skipped entirely */
            }
            if (shade_emit) {
              br2_st32(cpu->gpr[11] + 12, cpu->gpr[9]); /* sw attr,0xC(t3) */
              cpu->gpr[9]  = cpu->gpr[11] << 8;         /* sll t1,t3,8 */
              cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);     /* lw t0,(a1) old OT */
              swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);  /* swl (t3<<8),2(a1) */
              cpu->gpr[8]  = cpu->gpr[8] << 8;          /* sll t0,8 */
              swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]); /* swl (t0<<8),2(t3) */
            }
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + 136;                  /* bgtz delay slot: a0 += 136 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[8] = 1024;                                 /* t0 = 1024 */
  gte_write_ctrl(cpu, 30, cpu->gpr[8]);               /* ctc2 ctrl30 = 1024 */
  cpu->gpr[2] = cpu->gpr[4];                          /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018b380 ======== */

/* prim_nclip_cull_emit_48 — NCLIP-gated screen-cull OT emitter, input
 * record stride 72 (0x48), out stride 4, out-delta t4 = 24 when a2==0
 * else 48 (24+t6=24). MULTI-ENTRY guest fn: only entry 0x8018B380 is
 * ported (t6=24, t7=72, t8=4); sibling entries 0x8018B390/B3A0/B3B0/B3C0
 * remain generated-authoritative.
 * a0 = record cursor, a1 = OT base, a2 = out-delta select, a3 = count.
 *
 * Per record: lwc2 gte[0..5]; a3--; RTPT; cfc2 FLAG; NCLIP;
 * FLAG&0x7FC40000 != 0 skips (t3=a0+t4 in delay slot). NO secondary
 * vector mtc2, NO attr loads. out = base+8; MAC0<=0 skips (sx0
 * pre-computed in the blez delay slot). SXY1/SXY2 = gte[13]/[14];
 * three-way screen cull (sx via psx_ws_cull_sltiu, sy via >>16 <480);
 * SXY0 stored at out+0 in the sy-beq DELAY SLOT (unconditional once
 * past MAC0), SXY1/SXY2 at out+t8/+2*t8; base recomputed; OT swl pair.
 * No RTPS secondary vector, no AVSZ depth test.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018B380. */
uint32_t prim_nclip_cull_emit_48(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018B380: t6=24, t7=72, t8=4 */
  cpu->gpr[14] = 24;
  cpu->gpr[15] = 72;
  cpu->gpr[24] = 4;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 (nop delay) */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 24;                                  /* beq delay slot: t4 = 24 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];  /* t4 = 24+t6 */
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);        /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* RTPT */
    cpu->gpr[25] = 0x7FC40000u;                       /* lui t9,0x7FC4 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,FLAG */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    /* nop delay slot */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];        /* t9 = flag & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* delay: t3 = a0+t4 (unconditional) */
    if (cpu->gpr[25] == cpu->gpr[0]) {                /* bne mask,0 -> skip emit */
      cpu->gpr[11] = cpu->gpr[11] + 8;                /* out = base+8 */
      cpu->gpr[9]  = cpu->gte_data[24];               /* mfc2 t1,MAC0 */
      cpu->gpr[8]  = cpu->gte_data[12];               /* mfc2 t0,SXY0 */
      cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;           /* delay: t9 = sx0 (unconditional) */
      if ((int32_t)cpu->gpr[9] > 0) {                 /* blez t1 -> skip */
        cpu->gpr[9]  = cpu->gte_data[13];             /* mfc2 t1,SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];             /* mfc2 t2,SXY2 */
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok (3 verts) */
        int sx_ok = (cpu->gpr[25] != cpu->gpr[0]);    /* beq sx_ok,zero (pre-delay value) */
        cpu->gpr[25] = (int32_t)cpu->gpr[8] >> 16;    /* delay: SXY0 >> 16 (unconditional) */
        if (sx_ok) {
          cpu->gpr[25] = (cpu->gpr[25] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[13] = (int32_t)cpu->gpr[9] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
          cpu->gpr[13] = (int32_t)cpu->gpr[10] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13]; /* sy_ok (3 verts) */
          br2_st32(cpu->gpr[11], cpu->gpr[8]);        /* delay: sw SXY0,(t3) (unconditional) */
          if (cpu->gpr[25] != cpu->gpr[0]) {          /* beq sy_ok,zero -> skip */
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[9]);          /* sw SXY1,(t3) */
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[10]);         /* sw SXY2,(t3) */
            cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];    /* t3 = a0+t4 (base) */
            cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* lw t0,(a1) old OT */
            cpu->gpr[9]  = cpu->gpr[11] << 8;             /* sll t1,t3,8 */
            swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl (t3<<8),2(a1) */
            cpu->gpr[8]  = cpu->gpr[8] << 8;              /* sll t0,8 */
            swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl (t0<<8),2(t3) */
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];           /* bgtz delay slot: a0 += t7 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                            /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018b4b8 ======== */

/* prim_rtps_cull_emit_58 — RTPS secondary-vector screen-cull OT emitter,
 * input record stride 88 (0x58), out stride 4, out-delta t4 = 32 when
 * a2==0 else 60 (32+t6=32). MULTI-ENTRY guest fn: only entry 0x8018B4B8
 * is ported (t6=28, t7=88, t8=4); sibling entries
 * 0x8018B4C8/B4D8/B4E8/B4F8 remain generated-authoritative.
 * a0 = record cursor, a1 = OT base, a2 = out-delta select, a3 = count.
 *
 * Per record: lwc2 gte[0..5]; a3--; RTPT; t1=lw(a0+24), t2=lw(a0+28);
 * cfc2 FLAG; NCLIP; FLAG&0x7FC40000 != 0 skips (t3=a0+t4 in delay
 * slot). out = base+8; mtc2 gte[0]=t1, gte[1]=t2; MAC0<=0 skips but
 * SXY0 is STILL stored at out+0 in the blez delay slot. SXY1/SXY2 at
 * out+t8/+2*t8; RTPS; four-way screen cull (post-RTPS gte[14]-reload as
 * 4th word, stored at out+3*t8); NO AVSZ depth test — straight to OT
 * swl pair. a0 += t7 per record.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018B4B8. */
uint32_t prim_rtps_cull_emit_58(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018B4B8: t6=28, t7=88, t8=4 */
  cpu->gpr[14] = 28;
  cpu->gpr[15] = 88;
  cpu->gpr[24] = 4;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 (nop delay) */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 32;                                  /* beq delay slot: t4 = 32 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];  /* t4 = 32+t6 */
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);        /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* RTPT */
    cpu->gpr[9]  = br2_ld32(cpu->gpr[4] + 24);        /* lw t1,0x18(a0) */
    cpu->gpr[10] = br2_ld32(cpu->gpr[4] + 28);        /* lw t2,0x1C(a0) */
    cpu->gpr[25] = 0x7FC40000u;                       /* lui t9,0x7FC4 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,FLAG */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    /* nop delay slot */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];        /* t9 = flag & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* delay: t3 = a0+t4 (unconditional) */
    if (cpu->gpr[25] == cpu->gpr[0]) {                /* bne mask,0 -> skip emit */
      cpu->gpr[11] = cpu->gpr[11] + 8;                /* out = base+8 */
      cpu->gte_data[0] = cpu->gpr[9];                 /* mtc2 gte[0] = t1 */
      gte_write_data(cpu, 1, cpu->gpr[10]);           /* mtc2 gte[1] = t2 */
      cpu->gpr[9]  = cpu->gte_data[24];               /* mfc2 t1,MAC0 */
      cpu->gpr[8]  = cpu->gte_data[12];               /* mfc2 t0,SXY0 */
      br2_st32(cpu->gpr[11], cpu->gpr[8]);            /* delay: sw SXY0,(t3) (unconditional) */
      if ((int32_t)cpu->gpr[9] > 0) {                 /* blez t1 -> skip */
        cpu->gpr[9]  = cpu->gte_data[13];             /* mfc2 t1,SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];             /* mfc2 t2,SXY2 */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];    /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[9]);          /* sw SXY1,(t3) */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];    /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[10]);         /* sw SXY2,(t3) */
        gte_execute(cpu, 0x0180001u);                 /* RTPS */
        cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;         /* sx0 = SXY0 & 0xffff */
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[8]  = (int32_t)cpu->gpr[8] >> 16;
        cpu->gpr[8]  = (cpu->gpr[8] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok += */
        cpu->gpr[9]  = (int32_t)cpu->gpr[9] >> 16;
        cpu->gpr[9]  = (cpu->gpr[9] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[9];    /* sy_ok += */
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16;
        cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
        cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10];   /* sy_ok (3 verts) */
        cpu->gpr[10] = cpu->gte_data[14];             /* post-RTPS SXY2 */
        cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
        br2_st32(cpu->gpr[11], cpu->gpr[10]);         /* sw SXY2',(t3) */
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok 4th */
        cpu->gpr[10] = (int32_t)cpu->gpr[10] >> 16;   /* delay: SXY2' >> 16 (unconditional) */
        if (cpu->gpr[25] != cpu->gpr[0]) {            /* beq sx_ok,zero -> skip */
          cpu->gpr[10] = (cpu->gpr[10] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[8]  = cpu->gpr[8] + cpu->gpr[10]; /* sy_ok 4th */
          cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12]; /* delay: t3 = a0+t4 (unconditional) */
          if (cpu->gpr[8] != cpu->gpr[0]) {          /* beq sy_ok,zero -> skip */
            cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);     /* lw t0,(a1) old OT */
            cpu->gpr[9]  = cpu->gpr[11] << 8;         /* sll t1,t3,8 */
            swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);  /* swl (t3<<8),2(a1) */
            cpu->gpr[8]  = cpu->gpr[8] << 8;          /* sll t0,8 */
            swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]); /* swl (t0<<8),2(t3) */
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];           /* bgtz delay slot: a0 += t7 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                            /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018b628 ======== */

/* prim_avsz3_cull_emit_58 — AVSZ3 depth-gated screen-cull OT emitter,
 * input record stride 88 (0x58), out stride 8, out-delta t4 = 24 when
 * a2==0 else 56 (24+t6=24). Single-entry guest fn (constants inline:
 * t6=32, t7=88, t8=8). a0 = record cursor, a1 = OT base, a2 = out-delta
 * select, a3 = count.
 *
 * Per record: lwc2 gte[0..5]; a3--; RTPT; cfc2 FLAG; NCLIP;
 * FLAG&0x7FC40000 != 0 skips (t3=a0+t4 in delay slot). out = base+8;
 * MAC0<=0 skips (sx0 pre-computed in blez delay slot). SXY1/SXY2 =
 * gte[13]/[14]; three-way screen cull; SXY0 stored at out+0 in the
 * sy-beq DELAY SLOT; SXY1/SXY2 at out+t8/+2*t8; AVSZ3 (0x158002D);
 * OTZ < 2048 -> TAIL-DISPATCH to 0x8018B884 (the shared
 * prim_persp_ot_emit loop tail — remaining records continue under the
 * B76C emit body with the live t4/t7/t8); OTZ >= 2048 -> inline OT swl
 * pair then loop. Returns final cursor.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018B628. */
uint32_t prim_avsz3_cull_emit_58(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  CPUState *cpu = clean_cpu_state();
  (void)a0; (void)a1; (void)a2; (void)a3;

  /* entry 0x8018B628: t6=32, t7=88, t8=8 */
  cpu->gpr[14] = 32;
  cpu->gpr[15] = 88;
  cpu->gpr[24] = 8;

  if ((int32_t)cpu->gpr[7] <= 0) {                     /* blez a3 (nop delay) */
    cpu->gpr[2] = cpu->gpr[4];
    return cpu->gpr[2];
  }
  cpu->gpr[12] = 24;                                  /* beq delay slot: t4 = 24 */
  if (cpu->gpr[6] != cpu->gpr[0]) cpu->gpr[12] += cpu->gpr[14];  /* t4 = 24+t6 */
  do {
    cpu->gte_data[0] = psx_cyc_lwc2_read(cpu, cpu->gpr[4]);        /* lwc2 gte[0] */
    gte_write_data(cpu, 1, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 4));
    cpu->gte_data[2] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 8);
    gte_write_data(cpu, 3, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 12));
    cpu->gte_data[4] = psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 16);
    gte_write_data(cpu, 5, psx_cyc_lwc2_read(cpu, cpu->gpr[4] + 20));
    cpu->gpr[7]  = cpu->gpr[7] - 1u;                  /* a3-- */
    gte_execute(cpu, 0x0280030u);                     /* RTPT */
    cpu->gpr[25] = 0x7FC40000u;                       /* lui t9,0x7FC4 */
    cpu->gpr[2]  = gte_read_ctrl(cpu, 31);            /* cfc2 v0,FLAG */
    gte_execute(cpu, 0x1400006u);                     /* NCLIP */
    /* nop delay slot */
    cpu->gpr[25] = cpu->gpr[2] & cpu->gpr[25];        /* t9 = flag & mask */
    cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];        /* delay: t3 = a0+t4 (unconditional) */
    if (cpu->gpr[25] == cpu->gpr[0]) {                /* bne mask,0 -> skip emit */
      cpu->gpr[11] = cpu->gpr[11] + 8;                /* out = base+8 */
      cpu->gpr[9]  = cpu->gte_data[24];               /* mfc2 t1,MAC0 */
      cpu->gpr[8]  = cpu->gte_data[12];               /* mfc2 t0,SXY0 */
      cpu->gpr[25] = cpu->gpr[8] & 0xFFFFu;           /* delay: t9 = sx0 (unconditional) */
      if ((int32_t)cpu->gpr[9] > 0) {                 /* blez t1 -> skip */
        cpu->gpr[9]  = cpu->gte_data[13];             /* mfc2 t1,SXY1 */
        cpu->gpr[10] = cpu->gte_data[14];             /* mfc2 t2,SXY2 */
        cpu->gpr[25] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[25], 512u);
        cpu->gpr[13] = cpu->gpr[9] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
        cpu->gpr[13] = cpu->gpr[10] & 0xFFFFu;
        cpu->gpr[13] = (uint32_t)psx_ws_cull_sltiu(cpu->gpr[13], 512u);
        cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];   /* sx_ok (3 verts) */
        int sx_ok = (cpu->gpr[25] != cpu->gpr[0]);    /* beq sx_ok,zero (pre-delay value) */
        cpu->gpr[25] = (int32_t)cpu->gpr[8] >> 16;    /* delay: SXY0 >> 16 (unconditional) */
        if (sx_ok) {
          cpu->gpr[25] = (cpu->gpr[25] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[13] = (int32_t)cpu->gpr[9] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13];
          cpu->gpr[13] = (int32_t)cpu->gpr[10] >> 16;
          cpu->gpr[13] = (cpu->gpr[13] < (uint32_t)480) ? 1u : 0u;
          cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[13]; /* sy_ok (3 verts) */
          br2_st32(cpu->gpr[11], cpu->gpr[8]);        /* delay: sw SXY0,(t3) (unconditional) */
          if (cpu->gpr[25] != cpu->gpr[0]) {          /* beq sy_ok,zero -> skip */
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[9]);          /* sw SXY1,(t3) */
            cpu->gpr[11] = cpu->gpr[11] + cpu->gpr[24];   /* out += t8 */
            br2_st32(cpu->gpr[11], cpu->gpr[10]);         /* sw SXY2,(t3) */
            gte_execute(cpu, 0x158002Du);                 /* AVSZ3 -> OTZ */
            cpu->gpr[8]  = gte_read_data(cpu, 7);         /* mfc2 t0,OTZ */
            /* nop delay slot */
            cpu->gpr[8]  = ((int32_t)cpu->gpr[8] < 2048) ? 1u : 0u;/* slti t0,otz,2048 */
            if (cpu->gpr[8] != cpu->gpr[0]) {
              /* CPS taken split: tail-dispatch to 0x8018B884 (shared emit tail).
               * Direct call into prim_emit_tail_advance — identical observable
               * behavior (it advances a0 by t7 then re-enters the B76C emit loop
               * while a3 > 0, else returns the cursor). */
              return prim_emit_tail_advance(cpu->gpr[4], cpu->gpr[5],
                                            (int32_t)cpu->gpr[7], cpu->gpr[12],
                                            cpu->gpr[15], cpu->gpr[24]);
            }
            /* nop delay slot */
            cpu->gpr[11] = cpu->gpr[4] + cpu->gpr[12];    /* t3 = a0+t4 (base) */
            cpu->gpr[8]  = br2_ld32(cpu->gpr[5]);         /* lw t0,(a1) old OT */
            cpu->gpr[9]  = cpu->gpr[11] << 8;             /* sll t1,t3,8 */
            swl_guest(cpu->gpr[5] + 2, cpu->gpr[9]);      /* swl (t3<<8),2(a1) */
            cpu->gpr[8]  = cpu->gpr[8] << 8;              /* sll t0,8 */
            swl_guest(cpu->gpr[11] + 2, cpu->gpr[8]);     /* swl (t0<<8),2(t3) */
          }
        }
      }
    }
    cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[15];           /* bgtz delay slot: a0 += t7 */
  } while ((int32_t)cpu->gpr[7] > 0);
  cpu->gpr[2] = cpu->gpr[4];                            /* move v0,a0 */
  return cpu->gpr[2];
}

/* ======== 0x8018b100 ======== */

/* prim_xform3_task_run — dual-buffer primitive-list transform + task
 * runner (guest 0x8018B100, 264B, 12 CPS blocks — clean CPS coroutine;
 * all 12 addrs registered in clean_cps.map). Walks `count` 0x28-int8_t
 * records at a0; per record with lh(rec+0xC)==0 and lw(rec)!=0 it runs
 * gte_xform3_t_ip twice around GTE cmd 0x0480012, stashing live
 * t4-t8 + MAC1-3 into scratchpad 0x1F8003C0..0x3DC, then jalr's the
 * record's 16-int8_t task entries (count at [s2], entries at s2+4 stride
 * 16: a3=ent[1], a0=ent[2], fn=ent[3]). Caller s0-s6+ra saved to
 * scratchpad 0x1F8003E0..0x3FC and restored on exit; $v0 = final record
 * pointer (a0 + 0x28*count), or a0 when count <= 0.
 *
 * All three call sites are CPS tail-dispatches ($ra = continuation,
 * psx_check_interrupts_at, cpu->pc = callee, return) exactly like the
 * generated body — a yield inside any callee resumes via
 * clean_cps.map -> this body's pc-switch. a4/a5 arrive on the guest
 * stack (sp+16/sp+20 -> s4/s5) exactly as generated reads them.
 * Disasm generated/SLES_017.22_full_41.c br2eu_func_8018B100. */
uint32_t prim_xform3_task_run(void)
{
  extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
  CPUState *c = clean_cpu_state();
  uint32_t pc;
  if (!c) return 0;
  pc = c->pc;
  if (pc != 0u && pc != 0x8018B100u) {
    switch (pc) {
    case 0x8018B108u: goto b108;
    case 0x8018B13Cu: goto b13C;
    case 0x8018B14Cu: goto b14C;
    case 0x8018B154u: goto b154;
    case 0x8018B15Cu: goto b15C;
    case 0x8018B198u: goto b198;
    case 0x8018B1B0u: goto b1B0;
    case 0x8018B1C4u: goto b1C4;
    case 0x8018B1CCu: goto b1CC;
    case 0x8018B1D8u: goto b1D8;
    case 0x8018B200u: goto b200;
    default: break;
    }
  }

  /* block_8018B100 — entry: blez a3 -> shared epilogue; delay t0 = SPAD */
  { int _bc = ((int32_t)c->gpr[7] <= 0);
    c->gpr[8] = 0x1F800000u;                        /* delay: t0 = 0x1F800000 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018B200u);
      goto b200;
    } else {
      psx_check_interrupts_at(c, 0x8018B108u);
      goto b108;
    } }

b108:
  /* save caller s0-s6 + ra into scratchpad 0x1F8003E0..0x3FC */
  br2_st32(c->gpr[8] + 1020, c->gpr[31]);            /* ra  -> 0x1F8003FC */
  br2_st32(c->gpr[8] + 1016, c->gpr[16]);            /* s0  -> 0x1F8003F8 */
  br2_st32(c->gpr[8] + 1012, c->gpr[17]);            /* s1  -> 0x1F8003F4 */
  br2_st32(c->gpr[8] + 1008, c->gpr[18]);            /* s2  -> 0x1F8003F0 */
  br2_st32(c->gpr[8] + 1004, c->gpr[19]);            /* s3  -> 0x1F8003EC */
  br2_st32(c->gpr[8] + 1000, c->gpr[20]);            /* s4  -> 0x1F8003E8 */
  br2_st32(c->gpr[8] +  996, c->gpr[21]);            /* s5  -> 0x1F8003E4 */
  br2_st32(c->gpr[8] +  992, c->gpr[22]);            /* s6  -> 0x1F8003E0 */
  c->gpr[16] = c->gpr[4];                            /* s0 = a0 */
  c->gpr[17] = c->gpr[5];                            /* s1 = a1 */
  c->gpr[19] = c->gpr[7];                            /* s3 = a3 */
  c->gpr[20] = br2_ld32(c->gpr[29] + 16);            /* s4 = lw sp+16 (a4) */
  c->gpr[21] = br2_ld32(c->gpr[29] + 20);            /* s5 = lw sp+20 (a5) */
  /* fall through */

b13C:                                               /* record loop head */
  c->gpr[8] = (uint32_t)(int32_t)(int16_t)br2_ld16(c->gpr[16] + 12); /* lh sx16 */
  c->gpr[5] = br2_ld32(c->gpr[16]);                  /* delay: a1 = lw(s0) prim */
  { int _bc = (c->gpr[8] != c->gpr[0]);              /* bne t0,zero -> skip */
    c->gpr[4] = c->gpr[21];                          /* delay: a0 = s5 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018B1CCu);
      goto b1CC;
    } else {
      psx_check_interrupts_at(c, 0x8018B14Cu);
      goto b14C;
    } }

b14C:
  { int _bc = (c->gpr[5] == c->gpr[0]);              /* beq a1,zero -> skip */
    /* nop delay slot */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018B1CCu);
      goto b1CC;
    } else {
      psx_check_interrupts_at(c, 0x8018B154u);
      goto b154;
    } }

b154:
  c->gpr[31] = 0x8018B15Cu;                          /* jal link */
  c->gpr[5]  = c->gpr[5] + 32;                       /* delay: a1 = prim+0x20 */
  psx_check_interrupts_at(c, 0x80186F5Cu);           /* jal gte_xform3_t_ip */
  c->pc = 0x80186F5Cu;
  return c->gpr[2];

b15C:
  c->gte_data[0] = psx_cyc_lwc2_read(c, c->gpr[16] + 4);    /* lwc2 gte[0] */
  gte_write_data(c, 1, psx_cyc_lwc2_read(c, c->gpr[16] + 8)); /* lwc2 gte[1] */
  c->gpr[11] = 0x1F800000u;                          /* t3 = 0x1F800000 */
  gte_execute(c, 0x0480012u);                        /* GTE cmd 0x12 */
  br2_st32(c->gpr[11] + 960, c->gpr[14]);             /* sw t6 -> 0x1F8003C0 */
  br2_st32(c->gpr[11] + 964, c->gpr[12]);             /* sw t4 -> 0x1F8003C4 */
  br2_st32(c->gpr[11] + 968, c->gpr[15]);             /* sw t7 -> 0x1F8003C8 */
  br2_st32(c->gpr[11] + 972, c->gpr[24]);             /* sw t8 -> 0x1F8003CC */
  br2_st32(c->gpr[11] + 976, c->gpr[13]);             /* sw t5 -> 0x1F8003D0 */
  c->gpr[5]  = c->gpr[11] + 960;                      /* a1 = 0x1F8003C0 */
  br2_st32(c->gpr[11] + 980, c->gte_data[25]);        /* swc2 gte[25] -> 3D4 */
  gte_precision_store_word(c->gpr[11] + 980, 25);
  br2_st32(c->gpr[11] + 984, c->gte_data[26]);        /* swc2 gte[26] -> 3D8 */
  gte_precision_store_word(c->gpr[11] + 984, 26);
  br2_st32(c->gpr[11] + 988, c->gte_data[27]);        /* swc2 gte[27] -> 3DC */
  gte_precision_store_word(c->gpr[11] + 988, 27);
  c->gpr[31] = 0x8018B198u;                          /* jal link */
  c->gpr[4]  = c->gpr[20];                            /* delay: a0 = s4 */
  psx_check_interrupts_at(c, 0x80186F5Cu);            /* jal gte_xform3_t_ip */
  c->pc = 0x80186F5Cu;
  return c->gpr[2];

b198:
  c->gpr[18] = br2_ld32(c->gpr[16] + 28);             /* s2 = lw(s0+0x1C) tasks */
  c->gpr[5]  = c->gpr[17];                            /* a1 = s1 */
  c->gpr[22] = br2_ld32(c->gpr[18]);                  /* s6 = lw(s2) count */
  /* nop delay slot */
  { int _bc = (c->gpr[22] == c->gpr[0]);              /* beq s6,zero -> next rec */
    c->gpr[18] = c->gpr[18] + 4;                      /* delay: s2 += 4 (entries) */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018B1CCu);
      goto b1CC;
    } else {
      psx_check_interrupts_at(c, 0x8018B1B0u);
      goto b1B0;
    } }

b1B0:                                               /* task-entry loop */
  c->gpr[7] = br2_ld32(c->gpr[18] + 4);               /* a3 = ent[1] */
  c->gpr[8] = br2_ld32(c->gpr[18] + 12);              /* t0 = ent[3] fn */
  c->gpr[4] = br2_ld32(c->gpr[18] + 8);               /* a0 = ent[2] */
  c->gpr[31] = 0x8018B1C4u;                           /* jalr link */
  c->gpr[22] = c->gpr[22] + 0xFFFFFFFFu;              /* delay: s6-- */
  { uint32_t _jt = c->gpr[8];                         /* jalr $ra,$t0 */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt;
    return c->gpr[2]; }

b1C4:
  { int _bc = ((int32_t)c->gpr[22] > 0);              /* bgtz s6 -> next entry */
    c->gpr[18] = c->gpr[18] + 16;                     /* delay: s2 += 16 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018B1B0u);
      goto b1B0;
    } else {
      psx_check_interrupts_at(c, 0x8018B1CCu);
      goto b1CC;
    } }

b1CC:                                               /* record loop tail */
  c->gpr[19] = c->gpr[19] + 0xFFFFFFFFu;              /* s3-- */
  { int _bc = ((int32_t)c->gpr[19] > 0);              /* bgtz s3 -> next record */
    c->gpr[16] = c->gpr[16] + 40;                     /* delay: s0 += 0x28 */
    if (_bc) {
      psx_check_interrupts_at(c, 0x8018B13Cu);
      goto b13C;
    } else {
      psx_check_interrupts_at(c, 0x8018B1D8u);
      goto b1D8;
    } }

b1D8:                                               /* epilogue: restore SPAD */
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
  /* fall through */

b200:
  { uint32_t _jt = c->gpr[31];                        /* jr $ra */
    c->gpr[2] = c->gpr[4];                            /* delay: v0 = a0 */
    psx_check_interrupts_at(c, _jt);
    c->pc = _jt;
    return c->gpr[2]; }
}
