/* save.c — memory-card / settings / display-helper cluster.
 * Verbatim Ghidra ports from decomp/batch_shell.txt.
 * Guest globals bound via g_DAT_* (bind.c arena, payload-backed);
 * s_* strings resolve through guest_at into the loaded EXE image. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_state.h"

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
extern uint32_t br2_guest_frame(uint32_t);
extern void br2_guest_frame_free(void);
extern uint32_t bios_svc(uint32_t table,uint32_t svc,uint32_t a0,uint32_t a1);
extern uint32_t guest_sp(void);
extern uint16_t psx_read_half(uint32_t addr);
extern void decomp_trace_record(uint8_t tag, uint32_t dst, uint32_t src,
                                uint8_t mode, uint32_t len);
void gauge_scale_fixed(short a0,short a1);
void brightness_commit(uint16_t a0);


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
#define DAT_801bef64   (*(uint8_t *)g_DAT_801bef64) /* guest lbu @0x801bef64 (0x80157a00) */
#define DAT_801bef67   (*(uint8_t *)g_DAT_801bef67)
#define DAT_801bef68   (*(uint8_t *)g_DAT_801bef68)
#define DAT_801bef69   (*(uint8_t *)g_DAT_801bef69)
#define DAT_801bef6c   (*(uint8_t *)g_DAT_801bef6c)
#define DAT_801bef6d   (*(uint8_t *)g_DAT_801bef6d)
#define DAT_801bef6e   (*(uint8_t *)g_DAT_801bef6e)
#define DAT_801bef6f   (*(uint8_t *)g_DAT_801bef6f)
#define DAT_801bef72   (*(uint16_t *)g_DAT_801bef72)
#define DAT_801bef74   (*(uint16_t *)g_DAT_801bef74) /* guest lhu/sh @0x801bef74 (0x80157a00) */
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
/* Identified globals (roles inferred from use; names keep the guest addr):
 *   0x801bef60  memcard dialog/state block base (s0/s1 in datetime_confirm_*)
 *   0x801bef64  CD command result/status word (cd_sync_wait_a out-param)
 *   0x801bef67..69  BCD timestamp bytes fed to cd_stamp_to_sector
 *   0x801bef6c  populated slot count (cd_dir_read_wrap result)
 *   0x801bef6d  memcard tick state (switch cases 0x10..0x23)
 *   0x801bef6e  dialog mode byte; low nibble = confirm path (1/2)
 *   0x801bef6f  memcard_tick gate flag (!=0 -> tick returns early)
 *   0x801bef72  expected slot-count threshold for dir scan
 *   0x801bef74  slot scan counter
 *   0x801bef76  brightness fade countdown (u16, ticked in memcard_tick)
 *   0x801bef78/80/84/88  save-sector numbers (cd_stamp_to_sector results)
 *   0x801bef8c  directory-read buffer pointer
 *   0x801c11e0  save record block (0x264-byte stride area, 11 records)
 *   0x801c1264/1304/13a4  16-byte record tables (save_rec_table_copy)
 *   0x801c4580  CD command-in-flight flag
 *   0x801e8400  256-byte permutation table (perm_table_load/apply)
 *   0x801f24d0  24x16 slot poll records (memcard_slot_poll)
 *   0x801f35f4  live deferred-command count
 *   0x801f3600  deferred command queue, 32 entries x 6 bytes
 *   0x801f5968..73  debug marker mode params (dbg_marker_mode_set) */
#define _DAT_1f800008   (*(uint32_t *)g__DAT_1f800008)
#define _DAT_1f80000c   (*(uint32_t *)g__DAT_1f80000c)
#define _DAT_1f80007c   (*(uint16_t *)g__DAT_1f80007c) /* guest lhu @0x1f80007c (0x8015f6a0) */
#define _DAT_1f800082   (*(uint16_t *)g__DAT_1f800082) /* guest sh @0x1f800082 (0x8012b49c) */
#define _DAT_1f800084   (*(uint32_t *)g__DAT_1f800084)
#define _DAT_1f80008c   (*(int16_t *)g__DAT_1f80008c) /* guest sh/lh @0x1f80008c */
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
#define _DAT_801f36c0   (*(uint16_t *)g__DAT_801f36c0) /* guest sh @0x801f36c0 (0x801561c8) */
#define PTR_s___BloodyRoar2__byRAIZINGver1_60_801b43f0 ((uint8_t *)guest_at(*(uint32_t *)g_DAT_801b43f0))
#define SLOTSTR(i) ((char *)guest_at(g_PTR_u32_801b4160[i]))

extern uint8_t  *g_DAT_801f2650;
extern uint8_t  *g_DAT_801f35f4;
extern uint8_t *_g_system_config_ptr;
#define DAT_801f2650   (*(uint8_t  *)g_DAT_801f2650)
#define _DAT_801f35f4  (*(uint32_t *)g_DAT_801f35f4)

/* ---- callee externs ---- */
extern void brightness_packet_send();
extern void mc_state_init();
extern int cd_cmd_issue_async();
extern int cd_sync_wait_a();
extern int cd_result_wait_b_fwd();
extern void cd_file_open(int32_t,uint8_t *);
extern int cd_stamp_to_sector();
extern int cd_dir_read_wrap();
extern void slot_status_fill();
extern void cd_reg_window_update();
extern void cd_reg_window_u16();
extern void cd_chan_attr_apply();
extern int mc_action_dispatch_b();
extern int mc_action_dispatch();
extern int ret_one();
extern int deferred_cmd_exec();
extern void save_rec_table_copy();
extern int save_blk_cmp32(void);
extern int mc_data_write();
extern int mc_op_dispatch(uint32_t);
extern int mc_read_queue();
extern int mc_op_dispatch_b(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern int mc_data_read();
extern int sprintf_core(char *,char *,...);
extern void announce_param_block_init(uint32_t,uint32_t,const void *,uint32_t,uint32_t,int32_t);
extern void scroll_offset_accumulate();
extern void print_mode_dispatch_b();
extern int intro_movie_state();
extern void deferred_cmd_post();
extern int dialog_sel_dispatch(intptr_t,uint32_t);
extern void menu_stepper_state_advance();
extern void hud_gauge_render();
extern void empty_ret();
extern int frame_divisor_get();
extern void scroll_offset_set();
extern void battle_session_arm(intptr_t,int);
extern void frame_tick(intptr_t);
extern void cd_str_print();
extern void bios_table_tramp();
extern void bios_kobj_tramp();

/* ======== 0x80157a00 ======== */

void memcard_tick(void)

{
  ushort tick_dec;
  uint dir_n;
  int cd_rc;
  /* Guest builds a 3-byte BCD timestamp at sp+0x10..0x12 (sb @0x10,
   * 0x11, 0x12) and passes sp+0x10 to cd_stamp_to_sector, which reads
   * s[0..2]. Ghidra split it into 3 scalars — keep it one array. */
  uint8_t timestamp_bcd[3];
  
  if (DAT_801bef6f != '\0') {
    return;
  }
  tick_dec = DAT_801bef76 - 1;
  if (DAT_801bef76 != 0) {
    DAT_801bef76 = tick_dec;
    if ((int)((uint)tick_dec << 0x10) < 0) {
      DAT_801bef76 = 0;
    }
    brightness_packet_send((int)(short)DAT_801bef76);
    if (DAT_801bef76 == 0) {
      mc_state_init();
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
      brightness_packet_send(0);
      DAT_801bef60 = 0xca;
      cd_cmd_issue_async(0xe,(int)guest_of(&DAT_801bef60));
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
      cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
      if (cd_rc == 2) {
        /* disasm-verified: sprintf/cd buf = $sp+0x10, frame -0x28
         * (0x80157b44/0x80157f3c) -> entry_sp - 0x18. The buf is handed
         * to cd_cmd_issue_async as a guest param address that generated
         * CD frames read after pushing below $sp, so reserve the frame
         * window (sp+0x10 -> frm+0x18) or callee frames overwrite it. */
        extern uint32_t br2_guest_frame(uint32_t);
        extern void br2_guest_frame_free(void);
        uint32_t mct_frm = br2_guest_frame(0x28);
        uint32_t mct_buf = mct_frm + 0x18;
        cd_file_open(DAT_801bef80,(uint8_t *)guest_at(mct_buf));
        cd_cmd_issue_async(2,(int)mct_buf);
        br2_guest_frame_free();
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      if (cd_rc != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    case '\x15':
      cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
      if (cd_rc == 2) {
        brightness_packet_send(0xffffffff);
        cd_cmd_issue_async(0x1b,0);
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      if (cd_rc != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    case '\x16':
      cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
      if (cd_rc == 2) {
        cd_cmd_issue_async(1,0);
        DAT_801bef6d = DAT_801bef6d + '\x01';
        return;
      }
      if (cd_rc != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    case '\x17':
      cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
      if (cd_rc != 2) {
        if (cd_rc != 5) {
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
      cd_cmd_issue_async(0x10,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
      return;
    case '\x19':
      cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
      if (cd_rc != 2) {
        if (cd_rc != 5) {
          return;
        }
        DAT_801bef6d = 0x20;
        return;
      }
      DAT_801bef80 = cd_stamp_to_sector(&DAT_801bef64);
      if ((DAT_801bef80 <= DAT_801bef88) && (DAT_801bef84 + -300 <= DAT_801bef80)) {
        DAT_801bef6d = DAT_801bef6d + -1;
        return;
      }
      goto joined_r0x80157c80;
    case ' ':
      brightness_packet_send(0);
      if ((DAT_801bef64 & 0x10) == 0) {
        DAT_801bef6d = 0x23;
        DAT_801bef74 = DAT_801bef74 + 1;
        return;
      }
      DAT_801bef6d = DAT_801bef6d + '\x01';
      DAT_801bef74 = DAT_801bef74 + 1;
      return;
    case '!':
      cd_cmd_issue_async(1,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
      return;
    case '\"':
      cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
      if (cd_rc != 2) {
        if (cd_rc != 5) {
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
      cd_cmd_issue_async(0x13,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
      return;
    case '$':
      cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
      if (cd_rc != 2) {
        if (cd_rc != 5) {
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
    brightness_packet_send(0);
    DAT_801bef60 = 7;
    cd_cmd_issue_async(0xe,(int)guest_of(&DAT_801bef60));
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
    cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
    if (cd_rc == 2) {
      /* disasm-verified: sprintf/cd buf = $sp+0x10, frame -0x28
       * (0x80157b44/0x80157f3c) -> entry_sp - 0x18. Same guest-frame
       * reservation as the mode-2 branch above: generated CD frames
       * would otherwise overwrite the param buf before it is read. */
      extern uint32_t br2_guest_frame(uint32_t);
      extern void br2_guest_frame_free(void);
      uint32_t mct_frm = br2_guest_frame(0x28);
      uint32_t mct_buf = mct_frm + 0x18;
      cd_file_open(DAT_801bef80,(uint8_t *)guest_at(mct_buf));
      cd_cmd_issue_async(2,(int)mct_buf);
      br2_guest_frame_free();
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    else if (cd_rc == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case '\x15':
    cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
    if (cd_rc == 2) {
      brightness_packet_send(0xffffffff);
      cd_cmd_issue_async(3,0);
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    else if (cd_rc == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case '\x16':
    cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
    if (cd_rc == 2) {
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    else if (cd_rc == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case '\x17':
    cd_cmd_issue_async(1,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    cd_rc = cd_result_wait_b_fwd(1,guest_of(&DAT_801bef64));
    if (cd_rc == 1) {
      if ((DAT_801bef68 & 0x80) != 0) {
        return;
      }
      timestamp_bcd[0] = DAT_801bef67;
      timestamp_bcd[1] = DAT_801bef68;
      timestamp_bcd[2] = DAT_801bef69;
      DAT_801bef80 = cd_stamp_to_sector(timestamp_bcd);
      return;
    }
    if (cd_rc != 4) {
      if (cd_rc != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    }
joined_r0x80157c80:
    if ((DAT_801bef6e & 0x10) == 0) {
      brightness_packet_send(0);
      cd_cmd_issue_async(9,0);
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
    cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
    if (cd_rc == 2) {
      DAT_801bef6d = DAT_801bef6d + -1;
    }
    else if (cd_rc == 5) {
      DAT_801bef6d = ' ';
    }
    break;
  case ' ':
    brightness_packet_send(0);
    DAT_801bef74 = DAT_801bef74 + 1;
    if ((DAT_801bef64 & 0x10) == 0) {
      DAT_801bef6d = '#';
    }
    else {
      DAT_801bef6d = DAT_801bef6d + '\x01';
    }
    break;
  case '!':
    cd_cmd_issue_async(1,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    break;
  case '\"':
    cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
    if (cd_rc == 2) {
      if ((DAT_801bef64 & 0x10) == 0) {
        DAT_801bef6d = DAT_801bef6d + '\x01';
      }
      else {
        DAT_801bef6d = DAT_801bef6d + -1;
      }
    }
    else if (cd_rc == 5) {
      DAT_801bef6d = DAT_801bef6d + -1;
    }
    break;
  case '#':
    cd_cmd_issue_async(0x13,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    break;
  case '$':
    cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
    if (cd_rc != 2) {
      if (cd_rc != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    }
    goto switchD_80157ebc_caseD_30;
  case '0':
    cd_cmd_issue_async(1,0);
    DAT_801bef6d = DAT_801bef6d + '\x01';
    break;
  case '1':
    cd_rc = cd_sync_wait_a(1,(uint8_t *)guest_of(&DAT_801bef64));
    if (cd_rc != 2) {
      if (cd_rc != 5) {
        return;
      }
      DAT_801bef6d = 0x20;
      return;
    }
    dir_n = cd_dir_read_wrap(&DAT_801bef8c);
    if ((int)dir_n < 1) {
      DAT_801bef6d = DAT_801bef6d + -1;
      return;
    }
    DAT_801bef6c = (byte)dir_n;
    if ((int)(dir_n & 0xff) < (int)(short)DAT_801bef72) {
      /* guest div @0x80157E2C + mfhi: signed div HI = nm % dv;
       * dv==0 -> HI = dividend (recompiler no-trap semantics). */
      int32_t _dv = (int32_t)(dir_n & 0xff);
      int32_t _nm = (int32_t)(short)DAT_801bef72;
      DAT_801bef72 = (ushort)(_dv == 0 ? _nm : _nm % _dv);
    }
    if (DAT_801bef72 == 0) {
      DAT_801bef72 = (ushort)DAT_801bef6c;
    }
    DAT_801bef84 = cd_stamp_to_sector(&DAT_801bef8c + (short)DAT_801bef72 * 4);
switchD_80157ebc_caseD_30:
    DAT_801bef6d = '\x10';
  }
  return;
}

/* ======== 0x8015f6a0 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int memcard_file_flow(int a0)

{
  int rc;
  uint8_t *str_arg;
  char *fmt_str;
  int dlg_type;
  int op_rc;
  int slot_no;
  uint ow_flag;
  uint sm_state;
  /* Guest stepper descriptor: one contiguous 0x24-byte block at
   * sp+0x38..0x5b passed to menu_stepper_state_advance (a0 = sp+0x38);
   * the +0xc callback is a u32 GUEST code address (fn_lookup), stored
   * as uint32_t — a host code* would write 8 bytes and clobber +0x10.
   * Ghidra split the block into scalars; local_XX names keep their sp
   * offsets via macros (local_c0[i] == old local_X).
   * The block is backed by the mff_frm guest frame (sp+0x38 -> frm+0x40)
   * so guest_of(&local_c0) resolves for the stepper's call_guest path. */
  uint8_t *step_desc;
#define local_c0 (*(uint16_t *)&step_desc[0x00])
#define local_be (*(uint8_t  *)&step_desc[0x02])
#define local_bd (*(char     *)&step_desc[0x03])
#define local_bc (*(uint8_t  *)&step_desc[0x04])
#define local_bb (*(uint8_t  *)&step_desc[0x05])
#define local_ba (*(uint16_t *)&step_desc[0x06])
#define local_b8 (*(uint16_t *)&step_desc[0x08])
#define local_b4 (*(uint32_t *)&step_desc[0x0c])
#define local_b0 (*(uint16_t *)&step_desc[0x10])
#define local_ae (*(uint16_t *)&step_desc[0x12])
#define local_ac (*(uint16_t *)&step_desc[0x14])
#define local_aa (*(int16_t  *)&step_desc[0x16])
#define local_a8 (*(int16_t  *)&step_desc[0x18])
#define local_a6 (*(uint16_t *)&step_desc[0x1a])
#define local_a4 (*(uint16_t *)&step_desc[0x1c])
#define local_a2 (*(uint16_t *)&step_desc[0x1e])
#define local_a0 (*(uint16_t *)&step_desc[0x20])
#define local_9e (*(uint16_t *)&step_desc[0x22])
  char *msg_buf;   /* guest-backed staging buffer: $sp+0x80, frame -0xf8 */
  uint32_t op_sel;
  int sm_ret;
  int wait_cnt;
  char *msg_arg;
  /* guest sp-0xf8..sp frame image (frame 0xf8). msg_buf is written by
   * sprintf_core and re-read by announce_param_block_init* across the
   * whole state loop; without the reservation every nested guest call
   * lands its frame on top of the buffer. frame 0xf8 is 8 mod 16 so the
   * reservation aligns to 0x100 and guest sp+N maps to frm + N + 8;
   * sp+0x80 -> frm+0x88. */
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  uint32_t mff_frm = br2_guest_frame(0xf8);

  extern int br2_bail_pending(void);
  sm_state = 0;
  ow_flag = 0;
  dlg_type = 0;
  slot_no = 0;
  /* sprintf_core and announce_param_block_init both take host string pointers;
   * back both with the guest stack buffer ($sp+0x80, frame -0xf8 ->
   * entry_sp - 0x78, disasm-verified @0x8015f96c/0x8015fc68+) so one buffer
   * serves both. */
  msg_buf = (char *)guest_at(mff_frm + 0x88);
  /* stepper descriptor at sp+0x38 -> frm+0x40 (0x24 bytes, ends 0x64,
   * clear of the msg_buf string buffer at frm+0x88). */
  step_desc = (uint8_t *)guest_at(mff_frm + 0x40);
  msg_arg = msg_buf;
  sm_ret = -1;
  wait_cnt = 0;
  _DAT_1f80008c = 0;
  do {
    if (br2_bail_pending()) { br2_guest_frame_free(); return sm_ret; }
    if (sm_state != 1) {
      if (sm_state < 2) {
        if ((sm_state == 0) && (rc = frame_divisor_get(), rc == 0)) {
          print_mode_dispatch_b(2,0,0);
          rc = intro_movie_state(0);
          if (rc != 0) {
            deferred_cmd_post(0x203,0,0);
            /* desc+0xc is read by menu_stepper_state_advance as a u32
             * GUEST code address (fn_lookup -> call_guest fallback).
             * Guest stores 0x80160598 (dialog_sel_dispatch); a host fn
             * pointer would be truncated to garbage. */
            local_b4 = 0x80160598;
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
            if (a0 != 0) {
              ow_flag = 0xffffffff;
              dlg_type = 1;
              slot_no = 0;
              local_bd = '\x11';
            }
            battle_session_arm(0x1f800000,0xffffffff);
            sm_state = 1;
            goto LAB_8015f820;
          }
        }
        goto LAB_80160550;
      }
      if (sm_state == 0xf0) {
        deferred_cmd_post(0x203,0,0);
      }
      else if (sm_state != 0xf1) goto LAB_80160550;
      sm_state = 0xff;
      goto LAB_80160558;
    }
LAB_8015f820:
    if (a0 == 0) {
      menu_stepper_state_advance(&local_c0);
    }
    switch(local_bd) {
    case '\0':
      if ((_DAT_1f80007c & 0x10) != 0) {
        deferred_cmd_post(0x400,0xd,0);
        sm_state = 0xf0;
      }
      if ((local_b8 == local_ba) && ((_DAT_1f80007c & 0xe0) != 0)) {
        deferred_cmd_post(0x400,0xc,0);
LAB_8015f8c0:
        sm_state = 0xf0;
      }
      break;
    case '\x10':
      ow_flag = 1;
      sm_ret = -1;
      if (local_b8 == 2) {
        slot_no = (int)local_aa;
        dlg_type = 0;
        goto code_r0x8015ffac;
      }
      if (local_b8 < 3) {
        if (local_b8 == 1) {
          dlg_type = 4;
          slot_no = 0;
LAB_8015fbfc:
          local_bd = '\x12';
        }
      }
      else if (local_b8 == 3) {
        slot_no = (int)local_a8;
        dlg_type = 4;
        goto LAB_8015fbfc;
      }
      break;
    case '\x11':
      if ((int)ow_flag < 0) goto code_r0x8015ffac;
      scroll_offset_set(0x10,0x68);
      switch(dlg_type) {
      case 0:
        sprintf_core(msg_buf,s_LOAD_FROM_MEMORY_CARD__s___80121788,
                     SLOTSTR(slot_no));
        break;
      case 2:
        sprintf_core(msg_buf,s_FORMAT_MEMORY_CARD__s___801217d8,SLOTSTR(slot_no));
        break;
      case 3:
        sprintf_core(msg_buf,s_SAVING_TO_MEMORY_CARD__s__801217f0,SLOTSTR(slot_no));
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,msg_arg,0x10,0x1000,0);
        scroll_offset_accumulate(0,0x10);
        sprintf_core(msg_buf,s_OVER_WRITE_OK___8012180c);
      default:
        break;
      case 5:
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_MEMORY_CARD_CHANGED__801217a4,0x10,0x1000,2);
        scroll_offset_accumulate(0,0x10);
      case 1:
        sprintf_core(msg_buf,s_SAVE_TO_MEMORY_CARD__s___801217bc,SLOTSTR(slot_no))
        ;
      }
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,msg_buf,0x10,0x1000,0);
      scroll_offset_accumulate(0,0x10);
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,&DAT_8012181c,0x30,0x1000,ow_flag != 0);
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,&DAT_80121820,0x90,0x1000,ow_flag == 0);
      scroll_offset_accumulate(0,0x10);
      switch(dlg_type) {
      case 1:
      case 5:
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_1_BLOCK_IS_REQUIRED_TO_SAVE__80121824,0x10,0x1000
                     ,3);
      }
      scroll_offset_accumulate(0,0x10);
      if ((_DAT_1f80007c & 0xa000) != 0) {
        deferred_cmd_post(0x400,0xb,0);
        ow_flag = ow_flag ^ 1;
      }
      if ((_DAT_1f80007c & 0x10) == 0) {
        if ((_DAT_1f80007c & 0xe0) != 0) {
          deferred_cmd_post(0x400,0xc,0);
          switch(dlg_type) {
          case 0:
          case 1:
switchD_8015fbc4_caseD_0:
            if (ow_flag == 0) goto LAB_8015fbfc;
            goto LAB_8015fc08;
          case 2:
          case 3:
          case 5:
            if (a0 == 0) goto switchD_8015fbc4_caseD_0;
            if (ow_flag == 0) goto LAB_8015fbfc;
            local_bd = '\x11';
            dlg_type = 1;
            ow_flag = 1;
          }
        }
      }
      else {
        deferred_cmd_post(0x400,0xd,0);
LAB_8015fc08:
        local_bd = ' ';
      }
      break;
    case '\x12':
    case '\x13':
      scroll_offset_set(0x10,0x68);
      switch(dlg_type) {
      case 0:
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_NOW_LOADING_FROM_MEMORY_CARD_80121844,0x10,0x1000
                     ,0);
        str_arg = SLOTSTR(slot_no);
        fmt_str = s__s____80121864;
        break;
      case 2:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_NOW_FORMATTING_MEMORY_CARD__s____8012188c;
        break;
      case 3:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_NOW_SAVING_TO_MEMORY_CARD__s____8012186c;
        break;
      case 4:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_NOW_CHECKING_MEMORY_CARD__s____801218b0;
        break;
      case 5:
        dlg_type = 1;
      case 1:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_NOW_SAVING_TO_MEMORY_CARD__s____8012186c;
        break;
      default:
        goto switchD_8015fc3c_default;
      }
      sprintf_core(msg_buf,fmt_str,str_arg);
switchD_8015fc3c_default:
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,msg_buf,0x10,0x1000,0);
      scroll_offset_accumulate(0,0x10);
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_DO_NOT_REMOVE_MEMORY_CARD___801218d0,0x10,0x1000,3)
      ;
      goto code_r0x8015ffac;
    case '\x14':
      if (slot_no == 0) {
        op_sel = 0;
      }
      else if (slot_no == 1) {
        op_sel = 0x10;
      }
      switch(dlg_type) {
      case 0:
        op_rc = mc_data_read(op_sel,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608,DAT_801b460c,DAT_801b4610,DAT_801b4614,
                                PTR_DAT_801b4618,&DAT_801c11e0,0x414);
        if ((op_rc == 0) && (rc = save_blk_cmp32(), rc != 0)) {
          op_rc = 2;
        }
        break;
      case 1:
        op_rc = mc_data_write(op_sel,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608,DAT_801b460c,DAT_801b4610,DAT_801b4614,
                                PTR_DAT_801b4618,&DAT_801c11e0,0x414);
        break;
      case 2:
        op_rc = mc_op_dispatch(op_sel);
        break;
      case 3:
        op_rc = mc_read_queue(op_sel,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608,DAT_801b460c,DAT_801b4610,DAT_801b4614,
                                PTR_DAT_801b4618,&DAT_801c11e0,0x414);
        break;
      case 4:
        op_rc = mc_op_dispatch_b(op_sel,DAT_801b45f4,DAT_801b45f8,DAT_801b45fc,DAT_801b4600,
                                DAT_801b4604,DAT_801b4608);
      }
      wait_cnt = 100;
      sm_ret = op_rc;
      if (a0 == 0) {
        wait_cnt = -1;
      }
code_r0x8015ffac:
      local_bd = local_bd + '\x01';
      break;
    case '\x15':
      scroll_offset_set(0x10,0x68);
      switch(dlg_type) {
      case 0:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_LOAD_FROM_MEMORY_CARD__s__801218ec;
        break;
      case 1:
      case 3:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_SAVE_TO_MEMORY_CARD__s__80121908;
        break;
      case 2:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_FORMAT_MEMORY_CARD__s__80121920;
        break;
      case 4:
        str_arg = SLOTSTR(slot_no);
        fmt_str = s_CHECKING_MEMORY_CARD__s__80121938;
        break;
      default:
        goto switchD_8015ffe4_default;
      }
      sprintf_core(msg_buf,fmt_str,str_arg);
switchD_8015ffe4_default:
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,msg_arg,0x10,0x1000,0);
      scroll_offset_accumulate(0,0x10);
      if ((_DAT_1f80007c & 0xe0) != 0) {
        deferred_cmd_post(0x400,0xc,0);
        local_bd = '\x16';
      }
      if ((_DAT_1f80007c & 0x10) != 0) {
        deferred_cmd_post(0x400,0xd,0);
        local_bd = '\x16';
      }
      if (op_rc == 4) {
        if (dlg_type == 0) {
          fmt_str = s_NO_BLOODY_ROAR_2_DATA_DETECTED___801219e4;
        }
        else {
          fmt_str = s_UNFORMATTED_MEMORY_CARD___80121a08;
        }
      }
      else if (op_rc < 5) {
        if (op_rc != 1) {
          if (op_rc < 2) {
            if (op_rc != 0) goto LAB_801603ec;
            switch(dlg_type) {
            case 0:
              gauge_scale_fixed(*(uint8_t *)(_g_system_config_ptr + 0xe),
                           ((uint)*(byte *)(_g_system_config_ptr + 0xf) << 3) / 10);
              brightness_commit(*(byte *)(_g_system_config_ptr + 0xc) ^ 1);
            case 1:
            case 3:
              _DAT_801c15f0 = 0;
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_COMPLETED___80121954,0x10,0x1000,3);
              rc = wait_cnt + -1;
              if ((-1 < wait_cnt) && (wait_cnt = rc, rc == -1)) {
                local_bd = '\x16';
              }
              break;
            case 2:
              ow_flag = 0xffffffff;
              if (a0 == 0) {
                if (local_b8 != 2) {
                  if (local_b8 < 3) {
                    if (local_b8 == 1) goto LAB_8016026c;
                    local_bd = '\x11';
                  }
                  else {
                    if (local_b8 == 3) {
                      dlg_type = 1;
                      slot_no = (int)local_a8;
                      goto LAB_8016028c;
                    }
                    local_bd = '\x11';
                  }
                  break;
                }
                slot_no = (int)local_aa;
                dlg_type = 0;
              }
              else {
LAB_8016026c:
                dlg_type = 1;
                slot_no = 0;
              }
LAB_8016028c:
              local_bd = '\x11';
              break;
            case 4:
              dlg_type = 1;
              goto LAB_80160464;
            }
          }
          else if (op_rc == 2) {
            announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_DATA_ERROR____80121a24,0x10,0x1000,2);
          }
          else {
            if (op_rc != 3) goto LAB_801603ec;
            ow_flag = 1;
            if (dlg_type == 4) {
              dlg_type = 1;
            }
            else {
              dlg_type = 5;
            }
            local_bd = '\x11';
          }
          break;
        }
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_NO_MEMORY_CARD_IS_INSERTED_IN_80121960,0x10,
                     0x1000,2);
        sprintf_core(msg_buf,s_MEMORY_CARD__s___80121980,SLOTSTR(slot_no));
        fmt_str = msg_arg;
      }
      else if (op_rc == 7) {
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_THERE_IS_INSUFFICIENT_MEMORY_CAR_80121994,0x10,
                     0x1000,2);
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_BLOCKS_AND_1_BLOCK_IS_REQUIRED_801219b8,0x10,
                     0x1000,2);
        fmt_str = s_TO_SAVE__801219d8;
      }
      else {
        if (op_rc < 8) {
          if (op_rc == 5) {
            fmt_str = s_NO_BLOODY_ROAR_2_DATA_DETECTED___801219e4;
            goto LAB_801603fc;
          }
        }
        else if (op_rc == 0xf) {
          dlg_type = 3;
LAB_80160488:
          ow_flag = 1;
          local_bd = '\x11';
          break;
        }
LAB_801603ec:
        fmt_str = s_ERROR___80121a34;
      }
LAB_801603fc:
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,fmt_str,0x10,0x1000,2);
      break;
    case '\x16':
      if (dlg_type == 0) {
        if (a0 != 0) goto LAB_8015f8c0;
        if (op_rc == 0) goto LAB_80160510;
        if (local_b8 != 1) {
          local_bd = '\0';
          break;
        }
        goto LAB_80160504;
      }
      if (op_rc == 5) {
LAB_80160494:
        if (a0 != 0) goto LAB_8015f8c0;
LAB_801604a4:
        if (op_rc != 0) goto LAB_801604f4;
        goto LAB_80160510;
      }
      if (5 < op_rc) {
        if (op_rc == 0xf) goto LAB_80160494;
LAB_80160470:
        if (a0 == 0) goto LAB_801604a4;
        dlg_type = 1;
        goto LAB_80160488;
      }
      if (op_rc < 4) {
        if ((op_rc < 1) && (op_rc == 0)) goto LAB_80160494;
        goto LAB_80160470;
      }
      dlg_type = 2;
LAB_80160464:
      ow_flag = 1;
      local_bd = '\x11';
      break;
    case ' ':
    case '!':
      if (a0 != 0) goto LAB_8015f8c0;
LAB_801604f4:
      if (local_b8 == 1) {
LAB_80160504:
        *(uint8_t *)(_g_system_config_ptr + 0x22) = 0;
      }
LAB_80160510:
      local_bd = '\0';
    }
    if (a0 == 0) {
      hud_gauge_render(0x1f800000,_DAT_1f800008,7);
    }
LAB_80160550:
    frame_tick(0x1f800000);
LAB_80160558:
    if (sm_state == 0xff) {
      br2_guest_frame_free();
      return sm_ret;
    }
  } while( true );
}
#undef local_c0
#undef local_be
#undef local_bd
#undef local_bc
#undef local_bb
#undef local_ba
#undef local_b8
#undef local_b4
#undef local_b0
#undef local_ae
#undef local_ac
#undef local_aa
#undef local_a8
#undef local_a6
#undef local_a4
#undef local_a2
#undef local_a0
#undef local_9e

/* ======== 0x80156ff0 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void memcard_slot_poll(void)

{
  short age;
  int ent_off;
  uint8_t *rec;
  char *slot_flag;
  int slot_i;
  char status_buf [24];
  
  slot_status_fill(status_buf);
  slot_flag = &DAT_801f24d0;
  slot_i = 0;
  rec = &DAT_801f24d1;
  do {
    if (*slot_flag != '\0') {
      if (*slot_flag == -0x80) {
        *(uint16_t *)(rec + 0xb) = 0;
        *slot_flag = '\x01';
        _DAT_801f2650 = *(uint32_t *)(rec + 7);
        _DAT_801f2654 = 0x600e3;
        _DAT_801f2666 = 0;
        ent_off = (uint)*(ushort *)(rec + 5) * 8;
        _DAT_801f2668 = *(uint16_t *)guest_at((uint32_t)(ent_off + -0x7fe0d41e));
        _DAT_801f2658 = *(uint16_t *)(rec + 1);
        _DAT_801f265a = *(uint16_t *)(rec + 3);
        _DAT_801f266c = *(uint32_t *)guest_at((uint32_t)((uint)*(ushort *)guest_at((uint32_t)(ent_off + -0x7fe0d420)) * 4 + -0x7fe0d920));
        _DAT_801f268a = *(uint16_t *)guest_at((uint32_t)(ent_off + -0x7fe0d41c));
        _DAT_801f268c = *(uint16_t *)guest_at((uint32_t)(ent_off + -0x7fe0d41a));
        cd_reg_window_update(&DAT_801f2650);
      }
      else {
        age = *(short *)(rec + 0xb);
        *(short *)(rec + 0xb) = age + 1;
        if ((short)(age + 1) == 2) {
          cd_reg_window_u16(0,*(uint32_t *)(rec + 7));
        }
        if ((status_buf[slot_i] == '\0') && (0x14 < *(short *)(rec + 0xb))) {
          *slot_flag = '\0';
          *rec = 0;
        }
      }
    }
    slot_i = slot_i + 1;
    rec = rec + 0x10;
    slot_flag = slot_flag + 0x10;
  } while (slot_i < 0x18);
  return;
}

/* ======== 0x80156d98 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void deferred_cmd_drain(uint32_t a0,int a1)

{
  ushort cmdw;
  bool cls300_seen;
  bool cls400_seen;
  ushort cmdcls;
  short *rec_s;
  ushort *rec_u;
  int slot_i;
  
  cls300_seen = false;
  cls400_seen = false;
  rec_u = (ushort *)&DAT_801f3600;
  slot_i = 0;
  rec_s = (short *)&DAT_801f3602;
  do {
    cmdw = *rec_u;
    if (*rec_u != 0) {
      if (rec_s[1] == 0) {
        cmdcls = cmdw & 0xf00;
        if (cmdcls == 0x200) {
          a1 = mc_action_dispatch_b(cmdw & 0xff,(int)*rec_s);
        }
        else if (cmdcls < 0x201) {
          if (cmdcls == 0x100) {
            a1 = mc_action_dispatch(cmdw & 0xff,(int)*rec_s);
          }
        }
        else if (cmdcls == 0x300) {
          if (cls300_seen) goto LAB_80156ebc;
          cls300_seen = true;
          a1 = ret_one(cmdw & 0xff,(int)*rec_s);
        }
        else if (cmdcls == 0x400) {
          if (cls400_seen) goto LAB_80156ebc;
          cls400_seen = true;
          a1 = deferred_cmd_exec(cmdw & 0xff,(int)*rec_s);
        }
        if (a1 != 0) {
          *rec_u = 0;
          _DAT_801f35f4 = _DAT_801f35f4 + -1;
        }
      }
      else {
        rec_s[1] = rec_s[1] + -1;
      }
    }
LAB_80156ebc:
    slot_i = slot_i + 1;
    rec_s = rec_s + 3;
    rec_u = rec_u + 3;
    if (0x1f < slot_i) {
      return;
    }
  } while( true );
}

/* ======== 0x8015ab4c ======== */

void save_defaults_fill(void)

{
  uint16_t def_val;
  uint8_t *dst;
  uint16_t *src;
  int i;
  
  i = 0;
  src = &DAT_801b3f78;
  dst = &DAT_801c11e0;
  do {
    def_val = *src;
    src = src + 1;
    i = i + 1;
    *(uint16_t *)(dst + 0x400) = def_val;
    *(uint16_t *)(dst + 0x3f0) = def_val;
    dst = dst + 2;
  } while (i < 8);
  return;
}

/* ======== 0x80167a64 ======== */

void save_block_init(void)

{
  uint8_t *src_name;
  uint8_t *dst;
  uint8_t *src;
  int i;
  int j;
  int n;
  
  src_name = PTR_s___BloodyRoar2__byRAIZINGver1_60_801b43f0;
  i = 0;
  n = 0x1f;
  do {
    n = n + -1;
    dst = &DAT_801c11e0 + i;
    src = src_name + i;
    i = i + 1;
    *dst = *src;
  } while (-1 < n);
  i = 0;
  n = 0x264;
  do {
    j = 10;
    dst = &DAT_801c11f4 + n;
    *(uint16_t *)(&DAT_801c11ec + n) = 0;
    *(uint16_t *)(&DAT_801c11e8 + n) = 0;
    *(uint16_t *)(&DAT_801c11e4 + n) = 0;
    *(uint16_t *)guest_at((uint32_t)(n + -0x7fe3ee1e)) = 0;
    *(uint16_t *)(&DAT_801c11e0 + n) = 0;
    do {
      *(uint16_t *)(dst + 0xe) = 0;
      j = j + -1;
      dst = dst + -2;
    } while (-1 < j);
    i = i + 1;
    n = n + 0x24;
  } while (i < 0xb);
  return;
}

/* ======== 0x80163ab4 ======== */

void save_tables_commit(void)

{
  save_rec_table_copy(&DAT_801c1264,0,10);
  save_rec_table_copy(&DAT_801c1304,5,10);
  save_rec_table_copy(&DAT_801c13a4,0,10);
  return;
}

/* ======== 0x8018babc ======== */

void dbg_marker_mode_set(int a0)

{
  int idx;
  int j;
  int i;
  
  DAT_801f5969 = 0;
  DAT_801f5968 = 0;
  if (a0 == 1) {
    DAT_801b55d0 = 1;
    return;
  }
  if (a0 < 2) {
    if (a0 == 0) {
LAB_8018bb4c:
      i = 0;
      do {
        j = 0;
        idx = i * 2;
        do {
          j = j + 1;
          (&DAT_801f596c)[idx] = 0;
          (&DAT_801f5970)[idx] = 0;
          idx = j + i * 2;
        } while (j < 2);
        i = i + 1;
      } while (i < 2);
      DAT_801b55d0 = (char)a0;
      return;
    }
  }
  else {
    if (a0 == 2) goto LAB_8018bb4c;
    if (a0 == 3) {
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

/* gauge_scale_fixed lives in menu.c (disasm-verified magic-divide port). */

/* ======== 0x801561c8 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void brightness_commit(uint16_t a0)

{
  _DAT_801f36c0 = a0;
  brightness_packet_send(0xffffffff);
  return;
}


/* ======== system/shell helpers ======== */

/* ======== 0x801525c4 ======== */

void rec_timer_pair_clear(intptr_t a0)

{
  uint8_t *p = (uint8_t *)guest_at((uint32_t)a0);
  *(uint16_t *)(p + 0xb4) = 0;
  *(uint16_t *)(p + 0xb6) = 0;
  return;
}

/* ======== 0x80129f64 ======== */

void mode_flag_pair_set(uint8_t a0)

{
  DAT_801e83aa = a0;
  DAT_801e83b0 = a0;
  return;
}

/* ======== 0x80129ff4 ======== */

void perm_table_load(void)

{
  uint8_t *dst;
  int i;
  int j;
  
  dst = (uint8_t *)guest_at(0x801e8400);
  i = 0;
  do {
    j = i + 1;
    *dst = (&DAT_8011ec40)[i];
    dst = (uint8_t *)guest_at((uint32_t)(i + -0x7fe17bff));
    i = j;
  } while (j < 0x100);
  return;
}

/* ======== 0x8012a030 ======== */

void perm_table_apply(uint a0)

{
  byte perm_idx;
  uint8_t swap_tmp;
  uint8_t *cell;
  int i;
  
  i = 0;
  do {
    cell = (uint8_t *)guest_at((uint32_t)(i + -0x7fe17c00));
    perm_idx = (&DAT_8011ec40)[a0 & 0xff];
    swap_tmp = *cell;
    i = i + 1;
    *cell = *(uint8_t *)guest_at((uint32_t)(perm_idx + 0x801e8400));
    *(uint8_t *)guest_at((uint32_t)(perm_idx + 0x801e8400)) = swap_tmp;
    a0 = a0 + 1;
  } while (i < 0x100);
  return;
}

/* ======== 0x8012aa6c ======== */

void scroll_cfg_set(uint8_t a0,int a1)

{
  DAT_1f800017 = a0;
  DAT_1f800018 = (char)a1;
  DAT_1f800014 = DAT_1f80000c;
  if (a1 == 0) {
    DAT_1f800019 = 0;
    return;
  }
  DAT_1f800019 = (char)(0x100 / a1) + '\x01';
  return;
}

/* ======== 0x8019a5d8 ======== */

/* vsync_wait_n — clean port now lives in game/cd.c next to cd_jal /
 * cd_timer_read (it needs cd_jal for the timeout path's three-jal
 * sequence and br2_irq_poll for the frame-counter spin). The fnmap /
 * clean_table entries are unchanged: the definition is still named
 * vsync_wait_n(int,int) at guest 0x8019a5d8. */

/* ======== 0x8012b49c ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void flow_cfg_init(uint8_t a0)

{
  int idx;
  int i;
  
  uint32_t *cfg;
  empty_ret();
  _DAT_1f800082 = 0;
  cfg = (uint32_t *)guest_at(_DAT_1f800084);
  *cfg = 0;
  cfg[1] = 0;
  cfg[2] = 0;
  i = 0;
  cfg[4] = a0;
  do {
    idx = i * 2;
    i = i + 1;
    *(uint16_t *)((char *)cfg + (idx + 6) * 4) = 0;
  } while (i < 8);
  if (_DAT_1f8000b8 == 0) {
    cfg[3] = 3;
  }
  else {
    cfg[3] = 4;
  }
  if (DAT_1f80009b == '\x04') {
    cfg[3] = 7;
  }
  return;
}

/* ======== 0x8018e2f0 ======== */

void guest_bzero(uint32_t addr,uint32_t size)

{
  /* guest 0x8018e2f0 is a BIOS trampoline (`li t2,0xa0; li t1,0x39; jr t2`)
   * — kernel service A0/0x39 = memset(dst,len). Route through bios_svc so
   * both clean callers and guest dispatch hit the recompiled BIOS. */
  bios_svc(0xa0,0x39,addr,size);
  return;
}

/* ======== 0x8012a650 ======== */

uint attr_word_decode(uint8_t *a0)

{
  byte attr_hi;
  uint attr_word;
  
  attr_hi = *(byte *)(a0 + 1) & 0xf0;
  if (attr_hi == 0x40) {
    attr_word = ~(uint)CONCAT11(*(uint8_t *)(a0 + 2),*(uint8_t *)(a0 + 3)) & 63999;
  }
  else if ((attr_hi < 0x41) || (attr_hi == 0x60)) {
    attr_word = 0xffffffff;
  }
  else {
    if (attr_hi < 0x61) {
      if (attr_hi != 0x50) {
        return 0xffffffff;
      }
    }
    else if (attr_hi != 0x70) {
      return 0xffffffff;
    }
    attr_word = ~(uint)CONCAT11(*(uint8_t *)(a0 + 2),*(uint8_t *)(a0 + 3)) & 63999;
    if (*(uint8_t *)(a0 + 6) < 0x40) {
      attr_word = attr_word | 0x8000;
    }
    else if (0xc0 < *(uint8_t *)(a0 + 6)) {
      attr_word = attr_word | 0x2000;
    }
    if (*(uint8_t *)(a0 + 7) < 0x40) {
      attr_word = attr_word | 0x1000;
    }
    else if (0xc0 < *(uint8_t *)(a0 + 7)) {
      attr_word = attr_word | 0x4000;
    }
  }
  return attr_word;
}

/* ---- batch30: memcard/PAD helpers ---- */
#define SB8(a)   (*(uint8_t  *)guest_at(a))
#define SB16(a)  (*(uint16_t *)guest_at(a))
#define SB32(a)  (*(uint32_t *)guest_at(a))
extern uint8_t *_g_pad_buffer;
extern int frame_divisor_get(void);
extern int slot_display_pos(uint8_t *,uint32_t);
extern void deferred_cmd_post(int,int,int);
extern void mc_state_init(void);
extern void chan_sweep_flag100(void);
extern void chan_sweep_flag200(void);
extern void datetime_confirm_a(int,uint32_t);
extern void datetime_confirm_b(int,uint32_t);
extern int  cd_cmd_issue_sync(int,int,int);

/* memcard_err_sentinel — memcard error sentinel (-99). */
uint32_t memcard_err_sentinel(void)
{
  return 0xffffff9d;
}

/* mc_confirm_dispatch — PAD-start gated memcard confirm dispatch.
 * pad_buffer+2 bit12 = confirm press; DAT_801bef6e low nibble =
 * dialog state (1 -> datetime_confirm_a path, 2 -> datetime_confirm_b path). */
void mc_confirm_dispatch(int32_t a0,uint32_t a1)
{
  if ((((*(uint16_t *)(_g_pad_buffer + 2) >> 0xc) & 1) != 0) && (0 <= a0)) {
    mc_state_init();
    if ((SB8(0x801bef6e) & 0xf) == 1) {
      chan_sweep_flag100();
      datetime_confirm_a(a0,a1);
    } else if ((SB8(0x801bef6e) & 0xf) == 2) {
      chan_sweep_flag200();
      datetime_confirm_b(a0,a1);
    }
  }
}

/* mc_init_reset — memcard init/reset sequence: brightness off,
 * sync wait via cd_cmd_issue_sync(9), dialog-state globals cleared. */
void mc_init_reset(void)
{
  extern int br2_bail_pending(void);
  int rc;
  brightness_packet_send(0);
  do {
    if (br2_bail_pending()) return;
    rc = cd_cmd_issue_sync(9,0,0);
  } while (rc == 0);
  chan_sweep_flag100();
  chan_sweep_flag200();
  SB8(0x801bef6e) = 2;
  SB8(0x801bef6d) = 0;
  SB8(0x801bef76) = 0;
  SB8(0x801bef6f) = 0xff;
  SB8(0x801bef80) = SB8(0x801bef84);
}

/* save_slot_state_fixup — save-slot state fixup @guest 0x1f800000.
 * a0 = guest address (bridged). */
void save_slot_state_fixup(uint32_t a0,uint32_t a1)
{
  uint8_t *p = (uint8_t *)guest_at(a0);
  int32_t disp_rc;
  (void)a1;
  disp_rc = frame_divisor_get();
  if (disp_rc == 0) {
    disp_rc = slot_display_pos(p,a1);
    if (0 <= disp_rc) {
      p[0] = 5;
      p[1] = 0xf0;
    }
    if ((disp_rc == -99) && ((*(uint16_t *)(p + 0x7c) & 0x840) != 0)) {
      deferred_cmd_post(0x400,0xc,0);
      p[0] = 5;
      p[1] = 0xf0;
    }
    if ((*(uint16_t *)(p + 0x7c) & 0x10) != 0) {
      deferred_cmd_post(0x400,0xd,0);
      p[1] = 0xf0;
    }
  }
}

/* mc_pad_button_handler — memcard button-press handler: PAD bits 0x840 on
 * +0x68/+0x6a trigger deferred_cmd_post sfx + mc_slot_action slot action.
 * a0 = host ptr to save-manager state block. */
extern int32_t ret_zero_b(uint32_t);
extern void mc_slot_action(uint8_t *,int32_t);
uint32_t mc_pad_button_handler(uint8_t *a0)
{
  int32_t chk_rc;
  uint32_t snd_id;
  uint32_t handled = 0;
  a0[0xb4] = 0; a0[0xb5] = 0; a0[0xb6] = 0; a0[0xb7] = 0;
  if (a0[0x9b] == 0) {
    if (((*(uint16_t *)(a0 + 0x68) & 0x840) != 0) &&
        (chk_rc = ret_zero_b(0), 0 <= chk_rc)) {
      snd_id = (*a0 == 5) ? 0x80 : 0xc;
      deferred_cmd_post(0x400,snd_id,0);
      mc_slot_action(a0,0);
      handled = 1;
    }
    if ((*(uint16_t *)(a0 + 0x6a) & 0x840) == 0) return handled;
    chk_rc = ret_zero_b(1);
    if (chk_rc < 0) return handled;
    if (*a0 == 5) { snd_id = 0x80; goto LAB_801533b0; }
  } else {
    if (3 < a0[0x9b]) return 0;
    if ((*(uint16_t *)(a0 + 0x68) & 0x840) != 0) {
      snd_id = (*a0 == 5) ? 0x80 : 0xc;
      deferred_cmd_post(0x400,snd_id,0);
      mc_slot_action(a0,0);
      handled = 1;
    }
    if ((*(uint16_t *)(a0 + 0x6a) & 0x840) == 0) return handled;
    if (*a0 == 5) { snd_id = 0x80; goto LAB_801533b0; }
  }
  snd_id = 0xc;
LAB_801533b0:
  deferred_cmd_post(0x400,snd_id,0);
  mc_slot_action(a0,1);
  return handled | 2;
}

/* save_rec_table_copy — 16-byte record copy between caller buffer and one
 * of four static tables selected by a1:
 *   0: 0x801b427c -> dst   1: dst -> 0x801c1264   2: 0x801c1264 -> dst
 *   3: dst -> 0x801c1304   4: 0x801c1304 -> dst   5: 0x801b431c -> dst
 * a2 = record count. */
void save_rec_table_copy(uint32_t *a0,uint32_t a1,int32_t a2)
{
  int32_t i;
  uint32_t *tab_a, *tab_b;
  uint32_t w1, w2, w3;
  tab_a = (uint32_t *)g_DAT_801c1264;
  tab_b = (uint32_t *)g_DAT_801c1304;
  switch (a1) {
  case 0:
    i = 0;
    if (0 < a2) {
      tab_a = (uint32_t *)guest_at(0x801b427c);
      do {
        w1 = tab_a[1]; w2 = tab_a[2]; w3 = tab_a[3];
        a0[0] = tab_a[0]; a0[1] = w1; a0[2] = w2; a0[3] = w3;
        a0 += 4; i++; tab_a += 4;
      } while (i < a2);
    }
    break;
  case 1:
    i = 0;
    if (0 < a2) {
      do {
        w1 = a0[1]; w2 = a0[2]; w3 = a0[3];
        tab_a[0] = a0[0]; tab_a[1] = w1; tab_a[2] = w2; tab_a[3] = w3;
        tab_a += 4; i++; a0 += 4;
      } while (i < a2);
      return;
    }
    break;
  case 2:
    i = 0;
    if (0 < a2) {
      do {
        w1 = tab_a[1]; w2 = tab_a[2]; w3 = tab_a[3];
        a0[0] = tab_a[0]; a0[1] = w1; a0[2] = w2; a0[3] = w3;
        a0 += 4; i++; tab_a += 4;
      } while (i < a2);
      return;
    }
    break;
  case 3:
    i = 0;
    if (0 < a2) {
      do {
        w1 = a0[1]; w2 = a0[2]; w3 = a0[3];
        tab_b[0] = a0[0]; tab_b[1] = w1; tab_b[2] = w2; tab_b[3] = w3;
        tab_b += 4; i++; a0 += 4;
      } while (i < a2);
      return;
    }
    break;
  case 4:
    i = 0;
    if (0 < a2) {
      do {
        w1 = tab_b[1]; w2 = tab_b[2]; w3 = tab_b[3];
        a0[0] = tab_b[0]; a0[1] = w1; a0[2] = w2; a0[3] = w3;
        a0 += 4; i++; tab_b += 4;
      } while (i < a2);
      return;
    }
    break;
  case 5:
    i = 0;
    if (0 < a2) {
      tab_a = (uint32_t *)guest_at(0x801b431c);
      do {
        w1 = tab_a[1]; w2 = tab_a[2]; w3 = tab_a[3];
        a0[0] = tab_a[0]; a0[1] = w1; a0[2] = w2; a0[3] = w3;
        a0 += 4; i++; tab_a += 4;
      } while (i < a2);
      return;
    }
  }
  return;
}

/* ======== batch31: memcard slot / date helpers (0x8012,0x8015,0x8016) ======== */
extern uint8_t  *g_DAT_801e40b8;
extern uint8_t  *g_DAT_801e6208;
extern uint32_t *g_PTR_DAT_801b5b94;
extern int      str_len(char *);
extern uint8_t table_byte_next_a(void);
extern int32_t  mc_dialog_bit4(int32_t);
extern void     ret_stub_b();
extern void     hud_text_blk_alt(uint32_t,uint32_t,const void *,int32_t,int32_t,int32_t);
extern void     chan_blocks_reset(uint32_t,uint32_t,int32_t);
extern int      slot_icon_nibble_resolve(uint8_t *);
extern void     picker_table_shuffle(uint8_t *);

/* dialog_mode_mirror — mirror a mode byte into the two dialog-state cells
 * (guest 0x801e40b8 and 0x801e6208). */
void dialog_mode_mirror(int32_t a0)
{
  *g_DAT_801e6208 = (uint8_t)a0;
  *g_DAT_801e40b8 = (uint8_t)a0;
}

/* slot_name_measure — print the slot-name string for fighter index a1
 * (table @0x801b47ec, 8-byte entries {char *name0, char *name1}) then
 * measure a0 with str_len. Returns str_len's result. */
int slot_name_measure(void *a0,int32_t a1)
{
  uint32_t name_ent = *(uint32_t *)guest_at(0x801b47ec + ((uint32_t)a1 << 3));
  /* disasm-verified @0x801513fc: guest does sprintf(a0, name) straight
   * into the caller's buffer (a0), then str_len(a0). a0 is already
   * a host pointer into guest memory, so no staging buffer is needed. */
  sprintf_core((char *)a0,(char *)guest_at(name_ent));
  return str_len((char *)a0);
}

/* chan_blocks_reset — clear the 0x1f8000e8/0x1f8001f0 channel blocks: zero
 * +0xe0..+0xec fields, store a1 at +0xf0, then a2 selects the
 * +0xec counter (0 -> clear, 1 -> increment, other -> untouched).
 * a0 is a GUEST address (0x1f80xxx scratch). */
void chan_blocks_reset(uint32_t a0,uint32_t a1,int32_t a2)
{
  uint8_t *p = (uint8_t *)guest_at(a0);
  *(uint16_t *)(p + 0xe0) = 0;
  *(uint16_t *)(p + 0xe6) = 0;
  *(uint16_t *)(p + 0xe8) = 0;
  *(uint16_t *)(p + 0xea) = 0;
  *(uint16_t *)(p + 0xe2) = 0;
  p[0xe4] = 0;
  *(uint32_t *)(p + 0xf0) = a1;
  if (a2 == 0) {
    *(uint16_t *)(p + 0xec) = 0;
    return;
  }
  if (a2 == 1) {
    *(uint16_t *)(p + 0xec) = (uint16_t)(*(uint16_t *)(p + 0xec) + 1);
  }
}

/* slot_icon_nibble_resolve — resolve the save-slot icon nibble: index =
 * (int16)a0[0xa4] + (c0block[7] << 4) into the byte table at
 * guest 0x801b3f88; store value<<4 at a0+0xa0, return it. */
int slot_icon_nibble_resolve(uint8_t *a0)
{
  uint8_t *p = (uint8_t *)guest_at(*(uint32_t *)(a0 + 0xc0));
  uint32_t tbl_addr = 0x801b3f88u + (uint32_t)(int32_t)*(int16_t *)(a0 + 0xa4) +
                   ((uint32_t)p[7] << 4);
  int32_t icon_val = *(uint8_t *)guest_at(tbl_addr);
  icon_val = icon_val << 4;
  *(int16_t *)(a0 + 0xa0) = (int16_t)icon_val;
  return icon_val;
}

/* picker_table_shuffle — shuffle the two 11-byte picker tables at guest
 * 0x801f47a0 / 0x801f4770 using table_byte_next_a() rand bytes, selected by
 * a0[0x9c]: 0 -> Fisher-Yates on table A (mod 9), rotate the
 * A[7..10] window, swap B[4]/B[5], Fisher-Yates on B (mod 5);
 * 1..5 -> Fisher-Yates on A (mod 9) then B[i] = rand % 8 (11 entries);
 * anything else -> just the slot_icon_nibble_resolve icon resolve + +0xa4 clear. */
void picker_table_shuffle(uint8_t *a0)
{
  uint8_t *tab_a = (uint8_t *)guest_at(0x801f47a0);
  uint8_t *tab_b = (uint8_t *)guest_at(0x801f4770);
  uint8_t *cur;
  uint8_t tmp;
  int32_t n;
  int32_t i;
  int32_t j;

  for (n = 0; n < 0xb; n = n + 1) {
    tab_a[n] = (uint8_t)n;
    tab_b[n] = (uint8_t)(n % 9);
  }
  *(int16_t *)(a0 + 0xa4) = 0;
  slot_icon_nibble_resolve(a0);
  n = a0[0x9c];
  if (n == 0) {
    cur = tab_a;
    for (i = 0; i < 9; i = i + 1) {
      j = (int32_t)table_byte_next_a() % 9;
      tmp = cur[i];
      cur[i] = cur[j];
      cur[j] = tmp;
    }
    /* disasm-verified 4-rotate @0x8015a09c-0xa0cc:
     * new[7]=old[9], new[8]=old[10], new[9]=old[7], new[10]=old[8]. */
    {
      uint8_t o7 = tab_a[7], o8 = tab_a[8];
      uint8_t o9 = tab_a[9], o10 = tab_a[10];
      tab_a[9] = o7;
      tab_a[10] = o8;
      tab_a[7] = o9;
      tab_a[8] = o10;
    }
    tmp = tab_b[4];
    tab_b[4] = tab_b[5];
    tab_b[5] = tmp;
    cur = tab_b;
    for (i = 0; i < 5; i = i + 1) {
      j = (int32_t)table_byte_next_a() % 5;
      tmp = cur[i];
      cur[i] = cur[j];
      cur[j] = tmp;
    }
    return;
  }
  if (6 <= n) {
    return;
  }
  cur = tab_a;
  for (i = 0; i < 9; i = i + 1) {
    j = (int32_t)table_byte_next_a() % 9;
    tmp = cur[i];
    cur[i] = cur[j];
    cur[j] = tmp;
  }
  cur = tab_b;
  for (i = 0; i < 0xb; i = i + 1) {
    /* guest: v1 = rand; v0 = (v1 < 0) ? v1 + 7 : v1;
     * m = v1 - ((v0 >> 3) << 3)  (signed mod-8 via shifts) */
    j = (int32_t)table_byte_next_a();
    n = (j < 0) ? j + 7 : j;
    n = j - ((n >> 3) << 3);
    cur[i] = (uint8_t)n;
  }
}

/* mc_slot_action — memcard slot action dispatched by mc_pad_button_handler.
 * a0 = host ptr to save-manager state; a1 = slot (0/1).
 * Marks the c0-block busy flag, applies the per-state slot-mask table
 * (jump table @0x80120998 indexed by a0[0x9b]), resets the two
 * 0x1f80 scratch channel blocks via chan_blocks_reset, re-shuffles the
 * picker tables (picker_table_shuffle) and pokes channel a1 via
 * ret_stub_b. */
void mc_slot_action(uint8_t *a0,int32_t a1)
{
  uint8_t *s1 = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *s2 = (uint8_t *)guest_at(0x1f8001f0);
  uint8_t *q;
  int32_t state;

  q = (uint8_t *)guest_at(*(uint32_t *)(a0 + 0xc0));
  if ((*(uint16_t *)(a0 + ((uint32_t)a1 << 1) + 0x6c) & 0xf) == 0xf) {
    q[0x21] = 1;
  }
  else {
    q[0x21] = 0;
  }
  state = a0[0x9b];
  if (state < 5) {
    switch (state) {
    case 0:
      *(uint16_t *)(a0 + ((uint32_t)a1 << 1) + 0xb4) = 0x100;
      break;
    case 1:
    case 4:
      *(uint16_t *)(a0 + ((uint32_t)a1 << 1) + 0xb4) = 0x100;
      *(uint16_t *)(a0 + ((uint32_t)(a1 ^ 1) << 1) + 0xb4) = 0;
      break;
    case 2:
      *(uint16_t *)(a0 + 0xb4) = 0x100;
      *(uint16_t *)(a0 + 0xb6) = 0x100;
      break;
    case 3:
      *(uint16_t *)(a0 + 0x92) = 0x400;
      *(uint16_t *)(a0 + 0xb4) = 0;
      *(uint16_t *)(a0 + 0xb6) = 0;
      break;
    }
  }
  s1[4] = 0;
  s2[4] = 0;
  q[0x1b] = 0;
  q[0x1c] = 2;
  q[0x19] = q[0x19] & 3;
  q[0x1a] = q[0x1a] & 3;
  picker_table_shuffle(a0);
  chan_blocks_reset(0x1f8000e8,0,0);
  chan_blocks_reset(0x1f8001f0,0,0);
  ret_stub_b(0x1f8000e8 + (uint32_t)a1 * 264,0);
}

/* chan_sweep_flag200 — sweep the 32-entry x6-byte channel table at guest
 * 0x801f3600: entries whose high flag nibble is 0x200 are cleared and
 * the live counter at 0x801f35f4 is decremented. */
void chan_sweep_flag200(void)
{
  uint16_t *ent = (uint16_t *)g_DAT_801f3600;
  int32_t i = 0;
  do {
    if ((*ent & 0xf00) == 0x200) {
      *ent = 0;
      DAT_801f35f4 = DAT_801f35f4 - 1;
    }
    i = i + 1;
    ent = ent + 3;
  } while (i < 0x20);
}

/* chan_sweep_flag100 — same sweep as chan_sweep_flag200 but only entries whose
 * flag nibble is exactly 0x100 are cleared (guest branch chain:
 * skip 0x200, skip >=0x201, require 0x100). */
void chan_sweep_flag100(void)
{
  uint16_t *ent = (uint16_t *)g_DAT_801f3600;
  int32_t i = 0;
  do {
    uint16_t cls = *ent & 0xf00;
    if ((cls != 0x200) && (cls < 0x201) && (cls == 0x100)) {
      *ent = 0;
      DAT_801f35f4 = DAT_801f35f4 - 1;
    }
    i = i + 1;
    ent = ent + 3;
  } while (i < 0x20);
}

/* datetime_confirm_a — date/time dialog confirm path A. a0 = packed
 * date value stored at 0x801bef60+0x12 (clamped modulo byte +0xc),
 * a1 = optional mc_dialog_bit4 hook index. Computes the display
 * string offset table entry at +0x2c+idx*4 through cd_stamp_to_sector and
 * publishes the result at +0x20/+0x24. */
void datetime_confirm_a(int32_t a0,uint32_t a1)
{
  uint8_t *s0 = g_DAT_801bef60;
  int32_t tmp;
  uint8_t flags;
  const uint8_t *str_tbl;

  /* disasm: guest stores are sb (byte) at +0x12 — 0x801575D0/0x801575F8/
   * 0x80157614 `A6020012`/`A6040012`; reads are lh — 0x801575FC `86020012`,
   * 0x80157628 `86040012`. +0x16/+0x14/+0x10 are also sb — 0x80157638/
   * 0x8015763C/0x80157678. */
  s0[0x12] = (uint8_t)a0;
  /* guest compares against sign-extended a0 in-register (sll/sra at
   * 0x801575D4/0x801575DC), not a reload of the halfword. */
  tmp = (int32_t)(int16_t)a0;
  if (s0[0xc] < tmp) {
    /* guest div @0x801575EC + mfhi: HI = tmp % s0[0xc];
     * divisor 0 -> HI = dividend (no trap). */
    s0[0x12] = (uint8_t)(s0[0xc] == 0 ? tmp : tmp % s0[0xc]);
  }
  if (*(int16_t *)(s0 + 0x12) == 0) {
    s0[0x12] = s0[0xc];
  }
  if ((int32_t)a1 >= 0) {
    mc_dialog_bit4((int32_t)a1);
  }
  tmp = *(int16_t *)(s0 + 0x12);
  s0[0xd] = 0x10;
  flags = s0[0xe];
  s0[0x16] = 0;
  s0[0x14] = 0;
  s0[0xf] = 0;
  str_tbl = s0 + 0x2c + tmp * 4;
  s0[0xe] = (uint8_t)((flags & 0xf0) | 1);
  tmp = cd_stamp_to_sector(str_tbl);
  *(uint32_t *)(s0 + 0x24) = (uint32_t)tmp;
  *(uint32_t *)(s0 + 0x20) = (uint32_t)tmp;
  *(uint32_t *)(s0 + 0x28) = 0xffffffffu;
  *(uint32_t *)(s0 + 0x18) = 0;
  s0[0x10] = 0x7f;
}

/* datetime_confirm_b — date/time dialog confirm path B. a0 = packed
 * date value (a0 % 8 -> weekday nibble at +1); looks up the
 * date-string pointer in the table @0x801b5b94 (u32 entries,
 * index = a0/8 — guest sll $3,$3,2 at 0x801576DC), resolves it and the +0x12-indexed string at
 * guest 0x801b3310 via cd_stamp_to_sector, waits on cd_cmd_issue_sync(0xd),
 * then publishes like datetime_confirm_a with flag bits |2. */
void datetime_confirm_b(int32_t a0,uint32_t a1)
{
  extern int br2_bail_pending(void);
  uint8_t *s1 = g_DAT_801bef60;
  uint8_t *date_str;
  int32_t day_idx;
  int32_t wday;
  int32_t tmp;
  int32_t a0_adj;
  uint8_t flags;

  a0_adj = a0;
  if (a0 < 0) {
    a0_adj = a0 + 7;
  }
  day_idx = a0_adj >> 3;
  wday = a0 - day_idx * 8;
  date_str = (uint8_t *)guest_at(g_PTR_DAT_801b5b94[day_idx]);
  *(int16_t *)(s1 + 0x12) = (int16_t)a0;
  mc_state_init();
  tmp = cd_stamp_to_sector(date_str);
  tmp = tmp + cd_stamp_to_sector((const void *)guest_at(
      0x801b3310u + (uint32_t)*(uint16_t *)(s1 + 0x12) * 4)) * 2;
  *(uint32_t *)(s1 + 0x28) = (uint32_t)tmp;
  s1[0] = 1;
  s1[1] = (uint8_t)wday;
  do {
    if (br2_bail_pending()) return;
    tmp = cd_cmd_issue_sync(0xd,(int)guest_of(s1),0);
  } while (tmp == 0);
  if ((int32_t)a1 >= 0) {
    mc_dialog_bit4((int32_t)a1);
  }
  /* disasm: +0x16/+0x14/+0x10 are sb — 0x80157764/0x80157768/0x80157794;
   * +0x12 is sh — 0x801576E4 `A6240012`; +0x12 read is lhu — 0x80157704
   * `86230012`, so the index read must be unsigned. */
  flags = s1[0xe];
  s1[0x16] = 0;
  s1[0x14] = 0;
  s1[0xf] = 0;
  s1[0xd] = 0x10;
  s1[0xe] = (uint8_t)((flags & 0xf0) | 2);
  tmp = cd_stamp_to_sector(date_str);
  *(uint32_t *)(s1 + 0x24) = (uint32_t)tmp;
  *(uint32_t *)(s1 + 0x20) = (uint32_t)tmp;
  *(uint32_t *)(s1 + 0x18) = 0;
  s1[0x10] = 0x7f;
}

/* slot_display_pos — save-slot display-position helper. Gates on the
 * scratch flags at guest 0x1f8000dc ((v>>4)&3 != 0 and bit 0x40 set),
 * then a1 selects the fixed-point divisor: 0 -> lh22*3 scaled
 * >>2, 1 -> lh22*7 scaled >>3 (both mod-adjusted toward zero), with
 * arg4 = ((int16)a0[0x20] / 2) - 0x45 and arg6 = 4 handed to
 * hud_text_blk_alt(a0=+8 ptr, a1=+0xc ptr, a2=guest 0x80121e3c table).
 * Returns -0x63 (-99) on gate/selector failure. */
int slot_display_pos(uint8_t *a0,uint32_t a1)
{
  uint32_t flags = SB32(0x1f8000dc);
  int32_t y;
  int32_t x_raw;
  int32_t x;

  if (((flags >> 4) & 3) == 0) {
    return -0x63;
  }
  if ((flags & 0x40) == 0) {
    return -0x63;
  }
  if (a1 == 0) {
    y = *(int16_t *)(a0 + 0x22) * 3;
    if (y < 0) {
      y = y + 3;
    }
    x_raw = (int32_t)(int16_t)*(uint16_t *)(a0 + 0x20);
    x = (int16_t)((x_raw / 2) - 0x45);
    hud_text_blk_alt(*(uint32_t *)(a0 + 8),*(uint32_t *)(a0 + 0xc),
                (const void *)guest_at(0x80121e3c),x,
                (int32_t)((uint32_t)y << 14) >> 16,4);
    return 0;
  }
  if (a1 == 1) {
    y = *(int16_t *)(a0 + 0x22) * 7;
    if (y < 0) {
      y = y + 7;
    }
    x_raw = (int32_t)(int16_t)*(uint16_t *)(a0 + 0x20);
    x = (int16_t)((x_raw / 2) - 0x45);
    hud_text_blk_alt(*(uint32_t *)(a0 + 8),*(uint32_t *)(a0 + 0xc),
                (const void *)guest_at(0x80121e3c),x,
                (int32_t)((uint32_t)y << 13) >> 16,4);
    return -0x63;   /* disasm 0x80163618: v0=-99 on all paths */
  }
  return -0x63;
}

/* mc_dialog_bit4 — memcard dialog flag @0x801bef6e bit4 setter.
 * p<0 -> read-only return; p==0 -> clear 0x10; p>0 -> set 0x10.
 * Disasm-verified @0x801574b4. */
int32_t mc_dialog_bit4(int32_t a0)
{
  uint8_t *f = (uint8_t *)guest_at(0x801bef60);
  if (a0 < 0) return f[0xe];
  f[0xe] = (a0 == 0) ? (uint8_t)(f[0xe] & 0xef)
                        : (uint8_t)(f[0xe] | 0x10);
  return f[0xe];
}

/* blk_io_retry — memcard/block IO retry: poll mc_date_serial_cmp up to 2000x;
 * on handle, toggle s1+8 index, cd_bitstream_decode(h, *(s1+idx*4),
 * *(u32*)0x801f4758) then cd_dir_entry_probe(h); returns handle or -1.
 * Disasm 0x80159a14-0x80159aac. */
extern int32_t mc_date_serial_cmp(uint8_t *,void *);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t br2_call_clean(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t call_guest4(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void cd_bitstream_decode(int32_t,uint32_t,uint32_t);
extern void cd_dir_entry_probe(int32_t);
int32_t blk_io_retry(uint32_t a0,void *a1)
{
  uint8_t *s1 = (uint8_t *)guest_at(a0);
  int32_t handle, tries_left = 0x7d0;
  uint32_t sel;
  if (s1 == NULL) return -1;
  for (;;) {
    handle = mc_date_serial_cmp(s1,a1);
    if (handle != 0) {
      sel = (*(uint32_t *)(s1 + 8) < 1) ? 1 : 0;
      *(uint32_t *)(s1 + 8) = sel;
      cd_bitstream_decode(handle,*(uint32_t *)(s1 + sel * 4),
                   *(uint32_t *)guest_at(0x801f4758));
      cd_dir_entry_probe(handle);
      return handle;
    }
    if (--tries_left == 0) return -1;
  }
}

/* mc_date_serial_cmp — memcard entry compare/update: cd_channel_poll poll
 * (2000 tries), compares serial @+8, date u16s @+0x10/+0x12 against
 * globals @0x801b3f68/6c; on change rebuilds date fields on stack
 * record and stores scaled halves into s2 @+0x1c/1e/24/26/32.
 * Disasm 0x80159ab4-0x80159c1c. */
extern int32_t cd_channel_poll(uint32_t *,uint32_t *);
/* History: this body was delegated to generated (0x80159ab4) because the
 * cd_channel_poll wait needs per-iteration guest-cycle advance + IRQ
 * delivery — now supplied by br2_irq_poll inside the retry loop (the
 * proven cd_timer_read contract), so the clean port below is faithful.
 * Disasm 0x80159ab4-0x80159c1c. */
/* mc_date_serial_cmp — Faithful clean port of 0x80159ab4-0x80159c1c.
 * The cd_channel_poll wait is a device-deadline poll: br2_irq_poll per
 * retry advances guest cycles + delivers pending IRQs (the proven
 * cd_timer_read/cd_busy_wait contract), so the CD deferred coroutine can
 * mark the channel entry state==2 exactly like the generated per-block
 * psx_check_interrupts_at did. The two RTC out-params live on a real
 * 0x30 guest frame (br2_guest_frame) because cd_channel_poll writes guest
 * addresses through them. a0/a1 arrive as host pointers into
 * guest RAM (blk_io_retry passes guest_at()/table ptrs) — dereferenced
 * directly per file convention; the channel-entry pointer read back from
 * the frame is a GUEST address and goes through br2_ldN/br2_stN helpers. */
int32_t mc_date_serial_cmp(uint8_t *a0,void *a1)
{
  extern int32_t cd_channel_poll(uint32_t *,uint32_t *);
  extern void br2_irq_poll(void);
  extern int br2_bail_pending(void);
  extern uint16_t br2_ld16(uint32_t a);
  extern uint32_t br2_ld32(uint32_t a);
  extern void     br2_st16(uint32_t a, uint16_t v);
  extern void     br2_st32(uint32_t a, uint32_t v);
  uint32_t fr = br2_guest_frame(0x30);
  uint8_t *s1 = a0;
  uint8_t *s2 = (uint8_t *)a1;
  uint32_t tries = 2000, buf_p, entry_p, yr_val, aux;

  /* @0x80159AD8 loop: poll up to 2000x; rc==0 -> compare, exhausted -> 0.
   * Guest decrements the counter in the branch delay slot (always), but
   * it is dead on the success path — observable behavior identical. */
  for (;;) {
    int32_t rc = cd_channel_poll((uint32_t *)guest_at(fr + 24),
                                 (uint32_t *)guest_at(fr + 28));
    if (rc == 0) break;
    if (--tries == 0) { br2_guest_frame_free(); return 0; }
    if (br2_bail_pending()) { br2_guest_frame_free(); return 0; }
    br2_irq_poll();
  }

  entry_p = br2_ld32(fr + 28);              /* P = channel entry (guest addr) */
  /* @0x80159AF8: if lw(P+8) >= lw(s1+8) -> *0x801F4740 = 1 (@0x80159B14) */
  if (br2_ld32(entry_p + 8) >= *(uint32_t *)(s1 + 8))
    br2_st32(0x801f4740, 1);

  /* @0x80159B24/@0x80159B38: mismatch on either date half -> @0x80159B4C
   * writeback; both match -> skip straight to @0x80159BB8 tail. */
  if (br2_ld32(0x801b3f68) != (uint32_t)br2_ld16(entry_p + 0x10) ||
      br2_ld32(0x801b3f6c) != (uint32_t)br2_ld16(entry_p + 0x12)) {
    uint32_t x, t;
    br2_st16(fr + 16, 0);                          /* dead frame stores */
    br2_st16(fr + 18, 0);
    if (*(uint32_t *)(s1 + 0) == 0) {
      aux = (uint32_t)*(int16_t *)(s1 + 0x14);     /* lh — low16 stored */
    } else {
      x = *(uint32_t *)(s1 + 0x14);                /* @0x80159B64: 3x/2 */
      t = x * 3u;
      aux = (t + (t >> 31)) >> 1;                  /* srl pair, exact */
    }
    br2_st16(fr + 20, (uint16_t)aux);
    br2_st16(fr + 22, (uint16_t)(*(uint32_t *)(s1 + 0x18) << 1));
    /* @0x80159BA8/@0x80159BB4: publish the entry's date halves */
    br2_st32(0x801b3f68, (uint32_t)br2_ld16(entry_p + 0x10));
    br2_st32(0x801b3f6c, (uint32_t)br2_ld16(entry_p + 0x12));
  }

  /* @0x80159BB8 tail (both paths converge): year-ish value into s2
   * +0x1c/+0x24, month-ish global into s2 +0x1e/+0x26/+0x32. */
  if (*(uint32_t *)(s1 + 0) == 0) {
    yr_val = (uint32_t)br2_ld16(0x801b3f68);         /* @0x80159BE8: lhu */
  } else {
    uint32_t x = br2_ld32(0x801b3f68);            /* @0x80159BC8: 3x/2 */
    uint32_t t = x * 3u;
    yr_val = (t + (t >> 31)) >> 1;
  }
  aux = (uint32_t)br2_ld16(0x801b3f6c);
  buf_p = br2_ld32(fr + 24);                        /* v0 = *sp+24 = buf addr */
  *(uint16_t *)(s2 + 0x24) = (uint16_t)yr_val;
  *(uint16_t *)(s2 + 0x1c) = (uint16_t)yr_val;
  *(uint16_t *)(s2 + 0x26) = (uint16_t)aux;
  *(uint16_t *)(s2 + 0x1e) = (uint16_t)aux;
  *(uint16_t *)(s2 + 0x32) = (uint16_t)aux;
  br2_guest_frame_free();
  return (int32_t)buf_p;
}

/* rec_find_lt_f8 — first 0x10-stride rec where f8==0 or f8>=a1;
 * -1 if none. a0 host ptr, a1 u32, a2 count. Disasm 0x80163b08-40:
 * slt v0,f8,a1 then beq -> return i, i.e. match on !(f8<a1). */
int32_t rec_find_lt_f8(uint8_t *a0,uint32_t a1,int32_t a2)
{
  int32_t i;
  if (a2 <= 0) return -1;
  for (i = 0; i < a2; i++, a0 += 0x10) {
    uint32_t f8 = *(uint32_t *)(a0 + 8);
    if (f8 == 0 || f8 >= a1) return i;
  }
  return -1;
}

/* rec_find_ge_e — first 0x10-stride rec where !(a1 < s16(+0xe));
 * -1 if none. Disasm 0x80163b84-0x80163bb8. */
int32_t rec_find_ge_e(uint8_t *a0,int32_t a1,int32_t a2)
{
  int32_t i;
  if (a2 <= 0) return -1;
  for (i = 0; i < a2; i++, a0 += 0x10) {
    if (!((int32_t)*(int16_t *)(a0 + 0xe) > a1)) return i;
  }
  return -1;
}

/* rec_u32_store — copy 4 bytes a2 -> a0 + a1*16. Disasm verified. */
void rec_u32_store(uint8_t *a0,int32_t a1,const uint8_t *a2)
{
  uint8_t *d = a0 + (a1 << 4);
  d[0] = a2[0]; d[1] = a2[1]; d[2] = a2[2]; d[3] = a2[3];
}

/* save_blk_cmp32 — 32-byte compare: 0x801c11e0 vs *0x801b43f0;
 * returns 1 on first diff, 0 if equal. Disasm 0x80167ae8-30. */
int save_blk_cmp32(void)
{
  uint8_t *a = (uint8_t *)guest_at(0x801c11e0);
  uint8_t *b = (uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b43f0));
  uint32_t i;
  for (i = 0; i < 0x20; i++) {
    if (a[i] != b[i]) return 1;
  }
  return 0;
}

/* fighter_rec_fetch_apply — fighter_record_build(a0) -> idx table @0x801f36d0 + a1*4,
 * then cd_file_read_seq(ret, a2, *slot). Disasm 0x80156650-98. */
extern uint32_t fighter_record_build(uint32_t,uint32_t,uint32_t);
extern uint32_t cd_file_read_seq(uint32_t,uint32_t,uint32_t);
void fighter_rec_fetch_apply(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t r = fighter_record_build(a0,a1,a2);
  uint32_t slot = *(uint32_t *)guest_at(0x801f36d0 + (a1 << 2));
  cd_file_read_seq(r,a2,slot);
}

/* mc_state_init — memcard state init @0x801bef60: brightness_packet_send(0),
 * poll cd_cmd_issue_sync(9,0,0) till 0, then +0xd=0, +0xf=0xff,
 * +0x20 = +0x24. Disasm 0x801573ec-0x8015743c. */
extern void brightness_packet_send(int32_t);
extern int32_t cd_cmd_issue_sync(int32_t,int32_t,int32_t);
void mc_state_init(void)
{
  extern int br2_bail_pending(void);
  uint8_t *s0 = (uint8_t *)guest_at(0x801bef60);
  brightness_packet_send(0);
  while (cd_cmd_issue_sync(9,0,0) == 0) {
    if (br2_bail_pending()) return;
  }
  if (s0 == NULL) return;
  s0[0xd] = 0;
  s0[0xf] = 0xff;
  *(uint32_t *)(s0 + 0x20) = *(uint32_t *)(s0 + 0x24);
}

/* mc_mode_nibble_rw — memcard mode nibble: a0<0 -> return s0[0xe]&0xff;
 * else mc_state_init() then s0[0xe] = (s0[0xe]&0xf0)|a0, ret val.
 * Disasm 0x80157458-0x801574ac. */
uint32_t mc_mode_nibble_rw(int32_t a0)
{
  uint8_t *s0 = (uint8_t *)guest_at(0x801bef60);
  uint32_t val;
  if (s0 == NULL) return 0;
  if (a0 < 0) return s0[0xe] & 0xff;
  mc_state_init();
  val = (s0[0xe] & 0xf0) | (uint32_t)a0;
  s0[0xe] = (uint8_t)val;
  return val & 0xff;
}

/* se_value_select — gauge/se value select: gauge_scale_fixed(0,a0),
 * deferred_cmd_post(0x405,0,4), a1 = table[a0<8] {0,4fff,0,0,4fff,3fff,
 * 5fff,3fff}, blk_1f26c0_init_c(2,a1). Disasm 0x80155edc-0x80155f5c. */
extern void deferred_cmd_post(int32_t,int32_t,int32_t);
extern void blk_1f26c0_init_c(int32_t,uint32_t);
void se_value_select(uint32_t a0)
{
  static const uint16_t tab[8] = {0,0x4fff,0,0,0x4fff,0x3fff,0x5fff,0x3fff};
  uint32_t a1 = 0;
  gauge_scale_fixed(0,(short)a0);
  deferred_cmd_post(0x405,0,4);
  if (a0 < 8) a1 = tab[a0];
  blk_1f26c0_init_c(2,a1);
}

/* mode3_dispatch — mode dispatch on *(u32*)a1: 0->632e4, 1->63314,
 * 2->63320, else -> move v0,zero. Disasm 0x8016325c-0x801632dc.
 * Returns v0 like the MIPS original:
 *  - v1==0 -> block_copy_ret_len length
 *  - v1==1 -> jal 0x80163314 = `break 1031; jr ra`; break is a no-op in the
 *    recompiler and v0 (=1, the slti result) survives -> returns 1
 *  - v1==2 -> lzss_decode length
 *  - default -> 0x801632D0 `move v0,zero` -> returns 0
 * (An earlier port returned 2 for the default and halted via lzss_trap on
 * mode-1; both diverge from generated and inflate heap-A / desync records.) */
extern uint32_t block_copy_ret_len(uint32_t,uint32_t);
extern uint32_t lzss_decode(uint32_t,uint32_t);
uint32_t mode3_dispatch(uint32_t a0,uint32_t a1)
{
  uint32_t mode = *(uint32_t *)guest_at(a1);
  (void)a0;
  decomp_trace_record(0,a0,a1,(uint8_t)mode,0);
  if (mode == 0) return block_copy_ret_len(a0,a1);
  if (mode == 1) return 1;
  if (mode == 2) return lzss_decode(a0,a1);
  return 0;
}

/* slot_status_fill — 24-slot status byte fill: for i<24, rec =
 * *0x801bdeb0 + i*16, bit i of *0x801bde3c selects mapping of
 * lhu(rec+0xc): bit? (f?1:3) : (f?2:0). Disasm verified. */
void slot_status_fill(uint8_t *a0)
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

/* mc_action_dispatch — memcard action dispatch (JT 0x80120b74):
 * 0 -> if *0x801f36c2: datetime_confirm_a(a1,1); 1 -> brightness_packet_send(0x30);
 * 2 -> brightness_packet_send(0x7f); 3/4 -> mc_state_init(); 5 -> brightness_packet_send(a1).
 * Always returns 1. Disasm verified. */
int32_t mc_action_dispatch(uint32_t a0,uint32_t a1)
{
  if (a0 >= 6) return 1;
  switch (a0) {
  case 0:
    if (*(uint16_t *)guest_at(0x801f36c2) != 0) datetime_confirm_a(a1,1);
    break;
  case 1: brightness_packet_send(0x30); break;
  case 2: brightness_packet_send(0x7f); break;
  case 3: case 4: mc_state_init(); break;
  case 5: brightness_packet_send(a1); break;
  }
  return 1;
}

/* mc_action_dispatch_b — same dispatch via datetime_confirm_b (JT 0x80120b8c). */
int32_t mc_action_dispatch_b(uint32_t a0,uint32_t a1)
{
  if (a0 >= 6) return 1;
  switch (a0) {
  case 0:
    if (*(uint16_t *)guest_at(0x801f36c2) != 0) datetime_confirm_b(a1,1);
    break;
  case 1: brightness_packet_send(0x30); break;
  case 2: brightness_packet_send(0x7f); break;
  case 3: case 4: mc_state_init(); break;
  case 5: brightness_packet_send(a1); break;
  }
  return 1;
}

/* fighter_spawn_tail — fighter spawn tail: 2b49c(0), 525c4(s1),
 * 5a584(scratch pair, s1, s1[0xc0]), 51328(pair, s1), 6fbfc(),
 * if !a1 record_copy_1a(s8(s1+0x98)); s1[0xa0]=0xff.
 * Disasm 0x80166398-0x80166424. */
extern void fighter_link_init(uint32_t,uint32_t,uint8_t *,uint32_t);
extern void scratchpad_pair_init(void);
extern void flow_cfg_init(uint8_t);
extern void rec_timer_pair_clear(intptr_t);
extern void battle_state_init(uint8_t *,uint8_t *,uint32_t);
extern uint8_t *g_DAT_1f8000e8,*g_DAT_1f8001f0;
extern uint32_t guest_of(const void *);
extern void record_copy_1a(int);
void fighter_spawn_tail(uint8_t *s1,uint32_t s0)
{
  flow_cfg_init(0);
  rec_timer_pair_clear((intptr_t)guest_of(s1));
  fighter_link_init(0x1f8000e8,0x1f8001f0,(uint8_t *)s1,*(uint32_t *)(s1 + 0xc0));
  battle_state_init(g_DAT_1f8000e8,g_DAT_1f8001f0,guest_of(s1));
  scratchpad_pair_init();
  if (s0 == 0) record_copy_1a((int8_t)s1[0x98]);
  *(uint16_t *)(s1 + 0xa0) = 0xff;
}

/* cd_file_read_seq — CD file read seq: 9d4d0(0), 9d470(a1),
 * 9d410(a0,a2), poll 9d500(0) till nonzero; returns a2.
 * Disasm 0x801565ec-0x80156648. */
extern void cd_mode_store(int32_t);
extern void cd_status_shifted_read(uint32_t);
extern void cd_seek_param_issue_clamped(uint32_t,uint32_t);
extern int32_t cd_seek_complete_poll(int32_t);
extern void br2_irq_poll(void);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
/* cd_file_read_seq — delegate the whole body to the generated MIPS
 * (0x801565ec). Its seek-completion wait loop (0x80156628->0x80156630)
 * crosses a basic-block boundary every iteration, so psx_check_interrupts_at
 * runs per pass with a real guest resume PC. That is what lets the CD
 * deferred-interrupt coroutine (cd_ctx_restore -> cd_chan_full_init ->
 * cd_intc_dispatch) resume and set the 0xf0000009 event flag TestEvent
 * polls. A clean-C wait cannot supply a dispatchable guest resume PC for
 * the deferred switch, so br2_irq_poll alone starves the coroutine and
 * TestEvent spins forever — the same reason cd_init_session/cd_cmd_issue_*
 * were delegated. br2_call_generated also makes gen_clean_table skip
 * 0x801565ec so generated callers dispatch straight to the generated body
 * (no clean_table wrap -> thunk -> generated recursion). */
uint32_t cd_file_read_seq(uint32_t a0,uint32_t a1,uint32_t a2)
{
  /* BISECT: temporarily re-delegated to isolate menu regression — the
   * seek-completion wait loop needs live-gpr coroutine resume. */
  return br2_call_clean(0x801565ec, a0, a1, a2, 0);
}

/* fighter_link_init — fighter link init: a2[0x94]=-1, a3 guest ptr,
 * +0xde=0x4b on both scratch blocks, +0x54 = (a3+0x19/0x1a)&3,
 * +0x86=0x10, +0x96=3, 5ab88(0,s0) + 5ab88(1,s0).
 * Disasm 0x8015a584-0x8015a610. */
extern void u16_tbl_copy(uint32_t,uint32_t);
void fighter_link_init(uint32_t a0,uint32_t a1,uint8_t *a2,uint32_t a3)
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
  u16_tbl_copy(0,s0);
  u16_tbl_copy(1,s0);
}

/* block_copy_ret_len — s0=*(u32*)(a1+8); guest_memcpy(a0,a1+0xc,s0);
 * returns s0. Disasm 0x801632e4-0x8016330c. */
extern uint32_t guest_memcpy(uint32_t,uint32_t,uint32_t);
uint32_t block_copy_ret_len(uint32_t a0,uint32_t a1)
{
  uint8_t *b = (uint8_t *)guest_at(a1);
  uint32_t s0;
  if (b == NULL) return 0;
  s0 = *(uint32_t *)(b + 8);
  decomp_trace_record(1,a0,a1,0,s0);
  /* guest_memcpy takes guest addresses; b+0xc is host, so pass the
   * guest address a1+0xc directly (b = guest_at(a1)). */
  guest_memcpy(a0,a1 + 0xc,s0);
  return s0;
}

extern int32_t rec_sorted_lookup(uint8_t *,uint32_t,int32_t);
extern int32_t mc_op_result_poll(uint32_t);
extern int32_t mc_op_wait_stable(uint32_t);
extern int32_t mc_chan_job_stable_poll(uint32_t,const void *,const void *,const void *,uint32_t,uint32_t);

/* hud_name_fixup — name fixup: sanitize first 3 bytes (0x3c/0x7f -> 0x20),
 * then scan string-pointer table @0x801b4250 backwards (a2 from 0x15,
 * t0 -=4 per miss) for a 3-char prefix match; on match write "HUD",
 * return 1; exhausted -> 0. Disasm 0x80163d7c-0x80163e34. */
int hud_name_fixup(char *t1)
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

/* name_rec_insert — name-record insert: tab = 16-byte records, idx a1,
 * count = stack arg. Disasm 0x80163bf4-0x80163cb4: idx out of range
 * (a1 >= count || a1 < 0) returns immediately — branches at 0x80163C04
 * (beq on slt==0) and 0x80163C0C (blt) both target block_80163CB4, the
 * bare jr-$ra epilogue with NO stores; the +0xc/+0xe tail u16s are only
 * written on the in-range fill path. Else shift records [idx..count-2]
 * up one and fill slot: name[0..3], src[3]->+4, u32 src+0xf0->+8,
 * u16 src+0xe0->+0xc, u16 src+0xe6->+0xe. */
void name_rec_insert(uint8_t *a0,int32_t a1,uint8_t *a2,const void *a3_,int32_t count)
{
  uint8_t *t0 = a0 + a1 * 16;
  const uint8_t *a3 = (const uint8_t *)a3_;
  int32_t i;
  if (a1 >= count || a1 < 0) {
    return;
  }
  for (i = count - 2; i >= a1; i--)
    memcpy(a0 + (i + 1) * 16,a0 + i * 16,16);
  t0[0] = a3[0]; t0[1] = a3[1]; t0[2] = a3[2]; t0[3] = a3[3];
  t0[4] = a2[3];
  *(uint32_t *)(t0 + 8) = *(uint32_t *)(a2 + 0xf0);
  *(uint16_t *)(t0 + 0xc) = *(uint16_t *)(a2 + 0xe0);
  *(uint16_t *)(t0 + 0xe) = *(uint16_t *)(a2 + 0xe6);
}

/* name_rec_gate — name-table update gate: skip when byte @0x1f80009c
 * in [2,6); else rec_sorted_lookup lookup on table @0x801c1304 keyed by
 * src+0xe0, then name_rec_insert insert (str arg or default @0x80121fa4).
 * Disasm 0x80163cbc-0x80163d78. */
void name_rec_gate(uint8_t *s1,const void *a1_)
{
  int8_t v1 = *(int8_t *)guest_at(0x1f80009c);
  uint8_t *s0 = (uint8_t *)guest_at(0x801c1304);
  const uint8_t *s2 = (const uint8_t *)a1_;
  int32_t r;
  if (v1 >= 2 && v1 < 6) return;
  r = rec_sorted_lookup(s0,*(uint16_t *)(s1 + 0xe0),0xa);
  if (*(uint8_t *)s2 != 0) {
    if (r >= 0) name_rec_insert(s0,r,s1,s2,0xa);
  } else {
    if (r >= 0) name_rec_insert(s0,r,s1,(uint8_t *)guest_at(0x80121fa4),0xa);
  }
}

/* mc_op_dispatch — memcard op dispatch: s0 = -(a0!=0), s1 = s0&0x10,
 * r = mc_op_result_poll(s1); r==3||4 -> slot @0x801b43e8+(s0&4) = 3;
 * r==0||4 -> return mc_op_wait_stable(s1); else return r.
 * Disasm 0x80167b34-0x80167bd4. */
int mc_op_dispatch(uint32_t a0)
{
  int32_t s0 = -(a0 != 0);
  uint32_t s1 = (uint32_t)s0 & 0x10;
  int32_t r = mc_op_result_poll(s1);
  if (r == 3 || r == 4)
    *(uint32_t *)guest_at(0x801b43e8 + ((uint32_t)s0 & 4)) = 3;
  if (r == 0 || r == 4) return mc_op_wait_stable(s1);
  return r;
}

/* mc_op_dispatch_b — memcard op dispatch variant: r==3||0 -> slot select
 * @0x801b43e8+((a0!=0)?4:0), mc_chan_job_stable_poll probe; rc!=0 -> rc,
 * out2[0]==0 -> 0, else (*slot==3 ? 0xf : 0). r==4 -> slot=3 ret 4.
 * Disasm 0x80167bd8-0x80167cdc. */
int mc_op_dispatch_b(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t s4,uint32_t s5,uint32_t s6)
{
  int32_t s1 = -(a0 != 0);
  uint32_t fl = (uint32_t)s1 & 0x10;
  int32_t r = mc_op_result_poll(fl);
  if (r == 3 || r == 0) {
    uint32_t *slot = (uint32_t *)guest_at(0x801b43e8 + ((a0 != 0) ? 4 : 0));
    /* in/out1/out2 cross the BIOS boundary inside cd_chan_job_pump —
     * they must be guest RAM, so reserve a real guest stack frame
     * (mirrors the guest caller's sp+0x10/+0x20/+0x40 locals). */
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t fr = br2_guest_frame(0x60);
    uint32_t *in = (uint32_t *)guest_at(fr + 0x10);
    uint32_t *out1 = (uint32_t *)guest_at(fr + 0x20);
    uint32_t *out2 = (uint32_t *)guest_at(fr + 0x40);
    uint32_t st1 = 0, st2 = 1;
    in[0] = a1; in[1] = a2; in[2] = a3;
    memset(out1, 0, 0x20);
    memset(out2, 0, 0x20);
    int32_t rc = mc_chan_job_stable_poll(fl,in,out1,out2,st1,st2);
    if (rc != 0) { br2_guest_frame_free(); return rc; }
    if (out2[0] == 0) { br2_guest_frame_free(); return 0; }
    {
      int32_t ret = *slot == 3 ? 0xf : 0;
      br2_guest_frame_free();
      return ret;
    }
  }
  if (r == 4) {
    *(uint32_t *)guest_at(0x801b43e8 + ((uint32_t)s1 & 4)) = 3;
    return 4;
  }
  if (r >= 1 && r <= 2) return r;
  return r;
}

/* ret_zero_a/63898/638a0 — disabled save-slot helpers: guest is a
 * bare `jr ra; move v0,zero` (return 0), args ignored.
 * Disasm 0x80163850-0x801638a4. */
int32_t ret_zero_a(uint32_t a0) { (void)a0; return 0; }
int32_t ret_zero_b(uint32_t a0) { (void)a0; return 0; }
int32_t ret_zero_c(uint32_t a0) { (void)a0; return 0; }

/* rec_sorted_lookup — sorted-table lookup: scan a2 16-byte records for first
 * with key a1 < sh(rec+0xc); return index or -1.
 * Disasm 0x80163b48-0x80163b7c. */
int32_t rec_sorted_lookup(uint8_t *a0,uint32_t a1,int32_t a2)
{
  int32_t i;
  if (a2 <= 0) return -1;
  /* guest 0x80163b48: lh s16(+0xc); slt v0,a1,f; beq -> ret i, i.e.
   * match on a1 >= f (first record NOT greater than key). */
  for (i = 0; i < a2; i++, a0 += 0x10)
    if ((int32_t)a1 >= (int32_t)(int16_t)*(int16_t *)(a0 + 0xc)) return i;
  return -1;
}

/* u16_tbl_copy — copy 8 u16 from a1 -> 0x801f4780 + a0*16.
 * Disasm 0x8015ab88-0x8015abbc. */
void u16_tbl_copy(uint32_t a0,uint32_t a1)
{
  uint16_t *d = (uint16_t *)guest_at(0x801f4780 + a0 * 16);
  const uint16_t *s = (const uint16_t *)guest_at(a1);
  int i;
  for (i = 0; i < 8; i++) d[i] = s[i];
}

/* rec_slot_scan — scan 0x20 six-byte records @0x801f3600: when
 * (h & 0xf00) == 0x400 clear the u16 and decrement @0x801f35f4.
 * (Guest tests 0x200/<0x201/0x300/!=0x400 all collapse to ==0x400.)
 * Disasm 0x80156220-0x80156288. */
void rec_slot_scan(void)
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

/* rec_clear_irq — clear flag bytes over records @0x801f24d0 stride 0x10
 * for idx in [a0,a1): rec[0]!=0 -> rec[0]=0, irqmask|=rec[8], rec[1]=0.
 * Then cd_reg_window_u16(0,mask), @0x801f2650=mask, +4=3, +8/+0xa=0,
 * cd_chan_attr_push(@0x801f2650). Disasm 0x80156344-0x80156400. */
extern void FUN_8018e300(void);
extern void FUN_8018e310(void);
extern void cd_reg_window_u16(int32_t,uint32_t);
extern void cd_chan_attr_push(uint32_t);
void rec_clear_irq(uint32_t a0,uint32_t a1)
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
  cd_reg_window_u16(0,s1);
  *(uint32_t *)guest_at(0x801f2650) = s1;
  *(uint32_t *)guest_at(0x801f2654) = 3;
  *(uint16_t *)guest_at(0x801f265a) = 0;
  cd_chan_attr_push(0x801f2650);
  *(uint16_t *)guest_at(0x801f2658) = 0;
  FUN_8018e310();
}

/* fighter_record_build — fighter record builder (a0=guest rec, a1=slot idx,
 * a2=guest base). rec_slot_scan(); rec_clear_irq(6,0x18); blk_1f26c0_init().
 * Writes idx table @0x801f36d0, 64-entry ptr table @0x801f26e0, and
 * t2<=0x40 packed 8-byte records @0x801f2be0+idx*0x200. Returns
 * guest addr rec + ((u16@rec+0x12)<<9)+0xa20. Disasm 0x80156484-0x801565e8. */
extern void blk_1f26c0_init(void);
uint32_t fighter_record_build(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint32_t s1 = a1, acc = a1, s3 = a2;
  uint32_t t3, t2, base, i;
  uint8_t *p, *t1, *a2p;
  if (s0 == NULL) return 0;
  rec_slot_scan();
  rec_clear_irq(6,0x18);
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

/* lzss_decode — LZSS stream decoder. src[8..11] returned verbatim;
 * bitstream at src+0xc, 8 bits/byte refill when window hits 0x8000.
 * bit=1 literal; bit=0 back-ref: short form (bits 3-4 len-2, offset
 * byte -256..-1) or long form ((b0<<8|b1)>>3-0x2000 offset, len=(b1&7)+2
 * or count-byte+1; count byte 0 terminates -> return src[8]).
 * Disasm 0x80163320-0x80163478. */
uint32_t lzss_decode(uint32_t dst,uint32_t src)
{
  uint8_t *sd = (uint8_t *)guest_at(src);
  decomp_trace_record(2,dst,src,2,0);
  uint32_t t1 = *(const uint32_t *)(sd + 8);
  const uint8_t *ip = sd + 0xc;
  uint8_t *op = (uint8_t *)guest_at(dst);
  uint8_t *op0 = op;
  uint32_t runaway_logged = 0;
  uint32_t a2 = 0x8000;
  for (;;) {
    /* DIAGNOSTIC: flag runaway decode (garbage src -> unbounded *op++) so the
     * guilty src is captured before it sprays the 0x5555 poison pattern. */
    if (!runaway_logged && (uint32_t)(op - op0) > 0x100000u) {
      decomp_trace_record(3,dst,src,2,(uint32_t)(op - op0));
      runaway_logged = 1;
    }
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

void lzss_trap(uint32_t a0,uint32_t a1)
{
  /* Mode-1 (break) should be unreachable on real data; reaching it means the
   * compressed asset header is corrupt (bad CD/file delivery upstream). Halt
   * with state instead of a bare trap so the freeze dump + decomp_trace ring
   * capture the failing dst/src for diagnosis. */
  extern void psx_fatal_halt(const char *reason);
  char msg[96];
  snprintf(msg, sizeof(msg), "lzss mode1 break a0=%08x a1=%08x", a0, a1);
  psx_fatal_halt(msg);
}

/* ================= appended ports (psx_payload disasm) ================= */

/* callee externs needed by the appended ports */
extern void battle_struct_init(uint32_t,uint32_t);
extern void perm_table_apply(uint32_t);
extern int reset_sprite_ot_block(uint32_t);
extern void task_table_init(uint32_t);
extern void record_copy_1a(int);
extern void per_side_setup(int32_t,int32_t);
extern void rec_copy_init(int32_t,uint32_t,uint32_t);
extern int clear_f4ee8_u16s(uint32_t);
extern int reset_combat_slots(void);
extern void clear_f0300_records(void);
extern void fighter_record_apply(uint32_t,int32_t);
extern void page_table_ptr_set(uint32_t,int32_t);
/* real signature lives in menu.c: (guest heap ptr, host int* tag table,
 * tag) -> consumed length. The table argument is a GUEST address at these
 * call sites, so wrap it in guest_at() — passing the raw guest address as
 * int* made *a1 dereference host memory ~0x80xxxxxx (wild read). */
extern uint32_t stream_block_decode(uint32_t,int *,int);
extern void cd_timer_read(uint32_t);
extern uint32_t action_enter(void *battle, uint8_t *ctrl, uint32_t sel);
/* strong action_enter_fwd: forwards to action_enter (action.c).
   Overrides the weak stub in bind.c. */
int action_enter_fwd(void *battle, uint8_t *ctrl, uint32_t sel)
{
  return (int)action_enter(battle,ctrl,sel);
}
extern void fighter_anim_table_init(int32_t);
extern void sprite_anim_chain_build(uint32_t);
extern int select_phase_gate(uint32_t,uint32_t);
extern int display_cur_invalidate();
extern int display_pair_invalidate();
extern int display_page_commit_a(int32_t);
extern int save_channel_init_commit(uint32_t,int32_t,uint8_t *,uint8_t *);
extern int save_load_state_machine(uint32_t,int32_t);
extern int save_state_machine_alt(uint32_t,uint8_t *,int32_t);
extern void FUN_80195290(int32_t,int32_t);
extern void gte_cw_d000_set(int32_t);
extern int mc_file_cmd_stable_poll(uint32_t,const void *,uint32_t);
extern int mc_job_a_result_poll(uint32_t,const void *,void *,uint32_t,uint32_t);
extern int mc_job_b_result_poll(uint32_t,const void *,void *,uint32_t,uint32_t);
extern int mc_op_seq_probe(uint32_t,const void *,void *,uint32_t,uint32_t);
/* pointer args may arrive as guest addrs or host ptrs; normalise */
static void *mem_ptr(uint32_t v)
{
  if (v >= 0x1f800000 && v < 0x80300000) return guest_at(v);
  return (void *)(uintptr_t)v;
}

/* batch_mass17 forward decls (defs appended at end of file) */
extern int mc_file_cmd(uint32_t,const void *,uint32_t);
extern int mc_job_issue_a(uint32_t,const void *,void *,uint32_t,uint32_t);
extern int mc_job_issue_b(uint32_t,const void *,void *,uint32_t,uint32_t);
extern void data_relocate(uint32_t);
extern void cd_state_clear(void);
extern void slot_field_clear(uint32_t);
extern void elem_pair_flag100(void *,int32_t);
extern void elem_pair_flag8(void *,int32_t);
extern void elem_pair_field_set(void *,int32_t,int32_t,int32_t);
extern void elem_flag100(void *,int32_t);
extern void elem_flag8(void *,int32_t);
extern void elem_field_set(void *,int32_t,int32_t,int32_t);
extern uint32_t mat3x3_build(int32_t,int32_t,int32_t,uint32_t);
extern int32_t dev_byte_xfer(uint8_t *,int32_t);
extern void cd_file_header_copy(uint32_t,void *);
extern void cd_strcat_guard(void *,uint32_t);
extern void cd_job_slot_alloc(uint32_t,uint32_t);
extern int32_t FUN_801a0220(uint32_t,uint32_t,uint32_t);
extern void FUN_801a0250(int32_t);
extern uint32_t cd_xchg_fec24(uint32_t);
extern int32_t fifo_space_check(void);
extern int tween_seq_alloc(int,uint16_t,uint32_t,uint32_t,short,uint16_t,uint32_t,uint32_t);
extern int *node_lazy_attach(int,int,int,int);
extern int cd_file_load_sm(int32_t,uint32_t);
extern int sprite_quad_emit(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t spr_slot_pkt_submit();
extern void sprite_9slice_build(uint32_t,uint32_t,uint32_t,uint32_t);
extern void spr_attr_alloc_a(uint32_t,int16_t,int16_t,int16_t,int16_t,uint32_t,int32_t);
extern void spr_attr_alloc_b(uint32_t,uint32_t,uint32_t,uint32_t,int32_t,int32_t);
extern void spr_draw_pair(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void hud_sprite_desc_init(void);
extern int rgb_triple_store(uint32_t,uint32_t,uint32_t);
extern int scratch_013_set(uint32_t);
extern int spr_draw_off(int32_t,int32_t,int32_t,int32_t);

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

/* ---- deferred_cmd_exec @0x801568fc (216 insns) ---- */
int deferred_cmd_exec(uint32_t a0,int32_t a1)
{
  uint8_t *rec;
  uint8_t *slotp;
  uint8_t *rb = NULL;
  uint8_t key;
  int i;

  if (a0 >= 6) return 1;
  switch (a0) {
  case 3:
    rec_slot_scan();
    rec_clear_irq(6,0x18);
    return 1;
  case 5: {
    uint8_t *b = (uint8_t *)guest_at(g32(0x1f8000c0));
    int16_t x = b ? (int16_t)b[0xe] : 0;
    int16_t y = b ? (int16_t)b[0xf] : 0;
    gauge_scale_fixed(x,y);
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

/* ---- ret_one @0x80156d88 (1 insn: jr ra; li v0,1) ---- */
int ret_one(void)
{
  return 1;
}

/* ---- confirm_code_select @0x8015787c (96 insns) ---- */
void confirm_code_select(uint32_t a0,uint32_t a1)
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
    mc_confirm_dispatch(a1,1);
    return;
  }
  mc_confirm_dispatch(code,1);
}

/* ---- select_phase_update @0x8015a1e4 (231 insns) ----
   s4 = guest ptr; s3 = 0x1f8000e8, s2 = 0x1f8001f0 (guest ptrs);
   s4+0xa8/+0xac hold guest pointers 0x801f47a0/0x801f4770. */
void select_phase_update(uint32_t a0)
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

  s0 = select_phase_gate(a0,0);
  v0 = select_phase_gate(a0,1);
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
      int32_t r = table_byte_next_a();
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

/* ---- match_setup_apply @0x8015a69c (299 insns) ---- */
/* ---- match_setup_apply @0x8015a69c (299 insns) ----
   a0/a1/a2/a3 are guest pointers; s5 = *(u32*)(a2+0xc4) is also a guest
   pointer. Jump tables read from payload:
     0x80120f20 -> {1000,2000,3000,4500,-1}
     0x80120f38 -> {cond,8,0xb,0xe,0x10,0x12,0x15,0x18}
     0x80120f58 -> {0x64,0x5a,0x50,0x4b,0x46,0x41,0x3c,0x37}
     0x80120f78 -> {none,none,none,bb8,none,flag} */
void match_setup_apply(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
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
  /* u16_tbl_copy(0, table-or-custom) for s0[0x17] */
  {
    uint32_t sel = s0[0x17];
    uint32_t a1v = sel ? (0x801b3f68 + sel * 16) : 0x801c15d0;
    u16_tbl_copy(0,a1v);
  }
  {
    uint32_t sel = s0[0x18];
    uint32_t a1v = sel ? (0x801b3f68 + sel * 16) : 0x801c15e0;
    u16_tbl_copy(1,a1v);
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


/* ---- menu_sprite_load_sm @0x8015abc0 (72 insns) ----
   a0 guest state, a1 sprite sel, a2 second arg, a3 third arg,
   a4 (stack arg) 0x801c47b8 selector. */
int menu_sprite_load_sm(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t a4)
{
  uint8_t *s2 = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(s2 + 0x8c);
  int32_t s1 = (int32_t)a4;

  if (s0 < 0) return -1;
  if (s0 == 0) {
    cd_file_load_sm(-1,0);
    *(uint32_t *)guest_at(0x801c43a0) = 0;
    *(uint8_t *)guest_at(0x801e3d00) = 0xff;
    if (s1 == 0) {
      *(uint32_t *)guest_at(0x801c47b8) = 0x80010000;
    } else if (s1 == 1) {
      extern uint8_t *tag_table_get(void);
      extern uint32_t guest_of(const void *p);
      *(uint32_t *)guest_at(0x801c47b8) = guest_of(tag_table_get());
    }
    s0 = 0x10;
  } else if (s0 != 0x10) {
    *(int16_t *)(s2 + 0x8c) = (int16_t)s0;
    return 0;
  }
  if (cd_file_load_sm((int32_t)a1,g32(0x801c47b8)) == 0) {
    *(int16_t *)(s2 + 0x8c) = (int16_t)s0;
    return 0;
  }
  fighter_record_apply(g32(0x801c47b8),(int32_t)a3);
  page_table_ptr_set((int32_t)a2,(int32_t)a3);
  *(int16_t *)(s2 + 0x8c) = -1;
  return 1;
}

/* ---- menu_stepper_sm @0x8015ace4 (298 insns) ----
   a0 is a HOST pointer (callers pass _g_pad_buffer).
   The 0x38-byte descriptor at sp+0x10 is built once in the init state
   and persists across loop iterations (menu_stepper_state_advance may edit it). */
int menu_stepper_sm(uint8_t *s0)
{
  extern int br2_bail_pending(void);
  uint8_t *s4 = (uint8_t *)guest_at(0x1f800000);
  /* The 0x38-byte stepper descriptor must be guest-resident: the +0xc
   * callback (0x8015b190, unported) is dispatched via
   * call_guest(cb, guest_of(desc), arg) — a host stack desc makes
   * guest_of()==0 and the generated callee reads a0=0. */
  uint8_t *desc;
  int32_t s1 = 0;
  int32_t s3 = 0;

  desc = (uint8_t *)guest_at(br2_guest_frame(0x40));
  memset(desc,0,0x38);
  for (;;) {
    if (br2_bail_pending()) { br2_guest_frame_free(); return s3; }
    if (s1 == 1) goto body;
    if (s1 < 2) {
      if (s1 == 0) {
        uint16_t f;
        uint8_t *bp;
        if (frame_divisor_get() != 0) goto poll;
        print_mode_dispatch_b(2,0,0);
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
      deferred_cmd_post(0x400,0xd,0);
      s1 = 0xf0;
      s3 = 1;
    }
    if (*(uint16_t *)(s4 + 0x7c) & 0xe0) {
      deferred_cmd_post(0x400,0xc,0);
      s1 = 0xf0;
      s3 = 0;
    }
    menu_stepper_state_advance(desc);
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
    /* guest a0 = 0x1f800000 (immediate) — frame_tick re-resolves via
     * guest_at(); pass the guest address, not the host scratch ptr. */
    frame_tick(0x1f800000);
check:
    if (s1 != 0xff) continue;
    break;
  }
  br2_guest_frame_free();
  return s3;
}

/* ---- dialog_sel_dispatch @0x80160598 (188 insns) ----
   a0 = guest dialog-state ptr, a1 = selector (u16). */
extern void hud_text_b();
extern int u16_counter_step_pad_a();
extern void banner_stripe_scroll();
int dialog_sel_dispatch(intptr_t a0,uint32_t a1)
{
  /* a0 is a guest address when reached via clean_table, but the menu
   * stepper's fn_lookup leg passes the descriptor as a host pointer
   * into guest RAM — br2_gaddr normalises both to the guest address. */
  extern uint32_t br2_gaddr(const void *);
  a0 = (intptr_t)br2_gaddr((const void *)a0);
  uint8_t *s3 = (uint8_t *)guest_at((uint32_t)a0);
  uint8_t *s2 = (uint8_t *)guest_at(0x1f800000);
  int32_t s0 = 0;
  int32_t s1;
  uint32_t ctx8 = *(uint32_t *)(s2 + 8);
  uint32_t ctxc = *(uint32_t *)(s2 + 0xc);
  int32_t v0;

  scroll_offset_set(0x20,0xffffffff);
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
    scroll_offset_set(0xffffffff,0x18);
    announce_param_block_init(ctx8,ctxc,(const void *)guest_at(0x80121360),0x1000,0x1000,0);
    inrange = ((uint32_t)(s3[3] - 0x12) < 3);
    banner_stripe_scroll(4,0,inrange ? 0 : 4,7);
    scroll_offset_accumulate(0,0x10);
    return 0;
  }
  if (s1 < 0x101) {
    if (s1 == 2 || s1 == 3) {
      if ((*(uint16_t *)(s2 + 0x7c) & 0xe0) != 0) {
        deferred_cmd_post(0x400,0xc,0);
        s3[3] = 0x10;
      }
      return 0;
    }
    if (s1 == 1) {
      if (u16_counter_step_pad_a(ctx8,1,1) == 0) return 0;
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
    announce_param_block_init(ctx8,ctxc,(const void *)guest_at(0x80121b6c),0x1000,0x1000,s0);
    str = g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s3 + 0x14)) * 4);
    announce_param_block_init(ctx8,ctxc,(const void *)guest_at(str),0x160,0x1000,s0);
    scroll_offset_accumulate(0,0xc);
    return 0;
  }
  if (s1 == 0x102 || s1 == 0x103 || s1 == 0x104) {
    uint32_t str;
    if (s1 == 0x102)      str = 0x80121b78;
    else if (s1 == 0x103) str = 0x80121b98;
    else                  str = 0x80121318;
    announce_param_block_init(ctx8,ctxc,(const void *)guest_at(str),0x1000,0x1000,s0);
    scroll_offset_accumulate(0,0xc);
    return 0;
  }
  if (s1 == 0x1ff) return 0;
  return 1;
}

/* ---- card_sprite_load_sm @0x80165908 (234 insns) ----
   a0 = guest state ptr, a1 = selector s2. */
int card_sprite_load_sm(uint32_t a0,uint32_t a1)
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
    cd_file_load_sm(-1,0);
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
  if (cd_file_load_sm(s0,v3cfc) == 0) {
    *(int16_t *)(s4 + 0x8c) = (int16_t)s1;
    return 0;
  }
  s5 = g8(0x1f800013);
  scratch_013_set(0);
  v47b8 = g32(0x801c47b8);
  v3cfc = g32(0x801e3cfc);
  if (s2 < 0xf) {
    if (s2 >= 4) {
      /* four-sprite block */
      stream_block_decode(v47b8,(int *)guest_at(v3cfc),0);
      spr_draw_pair(v47b8,0,0x1f0,1,0x200,0,1);
      stream_block_decode(v47b8,(int *)guest_at(v3cfc),1);
      spr_draw_pair(v47b8,0,0x1f1,1,0x280,0,1);
      stream_block_decode(v47b8,(int *)guest_at(v3cfc),2);
      spr_draw_pair(v47b8,0,0x1f2,1,0x240,0x100,1);
      stream_block_decode(v47b8,(int *)guest_at(v3cfc),3);
      spr_draw_pair(v47b8,0,0x1f3,1,0x2c0,0x100,1);
    } else if (s2 == 3) {
      stream_block_decode(v47b8,(int *)guest_at(v3cfc),0);
      spr_draw_pair(v47b8,0,0x1f4,1,0x300,0,1);
      stream_block_decode(v47b8,(int *)guest_at(v3cfc),1);
      spr_draw_off((int32_t)v47b8,0x280,0,1);
    } else {
      goto single;
    }
  } else if (s2 == 0xf) {
    stream_block_decode(v47b8,(int *)guest_at(v3cfc),0);
    spr_draw_pair(v47b8,0,0x1f4,1,0x300,0,1);
    stream_block_decode(v47b8,(int *)guest_at(v3cfc),1);
    spr_draw_off((int32_t)v47b8,0x340,0,1);
  } else {
single:
    stream_block_decode(v47b8,(int *)guest_at(v3cfc),0);
    spr_draw_pair(v47b8,0,0x1f4,1,0x300,0,1);
  }
  cd_timer_read(0);
  scratch_013_set(s5);
  *(int16_t *)(s4 + 0x8c) = -1;
  return 1;
}

/* ---- card_fade_sm @0x801660c0 (181 insns) ----
   s1/s2 are callee-saved: they persist across calls (static here).
   Entry clears 0x1f800001 and 0x1f80008c; exit clears 0x1f800001. */
void card_fade_sm(uint32_t a0)
{
  extern int br2_bail_pending(void);
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
    if (br2_bail_pending()) return;
    int32_t st = s0[1];
    if (st == 1) goto tick;
    if (st < 2) {
      if (st == 0) {
        if (card_sprite_load_sm(0x1f800000,(uint32_t)(s4 + 4)) != 0 &&
            frame_divisor_get() == 0) {
          spr_attr_alloc_a(*(uint32_t *)(s0 + 8),0,0,
                        (int16_t)*(uint16_t *)(s0 + 0x20),
                        (int16_t)*(uint16_t *)(s0 + 0x22),
                        0x31ce,3);
          s1 = 0x12c;
          s0[1] = (uint8_t)(s0[1] + 1);
          goto tick;
        }
      }
    } else if (st == 0xf0) {
      deferred_cmd_post(0x204,0,0);
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
      spr_attr_alloc_a(*(uint32_t *)(s0 + 8),0,0,
                    (int16_t)*(uint16_t *)(s0 + 0x20),
                    (int16_t)*(uint16_t *)(s0 + 0x22),
                    (uint32_t)(v1 * 0x10101),3);
    } else if (s1 >= 0xfa && s1 < 0x12c) {
      v1 = (int32_t)((int32_t)s2 / 50);
      spr_attr_alloc_a(*(uint32_t *)(s0 + 8),0,0,
                    (int16_t)*(uint16_t *)(s0 + 0x20),
                    (int16_t)*(uint16_t *)(s0 + 0x22),
                    (uint32_t)(v1 * 0x10101),3);
    }
    spr_slot_pkt_submit(*(uint32_t *)(s0 + 8) + 4,0,0xefff0088,0x7c000000);
    spr_slot_pkt_submit(*(uint32_t *)(s0 + 8) + 4,0x100,0xefff008a,0x7c400000);
    spr_slot_pkt_submit(*(uint32_t *)(s0 + 8) + 4,0xf00000,0xefff0099,0x7c800000);
    spr_slot_pkt_submit(*(uint32_t *)(s0 + 8) + 4,0xf00100,0xefff009b,0x7cc00000);
    goto poll;
poll:
    /* guest a0 = 0x1f800000 (immediate) — pass the guest address. */
    frame_tick(0x1f800000);
check:
    if (s0[1] != 0xff) continue;
    break;
  }
  s0[1] = 0;
  (void)v1;
}

/* ---- battle_round_init @0x8016642c (69 insns) ---- */
void battle_round_init(uint32_t a0,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  int8_t s2 = (int8_t)s0[0x99];
  uint32_t w;

  w = *(uint32_t *)(s0 + 0xbc);
  s0[0x9d] = 0;
  s0[0x9e] = 0;
  s0[0x99] = 0;
  battle_struct_init(w,a0);
  perm_table_apply(table_byte_next_a());
  reset_sprite_ot_block(a0);
  task_table_init(0);
  record_copy_1a((int32_t)(int8_t)s0[0x98]);
  per_side_setup((int32_t)(int8_t)s0[0x98],(int32_t)(int8_t)s0[0x99]);
  rec_copy_init((int32_t)(int8_t)s0[0x98],0x1f8000e8,0x1f8001f0);
  clear_f4ee8_u16s(0x1f8000e8);
  clear_f4ee8_u16s(0x1f8001f0);
  reset_combat_slots();
  action_enter_fwd((void *)(uintptr_t)a0,(uint8_t *)guest_at(0x1f8000e8),a1);
  action_enter_fwd((void *)(uintptr_t)a0,(uint8_t *)guest_at(0x1f8001f0),a1);
  fighter_anim_table_init((int32_t)(int8_t)s0[0x99]);
  sprite_anim_chain_build(a0);
  clear_f0300_records();
  battle_struct_init(*(uint32_t *)(s0 + 0xbc),a0);
  s0[0x99] = (uint8_t)s2;
}

/* ---- round_banner_draw @0x801670c8 (117 insns) ---- */
void round_banner_draw(uint32_t a0,uint32_t a1)
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
    /* sprite path: two sprite_quad_emit draws then hud_text_blk_alt */
    q = (v0 < 0) ? v0 + 0x1f : v0;
    q >>= 5;
    a1v = ((v0 - q) - 0x68) & 0xffff;
    a1v |= 0x1a000000;
    sprite_quad_emit(*(uint32_t *)(s0 + 8),a1v,0x1e7f000f,0x7adfe000,
                  0x1000,0x2000,0);
    q = (v0 < 0) ? v0 + 0x1f : v0;
    q >>= 5;
    a1v = ((v0 - q) - 0x14) & 0xffff;
    a1v |= 0x19800000;
    sprite_quad_emit(*(uint32_t *)(s0 + 8),a1v,0x2627000f,0x7adfd880,
                  0x1000,0x2000,0);
    a3v = (int32_t)*(int16_t *)(s0 + 0x20);
    if (a3v < 0) a3v += 0xf;
    hud_text_blk_alt(*(uint32_t *)(s0 + 8),*(uint32_t *)(s0 + 0xc),
                  (const void *)guest_at(0x801220e4),
                  a3v >> 4,0x20,s3);
    return;
  }
  /* plain path: two sprite_9slice_build draws then hud_text_blk_alt */
  q = (v0 < 0) ? v0 + 0x1f : v0;
  q >>= 5;
  a1v = ((v0 - q) - 0x68) & 0xffff;
  a1v |= 0x00d00000;
  sprite_9slice_build(*(uint32_t *)(s0 + 8),a1v,0x1e7f000f,0x7adfe000);
  q = (v0 < 0) ? v0 + 0x1f : v0;
  q >>= 5;
  a1v = ((v0 - q) - 0x14) & 0xffff;
  a1v |= 0x00cc0000;
  sprite_9slice_build(*(uint32_t *)(s0 + 8),a1v,0x2627000f,0x7adfd880);
  a3v = (int32_t)*(int16_t *)(s0 + 0x20);
  if (a3v < 0) a3v += 0xf;
  hud_text_blk_alt(*(uint32_t *)(s0 + 8),*(uint32_t *)(s0 + 0xc),
                (const void *)guest_at(0x801220e4),
                a3v >> 4,0x10,s3);
}

/* ---- mc_read_queue @0x80167ce4 (37 insns, 13-arg ABI) ---- */
int mc_read_queue(uint32_t p0,uint32_t p1,uint32_t p2,uint32_t p3,
                 uint32_t p4,uint32_t p5,uint32_t p6,uint32_t p7,
                 uint32_t p8,uint32_t p9,uint32_t p10,uint32_t p11,
                 uint32_t p12)
{
  uint32_t slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
  *(uint32_t *)guest_at(slot) = 0;
  return mc_data_write(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12);
}

/* ---- mc_data_write @0x80167d7c (199 insns, 13-arg ABI) ----
   p0 = port flag (slot = 0x801b43e8 + (p0?4:0)), p1..p3 = in-header,
   p7 = arg for mc_file_cmd_stable_poll, p8/p9 = a3 select for data stage,
   p11 = data ptr, p12 = data size. */
int mc_data_write(uint32_t p0,uint32_t p1,uint32_t p2,uint32_t p3,
                 uint32_t p4,uint32_t p5,uint32_t p6,uint32_t p7,
                 uint32_t p8,uint32_t p9,uint32_t p10,uint32_t p11,
                 uint32_t p12)
{
  /* Guest 0x80167d7c keeps these buffers in its stack frame (in@sp+0xf4,
   * buf18/bc0/b40 in the 0xf0-byte frame). They cross guest-visible
   * boundaries — mc_issue stores guest_of(a2) as the DMA target,
   * cd_strcat_guard/cd_arg_ptr resolve a1, and mc_op_seq_probe forwards
   * guest addresses to memcard_block_verify — so they must be guest RAM,
   * not host stack. br2_guest_frame reserves real guest stack below $sp
   * (same pattern as mc_op_dispatch_b/mc_file_cmd/cd_chan_job_pump).
   * Layout: in@+0x00 (0x20: 3 header words + NUL pad for the strcat use),
   * buf18@+0x20 (0x28), bc0@+0x50 (0x30), b40@+0x80 (0x1080 — the
   * memcard_block_verify compare reads a4*0x20 = 0x1000 bytes for the
   * 0x80-count call, matching the guest over-read into stack). */
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  uint32_t fr = br2_guest_frame(0x1100);
  uint32_t *in = (uint32_t *)guest_at(fr + 0x00);
  uint8_t *buf18 = (uint8_t *)guest_at(fr + 0x20);
  uint8_t *bc0 = (uint8_t *)guest_at(fr + 0x50);
  uint8_t *b40 = (uint8_t *)guest_at(fr + 0x80);
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
  in[3] = 0; in[4] = 0; in[5] = 0; in[6] = 0; in[7] = 0;

poll:
  ret = mc_op_result_poll((uint32_t)s2);
  if (ret == 3) {
    slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
    *(uint32_t *)guest_at(slot) = (uint32_t)ret;
    br2_guest_frame_free();
    return ret;
  }
  if ((uint32_t)ret < 4) {
    if (ret == 0) goto probe;
    br2_guest_frame_free();
    return ret;
  }
  if (ret != 4) { br2_guest_frame_free(); return ret; }
  slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
  *(uint32_t *)guest_at(slot) = 3;
  br2_guest_frame_free();
  return 4;

probe:
  ret = mc_chan_job_stable_poll((uint32_t)s2,in,buf18,bc0,0,1);
  if (ret != 0) { br2_guest_frame_free(); return ret; }
  st = *(uint32_t *)bc0;
  if (st != 0) {
    /* header already present: verify slot state then decompress */
    slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
    if (*(uint32_t *)guest_at(slot) == 3) { br2_guest_frame_free(); return 0xf; }
    ret = mc_job_a_result_poll((uint32_t)s2,in,b40,0x200,
                       *(uint32_t *)guest_at(slot));
    if (ret != 0) { br2_guest_frame_free(); return ret; }
    s4 = (*(uint8_t *)b40 < 1) ? 1 : 0;
    goto stage2;
  }
  ret = mc_file_cmd_stable_poll((uint32_t)s2,in,p7);
  if (ret == 4) goto poll;
  if ((uint32_t)ret < 5) {
    if (ret != 0) { br2_guest_frame_free(); return ret; }
    memset(b40,0,0x80);
    s4 = 0;
  } else if (ret == 6) {
    memset(b40,0,0x80);
    s4 = 0;
  } else {
    br2_guest_frame_free();
    return ret;
  }

stage2:
  asz = ((uint32_t)(p12 + 0x7f)) & ~0x7fu;
  sel = (s4 != 0) ? p9 : p8;
  {
    void *dp = mem_ptr(p10);
    void *dd = mem_ptr(p11);
    ret = mc_job_b_result_poll((uint32_t)s2,in,dp,0,0x200);
    if (ret != 0) { br2_guest_frame_free(); return ret; }
    ret = mc_op_seq_probe((uint32_t)s2,in,dp,0,0x200);
    if (ret != 0) { br2_guest_frame_free(); return ret; }
    ret = mc_job_b_result_poll((uint32_t)s2,in,dd,sel,asz);
    if (ret != 0) { br2_guest_frame_free(); return ret; }
    ret = mc_op_seq_probe((uint32_t)s2,in,dd,sel,asz);
    if (ret != 0) { br2_guest_frame_free(); return ret; }
    dat = (uint8_t *)dd;
    t0 = 0;
    if ((int32_t)p12 > 0) {
      for (i = 0; i < p12; i++) t0 ^= dat[i];
    }
    b40[0] = (uint8_t)s4;
    b40[(uint32_t)s4 + 1] = t0;
    ret = mc_job_b_result_poll((uint32_t)s2,in,b40,0x200,0x80);
    if (ret != 0) { br2_guest_frame_free(); return ret; }
    ret = mc_op_seq_probe((uint32_t)s2,in,b40,0x200,0x80);
    if (ret != 0) { br2_guest_frame_free(); return ret; }
  }
  slot = 0x801b43e8 + ((p0 != 0) ? 4 : 0);
  *(uint32_t *)guest_at(slot) = 0;
  br2_guest_frame_free();
  return 0;
}

/* ---- mc_data_read @0x8016809c (119 insns, 13-arg ABI) ----
   Read-back counterpart of mc_data_write: p0 flag, p1..p3 header,
   p8/p9 select, p11 dest, p12 size. */
int mc_data_read(uint32_t p0,uint32_t p1,uint32_t p2,uint32_t p3,
                 uint32_t p4,uint32_t p5,uint32_t p6,uint32_t p7,
                 uint32_t p8,uint32_t p9,uint32_t p10,uint32_t p11,
                 uint32_t p12)
{
  /* guest frame: fp+0x44.. = {a1,a2,a3,p4..p12} = 12 words, indexed
     by buf[0] up to 255 -> keep a wide zero-padded view.
     Both buffers must be guest RAM: `in` is strcat'd through
     cd_arg_ptr and `buf` becomes the card-DMA target via
     mc_issue's s1[8]=guest_of(a2) store — a host malloc/stack buffer
     would store guest addr 0. br2_guest_frame reserves real guest
     stack below $sp (same pattern as mc_data_write/mc_file_cmd).
     Layout: in@+0x00 (0x470 slot), buf@+0x488 so buf-8 lands at
     +0x480 inside the frame (the guest sp+0x10 outgoing-arg slot). */
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  uint32_t *in;
  uint8_t *buf;
  size_t asz;
  uint32_t fr;
  int32_t s7 = (int32_t)p0;
  int32_t s4;
  int32_t ret;
  uint32_t slot;
  uint32_t s0 = p12;
  uint32_t s3 = p11;
  uint8_t s6, s5;
  uint32_t i;

  if (s0 == 0) return 0;
  s4 = (s7 != 0) ? -1 : 0;
  s4 &= 0x10;

  ret = mc_op_result_poll((uint32_t)s4);
  if (ret == 3 || ret == 0) {
    /* ok */
  } else {
    slot = 0x801b43e8 + (s4 & 4);
    *(uint32_t *)guest_at(slot) = 3;
    return ret;
  }

  asz = ((uint32_t)(s0 + 0x7f)) & ~0x7fu;
  fr = br2_guest_frame(0x488 + (uint32_t)asz + 0x20);
  in = (uint32_t *)guest_at(fr + 0x00);
  buf = (uint8_t *)guest_at(fr + 0x488);

  memset(in,0,0x470);
  in[0]=p1; in[1]=p2; in[2]=p3; in[3]=p4; in[4]=p5; in[5]=p6; in[6]=p7;
  in[7]=p8; in[8]=p9; in[9]=p10; in[10]=p11; in[11]=p12;

  *(uint32_t *)(buf - 8) = 0x80; /* sp+0x10 outgoing arg */
  ret = mc_job_a_result_poll((uint32_t)s4,in,buf,0x200,0x80);
  if (ret != 0) { br2_guest_frame_free(); return ret; }

  s6 = buf[0];
  s5 = buf[(uint8_t)s6 + 1];
  {
    uint32_t idx = (uint8_t)s6;
    for (;;) {
      uint32_t a3v = in[7 + idx];
      uint32_t sum = 0;
      uint32_t s1new;
      *(uint32_t *)(buf - 8) = asz;
      ret = mc_job_a_result_poll((uint32_t)s4,in,buf,a3v,asz);
      if (ret != 0) { br2_guest_frame_free(); return ret; }
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
        br2_guest_frame_free();
        return 0;
      }
      idx = s1new;
      if (s6 == s1new) { br2_guest_frame_free(); return 2; }
    }
  }
}

/* ---- countdown_display_sm @0x8016ddf8 (264 insns) ---- */
void countdown_display_sm(uint32_t a0)
{
  extern int br2_bail_pending(void);
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
  hud_sprite_desc_init();
  rgb_triple_store(0,0,0);
  t1 = (uint8_t *)guest_at(0x1f800000 + (uint32_t)s0 * 2);
  for (;;) {
    if (br2_bail_pending()) return;
    int32_t st;
    if (s5 == 1) goto body;
    if (s5 < 2) {
      if (s5 == 0) {
        if (frame_divisor_get() != 0) goto poll;
        display_cur_invalidate();
        display_pair_invalidate();
        *(int16_t *)(s2 + 0x8c) = 0;
        s4 = 0;
        s3 = s4;
        s5 = 1;
        goto body;
      }
      goto poll;
    }
    if (s5 == 0xf0) {
      deferred_cmd_post(0x203,0,0);
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
      if (display_page_commit_a(idx) != 0) {
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
    spr_attr_alloc_a(*(uint32_t *)(s2 + 8),0,0,
                  (int16_t)*(uint16_t *)(s2 + 0x20),
                  (int16_t)*(uint16_t *)(s2 + 0x22),
                  (uint32_t)(v1 * 0x10101),3);
icon:
    if ((int32_t)fp > 0) {
      spr_attr_alloc_b(*(uint32_t *)(s2 + 8) + 4,0,0xeffe0088,0x7fc00000,
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
          announce_param_block_init(*(uint32_t *)(s2 + 8),*(uint32_t *)(s2 + 0xc),
                        (const void *)guest_at(0x801222a4),
                        (uint32_t)(int32_t)(int16_t)yv,
                        (uint32_t)xs,0);
        }
        {
          uint16_t m = *(uint16_t *)(t1 + 0x68);
          if (m & 0x800) {
            s5 = 0xf0;
          } else if (m & 0xe0) {
            deferred_cmd_post(0x400,0xc,0);
            s6 = (int16_t)(s6 + 1);
            s3 = (s6 == 0x5c) ? 0x64 : 0x32;
            *(int16_t *)(s2 + 0x8c) = 0;
          }
        }
      }
    }
poll:
    /* guest a0 = s2 = 0x1f800000 — pass the guest address, not host ptr. */
    frame_tick(0x1f800000);
    if (s5 != 0xff) continue;
    break;
  }
  (void)s1;
}

/* ---- menu_res_load_sm @0x801709fc (206 insns) ---- */
int32_t menu_res_load_sm(uint32_t a0,uint8_t *s4,uint8_t *s5,int32_t s2)
{
  extern int br2_bail_pending(void);
  extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
  uint8_t *s1 = (uint8_t *)guest_at(a0);
  int32_t s3 = 0;
  int32_t s0;
  int32_t v0;

  v0 = (int32_t)*(int16_t *)(s1 + 0x8c);
  /* DIAG: record every menu_res_load_sm entry (a0, s4 guest, s5 guest, s2,
   * incoming s1+0x8c state) so versus-vs-arcade load coverage is provable. */
  decomp_trace_record(61,a0,s4?guest_of(s4):0,(uint8_t)s2,(uint32_t)v0);
  decomp_trace_record(62,a0,s5?guest_of(s5):0,(uint8_t)s2,(uint32_t)v0);
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
    if (br2_bail_pending()) return -1;
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
    cd_file_load_sm(-1,0);
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
    if (save_channel_init_commit(a0,(int32_t)(int8_t)s1[0x98],s4,s5) == 0) {
      goto store0;
    }
    s0 = 0x20;
    *(uint32_t *)guest_at(0x801e3cfc) = 0x800f0000;
    *(uint32_t *)guest_at(0x801c43a0) = g32(0x801c47b8);
    *(uint16_t *)(s1 + 0x8e) = 0;
    /* fall into st20 */

st20:
    if (save_load_state_machine(a0,(int32_t)(int8_t)s1[0x98]) == 0) {
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
    if (save_state_machine_alt(a0,s4,s3) == 0) {
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
    if (save_state_machine_alt(a0,s5,s3) == 0) {
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
      gte_cw_d000_set((int32_t)*(int16_t *)(s1 + 0x24));
    }
    return 1;

store0:
    *(int16_t *)(s1 + 0x8c) = (int16_t)s0;
    return 0;

dispatch:
    continue;
  }
}


/* ---- fighter_anim_table_init @0x80172a34 (877 insns) ----
   Fighter/menu animation-table init. a0!=0 -> immediate return.
   Builds records in two arena buffers (*(u32*)0x801f5210/14) plus
   scratchpad tables at 0x1f8000e8. */
extern uint32_t task_pool_init(uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t task_pool_alloc(uint32_t,uint32_t,uint32_t);
extern void task_heads_clear(uint32_t);
extern void spr_desc_b(uint32_t,uint32_t);
extern void spr_desc_c(uint32_t,uint32_t);
extern void sprite_rect_set_b(uint32_t,int32_t,int32_t,int32_t,int32_t);
extern void sprite_rect_set_c(uint32_t,int32_t,int32_t,int32_t,int32_t);
extern void sprite_rect_set_a(uint32_t,int32_t,int32_t,int32_t,int32_t);
extern void sprite_place_centered(uint32_t,uint32_t,int32_t,int32_t,int32_t,int32_t);
extern uint32_t fighter_height_check(uint32_t);
extern uint32_t task_alloc_wrap_a(uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t task_alloc_wrap_b(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);

/* record byte-fill: d = rec+0xe style pointer; s = 4-byte src table */
static void coord_rec_fill(uint8_t *d, uint32_t s)
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
void fighter_anim_table_init(int32_t a0)
{
  uint32_t s5, s3i, s4i;
  uint8_t *rec;
  uint32_t buf1, buf2, bp;
  int32_t i, j, k;

  if (a0 != 0) return;

  task_pool_init(0,2,0x1c00,0x801e95ec);
  task_pool_init(0,3,0x400,0x801e95ec + 0x1c00);
  task_heads_clear(0);
  *(uint32_t *)guest_at(0x801f4f28) = 0;
  *(uint32_t *)guest_at(0x801f4f2c) = 0;
  *(uint32_t *)guest_at(0x801f5210) = task_pool_alloc(0,2,0x898);
  *(uint32_t *)guest_at(0x801f5214) = task_pool_alloc(0,2,0x898);

  /* ---- block A: 0x801f4f30 region, two passes s5=0,0xc0 ---- */
  for (s3i = 0; s3i < 2; s3i++) {
    s5 = s3i * 0xc0;
    /* 8 records stride 0x18 at 0x801f4f30+s5 */
    for (s4i = 0; s4i < 8; s4i++) {
      rec = (uint8_t *)guest_at(0x801f4f30 + s5 + s4i * 0x18);
      rec[3] = 5;
      rec[7] = 0x28;
      spr_desc_b(0x801f4f30 + s5 + s4i * 0x18,0);
      spr_desc_c(0x801f4f30 + s5 + s4i * 0x18,1);
    }
    /* 2 records at 0x801f4f30+s5 stride 0x18 via sprite_rect_set_a,
       src table 0x801b4cbc step 8 */
    for (s4i = 0; s4i < 2; s4i++) {
      uint32_t t = 0x801b4cbc + s4i * 8;
      rec = (uint8_t *)guest_at(0x801f4f30 + s5 + s4i * 0x18);
      rec[4] = 2; rec[5] = 2; rec[6] = 9;
      sprite_rect_set_a(0x801f4f30 + s5 + s4i * 0x18,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
    }
    /* 2 records at 0x801f4f60+s5, table 0x801b4cdc */
    for (s4i = 0; s4i < 2; s4i++) {
      uint32_t t = 0x801b4cdc + s4i * 8;
      rec = (uint8_t *)guest_at(0x801f4f60 + s5 + s4i * 0x18);
      rec[4] = 2; rec[5] = 2; rec[6] = 9;
      sprite_rect_set_a(0x801f4f60 + s5 + s4i * 0x18,
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
      spr_desc_b(a,0);
      spr_desc_c(a,1);
    }
    /* 2 records stride 0x24, sprite_rect_set_b, table 0x801b4ccc */
    for (s4i = 0; s4i < 2; s4i++) {
      uint32_t t = 0x801b4ccc + s4i * 8;
      uint32_t a = 0x801f50b0 + off + s4i * 0x24;
      rec = (uint8_t *)guest_at(a + 0x1e);
      rec[-0x1a]=0xff; rec[-0x19]=0xf0; rec[-0x18]=0x20;
      rec[-0x12]=0xff; rec[-0x11]=0xf0; rec[-0x10]=0x20;
      rec[-0xa]=0xa0;  rec[-9]=0x60;   rec[-8]=0;
      rec[-2]=2;      rec[-1]=2;      rec[0]=9;
      sprite_rect_set_b(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
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
      sprite_rect_set_b(a,(int32_t)gs16(t),
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
      spr_desc_b(bp + (uint32_t)j * 0x28,0);
      spr_desc_c(bp + (uint32_t)j * 0x28,1);
      r[4] = 0x7f; r[5] = 0x7f; r[6] = 0x7f;
    }
    /* C2: 2 records at bp+0x5c8, sprite_rect_set_c + fill from 0x801b4c98 */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c88 + j * 8;
      uint32_t a = bp + 0x5c8 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a + 0xe);
      sprite_rect_set_c(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      coord_rec_fill(rec,0x801b4c98);
      *(uint16_t *)(rec) = 0x789e;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C3: sprite_place_centered on bp, table 0x801b4b48, then fills @0x801b4b50 */
    sprite_place_centered(bp,0x1f800000,
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
    /* C4: 2 records bp+0x28 stride 0x28, sprite_place_centered, table 0x801b4b5c */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4b5c + j * 8;
      uint32_t a = bp + 0x28 + (uint32_t)j * 0x28;
      sprite_place_centered(a,0x1f800000,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      *(uint16_t *)((uint8_t *)guest_at(a) + 0x16) = 0xf;
    }
    /* C5: nested fp<2 { s4<2 { s7<8 } } — 32 records from bp+0x78,
       sprite_rect_set_c, tables 0x801b4a48 (lh, step8) and
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
            sprite_rect_set_c(a,(int32_t)gs16(t6),
                         (int32_t)gs16(t6+2),(int32_t)gs16(t6+4),
                         (int32_t)gs16(t6+6));
            coord_rec_fill(d,s1b);
            *(uint16_t *)d = 0x781e;
            *(uint16_t *)(d + 8) = 0xe;
            t6 += 8; s1b += 4; aoff += 0x28;
          }
        }
      }
    }
    /* C6: 8 records bp+0x6b8 stride 0x28, sprite_rect_set_c,
       table 0x801b4be4, lhu id from 0x801b4be0, fill 0x801b4c24 */
    {
      uint16_t id = g16(0x801b4be0);
      for (j = 0; j < 8; j++) {
        uint32_t t = 0x801b4be4 + j * 8;
        uint32_t a = bp + 0x6b8 + (uint32_t)j * 0x28;
        rec = (uint8_t *)guest_at(a + 0xe);
        sprite_rect_set_c(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
                     (int32_t)gs16(t+4),(int32_t)gs16(t+6));
        coord_rec_fill(rec,0x801b4c24);
        *(uint16_t *)rec = id;
        *(uint16_t *)(rec + 8) = 0xe;
      }
    }
    /* C7: 2 records bp+0x626 stride 0x28, fill 0x801b4ca4, id 0x78de */
    for (j = 0; j < 2; j++) {
      uint32_t a = bp + 0x626 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a);
      coord_rec_fill(rec,0x801b4ca4);
      *(uint16_t *)rec = 0x78de;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C8: 2 records bp+0x578 stride 0x28, sprite_rect_set_c,
       table 0x801b4c30, fill 0x801b4c60, id 0x791d */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c30 + j * 8;
      uint32_t a = bp + 0x578 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a + 0xe);
      sprite_rect_set_c(a,(int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      coord_rec_fill(rec,0x801b4c60);
      *(uint16_t *)rec = 0x791d;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C9: 2 records bp+0x676 stride 0x28, fill 0x801b4cb8, id 0x781e */
    for (j = 0; j < 2; j++) {
      uint32_t a = bp + 0x676 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a);
      coord_rec_fill(rec,0x801b4cb8);
      *(uint16_t *)rec = 0x781e;
      *(uint16_t *)(rec + 8) = 0xe;
    }
    /* C10: 2 records bp+0x7f8 stride 0x28, sprite_place_centered,
       table 0x801b4c64; writes id 0x18/0x79dd at bp+0x806 */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c64 + j * 8;
      uint32_t a = bp + 0x7f8 + (uint32_t)j * 0x28;
      uint8_t *w = (uint8_t *)guest_at(bp + 0x806 + (uint32_t)j * 0x28);
      sprite_place_centered(a,0x1f800000,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      *(uint16_t *)(w + 8) = 0x18;
      *(uint16_t *)w = 0x79dd;
    }
    /* C11: 2 records bp+0x848 stride 0x28, sprite_place_centered,
       table 0x801b4c74, fill 0x801b4c84, id 0x781e/0x18 */
    for (j = 0; j < 2; j++) {
      uint32_t t = 0x801b4c74 + j * 8;
      uint32_t a = bp + 0x848 + (uint32_t)j * 0x28;
      rec = (uint8_t *)guest_at(a + 0xe);
      sprite_place_centered(a,0x1f800000,
                   (int32_t)gs16(t),(int32_t)gs16(t+2),
                   (int32_t)gs16(t+4),(int32_t)gs16(t+6));
      coord_rec_fill(rec,0x801b4c84);
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
      uint32_t rr = fighter_height_check(s0);
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
      /* guest @0x801736C8-0x801736E8: lh dv,[s0+0xb0]; lui v1,0x64
       * (num = 100<<16, 16.16 fixed-point); div; mflo; sw -> 0x801f5228[i].
       * dv==0 -> LO = num>=0 ? -1 : 1 = 0xffffffff (num is positive). */
      uint32_t q = (dv == 0) ? 0xffffffffu : (uint32_t)((100 << 16) / dv);
      *(uint32_t *)guest_at(0x801f5228 + (uint32_t)i * 4) = q;
      *(uint16_t *)guest_at(0x801f524c + (uint32_t)i * 2) = 0;
    }
  }

  /* ---- block E: per-record task_alloc_wrap_a over both buffers ---- */
  for (i = 0; i < 8; i++) {
    uint32_t off = 0x6b8 + (uint32_t)i * 0x28;
    uint32_t b1 = g32(0x801f5210);
    uint32_t b2 = g32(0x801f5214);
    task_alloc_wrap_a(b1 + off + 0xe, b2 + off + 0xe,
                 0x801b4cfc, 0x801b4d08);
  }

  /* ---- block F: task_alloc_wrap_b over both buffers ---- */
  for (i = 0; i < 2; i++) {
    uint32_t off = 0x578 + (uint32_t)i * 0x28;
    uint32_t b1 = g32(0x801f5210);
    uint32_t b2 = g32(0x801f5214);
    uint32_t r = task_alloc_wrap_b(b1 + off, b2 + off, 0, 0x801b4d34, 0x801b4d28);
    *(uint32_t *)guest_at(0x801f51d0 + (uint32_t)i * 4) = r;
    *(uint32_t *)guest_at(0x801f5218 + (uint32_t)i * 4) = 0;
  }
  *(uint32_t *)guest_at(0x801f4f20) = 0;
}


/* ---- sprite_anim_chain_build @0x8016d478 (409 insns) ----
   a0 = guest ptr (byte 0x9f cleared). Two passes building sprite/
   anim descriptor chains at 0x801f4cbc.. and 0x801f4d7c.. */
extern void prim_packet_init_tex(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void gpu_texclut_pkt(uint32_t,const void *,uint32_t,uint32_t);
extern void sprite_rect_pkt_fill(uint32_t,const void *);

static void tag24_merge(uint32_t dst, uint32_t src)
{
  uint32_t v = g32(dst);
  *(uint32_t *)guest_at(dst) = (v & 0xff000000u) | (src & 0xffffffu);
}

void sprite_anim_chain_build(uint32_t a0)
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
    prim_packet_init_tex(s6,1,g8(s4t + 0x16),0x40,0);
    d18[0]=0x100; d18[1]=0x1f0; d18[2]=0x60; d18[3]=0x10;
    gpu_texclut_pkt(s3,d18,0x100,0x1e0);
    d18[0]=0x100; d18[1]=0x1e0; d18[2]=0x60; d18[3]=0x10;
    sprite_rect_pkt_fill(s1,d18);
    {
      uint8_t *r = (uint8_t *)guest_at(s0);
      r[7]=0x60; *(uint16_t *)(r+8)=0x100; *(uint16_t *)(r+0xa)=0x1e0;
      *(uint16_t *)(r+0xc)=0x60; *(uint16_t *)(r+0xe)=0x10;
      r[3]=3; r[4]=0; r[5]=0; r[6]=0; r[7]=0x62;
    }
    prim_packet_init_tex(s2,g8(s4t + 0x17),g8(s4t + 0x16),0,0);
    sprite_rect_pkt_fill(s7,(const void *)(uintptr_t)s4t);
    tag24_merge(s6,s3);
    tag24_merge(s3,s1);
    tag24_merge(s1,s0);
    tag24_merge(s0,s2);
    tag24_merge(s2,s7);
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

    prim_packet_init_tex(h1_941e0,1,g8(s4t + 0x16),0x40,0);
    d18[0]=0; d18[1]=0x1f0; d18[2]=0x20; d18[3]=0x10;
    gpu_texclut_pkt(h1_9180,d18,0,0x1e0);
    d18[0]=0; d18[1]=0x1e0; d18[2]=0x20; d18[3]=0x10;
    sprite_rect_pkt_fill(h1_91f0a,d18);
    {
      uint8_t *r = (uint8_t *)guest_at(h1_fill);
      r[7]=0x60; *(uint16_t *)(r+8)=0; *(uint16_t *)(r+0xa)=0x1e0;
      *(uint16_t *)(r+0xc)=0x20; *(uint16_t *)(r+0xe)=0x10;
      r[3]=3; r[4]=0; r[5]=0; r[6]=0; r[7]=0x62;
    }
    prim_packet_init_tex(h1_e0,g8(s4t + 0x17),g8(s4t + 0x16),0,0);
    sprite_rect_pkt_fill(h1_f08,(const void *)(uintptr_t)s4t);
    tag24_merge(h1_941e0,h1_9180);
    tag24_merge(h1_9180,h1_91f0a);
    tag24_merge(h1_91f0a,h1_fill);
    tag24_merge(h1_fill,h1_e0);
    tag24_merge(h1_e0,h1_f08);

    prim_packet_init_tex(h2_e0c,0,g8(s4t + 0x16),0x40,0);
    d18[0]=0x30; d18[1]=0x1f0; d18[2]=0x20; d18[3]=0x10;
    gpu_texclut_pkt(h2_9180,d18,0,0x1e0);
    d18[0]=0x30; d18[1]=0x1e0; d18[2]=0x20; d18[3]=0x10;
    sprite_rect_pkt_fill(h2_91f0a,d18);
    {
      uint8_t *r = (uint8_t *)guest_at(h2_fill);
      r[7]=0x60; *(uint16_t *)(r+8)=0x30; *(uint16_t *)(r+0xa)=0x1e0;
      *(uint16_t *)(r+0xc)=0x20; *(uint16_t *)(r+0xe)=0x10;
      r[3]=3; r[4]=0; r[5]=0; r[6]=0; r[7]=0x62;
    }
    prim_packet_init_tex(h2_e0,g8(s4t + 0x17),g8(s4t + 0x16),0,0);
    sprite_rect_pkt_fill(sp24,(const void *)(uintptr_t)s4t);
    tag24_merge(h2_e0c,h2_9180);
    tag24_merge(h2_9180,h2_91f0a);
    tag24_merge(h2_91f0a,h2_fill);
    tag24_merge(h2_fill,h2_e0);
    tag24_merge(h2_e0,sp24);

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


/* ---- status_text_display @0x8016f588 (413 insns) ----
   Debug/status text display. a0 = guest char/state ptr,
   a1 = selector 0..15 (jump table 0x80122590). */
extern void print_dispatch(const char *,...);
extern void scroll_offset_set(int32_t,int32_t);
extern void hud_slot_blitter(uint32_t,uint32_t);
extern void flag_table_dump(uint32_t,uint32_t);
extern void record_struct_dump(uint32_t);
extern void slot_action_dump(uint32_t);
extern void guest_noop(uint32_t);
extern void status_text_dump(uint32_t);

static int32_t mul360_div4096(int32_t v)
{ /* v*360 / 4096 trunc toward zero (MIPS mult+sra pattern) */
  int32_t t = v * 360;
  return t / 4096;
}
static int32_t mul100_div256(int32_t v)
{
  int32_t t = v * 100;
  return t / 256;
}
static int32_t sdiv16(int32_t v)
{ /* (v<0 ? v+0xf : v) >> 4  == trunc(v/16) */
  return v / 16;
}

void status_text_display(uint32_t a0,uint32_t a1)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint8_t *s2;
  uint32_t s1 = a1;
  uint32_t s3 = 0x801f04a8;
  int32_t i;

  /* s2 = lw 0x38(s0): guest ptr stored in state block */
  s2 = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x38));

  if (s0[2] == 0) {
    scroll_offset_set(0,0x20);
    if (s0[2] == 0) goto sel;
  }
  scroll_offset_set(0,0x60);
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
    hud_slot_blitter(a0,0);
    flag_table_dump(a0,0);
    flag_table_dump(a0,3);
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
    flag_table_dump(a0,0);
    flag_table_dump(a0,4);
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
  case 4: record_struct_dump(a0); return;
  case 5: slot_action_dump(a0); return;
  case 6: hud_slot_blitter(a0,1); return;
  case 7: {
    uint8_t *q = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x20));
    int32_t a1 = mul100_div256(*(int32_t *)(q + 0x34));
    int32_t a2 = mul100_div256(*(int32_t *)(q + 0x38));
    int32_t a3 = mul100_div256(*(int32_t *)(q + 0x3c));
    print_dispatch((const char *)guest_at(0x80122460),a1,a2,a3);
    q = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x20));
    a1 = mul100_div256(*(int32_t *)(q + 0x174));
    a2 = mul100_div256(*(int32_t *)(q + 0x178));
    a3 = mul100_div256(*(int32_t *)(q + 0x17c));
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
    int32_t a1 = mul360_div4096(*(int16_t *)(s0 + 0x4e));
    print_dispatch((const char *)guest_at(0x801224c0),a1);
    a1 = mul360_div4096(*(int16_t *)(s0 + 0x70));
    {
      int32_t a2 = mul360_div4096(*(int16_t *)(s0 + 0x74));
      int32_t a3 = mul100_div256(*(int16_t *)(s0 + 0x76));
      print_dispatch((const char *)guest_at(0x801224c8),a1,a2,a3);
    }
    return;
  }
  case 9:
    status_text_dump(g32(0x1f8000bc));
    return;
  case 10: {
    uint8_t *q = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x38));
    int32_t a1 = sdiv16(*(int16_t *)(q + 0xc6));
    int32_t a2 = sdiv16(*(int16_t *)(q + 0xca));
    int32_t a3 = sdiv16(*(int16_t *)(q + 0xc8));
    int32_t s10 = sdiv16(*(int16_t *)(q + 0xc4));
    int32_t s14 = sdiv16(*(int16_t *)(q + 0xc2));
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
    flag_table_dump(a0,0); flag_table_dump(a0,1); flag_table_dump(a0,2);
    flag_table_dump(a0,3); flag_table_dump(a0,4); flag_table_dump(a0,5);
    return;
  case 13: guest_noop(a0); return;
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

/* ---- hud_state_machine @0x8016e21c (1066 insns) ----
   Per-frame HUD/status-bar state machine. a0 = mode (0,1,2).
   s5 = scratchpad 0x1f800000; fp = display state. */
extern int display_page_commit_b(uint32_t,uint32_t);
extern void hud_announce(uint32_t,uint32_t,uint32_t);
extern uint32_t spr_slot_pkt_submit();

static int32_t to_s16(uint32_t v){ return (int16_t)v; }
static int32_t sdiv60(int32_t a0)
{ /* mult 0x88888889; mfhi; +a0; sra 5; - sign(a0) : exact MIPS seq */
  int64_t t = (int64_t)a0 * (int64_t)(int32_t)0x88888889;
  int32_t hi = (int32_t)(t >> 32);
  return ((hi + a0) >> 5) - (a0 >> 31);
}
static int32_t shr4_trunc(int32_t v)
{ return v < 0 ? (v + 0xf) >> 4 : v >> 4; }
static int32_t shr2_trunc(int32_t v)
{ return v < 0 ? (v + 3) >> 2 : v >> 2; }

void hud_state_machine(uint32_t a0)
{
  extern int br2_bail_pending(void);
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
    if (br2_bail_pending()) return;
    sp20 = (uint8_t *)guest_at(sp58 ? 0x1f8000e8 : 0x1f8001f0);
    sp24 = (uint8_t *)guest_at(*(uint32_t *)(sp20 + 0x2c));
    idx   = sp20[3];
    sp64  = 0x801b46d8 + idx * 25;
    sp60  = idx * 25;
    sp68  = idx * 25 + 1;
    sp6c  = idx * 25 + 2;
    hud_sprite_desc_init();
    rgb_triple_store(0,0,0);
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
    if (fp == 0xf0) { deferred_cmd_post(0x203,0,0); fp = 0xff; goto poll; }
    if (fp == 0xf1) { fp = 0xff; goto poll; }
    goto poll;

case0:
    if (frame_divisor_get() != 0) goto poll;
    display_cur_invalidate();
    display_pair_invalidate();
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
        a4 = to_s16(*(uint16_t *)(g + 0xa4));
        v0 = *(uint8_t *)guest_at(0x801b46d8 + a4 * 3 + sp6c);
        sp48 = 0;
      } else {
        a4 = to_s16(*(uint16_t *)(g + 0xa4));
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
      if (to_s16(*(uint16_t *)(g + 0x8e)) >= 0) {
        tt = to_s16(t4h);
        if (tt < 0) tt += 7;
        if (display_page_commit_b(idx, (uint32_t)(tt >> 3)) != 0) {
          v0 = (int32_t)(uint8_t)g8(0x8007193 + (sv << 12));
          v1 = (int32_t)(uint8_t)g8(0x8007113 + (sv << 12));
          t0 = (v0 >> 4) + (v1 >> 4) * 10;
          if (to_s16(*(uint16_t *)(g + 0x28)) == t0) goto hit610;
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
      if (to_s16(*(uint16_t *)(g + 0x8c)) >= 0) {
        if (display_page_commit_a(to_s16(*(uint16_t *)(g + 0x28)) - 1) != 0)
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
    int32_t t5 = to_s16(*(uint16_t *)(g + 0x40));
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
      if (sp54 == 4) vv = shr4_trunc(a0x);
      else           vv = sdiv60(a0x);
      t0 = 0xff - vv;
      s7--;
      if (s7 == -1) { sp54 = 0; *(uint16_t *)(g + 0x40) = (uint16_t)-1; }
      a1 = 0; a2 = a1;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      spr_attr_alloc_a(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)to_s16(*w20),
                   (int16_t)to_s16(*w22), (uint32_t)v0, 2);
      goto after678;
    }
d754: /* sp54 3 : 8c71c growing bar */
    {
      int32_t a2x = s7 - 1;
      int32_t y  = to_s16(*w20);
      int32_t p  = a2x * y;
      t0 = shr4_trunc(p);
      s7 = a2x;
      if (s7 == -1) { sp54 = 0; *(uint16_t *)(g + 0x40) = 0; }
      a1 = (t0 - y) & 0xffff;
      spr_attr_alloc_b(ctx8 + 4, (uint32_t)a1, 0xeffe0088, 0x7fc00000,
                   t0, to_s16(*w22));
      goto after678;
    }
d7ac: /* sp54 1,2 : fade bar, cc6c stk18=3 */
    {
      int32_t a0x = s7 * 255;
      int32_t vv;
      if (sp54 == 1) vv = shr4_trunc(a0x);
      else           vv = sdiv60(a0x);
      t0 = 0xff - vv;
      s7--;
      if (s7 == -1) { sp54 = 0; *(uint16_t *)(g + 0x40) = 0; }
      a1 = 0; a2 = 0;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      spr_attr_alloc_a(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)to_s16(*w20),
                   (int16_t)to_s16(*w22), (uint32_t)v0, 3);
      goto after678;
    }
d818: /* sp54 0 : sub-state sp50 table */
    if (sp50 < 6) {
      static const uint8_t jt_ec[6] = {0,1,1,2,3,3};
      switch (jt_ec[sp50]) {
      case 0: /* 6e9bc : plain 8c71c */
        spr_attr_alloc_b(ctx8 + 4, 0, 0xeffe0088, 0x7fc00000,
                     to_s16(*w20), to_s16(*w22));
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
      t0 = (sp50 == 1) ? shr4_trunc(v1x) : sdiv60(v1x);
      s7--;
      if (s7 == -1) sp50 = 0;
      a1 = 0; a2 = 0;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      spr_attr_alloc_a(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)to_s16(*w20),
                   (int16_t)to_s16(*w22), (uint32_t)v0, 3);
      goto after678;
    }
e8a8: /* sp50 3 : growing bar, 8c71c */
    {
      int32_t y = to_s16(*w20);
      t0 = shr4_trunc(s7 * y);
      s7--;
      if (s7 == -1) sp50 = 0;
      spr_attr_alloc_b(ctx8 + 4, (uint32_t)(t0 & 0xffff), 0xeffe0088, 0x7fc00000,
                   (uint32_t)(t0 + y), to_s16(*w22));
      goto after678;
    }
e848: /* sp50 4,5 : shrinking bar, cc6c stk18=2 */
    {
      int32_t v1x = s7 * 255;
      t0 = (sp50 == 4) ? shr4_trunc(v1x) : sdiv60(v1x);
      s7--;
      if (s7 == -1) sp50 = 0;
      a1 = 0; a2 = 0;
      v0 = (t0 | (t0 << 8) | (t0 << 16));
      spr_attr_alloc_a(ctx8, (int16_t)a1, (int16_t)a2, (int16_t)to_s16(*w20),
                   (int16_t)to_s16(*w22), (uint32_t)v0, 2);
      goto after678;
    }

negpath:
    /* 6ea00 : sp40 == 0 handled above; here sp40 < 0 dim bar */
    spr_attr_alloc_a(ctx8 + 4, 0, 0, (int16_t)to_s16(*w20),
                 (int16_t)to_s16(*w22), 0xffffff, 0);
    goto after678;
    }

after678:
    /* 6ea44 gates */
    if (to_s16(*(uint16_t *)(g + 0x8c)) >= 0) goto poll;
    if (to_s16(*(uint16_t *)(g + 0x8e)) >= 0) goto poll;
    if (sp50 != 0) goto poll;
    if (sp54 != 0) goto poll;
    /* Guest stores flag F = ((lhu(g+0x92)&0xf00)^0x100)!=0 at sp+0x58 and
     * indexes the side row as g + F*2 (0x8016EA8C: lw sp+0x58; sll 1;
     * addu 0x1f800000). Clean sp58 is the INVERSE ((x)==0x100) — correct
     * for the sp20 side select above, wrong here: row select needs F. */
    {
      uint8_t *r = g + (sp58 ^ 1) * 2;
      if ((*(uint16_t *)(r + 0x6c) & 0xf) != 0) goto poll;
    }
    /* HUD body */
    if (mode == 2) goto mode2body;
    {
      uint32_t dw = *(uint32_t *)(g + 0xdc) & 0x30;
      int32_t sp30s = to_s16(*(uint16_t *)(g + 0x30));
      if (dw != 0 || sp30s >= 0) {
        /* sprite block (6ecac/6ed1c merged — identical) */
        s6 = shr2_trunc(to_s16(*w22) * 3);
        s0v = to_s16(*w20);
        s1 = (s0v - 0x1c2) & 0xffff;
        s3 = (int32_t)*(uint16_t *)(g + 0x38);
        s2 = s3 << 5;
        spr_attr_alloc_b(ctx8,
          (uint32_t)(((s6 + 4) << 16) | s1), 0x0efe001c,
          (uint32_t)((s2 << 8) | 0x7bd6), (uint32_t)(s0v - 4),
          (uint32_t)(s6 + 0x23));
        spr_attr_alloc_b(ctx8,
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
            spr_slot_pkt_submit(ctx8, (uint32_t)(((s6 + 8) << 16) | 4),
                         0x3e37009d, (uint32_t)a3);
          }
        }
        spr_attr_alloc_a(ctx8, 0, (int16_t)s6, (int16_t)to_s16(*w20),
                     0x50, 0x80808080, 3);
      }
    }
    goto afterdraw;

mode2body:
    /* mode==2 : three text rows + bar */
    {
      int32_t yv = to_s16(*w22);
      int32_t t0v = yv * 3;
      int32_t a3v, row;
      a3v = to_s16(*w20);
      a3v = (a3v + (a3v >> 31)) >> 1;      /* /2 trunc */
      a3v -= 0x60;
      a3v = (int16_t)a3v;
      t0v = yv * 3; if (t0v < 0) t0v += 3; t0v >>= 2;
      announce_param_block_init(ctx8, ctxc, (const void *)guest_at(0x801222a8),
                   (uint32_t)(int16_t)(t0v + 0x20), 0, a3v);
      t0v = yv * 3; if (t0v < 0) t0v += 3; t0v >>= 2;
      announce_param_block_init(ctx8, ctxc, (const void *)guest_at(0x801222b4),
                   (uint32_t)(int16_t)(t0v + 0x30), sp4c, a3v);
      t0v = yv * 3; if (t0v < 0) t0v += 3; t0v >>= 2;
      announce_param_block_init(ctx8, ctxc, (const void *)guest_at(0x801222b8),
                   (uint32_t)(int16_t)(t0v + 0x70), (uint32_t)(sp4c < 1), a3v);
      a2 = yv * 3; if (a2 < 0) a2 += 3; a2 >>= 2;
      spr_attr_alloc_a(ctx8, 0, (int16_t)a2, (int16_t)to_s16(*w20),
                   0x50, 0x80808080, 3);
      (void)row;
    }

afterdraw:
    /* 6ee7c : lhu 0x68 flag checks + nibble state advance */
    {
      /* same guest flag F = sp58^1 row select as the +0x6c gate above */
      uint8_t *r = g + (sp58 ^ 1) * 2;
      uint32_t f68 = *(uint16_t *)(r + 0x68);
      if ((f68 & 0x800) == 0) {
        if ((f68 & 0xe0) == 0) goto poll;
        deferred_cmd_post(0x400,0xc,0);
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
    int32_t a0x = to_s16(*(uint16_t *)(g + 0xa4));
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
      deferred_cmd_post(0x204,0,0);
      scroll_cfg_set(2,8);
      {
        /* guest flag F = sp58^1 row select (see afterdraw gate above) */
        uint8_t *r = g + (sp58 ^ 1) * 2;
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
    g[0x98] = *(uint8_t *)guest_at(0x801f4770 + to_s16(*(uint16_t *)(g + 0xa4)));
    fp = 0xf0;
    goto poll;
}

mode1body:
    /* guest 0x8016F100-0x8016F108: li $t5,2; sw $t5,0x98($sp) sits in the
     * DELAY SLOT of the sp48==0 test, so mode is set to 2 unconditionally on
     * entry; the sp48!=0 path (0x8016F138) then overwrites it with 0.
     * mode==2 routes modechk -> mode2sel -> fp=0xf0/0xff -> return (tally
     * exit). Omitting the mode=2 store left mode==1 -> mode1body forever
     * (the row-3 mode-(1,4) tally wedge). */
    mode = 2;
    if (sp48 == 0) goto poll;
    mode = 0;
    *(uint16_t *)(g + 0x8e) = 0;
    *(uint16_t *)(g + 0x30) = (uint16_t)(*(uint8_t *)guest_at(0x801b46d8 + to_s16(*(uint16_t *)(g + 0xa4)) * 3 + sp60) << 3);
    goto poll;

mode2sel:
    if (sp4c != 0) goto mode2b;
    *(uint16_t *)(sp20 + 0xec) = (uint16_t)(*(uint16_t *)(sp20 + 0xec) + 1);
    sp24[3] = *(uint8_t *)guest_at(0x801f47a0 + to_s16(*(uint16_t *)(g + 0xa4)));
    g[0x98] = *(uint8_t *)guest_at(0x801f4770 + to_s16(*(uint16_t *)(g + 0xa4)));
    fp = 0xf0;
    goto poll;
mode2b:
    c0 = (uint8_t *)guest_at(*(uint32_t *)(g + 0xc0));
    if (c0[0xd] == 0) {
      mc_confirm_dispatch(0x27,0); s7 = 0x2ee;
    } else {
      mc_confirm_dispatch(6,0);   s7 = 0x15e;
    }
    fp = 2;
    goto poll;
    }

case2:
    hud_announce(0x1f800000,0x16,(uint32_t)s7);
    s7--;
    if (s7 != -1) {
      /* guest flag F = sp58^1 row select (see afterdraw gate above) */
      uint8_t *r = g + (sp58 ^ 1) * 2;
      if ((*(uint16_t *)(r + 0x68) & 0xe0) == 0) goto poll;
    }
    deferred_cmd_post(0x204,0,0);
    scroll_cfg_set(2,8);
    fp = 0xf0;
    g[0] = 5; g[1] = 9;
    goto poll;

poll:
    /* frame_tick re-resolves its arg via guest_at((uint32_t)a0), so it
     * must receive the GUEST address 0x1f800000 -- passing the host pointer
     * g truncated to 32 bits and made pad_state_refresh write the held/edge
     * words to a phantom arena slot, leaving 0x1f800068..0x77 stale forever
     * (the afterdraw pad-edge gate then spins in poll -> WEDGE-A). */
    frame_tick(0x1f800000);
    if (fp != 0xff) continue;
    return;
  }
}

/* ---- dialog_sel_dispatch_wide @0x8015bdc0 (613 insns) ----
   a0 = guest dialog-state ptr, a1 = selector. Sibling of
   dialog_sel_dispatch: same init, wider selector dispatch. */
extern int u16_counter_step_pad_b();
extern void round_announce_block();
extern void mc_slot_action(uint8_t *,int);

int dialog_sel_dispatch_wide(intptr_t a0,uint32_t a1)
{
  /* Same dual ABI as dialog_sel_dispatch: guest addr via clean_table,
   * host-into-guest desc pointer via the stepper fn_lookup leg.
   * a0 is reused below as a guest address (a0+0xNN counter addrs). */
  extern uint32_t br2_gaddr(const void *);
  a0 = (intptr_t)br2_gaddr((const void *)a0);
  uint8_t *s4 = (uint8_t *)guest_at((uint32_t)a0);
  uint8_t *g  = (uint8_t *)guest_at(0x1f800000);
  uint32_t ctx8 = *(uint32_t *)(g + 8);
  uint32_t ctxc = *(uint32_t *)(g + 0xc);
  uint32_t flag, sel;
  uint8_t *c0;
  int32_t v0;

  scroll_offset_set(0x20,0xffffffff);
  sel = a1 & 0xffff;
  if (sel == (uint32_t)(*(uint16_t *)(s4 + 8) + 0x100)) {
    flag = 0;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121270),0x1000,0x1000,0);
  } else {
    flag = 1;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121274),0x1000,0x1000,flag);
  }

#define BDC0_E58C(str,sz,x) \
  announce_param_block_init(ctx8,ctxc,(const void *)guest_at(str),(sz),0x1000,(x))
#define BDC0_STATUS() do { *(uint32_t *)guest_at(0x801c11f0) = 1; return 0; } while (0)

  if (sel == 0x100) {
    scroll_offset_set(0xffffffff,0x18);
    BDC0_E58C(0x80121278,0x1000,0);
    banner_stripe_scroll(9,0,4,7);
    scroll_offset_accumulate(0,0x10);
    return 0;
  }
  if (sel < 0x101) {
    switch (sel) {
    case 1:
      if (u16_counter_step_pad_a(a0 + 0x14,3,1) == 0) return 0;
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
        deferred_cmd_post(0x400,0xc,0);
        mc_slot_action(g,0);
      } else if ((*(uint16_t *)(g + 0x6a) & 0xe0) != 0) {
        deferred_cmd_post(0x400,0xc,0);
        mc_slot_action(g,1);
      } else {
        return 0;
      }
      g[0] = 8; g[1] = 0xf0;
      return 0;
    case 2:
      /* a0 not rewritten by original: stale ctx8 (last a0 write) */
      if (u16_counter_step_pad_a(ctx8,3,1) == 0) return 0;
      BDC0_STATUS();
    case 3:
      if (u16_counter_step_pad_a(a0 + 0x18,1,1) == 0) return 0;
      BDC0_STATUS();
    case 4:
      if (u16_counter_step_pad_a(a0 + 0x1a,1,1) == 0) return 0;
      BDC0_STATUS();
    case 5:
      if (u16_counter_step_pad_b(ctx8,8) == 0) return 0;
      BDC0_STATUS();
    case 6:
      if (u16_counter_step_pad_a(a0 + 0x1e,8) == 0) return 0;
      BDC0_STATUS();
    default:
      return 0;
    }
  }
  if (sel < 0x107) {
    switch (sel) {
    case 7:
      if (u16_counter_step_pad_a(a0 + 0x20,1,1) == 0) return 0;
      BDC0_STATUS();
    case 8:
      if (u16_counter_step_pad_a(ctx8,1,1) == 0) return 0;
      BDC0_STATUS();
    case 9:
      if (u16_counter_step_pad_a(a0 + 0x24,1,1) == 0) return 0;
      BDC0_STATUS();
    case 0xa: {
      uint32_t w40;
      if ((*(uint16_t *)(g + 0x7c) & 0xe0) == 0) return 0;
      deferred_cmd_post(0x400,0xc,0);
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
      scroll_offset_accumulate(0,0xa);
      BDC0_E58C(g32(0x801b4038 + (uint32_t)(*(int16_t *)(s4 + 0x14)) * 4),0x140,f2);
      scroll_offset_accumulate(0,0xc);
      return 0;
    case 0x102:
      BDC0_E58C(0x8012128c,0x1000,flag);
      BDC0_E58C(g32(0x801b4048 + (uint32_t)(*(int16_t *)(s4 + 0x16)) * 4),0x140,flag);
      scroll_offset_accumulate(0,0xc);
      return 0;
    case 0x103:
      BDC0_E58C(0x80121298,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x18)) * 4),0x140,flag);
      scroll_offset_accumulate(0,0xc);
      return 0;
    case 0x104:
      BDC0_E58C(0x801212a8,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x1a)) * 4),0x140,flag);
      scroll_offset_accumulate(0,0xc);
      return 0;
    case 0x105: {
        /* buf = $sp+0x18, frame -0x48 -> entry_sp-0x30; sprintf writes it
         * and BDC0_E58C reads it through nested frames, so reserve the
         * guest frame (sp+0x18 -> frm+0x20) for the write->read span. */
        extern uint32_t br2_guest_frame(uint32_t);
        extern void br2_guest_frame_free(void);
        uint32_t dsw_frm = br2_guest_frame(0x48);
        uint32_t dsw_buf = dsw_frm + 0x20;
        BDC0_E58C(0x801212bc,0x1000,flag);
      round_announce_block(ctx8,ctxc,(const void *)guest_at(0x801212cc),0x140,0x1000,flag);
      sprintf_core((char *)guest_at(dsw_buf),(char *)guest_at(0x801212d0),
                   (int32_t)*(int16_t *)(s4 + 0x1c));
      BDC0_E58C(dsw_buf,0x140,flag);
      br2_guest_frame_free();
      scroll_offset_accumulate(0,0xc);
      return 0;
    }
    case 0x106: {
      extern uint32_t br2_guest_frame(uint32_t);
      extern void br2_guest_frame_free(void);
      uint32_t dsw_frm = br2_guest_frame(0x48);
      uint32_t dsw_buf = dsw_frm + 0x20;
      BDC0_E58C(0x801212d4,0x1000,flag);
      round_announce_block(ctx8,ctxc,(const void *)guest_at(0x801212cc),0x140,0x1000,flag);
      sprintf_core((char *)guest_at(dsw_buf),(char *)guest_at(0x801212d0),
                   (int32_t)*(int16_t *)(s4 + 0x1e));
      BDC0_E58C(dsw_buf,0x140,flag);
      br2_guest_frame_free();
      scroll_offset_accumulate(0,0xc);
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
      scroll_offset_accumulate(0,0xc);
      return 0;
    case 0x108:
      BDC0_E58C(0x801212f4,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x22)) * 4),0x140,flag);
      scroll_offset_accumulate(0,0xc);
      return 0;
    case 0x109:
      BDC0_E58C(0x80121304,0x1000,flag);
      BDC0_E58C(g32(0x801b3f70 + (uint32_t)(*(int16_t *)(s4 + 0x24)) * 4),0x140,flag);
      scroll_offset_accumulate(0,0xc);
      return 0;
    case 0x10a:
      BDC0_E58C(0x80121310,0x1000,flag);
      scroll_offset_accumulate(0,0xc);
      return 0;
    default:
      return 0;
    }
  }
  if (sel == 0x10b) {
    BDC0_E58C(0x80121318,0x1000,flag);
    scroll_offset_accumulate(0,0x14);
    return 0;
  }
  return 0;
}

/* ======================================================================
 * Round-3 ports: save/load state machines, memcard wrappers, HUD debug
 * dumps. Guest addrs via guest_at/br2_g*; buffer params that callers
 * pass as host pointers are dereferenced directly.
 * ====================================================================== */

/* These callees are defined in menu.c/game.c with real prototypes. The weak
 * ()-stubs made calls compile unprototyped: clang dropped argument
 * dereferences (g32/guest_at results) and could misplace multi-arg values —
 * the same defect class as heap_record_emit/terrain_packet_gen/heap_ptr_fixup.
 * Declare the real signatures instead of conflicting stubs. descblk_variant_*
 * param_3 is void* in the callee; const void* here is ABI-identical and
 * accepts both const and non-const call arguments. */
/* job_result_poll_stable/memcard_block_verify are guest-domain u32 functions
 * (menu.c defs; verified against generated 0x80168648/0x8016827c). Their
 * callers must therefore hand them guest addresses, not host pointers —
 * mc_op_seq_probe normalises with br2_gaddr and mc_data_write stages its
 * stack buffers into a br2_guest_frame so every pointer that crosses into
 * these callees (directly or via mc_job_* / cd_chan_job_pump's guest_of
 * stores) is guest-RAM-backed end to end. */
extern int32_t memcard_block_verify(uint32_t,uint32_t,int32_t);
extern int32_t job_result_poll_stable(uint32_t,uint32_t,uint32_t);
extern uint32_t br2_gaddr(const void *);
extern int32_t job_state_query(void);
extern int32_t slot_attr_packet_build(uint32_t,uint32_t,uint32_t,
    int32_t,int32_t,int32_t);
extern void descblk_variant_e(uint32_t,uint32_t,const void *,
    int32_t,int32_t,int32_t);
extern void descblk_variant_f(uint32_t,uint32_t,const void *,
    int32_t,int32_t,int32_t,int32_t);
extern void hud_text_a(uint32_t,uint16_t,const void *,
    uint16_t,uint16_t,int16_t);
extern int32_t attr_records_scan(uint32_t,uint32_t);
/* ret_zero_d is guest 0x8016DADC: writes the save-record header at a3 and
 * expands packed source fields into 0x70-byte records, returning the
 * advanced heap-A cursor (a3+20+0x70*nrec) the caller stores to
 * 0x801c47b8. Faithful port of generated 0x8016DADC-0x8016DDF4 — three
 * nested loops (outer t9=1..N-1 over N=lhu(a0+4); middle t3=0..b5-1 over
 * b5=u8(a1+5); inner t2=0..b4-1 over b4=u8(a1+4)) scanning a u16 selector
 * array at a1+8; each selector==1 writes one record at cursor a3+88+0x70*k
 * (t4 cursor a3+20 tracks it, +0x70 per record). No calls, no MMIO, no
 * IRQ-sensitive waits — clean C is faithful. a0-a4 are guest addresses
 * (heap cursors). Every access goes through br2_ldN/br2_stN on the guest
 * address, NOT a cached guest_at() host pointer: the record cursor can
 * legitimately run past the 2MB RAM arena top (heap-A sits near
 * 0x801Cxxxx), where the generated body's cpu->write_* mirrors/wraps the
 * guest address while a raw host pointer would walk off `ram[]` and
 * corrupt adjacent .bss (observed: text_ref_image/lo/hi trashed ->
 * wild memcmp in dirty_ram_text_native_ok_ranges_from at frame 7869). */
extern uint8_t  br2_ld8 (uint32_t a);
extern uint16_t br2_ld16(uint32_t a);
extern uint32_t br2_ld32(uint32_t a);
extern void     br2_st8 (uint32_t a, uint8_t  v);
extern void     br2_st16(uint32_t a, uint16_t v);
extern void     br2_st32(uint32_t a, uint32_t v);
uint32_t ret_zero_d(uint32_t a0, uint32_t a1, uint32_t a2,
                    uint32_t a3, uint32_t a4)
{
    uint32_t curg = a3 + 20;           /* t4 cursor: a3+20, +0x70/record */
    uint32_t nrec = 0;
    uint32_t N    = br2_ld16(a0 + 4);  /* record-group count */
    uint32_t cnt_a = br2_ld8(a1 + 4);  /* inner bound  (a1 reg) */
    uint32_t cnt_b = br2_ld8(a1 + 5);  /* middle bound (s2)    */
    uint32_t add6  = br2_ld8(a1 + 6);  /* s6 */
    uint32_t add7  = br2_ld8(a1 + 7);  /* s5 */
    uint32_t t9;
    { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
      decomp_trace_record(30,a0,a1,(uint8_t)cnt_a,N);
      decomp_trace_record(31,(uint32_t)cnt_b,a2,(uint8_t)add6,add7); }

    /* header at a3+0..16 (guest values stored are guest addresses) */
    br2_st32(a3 + 0,  1u);
    br2_st32(a3 + 4,  0x00040051u);
    br2_st32(a3 + 12, a3 + 20);
    br2_st32(a3 + 16, a4);

    for (t9 = 1; t9 < N; t9++) {
        /* per-outer-iteration values (DB74-DBCC):
         *   selector stream s7 = a1+8 (reset each t9, +2 per inner slot);
         *   middle-loop centre = cnt_b >> (t9 & 31)  (DB80 srlv -> sp+8);
         *   field-source ptr    = a0 + 8 + t9*8      (DB94 t9<<3 + DB40 a0+8). */
        uint32_t srcg  = a1 + 8;         /* t7: u16 selector stream */
        uint32_t half_b = cnt_b >> (t9 & 31u);
        uint32_t t1g   = a0 + 8 + t9 * 8; /* per-group field source */
        uint32_t t3;
        if (cnt_b == 0) continue;         /* DB88: beqz s2 -> outer bottom */
        for (t3 = 0; t3 < cnt_b; t3++) {
            /* middle-loop constants (DBA4-DBC0): tm5=-384*(t3-half_b),
             * tm8=tm5-384 — stored as u16 into each written record. */
            int32_t dm  = (int32_t)t3 - (int32_t)half_b;
            uint32_t tm5 = (uint32_t)(-(dm * 384));
            uint32_t tm8 = tm5 - 384u;
            uint32_t t2;
            if (cnt_a == 0) continue;     /* DB9C: beqz a1 -> middle bottom */
            for (t2 = 0; t2 < cnt_a; t2++) {
                uint32_t sel = br2_ld16(srcg);
                srcg += 2;                /* delay-slot advance, every iter */
                if (sel != t9) continue;  /* DBD8: sel != t9(gpr[25]) -> skip */
                {
                    /* ==1 path: write one 0x70 record. D = cur+68 cursor. */
                    uint32_t dg = curg + 68;
                    int32_t di = (int32_t)t2 - (int32_t)(cnt_a >> 1);
                    uint32_t vi = (uint32_t)(di * 384);   /* inner v1 */
                    uint32_t w  = vi + 384u;              /* inner v0 */
                    uint16_t f2, f6;
                    uint32_t fl, b0, b1, c0, c1;

                    br2_st8(dg - 33, 9);  br2_st8(dg - 29, 44);
                    br2_st8(dg + 7, 9);   br2_st8(dg + 11, 44);
                    br2_st8(dg - 32, 128); br2_st8(dg - 31, 128);
                    br2_st8(dg - 30, 128);
                    br2_st8(dg + 8, 128);  br2_st8(dg + 9, 128);
                    br2_st8(dg + 10, 128);
                    br2_st16(curg + 0, (uint16_t)vi);
                    br2_st16(dg - 66, 0);
                    br2_st16(dg - 64, (uint16_t)tm5);
                    br2_st16(dg - 60, (uint16_t)w);
                    br2_st16(dg - 58, 0);
                    br2_st16(dg - 56, (uint16_t)tm5);
                    br2_st16(dg - 52, (uint16_t)vi);
                    br2_st16(dg - 50, 0);
                    br2_st16(dg - 48, (uint16_t)tm8);
                    br2_st16(dg - 44, (uint16_t)w);
                    br2_st16(dg - 42, 0);
                    br2_st16(dg - 40, (uint16_t)tm8);

                    f2 = br2_ld16(t1g + 2);
                    br2_st16(dg + 18, f2);
                    br2_st16(dg - 22, f2);
                    b0 = br2_ld8(t1g);
                    br2_st8(dg + 32, (uint8_t)b0); br2_st8(dg - 8,  (uint8_t)b0);
                    br2_st8(dg + 16, (uint8_t)b0); br2_st8(dg - 24, (uint8_t)b0);
                    b1 = br2_ld8(t1g + 1);
                    br2_st8(dg + 25, (uint8_t)b1); br2_st8(dg - 15, (uint8_t)b1);
                    br2_st8(dg + 17, (uint8_t)b1); br2_st8(dg - 23, (uint8_t)b1);
                    c0 = b0 + add6 - 1;
                    br2_st8(dg + 40, (uint8_t)c0); br2_st8(dg + 0,  (uint8_t)c0);
                    br2_st8(dg + 24, (uint8_t)c0); br2_st8(dg - 16, (uint8_t)c0);
                    c1 = b1 + add7 - 1;
                    br2_st8(dg + 41, (uint8_t)c1); br2_st8(dg + 1,  (uint8_t)c1);
                    br2_st8(dg + 33, (uint8_t)c1); br2_st8(dg - 7,  (uint8_t)c1);
                    f6 = br2_ld16(t1g + 6);
                    br2_st16(dg + 26, f6);
                    br2_st16(dg - 14, f6);

                    fl = br2_ld16(t1g + 4);
                    if (fl & 1) {           /* DCF0-0x8016DD1C fixup */
                        uint32_t x = br2_ld8(dg - 23), y = br2_ld8(dg - 7);
                        uint32_t z = br2_ld8(dg + 1),  w2 = br2_ld8(dg - 15);
                        br2_st8(dg + 33, (uint8_t)x);  br2_st8(dg - 7,  (uint8_t)x);
                        br2_st8(dg + 17, (uint8_t)y);  br2_st8(dg - 23, (uint8_t)y);
                        br2_st8(dg + 25, (uint8_t)z);  br2_st8(dg - 15, (uint8_t)z);
                        br2_st8(dg + 41, (uint8_t)w2); br2_st8(dg + 1,  (uint8_t)w2);
                    }
                    if (fl & 2) {           /* 0x8016DD34-0x8016DD60 fixup */
                        uint32_t x = br2_ld8(dg - 24), y = br2_ld8(dg - 16);
                        uint32_t z = br2_ld8(dg + 0),  w2 = br2_ld8(dg - 8);
                        br2_st8(dg + 24,  (uint8_t)x); br2_st8(dg - 16, (uint8_t)x);
                        br2_st8(dg + 16,  (uint8_t)y); br2_st8(dg - 24, (uint8_t)y);
                        br2_st8(dg + 32,  (uint8_t)z); br2_st8(dg - 8,  (uint8_t)z);
                        br2_st8(dg + 40,  (uint8_t)w2); br2_st8(dg + 0, (uint8_t)w2);
                    }
                    nrec++;
                    curg += 0x70;            /* s0++/a3+=0x70/t4+=0x70 */
                }
            }
        }
    }

    /* epilogue 0x8016DDA4-0x8016DDF4 */
    br2_st32(a3 + 8,  nrec);
    br2_st32(a2 + 28, a3);
    br2_st16(a2 + 12, 0);
    br2_st16(a2 + 14, 0);
    { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
      decomp_trace_record(16,a3,(uint32_t)(a3 + 20 + nrec * 0x70),0,
                          nrec * 0x70); }
    return a3 + 20 + nrec * 0x70;
}
/* heap_ptr_fixup is defined in menu.c as void heap_ptr_fixup(uint32_t).
 * The weak ()-stub here made calls compile unprototyped: clang dropped the
 * g32() dereference on the argument, so heap_ptr_fixup received
 * guest_at(0x801e3cfc) — a host pointer to the cursor cell — instead of the
 * cursor value (the decoded tag table base). It then read count=0 via
 * open-bus and converted no offset slots, leaving raw offsets that
 * heap_record_emit fed to heap_serialize as blk=0xF4 (infinite block-walk).
 * Declare the real signature instead of a conflicting stub. */
extern void heap_ptr_fixup(uint32_t);
/* heap_record_emit is defined in menu.c with a full 9-param prototype
 * (param_6 is a uint32_t* out-count). The weak ()-stub made calls compile
 * unprototyped so the callee read a2/param_6 from the wrong arg slots —
 * a2 came back as a decode source pointer and was stored into the heap-A
 * cursor, running it into the compressed-source region. Declare the real
 * signature instead of a conflicting stub (same fix as terrain_packet_gen). */
extern uint32_t heap_record_emit(uint32_t,uint32_t,uint32_t,uint32_t,
    uint32_t,uint32_t *,uint32_t,uint32_t,uint32_t);
/* terrain_packet_gen is defined in menu.c with a full prototype; the weak
 * ()-stub here made calls compile as "no prototype / too many args" and
 * pass garbage in the arg registers (crash: a2 read as 0x25). Declare
 * the real signature instead of a conflicting stub. */
extern uint32_t terrain_packet_gen(uint32_t,uint32_t,const uint16_t *,
    const int16_t *,const int16_t *,int32_t,int32_t,const uint8_t *);
/* State-machine callees defined in menu.c as int32_t name(void); ret_zero_e
 * is int ret_zero_e(int32_t). Same unprototyped-stub defect class — declare
 * the real signatures. */
extern int32_t save_flow_state_machine(void);
extern int32_t options_state_machine(void);
extern int32_t screen_flow_state_machine(void);
extern int ret_zero_e(int32_t);
/* printf_forward's real def is the variadic gfx.c:982
 * void printf_forward(const char *, ...). A ()-stub here would shadow-call
 * unprototyped; declare the real signature instead. */
extern void printf_forward(const char *, ...);

/* callee externs for the ports below. The weak stubs above are ()-style
   non-prototype defs, so calls below promote args the usual way; results
   are read as int32_t. */
extern int32_t cd_job_result(uint32_t,uint32_t *,uint32_t *);
extern uint8_t *tag_table_get(void);

/* stream_block_decode's real signature (menu.c): (guest heap ptr, host int*
   tag table, tag) -> new heap ptr. save.c's mid-file extern says void;
   call through a cast to recover args+result faithfully. */
static uint32_t heap_alloc54(uint32_t heap, int *tbl, int32_t tag)
{
  return ((uint32_t (*)(uint32_t, int *, int))stream_block_decode)(heap, tbl, tag);
}

#define BR2_W32(a,v)   ((uint32_t)(a)==0x801c47b8u ? \
   ({uint32_t _bv=(uint32_t)(v);uint32_t *_bp=(uint32_t *)guest_at(0x801c47b8u);uint32_t _bo=*_bp;*_bp=_bv; \
     {extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);decomp_trace_record(21,_bo,_bv,0,__LINE__);} _bv;}) \
   : (*(uint32_t *)guest_at(a) = (uint32_t)(v)))
#define BR2_W16(a,v)   (*(uint16_t *)guest_at(a) = (uint16_t)(v))
#define BR2_WS16(a,v)  (*(int16_t *)guest_at(a) = (int16_t)(v))
#define BR2_W8(a,v)    (*(uint8_t *)guest_at(a) = (uint8_t)(v))

/* ---- display_cur_invalidate @0x8015e87c (3 insns) ---- */
int display_cur_invalidate(void)
{
  BR2_W32(0x801b4154, 0xffffffffu);
  return -1;
}

/* ---- display_pair_invalidate @0x8015e88c (5 insns) ---- */
int display_pair_invalidate(void)
{
  BR2_W32(0x801b4158, 0xffffffffu);
  BR2_W32(0x801b415c, 0xffffffffu);
  return (int)0x801b0000;
}

/* ---- display_page_commit_a @0x8015e8a4 (85 insns) ----
   Display-page commit for channel A (regs @0x801f4760/64, state @0x1f80008c,
   current @0x801b4154). a0 = requested page (s2). */
int display_page_commit_a(int32_t a0)
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
        cd_file_load_sm(-1, 0);
        s0 = 0x10;
        BR2_W32(0x801c43a0, 0);
        BR2_W8(0x801e3d00, 0xff);
      } else if (s0 != 0x10) {
        *(int16_t *)(s3 + 0x8c) = (int16_t)s0;
        return 0;
      }
      if (cd_file_load_sm((s2 >> 2) + 0x118,
                       (uint32_t)g32(0x801f4760)) == 0) {
        *(int16_t *)(s3 + 0x8c) = (int16_t)s0;
        return 0;
      }
    }
    /* commit path (also taken when same 1KB page) */
    s1 = (int32_t)g8(0x1f800013);
    scratch_013_set(0);
    stream_block_decode(g32(0x801f4764), (int *)guest_at(g32(0x801f4760)), s2 & 3);
    spr_draw_pair(g32(0x801f4764), 0, 0x1ff, 1, 0x200, 0, 1);
    cd_timer_read(0);
    scratch_013_set((uint32_t)s1);
    BR2_W32(0x801b4154, (uint32_t)s2);
  }
  *(int16_t *)(s3 + 0x8c) = -1;
  return 1;
}

/* ---- display_page_commit_b @0x8015e9fc (126 insns) ----
   Display-page commit for channel B (regs @0x801f4768/6c, state @0x1f80008e,
   currents @0x801b4158/0x801b415c). a0 = bank, a1 = offset. */
int display_page_commit_b(uint32_t a0, uint32_t a1)
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
    if (cd_file_load_sm(0x16, 0x80050000u) == 0) {
      *(int16_t *)(s3 + 0x8e) = (int16_t)s0;
      return 0;
    }
    s1 = (int32_t)g8(0x1f800013);
    scratch_013_set(0);
    spr_draw_off((int32_t)g32(0x801f4768), 0, 0x1e0, 1);
    s0 = 0x10;
    cd_timer_read(0);
    scratch_013_set((uint32_t)s1);
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
    cd_file_load_sm(-1, 0);
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
    if (cd_file_load_sm(a0x, (uint32_t)g32(0x801f4768)) == 0) {
      *(int16_t *)(s3 + 0x8e) = (int16_t)s0;
      return 0;
    }
  }
commit:
  s1 = (int32_t)g8(0x1f800013);
  scratch_013_set(0);
  stream_block_decode(g32(0x801f476c), (int *)guest_at(g32(0x801f4768)), s2 & 7);
  spr_draw_off((int32_t)g32(0x801f476c), 0x300, 0x100, 1);
  cd_timer_read(0);
  scratch_013_set((uint32_t)s1);
  BR2_W32(0x801b4158, s4);
  BR2_W32(0x801b415c, (uint32_t)s2);
same:
  *(int16_t *)(s3 + 0x8e) = -1;
  return 1;
}

/* ---- mc_op_seq_probe @0x8016831c (24 insns) ----
   memcard op wrapper: probe (job_result_poll_stable), on success memcard_block_verify then
   job_state_query. a2/a3/stack-a4 forwarded to memcard_block_verify.
   Guest contract (generated 0x8016831c-0x80168368): a0/a1 pass through to
   job_result_poll_stable with a2=1; on rc==0, memcard_block_verify gets
   (old a2, old a3, stack arg). Both callees are guest-domain u32 — the
   buffer args arrive here as host pointers, so normalise to guest
   addresses with br2_gaddr (guest RAM pointers map back; bare guest
   u32s pass through). */
int mc_op_seq_probe(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  int32_t r = job_result_poll_stable(a0,br2_gaddr(a1),1);
  if (r != 0) return r;
  memcard_block_verify(br2_gaddr(a2),a3,(int32_t)a4);
  return job_state_query();
}

/* ---- mc_file_cmd_stable_poll @0x801685b4 (36 insns) ----
   Poll mc_file_cmd until result stabilises (or 0). JT@0x80122144: idx0
   returns immediately; all other codes re-poll while the value changes. */
int mc_file_cmd_stable_poll(uint32_t a0,const void *a1,uint32_t a2)
{
  int32_t prev = 0;
  int32_t r;
  for (;;) {
    r = mc_file_cmd(a0,a1,a2);
    if (r == 0) return 0;
    if (r == prev) return r;
    prev = r;
  }
}

/* ---- mc_job_a_result_poll @0x80168838 (45 insns) ----
   Issue via mc_job_issue_a then poll cd_job_result until a stable nonzero
   result code (0 returns immediately). */
int mc_job_a_result_poll(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  extern int br2_bail_pending(void);
  uint32_t t1 = 0, code = 0;
  int32_t prev = 0;
  for (;;) {
    while (mc_job_issue_a(a0,a1,a2,a3,a4) == 0) {
      if (br2_bail_pending()) return -1;
    }
    while (cd_job_result(0,&t1,&code) == 0) {
      if (br2_bail_pending()) return -1;
    }
    if (code == 0) return 0;
    if ((int32_t)code != prev) { prev = (int32_t)code; continue; }
    return (int32_t)code;
  }
}

/* ---- mc_job_b_result_poll @0x801688f0 (45 insns) ---- same shape, mc_job_issue_b */
int mc_job_b_result_poll(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  extern int br2_bail_pending(void);
  uint32_t t1 = 0, code = 0;
  int32_t prev = 0;
  for (;;) {
    while (mc_job_issue_b(a0,a1,a2,a3,a4) == 0) {
      if (br2_bail_pending()) return -1;
    }
    while (cd_job_result(0,&t1,&code) == 0) {
      if (br2_bail_pending()) return -1;
    }
    if (code == 0) return 0;
    if ((int32_t)code != prev) { prev = (int32_t)code; continue; }
    return (int32_t)code;
  }
}

/* ---- hud_slot_blitter @0x8016f30c (158 insns) ----
   10-slot ambient/palette HUD blitter. a0 = guest state ptr, a1 = flag.
   s1 = per-slot record @0x801f4ee8 + idx*0x18; halfword attr array at
   s1+0..0x13, counters @0x14/0x15, timer @0x16. */
void hud_slot_blitter(uint32_t a0, uint32_t a1)
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
    slot_attr_packet_build(_DAT_1f800008, _DAT_1f80000c, attr, 0x1000, 0x1000, a1);
    idx %= 10;
    cnt += 1;
    if (cnt >= 10) break;
  }
  print_dispatch((const char *)guest_at(0x801223cc));
  if (a1 != 0)
    print_dispatch((const char *)guest_at(0x801223cc));
}

/* ---- flag_table_dump @0x8017f13c (150 insns) ----
   Debug flag-name dump. a0 = guest state ptr, a1 = group 0..5
   (AF/PF/NF/KF/DF/EF). Each group: header, 32 bits of mask @a0+off,
   name strings from table @0x801b4f28+grp*0x80. */
void flag_table_dump(uint32_t a0, uint32_t a1)
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

/* ---- record_struct_dump @0x8018106c (115 insns) ----
   Debug dump of controller/record struct. a0 = guest state ptr;
   s2 = sub-struct ptr @a0+0x3c (guest ptr -> host). */
void record_struct_dump(uint32_t a0)
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
  flag_table_dump(a0, 0);
  flag_table_dump(a0, 5);
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

/* ---- save_channel_init_commit @0x80170d38 (169 insns) ----
   Save-channel init/commit. a0 = guest state ptr (h@0x8e = phase),
   a1 unused, s2/s3 = host save-slot buffers. Uses heap A @0x801c47b8
   (tag table via tag_table_get) and heap B @0x801e3cfc. */
int save_channel_init_commit(uint32_t a0,int32_t a1,uint8_t *s2,uint8_t *s3)
{
  uint8_t *s5 = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(s5 + 0x8e);
  int32_t s4;
  int32_t i, sz;
  uint32_t head, np;
  int *tbl;
  (void)a1;

  if (s0 == 0) goto st0;
  if (s0 == 0x10) goto st10;
  *(int16_t *)(s5 + 0x8e) = (int16_t)s0;
  return 0;

st0:
  if (cd_file_load_sm(0x13, guest_of(tag_table_get())) == 0) {
    *(int16_t *)(s5 + 0x8e) = (int16_t)s0;
    return 0;
  }
  s4 = (int32_t)g8(0x1f800013);
  scratch_013_set(0);
  tbl = (int *)tag_table_get();
  for (i = 0; i < 0x64; i++) {
    /* gen 0x80170DC0-0x80170DEC: slot[i] = raw heap cursor, then
     * stream_block_decode(heap, tbl, i); on success cursor += sz and
     * data_relocate is called with a0 = [slot] — the value JUST stored
     * (the alloc base h), NOT the slot's previous contents. The earlier
     * port passed the stale `old` slot word, relocating the wrong block. */
    uint32_t h = g32(0x801c47b8);
    BR2_W32(0x801c43a8 + (uint32_t)i * 4, h);
    sz = (int32_t)heap_alloc54(h, tbl, i);
    if (sz != 0) {
      BR2_W32(0x801c47b8, g32(0x801c47b8) + (uint32_t)sz);
      data_relocate(h);
    } else {
      BR2_W32(0x801c43a8 + (uint32_t)i * 4, 0);
    }
    BR2_W32(0x801c47b8, (g32(0x801c47b8) + 3) & ~3u);
  }
  /* heap B blocks */
  sz = (int32_t)heap_alloc54(g32(0x801e3cfc), tbl, 0x100);
  if (sz != 0) {
    spr_draw_pair(g32(0x801e3cfc), 0x180, 0x1e0, 1, 0x3c0, 0x180, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  sz = (int32_t)heap_alloc54(g32(0x801e3cfc), tbl, 0x101);
  if (sz != 0) {
    spr_draw_off((int32_t)g32(0x801e3cfc), 0x240, 0x100, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  sz = (int32_t)heap_alloc54(g32(0x801e3cfc), tbl, 0x102);
  if (sz != 0) {
    spr_draw_off((int32_t)g32(0x801e3cfc), 0x190, 0x1e0, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  sz = (int32_t)heap_alloc54(g32(0x801e3cfc), tbl, 0x103);
  if (sz != 0) {
    spr_draw_off((int32_t)g32(0x801e3cfc), 0x1a0, 0x1e0, 1);
    BR2_W32(0x801e3cfc, g32(0x801e3cfc) + (uint32_t)sz);
  }
  cd_timer_read(0);
  scratch_013_set((uint32_t)s4);
  *(int16_t *)(s5 + 0x8e) = 0x10;
  return 0;

st10:
  if (cd_file_load_sm(0x10, g32(0x801c47b8)) == 0) {
    *(int16_t *)(s5 + 0x8e) = (int16_t)s0;
    return 0;
  }
  head = g32(0x801c47b8);
  np = head + 8;
  /* s2/s3 are host save-slot structs; +0x38 holds a GUEST pointer whose
     +0xa8 slot caches the new heap head — resolve via guest_at before the
     store or the raw guest address faults as a host pointer. */
  *(uint32_t *)guest_at(*(uint32_t *)(s2 + 0x38) + 0xa8) = np;
  *(uint32_t *)guest_at(*(uint32_t *)(s3 + 0x38) + 0xa8) = np;
  BR2_W32(0x801c47b8, head + *(uint32_t *)guest_at(head));
  return 1;
}

/* ---- hud_announce @0x8015143c (548 insns) ----
   Battle-announcement HUD printer. a0 = guest state ptr (regs/sprite
   coords), a1 = announcement id 0..0x16 (JT@0x80120840), a2 unused.
   Gate: (*(u16*)(*(u32*)(a0+0xc8)+2) >> 0xa) & 1 must be set.
   All cases draw via descblk_variant_e/descblk_variant_f/hud_text_a with
   coords derived from lhu/lh @+0x20/+0x22. */

static int32_t hud_x2(uint8_t *s0)
{ /* (sxt16 lhu@0x20) / 2 trunc */
  return ((int16_t)*(uint16_t *)(s0 + 0x20)) / 2;
}
static int32_t hud_y2(uint8_t *s0)
{ /* (sxt16 lhu@0x22) / 2 trunc */
  return ((int16_t)*(uint16_t *)(s0 + 0x22)) / 2;
}
static int32_t hud_y3(uint8_t *s0)
{ /* (lh@0x22 * 3) / 8 trunc */
  return (*(int16_t *)(s0 + 0x22) * 3) / 8;
}
static int32_t hud_div4(int32_t v)
{ /* (v<0 ? v+3 : v) >> 2 == v/4 trunc */
  return v / 4;
}

void hud_announce(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint8_t *s0 = (uint8_t *)guest_at(a0);
  uint32_t sel = a1;
  uint32_t r8, rc;
  int32_t x2, y2, y3;
  char *buf;
  (void)a2;

  r8 = g32(a0 + 0xc8);
  if ((((uint32_t)g16(r8 + 2)) >> 0xa & 1) == 0) return;
  if (sel >= 0x17) return;

  r8 = *(uint32_t *)(s0 + 8);
  rc = *(uint32_t *)(s0 + 0xc);
  x2 = hud_x2(s0);
  y2 = hud_y2(s0);
  y3 = hud_y3(s0);

  switch (sel) {
  case 0:
    /* "Here Comes A Challenger !!!" via 7-arg descblk_variant_f */
    descblk_variant_f(r8, rc, (const void *)guest_at(0x80120768),
                 x2 - 0xe7, y2 - 0xc, 8, -4);
    return;
  case 1:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x80120784),
                 (int16_t)(x2 - 0x84), y3, 8);
    return;
  case 2:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x80120794),
                 (int16_t)(x2 - 0x79), y3, 0xe);
    return;
  case 3: {
    char *bp;
    int32_t v;
    /* sprintf'd string is handed to descblk_variant_e -> guest_of(param_3);
     * a host stack buffer yields 0. Back it with a guest frame. */
    buf = (char *)guest_at(br2_guest_frame(0x20) + 0x10);
    bp = buf;
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207a0),
                 (int16_t)(x2 - 0x4d), y3, 0xe);
    sprintf_core(bp, (char *)guest_at(0x801207a8),
                 (int32_t)*(int8_t *)(s0 + 0x99) + 1);
    v = (*(int16_t *)(s0 + 0x22) * 3);
    if (v < 0) v += 7;
    v >>= 3;
    descblk_variant_e(r8, rc, bp, 0x1000, v, 0xe);
    br2_guest_frame_free();
    return;
  }
  case 4:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207ac),
                 (int16_t)(x2 - 0x6e), y3, 0xe);
    return;
  case 5:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207b8),
                 (int16_t)(x2 - 0x42), y3, 8);
    return;
  case 6:
  case 10:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207c0),
                 (int16_t)(x2 - 0x63), (int16_t)(y2 - 0x10), 0xe);
    return;
  case 7:
  case 11:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207cc),
                 (int16_t)(x2 - 0x79), (int16_t)(y2 - 0x10), 8);
    return;
  case 8:
  case 12:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207d8),
                 (int16_t)(x2 - 0x58), (int16_t)(y2 - 0x10), 8);
    return;
  case 9:
  case 13:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207e4),
                 (int16_t)(x2 - 0x2c), (int16_t)(y2 - 0x10), 8);
    return;
  case 14:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207ec),
                 (int16_t)(x2 - 0x4d), (int16_t)(y2 - 0x10), 8);
    return;
  case 15:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x801207f8),
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
    descblk_variant_e(r8, rc, (const void *)guest_at(0x80120808),
                 (int16_t)(x2 - 0x42), (int16_t)(y2 - 0x10), 8);
    return;
  case 19:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x80120810),
                 (int16_t)(x2 - 0x4d), (int16_t)(y2 - 0x10), 8);
    return;
  case 20:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x80120818),
                 (int16_t)(x2 - 0x58), (int16_t)(y2 - 0x10), 0xa);
    return;
  case 21: {
    int64_t m = (int64_t)(*(uint16_t *)(s0 + 0xb2) & 0x3fff) * 0x66666667LL;
    int32_t hi = (int32_t)(m >> 32);
    int32_t num = 9 - (int32_t)(((int32_t)(hi << 11)) >> 16);
    int32_t v;
    /* sprintf'd string is handed to descblk_variant_e -> guest_of(param_3);
     * a host stack buffer yields 0. Back it with a guest frame. */
    buf = (char *)guest_at(br2_guest_frame(0x20) + 0x10);
    sprintf_core(buf, (char *)guest_at(0x80120824), num);
    v = (*(int16_t *)(s0 + 0x22) * 3);
    if (v < 0) v += 3;
    v = hud_div4(v) - 0x30;
    descblk_variant_e(r8, rc, buf, 0, (int16_t)v, 8);
    br2_guest_frame_free();
    return;
  }
  case 22:
    descblk_variant_e(r8, rc, (const void *)guest_at(0x80120834),
                 (int16_t)(x2 - 0x63), (int16_t)(y2 - 0x10), 0xa);
    return;
  default:
    return;
  }
}

/* ---- save_load_state_machine @0x80170fe0 (555 insns) ----
   Save/load channel state machine. a0 = guest state struct (h@+0x8e =
   state), a1 = slot/index param. States: 0=init(load build), 0x10=commit
   tail, 0x20=alternate commit. Heap A @0x801c47b8, heap B @0x801e3cfc,
   tag table via tag_table_get. */
int save_load_state_machine(uint32_t a0, int32_t a1)
{
  uint8_t *s0h = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(s0h + 0x8e);
  int32_t t1 = a1;
  int32_t saved8 = (int32_t)g8(0x1f800013);
  uint8_t blk[0x20];           /* sp+0x28..0x47 block for terrain_packet_gen */
  /* heap_record_emit's param_6 is a uint32_t* out-count (*s5 = 0; ++*s5)
   * — the guest slots are words; u16 locals would be overflowed by the
   * 4-byte store. */
  uint32_t sp48 = 0, sp4c = 0;
  int32_t i, sel, ret;
  uint32_t heapB, heapA, sz, z0, z1, zsum, old, s7, s3n, s0v, h;
  uint8_t *s2h;
  uint32_t lhu4, lhu6;
  int32_t lh4, lh6;
#define BK16(o)  (*(int16_t *)(blk + ((o) - 0x28)))
#define BK8(o)   (blk[(o) - 0x28])
#define BKX(v)   ((int32_t)(((uint32_t)(int32_t)(int16_t)(v)) << 16) >> 23)

  if (s0 == 0x10) goto L10;
  if (s0 < 0x11) {
    if (s0 != 0) goto ret0;
    goto init;
  }
  if (s0 == 0x20) goto L20;
  goto ret0;

init:
  /* ---- state 0: build save image ---- */
  if (attr_records_scan(0,0x17) == 0) goto ret0;
  if (cd_file_load_sm(t1 + 0xa0, guest_of(tag_table_get())) == 0)
    goto ret0;
  scratch_013_set(0);
  {
    uint8_t *s5 = (uint8_t *)guest_at(0x801f04a8);
    int32_t *tbl = (int *)tag_table_get();
    *(int16_t *)(s5 + 4) = 0;

    /* alloc tag0 + tag1, then ret_zero_d */
    heapB = g32(0x801e3cfc);
    z0 = heap_alloc54(heapB, tbl, 0);
    if (z0 != 0) {
      uint32_t mid, fin, r;
      old = heapB;
      *(uint32_t *)(s5 + 0) = 0x801c11b0u;
      mid = old + z0;                    /* heapB advance #1 (delay slot) */
      BR2_W32(0x801e3cfc, mid);
      z1 = heap_alloc54(mid, tbl, 1);
      zsum = z0 + z1;
      heapA = g32(0x801c47b8);
      fin = mid + zsum;                  /* heapB advance #2 (delay slot) */
      BR2_W32(0x801e3cfc, fin);
      r = (uint32_t)ret_zero_d(old, mid, 0x801c11b0u, heapA,
                               g32(0x801b4844u + (uint32_t)t1 * 4));
      BR2_W32(0x801c47b8, r);
      *(int16_t *)(s5 + 4) = (int16_t)(*(int16_t *)(s5 + 4) + 1);
    }
    *(int16_t *)(s5 + 0xc) = 0;

    /* alloc tag2: 0x17 character-record slots */
    heapB = g32(0x801e3cfc);
    if (heap_alloc54(heapB, tbl, 2) != 0) {
      uint8_t *rec = (uint8_t *)guest_at(0x801e3d08);
      uint8_t *s2p = rec + 4;
      int32_t s3i;
      uint32_t s4o = (uint32_t)t1 * 23;
      heap_ptr_fixup(g32(0x801e3cfc));
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
          uint32_t r = (uint32_t)heap_record_emit(
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
    if (heap_alloc54(heapB, tbl, 3) != 0) {
      heap_ptr_fixup(g32(0x801e3cfc));
      *(uint32_t *)(s5 + 0x10) = 0x801c4588u;
      {
        uint32_t r = (uint32_t)heap_record_emit(
            g32(0x801e3cfc), 0x801c4588u, g32(0x801c47b8), 0,
            0, &sp4c, g8(0x801b49a4u + (uint32_t)t1), 0, 0);
        BR2_W32(0x801c47b8, r);
        *(int16_t *)(s5 + 0x14) = (int16_t)sp4c;
      }
    }

    /* alloc tags 0x12,0x13,0x14 with spr_draw_off(heapB,0,0,0) */
    {
      static const int32_t tg[3] = { 0x12, 0x13, 0x14 };
      for (i = 0; i < 3; i++) {
        heapB = g32(0x801e3cfc);
        sz = heap_alloc54(heapB, tbl, tg[i]);
        if (sz != 0) {
          spr_draw_off((int32_t)g32(0x801e3cfc), 0, 0, 0);
          BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
        }
      }
    }

    /* alloc tags 0x20..0x25, two spr_draw_off calls each */
    {
      uint32_t s0p = 0x801b49b4u;
      for (i = 0; i < 6; i++, s0p += 4) {
        heapB = g32(0x801e3cfc);
        sz = heap_alloc54(heapB, tbl, 0x20 + i);
        if (sz != 0) {
          uint16_t w0 = g16(s0p);
          uint16_t w1 = g16(s0p + 2);
          spr_draw_off((int32_t)g32(0x801e3cfc), w0, w1, 1);
          spr_draw_off((int32_t)g32(0x801e3cfc), w0, w1 + 0x10, 1);
          BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
        }
      }
    }

    /* ---- render/param block: 5x terrain_packet_gen ---- */
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
        ret = (int32_t)terrain_packet_gen(0x801c46c8u + 0xa0,
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
        ret = (int32_t)terrain_packet_gen(0x801c46c8u, (uint32_t)ret,
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
        ret = (int32_t)terrain_packet_gen(0x801c46c8u + 0x28, (uint32_t)ret,
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
        ret = (int32_t)terrain_packet_gen(0x801c46c8u + 0x50, (uint32_t)ret,
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
        ret = (int32_t)terrain_packet_gen(0x801c46c8u + 0x78, (uint32_t)ret,
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
      sz = heap_alloc54(h, tbl, 0x40 + i);
      if (sz != 0) {
        BR2_W32(0x801c47b8, g32(0x801c47b8) + sz);
        data_relocate(g32(slot));
      } else {
        BR2_W32(slot, 0);
      }
    }
    cd_timer_read(0);
    scratch_013_set((uint32_t)saved8);
    s0 = 0x10;
  }

L10:
  /* commit tail: also reached by state-0 fallthrough and state 0x10 */
  if (t1 < 0) sel = 7;
  else if (t1 < 8) sel = t1;
  else if (t1 == 0xb) sel = 0xb;
  else sel = 7;
  if (cd_file_load_sm(sel + 0xb0, guest_of(tag_table_get())) == 0)
    goto ret0;
  s0 = 0x20;   /* resume state for L20: generated 0x80171800 li s0,0x20 —
                  without it a busy L20 load stores s0=0x10 and re-runs L10,
                  ping-ponging the tag-table/heap loads forever. */
  fighter_record_apply(guest_of(tag_table_get()), 2);
  page_table_ptr_set((uint32_t)t1, 2);

L20:
  if (cd_file_load_sm(0x88, g32(0x801c47b8)) == 0) goto ret0;
  fighter_record_apply(g32(0x801c47b8), 1);
  page_table_ptr_set(0, 1);
  return 1;

ret0:
  *(int16_t *)(s0h + 0x8e) = (int16_t)s0;
  return 0;
#undef BK16
#undef BK8
#undef BKX
}

/* ---- dialog_msg_dispatch @0x8015caa8 (605 insns) ----
   Dialog/message dispatcher. a0 = guest dialog-state ptr (fields
   lhu@+8, lbu/lh@+0x18..0x2a), a1 = selector (low16 dispatched).
   Sibling of dialog_sel_dispatch_wide. Status flag @0x801c15f0 (=0x801c11e0+0x410). */
int dialog_msg_dispatch(intptr_t a0, uint32_t a1)
{
  /* Same dual ABI as dialog_sel_dispatch: guest addr via clean_table,
   * host-into-guest desc pointer via the stepper fn_lookup leg.
   * a0 is reused below as a guest address (a0+0xNN counter addrs). */
  extern uint32_t br2_gaddr(const void *);
  a0 = (intptr_t)br2_gaddr((const void *)a0);
  uint8_t *s3 = (uint8_t *)guest_at((uint32_t)a0);
  uint8_t *g  = (uint8_t *)guest_at(0x1f800000);
  uint32_t ctx8 = *(uint32_t *)(g + 8);
  uint32_t ctxc = *(uint32_t *)(g + 0xc);
  uint32_t sel = a1 & 0xffff;
  uint32_t flag, v0;
  char buf[16];

#define CAA8_E58C(str,sz,x) \
  announce_param_block_init(ctx8,ctxc,(const void *)guest_at(str),(sz),0x1000,(x))
#define CAA8_STATUS() do { BR2_W32(0x801c15f0, 1); return 0; } while (0)
#define CAA8_GATE() do { \
    if ((g16(0x1f80007c) & 0xe0) == 0) return 0; \
    deferred_cmd_post(0x400,0xc,0); \
  } while (0)

  scroll_offset_set(0x20,0xffffffff);
  if (sel == (uint32_t)(*(uint16_t *)(s3 + 8) + 0x100)) {
    flag = 0;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121270),0x1000,0x1000,0);
  } else {
    flag = 1;
    hud_text_b(ctx8,ctxc,(const void *)guest_at(0x80121274),0x1000,0x1000,flag);
  }

  if (sel == 0x100) {
    scroll_offset_set(0xffffffff,0x18);
    CAA8_E58C(0x80121358,0x1000,0);
    banner_stripe_scroll(3,0,4,7);
    scroll_offset_accumulate(0,0x10);
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
      memcard_file_flow(0);
      *(uint16_t *)guest_at(*(uint32_t *)(g + 0xc8)) = c0[0x1d];
      dbg_marker_mode_set(c0[0x24]);
      xv = (uint32_t)c0[0xf] << 3;
      gauge_scale_fixed((short)c0[0xe],
                   (short)(int32_t)(((xv * (uint64_t)0x66666667u) >> 32) >> 2));
      brightness_commit((uint16_t)(c0[0xc] ^ 1));
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
      ret_zero_e(0);
      return 0;
    case 3:
      if (u16_counter_step_pad_b(a0 + 0x18,7) == 0) return 0;
      CAA8_STATUS();
    case 4:
      if (u16_counter_step_pad_b(a0 + 0x1a,7) == 0) return 0;
      CAA8_STATUS();
    case 5:
      if (u16_counter_step_pad_a(a0 + 0x1c,4,1) == 0) return 0;
      CAA8_STATUS();
    case 6:
      if (u16_counter_step_pad_b(a0 + 0x1e,3) == 0) return 0;
      CAA8_STATUS();
    default:
      return 0;
    }
  }

  if (sel < 0x109) {
    switch (sel) {
    case 7:
      if (u16_counter_step_pad_a(a0 + 0x20,1,1) == 0) return 0;
      CAA8_STATUS();
    case 8:
      if (u16_counter_step_pad_a(a0 + 0x22,3,1) == 0) return 0;
      CAA8_STATUS();
    case 9:
      CAA8_GATE();
      save_flow_state_machine();
      return 0;
    case 0xa:
      CAA8_GATE();
      options_state_machine();
      return 0;
    case 0xb:
      CAA8_GATE();
      screen_flow_state_machine();
      return 0;
    case 0xc: {
      int32_t av;
      if (u16_counter_step_pad_a(a0 + 0x2a,1,1) == 0) return 0;
      BR2_W32(0x801c15f0, 1);
      av = *(int16_t *)(s3 + 0x2a);
      if (av == 0)      dbg_marker_mode_set(0);
      else if (av == 1) dbg_marker_mode_set(3);
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
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x102:
    CAA8_E58C(0x8012136c,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x103:
    CAA8_E58C(0x80121380,0x1000,flag);
    sprintf_core(buf,(char *)guest_at(0x801212d0),
                 (int32_t)*(int16_t *)(s3 + 0x18) + 1);
    announce_param_block_init(ctx8,ctxc,(const void *)buf,0x160,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x104:
    CAA8_E58C(0x8012138c,0x1000,flag);
    sprintf_core(buf,(char *)guest_at(0x801212d0),
                 (int32_t)*(int16_t *)(s3 + 0x1a) + 1);
    announce_param_block_init(ctx8,ctxc,(const void *)buf,0x160,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x105:
    CAA8_E58C(0x8012139c,0x1000,flag);
    announce_param_block_init(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b4060u +
                     (uint32_t)*(int16_t *)(s3 + 0x1c) * 4)),
                 0x160,0x1000,flag);
    if (*(int16_t *)(s3 + 0x1c) == 4) {
      scroll_offset_accumulate(0,0xc);
      return 0;
    }
    round_announce_block(ctx8,ctxc,(const void *)guest_at(0x801213a8),
                 0x180,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x106:
    CAA8_E58C(0x801213b0,0x1000,flag);
    sprintf_core(buf,(char *)guest_at(0x801212d0),
                 (int32_t)*(int16_t *)(s3 + 0x1e) + 1);
    announce_param_block_init(ctx8,ctxc,(const void *)buf,0x160,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x107:
    CAA8_E58C(0x801213bc,0x1000,flag);
    announce_param_block_init(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b4074u +
                     (uint32_t)*(int16_t *)(s3 + 0x20) * 4)),
                 0x160,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x108:
    CAA8_E58C(0x801213d0,0x1000,flag);
    announce_param_block_init(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b407cu +
                     (uint32_t)*(int16_t *)(s3 + 0x22) * 4)),
                 0x160,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x109:
    CAA8_E58C(0x801213e0,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x10a:
    CAA8_E58C(0x801213f0,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x10b:
    CAA8_E58C(0x80121404,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x10c:
    CAA8_E58C(0x80121410,0x1000,flag);
    announce_param_block_init(ctx8,ctxc,
                 (const void *)guest_at(g32(0x801b3f70u +
                     (uint32_t)*(int16_t *)(s3 + 0x2a) * 4)),
                 0x160,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x10d:
    CAA8_E58C(0x80121310,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  case 0x10e:
    CAA8_E58C(0x80121318,0x1000,flag);
    scroll_offset_accumulate(0,0xc);
    return 0;
  default:
    return 0;
  }
#undef CAA8_E58C
#undef CAA8_STATUS
#undef CAA8_GATE
}

/* ---- save_state_machine_alt @0x80171890 (526 insns) ----
   Sibling of save_load_state_machine: same heap/tag machinery, different state
   layout. fp = a0 guest state ptr (h@+0x8e), s4 = a1 guest param block
   (lbu@2,3,4; ptr@0x28,0x2c), a2 = gate flag. States 0/0x10/0x20/0x30/0x40. */
int save_state_machine_alt(uint32_t a0, uint8_t *s4, int32_t a2)
{
  uint8_t *fp = (uint8_t *)guest_at(a0);
  int32_t s0 = (int32_t)*(int16_t *)(fp + 0x8e);
  uint32_t s5 = s4[2];
  uint32_t s2 = s4[4] & 3;
  uint32_t s6 = g8(0x801b49f8u + s4[3]);
  /* DIAG: log each entry — resume state (fp+0x8e) in dst, s4 block in src,
     char index s4[3] in mode, s2 (s4[4]&3) in len. */
  decomp_trace_record(86,(uint32_t)s0,(uint32_t)s4,(uint8_t)s4[3],s2);
  int32_t saved8;
  /* guest sp+0x28 word — heap_record_emit param_6 out-count is u32* */
  uint32_t sp28 = 0;
  uint32_t heapB, heapA, sz, s1v, p, v1, i;
  int *tbl = (int *)tag_table_get();   /* tag table; pure getter */
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
  decomp_trace_record(81,(uint32_t)(s6 + 0xc4),s6,(uint8_t)s4[3],s2); /* L00 gate */
  if (cd_file_load_sm((int32_t)(s6 + 0xc4), heapB) == 0) goto ret0;
  if (s5 == 0) {
    fighter_record_apply(heapB, 3);
    page_table_ptr_set(s4[3], 3);
  } else {
    fighter_record_apply(heapB, 4);
    page_table_ptr_set(s4[3], 4);
  }
  s0 = 0x10;
  goto ret0;                   /* gen 0x80171990 sh s0,0x8e(fp) stores 0x10, then j 0x801720cc ret 0 —
                                  the store must NOT be skipped or L00 reloads s6+0xc4 forever */

L10:
  /* state 0x10: build image (two variants on s5) */
  decomp_trace_record(82,(uint32_t)(s6 + s2 + 0xc0),s6,(uint8_t)s4[3],s2); /* L10 gate */
  if (cd_file_load_sm((int32_t)(s6 + s2 + 0xc0),
                   guest_of(tag_table_get())) == 0) goto ret0;
  saved8 = (int32_t)g8(0x1f800013);
  scratch_013_set(0);
  heapA = g32(0x801c47b8);
  BR2_W32(0x801e3cfc, 0x800f0000u);
  BR2_W32(0x801f4f18, heapA);

  /* tag0 + cc5c -> p+0x10 / p+0 */
  heapB = g32(0x801e3cfc);
  sz = heap_alloc54(heapB, tbl, 0);
  heap_ptr_fixup(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801beff0u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 0, 0);
  } else {
    s1v = 0x801c00d0u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 3, 3);
  }
  BR2_W32(0x801c47b8, heapA);
  p = *(uint32_t *)(s4 + 0x28);
  *(uint32_t *)guest_at(p + 0x10) = s1v;
  *(uint8_t *)guest_at(p) = sp28;
  BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);

  /* tag1 + cc5c -> p+0x14 / p+1 */
  heapB = g32(0x801e3cfc);
  sz = heap_alloc54(heapB, tbl, 1);
  heap_ptr_fixup(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801bf860u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0x81, 0, 0);
  } else {
    s1v = 0x801c0940u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0x81, 3, 3);
  }
  BR2_W32(0x801c47b8, heapA);
  p = *(uint32_t *)(s4 + 0x28);
  *(uint32_t *)guest_at(p + 0x14) = s1v;
  *(uint8_t *)guest_at(p + 1) = sp28;
  BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);

  /* tag2 + cc5c -> p+0x34 / p+0x24 */
  heapB = g32(0x801e3cfc);
  sz = heap_alloc54(heapB, tbl, 2);
  heap_ptr_fixup(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801bf428u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 0, 0);
  } else {
    s1v = 0x801c0508u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0, 3, 3);
  }
  BR2_W32(0x801c47b8, heapA);
  p = *(uint32_t *)(s4 + 0x28);
  *(uint32_t *)guest_at(p + 0x34) = s1v;
  *(uint8_t *)guest_at(p + 0x24) = sp28;
  BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);

  /* tag3 + cc5c -> p+0x38 / p+0x25 */
  heapB = g32(0x801e3cfc);
  sz = heap_alloc54(heapB, tbl, 3);
  heap_ptr_fixup(g32(0x801e3cfc));
  if (s5 == 0) {
    s1v = 0x801bfc98u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
                 g32(0x801c47b8), 0, 0, &sp28, 0x81, 0, 0);
  } else {
    s1v = 0x801c0d78u;
    heapA = (uint32_t)heap_record_emit(g32(0x801e3cfc), s1v,
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
    sz = heap_alloc54(g32(0x801e3cfc), tbl, 4);
    if (sz) {
      spr_draw_off((int32_t)g32(0x801e3cfc), (int32_t)b0, 0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag5 */
    sz = heap_alloc54(g32(0x801e3cfc), tbl, 5);
    if (sz) {
      spr_draw_off((int32_t)g32(0x801e3cfc), (int32_t)(b0 + 0x40), 0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag6 */
    sz = heap_alloc54(g32(0x801e3cfc), tbl, 6);
    if (sz) {
      spr_draw_off((int32_t)g32(0x801e3cfc), (int32_t)(b0 + 0x80), 0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag7: s3=0x1e0 */
    sz = heap_alloc54(g32(0x801e3cfc), tbl, 7);
    if (sz) {
      spr_draw_off((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1e0, 1);
      spr_draw_off((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1f0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
    /* tag8: s0 += 0x10 unconditionally (delay slot) */
    b1 += 0x10;
    sz = heap_alloc54(g32(0x801e3cfc), tbl, 8);
    if (sz) {
      spr_draw_off((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1e0, 1);
      spr_draw_off((int32_t)g32(0x801e3cfc), (int32_t)b1, 0x1f0, 1);
      BR2_W32(0x801e3cfc, g32(0x801e3cfc) + sz);
    }
  }
  cd_timer_read(0);
  scratch_013_set((uint32_t)saved8);
  s0 = 0x20;
  /* fall into L20 */

L20:
  /* state 0x20 */
  decomp_trace_record(83,(uint32_t)(s6 + 0x25),s6,(uint8_t)s4[3],s2); /* L20 gate */
  if (cd_file_load_sm((int32_t)(s6 + 0x25),
                   guest_of(tag_table_get())) == 0) goto ret0;
  saved8 = (int32_t)g8(0x1f800013);
  scratch_013_set(0);
  heapB = g32(0x801e3cfc);
  sz = heap_alloc54(heapB, tbl, 0);
  if (sz) {
    if (s5 == 0)
      spr_draw_pair(heapB, 0x60, 0x1e0, 1, 0x3c0, 0x140, 1);
    else
      spr_draw_pair(heapB, 0x70, 0x1e0, 1, 0x3c0, 0x140, 1);
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
      sz = heap_alloc54(h, tbl, (int32_t)i + 1);
      if (sz) {
        BR2_W32(0x801c47b8, g32(0x801c47b8) + sz);
        data_relocate(h);
      } else {
        BR2_W32(slot, 0);
      }
    }
  }
  cd_timer_read(0);
  scratch_013_set((uint32_t)saved8);
  s0 = 0x30;                    /* next state L30 — gen 0x80172020 li s0,0x30 -> ret0;
                                   storing 0x20 here would re-run L20 and reload s6+0x25 forever */
  goto ret0;

L30:
  /* state 0x30 */
  heapA = (g32(0x801c47b8) + 3) & ~3u;
  BR2_W32(0x801c47b8, heapA);
  decomp_trace_record(84,(uint32_t)(s6 + s2 + 0x20),s6,(uint8_t)s4[3],s2); /* L30 gate */
  if (cd_file_load_sm((int32_t)(s6 + s2 + 0x20), heapA) == 0) goto ret0;
  saved8 = (int32_t)g8(0x1f800013);
  scratch_013_set(0);
  heapA = g32(0x801c47b8);
  s4[0xb] = 0;
  *(uint32_t *)(s4 + 0x30) = heapA;
  /* DIAG: FC_ANI bind — record which fighter block gets which heap ptr. */
  { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    decomp_trace_record(63,guest_of(s4),guest_of(s4)+0x30,0,heapA); }
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
  decomp_trace_record(85,(uint32_t)(s6 + 0x24),s6,(uint8_t)s4[3],s2); /* L40 gate */
  if (cd_file_load_sm((int32_t)(s6 + 0x24), heapA) == 0) goto ret0;
  heapA = g32(0x801c47b8);
  sz = *(uint32_t *)guest_at(heapA);
  BR2_W32(0x801c47b8, heapA + sz);
  return 1;

ret0:
  /* every generated return-0 path stores s0 -> fp+0x8e first (0x80171910/
     0x80171928/0x80171990/0x801720C8 all sh s0,0x8e(fp) before j 0x801720cc);
     there is no store-less return-0, so ret_nc is removed. */
  *(int16_t *)(fp + 0x8e) = (int16_t)s0;
  return 0;
#undef H90W16
}

/* ======== 0x8018123c ======== */

/* slot_action_dump @0x8018123c (~716 insns). Per-slot action-record dump.
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
void slot_action_dump(uint32_t a0)
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
/* 0x8019e3d4 — save-flow pair thunk owned by cd.c (`ret_zero_f` there
 * jals 0x8019e420/0x8019e600, which resolve to the B0-svc bridges).
 * This file only needs the declaration for cd_tail_call_a; the old weak
 * `return 0` def here was dead code shadowed by cd.c's strong def. */
extern void ret_zero_f(void);

extern void *fn_lookup(uint32_t);
extern void task_invoke(uint32_t fn_guest, uint32_t arg_guest);
extern uint32_t call_guest(uint32_t,uint32_t,uint32_t);
extern int rotmat2_build();
extern int rotmat_rtps_build();
extern int angle_vec_build();
extern int angle_pair_rotmat();
extern uint  rand_lcg(void);
extern void tween_dur_spread();
extern void prim_attr_word_set();
extern uint32_t tween_ensure(int);
extern void obj_flag34_set(int);
extern void obj_flag34_clear(int);
extern void task_node_free(int *);
extern void cd_mode_apply_seq(uint32_t);
extern void cd_mode_reset_seq(void);
extern void bios_tramp_a070(uint32_t,uint32_t);
void cd_channel_init(void);

/* 0x801a0bb0 — CD reset trio: cd_mode_apply_seq(); cd_mode_reset_seq(); bios_tramp_a070(). */
void cd_reset_trio(void)
{
  cd_mode_apply_seq(0);
  cd_mode_reset_seq();
  bios_tramp_a070(0,0);
  return;
}

/* 0x801a0dbc — tail thunk to ret_zero_f. */
void cd_tail_call_a(void)
{
  ret_zero_f();
  return;
}

/* 0x801a1230 — tail thunk to cd_state_clear. */
void cd_tail_call_b(void)
{
  cd_state_clear();
  return;
}

/* 0x801a1270 — tail thunk to cd_channel_init. */
void cd_tail_call_c(void)
{
  cd_channel_init();
  return;
}

/* 0x801adca4 — rand() low16 store: *(int16_t*)a1 = (int16_t)rand(). */
void rand_lo16_store(uint32_t a0,int16_t *a1)
{
  (void)a0;
  *a1 = (int16_t)rand_lcg();
  return;
}

/* 0x801adccc — rand() word store: *(uint32_t*)a1 = rand(). */
void rand_word_store(uint32_t a0,uint32_t *a1)
{
  (void)a0;
  *a1 = rand_lcg();
  return;
}

/* 0x801aec10 — if (*a1) task_node_free(*a1). */
void op_guarded_call(uint32_t *a1)
{
  if (*a1 != 0) {
    task_node_free((int *)(uintptr_t)*a1);
  }
  return;
}

/* 0x801af3cc — v = tween_ensure(a1); *(u32*)(*gp4c + 0x14) = v.
 * $gp = 0x801beedc; +0x4c cell = guest 0x801bef28. */
void op_set_gp14(uint32_t a0,int32_t a1)
{
  (void)a0;
  *(uint32_t *)guest_at(*(uint32_t *)guest_at(0x801bef28) + 0x14) =
      tween_ensure(a1);
  return;
}

/* 0x801af3f8 — identical body to op_set_gp14 (alias entry). */
void op_set_gp14_alias(uint32_t a0,int32_t a1)
{
  (void)a0;
  *(uint32_t *)guest_at(*(uint32_t *)guest_at(0x801bef28) + 0x14) =
      tween_ensure(a1);
  return;
}

/* 0x801af494 — elem_pair_flag100(a1, a0[1]). */
void elem_pair_flag100_wrap(int16_t *a0,void *a1)
{
  elem_pair_flag100(a1,a0[1]);
  return;
}

/* 0x801af4bc — elem_pair_flag8(a1, a0[1]). */
void elem_pair_flag8_wrap(int16_t *a0,void *a1)
{
  elem_pair_flag8(a1,a0[1]);
  return;
}

/* 0x801af4e4 — elem_pair_field_set(a1, a0[1], a0[2], a0[3]). */
void elem_pair_field_set_wrap(int16_t *a0,void *a1)
{
  elem_pair_field_set(a1,a0[1],a0[2],a0[3]);
  return;
}

/* 0x801af518 — elem_flag100(a1, a0[1]). */
void elem_flag100_wrap(int16_t *a0,void *a1)
{
  elem_flag100(a1,a0[1]);
  return;
}

/* 0x801af540 — elem_flag8(a1, a0[1]). */
void elem_flag8_wrap(int16_t *a0,void *a1)
{
  elem_flag8(a1,a0[1]);
  return;
}

/* 0x801af568 — elem_field_set(a1, a0[1], a0[2], a0[3]). */
void elem_field_set_wrap(int16_t *a0,void *a1)
{
  elem_field_set(a1,a0[1],a0[2],a0[3]);
  return;
}

/* 0x801af59c — tween_dur_spread(a1, a0[1]). */
void op_wrap_a73e4(int16_t *a0,int32_t a1)
{
  tween_dur_spread(a1,a0[1]);
  return;
}

/* 0x801af5c4 — slot_field_clear(a1); a0 unused. */
void slot_field_clear_wrap(uint32_t a0,int32_t a1)
{
  (void)a0;
  slot_field_clear(a1);
  return;
}

/* 0x801af5e4 — obj_flag34_set(a1); a0 unused. */
void op_wrap_a7498(uint32_t a0,int32_t a1)
{
  (void)a0;
  obj_flag34_set(a1);
  return;
}

/* 0x801af604 — obj_flag34_clear(a1); a0 unused. */
void op_wrap_a74f0(uint32_t a0,int32_t a1)
{
  (void)a0;
  obj_flag34_clear(a1);
  return;
}

/* 0x801af678 — prim_attr_word_set(a1, a0[1]). */
void op_wrap_ac9b8(int16_t *a0,int32_t a1)
{
  prim_attr_word_set(a1,a0[1]);
  return;
}

/* 0x801af6a0 — fighter_chan_delta_accum(a1, a0[1], a0[2]) via fnmap.
 * Guest: a0=a1, a1/a2 = sign-extended s16 fields, a3 = caller passthrough.
 * Callee is clean (battle.c) — fn_lookup resolves it; the fallback calls
 * it directly too (never generated). */
extern void fighter_chan_delta_accum(uint32_t,uint32_t,uint32_t,uint32_t);
void op_wrap_acb08(int16_t *a0,uint32_t a1)
{
  void *fn = fn_lookup(0x801acb08);
  if (fn != (void *)0x0) {
    ((void (*)(uint32_t,int,int))fn)(a1,a0[1],a0[2]);
  }
  else {
    /* Unported callee: reproduce the jal — a0=a1, a1/a2 = lh a0[1]/a0[2]
     * (sign-extended), a3 = caller's live $a3. */
    extern CPUState *clean_cpu_state(void);
    CPUState *cpu = clean_cpu_state();
    fighter_chan_delta_accum(a1,
                       (uint32_t)(int32_t)a0[1],(uint32_t)(int32_t)a0[2],
                       cpu != NULL ? cpu->gpr[7] : 0u);
  }
  return;
}

/* 0x801af7d4 — mat3x3_build(a2[0], a2[1], a2[2], a1); a0 unused. */
void mat3x3_build_wrap(uint32_t a0,uint32_t a1,int16_t *a2)
{
  (void)a0;
  mat3x3_build(a2[0],a2[1],a2[2],a1);
  return;
}

/* 0x801af804 — rotmat2_build(*a2, a1); a0 unused. */
void *op_wrap_8da2c_a(uint32_t a0,uint32_t *a1,int16_t *a2)
{
  (void)a0;
  return (void *)rotmat2_build((uint)(int32_t)*a2,a1);
}

/* 0x801af828 — rotmat_rtps_build(*a2, a1); a0 unused. */
void *op_wrap_8de6c_a(uint32_t a0,uint32_t *a1,int16_t *a2)
{
  (void)a0;
  return (void *)rotmat_rtps_build((uint)(int32_t)*a2,a1);
}

/* 0x801af84c — angle_vec_build(*a2, a1); a0 unused. */
void *op_wrap_8e028_a(uint32_t a0,uint32_t *a1,int16_t *a2)
{
  (void)a0;
  return (void *)angle_vec_build((uint)(int32_t)*a2,a1);
}

/* 0x801af870 — angle_pair_rotmat(a2[0], a2[1], a2[2], a1); a0 unused. */
void *op_wrap_8dac0_a(uint32_t a0,uint32_t a1,int16_t *a2)
{
  (void)a0;
  return (void *)angle_pair_rotmat((uint)(int32_t)a2[0],(uint)(int32_t)a2[1],
                              (uint32_t *)guest_at((uint32_t)a1));
}

/* 0x801af89c — rotmat2_build(a0[1], a1). */
void *op_wrap_8da2c_b(int16_t *a0,uint32_t *a1)
{
  return (void *)rotmat2_build((uint)(int32_t)a0[1],a1);
}

/* 0x801af8c0 — rotmat_rtps_build(a0[1], a1). */
void *op_wrap_8de6c_b(int16_t *a0,uint32_t *a1)
{
  return (void *)rotmat_rtps_build((uint)(int32_t)a0[1],a1);
}

/* 0x801af8e4 — angle_vec_build(a0[1], a1). */
void *op_wrap_8e028_b(int16_t *a0,uint32_t *a1)
{
  return (void *)angle_vec_build((uint)(int32_t)a0[1],a1);
}

/* 0x801af908 — angle_pair_rotmat(a0[1], a0[2], a1). */
void *op_wrap_8dac0_b(int16_t *a0,uint32_t a1)
{
  return (void *)angle_pair_rotmat((uint)(int32_t)a0[1],(uint)(int32_t)a0[2],
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

extern int prim_group_build();
extern int anim_node_alloc();
extern int op_anim_delta_walk();
extern int op_node_alloc_init();
extern int *op_job_alloc_init(int32_t,int32_t,uint32_t,uint32_t,uint32_t);
extern int op_ctx_node_attach();
extern void op_job_submit_init();
extern int32_t dev_byte_xfer(uint8_t *,int32_t);
extern uint32_t cd_svc_50(uint32_t,uint32_t);
extern uint32_t cd_svc_4e(uint32_t,uint32_t,uint32_t);
extern int fx_group_alloc(uint32_t,uint32_t,int,int);
extern uint32_t tween_chan_alloc(int,int);
extern void tween_seek(int,int);
extern int lut_sin_q12();
extern int lut_cos_q12();
extern int task_alloc_wrap_f(uint32_t,uint32_t,uint32_t,int);
extern int angle_from_xy(int,int);
extern uint32_t noop_c(int);
extern void node_tree_free(int);
extern void bios_tramp_c003(uint32_t,uint32_t);
extern void FUN_8018e300(void);
extern void FUN_8018e310(void);
extern void task_slot_step();
extern uint32_t guest_of(const void *);

/* 0x801a0620 — cd_svc_50(a0,a1); cd_svc_4e(a0,0x3f,0). */
void cd_svc_dual(uint32_t a0,uint32_t a1)
{
  cd_svc_50(a0,a1);
  cd_svc_4e(a0,0x3f,0);
  return;
}

/* 0x801a1b0c — CD channel init: crit-in, mode 3/1, table 0x801fefc0,
 * crit-out. Faithful port of generated 0x801A1B0C-0x801A1B48:
 *   FUN_8018e300();                          // EnterCriticalSection
 *   a0=3, a1=1; jal 0x8019a670;              // bios_kobj_tramp C0/0x0a —
 *     // the guest stub forwards caller a0-a3 untouched, so stage them on
 *     // the live CPUState for br2_bios_tramp's gpr read.
 *   a0=2, a1=0x801FEFC0; jal 0x8019a340;     // bios_tramp_c003 -> C0/0x03
 *   FUN_8018e310();                           // ExitCriticalSection
 * a2/a3 are never written by the guest body, so they stay whatever the
 * kobj service left in $a2/$a3 — matching bios_svc's live-gpr read. */
extern CPUState *clean_cpu_state(void);
void cd_channel_init(void)
{
  CPUState *cpu = clean_cpu_state();
  FUN_8018e300();
  if (cpu != NULL) { cpu->gpr[4] = 3; cpu->gpr[5] = 1; }
  bios_kobj_tramp();
  bios_tramp_c003(2,0x801fefc0);
  FUN_8018e310();
}

/* 0x801a30c0 — ctx fn-ptr call + dev_byte_xfer(ctx,-2). */
void ctx_fn_send(uint8_t *a0)
{
  uint32_t fn_g = *(uint32_t *)guest_at(0x801be798);
  void *fn = fn_lookup(fn_g);
  uint32_t r = 0;
  if (fn != (void *)0x0) {
    r = ((uint32_t (*)(uint32_t))fn)(guest_of(a0));
  }
  else {
    /* Guest jalr is unconditional: a0 = desc guest addr, a1 = caller's
     * live $a1 (jalr passthrough), a2/a3 likewise untouched. */
    CPUState *cpu = clean_cpu_state();
    r = call_guest(fn_g,guest_of(a0),cpu != NULL ? cpu->gpr[5] : 0u);
  }
  *(uint32_t *)guest_at(0x801be800) = r;
  *(uint8_t *)guest_at(*(uint32_t *)(a0 + 0x3c)) = 0;
  dev_byte_xfer(a0,-2);
  return;
}

/* 0x801aadc4 — deep-null check then task_slot_step(a0). */
void op_ctx_guard_call(uint8_t *a0)
{
  uint32_t *p4 = (uint32_t *)(a0 + 4);
  /* *p4 and p2[3] are 4-byte guest cells holding guest addresses */
  uint32_t *p2 = (uint32_t *)guest_at(*p4);
  if (*(uint32_t *)guest_at(*(uint32_t *)(p2 + 3)) == 0) {
    uint32_t *q = (uint32_t *)(a0 + 0xc);
    *q = 0;
    task_slot_step();
  }
  return;
}

/* 0x801ad298 — *(*a0)=0; if (a0[1]->f10) { a0[3]=a0[1]+f10; call 0x801b0db8(a0); } */
void op_list_head_reset(uint32_t *a0)
{
  *(uint32_t *)guest_at(a0[0]) = 0;
  {
    uint32_t p4 = a0[1];
    uint32_t v1 = *(uint32_t *)guest_at(p4 + 0x10);
    if (v1 != 0) {
      a0[3] = p4 + v1;
      {
        /* Guest jal 0x801B0DB8 (unconditional inside the v1!=0 branch):
         * a0 = struct guest addr, a1 = caller's live $a1. */
        void *fn = fn_lookup(0x801b0db8);
        if (fn != (void *)0x0) {
          ((void (*)(uint32_t))fn)(guest_of(a0));
        }
        else {
          CPUState *cpu = clean_cpu_state();
          call_guest(0x801b0db8u,guest_of(a0),cpu != NULL ? cpu->gpr[5] : 0u);
        }
      }
    }
  }
  return;
}

/* 0x801ad6f0 — ctx->fC = GP50 + a0[0]; call 0x801b0db8(ctx); *a1 += 4. */
void op_ctx_fc_set(int16_t *a0,uint32_t *a1)
{
  uint32_t s0 = *a1;
  uint32_t v = BR2_GP50 + (uint32_t)(int32_t)a0[0];
  BR2_GPF(0xc) = v;
  {
    /* Guest jal 0x801B0DB8: a0 = ctx ptr (BR2_GPC), a1 = caller's $a1
     * (= guest_of(a1) here — the wrapper passes gpr[5] through). */
    void *fn = fn_lookup(0x801b0db8);
    if (fn != (void *)0x0) {
      ((void (*)(uint32_t))fn)(BR2_GPC);
    }
    else {
      call_guest(0x801b0db8u,BR2_GPC,guest_of(a1));
    }
  }
  *a1 = s0 + 4;
  return;
}

/* 0x801adc60 — *a1 = (int8_t)(rand() magic-div). */
void op_rand_i8(uint32_t a0,int8_t *a1)
{
  (void)a0;
  {
    int32_t r = (int32_t)rand_lcg();
    int32_t hi = (int32_t)(((int64_t)r * 266338049LL) >> 32);
    int32_t q = (hi >> 3) - (r >> 31);
    *a1 = (int8_t)q;
  }
  return;
}

/* 0x801ae85c — if (*a1==0) *a1 = op_node_alloc_init(a0[0..3], a1). */
void op_cached_node_alloc(int16_t *a0,uint32_t a1)
{
  if (*(uint32_t *)guest_at(a1) == 0) {
    int32_t r = op_node_alloc_init(a0[0],a0[1],a0[2],a0[3],a1);
    *(uint32_t *)guest_at(a1) = (uint32_t)r;
  }
  return;
}

/* 0x801ae8a8 — return fx_group_alloc(a0[0..3]). */
int op_call_ad38c(int16_t *a0)
{
  return fx_group_alloc(a0[0],a0[1],a0[2],a0[3]);
}

/* 0x801ae8dc — *a1 = fx_group_alloc(a0[0..3]). */
void op_ad38c_store(int16_t *a0,uint32_t *a1)
{
  *a1 = (uint32_t)fx_group_alloc(a0[0],a0[1],a0[2],a0[3]);
  return;
}

/* 0x801ae91c — *a1 = fx_group_alloc(a0[0], a0[1], ctx8[a0[2]], a0[3]). */
void op_ad38c_ctx_store(int16_t *a0,uint32_t *a1)
{
  uint32_t b = BR2_GPF(8) + (uint32_t)(int32_t)a0[2];
  *a1 = (uint32_t)fx_group_alloc(a0[0],a0[1],
                              *(uint32_t *)guest_at(b),a0[3]);
  return;
}

/* 0x801aeff8 — printf_forward(0x801cee8, tbl[a0[1]][a0[0]], 0x801c0000). */
void op_trace_print_a(int16_t *a0)
{
  uint32_t b = BR2_GPF(8) + (uint32_t)(int32_t)a0[0];
  uint32_t inner = *(uint32_t *)guest_at(b);
  uint32_t a1 = *(uint32_t *)guest_at(inner + (uint32_t)(int32_t)a0[1] * 4);
  printf_forward((const char *)guest_at(0x801cee8),a1,0x801c0000);
  return;
}

/* 0x801af040 — same with cell 0x801ceec. */
void op_trace_print_b(int16_t *a0)
{
  uint32_t b = BR2_GPF(8) + (uint32_t)(int32_t)a0[0];
  uint32_t inner = *(uint32_t *)guest_at(b);
  uint32_t a1 = *(uint32_t *)guest_at(inner + (uint32_t)(int32_t)a0[1] * 4);
  printf_forward((const char *)guest_at(0x801ceec),a1,0x801c0000);
  return;
}

/* 0x801af094 — cached lookup: if (*a1==0) *a1 = prim_group_build(a1, T[a0[1]][a0[2]], a0[3], a0[4]); GPF(0x14)=1 on hit. */
void op_cached_lookup(int16_t *a0,uint32_t a1)
{
  BR2_GPF(0x14) = 0;
  if (*(uint32_t *)guest_at(a1) == 0) {
    uint32_t t1 = *(uint32_t *)guest_at(0x801ff638 + (uint32_t)(int32_t)a0[1] * 4);
    uint32_t av = *(uint32_t *)guest_at(t1 + (uint32_t)(int32_t)a0[2] * 4);
    uint32_t r = (uint32_t)prim_group_build(a1,(int)av,a0[3],a0[4]);
    *(uint32_t *)guest_at(a1) = r;
    if (r != 0) {
      BR2_GPF(0x14) = 1;
    }
  }
  return;
}

/* 0x801af114 — variant: a1arg = ctx8[a3[2]] then T[a3[1]][a1arg]. */
void op_cached_lookup_b(int16_t *a0,uint32_t a1)
{
  uint32_t idx = BR2_GPF(8) + (uint32_t)(int32_t)a0[2];
  uint32_t mid = *(uint32_t *)guest_at(idx);
  BR2_GPF(0x14) = 0;
  if (*(uint32_t *)guest_at(a1) == 0) {
    uint32_t t1 = *(uint32_t *)guest_at(0x801ff638 + (uint32_t)(int32_t)a0[1] * 4);
    uint32_t av = *(uint32_t *)guest_at(t1 + mid * 4);
    uint32_t r = (uint32_t)prim_group_build(a1,(int)av,a0[3],a0[4]);
    *(uint32_t *)guest_at(a1) = r;
    if (r != 0) {
      BR2_GPF(0x14) = 1;
    }
  }
  return;
}

/* 0x801af1a4 — variant: mid = ctx8[a0[1]] then T[mid][a0[2]]. */
void op_cached_lookup_c(int16_t *a0,uint32_t a1)
{
  uint32_t idx = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
  uint32_t mid = *(uint32_t *)guest_at(idx);
  BR2_GPF(0x14) = 0;
  if (*(uint32_t *)guest_at(a1) == 0) {
    uint32_t t1 = *(uint32_t *)guest_at(0x801ff638 + mid * 4);
    uint32_t av = *(uint32_t *)guest_at(t1 + (uint32_t)(int32_t)a0[2] * 4);
    uint32_t r = (uint32_t)prim_group_build(a1,(int)av,a0[3],a0[4]);
    *(uint32_t *)guest_at(a1) = r;
    if (r != 0) {
      BR2_GPF(0x14) = 1;
    }
  }
  return;
}

/* 0x801af238 — free ctx8[a0[0]] cell via node_tree_free. */
void op_ctx8_cell_free(int16_t *a0)
{
  uint32_t s0 = BR2_GPF(8) + (uint32_t)(int32_t)a0[0];
  uint32_t p = *(uint32_t *)guest_at(s0);
  if (p != 0) {
    node_tree_free((int)p);
    *(uint32_t *)guest_at(s0) = 0;
  }
  return;
}

/* 0x801af284 — double-indirect variant of op_ctx8_cell_free. */
void op_ctx8_cell_free_b(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t s0 = b + *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[0]);
  uint32_t p = *(uint32_t *)guest_at(s0);
  if (p != 0) {
    node_tree_free((int)p);
    *(uint32_t *)guest_at(s0) = 0;
  }
  return;
}

/* 0x801af2dc — GPF(0x14) = anim_node_alloc(a1, a0[1]). */
void op_gpf14_anim_alloc(int16_t *a0,int32_t a1)
{
  BR2_GPF(0x14) = (uint32_t)anim_node_alloc(a1,a0[1]);
  return;
}

/* 0x801af310 — GPF(0x14) = anim_node_alloc(a1, ctx8[a0[1]]). */
void op_gpf14_anim_alloc_b(int16_t *a0,uint32_t a1)
{
  uint32_t av = *(uint32_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  BR2_GPF(0x14) = (uint32_t)anim_node_alloc(a1,(int)av);
  return;
}

/* 0x801af354 — GPF(0x14) = tween_chan_alloc(a1, a0[1]). */
void op_gpf14_a72a0(int16_t *a0,int32_t a1)
{
  BR2_GPF(0x14) = tween_chan_alloc(a1,a0[1]);
  return;
}

/* 0x801af388 — GPF(0x14) = tween_chan_alloc(a1, ctx8[a0[1]]). */
void op_gpf14_a72a0_b(int16_t *a0,uint32_t a1)
{
  uint32_t av = *(uint32_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  BR2_GPF(0x14) = tween_chan_alloc(a1,(int)av);
  return;
}

/* 0x801af424 — op_anim_delta_walk(a1, (int16_t)ctx8[a0[1]]). */
int op_call_a984c(int16_t *a0,uint32_t a1)
{
  int16_t v = *(int16_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  return op_anim_delta_walk(a1,v);
}

/* 0x801af45c — tween_seek(a1, (int16_t)ctx8[a0[1]]). */
void op_call_a99e0(int16_t *a0,uint32_t a1)
{
  int16_t v = *(int16_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  tween_seek(a1,v);
  return;
}

/* 0x801af624 — ctx8[a0[1]]==0 ? obj_flag34_clear(a1) : obj_flag34_set(a1). */
void op_pick_a74xx(int16_t *a0,int32_t a1)
{
  int16_t t = *(int16_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  if (t == 0) {
    obj_flag34_clear(a1);
  } else {
    obj_flag34_set(a1);
  }
  return;
}

/* 0x801af6d0 — *(*(a1)+0x48) = op_ctx_node_attach(a1, ctx8[a0[1]]); a1 is guest addr. */
void op_node_attach_store(int16_t *a0,uint32_t a1)
{
  uint32_t d = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
  uint32_t av = *(uint32_t *)guest_at(d);
  uint32_t r = (uint32_t)op_ctx_node_attach(a1,(int)av);
  *(uint32_t *)guest_at(a1 + 0x48) = r;
  return;
}

/* 0x801af710 — if (a1) mat3x3_build(ctx8[a0[1]] s16 x3, *a1). */
void op_mat3_build_opt(int16_t *a0,uint32_t a1)
{
  if (a1 != 0) {
    uint32_t d = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
    uint8_t *p = guest_at(d);
    int16_t t0 = *(int16_t *)p;
    int16_t t1 = *(int16_t *)(p + 2);
    int16_t t2 = *(int16_t *)(p + 4);
    mat3x3_build(t0,t1,t2,*(uint32_t *)guest_at(a1));
  }
  return;
}

/* 0x801af758 — task_alloc_wrap_f(a1, ctx8[a0[1]]|0, GP50+a0[2], GP50+a0[3]); store. */
void op_call_38040(int16_t *a0,uint32_t a1)
{
  uint32_t b = BR2_GP50;
  uint32_t a2 = b + (uint32_t)(int32_t)a0[2];
  uint32_t a3 = b + (uint32_t)(int32_t)a0[3];
  int32_t r;
  if (a0[1] != 0) {
    uint32_t s0 = BR2_GPF(8) + (uint32_t)(int32_t)a0[1];
    r = task_alloc_wrap_f(a1,s0,a2,a3);
    *(uint32_t *)guest_at(s0) = (uint32_t)r;
  } else {
    r = task_alloc_wrap_f(a1,0,a2,a3);
  }
  BR2_GPF(0x14) = (uint32_t)r;
  return;
}

/* 0x801afad0 — ctx8[a0[1]]->[a0[2]] pair -> angle_from_xy(f0,f8); store ctx8[a0[0]]. */
void op_pair_f0f8_store(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t inner = *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[1]);
  uint32_t e = inner + (uint32_t)(int32_t)a0[2] * 4;
  uint32_t d = b + (uint32_t)(int32_t)a0[0];
  uint32_t r = (uint32_t)angle_from_xy(*(uint32_t *)guest_at(e),
                                     *(uint32_t *)guest_at(e + 8));
  *(uint32_t *)guest_at(d) = r;
  return;
}

/* 0x801afa30 — s16 variant: ctx8[a0[1]]->[a0[2]] s16 -> lut_sin_q12; store ctx8[a0[0]]. */
void op_s16_d29c_store(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t inner = *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[1]);
  int16_t v = *(int16_t *)guest_at(inner + (uint32_t)(int32_t)a0[2] * 2);
  uint32_t d = b + (uint32_t)(int32_t)a0[0];
  *(uint32_t *)guest_at(d) = (uint32_t)lut_sin_q12(v);
  return;
}

/* 0x801afa80 — same with lut_cos_q12. */
void op_s16_d298_store(int16_t *a0)
{
  uint32_t b = BR2_GPF(8);
  uint32_t inner = *(uint32_t *)guest_at(b + (uint32_t)(int32_t)a0[1]);
  int16_t v = *(int16_t *)guest_at(inner + (uint32_t)(int32_t)a0[2] * 2);
  uint32_t d = b + (uint32_t)(int32_t)a0[0];
  *(uint32_t *)guest_at(d) = (uint32_t)lut_cos_q12(v);
  return;
}

/* 0x801aecdc — if (*a2) { s0=ctx8[*a2]; r=(*a1)(); *(u32*)s0=r } else (*a1)(). */
void op_cond_call_store(uint32_t a0,uint32_t a1,int16_t *a2)
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
      else {
        /* Guest jalr: a0/a1 = incoming regs (a0 raw, a1 = fp-cell addr),
         * a2/a3 passthrough — call_guest leaves gpr[6]/gpr[7] live. */
        r = call_guest(fp,a0,a1);
      }
      *(uint32_t *)guest_at(s0) = r;
    } else if (fn != (void *)0x0) {
      ((void (*)(uint32_t))fn)(a0);
    }
    else {
      call_guest(fp,a0,a1);
    }
  }
  return;
}

/* 0x801aebd0 — r=noop_c(*a1); *(u32*)(r + a0[1]*4) = *(u32*)(a0+6). */
void op_field_store_idx(uint8_t *a0,uint32_t *a1)
{
  uint32_t r = noop_c(*a1);
  uint32_t d = r + ((uint32_t)(int32_t)*(int16_t *)(a0 + 2) << 2);
  *(uint32_t *)guest_at(d) = *(uint32_t *)(a0 + 6);
  return;
}

/* ======== batch34: 0x801a0a/0x801a18/0x801a51/0x801a6b/0x801a9b/0x801ac/0x801ad/0x801ae ======== */

extern void text_win_printf();
extern int *task_node_alloc(int,uint32_t,uint,int,int,int);
extern void tween_apply(short *,int,int);
extern void gp_20_store_default(void);
extern int prim_group_build();

/* 0x801a1834 — ctx flag gate + indirect callback; returns 1/0.
 * Faithful port of generated 0x801A1834-0x801A1898:
 *   ctx = *0x801BE7E0;   // lui 0x801C + lw -6176 -> 0x801BE7E0 (=0x1F801070)
 *   if (!(*(ctx+4) & 1)) return 0;   // beqz -> epilogue, v0=0
 *   if (!(*ctx     & 1)) return 0;
 *   fp  = *0x801BE7A8;   // lui 0x801C + lw -6232 -> 0x801BE7A8
 *   if (fp != 0) jalr fp;            // caller's a0-a3 forwarded untouched
 *   return 1;                        // v0 forced to 1 on both tail paths
 * The jalr target is a guest fn ptr: call_guest runs the clean wrapper or
 * nested-dispatches generated, and only writes $a0/$a1 — pass the live
 * values through so the callee sees the caller's regs; $a2/$a3 pass
 * through untouched exactly like the guest jalr. No IRQ wait, no loop —
 * clean C is faithful here (ctx flags are plain MMIO reads). */
extern CPUState *clean_cpu_state(void);
extern uint32_t br2_ld32(uint32_t a);
int op_flag_fn_check(void)
{
  /* ctx resolves to the SIO status reg 0x1F801070 — MMIO, so the ctx+4/ctx
   * flag reads must go through br2_ld32 (guest_at only backs RAM/scratch). */
  uint32_t ctx = *(uint32_t *)guest_at(0x801be7e0);
  uint32_t fp;
  if ((br2_ld32(ctx + 4) & 1) == 0) return 0;
  if ((br2_ld32(ctx) & 1) == 0) return 0;
  fp = *(uint32_t *)guest_at(0x801be7a8);
  if (fp != 0) {
    CPUState *cpu = clean_cpu_state();
    uint32_t a0 = cpu ? cpu->gpr[4] : 0;
    uint32_t a1 = cpu ? cpu->gpr[5] : 0;
    (void)call_guest(fp, a0, a1);
  }
  return 1;
}

/* 0x801a2fb4 — state-byte dispatch; returns 1 when a0[0x46]==2. */
int op_state_dispatch(uint8_t *a0)
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
    else {
      /* Guest jalr is unconditional on this path: a0 = desc guest addr,
       * a1 = caller's live $a1. */
      CPUState *cpu = clean_cpu_state();
      call_guest(fp,guest_of(a0),cpu != NULL ? cpu->gpr[5] : 0u);
    }
  }
  return 0;
}

/* cd_job_slot_alloc lives in cd.c (disasm-verified; counter @0x801be768). */

/* 0x801a0a8c — invoke channel fn ptr; decrement count on success. */
void op_chan_fn_invoke(void)
{
  int32_t i = *(int32_t *)guest_at(0x801be768);
  if (i >= 0) {
    uint32_t f = *(uint32_t *)guest_at(0x801fef60 + (uint32_t)i * 4);
    /* jalr through the slot fn ptr: run clean when ported, generated
     * otherwise — fn_lookup alone would silently skip generated targets. */
    uint32_t r = call_guest(f, 0x801fef20 + (uint32_t)i * 16, 0);
    if (r != 0) {
      *(uint32_t *)guest_at(0x801be768) = (uint32_t)(i - 1);
    }
  }
  return;
}

/* 0x801a6b0c — stash a1/a2 into gp globals, gp_20_store_default, submit job. */
int op_job_submit_ab(uint32_t a0,uint32_t a1,uint32_t a2)
{
  int *r;
  *(uint32_t *)guest_at(0x801bef18) = a1;
  *(uint32_t *)guest_at(0x801bef1c) = a2;
  *(uint32_t *)guest_at(0x801bef24) = a1;
  *(uint32_t *)guest_at(0x801bef00) = 0;
  gp_20_store_default();
  r = task_node_alloc(a0,2,0,0x801a6b70,0,0);
  if (r != (int *)0x0) {
    return 1;
  }
  return -1;
}

/* 0x801a9bc0 — validate ctx chain then tween_apply(node). */
void op_ctx_validate_call(uint8_t *a0)
{
  if (a0 != (uint8_t *)0x0) {
    uint32_t p = *(uint32_t *)(a0 + 0x3c);
    uint32_t qg = *(uint32_t *)guest_at(p);
    uint8_t *q = (uint8_t *)guest_at(qg);
    if (qg != 0 &&
        *(int16_t *)(q + 0xe) == (int32_t)*(uint32_t *)(q + 0x30) &&
        *(int16_t *)(q + 0xa) == (int32_t)*(uint32_t *)(q + 0x2c)) {
      tween_apply((short *)q,0,0);
    }
  }
  return;
}

/* 0x801aec40 — lazy ctx8[a0[3]] fill via op_job_alloc_init. */
void op_ctx8_lazy_fill(int16_t *a0)
{
  uint32_t s0 = BR2_GPF(8) + (uint32_t)(int32_t)a0[3];
  if (*(uint32_t *)guest_at(s0) == 0) {
    uint32_t stack5 = BR2_GP50 + (uint32_t)(int32_t)a0[2];
    int *jr = op_job_alloc_init(a0[0],a0[1],BR2_GPC,s0,stack5);
    *(uint32_t *)guest_at(s0) = (uint32_t)(uintptr_t)jr;
  }
  return;
}

extern int *op_job_alloc_init(int32_t,int32_t,uint32_t,uint32_t,uint32_t);

/* 0x801acf34 — four text_win_printf dumps of a0 fields. */
void op_dbg_dump4(uint32_t *a0)
{
  text_win_printf((const char *)guest_at(0x80126098),a0[0]);
  text_win_printf((const char *)guest_at(0x801260a8),a0[2]);
  text_win_printf((const char *)guest_at(0x801260b8),a0[8]);
  text_win_printf((const char *)guest_at(0x801260c8),a0[1]);
  return;
}

/* 0x801a5160 — five text_win_printf field dumps. */
void op_dbg_dump5(uint32_t a0,uint32_t a1)
{
  (void)a0;
  text_win_printf((const char *)guest_at(0x80125fd8),a1);
  text_win_printf((const char *)guest_at(0x80125fe8),*(uint32_t *)guest_at(a1));
  text_win_printf((const char *)guest_at(0x80126044),*(uint32_t *)guest_at(a1 + 4));
  text_win_printf((const char *)guest_at(0x80126054),*(uint32_t *)guest_at(a1 + 8) + 0x10);
  text_win_printf((const char *)guest_at(0x80126064),*(int16_t *)guest_at(a1 + 0xc));
  return;
}

/* 0x801a51d0 — table walk + text_win_printf dump (variadic tail args a1,a2,a3). */
void op_dbg_dump_tbl(uint32_t a0,uint32_t a1)
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
  text_win_printf((const char *)guest_at(0x80126074),a1,off,first);
  return;
}

/* 0x801ad4ac — task_node_alloc job submit; init result node from a2/a3. */
void op_job_submit_init(int32_t a0,int32_t a1,uint32_t a2,uint32_t a3)
{
  int *r = task_node_alloc(a0,a1,4,0x801b0db8,0,0x1c);
  if (r != (int *)0x0) {
    /* r is the node's GUEST address (task_node_alloc returns guest-addr-as-
     * pointer); node+0x10 cell holds the guest addr of the inline data. */
    uint32_t *p = (uint32_t *)guest_at(
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)r + 0x10));
    p[0] = 0;
    p[2] = *(uint32_t *)guest_at(a2 + 8);
    p[3] = a3;
    p[1] = *(uint32_t *)guest_at(a2 + 4);
  }
  return;
}

/* 0x801aea20 — r=noop_c(*a1); r[1]=e[0]; r[2]=e[1] (e=ctx8[a0[1]]+a0[2]*4). */
void op_pair_store(int16_t *a0,uint32_t *a1)
{
  uint32_t r = noop_c(*a1);
  uint32_t inner = *(uint32_t *)guest_at(BR2_GPF(8) + (uint32_t)(int32_t)a0[1]);
  uint32_t e = inner + (uint32_t)(int32_t)a0[2] * 4;
  *(uint32_t *)guest_at(r + 4) = *(uint32_t *)guest_at(e);
  *(uint32_t *)guest_at(r + 8) = *(uint32_t *)guest_at(e + 4);
  return;
}

/* ======== batch35: 0x801a4b/0x801a50/0x801a53/0x801a86/0x801aa3/0x801ad4/0x801ad5/0x801aed ======== */

/* guard_callee_a0/a1/b0/b1/c0/c1/d1/d0 — the guarded-dispatch targets
 * (guest 0x801a5bd0/6538/5d90/66dc/5f50/6350/6150/6880). Faithful clean
 * ports: each is a ~0x1c4-0x1d4-byte GTE emitter — index stream ->
 * lwc2/mtc2 vertex loads -> RTPT/NCLIP -> SXY/SZ stores -> MAC0/H-gated
 * ordering-table insert. Pure computation over guest RAM + COP2 regs:
 * no calls, no blocking waits, no live guest sp, no IRQ/deferred-
 * coroutine dependency -> portable per the delegation rule.
 *
 * Args arrive as guest values; a2 is already a guest address (the macro
 * resolves the host object pointer via guest_of). a3 is the caller's
 * passthrough $a3 on the v1==0 path and the sign-extended slot index v1
 * on the v1!=0 path — dead inside the guest bodies (never read).
 *
 * Shared shape (verified insn-by-insn against the disc bytes +
 * generated/SLES_017.22_full_50.c):
 *   prologue spills ra/s0-s7 to scratchpad 0x1f8003dc-0x1f8003fc (the
 *   epilogue reloads feed only callee-saved regs the clean body never
 *   touches — cpu->gpr already holds the caller's values — so only the
 *   stores are observable and are reproduced by guard_ctx_save).
 *   a1 <<= 2; s0 = *(a2+a1*4); s4 = *(a2+a1*4+8); s1 = *(a2+0x10)
 *   (vertex table); s2 = *(a2+0x18) (index stream); s3 = *(a2+0x24)
 *   (flags); s5 = lh(a2+0x30) (signed count).
 *   Per record: RTPT projects V0-V2 (V0 arrives via mtc2 of the a1/a2
 *   word pair saved from the previous iteration — first iteration keeps
 *   the pre-loop lwc2 loads), NCLIP, SXY0-2 -> record, MAC0 sign /
 *   flags&0x200 gate, then an OT insert:
 *     indexed variant (a0/b0/c0/d0 — FN_A): slot = a0 + (SZ3>>2)*4 with
 *       the 10 <= idx < 0x800 window;
 *     fixed variant   (a1/b1/c1/d1 — FN_B): push-front through *a0.
 *   Both chain a second record (tag|0x1000000) when flags&0x100.
 *   Guest v0 at jr ra = 0x00ffffff (the mask constant) — returned. */

#define GTE_DREG(n) ((n) * 0x800)   /* COP2 data-reg field encoding (bind.c) */
extern void setCopReg(int,int,uint32_t);
extern int  getCopReg(int,int);
extern void copFunction(int,uint32_t);
extern CPUState *clean_cpu_state(void);

/* lwc2 gte[reg], off(gaddr) — guest word load into a GTE data register.
 * Same routing as gfx.c's gte_lwc2: setCopReg -> gte_write_data, exactly
 * what the generated emitter calls for every MTC2/LWC2. */
static uint32_t guard_lwc2(uint32_t gaddr,int reg)
{
  uint32_t v = *(uint32_t *)guest_at(gaddr);
  setCopReg(2, GTE_DREG(reg), v);
  return v;
}

/* swc2 gte[reg], off(gaddr) — GTE data register store to guest RAM.
 * getCopReg -> gte_read_data, same value the generated emitter produces. */
static void guard_swc2(uint32_t gaddr,int reg)
{
  *(uint32_t *)guest_at(gaddr) = (uint32_t)getCopReg(2, GTE_DREG(reg));
}

/* Reproduce the guest prologue's register spill: ra@0x3dc then
 * s7,s6,s5,s4,s3,s2,s1,s0 @0x3e0..0x3fc — the live cpu->gpr values,
 * same cells/values the generated stores write. */
static void guard_ctx_save(void)
{
  CPUState *c = clean_cpu_state();
  uint32_t *b;
  if (c == NULL) return;
  b = (uint32_t *)guest_at(0x1f8003dc);
  b[0] = c->gpr[31];              /* ra */
  b[1] = c->gpr[23];              /* s7 */
  b[2] = c->gpr[22];              /* s6 */
  b[3] = c->gpr[21];              /* s5 */
  b[4] = c->gpr[20];              /* s4 */
  b[5] = c->gpr[19];              /* s3 */
  b[6] = c->gpr[18];              /* s2 */
  b[7] = c->gpr[17];              /* s1 */
  b[8] = c->gpr[16];              /* s0 */
}

/* Shared OT-insert tail for both variants. tag = the 0x4000000/0x5000000/
 * 0x6000000/0x8000000 OR'd into the old head; 0x1000000 marks the chained
 * second record; addresses are guest addresses end-to-end. */
static void guard_ot_fixed(uint32_t a0,uint32_t s0,uint32_t s4,
                           uint32_t s6,uint32_t tag,uint32_t *s4p)
{
  uint32_t t1;
  uint32_t t5 = *(uint32_t *)guest_at(a0);
  *(uint32_t *)guest_at(s0) = tag | t5;
  t1 = s0 & 0xffffffu;
  *(uint32_t *)guest_at(a0) = t1;
  if ((s6 & 0x100) != 0) {
    *(uint32_t *)guest_at(s4) = 0x1000000u | t1;
    t1 = s4 & 0xffffffu;
    *(uint32_t *)guest_at(a0) = t1;
    *s4p = s4 + 8;
  }
}

/* 3-vertex family: guest 0x801a5bd0/0x801a5d90 (indexed) and
 * 0x801a6538/0x801a66dc (fixed). Index stream = lh triples, stride 6.
 * tag/o1..o3/recsz select the per-function constants:
 *   a0/a1: tag 0x4000000, offs 8/0xc/0x10, recsz 0x14
 *   b0/b1: tag 0x6000000, offs 8/0x10/0x18, recsz 0x1c
 * fixed!=0 selects the *a0 push-front variant (FN_B callees). */
static int32_t guard_tri_emit(uint32_t a0,uint32_t a1,uint32_t a2,
                              uint32_t tag,uint32_t o1,uint32_t o2,
                              uint32_t o3,uint32_t recsz,int fixed)
{
  uint32_t s0,s1,s2,s3,s4,s6,s7,t1,t5,va0 = 0,va1 = 0;
  int32_t  s5,i0,i1,i2,t1s,mac0;
  uint32_t p;
  int first = 1;

  guard_ctx_save();
  a1 <<= 2;
  s0 = *(uint32_t *)guest_at(a1 + a2);
  s4 = *(uint32_t *)guest_at(a1 + a2 + 8);
  s1 = *(uint32_t *)guest_at(a2 + 0x10);
  s2 = *(uint32_t *)guest_at(a2 + 0x18);
  s3 = *(uint32_t *)guest_at(a2 + 0x24);
  s5 = (int32_t)*(int16_t *)guest_at(a2 + 0x30);

  /* pre-loop: first index triple -> V0,V1,V2 (GTE regs 0-5).
   * word0 = idx0|idx1<<16 (u16, zero-extended); word1's low s16 = idx2
   * (sign-extended lh). */
  {
    uint32_t w = *(uint32_t *)guest_at(s2);
    i0 = (int32_t)(w & 0xffffu);
    i1 = (int32_t)(w >> 16);
    i2 = (int32_t)*(int16_t *)guest_at(s2 + 4);
    p = s1 + ((uint32_t)i0 << 3);
    guard_lwc2(p,0); guard_lwc2(p + 4,1);
    p = s1 + ((uint32_t)i1 << 3);
    guard_lwc2(p,2); guard_lwc2(p + 4,3);
    p = (uint32_t)s1 + ((uint32_t)i2 << 3);
    guard_lwc2(p,4); guard_lwc2(p + 4,5);
  }

  s5 -= 1;
  if (s5 > 0) s2 += 6; else s2 -= 6;   /* bgtz/j delay slots */

  for (;;) {
    /* loop-back target 0x..84: mtc2 the saved a1/a2 pair into V0 —
     * skipped on the first iteration (V0 keeps the pre-loop lwc2). */
    if (!first) {
      setCopReg(2, GTE_DREG(0), va0);
      setCopReg(2, GTE_DREG(1), va1);
    }
    first = 0;
    s5 -= 1;
    copFunction(2, 0x0280030);              /* RTPT */
    /* current triple: idx0 pair -> next-iteration V0 (GPRs), idx1/idx2
     * -> V1/V2 via lwc2. All three indices are lh (sign-extended). */
    i0 = (int32_t)*(int16_t *)guest_at((uint32_t)s2);
    i1 = (int32_t)*(int16_t *)guest_at((uint32_t)s2 + 2);
    i2 = (int32_t)*(int16_t *)guest_at(s2 + 4);
    p = s1 + ((uint32_t)i0 << 3);
    va0 = *(uint32_t *)guest_at(p);
    va1 = *(uint32_t *)guest_at(p + 4);
    p = s1 + ((uint32_t)i1 << 3);
    guard_lwc2(p,2); guard_lwc2(p + 4,3);
    t1 = (uint32_t)getCopReg(2, GTE_DREG(19));   /* SZ3 */
    copFunction(2, 0x0400006);              /* NCLIP */
    t1s = (int32_t)t1 >> 2;                 /* sra — value <= 0xffff */
    p = (uint32_t)s1 + ((uint32_t)i2 << 3);
    guard_lwc2(p,4); guard_lwc2(p + 4,5);
    guard_swc2(s0 + o1,12);
    guard_swc2(s0 + o2,13);
    guard_swc2(s0 + o3,14);
    mac0 = (int32_t)getCopReg(2, GTE_DREG(24));  /* MAC0 */
    s6 = (uint32_t)*(int16_t *)guest_at(s3);   /* lh — sign-extended flags word */
    /* bgez MAC0 -> insert path; else require flags&0x200. */
    if (mac0 >= 0 || (s6 & 0x200) != 0) {
      if (fixed) {
        guard_ot_fixed(a0,s0,s4,s6,tag,&s4);
      } else {
        s7 = a0 + ((uint32_t)t1s << 2);
        if (t1s >= 10) {
          t5 = *(uint32_t *)guest_at(s7);
          if (t1s < 0x800) {
            *(uint32_t *)guest_at(s0) = tag | t5;
            t1 = s0 & 0xffffffu;
            *(uint32_t *)guest_at(s7) = t1;
            if ((s6 & 0x100) != 0) {
              *(uint32_t *)guest_at(s4) = 0x1000000u | t1;
              t1 = s4 & 0xffffffu;
              *(uint32_t *)guest_at(s7) = t1;
              s4 += 8;
            }
          }
        }
      }
    }
    /* tail: s0+=recsz, s2+=6, s3+=2 (bgtz delay — always), then
     * s5>0 -> next; s5<=0 -> s2-=6 (bgez delay — always) and s5==0 ->
     * one final pass over the same triple, else exit. */
    s0 += recsz;
    s2 += 6;
    s3 += 2;
    if (s5 > 0) continue;
    s2 -= 6;
    if (s5 >= 0) continue;
    break;
  }
  return 0x00ffffff;   /* guest v0 = the 0xffffff mask constant */
}

/* 4-vertex family: guest 0x801a5f50/0x801a6150 (indexed) and
 * 0x801a6350/0x801a6880 (fixed). Index stream = 2 packed u32 words per
 * record (4 x u16, zero-extended), stride 8. After the RTPT/NCLIP/SWc2
 * triple, the previous record's 4th vertex (kept in t4, rotated from t5)
 * is lwc2'd into V0 and projected with a second RTPS (cmd 0x0180001);
 * its SXY2 lands at s0+extra. Same MAC0/flags gate + OT insert.
 *   c0/c1: tag 0x5000000, offs 8/0xc/0x10, extra 0x14, recsz 0x18
 *   d0/d1: tag 0x8000000, offs 8/0x10/0x18, extra 0x20, recsz 0x24
 * Variant-A note: the *s7 slot load sits in a branch delay slot and runs
 * unconditionally once the MAC0/flags gate passes (before the bounds
 * checks decide the writes). */
static int32_t guard_quad_emit(uint32_t a0,uint32_t a1,uint32_t a2,
                               uint32_t tag,uint32_t o1,uint32_t o2,
                               uint32_t o3,uint32_t extra,uint32_t recsz,
                               int fixed)
{
  uint32_t s0,s1,s2,s3,s4,s6,s7,t1,t3,t4,t5,va0 = 0,va1 = 0;
  int32_t  s5,t1s,mac0;
  uint32_t w0,w1,p;
  int first = 1;

  guard_ctx_save();
  a1 <<= 2;
  s0 = *(uint32_t *)guest_at(a1 + a2);
  s4 = *(uint32_t *)guest_at(a1 + a2 + 8);
  s1 = *(uint32_t *)guest_at(a2 + 0x10);
  s2 = *(uint32_t *)guest_at(a2 + 0x18);
  s3 = *(uint32_t *)guest_at(a2 + 0x24);
  s5 = (int32_t)*(int16_t *)guest_at(a2 + 0x30);

  /* pre-loop: quad0 -> V0,V1,V2 via lwc2 (u16 halves, zero-extended);
   * t4 = ptr to idx3's vertex entry (consumed by the in-loop RTPS). */
  w0 = *(uint32_t *)guest_at(s2);
  w1 = *(uint32_t *)guest_at(s2 + 4);
  p = s1 + ((w0 & 0xffffu) << 3);
  guard_lwc2(p,0); guard_lwc2(p + 4,1);
  p = s1 + ((w0 >> 16) << 3);
  guard_lwc2(p,2); guard_lwc2(p + 4,3);
  p = s1 + ((w1 & 0xffffu) << 3);
  guard_lwc2(p,4); guard_lwc2(p + 4,5);
  t4 = s1 + ((w1 >> 16) << 3);

  s5 -= 1;
  if (s5 > 0) s2 += 8; else s2 -= 8;   /* bgtz/j delay slots */

  for (;;) {
    if (!first) {
      setCopReg(2, GTE_DREG(0), va0);
      setCopReg(2, GTE_DREG(1), va1);
    }
    first = 0;
    s5 -= 1;
    copFunction(2, 0x0280030);              /* RTPT */
    w0 = *(uint32_t *)guest_at(s2);
    w1 = *(uint32_t *)guest_at(s2 + 4);
    /* idx0 pair -> next-iteration V0 (GPRs); idx1 -> V1, idx2 -> V2. */
    p = s1 + ((w0 & 0xffffu) << 3);
    va0 = *(uint32_t *)guest_at(p);
    va1 = *(uint32_t *)guest_at(p + 4);
    p = s1 + ((w0 >> 16) << 3);
    guard_lwc2(p,2); guard_lwc2(p + 4,3);
    t1 = (uint32_t)getCopReg(2, GTE_DREG(19));   /* SZ3 */
    copFunction(2, 0x0400006);              /* NCLIP */
    t1s = (int32_t)t1 >> 2;
    p = s1 + ((w1 & 0xffffu) << 3);
    guard_lwc2(p,4); guard_lwc2(p + 4,5);
    t5 = (uint32_t)s1 + ((w1 >> 16) << 3);  /* idx3 ptr (rotates to t4) */
    guard_swc2(s0 + o1,12);
    guard_swc2(s0 + o2,13);
    guard_swc2(s0 + o3,14);
    mac0 = (int32_t)getCopReg(2, GTE_DREG(24));  /* MAC0 */
    s6 = (uint32_t)*(int16_t *)guest_at(s3);   /* lh — sign-extended flags word */
    if (mac0 >= 0 || (s6 & 0x200) != 0) {
      /* V0 <- idx3 vertex, projected with a lone RTPS; its SXY2 is the
       * 4th record slot. */
      if (fixed) {
        t3 = *(uint32_t *)guest_at(a0);
        guard_lwc2(t4,0); guard_lwc2(t4 + 4,1);
        copFunction(2, 0x0180001);          /* RTPS */
        *(uint32_t *)guest_at(s0) = tag | t3;
        t1 = s0 & 0xffffffu;
        *(uint32_t *)guest_at(a0) = t1;
        if ((s6 & 0x100) != 0) {
          *(uint32_t *)guest_at(s4) = 0x1000000u | t1;
          t1 = s4 & 0xffffffu;
          *(uint32_t *)guest_at(a0) = t1;
          s4 += 8;
        }
        guard_swc2(s0 + extra,14);
      } else {
        s7 = a0 + ((uint32_t)t1s << 2);
        guard_lwc2(t4,0); guard_lwc2(t4 + 4,1);
        copFunction(2, 0x0180001);          /* RTPS */
        /* delay-slot load: unconditional once the gate passed. */
        t3 = *(uint32_t *)guest_at(s7);
        if (t1s >= 10 && t1s < 0x800) {
          *(uint32_t *)guest_at(s0) = tag | t3;
          t1 = s0 & 0xffffffu;
          *(uint32_t *)guest_at(s7) = t1;
          if ((s6 & 0x100) != 0) {
            *(uint32_t *)guest_at(s4) = 0x1000000u | t1;
            t1 = s4 & 0xffffffu;
            *(uint32_t *)guest_at(s7) = t1;
            s4 += 8;
          }
          guard_swc2(s0 + extra,14);
        }
      }
    }
    /* tail: t4=t5, s0+=recsz, s2+=8, s3+=2, same s5 branch shape. */
    t4 = t5;
    s0 += recsz;
    s2 += 8;
    s3 += 2;
    if (s5 > 0) continue;
    s2 -= 8;
    if (s5 >= 0) continue;
    break;
  }
  return 0x00ffffff;
}

int guard_callee_a0(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_tri_emit(a0,a1,a2,0x4000000u,8,0xc,0x10,0x14,0); }
int guard_callee_a1(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_tri_emit(a0,a1,a2,0x4000000u,8,0xc,0x10,0x14,1); }
int guard_callee_b0(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_tri_emit(a0,a1,a2,0x6000000u,8,0x10,0x18,0x1c,0); }
int guard_callee_b1(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_tri_emit(a0,a1,a2,0x6000000u,8,0x10,0x18,0x1c,1); }
int guard_callee_c0(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_quad_emit(a0,a1,a2,0x5000000u,8,0xc,0x10,0x14,0x18,0); }
int guard_callee_c1(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_quad_emit(a0,a1,a2,0x5000000u,8,0xc,0x10,0x14,0x18,1); }
int guard_callee_d0(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_quad_emit(a0,a1,a2,0x8000000u,8,0x10,0x18,0x20,0x24,0); }
int guard_callee_d1(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3)
{ (void)a3; return guard_quad_emit(a0,a1,a2,0x8000000u,8,0x10,0x18,0x20,0x24,1); }

/* Live guest $a3 — the v1==0 jal path passes the caller's $a3 through
 * untouched (guard_dispatch never writes $a3 before the call). */
extern CPUState *clean_cpu_state(void);
static uint32_t br2_guard_passthru_a3(void)
{
  CPUState *c = clean_cpu_state();
  return c ? c->gpr[7] : 0;
}

/* 0x801a4be8 — register node a0 (guest addr) into table slot idx*72+0x40. */
void op_node_register(uint32_t a0)
{
  int32_t idx = *(int8_t *)guest_at(a0 + 0x14);
  uint32_t e = 0x801ff398 + (uint32_t)idx * 72;
  *(uint32_t *)guest_at(e + 0x40) = a0;
  *(uint32_t *)guest_at(0x801beef8) = (uint32_t)idx;
  if (*(int8_t *)guest_at(a0 + 0x18) != 0) {
    task_slot_step();
  } else {
    uint32_t fp = *(uint32_t *)guest_at(a0 + 8);
    uint32_t arg = *(uint32_t *)guest_at(a0 + 0x10);
    task_invoke(fp, arg);
  }
  return;
}

/* 0x801a4cd8 — count nodes across 16 buckets of table a0; returns count. */
int op_node_count16(uint32_t a0)
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
  text_win_printf((const char *)guest_at(0x80126008),s0);
  return s0;
}

/* 0x801a46c8 — node bucket move: unlink guest node a0 from the chain
 * headed at table[lb(a0+0x14)*72 + lb(a0+0x16)*4] (table base
 * 0x801ff398, 16 buckets x 4B per 72B type slot) and push it as head of
 * bucket a1 (a1 >= 0x10 or a0 == 0 -> no-op). Node links: +0 = next,
 * +4 = prev; +0x16 = sub-bucket index (byte). Disasm
 * 0x801a46c8-0x801a4784. */
void op_node_rebucket(uint32_t a0,uint32_t a1)
{
  int32_t tidx;
  uint32_t a2, v1, v0, slot;
  /* guest reads lb(a0+0x14) and forms a2 before the a0/a1 guards */
  tidx = *(int8_t *)guest_at(a0 + 0x14);
  a2 = 0x801ff398 + (uint32_t)(tidx * 72);
  if (a0 == 0) return;
  if (a1 >= 0x10) return;
  v1 = *(uint32_t *)guest_at(a0 + 0);
  if (v1 != 0) {
    v0 = *(uint32_t *)guest_at(a0 + 4);
    *(uint32_t *)guest_at(v1 + 4) = v0;
  }
  v1 = *(uint32_t *)guest_at(a0 + 4);
  if (v1 != 0) {
    v0 = *(uint32_t *)guest_at(a0 + 0);
    *(uint32_t *)guest_at(v1 + 0) = v0;
  } else {
    v0 = (uint32_t)*(int8_t *)guest_at(a0 + 0x16) * 4 + a2;
    v1 = *(uint32_t *)guest_at(a0 + 0);
    *(uint32_t *)guest_at(v0) = v1;
  }
  slot = a2 + a1 * 4;
  v0 = *(uint32_t *)guest_at(slot);
  if (v0 != 0) {
    *(uint32_t *)guest_at(v0 + 4) = a0;
    *(uint32_t *)guest_at(a0 + 0) = v0;
  } else {
    *(uint32_t *)guest_at(a0 + 0) = 0;
  }
  *(uint32_t *)guest_at(a0 + 4) = 0;
  *(uint32_t *)guest_at(slot) = a0;
  *(uint8_t *)guest_at(a0 + 0x16) = (uint8_t)a1;
}

/* 0x801a536c — count flagged (fC!=0) nodes; text_win_printf(fmt, a1, cnt). */
void op_node_count_flagged(uint32_t a0,uint32_t a1)
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
  text_win_printf((const char *)guest_at(0x80126088),a1,cnt);
  return;
}

/* 0x801a53e8 — op_dbg_dump5 dump for every node in ring. */
void op_dbg_dump_ring(uint32_t a0,uint32_t a1)
{
  uint32_t addr = 0x801ff268 + a1 * 12 + a0 * 60;
  uint32_t first = *(uint32_t *)guest_at(addr);
  uint32_t cur = first;
  if (cur != 0) {
    for (;;) {
      op_dbg_dump5(0x801cee0,cur);
      cur = *(uint32_t *)guest_at(cur + 4);
      if (cur == first) break;
    }
  }
  return;
}

/* 0x801a50d8 — header + 3 field dumps of table node (a0*60+a1*12+0x801ff268 indexed by a2? see asm: s0 = a1*60 + a2*12... actually a0*60? recheck: sll s0,a1,4; subu s0,s0,a1 → a1*15; sll 2 → a1*60. v0=a2*3*4=a2*12. s0 += a2*12 + 0x801ff268. */
void op_dbg_dump_node(uint32_t a0,uint32_t a1,uint32_t a2)
{
  uint32_t s0 = 0x801ff268 + a1 * 60 + a2 * 12;
  text_win_printf((const char *)guest_at(0x801ceec),a0);
  text_win_printf((const char *)guest_at(0x80126014),*(uint32_t *)guest_at(s0));
  text_win_printf((const char *)guest_at(0x80126024),*(uint32_t *)guest_at(s0 + 4));
  text_win_printf((const char *)guest_at(0x80126034),*(uint32_t *)guest_at(s0 + 8));
  return;
}

extern void op_tag_E_set(uint8_t *,uint32_t);
extern void op_tag_L_set(uint8_t *,uint32_t);
extern void op_tag_G_set(uint8_t *,uint32_t);

/* 0x801a24dc — dispatch on a0[0x46]: 3->3048(a0,a0[0xe4]) 2->3034(a0,a1) 4->3088(a0,a0[0x47]). */
void op_tag_dispatch(uint8_t *a0,uint32_t a1)
{
  uint8_t v1 = *(uint8_t *)(a0 + 0x46);
  if (v1 == 3) {
    op_tag_L_set(a0,*(uint8_t *)(a0 + 0xe4));
  } else if (v1 < 4) {
    if (v1 == 2) {
      op_tag_E_set(a0,a1);
    }
  } else if (v1 == 4) {
    op_tag_G_set(a0,*(uint8_t *)(a0 + 0x47));
  }
  return;
}

/* 0x801ad424 — task_node_alloc job; init node {a3, a2[1], a2[2], a4}; returns job ptr. */
int *op_job_alloc_init(int32_t a0,int32_t a1,uint32_t a2,uint32_t a3,uint32_t a4)
{
  int *r = task_node_alloc(a0,a1,4,0x801b0db8,0x801ad2dc,0x1c);
  if (r != (int *)0x0) {
    /* r is the node's GUEST address (task_node_alloc returns guest-addr-as-
     * pointer); node+0x10 cell holds the guest addr of the inline data. */
    uint32_t *p = (uint32_t *)guest_at(
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)r + 0x10));
    p[0] = a3;
    p[2] = *(uint32_t *)guest_at(a2 + 8);
    p[3] = a4;
    p[1] = *(uint32_t *)guest_at(a2 + 4);
  }
  return r;
}

/* 0x801aed3c — cb(a3[a0[2]]); store at ctx8[*a2] when *a2!=0. */
void op_cb1_store_ctx8(int16_t *a0,uint32_t a1,int16_t *a2,uint32_t a3)
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
    else {
      /* Guest jalr: a0=node, a1=fp-cell addr, a2=lh value (sign-extended),
       * a3=base — all four regs explicit. */
      r = br2_call_clean(fp,node,a1,(uint32_t)a2v,a3);
    }
    *(uint32_t *)guest_at(s0) = r;
  } else if (fn != (void *)0x0) {
    ((void (*)(uint32_t))fn)(node);
  }
  else {
    br2_call_clean(fp,node,a1,(uint32_t)a2v,a3);
  }
  return;
}

/* 0x801aedbc — cb(a3[a0[2]], a3[a0[3]]); store at ctx8[*a2] when *a2!=0. */
void op_cb2_store_ctx8(int16_t *a0,uint32_t a1,int16_t *a2,uint32_t a3)
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
    else {
      /* Guest jalr: a0=n0, a1=n1, a2=lh value, a3=base. */
      r = br2_call_clean(fp,n0,n1,(uint32_t)a2v,a3);
    }
    *(uint32_t *)guest_at(s0) = r;
  } else if (fn != (void *)0x0) {
    ((void (*)(uint32_t,uint32_t))fn)(n0,n1);
  }
  else {
    br2_call_clean(fp,n0,n1,(uint32_t)a2v,a3);
  }
  return;
}

/* ---- identical list-walk+dispatch family (0x801a6a68/863c, 0x801aa47c,
 *      0x801ac0d4, 0x801acd3c, 0x801ad5d0): for each node in *a1 chain:
 *      (*a0)->f40 = node; if node->b17: node->b18 ? task_slot_step(a0)
 *      : node->f8(node->f10). Params are host pointers; fn via fnmap. ---- */
static void walk_dispatch(uint8_t **a0,uint8_t *a1)
{
  uint8_t *node_ = a1;
  for (;;) {
    /* a0/a1/node_/ctx_ are host pointers into guest RAM; link/cell fields
     * are 4-byte GUEST addresses — never read/write them as host pointers. */
    uint32_t nxg_ = *(uint32_t *)node_;
    uint8_t *nx_ = nxg_ ? (uint8_t *)guest_at(nxg_) : (uint8_t *)0x0;
    uint8_t *ctx_ = (uint8_t *)guest_at(*(uint32_t *)a0);
    *(uint32_t *)(ctx_ + 0x40) = (uint32_t)guest_of(node_);
    if (*(int8_t *)(node_ + 0x17) != 0) {
      if (*(int8_t *)(node_ + 0x18) != 0) {
        task_slot_step();
      } else {
        uint32_t fp_ = *(uint32_t *)(node_ + 8);
        uint32_t ar_ = *(uint32_t *)(node_ + 0x10);
        /* node->f8/f10 is the same fn_guest/arg_guest pair task_invoke
         * dispatches — it applies the int-vs-pointer arg convention. */
        task_invoke(fp_, ar_);
      }
    }
    if (nx_ == (uint8_t *)0x0) break;
    node_ = nx_;
  }
  return;
}

void op_walk_dispatch_a(uint8_t **a0,uint8_t *a1) { walk_dispatch(a0,a1); return; }
void op_walk_dispatch_b(uint8_t **a0,uint8_t *a1) { walk_dispatch(a0,a1); return; }
void op_walk_dispatch_c(uint8_t **a0,uint8_t *a1) { walk_dispatch(a0,a1); return; }
void op_walk_dispatch_d(uint8_t **a0,uint8_t *a1) { walk_dispatch(a0,a1); return; }
void op_walk_dispatch_e(uint8_t **a0,uint8_t *a1) { walk_dispatch(a0,a1); return; }
void op_walk_dispatch_f(uint8_t **a0,uint8_t *a1) { walk_dispatch(a0,a1); return; }

/* 0x801aa3fc — free slot a1 of container s1 (f24[] and f3c[] arrays). */
void op_slot_free(uint8_t *s1,int32_t a1)
{
  if (s1 != (uint8_t *)0x0 && a1 >= 0 &&
      a1 < (int32_t)*(uint32_t *)(s1 + 0x44)) {
    uint32_t o = (uint32_t)a1 * 4;
    uint32_t *arr = (uint32_t *)guest_at(*(uint32_t *)(s1 + 0x24));
    task_node_free((int *)(uintptr_t)arr[a1]);
    arr[a1] = 0;
    {
      uint32_t *arr2 = (uint32_t *)guest_at(*(uint32_t *)(s1 + 0x3c));
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
      uint32_t o_ = guest_of((const void *)a0);                    \
      if (v1_ == 0) {                                              \
        FN_A(*(uint32_t *)guest_at(0x801bef24),a1_,o_,            \
             br2_guard_passthru_a3());                             \
      } else if ((uint32_t)(v1_ - 1) < 0x7ff) {                    \
        FN_B(*(uint32_t *)guest_at(0x801bef24) +                 \
             (uint32_t)v1_ * 4,a1_,o_,(uint32_t)(int32_t)v1_);     \
      }                                                            \
    }                                                              \
  } while (0)

void guard_dispatch_a(uint8_t *a0) { BR2_GUARD_DISPATCH(guard_callee_a0,guard_callee_a1); return; }
void guard_dispatch_b(uint8_t *a0) { BR2_GUARD_DISPATCH(guard_callee_b0,guard_callee_b1); return; }
void guard_dispatch_c(uint8_t *a0) { BR2_GUARD_DISPATCH(guard_callee_c0,guard_callee_c1); return; }
void guard_dispatch_d(uint8_t *a0) { BR2_GUARD_DISPATCH(guard_callee_d0,guard_callee_d1); return; }

/* ======== batch36: 0x801a0e/0x801a17/0x801a40/0x801aef ======== */

extern int32_t cd_seek_state_machine(uint32_t);

/* 0x801a0e98 — reset 7 globals via cd_seek_state_machine, then shift two 4-word blocks. */
void op_globals_reset_shift(void)
{
  cd_seek_state_machine(*(uint32_t *)guest_at(0x801ef744));
  cd_seek_state_machine(*(uint32_t *)guest_at(0x801ef774));
  cd_seek_state_machine(*(uint32_t *)guest_at(0x801ef778));
  cd_seek_state_machine(*(uint32_t *)guest_at(0x801ef77c));
  cd_seek_state_machine(*(uint32_t *)guest_at(0x801ef780));
  cd_seek_state_machine(*(uint32_t *)guest_at(0x801ef784));
  cd_seek_state_machine(*(uint32_t *)guest_at(0x801ef788));
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
void op_frame_advance(uint8_t *s0)
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
          return;   /* rec70==1, rec74<11 -> END (no clear-tail) */
        } else {
          *(uint8_t *)(s0 + 0x49) = 2;
          *(uint8_t *)(s0 + 0x46) = 0xff;
          return;   /* rec70==1 exhaust -> END (no clear-tail) */
        }
      } else {
        uint8_t t = *(uint8_t *)(s0 + 0x4a);
        if ((uint32_t)t < 0xb) {
          *(uint8_t *)(s0 + 0x4a) = (uint8_t)(t + 1);
          return;   /* rec70>=2, rec74<11 -> END (no clear-tail) */
        } else if (*(uint8_t *)(s0 + 0x49) != 0) {
          uint32_t fp = *(uint32_t *)guest_at(0x801be784);
          void *fn = fn_lookup(fp);
          if (fn != (void *)0x0) {
            ((void (*)(uint32_t))fn)((uint32_t)guest_of(s0));
          }
          else {
            /* Guest jalr: a0 = s0 guest addr (delay-slot move), a1 =
             * caller's live $a1. */
            CPUState *cpu = clean_cpu_state();
            call_guest(fp,(uint32_t)guest_of(s0),
                       cpu != NULL ? cpu->gpr[5] : 0u);
          }
        }
      }
    }
  }
  /* s0+0x3c / s0+0x30 hold guest addresses — deref through guest_at, the
   * same way the generated lw/sb pair resolves them. */
  if (*(uint8_t *)guest_at(*(uint32_t *)(s0 + 0x3c)) == 0xf3) {
    /* fallthrough: nothing */
  } else {
    uint8_t *t = (uint8_t *)guest_at(*(uint32_t *)(s0 + 0x30));
    t[0] = 0xff;
    t[1] = 0;
    *(uint8_t *)(s0 + 0xe8) = 0;
    *(uint8_t *)(s0 + 0x35) = 0;
  }
  return;
}

/* 0x801aef18 — 4-arg cb dispatch; store result at ctx8[*a2] when *a2!=0. */
void op_cb4_store_ctx8(int16_t *a0,uint32_t a1,int16_t *a2,uint32_t a3)
{
  int32_t a2v = *a2;
  uint32_t fp = *(uint32_t *)guest_at(a1);
  void *fn = fn_lookup(fp);
  if (a2v != 0) {
    uint32_t s0 = BR2_GPF(8) + (uint32_t)a2v;
    uint32_t r = 0;
    uint32_t n0 = *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[2] * 4);
    uint32_t n1 = *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[3] * 4);
    uint32_t n2 = *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[4] * 4);
    uint32_t n3 = *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[5] * 4);
    if (fn != (void *)0x0) {
      r = ((uint32_t (*)(uint32_t,uint32_t,uint32_t,uint32_t))fn)(n0,n1,n2,n3);
    }
    else {
      /* Guest jalr: a0-a3 = the four node values (a0[2..5] entries). */
      r = br2_call_clean(fp,n0,n1,n2,n3);
    }
    *(uint32_t *)guest_at(s0) = r;
  } else if (fn != (void *)0x0) {
    ((void (*)(uint32_t,uint32_t,uint32_t,uint32_t))fn)(
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[2] * 4),
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[3] * 4),
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[4] * 4),
      *(uint32_t *)guest_at(a3 + (uint32_t)(int32_t)a0[5] * 4));
  }
  else {
    br2_call_clean(fp,
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
void op_tag_E_set(uint8_t *a0,uint32_t a1)
{
  (void)a1;
  a0[0x37] = 0x45;
  *(uint32_t *)(a0 + 0x2c) = 0;
  a0[0x36] = 0;
}

/* 0x801a3048 — tag 0x4c ('L'): a0[0x37]=0x4c, a0[0x2c]=guest(a0+0x24),
 * a0[0x24]=a1, a0[0x36]=1. */
void op_tag_L_set(uint8_t *a0,uint32_t a1)
{
  a0[0x37] = 0x4c;
  *(uint32_t *)(a0 + 0x2c) = guest_of(a0 + 0x24);
  a0[0x24] = (uint8_t)a1;
  a0[0x36] = 1;
}

/* 0x801a3088 — tag 0x47 ('G'): same shape as 3048 with 0x47. */
void op_tag_G_set(uint8_t *a0,uint32_t a1)
{
  a0[0x37] = 0x47;
  *(uint32_t *)(a0 + 0x2c) = guest_of(a0 + 0x24);
  a0[0x24] = (uint8_t)a1;
  a0[0x36] = 1;
}

/* 0x801a3068 — tag 0x46 ('F'): same shape as op_tag_L/G_set —
 * a0[0x37]=0x46, a0[0x2c]=guest(a0+0x24), a0[0x24]=a1, a0[0x36]=1.
 * Guest leaves v0=1 (addiu v0,zero,1 @0x801a3078). */
uint32_t op_tag_F_set(uint8_t *a0,uint32_t a1)
{
  a0[0x37] = 0x46;
  *(uint32_t *)(a0 + 0x2c) = guest_of(a0 + 0x24);
  a0[0x24] = (uint8_t)a1;
  a0[0x36] = 1;
  return 1;
}

/* 0x801a30a8 — tag 0x4b ('K'): same shape as op_tag_E_set —
 * a0[0x37]=0x4b, a0[0x2c]=0, a0[0x36]=0, a1 unused.
 * Guest leaves v0=0x4b (addiu v0,zero,0x4b @0x801a30a8). */
uint32_t op_tag_K_set(uint8_t *a0,uint32_t a1)
{
  (void)a1;
  a0[0x37] = 0x4b;
  *(uint32_t *)(a0 + 0x2c) = 0;
  a0[0x36] = 0;
  return 0x4b;
}

/* ======== batch_mass17: disasm-verified ports ======== */

/* mc_job_issue_a / mc_job_issue_b — memory-card command issue.
 * State block @0x801febe0 (s1): [0]=state [1],[2] [4]=a0(path) [5]=flag
 * (<0 required) [6]=a3 [7]=a4(5th arg) [8]=a2 [0x24..]=cmd blk.
 * Checks -> printf str + ret 0; success -> state=3 (f35c) / 4 (f5b4),
 * queue cb 0x8019f474 / 0x8019f6cc, ret 1.
 * Disasm 0x8019f35c-0x8019f470, 0x8019f5b4-0x8019f6a8. */
static int mc_issue(uint32_t a0,const void *a1,void *a2,
                        uint32_t a3,uint32_t a4,int state,uint32_t cb)
{
  uint32_t *s1 = (uint32_t *)guest_at(0x801febe0);
  uint8_t *blk = (uint8_t *)guest_at(0x801febe0 + 0x24);
  if ((int32_t)s1[0] > 0) {
    printf_forward((const char *)guest_at(0x80125e8c));
    return 0;
  }
  if ((int32_t)s1[5] >= 0) {
    printf_forward((const char *)guest_at(0x80125de4));
    return 0;
  }
  if ((a4 & 0x7f) != 0) {
    printf_forward((const char *)guest_at(0x80125e30));
    return 0;
  }
  if ((a3 & 0x7f) != 0) {
    printf_forward((const char *)guest_at(0x80125e5c));
    return 0;
  }
  cd_file_header_copy(a0,blk);
  cd_strcat_guard(blk,(uintptr_t)a1);
  s1[0] = (uint32_t)state;
  s1[1] = 0;
  s1[2] = 0;
  s1[6] = a3;
  s1[8] = guest_of(a2);
  s1[7] = a4;
  s1[4] = a0;                        /* delay-slot store: before the call */
  cd_job_slot_alloc(cb,0);
  return 1;
}

int mc_job_issue_a(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  return mc_issue(a0,a1,a2,a3,a4,3,0x8019f474);
}

int mc_job_issue_b(uint32_t a0,const void *a1,void *a2,uint32_t a3,uint32_t a4)
{
  return mc_issue(a0,a1,a2,a3,a4,4,0x8019f6cc);
}

/* mc_file_cmd — memory-card file command (open/read/write style).
 * State @0x801febe0; cmd blk on stack (0x20 bytes @sp+0x10).
 * Returns -1 busy, 6 open-ok, 0 issued, 7 done-fail, else status.
 * Disasm 0x8019fb9c-0x8019fd94. */
int mc_file_cmd(uint32_t a0,const void *a1,uint32_t a2)
{
  uint32_t *s0 = (uint32_t *)guest_at(0x801febe0);
  /* cmd blk crosses the BIOS boundary (FUN_801a0220/801a0250 kernel
   * services) — must be guest RAM, so reserve a real guest frame. */
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  extern void br2_irq_poll(void);
  extern int br2_bail_pending(void);
  uint32_t fr = br2_guest_frame(0x40);
  uint8_t *blk = (uint8_t *)guest_at(fr + 0x10);
  uint32_t a2hi;
  int32_t s2 = 0;
  uint32_t st;
  int32_t v0;
  if (s0[0] != 0) {
    printf_forward((const char *)guest_at(0x80125e8c));
    br2_guest_frame_free();
    return -1;
  }
  cd_file_header_copy(a0,blk);
  cd_strcat_guard(blk,(uintptr_t)a1);
  s0[3] |= 1u << (s0[4] & 31);
  v0 = FUN_801a0220(fr + 0x10,1,s0[4]);
  a2hi = a2 << 16;                    /* delay slot: always executes */
  if (v0 >= 0) {
    FUN_801a0250(v0);
    br2_guest_frame_free();
    return 6;
  }
  for (;;) {
    v0 = FUN_801a0220(fr + 0x10,(a2hi | 0x200),s0[4]);
    if (v0 >= 0) {
      FUN_801a0250(v0);
      br2_guest_frame_free();
      return 0;
    }
    *(uint32_t *)guest_at(0x801fec38) = cd_xchg_fec24(0);
    if ((int32_t)s0[0] > 0) {
      printf_forward((const char *)guest_at(0x80125ce8));
    } else {
      s0[0] = 2;
      s0[1] = 0;
      s0[2] = 0;
      *(uint32_t *)guest_at(0x801febf0) = a0;  /* delay-slot store */
      cd_job_slot_alloc(0x8019eb00,0);
    }
    /* wait for state change */
    if (s0[0] == 0 && s0[2] == 0) {
      while (*(uint32_t *)guest_at(0x801febe8) == 0) {
        if (br2_bail_pending()) { br2_guest_frame_free(); return -1; }
        br2_irq_poll();
      }
    }
    st = *(uint32_t *)guest_at(0x801febd4);
    s0[2] = 0;
    cd_xchg_fec24(*(uint32_t *)guest_at(0x801fec38));
    if (st == 0) { br2_guest_frame_free(); return 7; }
    if (st == 3) continue;             /* re-issue same cmd */
    if (st == 2) {
      s2++;
      if (s2 < 4) continue;            /* retry issue */
    }
    br2_guest_frame_free();
    return (int32_t)st;
  }
}

/* data_relocate — relocate fixups in a loaded data block (magic
 * 0x000ed1c4 @+0). Adds block guest base to self-relative u32 fields:
 * +0x7c,+0x80..+0xa8 (12 fields); then per-slot record fixups driven by
 * tables at +0x5c..+0x78 (8 entries, stride 4 -> +0x34 records),
 * +0x7c/+0x28 count list, and +0x84/+0x88/+0x8c arrays gated by
 * +0xc mode / +0x10 / +0x14 flags, count +0x24, stride 0x14.
 * Disasm 0x801aa96c-0x801aad00. */
void data_relocate(uint32_t a0)
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

/* cd_state_clear — clear 0x801be7e8 (dead lw of same cell in guest). */
void cd_state_clear(void)
{
  *(uint32_t *)guest_at(0x801be7e8) = 0;
}

/* slot_field_clear — for i < *(a0+0x40): sh 0 @ +0x32 of
 * *(*(arr+0x10)[i] + 0x10). a0 arrives as guest addr or host ptr.
 * Disasm 0x801a7444-0x801a7494. */
void slot_field_clear(uint32_t a0)
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

/* elem_pair_flag100 / aa168 / aa1e4 / aa2c8 / aa320 / aa374 — animation-slot
 * record flag/field setters. a0 = host ctx; arrays at +0x34/+0x38 (or
 * +0x3c), count +0x44, idx a1 (signed). Records: flags u16 @+0x44,
 * params @+0x46/+0x48. Disasm 0x801aa0e4-0x801aa3f8. */
static uint8_t *elem_at(uint8_t *s,uint32_t arros,int32_t idx)
{
  uint8_t *arr = (uint8_t *)mem_ptr(*(uint32_t *)(s + arros));
  uint32_t e = *(uint32_t *)(arr + (uint32_t)idx * 4);
  return (uint8_t *)mem_ptr(e);
}

void elem_pair_flag100(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = elem_at(s,0x34,a1);
  if (e != (uint8_t *)0x0) *(uint16_t *)(e + 0x44) |= 0x100;
  e = elem_at(s,0x38,a1);
  if (e != (uint8_t *)0x0) *(uint16_t *)(e + 0x44) |= 0x100;
}

void elem_pair_flag8(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = elem_at(s,0x34,a1);
  if (e != (uint8_t *)0x0)
    *(uint16_t *)(e + 0x44) = (uint16_t)*(uint8_t *)(e + 0x44);
  e = elem_at(s,0x38,a1);
  if (e != (uint8_t *)0x0)
    *(uint16_t *)(e + 0x44) = (uint16_t)*(uint8_t *)(e + 0x44);
}

void elem_pair_field_set(void *a0,int32_t a1,int32_t a2,int32_t a3)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = elem_at(s,0x34,a1);
  if (e != (uint8_t *)0x0) {
    *(int16_t *)(e + 0x46) = (int16_t)a2;
    e = elem_at(s,0x34,a1);
    *(int16_t *)(e + 0x48) = (int16_t)a3;
    e = elem_at(s,0x34,a1);
    *(uint16_t *)(e + 0x44) |= 1;
  }
  e = elem_at(s,0x38,a1);
  if (e != (uint8_t *)0x0) {
    *(int16_t *)(e + 0x46) = (int16_t)a2;
    e = elem_at(s,0x38,a1);
    *(int16_t *)(e + 0x48) = (int16_t)a3;
    e = elem_at(s,0x38,a1);
    *(uint16_t *)(e + 0x44) |= 1;
  }
}

void elem_flag100(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = elem_at(s,0x3c,a1);
  if (e != (uint8_t *)0x0) *(uint16_t *)(e + 0x44) |= 0x100;
}

void elem_flag8(void *a0,int32_t a1)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = elem_at(s,0x3c,a1);
  if (e != (uint8_t *)0x0)
    *(uint16_t *)(e + 0x44) = (uint16_t)*(uint8_t *)(e + 0x44);
}

void elem_field_set(void *a0,int32_t a1,int32_t a2,int32_t a3)
{
  uint8_t *s = (uint8_t *)a0;
  uint8_t *e;
  if (s == (uint8_t *)0x0 || a1 < 0) return;
  if ((uint32_t)a1 >= *(uint32_t *)(s + 0x44)) return;
  e = elem_at(s,0x3c,a1);
  if (e != (uint8_t *)0x0) {
    *(int16_t *)(e + 0x46) = (int16_t)a2;
    e = elem_at(s,0x3c,a1);
    *(int16_t *)(e + 0x48) = (int16_t)a3;
    e = elem_at(s,0x3c,a1);
    *(uint16_t *)(e + 0x44) |= 1;
  }
}

/* 0x801AA2AC — elem_flag1: leaf alias tail-entry of elem_pair_field_set.
 * Entered via fn-ptr (BIOS CD sub-channel callback @0x801FEB48) with the
 * element pointer already live in $v1 (gpr[3]):
 *   v0 = lhu(elem+0x44) | 1 ; sh(elem+0x44) = v0 ; jr $ra
 * The flag load is LHU (insn 0x94620044, opcode 0x25) — zero-extended.
 * gpr[2] write-back preserved (caller may read $v0); gpr[3] untouched.
 * Disasm generated/SLES_017.22_full_52.c block_801AA2AC (0x801AA2B0-2BC). */
void elem_flag1(uint32_t elem)
{
  uint32_t f = (uint32_t)br2_ld16(elem + 0x44) | 1u;
  br2_st16(elem + 0x44, (uint16_t)f);
  {
    extern CPUState *clean_cpu_state(void);
    CPUState *cpu = clean_cpu_state();
    if (cpu != NULL) cpu->gpr[2] = f;
  }
}

/* 0x801AA244 — elem_pair_field_set_alt: alias entry into the shared tail
 * of elem_pair_field_set (canonical 0x801AA1E4), reached via fn-ptr
 * (BIOS CD sub-channel callback @0x801FEB44). Live-gpr contract — NO a0-a3
 * marshalling applies; the guest reads:
 *   v0 = base of the first element array (caller pre-loaded),
 *   a0 = ctx (second array ptr at a0+0x38),
 *   a1 = index*4 byte offset (pre-shifted by the caller),
 *   a2/a3 = field values for elem+0x46/+0x48.
 * Flow (all addrs guest):
 *   e1 = lw(a1+v0) -> $v1 ; flag-set e1: lhu(e1+0x44)|1 -> sh(e1+0x44)
 *   e2 = lw(a1 + lw(a0+0x38)) ; if e2==0 return early ($v0=0,$v1=e1)
 *   sh(e2+0x46)=a2 ; e2 reloaded ; sh(e2+0x48)=a3 ; e2 reloaded -> $v1
 *   flag-set e2 (shared 0x801AA2AC tail) ; return ($v0=flag|1,$v1=e2)
 * Leaf (jr $ra). Element ptrs are re-loaded between every store exactly as
 * the generated body does. Disasm generated ...:15441-15760. */
void elem_pair_field_set_alt(uint32_t v0,uint32_t a0,uint32_t a1,
                             uint32_t a2,uint32_t a3)
{
  uint32_t e1 = br2_ld32(a1 + v0);
  uint32_t f1 = (uint32_t)br2_ld16(e1 + 0x44) | 1u;
  br2_st16(e1 + 0x44, (uint16_t)f1);
  uint32_t e2 = br2_ld32(a1 + br2_ld32(a0 + 0x38));
  {
    extern CPUState *clean_cpu_state(void);
    CPUState *cpu = clean_cpu_state();
    if (e2 == 0) {
      if (cpu != NULL) { cpu->gpr[2] = 0; cpu->gpr[3] = e1; }
      return;
    }
    br2_st16(e2 + 0x46, (uint16_t)a2);
    e2 = br2_ld32(a1 + br2_ld32(a0 + 0x38));
    br2_st16(e2 + 0x48, (uint16_t)a3);
    e2 = br2_ld32(a1 + br2_ld32(a0 + 0x38));   /* -> $v1 */
    {
      uint32_t f2 = (uint32_t)br2_ld16(e2 + 0x44) | 1u;
      br2_st16(e2 + 0x44, (uint16_t)f2);
      if (cpu != NULL) { cpu->gpr[2] = f2; cpu->gpr[3] = e2; }
    }
  }
}

/* mat3x3_build — build 3x3 fixed-point matrix @a3 (9 s16, /4096)
 * from sin/cos table lookups lut_sin_q12/lut_cos_q12 of a0,a1,a2
 * (caller passes raw s16 angles). Returns a3.
 * Disasm 0x801b1140-0x801b1280. */
uint32_t mat3x3_build(int32_t a0,int32_t a1,int32_t a2,uint32_t a3)
{
  /* t2=d29c(a0) t1=d298(a0) t9=d29c(a1) t3=d298(a1)
   * t5=d29c(a2) v0=d298(a2); mult+mflo+sra 12 (signed, >>12 arith) */
  int32_t t2 = lut_sin_q12(a0);
  int32_t t1 = lut_cos_q12(a0);
  int32_t t9 = lut_sin_q12(a1);
  int32_t t3 = lut_cos_q12(a1);
  int32_t t5 = lut_sin_q12(a2);
  int32_t v0 = lut_cos_q12(a2);
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

/* anim_node_alloc — allocate/init an animation node pair via
 * tween_seq_alloc for slot s0=a1 (signed index into ctx arrays).
 * ctx a0 (guest addr): node @+8 (fC mode @+0xc must be 1|2 for 2nd pass,
 * f10 flag @+0x10), arrays @+0x1c/+0x20/+0x34/+0x38 (u32 slots, idx*4),
 * count @+0x44, aux ptrs @+0x14/+0x28, mode2 @+0x2c.
 * Returns 1 ok / 0 fail. Disasm 0x801a7038-0x801a7298. */
int anim_node_alloc(uint32_t a0,int32_t a1)
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
    uint32_t r = (uint32_t)tween_seq_alloc(0,
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
      uint32_t r = (uint32_t)tween_seq_alloc(0,
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
      task_node_free((int *)(uintptr_t)nd);
      *(uint32_t *)guest_at(arr + idx) = 0;
      return 0;
    }
  }
  return 1;
}

/* op_anim_delta_walk — walk two anim slots (@ctx+0x34/+0x38, first record
 * only) accumulating fixed-point deltas; when cumulative quotient
 * reaches clamped arg s2, emit tween_apply(node, idx, pos).
 * Signed lh/div semantics preserved (mflo trunc, sra, +0xfff fixup).
 * Disasm 0x801a984c-0x801a99dc. */
int op_anim_delta_walk(uint32_t a0,int32_t a1)
{
  uint8_t *s3;
  int32_t s2;
  int32_t s1;
  if (a0 == 0) return 0;
  s3 = (uint8_t *)guest_at(a0);
  s2 = a1;
  if (s2 < 0) s2 = 0;
  if (s2 >= 0x1001) s2 = 0x1000;
  s1 = 0;
  for (;;) {
    /* 0x801A988C slot head: s1==0 -> [s3+0x34], s1==1 -> [s3+0x38]. */
    uint32_t arr = *(uint32_t *)(s3 + (s1 == 0 ? 0x34 : 0x38));
    uint32_t s0g = *(uint32_t *)guest_at(arr);
    uint8_t *s0;
    int32_t cnt, a1c = 0, a2c = 0;
    uint32_t a0o = 0, a3u = 0;
    if (s0g == 0) goto slot_done;              /* beqz s0 -> 0x801A99B4 */
    s0 = (uint8_t *)guest_at(s0g);
    cnt = *(int16_t *)(s0 + 6);                /* lh cnt,[s0+6] */
    if (cnt < 0) goto slot_done;               /* bltz -> 0x801A99B4 */
    for (;;) {                                 /* 0x801A98E8 loop top */
      uint32_t ev = *(uint32_t *)guest_at(
          *(uint32_t *)(s0 + 0x18) + a0o + 4);
      int32_t dv = *(int16_t *)(s0 + 0xc);     /* lh -> sign-extended */
      int32_t num, quo;
      a3u += ev;                               /* gpr[7] += ev (wraps) */
      num = (int32_t)(a3u << 12);              /* sll v0,a3,12 */
      /* div @0x801A9904 + break @0x801A9910 (recompiler no-op): dv==0
       * FALLS THROUGH with LO = num>=0 ? -1 : 1 into the normal
       * mflo/slt/advance path — it does NOT exit the loop.
       * INT_MIN/-1 -> LO = INT_MIN per MIPS (no trap). */
      if (dv != 0) {
        quo = (num == (int32_t)0x80000000 && dv == -1)
              ? (int32_t)0x80000000 : num / dv;
      } else {
        quo = (num >= 0) ? -1 : 1;
      }
      /* mflo @0x801A992C; slt quo<s2; xori 1; beqz -> advance @0x801A9998,
       * else finish @0x801A9944 (i.e. quo >= s2). */
      if (quo < s2) {
        /* advance: a1c++, a2c++, a0o += 0x14; cnt reloaded each step. */
        a1c++;
        a2c++;
        a0o += 0x14;
        cnt = *(int16_t *)(s0 + 6);
        if (cnt < a2c) break;                  /* -> 0x801A99B4 slot_done */
        continue;
      }
      /* finish @0x801A9944-0x801A998C: t0 (gpr[8]) provably 0, so
       * q2 = (s2<<12)/1; +0xfff only if negative (impossible, s2>=0);
       * a2 = sra(q2,12) = s2; jal tween_apply(s0, a1c, s2). */
      {
        int32_t q2 = s2 << 12;
        if (q2 < 0) q2 += 0xfff;
        tween_apply((short *)s0,a1c,q2 >> 12);
      }
      break;                                   /* -> 0x801A9990 s1++ */
    }
  slot_done:
    s1++;                                      /* 0x801A99B4 */
    if (s1 >= 2) break;                        /* slti s1,2 @0x801A99B8 */
  }
  return 0;
}

/* op_ctx_node_attach — lazily attach node a1 to ctx a0: alloc via
 * node_lazy_attach(0x30-byte rec, cb 0x801aadc4, dtor 0x801aad18), link
 * into *(*(a0+0x18)+0x10) list, cache at a0+0x48. Returns node or 0.
 * Disasm 0x801aad24-0x801aadc0. */
int op_ctx_node_attach(uint32_t a0,uint32_t a1)
{
  uint8_t *s0;
  uint32_t v0, r, A, v1g, nd;
  if (a0 == 0) return 0;
  s0 = (uint8_t *)guest_at(a0);
  v0 = *(uint32_t *)(s0 + 0x48);
  if (v0 != 0) return (int)v0;
  if (a1 == 0) return 0;
  r = (uint32_t)(uintptr_t)node_lazy_attach((int)*(uint32_t *)(s0 + 0x18),
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

/* op_node_alloc_init — alloc/init node via task_node_alloc from table
 * 0x801ff620[a2][a3]; node fields from source s0 and stack arg a4.
 * Returns allocated guest addr or 0. Disasm 0x801ad2e8-0x801ad388. */
int op_node_alloc_init(int32_t a0,int32_t a1,int32_t a2,int32_t a3,uint32_t a4)
{
  uint32_t row = *(uint32_t *)guest_at(0x801ff620 + (uint32_t)a2 * 4);
  uint32_t s0  = *(uint32_t *)guest_at(row + (uint32_t)a3 * 4);
  uint32_t r = (uint32_t)(uintptr_t)task_node_alloc(a0,a1,4,
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

/* dev_byte_xfer — SIO/memory-card byte send/recv (0x801a1f74, 528B/20
 * blocks). a0 = host ctx (fields +0x3c buf guest ptr, +0x40 alt guest
 * ptr, +0x44/+0x45 counters — ctx itself is a host pointer into guest
 * RAM); a1 = byte (-2 = query/flush path). Globals: dev ptr cell
 * 0x801BE7E4 -> SIO regs 0x1F801040 (byte buf +0, u16 status +4,
 * u16 cmd +0xe, u16 +0xa), ctrl ptr cell 0x801BE7E0 -> 0x1F801070
 * (u32 status bit7). All register-block accesses go through
 * br2_ldN/br2_stN — the cells resolve to 0x1F801xxx hardware, not
 * guest RAM (same contract as dev_cmd_seq/dev_cmd_commit). The three
 * status polls are hardware-timed, so each pass runs br2_irq_poll +
 * br2_bail_pending exactly like the generated body's per-block
 * psx_check_interrupts_at. The 0x1F801120 FIFO-count read +
 * 0x801FF260/64 base/limit stores at 0x801A2054-0x801A2078 are the
 * inlined fifo_space_sample(0x1AE) — identical order (hw read, lim
 * store, count store), so the clean helper is called directly.
 * Disasm generated/SLES_017.22_full_49.c:0x801A1F74-0x801A2180. */
extern void fifo_space_sample(uint32_t);
int32_t dev_byte_xfer(uint8_t *a0,int32_t a1)
{
  extern int br2_bail_pending(void);
  uint8_t *s0 = a0;
  uint32_t dev, ctrl, buf;
  int32_t s1, s2;
  if (a1 < 0) {
    /* query/flush path 0x801A1F98-0x801A2018. */
    dev = br2_ld32(0x801be7e4u);                    /* 0x801A1F9C */
    buf = *(uint32_t *)(s0 + 0x40);                 /* 0x801A1FA0 alt ptr */
    s2 = (int32_t)br2_ld8(dev);                     /* 0x801A1FA4 lbu dev[0] */
    s0[0x44] = 0xff;                                /* 0x801A1FAC */
    s0[0x45] = 1;                                   /* 0x801A1FB4 */
    br2_st8(buf, (uint8_t)~a1);                     /* 0x801A1FBC *alt=~a1 */
    dev = br2_ld32(0x801be7e4u);                    /* 0x801A1FC4 reload */
    /* 0x801A1FE0: wait while (lhu(dev+4) & 1) == 0. */
    while ((br2_ld16(dev + 4u) & 1u) == 0) {
      if (br2_bail_pending()) return -20;
      br2_irq_poll();
    }
    /* 0x801A1FF4/0x801A1FFC: retry fifo_space_check until nonzero. */
    while (fifo_space_check() == 0) {
      if (br2_bail_pending()) return -20;
      br2_irq_poll();
    }
    dev = br2_ld32(0x801be7e4u);                    /* 0x801A2008 reload */
    br2_st8(dev, (uint8_t)~a1);                     /* 0x801A2010 dev[0]=~a1 */
    return s2;                                      /* v0=s2 @0x801A2018 */
  }
  /* send path 0x801A201C-0x801A2164. */
  buf = *(uint32_t *)(s0 + 0x3c);                   /* 0x801A201C buf ptr */
  s1 = 0x88;                                        /* 0x801A2034 li s1,0x88 */
  /* 0x801A2024-0x801A204C: nib = lbu(buf)>>4; s1=0x22 iff nib==8 and
   * ctx->f44 >= 9 (sltiu), else stays 0x88. */
  if (((int32_t)br2_ld8(buf) >> 4) == 8 &&
      (uint32_t)(uint8_t)s0[0x44] >= 9u)
    s1 = 0x22;
  fifo_space_sample(0x1aeu);                        /* 0x801A2054-78 inline */
  dev = br2_ld32(0x801be7e4u);                      /* 0x801A205C */
  /* 0x801A2088: wait while (lhu(dev+4) & 2) == 0. */
  while ((br2_ld16(dev + 4u) & 2u) == 0) {
    if (br2_bail_pending()) return -20;
    br2_irq_poll();
  }
  /* 0x801A209C: dev/ctrl reload, capture dev[0], program cmd reg. */
  dev  = br2_ld32(0x801be7e4u);                     /* 0x801A20A0 */
  ctrl = br2_ld32(0x801be7e0u);                     /* 0x801A20A8 */
  s2 = (int32_t)br2_ld8(dev);                       /* 0x801A20AC lbu dev[0] */
  br2_st16(dev + 0xeu, (uint16_t)s1);               /* 0x801A20B0 sh cmd */
  /* 0x801A20B4-0x801A20C4: if ((*ctrl & 0x80) == 0) enter the
   * fifo_space_check/ctrl-bit7 wait loop 0x801A20C8-0x801A20F4. */
  if ((br2_ld32(ctrl) & 0x80u) == 0) {
    for (;;) {
      if (fifo_space_check() != 0) return -20;      /* 0x801A20D0 -> -0x14 */
      ctrl = br2_ld32(0x801be7e0u);                 /* 0x801A20DC reload */
      if ((br2_ld32(ctrl) & 0x80u) != 0) break;     /* 0x801A20F0 */
      if (br2_bail_pending()) return -20;
      br2_irq_poll();
    }
  }
  /* 0x801A20F8: dev[0] = a1 (delay-slot byte store ahead of the s1 test). */
  dev = br2_ld32(0x801be7e4u);                      /* 0x801A20FC reload */
  br2_st8(dev, (uint8_t)a1);                        /* 0x801A2104 */
  if (s1 == 0x22) {                                 /* 0x801A210C-0x801A2114 */
    ctrl = br2_ld32(0x801be7e0u);                   /* 0x801A2118 */
    dev  = br2_ld32(0x801be7e4u);                   /* 0x801A2120 */
    br2_st32(ctrl, 0xffffff7fu);                    /* 0x801A2124 *ctrl */
    br2_st16(dev + 0xau,
             (uint16_t)(br2_ld16(dev + 0xau) | 0x10u)); /* 0x801A2128-134 */
  }
  /* 0x801A2138-0x801A2164: advance counters; buf[c44] = old dev[0]. */
  {
    uint32_t c45 = (uint32_t)(uint8_t)s0[0x45];    /* 0x801A2138 lbu */
    uint32_t c44 = (uint32_t)(uint8_t)s0[0x44];    /* 0x801A213C lbu */
    buf = *(uint32_t *)(s0 + 0x3c);                 /* 0x801A2140 reload */
    s0[0x45] = (uint8_t)(c45 + 1u);                 /* 0x801A214C */
    br2_st8(buf + c44, (uint8_t)s2);                /* 0x801A2150 sb s2 */
    /* 0x801A2154: guest RELOADS s0[0x44] after the buf store — the sb
     * above can alias s0+0x44, so do not reuse the stale c44. */
    s0[0x44] = (uint8_t)((uint32_t)(uint8_t)s0[0x44] + 1u); /* 0x801A2160 */
  }
  return s2;                                        /* v0=s2 @0x801A2164 */
}

/* fifo_space_sample — FIFO space baseline/limit setter (0x801A4210).
 * Single block, no calls: v0 = lhu 0x1F801120 (hardware FIFO count,
 * zero-extended), then *(u32*)0x801FF264 = a0 (limit) and
 * *(u32*)0x801FF260 = v0 (count snapshot). Those are exactly the
 * base/limit cells fifo_space_check @0x801A4230 consumes. The second
 * store sits in the jr-ra delay slot — both stores complete before
 * return, order between them is unobservable (distinct addresses).
 * Disasm generated/SLES_017.22_full_50.c: lui/ori 0x1F801120, lhu,
 * sw a0->0x801FF264, sw v0->0x801FF260 (delay slot), jr ra. */
void fifo_space_sample(uint32_t limit)
{
  uint32_t cnt = (uint32_t)psx_read_half(0x1f801120);
  *(uint32_t *)guest_at(0x801ff264) = limit;
  *(uint32_t *)guest_at(0x801ff260) = cnt;
}

/* qbyte_take — guest 0x801A39AC (16 bytes, 1 block, leaf):
 *   v0 = lb(a0+55); sb $zero -> a0+55; (jr-ra delay slot) sb v0 -> a0+56.
 * Net effect: byte at +56 = old byte at +55, byte at +55 = 0 — a
 * take-and-clear of a status/queue byte into its shadow slot.
 * a0 is a guest address; lb is sign-extended but the stored byte is
 * identical either way. Disasm generated/SLES_017.22_full_50.c. */
void qbyte_take(uint32_t a0)
{
  uint8_t *p = (uint8_t *)guest_at(a0);
  uint8_t t = p[55];
  p[55] = 0;
  p[56] = t;
}

/* fifo_space_check — FIFO space check. a0 = u16 count @0x1f801120 (+ u16
 * @0x1f801128, or +0x10000 when that is 0) when count < limit
 * @0x801ff260; then (a0 - base) compared against limit @0x801ff264,
 * raw when flag u16 @0x1f801124 & 0x200 else >>3 logical; returns
 * !(v0 < lim) i.e. 1 when space exhausted. Disasm 0x801a4230-0x801a42cc.
 * The 0x1f8011xx reads are absolute (lui $v0,0x1f80; ori $v0,$v0,0x1120/24/28)
 * — hardware register addresses outside the 1KB scratchpad, kept as-is. */
int32_t fifo_space_check(void)
{
  uint32_t a0 = (uint32_t)(uint16_t)psx_read_half(0x1f801120);
  uint32_t base = *(uint32_t *)guest_at(0x801ff260);
  uint32_t lim2 = *(uint32_t *)guest_at(0x801ff264);
  uint32_t v0;
  if (a0 < base) {
    uint32_t c2 = (uint32_t)(uint16_t)psx_read_half(0x1f801128);
    a0 += (c2 != 0) ? c2 : 0x10000u;
  }
  v0 = a0 - base;
  if (((uint16_t)psx_read_half(0x1f801124) & 0x200) == 0)
    v0 >>= 3;
  return (v0 < lim2) ? 0 : 1;
}

/* dev_cmd_commit — SIO/device command commit (guest 0x801A23FC, 144B,
 * 6 blocks). No args: works off the device pointer cells.
 *   ctrl = *(u32*)0x801BE7E0  (-> 0x1F801070 SIO control, MMIO)
 *   dev  = *(u32*)0x801BE7E4  (-> SIO register block, MMIO)
 *   *ctrl = 0xFFFFFF7F                  (clear ctrl bit7)
 *   while (lh(dev+4) & 0x80):           (TX-busy status bit)
 *       r = fifo_space_check();  if (r) return r;
 *       dev = *(u32*)0x801BE7E4;        (reload — callee may rewrite)
 *   sh(dev+10, lh(dev+10) | 0x10)       (set command bit4)
 *   return 1                            (li v0,1 in the delay-slot
 *                                         sequence before jr ra)
 * All register-block accesses go through br2_ldN/br2_stN — the cells
 * resolve to 0x1F801xxx hardware, not guest RAM (same contract as
 * dev_byte_xfer). The wait loop calls fifo_space_check which is itself
 * clean; the generated body CPS-checks interrupts between the jal
 * boundaries, and a clean call preserves that ordering exactly.
 * Disasm generated/SLES_017.22_full_49.c: sw 0xFFFFFF7F @0x801A2418,
 * lh+andi 0x80 loop @0x801A241C-0x801A2458, jal 0x801A4230 @0x801A2430,
 * lh|0x10 sh @0x801A2460-0x801A2478, li v0,1 @0x801A2470. */
uint32_t dev_cmd_commit(void)
{
  uint32_t ctrl = br2_ld32(0x801be7e0u);
  uint32_t dev  = br2_ld32(0x801be7e4u);
  uint32_t r;
  br2_st32(ctrl, 0xffffff7fu);
  while ((br2_ld16(dev + 4u) & 0x80u) != 0) {
    r = (uint32_t)fifo_space_check();
    if (r != 0) return r;
    dev = br2_ld32(0x801be7e4u);
  }
  dev = br2_ld32(0x801be7e4u);   /* B2460 reloads the cell unconditionally */
  br2_st16(dev + 10u, (uint16_t)(br2_ld16(dev + 10u) | 0x10u));
  return 1;
}

/* dev_stat_bit1_wait — SIO status bit1 poll (guest 0x801A248C, 40B/3
 * blocks, leaf). dev = *0x801BE7E4 loaded once; loop { v0 = lhu(dev+4)
 * & 2; while v0 == 0 repeat } — exits when status bit1 sets, returns
 * the masked value (always 2). The poll is hardware-timed, so each
 * pass goes through br2_irq_poll + br2_bail_pending (same contract as
 * dev_cmd_byte_send's identical wait). Disasm mipsel objdump:
 * lui/lw 0x801BE7E4, lhu+andi 0x2 @+4, beqz back-edge, jr ra. */
uint32_t dev_stat_bit1_wait(void)
{
  extern int br2_bail_pending(void);
  uint32_t dev = br2_ld32(0x801be7e4u);
  uint32_t v0;
  for (;;) {
    v0 = (uint32_t)br2_ld16(dev + 4u) & 2u;
    if (v0 != 0) break;
    if (br2_bail_pending()) return v0;
    br2_irq_poll();
  }
  return v0;
}

/* dev_cmd_seq — SIO/memory-card per-slot command sequencer (guest
 * 0x801A1B50, ~205 insns spanning CPS pieces 1B50/1BC4/1BEC/1C48/1C58/
 * 1C90/1CAC/1CCC/1D4C/1D50/1D54/1D5C/1D98/1DC8/1DD4/1DD8/1E3C/1E4C/1E58/
 * 1E5C/1E70). a0 = 240-byte queue record (0x801FF080+slot*240), a1 passes
 * through live to every indirect call.
 *
 * Phase 1 — device setup + pending-record drain:
 *   dev = *0x801BE7E4; sh 0x40->dev+10, sh 0->dev+10, sh 0xd->dev+8,
 *   sh 0x88->dev+14 (all 16-bit stores, unconditional order).
 *   fifo_space_sample(rec[0xE8]==8 ? 0x50 : 0x91).
 *   slot = *0x801BE7C0; dev->h10 = (slot==0 ? 0x1003 : 0x3003).
 *   n = *(0x801BE7D8 + slot*4)  (signed pending count).
 *   n < 0  -> skip drain AND the -1/callback pair, go to send phase.
 *   n > 0  -> drain loop: while tbl[slot] > 0 { tbl[slot]--; jalr
 *     *0x801BE7A0( rec->fC + (old-1)*240 ) with ra=0x801A1C34 } — the
 *     record stride is 240 (v0*15<<4). tbl[slot] and slot are RELOADED
 *     from their cells every iteration (the callback may rewrite them).
 *   n <= 0 -> recheck tbl[slot]: nonzero -> send phase; zero -> store
 *     -1 into tbl[slot] (jalr delay slot, BEFORE the call), then
 *     jalr *0x801BE7A0(rec) ra=0x801A1C98 and jalr *0x801BE7A4(rec)
 *     ra=0x801A1CAC. a1-a3 pass through live (br2_call_generated leaves
 *     caller-saved regs as the callee left them — the guest contract).
 *
 * Phase 2 — command send (only when dev status lhu(dev+4) & 0x200):
 *   dev->h10 |= 0x10; if bit9 cleared meanwhile -> *0x801BE7E0 =
 *   0xFFFFFF7F and tail. Else while (fifo_space_check()==0) retry
 *   (a0=2000 from the branch delay slot — dead: clean fifo_space_check
 *   reads the hardware counters itself). dev->b0 = 1 (delay-slot store
 *   ahead of the call), fifo_space_sample(2000), dev_cmd_commit();
 *   commit==0 -> return 0. dev_stat_bit1_wait(); fifo_space_sample(0x1ae)
 *   (preceded by a dead lbu dev->b0 — reproduced: the read itself may
 *   matter on MMIO, the value is discarded by the jal return).
 *
 * Phase 3/4 — two identical wait+send rounds: while !(*ctrl & 0x80)
 *   { if (fifo_space_check()!=0) return 0 } (a0=60, dead arg);
 *   round A sends dev->b0='B' then sample(60)/commit/stat_wait/
 *   sample(0x1ae); round B sends dev->b0=1 then sample(60)/commit/
 *   stat_wait and returns 0. Every commit failure returns 0.
 *
 * Tail (0x801A1E4C, reached when bit9 was clear or after phase 4):
 *   rec->f50==0 -> return 1; else rec->f37!=0 -> return 0; else 1.
 * Callers (cd_cmd_queue_service / cd_cmd_queue_drain) treat 0 as
 * "still busy, call again" — the whole send path returns 0.
 * Disasm: capstone of disc/SLES_017.22 @0x801A1B50-0x801A1E80,
 * cross-checked vs generated/SLES_017.22_full_49.c. */
uint32_t dev_cmd_seq(uint32_t rec, uint32_t a1)
{
  extern int br2_bail_pending(void);
  extern void br2_irq_poll(void);
  CPUState *c = clean_cpu_state();
  uint32_t dev, slot, n, fn, saved_ra;
  (void)a1;
  if (!c) return 0;

  dev = br2_ld32(0x801be7e4u);
  br2_st16(dev + 10u, 0x40u);
  br2_st16(dev + 10u, 0);
  br2_st16(dev +  8u, 0x0du);
  br2_st16(dev + 14u, 0x88u);

  fifo_space_sample((br2_ld8(rec + 0xe8u) == 8u) ? 0x50u : 0x91u);

  slot = br2_ld32(0x801be7c0u);
  dev  = br2_ld32(0x801be7e4u);              /* reload (B1BB0) */
  br2_st16(dev + 10u, (slot == 0u) ? 0x1003u : 0x3003u);

  n = (uint32_t)(int32_t)br2_ld32(0x801be7d8u + (slot << 2));
  if ((int32_t)n < 0) goto L_send;         /* bltz -> 0x801A1CAC */
  if (n != 0) {
    /* drain loop 0x801A1BEC-0x801A1C54: exits when tbl[slot] <= 0 and
     * FALLS THROUGH to the 0x801A1C58 recheck below (not straight to
     * send) — a drained-to-zero slot still runs the -1 + callbacks. */
    for (;;) {
      uint32_t s  = br2_ld32(0x801be7c0u);
      uint32_t tp = 0x801be7d8u + (s << 2);
      uint32_t cnt = (uint32_t)(int32_t)br2_ld32(tp);
      uint32_t idx = cnt - 1u;
      uint32_t arg;
      br2_st32(tp, idx);                    /* tbl[slot] = count-1 */
      arg = br2_ld32(rec + 0xcu) + idx * 240u;
      fn = br2_ld32(0x801be7a0u);
      saved_ra = c->gpr[31];
      c->gpr[31] = 0x801a1c34u;              /* jalr return PC */
      br2_call_clean(fn, arg, c->gpr[5], c->gpr[6], c->gpr[7]);
      c->gpr[31] = saved_ra;
      cnt = (uint32_t)(int32_t)
          br2_ld32(0x801be7d8u + (br2_ld32(0x801be7c0u) << 2));
      if ((int32_t)cnt <= 0) break;
    }
  }
  /* 0x801A1C58 recheck: tbl[slot]!=0 -> send; ==0 -> -1 store + cbs. */
  slot = br2_ld32(0x801be7c0u);
  n    = br2_ld32(0x801be7d8u + (slot << 2));
  if (n != 0) goto L_send;
  br2_st32(0x801be7d8u + (slot << 2), 0xffffffffu);   /* jalr delay slot */
  fn = br2_ld32(0x801be7a0u);
  saved_ra = c->gpr[31];
  c->gpr[31] = 0x801a1c98u;
  br2_call_clean(fn, rec, c->gpr[5], c->gpr[6], c->gpr[7]);
  c->gpr[31] = saved_ra;
  fn = br2_ld32(0x801be7a4u);
  saved_ra = c->gpr[31];
  c->gpr[31] = 0x801a1cacu;
  br2_call_clean(fn, rec, c->gpr[5], c->gpr[6], c->gpr[7]);
  c->gpr[31] = saved_ra;

L_send:
  dev = br2_ld32(0x801be7e4u);
  if ((br2_ld16(dev + 4u) & 0x200u) == 0) goto L_tail;
  br2_st16(dev + 10u, (uint16_t)(br2_ld16(dev + 10u) | 0x10u));
  /* 0x801A1CDC-0x801A1CE8: second bit9 test reuses the same dev (v1). */
  if ((br2_ld16(dev + 4u) & 0x200u) == 0) {
    /* 0x801A1E3C: *ctrl = 0xFFFFFF7F (v0 = -129 from the delay slot) */
    br2_st32(br2_ld32(0x801be7e0u), 0xffffff7fu);
    goto L_tail;
  }
  /* 0x801A1CF0: retry fifo_space_check until nonzero (a0=2000 in the
   * branch delay slot is dead — the clean check reads hw counters). */
  while (fifo_space_check() == 0) {
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }
  dev = br2_ld32(0x801be7e4u);
  br2_st8(dev, 1);                           /* delay-slot sb, pre-call */
  fifo_space_sample(2000);
  if (dev_cmd_commit() == 0) return 0;
  (void)dev_stat_bit1_wait();
  dev = br2_ld32(0x801be7e4u);
  (void)br2_ld8(dev);                        /* dead lbu dev->b0 */
  fifo_space_sample(0x1aeu);

  /* wait loop A (0x801A1D5C): while !(*ctrl & 0x80) { check!=0 -> ret 0 } */
  for (;;) {
    uint32_t ctrl = br2_ld32(0x801be7e0u);
    if ((br2_ld32(ctrl) & 0x80u) != 0) break;
    if (fifo_space_check() != 0) return 0;
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }
  dev = br2_ld32(0x801be7e4u);
  br2_st8(dev, 0x42u);                        /* 'B', delay-slot store */
  fifo_space_sample(60);
  if (dev_cmd_commit() == 0) return 0;
  (void)dev_stat_bit1_wait();
  dev = br2_ld32(0x801be7e4u);
  (void)br2_ld8(dev);                        /* dead lbu dev->b0 */
  fifo_space_sample(0x1aeu);

  /* wait loop B (0x801A1DD8): identical shape */
  for (;;) {
    uint32_t ctrl = br2_ld32(0x801be7e0u);
    if ((br2_ld32(ctrl) & 0x80u) != 0) break;
    if (fifo_space_check() != 0) return 0;
    if (br2_bail_pending()) return 0;
    br2_irq_poll();
  }
  dev = br2_ld32(0x801be7e4u);
  br2_st8(dev, 1);                           /* delay-slot sb, pre-call */
  fifo_space_sample(60);
  if (dev_cmd_commit() == 0) return 0;
  (void)dev_stat_bit1_wait();
  /* 0x801A1E24-0x801A1E38: dev reload + dead lbu dev->b0 into v1,
   * v0 = 0 -> return. */
  (void)br2_ld8(br2_ld32(0x801be7e4u));
  return 0;

L_tail:
  /* 0x801A1E4C: rec->f50==0 -> 1; else rec->f37!=0 -> 0; else 1. */
  if (br2_ld8(rec + 0x50u) == 0) return 1;
  if (br2_ld8(rec + 0x37u) != 0) return 0;
  return 1;
}

/* dev_cmd_process — memcard/SIO command processor (guest 0x801A3350,
 * 300B/75 blocks, single entry; CPS pieces 3350-36E0). a0 = command ctx
 * record (same 240-byte family as dev_cmd_seq's rec). Cell map:
 *   *0x801BE804 = enable flag / s4 arg source (reloaded after first cb)
 *   *0x801BE78C = per-record cb, *0x801BE788 = byte-request cb (a1 = arg),
 *   *0x801BE7A0/*0x801BE7A4 = drain/ctx cbs (same pair as dev_cmd_seq),
 *   *0x801BE780 = completion cb, *0x801BE7B4 = record-table base,
 *   *0x801BE7C0 = slot, *0x801BE7D8 = per-slot count tbl,
 *   *0x801BE7F8 = transaction counter, *0x801BE7FC = status ptr (+0xE h).
 * Every jalr parks $ra on its guest return PC so br2_call_generated's
 * nested dispatch stops at the mid-function continuation; a1-a3 pass
 * through live. Direct jals go to already-clean dev_byte_xfer /
 * fifo_space_sample / dev_cmd_commit / dev_stat_bit1_wait.
 * Disasm: capstone disc/SLES_017.22 @0x801A3350-0x801A36E0. */
uint32_t dev_cmd_process(uint32_t ctx)
{
  CPUState *c = clean_cpu_state();
  uint32_t s0, s1, s2, s3, s4, fn, r, saved_ra;
  if (!c) return 0;
  s1 = ctx;

  /* 0x801A3350: if (*BE804) cb78C(ctx); s4 = *BE804 (reload). */
  if (br2_ld32(0x801be804u) != 0) {
    fn = br2_ld32(0x801be78cu);
    saved_ra = c->gpr[31];
    c->gpr[31] = 0x801a3394u;
    br2_call_clean(fn, ctx, c->gpr[5], c->gpr[6], c->gpr[7]);
    c->gpr[31] = saved_ra;
  }
  s4 = br2_ld32(0x801be804u);
  s0 = 0xffffffffu;                            /* s0 = -1 */
  s2 = 0xfffffff0u;                            /* s2 = -0xf0 */

  if (s4 != 0) {
    /* send loop 0x801A33AC-0x801A3434: cell-- unconditional (delay
     * slot), exit when <=0; up to 4 record sends (s0=-1..3, s2 step
     * 0xF0). bltz s0 skips the cb78C call on the first pass only. */
    for (;;) {
      uint32_t v = br2_ld32(0x801be7f8u) - 1u;
      br2_st32(0x801be7f8u, v);                /* delay-slot store */
      if ((int32_t)v <= 0) break;
      if ((int32_t)s0 >= 0) {
        uint32_t a = br2_ld32(s1 + 0xcu) + s2;
        fn = br2_ld32(0x801be78cu);
        saved_ra = c->gpr[31];
        c->gpr[31] = 0x801a33e8u;
        br2_call_clean(fn, a, c->gpr[5], c->gpr[6], c->gpr[7]);
        c->gpr[31] = saved_ra;
      }
      /* 0x801A33E8: cb788(ctx,1) then byte_xfer(ctx, ret&0xff). */
      fn = br2_ld32(0x801be788u);
      saved_ra = c->gpr[31];
      c->gpr[31] = 0x801a3400u;
      r = br2_call_clean(fn, s1, 1u, c->gpr[6], c->gpr[7]);
      c->gpr[31] = saved_ra;
      r = (uint32_t)dev_byte_xfer((uint8_t *)guest_at(s1),
                                  (int32_t)(r & 0xffu));
      if ((int32_t)r < 0) return r;            /* -> epilogue v0=r */
      fifo_space_sample(60);
      if (dev_cmd_commit() == 0) return 0xfffffffdu;   /* -3 */
      s0 += 1u;                                /* delay slot: always */
      if ((int32_t)s0 >= 4) break;
      s2 += 0xf0u;                             /* delay slot on loop */
    }
  }

  /* 0x801A3438: slot-based dispatch setup. */
  s0 = 0;
  {
    uint32_t slot = br2_ld32(0x801be7c0u);
    uint32_t cnt  = br2_ld32(0x801be7f8u);
    uint32_t a0i;
    if ((int32_t)cnt < 2) goto L_collect;      /* bnez -> 0x801A35DC */
    a0i = (slot < 1u) ? 1u : 0u;               /* sltiu a0,slot,1 */
    s2 = 0x801be7d8u + (a0i << 2);             /* &tbl[a0i] */
    s3 = a0i * 240u;
    /* s5 = 3 (guest li) — the ==3 state compare below is literal. */
  }

  /* 0x801A347C: per-slot state dispatch (outer loop via 0x801A35B8). */
  for (;;) {
    int32_t st = (int32_t)br2_ld32(s2);
    if (st < 0) goto L_collect;
    if (st > 0) {
      /* drain one record: ptr = *BE7B4 + s3; arg = ptr->fC + st*240
       * - 0xF0 == ptr->fC + (st-1)*240; s0 = arg (persists). */
      uint32_t base = br2_ld32(0x801be7b4u) + s3;
      uint32_t arg  = br2_ld32(base + 0xcu) + (uint32_t)st * 240u - 0xf0u;
      s0 = arg;
      fn = br2_ld32(0x801be7a0u);
      saved_ra = c->gpr[31];
      c->gpr[31] = 0x801a34c8u;
      br2_call_clean(fn, arg, c->gpr[5], c->gpr[6], c->gpr[7]);
      c->gpr[31] = saved_ra;
    }
    /* 0x801A34C8: reload state, dispatch on value. */
    st = (int32_t)br2_ld32(s2);
    if (st == 3) {
      /* 0x801A3510: cb7A0(s0-0xF0); then *s2 = 1 (via 0x801A3568). */
      fn = br2_ld32(0x801be7a0u);
      saved_ra = c->gpr[31];
      c->gpr[31] = 0x801a3524u;
      br2_call_clean(fn, s0 - 0xf0u, c->gpr[5], c->gpr[6], c->gpr[7]);
      c->gpr[31] = saved_ra;
      br2_st32(s2, 1u);
      goto L_commitA;
    }
    if (st >= 0 && st < 2) {
      /* 0x801A352C: s0 = *BE7B4 + s3; cb7A0(s0); cb7A4(s0); *s2 = -1. */
      s0 = br2_ld32(0x801be7b4u) + s3;
      fn = br2_ld32(0x801be7a0u);
      saved_ra = c->gpr[31];
      c->gpr[31] = 0x801a3550u;
      br2_call_clean(fn, s0, c->gpr[5], c->gpr[6], c->gpr[7]);
      c->gpr[31] = saved_ra;
      fn = br2_ld32(0x801be7a4u);
      saved_ra = c->gpr[31];
      c->gpr[31] = 0x801a3564u;
      br2_call_clean(fn, s0, c->gpr[5], c->gpr[6], c->gpr[7]);
      c->gpr[31] = saved_ra;
      br2_st32(s2, 0xffffffffu);
      goto L_commitA;
    }
    if (st == 4) {
      /* 0x801A34FC-0x801A350C: *s2 = 3 (delay-slot store) then send. */
      br2_st32(s2, 3u);
    }
    /* all other states (2, >4, negative recheck) -> 0x801A3570 send. */
L_commitA:
    /* 0x801A3570: cb788(ctx, s4); byte_xfer(ctx, ret&0xff). */
    fn = br2_ld32(0x801be788u);
    saved_ra = c->gpr[31];
    c->gpr[31] = 0x801a3584u;
    r = br2_call_clean(fn, s1, s4, c->gpr[6], c->gpr[7]);
    c->gpr[31] = saved_ra;
    r = (uint32_t)dev_byte_xfer((uint8_t *)guest_at(s1),
                                (int32_t)(r & 0xffu));
    if ((int32_t)r < 0) return r;
    fifo_space_sample(60);
    if (dev_cmd_commit() == 0) return 0xfffffffdu;
    /* 0x801A35B8: cell-- unconditional; >=2 -> loop 0x801A347C. */
    {
      uint32_t v = br2_ld32(0x801be7f8u) - 1u;
      br2_st32(0x801be7f8u, v);
      if ((int32_t)v >= 2) continue;
    }
    break;
  }

L_collect:
  /* 0x801A35DC: response-collection loop while cell > 0. */
  {
    uint32_t v = br2_ld32(0x801be7f8u) - 1u;
    br2_st32(0x801be7f8u, v);                  /* delay-slot store */
    if ((int32_t)v > 0) {
      s0 = 0x22;
      for (;;) {
        /* 0x801A35FC: cb788(ctx,s4); byte_xfer(ctx,ret&0xff). */
        fn = br2_ld32(0x801be788u);
        saved_ra = c->gpr[31];
        c->gpr[31] = 0x801a3614u;
        r = br2_call_clean(fn, s1, s4, c->gpr[6], c->gpr[7]);
        c->gpr[31] = saved_ra;
        r = (uint32_t)dev_byte_xfer((uint8_t *)guest_at(s1),
                                    (int32_t)(r & 0xffu));
        if ((int32_t)r < 0) return r;
        /* 0x801A3628: lhu (*BE7FC)+0xE == 0x22 -> skip commit. */
        {
          uint32_t sp = br2_ld32(0x801be7fcu);
          if (br2_ld16(sp + 0xeu) != s0) {
            fifo_space_sample(60);
            if (dev_cmd_commit() == 0) return 0xfffffffdu;
          }
        }
        /* 0x801A365C: cell-- unconditional; >0 -> loop. */
        v = br2_ld32(0x801be7f8u) - 1u;
        br2_st32(0x801be7f8u, v);
        if ((int32_t)v <= 0) break;
      }
    }
  }

  /* 0x801A3678: stat wait, bump ctx->f44, stash *BE7FC status byte0 at
   * ctx->f3C + old_f44, then cb780(0); return 0. */
  (void)dev_stat_bit1_wait();
  {
    uint32_t old = br2_ld8(s1 + 0x44u);
    uint32_t sp  = br2_ld32(0x801be7fcu);   /* status ptr (0x801A3694) */
    uint32_t b   = br2_ld8(sp);             /* lbu (v0) -> status byte0 */
    br2_st8(s1 + 0x44u, (uint8_t)(old + 1u));
    br2_st8(br2_ld32(s1 + 0x3cu) + old, (uint8_t)b);
    fn = br2_ld32(0x801be780u);
    saved_ra = c->gpr[31];
    c->gpr[31] = 0x801a36bcu;
    br2_call_clean(fn, 0u, c->gpr[5], c->gpr[6], c->gpr[7]);
    c->gpr[31] = saved_ra;
  }
  return 0;
}

/* dev_cmd_round_dispatch — guest 0x801A1E84 (SIO command round-robin,
 * spans the CPS-split pieces 0x801A1E84/1EDC/1EE0/1F38/1F50/1F64).
 * Round counter cell @0x801BE7C4 indexes a guest fn-ptr table at
 * 0x801BE808 (+idx*4 via the 0x801C0000-6136 window idiom); the counter is
 * bumped BEFORE the call. The table fn is jalr'd with a1 = &counter and
 * ra = 0x801A1EC0 (mid-function continuation — reproduced by parking
 * $ra on that guest PC so br2_call_generated's dispatch stops there).
 * On a negative result the error fn *@0x801BE780 runs with a0 = result,
 * ra = 0x801A1F64 (function exit). Otherwise: counter==0 -> return -1;
 * counter!=0 -> unless counter==3 AND ctx[60]->byte0 == 128, run
 * fifo_space_sample(60) + dev_cmd_commit(); a zero commit result runs the
 * error fn with a0 = -3, ra = 0x801A1F2C. Tail (0x801A1F38): v0 =
 * counter-1 unconditionally (addiu delay slot); if counter < 5 (signed
 * slti) return v0 with NO store, else store counter = v0 and return v0.
 * a2/a3 pass through to every indirect call from the LIVE gpr file —
 * br2_call_generated leaves callee-clobbered arg regs in place, matching
 * the guest's caller-saved contract. a1 is likewise read live (the guest
 * never rewrites it after the prologue, but callees may clobber it).
 * Disasm generated/SLES_017.22_full_49.c 0x801A1E84-0x801A1F70. */

/* Per-call trace ring for the round-robin table read. Records the index,
 * resolved table address and loaded fn pointer for the most recent calls so
 * a freeze dump can show whether a wild fn (e.g. 0x801BCBA0) came from an
 * out-of-bounds counter index or an overwritten table slot. Diagnostic only —
 * no behaviour change. */
extern uint32_t g_drr_idx_ring[64];   /* defined in runtime freeze_heartbeat.c */
extern uint32_t g_drr_tbl_ring[64];
extern uint32_t g_drr_fn_ring[64];
extern volatile uint32_t g_drr_ring_pos;

uint32_t dev_cmd_round_dispatch(uint32_t ctx, uint32_t a2, uint32_t a3)
{
  CPUState *c = clean_cpu_state();
  uint32_t idx, fn, r, v1, saved_ra;
  if (!c) return 0;

  idx = br2_ld32(0x801be7c4u);
  fn  = br2_ld32(0x801c0000u + (idx << 2) - 6136u);  /* 0x801BE808+idx*4 */
  {
    uint32_t pos = g_drr_ring_pos & 63u;
    g_drr_idx_ring[pos] = idx;
    g_drr_tbl_ring[pos] = 0x801c0000u + (idx << 2) - 6136u;
    g_drr_fn_ring[pos]  = fn;
    g_drr_ring_pos = pos + 1u;
  }
  br2_st32(0x801be7c4u, idx + 1u);

  /* jalr table[idx] — ra = 0x801A1EC0 continuation. */
  saved_ra = c->gpr[31];
  c->gpr[31] = 0x801a1ec0u;
  r = br2_call_clean(fn, ctx, 0x801be7c4u, a2, a3);
  c->gpr[31] = saved_ra;

  if ((int32_t)r < 0) {
    /* 0x801A1F50: error fn *@0x801BE780, a0 = negative result,
     * ra = 0x801A1F64 (epilogue). */
    fn = br2_ld32(0x801be780u);
    saved_ra = c->gpr[31];
    c->gpr[31] = 0x801a1f64u;
    r = br2_call_clean(fn, r, c->gpr[5], c->gpr[6], c->gpr[7]);
    c->gpr[31] = saved_ra;
    return r;
  }

  v1 = br2_ld32(0x801be7c4u);
  if (v1 == 0) {
    /* beq -> 0x801A1F38 with v1 still 0: v0 = v1-1 = -1, bne -> ret. */
    return 0xffffffffu;
  }

  {
    int run_jals = 1;
    if (v1 == 3) {
      /* lw v0,0x3C(s0); lbu v1,0(v0); beq v1,128 -> wrap (skip jals). */
      uint32_t p = br2_ld32(ctx + 60u);
      run_jals = (*(volatile uint8_t *)guest_at(p) != 128);
    }
    if (run_jals) {
      /* jal 0x801A4210 (a0=60) then jal 0x801A23FC — both callees are
       * already clean ports in this file (fifo_space_sample /
       * dev_cmd_commit); the guest jal would dispatch to them through
       * clean_table anyway, so call them directly. a1-a3 are dead for
       * both callees (neither reads them), matching the live-pass
       * contract. */
      fifo_space_sample(60u);
      r = dev_cmd_commit();
      if (r == 0) {
        /* 0x801A1F18: error fn *@0x801BE780, a0 = -3,
         * ra = 0x801A1F2C (wrap-check continuation). */
        fn = br2_ld32(0x801be780u);
        saved_ra = c->gpr[31];
        c->gpr[31] = 0x801a1f2cu;
        br2_call_clean(fn, 0xfffffffdu,
                           c->gpr[5], c->gpr[6], c->gpr[7]);
        c->gpr[31] = saved_ra;
      }
    }
  }

  /* 0x801A1F2C/0x801A1F38: reload counter; v0 = v1-1 always (delay slot).
   * slti v1,5 -> return v0 untouched; else store counter = v0 first. */
  v1 = br2_ld32(0x801be7c4u);
  r = v1 - 1u;
  if ((int32_t)v1 >= 5) br2_st32(0x801be7c4u, r);
  return r;
}

/* dev_cmd_byte_send — SIO/memory-card command-byte exchange (guest
 * 0x801A2184, 632B/33 blocks, single-entry — the hottest unnamed device
 * fn in the versus fight: ~348 dispatches/45s, always jalr'd through the
 * dev_cmd_round_dispatch table with a0 = device ctx (0x801FF080/170),
 * a1 = command byte).
 *
 * Body decoded insn-by-insn (mipsel objdump of disc/SLES_017.22 .text):
 *   s0 = a0 ctx, s3 = a1 cmd byte.
 *   buf = lw(ctx+0x3C); nib = lbu(buf)>>4 (sra, but lbu <= 0xFF so == lsr).
 *   s1 = 136; the li s1,136 delay slot of the nib!=8 branch is
 *   UNCONDITIONAL, then s1 = 34 only when nib==8 AND lbu(ctx+0x44) >= 9.
 *   dev = *0x801BE7E4; poll lhu(dev+4)&2 == 0 (SIO status bit1, same
 *   wait as dev_stat_bit1_wait @0x801A248C) -> fifo_space_sample(400).
 *   dev reloaded; s2 = lbu(dev+0) & 0xFF (always — the andi is the bnez
 *   delay slot); cmd = (lbu(ctx+0x44) != 0) ? s1
 *        : ((s2 >> 4) == 8 ? 34 : s1);  sh cmd,14(dev).
 *   ctrl = *0x801BE7E0; if ((*ctrl & 0x80) == 0) loop { fifo space
 *   exhausted -> return -2; until *ctrl & 0x80 } — the space test is
 *   fifo_space_check() inlined against the same 0x801FF260/64 cells and
 *   0x1F801120/124/128 regs, so the clean helper is reused verbatim.
 *   if (lbu(ctx+0xE8) != 8 && *0x801BE7C4 == 2) { fifo_space_sample(60);
 *   while (!fifo_space_check()) }.
 *   dev reloaded; sb s3,0(dev) — command byte out.
 *   if (*0x801BE7C4 == 3 && s2 == 128) { *ctrl = 0xFFFFFF7F;
 *   dev[10] |= 0x10 } (same commit tail as dev_cmd_commit).
 *   ctx[0x45]++; if (ctx[0x44] != 255) buf[ctx[0x44]] = s2;
 *   ctx[0x44]++; return s2 (v0 = -2 on the fifo-exhausted bail).
 *
 * The status polls are hardware-timed waits: each iteration goes through
 * br2_irq_poll (advance_cycles + psx_check_interrupts_at) plus the
 * br2_bail_pending early-out — the established cd.c/dev_cmd_commit
 * contract; a raw clean-C spin would starve the deferred CD/SIO
 * coroutine. */
int32_t dev_cmd_byte_send(void *a0_ctx,int32_t a1_byte)
{
  extern int br2_bail_pending(void);
  uint8_t *s = (uint8_t *)a0_ctx;
  uint32_t buf = *(uint32_t *)(s + 0x3c);
  uint32_t dev, ctrl;
  uint32_t s1, cmd, b44;
  int32_t s2;

  /* nibble test: buf[0]>>4 == 8 && ctx[0x44] >= 9 -> cmd id 34 else 136 */
  s1 = (((uint32_t)(int32_t)br2_ld8(buf) >> 4) == 8u &&
        (uint32_t)*(uint8_t *)(s + 0x44) >= 9u) ? 34u : 136u;

  /* SIO status bit1 wait (identical poll to 0x801A248C) */
  dev = br2_ld32(0x801be7e4u);
  while ((br2_ld16(dev + 4u) & 2u) == 0) {
    if (br2_bail_pending()) return -1;
    br2_irq_poll();
  }
  fifo_space_sample(400u);

  /* response byte + command select; s2 is captured unconditionally */
  dev = br2_ld32(0x801be7e4u);
  s2 = (int32_t)(br2_ld8(dev) & 0xffu);
  b44 = (uint32_t)*(uint8_t *)(s + 0x44);
  cmd = (b44 != 0u) ? s1 : (((uint32_t)s2 >> 4) == 8u ? 34u : s1);
  br2_st16(dev + 14u, (uint16_t)cmd);

  /* FIFO-space / ctrl-bit7 wait; exhausted -> v0 = -2 */
  ctrl = br2_ld32(0x801be7e0u);
  if ((br2_ld32(ctrl) & 0x80u) == 0) {
    for (;;) {
      if (fifo_space_check() != 0) return -2;
      if ((br2_ld32(ctrl) & 0x80u) != 0) break;
      if (br2_bail_pending()) return -1;
      br2_irq_poll();
    }
  }

  if ((uint32_t)*(uint8_t *)(s + 0xe8) != 8u &&
      br2_ld32(0x801be7c4u) == 2u) {
    fifo_space_sample(60u);
    while (fifo_space_check() == 0) {
      if (br2_bail_pending()) return -1;
      br2_irq_poll();
    }
  }

  /* command byte out, then the conditional commit tail */
  dev = br2_ld32(0x801be7e4u);
  br2_st8(dev, (uint8_t)a1_byte);
  if (br2_ld32(0x801be7c4u) == 3u && s2 == 128) {
    ctrl = br2_ld32(0x801be7e0u);
    br2_st32(ctrl, 0xffffff7fu);
    dev = br2_ld32(0x801be7e4u);
    br2_st16(dev + 10u, (uint16_t)(br2_ld16(dev + 10u) | 0x10u));
  }

  /* ring-buffer bookkeeping: ctx[0x45] seq++, buf[ctx[0x44]] = reply,
   * ctx[0x44] index++ (skipped at the 255 wrap). */
  *(uint8_t *)(s + 0x45) = (uint8_t)(*(uint8_t *)(s + 0x45) + 1u);
  b44 = (uint32_t)*(uint8_t *)(s + 0x44);
  if (b44 != 255u) br2_st8(buf + b44, (uint8_t)s2);
  *(uint8_t *)(s + 0x44) = (uint8_t)(b44 + 1u);
  return s2;
}

/* ======================================================================
 * batch37: unnamed save-cluster leaf stubs & field ops @0x8015/0x8016.
 * All bodies decoded insn-by-insn from generated/SLES_017.22_full_*.c.
 * FUN_ names are deliberate — renames.map has no entry for these guest
 * addresses; gen_fnmap maps the FUN_<addr> spelling automatically.
 * The 0x801638xx run is the disabled save-slot op table (same family as
 * ret_zero_a/b/c @0x80163850/98/a0): each entry is `jr ra` with the
 * result-producing insn in the delay slot.
 * ====================================================================== */

extern uint32_t call_guest(uint32_t,uint32_t,uint32_t);

/* 0x80163840 — bare `jr ra; nop`: v0 passes through untouched. */
void FUN_80163840(void) { }
/* 0x80163848 — bare `jr ra; nop`: v0 passes through untouched. */
void FUN_80163848(void) { }
/* 0x80163858 — `jr ra; li v0,-99` (delay slot): returns -99.
 * Same sentinel value as memcard_err_sentinel @0x80163868. */
int32_t FUN_80163858(uint32_t a0) { (void)a0; return -99; }
/* 0x80163860/70/78/80/88/90/a8/b0/b8/c0 — `jr ra; move v0,zero`:
 * disabled save-slot ops, faithful return-0 (NOT stubs — the guest
 * literally zeroes v0 in the delay slot). */
int32_t FUN_80163860(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_80163870(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_80163878(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_80163880(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_80163888(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_80163890(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_801638a8(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_801638b0(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_801638b8(uint32_t a0) { (void)a0; return 0; }
int32_t FUN_801638c0(uint32_t a0) { (void)a0; return 0; }
/* 0x80163940 — shared `jr ra; nop` exit tail of the 16-byte record
 * copy family (0x80163948/90/d8/20/68); standalone entry = void ret. */
void FUN_80163940(void) { }
/* 0x80163b7c — `jr ra; nop`. */
void FUN_80163b7c(void) { }
/* 0x80163bb8 — `jr ra; nop`. */
void FUN_80163bb8(void) { }
/* 0x80163b40 — `jr ra; li v0,-1`: returns -1. */
int32_t FUN_80163b40(uint32_t a0) { (void)a0; return -1; }
/* 0x801631fc — `jr ra; move v0,zero`: not-found tail of the
 * 0x801631b4/1d0 record-chase; faithful return-0. */
int32_t FUN_801631fc(uint32_t a0) { (void)a0; return 0; }
/* 0x80163248 — `jr ra; move v0,zero`: same not-found tail family. */
int32_t FUN_80163248(uint32_t a0) { (void)a0; return 0; }
/* 0x8015a1dc — `jr ra; nop`. */
void FUN_8015a1dc(void) { }

/* 0x80163250 — `lw v0,0(a0); jr ra`: return *(u32*)a0. */
uint32_t FUN_80163250(uint32_t a0)
{
  return *(uint32_t *)guest_at(a0);
}

/* 0x80157358 — `lui v0,0x801c; lbu v0,-0x1091(v0); jr ra`:
 * returns byte @0x801bef6f (memcard busy flag). */
uint32_t FUN_80157358(void)
{
  return DAT_801bef6f;
}
/* 0x801573dc — same shape, byte @0x801bef6d (memcard cmd byte). */
uint32_t FUN_801573dc(void)
{
  return DAT_801bef6d;
}
/* 0x80157444 — copies u16 @0x801bef70 -> @0x801bef76 (reload the
 * memcard brightness countdown), returns base 0x801bef60 in v0. */
uint32_t FUN_80157444(void)
{
  *(uint16_t *)guest_at(0x801bef76) = *(uint16_t *)guest_at(0x801bef70);
  return 0x801bef60u;
}

/* 0x80157574 — memcard state-flag nudge on the @0x801bef60 block:
 * if busy flag f[0xf]==1: when (f[0xe]&0xf)==2 arm cmd f[0xd]=0x10,
 * then clear f[0xf]. v0 carries the last immediate (1 / 2 / 0x10). */
int32_t FUN_80157574(void)
{
  uint8_t *f = (uint8_t *)guest_at(0x801bef60);
  if (f[0xf] != 1) return 1;
  if ((f[0xe] & 0xf) == 2) {
    f[0xd] = 0x10;
    f[0xf] = 0;
    return 0x10;
  }
  f[0xf] = 0;
  return 2;
}

/* 0x80158274 — channel flag toggle @0x801f46f4 (the true entry;
 * renames.map's chan_flag_toggle @0x8015827c is the mid-prologue
 * alias): *f = (*f != 0) ? 0 : 0x200, then cd_reg_rw_d2(*f) and
 * cd_reg_bit40_ctrl(1). Prologue saves ra / sp-0x18 — plain subroutine,
 * no IRQ or deferred-coroutine interaction, so call_guest is faithful. */
void FUN_80158274(uint32_t a0, uint32_t a1)
{
  uint32_t *f = (uint32_t *)guest_at(0x801f46f4);
  (void)a0;
  if (*f != 0) *f = 0; else *f = 0x200;
  call_guest(0x8019d170, *f, 0);
  call_guest(0x8019d030, 1, a1);
}

/* 0x80159f54 — `sh a1,0xa0(a0)` then falls into 0x80159f58's `jr ra`:
 * store low16 of a1 at guest a0+0xa0. */
void FUN_80159f54(uint32_t a0, uint32_t a1)
{
  *(uint16_t *)guest_at(a0 + 0xa0) = (uint16_t)a1;
}
/* 0x80159f58 — `jr ra; nop`. */
void FUN_80159f58(void) { }
/* 0x80159f60 — signed/unsigned pair op on u16 @a0+0xa0:
 * d = (s16)field - a1 decides clamp-to-0 (bltz taken -> store 0 via the
 * 0x80159f7c tail); the delay slot always computes r = (u16)field - a1
 * which is what actually gets stored on the not-taken path AND is the
 * value left in v0 on both paths. */
uint32_t FUN_80159f60(uint32_t a0, uint32_t a1)
{
  int32_t d = (int32_t)*(int16_t *)guest_at(a0 + 0xa0) - (int32_t)a1;
  uint32_t r = *(uint16_t *)guest_at(a0 + 0xa0) - a1;
  *(uint16_t *)guest_at(a0 + 0xa0) = (d < 0) ? 0 : (uint16_t)r;
  return r;
}
/* 0x80159f7c — `jr ra; sh zero,0xa0(a0)` (delay slot): clear the
 * u16 field at guest a0+0xa0. */
void FUN_80159f7c(uint32_t a0)
{
  *(uint16_t *)guest_at(a0 + 0xa0) = 0;
}

/* 0x8016fca8 — `jr ra; sh zero,0xec(a0)` (delay slot): clear u16
 * @a0+0xec. */
void FUN_8016fca8(uint32_t a0)
{
  *(uint16_t *)guest_at(a0 + 0xec) = 0;
}
/* 0x8016fcb0 — u16 increment @a0+0xec: v0 = old+1 stored and returned
 * (addiu result is NOT masked; sh truncates the store only). */
uint32_t FUN_8016fcb0(uint32_t a0)
{
  uint32_t r = *(uint16_t *)guest_at(a0 + 0xec) + 1;
  *(uint16_t *)guest_at(a0 + 0xec) = (uint16_t)r;
  return r;
}

/* ---- 16-byte record copy family, guest 0x80163948-0x80163aac ----
 * Five leaf copies sharing one shape: `blez a2` tails to the shared
 * `jr ra` at 0x80163940, else copy a2 records of four words (16 bytes).
 * Direction/source differ per entry; two variants take the second
 * pointer in $t0 (marshalled via ARG_GPRS in gen_clean_table.py).
 * Disasm verified against generated/SLES_017.22_full_25.c. */

static void rec16_copy(uint32_t dst, uint32_t src, int32_t n)
{
  uint32_t *d = (uint32_t *)guest_at(dst);
  const uint32_t *s = (const uint32_t *)guest_at(src);
  int32_t i;
  for (i = 0; i < n; i++, s += 4, d += 4) {
    d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3];
  }
}

/* 0x80163948 — copy a2 recs a0 -> a3 (guest moves a1=a3 as dst cursor). */
void rec_tbl_copy_a0_a3(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  (void)a1;
  if ((int32_t)a2 <= 0) return;
  rec16_copy(a3, a0, (int32_t)a2);
}

/* 0x80163990 — copy a2 recs a3 -> a0 (guest moves a1=a3 as src cursor). */
void rec_tbl_copy_a3_a0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  (void)a1;
  if ((int32_t)a2 <= 0) return;
  rec16_copy(a0, a3, (int32_t)a2);
}

/* 0x801639d8 — copy a2 recs a0 -> $t0 (guest moves a1=t0 as dst cursor). */
void rec_tbl_copy_a0_t0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t t0)
{
  (void)a1;
  if ((int32_t)a2 <= 0) return;
  rec16_copy(t0, a0, (int32_t)a2);
}

/* 0x80163a20 — copy a2 recs $t0 -> a0 (guest moves a1=t0 as src cursor). */
void rec_tbl_copy_t0_a0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t t0)
{
  (void)a1;
  if ((int32_t)a2 <= 0) return;
  rec16_copy(a0, t0, (int32_t)a2);
}

/* 0x80163a68 — copy a2 recs from fixed table 0x801b431c -> a0. */
void rec_tbl_copy_dfl_a0(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
  (void)a1; (void)a3;
  if ((int32_t)a2 <= 0) return;
  rec16_copy(a0, 0x801b431cu, (int32_t)a2);
}

/* ======================================================================
 * batch38: remaining save-cluster leaf entries @0x8015/0x8016.
 * All bodies decoded insn-by-insn from generated/SLES_017.22_full_*.c.
 * FUN_ names are deliberate — renames.map has no entries for these guest
 * addresses; gen_fnmap maps the FUN_<addr> spelling automatically.
 * Functions whose live inputs arrive in NON-a0-a3 registers ($v0/$v1/$t0/
 * $a3/$t3/$a2) are marshalled via ARG_GPRS in gen_clean_table.py — each
 * entry verified against the generated body cited per function.
 * ====================================================================== */

/* 0x80152260 — `li v0,1` then falls through into 0x80152264's `jr ra;nop`:
 * returns 1. (generated/SLES_017.22_full_17.c) */
int32_t FUN_80152260(void) { return 1; }
/* 0x80152264 — bare `jr ra; nop`: v0 passes through untouched. */
void FUN_80152264(void) { }
/* 0x80153534 — `jr ra; move v0,zero` (delay slot): faithful return-0. */
int32_t FUN_80153534(void) { return 0; }
/* 0x8015353c — bare `jr ra; nop`: v0 passes through untouched. */
void FUN_8015353c(void) { }
/* 0x80156d90 — `jr ra; li v0,1` (delay slot): returns 1. */
int32_t FUN_80156d90(void) { return 1; }
/* 0x8015647c — `jr ra; li v0,1` (delay slot): scan-exhausted exit of the
 * 0x80156458 record-flag scan, returns 1. */
int32_t FUN_8015647c(void) { return 1; }

/* 0x80156458 — stride-0x10 record flag scan. Live inputs: a0 = running
 * index, a1 = signed limit, $v1 = table base (NOT an a0-a3 arg —
 * ARG_GPRS (4,5,3)). Each pass: b = lbu 0(v1); a0++ (unconditional,
 * delay slot of the beq); b!=0 -> return 0; else v1 += 0x10 and loop
 * while a0 < a1 (signed slt); on exhaustion tail into 0x8015647c ->
 * return 1. Always examines >=1 record (no pre-test). a0/v1 mutations
 * are call-clobbered and unobservable post-return. Leaf: guest RAM
 * loads only, bounded by a1-a0 — no IRQ/coroutine interaction.
 * Disasm generated/SLES_017.22_full_19.c:0x80156458-0x80156483. */
uint32_t FUN_80156458(uint32_t a0, uint32_t a1, uint32_t v1)
{
  uint8_t *p = (uint8_t *)guest_at(v1);
  for (;;) {
    uint32_t b = p[0];
    a0++;
    if (b != 0) return 0;
    p += 0x10;
    if ((int32_t)a0 < (int32_t)a1) continue;
    return 1;
  }
}

/* 0x80156470 — scan loop latch / re-entry. slt v0,a0,a1; delay slot
 * v1 += 0x10 (unconditional); a0<a1 -> branch back to 0x80156458 with
 * the bumped v1, else -> 0x8015647c return 1. Same live-reg contract
 * as FUN_80156458 (ARG_GPRS (4,5,3)). Disasm full_19:0x80156470-0x80156483. */
uint32_t FUN_80156470(uint32_t a0, uint32_t a1, uint32_t v1)
{
  v1 += 0x10;
  if ((int32_t)a0 < (int32_t)a1)
    return FUN_80156458(a0, a1, v1);
  return 1;
}

/* 0x80158514 — field hand-off leaf. Live inputs: $v0 = source struct
 * (ARG_GPRS (2,4) param0), a0 = dest struct. v1 = *(u32*)(v0+0x36e8);
 * *(u32*)(a0+0x3ed4) = 0; *(u32*)0x801f46f0 = v1 (the lui'd v0 base +
 * 0x46f0, stored in the jr-ra delay slot); returns 0x801f0000.
 * Disasm generated/SLES_017.22_full_20.c:0x80158514-0x80158527. */
uint32_t FUN_80158514(uint32_t v0, uint32_t a0)
{
  uint32_t v1 = *(uint32_t *)guest_at(v0 + 0x36e8);
  *(uint32_t *)guest_at(a0 + 0x3ed4) = 0;
  *(uint32_t *)guest_at(0x801f46f0) = v1;
  return 0x801f0000u;
}

/* 0x80163220 — offset-table entry resolve (sibling of the 0x801631d0
 * record chase). Live inputs: $v0 = base index, a0 = addend, a2 = slot
 * (ARG_GPRS (2,4,6)). base = v0+a0; v1 = *(u32*)(base+4); v1 == -1 ->
 * tail 0x80163248 -> return 0; else return a0 + ((a2+1)<<2) + v1 + 12
 * (the +12 sits in the jr-ra delay slot, applied to the updated a0).
 * Disasm generated/SLES_017.22_full_25.c:0x80163220-0x80163247. */
uint32_t FUN_80163220(uint32_t v0, uint32_t a0, uint32_t a2)
{
  uint32_t v1 = *(uint32_t *)guest_at(v0 + a0 + 4u);
  if (v1 == 0xffffffffu) return 0;
  return a0 + (((a2 + 1u) << 2) + v1) + 12u;
}

/* 0x801631b4 — guarded head of the 0x801631d0 record chase. a2 =
 * *(u32*)a0 (entry count); slt v0,a1,a2 signed decides the branch —
 * the delay slot sll v0,a1,2 supplies the offset the chase consumes.
 * a1 >= count -> return 0 (0x801631c8 tail); else chase with
 * offset = a1*4 and slot = count. Disasm full_25:0x801631b4-0x801631cf. */
uint32_t FUN_801631b4(uint32_t a0, uint32_t a1)
{
  uint32_t cnt = *(uint32_t *)guest_at(a0);
  uint32_t v0, v1;
  if ((int32_t)a1 >= (int32_t)cnt) return 0;
  v0 = a1 << 2;
  /* == 0x801631D0 body with a2 = cnt */
  v0 += a0;
  v1 = *(uint32_t *)guest_at(v0 + 4u);
  if (v1 == 0xffffffffu) return 0;      /* -> 0x801631fc tail */
  a0 += (((cnt + 1u) << 2) + v1);
  return *(uint32_t *)guest_at(a0 + 8u);
}

/* 0x801631d0 — record chase continuation. Live inputs: $v0 = pre-shifted
 * offset (a1*4 from the 0x801631b4 delay slot, or caller-supplied on a
 * direct entry), a0 = base, a2 = slot (ARG_GPRS (2,4,6)). v0 += a0;
 * v1 = *(u32*)(v0+4); v1 == -1 -> 0x801631fc -> return 0; else
 * a0 += ((a2+1)<<2) + v1 and return *(u32*)(a0+8) (jr-ra delay slot
 * is a nop — the load IS the return value).
 * Disasm full_25:0x801631d0-0x801631fb. */
uint32_t FUN_801631d0(uint32_t v0, uint32_t a0, uint32_t a2)
{
  uint32_t v1;
  v0 += a0;
  v1 = *(uint32_t *)guest_at(v0 + 4u);
  if (v1 == 0xffffffffu) return 0;
  a0 += (((a2 + 1u) << 2) + v1);
  return *(uint32_t *)guest_at(a0 + 8u);
}

/* 0x80163b10 — 0x10-stride record scan head (sibling of the ported
 * rec_find_lt_f8 @0x80163b08). Live inputs: a0 = cursor, a1 = signed
 * field limit, a2 = count, $v1 = running index (ARG_GPRS (4,5,6,3)).
 * Loop: f = *(u32*)(a0+8); f==0 -> return v1; f>=a1 (signed slt in the
 * delay slot, tested at 0x80163b28) -> return v1; else v1++, a0 += 0x10
 * and continue while v1 < a2; exhausted -> 0x80163b40 -> return -1.
 * Disasm full_25:0x80163b10-0x80163b3f. */
int32_t FUN_80163b10(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t v1)
{
  for (;;) {
    uint32_t f = *(uint32_t *)guest_at(a0 + 8u);
    if (f == 0) return (int32_t)v1;
    if ((int32_t)f >= (int32_t)a1) return (int32_t)v1;
    v1++;
    a0 += 0x10;
    if ((int32_t)v1 >= (int32_t)a2) return -1;
  }
}

/* 0x80163b20 — `jr ra; move v0,v1` (delay slot): returns $v1 verbatim
 * (the not-found/early-exit tail of the 0x80163b10 scan).
 * ARG_GPRS (3,). Disasm full_25:0x80163b20-0x80163b27. */
uint32_t FUN_80163b20(uint32_t v1)
{
  return v1;
}

/* 0x80163b28 — scan latch / re-entry. Live inputs: $v0 = the slt result
 * computed at 0x80163b1c (f<a1), $v1 = index, a0 = cursor, a1 = field
 * limit, a2 = count (ARG_GPRS (2,3,4,5,6)). beq v0,zero -> 0x80163b20
 * return v1; else v1++, a0 += 0x10 (delay slot) and loop back through
 * the 0x80163b10 head while v1 < a2; exhausted -> 0x80163b40 -> -1.
 * Disasm full_25:0x80163b28-0x80163b3f. */
int32_t FUN_80163b28(uint32_t v0, uint32_t v1, uint32_t a0,
                     uint32_t a1, uint32_t a2)
{
  if (v0 == 0) return (int32_t)v1;
  for (;;) {
    uint32_t f;
    v1++;
    a0 += 0x10;
    if ((int32_t)v1 >= (int32_t)a2) return -1;
    /* == 0x80163B10 loop head */
    f = *(uint32_t *)guest_at(a0 + 8u);
    if (f == 0) return (int32_t)v1;
    if ((int32_t)f >= (int32_t)a1) return (int32_t)v1;
  }
}

/* 0x80159f44 — conditional +0xa0 u16 field set (same family as the
 * ported FUN_80159f54/60/7c). Live inputs: $v0 = select flag, a0 =
 * struct, a1 = value (ARG_GPRS (2,4,5)). v0!=0 -> store a1 at a0+0xa0
 * (the 0x80159f54 tail); v0==0 -> store 0xff. Delay slot li v0,0xff
 * leaves v0 = 0xff on BOTH paths -> return 0xff.
 * Disasm generated/SLES_017.22_full_21.c:0x80159f44-0x80159f53. */
uint32_t FUN_80159f44(uint32_t v0, uint32_t a0, uint32_t a1)
{
  if (v0 != 0) {
    *(uint16_t *)guest_at(a0 + 0xa0) = (uint16_t)a1;
  } else {
    *(uint16_t *)guest_at(a0 + 0xa0) = 0xff;
  }
  return 0xff;
}

/* 0x801574cc — +0xe flag-nibble rw on the struct in $v1 (ARG_GPRS
 * (4,3): a0 = set/clear flag). a0!=0 -> f[0xe] |= 0x10; a0==0 ->
 * f[0xe] &= 0xef; then reload and return the byte (zero-extended).
 * Disasm generated/SLES_017.22_full_20.c:0x801574cc-0x801574fb. */
uint32_t FUN_801574cc(uint32_t a0, uint32_t v1)
{
  uint8_t *f = (uint8_t *)guest_at(v1 + 0xe);
  uint32_t b;
  if (a0 != 0) b = (uint32_t)(*f | 0x10);
  else         b = (uint32_t)(*f & 0xef);
  *f = (uint8_t)b;
  return (uint32_t)*f;
}

/* 0x801597e0 — u16 pair store: sh a3,0x2c(t0); sh t3,0x2e(t0) (the
 * second store is the jr-ra delay slot — both complete). Live inputs
 * in $t0/$a3/$t3 (ARG_GPRS (8,7,11)). No return value (v0 passthrough).
 * Disasm generated/SLES_017.22_full_20.c:0x801597e0-0x801597eb. */
void FUN_801597e0(uint32_t t0, uint32_t a3, uint32_t t3)
{
  *(uint16_t *)guest_at(t0 + 0x2c) = (uint16_t)a3;
  *(uint16_t *)guest_at(t0 + 0x2e) = (uint16_t)t3;
}

/* 0x80167b20 — byte-range compare driver. Live inputs: a1 = start
 * index, a2 = srcA base, a3 = srcB base (ARG_GPRS (5,6,7)).
 * sltiu v0,a1,32 unsigned: a1 >= 32 -> return 0; else per index compare
 * byte[a3+i] vs byte[a2+i] (lbu both), equal -> i++ loop, differ ->
 * return 1. Disasm generated/SLES_017.22_full_27.c:0x80167b20-0x80167b33. */
uint32_t FUN_80167b20(uint32_t a1, uint32_t a2, uint32_t a3)
{
  while (a1 < 32) {
    if (*(uint8_t *)guest_at(a3 + a1) != *(uint8_t *)guest_at(a2 + a1))
      return 1;
    a1++;
  }
  return 0;
}

/* 0x80167b00 — loop-body entry of the 0x80167b20 compare. Same live
 * regs (ARG_GPRS (5,6,7)). One compare at the CURRENT a1: differ ->
 * return 1; equal -> a1++ (delay slot) and continue via 0x80167b20.
 * Disasm full_27:0x80167b00-0x80167b1f. */
uint32_t FUN_80167b00(uint32_t a1, uint32_t a2, uint32_t a3)
{
  if (*(uint8_t *)guest_at(a3 + a1) != *(uint8_t *)guest_at(a2 + a1))
    return 1;
  return FUN_80167b20(a1 + 1, a2, a3);
}
