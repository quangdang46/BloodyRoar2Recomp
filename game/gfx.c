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

typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef int8_t   byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef int      int32_t_ghidra;
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

void gpu_load_image_cmd(undefined4,undefined4);
void prim_packet_init_tex(int,int,int,uint,byte *);
undefined1 * guest_memcpy(uint32_t *,uint32_t *,int);
void printf_forward(const char *, ...);
void  guest_memset(uint32_t *,undefined1,int);
void  cd_dispatch_tblc(void);
void  cd_timeout_arm(void);
int   cd_pump_watchdog(void);
void  bios_call_a0(uint);
undefined4 gpu_mode_regs_apply(uint);
void  gpu_prim_pkt_build(int,ushort *);
undefined4 gp_bde20_read(void);
uint32_t gpu_invoke(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
void  gpu_rect_bounds_dbg(undefined4,short *);

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

#define DAT_801b55dc   (*(int32_t *)g_DAT_801b55dc)
#define DAT_801b55e0   (*(int32_t *)g_DAT_801b55e0)
#define DAT_801b55e4   (*(int32_t *)g_DAT_801b55e4)
#define DAT_801f642c   (*(uint32_t *)g_DAT_801f642c)
#define DAT_801f68b4   (*(uint32_t *)g_DAT_801f68b4)
#define _DAT_1f8000dc  (*(uint32_t *)g__DAT_1f8000dc)
#define DAT_801e40b6   (*(uint8_t *)g_DAT_801e40b6)
#define DAT_801e40b7   (*(uint8_t *)g_DAT_801e40b7)
#define DAT_801b8320   (*(uint8_t  *)g_DAT_801b8320)
#define DAT_801b8321   (*(uint8_t  *)g_DAT_801b8321)
#define DAT_801b8322   (*(uint8_t  *)g_DAT_801b8322)
#define DAT_801b8323   (*(uint8_t  *)g_DAT_801b8323)
#define DAT_801b8324   (*(uint16_t *)g_DAT_801b8324)
#define DAT_801b8326   (*(uint16_t *)g_DAT_801b8326)
#define DAT_801b8330   ((uint32_t *)guest_at(0x801b8330))
#define DAT_801b838c   ((uint32_t *)guest_at(0x801b838c))
#define DAT_801b839c   (*(int32_t  *)guest_at(0x801b839c))
#define DAT_801b83e0   (*(uint32_t *)guest_at(0x801b83e0))

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
 * port has no guest stack, so stage the 8-byte rect in the arg-build
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

void gpu_pkt_unlink_n(uint *param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  uint *hp;
  uint32_t p1g, p1;
  
  /* param_1 arrives as a host pointer from the clean_table wrapper
   * (guest_at(gpr4)) but as a raw guest-address value from the game.c/
   * menu.c callers — resolve either form. The link value walked below
   * is the guest address's low 24 bits. */
  p1g = guest_of(param_1);
  p1 = p1g ? p1g : (uint32_t)(uintptr_t)param_1;
  hp = (uint *)guest_at(p1);
  uVar2 = p1 & 0xffffff;
  do {
    uVar1 = *hp;
    uVar2 = uVar2 - 4;
    puVar3 = hp;
    if (uVar1 == uVar2) {
      do {
        puVar3 = puVar3 + -1;
        uVar1 = *puVar3;
        param_2 = param_2 + -1;
        uVar2 = uVar2 - 4;
        if (param_2 < 1) break;
      } while (uVar1 == uVar2);
      *hp = uVar1;
    }
    param_2 = param_2 + -1;
    if (uVar1 == 0xffffff) {
      return;
    }
    hp = puVar3 + -1;
    if (param_2 < 1) {
      return;
    }
  } while( true );
}

/* ======== 0x8018bf40 ======== */

void spr_draw_off(int param_1,short param_2,short param_3,int param_4)

{
  /* Guest builds one contiguous 8-byte RECT at sp+0x10..0x17
   * (sh @0x10,@0x12,@0x14,@0x16, disasm 0x8018bf40: frame 0x20) and
   * gpu_load_image_cmd reads it through generated GPU frames — stage it
   * in a reserved guest frame (sp+0x10 -> frm+0x10) or the callee's own
   * frame lands on the old sp-0x10 scratch window. */
  uint16_t local_10[4];
  uint8_t *hp;
  
  /* param_1 is a guest address (callers pass the heap cursor
   * _DAT_801e3cfc); resolve before dereferencing. param_1 + 0x14
   * stays a guest address for gpu_load_image_cmd's a1. */
  hp = (uint8_t *)guest_at((uint32_t)param_1);
  if ((*(uint *)(hp + 4) & 8) != 0) {
    param_1 = param_1 + *(int *)(hp + 8);
    hp = (uint8_t *)guest_at((uint32_t)param_1);
  }
  local_10[0] = (uint16_t)param_2;
  local_10[1] = (uint16_t)param_3;
  if (param_4 == 0) {
    local_10[0] = (uint16_t)(*(short *)(hp + 0xc) + param_2);
    local_10[1] = (uint16_t)(*(short *)(hp + 0xe) + param_3);
  }
  local_10[2] = *(undefined2 *)(hp + 0x10);
  local_10[3] = *(undefined2 *)(hp + 0x12);
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t sdo_frm = br2_guest_frame(0x20);
    memcpy(guest_at(sdo_frm + 0x10), local_10, 8);
    gpu_load_image_cmd(sdo_frm + 0x10,(undefined4)(param_1 + 0x14));
    br2_guest_frame_free();
  }
  return;
}

/* ======== 0x8018bec0 ======== */

void spr_draw_cond(int param_1,short param_2,short param_3,int param_4)

{
  uint16_t local_10[4];
  uint8_t *hp;
  
  /* param_1 is a guest address; see spr_draw_off. */
  hp = (uint8_t *)guest_at((uint32_t)param_1);
  if ((*(uint *)(hp + 4) & 8) != 0) {
    local_10[0] = (uint16_t)param_2;
    local_10[1] = (uint16_t)param_3;
    if (param_4 == 0) {
      local_10[0] = (uint16_t)(*(short *)(hp + 0xc) + param_2);
      local_10[1] = (uint16_t)(*(short *)(hp + 0xe) + param_3);
    }
    local_10[2] = *(undefined2 *)(hp + 0x10);
    local_10[3] = *(undefined2 *)(hp + 0x12);
    /* Same guest frame as spr_draw_off (0x8018bec0: frame 0x20, rect at
     * sp+0x10) — reserved frame staging, see spr_draw_off. */
    {
      extern uint32_t br2_guest_frame(uint32_t);
      extern void br2_guest_frame_free(void);
      uint32_t sdc_frm = br2_guest_frame(0x20);
      memcpy(guest_at(sdc_frm + 0x10), local_10, 8);
      gpu_load_image_cmd(sdc_frm + 0x10,(undefined4)(param_1 + 0x14));
      br2_guest_frame_free();
    }
  }
  return;
}

/* ======== 0x8018bfcc ======== */

void spr_draw_pair(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6,undefined4 param_7)

{
  spr_draw_off(param_1,param_5,param_6,param_7);
  spr_draw_cond(param_1,param_2,param_3,param_4);
  return;
}

/* ======== 0x8018c034 ======== */

void spr_desc_tbl_a_init(void)

{
  int iVar2;
  uint8_t *puVar1;
  
  if (DAT_801b55dc < 0) {
    iVar2 = 0x1f;
    puVar1 = (uint8_t *)&DAT_801f642c;
    do {
      iVar2 = iVar2 + -1;
      *(uint32_t *)puVar1 = (uint32_t)(_DAT_1f8000dc - 2);
      puVar1[7] = 9;
      puVar1[0xb] = 0x2f;
      puVar1[8] = 0x80;
      puVar1[9] = 0x80;
      puVar1[10] = 0x80;
      puVar1 = puVar1 + -0x2c;
    } while (-1 < iVar2);
    DAT_801b55dc = 0;
  }
  return;
}

/* ======== 0x8018c118 ======== */

void spr_desc_tbl_b_init(void)

{
  int iVar2;
  uint8_t *puVar1;
  
  if (DAT_801b55e0 < 0) {
    iVar2 = 0x1f;
    puVar1 = (uint8_t *)&DAT_801f68b4;
    do {
      iVar2 = iVar2 + -1;
      *(uint32_t *)puVar1 = (uint32_t)(_DAT_1f8000dc - 2);
      puVar1[7] = 4;
      puVar1[0xb] = 0x67;
      puVar1[8] = 0x80;
      puVar1[9] = 0x80;
      puVar1[10] = 0x80;
      puVar1 = puVar1 + -0x24;
    } while (-1 < iVar2);
    DAT_801b55e0 = 0;
  }
  return;
}

/* ======== 0x8018c18c ======== */

/* Guest returned a 32-bit guest pointer; widened to host pointer —
 * sole caller spr_slot_pkt_submit is in this file. */
void *spr_desc_slot_alloc(void)

{
  uint32_t uVar1;
  int32_t iVar3;
  uint32_t *puVar4;
  int32_t iVar2;
  
  uVar1 = (uint32_t)DAT_801b55e0 * 0x24;
  puVar4 = (uint32_t *)guest_at(0x801f6458 + uVar1);
  iVar3 = (int32_t)*puVar4;
  if ((iVar3 != (int32_t)_DAT_1f8000dc) &&
      (iVar2 = DAT_801b55e0 + 1, iVar3 != (int32_t)(_DAT_1f8000dc - 1))) {
    if (0x1f < iVar2) {
      iVar2 = 0;
    }
    DAT_801b55e0 = iVar2;
    *puVar4 = _DAT_1f8000dc;
    return guest_at(0x801f645c + uVar1);
  }
  return (void *)0;
}

/* ======== 0x8018c824 ======== */

undefined4 spr_slot_pkt_submit(uint *param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  uint *puVar1;
  undefined4 uVar2;
  uint32_t guest;
  uint32_t *cell;
  
  /* param_1 carries either a guest address value (direct clean callers
   * pass _DAT_1f800008+n) or a host pointer (clean_table.c wrapper passes
   * guest_at(gpr4)). guest_of() resolves the host form; fall back to the
   * raw value otherwise. */
  {
    uint32_t p1g = guest_of(param_1);
    cell = (uint32_t *)guest_at(p1g ? p1g : (uint32_t)(uintptr_t)param_1);
  }
  puVar1 = (uint *)spr_desc_slot_alloc();
  if (puVar1 == (uint *)0x0) {
    uVar2 = 0xffffffff;
  }
  else {
    guest = guest_of(puVar1);
    *(ushort *)(puVar1 + 4) = ((ushort)((uint)param_3 >> 0x10) & 0xff) + 1;
    puVar1[3] = param_4;
    *(ushort *)((char *)puVar1 + 0x12) = (byte)((uint)param_3 >> 0x18) + 1;
    *(short *)(puVar1 + 2) = (short)param_2;
    *(short *)((char *)puVar1 + 10) = (short)((uint)param_2 >> 0x10);
    *(byte *)((char *)puVar1 + 7) = *(byte *)((char *)puVar1 + 7) & 0xfd | 1;
    puVar1[1] = 0x64808080;
    prim_packet_init_tex((int)guest_of(puVar1 + 5),DAT_801e40b7,DAT_801e40b6,(int)(short)param_3,0);
    uVar2 = 0;
    *puVar1 = *cell | 0x4000000;
    *cell = guest & 0xffffff;
    puVar1[5] = guest & 0xffffff | 0x2000000;
    *cell = (guest + 0x14) & 0xffffff;
  }
  return uVar2;
}

