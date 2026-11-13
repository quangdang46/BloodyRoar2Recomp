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
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t br2_ld32p(uint32_t cell);
extern void     br2_st32p(uint32_t cell, uint32_t v);

void gpu_load_image_cmd(undefined4,undefined4);
void prim_packet_init_tex(int,int,int,uint,byte *);
undefined1 * guest_memcpy(uint32_t *,uint32_t *,int);
void printf_forward(const char *, ...);
void  guest_memset(uint32_t *,undefined1,int);
void  cd_dispatch_tblc(void);
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
static uint32_t gpu_rect(const void *p)
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
  
  /* param_1 arrives as a guest address (low 24 bits are the
   * compressed link value); walk the resolved host cell. */
  hp = (uint *)guest_at((uint32_t)(uintptr_t)param_1);
  uVar2 = (uint)param_1 & 0xffffff;
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
  short local_10;
  short local_e;
  undefined2 local_c;
  undefined2 local_a;
  uint8_t *hp;
  
  /* param_1 is a guest address (callers pass the heap cursor
   * _DAT_801e3cfc); resolve before dereferencing. param_1 + 0x14
   * stays a guest address for gpu_load_image_cmd's a1. */
  hp = (uint8_t *)guest_at((uint32_t)param_1);
  if ((*(uint *)(hp + 4) & 8) != 0) {
    param_1 = param_1 + *(int *)(hp + 8);
    hp = (uint8_t *)guest_at((uint32_t)param_1);
  }
  local_10 = param_2;
  local_e = param_3;
  if (param_4 == 0) {
    local_10 = *(short *)(hp + 0xc) + param_2;
    local_e = *(short *)(hp + 0xe) + param_3;
  }
  local_c = *(undefined2 *)(hp + 0x10);
  local_a = *(undefined2 *)(hp + 0x12);
  gpu_load_image_cmd(gpu_rect(&local_10),(undefined4)(param_1 + 0x14));
  return;
}

/* ======== 0x8018bec0 ======== */

void spr_draw_cond(int param_1,short param_2,short param_3,int param_4)

{
  short local_10;
  short local_e;
  undefined2 local_c;
  undefined2 local_a;
  uint8_t *hp;
  
  /* param_1 is a guest address; see spr_draw_off. */
  hp = (uint8_t *)guest_at((uint32_t)param_1);
  if ((*(uint *)(hp + 4) & 8) != 0) {
    local_10 = param_2;
    local_e = param_3;
    if (param_4 == 0) {
      local_10 = *(short *)(hp + 0xc) + param_2;
      local_e = *(short *)(hp + 0xe) + param_3;
    }
    local_c = *(undefined2 *)(hp + 0x10);
    local_a = *(undefined2 *)(hp + 0x12);
    gpu_load_image_cmd(gpu_rect(&local_10),(undefined4)(param_1 + 0x14));
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

undefined4 gpu_mode_regs_apply(uint param_1)

{
  /* IRQ-path function: clean-C body cannot supply the live guest
   * register file / MMIO ack semantics the deferred interrupt path needs.
   * Delegate to generated MIPS (behavioral oracle). */
  return (undefined4)br2_call_generated(0x80193424u,param_1, 0, 0, 0);
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
  uint16_t local_30, local_2e, local_2c, local_2a;
  uint32_t slot;

  if ((int32_t)GB8(0x801b78c0) < 8) {
    if (GB8(0x801b78c0) == 0) GB32(0x801b82c8) = 0;
    slot = GB8(0x801b78c0) * 0x30;
    GB32(0x801b7740 + slot + 0x2c) = (param_3 == 0);
    iVar2 = (int32_t)GB32(0x801b82c8);
    if (0x400 < iVar2 + param_6) param_6 = 0x400 - iVar2;
    local_2c = 0x100; local_2a = 0x100;
    local_30 = 0;     local_2e = 0;
    prim_packet_init_tex((int)(0x801b7750 + slot),0,0,(uint32_t)GB32(0x801fad10),
                 (byte *)&local_30);
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
    *head = *head | 0x5000000;
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
    *head = *head | 0x9000000;
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
  char local_68[38];
  intptr_t puVar17i;
  char *pcVar15w;
  char **ppuVar14;
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
    pcVar15w = (char *)&local_40;
    if (uVar9 != 0 || (puVar19 = (uint32_t *)(uintptr_t)(local_68 - (intptr_t)pcVar15w), puVar17i != 0))
      goto LAB_8018e984_d;
    goto LAB_8018e9f8_d;
  }
LAB_8018e984_d:
  do {
    ppuVar14 = (char **)&pcVar15w;
    uVar22 = uVar9 / uVar10;
    if (uVar10 == 0) { /* guest trap(0x1c00) — div-by-zero */ }
    cVar3 = local_30[uVar9 % uVar10];
    pcVar15w = (char *)((intptr_t)ppuVar14 - 1);
    *pcVar15w = cVar3;
    uVar9 = uVar22;
  } while (uVar22 != 0);
  local_30 = s_0123456789abcdef_80124fe8;
  if (bVar7) {
    puVar19 = (uint32_t *)(uintptr_t)(local_68 - (intptr_t)pcVar15w);
    if (uVar10 != 8 || (puVar19 = (uint32_t *)(uintptr_t)(local_68 - (intptr_t)pcVar15w), cVar3 == '0'))
      goto LAB_8018e9f8_d;
    pcVar15w = (char *)((intptr_t)ppuVar14 - 2);
    *pcVar15w = '0';
  }
  puVar19 = (uint32_t *)(uintptr_t)(local_68 - (intptr_t)pcVar15w);
LAB_8018e9f8_d:
  puVar19 = (uint32_t *)((intptr_t)puVar19 + 0x38);
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
  gpu_load_image_cmd(gpu_rect(local_10),param_1);
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
  gpu_load_image_cmd(gpu_rect(local_10),param_1);
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
