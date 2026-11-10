/* save.c — memory-card / settings / display-helper cluster.
 * Verbatim Ghidra ports from decomp/batch_shell.txt.
 * Guest globals bound via g_DAT_* (bind.c arena, payload-backed);
 * s_* strings resolve through guest_at into the loaded EXE image. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef int8_t   byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef void code(void);
#define CONCAT11(hi,lo) (((uint16_t)(uint8_t)(hi) << 8) | (uint8_t)(lo))
#define CONCAT12(hi,lo) (((uint32_t)(uint8_t)(hi) << 16) | (uint16_t)(lo))
#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
void FUN_80156048(short param_1,short param_2);
void FUN_801561c8(undefined2 param_1);


/* ---- guest globals ---- */
extern uint8_t  *g_DAT_1f80000c;
extern uint8_t  *g_DAT_1f800014;
extern uint8_t  *g_DAT_1f800017;
extern uint8_t  *g_DAT_1f800018;
extern uint8_t  *g_DAT_1f800019;
extern uint8_t  *g_DAT_1f80009b;
extern uint8_t  *g_DAT_8011ec40;
extern uint8_t  *g_DAT_8012181c;
extern uint8_t  *g_DAT_80121820;
extern uint8_t  *g_DAT_801b3f78;
extern uint8_t  *g_DAT_801b45f4;
extern uint8_t  *g_DAT_801b45f8;
extern uint8_t  *g_DAT_801b45fc;
extern uint8_t  *g_DAT_801b4600;
extern uint8_t  *g_DAT_801b4604;
extern uint8_t  *g_DAT_801b4608;
extern uint8_t  *g_DAT_801b460c;
extern uint8_t  *g_DAT_801b4610;
extern uint8_t  *g_DAT_801b4614;
extern uint8_t  *g_DAT_801b55d0;
extern uint8_t  *g_DAT_801bdde0;
extern uint8_t  *g_DAT_801bef60;
extern uint8_t  *g_DAT_801bef64;
extern uint8_t  *g_DAT_801bef67;
extern uint8_t  *g_DAT_801bef68;
extern uint8_t  *g_DAT_801bef69;
extern uint8_t  *g_DAT_801bef6c;
extern uint8_t  *g_DAT_801bef6d;
extern uint8_t  *g_DAT_801bef6e;
extern uint8_t  *g_DAT_801bef6f;
extern uint8_t  *g_DAT_801bef72;
extern uint8_t  *g_DAT_801bef74;
extern uint8_t  *g_DAT_801bef76;
extern uint8_t  *g_DAT_801bef78;
extern uint8_t  *g_DAT_801bef80;
extern uint8_t  *g_DAT_801bef84;
extern uint8_t  *g_DAT_801bef88;
extern uint8_t  *g_DAT_801bef8c;
extern uint8_t  *g_DAT_801c11e0;
extern uint8_t  *g_DAT_801c11e4;
extern uint8_t  *g_DAT_801c11e8;
extern uint8_t  *g_DAT_801c11ec;
extern uint8_t  *g_DAT_801c11f4;
extern uint8_t  *g_DAT_801c1264;
extern uint8_t  *g_DAT_801c1304;
extern uint8_t  *g_DAT_801c13a4;
extern uint8_t  *g_DAT_801c15f0;
extern uint8_t  *g_DAT_801c4580;
extern uint8_t  *g_DAT_801e83aa;
extern uint8_t  *g_DAT_801e83b0;
extern uint8_t  *g_DAT_801e8400;
extern uint8_t  *g_DAT_801f09b;
extern uint8_t  *g_DAT_801f24d0;
extern uint8_t  *g_DAT_801f24d1;
extern uint8_t  *g_DAT_801f35f4;
extern uint8_t  *g_DAT_801f3600;
extern uint8_t  *g_DAT_801f3602;
extern uint8_t  *g_DAT_801f5968;
extern uint8_t  *g_DAT_801f5969;
extern uint8_t  *g_DAT_801f596c;
extern uint8_t  *g_DAT_801f596d;
extern uint8_t  *g_DAT_801f596e;
extern uint8_t  *g_DAT_801f596f;
extern uint8_t  *g_DAT_801f5970;
extern uint8_t  *g_DAT_801f5971;
extern uint8_t  *g_DAT_801f5972;
extern uint8_t  *g_DAT_801f5973;
extern uint8_t  *g_PTR_DAT_801b4618;
extern uint8_t  *g__DAT_1f800008;
extern uint8_t  *g__DAT_1f80000c;
extern uint8_t  *g__DAT_1f80007c;
extern uint8_t  *g__DAT_1f800082;
extern uint8_t  *g__DAT_1f800084;
extern uint8_t  *g__DAT_1f80008c;
extern uint8_t  *g__DAT_1f8000b8;
extern uint8_t  *g__DAT_801c15f0;
extern uint8_t  *g__DAT_801e8508;
extern uint8_t  *g__DAT_801e8518;
extern uint8_t  *g__DAT_801f2650;
extern uint8_t  *g__DAT_801f2654;
extern uint8_t  *g__DAT_801f2658;
extern uint8_t  *g__DAT_801f265a;
extern uint8_t  *g__DAT_801f2666;
extern uint8_t  *g__DAT_801f2668;
extern uint8_t  *g__DAT_801f266c;
extern uint8_t  *g__DAT_801f268a;
extern uint8_t  *g__DAT_801f268c;
extern uint8_t  *g__DAT_801f2690;
extern uint8_t  *g__DAT_801f2694;
extern uint8_t  *g__DAT_801f2696;
extern uint8_t  *g__DAT_801f26a0;
extern uint8_t  *g__DAT_801f26a2;
extern uint8_t  *g__DAT_801f26a8;
extern uint8_t  *g__DAT_801f36c0;
extern char *s_1_BLOCK_IS_REQUIRED_TO_SAVE__80121824;
extern char *s_BLOCKS_AND_1_BLOCK_IS_REQUIRED_801219b8;
extern char *s_CHECKING_MEMORY_CARD__s__80121938;
extern char *s_COMPLETED___80121954;
extern char *s_DATA_ERROR____80121a24;
extern char *s_DO_NOT_REMOVE_MEMORY_CARD___801218d0;
extern char *s_ERROR___80121a34;
extern char *s_FORMAT_MEMORY_CARD__s__80121920;
extern char *s_FORMAT_MEMORY_CARD__s___801217d8;
extern char *s_LOAD_FROM_MEMORY_CARD__s__801218ec;
extern char *s_LOAD_FROM_MEMORY_CARD__s___80121788;
extern char *s_MEMORY_CARD_CHANGED__801217a4;
extern char *s_MEMORY_CARD__s___80121980;
extern char *s_NOW_CHECKING_MEMORY_CARD__s____801218b0;
extern char *s_NOW_FORMATTING_MEMORY_CARD__s____8012188c;
extern char *s_NOW_LOADING_FROM_MEMORY_CARD_80121844;
extern char *s_NOW_SAVING_TO_MEMORY_CARD__s____8012186c;
extern char *s_NO_BLOODY_ROAR_2_DATA_DETECTED___801219e4;
extern char *s_NO_MEMORY_CARD_IS_INSERTED_IN_80121960;
extern char *s_OVER_WRITE_OK___8012180c;
extern char *s_SAVE_TO_MEMORY_CARD__s__80121908;
extern char *s_SAVE_TO_MEMORY_CARD__s___801217bc;
extern char *s_SAVING_TO_MEMORY_CARD__s__801217f0;
extern char *s_THERE_IS_INSUFFICIENT_MEMORY_CAR_80121994;
extern char *s_TO_SAVE__801219d8;
extern char *s_UNFORMATTED_MEMORY_CARD___80121a08;
extern char *s_VSync__timeout_80125b38;
extern char *s__s____80121864;
extern uint32_t *g_PTR_u32_801b4160;
extern uint8_t  *g_DAT_801b43f0;

/* ---- macros ---- */
#define DAT_1f80000c   (*(uint8_t *)g_DAT_1f80000c)
#define DAT_1f800014   (*(uint8_t *)g_DAT_1f800014)
#define DAT_1f800017   (*(uint8_t *)g_DAT_1f800017)
#define DAT_1f800018   (*(uint8_t *)g_DAT_1f800018)
#define DAT_1f800019   (*(uint8_t *)g_DAT_1f800019)
#define DAT_1f80009b   (*(uint8_t *)g_DAT_1f80009b)
#define DAT_8011ec40   (*(uint8_t *)g_DAT_8011ec40)
#define DAT_8012181c   (*(uint8_t *)g_DAT_8012181c)
#define DAT_80121820   (*(uint8_t *)g_DAT_80121820)
#define DAT_801b3f78   (*(uint16_t *)g_DAT_801b3f78)
#define DAT_801b45f4   (*(uint32_t *)g_DAT_801b45f4)
#define DAT_801b45f8   (*(uint32_t *)g_DAT_801b45f8)
#define DAT_801b45fc   (*(uint32_t *)g_DAT_801b45fc)
#define DAT_801b4600   (*(uint32_t *)g_DAT_801b4600)
#define DAT_801b4604   (*(uint32_t *)g_DAT_801b4604)
#define DAT_801b4608   (*(uint32_t *)g_DAT_801b4608)
#define DAT_801b460c   (*(uint32_t *)g_DAT_801b460c)
#define DAT_801b4610   (*(uint32_t *)g_DAT_801b4610)
#define DAT_801b4614   (*(uint32_t *)g_DAT_801b4614)
#define DAT_801b55d0   (*(uint8_t *)g_DAT_801b55d0)
#define DAT_801bdde0   (*(uint32_t *)g_DAT_801bdde0)
#define DAT_801bef60   (*(uint8_t *)g_DAT_801bef60)
#define DAT_801bef64   (*(uint32_t *)g_DAT_801bef64)
#define DAT_801bef67   (*(uint8_t *)g_DAT_801bef67)
#define DAT_801bef68   (*(uint8_t *)g_DAT_801bef68)
#define DAT_801bef69   (*(uint8_t *)g_DAT_801bef69)
#define DAT_801bef6c   (*(uint8_t *)g_DAT_801bef6c)
#define DAT_801bef6d   (*(uint8_t *)g_DAT_801bef6d)
#define DAT_801bef6e   (*(uint8_t *)g_DAT_801bef6e)
#define DAT_801bef6f   (*(uint8_t *)g_DAT_801bef6f)
#define DAT_801bef72   (*(uint16_t *)g_DAT_801bef72)
#define DAT_801bef74   (*(uint32_t *)g_DAT_801bef74)
#define DAT_801bef76   (*(uint16_t *)g_DAT_801bef76)
#define DAT_801bef78   (*(uint32_t *)g_DAT_801bef78)
#define DAT_801bef80   (*(uint32_t *)g_DAT_801bef80)
#define DAT_801bef84   (*(uint32_t *)g_DAT_801bef84)
#define DAT_801bef88   (*(uint32_t *)g_DAT_801bef88)
#define DAT_801bef8c   (*(uint32_t *)g_DAT_801bef8c)
#define DAT_801c11e0   (*(uint8_t *)g_DAT_801c11e0)
#define DAT_801c11e4   (*(uint8_t *)g_DAT_801c11e4)
#define DAT_801c11e8   (*(uint8_t *)g_DAT_801c11e8)
#define DAT_801c11ec   (*(uint8_t *)g_DAT_801c11ec)
#define DAT_801c11f4   (*(uint8_t *)g_DAT_801c11f4)
#define DAT_801c1264   (*(uint8_t *)g_DAT_801c1264)
#define DAT_801c1304   (*(uint8_t *)g_DAT_801c1304)
#define DAT_801c13a4   (*(uint8_t *)g_DAT_801c13a4)
#define DAT_801c15f0   (*(uint32_t *)g_DAT_801c15f0)
#define DAT_801c4580   (*(uint8_t *)g_DAT_801c4580)
#define DAT_801e83aa   (*(uint8_t *)g_DAT_801e83aa)
#define DAT_801e83b0   (*(uint8_t *)g_DAT_801e83b0)
#define DAT_801e8400   (*(uint8_t *)g_DAT_801e8400)
#define DAT_801f09b   (*(uint8_t *)g_DAT_801f09b)
#define DAT_801f24d0   (*(uint8_t *)g_DAT_801f24d0)
#define DAT_801f24d1   (*(uint8_t *)g_DAT_801f24d1)
#define DAT_801f35f4   (*(uint32_t *)g_DAT_801f35f4)
#define DAT_801f3600   (*(uint16_t *)g_DAT_801f3600)
#define DAT_801f3602   (*(uint16_t *)g_DAT_801f3602)
#define DAT_801f5968   (*(uint8_t *)g_DAT_801f5968)
#define DAT_801f5969   (*(uint8_t *)g_DAT_801f5969)
#define DAT_801f596c   (*(uint8_t *)g_DAT_801f596c)
#define DAT_801f596d   (*(uint8_t *)g_DAT_801f596d)
#define DAT_801f596e   (*(uint8_t *)g_DAT_801f596e)
#define DAT_801f596f   (*(uint8_t *)g_DAT_801f596f)
#define DAT_801f5970   (*(uint8_t *)g_DAT_801f5970)
#define DAT_801f5971   (*(uint8_t *)g_DAT_801f5971)
#define DAT_801f5972   (*(uint8_t *)g_DAT_801f5972)
#define DAT_801f5973   (*(uint8_t *)g_DAT_801f5973)
#define PTR_DAT_801b4618   (*(uint32_t *)g_PTR_DAT_801b4618)
#define _DAT_1f800008   (*(uint32_t *)g__DAT_1f800008)
#define _DAT_1f80000c   (*(uint32_t *)g__DAT_1f80000c)
#define _DAT_1f80007c   (*(uint32_t *)g__DAT_1f80007c)
#define _DAT_1f800082   (*(uint32_t *)g__DAT_1f800082)
#define _DAT_1f800084   (*(uint32_t *)g__DAT_1f800084)
#define _DAT_1f80008c   (*(uint32_t *)g__DAT_1f80008c)
#define _DAT_1f8000b8   (*(uint32_t *)g__DAT_1f8000b8)
#define _DAT_801c15f0   (*(uint32_t *)g__DAT_801c15f0)
#define _DAT_801e8508   (*(uint32_t *)g__DAT_801e8508)
#define _DAT_801e8518   (*(uint32_t *)g__DAT_801e8518)
#define _DAT_801f2650   (*(uint32_t *)g__DAT_801f2650)
#define _DAT_801f2654   (*(uint32_t *)g__DAT_801f2654)
#define _DAT_801f2658   (*(uint16_t *)g__DAT_801f2658)
#define _DAT_801f265a   (*(uint16_t *)g__DAT_801f265a)
#define _DAT_801f2666   (*(uint16_t *)g__DAT_801f2666)
#define _DAT_801f2668   (*(uint16_t *)g__DAT_801f2668)
#define _DAT_801f266c   (*(uint32_t *)g__DAT_801f266c)
#define _DAT_801f268a   (*(uint16_t *)g__DAT_801f268a)
#define _DAT_801f268c   (*(uint16_t *)g__DAT_801f268c)
#define _DAT_801f2690   (*(uint32_t *)g__DAT_801f2690)
#define _DAT_801f2694   (*(uint16_t *)g__DAT_801f2694)
#define _DAT_801f2696   (*(uint16_t *)g__DAT_801f2696)
#define _DAT_801f26a0   (*(uint16_t *)g__DAT_801f26a0)
#define _DAT_801f26a2   (*(uint16_t *)g__DAT_801f26a2)
#define _DAT_801f26a8   (*(uint32_t *)g__DAT_801f26a8)
#define _DAT_801f36c0   (*(uint32_t *)g__DAT_801f36c0)
#define PTR_s___BloodyRoar2__byRAIZINGver1_60_801b43f0 ((uint8_t *)guest_at(*(uint32_t *)g_DAT_801b43f0))
#define SLOTSTR(i) ((char *)guest_at(g_PTR_u32_801b4160[i]))

extern uint8_t  *g_DAT_801f2650;
extern uint8_t  *g_DAT_801f35f4;
extern uint8_t *_g_system_config_ptr;
#define DAT_801f2650   (*(uint8_t  *)g_DAT_801f2650)
#define _DAT_801f35f4  (*(uint32_t *)g_DAT_801f35f4)

/* ---- callee externs ---- */
extern void FUN_80156114();
extern void FUN_801573ec();
extern int FUN_80195ec8();
extern int FUN_80195d24();
extern int FUN_80195d44();
extern void cd_file_open(int32_t,uint8_t *);
extern int FUN_801962f0();
extern int cd_dir_read_wrap();
extern void FUN_8019da44();
extern void cd_reg_window_update();
extern void FUN_8019d220();
extern void FUN_8019d5f0();
extern int FUN_80156cf4();
extern int FUN_80156c60();
extern int FUN_80156d88();
extern int FUN_801568fc();
extern void FUN_801638c8();
extern int FUN_80167ae8(void);
extern int FUN_80167d7c();
extern int FUN_80167b34(uint32_t);
extern int FUN_80167ce4();
extern int FUN_80167bd8(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern int FUN_8016809c();
extern int FUN_8018ef60(char *,char *,...);
extern void FUN_8012e58c(uint32_t,uint32_t,const void *,uint32_t,uint32_t,int32_t);
extern void FUN_8012cdd8();
extern void FUN_8012cbc0();
extern int intro_movie_state();
extern void FUN_80156ef4();
extern int FUN_80160598(uint32_t,uint32_t);
extern void FUN_801294cc();
extern void FUN_8012c374();
extern void empty_ret();
extern int FUN_8012aaa8();
extern void FUN_8012cd9c();
extern void battle_session_arm(intptr_t,int);
extern void frame_tick(intptr_t);
extern void FUN_80197ad0();
extern void FUN_80199fe0();
extern void FUN_8019a670();

/* ======== 0x80157a00 ======== */

void FUN_80157a00(void)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  undefined1 local_18;
  byte local_17;
  undefined1 local_16;
  
  if (DAT_801bef6f != '\0') {
    return;
  }
  uVar1 = DAT_801bef76 - 1;
  if (DAT_801bef76 != 0) {
    DAT_801bef76 = uVar1;
    if ((int)((uint)uVar1 << 0x10) < 0) {
      DAT_801bef76 = 0;
    }
    FUN_80156114((int)(short)DAT_801bef76);
    if (DAT_801bef76 == 0) {
      FUN_801573ec();
      return;
    }
  }
  if ((DAT_801bef6e & 0xf) != 1) {
    if ((DAT_801bef6e & 0xf) != 2) {
      return;
    }
    switch(DAT_801bef6d) {
    default:
      return;
    case '\x10':
      FUN_80156114(0);
      DAT_801bef60 = 0xca;
      FUN_80195ec8(0xe,(int)guest_of(&DAT_801bef60));
      if (DAT_801c4580 != '\0') {
        DAT_801bef6d = DAT_801bef6d + '\x03';
      }
      DAT_801c4580 = '\x01';
    case '\x11':
    case '\x12':
    case '\x13':
      DAT_801bef6d = DAT_801bef6d + '\x01';
      return;
    case '\x14':
      iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
      if (iVar3 == 2) {
        cd_file_open(DAT_801bef80,(uint8_t *)guest_at(0x1f800f30));
        FUN_80195ec8(2,0x1f800f30);
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      if (iVar3 != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    case '\x15':
      iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
      if (iVar3 == 2) {
        FUN_80156114(0xffffffff);
        FUN_80195ec8(0x1b,0);
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      if (iVar3 != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    case '\x16':
      iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
      if (iVar3 == 2) {
        FUN_80195ec8(1,0);
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      if (iVar3 != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    case '\x17':
      iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
      if (iVar3 != 2) {
        if (iVar3 != 5) {
          return;
        }
        DAT_801bef6d = 0x20;
        return;
      }
      if ((DAT_801bef64 & 0x20) != 0) {
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      DAT_801bef6d = DAT_801bef6d + -1;
      return;
    case '\x18':
      FUN_80195ec8(0x10,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
      return;
    case '\x19':
      iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
      if (iVar3 != 2) {
        if (iVar3 != 5) {
          return;
        }
        DAT_801bef6d = 0x20;
        return;
      }
      DAT_801bef80 = FUN_801962f0(&DAT_801bef64);
      if ((DAT_801bef80 <= DAT_801bef88) && (DAT_801bef84 + -300 <= DAT_801bef80)) {
        DAT_801bef6d = DAT_801bef6d + -1;
        return;
      }
      goto joined_r0x80157c80;
    case ' ':
      FUN_80156114(0);
      if ((DAT_801bef64 & 0x10) == 0) {
        DAT_801bef6d = 0x23;
        DAT_801bef74 = DAT_801bef74 + 1;
        return;
      }
      DAT_801bef6d = DAT_801bef6d + '\x01';
      DAT_801bef74 = DAT_801bef74 + 1;
      return;
    case '!':
      FUN_80195ec8(1,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
      return;
    case '\"':
      iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
      if (iVar3 != 2) {
        if (iVar3 != 5) {
          return;
        }
        DAT_801bef6d = DAT_801bef6d + -1;
        return;
      }
      if ((DAT_801bef64 & 0x10) == 0) {
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      DAT_801bef6d = DAT_801bef6d + -1;
      return;
    case '#':
      FUN_80195ec8(0x13,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
      return;
    case '$':
      iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
      if (iVar3 != 2) {
        if (iVar3 != 5) {
          return;
        }
        DAT_801bef6d = 0x20;
        return;
      }
      break;
    case '0':
      break;
    }
    goto switchD_80157ebc_caseD_30;
  }
  switch(DAT_801bef6d) {
  case '\x10':
    FUN_80156114(0);
    DAT_801bef60 = 7;
    FUN_80195ec8(0xe,(int)guest_of(&DAT_801bef60));
    if (DAT_801c4580 == '\0') {
      DAT_801bef6d = DAT_801bef6d + '\x03';
    }
    DAT_801c4580 = '\0';
  case '\x11':
  case '\x12':
  case '\x13':
    DAT_801bef6d = DAT_801bef6d + '\x01';
    break;
  case '\x14':
    iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
    if (iVar3 == 2) {
      cd_file_open(DAT_801bef80,(uint8_t *)guest_at(0x1f800f30));
      FUN_80195ec8(2,0x1f800f30);
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    else if (iVar3 == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case '\x15':
    iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
    if (iVar3 == 2) {
      FUN_80156114(0xffffffff);
      FUN_80195ec8(3,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    else if (iVar3 == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case '\x16':
    iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
    if (iVar3 == 2) {
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    else if (iVar3 == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case '\x17':
    FUN_80195ec8(1,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    iVar3 = FUN_80195d44(1,guest_of(&DAT_801bef64));
    if (iVar3 == 1) {
      if ((DAT_801bef68 & 0x80) != 0) {
        return;
      }
      local_18 = DAT_801bef67;
      local_17 = DAT_801bef68;
      local_16 = DAT_801bef69;
      DAT_801bef80 = FUN_801962f0(&local_18);
      return;
    }
    if (iVar3 != 4) {
      if (iVar3 != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    }
joined_r0x80157c80:
    if ((DAT_801bef6e & 0x10) == 0) {
      FUN_80156114(0);
      FUN_80195ec8(9,0);
      DAT_801bef6d = '\0';
      DAT_801bef6f = -1;
      DAT_801bef80 = DAT_801bef84;
    }
    else {
      DAT_801bef6d = '\x10';
      DAT_801bef78 = DAT_801bef78 + 1;
      DAT_801bef80 = DAT_801bef84;
    }
    break;
  case '\x18':
    iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
    if (iVar3 == 2) {
      DAT_801bef6d = DAT_801bef6d + -1;
    }
    else if (iVar3 == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case ' ':
    FUN_80156114(0);
    DAT_801bef74 = DAT_801bef74 + 1;
    if ((DAT_801bef64 & 0x10) == 0) {
      DAT_801bef6d = '#';
    }
    else {
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    break;
  case '!':
    FUN_80195ec8(1,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    break;
  case '\"':
    iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
    if (iVar3 == 2) {
      if ((DAT_801bef64 & 0x10) == 0) {
        DAT_801bef6d = DAT_801bef6d + '\x01';
      }
      else {
        DAT_801bef6d = DAT_801bef6d + -1;
      }
    }
    else if (iVar3 == 5) {
      DAT_801bef6d = DAT_801bef6d + -1;
    }
    break;
  case '#':
    FUN_80195ec8(0x13,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    break;
  case '$':
    iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
    if (iVar3 != 2) {
      if (iVar3 != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    }
    goto switchD_80157ebc_caseD_30;
  case '0':
    FUN_80195ec8(1,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    break;
  case '1':
    iVar3 = FUN_80195d24(1,(undefined1 *)guest_of(&DAT_801bef64));
    if (iVar3 != 2) {
      if (iVar3 != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    }
    uVar2 = cd_dir_read_wrap(&DAT_801bef8c);
    if ((int)uVar2 < 1) {
      DAT_801bef6d = DAT_801bef6d + -1;
      return;
    }
    DAT_801bef6c = (byte)uVar2;
    if ((int)(uVar2 & 0xff) < (int)(short)DAT_801bef72) {
      DAT_801bef72 = (ushort)((int)(short)DAT_801bef72 % (int)(uVar2 & 0xff));
    }
    if (DAT_801bef72 == 0) {
      DAT_801bef72 = (ushort)DAT_801bef6c;
    }
    DAT_801bef84 = FUN_801962f0(&DAT_801bef8c + (short)DAT_801bef72 * 4);
switchD_80157ebc_caseD_30:
    DAT_801bef6d = '\x10';
  }
  return;
}

/* ======== 0x8015f6a0 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_8015f6a0(int param_1)

{
  int iVar1;
  undefined *puVar2;
  char *pcVar3;
  int iVar4;
  int unaff_s2;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined2 local_c0;
  undefined1 local_be;
  char local_bd;
  undefined1 local_bc;
  undefined1 local_bb;
  ushort local_ba;
  ushort local_b8;
  code *local_b4;
  undefined2 local_b0;
  undefined2 local_ae;
  undefined2 local_ac;
  short local_aa;
  short local_a8;
  undefined2 local_a6;
  undefined2 local_a4;
  undefined2 local_a2;
  undefined2 local_a0;
  undefined2 local_9e;
  char *acStack_78;   /* guest-backed staging buffer @0x1f800f80 */
  undefined4 local_38;
  int local_34;
  int local_30;
  char *local_2c;
  
  uVar7 = 0;
  uVar6 = 0;
  iVar4 = 0;
  iVar5 = 0;
  /* FUN_8018ef60 and FUN_8012e58c both take host string pointers;
   * back both with scratchpad 0x1f800f80 so one buffer serves both. */
  acStack_78 = (char *)guest_at(0x1f800f80);
  local_2c = acStack_78;
  local_34 = -1;
  local_30 = 0;
  _DAT_1f80008c = 0;
  do {
    if (uVar7 != 1) {
      if (uVar7 < 2) {
        if ((uVar7 == 0) && (iVar1 = FUN_8012aaa8(), iVar1 == 0)) {
          FUN_8012cbc0(2,0,0);
          iVar1 = intro_movie_state(0);
          if (iVar1 != 0) {
            FUN_80156ef4(0x203,0,0);
            local_b4 = (code *)FUN_80160598;
            local_ba = 4;
            local_b8 = 4;
            local_b0 = 0x10;
            local_ae = 0xffff;
            _DAT_801e8508 = 0;
            local_c0 = 0;
            local_be = 0;
            local_bd = '\0';
            local_bc = 1;
            local_bb = 0;
            local_ac = 0;
            local_aa = 0;
            local_a8 = 0;
            local_a6 = 0;
            local_a4 = 0;
            local_a2 = 0;
            local_a0 = 0;
            local_9e = 0;
            _DAT_801e8518 = 0x30;
            if (param_1 != 0) {
              uVar6 = 0xffffffff;
              iVar4 = 1;
              iVar5 = 0;
              local_bd = '\x11';
            }
            battle_session_arm(0x1f800000,0xffffffff);
            uVar7 = 1;
            goto LAB_8015f820;
          }
        }
        goto LAB_80160550;
      }
      if (uVar7 == 0xf0) {
        FUN_80156ef4(0x203,0,0);
      }
      else if (uVar7 != 0xf1) goto LAB_80160550;
      uVar7 = 0xff;
      goto LAB_80160558;
    }
LAB_8015f820:
    if (param_1 == 0) {
      FUN_801294cc(&local_c0);
    }
    switch(local_bd) {
    case '\0':
      if ((_DAT_1f80007c & 0x10) != 0) {
        FUN_80156ef4(0x400,0xd,0);
        uVar7 = 0xf0;
      }
      if ((local_b8 == local_ba) && ((_DAT_1f80007c & 0xe0) != 0)) {
        FUN_80156ef4(0x400,0xc,0);
LAB_8015f8c0:
        uVar7 = 0xf0;
      }
      break;
    case '\x10':
      uVar6 = 1;
      local_34 = -1;
      if (local_b8 == 2) {
        iVar5 = (int)local_aa;
        iVar4 = 0;
        goto code_r0x8015ffac;
      }
      if (local_b8 < 3) {
        if (local_b8 == 1) {
          iVar4 = 4;
          iVar5 = 0;
LAB_8015fbfc:
          local_bd = '\x12';
        }
      }
      else if (local_b8 == 3) {
        iVar5 = (int)local_a8;
        iVar4 = 4;
        goto LAB_8015fbfc;
      }
      break;
    case '\x11':
      if ((int)uVar6 < 0) goto code_r0x8015ffac;
      FUN_8012cd9c(0x10,0x68);
      switch(iVar4) {
      case 0:
        FUN_8018ef60((char *)guest_at(0x1f800f80),s_LOAD_FROM_MEMORY_CARD__s___80121788,
                     SLOTSTR(iVar5));
        break;
      case 2:
        FUN_8018ef60((char *)guest_at(0x1f800f80),s_FORMAT_MEMORY_CARD__s___801217d8,SLOTSTR(iVar5));
        break;
      case 3:
        FUN_8018ef60((char *)guest_at(0x1f800f80),s_SAVING_TO_MEMORY_CARD__s__801217f0,SLOTSTR(iVar5));
        FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,local_2c,0x10,0x1000,0);
        FUN_8012cdd8(0,0x10);
        FUN_8018ef60((char *)guest_at(0x1f800f80),s_OVER_WRITE_OK___8012180c);
      default:
        break;
      case 5:
        FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_MEMORY_CARD_CHANGED__801217a4,0x10,0x1000,2);
        FUN_8012cdd8(0,0x10);
      case 1:
        FUN_8018ef60((char *)guest_at(0x1f800f80),s_SAVE_TO_MEMORY_CARD__s___801217bc,SLOTSTR(iVar5))
        ;
      }
      FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,acStack_78,0x10,0x1000,0);
      FUN_8012cdd8(0,0x10);
      FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,&DAT_8012181c,0x30,0x1000,uVar6 != 0);
      FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,&DAT_80121820,0x90,0x1000,uVar6 == 0);
      FUN_8012cdd8(0,0x10);
      switch(iVar4) {
      case 1:
      case 5:
        FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_1_BLOCK_IS_REQUIRED_TO_SAVE__80121824,0x10,0x1000
                     ,3);
      }
      FUN_8012cdd8(0,0x10);
      if ((_DAT_1f80007c & 0xa000) != 0) {
        FUN_80156ef4(0x400,0xb,0);
        uVar6 = uVar6 ^ 1;
      }
      if ((_DAT_1f80007c & 0x10) == 0) {
        if ((_DAT_1f80007c & 0xe0) != 0) {
          FUN_80156ef4(0x400,0xc,0);
          switch(iVar4) {
          case 0:
          case 1:
switchD_8015fbc4_caseD_0:
            if (uVar6 == 0) goto LAB_8015fbfc;
            goto LAB_8015fc08;
          case 2:
          case 3:
          case 5:
            if (param_1 == 0) goto switchD_8015fbc4_caseD_0;
            if (uVar6 == 0) goto LAB_8015fbfc;
            local_bd = '\x11';
            iVar4 = 1;
            uVar6 = 1;
          }
        }
      }
      else {
        FUN_80156ef4(0x400,0xd,0);
LAB_8015fc08:
        local_bd = ' ';
      }
      break;
    case '\x12':
    case '\x13':
      FUN_8012cd9c(0x10,0x68);
      switch(iVar4) {
      case 0:
        FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_NOW_LOADING_FROM_MEMORY_CARD_80121844,0x10,0x1000
                     ,0);
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s__s____80121864;
        break;
      case 2:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_NOW_FORMATTING_MEMORY_CARD__s____8012188c;
        break;
      case 3:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_NOW_SAVING_TO_MEMORY_CARD__s____8012186c;
        break;
      case 4:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_NOW_CHECKING_MEMORY_CARD__s____801218b0;
        break;
      case 5:
        iVar4 = 1;
      case 1:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_NOW_SAVING_TO_MEMORY_CARD__s____8012186c;
        break;
      default:
        goto switchD_8015fc3c_default;
      }
      FUN_8018ef60((char *)guest_at(0x1f800f80),pcVar3,puVar2);
switchD_8015fc3c_default:
      FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,acStack_78,0x10,0x1000,0);
      FUN_8012cdd8(0,0x10);
      FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_DO_NOT_REMOVE_MEMORY_CARD___801218d0,0x10,0x1000,3)
      ;
      goto code_r0x8015ffac;
    case '\x14':
      if (iVar5 == 0) {
        local_38 = 0;
      }
      else if (iVar5 == 1) {
        local_38 = 0x10;
      }
      switch(iVar4) {
      case 0:
        unaff_s2 = FUN_8016809c(local_38,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608,DAT_801b460c,DAT_801b4610,DAT_801b4614,
                                PTR_DAT_801b4618,&DAT_801c11e0,0x414);
        if ((unaff_s2 == 0) && (iVar1 = FUN_80167ae8(), iVar1 != 0)) {
          unaff_s2 = 2;
        }
        break;
      case 1:
        unaff_s2 = FUN_80167d7c(local_38,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608,DAT_801b460c,DAT_801b4610,DAT_801b4614,
                                PTR_DAT_801b4618,&DAT_801c11e0,0x414);
        break;
      case 2:
        unaff_s2 = FUN_80167b34(local_38);
        break;
      case 3:
        unaff_s2 = FUN_80167ce4(local_38,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608,DAT_801b460c,DAT_801b4610,DAT_801b4614,
                                PTR_DAT_801b4618,&DAT_801c11e0,0x414);
        break;
      case 4:
        unaff_s2 = FUN_80167bd8(local_38,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608);
      }
      local_30 = 100;
      local_34 = unaff_s2;
      if (param_1 == 0) {
        local_30 = -1;
      }
code_r0x8015ffac:
      local_bd = local_bd + '\x01';
      break;
    case '\x15':
      FUN_8012cd9c(0x10,0x68);
      switch(iVar4) {
      case 0:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_LOAD_FROM_MEMORY_CARD__s__801218ec;
        break;
      case 1:
      case 3:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_SAVE_TO_MEMORY_CARD__s__80121908;
        break;
      case 2:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_FORMAT_MEMORY_CARD__s__80121920;
        break;
      case 4:
        puVar2 = SLOTSTR(iVar5);
        pcVar3 = s_CHECKING_MEMORY_CARD__s__80121938;
        break;
      default:
        goto switchD_8015ffe4_default;
      }
      FUN_8018ef60((char *)guest_at(0x1f800f80),pcVar3,puVar2);
switchD_8015ffe4_default:
      FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,local_2c,0x10,0x1000,0);
      FUN_8012cdd8(0,0x10);
      if ((_DAT_1f80007c & 0xe0) != 0) {
        FUN_80156ef4(0x400,0xc,0);
        local_bd = '\x16';
      }
      if ((_DAT_1f80007c & 0x10) != 0) {
        FUN_80156ef4(0x400,0xd,0);
        local_bd = '\x16';
      }
      if (unaff_s2 == 4) {
        if (iVar4 == 0) {
          pcVar3 = s_NO_BLOODY_ROAR_2_DATA_DETECTED___801219e4;
        }
        else {
          pcVar3 = s_UNFORMATTED_MEMORY_CARD___80121a08;
        }
      }
      else if (unaff_s2 < 5) {
        if (unaff_s2 != 1) {
          if (unaff_s2 < 2) {
            if (unaff_s2 != 0) goto LAB_801603ec;
            switch(iVar4) {
            case 0:
              FUN_80156048(*(undefined1 *)(_g_system_config_ptr + 0xe),
                           ((uint)*(byte *)(_g_system_config_ptr + 0xf) << 3) / 10);
              FUN_801561c8(*(byte *)(_g_system_config_ptr + 0xc) ^ 1);
            case 1:
            case 3:
              _DAT_801c15f0 = 0;
              FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_COMPLETED___80121954,0x10,0x1000,3);
              iVar1 = local_30 + -1;
              if ((-1 < local_30) && (local_30 = iVar1, iVar1 == -1)) {
                local_bd = '\x16';
              }
              break;
            case 2:
              uVar6 = 0xffffffff;
              if (param_1 == 0) {
                if (local_b8 != 2) {
                  if (local_b8 < 3) {
                    if (local_b8 == 1) goto LAB_8016026c;
                    local_bd = '\x11';
                  }
                  else {
                    if (local_b8 == 3) {
                      iVar4 = 1;
                      iVar5 = (int)local_a8;
                      goto LAB_8016028c;
                    }
                    local_bd = '\x11';
                  }
                  break;
                }
                iVar5 = (int)local_aa;
                iVar4 = 0;
              }
              else {
LAB_8016026c:
                iVar4 = 1;
                iVar5 = 0;
              }
LAB_8016028c:
              local_bd = '\x11';
              break;
            case 4:
              iVar4 = 1;
              goto LAB_80160464;
            }
          }
          else if (unaff_s2 == 2) {
            FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_DATA_ERROR____80121a24,0x10,0x1000,2);
          }
          else {
            if (unaff_s2 != 3) goto LAB_801603ec;
            uVar6 = 1;
            if (iVar4 == 4) {
              iVar4 = 1;
            }
            else {
              iVar4 = 5;
            }
            local_bd = '\x11';
          }
          break;
        }
        FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_NO_MEMORY_CARD_IS_INSERTED_IN_80121960,0x10,
                     0x1000,2);
        FUN_8018ef60((char *)guest_at(0x1f800f80),s_MEMORY_CARD__s___80121980,SLOTSTR(iVar5));
        pcVar3 = local_2c;
      }
      else if (unaff_s2 == 7) {
        FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_THERE_IS_INSUFFICIENT_MEMORY_CAR_80121994,0x10,
                     0x1000,2);
        FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,s_BLOCKS_AND_1_BLOCK_IS_REQUIRED_801219b8,0x10,
                     0x1000,2);
        pcVar3 = s_TO_SAVE__801219d8;
      }
      else {
        if (unaff_s2 < 8) {
          if (unaff_s2 == 5) {
            pcVar3 = s_NO_BLOODY_ROAR_2_DATA_DETECTED___801219e4;
            goto LAB_801603fc;
          }
        }
        else if (unaff_s2 == 0xf) {
          iVar4 = 3;
LAB_80160488:
          uVar6 = 1;
          local_bd = '\x11';
          break;
        }
LAB_801603ec:
        pcVar3 = s_ERROR___80121a34;
      }
LAB_801603fc:
      FUN_8012e58c(_DAT_1f800008,_DAT_1f80000c,pcVar3,0x10,0x1000,2);
      break;
    case '\x16':
      if (iVar4 == 0) {
        if (param_1 != 0) goto LAB_8015f8c0;
        if (unaff_s2 == 0) goto LAB_80160510;
        if (local_b8 != 1) {
          local_bd = '\0';
          break;
        }
        goto LAB_80160504;
      }
      if (unaff_s2 == 5) {
LAB_80160494:
        if (param_1 != 0) goto LAB_8015f8c0;
LAB_801604a4:
        if (unaff_s2 != 0) goto LAB_801604f4;
        goto LAB_80160510;
      }
      if (5 < unaff_s2) {
        if (unaff_s2 == 0xf) goto LAB_80160494;
LAB_80160470:
        if (param_1 == 0) goto LAB_801604a4;
        iVar4 = 1;
        goto LAB_80160488;
      }
      if (unaff_s2 < 4) {
        if ((unaff_s2 < 1) && (unaff_s2 == 0)) goto LAB_80160494;
        goto LAB_80160470;
      }
      iVar4 = 2;
LAB_80160464:
      uVar6 = 1;
      local_bd = '\x11';
      break;
    case ' ':
    case '!':
      if (param_1 != 0) goto LAB_8015f8c0;
LAB_801604f4:
      if (local_b8 == 1) {
LAB_80160504:
        *(undefined1 *)(_g_system_config_ptr + 0x22) = 0;
      }
LAB_80160510:
      local_bd = '\0';
    }
    if (param_1 == 0) {
      FUN_8012c374(0x1f800000,_DAT_1f800008,7);
    }
LAB_80160550:
    frame_tick(0x1f800000);
LAB_80160558:
    if (uVar7 == 0xff) {
      return local_34;
    }
  } while( true );
}

/* ======== 0x80156ff0 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80156ff0(void)

{
  short sVar1;
  int iVar2;
  undefined1 *puVar3;
  char *pcVar4;
  int iVar5;
  char acStack_38 [24];
  
  FUN_8019da44(acStack_38);
  pcVar4 = &DAT_801f24d0;
  iVar5 = 0;
  puVar3 = &DAT_801f24d1;
  do {
    if (*pcVar4 != '\0') {
      if (*pcVar4 == -0x80) {
        *(undefined2 *)(puVar3 + 0xb) = 0;
        *pcVar4 = '\x01';
        _DAT_801f2650 = *(undefined4 *)(puVar3 + 7);
        _DAT_801f2654 = 0x600e3;
        _DAT_801f2666 = 0;
        iVar2 = (uint)*(ushort *)(puVar3 + 5) * 8;
        _DAT_801f2668 = *(undefined2 *)guest_at((uint32_t)(iVar2 + -0x7fe0d41e));
        _DAT_801f2658 = *(undefined2 *)(puVar3 + 1);
        _DAT_801f265a = *(undefined2 *)(puVar3 + 3);
        _DAT_801f266c = *(undefined4 *)guest_at((uint32_t)((uint)*(ushort *)guest_at((uint32_t)(iVar2 + -0x7fe0d420)) * 4 + -0x7fe0d920));
        _DAT_801f268a = *(undefined2 *)guest_at((uint32_t)(iVar2 + -0x7fe0d41c));
        _DAT_801f268c = *(undefined2 *)guest_at((uint32_t)(iVar2 + -0x7fe0d41a));
        cd_reg_window_update(&DAT_801f2650);
      }
      else {
        sVar1 = *(short *)(puVar3 + 0xb);
        *(short *)(puVar3 + 0xb) = sVar1 + 1;
        if ((short)(sVar1 + 1) == 2) {
          FUN_8019d220(0,*(undefined4 *)(puVar3 + 7));
        }
        if ((acStack_38[iVar5] == '\0') && (0x14 < *(short *)(puVar3 + 0xb))) {
          *pcVar4 = '\0';
          *puVar3 = 0;
        }
      }
    }
    iVar5 = iVar5 + 1;
    puVar3 = puVar3 + 0x10;
    pcVar4 = pcVar4 + 0x10;
  } while (iVar5 < 0x18);
  return;
}

/* ======== 0x80156d98 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80156d98(undefined4 param_1,int param_2)

{
  ushort uVar1;
  bool bVar2;
  bool bVar3;
  ushort uVar4;
  short *psVar5;
  ushort *puVar6;
  int iVar7;
  
  bVar2 = false;
  bVar3 = false;
  puVar6 = (ushort *)&DAT_801f3600;
  iVar7 = 0;
  psVar5 = (short *)&DAT_801f3602;
  do {
    uVar1 = *puVar6;
    if (*puVar6 != 0) {
      if (psVar5[1] == 0) {
        uVar4 = uVar1 & 0xf00;
        if (uVar4 == 0x200) {
          param_2 = FUN_80156cf4(uVar1 & 0xff,(int)*psVar5);
        }
        else if (uVar4 < 0x201) {
          if (uVar4 == 0x100) {
            param_2 = FUN_80156c60(uVar1 & 0xff,(int)*psVar5);
          }
        }
        else if (uVar4 == 0x300) {
          if (bVar2) goto LAB_80156ebc;
          bVar2 = true;
          param_2 = FUN_80156d88(uVar1 & 0xff,(int)*psVar5);
        }
        else if (uVar4 == 0x400) {
          if (bVar3) goto LAB_80156ebc;
          bVar3 = true;
          param_2 = FUN_801568fc(uVar1 & 0xff,(int)*psVar5);
        }
        if (param_2 != 0) {
          *puVar6 = 0;
          _DAT_801f35f4 = _DAT_801f35f4 + -1;
        }
      }
      else {
        psVar5[1] = psVar5[1] + -1;
      }
    }
LAB_80156ebc:
    iVar7 = iVar7 + 1;
    psVar5 = psVar5 + 3;
    puVar6 = puVar6 + 3;
    if (0x1f < iVar7) {
      return;
    }
  } while( true );
}

/* ======== 0x8015ab4c ======== */

void FUN_8015ab4c(void)

{
  undefined2 uVar1;
  undefined1 *puVar2;
  undefined2 *puVar3;
  int iVar4;
  
  iVar4 = 0;
  puVar3 = &DAT_801b3f78;
  puVar2 = &DAT_801c11e0;
  do {
    uVar1 = *puVar3;
    puVar3 = puVar3 + 1;
    iVar4 = iVar4 + 1;
    *(undefined2 *)(puVar2 + 0x400) = uVar1;
    *(undefined2 *)(puVar2 + 0x3f0) = uVar1;
    puVar2 = puVar2 + 2;
  } while (iVar4 < 8);
  return;
}

/* ======== 0x80167a64 ======== */

void FUN_80167a64(void)

{
  undefined *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  puVar1 = PTR_s___BloodyRoar2__byRAIZINGver1_60_801b43f0;
  iVar4 = 0;
  iVar6 = 0x1f;
  do {
    iVar6 = iVar6 + -1;
    puVar2 = &DAT_801c11e0 + iVar4;
    puVar3 = puVar1 + iVar4;
    iVar4 = iVar4 + 1;
    *puVar2 = *puVar3;
  } while (-1 < iVar6);
  iVar4 = 0;
  iVar6 = 0x264;
  do {
    iVar5 = 10;
    puVar2 = &DAT_801c11f4 + iVar6;
    *(undefined2 *)(&DAT_801c11ec + iVar6) = 0;
    *(undefined2 *)(&DAT_801c11e8 + iVar6) = 0;
    *(undefined2 *)(&DAT_801c11e4 + iVar6) = 0;
    *(undefined2 *)guest_at((uint32_t)(iVar6 + -0x7fe3ee1e)) = 0;
    *(undefined2 *)(&DAT_801c11e0 + iVar6) = 0;
    do {
      *(undefined2 *)(puVar2 + 0xe) = 0;
      iVar5 = iVar5 + -1;
      puVar2 = puVar2 + -2;
    } while (-1 < iVar5);
    iVar4 = iVar4 + 1;
    iVar6 = iVar6 + 0x24;
  } while (iVar4 < 0xb);
  return;
}

/* ======== 0x80163ab4 ======== */

void FUN_80163ab4(void)

{
  FUN_801638c8(&DAT_801c1264,0,10);
  FUN_801638c8(&DAT_801c1304,5,10);
  FUN_801638c8(0x801c13a4,0,10);
  return;
}

/* ======== 0x8018babc ======== */

void FUN_8018babc(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  DAT_801f5969 = 0;
  DAT_801f5968 = 0;
  if (param_1 == 1) {
    DAT_801b55d0 = 1;
    return;
  }
  if (param_1 < 2) {
    if (param_1 == 0) {
LAB_8018bb4c:
      iVar3 = 0;
      do {
        iVar2 = 0;
        iVar1 = iVar3 * 2;
        do {
          iVar2 = iVar2 + 1;
          (&DAT_801f596c)[iVar1] = 0;
          (&DAT_801f5970)[iVar1] = 0;
          iVar1 = iVar2 + iVar3 * 2;
        } while (iVar2 < 2);
        iVar3 = iVar3 + 1;
      } while (iVar3 < 2);
      DAT_801b55d0 = (char)param_1;
      return;
    }
  }
  else {
    if (param_1 == 2) goto LAB_8018bb4c;
    if (param_1 == 3) {
      DAT_801b55d0 = 1;
      DAT_801f5968 = 0;
      DAT_801f5969 = 0;
      DAT_801f596c = 0x1e;
      DAT_801f596d = 0x1e;
      DAT_801f596e = 0x1e;
      DAT_801f596f = 0x1e;
      DAT_801f5970 = 0x40;
      DAT_801f5971 = 1;
      DAT_801f5972 = 0x40;
      DAT_801f5973 = 1;
      return;
    }
  }
  return;
}

/* FUN_80156048 lives in menu.c (disasm-verified magic-divide port). */

/* ======== 0x801561c8 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801561c8(undefined2 param_1)

{
  _DAT_801f36c0 = param_1;
  FUN_80156114(0xffffffff);
  return;
}


/* ======== system/shell helpers ======== */

/* ======== 0x801525c4 ======== */

void FUN_801525c4(intptr_t param_1)

{
  uint8_t *p = (uint8_t *)guest_at((uint32_t)param_1);
  *(undefined2 *)(p + 0xb4) = 0;
  *(undefined2 *)(p + 0xb6) = 0;
  return;
}

/* ======== 0x80129f64 ======== */

void FUN_80129f64(undefined1 param_1)

{
  DAT_801e83aa = param_1;
  DAT_801e83b0 = param_1;
  return;
}

/* ======== 0x80129ff4 ======== */

void FUN_80129ff4(void)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  
  puVar1 = (undefined1 *)guest_at(0x801e8400);
  iVar2 = 0;
  do {
    iVar3 = iVar2 + 1;
    *puVar1 = (&DAT_8011ec40)[iVar2];
    puVar1 = (undefined1 *)guest_at((uint32_t)(iVar2 + -0x7fe17bff));
    iVar2 = iVar3;
  } while (iVar3 < 0x100);
  return;
}

/* ======== 0x8012a030 ======== */

void FUN_8012a030(uint param_1)

{
  byte bVar1;
  undefined1 uVar2;
  undefined1 *puVar3;
  int iVar4;
  
  iVar4 = 0;
  do {
    puVar3 = (undefined1 *)guest_at((uint32_t)(iVar4 + -0x7fe17c00));
    bVar1 = (&DAT_8011ec40)[param_1 & 0xff];
    uVar2 = *puVar3;
    iVar4 = iVar4 + 1;
    *puVar3 = *(undefined1 *)guest_at((uint32_t)(bVar1 + 0x801e8400));
    *(undefined1 *)guest_at((uint32_t)(bVar1 + 0x801e8400)) = uVar2;
    param_1 = param_1 + 1;
  } while (iVar4 < 0x100);
  return;
}

/* ======== 0x8012aa6c ======== */

void FUN_8012aa6c(undefined1 param_1,int param_2)

{
  DAT_1f800017 = param_1;
  DAT_1f800018 = (char)param_2;
  DAT_1f800014 = DAT_1f80000c;
  if (param_2 == 0) {
    DAT_1f800019 = 0;
    return;
  }
  DAT_1f800019 = (char)(0x100 / param_2) + '\x01';
  return;
}

/* ======== 0x8019a5d8 ======== */

void FUN_8019a5d8(int param_1,int param_2)

{
  param_2 = param_2 << 0xf;
  do {
    if (param_1 <= DAT_801bdde0) {
      return;
    }
    param_2 = param_2 + -1;
  } while (param_2 != -1);
  FUN_80197ad0(s_VSync__timeout_80125b38);
  FUN_80199fe0(0);
  FUN_8019a670(3,0);
  return;
}

/* ======== 0x8012b49c ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8012b49c(undefined1 param_1)

{
  int iVar1;
  int iVar2;
  
  uint32_t *p84;
  empty_ret();
  _DAT_1f800082 = 0;
  p84 = (uint32_t *)guest_at(_DAT_1f800084);
  *p84 = 0;
  p84[1] = 0;
  p84[2] = 0;
  iVar2 = 0;
  p84[4] = param_1;
  do {
    iVar1 = iVar2 * 2;
    iVar2 = iVar2 + 1;
    *(undefined2 *)((char *)p84 + (iVar1 + 6) * 4) = 0;
  } while (iVar2 < 8);
  if (_DAT_1f8000b8 == 0) {
    p84[3] = 3;
  }
  else {
    p84[3] = 4;
  }
  if (DAT_1f80009b == '\x04') {
    p84[3] = 7;
  }
  return;
}

/* ======== 0x8018e2f0 ======== */

void FUN_8018e2f0(uint32_t addr,uint32_t size)

{
  /* guest: BIOS/kernel call LAB_000000a0 = bzero(dst,len).
   * Called as (bss_addr, byte_count) from entry_0x80126178. */
  memset(guest_at(addr),0,size);
  return;
}

/* ======== 0x8012a650 ======== */

uint FUN_8012a650(uint8_t *param_1)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = *(byte *)(param_1 + 1) & 0xf0;
  if (bVar1 == 0x40) {
    uVar2 = ~(uint)CONCAT11(*(undefined1 *)(param_1 + 2),*(undefined1 *)(param_1 + 3)) & 63999;
  }
  else if ((bVar1 < 0x41) || (bVar1 == 0x60)) {
    uVar2 = 0xffffffff;
  }
  else {
    if (bVar1 < 0x61) {
      if (bVar1 != 0x50) {
        return 0xffffffff;
      }
    }
    else if (bVar1 != 0x70) {
      return 0xffffffff;
    }
    uVar2 = ~(uint)CONCAT11(*(undefined1 *)(param_1 + 2),*(undefined1 *)(param_1 + 3)) & 63999;
    if (*(uint8_t *)(param_1 + 6) < 0x40) {
      uVar2 = uVar2 | 0x8000;
    }
    else if (0xc0 < *(uint8_t *)(param_1 + 6)) {
      uVar2 = uVar2 | 0x2000;
    }
    if (*(uint8_t *)(param_1 + 7) < 0x40) {
      uVar2 = uVar2 | 0x1000;
    }
    else if (0xc0 < *(uint8_t *)(param_1 + 7)) {
      uVar2 = uVar2 | 0x4000;
    }
  }
  return uVar2;
}

/* ---- batch30: memcard/PAD helpers ---- */
#define SB8(a)   (*(uint8_t  *)guest_at(a))
#define SB16(a)  (*(uint16_t *)guest_at(a))
#define SB32(a)  (*(uint32_t *)guest_at(a))
extern uint8_t *_g_pad_buffer;
extern int FUN_8012aaa8(void);
extern int FUN_80163520(uint8_t *,uint32_t);
extern void FUN_80156ef4(int,int,int);
extern void FUN_801573ec(void);
extern void FUN_801562e0(void);
extern void FUN_80156290(void);
extern void FUN_801575bc(int,uint32_t);
extern void FUN_8015768c(int,uint32_t);
extern int  FUN_80195ffc(int,int,int);

/* FUN_80163868 — memcard error sentinel (-99). */
uint32_t FUN_80163868(void)
{
  return 0xffffff9d;
}

/* FUN_801577bc — PAD-start gated memcard confirm dispatch.
 * pad_buffer+2 bit12 = confirm press; DAT_801bef6e low nibble =
 * dialog state (1 -> FUN_801575bc path, 2 -> FUN_8015768c path). */
void FUN_801577bc(int32_t param_1,uint32_t param_2)
{
  if ((((*(uint16_t *)(_g_pad_buffer + 2) >> 0xc) & 1) != 0) && (0 <= param_1)) {
    FUN_801573ec();
    if ((SB8(0x801bef6e) & 0xf) == 1) {
      FUN_801562e0();
      FUN_801575bc(param_1,param_2);
    } else if ((SB8(0x801bef6e) & 0xf) == 2) {
      FUN_80156290();
      FUN_8015768c(param_1,param_2);
    }
  }
}

/* FUN_80157368 — memcard init/reset sequence: brightness off,
 * sync wait via FUN_80195ffc(9), dialog-state globals cleared. */
void FUN_80157368(void)
{
  int iVar1;
  FUN_80156114(0);
  do {
    iVar1 = FUN_80195ffc(9,0,0);
  } while (iVar1 == 0);
  FUN_801562e0();
  FUN_80156290();
  SB8(0x801bef6e) = 2;
  SB8(0x801bef6d) = 0;
  SB8(0x801bef76) = 0;
  SB8(0x801bef6f) = 0xff;
  SB8(0x801bef80) = SB8(0x801bef84);
}

/* FUN_8016700c — save-slot state fixup @guest 0x1f800000.
 * param_1 = guest address (bridged). */
void FUN_8016700c(uint32_t param_1,uint32_t param_2)
{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  int32_t iVar1;
  (void)param_2;
  iVar1 = FUN_8012aaa8();
  if (iVar1 == 0) {
    iVar1 = FUN_80163520(p,param_2);
    if (0 <= iVar1) {
      p[0] = 5;
      p[1] = 0xf0;
    }
    if ((iVar1 == -99) && ((*(uint16_t *)(p + 0x7c) & 0x840) != 0)) {
      FUN_80156ef4(0x400,0xc,0);
      p[0] = 5;
      p[1] = 0xf0;
    }
    if ((*(uint16_t *)(p + 0x7c) & 0x10) != 0) {
      FUN_80156ef4(0x400,0xd,0);
      p[1] = 0xf0;
    }
  }
}

/* FUN_80153264 — memcard button-press handler: PAD bits 0x840 on
 * +0x68/+0x6a trigger FUN_80156ef4 sfx + FUN_8015260c slot action.
 * param_1 = host ptr to save-manager state block. */
extern int32_t FUN_80163898(uint32_t);
extern void FUN_8015260c(uint8_t *,int32_t);
uint32_t FUN_80153264(uint8_t *param_1)
{
  int32_t iVar1;
  uint32_t uVar2;
  uint32_t uVar3 = 0;
  param_1[0xb4] = 0; param_1[0xb5] = 0; param_1[0xb6] = 0; param_1[0xb7] = 0;
  if (param_1[0x9b] == 0) {
    if (((*(uint16_t *)(param_1 + 0x68) & 0x840) != 0) &&
        (iVar1 = FUN_80163898(0), 0 <= iVar1)) {
      uVar2 = (*param_1 == 5) ? 0x80 : 0xc;
      FUN_80156ef4(0x400,uVar2,0);
      FUN_8015260c(param_1,0);
      uVar3 = 1;
    }
    if ((*(uint16_t *)(param_1 + 0x6a) & 0x840) == 0) return uVar3;
    iVar1 = FUN_80163898(1);
    if (iVar1 < 0) return uVar3;
    if (*param_1 == 5) { uVar2 = 0x80; goto LAB_801533b0; }
  } else {
    if (3 < param_1[0x9b]) return 0;
    if ((*(uint16_t *)(param_1 + 0x68) & 0x840) != 0) {
      uVar2 = (*param_1 == 5) ? 0x80 : 0xc;
      FUN_80156ef4(0x400,uVar2,0);
      FUN_8015260c(param_1,0);
      uVar3 = 1;
    }
    if ((*(uint16_t *)(param_1 + 0x6a) & 0x840) == 0) return uVar3;
    if (*param_1 == 5) { uVar2 = 0x80; goto LAB_801533b0; }
  }
  uVar2 = 0xc;
LAB_801533b0:
  FUN_80156ef4(0x400,uVar2,0);
  FUN_8015260c(param_1,1);
  return uVar3 | 2;
}

/* FUN_801638c8 — 16-byte record copy between caller buffer and one
 * of four static tables selected by param_2:
 *   0: 0x801b427c -> dst   1: dst -> 0x801c1264   2: 0x801c1264 -> dst
 *   3: dst -> 0x801c1304   4: 0x801c1304 -> dst   5: 0x801b431c -> dst
 * param_3 = record count. */
void FUN_801638c8(uint32_t *param_1,uint32_t param_2,int32_t param_3)
{
  int32_t iVar1;
  uint32_t *puVar2, *puVar3;
  uint32_t uVar4, uVar5, uVar6;
  puVar2 = (uint32_t *)g_DAT_801c1264;
  puVar3 = (uint32_t *)g_DAT_801c1304;
  switch (param_2) {
  case 0:
    iVar1 = 0;
    if (0 < param_3) {
      puVar2 = (uint32_t *)guest_at(0x801b427c);
      do {
        uVar4 = puVar2[1]; uVar5 = puVar2[2]; uVar6 = puVar2[3];
        param_1[0] = puVar2[0]; param_1[1] = uVar4; param_1[2] = uVar5; param_1[3] = uVar6;
        param_1 += 4; iVar1++; puVar2 += 4;
      } while (iVar1 < param_3);
    }
    break;
  case 1:
    iVar1 = 0;
    if (0 < param_3) {
      do {
        uVar4 = param_1[1]; uVar5 = param_1[2]; uVar6 = param_1[3];
        puVar2[0] = param_1[0]; puVar2[1] = uVar4; puVar2[2] = uVar5; puVar2[3] = uVar6;
        puVar2 += 4; iVar1++; param_1 += 4;
      } while (iVar1 < param_3);
      return;
    }
    break;
  case 2:
    iVar1 = 0;
    if (0 < param_3) {
      do {
        uVar4 = puVar2[1]; uVar5 = puVar2[2]; uVar6 = puVar2[3];
        param_1[0] = puVar2[0]; param_1[1] = uVar4; param_1[2] = uVar5; param_1[3] = uVar6;
        param_1 += 4; iVar1++; puVar2 += 4;
      } while (iVar1 < param_3);
      return;
    }
    break;
  case 3:
    iVar1 = 0;
    if (0 < param_3) {
      do {
        uVar4 = param_1[1]; uVar5 = param_1[2]; uVar6 = param_1[3];
        puVar3[0] = param_1[0]; puVar3[1] = uVar4; puVar3[2] = uVar5; puVar3[3] = uVar6;
        puVar3 += 4; iVar1++; param_1 += 4;
      } while (iVar1 < param_3);
      return;
    }
    break;
  case 4:
    iVar1 = 0;
    if (0 < param_3) {
      do {
        uVar4 = puVar3[1]; uVar5 = puVar3[2]; uVar6 = puVar3[3];
        param_1[0] = puVar3[0]; param_1[1] = uVar4; param_1[2] = uVar5; param_1[3] = uVar6;
        param_1 += 4; iVar1++; puVar3 += 4;
      } while (iVar1 < param_3);
      return;
    }
    break;
  case 5:
    iVar1 = 0;
    if (0 < param_3) {
      puVar2 = (uint32_t *)guest_at(0x801b431c);
      do {
        uVar4 = puVar2[1]; uVar5 = puVar2[2]; uVar6 = puVar2[3];
        param_1[0] = puVar2[0]; param_1[1] = uVar4; param_1[2] = uVar5; param_1[3] = uVar6;
        param_1 += 4; iVar1++; puVar2 += 4;
      } while (iVar1 < param_3);
      return;
    }
  }
  return;
}

/* ======== batch31: memcard slot / date helpers (0x8012,0x8015,0x8016) ======== */
extern uint8_t  *g_DAT_801e40b8;
extern uint8_t  *g_DAT_801e6208;
extern uint32_t *g_PTR_DAT_801b5b94;
extern int      FUN_8018e510(char *);
extern undefined1 FUN_80129f78(void);
extern int32_t  FUN_801574b4(int32_t);
extern void     FUN_801279c8();
extern void     FUN_8012e078(uint32_t,uint32_t,const void *,int32_t,int32_t,int32_t);
extern void     FUN_8016fc74(uint32_t,uint32_t,int32_t);
extern int      FUN_80159d78(uint8_t *);
extern void     FUN_80159f84(uint8_t *);

/* FUN_801274e8 — mirror a mode byte into the two dialog-state cells
 * (guest 0x801e40b8 and 0x801e6208). */
void FUN_801274e8(int32_t param_1)
{
  *g_DAT_801e6208 = (uint8_t)param_1;
  *g_DAT_801e40b8 = (uint8_t)param_1;
}

/* FUN_801513fc — print the slot-name string for fighter index param_2
 * (table @0x801b47ec, 8-byte entries {char *name0, char *name1}) then
 * measure param_1 with FUN_8018e510. Returns FUN_8018e510's result. */
int FUN_801513fc(void *param_1,int32_t param_2)
{
  uint32_t uVar1 = *(uint32_t *)guest_at(0x801b47ec + ((uint32_t)param_2 << 3));
  /* FUN_8018ef60 formats into a host buffer; stage in scratchpad
   * then copy back to the caller's host buffer. */
  uint8_t *tmp = (uint8_t *)guest_at(0x1f800f40);
  int32_t n = FUN_8018ef60((char *)guest_at(0x1f800f40),(char *)guest_at(uVar1));
  memcpy(param_1,tmp,(size_t)n + 1);
  return FUN_8018e510((char *)param_1);
}

/* FUN_8016fc74 — clear the 0x1f80e8/0x1f801f0 channel blocks: zero
 * +0xe0..+0xec fields, store param_2 at +0xf0, then param_3 selects the
 * +0xec counter (0 -> clear, 1 -> increment, other -> untouched).
 * param_1 is a GUEST address (0x1f80xxx scratch). */
void FUN_8016fc74(uint32_t param_1,uint32_t param_2,int32_t param_3)
{
  uint8_t *p = (uint8_t *)guest_at(param_1);
  *(uint16_t *)(p + 0xe0) = 0;
  *(uint16_t *)(p + 0xe6) = 0;
  *(uint16_t *)(p + 0xe8) = 0;
  *(uint16_t *)(p + 0xea) = 0;
  *(uint16_t *)(p + 0xe2) = 0;
  p[0xe4] = 0;
  *(uint32_t *)(p + 0xf0) = param_2;
  if (param_3 == 0) {
    *(uint16_t *)(p + 0xec) = 0;
    return;
  }
  if (param_3 == 1) {
    *(uint16_t *)(p + 0xec) = (uint16_t)(*(uint16_t *)(p + 0xec) + 1);
  }
}

/* FUN_80159d78 — resolve the save-slot icon nibble: index =
 * (int16)param_1[0xa4] + (c0block[7] << 4) into the byte table at
 * guest 0x801b3f88; store value<<4 at param_1+0xa0, return it. */
int FUN_80159d78(uint8_t *param_1)
{
  uint8_t *p = (uint8_t *)guest_at(*(uint32_t *)(param_1 + 0xc0));
  uint32_t uVar1 = 0x801b3f88u + (uint32_t)(int32_t)*(int16_t *)(param_1 + 0xa4) +
                   ((uint32_t)p[7] << 4);
  int32_t iVar1 = *(uint8_t *)guest_at(uVar1);
  iVar1 = iVar1 << 4;
  *(int16_t *)(param_1 + 0xa0) = (int16_t)iVar1;
  return iVar1;
}

/* FUN_80159f84 — shuffle the two 11-byte picker tables at guest
 * 0x801f47a0 / 0x801f4770 using FUN_80129f78() rand bytes, selected by
 * param_1[0x9c]: 0 -> Fisher-Yates on table A (mod 9), rotate the
 * A[7..10] window, swap B[4]/B[5], Fisher-Yates on B (mod 5);
 * 1..5 -> Fisher-Yates on A (mod 9) then B[i] = rand % 8 (11 entries);
 * anything else -> just the FUN_80159d78 icon resolve + +0xa4 clear. */
void FUN_80159f84(uint8_t *param_1)
{
  uint8_t *puVar1 = (uint8_t *)guest_at(0x801f47a0);
  uint8_t *puVar2 = (uint8_t *)guest_at(0x801f4770);
  uint8_t *puVar3;
  uint8_t bVar4, bVar5, bVar6;
  int32_t iVar1;
  int32_t iVar2;
  int32_t iVar3;

  for (iVar1 = 0; iVar1 < 0xb; iVar1 = iVar1 + 1) {
    puVar1[iVar1] = (uint8_t)iVar1;
    puVar2[iVar1] = (uint8_t)(iVar1 % 9);
  }
  *(int16_t *)(param_1 + 0xa4) = 0;
  FUN_80159d78(param_1);
  iVar1 = param_1[0x9c];
  if (iVar1 == 0) {
    puVar3 = puVar1;
    for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
      iVar3 = (int32_t)FUN_80129f78() % 9;
      bVar4 = puVar3[iVar2];
      puVar3[iVar2] = puVar3[iVar3];
      puVar3[iVar3] = bVar4;
    }
    /* disasm-verified 4-rotate @0x8015a09c-0xa0cc:
     * new[7]=old[9], new[8]=old[10], new[9]=old[7], new[10]=old[8]. */
    {
      uint8_t o7 = puVar1[7], o8 = puVar1[8];
      uint8_t o9 = puVar1[9], o10 = puVar1[10];
      puVar1[9] = o7;
      puVar1[10] = o8;
      puVar1[7] = o9;
      puVar1[8] = o10;
    }
    bVar4 = puVar2[4];
    puVar2[4] = puVar2[5];
    puVar2[5] = bVar4;
    puVar3 = puVar2;
    for (iVar2 = 0; iVar2 < 5; iVar2 = iVar2 + 1) {
      iVar3 = (int32_t)FUN_80129f78() % 5;
      bVar4 = puVar3[iVar2];
      puVar3[iVar2] = puVar3[iVar3];
      puVar3[iVar3] = bVar4;
    }
    return;
  }
  if (6 <= iVar1) {
    return;
  }
  puVar3 = puVar1;
  for (iVar2 = 0; iVar2 < 9; iVar2 = iVar2 + 1) {
    iVar3 = (int32_t)FUN_80129f78() % 9;
    bVar4 = puVar3[iVar2];
    puVar3[iVar2] = puVar3[iVar3];
    puVar3[iVar3] = bVar4;
  }
  puVar3 = puVar2;
  for (iVar2 = 0; iVar2 < 0xb; iVar2 = iVar2 + 1) {
    /* guest: v1 = rand; v0 = (v1 < 0) ? v1 + 7 : v1;
     * m = v1 - ((v0 >> 3) << 3)  (signed mod-8 via shifts) */
    iVar3 = (int32_t)FUN_80129f78();
    iVar1 = (iVar3 < 0) ? iVar3 + 7 : iVar3;
    iVar1 = iVar3 - ((iVar1 >> 3) << 3);
    puVar3[iVar2] = (uint8_t)iVar1;
  }
}

/* FUN_8015260c — memcard slot action dispatched by FUN_80153264.
 * param_1 = host ptr to save-manager state; param_2 = slot (0/1).
 * Marks the c0-block busy flag, applies the per-state slot-mask table
 * (jump table @0x80120998 indexed by param_1[0x9b]), resets the two
 * 0x1f80 scratch channel blocks via FUN_8016fc74, re-shuffles the
 * picker tables (FUN_80159f84) and pokes channel param_2 via
 * FUN_801279c8. */
void FUN_8015260c(uint8_t *param_1,int32_t param_2)
{
  uint8_t *s1 = (uint8_t *)guest_at(0x1f80e8);
  uint8_t *s2 = (uint8_t *)guest_at(0x1f801f0);
  uint8_t *q;
  int32_t iVar1;

  q = (uint8_t *)guest_at(*(uint32_t *)(param_1 + 0xc0));
  if ((*(uint16_t *)(param_1 + ((uint32_t)param_2 << 1) + 0x6c) & 0xf) == 0xf) {
    q[0x21] = 1;
  }
  else {
    q[0x21] = 0;
  }
  iVar1 = param_1[0x9b];
  if (iVar1 < 5) {
    switch (iVar1) {
    case 0:
      *(uint16_t *)(param_1 + ((uint32_t)param_2 << 1) + 0xb4) = 0x100;
      break;
    case 1:
    case 4:
      *(uint16_t *)(param_1 + ((uint32_t)param_2 << 1) + 0xb4) = 0x100;
      *(uint16_t *)(param_1 + ((uint32_t)(param_2 ^ 1) << 1) + 0xb4) = 0;
      break;
    case 2:
      *(uint16_t *)(param_1 + 0xb4) = 0x100;
      *(uint16_t *)(param_1 + 0xb6) = 0x100;
      break;
    case 3:
      *(uint16_t *)(param_1 + 0x92) = 0x400;
      *(uint16_t *)(param_1 + 0xb4) = 0;
      *(uint16_t *)(param_1 + 0xb6) = 0;
      break;
    }
  }
  s1[4] = 0;
  s2[4] = 0;
  q[0x1b] = 0;
  q[0x1c] = 2;
  q[0x19] = q[0x19] & 3;
  q[0x1a] = q[0x1a] & 3;
  FUN_80159f84(param_1);
  FUN_8016fc74(0x1f80e8,0,0);
  FUN_8016fc74(0x1f801f0,0,0);
  FUN_801279c8(0x1f80e8 + (uint32_t)param_2 * 264,0);
}

/* FUN_80156290 — sweep the 32-entry x6-byte channel table at guest
 * 0x801f3600: entries whose high flag nibble is 0x200 are cleared and
 * the live counter at 0x801f35f4 is decremented. */
void FUN_80156290(void)
{
  uint16_t *puVar1 = (uint16_t *)g_DAT_801f3600;
  int32_t iVar1 = 0;
  do {
    if ((*puVar1 & 0xf00) == 0x200) {
      *puVar1 = 0;
      DAT_801f35f4 = DAT_801f35f4 - 1;
    }
    iVar1 = iVar1 + 1;
    puVar1 = puVar1 + 3;
  } while (iVar1 < 0x20);
}

/* FUN_801562e0 — same sweep as FUN_80156290 but only entries whose
 * flag nibble is exactly 0x100 are cleared (guest branch chain:
 * skip 0x200, skip >=0x201, require 0x100). */
void FUN_801562e0(void)
{
  uint16_t *puVar1 = (uint16_t *)g_DAT_801f3600;
  int32_t iVar1 = 0;
  do {
    uint16_t uVar1 = *puVar1 & 0xf00;
    if ((uVar1 != 0x200) && (uVar1 < 0x201) && (uVar1 == 0x100)) {
      *puVar1 = 0;
      DAT_801f35f4 = DAT_801f35f4 - 1;
    }
    iVar1 = iVar1 + 1;
    puVar1 = puVar1 + 3;
  } while (iVar1 < 0x20);
}

/* FUN_801575bc — date/time dialog confirm path A. param_1 = packed
 * date value stored at 0x801bef60+0x12 (clamped modulo byte +0xc),
 * param_2 = optional FUN_801574b4 hook index. Computes the display
 * string offset table entry at +0x2c+idx*4 through FUN_801962f0 and
 * publishes the result at +0x20/+0x24. */
void FUN_801575bc(int32_t param_1,uint32_t param_2)
{
  uint8_t *s0 = g_DAT_801bef60;
  int32_t iVar1;
  uint8_t bVar2;
  const uint8_t *puVar3;

  *(int16_t *)(s0 + 0x12) = (int16_t)param_1;
  iVar1 = (int32_t)(int16_t)param_1;
  if (s0[0xc] < iVar1) {
    *(int16_t *)(s0 + 0x12) = (int16_t)(iVar1 % s0[0xc]);
  }
  if (*(int16_t *)(s0 + 0x12) == 0) {
    *(int16_t *)(s0 + 0x12) = (int16_t)s0[0xc];
  }
  if ((int32_t)param_2 >= 0) {
    FUN_801574b4((int32_t)param_2);
  }
  iVar1 = *(int16_t *)(s0 + 0x12);
  s0[0xd] = 0x10;
  bVar2 = s0[0xe];
  *(int16_t *)(s0 + 0x16) = 0;
  *(int16_t *)(s0 + 0x14) = 0;
  s0[0xf] = 0;
  puVar3 = s0 + 0x2c + iVar1 * 4;
  s0[0xe] = (uint8_t)((bVar2 & 0xf0) | 1);
  iVar1 = FUN_801962f0(puVar3);
  *(uint32_t *)(s0 + 0x24) = (uint32_t)iVar1;
  *(uint32_t *)(s0 + 0x20) = (uint32_t)iVar1;
  *(uint32_t *)(s0 + 0x28) = 0xffffffffu;
  *(uint32_t *)(s0 + 0x18) = 0;
  *(int16_t *)(s0 + 0x10) = 0x7f;
}

/* FUN_8015768c — date/time dialog confirm path B. param_1 = packed
 * date value (param_1 % 8 -> weekday nibble at +1); looks up the
 * date-string pointer in the table @0x801b5b94 (8-byte entries,
 * index = param_1/8), resolves it and the +0x12-indexed string at
 * guest 0x801b3310 via FUN_801962f0, waits on FUN_80195ffc(0xd),
 * then publishes like FUN_801575bc with flag bits |2. */
void FUN_8015768c(int32_t param_1,uint32_t param_2)
{
  uint8_t *s1 = g_DAT_801bef60;
  uint8_t *s3;
  int32_t iVar1;
  int32_t iVar2;
  int32_t iVar3;
  int32_t iVar4;
  uint8_t bVar4;

  iVar4 = param_1;
  if (param_1 < 0) {
    iVar4 = param_1 + 7;
  }
  iVar1 = iVar4 >> 3;
  iVar2 = param_1 - iVar1 * 8;
  s3 = (uint8_t *)guest_at(g_PTR_DAT_801b5b94[iVar1]);
  *(int16_t *)(s1 + 0x12) = (int16_t)param_1;
  FUN_801573ec();
  iVar3 = FUN_801962f0(s3);
  iVar3 = iVar3 + FUN_801962f0((const void *)guest_at(
      0x801b3310u + (uint32_t)(*(int16_t *)(s1 + 0x12)) * 4)) * 2;
  *(uint32_t *)(s1 + 0x28) = (uint32_t)iVar3;
  s1[0] = 1;
  s1[1] = (uint8_t)iVar2;
  do {
    iVar3 = FUN_80195ffc(0xd,(int)guest_of(s1),0);
  } while (iVar3 == 0);
  if ((int32_t)param_2 >= 0) {
    FUN_801574b4((int32_t)param_2);
  }
  bVar4 = s1[0xe];
  *(int16_t *)(s1 + 0x16) = 0;
  *(int16_t *)(s1 + 0x14) = 0;
  s1[0xf] = 0;
  s1[0xd] = 0x10;
  s1[0xe] = (uint8_t)((bVar4 & 0xf0) | 2);
  iVar3 = FUN_801962f0(s3);
  *(uint32_t *)(s1 + 0x24) = (uint32_t)iVar3;
  *(uint32_t *)(s1 + 0x20) = (uint32_t)iVar3;
  *(uint32_t *)(s1 + 0x18) = 0;
  *(int16_t *)(s1 + 0x10) = 0x7f;
}

/* FUN_80163520 — save-slot display-position helper. Gates on the
 * scratch flags at guest 0x1f8000dc ((v>>4)&3 != 0 and bit 0x40 set),
 * then param_2 selects the fixed-point divisor: 0 -> lh22*3 scaled
 * >>2, 1 -> lh22*7 scaled >>3 (both mod-adjusted toward zero), with
 * arg4 = ((int16)param_1[0x20] / 2) - 0x45 and arg6 = 4 handed to
 * FUN_8012e078(a0=+8 ptr, a1=+0xc ptr, a2=guest 0x80121e3c table).
 * Returns -0x63 (-99) on gate/selector failure. */
int FUN_80163520(uint8_t *param_1,uint32_t param_2)
{
  uint32_t uVar1 = SB32(0x1f8000dc);
  int32_t iVar1;
  int32_t iVar2;
  int32_t iVar3;

  if (((uVar1 >> 4) & 3) == 0) {
    return -0x63;
  }
  if ((uVar1 & 0x40) == 0) {
    return -0x63;
  }
  if (param_2 == 0) {
    iVar1 = *(int16_t *)(param_1 + 0x22) * 3;
    if (iVar1 < 0) {
      iVar1 = iVar1 + 3;
    }
    iVar2 = (int32_t)(int16_t)*(uint16_t *)(param_1 + 0x20);
    iVar3 = (int16_t)((iVar2 / 2) - 0x45);
    FUN_8012e078(*(uint32_t *)(param_1 + 8),*(uint32_t *)(param_1 + 0xc),
                (const void *)guest_at(0x80121e3c),iVar3,
                (int32_t)((uint32_t)iVar1 << 14) >> 16,4);
    return 0;
  }
  if (param_2 == 1) {
    iVar1 = *(int16_t *)(param_1 + 0x22) * 7;
    if (iVar1 < 0) {
      iVar1 = iVar1 + 7;
    }
    iVar2 = (int32_t)(int16_t)*(uint16_t *)(param_1 + 0x20);
    iVar3 = (int16_t)((iVar2 / 2) - 0x45);
    FUN_8012e078(*(uint32_t *)(param_1 + 8),*(uint32_t *)(param_1 + 0xc),
                (const void *)guest_at(0x80121e3c),iVar3,
                (int32_t)((uint32_t)iVar1 << 13) >> 16,4);
    return -0x63;   /* disasm 0x80163618: v0=-99 on all paths */
  }
  return -0x63;
}

/* FUN_801574b4 — memcard dialog flag @0x801bef6e bit4 setter.
 * p<0 -> read-only return; p==0 -> clear 0x10; p>0 -> set 0x10.
 * Disasm-verified @0x801574b4. */
int32_t FUN_801574b4(int32_t param_1)
{
  uint8_t *f = (uint8_t *)guest_at(0x801bef60);
  if (param_1 < 0) return f[0xe];
  f[0xe] = (param_1 == 0) ? (uint8_t)(f[0xe] & 0xef)
                        : (uint8_t)(f[0xe] | 0x10);
  return f[0xe];
}

/* FUN_80159a14 — memcard/block IO retry: poll FUN_80159ab4 up to 2000x;
 * on handle, toggle s1+8 index, FUN_8018ff10(h, *(s1+idx*4),
 * *(u32*)0x801f4758) then cd_dir_entry_probe(h); returns handle or -1.
 * Disasm 0x80159a14-0x80159aac. */
extern int32_t FUN_80159ab4(uint8_t *,void *);
extern void FUN_8018ff10(int32_t,uint32_t,uint32_t);
extern void cd_dir_entry_probe(int32_t);
int32_t FUN_80159a14(uint32_t param_1,void *param_2)
{
  uint8_t *s1 = (uint8_t *)guest_at(param_1);
  int32_t s0, s2 = 0x7d0;
  uint32_t idx;
  if (s1 == NULL) return -1;
  for (;;) {
    s0 = FUN_80159ab4(s1,param_2);
    if (s0 != 0) {
      idx = (*(uint32_t *)(s1 + 8) < 1) ? 1 : 0;
      *(uint32_t *)(s1 + 8) = idx;
      FUN_8018ff10(s0,*(uint32_t *)(s1 + idx * 4),
                   *(uint32_t *)guest_at(0x801f4758));
      cd_dir_entry_probe(s0);
      return s0;
    }
    if (--s2 == 0) return -1;
  }
}

/* FUN_80159ab4 — memcard entry compare/update: FUN_80199360 poll
 * (2000 tries), compares serial @+8, date u16s @+0x10/+0x12 against
 * globals @0x801b3f68/6c; on change rebuilds date fields on stack
 * record and stores scaled halves into s2 @+0x1c/1e/24/26/32.
 * Disasm 0x80159ab4-0x80159c1c. */
extern int32_t FUN_80199360(uint32_t *,uint32_t *);
int32_t FUN_80159ab4(uint8_t *param_1,void *param_2)
{
  uint32_t sp18 = 0, sp1c = 0, s0 = 0x7d0;
  uint8_t *s2 = param_1;
  uint8_t *s1 = (uint8_t *)param_2;
  uint8_t *a0;
  int32_t half;
  while (FUN_80199360(&sp18,&sp1c) != 0) {
    if (--s0 == 0) return 0;
  }
  a0 = guest_at(sp1c);
  if (a0 == NULL || s1 == NULL) return (int32_t)sp18;
  if (!(*(uint32_t *)(a0 + 8) < *(uint32_t *)(s1 + 8)))
    *(uint32_t *)guest_at(0x801f4740) = 1;
  if (*(uint16_t *)(a0 + 0x10) != *(uint32_t *)guest_at(0x801b3f68) ||
      *(uint16_t *)(a0 + 0x12) != *(uint32_t *)guest_at(0x801b3f6c)) {
    int32_t d;
    *(uint32_t *)guest_at(0x801b3f68) = *(uint16_t *)(a0 + 0x10);
    *(uint32_t *)guest_at(0x801b3f6c) = *(uint16_t *)(a0 + 0x12);
    d = *(uint32_t *)s1 != 0 ? *(int32_t *)(s1 + 0x14) : (int16_t)*(uint16_t *)(s1 + 0x14);
    d = (d * 3 + (((d * 3) >> 31) & 1)) >> 1;
    *(uint16_t *)(s2 + 0x24) = (uint16_t)d;
    *(uint16_t *)(s2 + 0x1c) = (uint16_t)d;
    {
      int32_t g = *(int32_t *)guest_at(0x801b3f68);
      d = *(uint32_t *)s1 != 0 ? (g * 3 + (((g * 3) >> 31) & 1)) >> 1
                               : (int16_t)*(uint32_t *)guest_at(0x801b3f68);
    }
    *(uint16_t *)(s2 + 0x26) = (uint16_t)d;
    *(uint16_t *)(s2 + 0x1e) = (uint16_t)d;
    *(uint16_t *)(s2 + 0x32) = (uint16_t)d;
    return (int32_t)sp18;
  }
  if (*(uint32_t *)s1 != 0) {
    int32_t d = *(int32_t *)guest_at(0x801b3f68);
    half = (d * 3 + (((d * 3) >> 31) & 1)) >> 1;
  } else {
    half = (int16_t)*(uint32_t *)guest_at(0x801b3f68);
  }
  *(uint16_t *)(s2 + 0x24) = (uint16_t)half;
  *(uint16_t *)(s2 + 0x1c) = (uint16_t)half;
  half = (int16_t)*(uint32_t *)guest_at(0x801b3f6c);
  *(uint16_t *)(s2 + 0x26) = (uint16_t)half;
  *(uint16_t *)(s2 + 0x1e) = (uint16_t)half;
  *(uint16_t *)(s2 + 0x32) = (uint16_t)half;
  return (int32_t)sp18;
}

/* FUN_80163b08 — first 0x10-stride rec where f8==0 or f8<a1;
 * -1 if none. a0 host ptr, a1 u32, a2 count. Disasm 0x80163b08-40. */
int32_t FUN_80163b08(uint8_t *a0,uint32_t a1,int32_t a2)
{
  int32_t i;
  if (a2 <= 0) return -1;
  for (i = 0; i < a2; i++, a0 += 0x10) {
    uint32_t f8 = *(uint32_t *)(a0 + 8);
    if (f8 == 0 || f8 < a1) return i;
  }
  return -1;
}

/* FUN_80163b84 — first 0x10-stride rec where !(a1 < s16(+0xe));
 * -1 if none. Disasm 0x80163b84-0x80163bb8. */
int32_t FUN_80163b84(uint8_t *a0,int32_t a1,int32_t a2)
{
  int32_t i;
  if (a2 <= 0) return -1;
  for (i = 0; i < a2; i++, a0 += 0x10) {
    if (!((int32_t)*(int16_t *)(a0 + 0xe) > a1)) return i;
  }
  return -1;
}

/* FUN_80163bc0 — copy 4 bytes a2 -> a0 + a1*16. Disasm verified. */
void FUN_80163bc0(uint8_t *a0,int32_t a1,const uint8_t *a2)
{
  uint8_t *d = a0 + (a1 << 4);
  d[0] = a2[0]; d[1] = a2[1]; d[2] = a2[2]; d[3] = a2[3];
}

/* FUN_80167ae8 — 32-byte compare: 0x801c11e0 vs *0x801b43f0;
 * returns 1 on first diff, 0 if equal. Disasm 0x80167ae8-30. */
int FUN_80167ae8(void)
{
  uint8_t *a = (uint8_t *)guest_at(0x801c11e0);
  uint8_t *b = (uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b43f0));
  uint32_t i;
  for (i = 0; i < 0x20; i++) {
    if (a[i] != b[i]) return 1;
  }
  return 0;
}

/* FUN_80156650 — FUN_80156484(a0) -> idx table @0x801f36d0 + a1*4,
 * then FUN_801565ec(ret, a2, *slot). Disasm 0x80156650-98. */
extern uint32_t FUN_80156484(uint32_t,uint32_t,uint32_t);
extern uint32_t FUN_801565ec(uint32_t,uint32_t,uint32_t);
void FUN_80156650(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t r = FUN_80156484(a0,a1,a2);
  uint32_t slot = *(uint32_t *)guest_at(0x801f36d0 + (a1 << 2));
  FUN_801565ec(r,a2,slot);
}

/* FUN_801573ec — memcard state init @0x801bef60: FUN_80156114(0),
 * poll FUN_80195ffc(9,0,0) till 0, then +0xd=0, +0xf=0xff,
 * +0x20 = +0x24. Disasm 0x801573ec-0x8015743c. */
extern void FUN_80156114(int32_t);
extern int32_t FUN_80195ffc(int32_t,int32_t,int32_t);
void FUN_801573ec(void)
{
  uint8_t *s0 = (uint8_t *)guest_at(0x801bef60);
  FUN_80156114(0);
  while (FUN_80195ffc(9,0,0) != 0) { }
  if (s0 == NULL) return;
  s0[0xd] = 0;
  s0[0xf] = 0xff;
  *(uint32_t *)(s0 + 0x20) = *(uint32_t *)(s0 + 0x24);
}

/* FUN_80157458 — memcard mode nibble: a0<0 -> return s0[0xe]&0xff;
 * else FUN_801573ec() then s0[0xe] = (s0[0xe]&0xf0)|a0, ret val.
 * Disasm 0x80157458-0x801574ac. */
uint32_t FUN_80157458(int32_t a0)
{
  uint8_t *s0 = (uint8_t *)guest_at(0x801bef60);
  uint32_t v0;
  if (s0 == NULL) return 0;
  if (a0 < 0) return s0[0xe] & 0xff;
  FUN_801573ec();
  v0 = (s0[0xe] & 0xf0) | (uint32_t)a0;
  s0[0xe] = (uint8_t)v0;
  return v0 & 0xff;
}

/* FUN_80155edc — gauge/se value select: FUN_80156048(0,a0),
 * FUN_80156ef4(0x405,0,4), a1 = table[a0<8] {0,4fff,0,0,4fff,3fff,
 * 5fff,3fff}, blk_1f26c0_init_c(2,a1). Disasm 0x80155edc-0x80155f5c. */
extern void FUN_80156ef4(int32_t,int32_t,int32_t);
extern void blk_1f26c0_init_c(int32_t,uint32_t);
void FUN_80155edc(uint32_t a0)
{
  static const uint16_t tab[8] = {0,0x4fff,0,0,0x4fff,0x3fff,0x5fff,0x3fff};
  uint32_t a1 = 0;
  FUN_80156048(0,(short)a0);
  FUN_80156ef4(0x405,0,4);
  if (a0 < 8) a1 = tab[a0];
  blk_1f26c0_init_c(2,a1);
}

/* FUN_8016325c — mode dispatch on *(u32*)a1: 0->632e4, 1->63314,
 * 2->63320, else break. Disasm 0x8016325c-0x801632dc. */
extern uint32_t FUN_801632e4(uint32_t,uint32_t);
/* FUN_80163314 — guest `break 1,7; jr ra`: debug breakpoint stub.
 * Faithful port traps instead of silently returning. */
extern void FUN_80163314(uint32_t,uint32_t);

extern uint32_t FUN_80163320(uint32_t,uint32_t);
void FUN_8016325c(uint32_t a0,uint32_t a1)
{
  uint32_t v1 = *(uint32_t *)guest_at(a1);
  (void)a0;
  if (v1 == 1) { FUN_80163314(a0,a1); return; }
  if (v1 < 2) {
    if (v1 == 0) { FUN_801632e4(a0,a1); return; }
  } else if (v1 == 2) { FUN_80163320(a0,a1); return; }
}

/* FUN_8019da44 — 24-slot status byte fill: for i<24, rec =
 * *0x801bdeb0 + i*16, bit i of *0x801bde3c selects mapping of
 * lhu(rec+0xc): bit? (f?1:3) : (f?2:0). Disasm verified. */
void FUN_8019da44(uint8_t *a0)
{
  uint8_t *base = (uint8_t *)guest_at(*(uint32_t *)guest_at(0x801bdeb0));
  uint32_t mask = *(uint32_t *)guest_at(0x801bde3c);
  int i;
  for (i = 0; i < 24; i++) {
    uint8_t *rec = base + i * 16;
    uint32_t bit = mask & (1u << i);
    uint32_t f = *(uint16_t *)(rec + 0xc);
    a0[i] = bit ? (f ? 1 : 3) : (f ? 2 : 0);
  }
}

/* FUN_80156c60 — memcard action dispatch (JT 0x80120b74):
 * 0 -> if *0x801f36c2: FUN_801575bc(a1,1); 1 -> FUN_80156114(0x30);
 * 2 -> FUN_80156114(0x7f); 3/4 -> FUN_801573ec(); 5 -> FUN_80156114(a1).
 * Always returns 1. Disasm verified. */
int32_t FUN_80156c60(uint32_t a0,uint32_t a1)
{
  if (a0 >= 6) return 1;
  switch (a0) {
  case 0:
    if (*(uint16_t *)guest_at(0x801f36c2) != 0) FUN_801575bc(a1,1);
    break;
  case 1: FUN_80156114(0x30); break;
  case 2: FUN_80156114(0x7f); break;
  case 3: case 4: FUN_801573ec(); break;
  case 5: FUN_80156114(a1); break;
  }
  return 1;
}

/* FUN_80156cf4 — same dispatch via FUN_8015768c (JT 0x80120b8c). */
int32_t FUN_80156cf4(uint32_t a0,uint32_t a1)
{
  if (a0 >= 6) return 1;
  switch (a0) {
  case 0:
    if (*(uint16_t *)guest_at(0x801f36c2) != 0) FUN_8015768c(a1,1);
    break;
  case 1: FUN_80156114(0x30); break;
  case 2: FUN_80156114(0x7f); break;
  case 3: case 4: FUN_801573ec(); break;
  case 5: FUN_80156114(a1); break;
  }
  return 1;
}

/* FUN_80166398 — fighter spawn tail: 2b49c(0), 525c4(s1),
 * 5a584(scratch pair, s1, s1[0xc0]), 51328(pair, s1), 6fbfc(),
 * if !a1 record_copy_1a(s8(s1+0x98)); s1[0xa0]=0xff.
 * Disasm 0x80166398-0x80166424. */
extern void FUN_8015a584(uint32_t,uint32_t,uint8_t *,uint32_t);
extern void scratchpad_pair_init(void);
extern void FUN_8012b49c(undefined1);
extern void FUN_801525c4(intptr_t);
extern void FUN_80151328(uint8_t *,uint8_t *,uint32_t);
extern uint8_t *g_DAT_1f8000e8,*g_DAT_1f8001f0;
extern uint32_t guest_of(const void *);
extern void record_copy_1a(int);
void FUN_80166398(uint8_t *s1,uint32_t s0)
{
  FUN_8012b49c(0);
  FUN_801525c4((intptr_t)(void *)s1);
  FUN_8015a584(0x1f8000e8,0x1f8001f0,(uint8_t *)s1,*(uint32_t *)(s1 + 0xc0));
  FUN_80151328(g_DAT_1f8000e8,g_DAT_1f8001f0,guest_of(s1));
  scratchpad_pair_init();
  if (s0 == 0) record_copy_1a((int8_t)s1[0x98]);
  *(uint16_t *)(s1 + 0xa0) = 0xff;
}

/* FUN_801565ec — CD file read seq: 9d4d0(0), 9d470(a1),
 * 9d410(a0,a2), poll 9d500(0) till nonzero; returns a2.
 * Disasm 0x801565ec-0x80156648. */
extern void FUN_8019d4d0(int32_t);
extern void FUN_8019d470(uint32_t);
extern void FUN_8019d410(uint32_t,uint32_t);
extern int32_t FUN_8019d500(int32_t);
uint32_t FUN_801565ec(uint32_t a0,uint32_t a1,uint32_t a2)
{
  FUN_8019d4d0(0);
  FUN_8019d470(a1);
  FUN_8019d410(a0,a2);
  while (FUN_8019d500(0) == 0) { }
  return a2;
}

/* FUN_8015a584 — fighter link init: a2[0x94]=-1, a3 guest ptr,
 * +0xde=0x4b on both scratch blocks, +0x54 = (a3+0x19/0x1a)&3,
 * +0x86=0x10, +0x96=3, 5ab88(0,s0) + 5ab88(1,s0).
 * Disasm 0x8015a584-0x8015a610. */
extern void FUN_8015ab88(uint32_t,uint32_t);
void FUN_8015a584(uint32_t a0,uint32_t a1,uint8_t *a2,uint32_t a3)
{
  uint8_t *t1 = (uint8_t *)guest_at(a0);
  uint8_t *t0 = (uint8_t *)guest_at(a1);
  uint8_t *g = (uint8_t *)guest_at(a3);
  uint32_t s0 = 0x801b3f78;
  if (a2 == NULL || t1 == NULL || t0 == NULL || g == NULL) return;
  *(uint16_t *)(a2 + 0x94) = 0xffff;
  t0[0xde] = 0x4b;
  t1[0xde] = 0x4b;
  *(uint32_t *)(t1 + 0x54) = g[0x19] & 3;
  *(uint16_t *)(t0 + 0x86) = 0x10;
  *(uint32_t *)(t0 + 0x54) = g[0x1a] & 3;
  *(uint16_t *)(t1 + 0x86) = 0x10;
  *(uint8_t *)guest_at(0x801f04c8) = 7;
  *(uint16_t *)(a2 + 0x96) = 3;
  FUN_8015ab88(0,s0);
  FUN_8015ab88(1,s0);
}

/* FUN_801632e4 — s0=*(u32*)(a1+8); FUN_8018e490(a0,a1+0xc,s0);
 * returns s0. Disasm 0x801632e4-0x8016330c. */
extern uint32_t FUN_8018e490(uint32_t,uint32_t,uint32_t);
uint32_t FUN_801632e4(uint32_t a0,uint32_t a1)
{
  uint8_t *b = (uint8_t *)guest_at(a1);
  uint32_t s0;
  if (b == NULL) return 0;
  s0 = *(uint32_t *)(b + 8);
  /* FUN_8018e490 takes guest addresses; b+0xc is host, so pass the
   * guest address a1+0xc directly (b = guest_at(a1)). */
  FUN_8018e490(a0,a1 + 0xc,s0);
  return s0;
}

extern int32_t FUN_80163b48(uint8_t *,uint32_t,int32_t);
extern int32_t FUN_80168424(uint32_t);
extern int32_t FUN_801684c8(uint32_t);
extern int32_t FUN_8016850c(uint32_t,const void *,const void *,const void *,uint32_t,uint32_t);

/* FUN_80163d7c — name fixup: sanitize first 3 bytes (0x3c/0x7f -> 0x20),
 * then scan string-pointer table @0x801b4250 backwards (a2 from 0x15,
 * t0 -=4 per miss) for a 3-char prefix match; on match write "HUD",
 * return 1; exhausted -> 0. Disasm 0x80163d7c-0x80163e34. */
int FUN_80163d7c(char *t1)
{
  int i, a1, a2;
  uint32_t t0 = 0x801b4250;
  for (i = 0; i < 3; i++)
    if ((uint8_t)t1[i] == 0x3c || (uint8_t)t1[i] == 0x7f) t1[i] = 0x20;
  a2 = 0x15;
  for (;;) {
    uint8_t *s = (uint8_t *)guest_at(*(uint32_t *)guest_at(t0));
    for (a1 = 2; a1 >= 0; a1--)
      if (s[a1] != (uint8_t)t1[a1]) break;
    a2--;
    if (a1 < 0) {
      t1[0] = 'H'; t1[1] = 'U'; t1[2] = 'D';
      return 1;
    }
    if (a2 < 0) return 0;
    t0 -= 4;
  }
}

/* FUN_80163bf4 — name-record insert: tab = 16-byte records, idx a1,
 * count = stack arg. idx out of range -> fill tail u16s from src +0xe0/+0xe6.
 * Else shift records [idx..count-2] up one and fill slot: name[0..3],
 * src[3]->+4, u32 src+0xf0->+8, u16 src+0xe0->+0xc, u16 src+0xe6->+0xe.
 * Disasm 0x80163bf4-0x80163cb4. */
void FUN_80163bf4(uint8_t *a0,int32_t a1,uint8_t *a2,const void *a3_,int32_t count)
{
  uint8_t *t0 = a0 + a1 * 16;
  const uint8_t *a3 = (const uint8_t *)a3_;
  int32_t v1;
  if (a1 >= count || a1 < 0) {
    *(uint16_t *)(t0 + 0xc) = *(uint16_t *)(a2 + 0xe0);
    *(uint16_t *)(t0 + 0xe) = *(uint16_t *)(a2 + 0xe6);
    return;
  }
  for (v1 = count - 2; v1 >= a1; v1--)
    memcpy(a0 + (v1 + 1) * 16,a0 + v1 * 16,16);
  t0[0] = a3[0]; t0[1] = a3[1]; t0[2] = a3[2]; t0[3] = a3[3];
  t0[4] = a2[3];
  *(uint32_t *)(t0 + 8) = *(uint32_t *)(a2 + 0xf0);
  *(uint16_t *)(t0 + 0xc) = *(uint16_t *)(a2 + 0xe0);
  *(uint16_t *)(t0 + 0xe) = *(uint16_t *)(a2 + 0xe6);
}

/* FUN_80163cbc — name-table update gate: skip when byte @0x1f80009c
 * in [2,6); else FUN_80163b48 lookup on table @0x801c1304 keyed by
 * src+0xe0, then FUN_80163bf4 insert (str arg or default @0x80121fa4).
 * Disasm 0x80163cbc-0x80163d78. */
void FUN_80163cbc(uint8_t *s1,const void *a1_)
{
  int8_t v1 = *(int8_t *)guest_at(0x1f80009c);
  uint8_t *s0 = (uint8_t *)guest_at(0x801c1304);
  const uint8_t *s2 = (const uint8_t *)a1_;
  int32_t r;
  if (v1 >= 2 && v1 < 6) return;
  r = FUN_80163b48(s0,*(uint16_t *)(s1 + 0xe0),0xa);
  if (*(uint8_t *)s2 != 0) {
    if (r >= 0) FUN_80163bf4(s0,r,s1,s2,0xa);
  } else {
    if (r >= 0) FUN_80163bf4(s0,r,s1,(uint8_t *)guest_at(0x80121fa4),0xa);
  }
}

/* FUN_80167b34 — memcard op dispatch: s0 = -(a0!=0), s1 = s0&0x10,
 * r = FUN_80168424(s1); r==3||4 -> slot @0x801b43e8+(s0&4) = 3;
 * r==0||4 -> return FUN_801684c8(s1); else return r.
 * Disasm 0x80167b34-0x80167bd4. */
int FUN_80167b34(uint32_t a0)
{
  int32_t s0 = -(a0 != 0);
  uint32_t s1 = (uint32_t)s0 & 0x10;
  int32_t r = FUN_80168424(s1);
  if (r == 3 || r == 4)
    *(uint32_t *)guest_at(0x801b43e8 + ((uint32_t)s0 & 4)) = 3;
  if (r == 0 || r == 4) return FUN_801684c8(s1);
  return r;
}

/* FUN_80167bd8 — memcard op dispatch variant: r==3||0 -> slot select
 * @0x801b43e8+((a0!=0)?4:0), FUN_8016850c probe; rc!=0 -> rc,
 * out2[0]==0 -> 0, else (*slot==3 ? 0xf : 0). r==4 -> slot=3 ret 4.
 * Disasm 0x80167bd8-0x80167cdc. */
int FUN_80167bd8(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t s4,uint32_t s5,uint32_t s6)
{
  int32_t s1 = -(a0 != 0);
  uint32_t fl = (uint32_t)s1 & 0x10;
  int32_t r = FUN_80168424(fl);
  if (r == 3 || r == 0) {
    uint32_t *slot = (uint32_t *)guest_at(0x801b43e8 + ((a0 != 0) ? 4 : 0));
    uint32_t in[3] = { a1,a2,a3 };
    uint32_t out1[8] = {0}, out2[8] = {0};
    uint32_t st1 = 0, st2 = 1;
    int32_t rc = FUN_8016850c(fl,in,out1,out2,st1,st2);
    if (rc != 0) return rc;
    if (out2[0] == 0) return 0;
    return *slot == 3 ? 0xf : 0;
  }
  if (r == 4) {
    *(uint32_t *)guest_at(0x801b43e8 + ((uint32_t)s1 & 4)) = 3;
    return 4;
  }
  if (r >= 1 && r <= 2) return r;
  return r;
}

/* FUN_80163850/63898/638a0 — disabled save-slot helpers: guest is a
 * bare `jr ra; move v0,zero` (return 0), args ignored.
 * Disasm 0x80163850-0x801638a4. */
int32_t FUN_80163850(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_80163898(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_801638a0(uint32_t a0) { (void)a0; return 0; }

/* FUN_80163b48 — sorted-table lookup: scan a2 16-byte records for first
 * with key a1 < sh(rec+0xc); return index or -1.
 * Disasm 0x80163b48-0x80163b7c. */
int32_t FUN_80163b48(uint8_t *a0,uint32_t a1,int32_t a2)
{
  int32_t i;
  if (a2 <= 0) return -1;
  for (i = 0; i < a2; i++, a0 += 0x10)
    if ((int32_t)a1 < (int32_t)(int16_t)*(int16_t *)(a0 + 0xc)) return i;
  return -1;
}

/* FUN_8015ab88 — copy 8 u16 from a1 -> 0x801f4780 + a0*16.
 * Disasm 0x8015ab88-0x8015abbc. */
void FUN_8015ab88(uint32_t a0,uint32_t a1)
{
  uint16_t *d = (uint16_t *)guest_at(0x801f4780 + a0 * 16);
  const uint16_t *s = (const uint16_t *)guest_at(a1);
  int i;
  for (i = 0; i < 8; i++) d[i] = s[i];
}

/* FUN_80156220 — scan 0x20 six-byte records @0x801f3600: when
 * (h & 0xf00) == 0x400 clear the u16 and decrement @0x801f35f4.
 * (Guest tests 0x200/<0x201/0x300/!=0x400 all collapse to ==0x400.)
 * Disasm 0x80156220-0x80156288. */
void FUN_80156220(void)
{
  int i;
  for (i = 0; i < 0x20; i++) {
    uint16_t *r = (uint16_t *)guest_at(0x801f3600 + i * 6);
    if ((*r & 0xf00) == 0x400) {
      *(uint32_t *)guest_at(0x801f35f4) -= 1;
      *r = 0;
    }
  }
}

/* FUN_80156344 — clear flag bytes over records @0x801f24d0 stride 0x10
 * for idx in [a0,a1): rec[0]!=0 -> rec[0]=0, irqmask|=rec[8], rec[1]=0.
 * Then FUN_8019d220(0,mask), @0x801f2650=mask, +4=3, +8/+0xa=0,
 * FUN_8019dad0(@0x801f2650). Disasm 0x80156344-0x80156400. */
extern void FUN_8018e300(void);
extern void FUN_8018e310(void);
extern void FUN_8019d220(int32_t,uint32_t);
extern void FUN_8019dad0(uint32_t);
void FUN_80156344(uint32_t a0,uint32_t a1)
{
  uint32_t s1 = 0, i;
  FUN_8018e300();
  for (i = a0; i < a1; i++) {
    uint8_t *r = (uint8_t *)guest_at(0x801f24d0 + i * 0x10);
    if (r[0] != 0) {
      r[0] = 0;
      s1 |= *(uint32_t *)(r + 8);
      r[1] = 0;
    }
  }
  FUN_8019d220(0,s1);
  *(uint32_t *)guest_at(0x801f2650) = s1;
  *(uint32_t *)guest_at(0x801f2654) = 3;
  *(uint16_t *)guest_at(0x801f265a) = 0;
  FUN_8019dad0(0x801f2650);
  *(uint16_t *)guest_at(0x801f2658) = 0;
  FUN_8018e310();
}

/* FUN_80156484 — fighter record builder (a0=guest rec, a1=slot idx,
 * a2=guest base). FUN_80156220(); FUN_80156344(6,0x18); blk_1f26c0_init().
 * Writes idx table @0x801f36d0, 64-entry ptr table @0x801f26e0, and
 * t2<=0x40 packed 8-byte records @0x801f2be0+idx*0x200. Returns
 * guest addr rec + ((u16@rec+0x12)<<9)+0xa20. Disasm 0x80156484-0x801565e8. */
extern void blk_1f26c0_init(void);
uint32_t FUN_80156484(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint32_t s1 = a1, acc = a1, s3 = a2;
  uint32_t t3, t2, base, i;
  uint8_t *p, *t1, *a2p;
  if (s0 == NULL) return 0;
  FUN_80156220();
  FUN_80156344(6,0x18);
  blk_1f26c0_init();
  t3 = ((uint32_t)*(uint16_t *)(s0 + 0x12) << 9) + 0xa20;
  *(uint32_t *)guest_at(0x801f36d0 + (s1 << 2)) =
      *(uint32_t *)(s0 + 0xc) - t3;
  p = s0 + (((uint32_t)*(uint16_t *)(s0 + 0x12)) << 9) + 0x820;
  for (i = 0; i < 0x40; i++) {
    *(uint32_t *)guest_at(0x801f26e0 + ((s1 << 6) + i) * 4) = s3 + acc;
    acc += (uint32_t)*(uint16_t *)p * 8;
    p += 2;
  }
  t2 = *(uint16_t *)(s0 + 0x14);
  if (t2 >= 0x41) t2 = 0x40;
  t1 = (uint8_t *)guest_at(0x801f2be0 + (s1 << 9));
  a2p = s0 + 0x832;
  base = s1 << 6;
  for (i = 0; i < t2; i++) {
    int32_t d = (int32_t)a2p[-0xe] - (int32_t)a2p[-0xc];
    *(uint16_t *)(t1 + 2) = (uint16_t)(d * 0x100 + a2p[-0xd]);
    *(uint16_t *)(t1 + 0) = (uint16_t)(*(uint16_t *)(a2p + 4) + base);
    *(uint16_t *)(t1 + 4) = *(uint16_t *)(a2p - 2);
    *(uint16_t *)(t1 + 6) = *(uint16_t *)(a2p + 0);
    a2p += 0x20;
    t1 += 8;
  }
  return guest_of(s0 + t3);
}

/* FUN_80163320 — LZSS stream decoder. src[8..11] returned verbatim;
 * bitstream at src+0xc, 8 bits/byte refill when window hits 0x8000.
 * bit=1 literal; bit=0 back-ref: short form (bits 3-4 len-2, offset
 * byte -256..-1) or long form ((b0<<8|b1)>>3-0x2000 offset, len=(b1&7)+2
 * or count-byte+1; count byte 0 terminates -> return src[8]).
 * Disasm 0x80163320-0x80163478. */
uint32_t FUN_80163320(uint32_t dst,uint32_t src)
{
  uint8_t *sd = (uint8_t *)guest_at(src);
  uint32_t t1 = *(const uint32_t *)(sd + 8);
  const uint8_t *ip = sd + 0xc;
  uint8_t *op = (uint8_t *)guest_at(dst);
  uint32_t a2 = 0x8000;
  for (;;) {
    uint32_t bit, len;
    const uint8_t *a3;
    if ((a2 & 0xffff) == 0x8000) {
      uint32_t b = *ip++;
      a2 = (b << 8) | 0x80;
    }
    bit = a2 & 0x8000;
    a2 <<= 1;
    if (bit) {
      *op++ = *ip++;
      continue;
    }
    if ((a2 & 0xffff) == 0x8000) {
      uint32_t b = *ip++;
      a2 = (b << 8) | 0x80;
    }
    bit = a2 & 0x8000;
    a2 <<= 1;
    if (bit) {
      /* long back-ref: 13-bit offset, len in low 3 bits or count byte */
      uint32_t v0 = ((uint32_t)ip[0] << 8) | ip[1];
      uint32_t b1 = ip[1];
      int32_t off = (int32_t)(v0 >> 3) - 0x2000;
      ip += 2;
      len = b1 & 7;
      if (len != 0) {
        len += 2;
      } else {
        len = *ip++;
        if (len == 0) return t1;
        len += 1;
      }
      a3 = op + off;
    } else {
      /* short back-ref: 2-bit len code + offset byte */
      uint32_t l = 0, b;
      if ((a2 & 0xffff) == 0x8000) {
        uint32_t bb = *ip++;
        a2 = (bb << 8) | 0x80;
      }
      bit = a2 & 0x8000;
      a2 <<= 1;
      if (bit) l = 2;
      if ((a2 & 0xffff) == 0x8000) {
        uint32_t bb = *ip++;
        a2 = (bb << 8) | 0x80;
      }
      bit = a2 & 0x8000;
      a2 <<= 1;
      if (bit) l += 1;
      len = l + 2;
      b = *ip++;
      a3 = op + ((int32_t)b - 0x100);
    }
    while (len != 0) {
      *op++ = *a3++;
      len--;
    }
  }
}

void FUN_80163314(uint32_t a0,uint32_t a1)
{
  (void)a0; (void)a1;
  __builtin_trap();
}

/* ================= appended ports (psx_payload disasm) ================= */

/* callee externs needed by the appended ports */
extern void FUN_8012f024(uint32_t,uint32_t);
extern void FUN_8012a030(uint32_t);
extern int reset_sprite_ot_block(uint32_t);
extern void task_table_init(uint32_t);
extern void record_copy_1a(int);
extern void FUN_801358e8(int32_t,int32_t);
extern void FUN_8012e97c(int32_t,uint32_t,uint32_t);
extern int clear_f4ee8_u16s(uint32_t);
extern int reset_combat_slots(void);
extern void clear_f0300_records(void);
extern void FUN_801566a0(uint32_t,int32_t);
extern void FUN_801566d4(uint32_t,int32_t);
extern void FUN_80163154(uint32_t,uint32_t,int32_t);
extern void FUN_8019a460(uint32_t);
extern uint32_t br2_action_enter(void *battle, uint8_t *ctrl, uint32_t sel);
/* strong FUN_80175fec: forwards to br2_action_enter (action.c).
   Overrides the weak stub in bind.c. */
int FUN_80175fec(void *battle, uint8_t *ctrl, uint32_t sel)
{
  return (int)br2_action_enter(battle,ctrl,sel);
}
extern void FUN_80172a34(int32_t);
extern void FUN_8016d478(uint32_t);
extern int FUN_80153484(uint32_t,uint32_t);
extern int FUN_8015e87c();
extern int FUN_8015e88c();
extern int FUN_8015e8a4(int32_t);
extern int FUN_80170d38(uint32_t,int32_t,uint8_t *,uint8_t *);
extern int FUN_80170fe0(uint32_t,int32_t);
extern int FUN_80171890(uint32_t,uint8_t *,int32_t);
extern void FUN_80195290(int32_t,int32_t);
extern void FUN_801952b0(int32_t);
extern int FUN_801685b4(uint32_t,const void *,uint32_t);
extern int FUN_80168838(uint32_t,const void *,void *,uint32_t,uint32_t);
extern int FUN_801688f0(uint32_t,const void *,void *,uint32_t,uint32_t);
extern int FUN_8016831c(uint32_t,const void *,void *,uint32_t,uint32_t);
/* pointer args may arrive as guest addrs or host ptrs; normalise */
static void *mem_ptr(uint32_t v)
{
  if (v >= 0x1f800000 && v < 0x80300000) return guest_at(v);
  return (void *)(uintptr_t)v;
}

/* batch_mass17 forward decls (defs appended at end of file) */
extern int FUN_8019fb9c(uint32_t,const void *,uint32_t);
extern int FUN_8019f35c(uint32_t,const void *,void *,uint32_t,uint32_t);
extern int FUN_8019f5b4(uint32_t,const void *,void *,uint32_t,uint32_t);
extern void FUN_801aa96c(uint32_t);
extern void FUN_801a1a2c(void);
extern void FUN_801a7444(uint32_t);
extern void FUN_801aa0e4(void *,int32_t);
extern void FUN_801aa168(void *,int32_t);
extern void FUN_801aa1e4(void *,int32_t,int32_t,int32_t);
extern void FUN_801aa2c8(void *,int32_t);
extern void FUN_801aa320(void *,int32_t);
extern void FUN_801aa374(void *,int32_t,int32_t,int32_t);
extern uint32_t FUN_801b1140(int32_t,int32_t,int32_t,uint32_t);
extern int32_t FUN_801a1f74(uint8_t *,int32_t);
extern void FUN_801a01bc(uint32_t,void *);
extern void FUN_801a0530(void *,uint32_t);
extern void FUN_801a0a10(uint32_t,uint32_t);
extern int32_t FUN_801a0220(uint32_t,uint32_t,uint32_t);
extern void FUN_801a0250(int32_t);
extern uint32_t FUN_8019fa6c(uint32_t);
extern int32_t FUN_801a4230(void);
extern int FUN_801a9644(int,undefined2,undefined4,undefined4,short,undefined2,undefined4,undefined4);
extern int *FUN_801a4414(int,int,int,int);
extern int FUN_8018cfdc(int32_t,uint32_t);
extern int FUN_8018c31c(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t FUN_8018c824();
extern void FUN_8018c1f4(uint32_t,uint32_t,uint32_t,uint32_t);
extern void FUN_8018cc6c(uint32_t,int16_t,int16_t,int16_t,int16_t,uint32_t,int32_t);
extern void FUN_8018c71c(uint32_t,uint32_t,uint32_t,uint32_t,int32_t,int32_t);
extern void FUN_8018bfcc(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void FUN_80136ddc(void);
extern int FUN_801274fc(uint32_t,uint32_t,uint32_t);
extern int FUN_801274d8(uint32_t);
extern int FUN_8018bf40(int32_t,int32_t,int32_t,int32_t);

static uint32_t g32(uint32_t a)
{
  uint32_t *p = (uint32_t *)guest_at(a);
  return p ? *p : 0;
}

static uint16_t g16(uint32_t a)
{
  uint16_t *p = (uint16_t *)guest_at(a);
  return p ? *p : 0;
}

static int16_t gs16(uint32_t a)
{
  int16_t *p = (int16_t *)guest_at(a);
  return p ? *p : 0;
}

static uint8_t g8(uint32_t a)
{
  uint8_t *p = (uint8_t *)guest_at(a);
  return p ? *p : 0;
}

/* ---- FUN_801568fc @0x801568fc (216 insns) ---- */
int FUN_801568fc(uint32_t a0,int32_t a1)
{
  uint8_t *rec;
  uint8_t *slotp;
  uint8_t *rb = NULL;
  uint8_t key;
  int i;

  if (a0 >= 6) return 1;
  switch (a0) {
  case 3:
    FUN_80156220();
    FUN_80156344(6,0x18);
    return 1;
  case 5: {
    uint8_t *b = (uint8_t *)guest_at(g32(0x1f8000c0));
    int16_t x = b ? (int16_t)b[0xe] : 0;
    int16_t y = b ? (int16_t)b[0xf] : 0;
    FUN_80156048(x,y);
    return 1;
  }
  case 1:
  case 2:
  case 4:
    return 1;
  default:
    break; /* case 0 */
  }

  if (g16(0x801f36c2) == 0) return 1;
  {
    uint32_t *ptab = (uint32_t *)guest_at(0x801f35e0);
    int32_t idx = a1 >> 6;               /* sra */
    uint32_t base = ptab ? ptab[idx] : 0;
    rb = (uint8_t *)guest_at(base + (uint32_t)(a1 & 0x3f) * 6);
    uint16_t t;
    if (!rb) return 1;
    t = *(uint16_t *)(rb + 2);
    if (t >= 1 && t <= 8) {
      slotp = (uint8_t *)guest_at(0x801f24d0 + (uint32_t)(t - 1) * 0x10);
    } else if (t == 0x80 || t == 0x81) {
      uint32_t b0;
      if (t == 0x80 && g16(0x801f36c2) == 3) return 1;
      b0 = (t == 0x80) ? 0x801f2550 : 0x801f2510;
      int lo = (t == 0x80) ? 8 : 4;
      int hi = (t == 0x80) ? 0x18 : 8;
      uint8_t *p = (uint8_t *)guest_at(b0);
      key = rb[0];
      for (i = lo; i < hi; i++) {
        if (p[0] == 0) break;
        p += 0x10;
      }
      if (i == hi) {
        p = (uint8_t *)guest_at(b0);
        for (i = lo; i < hi; i++) {
          if (!(key < p[1])) break;
          p += 0x10;
        }
        if (i == hi) return 0;
      }
      slotp = p;
    } else {
      return 1;
    }
    if (!slotp) return 1;
  }
  rec = rb;
  key = rec[0];
  if ((uint8_t)key < slotp[1]) return 1;
  if (g16(0x801f36c2) == 2) {
    if (key != 0x60) return 1;
  }
  slotp[0] = 0x80;
  slotp[1] = key;
  if (g16(0x801f36c0) == 0) {
    uint16_t w = g16(0x801b33bc + ((uint32_t)(rec[1] >> 4) << 1));
    int32_t prod1 = (int32_t)*(uint16_t *)(rec + 4) * (int32_t)(w >> 8);
    int32_t prod2 = (int32_t)*(uint16_t *)(rec + 4) * (int32_t)(w & 0xff);
    *(uint16_t *)(slotp + 2) = (uint16_t)(prod1 >> 7);
    *(uint16_t *)(slotp + 4) = (uint16_t)(prod2 >> 7);
  } else {
    uint16_t x = *(uint16_t *)(rec + 4);
    *(uint16_t *)(slotp + 4) = x;
    *(uint16_t *)(slotp + 2) = x;
  }
  *(uint16_t *)(slotp + 6) = (uint16_t)a1;
  return 1;
}

/* ---- FUN_80156d88 @0x80156d88 (1 insn: jr ra; li v0,1) ---- */
int FUN_80156d88(void)
{
  return 1;
}

/* ---- FUN_8015787c @0x8015787c (96 insns) ---- */
void FUN_8015787c(uint32_t a0,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint32_t v0g;
  int32_t a2;
  int32_t code;

  v0g = *(uint32_t *)(s0 + 0xc0);
  {
    uint8_t *v0 = (uint8_t *)guest_at(v0g);
    a2 = v0 ? (int8_t)v0[0xd] : 0;
  }
  if (a2 == 0) {
    uint16_t b4 = *(uint16_t *)(s0 + 0xb4);
    uint16_t b6 = *(uint16_t *)(s0 + 0xb6);
    int c = ((b6 & 0x1000) == 0) || ((b4 & 0x1000) != 0);
    uint8_t *tab = (uint8_t *)guest_at(0x1f8000eb + (uint32_t)c * 264);
    uint32_t idx = tab ? tab[0] : 0;
    static const int8_t map[11] = {0,1,2,5,3,6,4,7,8,10,9};
    if (idx < 11) code = map[idx]; else code = 0;
    code += 0x14;
  } else if (a2 == 1) {
    int32_t v1 = (int32_t)(int8_t)s0[0x98];
    static const int8_t map[13] = {0,1,2,3,4,5,6,7,8,0,1,2,3};
    if ((uint32_t)v1 < 0xd) code = map[v1]; else code = 0;
    code += 0xa;
  } else {
    FUN_801577bc(a1,1);
    return;
  }
  FUN_801577bc(code,1);
}

/* ---- FUN_8015a1e4 @0x8015a1e4 (231 insns) ----
   s4 = guest ptr; s3 = 0x1f8000e8, s2 = 0x1f8001f0 (guest ptrs);
   s4+0xa8/+0xac hold guest pointers 0x801f47a0/0x801f4770. */
void FUN_8015a1e4(uint32_t a0)
{
  uint8_t *s4 = (uint8_t *)guest_at(a0);
  uint8_t *s3 = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *s2 = (uint8_t *)guest_at(0x1f8001f0);
  int32_t s0;
  int32_t s1;
  int32_t s5;
  int32_t v0;
  int32_t a1v;
  int32_t a0v;
  int32_t idx;
  uint8_t *bp;
  uint32_t *cell;

  s0 = FUN_80153484(a0,0);
  v0 = FUN_80153484(a0,1);
  if (s0 != 0) {
    s5 = (v0 == 0);
  } else {
    s5 = 0;
    if (v0 == 0) {
      s5 = (*(uint16_t *)(s4 + 0xb6) & 0x1000) != 0;
    }
  }
  *(uint32_t *)(s4 + 0xa8) = 0x801f47a0;
  *(uint16_t *)(s4 + 0xa2) = *(uint16_t *)(s4 + 0xa4);
  *(uint32_t *)(s4 + 0xac) = 0x801f4770;
  idx = (int32_t)*(int16_t *)(s4 + 0xa2);
  {
    uint8_t *ta = (uint8_t *)guest_at(0x801f47a0 + (uint32_t)idx);
    uint8_t *tb = (uint8_t *)guest_at(0x801f4770 + (uint32_t)idx);
    a1v = ta ? (int32_t)(int8_t)ta[0] : 0;
    a0v = tb ? (int32_t)(int8_t)tb[0] : 0;
  }
  /* jump table 0x80120f08 on lbu 0x9c(s4): only idx 3 has a body */
  if (s4[0x9c] == 3) {
    s1 = (int32_t)*(int16_t *)(s4 + 0xa2);
    if (s1 >= 0xf) {
      bp = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xc0));
      cell = bp ? (uint32_t *)(bp + 0x44) : NULL;
      if (cell && !(((*cell) >> 0xd) & 1)) {
        *cell |= 0x2000;
        *(uint32_t *)guest_at(0x801c15f0) = 1;
      }
    }
    if (s1 >= 0xb) {
      /* write pseudo-random bytes into the tables */
      int32_t r = FUN_80129f78();
      int32_t q = (s1 * (int32_t)0x2e8ba2e9) >> 33; /* mfhi>>1 - sign */
      int32_t rem;
      /* faithful: hi = mulhi(s1,0x2e8ba2e9); q = (hi>>1) - (s1>>31) */
      {
        int64_t hi = ((int64_t)s1 * (int64_t)(int32_t)0x2e8ba2e9) >> 32;
        q = (int32_t)((hi >> 1) - ((int32_t)s1 >> 31));
      }
      rem = s1 - q * 11;
      {
        uint8_t *ta = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xa8) + (uint32_t)rem);
        int64_t hi2;
        int32_t q2, rem2, adj, q3;
        if (ta) {
          hi2 = ((int64_t)r * (int64_t)(int32_t)0x2e8ba2e9) >> 32;
          q2 = (int32_t)((hi2 >> 1) - ((int32_t)r >> 31));
          rem2 = r - q2 * 11;
          ta[0] = (uint8_t)rem2;
        }
        /* second table: index rem (=s1%11), value r%8 trunc */
        adj = (r < 0) ? r + 7 : r;
        q3 = adj >> 3;
        rem2 = r - q3 * 8;
        {
          uint8_t *tb = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xac) + (uint32_t)rem);
          if (tb) tb[0] = (uint8_t)rem2;
        }
      }
    }
    /* common tail a3d8: a1 = tblA[s1%11], a0 = tblB[s1%8] */
    {
      int64_t hi = ((int64_t)s1 * (int64_t)(int32_t)0x2e8ba2e9) >> 32;
      int32_t q = (int32_t)((hi >> 1) - ((int32_t)s1 >> 31));
      int32_t rem = s1 - q * 11;
      int32_t adj, q3, rem8;
      uint8_t *ta = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xa8) + (uint32_t)rem);
      uint8_t *tb;
      a1v = ta ? (int32_t)(int8_t)ta[0] : 0;
      adj = (s1 < 0) ? s1 + 7 : s1;
      q3 = adj >> 3;
      rem8 = s1 - q3 * 8;
      tb = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xac) + (uint32_t)rem8);
      a0v = tb ? (int32_t)(int8_t)tb[0] : 0;
    }
  }
  if (s4[0x9b] == 0) {
    if (s3[1] != 0) s3[4] = 1;
    if (s2[1] != 0) s2[4] = 0;
    if (s3[1] != 0 && a1v == s2[3]) {
      s3[3] = (uint8_t)a1v;
      s3[4] = (s2[4] == 0) ? 1 : 0;
    }
    if (s2[1] != 0 && s3[3] == a1v) {
      s2[3] = (uint8_t)a1v;
      s2[4] = (s3[4] == 0) ? 1 : 0;
    }
    s4[0x98] = (uint8_t)a0v;
  }
  if (s3[3] == s2[3] && s3[4] == s2[4]) {
    if (s5 == 0) {
      s2[4] = (s3[4] != 0) ? 0 : 1;
    } else {
      s3[4] = (s3[4] != 0) ? 0 : 1;
    }
  }
  if (s4[0x9c] == 4) {
    s3[4] = 0;
    s2[4] = 0;
  }
}

/* ---- FUN_8015a69c @0x8015a69c (299 insns) ---- */
/* ---- FUN_8015a69c @0x8015a69c (299 insns) ----
   a0/a1/a2/a3 are guest pointers; s5 = *(u32*)(a2+0xc4) is also a guest
   pointer. Jump tables read from payload:
     0x80120f20 -> {1000,2000,3000,4500,-1}
     0x80120f38 -> {cond,8,0xb,0xe,0x10,0x12,0x15,0x18}
     0x80120f58 -> {0x64,0x5a,0x50,0x4b,0x46,0x41,0x3c,0x37}
     0x80120f78 -> {none,none,none,bb8,none,flag} */
void FUN_8015a69c(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{
  static const int32_t tab0[5] = {1000,2000,3000,4500,-1};
  static const int32_t tab1[8] = {0,8,0xb,0xe,0x10,0x12,0x15,0x18};
  static const int32_t tab2[8] = {0x64,0x5a,0x50,0x4b,0x46,0x41,0x3c,0x37};
  uint8_t *s2 = (uint8_t *)guest_at(a0);
  uint8_t *s3 = (uint8_t *)guest_at(a1);
  uint8_t *s4 = (uint8_t *)guest_at(a2);
  uint8_t *s0 = (uint8_t *)guest_at(a3);
  uint8_t *s5 = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xc4));
  uint8_t *s6 = (uint8_t *)guest_at(0x801f44a8);
  int32_t s1 = 0;
  uint32_t idx;
  int32_t v;

  /* table 0x80120f20 on lbu 9(s0) */
  idx = s0[9];
  if (idx < 5) {
    *(int16_t *)(s4 + 0x94) = (int16_t)tab0[idx];
  }
  /* 0x54(s2) = (s2[1] ? s0[0x19]&3 : s0[0x19]) */
  v = s2[1] ? (s0[0x19] & 3) : s0[0x19];
  *(uint32_t *)(s2 + 0x54) = (uint32_t)v;
  if ((*(uint32_t *)(s2 + 0x54) & 0xfffffffcU) == 0 && s5) {
    uint32_t c = s5[1];
    uint32_t nv = 0;
    int have = 1;
    if (c == 1)      nv = (s0[0x19] & 3) | 4;
    else if (c == 0) nv = (s0[0x19] & 3);
    else if (c == 2) nv = (s0[0x19] & 3) | 8;
    else if (c == 3) nv = (s0[0x19] & 3) | 0x10;
    else have = 0;
    if (have) *(uint32_t *)(s2 + 0x54) = nv;
  }
  /* 0x54(s3) = (s3[1] ? s0[0x1a]&3 : s0[0x1a]) */
  v = s3[1] ? (s0[0x1a] & 3) : s0[0x1a];
  *(uint32_t *)(s3 + 0x54) = (uint32_t)v;
  if ((*(uint32_t *)(s3 + 0x54) & 0xfffffffcU) == 0 && s5) {
    uint32_t c = s5[1];
    uint32_t nv = 0;
    int have = 1;
    if (c == 1)      nv = (s0[0x1a] & 3) | 4;
    else if (c == 0) nv = (s0[0x1a] & 3);
    else if (c == 2) nv = (s0[0x1a] & 3) | 8;
    else if (c == 3) nv = (s0[0x1a] & 3) | 0x10;
    else have = 0;
    if (have) *(uint32_t *)(s3 + 0x54) = nv;
  }
  /* table 0x80120f38 on lbu 0x26(s0) -> sh 0x86(s3), 0x86(s2) */
  idx = s0[0x26];
  if (idx < 8) {
    int32_t t;
    if (idx == 0) {
      t = (*(uint16_t *)(s0 + 0x26) >> 8) != 0 ? 0xe : 0x10;
    } else {
      t = tab1[idx];
    }
    *(int16_t *)(s3 + 0x86) = (int16_t)t;
    *(int16_t *)(s2 + 0x86) = (int16_t)t;
  } else {
    /* t0 stale in guest; store last computed value is unobservable -
       keep previous behaviour: store 0 */
    *(int16_t *)(s3 + 0x86) = 0;
    *(int16_t *)(s2 + 0x86) = 0;
  }
  if (s6) s6[0x20] = s0[0x12];
  *(int16_t *)(s4 + 0x96) = 3;
  /* FUN_8015ab88(0, table-or-custom) for s0[0x17] */
  {
    uint32_t sel = s0[0x17];
    uint32_t a1v = sel ? (0x801b3f68 + sel * 16) : 0x801c15d0;
    FUN_8015ab88(0,a1v);
  }
  {
    uint32_t sel = s0[0x18];
    uint32_t a1v = sel ? (0x801b3f68 + sel * 16) : 0x801c15e0;
    FUN_8015ab88(1,a1v);
  }
  if (s5 && ((*(uint32_t *)(s5 + 8) >> 4) & 1)) {
    *(uint32_t *)(s2 + 0x54) &= 0xfffffffeU;
    *(uint32_t *)(s3 + 0x54) &= 0xfffffffeU;
  }
  if (s5 && ((*(uint32_t *)(s5 + 8) >> 6) & 1)) {
    if (s6) s6[0x20] &= 0xfe;
  } else if (s6) {
    s6[0x20] |= 1;
  }
  /* table 0x80120f58 on lbu 8(s0) -> s1 */
  idx = s0[8];
  if (idx < 8) s1 = tab2[idx];
  /* table 0x80120f78 on lbu 0x9c(s4) */
  idx = s4[0x9c];
  if (idx < 6) {
    if (idx == 5) {
      if (s4[0x9b] == 4) {
        *(int16_t *)(s4 + 0x94) = -1;
        *(uint32_t *)(s2 + 0x54) |= 2;
        *(uint32_t *)(s3 + 0x54) |= 2;
      }
    } else if (idx == 3) {
      *(int16_t *)(s4 + 0x94) = 0xbb8;
    }
    /* all six targets store s1 -> 0xde(s2)/0xde(s3) */
    if (s2) s2[0xde] = (uint8_t)s1;
    if (s3) s3[0xde] = (uint8_t)s1;
  }
}


/* ---- FUN_8015abc0 @0x8015abc0 (72 insns) ----
   a0 guest state, a1 sprite sel, a2 second arg, a3 third arg,
   a4 (stack arg) 0x801c47b8 selector. */
int FUN_8015abc0(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t a4)
{
  uint8_t *s2 = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(s2 + 0x8c);
  int32_t s1 = (int32_t)a4;

  if (s0 < 0) return -1;
  if (s0 == 0) {
    FUN_8018cfdc(-1,0);
    *(uint32_t *)guest_at(0x801c43a0) = 0;
    *(uint8_t *)guest_at(0x801e3d00) = 0xff;
    if (s1 == 0) {
      *(uint32_t *)guest_at(0x801c47b8) = 0x80010000;
    } else if (s1 == 1) {
      extern uint8_t *FUN_80127680(void);
      extern uint32_t guest_of(const void *p);
      *(uint32_t *)guest_at(0x801c47b8) = guest_of(FUN_80127680());
    }
    s0 = 0x10;
  } else if (s0 != 0x10) {
    *(int16_t *)(s2 + 0x8c) = (int16_t)s0;
    return 0;
  }
  if (FUN_8018cfdc((int32_t)a1,g32(0x801c47b8)) == 0) {
    *(int16_t *)(s2 + 0x8c) = (int16_t)s0;
    return 0;
  }
  FUN_801566a0(g32(0x801c47b8),(int32_t)a3);
  FUN_801566d4((int32_t)a2,(int32_t)a3);
  *(int16_t *)(s2 + 0x8c) = -1;
  return 1;
}

/* ---- FUN_8015ace4 @0x8015ace4 (298 insns) ----
   a0 is a HOST pointer (callers pass _g_pad_buffer).
   The 0x38-byte descriptor at sp+0x10 is built once in the init state
   and persists across loop iterations (FUN_801294cc may edit it). */
int FUN_8015ace4(uint8_t *s0)
{
  uint8_t *s4 = (uint8_t *)guest_at(0x1f800000);
  uint8_t desc[0x38];
  int32_t s1 = 0;
  int32_t s3 = 0;

  memset(desc,0,sizeof(desc));
  for (;;) {
    if (s1 == 1) goto body;
    if (s1 < 2) {
      if (s1 == 0) {
        uint16_t f;
        uint8_t *bp;
        if (FUN_8012aaa8() != 0) goto poll;
        FUN_8012cbc0(2,0,0);
        f = *(uint16_t *)(s0 + 2);
        *(uint32_t *)(desc + 0xc) = 0x8015b190;
        /* explicit offsets (desc base = sp+0x10) */
        *(uint16_t *)(desc + 0x00) = 0;
        desc[0x02] = 0; desc[0x04] = 0;
        *(uint16_t *)(desc + 0x06) = 0x11;
        *(uint16_t *)(desc + 0x08) = 1;
        *(uint32_t *)(desc + 0x0c) = 0x8015b190;
        *(uint16_t *)(desc + 0x10) = 0xffff;
        *(uint16_t *)(desc + 0x12) = 0xffff;
        *(uint16_t *)(desc + 0x14) = ((f >> 2) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x16) = ((f >> 3) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x18) = ((f >> 4) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x1a) = ((f >> 5) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x1c) = ((f >> 6) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x1e) = ((f >> 7) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x20) = *(uint16_t *)(s0 + 4);
        *(uint16_t *)(desc + 0x22) = *(uint16_t *)(s0 + 6);
        *(uint16_t *)(desc + 0x24) = *(uint16_t *)(s0 + 8);
        *(uint16_t *)(desc + 0x26) = *(uint16_t *)(s0 + 0xa);
        *(uint16_t *)(desc + 0x28) = (f & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x2a) = ((f >> 1) & 1) ? 1 : 0;
        bp = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xc0));
        *(uint16_t *)(desc + 0x2c) = bp ? bp[0x25] : 0;
        *(uint16_t *)(desc + 0x2e) = bp ? bp[0xb] : 0;
        *(uint16_t *)(desc + 0x30) = ((f >> 0xa) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x32) = ((f >> 0xb) & 1) ? 1 : 0;
        *(uint16_t *)(desc + 0x34) = ((f >> 0xc) & 1) ? 1 : 0;
        battle_session_arm((intptr_t)0x1f800000,-1);
        s1++;
        goto body;
      }
      goto poll;
    }
    if (s1 == 0xf0) goto statef0;
    if (s1 == 0xf1) { s1 = 0xff; goto check; }
    goto poll;
body:
    if (*(uint16_t *)(s4 + 0x7c) & 0x10) {
      FUN_80156ef4(0x400,0xd,0);
      s1 = 0xf0;
      s3 = 1;
    }
    if (*(uint16_t *)(s4 + 0x7c) & 0xe0) {
      FUN_80156ef4(0x400,0xc,0);
      s1 = 0xf0;
      s3 = 0;
    }
    FUN_801294cc(desc);
    goto poll;
statef0:
    s1 = 0xff;
    {
      uint8_t *bp = (uint8_t *)guest_at(*(uint32_t *)(s4 + 0xc0));
      uint16_t w = 0;
      *(uint16_t *)(s0 + 2) = 0;
      if (*(int16_t *)(desc + 0x14) != 0) w = 4;
      *(uint16_t *)(s0 + 2) = w;
      if (*(int16_t *)(desc + 0x16) != 0) *(uint16_t *)(s0 + 2) |= 8;
      if (*(int16_t *)(desc + 0x18) != 0) *(uint16_t *)(s0 + 2) |= 0x10;
      if (*(int16_t *)(desc + 0x1a) != 0) *(uint16_t *)(s0 + 2) |= 0x20;
      if (*(int16_t *)(desc + 0x1c) != 0) *(uint16_t *)(s0 + 2) |= 0x40;
      if (*(int16_t *)(desc + 0x1e) != 0) *(uint16_t *)(s0 + 2) |= 0x80;
      *(uint16_t *)(s0 + 4)  = *(uint16_t *)(desc + 0x20);
      *(uint16_t *)(s0 + 6)  = *(uint16_t *)(desc + 0x22);
      *(uint16_t *)(s0 + 8)  = *(uint16_t *)(desc + 0x24);
      *(uint16_t *)(s0 + 0xa) = *(uint16_t *)(desc + 0x26);
      if (*(int16_t *)(desc + 0x28) != 0) *(uint16_t *)(s0 + 2) |= 1;
      if (*(int16_t *)(desc + 0x2a) != 0) *(uint16_t *)(s0 + 2) |= 2;
      if (bp) bp[0x25] = desc[0x2c];
      if (*(int16_t *)(desc + 0x30) != 0) *(uint16_t *)(s0 + 2) |= 0x400;
      if (*(int16_t *)(desc + 0x32) != 0) *(uint16_t *)(s0 + 2) |= 0x800;
      if (*(int16_t *)(desc + 0x34) != 0) *(uint16_t *)(s0 + 2) |= 0x1000;
    }
    goto check;
poll:
    frame_tick((intptr_t)s4);
check:
    if (s1 != 0xff) continue;
    break;
  }
  return s3;
}

/* ---- FUN_80160598 @0x80160598 (188 insns) ----
   a0 = guest dialog-state ptr, a1 = selector (u16). */
extern void hud_text_b();
extern int FUN_80129978();
extern void FUN_8012bf0c();
int FUN_80160598(uint32_t a0,uint32_t a1)
{
  uint8_t *s3 = (uint8_t *)guest_at(a0);
  uint8_t *s2 = (uint8_t *)guest_at(0x1f800000);
  int32_t s0 = 0;
  int32_t s1;
  uint32_t ctx8 = *(uint32_t *)(s2 + 8);
  uint32_t ctxc = *(uint32_t *)(s2 + 0xc);
  int32_t v0;

  FUN_8012cd9c(0x20,0xffffffff);
  s1 = (int32_t)(a1 & 0xffff);
  if (s1 == (int32_t)(*(uint16_t *)(s3 + 8) + 0x100)) {
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121270),0x1000,0x1000,0);
  } else {
    s0 = 1;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121274),0x1000,0x1000,s0);
  }
  {
    uint8_t *bp = (uint8_t *)guest_at(*(uint32_t *)(s2 + 0xc0));
    int32_t b = bp ? (int32_t)bp[0x22] : 0;
    *(int16_t *)(s3 + 0x14) = (int16_t)b;
  }
  s1 &= 0xffff;
  if (s1 == 0x100) {
    int32_t inrange;
    FUN_8012cd9c(0xffffffff,0x18);
    FUN_8012e58c(ctx8,ctxc,(const void *)guest_at(0x80121360),0x1000,0x1000,0);
    inrange = ((uint32_t)(s3[3] - 0x12) < 3);
    FUN_8012bf0c(4,0,inrange ? 0 : 4,7);
    FUN_8012cdd8(0,0x10);
    return 0;
  }
  if (s1 < 0x101) {
    if (s1 == 2 || s1 == 3) {
      if ((*(uint16_t *)(s2 + 0x7c) & 0xe0) != 0) {
        FUN_80156ef4(0x400,0xc,0);
        s3[3] = 0x10;
      }
      return 0;
    }
    if (s1 == 1) {
      if (FUN_80129978(ctx8,1,1) == 0) return 0;
      {
        uint8_t *bp = (uint8_t *)guest_at(*(uint32_t *)(s2 + 0xc0));
        if (bp) bp[0x22] = s3[0x14];
      }
      if (*(int16_t *)(s3 + 0x14) == 0) return 0;
      if (*(int16_t *)(s3 + 0x14) != s1) return 0;
      s3[3] = 0x10;
      return 0;
    }
    return 2;   /* s1 == 0 path: v0 = 2 */
  }
  /* s1 >= 0x101 */
  if (s1 == 0x101) {
    uint32_t str;
    FUN_8012e58c(ctx8,ctxc,(const void *)guest_at(0x80121b6c),0x1000,0x1000,s0);
    str = g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s3 + 0x14)) * 4);
    FUN_8012e58c(ctx8,ctxc,(const void *)guest_at(str),0x160,0x1000,s0);
    FUN_8012cdd8(0,0xc);
    return 0;
  }
  if (s1 == 0x102 || s1 == 0x103 || s1 == 0x104) {
    uint32_t str;
    if (s1 == 0x102)      str = 0x80121b78;
    else if (s1 == 0x103) str = 0x80121b98;
    else                  str = 0x80121318;
    FUN_8012e58c(ctx8,ctxc,(const void *)guest_at(str),0x1000,0x1000,s0);
    FUN_8012cdd8(0,0xc);
    return 0;
  }
  if (s1 == 0x1ff) return 0;
  return 1;
}

/* ---- FUN_80165908 @0x80165908 (234 insns) ----
   a0 = guest state ptr, a1 = selector s2. */
int FUN_80165908(uint32_t a0,uint32_t a1)
{
  static const int32_t tab[16] = {
    0x86,0x1f,0x1e,0x1d,0x26,0x2e,0x36,0x3e,
    0x46,0x4e,0x56,0x5e,0x66,0x6e,0x76,0x1c
  };
  uint8_t *s4 = (uint8_t *)guest_at(a0);
  int32_t s2 = (int32_t)a1;
  int32_t s1;
  int32_t s0 = 0x1c;
  int32_t s5;
  uint32_t v47b8;
  uint32_t v3cfc;

  s1 = (int32_t)*(int16_t *)(s4 + 0x8c);
  if (s1 < 0) return -1;
  if (s1 == 0) {
    FUN_8018cfdc(-1,0);
    s1 = 0x10;
    *(uint32_t *)guest_at(0x801c43a0) = 0;
    *(uint8_t *)guest_at(0x801e3d00) = 0xff;
    *(uint32_t *)guest_at(0x801c47b8) = 0x80010000;
    *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
  } else if (s1 != 0x10) {
    *(int16_t *)(s4 + 0x8c) = (int16_t)s1;
    return 0;
  }
  if ((uint32_t)s2 < 0x10) s0 = tab[s2];
  v3cfc = g32(0x801e3cfc);
  if (FUN_8018cfdc(s0,v3cfc) == 0) {
    *(int16_t *)(s4 + 0x8c) = (int16_t)s1;
    return 0;
  }
  s5 = g8(0x1f800013);
  FUN_801274d8(0);
  v47b8 = g32(0x801c47b8);
  v3cfc = g32(0x801e3cfc);
  if (s2 < 0xf) {
    if (s2 >= 4) {
      /* four-sprite block */
      FUN_80163154(v47b8,v3cfc,0);
      FUN_8018bfcc(v47b8,0,0x1f0,1,0x200,0,1);
      FUN_80163154(v47b8,v3cfc,1);
      FUN_8018bfcc(v47b8,0,0x1f1,1,0x280,0,1);
      FUN_80163154(v47b8,v3cfc,2);
      FUN_8018bfcc(v47b8,0,0x1f2,1,0x240,0x100,1);
      FUN_80163154(v47b8,v3cfc,3);
      FUN_8018bfcc(v47b8,0,0x1f3,1,0x2c0,0x100,1);
    } else if (s2 == 3) {
      FUN_80163154(v47b8,v3cfc,0);
      FUN_8018bfcc(v47b8,0,0x1f4,1,0x300,0,1);
      FUN_80163154(v47b8,v3cfc,1);
      FUN_8018bf40((int32_t)v47b8,0x280,0,1);
    } else {
      goto single;
    }
  } else if (s2 == 0xf) {
    FUN_80163154(v47b8,v3cfc,0);
    FUN_8018bfcc(v47b8,0,0x1f4,1,0x300,0,1);
    FUN_80163154(v47b8,v3cfc,1);
    FUN_8018bf40((int32_t)v47b8,0x340,0,1);
  } else {
single:
    FUN_80163154(v47b8,v3cfc,0);
    FUN_8018bfcc(v47b8,0,0x1f4,1,0x300,0,1);
  }
  FUN_8019a460(0);
  FUN_801274d8(s5);
  *(int16_t *)(s4 + 0x8c) = -1;
  return 1;
}

/* ---- FUN_801660c0 @0x801660c0 (181 insns) ----
   s1/s2 are callee-saved: they persist across calls (static here).
   Entry clears 0x1f800001 and 0x1f80008c; exit clears 0x1f800001. */
void FUN_801660c0(uint32_t a0)
{
  static int32_t s1;
  static int32_t s2;
  uint8_t *s0 = (uint8_t *)guest_at(0x1f800000);
  int32_t s3 = 0xff;
  int32_t s4 = (int32_t)a0;
  int32_t v1 = 0;

  s2 = (int32_t)((uint32_t)(s1 * 255) + 0xffff06faU);
  s0[1] = 0;
  *(uint16_t *)(s0 + 0x8c) = 0;
  for (;;) {
    int32_t st = s0[1];
    if (st == 1) goto tick;
    if (st < 2) {
      if (st == 0) {
        if (FUN_80165908(0x1f800000,(uint32_t)(s4 + 4)) != 0 &&
            FUN_8012aaa8() == 0) {
          FUN_8018cc6c(*(uint32_t *)(s0 + 8),0,0,
                        (int16_t)*(uint16_t *)(s0 + 0x20),
                        (int16_t)*(uint16_t *)(s0 + 0x22),
                        0x31ce,3);
          s1 = 0x12c;
          s0[1] = (uint8_t)(s0[1] + 1);
          goto tick;
        }
      }
    } else if (st == 0xf0) {
      FUN_80156ef4(0x204,0,0);
      s0[1] = (uint8_t)(s0[1] + 1);   /* dead: overwritten below */
      s0[1] = 0xff;
      goto check;
    } else if (st == 0xf1) {
      s0[1] = 0xff;
      goto check;
    }
    goto poll;
tick:
    s1--;
    s2 -= 0xff;
    if (s1 == 0) {
      s0[1] = 0xf0;
      goto poll;
    }
    if (s1 < 0x32) {
      v1 = 0xff - (int32_t)((int32_t)(s1 * 255) / 50);
      FUN_8018cc6c(*(uint32_t *)(s0 + 8),0,0,
                    (int16_t)*(uint16_t *)(s0 + 0x20),
                    (int16_t)*(uint16_t *)(s0 + 0x22),
                    (uint32_t)(v1 * 0x10101),3);
    } else if (s1 >= 0xfa && s1 < 0x12c) {
      v1 = (int32_t)((int32_t)s2 / 50);
      FUN_8018cc6c(*(uint32_t *)(s0 + 8),0,0,
                    (int16_t)*(uint16_t *)(s0 + 0x20),
                    (int16_t)*(uint16_t *)(s0 + 0x22),
                    (uint32_t)(v1 * 0x10101),3);
    }
    FUN_8018c824(*(uint32_t *)(s0 + 8) + 4,0,0xefff0088,0x7c000000);
    FUN_8018c824(*(uint32_t *)(s0 + 8) + 4,0x100,0xefff008a,0x7c400000);
    FUN_8018c824(*(uint32_t *)(s0 + 8) + 4,0xf00000,0xefff0099,0x7c800000);
    FUN_8018c824(*(uint32_t *)(s0 + 8) + 4,0xf00100,0xefff009b,0x7cc00000);
    goto poll;
poll:
    frame_tick((intptr_t)s0);
check:
    if (s0[1] != 0xff) continue;
    break;
  }
  s0[1] = 0;
  (void)v1;
}

/* ---- FUN_8016642c @0x8016642c (69 insns) ---- */
void FUN_8016642c(uint32_t a0,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  int8_t s2 = (int8_t)s0[0x99];
  uint32_t w;

  w = *(uint32_t *)(s0 + 0xbc);
  s0[0x9d] = 0;
  s0[0x9e] = 0;
  s0[0x99] = 0;
  FUN_8012f024(w,a0);
  FUN_8012a030(FUN_80129f78());
  reset_sprite_ot_block(a0);
  task_table_init(0);
  record_copy_1a((int32_t)(int8_t)s0[0x98]);
  FUN_801358e8((int32_t)(int8_t)s0[0x98],(int32_t)(int8_t)s0[0x99]);
  FUN_8012e97c((int32_t)(int8_t)s0[0x98],0x1f8000e8,0x1f8001f0);
  clear_f4ee8_u16s(0x1f8000e8);
  clear_f4ee8_u16s(0x1f8001f0);
  reset_combat_slots();
  FUN_80175fec((void *)(uintptr_t)a0,(uint8_t *)guest_at(0x1f8000e8),a1);
  FUN_80175fec((void *)(uintptr_t)a0,(uint8_t *)guest_at(0x1f8001f0),a1);
  FUN_80172a34((int32_t)(int8_t)s0[0x99]);
  FUN_8016d478(a0);
  clear_f0300_records();
  FUN_8012f024(*(uint32_t *)(s0 + 0xbc),a0);
  s0[0x99] = (uint8_t)s2;
}

/* ---- FUN_801670c8 @0x801670c8 (117 insns) ---- */
void FUN_801670c8(uint32_t a0,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  int32_t s3 = (int32_t)a1;
  int32_t v0;
  int32_t q;
  int32_t a1v;
  int32_t a3v;

  if (s3 != 1) return;
  v0 = (int32_t)*(int16_t *)(s0 + 0x20);
  if (*(uint8_t *)(s0 + 0x15) != 0) {
    /* sprite path: two FUN_8018c31c draws then FUN_8012e078 */
    q = (v0 < 0) ? v0 + 0x1f : v0;
    q >>= 5;
    a1v = ((v0 - q) - 0x68) & 0xffff;
    a1v |= 0x1a000000;
    FUN_8018c31c(*(uint32_t *)(s0 + 8),a1v,0x1e7f000f,0x7adfe000,
                  0x1000,0x2000,0);
    q = (v0 < 0) ? v0 + 0x1f : v0;
    q >>= 5;
    a1v = ((v0 - q) - 0x14) & 0xffff;
    a1v |= 0x19800000;
    FUN_8018c31c(*(uint32_t *)(s0 + 8),a1v,0x2627000f,0x7adfd880,
                  0x1000,0x2000,0);
    a3v = (int32_t)*(int16_t *)(s0 + 0x20);
    if (a3v < 0) a3v += 0xf;
    FUN_8012e078(*(uint32_t *)(s0 + 8),*(uint32_t *)(s0 + 0xc),
                  (const void *)guest_at(0x801220e4),
                  a3v >> 4,0x20,s3);
    return;
  }
  /* plain path: two FUN_8018c1f4 draws then FUN_8012e078 */
  q = (v0 < 0) ? v0 + 0x1f : v0;
  q >>= 5;
  a1v = ((v0 - q) - 0x68) & 0xffff;
  a1v |= 0x00d00000;
  FUN_8018c1f4(*(uint32_t *)(s0 + 8),a1v,0x1e7f000f,0x7adfe000);
  q = (v0 < 0) ? v0 + 0x1f : v0;
  q >>= 5;
  a1v = ((v0 - q) - 0x14) & 0xffff;
  a1v |= 0x00cc0000;
  FUN_8018c1f4(*(uint32_t *)(s0 + 8),a1v,0x2627000f,0x7adfd880);
  a3v = (int32_t)*(int16_t *)(s0 + 0x20);
  if (a3v < 0) a3v += 0xf;
  FUN_8012e078(*(uint32_t *)(s0 + 8),*(uint32_t *)(s0 + 0xc),
                (const void *)guest_at(0x801220e4),
                a3v >> 4,0x10,s3);
}

/* ---- FUN_80167ce4 @0x80167ce4 (37 insns, 13-arg ABI) ---- */
int FUN_80167ce4(uint32_t p0,uint32_t p1,uint32_t p2,uint32_t p3,
                 uint32_t p4,uint32_t p5,uint32_t p6,uint32_t p7,
                 uint32_t p8,uint32_t p9,uint32_t p10,uint32_t p11,
                 uint32_t p12)
{
  uint32_t slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
  *(uint32_t *)guest_at(slot) = 0;
  return FUN_80167d7c(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12);
}

/* ---- FUN_80167d7c @0x80167d7c (199 insns, 13-arg ABI) ----
   p0 = port flag (slot = 0x801b43e8 + (p0?4:0)), p1..p3 = in-header,
   p7 = arg for FUN_801685b4, p8/p9 = a3 select for data stage,
   p11 = data ptr, p12 = data size. */
int FUN_80167d7c(uint32_t p0,uint32_t p1,uint32_t p2,uint32_t p3,
                 uint32_t p4,uint32_t p5,uint32_t p6,uint32_t p7,
                 uint32_t p8,uint32_t p9,uint32_t p10,uint32_t p11,
                 uint32_t p12)
{
  uint32_t in[3];
  uint8_t buf18[0x28];
  uint8_t b40[0x80];
  uint8_t bc0[0x30];
  int32_t s2 = (p0 != 0) ? 0x10 : 0;
  int32_t s4 = 0;
  int32_t ret;
  uint32_t slot;
  uint8_t *dat;
  uint32_t i;
  uint8_t t0;
  uint32_t st;
  uint32_t asz;
  uint32_t sel;

  in[0] = p1; in[1] = p2; in[2] = p3;

poll:
  ret = FUN_80168424((uint32_t)s2);
  if (ret == 3) {
    slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
    *(uint32_t *)guest_at(slot) = (uint32_t)ret;
    return ret;
  }
  if ((uint32_t)ret < 4) {
    if (ret == 0) goto probe;
    return ret;
  }
  if (ret != 4) return ret;
  slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
  *(uint32_t *)guest_at(slot) = 3;
  return 4;

probe:
  ret = FUN_8016850c((uint32_t)s2,in,buf18,bc0,0,1);
  if (ret != 0) return ret;
  st = *(uint32_t *)bc0;
  if (st != 0) {
    /* header already present: verify slot state then decompress */
    slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
    if (*(uint32_t *)guest_at(slot) == 3) return 0xf;
    ret = FUN_80168838((uint32_t)s2,in,b40,0x200,
                       *(uint32_t *)guest_at(slot));
    if (ret != 0) return ret;
    s4 = (*(uint8_t *)b40 < 1) ? 1 : 0;
    goto stage2;
  }
  ret = FUN_801685b4((uint32_t)s2,in,p7);
  if (ret == 4) goto poll;
  if ((uint32_t)ret < 5) {
    if (ret != 0) return ret;
    memset(b40,0,0x80);
    s4 = 0;
  } else if (ret == 6) {
    memset(b40,0,0x80);
    s4 = 0;
  } else {
    return ret;
  }

stage2:
  asz = ((uint32_t)(p12 + 0x7f)) & ~0x7fu;
  sel = (s4 != 0) ? p9 : p8;
  {
    void *dp = mem_ptr(p10);
    void *dd = mem_ptr(p11);
    ret = FUN_801688f0((uint32_t)s2,in,dp,0,0x200);
    if (ret != 0) return ret;
    ret = FUN_8016831c((uint32_t)s2,in,dp,0,0x200);
    if (ret != 0) return ret;
    ret = FUN_801688f0((uint32_t)s2,in,dd,sel,asz);
    if (ret != 0) return ret;
    ret = FUN_8016831c((uint32_t)s2,in,dd,sel,asz);
    if (ret != 0) return ret;
    dat = (uint8_t *)dd;
    t0 = 0;
    if ((int32_t)p12 > 0) {
      for (i = 0; i < p12; i++) t0 ^= dat[i];
    }
    b40[0] = (uint8_t)s4;
    b40[(uint32_t)s4 + 1] = t0;
    ret = FUN_801688f0((uint32_t)s2,in,b40,0x200,0x80);
    if (ret != 0) return ret;
    ret = FUN_8016831c((uint32_t)s2,in,b40,0x200,0x80);
    if (ret != 0) return ret;
  }
  slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
  *(uint32_t *)guest_at(slot) = 0;
  return 0;
}

/* ---- FUN_8016809c @0x8016809c (119 insns, 13-arg ABI) ----
   Read-back counterpart of FUN_80167d7c: p0 flag, p1..p3 header,
   p8/p9 select, p11 dest, p12 size. */
int FUN_8016809c(uint32_t p0,uint32_t p1,uint32_t p2,uint32_t p3,
                 uint32_t p4,uint32_t p5,uint32_t p6,uint32_t p7,
                 uint32_t p8,uint32_t p9,uint32_t p10,uint32_t p11,
                 uint32_t p12)
{
  /* guest frame: fp+0x44.. = {a1,a2,a3,p4..p12} = 12 words, indexed
     by buf[0] up to 255 -> keep a wide zero-padded view. */
  uint32_t in[280];
  uint8_t *buf;
  size_t asz;
  int32_t s7 = (int32_t)p0;
  int32_t s4;
  int32_t ret;
  uint32_t slot;
  uint32_t s0 = p12;
  uint32_t s3 = p11;
  uint8_t s6, s5;
  uint32_t i;

  memset(in,0,sizeof(in));
  in[0]=p1; in[1]=p2; in[2]=p3; in[3]=p4; in[4]=p5; in[5]=p6; in[6]=p7;
  in[7]=p8; in[8]=p9; in[9]=p10; in[10]=p11; in[11]=p12;

  if (s0 == 0) return 0;
  s4 = (s7 != 0) ? -1 : 0;
  s4 &= 0x10;

  ret = FUN_80168424((uint32_t)s4);
  if (ret == 3 || ret == 0) {
    /* ok */
  } else {
    slot = 0x801b43e8 + (s4 & 4);
    *(uint32_t *)guest_at(slot) = 3;
    return ret;
  }

  asz = ((uint32_t)(s0 + 0x7f)) & ~0x7fu;
  buf = (uint8_t *)malloc((size_t)asz + 0x20);
  if (buf == NULL) return -1;
  buf += 0x18; /* s2 = sp+0x18 */

  *(uint32_t *)(buf - 8) = 0x80; /* sp+0x10 outgoing arg */
  ret = FUN_80168838((uint32_t)s4,in,buf,0x200,0x80);
  if (ret != 0) { free(buf - 0x18); return ret; }

  s6 = buf[0];
  s5 = buf[(uint8_t)s6 + 1];
  {
    uint32_t idx = (uint8_t)s6;
    for (;;) {
      uint32_t a3v = in[7 + idx];
      uint32_t sum = 0;
      uint32_t s1new;
      *(uint32_t *)(buf - 8) = asz;
      ret = FUN_80168838((uint32_t)s4,in,buf,a3v,asz);
      if (ret != 0) { free(buf - 0x18); return ret; }
      if ((int32_t)s0 > 0) {
        for (i = 0; i < s0; i++) sum ^= buf[i];
      }
      s1new = (s6 < 1) ? 1 : 0;
      if (s5 == (sum & 0xff)) {
        uint8_t *dst = (uint8_t *)mem_ptr(s3);
        uint8_t *src = buf;
        for (i = 0; i < s0; i++) *dst++ = *src++;
        slot = 0x801b43e8 + ((s7 != 0) ? 4 : 0);
        *(uint32_t *)guest_at(slot) = 0;
        free(buf - 0x18);
        return 0;
      }
      idx = s1new;
      if (s6 == s1new) { free(buf - 0x18); return 2; }
    }
  }
}

/* ---- FUN_8016ddf8 @0x8016ddf8 (264 insns) ---- */
void FUN_8016ddf8(uint32_t a0)
{
  uint8_t *s2 = (uint8_t *)guest_at(0x1f800000);
  uint8_t *t1;
  int32_t s0;
  int32_t s1 = 0;
  int32_t s3 = 0;
  int32_t s4 = 0;
  int32_t s5 = 0;
  int32_t s6 = 0x59;
  int32_t fp = -1;
  int32_t v1 = 0;
  (void)a0;

  s0 = ((*(uint16_t *)(s2 + 0x92) & 0xf00) ^ 0x100) != 0;
  FUN_80136ddc();
  FUN_801274fc(0,0,0);
  t1 = (uint8_t *)guest_at(0x1f800000 + (uint32_t)s0 * 2);
  for (;;) {
    int32_t st;
    if (s5 == 1) goto body;
    if (s5 < 2) {
      if (s5 == 0) {
        if (FUN_8012aaa8() != 0) goto poll;
        FUN_8015e87c();
        FUN_8015e88c();
        *(int16_t *)(s2 + 0x8c) = 0;
        s4 = 0;
        s3 = s4;
        s5 = 1;
        goto body;
      }
      goto poll;
    }
    if (s5 == 0xf0) {
      FUN_80156ef4(0x203,0,0);
      s5 = 0xff;
      goto poll;
    }
    if (s5 == 0xf1) {
      s5 = 0xff;
      goto poll;
    }
    goto poll;
body:
    if (s3 != 0) goto countdown;
    if (*(int16_t *)(s2 + 0x8c) < 0) goto check_draw;
    s1 = (int32_t)(int16_t)s6;
    {
      int32_t idx = s1 - 1;
      int32_t q = (idx < 0) ? (idx + 0x1e) : idx;
      int32_t bit;
      uint8_t *bp;
      uint32_t *cell;
      uint32_t w;
      q >>= 5;
      bit = idx - (q << 5);
      bp = (uint8_t *)guest_at(*(uint32_t *)(s2 + 0xc0));
      cell = bp ? (uint32_t *)(bp + (uint32_t)q * 4) : NULL;
      w = cell ? *cell : 0;
      if (FUN_8015e8a4(idx) != 0) {
        fp = s6;
        if (!((w >> bit) & 1)) {
          w |= (uint32_t)1 << bit;
          *cell = w;
          *(uint32_t *)guest_at(0x801c15f0) = 1;
        }
        s4 = (s1 == 0x59) ? 0x64 : 0x32;
      }
    }
    if (s4 == 0) goto icon;
    if (s6 == 0x59) {
      v1 = (int32_t)((int32_t)(s4 * 255) / 100);
    } else {
      v1 = (int32_t)((int32_t)(s6 - s4) / 50);
    }
    s4--;
    goto draw;
countdown:
    if (s6 == 0x5c) {
      v1 = (int32_t)((int32_t)(s3 * 255) / 100);
    } else {
      v1 = (int32_t)((int32_t)(s6 - s3) / 50);
    }
    s3--;
    v1 = 0xff - v1;
    if (s3 == 0) {
      fp = 0;
      if ((int16_t)s6 >= 0x5c) s5 = 0xf0;
    }
    goto draw;
check_draw:
    if (s4 == 0) goto icon;
    if (s6 == 0x59) {
      v1 = (int32_t)((int32_t)(s4 * 255) / 100);
    } else {
      v1 = (int32_t)((int32_t)(s6 - s4) / 50);
    }
    s4--;
draw:
    FUN_8018cc6c(*(uint32_t *)(s2 + 8),0,0,
                  (int16_t)*(uint16_t *)(s2 + 0x20),
                  (int16_t)*(uint16_t *)(s2 + 0x22),
                  (uint32_t)(v1 * 0x10101),3);
icon:
    if ((int32_t)fp > 0) {
      FUN_8018c71c(*(uint32_t *)(s2 + 8) + 4,0,0xeffe0088,0x7fc00000,
                    (int32_t)*(int16_t *)(s2 + 0x20),
                    (int32_t)*(int16_t *)(s2 + 0x22));
    }
    if (*(int16_t *)(s2 + 0x8c) < 0 && s4 == 0 && s3 == 0) {
      if ((*(uint16_t *)(t1 + 0x6c) & 0xf) == 0) {
        if ((g32(0x1f8000dc) & 0x30) != 0) {
          int32_t xv = (int32_t)(int16_t)*(uint16_t *)(s2 + 0x22);
          int32_t yv = (int32_t)(int16_t)*(uint16_t *)(s2 + 0x20) - 0x18;
          int32_t q = xv * 3;
          int32_t xs;
          if (q < 0) q += 3;
          xs = ((q >> 2) + 0x48);
          xs = (int16_t)xs;
          FUN_8012e58c(*(uint32_t *)(s2 + 8),*(uint32_t *)(s2 + 0xc),
                        (const void *)guest_at(0x801222a4),
                        (uint32_t)(int32_t)(int16_t)yv,
                        (uint32_t)xs,0);
        }
        {
          uint16_t m = *(uint16_t *)(t1 + 0x68);
          if (m & 0x800) {
            s5 = 0xf0;
          } else if (m & 0xe0) {
            FUN_80156ef4(0x400,0xc,0);
            s6 = (int16_t)(s6 + 1);
            s3 = (s6 == 0x5c) ? 0x64 : 0x32;
            *(int16_t *)(s2 + 0x8c) = 0;
          }
        }
      }
    }
poll:
    frame_tick((intptr_t)s2);
    if (s5 != 0xff) continue;
    break;
  }
  (void)s1;
}

/* ---- FUN_801709fc @0x801709fc (206 insns) ---- */
int32_t FUN_801709fc(uint32_t a0,uint8_t *s4,uint8_t *s5,int32_t s2)
{
  uint8_t *s1 = (uint8_t *)guest_at(a0);
  int32_t s3 = 0;
  int32_t s0;
  int32_t v0;

  v0 = (int32_t)*(int16_t *)(s1 + 0x8c);
  s3 = 0;
  if (v0 < 0) return v0;
  *(int16_t *)(s1 + 0xd2) = 0;
  if (s2 != 1) {
    *(int16_t *)(s1 + 0xd0) = 0;   /* delay slot: always executed */
  } else {
    *(int16_t *)(s1 + 0xd0) = 0;
    s3 = 1;
  }
  s0 = (int32_t)*(int16_t *)(s1 + 0x8c);

  for (;;) {
    if (s0 == 0x30) { s0 = 0x40; goto st40; }
    if (s0 < 0x31) {
      if (s0 == 0x10) goto st10;
      if (s0 < 0x11) {
        if (s0 == 0) goto st00;
        goto store0;
      }
      if (s0 == 0x20) goto st20;
      goto store0;
    }
    if (s0 == 0x50) goto st50;
    if (s0 < 0x51) {
      if (s0 == 0x40) goto st40;
      goto store0;
    }
    if (s0 == 0xf00) goto stf00;
    goto store0;

st00:
    FUN_8018cfdc(-1,0);
    s4[0] = 0;
    s5[0] = 0;
    *(uint32_t *)guest_at(0x801c47b8) = 0x80010000;
    *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
    *(uint16_t *)(s1 + 0x8e) = 0;
    if (s2 == 3) {
      v0 = (int32_t)g32(0x801c11d8);
      s0 = 0x40;
      *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
      *(uint32_t *)guest_at(0x801c47b8) = (uint32_t)v0;
      goto dispatch;
    }
    if (s2 < 4) {
      if (s2 == 2) {
        v0 = (int32_t)g32(0x801c43a0);
        s0 = 0x20;
        if (v0 != 0) {
          *(uint32_t *)guest_at(0x801c47b8) = (uint32_t)v0;
        } else {
          s0 = 0x10;
        }
        goto dispatch;
      }
      s0 = 0x10;
      goto dispatch;
    }
    if (s2 == 4) {
      v0 = (int32_t)g32(0x801c15f4);
      s0 = 0x50;
      *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
      *(uint32_t *)guest_at(0x801c47b8) = (uint32_t)v0;
      goto dispatch;
    }
    s0 = 0x10;
    goto dispatch;

st10:
    if (FUN_80170d38(a0,(int32_t)(int8_t)s1[0x98],s4,s5) == 0) {
      goto store0;
    }
    s0 = 0x20;
    *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
    *(uint32_t *)guest_at(0x801c43a0) = g32(0x801c47b8);
    *(uint16_t *)(s1 + 0x8e) = 0;
    /* fall into st20 */

st20:
    if (FUN_80170fe0(a0,(int32_t)(int8_t)s1[0x98]) == 0) {
      goto store0;
    }
    {
      int32_t b = (int32_t)(uint8_t)s1[0x98];
      *(uint8_t *)guest_at(0x801befec) = 0xff;
      *(uint8_t *)guest_at(0x801c4582) = 0xff;
      *(uint8_t *)guest_at(0x801befa6) = 0xff;
      *(uint8_t *)guest_at(0x801c4581) = 0xff;
      *(uint8_t *)guest_at(0x801e3d00) = (uint8_t)b;
      if (s2 == 2) {
        *(int16_t *)(s1 + 0x8c) = -1;
        return 1;
      }
      *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
      *(uint32_t *)guest_at(0x801c11d8) = g32(0x801c47b8);
      *(uint16_t *)(s1 + 0x8e) = 0;
      s0 = 0x40;
      goto st40;
    }

st40:
    if (FUN_80171890(a0,s4,s3) == 0) {
      goto store0;
    }
    *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
    *(uint32_t *)guest_at(0x801c15f4) = g32(0x801c47b8);
    s0 = 0x50;
    *(uint8_t *)guest_at(0x801c4581) = s4[3];
    *(uint8_t *)guest_at(0x801befa6) = s4[4];
    *(uint16_t *)(s1 + 0x8e) = 0;
    /* fall into st50 */

st50:
    if (FUN_80171890(a0,s5,s3) == 0) {
      goto store0;
    }
    *(uint8_t *)guest_at(0x801c4582) = s5[3];
    *(uint8_t *)guest_at(0x801befec) = s5[4];
    *(uint16_t *)(s1 + 0x8e) = 0;
    /* fall into stf00 */

stf00:
    *(int16_t *)(s1 + 0x8c) = -1;
    {
      int32_t x = (int32_t)*(int16_t *)(s1 + 0x20);
      int32_t y = (int32_t)*(int16_t *)(s1 + 0x22);
      FUN_80195290(x / 2, y / 2);
      FUN_801952b0((int32_t)*(int16_t *)(s1 + 0x24));
    }
    return 1;

store0:
    *(int16_t *)(s1 + 0x8c) = (int16_t)s0;
    return 0;

dispatch:
    continue;
  }
}


/* ---- FUN_80172a34 @0x80172a34 (877 insns) ----
   Fighter/menu animation-table init. a0!=0 -> immediate return.
   Builds records in two arena buffers (*(u32*)0x801f5210/14) plus
   scratchpad tables at 0x1f8000e8. */
extern uint32_t FUN_801a4e2c(uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t FUN_801a4ebc(uint32_t,uint32_t,uint32_t);
extern void FUN_801a42d0(uint32_t);
extern void FUN_80194060(uint32_t,uint32_t);
extern void FUN_80194090(uint32_t,uint32_t);
extern void FUN_80175708(uint32_t,int32_t,int32_t,int32_t,int32_t);
extern void FUN_80175760(uint32_t,int32_t,int32_t,int32_t,int32_t);
extern void FUN_801756b0(uint32_t,int32_t,int32_t,int32_t,int32_t);
extern void FUN_80148e70(uint32_t,uint32_t,int32_t,int32_t,int32_t,int32_t);
extern uint32_t FUN_80172a04(uint32_t);
extern uint32_t FUN_801725a4(uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t FUN_80172640(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);

/* record byte-fill: d = rec+0xe style pointer; s = 4-byte src table */
static void br2_72a34_fill(uint8_t *d, uint32_t s)
{
  uint8_t b0 = g8(s);
  uint8_t b1 = g8(s + 1);
  uint8_t b2 = g8(s + 2);
  uint8_t b3 = g8(s + 3);
  d[-2] = b0;
  d[-1] = b1;
  d[6]  = (uint8_t)(b0 + b2);
  d[7]  = b1;
  d[0xe]= b0;
  d[0xf]= (uint8_t)(b1 + b3);
  d[0x16]=(uint8_t)(b0 + b2);
  d[0x17]=(uint8_t)(b1 + b3);
}
void FUN_80172a34(int32_t a0)
{
  uint32_t s5, s3i, s4i;
  uint8_t *rec;
  uint32_t buf1, buf2, bp;
  int32_t i, j, k;

  if (a0 != 0) return;

  FUN_801a4e2c(0,2,0x1c00,0x801e95ec);
  FUN_801a4e2c(0,3,0x400,0x801e95ec + 0x1c00);
  FUN_801a42d0(0);
  *(uint32_t *)guest_at(0x801f4f28) = 0;
  *(uint32_t *)guest_at(0x801f4f2c) = 0;
  *(uint32_t *)guest_at(0x801f5210) = FUN_801a4ebc(0,2,0x898);
  *(uint32_t *)guest_at(0x801f5214) = FUN_801a4ebc(0,2,0x898);

  /* ---- block A: 0x801f4f30 region, two passes s5=0,0xc0 ---- */
  for (s3i = 0; s3i < 2; s3i++) {
    s5 = s3i * 0xc0;
    /* 8 records stride 0x18 at 0x801f4f30+s5 */
    for (s4i = 0; s4i < 8; s4i++) {
      rec = (uint8_t *)guest_at(0x801f4f30 + s5 + s4i * 0x18);
      rec[3] = 5;
      rec[7] = 0x28;
      FUN_80194060(0x801f4f30 + s5 + s4i * 0x18,0);
      FUN_80194090(0x801f4f30 + s5 + s4i * 0x18,1);
    }
    /* 2 records at 0x801f4f30+s5 stride 0x18 via FUN_801756b0,
       src table 0x801b4cbc step 8 */
    for (s4i = 0; s4i < 2; s4i++) {
      uint32_t t = 0x801b4cbc + s4i * 8;
      rec = (uint8_t *)guest_at(0x801f4f30 + s5 + s4i * 0x18);
      rec[4] = 2; rec[5] = 2; rec[6] = 9;
      FUN_801756b0(0x801f4f30 + s5 + s4i * 0x18,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
    }
    /* 2 records at 0x801f4f60+s5, table 0x801b4cdc */
    for (s4i = 0; s4i < 2; s4i++) {
      uint32_t t = 0x801b4cdc + s4i * 8;
      rec = (uint8_t *)guest_at(0x801f4f60 + s5 + s4i * 0x18);
      rec[4] = 2; rec[5] = 2; rec[6] = 9;
      FUN_801756b0(0x801f4f60 + s5 + s4i * 0x18,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
    }
    /* 2 records at 0x801f4f90+s5: {0xff,0,0} */
    for (s4i = 0; s4i < 2; s4i++) {
      rec = (uint8_t *)guest_at(0x801f4f90 + s5 + s4i * 0x18);
      rec[4] = 0xff; rec[5] = 0; rec[6] = 0;
    }
    /* 2 records at 0x801f4fc0+s5: {0,0,0x80} */
    for (s4i = 0; s4i < 2; s4i++) {
      rec = (uint8_t *)guest_at(0x801f4fc0 + s5 + s4i * 0x18);
      rec[4] = 0; rec[5] = 0; rec[6] = 0x80;
    }
  }

  /* ---- block B: 0x801f50b0 region, two passes offset 0,0x90 ---- */
  for (s3i = 0; s3i < 2; s3i++) {
    uint32_t off = s3i * 0x90;
    /* 4 records stride 0x24 at 0x801f50b0+off */
    for (s4i = 0; s4i < 4; s4i++) {
      uint32_t a = 0x801f50b0 + off + s4i * 0x24;
      rec = (uint8_t *)guest_at(a);
      rec[3] = 8; rec[7] = 0x38;
      FUN_80194060(a,0);
      FUN_80194090(a,1);
    }
    /* 2 records stride 0x24, FUN_80175708, table 0x801b4ccc */
    for (s4i = 0; s4i < 2; s4i++) {
      uint32_t t = 0x801b4ccc + s4i * 8;
      uint32_t a = 0x801f50b0 + off + s4i * 0x24;
      rec = (uint8_t *)guest_at(a + 0x1e);
      rec[-0x1a]=0xff; rec[-0x19]=0xf0; rec[-0x18]=0x20;
      rec[-0x12]=0xff; rec[-0x11]=0xf0; rec[-0x10]=0x20;
      rec[-0xa]=0xa0;  rec[-9]=0x60;   rec[-8]=0;
      rec[-2]=2;      rec[-1]=2;      rec[0]=9;
      FUN_80175708(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
    }
    /* 2 records at 0x801f50f8+off, table 0x801b4cec, a2 += flag */
    for (s4i = 0; s4i < 2; s4i++) {
      uint32_t t = 0x801b4cec + s4i * 8;
      uint32_t a = 0x801f50f8 + off + s4i * 0x24;
      int32_t fl = (g8(0x1f800015) < 1) ? 1 : 0;
      rec = (uint8_t *)guest_at(a + 0x1e);
      rec[-0x1a]=0x20; rec[-0x19]=0xf0; rec[-0x18]=0xff;
      rec[-0x12]=0x20; rec[-0x11]=0xf0; rec[-0x10]=0xff;
      rec[-0xa]=0;    rec[-9]=0x40;   rec[-8]=0xc0;
      rec[-2]=0;      rec[-1]=0x40;   rec[0]=0xc0;
      FUN_80175708(a,(int32_t)gs16(t),
                   (int32_t)gs16(t+2)+fl,
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
    }
  }

  /* ---- block C: per-buffer init, 2 iterations over
     {*(u32*)0x801f5210, *(u32*)0x801f5214} ---- */
  for (i = 0; i < 2; i++) {
    uint32_t slot = 0x801f5210 + i * 4;
    bp = g32(slot);
    /* C1: 0x37 records stride 0x28 */
    for (j = 0; j < 0x37; j++) {
      uint8_t *r = (uint8_t *)guest_at(bp + j * 0x28);
      r[3] = 9; r[7] = 0x2c;
      FUN_80194060(bp + (uint32_t)j * 0x28,0);
      FUN_80194090(bp + (uint32_t)j * 0x28,1);
      r[4] = 0x7f; r[5] = 0x7f; r[6] = 0x7f;
    }
    /* C2: 2 records at bp+0x5c8, FUN_80175760 + fill from 0x801b4c98 */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c88 + j * 8;
      uint32_t a = bp + 0x5c8 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a + 0xe);
      FUN_80175760(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      br2_72a34_fill(rec,0x801b4c98);
      *(uint16_t *)(rec) = 0x789e;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C3: FUN_80148e70 on bp, table 0x801b4b48, then fills @0x801b4b50 */
    FUN_80148e70(bp,0x1f800000,
                 (int32_t)gs16(0x801b4b48),(int32_t)gs16(0x801b4b4a),
                 (int32_t)gs16(0x801b4b4c),(int32_t)gs16(0x801b4b4e));
    {
      uint8_t *d = (uint8_t *)guest_at(bp);
      uint32_t s = 0x801b4b50;
      d[0xc]=g8(s);   d[0xd]=g8(s+1);
      d[0x14]=(uint8_t)(g8(s)+g8(s+2)); d[0x15]=g8(s+1);
      d[0x1c]=g8(s);  d[0x1d]=(uint8_t)(g8(s+1)+g8(s+3));
      d[0x24]=(uint8_t)(g8(s)+g8(s+2));
      d[0x25]=(uint8_t)(g8(s+1)+g8(s+3));
      *(uint16_t *)(d + 0xe) = 0x781e;
      *(uint16_t *)(d + 0x16) = 0xe;
    }
    /* C4: 2 records bp+0x28 stride 0x28, FUN_80148e70, table 0x801b4b5c */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4b5c + j * 8;
      uint32_t a = bp + 0x28 + (uint32_t)j * 0x28;
      FUN_80148e70(a,0x1f800000,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      *(uint16_t *)((uint8_t *)guest_at(a) + 0x16) = 0xf;
    }
    /* C5: nested fp<2 { s4<2 { s7<8 } } — 32 records from bp+0x78,
       FUN_80175760, tables 0x801b4a48 (lh, step8) and
       0x801b3190 (bytes, step4, reset each s4 group) */
    {
      uint32_t t6 = 0x801b4a48;
      uint32_t aoff = 0x78;
      for (k = 0; k < 2; k++) {          /* fp */
        uint32_t s1b = 0x801b3190;
        for (j = 0; j < 2; j++) {        /* s4 group */
          for (i = 0; i < 8; i++) {      /* s7 records */
            uint32_t a = bp + aoff;
            uint8_t *d = (uint8_t *)guest_at(a + 0xe);
            FUN_80175760(a,(int32_t)gs16(t6),
                         (int32_t)gs16(t6+2),(int32_t)gs16(t6+4),
                         (int32_t)gs16(t6+6));
            br2_72a34_fill(d,s1b);
            *(uint16_t *)d = 0x781e;
            *(uint16_t *)(d + 8) = 0xe;
            t6 += 8; s1b += 4; aoff += 0x28;
          }
        }
      }
    }
    /* C6: 8 records bp+0x6b8 stride 0x28, FUN_80175760,
       table 0x801b4be4, lhu id from 0x801b4be0, fill 0x801b4c24 */
    {
      uint16_t id = g16(0x801b4be0);
      for (j = 0; j < 8; j++) {
        uint32_t t = 0x801b4be4 + j * 8;
        uint32_t a = bp + 0x6b8 + (uint32_t)j * 0x28;
        rec = (uint8_t *)guest_at(a + 0xe);
        FUN_80175760(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
                     (int32_t)gs16(t+4),(int32_t)gs16(t+6));
        br2_72a34_fill(rec,0x801b4c24);
        *(uint16_t *)rec = id;
        *(uint16_t *)(rec + 8) = 0xe;
      }
    }
    /* C7: 2 records bp+0x626 stride 0x28, fill 0x801b4ca4, id 0x78de */
    for (j = 0; j < 2; j++) {
      uint32_t a = bp + 0x626 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a);
      br2_72a34_fill(rec,0x801b4ca4);
      *(uint16_t *)rec = 0x78de;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C8: 2 records bp+0x578 stride 0x28, FUN_80175760,
       table 0x801b4c30, fill 0x801b4c60, id 0x791d */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c30 + j * 8;
      uint32_t a = bp + 0x578 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a + 0xe);
      FUN_80175760(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      br2_72a34_fill(rec,0x801b4c60);
      *(uint16_t *)rec = 0x791d;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C9: 2 records bp+0x676 stride 0x28, fill 0x801b4cb8, id 0x781e */
    for (j = 0; j < 2; j++) {
      uint32_t a = bp + 0x676 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a);
      br2_72a34_fill(rec,0x801b4cb8);
      *(uint16_t *)rec = 0x781e;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C10: 2 records bp+0x7f8 stride 0x28, FUN_80148e70,
       table 0x801b4c64; writes id 0x18/0x79dd at bp+0x806 */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c64 + j * 8;
      uint32_t a = bp + 0x7f8 + (uint32_t)j * 0x28;
      uint8_t *w = (uint8_t *)guest_at(bp + 0x806 + (uint32_t)j * 0x28);
      FUN_80148e70(a,0x1f800000,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      *(uint16_t *)(w + 8) = 0x18;
      *(uint16_t *)w = 0x79dd;
    }
    /* C11: 2 records bp+0x848 stride 0x28, FUN_80148e70,
       table 0x801b4c74, fill 0x801b4c84, id 0x781e/0x18 */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c74 + j * 8;
      uint32_t a = bp + 0x848 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a + 0xe);
      FUN_80148e70(a,0x1f800000,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      br2_72a34_fill(rec,0x801b4c84);
      *(uint16_t *)rec = 0x781e;
      *(uint16_t *)(rec + 8) = 0x18;
    }
  }

  /* ---- block D: scratchpad fighter table 0x1f8000e8, 2 records ---- */
  for (i = 0; i < 2; i++) {
    uint32_t s0 = 0x1f8000e8 + (uint32_t)i * 0x108;
    uint8_t *d = (uint8_t *)guest_at(0x801f51d8 + (uint32_t)i * 0x1c);
    uint16_t v;
    v = (uint16_t)(int32_t)gs16(s0 + 0xac);
    *(uint32_t *)(d + 8) = v;
    *(uint32_t *)(d + 0xc) = v;
    v = g16(s0 + 0xaa);
    *(uint16_t *)(d + 6) = 0;
    {
      uint32_t rr = FUN_80172a04(s0);
      *(uint16_t *)(d + 0x10) = v;
      *(uint16_t *)(d + 0x14) = (uint16_t)rr;
    }
    v = g16(s0 + 0xd6);
    *(uint16_t *)(d + 0) = 1;
    *(uint16_t *)(d + 0x16) = 0;
    *(uint16_t *)(d + 0x1a) = 0xffff;
    *(uint16_t *)(d + 0x12) = v;
    {
      int32_t dv = (int32_t)gs16(s0 + 0xb0);
      uint32_t q = (dv == 0) ? 0xffffffffu : (uint32_t)(100 / dv);
      *(uint32_t *)guest_at(0x801f5228 + (uint32_t)i * 4) = q;
      *(uint16_t *)guest_at(0x801f524c + (uint32_t)i * 2) = 0;
    }
  }

  /* ---- block E: per-record FUN_801725a4 over both buffers ---- */
  for (i = 0; i < 8; i++) {
    uint32_t off = 0x6b8 + (uint32_t)i * 0x28;
    uint32_t b1 = g32(0x801f5210);
    uint32_t b2 = g32(0x801f5214);
    FUN_801725a4(b1 + off + 0xe, b2 + off + 0xe,
                 0x801b4cfc, 0x801b4d08);
  }

  /* ---- block F: FUN_80172640 over both buffers ---- */
  for (i = 0; i < 2; i++) {
    uint32_t off = 0x578 + (uint32_t)i * 0x28;
    uint32_t b1 = g32(0x801f5210);
    uint32_t b2 = g32(0x801f5214);
    uint32_t r = FUN_80172640(b1 + off, b2 + off, 0, 0x801b4d34, 0x801b4d28);
    *(uint32_t *)guest_at(0x801f51d0 + (uint32_t)i * 4) = r;
    *(uint32_t *)guest_at(0x801f5218 + (uint32_t)i * 4) = 0;
  }
  *(uint32_t *)guest_at(0x801f4f20) = 0;
}


/* ---- FUN_8016d478 @0x8016d478 (409 insns) ----
   a0 = guest ptr (byte 0x9f cleared). Two passes building sprite/
   anim descriptor chains at 0x801f4cbc.. and 0x801f4d7c.. */
extern void FUN_801941e0(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void FUN_80194180(uint32_t,const void *,uint32_t,uint32_t);
extern void FUN_80191f08(uint32_t,const void *);

static void br2_6d478_tag(uint32_t dst, uint32_t src)
{
  uint32_t v = g32(dst);
  *(uint32_t *)guest_at(dst) = (v & 0xff000000u) | (src & 0xffffffu);
}

void FUN_8016d478(uint32_t a0)
{
  uint32_t sp20 = 0, sp24, sp28, sp2c = 0;
  uint32_t s4t;
  int i;
  uint16_t d18[4];

  /* pass 1: records at 0x801f4cbc + i*0x58 */
  for (i = 0; i < 2; i++) {
    uint32_t s5 = (uint32_t)i * 0x58;
    uint32_t s6 = 0x801f4cbc + s5;
    uint32_t s3 = 0x801f4cc8 + s5;
    uint32_t s1 = 0x801f4ce0 + s5;
    uint32_t s0 = 0x801f4cec + s5;
    uint32_t s2 = 0x801f4cfc + s5;
    uint32_t s7 = 0x801f4d08 + s5;
    s4t = 0x801e40a0 + (uint32_t)i * 0x2150;

    sp20 += 1;
    FUN_801941e0(s6,1,g8(s4t + 0x16),0x40,0);
    d18[0]=0x100; d18[1]=0x1f0; d18[2]=0x60; d18[3]=0x10;
    FUN_80194180(s3,d18,0x100,0x1e0);
    d18[0]=0x100; d18[1]=0x1e0; d18[2]=0x60; d18[3]=0x10;
    FUN_80191f08(s1,d18);
    {
      uint8_t *r = (uint8_t *)guest_at(s0);
      r[7]=0x60; *(uint16_t *)(r+8)=0x100; *(uint16_t *)(r+0xa)=0x1e0;
      *(uint16_t *)(r+0xc)=0x60; *(uint16_t *)(r+0xe)=0x10;
      r[3]=3; r[4]=0; r[5]=0; r[6]=0; r[7]=0x62;
    }
    FUN_801941e0(s2,g8(s4t + 0x17),g8(s4t + 0x16),0,0);
    FUN_80191f08(s7,(const void *)(uintptr_t)s4t);
    br2_6d478_tag(s6,s3);
    br2_6d478_tag(s3,s1);
    br2_6d478_tag(s1,s0);
    br2_6d478_tag(s0,s2);
    br2_6d478_tag(s2,s7);
  }

  /* init + pass 2 setup */
  sp24 = 0x801f4e84;
  sp28 = 0x801f4d7c;
  *(uint16_t *)guest_at(0x801f4cb0) = 0;
  {
    uint8_t *v1 = (uint8_t *)guest_at(0x801f4cb0);
    *(uint16_t *)(v1 + 8) = 0xffff;
    *(uint16_t *)(v1 + 2) = 0;
    *(uint16_t *)(v1 + 4) = 0;
    *(uint16_t *)(v1 + 6) = 0;
    *(uint16_t *)(v1 + 0xa) = 0xfffe;
  }
  sp20 = 0;
  sp2c = 0;
  g8(0); /* keep helper referenced */
  *(uint8_t *)guest_at(a0 + 0x9f) = 0;

  for (i = 0; i < 2; i++) {
    uint32_t s7 = (uint32_t)i * 0x58;
    /* half 1: records sp28+0xc/+0x24/+0x30/+0x40/+0x4c/+0xbc */
    uint32_t h1_941e0 = sp28;
    uint32_t h1_9180  = 0x801f4d88 + s7;
    uint32_t h1_91f0a = 0x801f4da0 + s7;
    uint32_t h1_fill  = 0x801f4dac + s7;
    uint32_t h1_e0    = 0x801f4dbc + s7;
    uint32_t h1_f08   = 0x801f4dc8 + s7;
    /* half 2: records sp28+0xc8/+0xe0/+0xec/+0xfc/+0x4c/+0xbc,
       91f08 target sp24 */
    uint32_t h2_9180  = 0x801f4e44 + s7;
    uint32_t h2_91f0a = 0x801f4e5c + s7;
    uint32_t h2_fill  = 0x801f4e68 + s7;
    uint32_t h2_e0    = 0x801f4e78 + s7;
    uint32_t h2_f08   = 0x801f4dc8 + s7;
    uint32_t h2_e0c   = 0x801f4e38 + s7;
    s4t = 0x801e40a0 + sp2c;

    FUN_801941e0(h1_941e0,1,g8(s4t + 0x16),0x40,0);
    d18[0]=0; d18[1]=0x1f0; d18[2]=0x20; d18[3]=0x10;
    FUN_80194180(h1_9180,d18,0,0x1e0);
    d18[0]=0; d18[1]=0x1e0; d18[2]=0x20; d18[3]=0x10;
    FUN_80191f08(h1_91f0a,d18);
    {
      uint8_t *r = (uint8_t *)guest_at(h1_fill);
      r[7]=0x60; *(uint16_t *)(r+8)=0; *(uint16_t *)(r+0xa)=0x1e0;
      *(uint16_t *)(r+0xc)=0x20; *(uint16_t *)(r+0xe)=0x10;
      r[3]=3; r[4]=0; r[5]=0; r[6]=0; r[7]=0x62;
    }
    FUN_801941e0(h1_e0,g8(s4t + 0x17),g8(s4t + 0x16),0,0);
    FUN_80191f08(h1_f08,(const void *)(uintptr_t)s4t);
    br2_6d478_tag(h1_941e0,h1_9180);
    br2_6d478_tag(h1_9180,h1_91f0a);
    br2_6d478_tag(h1_91f0a,h1_fill);
    br2_6d478_tag(h1_fill,h1_e0);
    br2_6d478_tag(h1_e0,h1_f08);

    FUN_801941e0(h2_e0c,0,g8(s4t + 0x16),0x40,0);
    d18[0]=0x30; d18[1]=0x1f0; d18[2]=0x20; d18[3]=0x10;
    FUN_80194180(h2_9180,d18,0,0x1e0);
    d18[0]=0x30; d18[1]=0x1e0; d18[2]=0x20; d18[3]=0x10;
    FUN_80191f08(h2_91f0a,d18);
    {
      uint8_t *r = (uint8_t *)guest_at(h2_fill);
      r[7]=0x60; *(uint16_t *)(r+8)=0x30; *(uint16_t *)(r+0xa)=0x1e0;
      *(uint16_t *)(r+0xc)=0x20; *(uint16_t *)(r+0xe)=0x10;
      r[3]=3; r[4]=0; r[5]=0; r[6]=0; r[7]=0x62;
    }
    FUN_801941e0(h2_e0,g8(s4t + 0x17),g8(s4t + 0x16),0,0);
    FUN_80191f08(sp24,(const void *)(uintptr_t)s4t);
    br2_6d478_tag(h2_e0c,h2_9180);
    br2_6d478_tag(h2_9180,h2_91f0a);
    br2_6d478_tag(h2_91f0a,h2_fill);
    br2_6d478_tag(h2_fill,h2_e0);
    br2_6d478_tag(h2_e0,sp24);

    sp2c += 0x2150;
    sp20 += 1;
    sp28 += 0x58;
    sp24 += 0x58;
  }

  /* final stores */
  *(uint16_t *)guest_at(0x801f4e2c) = 0;
  *(uint16_t *)guest_at(0x801f4d70) = 0;
  *(uint16_t *)guest_at(0x801f4e34) = 0xffff;
  *(uint16_t *)guest_at(0x801f4d78) = 0xffff;
  *(uint16_t *)guest_at(0x801f4e2e) = 0;
  *(uint16_t *)guest_at(0x801f4d72) = 0;
  *(uint16_t *)guest_at(0x801f4e30) = 0;
  *(uint16_t *)guest_at(0x801f4d74) = 0;
  *(uint16_t *)guest_at(0x801f4e32) = 0;
  *(uint16_t *)guest_at(0x801f4d76) = 0;
  *(uint16_t *)guest_at(0x801f4e36) = 0xffff;
  *(uint16_t *)guest_at(0x801f4d7a) = 0xffff;
}


/* ---- FUN_8016f588 @0x8016f588 (413 insns) ----
   Debug/status text display. a0 = guest char/state ptr,
   a1 = selector 0..15 (jump table 0x80122590). */
extern void print_dispatch(const char *,...);
extern void FUN_8012cd9c(int32_t,int32_t);
extern void FUN_8016f30c(uint32_t,uint32_t);
extern void FUN_8017f13c(uint32_t,uint32_t);
extern void FUN_8018106c(uint32_t);
extern void FUN_8018123c(uint32_t);
extern void FUN_801863ac(uint32_t);
extern void FUN_8012f164(uint32_t);

static int32_t br2_6f588_div360(int32_t v)
{ /* v*360 / 4096 trunc toward zero (MIPS mult+sra pattern) */
  int32_t t = v * 360;
  return t / 4096;
}
static int32_t br2_6f588_div100_256(int32_t v)
{
  int32_t t = v * 100;
  return t / 256;
}
static int32_t br2_6f588_div16(int32_t v)
{ /* (v<0 ? v+0xf : v) >> 4  == trunc(v/16) */
  return v / 16;
}

void FUN_8016f588(uint32_t a0,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint8_t *s2;
  uint32_t s1 = a1;
  uint32_t s3 = 0x801f04a8;
  int32_t i;

  /* s2 = lw 0x38(s0): guest ptr stored in state block */
  s2 = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x38));

  if (s0[2] == 0) {
    FUN_8012cd9c(0,0x20);
    if (s0[2] == 0) goto sel;
  }
  FUN_8012cd9c(0,0x60);
sel:
  if (s1 != 0)
    print_dispatch((const char *)guest_at(0x801223d0),(int32_t)s0[2] + 1);
  if (s1 >= 0x10) return;

  switch (s1) {
  case 0: return;
  case 1: {
    int32_t b0 = (int32_t)*(int16_t *)(s0 + 0xb0);
    int32_t t0 = (int32_t)*(int16_t *)(s0 + 0xd6);
    if (t0 < 0) t0 += 0x3f;
    print_dispatch((const char *)guest_at(0x801223e0),
                 b0 - (int32_t)*(int16_t *)(s0 + 0xac),
                 b0 - (int32_t)*(int16_t *)(s0 + 0xaa),
                 b0, t0 >> 6);
    return;
  }
  case 2: {
    int32_t v;
    FUN_8016f30c(a0,0);
    FUN_8017f13c(a0,0);
    FUN_8017f13c(a0,3);
    print_dispatch((const char *)guest_at(0x801223fc),
                 (int32_t)(uint8_t)s0[0xda],(int32_t)(uint8_t)s0[0xdd]);
    v = (int32_t)*(int16_t *)(s0 + 0xc6) * 360 / 4096;
    {
      int32_t w = (int32_t)(uint8_t)s0[0xdb] * 5760;
      int32_t a3 = (int32_t)*(int16_t *)(s2 + 0x3a);
      print_dispatch((const char *)guest_at(0x80122410),v,w/4096,a3);
    }
    return;
  }
  case 3: {
    int32_t v1,v0;
    FUN_8017f13c(a0,0);
    FUN_8017f13c(a0,4);
    v1 = (int32_t)*(int16_t *)(s0 + 0xcc);
    if (v1 < 0) v1 += 0xf;
    v0 = v1 >> 4;
    print_dispatch((const char *)guest_at(0x8012242c),
                 (int32_t)*(int16_t *)(s0 + 0xca),
                 (int32_t)*(int16_t *)(s0 + 0xc2),
                 (int32_t)*(int16_t *)(s0 + 0xbe), v0);
    {
      int32_t a1 = (int32_t)*(int16_t *)(s0 + 0xc0);
      int32_t a3;
      if (a1 < 0) a1 += 0xf;
      a1 >>= 4;
      a3 = (int32_t)((uint32_t)*(uint16_t *)(s0 + 0xb2) << 16) >> 17;
      print_dispatch((const char *)guest_at(0x80122448),a1,
                   (int32_t)*(int16_t *)(s0 + 0xd2),a3);
    }
    return;
  }
  case 4: FUN_8018106c(a0); return;
  case 5: FUN_8018123c(a0); return;
  case 6: FUN_8016f30c(a0,1); return;
  case 7: {
    uint8_t *q = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x20));
    int32_t a1 = br2_6f588_div100_256(*(int32_t *)(q + 0x34));
    int32_t a2 = br2_6f588_div100_256(*(int32_t *)(q + 0x38));
    int32_t a3 = br2_6f588_div100_256(*(int32_t *)(q + 0x3c));
    print_dispatch((const char *)guest_at(0x80122460),a1,a2,a3);
    q = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x20));
    a1 = br2_6f588_div100_256(*(int32_t *)(q + 0x174));
    a2 = br2_6f588_div100_256(*(int32_t *)(q + 0x178));
    a3 = br2_6f588_div100_256(*(int32_t *)(q + 0x17c));
    print_dispatch((const char *)guest_at(0x80122478),a1,a2,a3);
    print_dispatch((const char *)guest_at(0x80122490),
                 (int32_t)*(int16_t *)(s0 + 0x8c),
                 (int32_t)*(int16_t *)(s0 + 0x8a),
                 (int32_t)*(int16_t *)(s0 + 0xb2),
                 (int32_t)*(int16_t *)(s0 + 0xb4));
    print_dispatch((const char *)guest_at(0x801224a8),
                 (int32_t)*(int16_t *)(s0 + 0x98),
                 (int32_t)*(int16_t *)(s0 + 0x96),
                 (int32_t)*(int16_t *)(s0 + 0x90),
                 (int32_t)*(int16_t *)(s0 + 0x92));
    return;
  }
  case 8: {
    int32_t a1 = br2_6f588_div360(*(int16_t *)(s0 + 0x4e));
    print_dispatch((const char *)guest_at(0x801224c0),a1);
    a1 = br2_6f588_div360(*(int16_t *)(s0 + 0x70));
    {
      int32_t a2 = br2_6f588_div360(*(int16_t *)(s0 + 0x74));
      int32_t a3 = br2_6f588_div100_256(*(int16_t *)(s0 + 0x76));
      print_dispatch((const char *)guest_at(0x801224c8),a1,a2,a3);
    }
    return;
  }
  case 9:
    FUN_8012f164(g32(0x1f8000bc));
    return;
  case 10: {
    uint8_t *q = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x38));
    int32_t a1 = br2_6f588_div16(*(int16_t *)(q + 0xc6));
    int32_t a2 = br2_6f588_div16(*(int16_t *)(q + 0xca));
    int32_t a3 = br2_6f588_div16(*(int16_t *)(q + 0xc8));
    int32_t s10 = br2_6f588_div16(*(int16_t *)(q + 0xc4));
    int32_t s14 = br2_6f588_div16(*(int16_t *)(q + 0xc2));
    print_dispatch((const char *)guest_at(0x801224dc),a1,a2,a3,s10,s14);
    return;
  }
  case 11: {
    uint8_t *q = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x38));
    print_dispatch((const char *)guest_at(0x80122508),
                 (int32_t)*(int16_t *)(q + 0x80),
                 (int32_t)*(int16_t *)(q + 0x14),
                 (int32_t)*(int16_t *)(q + 0x1c));
    print_dispatch((const char *)guest_at(0x8012251c),
                 (int32_t)*(int16_t *)(q + 0x24),
                 (int32_t)*(int16_t *)(q + 0x3c),
                 (int32_t)*(int16_t *)(q + 0xa2));
    print_dispatch((const char *)guest_at(0x80122530),
                 (int32_t)(uint8_t)q[0x100],(int32_t)(uint8_t)q[0x9c]);
    return;
  }
  case 12:
    FUN_8017f13c(a0,0); FUN_8017f13c(a0,1); FUN_8017f13c(a0,2);
    FUN_8017f13c(a0,3); FUN_8017f13c(a0,4); FUN_8017f13c(a0,5);
    return;
  case 13: FUN_801863ac(a0); return;
  case 14:
    print_dispatch((const char *)guest_at(0x80122540),
                 (int32_t)*(int16_t *)guest_at(s3 + 0x3c),
                 (int32_t)*(int16_t *)guest_at(s3 + 0x3e));
    print_dispatch((const char *)guest_at(0x8012254c),
                 (int32_t)*(int16_t *)guest_at(s3 + 0x40),
                 (int32_t)*(int16_t *)guest_at(s3 + 0x42));
    return;
  case 15: {
    uint8_t *q = s2;
    print_dispatch((const char *)guest_at(0x80122558),
                 (int32_t)(uint8_t)q[0x7c] + 1,
                 (int32_t)(uint8_t)q[0x7d] + 1,
                 (int32_t)*(int16_t *)(q + 0x3a));
    print_dispatch((const char *)guest_at(0x80122574),
                 (int32_t)(int8_t)q[0x7e]);
    if ((*(uint32_t *)(s0 + 0x5c) & 0x40) != 0) {
      print_dispatch((const char *)guest_at(0x80122580));
      if (q[0x7d] != 0) {
        for (i = 0; i < (int32_t)(uint8_t)q[0x7d]; i++)
          print_dispatch((const char *)guest_at(0x80122584),
                       (int32_t)*(int16_t *)(q + 0x3c + i * 2));
      }
      print_dispatch((const char *)guest_at(0x8012258c));
    }
    return;
  }
  }
}

/* ---- FUN_8016e21c @0x8016e21c (1066 insns) ----
   Per-frame HUD/status-bar state machine. a0 = mode (0,1,2).
   s5 = scratchpad 0x1f800000; fp = display state. */
extern int FUN_8015e9fc(uint32_t,uint32_t);
extern void FUN_8015143c(uint32_t,uint32_t,uint32_t);
extern uint32_t FUN_8018c824();

static int32_t br2_e21c_s16(uint32_t v){ return (int16_t)v; }
static int32_t br2_e21c_divm(int32_t a0)
{ /* mult 0x88888889; mfhi; +a0; sra 5; - sign(a0) : exact MIPS seq */
  int64_t t = (int64_t)a0 * (int64_t)(int32_t)0x88888889;
  int32_t hi = (int32_t)(t >> 32);
  return ((hi + a0) >> 5) - (a0 >> 31);
}
static int32_t br2_e21c_shr4(int32_t v)
{ return v < 0 ? (v + 0xf) >> 4 : v >> 4; }
static int32_t br2_e21c_div3_4(int32_t v)
{ return v < 0 ? (v + 3) >> 2 : v >> 2; }

void FUN_8016e21c(uint32_t a0)
{
  uint8_t *g  = (uint8_t *)guest_at(0x1f800000);
  uint8_t *sp20, *sp24, *c0;
  uint32_t fp = 0, mode = a0;
  uint32_t sp48 = 0, sp4c = 0, sp50 = 0, sp54 = 0;
  uint32_t sp58, sp60, sp64, sp68, sp6c, idx;
  int32_t s7 = 0;
  int32_t i, t0, v0, v1, a1, a2, a3, s6, s1, s0v, s2, s3;
  uint32_t ctx8, ctxc;
  uint16_t *w20, *w22;

  sp58 = (((uint32_t)*(uint16_t *)(g + 0x92) & 0xf00) ^ 0x100) == 0;
  for (;;) {
    sp20 = (uint8_t *)guest_at(sp58 ? 0x1f8000e8 : 0x1f8001f0);
    sp24 = (uint8_t *)guest_at(*(uint32_t *)(sp20 + 0x2c));
    idx   = sp20[3];
    sp64  = 0x801b46d8 + idx * 25;
    sp60  = idx * 25;
    sp68  = idx * 25 + 1;
    sp6c  = idx * 25 + 2;
    FUN_80136ddc();
    FUN_801274fc(0,0,0);
    ctx8  = *(uint32_t *)(g + 8);
    ctxc  = *(uint32_t *)(g + 0xc);
    w20   = (uint16_t *)(g + 0x20);
    w22   = (uint16_t *)(g + 0x22);

    if (fp == 2) goto case2;
    if (fp < 3) {
      if (fp == 0) goto case0;
      if (fp == 1) goto case1;
      goto poll;
    }
    if (fp == 0xf0) { FUN_80156ef4(0x203,0,0); fp = 0xff; goto poll; }
    if (fp == 0xf1) { fp = 0xff; goto poll; }
    goto poll;

case0:
    if (FUN_8012aaa8() != 0) goto poll;
    FUN_8015e87c();
    FUN_8015e88c();
    *(uint16_t *)(g + 0x28) = 0;
    *(uint16_t *)(g + 0x30) = 0;
    *(uint16_t *)(g + 0x40) = 0;
    if (mode == 0) {
      uint32_t n = *(uint8_t *)guest_at(0x801b461c + idx);
      uint8_t *t64 = (uint8_t *)guest_at(sp64);
      *(uint16_t *)(g + 0x30) = (uint16_t)(*t64 << 3);
      if ((int32_t)fp < (int32_t)n) {
        c0 = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc0));
        c0[0x10] = (uint8_t)n;
        for (i = 0; i < (int32_t)n; i++) {
          *(uint8_t *)guest_at(0x801f47a0 + i) =
            *(uint8_t *)guest_at(0x801b4680 + idx * 8 + i);
          *(uint8_t *)guest_at(0x801f4770 + i) =
            *(uint8_t *)guest_at(0x801b4628 + idx * 8 + i);
        }
      }
      fp++;
    } else if (mode == 1) {
      uint32_t b5 = sp20[0xe4], b4 = sp24[0xe4];
      int32_t a4;
      if (b4 < b5) {
        a4 = br2_e21c_s16(*(uint16_t *)(g + 0xa4));
        v0 = *(uint8_t *)guest_at(0x801b46d8 + a4 * 3 + sp6c);
        sp48 = 0;
      } else {
        a4 = br2_e21c_s16(*(uint16_t *)(g + 0xa4));
        sp48 = 1;
        *(uint16_t *)(g + 0xa4) = (uint16_t)(a4 + 1);
        v0 = *(uint8_t *)guest_at(0x801b46d8 + a4 * 3 + sp68);
      }
      *(uint16_t *)(g + 0x30) = (uint16_t)(v0 << 3);
      fp++;
    } else {
      fp++;
    }
    sp50 = 0; sp54 = 0;
    *(uint16_t *)(g + 0x8c) = (uint16_t)-1;
    *(uint16_t *)(g + 0x8e) = 0;
    goto poll;

case1: {
    uint32_t t4h = *(uint16_t *)(g + 0x30);
    uint32_t sv  = t4h & 7;
    int32_t tt;
    *(uint16_t *)(g + 0x38) = (uint16_t)sv;
    if (sp54 == 0) {
      if (br2_e21c_s16(*(uint16_t *)(g + 0x8e)) >= 0) {
        tt = br2_e21c_s16(t4h);
        if (tt < 0) tt += 7;
        if (FUN_8015e9fc(idx, (uint32_t)(tt >> 3)) != 0) {
          v0 = (int32_t)(uint8_t)g8(0x8007193 + (sv << 12));
          v1 = (int32_t)(uint8_t)g8(0x8007113 + (sv << 12));
          t0 = (v0 >> 4) + (v1 >> 4) * 10;
          if (br2_e21c_s16(*(uint16_t *)(g + 0x28)) == t0) goto hit610;
          if ((uint32_t)(t0 - 1) < 0x5f) {
            int32_t a2v = t0 - 1;
            uint32_t wi, bit;
            *(uint16_t *)(g + 0x28) = (uint16_t)t0;
            if (a2v < 0) { *(uint16_t *)(g + 0x8c) = 0; a2v += 0x1e; }
            wi = (uint32_t)(a2v >> 5);
            c0 = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc0));
            bit = (uint32_t)(a2v - (int32_t)(wi << 5));
            v0 = (int32_t)((*(uint32_t *)(c0 + wi * 4 + 0x54) >> bit) & 1);
            if (v0 == 0) {
              *(uint32_t *)(c0 + wi * 4 + 0x54) |= (uint32_t)1 << bit;
              *(uint32_t *)guest_at(0x801c11f0) = fp;
            }
          } else {
            *(uint16_t *)(g + 0x28) = 0;
            *(uint16_t *)(g + 0x40) = 0;
          }
        }
      }
      if (br2_e21c_s16(*(uint16_t *)(g + 0x8c)) >= 0) {
        if (FUN_8015e8a4(br2_e21c_s16(*(uint16_t *)(g + 0x28)) - 1) != 0)
          goto hit610;
      }
      goto draw678;
    }
    goto draw678;

hit610:
    v0 = (int32_t)(uint8_t)g8(0x8007213 + (sv << 12));
    v0 >>= 4;
    sp50 = (uint32_t)v0;
    *(uint16_t *)(g + 0x40) = *(uint16_t *)(g + 0x28);
    v1 = v0 - 1;
    if ((uint32_t)v1 < 5) {
      static const uint8_t jt_bc[5] = {0,1,0,0,1}; /* 0x10 for 0,2,3 ; 0x3c for 1,4 */
      s7 = jt_bc[v1] ? 0x3c : 0x10;
    } else {
      sp50 = 0;
    }
    goto draw678;

draw678: {
    int32_t t5 = br2_e21c_s16(*(uint16_t *)(g + 0x40));
    if (t5 > 0) {
      if (sp54 < 6) {
        static const uint8_t jt_d4[6] = {0,1,1,3,4,4};
        switch (jt_d4[sp54]) {
        case 0: goto d818;
        case 1: goto d7ac;
        case 2: goto d754;
        case 3: goto d6c0;
        }
      }
      goto after678;
    }
    if (t5 < 0) goto negpath;
    goto after678;

d6c0: /* sp54 4,5 : fade bar, cc6c stk18=2 */
    {
      int32_t a0x = s7 * 255;
      int32_t vv;
      if (sp54 == 4) vv = br2_e21c_shr4(a0x);
      else           vv = br2_e21c_divm(a0x);
      t0 = 0xff - vv;
      s7--;
      if (s7 == -1) { sp54 = 0; *(uint16_t *)(g + 0x40) = (uint16_t)-1; }
      a1 = 0; a2 = a1;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      FUN_8018cc6c(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)br2_e21c_s16(*w20),
                   (int16_t)br2_e21c_s16(*w22), (uint32_t)v0, 2);
      goto after678;
    }
d754: /* sp54 3 : 8c71c growing bar */
    {
      int32_t a2x = s7 - 1;
      int32_t y  = br2_e21c_s16(*w20);
      int32_t p  = a2x * y;
      t0 = br2_e21c_shr4(p);
      s7 = a2x;
      if (s7 == -1) { sp54 = 0; *(uint16_t *)(g + 0x40) = 0; }
      a1 = (t0 - y) & 0xffff;
      FUN_8018c71c(ctx8 + 4, (uint32_t)a1, 0xeffe0088, 0x7fc00000,
                   t0, br2_e21c_s16(*w22));
      goto after678;
    }
d7ac: /* sp54 1,2 : fade bar, cc6c stk18=3 */
    {
      int32_t a0x = s7 * 255;
      int32_t vv;
      if (sp54 == 1) vv = br2_e21c_shr4(a0x);
      else           vv = br2_e21c_divm(a0x);
      t0 = 0xff - vv;
      s7--;
      if (s7 == -1) { sp54 = 0; *(uint16_t *)(g + 0x40) = 0; }
      a1 = 0; a2 = 0;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      FUN_8018cc6c(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)br2_e21c_s16(*w20),
                   (int16_t)br2_e21c_s16(*w22), (uint32_t)v0, 3);
      goto after678;
    }
d818: /* sp54 0 : sub-state sp50 table */
    if (sp50 < 6) {
      static const uint8_t jt_ec[6] = {0,1,1,2,3,3};
      switch (jt_ec[sp50]) {
      case 0: /* 6e9bc : plain 8c71c */
        FUN_8018c71c(ctx8 + 4, 0, 0xeffe0088, 0x7fc00000,
                     br2_e21c_s16(*w20), br2_e21c_s16(*w22));
        goto after678;
      case 1: case 2: goto e91c;
      case 3: goto e8a8;
      case 4: case 5: goto e848;
      }
    }
    goto after678;

e91c: /* sp50 1,2 : shrinking bar, cc6c stk18=3 */
    {
      int32_t v1x = s7 * 255;
      t0 = (sp50 == 1) ? br2_e21c_shr4(v1x) : br2_e21c_divm(v1x);
      s7--;
      if (s7 == -1) sp50 = 0;
      a1 = 0; a2 = 0;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      FUN_8018cc6c(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)br2_e21c_s16(*w20),
                   (int16_t)br2_e21c_s16(*w22), (uint32_t)v0, 3);
      goto after678;
    }
e8a8: /* sp50 3 : growing bar, 8c71c */
    {
      int32_t y = br2_e21c_s16(*w20);
      t0 = br2_e21c_shr4(s7 * y);
      s7--;
      if (s7 == -1) sp50 = 0;
      FUN_8018c71c(ctx8 + 4, (uint32_t)(t0 & 0xffff), 0xeffe0088, 0x7fc00000,
                   (uint32_t)(t0 + y), br2_e21c_s16(*w22));
      goto after678;
    }
e848: /* sp50 4,5 : shrinking bar, cc6c stk18=2 */
    {
      int32_t v1x = s7 * 255;
      t0 = (sp50 == 4) ? br2_e21c_shr4(v1x) : br2_e21c_divm(v1x);
      s7--;
      if (s7 == -1) sp50 = 0;
      a1 = 0; a2 = 0;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      FUN_8018cc6c(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)br2_e21c_s16(*w20),
                   (int16_t)br2_e21c_s16(*w22), (uint32_t)v0, 2);
      goto after678;
    }

negpath:
    /* 6ea00 : sp40 == 0 handled above; here sp40 < 0 dim bar */
    FUN_8018cc6c(ctx8 + 4, 0, 0, (int16_t)br2_e21c_s16(*w20),
                 (int16_t)br2_e21c_s16(*w22), 0xffffff, 0);
    goto after678;
    }

after678:
    /* 6ea44 gates */
    if (br2_e21c_s16(*(uint16_t *)(g + 0x8c)) >= 0) goto poll;
    if (br2_e21c_s16(*(uint16_t *)(g + 0x8e)) >= 0) goto poll;
    if (sp50 != 0) goto poll;
    if (sp54 != 0) goto poll;
    {
      uint8_t *r = g + sp58 * 2;
      if ((*(uint16_t *)(r + 0x6c) & 0xf) != 0) goto poll;
    }
    /* HUD body */
    if (mode == 2) goto mode2body;
    {
      uint32_t dw = *(uint32_t *)(g + 0xdc) & 0x30;
      int32_t sp30s = br2_e21c_s16(*(uint16_t *)(g + 0x30));
      if (dw != 0 || sp30s >= 0) {
        /* sprite block (6ecac/6ed1c merged — identical) */
        s6 = br2_e21c_div3_4(br2_e21c_s16(*w22) * 3);
        s0v = br2_e21c_s16(*w20);
        s1 = (s0v - 0x1c2) & 0xffff;
        s3 = (int32_t)*(uint16_t *)(g + 0x38);
        s2 = s3 << 5;
        FUN_8018c71c(ctx8,
          (uint32_t)(((s6 + 4) << 16) | s1), 0x0efe001c,
          (uint32_t)((s2 << 8) | 0x7bd6), (uint32_t)(s0v - 4),
          (uint32_t)(s6 + 0x23));
        FUN_8018c71c(ctx8,
          (uint32_t)(((s6 + 0x2c) << 16) | s1), 0x0efe001c,
          (uint32_t)(((s2 | 0x10) << 8) | 0x7bd6), (uint32_t)(s0v - 4),
          (uint32_t)(s6 + 0x4a));
        {
          uint8_t *tb = (uint8_t *)guest_at(0x8007093 + (s3 << 12));
          int32_t vv = *tb >> 4;
          if ((uint32_t)(vv - 1) < 0xb) {
            int32_t t0v = vv - 1;
            int32_t hi = ((t0v >> 2) << 6) + 1;
            hi &= 0xff; hi <<= 8;
            a3 = ((vv + 0x1df) & 0x1ff) << 22;
            a3 |= hi;
            a3 |= (t0v & 3) << 6;
            FUN_8018c824(ctx8, (uint32_t)(((s6 + 8) << 16) | 4),
                         0x3e37009d, (uint32_t)a3);
          }
        }
        FUN_8018cc6c(ctx8, 0, (int16_t)s6, (int16_t)br2_e21c_s16(*w20),
                     0x50, 0x80808080, 3);
      }
    }
    goto afterdraw;

mode2body:
    /* mode==2 : three text rows + bar */
    {
      int32_t yv = br2_e21c_s16(*w22);
      int32_t t0v = yv * 3;
      int32_t a3v, row;
      a3v = br2_e21c_s16(*w20);
      a3v = (a3v + (a3v >> 31)) >> 1;      /* /2 trunc */
      a3v -= 0x60;
      a3v = (int16_t)a3v;
      t0v = yv * 3; if (t0v < 0) t0v += 3; t0v >>= 2;
      FUN_8012e58c(ctx8, ctxc, (const void *)guest_at(0x801222a8),
                   (uint32_t)(int16_t)(t0v + 0x20), 0, a3v);
      t0v = yv * 3; if (t0v < 0) t0v += 3; t0v >>= 2;
      FUN_8012e58c(ctx8, ctxc, (const void *)guest_at(0x801222b4),
                   (uint32_t)(int16_t)(t0v + 0x30), sp4c, a3v);
      t0v = yv * 3; if (t0v < 0) t0v += 3; t0v >>= 2;
      FUN_8012e58c(ctx8, ctxc, (const void *)guest_at(0x801222b8),
                   (uint32_t)(int16_t)(t0v + 0x70), (uint32_t)(sp4c < 1), a3v);
      a2 = yv * 3; if (a2 < 0) a2 += 3; a2 >>= 2;
      FUN_8018cc6c(ctx8, 0, (int16_t)a2, (int16_t)br2_e21c_s16(*w20),
                   0x50, 0x80808080, 3);
      (void)row;
    }

afterdraw:
    /* 6ee7c : lhu 0x68 flag checks + nibble state advance */
    {
      uint8_t *r = g + sp58 * 2;
      uint32_t f68 = *(uint16_t *)(r + 0x68);
      if ((f68 & 0x800) == 0) {
        if ((f68 & 0xe0) == 0) goto poll;
        FUN_80156ef4(0x400,0xc,0);
        if (mode != 2) {
          uint8_t *tb = (uint8_t *)guest_at(0x8007293 + (*(uint16_t *)(g + 0x38) << 12));
          int32_t vv = *tb >> 4;
          int32_t v1x = vv - 1;
          if ((uint32_t)v1x < 5) {
            static const uint8_t jt2304[5] = {0,1,0,0,1};
            s7 = jt2304[v1x] ? 0x3c : 0x10;
          } else sp54 = 0;
          {
            uint8_t *tb2 = (uint8_t *)guest_at(0x80071013 + (*(uint16_t *)(g + 0x38) << 12));
            int32_t t0v = *tb2 >> 4;
            if (t0v == 0xe) goto modechk;
            if (t0v == 0xf) {
              *(uint16_t *)(g + 0x30) = (uint16_t)((*(uint16_t *)(g + 0x30) + 8) & 0xfff8);
            } else {
              *(uint16_t *)(g + 0x30) = (uint16_t)(*(uint16_t *)(g + 0x30) + 1);
            }
            *(uint16_t *)(g + 0x8e) = 0;
            goto poll;
          }
        }
      }
    }
modechk:
    if (mode == 1) goto mode1body;
    if (mode < 2) {
      if (mode == 0) goto mode0body;
      goto poll;
    }
    if (mode == 2) goto mode2sel;
    goto poll;

mode0body: {
    uint32_t byte = *(uint8_t *)guest_at(0x801b461c + idx);
    int32_t a0x = br2_e21c_s16(*(uint16_t *)(g + 0xa4));
    if (a0x < (int32_t)byte && sp48 == 0) {
      uint8_t v = *(uint8_t *)guest_at(0x801f47a0 + a0x);
      sp24[3] = v;
      goto tail194;
    }
    /* 6f018 */
    c0 = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc0));
    v1 = *(uint32_t *)(c0 + 0x44);
    if ((v1 & 1) == 0) {
      *(uint32_t *)(c0 + 0x44) = v1 | 1;
      *(uint32_t *)guest_at(0x801c11f0) = 1;
    }
    if (*(uint16_t *)(sp20 + 0xec) == 0) {
      v1 = *(uint32_t *)(c0 + 0x44);
      if (((v1 >> 15) & 1) == 0) {
        *(uint32_t *)(c0 + 0x44) = v1 | 0x8000;
        *(uint32_t *)guest_at(0x801c11f0) = 1;
      }
      *(uint16_t *)(g + 0xa4) = (uint16_t)-2;
      FUN_80156ef4(0x204,0,0);
      FUN_8012aa6c(2,8);
      {
        uint8_t *r = g + sp58 * 2;
        uint32_t b4 = *(uint16_t *)(r + 0xb4);
        *(uint16_t *)(r + 0xb4) = (uint16_t)((b4 & 0xf000) | 0xa00);
      }
      g[0] = 9; g[1] = 9;
      *(uint16_t *)(g + 0x92) = (uint16_t)((*(uint16_t *)(g + 0x92) & 0xff00) | 0x10);
      fp = 0xf0;
      goto poll;
    }
    goto tail194;
tail194:
    g[0x98] = *(uint8_t *)guest_at(0x801f4770 + br2_e21c_s16(*(uint16_t *)(g + 0xa4)));
    fp = 0xf0;
    goto poll;
}

mode1body:
    if (sp48 == 0) goto poll;
    mode = 0;
    *(uint16_t *)(g + 0x8e) = 0;
    *(uint16_t *)(g + 0x30) = (uint16_t)(*(uint8_t *)guest_at(0x801b46d8 + br2_e21c_s16(*(uint16_t *)(g + 0xa4)) * 3 + sp60) << 3);
    goto poll;

mode2sel:
    if (sp4c != 0) goto mode2b;
    *(uint16_t *)(sp20 + 0xec) = (uint16_t)(*(uint16_t *)(sp20 + 0xec) + 1);
    sp24[3] = *(uint8_t *)guest_at(0x801f47a0 + br2_e21c_s16(*(uint16_t *)(g + 0xa4)));
    g[0x98] = *(uint8_t *)guest_at(0x801f4770 + br2_e21c_s16(*(uint16_t *)(g + 0xa4)));
    fp = 0xf0;
    goto poll;
mode2b:
    c0 = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc0));
    if (c0[0xd] == 0) {
      FUN_801577bc(0x27,0); s7 = 0x2ee;
    } else {
      FUN_801577bc(6,0);   s7 = 0x15e;
    }
    fp = 2;
    goto poll;
    }

case2:
    FUN_8015143c(0x1f800000,0x16,(uint32_t)s7);
    s7--;
    if (s7 != -1) {
      uint8_t *r = g + sp58 * 2;
      if ((*(uint16_t *)(r + 0x68) & 0xe0) == 0) goto poll;
    }
    FUN_80156ef4(0x204,0,0);
    FUN_8012aa6c(2,8);
    fp = 0xf0;
    g[0] = 5; g[1] = 9;
    goto poll;

poll:
    frame_tick((intptr_t)g);
    if (fp != 0xff) continue;
    return;
  }
}

/* ---- FUN_8015bdc0 @0x8015bdc0 (613 insns) ----
   a0 = guest dialog-state ptr, a1 = selector. Sibling of
   FUN_80160598: same init, wider selector dispatch. */
extern int FUN_80129b48();
extern void FUN_8012e8e8();
extern void FUN_8015260c(uint8_t *,int);

int FUN_8015bdc0(uint32_t a0,uint32_t a1)
{
  uint8_t *s4 = (uint8_t *)guest_at(a0);
  uint8_t *g  = (uint8_t *)guest_at(0x1f800000);
  uint32_t ctx8 = *(uint32_t *)(g + 8);
  uint32_t ctxc = *(uint32_t *)(g + 0xc);
  uint32_t flag, sel;
  uint8_t *c0;
  int32_t v0;

  FUN_8012cd9c(0x20,0xffffffff);
  sel = a1 & 0xffff;
  if (sel == (uint32_t)(*(uint16_t *)(s4 + 8) + 0x100)) {
    flag = 0;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121270),0x1000,0x1000,0);
  } else {
    flag = 1;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121274),0x1000,0x1000,flag);
  }

#define BDC0_E58C(str,sz,x) \
  FUN_8012e58c(ctx8,ctxc,(const void *)guest_at(str),(sz),0x1000,(x))
#define BDC0_STATUS() do { *(uint32_t *)guest_at(0x801c11f0) = 1; return 0; } while (0)

  if (sel == 0x100) {
    FUN_8012cd9c(0xffffffff,0x18);
    BDC0_E58C(0x80121278,0x1000,0);
    FUN_8012bf0c(9,0,4,7);
    FUN_8012cdd8(0,0x10);
    return 0;
  }
  if (sel < 0x101) {
    switch (sel) {
    case 1:
      if (FUN_80129978(a0 + 0x14,3,1) == 0) return 0;
      *(uint32_t *)guest_at(0x801c11f0) = 1;
      g[0x9c] = 5;
      v0 = *(int16_t *)(s4 + 0x14);
      if (v0 == 1)      g[0x9b] = 1;
      else if (v0 == 0) g[0x9b] = 2;
      else if (v0 == 2) {
        c0 = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc0));
        c0[0x10] = 0xb;
        g[0x9b] = 0;
      } else if (v0 == 3) g[0x9b] = 4;
      if ((*(uint16_t *)(g + 0x68) & 0xe0) != 0) {
        FUN_80156ef4(0x400,0xc,0);
        FUN_8015260c(g,0);
      } else if ((*(uint16_t *)(g + 0x6a) & 0xe0) != 0) {
        FUN_80156ef4(0x400,0xc,0);
        FUN_8015260c(g,1);
      } else {
        return 0;
      }
      g[0] = 8; g[1] = 0xf0;
      return 0;
    case 2:
      /* a0 not rewritten by original: stale ctx8 (last a0 write) */
      if (FUN_80129978(ctx8,3,1) == 0) return 0;
      BDC0_STATUS();
    case 3:
      if (FUN_80129978(a0 + 0x18,1,1) == 0) return 0;
      BDC0_STATUS();
    case 4:
      if (FUN_80129978(a0 + 0x1a,1,1) == 0) return 0;
      BDC0_STATUS();
    case 5:
      if (FUN_80129b48(ctx8,8) == 0) return 0;
      BDC0_STATUS();
    case 6:
      if (FUN_80129978(a0 + 0x1e,8) == 0) return 0;
      BDC0_STATUS();
    default:
      return 0;
    }
  }
  if (sel < 0x107) {
    switch (sel) {
    case 7:
      if (FUN_80129978(a0 + 0x20,1,1) == 0) return 0;
      BDC0_STATUS();
    case 8:
      if (FUN_80129978(ctx8,1,1) == 0) return 0;
      BDC0_STATUS();
    case 9:
      if (FUN_80129978(a0 + 0x24,1,1) == 0) return 0;
      BDC0_STATUS();
    case 0xa: {
      uint32_t w40;
      if ((*(uint16_t *)(g + 0x7c) & 0xe0) == 0) return 0;
      FUN_80156ef4(0x400,0xc,0);
      {
        uint8_t *v1 = (uint8_t *)guest_at(0x801b4280);
        *(uint16_t *)(s4 + 0x14) = 0;
        *(uint16_t *)(s4 + 0x16) = v1[0x39];
        *(uint16_t *)(s4 + 0x18) = v1[0x3a];
        w40 = *(uint32_t *)(v1 + 0x40);
        *(uint16_t *)(s4 + 0x1a) = (uint16_t)((w40 >> 13) & 1);
        *(uint16_t *)(s4 + 0x1c) = v1[0x3b];
        *(uint16_t *)(s4 + 0x1e) = v1[0x3c];
        *(uint16_t *)(s4 + 0x20) = (uint16_t)(((w40 >> 6) ^ 1) & 1);
        *(uint16_t *)(s4 + 0x22) = (uint16_t)((w40 >> 11) & 1);
        *(uint16_t *)(s4 + 0x24) = (uint16_t)((w40 >> 10) & 1);
      }
      *(uint32_t *)guest_at(0x801c11f0) = 1;
      return 0;
    }
    default:
      return 0;
    }
  }

  if (sel >= 0x101) {
    uint32_t f2;
    switch (sel) {
    case 0x101:
      f2 = (sel == (uint32_t)(*(uint16_t *)(s4 + 8) + 0x100)) ? 3 : 4;
      BDC0_E58C(0x80121280,0x1000,f2);
      BDC0_E58C(g32(0x801b4028 + (uint32_t)(*(int16_t *)(s4 + 0x14)) * 4),0x140,f2);
      FUN_8012cdd8(0,0xa);
      BDC0_E58C(g32(0x801b4038 + (uint32_t)(*(int16_t *)(s4 + 0x14)) * 4),0x140,f2);
      FUN_8012cdd8(0,0xc);
      return 0;
    case 0x102:
      BDC0_E58C(0x8012128c,0x1000,flag);
      BDC0_E58C(g32(0x801b4048 + (uint32_t)(*(int16_t *)(s4 + 0x16)) * 4),0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    case 0x103:
      BDC0_E58C(0x80121298,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x18)) * 4),0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    case 0x104:
      BDC0_E58C(0x801212a8,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x1a)) * 4),0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    case 0x105: {
        BDC0_E58C(0x801212bc,0x1000,flag);
      FUN_8012e8e8(ctx8,ctxc,(const void *)guest_at(0x801212cc),0x140,0x1000,flag);
      FUN_8018ef60((char *)guest_at(0x1f800f80),(char *)guest_at(0x801212d0),
                   (int32_t)*(int16_t *)(s4 + 0x1c));
      BDC0_E58C(0x1f800f80,0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    }
    case 0x106: {
      BDC0_E58C(0x801212d4,0x1000,flag);
      FUN_8012e8e8(ctx8,ctxc,(const void *)guest_at(0x801212cc),0x140,0x1000,flag);
      FUN_8018ef60((char *)guest_at(0x1f800f80),(char *)guest_at(0x801212d0),
                   (int32_t)*(int16_t *)(s4 + 0x1e));
      BDC0_E58C(0x1f800f80,0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    }
    default:
      return 0;
    }
  }
  /* sel >= 0x107 */
  if (sel < 0x10b) {
    switch (sel) {
    case 0x107:
      BDC0_E58C(0x801212e4,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x20)) * 4),0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    case 0x108:
      BDC0_E58C(0x801212f4,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x22)) * 4),0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    case 0x109:
      BDC0_E58C(0x80121304,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x24)) * 4),0x140,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    case 0x10a:
      BDC0_E58C(0x80121310,0x1000,flag);
      FUN_8012cdd8(0,0xc);
      return 0;
    default:
      return 0;
    }
  }
  if (sel == 0x10b) {
    BDC0_E58C(0x80121318,0x1000,flag);
    FUN_8012cdd8(0,0x14);
    return 0;
  }
  return 0;
}

/* ======================================================================
 * Round-3 ports: save/load state machines, memcard wrappers, HUD debug
 * dumps. Guest addrs via guest_at/br2_g*; buffer params that callers
 * pass as host pointers are dereferenced directly.
 * ====================================================================== */

/* ---- weak stubs for still-unported callees (mirrors bind.c) ---- */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8016827c() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_80168648() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_80168700() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8012ba04() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8012e3c4() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8012e4f4() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int hud_text_a() { return 0; }
extern int32_t FUN_8015643c(uint32_t,uint32_t);
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8016dadc() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8016cb20() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8016cc5c() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_80135a14() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8015d420() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8015ebf8() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8016088c() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_80161630() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8018e550() { return 0; }

/* callee externs for the ports below. The weak stubs above are ()-style
   non-prototype defs, so calls below promote args the usual way; results
   are read as int32_t. */
extern int32_t FUN_8019fa80(uint32_t,uint32_t *,uint32_t *);
extern uint8_t *FUN_80127680(void);

/* FUN_80163154's real signature (menu.c): (guest heap ptr, host int*
   tag table, tag) -> new heap ptr. save.c's mid-file extern says void;
   call through a cast to recover args+result faithfully. */
static uint32_t br2_alloc54(uint32_t heap, int *tbl, int32_t tag)
{
  return ((uint32_t (*)(uint32_t, int *, int))FUN_80163154)(heap, tbl, tag);
}

#define BR2_W32(a,v)   (*(uint32_t *)guest_at(a) = (uint32_t)(v))
#define BR2_W16(a,v)   (*(uint16_t *)guest_at(a) = (uint16_t)(v))
#define BR2_WS16(a,v)  (*(int16_t *)guest_at(a) = (int16_t)(v))
#define BR2_W8(a,v)    (*(uint8_t *)guest_at(a) = (uint8_t)(v))

/* ---- FUN_8015e87c @0x8015e87c (3 insns) ---- */
int FUN_8015e87c(void)
{
  BR2_W32(0x801b4154, 0xffffffffu);
  return -1;
}

/* ---- FUN_8015e88c @0x8015e88c (5 insns) ---- */
int FUN_8015e88c(void)
{
  BR2_W32(0x801b4158, 0xffffffffu);
  BR2_W32(0x801b415c, 0xffffffffu);
  return (int)0x801b0000;
}

/* ---- FUN_8015e8a4 @0x8015e8a4 (85 insns) ----
   Display-page commit for channel A (regs @0x801f4760/64, state @0x1f80008c,
   current @0x801b4154). a0 = requested page (s2). */
int FUN_8015e8a4(int32_t a0)
{
  uint8_t *s3 = (uint8_t *)guest_at(0x1f800000);
  int32_t s2 = a0;
  int32_t cur = (int32_t)g32(0x801b4154);
  int32_t s0;
  int32_t s1;

  BR2_W32(0x801f4760, 0x80010000u);
  BR2_W32(0x801f4764, 0x80040000u);

  if (cur != s2) {
    if ((cur >> 2) != (s2 >> 2)) {
      /* page change: state gate via h@0x1f80008c */
      s0 = (int32_t)*(int16_t *)(s3 + 0x8c);
      if (s0 == 0) {
        FUN_8018cfdc(-1, 0);
        s0 = 0x10;
        BR2_W32(0x801c43a0, 0);
        BR2_W8(0x801e3d00, 0xff);
      } else if (s0 != 0x10) {
        *(int16_t *)(s3 + 0x8c) = (int16_t)s0;
        return 0;
      }
      if (FUN_8018cfdc((s2 >> 2) + 0x118,
                       (uint32_t)g32(0x801f4760)) == 0) {
        *(int16_t *)(s3 + 0x8c) = (int16_t)s0;
        return 0;
      }
    }
    /* commit path (also taken when same 1KB page) */
    s1 = (int32_t)g8(0x1f800013);
    FUN_801274d8(0);
    FUN_80163154(g32(0x801f4764), g32(0x801f4760), s2 & 3);
    FUN_8018bfcc(g32(0x801f4764), 0, 0x1ff, 1, 0x200, 0, 1);
    FUN_8019a460(0);
    FUN_801274d8((uint32_t)s1);
    BR2_W32(0x801b4154, (uint32_t)s2);
  }
  *(int16_t *)(s3 + 0x8c) = -1;
  return 1;
}

/* ---- FUN_8015e9fc @0x8015e9fc (126 insns) ----
   Display-page commit for channel B (regs @0x801f4768/6c, state @0x1f80008e,
   currents @0x801b4158/0x801b415c). a0 = bank, a1 = offset. */
int FUN_8015e9fc(uint32_t a0, uint32_t a1)
{
  uint8_t *s3 = (uint8_t *)guest_at(0x1f800000);
  uint32_t s4 = a0;
  int32_t s2 = (int32_t)a1;
  int32_t cur8 = (int32_t)g32(0x801b4158);
  int32_t s0, s1;

  BR2_W32(0x801f4768, 0x80050000u);
  BR2_W32(0x801f476c, 0x80070000u);

  if ((int32_t)s4 == cur8) {
    int32_t curc = (int32_t)g32(0x801b415c);
    if (curc == s2) goto same;
    if ((curc >> 3) == (s2 >> 3)) goto commit;
  }
  /* state gate via h@0x1f80008e */
  s0 = (int32_t)*(int16_t *)(s3 + 0x8e);
  if (s0 == 5) {
    if (FUN_8018cfdc(0x16, 0x80050000u) == 0) {
      *(int16_t *)(s3 + 0x8e) = (int16_t)s0;
      return 0;
    }
    s1 = (int32_t)g8(0x1f800013);
    FUN_801274d8(0);
    FUN_8018bf40((int32_t)g32(0x801f4768), 0, 0x1e0, 1);
    s0 = 0x10;
    FUN_8019a460(0);
    FUN_801274d8((uint32_t)s1);
    goto mid;
  }
  if (s0 >= 6) {
    if (s0 != 0x10) {
      *(int16_t *)(s3 + 0x8e) = (int16_t)s0;
      return 0;
    }
    goto mid;
  }
  if (s0 == 0) {
    FUN_8018cfdc(-1, 0);
    BR2_W32(0x801c43a0, 0);
    BR2_W8(0x801e3d00, 0xff);
    if ((int32_t)g32(0x801b415c) != -1) {
      s0 = 0x10;
    } else {
      s0 = 5;
    }
    *(int16_t *)(s3 + 0x8e) = (int16_t)s0;
    return 0;
  }
  *(int16_t *)(s3 + 0x8e) = (int16_t)s0;
  return 0;

mid:
  {
    int32_t t = (int32_t)(uint8_t)g8(0x801b4148 + s4);
    int32_t a0x = (s2 >> 3) + t + 0x130;
    if (FUN_8018cfdc(a0x, (uint32_t)g32(0x801f4768)) == 0) {
      *(int16_t *)(s3 + 0x8e) = (int16_t)s0;
      return 0;
    }
  }
commit:
  s1 = (int32_t)g8(0x1f800013);
  FUN_801274d8(0);
  FUN_80163154(g32(0x801f476c), g32(0x801f4768), s2 & 7);
  FUN_8018bf40((int32_t)g32(0x801f476c), 0x300, 0x100, 1);
  FUN_8019a460(0);
  FUN_801274d8((uint32_t)s1);
  BR2_W32(0x801b4158, s4);
  BR2_W32(0x801b415c, (uint32_t)s2);
same:
  *(int16_t *)(s3 + 0x8e) = -1;
  return 1;
}

/* ---- FUN_8016831c @0x8016831c (24 insns) ----
   memcard op wrapper: probe (FUN_80168648), on success FUN_8016827c then
   FUN_80168700. a2/a3/stack-a4 forwarded to FUN_8016827c. */
int FUN_8016831c(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  int32_t r = FUN_80168648(a0,a1,1);
  if (r != 0) return r;
  FUN_8016827c(a2,a3,a4);
  return FUN_80168700();
}

/* ---- FUN_801685b4 @0x801685b4 (36 insns) ----
   Poll FUN_8019fb9c until result stabilises (or 0). JT@0x80122144: idx0
   returns immediately; all other codes re-poll while the value changes. */
int FUN_801685b4(uint32_t a0,const void *a1,uint32_t a2)
{
  int32_t prev = 0;
  int32_t r;
  for (;;) {
    r = FUN_8019fb9c(a0,a1,a2);
    if (r == 0) return 0;
    if (r == prev) return r;
    prev = r;
  }
}

/* ---- FUN_80168838 @0x80168838 (45 insns) ----
   Issue via FUN_8019f35c then poll FUN_8019fa80 until a stable nonzero
   result code (0 returns immediately). */
int FUN_80168838(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  uint32_t t1 = 0, code = 0;
  int32_t prev = 0;
  for (;;) {
    while (FUN_8019f35c(a0,a1,a2,a3,a4) == 0) { }
    while (FUN_8019fa80(0,&t1,&code) == 0) { }
    if (code == 0) return 0;
    if ((int32_t)code != prev) { prev = (int32_t)code; continue; }
    return (int32_t)code;
  }
}

/* ---- FUN_801688f0 @0x801688f0 (45 insns) ---- same shape, FUN_8019f5b4 */
int FUN_801688f0(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  uint32_t t1 = 0, code = 0;
  int32_t prev = 0;
  for (;;) {
    while (FUN_8019f5b4(a0,a1,a2,a3,a4) == 0) { }
    while (FUN_8019fa80(0,&t1,&code) == 0) { }
    if (code == 0) return 0;
    if ((int32_t)code != prev) { prev = (int32_t)code; continue; }
    return (int32_t)code;
  }
}

/* ---- FUN_8016f30c @0x8016f30c (158 insns) ----
   10-slot ambient/palette HUD blitter. a0 = guest state ptr, a1 = flag.
   s1 = per-slot record @0x801f4ee8 + idx*0x18; halfword attr array at
   s1+0..0x13, counters @0x14/0x15, timer @0x16. */
void FUN_8016f30c(uint32_t a0, uint32_t a1)
{
  uint8_t *g = (uint8_t *)guest_at(0x1f800000);
  uint8_t *st = (uint8_t *)guest_at(a0);
  uint32_t i = st[2];
  uint16_t *hw68 = (uint16_t *)(g + 0x68 + i * 2);
  uint16_t *hw6c = (uint16_t *)(g + 0x6c + i * 2);
  uint8_t *s1 = (uint8_t *)guest_at(0x801f4ee8 + i * 24);
  uint32_t s2 = (*(uint16_t *)(st + 0x7c) & 0xe8) | (*hw6c & 0xf000);
  int32_t s0 = (*(uint16_t *)(st + 0x7a) & 0xe8) | (*hw68 & 0xf000);
  uint32_t x, y;
  int32_t idx, cnt;

  if (*(uint16_t *)(s1 + 0x16) >= 0x32 && s0 != 0)
    clear_f4ee8_u16s(a0);
  if (s0 != 0) {
    *(uint16_t *)(s1 + 0x16) = 0;
  } else {
    uint32_t t = *(uint16_t *)(s1 + 0x16);
    if (t < 0x33) *(uint16_t *)(s1 + 0x16) = (uint16_t)(t + 1);
  }

  if (*(uint16_t *)(g + 0x82) == 0) {
    int do_adv;
    x = s1[0x14] % 10;
    y = s1[0x15] % 10;
    s1[0x14] = (uint8_t)x;
    s1[0x15] = (uint8_t)y;
    if (s0 != 0) {
      do_adv = 1;
    } else {
      uint32_t v1 = s2 & 0xf000;
      if (v1 == 0) {
        do_adv = 0;
      } else {
        do_adv = (*(uint16_t *)(s1 + (y & 0xff) * 2) & 0xf000) != v1;
      }
    }
    if (do_adv) {
      uint32_t ny = (y + 1) & 0xff;
      uint32_t xv;
      ny %= 10;
      s1[0x15] = (uint8_t)ny;
      xv = s1[0x14];
      if (ny == xv) {
        xv = (xv + 1) & 0xff;
        xv %= 10;
        s1[0x14] = (uint8_t)xv;
      }
      *(uint16_t *)(s1 + s1[0x15] * 2) = (uint16_t)s2;
    }
  }

  /* blit all 10 slots starting at cursor x */
  idx = s1[0x14];
  cnt = 0;
  for (;;) {
    uint16_t attr = *(uint16_t *)(s1 + idx * 2);
    idx += 1;
    FUN_8012ba04(_DAT_1f800008, _DAT_1f80000c, attr, 0x1000, 0x1000, a1);
    idx %= 10;
    cnt += 1;
    if (cnt >= 10) break;
  }
  print_dispatch((const char *)guest_at(0x801223cc));
  if (a1 != 0)
    print_dispatch((const char *)guest_at(0x801223cc));
}

/* ---- FUN_8017f13c @0x8017f13c (150 insns) ----
   Debug flag-name dump. a0 = guest state ptr, a1 = group 0..5
   (AF/PF/NF/KF/DF/EF). Each group: header, 32 bits of mask @a0+off,
   name strings from table @0x801b4f28+grp*0x80. */
void FUN_8017f13c(uint32_t a0, uint32_t a1)
{
  static const uint16_t offs[6]  = {0x58,0x5c,0x60,0x64,0x68,0x6c};
  static const uint32_t hdrs[6]  = {0x80122f80,0x80122f8c,0x80122f90,
                                    0x80122f94,0x80122f98,0x80122f9c};
  static const uint32_t tabs[6]  = {0x801b4f28,0x801b4fa8,0x801b5028,
                                    0x801b50a8,0x801b5128,0x801b51a8};
  int32_t s1;
  int32_t i;
  if (a1 >= 6) return;
  print_dispatch((const char *)guest_at(hdrs[a1]));
  s1 = (int32_t)g32(a0 + offs[a1]);
  for (i = 0; i < 0x20; i++) {
    if (s1 & 1)
      print_dispatch((const char *)guest_at(0x80122f84),
                   (const char *)guest_at(g32(tabs[a1] + i * 4)));
    s1 >>= 1;
  }
  print_dispatch((const char *)guest_at(0x80122f88));
}

/* ---- FUN_8018106c @0x8018106c (115 insns) ----
   Debug dump of controller/record struct. a0 = guest state ptr;
   s2 = sub-struct ptr @a0+0x3c (guest ptr -> host). */
void FUN_8018106c(uint32_t a0)
{
  uint8_t *s3 = (uint8_t *)guest_at(a0);
  uint8_t *s2;
  uint32_t s1, s0;
  int32_t idx;
  uint32_t f, t1, t2;
  const char *a3s;
  int32_t p;

  if (s3[1] == 0) return;
  s2 = (uint8_t *)guest_at(*(uint32_t *)(s3 + 0x3c));
  s1 = *(uint32_t *)(s3 + 0x58);
  s0 = *(uint32_t *)(s3 + 0x6c);
  *(uint32_t *)(s3 + 0x58) = *(uint32_t *)(s2 + 0x70);
  *(uint32_t *)(s3 + 0x6c) = *(uint32_t *)(s2 + 0x74);
  FUN_8017f13c(a0, 0);
  FUN_8017f13c(a0, 5);
  *(uint32_t *)(s3 + 0x58) = s1;
  *(uint32_t *)(s3 + 0x6c) = s0;

  idx = (int32_t)*(int16_t *)(s2 + 0x28);
  print_dispatch((const char *)guest_at(0x80123094),
               (const char *)guest_at(g32(0x801b52c8 + (uint32_t)(idx * 4))),
               (int32_t)*(int16_t *)(s2 + 0x2a));

  f = *(uint16_t *)(s2 + 6);
  t1 = 0x801b52f0 + ((f >> 12) << 2);
  t2 = 0x801b52f0 + ((f >> 3) & 0x1c);
  a3s = (f & 0x400) ? (const char *)guest_at(0x801230ac)
                    : (const char *)guest_at(0x801230b0);
  print_dispatch((const char *)guest_at(0x801230a0),
               (const char *)guest_at(g32(t1)),
               (const char *)guest_at(g32(t2 + 0x40)),
               a3s);

  p = (int32_t)*(int32_t *)(s2 + 0x30);
  if (p >= 0) {
    print_dispatch((const char *)guest_at(0x801230b4), p,
                 (int32_t)*(int32_t *)(s2 + 0x34));
  } else {
    print_dispatch((const char *)guest_at(0x801230c4));
  }
  print_dispatch((const char *)guest_at(0x801230cc),
               (int32_t)*(uint16_t *)(s2 + 0x6c));
  print_dispatch((const char *)guest_at(0x801230d4),
               (int32_t)*(int16_t *)(s3 + 0x76));
  print_dispatch((const char *)guest_at(0x801230dc),
               (int32_t)*(int16_t *)(s2 + 0x3e),
               (int32_t)*(int16_t *)(s2 + 0x40),
               (int32_t)*(int16_t *)(s2 + 0x42),
               (int32_t)*(int16_t *)(s2 + 0x44),
               (int32_t)*(int16_t *)(s2 + 0x46));
  if ((s3[1] & 0x80) != 0) {
    print_dispatch((const char *)guest_at(0x801230fc));
  } else if (s2[0x7c] != 0) {
    print_dispatch((const char *)guest_at(0x80123110), (const char *)(s2 + 0x7c));
  }
  print_dispatch((const char *)guest_at(0x80123114));
}

/* ---- FUN_80170d38 @0x80170d38 (169 insns) ----
   Save-channel init/commit. a0 = guest state ptr (h@0x8e = phase),
   a1 unused, s2/s3 = host save-slot buffers. Uses heap A @0x801c47b8
   (tag table via FUN_80127680) and heap B @0x801e3cfc. */
int FUN_80170d38(uint32_t a0,int32_t a1,uint8_t *s2,uint8_t *s3)
{
  uint8_t *s5 = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(s5 + 0x8e);
  int32_t s4;
  int32_t i, sz;
  uint32_t head, np, old;
  int *tbl;
  (void)a1;

  if (s0 == 0) goto st0;
  if (s0 == 0x10) goto st10;
  *(int16_t *)(s5 + 0x8e) = (int16_t)s0;
  return 0;

st0:
  if (FUN_8018cfdc(0x13, guest_of(FUN_80127680())) == 0) {
    *(int16_t *)(s5 + 0x8e) = (int16_t)s0;
    return 0;
  }
  s4 = (int32_t)g8(0x1f800013);
  FUN_801274d8(0);
  tbl = (int *)FUN_80127680();
  for (i = 0; i < 0x64; i++) {
    old = g32(0x801c43a8 + (uint32_t)i * 4);
    BR2_W32(0x801c43a8 + (uint32_t)i * 4, g32(0x801c47b8));
    sz = (int32_t)br2_alloc54(g32(0x801c47b8), tbl, i);
    if (sz != 0) {
      BR2_W32(0x801c47b8, g32(0x801c47b8) + (uint32_t)sz);
      FUN_801aa96c(old);
    } else {
      BR2_W32(0x801c43a8 + (uint32_t)i * 4, 0);
    }
    BR2_W32(0x801c47b8, (g32(0x801c47b8) + 3) & ~3u);
  }
  /* heap B blocks */
  sz = (int32_t)br2_alloc54(g32(0x801e3cfc), tbl, 0x100);
  if (sz != 0) {
    FUN_8018bfcc(g32(0x801e3cfc), 0x180, 0x1e0, 1, 0x3c0, 0x180, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  sz = (int32_t)br2_alloc54(g32(0x801e3cfc), tbl, 0x101);
  if (sz != 0) {
    FUN_8018bf40((int32_t)g32(0x801e3cfc), 0x240, 0x100, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  sz = (int32_t)br2_alloc54(g32(0x801e3cfc), tbl, 0x102);
  if (sz != 0) {
    FUN_8018bf40((int32_t)g32(0x801e3cfc), 0x190, 0x1e0, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  sz = (int32_t)br2_alloc54(g32(0x801e3cfc), tbl, 0x103);
  if (sz != 0) {
    FUN_8018bf40((int32_t)g32(0x801e3cfc), 0x1a0, 0x1e0, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  FUN_8019a460(0);
  FUN_801274d8((uint32_t)s4);
  *(int16_t *)(s5 + 0x8e) = 0x10;
  return 0;

st10:
  if (FUN_8018cfdc(0x10, g32(0x801c47b8)) == 0) {
    *(int16_t *)(s5 + 0x8e) = (int16_t)s0;
    return 0;
  }
  head = g32(0x801c47b8);
  np = head + 8;
  /* s2/s3 are host save-slot structs; +0x38 holds a host pointer whose
     +0xa8 slot caches the new heap head (guest addr value). */
  *(uint32_t *)(*(uint32_t *)(s2 + 0x38) + 0xa8) = np;
  *(uint32_t *)(*(uint32_t *)(s3 + 0x38) + 0xa8) = np;
  BR2_W32(0x801c47b8, head + *(uint32_t *)guest_at(head));
  return 1;
}

/* ---- FUN_8015143c @0x8015143c (548 insns) ----
   Battle-announcement HUD printer. a0 = guest state ptr (regs/sprite
   coords), a1 = announcement id 0..0x16 (JT@0x80120840), a2 unused.
   Gate: (*(u16*)(*(u32*)(a0+0xc8)+2) >> 0xa) & 1 must be set.
   All cases draw via FUN_8012e3c4/FUN_8012e4f4/hud_text_a with
   coords derived from lhu/lh @+0x20/+0x22. */

static int32_t br2_5143c_x2(uint8_t *s0)
{ /* (sxt16 lhu@0x20) / 2 trunc */
  return ((int16_t)*(uint16_t *)(s0 + 0x20)) / 2;
}
static int32_t br2_5143c_y2(uint8_t *s0)
{ /* (sxt16 lhu@0x22) / 2 trunc */
  return ((int16_t)*(uint16_t *)(s0 + 0x22)) / 2;
}
static int32_t br2_5143c_y3(uint8_t *s0)
{ /* (lh@0x22 * 3) / 8 trunc */
  return (*(int16_t *)(s0 + 0x22) * 3) / 8;
}
static int32_t br2_5143c_sdiv4(int32_t v)
{ /* (v<0 ? v+3 : v) >> 2 == v/4 trunc */
  return v / 4;
}

void FUN_8015143c(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint32_t sel = a1;
  uint32_t r8, rc;
  int32_t x2, y2, y3;
  char buf[16];
  (void)a2;

  r8 = g32(a0 + 0xc8);
  if ((((uint32_t)g16(r8 + 2)) >> 0xa & 1) == 0) return;
  if (sel >= 0x17) return;

  r8 = *(uint32_t *)(s0 + 8);
  rc = *(uint32_t *)(s0 + 0xc);
  x2 = br2_5143c_x2(s0);
  y2 = br2_5143c_y2(s0);
  y3 = br2_5143c_y3(s0);

  switch (sel) {
  case 0:
    /* "Here Comes A Challenger !!!" via 7-arg FUN_8012e4f4 */
    FUN_8012e4f4(r8, rc, (const void *)guest_at(0x80120768),
                 x2 - 0xe7, y2 - 0xc, 8, -4);
    return;
  case 1:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x80120784),
                 (int16_t)(x2 - 0x84), y3, 8);
    return;
  case 2:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x80120794),
                 (int16_t)(x2 - 0x79), y3, 0xe);
    return;
  case 3: {
    char *bp = buf;
    int32_t v;
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207a0),
                 (int16_t)(x2 - 0x4d), y3, 0xe);
    FUN_8018ef60(bp, (char *)guest_at(0x801207a8),
                 (int32_t)*(int8_t *)(s0 + 0x99) + 1);
    v = (*(int16_t *)(s0 + 0x22) * 3);
    if (v < 0) v += 7;
    v >>= 3;
    FUN_8012e3c4(r8, rc, bp, 0x1000, v, 0xe);
    return;
  }
  case 4:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207ac),
                 (int16_t)(x2 - 0x6e), y3, 0xe);
    return;
  case 5:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207b8),
                 (int16_t)(x2 - 0x42), y3, 8);
    return;
  case 6:
  case 10:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207c0),
                 (int16_t)(x2 - 0x63), (int16_t)(y2 - 0x10), 0xe);
    return;
  case 7:
  case 11:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207cc),
                 (int16_t)(x2 - 0x79), (int16_t)(y2 - 0x10), 8);
    return;
  case 8:
  case 12:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207d8),
                 (int16_t)(x2 - 0x58), (int16_t)(y2 - 0x10), 8);
    return;
  case 9:
  case 13:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207e4),
                 (int16_t)(x2 - 0x2c), (int16_t)(y2 - 0x10), 8);
    return;
  case 14:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207ec),
                 (int16_t)(x2 - 0x4d), (int16_t)(y2 - 0x10), 8);
    return;
  case 15:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x801207f8),
                 (int16_t)(x2 - 0x2c), (int16_t)(y2 - 0x10), 8);
    return;
  case 16: {
    int32_t v1 = (int32_t)*(int16_t *)(s0 + 0x20);
    int32_t a1v = (int32_t)*(int16_t *)(s0 + 0x22);
    if (v1 < 0) v1 += 3;
    if (a1v < 0) a1v += 3;
    hud_text_a(r8 + 4, rc, (const void *)guest_at(0x80120800),
                 (v1 >> 2) - 0x2a, a1v >> 2, 1);
    return;
  }
  case 17:
  case 18:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x80120808),
                 (int16_t)(x2 - 0x42), (int16_t)(y2 - 0x10), 8);
    return;
  case 19:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x80120810),
                 (int16_t)(x2 - 0x4d), (int16_t)(y2 - 0x10), 8);
    return;
  case 20:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x80120818),
                 (int16_t)(x2 - 0x58), (int16_t)(y2 - 0x10), 0xa);
    return;
  case 21: {
    int64_t m = (int64_t)(*(uint16_t *)(s0 + 0xb2) & 0x3fff) * 0x66666667LL;
    int32_t hi = (int32_t)(m >> 32);
    int32_t num = 9 - (int32_t)(((int32_t)(hi << 11)) >> 16);
    int32_t v;
    FUN_8018ef60(buf, (char *)guest_at(0x80120824), num);
    v = (*(int16_t *)(s0 + 0x22) * 3);
    if (v < 0) v += 3;
    v = br2_5143c_sdiv4(v) - 0x30;
    FUN_8012e3c4(r8, rc, buf, 0, (int16_t)v, 8);
    return;
  }
  case 22:
    FUN_8012e3c4(r8, rc, (const void *)guest_at(0x80120834),
                 (int16_t)(x2 - 0x63), (int16_t)(y2 - 0x10), 0xa);
    return;
  default:
    return;
  }
}

/* ---- FUN_80170fe0 @0x80170fe0 (555 insns) ----
   Save/load channel state machine. a0 = guest state struct (h@+0x8e =
   state), a1 = slot/index param. States: 0=init(load build), 0x10=commit
   tail, 0x20=alternate commit. Heap A @0x801c47b8, heap B @0x801e3cfc,
   tag table via FUN_80127680. */
int FUN_80170fe0(uint32_t a0, int32_t a1)
{
  uint8_t *s0h = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(s0h + 0x8e);
  int32_t t1 = a1;
  int32_t saved8 = (int32_t)g8(0x1f800013);
  uint8_t blk[0x20];           /* sp+0x28..0x47 block for FUN_80135a14 */
  uint16_t sp48 = 0, sp4c = 0;
  int32_t i, sel, ret;
  uint32_t heapB, heapA, sz, z0, z1, zsum, old, s7, s3n, s0v, h;
  uint8_t *s2h;
  uint32_t lhu4, lhu6;
  int32_t lh4, lh6;
#define BK16(o)  (*(int16_t *)(blk + ((o) - 0x28)))
#define BK8(o)   (blk[(o) - 0x28])
#define BKX(v)   ((int32_t)((uint32_t)(v) << 16) >> 17)

  if (s0 == 0x10) goto L10;
  if (s0 < 0x11) {
    if (s0 != 0) goto ret0;
    goto init;
  }
  if (s0 == 0x20) goto L20;
  goto ret0;

init:
  /* ---- state 0: build save image ---- */
  if (FUN_8015643c(0,0x17) == 0) goto ret0;
  if (FUN_8018cfdc(t1 + 0xa0, guest_of(FUN_80127680())) == 0)
    goto ret0;
  FUN_801274d8(0);
  {
    uint8_t *s5 = (uint8_t *)guest_at(0x801f4a8);
    int32_t *tbl = (int *)FUN_80127680();
    *(int16_t *)(s5 + 4) = 0;

    /* alloc tag0 + tag1, then FUN_8016dadc */
    heapB = g32(0x801e3cfc);
    z0 = br2_alloc54(heapB, tbl, 0);
    if (z0 != 0) {
      uint32_t mid, fin, r;
      old = heapB;
      *(uint32_t *)(s5 + 0) = 0x801c11b0u;
      mid = old + z0;                    /* heapB advance #1 (delay slot) */
      BR2_W32(0x801e3cfc, mid);
      z1 = br2_alloc54(mid, tbl, 1);
      zsum = z0 + z1;
      heapA = g32(0x801c47b8);
      fin = mid + zsum;                  /* heapB advance #2 (delay slot) */
      BR2_W32(0x801e3cfc, fin);
      r = (uint32_t)FUN_8016dadc(old, mid, 0x801c11b0u, heapA,
                               g32(0x801b4844u + (uint32_t)t1 * 4));
      BR2_W32(0x801c47b8, r);
      *(int16_t *)(s5 + 4) = (int16_t)(*(int16_t *)(s5 + 4) + 1);
    }
    *(int16_t *)(s5 + 0xc) = 0;

    /* alloc tag2: 0x17 character-record slots */
    heapB = g32(0x801e3cfc);
    if (br2_alloc54(heapB, tbl, 2) != 0) {
      uint8_t *rec = (uint8_t *)guest_at(0x801e3d08);
      uint8_t *s2p = rec + 4;
      int32_t s3i;
      uint32_t s4o = (uint32_t)t1 * 23;
      FUN_8016cb20(g32(0x801e3cfc));
      *(uint32_t *)(s5 + 8) = 0x801e3d08u;
      for (s3i = 0; s3i < 0x17; s3i++, rec += 0x28, s2p += 0x28) {
        uint32_t cp = 0x801b4878u + s4o + (uint32_t)s3i;
        uint32_t ch = g8(cp);
        if (ch == 0) break;
        if (ch == 0xff) {
          *(uint32_t *)rec = 0;
          *(int16_t *)(s2p + 0) = 0;
          *(int16_t *)(s2p + 2) = 0;
          *(int16_t *)(s2p + 4) = 0;
          *(int16_t *)(rec + 0xc) = 1;
          *(int16_t *)(rec + 0xe) = 0;
          *(uint32_t *)(rec + 0x20) = 0;
          *(uint32_t *)(rec + 0x24) = 0;
          *(int16_t *)(s5 + 0xc) =
            (int16_t)(*(int16_t *)(s5 + 0xc) + 1);
        } else {
          uint32_t r = (uint32_t)FUN_8016cc5c(
              g32(0x801e3cfc), 0x801e3d08u + (uint32_t)s3i * 0x28u,
              g32(0x801c47b8), (uint32_t)(s3i + 1),
              1, &sp48, ch, 0, 0);
          BR2_W32(0x801c47b8, r);
          *(int16_t *)(s5 + 0xc) =
            (int16_t)(*(int16_t *)(s5 + 0xc) + (int32_t)sp48);
        }
      }
    }

    /* alloc tag3 */
    *(int16_t *)(s5 + 0x14) = 0;
    heapB = g32(0x801e3cfc);
    if (br2_alloc54(heapB, tbl, 3) != 0) {
      FUN_8016cb20(g32(0x801e3cfc));
      *(uint32_t *)(s5 + 0x10) = 0x801c4588u;
      {
        uint32_t r = (uint32_t)FUN_8016cc5c(
            g32(0x801e3cfc), 0x801c4588u, g32(0x801c47b8), 0,
            0, &sp4c, g8(0x801b49a4u + (uint32_t)t1), 0, 0);
        BR2_W32(0x801c47b8, r);
        *(int16_t *)(s5 + 0x14) = (int16_t)sp4c;
      }
    }

    /* alloc tags 0x12,0x13,0x14 with FUN_8018bf40(heapB,0,0,0) */
    {
      static const int32_t tg[3] = { 0x12, 0x13, 0x14 };
      for (i = 0; i < 3; i++) {
        heapB = g32(0x801e3cfc);
        sz = br2_alloc54(heapB, tbl, tg[i]);
        if (sz != 0) {
          FUN_8018bf40((int32_t)g32(0x801e3cfc), 0, 0, 0);
          BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
        }
      }
    }

    /* alloc tags 0x20..0x25, two FUN_8018bf40 calls each */
    {
      uint32_t s0p = 0x801b49b4u;
      for (i = 0; i < 6; i++, s0p += 4) {
        heapB = g32(0x801e3cfc);
        sz = br2_alloc54(heapB, tbl, 0x20 + i);
        if (sz != 0) {
          uint16_t w0 = g16(s0p);
          uint16_t w1 = g16(s0p + 2);
          FUN_8018bf40((int32_t)g32(0x801e3cfc), w0, w1, 1);
          FUN_8018bf40((int32_t)g32(0x801e3cfc), w0, w1 + 0x10, 1);
          BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
        }
      }
    }

    /* ---- render/param block: 5x FUN_80135a14 ---- */
    s7 = g32(0x801b1674u + (uint32_t)t1 * 4);
    {
      uint8_t *s4p = (uint8_t *)guest_at(0x801c46c8);
      s2h = (uint8_t *)guest_at(0x801b1524u + (uint32_t)t1 * 8);
      *(int16_t *)(s5 + 0x1c) = 5;
      *(uint32_t *)(s5 + 0x18) = 0x801c46c8u;
      if (t1 < 9) {
        lhu4 = *(uint16_t *)(s2h + 4);
        lhu6 = *(uint16_t *)(s2h + 6);
        lh4 = *(int16_t *)(s2h + 4);
        lh6 = *(int16_t *)(s2h + 6);
        s3n = -(int32_t)s7;
        s0v = (s3n < 0 ? s3n + 0xff : s3n) >> 8;
        memset(blk, 0, sizeof blk);

        /* call 1: a0 = s4+0xa0 */
        BK16(0x28) = (int16_t)(-(int32_t)lhu4);
        BK16(0x2a) = 0;
        BK16(0x2c) = (int16_t)lhu6;
        BK16(0x30) = (int16_t)(lh4 * 2);
        BK16(0x32) = 0; BK16(0x34) = 0;
        BK16(0x38) = 0; BK16(0x3a) = 0;
        BK16(0x3c) = (int16_t)(-(lh6 * 2));
        BK8(0x40) = 0x40; BK8(0x41) = 0x40; BK8(0x42) = 0x80;
        ret = (int32_t)FUN_80135a14(0x801c46c8u + 0xa0,
                    g32(0x801c47b8), blk, blk + 8,
                    blk + 0x10, BKX(lhu4), BKX(lhu6), blk + 0x18);
        BR2_W32(0x801c47b8, (uint32_t)ret);

        /* call 2: a0 = s4 */
        BK16(0x2a) = (int16_t)s7;
        BK16(0x28) = (int16_t)lhu4;
        BK16(0x2c) = (int16_t)lhu6;
        BK16(0x30) = 0; BK16(0x32) = 0;
        BK16(0x34) = (int16_t)(-(lh6 * 2));
        BK16(0x38) = 0;
        BK16(0x3a) = (int16_t)s3n;
        BK16(0x3c) = 0;
        BK8(0x40) = 0x40; BK8(0x41) = 0x80; BK8(0x42) = 0x40;
        ret = (int32_t)FUN_80135a14(0x801c46c8u, (uint32_t)ret,
                    blk, blk + 8, blk + 0x10, BKX(lhu6), s0v,
                    blk + 0x18);
        BR2_W32(0x801c47b8, (uint32_t)ret);

        /* call 3: a0 = s4+0x28 */
        BK16(0x2a) = (int16_t)s7;
        BK16(0x28) = (int16_t)(-(int32_t)lhu4);
        BK16(0x2c) = (int16_t)(-(int32_t)lhu6);
        BK16(0x30) = 0; BK16(0x32) = 0; BK16(0x38) = 0;
        BK16(0x34) = (int16_t)(lh6 * 2);
        BK16(0x3a) = (int16_t)s3n;
        BK16(0x3c) = 0;
        ret = (int32_t)FUN_80135a14(0x801c46c8u + 0x28, (uint32_t)ret,
                    blk, blk + 8, blk + 0x10, BKX(lhu6), s0v,
                    blk + 0x18);
        BR2_W32(0x801c47b8, (uint32_t)ret);

        /* call 4: a0 = s4+0x50 */
        BK16(0x2a) = (int16_t)s7;
        BK16(0x28) = (int16_t)lhu4;
        BK16(0x2c) = (int16_t)(-(int32_t)lhu6);
        BK16(0x30) = (int16_t)(-(lh4 * 2));
        BK16(0x32) = 0; BK16(0x34) = 0; BK16(0x38) = 0;
        BK16(0x3a) = (int16_t)s3n;
        BK16(0x3c) = 0;
        ret = (int32_t)FUN_80135a14(0x801c46c8u + 0x50, (uint32_t)ret,
                    blk, blk + 8, blk + 0x10, BKX(lhu4), s0v,
                    blk + 0x18);
        BR2_W32(0x801c47b8, (uint32_t)ret);

        /* call 5: a0 = s4+0x78 */
        BK16(0x2a) = (int16_t)s7;
        BK16(0x28) = (int16_t)(-(int32_t)lhu4);
        BK16(0x2c) = (int16_t)lhu6;
        BK16(0x30) = (int16_t)(lh4 * 2);
        BK16(0x32) = 0; BK16(0x34) = 0; BK16(0x38) = 0;
        BK16(0x3a) = (int16_t)s3n;
        BK16(0x3c) = 0;
        ret = (int32_t)FUN_80135a14(0x801c46c8u + 0x78, (uint32_t)ret,
                    blk, blk + 8, blk + 0x10, BKX(lhu4), s0v,
                    blk + 0x18);
        BR2_W32(0x801c47b8, (uint32_t)ret);
      } else {
        *(int16_t *)(s4p + 0xc) = 1;
        *(int16_t *)(s4p + 0x34) = 1;
        *(int16_t *)(s4p + 0x5c) = 1;
        *(int16_t *)(s4p + 0x84) = 1;
        *(int16_t *)(s4p + 0xac) = 1;
      }
    }

    /* ---- common tail: 10 slot allocs @0x801c4420 ---- */
    for (i = 0; i < 10; i++) {
      uint32_t slot = 0x801c4420u + (uint32_t)i * 4;
      h = (g32(0x801c47b8) + 3) & ~3u;
      BR2_W32(0x801c47b8, h);
      BR2_W32(slot, h);
      sz = br2_alloc54(h, tbl, 0x40 + i);
      if (sz != 0) {
        BR2_W32(0x801c47b8, g32(0x801c47b8) + sz);
        FUN_801aa96c(g32(slot));
      } else {
        BR2_W32(slot, 0);
      }
    }
    FUN_8019a460(0);
    FUN_801274d8((uint32_t)saved8);
    s0 = 0x10;
  }

L10:
  /* commit tail: also reached by state-0 fallthrough and state 0x10 */
  if (t1 < 0) sel = 7;
  else if (t1 < 8) sel = t1;
  else if (t1 == 0xb) sel = 0xb;
  else sel = 7;
  if (FUN_8018cfdc(sel + 0xb0, guest_of(FUN_80127680())) == 0)
    goto ret0;
  FUN_801566a0(guest_of(FUN_80127680()), 2);
  FUN_801566d4((uint32_t)t1, 2);

L20:
  if (FUN_8018cfdc(0x88, g32(0x801c47b8)) == 0) goto ret0;
  FUN_801566a0(g32(0x801c47b8), 1);
  FUN_801566d4(0, 1);
  return 1;

ret0:
  *(int16_t *)(s0h + 0x8e) = (int16_t)s0;
  return 0;
#undef BK16
#undef BK8
#undef BKX
}

/* ---- FUN_8015caa8 @0x8015caa8 (605 insns) ----
   Dialog/message dispatcher. a0 = guest dialog-state ptr (fields
   lhu@+8, lbu/lh@+0x18..0x2a), a1 = selector (low16 dispatched).
   Sibling of FUN_8015bdc0. Status flag @0x801c15f0 (=0x801c11e0+0x410). */
int FUN_8015caa8(uint32_t a0, uint32_t a1)
{
  uint8_t *s3 = (uint8_t *)guest_at(a0);
  uint8_t *g  = (uint8_t *)guest_at(0x1f800000);
  uint32_t ctx8 = *(uint32_t *)(g + 8);
  uint32_t ctxc = *(uint32_t *)(g + 0xc);
  uint32_t sel = a1 & 0xffff;
  uint32_t flag, v0;
  char buf[16];

#define CAA8_E58C(str,sz,x) \
  FUN_8012e58c(ctx8,ctxc,(const void *)guest_at(str),(sz),0x1000,(x))
#define CAA8_STATUS() do { BR2_W32(0x801c15f0, 1); return 0; } while (0)
#define CAA8_GATE() do { \
    if ((g16(0x1f80007c) & 0xe0) == 0) return 0; \
    FUN_80156ef4(0x400,0xc,0); \
  } while (0)

  FUN_8012cd9c(0x20,0xffffffff);
  if (sel == (uint32_t)(*(uint16_t *)(s3 + 8) + 0x100)) {
    flag = 0;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121270),0x1000,0x1000,0);
  } else {
    flag = 1;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121274),0x1000,0x1000,flag);
  }

  if (sel == 0x100) {
    FUN_8012cd9c(0xffffffff,0x18);
    CAA8_E58C(0x80121358,0x1000,0);
    FUN_8012bf0c(3,0,4,7);
    FUN_8012cdd8(0,0x10);
    return 0;
  }

  if (sel < 0x101) {
    switch (sel) {
    case 1: {
      uint8_t *c0, *c8b;
      uint32_t xv;
      CAA8_GATE();
      c0  = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc0));
      c8b = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc8));
      c0[0x1d] = c8b[0];
      c0[7]    = s3[0x18];
      c0[8]    = s3[0x1a];
      c0[9]    = s3[0x1c];
      c0[6]    = s3[0x1e];
      c0[5]    = s3[0x1e];
      c0[0x11] = s3[0x20];
      c0[0x25] = s3[0x22];
      c0[0x24] = s3[0x2a];
      FUN_8015f6a0(0);
      *(uint16_t *)guest_at(*(uint32_t *)(g + 0xc8)) = c0[0x1d];
      FUN_8018babc(c0[0x24]);
      xv = (uint32_t)c0[0xf] << 3;
      FUN_80156048((short)c0[0xe],
                   (short)(int32_t)(((xv * (uint64_t)0x66666667u) >> 32) >> 2));
      FUN_801561c8((uint16_t)(c0[0xc] ^ 1));
      *(uint16_t *)(s3 + 0x18) = c0[7];
      *(uint16_t *)(s3 + 0x1a) = c0[8];
      *(uint16_t *)(s3 + 0x1c) = c0[9];
      *(uint16_t *)(s3 + 0x1e) = c0[5];
      *(uint16_t *)(s3 + 0x20) = c0[0x11];
      *(uint16_t *)(s3 + 0x22) = c0[0x25];
      *(uint16_t *)(s3 + 0x2a) = c0[0x24];
      return 0;
    }
    case 2:
      CAA8_GATE();
      FUN_80161630(0);
      return 0;
    case 3:
      if (FUN_80129b48(a0 + 0x18,7) == 0) return 0;
      CAA8_STATUS();
    case 4:
      if (FUN_80129b48(a0 + 0x1a,7) == 0) return 0;
      CAA8_STATUS();
    case 5:
      if (FUN_80129978(a0 + 0x1c,4,1) == 0) return 0;
      CAA8_STATUS();
    case 6:
      if (FUN_80129b48(a0 + 0x1e,3) == 0) return 0;
      CAA8_STATUS();
    default:
      return 0;
    }
  }

  if (sel < 0x109) {
    switch (sel) {
    case 7:
      if (FUN_80129978(a0 + 0x20,1,1) == 0) return 0;
      CAA8_STATUS();
    case 8:
      if (FUN_80129978(a0 + 0x22,3,1) == 0) return 0;
      CAA8_STATUS();
    case 9:
      CAA8_GATE();
      FUN_8015d420();
      return 0;
    case 0xa:
      CAA8_GATE();
      FUN_8015ebf8();
      return 0;
    case 0xb:
      CAA8_GATE();
      FUN_8016088c();
      return 0;
    case 0xc: {
      int32_t av;
      if (FUN_80129978(a0 + 0x2a,1,1) == 0) return 0;
      BR2_W32(0x801c15f0, 1);
      av = *(int16_t *)(s3 + 0x2a);
      if (av == 0)      FUN_8018babc(0);
      else if (av == 1) FUN_8018babc(3);
      else              return 0;
      return 0;
    }
    case 0xd:
      CAA8_GATE();
      {
        uint8_t *d = (uint8_t *)guest_at(0x801b1280);
        *(uint16_t *)(s3 + 0x18) = d[7];
        *(uint16_t *)(s3 + 0x1a) = d[8];
        *(uint16_t *)(s3 + 0x1c) = d[9];
        *(uint16_t *)(s3 + 0x1e) = d[6];
        *(uint16_t *)(s3 + 0x20) = d[0x11];
        *(uint16_t *)(s3 + 0x22) = 0;
        *(uint16_t *)(s3 + 0x2a) = 0;
      }
      CAA8_STATUS();
    default:
      return 0;
    }
  }

  /* sel >= 0x101 */
  switch (sel) {
  case 0x101:
    CAA8_E58C(0x80121360,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x102:
    CAA8_E58C(0x8012136c,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x103:
    CAA8_E58C(0x80121380,0x1000,flag);
    FUN_8018ef60(buf,(char *)guest_at(0x801212d0),
                 (int32_t)*(int16_t *)(s3 + 0x18) + 1);
    FUN_8012e58c(ctx8,ctxc,(const void *)buf,0x160,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x104:
    CAA8_E58C(0x8012138c,0x1000,flag);
    FUN_8018ef60(buf,(char *)guest_at(0x801212d0),
                 (int32_t)*(int16_t *)(s3 + 0x1a) + 1);
    FUN_8012e58c(ctx8,ctxc,(const void *)buf,0x160,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x105:
    CAA8_E58C(0x8012139c,0x1000,flag);
    FUN_8012e58c(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b4060u +
                     (uint32_t)*(int16_t *)(s3 + 0x1c) * 4)),
                 0x160,0x1000,flag);
    if (*(int16_t *)(s3 + 0x1c) == 4) {
      FUN_8012cdd8(0,0xc);
      return 0;
    }
    FUN_8012e8e8(ctx8,ctxc,(const void *)guest_at(0x801213a8),
                 0x180,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x106:
    CAA8_E58C(0x801213b0,0x1000,flag);
    FUN_8018ef60(buf,(char *)guest_at(0x801212d0),
                 (int32_t)*(int16_t *)(s3 + 0x1e) + 1);
    FUN_8012e58c(ctx8,ctxc,(const void *)buf,0x160,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x107:
    CAA8_E58C(0x801213bc,0x1000,flag);
    FUN_8012e58c(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b4074u +
                     (uint32_t)*(int16_t *)(s3 + 0x20) * 4)),
                 0x160,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x108:
    CAA8_E58C(0x801213d0,0x1000,flag);
    FUN_8012e58c(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b407cu +
                     (uint32_t)*(int16_t *)(s3 + 0x22) * 4)),
                 0x160,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x109:
    CAA8_E58C(0x801213e0,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x10a:
    CAA8_E58C(0x801213f0,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x10b:
    CAA8_E58C(0x80121404,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x10c:
    CAA8_E58C(0x80121410,0x1000,flag);
    FUN_8012e58c(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b3f70u +
                     (uint32_t)*(int16_t *)(s3 + 0x2a) * 4)),
                 0x160,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x10d:
    CAA8_E58C(0x80121310,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  case 0x10e:
    CAA8_E58C(0x80121318,0x1000,flag);
    FUN_8012cdd8(0,0xc);
    return 0;
  default:
    return 0;
  }
#undef CAA8_E58C
#undef CAA8_STATUS
#undef CAA8_GATE
}

/* ---- FUN_80171890 @0x80171890 (526 insns) ----
   Sibling of FUN_80170fe0: same heap/tag machinery, different state
   layout. fp = a0 guest state ptr (h@+0x8e), s4 = a1 guest param block
   (lbu@2,3,4; ptr@0x28,0x2c), a2 = gate flag. States 0/0x10/0x20/0x30/0x40. */
int FUN_80171890(uint32_t a0, uint8_t *s4, int32_t a2)
{
  uint8_t *fp = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(fp + 0x8e);
  uint32_t s5 = s4[2];
  uint32_t s2 = s4[4] & 3;
  uint32_t s6 = g8(0x801b49f8u + s4[3]);
  int32_t saved8;
  uint8_t sp28 = 0;
  uint32_t heapB, heapA, sz, s1v, p, v1, i;
  int *tbl = (int *)FUN_80127680();   /* tag table; pure getter */
#define H90W16(a,v) BR2_WS16(a,v)

  if (s0 == 0x20) goto L20;
  if (s0 < 0x21) {
    if (s0 == 0) goto L00;
    if (s0 == 0x10) goto L10;
    goto ret0;
  }
  if (s0 == 0x30) goto L30;
  if (s0 == 0x40) goto L40;
  goto ret0;

L00:
  /* state 0: quick commit, requires a2 == 0 */
  if (a2 != 0) goto ret0;
  heapB = g32(0x801e3cfc);
  if (FUN_8018cfdc((int32_t)(s6 + 0xc4), heapB) == 0) goto ret0;
  if (s5 == 0) {
    FUN_801566a0(heapB, 3);
    FUN_801566d4(s4[3], 3);
  } else {
    FUN_801566a0(heapB, 4);
    FUN_801566d4(s4[3], 4);
  }
  s0 = 0x10;
  goto ret_nc;                 /* j 0x801720cc: return 0, no store */

L10:
  /* state 0x10: build image (two variants on s5) */
  if (FUN_8018cfdc((int32_t)(s6 + s2 + 0xc0),
                   guest_of(FUN_80127680())) == 0) goto ret0;
  saved8 = (int32_t)g8(0x1f800013);
  FUN_801274d8(0);
  heapA = g32(0x801c47b8);
  BR2_W32(0x801e3cfc, 0x800f0000u);
  BR2_W32(0x801f4f18, heapA);

  /* tag0 + cc5c -> p+0x10 / p+0 */
  heapB = g32(0x801e3cfc);
  sz = br2_alloc54(heapB, tbl, 0);
  FUN_8016cb20(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801beff0u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 0);
  } else {
    s1v = 0x801c00d0u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 3, 3);
  }
  BR2_W32(0x801c47b8, heapA);
  p = *(uint32_t *)(s4 + 0x28);
  *(uint32_t *)guest_at(p + 0x10) = s1v;
  *(uint8_t *)guest_at(p) = sp28;
  BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);

  /* tag1 + cc5c -> p+0x14 / p+1 */
  heapB = g32(0x801e3cfc);
  sz = br2_alloc54(heapB, tbl, 1);
  FUN_8016cb20(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801bf860u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0x81, 0);
  } else {
    s1v = 0x801c0940u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0x81, 3, 3);
  }
  BR2_W32(0x801c47b8, heapA);
  p = *(uint32_t *)(s4 + 0x28);
  *(uint32_t *)guest_at(p + 0x14) = s1v;
  *(uint8_t *)guest_at(p + 1) = sp28;
  BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);

  /* tag2 + cc5c -> p+0x34 / p+0x24 */
  heapB = g32(0x801e3cfc);
  sz = br2_alloc54(heapB, tbl, 2);
  FUN_8016cb20(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801bf428u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 0);
  } else {
    s1v = 0x801c0508u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 3, 3);
  }
  BR2_W32(0x801c47b8, heapA);
  p = *(uint32_t *)(s4 + 0x28);
  *(uint32_t *)guest_at(p + 0x34) = s1v;
  *(uint8_t *)guest_at(p + 0x24) = sp28;
  BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);

  /* tag3 + cc5c -> p+0x38 / p+0x25 */
  heapB = g32(0x801e3cfc);
  sz = br2_alloc54(heapB, tbl, 3);
  FUN_8016cb20(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801bfc98u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0x81, 0);
  } else {
    s1v = 0x801c0d78u;
    heapA = (uint32_t)FUN_8016cc5c(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0x81, 3, 3);
  }
  BR2_W32(0x801c47b8, heapA);
  p = *(uint32_t *)(s4 + 0x28);
  *(uint32_t *)guest_at(p + 0x38) = s1v;
  *(uint8_t *)guest_at(p + 0x25) = sp28;
  BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);

  /* tags 4,5,6: bf40 strips; tag7,8: double bf40 */
  {
    uint32_t b0 = s5 ? 0x2c0 : 0x200;
    uint32_t b1 = s5 ? 0x30 : 0;
    /* tag4 */
    sz = br2_alloc54(g32(0x801e3cfc), tbl, 4);
    if (sz) {
      FUN_8018bf40((int32_t)g32(0x801e3cfc), (int32_t)b0, 0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag5 */
    sz = br2_alloc54(g32(0x801e3cfc), tbl, 5);
    if (sz) {
      FUN_8018bf40((int32_t)g32(0x801e3cfc), (int32_t)(b0 + 0x40), 0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag6 */
    sz = br2_alloc54(g32(0x801e3cfc), tbl, 6);
    if (sz) {
      FUN_8018bf40((int32_t)g32(0x801e3cfc), (int32_t)(b0 + 0x80), 0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag7: s3=0x1e0 */
    sz = br2_alloc54(g32(0x801e3cfc), tbl, 7);
    if (sz) {
      FUN_8018bf40((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1e0, 1);
      FUN_8018bf40((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1f0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag8: s0 += 0x10 unconditionally (delay slot) */
    b1 += 0x10;
    sz = br2_alloc54(g32(0x801e3cfc), tbl, 8);
    if (sz) {
      FUN_8018bf40((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1e0, 1);
      FUN_8018bf40((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1f0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
  }
  FUN_8019a460(0);
  FUN_801274d8((uint32_t)saved8);
  s0 = 0x20;
  /* fall into L20 */

L20:
  /* state 0x20 */
  if (FUN_8018cfdc((int32_t)(s6 + 0x25),
                   guest_of(FUN_80127680())) == 0) goto ret0;
  saved8 = (int32_t)g8(0x1f800013);
  FUN_801274d8(0);
  heapB = g32(0x801e3cfc);
  sz = br2_alloc54(heapB, tbl, 0);
  if (sz) {
    if (s5 == 0)
      FUN_8018bfcc(heapB, 0x60, 0x1e0, 1, 0x3c0, 0x140, 1);
    else
      FUN_8018bfcc(heapB, 0x70, 0x1e0, 1, 0x3c0, 0x140, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
  }
  v1 = s5 ? 0x3c : 0x28;
  {
    uint32_t slot = 0x801c43a8u + v1 * 4;
    for (i = 0; i < 0x14; i++, slot += 4) {
      uint32_t h = (g32(0x801c47b8) + 3) & ~3u;
      uint32_t r;
      BR2_W32(0x801c47b8, h);
      BR2_W32(slot, h);
      sz = br2_alloc54(h, tbl, (int32_t)i + 1);
      if (sz) {
        BR2_W32(0x801c47b8, g32(0x801c47b8) + sz);
        FUN_801aa96c(h);
      } else {
        BR2_W32(slot, 0);
      }
    }
  }
  FUN_8019a460(0);
  FUN_801274d8((uint32_t)saved8);
  s0 = 0x20;                    /* delay slot value stored by ret0 */
  goto ret0;

L30:
  /* state 0x30 */
  heapA = (g32(0x801c47b8) + 3) & ~3u;
  BR2_W32(0x801c47b8, heapA);
  if (FUN_8018cfdc((int32_t)(s6 + s2 + 0x20), heapA) == 0) goto ret0;
  saved8 = (int32_t)g8(0x1f800013);
  FUN_801274d8(0);
  heapA = g32(0x801c47b8);
  s4[0xb] = 0;
  *(uint32_t *)(s4 + 0x30) = heapA;
  sz = *(uint32_t *)guest_at(heapA);   /* lw (v0): size at heap head */
  BR2_W32(0x801c47b8, heapA + sz);
  s0 = 0x40;
  /* fall into L40 */

L40:
  /* state 0x40 */
  if (s5 != 0) {
    uint32_t q = *(uint32_t *)(s4 + 0x2c);
    if (s4[3] == *(uint8_t *)guest_at(q + 3)) return 1;
  }
  heapA = g32(0x801c47b8);
  if (FUN_8018cfdc((int32_t)(s6 + 0x24), heapA) == 0) goto ret0;
  heapA = g32(0x801c47b8);
  sz = *(uint32_t *)guest_at(heapA);
  BR2_W32(0x801c47b8, heapA + sz);
  return 1;

ret0:
  *(int16_t *)(fp + 0x8e) = (int16_t)s0;
ret_nc:
  return 0;
#undef H90W16
}

/* ======== 0x8018123c ======== */

/* FUN_8018123c @0x8018123c (~716 insns). Per-slot action-record dump.
 * a0 = guest ptr to a state block; byte @+2 = slot index (s4).
 * Guest tables:
 *   u16  @0x1f800082            global enable flags (lhu/lh)
 *   u16  @0x1f800068 + idx*2    per-slot flag word (& 0x10 tested)
 *   u16  @0x801f5950 + idx*2    tick counter (bit0 selects print mode)
 *   s16  @0x801f55b0 + idx*2    slot state A (-1/0/1/2)
 *   s16  @0x801f55b8 + idx*4    record count-1 (lo) / secondary ctr (hi)
 *   rec  @0x801f5848 + idx*132 + i*12   6 x s16 fields per record,
 *                                    11 records per slot (132 bytes)
 *   u32  @0x801f5558 + (idx*11+i)*4    per-record flag word
 * Prints through print_dispatch (formats @0x80123114..0x80123254). */
void FUN_8018123c(uint32_t a0)
{
  uint8_t *st = (uint8_t *)guest_at(a0);
  uint32_t idx = st[2];                       /* s4: slot index */
  uint16_t *cnt5950 = (uint16_t *)guest_at(0x801f5950 + idx * 2);
  int16_t *stateA  = (int16_t *)guest_at(0x801f55b0 + idx * 2);
  int16_t *cntB    = (int16_t *)guest_at(0x801f55b8 + idx * 4);
  uint8_t *rectab  = (uint8_t *)guest_at(0x801f5848);
  int16_t f82 = *(int16_t *)guest_at(0x1f800082);
  int32_t acc = 0;                            /* sp18: running total */
  int32_t s3 = 0;                             /* scaled-path streak */
  int32_t i, s0, s1;
  int16_t *rec;

  /* ---- prologue counters ---- */
  if (f82 != 0 &&
      (*(uint16_t *)guest_at(0x1f800068 + idx * 2) & 0x10) != 0)
    *cnt5950 = (uint16_t)(*cnt5950 + 1);
  if (f82 == 0)
    *(uint16_t *)((uint8_t *)cntB + 2) =
        (uint16_t)(*(uint16_t *)((uint8_t *)cntB + 2) + 1);

  /* ---- phase 1: append one record (skipped on bad state) ---- */
  {
    int32_t skip = 0;
    int32_t n = 0;
    int16_t *r = NULL;

    if (*stateA < 0) {
      skip = 1;
    } else {
      int16_t bc = *(int16_t *)(st + 0xbc);
      if (bc == -1) {
        if (*(uint32_t *)(st + 0xb8) == 0) skip = 1;
      } else if (bc != 0) {
        skip = 1;
      }
    }

    if (!skip) {
      if (*stateA == 0) {
        *cntB = 0;
        n = 0;
      } else {
        int16_t blo = *cntB;
        uint16_t bu = (uint16_t)blo;
        if (blo < 10) {
          bu = (uint16_t)(bu + 1);
          *cntB = (int16_t)bu;
          n = (int16_t)bu;
        } else {
          n = blo;
        }
      }
      r = (int16_t *)(rectab + idx * 132 + n * 12);

      {
        int32_t ba = *(int16_t *)(st + 0xba);
        int32_t b8v, d, s0v, s1v;
        if (ba < 0) ba += 0xf;
        s0v = ba >> 4;
        r[0] = (int16_t)s0v;
        b8v = *(int16_t *)(st + 0xb8);
        if (b8v < 0) b8v += 0xf;
        d = (b8v >> 4) - s0v;
        s1v = (int16_t)d;                    /* sll16/sra16 sign extend */
        r[1] = (int16_t)d;

        if (*stateA == 2) {
          uint8_t *q = (uint8_t *)guest_at(*(uint32_t *)(st + 0x38));
          int32_t c2 = *(int16_t *)(q + 0xc2);
          int32_t c4, val;
          int16_t *prev;
          uint32_t t;
          if (c2 < 0) c2 += 0xf;
          c2 >>= 4;
          c4 = *(int16_t *)(q + 0xc4);
          if (c4 < 0) c4 += 0xf;
          c4 >>= 4;
          val = c2 - c4 - s0v - s1v;
          r[2] = (int16_t)val;
          r[4] = (int16_t)val;
          prev = r - 6;                      /* record n-1 */
          t = (uint16_t)r[2] + (uint32_t)s0v + (uint32_t)s1v;
          prev[2] = (int16_t)((uint16_t)prev[2] - (uint16_t)t);
          prev[4] = (int16_t)((uint16_t)prev[4] - (uint16_t)t);
        } else {
          uint8_t *q = (uint8_t *)guest_at(*(uint32_t *)(st + 0x38));
          int32_t c2 = *(int16_t *)(q + 0xc2);
          int32_t val;
          if (c2 < 0) c2 += 0xf;
          c2 >>= 4;
          val = c2 - s0v - s1v;
          r[2] = (int16_t)val;
          r[4] = (int16_t)val;
          if (n != 0) {
            int16_t *prev = r - 6;
            uint32_t bhi = *(uint16_t *)((uint8_t *)cntB + 2);
            uint32_t t = bhi - (uint16_t)prev[0] - (uint16_t)prev[1];
            prev[4] = (int16_t)t;
          }
        }
      }

      /* record tail: clear hi counter, store bytes + flag word */
      *(uint16_t *)((uint8_t *)cntB + 2) = 0;
      r[5] = (int16_t)st[0xda];
      r[3] = (int16_t)st[0xdc];
      *(uint32_t *)(guest_at(0x801f5558) + (idx * 11 + (uint32_t)n) * 4) =
          *(uint32_t *)(st + 0x64);
    }

    print_dispatch((const char *)guest_at(0x80123118));   /* header line */

    /* ---- phase 2: dump every record 0..*cntB ---- */
    if (*cntB >= 0) {
      uint8_t *rb = rectab + idx * 132;
      i = 0;
      do {
        int16_t *cr = (int16_t *)rb;
        uint32_t cbit = (uint16_t)*cnt5950;

        if ((cbit & 1) == 0) {
          /* even tick: raw triple + (r5,r3) or single */
          int32_t r3 = cr[3], r5 = cr[5];
          if (r3 == r5)
            print_dispatch((const char *)guest_at(0x80123138),
                         cr[0], cr[1], cr[2], r3);
          else
            print_dispatch((const char *)guest_at(0x8012315c),
                         cr[0], cr[1], cr[2], r5, r3);
          s0 = r5;
          s1 = r3;
        } else {
          print_dispatch((const char *)guest_at(0x80123180),
                       cr[0], cr[1], cr[4]);
          if (i != 0) {
            int16_t *prev = (int16_t *)(rb - 12);
            int32_t sum = cr[0] + prev[1] + (prev[4] - 1);
            int32_t scaled = 0;
            if (sum < 0 ||
                ((prev[1] >= 2) && ((prev[4] + cr[0]) < 0))) {
              /* consecutive-anomaly path: scale r3/r5 by streak s3 */
              int32_t t3, t5, s1v, s0v;
              s3++;
              t3 = (int16_t)(uint16_t)cr[3];
              if (t3 == 0) {
                s0v = 0;
                s1v = 0;
              } else {
                s1v = (s3 < 9) ? (t3 * (10 - s3)) / 10 : t3 / 10;
                if (s1v == 0) s1v = 1;
                t5 = (int16_t)(uint16_t)cr[5];
                s0v = (s3 < 9) ? (t5 * (10 - s3)) / 10 : t5 / 10;
                if (s0v == 0) s0v = 1;
              }
              scaled = 1;
              s0 = s0v;
              s1 = s1v;
              if (sum < 0) {
                if (s1v == s0v)
                  print_dispatch((const char *)guest_at(0x80123198), s1v);
                else
                  print_dispatch((const char *)guest_at(0x801231a4), s0v, s1v);
              } else {
                if (s1v == s0v)
                  print_dispatch((const char *)guest_at(0x801231b4), s1v);
                else
                  print_dispatch((const char *)guest_at(0x801231c0), s0v, s1v);
              }
            }
            if (!scaled) {
              /* plain compare path */
              s3 = 0;
              s1 = cr[3];
              s0 = cr[5];
              if (s1 == s0)
                print_dispatch((const char *)guest_at(0x801231d0), s1);
              else
                print_dispatch((const char *)guest_at(0x801231dc), s0, s1);
            }
          } else {
            /* i == 0: plain compare */
            s3 = 0;
            s1 = cr[3];
            s0 = cr[5];
            if (s1 == s0)
              print_dispatch((const char *)guest_at(0x801231d0), s1);
            else
              print_dispatch((const char *)guest_at(0x801231dc), s0, s1);
          }
        }
        rec = (int16_t *)rb;

        /* ---- common tail: accumulate + date + flag letters ---- */
        acc += s0;
        {
          int32_t r3s = (int16_t)(uint16_t)rec[3];
          int32_t day = r3s / 2 + 5;
          int32_t mon = r3s * 3 / 4 + 6;
          int32_t s0n;
          uint32_t fl;
          if (day < 10) day = 10;
          if (day >= 25) day = 24;
          if (mon < 13) mon = 13;
          if (mon >= 121) mon = 120;
          print_dispatch((const char *)guest_at(0x801231ec), day, mon);

          if ((*(uint16_t *)cnt5950 & 1) == 0)   /* re-read: calls may write */
            s1 = rec[3];
          s0n = s1 + 6;
          if (s0n < 16) s0n = 16;
          print_dispatch((const char *)guest_at(0x801231f8), s0n);
          s0n = s1 * 153 / 128 + 6;
          if (s0n < 16) s0n = 16;
          print_dispatch((const char *)guest_at(0x801231f8), s0n);
          print_dispatch((const char *)guest_at(0x80123200));

          fl = *(uint32_t *)(guest_at(0x801f5558) + idx * 44 + i * 4);
          if (fl & 0x40)   print_dispatch((const char *)guest_at(0x80123204));
          if (fl & 0x80)   print_dispatch((const char *)guest_at(0x80123208));
          if (fl & 0x100)  print_dispatch((const char *)guest_at(0x8012320c));
          if (fl & 0x20000) print_dispatch((const char *)guest_at(0x801230ac));
          if (fl & 0x10000) {
            if (fl & 0x8)    print_dispatch((const char *)guest_at(0x80123210));
            if (fl & 0x10)   print_dispatch((const char *)guest_at(0x80123214));
            if (fl & 0x20)   print_dispatch((const char *)guest_at(0x80123218));
            if (fl & 0x40)   print_dispatch((const char *)guest_at(0x8012321c));
            if (fl & 0x80)   print_dispatch((const char *)guest_at(0x80123220));
            if (fl & 0x100)  print_dispatch((const char *)guest_at(0x80123224));
            if (fl & 0x400)  print_dispatch((const char *)guest_at(0x80123228));
          } else {
            if (fl & 0x8)    print_dispatch((const char *)guest_at(0x80123230));
            if (fl & 0x10)   print_dispatch((const char *)guest_at(0x80123234));
            if (fl & 0x20)   print_dispatch((const char *)guest_at(0x80123238));
            if (fl & 0x40)   print_dispatch((const char *)guest_at(0x8012323c));
            if (fl & 0x80)   print_dispatch((const char *)guest_at(0x80123240));
            if (fl & 0x100)  print_dispatch((const char *)guest_at(0x80123244));
            if (fl & 0x400)  print_dispatch((const char *)guest_at(0x80123248));
          }
          if ((int32_t)fl < 0)
            print_dispatch((const char *)guest_at(0x80123250));
          print_dispatch((const char *)guest_at(0x80123114));   /* '\n' */
        }

        i++;
        rb += 12;
      } while (i <= *cntB);
    }

    print_dispatch((const char *)guest_at(0x80123254), acc); /* ' %3d\n' */

    /* ---- epilogue: rewrite slot state A from state block ---- */
    if ((int32_t)*(uint32_t *)(st + 0x58) >= 0) {
      *stateA = 0;
    } else if (*(uint32_t *)(st + 0xb8) != 0) {
      *stateA = -1;
    } else if (*(int16_t *)(st + 0xbc) == -1) {
      *stateA = 2;
    } else {
      *stateA = 1;
    }
  }
}

/* ======== batch32: 0x801a/0x801af thunk & field-forwarding wrappers ======== */

/* ---- weak stubs for still-unported callees ---- */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_8019e3d4() { return 0; }

extern void *fn_lookup(uint32_t);
extern int FUN_8018da2c();
extern int FUN_8018de6c();
extern int FUN_8018e028();
extern int FUN_8018dac0();
extern uint  FUN_8018e4d0(void);
extern void FUN_801a73e4();
extern void FUN_801ac9b8();
extern undefined4 FUN_801a6f44(int);
extern void FUN_801a7498(int);
extern void FUN_801a74f0(int);
extern void FUN_801a48fc(int *);
extern void FUN_8019e330(uint32_t);
extern void FUN_8019e39c(void);
extern void bios_tramp_a070(uint32_t,uint32_t);
void FUN_801a1b0c(void);

/* 0x801a0bb0 — CD reset trio: FUN_8019e330(); FUN_8019e39c(); bios_tramp_a070(). */
void FUN_801a0bb0(void)
{
  FUN_8019e330(0);
  FUN_8019e39c();
  bios_tramp_a070(0,0);
  return;
}

/* 0x801a0dbc — tail thunk to FUN_8019e3d4. */
void FUN_801a0dbc(void)
{
  FUN_8019e3d4();
  return;
}

/* 0x801a1230 — tail thunk to FUN_801a1a2c. */
void FUN_801a1230(void)
{
  FUN_801a1a2c();
  return;
}

/* 0x801a1270 — tail thunk to FUN_801a1b0c. */
void FUN_801a1270(void)
{
  FUN_801a1b0c();
  return;
}

/* 0x801adca4 — rand() low16 store: *(int16_t*)a1 = (int16_t)rand(). */
void FUN_801adca4(uint32_t a0,int16_t *a1)
{
  (void)a0;
  *a1 = (int16_t)FUN_8018e4d0();
  return;
}

/* 0x801adccc — rand() word store: *(uint32_t*)a1 = rand(). */
void FUN_801adccc(uint32_t a0,uint32_t *a1)
{
  (void)a0;
  *a1 = FUN_8018e4d0();
  return;
}

/* 0x801aec10 — if (*a1) FUN_801a48fc(*a1). */
void FUN_801aec10(uint32_t *a1)
{
  if (*a1 != 0) {
    FUN_801a48fc((int *)(uintptr_t)*a1);
  }
  return;
}

/* 0x801af3cc — v = FUN_801a6f44(a1); *(u32*)(*gp4c + 0x14) = v.
 * $gp = 0x801beedc; +0x4c cell = guest 0x801bef28. */
void FUN_801af3cc(uint32_t a0,int32_t a1)
{
  (void)a0;
  *(uint32_t *)guest_at(*(uint32_t *)guest_at(0x801bef28) + 0x14) =
      FUN_801a6f44(a1);
  return;
}

/* 0x801af3f8 — identical body to FUN_801af3cc (alias entry). */
void FUN_801af3f8(uint32_t a0,int32_t a1)
{
  (void)a0;
  *(uint32_t *)guest_at(*(uint32_t *)guest_at(0x801bef28) + 0x14) =
      FUN_801a6f44(a1);
  return;
}

/* 0x801af494 — FUN_801aa0e4(a1, a0[1]). */
void FUN_801af494(int16_t *a0,void *a1)
{
  FUN_801aa0e4(a1,a0[1]);
  return;
}

/* 0x801af4bc — FUN_801aa168(a1, a0[1]). */
void FUN_801af4bc(int16_t *a0,void *a1)
{
  FUN_801aa168(a1,a0[1]);
  return;
}

/* 0x801af4e4 — FUN_801aa1e4(a1, a0[1], a0[2], a0[3]). */
void FUN_801af4e4(int16_t *a0,void *a1)
{
  FUN_801aa1e4(a1,a0[1],a0[2],a0[3]);
  return;
}

/* 0x801af518 — FUN_801aa2c8(a1, a0[1]). */
void FUN_801af518(int16_t *a0,void *a1)
{
  FUN_801aa2c8(a1,a0[1]);
  return;
}

/* 0x801af540 — FUN_801aa320(a1, a0[1]). */
void FUN_801af540(int16_t *a0,void *a1)
{
  FUN_801aa320(a1,a0[1]);
  return;
}

/* 0x801af568 — FUN_801aa374(a1, a0[1], a0[2], a0[3]). */
void FUN_801af568(int16_t *a0,void *a1)
{
  FUN_801aa374(a1,a0[1],a0[2],a0[3]);
  return;
}

/* 0x801af59c — FUN_801a73e4(a1, a0[1]). */
void FUN_801af59c(int16_t *a0,int32_t a1)
{
  FUN_801a73e4(a1,a0[1]);
  return;
}

/* 0x801af5c4 — FUN_801a7444(a1); a0 unused. */
void FUN_801af5c4(uint32_t a0,int32_t a1)
{
  (void)a0;
  FUN_801a7444(a1);
  return;
}

/* 0x801af5e4 — FUN_801a7498(a1); a0 unused. */
void FUN_801af5e4(uint32_t a0,int32_t a1)
{
  (void)a0;
  FUN_801a7498(a1);
  return;
}

/* 0x801af604 — FUN_801a74f0(a1); a0 unused. */
void FUN_801af604(uint32_t a0,int32_t a1)
{
  (void)a0;
  FUN_801a74f0(a1);
  return;
}

/* 0x801af678 — FUN_801ac9b8(a1, a0[1]). */
void FUN_801af678(int16_t *a0,int32_t a1)
{
  FUN_801ac9b8(a1,a0[1]);
  return;
}

/* 0x801af6a0 — FUN_801acb08(a1, a0[1], a0[2]) via fnmap (unported callee). */
void FUN_801af6a0(int16_t *a0,uint32_t a1)
{
  void *fn = fn_lookup(0x801acb08);
  if (fn != (void *)0x0) {
    ((void (*)(uint32_t,int,int))fn)(a1,a0[1],a0[2]);
  }
  return;
}

/* 0x801af7d4 — FUN_801b1140(a2[0], a2[1], a2[2], a1); a0 unused. */
void FUN_801af7d4(uint32_t a0,uint32_t a1,int16_t *a2)
{
  (void)a0;
  FUN_801b1140(a2[0],a2[1],a2[2],a1);
  return;
}

/* 0x801af804 — FUN_8018da2c(*a2, a1); a0 unused. */
void *FUN_801af804(uint32_t a0,undefined4 *a1,int16_t *a2)
{
  (void)a0;
  return (void *)FUN_8018da2c((uint)(int32_t)*a2,a1);
}

/* 0x801af828 — FUN_8018de6c(*a2, a1); a0 unused. */
void *FUN_801af828(uint32_t a0,uint32_t *a1,int16_t *a2)
{
  (void)a0;
  return (void *)FUN_8018de6c((uint)(int32_t)*a2,a1);
}

/* 0x801af84c — FUN_8018e028(*a2, a1); a0 unused. */
void *FUN_801af84c(uint32_t a0,uint32_t *a1,int16_t *a2)
{
  (void)a0;
  return (void *)FUN_8018e028((uint)(int32_t)*a2,a1);
}

/* 0x801af870 — FUN_8018dac0(a2[0], a2[1], a2[2], a1); a0 unused. */
void *FUN_801af870(uint32_t a0,uint32_t a1,int16_t *a2)
{
  (void)a0;
  return (void *)FUN_8018dac0((uint)(int32_t)a2[0],(uint)(int32_t)a2[1],
                              (uint32_t *)guest_at((uint32_t)a1));
}

/* 0x801af89c — FUN_8018da2c(a0[1], a1). */
void *FUN_801af89c(int16_t *a0,undefined4 *a1)
{
  return (void *)FUN_8018da2c((uint)(int32_t)a0[1],a1);
}

/* 0x801af8c0 — FUN_8018de6c(a0[1], a1). */
void *FUN_801af8c0(int16_t *a0,uint32_t *a1)
{
  return (void *)FUN_8018de6c((uint)(int32_t)a0[1],a1);
}

/* 0x801af8e4 — FUN_8018e028(a0[1], a1). */
void *FUN_801af8e4(int16_t *a0,uint32_t *a1)
{
  return (void *)FUN_8018e028((uint)(int32_t)a0[1],a1);
}

/* 0x801af908 — FUN_8018dac0(a0[1], a0[2], a1). */
void *FUN_801af908(int16_t *a0,uint32_t a1)
{
  return (void *)FUN_8018dac0((uint)(int32_t)a0[1],(uint)(int32_t)a0[2],
                              (uint32_t *)guest_at(a1));
}

/* ======== batch33: 0x801a/0x801ae/0x801af gp-cell wrappers ========
 * $gp = 0x801beedc.  lw $r,0x4c($gp)  -> u32 ctx ptr stored at guest 0x801bef28
 * lw $r,0x50($gp)  -> direct global at guest 0x801bef2c
 * lw $r,8($ctx)    -> ctx->table (guest addr); indexed by sign-extended s16. */
#define BR2_GPC   (*(uint32_t *)guest_at(0x801bef28))
#define BR2_GPF(o)   (*(uint32_t *)guest_at(BR2_GPC + (uint32_t)(o)))
#define BR2_GPS(o)   ((uint8_t *)guest_at(BR2_GPC + (uint32_t)(o)))
#define BR2_GP50 (*(uint32_t *)guest_at(0x801bef2c))

extern int FUN_801a6bd0();
extern int FUN_801a7038();
extern int FUN_801a984c();
extern int FUN_801ad2e8();
extern int *FUN_801ad424(int32_t,int32_t,uint32_t,uint32_t,uint32_t);
extern int FUN_801aad24();
extern void FUN_801ad4ac();
extern int32_t FUN_801a1f74(uint8_t *,int32_t);
extern uint32_t FUN_801a0610(uint32_t,uint32_t);
extern uint32_t FUN_801a0600(uint32_t,uint32_t,uint32_t);
extern int FUN_801ad38c(undefined4,undefined4,int,int);
extern undefined4 FUN_801a72a0(int,int);
extern void FUN_801a99e0(int,int);
extern int FUN_8018d29c();
extern int FUN_8018d298();
extern int FUN_80138040(undefined4,undefined4,undefined4,int);
extern int FUN_80142b3c(int,int);
extern undefined4 FUN_801ad284(int);
extern void FUN_801ac758(int);
extern void FUN_8019a340(uint32_t,uint32_t);
extern void FUN_8018e300(void);
extern void FUN_8018e310(void);
extern void FUN_801a482c();
extern int FUN_8018e550();
extern uint32_t guest_of(const void *);

/* 0x801a0620 — FUN_801a0610(a0,a1); FUN_801a0600(a0,0x3f,0). */
void FUN_801a0620(uint32_t a0,uint32_t a1)
{
  FUN_801a0610(a0,a1);
  FUN_801a0600(a0,0x3f,0);
  return;
}

/* 0x801a1b0c — CD channel init: crit-in, mode 3/1, table 0x801fc000, crit-out. */
void FUN_801a1b0c(void)
{
  FUN_8018e300();
  FUN_8019a670();
  FUN_8019a340(2,0x801fc000);
  FUN_8018e310();
  return;
}

/* 0x801a30c0 — ctx fn-ptr call + FUN_801a1f74(ctx,-2). */
void FUN_801a30c0(uint8_t *a0)
{
  void *fn = fn_lookup(*(uint32_t *)guest_at(0x801be798));
  uint32_t r = 0;
  if (fn != (void *)0x0) {
    r = ((uint32_t (*)(uint32_t))fn)(guest_of(a0));
  }
  *(uint32_t *)guest_at(0x801be800) = r;
  *(uint8_t *)*(uint32_t *)(a0 + 0x3c) = 0;
  FUN_801a1f74(a0,-2);
  return;
}

/* 0x801aadc4 — deep-null check then FUN_801a482c(a0). */
void FUN_801aadc4(uint8_t *a0)
{
  uint32_t *p4 = (uint32_t *)(a0 + 4);
  uint32_t *p2 = (uint32_t *)(uintptr_t)*p4;
  if (*(uint32_t *)(uintptr_t)*(uint32_t *)(p2 + 3) == 0) {
    uint32_t *q = (uint32_t *)(a0 + 0xc);
    *q = 0;
    FUN_801a482c(a0);
  }
  return;
}

/* 0x801ad298 — *(*a0)=0; if (a0[1]->f10) { a0[3]=a0[1]+f10; call 0x801b0db8(a0); } */
void FUN_801ad298(uint32_t *a0)
{
  *(uint32_t *)guest_at(a0[0]) = 0;
  {
    uint32_t p4 = a0[1];
    uint32_t v1 = *(uint32_t *)guest_at(p4 + 0x10);
    if (v1 != 0) {
      a0[3] = p4 + v1;
      {
        void *fn = fn_lookup(0x801b0db8);
        if (fn != (void *)0x0) {
          ((void (*)(uint32_t))fn)(guest_of(a0));
        }
      }
    }
  }
  return;
}

/* 0x801ad6f0 — ctx->fC = GP50 + a0[0]; call 0x801b0db8(ctx); *a1 += 4. */
void FUN_801ad6f0(int16_t *a0,uint32_t *a1)
{
  uint32_t s0 = *a1;
  uint32_t v = BR2_GP50 + (uint32_t)(int32_t)a0[0];
  BR2_GPF(0xc) = v;
  {
    void *fn = fn_lookup(0x801b0db8);
    if (fn != (void *)0x0) {
      ((void (*)(uint32_t))fn)(BR2_GPC);
    }
  }
  *a1 = s0 + 4;
  return;
}

/* 0x801adc60 — *a1 = (int8_t)(rand() magic-div). */
void FUN_801adc60(uint32_t a0,int8_t *a1)
{
  (void)a0;
  {
    int32_t r = (int32_t)FUN_8018e4d0();
    int32_t hi = (int32_t)(((int64_t)r * 266338049LL) >> 32);
    int32_t q = (hi >> 3) - (r >> 31);
    *a1 = (int8_t)q;
  }
  return;
}

/* 0x801ae85c — if (*a1==0) *a1 = FUN_801ad2e8(a0[0..3], a1). */
void FUN_801ae85c(int16_t *a0,uint32_t a1)
{
  if (*(uint32_t *)guest_at(a1) == 0) {
    int32_t r = FUN_801ad2e8(a0[0],a0[1],a0[2],a0[3],a1);
    *(uint32_t *)guest_at(a1) = (uint32_t)r;
  }
  return;
}

/* 0x801ae8a8 — return FUN_801ad38c(a0[0..3]). */
int FUN_801ae8a8(int16_t *a0)
{
  return FUN_801ad38c(a0[0],a0[1],a0[2],a0[3]);
}

/* 0x801ae8dc — *a1 = FUN_801ad38c(a0[0..3]). */
void FUN_801ae8dc(int16_t *a0,uint32_t *a1)
{
  *a1 = (uint32_t)FUN_801ad38c(a0[0],a0[1],a0[2],a0[3]);
  return;
}

/* 0x801ae91c — *a1 = FUN_801ad38c(a0[0], a0[1], ctx8[a0[2]], a0[3]). */
void FUN_801ae91c(int16_t *a0,uint32_t *a1)
{
  uint32_t b = BR2_GPF(8) + (uint32_t)(int32_t)a0[2];
  *a1 = (uint32_t)FUN_801ad38c(a0[0],a0[1],
                              *(uint32_t *)guest_at(b),a0[3]);
  return;
}

/* 0x801aeff8 — FUN_8018e550(0x801cee8, tbl[a0[1]][a0[0]], 0x801c0000). */
void FUN_801aeff8(int16_t *a0)
{
  uint32_t b = BR2_GPF(8) + (uint32_t)(int32_t)a0[0];
  uint32_t inner = *(uint32_t *)guest_at(b);
  uint32_t a1 = *(uint32_t *)guest_at(inner + (uint32_t)(int32_t)a0[1] * 4);
  FUN_8018e550((const char *)guest_at(0x801cee8),a1,0x801c0000);
  return;
}

/* 0x801af040 — same with cell 0x801ceec. */
void FUN_801af040(int16_t *a0)
{
  uint32_t b = BR2_GPF(8) + (uint32_t)(int32_t)a0[0];
  uint32_t inner = *(uint32_t *)guest_at(b);
  uint32_t a1 = *(uint32_t *)guest_at(inner + (uint32_t)(int32_t)a0[1] * 4);
  FUN_8018e550((const char *)guest_at(0x801ceec),a1,0x801c0000);
  return;
}

/* 0x801af094 — cached lookup: if (*a1==0) *a1 = FUN_801a6bd0(a1, T[a0[1]][a0[2]], a0[3], a0[4]); GPF(0x14)=1 on hit. */
void FUN_801af094(int16_t *a0,uint32_t a1)
{
  BR2_GPF(0x14) = 0;
  if (*(uint32_t *)guest_at(a1) == 0) {
    uint32_t t1 = *(uint32_t *)guest_at(0x801ff638 + (uint32_t)(int32_t)a0[1] * 4);
    uint32_t av = *(uint32_t *)guest_at(t1 + (uint32_t)(int32_t)a0[2] * 4);
    uint32_t r = (uint32_t)FUN_801a6bd0(a1,(int)av,a0[3],a0[4]);
    *(uint32_t *)guest_at(a1) = r;
    if (r != 0) {
      BR2_GPF(0x14) = 1;
    }
  }
  return;
}

/* 0x801af114 — variant: a1arg = ctx8[a3[2]] then T[a3[1]][a1arg]. */
void FUN_801af114(int16_t *a0,uint32_t a1)
{
  uint32_t idx = BR2_GPF(8) + (uint32_t)(int32_t)a0[2];
  uint32_t mid = *(uint32_t *)guest_at(idx);
  BR2_GPF(0x14) = 0;
  if (*(uint32_t *)guest_at(a1) == 0) {
    uint32_t t1 = *(uint32_t *)guest_at(0x801ff638 + (uint32_t)(int32_t)a0[1] * 4);
    uint32_t av = *(uint32_t *)guest_at(t1 + mid * 4);
    uint32_t r = (uint32_t)FUN_801a6bd0(a1,(int)av,a0[3],a0[4]);
    *(uint32_t *)guest_at(a1) = r;
    if (r != 0) {
      BR2_GPF(0x14) = 1;
    }
  }
  return;
}

/* 0x801af1a4 — variant: mid = ctx8[a0[1]] then T[mid][a0[2]]. */
void FUN_801af1a4(int16_t *a0,uint32_t a1)
{
  uint32_t idx = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
  uint32_t mid = *(uint32_t *)guest_at(idx);
  BR2_GPF(0x14) = 0;
  if (*(uint32_t *)guest_at(a1) == 0) {
    uint32_t t1 = *(uint32_t *)guest_at(0x801ff638 + mid * 4);
    uint32_t av = *(uint32_t *)guest_at(t1 + (uint32_t)(int32_t)a0[2] * 4);
    uint32_t r = (uint32_t)FUN_801a6bd0(a1,(int)av,a0[3],a0[4]);
    *(uint32_t *)guest_at(a1) = r;
    if (r != 0) {
      BR2_GPF(0x14) = 1;
    }
  }
  return;
}

/* 0x801af238 — free ctx8[a0[0]] cell via FUN_801ac758. */
void FUN_801af238(int16_t *a0)
{
  uint32_t s0 = BR2_GPF(8) + (uint32_t)(int32_t)a0[0];
  uint32_t p = *(uint32_t *)guest_at(s0);
  if (p != 0) {
    FUN_801ac758((int)p);
    *(uint32_t *)guest_at(s0) = 0;
  }
  return;
}

/* 0x801af284 — double-indirect variant of FUN_801af238. */
void FUN_801af284(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t s0 = b + *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[0]);
  uint32_t p = *(uint32_t *)guest_at(s0);
  if (p != 0) {
    FUN_801ac758((int)p);
    *(uint32_t *)guest_at(s0) = 0;
  }
  return;
}

/* 0x801af2dc — GPF(0x14) = FUN_801a7038(a1, a0[1]). */
void FUN_801af2dc(int16_t *a0,int32_t a1)
{
  BR2_GPF(0x14) = (uint32_t)FUN_801a7038(a1,a0[1]);
  return;
}

/* 0x801af310 — GPF(0x14) = FUN_801a7038(a1, ctx8[a0[1]]). */
void FUN_801af310(int16_t *a0,uint32_t a1)
{
  uint32_t av = *(uint32_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  BR2_GPF(0x14) = (uint32_t)FUN_801a7038(a1,(int)av);
  return;
}

/* 0x801af354 — GPF(0x14) = FUN_801a72a0(a1, a0[1]). */
void FUN_801af354(int16_t *a0,int32_t a1)
{
  BR2_GPF(0x14) = FUN_801a72a0(a1,a0[1]);
  return;
}

/* 0x801af388 — GPF(0x14) = FUN_801a72a0(a1, ctx8[a0[1]]). */
void FUN_801af388(int16_t *a0,uint32_t a1)
{
  uint32_t av = *(uint32_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  BR2_GPF(0x14) = FUN_801a72a0(a1,(int)av);
  return;
}

/* 0x801af424 — FUN_801a984c(a1, (int16_t)ctx8[a0[1]]). */
int FUN_801af424(int16_t *a0,uint32_t a1)
{
  int16_t v = *(int16_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  return FUN_801a984c(a1,v);
}

/* 0x801af45c — FUN_801a99e0(a1, (int16_t)ctx8[a0[1]]). */
void FUN_801af45c(int16_t *a0,uint32_t a1)
{
  int16_t v = *(int16_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  FUN_801a99e0(a1,v);
  return;
}

/* 0x801af624 — ctx8[a0[1]]==0 ? FUN_801a74f0(a1) : FUN_801a7498(a1). */
void FUN_801af624(int16_t *a0,int32_t a1)
{
  int16_t t = *(int16_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  if (t == 0) {
    FUN_801a74f0(a1);
  } else {
    FUN_801a7498(a1);
  }
  return;
}

/* 0x801af6d0 — *(*(a1)+0x48) = FUN_801aad24(a1, ctx8[a0[1]]); a1 is guest addr. */
void FUN_801af6d0(int16_t *a0,uint32_t a1)
{
  uint32_t d = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
  uint32_t av = *(uint32_t *)guest_at(d);
  uint32_t r = (uint32_t)FUN_801aad24(a1,(int)av);
  *(uint32_t *)guest_at(a1 + 0x48) = r;
  return;
}

/* 0x801af710 — if (a1) FUN_801b1140(ctx8[a0[1]] s16 x3, *a1). */
void FUN_801af710(int16_t *a0,uint32_t a1)
{
  if (a1 != 0) {
    uint32_t d = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
    uint8_t *p = guest_at(d);
    int16_t t0 = *(int16_t *)p;
    int16_t t1 = *(int16_t *)(p + 2);
    int16_t t2 = *(int16_t *)(p + 4);
    FUN_801b1140(t0,t1,t2,*(uint32_t *)guest_at(a1));
  }
  return;
}

/* 0x801af758 — FUN_80138040(a1, ctx8[a0[1]]|0, GP50+a0[2], GP50+a0[3]); store. */
void FUN_801af758(int16_t *a0,uint32_t a1)
{
  uint32_t b = BR2_GP50;
  uint32_t a2 = b + (uint32_t)(int32_t)a0[2];
  uint32_t a3 = b + (uint32_t)(int32_t)a0[3];
  int32_t r;
  if (a0[1] != 0) {
    uint32_t s0 = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
    r = FUN_80138040(a1,s0,a2,a3);
    *(uint32_t *)guest_at(s0) = (uint32_t)r;
  } else {
    r = FUN_80138040(a1,0,a2,a3);
  }
  BR2_GPF(0x14) = (uint32_t)r;
  return;
}

/* 0x801afad0 — ctx8[a0[1]]->[a0[2]] pair -> FUN_80142b3c(f0,f8); store ctx8[a0[0]]. */
void FUN_801afad0(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t inner = *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[1]);
  uint32_t e = inner + (uint32_t)(int32_t)a0[2] * 4;
  uint32_t d = b + (uint32_t)(int32_t)a0[0];
  uint32_t r = (uint32_t)FUN_80142b3c(*(uint32_t *)guest_at(e),
                                     *(uint32_t *)guest_at(e + 8));
  *(uint32_t *)guest_at(d) = r;
  return;
}

/* 0x801afa30 — s16 variant: ctx8[a0[1]]->[a0[2]] s16 -> FUN_8018d29c; store ctx8[a0[0]]. */
void FUN_801afa30(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t inner = *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[1]);
  int16_t v = *(int16_t *)guest_at(inner + (uint32_t)(int32_t)a0[2] * 2);
  uint32_t d = b + (uint32_t)(int32_t)a0[0];
  *(uint32_t *)guest_at(d) = (uint32_t)FUN_8018d29c(v);
  return;
}

/* 0x801afa80 — same with FUN_8018d298. */
void FUN_801afa80(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t inner = *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[1]);
  int16_t v = *(int16_t *)guest_at(inner + (uint32_t)(int32_t)a0[2] * 2);
  uint32_t d = b + (uint32_t)(int32_t)a0[0];
  *(uint32_t *)guest_at(d) = (uint32_t)FUN_8018d298(v);
  return;
}

/* 0x801aecdc — if (*a2) { s0=ctx8[*a2]; r=(*a1)(); *(u32*)s0=r } else (*a1)(). */
void FUN_801aecdc(uint32_t a0,uint32_t a1,int16_t *a2)
{
  (void)a0;
  {
    int32_t v1 = *a2;
    uint32_t fp = *(uint32_t *)guest_at(a1);
    void *fn = fn_lookup(fp);
    if (v1 != 0) {
      uint32_t s0 = BR2_GPF(8) + (uint32_t)v1;
      uint32_t r = 0;
      if (fn != (void *)0x0) {
        r = ((uint32_t (*)(uint32_t))fn)(a0);
      }
      *(uint32_t *)guest_at(s0) = r;
    } else if (fn != (void *)0x0) {
      ((void (*)(uint32_t))fn)(a0);
    }
  }
  return;
}

/* 0x801aebd0 — r=FUN_801ad284(*a1); *(u32*)(r + a0[1]*4) = *(u32*)(a0+6). */
void FUN_801aebd0(uint8_t *a0,uint32_t *a1)
{
  uint32_t r = FUN_801ad284(*a1);
  uint32_t d = r + ((uint32_t)(int32_t)*(int16_t *)(a0 + 2) << 2);
  *(uint32_t *)guest_at(d) = *(uint32_t *)(a0 + 6);
  return;
}

/* ======== batch34: 0x801a0a/0x801a18/0x801a51/0x801a6b/0x801a9b/0x801ac/0x801ad/0x801ae ======== */

extern void FUN_80190a04();
extern int *FUN_801a4308(int,undefined4,uint,int,int,int);
extern void FUN_801a9c24(short *,int,int);
extern void FUN_801a6ae8(void);
extern int FUN_801a6bd0();

/* 0x801a1834 — global ctx flag check + fn-ptr call; returns 1/0. */
int FUN_801a1834(void)
{
  uint32_t v1 = *(uint32_t *)guest_at(0x801be7e0);
  if ((*(uint32_t *)guest_at(v1 + 4) & 1) != 0 &&
      (*(uint32_t *)guest_at(v1) & 1) != 0) {
    uint32_t fp = *(uint32_t *)guest_at(0x801be7a8);
    void *fn = fn_lookup(fp);
    if (fn != (void *)0x0) {
      ((void (*)(void))fn)();
    }
    return 1;
  }
  return 0;
}

/* 0x801a2fb4 — state-byte dispatch; returns 1 when a0[0x46]==2. */
int FUN_801a2fb4(uint8_t *a0)
{
  if (*(uint8_t *)(a0 + 0x53) != 0) {
    if (*(uint8_t *)(a0 + 0x46) == 2) {
      return 1;
    }
    *(uint8_t *)(a0 + 0x46) = 0xfe;
    return 0;
  }
  {
    uint32_t fp = *(uint32_t *)guest_at(0x801be784);
    void *fn = fn_lookup(fp);
    if (fn != (void *)0x0) {
      ((void (*)(void))fn)();
    }
  }
  return 0;
}

/* FUN_801a0a10 lives in cd.c (disasm-verified; counter @0x801be768). */

/* 0x801a0a8c — invoke channel fn ptr; decrement count on success. */
void FUN_801a0a8c(void)
{
  int32_t i = *(int32_t *)guest_at(0x801be768);
  if (i >= 0) {
    uint32_t f = *(uint32_t *)guest_at(0x801fef60 + (uint32_t)i * 4);
    void *fn = fn_lookup(f);
    uint32_t r = 0;
    if (fn != (void *)0x0) {
      r = ((uint32_t (*)(uint32_t))fn)(0x801ff960 + (uint32_t)i * 16);
    }
    if (r != 0) {
      *(uint32_t *)guest_at(0x801be768) = (uint32_t)(i - 1);
    }
  }
  return;
}

/* 0x801a6b0c — stash a1/a2 into gp globals, FUN_801a6ae8, submit job. */
int FUN_801a6b0c(uint32_t a0,uint32_t a1,uint32_t a2)
{
  int *r;
  *(uint32_t *)guest_at(0x801bef18) = a1;
  *(uint32_t *)guest_at(0x801bef1c) = a2;
  *(uint32_t *)guest_at(0x801bef24) = a1;
  *(uint32_t *)guest_at(0x801bef00) = 0;
  FUN_801a6ae8();
  r = FUN_801a4308(a0,2,0,0x801a6b70,0,0);
  if (r != (int *)0x0) {
    return 1;
  }
  return -1;
}

/* 0x801a9bc0 — validate ctx chain then FUN_801a9c24(node). */
void FUN_801a9bc0(uint8_t *a0)
{
  if (a0 != (uint8_t *)0x0) {
    uint32_t p = *(uint32_t *)(a0 + 0x3c);
    uint8_t *q = (uint8_t *)*(uint32_t *)p;
    if (q != (uint8_t *)0x0 &&
        *(int16_t *)(q + 0xe) == (int32_t)*(uint32_t *)(q + 0x30) &&
        *(int16_t *)(q + 0xa) == (int32_t)*(uint32_t *)(q + 0x2c)) {
      FUN_801a9c24((short *)q,0,0);
    }
  }
  return;
}

/* 0x801aec40 — lazy ctx8[a0[3]] fill via FUN_801ad424. */
void FUN_801aec40(int16_t *a0)
{
  uint32_t s0 = BR2_GPF(8) + (uint32_t)(int32_t)a0[3];
  if (*(uint32_t *)guest_at(s0) == 0) {
    uint32_t stack5 = BR2_GP50 + (uint32_t)(int32_t)a0[2];
    int *jr = FUN_801ad424(a0[0],a0[1],BR2_GPC,s0,stack5);
    *(uint32_t *)guest_at(s0) = (uint32_t)(uintptr_t)jr;
  }
  return;
}

extern int *FUN_801ad424(int32_t,int32_t,uint32_t,uint32_t,uint32_t);

/* 0x801acf34 — four FUN_80190a04 dumps of a0 fields. */
void FUN_801acf34(uint32_t *a0)
{
  FUN_80190a04((const char *)guest_at(0x80126098),a0[0]);
  FUN_80190a04((const char *)guest_at(0x801260a8),a0[2]);
  FUN_80190a04((const char *)guest_at(0x801260b8),a0[8]);
  FUN_80190a04((const char *)guest_at(0x801260c8),a0[1]);
  return;
}

/* 0x801a5160 — five FUN_80190a04 field dumps. */
void FUN_801a5160(uint32_t a0,uint32_t a1)
{
  (void)a0;
  FUN_80190a04((const char *)guest_at(0x80125fd8),a1);
  FUN_80190a04((const char *)guest_at(0x80125fe8),*(uint32_t *)guest_at(a1));
  FUN_80190a04((const char *)guest_at(0x80126044),*(uint32_t *)guest_at(a1 + 4));
  FUN_80190a04((const char *)guest_at(0x80126054),*(uint32_t *)guest_at(a1 + 8) + 0x10);
  FUN_80190a04((const char *)guest_at(0x80126064),*(int16_t *)guest_at(a1 + 0xc));
  return;
}

/* 0x801a51d0 — table walk + FUN_80190a04 dump (variadic tail args a1,a2,a3). */
void FUN_801a51d0(uint32_t a0,uint32_t a1)
{
  uint32_t addr = 0x801ff268 + a1 * 12 + a0 * 60;
  uint32_t cur = *(uint32_t *)guest_at(addr);
  uint32_t first = cur;
  uint32_t off = 0;
  uint32_t v1;
  do {
    uint32_t nxt;
    v1 = off + 0x10;
    nxt = *(uint32_t *)guest_at(cur + 8);
    cur = *(uint32_t *)guest_at(cur + 4);
    off = v1 + nxt;
  } while (cur != first);
  FUN_80190a04((const char *)guest_at(0x80126074),a1,off,first);
  return;
}

/* 0x801ad4ac — FUN_801a4308 job submit; init result node from a2/a3. */
void FUN_801ad4ac(int32_t a0,int32_t a1,uint32_t a2,uint32_t a3)
{
  int *r = FUN_801a4308(a0,a1,4,0x801b0db8,0,0x1c);
  if (r != (int *)0x0) {
    uint32_t *p = *(uint32_t **)((char *)r + 0x10);
    p[0] = 0;
    p[2] = *(uint32_t *)guest_at(a2 + 8);
    p[3] = a3;
    p[1] = *(uint32_t *)guest_at(a2 + 4);
  }
  return;
}

/* 0x801aea20 — r=FUN_801ad284(*a1); r[1]=e[0]; r[2]=e[1] (e=ctx8[a0[1]]+a0[2]*4). */
void FUN_801aea20(int16_t *a0,uint32_t *a1)
{
  uint32_t r = FUN_801ad284(*a1);
  uint32_t inner = *(uint32_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  uint32_t e = inner + (uint32_t)(int32_t)a0[2] * 4;
  *(uint32_t *)guest_at(r + 4) = *(uint32_t *)guest_at(e);
  *(uint32_t *)guest_at(r + 8) = *(uint32_t *)guest_at(e + 4);
  return;
}

/* ======== batch35: 0x801a4b/0x801a50/0x801a53/0x801a86/0x801aa3/0x801ad4/0x801ad5/0x801aed ======== */

#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a5bd0() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a6538() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a5d90() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a66dc() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a5f50() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a6350() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a6150() { return 0; }
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
int FUN_801a6880() { return 0; }

/* 0x801a4be8 — register node a0 (guest addr) into table slot idx*72+0x40. */
void FUN_801a4be8(uint32_t a0)
{
  int32_t idx = *(int8_t *)guest_at(a0 + 0x14);
  uint32_t e = 0x801ff398 + (uint32_t)idx * 72;
  *(uint32_t *)guest_at(e + 0x40) = a0;
  *(uint32_t *)guest_at(0x801beef8) = (uint32_t)idx;
  if (*(int8_t *)guest_at(a0 + 0x18) != 0) {
    FUN_801a482c((int)a0);
  } else {
    uint32_t fp = *(uint32_t *)guest_at(a0 + 8);
    uint32_t arg = *(uint32_t *)guest_at(a0 + 0x10);
    void *fn = fn_lookup(fp);
    if (fn != (void *)0x0) {
      ((void (*)(uint32_t))fn)(arg);
    }
  }
  return;
}

/* 0x801a4cd8 — count nodes across 16 buckets of table a0; returns count. */
int FUN_801a4cd8(uint32_t a0)
{
  uint32_t v1 = 0x801ff398 + a0 * 72;
  int32_t s0 = 0;
  int32_t i;
  for (i = 0; i < 0x10; i++) {
    uint32_t v0 = *(uint32_t *)guest_at(v1);
    if (v0 != 0) {
      for (;;) {
        v0 = *(uint32_t *)guest_at(v0);
        s0++;
        if (v0 == 0) break;
      }
    }
    v1 += 4;
  }
  FUN_80190a04((const char *)guest_at(0x80126008),s0);
  return s0;
}

/* 0x801a536c — count flagged (fC!=0) nodes; FUN_80190a04(fmt, a1, cnt). */
void FUN_801a536c(uint32_t a0,uint32_t a1)
{
  uint32_t addr = 0x801ff268 + a1 * 12 + a0 * 60;
  uint32_t first = *(uint32_t *)guest_at(addr);
  uint32_t cur = first;
  uint32_t cnt = 0;
  if (cur != 0) {
    for (;;) {
      if (*(int16_t *)guest_at(cur + 0xc) != 0) {
        cnt++;
      }
      cur = *(uint32_t *)guest_at(cur + 4);
      if (cur == first) break;
    }
  }
  FUN_80190a04((const char *)guest_at(0x80126088),a1,cnt);
  return;
}

/* 0x801a53e8 — FUN_801a5160 dump for every node in ring. */
void FUN_801a53e8(uint32_t a0,uint32_t a1)
{
  uint32_t addr = 0x801ff268 + a1 * 12 + a0 * 60;
  uint32_t first = *(uint32_t *)guest_at(addr);
  uint32_t cur = first;
  if (cur != 0) {
    for (;;) {
      FUN_801a5160(0x801cee0,cur);
      cur = *(uint32_t *)guest_at(cur + 4);
      if (cur == first) break;
    }
  }
  return;
}

/* 0x801a50d8 — header + 3 field dumps of table node (a0*60+a1*12+0x801ff268 indexed by a2? see asm: s0 = a1*60 + a2*12... actually a0*60? recheck: sll s0,a1,4; subu s0,s0,a1 → a1*15; sll 2 → a1*60. v0=a2*3*4=a2*12. s0 += a2*12 + 0x801ff268. */
void FUN_801a50d8(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t s0 = 0x801ff268 + a1 * 60 + a2 * 12;
  FUN_80190a04((const char *)guest_at(0x801ceec),a0);
  FUN_80190a04((const char *)guest_at(0x80126014),*(uint32_t *)guest_at(s0));
  FUN_80190a04((const char *)guest_at(0x80126024),*(uint32_t *)guest_at(s0 + 4));
  FUN_80190a04((const char *)guest_at(0x80126034),*(uint32_t *)guest_at(s0 + 8));
  return;
}

extern void FUN_801a3034(uint8_t *,uint32_t);
extern void FUN_801a3048(uint8_t *,uint32_t);
extern void FUN_801a3088(uint8_t *,uint32_t);

/* 0x801a24dc — dispatch on a0[0x46]: 3->3048(a0,a0[0xe4]) 2->3034(a0,a1) 4->3088(a0,a0[0x47]). */
void FUN_801a24dc(uint8_t *a0,uint32_t a1)
{
  uint8_t v1 = *(uint8_t *)(a0 + 0x46);
  if (v1 == 3) {
    FUN_801a3048(a0,*(uint8_t *)(a0 + 0xe4));
  } else if (v1 < 4) {
    if (v1 == 2) {
      FUN_801a3034(a0,a1);
    }
  } else if (v1 == 4) {
    FUN_801a3088(a0,*(uint8_t *)(a0 + 0x47));
  }
  return;
}

/* 0x801ad424 — FUN_801a4308 job; init node {a3, a2[1], a2[2], a4}; returns job ptr. */
int *FUN_801ad424(int32_t a0,int32_t a1,uint32_t a2,uint32_t a3,uint32_t a4)
{
  int *r = FUN_801a4308(a0,a1,4,0x801b0db8,0x801ad2dc,0x1c);
  if (r != (int *)0x0) {
    uint32_t *p = *(uint32_t **)((char *)r + 0x10);
    p[0] = a3;
    p[2] = *(uint32_t *)guest_at(a2 + 8);
    p[3] = a4;
    p[1] = *(uint32_t *)guest_at(a2 + 4);
  }
  return r;
}

/* 0x801aed3c — cb(a3[a0[2]]); store at ctx8[*a2] when *a2!=0. */
void FUN_801aed3c(int16_t *a0,uint32_t a1,int16_t *a2,uint32_t a3)
{
  int32_t a2v = *a2;
  uint32_t node = *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[2] * 4);
  uint32_t fp = *(uint32_t *)guest_at(a1);
  void *fn = fn_lookup(fp);
  if (a2v != 0) {
    uint32_t s0 = BR2_GPF(8) + (uint32_t)a2v;
    uint32_t r = 0;
    if (fn != (void *)0x0) {
      r = ((uint32_t (*)(uint32_t))fn)(node);
    }
    *(uint32_t *)guest_at(s0) = r;
  } else if (fn != (void *)0x0) {
    ((void (*)(uint32_t))fn)(node);
  }
  return;
}

/* 0x801aedbc — cb(a3[a0[2]], a3[a0[3]]); store at ctx8[*a2] when *a2!=0. */
void FUN_801aedbc(int16_t *a0,uint32_t a1,int16_t *a2,uint32_t a3)
{
  int32_t a2v = *a2;
  uint32_t n0 = *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[2] * 4);
  uint32_t n1 = *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[3] * 4);
  uint32_t fp = *(uint32_t *)guest_at(a1);
  void *fn = fn_lookup(fp);
  if (a2v != 0) {
    uint32_t s0 = BR2_GPF(8) + (uint32_t)a2v;
    uint32_t r = 0;
    if (fn != (void *)0x0) {
      r = ((uint32_t (*)(uint32_t,uint32_t))fn)(n0,n1);
    }
    *(uint32_t *)guest_at(s0) = r;
  } else if (fn != (void *)0x0) {
    ((void (*)(uint32_t,uint32_t))fn)(n0,n1);
  }
  return;
}

/* ---- identical list-walk+dispatch family (0x801a6a68/863c, 0x801aa47c,
 *      0x801ac0d4, 0x801acd3c, 0x801ad5d0): for each node in *a1 chain:
 *      (*a0)->f40 = node; if node->b17: node->b18 ? FUN_801a482c(a0)
 *      : node->f8(node->f10). Params are host pointers; fn via fnmap. ---- */
static void br2_walk_dispatch(uint8_t **a0,uint8_t *a1)
{
  uint8_t *node_ = a1;
  for (;;) {
    uint8_t *nx_ = *(uint8_t **)node_;
    uint8_t *ctx_ = *(uint8_t **)a0;
    *(uint8_t **)(ctx_ + 0x40) = node_;
    if (*(int8_t *)(node_ + 0x17) != 0) {
      if (*(int8_t *)(node_ + 0x18) != 0) {
        FUN_801a482c((int)(uintptr_t)a0);
      } else {
        uint32_t fp_ = *(uint32_t *)(node_ + 8);
        uint32_t ar_ = *(uint32_t *)(node_ + 0x10);
        void *fn_ = fn_lookup(fp_);
        if (fn_ != (void *)0x0) {
          ((void (*)(uint32_t))fn_)(ar_);
        }
      }
    }
    if (nx_ == (uint8_t *)0x0) break;
    node_ = nx_;
  }
  return;
}

void FUN_801a6a68(uint8_t **a0,uint8_t *a1) { br2_walk_dispatch(a0,a1); return; }
void FUN_801a863c(uint8_t **a0,uint8_t *a1) { br2_walk_dispatch(a0,a1); return; }
void FUN_801aa47c(uint8_t **a0,uint8_t *a1) { br2_walk_dispatch(a0,a1); return; }
void FUN_801ac0d4(uint8_t **a0,uint8_t *a1) { br2_walk_dispatch(a0,a1); return; }
void FUN_801acd3c(uint8_t **a0,uint8_t *a1) { br2_walk_dispatch(a0,a1); return; }
void FUN_801ad5d0(uint8_t **a0,uint8_t *a1) { br2_walk_dispatch(a0,a1); return; }

/* 0x801aa3fc — free slot a1 of container s1 (f24[] and f3c[] arrays). */
void FUN_801aa3fc(uint8_t *s1,int32_t a1)
{
  if (s1 != (uint8_t *)0x0 && a1 >= 0 &&
      a1 < (int32_t)*(uint32_t *)(s1 + 0x44)) {
    uint32_t o = (uint32_t)a1 * 4;
    uint32_t *arr = (uint32_t *)(uintptr_t)*(uint32_t *)(s1 + 0x24);
    FUN_801a48fc((int *)(uintptr_t)arr[a1]);
    arr[a1] = 0;
    {
      uint32_t *arr2 = (uint32_t *)(uintptr_t)*(uint32_t *)(s1 + 0x3c);
      arr2[a1] = 0;
    }
    (void)o;
  }
  return;
}

/* 0x801a86bc/8744/87cc/8854 — guarded two-callee dispatch family. */
#define BR2_GUARD_DISPATCH(FN_A, FN_B)                              \
  do {                                                             \
    if (*(int16_t *)(a0 + 0x34) != 0 &&                            \
        *(uint32_t *)(a0 + 0x1c) == 0) {                           \
      int16_t v1_ = *(int16_t *)(a0 + 0x32);                       \
      uint32_t a1_ = *(uint32_t *)guest_at(0x801bef00);          \
      if (v1_ == 0) {                                              \
        FN_A(*(uint32_t *)guest_at(0x801bef24),a1_,a0);          \
      } else if ((uint32_t)(v1_ - 1) < 0x7ff) {                    \
        FN_B(*(uint32_t *)guest_at(0x801bef24) +                 \
             (uint32_t)v1_ * 4,a1_,a0);                            \
      }                                                            \
    }                                                              \
  } while (0)

void FUN_801a86bc(uint8_t *a0) { BR2_GUARD_DISPATCH(FUN_801a5bd0,FUN_801a6538); return; }
void FUN_801a8744(uint8_t *a0) { BR2_GUARD_DISPATCH(FUN_801a5d90,FUN_801a66dc); return; }
void FUN_801a87cc(uint8_t *a0) { BR2_GUARD_DISPATCH(FUN_801a5f50,FUN_801a6350); return; }
void FUN_801a8854(uint8_t *a0) { BR2_GUARD_DISPATCH(FUN_801a6150,FUN_801a6880); return; }

/* ======== batch36: 0x801a0e/0x801a17/0x801a40/0x801aef ======== */

extern int32_t FUN_8019d5b0(uint32_t);

/* 0x801a0e98 — reset 7 globals via FUN_8019d5b0, then shift two 4-word blocks. */
void FUN_801a0e98(void)
{
  FUN_8019d5b0(*(uint32_t *)guest_at(0x801ef744));
  FUN_8019d5b0(*(uint32_t *)guest_at(0x801ef774));
  FUN_8019d5b0(*(uint32_t *)guest_at(0x801ef778));
  FUN_8019d5b0(*(uint32_t *)guest_at(0x801ef77c));
  FUN_8019d5b0(*(uint32_t *)guest_at(0x801ef780));
  FUN_8019d5b0(*(uint32_t *)guest_at(0x801ef784));
  FUN_8019d5b0(*(uint32_t *)guest_at(0x801ef788));
  *(uint32_t *)guest_at(0x801ef79c) = 0;
  *(uint32_t *)guest_at(0x801ef798) = *(uint32_t *)guest_at(0x801ef79c);
  *(uint32_t *)guest_at(0x801ef794) = *(uint32_t *)guest_at(0x801ef798);
  *(uint32_t *)guest_at(0x801ef790) = *(uint32_t *)guest_at(0x801ef794);
  *(uint32_t *)guest_at(0x801ef7ac) = 0;
  *(uint32_t *)guest_at(0x801ef7a8) = *(uint32_t *)guest_at(0x801ef7ac);
  *(uint32_t *)guest_at(0x801ef7a4) = *(uint32_t *)guest_at(0x801ef7a8);
  *(uint32_t *)guest_at(0x801ef7a0) = *(uint32_t *)guest_at(0x801ef7a4);
  return;
}

/* 0x801a40f8 — per-frame state advance on s0 (host struct). */
void FUN_801a40f8(uint8_t *s0)
{
  uint32_t v0 = *(uint32_t *)(s0 + 0x4c) + 1;
  *(uint32_t *)(s0 + 0x4c) = v0;
  {
    uint8_t v1 = *(uint8_t *)(s0 + 0x46);
    if (v1 != 0) {
      if (v1 == 1) {
        uint8_t t = *(uint8_t *)(s0 + 0x4a);
        if ((uint32_t)t < 0xb) {
          *(uint8_t *)(s0 + 0x4a) = (uint8_t)(t + 1);
        } else {
          *(uint8_t *)(s0 + 0x49) = 2;
          *(uint8_t *)(s0 + 0x46) = 0xff;
        }
      } else {
        uint8_t t = *(uint8_t *)(s0 + 0x4a);
        if ((uint32_t)t < 0xb) {
          *(uint8_t *)(s0 + 0x4a) = (uint8_t)(t + 1);
        } else if (*(uint8_t *)(s0 + 0x49) != 0) {
          uint32_t fp = *(uint32_t *)guest_at(0x801be784);
          void *fn = fn_lookup(fp);
          if (fn != (void *)0x0) {
            ((void (*)(uint32_t))fn)((uint32_t)(uintptr_t)s0);
          }
        }
      }
    }
  }
  if (*(uint8_t *)(*(uint32_t *)(s0 + 0x3c)) == 0xf3) {
    /* fallthrough: nothing */
  } else {
    *(uint8_t *)(*(uint32_t *)(s0 + 0x30)) = 0xff;
    *(uint8_t *)(*(uint32_t *)(s0 + 0x30) + 1) = 0;
    *(uint8_t *)(s0 + 0xe8) = 0;
    *(uint8_t *)(s0 + 0x35) = 0;
  }
  return;
}

/* 0x801aef18 — 4-arg cb dispatch; store result at ctx8[*a2] when *a2!=0. */
void FUN_801aef18(int16_t *a0,uint32_t a1,int16_t *a2,uint32_t a3)
{
  int32_t a2v = *a2;
  uint32_t fp = *(uint32_t *)guest_at(a1);
  void *fn = fn_lookup(fp);
  if (a2v != 0) {
    uint32_t s0 = BR2_GPF(8) + (uint32_t)a2v;
    uint32_t r = 0;
    if (fn != (void *)0x0) {
      r = ((uint32_t (*)(uint32_t,uint32_t,uint32_t,uint32_t))fn)(
        *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[2] * 4),
        *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[3] * 4),
        *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[4] * 4),
        *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[5] * 4));
    }
    *(uint32_t *)guest_at(s0) = r;
  } else if (fn != (void *)0x0) {
    ((void (*)(uint32_t,uint32_t,uint32_t,uint32_t))fn)(
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[2] * 4),
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[3] * 4),
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[4] * 4),
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[5] * 4));
  }
  return;
}

/* ======== batch66: node-type tag setters ======== */

/* 0x801a3034 — tag 0x45 ('E'): a0[0x37]=0x45, a0[0x2c]=0,
 * a0[0x36]=0. a1 unused (guest never reads it). */
void FUN_801a3034(uint8_t *a0,uint32_t a1)
{
  (void)a1;
  a0[0x37] = 0x45;
  *(uint32_t *)(a0 + 0x2c) = 0;
  a0[0x36] = 0;
}

/* 0x801a3048 — tag 0x4c ('L'): a0[0x37]=0x4c, a0[0x2c]=guest(a0+0x24),
 * a0[0x24]=a1, a0[0x36]=1. */
void FUN_801a3048(uint8_t *a0,uint32_t a1)
{
  a0[0x37] = 0x4c;
  *(uint32_t *)(a0 + 0x2c) = guest_of(a0 + 0x24);
  a0[0x24] = (uint8_t)a1;
  a0[0x36] = 1;
}

/* 0x801a3088 — tag 0x47 ('G'): same shape as 3048 with 0x47. */
void FUN_801a3088(uint8_t *a0,uint32_t a1)
{
  a0[0x37] = 0x47;
  *(uint32_t *)(a0 + 0x2c) = guest_of(a0 + 0x24);
  a0[0x24] = (uint8_t)a1;
  a0[0x36] = 1;
}

/* ======== batch_mass17: disasm-verified ports ======== */

/* FUN_8019f35c / FUN_8019f5b4 — memory-card command issue.
 * State block @0x801febe0 (s1): [0]=state [1],[2] [4]=a0(path) [5]=flag
 * (<0 required) [6]=a3 [7]=a4(5th arg) [8]=a2 [0x24..]=cmd blk.
 * Checks -> printf str + ret 0; success -> state=3 (f35c) / 4 (f5b4),
 * queue cb 0x8019f474 / 0x8019f6cc, ret 1.
 * Disasm 0x8019f35c-0x8019f470, 0x8019f5b4-0x8019f6a8. */
static int br2_mc_issue(uint32_t a0,const void *a1,void *a2,
                        uint32_t a3,uint32_t a4,int state,uint32_t cb)
{
  uint32_t *s1 = (uint32_t *)guest_at(0x801febe0);
  uint8_t *blk = (uint8_t *)guest_at(0x801febe0 + 0x24);
  if ((int32_t)s1[0] > 0) {
    FUN_8018e550((const char *)guest_at(0x80125e8c));
    return 0;
  }
  if ((int32_t)s1[5] >= 0) {
    FUN_8018e550((const char *)guest_at(0x80125de4));
    return 0;
  }
  if ((a4 & 0x7f) != 0) {
    FUN_8018e550((const char *)guest_at(0x80125e30));
    return 0;
  }
  if ((a3 & 0x7f) != 0) {
    FUN_8018e550((const char *)guest_at(0x80125e5c));
    return 0;
  }
  FUN_801a01bc(a0,blk);
  FUN_801a0530(blk,(uint32_t)(uintptr_t)a1);
  s1[0] = (uint32_t)state;
  s1[1] = 0;
  s1[2] = 0;
  s1[6] = a3;
  s1[8] = (uint32_t)(uintptr_t)a2;
  s1[7] = a4;
  s1[4] = a0;                        /* delay-slot store: before the call */
  FUN_801a0a10(cb,0);
  return 1;
}

int FUN_8019f35c(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  return br2_mc_issue(a0,a1,a2,a3,a4,3,0x8019f474);
}

int FUN_8019f5b4(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  return br2_mc_issue(a0,a1,a2,a3,a4,4,0x8019f6cc);
}

/* FUN_8019fb9c — memory-card file command (open/read/write style).
 * State @0x801febe0; cmd blk on stack (0x20 bytes @sp+0x10).
 * Returns -1 busy, 6 open-ok, 0 issued, 7 done-fail, else status.
 * Disasm 0x8019fb9c-0x8019fd94. */
int FUN_8019fb9c(uint32_t a0,const void *a1,uint32_t a2)
{
  uint32_t *s0 = (uint32_t *)guest_at(0x801febe0);
  uint8_t blk[0x20];
  uint32_t a2hi;
  int32_t s2 = 0;
  uint32_t st;
  int32_t v0;
  if (s0[0] != 0) {
    FUN_8018e550((const char *)guest_at(0x80125e8c));
    return -1;
  }
  FUN_801a01bc(a0,blk);
  FUN_801a0530(blk,(uint32_t)(uintptr_t)a1);
  s0[3] |= 1u << (s0[4] & 31);
  v0 = FUN_801a0220((uint32_t)(uintptr_t)blk,1,s0[4]);
  a2hi = a2 << 16;                    /* delay slot: always executes */
  if (v0 >= 0) {
    FUN_801a0250(v0);
    return 6;
  }
  for (;;) {
    v0 = FUN_801a0220((uint32_t)(uintptr_t)blk,(a2hi | 0x200),s0[4]);
    if (v0 >= 0) {
      FUN_801a0250(v0);
      return 0;
    }
    *(uint32_t *)guest_at(0x801fec38) = FUN_8019fa6c(0);
    if ((int32_t)s0[0] > 0) {
      FUN_8018e550((const char *)guest_at(0x80125ce8));
    } else {
      s0[0] = 2;
      s0[1] = 0;
      s0[2] = 0;
      *(uint32_t *)guest_at(0x801febf0) = a0;  /* delay-slot store */
      FUN_801a0a10(0x8019eb00,0);
    }
    /* wait for state change */
    if (s0[0] == 0 && s0[2] == 0) {
      while (*(uint32_t *)guest_at(0x801febe8) == 0) { }
    }
    st = *(uint32_t *)guest_at(0x801febd4);
    s0[2] = 0;
    FUN_8019fa6c(*(uint32_t *)guest_at(0x801fec38));
    if (st == 0) return 7;
    if (st == 3) continue;             /* re-issue same cmd */
    if (st == 2) {
      s2++;
      if (s2 < 4) continue;            /* retry issue */
    }
    return (int32_t)st;
  }
}

/* FUN_801aa96c — relocate fixups in a loaded data block (magic
 * 0x000ed1c4 @+0). Adds block guest base to self-relative u32 fields:
 * +0x7c,+0x80..+0xa8 (12 fields); then per-slot record fixups driven by
 * tables at +0x5c..+0x78 (8 entries, stride 4 -> +0x34 records),
 * +0x7c/+0x28 count list, and +0x84/+0x88/+0x8c arrays gated by
 * +0xc mode / +0x10 / +0x14 flags, count +0x24, stride 0x14.
 * Disasm 0x801aa96c-0x801aad00. */
void FUN_801aa96c(uint32_t a0)
{
  uint8_t *b = (uint8_t *)guest_at(a0);
  uint32_t off, i, j, n;
  if (*(uint32_t *)(b + 0) != 0x000ed1c4u) return;
  /* absolute-ise the 12 base pointers (0x7c then 0x80..0xa8) */
  static const uint16_t fbase[12] = {
    0x7c,0x80,0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,0xa0,0xa4,0xa8 };
  for (i = 0; i < 12; i++) {
    off = fbase[i];
    *(uint32_t *)(b + off) += a0;
  }
  /* 8 slot-table entries at +0x5c..+0x78: if nonzero, fix 7 ptr fields
   * of each 0x34-byte record at *(+0x80) + k*0x34 */
  {
    uint32_t recbase = *(uint32_t *)(b + 0x80);
    uint32_t k = 0;
    for (i = 0; i < 8; i++) {
      if (*(uint32_t *)(b + 0x5c + i * 4) != 0) {
        uint8_t *rec = (uint8_t *)guest_at(recbase + k);
        static const uint16_t rofs[7] = {0x18,0x24,0x1c,0x20,0x2c,0x30,0x28};
        for (j = 0; j < 7; j++)
          *(uint32_t *)(rec + rofs[j]) += a0;
        k += 0x34;
      }
    }
  }
  /* count list at *(+0x7c): n2 = *(+0x28) entries of 8 bytes;
   * +4 = ptr array base (fix), +0 = count of u32 ptrs to fix */
  n = *(uint32_t *)(b + 0x28);
  if (n != 0) {
    uint32_t lst = *(uint32_t *)(b + 0x7c);
    for (i = 0; i < n; i++) {
      uint8_t *e = (uint8_t *)guest_at(lst + i * 8);
      int32_t cnt;
      uint32_t arr;
      *(uint32_t *)(e + 4) += a0;
      cnt = (int32_t)*(uint32_t *)(e + 0);
      arr = *(uint32_t *)(e + 4);
      if (cnt > 0) {
        for (j = 0; j < (uint32_t)cnt; j++) {
          uint32_t pa = arr + j * 4;
          *(uint32_t *)guest_at(pa) += a0;
        }
      }
    }
  }
  /* per-record fixups, count *(+0x24), stride 0x14 */
  n = *(uint32_t *)(b + 0x24);
  if (n != 0) {
    static const uint16_t rofs[3] = {0x84,0x88,0x8c};
    uint32_t mode = *(uint32_t *)(b + 0xc);
    int gate84 = (mode - 1) < 2;         /* sltiu (mode-1),2 */
    int gate88 = *(uint32_t *)(b + 0x10) == 1;
    int gate8c = *(uint32_t *)(b + 0x14) == 1;
    for (i = 0; i < n; i++) {
      uint32_t k = i * 0x14;
      int t;
      for (t = 0; t < 3; t++) {
        int on = (t == 0) ? gate84 : (t == 1) ? gate88 : gate8c;
        if (on) {
          uint8_t *rec = (uint8_t *)guest_at(*(uint32_t *)(b + rofs[t]) + k);
          *(uint32_t *)(rec + 8)  += a0;
          *(uint32_t *)(rec + 0xc) += a0;
          *(uint32_t *)(rec + 0x10) += a0;
        }
      }
    }
  }
  return;
}

/* FUN_801a1a2c — clear 0x801be7e8 (dead lw of same cell in guest). */
void FUN_801a1a2c(void)
{
  *(uint32_t *)guest_at(0x801be7e8) = 0;
}

/* FUN_801a7444 — for i < *(a0+0x40): sh 0 @ +0x32 of
 * *(*(arr+0x10)[i] + 0x10). a0 arrives as guest addr or host ptr.
 * Disasm 0x801a7444-0x801a7494. */
void FUN_801a7444(uint32_t a0)
{
  uint8_t *s = (uint8_t *)mem_ptr(a0);
  int32_t n, i;
  if (s == (uint8_t *)0x0) return;
  n = *(int32_t *)(s + 0x40);
  if (n <= 0) return;
  for (i = 0; i < n; i++) {
    uint32_t *arr = (uint32_t *)mem_ptr(*(uint32_t *)(s + 0x10));
    uint8_t *node = (uint8_t *)mem_ptr(arr[i]);
    uint8_t *rec = (uint8_t *)mem_ptr(*(uint32_t *)(node + 0x10));
    *(int16_t *)(rec + 0x32) = 0;
  }
}

/* FUN_801aa0e4 / aa168 / aa1e4 / aa2c8 / aa320 / aa374 — animation-slot
 * record flag/field setters. a0 = host ctx; arrays at +0x34/+0x38 (or
 * +0x3c), count +0x44, idx a1 (signed). Records: flags u16 @+0x44,
 * params @+0x46/+0x48. Disasm 0x801aa0e4-0x801aa3f8. */
static uint8_t *br2_aa_elem(uint8_t *s,uint32_t arros,int32_t idx)
{
  uint8_t *arr = (uint8_t *)mem_ptr(*(uint32_t *)(s + arros));
  uint32_t e = *(uint32_t *)(arr + (uint32_t)idx * 4);
  return (uint8_t *)mem_ptr(e);
}

void FUN_801aa0e4(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = br2_aa_elem(s,0x34,a1);
  if (e != (uint8_t *)0x0) *(uint16_t *)(e + 0x44) |= 0x100;
  e = br2_aa_elem(s,0x38,a1);
  if (e != (uint8_t *)0x0) *(uint16_t *)(e + 0x44) |= 0x100;
}

void FUN_801aa168(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = br2_aa_elem(s,0x34,a1);
  if (e != (uint8_t *)0x0)
    *(uint16_t *)(e + 0x44) = (uint16_t)*(uint8_t *)(e + 0x44);
  e = br2_aa_elem(s,0x38,a1);
  if (e != (uint8_t *)0x0)
    *(uint16_t *)(e + 0x44) = (uint16_t)*(uint8_t *)(e + 0x44);
}

void FUN_801aa1e4(void *a0,int32_t a1,int32_t a2,int32_t a3)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = br2_aa_elem(s,0x34,a1);
  if (e != (uint8_t *)0x0) {
    *(int16_t *)(e + 0x46) = (int16_t)a2;
    e = br2_aa_elem(s,0x34,a1);
    *(int16_t *)(e + 0x48) = (int16_t)a3;
    e = br2_aa_elem(s,0x34,a1);
    *(uint16_t *)(e + 0x44) |= 1;
  }
  e = br2_aa_elem(s,0x38,a1);
  if (e != (uint8_t *)0x0) {
    *(int16_t *)(e + 0x46) = (int16_t)a2;
    e = br2_aa_elem(s,0x38,a1);
    *(int16_t *)(e + 0x48) = (int16_t)a3;
    e = br2_aa_elem(s,0x38,a1);
    *(uint16_t *)(e + 0x44) |= 1;
  }
}

void FUN_801aa2c8(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = br2_aa_elem(s,0x3c,a1);
  if (e != (uint8_t *)0x0) *(uint16_t *)(e + 0x44) |= 0x100;
}

void FUN_801aa320(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = br2_aa_elem(s,0x3c,a1);
  if (e != (uint8_t *)0x0)
    *(uint16_t *)(e + 0x44) = (uint16_t)*(uint8_t *)(e + 0x44);
}

void FUN_801aa374(void *a0,int32_t a1,int32_t a2,int32_t a3)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = br2_aa_elem(s,0x3c,a1);
  if (e != (uint8_t *)0x0) {
    *(int16_t *)(e + 0x46) = (int16_t)a2;
    e = br2_aa_elem(s,0x3c,a1);
    *(int16_t *)(e + 0x48) = (int16_t)a3;
    e = br2_aa_elem(s,0x3c,a1);
    *(uint16_t *)(e + 0x44) |= 1;
  }
}

/* FUN_801b1140 — build 3x3 fixed-point matrix @a3 (9 s16, /4096)
 * from sin/cos table lookups FUN_8018d29c/8018d298 of a0,a1,a2
 * (caller passes raw s16 angles). Returns a3.
 * Disasm 0x801b1140-0x801b1280. */
uint32_t FUN_801b1140(int32_t a0,int32_t a1,int32_t a2,uint32_t a3)
{
  /* t2=d29c(a0) t1=d298(a0) t9=d29c(a1) t3=d298(a1)
   * t5=d29c(a2) v0=d298(a2); mult+mflo+sra 12 (signed, >>12 arith) */
  int32_t t2 = FUN_8018d29c(a0);
  int32_t t1 = FUN_8018d298(a0);
  int32_t t9 = FUN_8018d29c(a1);
  int32_t t3 = FUN_8018d298(a1);
  int32_t t5 = FUN_8018d29c(a2);
  int32_t v0 = FUN_8018d298(a2);
  int32_t t4 = -t9;                    /* -cos a1 */
  int32_t t9n = -t3;                   /* -sin a1 (neg at 0x801b1214) */
  int32_t t6a = (t2 * t5) >> 12;       /* cos0*cos2 */
  int32_t t6b = (t2 * v0) >> 12;       /* cos0*sin2 */
  int16_t *m = (int16_t *)mem_ptr(a3);
  m[2] = (int16_t)(-((t9 * t1) >> 12));
  m[8] = (int16_t)((t9 * v0) >> 12);
  m[6] = (int16_t)(((t9 * v0) >> 12) + ((t3 * t6a) >> 12));
  m[5] = (int16_t)(-t2);
  m[0] = (int16_t)(((t3 * v0) >> 12) + ((t4 * t6a) >> 12));
  m[7] = (int16_t)(((t4 * t5) >> 12) + ((t3 * t6b) >> 12));
  m[1] = (int16_t)(((t9n * t5) >> 12) + ((t4 * t6b) >> 12));
  m[3] = (int16_t)((t1 * t5) >> 12);
  m[4] = (int16_t)((t1 * v0) >> 12);
  return a3;
}

/* FUN_801a7038 — allocate/init an animation node pair via
 * FUN_801a9644 for slot s0=a1 (signed index into ctx arrays).
 * ctx a0 (guest addr): node @+8 (fC mode @+0xc must be 1|2 for 2nd pass,
 * f10 flag @+0x10), arrays @+0x1c/+0x20/+0x34/+0x38 (u32 slots, idx*4),
 * count @+0x44, aux ptrs @+0x14/+0x28, mode2 @+0x2c.
 * Returns 1 ok / 0 fail. Disasm 0x801a7038-0x801a7298. */
int FUN_801a7038(uint32_t a0,int32_t a1)
{
  uint8_t *s1 = (uint8_t *)guest_at(a0);
  int32_t s0 = a1;
  uint32_t g, v0, v1, idx;
  int32_t s2;
  if (s1 == (uint8_t *)0x0) return 0;
  g = *(uint32_t *)(s1 + 8);
  v1 = *(uint32_t *)guest_at(g + 0xc);
  if (v1 == 0) return 0;
  v0 = *(uint32_t *)(s1 + 0x44);
  if (v0 == 0 || s0 < 0 || s0 >= (int32_t)v0) return 0;
  if ((v1 - 1) < 2) {
    uint32_t arr = *(uint32_t *)(s1 + 0x1c);
    if (*(uint32_t *)guest_at(arr + (uint32_t)s0 * 4) != 0) return 0;
  }
  if (*(uint32_t *)guest_at(g + 0x10) == 1) {
    uint32_t arr = *(uint32_t *)(s1 + 0x20);
    if (*(uint32_t *)guest_at(arr + (uint32_t)s0 * 4) != 0) return 0;
  }
  g = *(uint32_t *)(s1 + 8);
  s2 = (int32_t)*(uint32_t *)guest_at(
      *(uint32_t *)guest_at(g + 0x7c) + (uint32_t)s0 * 8 + 4);
  v1 = *(uint32_t *)guest_at(g + 0xc);
  if ((v1 - 1) >= 2) return 0;
  {
    uint32_t sp10 = *(uint32_t *)guest_at(g + 0x24);
    uint32_t sp14 = *(uint32_t *)guest_at(*(uint32_t *)(s1 + 8) + 0x2c);
    int32_t  sp1c = (int8_t)*(uint8_t *)guest_at(*(uint32_t *)(s1 + 0x14) + 0x14);
    uint32_t r = (uint32_t)FUN_801a9644(0,
        (uint16_t)*(uint32_t *)guest_at(g + 0xc),
        *(uint32_t *)(s1 + 0x28),
        *(uint32_t *)guest_at(g + 0x84),
        (int16_t)sp10,(uint16_t)sp14,s2,sp1c);
    uint32_t arr, nd;
    idx = (uint32_t)s0 * 4;
    arr = *(uint32_t *)(s1 + 0x1c);
    *(uint32_t *)guest_at(arr + idx) = r;
    nd = *(uint32_t *)guest_at(arr + idx);
    if (nd == 0) return 0;
    arr = *(uint32_t *)(s1 + 0x34);
    *(uint32_t *)guest_at(arr + idx) = *(uint32_t *)guest_at(nd + 0x10);
  }
  /* second pass: only when node mode fC in {1,2} and f10==1 */
  g = *(uint32_t *)(s1 + 8);
  v0 = *(uint32_t *)guest_at(g + 0xc);
  if ((v0 - 1) < 2) {
    if (*(uint32_t *)guest_at(g + 0x10) != 1) return 0;
    {
      uint32_t sp10 = *(uint32_t *)guest_at(g + 0x24);
      uint32_t sp14 = *(uint32_t *)guest_at(*(uint32_t *)(s1 + 8) + 0x2c);
      int32_t  sp1c = (int8_t)*(uint8_t *)guest_at(*(uint32_t *)(s1 + 0x14) + 0x14);
      uint32_t r = (uint32_t)FUN_801a9644(0,
          (uint16_t)*(uint32_t *)guest_at(g + 0x10),
          *(uint32_t *)(s1 + 0x2c),
          *(uint32_t *)guest_at(g + 0x88),
          (int16_t)sp10,(uint16_t)sp14,s2,sp1c);
      uint32_t arr, nd;
      arr = *(uint32_t *)(s1 + 0x20);
      *(uint32_t *)guest_at(arr + idx) = r;
      nd = *(uint32_t *)guest_at(arr + idx);
      if (nd != 0) {
        arr = *(uint32_t *)(s1 + 0x38);
        *(uint32_t *)guest_at(arr + idx) = *(uint32_t *)guest_at(nd + 0x10);
        return 1;
      }
      /* alloc failed: free arr1c[idx] node, clear slot, ret 0 */
      arr = *(uint32_t *)(s1 + 0x1c);
      nd = *(uint32_t *)guest_at(arr + idx);
      FUN_801a48fc((int *)guest_at(nd));
      *(uint32_t *)guest_at(arr + idx) = 0;
      return 0;
    }
  }
  return 1;
}

/* FUN_801a984c — walk two anim slots (@ctx+0x34/+0x38, first record
 * only) accumulating fixed-point deltas; when cumulative quotient
 * reaches clamped arg s2, emit FUN_801a9c24(node, idx, pos).
 * Signed lh/div semantics preserved (mflo trunc, sra, +0xfff fixup).
 * Disasm 0x801a984c-0x801a99dc. */
int FUN_801a984c(uint32_t a0,int32_t a1)
{
  uint8_t *s3 = (uint8_t *)guest_at(a0);
  int32_t s2 = a1;
  int32_t s1;
  if (s3 == (uint8_t *)0x0) return 0;
  if (s2 < 0) s2 = 0;
  if (s2 >= 0x1001) s2 = 0x1000;
  for (s1 = 0; s1 < 2; s1++) {
    uint32_t arr = *(uint32_t *)(s3 + (s1 == 0 ? 0x34 : 0x38));
    uint32_t s0g = *(uint32_t *)guest_at(arr);
    uint8_t *s0;
    int32_t cnt, a1c = 0, a2c = 0;
    uint32_t a0o = 0, a3u = 0;
    int32_t dv;
    if (s0g == 0) continue;
    s0 = (uint8_t *)guest_at(s0g);
    cnt = *(int16_t *)(s0 + 6);
    if (cnt < 0) continue;
    for (;;) {
      uint32_t ev = *(uint32_t *)guest_at(
          *(uint32_t *)(s0 + 0x18) + a0o + 4);
      int32_t quo, q2;
      dv = *(int16_t *)(s0 + 0xc);
      a3u += ev;
      if (dv == 0) break;              /* guest break 7 trap */
      quo = (int32_t)((a3u << 12)) / dv;
      if (!(quo < s2)) {
        /* t0 is provably 0 here; kept literal for fidelity */
        int32_t t0 = 0;
        q2 = ((s2 - t0) << 12) / (1 - t0);
        if (q2 < 0) q2 += 0xfff;
        FUN_801a9c24((short *)s0,a1c,q2 >> 12);
        break;
      }
      /* advance: a1++, a2++, a0 += 0x14 */
      a1c++;
      a2c++;
      a0o += 0x14;
      cnt = *(int16_t *)(s0 + 6);
      if (cnt < a2c) break;
    }
  }
  return 0;
}

/* FUN_801aad24 — lazily attach node a1 to ctx a0: alloc via
 * FUN_801a4414(0x30-byte rec, cb 0x801aadc4, dtor 0x801aad18), link
 * into *(*(a0+0x18)+0x10) list, cache at a0+0x48. Returns node or 0.
 * Disasm 0x801aad24-0x801aadc0. */
int FUN_801aad24(uint32_t a0,uint32_t a1)
{
  uint8_t *s0;
  uint32_t v0, r, A, v1g, nd;
  if (a0 == 0) return 0;
  s0 = (uint8_t *)guest_at(a0);
  v0 = *(uint32_t *)(s0 + 0x48);
  if (v0 != 0) return (int)v0;
  if (a1 == 0) return 0;
  r = (uint32_t)(uintptr_t)FUN_801a4414((int)*(uint32_t *)(s0 + 0x18),
                                      0x801aadc4,0x801aad18,0x30);
  if (r == 0) return 0;
  A = *(uint32_t *)(s0 + 0x18);
  v1g = *(uint32_t *)guest_at(A + 0x10);
  *(uint32_t *)guest_at(v1g + 0x24) = *(uint32_t *)guest_at(a1);
  nd = *(uint32_t *)guest_at(r + 0x10);
  *(uint32_t *)guest_at(nd)     = a0;
  *(uint32_t *)guest_at(nd + 4) = a1;
  *(uint32_t *)guest_at(a0 + 0x48) = r;
  *(uint32_t *)guest_at(nd + 0xc) = v1g + 0x24;
  return (int)r;
}

/* FUN_801ad2e8 — alloc/init node via FUN_801a4308 from table
 * 0x801ff620[a2][a3]; node fields from source s0 and stack arg a4.
 * Returns allocated guest addr or 0. Disasm 0x801ad2e8-0x801ad388. */
int FUN_801ad2e8(int32_t a0,int32_t a1,int32_t a2,int32_t a3,uint32_t a4)
{
  uint32_t row = *(uint32_t *)guest_at(0x801ff620 + (uint32_t)a2 * 4);
  uint32_t s0  = *(uint32_t *)guest_at(row + (uint32_t)a3 * 4);
  uint32_t r = (uint32_t)(uintptr_t)FUN_801a4308(a0,a1,4,
      0x801b0db8,0x801ad298,
      *(uint32_t *)guest_at(s0 + 4) + 0x1c);
  if (r != 0) {
    uint32_t v1 = *(uint32_t *)guest_at(r + 0x10);
    *(uint32_t *)guest_at(v1)      = a4;
    *(uint32_t *)guest_at(v1 + 8)  = v1 + 0x1c;
    *(uint32_t *)guest_at(v1 + 4)  = s0;
    *(uint32_t *)guest_at(v1 + 0xc) = s0 + *(uint32_t *)guest_at(s0);
    *(uint32_t *)guest_at(v1 + 0x1c) = v1 + 0x1c;
  }
  return (int)r;
}

/* FUN_801a1f74 — device byte send/recv. a0 = host ctx (fields +0x3c
 * buf ptr, +0x40 alt ptr, +0x44/+0x45 counters); a1 = byte (-2 =
 * query/flush path). Globals: dev struct @0x801be3e4 (byte buf,
 * +4 status u16, +0xe u16 cmd, +0xa u16), ctrl @0x801be3e0 (u32
 * status bit7/bit2). Disasm 0x801a1f74-0x801a2180. */
int32_t FUN_801a1f74(uint8_t *a0,int32_t a1)
{
  uint8_t *s0 = a0;
  int32_t s1, s2;
  uint8_t *g1, *g2p;
  uint32_t gv, gv2;
  if (a1 < 0) {
    /* flush/query path */
    gv = *(uint32_t *)guest_at(0x801be3e4);
    g1 = (uint8_t *)mem_ptr(gv);
    s2 = g1[0] & 0xff;
    s0[0x44] = 0xff;
    s0[0x45] = 1;
    *(uint8_t *)(uintptr_t)*(uint32_t *)(s0 + 0x40) = (uint8_t)~a1;
    for (;;) {
      gv = *(uint32_t *)guest_at(0x801be3e4);
      g1 = (uint8_t *)mem_ptr(gv);
      if ((*(uint16_t *)(g1 + 4) & 1) != 0) break;
    }
    while (FUN_801a4230() == 0) { }
    gv = *(uint32_t *)guest_at(0x801be3e4);
    g1 = (uint8_t *)mem_ptr(gv);
    g1[0] = (uint8_t)~a1;
    return s2;
  }
  /* normal send path */
  {
    uint8_t *p3c = (uint8_t *)(uintptr_t)*(uint32_t *)(s0 + 0x3c);
    int32_t nib = (int32_t)(uint8_t)p3c[0] >> 4;   /* lbu then sra4 */
    s1 = 0x88;
    if (nib == 8 && s0[0x44] >= 9) s1 = 0x22;
  }
  gv = *(uint32_t *)guest_at(0x801be3e4);
  g1 = (uint8_t *)mem_ptr(gv);
  {
    uint16_t hw = *(uint16_t *)guest_at(0x1f801120);
    *(uint32_t *)guest_at(0x801ff264) = 0x1ae;
    *(uint32_t *)guest_at(0x801ff260) = hw;
  }
  gv = *(uint32_t *)guest_at(0x801be3e4);
  g1 = (uint8_t *)mem_ptr(gv);
  /* wait status bit1 */
  while ((*(uint16_t *)(g1 + 4) & 2) == 0) { }
  gv = *(uint32_t *)guest_at(0x801be3e4);
  g1 = (uint8_t *)mem_ptr(gv);
  s2 = g1[0] & 0xff;
  *(uint16_t *)(g1 + 0xe) = (uint16_t)s1;
  gv2 = *(uint32_t *)guest_at(0x801be3e0);
  for (;;) {
    g2p = (uint8_t *)mem_ptr(gv2);
    if ((*(uint32_t *)g2p & 0x80) != 0) break;
    if (FUN_801a4230() != 0) return -0x14;
    gv2 = *(uint32_t *)guest_at(0x801be3e0);
  }
  gv = *(uint32_t *)guest_at(0x801be3e4);
  g1 = (uint8_t *)mem_ptr(gv);
  g1[0] = (uint8_t)a1;
  if (s1 == 0x22) {
    gv2 = *(uint32_t *)guest_at(0x801be3e0);
    g2p = (uint8_t *)mem_ptr(gv2);
    *(uint32_t *)g2p = 0xffffff7fu;
    gv = *(uint32_t *)guest_at(0x801be3e4);
    g1 = (uint8_t *)mem_ptr(gv);
    *(uint16_t *)(g1 + 0xa) |= 0x10;
  }
  {
    uint8_t *buf = (uint8_t *)(uintptr_t)*(uint32_t *)(s0 + 0x3c);
    uint8_t idx = s0[0x44];
    uint8_t v = s0[0x45];
    s0[0x45] = (uint8_t)(v + 1);
    buf[(uint32_t)idx] = (uint8_t)s2;
    s0[0x44] = (uint8_t)(s0[0x44] + 1);
  }
  return s2;
}

/* FUN_801a4230 — FIFO space check. a0 = u16 count @0x1f801120 (+ u16
 * @0x1f801128, or +0x10000 when that is 0) when count < limit
 * @0x801ff260; then (a0 - base) compared against limit @0x801ff264,
 * raw when flag u16 @0x1f801124 & 0x200 else >>3 logical; returns
 * !(v0 < lim) i.e. 1 when space exhausted. Disasm 0x801a4230-0x801a42cc. */
int32_t FUN_801a4230(void)
{
  uint32_t a0 = (uint32_t)*(uint16_t *)guest_at(0x1f801120);
  uint32_t base = *(uint32_t *)guest_at(0x801ff260);
  uint32_t lim2 = *(uint32_t *)guest_at(0x801ff264);
  uint32_t v0;
  if (a0 < base) {
    uint32_t c2 = (uint32_t)*(uint16_t *)guest_at(0x1f801128);
    a0 += (c2 != 0) ? c2 : 0x10000u;
  }
  v0 = a0 - base;
  if ((*(uint16_t *)guest_at(0x1f801124) & 0x200) == 0)
    v0 >>= 3;
  return (v0 < lim2) ? 0 : 1;
}