/* ======== GPU command wrappers (libgpu-style dispatch) ======== */

/* ======== 0x80191428 ======== */

void gpu_load_image_cmd(undefined4 param_1,undefined4 param_2)

{
  gpu_rect_bounds_dbg(guest_of(s_LoadImage_80125504),(short *)(uintptr_t)param_1);
  gpu_call(8,GPU_TBL()[8],param_1,8,param_2);
  return;
}

/* ======== 0x80191300 ======== */

void gpu_clear_image_cmd(undefined4 param_1,uint param_2,uint param_3,uint param_4)

{
  gpu_rect_bounds_dbg(guest_of(s_ClearImage_801254ec),(short *)(uintptr_t)param_1);
  gpu_call(8,GPU_TBL()[3],param_1,8,
               (param_4 & 0xff) << 0x10 | (param_3 & 0xff) << 8 | param_2 & 0xff);
  return;
}

/* ======== 0x80191390 ======== */

void gpu_clear_image_cmd2(undefined4 param_1,uint param_2,uint param_3,uint param_4)

{
  gpu_rect_bounds_dbg(guest_of(s_ClearImage2_801254f8),(short *)(uintptr_t)param_1);
  gpu_call(8,GPU_TBL()[3],param_1,8,
               (param_4 & 0xff) << 0x10 | (param_3 & 0xff) << 8 | 0x80000000 | param_2 & 0xff);
  return;
}

/* ======== 0x80191668 ======== */

void gpu_clear_otag_r(uint32_t param_1,uint32_t param_2)

{
  if (1 < DAT_801b8322) {
    GPU_DBG(s_ClearOTagR__08x__d_____80125540,param_1,param_2);
  }
  /* jalr t[11] with a0=param_1,a1=param_2 (a2/a3 = caller leftovers in
   * guest; 0 here — ClearOTagR impl reads only a0/a1). */
  gpu_call(0x2c,param_1,param_2,0,0);
  DAT_801b83e0 = 0x41b83cc;
  *(uint32_t *)guest_at(param_1) = 0x1b83e0;
  return;
}

/* ======== 0x80191770 ======== */

void gpu_draw_otag(undefined4 param_1)

{
  if (1 < DAT_801b8322) {
    GPU_DBG(s_DrawOTag__08x_____80125558,param_1);
  }
  gpu_call(8,GPU_TBL()[6],param_1,0,0);
  return;
}

/* ======== 0x801917e0 ======== */

int gpu_draw_env_put(int param_1)

{
  undefined *puVar1;
  intptr_t hp;
  
  hp = (intptr_t)guest_at((uint32_t)param_1);
  if (1 < DAT_801b8322) {
    GPU_DBG(s_PutDrawEnv__08x_____8012556c,param_1);
  }
  gpu_prim_pkt_build(param_1 + 0x1c,(ushort *)(uintptr_t)param_1);
  *(uint32_t *)((char *)hp + 0x1c) = *(uint32_t *)((char *)hp + 0x1c) | 0xffffff;
  gpu_call(8,GPU_TBL()[6],param_1 + 0x1c,0x40,0);
  guest_memcpy((uint32_t *)0x801b8330,(uint32_t *)param_1,0x5c);
  return param_1;
}

/* ======== 0x80191ed8 ======== */

uint gpu_stat_sign(void)

{
  uint uVar1;
  
  uVar1 = gpu_call(0x38,0,0,0,0);
  return uVar1 >> 0x1f;
}

/* ======== 0x801910e4 ======== */

void gpu_disp_mask_set(int param_1)

{
  undefined4 uVar1;
  
  if (1 < DAT_801b8322) {
    GPU_DBG(s_SetDispMask__d_____801254a0,param_1);
  }
  if (param_1 == 0) {
    guest_memset((uint32_t *)0x801b838c,0xff,0x14);
  }
  uVar1 = 0x3000001;
  if (param_1 != 0) {
    uVar1 = 0x3000000;
  }
  gpu_call(0x10,uVar1,0,0,0);
  return;
}

/* ======== 0x8019117c ======== */

void gpu_draw_sync(undefined4 param_1)

{
  if (1 < DAT_801b8322) {
    GPU_DBG(s_DrawSync__d_____801254b4,param_1);
  }
  /* jalr t[15] with a0=param_1 (delay slot 0x801911D0: move a0,s0). */
  gpu_call(0x3c,param_1,0,0,0);
  return;
}

/* ======== 0x80190e00 ======== */

uint gpu_graph_reset(uint param_1)

{
  uint uVar1;
  
  uVar1 = param_1 & 7;
  if (uVar1 == 3) {
LAB_80190e4c:
    printf_forward(s_ResetGraph_jtb__08x_env__08x_80125410,
                 (uint32_t *)0x801b82d8,(uint32_t *)0x801b8320);
LAB_80190e68:
    guest_memset((uint32_t *)0x801b8320,0,0x80);
    cd_dispatch_tblc();
    bios_call_a0((*(uint32_t *)guest_at(0x801b8318)) & 0xffffff);
    DAT_801b8320 = (uint8_t)gpu_mode_regs_apply(param_1);
    DAT_801b8321 = 1;
    DAT_801b8324 = *(undefined2 *)((char *)guest_at(0x801b83a0) + (uint)DAT_801b8320 * 4);
    DAT_801b8326 = *(undefined2 *)((char *)guest_at(0x801b83ac) + (uint)DAT_801b8320 * 4);
    guest_memset((uint32_t *)0x801b8330,0xff,0x5c);
    guest_memset((uint32_t *)0x801b838c,0xff,0x14);
    uVar1 = (uint)DAT_801b8320;
  }
  else {
    if (uVar1 < 4) {
      if (uVar1 == 0) goto LAB_80190e4c;
    }
    else if (uVar1 == 5) goto LAB_80190e68;
    if (1 < DAT_801b8322) {
      GPU_DBG(s_ResetGraph__d_____80125430,param_1);
    }
    /* jalr t[13] with a0=1 (delay slot 0x80190F5C: addiu a0,zero,1);
     * a1 = leftover (s1=param_1 on debug path, caller a1 otherwise). */
    uVar1 = gpu_call(0x34,1,0,0,0);
  }
  return uVar1;
}

/* ======== 0x801919ac ======== */

/* Draw-env -> GPU display timing. param_1 is a guest address of the
 * 20-byte env struct (callers pass 0x801e40fc/0x801e624c-style addrs);
 * bridged to host here. Returns the host env pointer. */
uint32_t draw_env_timing(uint32_t param_1)

{
  ushort uVar1;
  bool bVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  ushort *env;
  uint16_t *t16;
  uint8_t  *t8;
  
  env = (ushort *)guest_at(param_1);
  if (1 < DAT_801b8322) {
    GPU_DBG(s_PutDispEnv__08x_____801255a0,param_1);
  }
  gpu_call(0x10,((env[1] & 0x3ff) << 10 | env[0] & 0x3ff | 0x5000000),0,0,0);
  if ((((DB32(0x801b839c) != *(int *)(env + 8)) || (DB16(0x801b838c) != env[0])) ||
      (DB16(0x801b838e) != env[1])) ||
     ((DB16(0x801b8390) != env[2] || (DB16(0x801b8392) != env[3])))) {
    cVar3 = gp_bde20_read();
    *(char *)(env + 9) = cVar3;
    /* s0 = 0x08000000 unconditionally (lui @0x801919DC delay slot);
     * ori s0,s0,8 @0x80191AC0 only when cVar3==1. */
    uVar10 = 0x08000000;
    if (cVar3 == '\x01') {
      uVar10 |= 8;
    }
    if (((uint8_t *)env)[0x11] != 0) {   /* lbu @env+0x11 (0x80191AC4) */
      uVar10 = uVar10 | 0x10;
    }
    if ((char)env[8] != '\0') {
      uVar10 = uVar10 | 0x20;
    }
    if (DAT_801b8323 != '\0') {
      uVar10 = uVar10 | 0x80;
    }
    uVar1 = env[2];
    if (0x118 < (short)uVar1) {
      if ((short)uVar1 < 0x161) {
        uVar10 = uVar10 | 1;
      }
      else if ((short)uVar1 < 0x191) {
        uVar10 = uVar10 | 0x40;
      }
      else if ((short)uVar1 < 0x231) {
        uVar10 = uVar10 | 2;
      }
      else {
        uVar10 = uVar10 | 3;
      }
    }
    bVar2 = (short)env[3] < 0x121;
    if ((char)env[9] == '\0') {
      bVar2 = (short)env[3] < 0x101;
    }
    if (!bVar2) {
      uVar10 = uVar10 | 0x24;
    }
    gpu_call(0x10,uVar10,0,0,0);
    *(undefined1 *)(env + 9) = 8;
  }
  if (((DB16(0x801b8394) == env[4]) && (DB16(0x801b8396) == env[5])) &&
     ((DB16(0x801b8398) == env[6] && ((DB16(0x801b839a) == env[7] && ((char)env[9] != '\b'))))))
   goto LAB_80191e74;
  cVar3 = gp_bde20_read();
  *(char *)(env + 9) = cVar3;
  uVar10 = (int)(short)env[5] + 0x13;
  if (cVar3 == '\0') {
    uVar10 = (int)(short)env[5] + 0x10;
  }
  uVar11 = uVar10 + (int)(short)env[7];
  if ((short)env[7] == 0) {
    uVar11 = uVar10 + 0xf0;
  }
  uVar1 = env[2];
  iVar9 = 0;
  if ((((0x118 < (short)uVar1) && (iVar9 = 1, 0x160 < (short)uVar1)) &&
      (iVar9 = 2, 400 < (short)uVar1)) && (iVar9 = 4, (short)uVar1 < 0x231)) {
    iVar9 = 3;
  }
  iVar4 = ((uint)(byte)env[9] * 5 + iVar9) * 4;
  t16 = (uint16_t *)guest_at(0x801b83f4);
  /* iVar4 is a BYTE offset into the u16 timing table (MIPS: lhu @base+iVar4),
   * not a u16 element index — divide by 2 for t16[] indexing. */
  iVar5 = (int)t16[iVar4 / 2 + 1] - (int)t16[iVar4 / 2];
  uVar7 = (uint)t16[iVar4 / 2] +
          (int)(short)env[4] * (uint)(byte)((uint8_t *)guest_at(0x801b841c))[iVar9];
  if ((short)env[6] != 0) {
    iVar5 = iVar5 * (short)env[6] >> 8;
  }
  uVar6 = uVar7 + iVar5;
  if ((char)env[9] == '\0') {
    uVar8 = 500;
    if ((499 < (int)uVar7) && (uVar8 = 0xcb2, (int)uVar7 < 0xcb3)) {
      uVar8 = uVar7;
    }
    uVar7 = uVar8 + (uint)(byte)((uint8_t *)guest_at(0x801b841c))[iVar9] * 4;
    if (((int)uVar7 <= (int)uVar6) && (uVar7 = 0xcda, (int)uVar6 < 0xcdb)) {
      uVar7 = uVar6;
    }
    if ((int)uVar10 < 0x10) {
      uVar6 = 0x10;
    }
    else {
      uVar6 = 0x101;
      if ((int)uVar10 < 0x102) {
        uVar6 = uVar10;
      }
    }
    uVar10 = uVar6 + 2;
    bVar2 = (int)uVar11 < 0x103;
    if ((int)uVar10 <= (int)uVar11) {
      uVar10 = 0x102;
      goto LAB_80191e0c;
    }
  }
  else {
    uVar8 = 0x21c;
    if ((0x21b < (int)uVar7) && (uVar8 = 0xc94, (int)uVar7 < 0xc95)) {
      uVar8 = uVar7;
    }
    uVar7 = uVar8 + (uint)(byte)((uint8_t *)guest_at(0x801b841c))[iVar9] * 4;
    if (((int)uVar7 <= (int)uVar6) && (uVar7 = 0xcbc, (int)uVar6 < 0xcbd)) {
      uVar7 = uVar6;
    }
    if ((int)uVar10 < 0x13) {
      uVar6 = 0x13;
    }
    else {
      uVar6 = 0x12f;
      if ((int)uVar10 < 0x130) {
        uVar6 = uVar10;
      }
    }
    bVar2 = (int)uVar11 < 0x132;
    uVar10 = uVar6 + 2;
    if ((int)(uVar6 + 2) <= (int)uVar11) {
      uVar10 = 0x131;
LAB_80191e0c:
      if (bVar2) {
        uVar10 = uVar11;
      }
    }
  }
  gpu_call(0x10,(uVar7 & 0xfff) << 0xc | uVar8 & 0xfff | 0x6000000,0,0,0);
  gpu_call(0x10,(uVar10 & 0x3ff) << 10 | uVar6 & 0x3ff | 0x7000000,0,0,0);
LAB_80191e74:
  guest_memcpy((uint32_t *)0x801b838c,(uint32_t *)param_1,0x14);
  return param_1;
}

