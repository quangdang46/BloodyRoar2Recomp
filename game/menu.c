/* menu.c — menu/options/save-manager cluster.
 * Verbatim Ghidra ports from decomp/batch_sm2.txt.
 * Guest globals bound via g_DAT_* (bind.c arena, payload-backed). */
#include <stdint.h>
#include <stdio.h>
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
typedef void code(void);
typedef unsigned long long ulonglong;
typedef long long longlong;
#define CONCAT11(a,b) ((uint16_t)((((a) & 0xff) << 8) | ((b) & 0xff)))
#define CONCAT12(a,b) ((uint32_t)((((a) & 0xff) << 16) | ((b) & 0xffff)))
#define CONCAT22(a,b) ((uint32_t)((((a) & 0xffff) << 16) | ((b) & 0xffff)))
#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint32_t br2_gaddr(const void *p);
extern void br2_st8(uint32_t a, uint8_t v);
extern uint32_t guest_sp(void);
extern uint32_t br2_guest_frame(uint32_t);
extern void br2_guest_frame_free(void);
extern uint8_t *_g_system_config_ptr;
extern uint8_t *_g_pad_buffer;
extern CPUState *clean_cpu_state(void);
/* g_game_state/g_sub_state are guest scratchpad bytes at 0x1f800000/0x1f800002
 * (the top-level mode SM read by entry_0x80126234 in game.c). They must alias
 * the SAME guest storage — declaring them `extern int` bound them to throwaway
 * host ints in bind.c, so menu writes (e.g. g_game_state=8 on mode confirm)
 * never reached the dispatcher and battle_flow_state_machine never ran. */
extern uint8_t *g_DAT_1f800000;
extern uint8_t *g_DAT_1f800002;
#define g_game_state (*(uint8_t *)g_DAT_1f800000)
#define g_sub_state  (*(uint8_t *)g_DAT_1f800002)

extern int rgb_triple_store(uint8_t r,uint8_t g,uint8_t b);
extern int reset_sprite_ot_block(uint32_t ctx);
extern void record_state_stepper(uint32_t);
extern void menu_stepper_state_advance(void *);
extern void menu_stepper_button_wait(void *,uint32_t);
extern int frame_present(intptr_t ctx);
extern uint32_t banner_stripe_scroll(int32_t,int32_t,int32_t,int16_t);
extern int intro_movie_state(int32_t which);
extern int hud_gauge_render(uint32_t st_g, uint32_t head_g, uint32_t variant);
extern int print_mode_dispatch_b(int32_t mode,uint32_t a1,uint32_t a2);
extern int scroll_offset_set(int32_t x,int32_t y);
extern int scroll_offset_accumulate(int16_t x,int16_t y);
extern int sprite_desc_table_init();
extern void announce_param_block_init(uint32_t,uint32_t,const void *,uint32_t,uint32_t,int32_t);
extern void announce_param_block_init_alt(uint32_t,uint32_t,const void *,uint32_t,uint32_t,int32_t);
extern void rec_copy_init(uint32_t);
extern void battle_struct_init(uint32_t,uint32_t);
extern void battle_field_set(uint32_t,int32_t);
extern int32_t per_side_setup(int32_t,int32_t);
extern int clear_f0300_records(void);
extern void record_copy_1a(int);
extern int hud_sprite_desc_init(void);
extern int task_table_init(int32_t arg);
extern int reset_combat_slots(void);
extern int FUN_801468ec(uint32_t a0,uint32_t a1,uint32_t mat,const void *ctrl,int32_t mode);
extern int battle_frame(void);
extern int32_t select_screen_tick(int32_t entry_mode);
void select_confirm_handler(uint32_t,uint32_t,uint8_t *,uint32_t,int32_t);
void post_confirm_config_build(uint32_t,uint8_t *,int32_t);
extern int battle_init(uint32_t battle, uint8_t *side_a, uint8_t *side_b);
extern int confirm_bookkeeping(uint32_t battle_g);
extern void battle_state_init(uint8_t *,uint8_t *,uint32_t);
extern uint32_t mc_pad_button_handler(uint8_t *battle_g);
extern int32_t post_battle_dispatch(uint8_t *side_a,uint8_t *side_b,uint32_t battle);
extern void battle_sides_commit(uint8_t *,uint8_t *,uint32_t);
extern int blk_1f26c0_init_b(void);
extern int blk_1f26c0_init(void);
extern int se_value_select(uint32_t a0);
extern int brightness_packet_send(int32_t val);
extern int fighter_rec_fetch_apply(uint32_t a0,uint32_t a1,uint32_t a2);
extern int mc_init_reset(void);
extern int mc_mode_nibble_rw(int32_t a0);
extern int32_t mc_dialog_bit4(int32_t);
extern int mc_confirm_dispatch(int32_t a0,uint32_t a1);
extern int confirm_code_select();
extern int select_phase_update(uint32_t a0);
extern int match_setup_apply(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3);
extern int menu_sprite_load_sm(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t a4);
extern int menu_stepper_sm(uint8_t *pad);
extern uint32_t mode3_dispatch(uint32_t,uint32_t);
extern int memcard_err_sentinel(void);
extern int rec_find_lt_f8(uint8_t *a0,uint32_t a1,int32_t a2);
extern int rec_find_ge_e(uint8_t *a0,int32_t a1,int32_t a2);
extern int rec_u32_store(uint8_t *a0,int32_t a1,const uint8_t *a2);
extern void name_rec_insert(uint8_t *,int32_t,uint8_t *,const void *,int32_t);
extern void name_rec_gate(uint8_t *,const void *);
extern int hud_name_fixup(char *);
extern int hud_time_score_draw(uint32_t tbl,uint16_t font,int32_t a2,int32_t clr, uint8_t *rec,int32_t mode);
extern int card_fade_sm(uint32_t a0);
extern int fighter_spawn_tail(uint8_t *rec,uint32_t flag);
extern int battle_round_init(uint32_t a0,uint32_t a1);
extern int fighter_respawn_init(uint32_t);
extern int fighter_reinit_tail(uint32_t);
extern int save_slot_state_fixup(uint32_t a0,uint32_t a1);
extern int round_banner_draw(uint32_t a0,uint32_t a1);
extern int sprite_anim_chain_build(uint32_t a0);
extern int countdown_display_sm(uint32_t a0);
extern int hud_state_machine(uint32_t a0);
extern int clear_f4ee8_u16s(uint32_t);
extern int status_text_display(uint32_t a0,uint32_t a1);
extern int scratchpad_pair_init(void);
extern int menu_res_load_sm(uint32_t a0,uint8_t *buf_a,uint8_t *buf_b,int32_t mode);
extern int fighter_anim_table_init(int32_t a0);
extern int action_enter_fwd(uint32_t battle,uint8_t *ctrl,uint32_t sel);
extern void fighter_frame(byte *,uint8_t *);
#define FUN_801787dc fighter_frame
extern int prim_xform3_task_run();
#define FUN_8018b100 prim_xform3_task_run
extern void pad_slot_state_walk(void);
extern uint32_t spr_attr_alloc_b(uint32_t,uint32_t,uint32_t,uint32_t,int32_t,int32_t);
extern int spr_slot_pkt_submit(uint32_t *ot_cell,uint32_t pos,uint32_t col,uint32_t attr);
extern uint32_t spr_attr_alloc_a(uint32_t,int16_t,int16_t,int16_t,int16_t,uint32_t,int32_t);
extern int cd_file_load_sm(int file_id,uint32_t dst);
extern int printf_forward(const char *fmt, ...);
extern int cd_fileop_reset(int32_t a0);
extern void cd_fileop_forward(uint32_t,uint32_t);
extern int cd_fileop_dispatch(uint32_t a0);
extern int gpu_disp_mask_set(int enable);
extern int gpu_draw_sync(uint32_t n);
extern int gpu_load_image_cmd(uint32_t rect, uint32_t src);
extern int gpu_clear_otag_r(uint32_t addr, uint32_t tag);
extern int gpu_draw_otag(uint32_t otag);
extern int gpu_draw_env_put(int env_g);
extern int draw_env_timing(uint32_t env_g);
extern int sprite_attr_init_a(uint32_t attr_g, uint16_t x, uint16_t y, uint16_t w, int32_t h);
extern int sprite_attr_init_b(uint32_t attr_g, uint16_t x, uint16_t y, uint16_t w, uint16_t aux);
extern int cd_dir_table_set(uint32_t a0,uint32_t a1);
extern int cd_mode_flag_set();
extern int cd_mode_set_0e(uint32_t s0);
extern int cd_read_params_store(uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3,uint32_t s2);
extern int cd_mc_state_machine(void);
extern int cd_chan_boot_fwd(void);
extern int cd_chan_attr_push();
extern int cd_subsys_reset_wrap();
extern int cd_chan_block_boot(void);


extern void display_mode_setup(uint32_t);
extern void frame_advance_helper(void);
extern void status_flag_cond_set(uint32_t,int);
extern int table_byte_next_a(void);
extern void br2_guest_frame_abandon(void);
extern uint32_t g_cdfn_log[24];
extern uint32_t g_bce_calls,g_bce_fz0,g_bce_fzN,g_bce_cls,g_bce_p3;
extern int br2_bail_pending(void);
extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
extern void cd_seek_note(uint32_t,uint32_t,uint32_t);
extern int cd_stamp_to_sector(const void *);


extern void perm_table_apply(uint);
extern void scroll_cfg_set(undefined1,int);
extern void battle_session_arm(intptr_t,int);
extern void frame_tick(intptr_t);
extern void flow_cfg_init(int);
extern void announce_param_block_init_b(uint32_t,uint32_t,const void *,uint16_t,uint16_t,int32_t);
extern int player_light_apply(byte *ctrl,int side);
extern void camera_state_cache(uint unused,int battle);
extern void battle_camera_step(uint cam,byte *f0,byte *f1,int battle);
extern int fighter_trail_emit(undefined4 *mat,undefined4 a1,int ent,int spec);
extern int slot_height_proj(undefined4 a0,int obj,int idx,int xf);
extern int battle_intro_fsm(undefined4 mat,undefined4 a1,undefined4 a2,int st);
extern void rec_timer_pair_clear(intptr_t);
extern void gauge_scale_fixed(short,short);
extern void brightness_commit(undefined2);
extern int deferred_cmd_reset(void);
extern void deferred_cmd_drain();
extern void deferred_cmd_post(uint cmd,int arg,short aux);
extern void memcard_slot_poll(void);
extern void memcard_tick(void);
extern undefined4 scene_entry(int vram_base,int stage_idx,int demo_flag);
extern void battle_frame_tick(int battle,uint phase);
extern int fighter_flash_update(uint battle,uint slot,int ctx2,byte *rec);
extern int fighter_tint_fade(int scene,int idx,int battle,int anim);
extern void battle_hud_draw(int unused,int fighter,int aux0,int aux1);
extern void fighter_contact_resolve(int f0,int f1,int battle);
extern void fighter_pair_update(int f0,int f1,int battle);
extern int prim_list_gte_invoke(int *lst,undefined4 a1,undefined4 a2,int total,undefined4 mtx_t ,undefined4 mtx_ip);
extern int prim_list_persp_step(int *lst,undefined4 a1,undefined4 a2,int n,undefined4 arg5 ,int xf,undefined4 arg7,undefined4 arg8);
extern void dbg_marker_mode_set(int);
extern int sprintf_core(char *,char *,...);
extern uint cd_timer_read(int);

extern void dialog_msg_dispatch(void);
extern void dialog_sel_dispatch_wide(void);

undefined1 frame_divisor_get(void);
extern int card_sprite_load_sm(uint32_t,int);
extern int attr_word_decode(void *);
undefined4 stream_block_decode(undefined4 ctx,int *tab,int idx);
uint pad_state_refresh(intptr_t pad);
undefined2 match_mode_set(int mode);
void cd_stream_mode_init(undefined4 stptr);

/* ---- guest globals ---- */
extern uint8_t  *g_DAT_1f800001;
extern uint8_t  *g_DAT_1f800010;
extern uint8_t  *g_DAT_1f800015;
extern uint8_t  *g_DAT_1f800068;
extern uint8_t  *g_DAT_1f800074;
extern uint8_t  *g_DAT_1f800098;
extern uint8_t  *g_DAT_1f800099;
extern uint8_t  *g_DAT_1f80009a;
extern uint8_t  *g_DAT_1f80009b;
extern uint8_t  *g_DAT_1f80009c;
extern uint8_t  *g_DAT_1f8000a2;
extern uint8_t  *g_DAT_1f80009d;
extern uint8_t  *g_DAT_1f80009e;
extern uint8_t  *g_DAT_1f80009f;
extern uint8_t  *g_DAT_1f8000b0;
extern uint8_t  *g_DAT_1f8000b1;
extern uint8_t  *g_DAT_1f8000e8;
extern uint8_t  *g_DAT_1f8000e9;
extern uint8_t  *g_DAT_1f8000eb;
extern uint8_t  *g_DAT_1f8000ec;
extern uint8_t  *g_DAT_1f8000ee;
extern uint8_t  *g_DAT_1f8001f0;
extern uint8_t  *g_DAT_1f8001f1;
extern uint8_t  *g_DAT_1f8001f3;
extern uint8_t  *g_DAT_1f8001f4;
extern uint8_t  *g_DAT_1f8001f6;
extern uint8_t  *g_DAT_80121f30;
extern uint8_t  *g_DAT_80121fe0;
extern uint8_t  *g_DAT_80122074;
extern uint8_t  *g_DAT_80122075;
extern uint8_t  *g_DAT_80122076;
extern uint8_t  *g_DAT_80122078;
extern uint8_t  *g_DAT_80122079;
extern uint8_t  *g_DAT_8012207c;
extern uint8_t  *g_DAT_8012207d;
extern uint8_t  *g_DAT_801b2253;
extern uint8_t  *g_DAT_801b2350;
extern uint8_t  *g_DAT_801b33dc;
extern uint8_t  *g_DAT_801b3400;
extern uint8_t  *g_DAT_801b346c;
extern uint8_t  *g_DAT_801b35a0;
extern uint8_t  *g_DAT_801b35b8;
extern uint8_t  *g_DAT_801b35cc;
extern uint8_t  *g_DAT_801b3758;
extern uint8_t  *g_DAT_801b377c;
extern uint8_t  *g_DAT_801b37dc;
extern uint8_t  *g_DAT_801b37f4;
extern uint8_t  *g_DAT_801b3814;
extern uint8_t  *g_DAT_801b3834;
extern uint8_t  *g_DAT_801b3854;
extern uint8_t  *g_DAT_801b3868;
extern uint8_t  *g_DAT_801b388c;
extern uint8_t  *g_DAT_801b38ac;
extern uint8_t  *g_DAT_801b38d0;
extern uint8_t  *g_DAT_801b3968;
extern uint8_t  *g_DAT_801b3a00;
extern uint8_t  *g_DAT_801b3a84;
extern uint8_t  *g_DAT_801b3b08;
extern uint8_t  *g_DAT_801b3b94;
extern uint8_t  *g_DAT_801b3c24;
extern uint8_t  *g_DAT_801b3ca8;
extern uint8_t  *g_DAT_801b3d34;
extern uint8_t  *g_DAT_801b3db4;
extern uint8_t  *g_DAT_801b3e44;
extern uint8_t  *g_DAT_801b3eb0;
extern uint8_t  *g_DAT_801b3ebc;
extern uint8_t  *g_DAT_801b3f64;
extern uint8_t  *g_DAT_801befa8;
extern uint8_t  *g_DAT_801befca;
extern uint8_t  *g_DAT_801c1264;
extern uint8_t  *g_DAT_801c1304;
extern uint8_t  *g_DAT_801c4580;
extern uint8_t  *g_DAT_801e3d00;
extern uint8_t  *g_DAT_801f24d0;
extern uint8_t  *g_DAT_801f24d8;
extern uint8_t  *g_PTR_DAT_801b4254;
extern uint8_t  *g__DAT_1f800004;
extern uint8_t  *g__DAT_1f800008;
extern uint8_t  *g__DAT_1f80000c;
extern uint8_t  *g__DAT_1f800020;
extern uint8_t  *g__DAT_1f800022;
extern uint8_t  *g__DAT_1f80007c;
extern uint8_t  *g__DAT_1f800080;
extern uint8_t  *g__DAT_1f800082;
extern uint8_t  *g__DAT_1f800088;
extern uint8_t  *g__DAT_1f80008c;
extern uint8_t  *g__DAT_1f80008e;
extern uint8_t  *g__DAT_1f800090;
extern uint8_t  *g__DAT_1f8000a4;
extern uint8_t  *g__DAT_1f8000b4;
extern uint8_t  *g__DAT_1f8000b6;
extern uint8_t  *g__DAT_1f8000bc;
extern uint8_t  *g__DAT_1f8000c4;
extern uint8_t  *g__DAT_1f8000dc;
extern uint8_t  *g__DAT_1f800100;
extern uint8_t  *g__DAT_1f800104;
extern uint8_t  *g__DAT_1f800110;
extern uint8_t  *g__DAT_1f800148;
extern uint8_t  *g__DAT_1f800154;
extern uint8_t  *g__DAT_1f800208;
extern uint8_t  *g__DAT_1f80020c;
extern uint8_t  *g__DAT_1f800218;
extern uint8_t  *g__DAT_1f800250;
extern uint8_t  *g__DAT_1f80025c;
extern uint8_t  *g__DAT_801c15f0;
extern uint8_t  *g__DAT_801c43a0;
extern uint8_t  *g__DAT_801c4578;
extern uint8_t  *g__DAT_801c47b8;
extern uint8_t  *g__DAT_801e3cfc;
extern uint8_t  *g__DAT_801e8508;
extern uint8_t  *g__DAT_801e850c;
extern uint8_t  *g__DAT_801e8518;
extern uint8_t  *g__DAT_801e851c;
extern uint8_t  *g__DAT_801f2650;
extern uint8_t  *g__DAT_801f2654;
extern uint8_t  *g__DAT_801f2658;
extern uint8_t  *g__DAT_801f265a;
extern uint8_t  *g__DAT_801f265c;
extern uint8_t  *g__DAT_801f265e;
extern uint8_t  *g__DAT_801f2664;
extern uint8_t  *g__DAT_801f2674;
extern uint8_t  *g__DAT_801f2678;
extern uint8_t  *g__DAT_801f267c;
extern uint8_t  *g__DAT_801f36c2;
extern uint8_t  *g__DAT_801f4714;
extern uint8_t  *g__DAT_801f4728;
extern uint8_t  *g__DAT_801f472c;
extern uint8_t  *g__DAT_801f4730;
extern uint8_t  *g__DAT_801f4732;
extern uint8_t  *g__DAT_801f4734;
extern uint8_t  *g__DAT_801f4738;
extern uint8_t  *g__DAT_801f473c;
extern uint8_t  *g__DAT_801f4744;
extern uint8_t  *g__DAT_801f4748;
extern uint8_t  *g__DAT_801f474c;
extern uint8_t  *g__DAT_801f4750;
extern uint8_t  *g__DAT_801f4754;
extern uint8_t  *g__DAT_801f4cb2;
extern uint8_t  *g__DAT_801f4cb6;
extern uint8_t  *g__DAT_801fc68c;
extern char *s_NAME_ENTRY_80122080;
extern char *s_RANKING_80122048;
extern char *s_SURVIVAL_8012205c;
extern char *s_TIME_ATTACK_80122050;
extern char *s_VS_RANKING_80122068;
extern char *s_time_out_in_decoding___80120e90;

/* ---- macros ---- */
#define DAT_1f800001   (*(uint8_t *)g_DAT_1f800001)
#define DAT_1f800010   (*(uint8_t *)g_DAT_1f800010)
#define DAT_1f800015   (*(uint8_t *)g_DAT_1f800015)
#define DAT_1f800068   (*(uint8_t *)g_DAT_1f800068)
#define DAT_1f800074   (*(uint8_t *)g_DAT_1f800074)
#define DAT_1f800098   (*(uint8_t *)g_DAT_1f800098)
#define DAT_1f800099   (*(uint8_t *)g_DAT_1f800099)
#define DAT_1f80009a   (*(uint8_t *)g_DAT_1f80009a)
#define DAT_1f80009b   (*(uint8_t *)g_DAT_1f80009b)
#define DAT_1f80009c   (*(uint8_t *)g_DAT_1f80009c)
#define DAT_1f80009d   (*(uint8_t *)g_DAT_1f80009d)
#define DAT_1f80009e   (*(uint8_t *)g_DAT_1f80009e)
#define DAT_1f80009f   (*(uint8_t *)g_DAT_1f80009f)
#define DAT_1f8000b0   (*(uint8_t *)g_DAT_1f8000b0)
#define DAT_1f8000b1   (*(uint8_t *)g_DAT_1f8000b1)
#define DAT_1f8000e8   (*(uint8_t *)g_DAT_1f8000e8)
#define DAT_1f8000e9   (*(uint8_t *)g_DAT_1f8000e9)
#define DAT_1f8000eb   (*(uint8_t *)g_DAT_1f8000eb)
#define DAT_1f8000ec   (*(uint8_t *)g_DAT_1f8000ec)
#define DAT_1f8000ee   (*(uint8_t *)g_DAT_1f8000ee)
#define DAT_1f8001f0   (*(uint8_t *)g_DAT_1f8001f0)
#define DAT_1f8001f1   (*(uint8_t *)g_DAT_1f8001f1)
#define DAT_1f8001f3   (*(uint8_t *)g_DAT_1f8001f3)
#define DAT_1f8001f4   (*(uint8_t *)g_DAT_1f8001f4)
#define DAT_1f8001f6   (*(uint8_t *)g_DAT_1f8001f6)
#define DAT_80121f30   (*(uint32_t *)g_DAT_80121f30)
#define DAT_80121fe0   (*(uint32_t *)g_DAT_80121fe0)
#define DAT_80122074   (*(uint8_t *)g_DAT_80122074)
#define DAT_80122075   (*(uint8_t *)g_DAT_80122075)
#define DAT_80122076   (*(uint8_t *)g_DAT_80122076)
#define DAT_80122078   (*(uint8_t *)g_DAT_80122078)
#define DAT_80122079   (*(uint8_t *)g_DAT_80122079)
#define DAT_8012207c   (*(uint8_t *)g_DAT_8012207c)
#define DAT_8012207d   (*(uint8_t *)g_DAT_8012207d)
#define DAT_801b2253   (*(uint8_t *)g_DAT_801b2253)
#define DAT_801b2350   (*(uint32_t *)g_DAT_801b2350)
#define DAT_801b33dc   (*(uint32_t *)g_DAT_801b33dc)
#define DAT_801b3400   (*(uint32_t *)g_DAT_801b3400)
#define DAT_801b346c   (*(uint32_t *)g_DAT_801b346c)
#define DAT_801b35a0   (*(uint32_t *)g_DAT_801b35a0)
#define DAT_801b35b8   (*(uint32_t *)g_DAT_801b35b8)
#define DAT_801b35cc   (*(uint32_t *)g_DAT_801b35cc)
#define DAT_801b3758   (*(uint32_t *)g_DAT_801b3758)
#define DAT_801b377c   (*(uint32_t *)g_DAT_801b377c)
#define DAT_801b37dc   (*(uint32_t *)g_DAT_801b37dc)
#define DAT_801b37f4   (*(uint32_t *)g_DAT_801b37f4)
#define DAT_801b3814   (*(uint32_t *)g_DAT_801b3814)
#define DAT_801b3834   (*(uint32_t *)g_DAT_801b3834)
#define DAT_801b3854   (*(uint32_t *)g_DAT_801b3854)
#define DAT_801b3868   (*(uint32_t *)g_DAT_801b3868)
#define DAT_801b388c   (*(uint32_t *)g_DAT_801b388c)
#define DAT_801b38ac   (*(uint32_t *)g_DAT_801b38ac)
#define DAT_801b38d0   (*(uint32_t *)g_DAT_801b38d0)
#define DAT_801b3968   (*(uint32_t *)g_DAT_801b3968)
#define DAT_801b3a00   (*(uint32_t *)g_DAT_801b3a00)
#define DAT_801b3a84   (*(uint32_t *)g_DAT_801b3a84)
#define DAT_801b3b08   (*(uint32_t *)g_DAT_801b3b08)
#define DAT_801b3b94   (*(uint32_t *)g_DAT_801b3b94)
#define DAT_801b3c24   (*(uint32_t *)g_DAT_801b3c24)
#define DAT_801b3ca8   (*(uint32_t *)g_DAT_801b3ca8)
#define DAT_801b3d34   (*(uint32_t *)g_DAT_801b3d34)
#define DAT_801b3db4   (*(uint32_t *)g_DAT_801b3db4)
#define DAT_801b3e44   (*(uint32_t *)g_DAT_801b3e44)
#define DAT_801b3eb0   (*(uint32_t *)g_DAT_801b3eb0)
#define DAT_801b3ebc   (*(uint32_t *)g_DAT_801b3ebc)
#define DAT_801b3f64   (*(uint32_t *)g_DAT_801b3f64)
#define DAT_801befa8   (*(uint8_t *)g_DAT_801befa8)
#define DAT_801befca   (*(uint8_t *)g_DAT_801befca)
#define DAT_801c1264   (*(uint32_t *)g_DAT_801c1264)
#define DAT_801c1304   (*(uint32_t *)g_DAT_801c1304)
#define DAT_801c4580   (*(uint8_t *)g_DAT_801c4580)
#define DAT_801e3d00   (*(uint8_t *)g_DAT_801e3d00)
#define DAT_801f24d0   (*(uint32_t *)g_DAT_801f24d0)
#define DAT_801f24d8   (*(uint32_t *)g_DAT_801f24d8)
#define PTR_DAT_801b4254   (*(uint32_t *)g_PTR_DAT_801b4254)
#define _DAT_1f800004   (*(uint32_t *)g__DAT_1f800004)
#define _DAT_1f800008   (*(uint32_t *)g__DAT_1f800008)
#define _DAT_1f80000c   (*(uint32_t *)g__DAT_1f80000c) /* guest lw/sw @0x1f80000c (e.g. 0x8014f260,0x80164ea4,0x801594a4) */
#define _DAT_1f800020   (*(uint16_t *)g__DAT_1f800020) /* guest lhu @0x1f800020 (0x8014f260 et al) */
#define _DAT_1f800022   (*(uint16_t *)g__DAT_1f800022) /* guest lhu @0x1f800022 */
#define _DAT_1f80007c   (*(uint16_t *)g__DAT_1f80007c) /* guest lhu @0x1f80007c (0x80164ea4:0x801657d4 lhu) */
#define _DAT_1f800080   (*(uint16_t *)g__DAT_1f800080) /* guest lhu @0x1f800080 (0x8015c754) */
#define _DAT_1f800082   (*(uint16_t *)g__DAT_1f800082) /* guest lhu @0x1f800082 (0x8016fcc4) */
#define _DAT_1f800088   (*(uint16_t *)g__DAT_1f800088) /* guest sh @0x1f800088 (0x8016fcc4) */
#define _DAT_1f80008c   (*(int16_t *)guest_at(0x1f80008c)) /* guest sh/lh @0x1f80008c (-1 idle state, cf. game.c int16 deref) */
#define _DAT_1f80008e   (*(uint16_t *)guest_at(0x1f80008e)) /* guest lhu/sh @0x1f80008e */
#define _DAT_1f800090   (*(uint16_t *)guest_at(0x1f800090)) /* guest lhu/sh @0x1f800090 */
#define _DAT_1f8000a4   (*(uint16_t *)g__DAT_1f8000a4) /* guest lhu @0x1f8000a4 (0x80167840) */
#define _DAT_1f8000b4   (*(uint16_t *)g__DAT_1f8000b4)
#define _DAT_1f8000b6   (*(uint16_t *)g__DAT_1f8000b6)
#define _DAT_1f8000bc   (*(uint32_t *)g__DAT_1f8000bc)
#define _DAT_1f8000c4   (*(uint32_t *)g__DAT_1f8000c4)
#define _DAT_1f8000dc   (*(uint32_t *)g__DAT_1f8000dc) /* guest lw/sw @0x1f8000dc (0x801594a4,0x80164ea4,0x8014f260) */
#define _DAT_1f800100   (*(uint32_t *)g__DAT_1f800100)
#define _DAT_1f800104   (*(uint32_t *)g__DAT_1f800104)
#define _DAT_1f800110   (*(uint32_t *)g__DAT_1f800110)
#define _DAT_1f800148   (*(uint32_t *)g__DAT_1f800148)
#define _DAT_1f800154   (*(uint32_t *)g__DAT_1f800154)
#define _DAT_1f800208   (*(uint32_t *)g__DAT_1f800208)
#define _DAT_1f80020c   (*(uint32_t *)g__DAT_1f80020c)
#define _DAT_1f800218   (*(uint32_t *)g__DAT_1f800218)
#define _DAT_1f800250   (*(uint32_t *)g__DAT_1f800250)
#define _DAT_1f80025c   (*(uint32_t *)g__DAT_1f80025c)
#define _DAT_801c15f0   (*(uint32_t *)g__DAT_801c15f0) /* guest sw @0x801c15f0 (0x80167840,0x8016fcc4) */
#define _DAT_801c43a0   (*(uint32_t *)g__DAT_801c43a0) /* guest sw @0x801c43a0 (0x80146e38) */
#define _DAT_801c4578   (*(uint32_t *)g__DAT_801c4578) /* guest lw @0x801c4578 (0x80165cb4) */
#define _DAT_801c47b8   (*(uint32_t *)g__DAT_801c47b8)
#define _DAT_801e3cfc   (*(uint32_t *)g__DAT_801e3cfc)
#define _DAT_801e8508   (*(uint32_t *)g__DAT_801e8508) /* guest sw @0x801e8508 (0x80164ea4,0x8015c754,0x8015b9a0,0x8014f260) */
#define _DAT_801e850c   (*(uint32_t *)g__DAT_801e850c)
#define _DAT_801e8518   (*(uint32_t *)g__DAT_801e8518) /* guest sw @0x801e8518 */
#define _DAT_801e851c   (*(uint32_t *)g__DAT_801e851c) /* guest sw @0x801e851c (0x80164ea4) */
#define _DAT_801f2650   (*(uint32_t *)g__DAT_801f2650)
#define _DAT_801f2654   (*(uint32_t *)g__DAT_801f2654)
#define _DAT_801f2658   (*(uint16_t *)g__DAT_801f2658) /* guest sh @0x801f2658 (0x80155f64) */
#define _DAT_801f265a   (*(uint16_t *)g__DAT_801f265a) /* guest sh @0x801f265a */
#define _DAT_801f265c   (*(uint16_t *)g__DAT_801f265c) /* guest sh @0x801f265c */
#define _DAT_801f265e   (*(uint16_t *)g__DAT_801f265e) /* guest sh @0x801f265e */
#define _DAT_801f2664   (*(uint16_t *)g__DAT_801f2664) /* guest sh @0x801f2664 */
#define _DAT_801f2674   (*(uint32_t *)g__DAT_801f2674) /* guest sw @0x801f2674 (0x80155f64) */
#define _DAT_801f2678   (*(uint32_t *)g__DAT_801f2678) /* guest sw @0x801f2678 */
#define _DAT_801f267c   (*(uint32_t *)g__DAT_801f267c) /* guest sw @0x801f267c */
#define _DAT_801f36c2   (*(uint16_t *)g__DAT_801f36c2)
#define _DAT_801f4714   (*(uint32_t *)g__DAT_801f4714)
#define _DAT_801f4728   (*(uint32_t *)g__DAT_801f4728)
#define _DAT_801f472c   (*(uint16_t *)g__DAT_801f472c) /* guest lh/lhu/sh @0x801f472c (0x80159864) */
#define _DAT_801f4730   (*(uint16_t *)g__DAT_801f4730) /* guest lh/lhu @0x801f4730 (0x80159864) */
#define _DAT_801f4732   (*(uint16_t *)g__DAT_801f4732)
#define _DAT_801f4734   (*(uint8_t *)g__DAT_801f4734)
#define _DAT_801f4738   (*(uint32_t *)g__DAT_801f4738) /* guest lw @0x801f4738 (0x80159864) */
#define _DAT_801f473c   (*(uint32_t *)g__DAT_801f473c) /* guest lw/sw @0x801f473c (0x80159864) */
#define _DAT_801f4744   (*(uint32_t *)g__DAT_801f4744)
#define _DAT_801f4748   (*(uint32_t *)g__DAT_801f4748)
#define _DAT_801f474c   (*(uint32_t *)g__DAT_801f474c)
#define _DAT_801f4750   (*(uint32_t *)g__DAT_801f4750)
#define _DAT_801f4754   (*(uint32_t *)g__DAT_801f4754)
#define _DAT_801f4cb2   (*(uint8_t *)g__DAT_801f4cb2)
#define _DAT_801f4cb6   (*(uint32_t *)g__DAT_801f4cb6)
#define _DAT_801fc68c   (*(uint32_t *)g__DAT_801fc68c) /* guest lw/sw @0x801fc68c (0x80159864) */

/* ======== 0x80167840 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void unlock_progress_scene_entry(void)

{
  uint char_idx;

  char_idx = 0xffffffff;
  if ((_DAT_1f8000b4 & 0xf00) == 0xa00) {
    char_idx = (uint)DAT_1f8000eb;
  }
  if ((_DAT_1f8000b6 & 0xf00) == 0xa00) {
    char_idx = (uint)DAT_1f8001f3;
  }
  switch(DAT_1f80009c) {
  case 0:
    card_fade_sm(char_idx);
    *(uint *)(_g_system_config_ptr + 0x60) =
         *(uint *)(_g_system_config_ptr + 0x60) | 1 << (char_idx + 0x10 & 0x1f);
    *(uint *)(_g_system_config_ptr + 0x50) =
         *(uint *)(_g_system_config_ptr + 0x50) | 1 << (char_idx & 0x1f);
    if ((int8_t)_DAT_1f8000a4 == -2) {
      *(uint *)(_g_system_config_ptr + 0x4c) = *(uint *)(_g_system_config_ptr + 0x4c) | 0x200;
      char_idx = *(uint *)(_g_system_config_ptr + 0x4c) | 0x400;
    }
    else {
      char_idx = *(uint *)(_g_system_config_ptr + 0x4c) | 0x200;
    }
    *(uint *)(_g_system_config_ptr + 0x4c) = char_idx;
    break;
  default:
    goto L_done;
  case 4:
    *(uint *)(_g_system_config_ptr + 0x60) =
         *(uint *)(_g_system_config_ptr + 0x60) | 1 << (char_idx & 0x1f);
    if (((*(uint *)(_g_system_config_ptr + 0x60) & 0x7ff) == 0x7ff) &&
       ((*(uint *)(_g_system_config_ptr + 0x44) >> 0xe & 1) == 0)) {
      *(uint *)(_g_system_config_ptr + 0x44) = *(uint *)(_g_system_config_ptr + 0x44) | 0x4000;
      _DAT_801c15f0 = 1;
    }
  }
  if ((int8_t)_DAT_1f8000a4 == -2) {
    scene_entry(0x80010000,2,*(uint *)(_g_system_config_ptr + 0x28) >> 2 & 1);
    char_idx = *(uint *)(_g_system_config_ptr + 0x28) | 4;
  }
  else {
    scene_entry(0x80010000,3,*(uint *)(_g_system_config_ptr + 0x28) >> 3 & 1);
    char_idx = *(uint *)(_g_system_config_ptr + 0x28) | 8;
  }
  *(uint *)(_g_system_config_ptr + 0x28) = char_idx;
  _DAT_801c15f0 = 1;
L_done:
  return;
}

/* ======== 0x8016fcc4 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void battle_flow_state_machine(intptr_t cmd_rec)

{
  int ot_base;
  bool battle_ran;
  undefined4 scr, pidx, gaddr;
  int res, otidx;
  int qres, otdp, sbyte;

  battle_ran = false;
  {
    if (DAT_1f800001 != 0 && DAT_1f800001 != 1)
      decomp_trace_record(60,(uint32_t)DAT_1f800001,(uint32_t)DAT_1f80009c,0,0);
    decomp_trace_record(70,(uint32_t)DAT_1f800001,(uint32_t)DAT_1f80009c,DAT_1f80009b,0); }
joined_r0x8016fd30:
  /* Bail-unwind in flight (g_psx_call_bail): this is a clean-C mega-loop
   * (goto joined_r0x8016fd30 every iteration) with no dispatch boundary —
   * return so the unwind reaches the psx_dispatch_impl frame above, exactly
   * like a generated CPS frame's `if (g_psx_call_bail) return;`.
   *
   * DAT1 exit invariant: on real HW this function's ONLY return path is the
   * DAT1==-1 branch below, which writes DAT_1f800001 = 0 before returning.
   * A bail-forced return bypasses that write and would leak a BFSM-private
   * DAT1 substate (e.g. battle_frame's parked 3) into the caller — the
   * generated top-level SM then transitions gs 8->5 and match_loop, whose
   * DAT1 machine knows only {0,1,2,0xf0,0xf1,0xff}, dead-loops forever on the
   * foreign value (proof/gs5_matchloop_dat1_3_dead_state.md). Restore the
   * canonical post-BFSM state so the gs->5 handoff sees exactly what the
   * guest guarantees. */
  {
    if (br2_bail_pending()) { DAT_1f800001 = 0; return; } }
  /* Guest block_8016FD38 (the loop head) reloads the OT base/index from
   * scratchpad EVERY iteration: lw $2C,0xC($16) / lw $2B,8($16) where $16 =
   * 0x1F800000. frame_tick's double-buffer toggle writes those cells at the
   * end of each pass, and the next emit must observe the NEW buffer. The
   * previous port cached them once at entry and wrote the stale values back,
   * pinning _DAT_1f800008 to the entry buffer forever: every emit appended to
   * the same OT cells with no intervening ch6 clear, so the second pass linked
   * each packet chain's tail to the previous head and the GPU walked a closed
   * cycle. Read them fresh here; the guest stores nothing back. */
  otidx = _DAT_1f80000c;
  ot_base = _DAT_1f800008;
  if ((int8_t)DAT_1f800001 == -1) {
    if (g_game_state != '\t') {
      rec_timer_pair_clear(0x1f800000);
    }
    DAT_1f800001 = 0;
    return;
  }
  switch(DAT_1f800001) {
  case '\0':
    DAT_1f800001 = '\x01';
  case '\x01':
    hud_sprite_desc_init();
    res = frame_divisor_get();
    if (res == 0) {
      display_mode_setup(*(undefined1 *)(_g_system_config_ptr + 0xb));
      frame_advance_helper();
      match_mode_set(1);
      rgb_triple_store(0,0,0);
      DAT_1f800010 = 0;
      DAT_1f80009e = '\0';
      DAT_1f800001 = '\0';
      confirm_bookkeeping(0x1f800000);
      select_phase_update(0x1f800000);
      battle_ran = false;
      switch(DAT_1f80009c) {
      case 0:
        _DAT_1f8000c4 = guest_of(_g_system_config_ptr + 0x2c);
        if ((DAT_1f80009b == '\0') && (*(char *)(_g_system_config_ptr + 0x11) != '\x01')) {
          res = battle_frame();
          /* battle_frame returns early on a bail unwind — propagate it so
           * the unwind reaches the dispatch boundary instead of continuing
           * this clean-C loop with every generated fallback dropped.
           * Same DAT1 exit invariant as the loop head: a bail-forced return
           * must not leak BFSM-private DAT1 into match_loop (see above). */
          {
            if (br2_bail_pending()) { DAT_1f800001 = 0; return; } }
          if (res < 0) {
            DAT_1f800001 = -1;
          }
          battle_ran = true;
        }
        else {
          res = select_screen_tick(0);
          if (res != 0) {
            DAT_1f800001 = -1;
          }
        }
        break;
      case 1:
      case 2:
        _DAT_1f8000c4 = guest_of(_g_system_config_ptr + 0x2c);
        res = select_screen_tick(1);
        {
          decomp_trace_record(74,0xf00d,(uint32_t)res,DAT_1f800001,(uint32_t)DAT_1f80009c); }
        if (res != 0) {
          DAT_1f800001 = -1;
        }
        break;
      case 3:
        _DAT_1f8000c4 = guest_of(_g_system_config_ptr + 0x2c);
        res = select_screen_tick(3);
        if (res != 0) {
          DAT_1f800001 = -1;
        }
        break;
      case 4:
        _DAT_1f8000c4 = guest_of(_g_system_config_ptr + 0x2c);
        deferred_cmd_post(0x403,0,0);
        frame_tick(0x1f800000);
        scene_entry(0x80010000,1,*(uint *)(_g_system_config_ptr + 0x28) >> 1 & 1);
        if ((*(uint *)(_g_system_config_ptr + 0x28) >> 1 & 1) == 0) {
          *(uint *)(_g_system_config_ptr + 0x28) = *(uint *)(_g_system_config_ptr + 0x28) | 2;
          _DAT_801c15f0 = 1;
        }
        res = select_screen_tick(2);
        if (res == 0) {
          countdown_display_sm(0);
          hud_state_machine(0);
        }
        else {
          DAT_1f800001 = -1;
        }
        break;
      case 5:
        _DAT_1f8000c4 = guest_of(_g_system_config_ptr + 0x38);
        scr = 1;
        if (DAT_1f80009b == '\0') {
          scr = 3;
        }
        res = select_screen_tick(scr);
        if (res != 0) {
          g_game_state = '\v';
          g_sub_state = 4;
          DAT_1f800001 = -1;
        }
      }
      if (DAT_1f800001 == '\0') {
LAB_8016ffd8:
        DAT_1f800001 = '\x02';
L_dat1_2:
        deferred_cmd_post(0x204,0,0);
        hud_sprite_desc_init();
        rgb_triple_store(0,0,0);
        print_mode_dispatch_b(2,0,0);
        match_setup_apply(0x1f8000e8u,0x1f8001f0u,0x1f800000u,guest_of(_g_system_config_ptr));
        battle_state_init(&DAT_1f8000e8,&DAT_1f8001f0,0x1f800000);
        scratchpad_pair_init();
        DAT_1f80009e = '\0';
        DAT_1f800001 = '\x03';
        {
          decomp_trace_record(73,0xcafe,(uint32_t)DAT_1f800001,DAT_1f80009b,(uint32_t)DAT_1f80009c); }
L_dat1_3:
        res = frame_divisor_get();
        if (res == 0) {
          if (battle_ran) {
            battle_ran = false;
          }
          else {
            battle_init(0x1f800000,&DAT_1f8000e8,&DAT_1f8001f0);
          }
          DAT_1f80009e = '\x03';
          _DAT_1f800088 = 0;
          _DAT_1f80008c = 0;
          _DAT_1f80008e = 0;
          _DAT_1f800090 = 0;
          DAT_1f800001 = '\x04';
        }
      }
      else if (g_game_state == '\r') {
        DAT_1f800001 = -1;
      }
      else if (DAT_1f800001 == '\x01') goto LAB_8016ffd8;
    }
    break;
  case '\x02':
    goto L_dat1_2;
  case '\x03':
    goto L_dat1_3;
  case '\x04':
    {
      decomp_trace_record(71,0xdead,(uint32_t)DAT_1f800001,DAT_1f80009b,(uint32_t)DAT_1f80009c); }
    if (_DAT_1f800090 != 0) {
      _DAT_1f800090 = 0;
    }
    qres = menu_res_load_sm(0x1f800000,&DAT_1f8000e8,&DAT_1f8001f0,0);
    /* menu_res_load_sm returns -1 both for "already done" (s1+0x8c<0
     * stored marker) and for a br2_bail_pending unwind. Only the bail case
     * must NOT advance the phase — the load is mid-flight and the fighter
     * model (ctrl+0x18/+0x1c/+0x20/+0x30) is still unbound. Propagate the
     * bail so the unwind reaches the contract frame and the next frame
     * resumes the load instead of entering battle on a NULL node.
     * DAT1 exit invariant (see loop head): a bail-forced return must not
     * leak BFSM-private DAT1 into match_loop — restore canonical 0. */
    {
      if (br2_bail_pending()) { DAT_1f800001 = 0; return; } }
    if ((qres != 0) && (_DAT_1f800090 == 0)) goto L_dat1_5;
    break;
  case '\x05':
L_dat1_5:
    se_value_select((char)DAT_1f800098);
    confirm_code_select(0x1f800000);
    battle_struct_init(_DAT_1f8000bc,0x1f800000);
    task_table_init(0);
    sprite_desc_table_init(0);
    print_mode_dispatch_b(2,0,0);
    battle_session_arm(0x1f800000,0xffffffff);
    DAT_1f800001 = '\x06';
  case '\x06':
    match_setup_apply(0x1f8000e8u,0x1f8001f0u,0x1f800000u,guest_of(_g_system_config_ptr));
    battle_state_init(&DAT_1f8000e8,&DAT_1f8001f0,0x1f800000);
    pidx = table_byte_next_a();
    perm_table_apply(pidx);
    reset_sprite_ot_block(0x1f800000);
    task_table_init(1);
    flow_cfg_init(1);
    brightness_packet_send(0x7f);
    record_copy_1a((char)DAT_1f800098);
    per_side_setup((char)DAT_1f800098,DAT_1f800099);
    rec_copy_init(DAT_1f800098);
    clear_f4ee8_u16s(0x1f8000e8);
    clear_f4ee8_u16s(0x1f8001f0);
    reset_combat_slots();
    action_enter_fwd(0x1f800000,&DAT_1f8000e8,DAT_1f80009a);
    action_enter_fwd(0x1f800000,&DAT_1f8001f0,DAT_1f80009a);
    fighter_anim_table_init(DAT_1f800099);
    sprite_anim_chain_build(0x1f800000);
    clear_f0300_records();
    frame_tick(0x1f800000);
    DAT_1f800001 = '\a';
L_dat1_7:
    qres = frame_divisor_get();
    if (qres == 0) {
      DAT_1f800001 = '\b';
L_dat1_8:
      {
        decomp_trace_record(72,0xbeef,(uint32_t)DAT_1f800001,DAT_1f80009b,(uint32_t)DAT_1f80009c); }
      if (DAT_1f80009b == '\x04') {
        battle_sides_commit(&DAT_1f8000e8,&DAT_1f8001f0,0x1f800000);
      }
      else {
        post_battle_dispatch(&DAT_1f8000e8,&DAT_1f8001f0,0x1f800000);
      }
      if ((DAT_1f800001 != '\x06') && (DAT_1f800001 != '\b')) {
        frame_present(0x1f800000);
        otidx = _DAT_1f80000c;
        ot_base = _DAT_1f800008;
        goto joined_r0x8016fd30;
      }
      if ((*(uint *)((uint32_t *)guest_at(_DAT_1f8000c4) + 8) >> 0xb & 1) != 0) {
        DAT_1f80009e = '\x01';
      }
      if (_DAT_1f800082 == 0) {
        record_state_stepper(0x1f800000);
        fighter_contact_resolve((int)guest_of(&DAT_1f8000e8),(int)guest_of(&DAT_1f8001f0),0x1f800000);
        FUN_801787dc((byte *)&DAT_1f8000e8,(uint8_t *)guest_at(0x1f800000));
        FUN_801787dc((byte *)&DAT_1f8001f0,(uint8_t *)guest_at(0x1f800000));
        fighter_pair_update((int)guest_of(&DAT_1f8000e8),(int)guest_of(&DAT_1f8001f0),0x1f800000);
      }
      if (((*(ushort *)(cmd_rec + 2) >> 2 & 1) != 0) && (DAT_1f8000b0 == '\0')) {
        otdp = ot_base + 0x10;
        if (DAT_1f80009d == '\0') {
          otdp = ot_base + 0x2038;
        }
        battle_hud_draw(otdp,0x1f800000,(int)guest_of(&DAT_1f8000e8),(int)guest_of(&DAT_1f8001f0));
      }
      battle_camera_step(_DAT_1f8000bc,(byte *)&DAT_1f8000e8,(byte *)&DAT_1f8001f0,0x1f800000);
      camera_state_cache(_DAT_1f8000bc,0x1f800000);
      if (((*(ushort *)(cmd_rec + 2) & 1) != 0) && ((_DAT_1f800148 & 2) == 0)) {
        fighter_flash_update(ot_base,otidx,0x1f800000,(byte *)&DAT_1f8000e8);
        if ((_DAT_1f800154 & 0x400) == 0) {
          if ((_DAT_1f800154 & 0x200) == 0) {
            if (((_DAT_1f800154 & 0x8000000) == 0) || (otidx == 0)) {
              player_light_apply((byte *)&DAT_1f8000e8,1);
              sbyte = (int8_t)DAT_1f8000ee;
              gaddr = _DAT_1f800100;
            }
            else {
              player_light_apply((byte *)&DAT_1f8000e8,1);
              /* Guest 0x80170430/0x8017043C: lb 0x24($v0) / lw 0x34($v0) —
               * BYTE offsets into the 0x24-byte render-record table at
               * _DAT_1f800110 (record[1]: count byte +0x24, prim hdr +0x34).
               * The decompiler's +0x24/+0x34 were element indices on a
               * uint32_t*, i.e. byte offsets 4x too far. */
              sbyte = (int8_t)*(uint8_t *)guest_at(_DAT_1f800110 + 0x24);
              gaddr = *(undefined4 *)guest_at(_DAT_1f800110 + 0x34);
            }
          }
          else {
            player_light_apply((byte *)&DAT_1f8000e8,1);
            /* Guest 0x80170414/0x80170404: lw 0x10($v0) / lb 0($v0) —
             * cmd_rec = record[0] prim hdr (byte +0x10), param_4 = signed
             * count byte at struct +0. */
            prim_list_gte_invoke((int *)guest_at(*(uint32_t *)guest_at(_DAT_1f800110 + 0x10)),ot_base + 0x40,otidx,
                         (int8_t)*(uint8_t *)guest_at(_DAT_1f800110),0x1f800028,0x1f80033c);
            if (otidx == 0) goto LAB_801704bc;
            sbyte = (int8_t)*(uint8_t *)guest_at(_DAT_1f800110 + 0x24);
            gaddr = *(undefined4 *)guest_at(_DAT_1f800110 + 0x34);
          }
          prim_list_gte_invoke((int *)guest_at(gaddr),ot_base + 0x40,otidx,sbyte,0x1f800028,0x1f80033c);
        }
LAB_801704bc:
        if (((*(ushort *)(cmd_rec + 2) >> 6 & 1) != 0) && (DAT_1f80009f == '\0')) {
          if (DAT_1f800098 == 4) {
LAB_80170524:
            FUN_8018b100(_DAT_1f800104,ot_base + 0x2040,otidx,0xf,0x1f800028,0x1f80035c);
          }
          else {
            if ((char)DAT_1f800098 < '\x05') {
              if (DAT_1f800098 == 1) goto LAB_80170524;
            }
            else if (DAT_1f800098 == 6) goto LAB_80170524;
            prim_list_persp_step((int *)guest_at(_DAT_1f800104),ot_base + 0x2040,otidx,0xf,0x1f800028,0x1f80035c,
                         ((int)((uint)DAT_1f800098 << 0x18) >> 0x15) + -0x7fe4e94c,ot_base + 0x206c);
          }
        }
      }
      if (((*(ushort *)(cmd_rec + 2) >> 1 & 1) != 0) && ((_DAT_1f800250 & 2) == 0)) {
        fighter_flash_update(ot_base,otidx,0x1f800000,(byte *)&DAT_1f8001f0);
        if ((_DAT_1f80025c & 0x400) == 0) {
          if ((_DAT_1f80025c & 0x200) == 0) {
            if (((_DAT_1f80025c & 0x8000000) == 0) || (otidx == 0)) {
              player_light_apply((byte *)&DAT_1f8001f0,2);
              sbyte = (int8_t)DAT_1f8001f6;
              gaddr = _DAT_1f800208;
            }
            else {
              player_light_apply((byte *)&DAT_1f8001f0,2);
              sbyte = (int8_t)*(uint8_t *)guest_at(_DAT_1f800218 + 0x24);
              gaddr = *(undefined4 *)guest_at(_DAT_1f800218 + 0x34);
            }
          }
          else {
            player_light_apply((byte *)&DAT_1f8001f0,2);
            prim_list_gte_invoke((int *)guest_at(*(uint32_t *)guest_at(_DAT_1f800218 + 0x10)),ot_base + 0x40,otidx,
                         (int8_t)*(uint8_t *)guest_at(_DAT_1f800218),0x1f800028,0x1f80033c);
            if (otidx == 0) goto LAB_801706ac;
            sbyte = (int8_t)*(uint8_t *)guest_at(_DAT_1f800218 + 0x24);
            gaddr = *(undefined4 *)guest_at(_DAT_1f800218 + 0x34);
          }
          prim_list_gte_invoke((int *)guest_at(gaddr),ot_base + 0x40,otidx,sbyte,0x1f800028,0x1f80033c);
        }
LAB_801706ac:
        if (((*(ushort *)(cmd_rec + 2) >> 6 & 1) != 0) && (DAT_1f80009f == '\0')) {
          if (DAT_1f800098 == 4) {
LAB_80170714:
            FUN_8018b100(_DAT_1f80020c,ot_base + 0x2040,otidx,0xf,0x1f800028,0x1f80035c);
          }
          else {
            if ((char)DAT_1f800098 < '\x05') {
              if (DAT_1f800098 == 1) goto LAB_80170714;
            }
            else if (DAT_1f800098 == 6) goto LAB_80170714;
            prim_list_persp_step((int *)guest_at(_DAT_1f80020c),ot_base + 0x2040,otidx,0xf,0x1f800028,0x1f80035c,
                         ((int)((uint)DAT_1f800098 << 0x18) >> 0x15) + -0x7fe4e94c,ot_base + 0x206c);
          }
        }
      }
      if ((*(ushort *)(cmd_rec + 2) >> 4 & 1) == 0) {
        DAT_1f8000b1 = 1;
      }
      battle_intro_fsm(0x1f800028,ot_base,otidx,0x1f800000);
      if (((_DAT_1f800154 & 0x8000) != 0) || (_DAT_801f4cb6 = 0xfff8, (_DAT_1f80025c & 0x8000) != 0)
         ) {
        _DAT_801f4cb6 = 0x10;
      }
      _DAT_801f4cb2 = 0;
      fighter_tint_fade(ot_base,otidx,0x1f800000,(char)DAT_1f800098);
      if ((*(ushort *)(cmd_rec + 2) >> 3 & 1) != 0) {
        fighter_trail_emit((undefined4 *)guest_at(0x1f800028),0x1f80033c,_DAT_1f8000bc,0x1f800000);
      }
      if (((DAT_1f80009f == '\0') && (DAT_1f80009e == '\0')) &&
         ((*(ushort *)(cmd_rec + 2) >> 5 & 1) != 0)) {
        slot_height_proj(ot_base + 0x207c,_DAT_1f8000bc,otidx,0x1f800000);
      }
      if (*(short *)(cmd_rec + 8) != 0) {
        FUN_801468ec(ot_base,otidx,0x1f800028,&DAT_1f8000e8,(int)*(short *)(cmd_rec + 8));
      }
      if (*(short *)(cmd_rec + 10) != 0) {
        FUN_801468ec(ot_base,otidx,0x1f800028,&DAT_1f8001f0,(int)*(short *)(cmd_rec + 10));
      }
      if (*(short *)(cmd_rec + 4) != 0) {
        status_text_display(0x1f8000e8u,(uint32_t)*(short *)(cmd_rec + 4));
      }
      if (*(short *)(cmd_rec + 6) != 0) {
        status_text_display(0x1f8001f0u,(uint32_t)*(short *)(cmd_rec + 6));
      }
    }
    break;
  case '\a':
    goto L_dat1_7;
  case '\b':
    goto L_dat1_8;
  case '\t':
    frame_present(0x1f800000);
    hud_sprite_desc_init();
    match_mode_set(1);
    blk_1f26c0_init();
    deferred_cmd_post(0x204,0,0);
    scroll_cfg_set(1,8);
    DAT_1f800010 = 0;
    DAT_1f80009e = '\0';
    task_table_init(1);
    rgb_triple_store(0,0,0);
    DAT_1f800001 = '\f';
    break;
  case '\v':
    hud_state_machine(1);
    if (DAT_1f800001 == '\v') {
      DAT_1f800001 = '\x02';
    }
    break;
  case '\f':
    res = frame_divisor_get();
    if (res != 0) break;
    DAT_1f800001 = -1;
    otidx = _DAT_1f80000c;
    ot_base = _DAT_1f800008;
    goto joined_r0x8016fd30;
  }
  frame_tick(0x1f800000);
  otidx = _DAT_1f80000c;
  ot_base = _DAT_1f800008;
  goto joined_r0x8016fd30;
}

/* ======== 0x8015c754 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 system_config_menu_a(void)

{
  short *pad_buf;
  intptr_t cfg_p, res;
  undefined4 ret;
  /* Guest stepper descriptor: one contiguous 0x30-byte block at
   * sp+0x10..0x3f passed to menu_stepper_state_advance / menu_stepper_sm
   * (a0 = sp+0x10); the +0xc callback is a u32 GUEST code address
   * (fn_lookup), stored as uint32_t — a host code* would write 8 bytes
   * and clobber +0x10..0x13. Ghidra split the block into scalars;
   * desc_a_fNN macros keep the step_desc_a byte offsets.
   * The block must live in guest RAM: menu_stepper_state_advance hands
   * guest_of(desc) to generated callbacks (dialog_msg_dispatch guest_at's
   * it), which is 0 for a host stack array. Reserve the real 0x70-byte
   * guest frame (prologue addiu sp,-0x70) and place the desc at
   * sp+0x10 -> frm+0x10. */
  uint32_t sca_frm = br2_guest_frame(0x70);
  uint8_t *step_desc_a = (uint8_t *)guest_at(sca_frm + 0x10);
#define desc_a_f00 (*(uint16_t *)&step_desc_a[0x00])
#define desc_a_f02 (*(uint8_t  *)&step_desc_a[0x02])
#define desc_a_f03 (*(uint8_t  *)&step_desc_a[0x03])
#define desc_a_f04 (*(uint8_t  *)&step_desc_a[0x04])
#define desc_a_f05 (*(uint8_t  *)&step_desc_a[0x05])
#define desc_a_f06 (*(int16_t  *)&step_desc_a[0x06])
#define desc_a_f08 (*(int16_t  *)&step_desc_a[0x08])
#define desc_a_f0c (*(uint32_t *)&step_desc_a[0x0c])
#define desc_a_f10 (*(uint16_t *)&step_desc_a[0x10])
#define desc_a_f12 (*(uint16_t *)&step_desc_a[0x12])
#define desc_a_f14 (*(uint16_t *)&step_desc_a[0x14])
#define desc_a_f16 (*(uint16_t *)&step_desc_a[0x16])
#define desc_a_f18 (*(uint16_t *)&step_desc_a[0x18])
#define desc_a_f1a (*(uint16_t *)&step_desc_a[0x1a])
#define desc_a_f1c (*(uint16_t *)&step_desc_a[0x1c])
#define desc_a_f1e (*(uint16_t *)&step_desc_a[0x1e])
#define desc_a_f20 (*(uint16_t *)&step_desc_a[0x20])
#define desc_a_f22 (*(uint16_t *)&step_desc_a[0x22])
#define desc_a_f24 (*(uint16_t *)&step_desc_a[0x24])
#define desc_a_f26 (*(uint16_t *)&step_desc_a[0x26])
#define desc_a_f28 (*(uint16_t *)&step_desc_a[0x28])
#define desc_a_f2a (*(uint16_t *)&step_desc_a[0x2a])
#define desc_a_f2c (*(uint16_t *)&step_desc_a[0x2c])
#define desc_a_f2e (*(uint16_t *)&step_desc_a[0x2e])

  pad_buf = _g_pad_buffer;
  ret = 0;
  _DAT_1f80008c = 0;
joined_r0x8015c784:
  /* Bail-unwind exit must preserve the DAT1 exit invariant: the 0xff exit
   * below writes DAT_1f800001 = 0 before returning; a bail-forced return
   * would leak a private substate into match_loop after gs->5 (same hazard
   * as battle_flow_state_machine). */
  {
    if (br2_bail_pending()) { DAT_1f800001 = 0; br2_guest_frame_abandon(); return ret; } }
  do {
    if (DAT_1f800001 == 0xff) {
      DAT_1f800001 = 0;
      br2_guest_frame_free();
      return ret;
    }
    if (DAT_1f800001 == 1) {
LAB_8015c900:
      if ((_DAT_1f80007c & 0x10) != 0) {
        deferred_cmd_post(0x400,0xd,0);
        DAT_1f800001 = 0xf0;
        ret = 1;
      }
      if (desc_a_f08 == desc_a_f06) {
        if ((*pad_buf == 0) || ((_DAT_1f800080 & 0xf) != 0xf)) {
          if ((_DAT_1f80007c & 0xe0) != 0) {
            deferred_cmd_post(0x400,0xc,0);
            DAT_1f800001 = 0xf0;
            ret = 1;
          }
        }
        else {
          menu_stepper_sm(_g_pad_buffer);
        }
      }
      menu_stepper_state_advance(&desc_a_f00);
      hud_gauge_render(0x1f800000,_DAT_1f800008,0);
    }
    else {
      if (1 < DAT_1f800001) {
        if (DAT_1f800001 == 0xf0) {
          *(undefined1 *)(_g_system_config_ptr + 7) = (undefined1)desc_a_f18;
          *(undefined1 *)(_g_system_config_ptr + 8) = (undefined1)desc_a_f1a;
          *(undefined1 *)(_g_system_config_ptr + 9) = (undefined1)desc_a_f1c;
          cfg_p = (intptr_t)_g_system_config_ptr;
          *(undefined1 *)(_g_system_config_ptr + 6) = (undefined1)desc_a_f1e;
          *(undefined1 *)(cfg_p + 5) = (undefined1)desc_a_f1e;
          *(undefined1 *)(_g_system_config_ptr + 0x11) = (undefined1)desc_a_f20;
          *(undefined1 *)(_g_system_config_ptr + 0x25) = (undefined1)desc_a_f22;
          *(undefined1 *)(_g_system_config_ptr + 0x24) = (undefined1)desc_a_f2a;
          dbg_marker_mode_set((int)(short)desc_a_f2a);
          deferred_cmd_post(0x203,0,0);
        }
        else if (DAT_1f800001 != 0xf1) goto LAB_8015ca68;
        DAT_1f800001 = 0xff;
        goto joined_r0x8015c784;
      }
      if ((DAT_1f800001 == 0) && (res = frame_divisor_get(), res == 0)) {
        print_mode_dispatch_b(2,0,0);
        res = intro_movie_state(0);
        if (res != 0) {
          deferred_cmd_post(0x203,0,0);
          /* desc+0xc is read by menu_stepper_state_advance as a u32 GUEST
           * code address (fn_lookup -> call_guest fallback). Guest stores
           * 0x8015CAA8 there (generated 0x8015C814); a host fn pointer
           * would be truncated to garbage. */
          desc_a_f0c = 0x8015caa8;
          desc_a_f06 = 0xe;
          desc_a_f08 = 0xe;
          desc_a_f00 = 0;
          desc_a_f02 = 0;
          desc_a_f03 = 0;
          desc_a_f04 = 1;
          desc_a_f05 = 0;
          desc_a_f10 = 0x10;
          desc_a_f12 = 0xffff;
          desc_a_f16 = 0;
          desc_a_f14 = 0;
          desc_a_f24 = 0;
          desc_a_f26 = 0;
          desc_a_f28 = 0;
          desc_a_f18 = (ushort)*(byte *)(_g_system_config_ptr + 7);
          desc_a_f1a = (ushort)*(byte *)(_g_system_config_ptr + 8);
          desc_a_f1c = (ushort)*(byte *)(_g_system_config_ptr + 9);
          desc_a_f1e = (ushort)*(byte *)(_g_system_config_ptr + 5);
          desc_a_f20 = (ushort)*(byte *)(_g_system_config_ptr + 0x11);
          desc_a_f22 = (ushort)*(byte *)(_g_system_config_ptr + 0x25);
          desc_a_f2a = (ushort)*(byte *)(_g_system_config_ptr + 0x24);
          desc_a_f2c = 0;
          desc_a_f2e = 0;
          sprite_desc_table_init(1);
          print_mode_dispatch_b(2,0,0);
          _DAT_801e8508 = 0;
          _DAT_801e8518 = 0x30;
          battle_session_arm(0x1f800000,0xffffffff);
          DAT_1f800001 = DAT_1f800001 + 1;
          goto LAB_8015c900;
        }
      }
    }
LAB_8015ca68:
    frame_tick(0x1f800000);
  } while( true );
}
#undef desc_a_f00
#undef desc_a_f02
#undef desc_a_f03
#undef desc_a_f04
#undef desc_a_f05
#undef desc_a_f06
#undef desc_a_f08
#undef desc_a_f0c
#undef desc_a_f10
#undef desc_a_f12
#undef desc_a_f14
#undef desc_a_f16
#undef desc_a_f18
#undef desc_a_f1a
#undef desc_a_f1c
#undef desc_a_f1e
#undef desc_a_f20
#undef desc_a_f22
#undef desc_a_f24
#undef desc_a_f26
#undef desc_a_f28
#undef desc_a_f2a
#undef desc_a_f2c
#undef desc_a_f2e

/* ======== 0x8015b9a0 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 system_config_menu_b(void)

{
  intptr_t cfg_base;
  int res;
  uint cfg_flags;
  byte *cfg_ptr;
  uint step_arg;
  undefined4 ret;
  /* Guest stepper descriptor: one contiguous 0x28-byte block at
   * sp+0x10..0x37 passed to menu_stepper_button_wait (a0 = sp+0x10).
   * The +0xc callback is a u32 GUEST code address like every other
   * stepper desc (guest stores 0x8015BDC0 = dialog_sel_dispatch_wide);
   * menu_stepper_button_wait/masked_sweep dispatch it via fn_lookup,
   * so it is stored as uint32_t — a host code* would write 8 bytes and
   * clobber +0x10..0x13. Ghidra split the block into scalars;
   * desc_b_fNN macros keep the step_desc_b byte offsets.
   * The block must live in guest RAM: the stepper helpers hand
   * guest_of(desc) to generated callbacks (dialog_sel_dispatch_wide
   * guest_at's it), which is 0 for a host stack array. Reserve the real
   * 0x78-byte guest frame (prologue addiu sp,-0x78) and place the desc
   * at sp+0x10 -> frm+0x10. */
  uint32_t scb_frm = br2_guest_frame(0x78);
  uint8_t *step_desc_b = (uint8_t *)guest_at(scb_frm + 0x10);
#define desc_b_f00 (*(uint16_t *)&step_desc_b[0x00])
#define desc_b_f02 (*(uint8_t  *)&step_desc_b[0x02])
#define desc_b_f03 (*(uint8_t  *)&step_desc_b[0x03])
#define desc_b_f04 (*(uint8_t  *)&step_desc_b[0x04])
#define desc_b_f05 (*(uint8_t  *)&step_desc_b[0x05])
#define desc_b_f06 (*(int16_t  *)&step_desc_b[0x06])
#define desc_b_f08 (*(int16_t  *)&step_desc_b[0x08])
#define desc_b_f0c (*(uint32_t *)&step_desc_b[0x0c])
#define desc_b_f10 (*(uint16_t *)&step_desc_b[0x10])
#define desc_b_f12 (*(uint16_t *)&step_desc_b[0x12])
#define desc_b_f14 (*(uint16_t *)&step_desc_b[0x14])
#define desc_b_f16 (*(uint16_t *)&step_desc_b[0x16])
#define desc_b_f18 (*(uint16_t *)&step_desc_b[0x18])
#define desc_b_f1a (*(uint16_t *)&step_desc_b[0x1a])
#define desc_b_f1c (*(uint16_t *)&step_desc_b[0x1c])
#define desc_b_f1e (*(uint16_t *)&step_desc_b[0x1e])
#define desc_b_f20 (*(uint16_t *)&step_desc_b[0x20])
#define desc_b_f22 (*(uint16_t *)&step_desc_b[0x22])
#define desc_b_f24 (*(uint16_t *)&step_desc_b[0x24])
#define desc_b_f26 (*(uint16_t *)&step_desc_b[0x26])

  cfg_base = (intptr_t)_g_system_config_ptr;
  ret = 0;
  step_arg = 0;
  _DAT_1f80008c = 0;
  cfg_ptr = (byte *)(_g_system_config_ptr + 0x38);
joined_r0x8015b9e0:
  /* Bail-unwind exit must preserve the DAT1 exit invariant (0xff exit
   * writes DAT_1f800001 = 0); see system_config_menu_a. */
  {
    if (br2_bail_pending()) { DAT_1f800001 = 0; br2_guest_frame_abandon(); return ret; } }
  do {
    if (DAT_1f800001 == 0xff) {
      DAT_1f800001 = 0;
      br2_guest_frame_free();
      return ret;
    }
    if (DAT_1f800001 == 1) {
LAB_8015bbd8:
      if ((_DAT_1f80007c & 0x10) != 0) {
        deferred_cmd_post(0x400,0xd,0);
        DAT_1f800001 = 0xf0;
        ret = 1;
      }
      if ((desc_b_f08 == desc_b_f06) && ((_DAT_1f80007c & 0xe0) != 0)) {
        deferred_cmd_post(0x400,0xc,0);
        DAT_1f800001 = 0xf0;
        ret = 1;
      }
      menu_stepper_button_wait(&desc_b_f00,step_arg);
      hud_gauge_render(0x1f800000,_DAT_1f800008,1);
    }
    else {
      if (1 < DAT_1f800001) {
        if (DAT_1f800001 == 0xf0) {
          *cfg_ptr = (byte)desc_b_f14;
          *(undefined1 *)(cfg_base + 0x39) = (undefined1)desc_b_f16;
          *(undefined1 *)(cfg_base + 0x3a) = (undefined1)desc_b_f18;
          if (desc_b_f1a == 0) {
            cfg_flags = *(uint *)(cfg_base + 0x40) & 0xffffdfff;
          }
          else {
            cfg_flags = *(uint *)(cfg_base + 0x40) | 0x2000;
          }
          *(uint *)(cfg_base + 0x40) = cfg_flags;
          *(undefined1 *)(cfg_base + 0x3b) = (undefined1)desc_b_f1c;
          *(undefined1 *)(cfg_base + 0x3c) = (undefined1)desc_b_f1e;
          if (desc_b_f20 == 0) {
            cfg_flags = *(uint *)(cfg_base + 0x40) | 0x40;
          }
          else {
            cfg_flags = *(uint *)(cfg_base + 0x40) & 0xffffffbf;
          }
          *(uint *)(cfg_base + 0x40) = cfg_flags;
          if (desc_b_f22 == 0) {
            cfg_flags = *(uint *)(cfg_base + 0x40) & 0xfffff7ff;
          }
          else {
            cfg_flags = *(uint *)(cfg_base + 0x40) | 0x800;
          }
          *(uint *)(cfg_base + 0x40) = cfg_flags;
          if (desc_b_f24 == 0) {
            cfg_flags = *(uint *)(cfg_base + 0x40) & 0xfffffbff;
          }
          else {
            cfg_flags = *(uint *)(cfg_base + 0x40) | 0x400;
          }
          *(uint *)(cfg_base + 0x40) = cfg_flags;
          deferred_cmd_post(0x203,0,0);
        }
        else if (DAT_1f800001 != 0xf1) goto LAB_8015bd78;
        DAT_1f800001 = 0xff;
        goto joined_r0x8015b9e0;
      }
      if (((DAT_1f800001 == 0) && (res = frame_divisor_get(), res == 0)) &&
         (res = intro_movie_state(0), res != 0)) {
        deferred_cmd_post(0x203,0,0);
        desc_b_f0c = 0x8015bdc0;   /* dialog_sel_dispatch_wide, guest addr */
        desc_b_f06 = 0xb;
        desc_b_f10 = 0x10;
        desc_b_f00 = 0;
        desc_b_f02 = 0;
        desc_b_f03 = 0;
        desc_b_f04 = 1;
        desc_b_f05 = 0;
        desc_b_f08 = 1;
        desc_b_f12 = 0xffff;
        desc_b_f26 = 0;
        desc_b_f14 = (ushort)*cfg_ptr;
        desc_b_f16 = (ushort)*(byte *)(cfg_base + 0x39);
        desc_b_f18 = (ushort)*(byte *)(cfg_base + 0x3a);
        desc_b_f1a = (ushort)(*(uint *)(cfg_base + 0x40) >> 0xd) & 1;
        desc_b_f1c = (ushort)*(byte *)(cfg_base + 0x3b);
        desc_b_f1e = (ushort)*(byte *)(cfg_base + 0x3c);
        desc_b_f20 = ((ushort)(*(uint *)(cfg_base + 0x40) >> 6) ^ 1) & 1;
        desc_b_f22 = (ushort)(*(uint *)(cfg_base + 0x40) >> 0xb) & 1;
        desc_b_f24 = (ushort)(*(uint *)(cfg_base + 0x40) >> 10) & 1;
        step_arg = *(uint *)(cfg_base + 0x44);
        step_arg = (step_arg & 1) << 2 | 0xc03 | step_arg >> 0xd & 8 | step_arg >> 9 & 0x10 | step_arg >> 9 & 0x20
                | step_arg >> 9 & 0x40 | (step_arg & 0x40) << 1 | step_arg >> 3 & 0x100 | step_arg >> 1 & 0x200;
        sprite_desc_table_init();
        print_mode_dispatch_b(2,0,0);
        _DAT_801e8508 = 0;
        _DAT_801e8518 = 0x30;
        battle_session_arm(0x1f800000,0xffffffff);
        DAT_1f800001 = DAT_1f800001 + 1;
        goto LAB_8015bbd8;
      }
    }
LAB_8015bd78:
    frame_tick(0x1f800000);
  } while( true );
}
#undef desc_b_f00
#undef desc_b_f02
#undef desc_b_f03
#undef desc_b_f04
#undef desc_b_f05
#undef desc_b_f06
#undef desc_b_f08
#undef desc_b_f0c
#undef desc_b_f10
#undef desc_b_f12
#undef desc_b_f14
#undef desc_b_f16
#undef desc_b_f18
#undef desc_b_f1a
#undef desc_b_f1c
#undef desc_b_f1e
#undef desc_b_f20
#undef desc_b_f22
#undef desc_b_f24
#undef desc_b_f26

/* ======== 0x8016362c ======== */

void menu_noop(void)

{
  return;
}

/* ======== 0x80164668 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void results_scoreboard(int mode)

{
  int res, ty, ri, ry;
  undefined4 draw_mode;
  char *title_str;
  undefined2 attr;
  int rowy, fv;
  int rec_off;
  int row_off;
  uint32_t *name_tbl;
  /* sprintf dst = guest $sp+0x18 (frame -0x88) = entry_sp-0x70; the
   * rank strings are re-read by announce_param_block_init_b/hud_time_score_draw
   * across the whole tally loop, so reserve the guest frame window
   * (sp+0x18 -> frm+0x20) for the function's lifetime. */
  uint32_t rsb_frm = br2_guest_frame(0x88);
  undefined1 *num_buf = (undefined1 *)guest_at(rsb_frm + 0x20);
  /* The tally state (tally slots, slide offsets, fill index, fade, name
   * table pointers) lives in the guest frame (sp+0x28..0x58 -> frm+0x30..0x60,
   * +8 skew from the 0x88->0x90 align) so it survives across per-frame calls;
   * names_a/names_b hold guest addresses, translated at the hud call. */
  int32_t *rsb_slots = (int32_t *)guest_at(rsb_frm + 0x30);
#define tally   rsb_slots
#define countdown   rsb_slots[6]
#define fade_level   rsb_slots[7]
#define slide_a   rsb_slots[8]
#define slide_b   rsb_slots[9]
#define fill_idx   rsb_slots[10]
#define names_a   (*(uint32_t *)guest_at(rsb_frm + 0x5c))
#define names_b   (*(uint32_t *)guest_at(rsb_frm + 0x60))
  uint32_t name_g;

  if (mode == 1) {
    names_b = 0x801c13a4;
    goto LAB_80164714;
  }
  if (mode < 2) {
    if (mode != 0) goto LAB_80164714;
    names_a = 0x801c1264;
  }
  else {
    if (mode == 2) {
      names_b = 0x801c1304;
      goto LAB_80164714;
    }
    if (mode != 3) goto LAB_80164714;
    names_a = 0x801c13a4;
  }
  names_b = 0x801c1304;
LAB_80164714:
  res = memcard_err_sentinel();
  if (res < 0) {
    _DAT_1f80008c = 0;
joined_r0x80164744:
    /* Bail-unwind exit must preserve the DAT1 exit invariant: every normal
     * exit of this DAT1 state machine writes DAT_1f800001 = 0 before
     * returning; a bail-forced return would leak a private substate into
     * match_loop after the caller's gs->5 transition (same hazard as
     * battle_flow_state_machine — see its loop head). */
    {
      if (br2_bail_pending()) { DAT_1f800001 = 0; return; } }
    if (DAT_1f800001 != 0xff) {
      if (DAT_1f800001 == 1) {
LAB_80164830:
        countdown = countdown + -1;
        if (countdown == 0) {
          DAT_1f800001 = 0xf0;
        }
        if ((((-1 < mode) && (mode < 2)) && (countdown < 0x1f5)) && (slide_b != 0)) {
          slide_a = slide_a + -4;
          slide_b = slide_b + -4;
        }
        if (-1 < fill_idx) {
          if (fill_idx < 5) {
            tally[fill_idx] = 0;
            fill_idx = fill_idx + 1;
          }
          else if (fill_idx == 5) {
            if (fade_level < 0xff) {
              fade_level = fade_level + 4;
            }
            if (0xff < fade_level) {
              fade_level = 0xff;
            }
          }
        }
        attr = 0x10;
        if (DAT_1f800015 != '\0') {
          attr = 0x20;
        }
        if (mode < 3) {
          if (mode < 1) {
            if (mode != 0) goto LAB_80164a2c;
            title_str = s_RANKING_80122048;
            ty = _DAT_1f800020 + -0x62;
          }
          else {
            title_str = s_TIME_ATTACK_80122050;
            ty = _DAT_1f800020 + -0x9a;
          }
LAB_80164a00:
          announce_param_block_init_b(_DAT_1f800008 + 4,_DAT_1f80000c,title_str,
                       (ty / 2 + slide_a) * 0x10000 >> 0x10,attr,1);
        }
        else if (mode == 3) {
          title_str = s_SURVIVAL_8012205c;
          ty = _DAT_1f800020 + -0x70;
          goto LAB_80164a00;
        }
LAB_80164a2c:
        rec_off = 0;
        row_off = 0;
        ri = rec_off;
        name_g = names_a;
        do {
          if (DAT_1f800015 == '\0') {
            rowy = row_off + 0x30;
          }
          else {
            rowy = (row_off + 0x30) * 2;
          }
          if (-1 < *(int *)((char *)tally + rec_off)) {
            sprintf_core((char *)num_buf,&DAT_80121fe0,ri + 1);
            announce_param_block_init_b(_DAT_1f800008 + 0x14,_DAT_1f80000c,num_buf,
                         (slide_a + 0x38) * 0x10000 >> 0x10,(int)(short)rowy,3);
            announce_param_block_init_b(_DAT_1f800008 + 0x14,_DAT_1f80000c,
                         (const void *)guest_at(*(uint32_t *)((char *)&PTR_DAT_801b4254 + rec_off)),
                         (slide_a + 0x48) * 0x10000 >> 0x10,(int)(short)rowy,4);
            if (-1 < mode) {
              if (mode < 3) {
                draw_mode = 0;
              }
              else {
                if (mode != 3) goto LAB_80164b68;
                draw_mode = 5;
              }
              hud_time_score_draw(_DAT_1f800008 + 0x14,_DAT_1f80000c,slide_a + 0x70,rowy,(uint8_t *)guest_at(name_g),draw_mode);
            }
LAB_80164b68:
            fv = *(int *)((char *)tally + rec_off);
            if (fv < _DAT_1f800020 + 0x80) {
              *(int *)((char *)tally + rec_off) = fv + 0x10;
            }
          }
          rec_off = rec_off + 4;
          name_g = name_g + 0x10;
          ri = ri + 1;
          row_off = row_off + 0x20;
        } while (ri < 5);
        attr = 0x10;
        if (DAT_1f800015 != '\0') {
          attr = 0x20;
        }
        if (mode == 0) {
          title_str = s_VS_RANKING_80122068;
          ty = _DAT_1f800020 + -0x8c;
LAB_80164c38:
          announce_param_block_init_b(_DAT_1f800008 + 4,_DAT_1f80000c,title_str,
                       (ty / 2 + slide_b) * 0x10000 >> 0x10,attr,1);
        }
        else if (mode == 1) {
          title_str = s_SURVIVAL_8012205c;
          ty = _DAT_1f800020 + -0x70;
          goto LAB_80164c38;
        }
        rec_off = 0;
        name_tbl = &PTR_DAT_801b4254;
        ry = 0;
        name_g = names_b;
        do {
          if (DAT_1f800015 == '\0') {
            row_off = ry + 0x30;
          }
          else {
            row_off = (ry + 0x30) * 2;
          }
          sprintf_core((char *)num_buf,&DAT_80121fe0,rec_off + 1);
          announce_param_block_init_b(_DAT_1f800008 + 0x14,_DAT_1f80000c,num_buf,
                       (slide_b + 0x38) * 0x10000 >> 0x10,(int)(short)row_off,3);
          announce_param_block_init_b(_DAT_1f800008 + 0x14,_DAT_1f80000c,(const void *)guest_at((uint32_t)(uintptr_t)*name_tbl),
                       (slide_b + 0x48) * 0x10000 >> 0x10,(int)(short)row_off,4);
          if (mode == 1) {
            draw_mode = 5;
LAB_80164d80:
            hud_time_score_draw(_DAT_1f800008 + 0x14,_DAT_1f80000c,slide_b + 0x70,row_off,(uint8_t *)guest_at(name_g),draw_mode);
          }
          else if (mode < 2) {
            if (mode == 0) {
LAB_80164d68:
              draw_mode = 4;
              goto LAB_80164d80;
            }
          }
          else if (mode < 4) goto LAB_80164d68;
          name_g = name_g + 0x10;
          name_tbl = name_tbl + 1;
          rec_off = rec_off + 1;
          ry = ry + 0x20;
        } while (rec_off < 5);
        if (fade_level != 0) {
          fighter_reinit_tail(0x1f800000);
        }
        *(short *)guest_at(_DAT_1f8000bc + 0x32) = *(short *)guest_at(_DAT_1f8000bc + 0x32) + 1;
      }
      else {
        if (1 < DAT_1f800001) {
          if (DAT_1f800001 == 0xf0) {
            deferred_cmd_post(0x204,0,0);
            scroll_cfg_set(1,8);
            rgb_triple_store(0,0,0);
          }
          else if (DAT_1f800001 != 0xf1) goto LAB_80164e34;
          DAT_1f800001 = 0xff;
          goto joined_r0x80164744;
        }
        if ((DAT_1f800001 == 0) && (res = frame_divisor_get(), res == 0)) {
          if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            draw_mode = 9;
            if (*(char *)(_g_system_config_ptr + 0xd) == '\0') {
              draw_mode = 0x29;
            }
            mc_confirm_dispatch(draw_mode,1);
          }
          fighter_respawn_init(0x1f800000);
          fill_idx = 0;
          fade_level = 0;
          slide_a = 0;
          slide_b = 0x200;
          battle_field_set(_DAT_1f8000bc,0x1e);
          tally[4] = 0xffffffff;
          tally[3] = 0xffffffff;
          tally[2] = 0xffffffff;
          tally[1] = 0xffffffff;
          tally[0] = -1;
          battle_session_arm(0x1f800000,0);
          countdown = 1000;
          DAT_1f800001 = DAT_1f800001 + 1;
          goto LAB_80164830;
        }
      }
LAB_80164e34:
      frame_tick(0x1f800000);
      if (DAT_1f800001 == 1) {
        save_slot_state_fixup(0x1f800000,1);
      }
      goto joined_r0x80164744;
    }
  }
  else {
    g_game_state = 5;
  }
  DAT_1f800001 = 0;
  br2_guest_frame_free();
#undef tally
#undef countdown
#undef fade_level
#undef slide_a
#undef slide_b
#undef fill_idx
#undef names_a
#undef names_b
  return;
}

/* ======== 0x80164ea4 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void player_name_entry(void)

{
  bool more;

  undefined4 aux_mode;
  char *name_ch;
  int disp_i;
  int char_x;
  int char_sel;
  uint32_t *name_tbl;
  undefined1 *rec_ptr;
  int name_len;
  /* Guest string slots at sp+0x18/0x20/0x28/0x30 (8-byte stride, offsets
   * verified against the generated body's sp accesses): slot0 @sp+0x18 is
   * the name buffer — name_buf u32 at +0 with name chars appended through
   * name_ptr = &name_buf + name_len — and slot3 @sp+0x30 is the disp_str
   * per-char display slot ("A\0" -> char + NUL). announce_param_block_init_alt
   * stores guest_of(param_3) into the text prim, so both must live in guest
   * RAM (guest_of is 0 for host locals); name_rec_insert/name_rec_gate/
   * hud_name_fixup/rec_u32_store read the same bytes through host pointers,
   * which guest_at() backing satisfies. Keep the whole 0x20-byte run inside
   * the reserved frame: pne_slots[i] == old local_XX. */
  /* sprintf dst = guest $sp+0x38 (frame -0x90) = entry_sp-0x58; the name
   * strings are re-read by announce_param_block_init / rec_u32_store
   * across the entry loop, so reserve the guest frame window
   * (sp+0x38 -> frm+0x38) for the function's lifetime. */
  uint32_t pne_frm = br2_guest_frame(0x90);
  undefined1 *num_buf = (undefined1 *)guest_at(pne_frm + 0x38);
  uint8_t *pne_slots = (uint8_t *)guest_at(pne_frm + 0x18);
#define name_buf   (*(uint32_t *)pne_slots)
#define slot1_b0   pne_slots[8]
#define slot1_b1   pne_slots[9]
#define slot1_b2   pne_slots[10]
#define slot2_b0   pne_slots[16]
#define slot2_b1   pne_slots[17]
#define disp_str   (pne_slots + 24)
  /* wait_timer/rec_idx/records are guest stack slots too (sp+0x40/0x4c/0x58):
   * the name-entry state machine carries them across calls via the stable
   * guest frame, so host locals would lose the countdown/selection state.
   * records_g holds the guest address; `records` translates it per use. */
#define wait_timer  (*(int32_t *)guest_at(pne_frm + 0x40))
#define rec_idx     (*(int32_t *)guest_at(pne_frm + 0x4c))
#define records_g   (*(uint32_t *)guest_at(pne_frm + 0x58))
#define records     ((undefined1 *)guest_at(records_g))
  undefined1 *spad_cfg_a;
  undefined1 *spad_cfg_b;
  undefined1 *active_bank;
  uint slot_idx;
  undefined1 *fmt_buf;
  undefined4 *name_ptr;

  char_sel = 0;
  spad_cfg_a = &DAT_1f8000e8;
  spad_cfg_b = &DAT_1f8001f0;
  active_bank = &DAT_1f8000e8;
  slot_idx = 0;
  name_buf = DAT_80121f30;
  slot1_b0 = DAT_80122074;
  slot1_b1 = DAT_80122075;
  slot1_b2 = DAT_80122076;
  slot2_b0 = DAT_80122078;
  slot2_b1 = DAT_80122079;
  disp_str[0] = DAT_8012207c;
  disp_str[1] = DAT_8012207d;
  name_len = 0;
  if (DAT_1f80009c == 3) {
    records_g = 0x801c13a4;
  }
  else if (DAT_1f80009c < 4) {
    records_g = 0x801c1264;
  }
  else if (DAT_1f80009c == 5) {
    records_g = 0x801c1264;
  }
  _DAT_1f80008c = 0;
  if (DAT_1f800001 != 0xff) {
    name_ptr = &name_buf;
    fmt_buf = num_buf;
    do {
      if (DAT_1f800001 == 3) {
LAB_801652bc:
        banner_stripe_scroll(2,1,3,0);
        int32_t tab_off = slot_idx * 2;
        if ((*(ushort *)(&DAT_1f800074 + tab_off) & 0x8000) != 0) {
          deferred_cmd_post(0x400,0xb,0);
          char_sel = char_sel + -1;
        }
        if ((*(ushort *)(&DAT_1f800074 + tab_off) & 0x2000) != 0) {
          deferred_cmd_post(0x400,0xb,0);
          char_sel = char_sel + 1;
        }
        int32_t csel = (char_sel + 0x2b) % 0x2b;
        char_sel = csel;
        if (2 < name_len) {
          if (csel < 0x1f) {
            char_sel = 0x1f;
          }
          if (0x20 < csel) {
            char_sel = 0x20;
          }
        }
        name_ch = (char *)name_ptr + name_len;
        *name_ch = ((char *)guest_at(0x801b43bc))[char_sel];
        if ((*(ushort *)(&DAT_1f800068 + slot_idx * 2) & 0xe0) != 0) {
          if (*name_ch == '<') {
            deferred_cmd_post(0x400,0xd,0);
            if (0 < name_len) {
              *name_ch = ' ';
              name_len = name_len + -1;
            }
          }
          else if (*name_ch == '^') {
            deferred_cmd_post(0x400,0xc,0);
            *name_ch = ' ';
            name_buf = name_buf & 0xffffff;
            hud_name_fixup(name_ptr);
            rec_u32_store(records,rec_idx,name_ptr);
            *_g_system_config_ptr = (char)name_buf;
            _g_system_config_ptr[1] = ((uint8_t *)&name_buf)[1];
            _g_system_config_ptr[2] = ((uint8_t *)&name_buf)[2];
            wait_timer = 0;
            _g_system_config_ptr[3] = '\0';
            DAT_1f800001 = 4;
            _DAT_801e850c = _DAT_1f800020;
          }
          else {
            deferred_cmd_post(0x400,0xc,0);
            name_len = name_len + 1;
            if (2 < name_len) {
              char_sel = 0x20;
            }
          }
        }
LAB_801654c0:
        scroll_offset_set(0x20,0x30);
        int ri = 0;
        name_tbl = &PTR_DAT_801b4254;
        rec_ptr = records;
        do {
          sprintf_core((char *)fmt_buf,&DAT_80121fe0,ri + 1);
          if (rec_idx == ri) {
            scroll_offset_accumulate(0,(int16_t)0xfffffffc);
            announce_param_block_init_alt(_DAT_1f800008,_DAT_1f80000c,fmt_buf,0x20,0x1000,3);
            announce_param_block_init_alt(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at((uint32_t)(uintptr_t)*name_tbl),0x1000,0x1000,0);
            aux_mode = 0x13;
            if (DAT_1f80009c == 3) {
              aux_mode = 0x15;
            }
            hud_time_score_draw(_DAT_1f800008,_DAT_1f80000c,0x60,0x1000,rec_ptr,aux_mode);
            char_x = 0x108;
            if (DAT_1f800001 == 3) {
              disp_i = 0;
              if (-1 < name_len) {
                more = true;
                while (more) {
                  if ((disp_i != name_len) || ((_DAT_1f8000dc & 8) != 0)) {
                    disp_str[0] = *(undefined1 *)((char *)name_ptr + disp_i);
                    announce_param_block_init_alt(_DAT_1f800008,_DAT_1f80000c,disp_str,char_x,0x1000,0);
                  }
                  disp_i = disp_i + 1;
                  char_x = char_x + 0x10;
                  if (name_len < disp_i) break;
                  more = disp_i < 4;
                }
              }
            }
            else {
              if ((_DAT_1f8000dc & 4) == 0) {
                aux_mode = 3;
              }
              else {
                aux_mode = 0;
              }
              announce_param_block_init_alt(_DAT_1f800008,_DAT_1f80000c,&name_buf,0x108,0x1000,aux_mode);
            }
            aux_mode = 0x14;
          }
          else {
            announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,fmt_buf,0x20,0x1000,3);
            announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at((uint32_t)(uintptr_t)*name_tbl),0x1000,0x1000,0);
            aux_mode = 0x12;
            if (DAT_1f80009c == 3) {
              aux_mode = 0x14;
            }
            hud_time_score_draw(_DAT_1f800008,_DAT_1f80000c,0x60,0x1000,rec_ptr,aux_mode);
            aux_mode = 0x10;
          }
          rec_ptr = rec_ptr + 0x10;
          scroll_offset_accumulate(0,aux_mode);
          ri = ri + 1;
          name_tbl = name_tbl + 1;
        } while (ri < 10);
        scroll_offset_set(0x20,0x18);
        announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,s_NAME_ENTRY_80122080,0x1000,0x1000,0);
        banner_stripe_scroll(0,0,4,7);
        hud_gauge_render(0x1f800000,_DAT_1f800008,0x23);
LAB_801658b4:
        frame_tick(0x1f800000);
      }
      else {
        int32_t res;
        if (DAT_1f800001 < 4) {
          if (DAT_1f800001 == 1) {
LAB_80165090:
            res = menu_sprite_load_sm(0x1f800000,0x8d,0x12,2,1);
            if (res == 0) goto LAB_801658b4;
            _DAT_801e8508 = 0;
            _DAT_801e8518 = 0x30;
            DAT_1f800001 = 2;
            _DAT_801e850c = _DAT_1f800020;
            _DAT_801e851c = _DAT_1f800022 + -0x3c;
          }
          else if (DAT_1f800001 < 2) {
            if (((DAT_1f800001 != 0) || (res = frame_divisor_get(), res != 0)) ||
               (res = intro_movie_state(0), res == 0)) goto LAB_801658b4;
            sprite_desc_table_init(1);
            print_mode_dispatch_b(2,0,0);
            _DAT_1f80008c = 0;
            DAT_1f800001 = 1;
            goto LAB_80165090;
          }
          if ((_DAT_1f8000b4 & 0xf00) == 0xa00) {
            active_bank = spad_cfg_a;
          }
          if ((_DAT_1f8000b6 & 0xf00) == 0xa00) {
            active_bank = spad_cfg_b;
          }
          slot_idx = (uint)(byte)active_bank[2];
          switch(DAT_1f80009c) {
          case 0:
            if ((byte)_g_system_config_ptr[9] < 2) goto LAB_801651e8;
            if (_g_system_config_ptr[6] == '\0') {
              rec_idx = -1;
            }
            else {
              rec_idx = rec_find_lt_f8(records,*(undefined4 *)(active_bank + 0xf0),10);
            }
            break;
          case 1:
          case 2:
          case 4:
          case 5:
LAB_801651e8:
            rec_idx = -1;
            break;
          case 3:
            rec_idx = rec_find_ge_e(records,*(undefined2 *)(active_bank + 0xe6),10);
          }
          if (*(int *)((uint32_t *)guest_at(_DAT_1f8000c4) + 8) != 0) {
            rec_idx = -1;
          }
          if (rec_idx < 0) {
            wait_timer = 0;
            DAT_1f800001 = 4;
            name_buf = name_buf & 0xffffff00;
          }
          else {
            aux_mode = 9;
            if (_g_system_config_ptr[0xd] == '\0') {
              aux_mode = 0x29;
            }
            mc_confirm_dispatch(aux_mode,1);
            if (*_g_system_config_ptr != '\0') {
              name_len = 3;
              char_sel = 0x20;
              name_buf = (uint)CONCAT12(_g_system_config_ptr[2],
                                        CONCAT11(_g_system_config_ptr[1],*_g_system_config_ptr));
            }
            name_rec_insert(records,rec_idx,active_bank,&name_buf,10);
            wait_timer = -1;
            DAT_1f800001 = 3;
          }
          battle_session_arm(0x1f800000,0xffffffff);
          goto LAB_801652bc;
        }
        if (DAT_1f800001 == 0xf0) {
          rec_timer_pair_clear(0x1f800000);
          name_rec_gate(active_bank,(const void *)&name_buf);
          status_flag_cond_set(guest_of(active_bank),1);
          deferred_cmd_post(0x204,0,0);
          frame_present(0x1f800000);
          scroll_cfg_set(1,8);
          rgb_triple_store(0,0,0);
          DAT_1f800001 = DAT_1f800001 + 1;
          goto LAB_801658b4;
        }
        if (DAT_1f800001 < 0xf1) {
          if (DAT_1f800001 != 4) goto LAB_801658b4;
          if (wait_timer == 0) {
            aux_mode = 7;
            if (_g_system_config_ptr[0xd] == '\0') {
              aux_mode = 0x28;
            }
            mc_confirm_dispatch(aux_mode,0);
            deferred_cmd_post(0x400,0x83,0x1e);
            deferred_cmd_post(0x400,0x84,0x23);
            deferred_cmd_post(0x400,0x85,0x28);
          }
          if ((100 < wait_timer) && ((_DAT_1f80007c & 0x8e0) != 0)) {
            wait_timer = 0x1356;
          }
          if (0x2ee < wait_timer) {
            DAT_1f800001 = 0xf0;
          }
          wait_timer = wait_timer + 1;
          banner_stripe_scroll(1,1,3,3);
          goto LAB_801654c0;
        }
        if ((DAT_1f800001 != 0xf1) || (res = frame_divisor_get(), res != 0)) goto LAB_801658b4;
        DAT_1f800001 = 0xff;
      }
    } while (DAT_1f800001 != 0xff);
  }
  DAT_1f800001 = 0;
  br2_guest_frame_free();
  return;
}
#undef name_buf
#undef slot1_b0
#undef slot1_b1
#undef slot1_b2
#undef slot2_b0
#undef slot2_b1
#undef disp_str
#undef wait_timer
#undef rec_idx
#undef records_g
#undef records

/* ======== 0x80165cb4 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void transition_screen_flow(int mode)

{
  int fade_acc;
  int val, res;
  uint gray;
  undefined4 spr_code;
  /* guest keeps the fade counter in $s1: on the DAT_1f800001!=0xff path it is
   * read before any write, so the faithful host value is the live gpr[17]
   * (callee-saved: at entry it still holds the caller's $s1). */
  int fade_ctr = (int)clean_cpu_state()->gpr[17];

  if ((mode != 0) || (_DAT_801c4578 == 1)) {
    fade_acc = memcard_err_sentinel();
    if (fade_acc < 0) {
      _DAT_1f80008c = 0;
      if (DAT_1f800001 != 0xff) {
        fade_acc = fade_ctr * 0xff + -0x956a;
        do {
          if (DAT_1f800001 == 1) {
LAB_80165dd8:
            fade_ctr = fade_ctr + -1;
            fade_acc = fade_acc + -0xff;
            if (fade_ctr == 0) {
              rgb_triple_store(0,0,0);
              DAT_1f800001 = 0xf0;
            }
            else {
              if (fade_ctr < 0x32) {
                gray = 0xff - (fade_ctr * 0xff) / 0x32;
LAB_80165ed0:
                spr_attr_alloc_a(_DAT_1f800008,0,0,(int)(short)_DAT_1f800020,_DAT_1f800022,
                             gray | gray << 8 | gray << 0x10,3);
              }
              else if ((0x95 < fade_ctr) && (fade_ctr < 200)) {
                gray = fade_acc / 0x32;
                goto LAB_80165ed0;
              }
              if (mode == 3) {
                rgb_triple_store(0xff,0xff,0xff);
                spr_attr_alloc_b(_DAT_1f800008,0,0xeffe008c,0x7d000000,
                             (((int)((uint)_DAT_1f800020 << 0x10) >> 0x10) -
                             ((int)((uint)_DAT_1f800020 << 0x10) >> 0x1f)) >> 1,_DAT_1f800022);
                spr_code = 0xeffe008a;
              }
              else {
                if (mode != 0xf) {
                  spr_attr_alloc_b(_DAT_1f800008,0,0xeffe008c,0x7d000000,(int)(short)_DAT_1f800020,
                               _DAT_1f800022);
                  goto LAB_80166058;
                }
                spr_attr_alloc_b(_DAT_1f800008,0,0xeffe000c,0x7d000000,
                             (((int)((uint)_DAT_1f800020 << 0x10) >> 0x10) -
                             ((int)((uint)_DAT_1f800020 << 0x10) >> 0x1f)) >> 1,_DAT_1f800022);
                spr_code = 0xeffe000d;
              }
              val = (int)((uint)_DAT_1f800020 << 0x10) >> 0x10;
              spr_attr_alloc_b(_DAT_1f800008,
                           (val - ((int)((uint)_DAT_1f800020 << 0x10) >> 0x1f)) >> 1 & 0xffff,spr_code,
                           0x7d000000,val,_DAT_1f800022);
            }
LAB_80166058:
            frame_tick(0x1f800000);
            if ((mode == 0) && (DAT_1f800001 == 1)) {
              save_slot_state_fixup(0x1f800000,2);
            }
          }
          else {
            if (DAT_1f800001 < 2) {
              if (((DAT_1f800001 == 0) && (res = card_sprite_load_sm(0x1f800000,mode), res != 0)) &&
                 (res = frame_divisor_get(), res == 0)) {
                fade_acc = 0x31ce;
                spr_attr_alloc_a(_DAT_1f800008,0,0,(int)(short)_DAT_1f800020,_DAT_1f800022,0,0);
                fade_ctr = 200;
                DAT_1f800001 = DAT_1f800001 + 1;
                goto LAB_80165dd8;
              }
              goto LAB_80166058;
            }
            if (DAT_1f800001 == 0xf0) {
              deferred_cmd_post(0x204,0,0);
            }
            else if (DAT_1f800001 != 0xf1) goto LAB_80166058;
            DAT_1f800001 = 0xff;
          }
        } while (DAT_1f800001 != 0xff);
      }
      DAT_1f800001 = 0;
    }
    else {
      g_game_state = 5;
      DAT_1f800001 = 0;
    }
  }
  return;
}

/* ======== 0x80167330 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void battle_intro_flow(int mode)

{
  bool is_attract;
  undefined1 saved_sub;
  ushort field_attr;
  int res, rnd;
  uint flags;
  char round_step;
  /* The generated code keeps the phase-2 intro countdown in $s3 across the
   * per-frame calls to this state machine; it must persist here or the
   * countdown restarts from stack garbage every frame. */
  static int intro_timer;
  undefined4 field_id;
  is_attract = false;
  res = memcard_err_sentinel();
  saved_sub = DAT_1f80009b;
  if (res < 0) {
    DAT_1f80009b = 3;
joined_r0x801673ac:
    /* Bail-unwind exit must preserve the DAT1 exit invariant (normal exits
     * write DAT_1f800001 = 0); see battle_flow_state_machine / results_
     * scoreboard for the full rationale. */
    {
      if (br2_bail_pending()) { DAT_1f800001 = 0; return; } }
    if (DAT_1f800001 != 0xff) {
      if (DAT_1f800001 == 2) {
LAB_80167620:
        intro_timer = intro_timer + -1;
        if (intro_timer == 0) {
          DAT_1f800001 = 0xf0;
        }
        if (_DAT_1f80008c == 0) {
LAB_8016767c:
          _DAT_1f80008c = _DAT_1f80008c + 1;
        }
        else if (_DAT_1f80008c == 1) {
          clear_f0300_records();
          _DAT_1f800148 = 0;
          _DAT_1f800250 = 0;
          goto LAB_8016767c;
        }
        if (_DAT_1f80008e == 0) {
          _DAT_1f80008e = 200;
          flags = table_byte_next_a();
          field_id = (flags & 1) ? 0x20 : 3;
          battle_field_set(_DAT_1f8000bc,field_id);
          field_attr = table_byte_next_a();
          *(ushort *)guest_at(_DAT_1f8000bc + 0x7e) = field_attr & 7;
        }
        _DAT_1f80008e = _DAT_1f80008e + -1;
        if ((DAT_1f8000e8 != '\x01') || (DAT_1f8001f0 != '\x01')) {
          DAT_1f800001 = 0xf0;
        }
        if (is_attract) {
          battle_frame_tick(0x1f800000,0x1ff);
        }
        else {
          battle_frame_tick(0x1f800000,0x1fb);
        }
      }
      else if (DAT_1f800001 < 3) {
        if (DAT_1f800001 == 0) {
          DAT_1f8000ec = 0;
          DAT_1f8001f4 = 0;
          _DAT_1f8000c4 = guest_of(_g_system_config_ptr + 0x2c);
          if (mode == 0) {
            rnd = table_byte_next_a();
            DAT_1f8000eb = (char)rnd + (char)(rnd / 9) * -9;
            rnd = table_byte_next_a();
            DAT_1f8001f3 = (char)rnd + (char)(rnd / 9) * -9;
            rnd = table_byte_next_a();
            round_step = (char)(rnd / 5) * '\x05';
          }
          else {
            is_attract = true;
            rnd = table_byte_next_a();
            DAT_1f8000eb = (char)rnd + (char)(rnd / 9) * -9;
            rnd = table_byte_next_a();
            DAT_1f8001f3 = (char)rnd + (char)(rnd / 9) * -9;
            rnd = table_byte_next_a();
            round_step = (char)(rnd / 6) * '\x06';
          }
          DAT_1f800098 = (char)rnd - round_step;
          DAT_1f800099 = *(char *)(_g_system_config_ptr + 4) * '\x02' + -2;
          fighter_spawn_tail((uint8_t *)guest_at(0x1f800000),0);
          _DAT_1f800088 = 0;
          _DAT_1f80008c = 0;
          _DAT_1f80008e = 0;
          _DAT_1f800090 = 0x1e;
          DAT_1f800001 = DAT_1f800001 + 1;
        }
        else if (DAT_1f800001 != 1) goto LAB_801677b4;
        if (_DAT_1f800090 != 0) {
          _DAT_1f800090 = 0;
        }
        res = menu_res_load_sm(0x1f800000,&DAT_1f8000e8,&DAT_1f8001f0,0);
        /* Same -1 ambiguity as battle_flow_state_machine: a br2_bail_pending
         * unwind must propagate, not be read as "load done". Restore the
         * DAT1 exit invariant (normal exits leave DAT_1f800001 = 0) so the
         * caller's gs->5 handoff can't leak a private substate to
         * match_loop. */
        {
          if (br2_bail_pending()) { DAT_1f800001 = 0; return; } }
        if ((res != 0) && (_DAT_1f800090 == 0)) {
          if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            confirm_code_select(0x1f800000);
          }
          intro_timer = 9000;
          se_value_select(DAT_1f800098);
          DAT_1f8000e9 = 0x11;
          DAT_1f8001f1 = 0x12;
          battle_round_init(0x1f800000,0);
          _DAT_1f80008c = 0;
          _DAT_1f80008e = 0;
          if (mode == 0) {
            intro_timer = 0x5dc;
          }
          battle_session_arm(0x1f800000,0xffffffff);
          DAT_1f800001 = DAT_1f800001 + 1;
          goto LAB_80167620;
        }
      }
      else {
        if (DAT_1f800001 == 0xf0) {
          frame_present(0x1f800000);
          hud_sprite_desc_init();
          blk_1f26c0_init();
          deferred_cmd_post(0x204,0,0);
          scroll_cfg_set(1,8);
          /* Same bound-pointer register clobber as frame_divisor_get: the
           * cached &g_DAT_1f800010 register goes stale across the calls
           * above, so access the scratchpad byte directly. */
          *(uint8_t *)guest_at(0x1f800010) = 0;
          DAT_1f80009e = 0;
          rgb_triple_store(0,0,0);
          DAT_1f800001 = DAT_1f800001 + 1;
        }
        else if (DAT_1f800001 != 0xf1) goto LAB_801677b4;
        res = frame_divisor_get();
        if (res == 0) {
          /* The hoisted 0xff constant lives in callee-saved x28 across the
           * whole do-while; the IRQ setjmp/longjmp path restores x28 stale
           * (g_irqctx_ring base), so even a fresh-pointer store emitted
           * strb w28. br2_st8 materializes the value in the arg register
           * at the call — immune to the stale callee-saved restore. */
          br2_st8(0x1f800001, 0xff);
          goto joined_r0x801673ac;
        }
      }
LAB_801677b4:
      frame_tick(0x1f800000);
      round_banner_draw(0x1f800000,1);
      if (DAT_1f800001 == 2) {
        save_slot_state_fixup(0x1f800000,0);
      }
      goto joined_r0x801673ac;
    }
  }
  else {
    g_game_state = 5;
  }
  DAT_1f800001 = 0;
  DAT_1f80009b = saved_sub;
  return;
}

/* ======== 0x801689a8 ======== */

void cd_subsys_boot(void)

{
  cd_subsys_reset_wrap(0);
  cd_chan_block_boot();
  return;
}

/* ======== 0x801594a4 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void display_frame_present(int *env,int ticks)

{

  int w;

  gpu_draw_sync(0);
  memcard_tick();
  deferred_cmd_drain();
  memcard_slot_poll();
  int tticks = 0;
  if (-1 < ticks) {
    tticks = ticks;
  }
  cd_timer_read(tticks);
  int ev3;
  if (*env == 0) {
    ev3 = env[3];
  }
  else {
    ev3 = (env[3] * 3) / 2;
  }
  if (*env == 0) {
    w = env[5];
  }
  else {
    w = (env[5] * 3) / 2;
  }
  sprite_attr_init_b(_DAT_1f800004 + 0x5c,*(short *)guest_at((uint32_t)(_DAT_1f80000c * 8 + -0x7fe0b8e8)) - ev3,
               (int)*(short *)guest_at((uint32_t)(_DAT_1f80000c * 8 + -0x7fe0b8e6)) - env[4],w,env[6]);
  *(undefined1 *)guest_at(_DAT_1f800004 + 0x6e) = 1;
  *(undefined2 *)guest_at(_DAT_1f800004 + 0x66) = 0x18;
  if (*env != 0) {
    *(char *)guest_at(_DAT_1f800004 + 0x6d) = (char)*env;
    int f60 = (int)*(short *)guest_at(_DAT_1f800004 + 0x60) << 1;
    *(short *)guest_at(_DAT_1f800004 + 0x60) =
         (short)((ulonglong)((longlong)f60 * 0x55555556) >> 0x20) - (short)(f60 >> 0x1f);
  }
  int ev5;
  if (*env == 0) {
    ev5 = env[5];
  }
  else {
    ev5 = (env[5] * 3) / 2;
  }
  sprite_attr_init_a(_DAT_1f800004,(int)*(short *)guest_at((uint32_t)(_DAT_1f80000c * 8 + -0x7fe0b8e8)),
               (int)*(short *)guest_at((uint32_t)(_DAT_1f80000c * 8 + -0x7fe0b8e6)),ev5,env[6]);
  draw_env_timing(_DAT_1f800004 + 0x5c);
  gpu_draw_env_put(_DAT_1f800004);
  gpu_draw_otag((int)_DAT_1f800008 + 0x24);
  gpu_disp_mask_set(1);
  _DAT_1f80000c = _DAT_1f80000c ^ 1;
  _DAT_1f800004 = _DAT_1f80000c * 0x2150 + -0x7fe1bf60;
  _DAT_1f800008 = (uint32_t)(_DAT_1f80000c * 0x2150 + 0x801e4110);
  _DAT_1f8000dc = _DAT_1f8000dc + 1;
  gpu_clear_otag_r(_DAT_1f800008,10);
  *(uint32_t *)guest_at(_DAT_1f800008) = *(uint32_t *)guest_at(_DAT_1f800008) | 0xffffff;
  pad_state_refresh((intptr_t)guest_at(0x1f800000));
  return;
}

/* ======== 0x80159734 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void tile_stream_job_init(undefined4 *job,undefined2 w0,undefined2 w1,undefined2 w2,
                 undefined2 w3,int *ctrl)

{
  undefined4 hdr0;
  undefined4 hdr1;
  undefined4 hdr2;
  undefined4 hdr3;
  undefined2 flg;

  job = (undefined4 *)guest_at((uint32_t)(uintptr_t)job);
  hdr3 = _DAT_801f4750;
  hdr2 = _DAT_801f474c;
  hdr1 = _DAT_801f4748;
  hdr0 = _DAT_801f4744;
  if (DAT_801b3f64 == 1) {
    job[2] = 0;
    job[5] = 0;
    job[10] = 0;
    job[0xd] = 0;
    DAT_801b3f64 = 0;
    *job = hdr0;
    job[1] = hdr1;
    job[3] = hdr2;
    job[4] = hdr3;
  }
  *(undefined2 *)(job + 6) = w0;
  *(undefined2 *)((char *)job + 0x1a) = w1;
  *(undefined2 *)(job + 8) = w2;
  *(undefined2 *)((char *)job + 0x22) = w3;
  flg = 0x10;
  if (*ctrl != 0) {
    flg = 0x18;
  }
  *(undefined2 *)(job + 0xc) = flg;
  job[0xe] = *ctrl;
  if (job[10] == 0) {
    *(undefined2 *)(job + 0xb) = w0;
    *(undefined2 *)((char *)job + 0x2e) = w1;
    return;
  }
  *(undefined2 *)(job + 0xb) = w2;
  *(undefined2 *)((char *)job + 0x2e) = w3;
  return;
}

/* ======== 0x801597ec ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void tile_stream_open(uint8_t *stream,void *name,int *params)

{
  cd_fileop_reset(0);
  cd_fileop_dispatch(br2_gaddr(name));
  cd_dir_table_set(_DAT_801f4754,0x20);
  cd_read_params_store(*params,params[1],0xffffffff,0,0);
  cd_stream_mode_init(guest_of(stream));
  return;
}

/* ======== 0x80159864 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void tile_stream_pump(void)

{
  short step;
  int buf_sel;
  int blk;
  undefined4 off_x;
  undefined4 off_y;
  g_cdfn_log[13]++;   /* CDFN_TILE_STREAM_PUMP */

  if ((_DAT_801f4738 != 0) && (_DAT_801fc68c != 0)) {
    cd_mc_state_machine();
    _DAT_801fc68c = 0;
  }
  buf_sel = _DAT_801f4714;
  off_x = _DAT_801f472c;
  off_y = _DAT_801f4730;
  _DAT_801f4714 = (uint)(_DAT_801f4714 == 0);
  step = _DAT_801f4730;
  if ((_DAT_801f473c != 0) &&
     (blk = (int)*(short *)guest_at((uint32_t)(_DAT_801f4728 * 8 + -0x7fe0b8e4)) % _DAT_801f4730, blk != 0)) {
    _DAT_801f473c = 0;
    step = (short)blk;
  }
  _DAT_801f472c = _DAT_801f472c + step;
  if (_DAT_801f472c <
      (int)*(short *)guest_at((uint32_t)(_DAT_801f4728 * 8 + -0x7fe0b8e8)) +
      (int)*(short *)guest_at((uint32_t)(_DAT_801f4728 * 8 + -0x7fe0b8e4))) {
    blk = _DAT_801f4732 + -1;
    if (blk < 0) {
      blk = _DAT_801f4732 + 0xe;
    }
    cd_fileop_forward(*(undefined4 *)guest_at((uint32_t)(_DAT_801f4714 * 4 + -0x7fe0b8f4)),
                 _DAT_801f4730 * 0x10 * ((blk >> 4) + 1) >> 1);
  }
  else {
    _DAT_801f4728 = (uint)(_DAT_801f4728 == 0);
    _DAT_801f4734 = 1;
    _DAT_801f472c = *(undefined4 *)guest_at((uint32_t)(_DAT_801f4728 * 8 + -0x7fe0b8e8));
    _DAT_801f473c = 1;
  }
  { uint32_t rect[2] = { off_x, off_y };
    /* guest rect lives at $sp+0x10 (frame -0x28) = entry_sp-0x18;
     * gpu_load_image_cmd reads it through nested frames, so stage it in
     * a reserved guest frame (sp+0x10 -> frm+0x18). */
    uint32_t tsp_frm = br2_guest_frame(0x28);
    uint32_t rect_g = tsp_frm + 0x18;
    memcpy(guest_at(rect_g), rect, 8);
    gpu_load_image_cmd(rect_g,*(undefined4 *)guest_at((uint32_t)(buf_sel * 4 + -0x7fe0b8f4)));
    br2_guest_frame_free(); }
  return;
}

/* ======== 0x80159c24 ======== */

void tile_decode_wait(intptr_t job)

{
  uint next_buf;
  int done;
  int timeout;

  job = (intptr_t)guest_at((uint32_t)job);
  done = *(int *)(job + 0x34);
  timeout = 0x800000;
  while (done == 0) {
    timeout = timeout + -1;
    if (timeout == 0) {
      printf_forward(s_time_out_in_decoding___80120e90);
      *(undefined4 *)(job + 0x34) = 1;
      next_buf = (uint)(*(int *)(job + 0x28) == 0);
      *(uint *)(job + 0x28) = next_buf;
      *(undefined2 *)(job + 0x2c) = *(undefined2 *)(job + next_buf * 8 + 0x18);
      *(undefined2 *)(job + 0x2e) =
           *(undefined2 *)(job + *(int *)(job + 0x28) * 8 + 0x1a);
    }
    done = *(int *)(job + 0x34);
  }
  *(undefined4 *)(job + 0x34) = 0;
  return;
}

/* ======== 0x80159ce4 ======== */

void cd_stream_mode_init(undefined4 cfg_g)

{
  int st;
  undefined1 arg_buf [8];
  uint32_t cfg_w0 = *(uint32_t *)guest_at(cfg_g);
  g_cdfn_log[14]++;   /* CDFN_STREAM_MODE_INIT */
  cd_seek_note(4, cfg_w0, (uint32_t)cd_stamp_to_sector(guest_at(cfg_g)));

  arg_buf[0] = 0x80;
  do {
    do {
      st = cd_mode_flag_set(2,cfg_g,0);
    } while (st == 0);
    do {
      st = cd_mode_flag_set(0xe,arg_buf,0);
    } while (st == 0);
    if (DAT_801c4580 == '\0') {
      cd_timer_read(3);
    }
    DAT_801c4580 = '\x01';
    st = cd_mode_set_0e(0x1e0);
  } while (st == 0);
  return;
}

/* ======== 0x80163154 ======== */

undefined4 stream_block_decode(undefined4 dst,int *tbl,int idx)

{
  if (idx < *tbl) {
    if (tbl[idx + 1] != -1) {
      /* tbl is a host pointer into guest RAM (callers pass guest_at()).
       * The original MIPS added the table's GUEST address; (int)tbl
       * truncated the host pointer instead, so mode3_dispatch received a
       * bogus a1 (~0x87xxxxxx), read the mode word from open bus, and
       * returned a garbage length that corrupted the heap cursor
       * (_DAT_801e3cfc / _DAT_801c47b8) -> wrong CD dest -> DMA into .text.
       * guest_of() recovers the guest address faithfully. */
      uint32_t srcg = guest_of(tbl) + (uint32_t)tbl[idx + 1] + (uint32_t)(*tbl + 1) * 4u;
      { uint32_t len = mode3_dispatch(dst,srcg);
        /* tag 11: record the RETURN length so cursor advances are fully
         * attributed (decode dst->dst+len). dst=heap cursor arg. */
        {
          decomp_trace_record(11,dst,srcg,0,len); }
        return len; }
    }
  }
  return 0;
}

/* ======== 0x80159dac ======== */
/* Leaf gauge/param update. a0 guest rec; a1=*(rec+0xc0). bound =
 * u8@(0x801b3f88 + lh@rec+0xa2 + u8@a1+7*16)*16. JT 0x80120ef0 on
 * u8@rec+0x9c: case2/early-exit -> store 0xff ret 0xff; case3 ->
 * lh@+0xa2*12 -> @+0xa0 (ret p if p<0x100 else 0xff path); default ->
 * clamp @+0xa0 into [bound-0x32, bound+0x1f]. Ends 0x80159ee4. */
int32_t gauge_leaf_update(uint32_t a0g)
{
  uint8_t *rec = (uint8_t *)guest_at(a0g);
  uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(rec + 0xc0));
  uint8_t *tb = (uint8_t *)guest_at(0x801b3f88);
  int32_t bound, gval, lim;
  uint32_t hv;
  bound = (int32_t)tb[(uint32_t)*(int16_t *)(rec + 0xa2) +
                     (uint32_t)cr[7] * 16] * 16;
  if (cr[0x21] != 0)
    goto set_ff;
  switch (rec[0x9c]) {
  case 2:
    goto set_ff;
  case 3: {
    int32_t gauge_v = (int32_t)*(int16_t *)(rec + 0xa2) * 12;
    *(int16_t *)(rec + 0xa0) = (int16_t)gauge_v;
    if (gauge_v < 0x100)
      return gauge_v;
    gval = 0xff;
    goto store_ff;
  }
  default:
    break;
  }
  hv = *(uint16_t *)(rec + 0xa0);
  gval = hv & 0xff;
  if ((hv & 0x100) != 0 || rec[0x99] != 0) {
    *(int16_t *)(rec + 0xa0) = (int16_t)gval;
    lim = bound - 0x32;
    if (gval < lim)
      *(int16_t *)(rec + 0xa0) = (int16_t)lim;
  } else {
    int32_t s = (int16_t)hv;
    if (s < bound)
      *(int16_t *)(rec + 0xa0) = (int16_t)bound;
  }
  gval = *(int16_t *)(rec + 0xa0);
  lim = bound + 0x1f;
  if (lim < gval)
    *(int16_t *)(rec + 0xa0) = (int16_t)lim;
  gval = *(int16_t *)(rec + 0xa0);
  if (gval < 0)
    *(int16_t *)(rec + 0xa0) = 0;
  gval = (*(int16_t *)(rec + 0xa0) < 0x100) ? 1 : 0;
  if (gval != 0)
    return gval;
set_ff:
  gval = 0xff;
store_ff:
  *(int16_t *)(rec + 0xa0) = (int16_t)gval;
  return gval;
}

/* ======== 0x801560f8 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 match_mode_set(int mode)

{
  if (-1 < mode) {
    _DAT_801f36c2 = (undefined2)mode;
  }
  return _DAT_801f36c2;
}

/* ======== 0x801566a0 ======== */

void fighter_record_apply(undefined4 base,int idx)

{
  fighter_rec_fetch_apply(base,idx,*(undefined4 *)(&DAT_801b3ebc + idx * 4));
  return;
}

/* ======== 0x801566d4 ======== */

void page_table_ptr_set(int bank,int slot,undefined *ptr)

{
  switch(slot) {
  case 0:
    if (bank == 0) {
      ptr = &DAT_801b3400;
    }
    else {
      if (bank != 1) goto LAB_801568ec;
      ptr = &DAT_801b33dc;
    }
    break;
  case 1:
    if (bank == 1) {
      ptr = &DAT_801b35b8;
    }
    else if (bank < 2) {
      if (bank != 0) goto LAB_801568ec;
      ptr = &DAT_801b346c;
    }
    else {
      if (bank != 2) goto LAB_801568ec;
      ptr = &DAT_801b3eb0;
    }
    break;
  case 2:
    switch(bank) {
    case 0:
      ptr = &DAT_801b37dc;
      break;
    case 1:
      ptr = &DAT_801b37f4;
      break;
    case 2:
      ptr = &DAT_801b3814;
      break;
    case 3:
      ptr = &DAT_801b3834;
      break;
    case 4:
      ptr = &DAT_801b3854;
      break;
    case 5:
      ptr = &DAT_801b3868;
      break;
    case 6:
      ptr = &DAT_801b388c;
      break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 0xc:
      ptr = &DAT_801b38ac;
      break;
    case 0xb:
    case 0x12:
      ptr = &DAT_801b3758;
      break;
    case 0x10:
      ptr = &DAT_801b35a0;
      break;
    case 0x11:
      ptr = &DAT_801b35cc;
      break;
    case 0x13:
      ptr = &DAT_801b377c;
    }
    break;
  case 3:
  case 4:
    switch(bank) {
    case 0:
      ptr = &DAT_801b38d0;
      break;
    case 1:
      ptr = &DAT_801b3968;
      break;
    case 2:
      ptr = &DAT_801b3a00;
      break;
    case 3:
      ptr = &DAT_801b3a84;
      break;
    case 4:
      ptr = &DAT_801b3b08;
      break;
    case 5:
      ptr = &DAT_801b3b94;
      break;
    case 6:
      ptr = &DAT_801b3c24;
      break;
    case 7:
      ptr = &DAT_801b3ca8;
      break;
    case 8:
      ptr = &DAT_801b3d34;
      break;
    case 9:
      ptr = &DAT_801b3db4;
      break;
    case 10:
      ptr = &DAT_801b3e44;
    }
  }
LAB_801568ec:
  *(uint32_t *)guest_at((uint32_t)(slot * 4 + -0x7fe0ca20)) = guest_of(ptr);
  return;
}

/* ======== 0x80155f64 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void boot_globals_init(void)

{
  int *chan_mask;
  undefined1 *chan_flag;
  int i;
  int mask;

  cd_chan_boot_fwd();
  mc_init_reset();
  mc_mode_nibble_rw(2);
  mc_dialog_bit4(0);
  gauge_scale_fixed(100,100);
  _DAT_801f2650 = 0xffffff;
  _DAT_801f2654 = 0x71f;
  _DAT_801f2674 = 5;
  _DAT_801f265a = 0;
  _DAT_801f2658 = 0;
  _DAT_801f265e = 0;
  _DAT_801f265c = 0;
  _DAT_801f2664 = 0;
  _DAT_801f2678 = 7;
  _DAT_801f267c = 7;
  cd_chan_attr_push();
  blk_1f26c0_init_b();
  match_mode_set(0);
  deferred_cmd_reset();
  brightness_commit(0);
  chan_flag = &DAT_801f24d0;
  mask = 1;
  i = 0;
  chan_mask = (int *)&DAT_801f24d8;
  do {
    *chan_flag = 0;
    *chan_mask = mask;
    mask = mask << 1;
    *(undefined1 *)((char *)chan_mask + -7) = 0;
    chan_mask = chan_mask + 4;
    i = i + 1;
    chan_flag = chan_flag + 0x10;
  } while (i < 0x18);
  return;
}

/* ======== 0x80146e38 ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 attract_stage_load_step(int state_g,int *out)

{
  undefined4 heap;
  int st;
  short phase;
  /* state_g is a GUEST base (0x1f800000 scratchpad state block), not a host
   * pointer — dereference via guest_at() or the raw 0x1f80008c access faults. */
  short *st8c = (short *)guest_at((uint32_t)state_g + 0x8cu);

  phase = *st8c;
  if (phase < 0) {
    return 0xffffffff;
  }
  if (phase != 0x10) {
    if (0x10 < phase) {
      if (phase == 0x20) {
        st = cd_file_load_sm(0x90,_DAT_801e3cfc);
        if (st != 0) {
          st = stream_block_decode(_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),2);
          heap = _DAT_801e3cfc;
          st = _DAT_801c47b8 + st;
          out[2] = _DAT_801c47b8;
          _DAT_801c47b8 = st;
          st = stream_block_decode(st,(int *)guest_at(heap),3);
          phase = 0x30;
          out[3] = _DAT_801c47b8;
          _DAT_801c47b8 = _DAT_801c47b8 + st;
        }
      }
      else {
        if (phase != 0x30) {
          *st8c = phase;
          return 0;
        }
        st = cd_file_load_sm(DAT_801b2350 + 0x91,_DAT_801e3cfc);
        if (st != 0) {
          st = stream_block_decode(_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),0);
          *out = _DAT_801c47b8;
          _DAT_801c47b8 = _DAT_801c47b8 + st;
          st = stream_block_decode(_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),1);
          out[1] = _DAT_801c47b8;
          _DAT_801c47b8 = _DAT_801c47b8 + st;
          *(undefined2 *)st8c = 0xffff;
          return 1;
        }
      }
      goto LAB_8014700c;
    }
    if (phase != 0) {
      *st8c = phase;
      return 0;
    }
    cd_file_load_sm(0xffffffff,0);
    _DAT_801c43a0 = 0;
    DAT_801e3d00 = 0xff;
    _DAT_801c47b8 = -0x7fff0000;
    _DAT_801e3cfc = 0x800dc800;
    DAT_801b2350 = DAT_801b2350 + 1;
    phase = 0x10;
    if (9 < DAT_801b2350) {
      DAT_801b2350 = 0;
    }
  }
  st = cd_file_load_sm(0x8a,_DAT_801c47b8);
  if (st != 0) {
    fighter_record_apply(_DAT_801c47b8,2);
    page_table_ptr_set(0x10,2,(undefined *)0);
    *(undefined2 *)st8c = 0x20;
    return 0;
  }
LAB_8014700c:
  *st8c = phase;
  return 0;
}

/* ======== 0x8014751c ======== */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 attract_menu_dispatch(intptr_t ctx,uint btn)

{
  int st;

  btn = btn & 0xffff;
  if (btn == 6) {
    if ((_DAT_1f80007c & 0x840) == 0) {
      return 0;
    }
    deferred_cmd_post(0x400,0xc,0);
    g_sub_state = 4;
  }
  else {
    if (btn < 7) {
      if (btn == 3) {
        DAT_1f80009b = 1;
        DAT_1f80009c = 4;
        st = mc_pad_button_handler((uint8_t *)guest_at(0x1f800000));
        if (st != 0) {
          g_game_state = 8;
          DAT_1f800001 = 0xf0;
          return 0;
        }
        return 0;
      }
      if (3 < btn) {
        if (btn == 4) {
          *(undefined1 *)(_g_system_config_ptr + 0x10) = 99;
          DAT_1f80009b = 0;
          DAT_1f80009c = 3;
          st = mc_pad_button_handler((uint8_t *)guest_at(0x1f800000));
          if (st != 0) {
            g_game_state = 8;
            DAT_1f800001 = 0xf0;
            return 0;
          }
          return 0;
        }
        if (btn == 5) {
          DAT_1f80009b = 3;
          DAT_1f80009c = 2;
          st = mc_pad_button_handler((uint8_t *)guest_at(0x1f800000));
          if (st != 0) {
            g_game_state = 8;
            DAT_1f800001 = 0xf0;
            return 0;
          }
          return 0;
        }
        return 1;
      }
      if (btn == 1) {
        *(undefined1 *)(_g_system_config_ptr + 0x10) = 8;
        DAT_1f80009b = 0;
        DAT_1f80009c = 0;
        st = mc_pad_button_handler((uint8_t *)guest_at(0x1f800000));
        if (st != 0) {
          g_game_state = 8;
          DAT_1f800001 = 0xf0;
          return 0;
        }
        return 0;
      }
      if (btn == 2) {
        DAT_1f80009b = 2;
        DAT_1f80009c = 1;
        st = mc_pad_button_handler((uint8_t *)guest_at(0x1f800000));
        if (st != 0) {
          g_game_state = 8;
          DAT_1f800001 = 0xf0;
          return 0;
        }
        return 0;
      }
      return 1;
    }
    if (0x107 < btn) {
      if (btn != 0x1ff) {
        return 1;
      }
      return 0;
    }
    if (0x100 < btn) {
      if ((uint)*(ushort *)(ctx + 8) != btn - 0x100) {
        return 0;
      }
      st = _DAT_1f800022 * 3;
      if (st < 0) {
        st = st + 3;
      }
      announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(char *)guest_at(((uint32_t *)guest_at(0x801b235c))[btn - 0x101]),
                   (int)((((((int)((uint)_DAT_1f800020 << 0x10) >> 0x10) -
                           ((int)((uint)_DAT_1f800020 << 0x10) >> 0x1f)) >> 1) -
                         (uint)(byte)(&DAT_801b2253)[btn]) * 0x10000) >> 0x10,
                   (st << 0xe) >> 0x10,0);
      return 0;
    }
    if (btn != 7) {
      if (btn != 0x100) {
        return 1;
      }
      st = _DAT_1f800022 * 3;
      if (st < 0) {
        st = st + 3;
      }
      spr_slot_pkt_submit((uint32_t *)(uintptr_t)(_DAT_1f800008 + 4),
                   ((st >> 2) + -0x10) * 0x10000 |
                   ((((int)((uint)_DAT_1f800020 << 0x10) >> 0x10) -
                    ((int)((uint)_DAT_1f800020 << 0x10) >> 0x1f)) >> 1) - 8U & 0xffff,0xf0f001e,
                   0x7816e800);
      st = _DAT_1f800022 * 3;
      if (st < 0) {
        st = st + 3;
      }
      spr_slot_pkt_submit((uint32_t *)(uintptr_t)(_DAT_1f800008 + 4),
                   ((st >> 2) + 0x10) * 0x10000 |
                   ((((int)((uint)_DAT_1f800020 << 0x10) >> 0x10) -
                    ((int)((uint)_DAT_1f800020 << 0x10) >> 0x1f)) >> 1) - 8U & 0xffff,0xf0f001e,
                   0x7816e810);
      return 0;
    }
    if ((_DAT_1f80007c & 0x840) == 0) {
      return 0;
    }
    deferred_cmd_post(0x400,0xc,0);
    g_sub_state = 3;
  }
  DAT_1f800001 = 0xf0;
  g_game_state = 0xb;
  return 0;
}

/* ======== 0x8012a72c ======== */

uint pad_state_refresh(intptr_t pad)

{
  char cnt;
  uint btns;
  intptr_t btns_b;
  intptr_t cur_w;
  uint pressed;
  intptr_t rec;
  int i;
  uint btns_old;

  if ((DAT_801befa8 != '\0') || (btns = attr_word_decode(&DAT_801befa8), (int)btns < 0)) {
    btns = 0;
  }
  if ((DAT_801befca != '\0') || (btns_b = attr_word_decode(&DAT_801befca), btns_b < 0)) {
    btns_b = 0;
  }
  i = 0;
  btns = btns_b << 0x10 | btns;
  btns_old = *(uint *)(pad + 0x6c);
  *(uint *)(pad + 0x6c) = btns;
  pressed = (btns ^ btns_old) & btns;
  *(uint *)(pad + 0x70) = btns_old;
  *(uint *)(pad + 0x68) = pressed;
  *(uint *)(pad + 0x74) = pressed;
  cur_w = pad;
  rec = pad;
  do {
    if ((*(short *)(cur_w + 0x6c) == 0) || (*(short *)(cur_w + 0x6c) != *(short *)(cur_w + 0x70))) {
      *(undefined1 *)(rec + 0x78) = 0x10;
    }
    else {
      cnt = *(char *)(rec + 0x78) + -1;
      *(char *)(rec + 0x78) = cnt;
      if (cnt == '\0') {
        *(undefined2 *)(cur_w + 0x74) = *(undefined2 *)(cur_w + 0x6c);
        *(undefined1 *)(rec + 0x78) = 2;
      }
    }
    rec = rec + 1;
    i = i + 1;
    cur_w = cur_w + 2;
  } while (i < 2);
  *(ushort *)(pad + 0x80) = *(ushort *)(pad + 0x6c) | *(ushort *)(pad + 0x6e);
  *(ushort *)(pad + 0x7c) = *(ushort *)(pad + 0x68) | *(ushort *)(pad + 0x6a);
  *(ushort *)(pad + 0x7e) = *(ushort *)(pad + 0x74) | *(ushort *)(pad + 0x76);
  pad_slot_state_walk();
  return btns;
}


/* ======== 0x8012aaa8 ======== */

undefined1 frame_divisor_get(void)

{
  /* Read the scratchpad byte directly.  The g_DAT_1f800019 bound-pointer
   * indirection miscompiled inside battle_intro_flow's inlined copy (the
   * cached &g_DAT_1f800019 register held a wrong address -> wild deref). */
  return *(uint8_t *)guest_at(0x1f800019);
}

/* ---- batch30: state-machine stepper + scroll stripes ---- */
extern void *fn_lookup(uint32_t addr);
extern uint32_t call_guest(uint32_t,uint32_t,uint32_t);
#define MB8(a)   (*(uint8_t  *)guest_at(a))
#define MB16(a)  (*(uint16_t *)guest_at(a))
#define MB32(a)  (*(uint32_t *)guest_at(a))
#define MB32S(a) (*(int32_t   *)guest_at(a))
extern void menu_stepper_input(void *);
extern void menu_stepper_redraw(void *);
extern char **PTR_s_NAME_ENTRY_801b13a8;
extern char *PTR_s_MOVIES_AND_PICTURES_801b13c4;

/* menu_stepper_state_advance — generic 3-state stepper: byte+2 state (0->1),
 * byte+3 gate, callback at +0xc invoked once, then menu_stepper_redraw. */
void menu_stepper_state_advance(void *desc)
{
  uint8_t *p = (uint8_t *)desc;
  uint8_t state = p[2];
  if (state != 1) {
    if (1 < state) return;
    if (state != 0) return;
    p[2] = 1;
  }
  if (*(char *)(p + 3) == '\0') {
    menu_stepper_input(desc);
    /* desc+0xc is a guest code address (e.g. 0x8015b190) — dispatch
     * through the fn map; arg = desc ptr, second arg = u16 @+8.
     * Guest jalr is unconditional: a0 = desc guest addr, a1 = lhu @+8. */
    uint32_t cb_g = *(uint32_t *)(p + 0xc);
    void *cb = fn_lookup(cb_g);
    if (cb != NULL)
      ((void (*)(void *,uint16_t))cb)(desc,*(uint16_t *)(p + 8));
    else
      call_guest(cb_g,guest_of(desc),(uint32_t)*(uint16_t *)(p + 8));
  }
  menu_stepper_redraw(desc);
}

/* banner_stripe_scroll — scrolling banner stripes: walks DAT_801e8508/8518
 * position pair, emits announce_param_block_init announce lines per screen row. */
uint32_t banner_stripe_scroll(int32_t banner,int32_t chan,int32_t step,int16_t aux)
{
  int32_t stride = MB32S(0x801b1348 + banner * 4);
  int32_t row = MB32S(0x801e8508 + chan * 4) - step;
  MB32S(0x801e8508 + chan * 4) = (uint32_t)row;
  while (row < -stride) {
    row = MB32S(0x801e8508 + chan * 4) + stride;
    MB32S(0x801e8508 + chan * 4) = (uint32_t)row;
  }
  int32_t y = MB32S(0x801e8508 + chan * 4);
  uint32_t auxw = MB32(0x801e8518 + chan * 4);
  if (y < MB16(0x1f800020)) {
    int32_t y_q16 = y << 0x10;
    do {
      char *str; int32_t dst;
      if (banner == 7) {
        dst = (int32_t)MB32(0x1f800008) + 4;
        str = PTR_s_MOVIES_AND_PICTURES_801b13c4;
      } else {
        str = PTR_s_NAME_ENTRY_801b13a8[banner];
        dst = (int32_t)MB32(0x1f800008);
      }
      y = y + stride;
      announce_param_block_init((uint32_t)dst,MB32(0x1f80000c),
                   str,(uint32_t)(y_q16 >> 0x10),
                   (int32_t)(int16_t)auxw,(int32_t)aux);
      y_q16 = y_q16 + stride * 0x10000;
    } while (y < MB16(0x1f800020));
  }
  return MB32(0x801e8508 + chan * 4);
}

/* cursor_sprite_anim — cursor/sprite anim state machine: byte+0x9e selects
 * sprite variant via spr_attr_alloc_a, byte+0x17 phase counter drives
 * dialog_mode_mirror/frame_counter_bump transitions. param_1 = host ptr. */
extern void dialog_mode_mirror(int32_t);
extern void frame_counter_bump(int32_t);
void cursor_sprite_anim(uint8_t *st)
{
  char phase;
  uint8_t variant;
  uint8_t gray;
  int32_t y, x;
  uint32_t sel;
  variant = st[0x9e];
  if (variant == 1) {
    dialog_mode_mirror(0);
    x = *(int16_t *)(st + 0x20);
    y = *(int16_t *)(st + 0x22);
    sel = 3;
LAB_8012a92c:
    spr_attr_alloc_a(*(uint32_t *)(st + 8),0,0,x,y,0x101010,sel);
  } else if (variant < 2) {
    sel = 1;
    if (variant == 0) goto LAB_8012a944;
  } else {
    if (variant == 2) {
      dialog_mode_mirror(0);
      x = *(int16_t *)(st + 0x20);
      y = *(int16_t *)(st + 0x22);
      sel = 2;
      goto LAB_8012a92c;
    }
    sel = 0;
    if (variant == 3) goto LAB_8012a944;
  }
  goto LAB_8012a9a0;
LAB_8012a944:
  dialog_mode_mirror((int)sel);
LAB_8012a9a0:
  phase = *(char *)(st + 0x17);
  if (phase == -1) {
    st[0x17] = 0xfe;
    return;
  }
  if (phase < 0) {
    if (phase != -2) return;
    frame_counter_bump(1);
    st[0x17] = 0;
    return;
  }
  if (phase == 1) {
    if (st[0x19] == 0) goto LAB_8012aa28;
    st[0x19] = st[0x19] - 1;
    dialog_mode_mirror(0);
    x = *(int16_t *)(st + 0x20);
    y = *(int16_t *)(st + 0x22);
    gray = st[0x18];
    sel = 3;
  } else {
    if (phase != 2) return;
    if (st[0x19] == 0) {
LAB_8012aa28:
      st[0x17] = 0xff;
      dialog_mode_mirror(1);
      frame_counter_bump(0);
      return;
    }
    st[0x19] = st[0x19] - 1;
    dialog_mode_mirror(0);
    y = *(int16_t *)(st + 0x22);
    x = *(int16_t *)(st + 0x20);
    gray = st[0x18];
    sel = 2;
  }
  spr_attr_alloc_a(*(uint32_t *)(st + 8),0,0,x,y,
               ((uint32_t)gray << 16) | ((uint32_t)gray << 8) | gray,sel);
}

/* select_screen_tick — character/stage-select orchestrator. DAT_1f800001
 * state machine: 0 = attract/wait (intro movie via intro_movie_state,
 * fighter setup via battle_config_eval x2 + post_confirm_config_build), 6 = select
 * screen loop (cursor nav select_cursor_step, confirm select_confirm_handler,
 * stage preview banner_stripe_scroll, HUD hud_gauge_render), 0xf0/0xf1 = exit.
 * local_a8/local_94 = per-player 0x30-stride record views (zero-init
 * locals in guest; reads only). param_1 = entry mode (0/1/2 battle,
 * 3/4 others). */
extern void spr_draw_off(int32_t,int32_t,int32_t,int32_t);
extern void hud_text_d(uint32_t,uint32_t,const void *,int32_t,int32_t,int32_t);
extern int32_t select_cursor_step(int32_t,int32_t);

extern void fighter_spawn_setup(uint32_t,uint8_t *,uint8_t *,uint8_t *,int32_t);
extern void post_confirm_config_build(uint32_t,uint8_t *,int32_t);
extern int32_t battle_config_eval(uint8_t *,uint32_t,int32_t);
extern void sprite_9slice_build(uint32_t,uint32_t,uint32_t,uint32_t);
int32_t select_screen_tick(int32_t entry_mode)
{
  uint8_t next_state;
  int32_t draw_slot, pkv;
  uint32_t pidx_b;
  uint8_t *rec;
  uint32_t *spr_tab;
  /* attr_idx mirrors guest $s2: written=0/1/2 on the entry_mode paths and
   * reused as the sprite-attr temp in the state-6 block; when entry_mode is
   * out of range the guest passes the incoming (caller) $s2 to
   * intro_movie_state, so seed from the live gpr. */
  uint32_t attr_idx = (uint32_t)clean_cpu_state()->gpr[18];
  int32_t sel_cursor = 0, ret_code = 0;
  /* guest sp-0xf8..sp frame image (prologue @0x8014f260: addiu sp,-0xf8,
   * s7=sp+0x40, sp+0x18 -> cell 0x801f10bc). Reserve the window on the
   * live guest sp (br2_guest_frame mirrors it into cpu->gpr[29]) so every
   * nested guest call inside the select loop allocates BELOW this frame
   * instead of overwriting the two 0x30-byte player records (sp+0x40) and
   * the sp+0x18 param block — without the reservation each callee frame
   * landed in this window and the +0x10/+0x24 select-state fields were
   * clobbered, wedging versus select at state 6. The reservation aligns
   * up to 0x100; the offsets below keep the guest's absolute local
   * addresses (sp+0x18/+0x40/+0x50/+0x64). */
  uint32_t sst_frm = br2_guest_frame(0xf8);
  /* sp+0x18 -> sst_frm+0x20; guest RAM backing so the 0x801f10bc cell
   * carries a real guest address. */
  uint8_t *arg_blk = (uint8_t *)guest_at(sst_frm + 0x20);
  /* player_recs = two 0x30-byte player records at sp+0x40 (s7);
   * rec_f10/rec_f24 are Ghidra aliases into that same guest record
   * array (+0x10 / +0x24), not separate host storage. */
  uint8_t *player_recs = (uint8_t *)guest_at(sst_frm + 0x48);
  int32_t *rec_f10 = (int32_t *)guest_at(sst_frm + 0x58);
  int32_t *rec_f24 = (int32_t *)guest_at(sst_frm + 0x6c);
  int32_t tick_cnt = 0;
  int32_t rdy_p0, rdy_p1, tick_gate = 0;
  uint8_t *spad_cfg_a = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *spad_cfg_b = (uint8_t *)guest_at(0x1f8001f0);
  *(uint32_t *)guest_at(0x801f10bc) = guest_of(arg_blk);
  rec_f10[3] = 0;
  rec_f24[10] = 0;
  hud_sprite_desc_init();
  rgb_triple_store(0,0,0);
  flow_cfg_init(0);
  *(int16_t *)g__DAT_1f80008c = 0;
  DAT_1f800001 = 0;
  do {
    if (DAT_1f800001 == 6) {
      fighter_spawn_setup(0x1f800000,spad_cfg_a,spad_cfg_b,arg_blk,1);
      if (((*(uint32_t *)g__DAT_1f80007c & 0x10) == 0) ||
          ((*(uint8_t *)g_DAT_1f8000a2 != 0 && (entry_mode != 1)))) {
        uint32_t side = 0;
        rec = player_recs;
        do {
          int32_t st = *(int32_t *)(rec + 0x24), thr, ocur;
          if (st == 1) {
            if (*(int32_t *)(rec + 0x10) == 0) {
              if (*(int32_t *)(rec + 0x2c) == 0) {
                if (rec_f10[(side ^ 1) * 0xc] == 0) {
                  thr = rec_f24[(side ^ 1) * 0xc];
                  goto L_promote_chk;
                }
                if (rec_f10[(side ^ 1) * 0xc] == 1) {
                  *(uint32_t *)(rec + 0x24) = 2;
                }
              }
            }
            else if (*(int32_t *)(rec + 0x10) == 1) {
              thr = rec_f24[(side ^ 1) * 0xc];
L_promote_chk:
              if (0 < thr) *(uint32_t *)(rec + 0x24) = 2;
            }
          }
          else if (st < 2) {
            if (st == 0 && tick_gate != 0) {
              ocur = *(int32_t *)(rec + 0x1c);
              uint32_t cur;
              if ((*(uint32_t *)(rec + 8) & 0x1000) != 0) {
                cur = (uint32_t)select_cursor_step(ocur,0);
                *(uint32_t *)(rec + 0x1c) = cur;
              }
              if ((*(uint32_t *)(rec + 8) & 0x2000) != 0) {
                cur = (uint32_t)select_cursor_step(*(int32_t *)(rec + 0x1c),1);
                *(uint32_t *)(rec + 0x1c) = cur;
              }
              if ((*(uint32_t *)(rec + 8) & 0x4000) != 0) {
                cur = (uint32_t)select_cursor_step(*(int32_t *)(rec + 0x1c),2);
                *(uint32_t *)(rec + 0x1c) = cur;
              }
              if ((*(uint32_t *)(rec + 8) & 0x8000) != 0) {
                cur = (uint32_t)select_cursor_step(*(int32_t *)(rec + 0x1c),3);
                *(uint32_t *)(rec + 0x1c) = cur;
              }
              int32_t ccur = *(int32_t *)(rec + 0x1c);
              if (ccur < 0) { *(uint32_t *)(rec + 0x1c) = 0; ccur = 0; }
              if (10 < ccur) *(uint32_t *)(rec + 0x1c) = 10;
              if (*(int32_t *)(rec + 0x1c) != ocur) deferred_cmd_post(0x400,0x40,0);
              if (((*(uint32_t *)(rec + 8) & 0xe0) != 0) ||
                  (((*(uint32_t *)(rec + 8) & 0x800) != 0 &&
                    ((*(uint32_t *)(_g_system_config_ptr + 0x50) >>
                      (*(uint32_t *)(rec + 0x1c) & 0x1f) & 1) != 0)))) {
                select_confirm_handler(0x1f800000,side,rec,*(uint32_t *)(rec + 8),1);
                *(uint32_t *)(rec + 0x24) = 1;
                deferred_cmd_post(0x400,0x41,0);
                *(uint32_t *)(rec + 0x2c) = 8;
              }
            }
          }
          else if (st == 2 && rec_f24[0] == 2 && rec_f24[0xc] == 2) {
            DAT_1f800001 = 0xf0;
          }
          rec += 0x30;
          side++;
        } while ((int32_t)side < 2);
        if ((DAT_1f80009b != 0) && (entry_mode != 2)) {
          int32_t k = 0;
          rec = player_recs;
          do {
            if ((*(uint32_t *)(rec + 8) & 4) != 0) { deferred_cmd_post(0x400,0x40,0); sel_cursor--; }
            if ((*(uint32_t *)(rec + 8) & 8) != 0) { deferred_cmd_post(0x400,0x40,0); sel_cursor++; }
            if (9 < sel_cursor) sel_cursor = 0;
            if (sel_cursor < 0) sel_cursor = 9;
            k++;
            rec += 0x30;
          } while (k < 2);
          announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x80120688),0xd8,0x88,4);
          announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x80120690),0xd0,0x98,4);
          {
            uint32_t *ptbl = (uint32_t *)guest_at(0x801b3294);
            uint8_t *xoff = (uint8_t *)guest_at(0x801b32f4 - 0xb0); /* (&DAT_801b32f4)[sel_cursor-?] */
            announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(ptbl[sel_cursor]),
                         0x100 - (uint32_t)*(uint8_t *)((uintptr_t)xoff + sel_cursor),0xb0,3);
          }
          announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x80120698),200,200,4);
          announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x8012069c),0x110,200,4);
        }
        int32_t row_b = rec_f24[10];
        int32_t row_a = rec_f10[3];
        if (rec_f10[0] == 0) rdy_p0 = 1;
        else if ((DAT_1f80009b == 0 || DAT_1f80009c == 4) ||
                 ((rec_f24[7] != 0 || rec_f24[0xc] < 1) &&
                  (rec_f24[7] != 1 || rec_f24[0] < 0))) rdy_p0 = 0;
        else rdy_p0 = 2;
        if (rec_f24[7] == 0) rdy_p1 = 1;
        else {
          rdy_p1 = 0;
          if (DAT_1f80009b != 0 && DAT_1f80009c != 4)
            rdy_p1 = (uint32_t)(0 < rec_f24[0]) << 1;
        }
        {
          int16_t *t44 = (int16_t *)guest_at(0x801b3244);
          int16_t *t46 = (int16_t *)guest_at(0x801b3246);
          if (rdy_p0 != 0) {
            pkv = 0xe9;
            if (rdy_p0 == 1) pkv = 0xb9;
            sprite_9slice_build(_DAT_1f800008,
                         ((uint32_t)(t46[rec_f10[3] * 2] + 0xf) << 16) |
                         ((uint32_t)(t44[rec_f10[3] * 2] + 0x24) & 0xffff),0x151d000e,
                         (uint32_t)((pkv << 8) | 0x7adc00b1));
            pkv = 0x7adc;
            if (rec_f24[0] < 1) pkv = (tick_cnt + 0x1e5 & 0x1ff) * 0x40 + 0x1c;
            sprite_9slice_build(_DAT_1f800008 + 4,
                         ((uint32_t)(t46[row_a * 2] + 0x1f) << 16) |
                         ((uint32_t)(t44[row_a * 2] + 0x1c) & 0xffff),0x3e37000e,
                         ((uint32_t)pkv << 16) | 0xc040);
          }
          if (rdy_p1 != 0) {
            pkv = 0xe9;
            if (rdy_p1 == 1) pkv = 0xd1;
            sprite_9slice_build(_DAT_1f800008,
                         ((uint32_t)(t46[row_b * 2] + 0x31) << 16) |
                         ((uint32_t)(t44[row_b * 2] + 0x12) & 0xffff),0x151d000e,
                         (uint32_t)((pkv << 8) | 0x7adc00b1));
            pkv = 0x7adc;
            if (rec_f24[0xc] < 1) pkv = (tick_cnt + 0x1e5 & 0x1ff) * 0x40 + 0x1c;
            sprite_9slice_build(_DAT_1f800008 + 4,
                         ((uint32_t)(t46[row_b * 2] + 0x1f) << 16) |
                         ((uint32_t)(t44[row_b * 2] + 0x1c) & 0xffff),0x3e37000e,
                         ((uint32_t)pkv << 16) | 0xc078);
          }
          attr_idx = 0;
          spr_tab = (uint32_t *)guest_at(0x801b3244);
          uint32_t cfgfl = *(uint32_t *)(_g_system_config_ptr + 0x4c);
          do {
            if ((cfgfl & 1) != 0) {
              spr_slot_pkt_submit((uint *)(uintptr_t)(_DAT_1f800008 + 4),*spr_tab,0x3e37009d,
                           ((attr_idx + 0x1e0 & 0x1ff) << 0x16) |
                           ((((int32_t)attr_idx >> 2) * 0x40 + 1U & 0xff) << 8) |
                           ((attr_idx & 3) << 6));
            }
            cfgfl = (int32_t)cfgfl >> 1;
            attr_idx++;
            spr_tab++;
          } while ((int32_t)attr_idx < 0xb);
          {
            int32_t cx = (((int16_t)((uint16_t)_DAT_1f800020 << 16) >> 16) -
                         (((int32_t)((uint16_t)_DAT_1f800020 << 16)) >> 31)) >> 1;
            announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206a0),
                         ((((cx - 0x80) * 0x10000) >> 16)),0x28,0);
          }
          switch (DAT_1f80009c) {
          case 0: attr_idx = 10; break;
          case 1: attr_idx = 0xb; break;
          case 2: attr_idx = 0xe; break;
          case 3: attr_idx = 0xf; break;
          case 4: attr_idx = 0xc; break;
          case 5:
            if (DAT_1f80009b == 1) attr_idx = 0x15;
            else if (DAT_1f80009b < 2) { attr_idx = 0xd; if (DAT_1f80009b == 0) attr_idx = 0x16; }
            else if (DAT_1f80009b == 2) attr_idx = 0x14;
            else { attr_idx = 0xd; if (DAT_1f80009b == 4) attr_idx = 0x17; }
          }
          banner_stripe_scroll(attr_idx,0,4,7);
          uint8_t *t270 = (uint8_t *)guest_at(0x801b3270);
          uint8_t *t271 = (uint8_t *)guest_at(0x801b3271);
          uint8_t *t2f4 = (uint8_t *)guest_at(0x801b32f4);
          uint8_t *t300 = (uint8_t *)guest_at(0x801b3300);
          uint32_t *p47ec = (uint32_t *)guest_at(0x801b47ec);
          uint32_t *p47f0 = (uint32_t *)guest_at(0x801b47f0);
          if (entry_mode < 4 && 1 < entry_mode) {
            uint32_t pidx = (uint32_t)rec_f24[10];
            if (rec_f10[0] == 0) pidx = (uint32_t)rec_f10[3];
            pidx_b = (uint32_t)(rec_f10[0] != 0);
            if (entry_mode == 2) {
              int32_t xc = (((int16_t)_DAT_1f800020) -
                      (((int32_t)((uint16_t)_DAT_1f800020 << 16)) >> 31)) >> 1;
              int32_t yc = ((int16_t)_DAT_1f800022 * 3) / 5;
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(p47ec[pidx * 2]),
                           (int32_t)(((xc - t300[pidx]) + -0x44) * 0x10000) >> 16,
                           (yc + -0x48) * 0x10000 >> 16,5);
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206c4),
                           0x1000,0x1000,5);
              {
                uint32_t *pb = (uint32_t *)guest_at(0x801b32c8);
                announce_param_block_init_alt(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(pb[pidx]),
                             xc - t2f4[pidx],yc + -0x30,0);
              }
              if ((*(uint32_t *)(_g_system_config_ptr + 0x60) >> (pidx & 0x1f) & 1) != 0) {
                int32_t tx = (int16_t)_DAT_1f800020 * 3;
                if (tx < 0) tx += 3;
                int32_t ty = (int32_t)(int16_t)_DAT_1f800022;
                if (ty < 0) ty += 7;
                hud_text_d(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206bc),
                             (tx << 0xe) >> 16,ty >> 3,3);
              }
            } else {
              int32_t sx = 0;
              if (pidx_b != 0) sx = 0x140;
              int32_t yc = ((int16_t)_DAT_1f800022 * 3) / 5;
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(p47ec[pidx * 2]),
                           (int32_t)(((uint32_t)(t270[pidx * 2] + sx)) * 0x10000) >> 16,
                           (yc + -0x34) * 0x10000 >> 16,5);
              hud_text_d(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206b4),
                           (sx + 0x48) * 0x10000 >> 16,(yc + -0x24) * 0x10000 >> 16,0);
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(p47f0[pidx * 2]),
                           (int32_t)(((uint32_t)(t271[pidx * 2] + sx)) * 0x10000) >> 16,
                           (yc + -0x1b) * 0x10000 >> 16,5);
            }
            int32_t yc = ((int16_t)_DAT_1f800022 * 3) / 5;
            int32_t yt = yc + -8;
            {
              uint8_t *hh = (uint8_t *)guest_at(0x801b3288);
              pkv = 0x140 - hh[pidx];
            }
            int32_t attrw;
            if (rec_f24[pidx_b * 0xc + 2] == 0) attrw = 0x7996;
            else { rec_f24[pidx_b * 0xc + 2]--; attrw = 0x7b56; }
            draw_slot = (int32_t)(pidx + 0xb);
            if (pidx == 8) draw_slot = 0x16;
            spr_draw_off(draw_slot * 0x8014 + -0x7fff0000,0x280,0,1);
            attrw <<= 16;
            attr_idx = 200;
            spr_attr_alloc_b(_DAT_1f800008 + 4,(uint32_t)((yc + -0x107) * 0x10000 | 0x13f),
                         0xfebe000a,(uint32_t)attrw,0x80,yt);
            spr_draw_off(pidx * 0x8014 + -0x7fff0000,0x200,0,1);
            int32_t yadj = -200;
            if (pidx == 8) { attr_idx = 0xff; yadj = -0xff; }
            uint32_t pkw = (uint32_t)((yt + yadj) * 0x10000 | ((pkv + (int32_t)attr_idx) & 0xffff));
            {
              uint32_t attr_code = 0xfefe0008;
              int32_t ot_ptr = _DAT_1f800008 + 4;
LAB_80150558:
              spr_attr_alloc_b((uint32_t)ot_ptr,pkw,attr_code,(uint32_t)attrw,pkv,yt);
            }
          } else {
            int32_t yc = ((int16_t)_DAT_1f800022 * 3) / 5;
            /* guest caches the two select-row indices in $s6/$fp at
             * 0x8014F8B0 (s7+0x1c / s7+0x4c == rec_f10[3] / rec_f24[10]) */
            int32_t ply_a = row_a;
            int32_t ply_b = row_b;
            uint32_t pkw;
            if (rdy_p0 != 0) {
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(p47ec[ply_a * 2]),
                           (int32_t)((uint32_t)t270[ply_a * 2] + 0), /* (&DAT_801b3270)[ply_a*2] u32? */
                           (yc + -0x34) * 0x10000 >> 16,5);
              hud_text_d(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206b4),0x48,
                           (yc + -0x24) * 0x10000 >> 16,0);
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(p47f0[ply_a * 2]),
                           (int32_t)((uint32_t)t271[ply_a * 2] + 0),
                           (yc + -0x1b) * 0x10000 >> 16,5);
              if ((*(uint32_t *)(_g_system_config_ptr + 0x60) >> ((ply_a + 0x10U) & 0x1f) & 1) != 0) {
                int32_t ty = (int32_t)(int16_t)_DAT_1f800022;
                if (ty < 0) ty += 7;
                hud_text_d(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206bc),8,
                             ty >> 3,3);
              }
            }
            if (rdy_p1 != 0) {
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(p47ec[ply_b * 2]),
                           (int32_t)((uint32_t)t270[ply_b * 2] + 0x140),
                           (yc + -0x34) * 0x10000 >> 16,5);
              hud_text_d(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206b4),0x188,
                           (yc + -0x24) * 0x10000 >> 16,0);
              announce_param_block_init(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(p47f0[ply_b * 2]),
                           (int32_t)((uint32_t)t271[ply_b * 2] + 0x140),
                           (yc + -0x1b) * 0x10000 >> 16,5);
              if ((*(uint32_t *)(_g_system_config_ptr + 0x60) >> ((ply_b + 0x10U) & 0x1f) & 1) != 0) {
                int32_t ty = (int32_t)(int16_t)_DAT_1f800022;
                if (ty < 0) ty += 7;
                hud_text_d(_DAT_1f800008,_DAT_1f80000c,(const void *)guest_at(0x801206bc),
                             (int32_t)((_DAT_1f800020 - 0x38) * 0x10000) >> 16,ty >> 3,3);
              }
            }
            if (rdy_p0 != 0) {
              int32_t attrw;
              if (rec_f24[2] == 0) attrw = 0x7996;
              else { attrw = 0x7b56; rec_f24[2]--; }
              spr_draw_off(ply_a * 0x8014 + -0x7fff0000,0x200,0,1);
              if (ply_a == 8) pkw = (uint32_t)_DAT_1f800022 * 0x10000 + 0xfe390000 | 0xff;
              else pkw = (uint32_t)_DAT_1f800022 * 0x10000 + 0xfe700000 | 200;
              spr_attr_alloc_b(_DAT_1f800008 + 4,pkw,0xfefe0008,(uint32_t)(attrw << 16),0,
                           (int16_t)_DAT_1f800022 + -200);
            }
            if (rdy_p1 != 0) {
              int32_t attrw;
              if (rec_f24[0xe] == 0) attrw = 0x7996;
              else { attrw = 0x7b56; rec_f24[0xe]--; }
              spr_draw_off(ply_b * 0x8014 + -0x7fff0000,0x240,0,1);
              int32_t yp; int32_t otptr; int32_t attrc; int32_t tx;
              if (ply_b == 8) {
                attrc = 0xfefe0009; attrw <<= 16; otptr = _DAT_1f800008 + 4;
                tx = (int32_t)(int16_t)_DAT_1f800020;
                pkw = (uint32_t)_DAT_1f800022 * 0x10000 + 0xfe390000 | ((tx - 0xffU) & 0xffff);
                yp = (int16_t)_DAT_1f800022 + -200;
              } else {
                attrc = 0xfefe0009; attrw <<= 16; otptr = _DAT_1f800008 + 4;
                tx = (int32_t)(int16_t)_DAT_1f800020;
                pkw = (uint32_t)_DAT_1f800022 * 0x10000 + 0xfe700000 | ((tx - 200U) & 0xffff);
                yp = (int16_t)_DAT_1f800022 + -200;
              }
              spr_attr_alloc_b((uint32_t)otptr,pkw,attrc,(uint32_t)attrw,tx,yp);
            }
          }
        }
        if ((*(uint32_t *)g__DAT_1f8000dc & 3) == 0 && (tick_cnt = tick_cnt + 1, 5 < tick_cnt))
          tick_cnt = 0;
        uint32_t gmode;
        if (entry_mode == 2) gmode = 0x19;
        else if ((*(uint8_t *)g_DAT_1f8000a2 == 0) || entry_mode == 1) gmode = 0x18;
        else gmode = 0x48;
        hud_gauge_render(0x1f800000,_DAT_1f800008,gmode);
        tick_gate++;
        goto LAB_80150610;
      }
      deferred_cmd_post(0x400,0xd,0);
      ret_code = -1;
      DAT_1f800001 = 0xf0;
    } else {
      if (DAT_1f800001 < 7) {
        int32_t fdiv;
        if (DAT_1f800001 == 0 && (fdiv = frame_divisor_get(), fdiv == 0)) {
          if (-1 < entry_mode) {
            if (entry_mode < 2) attr_idx = 1;
            else if (entry_mode < 4) attr_idx = 2;
          }
          int32_t mvst = intro_movie_state((int32_t)attr_idx);
          if (mvst != 0) {
            sprite_desc_table_init(1);
            print_mode_dispatch_b(2,0,0);
            battle_config_eval(spad_cfg_a,0x1f800000,1);
            battle_config_eval(spad_cfg_b,0x1f800000,1);
            post_confirm_config_build(0x1f800000,arg_blk,1);
            *(uint32_t *)g__DAT_801e8508 = 0;
            *(uint32_t *)g__DAT_801e8518 = 0x28;
            if (*(int32_t *)(_g_system_config_ptr + 0x40) < 0) sel_cursor = 9;
            uint32_t marg = 1;
            if (*(char *)(_g_system_config_ptr + 0xd) == 0) marg = 0x1f;
            mc_confirm_dispatch(marg,1);
            next_state = 6;
            tick_gate = 0;
            goto LAB_8015060c;
          }
        }
      } else {
        if (DAT_1f800001 == 0xf0) {
          deferred_cmd_post(0x204,0,0);
          next_state = 0xf1;
        } else {
          next_state = 0xff;
          if (DAT_1f800001 != 0xf1) goto LAB_80150610;
        }
LAB_8015060c:
        DAT_1f800001 = next_state;
      }
LAB_80150610:
      frame_tick(0x1f800000);
    }
    if (DAT_1f800001 == 0xff) {
      if (sel_cursor == 9) {
        *(uint32_t *)(_g_system_config_ptr + 0x40) |= 0x80000000;
        int32_t tb = table_byte_next_a();
        DAT_1f800098 = (uint8_t)((uint32_t)(tb * 9) >> 8);
      } else {
        *(uint32_t *)(_g_system_config_ptr + 0x40) &= 0x7fffffff;
        DAT_1f800098 = (uint8_t)sel_cursor;
      }
      DAT_1f800001 = 0;
      br2_guest_frame_free();
      return ret_code;
    }
  } while (1);
}

/* select_cursor_step — select-screen cursor step: walks the 4x4 cell grid
 * at 0x801b3218 from (param_1 row, param_2 col-offset a3) checking the
 * unlocked-character bitmask at system_config+0x4c; wraps column a3
 * mod 4 and returns the validated bit. Disasm-verified. */
int32_t select_cursor_step(int32_t row,int32_t col0)
{
  int32_t col = col0;
  uint8_t *grid = (uint8_t *)guest_at(0x801b3218);
  uint32_t unlocked = *(uint32_t *)(_g_system_config_ptr + 0x4c);
  uint32_t mask = unlocked, goff;
  int32_t ch_idx = grid[col + row * 4];
  uint32_t ok = (mask >> ch_idx) & 1;
  if (ok != 0) return (int32_t)ok;
  goff = (uint32_t)ch_idx * 4;
  goto LAB_f208;
LAB_f248:
  ok = (unlocked >> ch_idx) & 1;
LAB_f24c:
  ok &= 1;
  if (ok == 0) goto LAB_f208;
  return (int32_t)ok;
LAB_f208:
  {
    int32_t nch = grid[col + goff];
    ch_idx = nch;
    if (nch != ch_idx) goto LAB_f248;
    col = (col - 1) & 3;
    if (col != col0) {
      ch_idx = grid[col + goff];
      ok = (unlocked >> ch_idx) & 1;
      goto LAB_f24c;
    }
    ch_idx = row;
    goto LAB_f248;
  }
}

/* select_confirm_handler — select-screen confirm handler. param_1 = guest addr
 * of battle state (0x1f800000), player = player slot, recs =
 * records base (host ptr, 0x30 stride), btns = button flags,
 * direct = stack arg (direct vs table char lookup). Writes chosen
 * char to battle+0xc0[player]+0x1b and anim state to
 * 0x1f8000e8+player*0x108+3/+4; gates on select_phase_gate then toggles
 * battle+0xb4 phase bits. Disasm-verified. */
extern int32_t select_phase_gate(uint32_t,uint32_t);
extern void selection_stat_bump(uint8_t *,int32_t);
void select_confirm_handler(uint32_t battle_g,uint32_t player,uint8_t *recs,
                  uint32_t btns,int32_t direct)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  uint8_t *precb = (uint8_t *)guest_at(0x1f8000e8 + player * 0x108);
  uint8_t mode = b[0x9b];
  uint8_t *rec = recs + player * 0x30;
  uint8_t *tbl = (uint8_t *)guest_at(0x801b239c);
  if (mode != 0 && 0 <= (int8_t)mode && mode < 5) {
    if (direct != 0) {
      uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0)) + player;
      cr[0x1b] = rec[0x1c];
    } else {
      uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0)) + player;
      cr[0x1b] = tbl[*(uint32_t *)(rec + 0x1c)];
    }
    if (direct != 0) precb[3] = rec[0x1c];
    else precb[3] = tbl[*(uint32_t *)(rec + 0x1c)];
  }
  if (select_phase_gate(battle_g,player) == 0) {
    uint16_t *ph = (uint16_t *)(b + 0xb4);
    if ((ph[player] & 0xf00) == 0x200) {
      selection_stat_bump(precb,0);
      if (btns & 0x800) precb[4] = 3;
      if (btns & 0x20)  precb[4] = 2;
      if (btns & 0x40)  precb[4] = 1;
      if (btns & 0x80)  precb[4] = 0;
      uint32_t foe = player ^ 1;
      if (ph[foe] & 0x1000) ph[foe] &= 0xefff; else ph[foe] |= 0x1000;
      ph[player] = (ph[player] & 0xf000) | 0x300;
      uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0)) + player;
      cr[0x1b] = precb[3];
    }
  } else {
    if (mode == 0) {
      uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0)) + player;
      cr[0x19] = cr[0x19] & 3;
      precb[4] = (uint8_t)(player ^ 1);
    } else if (0 <= (int8_t)mode && mode < 5) {
      if (btns & 0x800) precb[4] = 3;
      if (btns & 0x20)  precb[4] = 2;
      if (btns & 0x40)  precb[4] = 1;
      if (btns & 0x80)  precb[4] = 0;
    }
  }
}

/* select_phase_gate — select-phase gate: returns 0 only when the player's
 * battle+0xb4 phase hi-nibble is 0x100/0x200/0x300/0x400/0x700,
 * else 1. param_1 = guest battle addr. Disasm-verified. */
int32_t select_phase_gate(uint32_t battle_g,uint32_t player)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  uint16_t *ph = (uint16_t *)(b + 0xb4);
  uint32_t cls = ph[player] & 0xf00;
  if (cls==0x100||cls==0x200||cls==0x300||cls==0x400||cls==0x700) return 0;
  return 1;
}

/* selection_stat_bump — per-mode selection stat bump. mode = battle+0x9c;
 * mode 2 always skips, mode 5 skips when battle+0x9b==4, else bumps
 * counter at 0x801c11e0 + state[3]*0x24 + 0x264 and sets dirty flag
 * 0x801c11e0+0x410. Disasm-verified (jump table 0x8011ebd0). */
void selection_stat_bump(uint8_t *state,int32_t unused)
{
  (void)unused;
  uint8_t *battle = (uint8_t *)guest_at(0x1f800000);
  uint32_t mode = battle[0x9c];
  if (mode < 6) {
    if (mode == 2) return;
    if (mode == 5 && battle[0x9b] == 4) return;
  }
  uint8_t *tbl = (uint8_t *)guest_at(0x801c11e0);
  uint8_t *rec = tbl + state[3] * 0x24;
  uint16_t *cnt = (uint16_t *)(rec + 0x264);
  *cnt = *cnt + 1;
  *(uint32_t *)(tbl + 0x410) = 1;
}

/* post_confirm_config_build — post-confirm battle-config builder. battle_g = guest
 * battle addr, recs = out record array (0x30 stride/player),
 * direct = direct-char flag. Per player: stores phase-gate results
 * (+0x38), flag word (+0x28), char id (+0x44), then mode-specific
 * (+0x3c/40/4c/48) fill. Disasm-verified. */
extern int32_t gate_read_ec(uint32_t,uint32_t);
void post_confirm_config_build(uint32_t battle_g,uint8_t *recs,int32_t direct)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);

  uint8_t *xlat = (uint8_t *)guest_at(0x801b23ac);
  uint8_t *u16src = (uint8_t *)guest_at(0x801b2378);
int32_t pi;
  int32_t i;

  confirm_bookkeeping(battle_g);
  for (i = 0; i < 0xa; i++)
    *(uint16_t *)(recs + 0x88 + i * 2) = *(uint16_t *)(u16src + i * 2);

  *(uint32_t *)(recs + 0x14) = 0x1387;
  *(uint32_t *)(recs + 0x20) = 0;
  *(uint32_t *)(recs + 0x1c) = 1;
  recs[0x24] = 0; recs[0x25] = 0;
  *(uint32_t *)(recs + 4) = 0;
  *(uint32_t *)(recs + 8) = 0;
  *(uint32_t *)(recs + 0xc) = 0;
  *(uint32_t *)(recs + 0x10) = 0;
  *(uint32_t *)(recs + 0x18) = (uint32_t)(int32_t)(int8_t)b[0x98];

  for (pi = 0; pi < 2; pi++) {
    uint8_t *prec = recs + pi * 0x30;
    uint16_t *ph = (uint16_t *)(b + 0xb4 + pi * 2);
    uint32_t cls = *ph & 0xf00;
    uint32_t t, fw, fw2;
    *(uint32_t *)(prec + 0x38) = (uint32_t)select_phase_gate(battle_g,pi);
    t = *(uint32_t *)(prec + 0x28);
    if (cls==0x100||cls==0x200||cls==0x700) t |= 1; else t &= ~1u;
    *(uint32_t *)(prec + 0x28) = t;
    {
      int32_t r = gate_read_ec(battle_g,pi);
      fw = (t & ~3u) | ((uint32_t)(r & 1) << 1);
      *(uint32_t *)(prec + 0x28) = fw;
      *(uint32_t *)(prec + 0x54) = 0;
      fw2 = fw & ~4u;
      fw2 |= (*ph >> 0xa) & 4;
      *(uint32_t *)(prec + 0x28) = fw2;
    }
  }

  {
    uint32_t mode = b[0x9b];
    int32_t one = 1;
    if (mode == 3) {
      /* path A: both players forced-ready */
      for (pi = 0; pi < 2; pi++) {
        uint8_t *rptr = recs + pi * 0x30;
        uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0));
        *(uint32_t *)(rptr + 0x3c) = (uint32_t)pi;
        *(uint32_t *)(rptr + 0x40) = (uint32_t)pi;
        *(uint32_t *)(rptr + 0x38) = 1;
        if (direct) *(uint32_t *)(rptr + 0x44) = cr[pi + 0x1b];
        else    *(uint32_t *)(rptr + 0x44) = xlat[cr[pi + 0x1b]];
        *(uint32_t *)(rptr + 0x4c) = 0;
        *(uint32_t *)(rptr + 0x28) |= 0x78;
      }
    } else if ((mode < 4 && (int8_t)mode >= 0) || mode == 4) {
      /* main per-player fill loop */
      for (pi = 0; pi < 2; pi++) {
        uint8_t *rptr = recs + pi * 0x30;
        uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0));
        uint32_t g38 = *(uint32_t *)(rptr + 0x38);
        *(uint32_t *)(rptr + 0x3c) = (uint32_t)pi;
        *(uint32_t *)(rptr + 0x40) = (uint32_t)pi;
        if (g38 == 0) {
          /* char select then +0x28 bit0 test */
          if (direct) *(uint32_t *)(rptr + 0x44) = cr[pi + 0x1b];
          else    *(uint32_t *)(rptr + 0x44) = xlat[cr[pi + 0x1b]];
          {
            uint32_t fw3 = *(uint32_t *)(rptr + 0x28);
            if (fw3 & 1) {
              *(uint32_t *)(rptr + 0x28) = fw3 | 0x78;
              *(uint32_t *)(rptr + 0x4c) = 0;
            } else {
              *(uint32_t *)(rptr + 0x28) = fw3 | 0x58;
              *(uint32_t *)(rptr + 0x4c) = (uint32_t)one;
            }
          }
        } else if (g38 == (uint32_t)one) {
          uint32_t fl_old = *(uint32_t *)(rptr + 0x28);
          uint32_t foe_bit;
          *(uint32_t *)(rptr + 0x28) = fl_old | 0x40;
          foe_bit = ((*(uint32_t *)(recs + (pi ^ 1) * 0x30 + 0x28) >> 1) & 1);
          if (foe_bit != (uint32_t)one) {
            /* 493c4: cr-based char select */
            if (direct) *(uint32_t *)(rptr + 0x44) = cr[pi + 0x1b];
            else    *(uint32_t *)(rptr + 0x44) = xlat[cr[pi + 0x1b]];
          } else {
            /* 49380: +0x28 = old|0x58, char via battle+0xa8 table */
            int16_t off;
            *(uint32_t *)(rptr + 0x28) = fl_old | 0x58;
            off = *(int16_t *)(b + 0xa2);
            if (direct) *(uint32_t *)(rptr + 0x44) =
              *(uint8_t *)guest_at(*(uint32_t *)(b + 0xa8) + (uint32_t)off);
            else    *(uint32_t *)(rptr + 0x44) = xlat[
              *(uint8_t *)guest_at(*(uint32_t *)(b + 0xa8) + (uint32_t)off)];
          }
          /* 49408 tail */
          if (*(uint8_t *)(b + 0x9c) == 5 &&
              (*(uint8_t *)(b + 0x9b) == (uint8_t)one ||
               *(uint8_t *)(b + 0x9b) == 4)) {
            *(uint32_t *)(rptr + 0x4c) = 0;
            *(uint32_t *)(rptr + 0x28) |= 0x78;
          } else {
            *(uint32_t *)(rptr + 0x4c) = (uint32_t)one;
          }
        }
        /* 49448 tail: +0x48 = +0x44 */
        *(uint32_t *)(rptr + 0x48) = *(uint32_t *)(rptr + 0x44);
      }
    }
  }
}


/* fighter_spawn_setup — fighter spawn/setup. battle_g = guest battle addr,
 * ab_a/ab_b = per-side arg blocks passed to battle_config_eval,
 * cfg = cfg base (records at +0x28, stride 0x30), flag = flag.
 * Patches battle+0xb4/b6 phase 0x500->0x600, per-player record fill
 * when rec+0x10==1 && phase==0x700, then mode dispatch on battle+0x9b
 * (jump table 0x801204c0) merging packed button u16s into rec+8.
 * Disasm-verified. */
extern void task_node_free(uint32_t);
extern uint32_t task_alloc_wrap_c(uint32_t,uint32_t,uint32_t,uint32_t);
void fighter_spawn_setup(uint32_t battle_g,uint8_t *ab_a,uint8_t *ab_b,
                  uint8_t *cfg,int32_t direct)
{
  uint8_t *bstate = (uint8_t *)guest_at(battle_g);
  uint16_t *bp = (uint16_t *)bstate;
  uint8_t *cfg_cur = cfg;
  uint8_t *srecs = cfg + 0x28;

  int32_t pi, i;

  battle_config_eval(ab_a,battle_g,direct);
  battle_config_eval(ab_b,battle_g,direct);

  if ((bp[0xb4/2] & 0xf00) == 0x500) { bp[0xb2/2] = 0; bp[0xb4/2] = 0x600; }
  if ((bp[0xb6/2] & 0xf00) == 0x500) { bp[0xb2/2] = 0; bp[0xb6/2] = 0x600; }

  if (bstate[1] < 7) {
    int32_t rtype = 0x59, poff_b = 0xb90, poff_a = 0xaa0;
    uint8_t *rcur = srecs;
    for (pi = 0; pi < 2; pi++) {
      if (*(uint32_t *)(rcur + 0x10) == 1 &&
          ((bp[0xb4/2 + pi] & 0xf00) == 0x700)) {
        uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(bstate + 0xc0));
        uint32_t ch;
        confirm_bookkeeping(battle_g);
        if (direct != 0) {
          ch = cr[pi + 0x1b];
          *(uint32_t *)rcur = (uint32_t)rtype;
          *(uint32_t *)(rcur + 0x24) = 0;
          *(uint32_t *)(rcur + 0x10) = 0;
          *(uint32_t *)(rcur + 0x1c) = ch;
          bstate[1] = 6;
        } else {
          uint8_t *a3t = (uint8_t *)guest_at(0x801b23ac);
          ch = a3t[cr[pi + 0x1b]];
          *(uint32_t *)rcur = (uint32_t)rtype;
          *(uint32_t *)(rcur + 0x24) = 0;
          *(uint32_t *)(rcur + 0x10) = 0;
          *(uint32_t *)(rcur + 0x1c) = ch;
          /* disasm 0x801488fc-0x80148924: DAT1==3 -> 2, DAT1==6 -> 5,
           * otherwise the store is skipped entirely (j 0x80148924). */
          if (bstate[1] == 3) bstate[1] = 2;
          else if (bstate[1] == 6) bstate[1] = 5;
          {
            uint32_t *c = (uint32_t *)cfg_cur;
            if (c[1] != 0) { task_node_free(c[1]); c[1] = 0; }
            if (c[3] != 0) { task_node_free(c[3]); c[3] = 0; }
          }
          {
            uint8_t *ttab = (uint8_t *)guest_at(0x801f10c0);
            uint32_t *c = (uint32_t *)cfg_cur;
            c[1] = task_alloc_wrap_c(*(uint32_t *)(ttab + 0) + poff_a + 0xe,
                                *(uint32_t *)(ttab + 4) + poff_a + 0xe,
                                0x801b2ed8,0x801b2ef4);
            c[3] = task_alloc_wrap_c(*(uint32_t *)(ttab + 0) + poff_b + 0xe,
                                *(uint32_t *)(ttab + 4) + poff_b + 0xe,
                                0x801b2ed8,0x801b2ef4);
          }
        }
        deferred_cmd_post(0x400,0x41,0);
        return;
      }
      poff_b += 0x28; poff_a += 0x28; cfg_cur += 4; pi++;
      rcur += 0x30;
    }
  }

  *(uint32_t *)(srecs + 0x38) = 0;
  *(uint32_t *)(srecs + 8) = 0;

  {
    uint32_t mode = bstate[0x9b];
    if (mode >= 5) return;
    switch (mode) {
    case 0: {
      uint32_t cls = bp[0x92/2] & 0xf00;
      int go;
      if (cls == 0x100) go = 2;
      else if (cls==0x200||cls==0x300||cls==0x400) go = 1;
      else return;
      (void)go;
      for (pi = 0; pi < 2; pi++) {
        uint8_t *pstate = bstate + pi * 2;
        uint8_t *sub = srecs + pi * 0x30;
        if (*(uint32_t *)(sub + 0x10) == 0 && (*(uint32_t *)sub & 1) == 1) {
          uint16_t *h = (uint16_t *)pstate;
          uint32_t v = (h[0x74/2] & 0xf00f) |
                       (h[0x6c/2] & 0xffff0700) |
                       (h[0x68/2] & 0x8f0);
          *(uint32_t *)(sub + 8) = v;
        }
      }
      return;
    }
    case 2: {
      /* merge both players' masked bits into own + foe records */
      uint32_t v;
      v = (bp[0x74/2] & 0xf00f) | (bp[0x6c/2] & 0xffff0700) | (bp[0x68/2] & 0x8f0);
      *(uint32_t *)(srecs + 8) = v;
      v = (bp[0x76/2] & 0xf00f) | (bp[0x6e/2] & 0xffff0700) | (bp[0x6a/2] & 0x8f0);
      *(uint32_t *)(srecs + 0x38) = v;
      return;
    }
    case 3: {
      /* guest @0x80148C64: merge BOTH players' bits into the FIRST record
       * whose +0x24 == 0 (beq -> merge); if both confirmed (+0x24!=0) do
       * nothing. Loop i<2 then return. */
      for (i = 0; i < 2; i++) {
        uint8_t *sub = srecs + i * 0x30;
        if (*(uint32_t *)(sub + 0x24) == 0) {
          uint32_t v = (bp[0x74/2] & 0xf00f) | (bp[0x6c/2] & 0xffff0700) |
                       (bp[0x68/2] & 0x8f0);
          v |= (bp[0x76/2] & 0xf00f) | (bp[0x6e/2] & 0xffff0700) |
               (bp[0x6a/2] & 0x8f0);
          /* note: guest ORs p1 fields into same accumulator then stores
           * once — masks differ per field so order matters; replicate:
           * v = p0merge | p1merge is NOT identical; do sequential OR of
           * the exact guest expression instead. */
          {
            uint32_t w = (bp[0x74/2] & 0xf00f);
            w |= (bp[0x76/2] & 0xf00f);
            w |= (bp[0x6c/2] & 0xffff0700);
            w |= (bp[0x6e/2] & 0xffff0700);
            w |= (bp[0x68/2] & 0x8f0);
            w |= (bp[0x6a/2] & 0x8f0);
            *(uint32_t *)(sub + 8) = w;
          }
          (void)v;
          return;
        }
      }
      return;
    }
    case 1:
    case 4:
    default: {
      /* per-player: select_phase_gate gate; rec+0x24 nonzero -> foe record,
       * zero -> own record; merge masked u16 triple */
      for (pi = 0; pi < 2; pi++) {
        uint8_t *s0p = bstate + pi * 2;
        uint8_t *s2p = srecs + pi * 0x30;
        if (select_phase_gate(battle_g,pi) != 0) continue;
        {
          uint8_t *dst;
          uint16_t *h = (uint16_t *)s0p;
          uint32_t v;
          if (*(uint32_t *)(s2p + 0x24) != 0) {
            int foe = pi ^ 1;
            dst = srecs + foe * 0x30;
          } else dst = s2p;
          v = (h[0x74/2] & 0xf00f) | (h[0x6c/2] & 0x700) | (h[0x68/2] & 0x8f0);
          *(uint32_t *)(dst + 8) = v;
        }
        return;   /* guest jumps to epilogue after first handled player */
      }
      return;
    }
    }
  }
}

/* gate_read_ec — returns state-rec u16 at +0xec for player when
 * select_phase_gate gate passes, else 0. Disasm-verified. */
int32_t gate_read_ec(uint32_t battle_g,uint32_t player)
{
  if (select_phase_gate(battle_g,player) != 0) return 0;
  uint8_t *st = (uint8_t *)guest_at(0x1f8000e8 + player * 0x108);
  return *(uint16_t *)(st + 0xec);
}

/* confirm_bookkeeping — confirm bookkeeping: per-player phase-gate results
 * pick battle+0x92 phase (0x100/0x200/0x300/0x400) and bump char-record
 * counters; mode==4 clears cr[4] and bumps state+1 when set;
 * battle+0x9c==3 sets cr[4]=1 (jump table 0x80120750). Disasm-verified. */
int confirm_bookkeeping(uint32_t battle_g)
{
  uint8_t *bg = (uint8_t *)guest_at(battle_g);
  uint8_t *ent_e8 = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *ent_1f0 = (uint8_t *)guest_at(0x1f8001f0);
  uint8_t *cr;
  int32_t g0, g1, mode;
  g0 = select_phase_gate(battle_g,0);
  g1 = select_phase_gate(battle_g,1);
  cr = (uint8_t *)guest_at(*(uint32_t *)(bg + 0xc0));
  mode = (int8_t)bg[0x9b];
  if (mode >= 0 && mode < 5) {
    uint32_t ph;
    if (g0 == 0) {
      if (g1 == 0) {
        ent_e8[1] = 0; ent_1f0[1] = 0;
        ph = 0x300;
        cr[4] = (uint8_t)(cr[5] + 1);
      } else {
        ent_e8[1] = 0;
        ent_1f0[1] = (uint8_t)(cr[0x14] + 1);
        ph = 0x100;
        cr[4] = (uint8_t)(cr[6] + 1);
      }
    } else {
      if (g1 == 0) {
        ent_e8[1] = (uint8_t)(cr[0x13] + 1);
        ent_1f0[1] = 0;
        ph = 0x200;
      } else {
        ent_e8[1] = (uint8_t)(cr[0x13] + 1);
        ent_1f0[1] = (uint8_t)(cr[0x14] + 1);
        ph = 0x400;
      }
      cr[4] = (uint8_t)(cr[6] + 1);
    }
    *(uint16_t *)(bg + 0x92) = (uint16_t)ph;
  }
  if (mode == 4) {
    cr[4] = 0;
    if (ent_e8[1] != 0) ent_e8[1] = (uint8_t)((cr[0xd4] | 0x40) + 1);
    if (ent_1f0[1] != 0) ent_1f0[1] = (uint8_t)((cr[0xd4] | 0x40) + 1);
  }
  {
    uint32_t m9c = bg[0x9c];
    if (m9c < 6 && m9c == 3) cr[4] = 1;
  }
  return 0;
}

/* battle_config_eval — per-player battle phase evaluator. argblock = per-side
 * config buffer (host ptr; byte +2 = player slot index), param_2 = guest
 * battle base (0x1f800000), param_3 = flag (a2 is rewritten by the guest
 * to (flag==0) in the entry delay slot — "fz").
 * Dispatches on the player's u16 phase at battle+idx*2+0xb4 hi-nibble
 * (0x000..0x900). Phase-0x000/0x300 re-dispatch on battle+0x9b via jump
 * tables 0x801209e0 / 0x801209f8 (5 entries each, index<5). Returns the
 * u16 at battle+idx*2+0x68 for the 0x100/0x400/0x500 classes and the
 * 0x801537b0 shared block, else 0. Disasm-verified 0x80153544-0x80153980. */
extern void mode_name_announce(uint32_t,uint32_t,int32_t);
extern int32_t bcfg_step_a(uint32_t,uint32_t,uint32_t);
extern void stage_fight_announce(uint32_t,uint8_t *);
extern void fight_timer_announce(uint32_t,uint8_t *,int32_t);
extern int32_t continue_countdown(uint32_t battle_g,uint32_t player);
extern void bcfg_step_d(uint32_t,uint32_t);
extern void bcfg_step_c(uint32_t,uint32_t);
extern void bcfg_step_b(uint32_t,int32_t);
int32_t battle_config_eval(uint8_t *argblk,uint32_t battle_g,int32_t flag)
{
  uint8_t *B = (uint8_t *)guest_at(battle_g);
  uint8_t *AB = argblk;
  int32_t fz = ((uint32_t)flag < 1);      /* sltiu $a2,$a2,1 */
  uint32_t ret = 0;                          /* $s3 */
  uint8_t *pl = B + AB[2] * 2;               /* $a0: player slot base */
  uint32_t ph = *(uint16_t *)(pl + 0xb4);    /* $a3 */
  uint32_t cls = ph & 0xf00;                 /* $v1 */
  uint32_t g, gcls, idx;
  int32_t r;

  g_bce_calls++;
  if (flag == 0) g_bce_fz0++; else g_bce_fzN++;
  g_bce_cls = cls; g_bce_p3 = (uint32_t)flag;

  if (cls == 0x500) goto L_ret68;            /* 0x801537f8 */
  if (cls < 0x501) {
    if (cls == 0x200) goto L_ret;            /* 0x80153960 */
    if (cls < 0x201) {
      if (cls == 0x000) goto L_state0;       /* 0x8015361c */
      /* cls == 0x100: 0x801536f4 */
      *(uint16_t *)(pl + 0xb4) = (uint16_t)((ph & 0x100) | 0x200);
      goto L_ret;
    }
    /* cls >= 0x201 (0x300/0x400 only, mask) */
    if (cls == 0x300) goto L_state3;         /* 0x80153700 */
    if (cls == 0x400) goto L_ret68;          /* 0x801537f8 */
    goto L_ret;                              /* 0x801535d4 */
  }
  /* cls >= 0x501 */
  if (cls == 0x800) goto L_800;              /* 0x80153910 */
  if (cls < 0x801) {
    if (cls == 0x600) goto L_600;            /* 0x80153804 */
    if (cls == 0x700) {                      /* 0x80153900 */
      bcfg_step_c(battle_g,AB[2]);
      goto L_ret;
    }
    goto L_ret;                              /* 0x80153600 */
  }
  if (cls == 0x900) goto L_900;              /* 0x80153918 */
  goto L_ret;                                /* 0x80153614 */

L_state0:                                    /* 0x8015361c, table 0x801209e0 */
  idx = B[0x9b];
  if (idx >= 5) goto L_ret;
  switch (idx) {
  case 0: goto L_A0;                         /* 0x8015364c */
  case 1:                                    /* 0x801536d8 */
  case 4: goto L_A14;
  case 2:                                    /* 0x801537b0 */
  case 3: goto L_ret68;
  }
  goto L_ret;

L_state3:                                    /* 0x80153700, table 0x801209f8 */
  idx = B[0x9b];
  if (idx >= 5) goto L_ret;
  switch (idx) {
  case 0: goto L_B0;                         /* 0x80153730 */
  case 1:                                    /* 0x801537b0 */
  case 4: goto L_ret68;
  case 2:                                    /* 0x801537cc */
  case 3: goto L_B23;
  }
  goto L_ret;

/* ---- phase-0x000 / state sub-blocks ---- */

L_A0:                                        /* 0x8015364c */
  g = *(uint16_t *)(B + 0x92);
  gcls = g & 0xf00;
  if (gcls == 0x200 || gcls == 0x100 || gcls == 0x300) goto L_A0_body;
  if (gcls == 0x400) goto L_ret68;           /* 0x80153690 */
  goto L_ret;                                /* 0x80153674/88 */

L_A0_body:                                   /* 0x80153698 */
  if (fz) mode_name_announce(battle_g,AB[2],0);
  r = bcfg_step_a(battle_g,AB[2],*(uint16_t *)(pl + 0x68));
  if (r == 0) goto L_ret;
  goto L_set700;                             /* 0x801538a4 */

L_A14:                                       /* 0x801536d8 */
  if (fz) mode_name_announce(battle_g,AB[2],0);
  goto L_ret;

/* ---- phase-0x300 / state sub-blocks ---- */

L_B0:                                        /* 0x80153730 */
  g = *(uint16_t *)(B + 0x92);
  gcls = g & 0xf00;
  if (gcls == 0x200 || gcls == 0x100) goto L_B0_774;
  if (gcls >= 0x201) {
    if (gcls == 0x300) goto L_B0_7cc;        /* 0x801537cc */
    goto L_ret;
  }
  goto L_ret;                                /* gcls==0 */

L_B0_774:                                    /* 0x80153774 */
  ret = *(uint16_t *)(pl + 0x68);
  if (!fz) goto L_ret;
  stage_fight_announce(battle_g,AB);
  fight_timer_announce(battle_g,AB,AB[2]);
  goto L_ret;

L_B0_7cc:                                    /* 0x801537cc */
  ret = *(uint16_t *)(pl + 0x68);
  if (!fz) goto L_ret;
  stage_fight_announce(battle_g,AB);
  goto L_ret;

L_B23:                                       /* 0x801537cc via table B idx 2/3 */
  goto L_B0_7cc;

/* ---- phase-0x600 ---- */

L_600:                                       /* 0x80153804 */
  if (B[0x9c] != 0) { *(uint16_t *)(pl + 0xb4) = 0x800; goto L_ret; }
  {
    uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(B + 0xc0));
    if (cr[0x1e] != 0) { *(uint16_t *)(pl + 0xb4) = 0x800; goto L_ret; }
  }
  /* 0x80153834: $a0 = $s1 (delay slot, always) */
  if (fz) mode_name_announce(battle_g,AB[2],1);
  r = continue_countdown(battle_g,AB[2]);
  if (r > 0) {                               /* 0x80153850 path */
    name_rec_gate(AB,(const void *)guest_at(0x801209ac));
    status_flag_cond_set(guest_of(AB),0); /* $a2=0x801209ac unused by callee */
    if (select_phase_gate(battle_g,(uint32_t)(AB[2] ^ 1)) != 0) {
      bcfg_step_d(battle_g,AB[2]);
      goto L_ret;
    }
    goto L_set700;                           /* 0x801538a4 */
  }
  if (r >= 0) goto L_ret;                    /* 0x801538bc bgez */
  /* r < 0: 0x801538c4 — phase store is the jal delay slot (before call) */
  *(uint16_t *)(pl + 0xb4) = 0x800;
  name_rec_gate(AB,(const void *)guest_at(0x801209ac));
  status_flag_cond_set(guest_of(AB),0);
  goto L_ret;

L_set700:                                    /* 0x801538a4 */
  *(uint16_t *)(pl + 0xb4) = 0x700;
  goto L_ret;

/* ---- phase-0x800 / 0x900 ---- */

L_800:                                       /* 0x80153910 */
  *(uint16_t *)(pl + 0xb4) = 0x900;
L_900:                                       /* 0x80153918 */
  if (fz) bcfg_step_b(battle_g,AB[2]);
  {
    uint32_t cur = *(uint16_t *)(pl + 0xb4);
    if ((cur & 0xff) < 0x96) *(uint16_t *)(pl + 0xb4) = (uint16_t)(cur + 1);
    else                    *(uint16_t *)(pl + 0xb4) = 0;
  }
  goto L_ret;

L_ret68:                                     /* 0x801537b0 / 0x801537f8 */
  ret = *(uint16_t *)(pl + 0x68);
L_ret:                                       /* 0x80153960/64 epilogue */
  return (int32_t)ret;
}

/* battle_state_gate — battle-state gate used by bcfg_step_a. Returns 0 when
 * s16(+0x82)!=0, mode byte +0x9c in [1,5], or +0x9b in [1,3]; then a
 * 17-entry jump table on +0x92 (<0x11) decides: states {0-4,15}->1,
 * {5-11,16}->0, {12-14}->(+0xa4>=0). JT 0x80120948 — disasm verified. */
int32_t battle_state_gate(uint32_t battle_g,uint32_t unused)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  uint32_t mode, b9b, s92;
  static const uint8_t jt_kind[17] = {
    1,1,1,1,1,0,0,0,0,0,0,0,2,2,2,1,0 };   /* 1->ret1 0->ret0 2->a4>=0 */
  (void)unused;
  if (*(int16_t *)(b + 0x82) != 0) return 0;
  mode = b[0x9c];
  if (mode != 0 && mode < 6) return 0;
  b9b = b[0x9b];
  if (b9b == 0) {
    if (*(int16_t *)(b + 0xa4) <
        (int32_t)*(uint8_t *)guest_at(*(uint32_t *)(b + 0xc0) + 0x10))
      goto do_jt;
    return 0;
  }
  if (b9b < 4) return 0;
do_jt:
  s92 = b[0x92];
  if (s92 >= 0x11) return 1;
  switch (jt_kind[s92]) {
  case 1: return 1;
  case 0: return 0;
  default: return *(int16_t *)(b + 0xa4) >= 0 ? 1 : 0;
  }
}

/* bcfg_step_a — combined gate: battle_state_gate(battle,0) && (flags&0x800)
 * && ret_zero_b(player)>=0. Disasm-verified. */
extern int32_t ret_zero_b(uint32_t);
int32_t bcfg_step_a(uint32_t battle_g,uint32_t player,uint32_t flags)
{
  if (battle_state_gate(battle_g,0) == 0) return 0;
  if ((flags & 0x800) == 0) return 0;
  return ret_zero_b(player) >= 0 ? 1 : 0;
}

/* bcfg_step_b — optional announce: when char-record flags bit 0x800
 * set, calls hud_text_blk_alt with string 0x8012834 and y = (a1? s16
 * obj+0x20 : 0x50) - 0x1b. Disasm-verified. */
extern void hud_text_blk_alt(uint32_t,uint32_t,const void *,int32_t,int32_t,int32_t);
void bcfg_step_b(uint32_t battle_g,int32_t flag)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  int32_t txt_y = flag ? ((int32_t)*(int16_t *)(b + 0x20) - 0x50) : 0x50;
  uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc8));
  if (((*(uint16_t *)(cr + 2) >> 0xb) & 1) != 0) {
    hud_text_blk_alt(*(uint32_t *)(b + 8),*(uint32_t *)(b + 0xc),
                 (const void *)guest_at(0x8012834),(int16_t)(txt_y - 0x1b),1,0x20);
  }
}

/* bcfg_step_c — player reset-to-idle: sfx, phase=0x100, char-rec
 * +0x19&=3, timer fixup on state+0x2c obj, chan_blocks_reset + ret_stub_b.
 * Disasm-verified. */
extern void chan_blocks_reset(uint32_t,uint32_t,int32_t);
extern void ret_stub_b(uint8_t *,int32_t);
void bcfg_step_c(uint32_t battle_g,uint32_t player)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  uint8_t *st_rec = (uint8_t *)guest_at(0x1f8000e8 + player * 0x108);
  uint8_t *obj, *cr;
  deferred_cmd_post(0x400,0xc,0);
  *(uint16_t *)(b + player * 2 + 0xb4) = 0x100;
  cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0)) + player;
  cr[0x19] = cr[0x19] & 3;
  obj = (uint8_t *)guest_at(*(uint32_t *)(st_rec + 0x2c));
  if (obj[1] == 0) {
    uint32_t cnt_cur = *(uint32_t *)(obj + 0xf0);
    uint32_t cnt_per = *(uint32_t *)(obj + 0xf4);
    if (cnt_cur >= cnt_per) *(uint32_t *)(obj + 0xf0) = cnt_cur - cnt_per;
  }
  chan_blocks_reset(0x1f8000e8 + player * 0x108,*(uint32_t *)guest_at(*(uint32_t *)(st_rec + 0x2c) + (0xf0)),1);
  ret_stub_b(st_rec,2);
}

/* bcfg_step_d — variant of 8015279c: same reset but foe-side extra
 * ctr_clamp(battle,-0x14) + battle+0xa0|=0x100 when obj+1 set, and
 * ret_stub_b(s0,1). Disasm-verified. */
extern void ctr_clamp(uint32_t,int32_t);
void bcfg_step_d(uint32_t battle_g,uint32_t player)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  uint8_t *st_rec = (uint8_t *)guest_at(0x1f8000e8 + player * 0x108);
  uint8_t *obj, *cr;
  deferred_cmd_post(0x400,0xc,0);
  *(uint16_t *)(b + player * 2 + 0xb4) = 0x100;
  obj = (uint8_t *)guest_at(*(uint32_t *)(st_rec + 0x2c));
  if (obj[1] == 0) {
    uint32_t cnt_cur = *(uint32_t *)(obj + 0xf0);
    uint32_t cnt_per = *(uint32_t *)(obj + 0xf4);
    if (cnt_cur >= cnt_per) *(uint32_t *)(obj + 0xf0) = cnt_cur - cnt_per;
  }
  cr = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0)) + player;
  cr[0x19] = cr[0x19] & 3;
  chan_blocks_reset(0x1f8000e8 + player * 0x108,*(uint32_t *)(st_rec + 0xf0),1);
  obj = (uint8_t *)guest_at(*(uint32_t *)(st_rec + 0x2c));
  if (obj[1] != 0) {
    ctr_clamp(battle_g,-0x14);
    *(uint16_t *)(b + 0xa0) |= 0x100;
  }
  ret_stub_b(st_rec,1);
}

/* fight_timer_announce — fight-timer announce: gates on char-record flag
 * bit0x800 + obj+0xb0==0 + cc+0xa==0; formats remaining frames
 * (obj+0xf0 /50 -> min, /60 -> sec+frame) via sprintf into stack buf,
 * then hud_text_blk draw. Disasm-verified (magic-div constants). */
extern void hud_text_blk(uint32_t,uint32_t,uint32_t,int32_t,int32_t,int32_t);
void fight_timer_announce(uint32_t battle_g,uint8_t *side,int32_t flag)
{
  uint8_t *bg = (uint8_t *)guest_at(battle_g);
  uint8_t *cr = (uint8_t *)guest_at(*(uint32_t *)(bg + 0xc8));
  int32_t rows, txt_x;
  uint32_t frame_cnt;
  int32_t secs, r, sec_rem, mins;
  /* sprintf buf = guest $sp+0x18 (frame -0x38) = entry_sp-0x20; the
   * string is re-read by hud_text_blk through nested frames, so the
   * write->draw span below runs inside a reserved guest frame
   * (sp+0x18 -> frm+0x20). */
  uint8_t *buf;
  if (((*(uint16_t *)(cr + 2) >> 0xb) & 1) == 0) return;
  if (bg[0xb0] != 0) return;
  if (*(int16_t *)guest_at(*(uint32_t *)(bg + 0xcc) + (0xa)) != 0) return;
  rows = (*(uint8_t *)guest_at(*(uint32_t *)(bg + 0xc0) + (0x21)) != 0) ? 6 : 5;
  txt_x = flag ? ((int32_t)*(int16_t *)(bg + 0x20) - 0x50) : 0x50;
  frame_cnt = *(uint32_t *)(side + 0xf0);
  /* frames/50 -> seconds (0x51eb851f magic, mulhi>>4), remainder*2 =
   * sub-frame count; seconds/60 -> min:sec (0x88888889 magic). */
  {
    int64_t m = (int64_t)(int32_t)frame_cnt * 0x51eb851f;
    int32_t mulhi = (int32_t)(m >> 32);
    secs = (mulhi >> 4) - ((int32_t)frame_cnt >> 31);
  }
  r = (int32_t)(frame_cnt - (uint32_t)secs * 50);
  r = r * 2;                            /* a3: sub-frame arg */
  {
    int64_t m = (int64_t)secs * 0x88888889;
    int32_t mulhi = (int32_t)(m >> 32);
    mins = ((mulhi + secs) >> 5) - (secs >> 31);   /* secs/60 = minutes */
  }
  sec_rem = secs - mins * 60;                     /* seconds */
  {
    uint32_t fta_frm = br2_guest_frame(0x38);
    buf = (uint8_t *)guest_at(fta_frm + 0x20);
    if (mins == 0) {
      txt_x += 0x20;
      sprintf_core((char *)buf,(const char *)guest_at(0x80120928),sec_rem,r);
    } else {
      if (mins >= 0x64) { mins = 0x63; sec_rem = 0x3b; r = 0x63; }
      txt_x += 8;
      sprintf_core((char *)buf,(const char *)guest_at(0x80120934),mins,sec_rem,r);
    }
    hud_text_blk(*(uint32_t *)(bg + 8),*(uint32_t *)(bg + 0xc),fta_frm + 0x20,txt_x,0x20,rows);
    br2_guest_frame_free();
  }
}

/* stage_fight_announce — round/stage announce selector. Gates on char-record
 * flag bit0x800; when side-obj+1 set picks a stage/fight string via
 * jump table 0x80120910[state+0x9c] (SPECIAL STAGE / FINAL STAGE /
 * STAGE %d / FINAL FIGHT / FIGHT %d) sprintf'd into a stack buf, else
 * direct "CHALLENGER" when 0x1f80dc&0x40 or champion_win_announce path.
 * Tail calls hud_text_blk_alt(text, y, 0x20, 7|1). Disasm-verified. */
extern void champion_win_announce(uint32_t,uint8_t *,int32_t);
void stage_fight_announce(uint32_t battle_g,uint8_t *side)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);

  uint8_t *subrec, *crecs;
  /* sprintf buf = guest $sp+0x18 (frame -0x38) = entry_sp-0x20; hud_text_blk_alt
   * re-reads it through nested frames, so the write->draw span inside the
   * subrec[1]!=0 branch runs inside a reserved guest frame (sp+0x18 -> frm+0x20). */
  uint8_t *buf;
  int32_t tot, txt_y;
  if (((*(uint16_t *)guest_at(*(uint32_t *)(b + 0xc8) + (2)) >> 0xb) & 1) == 0) return;
  subrec = (uint8_t *)guest_at(*(uint32_t *)(side + 0x2c));
  if (subrec[1] != 0) {
    uint32_t sfa_frm = br2_guest_frame(0x38);
    buf = (uint8_t *)guest_at(sfa_frm + 0x20);
    static const uint32_t jt[6] = {
      0x80151e4c,0x80151ed0,0x80151ed0,0x80151ea8,0x80151ea8,0x80151ed0 };
    int32_t wins = (int32_t)*(int16_t *)(b + 0xa2) + 1;
    crecs = (uint8_t *)guest_at(*(uint32_t *)(b + 0xc0));
    {
      uint32_t mode = b[0x9c];
      if (mode < 6) {
        uint32_t tgt = jt[mode];
        if (tgt == 0x80151e4c) {                 /* case 0 */
          if (wins == 9) {
            sprintf_core((char *)buf,(char *)guest_at(0x801208c0));
          } else if (wins < (int32_t)crecs[0x10]) {
            sprintf_core((char *)buf,(char *)guest_at(0x801208dc),wins);
          } else {
            sprintf_core((char *)buf,(char *)guest_at(0x801208d0));
          }
        } else if (tgt == 0x80151ea8) {          /* cases 3,4 */
          if (wins < (int32_t)crecs[0x10]) {
            sprintf_core((char *)buf,(char *)guest_at(0x801208f4),wins);
          } else {
            sprintf_core((char *)buf,(char *)guest_at(0x801208e8));
          }
        } else {                                /* cases 1,2,5 */
          sprintf_core((char *)buf,(char *)guest_at(0x801208f4),wins);
        }
      }
    }
    if (side[2] == 0) {
      txt_y = 0xe;
      hud_text_blk_alt(*(uint32_t *)(b + 8),*(uint32_t *)(b + 0xc),
                   buf,txt_y,0x20,7);
    } else {
      txt_y = (int32_t)*(int16_t *)(b + 0x20) - 0x92;
      hud_text_blk_alt(*(uint32_t *)(b + 8),*(uint32_t *)(b + 0xc),
                   buf,txt_y,0x20,7);
    }
    br2_guest_frame_free();
    return;
  }
  /* obj+1 clear path */
  tot = (int32_t)*(uint16_t *)(side + 0xe2) + (int32_t)*(uint16_t *)(subrec + 0xe2);
  if (tot == 0 &&
      (*(uint16_t *)(b + side[2] * 2 + 0xb4) & 0x1000) != 0) tot = 1;
  if (tot != 0) {
    champion_win_announce(battle_g,side,side[2]);
    return;
  }
  if ((*(uint32_t *)guest_at(0x1f8000dc) & 0x40) == 0) return;
  if (side[2] == 0) txt_y = 0x32;
  else            txt_y = (int32_t)*(int16_t *)(b + 0x20) - 0x6e;
  hud_text_blk_alt(*(uint32_t *)(b + 8),*(uint32_t *)(b + 0xc),
               (const void *)guest_at(0x80120900),txt_y,0x20,1);
}

/* champion_win_announce — champion-win announce: "CHAMPION 1 WIN" /
 * "CHAMPION %u WINS" gated on 0x1f8000dc mode bits (>>4)&3 !=0 and
 * &0x40==0; win counter at rec+0xe2 clamped to 999, drawn via
 * hud_text_blk_alt. Disasm 0x80151cd0-0x80151dc4. */
extern int str_len(char *);
void champion_win_announce(uint32_t battle_g,uint8_t *side,int32_t player)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  /* guest sprintf/draw buffer is a stack slot: $sp+0x18 post-prologue
   * (frame -0x40) = entry_sp-0x28; str_len/hud_text_blk_alt re-read it
   * through nested frames, so the write->draw span runs inside a
   * reserved guest frame (sp+0x18 -> frm+0x18). */
  uint8_t *buf;
  int32_t cnt, txt_w, txt_y;
  uint32_t dc = *(uint32_t *)guest_at(0x1f8000dc);
  if (((dc >> 4) & 3) == 0) return;
  if (dc & 0x40) return;
  cnt = *(int16_t *)(side + 0xe2);
  if (cnt < 0) { *(int16_t *)(side + 0xe2) = 0; return; }
  if (cnt == 0) return;
  if (cnt >= 0x3e8) { cnt = 0x3e7; *(int16_t *)(side + 0xe2) = (int16_t)cnt; }
  {
    uint32_t cwa_frm = br2_guest_frame(0x40);
    buf = (uint8_t *)guest_at(cwa_frm + 0x18);
    if (cnt == 1) {
      sprintf_core((char *)buf,(const char *)guest_at(0x8012089c));
      txt_w = 0xe;
    } else {
      sprintf_core((char *)buf,(const char *)guest_at(0x801208ac),cnt);
      txt_w = str_len((char *)buf);
    }
    if (player != 0) {
      txt_y = (int16_t)(*(int16_t *)(b + 0x20) - (txt_w * 3 + 0x50));
    } else {
      txt_y = (int16_t)(0x50 - txt_w * 3);
    }
    hud_text_blk_alt(*(uint32_t *)(b + 8),*(uint32_t *)(b + 0xc),
                 (const void *)buf,txt_y,0x20,1);
    br2_guest_frame_free();
  }
}

/* mode_name_announce — mode-name announce ("V.S./WATCH/SURVIVAL/STORY/
 * PRACTICE/CUSTOM MODE", "AWAITING CHALLENGER", "PRESS START").
 * Gates: char-rec bit0x800, 0x1f80dc bits4-5; a2!=0 forces the
 * challenger/press-start tail. Jump tables 0x80121ec4 (state+0x92,
 * 17) and 0x80121f0c (state+0x9c, 6) — disasm+rodata verified. */
extern void please_wait_announce(uint32_t,uint32_t);
void mode_name_announce(uint32_t battle_g,uint32_t player,int32_t force)
{
  uint8_t *B = (uint8_t *)guest_at(battle_g);
  int32_t text_x, attr;
  uint32_t text = 0;
  text_x = player ? ((int32_t)*(int16_t *)(B + 0x20) - 0x50) : 0x50;
  if (((*(uint16_t *)guest_at(*(uint32_t *)(B + 0xc8) + (2)) >> 0xb) & 1) == 0) return;
  if (((*(uint32_t *)guest_at(0x1f8000dc) >> 4) & 3) == 0) return;
  /* JT1 on B[0x92] < 0x11 */
  {
    static const uint8_t jt1_kind[17] = {
      0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,0,1 };   /* 0->636d0 1->524b0 2->a4-gate */
    uint32_t jt1_idx = B[0x92];
    int do524b0 = 0, cont = 1;
    if (jt1_idx < 0x11) {
      switch (jt1_kind[jt1_idx]) {
      case 1: do524b0 = 1; cont = 0; break;
      case 2:
        if (*(int16_t *)(B + 0xa4) < 0) { do524b0 = 1; cont = 0; }
        break;
      }
    }
    if (do524b0) { please_wait_announce(battle_g,player); return; }
    if (!cont) return;
  }
  /* 636d0: mode==0 extra gate */
  if (B[0x9c] == 0 &&
      (int32_t)*(int16_t *)(B + 0xa4) >=
      (int32_t)*(uint8_t *)guest_at(*(uint32_t *)(B + 0xc0) + (0x10))) {
    please_wait_announce(battle_g,player); return;
  }
  attr = 4;
  if (force == 0 && B[0x9c] < 6) {
    static const uint32_t jt2[6] = {
      0x801637d8,0x80163744,0x80163758,0x8016376c,0x801637c4,0x80163780 };
    uint32_t tgt = jt2[B[0x9c]];
    if (tgt == 0x80163744) { text = 0x80121e54; text_x -= 0x1b; }
    else if (tgt == 0x80163758) { text = 0x80121e60; text_x -= 0x1e; }
    else if (tgt == 0x8016376c) { text = 0x80121e6c; text_x -= 0x27; }
    else if (tgt == 0x801637c4) { text = 0x80121e98; text_x -= 0x1e; }
    else if (tgt == 0x80163780) {
      if (B[0x9b] == 4) { text = 0x80121e7c; text_x -= 0x27; }
      else               { text = 0x80121e8c; text_x -= 0x21; }
    } else tgt = 0x801637d8;
    if (text) goto emit;
  }
  /* 637d8 tail */
  if ((*(uint32_t *)guest_at(0x1f8000dc) & 0x40) != 0) {
    if (force != 0) return;
    text = 0x80121ea4;  /* AWAITING CHALLENGER */
    attr = 1; text_x -= 0x39;
  } else {
    text = 0x80121eb8;  /* PRESS START */
    text_x -= 0x21; attr = 4;  /* guest leaves $v0=4 (delay-slot li at 0x801637EC);
                         * only the AWAITING CHALLENGER path sets $v0=1 */
  }
emit:
  text_x = (int32_t)(int16_t)text_x;
  hud_text_blk_alt(*(uint32_t *)(B + 8),*(uint32_t *)(B + 0xc),
               (const void *)guest_at(text),text_x,0x20,attr);
}

/* continue_countdown — continue/countdown ticker on battle+0xb2 (14-bit
 * counter + 0x4000 armed flag). Prints "CONTINUE? %u" when armed,
 * advances counter, rescales >=0xa0 by /100*80 when buttons held,
 * returns -1 at 0x31f, 1 on ret_zero_c<0, else 0. Disasm-verified. */
extern int32_t battle_state_gate(uint32_t,uint32_t);
extern int32_t ret_zero_a(uint32_t);
extern int32_t ret_zero_c(uint32_t);
int32_t continue_countdown(uint32_t battle_g,uint32_t player)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);

  /* sprintf buf = guest $sp+0x18 (frame -0x38) = entry_sp-0x20; str_len/
   * hud_text_blk_alt re-read it through nested frames, so the
   * write->draw span below runs inside a reserved guest frame
   * (sp+0x18 -> frm+0x20). */
  uint8_t *buf;

  uint32_t dc;
  if (battle_state_gate(battle_g,1) == 0) return 0;
  dc = *(uint32_t *)guest_at(0x1f8000dc);
  if (((*(uint16_t *)guest_at(*(uint32_t *)(b + 0xc8) + (2)) >> 0xb) & 1) != 0 &&
      (dc & 0x40) != 0 && ((dc >> 4) & 3) != 0) {
    /* remaining = 9 - (counter&0x3fff)/10000 */
    uint32_t cnt = *(uint16_t *)(b + 0xb2) & 0x3fff;
    int64_t prod = (int64_t)(int32_t)cnt * 0x66666667;
    int32_t quo = (int32_t)(((int32_t)(prod >> 32) << 0xb) >> 0x10); /* hi>>5 */
    int32_t rem = 9 - quo;
    int w;
    uint32_t ccd_frm = br2_guest_frame(0x38);
    buf = (uint8_t *)guest_at(ccd_frm + 0x20);
    sprintf_core((char *)buf,(char *)guest_at(0x80120824),rem);
    w = str_len((char *)buf);
    {
      int32_t y;
      int32_t xext = w * 3;
      if (player != 0) { y = (int32_t)*(int16_t *)(b + 0x20) - (xext + 0x50); }
      else         { y = 0x50 - xext; }
      hud_text_blk_alt(*(uint32_t *)(b + 8),*(uint32_t *)(b + 0xc),
                   buf,(int16_t)y,0x20,1);
    }
    br2_guest_frame_free();
  }
  if (*(int16_t *)(b + 0xb2) == 0) ret_zero_a(player);
  {
    uint32_t b2 = *(uint16_t *)(b + 0xb2);
    if ((b2 & 0x3fff) < 0x31f) *(uint16_t *)(b + 0xb2) = (uint16_t)(b2 + 1);
  }
  if ((*(uint16_t *)(b + player * 2 + 0x68) & 0x800) != 0) {
    if (ret_zero_c(player) < 0) return 1;
  }
  if (ret_zero_a(player) != 0)
    *(uint16_t *)(b + 0xb2) = (uint16_t)((*(uint16_t *)(b + 0xb2) & 0x4000) + 1);
  if ((*(uint16_t *)(b + 0xb2) & 0x4000) == 0 &&
      (*(uint16_t *)(b + player * 2 + 0x68) & 0x800) != 0)
    *(uint16_t *)(b + 0xb2) = 0x4001;
  {
    uint32_t b2raw = *(uint16_t *)(b + 0xb2);
    uint32_t count = b2raw & 0x3fff;
    if (count < 4) return 0;
    if (count >= 0xa0 &&
        (*(uint16_t *)(b + player * 2 + 0x68) & 0xe8) != 0) {
      /* (count+0x50)/100 * 80 | flag */
      uint32_t x = count + 0x50;
      int64_t prod = (int64_t)(int32_t)x * 0x66666667;
      int32_t quo = (int32_t)(((int32_t)(prod >> 32)) >> 5) - ((int32_t)x >> 31);
      *(uint16_t *)(b + 0xb2) = (uint16_t)((uint32_t)quo * 80 | (b2raw & 0x4000));
    }
    if ((*(uint16_t *)(b + 0xb2) & 0x3fff) >= 0x31f) return -1;
    return 0;
  }
}

/* battle_state_init — per-player battle state init: zero +9d/+9e/+8c,
 * +0xb0=1/+0xb1=0, *(u16*)(*(u32*)(p+0x38)+0x80)=0x28 for both players,
 * phase-gate select_phase_gate -> force +0xb4/+0xb6 phase 0x300, then
 * confirm_bookkeeping. Disasm 0x80151328-0x801513c4. */
void battle_state_init(uint8_t *side0, uint8_t *side1, uint32_t battle_g)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);
  uint16_t phw;
  *(uint8_t *)(b + 0x9d) = 0;
  *(uint8_t *)(b + 0x9e) = 0;
  *(uint16_t *)(b + 0x8c) = 0;
  *(uint8_t *)(b + 0xb0) = 1;
  *(uint8_t *)(b + 0xb1) = 0;
  *(uint16_t *)guest_at(*(uint32_t *)(side0 + 0x38) + 0x80) = 0x28;
  *(uint16_t *)guest_at(*(uint32_t *)(side1 + 0x38) + 0x80) = 0x28;
  if (select_phase_gate(battle_g,0) == 0) {
    phw = *(uint16_t *)(b + 0xb4);
    *(uint16_t *)(b + 0xb4) = (uint16_t)((phw & 0xf000) | 0x300);
  }
  if (select_phase_gate(battle_g,1) == 0) {
    phw = *(uint16_t *)(b + 0xb6);
    *(uint16_t *)(b + 0xb6) = (uint16_t)((phw & 0xf000) | 0x300);
  }
  confirm_bookkeeping(battle_g);
}

/* ======== batch31: menu stepper / battle-struct helpers ======== */

/* record_state_stepper — per-record stepper dispatched on s16 state at rec+0
 * (jump table 0x8011ebe8, 7 entries; rec = u32 ptr stored at a0+0xcc,
 * all accesses guest-bridged). Case 1: u16 counter+6 / u16 timer+8 /
 * s16 index+2 advanced via results_record_build (timer 0x14, wrap idx>=3);
 * cases 2-5: counter+6++; case 6: same as 1 but timer 0xc8 and
 * state+0 = 1. Disasm 0x80128898-0x801289b0. */
extern int32_t results_record_build(int32_t,int32_t);
void record_state_stepper(uint32_t obj_g)
{
  uint32_t rec = *(uint32_t *)((uint8_t *)guest_at(obj_g) + 0xcc);
  int32_t st = (int16_t)MB16(rec);          /* lh ($s0) */
  if ((uint32_t)st >= 7) return;           /* sltiu $v1,7 */
  switch (st) {
  case 1: {
    uint32_t nc = MB16(rec + 6) + 1;        /* lhu +6, +1 */
    int32_t  ts = (int16_t)MB16(rec + 8);   /* lh +8 zero test */
    *(uint16_t *)guest_at(rec + 6) = (uint16_t)nc;   /* sh delay slot */
    if (ts != 0) {
      *(uint16_t *)guest_at(rec + 8) = (uint16_t)(MB16(rec + 8) - 1);
    } else {
      results_record_build((int16_t)MB16(rec + 2), (int16_t)nc);
      *(uint16_t *)guest_at(rec + 8) = 0x14;
      uint32_t f2 = MB16(rec + 2) + 1;
      *(uint16_t *)guest_at(rec + 2) = (uint16_t)f2;
      if ((int16_t)f2 >= 3) *(uint16_t *)guest_at(rec + 2) = 0;
    }
    break;
  }
  case 6: {
    uint32_t nc = MB16(rec + 6) + 1;
    *(uint16_t *)guest_at(rec + 6) = (uint16_t)nc;   /* jal delay slot */
    results_record_build((int16_t)MB16(rec + 2), (int16_t)nc);
    *(uint16_t *)guest_at(rec + 8) = 0xc8;
    uint32_t f2 = MB16(rec + 2) + 1;
    *(uint16_t *)guest_at(rec + 2) = (uint16_t)f2;
    if ((int16_t)f2 >= 3) *(uint16_t *)guest_at(rec + 2) = 0;
    *(uint16_t *)guest_at(rec) = 1;                 /* sh $v0,($rec), v0=1 */
    break;
  }
  case 2: case 3: case 4: case 5:
    *(uint16_t *)guest_at(rec + 6) = (uint16_t)(MB16(rec + 6) + 1);
    break;
  default: break;                                    /* case 0: nop */
  }
}

/* menu_stepper_button_wait — menu stepper with button-wait: struct p (host ptr;
 * +2 state u8, +3 gate u8, +6 u16 max, +8 u16 cursor/timer, +0xc u32 GUEST
 * callback addr). param_2 = u32 button mask. If p[3]==0 runs menu_stepper_input
 * input then spins on pad flags @0x1f80007e updating the timer
 * (0x4000 set: timer++ wraps 1..max; clear: timer-- wraps to max)
 * until bit `param_2 >> (timer&31)` fires, invokes cb(p,timer), then
 * menu_stepper_masked_sweep(p,param_2). Disasm 0x80129540-0x8012965c. */
extern void menu_stepper_masked_sweep(void *,uint32_t);
void menu_stepper_button_wait(void *desc, uint32_t btn_mask)
{
  uint8_t *p = (uint8_t *)desc;
  uint32_t b2;
  if (btn_mask == 0) return;
  b2 = p[2];
  if (b2 != 1) {
    if (b2 >= 2) return;
    p[2] = 1;                            /* b2 == 0 */
  }
  if (p[3] != 0) goto L_tail;
  menu_stepper_input(desc);
  /* first bit test on current timer */
  if (((btn_mask >> (*(uint16_t *)(p + 8) & 31)) & 1) != 0) goto L_cb;
  for (;;) {
    uint16_t t, c;
    if ((MB16(0x1f80007e) & 0x4000) == 0) {
      t = *(uint16_t *)(p + 8);
      if (t == 1) *(uint16_t *)(p + 8) = *(uint16_t *)(p + 6);
      else        *(uint16_t *)(p + 8) = (uint16_t)(t - 1);
    } else {
      t = *(uint16_t *)(p + 8);
      c = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (t < c) ? (uint16_t)(t + 1) : (uint16_t)1;
    }
    t = *(uint16_t *)(p + 8);
    if (((btn_mask >> (t & 31)) & 1) != 0) break;
  }
L_cb:
  {
    /* p+0xc is a u32 GUEST code address (guest lw/jalr), same field
     * menu_stepper_state_advance/menu_stepper_redraw resolve via
     * fn_lookup — not a host pointer. */
    uint32_t cb_g = *(uint32_t *)(p + 0xc);
    void *cb = fn_lookup(cb_g);
    if (cb != NULL)
      ((void (*)(void *,uint16_t))cb)(desc,*(uint16_t *)(p + 8));
    else
      call_guest(cb_g,guest_of(desc),(uint32_t)*(uint16_t *)(p + 8));
  }
L_tail:
  menu_stepper_masked_sweep(desc,btn_mask);
}

/* battle_field_set — battle-struct field setter: u16 @+0x76 = a1,
 * u16 @+0x78 = 0. param_1 = guest u32 struct addr. Disasm
 * 0x8012fb04-0x8012fb10. */
void battle_field_set(uint32_t obj_g, int32_t val)
{
  *(uint16_t *)guest_at(obj_g + 0x76) = (uint16_t)val;
  *(uint16_t *)guest_at(obj_g + 0x78) = 0;
}

/* battle_struct_init — battle-struct init: zero 31 u16 window fields
 * (+0x20..+0x6c, +0x7e) and 3 u32 fields of param_2 (+0x5c..+0x64),
 * seed +0x4a/+0x4c/+0x4e from table 0x8011f944[stride8,+2/+4] indexed
 * by s16 +0x7e (just zeroed -> entry 0), store 0x168 @param_2+0x24,
 * gte_cw_d000_set(0x168,param_2), then +0x7c=1, +0x80=+0x81=4,
 * battle_math_init(p0,p1), +0x7a=0x1000, +0x82=-1, battle_field_set(p0,3).
 * Disasm 0x8012f024-0x8012f160. */
extern void gte_cw_d000_set(int32_t,uint32_t);
extern void battle_math_init(uint32_t,uint32_t);
void battle_struct_init(uint32_t battle_g, uint32_t env_g)
{
  static const uint16_t zfld[] = {
    0x20,0x22,0x24,0x28,0x2a,0x2c,0x30,0x32,0x34,0x38,0x3a,0x3c,0x3e,
    0x42,0x44,0x46,0x4a,0x4c,0x4e,0x52,0x54,0x56,0x5a,0x5c,0x5e,0x60,
    0x64,0x66,0x68,0x6c,0x7e };
  unsigned i;
  for (i = 0; i < sizeof(zfld)/sizeof(zfld[0]); i++)
    *(uint16_t *)guest_at(battle_g + zfld[i]) = 0;
  *(uint32_t *)guest_at(env_g + 0x5c) = 0;
  *(uint32_t *)guest_at(env_g + 0x60) = 0;
  *(uint32_t *)guest_at(env_g + 0x64) = 0;
  *(uint16_t *)guest_at(battle_g + 0x4a) = MB16(0x8011f944);
  {
    uint32_t e = 0x8011f944u +
                 ((uint32_t)(int32_t)(int16_t)MB16(battle_g + 0x7e) << 3);
    *(uint16_t *)guest_at(battle_g + 0x4c) = MB16(e + 2);
    *(uint16_t *)guest_at(battle_g + 0x4e) = MB16(e + 4);
  }
  *(uint16_t *)guest_at(env_g + 0x24) = 0x168;   /* jal delay slot */
  gte_cw_d000_set(0x168,env_g);
  *(uint16_t *)guest_at(battle_g + 0x7c) = 1;
  *(uint8_t  *)guest_at(battle_g + 0x81) = 4;
  *(uint8_t  *)guest_at(battle_g + 0x80) = 4;       /* jal delay slot */
  battle_math_init(battle_g,env_g);
  *(uint16_t *)guest_at(battle_g + 0x7a) = 0x1000;
  *(uint16_t *)guest_at(battle_g + 0x82) = (uint16_t)-1; /* jal delay */
  battle_field_set(battle_g,3);
}

/* per_side_setup — per-side setup: record_table_select(a0,a1); rec @0x801f04a8
 * (+0x1e=0, +0x21 = (s8)B[0x99]+1 < u8(*(u32*)B[0xc0]+4)*2-1); copy 8
 * u16s from table 0x8011fda0 + (u32)a0*16 (pairs D[i]=S[i],
 * D[4+i]=S[4+i], zero-fill D[8+i]/D[12+i], D[i]=0xffff when S[4+i]==0);
 * if a1==0 clear u16 +0x22; then dispatch table 0x801b1774 indexed by
 * ((s8)B[0x98]<<24)>>22 = s8*4 -> call_guest(fn,0x801f04a8,0x1f800000).
 * Disasm 0x801358e8-0x80135a10. */
extern void record_table_select(int32_t,int32_t);
int32_t per_side_setup(int32_t side, int32_t flag)
{
  const uint32_t dst_g = 0x801f04a8u;
  const uint32_t src_g = ((uint32_t)side << 4) + 0x8011fda0u;
  uint16_t *D = (uint16_t *)guest_at(dst_g + 0x34);
  const uint16_t *S = (const uint16_t *)guest_at(src_g);
  uint32_t c0, fn, t;
  int32_t i, lhs, rhs;
  record_table_select(side,flag);
  *(uint16_t *)guest_at(dst_g + 0x1e) = 0;
  c0  = MB32(0x1f8000c0);
  lhs = (int32_t)(int8_t)MB8(0x1f800099) + 1;
  rhs = MB8(c0 + 4) * 2 - 1;
  *(uint8_t *)guest_at(dst_g + 0x21) = (lhs < rhs) ? 1 : 0;
  for (i = 0; i < 4; i++) {
    D[i]     = S[i];
    D[4 + i] = S[4 + i];
    if (D[4 + i] == 0) D[i] = 0xffff;
    D[8 + i]  = 0;
    D[12 + i] = 0;
  }
  if (flag == 0) *(uint16_t *)guest_at(dst_g + 0x22) = 0;
  t = 0x801b1774u +
      (uint32_t)((int32_t)((uint32_t)MB8(0x1f800098) << 24) >> 22);
  fn = MB32(t);
  if (fn != 0)
    return (int32_t)call_guest(fn,dst_g,0x1f800000u);
  return 0;
}

/* battle_sides_commit — two-side battle commit: s1[0x9d] = (side-config
 * +0x38-count > 0 for either side); s0 = battle_config_eval(sideA,s1,0) |
 * battle_config_eval(sideB,s1,0); if s1[0x92]==4 -> battle_input_dispatch(s1,s0&0xffff);
 * if s16 s1[0x82]==0 and u8 s1[0x92]<5 dispatch table 0x80120a90:
 *  case0: side->bc u16+0x82=0xffff, battle_field_set(bc,3), clear_f0300_records,
 *         u16 s1+0x8c=0, battle_subrecord_reset(s1), s1[0x92] lo=1.
 *  case1/2/3/4 fall-through chain: lo=2 (case1 only), lo=3,
 *         u32 sideA/B +0x60=0, battle_subrecord_set1(s1), lo=4, s16_dec_with_floor(s1),
 *         post_battle_side_commit(sideA,sideB,s1), and if s0&0x100 s1[0x9a]=2,
 *         s1[1]=6. param_3 = guest u32 battle base 0x1f800000.
 * Disasm 0x80155c14-0x80155dcc. */
extern void battle_input_dispatch(uint32_t,uint32_t);
extern void battle_subrecord_reset(uint32_t);
extern void battle_subrecord_set1(uint32_t);
extern void s16_dec_with_floor(uint32_t);
extern void post_battle_side_commit(uint8_t *,uint8_t *,uint32_t);
void battle_sides_commit(uint8_t *side_a, uint8_t *side_b, uint32_t battle_g)
{
  uint8_t *b = (uint8_t *)guest_at(battle_g);


  uint32_t cfg_fl = 0;
  int32_t t;
  uint8_t b92;
  t = (int32_t)MB32(*(uint32_t *)(side_a + 0x20) + 0x38);
  if (t <= 0) {
    t = (int32_t)MB32(*(uint32_t *)(side_b + 0x20) + 0x38);
    if (t <= 0) { b[0x9d] = 0; goto L_eval; }
  }
  b[0x9d] = 1;
L_eval:
  cfg_fl |= (uint32_t)battle_config_eval(side_a,battle_g,0);
  cfg_fl |= (uint32_t)battle_config_eval(side_b,battle_g,0);
  if (b[0x92] == 4)
    battle_input_dispatch(battle_g,cfg_fl & 0xffff);
  if ((int16_t)MB16((battle_g) + 0x82) != 0) return;
  b92 = b[0x92];
  if (b92 >= 5) return;
  switch (b92) {
  case 0: {
    uint32_t bc = MB32(battle_g + 0xbc);
    *(uint16_t *)guest_at(bc + 0x82) = (uint16_t)-1;
    battle_field_set(bc,3);
    clear_f0300_records();
    *(uint16_t *)(b + 0x8c) = 0;
    battle_subrecord_reset(battle_g);
    *(uint16_t *)(b + 0x92) =
      (uint16_t)((*(uint16_t *)(b + 0x92) & 0xff00) | 1);
    break;
  }
  case 1:
    *(uint16_t *)(b + 0x92) =
      (uint16_t)((*(uint16_t *)(b + 0x92) & 0xff00) | 2);
    /* fallthrough */
  case 2:
    *(uint16_t *)(b + 0x92) =
      (uint16_t)((*(uint16_t *)(b + 0x92) & 0xff00) | 3);
    /* fallthrough */
  case 3:
    *(uint32_t *)(side_a + 0x60) = 0;
    *(uint32_t *)(side_b + 0x60) = 0;
    battle_subrecord_set1(battle_g);
    *(uint16_t *)(b + 0x92) =
      (uint16_t)((*(uint16_t *)(b + 0x92) & 0xff00) | 4);
    /* fallthrough */
  case 4:
    s16_dec_with_floor(battle_g);
    post_battle_side_commit(side_a,side_b,battle_g);
    if (cfg_fl & 0x100) {
      b[0x9a] = 2;
      b[1]    = 6;
    }
    break;
  default: break;
  }
}

/* pad_slot_state_walk — two-slot pad/state machine walker (gate: u8
 * 0x801b55d0 & 1). For it = 0,1 (s3 = it*0x10): st = cd_ctx_status_map(it*0x10)
 * dispatch table 0x80123828 (7 entries):
 *  case6: once-only init when flag@0x801f5968+it == 0
 *         (cd_ctx_field_get -> cd_ctx_set_fwd(s1)/cd_ctx_cmd_fwd), then flag=1,
 *         falls into case4/5 timer decay.
 *  case4/5(+6): s8 countdown pairs @0x801f596c/+0x5970 (+2/iter):
 *         nonzero -> u8--, zero -> paired flag byte = 0.
 *  case2: clear all four bytes, flag=1. case0/1: clear four, flag=0.
 *  case3: nop. Disasm 0x8018bba8-0x8018bd20. */
extern int32_t cd_ctx_status_map(int32_t);
extern int32_t cd_ctx_field_get(int32_t,int32_t,int32_t);
extern void cd_ctx_set_fwd(int32_t,uint32_t,int32_t);
extern void cd_ctx_cmd_fwd(int32_t,uint32_t);
void pad_slot_state_walk(void)
{
  uint32_t cur2, cur0, cur1;
  int32_t ctx_off;
  int32_t st;
  if ((MB8(0x801b55d0) & 1) == 0) return;
  cur2 = 0x801f5968u; cur0 = 0x801f596cu; cur1 = 0x801f5970u; ctx_off = 0;
  do {
    st = cd_ctx_status_map(ctx_off);
    if ((uint32_t)st < 7) {
      switch (st) {
      case 6:
        if (MB8(cur2) == 0) {
          if (cd_ctx_field_get(ctx_off,2,0) != 0) {
            cd_ctx_set_fwd(ctx_off,cur1,2);
            cd_ctx_cmd_fwd(ctx_off,0x801b55d4u);
          }
        }
        MB8(cur2) = 1;
        /* fallthrough */
      case 4: case 5:
        if (*(int8_t *)guest_at(cur0) != 0)
          MB8(cur0) = (uint8_t)(MB8(cur0) - 1);
        else
          MB8(cur1) = 0;
        if (*(int8_t *)guest_at(cur0 + 1) != 0)
          MB8(cur0 + 1) = (uint8_t)(MB8(cur0 + 1) - 1);
        else
          MB8(cur1 + 1) = 0;
        break;
      case 2:
        MB8(cur1 + 1) = 0; MB8(cur1) = 0;
        MB8(cur0 + 1) = 0; MB8(cur0) = 0;
        MB8(cur2)     = 1;
        break;
      case 0: case 1:
        MB8(cur1 + 1) = 0; MB8(cur1) = 0;
        MB8(cur0 + 1) = 0; MB8(cur0) = 0;
        MB8(cur2)     = 0;
        break;
      default: break;                            /* case 3: nop */
      }
    }
    cur2 += 1; cur0 += 2; cur1 += 2; ctx_off += 0x10;
  } while (cur2 < 0x801f596au);
}

/* ======== batch32: menu helpers (capstone-verified disasm ports) ======== */

#define MBS8(a)   (*(int8_t  *)guest_at(a))
#define MBS16(a)  (*(int16_t *)guest_at(a))
#define MW8(a,v)  (*(uint8_t  *)guest_at(a) = (uint8_t)(v))
#define MW16(a,v) (*(uint16_t *)guest_at(a) = (uint16_t)(v))
#define MW32(a,v) ((uint32_t)(a)==0x801c47b8u ? \
   ({uint32_t _bv=(uint32_t)(v);uint32_t *_bp=(uint32_t *)guest_at(0x801c47b8u);uint32_t _bo=*_bp;*_bp=_bv; \
     {decomp_trace_record(20,_bo,_bv,0,__LINE__);} _bv;}) \
   : (*(uint32_t *)guest_at(a) = (uint32_t)(v)))

/* ======== 0x8017d198 / 0x8017d19c / 0x8017d1c4 ======== */
/* Descriptor-fragment halfword pair writer. The three guest entries share
 * one tail: they compute a record offset from the live v0/v1 pair, load
 * u16 src+2 (a2), and store it at +0x0c into BOTH table pointers held at
 * a3+0x18 and a3+0x1c; the tail returns a2+4.
 *   0x8017d198: v0 = v1<<2 then falls through -> off = v1*40.
 *   0x8017d19c: off = (v0+v1)<<3 (direct entry uses live v0/v1).
 *   0x8017d1c4: standalone tail -> return a2+4.
 * Disasm generated/SLES_017.22_full_35.c @0x8017d198/0x8017d19c/0x8017d1c4. */
static uint32_t desc_frag_store_pair(uint32_t off, uint32_t src,
                                     uint32_t owner)
{
  uint32_t hval  = MB16(src + 2);                  /* lhu a2+2 */
  uint32_t dst_a = MB32(owner + 0x18);
  uint32_t dst_b = MB32(owner + 0x1c);
  MW16(dst_a + off + 0x0c, hval);
  MW16(dst_b + off + 0x0c, hval);
  return src + 4;
}

uint32_t desc_frag_scale_store(uint32_t unused1, uint32_t unused2, uint32_t src,
                               uint32_t owner)
{
  CPUState *c = clean_cpu_state();
  uint32_t off;
  (void)unused1; (void)unused2;
  off = (uint32_t)c->gpr[3] * 40u;              /* (v1<<2 + v1) << 3 */
  return desc_frag_store_pair(off, src, owner);
}

uint32_t desc_frag_sum_store(uint32_t unused1, uint32_t unused2, uint32_t src,
                             uint32_t owner)
{
  CPUState *c = clean_cpu_state();
  uint32_t off;
  (void)unused1; (void)unused2;
  off = (uint32_t)(c->gpr[2] + c->gpr[3]) << 3;
  return desc_frag_store_pair(off, src, owner);
}

uint32_t desc_frag_tail(uint32_t unused1, uint32_t unused2, uint32_t cur, uint32_t unused4)
{
  (void)unused1; (void)unused2; (void)unused4;
  return cur + 4;
}

/* desc_frag_op_dispatch — guest 0x8017D0B0 (CPS-split head of the
 * desc_frag_* family; the 0x8017d198/19c/1c4 pieces above are its tail).
 * Reads op = (lhu src >> 8) & 0xFF and flag = lbu(flags+0x100), then
 * dispatches:
 *   op==15 -> single store t1+0x28C (flag!=0) or pair-store idx0
 *   op<16  -> op==11: t1+0x23C / op==14: t1+0x1C4 / else pair-store idx
 *   op>=16 -> op==16: t1+0x264 / op==128: 27-entry stride-(-40) fill of
 *             t1+12+{1040..0} / else pair-store idx
 * where "pair-store idx" = desc_frag_store_pair(idx*40, src, owner) and
 * the single stores hit only t1 = *(owner+0x18) with h = lhu(src+2).
 * flag==0 on ops 11/14/15/16 jumps into the 0x8017d198 piece with
 * v1 = flag&0xFF = 0 -> pair-store at offset 0. Every path returns
 * src+4 in v0. Disasm generated/SLES_017.22_full_35.c
 * 0x8017D0B0-0x8017D198 + pieces 0x8017D198/19C/1C4. */
uint32_t desc_frag_op_dispatch(uint32_t owner, uint32_t flags, uint32_t src)
{
  uint32_t op   = ((uint32_t)*(const uint16_t *)guest_at(src) >> 8) & 0xFFu;
  uint32_t flag = (uint32_t)*(const uint8_t *)guest_at(flags + 0x100u);
  uint32_t idx, subrec, h, cnt, off, dst;

  if (op == 15u) goto op15;                     /* beq -> 0x8017D13C */
  if (op >= 16u) goto op_ge16;                  /* slti op<16 == 0 -> D0F0 */
  /* op < 16 && op != 15 */
  if (op == 11u) goto op11;                     /* -> 0x8017D10C */
  if (op == 14u) goto op14;                     /* -> 0x8017D124 */
  idx = flag & 0xFFu;                           /* D0E8: v1 &= 0xFF */
  return desc_frag_store_pair(idx * 40u, src, owner);   /* -> D198 piece */

op_ge16:                                         /* 0x8017D0F0 */
  if (op == 16u) goto op16;                     /* -> 0x8017D154 */
  if (op == 128u) goto op128;                   /* -> 0x8017D16C */
  idx = flag & 0xFFu;                           /* D100 delay-slot mask */
  return desc_frag_store_pair(idx * 40u, src, owner);   /* D104 -> D19C */

op11:                                            /* 0x8017D10C */
  if (flag == 0)                                /* beq a1,zero -> D198 */
    return desc_frag_store_pair(0, src, owner);
  subrec = MB32(owner + 0x18);                       /* lw v1,0x18(s7) */
  MW16(subrec + 0x23Cu, MB16(src + 2));              /* sh v0,0x23C(v1) */
  return src + 4;                               /* -> D1C4 tail */

op14:                                            /* 0x8017D124 */
  if (flag == 0)
    return desc_frag_store_pair(0, src, owner);
  subrec = MB32(owner + 0x18);
  MW16(subrec + 0x1C4u, MB16(src + 2));
  return src + 4;

op15:                                            /* 0x8017D13C */
  if (flag == 0)
    return desc_frag_store_pair(0, src, owner);
  subrec = MB32(owner + 0x18);
  MW16(subrec + 0x28Cu, MB16(src + 2));
  return src + 4;

op16:                                            /* 0x8017D154 */
  if (flag == 0)
    return desc_frag_store_pair(0, src, owner);
  subrec = MB32(owner + 0x18);
  MW16(subrec + 0x264u, MB16(src + 2));
  return src + 4;

op128:                                           /* 0x8017D16C fill loop */
  cnt = 26; off = 1040;
  do {
    cnt--;
    subrec = MB32(owner + 0x18);                     /* lw v0,0x18(s7) — live reload */
    h  = MB16(src + 2);                          /* lhu v1,2(a2) — live reload */
    dst = off + subrec;
    off -= 40;
    MW16(dst + 12, h);                           /* sh v1,0xC(v0) delay slot */
  } while ((int32_t)cnt >= 0);                   /* bgez a1 -> loop */
  return src + 4;                               /* D190 tail: v0 = a2+4 */
}

/* 16-byte-block guest copy; lwl/lwr vs lw/sw in the original is only an
 * alignment variant of the same memcpy. */
static void blk_copy(uint32_t dst, uint32_t src, uint32_t len)
{
  memcpy(guest_at(dst), guest_at(src), len);
}

/* results_record_build — victory/results record builder @0x801c47c0 + idx*0x235c.
 * HW args are (a0 = battle base 0x1f800000, a1 = record index,
 * a2 = u16 tag). Every real caller passes a0 = 0x1f800000
 * (record_state_stepper(0x1f800000), battle_subrecord_reset(param_3 = 0x1f800000),
 * versus_stage_commit same family); the in-file call sites only forward
 * (index, value), so a0 is fixed here. Fills the 0x2358-byte record:
 * fixed table blobs, four flag-selected 0x870/0x2f4 sub-records, a
 * packed controller/option byte at +0x2358 (self-ptr slot also
 * receives the +0x16d0 host-visible cursor). Returns bit7 of the
 * second side's +0x264 flag word (v0 left live at jr $ra). */
int32_t results_record_build(int32_t idx, int32_t val)
{
  const uint32_t gctx = 0x1f800000u;
  uint32_t rec = 0x801c47c0u + ((uint32_t)idx * 0x235cu);
  uint32_t pa, pb, f, bit;

  MW16(rec + 0, (uint16_t)val);          /* sh $a2,($rec) */
  MW16(rec + 2, MB16(gctx + 0x94));            /* sh lhu 0x94(gctx),2 */

  blk_copy(rec + 0x16cc, 0x1f8000e8, 0x210);
  blk_copy(rec + 0x18dc, 0x801f5328, 0x230);
  blk_copy(rec + 0x1b0c, 0x801f5258, 0x90);
  blk_copy(rec + 0x1b9c, 0x801f52e8, 0x40);
  blk_copy(rec + 0x1bdc, 0x801f54a8, 0x194);
  blk_copy(rec + 0x1d70, 0x801f0fb0, 0x108);
  blk_copy(rec + 0x1e78, 0x801f0db0, 0x200);
  blk_copy(rec + 0x2078, 0x801f0ae0, 0x2d0);
  blk_copy(rec + 0x2348, 0x801f4d70, 8);
  blk_copy(rec + 0x2350, 0x801f4e2c, 8);

  bit = MB8(0x1f8000ed) & 1;
  blk_copy(rec + 4,      0x801e1b3c + bit * 2160u, 0x870);
  bit = MB8(0x1f8001f5) & 1;
  blk_copy(rec + 0x874,  0x801e2c1c + bit * 2160u, 0x870);
  bit = MB8(0x1f8000ed) & 1;
  blk_copy(rec + 0x10e4, 0x801e0f6c + bit * 756u,  0x2f4);
  bit = MB8(0x1f8001f5) & 1;
  blk_copy(rec + 0x13d8, 0x801e1554 + bit * 756u,  0x2f4);

  pa = MB32(0x1f800100);                     /* lw 0x18(0x1f8000e8) */
  pb = MB32(0x1f800208);                     /* lw 0x18(0x1f8001f0) */
  f  =  MB16(pa + 0x23c) & 1;
  f |= (MB16(pa + 0x1c4) & 1) << 1;
  f |= (MB16(pa + 0x28c) & 1) << 2;
  f |= (MB16(pa + 0x264) & 1) << 3;
  f |= (MB16(pb + 0x23c) & 1) << 4;
  f |= (MB16(pb + 0x1c4) & 1) << 5;
  f |= (MB16(pb + 0x28c) & 1) << 6;
  f |= (MB16(pb + 0x264) & 1) << 7;
  MW32(rec + 0x2358, f);                      /* jr delay slot */
  return (int32_t)((MB16(pb + 0x264) & 1) << 7);
}

/* battle_subrecord_reset — battle-subrecord reset: v1 = *(u32*)(a0+0xcc);
 * v1[0]=2, v1[2..0xe even u16s = 0; memcpy v1+0x18 <- 0x801e1b3c
 * (0x21c0), v1+0x21d8 <- 0x801e0f6c (0xbd0); then
 * results_record_build(a0, 3, lh v1+6). a0 = guest battle base. */
void battle_subrecord_reset(uint32_t battle_g)
{
  uint32_t sub = MB32(battle_g + 0xcc);
  uint8_t *hrec = (uint8_t *)guest_at(sub);
  MW16(sub + 0,    2);
  MW16(sub + 2,    0);
  MW16(sub + 4,    0);
  MW16(sub + 8,    0);
  MW16(sub + 6,    0);
  MW16(sub + 0xa,  0);
  MW16(sub + 0xc,  0);
  MW16(sub + 0xe,  0);
  memcpy(hrec + 0x18,  guest_at(0x801e1b3c), 0x21c0);
  memcpy(hrec + 0x21d8, guest_at(0x801e0f6c), 0xbd0);
  results_record_build(3, *(int16_t *)(hrec + 6));
}

/* battle_subrecord_set1 — *(u16*)(*(u32*)(a0+0xcc)) = 1. */
void battle_subrecord_set1(uint32_t battle_g)
{
  MW16(MB32(battle_g + 0xcc), 1);
}

/* menu_stepper_masked_sweep — masked menu-stepper sweep: cb(+0xc)(p,0x100); for
 * s0 = 1..u16(p+6): if bit (param_2 >> s0) set, paint row
 * (menu_row_paint when s0 == u16(p+8) else menu_row_paint_b) and
 * cb(p,(s0+0x100)&0xffff); finally cb(p,0x1ff). p+0xc = u32 guest cb. */
extern void menu_row_paint(void *);
extern void menu_row_paint_b(void *);
void menu_stepper_masked_sweep(void *desc, uint32_t btn_mask)
{
  uint8_t *p = (uint8_t *)desc;
  /* p+0xc is a u32 GUEST code address (guest lw/jalr) — resolve via
   * fn_lookup like menu_stepper_redraw, not a host pointer. */
  uint32_t cb_g = *(uint32_t *)(p + 0xc);
  void (*cb)(void *,uint32_t) =
      (void (*)(void *,uint32_t))fn_lookup(cb_g);
  uint32_t bidx;
#define CB_CALL(code_) do { \
    if (cb != NULL) cb(desc,(code_)); \
    else            call_guest(cb_g,guest_of(desc),(code_)); \
  } while (0)
  CB_CALL(0x100);
  if (*(uint16_t *)(p + 6) == 0) goto L_tail;
  bidx = 1;
  for (;;) {
    if (((btn_mask >> (bidx & 31)) & 1) != 0) {
      if (bidx != *(uint16_t *)(p + 8)) menu_row_paint_b(desc);
      else                            menu_row_paint(desc);
      CB_CALL((bidx + 0x100) & 0xffff);
    }
    bidx++;
    if (*(uint16_t *)(p + 6) < bidx) break;
  }
L_tail:
  CB_CALL(0x1ff);
#undef CB_CALL
}

/* u16_counter_step_pad_a — saturating u16 counter @guest a0 with pad-edge
 * sfx: on 0x2000 frame ++ (wrap to 0 when a1 < new), on 0x8000 frame
 * -- (reload a1 on underflow); a2 != 0 triggers deferred_cmd_post(0x400,
 * 0xb,0). Returns whether the s16 value changed. */
int u16_counter_step_pad_a(uint32_t ctr_g, int32_t max, int32_t sfx)
{
  uint8_t *ctr = (uint8_t *)guest_at(ctr_g);
  int16_t  old = *(int16_t *)ctr;
  uint32_t lim = (uint32_t)max;
  uint32_t pad = MB16(0x1f80007e);

  if (pad == 0x2000) {
    uint16_t nv;
    if (sfx != 0) deferred_cmd_post(0x400,0xb,0);
    nv = (uint16_t)(*(uint16_t *)ctr + 1);
    *(uint16_t *)ctr = nv;
    if ((int32_t)((uint32_t)max << 16) < (int32_t)(nv << 16))
      *(uint16_t *)ctr = 0;
  }
  if (MB16(0x1f80007e) == 0x8000) {
    uint16_t nv;
    if (sfx != 0) deferred_cmd_post(0x400,0xb,0);
    nv = (uint16_t)(*(uint16_t *)ctr - 1);
    *(uint16_t *)ctr = nv;
    /* sh $lim,(ctr) sits in the bgez delay slot and both branch
     * outcomes converge: the reload is UNCONDITIONAL. */
    *(uint16_t *)ctr = (uint16_t)lim;
    (void)nv;
  }
  return (int)(*(int16_t *)ctr != old);
}

/* u16_counter_step_pad_b — same counter shape as u16_counter_step_pad_a but different
 * wrap rules: on 0x2000 ++ and reload param_2 when param_2 < new
 * (else sfx); on 0x8000 -- and zero on underflow (else sfx). */
int u16_counter_step_pad_b(uint32_t ctr_addr, int32_t clamp)
{
  uint8_t *ctr = (uint8_t *)guest_at(ctr_addr);
  int16_t  old = *(int16_t *)ctr;

  if (MB16(0x1f80007e) == 0x2000) {
    uint16_t nv = (uint16_t)(*(uint16_t *)ctr + 1);
    *(uint16_t *)ctr = nv;
    if ((int32_t)((uint32_t)clamp << 16) < (int32_t)(nv << 16))
      *(uint16_t *)ctr = (uint16_t)clamp;   /* j delay: sh $a1 */
    else
      deferred_cmd_post(0x400,0xb,0);
  }
  if (MB16(0x1f80007e) == 0x8000) {
    uint16_t nv = (uint16_t)(*(uint16_t *)ctr - 1);
    *(uint16_t *)ctr = nv;
    if ((int32_t)(nv << 16) < 0)
      *(uint16_t *)ctr = 0;                    /* j delay: sh zero */
    else
      deferred_cmd_post(0x400,0xb,0);
  }
  return (int)(*(int16_t *)ctr != old);
}

/* battle_input_dispatch — battle input-edge dispatcher. a0 = guest battle
 * base, a1 = u16 button mask. State byte @(*(u32*)(a0+0x84))[0]
 * selects: 0 -> (mask&0x800) init record + sfx; 1 -> directional
 * repeat/edge handling then action dispatch; other -> negative-state
 * auto-increment. Returns void (v0 always 0). */
extern int32_t pad_handler_c(uint32_t,uint32_t);
extern int32_t pad_handler_a(uint32_t,uint32_t);
extern int32_t pad_handler_b(uint32_t,uint32_t);
void battle_input_dispatch(uint32_t battle, uint32_t mask)
{
  uint32_t btn = mask;
  uint32_t srec;
  int32_t  mode, rc;

  if (MB8(MB32(battle + 0xc0) + 0x1f) == 0) return;
  srec = MB32(battle + 0x84);
  mode  = MBS8(srec);

  if (mode == 0) {
    if ((btn & 0x800) != 0) {
      deferred_cmd_post(0x400,0xe,0);
      srec = MB32(battle + 0x84); MW8(srec,1);
      srec = MB32(battle + 0x84); MW8(srec + 1,0);
      srec = MB32(battle + 0x84); MW8(srec + 2,0);
      srec = MB32(battle + 0x84); MW16(srec + 6,0);
      deferred_cmd_post(0x201,0,0);
    }
    goto L_7a4;
  }
  if (mode != 1) {
    /* lb (srec) < 0 -> srec[0]++ (u8), then s0+0x82 = 1 */
    srec = MB32(battle + 0x84);
    if (MBS8(srec) < 0) MW8(srec,(uint8_t)(MB8(srec) + 1));
    MW16(battle + 0x82,1);
    return;
  }

  /* mode == 1 */
  if (MBS16(srec + 0x14) == 0) {
    if (MBS16(MB32(battle + 0xc8)) != 0) {
      if ((btn & 0x100) != 0) {
        int32_t b2, b3;
        deferred_cmd_post(0x400,0xa,0);
        srec = MB32(battle + 0x84);
        b2 = MBS8(srec + 2); b3 = MBS8(srec + 3);
        if (b2 < b3 - 1) MW8(srec + 2,(uint8_t)(MB8(srec + 2) + 1));
        else             MW8(srec + 2,0);
      }
    } else {
      if ((btn & 0x4100) != 0) {
        int32_t b2, b3;
        deferred_cmd_post(0x400,0xa,0);
        srec = MB32(battle + 0x84);
        b2 = MBS8(srec + 2); b3 = MBS8(srec + 3);
        if (b2 < b3 - 1) MW8(srec + 2,(uint8_t)(MB8(srec + 2) + 1));
        else             MW8(srec + 2,0);
      }
      if ((btn & 0x1000) != 0) {
        int32_t nv;
        deferred_cmd_post(0x400,0xa,0);
        srec = MB32(battle + 0x84);
        nv = (MBS8(srec + 2) == 0) ? MB8(srec + 3) - 1
                                  : MB8(srec + 2) - 1;
        MW8(srec + 2,(uint8_t)nv);
      }
    }
  }

  /* action dispatch on u8 @a0+0x9b / lh @(*(u32*)(a0+0xc8)) */
  if (MB8(battle + 0x9b) == 4)      rc = pad_handler_c(battle,btn & 0xffff);
  else if (MBS16(MB32(battle + 0xc8)) != 0)
                                   rc = pad_handler_a(battle,btn & 0xffff);
  else                              rc = pad_handler_b(battle,btn & 0xffff);

  if (rc != 0) {
    deferred_cmd_post(0x400,0x11,0);
    srec = MB32(battle + 0x84);
    MW8(srec,0xf6); MW8(srec + 1,0); MW8(srec + 2,0);
    MW16(battle + 0x82,1);              /* sh delay slot of jal */
    deferred_cmd_post(0x202,0,0);
    return;
  }
L_7a4:
  srec = MB32(battle + 0x84);
  MW16(battle + 0x82,
       (int8_t)(MB8(srec) ^ MB8(srec + 1)));
}

/* battle_math_init — battle-math struct init: dest a0 gets 7 u32 slots
 * (0x1000,0,0x1000,0,0x1000,0,0,0); sh@+0 = lh(src+0x20)<<12 *
 * 0x66666667 magic /-ish, sh@+8 = same with 0x88888889 + add;
 * then rtps_basis_xform(GTE table 0x8011f8c4 + idx*0x20, dest, dest)
 * where idx = u8 @(*(u32*)(src+0xc0))+0x25. Both args guest u32. */
extern uint *rtps_basis_xform(undefined4 *,undefined2 *,uint *);
void battle_math_init(uint32_t dst, uint32_t src)
{
  uint8_t *d = (uint8_t *)guest_at(dst);
  int32_t  val, quo;
  int64_t  prod;
  uint32_t idx, tab;

  MW32(dst + 0x00,0x1000); MW32(dst + 0x04,0);
  MW32(dst + 0x08,0x1000); MW32(dst + 0x0c,0);
  MW32(dst + 0x10,0x1000); MW32(dst + 0x14,0);
  MW32(dst + 0x18,0);      MW32(dst + 0x1c,0);

  val  = MBS16(src + 0x20) << 12;
  prod = (int64_t)val * (int64_t)(int32_t)0x66666667;
  quo  = (int32_t)((int32_t)(prod >> 32) >> 7) - (val >> 31);
  MW16(dst + 0, (uint16_t)quo);

  val  = MBS16(src + 0x22) << 12;
  prod = (int64_t)val * (int64_t)(int32_t)0x88888889;
  quo  = (int32_t)(((int32_t)(prod >> 32) + val) >> 7) - (val >> 31);
  MW16(dst + 8, (uint16_t)quo);

  idx = MB8(MB32(src + 0xc0) + 0x25);
  tab = 0x8011f8c4u + (idx << 5);
  rtps_basis_xform((undefined4 *)guest_at(tab),
               (undefined2 *)guest_at(dst),
               (uint *)guest_at(dst));
  (void)d;
}

/* status_text_dump — debug/status text dump: ten print_dispatch(format,
 * fields...) calls reading s16/s8 fields of the guest struct a0 and
 * two pointer tables @0x801b1494 / 0x801b148c. a0 = guest u32. */
extern void print_dispatch(const char *,...);
void status_text_dump(uint32_t cam)
{
  uint8_t *camp = (uint8_t *)guest_at(cam);
#define F2(a) ((const char *)guest_at(a))
  print_dispatch(F2(0x8011fb5c),
               MB32(0x801b1494 + (uint32_t)(MBS16(cam + 0x76) << 2)),
               MBS16(cam + 0x78));
  print_dispatch(F2(0x8011fb6c),
               MBS16(0x1f800024),
               MB32(0x801b148c + (uint32_t)(MBS16(cam + 0x7c) << 2)),
               MBS16(cam + 0x7e));
  print_dispatch(F2(0x8011fb84), MBS8(cam + 0x80), MBS8(cam + 0x81));
  print_dispatch(F2(0x8011fb98), MBS16(cam + 0x4a), MBS16(cam + 0x4c),
               MBS16(cam + 0x4e), MBS16(cam + 0x5a));
  print_dispatch(F2(0x8011fbb0), MBS16(cam + 0x28), MBS16(cam + 0x2a),
               MBS16(cam + 0x2c), MBS16(cam + 0x38));
  print_dispatch(F2(0x8011fbc8), MBS16(cam + 0x5c), MBS16(cam + 0x5e),
               MBS16(cam + 0x60), MBS16(cam + 0x6c));
  print_dispatch(F2(0x8011fbe0), MBS16(cam + 0x8a));
  print_dispatch(F2(0x8011fbf4), MBS16(cam + 0x52), MBS16(cam + 0x54),
               MBS16(cam + 0x56));
  print_dispatch(F2(0x8011fc08), MBS16(cam + 0x30), MBS16(cam + 0x32),
               MBS16(cam + 0x34));
  print_dispatch(F2(0x8011fc1c), MBS16(cam + 0x64), MBS16(cam + 0x66),
               MBS16(cam + 0x68));
#undef F2
  (void)camp;
}

/* record_table_select — if a1 <= 0: store table ptr 0x801b15f4 + a0*8 into
 * guest 0x801f04a4 and run clear_f0300_records. */
void record_table_select(int32_t idx, int32_t count)
{
  if (count <= 0) {
    MB32(0x801f04a4) = 0x801b15f4u + ((uint32_t)idx << 3);
    clear_f0300_records();
  }
}

/* s16_dec_with_floor — s16 @a0+0x94 decrement-with-floor: if nonzero,
 * -- and reload -1 (0xffff) once below -0x100. a0 = guest u32. */
void s16_dec_with_floor(uint32_t obj)
{
  uint8_t *p = (uint8_t *)guest_at(obj);
  int16_t  tgt = *(int16_t *)(p + 0x94);
  uint16_t v;
  if (tgt == 0) return;
  v = (uint16_t)(*(uint16_t *)(p + 0x94) - 1);
  *(uint16_t *)(p + 0x94) = v;
  if ((int16_t)v < -0x100) *(uint16_t *)(p + 0x94) = (uint16_t)-1;
}

/* post_battle_side_commit — post-battle per-side commit (a2 = guest battle
 * base; a0/a1 unused by the original). Only runs when s0[0x9c]==2:
 * u16 +0x7c & 0xe0 -> tag 3 + battle_field_set(bc,3); & 8 -> tag 0x20
 * then jump table 0x80120a10 (idx = s16 @bc+0x7e, <8 unsigned)
 * mapping {4,7,5,6,1,3,0,2} into bc+0x7e, then battle_field_set(bc,
 * lh s0+0x96); & 0x100 -> toggle s0[0xb0]. */
void post_battle_side_commit(uint8_t *unused_a, uint8_t *unused_b, uint32_t battle)
{
  uint8_t *bg = (uint8_t *)guest_at(battle);
  uint32_t f7c, bc;
  static const uint8_t jt[8] = {4,7,5,6,1,3,0,2};
  (void)unused_a; (void)unused_b;

  if (bg[0x9c] != 2) return;
  f7c = MB16(battle + 0x7c);
  if ((f7c & 0xe0) != 0) {
    bc = MB32(battle + 0xbc);
    MW16(battle + 0x96, 3);              /* sh $v0,0x96($bg), v0=3 */
    battle_field_set(bc,3);
  } else if ((f7c & 8) != 0) {
    int32_t idx;
    *(uint16_t *)(bg + 0x96) = 0x20;
    bc  = MB32(battle + 0xbc);
    idx = MBS16(bc + 0x7e);
    if ((uint32_t)(int32_t)idx < 8)
      MW16(bc + 0x7e, jt[idx & 7]);
    bc = MB32(battle + 0xbc);
    battle_field_set(bc,(int32_t)*(int16_t *)(bg + 0x96));
  }
  if ((MB16(battle + 0x7c) & 0x100) != 0) bg[0xb0] ^= 1;
}

/* battle_init — battle-init state machine (525 insns). a0 = guest
 * battle base 0x1f800000, a1/a2 = host ptrs into the guest image
 * (0x1f8000e8 / 0x1f8001f0 player blocks; only byte +3 is read).
 * Loop on s6: state 0 = battle_bringup_a gate then engine bring-up
 * (rgb_triple_store, task_pool_init/task_pool_alloc x2 -> 0x801f10c0/10c4,
 * record_init_sweep) and a 2x2 record-writer pass over the two engine
 * pointers; state 6 = three record-table sweeps (2 iters stride
 * 0x28, then 16 iters stride 0x28 from 0x578) plus a per-side
 * 0x8018c824 sprite emit when s5[0x9c]==3; other s6 -> idle.
 * Each pass ends with frame_tick; the sp+0x130 counter reaches 2
 * on the third state-6 pass -> victory_jingle_schedule(p1[3],p2[3]) and
 * s6 = 0xff exit (returns 0xff). Guest frame ptr sp+0x18 is stored
 * to 0x801f10bc; battle_bringup_a gets sp+0x118 (= frame+0x100). */
extern int32_t battle_bringup_a(uint32_t,uint32_t,uint32_t,void *);
extern void task_pool_init(int,int,int,int);
extern int  task_pool_alloc(int,int,int);
extern void record_init_sweep(uint32_t,uint32_t);
extern void palette_attr_record_write(uint32_t,uint32_t,int32_t,uint32_t);
extern void sprite_place_centered(uint32_t,uint32_t,int32_t,int32_t,int32_t,int32_t);
extern void ko_count_announce(uint32_t,uint32_t);
extern void victory_jingle_schedule(uint32_t,uint32_t);
int battle_init(uint32_t battle, uint8_t *side_a, uint8_t *side_b)
{
  /* guest sp-0x168..sp frame image (frame 0x168). Reserve the window on
   * the live guest sp (br2_guest_frame mirrors it into cpu->gpr[29]) so
   * every nested guest call inside the init loop — battle_bringup_a,
   * task_pool_*, record_init_sweep, frame_tick — allocates BELOW this
   * frame instead of overwriting the sp+0x18 argblock that 0x801f10bc
   * and battle_bringup_a's sp+0x118 pointer alias; without the
   * reservation each callee frame landed in this window and the
   * bring-up state corrupted, wedging arcade battle-init at DAT1=3.
   * frame 0x168 is 8 mod 16 so the reservation aligns to 0x170 and
   * guest sp+N maps to sst_frm + N + 8; sp+0x18 -> sst_frm+0x20. */
  uint32_t sst_frm = br2_guest_frame(0x168);
  /* sp+0x18 -> sst_frm+0x20; guest RAM backing so the 0x801f10bc cell
   * carries a real guest address aliasing the same bytes. */
  uint8_t  *arg_blk = (uint8_t *)guest_at(sst_frm + 0x20);
  uint32_t cnt   = 0;                   /* sp+0x130 */
  uint32_t p134  = 0x801b2604;          /* sp+0x134: lh-pair cursor */
  uint32_t p138  = 0xa50;               /* sp+0x138: record offset A */
  uint32_t p13c  = 0;                   /* sp+0x13c: record offset B */
  int32_t  state    = 0;
  uint8_t *p1 = side_a, *p2 = side_b;

  *(uint32_t *)guest_at(0x801f10bc) = guest_of(arg_blk);
  {
    decomp_trace_record(44,battle,(uint32_t)p1[3],(uint32_t)p2[3],0); }

  for (;;) {
    int32_t pi;
    if (state == 0) {
      /* ---------- state 0 ---------- */
      int32_t r, r1, r2;
      r = battle_bringup_a(battle,(uint32_t)p1[3],(uint32_t)p2[3],
                       arg_blk + 0x100);
      if (r == 0) goto L_tail;
      rgb_triple_store(0,0,0);
      task_pool_init(1,2,0x6400,0x801d01ec);
      r1 = task_pool_alloc(1,2,0xbe0);
      MW32(0x801f10c0,r1);              /* jal delay slot */
      r2 = task_pool_alloc(1,2,0xbe0);
      MW32(0x801f10c4,r2);              /* jal delay slot */
      record_init_sweep(battle,0);          /* a1 indeterminate in asm */

      pi = 0;
      do {
        uint8_t *cur = (pi == 0) ? p1 : p2;   /* sp+0x16c / +0x170 */
        uint32_t ctab  = 0x801b25ec + ((uint32_t)pi << 3);
        uint32_t offb  = p13c;
        uint32_t offa = p138;
        uint32_t pidx  = MB8(0x801b23ac + cur[3]);
        uint32_t ptab  = 0x801b260c + (pidx << 2);
        int32_t  j;
        for (j = 0; j < 2; j++) {
          uint32_t base = MB32(0x801f10c0 + (uint32_t)j * 4);
          int32_t stk5, stk6;
          palette_attr_record_write(battle,base + offa,pi,pidx);
          stk5 = MBS16(ptab);     stk6 = MBS16(ptab + 2);
          sprite_place_centered(base + offa,battle,MBS16(p134),0x180,stk5,stk6);
          stk5 = MBS16(ctab + 4); stk6 = MBS16(ctab + 6);
          sprite_place_centered(base + offb + 0xaf0,battle,MBS16(ctab),0x180,
                       stk5,stk6);
          if (pi == 0) MW16(base + 0xafe,0x7d1e);
          else         MW16(offb + base + 0xafe,0x7e5e);
        }
        p134 += 4; p138 += 0x28; p13c += 0x28; pi++;
      } while (pi < 2);

      if (MB8(battle + 0x9c) == 3) {
        /* pull 4 corner u16s (-0x18) of each side record: per outer
         * iter (t1 = 0x801f10c0+4*i) two rounds over three offset
         * groups {0xa0,0xa50,0xaf0}+0x28, then one +0xf0 group. */
        uint32_t otcur = 0x801f10c0;
        int32_t  i1 = 0;
        do {
          uint32_t osnap = otcur;
          uint32_t o1 = 0xa0, o2 = 0xa50, o3 = 0xaf0;
          int32_t  k = 0;
          do {
            uint32_t tbl_b;
            tbl_b = MB32(osnap) + o1;
            MW16(tbl_b + 0xa,  (uint16_t)(MB16(tbl_b + 0xa)  - 0x18));
            MW16(tbl_b + 0x12, (uint16_t)(MB16(tbl_b + 0x12) - 0x18));
            MW16(tbl_b + 0x1a, (uint16_t)(MB16(tbl_b + 0x1a) - 0x18));
            MW16(tbl_b + 0x22, (uint16_t)(MB16(tbl_b + 0x22) - 0x18));
            o1 += 0x28;
            tbl_b = MB32(osnap) + o2;
            MW16(tbl_b + 0xa,  (uint16_t)(MB16(tbl_b + 0xa)  - 0x18));
            MW16(tbl_b + 0x12, (uint16_t)(MB16(tbl_b + 0x12) - 0x18));
            MW16(tbl_b + 0x1a, (uint16_t)(MB16(tbl_b + 0x1a) - 0x18));
            MW16(tbl_b + 0x22, (uint16_t)(MB16(tbl_b + 0x22) - 0x18));
            o2 += 0x28;
            tbl_b = MB32(osnap) + o3;
            MW16(tbl_b + 0xa,  (uint16_t)(MB16(tbl_b + 0xa)  - 0x18));
            MW16(tbl_b + 0x12, (uint16_t)(MB16(tbl_b + 0x12) - 0x18));
            MW16(tbl_b + 0x1a, (uint16_t)(MB16(tbl_b + 0x1a) - 0x18));
            MW16(tbl_b + 0x22, (uint16_t)(MB16(tbl_b + 0x22) - 0x18));
            o3 += 0x28;
            k++;
          } while (k < 2);
          {
            uint32_t tbl_b = MB32(otcur) + 0xf0;
            MW16(tbl_b + 0xa,  (uint16_t)(MB16(tbl_b + 0xa)  - 0x18));
            MW16(tbl_b + 0x12, (uint16_t)(MB16(tbl_b + 0x12) - 0x18));
            MW16(tbl_b + 0x1a, (uint16_t)(MB16(tbl_b + 0x1a) - 0x18));
            MW16(tbl_b + 0x22, (uint16_t)(MB16(tbl_b + 0x22) - 0x18));
          }
          otcur += 4; i1++;
        } while (i1 < 2);
      }
      state = 6;
      goto L_tail;

    } else if (state == 6) {
      /* ---------- state 6 ---------- */
      const uint32_t T = 0x801b2378u;
      uint32_t a1o = 0;
      int32_t  i;
      for (i = 0; i < 2; i++) {
        uint32_t ao = 0xa0  + (uint32_t)i * 0x28;
        uint32_t to = 0xa50 + (uint32_t)i * 0x28;
        uint32_t uo = 0xaf0 + (uint32_t)i * 0x28;
        uint32_t ic, ib, baseA, arrC, arrA;
        /* record A: idx T+4, OR-slot literal 0xa0, mask off ao */
        ic   = MB32(battle + 0xc);
        ib   = (uint32_t)(MBS16(T + 4) << 2);
        baseA= MB32(battle + 8);
        arrC = MB32(0x801f10c0 + ic * 4);
        arrA = MB32(baseA + ib);
        MW32(a1o + arrC + 0xa0, arrA | 0x09000000u);
        ic   = MB32(battle + 0xc);
        ib   = (uint32_t)(MBS16(T + 4) << 2);
        baseA= MB32(battle + 8);
        arrC = MB32(0x801f10c0 + ic * 4);
        MW32(baseA + ib, (arrC + ao) & 0x00ffffffu);
        if (i == 1) {
          /* trailing 0xf0 record (third slot, fixed offsets) */
          ic   = MB32(battle + 0xc);
          ib   = (uint32_t)(MBS16(T + 4) << 2);
          baseA= MB32(battle + 8);
          arrC = MB32(0x801f10c0 + ic * 4);
          arrA = MB32(baseA + ib);
          MW32(arrC + 0xf0, arrA | 0x09000000u);  /* sw 0xf0(v1), no a1 */
          ic   = MB32(battle + 0xc);
          ib   = (uint32_t)(MBS16(T + 4) << 2);
          baseA= MB32(battle + 8);
          arrC = MB32(0x801f10c0 + ic * 4);
          MW32(baseA + ib, (arrC + 0xf0) & 0x00ffffffu);
        }
        /* record B: idx T+0xa, OR-slot literal 0xa50, mask off to */
        ic   = MB32(battle + 0xc);
        ib   = (uint32_t)(MBS16(T + 0xa) << 2);
        baseA= MB32(battle + 8);
        arrC = MB32(0x801f10c0 + ic * 4);
        arrA = MB32(baseA + ib);
        MW32(a1o + arrC + 0xa50, arrA | 0x09000000u);
        ic   = MB32(battle + 0xc);
        ib   = (uint32_t)(MBS16(T + 0xa) << 2);
        baseA= MB32(battle + 8);
        arrC = MB32(0x801f10c0 + ic * 4);
        MW32(baseA + ib, (arrC + to) & 0x00ffffffu);
        /* record C: idx T+0xa, OR-slot literal 0xaf0, mask off uo */
        ic   = MB32(battle + 0xc);
        ib   = (uint32_t)(MBS16(T + 0xa) << 2);
        baseA= MB32(battle + 8);
        arrC = MB32(0x801f10c0 + ic * 4);
        arrA = MB32(baseA + ib);
        MW32(a1o + arrC + 0xaf0, arrA | 0x09000000u);
        a1o += 0x28;                        /* jal-free delay ordering */
        ic   = MB32(battle + 0xc);
        ib   = (uint32_t)(MBS16(T + 0xa) << 2);
        baseA= MB32(battle + 8);
        arrC = MB32(0x801f10c0 + ic * 4);
        MW32(baseA + ib, (arrC + uo) & 0x00ffffffu);
      }
      /* 16-record sweep: idx T+0x10, OR addr = j + arrC + 0x578,
       * mask off 0x578 + j*0x28 */
      {
        uint32_t a2o = 0x578;
        int32_t  j;
        for (j = 0; j < 16; j++) {
          uint32_t ic,id,baseA,arrC,arrA;
          ic    = MB32(battle + 0xc);
          id    = (uint32_t)(MBS16(T + 0x10) << 2);
          baseA = MB32(battle + 8);
          arrC  = MB32(0x801f10c0 + ic * 4);
          arrA  = MB32(baseA + id);
          MW32((uint32_t)j * 0x28 + arrC + 0x578, arrA | 0x09000000u);
          ic    = MB32(battle + 0xc);
          id    = (uint32_t)(MBS16(T + 0x10) << 2);
          baseA = MB32(battle + 8);
          arrC  = MB32(0x801f10c0 + ic * 4);
          MW32(baseA + id, (arrC + a2o) & 0x00ffffffu);
          a2o += 0x28;
        }
      }
      /* trailer record: idx T+0x12, fixed 0xb68 offsets */
      {
        uint32_t ic,id,baseA,arrC,arrA;
        ic    = MB32(battle + 0xc);
        id    = (uint32_t)(MBS16(T + 0x12) << 2);
        baseA = MB32(battle + 8);
        arrC  = MB32(0x801f10c0 + ic * 4);
        arrA  = MB32(baseA + id);
        MW32(arrC + 0xb68, arrA | 0x09000000u);
        ic    = MB32(battle + 0xc);
        id    = (uint32_t)(MBS16(T + 0x12) << 2);
        baseA = MB32(battle + 8);
        arrC  = MB32(0x801f10c0 + ic * 4);
        MW32(baseA + id, (arrC + 0xb68) & 0x00ffffffu);
        ko_count_announce(battle, baseA + id);
      }

      if (MB8(battle + 0x9c) == 3) {
        int32_t remain = MBS16(battle + 0xa2);
        int32_t pos, batch;
        if (remain < 9) { pos = 0;     batch = remain; }
        else        { pos = remain-8; batch = 8; }
        if (batch > 0) {
          int32_t  k = 0;
          uint32_t attr_a = 0x180000, attr_b = 0x10;
          do {
            /* pos % 11 via 0x2e8ba2e9 magic (mult hi) */
            int64_t  prod = (int64_t)pos * (int64_t)(int32_t)0x2e8ba2e9;
            int32_t  quo  = (int32_t)((int32_t)(prod >> 32) >> 1) - (pos >> 31);
            int32_t  rem = pos - quo * 11;
            uint32_t pal_b   = MB8(MB32(battle + 0xa8) + (uint32_t)rem);
            uint32_t ot_ptr = MB32(battle + 8) + 4;
            uint32_t attr_c = ((pal_b + 0x1e0) & 0x1ff) << 22;
            attr_c |= ((((pal_b >> 2) << 6) + 1) & 0xff) << 8;
            attr_c |= (pal_b & 3) << 6;
            spr_slot_pkt_submit((uint *)guest_at(ot_ptr),
                         0x180000u | (attr_b & 0xffff),0x3e379d,attr_c);
            if (k < batch) {
              announce_param_block_init(MB32(battle + 8),MB32(battle + 0xc),
                           (const void *)guest_at(0x80126e8),
                           (uint32_t)((int32_t)attr_a >> 16),0x198,0);
            }
            attr_a += 0x3a; attr_b += 0x3a; k++; pos++;
          } while (k < batch);
        }
      }
L_e7c:
      if (cnt >= 2) {
        victory_jingle_schedule((uint32_t)p1[3],(uint32_t)p2[3]);
        state = 0xff;                          /* jal delay slot */
      }
      cnt++;
    } else {
      pi = 0;                               /* unused: straight to tail */
    }
L_tail:
    frame_tick((intptr_t)battle);
    if (state == 0xff) break;
  }
  br2_guest_frame_free();
  return 0xff;
}

/* ======== batch33 ======== */

/* shared callees not yet declared in this file */
extern uint8_t *tag_table_get(void);
extern void scratch_013_set(int32_t);
extern void spr_draw_pair(uint32_t,uint32_t,uint32_t,uint32_t,
                         uint32_t,uint32_t,uint32_t);
extern void menu_step_b(uint32_t,uint32_t,uint32_t,uint32_t);
extern void status_text_dump(uint32_t);
extern int  ret_zero_e(int32_t);
extern void string_pair_emit(uint32_t,uint32_t,uint32_t,uint32_t);
extern int32_t menu_step_a(uint32_t,int32_t,uint32_t);
extern int  text_render_full(uint32_t *,uint32_t,int32_t);

/* text-line emit helper shape shared by the three input handlers:
 * hud_text_blk_alt(heapA,heapB,fmt-string guest ptr,a3,stk5,stk6) where the
 * asm sign-extends every 16-bit field via sll 16 / sra 16. */
#define BR2_E078(b,pt,av,wv,cv) \
  hud_text_blk_alt(MB32((b)+8),MB32((b)+0xc),(const void *)guest_at(pt), \
              (int32_t)(int16_t)(av),(int32_t)(int16_t)(wv),(int32_t)(cv))
/* spr_attr_alloc_a(heapA,dy,dx,attr,rows,0x201010,tail) — 3 stack args. */
#define BR2_CC6C(b,av1,av2,av3,wv,fv,gv) \
  spr_attr_alloc_a(MB32((b)+8),(int16_t)(av1),(int16_t)(av2),(int16_t)(av3), \
               (int16_t)(wv),(uint32_t)(fv),(int32_t)(gv))

/* ======== 0x8012e8e8 ========
 * Round/announce param block @0x1f80038c (sibling of announce_param_block_init in
 * battle.c with different field constants). a0 = ctx (heap A ptr),
 * a1 = ctx2 (u16), a2 = host string ptr (stored back as guest addr),
 * a3 = u16 field, stack5 = u16 field8, stack6 = packed attr for +0x16.
 * Tail: text_render_full(ctx, ctx2 & 0xffff, 0x1f80038c). */
void round_announce_block(uint32_t ctx,uint32_t ctx2,const void *str,
                  uint32_t fld6,uint32_t fld8,int32_t attr)
{
  MW32(0x1f8003a8, guest_of(str));
  MW16(0x1f80038c + 6,    (uint16_t)fld6);
  MW16(0x1f80038c + 4,    7);
  MW16(0x1f80038c + 0xe,  0xffff);
  MW16(0x1f80038c + 0x12, 1);
  MW16(0x1f80038c + 0x14, 1);
  MW16(0x1f80038c + 0,    0);
  MW16(0x1f80038c + 2,    0);
  MW16(0x1f80038c + 0xa,  8);
  MW16(0x1f80038c + 0xc,  8);
  MW16(0x1f80038c + 0x10, 8);
  MW16(0x1f80038c + 0x18, 5);
  MW16(0x1f80038c + 8,    (uint16_t)fld8);
  MW16(0x1f80038c + 0x16,
       (uint16_t)((((uint32_t)attr + 0x1e0) & 0x1ff) * 0x40 + 0x16));
  text_render_full((uint32_t *)guest_at(ctx), ctx2 & 0xffff,
               0x1f80038c);
}

/* ======== 0x80148170 ========
 * Battle bring-up gate, 222 insns. a0 = guest battle base, a1/a2 = the
 * two side-record selector bytes (p1[3]/p2[3]), a3 unused. s5+0x8c is a
 * signed state halfword dispatched through the 0x801203f8 jump table
 * (cases 0,8,0x10,0x20,0x30; all other values store-and-return 0).
 * Returns -1 when the state halfword is negative, 1 after the full
 * heap/tag-table load (state -> -1), else 0. */
int32_t battle_bringup_a(uint32_t battle,uint32_t sel_a,uint32_t sel_b,
                     void *unused)
{
  uint32_t dlen;
  uint32_t ttab, state, res_id;
  int32_t  cur_st;
  (void)unused;

  ttab = guest_of(tag_table_get());       /* tag table 0x801c47c0 */
  cur_st = MBS16(battle + 0x8c);
  {
    decomp_trace_record(43,battle,sel_a,sel_b,(uint32_t)(int16_t)cur_st); }
  if (cur_st < 0) return -1;
  state = (uint32_t)cur_st;
  if (state >= 0x31) goto L_store;

  switch (state) {
  case 0:
    cd_file_load_sm(0xffffffffu,0);
    MW32(0x801c43a0,0);
    MW8(0x801e3d00,0xff);
    MW32(0x801c47b8,0x80010000);
    MW32(0x801e3cfc,0x800dc800);
    state = 8;
    /* fall through: case 8 enters at the shared tail */
  case 8:

    if (cd_file_load_sm(0x8c, MB32(0x801c47b8)) == 0) goto L_store;
    fighter_record_apply(MB32(0x801c47b8),2);
    state = 0x10;                            /* jal delay slot */
    page_table_ptr_set(0x11,2,(undefined *)0);
    cd_timer_read(0);
    res_id = (uint32_t)(MB8(0x801b49f8 + sel_a) + 0xc7);
    goto L_bchk;
  case 0x10:
    res_id = (uint32_t)(MB8(0x801b49f8 + sel_a) + 0xc7);
  L_bchk:
    if (cd_file_load_sm(res_id, ttab) == 0) goto L_store;
    goto L_mid;
  case 0x20:
    res_id = (sel_b == 8) ? 0x3f : MB8(0x801b49f8 + sel_b);
    res_id += 0xc7;
    if (cd_file_load_sm(res_id, ttab) == 0) goto L_store;
    scratch_013_set(0);
    spr_draw_pair(ttab,0,0x1f6,1,0x260,0,1);
    cd_timer_read(0);
    scratch_013_set(1);
    state = 0x30;
    /* fall through into the 0x81 sound gate */
  case 0x30:
    if (cd_file_load_sm(0x81, ttab) == 0) goto L_store;
    goto L_tail48;
  default:
    goto L_store;
  }

L_mid:                                        /* 0x80148280 */
  scratch_013_set(0);
  spr_draw_pair(ttab,0,0x1f5,1,0x200,0,1);       /* arg7 = state = 1 */
  if (sel_a == sel_b && sel_b != 8) {
    spr_draw_pair(ttab,0,0x1f6,1,0x260,0,1);
    state = 0x30;
  } else {
    state = 0x20;
  }
  cd_timer_read(0);
  scratch_013_set(1);
  MW16(battle + 0x8c,(uint16_t)state);              /* j delay slot */
  return 0;

L_tail48:                                     /* 0x80148394 */
  scratch_013_set(0);
  dlen = stream_block_decode(MB32(0x801c47b8),(int *)guest_at(ttab),0x31);
  spr_draw_off(MB32(0x801c47b8),0x300,0x100,1);
  MW32(0x801c47b8, MB32(0x801c47b8) + dlen);   /* delay-slot store */
  dlen = stream_block_decode(MB32(0x801c47b8),(int *)guest_at(ttab),0x32);
  spr_draw_off(MB32(0x801c47b8),0x280,0x100,1);
  MW32(0x801c47b8, MB32(0x801c47b8) + dlen);
  dlen = stream_block_decode(MB32(0x801c47b8),(int *)guest_at(ttab),0x30);
  spr_draw_off(MB32(0x801c47b8),0,0x1e0,1);
  {
    uint32_t rA = dlen;                        /* heapA still owes r3 */
    dlen = stream_block_decode(MB32(0x801e3cfc),(int *)guest_at(ttab),0x34);
    MW32(0x801c47b8, MB32(0x801c47b8) + rA);
    spr_draw_off(MB32(0x801e3cfc),0x2c0,0,1);
    MW32(0x801e3cfc, MB32(0x801e3cfc) + dlen);
    dlen = stream_block_decode(MB32(0x801e3cfc),(int *)guest_at(ttab),0x35);
    spr_draw_off(MB32(0x801e3cfc),0x2c0,0x100,1);
    MW32(0x801e3cfc, MB32(0x801e3cfc) + dlen);
  }
  cd_timer_read(0);
  scratch_013_set(1);
  MW16(battle + 0x8c,0xffff);                    /* j delay slot */
  return 1;

L_store:                                     /* 0x801484bc */
  MW16(battle + 0x8c,(uint16_t)state);
  return 0;
}

/* ======== 0x80148f58 ========
 * Per-record palette/attr writer, 111 insns. a0 = passthrough ctx, a1 =
 * guest record ptr, a2 = table index (0x801b2604 stride-4 pair selector),
 * a3 = 0x801b260c/2650/2658 index. For a3 in [2,12] (jump table
 * 0x801204d8) writes a packed u16 at rec+0x16 from the 0x801b2648
 * palette halfwords, then always runs sprite_place_centered + the byte-sum
 * fields at rec+0xc..0x25 and the packed u16 at rec+0xe. */
void palette_attr_record_write(uint32_t ctx,uint32_t rec,int32_t tbl_idx,
                  uint32_t slot)
{


  uint32_t btab = 0x801b2658 + slot * 2;
  uint32_t atab = 0x801b2650 + (uint32_t)tbl_idx * 4;
  uint32_t ptab = 0x801b260c + slot * 4;
  int32_t  idx = slot - 2;
  uint32_t attr, hibit, a0p;

  if ((uint32_t)idx < 0xb) {
    /* jump table: entries {0,1,7,9}->0x80148fd4 use +4/+6 halfwords,
     * the rest ->0x80148fec use +0/+2 halfwords of 0x801b2648. */
    if (idx == 0 || idx == 1 || idx == 7 || idx == 9) {
      attr = MB16(0x801b264c) & 0x3ff;
      hibit = MB16(0x801b264e) & 0x100;
    } else {
      attr = MB16(0x801b2648) & 0x3ff;
      hibit = MB16(0x801b264a) & 0x100;
    }
    attr = (attr >> 6) + (((hibit << 16) >> 20) & 0xffff);
    /* (hibit&0x100)<<16 >>a20 == (hibit&0x100) ? 0x10 : 0 */
    MW16(rec + 0x16,(uint16_t)attr);
  }

  a0p = 0x801b2604 + (uint32_t)tbl_idx * 4;
  sprite_place_centered(rec,ctx,
               MBS16(a0p),MBS16(a0p + 2),
               MBS16(ptab),MBS16(ptab + 2));

  MW8(rec + 0xc, MB8(btab));
  MW8(rec + 0xd, MB8(btab + 1));
  MW8(rec + 0x14,(uint8_t)(MB8(btab) + MB8(ptab)));
  MW8(rec + 0x15, MB8(btab + 1));
  MW8(rec + 0x1c, MB8(btab));
  MW8(rec + 0x1d,(uint8_t)(MB8(btab + 1) + MB8(ptab + 2)));
  MW8(rec + 0x24,(uint8_t)(MB8(btab) + MB8(ptab)));
  MW8(rec + 0x25,(uint8_t)(MB8(btab + 1) + MB8(ptab + 2)));
  MW16(rec + 0xe,
       (uint16_t)(((MB16(atab) & 0x3ff) >> 4) + ((MB16(atab + 2) & 0x1ff) << 6)));
}

/* ======== 0x8014a804 ========
 * Two-iteration victory jingle scheduler, 69 insns. a0/a1 = the two
 * side ids (p1[3]/p2[3]); second pass always uses a1. Per pass emits
 * six deferred_cmd_post(0x400,...) notes at record offsets derived from the
 * id*6, accumulating MB8(0x801b23bc + id*2) / +1 into the pitch.
 * id==10 is remapped to 2 between the two halves. */
void victory_jingle_schedule(uint32_t id_a,uint32_t id_b)
{

  uint32_t acc = 0;
  int32_t  i;

  for (i = 0; i < 2; i++) {
    uint32_t id2, id6;
    id2 = id_a << 1;
    id6 = (id2 + id_a) << 1;                  /* id * 6 */
    deferred_cmd_post(0x400,(int32_t)(id6 + 0x80),(int32_t)acc);
    deferred_cmd_post(0x400,(int32_t)(id6 + 0x81),(int32_t)(acc + 7));
    deferred_cmd_post(0x400,(int32_t)(id6 + 0x82),(int32_t)(acc + 0xe));
    id2 += 0x801b23bc;
    acc += MB8(id2);
    if (id_a == 10) id_a = 2;                 /* bne delay: a0=0x400 */
    id2 = id_a << 1;
    id6 = (id2 + id_a) << 1;
    deferred_cmd_post(0x400,(int32_t)(id6 + 0x83),(int32_t)acc);
    deferred_cmd_post(0x400,(int32_t)(id6 + 0x84),(int32_t)(acc + 7));
    deferred_cmd_post(0x400,(int32_t)(id6 + 0x85),(int32_t)(acc + 0xe));
    id2 += 0x801b23bc;
    acc += MB8(id2 + 1);
    id_a = id_b;                              /* loop-branch delay slot */
  }
}

/* ======== 0x80150f04 ========
 * KO/count announcement line emitter, 142 insns. a0 = guest battle
 * base, a1 = guest text-list ptr. State byte +0x9c (<6) selects one of
 * six layouts through the 0x80120738 jump table; each either prints a
 * formatted string via sprintf_core into a stack buf then emits it, or
 * emits a rodata string directly through announce_param_block_init_b. The y/attr pair
 * always derives from lh +0x20 (row) and lh +0x22 (col/16). */
void ko_count_announce(uint32_t battle,uint32_t text_list)
{

  char     buf[0x20];
  int32_t  ko_cnt, t, w, a1x, K;
  uint32_t sel, fmt;
  const void *p;
  int32_t  stk6;
  int      spr;

  ko_cnt  = MBS16(battle + 0xa2) + 1;
  sel = MB8(battle + 0x9c);
  if (sel >= 6) return;

  p = NULL; fmt = 0; stk6 = 4; spr = 0; K = 0;
  if (sel == 0) {
    if (ko_cnt == 9) {
      K = -0xb0; p = (const void *)guest_at(0x801206ec);
    } else if (ko_cnt < MB8(MB32(battle + 0xc0) + 0x10)) {
      spr = 1; fmt = 0x80120708; K = -0x60;
    } else {
      K = -0x90; p = (const void *)guest_at(0x801206fc);
    }
  } else if (sel <= 2) {
    spr = 1; fmt = 0x8012072c; K = -0x60;
  } else if (sel <= 4) {
    if (ko_cnt < MB8(MB32(battle + 0xc0) + 0x10)) {
      uint32_t mode_b = MB8(battle + 0x9b);
      if (mode_b == 4) {
        K = -0x70; p = (const void *)guest_at(0x80120720); stk6 = (int32_t)mode_b;
      } else {
        spr = 1; fmt = 0x8012072c; K = -0x60;
      }
    } else {
      K = -0x90; p = (const void *)guest_at(0x80120714);
    }
  } else {                                   /* sel == 5 */
    uint32_t mode_b = MB8(battle + 0x9b);
    if (mode_b == 4) {
      K = -0x70; p = (const void *)guest_at(0x80120720); stk6 = (int32_t)mode_b;
    } else {
      spr = 1; fmt = 0x8012072c; K = -0x60;
    }
  }

  if (spr) {
    sprintf_core(buf,(char *)guest_at(fmt),ko_cnt);
    p = (const void *)buf;
  }

  /* shared tail math: t = trunc2(lh20 + K), w = trunc16(lh22) */
  {
    int32_t v_raw = MBS16(battle + 0x20) + K;
    int32_t v_adj = v_raw + (int32_t)((uint32_t)v_raw >> 31);
    t = (v_adj << 15) >> 16;
    a1x = MBS16(battle + 0x22);
    if (a1x < 0) a1x += 0xf;
    w = a1x >> 4;
  }
  announce_param_block_init_b(text_list, MB32(battle + 0xc), p, (uint16_t)t, (uint16_t)w, stk6);
}

/* ======== 0x80161c54 ========
 * Battle input handler state 1, 395 insns. a0 = guest battle base,
 * a1 = u16 pad mask. When obj+0x84->0x14 == 0 and the &0xe0/&2 mask
 * check passes it emits a six-line hud_text_blk_alt announce block plus a
 * spr_attr_alloc_a bar; then dispatches on lb (obj+0x84)+2 (states 0..3):
 * 0 = confirm beep store, 1 = menu_step_b + strings, 2 = d-pad held
 * redraw block, 3 = shoulder-button page cycle. Returns 1 only via the
 * mask&0x800 early-out. */
int32_t pad_handler_a(uint32_t battle,uint32_t mask)
{

  /* lhu a1 */
  uint32_t en = 1, row, col, gap, dy, yb, xc, yc;
  uint32_t btns, p84;
  int32_t  lb2;

  /* row = trunc2(lh20 - 0x40), col = trunc4(lh22) */
  {
    int32_t x = MBS16(battle + 0x20) - 0x40;
    x += (int32_t)((uint32_t)x >> 31);
    row = (uint32_t)(x >> 1);             /* srav by s7 == 1 here */
    {
      int32_t y = MBS16(battle + 0x22);
      if (y < 0) y += 3;
      col = (uint32_t)(y >> 2);
    }
  }
  gap = (MB8(battle + 0x15) != 0) ? 0x10 : 8;
  btns = MB16(battle + 0x80);
  if ((btns & 0xe0) == 0xe0) en = 0;
  if (btns & 2) en = 0;
  p84 = MB32(battle + 0x84);
  if (MBS16(p84 + 0x14) != 0) goto L_dispatch;
  if (en == 0) goto L_chk800;

  /* ---- announce init block ---- */
  BR2_E078(battle,0x80121d50,(int32_t)(int16_t)(row + 0x10),col,4);
  yb = col + gap;
  xc = row + 8;
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  BR2_E078(battle,0x80121d58,(int32_t)(int16_t)xc,
           (int32_t)(int16_t)yb,(uint32_t)lb2 < 1);
  dy = gap << 1;
  BR2_E078(battle,0x80121d60,(int32_t)(int16_t)xc,
           (int32_t)(int16_t)(col + dy),(uint32_t)(lb2 ^ 1) < 1);
  dy += gap;
  BR2_E078(battle,0x80121d68,(int32_t)(int16_t)xc,
           (int32_t)(int16_t)(col + dy),(uint32_t)(lb2 ^ 2) < 1);
  {
    uint32_t c0 = MB32(battle + 0xc0);
    uint32_t dw_ent = MB32(0x801b4008 + (uint32_t)MB8(c0 + 0xb) * 4);
    BR2_E078(battle,dw_ent,(int32_t)(int16_t)xc,
             (int32_t)(int16_t)(col + gap * 4),(uint32_t)(lb2 ^ 3) < 1);
  }
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  BR2_E078(battle,0x80121270,(int32_t)(int16_t)row,
           (int32_t)(int16_t)(yb + gap * (uint32_t)lb2),1);
  BR2_CC6C(battle,row - 4,col - (gap >> 1),0x48,dy << 1,0x201010,0);

L_chk800:
  if (MBS16(MB32(battle + 0x84) + 0x14) != 0) goto L_dispatch;
  if (mask & 0x800) return 1;

L_dispatch:
  p84 = MB32(battle + 0x84);
  lb2 = MBS8(p84 + 2);
  if (lb2 == 1) {
    /* ---- case 1: partner record commit ---- */
    menu_step_b(MB32(battle + 0xbc),mask,0x1f8001f0,battle);
    MW16(battle + 0x96,MB16(MB32(battle + 0xbc) + 0x76));
    if (en != 0) {
      print_dispatch((const char *)guest_at(0x80121d70));
      status_text_dump(MB32(battle + 0xbc));
    }
    return 0;
  }
  if (lb2 == 0) {
    /* ---- case 0: confirm store ---- */
    if (MBS16(MB32(battle + 0xc8)) != 0 && (MB16(battle + 0x7e) & 1) != 0)
      MW8(p84 + 1,1);
    else
      MW8(MB32(battle + 0x84) + 1,0);
    return 0;
  }
  if (lb2 == 2) {
    /* ---- case 2: held-dpad redraw ---- */
    int32_t lh12;
    if (MBS16(p84 + 0x14) == 0) goto L_2194;
    if (mask & 0xa000) {
      uint32_t sub84;
      deferred_cmd_post(0x400,0xa,0);
      sub84 = MB32(battle + 0x84);
      MW16(sub84 + 0x12,(uint16_t)(MB16(sub84 + 0x12) ^ 1));
    }
    {
      int32_t x = MBS16(battle + 0x20) - 0x40;
      int32_t y;
      x += (int32_t)((uint32_t)x >> 31);
      row = (uint32_t)(x >> 1);
      y = MBS16(battle + 0x22);
      if (y < 0) y += 3;
      col = (uint32_t)(y >> 2);
    }
    BR2_E078(battle,0x80121d68,(int32_t)(int16_t)(row + 0x10),col,4);
    yc = (int32_t)(int16_t)(col + gap);
    lh12 = MBS16(MB32(battle + 0x84) + 0x12);
    BR2_E078(battle,0x8012181c,(int32_t)(int16_t)row,yc,(uint32_t)lh12 < 1);
    BR2_E078(battle,0x80121820,(int32_t)(int16_t)(row + 0x30),yc,
             (uint32_t)lh12 != 0);
    BR2_CC6C(battle,row - 4,col - (gap >> 1),0x48,gap * 3,0x201010,0);
    if (mask & 0x10) {
      uint32_t sub84;
      deferred_cmd_post(0x400,0xd,0);
      sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,0);
      sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,0);
    }
    if (mask & 0xe0) {
      lh12 = MBS16(MB32(battle + 0x84) + 0x12);
      if (lh12 == 0) {
        deferred_cmd_post(0x400,0xc,0);
        MB8(battle + 1) = 9;                  /* j delay slot */
      } else {
        uint32_t sub84;
        deferred_cmd_post(0x400,0xd,0);
        sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,0);
        sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,0);
      }
    }
    return 0;
  }
  if (lb2 == 3) {
    /* ---- case 3: shoulder page cycle ---- */
    uint32_t d;
    if (mask & 0x2000) {
      deferred_cmd_post(0x400,0xa,0);
      d = (uint32_t)(MB8(MB32(battle + 0xc0) + 0xb) + 1);
    } else if (mask & 0x8000) {
      deferred_cmd_post(0x400,0xa,0);
      d = (uint32_t)(MB8(MB32(battle + 0xc0) + 0xb) - 1);
    } else {
      return 0;
    }
    d &= 7;
    MW8(MB32(battle + 0xc0) + 0xb,(uint8_t)d);
    display_mode_setup(MB8(MB32(battle + 0xc0) + 0xb));
    frame_advance_helper();
    battle_math_init(MB32(battle + 0xbc),battle);
    return 0;
  }
  return 0;

L_2194:                                        /* case2, lh14 == 0 */
  if ((mask & 0xe0) == 0) return 0;
  {
    uint32_t sub84;
    deferred_cmd_post(0x400,0xc,0);
    sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,1);
    sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,1);
  }
  return 0;
}

/* ======== 0x80162280 ========
 * Battle input handler state !=0/!=1, 366 insns. Same skeleton as
 * pad_handler_a but keyed on the (m80 & 2) flag instead of the 0xe0
 * mask, with a wider announce block (five hud_text_blk_alt lines + bar).
 * Dispatch handles lb (obj+0x84)+2 states 0,1,2 only; state 1 commits
 * via ret_zero_e + string_pair_emit when (mask & 0xe0) and +0x9c != 2. */
int32_t pad_handler_b(uint32_t battle,uint32_t mask)
{

  /* pad mask */
  uint32_t en = 1, sely = 0;
  uint32_t row, col, gap, emode, yc;
  uint32_t p84;
  int32_t  lbv, lh12;

  /* row = trunc2(lh20 - 0x48), col = trunc4(lh22) */
  {
    int32_t x = MBS16(battle + 0x20) - 0x48;
    x += (int32_t)((uint32_t)x >> 31);
    row = (uint32_t)(x >> 1);             /* srav by a0 == 1 */
    {
      int32_t y = MBS16(battle + 0x22);
      if (y < 0) y += 3;
      col = (uint32_t)(y >> 2);
    }
  }
  gap = (MB8(battle + 0x15) != 0) ? 0x10 : 8;
  if (MB16(battle + 0x80) & 2) en = 0;
  p84 = MB32(battle + 0x84);
  if (MBS16(p84 + 0x14) != 0) goto L_dispatch;
  if (en == 0) goto L_chk800;

  /* ---- announce init block ---- */
  BR2_E078(battle,0x80121d50,(int32_t)(int16_t)(row + 0x14),col,4);
  lbv = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lbv == 0) { emode = 1; sely = col + gap; }
  BR2_E078(battle,0x80121d58,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap),emode);
  lbv = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lbv == 1) { emode = 1; sely = col + (gap << 1); }
  if (MB8(battle + 0x9c) == 2) emode = 2;
  BR2_E078(battle,0x80121d74,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 2),emode);
  yc = (int32_t)(int16_t)(col + gap * 3);
  BR2_E078(battle,0x80121d80,(int32_t)(int16_t)(row + 8),yc,emode);
  lbv = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lbv == 2) { emode = 1; sely = col + (gap << 2); }
  BR2_E078(battle,0x80121d68,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 4),emode);
  BR2_E078(battle,0x80121270,(int32_t)(int16_t)row,
           (int32_t)(int16_t)sely,1);
  BR2_CC6C(battle,row - 4,col - (gap >> 1),0x58,gap * 6,0x201010,3);

L_chk800:
  if (MBS16(MB32(battle + 0x84) + 0x14) != 0) goto L_dispatch;
  if (mask & 0x800) return 1;

L_dispatch:
  p84 = MB32(battle + 0x84);
  lbv = MBS8(p84 + 2);
  if (lbv == 1) {
    /* ---- state 1 ---- */
    if ((mask & 0xe0) == 0) return 0;
    if (MB8(battle + 0x9c) == 2) return 0;
    deferred_cmd_post(0x400,0xc,0);
    ret_zero_e(1);
    string_pair_emit(mask,0x1f8000e8,battle,MB32(battle + 0xc0));
    return 0;
  }
  if (lbv == 0) {
    /* ---- state 0 ---- */
    if ((mask & 0xe0) != 0) return 0;
    if (MBS16(MB32(battle + 0xc8)) != 0 && (MB16(battle + 0x7e) & 1) != 0)
      MW8(p84 + 1,1);
    else
      MW8(MB32(battle + 0x84) + 1,0);
    return 0;
  }
  if (lbv == 2) {
    /* ---- state 2 ---- */
    if (MBS16(p84 + 0x14) == 0) goto L_27d4;
    if (mask & 0xa000) {
      uint32_t sub84;
      deferred_cmd_post(0x400,0xa,0);
      sub84 = MB32(battle + 0x84);
      MW16(sub84 + 0x12,(uint16_t)(MB16(sub84 + 0x12) ^ 1));
    }
    {
      int32_t x = MBS16(battle + 0x20) - 0x40;
      int32_t y;
      x += (int32_t)((uint32_t)x >> 31);
      row = (uint32_t)(x >> 1);
      y = MBS16(battle + 0x22);
      if (y < 0) y += 3;
      col = (uint32_t)(y >> 2);
    }
    BR2_E078(battle,0x80121d68,(int32_t)(int16_t)(row + 0x10),col,4);
    yc = (int32_t)(int16_t)(col + gap);
    lh12 = MBS16(MB32(battle + 0x84) + 0x12);
    BR2_E078(battle,0x8012181c,(int32_t)(int16_t)row,yc,(uint32_t)lh12 < 1);
    BR2_E078(battle,0x80121820,(int32_t)(int16_t)(row + 0x30),yc,
             (uint32_t)lh12 != 0);
    BR2_CC6C(battle,row - 4,col - (gap >> 1),0x48,gap * 3,0x201010,0);
    if (mask & 0x10) {
      uint32_t sub84;
      deferred_cmd_post(0x400,0xd,0);
      sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,0);
      sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,0);
    }
    if (mask & 0xe0) {
      lh12 = MBS16(MB32(battle + 0x84) + 0x12);
      if (lh12 == 0) {
        deferred_cmd_post(0x400,0xc,0);
        MB8(battle + 1) = 9;
      } else {
        uint32_t sub84;
        deferred_cmd_post(0x400,0xd,0);
        sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,0);
        sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,0);
      }
    }
    return 0;
  }
  return 0;

L_27d4:                                        /* state2, lh14 == 0 */
  if ((mask & 0xe0) == 0) return 0;
  {
    uint32_t sub84;
    deferred_cmd_post(0x400,0xc,0);
    sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,1);
    sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,1);
  }
  return 0;
}

/* ======== 0x80162838 ========
 * Battle input handler state (MB8(b+0x9b)==4 gate in battle_input_dispatch),
 * 583 insns. a0 = guest battle base, a1 = u16 pad mask. First-use
 * init unpacks byte +0xd4 into five flag halfwords at obj+0x84
 * (+6..+0xe). Announce block emits up to nine hud_text_blk_alt lines whose
 * y-offsets step by s3 (8/0x10) and whose strings come half from
 * rodata and half from the 0x801b41e8/0x801b3f70 pointer tables
 * indexed by lh +8/+0xa/+0xc/+0xe. Dispatch (lb (obj+0x84)+2, <7 via
 * 0x80121e20): 0 = d-pad edge -> d4 recompute block (ret 1),
 * 1..4 = menu_step_a field setters, 5 = ret_zero_e+string_pair_emit,
 * 6 = held-dpad redraw. */
int32_t pad_handler_c(uint32_t battle,uint32_t mask)
{

  /* pad mask */
  uint32_t en = 1;
  uint32_t row, col, gap, sely = 0, emode, yc;
  uint32_t p84, dw_ent;
  int32_t  lb2, lh12;

  /* first-use init: obj+0x84->6 == 0 -> seed flag halfwords from +0xd4 */
  p84 = MB32(battle + 0x84);
  if (MBS16(p84 + 6) == 0) {
    uint32_t d;
    MW16(p84 + 6,1);
    d = MB8(battle + 0xd4);
    MW16(MB32(battle + 0x84) + 8, d & 7);
    d = MB8(battle + 0xd4);
    MW16(MB32(battle + 0x84) + 0xa, (d >> 3) & 1);
    d = MB8(battle + 0xd4);
    MW16(MB32(battle + 0x84) + 0xc, (d >> 4) & 1);
    d = MB8(battle + 0xd4);
    MW16(MB32(battle + 0x84) + 0xe, (d >> 5) & 1);
  }

  /* row = trunc2(lh20 - 0x140), col = trunc4(lh22) */
  {
    int32_t x = MBS16(battle + 0x20) - 0x140;
    x += (int32_t)((uint32_t)x >> 31);
    row = (uint32_t)(x >> 1);
    {
      int32_t y = MBS16(battle + 0x22);
      if (y < 0) y += 3;
      col = (uint32_t)(y >> 2);
    }
  }
  gap = (MB8(battle + 0x15) != 0) ? 0x10 : 8;
  if (MB16(battle + 0x80) & 2) en = 0;
  p84 = MB32(battle + 0x84);
  if (MBS16(p84 + 0x14) != 0) goto L_dispatch;
  if (en == 0) goto L_d6c;

  /* ---- announce init block ---- */
  BR2_E078(battle,0x80121dcc,(int32_t)(int16_t)(row + 0x14),col,4);
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lb2 == 0) { emode = 1; sely = col + gap; }
  BR2_E078(battle,0x80121d58,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap),emode);
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lb2 == 1) { emode = 1; sely = col + (gap << 1); }
  BR2_E078(battle,0x80121ddc,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 2),emode);
  dw_ent = MB32(0x801b41e8 + (uint32_t)MBS16(p84 + 8) * 4);
  BR2_E078(battle,dw_ent,(int32_t)(int16_t)(row + 0x98),
           (int32_t)(int16_t)(col + gap * 2),emode);
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lb2 == 2) { emode = 1; sely = col + gap * 3; }
  BR2_E078(battle,0x80121de8,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 3),emode);
  dw_ent = MB32(0x801b3f70 + (uint32_t)MBS16(p84 + 0xa) * 4);
  BR2_E078(battle,dw_ent,(int32_t)(int16_t)(row + 0x98),
           (int32_t)(int16_t)(col + gap * 3),emode);
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lb2 == 3) { emode = 1; sely = col + gap * 4; }
  BR2_E078(battle,0x80121df4,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 4),emode);
  dw_ent = MB32(0x801b3f70 + (uint32_t)MBS16(p84 + 0xc) * 4);
  BR2_E078(battle,dw_ent,(int32_t)(int16_t)(row + 0x98),
           (int32_t)(int16_t)(col + gap * 4),emode);
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lb2 == 4) { emode = 1; sely = col + gap * 5; }
  BR2_E078(battle,0x80121e0c,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 5),emode);
  dw_ent = MB32(0x801b3f70 + (uint32_t)MBS16(p84 + 0xe) * 4);
  BR2_E078(battle,dw_ent,(int32_t)(int16_t)(row + 0x98),
           (int32_t)(int16_t)(col + gap * 5),emode);
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lb2 == 5) { emode = 1; sely = col + gap * 6; }
  BR2_E078(battle,0x8012136c,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 6),emode);
  lb2 = MBS8(MB32(battle + 0x84) + 2);
  emode = 0;
  if (lb2 == 6) { emode = 1; sely = col + gap * 7; }
  BR2_E078(battle,0x80121d68,(int32_t)(int16_t)(row + 8),
           (int32_t)(int16_t)(col + gap * 7),emode);
  BR2_E078(battle,0x80121270,(int32_t)(int16_t)row,
           (int32_t)(int16_t)sely,1);
  BR2_CC6C(battle,row - 4,col - (gap >> 1),0x140,gap * 9,0x201010,0);

L_d6c:
  if (MBS16(MB32(battle + 0x84) + 0x14) != 0) goto L_dispatch;
  if ((mask & 0x800) == 0) goto L_dispatch;
  goto L_d8c;

L_dispatch:
  p84 = MB32(battle + 0x84);
  lb2 = MBS8(p84 + 2);
  if ((uint32_t)lb2 >= 7) return 0;      /* sltiu 7, beqz -> ret0 */
  switch (lb2) {
  case 0:
    if (mask & 0xe0) goto L_d8c;
    if (MBS16(MB32(battle + 0xc8)) != 0 && (MB16(battle + 0x7e) & 1) != 0)
      MW8(p84 + 1,1);
    else
      MW8(MB32(battle + 0x84) + 1,0);
    return 0;
  case 1:
    menu_step_a(MB32(battle + 0x84) + 8,4,mask & 0xffff);
    return 0;
  case 2:
    menu_step_a(MB32(battle + 0x84) + 0xa,1,mask & 0xffff);
    return 0;
  case 3:
    menu_step_a(MB32(battle + 0x84) + 0xc,1,mask & 0xffff);
    return 0;
  case 4:
    menu_step_a(MB32(battle + 0x84) + 0xe,1,mask & 0xffff);
    return 0;
  case 5:
    if ((mask & 0xe0) == 0) return 0;
    deferred_cmd_post(0x400,0xc,0);
    ret_zero_e(1);
    string_pair_emit(0x1f8000e8,0x1f8001f0,battle,MB32(battle + 0xc0));
    return 0;
  case 6:
    /* ---- held-dpad redraw ---- */
    if (MBS16(p84 + 0x14) == 0) goto L_30f0;
    if (mask & 0xa000) {
      uint32_t sub84;
      deferred_cmd_post(0x400,0xa,0);
      sub84 = MB32(battle + 0x84);
      MW16(sub84 + 0x12,(uint16_t)(MB16(sub84 + 0x12) ^ 1));
    }
    {
      int32_t x = MBS16(battle + 0x20) - 0x40;
      int32_t y;
      x += (int32_t)((uint32_t)x >> 31);
      row = (uint32_t)(x >> 1);
      y = MBS16(battle + 0x22);
      if (y < 0) y += 3;
      col = (uint32_t)(y >> 2);
    }
    BR2_E078(battle,0x80121d68,(int32_t)(int16_t)(row + 0x10),col,4);
    yc = (int32_t)(int16_t)(col + gap);
    lh12 = MBS16(MB32(battle + 0x84) + 0x12);
    BR2_E078(battle,0x8012181c,(int32_t)(int16_t)row,yc,(uint32_t)lh12 < 1);
    BR2_E078(battle,0x80121820,(int32_t)(int16_t)(row + 0x30),yc,
             (uint32_t)lh12 != 0);
    BR2_CC6C(battle,row - 4,col - (gap >> 1),0x48,gap * 3,0x201010,0);
    if (mask & 0x10) {
      uint32_t sub84;
      deferred_cmd_post(0x400,0xd,0);
      sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,0);
      sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,0);
    }
    if (mask & 0xe0) {
      lh12 = MBS16(MB32(battle + 0x84) + 0x12);
      if (lh12 == 0) {
        deferred_cmd_post(0x400,0xc,0);
        MB8(battle + 1) = 9;
      } else {
        uint32_t sub84;
        deferred_cmd_post(0x400,0xd,0);
        sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,0);
        sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,0);
      }
    }
    return 0;
  }
  return 0;

L_30f0:                                        /* case6, lh14 == 0 */
  if ((mask & 0xe0) == 0) return 0;
  {
    uint32_t sub84;
    deferred_cmd_post(0x400,0xc,0);
    sub84 = MB32(battle + 0x84); MW16(sub84 + 0x14,1);
    sub84 = MB32(battle + 0x84); MW16(sub84 + 0x12,1);
  }
  return 0;

L_d8c:                                         /* d4 nibble -> flags */
  {
    uint32_t rec84 = MB32(battle + 0x84);
    uint32_t new_mask = MB16(rec84 + 8) & 7;
    uint32_t prev_mask;
    if (MBS16(rec84 + 0xa) != 0) new_mask |= 8;
    if (MBS16(rec84 + 0xc) != 0) new_mask |= 0x10;
    if (MBS16(rec84 + 0xe) != 0) new_mask |= 0x20;
    prev_mask = MB8(battle + 0xd4);
    if (prev_mask != new_mask) {
      MW8(battle + 0x9a,2);
      MW8(battle + 0xd4,(uint8_t)new_mask);
      MW8(battle + 1,6);
    }
  }
  return 1;
}

/* ======== batch34 ======== */

/* shared callees not yet declared in this file */
extern void spr_desc_b(uint32_t,uint32_t);
extern void spr_desc_c(uint32_t,uint32_t);
extern void u16_tbl_copy(int32_t,const void *);

/* ======== 0x80129e40 ========
 * Signed byte stepper (55 insns). a0 = guest byte ptr (s0), a1 =
 * reset value (s1), a2 = pad bits (s2, only low16 used).
 * 0x2000: beep then ++byte, wrap to 0 if (int8)byte < (int8)(a1<<16>>16);
 * 0x8000: beep then --byte, wrap to a1 if result goes negative.
 * Returns 1 iff the byte changed. */
int32_t menu_step_a(uint32_t ctr_addr,int32_t floor,uint32_t mask)
{
  uint32_t ctr = ctr_addr;
  int32_t  fl = floor;
  uint32_t mask_lo = mask & 0xffff;
  int32_t  old = MBS8(ctr);
  uint32_t btn_code  = mask_lo;

  if (btn_code == 0x2000) {
    deferred_cmd_post(0x400,0xb,0);
    {
      int32_t nv = (int32_t)(int16_t)(fl << 16 >> 16);   /* fl sll/sra 16 */
      int32_t cb = MB8(ctr) + 1;                          /* lbu then +1 */
      MW8(ctr,(uint8_t)cb);
      cb = (int8_t)cb;                                  /* sll18/sra18 */
      if (nv < cb) MW8(ctr,0);
    }
  }
  if (btn_code == 0x8000) {
    deferred_cmd_post(0x400,0xb,0);
    {
      int32_t cb = (int8_t)(MB8(ctr) - 1);               /* lbu-1, sll24 test */
      MW8(ctr,(uint8_t)cb);
      if (cb < 0) MW8(ctr,fl);
    }
  }
  return (MBS8(ctr) ^ old) != 0;
}

/* ======== 0x8015a614 ========
 * Twin string-pair emit (32 insns). a3 = guest record (ctr).
 * +0x17 / +0x18 select: 0 -> fixed 0x801c15d0 / 0x801c15e0 string,
 * nonzero -> 0x801b3f68 + idx*16 record. Calls u16_tbl_copy(0/1,str). */
void string_pair_emit(uint32_t unused_a,uint32_t unused_b,uint32_t unused_c,
                  uint32_t rec)
{
  (void)unused_a;
  (void)unused_b;
  (void)unused_c;
  uint32_t str_idx = MB8(rec + 0x17);
  const void *s;
  if (str_idx == 0) s = guest_at(0x801c15d0);
  else         s = guest_at(0x801b3f68 + (str_idx << 4));
  u16_tbl_copy(0,s);
  str_idx = MB8(rec + 0x18);
  if (str_idx == 0) s = guest_at(0x801c15e0);
  else         s = guest_at(0x801b3f68 + (str_idx << 4));
  u16_tbl_copy(1,s);
}

/* ======== 0x8014950c ========
 * Record-init sweep (1067 insns), called by battle_init state 0 with
 * a0 = battle base 0x1f800000 (unused inside; a1 of E70 calls is it).
 * Outer loop over the TWO engine pointers at 0x801f10c0/0x801f10c4.
 * Per engine: 76-record prologue sweep (stride 0x28, spr_desc_b/
 * spr_desc_c per record), then ~15 table-driven record-init blocks
 * mixing sprite_place_centered lh-quad calls and byte/u16 table copies.
 * sprite_place_centered(rec,p1,t) reads lh t+0,+2,+4,+6 (a2,a3,stk5,stk6). */
#define BR2_E70(rec,t) \
  sprite_place_centered((rec),p1,MBS16(t),MBS16((t)+2), \
               MBS16((t)+4),MBS16((t)+6))
/* packed u16 stores shared by most blocks */
#define BR2_PKA(t) \
  (uint16_t)((((MB16(t) & 0x3ff) >> 4) + ((MB16((t)+2) & 0x1ff) << 6)))
#define BR2_PKB(t) \
  (uint16_t)(((MB16(t) & 0x3ff) >> 6) + \
             (int32_t)(((uint32_t)(MB16((t)+2) & 0x100) << 16) >> 20))
#define BR2_PKB80(t) \
  (uint16_t)(((MB16(t) & 0x3ff) >> 6) + 0x80 + \
             (int32_t)(((uint32_t)(MB16((t)+2) & 0x100) << 16) >> 20))
/* family-Y byte row at s0-0x19..s0 from four table bytes b0..b3 */
#define BR2_YROW(s0,b0,b1,b2,b3) do { \
    MW8((s0)-0x19,(b0)); MW8((s0)-0x18,(b1)); \
    MW8((s0)-0x11,(uint8_t)((b0)+(b2))); MW8((s0)-0x10,(b1)); \
    MW8((s0)-9,(b0)); \
    MW8((s0)-8,(uint8_t)((b1)+(b3))); \
    MW8((s0)-1,(uint8_t)((b0)+(b2))); MW8((s0),(uint8_t)((b1)+(b3))); \
  } while (0)
/* family-X byte row at s0-0xa..s0+0xf from b0..b3 */
#define BR2_XROW(s0,b0,b1,b2,b3) do { \
    MW8((s0)-0xa,(b0)); MW8((s0)-9,(b1)); \
    MW8((s0)-2,(uint8_t)((b0)+(b2))); MW8((s0)-1,(b1)); \
    MW8((s0)+6,(b0)); \
    MW8((s0)+7,(uint8_t)((b1)+(b3))); \
    MW8((s0)+0xe,(uint8_t)((b0)+(b2))); MW8((s0)+0xf,(uint8_t)((b1)+(b3))); \
  } while (0)
/* family-Z byte row at a0-0xa..a0+0xf with additive constants k1/k2 */
#define BR2_ZROW(a0,b0,b1,k1,k2) do { \
    MW8((a0)-0xa,(b0)); MW8((a0)-9,(b1)); \
    MW8((a0)-2,(uint8_t)((b0)+(k1))); MW8((a0)-1,(b1)); \
    MW8((a0)+6,(b0)); MW8((a0)+7,(uint8_t)((b1)+(k2))); \
    MW8((a0)+0xe,(uint8_t)((b0)+(k1))); MW8((a0)+0xf,(uint8_t)((b1)+(k2))); \
  } while (0)
/* family-W positive-offset row at r+0xc..r+0x25 from b0..b3 */
#define BR2_WROW(r,b0,b1,b2,b3) do { \
    MW8((r)+0xc,(b0)); MW8((r)+0xd,(b1)); \
    MW8((r)+0x14,(uint8_t)((b0)+(b2))); MW8((r)+0x15,(b1)); \
    MW8((r)+0x1c,(b0)); MW8((r)+0x1d,(uint8_t)((b1)+(b3))); \
    MW8((r)+0x24,(uint8_t)((b0)+(b2))); MW8((r)+0x25,(uint8_t)((b1)+(b3))); \
  } while (0)

void record_init_sweep(uint32_t ctx,uint32_t unused)
{
  uint32_t p1 = ctx;
  int32_t  i;
  (void)unused;
  for (i = 0; i < 2; i++) {
    uint32_t eng = MB32(0x801f10c0 + (uint32_t)i * 4);
    uint32_t rcur, it, srca, tba, tbb, pkta, pktb, k, rowp, srcb;

    /* ---- prologue: 76 records stride 0x28 ---- */
    rcur = eng; it = 0;
    do {
      rowp = rcur + 6;
      MW8(rcur + 3,9);  MW8(rcur + 7,0x2c);
      spr_desc_b(rcur,0);
      spr_desc_c(rcur,0);
      MW8(rcur + 4,0x7f); MW8(rcur + 5,0x7f); MW8(rcur + 6,0x7f);
      rowp += 0x28; it++; rcur += 0x28;
    } while (it < 0x4c);

    /* ---- combined loop: 4 outer x (2+2 inner) ---- */
    srca = 0x801b2400; pkta = 0x801b2484; pktb = 0x801b2480;
    tba = 0x801b2488; tbb = 0x801b2498;
    rcur = eng + 0x578; it = 0;
    do {
      /* phase 1: byte table s1 */
      rowp = rcur + 0x25;  srcb = srca + 6; k = 0;
      do {
        BR2_E70(rcur,srca);
        MW16(rowp - 0x17,MB16(pkta)); MW16(rowp - 0xf,MB16(pktb));
        BR2_YROW(rowp,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
        srca += 8; rcur += 0x28; rowp += 0x28; srcb += 8; k++;
      } while (k < 2);
      /* phase 2: byte table t0 */
      rowp = rcur + 0x25;  k = 0;
      do {
        BR2_E70(rcur,srca);
        MW16(rowp - 0x17,MB16(pkta)); MW16(rowp - 0xf,MB16(pktb + 2));
        BR2_YROW(rowp,MB8(tbb),MB8(tbb+1),MB8(tbb+2),MB8(tbb+3));
        srca += 8; rcur += 0x28; rowp += 0x28; srcb += 8; k++;
      } while (k < 2);
       tba += 4;  tbb += 4; it++;
    } while (it < 4);

    /* ---- block @eng+0xa0: 3 iters, packedA+packedB ---- */
    pktb = 0x801b24c0; srca = 0x801b24a8; pkta = 0x801b24cc; tba = 0x801b24d8;
    srcb = tba + 3; rcur = eng + 0xa0; rowp = eng + 0xb6; it = 0;
    do {
      BR2_E70(rcur,srca);
      BR2_XROW(rowp,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
      MW16(rowp - 8,BR2_PKA(pkta)); pkta += 4;
      MW16(rowp,BR2_PKB(pktb));     pktb += 4;
      srca += 8;  rcur += 0x28; tba += 4; srcb += 4; it++; rowp += 0x28;
    } while (it < 3);

    /* ---- block @eng+0x118: 4 iters ---- */
    pktb = 0x801b2504; srca = 0x801b24e4; pkta = 0x801b2508; tba = 0x801b250c;
    srcb = srca + 6; rcur = eng + 0x118; rowp = eng + 0x12e; it = 0;
    do {
      BR2_E70(rcur,srca);
      BR2_XROW(rowp,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
      MW16(rowp - 8,BR2_PKA(pkta)); pkta += 4;
      MW16(rowp,BR2_PKB(pktb));     pktb += 4;
      srca += 8; rcur += 0x28; it++; srcb += 8; tba += 4; rowp += 0x28;
    } while (it < 4);

    /* ---- block @eng+0x1ce: 4 iters, constant tables ---- */
    pktb = 0x801b25c0; pkta = 0x801b25bc; tba = 0x801b25c4;
    {
      uint32_t eng_cur = eng + 0x1ce; it = 0;
      do {
        BR2_XROW(eng_cur,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
        MW16(eng_cur - 8,BR2_PKA(pkta));
        MW16(eng_cur,BR2_PKB(pktb));
        it++; eng_cur += 0x28;
      } while (it < 4);
    }

    /* ---- block @eng+0x258: 2 iters ---- */
    pktb = 0x801b2520; srca = 0x801b2510; pkta = 0x801b253c; tba = 0x801b2524;
    srcb = tba + 3;  rcur = eng + 0x258; rowp = eng + 0x26e; it = 0;
    do {
      BR2_E70(rcur,srca);
      BR2_XROW(rowp,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
      MW16(rowp - 8,BR2_PKA(pkta)); pkta += 4;
      MW16(rowp,BR2_PKB(pktb));     pktb += 4;
      srca += 8;  rcur += 0x28; tba += 4; srcb += 4; it++; rowp += 0x28;
    } while (it < 2);

    /* ---- block @eng+0x00: 2 iters, packedB only ---- */
    pktb = 0x801b257c; srca = 0x801b254c; tba = 0x801b255c;
    srcb = tba + 3;  rcur = eng; rowp = eng + 0x16; it = 0;
    do {
      BR2_E70(rcur,srca);
      BR2_XROW(rowp,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
      MW16(rowp,BR2_PKB(pktb));
      srca += 8;  rcur += 0x28; tba += 4; srcb += 4; it++; rowp += 0x28;
    } while (it < 2);

    /* ---- block @eng+0x50: 2 iters, packedB const table ---- */
    srca = 0x801b2564; tba = 0x801b2574;
    srcb = tba + 3;  rcur = eng + 0x50; rowp = eng + 0x66; it = 0;
    do {
      BR2_E70(rcur,srca);
      BR2_XROW(rowp,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
      MW16(rowp,BR2_PKB(0x801b257c));
      srca += 8;  rcur += 0x28; tba += 4; srcb += 4; it++; rowp += 0x28;
    } while (it < 2);

    /* ---- block @eng+0x2be: 18 iters, no E70, +0x40/+7 consts ---- */
    pkta = 0x801b23d4; tba = 0x801b23d8; pktb = 0x801b23fc;
    {
      uint32_t eng_cur = eng + 0x2be; it = 0;
      do {
        BR2_ZROW(eng_cur,MB8(tba),MB8(tba+1),0x40,7);
        MW16(eng_cur - 8,MB16(pkta));
        MW16(eng_cur,BR2_PKB(pktb));
        it++; tba += 2; eng_cur += 0x28;
      } while (it < 0x12);
    }

    /* ---- single records @eng+0xb40 / +0xb68 ---- */
    srca = 0x801b25d8; rcur = eng + 0xb40;
    BR2_E70(rcur,srca);
    {
      uint32_t b = 0x801b25e0;
      BR2_WROW(rcur,MB8(b),MB8(b+1),MB8(b+2),MB8(b+3));
      MW16(rcur + 0xe,BR2_PKA(0x801b25e8));
      MW16(rcur + 0x16,BR2_PKB(0x801b25e4));
    }
    srca = 0x801b25c8; rcur = eng + 0xb68;
    BR2_E70(rcur,srca);
    {
      uint32_t b = 0x801b25d0;
      BR2_WROW(rcur,MB8(b),MB8(b+1),MB8(b+2),MB8(b+3));
      MW16(rcur + 0xe,0x7d9f);
      MW16(rcur + 0x16,BR2_PKB(0x801b25d4));
    }

    /* ---- block @eng+0xaf0: 2 iters, const byte table ---- */
    srca = 0x801b25ec; srcb = srca + 6; tba = 0x801b25fc;
    pkta = 0x801b2650; pktb = 0x801b2600;
    rcur = eng + 0xaf0; rowp = eng + 0xb06; it = 0;
    do {
      BR2_E70(rcur,srca);
      BR2_XROW(rowp,MB8(tba),MB8(tba+1),MB8(tba+2),MB8(tba+3));
      MW16(rowp - 8,BR2_PKA(pkta));
      MW16(rowp,BR2_PKB(pktb));
      srca += 8; srcb += 8; rcur += 0x28; it++; rowp += 0x28;
    } while (it < 2);

    /* ---- block @eng+0x7f8: 15 iters, E70 zero args, +0x38/+0x3f ---- */
    pktb = 0x801b2704; pkta = 0x801b2708; tba = 0x801b2744;
    srcb = tba + 1; rcur = eng + 0x7f8; rowp = eng + 0x80e; it = 0;
    do {
      sprite_place_centered(rcur,p1,0,0,0,0);
      BR2_ZROW(rowp,MB8(tba),MB8(srcb),0x38,0x3f);
      MW16(rowp - 8,BR2_PKA(pkta)); pkta += 4;
      MW16(rowp,BR2_PKB80(pktb));
      rcur += 0x28; it++; tba += 2; srcb += 2; rowp += 0x28;
    } while (it < 0xf);

    /* ---- block @eng+0xab6: 2 iters, no E70, const tables ---- */
    pktb = 0x801b2678; pkta = 0x801b2680; tba = 0x801b267c;
    {
      uint32_t eng_cur = eng + 0xab6; it = 0;
      do {
        BR2_ZROW(eng_cur,MB8(tba),MB8(tba+1),0x38,0x3f);
        MW16(eng_cur - 8,BR2_PKA(pkta));
        MW16(eng_cur,BR2_PKB(pktb));
        it++; tba += 2; eng_cur += 0x28;
      } while (it < 2);
    }

    /* ---- block @eng+0xba6: 2 iters, packed only ---- */
    {
      uint32_t eng_cur = eng + 0xba6; it = 0;
      do {
        MW16(eng_cur - 8,BR2_PKA(0x801b2680));
        MW16(eng_cur,BR2_PKB(0x801b2678));
        it++; eng_cur += 0x28;
      } while (it < 2);
    }
  }
}

/* ======== 0x80154930 ========
 * Two-side battle state commit (1208 insns). a0/a1 = HOST ptrs to the
 * two side records (&DAT_1f8000e8 / &DAT_1f8001f0), a2 = guest battle
 * base 0x1f800000 (s1). Side records are accessed through guest
 * addresses recovered with guest_of. Flow: side-config count ->
 * s1[0x9d]; battle_config_eval x2 -> s5; phase-class fixups on s1+0xb4/0xb6;
 * optional 0x1000-bit arm + select_phase_gate gate; then switch on
 * u8 s1[0x92] (jump table 0x80120a48, 17 entries). Returns void-ish
 * (v0 unused by callers); result code stored to s1[1] on some exits. */
extern void state_word_set_ffff(void);
extern int32_t subrecord_u16_rw(uint32_t,int32_t);
extern void battle_subrecord_set2(uint32_t);
extern void vs_confirm_c(uint8_t *,uint8_t *,uint32_t,uint32_t);
extern void vs_confirm_d(uint8_t *,uint8_t *,uint32_t,uint32_t);
extern void vs_confirm_e(uint8_t *,uint8_t *,uint32_t,uint32_t);
extern void side_phase_commit(uint32_t,uint32_t,uint32_t);
extern void side_phase_advance(uint32_t,uint32_t,uint32_t,int32_t);
extern void slot_triple_set(int32_t,int32_t,int32_t,int32_t);
extern void rec_clear_irq(uint32_t,uint32_t);
extern void stat_counter_accum(int32_t,int32_t);
extern int32_t record_field_step(int32_t,int32_t);
extern void hud_announce(uint32_t,uint32_t,uint32_t);
extern uint32_t g_pbd_calls,g_pbd_phase,g_pbd_ctr,g_pbd_exit,g_pbd_writes;
int32_t post_battle_dispatch(uint8_t *side_a,uint8_t *side_b,uint32_t battle)
{

  uint32_t g2 = guest_of(side_a);
  uint32_t g3 = guest_of(side_b);
  uint32_t cfgfl = 0;
  int32_t  r;
  uint32_t b92;
  g_pbd_calls++;
  g_pbd_phase = MB8(battle + 0x92);
  g_pbd_ctr   = (uint16_t)MBS16(battle + 0x8c);

  /* ---- preamble ---- */
  if ((int32_t)MB32(MB32(g2 + 0x20) + 0x38) > 0 ||
      (int32_t)MB32(MB32(g3 + 0x20) + 0x38) > 0)
    MW8(battle + 0x9d,1);
  else
    MW8(battle + 0x9d,0);
  cfgfl  = (uint32_t)battle_config_eval(side_a,battle,0);
  cfgfl |= (uint32_t)battle_config_eval(side_b,battle,0);

  {
    uint32_t ph_a = MB16(battle + 0xb4) & 0xf00;
    uint32_t ph_b = MB16(battle + 0xb6) & 0xf00;
    if (ph_a == 0x700 || ph_b == 0x700 || ph_a == 0x100 || ph_b == 0x100) {
      if ((MB16(battle + 0x92) & 0x1000) == 0) {
        MW16(battle + 0x8c,0);
        MW16(battle + 0x92,MB16(battle + 0x92) | 0x1000);
      }
    }
  }
  if (MB16(battle + 0x92) & 0x1000) {
    g_pbd_exit = 3;   /* armed path */
    /* armed: tick 0x8c and gate on select_phase_gate */
    MW16(battle + 0x8c,MB16(battle + 0x8c) + 1);
    r = select_phase_gate(battle,0);
    if (r == 0) {
      r = select_phase_gate(battle,1);
      if (r == 0) goto L54af0;
    }
    if (MBS16(battle + 0x8c) < 10) return 0;
    deferred_cmd_post(0x204,0,0);
    frame_present(battle);
    scroll_cfg_set(2,8);
    match_mode_set(1);
    if (MB8(MB32(battle + 0xc0) + 0x23) != 0) { MW8(battle + 1,1); return 0; }
    if (MBS16(battle + 0xb4) != 0) {
      if (MBS16(battle + 0xb6) != 0) { MW8(battle + 1,1); return 0; }
      MW16(battle + 0xb4,MB16(battle + 0xb4) | 0x1000);
    }
    if (MBS16(battle + 0xb6) != 0)
      MW16(battle + 0xb6,MB16(battle + 0xb6) | 0x1000);
    MW8(battle + 1,2);
    return 0;
  }
  /* L54b40: disarmed path */
  if (((MB16(battle + 0x92) & 0xff) == 4))
    battle_input_dispatch(battle,cfgfl & 0xffff);
  if (MBS16(battle + 0x82) != 0) { g_pbd_exit=1; return 0; }
  b92 = MB8(battle + 0x92);
  if (b92 >= 0x11) { g_pbd_exit=2; return 0; }
  g_pbd_exit = 10 + b92;   /* reached the phase switch; low nibble = phase */

  switch (b92) {
  case 0: {
    int32_t c, cond;
    if (MBS8(battle + 0x99) != 0) {
      if ((uint32_t)(MB8(battle + 0x9a) - 2) < 2)
        MW16(MB32(battle + 0xbc) + 0x82,(uint16_t)-1);
      battle_field_set(MB32(battle + 0xbc),3);
      clear_f0300_records();
      MW16(battle + 0x8c,0);
      MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 1);
      battle_subrecord_reset(battle);
      goto L_case1;                       /* j 0x80154cd8 */
    }
    c = MBS16(battle + 0x8c);
    if (c == 0) {
      MW16(MB32(battle + 0xbc) + 0x82,(uint16_t)-1);
      battle_field_set(MB32(battle + 0xbc),3);
      clear_f0300_records();
      c = MBS16(battle + 0x8c);
    }
    cond = (c < 10);
    if (!cond && (cfgfl & 0x8e8) != 0) {
      MW16(battle + 0x8c,0x1356);
      MW16(g2 + 0x4e,0x400);
      MW16(MB32(g2 + 0x38) + 0xd0,0x400);
      MW16(g3 + 0x4e,(uint16_t)-0x400);
      MW16(MB32(g3 + 0x38) + 0xd0,(uint16_t)-0x400);
    }
    if (MBS16(battle + 0x8c) == 0x3c) {
      MW16(MB32(g2 + 0x38) + 0x80,0x29);
      MW16(MB32(g3 + 0x38) + 0x80,0x29);
    }
    if (MBS16(battle + 0x8c) < 0xa0) {
      g_pbd_writes++;
      MW16(battle + 0x8c,MB16(battle + 0x8c) + 1);
      return 0;
    }
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 1);
    battle_subrecord_reset(battle);
    return 0;
  }

L_case1:
  case 1: {
    int32_t scnt = MBS8(battle + 0x99);
    int32_t ann, aval;
    if (scnt < 0) {
      MW8(battle + 0xb0,0);
      ann = 1; aval = MBS16(battle + 0x8c);
    } else {
      uint32_t t = MB8(MB32(battle + 0xc0) + 4);
      scnt = scnt + 1;
      ann = (scnt < (int32_t)(t * 2 - 1)) ? 3 : 2;
      aval = MBS16(battle + 0x8c);
    }
    hud_announce(battle,(uint32_t)ann,(uint32_t)aval);
    if (MBS16(battle + 0x8c) == 0) {
      if (MB8(g2 + 1) != 0) MW8(g2 + 1,MB8(g2 + 1) | 0x80);
      if (MB8(g3 + 1) != 0) MW8(g3 + 1,MB8(g3 + 1) | 0x80);
      MW32(g2 + 0x60,0x10008004);
      MW32(g3 + 0x60,0x10008004);
    }
    if (MBS16(battle + 0x8c) < 0x5a) {
      MW16(battle + 0x8c,MB16(battle + 0x8c) + 1);
      return 0;
    }
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 2);
    deferred_cmd_post(0x400,0x64,0);
    deferred_cmd_post(0x400,0x65,6);
    deferred_cmd_post(0x400,0x66,0xc);
    return 0;
  }

  case 2:
    hud_announce(battle,4,(uint32_t)(MBS16(battle + 0x8c) - 0x1e));
    if (MBS16(battle + 0x8c) < 0x3c) {
      MW16(battle + 0x8c,MB16(battle + 0x8c) + 1);
      return 0;
    }
    if (MB8(g2 + 1) != 0) MW8(g2 + 1,MB8(g2 + 1) & 0x7f);
    if (MB8(g3 + 1) != 0) MW8(g3 + 1,MB8(g3 + 1) & 0x7f);
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 3);
    deferred_cmd_post(0x400,0x67,0);
    deferred_cmd_post(0x400,0x68,6);
    deferred_cmd_post(0x400,0x69,0xc);
    return 0;

  case 3:
    hud_announce(battle,5,(uint32_t)MBS16(battle + 0x8c));
    if (MBS16(battle + 0x8c) == 0) {
      MW32(g2 + 0x60,0);
      MW32(g3 + 0x60,0);
      battle_subrecord_set1(battle);
    } else if (MBS16(battle + 0x8c) >= 0x19) {
      MW16(battle + 0x8c,0);
      MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 4);
    }
    MW16(battle + 0x8c,MB16(battle + 0x8c) + 1);
    /* fallthrough into case 4 */

  case 4: {
    int32_t skip;
    s16_dec_with_floor(battle);
    skip = (MB8(battle + 0x9c) == 0 &&
            MBS16(battle + 0xa2) == MB8(MB32(battle + 0xc0) + 0x10));
    if (!skip) {
      if (MB8(g2 + 1) == 0 && MB8(g3 + 1) != 0) {
        MW32(g2 + 0xf4,MB32(g2 + 0xf4) + 1);
        MW32(g2 + 0xf0,MB32(g2 + 0xf0) + 1);
      }
      if (MB8(g3 + 1) == 0 && MB8(g2 + 1) != 0) {
        MW32(g3 + 0xf4,MB32(g3 + 0xf4) + 1);
        MW32(g3 + 0xf0,MB32(g3 + 0xf0) + 1);
      }
    }
    post_battle_side_commit(side_a,side_b,battle);
    if (((MB32(g2 + 0x64) | MB32(g3 + 0x64)) & 1) != 0) {
      MW32(g2 + 0x64,MB32(g2 + 0x64) & 0xfffffffe);
      MW32(g3 + 0x64,MB32(g3 + 0x64) & 0xfffffffe);
      subrecord_u16_rw(battle,6);
    }
    if (MBS16(battle + 0x94) == 0) {
      int32_t d2, d3, dd;
      if (MB8(g2 + 0) == 1) MW8(g2 + 0,2);
      if (MB8(g3 + 0) == 1) MW8(g3 + 0,2);
      battle_subrecord_set2(battle);
      /* guest 0x8015500C/0x80155028: lh/lh/subu/sll12/div with NO
       * den==0 branch — MIPS div-by-zero yields LO = num>=0 ? -1 : 1. */
      {
        int32_t n2 = (int32_t)((uint32_t)(MBS16(g2 + 0xb0) -
                        MBS16(g2 + 0xac)) << 12);
        dd = MBS16(g2 + 0xb0);
        d2 = dd != 0 ? n2 / dd : (n2 >= 0 ? -1 : 1);
        n2 = (int32_t)((uint32_t)(MBS16(g3 + 0xb0) -
                        MBS16(g3 + 0xac)) << 12);
        dd = MBS16(g3 + 0xb0);
        d3 = dd != 0 ? n2 / dd : (n2 >= 0 ? -1 : 1);
      }
      if (d2 == d3) {
        MW16(battle + 0x8c,0);
        MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 8);
      } else if (d3 < d2) {
        MW16(battle + 0x8c,0);
        MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 5);
      } else {
        MW16(battle + 0x8c,0);
        MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 6);
      }
    }
    if (MBS8(g2 + 0) < 0) {
      if (MBS8(g3 + 0) < 0) {
        if (MBS16(battle + 0x94) == 0)
          MW16(battle + 0x94,MB16(battle + 0x94) + 1);
        battle_subrecord_set2(battle);
        MW16(battle + 0x8c,0);
        MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 7);
        return 0;
      }
      goto L_c4b;
    }
    if (MBS8(g3 + 0) >= 0) goto L_c4b;
    if (MBS16(battle + 0x94) == 0)
      MW16(battle + 0x94,MB16(battle + 0x94) + 1);
    if (MB8(g2 + 0) == 1) MW8(g2 + 0,2);
    battle_subrecord_set2(battle);
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 5);
    return 0;
L_c4b:
    if (MBS8(g2 + 0) >= 0) return 0;
    if (MBS16(battle + 0x94) == 0)
      MW16(battle + 0x94,MB16(battle + 0x94) + 1);
    if (MB8(g3 + 0) == 1) MW8(g3 + 0,2);
    battle_subrecord_set2(battle);
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 6);
    return 0;
  }

  case 5:
    vs_confirm_c(side_a,side_b,battle,cfgfl & 0xffff);
    return 0;
  case 6:
    vs_confirm_c(side_b,side_a,battle,cfgfl & 0xffff);
    return 0;

  case 7: case 8: {
    int inc2, inc3;
    hud_announce(battle,(b92 == 7) ? 0xe : 0xf,(uint32_t)MBS16(battle + 0x8c));
    if (MBS16(battle + 0x94) == 0 && MBS16(battle + 0x8c) == 0) {
      deferred_cmd_post(0x400,0x6d,0x14);
      deferred_cmd_post(0x400,0x6e,0x18);
      deferred_cmd_post(0x400,0x6f,0x1c);
    }
    MW32(g2 + 0x60,MB32(g2 + 0x60) | 0xc0820004);
    MW32(g3 + 0x60,MB32(g3 + 0x60) | 0xc0820004);
    if (MB32(g2 + 0x5c) & 0x10000000) {
      MW32(g2 + 0x60,(MB32(g2 + 0x60) & 2) | 0x40810005);
      MW16(MB32(g2 + 0x38) + 0x80,0x2e);
    }
    if (MB32(g3 + 0x5c) & 0x10000000) {
      MW32(g3 + 0x60,(MB32(g3 + 0x60) & 2) | 0x40810005);
      MW16(MB32(g3 + 0x38) + 0x80,0x2e);
    }
    if (MBS16(battle + 0x8c) != 0) goto L_c78;
    if (MB8(g2 + 1) != 0) MW8(g2 + 1,MB8(g2 + 1) | 0x80);
    if (MB8(g3 + 1) != 0) MW8(g3 + 1,MB8(g3 + 1) | 0x80);
    inc2 = inc3 = 0;
    if (MBS8(battle + 0x99) < 0) {
      if (MB8(g2 + 1) == MB8(g3 + 1)) {
        if (MB16(g2 + 0xe2) != 0) inc3 = 1;
        else if (MB16(g3 + 0xe2) != 0) inc2 = 1;
        else if (MB16(battle + 0xb6) & 0x1000) inc2 = 1;
        else inc3 = 1;
      } else {
        if (MB8(g2 + 1) != 0) inc3 = 1; else inc2 = 1;
      }
    } else { inc2 = 1; inc3 = 1; }
    if (inc2) MW8(g2 + 0xe4,MB8(g2 + 0xe4) + 1);
    if (inc3) MW8(g3 + 0xe4,MB8(g3 + 0xe4) + 1);
L_c78:
    MW16(battle + 0x8c,(int16_t)(MB16(battle + 0x8c) + 1));
    if ((int16_t)MB16(battle + 0x8c) < 0x32) return 0;
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 0xb);
    return 0;
  }

  case 9:
    vs_confirm_d(side_a,side_b,battle,cfgfl & 0xffff);
    return 0;
  case 10:
    vs_confirm_d(side_b,side_a,battle,cfgfl & 0xffff);
    return 0;

  case 11: {
    uint32_t e2, e3, lim;
    MW8(battle + 0xb0,1);
    lim = MB8(MB32(battle + 0xc0) + 4);
    e2 = MB8(g2 + 0xe4);
    e3 = MB8(g3 + 0xe4);
    if (e2 < lim) {
      if (e3 < lim) goto L_bchk; else goto L_cwin;
    }
    if (e3 < lim) goto L_bchk;
    if (e2 < e3) goto L_cwin;
    if (e3 < e2) goto L_dwin;
    MW8(battle + 0x99,0xff);
    goto L_ewin;
L_bchk:
    if (e2 < lim) goto L_ewin;
    goto L_dwin;
L_cwin: {
    MW32(g2 + 0xf4,0);
    MW32(g3 + 0xf4,0);
    battle_field_set(MB32(battle + 0xbc),3);
    if (MB8(g3 + 1) == 0) {
      MW16(g2 + 0xe2,MB16(g2 + 0xe2) + 1);
      if (MB16(g2 + 0xe0) < MB16(g2 + 0xe2))
        MW16(g2 + 0xe0,MB16(g2 + 0xe2));
    }
    MW16(g2 + 0xe6,MB16(g2 + 0xe6) + 1);
    if (MBS16(g2 + 0xac) == 0)
      MW16(g2 + 0xea,MB16(g2 + 0xea) + 1);
    MW16(g3 + 0xe2,0);
    if ((MB16(battle + 0xb4) & 0xf00) == 0x300) {
      stat_counter_accum((int32_t)g2,MB8(g3 + 1));
      MW16(battle + 0xb4,0x1400);
    }
    if ((MB16(battle + 0xb6) & 0xf00) == 0x300) {
      record_field_step((int32_t)g3,MB8(g2 + 1));
      MW16(battle + 0xb6,0x500);
    }
    side_phase_advance(battle,g2,g3,0);
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 0xd);
    return 0;
  }
L_dwin: {
    MW32(g2 + 0xf4,0);
    MW32(g3 + 0xf4,0);
    battle_field_set(MB32(battle + 0xbc),3);
    if (MB8(g2 + 1) == 0) {
      MW16(g3 + 0xe2,MB16(g3 + 0xe2) + 1);
      if (MB16(g3 + 0xe0) < MB16(g3 + 0xe2))
        MW16(g3 + 0xe0,MB16(g3 + 0xe2));
    }
    MW16(g3 + 0xe6,MB16(g3 + 0xe6) + 1);
    if (MBS16(g3 + 0xac) == 0)
      MW16(g3 + 0xea,MB16(g3 + 0xea) + 1);
    MW16(g2 + 0xe2,0);
    if ((MB16(battle + 0xb6) & 0xf00) == 0x300) {
      stat_counter_accum((int32_t)g3,MB8(g2 + 1));
      MW16(battle + 0xb6,0x1400);
    }
    if ((MB16(battle + 0xb4) & 0xf00) == 0x300) {
      record_field_step((int32_t)g2,MB8(g3 + 1));
      MW16(battle + 0xb4,0x500);
    }
    side_phase_advance(battle,g2,g3,1);
    MW16(battle + 0x8c,0);
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 0xe);
    return 0;
  }
L_ewin:
    MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 0xc);
    return 0;
  }

  case 12:
    if (MBS16(battle + 0x8c) >= 0x1e) {
      MW32(g2 + 0x60,MB32(g2 + 0x60) | 0x80028000);
      MW32(g3 + 0x60,MB32(g3 + 0x60) | 0x80028000);
    }
    if (MBS16(battle + 0x8c) < 0x5a) {
      MW16(battle + 0x8c,MB16(battle + 0x8c) + 1);
      return 0;
    }
    side_phase_commit(battle,g2,g3);
    MW16(battle + 0x8c,0);
    return 0;

  case 13:
    vs_confirm_e(side_a,side_b,battle,cfgfl & 0xffff);
    return 0;
  case 14:
    vs_confirm_e(side_b,side_a,battle,cfgfl & 0xffff);
    return 0;

  case 15: {
    int32_t src_val, mulhi, quot, rem;
    MW8(battle + 0xb1,1);                    /* jal delay slot: always */
    if (MBS16(battle + 0x8c) == 0) task_table_init(1);
    slot_triple_set(1,-1,-1,-1);
    slot_triple_set(2,-1,-1,-1);
    match_mode_set(3);
    if ((MB16(battle + 0xb6) & 0xf00) == 0x600) {
      MW32(g2 + 0x60,MB32(g2 + 0x60) | 2);
      MW32(g3 + 0x60,MB32(g3 + 0x60) & 0xfffffffd);
      if (MBS16(battle + 0x8c) == 0)
        battle_field_set(MB32(battle + 0xbc),7);
      if ((MB32(g3 + 0x5c) & 0x10000000) != 0 ||
          MBS16(MB32(g3 + 0x38) + 0x80) == 0x2e) {
        MW32(g3 + 0x60,0x40810005);
        MW16(MB32(g3 + 0x38) + 0x80,0x2d);
      }
      MW16(MB32(g2 + 0x38) + 0x80,1);
    }
    if ((MB16(battle + 0xb4) & 0xf00) == 0x600) {
      MW32(g2 + 0x60,MB32(g2 + 0x60) & 0xfffffffd);
      MW32(g3 + 0x60,MB32(g3 + 0x60) | 2);
      if (MBS16(battle + 0x8c) == 0)
        battle_field_set(MB32(battle + 0xbc),6);
      if ((MB32(g2 + 0x5c) & 0x10000000) != 0 ||
          MBS16(MB32(g2 + 0x38) + 0x80) == 0x2e) {
        MW32(g2 + 0x60,0x40810005);
        MW16(MB32(g2 + 0x38) + 0x80,0x2d);
      }
      MW16(MB32(g3 + 0x38) + 0x80,1);
    }
    if (MBS16(battle + 0x8c) == 1)
      mc_confirm_dispatch(MB8(MB32(battle + 0xc0) + 0xd) != 0 ? 5 : 0x2a,0);
    /* signed a0 % 80 via mult 0x66666667/mulhi>>5 */
    src_val = (int16_t)(MB16(battle + 0xb2) & 0x3fff);
    mulhi  = (int32_t)(((int64_t)src_val * (int64_t)0x66666667) >> 32);
    quot   = (mulhi >> 5) - (src_val >> 31);
    rem = (int16_t)(src_val - quot * 80);
    if (rem == 4) deferred_cmd_post(0x400,0x11,0);
    {
      uint32_t c8  = MB16(battle + 0x8c) + 1;
      uint32_t f4  = MB16(battle + 0xb4) & 0xf00;
      uint32_t f6  = MB16(battle + 0xb6) & 0xf00;
      MW16(battle + 0x8c,c8);
      if (f4 == 0x900 || f6 == 0x900 || f4 == 0x800 || f6 == 0x800) {
        MW16(battle + 0x8c,0);
        MW16(battle + 0x8e,0);
        MW16(battle + 0x90,0);
        MW16(battle + 0x92,(MB16(battle + 0x92) & 0xff00) | 0x10);
        deferred_cmd_post(0x204,0,0);
        return 0;
      }
      hud_announce(battle,0x15,(int32_t)(int16_t)c8);
    }
    return 0;
  }

  case 16: {
    int32_t spr_idx = -1, aux_b5 = 1;
    uint32_t nv, bval;
    int32_t tv;
    MW8(battle + 0xb1,0xff);                 /* jal delay slot: before call */
    slot_triple_set(1,-1,-1,-1);
    slot_triple_set(2,-1,-1,-1);
    match_mode_set(3);
    if ((MB32(battle + 0xb4) & 0x0f000f00) == 0) {
      if (MB16(battle + 0x7c) & 0x800) MW16(battle + 0x8e,0x1356);
      if (MB16(battle + 0x7c) & 0xe8)  MW16(battle + 0x8e,0x1356);
    }
    if (MB8(g2 + 1) == 0) { aux_b5 = MB8(g2 + 5); spr_idx = MB8(g2 + 3); }
    if (MB8(g3 + 1) == 0) { aux_b5 = MB8(g3 + 5); spr_idx = MB8(g3 + 3); }
    nv = MB16(battle + 0x90) + 1;
    MW16(battle + 0x90,nv);
    if ((int16_t)nv < 4) return 0;
    if (aux_b5 == 0) {
      uint32_t sv = MB8(0x801b49f8 + (uint32_t)spr_idx);
      if (menu_sprite_load_sm(battle,sv + 0xc5,0x13,2,1) == 0) return 0;
    }
    if (MBS16(battle + 0x8e) == 0) {
      uint32_t fv = 6;
      if ((MB16(battle + 0xb6) & 0xf00) == 0x900) fv = 7;
      if (MBS16(MB32(battle + 0xbc) + 0x76) != (int32_t)fv)
        battle_field_set(MB32(battle + 0xbc),fv);
      mc_confirm_dispatch(MB8(MB32(battle + 0xc0) + 0xd) != 0 ? 6 : 0x27,0);
      se_value_select((uint32_t)MBS8(battle + 0x98));
      if (aux_b5 == 0) deferred_cmd_post(0x400,0x80,0x1e);
    }
    nv = MB16(battle + 0x8e) + 1;
    MW16(battle + 0x8e,nv);
    hud_announce(battle,0x16,(int32_t)(int16_t)nv);
    bval = MB8(MB32(battle + 0xc0) + 0xd);
    tv = MBS16(battle + 0x8e);
    if (bval != 0) {
      if (tv < 0x15e) return 0;
    } else {
      if (tv < 0x2ee) return 0;
    }
    MW16(battle + 0x8e,0);
    MW16(battle + 0x90,0);
    rec_clear_irq(battle,0x18);
    match_mode_set(1);
    deferred_cmd_post(0x204,0,0);
    frame_present(battle);
    scroll_cfg_set(2,8);
    MW8(battle + 1,9);
    return 0;
  }
  }
  return 0;

L54af0:
  if (MBS16(battle + 0x8c) == 0) mc_confirm_dispatch(2,0);
  if (MBS16(battle + 0x8c) < 0xfa)
    hud_announce(battle,0,(uint32_t)MBS16(battle + 0x8c));
  else
    MW8(battle + 1,1);
  state_word_set_ffff();
  return 0;
}

/* ======== batch35 ======== */

/* shared callees not yet declared in this file */
extern void battle_side_flag_prop(uint32_t,uint8_t *);
extern void vs_confirm_a(uint8_t *,uint8_t *,uint32_t,uint32_t);
extern void versus_stage_commit(uint32_t,uint32_t);
extern void engine_tables_restore(uint32_t);
extern void vs_confirm_b(uint32_t,uint8_t *,uint8_t *,int32_t);
extern uint32_t *angle_pair_rotmat(uint32_t,uint32_t,uint32_t *);
extern uint16_t *gte_rtps_xform1(uint32_t *,uint32_t *,uint16_t *);
extern uint16_t attr_flag_mask(uint32_t);

/* ======== 0x80127ae0 ========
 * Record-field stepper (27 insns). a0 = guest ptr, a1 = direction flag.
 * record = 0x801c11e0 + MB8(a0+3)*36 + 0x264. idx = MB8(0x1f80009c);
 * idx<6 dispatches through u32 table @0x8011ebb8 (entries 0x80127b30 /
 * 0x80127b5c): 0x80127b5c is a bare jr $ra returning v0 = target itself.
 * Shared tail @0x80127b30: a1!=0 -> ++u16 @rec+6 (via fn @0x80127b4c);
 * a1==0 -> ++u16 @rec+0xa. Returns the incremented u32. */
int32_t record_field_step(int32_t ent,int32_t which)
{
  uint32_t ent_u = (uint32_t)ent;
  uint32_t rec;
  uint32_t idx, tgt;
  uint32_t nv;

  rec = 0x801c11e0u + ((uint32_t)MB8(ent_u + 3) * 36u) + 0x264u;
  idx = MB8(0x1f80009c);
  if (idx < 6) {
    tgt = MB32(0x8011ebb8u + (idx << 2));
    if (tgt == 0x80127b5cu) return (int32_t)tgt;   /* jr $v0 -> jr $ra, v0=tgt */
    /* tgt == 0x80127b30 -> shared tail */
  }
  if (which != 0) {
    nv = (uint32_t)MB16(rec + 6) + 1u;
    MW16(rec + 6,(uint16_t)nv);
    return (int32_t)nv;
  }
  nv = (uint32_t)MB16(rec + 0xa) + 1u;
  MW16(rec + 0xa,(uint16_t)nv);
  return (int32_t)nv;
}

/* ======== 0x80128bfc ========
 * 4 insns: *(u16*)MB32(a0+0xcc) = 2. */
void battle_subrecord_set2(uint32_t battle)
{
  MW16(MB32(battle + 0xcc),2);
}

/* ======== 0x80128f98 ========
 * 8 insns, two return paths. a1<0 -> return (s16)*(u16*)MB32(a0+0xcc);
 * a1>=0 -> store a1 there and return the pointer itself (v0). */
int32_t subrecord_u16_rw(uint32_t battle,int32_t val)
{
  uint32_t sub = MB32(battle + 0xcc);
  if (val < 0) return MBS16(sub);
  MW16(sub,(uint16_t)val);
  return (int32_t)sub;
}

/* ======== 0x8012b488 ========
 * 5 insns: u16 @0x1f800082 = 0xffff. */
void state_word_set_ffff(void)
{
  MW16(0x1f800082,0xffff);
}

/* ======== 0x8012f2a8 ========
 * Menu cursor-movement update (535 insns). a0 = guest cursor state (s1),
 * a3 = guest record (s3); a1/a2 unused. s2 = u16 @s3+0x68 (held buttons),
 * s0 = u16 @s3+0x6c. Auto-repeat bytes @s1+0x80/0x81 wrap at 0x11.
 * Dispatch: (MB16(s1+0x76)&0x7f) < 0x25 -> u32 table @0x8011fc34
 * (0/1->0x8012f408, 2->0x8012f608, 31->0x8012f774, 32->0x8012f8e4,
 * 35/36->0x8012f960, else epilogue). Direction table @0x8011f944
 * (8x3 u16). Matrix/vector path via angle_pair_rotmat + gte_rtps_xform1 on
 * s3+0x48; velocity deltas accumulate into s1+0x52..0x56 / 0x4a..0x4e. */
void menu_step_b(uint32_t cursor,uint32_t unused_a,uint32_t unused_b,
                  uint32_t rec)
{


  uint32_t btns = MB16(rec + 0x68);
  uint32_t dirs = MB16(rec + 0x6c);
  uint32_t nv, sv, mode, doff;
  uint32_t dv[2];                              /* sp+0x10..0x17, u16 lanes */
  uint16_t *dvh;
  uint32_t *m;

  (void)unused_a; (void)unused_b;

  /* ---- held-button auto-repeat bytes ---- */
  if (btns & 4) {
    uint32_t b = (uint32_t)MB8(cursor + 0x80) + 1u;
    MW8(cursor + 0x80,(uint8_t)b);
    if ((int32_t)(int8_t)(uint8_t)b >= 0x11) MW8(cursor + 0x80,0xff);
  }
  if (btns & 1) {
    uint32_t b = (uint32_t)MB8(cursor + 0x81) + 1u;
    MW8(cursor + 0x81,(uint8_t)b);
    if ((int32_t)(int8_t)(uint8_t)b >= 0x11) MW8(cursor + 0x81,0xff);
  }
  if (btns & 0x20) {
    /* signed mod-4 of (u16 @s1+0x76 + 1) into battle_field_set */
    nv = (uint32_t)MB16(cursor + 0x76) + 1u;
    sv = (uint32_t)(int32_t)(int16_t)(nv << 16 >> 16);
    MW16(cursor + 0x76,(uint16_t)nv);
    {
      int32_t field_val = (int32_t)sv;
      if (field_val < 0) field_val += 3;
      field_val = field_val >> 2;                          /* sra */
      field_val = (int32_t)sv - (field_val << 2);
      field_val = (int32_t)(int16_t)(field_val << 16 >> 16);
      battle_field_set(cursor,field_val);
    }
  }
  if (dirs & 8) {
    /* s0&0x80 -> unconditional +0x10; s0&0x40 -> -0x10 (delay-slot flag) */
    if (dirs & 0x80)
      MW16(rec + 0x24,(uint16_t)(MB16(rec + 0x24) + 0x10));
    if (dirs & 0x40)
      MW16(rec + 0x24,(uint16_t)(MB16(rec + 0x24) - 0x10));
    gte_cw_d000_set(MBS16(rec + 0x24),0);
  }

  mode = (uint32_t)MB16(cursor + 0x76) & 0x7f;
  if (mode >= 0x25) goto L_end;
  switch (mode) {
  case 0: case 1: goto L_case408;
  case 2:        goto L_case608;
  case 31:       goto L_case774;
  case 32:       goto L_case8e4;
  case 35: case 36: goto L_case960;
  default:       goto L_end;
  }

L_case408:                                     /* 0x8012f408 */
  if (btns & 2) {
    MW16(cursor + 0x7e,(uint16_t)((MB16(cursor + 0x7e) + 1) & 7));
    doff = (uint32_t)MBS16(cursor + 0x7e) * 8u;
    MW16(cursor + 0x52,MB16(0x8011f944u + doff));
    MW16(cursor + 0x54,MB16(0x8011f944u + doff + 2));
    MW16(cursor + 0x56,MB16(0x8011f944u + doff + 4));
  }
  /* shared velocity block 0x8012f46c */
  dv[0] = dv[1] = 0;                            /* sp+0x10..0x17 = 0 */
  dvh = (uint16_t *)dv;
  if (dirs & 0x80) {
    if (dirs & 0x1000) MW16(cursor + 0x30,(uint16_t)(MB16(cursor + 0x30) + 8));
    if (dirs & 0x4000) MW16(cursor + 0x30,(uint16_t)(MB16(cursor + 0x30) - 8));
    if (dirs & 0x8000) MW16(cursor + 0x32,(uint16_t)(MB16(cursor + 0x32) - 0x10));
    if (dirs & 0x2000) MW16(cursor + 0x32,(uint16_t)(MB16(cursor + 0x32) + 0x10));
  } else if (dirs & 0x10) {
    if (dirs & 0x1000) MW16(cursor + 0x68,(uint16_t)(MB16(cursor + 0x68) + 0x1a));
    if (dirs & 0x4000) MW16(cursor + 0x68,(uint16_t)(MB16(cursor + 0x68) - 0x1a));
  } else if (dirs & 0x40) {
    if (dirs & 0x1000) dvh[2] = 0x1a;
    if (dirs & 0x4000) dvh[2] = (uint16_t)-0x1a;
  } else {
    if (dirs & 0x8000) dvh[0] = (uint16_t)-0x1a;
    if (dirs & 0x2000) dvh[0] = 0x1a;
    if (dirs & 0x1000) dvh[1] = (uint16_t)-0x1a;
    if (dirs & 0x4000) dvh[1] = 0x1a;
  }
  /* dead stores to sp+0x38..0x43 in original omitted (never read) */
  m = angle_pair_rotmat((uint32_t)(-MBS16(cursor + 0x28)),
                   (uint32_t)MBS16(cursor + 0x2a),
                   (uint32_t *)guest_at(rec + 0x48));
  gte_rtps_xform1(m,(uint32_t *)dv,dvh);
  MW16(cursor + 0x52,(uint16_t)(MB16(cursor + 0x52) + dvh[0]));
  MW16(cursor + 0x54,(uint16_t)(MB16(cursor + 0x54) + dvh[1]));
  MW16(cursor + 0x56,(uint16_t)(MB16(cursor + 0x56) + dvh[2]));
  goto L_end;

L_case608:                                     /* 0x8012f608 */
  if (dirs & 0x40) {
    if (dirs & 8) {
      if (dirs & 0x1000) MW16(cursor + 0x68,(uint16_t)(MB16(cursor + 0x68) - 0x33));
      if (dirs & 0x4000) MW16(cursor + 0x68,(uint16_t)(MB16(cursor + 0x68) + 0x33));
    } else {
      if (dirs & 0x1000) MW16(cursor + 0x68,(uint16_t)(MB16(cursor + 0x68) - 0xc));
      if (dirs & 0x4000) MW16(cursor + 0x68,(uint16_t)(MB16(cursor + 0x68) + 0xc));
    }
  } else if (dirs & 8) {
    if (dirs & 0x1000) MW16(cursor + 0x30,(uint16_t)(MB16(cursor + 0x30) + 0x40));
    if (dirs & 0x4000) MW16(cursor + 0x30,(uint16_t)(MB16(cursor + 0x30) - 0x40));
    if (dirs & 0x8000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) + 0x40));
    if (dirs & 0x2000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) - 0x40));
  } else {
    if (dirs & 0x1000) MW16(cursor + 0x30,(uint16_t)(MB16(cursor + 0x30) + 0x10));
    if (dirs & 0x4000) MW16(cursor + 0x30,(uint16_t)(MB16(cursor + 0x30) - 0x10));
    if (dirs & 0x8000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) + 0x10));
    if (dirs & 0x2000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) - 0x10));
  }
  goto L_end;

L_case774:                                     /* 0x8012f774 */
  dv[0] = dv[1] = 0;
  dvh = (uint16_t *)dv;
  MW16(cursor + 0x60,0);
  MW16(cursor + 0x5e,0);
  MW16(cursor + 0x5c,0);
  if (dirs & 0x80) {
    if (dirs & 0x1000) MW16(cursor + 0x28,(uint16_t)(MB16(cursor + 0x28) + 8));
    if (dirs & 0x4000) MW16(cursor + 0x28,(uint16_t)(MB16(cursor + 0x28) - 8));
    if (dirs & 0x8000) MW16(cursor + 0x2a,(uint16_t)(MB16(cursor + 0x2a) - 0x10));
    if (dirs & 0x2000) MW16(cursor + 0x2a,(uint16_t)(MB16(cursor + 0x2a) + 0x10));
  } else if (dirs & 0x40) {
    if (dirs & 0x1000) dvh[2] = 0x1a;
    if (dirs & 0x4000) dvh[2] = (uint16_t)-0x1a;
  } else {
    if (dirs & 0x8000) dvh[0] = (uint16_t)-0x1a;
    if (dirs & 0x2000) dvh[0] = 0x1a;
    if (dirs & 0x1000) dvh[1] = (uint16_t)-0x1a;
    if (dirs & 0x4000) dvh[1] = 0x1a;
  }
  m = angle_pair_rotmat((uint32_t)(-MBS16(cursor + 0x28)),
                   (uint32_t)MBS16(cursor + 0x2a),
                   (uint32_t *)guest_at(rec + 0x48));
  gte_rtps_xform1(m,(uint32_t *)dv,dvh);
  MW16(cursor + 0x4a,(uint16_t)(MB16(cursor + 0x4a) + dvh[0]));
  MW16(cursor + 0x4c,(uint16_t)(MB16(cursor + 0x4c) + dvh[1]));
  MW16(cursor + 0x4e,(uint16_t)(MB16(cursor + 0x4e) + dvh[2]));
  goto L_end;

L_case8e4:                                     /* 0x8012f8e4 */
  if (btns & 2) {
    MW16(cursor + 0x2c,0);
    MW16(cursor + 0x2a,0);
    MW16(cursor + 0x28,0);
    MW16(cursor + 0x60,0);
    MW16(cursor + 0x5e,0);
    MW16(cursor + 0x5c,0);
    MW16(cursor + 0x7e,(uint16_t)((MB16(cursor + 0x7e) + 1) & 7));
    doff = (uint32_t)MBS16(cursor + 0x7e) * 8u;
    MW16(cursor + 0x4a,MB16(0x8011f944u + doff));
    MW16(cursor + 0x4c,MB16(0x8011f944u + doff + 2));
    MW16(cursor + 0x4e,MB16(0x8011f944u + doff + 4));
  }
  goto L_end;

L_case960:                                     /* 0x8012f960 */
  if (dirs & 0x40) {
    if (dirs & 8) {
      if (dirs & 0x1000) MW16(cursor + 0x60,(uint16_t)(MB16(cursor + 0x60) - 0x33));
      if (dirs & 0x4000) MW16(cursor + 0x60,(uint16_t)(MB16(cursor + 0x60) + 0x33));
    } else {
      if (dirs & 0x1000) MW16(cursor + 0x60,(uint16_t)(MB16(cursor + 0x60) - 0xc));
      if (dirs & 0x4000) MW16(cursor + 0x60,(uint16_t)(MB16(cursor + 0x60) + 0xc));
    }
  } else if (dirs & 8) {
    if (dirs & 0x1000) MW16(cursor + 0x28,(uint16_t)(MB16(cursor + 0x28) + 0x40));
    if (dirs & 0x4000) MW16(cursor + 0x28,(uint16_t)(MB16(cursor + 0x28) - 0x40));
    if (dirs & 0x8000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) + 0x40));
    if (dirs & 0x2000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) - 0x40));
  } else {
    if (dirs & 0x1000) MW16(cursor + 0x28,(uint16_t)(MB16(cursor + 0x28) + 0x10));
    if (dirs & 0x4000) MW16(cursor + 0x28,(uint16_t)(MB16(cursor + 0x28) - 0x10));
    if (dirs & 0x8000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) + 0x10));
    if (dirs & 0x2000) MW16(cursor + 0x8a,(uint16_t)(MB16(cursor + 0x8a) - 0x10));
  }
  if (dirs & 0x80) {                               /* 0x8012fac0 reset block */
    MW16(cursor + 0x2c,0);
    MW16(cursor + 0x2a,0);
    MW16(cursor + 0x28,0);
    MW16(cursor + 0x5e,0);
    MW16(cursor + 0x5c,0);
    MW16(cursor + 0x60,0x300);
    MW16(cursor + 0x8a,0);
  }
  goto L_end;

L_end:                                          /* 0x8012fae8 */
  return;
}

/* ======== 0x8015297c ========
 * Per-side phase-advance helper (152 insns). a0 = guest battle-ish struct
 * (t0), a1/a2 = side arg slots (s1/s2), a3 = flag. Dispatch on
 * MB8(t0+0x9c) via u32 table @0x801209b0 (0->0x801529cc, 1/2->0x80152bb4,
 * 3/5->0x80152b44, 4->epilogue). Counters @t0+0xa4 saturate to -1/-2
 * against limits @0x1f8001d4/0x1f8002dc or record byte @MB32(t0+0xc0)+0x10.
 * 0x300-class arms records @t0+0xb2/b4/b6 (hi nibble kept |0x900) and
 * calls name_rec_gate(side-block host ptr, table @0x801209ac) +
 * status_flag_cond_set(guest side,0). */
void side_phase_advance(uint32_t battle,uint32_t side_a,uint32_t side_b,
                  int32_t flag)
{




  uint32_t idx, ph, a0b, cnt, lim;
  const void *tbl = guest_at(0x801209ac);

  idx = MB8(battle + 0x9c);
  if (idx >= 6) return;
  switch (idx) {
  case 0: goto L_case0;
  case 1: case 2: goto L_inc;
  case 3: case 5: goto L_case345;
  case 4: return;
  }
  return;

L_case0:                                        /* 0x801529cc */
  ph  = MB16(battle + 0x92) & 0xf00;
  a0b = MB8(MB32(battle + 0xc0) + 0x10);
  if (ph == 0x200) goto L_200;
  if (ph < 0x201) {
    if (ph != 0x100) return;
    goto L_100;
  }
  if (ph != 0x300) return;
  goto L_300;

L_100:                                          /* 0x80152a14: flag must be 0 */
  if (flag != 0) return;
  cnt = (uint32_t)MB16(battle + 0xa4) + 1u;
  MW16(battle + 0xa4,(uint16_t)cnt);
  lim = MB16(0x1f8001d4);
  if (lim == 0) {
    if ((int32_t)(int16_t)cnt < (int32_t)a0b + 1) return;
    MW16(battle + 0xa4,0xfffe);
    return;
  }
  if ((int32_t)(int16_t)lim < (int32_t)a0b) return;
  MW16(battle + 0xa4,0xffff);
  return;

L_200:                                          /* 0x80152a74: flag must be nonzero */
  if (flag == 0) return;
  cnt = (uint32_t)MB16(battle + 0xa4) + 1u;
  MW16(battle + 0xa4,(uint16_t)cnt);
  lim = MB16(0x1f8002dc);
  if (lim == 0) {
    if ((int32_t)(int16_t)cnt < (int32_t)a0b + 1) return;
    MW16(battle + 0xa4,0xfffe);
    return;
  }
  if ((int32_t)(int16_t)lim < (int32_t)a0b) return;
  MW16(battle + 0xa4,0xffff);
  return;

L_300:                                          /* 0x80152ad4 */
  if (flag == 0) {
    MW16(battle + 0xb2,0);
    MW16(battle + 0xb6,(uint16_t)((MB16(battle + 0xb6) & 0xf000) | 0x900));
    name_rec_gate((uint8_t *)guest_at(side_b),tbl);
    status_flag_cond_set(side_b,0);                        /* a2=0x801209ac unused */
  } else {
    MW16(battle + 0xb2,0);
    MW16(battle + 0xb4,(uint16_t)((MB16(battle + 0xb4) & 0xf000) | 0x900));
    name_rec_gate((uint8_t *)guest_at(side_a),tbl);
    status_flag_cond_set(side_a,0);
  }
  return;

L_case345:                                      /* 0x80152b44 */
  if (MB8(battle + 0x9b) != 0) goto L_inc;
  ph  = MB16(battle + 0x92) & 0xf00;
  a0b = MB8(MB32(battle + 0xc0) + 0x10);
  if (!((ph == 0x100 && flag == 0) || (ph == 0x200 && flag != 0))) return;
  cnt = (uint32_t)MB16(battle + 0xa4) + 1u;
  MW16(battle + 0xa4,(uint16_t)cnt);
  if ((int32_t)(int16_t)cnt >= (int32_t)a0b) MW16(battle + 0xa4,0xffff);
  return;

L_inc:                                          /* 0x80152bb4 */
  MW16(battle + 0xa4,(uint16_t)(MB16(battle + 0xa4) + 1));
  return;
}

/* ======== 0x801531ec ========
 * 30 insns. a0 = guest struct (s0); a1/a2 unused. If (s8)@+0x99 >= 0
 * increment the u8; if u8 @+0x9b < 4 call confirm_bookkeeping(s0), then store
 * 2 or 3 @+0x9a (3 when the signed byte went negative) and 6 @+1. */
void side_phase_commit(uint32_t obj,uint32_t unused_a,uint32_t unused_b)
{

  (void)unused_a; (void)unused_b;
  if ((int32_t)MBS8(obj + 0x99) >= 0)
    MW8(obj + 0x99,(uint8_t)(MB8(obj + 0x99) + 1));
  if ((uint32_t)MB8(obj + 0x9b) < 4) {
    confirm_bookkeeping(obj);
    MW8(obj + 0x9a,(uint8_t)(((int32_t)MBS8(obj + 0x99) < 0) ? 3 : 2));
    MW8(obj + 1,6);
  }
}

/* ======== 0x80153ee8 ========
 * Versus-screen confirm/cancel handler (288 insns).
 * a0/a1 = per-side host arg blocks (s2/s1), a2 = guest battle state (s0),
 * a3 unused. Gates on s16 @s0+0x8c and phase @s0+0x92&0xf00; beeps via
 * deferred_cmd_post, timer via hud_announce, voice via ctr_clamp;
 * battle_side_flag_prop/vs_confirm_a on the guest/host pair. Ends with the
 * 0x80020000 flag merge, optional 0x81000005 remap, and the >=0x78
 * phase-reset writing 9/0xa/0xb into @s0+0x92. Returns 0 (v0). */
void vs_confirm_c(uint8_t *blk_a,uint8_t *blk_b,uint32_t battle,
                  uint32_t unused)
{



  uint32_t ph, step;
  (void)unused;

  if (MBS16(battle + 0x8c) == 0) {
    if (blk_a[1] != 0) blk_a[1] = (uint8_t)(blk_a[1] | 0x80);
    if (blk_b[1] != 0) blk_b[1] = (uint8_t)(blk_b[1] | 0x80);
    if (MB8(battle + 0x9b) == 0) {
      ph = MB16(battle + 0x92) & 0xf00;
      if (ph == 0x200 || ph == 0x100) {
        if (MBS16(battle + 0x94) == 0) {
          step = (blk_a[1] != 0) ? (uint32_t)-10 : 10u;
        } else if (blk_a[0] == 0x81) {
          step = (blk_a[1] != 0) ? (uint32_t)-10 : 10u;
        } else if (*(int16_t *)(blk_a + 0xac) == 0) {
          step = (blk_a[1] != 0) ? (uint32_t)-10 : 15u;
        } else {
          step = (blk_a[1] != 0) ? (uint32_t)-10 : 10u;
        }
        ctr_clamp(battle,(int32_t)step);
      }
    }
  }

  if (MBS16(battle + 0x94) == 0) {                 /* 0x80154010 block A */
    hud_announce(battle,6,(uint32_t)MBS16(battle + 0x8c));
    if (MBS16(battle + 0x8c) == 0) {
      deferred_cmd_post(0x400,0x6d,0x14);
      deferred_cmd_post(0x400,0x6e,0x18);
      deferred_cmd_post(0x400,0x6f,0x1c);
      blk_b[0xe4] = (uint8_t)(blk_b[0xe4] + 1);
    }
    *(uint32_t *)(blk_a + 0x60) |= 0x80000000u;
    *(uint32_t *)(blk_b + 0x60) |= 0x80000000u;
    goto L_tick8c;                             /* j 0x80154248 */
  }

  /* 0x801540a0 block B */
  if (blk_b[0] == 0x81) {
    if (MBS16(battle + 0x8c) >= 2)
      hud_announce(battle,7,(uint32_t)MBS16(battle + 0x8c));
  } else {
    if (*(int16_t *)(blk_a + 0xac) == 0) {
      if (MBS16(battle + 0x8c) >= 2)
        hud_announce(battle,8,(uint32_t)MBS16(battle + 0x8c));
    } else {
      if (MBS16(battle + 0x8c) >= 2)
        hud_announce(battle,9,(uint32_t)MBS16(battle + 0x8c));
    }
    if (MBS16(battle + 0x8c) == 0) battle_side_flag_prop(battle,blk_a);
  }
  /* 0x80154138 */
  if (MBS16(battle + 0x8c) == 0) {
    deferred_cmd_post(0x400,0x5f,0);
    blk_b[0xe4] = (uint8_t)(blk_b[0xe4] + 1);
    MW16(battle + 0x8c,(uint16_t)(MB16(battle + 0x8c) + 1));
  }

  /* 0x80154174 */
  if (MBS16(MB32(battle + 0xcc) + 0xa) != 0) {
    vs_confirm_a(blk_a,blk_b,battle,0);
    goto L_tail;                              /* j 0x80154258 */
  }
  *(uint32_t *)(blk_a + 0x60) |= 0x80000000u;
  *(uint32_t *)(blk_b + 0x60) |= 0x80000000u;
  if (MBS16(battle + 0x8c) == 1 && blk_a[0] != 0x81) {
    if (*(int16_t *)(blk_a + 0xac) == 0) {
      deferred_cmd_post(0x400,0x70,0x14);
      deferred_cmd_post(0x400,0x71,0x1a);
      deferred_cmd_post(0x400,0x72,0x20);
    } else {
      deferred_cmd_post(0x400,0x6a,0x14);
      deferred_cmd_post(0x400,0x6b,0x1a);
      deferred_cmd_post(0x400,0x6c,0x20);
    }
  }

L_tick8c:                                      /* 0x80154248 */
  MW16(battle + 0x8c,(uint16_t)(MB16(battle + 0x8c) + 1));

L_tail:                                       /* 0x80154258 */
  {
    uint32_t f = *(uint32_t *)(blk_b + 0x60) | 0x80020000u;
    *(uint32_t *)(blk_b + 0x60) = f;
    if (MBS16(MB32(battle + 0xcc) + 0xa) == 0 &&
        (*(uint32_t *)(blk_b + 0x5c) & 0x1000u) != 0) {
      *(uint32_t *)(blk_b + 0x60) = (f & 2u) | 0x81000005u;
      MW16(*(uint32_t *)(blk_b + 0x38) + 0x80,0x2e);
    }
  }
  if (MBS16(battle + 0x8c) >= 0x78) {             /* 0x801542c4 */
    uint32_t cls;
    MW8(battle + 0x9e,0);
    MW16(battle + 0x8c,0);
    if (MBS16(battle + 0x94) != 0 && blk_b[0] != 0x81 &&
        (uint32_t)blk_b[0xe4] >= (uint32_t)MB8(MB32(battle + 0xc0) + 4)) {
      cls = (blk_a[2] != 0) ? 0xa : 9;
    } else {
      cls = 0xb;
    }
    MW16(battle + 0x92,(uint16_t)((MB16(battle + 0x92) & 0xff00) | cls));
  }
  return;                                     /* v0 = 0 */
}

/* ======== 0x80154368 ========
 * Confirm-side handler (154 insns). a0/a1 = per-side host arg blocks
 * (s1/s2), a2 = guest battle state (s0), a3 = flag (s4).
 * s3 = attr_flag_mask(s1[2]) when s1[1]==0. Advances timer @s0+0x8c,
 * beeps via deferred_cmd_post, hud_announce(0x10), s16_dec_with_floor, then the
 * phase-0 arm: battle_field_set(bc,0xa/0xb), @s0+0xb0=1, task_table_init(1),
 * clear_f0300_records, versus_stage_commit(s0,2). Optional (s4&0x800) cancel via
 * engine_tables_restore + same reset writing 0xb. Tail: subrecord_u16_rw(s0,-1)==4
 * gate -> engine_tables_restore + reset writing 0xb. Returns 0. */
void vs_confirm_d(uint8_t *blk_a,uint8_t *blk_b,uint32_t battle,
                  uint32_t flag)
{




  uint32_t attr_fl = 0;


  if (blk_a[1] == 0) attr_fl = (uint32_t)attr_flag_mask(blk_a[2]);
  hud_announce(battle,0x10,(uint32_t)MBS16(battle + 0x8c));
  s16_dec_with_floor(battle);
  *(uint32_t *)(blk_a + 0x60) |= 0x80020000u;
  *(uint32_t *)(blk_b + 0x60) |= 0x80020000u;

  if (MBS16(battle + 0x8c) == 0) {
    battle_field_set(MB32(battle + 0xbc),(blk_b[2] != 0) ? 0xb : 0xa);
    MW8(battle + 0xb0,1);
    task_table_init(1);
    clear_f0300_records();
    versus_stage_commit(battle,2);
  }
  /* 0x80154458: cancel arm needs counter >= 0x14 and flag bit 0x800 */
  if (MBS16(battle + 0x8c) >= 0x14 && (flag & 0x800u) != 0) {
    /* 0x80154468: armed cancel */
    engine_tables_restore(battle);
    task_table_init(1);
    clear_f0300_records();
    MW16(battle + 0x92,(uint16_t)((MB16(battle + 0x92) & 0xff00) | 0xb));
    MW16(battle + 0x8c,0);
    MW16(MB32(battle + 0xbc) + 0x82,0xfffd);
    battle_field_set(MB32(battle + 0xbc),3);
    return;                                   /* v0 = 0 */
  }

  /* 0x801544b4 */
  MW16(battle + 0x8c,(uint16_t)(MB16(battle + 0x8c) + 1));
  MW8(battle + 0x9e,(uint8_t)(((attr_fl & 0xe8u) == 0xe0u) ? 1 : 0));
  if (MBS16(battle + 0x94) == 0) {
    if (blk_a[0] == 1) blk_a[0] = 2;
    if (blk_b[0] == 1) blk_b[0] = 2;
  }
  if ((uint32_t)blk_a[0] >= 2 || (uint32_t)blk_b[0] >= 2) {
    *(uint32_t *)(blk_a + 0x60) |= 0x80000000u;
    *(uint32_t *)(blk_b + 0x60) |= 0x80000000u;
  }
  if (subrecord_u16_rw(battle,-1) != 4) return;         /* v0 = 0 */
  engine_tables_restore(battle);
  task_table_init(1);
  clear_f0300_records();
  MW16(battle + 0x92,(uint16_t)((MB16(battle + 0x92) & 0xff00) | 0xb));
  MW16(battle + 0x8c,0);
  MW16(MB32(battle + 0xbc) + 0x82,0xfffd);
  battle_field_set(MB32(battle + 0xbc),3);
  return;                                       /* v0 = 0 */
}

/* ======== 0x801545d0 ========
 * Losing-side / post-match handler (216 insns). a0/a1 = per-side host
 * arg blocks (s2/s1), a2 = guest battle state (s0), a3 = flag (s3).
 * If MBS16(s0+0x8c) >= 0x12c: reset counter and tail-call
 * vs_confirm_b(s0, sideA, sideB, order-flag). Otherwise merge
 * 0x80020000 flags and dispatch on @s0+0x92&0xf00 (0x200->5143c 0x14,
 * 0x100/0x300/0x400->5143c 0x13/0x11, else skip) with mc_confirm_dispatch +
 * beep triplets; the 0x8c==0 arm does task_table_init/sprite_anim_chain_build/
 * action_enter_fwd x2 + field writes + battle_field_set(bc,3). Tail merges
 * 0x40a10005/0x40a10007, optional 0x8c==2 record fixup, and the
 * >=0xa && s3&0x800 -> 0x1356 clamp. Returns 0. */
void vs_confirm_e(uint8_t *blk_a,uint8_t *blk_b,uint32_t battle,
                  uint32_t flag)
{




  uint32_t ph;

  if (MBS16(battle + 0x8c) >= 0x12c) {
    MW16(battle + 0x8c,0);
    if (blk_a[2] == 0)
      vs_confirm_b(battle,blk_a,blk_b,0);
    else
      vs_confirm_b(battle,blk_b,blk_a,1);
    return;                                     /* v0 = 0 */
  }

  *(uint32_t *)(blk_a + 0x60) |= 0x80020000u;
  *(uint32_t *)(blk_b + 0x60) |= 0x80020000u;

  ph = MB16(battle + 0x92) & 0xf00;
  if (ph == 0x200) {                            /* 0x80154704 */
    if (blk_a[2] == 0) {
      hud_announce(battle,0x14,(uint32_t)MBS16(battle + 0x8c));
      goto L_chk8c_a;
    }
    goto L_513;
  }
  if (ph < 0x201) {
    if (ph == 0) goto L_main;
    if (ph == 0x100) {                          /* 0x801546b8 */
      if (blk_a[2] == 0) goto L_513;
      hud_announce(battle,0x14,(uint32_t)MBS16(battle + 0x8c));
      goto L_chk8c_a;
    }
    goto L_main;
  }
  if (ph == 0x300) {                            /* 0x801547ac */
    hud_announce(battle,0x11,(uint32_t)MBS16(battle + 0x8c));
    goto L_chk8c_b;
  }
  if (ph == 0x400) {                            /* 0x801547b0 */
    hud_announce(battle,0x11,(uint32_t)MBS16(battle + 0x8c));
    goto L_chk8c_b;
  }
  goto L_main;

L_513:                                          /* 0x80154750 */
  hud_announce(battle,0x13,(uint32_t)MBS16(battle + 0x8c));
  if (MBS16(battle + 0x8c) != 0) goto L_868;
  mc_confirm_dispatch(3,0);
  deferred_cmd_post(0x400,0x61,0x14);
  deferred_cmd_post(0x400,0x62,0x1a);
  deferred_cmd_post(0x400,0x63,0x20);
  goto L_main;

L_chk8c_a:                                      /* shared 0xc0+0xd -> 577bc */
  if (MBS16(battle + 0x8c) != 0) goto L_868;
  if (MB8(MB32(battle + 0xc0) + 0xd) == 0)
    mc_confirm_dispatch(4,0);
  else
    mc_confirm_dispatch((int32_t)(uint32_t)battle,0);
  goto L_main;

L_chk8c_b:                                      /* 0x300/0x400 tail */
  if (MBS16(battle + 0x8c) != 0) goto L_868;
  mc_confirm_dispatch(3,0);
  goto L_main;

L_main:                                         /* 0x801547d4 */
  if (MBS16(battle + 0x8c) != 0) goto L_868;
  task_table_init(1);
  sprite_anim_chain_build(battle);
  action_enter_fwd(battle,blk_a,4);
  action_enter_fwd(battle,blk_b,4);
  MW16(*(uint32_t *)(blk_a + 0x38) + 0x80,0x28);
  MW16(*(uint32_t *)(blk_b + 0x38) + 0x80,
       (uint16_t)(MBS16(battle + 0x94) != 0 ? 0x67 : 0x2d));
  MW16(MB32(battle + 0xbc) + 0x82,0xffff);
  battle_field_set(MB32(battle + 0xbc),3);
  *(uint32_t *)(blk_a + 0x60) |= 0x40a10005u;
  *(uint16_t *)(blk_b + 0xc0) = 0xffff;
  *(uint32_t *)(blk_b + 0x60) |= 0x40a10007u;
  if (MBS16(battle + 0x8c) >= 10 && (flag & 0x800u) != 0)
    MW16(battle + 0x8c,0x1356);
  MW16(battle + 0x8c,(uint16_t)(MB16(battle + 0x8c) + 1));
  return;                                       /* v0 = 0 */

L_868:                                          /* 0x80154868 */
  if (MBS16(battle + 0x8c) == 2) {
    uint32_t ent_ptr = *(uint32_t *)(blk_b + 0x38);    /* guest ptr value */
    uint32_t d;
    d = (uint32_t)*(uint16_t *)(blk_b + 0x88) - (uint32_t)MB16(ent_ptr + 0xca);
    MW16(ent_ptr + 0xc2,(uint16_t)(MB16(ent_ptr + 0xc2) + d));
    ent_ptr = *(uint32_t *)(blk_b + 0x38);
    MW16(ent_ptr + 0xc8,*(uint16_t *)(blk_b + 0x88));
    MW16(ent_ptr + 0xca,*(uint16_t *)(blk_b + 0x88));
    MW16(*(uint32_t *)(blk_a + 0x38) + 0x80,0x2c);
  }
  *(uint32_t *)(blk_a + 0x60) |= 0x40a10005u;
  *(uint16_t *)(blk_b + 0xc0) = 0xffff;
  *(uint32_t *)(blk_b + 0x60) |= 0x40a10007u;
  if (MBS16(battle + 0x8c) >= 10 && (flag & 0x800u) != 0)
    MW16(battle + 0x8c,0x1356);
  MW16(battle + 0x8c,(uint16_t)(MB16(battle + 0x8c) + 1));
  return;                                       /* v0 = 0 */
}


/* ======== batch36 ======== */
/* battle_frame — main battle-frame dispatcher @0x8014cdd0 (2301 insns,
 * decomp/battle_frame.asm verified complete through jr $ra @0x8014f1c0).
 * No args; battle base fixed s5 = 0x1f800000, engine pair
 * 0x801f10c0/0x801f10c4, dispatcher byte = guest 0x1f800001.
 * The 0x180-byte guest frame is a static local buffer (the guest sp
 * region is reused every call, so static models leftover slots);
 * record-table base (guest sp+0x40) is kept as host pointer T and
 * mirrored to slot 0x140 for callee visibility, same convention as
 * battle_init's 0x801f10bc store. battle_bringup_b gets host pointers
 * into the frame for its three output tables (sp+0xb8/sp+0xe8/
 * sp+0x118) like battle_bringup_a's stack arg in battle_init. Slots
 * 0x130/0x134 keep the guest addresses 0x1f8000e8/0x1f8001f0 and are
 * converted with guest_at at each battle_config_eval/fighter_spawn_setup call
 * (those callees take host arg-block pointers). */

extern int battle_bringup_b(uint32_t,void *,void *,void *);
extern void gpu_side_packet_emit(uint32_t,int32_t,int32_t);
extern void engine_record_pair_update(uint32_t,void *);
extern void side_flag_merge(uint32_t,void *);
extern void ctx_flag_set(uint32_t,void *);
extern void width3_num_format(void *,int32_t,int32_t);
extern void packed_field_store(uint32_t);
extern void descblk_variant_e(uint32_t,uint32_t,void *,int32_t,int32_t,int32_t);
extern int32_t victory_record_restore(uint32_t,int32_t);
extern void task_heads_clear(int);
extern void task_pools_tick(int);
extern void record_flag_byte_clear(uint32_t);
extern void record_flag_byte_set(uint32_t);
extern int32_t sprite_rect_set_c(uint32_t,int32_t,int32_t,int32_t,int32_t);
extern int task_alloc_wrap_e(int,int,int);
extern void clear_battle_counters(void);

#define B36_8(o)   (((uint8_t *)b36_sp)[(o)])
#define B36_16(o)  (*(uint16_t *)((uint8_t *)b36_sp + (o)))
#define B36_16S(o) (*(int16_t  *)((uint8_t *)b36_sp + (o)))
#define B36_32(o)  (b36_sp[(o) >> 2])

/* Ordering-table packet push shared by every GPU block: the packet's
 * link word gets old OT[slot] | 0x09000000, then OT[slot] = pkt &
 * 0xffffff. pkt is a guest address; slot a sign-extended lh. MIPS
 * sllv/srlv/srav use the low 5 bits of the shift count — &31 kept. */
static void ot_push(uint32_t pkt, int32_t slot)
{
  uint32_t ot = MB32(0x1f800008) + ((uint32_t)slot << 2);
  MW32(pkt, MB32(ot) | 0x09000000u);
  MW32(ot, pkt & 0x00ffffffu);
}

/* indexed engine base: MB32(0x801f10c0 + MB32(0x1f80000c)*4) */
#define B36_ENG() MB32(0x801f10c0 + MB32(0x1f80000c) * 4u)

/* 8-byte record copy used by the state-0 confirm and both state-1
 * sub-blocks: src[0..3] -> dst+{264,265,26c,26d,274,275,27c,27d} on
 * BOTH fixed engine tables (0x801f10c0 / 0x801f10c4 + s1). Byte
 * pattern per side: s0,s1,s0+s2,s1,s0,s1+s3,s0+s2,s1+s3. */
static void copy8_rec(uint32_t e0, uint32_t e1, uint32_t off,
                          const uint8_t *src)
{
  uint8_t *d0 = (uint8_t *)guest_at(e0 + off);
  uint8_t *d1 = (uint8_t *)guest_at(e1 + off);
  uint32_t g0 = guest_of(d0), g1 = guest_of(d1);
  MW8(g0 + 0x264, src[0]);
  MW8(g0 + 0x265, src[1]);
  MW8(g0 + 0x26c, (uint8_t)(src[0] + src[2]));
  MW8(g0 + 0x26d, src[1]);
  MW8(g0 + 0x274, src[0]);
  MW8(g0 + 0x275, (uint8_t)(src[1] + src[3]));
  MW8(g0 + 0x27c, (uint8_t)(src[0] + src[2]));
  MW8(g0 + 0x27d, (uint8_t)(src[1] + src[3]));
  MW8(g1 + 0x264, src[0]);
  MW8(g1 + 0x265, src[1]);
  MW8(g1 + 0x26c, (uint8_t)(src[0] + src[2]));
  MW8(g1 + 0x26d, src[1]);
  MW8(g1 + 0x274, src[0]);
  MW8(g1 + 0x275, (uint8_t)(src[1] + src[3]));
  MW8(g1 + 0x27c, (uint8_t)(src[0] + src[2]));
  MW8(g1 + 0x27d, (uint8_t)(src[1] + src[3]));
}

int battle_frame(void)
{
  /* guest sp .. sp+0x17f frame image (frame 0x180). The guest prologue
   * does `addiu sp,-0x180`; reserve the same window on the live guest sp
   * (br2_guest_frame mirrors it into cpu->gpr[29]) so every nested guest
   * call inside the battle loop allocates BELOW this frame instead of
   * overwriting the argblock-pointer slots at +0x130/+0x134 — without
   * the reservation those slots held callee-frame leftovers
   * (0x801bcb80/0x801fc6d0) and player-1's phase word never armed.
   * guest RAM backing so pointers stored into the frame (0x801f10bc,
   * slots 0x140/0x144) are real guest addresses that alias the same
   * bytes the host-pointer accesses below use. */
  uint32_t b36_frm = br2_guest_frame(0x180);
  uint32_t *b36_sp = (uint32_t *)guest_at(b36_frm);
  uint8_t *T;                               /* record table = sp+0x40 */
  uint8_t *ctx;                             /* context base = sp+0x18 */
  uint32_t st;

  /* prologue @0x8014cdd0 */
  B36_32(0x130) = 0x1f8000e8;
  B36_32(0x134) = 0x1f8001f0;
  ctx = (uint8_t *)b36_sp + 0x18;
  MW32(0x801f10bc, guest_of(ctx));
  B36_32(0x5c) = 0;
  B36_32(0x8c) = 0;
  task_heads_clear(0);
  task_heads_clear(1);
  task_heads_clear(2);
  packed_field_store(MB32(MB32(0x1f8000c0) + 0x4c));
  flow_cfg_init(0);
  MW16(0x1f80008c, 0);
  T = (uint8_t *)b36_sp + 0x40;
  B36_32(0x140) = guest_of(T);

  for (;;) {
    /* Bail-unwind in flight (g_psx_call_bail): this function is ONE clean-C
     * activation for the whole battle — there is no dispatch boundary until
     * it returns, so a latched bail can never reach the psx_dispatch_impl
     * frame that resolves/flattens it (every br2_call_generated inside would
     * silently drop — the announce-wedge DAT1=8 stall,
     * proof/announce_script_handler_drop.md). The generated CPS equivalent
     * returns immediately; do the same. The guest epilogue does NOT run on a
     * bail exit (no MW8(0x1f800001,0), no sp restore): abandon only the
     * host-side frame record so $sp keeps the un-returned frame, exactly as
     * generated code leaves it. */
    {
      if (br2_bail_pending()) { br2_guest_frame_abandon(); return 0; } }
    /* dispatcher @0x8014ce80 */
    st = MB8(0x1f800001);
    if (st == 0xff) break;
    if (st < 6) {
      if (st < 4) {
        if (st == 1) goto L_case1;
        if (st < 2) {
          if (st == 0) goto L_case0;
          goto L_tail;
        }
        if (st == 2) goto L_case2;
        if (st == 3) goto L_case3;
        goto L_tail;
      }
      goto L_case45;                         /* 4 or 5 */
    }
    if (st == 8) goto L_case8;
    if (st < 9) {
      if (st == 6) goto L_case6;
      if (st == 7) goto L_case7;
      goto L_tail;
    }
    if (st == 0xf0) goto L_casef0;
    if (st == 9) goto L_case9;
    if (st == 0xf1) { MW8(0x1f800001, 0xff); goto L_tail; }
    goto L_tail;

L_case0:                                      /* 0x8014cf38 */
    if (frame_divisor_get() != 0) goto L_tail;
    if (battle_bringup_b(0x1f800000,
                     (uint8_t *)b36_sp + 0xb8,
                     (uint8_t *)b36_sp + 0xe8,
                     (uint8_t *)b36_sp + 0x118) == 0) goto L_tail;
    print_mode_dispatch_b(2,0,0);
    mc_confirm_dispatch(MB8(MB32(0x1f8000c0) + 0xd) ? 1 : 0x1f, 1);
    task_pool_init(1,2,0x6400,0x801d01ec);
    task_pool_init(2,2,0x5000,0x801d65ec);
    MW32(0x801f10c0, task_pool_alloc(2,2,0xbe0));
    MW32(0x801f10c4, task_pool_alloc(2,2,0xbe0));
    task_heads_clear(1);
    task_heads_clear(2);
    clear_battle_counters();
    battle_config_eval((uint8_t *)guest_at(B36_32(0x130)),0x1f800000,0);
    battle_config_eval((uint8_t *)guest_at(B36_32(0x134)),0x1f800000,0);
    post_confirm_config_build(0x1f800000,(uint8_t *)b36_sp + 0x18,0);
    record_init_sweep(0x1f800000,0);                /* a1 indeterminate */
    /* falls into case1 */

L_case1:                                      /* 0x8014d03c */
    {
      int32_t side;
      uint8_t *rec = T;
      gpu_side_packet_emit(B36_32(0x118),0,2);
      for (side = 0; side < 2; side++, rec += 0x30) {
        uint32_t pk, sel, idx;
        if (side == 1 && *(uint32_t *)(T + 0x4c) == 5) {
          pk = B36_32(0x114); sel = 1;
        } else {
          idx = MB8(0x801b238c + *(uint32_t *)(rec + 0x1c));
          pk = B36_32(0xe8 + idx * 4); sel = (uint32_t)side;
        }
        gpu_side_packet_emit(pk,(int32_t)sel,0);
        idx = MB8(0x801b238c + *(uint32_t *)(rec + 0x1c));
        gpu_side_packet_emit(B36_32(0xb8 + idx * 4),(int32_t)side,1);
      }
    }
    side_flag_merge(0x1f800000,(uint8_t *)b36_sp + 0x18);
    ctx_flag_set(0x1f800000,(uint8_t *)b36_sp + 0x18);
    task_alloc_wrap_e(0,0x801b2764,0x801b2784);
    /* falls into case2 */

L_case2:                                      /* 0x8014d120 */
    engine_record_pair_update(0x1f800000,(uint8_t *)b36_sp + 0x18);
    MW8(0x1f800001,3);
    /* falls into case3 */

L_case3:                                      /* 0x8014d134 */
    {
      int32_t c = (int32_t)B36_32(0x2c) - 1;
      B36_32(0x2c) = (c < 0) ? 0u : (uint32_t)c;
    }
    B36_32(0x10) = 0;
    fighter_spawn_setup(0x1f800000,
                 (uint8_t *)guest_at(B36_32(0x130)),
                 (uint8_t *)guest_at(B36_32(0x134)),
                 (uint8_t *)b36_sp + 0x18,0);
    goto L_tail;

L_case45:                                     /* 0x8014d16c (4 or 5) */
    ctx_flag_set(0x1f800000,(uint8_t *)b36_sp + 0x18);
    engine_record_pair_update(0x1f800000,(uint8_t *)b36_sp + 0x18);
    MW8(0x1f800001,6);
    /* falls into case6 */

L_case6:                                      /* 0x8014d18c */
    {
      int32_t c = (int32_t)B36_32(0x2c) - 1;
      B36_32(0x2c) = (c < 0) ? 0u : (uint32_t)c;
    }
    B36_32(0x10) = 0;
    fighter_spawn_setup(0x1f800000,
                 (uint8_t *)guest_at(B36_32(0x130)),
                 (uint8_t *)guest_at(B36_32(0x134)),
                 (uint8_t *)b36_sp + 0x18,0);
    if (MB16(0x1f8000a2) == 0 && (MB16(0x1f80007c) & 0x10) != 0) {
      deferred_cmd_post(0x204,0,0);
      deferred_cmd_post(0x400,0xd,0);
      B36_32(0x38) = 0xffffffffu;
      MW8(0x1f800001,0xf1);                    /* j 0x8014df68 */
      goto L_tail;
    }
    /* ---------- per-player battle update @0x8014d20c ---------- */
    {
      int32_t side;
      uint8_t *rec;
      uint32_t eoff, doff;
      for (side = 0; side < 2; side++) {
        rec = T + (uint32_t)side * 0x30;
        eoff = (uint32_t)side * 0x28;
        doff = (uint32_t)side * 4;
        switch (*(uint32_t *)(rec + 0x24)) {
        case 0: goto L_st0;
        case 1: goto L_st1;
        case 2: goto L_st2;
        default: break;
        }
        goto L_next;

L_st0:                                         /* 0x8014d258 */
        if (*(uint32_t *)(rec + 0x10) != 0) goto L_next;
        if ((*(uint32_t *)(rec + 8) & 0x8000u) != 0) {
          /* downward scan, step -2, clamp parity -> 0xd/0xe */
          uint32_t pm = MB32(0x801f10b8);
          int32_t cv;
          for (;;) {
            cv = (int32_t)*(uint32_t *)(rec + 0x1c) - 2;
            *(uint32_t *)(rec + 0x1c) = (uint32_t)cv;
            if (cv < 0) {
              *(uint32_t *)(rec + 0x1c) = (uint32_t)((cv & 1) ? 0xd : 0xe);
              continue;
            }
            if (pm & (1u << (cv & 31))) break;
            if (((0xff8u >> (cv & 31)) & 1u) != 0) break;
          }
        }
        if ((*(uint32_t *)(rec + 8) & 0x2000u) != 0) {
          /* upward scan, step +2, clamp parity -> 1/0 */
          uint32_t pm = MB32(0x801f10b8);
          int32_t cv;
          for (;;) {
            cv = (int32_t)*(uint32_t *)(rec + 0x1c) + 2;
            *(uint32_t *)(rec + 0x1c) = (uint32_t)cv;
            if (cv >= 0xf) {
              *(uint32_t *)(rec + 0x1c) = (uint32_t)((cv & 1) ? 1 : 0);
              continue;
            }
            if (pm & (1u << (cv & 31))) break;
            if (((0xff8u >> (cv & 31)) & 1u) != 0) break;
          }
        }
        /* d39c/d3d4 gates -> d410 inc+downscan, d504 dec+upscan.
         * s3==0 wants (f1c&1)==0; s3==1 wants (f1c&1)!=0 and the
         * flag bits are swapped between the two checks. */
        if (side == 0) {
          if ((*(uint32_t *)(T + 8) & 0x1000u) != 0 &&
              (*(uint32_t *)(T + 0x1c) & 1u) == 0) goto L_d410;
        } else {
          if ((*(uint32_t *)(T + 0x30 + 8) & 0x4000u) != 0 &&
              (*(uint32_t *)(T + 0x30 + 0x1c) & 1u) != 0) goto L_d410;
        }
        goto L_d490;
L_d410:                                       /* 0x8014d410 */
        {
          uint32_t pm;
          int32_t nx, cv;
          nx = (int32_t)*(uint32_t *)(rec + 0x1c) + 1;
          *(uint32_t *)(rec + 0x1c) = (uint32_t)nx;
          if (nx >= 0xf) *(uint32_t *)(rec + 0x1c) = 0xe;
          pm = MB32(0x801f10b8);
          cv = (int32_t)*(uint32_t *)(rec + 0x1c);
          if ((pm & (1u << (cv & 31))) == 0 &&
              ((0xff8u >> (cv & 31)) & 1u) == 0) {
            nx = cv - 2;
            for (;;) {
              *(uint32_t *)(rec + 0x1c) = (uint32_t)nx;
              if (nx < 0) *(uint32_t *)(rec + 0x1c) = 0xe;
              cv = (int32_t)*(uint32_t *)(rec + 0x1c);
              if (pm & (1u << (cv & 31))) break;
              if (((0xff8u >> (cv & 31)) & 1u) != 0) break;
              nx = cv - 2;
            }
          }
        }
L_d490:                                       /* 0x8014d490 */
        if (side == 0) {
          if ((*(uint32_t *)(T + 8) & 0x4000u) != 0 &&
              (*(uint32_t *)(T + 0x1c) & 1u) != 0) goto L_d504;
        } else {
          if ((*(uint32_t *)(T + 0x30 + 8) & 0x1000u) != 0 &&
              (*(uint32_t *)(T + 0x30 + 0x1c) & 1u) == 0) goto L_d504;
        }
        goto L_d584;
L_d504:                                       /* 0x8014d504 */
        {
          uint32_t pm;
          int32_t nx, cv;
          nx = (int32_t)*(uint32_t *)(rec + 0x1c) - 1;
          *(uint32_t *)(rec + 0x1c) = (nx < 0) ? 1u : (uint32_t)nx;
          pm = MB32(0x801f10b8);
          cv = (int32_t)*(uint32_t *)(rec + 0x1c);
          if ((pm & (1u << (cv & 31))) == 0 &&
              ((0xff8u >> (cv & 31)) & 1u) == 0) {
            for (;;) {
              nx = cv + 2;
              *(uint32_t *)(rec + 0x1c) = (uint32_t)nx;
              if (nx >= 0xf) *(uint32_t *)(rec + 0x1c) = 0;
              cv = (int32_t)*(uint32_t *)(rec + 0x1c);
              if (pm & (1u << (cv & 31))) break;
              if (((0xff8u >> (cv & 31)) & 1u) != 0) break;
            }
          }
        }
L_d584:                                       /* 0x8014d584 */
        if (*(uint32_t *)(rec + 0x1c) != *(uint32_t *)(rec + 0x20)) {
          uint32_t idx;
          deferred_cmd_post(0x400,0x40,0);
          *(uint32_t *)(rec + 0x20) = *(uint32_t *)(rec + 0x1c);
          if (side == 1 && *(uint32_t *)(T + 0x4c) == 5) {
            gpu_side_packet_emit(B36_32(0x114),(int32_t)side,0);
          } else {
            idx = MB8(0x801b238c + *(uint32_t *)(rec + 0x1c));
            gpu_side_packet_emit(B36_32(0xe8 + idx * 4),(int32_t)side,0);
          }
          idx = MB8(0x801b238c + *(uint32_t *)(rec + 0x1c));
          gpu_side_packet_emit(B36_32(0xb8 + idx * 4),(int32_t)side,1);
          engine_record_pair_update(0x1f800000,(uint8_t *)b36_sp + 0x18);
        }
        if (B36_32(0x2c) == 0) {
          /* scan rec+0x1c upward until a pad bit is set */
          int32_t nx;
          for (;;) {
            nx = (int32_t)*(uint32_t *)(rec + 0x1c);
            if (MB32(0x801f10b8) & (1u << (nx & 31))) break;
            nx = nx + 1;
            *(uint32_t *)(rec + 0x1c) = (uint32_t)nx;
            if (nx >= 0xf) *(uint32_t *)(rec + 0x1c) = 0;
          }
          *(uint32_t *)(rec + 8) |= 0x80u;
        }
        /* confirm/select @0x8014d6bc */
        if ((MB32(0x801f10b8) &
             (1u << (*(uint32_t *)(rec + 0x1c) & 31))) != 0) {
          if ((*(uint32_t *)(rec + 8) & 0xe0u) != 0 ||
              (*(uint32_t *)(rec + 8) & 0x800u) != 0) {
            if ((*(uint32_t *)(rec + 8) & 0xe0u) != 0 ||
                ((MB32(MB32(0x1f8000c0) + 0x50) >>
                  (MB8(0x801b239c + *(uint32_t *)(rec + 0x1c)) & 31)) & 1u) != 0) {
              B36_32(0x10) = 0;
              select_confirm_handler(0x1f800000,(uint32_t)side,T,
                           *(uint32_t *)(rec + 8),0);
              deferred_cmd_post(0x400,0x41,0);
              *(uint32_t *)(rec + 0x24) = 1;
              copy8_rec(MB32(0x801f10c0),MB32(0x801f10c4),eoff,
                            (uint8_t *)guest_at(0x801b252c + doff));
              if (side == 0)
                task_alloc_wrap_c(MB32(0x801f10c0) + eoff + 0x266,
                             MB32(0x801f10c4) + eoff + 0x266,
                             0x801b3054,0x801b30dc);
              else
                task_alloc_wrap_c(MB32(0x801f10c0) + eoff + 0x28e,
                             MB32(0x801f10c4) + eoff + 0x28e,
                             0x801b3054,0x801b312c);
            }
          }
        }
        goto L_next;

L_st1:                                         /* 0x8014d8f8 */
        if (*(uint32_t *)(rec + 0x10) == 0) {
          /* copy other->f24 rule: other f10==1, or f10==0 with
           * other->f24>0, forces rec->f24 = 2 (v0 preset to 2 in the
           * delay slots — the stored value is always 2). */
          uint8_t *o = T + (uint32_t)(side ^ 1) * 0x30;
          uint32_t f10 = *(uint32_t *)(o + 0x10);
          if (f10 == 1 ||
              (f10 == 0 && (int32_t)*(uint32_t *)(o + 0x24) > 0))
            *(uint32_t *)(rec + 0x24) = 2;
          if (*(uint32_t *)(rec + 0x24) != 2) goto L_next;
          copy8_rec(MB32(0x801f10c0),MB32(0x801f10c4),eoff,
                        (uint8_t *)guest_at(0x801b2534 + doff));
          goto L_next;
        }
        if (*(uint32_t *)(rec + 0x10) == 1) {
          /* gate on the OPPONENT record T+(s3^1)*0x30: f24>0 or
           * (f0>>1)&1 (generated 0x8014DAC4: xori $2,$s3,1 -> *3 ->
           * <<4 -> +T). Hardcoding T+0x30 only matches s3==0; for the
           * P2 pass it must read P1's record or P2's confirm is
           * gated on its own not-yet-ready f24 and never completes. */
          uint8_t *a2r = T + (uint32_t)(side ^ 1) * 0x30;
          if ((int32_t)*(uint32_t *)(a2r + 0x24) <= 0 &&
              ((*(uint32_t *)(a2r + 0) >> 1) & 1u) == 0) goto L_next;
          {
            /* 0x8014db00: pick anim index through 0x1f8000a8 table */
            int32_t si = (int16_t)MB16(0x1f8000a2);
            uint8_t v = MB8(MB32(0x1f8000a8) + (uint32_t)si);
            v = MB8(0x801b23ac + v);
            *(uint32_t *)(rec + 0)    |= 8u;
            *(uint32_t *)(rec + 0x24) = 2;
            *(uint32_t *)(rec + 0x1c) = v;
          }
          copy8_rec(MB32(0x801f10c0),MB32(0x801f10c4),eoff,
                        (uint8_t *)guest_at(0x801b2534 + doff));
          if (((*(uint32_t *)(a2r + 0) >> 1) & 1u) == 0) {
            if (side == 0)
              task_alloc_wrap_c(MB32(0x801f10c0) + eoff + 0x266,
                           MB32(0x801f10c4) + eoff + 0x266,
                           0x801b3054,0x801b30dc);
            else
              task_alloc_wrap_c(MB32(0x801f10c0) + eoff + 0x28e,
                           MB32(0x801f10c4) + eoff + 0x28e,
                           0x801b3054,0x801b312c);
          }
          if (side == 1 && *(uint32_t *)(T + 0x4c) == 5) {
            gpu_side_packet_emit(B36_32(0x114),1,0);
          } else {
            uint32_t idx = MB8(0x801b238c + *(uint32_t *)(rec + 0x1c));
            gpu_side_packet_emit(B36_32(0xe8 + idx * 4),(int32_t)side,0);
          }
          {
            uint32_t idx = MB8(0x801b238c + *(uint32_t *)(rec + 0x1c));
            gpu_side_packet_emit(B36_32(0xb8 + idx * 4),(int32_t)side,1);
          }
          engine_record_pair_update(0x1f800000,(uint8_t *)b36_sp + 0x18);
        }
        goto L_next;

L_st2:                                         /* 0x8014dd90 */
        if (*(uint32_t *)(T + 0x24) == 2 &&
            *(uint32_t *)(T + 0x54) == *(uint32_t *)(T + 0x24))
          MW8(0x1f800001,7);
        goto L_next;

L_next:
        ;
      }
    }
    goto L_tail;

L_case7:                                      /* 0x8014dddc */
    task_alloc_wrap_e(0,0x801b2820,0x801b2844);      /* a0 indeterminate */
    MW8(0x1f800001,8);
    /* falls into case8 */

L_case8:                                      /* 0x8014ddf8 */
    battle_config_eval((uint8_t *)guest_at(B36_32(0x130)),0x1f800000,1);
    battle_config_eval((uint8_t *)guest_at(B36_32(0x134)),0x1f800000,1);
    {
      int32_t i;
      for (i = 0; i < 2; i++) {
        uint8_t *r = T + (uint32_t)i * 0x30;
        if (*(uint32_t *)(r + 0x10) == 1 &&
            (MB16(0x1f8000b4 + (uint32_t)i * 2) & 0xf00u) == 0x700) {
          B36_8(0x3c) = 1;
          B36_8(0x3d) = (uint8_t)i;
        }
      }
    }
    goto L_tail;

L_case9:                                      /* 0x8014de74 */
    if ((int8_t)B36_8(0x3c) != 0) {
      uint32_t side = B36_8(0x3d);
      uint8_t *rec;
      uint32_t aidx;
      confirm_bookkeeping(0x1f800000);
      task_heads_clear(1);
      task_heads_clear(2);
      print_mode_dispatch_b(2,0,0);
      post_confirm_config_build(0x1f800000,(uint8_t *)b36_sp + 0x18,0);
      record_init_sweep(0x1f800000,0);              /* a1 indeterminate */
      rec = T + side * 0x30;
      aidx = MB8(MB32(0x1f8000c0) + side + 0x1b);
      aidx = MB8(0x801b23ac + aidx);
      side ^= 1;
      *(uint32_t *)(rec + 0)    = 0x59;
      *(uint32_t *)(rec + 0x24) = 0;
      *(uint32_t *)(rec + 0x10) = 0;
      *(uint32_t *)(rec + 0x1c) = aidx;
      *(uint32_t *)(T + side * 0x30 + 0x24) = 1;
      MW8(0x1f800001,1);
    }
    goto L_tail;

L_casef0:                                     /* 0x8014df34 */
    deferred_cmd_post(0x204,0,0);
    victory_jingle_schedule(MB8(0x1f8000eb),MB8(0x1f8001f3));
    MW8(0x1f800001,0xf1);
    MW8(0x1f800001,0xff);
    goto L_tail;

L_tail:                                       /* 0x8014df6c */
    if (MB8(0x1f800001) < 2) goto L_aeec;
    task_pools_tick(1);
    task_pools_tick(2);
    if (MB8(0x1f800001) >= 5 && (B36_32(0x34) & 0x800u) != 0) {
      /* frame timer -> sp+0x138 text, then descblk_variant_e banner */
      int32_t secs;
      {
        int32_t fcnt = (int32_t)B36_32(0x2c);
        int64_t prod = (int64_t)fcnt * (int64_t)(int32_t)0x51eb851f;
        int32_t mulhi = (int32_t)(prod >> 32);
        secs = (mulhi >> 4) - (fcnt >> 31);
      }
      width3_num_format((uint8_t *)b36_sp + 0x138,secs,1);
      B36_32(0x10) = (MB8(0x1f800015) != 0) ? 0xf0u : 0x78u;
      B36_32(0x14) = 0xe;
      descblk_variant_e(MB32(0x1f800008),MB32(0x1f80000c),
                   (uint8_t *)b36_sp + 0x138,0xda,
                   (int32_t)B36_32(0x10),(int32_t)B36_32(0x14));
    }
    if ((B36_32(0x34) & 8u) != 0)
      ot_push(B36_ENG() + 0xb40, B36_16S(0xa8));
    if ((B36_32(0x34) & 0x10u) != 0) {
      /* 2 iters: rec_i->f0 & 8 -> two OT pushes, slot lh 0xaa */
      int32_t i;
      for (i = 0; i < 2; i++) {
        uint8_t *r = T + (uint32_t)i * 0x30;
        uint32_t io = (uint32_t)i * 0x28;
        if ((*(uint32_t *)(r + 0) & 8u) != 0) {
          ot_push(B36_ENG() + io + 0xa50, B36_16S(0xaa));
          ot_push(B36_ENG() + io + 0xaf0, B36_16S(0xaa));
        }
      }
    }
    if ((B36_32(0x34) & 0x20u) != 0) {
      /* per-record anim/coord update, 2 iters.
       * s7=T+0x30i, fp=0xaa0+0x28i, s1=0x28i, s4=ctx+4i,
       * S32(0x148)=0x30i, S32(0x14c)=2i. */
      int32_t i;
      uint32_t poff = 0xaa0, eoff = 0;
      B36_32(0x144) = guest_of(ctx);
      B36_32(0x148) = 0;
      B36_32(0x14c) = 0;
      for (i = 0; i < 2; i++) {
        uint8_t *rec = T + (uint32_t)i * 0x30;
        uint8_t *ent;
        uint32_t ga;
        int32_t f24 = (int32_t)*(uint32_t *)(rec + 0x24);
        B36_32(0x148) = (uint32_t)i * 0x30;
        B36_32(0x14c) = (uint32_t)i * 2;
        if ((*(uint32_t *)(ctx + (uint32_t)i * 0x30 + 0x28) & 8u) != 0) {
          /* rec_i->f0 & 8 (ctx+0x30i+0x28 == T+0x30i) */
          int32_t f1c = (int32_t)*(uint32_t *)(rec + 0x1c);
          ent = (uint8_t *)guest_at(0x801b268c + (uint32_t)f1c * 8);
          ga = 0x801b2684 + (uint32_t)f1c * 8;
          B36_32(0x10) = (uint32_t)(int32_t)*(int16_t *)(ent + 4);
          B36_32(0x14) = (uint32_t)(int32_t)*(int16_t *)(ent + 6);
          sprite_place_centered(B36_ENG() + poff,0x1f800000,
                       (int32_t)*(int16_t *)(ent + 0),
                       (int32_t)*(int16_t *)(ent + 2),
                       (int32_t)B36_32(0x10),(int32_t)B36_32(0x14));
          if (i == 0) {
            B36_32(0x10) = 0x20; B36_32(0x14) = 0x18;
            sprite_place_centered(B36_ENG() + 0xb90,0x1f800000,
                         (int16_t)(MB16(ga + 0) + 0xd),
                         (int16_t)(MB16(ga + 2) - 0x11),0x20,0x18);
          } else {
            B36_32(0x10) = 0x20; B36_32(0x14) = 0x18;
            sprite_place_centered(B36_ENG() + eoff + 0xb90,0x1f800000,
                         (int16_t)(MB16(ga + 0) - 0xc),
                         (int16_t)(MB16(ga + 2) + 0x14),0x20,0x18);
          }
          if (*(uint32_t *)(rec + 0x10) == 0) {
            /* src = s2 + S32(0x14c); bytes 0/1 */
            uint8_t *src = (uint8_t *)guest_at(ga + B36_32(0x14c));
            uint32_t dg = B36_ENG() + eoff;
            MW8(dg + 0xb9c, src[0]);
            MW8(dg + 0xb9d, src[1]);
            MW8(dg + 0xba4, (uint8_t)(src[0] + 0x20));
            MW8(dg + 0xba5, src[1]);
            MW8(dg + 0xbac, src[0]);
            MW8(dg + 0xbad, (uint8_t)(src[1] + 0x18));
            MW8(dg + 0xbb4, (uint8_t)(src[0] + 0x20));
            MW8(dg + 0xbb5, (uint8_t)(src[1] + 0x18));
          } else if (*(uint32_t *)(rec + 0x10) == 1) {
            /* s2 bytes 4/5 */
            uint8_t *src = (uint8_t *)guest_at(ga);
            uint32_t dg = B36_ENG() + eoff;
            MW8(dg + 0xb9c, src[4]);
            MW8(dg + 0xb9d, src[5]);
            MW8(dg + 0xba4, (uint8_t)(src[4] + 0x20));
            MW8(dg + 0xba5, src[5]);
            MW8(dg + 0xbac, src[4]);
            MW8(dg + 0xbad, (uint8_t)(src[5] + 0x17));
            MW8(dg + 0xbb4, (uint8_t)(src[4] + 0x20));
            MW8(dg + 0xbb5, (uint8_t)(src[5] + 0x17));
          }
          if (f24 > 0 && f24 < 3) {
            uint32_t h0 = B36_32(0x1c + 4 * i);
            uint32_t h1 = B36_32(0x24 + 4 * i);
            uint16_t tag;
            if (h0 != 0) { task_node_free(h0); B36_32(0x1c + 4 * i) = 0; }
            if (h1 != 0) { task_node_free(h1); B36_32(0x24 + 4 * i) = 0; }
            if (*(uint32_t *)(rec + 0x10) == 0) tag = 0x7c1f;
            else if (*(uint32_t *)(rec + 0x10) == 1) tag = 0x79db;
            else goto L_f20_ot;
            {
              uint8_t *d0 = (uint8_t *)guest_at(MB32(0x801f10c0) + eoff);
              uint8_t *d1 = (uint8_t *)guest_at(MB32(0x801f10c4) + eoff);
              uint32_t g0 = guest_of(d0), g1 = guest_of(d1);
              MW16(g0 + 0xaae, tag);
              MW16(g1 + 0xaae, tag);
              MW16(g0 + 0xb9e, tag);
              MW16(g1 + 0xb9e, tag);
            }
          } else {
            int32_t f1c = (int32_t)*(uint32_t *)(rec + 0x1c);
            if ((MB32(0x801f10b8) & (1u << (f1c & 31))) != 0) {
              record_flag_byte_set(B36_32(0x1c + 4 * i));
              record_flag_byte_set(B36_32(0x24 + 4 * i));
            } else {
              uint32_t dg = B36_ENG() + eoff;
              MW16(dg + 0xaae, 0x7a9c);
              record_flag_byte_clear(B36_32(0x1c + 4 * i));
              record_flag_byte_clear(B36_32(0x24 + 4 * i));
            }
          }
L_f20_ot:
          ot_push(B36_ENG() + eoff + 0xaa0, B36_16S(0xac));
          ot_push(B36_ENG() + eoff + 0xb90, B36_16S(0xac));
        }
        poff += 0x28; eoff += 0x28;
      }
    }
    if ((B36_32(0x34) & 0x40u) != 0) {
      /* 15 iters over pad bits 0..14: push eng+0x7f8+0x28i when
       * (padmask>>i)&1 or (0xff8>>i)&1, slot lh 0xae */
      int32_t i;
      uint32_t pm = MB32(0x801f10b8);
      for (i = 0; i < 0xf; i++) {
        if ((pm & (1u << i)) != 0 || ((0xff8u >> i) & 1u) != 0)
          ot_push(B36_ENG() + 0x7f8 + (uint32_t)i * 0x28,
                      B36_16S(0xae));
      }
    }
    if ((B36_32(0x34) & 2u) != 0) {
      /* 2 iters; a1=ctx+0x30i reads rec_i->f0(+0x28) and rec_i+0x2c
       * (+0x54). cond1 f0&0x10 -> pushes at +0/+0x50; cond2 f0&8 ->
       * +0xa0 and (i==1) eng+0xf0. slot lh 0xa4. */
      int32_t i;
      for (i = 0; i < 2; i++) {
        uint8_t *crec = ctx + (uint32_t)i * 0x30;
        uint32_t io = (uint32_t)i * 0x28;
        uint32_t f0 = *(uint32_t *)(crec + 0x28);
        uint32_t f2c = *(uint32_t *)(crec + 0x54);
        if ((f0 & 0x10u) != 0 && f2c == 0) {
          ot_push(B36_ENG() + io + 0x0,  B36_16S(0xa4));
          ot_push(B36_ENG() + io + 0x50, B36_16S(0xa4));
        }
        if ((f0 & 8u) != 0 && f2c == 0) {
          ot_push(B36_ENG() + io + 0xa0, B36_16S(0xa4));
          if (i == 1)
            ot_push(B36_ENG() + 0xf0, B36_16S(0xa4));
        }
        if (f2c != 0) *(uint32_t *)(crec + 0x54) = 0;
      }
    }
    if ((B36_32(0x34) & 4u) != 0) {
      /* 2 iters over rec_i (s7=T+0x30i, fp=0x28i, t0=0x258+0x28i).
       * simple path (eb1c) iff (f10==0 && f24==0) or
       * (f10==1 && f24<2 && !((other->f0>>1)&1)); else single push at
       * eng+0x28i+0x258. Simple path runs 8x sprite_rect_set_c + pushes. */
      int32_t i;
      uint32_t eoff = 0x258;
      uint8_t *rec = T;
      for (i = 0; i < 2; i++, rec += 0x30, eoff += 0x28) {
        uint32_t f10 = *(uint32_t *)(rec + 0x10);
        uint32_t f24 = *(uint32_t *)(rec + 0x24);
        int simple;
        if (f10 == 0) {
          simple = (f24 == 0);
        } else if (f10 == 1) {
          uint8_t *o = T + (uint32_t)(i ^ 1) * 0x30;
          simple = ((int32_t)f24 < 2 &&
                    ((*(uint32_t *)(o + 0) >> 1) & 1u) == 0);
        } else {
          simple = 0;
        }
        if (!simple) {
          ot_push(B36_ENG() + (uint32_t)i * 0x28 + 0x258,
                      B36_16S(0xa6));
        } else {
          /* s0 = (12*B36_32(0x2c) mod 272) + 0x50 via mult 0x78787879 */
          int32_t raw = (int32_t)B36_32(0x2c) * 12;
          int64_t prod = (int64_t)raw * (int64_t)(int32_t)0x78787879;
          int32_t mulhi = (int32_t)(prod >> 32);
          int32_t q = (mulhi >> 7) - (raw >> 31);
          int32_t pos = raw - q * 272 + 0x50;
          int j;
          uint32_t base = (i == 0) ? 0x2a8 : 0x410;
          int32_t attr1 = (i == 0) ? 0 : 0x100;
          int32_t attr3 = (i == 0) ? 0x100 : 0x102;
          for (j = 0; j < 8; j++) {
            if (pos >= 0x161) pos -= 0x110;
            B36_32(0x150) = eoff;
            sprite_rect_set_c(B36_ENG() + base + (uint32_t)j * 0x28,
                         attr1,pos,attr3,0x22);
            ot_push(B36_ENG() + (uint32_t)j * 0x28 + base,
                        B36_16S(0xa6));
            pos += 0x22;
          }
        }
      }
    }
    if ((B36_32(0x34) & 0x80u) != 0) {
      /* 4 iters base 0x578 slot lh 0xb0, then single eng+0xb68
       * slot lh 0xb2, then ko_count_announce */
      int32_t i;
      for (i = 0; i < 4; i++)
        ot_push(B36_ENG() + 0x578 + (uint32_t)i * 0x28,
                    B36_16S(0xb0));
      ot_push(B36_ENG() + 0xb68, B36_16S(0xb2));
      ko_count_announce(0x1f800000,
                   MB32(0x1f800008) +
                   (uint32_t)MBS16(0x801b238a) * 4u);
    }
    if ((B36_32(0x34) & 0x100u) != 0) {
      int32_t i;
      for (i = 0; i < 4; i++)
        ot_push(B36_ENG() + 0x618 + (uint32_t)i * 0x28,
                    B36_16S(0xb0));
    }
    if ((B36_32(0x34) & 0x200u) != 0) {
      int32_t i;
      for (i = 0; i < 4; i++)
        ot_push(B36_ENG() + 0x6b8 + (uint32_t)i * 0x28,
                    B36_16S(0xb0));
    }
    if ((B36_32(0x34) & 0x400u) != 0) {
      int32_t i;
      for (i = 0; i < 4; i++)
        ot_push(B36_ENG() + 0x758 + (uint32_t)i * 0x28,
                    B36_16S(0xb0));
    }
    if ((B36_32(0x34) & 1u) != 0) {
      int32_t i;
      for (i = 0; i < 4; i++) {
        ot_push(B36_ENG() + 0x118 + (uint32_t)i * 0x28,
                    B36_16S(0xa2));
        ot_push(B36_ENG() + 0x1b8 + (uint32_t)i * 0x28,
                    B36_16S(0xa2));
      }
    }
L_aeec:                                       /* 0x8014f164 */
    frame_tick(0x1f800000);
    if (MB8(0x1f800001) != 0xff) continue;
    break;
  }

  /* exit @0x8014f17c — guest epilogue `addiu sp,+0x180` */
  MW8(0x1f800001,0);
  {
    uint32_t ret = B36_32(0x38);
    if (ret == 0) MW8(0x1f800098, B36_8(0x30));
    br2_guest_frame_free();
    return (int32_t)ret;
  }
}

/* battle_side_flag_prop — battle-side flag propagation @0x80153dec (62 insns).
 * param_1 = guest battle arg, param_2 = host ptr to the per-side arg
 * block. Everything is gated on a1+0x5c bit 0x8; the nested pointers
 * at a1+0x38 / a1+0x2c hold guest addresses (the block lives in guest
 * space), so they are dereferenced through the MB/MBS guest macros. The record at
 * *(param_1+0xcc) is a guest pointer; only its +0xa halfword is
 * written. */
void battle_side_flag_prop(uint32_t battle,uint8_t *side)
{

  uint32_t fl5c = *(uint32_t *)(side + 0x5c);
  uint32_t rec;
  if ((fl5c & 8u) == 0) return;
  if ((fl5c & 0x40u) != 0 &&
      (*(uint32_t *)(side + 0x6c) & 0x02000000u) == 0 &&
      MBS8(*(uint32_t *)(side + 0x38) + 0x7e) < 0) {
    rec = MB32(battle + 0xcc);
    MW16(rec + 0xa,1);
    if ((side[5] & 1) != 0) {
      rec = MB32(battle + 0xcc);
      MW16(rec + 0xa,(uint16_t)(MB16(rec + 0xa) | 0x200u));
    }
  }
  if ((*(uint32_t *)(side + 0x64) & 0x2000u) != 0) {
    rec = MB32(battle + 0xcc);
    MW16(rec + 0xa,1);
    if ((side[5] & 1) != 0) {
      rec = MB32(battle + 0xcc);
      MW16(rec + 0xa,(uint16_t)(MB16(rec + 0xa) | 0x100u));
    }
  }
  if ((MB32(*(uint32_t *)(side + 0x2c) + 0x58) & 0x40000000u) != 0) {
    rec = MB32(battle + 0xcc);
    MW16(rec + 0xa,1);
  }
}

/* engine_tables_restore — snapshot/restore of the two 0x10c0-byte engine
 * record tables @0x80128ef0 (41 insns). s0 = guest ptr at a0+0xcc;
 * clears its halfword 0, word-copies 0x21c0 bytes s0+0x18 ->
 * 0x801e1b3c and 0xbd0 bytes s0+0x21d8 -> 0x801e0f6c, then
 * victory_record_restore(battle,4) and stores the low halfword at s0+6. */
void engine_tables_restore(uint32_t battle)
{
  uint32_t tbl = MB32(battle + 0xcc);
  MW16(tbl,0);
  memcpy(guest_at(0x801e1b3c),guest_at(tbl + 0x18),0x21c0);
  memcpy(guest_at(0x801e0f6c),guest_at(tbl + 0x21d8),0xbd0);
  MW16(tbl + 6,(uint16_t)victory_record_restore(battle,4));
}

/* ======== batch37 ======== */

/* shared callees not yet declared in this file */
extern uint32_t offset_table_lookup(uint32_t,int32_t);
extern void packed_u16_expand(uint32_t,uint32_t,uint32_t);
extern void versus_stage_commit_b(uint32_t,int32_t);
extern void cmd_queue_push(int32_t,int32_t,int32_t,int32_t,uint32_t);

/* battle_bringup_b — resumable battle-init state machine @0x80147df4
 * (223 insns, jr $ra @0x80148168). a0 = guest battle base
 * (0x1f800000; its +0x8c s16 is the resume state), a1/a2/a3 = HOST
 * pointers to the caller's frame output tables (11/12/6 u32 slots).
 * tag_table_get() returns the host-side tag table used as the int*
 * arg to stream_block_decode/offset_table_lookup. Globals: cursor pair
 * 0x801c47b8 / 0x801e3cfc, flags 0x801c43a0 / 0x801e3d00.
 * Jump table @0x80120330 on state 0..0x30: only states
 * 0/4/8/0x10/0x30 have bodies; each state re-runs its gate
 * cd_file_load_sm and on failure stores the state back and returns 0.
 * On full completion stores -1 to +0x8c and returns 1. */
int battle_bringup_b(uint32_t battle,void *arr1,void *arr2,void *arr3)
{
  uint32_t *o1 = (uint32_t *)arr1;              /* s6: 11 slots */
  uint32_t *o2 = (uint32_t *)arr2;              /* s4: 12 slots */
  uint32_t *o3 = (uint32_t *)arr3;              /* s7: 6 slots  */
  uint32_t ttab;
  int32_t state, cur_st;
  uint32_t cur, r;
  int i;

  ttab = guest_of(tag_table_get());         /* tag table 0x801c47c0 */
  state = MBS16(battle + 0x8c);
  if (state < 0) return -1;
  cur_st = state;                                    /* cur_st = v1 before dispatch */
  if ((uint32_t)state >= 0x31) goto L_store;     /* sltiu 0x31 -> default */

  switch (state) {
  case 0:  goto L_state0;
  case 4:  goto L_state4;
  case 8:  goto L_state8;
  case 0x10: goto L_state10;
  case 0x30: goto L_state30;
  default: goto L_store;
  }

L_state0:                                     /* 0x80147e74 */
  cd_file_load_sm(-1,0);
  MW32(0x801c43a0,0);
  MW8(0x801e3d00,0xff);
  MW32(0x801c47b8,0x80010000);
  MW32(0x801e3cfc,0x800dc800);
  cur_st = 4;
L_state4:                                     /* 0x80147eb4 */
  if (cd_file_load_sm(0x8b,MB32(0x801c47b8)) == 0) goto L_store;
  fighter_record_apply(MB32(0x801c47b8),1);
  cur_st = 8;                                     /* delay slot of jal 566a0 */
  page_table_ptr_set(1,1,(undefined *)0);
  cd_timer_read(0);
L_state8:                                     /* 0x80147eec */
  if (cd_file_load_sm(0x8c,MB32(0x801c47b8)) == 0) goto L_store;
  fighter_record_apply(MB32(0x801c47b8),2);
  cur_st = 0x10;
  page_table_ptr_set(0x11,2,(undefined *)0);
  cd_timer_read(0);
L_state10:                                    /* 0x80147f24 */
  /* gen 0x80147F28: a1 = *(0x801C47B8) = heap-A cursor — file 0x82 (the
   * o2[] offset table) loads INTO the heap data region, NOT the tag table.
   * Passing ttab (0x801c47c0) overwrote the master decode tag table, so the
   * offset_table_lookup below read foreign data -> wild +1.8MB cursor jump. */
  if (cd_file_load_sm(0x82,MB32(0x801c47b8)) == 0) goto L_store;
  cur_st = 0;
  for (i = 0; i < 0xc; i++)                   /* 0x80147f44 loop */
    o2[i] = offset_table_lookup(MB32(0x801c47b8),i + 0x10);
  /* absolute store: cursor = o2[11] + (o2[1]-o2[0])  (host reads) */
  MW32(0x801c47b8,o2[0xb] + (o2[1] - o2[0]));
  cur_st = 0x30;
L_state30:                                    /* 0x80147f84 */
  if (cd_file_load_sm(0x81,ttab) == 0) goto L_store;
  scratch_013_set(0);
  for (i = 0; i < 0xb; i++) {                 /* 0x80147fac loop */
    cur = MB32(0x801c47b8);
    r = stream_block_decode(cur,(int *)guest_at(ttab),i);
    o1[i] = cur;
    MW32(0x801c47b8,cur + r);
  }
  for (i = 0; i < 6; i++) {                  /* 0x80147fe8 loop */
    cur = MB32(0x801c47b8);
    r = stream_block_decode(cur,(int *)guest_at(ttab),i + 0x20);
    o3[i] = cur;
    MW32(0x801c47b8,cur + r);
  }
  /* interleaved 63154 cursor-advance + 8bf40 pairs. Each advance
   * stores the PREVIOUS call's return into 0x801c47b8 in the next
   * call's delay slot; the 0x34/0x35 steps read the SECOND cursor
   * 0x801e3cfc (never advanced here) while 0x801c47b8 keeps
   * accumulating every return. */
  r = stream_block_decode(MB32(0x801c47b8),(int *)guest_at(ttab),0x31);
  spr_draw_off(MB32(0x801c47b8),0x300,0x100,1);
  MW32(0x801c47b8,MB32(0x801c47b8) + r);
  r = stream_block_decode(MB32(0x801c47b8),(int *)guest_at(ttab),0x32);
  spr_draw_off(MB32(0x801c47b8),0x280,0x100,1);
  MW32(0x801c47b8,MB32(0x801c47b8) + r);
  r = stream_block_decode(MB32(0x801c47b8),(int *)guest_at(ttab),0x30);
  spr_draw_off(MB32(0x801c47b8),0,0x1e0,1);
  MW32(0x801c47b8,MB32(0x801c47b8) + r);      /* += r30, w/ 0x34 call */
  r = stream_block_decode(MB32(0x801e3cfc),(int *)guest_at(ttab),0x34);
  spr_draw_off(MB32(0x801e3cfc),0x2c0,0,1);
  MW32(0x801c47b8,MB32(0x801c47b8) + r);      /* += r34, w/ 0x35 call */
  r = stream_block_decode(MB32(0x801e3cfc),(int *)guest_at(ttab),0x35);
  spr_draw_off(MB32(0x801e3cfc),0x2c0,0x100,1);
  MW32(0x801c47b8,MB32(0x801c47b8) + r);      /* += r35, w/ 9a460 */
  cd_timer_read(0);
  scratch_013_set(1);
  MW16(battle + 0x8c,0xffff);
  return 1;

L_store:                                      /* 0x8014813c */
  MW16(battle + 0x8c,(uint16_t)cur_st);
  return 0;
}

/* width3_num_format — width-3 numeric formatter @0x80148cb0 (113 insns).
 * a0 = host output buf, a1 = value (reduced mod 1000 via the
 * 0x10624dd3 magic), a2 = mode: 0 = plain decimal (returns pointer
 * to first digit, buf+4 max), 1 = space-padded, 2 = zero-padded,
 * other = buf unchanged. The guest returns the digit pointer in v0;
 * the sole caller treats it as void so the value is dropped here. */
void width3_num_format(void *out,int32_t val,int32_t mode)
{
  uint8_t *buf = (uint8_t *)out;
  int32_t v = val;
  int32_t q, i;

  /* val %= 1000 via mult 0x10624dd3, mulhi>>6 - sign */
  {
    int32_t mulhi = (int32_t)(((int64_t)v * 0x10624dd3LL) >> 32);
    int32_t d = (mulhi >> 6) - (v >> 31);
    v -= d * 1000;
  }

  if (mode == 1) goto L_mode1;
  if (mode < 2) {
    if (mode == 0) goto L_mode0;
    return;                                   /* dead: mode==1 handled */
  }
  if (mode == 2) goto L_mode2;
  return;                                     /* v0 = buf */

L_mode0:                                      /* 0x80148d1c */
  buf[5] = 0;
  if (v == 0) {
    buf[4] = '0';                             /* ret = buf+4 (dropped) */
    return;
  }
  {
    uint8_t *p = buf + 5;
    do {
      int32_t mulhi = (int32_t)(((int64_t)v * 0x66666667LL) >> 32);
      q = (mulhi >> 2) - (v >> 31);              /* v / 10 */
      *--p = (uint8_t)(v - q * 10 + 0x30);
      v = q;
    } while (v != 0);
  }
  return;

L_mode1:                                      /* 0x80148d80 */
  if (v == 0) {
    buf[0] = 0x20; buf[1] = 0x20; buf[2] = '0';
  } else {
    for (i = 2; i >= 0; i--) {
      if (v != 0) {
        int32_t mulhi = (int32_t)(((int64_t)v * 0x66666667LL) >> 32);
        q = (mulhi >> 2) - (v >> 31);
        buf[i] = (uint8_t)(v - q * 10 + 0x30);
        v = q;
      } else {
        buf[i] = 0x20;
      }
    }
  }
  buf[3] = 0;
  return;

L_mode2:                                      /* 0x80148e00 */
  buf[0] = '0'; buf[1] = '0'; buf[2] = '0';
  for (i = 2; i >= 0; i--) {
    if (v != 0) {
      int32_t mulhi = (int32_t)(((int64_t)v * 0x66666667LL) >> 32);
      q = (mulhi >> 2) - (v >> 31);
      buf[i] = (uint8_t)(v - q * 10 + 0x30);
      v = q;
    }
  }
  buf[3] = 0;
  return;
}

/* width3_num_space_pad — CPS-split continuation of width3_num_format
 * @0x80148d80 (mode mode==1, space-padded 3-digit field). Reached in the
 * guest by a `j` from 0x80148cf0 with the reduced value already in
 * $val and the output buffer moved to $a3; $out/$mode are dead on entry
 * ($mode is reloaded with the loop count 2). value==0 emits "  0" style
 * (0x20,0x20,'0'); otherwise digits fill buf[2..0] right-to-left and
 * leading positions that outlive the value become 0x20. NUL at buf[3]
 * comes from the shared 0x80148e60 tail in the guest. */
void width3_num_space_pad(int32_t unused_a,int32_t val,int32_t unused_b,
                    uint8_t *out)
{
  uint8_t *buf = out;
  int32_t v = val;
  int32_t q, i;
  (void)unused_a;
  (void)unused_b;

  if (v == 0) {
    buf[0] = 0x20; buf[1] = 0x20; buf[2] = '0';
    buf[3] = 0;
    return;
  }
  for (i = 2; i >= 0; i--) {
    if (v != 0) {
      int32_t mulhi = (int32_t)(((int64_t)v * 0x66666667LL) >> 32);
      q = (mulhi >> 2) - (v >> 31);                /* v / 10 */
      buf[i] = (uint8_t)(v - q * 10 + 0x30);
      v = q;
    } else {
      buf[i] = 0x20;
    }
  }
  buf[3] = 0;
}

/* width3_num_zero_pad — CPS-split continuation of width3_num_format
 * @0x80148e00 (mode a2==2, zero-padded 3-digit field). Same entry ABI
 * as width3_num_space_pad (value in $a1, buf in $a3). buf[0..2] are
 * pre-filled with '0' so positions that outlive the value stay zero;
 * NUL at buf[3] via the shared 0x80148e60 tail. */
void width3_num_zero_pad(int32_t unused_a,int32_t val,int32_t unused_b,
                   uint8_t *out)
{
  uint8_t *buf = out;
  int32_t v = val;
  int32_t q, i;
  (void)unused_a;
  (void)unused_b;

  buf[0] = '0'; buf[1] = '0'; buf[2] = '0';
  for (i = 2; i >= 0; i--) {
    if (v != 0) {
      int32_t mulhi = (int32_t)(((int64_t)v * 0x66666667LL) >> 32);
      q = (mulhi >> 2) - (v >> 31);                /* v / 10 */
      buf[i] = (uint8_t)(v - q * 10 + 0x30);
      v = q;
    }
  }
  buf[3] = 0;
}

/* packed_field_store — packed-field writer @0x80148f30 (9 insns).
 * 0x801f10b8 = ((a0 & 0x3ff) << 3); the store is the beqz delay
 * slot so it always runs, then (a0 & 0x400) rewrites it |4. */
void packed_field_store(uint32_t val)
{
  uint32_t ctl_w = (val & 0x3ffu) << 3;
  MW32(0x801f10b8,ctl_w);
  if (val & 0x400u) MW32(0x801f10b8,ctl_w | 4u);
}

/* side_flag_merge — per-side flag merge @0x8014a5bc (58 insns).
 * val = guest battle base, a1 = host ctx. Two passes (i=0,1) over
 * ctx+0x28+i*0x30; the +0x9b!=0 arm sets BOTH fixed slots +0x28/+0x58
 * (not the per-i slot). Otherwise bit 0x40 is set on the per-i slot
 * unless select_phase_gate/gate_read_ec/phase checks say the side is live:
 * set when (53484==0 && (92&0xf00)!=0x300) or the chain yields 0. */
void side_flag_merge(uint32_t battle,void *ctxp)
{
  uint8_t *ctx = (uint8_t *)ctxp;
  int i;
  for (i = 0; i < 2; i++) {
    uint32_t *slot = (uint32_t *)(ctx + 0x28 + i * 0x30);
    int32_t t;
    if (MB8(battle + 0x9b) != 0) {
      *(uint32_t *)(ctx + 0x28) |= 0x40u;
      *(uint32_t *)(ctx + 0x58) |= 0x40u;
      continue;
    }
    if (select_phase_gate(battle,(uint32_t)i) != 0) {
      t = gate_read_ec(battle,(uint32_t)(i ^ 1));
      if (t == 0) *slot |= 0x40u;
    } else if ((MB16(battle + 0x92) & 0xf00u) != 0x300u) {
      *slot |= 0x40u;
    } else {
      t = (int32_t)(MB16(battle + 2 * i + 0xb4) & 0x1000u);
      if (t == 0) *slot |= 0x40u;
    }
  }
}

/* ctx_flag_set — ctx flag setter @0x8014a6a8 (23 insns, leaf).
 * battle unused. ctx+0x14 = 0x1387; for i=0,1 if the CROSS slot
 * ctx+(i^1)*0x30+0x38 == 1 then ctx+0x28+i*0x30 |= 0x18. */
void ctx_flag_set(uint32_t unused,void *ctxp)
{
  uint8_t *ctx = (uint8_t *)ctxp;
  uint32_t off = 0x28;
  int i;
  (void)unused;
  *(uint32_t *)(ctx + 0x14) = 0x1387u;
  for (i = 0; i < 2; i++) {
    if (*(uint32_t *)(ctx + (uint32_t)(i ^ 1) * 0x30 + 0x38) == 1)
      *(uint32_t *)(ctx + off) |= 0x18u;
    off += 0x30;
  }
}

/* engine_record_pair_update — paired engine-record updater @0x8014a708 (62 insns).
 * unused = guest battle base (passthrough to palette_attr_record_write), ctxp = host ctx.
 * Two iterations: palette_attr_record_write on engine[i]+0xa50/+0xa78 with the
 * ctx+0x44/+0x74 u32 args, then packed_u16_expand on engine0/1 + i*0x28
 * and +0x50+i*0x28 with ctx+i*0x30+0x44. */
void engine_record_pair_update(uint32_t battle,void *ctxp)
{
  uint8_t *ctx = (uint8_t *)ctxp;
  int i;
  for (i = 0; i < 2; i++) {
    uint32_t eng_ent = MB32(0x801f10c0 + (uint32_t)i * 4u);
    uint32_t pk_arg;
    palette_attr_record_write(battle,eng_ent + 0xa50,0,
                 *(uint32_t *)(ctx + 0x44));
    palette_attr_record_write(battle,eng_ent + 0xa78,1,
                 *(uint32_t *)(ctx + 0x74));
    pk_arg = *(uint32_t *)(ctx + (uint32_t)i * 0x30 + 0x44);
    packed_u16_expand(MB32(0x801f10c0) + (uint32_t)i * 0x28,
                 MB32(0x801f10c4) + (uint32_t)i * 0x28,pk_arg);
    packed_u16_expand(MB32(0x801f10c0) + 0x50 + (uint32_t)i * 0x28,
                 MB32(0x801f10c4) + 0x50 + (uint32_t)i * 0x28,pk_arg);
  }
}

/* gpu_side_packet_emit — GPU-packet emitter @0x8014a918 (113 insns).
 * battle = guest record ptr, ctxp = side index, a2 = mode. Always sets
 * u32 @MB32(0x801f10bc)+ctxp*0x30+0x54 = 1 first (beq delay slot).
 * mode 0/1 emit cmd_queue_push packets (record+8 word added when
 * rec+4 bit3 set, then +0x14); modes 2..5 emit spr_draw_pair with
 * halfword pairs from table 0x801b253c indexed by a2-2. */
void gpu_side_packet_emit(uint32_t rec,int32_t side,int32_t mode)
{
  uint32_t spkt = MB32(0x801f10bc) + (uint32_t)side * 3u * 0x10u;
  MW32(spkt + 0x54,1);                           /* unconditional */

  if (mode == 1) goto L_mode1;
  if (mode < 2) {
    if (mode == 0) goto L_mode0;
    return;
  }
  if (mode < 6) goto L_modeMid;
  return;

L_mode0:                                      /* 0x8014a97c */
  if ((MB32(rec + 4) & 8u) == 0) return;
  {
    uint32_t rec14 = rec + 0x14;
    rec += MB32(rec + 8) + 0x14;
    if (side == 0)
      cmd_queue_push(0,0x1f5,0x100,1,rec14);
    else
      cmd_queue_push(0,0x1f6,0x100,1,rec14);
    cmd_queue_push(side == 0 ? 0x200 : 0x260,0,0x60,0x100,rec);
  }
  return;

L_mode1:                                      /* 0x8014a9fc */
  if ((MB32(rec + 4) & 8u) != 0) rec += MB32(rec + 8);
  rec += 0x14;
  cmd_queue_push(side == 0 ? 0x340 : 0x360,0,0x18,0x60,rec);
  return;

L_modeMid:                                    /* 0x8014aa50, mode in 2..5 */
  {
    int32_t idx = mode - 2;
    int32_t h1 = MBS16(0x801b253c + (uint32_t)idx * 4);
    int32_t h2 = MBS16(0x801b253c + (uint32_t)idx * 4 + 2);
    spr_draw_pair(rec,(uint32_t)h1,(uint32_t)h2,1,
                 side == 0 ? 0x300u : 0x320u,0,1);
  }
  return;
}

/* record_flag_byte_clear — record byte clear @0x801a4c60 (4 insns).
 * if (rec) *(u8*)(rec+0x17) = 0. rec is a guest ptr. */
void record_flag_byte_clear(uint32_t rec)
{
  if (rec != 0) MW8(rec + 0x17,0);
}

/* record_flag_byte_set — record byte set @0x801a4c74 (4 insns).
 * v0=1 is the beqz delay slot so it returns 1 either way; the sole
 * caller ignores it, matching the void extern. */
void record_flag_byte_set(uint32_t rec)
{
  if (rec != 0) MW8(rec + 0x17,1);
}

/* vs_confirm_a — versus-confirm phase stepper @0x80153ab0 (207
 * insns through jr $ra @0x80153de4; the 0x80153dec tail is the
 * already-ported battle_side_flag_prop). rec/a1 = host side blocks,
 * a2 = guest battle state s0, a3 = flag. Dispatch on
 * MB8(MB32(s0+0xcc)+0xa) via table @0x80120a30 (0->return,
 * 1->0x80153b0c, 2->0x80153b34, 3->0x80153b70, 4->0x80153c7c,
 * 5->0x80153d90). The sub-record at MB32(s0+0xcc) is a guest ptr;
 * +0xc counts via lhu-store / lh-compare, +0xa/+0xe are u16
 * counters. Shared tails: 0x80153d74 = rec.a++ only;
 * 0x80153c5c = rec.a++ + s0+0x9e=0 (reached with rec.c already 0). */
void vs_confirm_a(uint8_t *blk_a,uint8_t *blk_b,uint32_t battle,
                  uint32_t flag)
{




  uint32_t rec, kind;
  int32_t ret;
  {
    decomp_trace_record(49,(uint32_t)(uintptr_t)blk_a,(uint32_t)(uintptr_t)blk_b,battle,flag); }

  rec = MB32(battle + 0xcc);
  kind = MB8(rec + 0xa);
  if (kind >= 6) return;

  switch (kind) {
  case 0:
    return;                                   /* -> 0x80153dd0 */

  case 1:                                     /* 0x80153b0c */
    MW16(MB32(battle + 0xcc) + 0xe,MB16(battle + 0x94));
    MW16(MB32(battle + 0xcc) + 0xc,0);            /* jal delay slot */
    battle_subrecord_set2(battle);
    goto L_reca;                            /* 0x80153d74 */

  case 2:                                     /* 0x80153b34 */
    rec = MB32(battle + 0xcc);
    MW16(rec + 0xc,(uint16_t)(MB16(rec + 0xc) + 1));
    rec = MB32(battle + 0xcc);
    if (MBS16(rec + 0xc) < 10) return;
    MW16(rec + 0xc,0);                        /* delay slot of j */
    goto L_recA_9e;                           /* 0x80153c5c */

  case 3:                                     /* 0x80153b70 */
    hud_announce(battle,0x10,(uint32_t)MBS16(battle + 0x8c));
    if (blk_a[0] == 1 && blk_b[0] == 1) s16_dec_with_floor(battle);
    rec = MB32(battle + 0xcc);
    if (MBS16(rec + 0xc) == 0) {
      battle_field_set(MB32(battle + 0xbc),flag != 0 ? 0x10 : 0xe);
      task_table_init(1);
      clear_f0300_records();
      versus_stage_commit(battle,1);
    }
    rec = MB32(battle + 0xcc);
    if (MBS16(rec + 0xc) == 1) MW8(battle + 0x9e,0);
    rec = MB32(battle + 0xcc);
    MW16(rec + 0xc,(uint16_t)(MB16(rec + 0xc) + 1));  /* jal delay */
    ret = subrecord_u16_rw(battle,-1);
    if (ret != 4) return;
    deferred_cmd_post(0x400,0x5f,0);
    MW16(MB32(battle + 0xcc) + 0xc,0);
    goto L_recA_9e;

  case 4:                                     /* 0x80153c7c */
    hud_announce(battle,0x10,(uint32_t)MBS16(battle + 0x8c));
    if (blk_a[0] == 1 && blk_b[0] == 1) s16_dec_with_floor(battle);
    rec = MB32(battle + 0xcc);
    if (MBS16(rec + 0xc) == 0) {
      battle_field_set(MB32(battle + 0xbc),flag != 0 ? 0x11 : 0xf);
      task_table_init(1);
      clear_f0300_records();
      versus_stage_commit_b(battle,1);
    }
    rec = MB32(battle + 0xcc);
    if (MBS16(rec + 0xc) == 2)
      MW8(battle + 0x9e,(uint8_t)(MB16(rec + 0xa) >> 8));
    rec = MB32(battle + 0xcc);
    MW16(rec + 0xc,(uint16_t)(MB16(rec + 0xc) + 1));  /* jal delay */
    ret = subrecord_u16_rw(battle,-1);
    if (ret != 4) return;
    deferred_cmd_post(0x400,0x60,0);
    MW16(MB32(battle + 0xcc) + 0xc,0);
    goto L_recA_9e;

  case 5:                                     /* 0x80153d90 */
    MW8(battle + 0xb0,0);
    MW16(MB32(battle + 0xbc) + 0x82,0xfffd);      /* jal delay slot */
    engine_tables_restore(battle);
    battle_subrecord_set2(battle);
    MW16(MB32(battle + 0xcc) + 0xa,0);
    MW16(battle + 0x94,MB16(MB32(battle + 0xcc) + 0xe));
    return;
  }
  return;

L_recA_9e:                                    /* 0x80153c5c */
  MW16(MB32(battle + 0xcc) + 0xa,(uint16_t)(MB16(MB32(battle + 0xcc) + 0xa) + 1));
  MW8(battle + 0x9e,0);                           /* j delay slot */
  return;

L_reca:                                     /* 0x80153d74 */
  MW16(MB32(battle + 0xcc) + 0xa,(uint16_t)(MB16(MB32(battle + 0xcc) + 0xa) + 1));
  return;
}

/* vs_confirm_b — versus round-end dispatcher @0x80152bdc (388
 * insns through jr $ra @0x801531e4). a0 = guest battle state s1,
 * a1/a2 = host side blocks s2/s3, a3 = order flag. Dispatch on
 * MB8(s1+0x9c) via table @0x801209c8 (0->0x80152c30, 1/2->0x80152d0c,
 * 3->0x80152df8, 4->0x80152e40, 5->0x80152cb0); >=6 falls into the
 * shared tail 0x80152ecc. Cases either jump straight to the epilogue
 * (byte stores at s1+0/1/2), run their own copy of the
 * 56ef4/2b23c/2aa6c tail (case 4), or fall into the shared phase
 * dispatch on MB16(s1+0x92)&0xf00 (0x200->0x80153040, 0x100->0x80152f20,
 * 0x300->0x8015316c). The 0x801531ac tail ends with s1+1 = 2 via
 * 0x801531cc; other v0 constants (1/5/0xb) reach it directly. */

/* shared scale step used by the two state-3 blocks: p = lh(side+0xb0)
 * * MBS16(s1+0x94); q = ((p*0x57619f1)>>32)>>7 - (p>>31); then
 * side+0xac = lhu(side+0xac) - q, clamped to 0 when bit15 set. */
static void side_scale_ac(uint8_t *side,uint32_t rec)
{
  int32_t prod = (int32_t)((int32_t)*(int16_t *)(side + 0xb0) *
                        MBS16(rec + 0x94));
  int32_t mulhi = (int32_t)(((int64_t)prod * 0x57619f1LL) >> 32);
  int32_t quot = (mulhi >> 7) - (prod >> 31);
  int32_t new_val = (int32_t)*(uint16_t *)(side + 0xac) - quot;
  *(int16_t *)(side + 0xac) = (int16_t)new_val;
  if (*(int16_t *)(side + 0xac) < 0) *(int16_t *)(side + 0xac) = 0;
}

void vs_confirm_b(uint32_t battle,uint8_t *blk_a,uint8_t *blk_b,int32_t flag)
{
  uint32_t mode, ph, crec;
  int32_t ac, b0;
  uint32_t b0u;

  mode = MB8(battle + 0x9c);
  if (mode >= 6) goto L_ecc;
  switch (mode) {
  case 0: goto L_case0;
  case 1: case 2: goto L_d10;
  case 3: goto L_case3;
  case 4: goto L_case4;
  case 5: goto L_case5;
  }
  return;

L_case0:                                      /* 0x80152c30 */
  ph = MB16(battle + 0x92) & 0xf00u;
  if (ph == 0x200u) {
    if (flag != 0) goto L_ecc;
    if (MBS16(battle + 0xa4) <
        MB8(MB32(battle + 0xc0) + 0x10)) goto L_ecc;
    goto L_3058;
  }
  if (ph < 0x201u) {
    if (ph != 0x100u) goto L_ecc;
    if (flag == 0) goto L_ecc;
    if (MBS16(battle + 0xa4) <
        MB8(MB32(battle + 0xc0) + 0x10)) goto L_ecc;
    goto L_2f38;
  }
  goto L_ecc;

L_case5:                                      /* 0x80152cb0 */
  if (MB8(battle + 0x9b) != 0) goto L_d10;
  ph = MB16(battle + 0x92) & 0xf00u;
  if ((ph == 0x100u && flag != 0) || (ph == 0x200u && flag == 0)) {
    MW8(battle + 1,9);
    MW8(battle + 0,0xb);
    MW8(battle + 2,4);
    return;                                   /* j 0x801531d0 */
  }
  goto L_ecc;

L_d10:                                        /* 0x80152d10 (cases 1/2,
                                               * case5 9b!=0) */
  status_flag_cond_set(guest_of(blk_a),0);           /* a2=0x801209ac unused */
  status_flag_cond_set(guest_of(blk_b),0);
  if (MBS16(battle + 0xb4) != 0)
    MW16(battle + 0xb4,(uint16_t)((MB16(battle + 0xb4) & 0xf000u) | 0x100u));
  if (MBS16(battle + 0xb6) != 0)
    MW16(battle + 0xb6,(uint16_t)((MB16(battle + 0xb6) & 0xf000u) | 0x100u));
  deferred_cmd_post(0x204,0,0);
  frame_present(battle);
  scroll_cfg_set(2,8);
  crec = MB32(battle + 0xc0);
  MW8(crec + 0x19,(uint8_t)(MB8(crec + 0x19) & 3));
  MW8(crec + 0x1a,(uint8_t)(MB8(crec + 0x1a) & 3));
  if ((MB16(battle + 0x80) & 0xfu) != 0) {
    confirm_bookkeeping(battle);
    battle_state_init(blk_a,blk_b,battle);
    scratchpad_pair_init();
    MW8(battle + 0x9e,0);                         /* j delay slot */
    MW8(battle + 1,5);
  } else {
    MW8(battle + 1,1);
  }
  return;

L_case3:                                      /* 0x80152df8 */
  ph = MB16(battle + 0x92) & 0xf00u;
  if (ph == 0x200u) {
    if (flag == 0) goto L_3058;
    goto L_ecc;
  }
  if (ph >= 0x201u) goto L_ecc;
  if (ph != 0x100u) goto L_ecc;
  if (flag == 0) goto L_ecc;
  goto L_2f38;

L_case4:                                      /* 0x80152e40 */
  if (MBS16(battle + 0xb4) != 0)
    MW16(battle + 0xb4,(uint16_t)((MB16(battle + 0xb4) & 0xf000u) | 0x100u));
  if (MBS16(battle + 0xb6) != 0)
    MW16(battle + 0xb6,(uint16_t)((MB16(battle + 0xb6) & 0xf000u) | 0x100u));
  deferred_cmd_post(0x204,0,0);
  frame_present(battle);
  scroll_cfg_set(2,8);
  MW8(battle + 1,0xb);                            /* j 0x801531cc, v0=0xb */
  return;

L_ecc:                                        /* 0x80152ecc shared tail */
  if (MB8(battle + 0x9b) != 0) return;
  ph = MB16(battle + 0x92) & 0xf00u;
  if (ph == 0x200u) goto L_3040;
  if (ph < 0x201u) {
    if (ph != 0x100u) return;
    goto L_2f20;
  }
  if (ph == 0x300u) goto L_316c;
  return;

L_2f20:                                       /* phase 0x100 arm */
  if (flag != 0) goto L_3014;
  if (MBS16(battle + 0xa4) < 0) goto L_2f38;      /* bgez -> L_2f74 w/ v0=3 */
  goto L_2f74;

L_2f74:                                       /* 0x80152f74: state==3 arm */
  if (MB8(battle + 0x9c) == 3) {
    ac = *(int16_t *)(blk_a + 0xac);
    b0 = *(int16_t *)(blk_a + 0xb0);
    b0u = *(uint16_t *)(blk_a + 0xb0);
    if (ac >= b0 || ac < 0)
      *(int16_t *)(blk_a + 0xac) = (int16_t)(b0u - 1);
    side_scale_ac(blk_a,battle);
  }
  MW16(battle + 0xb4,(uint16_t)((MB16(battle + 0xb4) & 0xf000u) | 0x100u));
  goto L_31ac;

L_3014:                                       /* 0x80153014: b4 0x600 arm */
  MW16(battle + 0x8c,0);
  MW16(battle + 0xb2,0);
  MW16(battle + 0xb4,(uint16_t)((MB16(battle + 0xb4) & 0xf000u) | 0x600u));
  MW16(battle + 0x92,(uint16_t)((MB16(battle + 0x92) & 0xff00u) | 0xfu));
  return;

L_3040:                                       /* phase 0x200 arm */
  if (flag == 0) goto L_3140;
  if (MBS16(battle + 0xa4) < 0) goto L_3058;
  goto L_3094;

L_3094:                                       /* 0x80153094: state==3 arm */
  if (MB8(battle + 0x9c) == 3) {
    ac = *(int16_t *)(blk_a + 0xac);
    b0 = *(int16_t *)(blk_a + 0xb0);
    b0u = *(uint16_t *)(blk_a + 0xb0);
    if (ac >= b0 || *(int16_t *)(blk_b + 0xac) < 0)
      *(int16_t *)(blk_a + 0xac) = (int16_t)(b0u - 1);
    side_scale_ac(blk_b,battle);
  }
  MW16(battle + 0xb6,(uint16_t)((MB16(battle + 0xb6) & 0xf000u) | 0x100u));
  goto L_31ac;

L_3140:                                       /* 0x80153140: b6 0x600 arm */
  MW16(battle + 0x8c,0);
  MW16(battle + 0xb2,0);
  MW16(battle + 0xb6,(uint16_t)((MB16(battle + 0xb6) & 0xf000u) | 0x600u));
  MW16(battle + 0x92,(uint16_t)((MB16(battle + 0x92) & 0xff00u) | 0xfu));
  return;

L_316c:                                       /* phase 0x300 arm */
  if (flag == 0)
    MW16(battle + 0xb4,(uint16_t)((MB16(battle + 0xb4) & 0xf000u) | 0x100u));
  else
    MW16(battle + 0xb6,(uint16_t)((MB16(battle + 0xb6) & 0xf000u) | 0x100u));
  goto L_31ac;

L_2f38:                                       /* 0x80152f38: b4 reset */
  MW8(battle + 0,9);
  MW8(battle + 1,9);
  MW16(battle + 0x8c,0);
  MW16(battle + 0x8e,0);
  MW16(battle + 0x90,0);
  MW16(battle + 0xb4,(uint16_t)((MB16(battle + 0xb4) & 0xf000u) | 0xa00u));
  MW16(battle + 0x92,(uint16_t)((MB16(battle + 0x92) & 0xff00u) | 0x10u));
  return;

L_3058:                                       /* 0x80153058: b6 reset */
  MW8(battle + 0,9);
  MW8(battle + 1,9);
  MW16(battle + 0x8c,0);
  MW16(battle + 0x8e,0);
  MW16(battle + 0x90,0);
  MW16(battle + 0xb6,(uint16_t)((MB16(battle + 0xb6) & 0xf000u) | 0xa00u));
  MW16(battle + 0x92,(uint16_t)((MB16(battle + 0x92) & 0xff00u) | 0x10u));
  return;

L_31ac:                                       /* 0x801531ac shared tail */
  deferred_cmd_post(0x204,0,0);
  frame_present(battle);
  scroll_cfg_set(2,8);
  MW8(battle + 1,2);                              /* v0=2 -> 0x801531cc */
  return;
}

/* ======== batch38 ======== */

/* offset_table_lookup — offset-table lookup @0x80163204 (18 insns, leaf).
 * a0 = guest table: [0] = s32 count, [4 + i*4] = u32 offsets
 * (0xffffffff = empty slot). Returns a0 + off + (count+1)*4 + 0xc
 * (a guest ptr into the blob area past the offset table), or 0 when
 * a1 >= count (signed slt) or the slot holds -1. The batch37 caller
 * uses the result as a cursor advance. */
uint32_t offset_table_lookup(uint32_t tbl,int32_t idx)
{
  int32_t n = (int32_t)MB32(tbl);
  uint32_t off, ret;
  if (idx >= n) { decomp_trace_record(14,tbl,(uint32_t)n,(uint8_t)idx,0); return 0; }
  off = MB32(tbl + (uint32_t)idx * 4u + 4u);
  if (off == 0xffffffffu) { decomp_trace_record(14,tbl,(uint32_t)n,(uint8_t)idx,0); return 0; }
  ret = tbl + off + (uint32_t)(n + 1) * 4u + 0xc;
  decomp_trace_record(14,tbl,(uint32_t)n,(uint8_t)idx,ret);
  return ret;
}

/* packed_u16_expand — packed u16-pair expander @0x80148edc (20 insns,
 * leaf). tbl = 0x801b2580 + a2*4 (two u16). val =
 * ((h0 & 0x3ff) >> 4) + ((h1 & 0x1ff) << 6); the SAME u16 is stored
 * to guest rec_a+0xe and rec_b+0xe (the paired engine records walked by
 * engine_record_pair_update). v0 = val at jr $ra; all in-file callers ignore it
 * so the value is dropped here. */
void packed_u16_expand(uint32_t rec_a,uint32_t rec_b,uint32_t idx)
{
  uint32_t tbl = 0x801b2580u + idx * 4u;
  uint32_t pkv = ((uint32_t)MB16(tbl) & 0x3ffu) >> 4;
  pkv += ((uint32_t)MB16(tbl + 2) & 0x1ffu) << 6;
  MW16(rec_a + 0xe, pkv);
  MW16(rec_b + 0xe, pkv);
}

/* cmd_queue_push — 8-entry u16 command-queue push @0x8012a5b8 (38
 * insns). Inlines leaf cmd_queue_slot_next @0x8012a4dc (9 insns):
 * nxt = MBS16(0x1f8000d0) + 1; slot = nxt < 9 (signed) ? nxt : 0.
 * slot 0 -> return. Else entry = 0x801c4530 + slot*8 receives the
 * four args as u16s, 0x1f8000d0 = slot, and the 5th (stack) arg
 * goes to u32 table 0x801bef38 + (slot-1)*4. v0 = slot at jr;
 * in-file callers treat it as void so it is dropped. */
void cmd_queue_push(int32_t f0,int32_t f1,int32_t f2,int32_t f3,
                  uint32_t aux)
{
  int32_t nxt = MBS16(0x1f8000d0) + 1;
  int32_t slot = (nxt < 9) ? nxt : 0;
  uint32_t e;
  if (slot == 0) return;
  e = 0x801c4530u + (uint32_t)slot * 8u;
  MW16(0x1f8000d0, (uint16_t)slot);
  MW16(e + 0, (uint16_t)f0);
  MW16(e + 2, (uint16_t)f1);
  MW16(e + 4, (uint16_t)f2);
  MW16(e + 6, (uint16_t)f3);
  MW32(0x801bef38u + (uint32_t)(slot - 1) * 4u, aux);
}

/* victory_record_restore — victory-record restorer @0x80128268 (397 insns,
 * jr $ra @0x80128890). f0 = guest battle base (kept in t3), f1 =
 * record index. rec = 0x801c47c0 + f1*0x235c — the same table
 * results_record_build builds. Copies nine fixed blobs OUT of the record
 * into the live tables (inverse direction of results_record_build), plus
 * two flag-selected 0x870 tables and two 756-stride subtables, then
 * unpacks the packed flag u32 at rec+0x2358 into the two side
 * records (pa = MB32(0x1f800100), pb = MB32(0x1f800208); bits 0-3
 * -> pa +0x23c/+0x1c4/+0x28c/+0x264, bits 4-7 -> pb same offsets),
 * mirrors rec+2 -> f0+0x94, writes -1/-2 sentinels around the
 * 0x801f4d70 blobs, and returns lh(rec+0). All lwl/lwr vs lw/sw
 * pairs are alignment variants of the same 16-byte block copy
 * (blk_copy); the K subtable genuinely copies only 0x2f0 (no tail
 * word) while L copies 0x2f0 + 1 trailing word = 0x2f4. */
int32_t victory_record_restore(uint32_t battle,int32_t idx)
{
  uint32_t rec = 0x801c47c0u + (uint32_t)idx * 0x235cu;
  uint32_t bit, pa, pb, f;

  blk_copy(0x1f8000e8, rec + 0x16cc, 0x210);
  blk_copy(0x801f5328, rec + 0x18dc, 0x230);
  blk_copy(0x801f5258, rec + 0x1b0c, 0x90);
  blk_copy(0x801f52e8, rec + 0x1b9c, 0x40);
  blk_copy(0x801f04a8, rec + 0x1bdc, 0x194);
  blk_copy(0x801ffb0,  rec + 0x1d70, 0x108);
  blk_copy(0x801f0db0, rec + 0x1e78, 0x200);
  blk_copy(0x801f0ae0, rec + 0x2078, 0x2d0);
  blk_copy(0x801f4d70, rec + 0x2348, 8);
  blk_copy(0x801f4e2c, rec + 0x2350, 8);

  bit = MB8(0x1f8000ed) & 1;
  blk_copy(0x801e1b3c + bit * 2160u, rec + 4,     0x870);
  bit = MB8(0x1f8001f5) & 1;
  blk_copy(0x801e2c1c + bit * 2160u, rec + 0x874, 0x870);
  bit = MB8(0x1f8000ed) & 1;
  blk_copy(0x801e0f6c + bit * 756u,  rec + 0x10e4, 0x2f0);
  bit = MB8(0x1f8001f5) & 1;
  blk_copy(0x801e1554 + bit * 756u,  rec + 0x13d8, 0x2f4);

  MW16(battle + 0x94, MB16(rec + 2));
  f  = MB32(rec + 0x2358);
  pa = MB32(0x1f800100);                     /* lw 0x18(0x1f8000e8) */
  MW16(pa + 0x23c, f & 1u);
  MW16(pa + 0x1c4, (f >> 1) & 1u);
  MW16(pa + 0x28c, (f >> 2) & 1u);
  MW16(pa + 0x264, (f >> 3) & 1u);
  pb = MB32(0x1f800208);                     /* lw 0x18(0x1f8001f0) */
  MW16(pb + 0x23c, (f >> 4) & 1u);
  MW16(pb + 0x1c4, (f >> 5) & 1u);
  MW16(pb + 0x28c, (f >> 6) & 1u);
  MW16(pb + 0x264, (f >> 7) & 1u);
  MW16(0x801f4d70 + 0xc4, 0xffffu);
  MW16(0x801f4d70 + 8,    0xffffu);
  MW16(0x801f4d70 + 0xc6, 0xfffeu);
  MW16(0x801f4d70 + 0xa,  0xfffeu);
  return MBS16(rec + 0);
}

/* shared tail of versus_stage_commit / versus_stage_commit_b: the wrapped-decrement
 * loop over s16 index+2. HW shape (0x80128cec-0x80128d14): a1 starts
 * at lh(s0+2); N = s2+1 iterations of {a1--; if (a1<0) a1=2}; a
 * final unchecked decrement in the bnez delay slot is cancelled by
 * the trailing a1++ — net effect is exactly N wrapped decrements. */
static int32_t dec_wrap(int32_t idx,int32_t steps)
{
  int32_t i;
  for (i = 0; i < steps; i++) {
    idx--;
    if (idx < 0) idx = 2;
  }
  return idx;
}

/* shared record-index walk: from rec = 0x801c47c0 + a1*0x235c walk
 * DOWN while lh(rec) < 0, a1-- per step; the pointer decrement is
 * -0x235c, the same record stride (generated 0x80128D18-D54 computes
 * idx*9052 = idx*0x235c both ways). Exits
 * with a1 possibly 0. */
static int32_t dec_walk(int32_t idx)
{
  uint32_t rec = 0x801c47c0u + (uint32_t)idx * 0x235cu;
  while (MBS16(rec) < 0) {
    idx--;
    rec -= 0x235cu;
    if (idx <= 0) break;
  }
  return idx;
}

/* shared epilogue: ret = victory_record_restore(battle, idx) - 1 stored to
 * s0+6; if the s16 result is negative store 0x257 instead. */
static void dec_finish(uint32_t battle,uint32_t rec,int32_t idx)
{
  int32_t ret = victory_record_restore(battle, idx) - 1;
  MW16(rec + 6, (uint16_t)ret);
  if ((int16_t)ret < 0) MW16(rec + 6, 0x257);
}

/* versus_stage_commit — versus-record stage commit @0x80128c18 (95 insns,
 * jr $ra @0x80128d94). a0 = guest battle base s1, a1 = stage arg s2.
 * s0 = MB32(s1+0xcc); s0[0] = 3; results_record_build(4, lh s0+6) (HW a0 =
 * battle base, folded into the ported 2-arg signature); then the
 * two engine-record blobs s0+0x18 -> 0x801e1b3c (0x21c0) and
 * s0+0x21d8 -> 0x801e0f6c (0xbd0) are copied back to the live
 * tables. s2 < 0 -> idx 3. Else idx = wrapped-decrement of
 * lh(s0+2) by s2+1, then the downward record walk while the
 * resulting record's lh is negative. */
void versus_stage_commit(uint32_t baddr,uint32_t stage)
{

  int32_t stg = (int32_t)stage;
  uint32_t rec = MB32(baddr + 0xcc);
  int32_t idx;
  {
    decomp_trace_record(47,baddr,stage,rec,MB32(baddr+0xcc)); }

  MW16(rec + 0, 3);
  results_record_build(4, MBS16(rec + 6));
  blk_copy(0x801e1b3c, rec + 0x18,   0x21c0);
  blk_copy(0x801e0f6c, rec + 0x21d8, 0xbd0);

  if (stg < 0) {
    idx = 3;
  } else {
    idx = MBS16(rec + 2);
    if (stg + 1 > 0) idx = dec_wrap(idx, stg + 1);
    if (idx > 0) idx = dec_walk(idx);
  }
  dec_finish(baddr, rec, idx);
}

/* versus_stage_commit_b — same stage commit minus the 27c60 rebuild @
 * 0x80128d98 (82 insns, jr $ra @0x80128edc). a0 = guest battle
 * base, a1 = stage arg t0. Identical record copies and index
 * resolution; t0 < 0 -> idx 3. */
void versus_stage_commit_b(uint32_t baddr,int32_t stage)
{
  int32_t stg = stage;
  uint32_t rec = MB32(baddr + 0xcc);
  int32_t idx;
  {
    decomp_trace_record(48,baddr,stage,rec,MB32(baddr+0xcc)); }

  MW16(rec + 0, 3);
  blk_copy(0x801e1b3c, rec + 0x18,   0x21c0);
  blk_copy(0x801e0f6c, rec + 0x21d8, 0xbd0);

  if (stg < 0) {
    idx = 3;
  } else {
    idx = MBS16(rec + 2);
    if (stg + 1 > 0) idx = dec_wrap(idx, stg + 1);
    if (idx > 0) idx = dec_walk(idx);
  }
  dec_finish(baddr, rec, idx);
}

/* ======== batch39 ======== */

extern void cd_reg_window_u16(int32_t,uint32_t);
extern void cd_reg_bit40_ctrl(int32_t);
extern void cd_dma_xfer_mode(uint32_t,int32_t);
extern void cd_state9_store(int32_t);
extern void cd_state_store(int32_t);
extern void cd_reg_rw_d2(uint32_t);
extern void cd_chan_attr_apply(uint32_t);
extern void motion_integrator_tail(int,int);
extern int32_t skeleton_init(uint32_t ctrl,uint32_t desc,uint32_t unused3);
extern int memcard_file_flow(int32_t);
extern int cd_cmd_issue_sync(int,int,int);
extern void print_dispatch_selfdesc(void *,const char *,...);
extern int options_page_dispatch(int32_t);
extern void slot_apply_guest(int,int);
extern uint32_t task_object_spawn(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t task_node_alloc(int32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t);

/* cd_reg_window_u16_wrap — cd_reg_window_u16(0,a0) trampoline @0x80157334 (8 insns). */
void cd_reg_window_u16_wrap(uint32_t val)
{
  cd_reg_window_u16(0, val);
}

/* options_pages_reset — reset pair @0x8014bfe4 (9 insns):
 * options_page_dispatch(0); options_page_dispatch(0xa). */
void options_pages_reset(void)
{
  options_page_dispatch(0);
  options_page_dispatch(0xa);
}

/* cursor_advance_thunk — motion_integrator_tail(a0,a1) then v0 = a2+2 @0x8017cfe0
 * (9 insns). a2 saved to s0 in the delay slot of the jal. */
int32_t cursor_advance_thunk(uint32_t arg0,uint32_t arg1,uint32_t cur)
{
  motion_integrator_tail((int)arg0, (int)arg1);
  return (int32_t)(cur + 2u);
}

/* audio_seq_reset — audio/seq reset pair @0x80158244 (11 insns):
 * cd_reg_bit40_ctrl(0); cd_dma_xfer_mode(0x801f36f0, 5). */
void audio_seq_reset(void)
{
  cd_reg_bit40_ctrl(0);
  cd_dma_xfer_mode(0x801f36f0u, 5);
}

/* chan_attr_block_write — channel-attr block @0x801f2690 @0x80156404 (13
 * insns): flags=3, +4=+6=(u16)a0, cd_chan_attr_apply(0x801f2690). */
void chan_attr_block_write(uint32_t val)
{
  MW32(0x801f2690u, 3);
  MW16(0x801f2696u, val);
  MW16(0x801f2694u, val);
  cd_chan_attr_apply(0x801f2690u);
}

/* chan_attr_rec_a — @0x801f2650 record variant A @0x801572c0 (13
 * insns): [0]=a0, [4]=0x10, [0x14]=a1, cd_chan_attr_push(0x801f2650). */
void chan_attr_rec_a(uint32_t ptr,uint32_t val)
{
  MW32(0x801f2650u, ptr);
  MW32(0x801f2654u, 0x10);
  MW16(0x801f2664u, val);
  cd_chan_attr_push(0x801f2650u);
}

/* dual_mode_announce — dual mode announce @0x801525d0 (14 insns):
 * mode_name_announce(a0,0,1); mode_name_announce(a0,1,1). */
void dual_mode_announce(uint32_t baddr)
{
  mode_name_announce(baddr, 0, 1);
  mode_name_announce(baddr, 1, 1);
}

/* chan_attr_rec_b — @0x801f2650 record variant B @0x801572f8 (14
 * insns): [0]=a0, [4]=3, [8]=[0xa]=a1, cd_chan_attr_push(0x801f2650). */
void chan_attr_rec_b(uint32_t ptr,uint32_t val)
{
  MW32(0x801f2650u, ptr);
  MW32(0x801f2654u, 3);
  MW16(0x801f2658u, val);
  MW16(0x801f265au, val);
  cd_chan_attr_push(0x801f2650u);
}

/* slot_byte_commit — slot-byte apply @0x8017c97c (15 insns): s0 = a2;
 * lh s0; sll 16; sra 24 -> a1 = sx8 of the HIGH byte of the halfword
 * (byte1); sh a1 -> MB8(a0+5); slot_apply_guest(a0, a1&1);
 * v0 = s0+2. a1 (in) is dead. */
int32_t slot_byte_commit(uint32_t obj,uint32_t unused,uint32_t cur)
{
  (void)unused;

  int32_t op = (int32_t)(int8_t)(MB16(cur) >> 8);
  MW8(obj + 5, (uint32_t)op);
  slot_apply_guest((int)obj, op & 1);
  return (int32_t)(cur + 2u);
}

/* elapsed_tick_clamp — elapsed-tick clamp @0x8012adf8 (16 insns):
 * d = cd_timer_read(1) - lhu(0x801e83ae); if (d >= 0xb signed)
 * sh d -> 0x1f80001c. */
void elapsed_tick_clamp(void)
{
  int32_t d = (int32_t)cd_timer_read(1) - MB16(0x801e83aeu);
  if (d >= 0xb) MW16(0x1f80001cu, (uint16_t)d);
}

/* chan_flag_toggle — gated 0x801f46f4 toggle @0x8015827c (16 insns;
 * real entry @0x80158274 with the lui/lw pair). If MB32(0x801f46f4)
 * == 0 store 0x200 there, else store 0; then cd_reg_rw_d2(that
 * value); cd_reg_bit40_ctrl(1). */
void chan_flag_toggle(void)
{
  uint32_t v = MB32(0x801f46f4u);
  if (v == 0) {
    MW32(0x801f46f4u, 0x200);
    v = 0x200;
  } else {
    MW32(0x801f46f4u, 0);
    v = 0;
  }
  cd_reg_rw_d2(v);
  cd_reg_bit40_ctrl(1);
}

/* mode_string_dispatch — mode-string dispatch @0x80129f1c (17 insns):
 * a1==0 -> print_dispatch_selfdesc(a0, 0x8011ec30); a1==1 -> same with
 * 0x8011ec38; any other a1 returns without calling. */
void mode_string_dispatch(uint32_t desc,int32_t sel)
{
  if (sel == 0) {
    print_dispatch_selfdesc((void *)guest_at(desc), (const char *)guest_at(0x8011ec30));
  } else if (sel == 1) {
    print_dispatch_selfdesc((void *)guest_at(desc), (const char *)guest_at(0x8011ec38));
  }
}

/* pending_work_drain — pending-work drain @0x8015f64c (20 insns):
 * a0 = guest ptr; if MB8(a0+0x22)==0 or MB32(0x801c15f0)==0 return
 * -1. Else clear 0x801c15f0 and return memcard_file_flow(1). */
int32_t pending_work_drain(uint32_t obj)
{
  if (MB8(obj + 0x22) == 0) return -1;
  if (MB32(0x801c15f0u) == 0) return -1;
  MW32(0x801c15f0u, 0);
  return memcard_file_flow(1);
}

/* descriptor_fetch — descriptor fetch @0x8017cf14 (21 insns): s0 = a2;
 * idx = ((s16)lh(s0+2)*2 + MB8(a0+5)) & 1; skeleton_init(a0,
 * MB32(a1+0x10) + idx*28, a1); v0 = s0+4. */
int32_t descriptor_fetch(uint32_t ctrl,uint32_t desc,uint32_t cur)
{

  uint32_t idx = ((uint32_t)MBS16(cur + 2) * 2u
                  + MB8(ctrl + 5)) & 1u;
  skeleton_init(ctrl, MB32(desc + 0x10) + idx * 28u, desc);
  return (int32_t)(cur + 4u);
}

/* byte2_probe_commit — signed-byte gate @0x8017cf6c (116B, 9 blocks):
 * s3 = sx8(MB8(a2+2)) [lhu(a2+2)<<16, sra 24]. If s3==1: when
 * MBS8(0x1f8000b1)!=0 ret 0x801f0000; else MW16(0x801f02f4,
 * MB16(a0+36)) and ret 0x801f0000. If s3!=1: slti(s3,2)==0 -> ret 0;
 * s3!=0 -> ret 0x801f0000; only s3==0 falls through: h=MBS16(a0+36),
 * if h<0 h+=15, MW16(0x801f02f6, h>>4); ret 0x801f0000. a1 dead. */
int32_t byte2_probe_commit(uint32_t rec,uint32_t unused,uint32_t desc)
{
  (void)unused;
  int32_t arg = (int32_t)MBS8(desc + 2);
  if (arg == 1) {
    if (MBS8(0x1f8000b1u) != 0) return 0x801f0000;
    MW16(0x801f02f4u, MB16(rec + 36));
    return 0x801f0000;
  }
  if (!(arg < 2)) return 0;
  if (arg != 0) return 0x801f0000;
  {
    int32_t h = MBS16(rec + 36);
    if (h < 0) h += 15;
    MW16(0x801f02f6u, (uint16_t)(h >> 4));
  }
  return 0x801f0000;
}

/* desc_cursor_advance — cursor advance @0x8017e46c (23 insns): s0 =
 * MB16(a2) >> 8; if (table_byte_next_a() < s0 signed) return a2+4, else
 * return MB16(a2+2) + MB32(a0+0x30). a1 dead. */
uint32_t desc_cursor_advance(uint32_t ctx,uint32_t unused,uint32_t cur)
{
  (void)unused;
  uint32_t lim = MB16(cur) >> 8;
  if (table_byte_next_a() < (int32_t)lim) return cur + 4u;
  return MB16(cur + 2) + MB32(ctx + 0x30);
}

/* chan_reset_clear — channel reset + 0x400-entry u16 clear @0x80158358
 * (28 insns): cd_reg_bit40_ctrl(0); cd_state9_store(0); cd_state_store(0);
 * four flag words cleared; then 0x200 iterations storing u16 0 at
 * 0x801f36f0+i*2 and 0x801f3af0+i*2 (v1 walks by 2). */
void chan_reset_clear(void)
{
  uint32_t i;
  cd_reg_bit40_ctrl(0);
  cd_state9_store(0);
  cd_state_store(0);
  MW32(0x801f36e8u, 0);
  MW32(0x801f46f8u, 0);
  MW32(0x801f46f0u, 0);
  MW32(0x801f36e4u, 0);
  for (i = 0; i < 0x200u; i++) {
    MW16(0x801f36f0u + i * 2u, 0);
    MW16(0x801f36f0u + 0x400u + i * 2u, 0);
  }
}

/* display_list_poke_redraw — display-list flag poke + redraw @0x8014ccd0 (29
 * insns): a0 = MB32(0x801f10bc); [a0+0x1c] = ([..]^4)|0x80;
 * [a0+0x58] and [a0+0x28] &= ~0x10; then task_alloc_wrap_c on the
 * 0x801f10c0/0x801f10c4 pair +0xb76 with strings 0x801b2e84/
 * 0x801b2ea8. */
void display_list_poke_redraw(void)
{
  uint32_t dctx = MB32(0x801f10bcu);
  MW32(dctx + 0x1c, (MB32(dctx + 0x1c) ^ 4u) | 0x80u);
  MW32(dctx + 0x58, MB32(dctx + 0x58) & ~0x10u);
  MW32(dctx + 0x28, MB32(dctx + 0x28) & ~0x10u);
  task_alloc_wrap_c(MB32(0x801f10c0u) + 0xb76u, MB32(0x801f10c4u) + 0xb76u,
               0x801b2e84u, 0x801b2ea8u);
}

/* one_shot_init — one-shot init flag @0x801574fc (29 insns): s0 =
 * 0x801bef60; if MB8(s0+0xf)!=0 return. brightness_packet_send(0); if
 * (MB8(s0+0xe)&0xf) in {1,2} poll cd_cmd_issue_sync(9,0,0) until nonzero;
 * MB8(s0+0xf) = 1. */
void one_shot_init(void)
{
  uint32_t base = 0x801bef60u;
  int32_t sub;
  if (MB8(base + 0xf) != 0) return;
  brightness_packet_send(0);
  sub = MB8(base + 0xe) & 0xf;
  if (sub > 0 && sub < 3) {
    while (cd_cmd_issue_sync(9, 0, 0) == 0) {
      if (br2_bail_pending()) return;
    }
  }
  MW8(base + 0xf, 1);
}

/* task_spawn_flag — task spawn + flag @0x8014c1d8 (31 insns):
 * v0 = task_alloc_wrap_e(4,0x801b27bc,0x801b27c0); if nonzero MW32(v0,0).
 * task_alloc_wrap_c(pair+0xb4e, 0x801b2f00, 0x801b2f18); then
 * [MB32(0x801f10bc)+0x1c] |= 0x40. */
void task_spawn_flag(void)
{
  int32_t task = task_alloc_wrap_e(4, 0x801b27bc, 0x801b27c0);
  uint32_t dctx;
  if (task != 0) MW32((uint32_t)task, 0);
  task_alloc_wrap_c(MB32(0x801f10c0u) + 0xb4eu, MB32(0x801f10c4u) + 0xb4eu,
               0x801b2f00u, 0x801b2f18u);
  dctx = MB32(0x801f10bcu);
  MW32(dctx + 0x1c, MB32(dctx + 0x1c) | 0x40u);
}

/* indexed_desc_dispatch — indexed descriptor dispatch @0x8014ca34 (31
 * insns): s0 = a0 (guest counter ptr); idx = MB32(s0); off =
 * idx*40 + 0x7f8; arg5 = MB32(0x801b2e2c + idx*4);
 * task_object_spawn(0, MB32(0x801f10c0)+off, MB32(0x801f10c4)+off,
 * 0x801b2d68, arg5); then (*s0)++. */
void indexed_desc_dispatch(uint32_t ctr)
{
  uint32_t idx = MB32(ctr);
  uint32_t off = idx * 40u + 0x7f8u;
  task_object_spawn(0, MB32(0x801f10c0u) + off, MB32(0x801f10c4u) + off,
               0x801b2d68u, MB32(0x801b2e2cu + idx * 4u));
  MW32(ctr, idx + 1u);
}

/* task_object_spawn — task/object spawn @0x8014b744 (50 insns):
 * blk = task_node_alloc(2,2,0,0x8014b37c,0x8014b73c,0x28); if blk==0
 * return 0. v0 = MB32(blk+0x10); stores a0..a3 at +0x14/+0/+4/+8,
 * five u16 zeros at +0x18..+0x20 (odd offsets), arg5 at +0xc,
 * a3-4 at +0x10, u16 0x400 @+0x22, u16 0x10 @+0x24; v0 = blk. */
uint32_t task_object_spawn(uint32_t w14,uint32_t w00,uint32_t w04,
                      uint32_t w08,uint32_t wc)
{
  uint32_t blk = task_node_alloc(2, 2, 0, 0x8014b37cu, 0x8014b73cu, 0x28);
  uint32_t obj;
  if (blk == 0) return 0;
  obj = MB32(blk + 0x10);
  MW32(obj + 0x14, w14);
  MW32(obj + 0x0,  w00);
  MW32(obj + 0x4,  w04);
  MW32(obj + 0x8,  w08);
  MW16(obj + 0x1e, 0);
  MW16(obj + 0x18, 0);
  MW16(obj + 0x1c, 0);
  MW16(obj + 0x1a, 0);
  MW16(obj + 0x20, 0);
  MW32(obj + 0xc,  wc);
  MW32(obj + 0x10, w08 - 4u);
  MW16(obj + 0x22, 0x400);
  MW16(obj + 0x24, 0x10);
  return blk;
}

/* ======== batch40 ======== */

extern void cd_mode_store(int32_t);
extern int cd_submit5(uint32_t a0,uint32_t a1,uint32_t a2);
extern int cd_submit6(uint32_t a0,uint32_t a1,uint32_t a2);
extern int32_t cd_job_result(uint32_t,uint32_t *,uint32_t *);
extern int fx_record_alloc(uint32_t,int32_t,uint32_t,uint32_t,uint32_t);
extern void quad_fx_spawn(int32_t,int32_t,uint32_t,uint16_t,uint16_t,
                         int32_t,int32_t);
extern void triple_track_spawn(int32_t,uint16_t,int32_t);
extern void sprite_node_emit_a(uint32_t,int32_t,int32_t,int32_t,int32_t,
                         uint32_t,uint32_t,uint32_t,uint32_t,int32_t);

/* shared tail of the four 0x1f80038c descriptor-init variants:
 * text_render_full((uint*)a0, a1 & 0xffff, 0x1f80038c). */
static void hud_msg_draw(uint32_t str,uint32_t attr)
{
  text_render_full((uint *)guest_at(str), attr & 0xffffu, 0x1f80038c);
}

/* descblk_variant_a — descriptor variant @0x8012e214 (34 insns):
 * param block @0x1f80038c: +0x1c=a2, +6=a3, +0xc=+0x10=0x10,
 * +0xa=+0xe=8, +0x12=0, +0x14=+0x18=1, +0/+2/+4=0,
 * +8=stk1, +0x16=((stk2+0x1e0)&0x1ff)*0x40+0x1f. */
void descblk_variant_a(uint32_t ctx,uint32_t ctx2,uint32_t str,
                  uint32_t f6,uint32_t f8,uint32_t attr)
{
  MW32(0x1f8003a8u, str);
  MW16(0x1f800392u, f6);
  MW16(0x1f800398u, 0x10);
  MW16(0x1f80039cu, 0x10);
  MW16(0x1f80038cu, 0);
  MW16(0x1f80038eu, 0);
  MW16(0x1f800390u, 0);
  MW16(0x1f800396u, 8);
  MW16(0x1f80039au, 8);
  MW16(0x1f80039eu, 0);
  MW16(0x1f8003a0u, 1);
  MW16(0x1f8003a4u, 1);
  MW16(0x1f800394u, f8);
  MW16(0x1f8003a2u,
       (uint16_t)(((attr + 0x1e0u) & 0x1ffu) * 0x40u + 0x1fu));
  hud_msg_draw(ctx, ctx2);
}

/* descblk_variant_d — descriptor variant @0x8012e85c (34 insns):
 * +0x1c=a2, +0xa=+0xc=+0xe=+0x10=8, +0x12=+0x14=1, +0/+2/+4=0,
 * +6=a3, +0x18=5, +8=stk1, +0x16=((stk2+0x1e0)&0x1ff)*0x40+0x16. */
void descblk_variant_d(uint32_t ctx,uint32_t ctx2,uint32_t str,
                  uint32_t f6,uint32_t f8,uint32_t attr)
{
  MW32(0x1f8003a8u, str);
  MW16(0x1f800396u, 8);
  MW16(0x1f800398u, 8);
  MW16(0x1f80039au, 8);
  MW16(0x1f80039cu, 8);
  MW16(0x1f80039eu, 1);
  MW16(0x1f8003a0u, 1);
  MW16(0x1f80038cu, 0);
  MW16(0x1f80038eu, 0);
  MW16(0x1f800390u, 0);
  MW16(0x1f800392u, f6);
  MW16(0x1f8003a4u, 5);
  MW16(0x1f800394u, f8);
  MW16(0x1f8003a2u,
       (uint16_t)(((attr + 0x1e0u) & 0x1ffu) * 0x40u + 0x16u));
  hud_msg_draw(ctx, ctx2);
}

/* job_wait_pump — job-wait pump @0x80168720 (34 insns): outer loop
 * { while (cd_submit5(a0,a1,a2)==0); while
 * (cd_job_result(0,&sp10,&sp14)==0); if (sp14==0 || s0==sp14) break;
 * s0 = sp14; }. sp10/sp14 are the guest stack slots the callee
 * writes through a1/a2 — host locals stand in. */
uint32_t job_wait_pump(uint32_t cd_a,uint32_t cd_b,uint32_t cd_c)
{
  uint32_t prev_st = 0;
  uint32_t out_aux = 0, status = 0;
  for (;;) {
    while (cd_submit5(cd_a, cd_b, cd_c) == 0) { }
    while (cd_job_result(0, &out_aux, &status) == 0) { }
    if (status == 0) break;
    if (prev_st == status) break;
    prev_st = status;
  }
  return status;
}

/* job_wait_pump_b — same pump on cd_submit6 @0x801687ac (34 insns). */
void job_wait_pump_b(uint32_t cd_a,uint32_t cd_b,uint32_t cd_c)
{
  uint32_t prev_st = 0;
  uint32_t out_aux = 0, status = 0;
  for (;;) {
    while (cd_submit6(cd_a, cd_b, cd_c) == 0) { }
    while (cd_job_result(0, &out_aux, &status) == 0) { }
    if (status == 0) break;
    if (prev_st == status) break;
    prev_st = status;
  }
}

/* cond_triple_spawn — conditional triple spawn @0x80140ca8 (35 insns;
 * real entry @0x80140ca0 loads v0 = MBS8(0x1f8000b1)). If that flag
 * is nonzero or a0 != 1, return. Else: fx_record_alloc(obj+0x124, 6,
 * 0x8000000e, 0x83, 0x801b1d48); quad_fx_spawn(s0, 0x12, obj+0x124,
 * 0x420, 0x3000, 2, 2); triple_track_spawn(MB8(s0+2), 0x30, 0). */
void cond_triple_spawn(uint32_t sel,uint32_t rec)
{
  uint32_t obj;
  if (MBS8(0x1f8000b1u) != 0) return;
  if (sel != 1) return;
  obj = MB32(rec + 0x20);
  fx_record_alloc(obj + 0x124, 6, 0x8000000eu, 0x83, 0x801b1d48u);
  quad_fx_spawn((int32_t)rec, 0x12, MB32(rec + 0x20) + 0x124, 0x420, 0x3000,
               2, 2);
  triple_track_spawn(MB8(rec + 2), 0x30, 0);
}

/* dual_sprite_attr_emit — dual sprite/attr emit @0x801672a0 (35 insns):
 * sprite_node_emit_a(p1, p2-p4, p3, p4, p5, 0, p6, 0, p6, 3) then
 * spr_attr_alloc_a(p1, p2, p3, lh(0x1f800020), p5, 0, 0). p5/p6 are the
 * two incoming stack args. */
void dual_sprite_attr_emit(uint32_t ot,uint32_t x1,uint32_t y,
                  uint32_t x0,uint32_t stk1,uint32_t stk2)
{
  sprite_node_emit_a(ot, (int32_t)(x1 - x0), (int32_t)y,
               (int32_t)x0, stk1, 0, stk2, 0, stk2, 3);
  spr_attr_alloc_a(ot, (int16_t)x1, (int16_t)y,
               MBS16(0x1f800020u), (int16_t)x0, stk1, 0);
}

/* descblk_variant_b — descriptor variant @0x8012e2a0 (36 insns, 7 args):
 * +0x1c=a2, +6=a3, +0xa=8, +0xe=-1, +0/+2/+4=0, +0xc=+0x10=0x10,
 * +0x14=+0x18=1, +8=stk1, +0x12=stk3, +0x16=((stk2+0x1e0)&0x1ff)
 * *0x40+0x1f. */
void descblk_variant_b(uint32_t ctx,uint32_t ctx2,uint32_t str,
                  uint32_t f6,uint32_t f8,uint32_t attr,
                  uint32_t fe)
{
  MW32(0x1f8003a8u, str);
  MW16(0x1f800392u, f6);
  MW16(0x1f800396u, 8);
  MW16(0x1f80039au, 0xffffu);
  MW16(0x1f80038cu, 0);
  MW16(0x1f80038eu, 0);
  MW16(0x1f800390u, 0);
  MW16(0x1f800398u, 0x10);
  MW16(0x1f80039cu, 0x10);
  MW16(0x1f8003a0u, 1);
  MW16(0x1f8003a4u, 1);
  MW16(0x1f800394u, f8);
  MW16(0x1f80039eu, fe);
  MW16(0x1f8003a2u,
       (uint16_t)(((attr + 0x1e0u) & 0x1ffu) * 0x40u + 0x1fu));
  hud_msg_draw(ctx, ctx2);
}

/* descblk_variant_c — descriptor variant @0x8012e45c (37 insns):
 * +0x1c=a2, +6=a3, +0xa=0x18, +0xe=-1, +0x12=-6, +0x14=2,
 * +0/+2/+4=0, +0xc=+0x10=0x20, +0x18=3, +8=stk1,
 * +0x16=((stk2+0x1e0)&0x1ff)*0x40+0x1e. */
void descblk_variant_c(uint32_t ctx,uint32_t ctx2,uint32_t str,
                  uint32_t f6,uint32_t f8,uint32_t attr)
{
  MW32(0x1f8003a8u, str);
  MW16(0x1f800392u, f6);
  MW16(0x1f800396u, 0x18);
  MW16(0x1f80039au, 0xffffu);
  MW16(0x1f80039eu, 0xfffau);
  MW16(0x1f8003a0u, 2);
  MW16(0x1f80038cu, 0);
  MW16(0x1f80038eu, 0);
  MW16(0x1f800390u, 0);
  MW16(0x1f800398u, 0x20);
  MW16(0x1f80039cu, 0x20);
  MW16(0x1f8003a4u, 3);
  MW16(0x1f800394u, f8);
  MW16(0x1f8003a2u,
       (uint16_t)(((attr + 0x1e0u) & 0x1ffu) * 0x40u + 0x1eu));
  hud_msg_draw(ctx, ctx2);
}

/* chan_reset_register — channel reset + callback register @0x801582c0
 * (37 insns): same 0x400-entry u16 clear of 0x801f36f0/0x801f3af0
 * and four flag clears as chan_reset_clear, then cd_mode_store(0),
 * cd_state_store(0x80158274) [= chan_flag_toggle entry],
 * cd_state9_store(0x80158244), MB32(0x801f46f4)=0x200,
 * cd_reg_rw_d2(0x200), cd_reg_bit40_ctrl(1). */
void chan_reset_register(void)
{
  uint32_t i;
  MW32(0x801f36e8u, 0);
  MW32(0x801f46f8u, 0);
  MW32(0x801f46f0u, 0);
  MW32(0x801f36e4u, 0);
  for (i = 0; i < 0x200u; i++) {
    MW16(0x801f36f0u + i * 2u, 0);
    MW16(0x801f36f0u + 0x400u + i * 2u, 0);
  }
  cd_mode_store(0);
  cd_state_store(0x80158274u);
  cd_state9_store(0x80158244u);
  MW32(0x801f46f4u, 0x200);
  cd_reg_rw_d2(0x200);
  cd_reg_bit40_ctrl(1);
}

/* ======== batch41 ======== */

extern int save_defaults_fill(void);
extern int save_block_init(void);
extern int save_tables_commit(void);
extern int cd_job_start_e(uint32_t a0);
extern void cd_file_open(int32_t,uint8_t *);
extern uint *gte_rtps_xform3(undefined4 *,ushort *,uint *);
extern uint32_t sprite_rect_set_a(int32_t,int16_t,int32_t,uint32_t,int32_t);
extern uint32_t gpu_texclut_pkt(uint32_t *,uint32_t *,uint32_t,uint32_t);

/* queue_slot_peek — queue-slot peek leaf @0x8012a4b4 (9 insns, no
 * prologue): v0 = lh(0x1f8000d2)+1; return v0 < 5 (signed) ? v0 : 0.
 * Args unused. */
int32_t queue_slot_peek(void)
{
  int32_t next = MBS16(0x1f8000d2u) + 1;
  return (next < 5) ? next : 0;
}

/* cmd_queue_slot_next — command-queue slot advance leaf @0x8012a4dc (10
 * insns, no prologue): v0 = lh(0x1f8000d0)+1; return v0 < 9 (signed) ? v0
 * : 0. Sole jal caller is cmd_queue_push @0x8012a5b8, which inlines this
 * same sequence; kept as a real function so any generated-path jal to the
 * leaf dispatches clean. Args unused. */
int32_t cmd_queue_slot_next(void)
{
  int32_t next = MBS16(0x1f8000d0u) + 1;
  return (next < 9) ? next : 0;
}

/* sprite_slot_commit — sprite-slot commit @0x8012a504 (44 insns):
 * s0 = queue_slot_peek(); if 0 return 0. slot = MB32(0x1f800004) +
 * s0*24 + 0x20d8; MW16(0x1f8000d2, s0); stack buf = four u16s
 * {a0,a1,a2,a3}; gpu_texclut_pkt(slot, buf, stk1, stk2); return s0. */
int32_t sprite_slot_commit(uint32_t w0,uint32_t w1,uint32_t w2,
                     uint32_t w3,uint32_t aux5,uint32_t aux6)
{
  uint32_t slot = (uint32_t)queue_slot_peek();
  uint32_t *gbuf;
  if (slot == 0) return 0;
  /* The four-u16 packet buf is handed to gpu_texclut_pkt, which resolves
   * a1 via guest_of()/guest_at(): a host stack buffer yields guest_of()==0
   * and the fallback reads the host pointer bits as a guest address
   * (garbage). Back it with a guest frame so guest_of() resolves it like
   * the generated guest-stack buffer. */
  gbuf = (uint32_t *)guest_at(br2_guest_frame(0x20) + 0x10);
  gbuf[0] = (w0 & 0xffffu) | ((w1 & 0xffffu) << 16);
  gbuf[1] = (w2 & 0xffffu) | ((w3 & 0xffffu) << 16);
  MW16(0x1f8000d2u, slot);
  gpu_texclut_pkt((uint32_t *)guest_at(MB32(0x1f800004u) + slot * 24u
                                      + 0x20d8u),
               gbuf, aux5, aux6);
  br2_guest_frame_free();
  return (int32_t)slot;
}

/* menu_quad_reset_redraw — quad reset + two redraws @0x8014cb64 (38 insns):
 * options_page_dispatch(4); (1); (0xb); (9); then task_alloc_wrap_c on the
 * 0x801f10c0/0x801f10c4 pair at +0x266 (str 0x801b3090/0x801b30dc)
 * and +0x28e (str 0x801b3090/0x801b312c). */
void menu_quad_reset_redraw(void)
{
  options_page_dispatch(4);
  options_page_dispatch(1);
  options_page_dispatch(0xb);
  options_page_dispatch(9);
  task_alloc_wrap_c(MB32(0x801f10c0u) + 0x266u, MB32(0x801f10c4u) + 0x266u,
               0x801b3090u, 0x801b30dcu);
  task_alloc_wrap_c(MB32(0x801f10c0u) + 0x28eu, MB32(0x801f10c4u) + 0x28eu,
               0x801b3090u, 0x801b312cu);
}

/* gte_quad_attr_emit — GTE quad-attr emit @0x8017a818 (38 insns): stack
 * param block = u16 triples {0x1000,0,0,0,0x1000,0,0,0,0x1000,0};
 * mode (a1, <5 unsigned) selects which of slots 0/4/8 get `val`
 * (a3): 0->all, 1->0+8, 2->0, 3->4, 4->8 (jump table 0x801226cc).
 * Then gte_rtps_xform3(obj+0x44 + idx*0x20, blk, obj+0x48 + idx*0x20)
 * where obj = a0, idx = a2. */
void gte_quad_attr_emit(uint32_t obj,uint32_t mode,uint32_t idx,
                  uint32_t val)
{
  uint16_t blk[10];
  uint32_t off;
  int i;
  for (i = 0; i < 10; i++) blk[i] = 0;
  blk[0] = 0x1000; blk[4] = 0x1000; blk[8] = 0x1000;
  if (mode < 5u) {
    switch (mode) {
    case 0: blk[0] = (uint16_t)val; blk[4] = (uint16_t)val;
            blk[8] = (uint16_t)val; break;
    case 1: blk[0] = (uint16_t)val; blk[8] = (uint16_t)val; break;
    case 2: blk[0] = (uint16_t)val; break;
    case 3: blk[4] = (uint16_t)val; break;
    case 4: blk[8] = (uint16_t)val; break;
    }
  }
  off = idx << 5;
  gte_rtps_xform3((undefined4 *)guest_at(MB32(obj + 0x44) + off),
               blk,
               (uint *)guest_at(MB32(obj + 0x48) + off));
}

/* fighter_frame_triple_emit — fighter-frame triple emit @0x80166f6c (39 insns;
 * real entry @0x80166f64 loads v0 = MBS8(0x801e3d00)). If that flag
 * is negative return -1. Else MB8(s0+0x9d)=0;
 * battle_camera_step(obj+0xbc, 0x1f8000e8, 0x1f8001f0, s0);
 * fighter_tint_fade(obj+8, obj+0xc, s0, MBS8(s0+0x98));
 * fighter_trail_emit(s0+0x28, 0x1f80033c, obj+0xbc, s0);
 * slot_height_proj(obj+8 + 0x207c, obj+0xbc, obj+0xc, s0); return 0. */
int32_t fighter_frame_triple_emit(uint32_t obj)
{
  uint32_t trail_ent = obj + 0x28;
  if (MBS8(0x801e3d00u) < 0) return -1;
  MW8(obj + 0x9d, 0);
  battle_camera_step(MB32(obj + 0xbc), (byte *)guest_at(0x1f8000e8),(byte *)guest_at(0x1f8001f0),(int32_t)obj);
  fighter_tint_fade(MB32(obj + 8), MB32(obj + 0xc), obj, MBS8(obj + 0x98));
  fighter_trail_emit((undefined4 *)guest_at(trail_ent), 0x1f80033cu, MB32(obj + 0xbc), (int32_t)obj);
  slot_height_proj(MB32(obj + 8) + 0x207cu, MB32(obj + 0xbc), MB32(obj + 0xc),
               obj);
  return 0;
}

/* job_pump_filter — job pump w/ result-class filter @0x80168380 (40
 * insns): like job_wait_pump but on cd_job_start_e and exits early
 * when sp14 is 0 or 3, or repeats-until-stable otherwise; returns
 * last sp14. */
int32_t job_pump_filter(uint32_t job)
{
  uint32_t prev = 0;
  uint32_t out_aux = 0, status = 0;
  for (;;) {
    while (cd_job_start_e(job) == 0) { }
    while (cd_job_result(0, &out_aux, &status) == 0) { }
    if (status < 3u) {
      if (status == 0) return (int32_t)status;
    } else {
      if (status == 3) return (int32_t)status;
    }
    if (prev == status) return (int32_t)status;
    prev = status;
  }
}

/* entry_attr_bump_loop — per-entry attr bump loop @0x8016b618 (40 insns):
 * if a2==0 return. s4=(a3<<6)&0x3ff, s3=(a3<<4)&0x100; walk s2
 * entries stride 0xc: MB16(s1)+=s4, MB16(s0-6)+=s3, then
 * gpu_load_image_cmd(s1, a1 + MB32(s0)*4); s0+=0xc,s1+=0xc. */
void entry_attr_bump_loop(uint32_t rec,uint32_t img,
                  uint32_t cnt,uint32_t amt)
{
  uint32_t walk = rec;
  uint32_t rec8 = rec + 8;
  int32_t n = (int32_t)cnt;
  uint32_t inc0 = (amt << 6) & 0x3ffu;
  uint32_t inc1 = (amt << 4) & 0x100u;
  if (n == 0) return;
  do {
    MW16(walk, MB16(walk) + inc0);
    MW16(rec8 - 6, MB16(rec8 - 6) + inc1);
    gpu_load_image_cmd(walk, img + MB32(rec8) * 4u);
    rec8 += 0xc;
    walk += 0xc;
    n--;
  } while (n != 0);
}

/* centered_sprite_emit — centered sprite emit @0x80172100 (40 insns, 6
 * args). If MB8(a1+0x15)==0 the a3/a6 values are halved (signed
 * (sx16+signbit) then LOGICAL >>1 — kept faithful). Then:
 * a1' = sx16(p3) - ((sx16(p5)+sb16(p5)) sra 1);
 * a2' = sx16(a2v) - ((sx16(v1v)+sb16(v1v)) sra 1);
 * a3' = sx16(p5); stack5 = sx16(v1v);
 * sprite_rect_set_a(p1, (short)a1', a2', a3', stack5). */
uint32_t centered_sprite_emit(uint32_t ctx,uint32_t frec,uint32_t x,
                      uint32_t w,uint32_t hgt,uint32_t vht)
{

  uint32_t hw = w, vsz, scr;
  int32_t sx_v1, sx_t2, half_t2, half_v1;
  if (MB8(frec + 0x15) == 0) {
    scr = (uint32_t)((int32_t)(int16_t)w
                   + (int32_t)((w >> 15) & 1u));
    hw = scr >> 1;
    scr = (uint32_t)((int32_t)(int16_t)vht
                   + (int32_t)((vht >> 15) & 1u));
    vsz = scr >> 1;
  } else {
    vsz = vht;
  }
  sx_v1 = (int32_t)(int16_t)vsz;
  sx_t2 = (int32_t)(int16_t)hgt;
  half_t2 = (sx_t2 + (int32_t)((hgt >> 15) & 1u)) >> 1;
  half_v1 = (sx_v1 + (int32_t)((vsz >> 15) & 1u)) >> 1;
  return sprite_rect_set_a((int32_t)ctx,
                      (int16_t)((int32_t)(int16_t)x - half_t2),
                      (int32_t)(int16_t)hw - half_v1,
                      (uint32_t)sx_t2, (int32_t)sx_v1);
}

/* descriptor_quad_dispatch — descriptor dispatch @0x8017d008 (41 insns): s0 =
 * a2; b = (s8)(MB16(s0)>>8); nib = b>>4 (arith). nib==1 -> if
 * MB8(a0+5)&1 return s0+6; nib==2 -> if !(MB8(a0+5)&1) return
 * s0+6; all other nibbles call through. Call:
 * gte_quad_attr_emit(a0, b&0xf, (s16)MB16(s0+2), (s16)MB16(s0+4)<<5);
 * return s0+6. */
uint32_t descriptor_quad_dispatch(uint32_t obj,uint32_t unused,uint32_t desc)
{
  (void)unused;
  int32_t tagb = (int32_t)(int8_t)(MB16(desc) >> 8);
  int32_t op = tagb >> 4;
  if (op == 1) {
    if ((MB8(obj + 5) & 1) != 0) return desc + 6u;
  } else if (op < 2) {
    /* call path */
  } else if (op == 2) {
    if ((MB8(obj + 5) & 1) == 0) return desc + 6u;
  }
  gte_quad_attr_emit(obj, (uint32_t)(tagb & 0xf),
               (uint32_t)MBS16(desc + 2),
               (uint32_t)(MBS16(desc + 4) << 5));
  return desc + 6u;
}

/* task_spawn_var_a — task spawn variant @0x8014b2cc (43 insns):
 * blk = task_node_alloc(1,2,0,0x8014afc0,0x8014b2c4,0x20); if 0 return
 * 0. v0=MB32(blk+0x10): +0=a0,+4=a1,+8=a2,+0xc=a3,+0x10=a2-4,
 * u16 zeros at +0x14,+0x16,+0x18,+0x1a,+0x1c; return blk. */
uint32_t task_spawn_var_a(uint32_t f0,uint32_t f4,
                      uint32_t f8,uint32_t fc)
{
  uint32_t blk = task_node_alloc(1, 2, 0, 0x8014afc0u, 0x8014b2c4u, 0x20);
  uint32_t inner;
  if (blk == 0) return 0;
  inner = MB32(blk + 0x10);
  MW32(inner + 0x0,  f0);
  MW32(inner + 0x4,  f4);
  MW32(inner + 0x8,  f8);
  MW32(inner + 0xc,  fc);
  MW32(inner + 0x10, f8 - 4u);
  MW16(inner + 0x1c, 0);
  MW16(inner + 0x14, 0);
  MW16(inner + 0x18, 0);
  MW16(inner + 0x16, 0);
  MW16(inner + 0x1a, 0);
  return blk;
}

/* desc_slot_fill_x15 — 15-slot descriptor fill @0x8014cab4 (43 insns):
 * [MB32(0x801f10bc)+0x1c] &= ~0x820; then for i in 0..14:
 * task_object_spawn(0, pair+0x7f8+i*0x28, 0x801b2e2c[i], 0x801b2d70);
 * options_page_dispatch(6). */
void desc_slot_fill_x15(void)
{
  uint32_t obj = MB32(0x801f10bcu);
  uint32_t strtab = 0x801b2e2cu;
  uint32_t off = 0x7f8u;
  int32_t i;
  MW32(obj + 0x1c, MB32(obj + 0x1c) & ~0x820u);
  for (i = 0; i < 0xf; i++) {
    uint32_t str = MB32(strtab);
    strtab += 4;
    task_object_spawn(0, MB32(0x801f10c0u) + off, MB32(0x801f10c4u) + off,
                 0x801b2d70u, str);
    off += 0x28u;
  }
  options_page_dispatch(6);
}

/* indexed_task_spawn — indexed task spawn @0x80140718 (44 insns; real
 * entry @0x80140710 loads v0 = MBS8(0x1f8000b1)). If flag nonzero,
 * (a1&0x7f) >= 3, or alloc fails return 0. Else inner =
 * MB32(blk+0x10); v1 = MB32(0x801b22ec + (a1&0x7f)*4);
 * inner[0]=inner[1]=v1; u16 0 @+0xc/+0xe; +8=a0; +0x10=(a1&0x80);
 * +0x12=a2; return inner. */
uint32_t indexed_task_spawn(uint32_t f8,uint32_t sel,
                      uint32_t f12)
{
  uint32_t idx = sel & 0x7fu;
  uint32_t hi = sel & 0x80u;
  uint32_t blk, inner, ent;
  if (MBS8(0x1f8000b1u) != 0) return 0;
  if (idx >= 3) return 0;
  blk = task_node_alloc(1, 2, 8, 0x80140510u, 0, 0x14);
  if (blk == 0) return 0;
  inner = MB32(blk + 0x10);
  ent = MB32(0x801b22ecu + idx * 4u);
  MW32(inner + 0x0, ent);
  MW32(inner + 0x4, ent);
  MW16(inner + 0xc, 0);
  MW16(inner + 0xe, 0);
  MW32(inner + 0x8, f8);
  MW16(inner + 0x10, hi);
  MW16(inner + 0x12, f12);
  return inner;
}

/* task_spawn_var_b — task spawn variant @0x8014af0c (44 insns):
 * blk = task_node_alloc(1,2,0,0x8014aee0,0x8014af04,0x20); stores
 * a0..a3 at +0/+4/+8/+0xc, a2-4 at +0x10, u16 zeros at
 * +0x14,+0x16,+0x18,+0x1a,+0x1c,+0x1e; return blk. */
uint32_t task_spawn_var_b(uint32_t f0,uint32_t f4,
                      uint32_t f8,uint32_t fc)
{
  uint32_t blk = task_node_alloc(1, 2, 0, 0x8014aee0u, 0x8014af04u, 0x20);
  uint32_t inner;
  if (blk == 0) return 0;
  inner = MB32(blk + 0x10);
  MW32(inner + 0x0,  f0);
  MW32(inner + 0x4,  f4);
  MW32(inner + 0x8,  f8);
  MW32(inner + 0xc,  fc);
  MW32(inner + 0x10, f8 - 4u);
  MW16(inner + 0x1c, 0);
  MW16(inner + 0x14, 0);
  MW16(inner + 0x18, 0);
  MW16(inner + 0x16, 0);
  MW16(inner + 0x1a, 0);
  MW16(inner + 0x1e, 0);
  return blk;
}

/* task_node_cb_noop_a/b/c — 8-byte `jr $ra; nop` task-node callbacks
 * @0x8014af04 / @0x8014b2c4 / @0x8014b73c. task_node_alloc() takes the
 * step/destroy callback as a guest address, and task_spawn_var_b /
 * task_spawn_var_a / task_object_spawn install exactly these three
 * addresses as the inert callback slots; the guest bodies are verified
 * to be a bare jr $ra with a nop delay slot, so the faithful port is an
 * empty body. */
void task_node_cb_noop_a(void) { }
void task_node_cb_noop_b(void) { }
void task_node_cb_noop_c(void) { }

extern void task_slot_step(void);
extern uint8_t *g__DAT_1f800082;   /* u16 sim/pause gate (see battle.c) */
static int32_t tpg_idiv(int32_t,int32_t);   /* defined in batch47 */

/* sprite_record_tick — tagged sprite-record stepper @0x8014aae0 (284
 * guest insns, 36 blocks). a0 = guest record R (created by
 * task_spawn_var_a / task_spawn_var_b: +0x00/+0x04 are the two sprite
 * heads, +0x08 is the tag-entry base, +0x0c the sx16 table base,
 * +0x10 the tag cursor). Gate phase: u16 @R+0x18 counts down while the
 * 0x1f800082 sim/pause gate is clear (u16 @R+0x14 ticks down in
 * parallel); the tag-list walk only runs once sx16 @R+0x14 <= 0.
 * The cursor walks 4-byte entries {u16 tag, u16 val} dispatched on
 * (tag & 0xff00): 0xFCxx = counted skip when val < sx16 @R+0x16 else
 * increment @R+0x16 and jump to base+sx16(val)*4; 0xFDxx = unconditional
 * jump; 0xFExx = @R+0x18 = u16 val, advance 4; 0xFFxx = past-end marker
 * (@R+0x1e=1, @R+0x14=0, cursor -= 4); hi==0 = terminator which fills
 * @R+0x1a = u16 val, @R+0x14 = u16 val and @R+0x1c = (sx16 val <= 0).
 * Tail always runs: eight sx16 coords are pulled from u32 @R+0x0c + sx16(cursor)*16 into
 * arr[0..7]; when sx16 @R+0x1c != 0 arr[0,1,4,5] +=
 * (arr[2,3,6,7]*sx16@R+0x14)/sx16@R+0x1a, else the same with
 * (sx16@R+0x1a - sx16@R+0x14) as numerator; then
 * sprite_place_centered fires twice (heads @R+0/+4 against 0x1f800000)
 * and task_slot_step() runs when sx16 @R+0x1e != 0. */
void sprite_record_tick(uint32_t rec)
{

  int32_t arr[8];
  uint32_t cur, tag, hi, lo, sent, src;
  int32_t i, idx, f14, f1a, d, prod;

  if (MBS16(rec + 0x18u) != 0) {
    MW16(rec + 0x18u, (uint16_t)(MB16(rec + 0x18u) - 1u));
    goto L_tail;
  }
  if (*(uint16_t *)g__DAT_1f800082 == 0)
    MW16(rec + 0x14u, (uint16_t)(MB16(rec + 0x14u) - 1u));
  if (MBS16(rec + 0x14u) > 0) goto L_tail;

  cur = MB32(rec + 0x10u);
  MW32(rec + 0x10u, cur + 4u);
  if ((MB16(cur + 4u) & 0xff00u) == 0) goto L_scan_done;

L_scan_loop:                                    /* 0x8014ab60 */
  sent   = MB32(rec + 0x10u);
  tag = MB16(sent);
  hi  = tag & 0xff00u;
  lo  = tag & 0x00ffu;
  if (hi == 0xfd00u) goto L_tag_fd;
  if (hi > 0xfd00u) {
    if (hi == 0xfe00u) goto L_tag_fe;
    if (hi == 0xff00u) goto L_tag_ff;
    goto L_next;
  }
  if (hi == 0xfc00u) goto L_tag_fc;
  goto L_next;

L_tag_fd:                                       /* 0x8014abb4 */
  MB32(rec + 0x10u) = MB32(rec + 0x08u) +
                     ((uint32_t)MBS16(sent + 2u) << 2);
  goto L_next;

L_tag_fe:                                       /* 0x8014abc0 */
  MW16(rec + 0x18u, MB16(sent + 2u));
  MB32(rec + 0x10u) = sent + 4u;
  goto L_next;

L_tag_fc:                                       /* 0x8014abd4 */
  if ((int32_t)lo < MBS16(rec + 0x16u)) {
    MB32(rec + 0x10u) = sent + 4u;                  /* 0x8014ac24 */
    goto L_next;
  }
  MW16(rec + 0x16u, (uint16_t)(MB16(rec + 0x16u) + 1u));
  MB32(rec + 0x10u) = MB32(rec + 0x08u) +
                     ((uint32_t)MBS16(sent + 2u) << 2);
  goto L_next;

L_tag_ff:                                       /* 0x8014ac10 */
  if ((int32_t)lo < MBS16(rec + 0x16u)) {
    MB32(rec + 0x10u) = sent + 4u;
    goto L_next;
  }
  MW16(rec + 0x1eu, 1u);
  MW16(rec + 0x14u, 0u);
  MB32(rec + 0x10u) = MB32(rec + 0x10u) - 4u;
  goto L_tail;

L_next:                                        /* 0x8014ac28 */
  sent = MB32(rec + 0x10u);
  if ((MB16(sent) & 0xff00u) != 0) goto L_scan_loop;
L_scan_done:                                    /* 0x8014ac44 */
  cur = MB32(rec + 0x10u);
  MW16(rec + 0x1au, MB16(cur + 2u));
  MW16(rec + 0x14u, MB16(cur + 2u));
  MW16(rec + 0x1cu, (uint16_t)(MBS16(cur + 2u) <= 0 ? 1u : 0u));

L_tail:                                         /* 0x8014ac9c */
  cur = MB32(rec + 0x10u);
  idx = MBS16(cur);
  src = MB32(rec + 0x0cu) + (uint32_t)idx * 16u;
  for (i = 0; i < 8; i++)
    arr[i] = MBS16(src + (uint32_t)i * 2u);

  f14 = MBS16(rec + 0x14u);
  f1a = MBS16(rec + 0x1au);
  if (MBS16(rec + 0x1cu) != 0) {
    prod = (int32_t)((int64_t)arr[2] * (int64_t)f14);
    arr[0] += tpg_idiv(prod, f1a);
    prod = (int32_t)((int64_t)arr[3] * (int64_t)f14);
    arr[1] += tpg_idiv(prod, f1a);
    prod = (int32_t)((int64_t)arr[6] * (int64_t)f14);
    arr[4] += tpg_idiv(prod, f1a);
    prod = (int32_t)((int64_t)arr[7] * (int64_t)f14);
    arr[5] += tpg_idiv(prod, f1a);
  } else {
    d = f1a - f14;
    prod = (int32_t)((int64_t)arr[2] * (int64_t)d);
    arr[0] += tpg_idiv(prod, f1a);
    prod = (int32_t)((int64_t)arr[3] * (int64_t)d);
    arr[1] += tpg_idiv(prod, f1a);
    prod = (int32_t)((int64_t)arr[6] * (int64_t)d);
    arr[4] += tpg_idiv(prod, f1a);
    prod = (int32_t)((int64_t)arr[7] * (int64_t)d);
    arr[5] += tpg_idiv(prod, f1a);
  }

  sprite_place_centered(MB32(rec + 0x00u), 0x1f800000u,
                        (int16_t)arr[0], (int16_t)arr[1],
                        (int16_t)arr[4], (int16_t)arr[5]);
  sprite_place_centered(MB32(rec + 0x04u), 0x1f800000u,
                        (int16_t)arr[0], (int16_t)arr[1],
                        (int16_t)arr[4], (int16_t)arr[5]);
  if (MBS16(rec + 0x1eu) != 0) task_slot_step();
}

/* bcd_time_frame_print — BCD time -> frame count print @0x80158da8 (44
 * insns): d = (MB32(0x801bef80)-MB32(0x801bef84)-0x12c)/2 (trunc);
 * cd_file_open(d, buf); bcd = (buf[2]>>4)*10 + (buf[2]&0xf);
 * frames = magic-div(bcd*100) by ~75; sprintf_core(a0, 0x80120de0,
 * buf[0], buf[1], frames). */
void bcd_time_frame_print(uint32_t out)
{

  uint8_t buf[8];
  int32_t fdelta;
  uint32_t sec_bcd, sec100;
  int64_t prod;
  int32_t quo;
  memset(buf, 0, sizeof(buf));
  fdelta = (int32_t)(MB32(0x801bef80u) - MB32(0x801bef84u)) - 0x12c;
  fdelta = (fdelta + (int32_t)((uint32_t)fdelta >> 31)) >> 1;
  cd_file_open(fdelta, buf);
  sec_bcd = ((uint32_t)buf[2] >> 4) * 10u + ((uint32_t)buf[2] & 0xfu);
  sec100 = sec_bcd * 100u;
  prod = (int64_t)(int32_t)sec100 * (int64_t)(int32_t)0x1b4e81b5;
  quo = (int32_t)(prod >> 32) >> 3;
  quo -= (int32_t)sec100 >> 31;
  sprintf_core((char *)guest_at(out), (char *)guest_at(0x80120de0),
               (int)buf[0], (int)buf[1], quo);
}

/* versus_state_init — versus-screen state init @0x80126720 (46 insns):
 * copy 0x64 bytes 0x801b1280 -> a0 (six 16B blocks + tail word);
 * save_defaults_fill(); save_block_init(); save_tables_commit();
 * dbg_marker_mode_set(MB8(a0+0x24)); gauge_scale_fixed(MB8(a0+0xe),
 * magic-div(MB8(a0+0xf)<<3)); brightness_commit(MB8(a0+0xc)^1). */
void versus_state_init(uint32_t state)
{

  uint32_t src = 0x801b1280u;
  uint32_t dst = state;
  int i;
  for (i = 0; i < 6; i++) {
    MW32(dst + 0x0,  MB32(src + 0x0));
    MW32(dst + 0x4,  MB32(src + 0x4));
    MW32(dst + 0x8,  MB32(src + 0x8));
    MW32(dst + 0xc,  MB32(src + 0xc));
    src += 0x10;
    dst += 0x10;
  }
  MW32(dst, MB32(src));
  save_defaults_fill();
  save_block_init();
  save_tables_commit();
  dbg_marker_mode_set(MB8(state + 0x24));
  {
    uint32_t x = MB8(state + 0xf) << 3;
    int32_t gscale = (int32_t)(((uint64_t)x * 0x66666667u) >> 32) >> 2;
    gscale -= (int32_t)x >> 31;
    gauge_scale_fixed((short)MB8(state + 0xe), (short)gscale);
  }
  brightness_commit((undefined2)(MB8(state + 0xc) ^ 1u));
}

/* ======== batch42 ======== */

extern int hit_chain_clear();
extern int pending_hit_a_clear();
extern int pending_hit_b_clear();
extern int32_t options_menu_alloc(void);
extern int atan2_lut(int32_t,int32_t);
extern int gte_sqr_vec3_b(uint32_t *,uint32_t *);
extern int fixed_recip_lut(uint32_t);
#define FUN_80195370 gte_sqr_vec3_b
#define FUN_801948c0 fixed_recip_lut
extern undefined2 *vec2_rot(uint32_t,uint32_t,undefined2 *);
extern uint *angle_gte_vec(uint32_t,uint32_t,uint *);
extern int32_t state_pair_init(void);

/* shared 4x redraw pair used by dual_redraw_a/8014cc00/8014c00c:
 * two loops of 4 task_alloc_wrap_c calls over the 0x801f10c0/0x801f10c4
 * descriptor pair, offsets start..start+3*0x28 step 0x28, +0xe. */
static void redraw_4recs(uint32_t start0,uint32_t str0a,
                            uint32_t str0b,uint32_t start1,
                            uint32_t str1a,uint32_t str1b)
{
  int32_t i;
  uint32_t off;
  for (i = 0; i < 4; i++) {
    off = start0 + (uint32_t)i * 0x28u;
    task_alloc_wrap_c(MB32(0x801f10c0u) + off + 0xeu,
                 MB32(0x801f10c4u) + off + 0xeu, str0a, str0b);
  }
  for (i = 0; i < 4; i++) {
    off = start1 + (uint32_t)i * 0x28u;
    task_alloc_wrap_c(MB32(0x801f10c0u) + off + 0xeu,
                 MB32(0x801f10c4u) + off + 0xeu, str1a, str1b);
  }
}

/* battle_reset_tail — battle-reset tail @0x8017e77c (48 insns): s2 = a2;
 * s1 = (s8)(MB16(s2)>>8); if !(s1&2) deferred_cmd_post(0x400,7,0); if
 * !(s1&1) cond_triple_spawn(1,s0); clear bit15 of [s0+0x6c], zero
 * +0xd6/+0xd8/+5, copy +0xac -> +0xae; slot_apply_guest(s0,0);
 * hit_chain_clear/42da0/42dd8(s0); return s2+2. */
uint32_t battle_reset_tail(uint32_t obj,uint32_t unused,uint32_t desc)
{
  (void)unused;
  int32_t tag_fl = (int32_t)(int8_t)(MB16(desc) >> 8);
  if ((tag_fl & 2) == 0) deferred_cmd_post(0x400, 7, 0);
  if ((tag_fl & 1) == 0) cond_triple_spawn(1, obj);
  MW32(obj + 0x6c, MB32(obj + 0x6c) & 0xffff7fffu);
  MW16(obj + 0xd6, 0);
  MW16(obj + 0xd8, 0);
  MW8(obj + 5, 0);
  MW16(obj + 0xae, MB16(obj + 0xac));
  slot_apply_guest((int)obj, 0);
  hit_chain_clear(obj);
  pending_hit_a_clear(obj);
  pending_hit_b_clear(obj);
  return desc + 2u;
}

/* dual_axis_coord_xform — dual-axis coord transform @0x8012eaa8 (50
 * insns): o = MB32(a3+0x20); sp = {MB32(o+0x174),
 * MB32(o+0x178)+0xa00, MB32(o+0x17c)}; s1 = atan2_lut(sp[2],
 * sp[0]); gte_sqr_vec3_b(&sp[0],&sp[0]) (ret discarded); r =
 * atan2_lut(-sp[1], fixed_recip_lut(sp[0]+sp[2]));
 * s0=(s16)r, s1=(s16)s1; vec2_rot(s0,s1,a2+6);
 * angle_gte_vec(s0,s1,a2+0x20).
 * Generated reads the object through $a3 (gpr[7]) and the output base
 * through $a2 (gpr[6]); $a0/$a1 are dead on entry. */
void dual_axis_coord_xform(uint32_t unused,uint32_t unused2,uint32_t dst,
                  uint32_t ent)
{
  (void)unused;
  (void)unused2;
  uint32_t obj = MB32(ent + 0x20);
  uint32_t pos[3];

  uint32_t cv, av, ang_a;
  int32_t ang;
  pos[0] = MB32(obj + 0x174);
  cv = MB32(obj + 0x178) + 0xa00u;
  pos[1] = cv;
  pos[2] = MB32(obj + 0x17c);
  ang_a = (uint32_t)atan2_lut((int32_t)pos[2], (int32_t)pos[0]);
  (void)FUN_80195370(&pos[0], &pos[0]);
  ang = FUN_801948c0(pos[0] + pos[2]);
  ang = atan2_lut(-(int32_t)cv, ang);
  av = (uint32_t)(int32_t)(int16_t)ang;
  ang_a = (uint32_t)(int32_t)(int16_t)ang_a;
  vec2_rot(av, ang_a, (undefined2 *)guest_at(dst + 6));
  angle_gte_vec(av, ang_a, (uint *)guest_at(dst + 0x20));
}

/* dual_redraw_a — dual redraw, no pre/post call @0x8014c258 (50
 * insns): loops {0x118,4x0x28, 0x801b2ff0/0x801b3030} then
 * {0x1b8, 0x801b2f7c/0x801b2fbc}. */
void dual_redraw_a(void)
{
  redraw_4recs(0x118u, 0x801b2ff0u, 0x801b3030u,
                  0x1b8u, 0x801b2f7cu, 0x801b2fbcu);
}

/* dual_redraw_b — options_menu_alloc() then dual redraw @0x8014cc00 (51
 * insns): loops {0x118, 0x801b3008/0x801b3030} then {0x1b8,
 * 0x801b2f94/0x801b2fbc}. */
void dual_redraw_b(void)
{
  options_menu_alloc();
  redraw_4recs(0x118u, 0x801b3008u, 0x801b3030u,
                  0x1b8u, 0x801b2f94u, 0x801b2fbcu);
}

/* dual_redraw_c — dual redraw then options_menu_alloc() @0x8014c00c (52
 * insns): loops {0x118, 0x801b2fc8/0x801b3030} then {0x1b8,
 * 0x801b2f54/0x801b2fbc}, then options_menu_alloc(). */
void dual_redraw_c(void)
{
  redraw_4recs(0x118u, 0x801b2fc8u, 0x801b3030u,
                  0x1b8u, 0x801b2f54u, 0x801b2fbcu);
  options_menu_alloc();
}

/* u16_wrap_counter_step — wrap-around counter step @0x80129a6c (54
 * insns): s0 = a0 (u16*), old = (s16)MB16(s0).
 * If MB16(0x1f80007e)==0x1000: deferred_cmd_post(0x400,0xb,0);
 * MB16(s0)++; if ((s16)param_2 < (s16)new) MB16(s0)=0.
 * If MB16(0x1f80007e)==0x4000: deferred_cmd_post(0x400,0xb,0);
 * MB16(s0)--; if ((s16)new < 0) MB16(s0)=param_2.
 * Return (MB16(s0)^old)!=0. */
int32_t u16_wrap_counter_step(uint32_t ctr,int32_t wrap)
{
  int32_t old = MBS16(ctr);
  uint32_t v;
  if (MB16(0x1f80007eu) == 0x1000u) {
    deferred_cmd_post(0x400, 0xb, 0);
    v = MB16(ctr) + 1u;
    MW16(ctr, v);
    if ((int32_t)(int16_t)wrap < (int32_t)(int16_t)v) MW16(ctr, 0);
  }
  if (MB16(0x1f80007eu) == 0x4000u) {
    deferred_cmd_post(0x400, 0xb, 0);
    v = MB16(ctr) - 1u;
    MW16(ctr, v);
    if ((int32_t)(int16_t)v < 0) MW16(ctr, (uint16_t)wrap);
  }
  return (MBS16(ctr) ^ old) != 0;
}

/* desc_cursor_select — descriptor cursor select @0x8017a8b4 (54 insns):
 * sel = (s8)(MB16(a2)>>8); if (u32)sel >= 9 return MB32(a1+0x8c).
 * Jump table 0x801226e4: 0->0; 1->a2+2; 2->MB32(a1+0x8c);
 * 3-> MB32(a1+0x94)==0 ? MB32(a1+0x8c) : a2+2;
 * 4-> MB32(a1+0x94)!=0 ? MB32(a1+0x8c) : a2+2;
 * 5-> (s32)MB32(a1+0x94)>=0 ? MB32(a1+0x8c) : a2+2;
 * 6-> (s32)MB32(a1+0x94)<0 ? MB32(a1+0x8c) : a2+2;
 * 7-> MB32(a1+0x84)=a2+2, return 0; 8-> state_pair_init(), a2+2. */
uint32_t desc_cursor_select(uint32_t unused,uint32_t st,uint32_t desc)
{
  (void)unused;
  int32_t sel = (int32_t)(int8_t)(MB16(desc) >> 8);
  int32_t f94;
  if ((uint32_t)sel >= 9u) return MB32(st + 0x8c);
  switch (sel) {
  case 0: return 0;
  case 1: return desc + 2u;
  case 2: return MB32(st + 0x8c);
  case 3:
    return (MB32(st + 0x94) == 0) ? MB32(st + 0x8c)
                                      : desc + 2u;
  case 4:
    return (MB32(st + 0x94) != 0) ? MB32(st + 0x8c)
                                      : desc + 2u;
  case 5:
    f94 = (int32_t)MB32(st + 0x94);
    return (f94 >= 0) ? MB32(st + 0x8c) : desc + 2u;
  case 6:
    f94 = (int32_t)MB32(st + 0x94);
    return (f94 < 0) ? MB32(st + 0x8c) : desc + 2u;
  case 7:
    MW32(st + 0x84, desc + 2u);
    return 0;
  case 8:
    state_pair_init();
    return desc + 2u;
  }
  return MB32(st + 0x8c);
}

/* ======== batch43 ======== */

/* u8_wrap_counter_step — byte wrap-around counter @0x80129d5c (56 insns):
 * byte twin of u16_wrap_counter_step. s3 = MBS8(s0). If flag==0x2000:
 * deferred_cmd_post(0x400,0xb,0); MB8(s0)++; if ((s16)param_2 <
 * (s8)new) MB8(s0)=0. If flag==0x8000: same call; MB8(s0)--; if
 * ((s8)new < 0) MB8(s0)=param_2. Return changed!=0. */
int32_t u8_wrap_counter_step(uint32_t ctr,int32_t wrap)
{
  int32_t old = MBS8(ctr);
  int32_t v;
  if (MB16(0x1f80007eu) == 0x2000u) {
    deferred_cmd_post(0x400, 0xb, 0);
    v = MB8(ctr) + 1;
    MW8(ctr, (uint32_t)v);
    v = (int32_t)(int8_t)v;
    if ((int32_t)(int16_t)wrap < v) MW8(ctr, 0);
  }
  if (MB16(0x1f80007eu) == 0x8000u) {
    deferred_cmd_post(0x400, 0xb, 0);
    v = MB8(ctr) - 1;
    MW8(ctr, (uint32_t)v);
    if ((int32_t)(int8_t)v < 0) MW8(ctr, (uint8_t)wrap);
  }
  return ((int32_t)MBS8(ctr) ^ old) != 0;
}

/* quad_redraw_flag — quad redraw + flag @0x8014c0e0 (61 insns):
 * options_page_dispatch(3); options_page_dispatch(5); then four task_alloc_wrap_c calls
 * on the 0x801f10c0/0x801f10c4 pair at +0xa5e/+0xa86/+0xafe/+0xb26
 * with str pairs 2f24/2f3c, 2f24/2f48, 2f24/2f3c, 2f24/2f48; then
 * [MB32(0x801f10bc)+0x1c] |= 0x18. */
void quad_redraw_flag(void)
{
  uint32_t b0 = MB32(0x801f10c0u);
  uint32_t b1 = MB32(0x801f10c4u);
  uint32_t ctxp;
  options_page_dispatch(3);
  options_page_dispatch(5);
  task_alloc_wrap_c(b0 + 0xa5e, b1 + 0xa5e, 0x801b2f24u, 0x801b2f3cu);
  task_alloc_wrap_c(b0 + 0xa86, b1 + 0xa86, 0x801b2f24u, 0x801b2f48u);
  task_alloc_wrap_c(b0 + 0xafe, b1 + 0xafe, 0x801b2f24u, 0x801b2f3cu);
  task_alloc_wrap_c(b0 + 0xb26, b1 + 0xb26, 0x801b2f24u, 0x801b2f48u);
  ctxp = MB32(0x801f10bcu);
  MW32(ctxp + 0x1c, MB32(ctxp + 0x1c) | 0x18u);
}

/* dual_entry_attr_bump — dual-entry attr bump loop @0x8016b6bc (64 insns,
 * 6 args): count a3==0 return. s6=(p5<<6)&0x3ff, s5=(p5<<4)&0x100,
 * s4=(p6<<4)&0x3ff, s3=((s32)p6>>6)&0x1ff; s0=a0+0x14, s1=a0.
 * Per 0x18-stride entry: MB16(s1)+=s6; MB16(s0-0x12)+=s5;
 * MB16(s0-6)+=s3; MB16(s0-8)+=s4; gpu_load_image_cmd(s1, a1+MB32(s0-0xc)*4);
 * gpu_load_image_cmd(s1+0xc, a2+MB32(s0)*4). */
void dual_entry_attr_bump(uint32_t rec,uint32_t im0,uint32_t im1,
                  uint32_t cnt,uint32_t amt0,uint32_t amt1)
{
  uint32_t walk = rec;
  uint32_t rec14 = rec + 0x14;
  uint32_t im0a = im0, im1a = im1;
  int32_t n = (int32_t)cnt;
  uint32_t inc0 = (amt0 << 6) & 0x3ffu;
  uint32_t inc1 = (amt0 << 4) & 0x100u;
  uint32_t inc2 = (amt1 << 4) & 0x3ffu;
  uint32_t inc3 = ((int32_t)amt1 >> 6) & 0x1ffu;
  if (n == 0) return;
  do {
    MW16(walk, MB16(walk) + inc0);
    MW16(rec14 - 0x12, MB16(rec14 - 0x12) + inc1);
    MW16(rec14 - 6,    MB16(rec14 - 6)    + inc3);
    MW16(rec14 - 8,    MB16(rec14 - 8)    + inc2);
    gpu_load_image_cmd(walk, im0a + MB32(rec14 - 0xc) * 4u);
    gpu_load_image_cmd(walk + 0xc, im1a + MB32(rec14) * 4u);
    rec14 += 0x18;
    walk += 0x18;
    n--;
  } while (n != 0);
}

/* ======== batch44 ======== */

extern void text_render_simple(uint32_t *,uint32_t,int32_t);
extern int32_t text_win_printf(int32_t,uint32_t,uint32_t,uint32_t,...);
extern void cd_reg_window_update(uint8_t *);

/* gpu_prim8_emit — 8-tile GPU prim emitter @0x801479c4 (123 insns,
 * 5 args). Negative h/w fold into x/y. Ring idx MB8(0x801f10c8)
 * incremented (&0xff, wraps to 0 at >=8). dst = 0x801f10c8 +
 * idx*640 + 4 + MB32(rec+0xc)*320. Eight sprite_rect_set_c quads at
 * +0x00/+0x28/+0x50/+0x78/+0xa0/+0xc8/+0xf0/+0x118. */
void gpu_prim8_emit(uint32_t rec,int32_t x0,int32_t y0,
                  int32_t w0,int32_t h0)
{
  int32_t x = x0, y = y0, w = w0, h = h0;
  int32_t x1, y1, x2, wi, yi, hi;
  uint32_t dst, slot;
  if (w0 < 0) { x = x0 + w0; w = -w0; }
  if (h0 < 0) { y = y0 + h0; h = -h0; }
  slot = MB8(0x801f10c8u);
  slot = (slot + 1) & 0xffu;
  MW8(0x801f10c8u, slot);
  if (slot >= 8) MW8(0x801f10c8u, 0);
  x = (int16_t)x;
  y = (int16_t)y;
  dst = 0x801f10c8u + slot * 640u + 4u + MB32(rec + 0xc) * 320u;
  sprite_rect_set_c(dst, x, y, 8, 8);
  w = (int16_t)w;
  x1 = x + w - 8;
  sprite_rect_set_c(dst + 0x28, x1, y, 8, 8);
  h = (int16_t)h;
  y1 = y + h - 8;
  sprite_rect_set_c(dst + 0x50, x, y1, 8, 8);
  sprite_rect_set_c(dst + 0x78, x1, y1, 8, 8);
  x2 = x + 1;
  wi = w - 2;
  sprite_rect_set_c(dst + 0xa0, x2, y, wi, 8);
  sprite_rect_set_c(dst + 0xc8, x2, y1, wi, 8);
  yi = y + 1;
  hi = h - 2;
  sprite_rect_set_c(dst + 0xf0, x1, yi, 8, hi);
  sprite_rect_set_c(dst + 0x118, x, yi, 8, hi);
}

/* sprite_blit_center_a — centred sprite blit A @0x80147cdc (65 insns, 6
 * args). Calls gpu_prim8_emit(rec, a2-trunc2(sx16(p5)),
 * a3-trunc2(sx16(p6)), sx16(p5), sx16(p6)) where trunc2(x) =
 * (x + (x<0))>>1. Then s0 = ((p2+0x1e0)&0x1ff)<<6 + 0x1e and 8
 * prim slots at 0x801f10c8+idx*640+4+MB32(rec+0xc)*320 (+0x28
 * stride): sh s0 @+0xe, link-or 0x90000000 store, backlink =
 * slot&0xffffff -> MB32(MB32(rec+8)+0x2040). */
void sprite_blit_center_a(uint32_t rec,int32_t attr,int32_t cx,int32_t cy,
                  int32_t w,int32_t h)
{
  int32_t sh = (int16_t)h;
  int32_t sw = (int16_t)w;
  int32_t x, y, i;
  uint32_t tag, dst, link;
  x = cx - ((sw + ((sw < 0) ? 1 : 0)) >> 1);
  y = cy - ((sh + ((sh < 0) ? 1 : 0)) >> 1);
  gpu_prim8_emit(rec, (int16_t)x, (int16_t)y, sw, sh);
  tag = (((attr + 0x1e0) & 0x1ff) << 6) + 0x1e;
  dst = 0x801f10c8u + MB8(0x801f10c8u) * 640u + 4u
       + MB32(rec + 0xc) * 320u;
  for (i = 0; i < 8; i++) {
    MW16(dst + 0xe, (uint16_t)tag);
    link = MB32(MB32(rec + 8) + 0x2040);
    MW32(dst, link | 0x90000000u);
    MW32(MB32(rec + 8) + 0x2040, dst & 0xffffffu);
    dst += 0x28;
  }
}

/* sprite_blit_center_b — centred sprite blit B @0x80147bb4 (73 insns, 7
 * args). Same as 80147cdc but p7 supplies the x-centre and p5 the
 * y-centre swapped roles: gpu_prim8_emit(rec, sx16(a3)-trunc2(sx16(p6)),
 * sx16(p5)-trunc2(sx16(p7)), sx16(p6), sx16(p7)); backlink target
 * MB32(sx16(p2)*4 + MB32(rec+8)). */
void sprite_blit_center_b(uint32_t rec,int32_t slot,int32_t attr,int32_t cx,
                  int32_t cy,int32_t w,int32_t h)
{
  int32_t sh = (int16_t)h;
  int32_t sw = (int16_t)w;
  int32_t off = ((int16_t)slot) * 4;
  int32_t x, y, i;
  uint32_t tag, dst;
  x = cx - ((sw + ((sw < 0) ? 1 : 0)) >> 1);
  y = cy - ((sh + ((sh < 0) ? 1 : 0)) >> 1);
  gpu_prim8_emit(rec, (int16_t)x, (int16_t)y, sw, sh);
  tag = (((attr + 0x1e0) & 0x1ff) << 6) + 0x1e;
  dst = 0x801f10c8u + MB8(0x801f10c8u) * 640u + 4u
       + MB32(rec + 0xc) * 320u;
  for (i = 0; i < 8; i++) {
    uint32_t link;
    MW16(dst + 0xe, (uint16_t)tag);
    link = off + MB32(rec + 8);
    MW32(dst, MB32(link) | 0x90000000u);
    MW32(link, dst & 0xffffffu);
    dst += 0x28;
  }
}

/* voice_desc_fill — sound/voice descriptor fill @0x80157188 (77
 * insns, 4 args). t0 = 0x801f2be0+a1*8; v1 = MB32(0x801f35e0+
 * (sra(a1,6))*4); t1 = v1 + (a1&0x3f)*6. a2<0: hdr 0x600e3, +0x16=0,
 * +0x18=MB16(t0+2); else hdr 0x60093, +0x14=a2. +0=a0. a3==0:
 * volume from table 0x801b33bc[(MB8(t1+1)>>4)*2] scaled by
 * MB16(t1+4)>>7; else +8=+0xa=a3. +0x1c = MB32(0x801f26e0+
 * MB16(t0)*4); +0x3a=MB16(t0+4); +0x3c=MB16(t0+6);
 * cd_reg_window_update(0x801f2650). */
void voice_desc_fill(uint32_t id,uint32_t sel,int32_t kind,
                  int32_t vol)
{
  uint32_t ent = 0x801f2be0u + sel * 8u;
  uint32_t bank = MB32(0x801f35e0u + ((int32_t)sel >> 6) * 4u);
  uint32_t sub = bank + (sel & 0x3fu) * 6u;
  uint32_t win = 0x801f2650u;
  uint32_t ev, av;
  if (kind < 0) {
    MW32(win + 4, 0x600e3u);
    MW16(win + 0x16, 0);
    MW16(win + 0x18, MB16(ent + 2));
  } else {
    MW32(win + 4, 0x60093u);
    MW16(win + 0x14, (uint16_t)kind);
  }
  MW32(win, id);
  if (vol == 0) {
    ev = MB16(0x801b33bcu + (MB8(sub + 1) >> 4) * 2u);
    av = MB16(sub + 4);
    MW16(win + 8,  (uint16_t)((int32_t)(uint32_t)(av * (ev >> 8)) >> 7));
    MW16(win + 0xa, (uint16_t)((int32_t)(uint32_t)(av * (ev & 0xffu)) >> 7));
  } else {
    MW16(win + 0xa, (uint16_t)vol);
    MW16(win + 8,   (uint16_t)vol);
  }
  MW32(win + 0x1c, MB32(0x801f26e0u + MB16(ent) * 4u));
  MW16(win + 0x3a, MB16(ent + 4));
  MW16(win + 0x3c, MB16(ent + 6));
  cd_reg_window_update((uint8_t *)guest_at(win));
}

/* print_mode_dispatch — print-mode dispatch @0x8012ce30 (76 insns, 9
 * args). v1 = MB32(0x1f800384): ==1 -> text_win_printf(p1..p9);
 * ==2 -> sprintf(buf, p2..p9) into the guest stack buf ($sp+0x28,
 * frame -0x130 = entry_sp-0x108), init the
 * 0x1f80038c GPU block and text_render_simple(MB32(0x1f800008),
 * MB16(0x1f80000c), 0x1f80038c); else return. */
void print_mode_dispatch(uint32_t win,uint32_t fmt,uint32_t arg3,
                  uint32_t arg4,uint32_t arg5,uint32_t arg6,
                  uint32_t arg7,uint32_t arg8,uint32_t arg9)
{
  uint32_t mode = MB32(0x1f800384u);
  if (mode == 1) {
    text_win_printf((int32_t)win, fmt, arg3, arg4,
                 arg5, arg6, arg7, arg8, arg9);
  } else if (mode == 2) {
    /* sprintf buf = guest $sp+0x28 (frame -0x130) = entry_sp-0x108;
     * text_render_simple re-reads it through the 0x1f80038c block
     * pointer, so reserve the guest frame window (sp+0x28 -> frm+0x28)
     * for the write->draw span. */
    uint32_t pmd_frm = br2_guest_frame(0x130);
    uint8_t *buf = (uint8_t *)guest_at(pmd_frm + 0x28);
    uint32_t blk = 0x1f80038cu;
    sprintf_core((char *)buf, (char *)guest_at(fmt),
                 arg3, arg4, arg5, arg6, arg7,
                 arg8, arg9);
    MW16(blk + 6,  0x1000u);
    MW16(blk + 8,  0x1000u);
    MW16(blk + 0xe, 0xffffu);
    MW16(blk,     0);
    MW16(blk + 2, 0);
    MW16(blk + 4, 0);
    MW16(blk + 0xa, 8);
    MW16(blk + 0xc, 8);
    MW16(blk + 0x10, 8);
    MW16(blk + 0x12, 0);
    MW16(blk + 0x14, 0);
    MW16(blk + 0x16, 0x78dfu);
    MW32(blk + 0x1c, pmd_frm + 0x28);
    MW16(blk + 0x18, 0);
    text_render_simple((uint32_t *)guest_at(MB32(0x1f800008u)),
                 MB16(0x1f80000cu), (int32_t)blk);
    br2_guest_frame_free();
  }
}

/* scroll_attr_100_update — 100-slot scroll/attr updater @0x8014b818 (71
 * insns). s1 = param_1 + MB32(0x1f80000c)*4000. For i in 0..99
 * (s0=param_1+4i, s2=param_1+2i, slot=s1+0x28i): if
 * MB32(s0+0x2260)!=0 accumulate MB16(s0+0x1f40)+=MB16(s0+0x20d0),
 * MB16(s0+0x1f42)+=MB16(s0+0x20d2), then
 * sprite_rect_set_c(slot, lh(s0+0x1f40), sx16(new +0x1f42),
 * lh(s2+0x23f0), lh(s2+0x23f0)) and link slot into
 * MB32(MB32(0x1f800008)+0x40)|0x90000000 with backlink slot&0xffffff.
 * Tail: MB16(param_1+0x24b8)-- ; if sx16<0 task_slot_step(). */
void scroll_attr_100_update(uint32_t base)
{
  uint32_t slot = base + MB32(0x1f80000cu) * 4000u;
  uint32_t cur4 = base, cur2 = base;
  int32_t i;
  uint32_t ot_tail;
  for (i = 0; i < 100; i++) {
    if (MB32(cur4 + 0x2260) != 0) {
      uint32_t sy;
      MW16(cur4 + 0x1f40, MB16(cur4 + 0x1f40) + MB16(cur4 + 0x20d0));
      sy = MB16(cur4 + 0x1f42) + MB16(cur4 + 0x20d2);
      MW16(cur4 + 0x1f42, sy);
      sy = (uint32_t)(int32_t)(int16_t)sy;
      {
        int32_t sz = MBS16(cur2 + 0x23f0);
        sprite_rect_set_c(slot, MBS16(cur4 + 0x1f40), (int32_t)sy,
                     sz, sz);
      }
      ot_tail = MB32(MB32(0x1f800008u) + 0x40);
      MW32(slot, ot_tail | 0x90000000u);
      MW32(MB32(0x1f800008u) + 0x40, slot & 0xffffffu);
    }
    slot += 0x28;
    cur2 += 2;
    cur4 += 4;
  }
  ot_tail = MB16(base + 0x24b8) - 1u;
  MW16(base + 0x24b8, ot_tail);
  if ((int32_t)(int16_t)ot_tail < 0) task_slot_step();
}

/* ======== batch45 ======== */

extern void *spr_pool_a_alloc(void);
extern void prim_packet_init_tex(int,int,int,uint32_t,void *);
extern void spr_draw_pair(uint32_t,uint32_t,uint32_t,uint32_t,
                         uint32_t,uint32_t,uint32_t);
/* spr_attr_alloc_c — sprite-attr slot alloc @0x80158698 (73 insns, 7
 * args). Twin of spr_attr_alloc_a: spr_pool_a_alloc() slot, halfword box
 * (+8=p2,+0xa=p3,+0xc=p2+p4,+0xe=p3,+0x10=p2,+0x14=p2+p4,
 * +0x12=+0x16=p3+p5), tag p6|0x2a000000 with (p7-1)*0x20 when
 * p7!=0 else p6|0x28000000, prim_packet_init_tex(slot+0x18, 0x801e40b7,
 * 0x801e40b6, tagarg, 0); head cell: b[0]=(*head)|0x05000000,
 * *head=g&0xffffff, b[6]=(g&0xffffff)|0x02000000,
 * *head=(g+0x18)&0xffffff. */
uint32_t spr_attr_alloc_c(uint32_t head_addr,uint32_t x0,uint32_t y0,
                      uint32_t w,uint32_t h,uint32_t tag,
                      int32_t cnt)
{
  uint8_t *rec;
  uint32_t *head;
  uint32_t g;
  int32_t tex;
  rec = (uint8_t *)spr_pool_a_alloc();
  if (rec == (uint8_t *)0) return 0xffffffffu;
  *(int16_t *)(rec + 8)  = (int16_t)x0;
  *(int16_t *)(rec + 10) = (int16_t)y0;
  *(int16_t *)(rec + 12) = (int16_t)(x0 + w);
  *(int16_t *)(rec + 14) = (int16_t)y0;
  *(int16_t *)(rec + 16) = (int16_t)x0;
  *(int16_t *)(rec + 20) = (int16_t)(x0 + w);
  *(int16_t *)(rec + 18) = (int16_t)(y0 + h);
  *(int16_t *)(rec + 22) = (int16_t)(y0 + h);
  if (cnt != 0) {
    *(uint32_t *)(rec + 4) = tag | 0x2a000000u;
    tex = (cnt - 1) * 0x20;
  } else {
    *(uint32_t *)(rec + 4) = tag | 0x28000000u;
    tex = 0;
  }
  prim_packet_init_tex((int)(uintptr_t)guest_of(rec + 0x18),
               MB8(0x801e40b7u), MB8(0x801e40b6u), (uint32_t)tex,
               (void *)0);
  head = (uint32_t *)guest_at(head_addr);
  g = guest_of(rec);
  *(uint32_t *)rec = *head | 0x05000000u;
  *head = g & 0xffffffu;
  *(uint32_t *)(rec + 0x18) = (g & 0xffffffu) | 0x02000000u;
  *head = (g + 0x18) & 0xffffffu;
  return 0;
}

/* mode_select_commit — mode-select commit @0x8015e8c4 (77 insns; real
 * entry @0x8015e8a4 loads v1 = MB32(0x801b4154)). If v1==a0 ->
 * MB16(0x1f80008c)=-1, ret 1. If (v1>>2)!=(a0>>2): s0 =
 * sx16(MB16(0x1f80008c)); s0==0 -> cd_file_load_sm(-1,0), s0=0x10,
 * MB32(0x801c43a0)=0, MB8(0x801e3d00)=0xff; s0==0x10 ok; else
 * store s0, ret 0. Then cd_file_load_sm((a0>>2)+0x118,
 * MB32(0x801f4760)); if 0 -> store s0, ret 0. Common: s1=
 * MB8(0x1f800013); scratch_013_set(0); stream_block_decode(MB32(0x801f4764),
 * MB32(0x801f4760), a0&3); spr_draw_pair(MB32(0x801f4764),0,0x1ff,1,
 * 0x200,0,1); cd_timer_read(0); scratch_013_set(s1);
 * MB32(0x801b4154)=a0; MB16(0x1f80008c)=-1; ret 1. */
int32_t mode_select_commit(uint32_t sel)
{
  uint32_t prev = MB32(0x801b4154u);
  int32_t state, f013;
  int32_t rc;
  if (prev == sel) goto commit;
  if (((int32_t)prev >> 2) != ((int32_t)sel >> 2)) {
    state = MBS16(0x1f80008cu);
    if (state == 0) {
      cd_file_load_sm(-1, 0);
      state = 0x10;
      MW32(0x801c43a0u, 0);
      MB8(0x801e3d00u); /* keep read for faithfulness */
      *(uint8_t *)guest_at(0x801e3d00u) = 0xffu;
    } else if (state != 0x10) {
      MW16(0x1f80008cu, (uint16_t)state);
      return 0;
    }
    if (cd_file_load_sm((int32_t)(sel >> 2) + 0x118,
                     MB32(0x801f4760u)) == 0) {
      MW16(0x1f80008cu, (uint16_t)state);
      return 0;
    }
  }
  f013 = MB8(0x1f800013u);
  scratch_013_set(0);
  stream_block_decode(MB32(0x801f4764u), (int *)guest_at(MB32(0x801f4760u)),
               (int32_t)(sel & 3u));
  spr_draw_pair(MB32(0x801f4764u), 0, 0x1ff, 1, 0x200, 0, 1);
  cd_timer_read(0);
  scratch_013_set((uint32_t)f013);
  MW32(0x801b4154u, sel);
commit:
  MW16(0x1f80008cu, 0xffffu);
  rc = 1;
  (void)rc;
  return 1;
}

/* ======== batch46 ======== */

extern void slot_mark2_set(uint32_t);

/* desc_state_commit — descriptor state commit @0x8017c66c (80 insns,
 * 3 args). mode = sx4(MB16(a2)>>12) i.e. sign-extended HIGH NIBBLE of
 * byte1 (lhu;sll16;sra28 — NOT sx8 of the byte). mode==1: if
 * !(MB8(a0+5)&1) -> error stores; else ret a2+4. mode<=0 -> error
 * stores. mode==2: if !(MB8(a0+5)&1) ret a2+4; else error stores.
 * mode>2: sh -1 -> a1+0x101 (covers +0x101,+0x102) then the shared
 * +0x102..+0x104 byte stores — net all four bytes 0xff, same tail.
 * Error stores: a1+0x101..0x104 = -1, MB8(a0+0x26)=-1,
 * MB8(MB32(a0+0x14))=-1, MB16(a0+0xbc) = ((MB16(a2)>>8)&0xf)?-1:0,
 * then lb a2+2 <0 -> slot_mark2_set(MB8(a2+2)), +0xba=-1; else
 * +0xba=MB8(a2+2)<<4, if that byte==0 MB8(a0+0x26)=0.
 * +0xb8 = sx8(MB16(a2+2)>>8)<0 ? -1 : <<4.
 * MB16(MB32(a0+0x38)+0x116) = MB16(a0+0xba). Always ret a2+4. */
uint32_t desc_state_commit(uint32_t obj,uint32_t st,
                      uint32_t desc)
{
  int32_t mode = (int32_t)((int16_t)MB16(desc) >> 12);
  uint32_t db;
  uint32_t sub;
  int32_t b2;
  if (mode == 1) {
    if ((MB8(obj + 5) & 1u) != 0) return desc + 4;
  } else if (mode < 2) {
    /* fall into error stores */
  } else if (mode == 2) {
    if ((MB8(obj + 5) & 1u) == 0) return desc + 4;
  } else {
    /* guest: sh 0xffff -> a1+0x101 (bytes +0x101,+0x102), then the
     * shared sb 0xff -> +0x102,+0x103,+0x104 and the common tail */
    MW16(st + 0x101, 0xffffu);
    MW8(st + 0x102, 0xffu);
    MW8(st + 0x103, 0xffu);
    MW8(st + 0x104, 0xffu);
    goto common;
  }
  MW8(st + 0x101, 0xffu);
  MW8(st + 0x102, 0xffu);
  MW8(st + 0x103, 0xffu);
  MW8(st + 0x104, 0xffu);
common:
  sub = MB32(obj + 0x14);
  MW8(obj + 0x26, 0xffu);
  MW8(sub, 0xffu);
  db = (MB16(desc) >> 8) & 0xfu;
  MW16(obj + 0xbc, db != 0 ? 0xffffu : 0);
  if (MBS8(desc + 2) < 0) {
    slot_mark2_set(MB8(desc + 2));
    MW16(obj + 0xba, 0xffffu);
  } else {
    b2 = MB8(desc + 2);
    MW16(obj + 0xba, (uint16_t)(b2 << 4));
    if (b2 == 0) MW8(obj + 0x26, 0);
  }
  db = (uint32_t)(int32_t)(int8_t)(MB16(desc + 2) >> 8);
  MW16(obj + 0xb8, db != 0 && (int32_t)(int8_t)db < 0 ?
          0xffffu : (uint16_t)((int32_t)(int8_t)db << 4));
  MW16(MB32(obj + 0x38) + 0x116, MB16(obj + 0xba));
  return desc + 4;
}

/* counter_step_pad_c — port counter step @0x80129c10 (82 insns, 4
 * args). a3==0: flag 0x4000 -> deferred_cmd_post(0x400,0xa,0), s0++,
 * wrap to 0 if sx16(a1)<s0; flag 0x1000 -> same sfx, s0--, if
 * s0<0 s0=sx16(a1). a3==1: same with 0x2000/0x8000 and sfx 0xb.
 * Returns s0. Flag word = MB16(0x1f800074 + a2*2). */
int32_t counter_step_pad_c(uint32_t init,int32_t wrap,uint32_t pad,
                     uint32_t mode)
{
  int32_t ctr = (int32_t)init;
  uint32_t wrap0 = (uint32_t)wrap;
  uint32_t faddr = 0x1f800074u + pad * 2u;
  if (mode == 0) {
    if (MB16(faddr) == 0x4000u) {
      deferred_cmd_post(0x400, 0xa, 0);
      ctr++;
      if ((int32_t)(int16_t)wrap < ctr) ctr = 0;
    }
    if (MB16(faddr) == 0x1000u) {
      deferred_cmd_post(0x400, 0xa, 0);
      ctr--;
      if (ctr < 0) ctr = (int32_t)(int16_t)wrap;
    }
    return ctr;
  }
  if (mode == 1) {
    if (MB16(faddr) == 0x2000u) {
      deferred_cmd_post(0x400, 0xb, 0);
      ctr++;
      if ((int32_t)(int16_t)wrap < ctr) ctr = 0;
    }
    if (MB16(faddr) == 0x8000u) {
      deferred_cmd_post(0x400, 0xb, 0);
      ctr--;
      if (ctr < 0) ctr = (int32_t)(int16_t)wrap;
    }
  }
  return ctr;
}

/* ======== batch47 ======== */

/* signed /6 via the MIPS mult 0x2aaaaaab + hi>>8 - sign pattern */
static int32_t sdiv6(int32_t p)
{
  int32_t hi = (int32_t)(((int64_t)p * 0x2aaaaaabLL) >> 32);
  return (hi >> 8) - (p >> 31);
}

/* proj_coord_gte_emit — projected coord + GTE emit @0x8012eb74 (81
 * insns, 4 args). o = MB32(a3+0x20); v = atan2_lut(
 * MB32(o+0x17c), MB32(o+0x174)) (the sp+0x10/0x18 stores are dead
 * outgoing slots). a2 = clamp(MBS16(a3+0x72), <=0x600). Stores
 * sdiv6(lh(a0+0x14/0x16/0x18)*a2) to a1+0x28/0x2e/0x34.
 * vec2_rot(0x400, (sx16(v)+0x800)&0xfff, a2arg);
 * vec2_rot(lh(a0+0xa), lh(a0+8), a2arg+6);
 * angle_gte_vec(lh(a0+2), lh(a0), a2arg+0x20). */
void proj_coord_gte_emit(uint32_t vec,uint32_t dst,uint32_t xform,
                  uint32_t ent)
{
  uint32_t obj = MB32(ent + 0x20);
  int32_t yaw, scale;
  uint32_t rot;
  (void)MB32(obj + 0x174);
  (void)MB32(obj + 0x17c);
  yaw = atan2_lut((int32_t)MB32(obj + 0x17c), (int32_t)MB32(obj + 0x174));
  scale = MBS16(ent + 0x72);
  if (scale >= 0x601) scale = 0x600;
  MW16(dst + 0x28, (uint16_t)sdiv6(MBS16(vec + 0x14) * scale));
  MW16(dst + 0x2e, (uint16_t)sdiv6(MBS16(vec + 0x16) * scale));
  MW16(dst + 0x34, (uint16_t)sdiv6(MBS16(vec + 0x18) * scale));
  rot = ((uint32_t)(int32_t)(int16_t)yaw + 0x800u) & 0xfffu;
  vec2_rot(0x400, rot, (undefined2 *)guest_at(xform));
  vec2_rot((uint32_t)MBS16(vec + 0xa), (uint32_t)MBS16(vec + 8),
               (undefined2 *)guest_at(xform + 6));
  angle_gte_vec((uint32_t)MBS16(vec + 2), (uint32_t)MBS16(vec),
               (uint *)guest_at(xform + 0x20));
}

/* resource_rebuild_a — two-slot resource rebuild @0x8014c8d4 (87
 * insns). For i in 0..1 (e = MB32(0x801f10bc)+i*4): free
 * MB32(e+4)/MB32(e+0xc via task_node_free, zero them, then
 * e+4 = task_alloc_wrap_c(MB32(0x801f10c0)+off1+0xe,
 * MB32(0x801f10c4)+off1+0xe, 0x801b2ed8, 0x801b2ef4) and e+0xc
 * same with off2; off1 = 0xaa0+0x28i, off2 = 0xb90+0x28i.
 * Tail: MB8(0x1f800001) = 5. */
void resource_rebuild_a(void)
{
  int32_t i;
  uint32_t off1 = 0xaa0, off2 = 0xb90;
  for (i = 0; i < 2; i++) {
    uint32_t ent = MB32(0x801f10bcu) + (uint32_t)i * 4u;
    uint32_t t;
    t = MB32(ent + 4);
    if (t != 0) task_node_free(t);
    t = MB32(ent + 0xc);
    if (t != 0) task_node_free(t);
    MW32(ent + 4, 0);
    MW32(ent + 0xc, 0);
    MW32(ent + 4, task_alloc_wrap_c(MB32(0x801f10c0u) + off1 + 0xe,
                             MB32(0x801f10c4u) + off1 + 0xe,
                             0x801b2ed8u, 0x801b2ef4u));
    MW32(ent + 0xc, task_alloc_wrap_c(MB32(0x801f10c0u) + off2 + 0xe,
                               MB32(0x801f10c4u) + off2 + 0xe,
                               0x801b2ed8u, 0x801b2ef4u));
    off1 += 0x28;
    off2 += 0x28;
  }
  *(uint8_t *)guest_at(0x1f800001u) = 5;
}
/* ======== batch48 ======== */


/* resource_rebuild_b — two-slot resource rebuild B @0x8014c76c (89
 * insns). MB32(MB32(0x801f10bc)+0x1c) |= 0x820, then the same
 * 2-iteration free/zero/rebuild loop as resource_rebuild_a with string
 * pair 0x801b2eb4/0x801b2ecc. */
void resource_rebuild_b(void)
{
  int32_t i;
  uint32_t off1 = 0xaa0, off2 = 0xb90;
  uint32_t base = MB32(0x801f10bcu);
  MW32(base + 0x1c, MB32(base + 0x1c) | 0x820u);
  for (i = 0; i < 2; i++) {
    uint32_t ent = MB32(0x801f10bcu) + (uint32_t)i * 4u;
    uint32_t t;
    t = MB32(ent + 4);
    if (t != 0) task_node_free(t);
    t = MB32(ent + 0xc);
    if (t != 0) task_node_free(t);
    MW32(ent + 4, 0);
    MW32(ent + 0xc, 0);
    MW32(ent + 4, task_alloc_wrap_c(MB32(0x801f10c0u) + off1 + 0xe,
                             MB32(0x801f10c4u) + off1 + 0xe,
                             0x801b2eb4u, 0x801b2eccu));
    MW32(ent + 0xc, task_alloc_wrap_c(MB32(0x801f10c0u) + off2 + 0xe,
                               MB32(0x801f10c4u) + off2 + 0xe,
                               0x801b2eb4u, 0x801b2eccu));
    off1 += 0x28;
    off2 += 0x28;
  }
}

/* quad_gauge_emit — quad gauge emit @0x80158528 (91 insns, 5 args).
 * t0/s7/s2/fp = MB32(0x801f46f8/36e8/36e4/46f0)>>10. mode = stk arg:
 * 0 -> print_dispatch(0x80120d98) + print_dispatch(0x80120dac,
 * MB32(0x801f36e8), MB32(0x801f46f0)); 1 -> four spr_attr_alloc_a
 * border quads at head+0/+4 with y = a3+0x20 - edge, heights
 * 0xffff/0x60; else nothing. */
void quad_gauge_emit(uint32_t head,uint32_t unused,uint32_t y,
                  uint32_t rec,uint32_t mode)
{
  (void)unused;
  uint32_t c0 = MB32(0x801f46f8u) >> 10;
  uint32_t c1 = MB32(0x801f36e8u) >> 10;
  uint32_t c2 = MB32(0x801f36e4u) >> 10;
  uint32_t c3 = MB32(0x801f46f0u) >> 10;
  uint32_t yb, y7, h4;
  if (mode == 0) {
    print_dispatch((const char *)guest_at(0x80120d98u));
    print_dispatch((const char *)guest_at(0x80120dacu),
                 MB32(0x801f36e8u), MB32(0x801f46f0u));
  } else if (mode == 1) {
    yb = rec + 0x20;
    h4 = head + 4;
    y7 = y + 7;
    spr_attr_alloc_a(head, (int16_t)y, (int16_t)(yb - c0), 4,
                 (uint16_t)c0, 0xffffu, 0);
    spr_attr_alloc_a(h4, (int16_t)y, (int16_t)(yb - c2), 4,
                 (uint16_t)c2, 0x60u, 2);
    spr_attr_alloc_a(head, (int16_t)y7, (int16_t)(yb - c1), 4,
                 (uint16_t)c1, 0xffffu, 0);
    spr_attr_alloc_a(h4, (int16_t)y7, (int16_t)(yb - c3), 4,
                 (uint16_t)c3, 0x60u, 2);
  }
}

/* ======== batch49 ======== */

extern uint32_t rand_lcg(void);

/* hud_prim_init — 12-entry HUD/prim block init @0x80140b20 (91
 * insns, 2 args; v0 = MBS8(0x1f8000b1) live-in gate). Allocates
 * rand_lcg-style block via task_node_alloc(1,2,0xe,0x80147cc,
 * 0,0x190); s1 = MB32(blk+0x10); stores params at s1+0x160/0x164,
 * 0x14 at +0x168. Six halfword triple-writes from tables
 * 0x801b2308/0x801b2314 with running a2 += 0x2aa. Then two
 * 6x0x1c blocks copying u32s from 0x801b22f8/+4/+0xc, tag
 * 0x600000, byte 0x32 @+7, and 0x1000000/0xe1000020 at
 * +0x150/+0x154 of each 0xa8-strided block. */
void hud_prim_init(uint32_t f160,uint32_t f164)
{
  uint32_t blk, rec;
  int32_t i, j;
  int32_t attr;
  if (MBS8(0x1f8000b1u) != 0) return;
  blk = task_node_alloc(1, 2, 0xe, 0x80147ccu, 0, 0x190);
  if (blk == 0) return;
  rec = MB32(blk + 0x10);
  MW32(rec + 0x160, f160);
  MW32(rec + 0x164, f164);
  MW32(rec + 0x168, 0x14);
  attr = (int32_t)rand_lcg();
  {
    uint32_t wp = rec;
    uint32_t ta = 0x801b2308u, tb = 0x801b2314u;
    for (i = 0; i < 6; i++) {
      MW16(wp + 0x178, MB16(ta));
      ta += 2;
      MW16(wp + 0x16c, (uint16_t)attr);
      attr += 0x2aa;
      MW16(wp + 0x184, MB16(tb));
      tb += 2;
      wp += 2;
    }
  }
  for (i = 0; i < 2; i++) {
    uint32_t sub = rec + (uint32_t)i * 0xa8u;
    uint32_t wp = sub;
    for (j = 0; j < 6; j++) {
      MW32(wp + 4,    MB32(0x801b22f8u));
      MW32(wp + 0xc,  MB32(0x801b22fcu));
      MW32(wp + 0x14, MB32(0x801b2304u));
      MW32(wp, 0x600000u);
      *(uint8_t *)guest_at(wp + 7) = 0x32u;
      wp += 0x1c;
    }
    MW32(sub + 0x150, 0x1000000u);
    MW32(sub + 0x154, 0xe1000020u);
  }
}

/* ======== batch50 ======== */

/* record_table_expand_a — record table expander @0x80168f60 (102 insns, 7
 * args). For count (stk5) records stride dst+0x60 / src+0x14:
 * tag bytes 7/0x24 at +0x23/+0x27/+0x43/+0x47; copies u32s from
 * src rec +0/+4/+8 into +0x4c/+0x2c, +0x54/+0x34, +0x5c/+0x3c;
 * three u16 lookups MB16(src+0xc)*8+tblB -> +0x18/1a/1c; three
 * 8-byte copies via MB16(src+0xe/0x10/0x12)*8+tblA -> dst+0,
 * +8, +0x10; then u16 += dy/dx pairs at +0x2e/+0x36 and
 * +0x4e/+0x56 (lhu/addu/sh). Returns dst + count*0x60. */
uint32_t record_table_expand_a(uint32_t dst,uint32_t src,uint32_t tblA,
                      uint32_t tblB,uint32_t count,uint32_t dx,
                      uint32_t dy)
{
  uint32_t dw = dst;
  uint32_t srw = src;
  uint32_t n = count;
  uint32_t aw = dst + 0x4e;
  uint32_t srcw = src + 0x12;
  if (n == 0) return dw;
  do {
    uint32_t rec = aw - 0x4e;      /* record base */
    uint32_t ti;
    MW8(rec + 0x23, 7);
    MW8(rec + 0x27, 0x24);
    MW8(rec + 0x43, 7);
    MW8(rec + 0x47, 0x24);
    MW32(rec + 0x4c, MB32(srw));
    MW32(rec + 0x2c, MB32(srw));
    MW32(rec + 0x54, MB32(srcw - 0xe));
    MW32(rec + 0x34, MB32(srcw - 0xe));
    MW32(rec + 0x5c, MB32(srcw - 0xa));
    MW32(rec + 0x3c, MB32(srcw - 0xa));
    ti = tblB + (uint32_t)MBS16(srcw - 6) * 8u;
    MW16(rec + 0x18, MB16(ti));
    MW16(rec + 0x1a, MB16(ti + 2));
    MW16(rec + 0x1c, MB16(ti + 4));
    ti = tblA + (uint32_t)MBS16(srcw - 4) * 8u;
    MW32(dw,      MB32(ti));
    MW32(dw + 4,  MB32(ti + 4));
    ti = tblA + (uint32_t)MBS16(srcw - 2) * 8u;
    MW32(rec + 8,  MB32(ti));
    MW32(rec + 0xc, MB32(ti + 4));
    ti = tblA + (uint32_t)MBS16(srcw) * 8u;
    MW32(rec + 0x10, MB32(ti));
    MW32(rec + 0x14, MB32(ti + 4));
    MW16(rec + 0x36, MB16(rec + 0x36) + dx);
    MW16(rec + 0x2e, MB16(rec + 0x2e) + dy);
    MW16(rec + 0x56, MB16(rec + 0x56) + dx);
    MW16(rec + 0x4e, MB16(rec + 0x4e) + dy);
    dw += 0x60;
    aw += 0x60;
    srw += 0x14;
    srcw += 0x14;
    n--;
  } while (n != 0);
  return dw;
}

/* ======== batch51 ======== */

/* hp_bar_tick — gauge/HP bar tick @0x8015986c (105 insns; v0 =
 * MB32(0x801f4738) live-in gate, no args). If gate &&
 * MB32(0x801fc68c): cd_mc_state_machine(); clear it. Work on
 * 0x801f4700: +0x14 = (old==0); if MB32(0x801f473c)!=0 and
 * rem = lh(entry[+0x28]*8+0x1c) % lh(+0x30) != 0 -> clear gate,
 * +0x2c += rem; else +0x2c(0x801f472c) += +0x30(0x801f4730).
 * If MB16(+0x2c) < lh(+0x18)+lh(+0x1c) of entry[+0x28]: compute
 * cnt=lh(+0x32), prod = lh(+0x30)<<4 * (((cnt-1 or cnt+0xe)>>4)+1),
 * cd_fileop_forward(MB32(entry2+0xc), prod>>1, cnt, prod); else
 * +0x28 = (idx < sum), +0x34 = sum, +0x2c/+0x2e = MB16 of
 * entry[flag]+0x18/0x1a, MB32(0x801f473c) = sum. Tail:
 * gpu_load_image_cmd(rect=copy of +0x2c..0x33, MB32(0x801f470c+s0*4)). */
void hp_bar_tick(void)
{
  uint32_t rec_a = 0x801f4700u;
  uint32_t rec_b = rec_a;
  uint32_t fcnt, gbase, bidx, idx, flag, sum, prod, cnt, adj;
  /* guest rect = $sp+0x10 (frame -0x28) = entry_sp-0x18; it is filled
   * near the top and re-read by gpu_load_image_cmd at the bottom across
   * cd_mc_state_machine/cd_fileop_forward nested frames, so reserve the
   * guest frame window (sp+0x10 -> frm+0x18) for the function's
   * lifetime. */
  uint32_t hpb_frm = br2_guest_frame(0x28);
  uint32_t hpb_rect = hpb_frm + 0x18;
  if (MB32(0x801f4738u) != 0) {
    if (MB32(0x801fc68cu) != 0) {
      cd_mc_state_machine();
      MW32(0x801fc68cu, 0);
    }
  }
  memcpy(guest_at(hpb_rect), guest_at(rec_a + 0x2c), 8);
  fcnt = MB32(rec_a + 0x14);
  MW32(rec_a + 0x14, fcnt < 1u ? 1u : 0u);
  if (MB32(0x801f473cu) != 0) {
    int32_t rem;
    idx = MB32(rec_a + 0x28) * 8u + rec_a;
    /* guest div @0x801598F8 + mfhi: HI = lh(+0x1c) % lh(+0x30);
     * divisor 0 -> HI = dividend (no trap). */
    {
      int32_t divisor = MBS16(rec_a + 0x30);
      int32_t dividend = MBS16(idx + 0x1c);
      rem = divisor == 0 ? dividend : dividend % divisor;
    }
    if (rem != 0) {
      MW32(0x801f473cu, 0);
      MW16(rec_a + 0x2c, MB16(rec_a + 0x2c) + (uint16_t)rem);
    } else {
      MW16(0x801f472cu, MB16(0x801f472cu) + MB16(0x801f4730u));
    }
  } else {
    MW16(0x801f472cu, MB16(0x801f472cu) + MB16(0x801f4730u));
  }
  bidx = MB32(rec_b + 0x28);
  idx = bidx * 8u + rec_b;
  sum = (uint32_t)(int32_t)(MBS16(idx + 0x18) + MBS16(idx + 0x1c));
  if (MB16(rec_b + 0x2c) < (int32_t)sum) {
    cnt = (uint32_t)MBS16(rec_b + 0x32);
    gbase = (uint32_t)MBS16(rec_b + 0x30) << 4;
    adj = cnt - 1;
    if ((int32_t)adj < 0) adj = cnt + 0xe;
    prod = (uint32_t)((int32_t)(gbase * ((adj >> 4) + 1)));
    /* cd_fileop_forward forwards only a0/a1 to mdec_pump_b; cnt/prod
     * (bidx/a3) are dead registers in the guest thunk. */
    cd_fileop_forward(MB32(rec_b + (MB32(rec_b + 0x14) * 4u) + 0xc),
                 prod >> 1);
  } else {
    /* a0 still = lh(entry+0x1c): unsigned idx<a0 flag, +0x34 and
     * the 0x801f473c gate store that sx16 value. */
    uint32_t lim = (uint32_t)MBS16(idx + 0x1c);
    flag = bidx < lim ? 1u : 0u;
    MW32(rec_b + 0x28, flag);
    idx = rec_b + flag * 8u;
    MW32(rec_b + 0x34, lim);
    MW16(rec_b + 0x2c, MB16(idx + 0x18));
    MW16(rec_b + 0x2e, MB16(idx + 0x1a));
    MW32(0x801f473cu, lim);
  }
  idx = 0x801f4700u + fcnt * 4u;
  gpu_load_image_cmd(hpb_rect, MB32(idx + 0xc));
  br2_guest_frame_free();
}

/* ======== batch52 ======== */

extern void slot_status_fill(uint8_t *);

/* cd_sector_debug_dump — CD/debug sector dump @0x80158e5c (114 insns).
 * s2 = 0x801bef60. sel = MB8(s2+0xe)&0xf: 1 -> print_mode_dispatch(
 * 0x8012ddf0), 2 -> (0x8012ddfc); high nibble: !=0 ->
 * 0x8012de08 else 0x8012de10 (remaining ce30 args are
 * caller-leftover regs in the original; zeroed here). Then
 * cd_file_open time-decode + print_dispatch label prints for the
 * four fields at s2+0x20/0x24/0x28, slot_status_fill 24-byte status
 * dump at sp+0x20 printed 12+12 via 0x80120e84/0x80120e8c. */
void cd_sector_debug_dump(void)
{
  uint8_t buf8[8];
  uint8_t buf24[24];
  uint32_t st = 0x801bef60u;
  int32_t sel = MB8(st + 0xe) & 0xf;
  int32_t i;
  if (sel == 1) {
    print_mode_dispatch(0x8012ddf0u,0,0,0,0,0,0,0,0);
  } else if (sel == 2) {
    print_mode_dispatch(0x8012ddfcu,0,0,0,0,0,0,0,0);
  }
  if ((MB8(st + 0xe) & 0xf0u) != 0) {
    print_mode_dispatch(0x8012de08u,0,0,0,0,0,0,0,0);
  } else {
    print_mode_dispatch(0x8012de10u,0,0,0,0,0,0,0,0);
  }
  cd_file_open((int32_t)MB32(st + 0x20), buf8);
  print_dispatch((const char *)guest_at(0x8012de18u),
               buf8[0], buf8[1], buf8[2]);
  cd_file_open((int32_t)(MB32(st + 0x20) - MB32(st + 0x24)), buf8);
  print_dispatch((const char *)guest_at(0x8012de30u),
               buf8[0], buf8[1], buf8[2]);
  cd_file_open((int32_t)MB32(st + 0x24), buf8);
  print_dispatch((const char *)guest_at(0x8012de48u),
               buf8[0], buf8[1], buf8[2]);
  cd_file_open((int32_t)MB32(st + 0x28), buf8);
  print_dispatch((const char *)guest_at(0x8012de60u),
               buf8[0], buf8[1], buf8[2]);
  slot_status_fill(buf24);
  print_dispatch((const char *)guest_at(0x8012de78u));
  for (i = 0; i < 0xc; i++) {
    print_dispatch((const char *)guest_at(0x80120e84u), buf24[i]);
  }
  print_dispatch((const char *)guest_at(0x80120e8cu));
  for (i = 0xc; i < 0x18; i++) {
    print_dispatch((const char *)guest_at(0x80120e84u), buf24[i]);
  }
  print_dispatch((const char *)guest_at(0x80120e8cu));
}

/* record_table_expand_b — record table expander variant @0x801690fc (118
 * insns, 7 args). Same shape as record_table_expand_a with dst stride
 * 0x78, src stride 0x18, tags 9/0x2c at +0x2b/+0x2f/+0x53/+0x57;
 * extra u16 copy src+0xc -> +0x4c/+0x74; coord adds dx at
 * +0x3e/+0x66, dy at +0x36/+0x5e. Returns dst + count*0x78. */
uint32_t record_table_expand_b(uint32_t dst,uint32_t src,uint32_t tblA,
                      uint32_t tblB,uint32_t count,uint32_t dx,
                      uint32_t dy)
{
  uint32_t dw = dst;
  uint32_t srw = src;
  uint32_t n = count;
  uint32_t aw = dst + 0x5e;
  uint32_t srcw = src + 0x16;
  if (n == 0) return dw;
  do {
    uint32_t rec = aw - 0x5e;
    uint32_t ti;
    MW8(rec + 0x2b, 9);
    MW8(rec + 0x2f, 0x2c);
    MW8(rec + 0x53, 9);
    MW8(rec + 0x57, 0x2c);
    MW32(rec + 0x5c, MB32(srw));
    MW32(rec + 0x34, MB32(srw));
    MW32(rec + 0x64, MB32(srcw - 0x12));
    MW32(rec + 0x3c, MB32(srcw - 0x12));
    MW32(rec + 0x6c, MB32(srcw - 0xe));
    MW32(rec + 0x44, MB32(srcw - 0xe));
    MW32(rec + 0x74, MB16(srcw - 0xa));
    MW32(rec + 0x4c, MB16(srcw - 0xa));
    ti = tblB + (uint32_t)MBS16(srcw - 8) * 8u;
    MW16(rec + 0x20, MB16(ti));
    MW16(rec + 0x22, MB16(ti + 2));
    MW16(rec + 0x24, MB16(ti + 4));
    ti = tblA + (uint32_t)MBS16(srcw - 6) * 8u;
    MW32(dw,      MB32(ti));
    MW32(dw + 4,  MB32(ti + 4));
    ti = tblA + (uint32_t)MBS16(srcw - 4) * 8u;
    MW32(rec + 8,  MB32(ti));
    MW32(rec + 0xc, MB32(ti + 4));
    ti = tblA + (uint32_t)MBS16(srcw - 2) * 8u;
    MW32(rec + 0x10, MB32(ti));
    MW32(rec + 0x14, MB32(ti + 4));
    ti = tblA + (uint32_t)MBS16(srcw) * 8u;
    MW32(rec + 0x18, MB32(ti));
    MW32(rec + 0x1c, MB32(ti + 4));
    MW16(rec + 0x3e, MB16(rec + 0x3e) + dx);
    MW16(rec + 0x36, MB16(rec + 0x36) + dy);
    MW16(rec + 0x66, MB16(rec + 0x66) + dx);
    MW16(rec + 0x5e, MB16(rec + 0x5e) + dy);
    dw += 0x78;
    aw += 0x78;
    srw += 0x18;
    srcw += 0x18;
    n--;
  } while (n != 0);
  return dw;
}

/* ======== batch53 ======== */

/* condition_word_dispatch — condition-word dispatcher @0x8017d378 (120
 * insns, 3 args). sx = sx8(MB16(a2)>>8); mode = (sx>>4)&3;
 * sx&0x40 -> a0 = MB32(a0+0x2c). mode1: bit5 set -> ret a2+6;
 * mode2: bit5 clear -> ret a2+6; mode0/3 fall through. flag =
 * sx&0x80: ==0 -> jump table 0x80122984[v1&0xf<9]: idx1-5 store
 * MB16(a0+{7c,7a,80,84,82}) & MB16(a2+2) to a1+0x94; idx6-8 call
 * hit_chain_clear/da0/dd8 then ret a2+6; ==0x40 -> table
 * 0x801229ac[v1&0xf<6] xor-test stores (dead: flag can only be
 * 0/0x80); else ret a2+6. */
uint32_t condition_word_dispatch(uint32_t obj,uint32_t st,
                      uint32_t desc)
{
  int32_t sx = (int32_t)(int8_t)(MB16(desc) >> 8);
  int32_t mode = (sx >> 4) & 3;
  uint32_t f80, idx;
  if (sx & 0x40) obj = MB32(obj + 0x2c);
  if (mode == 1) {
    if ((MB8(obj + 5) & 1u) != 0) return desc + 6;
  } else if (mode < 2) {
    /* fall into flag dispatch */
  } else if (mode == 2) {
    if ((MB8(obj + 5) & 1u) == 0) return desc + 6;
  }
  f80 = (uint32_t)sx & 0x80u;
  if (f80 == 0) {
    idx = (uint32_t)sx & 0xfu;
    if (idx >= 9) return desc + 6;
    switch (idx) {
    case 0: return desc + 6;
    case 1: MW32(st + 0x94, MB16(obj + 0x7c) & MB16(desc + 2)); break;
    case 2: MW32(st + 0x94, MB16(obj + 0x7a) & MB16(desc + 2)); break;
    case 3: MW32(st + 0x94, MB16(obj + 0x80) & MB16(desc + 2)); break;
    case 4: MW32(st + 0x94, MB16(obj + 0x84) & MB16(desc + 2)); break;
    case 5: MW32(st + 0x94, MB16(obj + 0x82) & MB16(desc + 2)); break;
    case 6: hit_chain_clear(obj, st); return desc + 6;
    case 7: pending_hit_a_clear(obj, st); return desc + 6;
    case 8: pending_hit_b_clear(obj, st); return desc + 6;
    }
    return desc + 6;
  }
  if (f80 == 0x40) {   /* unreachable: f80 is only ever 0 or 0x80 */
    static const uint16_t off[5] = {0x7c,0x7a,0x80,0x84,0x82};
    idx = (uint32_t)sx & 0xfu;
    if (idx >= 6 || idx == 0) return desc + 6;
    {
      uint32_t xf = (MB16(obj + off[idx - 1]) & MB16(desc + 2))
                   ^ MB16(desc + 4);
      MW32(st + 0x94, xf < 1u ? 1u : 0u);
    }
  }
  return desc + 6;
}

/* ======== batch54 ======== */

extern int burst_slot_active(int32_t);

/* button_cond_eval — button/condition eval B @0x8017d7f4 (121
 * insns, 3 args). Same sx/mode prologue as condition_word_dispatch.
 * mode1 bit5 set or mode2 bit5 clear -> ret a2+4. Else idx =
 * sx&0xf: 0 -> zero a1+0x94, require MB32(MB32(a0+0x20)+0x178)
 * >= -0x280 and MB32(s0+0x5c)&0x1000000, then for bits 1/2/4/8
 * of MB32(s0+0x6c) call burst_slot_active(0..3) and store 1 on hit;
 * 1 -> same gates then a1+0x94 = idx(1); default ret a2+2. */
uint32_t button_cond_eval(uint32_t obj,uint32_t st,
                      uint32_t desc)
{
  int32_t sx = (int32_t)(int8_t)(MB16(desc) >> 8);
  int32_t mode = (sx >> 4) & 3;
  uint32_t idx;
  if (sx & 0x40) obj = MB32(obj + 0x2c);
  if (mode == 1) {
    if ((MB8(obj + 5) & 1u) != 0) return desc + 4;
  } else if (mode < 2) {
    /* fall into dispatch */
  } else if (mode == 2) {
    if ((MB8(obj + 5) & 1u) == 0) return desc + 4;
  }
  idx = (uint32_t)sx & 0xfu;
  if (idx == 0 || idx == 1) {
    uint32_t gate;
    MW32(st + 0x94, 0);
    gate = MB32(MB32(obj + 0x20) + 0x178);
    if ((int32_t)gate < -0x280) return desc + 2;
    if ((MB32(obj + 0x5c) & 0x1000000u) == 0) return desc + 2;
    if (idx == 1) {
      MW32(st + 0x94, idx);
      return desc + 2;
    }
    if ((MB32(obj + 0x6c) & 1u) && burst_slot_active(0))
      MW32(st + 0x94, 1);
    if ((MB32(obj + 0x6c) & 2u) && burst_slot_active(1))
      MW32(st + 0x94, 1);
    if ((MB32(obj + 0x6c) & 4u) && burst_slot_active(2))
      MW32(st + 0x94, 1);
    if (MB32(obj + 0x6c) & 8u) {
      if (burst_slot_active(3)) MW32(st + 0x94, 1);
    }
    return desc + 2;
  }
  return desc + 2;
}

/* ======== batch55 ======== */

extern void impact_fx_spawn(void *,uint32_t,int32_t,int32_t,uint32_t);

/* script_entry_walk — script entry walker @0x80140518 (125 insns; v0 =
 * MBS8(0x1f8000b1) live-in gate, 1 arg). Walks MB32(s0+4) entries:
 * tag = MB16(e)&0xff00, sub = &0xff. 0xfe00 -> e = base +
 * lh(e+2)*8, s0+0xc = MB16(e). 0xfc00 -> if sub < lh(s0+0xe)
 * e+=8 else e = base+lh(e+2)*8, lh(s0+0xe)++, s0+0xc = MB16(e).
 * 0xff00 -> sub < lh(s0+0xe) ? e+=8 : task_slot_step(); return.
 * Other -> match block: if lh(s0+0xc)!=lh(e) {s0+0xc++; ret}.
 * Body: v1 = lh(e+4), if lh(s0+0x10)!=0 v1 = MB8(0x801b180c+v1);
 * for lh(e+2) iters: if lh(cur+6)==0 impact_fx_spawn(
 * MB32(MB32(s0+8)+0x20)+v1*80+0x34, 0x80000003, lh(s0+0x12),0,0).
 * Advance e+=8 while lh(s0+0xc)==lh(prev+8); else s0+0xc++, ret. */
void script_entry_walk(uint32_t w)
{
  uint32_t lut = 0x801b180cu;
  uint32_t e, ne, v;
  uint32_t tag, sub;
  int32_t i;
  uint32_t voff;
  if (MBS8(0x1f8000b1u) != 0) return;
  for (;;) {
    e = MB32(w + 4);
    v = MB16(e);
    sub = v & 0xffu;
    tag = v & 0xff00u;
    if (tag == 0xfe00u) {
      ne = MB32(w) + (uint32_t)MBS16(e + 2) * 8u;
      MW32(w + 4, ne);
      MW16(w + 0xc, MB16(ne));
      continue;
    }
    if ((int32_t)tag > 0xfe00) {
      if (tag == 0xff00u) {
        if (sub < (uint32_t)MBS16(w + 0xe)) {
          MW32(w + 4, e + 8);
        } else {
          task_slot_step();
        }
        return;
      }
    } else if (tag == 0xfc00u) {
      if (sub < (uint32_t)MBS16(w + 0xe)) {
        MW32(w + 4, e + 8);
      } else {
        ne = MB32(w) + (uint32_t)MBS16(e + 2) * 8u;
        MW32(w + 4, ne);
        MW16(w + 0xe, MB16(w + 0xe) + 1);
        MW16(w + 0xc, MB16(ne));
      }
      continue;
    }
    /* match block */
    ne = MB32(w + 4);
    if (MBS16(w + 0xc) != MBS16(ne)) {
      MW16(w + 0xc, MB16(w + 0xc) + 1);
      return;
    }
    for (;;) {
      v = (uint32_t)MBS16(ne + 4);
      if (MBS16(w + 0x10) != 0)
        v = MB8(lut + v);
      if (MBS16(ne + 2) > 0) {
        voff = v * 80u;
        for (i = 0; i < MBS16(MB32(w + 4) + 2); i++) {
          if (MBS16(MB32(w + 4) + 6) == 0) {
            impact_fx_spawn((void *)guest_at(
                           MB32(MB32(w + 8) + 0x20) + voff + 0x34),
                         0x80000003u, MBS16(w + 0x12), 0, 0);
          }
        }
      }
      e = MB32(w + 4);
      MW32(w + 4, e + 8);
      if (MBS16(w + 0xc) != MBS16(e + 8)) break;
      ne = e + 8;
    }
    MW16(w + 0xc, MB16(w + 0xc) + 1);
    return;
  }
}

/* ======== batch56 ======== */

/* color_rec_table_init — 60-record colour table init @0x8014140c (128
 * insns, 2 args). blk = task_node_alloc(1,2,0xe,0x801411e8,0,0x9fc);
 * t0 = MB32(blk+0x10); MB32(t0+0x9f8) = param_2. Two outer blocks
 * (a3 = 0, 0x4b0) x 30 records stride 0x28 at t0+a3+i*0x28:
 * tag 0x90000000, byte 0x2e @+7, 0x39 @+0x16, 0x2a @+4/5/6, and
 * an RGB-ish 8-byte pattern selected by i&3 (case0/1:
 * a0/c0/a7/df, case2: a8/c0/af/df, case3: 70/a0/7f/af). Phase 2:
 * per i<30: two rand_lcg()&0x1ff halfwords at t0+0x960+4i,
 * byte (rand&0x3f)+0x20 if i&3==2 else +8 at t0+0x9d8+i. */
void color_rec_table_init(uint32_t unused,uint32_t param16)
{
  uint32_t blk, rec;
  int32_t oi, i;
  uint32_t a3off;
  (void)unused;
  blk = task_node_alloc(1, 2, 0xe, 0x801411e8u, 0, 0x9fc);
  if (blk == 0) return;
  rec = MB32(blk + 0x10);
  MW32(rec + 0x9f8, param16);
  a3off = 0;
  for (oi = 0; oi < 2; oi++) {
    for (i = 0; i < 0x1e; i++) {
      uint32_t rp = rec + a3off + (uint32_t)i * 0x28u;
      uint32_t ci = (uint32_t)i & 3u;
      MW32(rp, 0x90000000u);
      MW8(rp + 7, 0x2e);
      MW16(rp + 0x16, 0x39);
      MW8(rp + 4, 0x2a);
      MW8(rp + 5, 0x2a);
      MW8(rp + 6, 0x2a);
      if (ci == 2) {
        MW8(rp + 0xc, 0xa8); MW8(rp + 0xd, 0xc0);
        MW8(rp + 0x14, 0xaf); MW8(rp + 0x15, 0xc0);
        MW8(rp + 0x1c, 0xa8); MW8(rp + 0x1d, 0xdf);
        MW8(rp + 0x24, 0xaf); MW8(rp + 0x25, 0xdf);
      } else if (ci == 3) {
        MW8(rp + 0xc, 0x70); MW8(rp + 0xd, 0xa0);
        MW8(rp + 0x14, 0x7f); MW8(rp + 0x15, 0xa0);
        MW8(rp + 0x1c, 0x70); MW8(rp + 0x1d, 0xaf);
        MW8(rp + 0x24, 0x7f); MW8(rp + 0x25, 0xaf);
      } else if (ci < 3) {
        MW8(rp + 0xc, 0xa0); MW8(rp + 0xd, 0xc0);
        MW8(rp + 0x14, 0xa7); MW8(rp + 0x15, 0xc0);
        MW8(rp + 0x1c, 0xa0); MW8(rp + 0x1d, 0xdf);
        MW8(rp + 0x24, 0xa7); MW8(rp + 0x25, 0xdf);
      }
    }
    a3off += 0x4b0;
  }
  {
    uint32_t wp = rec, bp = rec;
    for (i = 0; i < 0x1e; i++) {
      uint32_t r;
      r = rand_lcg() & 0x1ffu;
      MW16(wp + 0x960, (uint16_t)r);
      r = rand_lcg() & 0x1ffu;
      MW16(wp + 0x962, (uint16_t)r);
      r = rand_lcg() & 0x3fu;
      MW8(bp + 0x9d8, (uint8_t)(r + ((i & 3) == 2 ? 0x20 : 8)));
      bp += 1;
      wp += 4;
    }
  }
}

/* ======== batch57 ======== */

/* emitter_30_teardown — 30-slot emitter teardown @0x801411e8 (136
 * insns). Restores 8 halfwords 0x801b2320 -> 0x1f8003f0. s5 =
 * s4 + MB32(0x1f80000c)*1200. If MB16(0x1f800082)==0: decrement
 * MB32(s4+0x9f8), <0 -> task_slot_step + ret; else 30-iter decay:
 * MB16(s4+0x962+4i) -= MB8(s4+0x9d8+i), clamp <-0x40 -> 0x200 +
 * rand halfword at s4+0x960+4i. Emit loop (always): per i<30,
 * 8 halfword pairs from s4+0x960/0x962 (+0x18/+0x60 deltas, first
 * field palette-indexed via 0x1f8003f0[(v&7)*2]) written at
 * s5+0xe..+0x22 step 0x28, GPU link 0x90000000|field with
 * backlink s5&0xffffff into MB32(MB32(0x1f800008)+0x203c). */
void emitter_30_teardown(uint32_t rec)
{
  uint32_t slot, p16, q16, dec, dst, qnew, dec_b;
  int32_t i;
  uint32_t pal = 0x1f8003f0u;
  for (i = 0; i < 8; i++)
    MW16(pal + (uint32_t)i * 2u, MB16(0x801b2320u + (uint32_t)i * 2u));
  slot = rec + MB32(0x1f80000cu) * 1200u;
  if (MB16(0x1f800082u) == 0) {
    int32_t c = (int32_t)MB32(rec + 0x9f8) - 1;
    MW32(rec + 0x9f8, (uint32_t)c);
    if (c < 0) {
      task_slot_step();
      return;
    }
    p16 = rec + 0x960;
    dec = rec + 0x9d8;
    q16 = rec + 0x962;
    for (i = 0; i < 0x1e; i++) {
      dec_b = MB8(dec);
      qnew = (uint32_t)(int16_t)(MB16(q16) - dec_b);
      MW16(q16, (uint16_t)qnew);
      if ((int32_t)(int16_t)qnew < -0x40) {
        MW16(q16, 0x200u);
        MW16(p16, (uint16_t)(rand_lcg() & 0x1ffu));
      }
      dec += 1;
      q16 += 4;
      p16 += 4;
    }
  }
  p16 = rec + 0x960;
  q16 = rec + 0x962;
  dst = slot + 0x22;
  for (i = 0; i < 0x1e; i++) {
    MW16(dst - 0x14, MB16(pal + (MB16(q16) & 7u) * 2u));
    MW16(dst - 0x1a, MB16(p16));
    MW16(dst - 0x18, MB16(q16));
    MW16(dst - 0x12, MB16(p16) + 0x18u);
    MW16(dst - 0x10, MB16(q16));
    MW16(dst - 0xa,  MB16(p16));
    MW16(dst - 8,   MB16(q16) + 0x60u);
    MW16(dst - 2,   MB16(p16) + 0x18u);
    MW16(dst,       MB16(q16) + 0x60u);
    qnew = MB32(MB32(0x1f800008u) + 0x203cu) | 0x90000000u;
    MW32(slot, qnew);
    MW32(MB32(0x1f800008u) + 0x203cu, slot & 0xffffffu);
    p16 += 4;
    q16 += 4;
    dst += 0x28;
    slot += 0x28;
  }
}

/* ======== batch58 ======== */

extern void keyframe_advance(byte*,byte*);

/* stat_field_dispatch — stat-field dispatcher @0x8017bd94 (136 insns,
 * 3 args; jump table 0x801228ec on sx8(MB16(a2)>>8), idx>=8 ->
 * ret a2+4). All paths ret a2+4. Cases (a3=param_1, a1=param_2):
 * 0: t=lh(a2+2)<2 ? {c2+=88-ca, c8=ca=88} : {c8=ca=t*16,
 *    c2+=t*16-ca};
 * 1: c6=MB16(a2+2); sub-dispatch on lh(sx16(c6)*4+MB32(a1+0xb4)+2)
 *    same shape vs 0xcc; c6++; keyframe_advance(a3);
 * 2: t=lh(a3+0xbe); t<2 ? c8=ca=MB16(a3+0x88) : c8=ca=t*16
 *    (s0+2 is NOT read on this path — verified vs 0x8017bebc..bf50);
 * 3: d=lh(a2+2)*16; cc+=d; c2+=d;
 * 4: t=((lh(a3+0xb2)<0?+0x1ff:0)>>9)+lh(a2+2), clamp 0xf;
 *    t<2 ? c8=ca=88 : c8=ca=t*16;
 * 5: cc += lh(a3+0xc2)*16;
 * 6: cc += lh(a3+0xbe)*16;
 * 7: ca+=lh(a2+2)*16; c2+=same; c8+=same. */
uint32_t stat_field_dispatch(uint32_t src,uint32_t dst,
                      uint32_t desc)
{
  int32_t idx = (int32_t)(int8_t)(MB16(desc) >> 8);
  int32_t t;
  uint32_t val, dlt;
  if ((uint32_t)idx >= 8u) return desc + 4;
  switch (idx) {
  case 0:
    t = MBS16(desc + 2);
    if (t < 2) {
      dlt = MB16(src + 0x88) - MB16(dst + 0xca);
      MW16(dst + 0xc2, MB16(dst + 0xc2) + dlt);
      MW16(dst + 0xc8, MB16(src + 0x88));
      MW16(dst + 0xca, MB16(src + 0x88));
    } else {
      t <<= 4;
      dlt = MB16(dst + 0xca);
      MW16(dst + 0xc8, (uint16_t)t);
      MW16(dst + 0xca, (uint16_t)t);
      MW16(dst + 0xc2, MB16(dst + 0xc2) + (uint16_t)(t - dlt));
    }
    break;
  case 1: {
    uint32_t c6 = MB16(desc + 2);
    uint32_t e = MB32(dst + 0xb4) + (uint32_t)(int32_t)(int16_t)c6 * 4u;
    MW16(dst + 0xc6, (uint16_t)c6);
    t = MBS16(e + 2);
    if (t < 2) {
      dlt = MB16(src + 0x88) - MB16(dst + 0xcc);
      MW16(dst + 0xc2, MB16(dst + 0xc2) + dlt);
      MW16(dst + 0xcc, MB16(src + 0x88));
    } else {
      t <<= 4;
      dlt = MB16(dst + 0xcc);
      MW16(dst + 0xcc, (uint16_t)t);
      MW16(dst + 0xc2, MB16(dst + 0xc2) + (uint16_t)(t - dlt));
    }
    MW16(dst + 0xc6, MB16(dst + 0xc6) + 1);
    keyframe_advance((byte *)guest_at(src),(byte *)guest_at(dst));
    break;
  }
  case 2:
    t = MBS16(src + 0xbe);
    val = t < 2 ? MB16(src + 0x88) : (uint32_t)(t << 4);
    MW16(dst + 0xc8, (uint16_t)val);
    MW16(dst + 0xca, (uint16_t)val);
    break;
  case 3:
    val = (uint32_t)(int32_t)(int16_t)(MB16(desc + 2) * 16);
    MW16(dst + 0xcc, MB16(dst + 0xcc) + (uint16_t)val);
    MW16(dst + 0xc2, MB16(dst + 0xc2) + (uint16_t)val);
    break;
  case 4: {
    int32_t b = MBS16(src + 0xb2);
    if (b < 0) b += 0x1ff;
    t = (b >> 9) + MBS16(desc + 2);
    if (t >= 0x10) t = 0xf;
    if (t < 2) {
      MW16(dst + 0xc8, MB16(src + 0x88));
      MW16(dst + 0xca, MB16(src + 0x88));
    } else {
      val = (uint32_t)(t * 16);
      MW16(dst + 0xc8, (uint16_t)val);
      MW16(dst + 0xca, (uint16_t)val);
    }
    break;
  }
  case 5:
    val = (uint32_t)(int32_t)(int16_t)(MB16(src + 0xc2) * 16);
    MW16(dst + 0xcc, MB16(dst + 0xcc) + (uint16_t)val);
    break;
  case 6:
    val = (uint32_t)(int32_t)(int16_t)(MB16(src + 0xbe) * 16);
    MW16(dst + 0xcc, MB16(dst + 0xcc) + (uint16_t)val);
    break;
  case 7:
    val = (uint32_t)(int32_t)(int16_t)(MB16(desc + 2) * 16);
    MW16(dst + 0xca, MB16(dst + 0xca) + (uint16_t)val);
    MW16(dst + 0xc2, MB16(dst + 0xc2) + (uint16_t)val);
    MW16(dst + 0xc8, MB16(dst + 0xc8) + (uint16_t)val);
    break;
  }
  return desc + 4;
}

/* ======== batch59 ======== */

/* quad_strip_emit_a — quad-strip emitter variant @0x801692d8 (~144
 * insns, 7 args: dst, src, texTab, palTab, count, dx, dy on
 * stack). Same shape as record_table_expand_a/record_table_expand_b with dst stride
 * 0x80, src stride 0x18, AO=0x66/A1=0x16, attr bytes 9/0x34.
 * Three palette triples (palTab[idx*8]) and four 8-byte texel
 * copies (texTab[idx*8]) via lwl/lwr+swl/swr = unaligned LE
 * word pairs, expressed as MB32/MW32 pairs. lwl/lwr at
 * v0..v0+7 == MB32(v0),MB32(v0+4) for any alignment on this
 * flat guest memory. Returns dst + count*0x80. */
uint32_t quad_strip_emit_a(uint32_t dst, uint32_t src, uint32_t tb,
                      uint32_t ta, uint32_t count, uint32_t dx,
                      uint32_t dy)
{
  uint32_t dw = dst, srw = src;
  uint32_t wa = dst + 0x66, sa = src + 0x16;
  int32_t n = (int32_t)count;
  uint32_t v, ti;
  if (n == 0) return dw;
  for (; n != 0; n--) {
    MW8(wa - 0x33, 9); MW8(wa - 0x2f, 0x34);
    MW8(wa - 0xb, 9);  MW8(wa - 7, 0x34);
    v = MB32(srw);
    MW32(wa - 2, v); MW32(wa - 0x2a, v);
    v = MB32(sa - 0x12);
    MW32(wa + 0xa, v); MW32(wa - 0x1e, v);
    v = MB32(sa - 0xe);
    MW32(wa + 0x16, v); MW32(wa - 0x12, v);
    ti = ta + (uint32_t)MBS16(sa - 0xa) * 8u;
    MW16(wa - 0x4e, MB16(ti));
    MW16(wa - 0x4c, MB16(ti + 2));
    MW16(wa - 0x4a, MB16(ti + 4));
    ti = tb + (uint32_t)MBS16(sa - 8) * 8u;
    MW32(dw, MB32(ti)); MW32(dw + 4, MB32(ti + 4));
    ti = ta + (uint32_t)MBS16(sa - 6) * 8u;
    MW16(wa - 0x46, MB16(ti));
    MW16(wa - 0x44, MB16(ti + 2));
    MW16(wa - 0x42, MB16(ti + 4));
    ti = tb + (uint32_t)MBS16(sa - 4) * 8u;
    MW32(wa - 0x5e, MB32(ti)); MW32(wa - 0x5a, MB32(ti + 4));
    ti = ta + (uint32_t)MBS16(sa - 2) * 8u;
    MW16(wa - 0x3e, MB16(ti));
    MW16(wa - 0x3c, MB16(ti + 2));
    MW16(wa - 0x3a, MB16(ti + 4));
    ti = tb + (uint32_t)MBS16(sa + 0) * 8u;
    MW32(wa - 0x56, MB32(ti)); MW32(wa - 0x52, MB32(ti + 4));
    MW16(wa - 0x1c, MB16(wa - 0x1c) + dx);
    MW16(wa - 0x28, MB16(wa - 0x28) + dy);
    MW16(wa + 0xc, MB16(wa + 0xc) + dx);
    MW16(wa + 0, MB16(wa + 0) + dy);
    wa += 0x80; sa += 0x18; dw += 0x80; srw += 0x18;
  }
  return dw;
}

/* quad_strip_emit_b — quad-strip emitter variant @0x8016951c (~181
 * insns). dst stride 0xa8, src stride 0x1c, AO=0x82/A1=0x1a,
 * attr bytes 0xc/0x3c, four palette triples, four texel copies.
 * Returns dst + count*0xa8. */

uint32_t quad_strip_emit_b(uint32_t dst, uint32_t src, uint32_t tb,
                      uint32_t ta, uint32_t count, uint32_t dx,
                      uint32_t dy)
{
  uint32_t dw = dst, srw = src;
  uint32_t wa = dst + 0x82, sa = src + 0x1a;
  int32_t n = (int32_t)count;
  uint32_t v, ti;
  if (n == 0) return dw;
  for (; n != 0; n--) {
    MW8(wa - 0x3f, 0xc); MW8(wa - 0x3b, 0x3c);
    MW8(wa - 0xb, 0xc);  MW8(wa - 7, 0x3c);
    v = MB32(srw);
    MW32(wa - 2, v); MW32(wa - 0x36, v);
    v = MB32(sa - 0x16);
    MW32(wa + 0xa, v); MW32(wa - 0x2a, v);
    v = MB32(sa - 0x12);
    MW32(wa + 0x16, v); MW32(wa - 0x1e, v);
    v = MB32(sa - 0xe);
    MW32(wa + 0x22, v); MW32(wa - 0x12, v);
    ti = ta + (uint32_t)MBS16(sa - 0x10) * 8u;
    MW16(wa - 0x62, MB16(ti));
    MW16(wa - 0x60, MB16(ti + 2));
    MW16(wa - 0x5e, MB16(ti + 4));
    ti = tb + (uint32_t)MBS16(sa - 0xc) * 8u;
    MW32(dw, MB32(ti)); MW32(dw + 4, MB32(ti + 4));
    ti = ta + (uint32_t)MBS16(sa - 0xa) * 8u;
    MW16(wa - 0x5a, MB16(ti));
    MW16(wa - 0x58, MB16(ti + 2));
    MW16(wa - 0x56, MB16(ti + 4));
    ti = tb + (uint32_t)MBS16(sa - 8) * 8u;
    MW32(wa - 0x7a, MB32(ti)); MW32(wa - 0x76, MB32(ti + 4));
    ti = ta + (uint32_t)MBS16(sa - 6) * 8u;
    MW16(wa - 0x52, MB16(ti));
    MW16(wa - 0x50, MB16(ti + 2));
    MW16(wa - 0x4e, MB16(ti + 4));
    ti = tb + (uint32_t)MBS16(sa - 4) * 8u;
    MW32(wa - 0x72, MB32(ti)); MW32(wa - 0x6e, MB32(ti + 4));
    ti = ta + (uint32_t)MBS16(sa - 2) * 8u;
    MW16(wa - 0x4a, MB16(ti));
    MW16(wa - 0x48, MB16(ti + 2));
    MW16(wa - 0x46, MB16(ti + 4));
    ti = tb + (uint32_t)MBS16(sa + 0) * 8u;
    MW32(wa - 0x6a, MB32(ti)); MW32(wa - 0x66, MB32(ti + 4));
    MW16(wa - 0x28, MB16(wa - 0x28) + dx);
    MW16(wa - 0x34, MB16(wa - 0x34) + dy);
    MW16(wa + 0xc, MB16(wa + 0xc) + dx);
    MW16(wa + 0, MB16(wa + 0) + dy);
    wa += 0xa8; sa += 0x1c; dw += 0xa8; srw += 0x1c;
  }
  return dw;
}

/* ======== batch60 ======== */

/* gauge_scale_fixed — fixed-point gauge scaler @0x80156048 (46
 * insns; candidate 0x8015607c is a false mid-function entry —
 * this is the real prologue). sx16 args, magic-multiply divide
 * by ~3200/6400 (0x51eb851f mulhi >>5 - sign), clamp a1<=0x3fff,
 * v1<=0x7fff, then fill struct 0x801f2690 {u32 0x2c3, u16 x4,
 * u32 1 @+0x18} and cd_chan_attr_apply(0x801f2690). */
void gauge_scale_fixed(int16_t gw,int16_t gh)
{
  int32_t x = (int32_t)(int16_t)gw;
  int32_t y = (int32_t)(int16_t)gh;
  int32_t p, hi;
  p = x * 0x3fff;
  hi = (int32_t)(((int64_t)p * (int64_t)(int32_t)0x51eb851f) >> 32);
  x = (hi >> 5) - (p >> 31);
  p = y * 0x7fff;
  hi = (int32_t)(((int64_t)p * (int64_t)(int32_t)0x51eb851f) >> 32);
  p = (hi >> 5) - (p >> 31);
  y = x;
  if (y >= 0x4000) y = 0x3fff;
  if (p > 0x7fff) p = 0x7fff;
  MW32(0x801f2690u, 0x2c3u);
  MW16(0x801f2690u + 6, (uint16_t)y);
  MW16(0x801f2690u + 4, (uint16_t)y);
  MW16(0x801f2690u + 0x12, (uint16_t)p);
  MW16(0x801f2690u + 0x10, (uint16_t)p);
  MW32(0x801f2690u + 0x18, 1u);
  cd_chan_attr_apply(0x801f2690u);
}

/* stage_char_data_load — stage/character data loader @0x80129218 (172
 * insns, 2 args). K = a1*23140. Bulk-copies RO data tables into
 * work buffers (all lengths exact multiples of 16 plus memcpy
 * tails — lwl/lwr align fixup collapses to blk_copy):
 * 0x80200000+K -> 0x801c15f8 len 0x2da8; 0x80202da8+K ->
 * 0x801c47c0 len 0x235c; 0x80205104+K -> 0x801cf88c len 0x960;
 * MW16(*MB32(a0+0xcc),3); then s0+0x18..+0x21d8 -> 0x801e1b3c
 * (0x21c0), s0+0x21d8..+0x2da8 -> 0x801e0f6c (0xbd0).
 * victory_record_restore(a0,0); s0+6 = ret-1; copies s0+0x98/0x14/0x16 to
 * a0+0x98/0x99/0xa0; s0+6 <0 -> 0x257. */
void stage_char_data_load(uint32_t rec,uint32_t idx)
{
  uint32_t K = idx * 23140u;
  {
    decomp_trace_record(46,rec,idx,K,0); }
  uint32_t blk = MB32(rec + 0xcc);
  uint32_t ret;
  blk_copy(0x801c15f8u, 0x80200000u + K, 0x2da8u);
  blk_copy(0x801c47c0u, 0x80202da8u + K, 0x235cu);
  blk_copy(0x801cf88cu, 0x80205104u + K, 0x960u);
  MW16(blk, 3u);
  blk_copy(0x801e1b3cu, blk + 0x18u, 0x21c0u);
  blk_copy(0x801e0f6cu, blk + 0x21d8u, 0xbd0u);
  ret = (uint32_t)(victory_record_restore(rec, 0) - 1);
  MW16(blk + 6, (uint16_t)ret);
  MW8(rec + 0x98, MB8(blk + 0x98));
  MW8(rec + 0x99, MB8(blk + 0x14));
  MW16(rec + 0xa0, MB16(blk + 0x16));
  if (MBS16(blk + 6) < 0) MW16(blk + 6, 0x257u);
}

/* ======== batch61 ======== */

/* event_record_walk — event record walker/dispatcher @0x8014afc0 (192
 * insns, 1 arg). s0 = param_1.
 * Phase1: if MBS16(s0+0x18)!=0 -> s0+0x18 = MB16-1, goto emit.
 * Else if MB16(0x1f800082)==0 -> s0+0x14 = MB16-1. If
 * MBS16(s0+0x14)>0 -> emit. Else advance MB32(s0+0x10) by 4 and
 * scan records: hi = MB16(ptr)&0xff00, lo = &0xff.
 *  hi==0xfd00 or (0xfc00 && lo>=cnt++ path) -> ptr =
 *    MB32(s0+8) + MBS16(ptr+2)*4;
 *  hi==0xfe00 -> ptr+=4, s0+0x18 = MB16(ptr_old+2);
 *  hi==0xfc00 && lo<MB16(s0+0x16) -> ptr+=4;
 *  hi==0xfc00 && lo>=cnt -> cnt++, ptr = table[MBS16(ptr+2)];
 *  hi==0xff00 && lo>=cnt -> task_slot_step, s0+0x14=0, ptr-=4, emit;
 *  hi==0xff00 && lo<cnt -> ptr+=4;
 *  loop while MB16(ptr)&0xff00 != 0. On terminator: s0+0x1a =
 *  s0+0x14 = MB16(ptr+2); s0+0x1c = (lh(ptr+2)<=0 ? lh : 0).
 * Phase2 (emit): copy 6 u16 from MB32(s0+0xc)+lh(ptr)*12 to
 * scratch buf; a3 = lh(s0+0x14)<<12 / lh(s0+0x1a) (or 0x1000 -
 * that when s0+0x1c != 0); gte_xform_accum(buf, buf+8, 1, a3); then
 * buf bytes 0/2/4 -> MB32(s0)+4/5/6 and MB32(s0+4)+4/5/6. */
extern void gte_xform_accum(void *,void *,int32_t,uint32_t);
void event_record_walk(uint32_t w)
{
  uint32_t cur, tagw, hi, lo, fpa, fpb, qsrc, quo, scale;
  int32_t den;
  /* emit vec = guest $sp+0x10 (frame -0x30) = entry_sp-0x20; it is filled
   * in the emit phase and re-read by gte_xform_accum through nested
   * frames, so reserve the guest frame window (sp+0x10 -> frm+0x10) for
   * the function's lifetime. */
  uint32_t erw_frm = br2_guest_frame(0x30);
  uint32_t gbuf = erw_frm + 0x10;
  uint8_t *buf = (uint8_t *)guest_at(gbuf);
  uint32_t h0, h1;
  if (MBS16(w + 0x18) != 0) {
    MW16(w + 0x18, MB16(w + 0x18) - 1u);
    goto emit;
  }
  if (MBS16(0x1f800082u) == 0)
    MW16(w + 0x14, MB16(w + 0x14) - 1u);
  if (MBS16(w + 0x14) > 0) goto emit;
  cur = MB32(w + 0x10) + 4u;
  MW32(w + 0x10, cur);
  if ((MB16(cur) & 0xff00u) == 0) goto term;
  for (;;) {
    tagw = MB16(cur);
    hi = tagw & 0xff00u;
    lo = tagw & 0xffu;
    if (hi == 0xfd00u) {
      cur = MB32(w + 8) + (uint32_t)MBS16(cur + 2) * 4u;
      MW32(w + 0x10, cur);
    } else if (hi > 0xfd00u) {
      if (hi == 0xfe00u) {
        MW32(w + 0x10, cur + 4u);
        MW16(w + 0x18, MB16(cur + 2));
      } else if (hi == 0xff00u) {
        if ((int32_t)lo < MBS16(w + 0x16)) {
          MW32(w + 0x10, cur + 4u);
        } else {
          task_slot_step();
          MW16(w + 0x14, 0);
          MW32(w + 0x10, cur - 4u);
          goto emit;
        }
      }
    } else if (hi == 0xfc00u) {
      if ((int32_t)lo < MB16(w + 0x16)) {
        MW32(w + 0x10, cur + 4u);
      } else {
        MW16(w + 0x16, MB16(w + 0x16) + 1u);
        cur = MB32(w + 8) + (uint32_t)MBS16(cur + 2) * 4u;
        MW32(w + 0x10, cur);
      }
    } else {
      /* default: keep current ptr */
    }
    cur = MB32(w + 0x10);
    if ((MB16(cur) & 0xff00u) == 0) break;
  }
term:
  fpa = MB32(w + 0x10);
  fpb = MB32(w + 0x10);
  MW16(w + 0x1a, MB16(fpa + 2));
  MW16(w + 0x14, MB16(fpb + 2));
  {
    int32_t t = MBS16(fpb + 2);
    MW16(w + 0x1c, t <= 0 ? (uint16_t)t : 0);
  }
emit:
  cur = MB32(w + 0x10);
  h0 = MB32(w);
  h1 = MB32(w + 4);
  qsrc = MB32(w + 0xc) + (uint32_t)MBS16(cur) * 12u;
  MW16(gbuf + 0, MB16(qsrc));
  MW16(gbuf + 2, MB16(qsrc + 2));
  MW16(gbuf + 4, MB16(qsrc + 4));
  MW16(gbuf + 6, MB16(qsrc + 6));
  MW16(gbuf + 8, MB16(qsrc + 8));
  MW16(gbuf + 10, MB16(qsrc + 0xa));
  /* guest 0x8014B208/0x8014B228: lh/lh/sll12/div unconditional —
   * MIPS div-by-zero LO = num>=0 ? 0xFFFFFFFF : 1 (mflo raw u32). */
  {
    int32_t num = MBS16(w + 0x14) << 12;
    if (MBS16(w + 0x1c) == 0) {
      den = MBS16(w + 0x1a);
      scale = den != 0 ? (uint32_t)(num / den)
                    : (num >= 0 ? 0xFFFFFFFFu : 1u);
    } else {
      den = MBS16(w + 0x1a);
      quo = den != 0 ? (uint32_t)(num / den)
                    : (num >= 0 ? 0xFFFFFFFFu : 1u);
      scale = 0x1000u - quo;
    }
  }
  gte_xform_accum(buf, buf + 8, 1, scale);
  MW8(h0 + 4, buf[0]);
  MW8(h0 + 5, buf[2]);
  MW8(h0 + 6, buf[4]);
  MW8(h1 + 4, buf[0]);
  MW8(h1 + 5, buf[2]);
  MW8(h1 + 6, buf[4]);
  br2_guest_frame_free();
}

/* ======== batch62 ======== */

extern void gpu_pkt_unlink_n(uint32_t,int32_t);
extern uint32_t gpu_stat_sign(void);
extern uint32_t gpu_graph_reset(uint32_t);
extern void gpu_clear_image_cmd(uint32_t,uint32_t,uint32_t,uint32_t);
/* system_tick_frame — system tick @0x8012aae4 (196 insns, no args).
 * s2 = 0x1f800000, s1 = MB32(+8), s4 = MB32(+4). Timestamp delta
 * -> MB16(0x1f80001e). If MB8(+0x13): optional gpu_pkt_unlink_n when
 * MB32(0x801e8500)==0; spin gpu_stat_sign until result !=
 * MB32(0x801e8500) or (cd_timer_read(1)-s3)&0xffff >= 0xc, store
 * result; then gpu_graph_reset(1), draw_env_timing(s4+0x5c),
 * gpu_draw_env_put(s4). Rotate MB32(0x1f8000e4)->0x1f8000e0, clear
 * e4; cd_timer_read(1) ret -> MB16(0x801e83ae). For i = sx16
 * MB16(0x1f8000d0) down to 1: gpu_load_image_cmd(0x801c4530+i*8,
 * MB32(0x801bef38+(i-1)*4)). If MB8(+0x12): clear +0xd0,
 * gpu_disp_mask_set(byte-1), clear +0x12. If MB8(+0x13):
 * MB32(0x801e83b4)!=0 -> char slot idx = MB32(+0xc): if
 * MBS16(0x801f0660+idx*104+0xe)>0 && MB8(+0x9e)==0 ->
 * MW16(t0+0xa, MB16(0x801e40a0+idx*8528+2)),
 * gpu_clear_image_cmd(t0+8, MB8(t0+4), MB8(t0+5), MB8(t0+6));
 * gpu_draw_otag(s4+0x20ec), clear flag. Else same inner with
 * idx^1 + gpu_draw_otag(s4==0x801e40a0 ? 0x801e82dc : 0x801e618c),
 * MB16(0x801e83a8)++. If MB8(+0x13)==0 and flag set -> clear. */
void system_tick_frame(void)
{
  uint32_t io = 0x1f800000u;
  uint32_t ot = MB32(io + 8);
  uint32_t env = MB32(io + 4);
  uint32_t ts = cd_timer_read(1);
  uint32_t stat, slot, idx, env_sv;
  int32_t i;
  MW16(io + 0x1e, (uint16_t)(ts - MB16(0x801e83acu)));
  if (MB8(io + 0x13) != 0) {
    if (MB8(io + 0x15) != 0) {
        uint32_t elapsed;
      if (MB32(0x801e8500u) == 0)
        gpu_pkt_unlink_n(ot + 0x207c, 0x81f);
      for (;;) {
        stat = gpu_stat_sign();
        if (MB32(0x801e8500u) != stat) break;
        elapsed = (cd_timer_read(1) - ts) & 0xffffu;
        if (elapsed >= 0xc) break;
      }
      MW32(0x801e8500u, stat);
    }
    if (MB8(io + 0x13) != 0) {
      gpu_graph_reset(1);
      draw_env_timing(env + 0x5c);
      gpu_draw_env_put(env);
    }
  }
  env_sv = MB32(io + 0xe4);
  MW32(io + 0xe4, 0);
  MW32(io + 0xe0, env_sv);
  MW16(0x801e83aeu, (uint16_t)cd_timer_read(1));
  i = (int32_t)(int16_t)MB16(io + 0xd0);
  while (i != 0) {
    gpu_load_image_cmd(0x801c4530u + (uint32_t)i * 8u,
                 MB32(0x801bef38u + (uint32_t)(i - 1) * 4u));
    i--;
  }
  if (MB8(io + 0x12) != 0) {
    MW16(io + 0xd0, 0);
    gpu_disp_mask_set(MB8(io + 0x12) - 1);
    MW8(io + 0x12, 0);
  }
  if (MB8(io + 0x13) != 0) {
    if (MB32(0x801e83b4u) != 0) {
      idx = MB32(io + 0xc);
      slot = 0x801f0660u + idx * 104u;
      if (MBS16(slot + 0xe) > 0 && MB8(io + 0x9e) == 0) {
        uint32_t src_rec = 0x801e40a0u + idx * 8528u;
        MW16(slot + 0xa, MB16(src_rec + 2));
        gpu_clear_image_cmd(slot + 8, MB8(slot + 4), MB8(slot + 5), MB8(slot + 6));
      }
      gpu_draw_otag(env + 0x20ecu);
      MW32(0x801e83b4u, 0);
    } else {
      idx = MB32(io + 0xc) ^ 1u;
      slot = 0x801f0660u + idx * 104u;
      if (MBS16(slot + 0xe) > 0 && MB8(io + 0x9e) == 0) {
        uint32_t src_rec = 0x801e40a0u + idx * 8528u;
        MW16(slot + 0xa, MB16(src_rec + 2));
        gpu_clear_image_cmd(slot + 8, MB8(slot + 4), MB8(slot + 5), MB8(slot + 6));
      }
      gpu_draw_otag(env == 0x801e40a0u ? 0x801e82dcu : 0x801e618cu);
      MW16(0x801e83a8u, MB16(0x801e83a8u) + 1u);
    }
  } else if (MB32(0x801e83b4u) != 0) {
    MW32(0x801e83b4u, 0);
  }
}

/* ======== batch63 ======== */

/* desc_cmd_map — descriptor command mapper @0x8017e138 (204
 * insns, 3 args; always returns a2+6). t0 = sx8 of desc high
 * byte; mode = (t0>>4)&3. mode1/2 gate on MB8(a0+5)&1 (mode1:
 * set -> ret; mode2: clear -> ret; else continue).
 * src byte pair: flag40 ? MB8(MB32(a0+0x2c)+2/3) : MB8(a0+2/3);
 * subop = t0&0xf.
 * subop==0: quo = MBS16(s0+4)<<4 / MBS16(a0+0x88); deferred_cmd_post(0x400,
 *   lh(s0+2) + (lh>=0xc0 && srcB!=0 ? 0x40 : 0), quo).
 * subop==1: cmdarg = lh(s0+2), tbl_i = srcB. If MB8(a0+5)&1==0: cmdarg==0xfe ->
 *   T_fe[tbl_i<0xb]; cmdarg==0xff -> T_ff[tbl_i<0xb]; cmdarg==0xfd -> T_fd[tbl_i<0xb];
 *   else keep cmdarg. If bit set: compare chain -> 0xc3/0xc4/keep.
 *   Tables (tbl_i index 0..10):
 *   ff: d3 cf cd cd ce d0 cd cb d2 d2 cc
 *   fe: d4 d0 ce ce cf d1 ce cc d3 d3 cd
 *   fd: d5 d1 cf cf d0 d2 cf cd d0 cc ce
 * Store (a2>=0): flag40==0 -> MB16(a1+0x10e)=a2,
 *   MB16(a1+0x110)=lh(s0+4)<<4; else same into
 *   MB32(MB32(a0+0x2c)+0x38)+0x10e/0x110. */
uint32_t desc_cmd_map(uint32_t obj,uint32_t st,
                      uint32_t desc)
{
  int32_t op = (int32_t)(int8_t)(MB16(desc) >> 8);
  int32_t mode = (op >> 4) & 3;
  int32_t flag40, subop, cmdarg, tbl_i, val, quo, den;
  uint32_t sub;
  static const uint8_t t_ff[11] = {0xd3,0xcf,0xcd,0xcd,0xce,0xd0,0xcd,0xcb,0xd2,0xd2,0xcc};
  static const uint8_t t_fe[11] = {0xd4,0xd0,0xce,0xce,0xcf,0xd1,0xce,0xcc,0xd3,0xd3,0xcd};
  static const uint8_t t_fd[11] = {0xd5,0xd1,0xcf,0xcf,0xd0,0xd2,0xcf,0xcd,0xd0,0xcc,0xce};
  if (mode == 1) {
    if (MB8(obj + 5) & 1) return desc + 6;
  } else if (mode == 2) {
    if (!(MB8(obj + 5) & 1)) return desc + 6;
  }
  flag40 = op & 0x40;
  if (flag40) {
    sub = MB32(obj + 0x2c);
    val = MB8(sub + 2);
    tbl_i = MB8(sub + 3);
  } else {
    val = MB8(obj + 2);
    tbl_i = MB8(obj + 3);
  }
  subop = op & 0xf;
  if (subop == 0) {
    int32_t c = MBS16(desc + 2);
    int32_t n4 = MBS16(desc + 4) << 4;
    den = MBS16(obj + 0x88);
    /* guest 0x8017E22C/0x8017E254: lh/lh/sll4/div unconditional —
     * MIPS div-by-zero LO = num>=0 ? -1 : 1. */
    quo = den != 0 ? n4 / den : (n4 >= 0 ? -1 : 1);
    deferred_cmd_post(0x400, c + (c >= 0xc0 && val != 0 ? 0x40 : 0), quo);
    return desc + 6;
  }
  if (subop != 1) return desc + 6;
  cmdarg = MBS16(desc + 2);
  val = cmdarg;
  if (MB8(obj + 5) & 1) {
    /* bit set: compare chain */
    if (cmdarg == 6) val = 0xc3;
    else if (cmdarg < 7) {
      if (cmdarg == 2) val = 0xc3;
      else if (cmdarg < 3) {
        if (cmdarg == 0) val = 0xc4;
        else if (cmdarg == (op & 0xf)) val = 0xc3;
        /* else keep cmdarg */
      } else val = 0xc3;
    } else if (cmdarg == 9) val = 0xc3;
    else if (cmdarg < 0xa) val = 0xc3;      /* 7,8 */
    else if (cmdarg == 0xa) val = 0xc3;
    else if (cmdarg < 0x100) {
      if (cmdarg >= 0xfd) val = 0xc4;
      /* else keep cmdarg */
    }
    /* cmdarg >= 0x100 keeps cmdarg */
  } else {
    if (cmdarg == 0xfe) { if ((uint32_t)tbl_i < 0xb) val = t_fe[tbl_i]; }
    else if (cmdarg == 0xff) { if ((uint32_t)tbl_i < 0xb) val = t_ff[tbl_i]; }
    else if (cmdarg == 0xfd) { if ((uint32_t)tbl_i < 0xb) val = t_fd[tbl_i]; }
    /* else keep cmdarg (val already = cmdarg? no — val was src byte!) */
  }
  if (val < 0) return desc + 6;
  if (!flag40) {
    MW16(st + 0x10e, (uint16_t)val);
    MW16(st + 0x110, (uint16_t)(MBS16(desc + 4) << 4));
  } else {
    sub = MB32(MB32(obj + 0x2c) + 0x38);
    MW16(sub + 0x10e, (uint16_t)val);
    MW16(sub + 0x110, (uint16_t)(MBS16(desc + 4) << 4));
  }
  return desc + 6;
}

/* ================================================================== */
/* desc_cmd_interp — descriptor command-stream interpreter @0x8017DB88 */
/* (1456B, 91 blocks). ONE coherent CPS function: the 16 "alias"      */
/* addresses (0x8017DC78..0x8017E118) are interior opcode-handler     */
/* entries reached ONLY through three computed `jr $v0` dispatches    */
/* (jump tables @0x80122A04 / 0x80122A2C / 0x80122A54 in guest RAM).  */
/* No external jal/j targets them (verified vs EXE + all generated).  */
/*                                                                    */
/* Faithful port: the whole body is a single clean C function —      */
/* state lives in cpu->gpr[] exactly like the generated CPS frame.   */
/* The three `jr $v0` sites became an internal switch->goto on the   */
/* resolved table target (handlers share s0-s3 live state, so they  */
/* MUST stay inside this frame; the generated alias bodies would     */
/* need the guest sp-40 frame this port does not create).            */
/* `jal table_byte_next_a`/`jal deferred_cmd_post` became            */
/* synchronous clean calls + goto-resume (identical register image;  */
/* CPS suspend/resume collapses to call+continue). The prologue/     */
/* epilogue guest-stack spill of s0-s3/ra was write-then-read only, */
/* so it lives in C locals — same frame-less convention as every     */
/* clean port ($sp/$ra untouched).                                    */
/*                                                                    */
/* Entry: a0 = command descriptor, a2 = stream record (s3). High     */
/* byte of lh(s3) = signed opcode; (opcode>>4)&3 = mode selects the  */
/* sub-table/field source. Handlers post deferred_cmd_post(0x400,    */
/* op, 0) or store lh(s3+2) into descriptor fields, converge on      */
/* v0 = s3+4 -> epilogue. Body transliterated insn-for-insn from     */
/* generated/SLES_017.22_full_36.c (br2eu_func_8017DB88).            */
/* ================================================================== */

extern uint32_t br2_ld8(uint32_t);
extern uint32_t br2_ld16(uint32_t);
extern uint32_t br2_ld32(uint32_t);
extern void br2_st16(uint32_t, uint16_t);
extern uint32_t call_guest4(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

uint32_t desc_cmd_interp(uint32_t unused1, uint32_t unused2,
                         uint32_t unused3, uint32_t unused4)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t _sv_s0 = cpu->gpr[16], _sv_s1 = cpu->gpr[17];
  uint32_t _sv_s2 = cpu->gpr[18], _sv_s3 = cpu->gpr[19];
  uint32_t jt;
  (void)unused1; (void)unused2; (void)unused3; (void)unused4;

block_8017DB88:
{ cpu->gpr[19] = cpu->gpr[6];
}
{ cpu->gpr[2] = br2_ld16(cpu->gpr[19]);
}
{ cpu->gpr[2] = cpu->gpr[2] << 16;
}
{ cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
}
{ cpu->gpr[2] = cpu->gpr[2] << 16;
}
{ cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 16;
}
{ cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 20;
}
cpu->gpr[6] = cpu->gpr[2] & 0x0003;
{ cpu->gpr[2] = 1;
}
int _bc_8017DBC8 = (cpu->gpr[6] == cpu->gpr[2]);
{ cpu->gpr[7] = cpu->gpr[5];
}
if (_bc_8017DBC8) {
goto block_8017DBF0;
} else {
goto block_8017DBD0;
}
block_8017DBD0:
cpu->gpr[2] = ((int32_t)cpu->gpr[6] < 2) ? 1 : 0;
int _bc_8017DBD4 = (cpu->gpr[2] != cpu->gpr[0]);
cpu->gpr[2] = cpu->gpr[3] & 0x0040;
if (_bc_8017DBD4) {
goto block_8017DC24;
} else {
goto block_8017DBDC;
}
block_8017DBDC:
{ cpu->gpr[2] = 2;
}
int _bc_8017DBE0 = (cpu->gpr[6] == cpu->gpr[2]);
cpu->gpr[2] = cpu->gpr[3] & 0x0040;
if (_bc_8017DBE0) {
goto block_8017DC0C;
} else {
goto block_8017DBE8;
}
block_8017DBE8:
goto block_8017DC24;
block_8017DBF0:
{ cpu->gpr[2] = br2_ld8(cpu->gpr[4] + 5);
}
cpu->gpr[2] = cpu->gpr[2] & 0x0001;
int _bc_8017DBFC = (cpu->gpr[2] == cpu->gpr[0]);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DBFC) {
goto block_8017DC20;
} else {
goto block_8017DC04;
}
block_8017DC04:
goto block_8017E11C;
block_8017DC0C:
{ cpu->gpr[2] = br2_ld8(cpu->gpr[4] + 5);
}
cpu->gpr[2] = cpu->gpr[2] & 0x0001;
int _bc_8017DC18 = (cpu->gpr[2] == cpu->gpr[0]);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DC18) {
goto block_8017E11C;
} else {
goto block_8017DC20;
}
block_8017DC20:
cpu->gpr[2] = cpu->gpr[3] & 0x0040;
block_8017DC24: ;
int _bc_8017DC24 = (cpu->gpr[2] != cpu->gpr[0]);
if (_bc_8017DC24) {
goto block_8017DC3C;
} else {
goto block_8017DC2C;
}
block_8017DC2C:
{ cpu->gpr[18] = br2_ld8(cpu->gpr[4] + 2);
}
{ cpu->gpr[6] = br2_ld8(cpu->gpr[4] + 3);
}
cpu->gpr[3] = cpu->gpr[3] & 0x000F;
goto block_8017DC50;
block_8017DC3C:
{ cpu->gpr[2] = br2_ld32(cpu->gpr[4] + 44);
}
{ cpu->gpr[18] = br2_ld8(cpu->gpr[2] + 2);
}
{ cpu->gpr[6] = br2_ld8(cpu->gpr[2] + 3);
}
cpu->gpr[3] = cpu->gpr[3] & 0x000F;
block_8017DC50:
cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)10) ? 1 : 0;
int _bc_8017DC54 = (cpu->gpr[2] == cpu->gpr[0]);
cpu->gpr[2] = 0x8012 << 16;
if (_bc_8017DC54) {
goto block_8017E118;
} else {
goto block_8017DC5C;
}
block_8017DC5C:
{ cpu->gpr[2] = cpu->gpr[2] + 10756;
}
{ cpu->gpr[3] = cpu->gpr[3] << 2;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
}
{ cpu->gpr[2] = br2_ld32(cpu->gpr[3]);
}
jt = cpu->gpr[2];
goto jt_dispatch;
block_8017DC78:
{ cpu->gpr[5] = (int32_t)(int16_t)br2_ld16(cpu->gpr[19] + 2);
}
cpu->gpr[2] = ((int32_t)cpu->gpr[5] < 192) ? 1 : 0;
int _bc_8017DC84 = (cpu->gpr[2] != cpu->gpr[0]);
{ cpu->gpr[4] = 1024;
}
if (_bc_8017DC84) {
goto block_8017E10C;
} else {
goto block_8017DC8C;
}
block_8017DC8C: ;
int _bc_8017DC8C = (cpu->gpr[18] != cpu->gpr[0]);
{ cpu->gpr[5] = cpu->gpr[5] + 64;
}
if (_bc_8017DC8C) {
goto block_8017E10C;
} else {
goto block_8017DC94;
}
block_8017DC94:
{ cpu->gpr[5] = (int32_t)(int16_t)br2_ld16(cpu->gpr[19] + 2);
}
{ cpu->gpr[6] = cpu->gpr[0];
}
goto block_8017E110;
block_8017DCA0:
{ cpu->gpr[3] = (int32_t)(int16_t)br2_ld16(cpu->gpr[7] + 274);
}
{ cpu->gpr[6] = 64;
}
int _bc_8017DCA8 = (cpu->gpr[3] == cpu->gpr[6]);
cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 65) ? 1 : 0;
if (_bc_8017DCA8) {
goto block_8017DCD0;
} else {
goto block_8017DCB0;
}
block_8017DCB0: ;
int _bc_8017DCB0 = (cpu->gpr[2] == cpu->gpr[0]);
{ cpu->gpr[2] = -1;
}
if (_bc_8017DCB0) {
goto block_8017DD98;
} else {
goto block_8017DCB8;
}
block_8017DCB8: ;
int _bc_8017DCB8 = (cpu->gpr[3] == cpu->gpr[2]);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DCB8) {
goto block_8017E11C;
} else {
goto block_8017DCC0;
}
block_8017DCC0: ;
int _bc_8017DCC0 = (cpu->gpr[3] == cpu->gpr[0]);
if (_bc_8017DCC0) {
goto block_8017DD34;
} else {
goto block_8017DCC8;
}
block_8017DCC8:
goto block_8017DD98;
block_8017DCD0:
{ cpu->gpr[2] = br2_ld32(cpu->gpr[4] + 104);
}
cpu->gpr[3] = cpu->gpr[2] & 0x0340;
{ cpu->gpr[2] = 256;
}
int _bc_8017DCE0 = (cpu->gpr[3] == cpu->gpr[2]);
cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 257) ? 1 : 0;
if (_bc_8017DCE0) {
goto block_8017DD1C;
} else {
goto block_8017DCE8;
}
block_8017DCE8: ;
int _bc_8017DCE8 = (cpu->gpr[2] == cpu->gpr[0]);
{ cpu->gpr[2] = 512;
}
if (_bc_8017DCE8) {
goto block_8017DD00;
} else {
goto block_8017DCF0;
}
block_8017DCF0: ;
int _bc_8017DCF0 = (cpu->gpr[3] == cpu->gpr[6]);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DCF0) {
goto block_8017DD10;
} else {
goto block_8017DCF8;
}
block_8017DCF8:
goto block_8017E11C;
block_8017DD00: ;
int _bc_8017DD00 = (cpu->gpr[3] == cpu->gpr[2]);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DD00) {
goto block_8017DD28;
} else {
goto block_8017DD08;
}
block_8017DD08:
goto block_8017E11C;
block_8017DD10:
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[5] = 64;
}
goto block_8017E10C;
block_8017DD1C:
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[5] = 65;
}
goto block_8017E10C;
block_8017DD28:
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[5] = 66;
}
goto block_8017E10C;
block_8017DD34:
{ cpu->gpr[2] = br2_ld32(cpu->gpr[4] + 104);
}
cpu->gpr[3] = cpu->gpr[2] & 0x0340;
{ cpu->gpr[2] = 256;
}
int _bc_8017DD44 = (cpu->gpr[3] == cpu->gpr[2]);
cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 257) ? 1 : 0;
if (_bc_8017DD44) {
goto block_8017DD80;
} else {
goto block_8017DD4C;
}
block_8017DD4C: ;
int _bc_8017DD4C = (cpu->gpr[2] == cpu->gpr[0]);
{ cpu->gpr[2] = 512;
}
if (_bc_8017DD4C) {
goto block_8017DD64;
} else {
goto block_8017DD54;
}
block_8017DD54: ;
int _bc_8017DD54 = (cpu->gpr[3] == cpu->gpr[6]);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DD54) {
goto block_8017DD74;
} else {
goto block_8017DD5C;
}
block_8017DD5C:
goto block_8017E11C;
block_8017DD64: ;
int _bc_8017DD64 = (cpu->gpr[3] == cpu->gpr[2]);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DD64) {
goto block_8017DD8C;
} else {
goto block_8017DD6C;
}
block_8017DD6C:
goto block_8017E11C;
block_8017DD74:
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[5] = cpu->gpr[0];
}
goto block_8017E10C;
block_8017DD80:
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[5] = 1;
}
goto block_8017E10C;
block_8017DD8C:
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[5] = 2;
}
goto block_8017E10C;
block_8017DD98:
{ cpu->gpr[5] = (int32_t)(int16_t)br2_ld16(cpu->gpr[7] + 274);
}
cpu->gpr[2] = ((int32_t)cpu->gpr[5] < 192) ? 1 : 0;
int _bc_8017DDA4 = (cpu->gpr[2] != cpu->gpr[0]);
{ cpu->gpr[4] = 1024;
}
if (_bc_8017DDA4) {
goto block_8017E10C;
} else {
goto block_8017DDAC;
}
block_8017DDAC: ;
int _bc_8017DDAC = (cpu->gpr[18] != cpu->gpr[0]);
{ cpu->gpr[5] = cpu->gpr[5] + 64;
}
if (_bc_8017DDAC) {
goto block_8017E10C;
} else {
goto block_8017DDB4;
}
block_8017DDB4:
{ cpu->gpr[5] = (int32_t)(int16_t)br2_ld16(cpu->gpr[7] + 274);
}
{ cpu->gpr[6] = cpu->gpr[0];
}
goto block_8017E110;
block_8017DDC0:
{ cpu->gpr[2] = br2_ld32(cpu->gpr[4] + 44);
}
{ cpu->gpr[3] = br2_ld32(cpu->gpr[2] + 56);
}
{ cpu->gpr[2] = br2_ld16(cpu->gpr[19] + 2);
}
{ br2_st16(cpu->gpr[3] + 274, (uint16_t)((uint16_t)cpu->gpr[2]));
}
goto block_8017E118;
block_8017DDD8:
{ cpu->gpr[3] = br2_ld32(cpu->gpr[4] + 56);
}
{ cpu->gpr[2] = br2_ld16(cpu->gpr[19] + 2);
}
{ br2_st16(cpu->gpr[3] + 276, (uint16_t)((uint16_t)cpu->gpr[2]));
}
goto block_8017E118;
block_8017DDE8:
{ cpu->gpr[2] = (int32_t)(int16_t)br2_ld16(cpu->gpr[19] + 2);
}
{ cpu->gpr[3] = br2_ld32(cpu->gpr[4] + 56);
}
{ cpu->gpr[2] = cpu->gpr[2] << 4;
}
{ br2_st16(cpu->gpr[3] + 278, (uint16_t)((uint16_t)cpu->gpr[2]));
}
goto block_8017E118;
block_8017DDFC:
{ cpu->gpr[2] = br2_ld8(cpu->gpr[4] + 5);
}
cpu->gpr[2] = cpu->gpr[2] & 0x0001;
int _bc_8017DE08 = (cpu->gpr[2] != cpu->gpr[0]);
{ cpu->gpr[16] = 87;
}
if (_bc_8017DE08) {
goto block_8017DE50;
} else {
goto block_8017DE10;
}
block_8017DE10:
cpu->gpr[2] = (cpu->gpr[6] < (uint32_t)11) ? 1 : 0;
int _bc_8017DE14 = (cpu->gpr[2] == cpu->gpr[0]);
{ cpu->gpr[16] = -1;
}
if (_bc_8017DE14) {
goto block_8017DE50;
} else {
goto block_8017DE1C;
}
block_8017DE1C:
cpu->gpr[2] = 0x8012 << 16;
{ cpu->gpr[2] = cpu->gpr[2] + 10796;
}
{ cpu->gpr[3] = cpu->gpr[6] << 2;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
}
{ cpu->gpr[2] = br2_ld32(cpu->gpr[3]);
}
jt = cpu->gpr[2];
goto jt_dispatch;
block_8017DE3C:
{ cpu->gpr[16] = 201;
}
goto block_8017DE50;
block_8017DE44:
{ cpu->gpr[16] = 200;
}
goto block_8017DE50;
block_8017DE4C:
{ cpu->gpr[16] = 199;
}
block_8017DE50: ;
int _bc_8017DE50 = ((int32_t)cpu->gpr[16] < 0);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DE50) {
goto block_8017E11C;
} else {
goto block_8017DE58;
}
block_8017DE58:
cpu->gpr[17] = 0x5555 << 16;
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017DE60;
block_8017DE60:
{ cpu->gpr[17] = cpu->gpr[17] | 0x5556;
}
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[17]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[4] = cpu->gpr[8] - cpu->gpr[4];
}
{ cpu->gpr[3] = cpu->gpr[4] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
}
cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 192) ? 1 : 0;
int _bc_8017DE88 = (cpu->gpr[2] != cpu->gpr[0]);
if (_bc_8017DE88) {
goto block_8017DED0;
} else {
goto block_8017DE90;
}
block_8017DE90: ;
int _bc_8017DE90 = (cpu->gpr[18] == cpu->gpr[0]);
if (_bc_8017DE90) {
goto block_8017DED0;
} else {
goto block_8017DE98;
}
block_8017DE98:
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017DEA0;
block_8017DEA0:
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[17]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[6] = cpu->gpr[0];
}
{ cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[5] = cpu->gpr[8] - cpu->gpr[5];
}
{ cpu->gpr[3] = cpu->gpr[5] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[5];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
}
{ cpu->gpr[5] = cpu->gpr[2] + 64;
}
goto block_8017E110;
block_8017DED0:
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017DED8;
block_8017DED8:
cpu->gpr[3] = 0x5555 << 16;
{ cpu->gpr[3] = cpu->gpr[3] | 0x5556;
}
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[3]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[6] = cpu->gpr[0];
}
{ cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[5] = cpu->gpr[8] - cpu->gpr[5];
}
{ cpu->gpr[3] = cpu->gpr[5] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[5];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[5] = cpu->gpr[16] + cpu->gpr[2];
}
goto block_8017E110;
block_8017DF0C:
{ cpu->gpr[2] = (int32_t)(int16_t)br2_ld16(cpu->gpr[19] + 2);
}
int _bc_8017DF14 = (cpu->gpr[2] != cpu->gpr[0]);
{ cpu->gpr[4] = 1024;
}
if (_bc_8017DF14) {
goto block_8017DF24;
} else {
goto block_8017DF1C;
}
block_8017DF1C:
{ cpu->gpr[5] = 128;
}
goto block_8017E10C;
block_8017DF24:
{ cpu->gpr[5] = 129;
}
goto block_8017E10C;
block_8017DF2C:
{ cpu->gpr[2] = br2_ld8(cpu->gpr[4] + 5);
}
cpu->gpr[2] = cpu->gpr[2] & 0x0001;
int _bc_8017DF38 = (cpu->gpr[2] != cpu->gpr[0]);
{ cpu->gpr[16] = 87;
}
if (_bc_8017DF38) {
goto block_8017DF80;
} else {
goto block_8017DF40;
}
block_8017DF40:
cpu->gpr[2] = (cpu->gpr[6] < (uint32_t)11) ? 1 : 0;
int _bc_8017DF44 = (cpu->gpr[2] == cpu->gpr[0]);
{ cpu->gpr[16] = -1;
}
if (_bc_8017DF44) {
goto block_8017DF80;
} else {
goto block_8017DF4C;
}
block_8017DF4C:
cpu->gpr[2] = 0x8012 << 16;
{ cpu->gpr[2] = cpu->gpr[2] + 10844;
}
{ cpu->gpr[3] = cpu->gpr[6] << 2;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
}
{ cpu->gpr[2] = br2_ld32(cpu->gpr[3]);
}
jt = cpu->gpr[2];
goto jt_dispatch;
block_8017DF6C:
{ cpu->gpr[16] = 201;
}
goto block_8017DF80;
block_8017DF74:
{ cpu->gpr[16] = 200;
}
goto block_8017DF80;
block_8017DF7C:
{ cpu->gpr[16] = 199;
}
block_8017DF80: ;
int _bc_8017DF80 = ((int32_t)cpu->gpr[16] < 0);
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
if (_bc_8017DF80) {
goto block_8017E11C;
} else {
goto block_8017DF88;
}
block_8017DF88:
cpu->gpr[17] = 0x5555 << 16;
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017DF90;
block_8017DF90:
{ cpu->gpr[17] = cpu->gpr[17] | 0x5556;
}
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[17]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[4] = cpu->gpr[8] - cpu->gpr[4];
}
{ cpu->gpr[3] = cpu->gpr[4] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
}
cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 192) ? 1 : 0;
int _bc_8017DFB8 = (cpu->gpr[2] != cpu->gpr[0]);
if (_bc_8017DFB8) {
goto block_8017E000;
} else {
goto block_8017DFC0;
}
block_8017DFC0: ;
int _bc_8017DFC0 = (cpu->gpr[18] == cpu->gpr[0]);
if (_bc_8017DFC0) {
goto block_8017E000;
} else {
goto block_8017DFC8;
}
block_8017DFC8:
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017DFD0;
block_8017DFD0:
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[17]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[6] = cpu->gpr[0];
}
{ cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[5] = cpu->gpr[8] - cpu->gpr[5];
}
{ cpu->gpr[3] = cpu->gpr[5] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[5];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
}
{ cpu->gpr[5] = cpu->gpr[2] + 64;
}
goto block_8017E038;
block_8017E000:
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017E008;
block_8017E008:
cpu->gpr[3] = 0x5555 << 16;
{ cpu->gpr[3] = cpu->gpr[3] | 0x5556;
}
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[3]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[6] = cpu->gpr[0];
}
{ cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[5] = cpu->gpr[8] - cpu->gpr[5];
}
{ cpu->gpr[3] = cpu->gpr[5] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[5];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[5] = cpu->gpr[16] + cpu->gpr[2];
}
block_8017E038:
cpu->gpr[17] = 0x5555 << 16;
deferred_cmd_post(cpu->gpr[4], (int)cpu->gpr[5], (short)cpu->gpr[6]);
goto block_8017E040;
block_8017E040:
{ cpu->gpr[17] = cpu->gpr[17] | 0x5556;
}
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017E048;
block_8017E048:
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[17]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[4] = cpu->gpr[8] - cpu->gpr[4];
}
{ cpu->gpr[3] = cpu->gpr[4] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
}
cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 192) ? 1 : 0;
int _bc_8017E06C = (cpu->gpr[2] != cpu->gpr[0]);
if (_bc_8017E06C) {
goto block_8017E0B4;
} else {
goto block_8017E074;
}
block_8017E074: ;
int _bc_8017E074 = (cpu->gpr[18] == cpu->gpr[0]);
if (_bc_8017E074) {
goto block_8017E0B4;
} else {
goto block_8017E07C;
}
block_8017E07C:
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017E084;
block_8017E084:
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[17]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[6] = 4;
}
{ cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[5] = cpu->gpr[8] - cpu->gpr[5];
}
{ cpu->gpr[3] = cpu->gpr[5] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[5];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];
}
{ cpu->gpr[5] = cpu->gpr[2] + 64;
}
goto block_8017E110;
block_8017E0B4:
cpu->gpr[2] = (uint32_t)(uint8_t)table_byte_next_a();
goto block_8017E0BC;
block_8017E0BC:
cpu->gpr[3] = 0x5555 << 16;
{ cpu->gpr[3] = cpu->gpr[3] | 0x5556;
}
{ int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[3]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
{ cpu->gpr[4] = 1024;
}
{ cpu->gpr[6] = 4;
}
{ cpu->gpr[5] = (int32_t)cpu->gpr[2] >> 31;
}
cpu->gpr[8] = cpu->hi;
{ cpu->gpr[5] = cpu->gpr[8] - cpu->gpr[5];
}
{ cpu->gpr[3] = cpu->gpr[5] << 1;
}
{ cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[5];
}
{ cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[3];
}
{ cpu->gpr[5] = cpu->gpr[16] + cpu->gpr[2];
}
goto block_8017E110;
block_8017E0F0:
{ cpu->gpr[2] = (int32_t)(int16_t)br2_ld16(cpu->gpr[19] + 2);
}
int _bc_8017E0F8 = (cpu->gpr[2] != cpu->gpr[0]);
{ cpu->gpr[4] = 1024;
}
if (_bc_8017E0F8) {
goto block_8017E108;
} else {
goto block_8017E100;
}
block_8017E100:
{ cpu->gpr[5] = 72;
}
goto block_8017E10C;
block_8017E108:
{ cpu->gpr[5] = 73;
}
block_8017E10C:
{ cpu->gpr[6] = cpu->gpr[0];
}
block_8017E110:
deferred_cmd_post(cpu->gpr[4], (int)cpu->gpr[5], (short)cpu->gpr[6]);
goto block_8017E118;
block_8017E118:
{ cpu->gpr[2] = cpu->gpr[19] + 4;
}
block_8017E11C:
/* epilogue: guest reloaded s0-s3/ra from its sp-40 frame and jr $ra.
 * The frame was write-then-read only, so restore the C-local saves;
 * $ra/$sp were never clobbered (frame-less convention). */
cpu->gpr[16] = _sv_s0; cpu->gpr[17] = _sv_s1;
cpu->gpr[18] = _sv_s2; cpu->gpr[19] = _sv_s3;
return cpu->gpr[2];

jt_dispatch:
/* `jr $v0` through the three guest jump tables (@0x80122A04 /
 * 0x80122A2C / 0x80122A54 relative to the descriptor base). Every
 * valid table entry is an interior handler label of THIS function
 * (verified vs EXE). Anything else is an external guest fn ptr —
 * tail-dispatch with live a0-a3 exactly like guest `jr $v0`. */
switch (jt) {
  case 0x8017DC78u: goto block_8017DC78;
  case 0x8017DCA0u: goto block_8017DCA0;
  case 0x8017DDC0u: goto block_8017DDC0;
  case 0x8017DDD8u: goto block_8017DDD8;
  case 0x8017DDE8u: goto block_8017DDE8;
  case 0x8017DDFCu: goto block_8017DDFC;
  case 0x8017DE3Cu: goto block_8017DE3C;
  case 0x8017DE44u: goto block_8017DE44;
  case 0x8017DE4Cu: goto block_8017DE4C;
  case 0x8017DF0Cu: goto block_8017DF0C;
  case 0x8017DF2Cu: goto block_8017DF2C;
  case 0x8017DF6Cu: goto block_8017DF6C;
  case 0x8017DF74u: goto block_8017DF74;
  case 0x8017DF7Cu: goto block_8017DF7C;
  case 0x8017E0F0u: goto block_8017E0F0;
  case 0x8017E118u: goto block_8017E118;
  default:
    cpu->gpr[16] = _sv_s0; cpu->gpr[17] = _sv_s1;
    cpu->gpr[18] = _sv_s2; cpu->gpr[19] = _sv_s3;
    return call_guest4(jt, cpu->gpr[4], cpu->gpr[5],
                       cpu->gpr[6], cpu->gpr[7]);
}
}

/* ======== batch64 ======== */

/* memmap_debug_dump — memory-map debug dump @0x8012768c (204 insns,
 * no args). Sequence of printf_forward(fmt,...) printf-style
 * registrations for scratch/replay/etc work sizes; args tracked
 * through delay slots. Tail copies 0x410 bytes 0x801c11e4 ->
 * dead stack buf (unobservable, skipped) then
 * printf_forward(str,0x414,0x414,MB32(0x801c11e0)). */
void memmap_debug_dump(void)
{
  printf_forward((const char *)guest_at(0x8011e858u),0,0,0);
  printf_forward((const char *)guest_at(0x8011e870u),0x1f80038bu,0,0);
  printf_forward((const char *)guest_at(0x8011e884u),0x38c,0,0);
  printf_forward((const char *)guest_at(0x8011e898u),0x38c,0,0);
  printf_forward((const char *)guest_at(0x8011e8b0u),0x50,0x10e0,0);
  printf_forward((const char *)guest_at(0x8011e8c8u),0x1c,0x5e8,0);
  printf_forward((const char *)guest_at(0x8011e8e0u),0x108,0x210,0);
  printf_forward((const char *)guest_at(0x8011e8f8u),0x118,0x230,0);
  printf_forward((const char *)guest_at(0x8011e910u),0x24,0x90,0);
  printf_forward((const char *)guest_at(0x8011e928u),4,0x40,0);
  printf_forward((const char *)guest_at(0x8011e940u),0x194,0x194,0);
  printf_forward((const char *)guest_at(0x8011e958u),0x84,0x108,0);
  printf_forward((const char *)guest_at(0x8011e970u),8,0x200,0);
  printf_forward((const char *)guest_at(0x8011e988u),0xa,0x2d0,0);
  printf_forward((const char *)guest_at(0x8011e9a0u),8,8,0);
  printf_forward((const char *)guest_at(0x8011e9b8u),0x235c,0x235c,0);
  printf_forward((const char *)guest_at(0x8011e9d0u),0x235c,0x235c,0);
  printf_forward((const char *)guest_at(0x8011e9e8u),0x90,0x90,0);
  printf_forward((const char *)guest_at(0x8011ea00u),0x48,0x48,0);
  printf_forward((const char *)guest_at(0x8011ea18u),0x48,0x48,0);
  printf_forward((const char *)guest_at(0x8011ea34u),0x235c,0xb0cc,0x801c47c0u);
  printf_forward((const char *)guest_at(0x8011ea50u),2,0x960,0x801cf88cu);
  printf_forward((const char *)guest_at(0x8011ea6cu),0x10000,0x10000,0x801d01ecu);
  printf_forward((const char *)guest_at(0x8011ea88u),4,0,0x801e01ecu);
  printf_forward((const char *)guest_at(0x8011eaa4u),0x1ba2c,0x1ba2c,0x801e01ecu);
  printf_forward((const char *)guest_at(0x8011eabcu),0x20,0x6c0,0x801e01ecu);
  printf_forward((const char *)guest_at(0x8011ead8u),0x20,0x6c0,0x801e08acu);
  printf_forward((const char *)guest_at(0x8011eaf4u),0x1c,0xbd0,0x801e0f6cu);
  printf_forward((const char *)guest_at(0x8011eb10u),0x50,0x21c0,0x801e1b3cu);
  printf_forward((const char *)guest_at(0x8011eaa4u),0x1f53c,0x1f53c,0x801e1b3cu);
  printf_forward((const char *)guest_at(0x8011eb2cu),0x1f53c,0x1f53c,0x801e1b3cu);
  printf_forward((const char *)guest_at(0x8011eb4cu),0x5a64,0x3e24c,0x80200000u);
  printf_forward((const char *)guest_at(0x8011eb68u),0x5a64,0x3e24c,0x80200000u);
  printf_forward((const char *)guest_at(0x8011eb80u),0x414,0x414,
               MB32(0x801c11e0u));
}

/* ======== batch65 ======== */


/* rec16_fill — 16-byte record fill @0x80137578 (6 insns):
 * a0 = guest ptr; {a1,0,a2,0} u32s + a3 u16 at +0x10 (delay slot). */
void rec16_fill(uint32_t dst,uint32_t f0,
                  uint32_t f8,uint32_t f10)
{
  uint8_t *p = (uint8_t *)guest_at(dst);
  *(uint32_t *)(p + 0)  = f0;
  *(uint32_t *)(p + 4)  = 0;
  *(uint32_t *)(p + 8)  = f8;
  *(uint32_t *)(p + 12) = 0;
  *(uint16_t *)(p + 16) = (uint16_t)f10;
}

/* anim_workblock_init — anim work-block init @0x801401d0 (207 insns,
 * 5 args + live-in v0 gate: v0!=0 -> ret). blk =
 * task_node_alloc(1,2,0xc,0x8013f828,0,0x978); fail ->
 * task_node_free(0). s0 = MB32(blk+0x10); a1 = max(arg5>>16,2->0xc
 * if <2); fills header fields, palette ptr MB32(s5+0x20)+s1*80
 * +0x20 -> +0x93c. s1 = (s1==7||s1==0xa) ? 0x801b1ef4 :
 * MB8(s5+3)*48+0x801b1f24. If arg5&0xffff: rec16_fill fill +
 * 6-entry table from s1 (odd idx: lh*divu(0x100,n)>>12) ->
 * +0x940 = s0+0x904; else +0x940 = s1. t2 = MB8(s5+2)?0x7bc7:
 * 0x7bc6; +0x944 = MB8(s5+3)==0xa ? 0x801b216c : 0x801b2134.
 * 2x21 records stride 0x28 from s0+4: tag 0x90000000, bytes
 * 7/39/e + 3 src bytes + fixed palette nibbles. Then 3 ptr
 * quads at s0+0x694.. stride 0x10 offsets 0x6c4/0x754/0x7e4.
 * Header: +0x936=4,+0x938=8,+0x934=0,+0x93a=0xc. */
void anim_workblock_init(uint32_t ent,uint32_t aidx,uint32_t v3,
                  uint32_t v7,uint32_t pk,uint32_t gate)
{
  uint32_t blk, rec, wp, t, cnt, ai;
  int32_t i, j, ci;
  uint32_t rp, pat;
  if (gate != 0) return;
  blk = task_node_alloc(1, 2, 0xc, 0x8013f828u, 0, 0x978u);
  if (blk == 0) { task_node_free(0); return; }
  rec = MB32(blk + 0x10);
  cnt = pk >> 16;
  pk &= 0xffffu;
  if (cnt < 2) cnt = 0xc;
  wp = rec + 4;
  MW16(rec + 0x974, 0x1000);
  MW16(rec + 0x96a, (uint16_t)(v3 + 3));
  MW16(rec + 0x972, (uint16_t)cnt);
  MW16(rec + 0x976, (uint16_t)(0x1000 / (int32_t)cnt));
  MW16(rec + 0x96c, (uint16_t)v7);
  t = MB32(ent + 0x20) + aidx * 80u + 0x20u;
  MW32(rec + 0x93c, t);
  MW16(rec + 0x96e, MB16(ent + 2));
  if (aidx == 7 || aidx == 0xa) aidx = 0x801b1ef4u;
  else aidx = (uint32_t)MB8(ent + 3) * 48u + 0x801b1f24u;
  if ((pk << 16) != 0) {
    uint32_t div;
    MW16(rec + 0x970, (uint16_t)pk);
    rec16_fill(rec + 0x948, 0x1000, pk, 0x1000);
    div = 0x100u / pk;
    t = aidx;
    for (i = 0; i < 6; i++) {
      uint32_t o = rec + 0x904 + (uint32_t)i * 8u;
      MW16(o, MB16(t));
      MW16(o + 4, MB16(t + 4));
      if (i & 1)
        MW16(o + 2, (uint16_t)((int32_t)((int64_t)MBS16(t + 2) * (int64_t)div) >> 12));
      else
        MW16(o + 2, MB16(t + 2));
      t += 8;
    }
    MW32(rec + 0x940, rec + 0x904);
  } else {
    MW32(rec + 0x940, aidx);
  }
  {
    uint16_t ptag = MB8(ent + 2) != 0 ? 0x7bc7 : 0x7bc6;
    MW16(rec + 0x968, 0);
    MW32(rec + 0x944, MB8(ent + 3) == 0xa ? 0x801b216cu : 0x801b2134u);
    for (j = 0; j < 2; j++) {
      ai = 0;
      ci = 0;
      pat = 0;
      for (i = 0; i < 0x15; i++) {
        if (ci == 7) ci = 0;
        if (ci == 0) pat = MB32(rec + 0x944);
        rp = wp;
        MW32(rp, 0x90000000u);
        wp += 0x28;
        MW8(rp + 7, 0x2e);
        MW16(rp + 0x16, 0x39);
        MW16(rp + 0xe, ptag);
        MW8(rp + 4, MB8(pat));
        MW8(rp + 5, MB8(pat + 2));
        MW8(rp + 6, MB8(pat + 4));
        MW8(rp + 0xc, 0xa0);
        MW8(rp + 0xd, 0xa0);
        MW8(rp + 0x14, 0xbf);
        MW8(rp + 0x15, 0xa0);
        MW8(rp + 0x1c, 0xa0);
        MW8(rp + 0x1d, 0xaf);
        MW8(rp + 0x24, 0xbf);
        MW8(rp + 0x25, 0xaf);
        ai++;
        ci++;
      }
    }
    for (j = 0; j < 3; j++) {
      uint32_t base = rec + 0x6c4u + (uint32_t)j * 0x90u;
      uint32_t o = rec + (uint32_t)j * 0x10u;
      MW32(o + 0x698, base + 0x20);
      MW32(o + 0x694, base);
      MW32(o + 0x69c, base + 0x40);
      MW32(o + 0x6a0, base + 0x60);
    }
    MW16(rec + 0x936, 4);
    MW16(rec + 0x938, 8);
    MW16(rec + 0x934, 0);
    MW16(rec + 0x93a, 0xc);
  }
}

/* ======== batch66 ======== */

extern int32_t task_node_alloc_init(int32_t,int32_t,int32_t);

/* anim_wb_init_gate — trampoline @0x801401c8 (2 insns + fallthrough):
 * v0 = MB8(0x1f8000b1) then tail-enters anim_workblock_init. */
void anim_wb_init_gate(uint32_t ent,uint32_t aidx,uint32_t v3,
                  uint32_t v7,uint32_t pk)
{
  anim_workblock_init(ent,aidx,v3,v7,pk,
               MB8(0x1f8000b1u));
}

/* desc_slot_alloc — descriptor slot allocator @0x8017bfb8 (232
 * insns, 3 args; returns s3+4 or early s3+4). s1 = sx8(desc hi
 * byte), mode = s1>>4. mode1: flag&1 -> ret; mode2: !flag ->
 * ret. Scan 8 slots MB32(s0+0x14)+i*4 for byte==-1: clear,
 * b1 = MB8(t0+MB8(s3+2))+1, h1 = desc+2 hi byte, s1 = sx8(new
 * count), next slot b0 = 0xff (only if i+1<8). Then q1/q2/q3
 * from s2+0x102/103/104 signed bytes (neg -> lh(s0+0xba/b8)/
 * lh(s0+0x88)[-3 for q1], pos -> b*16/lh(s0+0x88)); q3 <<= 16.
 * b101 = MB8(s2+0x101): ==2 -> s1==0xa/7/0xd/0x10 calls;
 * ==-1 -> flag&&s1>=0xb&&s1==0xd/0x10 calls;
 * anim_wb_init_gate(s0,K,q2,q1,q3). Tail: if MB32(s0+0x64)&0x20000 &&
 * MBS16(s2+0x80)!=0x11f: s1 in {2,7,10,13,16} ->
 * task_node_alloc_init(s0,s1,0); s1!=0 -> deferred_cmd_post(0x400,0x5a,0). */
uint32_t desc_slot_alloc(uint32_t ent,uint32_t aux,
                      uint32_t desc)
{

  uint32_t cmdw = MB16(desc);
  int32_t cmd_hi = (int32_t)(int8_t)(cmdw >> 8), opc = cmd_hi;
  int32_t mode = cmd_hi >> 4;
  uint32_t rec = aux + MB8(aux + 0x100) * 4u;
  uint32_t tbl = MB32(rec + 0xf8);
  int32_t flag = MB8(ent + 5) & 1;
  int32_t q1, q2, q3, b101, den;
  int i;
  if (mode == 1) { if (flag) return desc + 4; }
  else if (mode == 2) { if (!flag) return desc + 4; }
  for (i = 0; i < 8; i++) {
    uint32_t slot = MB32(ent + 0x14) + (uint32_t)i * 4u;
    if ((int8_t)MB8(slot) == -1) {
      uint32_t nb;
      MW8(slot, 0);
      nb = MB8(tbl + MB8(desc + 2)) + 1u;
      MW8(slot + 1, (uint8_t)nb);
      MW16(slot + 2, (uint16_t)(MB16(desc + 2) >> 8));
      opc = (int32_t)(int8_t)nb;
      if (i + 1 < 8)
        MW8(MB32(ent + 0x14) + (uint32_t)(i + 1) * 4u, 0xff);
      break;
    }
  }
  den = MBS16(ent + 0x88);
  {
    /* MIPS div-by-zero rule (generated): lo = num>=0 ? -1 : 1 */
    int32_t b = (int8_t)MB8(aux + 0x102);
    int32_t n;
    if (b < 0) {
      n = MBS16(ent + 0xba);
      q1 = (den != 0 ? n / den : (n >= 0 ? -1 : 1)) - 3;
    } else {
      n = b * 16;
      q1 = den != 0 ? n / den : -1;
    }
    b = (int8_t)MB8(aux + 0x103);
    if (b < 0) {
      n = MBS16(ent + 0xb8);
      q2 = den != 0 ? n / den : (n >= 0 ? -1 : 1);
    } else {
      n = b * 16;
      q2 = den != 0 ? n / den : -1;
    }
    b = (int8_t)MB8(aux + 0x104);
    if (b < 0) q3 = 4;
    else {
      n = b * 16;
      q3 = den != 0 ? n / den : -1;
    }
  }
  q3 <<= 16;
  b101 = (int8_t)MB8(aux + 0x101);
  if (b101 == 2) {
    if (opc == 0xa) anim_wb_init_gate(ent, 0xa, (uint32_t)q2, (uint32_t)q1, (uint32_t)q3);
    else if (opc < 0xb) { if (opc == 7) anim_wb_init_gate(ent, 7, (uint32_t)q2, (uint32_t)q1, (uint32_t)q3); }
    else {
      if (opc == 0xd) anim_wb_init_gate(ent, 0xd, (uint32_t)q2, (uint32_t)q1, (uint32_t)q3);
      else if (opc == 0x10) anim_wb_init_gate(ent, 0x10, (uint32_t)q2, (uint32_t)q1, (uint32_t)q3);
    }
  } else if (b101 == -1) {
    /* guest reloads lbu 5(ent) here (0x8017c1c4) — after the slot
     * stores, so re-read rather than reuse the cached flag */
    if ((MB8(ent + 5) & 1) && opc != 0xa && opc >= 0xb) {
      if (opc == 0xd) anim_wb_init_gate(ent, 0xd, (uint32_t)q2, (uint32_t)q1, (uint32_t)q3);
      else if (opc == 0x10) anim_wb_init_gate(ent, 0x10, (uint32_t)q2, (uint32_t)q1, (uint32_t)q3);
    }
  }
  if ((MB32(ent + 0x64) & 0x20000u) != 0 && MBS16(aux + 0x80) != 0x11f) {
    if ((uint32_t)(opc - 2) < 0xf &&
        (opc == 2 || opc == 7 || opc == 10 || opc == 13 || opc == 16))
      task_node_alloc_init((int32_t)ent, opc, 0);
    if (opc != 0) deferred_cmd_post(0x400, 0x5a, 0);
  }
  return desc + 4;
}

/* ======== batch67 ======== */

extern uint32_t hit_spark_spawn(undefined4 pos,uint kind,undefined2 aux);
extern uint32_t speech_bubble_spawn(undefined4 *pos,short ang,int idx,ushort fl);
extern uint32_t effect_arg_set(undefined4 arg);
extern uint32_t sprite_burst_spawn(undefined4 ent,undefined4 arg);
extern uint32_t confetti_burst_spawn(undefined4 ent,undefined4 arg);
extern uint32_t popup_spawn(int ent,uint kind,uint fl);
extern uint32_t iris_wipe_spawn(int req,int arg);
extern uint32_t fx_group_alloc(undefined4 pool,undefined4 kind,int ix,int iy);
extern uint32_t noop_b(int node,undefined4 w0,undefined4 w1);
extern uint32_t firework_spawn(undefined2 *pos);
extern uint32_t emit_task_spawn_b(undefined4 *pos,undefined4 ea,undefined4 eb,undefined4 fl);
extern uint32_t fighter_reset_fx(int reset,byte *ctrl);

/* desc_opcode_dispatch — descriptor opcode dispatcher @0x80141d34 (244
 * insns, 3 args + live-in v0 gate -> ret s2+4). a3 =
 * MB16(s2+2); idx = sx8(desc hi byte) < 0x10 -> 16-way table
 * 0x801201e8, else ret. All paths ret s2+4.
 * 0: hit_spark_spawn(s1, flag80?a3&0xff:a3&0x7f, sx8(a3>>8))
 * 1: triple_track_spawn(MB8(s1+2), a3&0xff, sx8(a3>>8))
 * 2/3: speech_bubble_spawn(MB32(s1+0x20)+0x124, MBS16(s1+0x4e)-/+0x400,
 *      a3&0xff, sx8(a3>>8))
 * 4: effect_arg_set(s1)
 * 5/8: sprite_burst_spawn/iris_wipe_spawn(s1, sx16(a3))
 * 6: confetti_burst_spawn(s1, sx16(a3))
 * 7: MB8(MB32(0x1f8000c4)+2)!=0 -> popup_spawn(s1,sx8,a3&0xff)
 * 9: s0 = flag100 ? MB8(0x801b180c+sx16(a3)) : stale; p =
 *    fx_group_alloc(1,1,0,0xf); if p: noop_b(p,
 *    MB32(s1+0x20)+s0*80+0x34, 0x801f0760)
 * 10: color_rec_table_init(MB8(s1+3), sx16(a3))
 * 11: firework_spawn(MB32(s1+0x20)+sx16(a3)*80+0x34)
 * 12: idx2 = (sx8(a3>>8)&0x7f); flag100&&bit15 -> idx2 =
 *    MB8(0x801b180c+idx2); rec = MB32(s1+0x20)+idx2*80; copy
 *    u16 rec+0x34/38/3c to the stack vec; emit_task_spawn_b(buf,
 *    MB32(0x801f0790+(a3&0xff)*4), MB32(0x801f0780+(a3&0xff)*4),0)
 * 13: p = fx_record_alloc(MB32(s1+0x20)+0x124,6,0x80000010,2,
 *    MB32(0x801b1d44)); if p: MW16(p,rand_lcg())
 * 14: p = fx_record_alloc(MB32(s1+0x20)+0x444,0xc,0x80000012,0x8e,
 *    0x801b1d5c); if p: MB32(p)=s1, MB32(p+4)=
 *    0x1f8000e8+((MB8(s1+2)^1)*264)
 * 15: fighter_reset_fx(1,s1) */
uint32_t desc_opcode_dispatch(uint32_t ent,uint32_t unused,
                      uint32_t desc,uint32_t gate)
{
  (void)unused;
  uint32_t arg = MB16(desc + 2);
  uint32_t op, blk, rec;
  int32_t sx8 = (int32_t)(int8_t)(arg >> 8);
  int32_t sx16 = (int32_t)(int16_t)arg;
  /* case-12 vec = guest $sp+0x20 (frame -0x38) = entry_sp-0x18; staged
   * inside a reserved guest frame in the case-12 block below. */
  if (gate != 0) return desc + 4;
  op = (uint32_t)(int32_t)(int8_t)(MB16(desc) >> 8);
  if (op >= 0x10) return desc + 4;
  switch (op) {
  case 0: {
    uint32_t f = MB32(ent + 0x38);
    uint32_t sel = (MB8(f + 0x100) != 0 && (arg & 0x80)) ? (arg & 0xff) : (arg & 0x7f);
    hit_spark_spawn(ent, sel, (uint32_t)sx8);
    break;
  }
  case 1:
    triple_track_spawn(MB8(ent + 2), arg & 0xffu, (uint32_t)sx8);
    break;
  case 2:
    speech_bubble_spawn((undefined4 *)guest_at(MB32(ent + 0x20) + 0x124u),
                 (uint32_t)(MBS16(ent + 0x4e) - 0x400),
                 (uint32_t)sx8, arg & 0xffu);
    break;
  case 3:
    speech_bubble_spawn((undefined4 *)guest_at(MB32(ent + 0x20) + 0x124u),
                 (uint32_t)(MBS16(ent + 0x4e) + 0x400),
                 (uint32_t)sx8, arg & 0xffu);
    break;
  case 4:
    effect_arg_set(ent);
    break;
  case 5:
    sprite_burst_spawn(ent, (uint32_t)sx16);
    break;
  case 6:
    confetti_burst_spawn(ent, (uint32_t)sx16);
    break;
  case 7:
    if (MB8(MB32(0x1f8000c4u) + 2) != 0)
      popup_spawn(ent, (uint32_t)sx8, arg & 0xffu);
    break;
  case 8:
    iris_wipe_spawn(ent, (uint32_t)sx16);
    break;
  case 9: {
    /* guest: s0 = sx16(param) loaded in the branch delay slot
     * (@0x80141f24 sra $16,$3,16); when mo+0x100 != 0 it is replaced by
     * lbu(0x801b180c+s0) (@0x80141f28-34), else it stays sx16 — single
     * convergent path, offset = s0*0x50 + 0x34 (@0x80141f5c-74). */
    int32_t pi = sx16;
    if (MB8(MB32(ent + 0x38) + 0x100) != 0)
      pi = MB8(0x801b180cu + (uint32_t)sx16);
    blk = fx_group_alloc(1, 1, 0, 0xf);
    if (blk != 0)
      noop_b(blk, MB32(ent + 0x20) + (uint32_t)(pi * 0x50) + 0x34u, 0x801f0760u);
    break;
  }
  case 10:
    color_rec_table_init(MB8(ent + 3), (uint32_t)sx16);
    break;
  case 11:
    firework_spawn((undefined2 *)guest_at(MB32(ent + 0x20) + (uint32_t)sx16 * 80u + 0x34u));
    break;
  case 12: {
    /* vec = guest $sp+0x20 (frame -0x38) = entry_sp-0x18; emit_task_spawn_b
     * re-reads it through nested frames, so stage it in a reserved guest
     * frame (sp+0x20 -> frm+0x28). */
    uint32_t dod_frm = br2_guest_frame(0x38);
    uint32_t dod_vec = dod_frm + 0x28;
    uint32_t i2 = (uint32_t)(sx8 & 0x7f);
    if (MB8(MB32(ent + 0x38) + 0x100) != 0 && (sx16 & 0x8000))
      i2 = MB8(0x801b180cu + i2);
    rec = MB32(ent + 0x20) + i2 * 80u;
    MW16(dod_vec + 0, MB16(rec + 0x34));
    MW16(dod_vec + 2, MB16(rec + 0x38));
    MW16(dod_vec + 4, MB16(rec + 0x3c));
    /* emit_task_spawn_b takes a host pointer to the 8-byte vec */
    emit_task_spawn_b((undefined4 *)guest_at(dod_vec),
                 MB32(0x801f0790u + (arg & 0xffu) * 4u),
                 MB32(0x801f0780u + (arg & 0xffu) * 4u), 0);
    br2_guest_frame_free();
    break;
  }
  case 13:
    blk = fx_record_alloc(MB32(ent + 0x20) + 0x124u, 6, 0x80000010u, 2,
                     MB32(0x801b1d44u));
    if (blk != 0) MW16(blk, (uint16_t)rand_lcg());
    break;
  case 14:
    blk = fx_record_alloc(MB32(ent + 0x20) + 0x444u, 0xc, 0x80000012u, 0x8e,
                     0x801b1d5cu);
    if (blk != 0) {
      MW32(blk, ent);
      MW32(blk + 4, 0x1f8000e8u + (uint32_t)(MB8(ent + 2) ^ 1u) * 264u);
    }
    break;
  case 15:
    fighter_reset_fx(1, (byte *)guest_at(ent));
    break;
  }
  return desc + 4;
}

/* script_move_timer_eval — scripted movement/timer evaluator @0x8014b37c
 * (239 insns, 1 arg s0). Phase1: if MBS16(s0+0x1c)!=0 dec it;
 * else if MBS16(0x1f800082)==0 dec MBS16(s0+0x18); if <=0 run
 * opcode stream at MB32(s0+0x10) (pre-advanced +4): cmds
 * 0xfc00/0xfd00/0xfe00/0xff00 with arg; 0xfc00 = if arg<
 * MBS16(s0+0x1a) skip4 else ++cnt & jump MB32(s0+8)+MBS16(ptr+2)*4;
 * 0xfd00 = jump same way; 0xfe00 = +0x1c = MBS16(ptr+2), skip4;
 * 0xff00 = arg<MBS16(s0+0x1a)? skip4 : task_slot_step + rewind4
 * (skips finish block, goes straight to phase2); loop while
 * (MB16(ptr)&0xff00)!=0. Stream end: w=MB16(ptr+2);
 * sx16(w)>0 -> +0x20=w,+0x1e=0,+0x18=w else +0x20=-w,+0x1e=1,
 * +0x18=-w. Phase2: if 0<=MB32(s0+0x14)<2 interpolate s1/s3
 * from s2=MB32(s0+0xc)+tag*12 quad vs +0x18/+0x20. Phase3: if
 * MB32(s0+0x14)==0 -> sprite_place_centered calls (a0 = s0/s0+4 by
 * MBS16(s0+0x18)==0, or indexed by MB32(0x1f80000c)). */
void script_move_timer_eval(uint32_t rec)
{
  uint32_t quad = 0, cur = 0, tw = 0;
  int32_t x = 0, y = 0;
  int32_t cnt = MBS16(rec + 0x1c);
  int do_finish = 0, terminated = 0;
  if (cnt != 0) {
    MW16(rec + 0x1c, MB16(rec + 0x1c) - 1u);
  } else {
    if (MBS16(0x1f800082u) == 0)
      MW16(rec + 0x18, MB16(rec + 0x18) - 1u);
    if (MBS16(rec + 0x18) <= 0) {
      cur = MB32(rec + 0x10);
      MW32(rec + 0x10, cur + 4u);
      if ((MB16(cur + 4) & 0xff00u) != 0) {
        for (;;) {
          uint32_t cmd, arg, next = 0;
          int jumped = 0, stored = 0;
          cur = MB32(rec + 0x10);
          tw = MB16(cur);
          cmd = tw & 0xff00u; arg = tw & 0xffu;
          if (cmd == 0xfd00u) {
            next = MB32(rec + 8) + (uint32_t)MBS16(cur + 2) * 4u;
            jumped = 1;
          } else if (cmd > 0xfd00u) {
            if (cmd == 0xfe00u) {
              MW16(rec + 0x1c, MB16(cur + 2));
              next = cur + 4u; stored = 1;
            } else if (cmd == 0xff00u) {
              if (arg < (uint32_t)MBS16(rec + 0x1a)) {
                next = cur + 4u; stored = 1;
              } else {
                task_slot_step();
                MW16(rec + 0x18, 0);
                MW32(rec + 0x10, MB32(rec + 0x10) - 4u);
                terminated = 1;
                break;
              }
            }
          } else if (cmd == 0xfc00u) {
            if (arg < (uint32_t)MBS16(rec + 0x1a)) {
              next = cur + 4u; stored = 1;
            } else {
              MW16(rec + 0x1a, MB16(rec + 0x1a) + 1u);
              next = MB32(rec + 8) +
                     (uint32_t)MBS16(MB32(rec + 0x10) + 2) * 4u;
              jumped = 1;
            }
          }
          if (jumped || stored) MW32(rec + 0x10, next);
          if ((MB16(MB32(rec + 0x10)) & 0xff00u) == 0) {
            do_finish = 1;
            break;
          }
        }
      } else {
        do_finish = 1;
      }
      if (do_finish) {
        cur = MB32(rec + 0x10);
        tw = MB16(cur + 2);
        if ((int32_t)(int16_t)tw > 0) {
          MW16(rec + 0x20, tw);
          MW16(rec + 0x1e, 0);
          MW16(rec + 0x18, MB16(cur + 2));
        } else {
          MW16(rec + 0x20, (uint16_t)(0u - tw));
          MW16(rec + 0x1e, 1);
          MW16(rec + 0x18, (uint16_t)(0u - MB16(cur + 2)));
        }
      }
    }
  }
  /* phase2 (also reached via the task_slot_step terminate path,
   * which skips only the finish block above) */
  cur = MB32(rec + 0x10);
  quad = MB32(rec + 0xc) + (uint32_t)MBS16(cur) * 12u;
  {
    int32_t ph = (int32_t)MB32(rec + 0x14);
    if (ph >= 0 && ph < 2) {
      int32_t x0 = MBS16(quad), y0 = MBS16(quad + 2);
      int32_t x1 = MBS16(quad + 4), y1 = MBS16(quad + 6);
      int32_t A = MBS16(rec + 0x18), D = MBS16(rec + 0x20);
      int32_t p1, p2;
      if (MB16(rec + 0x1e) == 0) {
        p1 = D != 0 ? (int32_t)((int32_t)((int64_t)(((int64_t)(x0 << 16) -
              (x1 << 16)) * A)) / D) : 0;
        p2 = D != 0 ? (int32_t)((int32_t)((int64_t)(((int64_t)(y0 << 16) -
              (y1 << 16)) * A)) / D) : 0;
        x = x1 + (p1 >> 16);
        y = y1 + (p2 >> 16);
      } else {
        p1 = D != 0 ? (int32_t)((int32_t)((int64_t)(((int64_t)(x1 << 16) -
              (x0 << 16)) * A)) / D) : 0;
        p2 = D != 0 ? (int32_t)((int32_t)((int64_t)(((int64_t)(y1 << 16) -
              (y0 << 16)) * A)) / D) : 0;
        x = x0 + (p1 >> 16);
        y = y0 + (p2 >> 16);
      }
    }
  }
phase2:
  if (MB32(rec + 0x14) != 0) return;
  quad = MB32(rec + 0xc) + (uint32_t)MBS16(MB32(rec + 0x10)) * 12u;
  {
    int32_t dimx = MBS16(quad + 8), dimy = MBS16(quad + 0xa);
    x = (int32_t)(int16_t)x; y = (int32_t)(int16_t)y;
    if (MBS16(rec + 0x18) == 0) {
      sprite_place_centered(MB32(rec), 0x1f800000u, x, y, dimx, dimy);
      sprite_place_centered(MB32(rec + 4), 0x1f800000u, x, y, dimx, dimy);
    } else {
      uint32_t base = MB32(rec);
      if (MB32(0x1f80000cu) != 0) base = MB32(rec + 4);
      sprite_place_centered(base, 0x1f800000u, x, y, dimx, dimy);
    }
  }
}

/* ======== batch69 ======== */

extern int32_t lut_cos_q12(uint32_t);
extern int32_t lut_sin_q12(uint32_t);

/* desc_transform_dispatch — descriptor transform/effect dispatcher
 * @0x8017b1ec (257 insns, 3 args; ret s5+6 or early s5+6).
 * byte1 = sx8(MB16(s5)>>8); mode = byte1>>4; sub = byte1&0xf.
 * mode1: flag MB8(a3+5)&1 -> ret; mode2: !flag -> ret.
 * s2 = MBS16(s5+2), s4 = MBS16(s5+4); sub>=9 -> ret.
 * Table 0x801227d4[sub]:
 *  0: d4=s2, d8=s4
 *  1: r1=8018d298(s1=MBS16(s3+0xd0)); r2,r3,r4=8018d29c(s1)x3;
 *     d4+=(s2*r1 - s4*r2)>>16; d8+=(s2*r3 + s4*r4)>>16
 *  2: rec=MB32(MB32(a3+0x2c)+0x20): d4=MB16(rec+0x34)+s2,
 *     d6=MB16(rec+0x38), d8=MB16(rec+0x3c)+s4
 *  3: rec=MB32(s3+0x10)+(MB8(a3+5)&1)*28; a0=MB32(rec+8)+
 *     MB32(a3+0x30); x=MB32(a0); v1=MB32(a3+0x40):
 *     v1+0x24 = -(sx10(x)<<4); v1+0x26 = -((sx10(x>>10)+s4)<<4);
 *     v1+0x28 = -(sx10(x>>20)<<4); v1+0x78/7a/7c = 0;
 *     d4=MB16(MB32(a3+0x20)+0x174), d6=MB16(+0x178)-s4,
 *     d8=MB16(+0x17c)
 *  4: if s2 < MBS16(s3+0xd6)-MBS16(a3+0x94): d6=MB16(a3+0x94)+s2
 *  5: d6 = MB16(a3+0x94)+s2
 *  6: t=0x801b158c+MB8(0x1f800098)*8; f=MB32(a3+0x6c):
 *     &1 d4=h4(t)-s2; &2 d4=s2-h4(t); &4 d8=h6(t)-s2;
 *     &8 d8=s2-h6(t)
 *  7: d4=MB16(MB32(a3+0x20)+0x174); d8=MB16(+0x17c)
 *  8: s0=s2+MBS16(a3+0x4e); d4+=((8018d298(s0)*s4)+(neg?0xfff:0)
 *     >>12); d8+=((8018d29c(s0)*s4)+(neg?0xfff:0)>>12) */
uint32_t desc_transform_dispatch(uint32_t ent,uint32_t st,
                      uint32_t desc)
{
  int32_t op = (int32_t)(int8_t)(MB16(desc) >> 8);
  int32_t mode = op >> 4, sub = op & 0xf;
  int32_t dx, dy;
  if (mode == 1) { if (MB8(ent + 5) & 1) return desc + 6; }
  else if (mode == 2) { if (!(MB8(ent + 5) & 1)) return desc + 6; }
  dx = MBS16(desc + 2); dy = MBS16(desc + 4);
  if ((uint32_t)sub >= 9) return desc + 6;
  switch (sub) {
  case 0:
    MW16(st + 0xd4, (uint16_t)dx);
    MW16(st + 0xd8, (uint16_t)dy);
    break;
  case 1: {
    int32_t rot = MBS16(st + 0xd0);
    int32_t r1 = lut_cos_q12((uint32_t)rot);
    int32_t r2 = lut_sin_q12((uint32_t)rot);
    int32_t r3 = lut_sin_q12((uint32_t)rot);
    int32_t r4 = lut_cos_q12((uint32_t)rot);
    int32_t d;
    d = (int32_t)(((int32_t)((int64_t)dx * r1) -
                   (int32_t)((int64_t)dy * r2)) >> 16);
    MW16(st + 0xd4, (uint16_t)(MB16(st + 0xd4) + (uint16_t)d));
    d = (int32_t)(((int32_t)((int64_t)dx * r3) +
                   (int32_t)((int64_t)dy * r4)) >> 16);
    MW16(st + 0xd8, (uint16_t)(MB16(st + 0xd8) + (uint16_t)d));
    break;
  }
  case 2: {
    uint32_t rec = MB32(MB32(ent + 0x2c) + 0x20);
    MW16(st + 0xd4, (uint16_t)(MB16(rec + 0x34) + (uint16_t)dx));
    MW16(st + 0xd6, MB16(rec + 0x38));
    MW16(st + 0xd8, (uint16_t)(MB16(rec + 0x3c) + (uint16_t)dy));
    break;
  }
  case 3: {
    uint32_t rec = MB32(st + 0x10) + (uint32_t)(MB8(ent + 5) & 1) * 28u;
    uint32_t aptr = MB32(rec + 8) + MB32(ent + 0x30);
    uint32_t p20 = MB32(ent + 0x20);
    int32_t e;
    /* guest stores d4/d6/d8 first, then computes aptr; ent+0x40 and
     * MB32(aptr) are RELOADED before every use, matching the guest's
     * repeated lw's */
    MW16(st + 0xd4, MB16(p20 + 0x174));
    MW16(st + 0xd6, (uint16_t)(MB16(p20 + 0x178) - (uint16_t)dy));
    MW16(st + 0xd8, MB16(p20 + 0x17c));
    e = (int32_t)(((int32_t)(MB32(aptr) << 22)) >> 18);
    MW16(MB32(ent + 0x40) + 0x24, (uint16_t)(0u - (uint32_t)e));
    e = (int32_t)((((int32_t)(MB32(aptr) << 12)) >> 22) + dy);
    MW16(MB32(ent + 0x40) + 0x26, (uint16_t)(0u - (uint32_t)(e << 4)));
    e = (int32_t)(((int32_t)(MB32(aptr) << 2)) >> 22);
    MW16(MB32(ent + 0x40) + 0x28, (uint16_t)(0u - (uint32_t)(e << 4)));
    MW16(MB32(ent + 0x40) + 0x78, 0);
    MW16(MB32(ent + 0x40) + 0x7a, 0);
    MW16(MB32(ent + 0x40) + 0x7c, 0);
    break;
  }
  case 4:
    if (dx < MBS16(st + 0xd6) - MBS16(ent + 0x94))
      MW16(st + 0xd6, (uint16_t)(MB16(ent + 0x94) + (uint16_t)dx));
    break;
  case 5:
    MW16(st + 0xd6, (uint16_t)(MB16(ent + 0x94) + (uint16_t)dx));
    break;
  case 6: {
    uint32_t row = 0x801b158cu + (uint32_t)(int32_t)(int8_t)MB8(0x1f800098u) * 8u;
    uint32_t fl = MB32(ent + 0x6c);
    if (fl & 1) MW16(st + 0xd4, (uint16_t)(MB16(row + 4) - (uint16_t)dx));
    if (fl & 2) MW16(st + 0xd4, (uint16_t)((uint16_t)dx - MB16(row + 4)));
    if (fl & 4) MW16(st + 0xd8, (uint16_t)(MB16(row + 6) - (uint16_t)dx));
    if (fl & 8) MW16(st + 0xd8, (uint16_t)((uint16_t)dx - MB16(row + 6)));
    break;
  }
  case 7:
    MW16(st + 0xd4, MB16(MB32(ent + 0x20) + 0x174));
    MW16(st + 0xd8, MB16(MB32(ent + 0x20) + 0x17c));
    break;
  case 8: {
    int32_t ang = dx + MBS16(ent + 0x4e);
    int32_t prod, dv;
    prod = (int32_t)((int64_t)lut_cos_q12((uint32_t)ang) * dy) ;
    if (prod < 0) prod += 0xfff;
    dv = prod >> 12;
    MW16(st + 0xd4, (uint16_t)(MB16(st + 0xd4) + (uint16_t)dv));
    prod = (int32_t)((int64_t)lut_sin_q12((uint32_t)ang) * dy);
    if (prod < 0) prod += 0xfff;
    dv = prod >> 12;
    MW16(st + 0xd8, (uint16_t)(MB16(st + 0xd8) + (uint16_t)dv));
    break;
  }
  }
  return desc + 6;
}

/* ======================================================================
 * Round-4 ports: disasm-verified guest functions appended below.
 * Guest addrs go through the MB/MW macros; host-to-guest via
 * guest_of. */

extern int32_t cd_file_open_wait(uint32_t,uint32_t,uint32_t);
extern int task_alloc_wrap_d(uint32_t,uint32_t,uint32_t,uint32_t,int32_t);
extern void cd_pending_release(void);
extern int32_t heap_serialize(uint32_t,uint32_t,uint32_t,uint32_t,
                              uint32_t,uint32_t,uint32_t);
extern uint32_t peak_track(void);
extern int32_t display_cur_invalidate(void);
extern int32_t display_pair_invalidate(void);
extern int32_t euler_rotmat(uint32_t,uint32_t,uint32_t,void *);
extern void part_link_unlink(void *,void *,int32_t);

/* state_pair_init @0x8012b46c (5 insns): p = MB32(0x1f800084);
 * MB8(p)=1; MB8(p+1)=0; return p (reloaded). */
int32_t state_pair_init(void)
{
  uint32_t p = MB32(0x1f800084u);
  MW8(p + 0u, 1u);
  p = MB32(0x1f800084u);
  MW8(p + 1u, 0u);
  return (int32_t)MB32(0x1f800084u);
}

/* descblk_variant_e @0x8012e3c4 — 0x1f80038c text prim init + text_render_full.
 * Verified vs disasm; block layout matches announce_param_block_init port. */
void descblk_variant_e(uint32_t ctx,uint32_t ctx2,void *str,
                  int32_t x,int32_t y,int32_t attr)
{
  MW32(0x1f8003a8u, guest_of(str));
  MW16(0x1f800396u, (uint16_t)x);              /* +6  x */
  MW16(0x1f800398u, 0x18u);                          /* +0xa */
  MW16(0x1f80039au, 0xffffu);                        /* +0xe */
  MW16(0x1f80039cu, 0x20u);                          /* +0x10 */
  MW16(0x1f80039eu, 1u);                             /* +0x12 */
  MW16(0x1f8003a0u, 2u);                             /* +0x14 */
  MW16(0x1f80038cu, 0u);
  MW16(0x1f80038eu, 0u);
  MW16(0x1f800390u, 0u);
  MW16(0x1f800392u, 0x20u);                          /* +0xc */
  MW16(0x1f800394u, (uint16_t)y);              /* +8  y */
  MW16(0x1f8003a4u, 3u);                             /* +0x18 */
  MW16(0x1f8003a2u,
       (uint16_t)((((uint32_t)attr + 0x1e0u) & 0x1ffu) * 0x40u + 0x1eu));
  hud_msg_draw(ctx, ctx2);
}

/* descblk_variant_f @0x8012e4f4 — 7-arg variant: +0x12 = arg7, +0x16 from
 * arg6; +0x10 left untouched (disasm-verified). */
void descblk_variant_f(uint32_t ctx,uint32_t ctx2,void *str,
                  int32_t x,int32_t y,int32_t attr,
                  int32_t f12)
{
  MW32(0x1f8003a8u, guest_of(str));
  MW16(0x1f800396u, (uint16_t)x);              /* +6 */
  MW16(0x1f800398u, 0x18u);                          /* +0xa */
  MW16(0x1f80039au, 0xffffu);                        /* +0xe */
  /* +0x10 not written */
  MW16(0x1f80039eu, (uint16_t)f12);              /* +0x12 */
  MW16(0x1f8003a0u, 2u);                             /* +0x14 */
  MW16(0x1f80038cu, 0u);
  MW16(0x1f80038eu, 0u);
  MW16(0x1f800390u, 0u);
  MW16(0x1f800392u, 0x20u);                          /* +0xc */
  MW16(0x1f800394u, (uint16_t)y);              /* +8 */
  MW16(0x1f8003a4u, 3u);                             /* +0x18 */
  MW16(0x1f8003a2u,
       (uint16_t)((((uint32_t)attr + 0x1e0u) & 0x1ffu) * 0x40u + 0x1eu));
  hud_msg_draw(ctx, ctx2);
}

/* attr_records_scan @0x8015643c (16 insns): scan 16-byte records at
 * 0x801f24d0 + i*16 for i in [a0,a1); return 0 on first nonzero
 * byte, 1 if all zero (or empty range). */
int32_t attr_records_scan(uint32_t lo,uint32_t hi)
{
  uint32_t i;
  for (i = lo; i < hi; i++) {
    if (MB8(0x801f24d0u + i * 16u) != 0) return 0;
  }
  return 1;
}

/* memcard_block_verify @0x8016827c — memcard verify: for each 0x80-byte block
 * of a1 (a2 = iter count, -4 per block), pump job_wait_pump into a
 * guest-visible 0x80 buffer then compare 0x20 u32s against a0.
 * Returns pump status or -1. */
int32_t memcard_block_verify(uint32_t exp,uint32_t src,int32_t iters)
{
  /* job_wait_pump DMAs card data to a guest address, so the compare buffer
   * must live in guest RAM — a host .bss array makes guest_of() return 0
   * and the DMA lands at guest addr 0. br2_guest_frame mirrors the guest
   * stack buffer. */
  uint32_t fr = br2_guest_frame(0x80);
  uint32_t gbuf = fr;
  uint32_t ep = exp;
  uint32_t sp = src;
  int32_t  n = iters;
  int32_t  rc = 0;
  if (n <= 0) { br2_guest_frame_free(); return 0; }
  do {
    uint32_t pr = job_wait_pump(gbuf, sp, 0x80u);
    uint32_t j;
    if (pr != 0) { rc = (int32_t)pr; break; }
    for (j = 0; j < 0x20; j++) {
      if (MB32(ep) != MB32(gbuf + j * 4u)) { rc = -1; goto out; }
      ep += 4u;
    }
    sp += 0x80u;
    n -= 4;
  } while (n > 0);
out:
  br2_guest_frame_free();
  return rc;
}

/* job_result_poll_stable @0x80168648 — poll cd_file_open_wait until two consecutive
 * equal results; unsigned <3 && ==0 returns 0; ==-1 (unsigned >=3)
 * triggers cd_pending_release each poll before the stability check. */
int32_t job_result_poll_stable(uint32_t arg0,uint32_t arg1,uint32_t arg2)
{
  int32_t prev = 0;
  int32_t res;
  for (;;) {
    res = cd_file_open_wait(arg0, arg1, arg2);
    if ((uint32_t)res < 3u) {
      if (res == 0) return 0;
    } else if (res == -1) {
      cd_pending_release();
    }
    if (prev != res) { prev = res; continue; }
    return res;
  }
}

/* job_state_query @0x80168700 — cd_pending_release wrapper; returns the
 * post-call job state word (v0 = -1 after a cancel, else the
 * negative status left in 0x801febf4). */
int32_t job_state_query(void)
{
  cd_pending_release();
  return (int32_t)MB32(0x801febf4u);
}

/* save_flow_state_machine @0x8015d420 — save-flow state machine (states
 * 0=init,1=frame,0xf0=commit,0xf1/0xff=exit). blk[] mirrors guest
 * sp+0x10..0x37 passed to menu_stepper_state_advance. */
int32_t save_flow_state_machine(void)
{
  /* blk mirrors the guest sp+0x10..0x37 stepper descriptor handed to
   * menu_stepper_state_advance; its +0xc callback is a u32 guest code
   * address and generated callees receive a0 = guest_of(blk), which is
   * 0 for a host stack array — back it with the real 0x68-byte guest
   * frame (prologue addiu sp,-0x68), blk at sp+0x10 -> frm+0x10. */
  uint32_t sff_frm = br2_guest_frame(0x68);
  uint8_t *blk = (uint8_t *)guest_at(sff_frm + 0x10);
  int32_t state = 0;
  MW16(0x1f80008cu, 0);
  memset(blk, 0, 0x28);
  for (;;) {
    int do_checks = 0;
    if (state == 0) {
      uint32_t cfg;
      if (frame_divisor_get() == 0) {
        print_mode_dispatch_b(2, 0, 0);
        if (intro_movie_state(0) != 0) {
          deferred_cmd_post(0x203, 0, 0);
          chan_reset_register();
          cfg = MB32(0x1f8000c0u);
          *(uint16_t *)(blk + 0x00) = 0;
          blk[0x02] = 0; blk[0x03] = 0;
          blk[0x04] = 1; blk[0x05] = 0;
          *(uint16_t *)(blk + 0x06) = 9;
          *(uint16_t *)(blk + 0x08) = 9;
          *(uint32_t *)(blk + 0x0c) = 0x8015d69cu;
          *(uint16_t *)(blk + 0x10) = 0x10;
          *(uint16_t *)(blk + 0x12) = 0xffffu;
          *(uint16_t *)(blk + 0x14) = MB8(cfg + 0xcu);
          *(uint16_t *)(blk + 0x16) = MB8(cfg + 0xdu);
          *(uint16_t *)(blk + 0x18) = MB8(cfg + 0xeu);
          *(uint16_t *)(blk + 0x1a) = MB8(cfg + 0xfu);
          MW32(0x801f8508u, 0);
          MW32(0x801f8518u, 0x30u);
          battle_session_arm(0x1f800000u, 0xffffffffu);
        }
      }
      state = 1;
      do_checks = 1;
    } else if (state == 1) {
      do_checks = 1;
    } else if (state == 0xf0) {
      uint32_t cfg;
      deferred_cmd_post(0x203, 0, 0);
      cfg = MB32(0x1f8000c0u);
      MW8(cfg + 0xcu, blk[0x14]);
      brightness_commit((int32_t)(MB8(MB32(0x1f8000c0u) + 0xcu) ^ 1u));
      cfg = MB32(0x1f8000c0u);
      MW8(cfg + 0xdu, blk[0x16]);
      MW8(cfg + 0xeu, blk[0x18]);
      MW8(cfg + 0xfu, blk[0x1a]);
      cfg = MB32(0x1f8000c0u);
      gauge_scale_fixed(MB8(cfg + 0xeu), MB8(cfg + 0xfu));
      chan_reset_clear();
      state = 0xff;
    } else if (state == 0xf1) {
      state = 0xff;
    }
    if (state != 0xff) {
      if (do_checks) {
        if ((MB16(0x1f80007cu) & 0x10u) != 0) {
          deferred_cmd_post(0x400, 0xd, 0);
          state = 0xf0;
        } else if (*(uint16_t *)(blk + 0x08) == *(uint16_t *)(blk + 0x06) &&
                   (MB16(0x1f80007cu) & 0xe0u) != 0) {
          deferred_cmd_post(0x400, 0xc, 0);
          state = 0xf0;
        }
        peak_track();
        menu_stepper_state_advance(blk);
        hud_gauge_render(0x1f800000u, MB32(0x1f800008u), 2);
      }
      frame_tick(0x1f800000);
    }
    if (state == 0xff) { br2_guest_frame_free(); return 1; }
  }
}

/* options_state_machine @0x8015ebf8 — options state machine; same skeleton as
 * save_flow_state_machine but blk layout differs (popcount into +0x1a, cb
 * 0x8016ee40) and tail uses display_cur_invalidate/5e88c + c374 mode 6. */
int32_t options_state_machine(void)
{
  /* Same guest-RAM requirement as save_flow_state_machine: blk is the
   * sp+0x10..0x37 stepper descriptor whose +0xc callback reaches
   * generated callees as a0 = guest_of(blk). Reserve the real 0x80-byte
   * guest frame (prologue addiu sp,-0x80), blk at sp+0x10 -> frm+0x10. */
  uint32_t osf_frm = br2_guest_frame(0x80);
  uint8_t *blk = (uint8_t *)guest_at(osf_frm + 0x10);
  int32_t state = 0;
  int32_t rc = 0;
  MW16(0x1f80008cu, 0);
  MW16(0x1f80008eu, 0);
  memset(blk, 0, 0x28);
  for (;;) {
    int do_checks = 0;
    if (state == 0) {
      uint32_t cfg, i;
      if (frame_divisor_get() == 0) {
        print_mode_dispatch_b(2, 0, 0);
        if (intro_movie_state(0) != 0) {
          uint32_t n = 0;
          deferred_cmd_post(0x203, 0, 0);
          *(uint16_t *)(blk + 0x06) = 3;
          *(uint16_t *)(blk + 0x08) = 3;
          blk[0x00] = 0; blk[0x01] = 0;
          *(uint16_t *)(blk + 0x02) = 0;
          blk[0x04] = 1; blk[0x05] = 0;
          *(uint16_t *)(blk + 0x0c - 0x10 + 0x10) = 0; /* +0x10 u16 */
          *(uint16_t *)(blk + 0x10) = 0x10;
          *(uint16_t *)(blk + 0x12) = 0xffffu;
          *(uint32_t *)(blk + 0x0c) = 0x8015ee40u;
          cfg = MB32(0x1f8000c0u);
          for (i = 0; i < 3; i++) {
            uint32_t v = MB32(cfg + 0x54u + i * 4u);
            while (v != 0) { v &= v - 1u; n++; }
          }
          *(uint16_t *)(blk + 0x1a) = (uint16_t)n;
          *(uint16_t *)(blk + 0x1c) = 0;
          *(uint16_t *)(blk + 0x1e) = 0xffffu;
          *(uint16_t *)(blk + 0x20) = 0;
          *(uint16_t *)(blk + 0x22) = 0xffffu;
          display_cur_invalidate();
          display_pair_invalidate();
          state = 1;
        }
      }
      MW32(0x801f8508u, 0);
      MW32(0x801f8518u, 0x30u);
      battle_session_arm(0x1f800000u, 0xffffffffu);
      rc = (state == 1) ? 1 : 0;
      state = 1;
      do_checks = 1;
    } else if (state == 1) {
      do_checks = 1;
    } else if (state == 0xf0) {
      deferred_cmd_post(0x203, 0, 0);
      state = 0xff;
    } else if (state == 0xf1) {
      state = 0xff;
    }
    if (state != 0xff) {
      if (do_checks) {
        if ((MB16(0x1f80007cu) & 0x10u) != 0) {
          deferred_cmd_post(0x400, 0xd, 0);
          state = 0xf0;
          rc = 1;
        } else if (*(uint16_t *)(blk + 0x08) == *(uint16_t *)(blk + 0x06) &&
                   (MB16(0x1f80007cu) & 0xe0u) != 0) {
          deferred_cmd_post(0x400, 0xc, 0);
          state = 0xf0;
          rc = 1;
        }
        peak_track();
        menu_stepper_state_advance(blk);
        hud_gauge_render(0x1f800000u, MB32(0x1f800008u) + 4u, 6);
      }
      frame_tick(0x1f800000);
    }
    if (state == 0xff) { br2_guest_frame_free(); return rc; }
  }
}

/* screen_flow_state_machine @0x8016088c — same skeleton; blk+0x10=0, +0x12/+0x14
 * fields differ, cb 0x80160a64, tail c374 mode 5, ret s2. */
int32_t screen_flow_state_machine(void)
{
  /* Same guest-RAM requirement as save_flow_state_machine: blk is the
   * sp+0x10..0x37 stepper descriptor whose +0xc callback reaches
   * generated callees as a0 = guest_of(blk). Reserve the real 0x70-byte
   * guest frame (prologue addiu sp,-0x70), blk at sp+0x10 -> frm+0x10. */
  uint32_t scf_frm = br2_guest_frame(0x70);
  uint8_t *blk = (uint8_t *)guest_at(scf_frm + 0x10);
  int32_t state = 0;
  int32_t rc = 0;
  MW16(0x1f80008cu, 0);
  memset(blk, 0, 0x28);
  for (;;) {
    int do_checks = 0;
    if (state == 0) {
      if (frame_divisor_get() == 0) {
        print_mode_dispatch_b(2, 0, 0);
        if (intro_movie_state(0) != 0) {
          deferred_cmd_post(0x203, 0, 0);
          *(uint16_t *)(blk + 0x06) = 2;
          *(uint16_t *)(blk + 0x08) = 2;
          blk[0x00] = 0; blk[0x01] = 0;
          *(uint16_t *)(blk + 0x02) = 0;
          blk[0x04] = 1; blk[0x05] = 0;
          *(uint16_t *)(blk + 0x10) = 0x10;
          *(uint16_t *)(blk + 0x12) = 0xffffu;
          *(uint32_t *)(blk + 0x0c) = 0x80160a64u;
          *(uint16_t *)(blk + 0x14) = 0;
          *(uint16_t *)(blk + 0x16) = 0;
          MW32(0x801f8508u, 0);
          MW32(0x801f8518u, 0x30u);
          battle_session_arm(0x1f800000u, 0xffffffffu);
        }
      }
      state = 1;
      do_checks = 1;
    } else if (state == 1) {
      do_checks = 1;
    } else if (state == 0xf0) {
      deferred_cmd_post(0x203, 0, 0);
      state = 0xff;
    } else if (state == 0xf1) {
      state = 0xff;
    }
    if (state != 0xff) {
      if (do_checks) {
        if ((MB16(0x1f80007cu) & 0x10u) != 0) {
          deferred_cmd_post(0x400, 0xd, 0);
          state = 0xf0;
          rc = 1;
        } else if (*(uint16_t *)(blk + 0x08) == *(uint16_t *)(blk + 0x06) &&
                   (MB16(0x1f80007cu) & 0xe0u) != 0) {
          deferred_cmd_post(0x400, 0xc, 0);
          state = 0xf0;
          rc = 1;
        }
        peak_track();
        menu_stepper_state_advance(blk);
        hud_gauge_render(0x1f800000u, MB32(0x1f800008u), 5);
      }
      frame_tick(0x1f800000);
    }
    if (state == 0xff) { br2_guest_frame_free(); return rc; }
  }
}

/* heap_ptr_fixup @0x8016cb20 — heap pointer fixup pass over the block
 * at guest a0. Two phases: (1) if MB32(a0+4)!=0, convert the index
 * table at a0+MB32(a0+8)*4 (entries >0 = row counts of u32 lists;
 * values with bit31 set are base+idx*4 pointers, bit31 clear kept),
 * then set MB32(a0+4)=1. (2) for MB32(a0+0xc) entries at a0+0x10:
 * nonzero v -> *slot = a0+v*4 then walk the 3-word node chain fixing
 * +0 (skip -1), +4 (base+idx*4), +8 (mask 0x7fffffff). */
void heap_ptr_fixup(uint32_t blk)
{
  int32_t  n;
  uint32_t cur;
  {
    /* tag25/29 hfix_in: dump raw header words blk+0,+4 and +8,+0xc */
    decomp_trace_record(25,blk,MB32(blk+0u),0,MB32(blk+4u));
    decomp_trace_record(29,blk,MB32(blk+8u),0,MB32(blk+0xcu)); }
  /* Phase-1 gate (generated 0x8016CB20): runs iff MB32(blk+4)==0 — the
   * "already fixed" flag. +8 then holds a small word-offset into the
   * block; it is resolved in place to the table's KSEG0 pointer. The
   * previous port gated on the offset instead, which both skipped the
   * pass for (+4==0, +8==0) blocks the guest still processes and ran it
   * for (+4!=0, +8 small) blocks the guest skips. */
  if (MB32(blk + 4u) == 0) {
    uint32_t tbl_off = MB32(blk + 8u);
    cur = blk + tbl_off * 4u;
    MW32(blk + 8u, cur);
    n = (int32_t)MB32(cur);
    cur += 4u;
    if (n > 0) {
      do {
        int32_t inner_n = (int32_t)MB32(cur);
        cur += 4u;
        if (inner_n > 0) {
          do {
            /* generated stores back to the SAME slot it loaded
             * (cur advances in the delay slot AFTER the sw), and the
             * only test is bit31 — a 0x80000000 entry stores blk+0.
             * The prior port wrote one word too far (post-increment)
             * and added a spurious low-bits!=0 guard. */
            uint32_t ent = MB32(cur);
            if ((ent & 0x80000000u) != 0)
              MW32(cur, blk + (ent & 0x7fffffffu) * 4u);
            cur += 4u;
          } while (--inner_n > 0);
        }
      } while (--n > 0);
    }
    MW32(blk + 4u, 1u);
  }
  n = (int32_t)MB32(blk + 0xcu);
  cur = blk + 0x10u;
  if (n > 0) {
    do {
      uint32_t off = MB32(cur);
      if (off != 0) {
        uint32_t cur_ent;
        MW32(cur, blk + off * 4u);
        cur_ent = blk + off * 4u;
        while ((int32_t)MB32(cur_ent + 8u) < 0) {   /* & 0x80000000 */
          uint32_t next;
          next = MB32(cur_ent);
          if (next != 0xffffffffu) MW32(cur_ent, blk + next * 4u);
          MW32(cur_ent + 4u, blk + MB32(cur_ent + 4u) * 4u);
          MW32(cur_ent + 8u, MB32(cur_ent + 8u) & 0x7fffffffu);
          cur_ent = MB32(cur_ent);
          if ((int32_t)cur_ent == -1) break;
        }
      }
      n--;
      cur += 4u;
    } while (n > 0);
  }
  {
    /* tag26 hfix_out: dst=blk, src=gate(+4 post), mode=slotcount(+0xc), len=nested(+8 post) FULL */
    decomp_trace_record(26,blk,MB32(blk+4u),(uint8_t)MB32(blk+0xcu),MB32(blk+8u)); }
}

/* heap_record_emit @0x8016cc5c — serialize one heap record: for rc slots
 * (arg5, or remaining count when 0) at a0+0x10+a3*4, call
 * heap_serialize(slot, a2, dst=a1, fp, &sp20, s7, s6); when it produced
 * output (sp20 != 0) emit a 0x28 record at a1: u16 zeros at +4/+6/+8/
 * +0xc/+0xe, +0x20=src, +0x24=size, and (*s5)++. Returns last a2.
 * heap_serialize's arg5 is a guest out-count pointer (it MW32s the count),
 * so stage it in a real guest frame — a host &sp20 would be written through
 * guest_at() to a bogus address and never reach sp20. */
uint32_t heap_record_emit(uint32_t heap,uint32_t dst,uint32_t src,
                      uint32_t idx,uint32_t cnt0,uint32_t *out,
                      uint32_t fp,uint32_t xoff,uint32_t yoff)
{
  uint32_t dw = dst;          /* dst cursor */
  uint32_t srcw = src;          /* src (returned/advanced) */
  uint32_t idx0 = idx;
  int32_t  cnt = (int32_t)cnt0;
  uint32_t slotp, outsz = 0, fr;
  uint32_t total;
  {
    decomp_trace_record(23,heap,src,idx,0); }  /* tag23 hrec_in */
  {
    /* tag24: src(heapA arg) vs *guest_at(0x801c47b8) live cell — split-brain probe */
    decomp_trace_record(24,src,*(uint32_t *)guest_at(0x801c47b8u),0,0); }
  *out = 0;
  total = MB32(heap + 0xcu);
  if (idx0 >= total) return srcw;
  slotp = heap + 0x10u + idx0 * 4u;
  /* generated tests the FINAL count (arg5, or total-idx0 when arg5==0) with
   * blez before the first slot — a negative nonzero arg5 must not run
   * the do-loop once. */
  if (cnt == 0) cnt = (int32_t)(total - idx0);
  if (cnt <= 0) return srcw;
  fr = br2_guest_frame(0x10);
  do {
    uint32_t slot = MB32(slotp);
    if (slot != 0) {
      uint32_t old = srcw;
      srcw = (uint32_t)heap_serialize(slot, srcw, dw, fp, fr, xoff, yoff);
      outsz = MB32(fr);
      if (outsz != 0) {
        uint32_t wcur = dw + 0x24u;
        /* generated 0x8016CD14: sw zero -> rec+0 (u32), then five u16
         * zeros at +4/+6/+8/+0xc/+0xe, +0x20=src cursor, +0x24=size.
         * The +0 word was missing — the emitted record kept stale heap
         * data in its tag/first word. */
        MW32(dw + 0x00u, 0);
        MW16(dw + 0x04u, 0); MW16(dw + 0x06u, 0); MW16(dw + 0x08u, 0);
        MW16(dw + 0x0cu, 0); MW16(dw + 0x0eu, 0);
        MW32(wcur - 4u, old);          /* +0x20 = src cursor */
        MW32(wcur + 0x00u, srcw - old);  /* +0x24 = bytes produced */
        wcur += 0x28u;
        dw += 0x28u;
        (*out)++;
      }
    }
    slotp += 4u;
    cnt--;
  } while (cnt > 0);
  br2_guest_frame_free();
  {
    /* tag 12: heapA cursor advance = return(srcw) - src(heapA in).
     * dst=heapA_in, src=dst_cursor dw, len=advance. Catches inflated emit. */
    decomp_trace_record(12,src,dw,0,(srcw>src)?(srcw-src):0); }
  return srcw;
}

/* options_menu_alloc @0x8014b938 — allocate + init the options-menu object:
 * task_node_alloc(2,2,0,0x8015b810,0,0x24bc); on success s6 =
 * MB32(blk+0x10); loop1 i=0..99 fills per-entry fields via
 * task_alloc_wrap_d/task_alloc_wrap_c + rand-derived u16s; loop2 s3=0..1,
 * rc=0..99 writes 0x28-stride records (two banks at s5 += 0xfa0). */
int32_t options_menu_alloc(void)
{
  int32_t *blk = (int32_t *)(uintptr_t)task_node_alloc(2, 2, 0, 0x8015b810, 0, 0x24bc);
  uint32_t obj, off, b2, w1, r1, di, w2;
  int32_t  i;
  if (blk == 0) return 0;
  /* blk is the node's GUEST address (task_node_alloc returns guest_of(node));
   * read the +0x10 inline-buffer pointer field by guest addr, not guest_of()
   * (which expects a host ptr and would return 0 -> obj=0 -> wrap_d dest ptrs
   * collapse to small offsets that script_stream_tick writes into low mem). */
  obj = MB32((uint32_t)(uintptr_t)blk + 0x10u);
  off = 0xfa0u;
  b2 = obj + off;
  w1 = obj;
  r1 = obj;
  w2 = obj;
  for (i = 0; i < 100; i++) {
    uint32_t r;
    task_alloc_wrap_d(r1, b2, obj + (uint32_t)i * 4u + 0x2260u,
                 0x801b31d4u, 0x801b31b0u);
    task_alloc_wrap_c(r1 + 0xeu, b2 + 0xeu, 0x801b31f4u, 0x801b320cu);
    r = rand_lcg();
    MW16(w1 + 0x1f40u, (uint16_t)(r & 0x1ffu));
    MW16(w1 + 0x1f42u, (uint16_t)((r & 0x1fu) + 0xf5u));
    {
      int32_t v = (int32_t)(r & 7u) - 4;
      MW16(w1 + 0x20d0u,
           (uint16_t)((v + (int32_t)((uint32_t)v >> 31)) >> 1));
      MW16(w1 + 0x20d2u,
           (uint16_t)((((int32_t)(r & 0xfu) - 4) +
                       (int32_t)(((uint32_t)((int32_t)(r & 0xfu) - 4)) >> 31)) >> 1));
    }
    MW32(w1 + 0x2260u, 1u);
    MW16(w2 + 0x23f0u, (uint16_t)((r & 7u) + 2u));
    w1 += 4u;
    r1 += 0x28u;
    di = (uint32_t)i;
    w2 += 2u;
    (void)di;
  }
  /* loop2: two banks of 100 records */
  {
    int32_t bi;
    uint32_t bank_off = 0;
    for (bi = 0; bi < 2; bi++, bank_off += 0xfa0u) {
      uint32_t b1 = obj + bank_off;
      int32_t  j;
      for (j = 0; j < 100; j++, b1 += 0x28u) {
        uint32_t e = b1 + 0xeu;
        MW8(b1 + 3u, 9u);
        MW8(b1 + 7u, 0x2cu);
        spr_desc_b(b1, 0u);
        spr_desc_c(b1, 0u);
        MW16(b1 + 0x16u, 0x1cu);
        MW8(b1 + 4u, 0x7fu); MW8(b1 + 5u, 0x7fu); MW8(b1 + 6u, 0x7fu);
        MW16(b1 + 0xeu, 0x7d9fu);
        (void)e;
      }
    }
  }
  return 1;
}

/* options_page_dispatch @0x8014bb34 — options-page dispatcher: a0 < 0xc selects
 * one of 12 init cases (jump table @0x80120508); cases 2,7,8 and
 * a0 >= 0xc are no-ops. All task_object_spawn calls use a0=0,
 * a1=MB32(0x801f10c0)+off, a2=MB32(0x801f10c4)+off, a3=tag,
 * arg5=aux; task_spawn_var_b uses a0/a1 = the same pair, a2=tag, a3=aux. */
int32_t options_page_dispatch(int32_t page)
{
  uint32_t P = MB32(0x801f10c0u);
  uint32_t Q = MB32(0x801f10c4u);
  if ((uint32_t)page >= 0xcu) return 0;
  switch (page) {
  case 0:
    task_object_spawn(0, P + 0x118u, Q + 0x118u, 0x801b2980u, 0x801b2990u);
    task_object_spawn(0, P + 0x140u, Q + 0x140u, 0x801b2980u, 0x801b29a8u);
    task_object_spawn(0, P + 0x168u, Q + 0x168u, 0x801b2980u, 0x801b29c0u);
    task_object_spawn(0, P + 0x190u, Q + 0x190u, 0x801b2980u, 0x801b29d8u);
    break;
  case 1:
    task_object_spawn(0, P + 0x118u, Q + 0x118u, 0x801b29f0u, 0x801b2a00u);
    task_object_spawn(0, P + 0x140u, Q + 0x140u, 0x801b29f0u, 0x801b2a18u);
    task_object_spawn(0, P + 0x168u, Q + 0x168u, 0x801b29f0u, 0x801b2a30u);
    task_object_spawn(0, P + 0x190u, Q + 0x190u, 0x801b29f0u, 0x801b2a48u);
    break;
  case 3: {
    uint32_t R  = MB32(MB32(0x801f10bcu) + 0x44u);
    uint32_t R2 = MB32(MB32(0x801f10bcu) + 0x74u);
    task_object_spawn(0, P + 0xa50u, Q + 0xa50u, 0x801b2a60u,
                 MB32(0x801b2b70u + R * 4u));
    task_object_spawn(0, P + 0xa78u, Q + 0xa78u, 0x801b2a60u,
                 MB32(0x801b2cf0u + R2 * 4u));
    task_object_spawn(0, P + 0xaf0u, Q + 0xaf0u, 0x801b2948u, 0x801b2950u);
    task_object_spawn(0, P + 0xb18u, Q + 0xb18u, 0x801b2948u, 0x801b295cu);
    break;
  }
  case 4: {
    uint32_t R  = MB32(MB32(0x801f10bcu) + 0x44u);
    uint32_t R2 = MB32(MB32(0x801f10bcu) + 0x74u);
    task_object_spawn(0, P + 0xa50u, Q + 0xa50u, 0x801b2a60u,
                 MB32(0x801b2bacu + R * 4u));
    task_object_spawn(0, P + 0xa78u, Q + 0xa78u, 0x801b2a60u,
                 MB32(0x801b2d2cu + R2 * 4u));
    task_object_spawn(0, P + 0xaf0u, Q + 0xaf0u, 0x801b2948u, 0x801b2968u);
    task_object_spawn(0, P + 0xb18u, Q + 0xb18u, 0x801b2948u, 0x801b2974u);
    break;
  }
  case 5:
    task_object_spawn(0, P + 0xb40u, Q + 0xb40u, 0x801b2e68u, 0x801b2e78u);
    break;
  case 6:
    task_object_spawn(0, P + 0xb40u, Q + 0xb40u, 0x801b2e70u, 0x801b2e78u);
    break;
  case 9:
    task_spawn_var_b(P + 0x258u, Q + 0x258u, 0x801b2880u, 0x801b2888u);
    task_spawn_var_b(P + 0x280u, Q + 0x280u, 0x801b2880u, 0x801b2898u);
    break;
  case 10:
    task_spawn_var_b(P + 0x1b8u, Q + 0x1b8u, 0x801b28a8u, 0x801b28c8u);
    task_spawn_var_b(P + 0x1e0u, Q + 0x1e0u, 0x801b28a8u, 0x801b28e8u);
    task_spawn_var_b(P + 0x208u, Q + 0x208u, 0x801b28a8u, 0x801b2908u);
    task_spawn_var_b(P + 0x230u, Q + 0x230u, 0x801b28a8u, 0x801b2928u);
    break;
  case 11:
    task_spawn_var_b(P + 0x1b8u, Q + 0x1b8u, 0x801b28b8u, 0x801b28c8u);
    task_spawn_var_b(P + 0x1e0u, Q + 0x1e0u, 0x801b28b8u, 0x801b28e8u);
    task_spawn_var_b(P + 0x208u, Q + 0x208u, 0x801b28b8u, 0x801b2908u);
    task_spawn_var_b(P + 0x230u, Q + 0x230u, 0x801b28b8u, 0x801b2928u);
    break;
  default:
    break; /* 2,7,8: return */
  }
  return 0;
}

/* slot_attr_packet_build @0x8012ba04 — slot/attr packet builder (~260 insns).
 * State block T=0x1f80037c: +0xc sector, +4 u16 count (<0x190),
 * +0/+2 u16 cursor x/y. a0=headcell guest ptr, a1=sector, a2=attr
 * (hi nibble -> block-A code, bits3..7 -> block-B code), a3=x,
 * a4=y (stack arg5), a5=flags (stack arg6; bit0 shade/tab select,
 * bit4 gate). Emits 0x28-byte sprite records at
 * 0x801f8528 + sector*16000 + count*40. Returns -1 when count hits
 * 0x190 after block A, else 0. */
static int32_t attr_code1(uint32_t v)
{
  switch (v) {
  case 0x1000: return 1;
  case 0x2000: return 4;
  case 0x3000: return 5;
  case 0x4000: return 2;
  case 0x6000: return 6;
  case 0x8000: return 3;
  case 0x9000: return 8;
  case 0xc000: return 7;
  default: return 0;
  }
}
static int32_t attr_code2(uint32_t v)
{
  switch (v) {
  case 0x80: return 1;
  case 0x40: return 2;
  case 0x20: return 3;
  case 0x60: return 5;
  case 0xc0: return 4;
  case 0xa0: return 6;
  case 0xe0: return 7;
  default: return 0;
  }
}

int32_t slot_attr_packet_build(uint32_t head,uint32_t sector,uint32_t attr,
                     int32_t cx,int32_t cy,int32_t flags)
{
  const uint32_t T = 0x1f80037cu;
  int32_t shade, code1, code2;
  uint32_t pkt_ptr, base, x, y;
  int drawA;
  if (MB32(T + 0xcu) != sector) {
    MW32(T + 0xcu, sector);
    MW16(T + 4u, 0);
  }
  if ((int16_t)MB16(T + 4u) >= 0x190) return 0;
  base = sector * 16000u;
  pkt_ptr = 0x801f8528u + base +
       (uint32_t)((int32_t)(int16_t)MB16(T + 4u) * 40);
  if ((int16_t)cx < 0x1000) MW16(T + 0u, (uint16_t)cx);
  if ((int16_t)cy < 0x1000) MW16(T + 2u, (uint16_t)cy);
  shade = (flags & 1) ? 0xf : 8;
  code1 = attr_code1(attr & 0xf000u);
  code2 = attr_code2(attr & 0xe8u);

  /* ---- block A ---- */
  if ((flags & 0x10) != 0) {
    if (code1 == 0) goto blockB;
    drawA = 1;
  } else {
    drawA = (base != 0 || code1 != 0);
  }
  if (drawA) {
    uint32_t tab = (flags & 1) ? 0x801b1324u : 0x801b1300u;
    uint32_t p = tab + (uint32_t)code1 * 2u;
    uint8_t b0 = MB8(p), b1 = MB8(p + 1u);
    MW16(pkt_ptr + 0xeu, 0x799eu);
    MW16(pkt_ptr + 0x16u, 0x2eu);
    MW8(pkt_ptr + 0xcu, b0);  MW8(pkt_ptr + 0xdu, b1);
    MW8(pkt_ptr + 0x14u, (uint8_t)(b0 + shade)); MW8(pkt_ptr + 0x15u, b1);
    MW8(pkt_ptr + 0x1cu, b0); MW8(pkt_ptr + 0x1du, (uint8_t)(b1 + shade));
    MW8(pkt_ptr + 0x24u, (uint8_t)(b0 + shade)); MW8(pkt_ptr + 0x25u, (uint8_t)(b1 + shade));
    x = MB16(T + 0u); y = MB16(T + 2u);
    MW16(pkt_ptr + 0x08u, (uint16_t)x);
    MW16(pkt_ptr + 0x0au, (uint16_t)y);
    MW16(pkt_ptr + 0x10u, (uint16_t)(x + shade));
    MW16(pkt_ptr + 0x12u, (uint16_t)y);
    MW16(pkt_ptr + 0x18u, (uint16_t)x);
    MW16(pkt_ptr + 0x1au, (uint16_t)(y + shade));
    MW16(pkt_ptr + 0x20u, (uint16_t)(x + shade));
    MW16(pkt_ptr + 0x22u, (uint16_t)(y + shade));
    MW32(pkt_ptr + 0x00u, MB32(head) | 0x90000000u);
    MW32(head, pkt_ptr & 0xffffffu);
    {
      uint32_t c2 = (uint16_t)(MB16(T + 4u) + 1u);
      MW16(T + 4u, (uint16_t)c2);
      if ((int16_t)c2 < 0x190) {
        MW16(T + 0u, (uint16_t)(MB16(T + 0u) + shade));
        goto blockB;
      }
      return -1;
    }
  }
  if ((flags & 0x10) == 0 || code1 == 0) {
    /* skipped draw: still advance x cursor by shade */
    MW16(T + 0u, (uint16_t)(MB16(T + 0u) + shade));
  }
blockB:
  if ((flags & 1) != 0)
    MW16(T + 0u, (uint16_t)(MB16(T + 0u) - 8));
  if ((flags & 0x10) != 0 && code2 == 0) {
    MW16(T + 0u, (uint16_t)(MB16(T + 0u) + 8));
    return 0;
  }
  if ((flags & 1) == 0 || code2 != 0) {
    /* draw block B */
    uint32_t tab = (flags & 1) ? 0x801b1338u : 0x801b1314u;
    uint32_t p = tab + (uint32_t)code2 * 2u;
    uint8_t b0 = MB8(p), b1 = MB8(p + 1u);
    MW16(pkt_ptr + 0xeu, 0x799eu);
    MW16(pkt_ptr + 0x16u, 0x2eu);
    MW8(pkt_ptr + 0xcu, b0);  MW8(pkt_ptr + 0xdu, b1);
    MW8(pkt_ptr + 0x14u, (uint8_t)(b0 + shade)); MW8(pkt_ptr + 0x15u, b1);
    MW8(pkt_ptr + 0x1cu, b0); MW8(pkt_ptr + 0x1du, (uint8_t)(b1 + shade));
    MW8(pkt_ptr + 0x24u, (uint8_t)(b0 + shade)); MW8(pkt_ptr + 0x25u, (uint8_t)(b1 + shade));
    x = MB16(T + 0u); y = MB16(T + 2u);
    MW16(pkt_ptr + 0x08u, (uint16_t)x);
    MW16(pkt_ptr + 0x0au, (uint16_t)y);
    MW16(pkt_ptr + 0x10u, (uint16_t)(x + shade));
    MW16(pkt_ptr + 0x12u, (uint16_t)y);
    MW16(pkt_ptr + 0x18u, (uint16_t)x);
    MW16(pkt_ptr + 0x1au, (uint16_t)(y + shade));
    MW16(pkt_ptr + 0x20u, (uint16_t)(x + shade));
    MW16(pkt_ptr + 0x22u, (uint16_t)(y + shade));
    MW32(pkt_ptr + 0x00u, MB32(head) | 0x90000000u);
    MW32(head, pkt_ptr & 0xffffffu);
    MW16(T + 4u, (uint16_t)(MB16(T + 4u) + 1u));
    MW16(T + 0u, (uint16_t)(MB16(T + 0u) + shade));
    MW16(T + 0u, (uint16_t)(MB16(T + 0u) + 8));
    return 0;
  }
  /* bit0 set, code2==0, 0x10 clear: skip draw, x += shade + 8 */
  MW16(T + 0u, (uint16_t)(MB16(T + 0u) + shade));
  MW16(T + 0u, (uint16_t)(MB16(T + 0u) + 8));
  return 0;
}

/* terrain_packet_gen @0x80135a14 — terrain/grid packet emitter.
 * Faithful clean port of guest 0x80135A14 plus its CPS split pieces
 * 0x80135BD0/0x80135BF8/0x80135C04/0x80135C3C/0x80135D14/0x80135D1C/
 * 0x80135D28/0x80135DE4/0x80135DE8/0x80135DF0/0x80135F00/0x80135F08/
 * 0x80135F10/0x80135FB8/0x80135FF0/0x8013602C/0x80136074 (verified
 * insn-by-insn against generated/SLES_017.22_full_06.cfg).
 *
 * Guest contract: a0 = packet header (guest addr), a1 = out cursor
 * (guest addr), a2 = u16 triple ptr, a3 = s16 triple ptr, stack args
 * arg5 = s16 triple ptr, arg6 = col count/divisor, arg7 = row
 * count/divisor, arg8 = u8 triple ptr. The pointer params arrive here
 * as host pointers (save.cfg blk[] or guest_at() aliases from the
 * gen_clean_table wrapper) — the triple loads are plain host reads,
 * matching the guest lh/lhu/lbu semantics (param_4/5 signed, param_3
 * unsigned, param_8 bytes).
 *
 * The guest body keeps its scratch halfword frame at sp+0..0x1c and
 * two record cursors in s-registers: g10 = 72-byte-record cursor,
 * g11 = 48-byte-record cursor (both advanced in lockstep with their
 * +20/+12 aliases g6/g7). All frame reloads are lhu; the only lh
 * loads are the two divisor triples. Loop bounds are sltu (unsigned)
 * against param_6/param_7; the cell-shape selects use signed diff
 * compares. Returns the 72-byte cursor (guest $v0 = $t2). */

/* MIPS div lo semantics as modelled by the generated body: signed
 * quotient, and on divide-by-zero lo = -1 when dividend >= 0 else 1.
 * int64 division keeps INT32_MIN / -1 defined. */
static int32_t tpg_idiv(int32_t a, int32_t d)
{
  if (d != 0) return (int32_t)((int64_t)a / (int64_t)d);
  return a >= 0 ? -1 : 1;
}

/* 72-byte grid-cell record. Byte section identical for both variants
 * (blocks 0x80135C04+0x80135C3C and 0x80135DF0): tag bytes 5/72 at
 * +27/+31 and +51/+55, 0x55555555 words at +44/+68, the u8 triple
 * duplicated at +28..30 and +52..54. The halfword section differs:
 * v1 (col >= row) writes the raw triple then col-quotient sums then
 * row+col sums; v2 (col < row / col == row tail) writes row+col sums,
 * row sums, then the raw triple. r = record base (guest $t2/g10). */
static void tpg_cell72_v1(uint32_t r, const uint16_t *fr,
                          uint8_t b0, uint8_t b1, uint8_t b2)
{
  uint32_t cfg = r + 20u;                    /* guest $a2 = r+20 alias */
  MW8(cfg + 7u,  5u);  MW8(cfg + 11u, 72u);
  MW32(cfg + 24u, 0x55555555u);
  MW8(cfg + 31u, 5u);  MW8(cfg + 35u, 72u);
  MW32(cfg + 48u, 0x55555555u);
  MW8(cfg + 8u, b0);  MW8(cfg + 9u, b1);  MW8(cfg + 10u, b2);
  MW8(cfg + 32u, b0); MW8(cfg + 33u, b1); MW8(cfg + 34u, b2);
  MW16(r + 0u,  fr[0]);
  MW16(r + 2u,  fr[1]);
  MW16(r + 4u,  fr[2]);
  MW16(r + 8u,  (uint16_t)(fr[0]  + fr[8]));
  MW16(r + 10u, (uint16_t)(fr[1]  + fr[9]));
  MW16(r + 12u, (uint16_t)(fr[2]  + fr[10]));
  MW16(r + 16u, (uint16_t)(fr[12] + fr[0] + fr[8]));
  MW16(r + 18u, (uint16_t)(fr[13] + fr[1] + fr[9]));
  MW16(r + 20u, (uint16_t)(fr[14] + fr[2] + fr[10]));
}

static void tpg_cell72_v2(uint32_t r, const uint16_t *fr,
                          uint8_t b0, uint8_t b1, uint8_t b2)
{
  uint32_t cfg = r + 20u;
  MW8(cfg + 7u,  5u);  MW8(cfg + 11u, 72u);
  MW32(cfg + 24u, 0x55555555u);
  MW8(cfg + 31u, 5u);  MW8(cfg + 35u, 72u);
  MW32(cfg + 48u, 0x55555555u);
  MW8(cfg + 8u, b0);  MW8(cfg + 9u, b1);  MW8(cfg + 10u, b2);
  MW8(cfg + 32u, b0); MW8(cfg + 33u, b1); MW8(cfg + 34u, b2);
  MW16(r + 0u,  (uint16_t)(fr[12] + fr[0] + fr[8]));
  MW16(r + 2u,  (uint16_t)(fr[13] + fr[1] + fr[9]));
  MW16(r + 4u,  (uint16_t)(fr[14] + fr[2] + fr[10]));
  MW16(r + 8u,  (uint16_t)(fr[0]  + fr[12]));
  MW16(r + 10u, (uint16_t)(fr[1]  + fr[13]));
  MW16(r + 12u, (uint16_t)(fr[2]  + fr[14]));
  MW16(r + 16u, fr[0]);
  MW16(r + 18u, fr[1]);
  MW16(r + 20u, fr[2]);
}

/* 48-byte edge record (blocks 0x80135D28 and 0x80135F10+0x80135FB8).
 * Byte section identical: tag bytes 3/64 at +19/+23 and +35/+39, the
 * u8 triple duplicated at +20..22 and +36..38. Halfword section:
 * v1 (block 0x80135D28) = raw triple, col sums, col+row sums;
 * v2 (block 0x80135F10) = row+col sums, row sums, col+row sums.
 * r = record base (guest $t3/g11). */
static void tpg_cell48_v1(uint32_t r, const uint16_t *fr,
                          uint8_t b0, uint8_t b1, uint8_t b2)
{
  uint32_t cfg = r + 12u;                    /* guest $a3 = r+12 alias */
  MW8(cfg + 7u,  3u);  MW8(cfg + 11u, 64u);
  MW8(cfg + 23u, 3u);  MW8(cfg + 27u, 64u);
  MW8(cfg + 8u, b0);  MW8(cfg + 9u, b1);  MW8(cfg + 10u, b2);
  MW8(cfg + 24u, b0); MW8(cfg + 25u, b1); MW8(cfg + 26u, b2);
  MW16(r + 0u,  fr[0]);
  MW16(r + 2u,  fr[1]);
  MW16(r + 4u,  fr[2]);
  MW16(r + 8u,  (uint16_t)(fr[0] + fr[8]));
  MW16(r + 10u, (uint16_t)(fr[1] + fr[9]));
  MW16(r + 12u, (uint16_t)(fr[2] + fr[10]));
}

static void tpg_cell48_v2(uint32_t r, const uint16_t *fr,
                          uint8_t b0, uint8_t b1, uint8_t b2)
{
  uint32_t cfg = r + 12u;
  MW8(cfg + 7u,  3u);  MW8(cfg + 11u, 64u);
  MW8(cfg + 23u, 3u);  MW8(cfg + 27u, 64u);
  MW8(cfg + 8u, b0);  MW8(cfg + 9u, b1);  MW8(cfg + 10u, b2);
  MW8(cfg + 24u, b0); MW8(cfg + 25u, b1); MW8(cfg + 26u, b2);
  MW16(r + 0u,  (uint16_t)(fr[12] + fr[0] + fr[8]));
  MW16(r + 2u,  (uint16_t)(fr[13] + fr[1] + fr[9]));
  MW16(r + 4u,  (uint16_t)(fr[14] + fr[2] + fr[10]));
  MW16(r + 8u,  (uint16_t)(fr[0] + fr[12]));
  MW16(r + 10u, (uint16_t)(fr[1] + fr[13]));
  MW16(r + 12u, (uint16_t)(fr[2] + fr[14]));
}

uint32_t terrain_packet_gen(uint32_t hdr,uint32_t cur,
                      const uint16_t *u16vec,const int16_t *colvec,
                      const int16_t *rowvec,int32_t sx,
                      int32_t sy,const uint8_t *u8vec)
{




  uint32_t prod, r48, r72;             /* mult lo; g11/g10 cursors */
  uint16_t fr[16];                     /* guest sp+0..0x1c frame (lhu) */
  int32_t  col, row;                   /* g8/g12 loop counters */
  int32_t  diff;                       /* g24 = sx - sy (signed) */
  uint8_t  b0 = u8vec[0], b1 = u8vec[1], b2 = u8vec[2];
  int i;

  /* ---- block_80135A14: header ---- */
  MW32(hdr + 0x1cu, cur);              /* hdr->out_cursor = cur */
  MW16(hdr + 0x0cu, 0);
  MW16(hdr + 0x0eu, 0);

  /* mult lo: (sy<sx) ? sx*(sy+1) : (sx+1)*sy — 32-bit product low.
   * u32 arithmetic reproduces the guest 32-bit wrap exactly. */
  prod = (sy < sx)
       ? (uint32_t)sx * ((uint32_t)sy + 1u)
       : ((uint32_t)sx + 1u) * (uint32_t)sy;
  diff = (int32_t)((uint32_t)sx - (uint32_t)sy);

  if (diff == 0) {
    /* block_80135A70: single header record, cursors cur+8.. */
    MW32(cur + 0x00u, 1u);
    MW32(cur + 0x04u, 0x00140000u);
    MW32(cur + 0x10u, 0x8018ac54u);
    MW32(cur + 0x0cu, cur + 20u);
    MW32(cur + 0x08u, prod);
    r48 = 0;
    r72 = cur + 20u;
  } else {
    /* blocks 80135AA0/80135AAC/80135AB0: two header records */
    uint32_t ud  = (uint32_t)diff;
    uint32_t ad  = (diff >= 0) ? ud : (0u - ud);   /* |diff| */
    uint32_t p36 = cur + 36u;
    uint32_t p72 = p36 + ad * 48u;
    MW32(cur + 0x00u, 2u);
    MW32(cur + 0x04u, 0x00140000u);
    MW32(cur + 0x10u, 0x8018ab84u);
    MW32(cur + 0x08u, ad);
    MW32(cur + 0x0cu, p36);
    MW32(cur + 0x14u, 0x00140000u);
    MW32(cur + 0x20u, 0x8018ac54u);
    MW32(cur + 0x1cu, p72);
    MW32(cur + 0x18u, prod - ad);
    r48 = p36;
    r72 = p72;
  }

  /* ---- block_80135B00: scaled triples into the sp frame ----
   * sp+16..20 = lh(colvec)/sx, sp+24..28 = lh(rowvec)/sy,
   * sp+0..4 = lhu(u16vec). Frame reloads everywhere below are lhu. */
  for (i = 0; i < 3; i++) {
    fr[8  + i] = (uint16_t)tpg_idiv((int32_t)colvec[i], sx);
    fr[12 + i] = (uint16_t)tpg_idiv((int32_t)rowvec[i], sy);
  }
  fr[0] = u16vec[0];
  fr[1] = u16vec[1];
  fr[2] = u16vec[2];

  if (sy == 0) return r72;             /* block_80135BB0 -> epilogue */

  row = 0;
  for (;;) {                           /* block_80135BD0: row head */
    fr[4] = fr[0]; fr[5] = fr[1]; fr[6] = fr[2];  /* snapshot sp+8..12 */
    col = 0;                           /* g8 */
    if (sx != 0) {                     /* block_80135BE8 guard */
      /* block_80135BF0: g6 = g10+20, g7 = g11+12 — the +20/+12 aliases
       * stay locked to the record cursors because both advance by the
       * record stride together, so the emit helpers derive them from
       * r72/r48 at each call. */
      for (;;) {                       /* block_80135BF8: col head */
        if ((uint32_t)col < (uint32_t)row) {
          /* block_80135D14: lower-triangle column.
           * emit48 iff diff<0 && col==sx-1 (blocks 80135D1C/80135D28);
           * the 48B cursor then always falls into the v2 72B emit. */
          if (diff < 0 && (uint32_t)col == (uint32_t)sx - 1u) {
            tpg_cell48_v1(r48, fr, b0, b1, b2);
            r48 += 48u;
          }
          tpg_cell72_v2(r72, fr, b0, b1, b2);
          r72 += 72u;
        } else {
          /* block_80135C04/80135C3C: upper-triangle column, v1 72B. */
          tpg_cell72_v1(r72, fr, b0, b1, b2);
          r72 += 72u;
          if ((uint32_t)row < (uint32_t)col) {
            /* blocks 80135F00/80135F08: last row of a wide grid also
             * emits a v2 48B edge record iff diff>0 && row==sy-1. */
            if (diff > 0 && (uint32_t)row == (uint32_t)sy - 1u) {
              tpg_cell48_v2(r48, fr, b0, b1, b2);
              r48 += 48u;
            }
          } else {
            /* row == col: block_80135DE8 g2=0 -> block_80135DF0 v2. */
            tpg_cell72_v2(r72, fr, b0, b1, b2);
            r72 += 72u;
          }
        }
        /* block_80135FF0: next column + accumulate col quotients */
        col++;
        fr[0] = (uint16_t)(fr[0] + fr[8]);
        fr[1] = (uint16_t)(fr[1] + fr[9]);
        fr[2] = (uint16_t)(fr[2] + fr[10]);
        if (!((uint32_t)col < (uint32_t)sx)) break;
      }
    }
    /* block_8013602C: next row + accumulate row quotients */
    row++;
    fr[0] = (uint16_t)(fr[4] + fr[12]);
    fr[1] = (uint16_t)(fr[5] + fr[13]);
    fr[2] = (uint16_t)(fr[6] + fr[14]);
    if (!((uint32_t)row < (uint32_t)sy)) break;
  }
  return r72;                          /* $v0 = $t2 (block_80136074) */
}



/* ======== batch66: peak tracker ======== */

/* peak_track @0x801583cc (~85 insns, 0 args): scans a 256-entry
 * window of two s16 tables @0x801f36f0 / @0x801f3af0 (+0x400 stride
 * pair per index) for max |value| in each. Window = [0x100,0x200)
 * when *@0x801f46f4 != 0 else [0,0x100). Stores max1 -> @0x801f46f8.
 * If *@0x801f36e4 < max1 (u32): @0x801f36e8 = max2, @0x801f36e4 =
 * max1, @0x801b3ed0 = 0. If *@0x801f46f0 < max2: @0x801f46f0 = max2,
 * @0x801b3ed4 = 0. Then: if @0x801b3ed0 < 0x78 -> ++ and continue to
 * ed4 check; else @0x801b3ed0 = 0 and @0x801f36e4 = max1. For ed4:
 * <0x78 -> ++, return new count; else @0x801b3ed4 = 0 and
 * @0x801f46f0 = @0x801f36e8, return 0x801f0000 (guest quirk: v0
 * left as the lui base). */
uint32_t peak_track(void)
{
  uint32_t pk_a = 0, pk_b = 0;
  uint32_t i_lo, i_hi, i;
  if (MB32(0x801f46f4u) != 0) { i_lo = 0x100; i_hi = 0x200; }
  else                        { i_lo = 0;     i_hi = 0x100; }
  for (i = i_lo; i < i_hi; i++) {
    uint32_t p = 0x801f36f0u + i * 2u;
    int32_t samp_a = MBS16(p);
    int32_t samp_b = MBS16(p + 0x400u);
    int32_t mag_a = samp_a < 0 ? -samp_a : samp_a;
    int32_t mag_b = samp_b < 0 ? -samp_b : samp_b;
    if ((uint16_t)pk_a < (uint16_t)mag_a) pk_a = (uint32_t)mag_a;
    if ((uint16_t)pk_b < (uint16_t)mag_b) pk_b = (uint32_t)mag_b;
  }
  {
    uint32_t pk_a16 = pk_a & 0xffffu;
    uint32_t mag_a = pk_b & 0xffffu;
    uint32_t tcnt;
    MW32(0x801f46f8u, pk_a16);
    if (MB32(0x801f36e4u) < pk_a16) {
      MW32(0x801f36e8u, mag_a);
      MW32(0x801f36e4u, pk_a16);
      MW32(0x801b3ed0u, 0);
    }
    if (MB32(0x801f46f0u) < mag_a) {
      MW32(0x801f46f0u, mag_a);
      MW32(0x801b3ed4u, 0);
    }
    tcnt = MB32(0x801b3ed0u);
    if (tcnt < 0x78) {
      MW32(0x801b3ed0u, tcnt + 1);
    } else {
      MW32(0x801b3ed0u, 0);
      MW32(0x801f36e4u, pk_a16);
    }
    tcnt = MB32(0x801b3ed4u);
    if (tcnt < 0x78) {
      MW32(0x801b3ed4u, tcnt + 1);
      return tcnt + 1;
    }
    tcnt = MB32(0x801f36e8u);
    MW32(0x801b3ed4u, 0);
    MW32(0x801f46f0u, tcnt);
    return 0x801f0000u;
  }
}

/* ======================================================================
 * Round-5 ports: skeleton matrix init + heap-record serializer walk.
 * Both disasm-verified against psx_payload.bin (file off = vaddr -
 * 0x8011e000). Guest addrs go through the MB/MW macros; the two
 * host-pointer callees (euler_rotmat matrix decode, part_link_unlink
 * part-link) take guest_at() aliases of the guest addresses.
 * ====================================================================== */

/* skeleton_init @0x80179ae0 (~350 insns) — skeleton_init: part/matrix
 * init from S5 descriptor. a0 = ctrl (guest), a1 = desc block inside
 * S5 (two per section, slot*0x1c apart), a2 dead.
 *
 * desc fields are offsets into the ani blob MB32(ctrl+0x30):
 *   +0x00 s16 linktab   +0x04 s16 partmap   +0x08 u32 joint seeds
 *   +0x0c u32 rec seeds +0x10 u32 euler seeds+0x18 u16 extra table
 * ctrl fields: +6 nparts (lb, abs'd for tail loops), +8 nrecs (lb),
 *   +0x18/+0x1c rec arrays (stride 0x28), +0x20 fobj parts (0x50),
 *   +0x2c foe ctrl, +0x30 ani base, +0x44/+0x48 matrix arrays (0x20).
 * NOTE: euler seeds come from desc+0x10[i-1], joint fields (+0x18/
 * 1a/1c) from desc+8[i-1] — action.c's skeleton_init reads both
 * from desc+8; the disasm splits them (s7=+0x10 for euler_rotmat,
 * a0=+8 for the sh fields). Ported verbatim here.
 * Return value is whatever v0 last held; callers ignore it. */
int32_t skeleton_init(uint32_t ctrl,uint32_t desc,uint32_t unused3)
{


  uint32_t base = MB32(ctrl + 0x30u);
  int32_t  np   = (int8_t)MB8(ctrl + 6u);   /* lb */
  int32_t  nr   = (int8_t)MB8(ctrl + 8u);   /* lb */
  uint32_t linktab = MB32(desc + 0x00u) + base;
  uint32_t partmap = MB32(desc + 0x04u) + base;
  uint32_t seedJ   = MB32(desc + 0x08u) + base;
  uint32_t seedB   = MB32(desc + 0x0cu) + base;
  uint32_t seedM   = MB32(desc + 0x10u) + base;
  uint32_t extrat  = MB32(desc + 0x18u) + base;
  int32_t  i, j, k;
  (void)unused3;

  /* zero part-record lead fields: fobj + i*0x50, i = nr-1 .. 0 */
  for (i = nr - 1; i >= 0; i--)
    MW16(MB32(ctrl + 0x20u) + (uint32_t)i * 0x50u, 0u);

  /* per-part transform/matrix init, i = nr-1 .. 0 */
  for (i = nr - 1; i >= 0; i--) {
    int32_t  link = MBS16(linktab + (uint32_t)i * 2u);
    uint32_t ioff = (uint32_t)i * 0x20u;
    if (i == 0) {
      uint32_t m = MB32(ctrl + 0x44u);
      MW32(m + 0x00u, 0x1000u); MW32(m + 0x04u, 0u);
      MW32(m + 0x08u, 0x1000u); MW32(m + 0x0cu, 0u);
      MW32(m + 0x10u, 0x1000u); MW32(m + 0x14u, 0u);
      MW32(m + 0x18u, 0u);      MW32(m + 0x1cu, 0u);
    } else {
      uint32_t m32  = MB32(seedM + (uint32_t)(i - 1) * 4u);
      uint32_t jv  = MB32(seedJ + (uint32_t)(i - 1) * 4u);
      uint32_t mdst = MB32(ctrl + 0x44u) + ioff;
      uint32_t part = MB32(ctrl + 0x20u) + (uint32_t)i * 0x50u;
      int32_t e0 = (int32_t)((m32 << 22) & 0xffc00000u) >> 20;
      int32_t e1 = (int32_t)((m32 << 12) & 0xffc00000u) >> 20;
      int32_t e2 = (int32_t)((m32 << 2)  & 0xffc00000u) >> 20;
      euler_rotmat((uint32_t)e0,(uint32_t)e1,(uint32_t)e2,
                   (void *)guest_at(mdst));
      MW16(part + 0x18u, (uint16_t)(int16_t)((int32_t)(jv << 22) >> 22));
      MW16(part + 0x1au, (uint16_t)(int16_t)((int32_t)(jv << 12) >> 22));
      MW16(part + 0x1cu, (uint16_t)(int16_t)((int32_t)(jv << 2)  >> 22));
    }
    /* mirror m44[i*0x20..+0x1f] -> m48[i*0x20] (8 x u32) */
    {
      uint32_t s = MB32(ctrl + 0x44u) + ioff;
      uint32_t d = MB32(ctrl + 0x48u) + ioff;
      for (k = 0; k < 8; k++) MW32(d + (uint32_t)k * 4u, MB32(s + (uint32_t)k * 4u));
    }
    /* link dispatch: >=0 own part mode 1; -1 NULL mode 1; -2 own
     * part 0 mode 1; <-2 foe part (-link-3) mode 3 + field clear */
    {
      uint32_t curp = MB32(ctrl + 0x20u) + (uint32_t)i * 0x50u;
      if (link >= 0) {
        part_link_unlink((void *)guest_at(MB32(ctrl + 0x20u)
                                        + (uint32_t)link * 0x50u),
                     (void *)guest_at(curp), 1);
      } else if (link == -1) {
        part_link_unlink((void *)0, (void *)guest_at(curp), 1);
      } else if (link == -2) {
        part_link_unlink((void *)guest_at(MB32(ctrl + 0x20u)),
                     (void *)guest_at(curp), 1);
      } else {
        uint32_t ffobj = MB32(MB32(ctrl + 0x2cu) + 0x20u);
        uint32_t a0g   = ffobj + (uint32_t)(-link) * 0x50u - 0xf0u;
        part_link_unlink((void *)guest_at(a0g),
                     (void *)guest_at(curp), 3);
        MW16(curp + 0x1cu, 0u);
        MW16(curp + 0x1au, 0u);
        MW16(curp + 0x18u, 0u);
      }
    }
  }

  /* linked-part table init, j = abs(np)-1 .. 0 (sp10 = |lb(ctrl+6)|) */
  if (np < 0) np = -np;
  for (j = np - 1; j >= 0; j--) {
    int32_t  pm   = MBS16(partmap + (uint32_t)j * 2u);
    uint32_t rec  = MB32(ctrl + 0x18u) + (uint32_t)j * 0x28u;
    if (pm != -1) {
      MW32(MB32(ctrl + 0x20u) + (uint32_t)pm * 0x50u + 0x4cu, rec);
      {
        uint32_t sv = MB32(seedB + (uint32_t)j * 4u);
        MW16(rec + 0x04u, (uint16_t)(int16_t)((int32_t)(sv << 22) >> 22));
        MW16(rec + 0x06u, (uint16_t)(int16_t)((int32_t)(sv << 12) >> 22));
        MW16(rec + 0x08u, (uint16_t)(int16_t)((int32_t)(sv << 2)  >> 22));
      }
    } else {
      MW16(rec + 0x08u, 0u);
      MW16(rec + 0x06u, 0u);
      MW16(rec + 0x04u, 0u);
    }
  }
  for (j = np - 1; j >= 0; j--) {
    int32_t  pm  = MBS16(partmap + (uint32_t)j * 2u);
    uint32_t r18 = MB32(ctrl + 0x18u) + (uint32_t)j * 0x28u;
    uint32_t r1c = MB32(ctrl + 0x1cu) + (uint32_t)j * 0x28u;
    if (pm != -1) {
      uint32_t v = MB32(ctrl + 0x20u) + (uint32_t)pm * 0x50u;
      MW32(r18, v);
      MW32(r1c, v);
    } else {
      MW32(r18, 0u);
      MW32(r1c, 0u);
    }
  }
  for (j = np - 1; j >= 0; j--) {
    uint32_t ev  = MB16(extrat + (uint32_t)j * 2u);   /* lhu */
    uint32_t r18 = MB32(ctrl + 0x18u) + (uint32_t)j * 0x28u;
    uint32_t r1c = MB32(ctrl + 0x1cu) + (uint32_t)j * 0x28u;
    MW16(r18 + 0x0eu, (uint16_t)(ev << 2));
    MW16(r1c + 0x0eu, 0u);
  }
  return 0;
}

/* ---- heap_serialize serializer callees ----
 * Ported already (strong defs above): record_table_expand_a, quad_strip_emit_a,
 * record_table_expand_b, quad_strip_emit_b (7-arg emitters), entry_attr_bump_loop,
 * dual_entry_attr_bump (attr bumps), heap_serialize_tag_01000000 (@0x8016a82c).
 * The rest delegate to the generated
 * guest bodies at the bottom of this file (br2_call_generated +
 * br2_guest_frame stack-arg staging). */
extern uint32_t heap_serialize_tag_08(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_0a(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_0c(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_10(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_12(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_14(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_16(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_0e(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_40048(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_4004c(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_40050(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_40054(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_01000008(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_0100000c(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_0100000a(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_0100000e(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_01000010(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_01000014(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_01000012(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_01000016(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t);
extern uint32_t heap_serialize_tag_40049(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_4004d(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_40051(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_40055(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_01000009(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_0100000d(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_01000011(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_01000015(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t,uint32_t);
extern uint32_t heap_serialize_tag_01000000(uint32_t,uint32_t,uint32_t,uint32_t,
                             uint32_t,uint32_t);

/* heap_serialize @0x8016b7c0 (~1239 insns) — heap-record serializer
 * walk. Args (o32): a0 = block list head (blk+0 next/-1, +4 ctx,
 * +8 u32 record count, +0xc records), a1 = cursor in/out (s1),
 * a2 = dst record (gets s1 at +0x1c), a3 = mode (name select),
 * stack: arg5 = out count ptr, arg6 = dx (s7), arg7 = dy (fp).
 *
 * Record format: {u32 tag; u16 size@+4; u16 flags@+6; payload};
 * next = rec + 4 + size*4. tag & 0x0fffffff dispatches; handled set
 * verified against the compare tree (skipped tags fall through to
 * the stride advance). Each handled record emits a 16-byte scratch
 * entry {raw tag, flags&0x7fff, cursor-before-call, name-ptr} and
 * calls a per-tag serializer; cursor = return value. Two tags
 * (0x01000000, 0x02000000/1) bypass the emit path.
 *
 * Name selection: two-way cases pick 0x8019xxxx when mode==1 else
 * 0x8018xxxx; multi-way cases key on mode (1/3/7/0x81/0x23 and the
 * two 0x23-entry guest jump tables @0x80122164/0x801221f4 for tags
 * 0x40049/0x40051 — tables read from the payload, only indices
 * {0,2,6,18,34} have non-default targets).
 *
 * Epilogue: *dst+0x1c = cursor (before trailer), *outp = count,
 * then u32 count + count*16 scratch bytes appended at cursor;
 * returns cursor past the trailer. */
int32_t heap_serialize(uint32_t blk,uint32_t cur,uint32_t dst,
                     uint32_t mode,uint32_t outp,uint32_t xoff,
                     uint32_t yoff)
{







  uint32_t scr[128];             /* sp+0x20: guest fits 16 x 16B;
                                  * 32 here so an over-count can't
                                  * smash the C frame (guest would
                                  * corrupt its own locals instead) */
  uint32_t emit_n = 0;
  uint32_t i;
  uint32_t entry_blk = blk;   /* entry blk for trace */
  {
    decomp_trace_record(22,blk,cur,dst,0); }  /* tag22 hser_in */

#define B7C0_EMIT(N_) do {                                    \
    scr[emit_n*4u+0u] = tag;   scr[emit_n*4u+1u] = param;             \
    scr[emit_n*4u+2u] = cur;    scr[emit_n*4u+3u] = (uint32_t)(N_);   \
    emit_n++; } while (0)
#define B7C0_CALL5(F_)  cur = (F_)(cur, src,                    \
                     MB32(ctx + 8u), MB32(ctx + 0xcu), param)
#define B7C0_CALL7(F_)  cur = (F_)(cur, src,                    \
                     MB32(ctx + 8u), MB32(ctx + 0xcu), param,   \
                     xoff, yoff)
#define B7C0_CALL5H(F_) cur = (F_)(cur, src,                    \
                     MB32(ctx + 0xcu), MB32(ctx + 0x14u), param)
#define B7C0_CALL7H(F_) cur = (F_)(cur, src,                    \
                     MB32(ctx + 0xcu), MB32(ctx + 0x14u), param,\
                     xoff, yoff)

  for (;;) {
    uint32_t cnt = MB32(blk + 8u);
    uint32_t ctx = MB32(blk + 4u);
    uint32_t rec;
    /* The serialize ctx's I/O base pointers (+4/+8/+0xc/+0x10/+0x14) arrive
     * offset-encoded as 0x8000xxxx tags into the 0x800F0000 heap image; the
     * tag serializers consume them as absolute KSEG0 pointers, so resolve
     * them against the image base. Guarded: only fires while a field still
     * carries the unrelocated 0x8000xxxx tag (relocated 0x800Fxxxx and live
     * heapA 0x8001xxxx pointers are untouched). */
    { uint32_t ci;
      for (ci = 0u; ci < 5u; ci++) {
        uint32_t ro = 4u + ci * 4u;
        uint32_t v  = MB32(ctx + ro);
        if (v >= 0x80000000u && v < 0x80010000u)
          MW32(ctx + ro, 0x800F0000u + (v & 0x7FFFFFFFu));
      }
    }
    if (cnt == 0) goto next_block;
    rec = blk + 0xcu;
    for (i = 0; i < cnt; i++) {
      uint32_t rp    = rec;
      uint32_t tag   = MB32(rp);
      uint32_t f4    = MB16(rp + 4u);
      uint32_t param = MB16(rp + 6u) & 0x7fffu;
      uint32_t tagk    = tag & 0x0f04ffffu;  /* generated @0x8016B848-864:
                                          * lui 0x0F04/ori 0xFFFF/and — NOT
                                          * 0x0FFFFFFF; strips flag bits
                                          * 16,17,19-23 so tagged variants
                                          * fold into the base cases */
      uint32_t src   = MB32(ctx + 4u) + MB32(rp + 8u) * 4u;
      rec = rp + 4u + f4 * 4u;      /* next record (stride at end) */
      switch (tagk) {
      /* ---- two-way name cases, ctx+8/ctx+0xc args ---- */
      case 0x00000008u: B7C0_EMIT(mode==1?0x80188d2cu:0x801872ccu); B7C0_CALL7(heap_serialize_tag_08); break;
      case 0x00000009u: B7C0_EMIT(mode==1?0x8018902cu:0x801875fcu); B7C0_CALL7(record_table_expand_a); break;
      case 0x0000000au: B7C0_EMIT(mode==1?0x80189360u:0x80187980u); B7C0_CALL5(heap_serialize_tag_0a); break;
      case 0x0000000cu: B7C0_EMIT(mode==1?0x80188e88u:0x8018743cu); B7C0_CALL5(heap_serialize_tag_0c); break;
      case 0x0000000du: B7C0_EMIT(mode==1?0x801891a4u:0x80187794u); B7C0_CALL7(quad_strip_emit_a); break;
      case 0x0000000eu: B7C0_EMIT(mode==1?0x80189428u:0x80187a48u); B7C0_CALL5(heap_serialize_tag_0e); break;
      case 0x00000010u: B7C0_EMIT(mode==1?0x80188dccu:0x80187378u); B7C0_CALL5(heap_serialize_tag_10); break;
      case 0x00000011u: B7C0_EMIT(mode==1?0x801890dcu:0x801876bcu); B7C0_CALL7(record_table_expand_b); break;
      case 0x00000012u: B7C0_EMIT(mode==1?0x80189500u:0x80187b20u); B7C0_CALL5(heap_serialize_tag_12); break;
      case 0x00000014u: B7C0_EMIT(mode==1?0x80188f40u:0x80187504u); B7C0_CALL5(heap_serialize_tag_14); break;
      case 0x00000015u: B7C0_EMIT(mode==1?0x8018926cu:0x8018786cu); B7C0_CALL7(quad_strip_emit_b); break;
      case 0x00000016u: B7C0_EMIT(mode==1?0x801895f8u:0x80187c10u); B7C0_CALL5(heap_serialize_tag_16); break;
      /* ---- multi-way name cases ---- */
      case 0x00040048u: {   /* heap_serialize_tag_40048, 5 args */
        uint32_t nm;
        if (mode == 3) nm = 0x8018a5acu;
        else if (mode < 4) nm = (mode == 1) ? 0x8018a27cu : 0x80188b5cu;
        else if (mode == 7) nm = 0x8018a5acu;
        else if (mode == 0x81) nm = 0x8018b390u;
        else nm = 0x80188b5cu;
        B7C0_EMIT(nm); B7C0_CALL5(heap_serialize_tag_40048); break; }
      case 0x00040049u: {   /* heap_serialize_tag_40049, 7 args; jt @0x80122164 */
        uint32_t nm = 0x80188b7cu;
        if (mode >= 1 && mode <= 0x23) {
          switch (mode) {
          case 1:  nm = 0x8018a29cu; break;
          case 3:  nm = 0x8018a5ccu; break;
          case 7:  nm = 0x8018a864u; break;
          case 19: nm = 0x8018b894u; break;
          case 35: nm = 0x8018b628u; break;
          }
        }
        B7C0_EMIT(nm); B7C0_CALL7(heap_serialize_tag_40049); break; }
      case 0x0004004cu: {   /* heap_serialize_tag_4004c, 5 args */
        uint32_t nm;
        if (mode == 3) nm = 0x8018a5bcu;
        else if (mode < 4) nm = (mode == 1) ? 0x8018a28cu : 0x80188b6cu;
        else nm = (mode == 7) ? 0x8018a5bcu : 0x80188b6cu;
        B7C0_EMIT(nm); B7C0_CALL5(heap_serialize_tag_4004c); break; }
      case 0x0004004du: {   /* heap_serialize_tag_4004d, 7 args */
        uint32_t nm;
        if (mode == 3) nm = 0x8018a5dcu;
        else if (mode < 4) nm = (mode == 1) ? 0x8018a2acu : 0x80188b8cu;
        else nm = (mode == 7) ? 0x8018a874u : 0x80188b8cu;
        B7C0_EMIT(nm); B7C0_CALL7(heap_serialize_tag_4004d); break; }
      case 0x00040050u: {   /* heap_serialize_tag_40050, 5 args */
        uint32_t nm = 0x80188c34u;
        if (mode == 7) nm = 0x8018a6e4u;
        else if (mode < 8) {
          if (mode == 1) nm = 0x8018a340u;
          else if (mode == 3) nm = 0x8018a6e4u;
        } else {
          if (mode == 0x23) nm = 0x8018b738u;
          else if (mode == 0x81) nm = 0x8018b4c8u;
        }
        B7C0_EMIT(nm); B7C0_CALL5(heap_serialize_tag_40050); break; }
      case 0x00040051u: {   /* heap_serialize_tag_40051, 7 args; jt @0x801221f4 */
        uint32_t nm = 0x80188c54u;
        if (mode >= 1 && mode <= 0x23) {
          switch (mode) {
          case 1:  nm = 0x8018a360u; break;
          case 3:  nm = 0x8018a704u; break;
          case 7:  nm = 0x8018a9e8u; break;
          case 19: nm = 0x8018b98cu; break;
          case 35: nm = 0x8018b748u; break;
          }
        }
        B7C0_EMIT(nm); B7C0_CALL7(heap_serialize_tag_40051); break; }
      case 0x00040054u: {   /* heap_serialize_tag_40054, 5 args */
        uint32_t nm;
        if (mode == 3) nm = 0x8018a6f4u;
        else if (mode < 4) nm = (mode == 1) ? 0x8018a350u : 0x80188c44u;
        else nm = (mode == 7) ? 0x8018a6f4u : 0x80188c44u;
        B7C0_EMIT(nm); B7C0_CALL5(heap_serialize_tag_40054); break; }
      case 0x00040055u: {   /* heap_serialize_tag_40055, 7 args */
        uint32_t nm;
        if (mode == 3) nm = 0x8018a714u;
        else if (mode < 4) nm = (mode == 1) ? 0x8018a370u : 0x80188c64u;
        else nm = (mode == 7) ? 0x8018a9f8u : 0x80188c64u;
        B7C0_EMIT(nm); B7C0_CALL7(heap_serialize_tag_40055); break; }
      /* ---- 0x010000xx family: ctx+0xc / ctx+0x14 args ---- */
      case 0x01000008u: B7C0_EMIT(mode==1?0x80189704u:0x80187e18u); B7C0_CALL5(heap_serialize_tag_01000008); break;
      case 0x01000009u: B7C0_EMIT(mode==1?0x801898a0u:0x80187ffcu); B7C0_CALL7H(heap_serialize_tag_01000009); break;
      case 0x0100000au: B7C0_EMIT(mode==1?0x80189a60u:0x80188200u); B7C0_CALL5H(heap_serialize_tag_0100000a); break;
      case 0x0100000cu: B7C0_EMIT(mode==1?0x801897b0u:0x80187ee0u); B7C0_CALL5H(heap_serialize_tag_0100000c); break;
      case 0x0100000du: B7C0_EMIT(mode==1?0x8018995cu:0x801880d4u); B7C0_CALL7H(heap_serialize_tag_0100000d); break;
      case 0x0100000eu: B7C0_EMIT(mode==1?0x80189b50u:0x80188308u); B7C0_CALL5H(heap_serialize_tag_0100000e); break;
      case 0x01000010u: B7C0_EMIT(mode==1?0x80189c4cu:0x80188434u); B7C0_CALL5H(heap_serialize_tag_01000010); break;
      case 0x01000011u: B7C0_EMIT(mode==1?0x80189e2cu:0x8018866cu); B7C0_CALL7H(heap_serialize_tag_01000011); break;
      case 0x01000012u: B7C0_EMIT(mode==1?0x8018a030u:0x801888c0u); B7C0_CALL5H(heap_serialize_tag_01000012); break;
      case 0x01000014u: B7C0_EMIT(mode==1?0x80189d08u:0x80188510u); B7C0_CALL5H(heap_serialize_tag_01000014); break;
      case 0x01000015u: B7C0_EMIT(mode==1?0x80189efcu:0x80188754u); B7C0_CALL7H(heap_serialize_tag_01000015); break;
      case 0x01000016u: B7C0_EMIT(mode==1?0x8018a14cu:0x801889f4u); B7C0_CALL5H(heap_serialize_tag_01000016); break;
      /* ---- special cases ---- */
      case 0x01000000u: {
        /* conditional double emit + heap_serialize_tag_01000000(cur, rp+4,
         * ctx+8, ctx+0xc, ctx+0x10, ctx+0x14); 6 args */
        uint32_t w0 = MB32(rp + 0x08u);
        uint32_t w1 = MB32(rp + 0x14u);
        if (w0 != 0) {
          scr[emit_n*4u+0u] = tag; scr[emit_n*4u+1u] = w0;
          scr[emit_n*4u+2u] = cur; scr[emit_n*4u+3u] = 0x80187d14u;
          emit_n++;
        }
        if (w1 != 0) {
          scr[emit_n*4u+0u] = tag; scr[emit_n*4u+1u] = w1;
          scr[emit_n*4u+2u] = cur + MB32(rp + 0x08u) * 16u;
          scr[emit_n*4u+3u] = 0x80187dccu;
          emit_n++;
        }
        {
          decomp_trace_record(27,ctx,MB32(ctx+0x14u),0,MB32(rp+0x18u));
          decomp_trace_record(28,rp,cur,0,MB32(ctx+0x10u));
          decomp_trace_record(30,MB32(ctx+4u),MB32(ctx+8u),0,MB32(ctx+0xcu));
          decomp_trace_record(31,MB32(blk+0u),MB32(blk+8u),0,blk); }
        cur = heap_serialize_tag_01000000(cur, rp + 4u,
                           MB32(ctx + 8u), MB32(ctx + 0xcu),
                           MB32(ctx + 0x10u), MB32(ctx + 0x14u));
        break; }
      case 0x02000000u:   /* entry_attr_bump_loop(payload, ctx+4, param, xoff);
                           * guest also passes yoff as unused arg5;
                           * ported signature is 4-arg. ret ignored */
        entry_attr_bump_loop(rp + 8u, MB32(ctx + 4u), param, xoff);
        break;
      case 0x02000001u:   /* dual_entry_attr_bump(payload, ctx+4, ctx+8,
                           * param, xoff, yoff); ret ignored */
        dual_entry_attr_bump(rp + 8u, MB32(ctx + 4u), MB32(ctx + 8u),
                     param, xoff, yoff);
        break;
      default: break;    /* unhandled tags: stride only */
      }
    }
next_block:
    blk = MB32(blk);
    /* Terminators: 0xffffffff sentinel AND null next-ptr (blk==0). A block's
     * +0 next-pointer is 0 at end-of-list; without the blk==0 check the walk
     * reads MB32(0)=0 forever (guest[0]==0 fixed point) -> infinite clean-C
     * loop that never charges cycles -> psx_cycle_count freezes -> all device
     * clocks (CD/DMA/timer) stall -> wedge. blk==0 can never be a valid heap
     * block (blocks live at 0x800Fxxxx+). */
    if (blk == 0xffffffffu || blk == 0u) break;
  }

  MW32(dst + 0x1cu, cur);
  MW32(outp, emit_n);
  MW32(cur, emit_n);
  cur += 4u;
  {
    uint32_t n;
    for (n = 0; n < emit_n; n++) {
      MW32(cur + 0x00u, scr[n*4u+0u]);
      MW32(cur + 0x04u, scr[n*4u+1u]);
      MW32(cur + 0x08u, scr[n*4u+2u]);
      MW32(cur + 0x0cu, scr[n*4u+3u]);
      cur += 0x10u;
    }
  }
  {
    /* tag15 hser: dst=entry blk, src=ret cur, len=emit count emit_n */
    decomp_trace_record(15,entry_blk,(uint32_t)cur,0,emit_n); }
  return (int32_t)cur;

#undef B7C0_EMIT
#undef B7C0_CALL5
#undef B7C0_CALL7
#undef B7C0_CALL5H
#undef B7C0_CALL7H
}

/* Serializer callees — delegated to the generated guest bodies.
 * Args 5+ travel on the guest stack (o32): staged at fr+0x10/+0x14/+0x18
 * via br2_guest_frame + br2_st32, same pattern as game/bind.c.
 * All args are guest u32 values (no host pointers -> no guest_of). */
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t br2_call_clean(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void br2_st32(uint32_t,uint32_t);

#define BR2_TAG5(NM,AD) \
uint32_t NM(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e) \
{ uint32_t fr = br2_guest_frame(0x20); uint32_t r; \
  br2_st32(fr + 0x10, e); \
  r = br2_call_clean(AD,a,b,c,d); \
  br2_guest_frame_free(); return r; }
#define BR2_TAG7(NM,AD) \
uint32_t NM(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, \
             uint32_t f,uint32_t g) \
{ uint32_t fr = br2_guest_frame(0x20); uint32_t r; \
  br2_st32(fr + 0x10, e); br2_st32(fr + 0x14, f); br2_st32(fr + 0x18, g); \
  r = br2_call_clean(AD,a,b,c,d); \
  br2_guest_frame_free(); return r; }

/* Faithful clean ports of the heap_serialize_tag_* serializers
 * (0x801689d0-0x8016b4c4). All share one shape verified against the
 * generated bodies: guard on the stack count arg, preheader bumps the
 * dst cursor +0x10 and src +stride-2, then a count-loop that stamps
 * four tag bytes, copies a u32 stream word, two lw words, and four
 * u16-indexed 8-byte table lookups (first via arg4 table, rest via
 * arg3 table) into the dst record; returns dst + count*stride.
 * lwl/lwr+swl/swr pairs are unaligned LE word accesses == MB32/MW32.
 * lh indices are sign-extended then <<3 (index*8 into the tables). */

/* heap_serialize_tag_08 @0x801689d0 (68 insns, 7 args): dst stride
 * 0x4c, src u32 stream stride 0xc at src+0, u16 index quads at
 * src+4/+6/+8/+10 stride 0xc, src aux ptr src+10 stride 0xc.
 * Tag bytes 4/0x20 at dst+0x27/0x2b/0x3b/0x3f (a0 = cur+0x10).
 * Returns cur + count*0x4c. */
uint32_t heap_serialize_tag_08(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count,uint32_t unused6,
                     uint32_t unused7)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  (void)unused6; (void)unused7;
  if (count == 0) return cur;
  wcur += 16; scur += 10;
  do {
    MW8(wcur + 23, 4);  MW8(wcur + 27, 32);
    MW8(wcur + 43, 4);  MW8(wcur + 47, 32);
    MW32(wcur + 8, MB32(src_cur));
    src_cur += 12;
    tp = (uint32_t)MBS16(scur - 6) << 3;
    tp += ptab2;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(wcur + 12, plo); MW32(wcur + 16, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;
    count -= 1;
    tp += ptab;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(cur + 0, plo);  MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;
    cur += 76;
    tp += ptab;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(wcur - 8, plo);  MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;
    scur += 12;
    tp += ptab;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(wcur + 0, plo);  MW32(wcur + 4, phi);
    wcur += 76;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_0a @0x801697f4 (74 insns, 5 args): dst stride
 * 0x64, src u32 stream stride 0x14 at src+0, two extra u32s at
 * src+4/+8, u16 index quads at src+12/+14/+16/+18 stride 0x14,
 * aux src+18 stride 0x14. Tag bytes 6/0x30 at dst+0x2f/0x33/0x4b/0x4f.
 * Returns cur + count*0x64. */
uint32_t heap_serialize_tag_0a(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  if (count == 0) return cur;
  wcur += 16; scur += 18;
  do {
    MW8(wcur + 31, 6);  MW8(wcur + 35, 48);
    MW8(wcur + 59, 6);  MW8(wcur + 63, 48);
    MW32(wcur + 8,  MB32(src_cur));
    MW32(wcur + 12, MB32(scur - 14));
    MW32(wcur + 16, MB32(scur - 10));
    src_cur += 20;
    tp = (uint32_t)MBS16(scur - 6) << 3;
    tp += ptab2;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(wcur + 20, plo); MW32(wcur + 24, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;
    count -= 1;
    tp += ptab;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(cur + 0, plo);  MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;
    cur += 100;
    tp += ptab;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(wcur - 8, plo);  MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;
    scur += 20;
    tp += ptab;
    plo = MB32(tp);     phi = MB32(tp + 4);
    MW32(wcur + 0, plo);  MW32(wcur + 4, phi);
    wcur += 100;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_0c @0x80168c20 (92 insns, 5 args): dst stride
 * 0x6c, src u32 stream stride 0x10 at src+0, u16 index quint at
 * src+4/+6/+8/+10/+12 stride 0x10, aux src+14 stride 0x10.
 * Six 8-byte table lookups alternating arg4/arg3 tables
 * (-10,-8,-6,-4,-2,0). Tag bytes 6/0x30 at dst+0x37/0x3b/0x5b/0x5f.
 * Returns cur + count*0x6c. */
uint32_t heap_serialize_tag_0c(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  if (count == 0) return cur;
  wcur += 16; scur += 14;
  do {
    MW8(wcur + 39, 6);  MW8(wcur + 43, 48);
    MW8(wcur + 67, 6);  MW8(wcur + 71, 48);
    MW32(wcur + 8, MB32(src_cur));
    src_cur += 16;
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 12, plo); MW32(wcur + 16, phi);
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 20, plo); MW32(wcur + 24, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;  count -= 1; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;  tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 28, plo); MW32(wcur + 32, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;  scur += 16; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    cur += 108;
    wcur += 108;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_10 @0x80168ae0 (80 insns, 5 args): dst stride
 * 0x5c, src u32 stream stride 0x10 at src+0, u16 index quad at
 * src+4/+6/+8/+10 stride 0x10, aux src+12 stride 0x10.
 * Tag bytes 5/0x28 at dst+0x27/0x2b/0x3b/0x3f (a0 = cur+0x18).
 * Returns cur + count*0x5c. */
uint32_t heap_serialize_tag_10(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  if (count == 0) return cur;
  wcur += 24; scur += 12;
  do {
    MW8(wcur + 23, 5);  MW8(wcur + 27, 40);
    MW8(wcur + 47, 5);  MW8(wcur + 51, 40);
    MW32(wcur + 8, MB32(src_cur));
    src_cur += 16;
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 12, plo); MW32(wcur + 16, phi);
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;  count -= 1; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 16, plo); MW32(wcur - 12, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;  cur += 92; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;  scur += 16; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 92;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_14 @0x80168d90 (116 insns, 5 args): dst stride
 * 0x8c, src u32 stream stride 0x14 at src+0, u16 index octet at
 * src+4..+18 stride 0x14, aux src+18 stride 0x14.
 * Eight 8-byte lookups alternating arg4/arg3 tables
 * (-14,-12,-10,-8,-6,-4,-2,0). Tag bytes 8/0x38 at
 * dst+0x3f/0x43/0x6b/0x6f (a0 = cur+0x18). Returns cur + count*0x8c. */
uint32_t heap_serialize_tag_14(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  if (count == 0) return cur;
  wcur += 24; scur += 18;
  do {
    MW8(wcur + 47, 8);  MW8(wcur + 51, 56);
    MW8(wcur + 83, 8);  MW8(wcur + 87, 56);
    MW32(wcur + 8, MB32(src_cur));
    src_cur += 20;
    tp = (uint32_t)MBS16(scur - 14) << 3; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 12, plo); MW32(wcur + 16, phi);
    tp = (uint32_t)MBS16(scur - 12) << 3; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 20, plo); MW32(wcur + 24, phi);
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 16, plo); MW32(wcur - 12, phi);
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 28, plo); MW32(wcur + 32, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;  count -= 1; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;  cur += 140; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 36, plo); MW32(wcur + 40, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;  scur += 20; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 140;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_12 @0x80169aa4 (89 insns, 5 args): dst stride
 * 0x80, src u32 stream stride 0x1c at src+0 plus three u32s at
 * src+4/+8/+12, u16 index quad at src+16/+18/+20/+22 stride 0x1c,
 * aux src+24 stride 0x1c. Tag bytes 8/0x38 at dst+0x33/0x37/0x57/0x5b
 * (a0 = cur+0x18). Returns cur + count*0x80. */
uint32_t heap_serialize_tag_12(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  if (count == 0) return cur;
  wcur += 24; scur += 24;
  do {
    MW8(wcur + 35, 8);  MW8(wcur + 39, 56);
    MW8(wcur + 71, 8);  MW8(wcur + 75, 56);
    MW32(wcur + 8,  MB32(src_cur));
    MW32(wcur + 12, MB32(scur - 20));
    MW32(wcur + 16, MB32(scur - 16));
    MW32(wcur + 20, MB32(scur - 12));
    src_cur += 28;
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 24, plo); MW32(wcur + 28, phi);
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;  count -= 1; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 16, plo); MW32(wcur - 12, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;  cur += 128; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;  scur += 28; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 128;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_16 @0x80169c08 (125 insns, 5 args): dst stride
 * 0x98, src u32 stream stride 0x20 at src+0 plus three u32s at
 * src+4/+8/+12, u16 index octet at src+16..+30 stride 0x20,
 * aux src+30 stride 0x20. Eight 8-byte lookups alternating
 * arg4/arg3 tables (-14..0 step 2). Tag bytes 8/0x38 at
 * dst+0x3b/0x3f/0x5f/0x63 (a0 = cur+0x18). Returns cur + count*0x98. */
uint32_t heap_serialize_tag_16(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  if (count == 0) return cur;
  wcur += 24; scur += 30;
  do {
    MW8(wcur + 59, 8);  MW8(wcur + 63, 56);
    MW8(wcur + 95, 8);  MW8(wcur + 99, 56);
    MW32(wcur + 8,  MB32(src_cur));
    MW32(wcur + 12, MB32(scur - 26));
    MW32(wcur + 16, MB32(scur - 22));
    MW32(wcur + 20, MB32(scur - 18));
    src_cur += 32;
    tp = (uint32_t)MBS16(scur - 14) << 3; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 24, plo); MW32(wcur + 28, phi);
    tp = (uint32_t)MBS16(scur - 12) << 3; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 32, plo); MW32(wcur + 36, phi);
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 16, plo); MW32(wcur - 12, phi);
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 40, plo); MW32(wcur + 44, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;  count -= 1; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;  cur += 152; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 48, plo); MW32(wcur + 52, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;  scur += 32; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 152;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_0e @0x8016991c (98 insns, 5 args): dst stride
 * 0x74, src u32 stream stride 0x18 at src+0 plus two u32s at
 * src+4/+8, u16 index sextet at src+12..+22 stride 0x18,
 * aux src+22 stride 0x18. Six 8-byte lookups alternating
 * arg4/arg3 tables (-10..0 step 2). Tag bytes 6/0x30 at
 * dst+0x2f/0x33/0x4b/0x4f (a0 = cur+0x10). Returns cur + count*0x74. */
uint32_t heap_serialize_tag_0e(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;

  uint32_t src_cur = scur;
  uint32_t tp, plo, phi;
  if (count == 0) return cur;
  wcur += 16; scur += 22;
  do {
    MW8(wcur + 47, 6);  MW8(wcur + 51, 48);
    MW8(wcur + 75, 6);  MW8(wcur + 79, 48);
    MW32(wcur + 8,  MB32(src_cur));
    MW32(wcur + 12, MB32(scur - 18));
    MW32(wcur + 16, MB32(scur - 14));
    src_cur += 24;
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 20, plo); MW32(wcur + 24, phi);
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 28, plo); MW32(wcur + 32, phi);
    tp = (uint32_t)MBS16(scur - 4) << 3;  count -= 1; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    tp = (uint32_t)MBS16(scur - 2) << 3;  cur += 116; tp += ptab2;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 36, plo); MW32(wcur + 40, phi);
    tp = (uint32_t)MBS16(scur + 0) << 3;  scur += 24; tp += ptab;
    plo = MB32(tp); phi = MB32(tp + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 116;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_40048 @0x80169dfc (57 insns, 5 args): dst
 * stride 0x40, src u32 stream stride 0xc at src+0 (dual-stored to
 * dst+0xc/+0x20 region), u16 index triple at src+4/+6/+8 stride 0xc,
 * aux src+8. Three 8-byte lookups all via arg3 table (-4,-2,0);
 * first lands at the a3 cursor pair, a3 += 0x40 per record.
 * Tag bytes 4/0x20 at dst+0xb/0xf/0x1f/0x23 (a0 = cur+0x10).
 * Returns cur + count*0x40. */
uint32_t heap_serialize_tag_40048(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count)
{



  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t v0, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 16; scur += 8;
  do {
    MW8(wcur + 11, 4);  MW8(wcur + 15, 32);
    MW8(wcur + 31, 4);  MW8(wcur + 35, 32);
    v0 = MB32(src_cur); src_cur += 12;
    MW32(wcur + 32, v0); MW32(wcur + 12, v0);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    cur += 64;
    v0 = (uint32_t)MBS16(scur - 2) << 3; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 12; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 64;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_4004c @0x80169ee0 (65 insns, 5 args): dst
 * stride 0x50, src u32 stream stride 0x14 at src+0 plus two u32s at
 * src+4/+8 (each dual-stored), u16 index triple at src+12/+14/+16
 * stride 0x14, aux src+16. Three 8-byte lookups via arg3 table
 * (-4,-2,0); first at a3 pair, a3 += 0x50.
 * Tag bytes 6/0x30 at dst+0xb/0xf/0x27/0x2b (a0 = cur+0x10).
 * Returns cur + count*0x50. */
uint32_t heap_serialize_tag_4004c(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count)
{



  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t v0, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 16; scur += 16;
  do {
    MW8(wcur + 11, 6);  MW8(wcur + 15, 48);
    MW8(wcur + 39, 6);  MW8(wcur + 43, 48);
    v0 = MB32(src_cur); MW32(wcur + 40, v0); MW32(wcur + 12, v0);
    v0 = MB32(scur - 12); MW32(wcur + 48, v0); MW32(wcur + 20, v0);
    v0 = MB32(scur - 8);  src_cur += 20;
    MW32(wcur + 56, v0); MW32(wcur + 28, v0);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    cur += 80;
    v0 = (uint32_t)MBS16(scur - 2) << 3; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 20; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 80;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_40050 @0x80169fe4 (69 insns, 5 args): dst
 * stride 0x50, src u32 stream stride 0xc at src+0 (dual-stored),
 * u16 index quadruple at src+4/+6/+8/+10 stride 0xc, aux src+10.
 * Four 8-byte lookups via arg3 table (-6,-4,-2,0); first at a3
 * pair, a3 += 0x50. Tag bytes 5/0x28 at dst+0xb/0xf/0x23/0x27
 * (a0 = cur+0x18). Returns cur + count*0x50. */
uint32_t heap_serialize_tag_40050(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count)
{



  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t v0, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 24; scur += 10;
  do {
    MW8(wcur + 11, 5);  MW8(wcur + 15, 40);
    MW8(wcur + 35, 5);  MW8(wcur + 39, 40);
    v0 = MB32(src_cur); src_cur += 12;
    MW32(wcur + 36, v0); MW32(wcur + 12, v0);
    v0 = (uint32_t)MBS16(scur - 6) << 3; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 16, plo); MW32(wcur - 12, phi);
    v0 = (uint32_t)MBS16(scur - 2) << 3; cur += 80; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 12; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 80;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_40054 @0x8016a0f8 (81 insns, 5 args): dst
 * stride 0x68, src u32 stream stride 0x18 at src+0 plus three u32s
 * at src+4/+8/+12 (each dual-stored), u16 index quadruple at
 * src+16/+18/+20/+22 stride 0x18, aux src+22.
 * Four 8-byte lookups via arg3 table (-6,-4,-2,0); first at a3
 * pair, a3 += 0x68. Tag bytes 8/0x38 at dst+0xb/0xf/0x2f/0x33
 * (a0 = cur+0x18). Returns cur + count*0x68. */
uint32_t heap_serialize_tag_40054(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count)
{



  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t v0, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 24; scur += 22;
  do {
    MW8(wcur + 11, 8);  MW8(wcur + 15, 56);
    MW8(wcur + 47, 8);  MW8(wcur + 51, 56);
    v0 = MB32(src_cur);      MW32(wcur + 48, v0); MW32(wcur + 12, v0);
    v0 = MB32(scur - 18); MW32(wcur + 56, v0); MW32(wcur + 20, v0);
    v0 = MB32(scur - 14); MW32(wcur + 64, v0); MW32(wcur + 28, v0);
    v0 = MB32(scur - 10); src_cur += 24;
    MW32(wcur + 72, v0); MW32(wcur + 36, v0);
    v0 = (uint32_t)MBS16(scur - 6) << 3; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 16, plo); MW32(wcur - 12, phi);
    v0 = (uint32_t)MBS16(scur - 2) << 3; cur += 104; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 8, plo); MW32(wcur - 4, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 24; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur + 0, plo); MW32(wcur + 4, phi);
    wcur += 104;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_40049 @0x8016a23c (82 insns, 7 args): dst
 * stride 0x58, src u32 stream stride 0x14 at src+0 plus u32s at
 * src+4/+8 and a u16 at src+12 (all dual-stored), u16 index triple
 * at src+12/+14/+16 stride 0x14, aux src+18.
 * Three 8-byte lookups via arg3 table (-4,-2,0); first at the t0
 * cursor pair, t0 += 0x58. Tail adds arg6 to the two x u16s and
 * arg7 to the two y u16s (a0-0x18/+8 x, a0-0x20/+0 y).
 * Tag bytes 7/0x24 at dst+0x1b/0x1f/0x3b/0x3f (a0 = cur+0x46).
 * Returns cur + count*0x58. */
uint32_t heap_serialize_tag_40049(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{



  uint32_t cur = wcur;



  uint32_t src_cur = scur;
  uint32_t v0, cv, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 70; scur += 18;
  do {
    MW8(wcur - 43, 7);  MW8(wcur - 39, 36);
    MW8(wcur - 11, 7);  MW8(wcur - 7, 36);
    v0 = MB32(src_cur); MW32(wcur - 42, v0); MW32(wcur - 10, v0);
    v0 = MB32(scur - 14); MW32(wcur - 34, v0); MW32(wcur - 2, v0);
    v0 = MB32(scur - 10); MW32(wcur - 26, v0); MW32(wcur + 6, v0);
    v0 = MB16(scur - 6);  src_cur += 20;
    MW32(wcur - 18, v0); MW32(wcur + 14, v0);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    v0 = (uint32_t)MBS16(scur - 2) << 3; cur += 88; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 62, plo); MW32(wcur - 58, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 20; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 54, plo); MW32(wcur - 50, phi);
    v0 = MB16(wcur - 24); cv = MB16(wcur - 32);
    v0 += xoff; MW16(wcur - 24, v0);
    v0 = MB16(wcur + 8);  cv += yoff; MW16(wcur - 32, cv);
    cv = MB16(wcur + 0);
    v0 += xoff; cv += yoff;
    MW16(wcur + 8, v0);  MW16(wcur + 0, cv);
    wcur += 88;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_4004d @0x8016a50c (90 insns, 7 args): dst
 * stride 0x68, src u32 stream stride 0x1c at src+0 plus four u32s
 * at src+4..+16 and a u16 at src+20 (all dual-stored), u16 index
 * triple at src+22/+24/+26 stride 0x1c, aux src+26.
 * Three 8-byte lookups via arg3 table (-4,-2,0); first at the t0
 * cursor pair, t0 += 0x68. Tail adds arg6/arg7 to the coord u16s
 * (a0-0x1c/+0xc x, a0-0x28/+0 y). Tag bytes 9/0x34 at
 * dst+0x1b/0x1f/0x43/0x47 (a0 = cur+0x4e). Returns cur + count*0x68. */
uint32_t heap_serialize_tag_4004d(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{



  uint32_t cur = wcur;



  uint32_t src_cur = scur;
  uint32_t v0, cv, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 78; scur += 26;
  do {
    MW8(wcur - 51, 9);  MW8(wcur - 47, 52);
    MW8(wcur - 11, 9);  MW8(wcur - 7, 52);
    v0 = MB32(src_cur);      MW32(wcur - 50, v0); MW32(wcur - 10, v0);
    v0 = MB32(scur - 22); MW32(wcur - 38, v0); MW32(wcur + 2, v0);
    v0 = MB32(scur - 18); MW32(wcur - 26, v0); MW32(wcur + 14, v0);
    v0 = MB32(scur - 14); MW32(wcur - 42, v0); MW32(wcur - 2, v0);
    v0 = MB32(scur - 10); MW32(wcur - 30, v0); MW32(wcur + 10, v0);
    v0 = MB16(scur - 6);  src_cur += 28;
    MW32(wcur - 18, v0); MW32(wcur + 22, v0);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    v0 = (uint32_t)MBS16(scur - 2) << 3; cur += 104; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 70, plo); MW32(wcur - 66, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 28; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 62, plo); MW32(wcur - 58, phi);
    v0 = MB16(wcur - 28); cv = MB16(wcur - 40);
    v0 += xoff; cv += yoff;
    MW16(wcur - 28, v0); MW16(wcur - 40, cv);
    v0 = MB16(wcur + 12); cv = MB16(wcur + 0);
    v0 += xoff; cv += yoff;
    MW16(wcur + 12, v0); MW16(wcur + 0, cv);
    wcur += 104;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_40051 @0x8016a384 (98 insns, 7 args): dst
 * stride 0x70, src u32 stream stride 0x18 at src+0 plus three u32s
 * at src+4/+8/+12 and two u16s at src+16/+20 (all dual-stored),
 * u16 index quadruple at src+14..+22 stride 0x18, aux src+22.
 * Four 8-byte lookups via arg3 table (-8,-4,-2,0); first at the
 * t0 cursor pair, t0 += 0x70. Tail adds arg6/arg7 to coord u16s
 * (a0-0x20/+8 x, a0-0x28/+0 y). Tag bytes 9/0x2c at
 * dst+0x1b/0x1f/0x43/0x47 (a0 = cur+0x56). Returns cur + count*0x70. */
uint32_t heap_serialize_tag_40051(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{



  uint32_t cur = wcur;



  uint32_t src_cur = scur;
  uint32_t v0, cv, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 86; scur += 22;
  do {
    MW8(wcur - 51, 9);  MW8(wcur - 47, 44);
    MW8(wcur - 11, 9);  MW8(wcur - 7, 44);
    v0 = MB32(src_cur);      MW32(wcur - 50, v0); MW32(wcur - 10, v0);
    v0 = MB32(scur - 18); MW32(wcur - 42, v0); MW32(wcur - 2, v0);
    v0 = MB32(scur - 14); MW32(wcur - 34, v0); MW32(wcur + 6, v0);
    v0 = MB16(scur - 10); MW32(wcur - 26, v0); MW32(wcur + 14, v0);
    v0 = MB16(scur - 6);  src_cur += 24;
    MW32(wcur - 18, v0); MW32(wcur + 22, v0);
    v0 = (uint32_t)MBS16(scur - 8) << 3; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 78, plo); MW32(wcur - 74, phi);
    v0 = (uint32_t)MBS16(scur - 2) << 3; cur += 112; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 70, plo); MW32(wcur - 66, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 24; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 62, plo); MW32(wcur - 58, phi);
    v0 = MB16(wcur - 32); cv = MB16(wcur - 40);
    v0 += xoff; cv += yoff;
    MW16(wcur - 32, v0); MW16(wcur - 40, cv);
    v0 = MB16(wcur + 8);  cv = MB16(wcur + 0);
    v0 += xoff; cv += yoff;
    MW16(wcur + 8, v0);   MW16(wcur + 0, cv);
    wcur += 112;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_40055 @0x8016a674 (110 insns, 7 args): dst
 * stride 0x88, src u32 stream stride 0x24 at src+0 plus five u32s
 * at src+4..+20 and two u16s at src+24/+28 (all dual-stored),
 * u16 index quadruple at src+26..+34 stride 0x24, aux src+34.
 * Four 8-byte lookups via arg3 table (-8,-4,-2,0); first at the
 * t0 cursor pair, t0 += 0x88. Tail adds arg6 to the two x u16s
 * (a0-0x28/+0xc) and arg7 to the two y u16s (a0-0x34/+0).
 * Tag bytes 12/0x3c at dst+0x1b/0x1f/0x57/0x5b (a0 = cur+0x62).
 * Returns cur + count*0x88. */
uint32_t heap_serialize_tag_40055(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t unused4,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{



  uint32_t cur = wcur;



  uint32_t src_cur = scur;
  uint32_t v0, cv, plo, phi;
  (void)unused4;
  if (count == 0) return cur;
  wcur += 98; scur += 34;
  do {
    MW8(wcur - 63, 12);  MW8(wcur - 59, 60);
    MW8(wcur - 11, 12);  MW8(wcur - 7, 60);
    v0 = MB32(src_cur);      MW32(wcur - 62, v0); MW32(wcur - 10, v0);
    v0 = MB32(scur - 30); MW32(wcur - 50, v0); MW32(wcur + 2, v0);
    v0 = MB32(scur - 26); MW32(wcur - 38, v0); MW32(wcur + 14, v0);
    v0 = MB32(scur - 22); MW32(wcur - 26, v0); MW32(wcur + 26, v0);
    v0 = MB32(scur - 18); MW32(wcur - 54, v0); MW32(wcur - 2, v0);
    v0 = MB32(scur - 14); MW32(wcur - 42, v0); MW32(wcur + 10, v0);
    v0 = MB16(scur - 10); MW32(wcur - 30, v0); MW32(wcur + 22, v0);
    v0 = MB16(scur - 6);  src_cur += 36;
    MW32(wcur - 18, v0); MW32(wcur + 34, v0);
    v0 = (uint32_t)MBS16(scur - 8) << 3; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(cur + 0, plo); MW32(cur + 4, phi);
    v0 = (uint32_t)MBS16(scur - 4) << 3; count -= 1; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 90, plo); MW32(wcur - 86, phi);
    v0 = (uint32_t)MBS16(scur - 2) << 3; cur += 136; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 82, plo); MW32(wcur - 78, phi);
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 36; v0 += ptab;
    plo = MB32(v0); phi = MB32(v0 + 4);
    MW32(wcur - 74, plo); MW32(wcur - 70, phi);
    v0 = MB16(wcur - 40); cv = MB16(wcur - 52);
    v0 += xoff; MW16(wcur - 40, v0);
    v0 = MB16(wcur + 12); cv += yoff; MW16(wcur - 52, cv);
    cv = MB16(wcur + 0);
    v0 += xoff; cv += yoff;
    MW16(wcur + 12, v0); MW16(wcur + 0, cv);
    wcur += 136;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000008 @0x8016a8ec (48 insns, 5 args): dst
 * stride 0x3c, src u32 stream stride 0xc at src+0, u16 index quad
 * at src+4/+6/+8/+10 stride 0xc, aux src+10.
 * Four single-word lookups: lh idx<<3 +table -> lw -> sw
 * (arg4 table for -6, arg3 table for -4/-2/0). Tag bytes 4/0x20 at
 * dst+0xf/0x13/0x23/0x27 (a0 = cur+8). Returns cur + count*0x3c. */
uint32_t heap_serialize_tag_01000008(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 8; scur += 10;
  do {
    MW8(wcur + 15, 4);  MW8(wcur + 19, 32);
    MW8(wcur + 35, 4);  MW8(wcur + 39, 32);
    MW32(wcur + 4, MB32(src_cur));
    tp = (uint32_t)MBS16(scur - 6) << 3; tp += ptab2;
    MW32(wcur + 8, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3; tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3; src_cur += 12; tp += ptab;
    count -= 1;
    MW32(wcur - 4, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3; cur += 60; scur += 12; tp += ptab;
    MW32(wcur + 0, MB32(tp));
    wcur += 60;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_0100000c @0x8016a9ac (62 insns, 5 args): dst
 * stride 0x54, src u32 stream stride 0x10 at src+0, u16 index
 * quint at src+4..+12 stride 0x10, aux src+14. Six single-word
 * lookups alternating arg4/arg3 tables (-10,-8,-6,-4,-2,0).
 * Tag bytes 6/0x30 stamped at loop tail at dst+0x1f/0x23/0x3b/0x3f
 * (a0 = cur+0x3f). Returns cur + count*0x54. */
uint32_t heap_serialize_tag_0100000c(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 63; scur += 14;
  do {
    MW32(wcur - 51, MB32(src_cur));
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    MW32(wcur - 47, MB32(tp));
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    MW32(wcur - 43, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3;  tp += ptab;
    MW32(wcur - 59, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3;  src_cur += 16; tp += ptab2;
    count -= 1;
    MW32(wcur - 39, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3;  cur += 84; scur += 16; tp += ptab;
    MW32(wcur - 55, MB32(tp));
    MW8(wcur - 32, 6);  MW8(wcur - 28, 48);
    MW8(wcur - 4, 6);   MW8(wcur + 0, 48);
    wcur += 84;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_0100000a @0x8016ad04 (54 insns, 5 args): dst
 * stride 0x54, src u32 stream stride 0x14 at src+0 plus two u32s
 * at src+4/+8, u16 index quad at src+12/+14/+16/+18 stride 0x14,
 * aux src+18. Four single-word lookups (arg4 for -6, arg3 for
 * -4/-2/0). Tag bytes 6/0x30 at dst+0x1f/0x23/0x37/0x3b
 * (a0 = cur+8). Returns cur + count*0x54. */
uint32_t heap_serialize_tag_0100000a(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 8; scur += 18;
  do {
    MW8(wcur + 23, 6);  MW8(wcur + 27, 48);
    MW8(wcur + 51, 6);  MW8(wcur + 55, 48);
    MW32(wcur + 4, MB32(src_cur));
    MW32(wcur + 8,  MB32(scur - 14));
    MW32(wcur + 12, MB32(scur - 10));
    tp = (uint32_t)MBS16(scur - 6) << 3; tp += ptab2;
    MW32(wcur + 16, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3; tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3; src_cur += 20; tp += ptab;
    count -= 1;
    MW32(wcur - 4, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3; cur += 84; scur += 20; tp += ptab;
    MW32(wcur + 0, MB32(tp));
    wcur += 84;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_0100000e @0x8016addc (68 insns, 5 args): dst
 * stride 0x5c, src u32 stream stride 0x18 at src+0 plus two u32s
 * at src+4/+8, u16 index sextet at src+12..+22 stride 0x18,
 * aux src+22. Six single-word lookups alternating arg4/arg3
 * tables (-10,-8,-6,-4,-2,0). Tag bytes 6/0x30 at
 * dst+0x1f/0x23/0x3b/0x3f (a0 = cur+8). Returns cur + count*0x5c. */
uint32_t heap_serialize_tag_0100000e(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 8; scur += 22;
  do {
    MW8(wcur + 31, 6);  MW8(wcur + 35, 48);
    MW8(wcur + 59, 6);  MW8(wcur + 63, 48);
    MW32(wcur + 4,  MB32(src_cur));
    MW32(wcur + 8,  MB32(scur - 18));
    MW32(wcur + 12, MB32(scur - 14));
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    MW32(wcur + 16, MB32(tp));
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    MW32(wcur + 20, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3;  tp += ptab;
    MW32(wcur - 4, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3;  src_cur += 24; tp += ptab2;
    count -= 1;
    MW32(wcur + 24, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3;  cur += 92; scur += 24; tp += ptab;
    MW32(wcur + 0, MB32(tp));
    wcur += 92;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000010 @0x8016aeec (55 insns, 5 args): dst
 * stride 0x48, src u32 stream stride 0x10 at src+0, u16 index
 * quint at src+4..+12 stride 0x10, aux src+12.
 * Five single-word lookups (arg4 for -8, arg3 for -6/-4/-2/0).
 * Tag bytes 5/0x28 at dst+0xf/0x13/0x27/0x2b (a0 = cur+0xc).
 * Returns cur + count*0x48. */
uint32_t heap_serialize_tag_01000010(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 12; scur += 12;
  do {
    MW8(wcur + 15, 5);  MW8(wcur + 19, 40);
    MW8(wcur + 39, 5);  MW8(wcur + 43, 40);
    MW32(wcur + 4, MB32(src_cur));
    tp = (uint32_t)MBS16(scur - 8) << 3; tp += ptab2;
    MW32(wcur + 8, MB32(tp));
    tp = (uint32_t)MBS16(scur - 6) << 3; tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3; tp += ptab;
    MW32(wcur - 8, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3; src_cur += 16; tp += ptab;
    count -= 1;
    MW32(wcur - 4, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3; cur += 72; scur += 16; tp += ptab;
    MW32(wcur + 0, MB32(tp));
    wcur += 72;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000014 @0x8016afc8 (76 insns, 5 args): dst
 * stride 0x6c, src u32 stream stride 0x14 at src+0, u16 index
 * octet at src+4..+18 stride 0x14, aux src+18.
 * Eight single-word lookups alternating arg4/arg3 tables
 * (-14..0 step 2). Tag bytes 8/0x38 at dst+0x1b/0x1f/0x3f/0x43
 * (a0 = cur+0xc). Returns cur + count*0x6c. */
uint32_t heap_serialize_tag_01000014(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 12; scur += 18;
  do {
    MW8(wcur + 27, 8);  MW8(wcur + 31, 56);
    MW8(wcur + 63, 8);  MW8(wcur + 67, 56);
    MW32(wcur + 4, MB32(src_cur));
    tp = (uint32_t)MBS16(scur - 14) << 3; tp += ptab2;
    MW32(wcur + 8, MB32(tp));
    tp = (uint32_t)MBS16(scur - 12) << 3; tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    MW32(wcur + 12, MB32(tp));
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    MW32(wcur - 8, MB32(tp));
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    MW32(wcur + 16, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3;  tp += ptab;
    MW32(wcur - 4, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3;  src_cur += 20; tp += ptab2;
    count -= 1;
    MW32(wcur + 20, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3;  cur += 108; scur += 20; tp += ptab;
    MW32(wcur + 0, MB32(tp));
    wcur += 108;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000012 @0x8016b3c4 (64 insns, 5 args): dst
 * stride 0x6c, src u32 stream stride 0x1c at src+0 plus three u32s
 * at src+4/+8/+12, u16 index quad at src+16/+18/+20/+22 stride
 * 0x1c, aux src+24. Five single-word lookups (arg4 for -8, arg3
 * for -6/-4/-2/0). Tag bytes 8/0x38 at dst+0x1b/0x1f/0x3f/0x43
 * (a0 = cur+0xc). Returns cur + count*0x6c. */
uint32_t heap_serialize_tag_01000012(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 12; scur += 24;
  do {
    MW8(wcur + 27, 8);  MW8(wcur + 31, 56);
    MW8(wcur + 63, 8);  MW8(wcur + 67, 56);
    MW32(wcur + 4,  MB32(src_cur));
    MW32(wcur + 8,  MB32(scur - 20));
    MW32(wcur + 12, MB32(scur - 16));
    MW32(wcur + 16, MB32(scur - 12));
    tp = (uint32_t)MBS16(scur - 8) << 3; tp += ptab2;
    MW32(wcur + 20, MB32(tp));
    tp = (uint32_t)MBS16(scur - 6) << 3; tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3; tp += ptab;
    MW32(wcur - 8, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3; src_cur += 28; tp += ptab;
    count -= 1;
    MW32(wcur - 4, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3; cur += 108; scur += 28; tp += ptab;
    MW32(wcur + 0, MB32(tp));
    wcur += 108;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000016 @0x8016b4c4 (85 insns, 5 args): dst
 * stride 0x78, src u32 stream stride 0x20 at src+0 plus three u32s
 * at src+4/+8/+12, u16 index octet at src+16..+30 stride 0x20,
 * aux src+30. Eight single-word lookups alternating arg4/arg3
 * tables (-14..0 step 2). Tag bytes 8/0x38 at dst+0x27/0x2b/0x4b/0x4f
 * (a0 = cur+0xc). Returns cur + count*0x78. */
uint32_t heap_serialize_tag_01000016(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;

  uint32_t tp;
  if (count == 0) return cur;
  wcur += 12; scur += 30;
  do {
    MW8(wcur + 39, 8);  MW8(wcur + 43, 56);
    MW8(wcur + 75, 8);  MW8(wcur + 79, 56);
    MW32(wcur + 4,  MB32(src_cur));
    MW32(wcur + 8,  MB32(scur - 26));
    MW32(wcur + 12, MB32(scur - 22));
    MW32(wcur + 16, MB32(scur - 18));
    tp = (uint32_t)MBS16(scur - 14) << 3; tp += ptab2;
    MW32(wcur + 20, MB32(tp));
    tp = (uint32_t)MBS16(scur - 12) << 3; tp += ptab;
    MW32(cur + 0, MB32(tp));
    tp = (uint32_t)MBS16(scur - 10) << 3; tp += ptab2;
    MW32(wcur + 24, MB32(tp));
    tp = (uint32_t)MBS16(scur - 8) << 3;  tp += ptab;
    MW32(wcur - 8, MB32(tp));
    tp = (uint32_t)MBS16(scur - 6) << 3;  tp += ptab2;
    MW32(wcur + 28, MB32(tp));
    tp = (uint32_t)MBS16(scur - 4) << 3;  tp += ptab;
    MW32(wcur - 4, MB32(tp));
    tp = (uint32_t)MBS16(scur - 2) << 3;  src_cur += 32; tp += ptab2;
    count -= 1;
    MW32(wcur + 32, MB32(tp));
    tp = (uint32_t)MBS16(scur + 0) << 3;  cur += 120; scur += 32; tp += ptab;
    MW32(wcur + 0, MB32(tp));
    wcur += 120;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000009 @0x8016aaa4 (68 insns, 7 args): dst
 * stride 0x50, src u32 stream stride 0x14 at src+0 plus u32s at
 * src+4/+8 (all dual-stored), u16 index quad at src+12..+18
 * stride 0x14, aux src+18. Four single-word lookups (arg4 for -6,
 * arg3 for -4/-2/0). Tail adds param_6 to x u16s (t0-0x18/+8) and
 * param_7 to y u16s (t0-0x20/+0). Tag bytes 7/0x24 at
 * dst+0x13/0x17/0x33/0x37 (t0 = cur+0x3e). Returns cur + count*0x50. */
uint32_t heap_serialize_tag_01000009(uint32_t a0,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{




  uint32_t cur = a0;
  uint32_t src_cur = scur;



  uint32_t cur2 = a0 + 62;
  uint32_t v0;
  if (count == 0) return cur;
  scur += 18;
  do {
    MW8(cur2 - 43, 7);  MW8(cur2 - 39, 36);
    MW8(cur2 - 11, 7);  MW8(cur2 - 7, 36);
    v0 = MB32(src_cur); MW32(cur2 - 34, v0); MW32(cur2 - 2, v0);
    v0 = MB32(scur - 14); MW32(cur2 - 26, v0); MW32(cur2 + 6, v0);
    v0 = MB32(scur - 10); src_cur += 20;
    MW32(cur2 - 18, v0); MW32(cur2 + 14, v0);
    v0 = (uint32_t)MBS16(scur - 6) << 3; v0 += ptab2;
    MW32(cur2 - 50, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 4) << 3; v0 += ptab;
    MW32(cur + 0, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 2) << 3; v0 += ptab;
    count -= 1;
    cur += 80;
    MW32(cur2 - 58, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 20; v0 += ptab;
    a0 = MB32(v0);
    MW32(cur2 - 54, a0);
    { uint32_t c0 = MB16(cur2 - 32), c1 = MB16(cur2 - 24);
      c0 += yoff; MW16(cur2 - 32, c0);
      c1 += xoff; MW16(cur2 - 24, c1);
      c0 = MB16(cur2 + 0);  c1 = MB16(cur2 + 8);
      c0 += yoff; c1 += xoff;
      MW16(cur2 + 0, c0);   MW16(cur2 + 8, c1); }
    cur2 += 80;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_0100000d @0x8016abb4 (84 insns, 7 args): dst
 * stride 0x68, src u32 stream stride 0x18 at src+0 plus u32s at
 * src+4/+8 (all dual-stored), u16 index sextet at src+12..+22
 * stride 0x18 (positive lh offsets), aux src+8/+32.
 * Six single-word lookups alternating arg4/arg3 tables
 * (+4,+6,+8,+10,+12,+14). Tail adds param_6 to x u16s (a0-0x1c/+0xc)
 * and param_7 to y u16s (a0-0x28/+0). Tag bytes 9/0x34 at
 * dst+0x1b/0x1f/0x43/0x47 (a0 = cur+0x4e). Returns cur + count*0x68. */
uint32_t heap_serialize_tag_0100000d(uint32_t wcur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{




  uint32_t cur = wcur;
  uint32_t src_cur = scur;



  uint32_t v0, v1;
  if (count == 0) return cur;
  wcur += 78; scur += 8;
  do {
    v0 = (uint32_t)MBS16(scur + 4) << 3;  v0 += ptab2;
    MW32(wcur - 66, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 6) << 3;  v0 += ptab;
    MW32(cur + 0, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 8) << 3;  v0 += ptab2;
    MW32(wcur - 62, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 10) << 3; v0 += ptab;
    MW32(wcur - 74, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 12) << 3; v0 += ptab2;
    MW32(wcur - 58, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 14) << 3; v0 += ptab;
    count -= 1;
    MW32(wcur - 70, MB32(v0));
    v0 = MB32(src_cur); cur += 104; src_cur += 24;
    MW32(wcur - 42, v0); MW32(wcur - 2, v0);
    v0 = MB32(scur - 4); v1 = MB16(wcur - 40);
    MW32(wcur - 30, v0); MW32(wcur + 10, v0);
    v0 = MB32(scur + 0); scur += 24;
    MW32(wcur - 18, v0); MW32(wcur + 22, v0);
    v0 = MB16(wcur - 28); v0 += xoff; MW16(wcur - 28, v0);
    v1 += yoff; MW16(wcur - 40, v1);
    v0 = MB16(wcur + 12); v1 = MB16(wcur + 0);
    v0 += xoff; v1 += yoff;
    MW16(wcur + 12, v0); MW16(wcur + 0, v1);
    MW8(wcur - 51, 9);  MW8(wcur - 47, 52);
    MW8(wcur - 11, 9);  MW8(wcur - 7, 52);
    wcur += 104;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000011 @0x8016b0f8 (79 insns, 7 args): dst
 * stride 0x64, src u32 stream stride 0x18 at src+0 plus three u32s
 * at src+4/+8/+12 (all dual-stored), u16 index quintet at
 * src+14..+22 stride 0x18, aux src+22.
 * Five single-word lookups (arg4 for -8, arg3 for -6/-4/-2/0).
 * Tail adds param_6 to x u16s (t0-0x20/+8) and param_7 to y u16s
 * (t0-0x28/+0). Tag bytes 9/0x2c at dst+0x1b/0x1f/0x43/0x47
 * (t0 = cur+0x4a). Returns cur + count*0x64. */
uint32_t heap_serialize_tag_01000011(uint32_t cur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{




  uint32_t src_cur = scur;



  uint32_t cur2 = cur + 74;
  uint32_t v0;
  if (count == 0) return cur;
  scur += 22;
  do {
    MW8(cur2 - 51, 9);  MW8(cur2 - 47, 44);
    MW8(cur2 - 11, 9);  MW8(cur2 - 7, 44);
    v0 = MB32(src_cur); MW32(cur2 - 42, v0); MW32(cur2 - 2, v0);
    v0 = MB32(scur - 18); MW32(cur2 - 34, v0); MW32(cur2 + 6, v0);
    v0 = MB32(scur - 14); MW32(cur2 - 26, v0); MW32(cur2 + 14, v0);
    v0 = MB32(scur - 10); src_cur += 24;
    MW32(cur2 - 18, v0); MW32(cur2 + 22, v0);
    v0 = (uint32_t)MBS16(scur - 8) << 3; v0 += ptab2;
    MW32(cur2 - 58, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 6) << 3; v0 += ptab;
    MW32(cur + 0, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 4) << 3; v0 += ptab;
    count -= 1;
    MW32(cur2 - 70, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 2) << 3; v0 += ptab;
    cur += 100;
    MW32(cur2 - 66, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 0) << 3; scur += 24; v0 += ptab;
    { uint32_t r = MB32(v0);
      { uint32_t c0 = MB16(cur2 - 40), c1 = MB16(cur2 - 32);
        c1 += xoff; MW16(cur2 - 32, c1);
        MW32(cur2 - 62, r);
        c0 += yoff; MW16(cur2 - 40, c0);
        c0 = MB16(cur2 + 8);  c1 = MB16(cur2 + 0);
        c0 += xoff; c1 += yoff;
        MW16(cur2 + 8, c0);   MW16(cur2 + 0, c1); } }
    cur2 += 100;
  } while (count != 0);
  return cur;
}

/* heap_serialize_tag_01000015 @0x8016b234 (100 insns, 7 args): dst
 * stride 0x88, src u32 stream stride 0x1c at src+0 plus three u32s
 * at src+4/+8/+12 (all dual-stored), u16 index octet at
 * src+12..+26 stride 0x1c, aux src+26.
 * Eight single-word lookups alternating arg4/arg3 tables
 * (-16..0 step 2). Tail adds param_6 to x u16s (t0-0x28/+0xc) and
 * param_7 to y u16s (t0-0x34/+0). Tag bytes 12/0x3c at
 * dst+0x1b/0x1f/0x57/0x5b (t0 = cur+0x62). Returns cur + count*0x88. */
uint32_t heap_serialize_tag_01000015(uint32_t cur,uint32_t scur,uint32_t ptab,
                     uint32_t ptab2,uint32_t count,uint32_t xoff,
                     uint32_t yoff)
{




  uint32_t src_cur = scur;



  uint32_t cur2 = cur + 98;
  uint32_t v0;
  if (count == 0) return cur;
  scur += 26;
  do {
    MW8(cur2 - 63, 12);  MW8(cur2 - 59, 60);
    MW8(cur2 - 11, 12);  MW8(cur2 - 7, 60);
    v0 = MB32(src_cur);      MW32(cur2 - 54, v0); MW32(cur2 - 2, v0);
    v0 = MB32(scur - 22); MW32(cur2 - 42, v0); MW32(cur2 + 10, v0);
    v0 = MB32(scur - 18); MW32(cur2 - 30, v0); MW32(cur2 + 22, v0);
    v0 = MB32(scur - 14); src_cur += 28;
    MW32(cur2 - 18, v0); MW32(cur2 + 34, v0);
    v0 = (uint32_t)MBS16(scur - 16) << 3; v0 += ptab2;
    MW32(cur2 - 82, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 12) << 3; v0 += ptab;
    MW32(cur + 0, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 10) << 3; v0 += ptab2;
    MW32(cur2 - 78, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 8) << 3;  v0 += ptab;
    MW32(cur2 - 94, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 6) << 3;  v0 += ptab2;
    MW32(cur2 - 74, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 4) << 3;  v0 += ptab;
    count -= 1;
    MW32(cur2 - 90, MB32(v0));
    v0 = (uint32_t)MBS16(scur - 2) << 3;  v0 += ptab2;
    cur += 136;
    MW32(cur2 - 70, MB32(v0));
    v0 = (uint32_t)MBS16(scur + 0) << 3;  scur += 28; v0 += ptab;
    { uint32_t r = MB32(v0);
      { uint32_t cy = MB16(cur2 - 52), cx = MB16(cur2 - 40);
        cy += yoff; MW16(cur2 - 52, cy);
        MW32(cur2 - 86, r);
        cx += xoff; MW16(cur2 - 40, cx);
        cy = MB16(cur2 + 0); cx = MB16(cur2 + 12);
        cx += xoff; cy += yoff;
        MW16(cur2 + 12, cx); MW16(cur2 + 0, cy); } }
    cur2 += 136;
  } while (count != 0);
  return cur;
}


/* heap_serialize_tag_01000000 — heap record serializer @0x8016a82c
 * (192 bytes, 7 blocks, leaf: no calls, no MMIO). Faithful clean port.
 *
 * Args (all guest addrs): a = dst cursor (16-byte record array),
 * b = record header {+4 count1, +8 base1, +0x10 count2, +0x14 base2},
 * c/d = pass-1 src 8-byte-record array / ptr-slot array,
 * e/f = pass-2 src / ptr-slot array (guest stack args 5/6).
 *
 * Each pass: off = MB32(hdr+base_off)*8; src += off; ptab += off;
 * then count times: copy 8 bytes src[i] -> dst record's low half,
 * store &dst[+8] into ptab[i]; dst += 16, src/ptab += 8.
 * Returns dst advanced by (count1+count2)*16. lwl/lwr pairs ==
 * MB32/MW32 word accesses (file convention). */
uint32_t heap_serialize_tag_01000000(uint32_t dst,uint32_t hdr,
                      uint32_t src1,uint32_t ptab1,
                      uint32_t src2,uint32_t ptab2)
{
  uint32_t n = MB32(hdr + 4u);
  uint32_t off;
  if (n != 0) {
    off = MB32(hdr + 8u) * 8u;
    src1 += off; ptab1 += off;
    do {
      uint32_t rec = dst;
      MW32(dst,     MB32(src1));
      MW32(dst + 4, MB32(src1 + 4));
      dst += 16;
      MW32(ptab1, rec + 8);
      ptab1 += 8;
      src1 += 8;
      n -= 1;
    } while (n != 0);
  }
  n = MB32(hdr + 16u);
  if (n != 0) {
    off = MB32(hdr + 20u) * 8u;
    src2 += off; ptab2 += off;
    do {
      uint32_t rec = dst;
      MW32(dst,     MB32(src2));
      MW32(dst + 4, MB32(src2 + 4));
      dst += 16;
      MW32(ptab2, rec + 8);
      ptab2 += 8;
      src2 += 8;
      n -= 1;
    } while (n != 0);
  }
  return dst;
}

#undef BR2_TAG5
#undef BR2_TAG7