/* ======== batch27: GPU/env helpers ======== */

extern uint32_t gpu_coord_clamp_a(uint32_t,uint32_t);
extern uint32_t gpu_coord_clamp_b(uint32_t,uint32_t);
extern uint32_t gpu_coord_pack(int,int);
extern uint32_t gpu_pkt_pack(uint8_t,char,uint16_t);
extern uint32_t gpu_texpage_pack(uint16_t *);
extern uint32_t cd_u16_swap_store(int);
extern uint32_t gpu_status_cmd(uint);
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

void guest_memset(uint32_t *param_1,undefined1 param_2,int param_3)

{
  int iVar1;
  /* param_1: host pointer from clean_table wrapper, guest-address value
   * from in-file callers (0x801b83xx constants) — resolve either form. */
  uint32_t g1 = guest_of(param_1);
  uint8_t *d = (uint8_t *)guest_at(g1 ? g1 : (uint32_t)(uintptr_t)param_1);
  
  iVar1 = param_3 + -1;
  if (param_3 != 0) {
    do {
      *d = param_2;
      iVar1 = iVar1 + -1;
      d = d + 1;
    } while (iVar1 != -1);
  }
  return;
}

/* ======== 0x8018e490 ======== */

undefined1 * guest_memcpy(uint32_t *param_1,uint32_t *param_2,int param_3)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  uint8_t *d, *s;
  
  puVar3 = (undefined1 *)0x0;
  /* params arrive as host pointers from the clean_table wrapper but as
   * guest-address values from cd.c callers — resolve either form. */
  {
    uint32_t g1 = guest_of(param_1);
    uint32_t g2 = guest_of(param_2);
    d = (uint8_t *)guest_at(g1 ? g1 : (uint32_t)(uintptr_t)param_1);
    s = (uint8_t *)guest_at(g2 ? g2 : (uint32_t)(uintptr_t)param_2);
  }
  puVar2 = d;
  if (param_1 != (uint32_t *)0x0) {
    for (; puVar3 = (undefined1 *)d, 0 < param_3; param_3 = param_3 + -1) {
      uVar1 = *s;
      s = s + 1;
      *puVar2 = uVar1;
      puVar2 = (undefined1 *)(puVar2 + 1);
    }
  }
  return puVar3;
}

/* ======== 0x801921c0 ======== */

void gpu_prim_pkt_build(int param_1,ushort *param_2)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  undefined4 uVar4;
  char cVar5;
  undefined4 local_18;
  undefined4 local_14;
  uint8_t *dst;
  ushort *env;
  
  /* param_2: host pointer from clean_table wrapper, guest-address value
   * from gpu_draw_env_put — resolve either form. param_1 stays a guest
   * address value in both paths. */
  uint32_t p2g = guest_of(param_2);
  dst = (uint8_t *)guest_at((uint32_t)param_1);
  env = (ushort *)guest_at(p2g ? p2g : (uint32_t)(uintptr_t)param_2);
  uVar4 = gpu_coord_clamp_a((int)(short)env[0],(int)(short)env[1]);
  *(undefined4 *)(dst + 4) = uVar4;
  uVar4 = gpu_coord_clamp_b((int)(((uint)env[2] + (uint)env[0] + -1) * 0x10000) >> 0x10,
                       (int)(((uint)env[1] + (uint)env[3] + -1) * 0x10000) >> 0x10);
  *(undefined4 *)(dst + 8) = uVar4;
  uVar4 = gpu_coord_pack((int)(short)env[4],(int)(short)env[5]);
  *(undefined4 *)(dst + 0xc) = uVar4;
  uVar4 = gpu_pkt_pack(*(uint8_t *)((char *)env + 0x17),(char)env[0xb],env[10]);
  *(undefined4 *)(dst + 0x10) = uVar4;
  uVar4 = gpu_texpage_pack(env + 6);
  *(undefined4 *)(dst + 0x14) = uVar4;
  *(undefined4 *)(dst + 0x18) = 0xe6000000;
  cVar5 = '\a';
  if ((char)env[0xc] != '\0') {
    uVar4 = *(undefined4 *)env;
    uVar3 = env[2];
    uVar1 = env[3];
    uVar2 = 0;
    if ((-1 < (short)uVar3) && (uVar2 = DAT_801b8324 - 1, (int)(short)uVar3 <= DAT_801b8324 + -1)) {
      uVar2 = uVar3;
    }
    if ((short)uVar1 < 0) {
      uVar3 = 0;
    }
    else {
      uVar3 = DAT_801b8326 - 1;
      if ((int)(short)uVar1 <= DAT_801b8326 + -1) {
        uVar3 = uVar1;
      }
    }
    local_14 = ((uint32_t)uVar3 << 16) | uVar2;
    if (((env[0] & 0x3f) == 0) && ((uVar2 & 0x3f) == 0)) {
      cVar5 = '\n';
      *(uint *)(dst + 0x1c) =
           (uint)*(uint8_t *)((char *)env + 0x1b) << 0x10 | (uint)(uint8_t)env[0xd] << 8 | 0x2000000
           | (uint)*(uint8_t *)((char *)env + 0x19);
      *(undefined4 *)(dst + 0x20) = uVar4;
      *(undefined4 *)(dst + 0x24) = local_14;
    }
    else {
      cVar5 = '\n';
      local_18 = ((uint32_t)(uint16_t)(env[1] - env[5]) << 16) | (uint16_t)(env[0] - env[4]);
      *(uint *)(dst + 0x1c) =
           (uint)*(uint8_t *)((char *)env + 0x1b) << 0x10 | (uint)(uint8_t)env[0xd] << 8 | 0x60000000
           | (uint)*(uint8_t *)((char *)env + 0x19);
      *(undefined4 *)(dst + 0x20) = local_18;
      *(undefined4 *)(dst + 0x24) = local_14;
    }
  }
  *(char *)(dst + 3) = cVar5 + -1;
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
undefined4 gpu_mode_regs_apply(uint param_1)
{
  uint32_t mode = param_1 & 7u;
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
  if (mode == 0) r = gpu_status_cmd(param_1);
  return r;
}

/* ======== 0x80193d30 ======== */

void bios_call_a0(uint param_1)

{
  /* guest 0x80193d30: `li t2,0xa0; jr t2; li t1,0x49` — BIOS svc A0/0x49
   * with a0 = param_1 (caller's arg register). */
  extern uint32_t bios_svc6(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
  bios_svc6(0xa0,0x49,param_1,0,0,0);
  return;
}

/* ======== 0x801911e4 ======== */

void gpu_rect_bounds_dbg(undefined4 param_1,short *param_2)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  /* param_2 arrives as a host pointer from the clean_table wrapper
   * (guest_at(gpr5)) but as a guest-address value from gpu_load_image_cmd
   * family callers — resolve either form. */
  uint32_t p2g = guest_of(param_2);
  short *rc = (short *)guest_at(p2g ? p2g : (uint32_t)(uintptr_t)param_2);
  
  if (DAT_801b8322 == '\x01') {
    iVar3 = (int)rc[2];
    if ((iVar3 <= DAT_801b8324) && (iVar3 + rc[0] <= (int)DAT_801b8324)) {
      iVar1 = (int)rc[1];
      if (((iVar1 <= DAT_801b8326) &&
          (((iVar1 + rc[3] <= (int)DAT_801b8326 && (0 < iVar3)) && (-1 < rc[0])))) &&
         ((-1 < iVar1 && (0 < rc[3])))) {
        return;
      }
    }
    pcVar2 = s__s_bad_RECT_801254c8;
  }
  else {
    if (DAT_801b8322 != '\x02') {
      return;
    }
    pcVar2 = (char *)guest_at(0x801254e8);
  }
  gpu_dbg(pcVar2,param_1);
  gpu_dbg(s___d__d____d__d__801254d4,(int)rc[0],(int)rc[1],(int)rc[2],
              (int)rc[3]);
  return;
}

/* ======== 0x8019b1e4 ======== */

undefined4 gp_bde20_read(void)

{
  return GP32(0x801bde20);
}

/* ======== 0x8019b220 ======== */

void cd_chan_boot_init(int param_1)

{
  /* Keep clean: it must reach cd_ext_init_once() so the CD
   * OpenEvent(0xF0000009) init path runs. Delegating this body (and
   * cd_chan_boot_fwd) to generated was tried; the generated CPS path did not
   * dispatch cd_ext_init_once either, leaving the guard unset. */
  undefined2 *puVar1;
  int iVar2;
  uint16_t *t;
  
  cd_dispatch_tblc();
  cd_chan_reg_init(param_1);
  if (param_1 == 0) {
    iVar2 = 0x17;
    t = (uint16_t *)guest_at(0x801bde9a);
    puVar1 = t;
    do {
      *puVar1 = 0xc000;
      iVar2 = iVar2 + -1;
      puVar1 = puVar1 + -1;
    } while (-1 < iVar2);
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
void printf_forward(const char *param_1, ...)
{
  uint32_t args[8];
  va_list ap;
  int i;
  va_start(ap,param_1);
  for (i = 0; i < 8; i++) {
    uintptr_t v = va_arg(ap,uintptr_t);
    uint32_t g = guest_of((const void *)v);
    args[i] = g ? g : (uint32_t)v;
  }
  va_end(ap);
  printf_core(1,param_1,args);
}

/* sprite_attr_init_a — sprite/OAM attribute block init (u16 fields + tail
 * bytes). param_1 = host pointer to the attr struct. */
void sprite_attr_init_a(uint32_t param_1,uint16_t param_2,uint16_t param_3,
                  uint16_t param_4,int32_t param_5)
{
  int iVar2;
  uint16_t *p1 = (uint16_t *)guest_at(param_1);
  uint8_t *b = (uint8_t *)p1;
  iVar2 = gp_bde20_read();
  p1[0] = param_2;
  p1[1] = param_3;
  p1[2] = param_4;
  p1[6] = 0;
  p1[7] = 0;
  p1[8] = 0;
  p1[9] = 0;
  b[0x19] = 0;
  b[0x1a] = 0;
  b[0x1b] = 0;
  b[0x16] = 1;
  p1[3] = (int16_t)param_5;
  b[0x17] = (uint8_t)(iVar2 == 0 ? param_5 < 0x101 : param_5 < 0x121);
  p1[4] = param_2;
  p1[5] = param_3;
  p1[10] = 10;
  b[0x18] = 0;
}

/* sprite_attr_init_b — second sprite-attr variant init. */
void sprite_attr_init_b(uint32_t param_1,uint16_t param_2,uint16_t param_3,
                  uint16_t param_4,uint16_t param_5)
{
  uint16_t *p1 = (uint16_t *)guest_at(param_1);
  uint8_t *b = (uint8_t *)p1;
  p1[0] = param_2;
  p1[1] = param_3;
  p1[2] = param_4;
  p1[4] = 0;
  p1[5] = 0;
  p1[6] = 0;
  p1[7] = 0;
  b[0x11] = 0;
  b[0x10] = 0;
  b[0x13] = 0;
  b[0x12] = 0;
  p1[3] = param_5;
}

/* ---- batch29: sprite-attr bit helpers (guest-addr convention) ---- */

/* bitfield_or_write — unaligned bitfield OR-write: inserts param_2 (8-bit
 * value) at bit offset derived from param_1 alignment. param_1 is a
 * host pointer (sole caller hud_sprite_desc_init passes arena ptrs). */
void bitfield_or_write(const void *param_1,int param_2)
{
  uintptr_t a = (uintptr_t)param_1;
  uint32_t uVar1 = (uint32_t)(a + 2) & 3;
  uint32_t *puVar2 = (uint32_t *)((a + 2) - uVar1);
  *puVar2 = *puVar2 & (0xffffffffu << ((uVar1 + 1) * 8)) |
           (uint32_t)((uint32_t)param_2 << 8) >> ((3 - uVar1) * 8);
}

/* spr_desc_a/4060/4090 — per-slot sprite descriptor helpers.
 * param_1 = guest address of a 0x28-byte slot record. */
void spr_desc_a(uint32_t param_1)
{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  p[3] = 9;
  p[7] = 0x2c;
}

void spr_desc_b(uint32_t param_1,uint32_t param_2)
{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  if (param_2 == 0) p[7] &= 0xfd;
  else               p[7] |= 2;
}

void spr_desc_c(uint32_t param_1,uint32_t param_2)
{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  if (param_2 == 0) p[7] &= 0xfe;
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
void text_win_init(uint32_t param_1,int32_t param_2)
{
  GB32(0x801fad12) = gpu_sprite_rect_emit(0x801b78c8,param_1,(uint32_t)(param_2 + 0x80));
  GB32(0x801fad10) = gpu_sprite_rect_emit_mode(0x801b7ac8,0,0,param_1,(uint32_t)param_2,0x80,0x20);
  GB8(0x801b78c0) = 0;
  guest_memset_b(0x801b7740,0,0x180);
}

/* text_line_select — select active text line (bounds vs DAT_801b78c0). */
void text_line_select(int32_t param_1)
{
  if ((0 <= param_1) && (param_1 <= (int32_t)GB8(0x801b78c0))) {
    PTR_FUN_801b831c = (void *)text_win_printf;
    GB8(0x801b78c4) = (uint8_t)param_1;
  }
}

/* text_widget_alloc — allocate one 0x30-byte text-string widget slot.
 * Returns slot index or -1 when DAT_801b78c0 >= 8. */
int32_t text_widget_alloc(uint16_t param_1,uint16_t param_2,int32_t param_3,
                     uint16_t param_4,int32_t param_5,int32_t param_6)
{
  int32_t iVar1, iVar2;
  /* guest sp-0x30..-0x28 is one contiguous 8-byte texcoord buffer read by
   * prim_packet_init_tex as bytes 0..7 (param_5[0..2] + shorts +4/+6).
   * Ghidra split the four halfwords into scalars; keep them contiguous. */
  uint16_t local_30[4];
  uint32_t slot;

  if ((int32_t)GB8(0x801b78c0) < 8) {
    if (GB8(0x801b78c0) == 0) GB32(0x801b82c8) = 0;
    slot = GB8(0x801b78c0) * 0x30;
    GB32(0x801b7740 + slot + 0x2c) = (param_3 == 0);
    iVar2 = (int32_t)GB32(0x801b82c8);
    if (0x400 < iVar2 + param_6) param_6 = 0x400 - iVar2;
    local_30[2] = 0x100; local_30[3] = 0x100;
    local_30[0] = 0;     local_30[1] = 0;
    prim_packet_init_tex((int)(0x801b7750 + slot),0,0,(uint32_t)GB32(0x801fad10),
                 (byte *)local_30);
    if (param_5 != 0) {
      gpu_pkt_tag_60(0x801b7740 + slot);
      GB8(0x801b7744 + slot) = 0;
      GB8(0x801b7745 + slot) = 0;
      GB8(0x801b7746 + slot) = 0;
      spr_desc_b(0x801b7740 + slot,(param_5 == 2));
    }
    iVar2 = (int32_t)GB32(0x801b82c8);
    slot = GB8(0x801b78c0) * 0x30;
    GB16(0x801b7748 + slot) = param_1;
    GB16(0x801b774a + slot) = param_2;
    GB16S(0x801b774c + slot) = (int16_t)param_3;
    GB16(0x801b774e + slot) = param_4;
    GB32S(0x801b775c + slot) = param_6;
    GB32(0x801b7768 + slot) = 0;
    GB32S(0x801b7764 + slot) = iVar2 + (int32_t)0x801f6910;
    GB32S(0x801b7760 + slot) = iVar2 * 0x10 + (int32_t)0x801f6d10;
    *(uint8_t *)guest_at(GB32(0x801b7764 + slot)) = 0;
    iVar2 = GB32S(0x801b7760 + GB8(0x801b78c0) * 0x30);
    iVar1 = 0;
    if (0 < param_6) {
      do {
        gpu_pkt_tag_74((uint32_t)iVar2);
        iVar1 = iVar1 + 1;
        GB16((uint32_t)iVar2 + 0xe) = GB16(0x801fad12);
        iVar2 += 0x10;
      } while (iVar1 < param_6);
    }
    GB32(0x801b82c8) = GB32(0x801b82c8) + (uint32_t)param_6;
    iVar2 = GB8(0x801b78c0);
    GB8(0x801b78c0) = GB8(0x801b78c0) + 1;
  } else {
    iVar2 = -1;
  }
  return iVar2;
}

/* spr_attr_alloc_a — sprite-attr slot alloc with 0x5000000/0x28000000|
 * 0x2a000000 tag packing (same cluster as spr_desc_slot_alloc/c824).
 * param_1 = guest address of the head pointer cell. */
extern void *spr_pool_a_alloc(void);
uint32_t spr_attr_alloc_a(uint32_t param_1,int16_t param_2,int16_t param_3,
                      int16_t param_4,int16_t param_5,uint32_t param_6,
                      int32_t param_7)
{
  uint32_t *puVar1;
  uint32_t uVar2;
  int32_t iVar3;
  puVar1 = (uint32_t *)spr_pool_a_alloc();
  if (puVar1 == (uint32_t *)0x0) {
    uVar2 = 0xffffffff;
  } else {
    uint8_t *b = (uint8_t *)puVar1;
    *(int16_t *)(b + 8) = param_2;
    *(int16_t *)(b + 10) = param_3;
    *(int16_t *)(b + 12) = (int16_t)(param_2 + param_4);
    *(int16_t *)(b + 14) = param_3;
    *(int16_t *)(b + 16) = param_2;
    *(int16_t *)(b + 18) = (int16_t)(param_2 + param_4);
    *(int16_t *)(b + 22) = (int16_t)(param_3 + param_5);
    *(int16_t *)(b + 26) = (int16_t)(param_3 + param_5);
    if (param_7 == 0) {
      puVar1[1] = param_6 | 0x28000000;
      iVar3 = 0;
    } else {
      puVar1[1] = param_6 | 0x2a000000;
      iVar3 = (param_7 + -1) * 0x20;
    }
    prim_packet_init_tex((int)guest_of(puVar1 + 6),GB8(0x801e40b7),GB8(0x801e40b6),iVar3,0);
    uVar2 = 0;
    uint32_t *head = (uint32_t *)guest_at(param_1);
    /* OT link word goes into the packet's first word (node[0] =
     * old_head | 0x05000000), NOT into the head cell — writing *head
     * here leaves node[0] uninitialised and the GPU linked-list DMA
     * follows a garbage next-pointer (versus/attract OT runaway). */
    puVar1[0] = *head | 0x05000000u;
    *head = guest_of(puVar1) & 0xffffff;
    puVar1[6] = guest_of(puVar1) & 0xffffff | 0x2000000;
    *head = guest_of(puVar1 + 6) & 0xffffff;
  }
  return uVar2;
}

/* spr_attr_alloc_b — sprite-attr slot alloc variant: fixed tag
 * 0x2c808080, *param_1 head cell gets 0x9000000 tag. Same
 * guest-addr + pointer-tag conventions as spr_attr_alloc_a. */
extern void *spr_pool_b_alloc(void);
uint32_t spr_attr_alloc_b(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                      uint32_t param_4,uint16_t param_5,uint16_t param_6)
{
  uint32_t *puVar1;
  uint32_t uVar2;
  uint16_t uVar3;
  int8_t cVar4, cVar5;
  puVar1 = (uint32_t *)spr_pool_b_alloc();
  if (puVar1 == (uint32_t *)0x0) {
    uVar2 = 0xffffffff;
  } else {
    uint8_t *b = (uint8_t *)puVar1;
    uVar2 = 0;
    cVar5 = (int8_t)param_4 + (int8_t)(param_3 >> 16) + 1;
    cVar4 = (int8_t)(param_4 >> 8);
    b[0x1c] = (uint8_t)param_4;
    b[0x15] = (uint8_t)cVar4;
    cVar4 = (int8_t)(cVar4 + (param_3 >> 24) + 1);
    puVar1[3] = param_4;                    /* byte 0x0c */
    *(int16_t *)(b + 0x16) = (int16_t)param_3;
    b[0x14] = (uint8_t)cVar5;
    b[0x1d] = (uint8_t)cVar4;
    b[0x24] = (uint8_t)cVar5;
    b[0x25] = (uint8_t)cVar4;
    *(int16_t *)(b + 0x08) = (int16_t)param_2;
    uVar3 = (uint16_t)(param_2 >> 16);
    *(uint16_t *)(b + 0x0a) = uVar3;
    *(uint16_t *)(b + 0x10) = param_5;
    *(uint16_t *)(b + 0x12) = uVar3;
    *(int16_t *)(b + 0x18) = (int16_t)param_2;
    *(uint16_t *)(b + 0x1a) = param_6;
    *(uint16_t *)(b + 0x20) = param_5;
    *(uint16_t *)(b + 0x22) = param_6;
    puVar1[1] = 0x2c808080;
    uint32_t *head = (uint32_t *)guest_at(param_1);
    /* node[0] = old_head | 0x09000000 (link word into the packet, not
     * the head cell) — same fix as spr_attr_alloc_a. */
    puVar1[0] = *head | 0x09000000u;
    *head = guest_of(puVar1) & 0xffffff;
  }
  return uVar2;
}

/* hud_gauge_render — HUD gauge/frame sprite renderer. param_1 = guest
 * addr of fighter-state block (+0xc attr u32, +0x20/+0x22 s16 x/y);
 * param_2 = guest addr of sprite-list head cell; param_3 low nibble =
 * palette set, bits 4-7 = layout variant. Emits spr_attr_alloc_b/8c824/
 * 8ce70/8c92c sprite records + hud_text_d tile blits. */
extern void hud_text_d(uint32_t,uint32_t,const void *,int32_t,int32_t,int32_t);
extern void sprite_node_emit_a(uint32_t,int32_t,int32_t,int32_t,int32_t,uint32_t,uint32_t,uint32_t,uint32_t,int32_t);
extern void sprite_node_emit_b(uint32_t,uint32_t,uint32_t,uint32_t,int32_t,int32_t);
void hud_gauge_render(uint32_t param_1,uint32_t param_2,uint32_t param_3)
{
  int16_t sVar1;
  int32_t iVar2, iVar4;
  uint32_t uVar3, uVar5, uVar6, uVar7, uVar8;
  uint8_t *st = (uint8_t *)guest_at(param_1);
  uVar3 = *(uint32_t *)(st + 0xc);
  switch (param_3 & 0xf) {
  default:
    uVar8 = 0xc; uVar7 = 0x20; uVar5 = 0x30; uVar6 = 0x40;
    goto LAB_8012c4f8;
  case 1:
    uVar8 = 0; uVar7 = 0x1000; uVar5 = 0x3000; uVar6 = 0x3000;
    iVar4 = 1;
    break;
  case 2:
    uVar8 = 0x100000; uVar7 = 0x300000; uVar5 = 0x400000; uVar6 = 0x600000;
    iVar4 = 2;
    break;
  case 3:
    uVar8 = 0x101010; uVar7 = 0x202020; uVar5 = 0x303030; uVar6 = 0x404040;
    iVar4 = 3;
    break;
  case 4:
    uVar8 = 0x8121c; uVar7 = 0x121c26; uVar5 = 0x1c2630; uVar6 = 0x26303a;
    iVar4 = 4;
    break;
  case 5:
    uVar8 = 0xc000c; uVar7 = 0x200020; uVar5 = 0x300030; uVar6 = 0x400040;
    iVar4 = 5;
    break;
  case 6:
    uVar8 = 0x408; uVar7 = 0xc18; uVar5 = 0x1020; uVar6 = 0x1830;
    iVar4 = 6;
    break;
  case 7:
    uVar8 = 0x80805; uVar7 = 0x18180f; uVar5 = 0x202014; uVar6 = 0x30301c;
    iVar4 = 7;
    break;
  case 8:
    uVar8 = 0; uVar7 = 0x80; uVar5 = 0x800000; uVar6 = 0x200000;
    goto LAB_8012c4f8;
  case 9:
    uVar8 = 0x800000; uVar7 = 0; uVar5 = 0x80; uVar6 = 0x20;
LAB_8012c4f8:
    iVar4 = 0;
  }
  param_3 = param_3 & 0xf0;
  if (param_3 == 0x20) {
LAB_8012c64c:
    iVar2 = param_2 + 4;
    spr_attr_alloc_b(iVar2,0x280000,0x177e001e,0x7ad68080,0x80,0x48);
    spr_attr_alloc_b(iVar2,0x280080,0x177e001e,0x7a968080,0x100,0x48);
    spr_attr_alloc_b(iVar2,0x280100,0x177e001e,0x7a568080,0x180,0x48);
    spr_attr_alloc_b(iVar2,0x280180,0x1780001e,0x7a5680ff,0x200,0x48);
    spr_slot_pkt_submit((uint *)guest_at(iVar2),(*(int16_t *)(st + 0x22) + -0x54) * 0x10000 |
                 ((uint32_t)*(int16_t *)(st + 0x20) - 0x90U) & 0xffff,0x277e001e,
                 ((uint32_t)(iVar4 << 6) | 0x7e16U) << 0x10 | 0x9880);
    sprite_node_emit_a(iVar2,0,*(int16_t *)(st + 0x22) + -0x40,(int32_t)*(int16_t *)(st + 0x20),0x18,
                 uVar5,0,uVar5,0,2);
    sprite_node_emit_b(iVar2,((uint32_t)(*(int16_t *)(st + 0x22) + -0x13f) * 0x10000) |
                 ((uint32_t)*(int16_t *)(st + 0x20) - 0xffU) & 0xffff,0xfefe001f,
                 ((uint32_t)(iVar4 << 6) | 0x7c16U) << 0x10,0x80,1);
LAB_8012cb64:
    sVar1 = *(int16_t *)(st + 0x20);
    iVar4 = (int32_t)*(int16_t *)(st + 0x22);
    iVar2 = 0;
  } else {
    if (param_3 < 0x21) {
      if (param_3 == 0) {
        iVar2 = param_2 + 4;
        hud_text_d(iVar2,uVar3,guest_at(0x8011f10c),0xc0,
                     (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x38) * 0x10000) >> 0x10,0);
        spr_slot_pkt_submit((uint *)guest_at(iVar2),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0xd8,0xf0f001e,0x7a16d830);
        spr_slot_pkt_submit((uint *)guest_at(iVar2),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0xe8,0xf0f001e,0x7a16d820);
        spr_slot_pkt_submit((uint *)guest_at(iVar2),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0xf8,0xf0f001e,0x7a16d810);
        hud_text_d(iVar2,uVar3,guest_at(0x8011f110),0x120,
                     (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x38) * 0x10000) >> 0x10,0);
        spr_slot_pkt_submit((uint *)guest_at(iVar2),((uint32_t)(*(int16_t *)(st + 0x22) + -0x3c) * 0x10000) | 0x148,0xf0f001e,0x7a16d800);
        goto LAB_8012c64c;
      }
      if (param_3 != 0x10) return;
      hud_text_d(param_2 + 4,uVar3,guest_at(0x8011f110),0x120,
                   (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x30) * 0x10000) >> 0x10,0);
      spr_slot_pkt_submit((uint *)guest_at(param_2 + 4),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0x148,0xf0f001e,0x7a16d800);
    } else {
      if (param_3 == 0x30) {
        iVar2 = param_2 + 4;
        spr_attr_alloc_b(iVar2,0x280000,0x177e001e,0x7ad68080,0x80,0x48);
        spr_attr_alloc_b(iVar2,0x280080,0x177e001e,0x7a968080,0x100,0x48);
        spr_attr_alloc_b(iVar2,0x280100,0x177e001e,0x7a568080,0x180,0x48);
        spr_attr_alloc_b(iVar2,0x280180,0x1780001e,0x7a5680ff,0x200,0x48);
        spr_slot_pkt_submit((uint *)guest_at(iVar2),((uint32_t)(*(int16_t *)(st + 0x22) + -0x54) * 0x10000) |
                     ((uint32_t)*(int16_t *)(st + 0x20) - 0x90U) & 0xffff,0x277e001e,
                     ((uint32_t)(iVar4 << 6) | 0x7e16U) << 0x10 | 0x9880);
        sprite_node_emit_a(iVar2,0,*(int16_t *)(st + 0x22) + -0x40,(int32_t)*(int16_t *)(st + 0x20),0x18,
                     uVar5,0,uVar5,0,2);
        goto LAB_8012cb64;
      }
      if (param_3 != 0x40) return;
    }
    iVar4 = param_2 + 4;
    hud_text_d(iVar4,uVar3,guest_at(0x8011f10c),0xc0,
                 (int32_t)(((uint32_t)*(uint16_t *)(st + 0x22) - 0x30) * 0x10000) >> 0x10,0);
    spr_slot_pkt_submit((uint *)guest_at(iVar4),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0xd8,0xf0f001e,0x7a16d830);
    spr_slot_pkt_submit((uint *)guest_at(iVar4),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0xe8,0xf0f001e,0x7a16d820);
    spr_slot_pkt_submit((uint *)guest_at(iVar4),((uint32_t)(*(int16_t *)(st + 0x22) + -0x34) * 0x10000) | 0xf8,0xf0f001e,0x7a16d810);
    spr_attr_alloc_b(param_2,0x200000,0x1780001e,0x7a5680ff,0x80,0x40);
    spr_attr_alloc_b(param_2,0x200080,0x1780001e,0x7a9680ff,0x100,0x40);
    spr_attr_alloc_b(param_2,0x200100,0x177e001e,0x7a968080,0x180,0x40);
    spr_attr_alloc_b(param_2,0x200180,0x177e001e,0x7a568080,0x200,0x40);
    spr_attr_alloc_a(iVar4,0,*(int16_t *)(st + 0x22) + -0x38,(int32_t)*(int16_t *)(st + 0x20),0x18,
                 uVar6,2);
    iVar2 = (*(int16_t *)(st + 0x22) * 3) / 5 + -8;
    sprite_node_emit_a(iVar4,0,0,(int32_t)*(int16_t *)(st + 0x20),iVar2,(int32_t)uVar8,(int32_t)uVar8,(int32_t)uVar7,(int32_t)uVar7,0);
    sVar1 = *(int16_t *)(st + 0x20);
    uVar8 = 0;
    iVar4 = *(int16_t *)(st + 0x22) - iVar2;
    uVar7 = uVar5;
    uVar6 = uVar5;
  }
  sprite_node_emit_a(param_2 + 4,0,iVar2,(int32_t)sVar1,iVar4,(int32_t)uVar7,(int32_t)uVar6,0,(int32_t)uVar8,0);
}

/* ======== 0x8018cbfc ======== */

/* Sprite-attr pool A allocator: 0x18 slots x 0x28 stride at
 * 0x801f5978, round-robin counter DAT_801b55e4, sentinel
 * _DAT_1f8000dc. Slot word0 == sentinel or sentinel-1 -> NULL;
 * else mark sentinel, bump counter (wrap at 0x18), return slot+4. */
void *spr_pool_a_alloc(void)
{
  int32_t iVar3;
  uint32_t *puVar4;
  int32_t iVar2;
  int32_t v1 = DAT_801b55e4;
  uint32_t sent = _DAT_1f8000dc;
  if (v1 < 0) {
    puVar4 = (uint32_t *)guest_at(0x801f5978);
    v1 = 0;
  } else {
    puVar4 = (uint32_t *)guest_at(0x801f5978 + (uint32_t)v1 * 0x28);
  }
  iVar3 = (int32_t)*puVar4;
  if (iVar3 == (int32_t)sent || iVar3 == (int32_t)(sent - 1))
    return (void *)0;
  iVar2 = v1 + 1;
  if (0x17 < iVar2) iVar2 = 0;
  DAT_801b55e4 = iVar2;
  *puVar4 = sent;
  return guest_at(0x801f5978 + (uint32_t)v1 * 0x28 + 4);
}

/* ======== 0x8018c0a8 ======== */

/* Sprite-attr pool B allocator: 0x20 slots x 0x2c stride at
 * 0x801f5ed8, counter DAT_801b55dc. Same sentinel protocol. */
void *spr_pool_b_alloc(void)
{
  int32_t iVar3;
  uint32_t *puVar4;
  int32_t iVar2;
  int32_t v1 = DAT_801b55dc;
  uint32_t sent = _DAT_1f8000dc;
  puVar4 = (uint32_t *)guest_at(0x801f5ed8 + (uint32_t)v1 * 0x2c);
  iVar3 = (int32_t)*puVar4;
  if (iVar3 == (int32_t)sent || iVar3 == (int32_t)(sent - 1))
    return (void *)0;
  iVar2 = v1 + 1;
  if (0x1f < iVar2) iVar2 = 0;
  DAT_801b55dc = iVar2;
  *puVar4 = sent;
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
extern uint8_t DAT_801b6701[];
uint32_t printf_core(uint32_t param_1,const uint8_t *param_2,const uint32_t *param_3)
{
  uint8_t bVar1, bVar2;
  char cVar3;
  int bVar4, bVar5, bVar6, bVar7;
  int32_t iVar8;
  uint32_t uVar9, uVar10, uVar16, uVar22;
  uint32_t *puVar12, *puVar13;
  const uint32_t *puVar11_;
  char *pcVar15;
  uint32_t *puVar17, *puVar18, *puVar19;
  const uint8_t *pbVar20;
  int32_t iVar21;
  intptr_t local_40, local_3c, local_38, local_34;
  char local_68[40];
  intptr_t puVar17i;
  char *pcVar15w;
  (void)param_1;
  if (param_2 == 0) return 0;
  const char *local_30 = s_0123456789abcdef_80124fe8;
  local_40 = 0;
LAB_8018e5e0:
  uVar9 = (uint32_t)*param_2;
  if (uVar9 != 0) {
    uVar16 = 0;
    bVar4 = 0; bVar7 = 0; bVar6 = 0; bVar5 = 0;
    if (uVar9 == 0x25) {
      puVar17i = -1;
      iVar21 = 0;
      local_3c = 0;
      local_38 = 0;
      local_34 = 0;
      puVar13 = (uint32_t *)param_3;
      pbVar20 = param_2;
LAB_8018e60c:
      param_2 = pbVar20 + 1;
      puVar12 = puVar13;
      switch (*param_2) {
      case 0:
        goto switchD_8018e634_caseD_0;
      default:
        uVar9 = (uint32_t)(char)*param_2;
        local_40 = local_40 + 1;
        param_3 = (const uint32_t *)puVar13;
        break;
      case 0x20:
        pbVar20 = param_2;
        if (iVar21 == 0) iVar21 = 0x20;
        goto LAB_8018e60c;
      case 0x23:
        bVar7 = 1;
        pbVar20 = param_2;
        goto LAB_8018e60c;
      case 0x2a:
        puVar12 = puVar13 + 1;
        local_34 = (intptr_t)*puVar13;
        puVar13 = puVar12;
        pbVar20 = param_2;
        if (-1 < (int)local_34) goto LAB_8018e60c;
        local_34 = -local_34;
        /* fallthrough */
      case 0x2d:
        uVar16 |= 0x10;
        puVar13 = puVar12;
        pbVar20 = param_2;
        goto LAB_8018e60c;
      case 0x2b:
        iVar21 = 0x2b;
        pbVar20 = param_2;
        goto LAB_8018e60c;
      case 0x2e:
        pbVar20 = pbVar20 + 2;
        bVar1 = *pbVar20;
        if (bVar1 == 0x2a) {
          puVar17i = (intptr_t)*puVar13;
          puVar13 = puVar13 + 1;
        } else {
          puVar17i = 0;
          while (bVar1 < 0x80 && (bVar2 = *pbVar20, (DAT_801b6701[bVar2] & 4) != 0)) {
            pbVar20 = pbVar20 + 1;
            bVar1 = *pbVar20;
            puVar17i = (intptr_t)((intptr_t)puVar17i * 10 + -0x30 + (uint32_t)bVar2);
          }
          pbVar20 = pbVar20 - 1;
        }
        if ((int)puVar17i < 0) puVar17i = -1;
        goto LAB_8018e60c;
      case 0x30:
        uVar16 |= 0x20;
        pbVar20 = param_2;
        goto LAB_8018e60c;
      case 0x31: case 0x32: case 0x33: case 0x34:
      case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
        local_34 = 0;
        do {
          pbVar20 = param_2;
          param_2 = pbVar20 + 1;
          local_34 = (intptr_t)((intptr_t)local_34 * 10 + -0x30 + (uint32_t)*pbVar20);
          if (0x7f < *param_2) break;
        } while ((DAT_801b6701[*param_2] & 4) != 0);
        goto LAB_8018e60c;
      case 0x44:
        bVar5 = 1;
        /* fallthrough */
      case 100:
      case 0x69:
        if (bVar5 || !bVar6) uVar9 = *puVar13;
        else uVar9 = (uint32_t)(int16_t)*puVar13;
        uVar10 = 10;
        if ((int32_t)uVar9 < 0) { uVar9 = -uVar9; iVar21 = 0x2d; }
        goto LAB_8018e95c_d;
      case 0x4c:
        pbVar20 = param_2;
        goto LAB_8018e60c;
      case 0x4f:
        bVar5 = 1;
        /* fallthrough */
      case 0x6f:
        if (bVar5 || (uVar10 = 8, !bVar6)) {
          uVar9 = *puVar13;
          uVar10 = 8;
          goto LAB_8018e958_d;
        }
        uVar9 = (uint32_t)(int16_t)*puVar13;
        iVar21 = 0;
        goto LAB_8018e95c_d;
      case 0x55:
        bVar5 = 1;
        /* fallthrough */
      case 0x75:
        if (bVar5 || (uVar10 = 10, !bVar6)) {
          uVar9 = *puVar13;
          uVar10 = 10;
          goto LAB_8018e958_d;
        }
        uVar9 = (uint32_t)(int16_t)*puVar13;
        iVar21 = 0;
        goto LAB_8018e95c_d;
      case 0x58:
        local_30 = s_0123456789ABCDEF_80125004;
        /* fallthrough */
      case 0x78:
        if (bVar5 || !bVar6) uVar9 = *puVar13;
        else uVar9 = (uint32_t)(int16_t)*puVar13;
        uVar10 = 0x10;
        iVar21 = 0;
        if (bVar7 && uVar9 != 0) { bVar4 = 1; goto LAB_8018e958_d; }
        goto LAB_8018e95c_d;
      case 99:
        pcVar15 = (char *)local_68;
        puVar19 = (uint32_t *)1;
        iVar21 = 0;
        local_68[0] = (char)*puVar13;
        goto LAB_8018e9fc_d;
      case 0x68:
        bVar6 = 1;
        pbVar20 = param_2;
        goto LAB_8018e60c;
      case 0x6c:
        bVar5 = 1;
        pbVar20 = param_2;
        goto LAB_8018e60c;
      case 0x6e:
        if (bVar5 || !bVar6) {
          param_3 = (const uint32_t *)(puVar13 + 1);
          *(uint32_t *)guest_at(*puVar13) = (uint32_t)local_40;
        } else {
          param_3 = (const uint32_t *)(puVar13 + 1);
          *(uint16_t *)guest_at(*puVar13) = (uint16_t)local_40;
        }
        goto LAB_8018ebec_d;
      case 0x70:
        uVar9 = *puVar13;
        uVar10 = 0x10;
        goto LAB_8018e958_d;
      case 0x73:
        pcVar15 = *puVar13 == 0 ? (char *)s__null__80124ffc
                                : (char *)guest_at(*puVar13);
        if ((int)puVar17i < 0) {
          puVar17i = str_len(pcVar15);
        } else {
          char *pcVar8 = strchr_bounded(pcVar15,0,(int)puVar17i);
          if (pcVar8 != 0 &&
              (iVar21 = 0, puVar19 = (uint32_t *)(intptr_t)(pcVar8 - pcVar15),
               pcVar8 - pcVar15 <= (intptr_t)puVar17i))
            goto LAB_8018e9fc_d;
        }
        iVar21 = 0;
        puVar19 = (uint32_t *)puVar17i;
        goto LAB_8018e9fc_d;
      }
      console_emit(uVar9);
      goto LAB_8018ebec_d;
    }
switchD_8018e634_caseD_0:
    console_pct();
  }
  return (uint32_t)local_40;
LAB_8018e958_d:
  iVar21 = 0;
LAB_8018e95c_d:
  local_3c = (intptr_t)puVar17i;
  if (-1 < (int)puVar17i) uVar16 &= ~0x20;
  {
    /* Guest digit scratch = sp+0x10..0x37, i.e. local_68[40]; the cursor
     * s2 starts one past the top (sp+0x38, adjacent to the local_40 char
     * counter) and walks down storing digits. The emitted string starts
     * at the final cursor and its length is top - cursor. Ghidra split
     * the scratch into &local_40/local_68 scalars and emitted pointer
     * arithmetic that wrote digits below the pcVar15w variable itself and
     * never assigned pcVar15 — rebuild it as one real buffer. */
    pcVar15w = local_68 + sizeof(local_68);
    if (uVar9 != 0 || puVar17i != 0)
      goto LAB_8018e984_d;
    pcVar15 = pcVar15w;
    puVar19 = (uint32_t *)0;
    goto LAB_8018e9fc_d;
  }
LAB_8018e984_d:
  do {
    uVar22 = uVar9 / uVar10;
    if (uVar10 == 0) { /* guest break 7 — div-by-zero */ }
    cVar3 = local_30[uVar9 % uVar10];
    pcVar15w = pcVar15w - 1;
    *pcVar15w = cVar3;
    uVar9 = uVar22;
  } while (uVar22 != 0);
  local_30 = s_0123456789abcdef_80124fe8;
  if (bVar7 && uVar10 == 8 && cVar3 != '0') {
    pcVar15w = pcVar15w - 1;
    *pcVar15w = '0';
  }
  pcVar15 = pcVar15w;
  puVar19 = (uint32_t *)(uintptr_t)((local_68 + sizeof(local_68)) - pcVar15w);
LAB_8018e9fc_d:
  param_3 = (const uint32_t *)((const uint32_t *)puVar13 + 1);
  puVar17 = (uint32_t *)((intptr_t)puVar19 + local_38);
  if (iVar21 != 0) puVar17 = (uint32_t *)((intptr_t)puVar17 + 1);
  if (bVar4) puVar17 = (uint32_t *)((intptr_t)puVar17 + 2);
  puVar18 = (uint32_t *)local_3c;
  if (local_3c < (intptr_t)puVar17) puVar18 = puVar17;
  if (uVar16 == 0 && local_34 != 0 && (intptr_t)puVar18 < local_34) {
    intptr_t puVar11 = (intptr_t)puVar18;
    do { console_emit(0x20); puVar11++; } while (puVar11 < local_34);
  }
  if (iVar21 != 0) console_emit((uint32_t)iVar21);
  if (bVar4) { console_emit(0x30); console_emit((uint32_t)(char)*param_2); }
  if (uVar16 == 0x20 && (intptr_t)puVar18 < local_34) {
    intptr_t puVar11 = (intptr_t)puVar18;
    do { console_emit(0x30); puVar11++; } while (puVar11 < local_34);
  }
  if ((intptr_t)puVar17 < local_3c) {
    do { console_emit(0x30); puVar17 = (uint32_t *)((intptr_t)puVar17 + 1); }
    while ((intptr_t)puVar17 < local_3c);
  }
  while ((puVar19 = (uint32_t *)((intptr_t)puVar19 - 1)), (intptr_t)puVar19 >= 0) {
    cVar3 = *pcVar15;
    pcVar15 = (char *)((intptr_t)pcVar15 + 1);
    console_emit((uint32_t)cVar3);
  }
  while ((local_38 = local_38 - 1), local_38 >= 0) console_emit(0x30);
  if ((uVar16 & 0x10) != 0 && (intptr_t)puVar18 < local_34) {
    intptr_t puVar17b = (intptr_t)puVar18;
    do { console_emit(0x20); puVar17b++; } while (puVar17b < local_34);
  }
  puVar17 = (uint32_t *)local_34;
  if (local_34 < (intptr_t)puVar18) puVar17 = puVar18;
  local_40 = local_40 + (intptr_t)puVar17;
LAB_8018ebec_d:
  param_2 = param_2 + 1;
  goto LAB_8018e5e0;
}

/* ======== debug console emit layer ========
 * 32-byte line buffer @0x801f68f0, col @0x801b6790, count @0x801b6794,
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
    if ((DAT_801b6701[ch & 0xff] & 0x97) != 0)
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
uint32_t sprite_9slice_build(uint32_t param_1,uint32_t param_2,uint32_t param_3,uint32_t param_4)
{
  uint32_t *t1;
  uint8_t *b;
  int32_t a2, a3, a0, v1, v0_;
  t1 = (uint32_t *)spr_pool_b_alloc();
  if (t1 == 0) return 0xffffffff;
  b = (uint8_t *)t1;
  a2 = (int32_t)(param_3 >> 16) & 0xff; a2 += 1;
  a0 = (int32_t)param_4 & 0xff;
  int32_t a1 = a0 + a2;
  v1 = (int32_t)(param_4 >> 8) & 0xff;
  a3 = (int32_t)(param_3 >> 24) + 1;
  b[0x15] = (uint8_t)v1;
  v1 += a3;
  b[0x14] = (uint8_t)a1;
  b[0x24] = (uint8_t)a1;
  b[0x1d] = (uint8_t)v1;
  b[0x25] = (uint8_t)v1;
  b[0x1c] = (uint8_t)a0;
  a1 = -a2;
  a1 = (a1 + ((uint32_t)a1 >> 31)) >> 1;   /* -(a2)/2 rounding */
  a1 += (int32_t)param_2;
  v1 = -a3;
  v1 = (v1 + ((uint32_t)v1 >> 31)) >> 1;
  a0 = (int32_t)param_2 >> 16;
  v1 += a0;
  a2 >>= 1; a2 += (int32_t)param_2;
  a3 >>= 1; a0 += a3;
  t1[3] = param_4;                          /* +0xc */
  *(int16_t *)(b + 0x16) = (int16_t)param_3;
  *(int16_t *)(b + 8) = (int16_t)a1;
  *(int16_t *)(b + 0xa) = (int16_t)v1;
  *(int16_t *)(b + 0x10) = (int16_t)a2;
  *(int16_t *)(b + 0x12) = (int16_t)v1;
  *(int16_t *)(b + 0x18) = (int16_t)a1;
  *(int16_t *)(b + 0x1a) = (int16_t)a0;
  *(int16_t *)(b + 0x20) = (int16_t)a2;
  *(int16_t *)(b + 0x22) = (int16_t)a0;
  t1[1] = 0x2e808080;
  uint32_t *head = (uint32_t *)guest_at(param_1);
  uint32_t old = *head;
  *t1 = old | 0x9000000;
  *head = guest_of(t1) & 0xffffff;
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

void mdec_decode_issue(uint32_t param_1,uint32_t param_2)

{
  uint32_t sz;
  
  mdec_stat_wait_a();
  MDECREG_ST(0x801b68f8, MDECREG(0x801b68f8) | 0x88);
  MDECREG_ST(0x801b68c0, param_1 + 4);
  sz = ((param_2 >> 5) << 0x10) | 0x20;
  MDECREG_ST(0x801b68c4, sz);
  MDECREG_ST(0x801b68f0, *(uint32_t *)guest_at(param_1));
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

void mdec_error_reset(uint32_t param_1)

{
  printf_forward((const char *)guest_at(0x80125324),
               (const char *)guest_at(param_1));
  MDECREG_ST(0x801b68f4, 0x80000000);
  MDECREG_ST(0x801b68c8, 0);
  MDECREG_ST(0x801b68d4, 0);
  MDECREG_ST(0x801b68f4, 0x60000000);
  return;
}

/* ======== 0x8018fb10 ======== */

void mdec_pump_a(int32_t param_1)

{
  if (param_1 == 0) {
    MDECREG_ST(0x801b68f4, 0x80000000);
    MDECREG_ST(0x801b68c8, 0);
    MDECREG_ST(0x801b68d4, 0);
    MDECREG_ST(0x801b68f4, 0x60000000);
    mdec_decode_issue(0x801b67b0,0x20);
    mdec_decode_issue(0x801b6834,0x20);
  }
  else if (param_1 == 1) {
    MDECREG_ST(0x801b68f4, 0x80000000);
    MDECREG_ST(0x801b68c8, 0);
    MDECREG_ST(0x801b68d4, 0);
    MDECREG_ST(0x801b68f4, 0x60000000);
  }
  else {
    printf_forward((const char *)guest_at(0x801252e8),param_1);
  }
  return;
}

/* ======== 0x8018fc90 ======== */

void mdec_pump_b(uint32_t param_1,uint32_t param_2)

{
  uint32_t sz;
  
  mdec_stat_wait_b();
  MDECREG_ST(0x801b68f8, MDECREG(0x801b68f8) | 0x88);
  MDECREG_ST(0x801b68d4, 0);
  MDECREG_ST(0x801b68cc, param_1);
  sz = ((param_2 >> 5) << 0x10) | 0x20;
  MDECREG_ST(0x801b68d0, sz);
  MDECREG_ST(0x801b68d4, 0x1000200);
  return;
}

/* ======== 0x80193fa0 ======== */

uint32_t gpu_bits_pack_a(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                      uint32_t param_4)

{
  return ((param_1 & 3) << 7) | ((param_2 & 3) << 5) |
         ((param_4 & 0x100) >> 4) | ((param_3 & 0x3ff) >> 6) |
         ((param_4 & 0x200) << 2);
}

/* ======== 0x80193fe0 ======== */

uint32_t gpu_coord_pack_b(uint32_t param_1,uint32_t param_2)

{
  return (((param_2 << 6) | ((param_1 >> 4) & 0x3f)) & 0xffff);
}

/* ======== 0x80193e30 ======== */

uint32_t gpu_sprite_rect_emit(uint32_t param_1,uint32_t param_2,uint32_t param_3)

{
  int16_t local_10[4];
  
  local_10[0] = (int16_t)param_2;
  local_10[1] = (int16_t)param_3;
  local_10[2] = 0x10;
  local_10[3] = 1;
  /* guest rect = $sp+0x10 (frame 0x28, disasm 0x80193e30) -> frm+0x10;
   * reserved frame staging, see spr_draw_off. */
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t gre_frm = br2_guest_frame(0x28);
    memcpy(guest_at(gre_frm + 0x10), local_10, 8);
    gpu_load_image_cmd(gre_frm + 0x10,param_1);
    br2_guest_frame_free();
  }
  return gpu_coord_pack_b(param_2,param_3) & 0xffff;
}

/* ======== 0x80193d40 ======== */

uint32_t gpu_sprite_rect_emit_mode(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                      uint32_t param_4,uint32_t param_5,uint32_t param_6,
                      uint32_t param_7)

{
  int16_t local_10[4];
  int16_t f14;                          /* guest leaves +0x14 uninit for
                                         * mode > 2 / negative mode */
  int32_t m = (int32_t)param_2;
  int32_t v = (int32_t)param_6;
  
  local_10[0] = (int16_t)param_4;
  local_10[1] = (int16_t)param_5;
  local_10[3] = (int16_t)param_7;
  if (m == 1) {
    f14 = (int16_t)(v / 2);
  }
  else if (m < 2) {
    if (m == 0) f14 = (int16_t)(v / 4);
  }
  else if (m == 2) {
    f14 = (int16_t)param_6;
  }
  local_10[2] = f14;
  /* guest rect = $sp+0x10 (frame 0x30, disasm 0x80193d40) -> frm+0x10;
   * reserved frame staging, see spr_draw_off. */
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t grem_frm = br2_guest_frame(0x30);
    memcpy(guest_at(grem_frm + 0x10), local_10, 8);
    gpu_load_image_cmd(grem_frm + 0x10,param_1);
    br2_guest_frame_free();
  }
  return gpu_bits_pack_a(param_2,param_3,param_4,param_5) & 0xffff;
}

/* ======== 0x80194120 ======== */

void gpu_pkt_tag_74(uint32_t param_1)

{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  p[3] = 3;
  p[7] = 0x74;                        /* jr delay slot */
  return;
}

/* ======== 0x80194140 ======== */

void gpu_pkt_tag_60(uint32_t param_1)

{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  p[3] = 3;
  p[7] = 0x60;                        /* jr delay slot */
  return;
}

/* ======== 0x80190dd0 ======== */

void guest_memset_b(uint32_t param_1,uint32_t param_2,uint32_t param_3)

{
  uint8_t *d;
  int32_t n = (int32_t)param_3;
  
  if (param_1 == 0) return;
  d = (uint8_t *)guest_at(param_1);
  if (n > 0) {
    do {
      *d = (uint8_t)param_2;
      n = n - 1;
      d = d + 1;
    } while (n > 0);
  }
  return;
}

/* ======== 0x80190a04 ======== */

/* Text-window printf: vsprintf into the 0x30-byte window slot at
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

int32_t text_win_printf(int32_t param_1,uint32_t param_2,uint32_t param_3,
                     uint32_t param_4,...)

{
  uint32_t iargs[3];
  va_list ap;
  int ai;
  int32_t win;
  const char *fmt;
  uint32_t slot;
  int32_t count = (int32_t)param_2;    /* live $a1: stale until a spec
                                        * handler sets it */
  int32_t c;
  uint8_t dbuf[0x210];
  uint8_t *dtop = dbuf + 0x210;
  const uint8_t *ep = dtop;            /* guest $s0: stale across specs */
  
  iargs[0] = param_2;
  iargs[1] = param_3;
  iargs[2] = param_4;
  va_start(ap,param_4);
  if ((param_1 < 0) || ((int32_t)GB32(0x801b78c0) <= param_1)) {
    /* out-of-range a0: it is the format string; window = DAT_801b78c4 */
    win = GB32(0x801b78c4);
    if (GB32(0x801b7740 + win * 0x30 + 0x24) == 0) {
      va_end(ap);
      return -1;
    }
    fmt = (const char *)guest_at((uint32_t)param_1);
    ai = 0;                            /* varargs start at param_2 */
  }
  else {
    win = (uint32_t)param_1;
    fmt = (const char *)guest_at(param_2);
    ai = 1;                            /* varargs start at param_3 */
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

void gp_20_store(uint32_t param_1)

{
  /* $gp = 0x801beedc (set at 0x801261f8: lui/addiu $gp); +0x20 cell */
  *(uint32_t *)guest_at(0x801beefc) = param_1;
  return;
}

/* ======== 0x801a6ae8 ======== */

void gp_20_store_default(void)

{
  gp_20_store(0x801b17b4);
  return;
}

/* ======== 0x801ad59c ======== */

int32_t aux_tbl_store_a(uint32_t param_1,int32_t param_2)

{
  if (param_2 < 5) {
    *(uint32_t *)guest_at(0x801ff638 + param_2 * 4) = param_1;
    return 1;
  }
  return 0;
}

/* ======== 0x801ad1e4 ======== */

int32_t aux_tbl_store_b(uint32_t param_1,int32_t param_2)

{
  if (param_2 < 5) {
    *(uint32_t *)guest_at(0x801ff620 + param_2 * 4) = param_1;
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
uint32_t gpu_status_cmd(uint32_t param_1)
{
  uint32_t v;
  br2_st32p(0x801b8428, 0x10000007);
  v = br2_ld32p(0x801b8424) & 0xffffff;
  if (v == 2) {
    if ((param_1 & 8) != 0) {
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
void gpu_pkt_attr_set(uint32_t param_1,int param_2)
{
  uint32_t *w = (uint32_t *)guest_at(param_1);
  if (param_2 & 1) *w &= 0xf7ffffff;
  else             *w |= 0x08000000;
  if (param_2 & 2) *w |= 0x02000000;
  else             *w &= 0xfdffffff;
  mdec_decode_issue(param_1,*(uint16_t *)w);
}

/* vram_tile_decode — VRAM tile RLE decoder + XOR descramble. Control bytes
 * from guest stream 0x801b6930: <0xf0 = run of n+1 bytes (n<0x80 only;
 * dist==0 copies literals, else backrefs from dst-dist); ==0xf0 resets
 * dist; >0xf0 packs dist = ((n<<8)|next) - 0xf0ff, dist==0xf00 ends.
 * Then u16 buf[i] ^= buf[i-4] for i in [4,0x87ff]. dst = host buffer.
 * Disasm 0x80190260-0x8019033c. */
void vram_tile_decode(uint8_t *param_1)
{
  const uint8_t *s = (const uint8_t *)guest_at(0x801b6930);
  /* param_1: host pointer from the clean_table wrapper, guest-address
   * value from game.c scene_entry (_DAT_801f4758 = 0x80010000+0x6f000) —
   * resolve either form. */
  uint8_t *base;
  {
    uint32_t g = guest_of(param_1);
    base = (uint8_t *)guest_at(g ? g : (uint32_t)(uintptr_t)param_1);
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

/* spr_prim_tag_a — sprite-prim tag init: byte+3=5, byte+7=0x28.
 * a0 is a guest address. Disasm 0x801940c0-0x801940d0. */
void spr_prim_tag_a(uint32_t param_1)
{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  if (p == NULL) return;
  p[3] = 5;
  p[7] = 0x28;
}

/* spr_prim_tag_b — sprite-prim tag init: byte+3=3, byte+7=0x40.
 * a0 is a guest address. Disasm 0x80194160-0x80194170. */
void spr_prim_tag_b(uint32_t param_1)
{
  uint8_t *p = (uint8_t *)guest_at(param_1);
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

/* gpu_texpage_pack — GPU texpage/coord pack from 8-byte descriptor:
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
extern int lut_sin_q12(uint);
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
    sn = lut_sin_q12((uint)ang);
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
/* Single-byte flag read @0x801c8322 (adjacent to the 0x801c8324/26
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
 * each word's own top byte, then tag *a0 with the compressed a1 pointer:
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
/* Packet tag byte pair: p[3]=0x0c, p[7]=0x3c. Sits between the
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
 * over the s16 coord fields of the 20-byte record at a0, with the
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
 * record a0's low byte in the 256-entry tag table @0x801fad68 indexed
 * by a0>>24. Guest returns a0>>24 in $v0 (the srl result survives
 * into the jr delay slot which performs the sb). Disasm
 * 0x80192e00-0x80192e20. */
uint32_t gpu_tag_cmd_store(uint32_t param_1)
{
  uint32_t idx = param_1 >> 24;
  br2_st32p(0x801b8428, param_1);
  GP8(0x801fad68 + idx) = (uint8_t)param_1;
  return idx;
}

/* ======== 0x80192e78 ======== */

/* gpu_reg_cells_init — quad register-cell init: writes 0x04000002,
 * a0, 0 and 0x01000401 through pointer cells 0x801b8428/2c/30/34.
 * Guest leaves $v0 = the last cell pointer (callers ignore it).
 * Disasm 0x80192e78-0x80192ebc. */
void gpu_reg_cells_init(uint32_t param_1)
{
  br2_st32p(0x801b8428, 0x04000002u);
  br2_st32p(0x801b842c, param_1);
  br2_st32p(0x801b8430, 0u);
  br2_st32p(0x801b8434, 0x01000401u);
}

/* ======== 0x80192ec0 ======== */

/* gpu_reg_cmd_commit — issue param_1 | 0x10000000 through the pointer
 * cell @0x801b8428, then return *(*0x801b8424) & 0x00FFFFFF. The cell
 * contents are device/register addresses (can be 0x1F80xxxx MMIO), so
 * both dereferences go through br2_ld32/br2_st32. Disasm
 * 0x80192EC0-0x80192EF0. */
uint32_t gpu_reg_cmd_commit(uint32_t param_1)
{
  uint32_t dst = br2_ld32(0x801b8428u);
  br2_st32(dst, param_1 | 0x10000000u);
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

/* ======== 0x80187dcc ======== */

/* gte_persp_batch — GTE batch transform over 16-byte records.
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
 * 4-byte table at a2), a2 is the record descriptor:
 *   +0x10 vertex table (8-byte xy/z pairs), +0x18 index stream,
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

/* Load three 8-byte vertex records (V0,V1,V2) by signed index. */
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
 * stream of 3xh16 stride 6, +0x1c 8-byte color table, +0x20 secondary
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

/* rec_hibyte_pair_swap — cross record high-byte store: t = *(u32*)a0;
 * *(u16*)(a0+2) = (u16)(a1 << 8); *(u16*)(a1+2) = (u16)(t << 8);
 * returns a0. Each sh keeps only the source low byte in the halfword's
 * high byte (low byte zeroed). Disasm 0x8018bd24-0x8018bd3c
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

/* prim_emit_60 — input stride 0x60. RTPT/NCLIP gate with the attribute byte
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

/* prim_emit_78 — input stride 0x78. RTPT/NCLIP gate, attribute-byte GTE6,
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

/* prim_emit_80 — input stride 0x80. RTPT/NCLIP gate, attribute-byte GTE6,
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
 * gate, attribute-byte GTE6, RTPS + NCCT + AVSZ4 + NCCS color groups and
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
 * cur+0xc; MAC0>0 folds the out+7 attribute byte into GTE6, runs AVSZ3 +
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

extern int *rtps_point_xform2(undefined4 *, undefined4 *, int *);
extern undefined2 *gte_rtps_xform1(undefined4 *, undefined4 *, undefined2 *);
extern uint *gte_rtps_xform3(undefined4 *, ushort *, uint *);
extern void part_matrix_compute(undefined4 *, byte *, short *, undefined4);
extern int atan2_lut(int, int);
extern int lut_cos_q12(int);
extern int lut_sin_q12(uint);

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
      (undefined4 *)guest_at(g[4]),
      (undefined4 *)guest_at(g[5]),
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
      (undefined4 *)guest_at(g[4]),
      (undefined4 *)guest_at(g[5]),
      (undefined2 *)guest_at(g[6])));

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
  part_matrix_compute((undefined4 *)guest_at(g[4]),
                      (byte *)guest_at(g[5]),
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
      (undefined4 *)guest_at(g[4]),
      (undefined4 *)guest_at(g[5]),
      (undefined2 *)guest_at(g[6])));

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
  part_matrix_compute((undefined4 *)guest_at(g[4]),
                      (byte *)guest_at(g[5]),
                      (short *)guest_at(g[6]), g[7]);

L_80186908:
  g[5] = g[16];                           /* a1 + 4 */
  g[8] = br2_ld32(fr + 68);               /* reload saved ptr */
  g[6] = g[22] + 32;
  /* jal 0x80186914 -> gte_rtps_xform3(g8+32, a1+4, a1+32) */
  g[4] = g[8] + 32;                       /* delay slot */
  g[2] = (uint32_t)guest_of(gte_rtps_xform3(
      (undefined4 *)guest_at(g[4]),
      (ushort *)guest_at(g[5]),
      (uint *)guest_at(g[6])));

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
