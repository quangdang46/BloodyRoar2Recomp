/* battle.c — battle_frame_tick battle-frame tick.
 * Faithful verbatim port: guest identifiers/labels preserved.
 * param_1 = battle object (guest addr as int), param_2 = phase mask.
 * Fighter ctrls are the absolute globals 0x1f8000e8 (P1) /
 * 0x1f8001f0 (P2); &DAT_x yields the guest base pointer. */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

typedef uint8_t  byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef uint8_t  undefined1;
typedef uint8_t  undefined;
typedef uint64_t ulonglong;
typedef int64_t  longlong;

extern void    *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint32_t br2_gaddr(const void *p);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t br2_call_clean(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t call_guest(uint32_t fn, uint32_t a, uint32_t b);
extern uint32_t call_guest4(uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
extern uint32_t br2_guest_frame(uint32_t);
extern void    br2_guest_frame_free(void);
extern void    br2_st32(uint32_t,uint32_t);
extern uint32_t guest_sp(void);
extern uint32_t guest_gp(void);
extern void    *fn_lookup(uint32_t addr);
#define CONCAT22(hi,lo) (((undefined4)(uint16_t)(hi) << 16) | (uint16_t)(lo))
#define CONCAT11(hi,lo) (((uint16_t)(uint8_t)(hi) << 8) | (uint8_t)(lo))

/* guest absolute-address globals (host binds storage) */
extern uint8_t  *g_DAT_1f8000e8, *g_DAT_1f8001f0;
extern uint8_t  *g_DAT_1f8000ea, *g_DAT_1f8001f2, *g_DAT_1f8000ee, *g_DAT_1f8001f6;
extern uint8_t  *g_DAT_801f4d72, *g_DAT_1f8000dc8;
extern uint8_t  *g_DAT_801f4d70, *g_DAT_801f4d74, *g_DAT_801f4d76, *g_DAT_801f4d78,
                *g_DAT_801f4d7a, *g_DAT_801f4d7c, *g_DAT_801e40b6;
extern uint32_t *g__DAT_1f800148, *g__DAT_1f800154, *g__DAT_1f800250, *g__DAT_1f80025c;
extern uint32_t *g__DAT_1f8000dc, *g__DAT_1f800100, *g__DAT_1f800104, *g__DAT_1f800110;
extern uint32_t *g__DAT_1f800208, *g__DAT_1f80020c, *g__DAT_1f800218;
extern int32_t  *g__DAT_801f4cb6, *g__DAT_801f4cb2;

#define DAT_1f8000e8    (*g_DAT_1f8000e8)
#define DAT_1f8001f0    (*g_DAT_1f8001f0)
#define DAT_1f8000ea    (*g_DAT_1f8000ea)
#define DAT_1f8001f2    (*g_DAT_1f8001f2)
#define DAT_1f8000ee    (*g_DAT_1f8000ee)
#define DAT_1f8001f6    (*g_DAT_1f8001f6)
#define DAT_801f4d72    (*g_DAT_801f4d72)
#define DAT_801f4d70    (*g_DAT_801f4d70)
#define DAT_801f4d74    (*g_DAT_801f4d74)
#define DAT_801f4d76    (*g_DAT_801f4d76)
#define DAT_801f4d78    (*g_DAT_801f4d78)
#define DAT_801f4d7a    (*g_DAT_801f4d7a)
#define DAT_801f4d7c    (*g_DAT_801f4d7c)
#define DAT_801e40b6    (*g_DAT_801e40b6)
#define _DAT_1f800148   (*g__DAT_1f800148)
#define _DAT_1f800154   (*g__DAT_1f800154)
#define _DAT_1f800250   (*g__DAT_1f800250)
#define _DAT_1f80025c   (*g__DAT_1f80025c)
#define _DAT_1f8000dc   (*g__DAT_1f8000dc)
#define _DAT_1f800100   (*g__DAT_1f800100)
#define _DAT_1f800104   (*g__DAT_1f800104)
#define _DAT_1f800110   (*g__DAT_1f800110)
#define _DAT_1f800208   (*g__DAT_1f800208)
#define _DAT_1f80020c   (*g__DAT_1f80020c)
#define _DAT_1f800218   (*g__DAT_1f800218)
#define _DAT_801f4cb6   (*g__DAT_801f4cb6)
#define _DAT_801f4cb2   (*g__DAT_801f4cb2)

typedef void code();

/* additional guest globals (camera/viewport state) */
extern uint16_t *g_DAT_1f80030c, *g_DAT_801f0234, *g_DAT_1f8002f8;
extern uint16_t *g_DAT_801f0250;
extern uint8_t  *g_DAT_801b144c, *g_DAT_801b140c, *g_DAT_1f800098, *g_DAT_801f0228;
extern uint32_t *g__DAT_801f022c;
#define DAT_1f80030c   g_DAT_1f80030c[0]
#define DAT_1f80030e   g_DAT_1f80030c[1]
#define DAT_1f800310   g_DAT_1f80030c[2]
#define DAT_1f800314   g_DAT_1f80030c[3]
#define DAT_1f800316   g_DAT_1f80030c[4]
#define DAT_1f800318   g_DAT_1f80030c[5]
#define DAT_1f80031c   g_DAT_1f80030c[6]
#define DAT_1f80031e   g_DAT_1f80030c[7]
#define DAT_1f800320   g_DAT_1f80030c[8]
#define _DAT_1f80030c   g_DAT_1f80030c[0]
#define _DAT_1f80030e   g_DAT_1f80030c[1]
#define _DAT_1f800310   g_DAT_1f80030c[2]
#define _DAT_1f800314   g_DAT_1f80030c[3]
#define _DAT_1f800316   g_DAT_1f80030c[4]
#define _DAT_1f800318   g_DAT_1f80030c[5]
#define _DAT_1f80031c   g_DAT_1f80030c[6]
#define _DAT_1f80031e   g_DAT_1f80030c[7]
#define _DAT_1f800320   g_DAT_1f80030c[8]
#define DAT_801f0234   g_DAT_801f0234[0]
#define DAT_801f0236   g_DAT_801f0234[1]
#define DAT_801f0238   g_DAT_801f0234[2]
#define _DAT_801f0234   g_DAT_801f0234[0]
#define _DAT_801f0236   g_DAT_801f0234[1]
#define _DAT_801f0238   g_DAT_801f0234[2]
#define DAT_1f8002f8   g_DAT_1f8002f8[0]
#define DAT_1f8002fa   g_DAT_1f8002f8[1]
#define DAT_1f8002fc   g_DAT_1f8002f8[2]
#define DAT_1f8002fe   g_DAT_1f8002f8[3]
#define DAT_1f800300   g_DAT_1f8002f8[4]
#define DAT_1f800302   g_DAT_1f8002f8[5]
#define _DAT_1f8002f8   g_DAT_1f8002f8[0]
#define _DAT_1f8002fa   g_DAT_1f8002f8[1]
#define _DAT_1f8002fc   g_DAT_1f8002f8[2]
#define _DAT_1f8002fe   g_DAT_1f8002f8[3]
#define _DAT_1f800300   g_DAT_1f8002f8[4]
#define _DAT_1f800302   g_DAT_1f8002f8[5]
#define DAT_801f0250   g_DAT_801f0250[0]
#define _DAT_801f0250  g_DAT_801f0250[0]
#define _DAT_801f0252  g_DAT_801f0250[1]
#define _DAT_801f0254  g_DAT_801f0250[2]
#define _DAT_801f0256  g_DAT_801f0250[3]
#define _DAT_801f0258  g_DAT_801f0250[4]
#define _DAT_801f025a  g_DAT_801f0250[5]
#define _DAT_801f025c  g_DAT_801f0250[6]
#define _DAT_801f025e  g_DAT_801f0250[7]
#define _DAT_801f0260  g_DAT_801f0250[8]
#define DAT_801b144c   (*g_DAT_801b144c)
#define DAT_801b140c   (*g_DAT_801b140c)
#define DAT_1f800098   (*g_DAT_1f800098)
#define DAT_801f0228   (*g_DAT_801f0228)
#define _DAT_801f022c  (*(int16_t *)g__DAT_801f022c) /* guest lh/sh @0x801f022c (0x8012ee48,0x8012ecbc) */

/* host hooks (guest-faithful names; runtime binds implementations) */
extern void fighter_frame(byte*,uint8_t*);
#define FUN_801787dc fighter_frame
extern void sprite_strip_draw_dual(int param_1,int param_2,int param_3,int param_4,ushort *param_5,int param_6, int param_7,int param_8);
extern int task_alloc_wrap_a(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4);
extern int task_alloc_wrap_b(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4, int param_5);
extern void hud_cell_frame_set(int param_1,int param_2);
extern void hud_digits_color(int param_1,int param_2);
extern bool fighter_height_check(int param_1);
extern undefined4 sprite_rect_set_a(int param_1,short param_2,int param_3,undefined4 param_4,int param_5);
extern undefined4 sprite_rect_set_b(int param_1,short param_2,int param_3,undefined4 param_4,int param_5);
extern undefined4 sprite_rect_set_c(int param_1,short param_2,int param_3,undefined4 param_4,int param_5);
extern int * rtps_point_xform(undefined4 *param_1,undefined4 *param_2,int *param_3,int param_4);
extern int * rtps_point_xform2(undefined4 *param_1,undefined4 *param_2,int *param_3);
extern uint * rotmat_rtps_build(uint param_1,uint *param_2);
extern uint * rotmat2_pair_build(uint param_1,uint param_2,uint *param_3);
extern int sprintf_core(char *param_1,char *param_2,...);
extern uint * rtps_basis_xform(undefined4 *param_1,undefined2 *param_2,uint *param_3);
extern undefined4 * rotmat2_build(uint param_1,undefined4 *param_2);
extern int * task_node_alloc(int param_1,undefined4 param_2,uint param_3,int param_4,int param_5,int param_6);
extern void task_node_free(int *param_1);
extern void task_pools_tick(int param_1);
extern void noop_a(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4);
extern int fx_group_alloc(undefined4 param_1,undefined4 param_2,int param_3,int param_4);
extern void noop_b(int param_1,undefined4 param_2,undefined4 param_3);
extern undefined4 noop_c(int param_1);
extern void node_tree_free(int param_1);
extern void prim_attr_word_set(int param_1,undefined2 param_2);
extern undefined4 * prim_group_build(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4);
extern undefined4 tween_chan_alloc(int param_1,int param_2);
extern void tween_dur_spread(int param_1,int param_2);
extern int str_len(char *param_1);
extern char * strchr_bounded(char *param_1,char param_2,int param_3);
extern undefined1 * mem_move(undefined1 *param_1,undefined1 *param_2,int param_3);
extern undefined1 FUN_801b0db8[];
extern uint32_t *g__DAT_801f0654, *g__DAT_801f0656;
extern uint8_t  *g__DAT_1f800082;
/* guest lh/lhu @0x1f800082 — a u16 sim/pause gate, NOT a u32. Reading
 * 4 bytes at the unaligned address picks up the low half of the pointer
 * stored at 0x1f800084 (0x801BEF90 -> 0xEF900000) and the gate reads
 * nonzero forever, freezing every channel-script task (LAB_80137acc et
 * al.) so the ROUND-FIGHT phase never advances past 0x300. menu.c:432
 * and save.c:236 already bind it as u16. */
#define _DAT_1f800082   (*(uint16_t *)g__DAT_1f800082)
extern void script_stream_tick(int *param_1);
extern undefined4 always_one(int);
extern void triple_track_spawn(int,undefined2,int);
extern void xform_accum_spawn(int,int);
extern void impact_fx_spawn(undefined4*,uint,int,int,uint);
extern int fx_record_alloc(undefined4,int,uint,undefined4,undefined4);
extern void emit_task_spawn(int*,int,undefined2,short,undefined2,undefined2);
extern void emit_task_spawn_b(undefined4*,undefined4,undefined4,undefined4);
extern void quad_fx_spawn(int,int,undefined4*,undefined2,ushort,int,int);
extern void group_fx_spawn(undefined4,undefined4,undefined4,undefined4,undefined4,undefined4,undefined4);
extern undefined4 *vec3_step_toward(undefined4*,short*,short*,short);
extern undefined4 *vec3_step_toward_spd(undefined4*,short*,short*,int);
extern undefined4 *vec3_step_toward_q(undefined4*,short*,short*,short);
extern undefined4 *vec3_step_toward_q_spd(undefined4*,short*,short*,int);
extern undefined1 table_byte_next_a(void);
extern undefined1 table_byte_next_b(void);
extern void pad_edge_scan(int);
extern void cmd_input_parse(int);
extern void action_from_input(int);
extern undefined4 seg_sphere_hit(int,int,int*,int*,int*,short*);
extern undefined4 sphere_pt_hit(int,int,int*,int*,short*);
extern void debug_strike_fx_a(short*,int,int,undefined4,int);
extern void debug_strike_fx_b(short*,undefined4,undefined4,ushort,int);
extern void noop_fn();
extern undefined2 *rtps_pt_xform(ushort*,undefined2*);
extern undefined4 FUN_80194660(undefined2*,undefined2*);
extern void node_xform_apply(int,undefined4,undefined4);
extern ushort attr_flag_mask(uint);
extern undefined2 pad_hist_record(int,int,undefined2);
extern void fighter_script_fsm(int,undefined4,int*,int);
extern bool pt_box_radius_test(int,int,int*,int*);
extern void fighter_state_tick(uint*,undefined4,int);
extern undefined4 fighter_atk_dir(int,int);
extern void hit_pose_swap(int,int);
extern undefined4 hit_elig_check(int,int);
extern undefined4 hitbox_pair_test(int,int,undefined2*);
extern uint fighter_range_code(int,int);
extern int hit_scale_calc(int,int);
extern undefined4 hit_react_apply(int,int,int,short*,int);
extern uint hitbox_wall_collide(int,undefined2*,int);
extern void camera_aim_offset(int,int,undefined4);
extern void node_translate_tree(int,int,int,int);
extern void fighter_attach_run(int);
extern void fighter_height_track(int,int);
extern void hitbox_corners_xform(int,int);
extern void fighter_contact_resolve(int,int,int);
extern void fighter_pair_update(int,int,int);
extern void battle_camera_step(uint,byte*,byte*,int);
extern void battle_hud_draw(int,int,int,int);
extern void fighter_flash_update(uint,uint,int,byte*);
extern void camera_state_cache(uint,int);
extern void player_light_apply(byte*,int);
extern int  atan2_lut(int,int);

extern void hud_text_blk_alt(undefined4 param_1,undefined2 param_2,const void *param_3,undefined2 param_4, undefined2 param_5,short param_6);
extern void announce_param_block_init_b(undefined4 param_1,undefined2 param_2,const void *param_3,undefined2 param_4, undefined2 param_5,short param_6);
extern void slot_triple_set(int param_1,int param_2,int param_3,int param_4);
extern void rec_flag1_set(undefined2 *param_1);
extern void target_angles_set(short *param_1,int param_2);
extern void target_depth_set(short *param_1,int param_2);
extern void pair_depth_set(short *param_1,short *param_2,int param_3);
extern void target_angles_depth_set(short *param_1,int param_2);
extern void midpoint_angles_set(int *param_1,int *param_2,int param_3);
extern void dual_target_track(int param_1,int param_2,int param_3);
extern void dual_target_track_b(int param_1,int param_2,int param_3);
extern void camera_track_shot(int param_1,int param_2,int param_3);
extern void camera_track_step(int param_1,int param_2,int param_3);
extern int u16_step_toward(ushort *param_1,ushort *param_2,short param_3);
extern int fixed_approach_s16(ushort *param_1,short *param_2,uint param_3);
extern void fighter_trail_emit(undefined4 *param_1,undefined4 param_2,int param_3,int param_4);
extern bool burst_slot_active(int param_1);
extern void prim_packet_init_tex(int param_1,int param_2,int param_3,uint param_4,byte *param_5);
extern undefined4 FUN_80194630(undefined4 *param_1,undefined4 *param_2);
extern void gte_xform_accum(short *param_1,uint *param_2,int param_3,undefined4 param_4);
extern uint * gte_rtps_xform3(undefined4 *param_1,ushort *param_2,uint *param_3);
extern uint * gte_xform3_nold(ushort *param_1,uint *param_2);
extern int * gte_mvmva_add(undefined4 *param_1,uint *param_2,int *param_3);
extern undefined2 * gte_rtps_xform1(undefined4 *param_1,undefined4 *param_2,undefined2 *param_3);
extern void gte_rot_load(undefined4 *param_1);
extern void gte_light_load(undefined4 *param_1);
extern void gte_tr_load(int param_1);
extern void gte_cw_d000_set(undefined4 param_1);
extern undefined4 * gte_op_a00428(undefined4 *param_1,undefined4 *param_2);
extern void orbit_fx_spawn(int param_1);
extern void task_heads_clear(int param_1);
extern void task_slot_step(void);
extern void task_slot_fns_run(int param_1);
extern void task_pool_init(int param_1,int param_2,int param_3,int param_4);
extern int task_pool_alloc(int param_1,int param_2,int param_3);
extern void task_pool_free(int param_1);
extern void gp_slot48_set(undefined4 param_1);
extern void gp_slot24_set(undefined4 param_1);
extern int prim_batch_emit(int param_1,int param_2,undefined4 param_3,undefined4 param_4,int *param_5, undefined4 param_6,undefined4 param_7,int param_8,int param_9,uint param_10, undefined4 param_11);
extern void obj_flag34_clear(int param_1);
extern int tween_seq_alloc(int param_1,undefined2 param_2,undefined4 param_3,undefined4 param_4,short param_5, undefined2 param_6,undefined4 param_7,undefined4 param_8);
extern void gte_matrix_base_set(undefined4 param_1,undefined4 param_2);
extern int lazy_node_wrap(undefined4 param_1,undefined4 param_2,undefined2 param_3);
extern int prim_group_alloc(undefined4 param_1,int param_2,int param_3,int param_4,int param_5,int param_6, undefined4 param_7);
extern void node_param_quad_set(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4, undefined4 param_5);
extern int * prim_list_gte_invoke(int *param_1,undefined4 param_2,undefined4 param_3,int param_4,undefined4 param_5 ,undefined4 param_6);
extern undefined4 text_render_full(uint *param_1,uint param_2,int param_3);
extern undefined4 text_render_simple(uint *param_1,uint param_2,int param_3);
extern void particle_burst_spawn(int param_1,int param_2);
extern void particle_burst_update(int param_1,undefined4 param_2,undefined4 param_3,int param_4);
extern void radial_glow_draw(int *param_1);
extern void iris_wipe_spawn(int param_1,int param_2);
extern undefined4 * gte_xform3_ip(undefined4 *param_1,undefined2 *param_2);
extern int deferred_cmd_post();
extern int range_circle_test();
extern void vec_emit_spawn(undefined4 *param_1);
extern void iris_wipe_update(int *param_1);
extern undefined4 script_op_dispatch(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4);
extern int trig_sin_lookup(uint param_1);
extern uint * angle_vec_build(uint param_1,uint *param_2);
extern uint rand_lcg(void);
extern int isqrt(int param_1);
extern uint * gte_rtps_xform3_t(undefined4 *param_1,ushort *param_2,uint *param_3);
extern undefined4 * gte_xform1_ir(undefined4 *param_1,undefined4 *param_2);
extern undefined4 * gte_rtps_xform1_raw(undefined4 *param_1,undefined4 *param_2,undefined4 *param_3);
extern int * node_lazy_attach(int param_1,int param_2,int param_3,int param_4);
extern undefined4 tween_ensure(int param_1);
extern void obj_flag34_set(int param_1);
extern void tween_seek(int param_1,int param_2);
extern void tween_seq_step(short *param_1);
extern void prim_group_gc(int param_1);
extern void rect_prim_spawn(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4, undefined1 param_5,undefined1 param_6,undefined2 param_7,undefined2 param_8, undefined4 param_9);
extern undefined4 isqrt_refine(int param_1);
extern undefined4 cop2_clz(undefined4 param_1);
extern void gte_points_xform(undefined4 *param_1,undefined4 *param_2,undefined2 *param_3,undefined2 *param_4, undefined2 *param_5,int param_6);
extern int tween_track_alloc(int param_1,undefined2 param_2,undefined4 param_3,undefined2 *param_4,short param_5 ,undefined2 param_6,undefined2 param_7,undefined4 param_8);
extern void tween_apply(short *param_1,int param_2,int param_3);
void keyframe_cursor_search(uint *param_1);
extern void rect_prim_tick(uint *param_1);
extern void prim_tag_set(int param_1);
extern void prim_packet_init(int param_1,int param_2,int param_3,uint param_4);
extern void verts_offset_apply(undefined4 *param_1,uint *param_2,int param_3,short *param_4,int param_5, undefined4 param_6);
extern void verts_rgb_apply(uint *param_1,uint *param_2,int param_3,short *param_4,int param_5, undefined4 param_6);
extern void verts_add_apply(int param_1,undefined4 *param_2,ushort *param_3,int param_4);
extern void verts_sub_apply(int param_1,undefined4 *param_2,ushort *param_3,int param_4);
extern int getCopControlWord();
extern int fixed_point_divide(int param_1,int param_2);
extern undefined4 * gte_xform3_t_ip(undefined4 *param_1,undefined2 *param_2);
extern int prim_task_run(int param_1,undefined4 param_2,undefined4 param_3,int param_4);
extern undefined2 * vec2_rot(undefined4 param_1,undefined4 param_2,undefined2 *param_3);
extern uint * angle_gte_vec(undefined4 param_1,undefined4 param_2,uint *param_3);
extern int * prim_list_persp_step(int *param_1,undefined4 param_2,undefined4 param_3,int param_4,undefined4 param_5 ,int param_6,undefined4 param_7,undefined4 param_8);
extern void gte_light_set(undefined4 param_1,undefined4 param_2,undefined4 param_3);
extern int * gte_xform_add_off(int param_1,undefined4 *param_2,int *param_3);
extern uint * angle_pair_rotmat(uint param_1,uint param_2,uint *param_3);
extern void slot_height_proj(undefined4 param_1,int param_2,int param_3,int param_4);
extern void battle_intro_fsm(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4);
extern int task_alloc_wrap_c(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4);
extern int task_alloc_wrap_d(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,int param_5);
extern int task_alloc_wrap_e(int param_1,int param_2,int param_3);
extern int task_alloc_wrap_f(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4);
extern void slot_triple_tick(int *param_1);
extern void xform_accum_tick(undefined4 *param_1);
extern void impact_fx_update(int param_1);
extern void impact_fx_emit_tick(int param_1);
extern void fx_record_init(undefined4 *param_1);
extern void quad_fx_update(int param_1);
extern int angle_from_xy(int param_1,int param_2);
extern void table_1f0ae0_clear(int param_1);
extern void sprite_place_centered(undefined4 param_1,undefined4 param_2,short param_3,short param_4,int param_5, int param_6);
extern void fighter_tint_fade(int param_1,int param_2,int param_3,int param_4);

void battle_frame_tick(int battle,uint phase)

{
  char light;
  int hud_ot;
  undefined4 prim_list;
  undefined4 aux;
  int ctx;
  
  ctx = battle + 0x28;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x82) ))== 0) {
    fighter_contact_resolve((int)guest_of(&DAT_1f8000e8),(int)guest_of(&DAT_1f8001f0),battle);
    FUN_801787dc(&DAT_1f8000e8,(uint8_t *)guest_at((uint32_t)battle));
    FUN_801787dc(&DAT_1f8001f0,(uint8_t *)guest_at((uint32_t)battle));
    fighter_pair_update((int)guest_of(&DAT_1f8000e8),(int)guest_of(&DAT_1f8001f0),battle);
  }
  if (((int)phase >> 2 & 1U) != 0) {
    if (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9d) ))== '\0') {
      hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x2038;
    }
    else {
      hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x10;
    }
    battle_hud_draw(hud_ot,battle,(int)guest_of(&DAT_1f8000e8),(int)guest_of(&DAT_1f8001f0));
  }
  battle_camera_step(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),&DAT_1f8000e8,&DAT_1f8001f0,battle);
  camera_state_cache(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),battle);
  if (((phase & 1) != 0) && ((_DAT_1f800148 & 2) == 0)) {
    fighter_flash_update(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle,&DAT_1f8000e8);
    if (((_DAT_1f800154 & 0x400) == 0) ||
       (*(short *)(&DAT_801f4d72 + (uint)DAT_1f8000ea * 0xbc) != 0)) {
      if (((_DAT_1f800154 & 0x8000000) == 0) || ((_DAT_1f8000dc & 1) == 0)) {
        player_light_apply(&DAT_1f8000e8,1);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = _DAT_1f800100;
        light = DAT_1f8000ee;
      }
      else {
        player_light_apply(&DAT_1f8000e8,1);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800110 + 0x34)));
        light = *(char *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800110 + 0x24)));
      }
      prim_list_gte_invoke((int *)guest_at((uint32_t)prim_list),hud_ot + 0x40,aux,(int)light,ctx,0x1f80033c);
    }
    if ((((int)phase >> 6 & 1U) != 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))== '\0')) {
      prim_list_persp_step((int *)guest_at((uint32_t)_DAT_1f800104),*(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x2040,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),0xf,
                   ctx,0x1f80035c,*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 8 + -0x7fe4e94c,
                   *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x206c);
    }
  }
  if ((((int)phase >> 1 & 1U) != 0) && ((_DAT_1f800250 & 2) == 0)) {
    fighter_flash_update(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle,&DAT_1f8001f0);
    if (((_DAT_1f80025c & 0x400) == 0) ||
       (*(short *)(&DAT_801f4d72 + (uint)DAT_1f8001f2 * 0xbc) != 0)) {
      if (((_DAT_1f80025c & 0x8000000) == 0) || ((_DAT_1f8000dc & 1) == 0)) {
        player_light_apply(&DAT_1f8001f0,2);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = _DAT_1f800208;
        light = DAT_1f8001f6;
      }
      else {
        player_light_apply(&DAT_1f8001f0,1);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800218 + 0x34)));
        light = *(char *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800218 + 0x24)));
      }
      prim_list_gte_invoke((int *)guest_at((uint32_t)prim_list),hud_ot + 0x40,aux,(int)light,ctx,0x1f80033c);
    }
    if ((((int)phase >> 6 & 1U) != 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))== '\0')) {
      prim_list_persp_step((int *)guest_at((uint32_t)_DAT_1f80020c),*(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x2040,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),0xf,
                   ctx,0x1f80035c,*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 8 + -0x7fe4e94c,
                   *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x206c);
    }
  }
  if (((int)phase >> 4 & 1U) == 0) {
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((battle + 0xb1) ))= 1;
  }
  battle_intro_fsm(ctx,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle);
  if (((int)phase >> 8 & 1U) != 0) {
    if (((_DAT_1f800154 & 0x8000) == 0) && ((_DAT_1f80025c & 0x8000) == 0)) {
      _DAT_801f4cb6 = 0xfff8;
    }
    else {
      _DAT_801f4cb6 = 0x10;
    }
    _DAT_801f4cb2 = 0;
  }
  if (((int)phase >> 7 & 1U) != 0) {
    fighter_tint_fade(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle,
                 (int)*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98))));
  }
  if (((int)phase >> 3 & 1U) != 0) {
    fighter_trail_emit((undefined4 *)guest_at((uint32_t)ctx),0x1f80033c,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),battle);
  }
  if (((*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))== '\0') && (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9e) ))== '\0')) &&
     (((int)phase >> 5 & 1U) != 0)) {
    slot_height_proj(*(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x207c,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),
                 *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),(int)(uintptr_t)battle);
  }
  return;

}
void fighter_contact_resolve(int f0,int f1,int battle)
{
  int dir0;
  int dir1;
  undefined4 cam;
  int work1;
  int work0;
  
  cam = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc)));
  work0 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38)));
  work1 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
  fighter_state_tick((uint *)guest_at((uint32_t)(uintptr_t)(f0)),battle,cam);
  fighter_state_tick((uint *)guest_at((uint32_t)(uintptr_t)(f1)),battle,cam);
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x10000) != 0) {
    return;
  }
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x40100000) != 0) ||
     ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x40100000) != 0)) {
    if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x40000000) != 0) &&
        ((((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x100000) != 0 && (0 < *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0xd4))))) &&
         ((*(ushort *)guest_at((uint32_t)(uintptr_t)((f1 + 0x80) ))& 0xc0) == 0xc0)))) &&
       ((*(ushort *)guest_at((uint32_t)(uintptr_t)((f1 + 0x7a) ))& 0xc0) != 0)) {
      *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x4000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x4000000;
      return;
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x40000000) == 0) {
      return;
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x100000) == 0) {
      return;
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0xd4) ))< 1) {
      return;
    }
    if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((f0 + 0x80) ))& 0xc0) != 0xc0) {
      return;
    }
    if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((f0 + 0x7a) ))& 0xc0) == 0) {
      return;
    }
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x4000000;
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x4000000;
    return;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))& 0x8000) == 0) {
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))& 0x8000) == 0) {
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x6000000) != 0) {
        return;
      }
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x6000000) != 0) {
        return;
      }
      dir0 = fighter_atk_dir(f0,f1);
      dir1 = fighter_atk_dir(f1,f0);
      if (dir0 == 0 && dir1 == 0) {
        return;
      }
      if (dir0 == dir1) {
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))& 0x4000) != 0) {
          *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x600000;
        }
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))& 0x4000) != 0) {
          *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x600000;
        }
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
      }
      else {
        if (dir0 == -1) {
          if ((*(uint *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))& 0x4000) != 0) {
            *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x600000;
          }
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
        }
        else if (dir0 == 1) {
          *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x400000;
          *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x800000;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work1 + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x24)));
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
          hit_pose_swap(f0,work0);
          hit_pose_swap(f1,work1);
        }
        if (dir1 != -1) {
          if (dir1 != 1) {
            return;
          }
          *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x400000;
          *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x800000;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work1 + 0x24)));
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))= 0;
          goto LAB_8017f680;
        }
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))& 0x4000) != 0) {
          *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x600000;
        }
      }
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))= 0;
      return;
    }
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))= 0;
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x400000;
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x800000;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work1 + 0x24)));
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))| 0x10000000;
  }
  else {
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x400000;
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x800000;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work1 + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((work0 + 0x24)));
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))| 0x10000000;
  }
LAB_8017f680:
  hit_pose_swap(f0,work0);
  hit_pose_swap(f1,work1);
  return;

}

void camera_state_cache(uint unused,int battle)
{
  _DAT_801f0250 = _DAT_1f80030c;
  _DAT_801f0256 = _DAT_1f80030e;
  _DAT_801f025c = _DAT_1f800310;
  _DAT_801f0252 = _DAT_1f800314;
  _DAT_801f0258 = _DAT_1f800316;
  _DAT_801f025e = _DAT_1f800318;
  if (_DAT_801f022c == 0) {
    _DAT_801f0254 = _DAT_1f80031c;
    _DAT_801f025a = _DAT_1f80031e;
    _DAT_801f0260 = _DAT_1f800320;
  }
  else {
    _DAT_801f0254 = _DAT_801f0234;
    _DAT_801f025a = _DAT_801f0236;
    _DAT_801f0260 = _DAT_801f0238;
  }
  if (*(uint32_t *)(&DAT_801b144c + *(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 4) == 0) {
    angle_gte_vec((int)_DAT_1f8002fa,(int)_DAT_1f8002f8,(uint *)guest_at((uint32_t)0x1f80035c));
    vec2_rot((int)_DAT_1f8002fe,(int)_DAT_1f8002fc,(undefined2*)guest_at((uint32_t)0x1f80033c));
    vec2_rot((int)_DAT_1f800302,(int)_DAT_1f800300,(undefined2*)guest_at((uint32_t)0x1f800342));
  }
  else {
    uint32_t fn_g = *(uint32_t *)(&DAT_801b144c + *(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 4);
    void *fn = fn_lookup(fn_g);
    if (fn != (void *)0x0) {
      /* Guest jalr @0x8012EF04 passes unused=0x1F8002F8, battle=0x801F0228,
       * a2=battle — guest addresses. Clean callees in this table
       * (dual_axis_coord_xform / proj_coord_gte_emit) take guest-u32
       * params, so pass guest addresses, not &DAT_ host pointers. */
      ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t))fn)
          (0x1f8002f8u,0x801f0228u,(uint32_t)battle,0);
    }
    else {
      /* Unported jalr target: run the generated body with the same guest
       * args — unused=0x1F8002F8, battle=0x801F0228 (guest addrs, not host ptrs),
       * a2=battle. */
      br2_call_clean(fn_g,0x1f8002f8u,0x801f0228u,(uint32_t)battle,0);
    }
  }
  return;

}

void player_light_apply(byte *ctrl,int side)
{
  short ang;
  int tmp;
  
  tmp = side * 8;
  gte_light_set((int)*(short *)guest_at((uint32_t)(uintptr_t)((tmp + 0x1f800324))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((tmp + 0x1f800326))),
               (int)*(short *)guest_at((uint32_t)(uintptr_t)((tmp + 0x1f800328))));
  if (_DAT_801f022c != 0) {
    ang = angle_from_xy((int)*(short *)(ctrl + 0x76),(int)*(short *)(ctrl + 0x78));
    tmp = (int)*(short *)(ctrl + 0x4e) + (int)*(short *)(ctrl + 0x74);
    if (tmp < 0) {
      if (tmp < 0) {
        tmp = -tmp;
      }
      tmp = 0x1000 - tmp;
    }
    vec2_rot(0x800U - (int)ang & 0xfff,0x1000 - tmp,(undefined2*)guest_at((uint32_t)0x1f800348));
    if (side == 2) {
      _DAT_801f022c = 0;
    }
  }
  if (*(uint32_t *)(&DAT_801b140c + ((int)((uint)DAT_1f800098 << 0x18) >> 0x16)) != 0) {
    uint32_t fn_g = *(uint32_t *)(&DAT_801b140c + ((int)((uint)DAT_1f800098 << 0x18) >> 0x16));
    void *fn = fn_lookup(fn_g);
    if (fn != (void *)0x0) {
      /* Guest jalr @0x8012EDC4 passes ctrl=0x1F8002F8, side=0x801F0228,
       * a2=0x1F80033C, a3=ctrl's guest address — all guest addrs.
       * Clean callees (proj_coord_gte_emit / dual_axis_coord_xform)
       * take guest-u32 params; ctrl is a host ptr here, so resolve
       * it with guest_of like the generated fallback below. */
      ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t))fn)
          (0x1f8002f8u,0x801f0228u,0x1f80033cu,guest_of(ctrl));
    }
    else {
      /* Unported jalr target: guest args ctrl=0x1F8002F8, side=0x801F0228,
       * a2=0x1F80033C, a3=ctrl's guest address. */
      br2_call_clean(fn_g,0x1f8002f8u,0x801f0228u,0x1f80033cu,guest_of(ctrl));
    }
  }
  gte_light_load(&DAT_801f0250);
  return;

}
void fighter_pair_update(int f0,int f1,int battle)
{
  short head0;
  short head1;
  undefined2 qres;
  int iVar4;
  int iVar5;
  undefined4 zz0;
  int iVar7;
  int iVar8;
  int iVar9;
  int scale0;
  undefined4 zz1;
  int scale1;
  /* Guest stack layout (br2eu_func_801809C8): the two hitbox-result buffers
   * are 8-byte slots at sp+0x18 and sp+0x20. hitbox_pair_test /
   * seg_sphere_hit / sphere_pt_hit write result shorts at indices 0..2
   * (6 bytes); the decompiler split each 8-byte slot into a 2-short array
   * plus a trailing scalar, so index-2 writes ran off the end of the array
   * and smashed fighter_pair_update's saved x28 on the host stack
   * (observed: x28 low16 overwritten -> fighter_flash_update a3
   * garbage -> SIGSEGV). Model each slot as a 4-short array; local_34 is
   * hitres0[2], local_2c is hitres1[2]. */
  short hitres0 [4];
  short hitres1 [4];
  
  iVar9 = 0;
  iVar8 = 0;
  hitbox_corners_xform(f0,f1);
  fighter_height_track(f0,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38))));
  fighter_height_track(f1,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38))));
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x10000000) != 0) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))| 0x40000000;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x10000000) != 0) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))| 0x40000000;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x800000) != 0) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))| 0x2000;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x800000) != 0) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))| 0x2000;
  }
  iVar4 = hit_elig_check(f0,f1);
  iVar5 = hit_elig_check(f1,f0);
  iVar7 = 0;
  if (iVar4 != 0) {
    zz0 = fighter_range_code(f0,f1);
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))= zz0;
    scale0 = hit_scale_calc(f0,f1);
    iVar9 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((f0 + 0xda) ))* scale0;
    if (iVar9 < 0) {
      iVar9 = iVar9 + 0x7f;
    }
    iVar9 = iVar9 >> 7;
    iVar7 = hitbox_pair_test(f0,f1,hitres0);
  }
  iVar4 = 0;
  if (iVar5 != 0) {
    zz0 = fighter_range_code(f1,f0);
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((f0 + 0x68) ))= zz0;
    scale1 = hit_scale_calc(f1,f0);
    iVar8 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((f1 + 0xda) ))* scale1;
    if (iVar8 < 0) {
      iVar8 = iVar8 + 0x7f;
    }
    iVar8 = iVar8 >> 7;
    iVar4 = hitbox_pair_test(f1,f0,hitres1);
  }
  if (iVar7 != 0) {
    if (iVar4 != 0) {
      if (iVar9 < iVar8) {
        iVar7 = 0;
      }
      else if (iVar8 < iVar9) {
        iVar4 = 0;
      }
    }
    if (iVar7 != 0) {
      hit_react_apply(f0,f1,scale0,hitres0,iVar7);
    }
  }
  if (iVar4 != 0) {
    hit_react_apply(f1,f0,scale1,hitres1,iVar4);
  }
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x48000) == 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x4000000) == 0)
     ) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xdfffffff;
  }
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x10000000) == 0) &&
     ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x2000000) == 0)) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xbfffdfff;
  }
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x48000) == 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x4000000) == 0)
     ) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0xdfffffff;
  }
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x10000000) == 0) &&
     ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x2000000) == 0)) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0xbfffdfff;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x60000000) == 0) {
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((f1 + 0xdd) ))= 0;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x60000000) == 0) {
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((f0 + 0xdd) ))= 0;
  }
  iVar9 = atan2_lut(*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))-
                       *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c))),
                       *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34) ))-
                       *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34))));
  camera_aim_offset(f0,f1,iVar9);
  iVar8 = hitbox_wall_collide(f0,hitres0,battle);
  iVar7 = hitbox_wall_collide(f1,hitres1,battle);
  if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x40000000) == 0) ||
      ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x100000) == 0)) &&
     (((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x40000000) == 0 ||
      ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x100000) == 0)))) {
    if (iVar8 != 0) {
      node_translate_tree(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))),(int)hitres0[0],0,(int)hitres0[2]);
      *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))+ hitres0[0];
      *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))+ hitres0[2];
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18) ))=
           *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a) ))=
           *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c) ))=
           *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
    }
    if (iVar7 == 0) goto LAB_80180f88;
    node_translate_tree(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))),(int)hitres1[0],0,(int)hitres1[2]);
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))+ hitres1[0];
    iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
  }
  else {
    if (iVar8 == 0 && iVar7 == 0) goto LAB_80180f88;
    hitres0[0] = hitres0[0] + hitres1[0];
    hitres0[2] = hitres0[2] + hitres1[2];
    node_translate_tree(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))),(int)hitres0[0],0,(int)hitres0[2]);
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))+ hitres0[0];
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))+ hitres0[2];
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
    node_translate_tree(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))),(int)hitres0[0],0,(int)hitres0[2]);
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))+ hitres0[0];
    iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
    hitres1[2] = hitres0[2];
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8) ))+ hitres1[2];
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18) ))=
       *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a) ))=
       *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c) ))=
       *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c)));
LAB_80180f88:
  head0 = *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x4e)));
  head1 = *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x4e)));
  iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
  zz0 = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
  iVar7 = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34)));
  zz1 = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c)));
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x74) ))= (short)((iVar9 - head0) * 0x100000 >> 0x14);
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x74) ))=
       (short)((int)(((iVar9 + 0x800U & 0xfff) - (int)head1) * 0x100000) >> 0x14);
  iVar9 = atan2_lut(zz0,-iVar8);
  qres = fixed_point_divide(zz0,iVar8);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((f0 + 0x72) ))= qres;
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x70) ))= (short)((iVar9 - head0) * 0x100000 >> 0x14);
  iVar9 = atan2_lut(zz1,-iVar7);
  qres = fixed_point_divide(zz1,iVar7);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((f1 + 0x72) ))= qres;
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x70) ))= (short)((iVar9 - head1) * 0x100000 >> 0x14);
  fighter_attach_run(f0);
  fighter_attach_run(f1);
  return;

}

void fighter_flash_update(uint battle,uint slot,int ctx2,byte *rec)
{
  int roff;
  uint flags;
  short *prec;
  undefined1 *prim;
  uint fidx;
  undefined1 val;
  int bright;
  undefined4 pflags;
  short *recp;
  fidx = (uint)*(byte *)(rec + 2);
  roff = fidx * 0xbc;
  recp = (short *)(&DAT_801f4d70 + roff);
  { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    decomp_trace_record(80,(uint32_t)battle,fidx,
        (uint8_t)((void*)rec==(void*)g_DAT_1f8001f0?2:1),(uint32_t)ctx2); }
  if (*(short *)guest_at((uint32_t)(uintptr_t)((ctx2 + 0x82) ))== 0) {
    if (0 < *recp) {
      *recp = *recp + -1;
      return;
    }
    bright = (int)*(short *)(&DAT_801f4d74 + roff) + (int)*(short *)(&DAT_801f4d76 + roff);
    if (bright < 0x100) {
      if (bright < 0) {
        *(undefined2 *)(&DAT_801f4d76 + roff) = 0;
        bright = 0;
      }
    }
    else {
      *(undefined2 *)(&DAT_801f4d76 + roff) = 0;
      bright = 0xff;
    }
    *(short *)(&DAT_801f4d74 + roff) = (short)bright;
  }
  else {
    bright = (int)*(short *)(&DAT_801f4d74 + roff);
  }
  if (bright == 0xff) {
    flags = *(uint *)(rec + 0x6c) | 0x400;
  }
  else {
    flags = *(uint *)(rec + 0x6c) & 0xfffffbff;
  }
  *(uint *)(rec + 0x6c) = flags;
  if ((bright == *(short *)(&DAT_801f4d78 + roff)) && (bright == *(short *)(&DAT_801f4d7a + roff)))
  {
    return;
  }
  *(undefined2 *)(&DAT_801f4d7a + roff) = *(undefined2 *)(&DAT_801f4d78 + roff);
  *(short *)(&DAT_801f4d78 + roff) = (short)bright;
  val = (undefined1)bright;
  switch(*(undefined2 *)(&DAT_801f4d72 + roff)) {
  case 0:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = val;
    *(undefined1 *)((intptr_t)recp + slot * 0x58 + 0x41) = val;
    *(undefined1 *)(recp + slot * 0x2c + 0x21) = val;
    val = (&DAT_801e40b6)[slot * 0x2150];
    pflags = 0x40;
    goto LAB_8016d1cc;
  case 1:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = val;
    *(undefined1 *)((intptr_t)recp + slot * 0x58 + 0x41) = val;
    *(undefined1 *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 2:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = val;
    *(undefined1 *)((intptr_t)recp + slot * 0x58 + 0x41) = 0;
    goto LAB_8016d1c0;
  case 3:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = 0;
    goto LAB_8016d1bc;
  case 4:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = 0;
    *(undefined1 *)((intptr_t)recp + slot * 0x58 + 0x41) = 0;
    *(undefined1 *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 5:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = val;
    goto LAB_8016d1bc;
  case 6:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = val;
    *(undefined1 *)((intptr_t)recp + slot * 0x58 + 0x41) = 0;
    *(undefined1 *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 7:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(undefined1 *)(recp + slot * 0x2c + 0x20) = 0;
    *(undefined1 *)((intptr_t)recp + slot * 0x58 + 0x41) = val;
    *(undefined1 *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 8:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(char *)(recp + slot * 0x2c + 0x20) = (char)(bright / 2);
LAB_8016d1bc:
    *(undefined1 *)((intptr_t)prec + 5) = val;
LAB_8016d1c0:
    *(undefined1 *)(prec + 3) = 0;
    break;
  default:
    goto switchD_8016cecc_default;
  }
  val = (&DAT_801e40b6)[slot * 0x2150];
  pflags = 0x20;
LAB_8016d1cc:
  prim_packet_init_tex((int)guest_of(prim),1,val,pflags,0);
switchD_8016cecc_default:
  *(uint *)(recp + slot * 0x2c + 0x2c) =
       *(uint *)(recp + slot * 0x2c + 0x2c) & 0xff000000 |
       *(uint *)guest_at((uint32_t)(uintptr_t)((battle + 0x207c) ))& 0xffffff;
  *(uint *)guest_at((uint32_t)(uintptr_t)((battle + 0x207c) ))=
       *(uint *)guest_at((uint32_t)(uintptr_t)((battle + 0x207c) ))& 0xff000000 | guest_of(recp + slot * 0x2c + 6) & 0xffffff;
  return;

}
#define PTR_FUN_801b55c0  (*g_PTR_FUN_801b55c0)
#define PTR_DAT_801b47ec   (*g_PTR_DAT_801b47ec)
#define PTR_DAT_801b4e10    (*g_PTR_DAT_801b4e10)
#define PTR_DAT_801b4e14    (*g_PTR_DAT_801b4e14)

/* guest globals for battle_camera_step / battle_hud_draw (host binds storage) */
extern uint8_t *g_DAT_8011f944, *g_DAT_8011f946, *g_DAT_8011f948, *g_DAT_8011f9c4, *g_DAT_8011f9c6, *g_DAT_8011f9c8, *g_DAT_8011fa04, *g_DAT_8011fa0c, *g_DAT_801b4a30, *g_DAT_801b4a48, *g_DAT_801b4ac8, *g_DAT_801b4b48, *g_DAT_801b4b5c, *g_DAT_801b4be4, *g_DAT_801b4c24, *g_DAT_801b4c25, *g_DAT_801b4c26, *g_DAT_801b4c27, *g_DAT_801b4c28, *g_DAT_801b4c29, *g_DAT_801b4c2a, *g_DAT_801b4c2b, *g_DAT_801b4c30, *g_DAT_801b4c32, *g_DAT_801b4c34, *g_DAT_801b4c36, *g_DAT_801b4c40, *g_DAT_801b4c42, *g_DAT_801b4c44, *g_DAT_801b4c46, *g_DAT_801b4c50, *g_DAT_801b4c52, *g_DAT_801b4c54, *g_DAT_801b4c56, *g_DAT_801b4c88, *g_DAT_801b4c8a, *g_DAT_801b4c8c, *g_DAT_801b4c8e, *g_DAT_801b4c9c, *g_DAT_801b4c9e, *g_DAT_801b4cbc, *g_DAT_801b4ccc, *g_DAT_801b4cd2, *g_DAT_801b4cdc, *g_DAT_801b4cec, *g_DAT_801b4cf2, *g_DAT_801b4d28, *g_DAT_801b4d34, *g_DAT_801b4d3c, *g_DAT_801b4d50, *g_DAT_801b4d58, *g_DAT_801b4d6c, *g_DAT_801b4d7c, *g_DAT_801b4d90, *g_DAT_801b4d98, *g_DAT_801b4dac, *g_DAT_801b4dbc, *g_DAT_801b4dd4, *g_DAT_801b4de8, *g_DAT_801b4e08, *g_DAT_801b4e20, *g_DAT_801f0760, *g_DAT_801f0940, *g_DAT_801f4f28, *g_DAT_801f51d0, *g_DAT_801f51d8, *g_DAT_801f5210, *g_DAT_801f5218;
#define DAT_8011f944   (*g_DAT_8011f944)
#define DAT_8011f946   (*g_DAT_8011f946)
#define DAT_8011f948   (*g_DAT_8011f948)
#define DAT_8011f9c4   (*g_DAT_8011f9c4)
#define DAT_8011f9c6   (*g_DAT_8011f9c6)
#define DAT_8011f9c8   (*g_DAT_8011f9c8)
#define DAT_8011fa04   (*g_DAT_8011fa04)
#define DAT_8011fa0c   (*g_DAT_8011fa0c)
#define DAT_801b4a30   (*g_DAT_801b4a30)
#define DAT_801b4a48   (*g_DAT_801b4a48)
#define DAT_801b4ac8   (*g_DAT_801b4ac8)
#define DAT_801b4b48   (*g_DAT_801b4b48)
#define DAT_801b4b5c   (*g_DAT_801b4b5c)
#define DAT_801b4be4   (*g_DAT_801b4be4)
#define DAT_801b4c24   (*g_DAT_801b4c24)
#define DAT_801b4c25   (*g_DAT_801b4c25)
#define DAT_801b4c26   (*g_DAT_801b4c26)
#define DAT_801b4c27   (*g_DAT_801b4c27)
#define DAT_801b4c28   (*g_DAT_801b4c28)
#define DAT_801b4c29   (*g_DAT_801b4c29)
#define DAT_801b4c2a   (*g_DAT_801b4c2a)
#define DAT_801b4c2b   (*g_DAT_801b4c2b)
#define DAT_801b4c30   (*g_DAT_801b4c30)
#define DAT_801b4c32   (*g_DAT_801b4c32)
#define DAT_801b4c34   (*g_DAT_801b4c34)
#define DAT_801b4c36   (*g_DAT_801b4c36)
#define DAT_801b4c40   (*g_DAT_801b4c40)
#define DAT_801b4c42   (*g_DAT_801b4c42)
#define DAT_801b4c44   (*g_DAT_801b4c44)
#define DAT_801b4c46   (*g_DAT_801b4c46)
#define DAT_801b4c50   (*g_DAT_801b4c50)
#define DAT_801b4c52   (*g_DAT_801b4c52)
#define DAT_801b4c54   (*g_DAT_801b4c54)
#define DAT_801b4c56   (*g_DAT_801b4c56)
#define DAT_801b4c88   (*g_DAT_801b4c88)
#define DAT_801b4c8a   (*g_DAT_801b4c8a)
#define DAT_801b4c8c   (*g_DAT_801b4c8c)
#define DAT_801b4c8e   (*g_DAT_801b4c8e)
#define DAT_801b4c9c   (*g_DAT_801b4c9c)
#define DAT_801b4c9e   (*g_DAT_801b4c9e)
#define DAT_801b4cbc   (*g_DAT_801b4cbc)
#define DAT_801b4ccc   (*g_DAT_801b4ccc)
#define DAT_801b4cd2   (*g_DAT_801b4cd2)
#define DAT_801b4cdc   (*g_DAT_801b4cdc)
#define DAT_801b4cec   (*g_DAT_801b4cec)
#define DAT_801b4cf2   (*g_DAT_801b4cf2)
#define DAT_801b4d28   (*g_DAT_801b4d28)
#define DAT_801b4d34   (*g_DAT_801b4d34)
#define DAT_801b4d3c   (*g_DAT_801b4d3c)
#define DAT_801b4d50   (*g_DAT_801b4d50)
#define DAT_801b4d58   (*g_DAT_801b4d58)
#define DAT_801b4d6c   (*g_DAT_801b4d6c)
#define DAT_801b4d7c   (*g_DAT_801b4d7c)
#define DAT_801b4d90   (*g_DAT_801b4d90)
#define DAT_801b4d98   (*g_DAT_801b4d98)
#define DAT_801b4dac   (*g_DAT_801b4dac)
#define DAT_801b4dbc   (*g_DAT_801b4dbc)
#define DAT_801b4dd4   (*g_DAT_801b4dd4)
#define DAT_801b4de8   (*g_DAT_801b4de8)
#define DAT_801b4e08   (*g_DAT_801b4e08)
#define DAT_801b4e20   (*g_DAT_801b4e20)
#define DAT_801b4b54   (*g_DAT_801b4b54)
#define DAT_801b4b56   (*g_DAT_801b4b56)
#define DAT_801b4b94   (*g_DAT_801b4b94)
#define DAT_801b4b95   (*g_DAT_801b4b95)
#define DAT_801b4b96   (*g_DAT_801b4b96)
#define DAT_801b4b97   (*g_DAT_801b4b97)
#define DAT_801b4e28   (*g_DAT_801b4e28)
#define DAT_801b1840   (*g_DAT_801b1840)
#define DAT_801b1844   (*g_DAT_801b1844)
#define DAT_801b1848   (*g_DAT_801b1848)
#define DAT_801b184c   (*g_DAT_801b184c)
#define DAT_801b1850   (*g_DAT_801b1850)
#define DAT_801b1854   (*g_DAT_801b1854)
#define DAT_801b1858   (*g_DAT_801b1858)
#define DAT_801b185c   (*g_DAT_801b185c)
#define DAT_801b1d78   (*g_DAT_801b1d78)
#define DAT_801b1d70   (*g_DAT_801b1d70)
#define DAT_8011fd7a   (*g_DAT_8011fd7a)
#define DAT_1f800394   (*g_DAT_1f800394)
#define DAT_1f80039c   (*g_DAT_1f80039c)
#define DAT_8011fd78   (*g_DAT_8011fd78)
#define DAT_8011fd98   (*g_DAT_8011fd98)
#define DAT_801f04a8   (*g_DAT_801f04a8)
#define DAT_801f04c8   (*g_DAT_801f04c8)
#define DAT_801f04c9   (*g_DAT_801f04c9)
#define DAT_801f04e4   (*g_DAT_801f04e4)
#define DAT_1f8003a8   (*g_DAT_1f8003a8)
#define DAT_1f8003a0   (*g_DAT_1f8003a0)
#define DAT_1f8003b4   (*g_DAT_1f8003b4)
#define DAT_801b1ea4   (*g_DAT_801b1ea4)
#define DAT_1f800108   (*g_DAT_1f800108)
#define DAT_1f8003f8   (*g_DAT_1f8003f8)
#define DAT_801be838   (*g_DAT_801be838)
#define DAT_801b17f4   (*g_DAT_801b17f4)
#define DAT_801b1528   (*g_DAT_801b1528)
#define DAT_801b152a   (*g_DAT_801b152a)
#define DAT_1f8000eb   (*g_DAT_1f8000eb)
#define DAT_801e40b9   (*g_DAT_801e40b9)
#define DAT_801f066e   (*g_DAT_801f066e)
#define DAT_1f8000ec   (*g_DAT_1f8000ec)
#define DAT_1f800148   (*g_DAT_1f800148)
#define DAT_1f8003e0   (*g_DAT_1f8003e0)
#define DAT_8011fea0   (*g_DAT_8011fea0)
#define DAT_8011fea1   (*g_DAT_8011fea1)
#define DAT_8011fea2   (*g_DAT_8011fea2)
#define DAT_8011feac   (*g_DAT_8011feac)
#define DAT_8011fead   (*g_DAT_8011fead)
#define DAT_8011feae   (*g_DAT_8011feae)
#define DAT_801bef20   (*g_DAT_801bef20)
#define DAT_801e40ba   (*g_DAT_801e40ba)
#define DAT_801e40bb   (*g_DAT_801e40bb)
#define DAT_801f0664   (*g_DAT_801f0664)
#define DAT_801f0750   (*g_DAT_801f0750)
#define DAT_801f0780   (*g_DAT_801f0780)
#define DAT_801f0790   (*g_DAT_801f0790)
#define DAT_801f0ad8   (*g_DAT_801f0ad8)
#define DAT_801f0ad9   (*g_DAT_801f0ad9)
#define DAT_801f0ae4   (*g_DAT_801f0ae4)
#define DAT_801f4cbc   (*g_DAT_801f4cbc)
#define DAT_8011ed80   (*g_DAT_8011ed80)
#define DAT_8011ede0   (*g_DAT_8011ede0)
#define DAT_8011ee40   (*g_DAT_8011ee40)
#define DAT_8011ee5b   (*g_DAT_8011ee5b)
#define DAT_8011ee74   (*g_DAT_8011ee74)
#define DAT_8011ee8f   (*g_DAT_8011ee8f)
#define DAT_8011eea4   (*g_DAT_8011eea4)
#define DAT_8011eed4   (*g_DAT_8011eed4)
#define DAT_8011ef04   (*g_DAT_8011ef04)
#define DAT_8011ef64   (*g_DAT_8011ef64)
#define DAT_8011ef75   (*g_DAT_8011ef75)
#define DAT_801f0300   (*g_DAT_801f0300)
#define DAT_801f0304   (*g_DAT_801f0304)
#define DAT_801f0328   (*g_DAT_801f0328)
#define DAT_801237e0   (*g_DAT_801237e0)
#define DAT_80124048   (*g_DAT_80124048)
#define _DAT_801f0654   (*g__DAT_801f0654)
#define _DAT_801f0656   (*g__DAT_801f0656)
#define _DAT_1f80000c   (*g__DAT_1f80000c)
#define _DAT_1f8003e0   (*g__DAT_1f8003e0)
#define _DAT_1f8003e4   (*g__DAT_1f8003e4)
#define _DAT_1f8003e8   (*g__DAT_1f8003e8)
#define _DAT_1f8003ec   (*g__DAT_1f8003ec)
#define _DAT_1f8003f0   (*g__DAT_1f8003f0)
#define _DAT_1f8003f4   (*g__DAT_1f8003f4)
#define _DAT_1f8003f8   (*g__DAT_1f8003f8)
#define _DAT_1f8003fc   (*g__DAT_1f8003fc)
#define _DAT_801c4508   (*g__DAT_801c4508)
#define _DAT_801f0298   (*(int16_t *)g__DAT_801f0298) /* guest lh @0x801f0298 (0x80137098) */
#define _DAT_801f029a   (*(int16_t *)g__DAT_801f029a) /* guest lh @0x801f029a */
#define _DAT_1f80038e   (*(uint16_t *)g__DAT_1f80038e) /* guest sh @0x1f80038e */
#define _DAT_1f800392   (*(uint16_t *)g__DAT_1f800392) /* guest sh @0x1f800392 */
#define _DAT_1f800396   (*(uint16_t *)g__DAT_1f800396) /* guest sh @0x1f800396 */
#define _DAT_1f80039a   (*(uint16_t *)g__DAT_1f80039a) /* guest sh @0x1f80039a */
#define _DAT_1f80039e   (*(uint16_t *)g__DAT_1f80039e) /* guest sh @0x1f80039e */
#define _DAT_1f8003a2   (*(uint16_t *)g__DAT_1f8003a2) /* guest sh @0x1f8003a2 */
#define _DAT_801f0228   (*g__DAT_801f0228)
#define _DAT_801f04b0   (*g__DAT_801f04b0)
#define _DAT_801f04c0   (*g__DAT_801f04c0)
#define _DAT_801f04c6   (*g__DAT_801f04c6)
#define _DAT_801f0730   (*g__DAT_801f0730)
#define _DAT_801f0734   (*g__DAT_801f0734)
#define _DAT_801f0738   (*g__DAT_801f0738)
#define _DAT_801f0740   (*g__DAT_801f0740)
#define _DAT_801f0744   (*g__DAT_801f0744)
#define _DAT_801f0758   (*g__DAT_801f0758)
#define _DAT_801f075c   (*g__DAT_801f075c)
#define _DAT_801f0930   (*g__DAT_801f0930)
#define _DAT_801f0938   (*(int16_t *)g__DAT_801f0938) /* guest lh/sh @0x801f0938 */
#define _DAT_801f093a   (*g__DAT_801f093a)
#define _DAT_801f093c   (*(int16_t *)g__DAT_801f093c) /* guest lh/sh @0x801f093c */
#define _DAT_801f093e   (*g__DAT_801f093e)
#define _DAT_801f0940   (*g__DAT_801f0940)
#define _DAT_801f0944   (*g__DAT_801f0944)
#define _DAT_801f4cb4   (*(int16_t *)g__DAT_801f4cb4) /* guest lh/sh @0x801f4cb4 (0x8016d244) */
#define _DAT_801f4cb8   (*(int16_t *)g__DAT_801f4cb8) /* guest lh/sh @0x801f4cb8 */
#define _DAT_801f4cba   (*(int16_t *)g__DAT_801f4cba) /* guest lh/sh @0x801f4cba */
#define _DAT_1f80037c   (*(int16_t *)g__DAT_1f80037c) /* guest lh/sh @0x1f80037c (0x8012d154,0x8012dce4) */
#define _DAT_1f80037e   (*(int16_t *)g__DAT_1f80037e) /* guest lh/sh @0x1f80037e */
#define _DAT_1f800380   (*(int16_t *)g__DAT_1f800380) /* guest lh/sh @0x1f800380 */
#define _DAT_1f800388   (*g__DAT_1f800388)
#define _DAT_1f8003cc   (*g__DAT_1f8003cc)
#define _DAT_1f8003d0   (*g__DAT_1f8003d0)
#define _DAT_1f8003d4   (*g__DAT_1f8003d4)
#define _DAT_1f8003d8   (*g__DAT_1f8003d8)
#define _DAT_1f8003dc   (*g__DAT_1f8003dc)
#define _DAT_1f800008   (*g__DAT_1f800008)
#define _DAT_1f800024   (*g__DAT_1f800024)
#define _DAT_801f68e0   (*g__DAT_801f68e0)
#define _DAT_801f04a0   (*g__DAT_801f04a0)
#define _DAT_801f04a4   ((int *)guest_at(*(uint32_t *)g__DAT_801f04a4))
#define _DAT_801f04b8   (*g__DAT_801f04b8)
#define _DAT_801f04bc   (*g__DAT_801f04bc)
#define _DAT_801f0760   (*g__DAT_801f0760)
#define _DAT_801f0764   (*g__DAT_801f0764)
#define _DAT_801f0768   (*g__DAT_801f0768)
#define _DAT_801f076c   (*g__DAT_801f076c)
#define _DAT_801f0770   (*g__DAT_801f0770)
#define DAT_1f800015   (*g_DAT_1f800015)
#define DAT_801247a8   (*g_DAT_801247a8)
#define DAT_801b4b6c   (*g_DAT_801b4b6c)
#define DAT_801b4b6d   (*g_DAT_801b4b6d)
#define DAT_801b4b6e   (*g_DAT_801b4b6e)
#define DAT_801b4b6f   (*g_DAT_801b4b6f)
#define DAT_801b67a0   (*g_DAT_801b67a0)
#define DAT_801b67a4   (*g_DAT_801b67a4)
#define DAT_801b67a8   (*g_DAT_801b67a8)
#define DAT_801f0760   (*g_DAT_801f0760)
#define DAT_801f0940   (*g_DAT_801f0940)
#define DAT_801f4f28   (*g_DAT_801f4f28)
#define DAT_801f51d0   (*g_DAT_801f51d0)
#define DAT_801f51d8   (*g_DAT_801f51d8)
#define DAT_801f5210   (*g_DAT_801f5210)
#define DAT_801f5218   (*g_DAT_801f5218)
extern uint32_t *g__DAT_801f4f20, *g__DAT_801f50c0, *g__DAT_801f50d0, *g__DAT_801f50e4, *g__DAT_801f50f4, *g__DAT_801f5150, *g__DAT_801f5160, *g__DAT_801f5174, *g__DAT_801f5184, *g__DAT_801f5210, *g__DAT_801f5214;
#define _DAT_801f4f20   (*g__DAT_801f4f20)
#define _DAT_801f50c0   (*(uint16_t *)g__DAT_801f50c0) /* guest sh @0x801f50c0 (0x801737ec) */
#define _DAT_801f50d0   (*(uint16_t *)g__DAT_801f50d0) /* guest sh @0x801f50d0 */
#define _DAT_801f50e4   (*(uint16_t *)g__DAT_801f50e4) /* guest sh @0x801f50e4 */
#define _DAT_801f50f4   (*(uint16_t *)g__DAT_801f50f4) /* guest sh @0x801f50f4 */
#define _DAT_801f5150   (*(uint16_t *)g__DAT_801f5150) /* guest sh @0x801f5150 */
#define _DAT_801f5160   (*(uint16_t *)g__DAT_801f5160) /* guest sh @0x801f5160 */
#define _DAT_801f5174   (*(uint16_t *)g__DAT_801f5174) /* guest sh @0x801f5174 */
#define _DAT_801f5184   (*(uint16_t *)g__DAT_801f5184) /* guest sh @0x801f5184 */
#define _DAT_801f5210   (*g__DAT_801f5210)
#define _DAT_801f5214   (*g__DAT_801f5214)
extern code    **g_PTR_FUN_801b55c0;
extern uint8_t  **g_PTR_DAT_801b47ec, **g_PTR_DAT_801b4e10, **g_PTR_DAT_801b4e14;
extern uint8_t  *_g_pad_buffer;
extern uint8_t  *g_DAT_801b4b94, *g_DAT_801b4b95, *g_DAT_801b4b96, *g_DAT_801b4b97, *g_DAT_801b4e28;
extern uint16_t *g_DAT_801b4b54, *g_DAT_801b4b56;
extern uint8_t  *g_DAT_8011fd7a2, *g_DAT_801b1840, *g_DAT_801b1844, *g_DAT_801b1848, *g_DAT_801b184c, *g_DAT_801b1850, *g_DAT_801b1854, *g_DAT_801b1858, *g_DAT_801b185c, *g_DAT_801b1d78, *g_DAT_801b1d70, *g_DAT_8011fd7a, *g_DAT_1f800394, *g_DAT_1f80039c, *g_DAT_8011fd78, *g_DAT_8011fd98, *g_DAT_801f04a8, *g_DAT_801f04c8, *g_DAT_801f04c9, *g_DAT_801f04e4, *g_DAT_1f8003a8, *g_DAT_1f8003a0, *g_DAT_1f8003b4, *g_DAT_801b1ea4, *g_DAT_1f800108, *g_DAT_1f8003f8, *g_DAT_801be838, *g_DAT_801b17f4, *g_DAT_801b1528, *g_DAT_801b152a, *g_DAT_1f8000eb, *g_DAT_1f8000ec, *g_DAT_1f800148, *g_DAT_1f8003e0, *g_DAT_8011fea0, *g_DAT_8011fea1, *g_DAT_8011fea2, *g_DAT_8011feac, *g_DAT_8011fead, *g_DAT_8011feae, *g_DAT_801bef20, *g_DAT_801e40ba, *g_DAT_801e40bb, *g_DAT_801f0664, *g_DAT_801f0750, *g_DAT_801f0780, *g_DAT_801f0790, *g_DAT_801f0ad8, *g_DAT_801f0ad9, *g_DAT_801f0ae4, *g_DAT_801f4cbc, *g_DAT_8011ed80, *g_DAT_8011ede0, *g_DAT_8011ee40, *g_DAT_8011ee5b, *g_DAT_8011ee74, *g_DAT_8011ee8f, *g_DAT_8011eea4, *g_DAT_8011eed4, *g_DAT_8011ef04, *g_DAT_8011ef64, *g_DAT_8011ef75, *g_DAT_801f0300, *g_DAT_801f0304, *g_DAT_801f0328, *g_DAT_801237e0, *g_DAT_80124048;
extern int32_t *g__DAT_801f04a4;
extern uint32_t *g__DAT_1f80000c, *g__DAT_1f8003e0, *g__DAT_1f8003e4, *g__DAT_1f8003e8, *g__DAT_1f8003ec, *g__DAT_1f8003f0, *g__DAT_1f8003f4, *g__DAT_1f8003f8, *g__DAT_1f8003fc, *g__DAT_801c4508, *g__DAT_801f0298, *g__DAT_801f029a, *g__DAT_1f80038e, *g__DAT_1f800392, *g__DAT_1f800396, *g__DAT_1f80039a, *g__DAT_1f80039e, *g__DAT_1f8003a2, *g__DAT_801f0228, *g__DAT_801f04b0, *g__DAT_801f04c0, *g__DAT_801f04c6, *g__DAT_801f0730, *g__DAT_801f0734, *g__DAT_801f0738, *g__DAT_801f0740, *g__DAT_801f0744, *g__DAT_801f0758, *g__DAT_801f075c, *g__DAT_801f0930, *g__DAT_801f0938, *g__DAT_801f093a, *g__DAT_801f093c, *g__DAT_801f093e, *g__DAT_801f0940, *g__DAT_801f0944, *g__DAT_801f4cb4, *g__DAT_801f4cb8, *g__DAT_801f4cba, *g__DAT_1f80037c, *g__DAT_1f80037e, *g__DAT_1f800380, *g__DAT_1f800388, *g__DAT_1f8003cc, *g__DAT_1f8003d0, *g__DAT_1f8003d4, *g__DAT_1f8003d8, *g__DAT_1f8003dc, *g__DAT_1f800008, *g__DAT_1f800024, *g__DAT_801f68e0, *g__DAT_801f04a0, *g__DAT_801f04b8, *g__DAT_801f04bc, *g__DAT_801f0760, *g__DAT_801f0764, *g__DAT_801f0768, *g__DAT_801f076c, *g__DAT_801f0770;
extern uint32_t PTR_DAT_801b5574[];
extern uint32_t PTR_entry_0x80182f3c_801b5528[];
extern undefined1 LAB_801aae08[];
extern code    **PTR_LAB_801200b8;
extern uint8_t **g_PTR_DAT_801be84c;
extern undefined4 **g_PTR_PTR_801be894;
#define PTR_DAT_801be84c   (*g_PTR_DAT_801be84c)
#define PTR_PTR_801be894   (*g_PTR_PTR_801be894)
extern code    **PTR_FUN_801b1734;
extern void     trap(int);
extern uint8_t  *g_DAT_801e40b9, *g_DAT_801f066e, *g_DAT_1f800015, *g_DAT_801247a8, *g_DAT_801b4b6c, *g_DAT_801b4b6d, *g_DAT_801b4b6e, *g_DAT_801b4b6f, *g_DAT_801b67a0, *g_DAT_801b67a4, *g_DAT_801b67a8;
extern uint32_t *g__DAT_1f8000c4;
#define _DAT_1f8000c4   (*g__DAT_1f8000c4)
extern char     s__1___02d__1x_0_80123264[];
extern char     s_0123456789ABCDEF_80125208[], s_0123456789abcdef_8012521c[];
extern undefined1 LAB_8013ce68[];
extern void     setCopReg(int,int,int);
extern void     copFunction(int,int);
extern int      getCopReg(int,int);
extern void     setCopControlWord(int,int,int);
extern int lut_cos_q12();
extern int lut_sin_q12();
extern int FUN_80194500(uint32_t a0, ...);

/* Faithful signed 32/32 `div` LO result (same idiom as resolve.c).
 * Guest div has NO trap: div-by-zero yields LO = (dividend>=0) ? -1 : 1,
 * INT_MIN/-1 yields INT_MIN. C / would SIGFPE — use wherever the guest
 * emits `div`/`break`-free signed divide. */
static int32_t mips_div_lo(int32_t num, int32_t den)
{
    if (den == 0) return num >= 0 ? -1 : 1;
    if (num == (int32_t)0x80000000 && den == -1) return (int32_t)0x80000000;
    return num / den;
}

void battle_camera_step(uint cam,byte *f0,byte *f1,int battle)
{
  char cVar1;
  short sVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  ushort uVar5;
  short sVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  undefined1 *puVar10;
  int iVar11;
  undefined4 uVar12;
  short unaff_s2;
  uint unaff_s5;
  /* Guest vec3 slots: target_angles_depth_set / target_depth_set read
   * 3 contiguous s16 through ang_prev; FUN_80194630, pair_depth_set and
   * midpoint_angles_set read/write 3 contiguous u32 through local_90,
   * local_80 and local_70. The decompiler split each vec3 into scalars.
   * ang_prev[0..2] = old ang_prev/local_96/local_94;
   * local_90[0..2] = old local_90/local_8c/local_88;
   * local_80[0..2] = old local_80/local_7c/local_78;
   * local_70[0..2] = old local_70/local_6c/local_68. */
  short ang_prev [3];
  int local_90 [3];
  int local_80 [3];
  int local_70 [3];
  undefined1 auStack_60 [8];
  undefined1 auStack_58 [8];
  undefined1 auStack_50 [8];
  undefined1 auStack_48 [8];
  undefined1 auStack_40 [8];
  undefined1 auStack_38 [8];
  undefined1 auStack_30 [8];
  undefined2 local_28 [4];
  
  switch(*(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x7f) {
  case 0:
    if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x80) == 0) {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a) ))< 1) {
        vec3_step_toward_spd(auStack_58,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),4);
      }
      else {
        vec3_step_toward(auStack_60,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a))));
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38) ))< 1) {
        vec3_step_toward_q_spd(auStack_48,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      }
      else {
        vec3_step_toward_q(auStack_50,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38))));
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))< 1) {
        vec3_step_toward_spd(auStack_38,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),4);
      }
      else {
        vec3_step_toward(auStack_40,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c))));
      }
    }
    else {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))= *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x7f;
    }
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
    goto LAB_80131f4c;
  case 1:
    iVar9 = (int)*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80)));
    if (iVar9 < 0) {
      if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81)))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
        cVar1 = *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
        iVar9 = *(int *)(f1 + 0x20);
        goto LAB_8012fec0;
      }
    }
    else if (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0') {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
           *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
      cVar1 = *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
           *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
      iVar9 = *(int *)(f0 + 0x20);
LAB_8012fec0:
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cVar1 * 0x50 + iVar9 + 0x3c)));
    }
    else {
      local_80[0] = *(int *)guest_at((uint32_t)(uintptr_t)((iVar9 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
      local_70[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= (short)((local_80[0] + local_70[0]) / 2);
      local_80[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
      local_70[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= (short)((local_80[1] + local_70[1]) / 2);
      local_80[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
      local_70[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= (short)((local_80[2] + local_70[2]) / 2);
    }
    if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x80) == 0) {
      vec3_step_toward_spd(auStack_60,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42))),4);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a) ))< 1) {
        vec3_step_toward_spd(auStack_50,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),4);
      }
      else {
        vec3_step_toward(auStack_58,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a))));
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38) ))< 1) {
        vec3_step_toward_q_spd(auStack_40,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      }
      else {
        vec3_step_toward_q(auStack_48,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38))));
      }
      puVar10 = auStack_38;
      if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c)))) goto LAB_80130048;
      puVar10 = auStack_30;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
        if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
          puVar10 = auStack_30;
        }
        else {
          pair_depth_set((short *)local_80,(short *)local_70,cam);
          puVar10 = auStack_30;
        }
      }
LAB_8013049c:
      vec3_step_toward_spd(puVar10,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),4);
    }
    else {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
        if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
        }
        else {
          pair_depth_set((short *)local_80,(short *)local_70,cam);
        }
      }
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))= *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x7f;
    }
    break;
  case 2:
    iVar9 = (int)*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80)));
    if (iVar9 < 0) {
      if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81)))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
        sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
        sVar2 = *(short *)(f1 + 0x4e);
        goto LAB_801302d0;
      }
    }
    else {
      if (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0') {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
        sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
        sVar2 = *(short *)(f0 + 0x4e);
LAB_801302d0:
        uVar5 = (sVar6 + 0x1000) - sVar2;
      }
      else {
        local_80[0] = *(int *)guest_at((uint32_t)(uintptr_t)((iVar9 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
        local_70[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= (short)((local_80[0] + local_70[0]) / 2);
        local_80[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
        local_70[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= (short)((local_80[1] + local_70[1]) / 2);
        local_80[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
        local_70[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
        uVar5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= (short)((local_80[2] + local_70[2]) / 2);
      }
      *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= uVar5 & 0xfff;
    }
    if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x80) == 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a) ))< 1) {
        vec3_step_toward_spd(auStack_58,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),4);
      }
      else {
        vec3_step_toward(auStack_60,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a))));
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38) ))< 1) {
        vec3_step_toward_q_spd(auStack_48,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      }
      else {
        vec3_step_toward_q(auStack_50,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38))));
      }
      puVar10 = auStack_40;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))< 1) {
        puVar10 = auStack_38;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
          if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
            target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
            puVar10 = auStack_38;
          }
          else {
            pair_depth_set((short *)local_80,(short *)local_70,cam);
            puVar10 = auStack_38;
          }
        }
        goto LAB_8013049c;
      }
LAB_80130048:
      vec3_step_toward(puVar10,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c))));
    }
    else {
      uVar3 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      uVar4 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= uVar4;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= uVar4;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= uVar3;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= uVar3;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= uVar3;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= uVar4;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
        if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
        }
        else {
          pair_depth_set((short *)local_80,(short *)local_70,cam);
        }
      }
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
    }
    break;
  case 3:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x82) ))!= 0) goto LAB_80131f4c;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
    }
    else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))!= 1) goto LAB_80131f4c;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x82) ))< 0) {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))!= 0x168) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
        gte_cw_d000_set(0x168);
      }
      dual_target_track_b((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    }
    else {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))!= 0x168) {
        local_28[0] = 0x168;
        u16_step_toward((ushort *)guest_at((uint32_t)(battle + 0x24)),(ushort *)(uintptr_t)local_28,10);
        gte_cw_d000_set((int)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      }
      dual_target_track((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    }
    break;
  case 4:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set((undefined4)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      unaff_s5 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x38) + 0x100)));
      unaff_s2 = 0x1000 - (*(short *)(f0 + 0x4e) + *(short *)(f0 + 0xc6));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))= 0;
      uVar5 = table_byte_next_a();
      *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= (uVar5 & 3) + 1;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x128)));
    }
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x124)));
    iVar9 = *(int *)(f0 + 0x20);
    goto code_r0x8013181c;
  case 5:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set((undefined4)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      unaff_s5 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x38) + 0x100)));
      unaff_s2 = 0x1000 - (*(short *)(f1 + 0x4e) + *(short *)(f1 + 0xc6));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))= 0;
      uVar5 = table_byte_next_a();
      *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= (uVar5 & 3) + 1;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x128)));
    }
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x124)));
    iVar9 = *(int *)(f1 + 0x20);
code_r0x8013181c:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 300)));
    sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78)));
    if (sVar6 == 2) {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x380;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x280;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x180;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30) ))= 0xff80;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= unaff_s2;
      }
      sVar6 = fixed_approach_s16((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      iVar9 = cam + 0x60;
      if (sVar6 == 0) {
        iVar11 = cam + 0x68;
        uVar12 = 2;
        goto LAB_80131a34;
      }
    }
    else if (sVar6 < 3) {
      if (sVar6 != 1) goto LAB_80131f4c;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x380;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x280;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x100;
        if (unaff_s5 == 0) {
          sVar6 = -0x600;
        }
        else {
          sVar6 = 0x600;
        }
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= unaff_s2 + sVar6;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= unaff_s2;
      }
      sVar6 = fixed_approach_s16((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32))),10);
      iVar9 = cam + 0x60;
      if (sVar6 == 0) {
        iVar11 = cam + 0x68;
        uVar12 = 2;
        goto LAB_80131a34;
      }
    }
    else if (sVar6 == 3) {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x300;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
        if (unaff_s5 == 0) {
          sVar6 = -0xc00;
        }
        else {
          sVar6 = 0xc00;
        }
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= unaff_s2 + sVar6;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= unaff_s2;
      }
      sVar6 = fixed_approach_s16((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32))),10);
      iVar9 = cam + 0x60;
      if (sVar6 == 0) {
        iVar11 = cam + 0x68;
        uVar12 = 2;
LAB_80131a34:
        fixed_approach_s16((ushort *)guest_at((uint32_t)iVar9),(short *)guest_at((uint32_t)iVar11),uVar12);
      }
    }
    else {
      if (sVar6 != 4) goto LAB_80131f4c;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x380;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x80;
        if (unaff_s5 == 0) {
          sVar6 = -0xe00;
        }
        else {
          sVar6 = 0xe00;
        }
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= unaff_s2 + sVar6;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= unaff_s2;
      }
      sVar6 = fixed_approach_s16((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68))),2);
      iVar9 = cam + 0x2a;
      if (sVar6 == 0) {
        iVar11 = cam + 0x32;
        uVar12 = 5;
        goto LAB_80131a34;
      }
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))+ 1;
    goto LAB_80131f4c;
  case 6:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= *(undefined2 *)(f0 + 0x4e);
    goto LAB_80131ae0;
  case 7:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= *(undefined2 *)(f1 + 0x4e);
LAB_80131ae0:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x400;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0xa00;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set((undefined4)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
LAB_80131b40:
      if (0x300 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60)))) {
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))+ -2;
      }
    }
    else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 1) goto LAB_80131b40;
    uVar5 = 0x1000 - *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
    goto LAB_80131b64;
  case 8:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((undefined4)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    camera_track_shot((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    break;
  case 9:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((undefined4)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    camera_track_shot((int)(uintptr_t)cam,(int)guest_of(f1),(int)guest_of(f0));
    break;
  case 10:
  case 0xb:
    switch(*(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x78)))) {
    case 0:
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set(0x168);
      uVar7 = table_byte_next_a();
      uVar7 = uVar7 & 3;
      if (uVar7 == 1) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
          iVar9 = *(int *)(f0 + 0x20);
        }
        else {
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
          iVar9 = *(int *)(f1 + 0x20);
        }
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0x17c)));
        uVar7 = table_byte_next_b();
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= *(undefined2 *)(&DAT_8011fa04 + (uVar7 & 3) * 2);
        uVar7 = table_byte_next_b();
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x72) ))= *(undefined2 *)(&DAT_8011fa0c + (uVar7 & 1) * 2);
        uVar5 = table_byte_next_b();
        *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x70) ))= uVar5 & 1;
        sVar6 = table_byte_next_b();
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= sVar6 << 4;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 2;
        goto switchD_801306ac_caseD_2;
      }
      if (uVar7 < 2) {
        if (uVar7 == 0) {
          uVar7 = table_byte_next_a();
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)(&DAT_8011f9c4 + (uVar7 & 7) * 8);
          uVar7 = table_byte_next_a();
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)(&DAT_8011f9c6 + (uVar7 & 7) * 8);
          uVar7 = table_byte_next_a();
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)(&DAT_8011f9c8 + (uVar7 & 7) * 8);
          local_80[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
          local_80[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
          local_80[2] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
          if (-0x100 < local_80[1]) {
            local_80[1] = -0x100;
          }
          local_70[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
          local_70[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
          local_70[2] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
          if (-0x100 < local_70[1]) {
            local_70[1] = -0x100;
          }
          midpoint_angles_set(local_80,local_70,cam);
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 1;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
        }
      }
      else {
        if (uVar7 == 2) {
          if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
            sVar6 = *(short *)(f0 + 0x4e);
          }
          else {
            sVar6 = *(short *)(f1 + 0x4e);
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0x800 - sVar6;
          uVar7 = table_byte_next_b();
          if ((uVar7 & 1) == 0) {
            sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ -0x180;
          }
          else {
            sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ 0x180;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= sVar6;
          if (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))== '\x06') {
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
          }
          else {
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0xff00;
          }
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 3;
          goto switchD_801306ac_caseD_3;
        }
        if (uVar7 == 3) {
          if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
            iVar9 = (int)*(short *)(f1 + 0x4e) + (int)*(short *)(f1 + 0x74);
          }
          else {
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
            iVar9 = (int)*(short *)(f0 + 0x4e) + (int)*(short *)(f0 + 0x74);
          }
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= 0xff00;
          uVar7 = table_byte_next_a();
          if ((uVar7 & 1) == 0) {
            iVar9 = iVar9 + 0x1000;
          }
          else {
            iVar9 = iVar9 + 0x800;
          }
          iVar11 = lut_cos_q12(iVar9);
          iVar11 = iVar11 * 0x300;
          if (iVar11 < 0) {
            iVar11 = iVar11 + 0xfff;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))+ (short)(iVar11 >> 0xc);
          iVar9 = lut_sin_q12(iVar9);
          iVar9 = iVar9 * 0x300;
          if (iVar9 < 0) {
            iVar9 = iVar9 + 0xfff;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))+ (short)(iVar9 >> 0xc);
          target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 4;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
        }
      }
    case 1:
switchD_801306ac_caseD_1:
      local_80[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
      local_80[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
      local_80[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
      if (((*(uint *)(f0 + 0x5c) & 0x80000) == 0) && (-0x100 < local_80[1])) {
        local_80[1] = -0x100;
      }
      local_70[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
      local_70[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
      local_70[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
      if (((*(uint *)(f1 + 0x5c) & 0x80000) == 0) && (-0x100 < local_70[1])) {
        local_70[1] = -0x100;
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= (short)((local_80[0] + local_70[0]) / 2);
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= (short)((local_80[1] + local_70[1]) / 2);
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= (short)((local_80[2] + local_70[2]) / 2);
      local_90[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a)));
      local_90[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c)));
      local_90[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e)));
      iVar9 = FUN_80194630((undefined4 *)local_90,(undefined4 *)local_90);
      if (iVar9 != 0) {
        iVar9 = FUN_80194500(iVar9);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))+ (short)((uint)(local_90[0] * iVar9) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))+ (short)((uint)(local_90[1] * iVar9) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))+ (short)((uint)(local_90[2] * iVar9) >> 0x10);
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      vec3_step_toward_q(auStack_60,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),0x40);
      pair_depth_set((short *)local_80,(short *)local_70,cam);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) goto LAB_80131e94;
      iVar11 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60)));
      iVar8 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      iVar9 = iVar11 - iVar8;
      if (iVar8 <= iVar11) goto LAB_80131278;
      if (iVar9 < 0) {
        iVar9 = -iVar9;
      }
      if (iVar9 < 0) {
        iVar9 = iVar9 + 3;
      }
      iVar9 = iVar9 << 0xe;
      goto LAB_8013129c;
    case 2:
switchD_801306ac_caseD_2:
      sVar6 = -8;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x70) ))== 0) {
        sVar6 = 8;
      }
      sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))+ sVar6;
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= sVar6;
      iVar9 = lut_cos_q12((int)sVar6);
      iVar9 = iVar9 * *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x72)));
      if (iVar9 < 0) {
        iVar9 = iVar9 + 0xfff;
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))+ (short)(iVar9 >> 0xc);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
      iVar9 = lut_sin_q12((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a))));
      iVar9 = iVar9 * *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x72)));
      if (iVar9 < 0) {
        iVar9 = iVar9 + 0xfff;
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))+ (short)(iVar9 >> 0xc);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
        uVar7 = *(uint *)(f0 + 0x5c);
      }
      else {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
        uVar7 = *(uint *)(f1 + 0x5c);
      }
      if (((uVar7 & 0x80000) == 0) && (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c))))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= 0xff00;
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      break;
    case 3:
switchD_801306ac_caseD_3:
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
        uVar7 = *(uint *)(f0 + 0x5c);
      }
      else {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
        uVar7 = *(uint *)(f1 + 0x5c);
      }
      if (((uVar7 & 0x80000) == 0) && (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c))))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
      }
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
      break;
    case 4:
      if (((*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) && ((*f1 & 0x80) == 0)) ||
         ((*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0xb && ((*f0 & 0x80) != 0)))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
        uVar7 = *(uint *)(f0 + 0x5c);
      }
      else {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
        uVar7 = *(uint *)(f1 + 0x5c);
      }
      if (((uVar7 & 0x80000) == 0) && (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c))))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= 0xff00;
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      vec3_step_toward_q(auStack_60,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),0x40);
    }
    break;
  case 0xc:
  case 0xd:
switchD_8012fb70_caseD_c:
    camera_track_step((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    break;
  case 0xe:
  case 0x10:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0x400;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0xff00;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x300;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((undefined4)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= 0;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0xe) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x124)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x128)));
      iVar9 = *(int *)(f0 + 0x20);
    }
    else {
LAB_8013062c:
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x124)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x128)));
      iVar9 = *(int *)(f1 + 0x20);
    }
    goto code_r0x8013065c;
  case 0xf:
  case 0x11:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0xf) {
      if ((*f1 & 0x80) != 0) {
        uVar7 = *(uint *)(f1 + 0x68);
LAB_80130574:
        if ((uVar7 & 0x2000) != 0) goto switchD_8012fb70_caseD_c;
      }
    }
    else if ((*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0x11) && ((*f0 & 0x80) != 0)) {
      uVar7 = *(uint *)(f0 + 0x68);
      goto LAB_80130574;
    }
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0xfe00;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((undefined4)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= 0;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))!= 0xf) goto LAB_8013062c;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x124)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x128)));
    iVar9 = *(int *)(f0 + 0x20);
code_r0x8013065c:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 300)));
    if (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)))) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
    }
    break;
  default:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))!= 0) break;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
    goto LAB_80131e98;
  case 0x1c:
    ang_prev[0] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
    ang_prev[1] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
    ang_prev[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
    uVar7 = *(uint *)(f0 + 0x5c);
    goto LAB_80131c60;
  case 0x1d:
    ang_prev[0] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
    ang_prev[1] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
    ang_prev[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
    uVar7 = *(uint *)(f1 + 0x5c);
LAB_80131c60:
    if (((uVar7 & 0x80000) == 0) && (-0x100 < ang_prev[1])) {
      ang_prev[1] = -0x100;
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= (&DAT_8011f944)[*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 4];
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
           *(undefined2 *)(&DAT_8011f946 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
           *(undefined2 *)(&DAT_8011f948 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      target_angles_depth_set(ang_prev,cam);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
    }
    else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 1) {
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= ang_prev[0];
      local_90[0] = (int)ang_prev[0] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= ang_prev[1];
      local_90[1] = (int)ang_prev[1] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= ang_prev[2];
      local_90[2] = (int)ang_prev[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e)));
      iVar9 = FUN_80194630((undefined4 *)local_90,(undefined4 *)local_90);
      if (iVar9 != 0) {
        iVar9 = FUN_80194500(iVar9);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))+ (short)((uint)(local_90[0] * iVar9) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))+ (short)((uint)(local_90[1] * iVar9) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))+ (short)((uint)(local_90[2] * iVar9) >> 0x10);
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      vec3_step_toward_q(auStack_60,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),0x40);
      target_depth_set(ang_prev,cam);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
LAB_80131e94:
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
LAB_80131e98:
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
      }
      else {
        iVar11 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60)));
        iVar8 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
        iVar9 = iVar11 - iVar8;
        if (iVar11 < iVar8) {
          if (iVar9 < 0) {
            iVar9 = -iVar9;
          }
          if (iVar9 < 0) {
            iVar9 = iVar9 + 3;
          }
          iVar9 = iVar9 << 0xe;
        }
        else {
LAB_80131278:
          iVar11 = iVar11 - iVar8;
          if (iVar11 < 0) {
            iVar11 = -iVar11;
          }
          if (iVar11 < 0) {
            iVar11 = iVar11 + 0xf;
          }
          iVar9 = iVar11 << 0xc;
        }
LAB_8013129c:
        u16_step_toward((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68))),iVar9 >> 0x10);
      }
    }
    break;
  case 0x1e:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
    }
    else {
      iVar9 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)))) * 0x100000 >> 0x14
      ;
      if (iVar9 < 0) {
        iVar9 = -iVar9;
      }
      if (iVar9 < 0) {
        iVar9 = iVar9 + 0x1f;
      }
      fixed_approach_s16((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32))),(iVar9 << 0xb) >> 0x10);
    }
    break;
  case 0x20:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      local_80[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
      local_80[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
      local_80[2] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
      if (((*(uint *)(f0 + 0x5c) & 0x80000) == 0) && (-0x100 < local_80[1])) {
        local_80[1] = -0x100;
      }
      local_70[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
      local_70[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
      local_70[2] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
      if (((*(uint *)(f1 + 0x5c) & 0x80000) == 0) && (-0x100 < local_70[1])) {
        local_70[1] = -0x100;
      }
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= (&DAT_8011f944)[*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 4];
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
           *(undefined2 *)(&DAT_8011f946 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
           *(undefined2 *)(&DAT_8011f948 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      midpoint_angles_set(local_80,local_70,cam);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
    }
    else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))!= 1) break;
    goto switchD_801306ac_caseD_1;
  case 0x21:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd8)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xdc)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd8)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xdc)));
    }
    else {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x38) ))+ -0x180;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x3c)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x34)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x38)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x3c)));
    }
    target_angles_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
      goto LAB_801315ec;
    }
LAB_801315d4:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
    goto LAB_801315f4;
  case 0x22:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd8)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xdc)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd8)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xdc)));
    }
    else {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x38) ))+ -0x180;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x3c)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x34)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x38)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x3c)));
    }
    target_angles_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))!= 0) goto LAB_801315d4;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
LAB_801315ec:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
LAB_801315f4:
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ -0x400;
    goto LAB_80131f4c;
  case 0x23:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x34)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
    uVar5 = (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))+ 0x1000) - *(short *)(f0 + 0x4e) & 0xfff;
    goto LAB_80131b64;
  case 0x24:
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
         *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
    uVar5 = (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))+ 0x1000) - *(short *)(f1 + 0x4e) & 0xfff;
LAB_80131b64:
    *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= uVar5;
    *(ushort *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= uVar5;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
    goto LAB_80131f4c;
  }
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
LAB_80131f4c:
  sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x84)));
  iVar9 = sVar6 * -3;
  if (iVar9 < 0) {
    iVar9 = iVar9 + 3;
  }
  sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x86)));
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x84) ))= (short)(iVar9 >> 2);
  iVar9 = sVar2 * -3;
  if (iVar9 < 0) {
    iVar9 = iVar9 + 3;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x86) ))= (short)(iVar9 >> 2);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0x48) ))= 0x1000;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0x4c) ))= 0;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0x50) ))= 0x1000;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0x54) ))= 0;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((battle + 0x58) ))= 0x1000;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x5c) ))= -(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a)));
  iVar9 = battle + 0x28;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x60) ))= -(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)));
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 100) ))= -(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e)));
  rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(uint *)guest_at((uint32_t)iVar9));
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))+ (int)sVar2;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x40) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))+ ((int)sVar6 << 8) / 10000;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x44) ))=
       mips_div_lo((int32_t)((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))<< 0xc),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7a)))); /* guest div @0x80132030 */
  rtps_basis_xform((undefined4 *)guest_at((uint32_t)iVar9),(undefined2 *)guest_at((uint32_t)(battle + 0x48)),(uint *)guest_at((uint32_t)iVar9));
  rtps_basis_xform((undefined4 *)guest_at((uint32_t)cam),(undefined2 *)guest_at((uint32_t)iVar9),(uint *)guest_at((uint32_t)iVar9));
  return;

}
void battle_hud_draw(int unused,int fighter,int aux0,int aux1)
{
  short sVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  int iVar6;
  int iVar7;
  uint *puVar8;
  uint *puVar9;
  int *piVar10;
  uint uVar11;
  undefined4 uVar12;
  undefined1 *puVar13;
  uint *puVar14;
  short *psVar15;
  uint *puVar16;
  short *psVar17;
  int iVar18;
  int iVar19;
  int iVar20;
  undefined2 *puVar21;
  int iVar22;
  int iVar23;
  int local_80 [2];
  uint *rectbl;
  uint *rectbl2;
  uint *rectbl3;
  uint *local_6c;
  int local_68;
  int *local_64;
  int local_60;
  undefined1 *local_5c;
  int local_58;
  undefined4 *local_54;
  int local_50;
  int *local_4c;
  int local_48;
  undefined1 *local_44;
  int local_40;
  int local_3c;
  int local_38;
  uint *local_30;
  
  iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc)));
  rectbl2 = (uint *)guest_at((uint32_t)(uintptr_t)((iVar4 * 0xc0 + -0x7fe0b0d0)));
  rectbl3 = (uint *)guest_at((uint32_t)(uintptr_t)((iVar4 * 0x90 + -0x7fe0af50)));
  rectbl = (uint *)guest_at(*(uint32_t *)(&DAT_801f5210 + iVar4 * 4));
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc4) ))+ 8) ))>> 1 & 1) == 0) {
    _DAT_801f4f20 = _DAT_801f4f20 + 1;
    local_80[0] = aux0;
    local_80[1] = aux1;
    if ((int)_DAT_801f4f20 < 0x21) {
      sprite_strip_draw_dual(_DAT_801f5210 + 0x78,_DAT_801f5214 + 0x78,_DAT_801f4f20 * -0x80 + 0x1000,0x10,
                   &DAT_801b4a48,0,0xffffff80,0);
      sprite_strip_draw_dual(_DAT_801f5210 + 0x2f8,_DAT_801f5214 + 0x2f8,_DAT_801f4f20 * -0x80 + 0x1000,0x10,
                   &DAT_801b4ac8,0,0x80,0);
      sprite_strip_draw_dual(_DAT_801f5210 + 0x6b8,_DAT_801f5214 + 0x6b8,_DAT_801f4f20 * -0x80 + 0x1000,8,
                   &DAT_801b4be4,0,0xffffff80,0);
      sprite_strip_draw_dual(_DAT_801f5210,_DAT_801f5214,_DAT_801f4f20 * -0x80 + 0x1000,1,&DAT_801b4b48,0,
                   0xffffff80,1);
      sprite_strip_draw_dual(_DAT_801f5210 + 0x28,_DAT_801f5214 + 0x28,_DAT_801f4f20 * -0x80 + 0x1000,2,
                   &DAT_801b4b5c,0,0xffffff80,1);
      sprite_strip_draw_dual(_DAT_801f5210 + 0x578,_DAT_801f5214 + 0x578,_DAT_801f4f20 * -0x80 + 0x1000,2,
                   &DAT_801b4c30,0,0x80,0);
      sprite_strip_draw_dual(0x801f50b0,0x801f5140,_DAT_801f4f20 * -0x80 + 0x1000,2,&DAT_801b4ccc,0,0xffffff80
                   ,3);
      sprite_strip_draw_dual(_DAT_801f5210 + 0x618,_DAT_801f5214 + 0x618,_DAT_801f4f20 * -0x80 + 0x1000,2,
                   &DAT_801b4cec,0,0x80,0);
      sprite_strip_draw_dual(0x801f4f30,0x801f4ff0,_DAT_801f4f20 * -0x80 + 0x1000,2,&DAT_801b4cbc,0,0xffffff80
                   ,2);
      sprite_strip_draw_dual(0x801f4f60,0x801f5020,_DAT_801f4f20 * -0x80 + 0x1000,2,&DAT_801b4cdc,0,0x80,2);
    }
    puVar16 = rectbl + 10;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((fighter + 0x94) ))< 0) {
      iVar4 = -1;
    }
    else {
      iVar4 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((fighter + 0x94) ))/ 0x32) * 0x10000 >> 0x10;
    }
    hud_digits_color(iVar4,(int)guest_of(puVar16));
    hud_cell_frame_set(iVar4,(int)guest_of(puVar16));
    iVar4 = 0;
    do {
      iVar4 = iVar4 + 1;
      *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(puVar16) & 0xffffff;
      puVar16 = puVar16 + 10;
    } while (iVar4 < 2);
    *rectbl = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
    *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(rectbl) & 0xffffff;
    if (0x20 < (int)_DAT_801f4f20) {
      uVar12 = 0x50;
      if (*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0') {
        uVar12 = 0x28;
      }
      hud_text_blk_alt(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                   (const void *)guest_at(((uint32_t *)&PTR_DAT_801b47ec)[(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((aux0 + 3) ))* 2]),0x22,uVar12,0);
      hud_text_blk_alt(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                   (const void *)guest_at(((uint32_t *)&PTR_DAT_801b47ec)[(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((aux1 + 3) ))* 2]),
                   (int)*(short *)(&DAT_801b4a30 + (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((aux1 + 3) ))* 2),uVar12,0);
    }
    iVar19 = 0;
    puVar13 = &DAT_801f51d8;
    iVar22 = 0x7f8;
    iVar18 = 0;
    local_38 = 0;
    iVar20 = 0x5c8;
    puVar16 = (uint *)guest_at((uint32_t)(uintptr_t)((*(int *)(&DAT_801f5210 + *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))* 4) + 0x5c8)));
    iVar4 = aux0;
    do {
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xd6) ))< 0x1e00) || (0x1dff < *(short *)(puVar13 + 0x12))) {
LAB_80173dd8:
        if (-1 < *(short *)(puVar13 + 0x1a)) goto LAB_80173de8;
      }
      else {
        if (*(short *)(puVar13 + 0x1a) < 0) {
          if (*(char *)guest_at((uint32_t)(uintptr_t)((iVar4 + 5) ))== '\0') {
            task_alloc_wrap_a(_DAT_801f5210 + iVar20 + 0xe,_DAT_801f5214 + iVar20 + 0xe, (undefined4)br2_gaddr(&DAT_801b4de8),
                          (undefined4)br2_gaddr(&DAT_801b4e08));
            *(undefined2 *)(puVar13 + 0x18) = 0x1000;
            *(undefined2 *)(puVar13 + 0x1a) = 0x78;
            piVar10 = (int *)((intptr_t)local_80 + iVar18);
            triple_track_spawn(*(undefined1 *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 2))),0x30,0);
            iVar4 = fx_group_alloc(1,1,0,0x13);
            if (iVar4 != 0) {
              noop_a(iVar4,fighter + iVar19 * 2 + 0x6c,*(int *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 0x20) ))+ 0x124,
                            (undefined4)br2_gaddr(&DAT_801f0760));
            }
            group_fx_spawn(*piVar10,0x1b,*(int *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 0x20) ))+ 0x124,0xe00,0x2000,0,5);
          }
          goto LAB_80173dd8;
        }
LAB_80173de8:
        sprite_rect_set_c((int)guest_of(puVar16),(int)*(short *)((intptr_t)&DAT_801b4c88 + local_38) +
                             ((int)*(short *)((intptr_t)&DAT_801b4c9c + iVar18) *
                              (int)*(short *)(puVar13 + 0x18) >> 0xc),
                     (int)*(short *)((intptr_t)&DAT_801b4c8a + local_38) +
                     ((int)*(short *)((intptr_t)&DAT_801b4c9e + iVar18) * (int)*(short *)(puVar13 + 0x18)
                     >> 0xc),(int)*(short *)((intptr_t)&DAT_801b4c8c + local_38),
                     (int)*(short *)((intptr_t)&DAT_801b4c8e + local_38));
        if (0x50 < *(short *)(puVar13 + 0x1a)) {
          *(short *)(puVar13 + 0x18) = (short)(*(short *)(puVar13 + 0x18) * 0x1a9 >> 9);
        }
        if (*(short *)(puVar13 + 0x1a) == 100) {
          piVar10 = (int *)(&DAT_801f4f28 + iVar18);
          if (*piVar10 != 0) {
            task_node_free((int *)(uintptr_t)*piVar10);
          }
          iVar4 = task_alloc_wrap_b(_DAT_801f5210 + iVar22,_DAT_801f5214 + iVar22,(undefined4)br2_gaddr(piVar10), (undefined4)br2_gaddr(&DAT_801b4dd4),
                                (undefined4)br2_gaddr(&DAT_801b4dbc));
          *piVar10 = iVar4;
        }
        if (*(short *)(puVar13 + 0x1a) < 0x28) {
          *(short *)(puVar13 + 0x18) = *(short *)(puVar13 + 0x18) + 500;
        }
        *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
        *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= guest_of(puVar16) & 0xffffff;
        *(short *)(puVar13 + 0x1a) = *(short *)(puVar13 + 0x1a) + -1;
      }
      puVar16 = puVar16 + 10;
      puVar13 = puVar13 + 0x1c;
      iVar22 = iVar22 + 0x28;
      iVar18 = iVar18 + 4;
      iVar20 = iVar20 + 0x28;
      iVar19 = iVar19 + 1;
      local_38 = local_38 + 8;
      iVar4 = aux1;
    } while (iVar19 < 2);
    psVar15 = &DAT_801b4ccc;
    if ((-1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99)))) && (0x20 < (int)_DAT_801f4f20)) {
      iVar18 = 0;
      piVar10 = local_80;
      iVar4 = -0x7fe0af50;
      puVar13 = &DAT_801f51d8;
      psVar17 = &DAT_801b4cd2;
      puVar5 = rectbl3 + 8;
      puVar16 = rectbl3;
      do {
        iVar19 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 0xac)));
        if (-1 < iVar19) {
          *(undefined2 *)(puVar13 + 4) = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 0xac)));
          iVar19 = sprite_rect_set_b((int)guest_of(puVar16),(int)*psVar15,(int)psVar17[-2],
                                mips_div_lo((int32_t)((int)psVar17[-1] * (*(short *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 0xb0) ))- iVar19)),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 0xb0)))), (int)*psVar17);
          if (iVar19 == 0) {
            if (iVar18 == 0) {
              _DAT_801f50c0 = (short)puVar5[-6] + -1;
              _DAT_801f50d0 = (short)puVar5[-6] + -1;
              _DAT_801f5150 = (short)puVar5[-6] + -1;
              _DAT_801f5160 = (short)puVar5[-6] + -1;
            }
            else {
              _DAT_801f50e4 = (short)puVar5[-6] + 1;
              _DAT_801f50f4 = (short)puVar5[-6] + 1;
              _DAT_801f5174 = (short)puVar5[-6] + 1;
              _DAT_801f5184 = (short)puVar5[-6] + 1;
            }
          }
          else {
            iVar19 = (*(uint *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 0x90 + iVar4;
            *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x10) ))= puVar5[-4];
            *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x20) ))= *puVar5;
          }
          if (-1 < *(short *)guest_at((uint32_t)(uintptr_t)((*piVar10 + 0xac)))) {
            *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x8000000;
            *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(puVar16) & 0xffffff;
          }
        }
        puVar5 = puVar5 + 9;
        puVar16 = puVar16 + 9;
        psVar17 = psVar17 + 4;
        psVar15 = psVar15 + 4;
        piVar10 = piVar10 + 1;
        iVar4 = iVar4 + 0x24;
        iVar18 = iVar18 + 1;
        puVar13 = puVar13 + 0x1c;
      } while (iVar18 < 2);
    }
    psVar15 = &DAT_801b4ccc;
    puVar16 = rectbl2 + 0x18;
    if ((0x20 < (int)_DAT_801f4f20) && (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99))))) {
      iVar19 = 0;
      puVar13 = &DAT_801f51d8;
      puVar8 = (uint *)0x0;
      psVar17 = &DAT_801b4cd2;
      puVar14 = rectbl2 + 0x1c;
      iVar4 = aux1;
      iVar18 = aux0;
      puVar5 = rectbl3;
      do {
        uVar3 = *(uint *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1;
        if (((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0x58) ))& 0x8000) == 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x5c) ))& 0x40) == 0)) {
          sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xac)));
          *(undefined2 *)(puVar13 + 6) = 1;
          *(int *)(puVar13 + 0xc) = (int)sVar2;
        }
        puVar9 = puVar8;
        if (*(short *)(puVar13 + 6) != 0) {
          iVar4 = *(int *)(puVar13 + 8) + 2;
          if (*(int *)(puVar13 + 0xc) < *(int *)(puVar13 + 8)) {
            *(int *)(puVar13 + 8) = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xac)));
          }
          else {
            *(int *)(puVar13 + 8) = iVar4;
            if (*(int *)(puVar13 + 0xc) <= iVar4) {
              *(int *)(puVar13 + 8) = *(int *)(puVar13 + 0xc);
              *(undefined2 *)(puVar13 + 6) = 0;
            }
            local_30 = (uint *)(uintptr_t)puVar8;
            sprite_rect_set_a((int)guest_of(puVar16),(int)*psVar15,(int)psVar17[-2],
                         mips_div_lo((int32_t)((int)psVar17[-1] *
                         ((int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xb0) ))- *(int *)(puVar13 + 8))),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xb0)))), (int)*psVar17); /* guest div @0x80174294 */
            puVar8[uVar3 * 0x30 + -0x1ff82c19] = puVar14[-1];
            puVar8[uVar3 * 0x30 + -0x1ff82c17] = puVar14[1];
            puVar9 = local_30;
          }
        }
        *(short *)(puVar14 + -2) = (short)puVar5[4];
        *(short *)puVar14 = (short)puVar5[8];
        puVar8[uVar3 * 0x30 + -0x1ff82c1a] = puVar14[-2];
        puVar8[uVar3 * 0x30 + -0x1ff82c18] = *puVar14;
        if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xac)))) {
          *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x5000000;
          *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(puVar16) & 0xffffff;
        }
        puVar14 = puVar14 + 6;
        puVar16 = puVar16 + 6;
        psVar17 = psVar17 + 4;
        psVar15 = psVar15 + 4;
        puVar5 = puVar5 + 9;
        puVar13 = puVar13 + 0x1c;
        iVar19 = iVar19 + 1;
        puVar8 = puVar9 + 6;
        iVar4 = aux0;
        iVar18 = aux1;
      } while (iVar19 < 2);
    }
    psVar15 = &DAT_801b4ccc;
    puVar16 = rectbl2 + 0x24;
    if ((0x20 < (int)_DAT_801f4f20) && (iVar4 = 0, -1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99))))) {
      puVar13 = &DAT_801f51d8;
      iVar19 = 0;
      psVar17 = &DAT_801b4cd2;
      puVar8 = rectbl2 + 0x25;
      puVar5 = rectbl2;
      iVar18 = aux0;
      do {
        iVar20 = (*(uint *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 0xc0 + -0x7fe0b040 + iVar19;
        if (*(short *)(puVar13 + 0x10) != *(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xaa)))) {
          *(undefined2 *)(puVar13 + 0x10) = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xaa)));
          local_30 = (uint *)(uintptr_t)puVar5;
          sprite_rect_set_a((int)guest_of(puVar16),(int)*psVar15,(int)psVar17[-2],
                       mips_div_lo((int32_t)((int)psVar17[-1] *
                       ((int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xb0) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xaa))))),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xb0)))), (int)*psVar17); /* guest div @0x80174448 */
          *(uint *)guest_at((uint32_t)(uintptr_t)((iVar20 + 8) ))= puVar8[1];
          *(uint *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0xc) ))= puVar8[2];
          *(uint *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x10) ))= puVar8[3];
          *(uint *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x14) ))= puVar8[4];
          puVar5 = local_30;
        }
        if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xac)))) {
          *(short *)(puVar8 + 1) = (short)puVar5[0x1b];
          *(short *)(puVar8 + 3) = (short)puVar5[0x1d];
          *(uint *)guest_at((uint32_t)(uintptr_t)((iVar20 + 8) ))= puVar8[1];
          *(uint *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x10) ))= puVar8[3];
          if ((short)puVar8[1] != (short)puVar8[2]) {
            if (((*(byte *)guest_at((uint32_t)(uintptr_t)((iVar18 + 5) ))& 0x10) == 0) || ((_DAT_801f4f20 & 1) != 0)) {
              uVar3 = 0x28a06040;
            }
            else {
              uVar3 = 0x28ffdcdc;
            }
            *puVar8 = uVar3;
            *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x5000000;
            *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(puVar16) & 0xffffff;
          }
        }
        puVar8 = puVar8 + 6;
        puVar16 = puVar16 + 6;
        psVar17 = psVar17 + 4;
        psVar15 = psVar15 + 4;
        puVar5 = puVar5 + 6;
        puVar13 = puVar13 + 0x1c;
        iVar4 = iVar4 + 1;
        iVar19 = iVar19 + 0x18;
        iVar18 = aux1;
      } while (iVar4 < 2);
    }
    psVar15 = &DAT_801b4cec;
    iVar18 = 0;
    psVar17 = &DAT_801b4cf2;
    local_44 = &DAT_801f51d8;
    local_40 = 0;
    local_3c = 0x618;
    puVar5 = rectbl + 0x186;
    puVar16 = rectbl3 + 0x12;
    puVar8 = rectbl + 0x18e;
    puVar14 = rectbl3 + 0x1a;
    iVar4 = aux0;
    do {
      if ((0x1dff < *(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xd6)))) && ((*(byte *)guest_at((uint32_t)(uintptr_t)((iVar4 + 5) ))& 0x10) == 0)) {
        uVar3 = _DAT_801f4f20;
        if ((int)_DAT_801f4f20 < 0) {
          uVar3 = _DAT_801f4f20 + 3;
        }
        *(ushort *)((intptr_t)puVar8 + -0x12) = (ushort)((uVar3 & 0x1c) << 4) | 0x7a1d;
      }
      sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xd6)));
      *(short *)(local_44 + 0x12) = sVar2;
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xd6) ))< 0x1e00) || ((*(byte *)guest_at((uint32_t)(uintptr_t)((iVar4 + 5) ))& 0x10) != 0)) {
        iVar19 = (*(uint *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 0x90 + -0x7fe0af08 + (local_40 + iVar18) * 4;
        if ((*(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xd6) ))< 0x1e00) && ((*(byte *)guest_at((uint32_t)(uintptr_t)((iVar4 + 5) ))& 0x10) == 0)) {
          iVar22 = (int)psVar17[-1] * (int)*(short *)(local_44 + 0x12);
          sVar2 = *psVar15;
          sVar1 = *psVar17;
          iVar23 = (int)psVar17[-2] + (uint)(*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0');
          iVar20 = (int)((ulonglong)((longlong)iVar22 * 0x88888889) >> 0x20);
        }
        else {
          iVar22 = (int)psVar17[-1] * (int)*(short *)(local_44 + 0x12);
          iVar20 = (int)((ulonglong)((longlong)iVar22 * 0x4445678f) >> 0x20);
          sVar2 = *psVar15;
          sVar1 = *psVar17;
          iVar23 = (int)psVar17[-2] + (uint)(*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0');
        }
        sprite_rect_set_b((int)guest_of(puVar16),(int)sVar2,iVar23,(iVar20 >> 0xc) - (iVar22 >> 0x1f),(int)sVar1);
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 8) ))= puVar14[-6];
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x10) ))= puVar14[-4];
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x18) ))= puVar14[-2];
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x20) ))= *puVar14;
      }
      else {
        iVar19 = *(int *)(&DAT_801f5210 + (*(uint *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 4) + local_3c;
        sprite_rect_set_c((int)guest_of(puVar5),(int)*psVar15,
                     (int)psVar17[-2] + (uint)(*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0'),
                     ((int)psVar17[-1] * (int)sVar2) / 0x3bff,(int)*psVar17);
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 8) ))= puVar8[-6];
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x10) ))= puVar8[-4];
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x18) ))= puVar8[-2];
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x20) ))= *puVar8;
      }
      if (0x20 < (int)_DAT_801f4f20) {
        if ((*(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xd6) ))< 0x1e00) || ((*(byte *)guest_at((uint32_t)(uintptr_t)((iVar4 + 5) ))& 0x10) != 0)) {
          *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x8000000;
          iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8)));
          puVar9 = puVar16;
        }
        else {
          *puVar5 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
          iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8)));
          puVar9 = puVar5;
        }
        *(uint *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x10) ))= guest_of(puVar9) & 0xffffff;
      }
      puVar14 = puVar14 + 9;
      puVar16 = puVar16 + 9;
      puVar8 = puVar8 + 10;
      puVar5 = puVar5 + 10;
      psVar17 = psVar17 + 4;
      psVar15 = psVar15 + 4;
      iVar18 = iVar18 + 1;
      local_44 = local_44 + 0x1c;
      local_40 = local_40 + 8;
      local_3c = local_3c + 0x28;
      iVar4 = aux1;
    } while (iVar18 < 2);
    iVar18 = 0;
    iVar4 = 0x7f8;
    piVar10 = (int *)&DAT_801f4f28;
    puVar16 = rectbl;
    do {
      if (*piVar10 != 0) {
        puVar16[0x1fe] = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
        *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= (guest_of(rectbl) + iVar4) & 0xffffff;
      }
      iVar4 = iVar4 + 0x28;
      puVar16 = puVar16 + 10;
      iVar18 = iVar18 + 1;
      piVar10 = piVar10 + 1;
    } while (iVar18 < 2);
    iVar19 = 0;
    iVar18 = 0x578;
    local_5c = &DAT_801f51d8;
    local_60 = 0;
    local_58 = 0;
    local_54 = (undefined4 *)&DAT_801f51d0;
    local_50 = 0;
    local_4c = (int *)&DAT_801f5218;
    local_48 = 0;
    puVar16 = rectbl + 0x15e;
    iVar4 = aux0;
    do {
      sVar2 = *(short *)(local_5c + 0x14);
      iVar20 = fighter_height_check(iVar4);
      if (sVar2 != iVar20) {
        if (*local_4c != 0) {
          task_node_free((int *)(uintptr_t)*local_4c);
          *local_4c = 0;
        }
        piVar10 = (int *)(&DAT_801f51d0 + local_60);
        if (*piVar10 != 0) {
          task_node_free((int *)(uintptr_t)*piVar10);
          *piVar10 = 0;
        }
        sVar2 = fighter_height_check(iVar4);
        iVar20 = local_50;
        iVar4 = local_58;
        *(short *)(local_5c + 0x14) = sVar2;
        if (sVar2 == 1) {
          iVar23 = 0;
          iVar22 = 0x1b;
          if (iVar19 == 0) {
            iVar22 = 0x13;
          }
          iVar22 = iVar22 * 0x28;
          do {
            iVar23 = iVar23 + 1;
            iVar6 = iVar22 + _DAT_801f5210;
            iVar7 = iVar22 + _DAT_801f5214;
            iVar22 = iVar22 + 0x28;
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0xe) ))= 0x781e;
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0xe) ))= 0x781e;
          } while (iVar23 < 8);
          iVar22 = 0;
          psVar15 = (short *)((intptr_t)&DAT_801b4c30 + local_58);
          piVar10 = (int *)&DAT_801f5210;
          do {
            iVar22 = iVar22 + 1;
            sprite_rect_set_c(*piVar10 + iVar20 + 0x578,(int)*psVar15,
                         (int)*(short *)((intptr_t)&DAT_801b4c32 + iVar4),
                         (int)*(short *)((intptr_t)&DAT_801b4c34 + iVar4),
                         (int)*(short *)((intptr_t)&DAT_801b4c36 + iVar4));
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar20 + *piVar10 + 0x586) ))= 0x791d;
            piVar10 = piVar10 + 1;
          } while (iVar22 < 2);
          uVar12 = task_alloc_wrap_b(_DAT_801f5210 + iVar18,_DAT_801f5214 + iVar18,0, (undefined4)br2_gaddr(&DAT_801b4d34),
                                 (undefined4)br2_gaddr(&DAT_801b4d28));
          *local_54 = uVar12;
        }
        else if (sVar2 < 2) {
          iVar20 = 0;
          if (sVar2 == 0) {
            iVar22 = 0x1b;
            if (iVar19 == 0) {
              iVar22 = 0x13;
            }
            iVar22 = iVar22 * 0x28;
            do {
              iVar20 = iVar20 + 1;
              iVar7 = iVar22 + _DAT_801f5210;
              iVar23 = iVar22 + _DAT_801f5214;
              iVar22 = iVar22 + 0x28;
              *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xe) ))= 0x791e;
              *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0xe) ))= 0x791e;
            } while (iVar20 < 8);
            iVar22 = 0;
            iVar20 = -0x7fe0af08;
            do {
              iVar23 = iVar20 + local_48;
              iVar20 = iVar20 + 0x90;
              iVar22 = iVar22 + 1;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 4) ))= 0x20;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 5) ))= 0xf0;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 6) ))= 0xff;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xc) ))= 0x20;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xd) ))= 0xf0;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xe) ))= 0xff;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x14) ))= 0;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x15) ))= 0x40;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x16) ))= 0xc0;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x1c) ))= 0;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x1d) ))= 0x40;
              *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x1e) ))= 0xc0;
            } while (iVar22 < 2);
            iVar20 = 0;
            psVar15 = (short *)(&DAT_801b4c50 + local_58);
            piVar10 = (int *)&DAT_801f5210;
            do {
              iVar22 = *piVar10;
              piVar10 = piVar10 + 1;
              iVar20 = iVar20 + 1;
              sprite_rect_set_c(iVar22 + iVar18,(int)*psVar15,(int)*(short *)(&DAT_801b4c52 + iVar4),
                           (int)*(short *)(&DAT_801b4c54 + iVar4),
                           (int)*(short *)(&DAT_801b4c56 + iVar4));
            } while (iVar20 < 2);
            iVar4 = task_alloc_wrap_a(_DAT_801f5210 + iVar18 + 0xe,_DAT_801f5214 + iVar18 + 0xe,
                                  (undefined4)br2_gaddr(&DAT_801b4d7c), (undefined4)br2_gaddr(&DAT_801b4d90));
            *local_4c = iVar4;
            uVar12 = task_alloc_wrap_b(_DAT_801f5210 + iVar18,_DAT_801f5214 + iVar18,0, (undefined4)br2_gaddr(&DAT_801b4dac),
                                   (undefined4)br2_gaddr(&DAT_801b4d98));
            *local_54 = uVar12;
          }
        }
        else if (sVar2 == 2) {
          iVar22 = 0;
          iVar20 = 0x1b;
          if (iVar19 == 0) {
            iVar20 = 0x13;
          }
          iVar20 = iVar20 * 0x28;
          do {
            iVar22 = iVar22 + 1;
            iVar7 = iVar20 + _DAT_801f5210;
            iVar23 = iVar20 + _DAT_801f5214;
            iVar20 = iVar20 + 0x28;
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xe) ))= 0x795e;
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0xe) ))= 0x795e;
          } while (iVar22 < 8);
          iVar22 = 0;
          iVar20 = -0x7fe0af08;
          do {
            iVar23 = iVar20 + local_48;
            iVar20 = iVar20 + 0x90;
            iVar22 = iVar22 + 1;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 4) ))= 0xff;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 5) ))= 0xf0;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 6) ))= 0x20;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xc) ))= 0xff;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xd) ))= 0xf0;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0xe) ))= 0x20;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x14) ))= 0xa0;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x15) ))= 0x60;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x16) ))= 0;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x1c) ))= 0xa0;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x1d) ))= 0x60;
            *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar23 + 0x1e) ))= 0;
          } while (iVar22 < 2);
          iVar20 = 0;
          psVar15 = (short *)(&DAT_801b4c40 + local_58);
          piVar10 = (int *)&DAT_801f5210;
          do {
            iVar22 = *piVar10;
            piVar10 = piVar10 + 1;
            iVar20 = iVar20 + 1;
            sprite_rect_set_c(iVar22 + iVar18,(int)*psVar15,(int)*(short *)(&DAT_801b4c42 + iVar4),
                         (int)*(short *)(&DAT_801b4c44 + iVar4),
                         (int)*(short *)(&DAT_801b4c46 + iVar4));
          } while (iVar20 < 2);
          iVar4 = task_alloc_wrap_a(_DAT_801f5210 + iVar18 + 0xe,_DAT_801f5214 + iVar18 + 0xe,
                                (undefined4)br2_gaddr(&DAT_801b4d3c), (undefined4)br2_gaddr(&DAT_801b4d50));
          *local_4c = iVar4;
          uVar12 = task_alloc_wrap_b(_DAT_801f5210 + iVar18,_DAT_801f5214 + iVar18,0, (undefined4)br2_gaddr(&DAT_801b4d6c),
                                (int)guest_of(&DAT_801b4d58));
          *local_54 = uVar12;
        }
      }
      if ((*(short *)(local_5c + 0x1a) < 100) && (*(int *)(&DAT_801f4f28 + local_60) == 0)) {
        *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
        *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= guest_of(puVar16) & 0xffffff;
      }
      puVar16 = puVar16 + 10;
      iVar18 = iVar18 + 0x28;
      iVar19 = iVar19 + 1;
      local_60 = local_60 + 4;
      local_5c = local_5c + 0x1c;
      local_58 = local_58 + 8;
      local_54 = local_54 + 1;
      local_50 = local_50 + 0x28;
      local_4c = local_4c + 1;
      local_48 = local_48 + 0x24;
      iVar4 = aux1;
    } while (iVar19 < 2);
    iVar4 = 0;
    puVar16 = rectbl + 0x1e;
    do {
      iVar4 = iVar4 + 1;
      *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(puVar16) & 0xffffff;
      puVar16 = puVar16 + 10;
    } while (iVar4 < 0x10);
    iVar4 = 0;
    do {
      iVar4 = iVar4 + 1;
      *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= guest_of(puVar16) & 0xffffff;
      puVar16 = puVar16 + 10;
    } while (iVar4 < 0x10);
    iVar18 = 0;
    iVar4 = 0x30;
    puVar16 = rectbl2;
    do {
      iVar18 = iVar18 + 1;
      *puVar16 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x5000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(puVar16) & 0xffffff;
      uVar3 = (intptr_t)rectbl2 + iVar4;
      iVar4 = iVar4 + 0x18;
      puVar16[0xc] = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x5000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= uVar3 & 0xffffff;
      puVar16 = puVar16 + 6;
    } while (iVar18 < 2);
    iVar4 = 0;
    if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99)))) {
      iVar19 = 0x6b8;
      puVar13 = &DAT_801f51d8;
      iVar18 = aux1;
      do {
        if (*(ushort *)(puVar13 + 0x16) != (ushort)*(byte *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xe4)))) {
          iVar22 = 0;
          iVar20 = (intptr_t)rectbl + iVar19;
          if (*(byte *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xe4) ))!= 0) {
            do {
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0xc) ))= DAT_801b4c28;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0xd) ))= DAT_801b4c29;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x14) ))= DAT_801b4c28 + DAT_801b4c2a;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x15) ))= DAT_801b4c29;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x1c) ))= DAT_801b4c28;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x1d) ))= DAT_801b4c29 + DAT_801b4c2b;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x24) ))= DAT_801b4c28 + DAT_801b4c2a;
              iVar22 = iVar22 + 1;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x25) ))= DAT_801b4c29 + DAT_801b4c2b;
              iVar20 = iVar20 + 0x28;
            } while (iVar22 < (int)(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xe4))));
          }
          if (iVar22 < (int)(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4)))) {
            iVar18 = iVar22 * 0x28 + (intptr_t)rectbl + iVar19;
            do {
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xc) ))= DAT_801b4c24;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xd) ))= DAT_801b4c25;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0x14) ))= DAT_801b4c24 + DAT_801b4c26;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0x15) ))= DAT_801b4c25;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0x1c) ))= DAT_801b4c24;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0x1d) ))= DAT_801b4c25 + DAT_801b4c27;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0x24) ))= DAT_801b4c24 + DAT_801b4c26;
              *(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0x25) ))= DAT_801b4c25 + DAT_801b4c27;
              iVar22 = iVar22 + 1;
              iVar18 = iVar18 + 0x28;
            } while (iVar22 < (int)(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4))));
          }
        }
        iVar19 = iVar19 + 0xa0;
        iVar4 = iVar4 + 1;
        puVar13 = puVar13 + 0x1c;
        iVar18 = aux0;
      } while (iVar4 < 2);
      iVar4 = 0;
      puVar16 = rectbl + 0x1ae;
      do {
        iVar18 = 0;
        puVar5 = puVar16;
        if (*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4) ))!= '\0') {
          do {
            *puVar5 = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
            *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(puVar5) & 0xffffff;
            iVar18 = iVar18 + 1;
            puVar5 = puVar5 + 10;
          } while (iVar18 < (int)(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4))));
        }
        iVar4 = iVar4 + 1;
        puVar16 = puVar16 + 0x28;
      } while (iVar4 < 2);
    }
    task_pools_tick(0);
    uVar3 = 0;
    local_30 = (uint *)0x801f5220;
    iVar22 = 0;
    iVar23 = 4;
    puVar21 = &DAT_801b4e20;
    local_68 = 0;
    local_64 = (int *)&DAT_801f0940;
    local_6c = rectbl;
    iVar4 = aux1;
    iVar18 = aux0;
    iVar19 = (intptr_t)local_30;
    iVar20 = (intptr_t)local_30;
    do {
      iVar7 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xdd) ))+ *local_64;
      if (iVar7 != 0) {
        sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xac)));
        piVar10 = (int *)((intptr_t)local_30 + (uVar3 ^ 1) * 4);
        *piVar10 = (int)sVar2;
        if (sVar2 < 0) {
          *piVar10 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0xb0)));
        }
      }
      if (*(char *)guest_at((uint32_t)(uintptr_t)((iVar18 + 0xdd) ))== '\0') {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x30) ))= 0;
        *local_64 = 0;
      }
      else if ((1 < iVar7) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x58) ))& 0x8000) != 0)) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x30) ))== 0) {
          iVar18 = 0;
          iVar4 = iVar22;
          do {
            *(undefined2 *)((intptr_t)local_30 + iVar4 + 0x10) = *puVar21;
            iVar18 = iVar18 + 1;
            *(undefined2 *)((intptr_t)local_30 + iVar4 + 0x1c) =
                 *(undefined2 *)((intptr_t)&DAT_801b4e20 + iVar23);
            iVar4 = iVar4 + 2;
          } while (iVar18 < 3);
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x2c) ))= 0x50;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28) ))= 2;
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x30) ))= 1;
        }
        iVar4 = 0;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28) ))< iVar7) {
          iVar18 = iVar22;
          do {
            *(undefined2 *)((intptr_t)local_30 + iVar18 + 0x10) = *puVar21;
            iVar4 = iVar4 + 1;
            *(undefined2 *)((intptr_t)local_30 + iVar18 + 0x1c) =
                 *(undefined2 *)((intptr_t)&DAT_801b4e20 + iVar23);
            iVar18 = iVar18 + 2;
          } while (iVar4 < 2);
          *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x2c) ))= 0x78;
          *(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28) ))= (short)iVar7;
        }
      }
      if (99 < *(byte *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28)))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28) ))= 99;
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((fighter + 0x82) ))== 0) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x2c) ))!= 0) {
          *(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x2c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x2c) ))+ -1;
          goto LAB_8017544c;
        }
      }
      else {
LAB_8017544c:
        iVar18 = 0;
        iVar4 = iVar22;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x2c) ))!= 0) {
          do {
            iVar6 = (intptr_t)local_30 + iVar4;
            iVar7 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1c) ))* 0xed8;
            *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x10) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x10) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1c)));
            if (iVar7 < 0) {
              iVar7 = iVar7 + 0xfff;
            }
            *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1c) ))= (short)(iVar7 >> 0xc);
            iVar18 = iVar18 + 1;
            iVar4 = iVar4 + 2;
          } while (iVar18 < 3);
          sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28)));
          /* 0x801B4E10 / 0x801B4E14 are guest POINTER cells (lw in the guest):
           * each holds a guest address of a 1-char digit buffer. Deref the
           * u32 entry, then the pointed-to byte — never the host ** value. */
          if (sVar2 < 10) {
            *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b4e10)) = 0x20;
          }
          else {
            *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b4e10)) =
                 (((char)((int)sVar2 / 10) + (char)(sVar2 >> 7)) - (char)(sVar2 >> 0xf)) + '0';
          }
          *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b4e14)) =
               (char)*(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28) ))+ (char)((int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar19 + 0x28) ))/ 10) * -10 +
               '0';
          uVar12 = 0x5e;
          if (*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0') {
            uVar12 = 0x2f;
          }
          announce_param_block_init_b(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                       (const void *)guest_at(*(uint32_t *)guest_at(0x801b4e10)),(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x10))),uVar12,1);
          announce_param_block_init_b(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                       (const void *)guest_at(*(uint32_t *)guest_at(0x801b4e14)),(int)((*(ushort *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x12) ))+ 0x10) * 0x10000) >> 0x10
                       ,uVar12,1);
          uVar11 = guest_of(rectbl) + (uint32_t)local_68 + 0x668;
          sprite_rect_set_c(uVar11,*(short *)guest_at((uint32_t)(uintptr_t)((iVar20 + 0x14) ))+ 0x24,0x72,0x20,0x18);
          local_6c[0x19a] = *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
          *(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= uVar11 & 0xffffff;
        }
      }
      iVar20 = iVar20 + 6;
      iVar22 = iVar22 + 6;
      iVar19 = iVar19 + 2;
      iVar23 = iVar23 + 2;
      puVar21 = puVar21 + 1;
      uVar3 = uVar3 + 1;
      local_6c = local_6c + 10;
      local_68 = local_68 + 0x28;
      local_64 = local_64 + 1;
      iVar4 = aux0;
      iVar18 = aux1;
    } while ((int)uVar3 < 2);
  }
  return;

}
extern uint8_t *g_DAT_801b1590, *g_DAT_801b1592, *g_DAT_801b55d0, *g_DAT_801f596d, *g_DAT_801f5971;
#define DAT_801b1590   (*g_DAT_801b1590)
#define DAT_801b1592   (*g_DAT_801b1592)
#define DAT_801b55d0   (*g_DAT_801b55d0)
#define DAT_801f596d   (*g_DAT_801f596d)
#define DAT_801f5971   (*g_DAT_801f5971)
extern uint32_t *g__DAT_801f0292, *g__DAT_801f02f6, *g__DAT_801f02f8, *g__DAT_801f04e4, *g__DAT_801f04e6, *g__DAT_801f04e8, *g__DAT_801f04ea, *g__DAT_801f04ec, *g__DAT_801f04ee, *g__DAT_801f04f0, *g__DAT_801f04f2, *g__DAT_801f04f4, *g__DAT_801f04f6, *g__DAT_801f04f8, *g__DAT_801f04fa;
#define _DAT_801f0292   (*g__DAT_801f0292)
#define _DAT_801f02f6   (*(uint16_t *)g__DAT_801f02f6) /* guest sh @0x801f02f6 (0x80143d38) */
#define _DAT_801f02f8   (*(uint16_t *)g__DAT_801f02f8) /* guest sh @0x801f02f8 */
#define _DAT_801f04e4   (*(int16_t *)g__DAT_801f04e4) /* guest lh/sh @0x801f04e4 (0x8014642c) */
#define _DAT_801f04e6   (*(int16_t *)g__DAT_801f04e6) /* guest lh/sh @0x801f04e6 */
#define _DAT_801f04e8   (*(int16_t *)g__DAT_801f04e8) /* guest lh/sh @0x801f04e8 */
#define _DAT_801f04ea   (*(int16_t *)g__DAT_801f04ea) /* guest lh/sh @0x801f04ea */
#define _DAT_801f04ec   (*(int16_t *)g__DAT_801f04ec) /* guest sh @0x801f04ec */
#define _DAT_801f04ee   (*(int16_t *)g__DAT_801f04ee) /* guest sh @0x801f04ee */
#define _DAT_801f04f0   (*(int16_t *)g__DAT_801f04f0) /* guest sh @0x801f04f0 */
#define _DAT_801f04f2   (*(int16_t *)g__DAT_801f04f2) /* guest sh @0x801f04f2 */
#define _DAT_801f04f4   (*(int16_t *)g__DAT_801f04f4) /* guest lh/sh @0x801f04f4 */
#define _DAT_801f04f6   (*(int16_t *)g__DAT_801f04f6) /* guest lh/sh @0x801f04f6 */
#define _DAT_801f04f8   (*(int16_t *)g__DAT_801f04f8) /* guest lh/sh @0x801f04f8 */
#define _DAT_801f04fa   (*(int16_t *)g__DAT_801f04fa) /* guest lh/sh @0x801f04fa */
extern void trig_lut_read(uint32_t,byte *);
extern void trig_vel_flat(uint32_t,byte *);
extern void trig_vel_up(uint32_t,byte *);
extern void trig_vel_diag(uint32_t,byte *);

extern uint8_t *g_DAT_1f80006c, *g_DAT_1f8000b1, *g_DAT_1f80038c, *g_DAT_1f800398, *g_DAT_1f8003a4, *g_DAT_1f8003b0, *g_DAT_801b1bd4, *g_DAT_801b1be8, *g_DAT_801b5288, *g_DAT_801b5298, *g_DAT_801b52a8, *g_DAT_801b52b8, *g_DAT_801b5370, *g_DAT_801cf88c, *g_DAT_801f0ae0, *g_DAT_801f0db0, *g_DAT_801f0fb0;
#define DAT_1f80006c   (*g_DAT_1f80006c)
#define DAT_1f8000b1   (*g_DAT_1f8000b1)
#define DAT_1f80038c   (*g_DAT_1f80038c)
#define DAT_1f800398   (*g_DAT_1f800398)
#define DAT_1f8003a4   (*g_DAT_1f8003a4)
#define DAT_1f8003b0   (*g_DAT_1f8003b0)
#define DAT_801b1bd4   (*g_DAT_801b1bd4)
#define DAT_801b1be8   (*g_DAT_801b1be8)
#define DAT_801b5288   (*g_DAT_801b5288)
#define DAT_801b5298   (*g_DAT_801b5298)
#define DAT_801b52a8   (*g_DAT_801b52a8)
#define DAT_801b52b8   (*g_DAT_801b52b8)
#define DAT_801b5370   (*g_DAT_801b5370)
#define DAT_801cf88c   (*g_DAT_801cf88c)
#define DAT_801f0ae0   (*g_DAT_801f0ae0)
#define DAT_801f0db0   (*g_DAT_801f0db0)
#define DAT_801f0fb0   (*g_DAT_801f0fb0)
extern uint32_t *g__DAT_1f8000c4, *g__DAT_1f80038c, *g__DAT_1f800390, *g__DAT_1f800394, *g__DAT_1f800398, *g__DAT_1f80039c, *g__DAT_1f8003a0, *g__DAT_1f8003a4, *g__DAT_1f8003a8, *g__DAT_1f8003ac, *g__DAT_1f8003b0, *g__DAT_1f8003b2, *g__DAT_1f8003b4, *g__DAT_1f8003b6, *g__DAT_1f8003b8, *g__DAT_1f8003ba, *g__DAT_1f8003bc, *g__DAT_1f8003be, *g__DAT_1f8003c0, *g__DAT_801c4578, *g__DAT_801f0780, *g__DAT_801f0784, *g__DAT_801f0788, *g__DAT_801f0790, *g__DAT_801f0794, *g__DAT_801f0798;
#define _DAT_1f80038c   (*g__DAT_1f80038c)
#define _DAT_1f800390   (*g__DAT_1f800390)
#define _DAT_1f800394   (*g__DAT_1f800394)
#define _DAT_1f800398   (*g__DAT_1f800398)
#define _DAT_1f80039c   (*g__DAT_1f80039c)
#define _DAT_1f8003a0   (*g__DAT_1f8003a0)
#define _DAT_1f8003a4   (*g__DAT_1f8003a4)
#define _DAT_1f8003a8   (*g__DAT_1f8003a8)
#define _DAT_1f8003ac   (*g__DAT_1f8003ac)
#define _DAT_1f8003b0   (*g__DAT_1f8003b0)
#define _DAT_1f8003b2   (*g__DAT_1f8003b2)
#define _DAT_1f8003b4   (*g__DAT_1f8003b4)
#define _DAT_1f8003b6   (*g__DAT_1f8003b6)
#define _DAT_1f8003b8   (*g__DAT_1f8003b8)
#define _DAT_1f8003ba   (*g__DAT_1f8003ba)
#define _DAT_1f8003bc   (*g__DAT_1f8003bc)
#define _DAT_1f8003be   (*g__DAT_1f8003be)
#define _DAT_1f8003c0   (*g__DAT_1f8003c0)
#define _DAT_801c4578   (*g__DAT_801c4578)
#define _DAT_801f0780   (*g__DAT_801f0780)
#define _DAT_801f0784   (*g__DAT_801f0784)
#define _DAT_801f0788   (*g__DAT_801f0788)
#define _DAT_801f0790   (*g__DAT_801f0790)
#define _DAT_801f0794   (*g__DAT_801f0794)
#define _DAT_801f0798   (*g__DAT_801f0798)
extern int FUN_80194690();

extern uint8_t *g_DAT_8011ec40, *g_DAT_801b1820, *g_DAT_801b1824, *g_DAT_801b1828, *g_DAT_801b182c, *g_DAT_801b1830, *g_DAT_801b1880, *g_DAT_801b18a8, *g_DAT_801b18d0, *g_DAT_801b18f8, *g_DAT_801b1904, *g_DAT_801b192c, *g_DAT_801b1a18, *g_DAT_801b1a2c, *g_DAT_801b1a3c, *g_DAT_801b1a50, *g_DAT_801b1a74, *g_DAT_801b1a94, *g_DAT_801b1ab8, *g_DAT_801b1ad8, *g_DAT_801b1b1c, *g_DAT_801b1b5c, *g_DAT_801b1b78, *g_DAT_801b1b90, *g_DAT_801b1bb4, *g_DAT_801b1bfc, *g_DAT_801b1c30, *g_DAT_801b1c60, *g_DAT_801b1c74, *g_DAT_801b1cbc, *g_DAT_801b1ccc, *g_DAT_801b2330, *g_DAT_801b2332, *g_DAT_801b2334, *g_DAT_801b2338, *g_DAT_801b2340, *g_DAT_801b2342, *g_DAT_801b2344, *g_DAT_801b2348, *g_DAT_801c43a8, *g_DAT_801e83aa, *g_DAT_801e83b0, *g_DAT_801f0740;
#define DAT_8011ec40   (*g_DAT_8011ec40)
#define DAT_801b1820   (*g_DAT_801b1820)
#define DAT_801b1824   (*g_DAT_801b1824)
#define DAT_801b1828   (*g_DAT_801b1828)
#define DAT_801b182c   (*g_DAT_801b182c)
#define DAT_801b1830   (*g_DAT_801b1830)
#define DAT_801b1880   (*g_DAT_801b1880)
#define DAT_801b18a8   (*g_DAT_801b18a8)
#define DAT_801b18d0   (*g_DAT_801b18d0)
#define DAT_801b18f8   (*g_DAT_801b18f8)
#define DAT_801b1904   (*g_DAT_801b1904)
#define DAT_801b192c   (*g_DAT_801b192c)
#define DAT_801b1a18   (*g_DAT_801b1a18)
#define DAT_801b1a2c   (*g_DAT_801b1a2c)
#define DAT_801b1a3c   (*g_DAT_801b1a3c)
#define DAT_801b1a50   (*g_DAT_801b1a50)
#define DAT_801b1a74   (*g_DAT_801b1a74)
#define DAT_801b1a94   (*g_DAT_801b1a94)
#define DAT_801b1ab8   (*g_DAT_801b1ab8)
#define DAT_801b1ad8   (*g_DAT_801b1ad8)
#define DAT_801b1b1c   (*g_DAT_801b1b1c)
#define DAT_801b1b5c   (*g_DAT_801b1b5c)
#define DAT_801b1b78   (*g_DAT_801b1b78)
#define DAT_801b1b90   (*g_DAT_801b1b90)
#define DAT_801b1bb4   (*g_DAT_801b1bb4)
#define DAT_801b1bfc   (*g_DAT_801b1bfc)
#define DAT_801b1c30   (*g_DAT_801b1c30)
#define DAT_801b1c60   (*g_DAT_801b1c60)
#define DAT_801b1c74   (*g_DAT_801b1c74)
#define DAT_801b1cbc   (*g_DAT_801b1cbc)
#define DAT_801b1ccc   (*g_DAT_801b1ccc)
#define DAT_801b2330   (*g_DAT_801b2330)
#define DAT_801b2332   (*g_DAT_801b2332)
#define DAT_801b2334   (*g_DAT_801b2334)
#define DAT_801b2338   (*g_DAT_801b2338)
#define DAT_801b2340   (*g_DAT_801b2340)
#define DAT_801b2342   (*g_DAT_801b2342)
#define DAT_801b2344   (*g_DAT_801b2344)
#define DAT_801b2348   (*g_DAT_801b2348)
#define DAT_801c43a8   (*g_DAT_801c43a8)
#define DAT_801e83aa   (*g_DAT_801e83aa)
#define DAT_801e83b0   (*g_DAT_801e83b0)
#define DAT_801f0740   (*g_DAT_801f0740)
extern uint32_t *g__DAT_801f0748;
#define _DAT_801f0748   (*g__DAT_801f0748)
extern uint8_t  **g_PTR_DAT_801b19d4, **g_PTR_DAT_801b1e8c, **g_PTR_FUN_801b1ea8;
#define PTR_DAT_801b19d4   (*g_PTR_DAT_801b19d4)
#define PTR_DAT_801b1e8c   (*g_PTR_DAT_801b1e8c)
#define PTR_FUN_801b1ea8   (*g_PTR_FUN_801b1ea8)

void fighter_state_tick(uint *ctrl,undefined4 pad,int ctx)
{
  short tmr;
  ushort padf;
  int hdg;
  ushort afl;
  uint st;
  uint tv;
  
  st = ctrl[0xe];
  padf = 0;
  if (((ctrl[0x17] & 0x40) == 0) && ((ctrl[0x18] & 0x10) == 0)) {
    hdg = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctx + 0x22) ))+ (int)*(short *)((intptr_t)ctrl + 0x4e);
  }
  else {
    hdg = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctx + 0x22) ))+ (int)*(short *)guest_at((uint32_t)(uintptr_t)((st + 0xd0)));
  }
  tv = -((hdg << 0x14) >> 0x1f);
  if ((ctrl[0x1b] & 0x2000) != 0) {
    tv = (uint)(tv == 0);
  }
  if ((*ctrl & 0x80) == 0) {
    padf = attr_flag_mask(*(byte *)((intptr_t)ctrl + 2));
  }
  if (*(byte *)((intptr_t)ctrl + 1) == 0) {
    afl = padf;
    if (tv != 0) {
      afl = padf & 0x5fff;
      if ((padf & 0x8000) != 0) {
        afl = padf & 0x5fff | 0x2000;
      }
      if ((padf & 0x2000) != 0) {
        afl = afl | 0x8000;
      }
    }
    padf = afl & 0x8000;
    if ((((ctrl[0x18] & 0x400000) != 0) || (padf = afl & 0x8000, (ctrl[0x15] & 1) == 0)) ||
       (padf = afl & 0x8000, (*ctrl & 0x80) != 0)) goto LAB_8017fca0;
    afl = afl | 0x800;
  }
  else {
    afl = 0;
    if ((*ctrl & 0x8080) == 0) {
      fighter_script_fsm((int)guest_of(ctrl),st,(int *)guest_at(ctrl[0xf]),pad);
      afl = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl[0xf] + 6)));
      padf = afl & 0x8000;
      goto LAB_8017fca0;
    }
  }
  padf = afl & 0x8000;
LAB_8017fca0:
  if (padf != 0) {
    afl = afl | 0x400;
  }
  if ((int)ctrl[0x18] < 0) {
    afl = afl & 0xfff;
  }
  if ((ctrl[0x18] & 0x20000) != 0) {
    afl = afl & 0xf000;
  }
  if (tv != 0) {
    afl = afl | 0x200;
  }
  padf = pad_hist_record(pad,*(byte *)((intptr_t)ctrl + 2),afl);
  tv = ctrl[0x1f];
  *(ushort *)(ctrl + 0x1f) = padf;
  *(ushort *)((intptr_t)ctrl + 0x7e) = (ushort)tv;
  *(ushort *)((intptr_t)ctrl + 0x7a) = padf & (padf ^ (ushort)tv);
  tmr = *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1e)));
  if ((*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1e) ))!= 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1e) ))= tmr + -1, tmr == 1)) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((st + 0x1c) ))= 0xffff;
  }
  tmr = *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36)));
  if ((*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36) ))!= 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36) ))= tmr + -1, tmr == 1)) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((st + 0x34) ))= 0xffff;
  }
  tmr = *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x26)));
  if (((*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x26) ))!= 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x8000) == 0)) &&
     (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x26) ))= tmr + -1, tmr == 1)) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((st + 0x24) ))= 0xffff;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((st + 0x2a) ))= 0xffff;
  }
  if ((-1 < (int)ctrl[0x16]) && (*(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7c) ))!= *(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7d))))) {
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))= 0;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((st + 0x7c) ))= 0;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((st + 0x7e) ))= 0xff;
  }
  if ((ctrl[0x1e] & 0x880000) == 0x80000) {
    ctrl[0x18] = ctrl[0x18] | 0x40000;
    table_1f0ae0_clear(*(byte *)((intptr_t)ctrl + 2));
    ctrl[0x1b] = ctrl[0x1b] & 0xfdffffff;
  }
  else if (-1 < (int)ctrl[0x16]) {
    ctrl[0x18] = ctrl[0x18] & 0xfffbffff;
  }
  pad_edge_scan((int)br2_gaddr(ctrl));
  if ((ctrl[0x18] & 0x40000) == 0) {
    cmd_input_parse((int)br2_gaddr(ctrl));
  }
  action_from_input((int)br2_gaddr(ctrl));
  return;

}

undefined4 fighter_atk_dir(int atk,int def)
{
  int dist;
  undefined4 res;
  int st;
  
  st = *(int *)guest_at((uint32_t)(uintptr_t)((atk + 0x38)));
  if (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x24) ))< 0) {
    return 0;
  }
  if (*(int *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))== 0) {
    return 0;
  }
  if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xce)))) {
    return 0;
  }
  if (((*(ushort *)guest_at((uint32_t)(uintptr_t)((atk + 0x7a) ))& 8) == 0) && ((*(ushort *)guest_at((uint32_t)(uintptr_t)((atk + 0x7c) ))& 8) != 0)) {
    return 0;
  }
  if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x10008000) != 0) &&
      ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) == 0)) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 100) ))& 0x8000) == 0)) {
    return 0;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x1040000) != 0) {
    return 0;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))& 0x40) != 0) {
    return 0;
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x100) != 0) {
    if (-0x180 < *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((atk + 0x20) ))+ 0x178)))) {
      return 0;
    }
    if (-0x180 < *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((def + 0x20) ))+ 0x178)))) {
      return 0;
    }
    dist = (int)*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x78)));
    if (dist < 0) {
      dist = -dist;
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x28) ))< dist) {
      return 0;
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))& 0x80) == 0) {
      return 0;
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x1000) == 0) {
      return 0;
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x4000) != 0) {
      return 0;
    }
  }
  if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x80) != 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x1800) != 0)) ||
     (((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0xa0000000) == 0xa0000000 ||
      ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))& 0x4000) == 0)))) {
    return 0;
  }
  if ((((*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd0) ))== 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x60) ))& 0x40000000) == 0)) &&
      ((((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x400000) != 0 &&
        ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x80080000) != 0x80080000)) ||
       (((-1 < *(int *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))|| ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 100) ))& 0x8000) != 0)) ||
        (0 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbc))))))))) &&
     (((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x76) ))<= *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x28) ))&&
       ((ushort)(*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x74) ))+ 0xaaU) < 0x155)) &&
      ((((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x1000) != 0 && ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x400000) == 0))
       || (((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x2000) != 0 && ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x400000) != 0)
           ))))))) {
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x200) != 0) {
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) != 0) {
        return 0xffffffff;
      }
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 1) != 0) {
        return 1;
      }
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x400) != 0) {
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) != 0) {
        return 0xffffffff;
      }
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x30) != 0) {
        return 1;
      }
    }
    if (((*(uint *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x800) == 0) ||
       (res = 1, (*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) == 0)) {
      res = 0xffffffff;
    }
    return res;
  }
  return 0xffffffff;

}

void hit_pose_swap(int f0,int f1)
{
  undefined2 tmp;
  int anim;
  
  if (*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))!= '\0') {
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= 0;
    *(byte *)guest_at((uint32_t)(uintptr_t)((f1 + 0x100) ))= *(byte *)guest_at((uint32_t)(uintptr_t)((f1 + 0x100) ))^ 1;
    anim = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18)));
    tmp = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((anim + 0x23c)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((anim + 0x23c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((anim + 0x1c4)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x1c4) ))= tmp;
    anim = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18)));
    tmp = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((anim + 0x28c)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((anim + 0x28c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((anim + 0x264)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x264) ))= tmp;
  }
  return;

}

undefined4 hit_elig_check(int f0,int f1)
{
  uint fb;
  uint fa;
  
  if ((((*(char *)guest_at((uint32_t)(uintptr_t)((f0 + 0xdc) ))== '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((f0 + 0x26) ))== -1)) ||
      (*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 0x27) ))== -1)) ||
     (((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))& 4) != 0 || ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x80080000) != 0)))) {
    return 0;
  }
  fa = *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 100)));
  fb = *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58)));
  if ((fa & 0x80) == 0) {
    if ((fa & 0x40) == 0) {
      if ((fa & 0x100) != 0) {
        if ((fb & 0x10) != 0) {
          return 0;
        }
        if ((fb & 0x20) != 0) {
          return 0;
        }
      }
    }
    else if ((fb & 0x4000) != 0) {
      return 0;
    }
  }
  else if ((fb & 0x30) == 0x20) {
    return 0;
  }
  return 1;

}

undefined4 hitbox_pair_test(int f0,int f1,undefined2 *out)
{
  char cVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  /* Guest stores the sign-extended hit-sphere center as 3 contiguous
   * words at sp+0x18/0x1c/0x20 and passes sp+0x18 to seg_sphere_hit,
   * which reads a3[0..2]. Ghidra split the vec3 into 3 scalars —
   * keep it one array (local_60[i] == old local_5X). */
  int local_60[3];
  int local_50;
  int local_4c;
  int local_48;
  uint local_44;
  int local_40;
  int local_3c;
  int local_38;
  undefined2 *local_30;
  
  local_48 = 0;
  uVar2 = *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58)));
  iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x14)));
  local_40 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x10)));
  local_3c = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
  iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
  if (((uVar2 & 0x40000) != 0) ||
     ((((uVar2 & 0x30000) != 0 && ((uVar2 & 0x8000000) == 0)) &&
      ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x80000080) != 0x80)))) {
    local_48 = 1;
  }
  local_50 = 0;
  local_38 = 0;
  local_4c = (int)*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 10)));
  local_44 = *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 100) ))& 0x1000;
  local_30 = out;
  do {
    cVar1 = *(char *)guest_at((uint32_t)(uintptr_t)((local_38 + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x14))))));
    if (cVar1 == -1) {
      return 0;
    }
    if ((cVar1 != -2) && ((local_48 == 0 || (local_44 != 0)))) {
      iVar3 = *(char *)guest_at((uint32_t)(uintptr_t)((iVar5 + 1) ))* 0x1c;
      local_60[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar3 + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0xe)));
      local_60[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar3 + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0x10)));
      local_60[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar3 + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0x12)));
      iVar3 = local_3c + *(char *)guest_at((uint32_t)(uintptr_t)((iVar5 + 1) ))* 0x50 + 0x34;
      if (iVar3 != 0) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 2) ))== 1) {
          if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x40080) == 0) {
            iVar3 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x76)));
            if (iVar3 < 0) {
              iVar3 = -iVar3;
            }
            if (iVar3 < 0x300) {
              *local_30 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x34)));
              local_30[1] = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x38)));
              local_30[2] = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x3c)));
              return 4;
            }
          }
        }
        else {
          iVar7 = 0;
          iVar6 = local_40;
          if (0 < local_4c) {
            do {
              iVar4 = iVar8 + *(char *)guest_at((uint32_t)(uintptr_t)((iVar6 + 1) ))* 0x50 + 0x34;
              if (iVar4 != 0) {
                if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0x800) == 0) {
                  iVar4 = seg_sphere_hit((int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 2))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 2))),(int *)guest_at((uint32_t)(uintptr_t)(iVar3)),
                                       local_60,(int *)guest_at((uint32_t)(uintptr_t)(iVar4)),local_30);
                }
                else {
                  iVar4 = sphere_pt_hit((int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 2))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 2))),(int *)guest_at((uint32_t)(uintptr_t)(iVar3)),
                                       (int *)guest_at((uint32_t)(uintptr_t)(iVar4)),local_30);
                }
                if (iVar4 != 0) {
                  return 1;
                }
              }
              iVar7 = iVar7 + 1;
              iVar6 = iVar6 + 4;
            } while (iVar7 < local_4c);
          }
        }
      }
    }
    local_38 = local_38 + 4;
    iVar5 = iVar5 + 4;
    local_50 = local_50 + 1;
    if (7 < local_50) {
      return 0;
    }
  } while( true );

}

uint fighter_range_code(int f0,int f1)
{
  int rel;
  uint code;
  
  if (*(char *)guest_at((uint32_t)(uintptr_t)((f0 + 0xdb) ))== '\0') {
    rel = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x4e) ))+ (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x74)));
  }
  else {
    rel = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x4e) ))+ (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0xc6)));
  }
  rel = ((*(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x4e) ))- rel) + -0x800) * 0x100000 >> 0x14;
  code = 4;
  if (rel < 0) {
    code = 8;
    if (-rel < 0x155) {
      return 9;
    }
  }
  else if (rel < 0x155) {
    return 5;
  }
  if (rel < 0) {
    rel = -rel;
  }
  if (0x4e3 < rel) {
    code = code | 2;
  }
  return code;

}

int hit_scale_calc(int f0,int f1)
{
  uint fa;
  int scale;
  
  fa = *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 100)));
  scale = 0x80;
  if ((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))< 0) &&
     ((*(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0xba) ))!= 0 || (*(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0xb8) ))!= 0)))) {
    scale = 0x99;
    *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))| 0x8000000;
  }
  if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x20) != 0) && ((fa & 0x40) != 0)) &&
     ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x10) == 0)) {
    return scale << 1;
  }
  if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))& 2) == 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 8) == 0)) &&
     (*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))!= 0x402)) {
    return scale;
  }
  return scale * 0x99 >> 7;

}

undefined4 hit_react_apply(int atk,int def,int scale,short *hits,int n)
{
  char cVar1;
  short sVar2;
  undefined2 uVar3;
  short sVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  undefined4 uVar15;
  
  *(undefined1 *)guest_at((uint32_t)(uintptr_t)((atk + 0x26) ))= 0xff;
  uVar12 = *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x68)));
  uVar14 = *(uint *)guest_at((uint32_t)(uintptr_t)((atk + 100)));
  if (n == 4) {
    iVar5 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))* (0x300 - *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x76))));
    if (iVar5 < 0) {
      iVar5 = iVar5 + 0x1ff;
    }
    *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))= (char)(iVar5 >> 9);
    if (0x13 < (iVar5 >> 9 & 0xffU)) {
      uVar14 = uVar14 | 0x1000000;
    }
  }
  iVar5 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))* scale;
  if (iVar5 < 0) {
    iVar5 = iVar5 + 0x7f;
  }
  scale = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xdc) ))* scale;
  iVar11 = iVar5 >> 7;
  if (scale < 0) {
    scale = scale + 0x7f;
  }
  uVar13 = scale >> 7;
  if (n == 2) {
    debug_strike_fx_a(hits,0,0,uVar13,atk);
    return 1;
  }
  if (n == 3) {
    debug_strike_fx_b(hits,0,0,uVar13,atk);
    return 2;
  }
  uVar6 = 0x100000;
  if ((((uVar14 & 0x40) != 0) || (uVar6 = 0x400000, (uVar14 & 0x100) != 0)) ||
     (uVar6 = 0x200000, (uVar14 & 0x80) != 0)) {
    uVar12 = uVar12 | uVar6;
  }
  uVar6 = 0;
  if ((uVar14 & 0x4000) == 0) {
    uVar9 = *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c)));
    if ((uVar9 & 1) != 0) {
      if ((uVar14 & 0x40) == 0) {
        if ((uVar14 & 0x80) == 0) {
          if ((uVar14 & 0x100) != 0) {
            if ((uVar9 & 0x80) == 0) goto LAB_80143f44;
            if ((uVar9 & 0x40000000) == 0) {
              uVar6 = 0x42000000;
            }
          }
        }
        else if ((uVar9 & 0x80) == 0) {
LAB_80143f44:
          uVar9 = *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 1;
          if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x20) == 0) goto LAB_80143f58;
        }
        else if ((uVar9 & 0x40000000) == 0) {
          uVar6 = 0x42000000;
        }
      }
      else if ((uVar9 & 0x80) == 0) {
        uVar8 = *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58)));
        if ((uVar8 & 0x10) == 0) {
          if (((uVar8 & 0x20) == 0) && (uVar9 = uVar14 & 0x80, (uVar8 & 1) != 0)) {
LAB_80143f58:
            if (uVar9 != 0) goto LAB_80143f60;
          }
        }
        else {
LAB_80143f60:
          uVar6 = 0x42000000;
        }
      }
      else if ((uVar9 & 0x40000000) == 0) {
        uVar6 = 0x42000000;
      }
      if (((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 2) != 0) && ((uVar6 & 0x2000000) != 0)) {
        uVar6 = uVar6 | 4;
      }
    }
  }
  if (((uVar14 & 0x20000) != 0) && ((uVar6 & 4) == 0)) {
    if ((uVar6 & 0x2000000) == 0) {
      uVar12 = uVar12 | 0x800400;
    }
    else {
      iVar11 = (iVar11 - (iVar5 >> 0x1f)) >> 1;
    }
    uVar6 = uVar6 & 0xbdffffff;
  }
  if (uVar6 != 0) {
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))=
         (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x74)))) * 0x10) >> 4;
    if (*(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))== '\0') {
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))= 0x40;
    }
    cVar1 = '\x01';
    if (((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000000) == 0) ||
       (cVar1 = *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))+ '\x01', *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))!= -1)) {
      *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))= cVar1;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)uVar13 * 0x60 + 0x800;
    if ((uVar14 & 0x20000) != 0) {
      if (((int)_DAT_801f0292 + (int)*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e)))) * 0x100000 < 0) {
        _DAT_801f02f6 = (undefined2)((int)(uVar13 * -3) / 2);
      }
      else {
        _DAT_801f02f6 = (undefined2)((int)(uVar13 * 3) / 2);
      }
    }
    _DAT_801f02f8 = 4;
    if ((*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) != 0) {
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))= (short)(-iVar11 / 10);
    }
    if ((uVar6 & 4) == 0) {
      iVar5 = ((int)(uVar13 - (scale >> 0x1f)) >> 1) + 5;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= (short)iVar5;
      if ((*(char *)guest_at((uint32_t)(uintptr_t)((def + 1) ))== '\0') && ((DAT_801b55d0 & 1) != 0)) {
        iVar5 = iVar5 * 0x10000;
        (&DAT_801f596d)[(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] =
             (char)(((iVar5 >> 0x10) - (iVar5 >> 0x1f)) >> 1);
        (&DAT_801f5971)[(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] = 1;
      }
      uVar3 = 10;
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))< 10) || (uVar3 = 0x18, 0x18 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe))))) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= uVar3;
      }
      sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))+ -2;
    }
    else {
      iVar5 = uVar13 * 3;
      if (iVar5 < 0) {
        iVar5 = iVar5 + 3;
      }
      iVar5 = (iVar5 >> 2) + 6;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= (short)iVar5;
      if ((*(char *)guest_at((uint32_t)(uintptr_t)((def + 1) ))== '\0') && ((DAT_801b55d0 & 1) != 0)) {
        iVar5 = iVar5 * 0x10000;
        (&DAT_801f596d)[(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] =
             (char)(((iVar5 >> 0x10) - (iVar5 >> 0x1f)) >> 1);
        (&DAT_801f5971)[(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] = 1;
      }
      uVar3 = 0xd;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))< 0xd) {
LAB_80144ac4:
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= uVar3;
      }
      else if (0x78 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe)))) {
        uVar3 = 0x78;
        goto LAB_80144ac4;
      }
      sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))+ -0xc;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= sVar4;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xcc) ))= 0;
    if ((*(byte *)guest_at((uint32_t)(uintptr_t)((def + 5) ))& 0x10) == 0) {
      sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+ (ushort)(*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))>> 3) * 0x3c;
    }
    else {
      sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+ (ushort)(*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))>> 3) * -0x3c;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))= sVar4;
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x800) != 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= 0xfd60;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0x92) ))= 0xe0;
      sVar4 = (short)mips_div_lo((int32_t)(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))* 1000),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 0x24)))); /* guest div @0x80144C14 */
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= sVar4;
      if ((*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) != 0) {
        *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= (short)((sVar4 * 3) / 2);
      }
    }
    uVar12 = uVar12 & 0xfffffcbf;
    uVar15 = 2;
    if ((int)uVar13 < 0x1a) {
      if ((int)uVar13 < 0x15) {
        uVar12 = uVar12 | 0x40;
      }
      else {
        uVar12 = uVar12 | 0x100;
      }
    }
    else {
      uVar12 = uVar12 | 0x200;
    }
    if ((uVar12 & 0x800000) == 0) {
      xform_accum_spawn(2,uVar13);
      debug_strike_fx_a(hits,(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))+ 0x400) * 0x100000 >> 0x14,
                   (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))<< 4,uVar13 | 0x1000,atk);
      uVar9 = 0x20;
    }
    else {
      xform_accum_spawn(0,uVar13);
      noop_fn(0x100000c,hits,0,200,0xf,1);
      uVar9 = 0x20;
    }
    goto LAB_80144d0c;
  }
  cVar1 = '\x01';
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x20000000) == 0) ||
     (cVar1 = *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))+ '\x01', *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))!= -1)) {
    *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))= cVar1;
  }
  iVar5 = iVar11 >> 0x1f;
  switch(*(undefined1 *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd)))) {
  case 0:
  case 1:
    goto switchD_8014402c_caseD_0;
  case 2:
    iVar7 = uVar13 * 9;
    iVar5 = (int)((ulonglong)((longlong)iVar7 * 0x66666667) >> 0x20);
    iVar10 = iVar11 << 3;
    goto LAB_80144130;
  case 3:
    iVar7 = uVar13 << 3;
    iVar5 = (int)((ulonglong)((longlong)iVar7 * 0x66666667) >> 0x20);
    iVar10 = iVar11 << 3;
    break;
  case 4:
    iVar7 = uVar13 * 7;
    iVar5 = (int)((ulonglong)((longlong)iVar7 * 0x66666667) >> 0x20);
    iVar10 = iVar11 * 7;
    break;
  case 5:
    uVar13 = (int)(uVar13 * 6) / 10;
    iVar5 = iVar11 * 6 >> 0x1f;
    iVar11 = (iVar11 * 6) / 10 + iVar5;
    goto LAB_801441c4;
  case 6:
    uVar13 = (int)(uVar13 - (scale >> 0x1f)) >> 1;
    iVar11 = iVar11 / 2;
    goto switchD_8014402c_caseD_0;
  case 7:
    iVar7 = uVar13 << 2;
    iVar5 = (int)((ulonglong)((longlong)iVar7 * 0x66666667) >> 0x20);
    iVar10 = iVar11 << 2;
    break;
  case 8:
    iVar7 = uVar13 * 3;
    iVar5 = (int)((ulonglong)((longlong)iVar7 * 0x66666667) >> 0x20);
    iVar10 = iVar11 << 1;
LAB_80144130:
    iVar10 = iVar10 + iVar11;
    break;
  case 9:
    uVar13 = (int)uVar13 / 5;
    iVar11 = iVar11 / 5 + iVar5;
    goto LAB_801441c4;
  default:
    uVar13 = (int)uVar13 / 10;
    iVar11 = iVar11 / 10 + iVar5;
LAB_801441c4:
    iVar11 = iVar11 - iVar5;
    goto switchD_8014402c_caseD_0;
  }
  uVar13 = (iVar5 >> 2) - (iVar7 >> 0x1f);
  iVar11 = iVar10 / 10;
switchD_8014402c_caseD_0:
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)uVar13 * 0x60 + 0x800;
  uVar6 = 0x24000000;
  if ((*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) == 0) {
    sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xfe)));
  }
  else {
    sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x100)));
  }
  iVar11 = iVar11 * sVar4;
  if (iVar11 < 0) {
    iVar11 = iVar11 + 0x7f;
  }
  if ((*(byte *)guest_at((uint32_t)(uintptr_t)((def + 5) ))& 0x10) == 0) {
    sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x102)));
  }
  else {
    sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x104)));
  }
  iVar5 = (iVar11 >> 7) * (int)sVar4;
  if (iVar5 < 0) {
    iVar5 = iVar5 + 0x7f;
  }
  iVar11 = iVar5 >> 7;
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x20000000) == 0) || ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x100) == 0))
  {
    if (((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x800) == 0) &&
       (((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x8030000) != 0 && ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 100) ))& 0x1000) == 0))
       )) {
      iVar5 = iVar11 * 3;
      iVar11 = iVar5 >> 2;
      if (iVar5 < 0) {
        iVar11 = (iVar5 + 3) >> 2;
      }
    }
  }
  else {
    iVar11 = (iVar11 - (iVar5 >> 0x1f)) >> 1;
  }
  if (uVar13 == 0) {
    uVar13 = 1;
  }
  if (iVar11 == 0) {
    iVar11 = 1;
  }
  if (((int)_DAT_801f0292 + (int)*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e)))) * 0x100000 < 0) {
    iVar5 = uVar13 * -3;
  }
  else {
    iVar5 = uVar13 * 3;
  }
  if (iVar5 < 0) {
    iVar5 = iVar5 + 3;
  }
  _DAT_801f02f6 = (undefined2)(iVar5 >> 2);
  uVar12 = uVar12 & 0xfffffcbf;
  _DAT_801f02f8 = 4;
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))= (short)(uVar13 + 6);
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))= (short)iVar11;
  if ((int)((uVar13 + 6) * 0x10000) >> 0x10 < 0x10) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))= 0x10;
  }
  sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2)));
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xcc) ))= (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))+ -5) * 0x10;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))< 0x1b) {
    sVar2 = sVar4 + -0x10;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))< 0x16) {
      uVar12 = uVar12 | 0x40;
    }
    else {
      uVar12 = uVar12 | 0x100;
      sVar2 = sVar4 + -0x15;
    }
  }
  else {
    uVar12 = uVar12 | 0x200;
    sVar2 = sVar4 + -0x1a;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))= sVar2;
  uVar9 = *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58)));
  if ((uVar9 & 0x800) != 0) {
LAB_801443f4:
    if ((uVar14 & 0x80000) == 0) {
      uVar8 = 0x840000;
      if (((uVar14 & 0x4000000) == 0) && (uVar8 = 0x810000, (uVar14 & 0x1000000) == 0)) {
        if ((uVar14 & 0x200000) == 0) {
          if ((uVar14 & 0x100000) == 0) {
            if ((uVar14 & 0x400000) == 0) {
              uVar8 = 0x804400;
            }
            else {
              uVar8 = 0x804400;
            }
          }
          else {
            uVar8 = 0x804400;
          }
        }
        else {
          uVar8 = 0x802400;
        }
      }
      else {
LAB_80144748:
        uVar8 = uVar8 | 0x400;
      }
    }
    else if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x80) == 0) {
      uVar8 = 0x800c00;
    }
    else {
      uVar8 = 0x804400;
    }
    goto LAB_8014474c;
  }
  uVar8 = 0x2000000;
  if ((uVar9 & 0x8030000) != 0) goto LAB_80144748;
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) != 0) goto LAB_801443f4;
  if ((uVar9 & 0x100) == 0) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0x92) ))= 0;
    if ((uVar14 & 0x80000) != 0) {
      if ((uVar14 & 0x10000) == 0) {
        if (((uVar12 & 0x8000000) != 0) || ((uVar12 & 0x800000) != 0)) {
LAB_801445d8:
          uVar12 = uVar12 | 0xc00;
          goto LAB_80144754;
        }
        uVar8 = 0x804400;
      }
      else {
        if ((uVar12 & 0x8000000) == 0) goto LAB_80144754;
        if ((uVar12 & 0x800000) != 0) goto LAB_801445d8;
        uVar8 = 0x804400;
      }
      goto LAB_8014474c;
    }
    if ((uVar14 & 0x400000) == 0) {
      if ((uVar14 & 0x200000) == 0) {
        if ((uVar14 & 0x1000000) == 0) {
          if ((uVar14 & 0x4000000) == 0) {
            if ((uVar14 & 0x100000) == 0) {
              if ((uVar14 & 0x800000) == 0) {
                if ((((uVar14 & 0x1000) != 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x30000) != 0)) &&
                   (uVar8 = 0x2000000, (*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x80) == 0)) goto LAB_80144748;
              }
              else if (((uVar14 & 0x10000) == 0) || ((uVar12 & 0x8000000) != 0)) {
                uVar12 = uVar12 | 0x8400;
              }
            }
            else if (((uVar14 & 0x10000) == 0) || ((uVar12 & 0x8000000) != 0)) {
              uVar12 = uVar12 | 0x1400;
            }
          }
          else {
            if ((uVar14 & 0x10000) == 0) {
              uVar8 = 0x840000;
              if ((uVar12 & 0x8000000) == 0) {
                uVar8 = 0x40000;
              }
              goto LAB_80144748;
            }
            if ((uVar12 & 0x8000000) != 0) {
              uVar8 = 0x40400;
              goto LAB_8014474c;
            }
          }
        }
        else {
          if ((uVar14 & 0x10000) == 0) {
            uVar8 = 0x810000;
            if ((uVar12 & 0x8000000) == 0) {
              uVar8 = 0x10000;
            }
            goto LAB_80144748;
          }
          if ((uVar12 & 0x8000000) != 0) {
            uVar8 = 0x10400;
            goto LAB_8014474c;
          }
        }
      }
      else {
        if ((uVar14 & 0x10000) == 0) {
          if ((uVar12 & 0x8000000) != 0) {
            uVar8 = 0x802400;
            goto LAB_8014474c;
          }
        }
        else if ((uVar12 & 0x8000000) == 0) goto LAB_80144754;
        uVar12 = uVar12 | 0x2400;
      }
    }
    else {
      if ((uVar14 & 0x10000) == 0) {
        if ((uVar12 & 0x8000000) != 0) {
          if ((uVar12 & 0x800000) != 0) goto LAB_801445d8;
          uVar8 = 0x804400;
          goto LAB_8014474c;
        }
      }
      else if ((uVar12 & 0x8000000) == 0) goto LAB_80144754;
      uVar12 = uVar12 | 0x4400;
    }
  }
  else {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0x92) ))= 0;
    if ((uVar14 & 0x80000) == 0) {
      if ((uVar14 & 0x400000) == 0) {
        if ((uVar14 & 0x200000) == 0) {
          uVar8 = 0x810000;
          if (((uVar14 & 0x1000000) != 0) || (uVar8 = 0x840000, (uVar14 & 0x4000000) != 0))
          goto LAB_80144748;
          if ((uVar14 & 0x100000) == 0) goto LAB_80144754;
          uVar8 = 0x804400;
        }
        else {
          uVar8 = 0x802400;
        }
      }
      else {
        uVar8 = 0x804400;
      }
    }
    else {
      uVar8 = 0x800c00;
    }
LAB_8014474c:
    uVar12 = uVar12 | uVar8;
  }
LAB_80144754:
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))=
       (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x74)))) * 0x10) >> 4;
  if ((uVar12 & 0x800400) == 0x800400) {
    if (*(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))== '\0') {
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))= 0x40;
    }
    else {
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))=
           (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xc6)))) * 0x10) >> 4;
    }
  }
  iVar11 = (uint)*(ushort *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))<< 0x10;
  iVar5 = iVar11 >> 0x10;
  if (-1 < iVar5) {
    if ((uVar12 & 0x2000000) == 0) {
      iVar5 = (uint)*(ushort *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))+ uVar13 * 0x10;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))= (short)iVar5;
      uVar3 = 0x12c0;
      if (iVar5 * 0x10000 >> 0x10 < 0x12c1) goto LAB_80144804;
    }
    else {
      uVar3 = (undefined2)((iVar5 - (iVar11 >> 0x1f)) >> 1);
    }
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))= uVar3;
  }
LAB_80144804:
  if (((*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) == 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x6c) ))& 0x8000) == 0)) {
    *(ushort *)guest_at((uint32_t)(uintptr_t)((atk + 0xd8) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xd8) ))+ (ushort)(*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))>> 1) * 0x3c;
  }
  if ((*(byte *)guest_at((uint32_t)(uintptr_t)((def + 5) ))& 0x10) == 0) {
    sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+
            (short)((uint)(((int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))/ 3) * 0x10000) >> 0x10) * 0x3c;
  }
  else {
    sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))* -0x78;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))= sVar4;
  uVar15 = 3;
  xform_accum_spawn(3,uVar13);
  uVar9 = 8;
  debug_strike_fx_b(hits,(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))+ -0x400) * 0x100000 >> 0x14,
               (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))<< 4,uVar13 | 0x1000,atk);
LAB_80144d0c:
  if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))!= 0) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xb4) ))= 0x68;
  }
  if (((uVar12 & 0x800) == 0) && ((uVar12 & 0x804000) != 0x804000)) {
    if ((uVar12 & 0x810000) == 0x810000) {
      trig_vel_flat(atk,(byte *)(intptr_t)def);
    }
    else if ((uVar12 & 0x802000) == 0x802000) {
      trig_vel_up(atk,(byte *)(intptr_t)def);
    }
    else if ((uVar12 & 0x840000) == 0x840000) {
      trig_vel_diag(atk,(byte *)(intptr_t)def);
    }
  }
  else {
    trig_lut_read(atk,(byte *)(intptr_t)def);
  }
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x54) ))& 0x20) != 0) {
    iVar5 = (uint)*(ushort *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))<< 0x10;
    iVar11 = (uint)*(ushort *)guest_at((uint32_t)(uintptr_t)((def + 0x98) ))<< 0x10;
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)(((iVar5 >> 0x10) - (iVar5 >> 0x1f)) >> 1);
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x98) ))= (short)(((iVar11 >> 0x10) - (iVar11 >> 0x1f)) >> 1);
  }
  if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))!= 0) {
    if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))< 0) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= 0;
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x6c) ))& 0xf0) != 0) {
      *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xc4) ))=
           (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x70)))) * 0x10) >> 4;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((atk + 0xb4) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xb4)));
      iVar5 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))<< 1;
      *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xb2) ))=
           (short)((ulonglong)((longlong)iVar5 * 0x55555556) >> 0x20) - (short)(iVar5 >> 0x1f);
      iVar5 = (uint)*(ushort *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))<< 0x10;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)(((iVar5 >> 0x10) - (iVar5 >> 0x1f)) >> 1);
      *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xb2) ))=
           (short)mips_div_lo((int32_t)(*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xb2) ))* 500),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x24)))); /* guest div @0x80144EB0 */
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))=
         (short)mips_div_lo((int32_t)(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))* 500),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 0x24)))); /* guest div @0x80144EE0 */
    if (0x6000 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2)))) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= 0x6000;
    }
  }
  if (((uVar14 & 0x20000000) != 0) && ((uVar6 & 0x4000000) != 0)) {
    uVar12 = uVar12 | 0x80000;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= 0;
  }
  *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x68) ))= uVar12;
  *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0xf9ffffff | uVar6;
  *(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))| uVar9;
  return uVar15;

}

uint hitbox_wall_collide(int f0,undefined2 *hits,int battle)
{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  
  iVar16 = 0;
  iVar17 = 0;
  uVar13 = 0;
  uVar14 = 0;
  bVar5 = true;
  bVar3 = true;
  bVar2 = true;
  bVar1 = true;
  iVar9 = *(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 8;
  uVar10 = *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58)));
  if ((uVar10 & 0x100000) == 0) {
    if (((uVar10 & 0x8000) == 0) &&
       (((-1 < (int)uVar10 || ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 100) ))& 0x1000) == 0)) ||
        (*(byte *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x2c) )))< 0x81)))) {
      if (((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x20000000) != 0) ||
         ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x80000) != 0)) {
        bVar5 = false;
        bVar3 = false;
        bVar2 = false;
        bVar1 = false;
      }
    }
    else {
      bVar1 = _DAT_801f04e4 != 0;
      bVar2 = _DAT_801f04e6 != 0;
      bVar3 = _DAT_801f04e8 != 0;
      bVar5 = true;
      if (_DAT_801f04ea == 0) {
        bVar5 = false;
      }
    }
  }
  iVar15 = 1;
  iVar12 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0xc)));
  if (1 < *(char *)guest_at((uint32_t)(uintptr_t)((f0 + 9)))) {
    do {
      iVar6 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ *(char *)guest_at((uint32_t)(uintptr_t)((iVar12 + 5) ))* 0x50;
      iVar7 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x34)));
      iVar11 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 6)));
      if (iVar7 < 0) {
        iVar7 = -(iVar7 - iVar11) - (int)*(short *)(&DAT_801b1590 + iVar9);
        if (-0x40 < iVar7) {
          uVar14 = uVar14 | 2;
        }
        if ((0 < iVar7) && (bVar4 = iVar16 < iVar7, bVar2)) {
          uVar13 = uVar13 | 2;
          goto LAB_8014660c;
        }
      }
      else {
        iVar7 = (int)*(short *)(&DAT_801b1590 + iVar9) - (iVar7 + iVar11);
        if (iVar7 < 0x40) {
          uVar14 = uVar14 | 1;
        }
        if ((iVar7 < 0) && (bVar4 = iVar7 < iVar16, bVar1)) {
          uVar13 = uVar13 | 1;
LAB_8014660c:
          if (bVar4) {
            iVar16 = iVar7;
          }
        }
      }
      iVar6 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x3c)));
      if (iVar6 < 0) {
        iVar6 = -(iVar6 - iVar11) - (int)*(short *)(&DAT_801b1592 + iVar9);
        if (-0x40 < iVar6) {
          uVar14 = uVar14 | 8;
        }
        if ((0 < iVar6) && (bVar4 = iVar17 < iVar6, bVar5)) {
          uVar13 = uVar13 | 8;
          goto LAB_80146684;
        }
      }
      else {
        iVar6 = (int)*(short *)(&DAT_801b1592 + iVar9) - (iVar6 + iVar11);
        if (iVar6 < 0x40) {
          uVar14 = uVar14 | 4;
        }
        if ((iVar6 < 0) && (bVar4 = iVar6 < iVar17, bVar3)) {
          uVar13 = uVar13 | 4;
LAB_80146684:
          if (bVar4) {
            iVar17 = iVar6;
          }
        }
      }
      iVar15 = iVar15 + 1;
      iVar12 = iVar12 + 4;
    } while (iVar15 < *(char *)guest_at((uint32_t)(uintptr_t)((f0 + 9))));
  }
  iVar9 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
  iVar12 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
  uVar10 = *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xfeffffff;
  *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0xffffff00;
  *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= uVar10;
  if (uVar13 == 0) goto LAB_8014672c;
  iVar15 = iVar9;
  if (iVar9 < 0) {
    iVar15 = -iVar9;
  }
  iVar6 = iVar12;
  if (iVar12 < 0) {
    iVar6 = -iVar12;
  }
  if (iVar15 < iVar6) {
    uVar8 = uVar13 & 4;
    if (iVar12 < 1) {
      uVar8 = uVar13 & 8;
      iVar9 = iVar12;
      goto joined_r0x80146714;
    }
  }
  else {
    uVar8 = uVar13 & 1;
    if (iVar9 < 1) {
      uVar8 = uVar13 & 2;
joined_r0x80146714:
      if (-1 < iVar9) goto LAB_8014672c;
    }
  }
  if (uVar8 != 0) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= uVar10 | 0x1000000;
  }
LAB_8014672c:
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x20000000) == 0) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| uVar13 | uVar14 << 4;
  }
  *hits = 0;
  hits[2] = 0;
  if (uVar13 == 0) {
    uVar13 = 0;
  }
  else {
    *hits = (short)iVar16;
    if (iVar16 < 0) {
      iVar16 = -iVar16;
    }
    hits[2] = (short)iVar17;
    if (iVar17 < 0) {
      iVar17 = -iVar17;
    }
    if (0x80 < iVar16) {
      iVar16 = 0x80;
    }
    if (0x80 < iVar17) {
      iVar17 = 0x80;
    }
    if (((uVar13 & 1) != 0) && (_DAT_801f04f4 == 0)) {
      _DAT_801f04ec = (undefined2)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* iVar16) / 0x500);
    }
    if (((uVar13 & 2) != 0) && (_DAT_801f04f6 == 0)) {
      _DAT_801f04ee = (undefined2)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* iVar16) / 0x500);
    }
    if (((uVar13 & 4) != 0) && (_DAT_801f04f8 == 0)) {
      _DAT_801f04f0 = (undefined2)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* iVar17) / 0x500);
    }
    if (((uVar13 & 8) != 0) && (_DAT_801f04fa == 0)) {
      _DAT_801f04f2 = (undefined2)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* iVar17) / 0x500);
    }
  }
  return uVar13;

}

void camera_aim_offset(int f0,int f1,undefined4 ang)
{
  bool bVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  /* Guest vec3 slots: rtps_pt_xform / FUN_80194660 (vec3_s16_normalize)
   * and gte_rtps_xform1 all read/write THREE contiguous s16 through
   * local_70 / local_68. The decompiler split each 6-byte vec3 into
   * scalars + a 2-short array, so index-2 writes overflowed the array
   * (same host-stack-smash class as fighter_pair_update's local_38).
   * local_70[0..2] = old local_70/local_6e/local_6c;
   * local_68[0..2] = old local_68[0..1]/local_64. */
  short local_70 [3];
  short local_68 [3];
  undefined1 auStack_60 [32];
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  
  rotmat_rtps_build(ang,auStack_60);
  local_70[0] = (short)(((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x10) ))+
                      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x60)))) -
                     (*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x10) ))+
                     *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x60))))) / 2);
  local_70[1] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 100) ))- *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 100)))
  ;
  local_70[2] = (short)(((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x18) ))+
                      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x68)))) -
                     (*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x18) ))+
                     *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x68))))) / 2);
  gte_rtps_xform1(auStack_60,local_70,local_70);
  sVar2 = local_70[2];
  if (local_70[2] < 0) {
    sVar2 = -local_70[2];
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x76) ))= sVar2;
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x76) ))= sVar2;
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x78) ))= local_70[1];
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x78) ))= -local_70[1];
  *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xff7fffff;
  *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0xff7fffff;
  iVar3 = (int)local_70[2];
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  if (iVar3 < 0x181) {
    iVar3 = (int)local_70[1];
    if (iVar3 < 0) {
      iVar3 = -iVar3;
    }
    if ((((iVar3 < 0x141) && (local_34 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))), *(int *)guest_at((uint32_t)(uintptr_t)((local_34 + 0x40) ))== 0))
        && (local_30 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))), *(int *)guest_at((uint32_t)(uintptr_t)((local_30 + 0x40) ))== 0)) &&
       (((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x200000) == 0 &&
        (iVar3 = 0, (*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))& 0x200000) == 0)))) {
      bVar1 = false;
      local_38 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0xc)));
      if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x30000) != 0) &&
          ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x8000000) == 0)) ||
         (((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x30000) != 0 &&
          ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x8000000) == 0)))) {
        bVar1 = true;
      }
      bVar1 = !bVar1;
      if (bVar1) {
        local_40 = (int)*(char *)guest_at((uint32_t)(uintptr_t)((f0 + 9)));
        local_3c = (int)*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 9)));
      }
      else {
        local_3c = 1;
        local_40 = 1;
      }
      uVar11 = (uint)bVar1;
      uVar5 = (uint)bVar1;
      if ((int)uVar11 < local_40) {
        iVar10 = uVar11 * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0xc)));
        do {
          iVar9 = local_34 + *(char *)guest_at((uint32_t)(uintptr_t)((iVar10 + 1) ))* 0x50 + 0x34;
          if ((int)uVar5 < local_3c) {
            iVar7 = uVar5 * 4 + local_38;
            uVar8 = uVar5;
            do {
              iVar6 = local_30 + *(char *)guest_at((uint32_t)(uintptr_t)((iVar7 + 1) ))* 0x50 + 0x34;
              iVar4 = pt_box_radius_test((int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar10 + 2))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 2))),(int *)guest_at((uint32_t)(uintptr_t)(iVar9)),
                                   (int *)guest_at((uint32_t)(uintptr_t)(iVar6)));
              if (iVar4 != 0) {
                rtps_pt_xform((ushort *)guest_at((uint32_t)(uintptr_t)(iVar9)),local_70);
                rtps_pt_xform((ushort *)guest_at((uint32_t)(uintptr_t)(iVar6)),local_68);
                iVar4 = ((int)local_68[2] + (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 2)))) -
                        ((int)local_70[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar10 + 2))));
                if (iVar3 < iVar4) {
                  iVar3 = iVar4;
                }
              }
              uVar8 = uVar8 + 1;
              iVar7 = iVar7 + 4;
            } while ((int)uVar8 < local_3c);
          }
          uVar11 = uVar11 + 1;
          iVar10 = iVar10 + 4;
        } while ((int)uVar11 < local_40);
      }
      if (iVar3 != 0) {
        *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))| 0x800000;
        uVar5 = *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c)));
        *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= uVar5 | 0x800000;
        if (200 < iVar3) {
          iVar3 = 200;
        }
        iVar3 = iVar3 * 3;
        if (iVar3 < 0) {
          iVar3 = iVar3 + 3;
        }
        uVar11 = *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x8000;
        if ((uVar11 == 0) && ((uVar5 & 0x8000) == 0)) {
          uVar8 = (uint)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24)));
          iVar10 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x24)));
          uVar11 = uVar8 + iVar10;
        }
        else {
          iVar10 = 0;
          uVar8 = (uint)(uVar11 != 0);
          uVar11 = (uint)(uVar11 != 0);
          if ((uVar5 & 0x8000) != 0) {
            iVar10 = 1;
            uVar11 = uVar11 + 1;
          }
        }
        iVar9 = (iVar3 >> 2) * 0x80;
        local_70[1] = 0;
        local_70[0] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x40) ))+ 0xe) ))-
                   *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0xe)));
        local_70[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x40) ))+ 0x12) ))-
                   *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0x12)));
        FUN_80194660(local_70,local_68);
        iVar10 = mips_div_lo((int32_t)(iVar9 * iVar10),(int32_t)(int)uVar11); /* guest div @0x80146268 */
        iVar3 = local_68[0] * iVar10;
        iVar10 = local_68[2] * iVar10;
        local_70[0] = (short)(iVar3 >> 0x13);
        local_70[2] = (short)(iVar10 >> 0x13);
        node_translate_tree(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))),-(iVar3 >> 0x13),0,-(iVar10 >> 0x13));
        iVar10 = mips_div_lo((int32_t)(iVar9 * uVar8),(int32_t)(int)uVar11); /* guest div @0x801462C8 */
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))- local_70[0];
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))- local_70[2];
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
        iVar3 = local_68[0] * iVar10;
        iVar10 = local_68[2] * iVar10;
        local_68[0] = (short)(iVar3 >> 0x13);
        local_68[2] = (short)(iVar10 >> 0x13);
        node_translate_tree(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))),iVar3 >> 0x13,0,iVar10 >> 0x13);
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))+ local_68[0];
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd8) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd8) ))+ local_68[2];
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38)));
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c) ))=
             *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c)));
      }
    }
  }
  return;

}

void node_translate_tree(int node,int dx,int dy,int dz)
{
  int child;
  int z0;
  undefined4 child2;
  
  if (node != 0) {
    z0 = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x3c)));
    child = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x4c)));
    *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x34) ))= *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x34) ))+ dx;
    *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x38) ))= *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x38) ))- dy;
    if (child != 0) {
      *(int *)guest_at((uint32_t)(uintptr_t)((child + 0x10) ))= *(int *)guest_at((uint32_t)(uintptr_t)((child + 0x10) ))+ dx;
      *(int *)guest_at((uint32_t)(uintptr_t)((child + 0x14) ))= *(int *)guest_at((uint32_t)(uintptr_t)((child + 0x14) ))- dy;
      *(int *)guest_at((uint32_t)(uintptr_t)((child + 0x18) ))= *(int *)guest_at((uint32_t)(uintptr_t)((child + 0x18) ))+ dz;
    }
    child2 = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + 0x44)));
    *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x3c) ))= z0 + dz;
    node_translate_tree(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + 0x48))),dx,dy,dz);
    node_translate_tree(child2,dx,dy,dz);
  }
  return;

}

/* guest byte add: lbu + addu + sb — only the low 8 bits survive. */
static void chan_byte_add(uint32_t addr, uint32_t delta)
{
  *(uint8_t *)guest_at(addr) =
      (uint8_t)(*(uint8_t *)guest_at(addr) + delta);
}

/* guest 0x801ACB08 — fighter channel delta accumulator (pure computation:
 * bounded loops over guest RAM, no calls / waits / IRQ deps -> faithful
 * clean port).
 *
 * node = fighter record (guest addr).  dx/dy = the two delta
 * values the guest adds into the record bytes (dx -> even offset, dy ->
 * odd offset of each pair).  dz (dz) is dead in the guest body — it is
 * overwritten with the inner-loop counter before first use.
 *
 * Guest register map (verified against generated/SLES_017.22_full_53.c):
 *   t4 = *(u32*)(node+8)   count-table base, fixed all function
 *   t0 = *(u32*)(node+16)  ptr-table base, +4 after each chan 4/5/6 pass
 *   t3 = t4 + 4*i             count cell = *(u32*)(t3+0x5c), +4 every i
 *   i  = t2 = 0..7 outer index; only i == 4,5,6 own record arrays.
 * Per channel i in {4,5,6}:
 *   q   = *(u32*)(t0)                 ptr-table entry
 *   n   = *(u32*)(t4 + 0x6c/0x70/0x74) pair count (same cell as t3+0x5c)
 *   arr = *(u32*)(*(u32*)(q+0x10))    record array base
 *   if ((n << 1) != 0): for j = 0; j < (*(u32*)(t3+0x5c) << 1); j++
 *       apply byte-pair deltas to record j (stride below).
 *   t0 += 4; t3 += 4;
 * All other i (and i with count cell == 0): t3 += 4 only — t0 does NOT
 * advance, matching the guest delay-slot / block_801ACD20 split. */
void fighter_chan_delta_accum(uint32_t node, uint32_t dx,
                              uint32_t dy, uint32_t dz)
{
  uint32_t i = 0;
  uint32_t t4 = *(uint32_t *)guest_at(node + 8);
  uint32_t t0 = *(uint32_t *)guest_at(node + 16);
  uint32_t t3 = t4;
  (void)dz;

  for (;;) {
    uint32_t cnt = *(uint32_t *)guest_at(t3 + 0x5c);
    if (cnt != 0) {
      if (i == 5) {
        /* chan 5: count cell t4+0x70, 40-byte records, base +0x25,
         * pairs at record offsets {0xC,0xE,0x1C,0x24}. */
        uint32_t q = *(uint32_t *)guest_at(t0);
        uint32_t n = *(uint32_t *)guest_at(t4 + 0x70);
        uint32_t arr = *(uint32_t *)guest_at(*(uint32_t *)guest_at(q + 0x10));
        uint32_t t1 = t3;
        if ((n << 1) != 0) {
          uint32_t p = arr + 0x25;
          uint32_t j = 0;
          do {
            j++;
            chan_byte_add(p - 25, dx); chan_byte_add(p - 24, dy);
            chan_byte_add(p - 17, dx); chan_byte_add(p - 16, dy);
            chan_byte_add(p - 9,  dx); chan_byte_add(p - 8,  dy);
            chan_byte_add(p - 1,  dx); chan_byte_add(p,      dy);
            p += 0x28;
          } while (j < (*(uint32_t *)guest_at(t1 + 0x5c) << 1));
        }
        t0 += 4;
        t3 += 4;
      }
      else if (i >= 6) {
        if (i == 6) {
          /* chan 6: count cell t4+0x74, 40-byte records, base +0x25,
           * pairs at record offsets {0xC,0x18,0x24}. */
          uint32_t q = *(uint32_t *)guest_at(t0);
          uint32_t n = *(uint32_t *)guest_at(t4 + 0x74);
          uint32_t arr = *(uint32_t *)guest_at(*(uint32_t *)guest_at(q + 0x10));
          uint32_t t1 = t3;
          if ((n << 1) != 0) {
            uint32_t p = arr + 0x25;
            uint32_t j = 0;
            do {
              j++;
              chan_byte_add(p - 25, dx); chan_byte_add(p - 24, dy);
              chan_byte_add(p - 13, dx); chan_byte_add(p - 12, dy);
              chan_byte_add(p - 1,  dx); chan_byte_add(p,      dy);
              p += 0x28;
            } while (j < (*(uint32_t *)guest_at(t1 + 0x5c) << 1));
          }
          t0 += 4;
          t3 += 4;
        }
        else {
          t3 += 4;
        }
      }
      else if (i == 4) {
        /* chan 4: count cell t4+0x6c, 32-byte records, base +0x1d,
         * pairs at record offsets {0xC,0x14,0x1C}. */
        uint32_t q = *(uint32_t *)guest_at(t0);
        uint32_t n = *(uint32_t *)guest_at(t4 + 0x6c);
        uint32_t arr = *(uint32_t *)guest_at(*(uint32_t *)guest_at(q + 0x10));
        uint32_t t1 = t3;
        if ((n << 1) != 0) {
          uint32_t p = arr + 0x1d;
          uint32_t j = 0;
          do {
            j++;
            chan_byte_add(p - 17, dx); chan_byte_add(p - 16, dy);
            chan_byte_add(p - 9,  dx); chan_byte_add(p - 8,  dy);
            chan_byte_add(p - 1,  dx); chan_byte_add(p,      dy);
            p += 0x20;
          } while (j < (*(uint32_t *)guest_at(t1 + 0x5c) << 1));
        }
        t0 += 4;
        t3 += 4;
      }
      else {
        t3 += 4;
      }
    }
    else {
      t3 += 4;
    }
    i++;
    if (i >= 8) break;
  }
  return;

}

void fighter_attach_run(int f)
{
  int ent;
  int idx;
  int ni;
  int aux;
  
  idx = 0x13;
  aux = *(int *)guest_at((uint32_t)(uintptr_t)((f + 0x20) ))+ 0x5f0;
  ent = *(int *)guest_at((uint32_t)(uintptr_t)((f + 0x40) ))+ 0x214;
  if ('\x13' < *(char *)guest_at((uint32_t)(uintptr_t)((f + 8)))) {
    do {
      if (*(ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ent + 0x18) )))== 0xffff) {
        return;
      }
      ni = idx + 1;
      {
        uint32_t fn_idx = *(ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ent + 0x18) )))& 0xf;
        uint32_t fn_g = *(uint32_t *)guest_at(0x801b55c0u + fn_idx * 4);
        void *fn = fn_lookup(fn_g);
        if (fn != (void *)0x0) {
          /* Guest jalr passes f=f, a1=aux, a2=ent, a3=idx —
           * all guest addresses/values; clean callees take guest-u32 args. */
          ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t))fn)
              ((uint32_t)f,(uint32_t)aux,(uint32_t)ent,(uint32_t)idx);
        }
        else {
          /* Unported jalr target: run the generated body with the same
           * guest args. */
          br2_call_clean(fn_g,(uint32_t)f,(uint32_t)aux,(uint32_t)ent,(uint32_t)idx);
        }
      }
      ent = ent + 0x1c;
      idx = ni;
      aux = aux + 0x50;
    } while (ni < *(char *)guest_at((uint32_t)(uintptr_t)((f + 8))));
  }
  return;

}

void fighter_height_track(int f,int st)
{
  short hnew;
  int cur_h;
  uint fl;
  int hnew_i;
  
  hnew = *(short *)guest_at((uint32_t)(uintptr_t)((f + 0x94)));
  hnew_i = (int)hnew;
  fl = *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))& 0xfffffe7f;
  cur_h = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x20) ))+ 0x38)));
  *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= fl;
  if ((cur_h != hnew_i) || (*(short *)guest_at((uint32_t)(uintptr_t)((f + 0x90) ))!= 0 || *(short *)guest_at((uint32_t)(uintptr_t)((f + 0x92) ))!= 0)) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= fl | 0x80;
    if ((hnew_i <= cur_h) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x58) ))& 0x1000000) == 0)) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((f + 0x92) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((f + 0x90) ))= 0;
      *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x6c) ))| 0x80000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))& 0xffffff7f;
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x20) ))+ 0x38) ))= hnew_i;
      *(short *)guest_at((uint32_t)(uintptr_t)((st + 0xd6) ))= hnew;
      return;
    }
    if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((f + 0x90)))) {
      *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))| 0x100;
    }
  }
  return;

}

void hitbox_corners_xform(int f0,int f1)
{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  undefined4 local_30;
  
  iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38)));
  iVar9 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0x60000) == 0) {
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))& 0x60000) == 0) {
      sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xe0)));
      iVar6 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
      sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xdc)));
      iVar7 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd0)));
      sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xde)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x18) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      iVar8 = lut_cos_q12(iVar7);
      iVar5 = lut_sin_q12(iVar7);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))+ (sVar3 >> 4);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))+ (short)((uint)(sVar2 * iVar8 - sVar1 * iVar5) >> 0x10);
      iVar8 = lut_sin_q12(iVar7);
      iVar5 = lut_cos_q12(iVar7);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))+ (short)((uint)(sVar2 * iVar8 + sVar1 * iVar5) >> 0x10);
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x38)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar6 + 0x3c)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 4)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x24) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 8)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x28) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0xc)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x2c) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x10)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x30) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x14)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c)));
      node_xform_apply(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x44))),*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20,
                   0);
      sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xe0)));
      iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
      sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xdc)));
      sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd0)));
      sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xde)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x18) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    }
    else {
      *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))& 0xfffdffff;
      sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xe0)));
      iVar7 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
      sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xdc)));
      iVar10 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd0)));
      sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xde)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      iVar5 = lut_cos_q12(iVar10);
      iVar6 = lut_sin_q12(iVar10);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))+ (sVar3 >> 4);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))+ (short)((uint)(sVar2 * iVar5 - sVar1 * iVar6) >> 0x10);
      iVar5 = lut_sin_q12(iVar10);
      iVar6 = lut_cos_q12(iVar10);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))+ (short)((uint)(sVar2 * iVar5 + sVar1 * iVar6) >> 0x10);
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x38)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x3c)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 4)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x24) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 8)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x28) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0xc)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x2c) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x10)));
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x30) ))=
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x14)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c)));
      node_xform_apply(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x44))),*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20,
                   0);
      sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xe0)));
      sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xdc)));
      sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd0)));
      sVar4 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xde)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8)));
      iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x18) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6)));
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8)));
    }
    local_30 = (int)sVar1;
    iVar6 = (int)sVar3;
    iVar9 = lut_cos_q12(iVar6);
    iVar5 = lut_sin_q12(iVar6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))+ (sVar4 >> 4);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))+ (short)((uint)(sVar2 * iVar9 - local_30 * iVar5) >> 0x10);
    iVar9 = lut_sin_q12(iVar6);
    iVar5 = lut_cos_q12(iVar6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))+ (short)((uint)(sVar2 * iVar9 + local_30 * iVar5) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x34)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x38)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x3c)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x20) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 4)))
    ;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x24) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 8)))
    ;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x28) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0xc)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x2c) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x10)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x30) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x14)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c)));
    iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
  }
  else {
    *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0xfffdffff;
    sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xe0)));
    iVar7 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
    sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xdc)));
    iVar10 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd0)));
    sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xde)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    iVar5 = lut_cos_q12(iVar10);
    iVar6 = lut_sin_q12(iVar10);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))+ (sVar3 >> 4);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))+ (short)((uint)(sVar2 * iVar5 - sVar1 * iVar6) >> 0x10);
    iVar5 = lut_sin_q12(iVar10);
    iVar6 = lut_cos_q12(iVar10);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))+ (short)((uint)(sVar2 * iVar5 + sVar1 * iVar6) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x34)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x38)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x3c)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x20) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 4)))
    ;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x24) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 8)))
    ;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x28) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0xc)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x2c) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x10)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x30) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x14)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c)));
    node_xform_apply(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x44))),*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x20,0)
    ;
    sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xe0)));
    sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xdc)));
    iVar6 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd0)));
    sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xde)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd4)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd6)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar9 + 0xd8)));
    iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x18) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd4)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd6)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0xd8)));
    iVar8 = lut_cos_q12(iVar6);
    iVar9 = lut_sin_q12(iVar6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))+ (sVar3 >> 4);
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))+ (short)((uint)(sVar2 * iVar8 - sVar1 * iVar9) >> 0x10);
    iVar8 = lut_sin_q12(iVar6);
    iVar9 = lut_cos_q12(iVar6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))+ (short)((uint)(sVar2 * iVar8 + sVar1 * iVar9) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x34)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x38)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x3c)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 4)))
    ;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x24) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 8)))
    ;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x28) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0xc)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x2c) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x10)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x30) ))=
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x14)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c)));
    iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
  }
  node_xform_apply(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x44))),iVar8 + 0x20,0);
  return;

}

void pad_edge_scan(int ctrl_g)
{
  ushort cur;
  ushort prev;
  uint old;
  int ent;
  uint *slot;
  uint bit;
  uint i;
  uint edge;
  uint multi;
  uint rep;
  edge = 0;
  rep = 0;
  multi = 0;
  i = 0;
  cur = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c)));
  prev = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a)));
  ent = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x84;
  slot = (uint *)(&DAT_801f0fb0 + ent);
  do {
    bit = 1 << (i & 0x1f);
    if ((prev & bit) == 0) {
      if ((cur & bit) == 0) {
        *slot = 0;
      }
      else {
        old = *slot;
        *slot = old + 1;
        if ((old + 1 < 0x11) && ((*(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82) ))& bit) != 0)) {
          edge = edge | bit;
        }
      }
      slot[0x10] = slot[0x10] + 1;
    }
    else {
      if ((slot[0x10] <= *(uint *)guest_at((uint32_t)(uintptr_t)((ent + -0x7fe0efd0)))) && (slot[0x10] < 0xc)) {
        edge = edge | bit;
      }
      slot[0x10] = 0;
      *slot = 0;
    }
    if (slot[0x10] < 5) {
      rep = rep | bit;
    }
    if (2 < *slot) {
      multi = multi | bit;
    }
    i = i + 1;
    slot = slot + 1;
  } while ((int)i < 0x10);
  if (prev == 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)((ent + -0x7fe0efd0) ))= *(int *)guest_at((uint32_t)(uintptr_t)((ent + -0x7fe0efd0) ))+ 1;
  }
  else {
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((ent + -0x7fe0efd0) ))= 0;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x84) ))= (short)multi;
  *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80) ))= (short)rep;
  *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82) ))= (short)edge;
  return;

}

void cmd_input_parse(int ctrl_g)
{
  bool bVar1;
  short sVar2;
  bool bVar3;
  short sVar4;
  byte bVar5;
  ushort uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  short *psVar11;
  undefined2 *puVar12;
  short *psVar13;
  int iVar14;
  int iVar15;
  ushort in_t5;
  int iVar16;
  int *piVar17;
  uint uVar18;
  short sVar19;
  uVar18 = 0xffffffff;
  sVar19 = 0;
  iVar15 = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x38)));
  iVar14 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x168;
  psVar11 = (short *)(&DAT_801f0ae0 + iVar14);
  piVar17 = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0xc))));
  bVar3 = false;
  if (((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) == 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7e) ))< '\0')) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x3a) ))= 0;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7c) ))= 0;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))= 0;
  }
  uVar7 = *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))& 0xfcffffff;
  *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))= uVar7;
  sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x14)));
  if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7e)))) {
    *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))= uVar7 | 0x1000000;
    sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar15 + *(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7e) ))* 2 + 0x3c)));
  }
  iVar16 = 0;
  psVar13 = (short *)guest_at((uint32_t)(uintptr_t)((iVar14 + -0x7fe0f51e)));
LAB_8014359c:
  if (*piVar17 != 0) {
    iVar14 = *piVar17 + *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x30)));
    do {
      switch(*(undefined2 *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14)))) {
      case 0:
        goto switchD_801435e8_caseD_0;
      case 1:
        if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)))) goto LAB_80143970;
        uVar6 = *(ushort *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14 + 2)));
        bVar1 = (uVar6 & 0x4000) == 0;
        if (bVar1) {
          uVar7 = 1;
        }
        else {
          uVar7 = (uVar6 & 0xfff) + 1;
        }
        uVar9 = (uint)!bVar1;
        if (uVar7 <= uVar9) goto LAB_80143970;
        goto LAB_8014365c;
      case 2:
        if (((-1 < *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)))) || (((ushort)psVar13[1] & 0xfff) != (int)sVar2)) ||
           (((psVar13[1] & 0x1000U) != 0 &&
            ((*(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7c) ))!= *(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))||
             ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) != 0)))))) goto LAB_80143970;
        if ((-1 < psVar13[2]) &&
           ((*(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7e) ))< '\0' && (psVar13[2] < *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xbc)))))))
        goto switchD_801435e8_default;
        *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))| 0x2000000;
        switch(*(undefined2 *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14 + 4)))) {
        case 0:
        case 1:
          in_t5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c)));
          break;
        case 2:
          in_t5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a)));
          break;
        case 3:
          in_t5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          break;
        case 4:
          in_t5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x84)));
          break;
        case 5:
          in_t5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82)));
          break;
        case 6:
          in_t5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c) ))& 0xf000 | *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          break;
        case 7:
          in_t5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          uVar6 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          if ((uVar6 != 0xc0) && (uVar6 != 0x88)) goto switchD_801435e8_default;
        }
        if (((*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd6) ))< 0x1e00)) {
          in_t5 = in_t5 & 0xffdf;
        }
        iVar10 = *psVar11 * 2 + iVar14;
        if ((in_t5 & *(ushort *)guest_at((uint32_t)(uintptr_t)((iVar10 + 6)))) != *(ushort *)guest_at((uint32_t)(uintptr_t)((iVar10 + 8))))
        goto switchD_801435e8_default;
        sVar4 = *psVar13;
        *psVar13 = sVar4 + 1;
        if ((int)(short)(sVar4 + 1) < (int)(uint)*(ushort *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14 + 2))))
        goto switchD_801435e8_default;
        *psVar11 = *psVar11 + 5;
        *psVar13 = 0;
        break;
      case 3:
        uVar6 = *(ushort *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14 + 2)));
        bVar3 = false;
        if ((uVar6 & 0x2000) == 0) goto LAB_801438fc;
        iVar10 = 0;
        if (*(byte *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))== 0) goto LAB_801438fc;
        iVar8 = iVar15;
        goto LAB_801438e0;
      case 4:
        psVar13[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14 + 2) ))<< 4;
        psVar13[3] = *(short *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14 + 4) ))<< 4;
        sVar4 = *psVar11 + 3;
        goto LAB_801439c4;
      default:
        goto switchD_801435e8_default;
      }
    } while( true );
  }
  goto LAB_801439e8;
  while (iVar8 = iVar8 + 2, iVar10 < (int)(uint)*(byte *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d)))) {
LAB_801438e0:
    iVar10 = iVar10 + 1;
    if ((uVar6 & 0xfff) == (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x3c)))) goto LAB_80143970;
  }
LAB_801438fc:
  uVar18 = uVar6 & 0xfff;
  sVar19 = psVar13[3];
  psVar13[1] = (short)uVar18;
  psVar13[2] = -1;
  psVar13[3] = 0;
  *psVar11 = *psVar11 + 2;
  *psVar13 = 0;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((*psVar11 * 2 + iVar14) ))== 0) goto LAB_801439d8;
switchD_801435e8_default:
  piVar17 = piVar17 + 1;
  psVar13 = psVar13 + 5;
  psVar11 = psVar11 + 5;
LAB_801439d8:
  iVar16 = iVar16 + 1;
  if (0x23 < iVar16) {
LAB_801439e8:
    if (bVar3) {
      iVar14 = 0x23;
      puVar12 = (undefined2 *)(&DAT_801f0ae0 + (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x168);
      do {
        *puVar12 = 0;
        iVar14 = iVar14 + -1;
        puVar12 = puVar12 + 5;
      } while (-1 < iVar14);
    }
    if (-1 < (int)(uVar18 << 0x10)) {
      if (((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) == 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7e) ))< '\0')) {
        *(ushort *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x3a) ))= (ushort)*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xda)));
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar15 + (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))* 2 + 0x3c) ))= (short)uVar18;
      *(short *)guest_at((uint32_t)(uintptr_t)((iVar15 + (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))* 2 + 0x5c) ))= sVar19;
      bVar5 = *(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))+ 1;
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7e) ))= *(undefined1 *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d)));
      *(byte *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))= bVar5 - (bVar5 & 0xf0);
    }
    return;
  }
  goto LAB_8014359c;
LAB_80143970:
  *psVar11 = 0;
  goto switchD_801435e8_default;
LAB_8014365c:
  uVar6 = *(ushort *)guest_at((uint32_t)(uintptr_t)(((uVar9 + (int)*psVar11) * 2 + iVar14 + 2)));
  if ((((uVar6 & 0x1000) != 0) &&
      ((*(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7c) ))!= *(char *)guest_at((uint32_t)(uintptr_t)((iVar15 + 0x7d) ))||
       ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) != 0)))) || ((int)sVar2 != (uVar6 & 0xfff)))
  goto LAB_801436a8;
  psVar13[1] = uVar6;
  psVar13[2] = -1;
  psVar13[3] = 0;
  sVar4 = *psVar11 + 1 + (short)uVar7;
LAB_801439c4:
  *psVar11 = sVar4;
  *psVar13 = 0;
  goto switchD_801435e8_default;
LAB_801436a8:
  uVar9 = uVar9 + 1;
  if ((int)uVar7 <= (int)uVar9) goto code_r0x801436b8;
  goto LAB_8014365c;
code_r0x801436b8:
  *psVar11 = 0;
  goto switchD_801435e8_default;
switchD_801435e8_caseD_0:
  *psVar11 = 0;
  bVar3 = true;
  goto switchD_801435e8_default;

}

void action_from_input(int ctrl_g)
{
  short sVar1;
  int iVar2;
  ushort uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  short *psVar7;
  int iVar8;
  uint *puVar9;
  short *psVar10;
  short *psVar11;
  ushort in_t3;
  int iVar12;
  uint *puVar13;
  iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x38)));
  iVar2 = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x100;
  psVar11 = (short *)(&DAT_801f0db0 + iVar2);
  puVar13 = (uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar8 + 8))));
  iVar12 = 0;
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x8000) == 0) {
    puVar9 = puVar13 + 2;
    psVar10 = (short *)guest_at((uint32_t)(uintptr_t)((iVar2 + -0x7fe0f24a)));
    do {
      if (*puVar13 == 0) {
        return;
      }
      if (((*puVar13 & 0x100000) == 0) || (_DAT_801c4578 != 0)) {
LAB_80142fa0:
        iVar4 = *psVar11 + -1;
        iVar2 = iVar4 * 4;
        if (iVar4 < 0) {
          iVar4 = 0;
          iVar2 = 0;
        }
        psVar7 = (short *)guest_at((uint32_t)(uintptr_t)((puVar9[1] + (iVar2 + iVar4) * 2 + *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x30))))));
        switch(psVar7[2]) {
        case 0:
          *psVar11 = 0;
          uVar5 = *puVar13;
          if ((uVar5 & 0x80000) == 0) {
            if ((((uVar5 & 0x40) != 0) && ((*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0)) &&
               (((uVar5 & 0x80) != 0 || (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd6) ))< 0x1e00)))) goto LAB_801434b4;
          }
          else if ((*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0) goto LAB_801434b4;
          uVar5 = *puVar13;
          if ((uVar5 & 0x80) == 0) {
            if ((uVar5 & 0x100) == 0) {
              if ((uVar5 & 0x10000) == 0) {
                if (*(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1c) ))< (short)puVar9[-1]) {
                  *(uint *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x18) ))= uVar5;
                  *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1c) ))= (short)puVar9[-1];
                  *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x1e) ))= *(short *)((intptr_t)puVar9 + -2);
                }
                goto LAB_801434b4;
              }
              uVar6 = *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)));
              if ((uVar6 & 0x1000) == 0) goto LAB_801434b4;
              if ((uVar5 & 0x20000) == 0) {
                if ((uVar5 & 0x40000) != 0) {
                  if ((uVar6 & 0x80002010) == 0x2000) goto LAB_801431cc;
                  goto LAB_801434b4;
                }
                if (((uVar6 & 0x800) == 0) || ((short)puVar9[-1] <= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x34)))))
                goto LAB_801434b4;
                *(uint *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x30) ))= uVar5;
                *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x34) ))= (short)puVar9[-1];
                *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x36) ))= *(short *)((intptr_t)puVar9 + -2);
                uVar3 = *(ushort *)((intptr_t)puVar9 + 2);
                *(ushort *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x38) ))= uVar3;
                uVar5 = *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x54)));
              }
              else {
                if ((uVar6 & 0x2800) != 0) goto LAB_801434b4;
LAB_801431cc:
                if ((short)puVar9[-1] <= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x34)))) goto LAB_801434b4;
                *(uint *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x30) ))= uVar5;
                *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x34) ))= (short)puVar9[-1];
                *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x36) ))= *(short *)((intptr_t)puVar9 + -2);
                uVar3 = *(ushort *)((intptr_t)puVar9 + 2);
                *(ushort *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x38) ))= uVar3;
                uVar5 = *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x54)));
              }
              if ((uVar5 & 8) == 0) {
                if ((uVar5 & 0x10) != 0) {
                  iVar2 = (short)uVar3 * 7;
                  if (iVar2 < 0) {
                    iVar2 = iVar2 + 7;
                  }
                  *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x38) ))= (short)(iVar2 >> 3);
                }
              }
              else {
                *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x38) ))=
                     (short)((((int)((uint)uVar3 << 0x10) >> 0x10) -
                             ((int)((uint)uVar3 << 0x10) >> 0x1f)) >> 1);
              }
              goto LAB_801434b4;
            }
            if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x800) == 0) ||
                ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x20) ))& 0x8000) != 0)) || (iVar12 <= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x2a)))))
            goto LAB_801434b4;
            *(uint *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x20) ))= uVar5;
          }
          else {
            if ((((uVar5 & 0x3000) != 0x3000) &&
                ((((uVar5 & 0x1000) != 0 &&
                  ((*(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x2c) ))+ 0x5c) ))& 0x400000) != 0)) ||
                 (((uVar5 & 0x2000) != 0 &&
                  ((*(uint *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x2c) ))+ 0x5c) ))& 0x400000) == 0)))))) ||
               ((((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x800) != 0 ||
                 ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x20) ))& 0x8000) != 0)) || (iVar12 <= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x2a)))))
               )) goto LAB_801434b4;
            *(uint *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x20) ))= *puVar13;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x24) ))= (short)puVar9[-1];
          *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x26) ))= *(short *)((intptr_t)puVar9 + -2);
          sVar1 = *(short *)((intptr_t)puVar9 + 2);
          *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x2a) ))= (short)iVar12;
          *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 0x28) ))= sVar1;
          goto LAB_801434b4;
        case 1:
          in_t3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c)));
          break;
        case 2:
          in_t3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a)));
          break;
        case 3:
          in_t3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          break;
        case 4:
          in_t3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x84)));
          break;
        case 5:
          in_t3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82)));
          break;
        case 6:
          in_t3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c) ))& 0xf000 | *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          break;
        case 7:
          in_t3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          uVar3 = *(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          if ((uVar3 == 0xc0) || (uVar3 == 0x88)) break;
          goto LAB_80143448;
        }
        if (((*(byte *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd6) ))< 0x1e00)) {
          in_t3 = in_t3 & 0xffdf;
        }
        if ((in_t3 & psVar7[3]) == psVar7[4]) {
          if (*psVar11 == 0) {
            *psVar11 = *psVar11 + 1;
            *psVar10 = 0;
            psVar10[-1] = 0;
            psVar10[-2] = 0;
          }
          sVar1 = psVar10[-2];
          psVar10[-2] = sVar1 + 1;
          if ((short)(sVar1 + 1) < *psVar7) goto LAB_80143484;
          *psVar11 = *psVar11 + 1;
          psVar10[-2] = 0;
          psVar10[-1] = 0;
          goto LAB_80142fa0;
        }
LAB_80143448:
        if (*psVar11 == 0) goto LAB_801434b4;
        sVar1 = psVar10[-1];
        psVar10[-1] = sVar1 + 1;
        if (psVar7[1] <= (short)(sVar1 + 1)) {
          *psVar11 = 0;
        }
LAB_80143484:
        sVar1 = *psVar10;
        *psVar10 = sVar1 + 1;
        if ((short)*puVar9 <= (short)(sVar1 + 1)) {
          *psVar11 = 0;
        }
      }
LAB_801434b4:
      psVar10 = psVar10 + 4;
      psVar11 = psVar11 + 4;
      puVar9 = puVar9 + 4;
      iVar12 = iVar12 + 1;
      puVar13 = puVar13 + 4;
    } while (iVar12 < 0x20);
  }
  return;

}

undefined4 seg_sphere_hit(int rad0,int rad1,int *seg0,int *seg1,int *cen,short *hit)
{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  iVar2 = (rad0 + rad1) * (rad0 + rad1);
  iVar4 = *seg0;
  iVar3 = seg0[1];
  iVar6 = *cen;
  iVar5 = cen[1];
  iVar7 = cen[2] - seg0[2];
  _DAT_1f800398 = iVar6 - iVar4;
  _DAT_1f80039c = iVar5 - iVar3;
  _DAT_1f8003a0 = iVar7;
  gte_op_a00428(&DAT_1f800398,&DAT_1f8003a4);
  if (_DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac < iVar2) {
    *hit = (short)*seg0 + (short)((iVar6 - iVar4) / 2);
    hit[1] = (short)seg0[1] + (short)((iVar5 - iVar3) / 2);
    sVar1 = (short)seg0[2];
    iVar7 = iVar7 - (iVar7 >> 0x1f);
  }
  else {
    if (((*seg1 - *seg0 == 0) && (seg1[1] - seg0[1] == 0)) &&
       (seg1[2] - seg0[2] == 0)) {
      return 0;
    }
    iVar4 = *cen;
    iVar3 = *seg1;
    iVar6 = cen[1];
    iVar5 = seg1[1];
    iVar7 = cen[2] - seg1[2];
    _DAT_1f80038c = *seg1 - *seg0;
    _DAT_1f800390 = seg1[1] - seg0[1];
    _DAT_1f800394 = seg1[2] - seg0[2];
    _DAT_1f8003a4 = iVar4 - iVar3;
    _DAT_1f8003a8 = iVar6 - iVar5;
    _DAT_1f8003ac = iVar7;
    gte_op_a00428(&DAT_1f8003a4,&DAT_1f8003a4);
    if (iVar2 <= _DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac) {
      _DAT_1f8003a4 = _DAT_1f800398 << 0xc;
      _DAT_1f8003b0 = _DAT_1f80038c;
      _DAT_1f8003c0 = 0;
      _DAT_1f8003be = 0;
      _DAT_1f8003bc = 0;
      _DAT_1f8003ba = 0;
      _DAT_1f8003b8 = 0;
      _DAT_1f8003b6 = 0;
      _DAT_1f8003b2 = _DAT_1f800390;
      _DAT_1f8003b4 = _DAT_1f800394;
      _DAT_1f8003a8 = _DAT_1f80039c << 0xc;
      _DAT_1f8003ac = _DAT_1f8003a0 << 0xc;
      gte_mvmva_add(&DAT_1f8003b0,&DAT_1f8003a4,&DAT_1f8003a4);
      iVar3 = _DAT_1f8003a4;
      gte_op_a00428(&DAT_1f80038c,&DAT_1f8003a4);
      iVar4 = _DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac;
      if (iVar4 < iVar3) {
        return 0;
      }
      iVar4 = iVar4 >> 8;
      if (iVar4 == 0) {
        return 0;
      }
      uVar9 = (iVar3 << 4) / iVar4;
      if (uVar9 < 0x1001) {
        _DAT_1f8003b0 = (undefined2)uVar9;
        _DAT_1f8003b4 = 0;
        _DAT_1f8003b2 = 0;
        _DAT_1f8003b8 = _DAT_1f8003b0;
        _DAT_1f8003c0 = _DAT_1f8003b0;
        gte_mvmva_add(&DAT_1f8003b0,&DAT_1f80038c,&DAT_1f8003a4);
        iVar5 = _DAT_1f8003a4 + *seg0;
        iVar3 = *cen - iVar5;
        iVar6 = _DAT_1f8003a8 + seg0[1];
        iVar4 = cen[1] - iVar6;
        iVar8 = _DAT_1f8003ac + seg0[2];
        iVar7 = cen[2] - iVar8;
        _DAT_1f80038c = iVar3;
        _DAT_1f800390 = iVar4;
        _DAT_1f800394 = iVar7;
        gte_op_a00428(&DAT_1f80038c,&DAT_1f8003a4);
        if (_DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac < iVar2) {
          *hit = (short)iVar5 + (short)(iVar3 / 2);
          hit[1] = (short)iVar6 + (short)(iVar4 / 2);
          hit[2] = (short)iVar8 + (short)(iVar7 / 2);
          return 1;
        }
      }
      return 0;
    }
    *hit = (short)*seg1 + (short)((iVar4 - iVar3) / 2);
    iVar7 = iVar7 - (iVar7 >> 0x1f);
    hit[1] = (short)seg1[1] + (short)((iVar6 - iVar5) / 2);
    sVar1 = (short)seg1[2];
  }
  hit[2] = sVar1 + (short)(iVar7 >> 1);
  return 1;

}

undefined4 sphere_pt_hit(int r0,int r1,int *p,int *q,short *mid)
{
  int ad;
  int dy;
  int dz;
  int dx;
  /* Guest vec3 slot: gte_op_a00428 reads/writes 3 contiguous u32
   * through vdif; the decompiler split the 12-byte vec3 into
   * scalars. vdif[0..2] = old vdif/local_2c/local_28. */
  int vdif [3];
  r0 = r0 + r1;
  dx = *p - *q;
  ad = dx;
  if (dx < 0) {
    ad = -dx;
  }
  if (ad < r0) {
    dy = p[1] - q[1];
    ad = dy;
    if (dy < 0) {
      ad = -dy;
    }
    if (ad < r0) {
      dz = p[2] - q[2];
      ad = dz;
      if (dz < 0) {
        ad = -dz;
      }
      if ((ad < r0) &&
         (vdif[0] = dx, vdif[1] = dy, vdif[2] = dz, gte_op_a00428((undefined4 *)vdif,(undefined4 *)vdif),
         vdif[0] + vdif[1] + vdif[2] < r0 * r0)) {
        *mid = (short)*q + (short)(dx / 2);
        mid[1] = (short)q[1] + (short)(dy / 2);
        mid[2] = (short)q[2] + (short)(dz / 2);
        return 1;
      }
    }
  }
  return 0;

}

void debug_strike_fx_a(short *pos,int x,int y,undefined4 unused,int f)
{
  int en;
  /* Guest vec3 slot: quad_fx_spawn reads y[0..2] as 3
   * contiguous u32; the decompiler split the 12-byte vec3 into
   * scalars. vpos[0..2] = old vpos/local_24/local_20. */
  int vpos [3];
  if ((((DAT_1f8000b1 == '\0') && (en = always_one(8), en != 0)) &&
      ((*(ushort *)(_g_pad_buffer + 2) >> 4 & 1) != 0)) && (DAT_1f8000b1 == '\0')) {
    impact_fx_spawn(pos,0x40000001,0,0,0);
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f + 100) ))& 0x20000) != 0) {
      vpos[0] = (int)*pos;
      vpos[1] = (int)pos[1];
      vpos[2] = (int)pos[2];
      quad_fx_spawn(f,0x13,(undefined4 *)vpos,800,0x1000,0,1);
      emit_task_spawn(pos,6,x,y,0x18,1);
      emit_task_spawn(pos,6,x + 0x800,0x800 - y,0xc,1);
    }
  }
  return;

}

void debug_strike_fx_b(short *pos,undefined4 x,undefined4 y,ushort ang,int f)
{
  char mode;
  undefined *pal;
  undefined2 *rec;
  uint fl;
  ushort t;
  ushort dur;
  undefined4 ea;
  undefined4 eb;
  if (DAT_1f8000b1 == '\0') {
    mode = *(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x38) ))+ 0x101)));
    ang = ang & 0xfff;
    if ((((mode == '\x02') || (mode == '\f')) && (*(char *)guest_at((uint32_t)(uintptr_t)((_DAT_1f8000c4 + 2) ))!= '\0')) &&
       ((*(uint *)guest_at((uint32_t)(uintptr_t)((f + 100) ))& 0x20000) == 0)) {
      impact_fx_spawn(pos,0x40000000,ang,0,0);
      if (ang < 0xf) {
        pal = &DAT_801b1bd4;
      }
      else {
        pal = &DAT_801b1be8;
      }
      rec = (undefined2 *)guest_at((uint32_t)(uintptr_t)(fx_record_alloc((undefined4)br2_gaddr(pos),6,0x40000005,0,(undefined4)br2_gaddr(pal))));
      if (rec != (undefined2 *)0x0) {
        rec[1] = 0x400 - (short)y;
        *rec = (short)x;
        t = 10;
        if (9 < ang) {
          t = ang;
        }
        dur = 0x17;
        if (t < 0x18) {
          dur = t;
        }
        rec[2] = dur;
      }
    }
    else {
      fl = *(uint *)guest_at((uint32_t)(uintptr_t)((f + 100)));
      emit_task_spawn(pos,6,x,y,ang,0);
      impact_fx_spawn(pos,0x40000000,ang,0,0);
      ea = _DAT_801f0790;
      eb = _DAT_801f0780;
      if ((0xf < ang) &&
         (emit_task_spawn(pos,8,x,y,ang,0), ea = _DAT_801f0798,
         eb = _DAT_801f0788, ang < 0x17)) {
        ea = _DAT_801f0794;
        eb = _DAT_801f0784;
      }
      emit_task_spawn_b(pos,ea,eb,fl & 0x20000);
    }
  }
  return;

}

void noop_fn(void)
{
  return;

}

undefined2 * rtps_pt_xform(ushort *pt,undefined2 *out)
{
  undefined4 tx;
  undefined4 ty;
  undefined4 tz;
  setCopReg(2,0,*(int *)(pt + 2) << 0x10 | (uint)*pt);
  setCopReg(2,0x800,*(undefined4 *)(pt + 4));
  copFunction(2,0x486012);
  tx = getCopReg(2,0x4800);
  ty = getCopReg(2,0x5000);
  tz = getCopReg(2,0x5800);
  *out = (short)tx;
  out[1] = (short)ty;
  out[2] = (short)tz;
  return out;

}


void node_xform_apply(int node,undefined4 mat,undefined4 flg)
{
  int aux;
  if (node != 0) {
    /* DIAG N+27: flag transform-node links that leave plausible node RAM.
     * Valid scene nodes live in the fighter arenas (~0x801exxxx); a child
     * below 0x80100000 or above 0x80200000 is a stale/garbage link. */
    {
      extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
      uint32_t c48 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x48)));
      uint32_t c44 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x44)));
      if (c48 != 0 && (c48 < 0x80100000u || c48 >= 0x80200000u))
        decomp_trace_record(50, (uint32_t)node, c48, 0x48, 0);
      if (c44 != 0 && (c44 < 0x80100000u || c44 >= 0x80200000u))
        decomp_trace_record(50, (uint32_t)node, c44, 0x44, 0);
    }
    rtps_point_xform((undefined4 *)guest_at((uint32_t)mat),(undefined4 *)guest_at((uint32_t)(node + 0x18)),(int *)guest_at((uint32_t)(node + 0x34)),(int)flg);
    gte_xform3_nold((ushort *)guest_at((uint32_t)(node + 4)),(uint *)guest_at((uint32_t)(node + 0x20)));
    node_xform_apply(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + 0x48))),mat,1);
    aux = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x4c)));
    if (aux != 0) {
      rtps_point_xform2((undefined4 *)guest_at((uint32_t)(node + 0x20)),(undefined4 *)guest_at((uint32_t)(aux + 4)),(int *)guest_at((uint32_t)(aux + 0x10)));
    }
    node_xform_apply(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + 0x44))),node + 0x20,aux);
  }
  return;

}

ushort attr_flag_mask(uint node)
{
  ushort fl;
  int base;
  ushort m;
  fl = *(ushort *)(&DAT_1f80006c + (node & 0xff) * 2);
  base = (node & 0xff) * 0x10;
  m = fl & 0xf000;
  if ((fl & 0x80) != 0) {
    m = *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b880) ))| m;
  }
  if ((fl & 0x40) != 0) {
    m = m | *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b87e)));
  }
  if ((fl & 0x20) != 0) {
    m = m | *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b87c)));
  }
  if ((fl & 0x10) != 0) {
    m = m | *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b87a)));
  }
  if ((fl & 8) != 0) {
    m = m | *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b878)));
  }
  if ((fl & 2) != 0) {
    m = m | *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b876)));
  }
  if ((fl & 4) != 0) {
    m = m | *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b874)));
  }
  if ((fl & 1) != 0) {
    m = m | *(ushort *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b872)));
  }
  return m;

}

undefined2 pad_hist_record(int ctrl_g,int pad,undefined2 flags)
{
  undefined2 *hist;
  /* cell at ctrl_g+0xcc holds a guest u32 pointer to the u16 history
   * array — read the guest word then translate (never a host-ptr-size
   * deref of guest memory). */
  hist = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc))));
  switch(*hist) {
  default:
    return flags;
  case 3:
    if (599 < (short)hist[3]) {
      hist[3] = 0;
    }
    if ((int)(short)hist[3] != (int)(short)hist[2]) {
      return *(undefined2 *)(&DAT_801cf88c + (short)hist[3] * 2 + pad * 0x4b0);
    }
    *hist = 4;
switchD_801289e8_caseD_4:
    return 0;
  case 4:
    goto switchD_801289e8_caseD_4;
  case 5:
    if (599 < (short)hist[3]) {
      hist[3] = 0;
    }
    if ((int)(short)hist[3] != (int)(short)hist[2]) {
      return *(undefined2 *)(&DAT_801cf88c + (short)hist[3] * 2 + pad * 0x4b0);
    }
    *hist = 1;
  case 1:
  case 2:
    if (599 < (short)hist[3]) {
      hist[3] = 0;
    }
    hist[2] = hist[3];
    *(undefined2 *)(&DAT_801cf88c + (short)hist[3] * 2 + pad * 0x4b0) = flags;
    return flags;
  }

}

void fighter_script_fsm(int ctrl_g,undefined4 pad,int *script,int ctx)
{
  byte bVar1;
  byte bVar2;
  short sVar3;
  ushort uVar4;
  ushort uVar5;
  int iVar6;
  ushort *puVar7;
  undefined *puVar8;
  uint uVar9;
  byte bVar10;
  undefined2 uVar11;
  int iVar12;
  uint uVar13;
  iVar12 = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x2c)));
  /* guest stores the guest addr s0+0x7c (inline buf cursor) at +0x78;
   * script is the host mirror of that guest struct. */
  script[0x1e] = (int)(guest_of(script) + 0x7c);
  uVar11 = 0;
  *(undefined1 *)(script + 0x1f) = 0;
  uVar13 = (uint)(*(ushort *)(script + 2) >> 4);
  if (((*(char *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))== '\0') && (*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctx + 200) ))+ 4) ))== 4)) ||
     ((*(char *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))== '\x01' && (*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctx + 200) ))+ 6) ))== 4)))) {
    uVar11 = 1;
  }
  *(undefined2 *)((intptr_t)script + 0x6e) = uVar11;
  script[0x1c] = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)));
  iVar6 = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c)));
  *(undefined2 *)((intptr_t)script + 6) = 0;
  script[0x1d] = iVar6;
  switch((short)script[10]) {
  case 0:
    break;
  case 1:
    goto switchD_80181ff0_caseD_1;
  case 2:
    goto switchD_80181ff0_caseD_2;
  case 3:
    goto switchD_80181ff0_caseD_3;
  case 4:
    goto switchD_80181ff0_caseD_4;
  case 5:
    goto switchD_80181ff0_caseD_5;
  case 6:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd2) ))== 0) {
      *(undefined2 *)((intptr_t)script + 6) = 0x400;
      goto LAB_801824f8;
    }
    sVar3 = *(short *)((intptr_t)script + 0x2a);
    if (sVar3 == 0) {
      bVar10 = *(byte *)(script + 1);
      *(byte *)(script + 1) = bVar10 + 1;
      uVar11 = 0xffff;
      if (0x3f < *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10)))) {
        uVar11 = 1;
      }
      *(undefined2 *)((intptr_t)script + 0x2a) = uVar11;
LAB_80182908:
      bVar10 = *(byte *)(script + 1);
      *(byte *)(script + 1) = bVar10 + 1;
      if ((byte)(&DAT_801b52b8)[uVar13] < *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10))))
      goto switchD_80181ff0_caseD_9;
    }
    else {
      if (0 < sVar3) {
        if (sVar3 != 1) goto switchD_80181ff0_caseD_9;
        goto LAB_80182908;
      }
      if (sVar3 != -1) goto switchD_80181ff0_caseD_9;
    }
    if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd2) ))& 0x10) != 0) {
      *(undefined2 *)((intptr_t)script + 6) = 0x8400;
      goto switchD_80181ff0_caseD_9;
    }
    goto LAB_80182e9c;
  case 7:
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x2000000) != 0) {
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x4000000) != 0) {
        *(undefined2 *)((intptr_t)script + 0x2a) = 0;
      }
      if ((*(short *)((intptr_t)script + 0x2a) == 0) &&
         (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))<= *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x86))))) {
        bVar10 = *(byte *)(script + 1);
        *(byte *)(script + 1) = bVar10 + 1;
        if ((*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))< (byte)(&DAT_801b5288)[uVar13]) &&
           ((-1 < *(int *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58) ))||
            (*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x86) ))* 5 < (int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xba))))))) {
          *(byte *)(script + 1) = bVar10 + 2;
          uVar11 = *(undefined2 *)
                    (&DAT_801b5370 + (*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)(byte)(bVar10 + 1)) ))& 7) * 2);
          *(undefined2 *)(script + 10) = 0;
          *(undefined2 *)((intptr_t)script + 6) = uVar11;
        }
        *(undefined2 *)((intptr_t)script + 0x2a) = 0xffff;
      }
      goto switchD_80181ff0_caseD_9;
    }
    goto LAB_801824f8;
  case 8:
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x60) ))& 0x8000) == 0) {
LAB_80182a6c:
      script_op_dispatch(ctrl_g,pad,(int)guest_of(script),ctx);
    }
  case 9:
    goto switchD_80181ff0_caseD_9;
  default:
    switch((int)(((*(ushort *)(script + 10) & 0x47) - 0x40) * 0x10000) >> 0x10) {
    case 0:
      break;
    case 1:
      if (*(int *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58) ))< 0) {
        *(undefined2 *)((intptr_t)script + 0x2a) = 4;
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x60000000) != 0) {
          *(ushort *)(script + 10) = *(ushort *)(script + 10) | 8;
        }
      }
      else if (*(short *)((intptr_t)script + 0x2a) < 1) {
        *(ushort *)(script + 10) = *(ushort *)(script + 10) & 0xfff7;
      }
      else {
        *(short *)((intptr_t)script + 0x2a) = *(short *)((intptr_t)script + 0x2a) + -1;
      }
      break;
    case 2:
      uVar11 = 0x4000;
      goto LAB_80182d20;
    case 3:
      uVar11 = 0x1000;
LAB_80182d20:
      *(undefined2 *)((intptr_t)script + 6) = uVar11;
      break;
    case 4:
      *(ushort *)(script + 10) = *(ushort *)(script + 10) | 0x18;
      if (((((*(int *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58) ))< 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x76) ))< 0x400)) &&
           (0xb < mips_div_lo((int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xba) )),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x88)))))) && /* guest div @0x80182B00 */
          ((*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xbc) ))< 1 && ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x40) != 0)))) &&
         (bVar10 = *(byte *)(script + 1), *(byte *)(script + 1) = bVar10 + 1,
         (*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))& 3) == 0)) goto LAB_80182b6c;
      if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) != 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))< 0x61))
         && (bVar10 = *(byte *)(script + 1), *(byte *)(script + 1) = bVar10 + 1,
            *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))< 0x15)) goto LAB_80182d94;
      *(ushort *)((intptr_t)script + 10) = *(ushort *)((intptr_t)script + 10) | 1;
      iVar6 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x74)));
      if (iVar6 < 0) {
        iVar6 = -iVar6;
      }
      if ((0x6aa < iVar6) || (0x200 < iVar6)) goto LAB_80182df0;
      if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58)))) goto switchD_80181ff0_caseD_9;
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x2000000) != 0) goto LAB_80182c28;
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) == 0) {
        if (0x3ff < *(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x76)))) goto switchD_80181ff0_caseD_9;
        uVar13 = *(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100)));
        if (((uVar13 & 0x40) == 0) && ((uVar13 & 0x180) != 0x100)) {
          if ((uVar13 & 0x20000) == 0) {
            *(undefined2 *)((intptr_t)script + 6) = 0;
            goto switchD_80181ff0_caseD_9;
          }
          goto LAB_80182ec4;
        }
        goto joined_r0x80182c88;
      }
      goto LAB_80182118;
    default:
      goto switchD_80181ff0_caseD_9;
    }
    if (((((*(ushort *)(script + 10) >> 4 & 1) != 0) &&
         ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) != 0)) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))< 0x61)) &&
       (bVar10 = *(byte *)(script + 1), *(byte *)(script + 1) = bVar10 + 1,
       *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))< 0x15)) goto LAB_80182d94;
    if ((*(ushort *)(script + 10) >> 3 & 1) == 0) {
      uVar5 = *(ushort *)((intptr_t)script + 10) & 0xfffe;
      goto LAB_80182ee0;
    }
    *(ushort *)((intptr_t)script + 10) = *(ushort *)((intptr_t)script + 10) | 1;
    iVar6 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x74)));
    if (iVar6 < 0) {
      iVar6 = -iVar6;
    }
    if ((0x6aa < iVar6) || (0x200 < iVar6)) {
LAB_80182df0:
      *(undefined2 *)((intptr_t)script + 6) = 0xc000;
      goto switchD_80181ff0_caseD_9;
    }
    if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58)))) goto switchD_80181ff0_caseD_9;
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x2000000) == 0) {
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) != 0) goto LAB_80182118;
      if (0x3ff < *(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x76)))) goto switchD_80181ff0_caseD_9;
      if ((((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x40) == 0) && ((*(ushort *)(script + 10) & 0x47) != 0x42))
         && (((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x180) != 0x100 ||
             ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) == 0)))) {
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x20000) == 0) {
          *(undefined2 *)((intptr_t)script + 6) = 0;
        }
        else {
LAB_80182ec4:
          *(undefined2 *)((intptr_t)script + 6) = 0x400;
        }
        goto switchD_80181ff0_caseD_9;
      }
      uVar13 = *(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100)));
joined_r0x80182c88:
      if ((uVar13 & 0x20000) != 0) {
LAB_80182e9c:
        *(undefined2 *)((intptr_t)script + 6) = 0x4400;
        goto switchD_80181ff0_caseD_9;
      }
      goto LAB_80182ea8;
    }
LAB_80182c28:
    if (*(short *)((intptr_t)script + 6) != 0) goto switchD_80181ff0_caseD_9;
    goto LAB_80182118;
  }
  bVar10 = *(byte *)(script + 1);
  puVar7 = (ushort *)guest_at((uint32_t)(uintptr_t)((script[4] + uVar13 * 4)));
  uVar5 = puVar7[1];
  uVar4 = *puVar7;
  *(byte *)(script + 1) = bVar10 + 1;
  iVar6 = (((uint)uVar5 - (uint)uVar4) + 1) * (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10)));
  if (iVar6 < 0) {
    iVar6 = iVar6 + 0xff;
  }
  uVar5 = *(ushort *)guest_at((uint32_t)(uintptr_t)((script[4] + uVar13 * 4)));
  script[0xc] = -1;
  *(undefined2 *)(script + 10) = 1;
  *(ushort *)((intptr_t)script + 10) = *(ushort *)((intptr_t)script + 10) | 1;
  iVar6 = (uint)uVar5 + (iVar6 >> 8);
  *(short *)((intptr_t)script + 0x2a) = (short)iVar6;
  iVar6 = iVar6 * 0x10000;
  *(short *)(script + 0xb) = (short)(((iVar6 >> 0x10) - (iVar6 >> 0x1f)) >> 1);
switchD_80181ff0_caseD_1:
  uVar9 = *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)));
  if ((uVar9 & 0xc0000000) != 0) goto switchD_80181ff0_caseD_9;
  if ((uVar9 & 0x10000000) == 0) {
    if ((uVar9 & 0x8000) != 0) {
      script[0xc] = 0;
      goto LAB_80182544;
    }
    iVar6 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x74)));
    if (iVar6 < 0) {
      iVar6 = -iVar6;
    }
    if (0x6aa < iVar6) {
      script[0xc] = 1;
      goto LAB_80182544;
    }
    if (0x200 < iVar6) {
      script[0xc] = 2;
      goto LAB_80182544;
    }
    sVar3 = *(short *)((intptr_t)script + 0x2a);
    if ((*(short *)((intptr_t)script + 0x2a) == 0) ||
       (*(short *)((intptr_t)script + 0x2a) = sVar3 + -1, sVar3 == 1)) {
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x60) ))& 0x8000) == 0) {
        script[0xc] = 3;
        goto LAB_80182544;
      }
      goto switchD_80181ff0_caseD_9;
    }
    uVar9 = *(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58)));
    if (-1 < (int)uVar9) {
      script[0xc] = 6;
      uVar13 = *(uint *)guest_at((uint32_t)(uintptr_t)((script[6] + 0x48)));
      while (uVar13 != 0) {
        if ((uVar9 & uVar13) == uVar13) {
          if (*(short *)((intptr_t)script + 0x2a) <= (short)script[0xb]) goto LAB_80182544;
          if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
            *(undefined2 *)((intptr_t)script + 6) = 0x4000;
          }
          script[0xc] = -1;
          *(undefined2 *)(script + 10) = 1;
          goto switchD_80181ff0_caseD_9;
        }
        iVar12 = script[0xc];
        script[0xc] = iVar12 + 1;
        uVar13 = *(uint *)guest_at((uint32_t)(uintptr_t)(((iVar12 + 1) * 0xc + script[6])));
      }
      if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
        *(undefined2 *)((intptr_t)script + 6) = 0x4000;
      }
      script[0xc] = -1;
      goto switchD_80181ff0_caseD_9;
    }
    if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xbc)))) {
      script[0xc] = 5;
      goto LAB_80182544;
    }
    bVar10 = *(byte *)(script + 1);
    *(byte *)(script + 1) = bVar10 + 1;
    if (*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))< *(byte *)guest_at((uint32_t)(uintptr_t)((script[7] + uVar13)))) {
      *(byte *)(script + 1) = bVar10 + 2;
      if (*(byte *)guest_at((uint32_t)(uintptr_t)((script[8] + uVar13) ))<= *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)(byte)(bVar10 + 1))))) {
        do {
          script[0xc] = 4;
LAB_80182544:
          sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x76)));
          script[0xd] = 0;
          uVar5 = *(ushort *)guest_at((uint32_t)(uintptr_t)(script[5]));
          while ((int)(uint)uVar5 < (int)sVar3) {
            iVar12 = script[0xd];
            script[0xd] = iVar12 + 1;
            uVar5 = ((ushort *)guest_at((uint32_t)(uintptr_t)(script[5])))[iVar12 + 1];
          }
          script[9] = (uint)*(ushort *)guest_at((uint32_t)(uintptr_t)((script[0xc] * 0xc + script[6] + script[0xd] * 2 + 4) ))+
                       *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x30)));
          if ((*(short *)((intptr_t)script + 0x6e) != 0) &&
             ((script[0x1e] + -0x7c) - (int)guest_of(script) < 0xbb)) {
            iVar12 = sprintf_core((char *)guest_at((uint32_t)script[0x1e]),s__1___02d__1x_0_80123264,script[0xc],script[0xd])
            ;
            script[0x1e] = script[0x1e] + iVar12;
          }
          bVar10 = *(byte *)(script + 1);
          *(byte *)(script + 1) = bVar10 + 1;
          bVar1 = *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10)));
          *(byte *)(script + 1) = bVar10 + 2;
          bVar2 = *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)(byte)(bVar10 + 1))));
          *(byte *)(script + 1) = bVar10 + 3;
          iVar12 = (0x100 - (uint)*(ushort *)(script + 2)) *
                   ((uint)bVar1 + (uint)bVar2 + (uint)*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)(byte)(bVar10 + 2))))
                   );
          if (iVar12 < 0) {
            iVar12 = iVar12 + 0x1fff;
          }
          *(short *)((intptr_t)script + 0x2a) = (short)(iVar12 >> 0xd);
          *(undefined2 *)(script + 0x1b) = 0;
          *(undefined2 *)(script + 10) = 4;
switchD_80181ff0_caseD_4:
          uVar13 = *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x8000;
          if ((uVar13 == 0) || ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x30000) == 0)) {
            uVar11 = 6;
            if (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd2) ))!= 0) {
LAB_80182768:
              *(undefined2 *)(script + 10) = uVar11;
              *(undefined2 *)((intptr_t)script + 0x2a) = 0;
              goto switchD_80181ff0_caseD_9;
            }
            if (((script[0x1c] & 0x8000U) == 0) && (uVar13 != 0)) goto LAB_801824f8;
            if (*(short *)((intptr_t)script + 0x2a) == 0) goto LAB_80182a6c;
            *(short *)((intptr_t)script + 0x2a) = *(short *)((intptr_t)script + 0x2a) + -1;
            goto switchD_80181ff0_caseD_9;
          }
          bVar10 = *(byte *)(script + 1);
          *(undefined2 *)(script + 10) = 5;
          *(byte *)(script + 1) = bVar10 + 1;
          if (*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))< 0x20) {
            uVar5 = *(ushort *)((intptr_t)script + 10) | 4;
          }
          else {
            uVar5 = *(ushort *)((intptr_t)script + 10) & 0xfffb;
          }
          *(ushort *)((intptr_t)script + 10) = uVar5;
switchD_80181ff0_caseD_5:
          if (((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x2000000) != 0) &&
             (uVar11 = 7, (*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x68) ))& 0x800000) != 0)) goto LAB_80182768;
        } while ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x30000) == 0);
        if (((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x40000) != 0) || (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xc0) ))< 0))
        goto switchD_80181ff0_caseD_9;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xc0) ))== 0) {
          bVar10 = *(byte *)(script + 1);
          *(byte *)(script + 1) = bVar10 + 1;
          bVar10 = *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10)));
          if (bVar10 < 0x30) {
LAB_80182d94:
            *(undefined2 *)((intptr_t)script + 6) = 0x2000;
            goto switchD_80181ff0_caseD_9;
          }
          if (0x5f < bVar10) {
            if (bVar10 < 0x90) {
LAB_80182b6c:
              *(undefined2 *)((intptr_t)script + 6) = 0x1000;
            }
            else if (bVar10 < 0xc0) {
              *(undefined2 *)((intptr_t)script + 6) = 0x4000;
            }
            goto switchD_80181ff0_caseD_9;
          }
        }
        else {
          bVar10 = *(byte *)(script + 1);
          *(byte *)(script + 1) = bVar10 + 1;
          bVar10 = *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10)));
          if ((uint)((int)(*(ushort *)(script + 2) + 0x20) >> 1) <= (uint)bVar10)
          goto switchD_80181ff0_caseD_9;
          if ((*(ushort *)((intptr_t)script + 10) & 4) != 0) {
            *(undefined2 *)((intptr_t)script + 6) = 0x40;
            goto switchD_80181ff0_caseD_9;
          }
          if ((bVar10 & 1) == 0) {
LAB_80182ea8:
            *(undefined2 *)((intptr_t)script + 6) = 0x4000;
            goto switchD_80181ff0_caseD_9;
          }
        }
        *(undefined2 *)((intptr_t)script + 6) = 0x8000;
        goto switchD_80181ff0_caseD_9;
      }
      goto LAB_801823b8;
    }
    *(undefined2 *)(script + 10) = 2;
switchD_80181ff0_caseD_2:
    if (((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58) ))& 0x8000) != 0) || (*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xb8) ))== 0)) {
      *(undefined2 *)(script + 10) = 0;
    }
    if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x800) != 0) {
      uVar5 = *(ushort *)((intptr_t)script + 10) | 1;
LAB_80182ee0:
      *(ushort *)((intptr_t)script + 10) = uVar5;
      goto switchD_80181ff0_caseD_9;
    }
    *(ushort *)((intptr_t)script + 10) = *(ushort *)((intptr_t)script + 10) & 0xfffe;
    if ((0x400 < *(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x76)))) || ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x80) == 0))
    goto switchD_80181ff0_caseD_9;
    iVar12 = mips_div_lo((int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xba) )),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x88)))); /* guest div @0x8018235C */
    if (iVar12 == 5) {
      bVar10 = *(byte *)(script + 1);
      *(byte *)(script + 1) = bVar10 + 1;
      if (*(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))< 0x40) {
        *(undefined2 *)((intptr_t)script + 6) = 0x4000;
      }
      goto switchD_80181ff0_caseD_9;
    }
    if (4 < iVar12) goto switchD_80181ff0_caseD_9;
  }
  else if ((((*(int *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x58) ))< 0) &&
            (*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xba) ))<= *(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x88))))) &&
           (-1 < *(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xba))))) &&
          (bVar10 = *(byte *)(script + 1), *(byte *)(script + 1) = bVar10 + 1,
          *(byte *)guest_at((uint32_t)(uintptr_t)((*script + (uint)bVar10) ))< *(byte *)guest_at((uint32_t)(uintptr_t)((uVar13 + script[7]))))) {
LAB_801823b8:
    *(undefined2 *)(script + 10) = 3;
switchD_80181ff0_caseD_3:
    uVar9 = *(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c)));
    if ((uVar9 & 0x2000000) == 0) {
      if ((uVar9 & 0x40000000) != 0) goto LAB_80182458;
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0x76) ))< 0x400) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x40) != 0)) {
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x20000) == 0) {
          *(undefined2 *)((intptr_t)script + 6) = 0x4000;
        }
        else {
          *(undefined2 *)((intptr_t)script + 6) = 0x4400;
        }
      }
      else if ((*(uint *)guest_at((uint32_t)(uintptr_t)((iVar12 + 100) ))& 0x20000) == 0) {
        *(undefined2 *)((intptr_t)script + 6) = 0;
      }
      else {
        *(undefined2 *)((intptr_t)script + 6) = 0x400;
      }
    }
    else {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))< 0x61) {
        if ((uVar9 & 0x80) == 0) {
          bVar10 = *(byte *)(script + 1);
          iVar6 = *script;
          *(byte *)(script + 1) = bVar10 + 1;
          puVar8 = &DAT_801b5298;
        }
        else {
          bVar10 = *(byte *)(script + 1);
          iVar6 = *script;
          *(byte *)(script + 1) = bVar10 + 1;
          puVar8 = &DAT_801b52a8;
        }
        if (*(byte *)guest_at((uint32_t)(uintptr_t)((iVar6 + (uint)bVar10) ))< (byte)puVar8[uVar13]) {
          *(undefined2 *)((intptr_t)script + 6) = 0x2000;
        }
      }
      if (*(short *)((intptr_t)script + 6) == 0) {
LAB_80182458:
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
          *(undefined2 *)((intptr_t)script + 6) = 0x4000;
        }
      }
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((iVar12 + 0xb8) ))== 0) {
LAB_801824f8:
      *(undefined2 *)(script + 10) = 0;
    }
    goto switchD_80181ff0_caseD_9;
  }
LAB_80182118:
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
    *(undefined2 *)((intptr_t)script + 6) = 0x4000;
  }
switchD_80181ff0_caseD_9:
  if ((*(ushort *)((intptr_t)script + 6) & 0x400) != 0) {
    *(ushort *)((intptr_t)script + 6) = *(ushort *)((intptr_t)script + 6) | 8;
  }
  if ((*(ushort *)((intptr_t)script + 10) & 1) != 0) {
    *(ushort *)((intptr_t)script + 6) = *(ushort *)((intptr_t)script + 6) | 0x800;
  }
  return;

}

bool pt_box_radius_test(int r0,int r1,int *p,int *q)
{
  int r2;
  int ad;
  /* Guest vec3 slot: gte_op_a00428 reads/writes 3 contiguous u32
   * through vdif; the decompiler split the 12-byte vec3 into
   * scalars. vdif[0..2] = old vdif/local_14/local_10. */
  int vdif [3];
  r0 = r0 + r1;
  vdif[0] = *q - *p;
  ad = vdif[0];
  if (vdif[0] < 0) {
    ad = -vdif[0];
  }
  if (ad < r0) {
    vdif[1] = q[1] - p[1];
    ad = vdif[1];
    if (vdif[1] < 0) {
      ad = -vdif[1];
    }
    if (ad < r0) {
      vdif[2] = q[2] - p[2];
      ad = vdif[2];
      if (vdif[2] < 0) {
        ad = -vdif[2];
      }
      r2 = r0 * r0;
      if (ad < r0) {
        gte_op_a00428((undefined4 *)vdif,(undefined4 *)vdif);
        if (r2 <= vdif[0]) {
          return false;
        }
        if (vdif[0] + vdif[1] < r2) {
          return vdif[0] + vdif[1] + vdif[2] < r2;
        }
      }
    }
  }
  return false;

}
undefined4 always_one(int unused)
{
  (void)unused;
  return 1;

}

void triple_track_spawn(int idx,undefined2 val,int tid)
{
  undefined2 uv;
  int task;
  undefined2 *src;
  undefined4 *dst;
  if (DAT_1f8000b1 == '\0') {
    if (*(int *)(&DAT_801f0740 + idx * 4) != 0) {
      task_node_free((int *)(uintptr_t)*(int *)(&DAT_801f0740 + idx * 4));
    }
    task = (int)(intptr_t)task_node_alloc(1,2,8,0x8013885c,0,0x10);
    *(int *)(&DAT_801f0740 + idx * 4) = task;
    if (task != 0) {
      dst = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((task + 0x10))));
      /* Table @0x801b1e8c holds 4-byte GUEST pointers to u16-pair data.
       * g_PTR_DAT_801b1e8c is uint8_t** so [&PTR][i] reads 8 bytes at i*8,
       * folding two guest entries into one bogus host ptr (SIGSEGV
       * 0x801B1DE4801B1DD4). Index as u32 and guest_at() the entry. */
      src = (undefined2 *)guest_at(((uint32_t *)&PTR_DAT_801b1e8c)[tid]);
      *(undefined2 *)((intptr_t)dst + 0xe) = val;
      *(short *)(dst + 2) = (short)idx;
      *(undefined2 *)(dst + 3) = 0;
      uv = *src;
      src = src + 1;
      *dst = (undefined4)br2_gaddr(src);
      dst[1] = (undefined4)br2_gaddr(src);
      *(undefined2 *)((intptr_t)dst + 10) = uv;
    }
  }
  return;

}

void xform_accum_spawn(int mode,int lvl)
{
  undefined4 tmp;
  short *rec;
  short v;
  if (DAT_1f8000b1 != '\0') {
    return;
  }
  if (_DAT_801f0748 != 0) {
    task_node_free((int *)(uintptr_t)_DAT_801f0748);
  }
  _DAT_801f0748 = (uint32_t)(uintptr_t)task_node_alloc(1,2,8,0x8013893c,0,0x14);
  if (_DAT_801f0748 == 0) {
    return;
  }
  lvl = lvl << 8;
  rec = (short *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_801f0748 + 0x10))));
  rec[8] = 0xfff;
  rec[9] = 0;
  tmp = _DAT_1f800320;
  *(undefined4 *)rec = _DAT_1f80031c;
  *(undefined4 *)(rec + 2) = tmp;
  tmp = _DAT_1f800320;
  *(undefined4 *)(rec + 4) = _DAT_1f80031c;
  *(undefined4 *)(rec + 6) = tmp;
  if (0xfff < lvl) {
    lvl = 0xfff;
  }
  v = (short)lvl;
  if (mode == 1) {
    rec[4] = v;
    goto LAB_80138c24;
  }
  if (mode < 2) {
    rec[4] = v;
LAB_80138c1c:
    rec[5] = v;
  }
  else if (mode != 2) {
    if (mode == 3) {
      rec[4] = v;
      rec[5] = v;
      goto LAB_80138c24;
    }
    rec[4] = v;
    goto LAB_80138c1c;
  }
  rec[6] = v;
LAB_80138c24:
  rec[4] = rec[4] - *rec;
  rec[5] = rec[5] - rec[1];
  rec[6] = rec[6] - rec[2];
  return;

}

void impact_fx_spawn(undefined4 *pos,uint fid,int ang,int aux,uint fl)
{
  undefined2 uVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined *puVar8;
  undefined4 *puVar9;
  int iVar10;
  short *psVar11;
  undefined4 uVar12;
  undefined2 *puVar13;
  undefined4 *puVar14;
  if (DAT_1f8000b1 != '\0') {
    return;
  }
  iVar4 = always_one(1);
  if (iVar4 == 0) {
    return;
  }
  iVar4 = (int)(intptr_t)task_node_alloc(1,2,10,0x80138cec,0x80139e0c,
                       *(short *)(&DAT_801b1880 + (fid & 0xfffffff) * 2) + 0x68);
  if (iVar4 == 0) {
    return;
  }
  puVar14 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x10))));
  { extern uint32_t g_pu_raw,g_pu_guestof,g_pu_gbuf,g_pu_node,g_pu_count;
    uint32_t _gof = (uint32_t)guest_of(puVar14);
    uint32_t _gbuf = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x10)));
    g_pu_count++;
    g_pu_raw=(uint32_t)(uintptr_t)puVar14; g_pu_guestof=_gof;
    g_pu_gbuf=_gbuf; g_pu_node=(uint32_t)iVar4;
  }
  if (((fid & 0xf0000000) == 0x40000000) || ((fid & 0xf0000000) != 0x80000000)) {
    uVar12 = pos[1];
    puVar14[0x16] = *pos;
    puVar14[0x17] = uVar12;
  }
  else {
    *(undefined2 *)(puVar14 + 0x16) = *(undefined2 *)pos;
    *(undefined2 *)((intptr_t)puVar14 + 0x5a) = *(undefined2 *)(pos + 1);
    *(undefined2 *)(puVar14 + 0x17) = *(undefined2 *)(pos + 2);
  }
  *(short *)(puVar14 + 0x19) = (short)fid;
  puVar14[0x15] = (int)*(short *)(&DAT_801b18a8 + ((int)(fid << 0x10) >> 0xf));
  if (*(short *)(&DAT_801b1880 + *(short *)(puVar14 + 0x19) * 2) == 0) {
    puVar14[0x18] = 0;
  }
  else {
    puVar14[0x18] = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x10) ))+ 0x68;
  }
  iVar10 = 0;
  puVar14[0x14] = 1;
  *puVar14 = 0x9000000;
  *(undefined1 *)((intptr_t)puVar14 + 7) = 0x2c;
  puVar14[10] = 0x9000000;
  *(undefined1 *)((intptr_t)puVar14 + 0x2f) = 0x2c;
  puVar9 = puVar14;
  do {
    *(undefined2 *)((intptr_t)puVar9 + 0xe) =
         *(undefined2 *)(&DAT_801b18d0 + *(short *)(puVar14 + 0x19) * 2);
    uVar1 = *(undefined2 *)(&DAT_801b1904 + *(short *)(puVar14 + 0x19) * 2);
    iVar10 = iVar10 + 1;
    *(undefined1 *)(puVar9 + 1) = 0x7f;
    *(undefined1 *)((intptr_t)puVar9 + 5) = 0x7f;
    *(undefined1 *)((intptr_t)puVar9 + 6) = 0x7f;
    *(byte *)((intptr_t)puVar9 + 7) = *(byte *)((intptr_t)puVar9 + 7) & 0xfe | 2;
    *(undefined2 *)((intptr_t)puVar9 + 0x16) = uVar1;
    iVar5 = _DAT_801c4578;
    puVar9 = puVar9 + 10;
  } while (iVar10 < 2);
  switch(*(undefined2 *)(puVar14 + 0x19)) {
  case 0:
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar14 + 0x14), (undefined4)br2_gaddr(&DAT_801b1b1c), (undefined4)br2_gaddr(&DAT_801b1ad8));
    if (iVar10 != 0) {
      if (ang < 10) {
        puVar14[0x15] = 0xa4;
        return;
      }
      if (ang < 0xf) {
        puVar14[0x15] = 200;
        return;
      }
      puVar14[0x15] = 0x104;
      return;
    }
    break;
  case 1:
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar14 + 0x14), (undefined4)br2_gaddr(&DAT_801b1b78), (undefined4)br2_gaddr(&DAT_801b1b5c));
    if (iVar10 != 0) {
      puVar13 = (undefined2 *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18]));
      uVar1 = rand_lcg();
      iVar4 = 0;
      *puVar13 = uVar1;
      puVar13[1] = 0x29a;
      do {
        *(undefined1 *)(puVar14 + 1) = 0xc0;
        *(undefined1 *)((intptr_t)puVar14 + 5) = 0xc0;
        *(undefined1 *)((intptr_t)puVar14 + 6) = 0xc0;
        iVar4 = iVar4 + 1;
        puVar14 = puVar14 + 10;
      } while (iVar4 < 2);
      return;
    }
    break;
  case 2:
  case 3:
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar14 + 0x14), (undefined4)br2_gaddr(&DAT_801b1a74), (undefined4)br2_gaddr(&DAT_801b1a50));
    if (iVar10 != 0) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18] ))= (uint32_t)guest_of(pos);
      uVar6 = rand_lcg();
      iVar4 = 0;
      puVar14[0x15] = (uVar6 & 0x7f) + 0x60;
      do {
        *(undefined1 *)(puVar14 + 1) = 0xff;
        *(undefined1 *)((intptr_t)puVar14 + 5) = 0xff;
        *(undefined1 *)((intptr_t)puVar14 + 6) = 0xff;
        if (ang != 0) {
          *(ushort *)((intptr_t)puVar14 + 0xe) = ((short)ang + 0x1e0U & 0x1ff) * 0x40 + 0x1a;
        }
        iVar4 = iVar4 + 1;
        puVar14 = puVar14 + 10;
      } while (iVar4 < 2);
      return;
    }
    break;
  case 4:
  case 9:
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar14 + 0x14), (undefined4)br2_gaddr(&DAT_801b1ab8), (undefined4)br2_gaddr(&DAT_801b1a94));
    if (iVar10 != 0) {
      return;
    }
    break;
  case 5:
    puVar9 = puVar14 + 0x14;
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar9), (undefined4)br2_gaddr(&DAT_801b1bb4), (undefined4)br2_gaddr(&DAT_801b1b90));
    if (iVar10 != 0) {
      puVar13 = (undefined2 *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18]));
      puVar14[0x15] = (int)fl >> 0xf;
      uVar6 = rand_lcg();
      iVar10 = (int)((fl & 0xffff) * ((uVar6 & 0x7ff) + 0x400)) >> 0xc;
      uVar6 = rand_lcg();
      iVar5 = ang + -0x100 + (uVar6 & 0x1ff);
      uVar6 = rand_lcg();
      iVar4 = lut_cos_q12(iVar5);
      *puVar13 = (short)(iVar4 * iVar10 >> 0xb);
      iVar4 = lut_sin_q12(aux + -0x100 + (uVar6 & 0x1ff));
      puVar13[1] = (short)(iVar4 * iVar10 >> 0xb);
      iVar4 = lut_sin_q12(iVar5);
      puVar13[2] = (short)(iVar4 * iVar10 >> 0xb);
      do {
        if ((_DAT_801c4578 != 0) && (_DAT_801c4578 != 3)) {
          *(byte *)((intptr_t)puVar14 + 7) = *(byte *)((intptr_t)puVar14 + 7) & 0xfd;
        }
        *(undefined1 *)(puVar14 + 1) = 0xa0;
        *(undefined1 *)((intptr_t)puVar14 + 5) = 0xff;
        *(undefined1 *)((intptr_t)puVar14 + 6) = 100;
        puVar14 = puVar14 + 10;
      } while ((intptr_t)puVar14 < (intptr_t)puVar9);
      return;
    }
    break;
  case 6:
    iVar4 = 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18] ))= 0x20;
    puVar9 = puVar14;
    do {
      *(undefined1 *)(puVar9 + 1) = 0xa0;
      *(undefined1 *)((intptr_t)puVar9 + 5) = 0xff;
      *(undefined1 *)((intptr_t)puVar9 + 6) = 100;
      *(undefined1 *)(puVar9 + 3) = 0xe0;
      *(undefined1 *)((intptr_t)puVar9 + 0xd) = 0xb0;
      *(undefined1 *)(puVar9 + 5) = 0xef;
      *(undefined1 *)((intptr_t)puVar9 + 0x15) = 0xb0;
      *(undefined1 *)(puVar9 + 7) = 0xe0;
      *(undefined1 *)((intptr_t)puVar9 + 0x1d) = 0xbf;
      *(undefined1 *)(puVar9 + 9) = 0xef;
      *(undefined1 *)((intptr_t)puVar9 + 0x25) = 0xbf;
      iVar4 = iVar4 + 1;
      puVar9 = puVar9 + 10;
    } while (iVar4 < 2);
    if (_DAT_801c4578 == 0) {
      return;
    }
    if (_DAT_801c4578 == 3) {
      return;
    }
    puVar14[0x15] = 0x96;
    return;
  case 7:
    psVar11 = (short *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18]));
    psVar11[4] = 0x20;
    uVar12 = puVar14[0x17];
    *(undefined4 *)psVar11 = puVar14[0x16];
    *(undefined4 *)(psVar11 + 2) = uVar12;
    if ((iVar5 != 0) && (iVar5 != 3)) {
      puVar14[0x15] = 0x5a;
    }
    if (ang == 1) {
      sVar3 = psVar11[2] - *(short *)(puVar14 + 0x15);
    }
    else {
      if (ang < 2) {
        if (ang == 0) {
          *psVar11 = *psVar11 - *(short *)(puVar14 + 0x15);
        }
        goto LAB_8013a628;
      }
      if (ang == 2) {
        *psVar11 = *psVar11 + *(short *)(puVar14 + 0x15);
        goto LAB_8013a628;
      }
      if (ang != 3) goto LAB_8013a628;
      sVar3 = psVar11[2] + *(short *)(puVar14 + 0x15);
    }
    psVar11[2] = sVar3;
LAB_8013a628:
    iVar4 = 0;
    psVar11[1] = psVar11[1] + *(short *)(puVar14 + 0x15);
    do {
      *(undefined1 *)(puVar14 + 1) = 0xa0;
      *(undefined1 *)((intptr_t)puVar14 + 5) = 0xff;
      *(undefined1 *)((intptr_t)puVar14 + 6) = 100;
      *(undefined1 *)(puVar14 + 3) = 0xe0;
      *(undefined1 *)((intptr_t)puVar14 + 0xd) = 0xb0;
      *(undefined1 *)(puVar14 + 5) = 0xef;
      *(undefined1 *)((intptr_t)puVar14 + 0x15) = 0xb0;
      *(undefined1 *)(puVar14 + 7) = 0xe0;
      *(undefined1 *)((intptr_t)puVar14 + 0x1d) = 0xbf;
      *(undefined1 *)(puVar14 + 9) = 0xef;
      *(undefined1 *)((intptr_t)puVar14 + 0x25) = 0xbf;
      iVar4 = iVar4 + 1;
      puVar14 = puVar14 + 10;
    } while (iVar4 < 2);
    return;
  case 8:
  case 0x12:
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar14 + 0x14), (undefined4)br2_gaddr(&DAT_801b1c30), (undefined4)br2_gaddr(&DAT_801b1bfc));
    if (iVar10 != 0) {
      iVar5 = task_alloc_wrap_c((undefined4)(uintptr_t)((intptr_t)puVar14 + 0xe),(undefined4)(uintptr_t)((intptr_t)puVar14 + 0x36),(int)guest_of(&DAT_801b1c60),(int)guest_of(&DAT_801b1c74));
      if (iVar5 != 0) {
        if (*(short *)(puVar14 + 0x19) != 0x12) {
          return;
        }
        puVar13 = (undefined2 *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18]));
        iVar4 = lut_cos_q12(aux);
        *puVar13 = (short)(iVar4 >> 7);
        iVar4 = lut_sin_q12(ang);
        puVar13[1] = (short)(iVar4 >> 9);
        iVar4 = lut_sin_q12(aux);
        puVar13[2] = (short)(iVar4 >> 7);
        return;
      }
      task_node_free((int *)(uintptr_t)iVar10);
    }
    break;
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
    /* guest: lw aux, (0x801B19D4 + (idx-10)*4) — u32 table of guest addrs,
     * passed raw to task_alloc_wrap_d. g_PTR_DAT_801b19d4 is uint8_t** so
     * [&PTR][i] would read 8 bytes at i*8; index as u32 instead. */
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar14 + 0x14),(undefined4)((uint32_t *)&PTR_DAT_801b19d4)[*(short *)(puVar14 + 0x19) + -10], (undefined4)br2_gaddr(&DAT_801b192c));
    if (iVar10 != 0) {
      puVar13 = (undefined2 *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18]));
      uVar6 = rand_lcg();
      iVar10 = (uVar6 & 0xc63) + 0x200;
      uVar6 = rand_lcg();
      uVar7 = rand_lcg();
      iVar5 = ang + -0x400 + (uVar7 & 0x7ff);
      uVar7 = rand_lcg();
      iVar4 = lut_cos_q12(iVar5);
      iVar4 = (iVar4 >> 7) * iVar10;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 0xfff;
      }
      *puVar13 = (short)(iVar4 >> 0xc);
      iVar4 = lut_sin_q12(iVar5);
      iVar10 = (iVar4 >> 7) * iVar10;
      if (iVar10 < 0) {
        iVar10 = iVar10 + 0xfff;
      }
      puVar13[2] = (short)(iVar10 >> 0xc);
      if (fl == 0x10) {
        uVar2 = rand_lcg();
        sVar3 = (uVar2 & 0x3f) + 0x20;
      }
      else {
        iVar4 = lut_sin_q12(aux + -0x400 + (uVar7 & 0x7ff));
        iVar4 = (iVar4 >> 7) * ((uVar6 & 0xc63) + 0x200);
        if (iVar4 < 0) {
          iVar4 = iVar4 + 0xfff;
        }
        sVar3 = (short)(iVar4 >> 0xc);
      }
      puVar13[1] = sVar3;
      iVar4 = 0;
      do {
        iVar4 = iVar4 + 1;
        uVar1 = *(undefined2 *)(&DAT_801b18d0 + ((byte)(&DAT_801b18f8)[DAT_1f800098] + 10) * 2);
        *(byte *)((intptr_t)puVar14 + 7) = *(byte *)((intptr_t)puVar14 + 7) & 0xfd;
        *(undefined2 *)((intptr_t)puVar14 + 0xe) = uVar1;
        puVar14 = puVar14 + 10;
      } while (iVar4 < 2);
      return;
    }
    break;
  case 0xe:
  case 0xf:
    if (*(short *)(puVar14 + 0x19) == 0xe) {
      puVar8 = &DAT_801b1a18;
    }
    else {
      puVar8 = &DAT_801b1a3c;
    }
    iVar10 = task_alloc_wrap_d((undefined4)br2_gaddr(puVar14),(undefined4)br2_gaddr(puVar14 + 10),(undefined4)br2_gaddr(puVar14 + 0x14), (undefined4)br2_gaddr(&DAT_801b1a2c),(undefined4)br2_gaddr(puVar8));
    if (iVar10 != 0) {
      if (*(short *)(puVar14 + 0x19) != 0xe) {
        return;
      }
      puVar13 = (undefined2 *)guest_at((uint32_t)(uintptr_t)(puVar14[0x18]));
      iVar4 = lut_cos_q12(ang);
      iVar4 = (iVar4 >> 5) * fl;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 0xfff;
      }
      *puVar13 = (short)(iVar4 >> 0xc);
      iVar4 = lut_sin_q12(aux);
      iVar4 = (iVar4 >> 5) * fl;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 0xfff;
      }
      puVar13[1] = (short)(iVar4 >> 0xc);
      iVar4 = lut_sin_q12(ang);
      iVar4 = (iVar4 >> 5) * fl;
      if (iVar4 < 0) {
        iVar4 = iVar4 + 0xfff;
      }
      puVar13[2] = (short)(iVar4 >> 0xc);
      iVar4 = rand_lcg();
      *(uint *)(puVar13 + 4) = iVar4 >> 8 & 1;
      *(undefined4 *)(puVar13 + 6) = 0;
      return;
    }
    break;
  default:
    goto switchD_8013a1f8_caseD_10;
  }
  task_node_free((int *)(uintptr_t)iVar4);
switchD_8013a1f8_caseD_10:
  return;

}

int fx_record_alloc(undefined4 pos_g,int kind,uint fl,undefined4 aux,undefined4 pal_g)
{
  int res;
  int rec;
  res = (int)DAT_1f8000b1;
  if (res == 0) {
    /* guest passes kind = 0x801B1EA8 (guest addr of the fn-ptr table) to
     * task_alloc_wrap_e — &PTR_FUN_801b1ea8 is a host pointer; convert. */
    rec = task_alloc_wrap_e((int)(uintptr_t)(kind + 0x1c),(int)br2_gaddr(&PTR_FUN_801b1ea8),pal_g);
    res = 0;
    if (rec != 0) {
      *(uint *)guest_at((uint32_t)(uintptr_t)((rec + 0x10) ))= fl & 0xfffffff;
      *(uint *)guest_at((uint32_t)(uintptr_t)((rec + 0x18) ))= fl & 0xf0000000;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x14) ))= aux;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 8) ))= pos_g;
      *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))= rec + 0x1c;
      /* rec is a guest address; fx_record_init dereferences pos_g as a
       * host pointer, so resolve it (a raw cast sign-extends KSEG0 addrs). */
      fx_record_init((undefined4 *)guest_at((uint32_t)rec));
      res = *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc)));
    }
  }
  return res;

}

void emit_task_spawn(int *pos,int kind,undefined2 x,short y,undefined2 p5, undefined2 p6)
{
  int tmp;
  int *cur;
  int *pay;
  if ((DAT_1f8000b1 == '\0') && (tmp = always_one(3), tmp != 0)) {
    tmp = (int)(intptr_t)task_node_alloc(1,2,0xb,0x8013ce68,0,kind * 0x62 + 0x2c);
    if (tmp != 0) {
      pay = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((tmp + 0x10))));
      cur = pay + 0xb;
      *pay = (int)br2_gaddr(cur);
      cur = cur + kind * 10;
      pay[1] = (int)br2_gaddr(cur);
      cur = cur + kind * 10;
      pay[2] = (int)br2_gaddr(cur);
      pay[3] = (int)br2_gaddr(cur + kind * 2);
      pay[4] = (int)br2_gaddr(cur + kind * 2 + kind * 2);
      *(undefined2 *)(pay + 7) = 0;
      *(short *)((intptr_t)pay + 0x1e) = (short)kind;
      *(undefined2 *)(pay + 8) = 0;
      *(short *)(pay + 9) = 0x400 - y;
      *(undefined2 *)((intptr_t)pay + 0x22) = x;
      *(undefined2 *)((intptr_t)pay + 0x26) = p5;
      tmp = pos[1];
      pay[5] = *pos;
      pay[6] = tmp;
      *(undefined2 *)(pay + 10) = p6;
    }
  }
  return;

}

void emit_task_spawn_b(undefined4 *pos,undefined4 ea,undefined4 eb,undefined4 fl)
{
  int node;
  undefined4 *pay;
  undefined4 py;
  node = (int)(intptr_t)task_node_alloc(1,2,0xb,0x8013eb04,0,0x20);
  if (node != 0) {
    pay = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    py = pos[1];
    *pay = *pos;
    pay[1] = py;
    pay[5] = eb;
    pay[6] = ea;
    *(undefined2 *)(pay + 7) = 0;
    *(undefined2 *)((intptr_t)pay + 0x1e) = 0;
    pay[4] = fl;
  }
  return;

}

void quad_fx_spawn(int f,int kind,undefined4 *pos,undefined2 aux,ushort fl, int p6,int p7)
{
  int d0;
  int d1;
  int d2;
  int t0;
  int t;
  undefined2 *tp;
  undefined2 w;
  int *pay;
  if (DAT_1f8000b1 != '\0') {
    return;
  }
  t0 = (int)(intptr_t)task_node_alloc(1,2,7,0x80141610,0,0x78);
  if (t0 == 0) {
    return;
  }
  pay = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
  t = (int)(intptr_t)prim_group_build((undefined4)br2_gaddr(pay + 1),*(undefined4 *)(&DAT_801c43a8 + kind * 4),0,1);
  pay[1] = t;
  if (t != 0) {
    t = tween_chan_alloc(t,0);
    if (t != 0) {
      pay[2] = 0;
      if (kind != 0x17) {
LAB_80141918:
        *pay = f;
        pay[3] = (intptr_t)pos;
        *(ushort *)(pay + 0x1c) = fl;
        *(short *)((intptr_t)pay + 0x72) = (short)p6;
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x14) ))= *pos;
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x18) ))= pos[1];
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x1c) ))= pos[2];
        if (p7 != 0) {
          prim_attr_word_set(pay[1],(p7 + 0x1e0U & 0x1ff) << 6 | 0x1a);
        }
        if (p6 == 1) {
          rotmat2_build(0xfffffc00,pay + 4);
          if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x20) ))+ 0x128)))) {
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x18) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((f + 0x94)));
          }
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x18) ))= 0;
        }
        else if (p6 < 2) {
          if (p6 == 0) {
            angle_pair_rotmat((uint)*(byte *)guest_at((uint32_t)(uintptr_t)((f + 0xdb) ))* -0x10 + 0xc00,
                         -((int)*(short *)guest_at((uint32_t)(uintptr_t)((f + 0x4e) ))+ (int)*(short *)guest_at((uint32_t)(uintptr_t)((f + 0xc6)))),
                         pay + 4);
          }
        }
        else if (p6 == 2) {
          tween_dur_spread(pay[1],3);
        }
        else if (p6 == 3) {
          rotmat2_build(0xfffffc00,pay + 4);
        }
        t0 = DAT_801b1820;
        pay[0xc] = DAT_801b1820;
        t = DAT_801b1824;
        pay[0xd] = DAT_801b1824;
        d0 = DAT_801b1828;
        pay[0xe] = DAT_801b1828;
        d1 = DAT_801b182c;
        pay[0xf] = DAT_801b182c;
        d2 = DAT_801b1830;
        pay[0x14] = t0;
        pay[0x15] = t;
        pay[0x16] = d0;
        pay[0x17] = d1;
        *(undefined2 *)(pay + 0x14) = aux;
        *(undefined2 *)(pay + 0x16) = aux;
        pay[0x10] = d2;
        pay[0x18] = d2;
        *(undefined2 *)(pay + 0x18) = aux;
        if ((fl & 0x1000) == 0) {
          *(undefined2 *)(pay + 0xc) = DAT_801b2340;
          *(undefined2 *)(pay + 0xe) = DAT_801b2342;
          *(undefined2 *)(pay + 0x10) = DAT_801b2344;
          tp = &DAT_801b2348;
          w = DAT_801b2348;
        }
        else {
          *(undefined2 *)(pay + 0xc) = DAT_801b2330;
          *(undefined2 *)(pay + 0xe) = DAT_801b2332;
          *(undefined2 *)(pay + 0x10) = DAT_801b2334;
          tp = &DAT_801b2338;
          w = DAT_801b2338;
        }
        *(undefined2 *)(pay + 0x1d) = w;
        *(undefined2 *)((intptr_t)pay + 0x76) = tp[1];
        return;
      }
      t = task_alloc_wrap_f((undefined4)(uintptr_t)pay[1],(undefined4)br2_gaddr(pay + 2), (undefined4)br2_gaddr(&DAT_801b1cbc), (undefined4)br2_gaddr(&DAT_801b1ccc));
      pay[2] = t;
      if (t != 0) goto LAB_80141918;
    }
    node_tree_free(pay[1]);
  }
  task_node_free((int *)(uintptr_t)t0);
  return;

}

void group_fx_spawn(undefined4 a0,undefined4 a1,undefined4 a2,undefined4 a3, undefined4 arg5,undefined4 arg6,undefined4 arg7)
{
  int grp;
  int rec;
  grp = fx_group_alloc(1,1,0,0xc);
  if (grp != 0) {
    rec = noop_c(grp);
    noop_b(grp,a0, (undefined4)br2_gaddr(&DAT_801f0760));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))= a1;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x10) ))= a2;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x14) ))= a3;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x18) ))= arg5;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x1c) ))= arg6;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x20) ))= arg7;
  }
  return;

}

undefined4 * vec3_step_toward(undefined4 *out,short *pos,short *dst,short a3)
{
  undefined4 len2;
  int len;
  short dc;
  short dy;
  int step;
  /* Guest vec3 slots: FUN_80194630 reads/writes 3 contiguous u32 through
   * delta (old local_40) and ndir (normalized out, old local_30); the
   * decompiler split each 12-byte vec3 into scalars. delta[0..2] = old
   * local_40/local_3c/local_38; ndir[0..2] = old
   * local_30/local_2c/local_28. */
  int delta [3];
  int ndir [3];
  undefined4 step_a;
  undefined4 step_b;
  delta[0] = (int)*dst - (int)*pos;
  delta[1] = (int)dst[1] - (int)pos[1];
  delta[2] = (int)dst[2] - (int)pos[2];
  if (a3 == 0) {
    a3 = 1;
  }
  step = (int)a3;
  len2 = FUN_80194630((undefined4 *)delta,(undefined4 *)ndir);
  len = FUN_80194500(len2);
  if (len < step) {
    *pos = *dst;
    pos[1] = dst[1];
    pos[2] = dst[2];
  }
  else {
    dc = (short)(ndir[0] * step >> 0xc);
    *pos = *pos + dc;
    dy = (short)(ndir[1] * step >> 0xc);
    step_a = CONCAT22(dy,dc);
    pos[1] = pos[1] + dy;
    dc = (short)(ndir[2] * step >> 0xc);
    step_b = CONCAT22((step_b >> 16),dc);
    pos[2] = pos[2] + dc;
  }
  *out = step_a;
  out[1] = step_b;
  return out;

}

undefined4 * vec3_step_toward_spd(undefined4 *out,short *pos,short *dst,int spd)
{
  short rz;
  int t;
  uint sh;
  undefined2 ry;
  uint rx;
  /* Guest vec3 slots: FUN_80194630 reads/writes 3 contiguous u32 through
   * delta (old local_38) and ndir (normalized out, old local_28); the
   * decompiler split each 12-byte vec3 into scalars. delta[0..2] = old
   * local_38/local_34/local_30; ndir[0..2] = old
   * local_28/local_24/local_20. */
  int delta [3];
  int ndir [3] = {0, 0, 0};
  undefined4 res_a;
  undefined4 res_b;
  delta[0] = (int)*dst - (int)*pos;
  delta[1] = (int)dst[1] - (int)pos[1];
  delta[2] = (int)dst[2] - (int)pos[2];
  t = FUN_80194630((undefined4 *)delta,(undefined4 *)ndir);
  if (t == 0) {
    res_b = (uint)(res_b >> 16) << 0x10;
    res_a = 0;
  }
  else {
    t = FUN_80194500((uint)t);
    sh = (spd + 0xc) * 0x10000 >> 0x10;
    rx = ndir[0] * t >> (sh & 0x1f);
    ry = (undefined2)(ndir[1] * t >> (sh & 0x1f));
    res_a = CONCAT22(ry,(short)rx);
    rz = (short)(ndir[2] * t >> (sh & 0x1f));
    res_b = CONCAT22((res_b >> 16),rz);
    if (((rx & 0xffff) == 0) && (ndir[0] != 0)) {
      res_a = CONCAT22(ry,1);
    }
    if (((res_a >> 16) == 0) && (ndir[1] != 0)) {
      res_a = CONCAT22(1,(short)res_a);
    }
    if ((rz == 0) && (ndir[2] != 0)) {
      res_b = CONCAT22((res_b >> 16),1);
    }
    *pos = *pos + (short)res_a;
    pos[1] = pos[1] + (res_a >> 16);
    pos[2] = pos[2] + (short)res_b;
  }
  *out = res_a;
  out[1] = res_b;
  return out;

}

undefined4 * vec3_step_toward_q(undefined4 *out,short *pos,short *dst,short a3)
{
  undefined4 len2;
  int len;
  short dc;
  short dy;
  int step;
  /* Guest vec3 slots: FUN_80194660 (vec3_s16_normalize) reads 3
   * contiguous s16 through pk (old local_28, packed into the two u32
   * halves of pk[0] plus the high half of pk[1]) and writes 3
   * contiguous s16 through ndir (old local_20). The decompiler split
   * the s16 vec3 into scalars. ndir[0..2] = old local_20/local_1e/local_1c. */
  undefined4 pk [2];
  short ndir [3];
  pk[0] = CONCAT22((short)((dst[1] - pos[1]) * 0x10) >> 4,
                      (short)((*dst - *pos) * 0x10) >> 4);
  pk[1] = CONCAT22((pk[1] >> 16),(short)((dst[2] - pos[2]) * 0x10) >> 4);
  if (a3 == 0) {
    a3 = 1;
  }
  step = (int)a3;
  len2 = FUN_80194660((undefined2 *)pk,(undefined2 *)ndir);
  len = FUN_80194500(len2);
  if (step < len) {
    dc = (short)(ndir[0] * step >> 0xc);
    *pos = *pos + dc;
    dy = (short)(ndir[1] * step >> 0xc);
    pk[0] = CONCAT22(dy,dc);
    pos[1] = pos[1] + dy;
    dc = (short)(ndir[2] * step >> 0xc);
    pk[1] = CONCAT22((pk[1] >> 16),dc);
    pos[2] = pos[2] + dc;
  }
  else {
    *pos = *dst;
    pos[1] = dst[1];
    pos[2] = dst[2];
  }
  *out = pk[0];
  out[1] = pk[1];
  return out;

}

undefined4 * vec3_step_toward_q_spd(undefined4 *out,short *pos,short *dst,int spd)
{
  short rz;
  int t;
  uint sh;
  undefined2 ry;
  uint rx;
  /* Guest vec3 slots: FUN_80194660 (vec3_s16_normalize) reads 3
   * contiguous s16 packed through pk[0] (old local_20) plus the high
   * half of pk[1], and writes 3 contiguous s16 through ndir (old
   * local_18). The decompiler split the s16 vec3 into scalars.
   * ndir[0..2] = old local_18/local_16/local_14. */
  undefined4 pk [2];
  short ndir [3];
  pk[0] = CONCAT22((short)((dst[1] - pos[1]) * 0x10) >> 4,
                      (short)((*dst - *pos) * 0x10) >> 4);
  pk[1] = (pk[1] & 0xffff0000) | (uint16_t)((short)((dst[2] - pos[2]) * 0x10) >> 4);
  t = FUN_80194660((undefined2 *)pk,(undefined2 *)ndir);
  if (t == 0) {
    pk[1] = (uint)(pk[1] >> 16) << 0x10;
    pk[0] = 0;
  }
  else {
    t = FUN_80194500((uint)t);
    sh = (spd + 0xc) * 0x10000 >> 0x10;
    rx = ndir[0] * t >> (sh & 0x1f);
    ry = (undefined2)(ndir[1] * t >> (sh & 0x1f));
    pk[0] = CONCAT22(ry,(short)rx);
    rz = (short)(ndir[2] * t >> (sh & 0x1f));
    pk[1] = CONCAT22((pk[1] >> 16),rz);
    if (((rx & 0xffff) == 0) && (ndir[0] != 0)) {
      pk[0] = CONCAT22(ry,1);
    }
    if (((pk[0] >> 16) == 0) && (ndir[1] != 0)) {
      pk[0] = CONCAT22(1,(short)pk[0]);
    }
    if ((rz == 0) && (ndir[2] != 0)) {
      pk[1] = CONCAT22((pk[1] >> 16),1);
    }
    *pos = *pos + (short)pk[0];
    pos[1] = pos[1] + (pk[0] >> 16);
    pos[2] = pos[2] + (short)pk[1];
  }
  *out = pk[0];
  out[1] = pk[1];
  return out;

}

undefined1 table_byte_next_a(void)
{
  byte idx;
  idx = DAT_801e83aa;
  DAT_801e83aa = DAT_801e83aa + 1;
  return (&DAT_8011ec40)[idx];

}

undefined1 table_byte_next_b(void)
{
  byte idx;
  idx = DAT_801e83b0;
  DAT_801e83b0 = DAT_801e83b0 + 1;
  return *(undefined1 *)guest_at((uint32_t)(uintptr_t)((idx + 0x801e8400)));

}
void sprite_strip_draw_dual(int src_a,int src_b,int uv,int n,ushort *pal,int arg6, int rgb,int tag)
{
  ushort *pa;
  ushort *pb;
  ushort pv;
  int xfp;
  int yfp;
  ushort *pq;
  int i;
  int cur_a;
  int cur_b;
  if (tag == 1) {
    i = 0;
    if (0 < n) {
      arg6 = arg6 * uv;
      pq = pal + 3;
      rgb = rgb * uv;
      do {
        xfp = arg6;
        if (arg6 < 0) {
          xfp = arg6 + 0xfff;
        }
        yfp = rgb;
        if (rgb < 0) {
          yfp = rgb + 0xfff;
        }
        sprite_place_centered(src_a,0x1f800000,(int)(((uint)*pal + (xfp >> 0xc)) * 0x10000) >> 0x10,
                     (int)(((uint)pq[-2] + (yfp >> 0xc)) * 0x10000) >> 0x10,
                     (int)(short)pq[-1],(int)(short)*pq);
        i = i + 1;
        pa = pq + -2;
        pb = pq + -1;
        pv = *pq;
        pq = pq + 4;
        sprite_place_centered(src_b,0x1f800000,(int)(((uint)*pal + (xfp >> 0xc)) * 0x10000) >> 0x10,
                     (int)(((uint)*pa + (yfp >> 0xc)) * 0x10000) >> 0x10,(int)(short)*pb,
                     (int)(short)pv);
        pal = pal + 4;
        src_a = src_a + 0x28;
        src_b = src_b + 0x28;
      } while (i < n);
    }
  }
  else if (tag < 2) {
    if ((tag == 0) && (i = 0, 0 < n)) {
      arg6 = arg6 * uv;
      pq = pal + 3;
      rgb = rgb * uv;
      do {
        xfp = arg6;
        if (arg6 < 0) {
          xfp = arg6 + 0xfff;
        }
        yfp = rgb;
        if (rgb < 0) {
          yfp = rgb + 0xfff;
        }
        sprite_rect_set_c(src_a,(int)(short)*pal + (xfp >> 0xc),
                     (int)(short)pq[-2] + (yfp >> 0xc),(int)(short)pq[-1],
                     (int)(short)*pq);
        pa = pq + -2;
        pb = pq + -1;
        pv = *pq;
        pq = pq + 4;
        i = i + 1;
        sprite_rect_set_c(src_b,(int)(short)*pal + (xfp >> 0xc),
                     (int)(short)*pa + (yfp >> 0xc),(int)(short)*pb,(int)(short)pv);
        pal = pal + 4;
        src_a = src_a + 0x28;
        src_b = src_b + 0x28;
      } while (i < n);
    }
  }
  else if (tag == 2) {
    i = 0;
    if (0 < n) {
      arg6 = arg6 * uv;
      pq = pal + 3;
      rgb = rgb * uv;
      cur_a = src_a;
      do {
        xfp = arg6;
        if (arg6 < 0) {
          xfp = arg6 + 0xfff;
        }
        yfp = rgb;
        if (rgb < 0) {
          yfp = rgb + 0xfff;
        }
        sprite_rect_set_a(cur_a,(int)(short)*pal + (xfp >> 0xc),
                     (int)(short)pq[-2] + (yfp >> 0xc),(int)(short)pq[-1],
                     (int)(short)*pq);
        pa = pq + -2;
        pb = pq + -1;
        pv = *pq;
        pq = pq + 4;
        i = i + 1;
        cur_a = cur_a + 0x18;
        sprite_rect_set_a(src_b,(int)(short)*pal + (xfp >> 0xc),
                     (int)(short)*pa + (yfp >> 0xc),(int)(short)*pb,(int)(short)pv);
        pal = pal + 4;
        src_b = src_b + 0x18;
      } while (i < n);
    }
  }
  else if ((tag == 3) && (i = 0, 0 < n)) {
    arg6 = arg6 * uv;
    pq = pal + 3;
    rgb = rgb * uv;
    cur_b = src_a;
    do {
      xfp = arg6;
      if (arg6 < 0) {
        xfp = arg6 + 0xfff;
      }
      yfp = rgb;
      if (rgb < 0) {
        yfp = rgb + 0xfff;
      }
      sprite_rect_set_b(cur_b,(int)(short)*pal + (xfp >> 0xc),
                   (int)(short)pq[-2] + (yfp >> 0xc),(int)(short)pq[-1],
                   (int)(short)*pq);
      pa = pq + -2;
      pb = pq + -1;
      pv = *pq;
      pq = pq + 4;
      i = i + 1;
      cur_b = cur_b + 0x24;
      sprite_rect_set_b(src_b,(int)(short)*pal + (xfp >> 0xc),
                   (int)(short)*pa + (yfp >> 0xc),(int)(short)*pb,(int)(short)pv);
      pal = pal + 4;
      src_b = src_b + 0x24;
    } while (i < n);
  }
  return;

}

int task_alloc_wrap_a(undefined4 a0,undefined4 a1,int a2,undefined4 a3)
{
  int node;
  undefined4 *pay;
  node = (int)(intptr_t)task_node_alloc(0,2,0,0x80137680,0,0x1c);
  if (node != 0) {
    pay = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = a0;
    pay[1] = a1;
    pay[2] = a2;
    pay[3] = a3;
    pay[4] = a2 + -4;
    pay[5] = 0;
    pay[6] = 0;
  }
  return node;

}

int task_alloc_wrap_b(undefined4 a0,undefined4 a1,undefined4 a2,undefined4 a3, int arg5)
{
  int node;
  undefined4 *pay;
  { extern uint32_t g_wb_last_p1,g_wb_last_p2,g_wb_last_p3,g_wb_last_p4,g_wb_last_p5;
    extern uint32_t g_wb_bad_count,g_wb_bad_node,g_wrap_src;
    g_wrap_src=1;
    g_wb_last_p1=(uint32_t)a0; g_wb_last_p2=(uint32_t)a1;
    g_wb_last_p3=(uint32_t)a2; g_wb_last_p4=(uint32_t)a3;
    g_wb_last_p5=(uint32_t)arg5;
    if (((uint32_t)a0 < 0x80100000u || (uint32_t)a1 < 0x80100000u) &&
        (uint32_t)a0 != 0u && (uint32_t)a1 != 0u) {
      g_wb_bad_count++; g_wb_bad_node=(uint32_t)a3;
    }
  }
  node = (int)(intptr_t)task_node_alloc(0,3,0,0x80137824,0,0x1c);
  if (node != 0) {
    pay = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = a0;
    pay[1] = a1;
    pay[2] = a2;
    pay[3] = a3;
    pay[4] = arg5;
    pay[5] = arg5 + -4;
    *(undefined2 *)(pay + 6) = 0;
    *(undefined2 *)((intptr_t)pay + 0x1a) = 0;
  }
  return node;

}

void hud_cell_frame_set(int val,int cell)
{
  ushort fr;
  if (val < 10) {
    *(ushort *)guest_at((uint32_t)(uintptr_t)((cell + 0xe) ))=
         (short)((DAT_801b4b54 & 0x3ff) >> 4) +
         (DAT_801b4b56 + (ushort)(byte)(&DAT_801b4e28)[_DAT_801f4f20 % 3] & 0x1ff) * 0x40;
  }
  else {
    if (val < 0x14) {
      fr = DAT_801b4b56 + 1;
    }
    else {
      fr = DAT_801b4b56 + 3;
    }
    *(ushort *)guest_at((uint32_t)(uintptr_t)((cell + 0xe) ))= (short)((DAT_801b4b54 & 0x3ff) >> 4) + (fr & 0x1ff) * 0x40;
  }
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cell + 0x36) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cell + 0xe)));
  return;

}

void hud_digits_color(int val,int cell)
{
  char c0;
  int tens_off;
  int ones_off;
  char *dg;
  if (val < 0) {
    dg = &DAT_801b4b94;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0xc) ))= DAT_801b4b94;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0xd) ))= DAT_801b4b95;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x14) ))= DAT_801b4b94 + DAT_801b4b96;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x15) ))= DAT_801b4b95;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x1c) ))= DAT_801b4b94;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x1d) ))= DAT_801b4b95 + DAT_801b4b97;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x24) ))= DAT_801b4b94 + DAT_801b4b96;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x25) ))= DAT_801b4b95 + DAT_801b4b97;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x34) ))= DAT_801b4b94;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x35) ))= DAT_801b4b95;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x3c) ))= DAT_801b4b94 + DAT_801b4b96;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x3d) ))= DAT_801b4b95;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x44) ))= DAT_801b4b94;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x45) ))= DAT_801b4b95 + DAT_801b4b97;
    c0 = DAT_801b4b94;
  }
  else {
    if (99 < val) {
      val = 0;
    }
    tens_off = (val / 10 & 0xffU) * 4;
    dg = &DAT_801b4b6c + tens_off;
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0xc) ))= *dg;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((cell + 0xd) ))= (&DAT_801b4b6d)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x14) ))= *dg + (&DAT_801b4b6e)[tens_off];
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((cell + 0x15) ))= (&DAT_801b4b6d)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x1c) ))= *dg;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((cell + 0x1d) ))= (&DAT_801b4b6d)[tens_off] + (&DAT_801b4b6f)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x24) ))= *dg + (&DAT_801b4b6e)[tens_off];
    ones_off = (val % 10 & 0xffU) * 4;
    dg = &DAT_801b4b6c + ones_off;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((cell + 0x25) ))= (&DAT_801b4b6d)[tens_off] + (&DAT_801b4b6f)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x34) ))= *dg;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((cell + 0x35) ))= (&DAT_801b4b6d)[ones_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x3c) ))= *dg + (&DAT_801b4b6e)[ones_off];
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((cell + 0x3d) ))= (&DAT_801b4b6d)[ones_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x44) ))= *dg;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((cell + 0x45) ))= (&DAT_801b4b6d)[ones_off] + (&DAT_801b4b6f)[ones_off];
    c0 = *dg;
  }
  *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x4c) ))= c0 + dg[2];
  *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x4d) ))= dg[1] + dg[3];
  return;

}

bool fighter_height_check(int f)
{
  if ((*(byte *)guest_at((uint32_t)(uintptr_t)((f + 5) ))& 0x10) == 0) {
    return 0x1dff < *(short *)guest_at((uint32_t)(uintptr_t)((f + 0xd6)));
  }
  return (bool)2;

}

undefined4 sprite_rect_set_a(int rec,short x,int y,undefined4 w,int h)
{
  short y1;
  short y2;
  short x2;
  x2 = x + (short)w;
  if (DAT_1f800015 == '\0') {
    y = y / 2;
    h = h / 2;
  }
  y1 = (short)y;
  y2 = y1 + (short)h;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x12) ))= y2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x16) ))= y2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 8) ))= x;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 10) ))= y1;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))= x2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ))= y1;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x10) ))= x;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x14) ))= x2;
  return w;

}

undefined4 sprite_rect_set_b(int rec,short x,int y,undefined4 w,int h)
{
  short y1;
  short y2;
  short x2;
  x2 = x + (short)w;
  if (DAT_1f800015 == '\0') {
    y = y / 2;
    h = h / 2;
  }
  y1 = (short)y;
  y2 = y1 + (short)h;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x1a) ))= y2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x22) ))= y2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 8) ))= x;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 10) ))= y1;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x10) ))= x2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x12) ))= y1;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x18) ))= x;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x20) ))= x2;
  return w;

}

undefined4 sprite_rect_set_c(int rec,short x,int y,undefined4 w,int h)
{
  short y1;
  short y2;
  short x2;
  x2 = x + (short)w;
  if (DAT_1f800015 == '\0') {
    y = y / 2;
    h = h / 2;
  }
  y1 = (short)y;
  y2 = y1 + (short)h;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x1a) ))= y2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x22) ))= y2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 8) ))= x;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 10) ))= y1;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x10) ))= x2;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x12) ))= y1;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x18) ))= x;
  *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x20) ))= x2;
  return w;

}

int * rtps_point_xform(undefined4 *rt,undefined4 *in,int *out,int flag)
{
  undefined4 in_zero;
  undefined4 in_at;
  int ty;
  int tz;
  int gx;
  int gy;
  int gz;
  if (flag == 0) {
    setCopControlWord(2,0,*rt);
    setCopControlWord(2,0x800,rt[1]);
    setCopControlWord(2,0x1000,rt[2]);
    setCopControlWord(2,0x1800,rt[3]);
    setCopControlWord(2,0x2000,rt[4]);
  }
  setCopReg(2,in_zero,*in);
  setCopReg(2,in_at,in[1]);
  copFunction(2,0x486012);
  ty = rt[6];
  tz = rt[7];
  gx = getCopReg(2,0x4800);
  gy = getCopReg(2,0x5000);
  gz = getCopReg(2,0x5800);
  *out = rt[5] + gx;
  out[1] = ty + gy;
  out[2] = tz + gz;
  return out;

}

int * rtps_point_xform2(undefined4 *rt,undefined4 *in,int *out)
{
  undefined4 in_zero;
  undefined4 in_at;
  int ty;
  int tz;
  int gx;
  int gy;
  int gz;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,in_zero,*in);
  setCopReg(2,in_at,in[1]);
  copFunction(2,0x486012);
  ty = rt[6];
  tz = rt[7];
  gx = getCopReg(2,0x4800);
  gy = getCopReg(2,0x5000);
  gz = getCopReg(2,0x5800);
  *out = rt[5] + gx;
  out[1] = ty + gy;
  out[2] = tz + gz;
  return out;

}

uint * rotmat_rtps_build(uint ang,uint *m)
{
  int ia;
  int ic;
  uint tv0;
  int ib;
  uint tv1;
  m[2] = 0x1000;
  ia = (ang & 0x3ff) * 2;
  ib = (ang & 0x3ff) * -2 + 0x800;
  ic = ia;
  if ((ang & 0x400) != 0) {
    ic = ib;
    ib = ia;
  }
  tv0 = (uint)*(ushort *)(&DAT_801247a8 + ic);
  if ((ang & 0x800) != 0) {
    tv0 = -tv0 & 0xffff;
  }
  tv1 = (uint)*(ushort *)(&DAT_801247a8 + ib);
  if ((ang + 0x400 & 0x800) != 0) {
    tv1 = -tv1 & 0xffff;
  }
  *m = tv1;
  m[1] = tv0;
  m[3] = -tv0 & 0xffff;
  *(short *)(m + 4) = (short)tv1;
  return m;

}

uint * rotmat2_pair_build(uint a,uint b,uint *out)
{
  int i0;
  int i1;
  int i2;
  int i3;
  uint v0;
  uint v1;
  uint v2;
  int i4;
  i0 = (a & 0x3ff) * 2;
  i4 = (a & 0x3ff) * -2 + 0x800;
  i3 = i0;
  if ((a & 0x400) != 0) {
    i3 = i4;
    i4 = i0;
  }
  i3 = (int)*(short *)(&DAT_801247a8 + i3);
  if ((a & 0x800) != 0) {
    i3 = -i3;
  }
  i1 = (b & 0x3ff) * 2;
  i2 = (b & 0x3ff) * -2 + 0x800;
  i0 = i1;
  if ((b & 0x400) != 0) {
    i0 = i2;
    i2 = i1;
  }
  v1 = (uint)*(short *)(&DAT_801247a8 + i0);
  if ((b & 0x800) != 0) {
    v1 = -v1;
  }
  v2 = (uint)*(short *)(&DAT_801247a8 + i2);
  if ((b + 0x400 & 0x800) != 0) {
    v2 = -v2;
  }
  v0 = (uint)*(short *)(&DAT_801247a8 + i4);
  if ((a + 0x400 & 0x800) != 0) {
    v0 = -v0;
  }
  out[1] = v1 & 0xffff | ((int)(v1 * i3) >> 0xc) << 0x10;
  out[2] = v0 & 0xffff | ((int)(v2 * -i3) >> 0xc) << 0x10;
  *out = v2 & 0xffff;
  out[3] = i3 << 0x10 | -((int)(v1 * v0) >> 0xc) & 0xffffU;
  *(short *)(out + 4) = (short)((int)(v2 * v0) >> 0xc);
  return out;

}

int sprintf_core(char *dst,char *fmt,...)
{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  int *piVar7;
  int iVar8;
  char *local_res4;
  va_list ap;
  /* Guest: one 0x200-byte digit scratch at sp+0x10..0x20f; the byte
   * cursor starts at sp+0x210 (one past the top, adjacent to the
   * local_38 flags word) and walks down. %c stores its byte at the
   * scratch top-1 (sp+0x20f == old local_39). Ghidra split the scratch
   * into &local_38/&local_39 scalars, so digits written pbVar6-1 ran
   * off into unrelated host stack — keep it one array. */
  byte sc_buf[0x200];
#define local_39 sc_buf[0x1ff]
  undefined4 local_38;
  uint local_34;
  uint local_30;
  va_start(ap,fmt);
  cVar1 = *fmt;
  iVar8 = 0;
  local_res4 = fmt;
joined_r0x8018efa8:
  iVar4 = (int)cVar1;
  if (iVar4 != 0) {
    if (iVar4 == 0x25) {
      local_38 = DAT_801b67a0;
      local_34 = DAT_801b67a4;
      local_30 = DAT_801b67a8;
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                pcVar2 = local_res4 + 1;
                iVar4 = (int)local_res4[1];
                if (iVar4 != 0x2d) break;
                local_38 = local_38 | 1;
                local_res4 = pcVar2;
              }
              if (iVar4 != 0x2b) break;
              local_38 = local_38 | 2;
              local_res4 = pcVar2;
            }
            if (iVar4 != 0x20) break;
            local_38 = (local_38 & 0xffff0000) | CONCAT11(local_res4[1],(byte)local_38);
            local_res4 = pcVar2;
          }
          if (iVar4 != 0x23) break;
          local_38 = local_38 | 4;
          local_res4 = pcVar2;
        }
        if (iVar4 != 0x30) break;
        local_38 = local_38 | 8;
        local_res4 = pcVar2;
      }
      if (iVar4 == 0x2a) {
        local_34 = va_arg(ap,uint32_t);

        if ((int)local_34 < 0) {
          local_34 = -local_34;
          local_38 = local_38 | 1;
        }
        iVar4 = (int)local_res4[2];
        local_res4 = local_res4 + 2;
      }
      else {
        while (local_res4 = pcVar2, iVar4 - 0x30U < 10) {
          local_34 = local_34 * 10 + -0x30 + iVar4;
          pcVar2 = local_res4 + 1;
          iVar4 = (int)local_res4[1];
        }
      }
      if (iVar4 == 0x2e) {
        iVar4 = (int)local_res4[1];
        pcVar2 = local_res4;
        if (iVar4 == 0x2a) {
          local_30 = va_arg(ap,uint32_t);

          iVar4 = (int)local_res4[2];
          local_res4 = local_res4 + 2;
        }
        else {
          while (local_res4 = pcVar2 + 1, iVar4 - 0x30U < 10) {
            local_30 = local_30 * 10 + -0x30 + iVar4;
            iVar4 = (int)pcVar2[2];
            pcVar2 = local_res4;
          }
        }
        if (-1 < (int)local_30) {
          local_38 = local_38 | 0x10;
        }
      }
      pbVar6 = sc_buf + sizeof(sc_buf);
      if ((local_38 & 1) != 0) {
        local_38 = local_38 & 0xfffffff7;
      }
      do {
        switch(iVar4) {
        case 0x4c:
          local_38 = local_38 | 0x80;
          break;
        default:
          goto switchD_8018f218_caseD_4d;
        case 99:
          pbVar6 = &local_39;
          uVar5 = 1;
          local_39 = (byte)va_arg(ap,uint32_t);

          goto LAB_8018f700;
        case 100:
        case 0x69:
          uVar3 = va_arg(ap,uint32_t);
          if ((local_38 >> 5 & 1) != 0) {
            uVar3 = (uint)(short)uVar3;
          }
          if ((int)uVar3 < 0) {
            uVar3 = -uVar3;
            local_38 = (local_38 & 0xffff0000) | CONCAT11(0x2d,(byte)local_38);
          }
          else if ((local_38 >> 1 & 1) != 0) {
            local_38 = (local_38 & 0xffff0000) | CONCAT11(0x2b,(byte)local_38);
          }
          goto LAB_8018f2f0;
        case 0x68:
          local_38 = local_38 | 0x20;
          break;
        case 0x6c:
          local_38 = local_38 | 0x40;
          break;
        case 0x6e:
          {
            uintptr_t nv = va_arg(ap,uintptr_t);
            uint32_t ng = guest_of((const void *)nv);
            piVar7 = (int *)(ng ? guest_at(ng)
                        : ((uint32_t)nv < 0x20000000u || (uint32_t)nv >= 0x80000000u
                           ? guest_at((uint32_t)nv) : (void *)(uintptr_t)nv));
          }

          if ((local_38 >> 5 & 1) == 0) {
            *piVar7 = iVar8;
          }
          else {
            *(short *)piVar7 = (short)iVar8;
          }
          goto LAB_8018f78c;
        case 0x6f:
          uVar3 = va_arg(ap,uint32_t);

          if ((local_38 >> 5 & 1) != 0) {
            uVar3 = uVar3 & 0xffff;
          }
          if ((local_38 >> 4 & 1) == 0) {
            if ((local_38 >> 3 & 1) != 0) {
              local_30 = local_34;
            }
            if ((int)local_30 < 1) {
              local_30 = 1;
            }
          }
          uVar5 = 0;
          for (; uVar3 != 0; uVar3 = uVar3 >> 3) {
            pbVar6 = pbVar6 + -1;
            *pbVar6 = ((byte)uVar3 & 7) + 0x30;
            uVar5 = uVar5 + 1;
          }
          if ((((local_38 >> 2 & 1) != 0) && (uVar5 != 0)) && (*pbVar6 != 0x30)) {
            pbVar6 = pbVar6 + -1;
            *pbVar6 = 0x30;
            uVar5 = uVar5 + 1;
          }
          for (; (int)uVar5 < (int)local_30; uVar5 = uVar5 + 1) {
            pbVar6 = pbVar6 + -1;
            *pbVar6 = 0x30;
          }
          goto LAB_8018f700;
        case 0x70:
          local_30 = 8;
          local_38 = local_38 | 0x50;
        case 0x58:
          pcVar2 = s_0123456789ABCDEF_80125208;
          goto LAB_8018f504;
        case 0x73:
          {
            uintptr_t sv = va_arg(ap,uintptr_t);
            uint32_t sg = guest_of((const void *)sv);
            pbVar6 = sg ? (byte *)guest_at(sg)
                        : ((uint32_t)sv < 0x20000000u || (uint32_t)sv >= 0x80000000u
                           ? (byte *)guest_at((uint32_t)sv) : (byte *)sv);
          }

          if ((local_38 >> 2 & 1) == 0) {
            if ((local_38 >> 4 & 1) == 0) {
              uVar5 = str_len(pbVar6);
            }
            else {
              iVar4 = (int)(intptr_t)strchr_bounded((char *)(uintptr_t)pbVar6,0,local_30);
              uVar5 = iVar4 - (intptr_t)pbVar6;
              if (iVar4 == 0) {
                uVar5 = local_30;
              }
            }
          }
          else {
            uVar5 = (uint)*pbVar6;
            pbVar6 = pbVar6 + 1;
            if (((local_38 >> 4 & 1) != 0) && ((int)local_30 < (int)uVar5)) {
              uVar5 = local_30;
            }
          }
          goto LAB_8018f700;
        case 0x75:
          uVar3 = va_arg(ap,uint32_t);
          if ((local_38 >> 5 & 1) != 0) {
            uVar3 = uVar3 & 0xffff;
          }
          local_38 = (local_38 & 0xffff0000) | (uint16_t)(byte)local_38;
LAB_8018f2f0:

          if ((local_38 >> 4 & 1) == 0) {
            if (((local_38 >> 3 & 1) != 0) && (local_30 = local_34, ((local_38 >> 8) & 0xff) != 0)) {
              local_30 = local_34 - 1;
            }
            if ((int)local_30 < 1) {
              local_30 = 1;
            }
          }
          uVar5 = 0;
          while (uVar3 != 0) {
            pbVar6 = pbVar6 + -1;
            uVar5 = uVar5 + 1;
            *pbVar6 = (char)uVar3 + (char)(uVar3 / 10) * -10 + 0x30;
            uVar3 = uVar3 / 10;
          }
          for (; (int)uVar5 < (int)local_30; uVar5 = uVar5 + 1) {
            pbVar6 = pbVar6 + -1;
            *pbVar6 = 0x30;
          }
          if (((local_38 >> 8) & 0xff) != 0) {
            pbVar6 = pbVar6 + -1;
            *pbVar6 = (char)((local_38 >> 8) & 0xff);
            uVar5 = uVar5 + 1;
          }
          goto LAB_8018f700;
        case 0x78:
          pcVar2 = s_0123456789abcdef_8012521c;
LAB_8018f504:
          uVar3 = va_arg(ap,uint32_t);

          if ((local_38 >> 5 & 1) != 0) {
            uVar3 = uVar3 & 0xffff;
          }
          if ((local_38 >> 4 & 1) == 0) {
            if (((local_38 >> 3 & 1) != 0) && (local_30 = local_34, (local_38 >> 2 & 1) != 0)) {
              local_30 = local_34 - 2;
            }
            if ((int)local_30 < 1) {
              local_30 = 1;
            }
          }
          uVar5 = 0;
          for (; uVar3 != 0; uVar3 = uVar3 >> 4) {
            pbVar6 = pbVar6 + -1;
            uVar5 = uVar5 + 1;
            *pbVar6 = pcVar2[uVar3 & 0xf];
          }
          for (; (int)uVar5 < (int)local_30; uVar5 = uVar5 + 1) {
            pbVar6 = pbVar6 + -1;
            *pbVar6 = 0x30;
          }
          if ((local_38 >> 2 & 1) != 0) {
            pbVar6[-1] = (byte)iVar4;
            pbVar6 = pbVar6 + -2;
            *pbVar6 = 0x30;
            uVar5 = uVar5 + 2;
          }
LAB_8018f700:
          if (((int)uVar5 < (int)local_34) && ((local_38 & 1) == 0)) {
            do {
              *(undefined1 *)(dst + iVar8)= 0x20;
              local_34 = local_34 - 1;
              iVar8 = iVar8 + 1;
            } while ((int)uVar5 < (int)local_34);
          }
          mem_move((undefined1 *)(dst + iVar8),pbVar6,uVar5);
          iVar8 = iVar8 + uVar5;
          for (; (int)uVar5 < (int)local_34; uVar5 = uVar5 + 1) {
            *(undefined1 *)(dst + iVar8)= 0x20;
            iVar8 = iVar8 + 1;
          }
          goto LAB_8018f78c;
        }
        iVar4 = (int)local_res4[1];
        local_res4 = local_res4 + 1;
      } while( true );
    }
    goto LAB_8018f6f4;
  }
LAB_8018f7ac:
  *(undefined1 *)(dst + iVar8)= 0;
  return iVar8;
switchD_8018f218_caseD_4d:
  if (iVar4 != 0x25) goto LAB_8018f7ac;
LAB_8018f6f4:
  *(char *)(dst + iVar8) = (char)iVar4;
  iVar8 = iVar8 + 1;
LAB_8018f78c:
  cVar1 = local_res4[1];
  local_res4 = local_res4 + 1;
  goto joined_r0x8018efa8;

}
#undef local_39

uint * rtps_basis_xform(undefined4 *rt,undefined2 *bas,uint *out)
{
  ushort in0;
  undefined4 in2;
  uint t0;
  int t1;
  uint t2;
  int iVar6;
  int t3;
  int in1;
  uint t4;
  int t5;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,CONCAT22(bas[3],*bas));
  setCopReg(2,0x800,*(uint *)(bas + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t2 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)bas[1] | *(uint *)(bas + 4) << 0x10);
  setCopReg(2,0x800,*(uint *)(bas + 6) >> 0x10);
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)bas[2] | *(uint *)(bas + 4) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(bas + 8));
  copFunction(2,0x486012);
  in0 = bas[10];
  in1 = *(int *)(bas + 0xc);
  in2 = *(undefined4 *)(bas + 0xe);
  *out = t0 & 0xffff | t3 << 0x10;
  out[3] = t5 << 0x10 | t2 & 0xffff;
  t2 = getCopReg(2,0x4800);
  t3 = getCopReg(2,0x5000);
  t0 = getCopReg(2,0xb);
  out[4] = t0;
  setCopReg(2,0,(uint)in0 | in1 << 0x10);
  setCopReg(2,0x800,in2);
  copFunction(2,0x486012);
  out[1] = t2 & 0xffff | t1 << 0x10;
  out[2] = t3 << 0x10 | t4 & 0xffff;
  t3 = getCopReg(2,0xc800);
  t1 = getCopReg(2,0xd000);
  in1 = getCopReg(2,0xd800);
  t5 = rt[6];
  iVar6 = rt[7];
  out[5] = t3 + rt[5];
  out[6] = t1 + t5;
  out[7] = in1 + iVar6;
  return out;

}

undefined4 * rotmat2_build(uint ang,undefined4 *m)
{
  int ia;
  int ic;
  uint tv0;
  int ib;
  uint tv1;
  *m = 0x1000;
  ia = (ang & 0x3ff) * 2;
  ib = (ang & 0x3ff) * -2 + 0x800;
  ic = ia;
  if ((ang & 0x400) != 0) {
    ic = ib;
    ib = ia;
  }
  tv0 = (uint)*(ushort *)(&DAT_801247a8 + ic);
  if ((ang & 0x800) != 0) {
    tv0 = -tv0;
  }
  tv1 = (uint)*(ushort *)(&DAT_801247a8 + ib);
  if ((ang + 0x400 & 0x800) != 0) {
    tv1 = -tv1 & 0xffff;
  }
  m[1] = 0;
  m[2] = tv1 | tv0 * -0x10000;
  m[3] = tv0 * 0x10000;
  *(short *)(m + 4) = (short)tv1;
  return m;

}

/* ---- batch12: task/effect system + printf tail ---- */
/* task_invoke — guest task-callback dispatch. Task nodes store the
 * entry/exit callbacks as 32-bit GUEST code addresses and the callback
 * argument as a GUEST address of the node's inline data area.
 *
 * Ported callbacks come in two ABI flavors:
 *  - `int dst` ports treat the arg as a raw guest u32 and do
 *    `dst + off` guest-address arithmetic inside guest_at.
 *  - pointer-typed ports (`int *`, `undefined4 *`, ...) treat it as a
 *    host pointer into the guest data area.
 * The guest-u32 list below must stay in sync with those ports; targets with
 * no clean port (fn_lookup -> NULL) fall back to call_guest, which reproduces
 * the guest jalr through the generated dispatcher. */
void task_invoke(uint32_t fn_guest, uint32_t arg_guest)
{
  /* Bail-unwind in flight (g_psx_call_bail): generated CPS frames return
   * immediately without running their continuation; the clean-C equivalent
   * is to skip the callback entirely. Invoking now would run guest code the
   * wild flow already bypassed, and any br2_call_generated/call_guest inside
   * would silently drop anyway. */
  { extern int br2_bail_pending(void);
    if (br2_bail_pending()) return; }
  void *fn = fn_lookup(fn_guest);
  if (fn == (void *)0x0) {
    /* Unported target: reproduce the guest jalr — a0 = arg_guest (a guest
     * address), callee runs its generated body via the dispatcher. */
    call_guest(fn_guest, arg_guest, 0);
    return;
  }
  switch (fn_guest) {
  case 0x80138cec:   /* impact_fx_update(int)      — guest u32 arg */
  case 0x80139e0c:   /* impact_fx_emit_tick(int)      — guest u32 arg */
  case 0x8013d7e4:   /* confetti_burst_update(int)      — guest u32 arg */
  case 0x801407cc:   /* ring_effect_task_update(int)      — guest u32 arg */
  case 0x80140d38:   /* firework_task_update(int)      — guest u32 arg */
  case 0x80141610:   /* quad_fx_update(int)      — guest u32 arg */
  case 0x801ac154:   /* prim_group_gc(int)      — guest u32 arg */
  case 0x801b0db8:   /* scene_node_walk(uint32_t ctx) — guest u32 arg */
  case 0x8014b37c:   /* script_move_timer_eval(uint32) — MB32/MBS16 guest accessors need raw guest u32, not guest_at() host ptr */
    ((void (*)(uint32_t))fn)(arg_guest);
    break;
  default:
    ((void (*)(uintptr_t))fn)(arg_guest != 0 ? (uintptr_t)guest_at(arg_guest) : 0);
    break;
  }
}

int * task_node_alloc(int kind,undefined4 cnt,uint sz,int cb,int cb_arg,int dsize)
{
  int *node;
  int *headp;
  int next;
  int plist;
  plist = kind * 0x48 + -0x7fe00c68;
  if ((cb != 0) && (sz < 0x10)) {
    int allocg = task_pool_alloc(kind,cnt,dsize + 0x1c);
    node = allocg ? (int *)guest_at((uint32_t)(uintptr_t)allocg) : (int *)0x0;
    if (node != (int *)0x0) {
      headp = (int *)guest_at((uint32_t)(uintptr_t)((plist + sz * 4)));
      next = *headp;
      if (next == 0) {
        *node = 0;
      }
      else {
        *node = next;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*headp + 4) ))= guest_of(node);
      }
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((plist + sz * 4) ))= guest_of(node);
      *(undefined1 *)((intptr_t)node + 0x17) = 1;
      node[1] = 0;
      node[2] = cb;
      node[3] = cb_arg;
      *(char *)(node + 5) = (char)kind;
      *(char *)((intptr_t)node + 0x15) = (char)cnt;
      *(char *)((intptr_t)node + 0x16) = (char)sz;
      node[4] = (int)guest_of((char *)(node + 7));
      *(undefined1 *)(node + 6) = 0;
      return (int *)(uintptr_t)guest_of(node);
    }
  }
  return (int *)0x0;

}

void task_node_free(int *node)
{
  int pool;
  /* callers pass the node's GUEST address (cast to int*); resolve it */
  if (node == (int *)0x0) {
    return;
  }
  node = (int *)guest_at((uint32_t)(uintptr_t)node);
  pool = node[5];
  {
    if ((char)node[6] == '\0') {
      *(undefined1 *)(node + 6) = 1;
    }
    else {
      node[2] = 0;
      task_invoke((uint32_t)node[3],(uint32_t)node[4]);
      if (*node != 0) {
        *(int *)guest_at((uint32_t)(uintptr_t)((*node + 4) ))= node[1];
      }
      if (node[1] == 0) {
        *(int *)guest_at((uint32_t)(uintptr_t)(((char)pool * 0x48 + -0x7fe00c68 + *(char *)((intptr_t)node + 0x16) * 4) ))= *node;
      }
      else {
        *(int *)guest_at((uint32_t)(uintptr_t)(node[1] ))= *node;
      }
      task_pool_free((int)guest_of(node));
    }
  }
  return;

}

void task_pools_tick(int pool)
{
  int pbase;
  int *node;
  int *next;
  int slot;
  int *slotp;
  int gp_save = (int)guest_gp();
  uint32_t headg;
  uint32_t nextg;
  slot = 0;
  pbase = pool * 0x48;
  slotp = (int *)guest_at((uint32_t)(uintptr_t)((pbase + -0x7fe00c68)));
  *(int *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x1c) ))= pool;
  do {
    headg = (uint32_t)*slotp;
    node = headg ? (int *)guest_at(headg) : (int *)0x0;
    if (node != (int *)0x0) {
      *(int *)guest_at((uint32_t)(uintptr_t)((pbase + -0x7fe00c24) ))= slot;
      do {
        nextg = (uint32_t)*node;
        next = nextg ? (int *)guest_at(nextg) : (int *)0x0;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pbase + -0x7fe00c28) ))= guest_of(node);
        if (*(char *)((intptr_t)node + 0x17) != '\0') {
          if ((char)node[6] == '\0') {
            task_invoke((uint32_t)node[2],(uint32_t)node[4]);
          }
          else {
            task_slot_step();
          }
          /* Bail-unwind in flight: stop ticking the remaining nodes — the
           * generated task runner's CPS frames would all return immediately,
           * and clean callbacks invoked now would run dead guest code. */
          { extern int br2_bail_pending(void);
            if (br2_bail_pending()) return; }
        }
        node = next;
      } while (nextg != 0);
    }
    slot = slot + 1;
    slotp = slotp + 1;
  } while (slot < 0x10);
  return;

}

void noop_a(int u0,undefined4 u1,undefined4 u2,undefined4 u3)
{
  int datap;
  datap = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((u0 + 0x10) ))+ 8)));
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 4) ))= u1;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 8) ))= u2;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 0xc) ))= u3;
  return;

}

int fx_group_alloc(undefined4 pool,undefined4 kind,int ix,int iy)
{
  int node;
  undefined4 *pay;
  int *grp;
  grp = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iy * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((ix * 4 + -0x7fe009e0)))))));
  node = (int)(intptr_t)task_node_alloc(pool,kind,4,0x801b0db8,0,grp[1] + 0x1c);
  if (node != 0) {
    pay = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = 0;
    pay[2] = (undefined4)br2_gaddr(pay + 7);
    pay[1] = (undefined4)br2_gaddr(grp);
    pay[3] = (undefined4)(br2_gaddr(grp) + (uint32_t)*grp);
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pay[2] ))= pay[2];
  }
  return node;

}

void noop_b(int u0,undefined4 u1,undefined4 u2)
{
  int datap;
  datap = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((u0 + 0x10) ))+ 8)));
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 4) ))= u1;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 8) ))= u2;
  return;

}

undefined4 noop_c(int u0)
{
  return *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((u0 + 0x10) ))+ 8)));

}

void node_tree_free(int node)
{
  int i;
  int off;
  if (node != 0) {
    i = 0;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x40)))) {
      do {
        if (*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10)))) ))!= 0) {
          task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10)))))));
        }
        i = i + 1;
      } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x40))));
    }
    if ((undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0xc) )))!= (undefined4 *)0x0) {
      *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0xc) )))= 0;
    }
    i = 0;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x44)))) {
      do {
        off = i * 4;
        if (*(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x1c)))) ))!= 0) {
          task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x1c)))))));
        }
        if (*(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x20)))) ))!= 0) {
          task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x20)))))));
        }
        if (*(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x24)))) ))!= 0) {
          task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x24)))))));
        }
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x34)))) ))= 0;
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x38)))) ))= 0;
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x3c)))) ))= 0;
        i = i + 1;
      } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x44))));
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x18) ))!= 0) {
      task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x18))));
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x48) ))!= 0) {
      task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x48))));
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x14) ))!= 0) {
      task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x14))));
    }
  }
  return;

}

void prim_attr_word_set(int prim,undefined2 attr)
{
  int rec;
  uint k;
  int *tab;
  int slot;
  int ent;
  int head;
  slot = 0;
  head = *(int *)guest_at((uint32_t)(uintptr_t)((prim + 8)));
  tab = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((prim + 0x10))));
  ent = head;
  do {
    if (*(int *)guest_at((uint32_t)(uintptr_t)((ent + 0x5c) ))!= 0) {
      if (slot == 5) {
        rec = *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*tab + 0x10))));
        k = 0;
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((head + 0x70) ))& 0x7fffffff) == 0) {
LAB_801acae8:
          tab = tab + 1;
        }
        else {
          do {
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ))= attr;
            k = k + 1;
            rec = rec + 0x28;
          } while (k < (uint)(*(int *)guest_at((uint32_t)(uintptr_t)((ent + 0x5c) ))<< 1));
          tab = tab + 1;
        }
      }
      else if (slot < 6) {
        if (slot == 4) {
          rec = *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*tab + 0x10))));
          k = 0;
          if ((*(uint *)guest_at((uint32_t)(uintptr_t)((head + 0x6c) ))& 0x7fffffff) == 0) goto LAB_801acae8;
          do {
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ))= attr;
            k = k + 1;
            rec = rec + 0x20;
          } while (k < (uint)(*(int *)guest_at((uint32_t)(uintptr_t)((ent + 0x5c) ))<< 1));
          tab = tab + 1;
        }
      }
      else if (slot == 6) {
        rec = *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*tab + 0x10))));
        k = 0;
        if ((*(uint *)guest_at((uint32_t)(uintptr_t)((head + 0x74) ))& 0x7fffffff) != 0) {
          do {
            *(undefined2 *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ))= attr;
            k = k + 1;
            rec = rec + 0x28;
          } while (k < (uint)(*(int *)guest_at((uint32_t)(uintptr_t)((ent + 0x5c) ))<< 1));
        }
        goto LAB_801acae8;
      }
    }
    ent = ent + 4;
    slot = slot + 1;
    if (7 < slot) {
      return;
    }
  } while( true );

}

undefined4 * prim_group_build(undefined4 prim,int attr,undefined4 a2,undefined4 a3)
{
  undefined4 *grp;
  int lnode;
  undefined4 *bd;
  uint i;
  int emit;
  int *outp;
  undefined4 xf [2];
  bd = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x80))));
  grp = (undefined4 *)
           prim_group_alloc(2,attr,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x20))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x28))),
                        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x24))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x18))),a3);
  if (grp != (undefined4 *)0x0) {
    /* grp holds a GUEST address (prim_group_alloc returns the node's
     * guest pointer). Field accesses must resolve through guest_at; the
     * guest addr itself is what node_tree_free/return propagate. */
    uint32_t g1 = (uint32_t)(uintptr_t)grp;
    outp = (int *)guest_at(*(uint32_t *)guest_at(g1 + 0x10));
    xf[0] = *(undefined4 *)guest_at(g1 + 0x30);
    *(undefined4 *)guest_at(g1 + 0x8) = attr;
    *(undefined4 *)guest_at(g1 + 0xc) = prim;
    i = 0;
    if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x20) ))!= 0) {
      do {
        /* bd is a HOST pointer into the batch-descriptor table; the
         * callee indexes it as a GUEST address via guest_at(prim+off)
         * (generated passes the descriptor's guest addr in $prim). Hand it the
         * guest address, not the truncated host pointer. */
        emit = prim_batch_emit((int)(intptr_t)br2_gaddr(bd),*bd,*(undefined4 *)guest_at(g1 + 0x28),*(undefined4 *)guest_at(g1 + 0x2c),xf,
                                *(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x2c))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x34))),
                                *(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x30))),a2,
                                *(undefined4 *)guest_at((uint32_t)(uintptr_t)((attr + 0x1c))),a3);
        *outp = emit;
        outp = outp + 1;
        if (emit == 0) goto LAB_801a6cfc;
        i = i + 1;
        bd = bd + 0xd;
      } while (i < *(uint *)guest_at((uint32_t)(uintptr_t)((attr + 0x20))));
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x20) ))!= 0) {
      lnode = lazy_node_wrap(emit,*(undefined4 *)guest_at(g1 + 0x4),*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x10) ))!= 0);
      *(undefined4 *)guest_at(g1 + 0x18) = lnode;
      if (lnode == 0) {
LAB_801a6cfc:
        node_tree_free((int)(uintptr_t)grp);
        grp = (undefined4 *)0x0;
      }
      else {
        *(undefined4 *)guest_at(g1) = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((lnode + 0x10)));
      }
    }
  }
  return grp;

}

undefined4 tween_chan_alloc(int grp,int idx)
{
  undefined4 seq;
  int sub;
  if ((((grp != 0) && (sub = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 8) ))+ 0x14))), sub != 0)) &&
      (*(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x44) ))!= 0)) && ((-1 < idx && (idx < *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x44)))))))
  {
    if ((sub == 1) && (*(int *)guest_at((uint32_t)(uintptr_t)((idx * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))) ))!= 0)) {
      return 0;
    }
    sub = *(int *)guest_at((uint32_t)(uintptr_t)((grp + 8)));
    if (*(int *)guest_at((uint32_t)(uintptr_t)((sub + 0x14) ))!= 1) {
      return 1;
    }
    seq = tween_seq_alloc(2,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((sub + 0x14))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x30))),
                         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((sub + 0x8c))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((sub + 0x24))),
                         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 8) ))+ 0x30))),
                         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((idx * 8 + *(int *)guest_at((uint32_t)(uintptr_t)((sub + 0x7c) ))+ 4))),
                         (int)*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x14) ))+ 0x14))));
    idx = idx * 4;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((idx + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))) ))= seq;
    sub = *(int *)guest_at((uint32_t)(uintptr_t)((idx + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24))))));
    if (sub != 0) {
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((idx + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c)))) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((sub + 0x10)));
      return 1;
    }
  }
  return 0;

}

void tween_dur_spread(int grp,int n)
{
  int i;
  if (n < 0) {
    n = 1;
  }
  if ((grp != 0) && (i = 0, 0 < *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x40))))) {
    do {
      *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x10)))) ))+ 0x10) ))+ 0x32) ))=
           (short)n;
      i = i + 1;
    } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x40))));
  }
  return;

}

int str_len(char *s)
{
  char cVar1;
  int i;
  int len;
  len = 0;
  i = 0;
  if (s != (char *)0x0) {
    while (len = i, cVar1 = *s, s = s + 1, cVar1 != '\0') {
      i = len + 1;
    }
  }
  return len;

}

char * strchr_bounded(char *s,char c,int n)
{
  char *cur;
  cur = (char *)0x0;
  if ((s != (char *)0x0) && (0 < n)) {
    do {
      cur = s;
      n = n + -1;
      if (n < 0) {
        return (char *)0x0;
      }
      s = cur + 1;
    } while (*cur != c);
  }
  return cur;

}

undefined1 * mem_move(undefined1 *s,undefined1 *src,int n)
{
  undefined1 tmp;
  if (s < src) {
    for (; 0 < n; n = n + -1) {
      tmp = *src;
      src = src + 1;
      *s = tmp;
      s = s + 1;
    }
  }
  else {
    while (0 < n) {
      n = n + -1;
      s[n] = src[n];
    }
  }
  return s;

}

/* script_stream_tick dest-ptr anomaly latch: the quad-vertex dest pointers
 * (*s, s[1]) must be normal RAM (>=0x80100000). A value in the
 * kernel/vector page means the stream state's pointers are corrupt — record
 * the state addr, both dest ptrs, and the script base/pc for freeze_check. */
extern uint32_t g_sst_bad_state, g_sst_bad_d0, g_sst_bad_d1;
extern uint32_t g_sst_bad_sbase, g_sst_bad_spc, g_sst_bad_count;
extern uint32_t g_sst_bad_d2, g_sst_bad_d3, g_sst_bad_d4, g_sst_bad_d6;

void script_stream_tick(int *stream)
{
  short opw;
  int pc;
  uint arg;
  char *col;
  uint opc;
  ushort *ip;
  if (((uint32_t)*stream < 0x80100000u || (uint32_t)stream[1] < 0x80100000u) &&
      (uint32_t)*stream != 0u && (uint32_t)stream[1] != 0u) {
    g_sst_bad_count++;
    g_sst_bad_state = (uint32_t)(uintptr_t)stream;
    g_sst_bad_d0    = (uint32_t)*stream;
    g_sst_bad_d1    = (uint32_t)stream[1];
    g_sst_bad_sbase = (uint32_t)stream[4];
    g_sst_bad_spc   = (uint32_t)stream[5];
    g_sst_bad_d2    = (uint32_t)stream[2];
    g_sst_bad_d3    = (uint32_t)stream[3];
    g_sst_bad_d4    = (uint32_t)stream[4];
    g_sst_bad_d6    = (uint32_t)stream[6];
  }
  if (_DAT_1f800082 == 0) {
    *(short *)(stream + 6) = (short)stream[6] + -1;
  }
  if ((short)stream[6] < 1) {
    pc = stream[5];
    stream[5] = pc + 4;
    opw = *(short *)guest_at((uint32_t)(uintptr_t)((pc + 4)));
    while (((int)opw & 0xff00U) != 0) {
      ip = (ushort *)guest_at((uint32_t)(uintptr_t)(stream[5]));
      arg = *ip & 0xff;
      opc = *ip & 0xff00;
      if (opc == 0xfe00) {
        ip = (ushort *)guest_at((uint32_t)(uintptr_t)((stream[4] + arg * 4)));
LAB_80137954:
        stream[5] = (int)guest_of(ip);
      }
      else if (opc < 0xfe01) {
        if (opc == 0xfc00) {
          ip = ip + 2;
          if ((int)arg < (int)*(short *)((intptr_t)stream + 0x1a)) goto LAB_80137954;
          *(short *)((intptr_t)stream + 0x1a) = *(short *)((intptr_t)stream + 0x1a) + 1;
          stream[5] = stream[4] + *(short *)guest_at((uint32_t)(uintptr_t)((stream[5] + 2) ))* 4;
        }
      }
      else if (opc == 0xff00) {
        ip = ip + 2;
        if ((int)arg <= (int)*(short *)((intptr_t)stream + 0x1a)) {
          if (stream[2] != 0) {
            *(undefined4 *)guest_at((uint32_t)(uintptr_t)(stream[2] ))= 0;
          }
          task_slot_step();
          return;
        }
        goto LAB_80137954;
      }
      opw = *(short *)guest_at((uint32_t)(uintptr_t)(stream[5]));
    }
    col = (char *)guest_at((uint32_t)(uintptr_t)((stream[3] + *(short *)guest_at((uint32_t)(uintptr_t)(stream[5] ))* 4)));
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0xc) ))= *col;
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0xd) ))= col[1];
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0x14) ))= *col + col[2];
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0x15) ))= col[1];
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0x1c) ))= *col;
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0x1d) ))= col[1] + col[3];
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0x24) ))= *col + col[2];
    *(char *)guest_at((uint32_t)(uintptr_t)((*stream + 0x25) ))= col[1] + col[3];
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0xc) ))= *col;
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0xd) ))= col[1];
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0x14) ))= *col + col[2];
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0x15) ))= col[1];
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0x1c) ))= *col;
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0x1d) ))= col[1] + col[3];
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0x24) ))= *col + col[2];
    *(char *)guest_at((uint32_t)(uintptr_t)((stream[1] + 0x25) ))= col[1] + col[3];
    *(undefined2 *)(stream + 6) = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((stream[5] + 2)));
  }
  return;

}

/* ---- batch13: effect/task worker callbacks + misc ---- */
void slot_height_proj(undefined4 a0,int obj,int idx,int xf)
{
  short ang;
  int off;
  undefined4 res;
  undefined4 in_zero;
  undefined4 extraout_at;
  int tmp;
  int scl;
  undefined4 at_val;
  off = idx * 0x68;
  ang = *(short *)guest_at((uint32_t)(uintptr_t)((obj + 0x2a)));
  setCopControlWord(2,0,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x28))));
  setCopControlWord(2,0x800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x2c))));
  setCopControlWord(2,0x1000,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x30))));
  setCopControlWord(2,0x1800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x34))));
  setCopControlWord(2,0x2000,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x38))));
  setCopControlWord(2,0x2800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x3c))));
  setCopControlWord(2,0x3000,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x40))));
  setCopControlWord(2,0x3800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((xf + 0x44))));
  scl = (int)_DAT_801f0654;
  tmp = lut_sin_q12((int)ang);
  tmp = tmp * scl;
  if (tmp < 0) {
    tmp = tmp + 0xfff;
  }
  res = CONCAT22(_DAT_801f0656,(short)(tmp >> 0xc));
  tmp = lut_cos_q12((int)ang);
  tmp = tmp * scl;
  if (tmp < 0) {
    tmp = tmp + 0xfff;
  }
  at_val = (at_val & 0xffff0000) | (uint16_t)(short)(tmp >> 0xc);
  setCopReg(2,in_zero,res);
  setCopReg(2,extraout_at,at_val);
  copFunction(2,0x180001);
  tmp = getCopReg(2,0xe);
  if (tmp < 0) {
    tmp = tmp + 0xffff;
  }
  *(short *)(&DAT_801f066e + off) = (short)((uint)tmp >> 0x10);
  idx = idx * 0x2150;
  (&DAT_801e40b9)[idx] = *(undefined1 *)guest_at((uint32_t)(uintptr_t)((off + -0x7fe0f944)));
  (&DAT_801e40ba)[idx] = *(undefined1 *)guest_at((uint32_t)(uintptr_t)((off + -0x7fe0f943)));
  (&DAT_801e40bb)[idx] = *(undefined1 *)guest_at((uint32_t)(uintptr_t)((off + -0x7fe0f942)));
  if (*(short *)(&DAT_801f066e + off) < 0) {
    *(undefined2 *)(&DAT_801f066e + off) = 0;
  }
  else if (*(short *)guest_at((uint32_t)(uintptr_t)((xf + 0x22) ))< *(short *)(&DAT_801f066e + off)) {
    *(undefined2 *)(&DAT_801f066e + off) = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((xf + 0x22)));
  }
  return;

}

void battle_intro_fsm(undefined4 a0,undefined4 a1,undefined4 a2,int a3)
{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  if (*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0xb1) ))< '\0') {
    return;
  }
  if (_DAT_801f0738 == 1) {
    task_pool_init(1,2,0x7400,0x801d01ec);
    task_pool_init(1,1,0x1400,0x801d75ec);
    task_pool_init(2,1,0xc00,0x801d89ec);
    task_heads_clear(1);
    task_heads_clear(2);
    _DAT_801f0738 = 2;
    return;
  }
  if (_DAT_801f0738 < 2) {
    if (_DAT_801f0738 != 0) {
      return;
    }
    _DAT_801f0738 = 1;
    return;
  }
  if (_DAT_801f0738 != 2) {
    if (_DAT_801f0738 != 3) {
      return;
    }
    DAT_801bef20 = (uint)(*(short *)guest_at((uint32_t)(uintptr_t)((a3 + 0x82) ))!= 0);
    iVar6 = 0;
    iVar7 = 0;
    do {
      cVar1 = (&DAT_801f0ad8)[iVar6];
      if (cVar1 != '\0') {
        (&DAT_801f0ad8)[iVar6] = cVar1 + -1;
      }
      if ((&DAT_1f8000eb)[iVar7] == '\b') {
        iVar2 = noop_c(*(undefined4 *)(&DAT_801f0750 + (uint)(byte)(&DAT_1f8000ea)[iVar7] * 4)
                            );
        if ((*(uint *)(&DAT_1f800148 + iVar7) & 2) == 0) {
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar2 + 0x14) ))= 0;
        }
        else {
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar2 + 0x14) ))= 1;
        }
      }
      iVar6 = iVar6 + 1;
      iVar7 = iVar7 + 0x108;
    } while (iVar6 < 2);
    gp_slot48_set(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((a3 + 8))));
    gp_slot24_set(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((a3 + 0xc))));
    gte_matrix_base_set(a0,a0);
    angle_pair_rotmat(-(int)_DAT_801f0298,(int)_DAT_801f029a,(uint *)(uintptr_t)&DAT_801f0760);
    gte_rot_load((undefined4 *)(uintptr_t)&DAT_801be838);
    gte_tr_load(DAT_801be838);
    task_pools_tick(2);
    task_slot_fns_run(1);
    return;
  }
  _DAT_801f0738 = 3;
  task_node_alloc(1,2,9,0x80137590,0,0);
  _DAT_801f0734 = 0;
  iVar7 = (int)DAT_1f800098;
  _DAT_801f0730 = (int)*(short *)(&DAT_801b17f4 + iVar7 * 2);
  _DAT_801f0740 = 0;
  _DAT_801f0744 = 0;
  iVar6 = iVar7 * 8;
  _DAT_801f0938 = *(short *)(&DAT_801b1528 + iVar6) + -200;
  _DAT_801f093a = *(undefined2 *)(&DAT_801b1528 + iVar6);
  _DAT_801f0748 = 0;
  _DAT_801f093c = *(short *)(&DAT_801b152a + iVar6) + -200;
  _DAT_801f093e = *(undefined2 *)(&DAT_801b152a + iVar6);
  if (DAT_1f8000b1 == '\0') {
    switch(iVar7) {
    case 0:
      uVar3 = 0;
      break;
    default:
      goto switchD_80137384_caseD_1;
    case 3:
      fx_group_alloc(2,1,3,1);
      goto LAB_801373c0;
    case 6:
      uVar3 = 6;
    }
    orbit_fx_spawn(uVar3);
  }
switchD_80137384_caseD_1:
LAB_801373c0:
  iVar6 = 0;
  piVar8 = (int *)&DAT_801f0750;
  iVar7 = 0;
  do {
    if ((&DAT_1f8000eb)[iVar7] == '\x04') {
      if ((&DAT_1f8000ec)[iVar7] == '\x02') {
        iris_wipe_spawn((int)guest_of(&DAT_1f8000e8 + iVar7),0xfffffff);
      }
    }
    else if ((&DAT_1f8000eb)[iVar7] == '\b') {
      iVar2 = fx_group_alloc(1,1,iVar6 + 1,8);
      *piVar8 = iVar2;
      if (iVar2 != 0) {
        node_param_quad_set(*piVar8,(int)guest_of(&DAT_1f8000e8 + iVar7),*(int *)(&DAT_1f800108 + iVar7) + 0x110,0,0);
      }
    }
    piVar8 = piVar8 + 1;
    iVar6 = iVar6 + 1;
    iVar7 = iVar7 + 0x108;
  } while (iVar6 < 2);
  iVar6 = 0;
  piVar5 = (int *)&DAT_801f0790;
  piVar4 = (int *)&DAT_801f0780;
  piVar8 = (int *)guest_at(_DAT_801c4508);
  do {
    *piVar4 = *piVar8;
    iVar6 = iVar6 + 1;
    *piVar5 = piVar8[4];
    piVar8 = piVar8 + 1;
    *piVar4 = *piVar4 + (int)_DAT_801c4508;
    piVar4 = piVar4 + 1;
    *piVar5 = *piVar5 + (int)_DAT_801c4508;
    piVar5 = piVar5 + 1;
  } while (iVar6 < 4);
  DAT_801f0ad8 = 0;
  DAT_801f0ad9 = 0;
  slot_triple_set(1,0xffffffff,0xffffffff,0xffffffff);
  slot_triple_set(2,0xffffffff,0xffffffff,0xffffffff);
  _DAT_801f0758 = fighter_height_check((int)guest_of(&DAT_1f8000e8));
  _DAT_801f075c = fighter_height_check((int)guest_of(&DAT_1f8001f0));
  _DAT_801f0944 = 0;
  _DAT_801f0940 = 0;
  _DAT_801f0930 = 0;
  return;

}

int task_alloc_wrap_c(undefined4 a0,undefined4 a1,int a2,undefined4 a3)
{
  int node;
  undefined4 *pay;
  node = (int)(intptr_t)task_node_alloc(1,2,6,0x80137680,0,0x1c);
  if (node != 0) {
    pay = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = a0;
    pay[1] = a1;
    pay[2] = a2;
    pay[3] = a3;
    pay[4] = a2 + -4;
    pay[5] = 0;
    pay[6] = 0;
  }
  return node;

}

int task_alloc_wrap_d(undefined4 a0,undefined4 a1,undefined4 a2,undefined4 a3,int arg5)
{
  int node;
  undefined4 *pay;
  /* creation-time arg latch: the dest-ptr args must be real guest addrs
   * (>=0x80100000). Recording them lets freeze_check tell a bad br2_gaddr /
   * puVar14 at alloc apart from the stream state being overwritten later. */
  { extern uint32_t g_wd_last_p1,g_wd_last_p2,g_wd_last_p3,g_wd_last_p4,g_wd_last_p5;
    extern uint32_t g_wd_bad_count,g_wd_bad_node,g_wrap_src;
    g_wrap_src=2;
    g_wd_last_p1=(uint32_t)a0; g_wd_last_p2=(uint32_t)a1;
    g_wd_last_p3=(uint32_t)a2; g_wd_last_p4=(uint32_t)a3;
    g_wd_last_p5=(uint32_t)arg5;
    if (((uint32_t)a0 < 0x80100000u || (uint32_t)a1 < 0x80100000u ||
         (uint32_t)a2 < 0x80100000u) &&
        (uint32_t)a0 != 0u && (uint32_t)a1 != 0u) {
      g_wd_bad_count++; g_wd_bad_node=(uint32_t)(uintptr_t)a3;
    }
  }
  node = (int)(intptr_t)task_node_alloc(1,2,6,0x80137824,0,0x1c);
  if (node != 0) {
    pay = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = a0;
    pay[1] = a1;
    pay[2] = a2;
    pay[3] = a3;
    pay[4] = arg5;
    pay[5] = arg5 + -4;
    *(undefined2 *)(pay + 6) = 0;
    *(undefined2 *)((intptr_t)pay + 0x1a) = 0;
  }
  return node;

}

int task_alloc_wrap_e(int a0,int a1,int a2)
{
  int res;
  int *pay;
  { extern uint32_t g_wrape_calls,g_wrape_last_ret,g_wrape_p1,g_wrape_p2,g_wrape_p3;
    g_wrape_calls++; g_wrape_p1=(uint32_t)a0;
    g_wrape_p2=(uint32_t)a1; g_wrape_p3=(uint32_t)a2; }
  res = (int)(intptr_t)task_node_alloc(1,2,6,0x80137acc,0,a0 + 0x14);
  if (res == 0) {
    res = 0;
    { extern uint32_t g_wrape_last_ret; g_wrape_last_ret = 0; }
  }
  else {
    { extern uint32_t g_wrape_last_ret; g_wrape_last_ret = (uint32_t)res; }
    pay = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((res + 0x10))));
    pay[1] = a2;
    pay[2] = a2;
    pay[4] = 0;
    pay[3] = a1;
    res = *(int *)guest_at((uint32_t)(uintptr_t)((res + 0x10) ))+ 0x14;
    *pay = res;
  }
  return res;

}

int task_alloc_wrap_f(undefined4 a0,undefined4 a1,undefined4 a2,int a3)
{
  short seq_next;
  int node;
  undefined4 *pay;
  node = (int)(intptr_t)task_node_alloc(1,2,6,0x80137cd0,0,0x18);
  if (node != 0) {
    pay = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = a0;
    pay[1] = a1;
    pay[2] = a2;
    pay[3] = a3;
    pay[4] = a3;
    seq_next = *(short *)guest_at((uint32_t)(uintptr_t)((a3 + 2)));
    *(undefined2 *)((intptr_t)pay + 0x16) = 0;
    *(short *)(pay + 5) = seq_next + 1;
  }
  return node;

}

void slot_triple_tick(int *slot)
{
  short cnt;
  short *tp;
  int npc;
  tp = (short *)guest_at((uint32_t)(uintptr_t)(slot[1]));
  slot_triple_set((short)slot[2] + 1,(int)*tp,(int)tp[1],(int)tp[2]);
  cnt = (short)slot[3] + 1;
  *(short *)(slot + 3) = cnt;
  if (cnt < *(short *)((intptr_t)slot + 10)) {
    npc = slot[1] + 6;
  }
  else {
    npc = *slot;
    *(undefined2 *)(slot + 3) = 0;
  }
  slot[1] = npc;
  if (_DAT_1f800082 == 0) {
    *(short *)((intptr_t)slot + 0xe) = *(short *)((intptr_t)slot + 0xe) + -1;
  }
  if (*(short *)((intptr_t)slot + 0xe) == 0) {
    slot_triple_set((short)slot[2] + 1,0xffffffff,0xffffffff,0xffffffff);
    *(undefined4 *)(&DAT_801f0740 + (short)slot[2] * 4) = 0;
    task_slot_step();
  }
  return;

}

void xform_accum_tick(undefined4 *rec)
{
  short cnt;
  _DAT_1f8003f8 = *rec;
  _DAT_1f8003fc = rec[1];
  gte_xform_accum((short *)(uintptr_t)&DAT_1f8003f8,(uint *)(uintptr_t)(rec + 2),1,(int)*(short *)(rec + 4));
  rec_flag1_set((undefined2 *)(uintptr_t)&DAT_1f8003f8);
  if (*(short *)(rec + 4) == 0) {
    task_slot_step();
    _DAT_801f0748 = 0;
  }
  else if (((_DAT_1f800082 == 0) &&
           (cnt = *(short *)((intptr_t)rec + 0x12) + 1, *(short *)((intptr_t)rec + 0x12) = cnt,
           10 < cnt)) &&
          (cnt = *(short *)(rec + 4), *(ushort *)(rec + 4) = cnt - 0x88U,
          (int)((uint)(ushort)(cnt - 0x88U) << 0x10) < 0)) {
    *(undefined2 *)(rec + 4) = 0;
  }
  return;

}

void impact_fx_update(int rec)
{
  /* guest impact_fx_update @0x80138CEC: after the type switch, EVERY
   * non-returning case reaches a computed goto — lw v0,(0x801200B8+type*4);
   * jr v0 — into mid-function continuations (0x80138F48..0x8013A014) that
   * consume the function's live s-register file (s1=record, s3=0x1F8003FC,
   * s4=0x1F8000xx, s5=0x1F8003F8 plus caller-preserved s0/s2/s6/s7/fp).
   * That register state cannot be reconstructed from clean C locals, so the
   * whole body delegates to the generated implementation (faithful per the
   * delegation rule) rather than guessing the jump-table dispatch. */
  call_guest4(0x80138CECu,(uint32_t)rec,0,0,0);
}

void impact_fx_emit_tick(int rec)
{
  uint slot;
  int tmp;
  undefined4 type;
  _DAT_801f0734 = _DAT_801f0734 + -1;
  if (_DAT_801f0738 == 0) {
    return;
  }
  if (*(short *)guest_at((uint32_t)(uintptr_t)((rec + 100) ))!= 5) {
    return;
  }
  slot = rand_lcg();
  if ((slot & 1) != 0) {
    return;
  }
  tmp = (int)*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x58)));
  if (tmp < 0) {
    tmp = -tmp;
  }
  if (tmp <= _DAT_801f0938) {
    tmp = (int)*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x5c)));
    if (tmp < 0) {
      tmp = -tmp;
    }
    if (tmp <= _DAT_801f093c) {
      type = 0x40000006;
      slot = 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((rec + 0x5a) ))= 0;
      goto LAB_80139fb0;
    }
  }
  tmp = angle_from_xy((int)*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x58))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x5c))));
  slot = (int)(tmp + 0x200U & 0xfff) >> 10;
  tmp = burst_slot_active((&DAT_801b1ea4)[slot]);
  if (tmp == 0) {
    return;
  }
  if (slot == 1) {
    *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x58) ))= _DAT_801f093a;
  }
  else if (slot < 2) {
    if (slot == 0) {
      *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x5c) ))= -_DAT_801f093e;
    }
  }
  else if (slot == 2) {
    *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x5c) ))= _DAT_801f093e;
  }
  else if (slot == 3) {
    *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x58) ))= -_DAT_801f093a;
  }
  type = 0x40000007;
LAB_80139fb0:
  impact_fx_spawn((undefined4 *)guest_at((uint32_t)(rec + 0x58)),type,slot,0,0);
  return;

}

void fx_record_init(undefined4 *rec)
{
  undefined2 *src;
  undefined4 w1;
  if (rec[6] == -0x80000000) {
    src = (undefined2 *)guest_at((uint32_t)(uintptr_t)(rec[2]));
    *(undefined2 *)rec = *src;
    *(undefined2 *)((intptr_t)rec + 2) = src[2];
    *(undefined2 *)(rec + 1) = src[4];
    return;
  }
  w1 = ((undefined4 *)guest_at((uint32_t)(uintptr_t)(rec[2])))[1];
  *rec = *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec[2]));
  rec[1] = w1;
  return;

}

void quad_fx_update(int rec)
{
  short mode;
  undefined2 *tab;
  undefined1 *va;
  undefined4 mat;
  undefined1 *vb;
  if (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) ))== 0) {
    if (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 8) ))!= 0) {
      task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((rec + 8))));
    }
    task_slot_step();
    return;
  }
  if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((rec + 0x70) ))& 0x2000) != 0) {
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) )))+ 0x14) ))= *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0xc))));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) )))+ 0x18) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))+ 4)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) )))+ 0x1c) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))+ 8)));
  }
  if (_DAT_1f800082 == 0) {
    if ((*(ushort *)guest_at((uint32_t)(uintptr_t)((rec + 0x70) ))& 0x1000) == 0) {
      tab = &DAT_801b2348;
    }
    else {
      tab = &DAT_801b2338;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x74) ))= *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x74) ))+ tab[3];
    *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x76) ))= *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x76) ))+ tab[3];
    *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x30) ))= *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x30) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x74)));
    *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x38) ))= *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x38) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x76)));
  }
  mode = *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x72)));
  vb = &DAT_1f8003e0;
  if (mode != 1) {
    if (mode < 2) {
      if (mode != 0) {
        return;
      }
      gte_rtps_xform3((undefined4 *)guest_at((uint32_t)(rec + 0x10)),(ushort *)guest_at((uint32_t)(rec + 0x30)),(uint *)(uintptr_t)&DAT_1f8003e0);
      va = (undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + 0x50)));
      mat = *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4))));
      vb = &DAT_1f8003e0;
      goto LAB_801417f8;
    }
    if (mode == 2) {
      gte_rtps_xform3((undefined4 *)(uintptr_t)&DAT_801f0760,(ushort *)guest_at((uint32_t)(rec + 0x30)),(uint *)(uintptr_t)&DAT_1f8003e0);
      va = &DAT_1f8003e0;
      mat = *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4))));
      vb = (undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + 0x50)));
      goto LAB_801417f8;
    }
    if (mode != 3) {
      return;
    }
  }
  gte_rtps_xform3((undefined4 *)guest_at((uint32_t)(rec + 0x10)),(ushort *)guest_at((uint32_t)(rec + 0x30)),(uint *)(uintptr_t)&DAT_1f8003e0);
  va = (undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + 0x50)));
  mat = *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4))));
LAB_801417f8:
  gte_rtps_xform3((undefined4 *)(uintptr_t)va,(ushort *)(uintptr_t)vb,(uint *)guest_at((uint32_t)mat));
  return;

}

int angle_from_xy(int x,int y)
{
  int iVar1;
  int iVar2;
  if (x == 0) {
    x = 1;
  }
  if (y == 0) {
    y = 1;
  }
  iVar2 = x;
  if (x < 1) {
    iVar2 = -x;
  }
  iVar1 = y;
  if (y < 1) {
    iVar1 = -y;
  }
  if (iVar1 < iVar2) {
    if (x < 1) {
      if (y < 1) {
        iVar1 = iVar2 * -8;
      }
      else {
        iVar1 = iVar2 * 8;
      }
      iVar2 = 0xc00 - (y * 0x200 + iVar1) / iVar2;
    }
    else {
      if (y < 1) {
        iVar1 = iVar2 * -8;
      }
      else {
        iVar1 = iVar2 * 8;
      }
      iVar2 = (y * 0x200 + iVar1) / iVar2 + 0x400;
    }
  }
  else if (y < 1) {
    if (0 < x) {
      return (x * 0x200 + iVar1 * 8) / iVar1;
    }
    iVar2 = (x * 0x200 + iVar1 * -8) / iVar1 + 0xfff;
  }
  else {
    if (x < 1) {
      iVar2 = iVar1 * -8;
    }
    else {
      iVar2 = iVar1 * 8;
    }
    iVar2 = 0x800 - (x * 0x200 + iVar2) / iVar1;
  }
  return iVar2;

}

void table_1f0ae0_clear(int idx)
{
  int i;
  i = 0;
  idx = idx * 0x168;
  do {
    i = i + 1;
    *(undefined2 *)(&DAT_801f0ae0 + idx) = 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0f51e) ))= 0;
    *(undefined2 *)(&DAT_801f0ae4 + idx) = 0;
    idx = idx + 10;
  } while (i < 0x24);
  return;

}

void sprite_place_centered(undefined4 rec,undefined4 st,short x,short y,int w, int h)
{
  int h_s;
  int w_s;
  h_s = (h << 0x10) >> 0x10;
  w_s = (w << 0x10) >> 0x10;
  sprite_rect_set_c(rec,(int)x - ((w_s - ((w << 0x10) >> 0x1f)) >> 1),
               (int)y - ((h_s - ((h << 0x10) >> 0x1f)) >> 1),w_s,h_s);
  return;

}

void fighter_tint_fade(int scene,int idx,int battle,int anim)
{
  byte th1;
  byte th2;
  int rec;
  undefined1 fade;
  int tint;
  anim = anim * 0x1a;
  rec = idx * 0x68;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x82) ))!= 0) {
    tint = (int)_DAT_801f4cb4;
    goto code_r0x8016d2ec;
  }
  tint = (int)_DAT_801f4cb4 + (int)_DAT_801f4cb6;
  if (tint < 0x100) {
    if (tint < 0) {
      tint = 0;
      goto LAB_8016d2d4;
    }
  }
  else {
    tint = 0xff;
LAB_8016d2d4:
    _DAT_801f4cb6 = 0;
  }
  _DAT_801f4cb4 = (short)tint;
code_r0x8016d2ec:
  *(bool *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))= tint == 0xff;
  if ((tint != _DAT_801f4cb8) || (tint != _DAT_801f4cba)) {
    _DAT_801f4cba = _DAT_801f4cb8;
    _DAT_801f4cb8 = (short)tint;
    th1 = (&DAT_8011fea2)[anim];
    th2 = (&DAT_8011fea1)[anim];
    if ((int)((uint)(byte)(&DAT_8011fea0)[anim] - tint) < 0) {
      (&DAT_801f0664)[rec] = 0;
    }
    else {
      (&DAT_801f0664)[rec] = (char)((uint)(byte)(&DAT_8011fea0)[anim] - tint);
    }
    if ((int)((uint)th2 - tint) < 0) {
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99b) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99b) ))= (char)((uint)th2 - tint);
    }
    if ((int)((uint)th1 - tint) < 0) {
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99a) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99a) ))= (char)((uint)th1 - tint);
    }
    th1 = (&DAT_8011feae)[anim];
    th2 = (&DAT_8011fead)[anim];
    if ((int)((uint)(byte)(&DAT_8011feac)[anim] - tint) < 0) {
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f944) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f944) ))= (char)((uint)(byte)(&DAT_8011feac)[anim] - tint);
    }
    if ((int)((uint)th2 - tint) < 0) {
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f943) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f943) ))= (char)((uint)th2 - tint);
    }
    if ((int)((uint)th1 - tint) < 0) {
      *(undefined1 *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f942) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f942) ))= (char)((uint)th1 - tint);
    }
    if (tint < 0x40) {
      fade = 0;
    }
    else {
      fade = (undefined1)(((tint + -0x40) * 0x100) / 0xc0);
    }
    idx = idx * 0x58;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b310) ))= fade;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b30f) ))= fade;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b30e) ))= fade;
    *(uint *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b2f8) ))=
         *(uint *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b2f8) ))& 0xff000000 | *(uint *)guest_at((uint32_t)(uintptr_t)((scene + 0x207c) ))& 0xffffff;
    *(uint *)guest_at((uint32_t)(uintptr_t)((scene + 0x207c) ))=
         *(uint *)guest_at((uint32_t)(uintptr_t)((scene + 0x207c) ))& 0xff000000 | guest_of(&DAT_801f4cbc + idx) & 0xffffff;
  }
  return;

}

/* ---- batch14: HUD/score/round helpers (0x8012e-0x80135) ---- */
void hud_text_blk_alt(undefined4 a0,undefined2 a1,const void *a2,undefined2 a3, undefined2 arg5,short arg6)
{
  _DAT_1f80038c = 0;
  _DAT_1f80038e = 0;
  _DAT_1f800390 = 0;
  _DAT_1f800396 = 8;
  _DAT_1f800398 = 8;
  _DAT_1f80039a = 0xffff;
  _DAT_1f80039c = 8;
  _DAT_1f80039e = 0;
  _DAT_1f8003a0 = 0;
  _DAT_1f8003a4 = 0;
  _DAT_1f8003a2 = (arg6 + 0x1e0U & 0x1ff) * 0x40 + 0x1f;
  _DAT_1f800394 = arg5;
  _DAT_1f800392 = a3;
  _DAT_1f8003a8 = guest_of(a2);
  text_render_simple((uint *)guest_at(a0),a1,0x1f80038c);
  return;

}

/* hud_text_blk — 0x1f80038c text-block variant: +0x1c = raw string guest
 * addr, +0xa..0x10 all 8, +0x16 timing formula; disasm 0x8012e100-0x8012e180. */
void hud_text_blk(undefined4 a0,undefined2 a1,uint32_t a2,undefined2 a3, undefined2 arg5,short arg6)
{
  _DAT_1f80038c = 0;
  _DAT_1f80038e = 0;
  _DAT_1f800390 = 0;
  _DAT_1f800392 = a3;
  _DAT_1f800394 = arg5;
  _DAT_1f800396 = 8;
  _DAT_1f800398 = 8;
  _DAT_1f80039a = 8;
  _DAT_1f80039c = 8;
  _DAT_1f80039e = 0;
  _DAT_1f8003a0 = 0;
  _DAT_1f8003a2 = (arg6 + 0x1e0U & 0x1ff) * 0x40 + 0x1f;
  _DAT_1f8003a4 = 0;
  _DAT_1f8003a8 = a2;
  text_render_simple((uint *)guest_at(a0),a1 & 0xffff,0x1f80038c);
  return;
}

/* please_wait_announce — "PLEASE WAIT" announce when record flag bit 0x800 set
 * and 0x1f8000dc mode bits 4-5 nonzero; disasm 0x801524b0-0x80152440. */
void please_wait_announce(uint32_t a0,int a1)
{
  uint8_t *rec = (uint8_t *)guest_at(a0);
  int32_t yadj;
  uint32_t fl;
  if (rec == NULL) return;
  if (a1) yadj = (int32_t)*(int16_t *)(rec + 0x20) - 0x50;
  else    yadj = 0x50;
  fl = *(uint16_t *)((uint8_t *)guest_at(*(uint32_t *)(rec + 0xc8)) + 2);
  if (((fl >> 0xb) & 1) == 0) return;
  if (((_DAT_1f8000dc >> 4) & 3) == 0) return;
  hud_text_blk_alt(*(uint32_t *)(rec + 8),*(uint16_t *)(rec + 0xc),
               (const void *)guest_at(0x8012098c),
               (uint16_t)(int16_t)(yadj - 0x21),0x20,1);
}

/* ctr_clamp — signed counter adjust on a0+0xa0 gated by mode byte
 * a0+0x9c (mode 0, mode>5, or mode==5&&+0x9b==0); clamps [-inf,0xff].
 * Disasm 0x80159eec-0x80159f58. */
void ctr_clamp(uint32_t a0,int32_t a1)
{
  uint8_t *rec = (uint8_t *)guest_at(a0);
  int mode, adj = 0;
  if (rec == NULL) return;
  mode = rec[0x9c];
  if (mode < 5) { if (mode == 0) adj = 1; }
  else if (mode != 5 || rec[0x9b] == 0) adj = 1;
  if (!adj) return;
  a1 += *(int16_t *)(rec + 0xa0);
  if (a1 < 0) { *(int16_t *)(rec + 0xa0) = 0; return; }
  *(int16_t *)(rec + 0xa0) = (a1 < 0x100) ? (int16_t)a1 : (int16_t)0xff;
}

void announce_param_block_init_b(undefined4 a0,undefined2 a1,const void *a2,undefined2 a3, undefined2 arg5,short arg6)
{
  _DAT_1f800396 = 0x10;
  _DAT_1f80039a = 0xffff;
  _DAT_1f80038c = 0;
  _DAT_1f80038e = 0;
  _DAT_1f800390 = 0;
  _DAT_1f800398 = 0x18;
  _DAT_1f80039c = 0x18;
  _DAT_1f80039e = 0;
  _DAT_1f8003a0 = 2;
  _DAT_1f8003a4 = 2;
  _DAT_1f8003a2 = (arg6 + 0x1e0U & 0x1ff) * 0x40 + 0x1f;
  _DAT_1f800394 = arg5;
  _DAT_1f800392 = a3;
  _DAT_1f8003a8 = guest_of(a2);
  text_render_full((uint *)guest_at(a0),a1,0x1f80038c);
  return;

}

void slot_triple_set(int slot,int c0,int c1,int c2)
{
  int base;
  int off;
  base = _DAT_801f0228;
  if (c0 == -1) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((slot * 8 + 0x1f800324) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((_DAT_801f0228 + slot * 8 + 0x2c)));
  }
  else {
    *(short *)guest_at((uint32_t)(uintptr_t)((slot * 8 + 0x1f800324) ))= (short)c0;
  }
  off = slot * 8;
  if (c1 == -1) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((off + 0x1f800326) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((base + off + 0x2e)));
  }
  else {
    *(short *)guest_at((uint32_t)(uintptr_t)((off + 0x1f800326) ))= (short)c1;
  }
  slot = slot * 8;
  if (c2 == -1) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((slot + 0x1f800328) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((base + slot + 0x30)));
    return;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((slot + 0x1f800328) ))= (short)c2;
  return;

}

void rec_flag1_set(undefined2 *a0)
{
  _DAT_801f022c = 1;
  _DAT_801f0234 = *a0;
  _DAT_801f0236 = a0[1];
  _DAT_801f0238 = a0[2];
  return;

}

void target_angles_set(short *dst,int idx)
{
  short y0;
  short y1;
  undefined2 ang;
  undefined4 d2;
  int dz;
  int dx;
  dx = (int)*dst - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  dz = (int)dst[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  y0 = dst[1];
  y1 = *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  d2 = FUN_80194500(dx * dx + dz * dz);
  ang = atan2_lut(d2,(int)y0 - (int)y1);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x30) ))= ang;
  ang = atan2_lut(dz,dx);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x32) ))= ang;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x34) ))= 0;
  return;

}

void target_depth_set(short *dst,int idx)
{
  undefined4 uVar1;
  int dep2;
  int dep;
  _DAT_1f80038c = *dst - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f80038e = dst[1] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800390 = dst[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  {
    uint *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x28))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x2a))),
                       (uint *)(uintptr_t)&DAT_1f8003a0);
    gte_rtps_xform1_raw((undefined4 *)rm,(undefined4 *)(uintptr_t)&DAT_1f80038c,(undefined4 *)(uintptr_t)&DAT_1f800394);
  }
  dep = _DAT_1f800394;
  if (_DAT_1f800394 < 0) {
    dep = -_DAT_1f800394;
  }
  dep2 = _DAT_1f800398;
  if (_DAT_1f800398 < 0) {
    dep2 = -_DAT_1f800398;
  }
  dep = ((dep + 0x180) * 0x168) / 0x140;
  dep2 = ((dep2 + 0x180) * 0x168) / 0xf0;
  if (dep2 <= dep) {
    dep2 = dep;
  }
  dep = 0x4a4;
  if (0x4a3 < dep2) {
    dep = dep2;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x68) ))= (short)dep - _DAT_1f80039c;
  return;

}

void pair_depth_set(short *pa,short *pb,int idx)
{
  short dmin;
  undefined4 uVar2;
  int az;
  int ay;
  int ax;
  _DAT_1f80038c = *pa - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f80038e = pa[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800390 = pa[4] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  _DAT_1f800394 = *pb - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f800396 = pb[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800398 = pb[4] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  {
    uint *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x28))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x2a))),
                       &DAT_1f8003b4);
    gte_rtps_xform1_raw((undefined4 *)rm,(undefined4 *)(uintptr_t)&DAT_1f80038c,(undefined4 *)(uintptr_t)&DAT_1f80039c);
  }
  gte_xform1_ir(&DAT_1f800394,&DAT_1f8003a8);
  ax = _DAT_1f80039c;
  if (_DAT_1f80039c < 0) {
    ax = -_DAT_1f80039c;
  }
  ay = _DAT_1f8003a8;
  if (_DAT_1f8003a8 < 0) {
    ay = -_DAT_1f8003a8;
  }
  if (ay <= ax) {
    ay = ax;
  }
  ax = _DAT_1f8003a0;
  if (_DAT_1f8003a0 < 0) {
    ax = -_DAT_1f8003a0;
  }
  az = _DAT_1f8003ac;
  if (_DAT_1f8003ac < 0) {
    az = -_DAT_1f8003ac;
  }
  ay = ((ay * 2 + 0x180) * 0x168) / 0x140;
  if (az <= ax) {
    az = ax;
  }
  ax = ((az * 2 + 0x180) * 0x168) / 0xf0;
  if (ax <= ay) {
    ax = ay;
  }
  ay = 0x4a4;
  if (0x4a3 < ax) {
    ay = ax;
  }
  if (_DAT_1f8003a4 < _DAT_1f8003b0) {
    dmin = _DAT_1f8003a4;
  }
  else {
    dmin = _DAT_1f8003b0;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x68) ))= (short)ay - dmin;
  return;

}

void target_angles_depth_set(short *dst,int idx)
{
  short dep;
  undefined2 ang;
  undefined4 d2;
  int iVar4;
  int dx;
  int dz;
  dx = (int)*dst - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f80038c = (undefined2)dx;
  iVar4 = (int)dst[1] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f80038e = (undefined2)iVar4;
  dz = (int)dst[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  _DAT_1f800390 = (undefined2)dz;
  d2 = FUN_80194500(dx * dx + dz * dz);
  ang = atan2_lut(d2,iVar4);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x30) ))= ang;
  ang = atan2_lut(dz,dx);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x32) ))= ang;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x34) ))= 0;
  {
    uint *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x30))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x32))),
                       &DAT_1f8003a0);
    gte_rtps_xform1_raw((undefined4 *)rm,(undefined4 *)(uintptr_t)&DAT_1f80038c,(undefined4 *)(uintptr_t)&DAT_1f800394);
  }
  dep = _DAT_1f80039c;
  iVar4 = _DAT_1f800394;
  if (_DAT_1f800394 < 0) {
    iVar4 = -_DAT_1f800394;
  }
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x66) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 100) ))= 0;
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x68) ))= (short)(((iVar4 + 0x180) * 0x168) / 0x140) - dep;
  return;

}

void midpoint_angles_set(int *pa,int *pb,int idx)
{
  short sVar1;
  short mz;
  undefined2 ang;
  undefined4 d2;
  short my;
  int iVar6;
  int iVar7;
  int iVar8;
  sVar1 = (short)((*pa + *pb) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x42) ))= sVar1;
  iVar7 = (int)sVar1 - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  my = (short)((pa[1] + pb[1]) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x44) ))= my;
  mz = (short)((pa[2] + pb[2]) / 2);
  iVar8 = (int)mz - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x46) ))= mz;
  d2 = FUN_80194500(iVar7 * iVar7 + iVar8 * iVar8);
  ang = atan2_lut(d2,(int)my - (int)sVar1);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x30) ))= ang;
  ang = atan2_lut(iVar8,iVar7);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x32) ))= ang;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((idx + 0x34) ))= 0;
  _DAT_1f80038c = (short)*pa - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f80038e = (short)pa[1] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800390 = (short)pa[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  _DAT_1f800394 = (short)*pb - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f800396 = (short)pb[1] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800398 = (short)pb[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  {
    uint *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x30))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x32))),
                       &DAT_1f8003b4);
    gte_rtps_xform1_raw((undefined4 *)rm,(undefined4 *)(uintptr_t)&DAT_1f80038c,(undefined4 *)(uintptr_t)&DAT_1f80039c);
  }
  gte_xform1_ir(&DAT_1f800394,&DAT_1f8003a8);
  iVar7 = _DAT_1f80039c;
  if (_DAT_1f80039c < 0) {
    iVar7 = -_DAT_1f80039c;
  }
  iVar8 = _DAT_1f8003a8;
  if (_DAT_1f8003a8 < 0) {
    iVar8 = -_DAT_1f8003a8;
  }
  if (iVar8 <= iVar7) {
    iVar8 = iVar7;
  }
  iVar7 = _DAT_1f8003a0;
  if (_DAT_1f8003a0 < 0) {
    iVar7 = -_DAT_1f8003a0;
  }
  iVar6 = _DAT_1f8003ac;
  if (_DAT_1f8003ac < 0) {
    iVar6 = -_DAT_1f8003ac;
  }
  iVar8 = ((iVar8 * 2 + 0x180) * 0x168) / 0x140;
  if (iVar6 <= iVar7) {
    iVar6 = iVar7;
  }
  iVar7 = ((iVar6 * 2 + 0x180) * 0x168) / 0xf0;
  if (iVar7 <= iVar8) {
    iVar7 = iVar8;
  }
  iVar8 = 0x4a4;
  if (0x4a3 < iVar7) {
    iVar8 = iVar7;
  }
  if (_DAT_1f8003a4 < _DAT_1f8003b0) {
    sVar1 = _DAT_1f8003a4;
  }
  else {
    sVar1 = _DAT_1f8003b0;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x68) ))= (short)iVar8 - sVar1;
  return;

}

void dual_target_track(int tgt,int f_a,int f_b)
{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  /* Guest vec3 slots: FUN_80194630 / pair_depth_set read and write 3
   * contiguous u32 through local_48 (delta / first target pos) and
   * local_38 (normalized out / second target pos); the decompiler
   * split each 12-byte vec3 into scalars. local_48[0..2] = old
   * local_48/local_44/local_40; local_38[0..2] = old
   * local_38/local_34/local_30. */
  int local_48 [3];
  int local_38 [3];
  iVar3 = *(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20)));
  iVar11 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0x178)));
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f_a + 0x5c) ))& 0x40000) == 0) {
    iVar10 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0x34)));
    iVar3 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0x3c)));
  }
  else {
    iVar10 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0x174)));
    iVar3 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0x17c)));
  }
  iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20)));
  iVar9 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x178)));
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((f_b + 0x5c) ))& 0x40000) == 0) {
    iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x34)));
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x3c)));
  }
  else {
    iVar8 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x174)));
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x17c)));
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x52) ))= (short)((iVar8 + iVar10) / 2);
  sVar1 = (short)((iVar9 + iVar11) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x54) ))= sVar1;
  if (((-0x100 < sVar1) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((f_a + 0x5c) ))& 0x80000) == 0)) &&
     ((*(uint *)guest_at((uint32_t)(uintptr_t)((f_b + 0x5c) ))& 0x80000) == 0)) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((tgt + 0x54) ))= 0xff00;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x56) ))= (short)((iVar4 + iVar3) / 2);
  local_48[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x52) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4a)));
  local_48[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x54) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4c)));
  local_48[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x56) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4e)));
  iVar2 = FUN_80194630((undefined4 *)local_48,(undefined4 *)local_38);
  iVar7 = iVar4 - iVar3;
  if (iVar2 != 0) {
    iVar2 = FUN_80194500((uint)iVar2);
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4a) ))+ (short)((uint)(local_38[0] * iVar2) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4c) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4c) ))+ (short)((uint)(local_38[1] * iVar2) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4e) ))+ (short)((uint)(local_38[2] * iVar2) >> 0x10);
  }
  iVar5 = iVar8 - iVar10;
  iVar2 = FUN_80194500(iVar7 * iVar7 + iVar5 * iVar5);
  if (0x80 < iVar2) {
    iVar2 = atan2_lut(iVar5,iVar7);
    uVar6 = -iVar2;
    iVar2 = (*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x2a) ))+ iVar2) * 0x100000 >> 0x14;
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    if (0x400 < iVar2) {
      uVar6 = uVar6 + 0x800;
    }
    iVar2 = (int)(((int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x2a) ))- (uVar6 & 0xfff)) * 0x100000) >> 0x14;
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x32) ))= (short)(uVar6 & 0xfff);
    if (iVar2 < 0) {
      iVar2 = iVar2 + 0x3f;
    }
    iVar2 = iVar2 >> 6;
    if (iVar2 < 1) {
      iVar2 = 1;
    }
    fixed_approach_s16((ushort *)guest_at((uint32_t)(tgt + 0x2a)),(short *)guest_at((uint32_t)(tgt + 0x32)),(int)(short)iVar2);
  }
  iVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x28) ))+ -0x5b;
  if (iVar2 < 0) {
    iVar2 = -iVar2;
  }
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((tgt + 0x30) ))= 0x5b;
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0x1f;
  }
  iVar2 = iVar2 >> 5;
  if (iVar2 < 1) {
    iVar2 = 1;
  }
  fixed_approach_s16((ushort *)guest_at((uint32_t)(tgt + 0x28)),(short *)guest_at((uint32_t)(tgt + 0x30)),(int)(short)iVar2);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((tgt + 0x34) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((tgt + 0x2c) ))= 0;
  if (-0x80 < iVar11) {
    iVar11 = -0x80;
  }
  if (-0x80 < iVar9) {
    iVar9 = -0x80;
  }
  local_48[0] = iVar10;
  local_48[1] = iVar11;
  local_48[2] = iVar3;
  local_38[0] = iVar8;
  local_38[1] = iVar9;
  local_38[2] = iVar4;
  pair_depth_set((short *)local_48,(short *)local_38,tgt);
  iVar3 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x60) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x68)));
  if ((int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x60) ))< (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x68)))) {
    if (iVar3 < 0) {
      iVar3 = -iVar3;
    }
    if (iVar3 < 0) {
      iVar3 = iVar3 + 3;
    }
    iVar3 = iVar3 << 0xe;
  }
  else {
    if (iVar3 < 0) {
      iVar3 = -iVar3;
    }
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0xf;
    }
    iVar3 = iVar3 << 0xc;
  }
  u16_step_toward((ushort *)guest_at((uint32_t)(tgt + 0x60)),(ushort *)guest_at((uint32_t)(tgt + 0x68)),iVar3 >> 0x10);
  return;

}

void dual_target_track_b(int a0,int a1,int a2)
{
  short sVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  ushort uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  /* Guest vec3 slots: pair_depth_set reads 3 contiguous u32 through
   * local_48 (first target pos) and local_38 (second target pos); the
   * decompiler split each 12-byte vec3 into scalars. local_48[0..2] =
   * old local_48/local_44/local_40; local_38[0..2] = old
   * local_38/local_34/local_30. */
  int local_48 [3];
  int local_38 [3];
  iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x20)));
  iVar12 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x178)));
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((a1 + 0x5c) ))& 0x40000) == 0) {
    iVar11 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x34)));
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x3c)));
  }
  else {
    iVar11 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x174)));
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x17c)));
  }
  iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((a2 + 0x20)));
  iVar10 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x178)));
  if ((*(uint *)guest_at((uint32_t)(uintptr_t)((a2 + 0x5c) ))& 0x40000) == 0) {
    iVar9 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x34)));
    iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x3c)));
  }
  else {
    iVar9 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x174)));
    iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x17c)));
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4a) ))= (short)((iVar9 + iVar11) / 2);
  sVar1 = (short)((iVar10 + iVar12) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4c) ))= sVar1;
  iVar8 = iVar5 - iVar4;
  if (sVar1 < -0xff) {
LAB_80132e74:
    iVar3 = iVar8 * iVar8;
  }
  else {
    iVar3 = iVar8 * iVar8;
    if (((*(uint *)guest_at((uint32_t)(uintptr_t)((a1 + 0x5c) ))& 0x80000) == 0) && ((*(uint *)guest_at((uint32_t)(uintptr_t)((a2 + 0x5c) ))& 0x80000) == 0)
       ) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4c) ))= 0xff00;
      iVar8 = iVar5 - iVar4;
      goto LAB_80132e74;
    }
  }
  iVar7 = iVar9 - iVar11;
  *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4e) ))= (short)((iVar5 + iVar4) / 2);
  iVar3 = FUN_80194500(iVar3 + iVar7 * iVar7);
  if (iVar3 < 0x81) goto LAB_80132f50;
  iVar8 = atan2_lut(iVar7,iVar8);
  uVar6 = -(short)iVar8;
  sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x82)));
  if (sVar1 == -2) {
    uVar2 = uVar6 + 0x800;
LAB_80132f40:
    *(ushort *)guest_at((uint32_t)(uintptr_t)((a0 + 0x32) ))= uVar2;
    *(ushort *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2a) ))= uVar2;
  }
  else {
    if (sVar1 < -1) {
      if (sVar1 != -3) {
        *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x82) ))= 0;
        goto LAB_80132f50;
      }
      iVar8 = (*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2a) ))+ iVar8) * 0x100000 >> 0x14;
      if (iVar8 < 0) {
        iVar8 = -iVar8;
      }
      uVar2 = uVar6 & 0xfff;
      if (0x400 < iVar8) {
        uVar2 = uVar6 + 0x800 & 0xfff;
      }
      goto LAB_80132f40;
    }
    if (sVar1 == -1) {
      *(ushort *)guest_at((uint32_t)(uintptr_t)((a0 + 0x32) ))= uVar6;
      *(ushort *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2a) ))= uVar6;
    }
  }
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x82) ))= 0;
LAB_80132f50:
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x28) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2c) ))= 0;
  if (-0x80 < iVar12) {
    iVar12 = -0x80;
  }
  if (-0x80 < iVar10) {
    iVar10 = -0x80;
  }
  local_48[0] = iVar11;
  local_48[1] = iVar12;
  local_48[2] = iVar4;
  local_38[0] = iVar9;
  local_38[1] = iVar10;
  local_38[2] = iVar5;
  pair_depth_set((short *)local_48,(short *)local_38,a0);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 100)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x66)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x68)));
  return;

}

void camera_track_shot(int cam,int f_a,int f_b)
{
  undefined2 pit;
  short prev;
  uint r;
  int t;
  /* Guest vec3 slot: FUN_80194630 reads/writes 3 contiguous u32
   * through delta in place; the decompiler split the 12-byte vec3
   * into scalars. delta[0..2] = old delta/local_2c/local_28. */
  int delta [3];
  if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x178)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x17c)));
    if (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)))) {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
    }
    t = atan2_lut(*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20) ))+ 0x17c) ))-
                         *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x17c))),
                         *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20) ))+ 0x174) ))-
                         *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174))));
    prev = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
    r = table_byte_next_a();
    r = r & 7;
    if ((t - prev) * 0x100000 >> 0x14 < 0) {
      prev = *(short *)(&DAT_8011fd78 + r * 4);
    }
    else {
      prev = -*(short *)(&DAT_8011fd78 + r * 4);
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= (short)t + prev;
    pit = *(undefined2 *)(&DAT_8011fd7a + r * 4);
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0xfc00;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0xfc00;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0xfc00;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= pit;
    pair_depth_set((short *)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174),(short *)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20) ))+ 0x174),(int)(uintptr_t)cam);
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
  }
  else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))!= 1) goto LAB_801332f0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x178)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x17c)));
  if (-0x80 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)))) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= 0xff80;
  }
  delta[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a)));
  delta[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)));
  delta[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e)));
  t = FUN_80194630((undefined4 *)delta,(undefined4 *)delta);
  if (t != 0) {
    t = FUN_80194500(t);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))+ (short)((uint)(delta[0] * t) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))+ (short)((uint)(delta[1] * t) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))+ (short)((uint)(delta[2] * t) >> 0x10);
  }
  pair_depth_set((short *)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174),(short *)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20) ))+ 0x174),(int)(uintptr_t)cam);
  prev = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))+ -0x100;
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= prev;
  t = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))- (int)prev;
  if ((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))< (int)prev) {
    if (t < 0) {
      t = -t;
    }
    if (t < 0) {
      t = t + 7;
    }
    t = t << 0xd;
  }
  else {
    if (t < 0) {
      t = -t;
    }
    if (t < 0) {
      t = t + 0x1f;
    }
    t = t << 0xb;
  }
  u16_step_toward((ushort *)guest_at((uint32_t)(cam + 0x60)),(ushort *)guest_at((uint32_t)(cam + 0x68)),t >> 0x10);
LAB_801332f0:
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
  return;

}

void camera_track_step(int a0,int a1,int a2)
{
  ushort uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int local_30;
  /* Guest vec3 slots: FUN_80194630 / pair_depth_set read and write 3
   * contiguous u32 through local_50 (delta / first target pos) and
   * local_40 (normalized out / second target pos); the decompiler
   * split each 12-byte vec3 into scalars. local_50[0..2] = old
   * local_50/local_4c/local_48; local_40[0..2] = old
   * local_40/local_3c/local_38. local_30 stays a scalar. */
  int local_50 [3];
  int local_40 [3];
  iVar7 = *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x20)));
  iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((a2 + 0x20)));
  iVar11 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x174)));
  local_30 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x174)));
  iVar10 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x178)));
  iVar9 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x178)));
  iVar7 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar7 + 0x17c)));
  iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0x17c)));
  *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x52) ))= (short)((local_30 + iVar11) / 2);
  sVar2 = (short)((iVar9 + iVar10) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x54) ))= sVar2;
  if (-0x100 < sVar2) {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x54) ))= 0xff00;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x56) ))= (short)((iVar5 + iVar7) / 2);
  local_50[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x52) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4a)));
  local_50[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x54) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4c)));
  local_50[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x56) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4e)));
  iVar3 = FUN_80194630((undefined4 *)local_50,(undefined4 *)local_40);
  if (iVar3 != 0) {
    iVar3 = FUN_80194500(iVar3);
    *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4a) ))+ (short)(local_40[0] * iVar3 >> 0xc);
    *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4c) ))+ (short)(local_40[1] * iVar3 >> 0xc);
    *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x4e) ))+ (short)(local_40[2] * iVar3 >> 0xc);
  }
  iVar3 = iVar5 - iVar7;
  iVar8 = local_30 - iVar11;
  iVar4 = FUN_80194500(iVar3 * iVar3 + iVar8 * iVar8);
  if (0x80 < iVar4) {
    iVar3 = atan2_lut(iVar8,iVar3);
    uVar6 = -iVar3;
    iVar3 = (*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2a) ))+ iVar3) * 0x100000 >> 0x14;
    if (iVar3 < 0) {
      iVar3 = -iVar3;
    }
    if (0x400 < iVar3) {
      uVar6 = uVar6 + 0x800;
    }
    uVar6 = uVar6 & 0xfff;
    uVar1 = *(ushort *)(&DAT_8011fd98 + ((int)((int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2a) ))- uVar6) >> 9 & 6));
    *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x32) ))= (short)uVar6;
    iVar8 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2a) ))- ((int)((uVar6 + uVar1) * 0x10000) >> 0x10)) *
            0x100000 >> 0x14;
    if (iVar8 < 0) {
      iVar8 = -iVar8;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x32) ))= (short)(uVar6 + uVar1);
    if (iVar8 < 0) {
      iVar8 = iVar8 + 7;
    }
    iVar8 = iVar8 >> 3;
    if (iVar8 < 1) {
      iVar8 = 1;
    }
    fixed_approach_s16((ushort *)guest_at((uint32_t)(a0 + 0x2a)),(short *)guest_at((uint32_t)(a0 + 0x32)),(int)(short)iVar8);
  }
  iVar8 = iVar3 << 1;
  if (0x400 < iVar3) {
    iVar3 = 0x800 - iVar3;
    iVar8 = iVar3 * 2;
  }
  iVar3 = (0x400 - (iVar4 + iVar8 + iVar3)) * 0x2d;
  if (iVar3 < 0) {
    iVar3 = iVar3 + 0xff;
  }
  iVar3 = iVar3 >> 8;
  if (0 < iVar3) {
    iVar3 = 0;
  }
  sVar2 = atan2_lut(0x5a0,iVar3);
  *(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x30) ))= -sVar2;
  iVar3 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x28) ))- (int)-sVar2) * 0x100000 >> 0x14;
  if (iVar3 < 0) {
    iVar3 = -iVar3;
  }
  if (iVar3 < 0) {
    iVar3 = iVar3 + 7;
  }
  iVar3 = iVar3 >> 3;
  if (iVar3 < 1) {
    iVar3 = 1;
  }
  fixed_approach_s16((ushort *)guest_at((uint32_t)(a0 + 0x28)),(short *)guest_at((uint32_t)(a0 + 0x30)),(int)(short)iVar3);
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x34) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2c) ))= 0;
  if (-0x80 < iVar10) {
    iVar10 = -0x80;
  }
  if (-0x80 < iVar9) {
    iVar9 = -0x80;
  }
  local_40[0] = local_30;
  local_50[0] = iVar11;
  local_50[1] = iVar10;
  local_50[2] = iVar7;
  local_40[1] = iVar9;
  local_40[2] = iVar5;
  pair_depth_set((short *)local_50,(short *)local_40,a0);
  iVar5 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x60) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x68)));
  if ((int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x60) ))< (int)*(short *)guest_at((uint32_t)(uintptr_t)((a0 + 0x68)))) {
    if (iVar5 < 0) {
      iVar5 = -iVar5;
    }
    if (iVar5 < 0) {
      iVar5 = iVar5 + 3;
    }
    iVar5 = iVar5 << 0xe;
  }
  else {
    if (iVar5 < 0) {
      iVar5 = -iVar5;
    }
    if (iVar5 < 0) {
      iVar5 = iVar5 + 0xf;
    }
    iVar5 = iVar5 << 0xc;
  }
  u16_step_toward((ushort *)guest_at((uint32_t)(a0 + 0x60)),(ushort *)guest_at((uint32_t)(a0 + 0x68)),iVar5 >> 0x10);
  return;

}

int u16_step_toward(ushort *cur,ushort *tgt,short step)
{
  ushort old;
  short d;
  int delta;
  old = *cur;
  delta = (uint)*tgt - (uint)old;
  d = (short)delta;
  delta = delta * 0x10000 >> 0x10;
  if (delta != 0) {
    if (step == 0) {
      step = 1;
    }
    if (delta < 1) {
      if ((int)step < -delta) {
        d = -step;
        *cur = old + d;
        goto LAB_801338bc;
      }
    }
    else if (step < delta) {
      *cur = old + step;
      d = step;
      goto LAB_801338bc;
    }
    *cur = *tgt;
  }
LAB_801338bc:
  return (int)d;

}

int fixed_approach_s16(ushort *cur,short *tgt,uint step)
{
  int t20;
  int diff;
  int s20;
  int c20;
  c20 = (short)*cur * 0x100000;
  t20 = *tgt * 0x100000;
  if ((step & 0xffff) == 0) {
    step = 1;
  }
  diff = t20 + (short)*cur * -0x100000;
  s20 = step * 0x100000;
  if (diff != 0) {
    if (diff < 1) {
      if (s20 < -diff) {
        diff = step * -0x100000;
        t20 = c20 + diff;
      }
    }
    else if (s20 < diff) {
      t20 = c20 + s20;
      diff = s20;
    }
    *cur = (ushort)((uint)t20 >> 0x14);
    return diff >> 0x14;
  }
  return 0;

}

void fighter_trail_emit(undefined4 *a0,undefined4 a1,int a2,int a3)
{
  ushort uVar1;
  bool bVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int unaff_s3;
  short sVar7;
  int unaff_s4;
  int iVar8;
  undefined1 *puVar9;
  int iVar10;
  undefined4 uVar11;
  /* Guest vec3 slot: gte_xform_add_off writes a2[0..2] as 3
   * contiguous int; the decompiler split the 12-byte vec3 into
   * scalars. local_44[0..2] = old local_44/local_40/local_3c. */
  undefined4 local_44 [3];
  short local_38 [4];
  uint local_30;
  uint local_2c;
  puVar9 = &DAT_801f04a8;
  uVar11 = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((a3 + 0xc)));
  iVar10 = *(int *)guest_at((uint32_t)(uintptr_t)((a3 + 8)));
  iVar8 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x2a) ))<< 0x14) >> 0x14;
  setCopControlWord(2,0,*a0);
  setCopControlWord(2,0x800,a0[1]);
  setCopControlWord(2,0x1000,a0[2]);
  setCopControlWord(2,0x1800,a0[3]);
  setCopControlWord(2,0x2000,a0[4]);
  setCopControlWord(2,0x2800,a0[5]);
  setCopControlWord(2,0x3000,a0[6]);
  setCopControlWord(2,0x3800,a0[7]);
  iVar6 = 0;
  if (*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0x9e) ))== '\0') {
    local_30 = (uint)(-iVar8 < 0x4e4);
    local_2c = (uint)(-iVar8 < 0x31c);
    do {
      if (0 < *(short *)(puVar9 + 0x3c)) {
        local_38[2] = 0;
        local_38[1] = 0;
        local_38[0] = 0;
        if (*(short *)(puVar9 + 0x4c) == 0) {
          iVar4 = (uint)*(ushort *)(puVar9 + 0x3c) -
                  ((((int)((uint)*(ushort *)(puVar9 + 0x44) << 0x10) >> 0x10) -
                   ((int)((uint)*(ushort *)(puVar9 + 0x44) << 0x10) >> 0x1f)) >> 1);
          *(short *)(puVar9 + 0x3c) = (short)iVar4;
          if (iVar4 * 0x10000 < 1) {
LAB_80135314:
            *(undefined2 *)(puVar9 + 0x3c) = 1;
          }
LAB_8013531c:
          *(undefined2 *)(puVar9 + 0x4c) = 0x100;
        }
        else if (*(short *)(puVar9 + 0x4c) < 0) {
          uVar1 = *(ushort *)(puVar9 + 0x3c);
          *(short *)(puVar9 + 0x3c) = (short)((uint)uVar1 - (uint)*(ushort *)(puVar9 + 0x44));
          if ((int)(((uint)uVar1 - (uint)*(ushort *)(puVar9 + 0x44)) * 0x10000) < 1) {
            if ((*(short *)(puVar9 + 0x44) != 10000) || (DAT_801f04c9 != '\0')) goto LAB_80135314;
            particle_burst_spawn(iVar6,(int)*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0x98))));
            *(undefined2 *)(puVar9 + 0x34) = 0xffff;
            *(undefined2 *)(puVar9 + 0x3c) = 0;
            DAT_801f04c9 = '\x01';
            goto LAB_80135630;
          }
          goto LAB_8013531c;
        }
        if ((*(ushort *)(puVar9 + 0x4c) & 3) < 2) {
          unaff_s4 = (int)*(short *)(puVar9 + 0x44);
          if (0x80 < unaff_s4) {
            unaff_s4 = 0x80;
          }
        }
        else if ((*(ushort *)(puVar9 + 0x4c) & 3) < 4) {
          unaff_s4 = 0;
        }
        if (*(short *)guest_at((uint32_t)(uintptr_t)((a3 + 0x82) ))== 0) {
          iVar4 = *(short *)(puVar9 + 0x44) * 7;
          *(short *)(puVar9 + 0x4c) = *(short *)(puVar9 + 0x4c) + 1;
          if (iVar4 < 0) {
            iVar4 = iVar4 + 7;
          }
          *(short *)(puVar9 + 0x44) = (short)(iVar4 >> 3);
          if ((iVar4 >> 3 & 0xffffU) == 0) {
            *(undefined2 *)(puVar9 + 0x4c) = 0;
          }
        }
        sVar7 = (short)unaff_s4;
        if (iVar6 == 1) {
          sVar3 = -sVar7;
          sVar7 = local_38[2];
        }
        else {
          sVar3 = local_38[0];
          if (iVar6 < 2) {
            sVar3 = sVar7;
            sVar7 = local_38[2];
            if (iVar6 != 0) {
              sVar3 = local_38[0];
            }
          }
          else if ((iVar6 != 2) && (sVar7 = -sVar7, iVar6 != 3)) {
            sVar7 = local_38[2];
          }
        }
        local_38[2] = sVar7;
        local_38[0] = sVar3;
        if ((DAT_801f04c8 & 1) != 0) {
          bVar2 = false;
          if (iVar6 == 1) {
            iVar4 = iVar8 + -0x400;
LAB_80135460:
            iVar4 = (iVar4 << 0x14) >> 0x14;
            if (iVar4 < 0) {
              if (0x31b < -iVar4) {
                bVar2 = true;
              }
            }
            else {
joined_r0x801354dc:
              if (0x31b < iVar4) {
LAB_801354fc:
                bVar2 = true;
              }
            }
          }
          else {
            if (iVar6 < 2) {
              if (iVar6 != 0) goto LAB_80135504;
              iVar4 = iVar8 + 0x400;
              goto LAB_80135460;
            }
            if (iVar6 == 2) {
              if (iVar8 < 0) {
                if (local_30 != 0) {
                  bVar2 = true;
                }
                goto LAB_80135504;
              }
              if (0x4e3 < iVar8) goto LAB_80135504;
              goto LAB_801354fc;
            }
            if (iVar6 == 3) {
              iVar4 = iVar8;
              if (iVar8 < 0) {
                if (local_2c == 0) goto LAB_801354fc;
                goto LAB_80135504;
              }
              goto joined_r0x801354dc;
            }
          }
LAB_80135504:
          gte_xform_add_off((int)guest_of(a0),(undefined4 *)local_38,(int *)local_44);
          setCopControlWord(2,0x2800,local_44[0]);
          setCopControlWord(2,0x3000,local_44[1]);
          setCopControlWord(2,0x3800,local_44[2]);
          if (*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0x9f) ))== '\0') {
            unaff_s3 = (int)*(short *)(puVar9 + 0x34);
            if (((unaff_s3 < 0) || (iVar4 = _DAT_801f04b0 + unaff_s3 * 0x28, _DAT_801f04c6 != 0)) ||
               (!bVar2)) goto LAB_80135630;
            iVar5 = iVar10 + 0x203c;
          }
          else {
            if (iVar6 == 1) {
              unaff_s3 = 1;
            }
            else if (iVar6 < 2) {
              if (iVar6 == 0) {
                unaff_s3 = 0;
              }
            }
            else if (iVar6 == 2) {
              unaff_s3 = 3;
            }
            else if (iVar6 == 3) {
              unaff_s3 = 2;
            }
            iVar5 = iVar10 + 0x203c;
            if (bVar2) {
              iVar4 = _DAT_801f04c0 + unaff_s3 * 0x28;
            }
            else {
              iVar5 = iVar10 + 0x18;
              iVar4 = _DAT_801f04c0 + unaff_s3 * 0x28;
            }
          }
          prim_task_run(iVar4,iVar5,uVar11,1);
        }
      }
LAB_80135630:
      iVar6 = iVar6 + 1;
      puVar9 = puVar9 + 2;
    } while (iVar6 < 4);
    if (*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0x9e) ))== '\0') {
      if (*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0x9f) ))== '\0') {
        /* PTR_FUN_801b1734 is the guest u32 code-pointer table @0x801B1734
         * (channel_script_emit_* handlers). It was never populated with host
         * pointers; dispatch the real guest jalr — a0=s7=*(a3+8),
         * a1=fp=*(a3+0xc), a2=0x801F04A8, a3=a3, arg5@sp+16=
         * a0's guest address. */
        {
          int _ti = (int)*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0x98)));
          uint32_t _tf = *(uint32_t *)guest_at(0x801b1734u + (uint32_t)_ti * 4u);
          if (_tf != 0) {
            uint32_t _fr = br2_guest_frame(0x20);
            br2_st32(_fr + 0x10, br2_gaddr(a0));
            br2_call_clean(_tf,(uint32_t)iVar10,uVar11,0x801f04a8u,(uint32_t)a3);
            br2_guest_frame_free();
          }
        }
      }
      else {
        prim_task_run(_DAT_801f04c0 + 0xa0,iVar10 + 0x2074,uVar11,1);
      }
    }
  }
  if (_DAT_801f04c6 == 0) {
    particle_burst_update(iVar10,uVar11,(undefined4)br2_gaddr(a0),a3);
  }
  if (0 < _DAT_801f04c6) {
    _DAT_801f04c6 = _DAT_801f04c6 + -1;
  }
  return;

}

bool burst_slot_active(int slot)
{
  return *(short *)(&DAT_801f04e4 + slot * 2) != 0;

}

/* ---- batch15: GTE/vector + string helpers ---- */
void prim_packet_init_tex(int pkt,int flag1,int flag2,uint fl,byte *tex)
{
  uint pw;
  pw = 0xe1000000;
  *(undefined1 *)guest_at((uint32_t)(uintptr_t)((pkt + 3) ))= 2;
  if (flag2 != 0) {
    pw = 0xe1000200;
  }
  fl = fl & 0x9ff;
  if (flag1 != 0) {
    fl = fl | 0x400;
  }
  *(uint *)guest_at((uint32_t)(uintptr_t)((pkt + 4) ))= pw | fl;
  if (tex == (byte *)0x0) {
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((pkt + 8) ))= 0;
  }
  else {
    *(uint *)guest_at((uint32_t)(uintptr_t)((pkt + 8) ))=
         (uint)(tex[2] >> 3) << 0xf | (uint)(*tex >> 3) << 10 | 0xe2000000 |
         (-(int)*(short *)(tex + 6) & 0xf8U) << 2 |
         (int)(-(int)*(short *)(tex + 4) & 0xffU) >> 3;
  }
  return;

}


void gte_xform_accum(short *acc,uint *vec,int n,undefined4 tr)
{
  undefined4 old;
  uint *end;
  undefined4 rx;
  undefined4 ry;
  undefined4 rz;
  end = vec + n * 2;
  setCopReg(2,0x4000,tr);
  do {
    setCopReg(2,0x4800,*vec & 0xffff);
    setCopReg(2,0x5000,(int)*vec >> 0x10);
    setCopReg(2,0x5800,(int)(short)vec[1]);
    copFunction(2,0x198003d);
    old = *(undefined4 *)acc;
    vec = vec + 2;
    rx = getCopReg(2,0x4800);
    ry = getCopReg(2,0x5000);
    rz = getCopReg(2,0x5800);
    *acc = (short)old + (short)rx;
    acc[1] = (short)((uint)old >> 0x10) + (short)ry;
    acc[2] = acc[2] + (short)rz;
    acc = acc + 4;
  } while (vec != end);
  return;

}

uint * gte_rtps_xform3(undefined4 *rt,ushort *in,uint *out)
{
  uint t0;
  int t1;
  uint t2;
  int t3;
  uint t4;
  int t5;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,(uint)*in | *(uint *)(in + 2) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(in + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t2 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)in[1] | *(int *)(in + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)in[7]);
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)in[2] | *(uint *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(in + 8));
  copFunction(2,0x486012);
  *out = t3 << 0x10 | t0 & 0xffff;
  out[3] = t5 << 0x10 | t2 & 0xffff;
  t0 = getCopReg(2,0x4800);
  t3 = getCopReg(2,0x5000);
  out[1] = t0 & 0xffff | t1 << 0x10;
  out[2] = t3 << 0x10 | t4 & 0xffff;
  t0 = getCopReg(2,0xb);
  out[4] = t0;
  return out;

}

uint * gte_xform3_nold(ushort *in,uint *out)
{
  uint t0;
  int t1;
  uint t2;
  int t3;
  uint t4;
  int t5;
  setCopReg(2,0,(uint)*in | *(uint *)(in + 2) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(in + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t2 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)in[1] | *(int *)(in + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)in[7]);
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)in[2] | *(uint *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(in + 8));
  copFunction(2,0x486012);
  *out = t3 << 0x10 | t0 & 0xffff;
  out[3] = t5 << 0x10 | t2 & 0xffff;
  t0 = getCopReg(2,0x4800);
  t3 = getCopReg(2,0x5000);
  out[1] = t0 & 0xffff | t1 << 0x10;
  out[2] = t3 << 0x10 | t4 & 0xffff;
  t0 = getCopReg(2,0xb);
  out[4] = t0;
  return out;

}

int * gte_mvmva_add(undefined4 *rt,uint *vec,int *out)
{
  uint vx;
  int t3;
  uint vy;
  int t4;
  uint vz;
  int t5;
  int t0;
  int t1;
  int t2;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  vx = *vec;
  vy = vec[1];
  vz = vec[2];
  if ((int)vx < 0) {
    t0 = -((int)-vx >> 0xf);
    vx = -(-vx & 0x7fff);
  }
  else {
    t0 = (int)vx >> 0xf;
    vx = vx & 0x7fff;
  }
  if ((int)vy < 0) {
    t1 = -((int)-vy >> 0xf);
    vy = -(-vy & 0x7fff);
  }
  else {
    t1 = (int)vy >> 0xf;
    vy = vy & 0x7fff;
  }
  if ((int)vz < 0) {
    t2 = -((int)-vz >> 0xf);
    vz = -(-vz & 0x7fff);
  }
  else {
    t2 = (int)vz >> 0xf;
    vz = vz & 0x7fff;
  }
  setCopReg(2,0x4800,t0);
  setCopReg(2,0x5000,t1);
  setCopReg(2,0x5800,t2);
  copFunction(2,0x41e012);
  t0 = getCopReg(2,0xc800);
  t1 = getCopReg(2,0xd000);
  t2 = getCopReg(2,0xd800);
  setCopReg(2,0x4800,vx);
  setCopReg(2,0x5000,vy);
  setCopReg(2,0x5800,vz);
  copFunction(2,0x49e012);
  if (t0 < 0) {
    t0 = t0 * 8;
  }
  else {
    t0 = t0 << 3;
  }
  if (t1 < 0) {
    t1 = t1 * 8;
  }
  else {
    t1 = t1 << 3;
  }
  if (t2 < 0) {
    t2 = t2 * 8;
  }
  else {
    t2 = t2 << 3;
  }
  t3 = getCopReg(2,0xc800);
  t4 = getCopReg(2,0xd000);
  t5 = getCopReg(2,0xd800);
  *out = t3 + t0;
  out[1] = t4 + t1;
  out[2] = t5 + t2;
  return out;

}

undefined2 * gte_rtps_xform1(undefined4 *rt,undefined4 *in,undefined2 *out)
{
  undefined4 in_zero;
  undefined4 in_at;
  undefined4 gx;
  undefined4 gy;
  undefined4 gz;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,in_zero,*in);
  setCopReg(2,in_at,in[1]);
  copFunction(2,0x486012);
  gx = getCopReg(2,0x4800);
  gy = getCopReg(2,0x5000);
  gz = getCopReg(2,0x5800);
  *out = (short)gx;
  out[1] = (short)gy;
  out[2] = (short)gz;
  return out;

}

void gte_rot_load(undefined4 *m)
{
  setCopControlWord(2,0,*m);
  setCopControlWord(2,0x800,m[1]);
  setCopControlWord(2,0x1000,m[2]);
  setCopControlWord(2,0x1800,m[3]);
  setCopControlWord(2,0x2000,m[4]);
  return;

}

void gte_light_load(undefined4 *light)
{
  setCopControlWord(2,0x8000,*light);
  setCopControlWord(2,0x8800,light[1]);
  setCopControlWord(2,0x9000,light[2]);
  setCopControlWord(2,0x9800,light[3]);
  setCopControlWord(2,0xa000,light[4]);
  return;

}

void gte_tr_load(int trec)
{
  setCopControlWord(2,0x2800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((trec + 0x14))));
  setCopControlWord(2,0x3000,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((trec + 0x18))));
  setCopControlWord(2,0x3800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((trec + 0x1c))));
  return;

}

void gte_cw_d000_set(undefined4 cw)
{
  setCopControlWord(2,0xd000,cw);
  return;

}

undefined4 * gte_op_a00428(undefined4 *in,undefined4 *out)
{
  undefined4 res;
  undefined4 in_t1;
  undefined4 in_t2;
  undefined4 in_t3;
  setCopReg(2,in_t1,*in);
  setCopReg(2,in_t2,in[1]);
  setCopReg(2,in_t3,in[2]);
  copFunction(2,0xa00428);
  res = getCopReg(2,0x19);
  *out = res;
  res = getCopReg(2,0x1a);
  out[1] = res;
  res = getCopReg(2,0x1b);
  out[2] = res;
  return out;

}

void orbit_fx_spawn(int var)
{
  int tmp3;
  int tmp;
  int tmp2;
  int *cur;
  undefined2 *dp;
  int *pay;
  tmp = (int)(intptr_t)task_node_alloc(2,1,4,0x801380e4,0,0x3c);
  if (tmp != 0) {
    pay = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((tmp + 0x10))));
    tmp = 0;
    if (var == 6) {
      var = 1;
    }
    dp = &DAT_801b1d78;
    cur = pay;
    do {
      tmp2 = (int)(intptr_t)prim_group_build((undefined4)br2_gaddr(cur),*(undefined4 *)
                                   (&DAT_801c43a8 + ((byte)(&DAT_801b1d70)[tmp] + 0x1e) * 4),0,1);
      *cur = tmp2;
      if (tmp2 != 0) {
        tween_dur_spread(tmp2,*dp);
        obj_flag34_clear(*cur);
        tween_ensure(*cur);
        tmp2 = (var * 5 + tmp) * 8;
        *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 0x14) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((tmp2 + -0x7fe4e27c)));
        *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 0x18) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((tmp2 + -0x7fe4e27a)));
        *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 0x1c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((tmp2 + -0x7fe4e278)));
      }
      tmp3 = DAT_801b1848;
      tmp2 = DAT_801b1844;
      cur = cur + 1;
      tmp = tmp + 1;
      dp = dp + 1;
    } while (tmp < 5);
    pay[5] = DAT_801b1840;
    pay[6] = tmp2;
    pay[7] = tmp3;
    tmp2 = DAT_801b1854;
    tmp = DAT_801b1850;
    pay[8] = DAT_801b184c;
    pay[9] = tmp;
    pay[10] = tmp2;
    tmp = DAT_801b185c;
    pay[0xb] = DAT_801b1858;
    pay[0xc] = tmp;
    var = var * 0x28;
    *(undefined2 *)(pay + 0xd) = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((var + -0x7fe4e27c)));
    *(undefined2 *)((intptr_t)pay + 0x36) = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((var + -0x7fe4e27a)));
    *(undefined2 *)(pay + 0xe) = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((var + -0x7fe4e278)));
  }
  return;

}

/* ---- batch16: task/resource internals ---- */
void task_heads_clear(int pool)
{
  int i;
  i = 0;
  pool = pool * 0x48;
  do {
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((pool + -0x7fe00c68) ))= 0;
    i = i + 1;
    pool = pool + 4;
  } while (i < 0x10);
  return;

}

void task_slot_step(void)
{
  int pbase;
  int *node;
  int gp_save = (int)guest_gp();
  pbase = *(int *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x1c) ))* 0x48;
  node = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((pbase + -0x7fe00c28))));
  if ((char)node[6] == '\0') {
    *(undefined1 *)(node + 6) = 1;
  }
  else {
    node[2] = 0;
    task_invoke((uint32_t)node[3],(uint32_t)node[4]);
    if (*node != 0) {
      *(int *)guest_at((uint32_t)(uintptr_t)((*node + 4) ))= node[1];
    }
    if (node[1] == 0) {
      *(int *)guest_at((uint32_t)(uintptr_t)((pbase + -0x7fe00c68 + *(int *)guest_at((uint32_t)(uintptr_t)((pbase + -0x7fe00c24) ))* 4) ))= *node;
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)(node[1] ))= *node;
    }
    task_pool_free((int)guest_of(node));
  }
  return;

}

void task_slot_fns_run(int slot)
{
  int ctx;
  int i;
  int gp_save = (int)guest_gp();
  int argblk [2];
  i = 0;
  argblk[0] = slot * 0x48 + -0x7fe00c68;
  *(int *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x1c) ))= slot;
  do {
    if (*(int *)guest_at((uint32_t)(uintptr_t)((argblk[0] + i * 4) ))!= 0) {
      ctx = *(int *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x20)));
      *(int *)guest_at((uint32_t)(uintptr_t)((argblk[0] + 0x44) ))= i;
      {
        void *fn = fn_lookup((uint32_t)*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + ctx))));
        if (fn != (void *)0x0) {
          /* Guest jalr passes slot = &ctx_base cell (sp+0x10) and
           * a1 = ctx_base[i] node-head guest addr.  The clean slot fns take
           * host pointers, so resolve a1 through guest_at. */
          uint32_t nodeg_ = (uint32_t)*(int *)guest_at((uint32_t)(uintptr_t)((argblk[0] + i * 4)));
          ((void (*)(uintptr_t,uintptr_t))fn)((uintptr_t)argblk, (uintptr_t)guest_at(nodeg_));
        }
      }
    }
    i = i + 1;
  } while (i < 0x10);
  return;

}

void task_pool_init(int pool,int kind,int count,int base)
{
  int *hdr;
  hdr = (int *)guest_at((uint32_t)(uintptr_t)((pool * 0x3c + kind * 0xc + -0x7fe00d98)));
  *hdr = base;
  hdr[1] = base;
  hdr[2] = *hdr + count * 0x10;
  *(int *)guest_at((uint32_t)(uintptr_t)(hdr[1] ))= *hdr;
  *(int *)guest_at((uint32_t)(uintptr_t)((hdr[1] + 4) ))= *hdr;
  *(int *)guest_at((uint32_t)(uintptr_t)((hdr[1] + 8) ))= count + -0x10;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((hdr[1] + 0xc) ))= 0;
  task_pool_alloc(pool,kind,0);
  return;

}

/* task_pool_alloc anomaly latch globals — defined in runtime interrupts.c so
 * both the full and -us builds link them (battle.c is not in the -us target). */
extern uint32_t g_tpool_bad_prev,  g_tpool_bad_cur,   g_tpool_bad_link;
extern uint32_t g_tpool_bad_count;
extern int32_t  g_tpool_bad_p1,    g_tpool_bad_p2,    g_tpool_bad_p3;
/* alloc-failure latch: records the freelist head + first-block fields at the
 * moment task_pool_alloc gives up, so a versus-handshake wedge can be
 * diagnosed post-mortem (the pool looks healthy again by the time we probe). */
extern uint32_t g_tpool_fail_count, g_tpool_fail_head, g_tpool_fail_next,
                g_tpool_fail_size, g_tpool_fail_flag;
extern int32_t  g_tpool_fail_p1, g_tpool_fail_p2, g_tpool_fail_p3;

int task_pool_alloc(int pool,int kind,int size)
{
  short flag;
  int *prev;
  uint need;
  int blk;
  int cur;
  int nb;
  int *hdr;
  hdr = (int *)guest_at((uint32_t)(uintptr_t)((pool * 0x3c + kind * 0xc + -0x7fe00d98)));
  blk = hdr[1];
  flag = *(short *)guest_at((uint32_t)(uintptr_t)((blk + 0xc)));
  cur = blk;
  while ((flag != 0 || (*(int *)guest_at((uint32_t)(uintptr_t)((cur + 8) ))< size))) {
    int prev_blk = cur;
    uint32_t linkv = (uint32_t)*(int *)guest_at((uint32_t)(uintptr_t)((cur + 4)));
    cur = (int)linkv;
    if ((uint32_t)cur < 0x80100000u && cur != 0) {
      g_tpool_bad_count++;
      g_tpool_bad_prev = (uint32_t)prev_blk;
      g_tpool_bad_cur  = (uint32_t)cur;
      g_tpool_bad_link = linkv;
      g_tpool_bad_p1 = pool; g_tpool_bad_p2 = kind; g_tpool_bad_p3 = size;
    }
    if (cur == blk) {
      g_tpool_fail_count++;
      g_tpool_fail_p1 = pool; g_tpool_fail_p2 = kind;
      g_tpool_fail_p3 = size;
      g_tpool_fail_head = (uint32_t)blk;
      if (blk >= 0x80100000 && blk < 0x80200000) {
        int *hb = (int *)guest_at((uint32_t)(uintptr_t)blk);
        g_tpool_fail_next = (uint32_t)hb[1];
        g_tpool_fail_size = (uint32_t)hb[2];
        g_tpool_fail_flag = (uint32_t)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(blk + 0xc)));
      }
      return 0;
    }
    flag = *(short *)guest_at((uint32_t)(uintptr_t)((cur + 0xc)));
  }
  need = size + 3U & 0xfffffffc;
  if (0x1f < (int)(*(int *)guest_at((uint32_t)(uintptr_t)((cur + 8) ))- need)) {
    blk = cur + need;
    nb = blk + 0x10;
    *(uint *)guest_at((uint32_t)(uintptr_t)((blk + 0x18) ))= (*(int *)guest_at((uint32_t)(uintptr_t)((cur + 8) ))+ -0x10) - need;
    *(int *)guest_at((uint32_t)(uintptr_t)((blk + 0x10) ))= cur;
    /* free-list link fields hold GUEST addresses: resolve before deref,
     * store via guest_of */
    prev = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((cur + 4))));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((blk + 0x14) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((cur + 4)));
    *prev = nb;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((blk + 0x1c) ))= 0;
    *(uint *)guest_at((uint32_t)(uintptr_t)((cur + 8) ))= need;
    *(int *)guest_at((uint32_t)(uintptr_t)((cur + 4) ))= nb;
    *(short *)guest_at((uint32_t)(uintptr_t)((cur + 0xc) ))= (short)kind + 1;
    *(short *)guest_at((uint32_t)(uintptr_t)((cur + 0xe) ))= (short)pool;
    if (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((blk + 0x14) ))== (uint32_t)*hdr) {
      *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((blk + 0x14) )))= (uint32_t)nb;
    }
    hdr[1] = nb;
    return cur + 0x10;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((cur + 0xc) ))= (short)kind + 1;
  *(short *)guest_at((uint32_t)(uintptr_t)((cur + 0xe) ))= (short)pool;
  return cur + 0x10;

}

void task_pool_free(int pool)
{
  short kind;
  int *bh;
  int next;
  int phdr;
  bh = (int *)guest_at((uint32_t)(uintptr_t)((pool + -0x10)));
  kind = *(short *)guest_at((uint32_t)(uintptr_t)((pool + -4)));
  next = *(int *)guest_at((uint32_t)(uintptr_t)((pool + -0xc)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((pool + -4) ))= 0;
  phdr = *(short *)guest_at((uint32_t)(uintptr_t)((pool + -2) ))* 0x3c + -0x7fe00da4 + kind * 0xc;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((next + 0xc) ))== 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)((pool + -8) ))= *(int *)guest_at((uint32_t)(uintptr_t)((pool + -8) ))+ 0x10 + *(int *)guest_at((uint32_t)(uintptr_t)((next + 8)));
    if (next == *(int *)guest_at((uint32_t)(uintptr_t)((phdr + 4)))) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((phdr + 4) ))= (uint32_t)guest_of(bh);
    }
    /* free-list links are GUEST addresses: resolve inner ptr, store guest addr */
    *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) ))+ 4) )))= (uint32_t)guest_of(bh);
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) ))+ 4)));
  }
  next = *bh;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((next + 0xc) ))== 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)((next + 8) ))= *(int *)guest_at((uint32_t)(uintptr_t)((next + 8) ))+ 0x10 + *(int *)guest_at((uint32_t)(uintptr_t)((pool + -8)));
    if ((int)guest_of(bh) == *(int *)guest_at((uint32_t)(uintptr_t)((phdr + 4)))) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((phdr + 4) ))= (uint32_t)*bh;
    }
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*bh + 4) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((pool + -0xc)));
    *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) )))= (uint32_t)*bh;
  }
  return;

}

void gp_slot48_set(undefined4 v)
{
  int gp_save = (int)guest_gp();
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x48) ))= v;
  return;

}

void gp_slot24_set(undefined4 v)
{
  int gp_save = (int)guest_gp();
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x24) ))= v;
  return;

}

int prim_batch_emit(int a0,int a1,undefined4 a2,undefined4 a3,int *arg5, undefined4 arg6,undefined4 arg7,int arg8,int arg9,uint arg10, undefined4 arg11)
{
  undefined2 uVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  byte *pbVar6;
  byte *pbVar7;
  undefined4 *puVar8;
  undefined2 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  int iVar12;
  undefined2 *puVar13;
  ushort *puVar14;
  undefined4 *puVar15;
  undefined4 *puVar16;
  undefined2 *puVar17;
  undefined4 *puVar18;
  undefined2 *puVar19;
  undefined4 *puVar20;
  /* PTR_PTR_801be894 / PTR_DAT_801be84c are guest pointer-tables: entry
   * [arg9] is a guest sub-table address, then [a1] indexes it.
   * Read the 32-bit guest entries via guest_at (never host ** deref). */
  uint32_t ptrsub = *(uint32_t *)guest_at(0x801be894u + (uint32_t)arg9 * 4);
  uint32_t datsub = *(uint32_t *)guest_at(0x801be84cu + (uint32_t)arg9 * 4);
  iVar3 = (int)(intptr_t)task_node_alloc(arg11,2,0xd,*(undefined4 *)guest_at(ptrsub + (uint32_t)a1 * 4),0,
                       (uint)*(uint8_t *)guest_at(datsub + (uint32_t)a1) * *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8) ))* 2
                       + *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 0xc) ))* 0x10 + 0x38);
  if (iVar3 == 0) {
    return 0;
  }
  puVar20 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0x10))));
  puVar16 = puVar20 + 0xe;
  puVar11 = puVar16 + *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 0xc) ))* 4;
  *puVar20 = (undefined4)br2_gaddr(puVar11);
  bVar2 = *(uint8_t *)guest_at(datsub + (uint32_t)a1);
  iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)));
  puVar20[2] = (undefined4)br2_gaddr(puVar16);
  puVar20[1] = (undefined4)(br2_gaddr(puVar11) + (uint)bVar2 * iVar4);
  iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 0xc)));
  puVar20[4] = a2;
  puVar20[7] = a3;
  puVar20[3] = (undefined4)br2_gaddr(puVar16 + iVar4 * 2);
  puVar20[6] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x18)));
  uVar1 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a0 + 8)));
  puVar20[10] = iVar3;
  puVar20[0xb] = 0;
  *(undefined2 *)(puVar20 + 0xc) = uVar1;
  puVar20[8] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x1c)));
  puVar20[9] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20)));
  uVar5 = *(undefined4 *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24)));
  *(undefined2 *)((intptr_t)puVar20 + 0x32) = 0;
  *(undefined2 *)(puVar20 + 0xd) = 1;
  puVar20[5] = uVar5;
  iVar4 = 0;
  if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 0xc) ))<< 1) {
    do {
      *puVar16 = 0x1000000;
      puVar16[1] = (arg10 & 0x4f) << 5 | 0xe1000000;
      iVar4 = iVar4 + 1;
      puVar16 = puVar16 + 2;
    } while (iVar4 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 0xc) ))<< 1);
  }
  switch(a1 + arg9 * 0x100) {
  case 0:
    iVar4 = 0;
    puVar11 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*puVar20));
    puVar16 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(puVar20[1]));
    puVar8 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
    puVar18 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
    puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
    puVar15 = puVar18 + arg8;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
      pbVar7 = (byte *)((intptr_t)puVar16 + 7);
      pbVar6 = (byte *)((intptr_t)puVar11 + 7);
      do {
        if (puVar20[7] == 0) {
          *(undefined4 *)(pbVar6 + -3) = *puVar8;
          *(undefined4 *)(pbVar7 + -3) = *puVar8;
          *pbVar6 = *pbVar6 | 1;
          bVar2 = *pbVar7 | 1;
        }
        else {
          *pbVar6 = *pbVar6 & 0xfe;
          bVar2 = *pbVar7 & 0xfe;
        }
        *pbVar7 = bVar2;
        if (arg8 != 0) {
          *puVar18 = (undefined4)br2_gaddr(puVar11 + 1);
          puVar18 = puVar18 + 1;
          *puVar15 = (undefined4)br2_gaddr(puVar16 + 1);
          puVar15 = puVar15 + 1;
        }
        *puVar11 = 0x4000000;
        *pbVar6 = 0x20;
        *puVar16 = 0x4000000;
        *pbVar7 = 0x20;
        if ((*puVar14 & 0x100) == 0) {
          *pbVar6 = *pbVar6 & 0xfd;
          bVar2 = *pbVar7 & 0xfd;
        }
        else {
          *pbVar6 = *pbVar6 | 2;
          bVar2 = *pbVar7 | 2;
        }
        *pbVar7 = bVar2;
        puVar14 = puVar14 + 1;
        puVar8 = puVar8 + 1;
        pbVar6 = pbVar6 + 0x14;
        puVar11 = puVar11 + 5;
        pbVar7 = pbVar7 + 0x14;
        iVar4 = iVar4 + 1;
        puVar16 = puVar16 + 5;
      } while (iVar4 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
    }
    iVar12 = *arg5;
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8) ))<< 2;
    break;
  case 1:
    iVar4 = 0;
    puVar11 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*puVar20));
    puVar16 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(puVar20[1]));
    puVar8 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
    puVar18 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
    puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
    puVar15 = puVar18 + arg8;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
      pbVar7 = (byte *)((intptr_t)puVar16 + 7);
      pbVar6 = (byte *)((intptr_t)puVar11 + 7);
      do {
        if (puVar20[7] == 0) {
          *(undefined4 *)(pbVar6 + -3) = *puVar8;
          *(undefined4 *)(pbVar7 + -3) = *puVar8;
          *pbVar6 = *pbVar6 | 1;
          bVar2 = *pbVar7 | 1;
        }
        else {
          *pbVar6 = *pbVar6 & 0xfe;
          bVar2 = *pbVar7 & 0xfe;
        }
        *pbVar7 = bVar2;
        if (arg8 != 0) {
          *puVar18 = (undefined4)br2_gaddr(puVar11 + 1);
          puVar18 = puVar18 + 1;
          *puVar15 = (undefined4)br2_gaddr(puVar16 + 1);
          puVar15 = puVar15 + 1;
        }
        *puVar11 = 0x5000000;
        *pbVar6 = 0x28;
        *puVar16 = 0x5000000;
        *pbVar7 = 0x28;
        if ((*puVar14 & 0x100) == 0) {
          *pbVar6 = *pbVar6 & 0xfd;
          bVar2 = *pbVar7 & 0xfd;
        }
        else {
          *pbVar6 = *pbVar6 | 2;
          bVar2 = *pbVar7 | 2;
        }
        *pbVar7 = bVar2;
        puVar14 = puVar14 + 1;
        puVar8 = puVar8 + 1;
        pbVar6 = pbVar6 + 0x18;
        puVar11 = puVar11 + 6;
        pbVar7 = pbVar7 + 0x18;
        iVar4 = iVar4 + 1;
        puVar16 = puVar16 + 6;
      } while (iVar4 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
    }
    iVar12 = *arg5;
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8) ))<< 2;
    break;
  case 2:
    iVar4 = 0;
    puVar18 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*puVar20));
    puVar8 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(puVar20[1]));
    puVar15 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
    puVar16 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
    puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
    puVar11 = puVar16 + arg8;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
      pbVar7 = (byte *)((intptr_t)puVar8 + 7);
      pbVar6 = (byte *)((intptr_t)puVar18 + 7);
      puVar10 = puVar15 + 2;
      do {
        if (puVar20[7] == 0) {
          *(undefined4 *)(pbVar6 + -3) = *puVar15;
          *(undefined4 *)(pbVar7 + -3) = *puVar15;
          *(undefined4 *)(pbVar6 + 5) = puVar10[-1];
          *(undefined4 *)(pbVar7 + 5) = puVar10[-1];
          *(undefined4 *)(pbVar6 + 0xd) = *puVar10;
          *(undefined4 *)(pbVar7 + 0xd) = *puVar10;
          *pbVar6 = *pbVar6 | 1;
          bVar2 = *pbVar7 | 1;
        }
        else {
          *pbVar6 = *pbVar6 & 0xfe;
          bVar2 = *pbVar7 & 0xfe;
        }
        *pbVar7 = bVar2;
        if (arg8 != 0) {
          *puVar16 = (undefined4)br2_gaddr(puVar18 + 1);
          puVar16[1] = (undefined4)(br2_gaddr(puVar18) + 3);
          puVar16[2] = (undefined4)(br2_gaddr(puVar18) + 5);
          puVar16 = puVar16 + 3;
          *puVar11 = (undefined4)br2_gaddr(puVar8 + 1);
          puVar11[1] = (undefined4)(br2_gaddr(puVar8) + 3);
          puVar11[2] = (undefined4)(br2_gaddr(puVar8) + 5);
          puVar11 = puVar11 + 3;
        }
        *puVar18 = 0x6000000;
        *pbVar6 = 0x30;
        *puVar8 = 0x6000000;
        *pbVar7 = 0x30;
        if ((*puVar14 & 0x100) == 0) {
          *pbVar6 = *pbVar6 & 0xfd;
          bVar2 = *pbVar7 & 0xfd;
        }
        else {
          *pbVar6 = *pbVar6 | 2;
          bVar2 = *pbVar7 | 2;
        }
        *pbVar7 = bVar2;
        puVar14 = puVar14 + 1;
        puVar10 = puVar10 + 3;
        puVar15 = puVar15 + 3;
        pbVar6 = pbVar6 + 0x1c;
        puVar18 = puVar18 + 7;
        pbVar7 = pbVar7 + 0x1c;
        iVar4 = iVar4 + 1;
        puVar8 = puVar8 + 7;
      } while (iVar4 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
    }
    goto LAB_801a83ec;
  case 3:
    iVar4 = 0;
    puVar18 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*puVar20));
    puVar8 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(puVar20[1]));
    puVar15 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
    puVar11 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
    puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
    puVar16 = puVar11 + arg8;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
      pbVar7 = (byte *)((intptr_t)puVar8 + 7);
      pbVar6 = (byte *)((intptr_t)puVar18 + 7);
      puVar10 = puVar15 + 3;
      do {
        if (puVar20[7] == 0) {
          *(undefined4 *)(pbVar6 + -3) = *puVar15;
          *(undefined4 *)(pbVar7 + -3) = *puVar15;
          *(undefined4 *)(pbVar6 + 5) = puVar10[-2];
          *(undefined4 *)(pbVar7 + 5) = puVar10[-2];
          *(undefined4 *)(pbVar6 + 0xd) = puVar10[-1];
          *(undefined4 *)(pbVar7 + 0xd) = puVar10[-1];
          *(undefined4 *)(pbVar6 + 0x15) = *puVar10;
          *(undefined4 *)(pbVar7 + 0x15) = *puVar10;
          *pbVar6 = *pbVar6 | 1;
          bVar2 = *pbVar7 | 1;
        }
        else {
          *pbVar6 = *pbVar6 & 0xfe;
          bVar2 = *pbVar7 & 0xfe;
        }
        *pbVar7 = bVar2;
        if (arg8 != 0) {
          *puVar11 = (undefined4)br2_gaddr(puVar18 + 1);
          puVar11[1] = (undefined4)(br2_gaddr(puVar18) + 3);
          puVar11[2] = (undefined4)(br2_gaddr(puVar18) + 5);
          puVar11[3] = (undefined4)(br2_gaddr(puVar18) + 7);
          puVar11 = puVar11 + 4;
          *puVar16 = (undefined4)br2_gaddr(puVar8 + 1);
          puVar16[1] = (undefined4)(br2_gaddr(puVar8) + 3);
          puVar16[2] = (undefined4)(br2_gaddr(puVar8) + 5);
          puVar16[3] = (undefined4)(br2_gaddr(puVar8) + 7);
          puVar16 = puVar16 + 4;
        }
        *puVar18 = 0x8000000;
        *pbVar6 = 0x38;
        *puVar8 = 0x8000000;
        *pbVar7 = 0x38;
        if ((*puVar14 & 0x100) == 0) {
          *pbVar6 = *pbVar6 & 0xfd;
          bVar2 = *pbVar7 & 0xfd;
        }
        else {
          *pbVar6 = *pbVar6 | 2;
          bVar2 = *pbVar7 | 2;
        }
        *pbVar7 = bVar2;
        puVar14 = puVar14 + 1;
        puVar10 = puVar10 + 4;
        puVar15 = puVar15 + 4;
        pbVar6 = pbVar6 + 0x24;
        puVar18 = puVar18 + 9;
        pbVar7 = pbVar7 + 0x24;
        iVar4 = iVar4 + 1;
        puVar8 = puVar8 + 9;
      } while (iVar4 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
    }
    goto LAB_801a85d4;
  case 4:
    iVar4 = 0;
    puVar11 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*puVar20));
    puVar16 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(puVar20[1]));
    puVar8 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
    puVar13 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x30))));
    puVar19 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x28))));
    puVar17 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2c))));
    puVar18 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
    puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
    puVar15 = puVar18 + arg8;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
      pbVar7 = (byte *)((intptr_t)puVar16 + 7);
      pbVar6 = (byte *)((intptr_t)puVar11 + 7);
      puVar9 = puVar13 + 2;
      do {
        *(undefined2 *)(pbVar6 + 7) = *puVar19;
        *(undefined2 *)(pbVar7 + 7) = *puVar19;
        *(undefined2 *)(pbVar6 + 0xf) = *puVar17;
        *(undefined2 *)(pbVar7 + 0xf) = *puVar17;
        *(undefined2 *)(pbVar6 + 5) = *puVar13;
        *(undefined2 *)(pbVar7 + 5) = *puVar13;
        *(undefined2 *)(pbVar6 + 0xd) = puVar9[-1];
        *(undefined2 *)(pbVar7 + 0xd) = puVar9[-1];
        *(undefined2 *)(pbVar6 + 0x15) = *puVar9;
        *(undefined2 *)(pbVar7 + 0x15) = *puVar9;
        if (puVar20[7] == 0) {
          *(undefined4 *)(pbVar6 + -3) = *puVar8;
          *(undefined4 *)(pbVar7 + -3) = *puVar8;
          *pbVar6 = *pbVar6 | 1;
          bVar2 = *pbVar7 | 1;
        }
        else {
          *pbVar6 = *pbVar6 & 0xfe;
          bVar2 = *pbVar7 & 0xfe;
        }
        *pbVar7 = bVar2;
        if (arg8 != 0) {
          *puVar18 = (undefined4)br2_gaddr(puVar11 + 1);
          puVar18 = puVar18 + 1;
          *puVar15 = (undefined4)br2_gaddr(puVar16 + 1);
          puVar15 = puVar15 + 1;
        }
        *puVar11 = 0x7000000;
        *pbVar6 = 0x24;
        *puVar16 = 0x7000000;
        *pbVar7 = 0x24;
        if ((*puVar14 & 0x100) == 0) {
          *pbVar6 = *pbVar6 & 0xfd;
          bVar2 = *pbVar7 & 0xfd;
        }
        else {
          *pbVar6 = *pbVar6 | 2;
          bVar2 = *pbVar7 | 2;
        }
        *pbVar7 = bVar2;
        puVar14 = puVar14 + 1;
        puVar8 = puVar8 + 1;
        puVar9 = puVar9 + 3;
        puVar13 = puVar13 + 3;
        puVar19 = puVar19 + 1;
        puVar17 = puVar17 + 1;
        pbVar6 = pbVar6 + 0x20;
        puVar11 = puVar11 + 8;
        pbVar7 = pbVar7 + 0x20;
        iVar4 = iVar4 + 1;
        puVar16 = puVar16 + 8;
      } while (iVar4 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
    }
    iVar12 = *arg5;
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8) ))<< 2;
    break;
  case 5:
    iVar4 = 0;
    puVar11 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*puVar20));
    puVar16 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(puVar20[1]));
    puVar8 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
    puVar13 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x30))));
    puVar19 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x28))));
    puVar17 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2c))));
    puVar18 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
    puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
    puVar15 = puVar18 + arg8;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
      pbVar7 = (byte *)((intptr_t)puVar16 + 7);
      pbVar6 = (byte *)((intptr_t)puVar11 + 7);
      puVar9 = puVar13 + 3;
      do {
        *(undefined2 *)(pbVar6 + 7) = *puVar19;
        *(undefined2 *)(pbVar7 + 7) = *puVar19;
        *(undefined2 *)(pbVar6 + 0xf) = *puVar17;
        *(undefined2 *)(pbVar7 + 0xf) = *puVar17;
        *(undefined2 *)(pbVar6 + 5) = *puVar13;
        *(undefined2 *)(pbVar7 + 5) = *puVar13;
        *(undefined2 *)(pbVar6 + 0xd) = puVar9[-2];
        *(undefined2 *)(pbVar7 + 0xd) = puVar9[-2];
        *(undefined2 *)(pbVar6 + 0x15) = puVar9[-1];
        *(undefined2 *)(pbVar7 + 0x15) = puVar9[-1];
        *(undefined2 *)(pbVar6 + 0x1d) = *puVar9;
        *(undefined2 *)(pbVar7 + 0x1d) = *puVar9;
        if (puVar20[7] == 0) {
          *(undefined4 *)(pbVar6 + -3) = *puVar8;
          *(undefined4 *)(pbVar7 + -3) = *puVar8;
          *pbVar6 = *pbVar6 | 1;
          bVar2 = *pbVar7 | 1;
        }
        else {
          *pbVar6 = *pbVar6 & 0xfe;
          bVar2 = *pbVar7 & 0xfe;
        }
        *pbVar7 = bVar2;
        if (arg8 != 0) {
          *puVar18 = (undefined4)br2_gaddr(puVar11 + 1);
          puVar18 = puVar18 + 1;
          *puVar15 = (undefined4)br2_gaddr(puVar16 + 1);
          puVar15 = puVar15 + 1;
        }
        *puVar11 = 0x9000000;
        *pbVar6 = 0x2c;
        *puVar16 = 0x9000000;
        *pbVar7 = 0x2c;
        if ((*puVar14 & 0x100) == 0) {
          *pbVar6 = *pbVar6 & 0xfd;
          bVar2 = *pbVar7 & 0xfd;
        }
        else {
          *pbVar6 = *pbVar6 | 2;
          bVar2 = *pbVar7 | 2;
        }
        *pbVar7 = bVar2;
        puVar14 = puVar14 + 1;
        puVar8 = puVar8 + 1;
        puVar9 = puVar9 + 4;
        puVar13 = puVar13 + 4;
        puVar19 = puVar19 + 1;
        puVar17 = puVar17 + 1;
        pbVar6 = pbVar6 + 0x28;
        puVar11 = puVar11 + 10;
        pbVar7 = pbVar7 + 0x28;
        iVar4 = iVar4 + 1;
        puVar16 = puVar16 + 10;
      } while (iVar4 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
    }
    iVar12 = *arg5;
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8) ))<< 2;
    break;
  case 6:
    iVar4 = 0;
    do {
      puVar16 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
      puVar13 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x30))));
      puVar19 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x28))));
      puVar17 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2c))));
      puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
      if (iVar4 == 0) {
        puVar8 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
      }
      else {
        puVar8 = (undefined4 *)guest_at((uint32_t)(uintptr_t)((*arg5 + arg8 * 4)));
      }
      iVar12 = 0;
      if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
        pbVar6 = (byte *)((intptr_t)puVar11 + 7);
        puVar9 = puVar13 + 2;
        puVar18 = puVar16 + 2;
        do {
          *(undefined2 *)(pbVar6 + 7) = *puVar19;
          *(undefined2 *)(pbVar6 + 0x13) = *puVar17;
          *(undefined2 *)(pbVar6 + 5) = *puVar13;
          *(undefined2 *)(pbVar6 + 0x11) = puVar9[-1];
          *(undefined2 *)(pbVar6 + 0x1d) = *puVar9;
          if (puVar20[7] == 0) {
            *(undefined4 *)(pbVar6 + -3) = *puVar16;
            *(undefined4 *)(pbVar6 + 9) = puVar18[-1];
            *(undefined4 *)(pbVar6 + 0x15) = *puVar18;
            bVar2 = *pbVar6 | 1;
          }
          else {
            bVar2 = *pbVar6 & 0xfe;
          }
          *pbVar6 = bVar2;
          if (arg8 != 0) {
            *puVar8 = (undefined4)br2_gaddr(puVar11 + 1);
            puVar8[1] = (undefined4)(br2_gaddr(puVar11) + 4);
            puVar8[2] = (undefined4)(br2_gaddr(puVar11) + 7);
            puVar8 = puVar8 + 3;
          }
          *puVar11 = 0x9000000;
          *pbVar6 = 0x34;
          bVar2 = 0x36;
          if ((*puVar14 & 0x100) == 0) {
            bVar2 = 0x34;
          }
          *pbVar6 = bVar2;
          puVar14 = puVar14 + 1;
          puVar18 = puVar18 + 3;
          puVar16 = puVar16 + 3;
          puVar9 = puVar9 + 3;
          puVar13 = puVar13 + 3;
          puVar19 = puVar19 + 1;
          puVar17 = puVar17 + 1;
          pbVar6 = pbVar6 + 0x28;
          iVar12 = iVar12 + 1;
          puVar11 = puVar11 + 10;
        } while (iVar12 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < 2);
LAB_801a83ec:
    *arg5 = *arg5 + *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8) ))* 0xc;
    return iVar3;
  case 7:
    iVar4 = 0;
    do {
      puVar16 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x24))));
      puVar13 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x30))));
      puVar19 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x28))));
      puVar17 = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x2c))));
      puVar14 = (ushort *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a0 + 0x20))));
      if (iVar4 == 0) {
        puVar8 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(*arg5));
      }
      else {
        puVar8 = (undefined4 *)guest_at((uint32_t)(uintptr_t)((*arg5 + arg8 * 4)));
      }
      iVar12 = 0;
      if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8)))) {
        pbVar6 = (byte *)((intptr_t)puVar11 + 7);
        puVar9 = puVar13 + 3;
        puVar18 = puVar16 + 3;
        do {
          *(undefined2 *)(pbVar6 + 7) = *puVar19;
          *(undefined2 *)(pbVar6 + 0x13) = *puVar17;
          *(undefined2 *)(pbVar6 + 5) = *puVar13;
          *(undefined2 *)(pbVar6 + 0x11) = puVar9[-2];
          *(undefined2 *)(pbVar6 + 0x1d) = puVar9[-1];
          *(undefined2 *)(pbVar6 + 0x29) = *puVar9;
          if (puVar20[7] == 0) {
            *(undefined4 *)(pbVar6 + -3) = *puVar16;
            *(undefined4 *)(pbVar6 + 9) = puVar18[-2];
            *(undefined4 *)(pbVar6 + 0x15) = puVar18[-1];
            *(undefined4 *)(pbVar6 + 0x21) = *puVar18;
            bVar2 = *pbVar6 | 1;
          }
          else {
            bVar2 = *pbVar6 & 0xfe;
          }
          *pbVar6 = bVar2;
          if (arg8 != 0) {
            *puVar8 = (undefined4)br2_gaddr(puVar11 + 1);
            puVar8[1] = (undefined4)(br2_gaddr(puVar11) + 4);
            puVar8[2] = (undefined4)(br2_gaddr(puVar11) + 7);
            puVar8[3] = (undefined4)(br2_gaddr(puVar11) + 10);
            puVar8 = puVar8 + 4;
          }
          *puVar11 = 0xc000000;
          *pbVar6 = 0x3c;
          bVar2 = 0x3e;
          if ((*puVar14 & 0x100) == 0) {
            bVar2 = 0x3c;
          }
          *pbVar6 = bVar2;
          puVar14 = puVar14 + 1;
          puVar18 = puVar18 + 4;
          puVar16 = puVar16 + 4;
          puVar9 = puVar9 + 4;
          puVar13 = puVar13 + 4;
          puVar19 = puVar19 + 1;
          puVar17 = puVar17 + 1;
          pbVar6 = pbVar6 + 0x34;
          iVar12 = iVar12 + 1;
          puVar11 = puVar11 + 0xd;
        } while (iVar12 < *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8))));
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < 2);
LAB_801a85d4:
    iVar12 = *arg5;
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((a0 + 8) ))<< 4;
    break;
  default:
    goto switchD_801a776c_default;
  }
  *arg5 = iVar12 + iVar4;
switchD_801a776c_default:
  return iVar3;

}

void obj_flag34_clear(int obj)
{
  int i;
  if ((obj != 0) && (i = 0, 0 < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))))) {
    do {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x10)))) ))+ 0x10) ))+ 0x34) ))= 0;
      i = i + 1;
    } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))));
  }
  return;

}

int tween_seq_alloc(int a0,undefined2 a1,undefined4 a2,undefined4 a3,short arg5, undefined2 arg6,undefined4 arg7,undefined4 arg8)
{
  int node;
  undefined2 *pay;
  node = (int)(intptr_t)task_node_alloc(arg8,2,3,0x801aa4fc,0,0x4c);
  if (node == 0) {
    node = 0;
  }
  else {
    pay = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = (short)a0;
    pay[1] = a1;
    pay[2] = 1;
    pay[3] = arg5 + -1;
    *(undefined4 *)(pay + 0xc) = a3;
    pay[4] = arg6;
    if (a0 == 0) {
      *(undefined4 *)(pay + 0xe) = a2;
    }
    else {
      *(undefined4 *)(pay + 0x10) = a2;
    }
    pay[5] = 0xffff;
    *(undefined4 *)(pay + 10) = 0;
    pay[8] = 0;
    pay[7] = 0;
    pay[0x1c] = 0;
    pay[0x1d] = 0xffff;
    pay[0x22] = 0;
    *(undefined4 *)(pay + 0x1a) = arg7;
  }
  return node;

}

void gte_matrix_base_set(undefined4 mat,undefined4 aux)
{
  int gp_save = (int)guest_gp();
  DAT_801be838 = mat;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x34) ))= aux;
  return;

}

int lazy_node_wrap(undefined4 a0,undefined4 a1,undefined2 a2)
{
  int node;
  int pay;
  node = (int)(uintptr_t)node_lazy_attach(a0,0x801aae08,0,0x2c);
  if (node == 0) {
    node = 0;
  }
  else {
    pay = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10)));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((pay + 0x20) ))= a1;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((pay + 0x24) ))= 0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((pay + 0x28) ))= a2;
  }
  return node;

}

int prim_group_alloc(undefined4 a0,int a1,int a2,int a3,int arg5,int arg6, undefined4 arg7)
{
  undefined2 ff;
  int node;
  int off;
  undefined4 *src;
  undefined4 *dst;
  uint i;
  int grp;
  undefined4 w1;
  int m;
  int pad;
  m = a3;
  if ((a3 == 0) && (arg5 != 0)) {
    m = 1;
  }
  pad = (uint)(arg6 != 0x1000) * 0x20;
  node = (int)(intptr_t)task_node_alloc(arg7,2,1,0x801ac154,0,
                       a2 * 4 + m * 0x18 + 0x4c + *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x2c) ))* 8 +
                       *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x34) ))* 8 + *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x30) ))* 8 + pad);
  if (node == 0) {
    grp = 0;
  }
  else {
    grp = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10)));
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x14) ))= node;
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x40) ))= a2;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x18) ))= 0;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x28) ))= 0;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= 0;
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x44) ))= a3;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x48) ))= 0;
    if ((arg6 != 0x1000) == 0) {
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))= 0;
    }
    else {
      /* guest @0x801AC41C-0x801AC430: num=0x01000000, div arg6;
       * break @0x801AC42C is a no-op -> dv==0 yields LO=-1 (num>=0),
       * stored truncated to u16. C /0 would SIGFPE — guard faithfully. */
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))= grp + 0x4c;
      ff = (undefined2)(arg6 == 0 ? (int32_t)0xffffffff
                                      : 0x1000000 / arg6);
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((grp + 0x4c) ))= ff;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((grp + 0x54) ))= ff;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x10) ))= ff;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 2) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 4) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 6) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 10) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0xc) ))= 0;
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0xe) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x14) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x18) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x1c) ))= 0;
    }
    pad = grp + 0x4c + pad;
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x10) ))= pad;
    node = 0;
    if (0 < a2) {
      off = 0;
      do {
        node = node + 1;
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x10)))) ))= 0;
        off = node * 4;
      } while (node < a2);
    }
    pad = pad + a2 * 4;
    if (*(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x2c) ))== 0) {
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x28) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((a1 + 0x90)));
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x28) ))= pad;
      i = 0;
      node = 0;
      if (*(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x2c) ))!= 0) {
        node = 0;
        do {
          dst = (undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x28))))));
          src = (undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x90))))));
          w1 = src[1];
          *dst = *src;
          dst[1] = w1;
          i = i + 1;
          node = i * 8;
        } while (i < *(uint *)guest_at((uint32_t)(uintptr_t)((a1 + 0x2c))));
        node = *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x2c)));
      }
      pad = pad + node * 8;
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x34) ))== 0) {
      if (*(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x10) ))== 1) {
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((a1 + 0x94)));
      }
      else {
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= 0;
      }
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= pad;
      i = 0;
      node = 0;
      if (*(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x34) ))!= 0) {
        node = 0;
        do {
          dst = (undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c))))));
          src = (undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x94))))));
          w1 = src[1];
          *dst = *src;
          dst[1] = w1;
          i = i + 1;
          node = i * 8;
        } while (i < *(uint *)guest_at((uint32_t)(uintptr_t)((a1 + 0x34))));
        node = *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x34)));
      }
      pad = pad + node * 8;
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x30) ))== 0) {
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x30) ))= 0;
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x30) ))= pad;
      pad = pad + *(int *)guest_at((uint32_t)(uintptr_t)((a1 + 0x30) ))* 8;
    }
    node = m * 4;
    if (m == 0) {
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x20) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x24) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x34) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x38) ))= 0;
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c) ))= 0;
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c) ))= pad;
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x20) ))= pad + node;
      off = pad + node + node;
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24) ))= off;
      off = off + node;
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x34) ))= off;
      off = off + node;
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x38) ))= off;
      pad = 0;
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c) ))= off + node;
      if (0 < m) {
        do {
          node = pad * 4;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c)))) ))= 0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x20)))) ))= 0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))) ))= 0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x34)))) ))= 0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x38)))) ))= 0;
          pad = pad + 1;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c)))) ))= 0;
        } while (pad < m);
      }
    }
  }
  return grp;

}

void node_param_quad_set(int node,undefined4 q0,undefined4 q1,undefined4 q2, undefined4 q3)
{
  int datap;
  datap = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10) ))+ 8)));
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 4) ))= q0;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 8) ))= q1;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 0xc) ))= q2;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)((datap + 0x10) ))= q3;
  return;

}

int * prim_list_gte_invoke(int *a0,undefined4 a1,undefined4 a2,int a3,undefined4 arg5 ,undefined4 arg6)
{
  undefined4 in_zero;
  undefined4 extraout_at;
  undefined4 t0;
  undefined4 t1;
  undefined4 t2;
  undefined4 unaff_s0;
  undefined4 unaff_s1;
  undefined4 unaff_s2;
  int *rp;
  undefined4 unaff_s3;
  undefined4 unaff_s4;
  undefined4 unaff_s5;
  undefined4 unaff_s6;
  int n;
  undefined4 unaff_retaddr;
  if (0 < a3) {
    setCopControlWord(2,0xe800,0x555);
    setCopControlWord(2,0xf000,0x400);
    _DAT_1f8003e0 = unaff_s6;
    _DAT_1f8003e4 = unaff_s5;
    _DAT_1f8003e8 = unaff_s4;
    _DAT_1f8003ec = unaff_s3;
    _DAT_1f8003f0 = unaff_s2;
    _DAT_1f8003f4 = unaff_s1;
    _DAT_1f8003f8 = unaff_s0;
    _DAT_1f8003fc = unaff_retaddr;
    do {
      if (((short)a0[3] == 0) && (*a0 != 0)) {
        gte_xform3_ip((undefined4 *)guest_at((uint32_t)arg6),(undefined2 *)guest_at((uint32_t)(*a0 + 0x20)));
        gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)arg5),(undefined2 *)guest_at((uint32_t)(*a0 + 0x20)));
        setCopReg(2,in_zero,a0[1]);
        setCopReg(2,extraout_at,a0[2]);
        copFunction(2,0x480012);
        n = *(int *)guest_at((uint32_t)(uintptr_t)(a0[7]));
        t0 = getCopReg(2,0xc800);
        rp = (int *)guest_at((uint32_t)(uintptr_t)(a0[7] ))+ 1;
        if (n != 0) {
          t1 = getCopReg(2,0xd000);
          t2 = getCopReg(2,0xd800);
          setCopControlWord(2,0x2800,t0);
          setCopControlWord(2,0x3000,t1);
          setCopControlWord(2,0x3800,t2);
          /* Generated 0x801871BC jalr's each record fn with the full a0-a3
           * set: a0 = record[2] (data cursor), a1 = a1 + lh(a0
           * +0xE), a2 = a2, a3 = record[1] (per-record field).
           * task_invoke/call_guest only program $a0/$a1 — $a1 arrived as 0
           * and $a2/$a3 stale, so the GTE emit leaf 0x801876BC used a1=0 as
           * its OT/link-table base and SWL'd into BIOS kernel RAM
           * (phys 0x0000-0x1FFC). call_guest4 reproduces the guest ABI
           * exactly (same fix as prim_task_run). */
          {
            uint32_t prim_a1 = (uint32_t)a1
                + (uint32_t)(int)*(int16_t *)((char *)a0 + 0xe);
            do {
              n = n + -1;
              call_guest4((uint32_t)rp[3],(uint32_t)rp[2],prim_a1,
                          (uint32_t)a2,(uint32_t)rp[1]);
              rp = rp + 4;
            } while (0 < n);
          }
        }
      }
      a3 = a3 + -1;
      a0 = a0 + 10;
    } while (0 < a3);
  }
  return a0;

}

/* ---- batch17: announcer/round + vector math ---- */
undefined4 text_render_full(uint *a0,uint a1,int a2)
{
  byte bVar1;
  undefined2 uVar2;
  char cVar3;
  char cVar4;
  char cVar5;
  short sVar6;
  short sVar7;
  char cVar8;
  short sVar9;
  char cVar10;
  ushort uVar11;
  uint uVar12;
  short *psVar13;
  char cVar14;
  byte bVar15;
  short sVar16;
  ushort uVar17;
  short sVar18;
  uint *puVar19;
  ushort uVar20;
  int iVar21;
  byte *pbVar22;
  byte *pbVar23;
  ushort uVar24;
  cVar5 = DAT_1f800015;
  a1 = a1 & 0xffff;
  if (_DAT_1f800388 != a1) {
    _DAT_1f800380 = 0;
    _DAT_1f800388 = a1;
  }
  if (_DAT_1f800380 < 400) {
    puVar19 = (uint *)guest_at((uint32_t)(uintptr_t)((a1 * 16000 + _DAT_1f800380 * 0x28 + -0x7fe17ad8)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 6) ))< 0x1000) {
      _DAT_1f80037c = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 6)));
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 8) ))< 0x1000) {
      _DAT_1f80037e = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 8)));
    }
    uVar17 = *(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 10)));
    uVar2 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a2 + 0x10)));
    if (((*(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x18) ))< 6) && (3 < *(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x18))))) ||
       (DAT_1f800015 == '\0')) {
      sVar18 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0xc)));
    }
    else {
      sVar18 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0xc) ))<< 1;
    }
    /* +0x1c cell stores a GUEST text pointer — resolve via guest_at before
     * dereferencing (raw guest addr faults as a host pointer). */
    pbVar22 = (byte *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a2 + 0x1c))));
    if (*pbVar22 != 0) {
      iVar21 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0xe)));
      cVar4 = (char)*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0xe)));
      uVar20 = (ushort)DAT_8011ede0;
      uVar24 = (ushort)DAT_8011ef64;
      psVar13 = (short *)((intptr_t)puVar19 + 0x22);
      uVar11 = (ushort)DAT_8011ed80;
      do {
        psVar13[-10] = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x16)));
        uVar12 = (uint)*pbVar22;
        pbVar23 = pbVar22 + 1;
        sVar16 = _DAT_1f80037e + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 4)));
        sVar7 = _DAT_1f80037c + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 2)));
        if (uVar12 != 0x20) {
          if (uVar12 < 0x21) {
            if (uVar12 == 10) {
LAB_8012d3f4:
              _DAT_1f80037c = 0;
              _DAT_1f80037e = _DAT_1f80037e + sVar18 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x14)));
              goto LAB_8012dca4;
            }
          }
          else if (uVar12 == 0x23) {
            bVar15 = *pbVar23;
            uVar12 = (uint)bVar15;
            pbVar23 = pbVar22 + 2;
            if (uVar12 != 0x23) {
              if (uVar12 != 0x6e) {
                *(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x16) ))=
                     (*(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x16) ))& 0xfc3f) + (bVar15 - 0x30 & 0x1ff) * 0x40;
                goto LAB_8012dca4;
              }
              goto LAB_8012d3f4;
            }
          }
          cVar3 = (char)uVar2;
          switch(*(undefined2 *)guest_at((uint32_t)(uintptr_t)((a2 + 0x18)))) {
          default:
            psVar13[-6] = 0x2f;
            uVar12 = uVar12 - 0x20;
            cVar14 = ((byte)uVar12 & 0xf) * '\b';
            cVar10 = (char)((uVar12 & 0xf0) >> 1);
            if (iVar21 < 0) {
              bVar15 = (&DAT_8011ed80)[uVar12 & 0xff];
              uVar17 = (ushort)bVar15;
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
              cVar14 = bVar15 + cVar14;
              *(char *)(psVar13 + -7) = cVar14;
              *(char *)(psVar13 + 1) = cVar14;
            }
            else {
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)(psVar13 + -7) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)(psVar13 + 1) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
            }
            psVar13[-9] = sVar7 + uVar17;
            psVar13[-0xd] = sVar7;
            psVar13[-0xc] = sVar16;
            psVar13[-8] = sVar16;
            psVar13[-5] = sVar7;
            sVar6 = sVar16;
            if (cVar5 != '\0') {
              sVar6 = sVar16 + -1;
            }
            psVar13[-4] = sVar6 + sVar18;
            psVar13[-1] = sVar7 + uVar17;
            if (cVar5 == '\0') {
              sVar6 = sVar16 + sVar18;
            }
            else {
              sVar6 = sVar16 + -1 + sVar18;
            }
            break;
          case 1:
            psVar13[-6] = 0x2f;
            bVar15 = (byte)(uVar12 - 0x20);
            cVar14 = (bVar15 & 0xf) * '\b';
            cVar10 = (bVar15 & 0xf0) + 0x30;
            if (iVar21 < 0) {
              bVar15 = (&DAT_8011ede0)[uVar12 - 0x20 & 0xff];
              uVar17 = (ushort)bVar15;
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
              cVar14 = bVar15 + cVar14;
              *(char *)(psVar13 + -7) = cVar14;
              *(char *)(psVar13 + 1) = cVar14;
            }
            else {
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)(psVar13 + -7) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)(psVar13 + 1) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
            }
            psVar13[-9] = sVar7 + uVar17;
            psVar13[-0xd] = sVar7;
            psVar13[-0xc] = sVar16;
            psVar13[-8] = sVar16;
            psVar13[-5] = sVar7;
            sVar6 = sVar16;
            if (cVar5 != '\0') {
              sVar6 = sVar16 + -1;
            }
            psVar13[-4] = sVar6 + sVar18;
            psVar13[-1] = sVar7 + uVar17;
            if (cVar5 == '\0') {
              sVar6 = sVar16 + sVar18;
            }
            else {
              sVar6 = sVar16 + -1 + sVar18;
            }
            break;
          case 2:
            psVar13[-6] = 0x2f;
            switch(uVar12) {
            case 0x21:
              uVar12 = 0x1b;
              break;
            case 0x22:
              uVar12 = 0x24;
              break;
            default:
              if ((uVar12 - 0x30 & 0xff) < 10) {
                uVar12 = uVar12 - 10;
              }
              else if (uVar12 < 0x61) {
                uVar12 = uVar12 - 0x41;
              }
              else {
                uVar12 = uVar12 - 0x61;
              }
              break;
            case 0x26:
              uVar12 = 0x1d;
              break;
            case 0x27:
              bVar15 = 0xc0;
              cVar14 = '`';
              uVar12 = 0x30;
              goto LAB_8012d724;
            case 0x2a:
              uVar12 = 0x1f;
              break;
            case 0x2d:
              uVar12 = 0x1e;
              break;
            case 0x2e:
              uVar12 = 0x1a;
              break;
            case 0x3c:
              uVar12 = 0x20;
              break;
            case 0x3e:
              uVar12 = 0x22;
              break;
            case 0x3f:
              uVar12 = 0x1c;
              break;
            case 0x5f:
              uVar12 = 0x23;
              break;
            case 0x7e:
              uVar12 = 0x25;
              break;
            case 0x7f:
              uVar12 = 0x21;
            }
            bVar15 = (byte)((uVar12 & 7) << 4) | 0x80;
            cVar14 = (char)((uVar12 & 0xff) >> 3) * '\x18';
LAB_8012d724:
            cVar10 = bVar15 + cVar4 + -1;
            if (iVar21 < 0) {
              bVar1 = (&DAT_8011ee40)[uVar12 & 0xff];
              uVar17 = (ushort)bVar1;
              *(byte *)(psVar13 + -0xb) = bVar15;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar14;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar14;
              *(byte *)(psVar13 + -3) = bVar15;
              *(char *)((intptr_t)psVar13 + -5) = cVar14 + cVar3;
              *(char *)((intptr_t)psVar13 + 3) = cVar14 + cVar3;
              psVar13[-0xd] = sVar7;
              psVar13[-0xc] = sVar16;
              psVar13[-8] = sVar16;
              psVar13[-5] = sVar7;
              *(byte *)(psVar13 + -7) = bVar1 + bVar15;
              *(byte *)(psVar13 + 1) = bVar1 + bVar15;
              psVar13[-9] = sVar7 + (ushort)bVar1;
              sVar6 = sVar16;
              if (cVar5 != '\0') {
                sVar6 = sVar16 + -1;
              }
              psVar13[-4] = sVar6 + sVar18;
              psVar13[-1] = sVar7 + (ushort)bVar1;
              if (cVar5 == '\0') {
                sVar6 = sVar16 + sVar18;
              }
              else {
                sVar6 = sVar16 + -1 + sVar18;
              }
            }
            else {
              *(byte *)(psVar13 + -0xb) = bVar15;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar14;
              *(char *)(psVar13 + -7) = cVar10;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar14;
              *(byte *)(psVar13 + -3) = bVar15;
              *(char *)((intptr_t)psVar13 + -5) = cVar14 + cVar3;
              *(char *)(psVar13 + 1) = cVar10;
              *(char *)((intptr_t)psVar13 + 3) = cVar14 + cVar3;
              psVar13[-0xd] = sVar7;
              psVar13[-0xc] = sVar16;
              sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 10)));
              psVar13[-8] = sVar16;
              psVar13[-5] = sVar7;
              psVar13[-9] = sVar6 + sVar7 + -1;
              sVar6 = sVar16;
              if (cVar5 != '\0') {
                sVar6 = sVar16 + -1;
              }
              psVar13[-4] = sVar6 + sVar18;
              psVar13[-1] = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 10) ))+ sVar7 + -1;
              if (cVar5 == '\0') {
                sVar6 = sVar16 + sVar18;
              }
              else {
                sVar6 = sVar16 + -1 + sVar18;
              }
            }
            break;
          case 3:
            psVar13[-6] = 0x2e;
            switch(uVar12) {
            case 0x21:
              uVar12 = 0x1b;
              break;
            default:
              if ((uVar12 - 0x30 & 0xff) < 10) {
                uVar12 = uVar12 - 10;
              }
              else if (uVar12 < 0x61) {
                uVar12 = uVar12 - 0x41;
              }
              else {
                uVar12 = uVar12 - 0x61;
              }
              break;
            case 0x26:
              uVar12 = 0x1d;
              break;
            case 0x2a:
              uVar12 = 0x1f;
              break;
            case 0x2d:
              uVar12 = 0x1e;
              break;
            case 0x2e:
              uVar12 = 0x1a;
              break;
            case 0x3c:
              uVar12 = 0x20;
              break;
            case 0x3e:
              uVar12 = 0x22;
              break;
            case 0x3f:
              uVar12 = 0x1c;
              break;
            case 0x5f:
              uVar12 = 0x23;
              break;
            case 0x7e:
              uVar12 = 0x25;
              break;
            case 0x7f:
              uVar12 = 0x21;
            }
            uVar12 = uVar12 & 0xff;
            cVar14 = (&DAT_8011eea4)[uVar12];
            cVar10 = (&DAT_8011eed4)[uVar12];
            if (iVar21 < 0) {
              bVar15 = (&DAT_8011ee74)[uVar12];
              uVar17 = (ushort)bVar15;
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
              psVar13[-0xd] = sVar7;
              psVar13[-0xc] = sVar16;
              psVar13[-8] = sVar16;
              psVar13[-5] = sVar7;
              *(byte *)(psVar13 + -7) = bVar15 + cVar14;
              *(byte *)(psVar13 + 1) = bVar15 + cVar14;
              psVar13[-9] = sVar7 + (ushort)bVar15;
              sVar6 = sVar16;
              if (cVar5 != '\0') {
                sVar6 = sVar16 + -1;
              }
              psVar13[-4] = sVar6 + sVar18;
              psVar13[-1] = sVar7 + (ushort)bVar15;
              if (cVar5 == '\0') {
                sVar6 = sVar16 + sVar18;
              }
              else {
                sVar6 = sVar16 + -1 + sVar18;
              }
            }
            else {
              cVar8 = cVar14 + cVar4 + -1;
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)(psVar13 + -7) = cVar8;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)(psVar13 + 1) = cVar8;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
              psVar13[-0xd] = sVar7;
              psVar13[-0xc] = sVar16;
              sVar6 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 10)));
              psVar13[-8] = sVar16;
              psVar13[-5] = sVar7;
              psVar13[-9] = sVar6 + sVar7 + -1;
              sVar6 = sVar16;
              if (cVar5 != '\0') {
                sVar6 = sVar16 + -1;
              }
              psVar13[-4] = sVar6 + sVar18;
              psVar13[-1] = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 10) ))+ sVar7 + -1;
              if (cVar5 == '\0') {
                sVar6 = sVar16 + sVar18;
              }
              else {
                sVar6 = sVar16 + -1 + sVar18;
              }
            }
            break;
          case 4:
            if (0x5f < uVar12) {
              uVar17 = 0xf;
              psVar13[-6] = 0x5e;
              cVar3 = (char)uVar12 * '\x10';
              bVar15 = (char)uVar12 + 0xa0U & 0xf0;
              cVar10 = bVar15 - 0x28;
              cVar14 = bVar15 - 0x18;
              *(char *)(psVar13 + -0xb) = cVar3;
              *(char *)(psVar13 + -3) = cVar3;
              sVar9 = sVar7 + 0x10;
              *(char *)((intptr_t)psVar13 + -5) = cVar14;
              *(char *)((intptr_t)psVar13 + 3) = cVar14;
              sVar6 = sVar16 + 0x10;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)(psVar13 + -7) = cVar3 + '\x10';
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + 1) = cVar3 + '\x10';
              goto LAB_8012dc2c;
            }
            psVar13[-6] = 0x5d;
            uVar12 = uVar12 - 0x20;
            cVar14 = (byte)uVar12 * '\x10';
            cVar10 = ((byte)uVar12 & 0xf0) - 0x40;
            if (iVar21 < 0) {
              bVar15 = (&DAT_8011ef64)[uVar12 & 0xff];
              uVar17 = (ushort)bVar15;
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
              cVar14 = bVar15 + cVar14;
              *(char *)(psVar13 + -7) = cVar14;
              *(char *)(psVar13 + 1) = cVar14;
            }
            else {
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)(psVar13 + -7) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)(psVar13 + 1) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
            }
            if ((iVar21 < 0) || ((uVar12 & 0xff) != 0x11)) goto LAB_8012dc28;
            uVar12 = (uint)DAT_8011ef75;
            psVar13[-0xc] = sVar16;
            psVar13[-8] = sVar16;
            psVar13[-4] = sVar16 + sVar18;
            *psVar13 = sVar16 + sVar18;
            sVar7 = sVar7 + (short)((int)(iVar21 - uVar12) / 2);
            psVar13[-0xd] = sVar7;
            psVar13[-9] = uVar17 + sVar7;
            psVar13[-5] = sVar7;
            psVar13[-1] = uVar17 + sVar7;
            goto LAB_8012dc4c;
          case 5:
            psVar13[-6] = 0x3e;
            uVar12 = uVar12 - 0x20;
            cVar14 = (char)uVar12 * '\b';
            cVar10 = (char)((uVar12 & 0xe0) >> 2) + -0x40;
            if (iVar21 < 0) {
              bVar15 = (&DAT_8011ef04)[uVar12 & 0xff];
              uVar17 = (ushort)bVar15;
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
              cVar14 = bVar15 + cVar14;
              *(char *)(psVar13 + -7) = cVar14;
              *(char *)(psVar13 + 1) = cVar14;
            }
            else {
              *(char *)(psVar13 + -0xb) = cVar14;
              *(char *)((intptr_t)psVar13 + -0x15) = cVar10;
              *(char *)(psVar13 + -7) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + -0xd) = cVar10;
              *(char *)(psVar13 + -3) = cVar14;
              *(char *)((intptr_t)psVar13 + -5) = cVar10 + cVar3;
              *(char *)(psVar13 + 1) = cVar14 + cVar4;
              *(char *)((intptr_t)psVar13 + 3) = cVar10 + cVar3;
            }
LAB_8012dc28:
            sVar9 = sVar7 + uVar17;
            sVar6 = sVar16 + sVar18;
LAB_8012dc2c:
            psVar13[-0xd] = sVar7;
            psVar13[-0xc] = sVar16;
            psVar13[-9] = sVar9;
            psVar13[-8] = sVar16;
            psVar13[-5] = sVar7;
            psVar13[-4] = sVar6;
            psVar13[-1] = sVar9;
          }
          *psVar13 = sVar6;
LAB_8012dc4c:
          psVar13 = psVar13 + 0x14;
          *puVar19 = *a0 | 0x9000000;
          *a0 = guest_of(puVar19) & 0xffffff;
          _DAT_1f800380 = _DAT_1f800380 + 1;
          puVar19 = puVar19 + 10;
          if (399 < _DAT_1f800380) {
            return 0xffffffff;
          }
          sVar7 = uVar17 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
          goto LAB_8012dc9c;
        }
        if (-1 < iVar21) {
          _DAT_1f80037c = _DAT_1f80037c + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 10) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
          goto LAB_8012dca4;
        }
        switch(*(undefined2 *)guest_at((uint32_t)(uintptr_t)((a2 + 0x18)))) {
        default:
          sVar7 = uVar11 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
          break;
        case 1:
          sVar7 = uVar20 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
          break;
        case 2:
          bVar15 = DAT_8011ee5b;
          goto LAB_8012d390;
        case 3:
          bVar15 = DAT_8011ee8f;
LAB_8012d390:
          _DAT_1f80037c = _DAT_1f80037c + (ushort)bVar15 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
          goto LAB_8012dca4;
        case 4:
          sVar7 = uVar24 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
        }
LAB_8012dc9c:
        _DAT_1f80037c = _DAT_1f80037c + sVar7;
LAB_8012dca4:
        pbVar22 = pbVar23;
      } while (*pbVar23 != 0);
    }
  }
  return 0;

}

undefined4 text_render_simple(uint *a0,uint a1,int a2)
{
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  short sVar4;
  char cVar5;
  char cVar6;
  char cVar7;
  short sVar8;
  char cVar9;
  short *psVar10;
  char cVar11;
  uint uVar12;
  short sVar13;
  byte *pbVar14;
  byte *pbVar15;
  ushort uVar16;
  uint *puVar17;
  ushort uVar18;
  short sVar19;
  cVar5 = DAT_1f800015;
  a1 = a1 & 0xffff;
  if (_DAT_1f800388 != a1) {
    _DAT_1f800380 = 0;
    _DAT_1f800388 = a1;
  }
  *(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x18) ))= *(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x18) ))& 3;
  if (_DAT_1f800380 < 400) {
    puVar17 = (uint *)guest_at((uint32_t)(uintptr_t)((a1 * 16000 + _DAT_1f800380 * 0x28 + -0x7fe17ad8)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 6) ))< 0x1000) {
      _DAT_1f80037c = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 6)));
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 8) ))< 0x1000) {
      _DAT_1f80037e = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 8)));
    }
    uVar16 = *(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 10)));
    sVar2 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0xe)));
    uVar3 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a2 + 0x10)));
    if (cVar5 == '\0') {
      sVar19 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0xc)));
    }
    else {
      sVar19 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0xc) ))<< 1;
    }
    /* The +0x1c cell stores a GUEST pointer (guest_of(text buf)); resolve it
     * through guest_at before dereferencing or the host faults on the raw
     * guest address. */
    pbVar14 = (byte *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a2 + 0x1c))));
    if (*pbVar14 != 0) {
      uVar18 = (ushort)DAT_8011ed80;
      psVar10 = (short *)((intptr_t)puVar17 + 0x22);
      do {
        psVar10[-10] = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x16)));
        uVar12 = (uint)*pbVar14;
        pbVar15 = pbVar14 + 1;
        sVar8 = _DAT_1f80037e + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 4)));
        sVar13 = _DAT_1f80037c + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 2)));
        if (uVar12 == 0x20) {
          if (sVar2 < 0) {
            sVar8 = uVar18 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
          }
          else {
            sVar8 = *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 10) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
          }
          _DAT_1f80037c = _DAT_1f80037c + sVar8;
        }
        else {
          if (uVar12 < 0x21) {
            if (uVar12 == 10) {
LAB_8012dec4:
              _DAT_1f80037c = 0;
              _DAT_1f80037e = _DAT_1f80037e + sVar19 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x14)));
              goto LAB_8012e044;
            }
            psVar10[-6] = 0x2f;
          }
          else if (uVar12 == 0x23) {
            bVar1 = *pbVar15;
            pbVar15 = pbVar14 + 2;
            if (bVar1 != 0x23) {
              if (bVar1 == 0x6e) goto LAB_8012dec4;
              *(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x16) ))=
                   (*(ushort *)guest_at((uint32_t)(uintptr_t)((a2 + 0x16) ))& 0xfc3f) + (bVar1 - 0x30 & 0x1ff) * 0x40;
              goto LAB_8012e044;
            }
            psVar10[-6] = 0x2f;
            uVar12 = 0x23;
          }
          else {
            psVar10[-6] = 0x2f;
          }
          uVar12 = uVar12 - 0x20;
          cVar11 = ((byte)uVar12 & 0xf) * '\b';
          cVar9 = (char)((uVar12 & 0xf0) >> 1);
          cVar6 = (char)uVar3;
          if (sVar2 < 0) {
            bVar1 = (&DAT_8011ed80)[uVar12 & 0xff];
            uVar16 = (ushort)bVar1;
            cVar6 = cVar9 + cVar6;
            *(char *)(psVar10 + -0xb) = cVar11;
            *(char *)((intptr_t)psVar10 + -0x15) = cVar9;
            *(char *)((intptr_t)psVar10 + -0xd) = cVar9;
            *(char *)(psVar10 + -3) = cVar11;
            *(char *)((intptr_t)psVar10 + -5) = cVar6;
            *(char *)((intptr_t)psVar10 + 3) = cVar6;
            cVar11 = bVar1 + cVar11;
            *(char *)(psVar10 + -7) = cVar11;
            *(char *)(psVar10 + 1) = cVar11;
          }
          else {
            cVar7 = (char)sVar2 + cVar11;
            cVar6 = cVar9 + cVar6;
            *(char *)(psVar10 + -0xb) = cVar11;
            *(char *)((intptr_t)psVar10 + -0x15) = cVar9;
            *(char *)(psVar10 + -7) = cVar7;
            *(char *)((intptr_t)psVar10 + -0xd) = cVar9;
            *(char *)(psVar10 + -3) = cVar11;
            *(char *)((intptr_t)psVar10 + -5) = cVar6;
            *(char *)(psVar10 + 1) = cVar7;
            *(char *)((intptr_t)psVar10 + 3) = cVar6;
          }
          psVar10[-9] = sVar13 + uVar16;
          psVar10[-0xd] = sVar13;
          psVar10[-0xc] = sVar8;
          psVar10[-8] = sVar8;
          psVar10[-5] = sVar13;
          sVar4 = sVar8;
          if (cVar5 != '\0') {
            sVar4 = sVar8 + -1;
          }
          psVar10[-4] = sVar4 + sVar19;
          psVar10[-1] = sVar13 + uVar16;
          if (cVar5 != '\0') {
            sVar8 = sVar8 + -1;
          }
          *psVar10 = sVar8 + sVar19;
          psVar10 = psVar10 + 0x14;
          *puVar17 = *a0 | 0x9000000;
          *a0 = guest_of(puVar17) & 0xffffff;
          _DAT_1f800380 = _DAT_1f800380 + 1;
          puVar17 = puVar17 + 10;
          if (399 < _DAT_1f800380) {
            return 0xffffffff;
          }
          _DAT_1f80037c = _DAT_1f80037c + uVar16 + *(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x12)));
        }
LAB_8012e044:
        pbVar14 = pbVar15;
      } while (*pbVar15 != 0);
    }
  }
  return 0;

}

void particle_burst_spawn(int type,int idx)
{
  short tv;
  int r;
  int step;
  int cnt;
  undefined2 *p;
  int rad;
  int i;
  undefined2 *rm;
  rm = (undefined2 *)&DAT_801f0300;
  _DAT_801f04a0 = 9;
  idx = idx * 8;
  if (type == 1) {
    rad = (int)*(short *)(&DAT_801b152a + idx);
    cnt = (int)_DAT_801f04bc;
    step = rad << 1;
    i = 0;
    if (0 < cnt) {
      p = (undefined2 *)&DAT_801f0328;
      do {
        p[3] = 0xfc00;
        p[2] = 0x400;
        r = table_byte_next_a();
        r = r * -0x80;
        if (r < 0) {
          r = r + 0xff;
        }
        p[4] = (short)((uint)r >> 8);
        rotmat_rtps_build(0xfffffc00,rm + 2);
        rad = rad - step / (cnt + 1);
        tv = *(short *)(&DAT_801b1528 + idx);
        *(undefined4 *)(p + -6) = 0xffffff00;
        *(int *)(p + -4) = rad;
        *(int *)(p + -8) = -(int)tv;
        r = table_byte_next_a();
        r = (r + 0xc0) * -0x33;
        if (r < 0) {
          r = r + 0xff;
        }
        p[-2] = (short)((uint)r >> 8);
        r = table_byte_next_a();
        p[-1] = (short)((r + -0xa0) >> 2);
        r = rad;
        if (rad < 0) {
          r = rad + 0x3f;
        }
        i = i + 1;
        *p = (short)(r >> 6);
        p = p + 0x1a;
        *rm = 0x10;
        rm = rm + 0x1a;
      } while (i < _DAT_801f04bc);
    }
  }
  else if (type < 2) {
    if (type == 0) {
      rad = (int)*(short *)(&DAT_801b152a + idx);
      cnt = (int)_DAT_801f04bc;
      step = rad << 1;
      i = 0;
      if (0 < cnt) {
        p = (undefined2 *)&DAT_801f0328;
        do {
          p[3] = 0x400;
          p[2] = 0x400;
          r = table_byte_next_a();
          r = r * -0x80;
          if (r < 0) {
            r = r + 0xff;
          }
          p[4] = (short)((uint)r >> 8);
          rotmat_rtps_build(0x400,rm + 2);
          rad = rad - step / (cnt + 1);
          tv = *(short *)(&DAT_801b1528 + idx);
          *(undefined4 *)(p + -6) = 0xffffff00;
          *(int *)(p + -4) = rad;
          *(int *)(p + -8) = (int)tv;
          r = table_byte_next_a();
          r = (r + 0xc0) * 0x33;
          if (r < 0) {
            r = r + 0xff;
          }
          p[-2] = (short)((uint)r >> 8);
          r = table_byte_next_a();
          p[-1] = (short)((r + -0xa0) >> 2);
          r = rad;
          if (rad < 0) {
            r = rad + 0x3f;
          }
          i = i + 1;
          *p = (short)(r >> 6);
          p = p + 0x1a;
          *rm = 0x10;
          rm = rm + 0x1a;
        } while (i < _DAT_801f04bc);
      }
    }
  }
  else if (type == 2) {
    rad = (int)*(short *)(&DAT_801b1528 + idx);
    cnt = (int)_DAT_801f04bc;
    step = rad << 1;
    i = 0;
    if (0 < cnt) {
      p = (undefined2 *)&DAT_801f0328;
      do {
        p[3] = 0;
        p[2] = 0x400;
        r = table_byte_next_a();
        r = r * -0x80;
        if (r < 0) {
          r = r + 0xff;
        }
        rad = rad - step / (cnt + 1);
        p[4] = (short)((uint)r >> 8);
        *(undefined4 *)(p + -0x12) = 0x1000;
        *(undefined4 *)(p + -0x10) = 0;
        *(undefined4 *)(p + -0xe) = 0x1000;
        *(undefined4 *)(p + -0xc) = 0;
        *(undefined4 *)(p + -10) = 0x1000;
        *(int *)(p + -8) = rad;
        *(undefined4 *)(p + -6) = 0xffffff00;
        *(int *)(p + -4) = (int)*(short *)(&DAT_801b152a + idx);
        r = rad;
        if (rad < 0) {
          r = rad + 0x3f;
        }
        p[-2] = (short)(r >> 6);
        r = table_byte_next_a();
        p[-1] = (short)((r + -0xa0) >> 2);
        r = table_byte_next_a();
        r = (r + 0xc0) * 0x33;
        if (r < 0) {
          r = r + 0xff;
        }
        i = i + 1;
        *p = (short)((uint)r >> 8);
        p = p + 0x1a;
        *rm = 0x10;
        rm = rm + 0x1a;
      } while (i < _DAT_801f04bc);
    }
  }
  else if (type == 3) {
    rad = (int)*(short *)(&DAT_801b1528 + idx);
    cnt = (int)_DAT_801f04bc;
    step = rad << 1;
    i = 0;
    if (0 < cnt) {
      p = (undefined2 *)&DAT_801f0328;
      do {
        p[3] = 0x800;
        p[2] = 0x400;
        r = table_byte_next_a();
        r = r * -0x80;
        if (r < 0) {
          r = r + 0xff;
        }
        p[4] = (short)((uint)r >> 8);
        rotmat_rtps_build(0x800,rm + 2);
        rad = rad - step / (cnt + 1);
        *(int *)(p + -8) = rad;
        *(undefined4 *)(p + -6) = 0xffffff00;
        *(int *)(p + -4) = -(int)*(short *)(&DAT_801b152a + idx);
        r = rad;
        if (rad < 0) {
          r = rad + 0x3f;
        }
        p[-2] = (short)(r >> 6);
        r = table_byte_next_a();
        p[-1] = (short)((r + -0xa0) >> 2);
        r = table_byte_next_a();
        r = (r + 0xc0) * -0x33;
        if (r < 0) {
          r = r + 0xff;
        }
        i = i + 1;
        *p = (short)((uint)r >> 8);
        p = p + 0x1a;
        *rm = 0x10;
        rm = rm + 0x1a;
      } while (i < _DAT_801f04bc);
    }
  }
  return;

}

void particle_burst_update(int a0,undefined4 a1,undefined4 a2,int a3)
{
  short vx;
  short vy;
  short *bnd;
  int t;
  undefined1 *pr;
  short *slot;
  int k;
  /* Guest GTE output slot: gte_rtps_xform3_t writes a2[0..7] as
   * 8 contiguous u32; the decompiler split them into scalars.
   * xf[0..7] = old xf/local_44/local_40/local_3c/
   * local_38/local_34/local_30/local_2c. */
  undefined4 xf [8];
  if (_DAT_801f04a0 == 1) {
    deferred_cmd_post(0x400,0x80,0);
LAB_8013663c:
    deferred_cmd_post(0x400,0x82,0);
  }
  else if (_DAT_801f04a0 < 2) {
    if (_DAT_801f04a0 == 0) goto LAB_80136660;
  }
  else if ((_DAT_801f04a0 == 5) || (_DAT_801f04a0 == 8)) goto LAB_8013663c;
  _DAT_801f04a0 = _DAT_801f04a0 + -1;
LAB_80136660:
  slot = (short *)&DAT_801f0300;
  k = 0;
  if (0 < _DAT_801f04bc) {
    pr = &DAT_801f0304;
    do {
      if (*slot != 0) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((a3 + 0x82) ))== 0) {
          *(short *)(pr + 0x22) = *(short *)(pr + 0x22) + 2;
          *(int *)(pr + 0x1c) = *(int *)(pr + 0x1c) + (int)*(short *)(pr + 0x24);
          *(int *)(pr + 0x14) = *(int *)(pr + 0x14) + (int)*(short *)(pr + 0x20);
          bnd = (short *)(uintptr_t)_DAT_801f04a4;
          t = *(int *)(pr + 0x18);
          *(int *)(pr + 0x18) = t + *(short *)(pr + 0x22);
          t = (t + *(short *)(pr + 0x22)) - (int)bnd[1];
          if (*(char *)guest_at((uint32_t)(uintptr_t)((a3 + 0x98) ))== '\x02') {
            if (-1 < t) {
              vec_emit_spawn(slot + 0xc);
              deferred_cmd_post(0x400,0x83,0);
              *slot = 0;
              goto LAB_80136b18;
            }
LAB_8013683c:
            *(short *)(pr + 0x28) = *(short *)(pr + 0x28) + *(short *)(pr + 0x2c);
          }
          else {
            if ((t < 0) || (*(short *)(pr + 0x22) < -1)) goto LAB_8013683c;
            if (0xc < *(short *)(pr + 0x22)) {
              deferred_cmd_post(0x400,0x83,0);
            }
            *(undefined2 *)(pr + 0x28) = 0;
            *(short *)(pr + 0x22) =
                 -(short)((((int)((uint)*(ushort *)(pr + 0x22) << 0x10) >> 0x10) -
                          ((int)((uint)*(ushort *)(pr + 0x22) << 0x10) >> 0x1f)) >> 1);
            *(short *)(pr + 0x20) =
                 (short)((((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x10) -
                         ((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
            *(short *)(pr + 0x24) =
                 (short)((((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x10) -
                         ((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
            *(short *)(pr + 0x2c) =
                 (short)((((int)((uint)*(ushort *)(pr + 0x2c) << 0x10) >> 0x10) -
                         ((int)((uint)*(ushort *)(pr + 0x2c) << 0x10) >> 0x1f)) >> 1);
            if (-2 < *(short *)(pr + 0x22)) {
              t = (int)*(short *)(pr + 0x20);
              if (t < 0) {
                t = -t;
              }
              if (t < 2) {
                t = (int)*(short *)(pr + 0x24);
                if (t < 0) {
                  t = -t;
                }
                if (t < 2) {
                  *slot = *slot + -1;
                }
              }
            }
            *(int *)(pr + 0x18) = (int)_DAT_801f04a4[1];
          }
          if (*(int *)(_DAT_801f04a4 + 2) != 0) {
            if (*_DAT_801f04a4 == 0) {
              t = (int)_DAT_801f04a4[2];
              if (((t < *(int *)(pr + 0x14)) && (0 < *(short *)(pr + 0x20))) ||
                 ((t = -t, *(int *)(pr + 0x14) < t &&
                  (*(short *)(pr + 0x20) < 0)))) {
                deferred_cmd_post(0x400,0x83,0);
                *(int *)(pr + 0x14) = t;
                *(short *)(pr + 0x20) =
                     -(short)((((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x10) -
                              ((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
                *(short *)(pr + 0x24) =
                     (short)((((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x10) -
                             ((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
                *(short *)(pr + 0x2c) = -*(short *)(pr + 0x2c);
              }
              t = (int)_DAT_801f04a4[3];
              if (((t < *(int *)(pr + 0x1c)) && (0 < *(short *)(pr + 0x24))) ||
                 ((t = -t, *(int *)(pr + 0x1c) < t &&
                  (*(short *)(pr + 0x24) < 0)))) {
                deferred_cmd_post(0x400,0x83,0);
                *(int *)(pr + 0x1c) = t;
                *(short *)(pr + 0x20) =
                     -(short)((((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x10) -
                              ((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
                *(short *)(pr + 0x24) =
                     (short)((((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x10) -
                             ((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
                *(short *)(pr + 0x2c) = -*(short *)(pr + 0x2c);
              }
            }
            else if (*(int *)(_DAT_801f04a4 + 2) <
                     *(int *)(pr + 0x14) * *(int *)(pr + 0x14) +
                     *(int *)(pr + 0x1c) * *(int *)(pr + 0x1c)) {
              deferred_cmd_post(0x400,0x83,0);
              vx = *(short *)(pr + 0x20);
              vy = *(short *)(pr + 0x24);
              *(short *)(pr + 0x20) =
                   -(short)((((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x10) -
                            ((int)((uint)*(ushort *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
              *(short *)(pr + 0x24) =
                   -(short)((((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x10) -
                            ((int)((uint)*(ushort *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
              *(int *)(pr + 0x14) = *(int *)(pr + 0x14) - (int)vx;
              *(int *)(pr + 0x1c) = *(int *)(pr + 0x1c) - (int)vy;
              *(short *)(pr + 0x2c) = -*(short *)(pr + 0x2c);
            }
          }
          angle_pair_rotmat((int)*(short *)(pr + 0x28),(int)*(short *)(pr + 0x2a),pr);
        }
        gte_rtps_xform3_t((undefined4 *)(uintptr_t)a2,(ushort *)(uintptr_t)pr,(uint *)xf);
        setCopControlWord(2,0,xf[0]);
        setCopControlWord(2,0x800,xf[1]);
        setCopControlWord(2,0x1000,xf[2]);
        setCopControlWord(2,0x1800,xf[3]);
        setCopControlWord(2,0x2000,xf[4]);
        setCopControlWord(2,0x2800,xf[5]);
        setCopControlWord(2,0x3000,xf[6]);
        setCopControlWord(2,0x3800,xf[7]);
        prim_task_run(_DAT_801f04b8 + k * 0x28,a0 + 0x2068,a1,1);
        if (*slot == 0) {
          *slot = 1;
        }
      }
LAB_80136b18:
      k = k + 1;
      pr = pr + 0x34;
      slot = slot + 0x1a;
    } while (k < _DAT_801f04bc);
  }
  return;

}

void radial_glow_draw(int *rec)
{
  undefined4 xy;
  undefined4 in_zero;
  undefined4 in_at;
  short ox;
  short cy;
  short oy;
  int rad;
  int cs;
  short cx;
  undefined2 lvl;
  int t;
  int *cur;
  int k;
  short ex;
  short ey;
  short sx;
  short sy;
  uint rnd;
  setCopReg(2,in_zero,rec[0xd]);
  setCopReg(2,in_at,rec[0xe]);
  copFunction(2,0x180001);
  xy = getCopReg(2,0xe);
  ex = (short)xy;
  ey = (short)((uint)xy >> 0x10);
  angle_vec_build(((int)ex + (int)ey) * 2,&DAT_1f8003e0);
  rad = isqrt(((0xff - ex) * (0xff - ex) +
                       (0xff - ey) * (0xff - ey)) * 0x1000);
  rad = rad >> 0xc;
  k = 0;
  if ((ushort)(ex + 0x32U) < 0x265) {
    t = (int)ey;
    if ((-0x33 < t) && (t < 0x233)) {
      k = rad;
      if (rad < 0) {
        k = rad + 7;
      }
      t = angle_from_xy(0xff - ex,0xff - t);
      cs = lut_cos_q12(t + -0x400);
      ox = (short)((k >> 3) * cs >> 0xc);
      t = lut_sin_q12(t + -0x400);
      cx = (short)((ex + -0x100) / 2) + 0xf0;
      cy = (short)((ey + -0x100) / 2) + 0xf0;
      oy = (short)((k >> 3) * t >> 0xc);
      rnd = rand_lcg();
      if ((rnd & 3) != 0) {
        rect_prim_spawn((int)cx,(int)cy,0x1f,0x1f,0xc0,0x20,0x39,0x78d9,1);
      }
      rect_prim_spawn(((int)cx - (int)ox) + -0x10,((int)cy - (int)oy) + -0x10,0x3f,0x3f,
                   0xc0,0x40,0x39,0x78d9,1);
      sx = 0xf0;
      sy = 0xf0;
      if (rad < 0x96) {
        rnd = rand_lcg();
        if ((rnd & 7) != 0) {
          rect_prim_spawn(0xe0,0xe0,0x40,0x40,0xc0,0x40,0x39,0x78d9,1);
        }
        k = 0;
        do {
          sx = sx + ox;
          sy = sy + oy;
          rnd = rand_lcg();
          if ((rnd & 3) != 0) {
            rect_prim_spawn((int)sx,(int)sy,0x1f,0x1f,0xc0,0x20,0x39,0x78d9,1);
          }
          k = k + 1;
        } while (k < 2);
      }
      k = 0;
      cur = rec;
      do {
        t = *cur;
        cur = cur + 1;
        k = k + 1;
        obj_flag34_set(t);
      } while (k < 3);
      cur = rec + 3;
      do {
        rnd = rand_lcg();
        if ((rnd & 3) == 0) {
          obj_flag34_clear(*cur);
        }
        else {
          obj_flag34_set(*cur);
        }
        cur = cur + 1;
      } while ((intptr_t)cur < (intptr_t)(rec + 5));
      t = rad * 0x10 + -0x200;
      k = (0x1000 - t) / 2 + 0x800;
      rad = k;
      if (k < 0) {
        rad = 0;
      }
      lvl = (undefined2)rad;
      if (0xfff < k) {
        lvl = 0xfff;
      }
      rad = 1;
      *(undefined2 *)(rec + 5) = lvl;
      *(undefined2 *)(rec + 7) = lvl;
      *(undefined2 *)(rec + 9) = lvl;
      cur = rec;
      do {
        cur = cur + 1;
        rad = rad + 1;
        tween_seek(*cur,t);
      } while (rad < 5);
      rad = 0;
      cur = rec;
      do {
        if (*cur != 0) {
          if (rad == 0) {
            gte_rtps_xform3((undefined4 *)(uintptr_t)&DAT_801f0760,(ushort *)(uintptr_t)&DAT_1f8003e0,(uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec))));
          }
          else if (rad == 1) {
            gte_rtps_xform3((undefined4 *)(rec + 5),(ushort *)(uintptr_t)&DAT_801f0760,(uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]))));
          }
          else {
            *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(*cur )))= _DAT_801f0760;
            *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 4) ))= _DAT_801f0764;
            *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 8) ))= _DAT_801f0768;
            *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 0xc) ))= _DAT_801f076c;
            *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 0x10) ))= _DAT_801f0770;
          }
        }
        cur = cur + 1;
        rad = rad + 1;
      } while (rad < 5);
      return;
    }
  }
  do {
    rad = *rec;
    rec = rec + 1;
    k = k + 1;
    obj_flag34_clear(rad);
  } while (k < 5);
  return;

}

void iris_wipe_spawn(int req,int arg)
{
  char x1;
  int tmp;
  undefined1 *bp;
  char x0;
  int *pay;
  int j;
  int *cur;
  if ((DAT_1f8000b1 == '\0') &&
     (((*(char *)guest_at((uint32_t)(uintptr_t)((req + 3) ))!= '\x04' || (*(char *)guest_at((uint32_t)(uintptr_t)((req + 4) ))!= '\x02')) ||
      (arg == 0xfffffff)))) {
    tmp = (int)(uintptr_t)task_node_alloc(1,2,6,0x8013b12c,0,0x350);
    if (tmp != 0) {
      pay = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((tmp + 0x10))));
      *pay = req;
      pay[0xd1] = arg;
      *(undefined2 *)(pay + 0xd2) = 0;
      cur = pay + 0x1d;
      if (*(char *)guest_at((uint32_t)(uintptr_t)((req + 3) ))== '\x04') {
        *(undefined2 *)((intptr_t)pay + 0x34a) = 1;
        if (*(char *)guest_at((uint32_t)(uintptr_t)((req + 5) ))== '\x11') {
          *(undefined2 *)(pay + 0xd3) = 1;
        }
        else {
          *(undefined2 *)(pay + 0xd3) = 0;
        }
      }
      else {
        *(undefined2 *)((intptr_t)pay + 0x34a) = 0;
      }
      tmp = 0;
      *(undefined2 *)(pay + 1) = 10;
      *(undefined2 *)((intptr_t)pay + 6) = 0xffe9;
      *(undefined2 *)(pay + 2) = 0xffd8;
      *(undefined2 *)(pay + 3) = 0xfff6;
      *(undefined2 *)((intptr_t)pay + 0xe) = 0xffe9;
      *(undefined2 *)(pay + 4) = 0xffd8;
      do {
        j = 0;
        bp = (undefined1 *)((intptr_t)cur + 7);
        x0 = -1;
        do {
          x1 = x0 + -0x32;
          bp[-3] = x0;
          bp[5] = x0;
          bp[-2] = 0;
          bp[-1] = 0;
          bp[6] = 0;
          bp[7] = 0;
          bp[0xd] = x1;
          bp[0xe] = 0;
          bp[0xf] = 0;
          bp[0x15] = x1;
          bp[0x16] = 0;
          bp[0x17] = 0;
          *cur = 0x8000000;
          *bp = 0x3a;
          bp = bp + 0x24;
          j = j + 1;
          cur = cur + 9;
          x0 = x1;
        } while (j < 5);
        tmp = tmp + 1;
      } while (tmp < 4);
    }
  }
  return;

}

undefined4 * gte_xform3_ip(undefined4 *rt,undefined2 *in)
{
  uint t0;
  int t1;
  undefined4 t2;
  uint t3;
  int t4;
  uint t5;
  int t6;
  uint t7;
  int t8;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,CONCAT22(in[3],*in));
  setCopReg(2,0x800,*(uint *)(in + 6));
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)in[1] | *(uint *)(in + 4) << 0x10);
  setCopReg(2,0x800,*(uint *)(in + 6) >> 0x10);
  copFunction(2,0x486012);
  t6 = getCopReg(2,0x4800);
  t7 = getCopReg(2,0x5000);
  t8 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)in[2] | *(uint *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(in + 8));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t2 = getCopReg(2,0x5800);
  setCopControlWord(2,0x4000,t6 << 0x10 | t3 & 0xffff);
  setCopControlWord(2,0x4800,t0 & 0xffff | t4 << 0x10);
  setCopControlWord(2,0x5000,t1 << 0x10 | t7 & 0xffff);
  setCopControlWord(2,0x5800,t8 << 0x10 | t5 & 0xffff);
  setCopControlWord(2,0x6000,t2);
  return rt;

}

undefined4 * gte_xform3_t_ip(undefined4 *rt,undefined2 *in)
{
  uint t0;
  int t1;
  uint t2;
  int t3;
  int t4;
  int t5;
  int t6;
  uint t7;
  undefined4 t8;
  int t9;
  uint t10;
  int t11;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,CONCAT22(in[3],*in));
  setCopReg(2,0x800,*(uint *)(in + 6));
  copFunction(2,0x486012);
  t2 = getCopReg(2,0x4800);
  t6 = getCopReg(2,0x5000);
  t7 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)in[1] | *(uint *)(in + 4) << 0x10);
  setCopReg(2,0x800,*(uint *)(in + 6) >> 0x10);
  copFunction(2,0x486012);
  t9 = getCopReg(2,0x4800);
  t10 = getCopReg(2,0x5000);
  t11 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)in[2] | *(uint *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(in + 8));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t3 = getCopReg(2,0x5000);
  t8 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)(ushort)in[10] | *(int *)(in + 0xc) << 0x10);
  setCopReg(2,0x800,*(undefined4 *)(in + 0xe));
  copFunction(2,0x486012);
  t4 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopControlWord(2,0,t9 << 0x10 | t2 & 0xffff);
  setCopControlWord(2,0x800,t0 & 0xffff | t6 << 0x10);
  setCopControlWord(2,0x1000,t10 & 0xffff | t3 << 0x10);
  setCopControlWord(2,0x1800,t11 << 0x10 | t7 & 0xffff);
  setCopControlWord(2,0x2000,t8);
  setCopControlWord(2,0x2800,rt[5] + t4);
  setCopControlWord(2,0x3000,t1 + rt[6]);
  setCopControlWord(2,0x3800,rt[7] + t5);
  return rt;

}

int prim_task_run(int rec,undefined4 a1,undefined4 a2,int mode)
{
  undefined4 t1;
  undefined4 t2;
  undefined4 t3;
  undefined4 t4;
  undefined4 t5;
  undefined4 t6;
  undefined4 sv_ra;
  undefined4 sv_s6;
  undefined4 sv_s3;
  undefined4 sv_s2;
  undefined4 sv_s1;
  undefined4 sv_s0;
  undefined4 unaff_s0;
  undefined4 unaff_s1;
  undefined4 unaff_s2;
  int *pi;
  undefined4 unaff_s3;
  undefined4 unaff_s6;
  int n;
  undefined4 unaff_retaddr;
  sv_ra = unaff_retaddr;
  sv_s6 = unaff_s6;
  sv_s3 = unaff_s3;
  sv_s2 = unaff_s2;
  sv_s1 = unaff_s1;
  sv_s0 = unaff_s0;
  t1 = _DAT_1f8003fc;
  t2 = _DAT_1f8003f8;
  t3 = _DAT_1f8003f4;
  t4 = _DAT_1f8003f0;
  t5 = _DAT_1f8003ec;
  t6 = _DAT_1f8003e0;
  for (; _DAT_1f8003f8 = sv_s0, _DAT_1f8003f4 = sv_s1, _DAT_1f8003f0 = sv_s2,
      _DAT_1f8003ec = sv_s3, _DAT_1f8003e0 = sv_s6, _DAT_1f8003fc = sv_ra, 0 < mode;
      mode = mode + -1) {
    if (*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))== 0) {
      n = *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x1c))));
      pi = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x1c) )))+ 1;
      if (n != 0) {
        /* Generated 0x8018A4F8 jalr's each record fn with the full rec-mode
         * set: rec = record[2] (data cursor), a1 = a1 + lh(task+0xE)
         * (computed once per task block), a2 = a2, mode = record[1]
         * (per-record field). task_invoke/call_guest only program $rec/$a1,
         * leaving $a2/$mode as stale register-file values — truncated host
         * pointers that the generated mode-7 serializer (0x8018A9E8)
         * dereferences, spraying kernel RAM. call_guest4 reproduces the
         * guest ABI exactly. */
        uint32_t prim_a1 = a1 + (uint32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ));
        do {
          n = n + -1;
          call_guest4((uint32_t)pi[3],(uint32_t)pi[2],prim_a1,(uint32_t)a2,(uint32_t)pi[1]);
          pi = pi + 4;
        } while (0 < n);
      }
    }
    rec = rec + 0x28;
    sv_ra = _DAT_1f8003fc;
    sv_s6 = _DAT_1f8003e0;
    sv_s3 = _DAT_1f8003ec;
    sv_s2 = _DAT_1f8003f0;
    sv_s1 = _DAT_1f8003f4;
    sv_s0 = _DAT_1f8003f8;
    t1 = _DAT_1f8003fc;
    t2 = _DAT_1f8003f8;
    t3 = _DAT_1f8003f4;
    t4 = _DAT_1f8003f0;
    t5 = _DAT_1f8003ec;
    t6 = _DAT_1f8003e0;
  }
  _DAT_1f8003e0 = t6;
  _DAT_1f8003ec = t5;
  _DAT_1f8003f0 = t4;
  _DAT_1f8003f4 = t3;
  _DAT_1f8003f8 = t2;
  _DAT_1f8003fc = t1;
  return rec;

}

undefined2 * vec2_rot(undefined4 ang0,undefined4 ang1,undefined2 *dst)
{
  undefined2 c0;
  int ns0;
  int p;
  int c1;
  ns0 = lut_sin_q12();
  ns0 = -ns0;
  c0 = lut_cos_q12();
  dst[1] = c0;
  p = lut_sin_q12(ang1);
  p = ns0 * p;
  c1 = lut_cos_q12();
  *dst = (short)(p >> 0xc);
  dst[2] = (short)(ns0 * c1 >> 0xc);
  return dst;

}

uint * angle_gte_vec(undefined4 x,undefined4 y,uint *out)
{
  int sx;
  int xy;
  int xz;
  uint one;
  one = 0x1000;
  sx = trig_sin_lookup(x);
  xy = lut_sin_q12(y);
  xy = sx * xy;
  xz = lut_cos_q12(y);
  *out = one | (xy >> 0xc) << 0x10;
  out[1] = 0;
  out[2] = 0;
  *(short *)(out + 4) = (short)one;
  out[3] = (sx * xz >> 0xc) << 0x10;
  return out;

}

int * prim_list_persp_step(int *a0,undefined4 a1,undefined4 a2,int a3,undefined4 arg5 ,int arg6,undefined4 arg7,undefined4 arg8)
{
  undefined4 in_zero;
  undefined4 extraout_at;
  int t;
  int vp;
  int sz;
  undefined4 in_t4;
  undefined4 in_t5;
  undefined4 in_t6;
  undefined4 in_t7;
  undefined4 unaff_s0;
  undefined4 unaff_s1;
  undefined4 unaff_s2;
  int *rp;
  undefined4 unaff_s3;
  undefined4 unaff_s4;
  undefined4 unaff_s5;
  undefined4 unaff_s6;
  undefined4 unaff_s7;
  undefined4 in_t8;
  undefined4 unaff_retaddr;
  if (0 < a3) {
    _DAT_1f8003d4 = arg5;
    _DAT_1f8003d0 = arg7;
    _DAT_1f8003cc = arg8;
    _DAT_1f8003d8 = a1;
    _DAT_1f8003dc = unaff_s7;
    _DAT_1f8003e0 = unaff_s6;
    _DAT_1f8003e4 = unaff_s5;
    _DAT_1f8003e8 = unaff_s4;
    _DAT_1f8003ec = unaff_s3;
    _DAT_1f8003f0 = unaff_s2;
    _DAT_1f8003f4 = unaff_s1;
    _DAT_1f8003f8 = unaff_s0;
    _DAT_1f8003fc = unaff_retaddr;
    do {
      if (((short)a0[3] == 0) && (*a0 != 0)) {
        vp = *a0 + 0x20;
        gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)arg6),(undefined2 *)guest_at((uint32_t)(*a0 + 0x20)));
        setCopReg(2,in_zero,a0[1]);
        setCopReg(2,extraout_at,a0[2]);
        copFunction(2,0x480012);
        t = getCopReg(2,0xc800);
        sz = getCopReg(2,0xd800);
        _DAT_1f8003a0 = in_t6;
        _DAT_1f8003a4 = in_t4;
        _DAT_1f8003a8 = in_t7;
        _DAT_1f8003ac = in_t8;
        _DAT_1f8003b0 = in_t5;
        _DAT_1f8003b8 = range_circle_test(t,sz,(uint *)guest_at((uint32_t)_DAT_1f8003d0),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((vp + 0x18))));
        if (_DAT_1f8003b8 != 0) {
          t = t + ((int)*(short *)guest_at((uint32_t)(uintptr_t)((arg6 + 2) ))* -_DAT_1f8003b8 >> 0xc);
          sz = sz + ((int)*(short *)guest_at((uint32_t)(uintptr_t)((arg6 + 0xe) ))* -_DAT_1f8003b8 >> 0xc);
        }
        _DAT_1f8003b4 = t;
        _DAT_1f8003bc = sz;
        gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)_DAT_1f8003d4),(undefined2 *)(uintptr_t)&DAT_1f8003a0);
        t = *(int *)guest_at((uint32_t)(uintptr_t)(a0[7]));
        rp = (int *)guest_at((uint32_t)(uintptr_t)(a0[7] ))+ 1;
        if (t != 0) {
          /* Generated 0x8018B208 jalr's each record fn with the full a0-a3
           * set, same {count,[x,a3field,arg0,func]} stride-16 table as
           * prim_task_run: a0 = entry[+8] (data cursor), a1 = a1,
           * a2 = a2 (both loaded once per node before the entry loop),
           * a3 = entry[+4] (per-record field). task_invoke/call_guest only
           * program $a0/$a1, leaving $a2/$a3 as stale register-file values
           * the callee dereferences. call_guest4 reproduces the guest ABI
           * exactly (same fix prim_task_run carries). */
          do {
            t = t + -1;
            call_guest4((uint32_t)rp[3],(uint32_t)rp[2],
                        (uint32_t)a1,(uint32_t)a2,(uint32_t)rp[1]);
            rp = rp + 4;
          } while (0 < t);
        }
      }
      a3 = a3 + -1;
      a0 = a0 + 10;
    } while (0 < a3);
  }
  return a0;

}

void gte_light_set(undefined4 x,undefined4 y,undefined4 z)
{
  setCopControlWord(2,0x6800,x);
  setCopControlWord(2,0x7000,y);
  setCopControlWord(2,0x7800,z);
  return;

}


int * gte_xform_add_off(int x,undefined4 *y,int *z)
{
  undefined4 in_zero;
  undefined4 in_at;
  int oy;
  int oz;
  int tx;
  int ty;
  int tz;
  setCopReg(2,in_zero,*y);
  setCopReg(2,in_at,y[1]);
  copFunction(2,0x486012);
  oy = *(int *)guest_at((uint32_t)(uintptr_t)((x + 0x18)));
  oz = *(int *)guest_at((uint32_t)(uintptr_t)((x + 0x1c)));
  tx = getCopReg(2,0x4800);
  ty = getCopReg(2,0x5000);
  tz = getCopReg(2,0x5800);
  *z = *(int *)guest_at((uint32_t)(uintptr_t)((x + 0x14) ))+ tx;
  z[1] = oy + ty;
  z[2] = oz + tz;
  return z;

}

uint * angle_pair_rotmat(uint x,uint y,uint *z)
{
  int i0;
  int i1;
  int i2;
  int i3;
  uint v0;
  uint v1;
  int i4;
  i0 = (x & 0x3ff) * 2;
  i4 = (x & 0x3ff) * -2 + 0x800;
  i3 = i0;
  if ((x & 0x400) != 0) {
    i3 = i4;
    i4 = i0;
  }
  i3 = (int)*(short *)(&DAT_801247a8 + i3);
  if ((x & 0x800) != 0) {
    i3 = -i3;
  }
  i1 = (y & 0x3ff) * 2;
  i2 = (y & 0x3ff) * -2 + 0x800;
  i0 = i1;
  if ((y & 0x400) != 0) {
    i0 = i2;
    i2 = i1;
  }
  i0 = (int)*(short *)(&DAT_801247a8 + i0);
  if ((y & 0x800) != 0) {
    i0 = -i0;
  }
  v1 = (uint)*(short *)(&DAT_801247a8 + i2);
  if ((y + 0x400 & 0x800) != 0) {
    v1 = -v1;
  }
  v0 = (uint)*(short *)(&DAT_801247a8 + i4);
  if ((x + 0x400 & 0x800) != 0) {
    v0 = -v0;
  }
  z[3] = -i0 & 0xffffU | ((int)(v1 * i3) >> 0xc) << 0x10;
  *z = v1 & 0xffff | (i0 * i3 >> 0xc) << 0x10;
  z[1] = (int)(i0 * v0) >> 0xc & 0xffff;
  z[2] = v0 & 0xffff | i3 * -0x10000;
  *(short *)(z + 4) = (short)((int)(v1 * v0) >> 0xc);
  return z;

}

/* ---- batch18: task/alloc + GTE helpers ---- */
void vec_emit_spawn(undefined4 *rec)
{
  int grp;
  grp = fx_group_alloc(1,1,3,3);
  if (grp != 0) {
    noop_a(grp,*rec,rec[1],rec[2]);
  }
  return;

}

void iris_wipe_update(int *rec)
{
  short sVar1;
  undefined4 in_zero;
  undefined4 in_at;
  int iVar2;
  int iVar3;
  undefined2 *puVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  short *psVar11;
  short *psVar12;
  uint *puVar13;
  short *psVar14;
  int iVar15;
  short local_88 [24];
  undefined1 auStack_58 [24];
  ushort local_40 [12];
  if (DAT_801bef20 == 0) {
    iVar2 = rec[0xd1];
    rec[0xd1] = iVar2 + -1;
    if (iVar2 + -1 < 1) {
      task_slot_step();
      return;
    }
    if (*(short *)((intptr_t)rec + 0x34a) != 0) {
      if (*(char *)guest_at((uint32_t)(uintptr_t)((*rec + 5) ))== '\0') {
        *(undefined2 *)(rec + 0xd3) = 0;
        return;
      }
      if ((short)rec[0xd3] == 0) {
        *(undefined2 *)(rec + 0xd3) = 1;
        *(undefined2 *)(rec + 0xd2) = 0;
      }
    }
    iVar15 = 0;
    iVar7 = 0x14;
    iVar2 = 4;
    *(short *)(rec + 0xd2) = (short)rec[0xd2] + 1;
    piVar6 = rec;
    do {
      iVar3 = *(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20)));
      setCopControlWord(2,0,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0xc0))));
      setCopControlWord(2,0x800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0xc4))));
      setCopControlWord(2,0x1000,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar3 + 200))));
      setCopControlWord(2,0x1800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0xcc))));
      setCopControlWord(2,0x2000,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0xd0))));
      setCopReg(2,in_zero,*(undefined4 *)((intptr_t)rec + iVar2));
      setCopReg(2,in_at,((undefined4 *)((intptr_t)rec + iVar2))[1]);
      copFunction(2,0x486012);
      puVar4 = (undefined2 *)((intptr_t)rec + iVar7);
      uVar8 = getCopReg(2,0x4800);
      uVar9 = getCopReg(2,0x5000);
      uVar10 = getCopReg(2,0x5800);
      *puVar4 = (short)uVar8;
      puVar4[1] = (short)uVar9;
      puVar4[2] = (short)uVar10;
      *(short *)(piVar6 + 5) = (short)piVar6[5] + *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ 0xd4)));
      iVar7 = iVar7 + 0x30;
      *(short *)((intptr_t)piVar6 + 0x16) =
           *(short *)((intptr_t)piVar6 + 0x16) + *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ 0xd8)));
      iVar2 = iVar2 + 8;
      iVar15 = iVar15 + 1;
      *(short *)(piVar6 + 6) = (short)piVar6[6] + *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ 0xdc)));
      piVar6 = piVar6 + 0xc;
    } while (iVar15 < 2);
    iVar7 = 0;
    iVar2 = 0;
    do {
      iVar3 = 5;
      iVar15 = 0x28;
      do {
        iVar3 = iVar3 + -1;
        iVar5 = iVar3 * 8 + iVar2;
        uVar8 = *(undefined4 *)((intptr_t)rec + iVar5 + 0x18);
        *(undefined4 *)((intptr_t)rec + iVar15 + iVar2 + 0x14) =
             *(undefined4 *)((intptr_t)rec + iVar5 + 0x14);
        *(undefined4 *)((intptr_t)rec + iVar15 + iVar2 + 0x18) = uVar8;
        iVar15 = iVar3 * 8;
      } while (0 < iVar3);
      iVar7 = iVar7 + 1;
      iVar2 = iVar2 + 0x30;
    } while (iVar7 < 2);
  }
  psVar14 = local_88;
  gte_rot_load((undefined4 *)(uintptr_t)&DAT_801be838);
  iVar7 = 0;
  gte_tr_load(DAT_801be838);
  gte_points_xform(rec + 5,psVar14,local_40,auStack_58,auStack_58,0xc);
  iVar2 = _DAT_1f80000c * 0x5a + 0x1d;
  do {
    puVar13 = (uint *)(rec + iVar2);
    iVar2 = 0;
    psVar12 = (short *)((intptr_t)puVar13 + 0x22);
    psVar11 = psVar14 + 3;
    do {
      if ((short)rec[0xd2] <= iVar2) break;
      uVar8 = angle_from_xy((int)psVar11[-1] - (int)*psVar14,(int)*psVar11 - (int)psVar11[-2]);
      iVar3 = mips_div_lo((int32_t)((int)((uint)_DAT_1f800024 << 0x10) >> 0xc),(int32_t)(int)((uint)local_40[0] << 2)); /* guest div @0x8013B3F0 */
      iVar15 = lut_cos_q12(uVar8);
      psVar12[-0xd] = *psVar14 + (short)(iVar3 * iVar15 >> 0xc);
      iVar15 = lut_cos_q12(uVar8);
      psVar12[-9] = *psVar14 - (short)(iVar3 * iVar15 >> 0xc);
      iVar15 = lut_sin_q12(uVar8);
      psVar12[-0xc] = psVar11[-2] + (short)(iVar3 * iVar15 >> 0xc);
      iVar15 = lut_sin_q12(uVar8);
      psVar12[-8] = psVar11[-2] - (short)(iVar3 * iVar15 >> 0xc);
      iVar15 = lut_cos_q12(uVar8);
      psVar12[-5] = psVar11[-1] + (short)(iVar3 * iVar15 >> 0xc);
      iVar15 = lut_cos_q12(uVar8);
      psVar12[-1] = psVar11[-1] - (short)(iVar3 * iVar15 >> 0xc);
      iVar15 = lut_sin_q12(uVar8);
      psVar12[-4] = *psVar11 + (short)(iVar3 * iVar15 >> 0xc);
      iVar15 = lut_sin_q12(uVar8);
      iVar2 = iVar2 + 1;
      psVar14 = psVar14 + 2;
      sVar1 = *psVar11;
      psVar11 = psVar11 + 2;
      *psVar12 = sVar1 - (short)(iVar3 * iVar15 >> 0xc);
      psVar12 = psVar12 + 0x12;
      *puVar13 = *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x40) ))| 0x8000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x40) ))= guest_of(puVar13) & 0xffffff;
      puVar13 = puVar13 + 9;
    } while (iVar2 < 5);
    iVar7 = iVar7 + 1;
    iVar2 = _DAT_1f80000c * 0x5a + 0x4a;
    psVar14 = local_88 + 12;
    if (1 < iVar7) {
      return;
    }
  } while( true );

}

undefined4 script_op_dispatch(undefined4 a0,undefined4 a1,int a2,undefined4 a3)
{
  byte op;
  short cont;
  int n;
  int iter;
  iter = 0;
  do {
    op = *(byte *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((a2 + 0x24))));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((a2 + 0x6e) ))!= 0) {
      if ((*(int *)guest_at((uint32_t)(uintptr_t)((a2 + 0x78) ))+ -0x7c) - a2 < 0xbc) {
        /* guest: a0 = *(u32*)(s0+0x78) guest cursor addr; a2 = guest
         * string-table entry @0x801b5574[op] (guest addr -> host). */
        n = sprintf_core((char *)guest_at((uint32_t)*(int *)guest_at(a2 + 0x78)),
                             &DAT_801237e0,
                             (char *)guest_at(*(uint32_t *)guest_at(0x801b5574 + (uint32_t)op * 4)),
                             a3);
        *(int *)guest_at((uint32_t)(uintptr_t)((a2 + 0x78) ))= *(int *)guest_at((uint32_t)(uintptr_t)((a2 + 0x78) ))+ n;
      }
    }
    /* PTR_entry_0x80182f3c_801b5528 is the guest opcode-handler jump table
     * @0x801b5528 (19 entries, handlers @0x80182f3c..0x80184f28). It was never
     * populated with host pointers, so indexing it called address 0. Dispatch
     * through the real guest table via br2_call_generated — the same jalr the
     * original performs — returning the handler's $v0 as the s16 result. */
    cont = (short)br2_call_clean(
        *(uint32_t *)guest_at(0x801b5528u + (uint32_t)op * 4u),
        a0, a1, (uint32_t)a2, a3);
    iter = iter + 1;
  } while ((cont != 0) && (iter * 0x10000 >> 0x10 < 0x32));
  return 0;

}

int trig_sin_lookup(uint ang)
{
  uint idx;
  int res;
  idx = ang & 0x3ff;
  if ((ang & 0x400) != 0) {
    idx = (ang & 0x400) - idx;
  }
  if ((int)(idx - 0x3af) < 0) {
    res = (int)*(short *)(&DAT_80124048 + idx * 2);
  }
  else {
    res = 0x7fff;
  }
  if ((ang & 0x400) != 0) {
    return -res;
  }
  return res;

}

uint * angle_vec_build(uint ang,uint *out)
{
  int i0;
  int i2;
  uint v0;
  int i1;
  uint v1;
  *(undefined2 *)(out + 4) = 0x1000;
  i0 = (ang & 0x3ff) * 2;
  i1 = (ang & 0x3ff) * -2 + 0x800;
  i2 = i0;
  if ((ang & 0x400) != 0) {
    i2 = i1;
    i1 = i0;
  }
  v0 = (uint)*(ushort *)(&DAT_801247a8 + i2);
  if ((ang & 0x800) != 0) {
    v0 = -v0;
  }
  v1 = (uint)*(ushort *)(&DAT_801247a8 + i1);
  if ((ang + 0x400 & 0x800) != 0) {
    v1 = -v1 & 0xffff;
  }
  *out = v1 | v0 * -0x10000;
  out[1] = v0 * 0x10000;
  out[2] = v1;
  out[3] = 0;
  return out;

}

uint rand_lcg(void)
{
  _DAT_801f68e0 = _DAT_801f68e0 * 0x41c64e6d + 0x3039;
  return _DAT_801f68e0 >> 0x10 & 0x7fff;

}

int isqrt(int n)
{
  int r;
  uint sh;
  if (n == 0) {
    r = 0;
  }
  else {
    r = cop2_clz(n);
    sh = (8 - r) >> 1;
    if (8 - r < 0) {
      sh = sh + 1;
      n = n << (sh * -2 & 0x1f);
    }
    else {
      n = n >> ((sh & 0xf) << 1);
    }
    sh = sh - 6;
    if ((int)sh < 0) {
      r = isqrt_refine(n);
      r = r >> (-sh & 0x1f);
    }
    else {
      r = isqrt_refine(n);
      r = r << (sh & 0x1f);
    }
  }
  return r;

}

uint * gte_rtps_xform3_t(undefined4 *n,ushort *a1,uint *a2)
{
  uint t0;
  int t1;
  int t2;
  uint t3;
  int t4;
  int t5;
  uint t6;
  int t7;
  setCopControlWord(2,0,*n);
  setCopControlWord(2,0x800,n[1]);
  setCopControlWord(2,0x1000,n[2]);
  setCopControlWord(2,0x1800,n[3]);
  setCopControlWord(2,0x2000,n[4]);
  setCopReg(2,0,(uint)*a1 | *(uint *)(a1 + 2) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(a1 + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t3 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)a1[1] | *(int *)(a1 + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)a1[7]);
  copFunction(2,0x486012);
  t5 = getCopReg(2,0x4800);
  t6 = getCopReg(2,0x5000);
  t7 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)a1[2] | *(uint *)(a1 + 4) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(a1 + 8));
  copFunction(2,0x486012);
  *a2 = t5 << 0x10 | t0 & 0xffff;
  a2[3] = t7 << 0x10 | t3 & 0xffff;
  t3 = getCopReg(2,0x4800);
  t5 = getCopReg(2,0x5000);
  t0 = getCopReg(2,0xb);
  a2[4] = t0;
  setCopReg(2,0,(uint)a1[10] | *(int *)(a1 + 0xc) << 0x10);
  setCopReg(2,0x800,*(undefined4 *)(a1 + 0xe));
  copFunction(2,0x486012);
  a2[1] = t3 & 0xffff | t1 << 0x10;
  a2[2] = t5 << 0x10 | t6 & 0xffff;
  t5 = getCopReg(2,0xc800);
  t1 = getCopReg(2,0xd000);
  t7 = getCopReg(2,0xd800);
  t2 = n[6];
  t4 = n[7];
  a2[5] = t5 + n[5];
  a2[6] = t1 + t2;
  a2[7] = t7 + t4;
  return a2;

}

undefined4 * gte_xform1_ir(undefined4 *mtx,undefined4 *out)
{
  undefined4 v;
  setCopReg(2,0,*mtx);
  setCopReg(2,0x800,mtx[1]);
  copFunction(2,0x486012);
  v = getCopReg(2,9);
  *out = v;
  v = getCopReg(2,10);
  out[1] = v;
  v = getCopReg(2,0xb);
  out[2] = v;
  return out;

}

undefined4 * gte_rtps_xform1_raw(undefined4 *mtx,undefined4 *in,undefined4 *out)
{
  undefined4 v;
  undefined4 in_zero;
  undefined4 in_at;
  setCopControlWord(2,0,*mtx);
  setCopControlWord(2,0x800,mtx[1]);
  setCopControlWord(2,0x1000,mtx[2]);
  setCopControlWord(2,0x1800,mtx[3]);
  setCopControlWord(2,0x2000,mtx[4]);
  setCopReg(2,in_zero,*in);
  setCopReg(2,in_at,in[1]);
  copFunction(2,0x486012);
  v = getCopReg(2,0x19);
  *out = v;
  v = getCopReg(2,0x1a);
  out[1] = v;
  v = getCopReg(2,0x1b);
  out[2] = v;
  return out;

}

int * node_lazy_attach(int n,int a1,int a2,int a3)
{
  char cVar1;
  undefined1 sh;
  int *piVar3;
  int *piVar4;
  cVar1 = *(char *)guest_at((uint32_t)(uintptr_t)((n + 0x14)));
  if ((a1 == 0) ||
     (piVar3 = (int *)guest_at((uint32_t)(uintptr_t)(task_pool_alloc((int)cVar1,(int)*(char *)guest_at((uint32_t)(uintptr_t)((n + 0x15))),a3 + 0x1c))),
     piVar3 == (int *)0x0)) {
    piVar3 = (int *)0x0;
  }
  else {
    piVar4 = (int *)guest_at((uint32_t)(uintptr_t)((cVar1 * 0x48 + -0x7fe00c68 + *(char *)guest_at((uint32_t)(uintptr_t)((n + 0x16) ))* 4)));
    if (*piVar4 == n) {
      *piVar4 = (int)guest_of(piVar3);
    }
    else {
      /* field at n+4 holds a GUEST addr; resolve before deref */
      *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(n + 4)))= guest_of(piVar3);
    }
    *piVar3 = n;
    piVar3[1] = *(int *)guest_at((uint32_t)(uintptr_t)((n + 4)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((n + 4) ))= guest_of(piVar3);
    piVar3[2] = a1;
    piVar3[3] = a2;
    *(undefined1 *)(piVar3 + 5) = *(undefined1 *)guest_at((uint32_t)(uintptr_t)((n + 0x14)));
    *(undefined1 *)((intptr_t)piVar3 + 0x15) = *(undefined1 *)guest_at((uint32_t)(uintptr_t)((n + 0x15)));
    sh = *(undefined1 *)guest_at((uint32_t)(uintptr_t)((n + 0x16)));
    *(undefined1 *)((intptr_t)piVar3 + 0x17) = 1;
    piVar3[4] = (int)guest_of((char *)(piVar3 + 7));
    *(undefined1 *)(piVar3 + 6) = 0;
    *(undefined1 *)((intptr_t)piVar3 + 0x16) = sh;
  }
  return (int *)(uintptr_t)guest_of(piVar3);

}

undefined4 tween_ensure(int chan)
{
  undefined4 ret;
  int sub;
  ret = 0;
  if ((chan != 0) && (ret = 0, *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))== 0)) {
    sub = *(int *)guest_at((uint32_t)(uintptr_t)((chan + 8)));
    ret = 1;
    if (*(int *)guest_at((uint32_t)(uintptr_t)((sub + 0x14) ))== 1) {
      ret = tween_track_alloc(2,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((sub + 0x14))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((chan + 0x30))),
                           (undefined2 *)(uintptr_t)*(undefined4 *)guest_at((uint32_t)(uintptr_t)((sub + 0x8c))),*(undefined4 *)guest_at((uint32_t)(uintptr_t)((sub + 0x24))),
                           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((chan + 8) ))+ 0x30))),
                           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((chan + 8) ))+ 0x38))),
                           (int)*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((chan + 0x14) ))+ 0x14))));
      *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))= ret;
      if (*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))== 0) {
        ret = 0;
      }
      else {
        *(undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x3c) )))= *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))+ 0x10)));
        ret = 1;
      }
    }
  }
  return ret;

}

void obj_flag34_set(int obj)
{
  int i;
  if ((obj != 0) && (i = 0, 0 < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))))) {
    do {
      *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x10)))) ))+ 0x10) ))+ 0x34) ))= 1;
      i = i + 1;
    } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))));
  }
  return;

}

void tween_seek(int seq,int pos)
{
  uint ok;
  int dv;
  int trk;
  int p;
  int off;
  int t;
  int acc;
  int from;
  if (seq != 0) {
    p = pos;
    if (pos < 0) {
      p = 0;
    }
    if (0x1000 < pos) {
      p = 0x1000;
    }
    trk = *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq + 0x3c))));
    if (trk != 0) {
      from = 0;
      acc = 0;
      t = 0;
      if (-1 < *(short *)guest_at((uint32_t)(uintptr_t)((trk + 6)))) {
        off = 0;
        do {
          dv = (int)*(short *)guest_at((uint32_t)(uintptr_t)((trk + 0xc)));
          acc = acc + *(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((trk + 0x18) ))+ 4)));
          /* guest div @0x801A9A5C: num = acc<<12, den = dv (lh).
           * break @0x801A9A68 (dv==0) and @0x801A9A80 (INT_MIN/-1) are
           * recompiler no-ops -> LO = dv==0 ? (num>=0?-1:1)
           * : (INT_MIN/-1 ? INT_MIN : num/dv). C /0 would SIGFPE. */
          {
            int32_t _num = (int32_t)((uint32_t)acc << 12);
            int32_t _quo;
            if (dv != 0) {
              _quo = (_num == (int32_t)0x80000000 && dv == -1)
                   ? (int32_t)0x80000000 : _num / dv;
            } else {
              _quo = (_num >= 0) ? -1 : 1;
            }
            ok = (uint)((_quo < p) ^ 1);
          }
          if (ok != 0) {
            t = (p * 0x1000) / (int)ok;
            if (ok == 0) {
              trap(0x1c00);
            }
            if ((ok == 0xffffffff) && (p * 0x1000 == -0x80000000)) {
              trap(0x1800);
            }
            if (t < 0) {
              t = t + 0xfff;
            }
            tween_apply((short *)guest_at((uint32_t)trk),from,t >> 0xc);
            return;
          }
          from = from + 1;
          t = t + 1;
          off = off + 0x14;
        } while (t <= *(short *)guest_at((uint32_t)(uintptr_t)((trk + 6))));
      }
    }
  }
  return;

}

void tween_seq_step(short *seq)
{
  short sVar1;
  byte *pbVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  int iVar6;
  int unaff_gp = (int)guest_gp();
  if ((((*(int *)guest_at((uint32_t)(uintptr_t)((unaff_gp + 0x44) ))!= 1) && (seq != (short *)0x0)) && (-1 < seq[0x22])) &&
     (seq[0x22] < 2)) {
    if (seq[5] < 0) {
      sVar1 = seq[0x1d] + 1;
LAB_801aa558:
      seq[0x1d] = sVar1;
LAB_801aa55c:
      do {
        pbVar2 = (byte *)guest_at((uint32_t)(uintptr_t)((seq[0x1d] * 2 + *(int *)guest_at((uint32_t)(uintptr_t)((seq[0x1c] * 4 + *(int *)(seq + 0x1a))))
                         )));
        bVar5 = *pbVar2;
        uVar4 = (int)*(short *)pbVar2 & 0xff00;
        if (uVar4 == 0x2000) {
          seq[0x22] = seq[0x22] | 0x100;
          return;
        }
        if (0x2000 < uVar4) {
          if (uVar4 == 0x4000) {
            seq[2] = -1;
            seq[0x22] = -1;
            return;
          }
          if (uVar4 == 0x8000) {
            seq[2] = -2;
            seq[0x22] = -1;
            return;
          }
LAB_801aa618:
          if (seq[0x22] == 0) {
LAB_801aa65c:
            if ((int)((uint)bVar5 << 0x18) < 0) {
              seq[0x1f] = -1;
              bVar5 = -bVar5;
            }
            else {
              seq[0x1f] = 1;
            }
          }
          else if (seq[0x22] == 1) {
            if ((int)(char)bVar5 != (int)seq[0x23]) goto LAB_801aa65c;
            seq[0x1d] = 0;
            seq[0x22] = 0;
            seq[0x1c] = seq[0x24];
            goto LAB_801aa55c;
          }
          seq[7] = (short)((char)bVar5 + -1);
          seq[0x1e] = (short)(char)bVar5;
          seq[5] = *(short *)guest_at((uint32_t)(uintptr_t)(((((char)bVar5 + -1) * 0x10000 >> 0x10) * 0x14 +
                                  *(int *)(seq + 0xc) + 4) ))+ -1;
          *(undefined4 *)(seq + 10) =
               *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0x10)));
          seq[8] = *(short *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc))));
          *(undefined4 *)(seq + 0x12) =
               *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 8)));
          *(undefined4 *)(seq + 0x14) =
               *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0xc)));
          if (seq[1] == 1) {
            /* guest div @0x801AA75C/0x801AA79C: num=0x1000, den=lh[p1+0xa]+1;
             * break @0x801AA768/@0x801AA7A8 (den==0) are recompiler no-ops
             * -> LO = den==0 ? -1 : 0x1000/den. C /0 would SIGFPE. */
            int32_t _den = (int32_t)seq[5] + 1;
            int32_t _q = (_den == 0) ? -1 : 0x1000 / _den;
            if (seq[0x1f] < 1) {
              seq[0x20] = 0x1000;
              sVar1 = -(short)_q;
            }
            else {
              sVar1 = (short)_q;
              seq[0x20] = 0;
            }
            seq[0x21] = sVar1;
            seq[0x20] = seq[0x20] + seq[0x21];
          }
          break;
        }
        if (uVar4 == 0x100) goto LAB_801aa600;
        if (uVar4 != 0x200) goto LAB_801aa618;
        seq[0x1c] = (short)(char)bVar5;
        seq[0x1d] = 0;
      } while( true );
    }
    if (seq[1] == 2) {
      if (seq[0x1f] < 1) {
        verts_sub_apply(*(undefined4 *)(seq + 0xe),(undefined4 *)guest_at(*(uint32_t *)(seq + 0x12)),
                     (ushort *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8]);
      }
      else {
        verts_add_apply(*(undefined4 *)(seq + 0xe),(undefined4 *)guest_at(*(uint32_t *)(seq + 0x12)),
                     (ushort *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8]);
      }
    }
    else {
      sVar1 = seq[0x20];
      if (sVar1 < 0) {
        seq[0x20] = 0;
        sVar1 = seq[0x20];
      }
      if (0x1000 < sVar1) {
        seq[0x20] = 0x1000;
      }
      if (*seq == 0) {
        verts_offset_apply((undefined4 *)guest_at(*(uint32_t *)(seq + 0x12)),(uint *)guest_at(*(uint32_t *)(seq + 0x14)),
                     *(undefined4 *)(seq + 0xe),(short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],
                     (int)seq[0x20]);
      }
      else if (*seq == 2) {
        iVar3 = seq[7] * 0x14 + *(int *)(seq + 0xc);
        if (*(int *)guest_at((uint32_t)(uintptr_t)((unaff_gp + 0x24) ))== 0) {
          iVar6 = *(int *)(seq + 0x10);
        }
        else {
          iVar6 = *(int *)(seq + 0x10) + seq[4] * 4;
        }
        verts_rgb_apply((uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar3 + 8)))),(uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0xc)))),iVar6,
                     (short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],(int)seq[0x20]);
      }
      seq[0x20] = seq[0x20] + seq[0x21];
    }
    seq[5] = seq[5] + -1;
  }
  return;
LAB_801aa600:
  sVar1 = (short)(char)bVar5;
  goto LAB_801aa558;

}

void prim_group_gc(int grp)
{
  short st;
  int ent;
  int off;
  int i;
  i = 0;
  if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x44)))) {
    off = 0;
    do {
      ent = *(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x34))))));
      if (ent != 0) {
        st = *(short *)guest_at((uint32_t)(uintptr_t)((ent + 4)));
        if (st == -2) {
          task_node_free((int *)(uintptr_t)*(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c)))))));
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c)))) ))= 0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x34)))) ))= 0;
        }
        else if (st == -1) goto LAB_801ac2ac;
      }
      ent = *(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x38))))));
      if (ent != 0) {
        st = *(short *)guest_at((uint32_t)(uintptr_t)((ent + 4)));
        if (st == -2) {
          task_node_free((int *)(uintptr_t)*(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x20)))))));
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x20)))) ))= 0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x38)))) ))= 0;
        }
        else if (st == -1) goto LAB_801ac2ac;
      }
      ent = *(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c))))));
      if (ent != 0) {
        st = *(short *)guest_at((uint32_t)(uintptr_t)((ent + 4)));
        if (st == -2) {
          task_node_free((int *)(uintptr_t)*(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))))));
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))) ))= 0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c)))) ))= 0;
        }
        else if (st == -1) {
LAB_801ac2ac:
          node_tree_free(*(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x14) ))+ 0x10))));
          return;
        }
      }
      i = i + 1;
      off = off + 4;
    } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x44))));
  }
  return;

}

void rect_prim_spawn(undefined2 a0,undefined2 a1,undefined2 a2,undefined2 a3, undefined1 arg5,undefined1 arg6,undefined2 arg7,undefined2 arg8, undefined4 arg9)
{
  int t;
  t = (int)(uintptr_t)task_node_alloc(arg9,2,0xe,0x801acdbc,0,0x20);
  if (t != 0) {
    t = *(int *)guest_at((uint32_t)(uintptr_t)((t + 0x10)));
    prim_packet_init(t + 0x14,0,0,arg7);
    prim_tag_set(t);
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((t + 8) ))= a0;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((t + 10) ))= a1;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((t + 0x10) ))= a2;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((t + 0x12) ))= a3;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((t + 0xc) ))= arg5;
    *(undefined1 *)guest_at((uint32_t)(uintptr_t)((t + 0xd) ))= arg6;
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((t + 0xe) ))= arg8;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)((t + 0x1c) ))= 1;
    *(byte *)guest_at((uint32_t)(uintptr_t)((t + 7) ))= *(byte *)guest_at((uint32_t)(uintptr_t)((t + 7) ))| 3;
  }
  return;

}

/* ---- batch19: task/effect node ops ---- */
undefined4 isqrt_refine(int n)
{
  /* Faithful port of guest 0x80194280: a restoring digit-recurrence integer
   * square-root.  Two recurrences run in parallel — g[] (the root digits)
   * and p[] (the residual/partner) — seeded at param +/- 0x5d50ad, refined
   * for i = 1..6.  The i == 4 step uses a wider cross-term refinement.
   * g[i] and p[i] map to the guest stack slots sp+0x04.. and sp+0x24.. . */
  int g[7];
  int p[7];
  int i;
  g[0] = n + 0x5d50ad;
  p[0] = n - 0x5d50ad;
  for (i = 1; i < 7; i = i + 1) {
    if (i == 4) {
      int n = p[3];
      int v0 = n >> 4;
      int v1 = g[3];
      int a1 = v1 - v0;
      v0 = v1 >> 4;
      v1 = n - v0;
      g[3] = a1;
      p[3] = v1;
      if (v1 < 0) {
        g[4] = a1 + (v1 >> 4);
        p[4] = v1 + (a1 >> 4);
      }
      else {
        g[4] = a1 - (v1 >> 4);
        p[4] = v1 - (a1 >> 4);
      }
    }
    else {
      int pv = p[i - 1];
      if (pv < 0) {
        g[i] = g[i - 1] + (pv >> i);
        p[i] = pv + (g[i - 1] >> i);
      }
      else {
        g[i] = g[i - 1] - (pv >> i);
        p[i] = pv - (g[i - 1] >> i);
      }
    }
  }
  return (undefined4)g[6];

}

undefined4 cop2_clz(undefined4 v)
{
  undefined4 res;
  setCopReg(2,0xf000,v);
  res = getCopReg(2,0xf800);
  return res;

}

void gte_points_xform(undefined4 *a0,undefined4 *a1,undefined2 *a2,undefined2 *a3, undefined2 *arg5,int arg6)
{
  undefined4 in_zero;
  undefined4 in_at;
  uint code;
  undefined4 t0;
  undefined4 t1;
  do {
    setCopReg(2,in_zero,*a0);
    setCopReg(2,in_at,a0[1]);
    copFunction(2,0x180001);
    a0 = a0 + 2;
    arg6 = arg6 + -1;
    t0 = getCopReg(2,0xe);
    *a1 = t0;
    t0 = getCopReg(2,0x9800);
    t1 = getCopReg(2,0x4000);
    code = getCopControlWord(2,0xf800);
    *a2 = (short)t0;
    *a3 = (short)t1;
    *arg5 = (short)(code >> 0xc);
    a1 = a1 + 1;
    a2 = a2 + 1;
    a3 = a3 + 1;
    arg5 = arg5 + 1;
  } while (0 < arg6);
  return;

}

int tween_track_alloc(int a0,undefined2 a1,undefined4 a2,undefined2 *a3,short arg5 ,undefined2 arg6,undefined2 arg7,undefined4 arg8)
{
  undefined2 fld0;
  int node;
  int ret;
  undefined2 *pay;
  node = (int)(uintptr_t)task_node_alloc(arg8,2,3,0x801a972c,0,0x4c);
  ret = 0;
  if (node != 0) {
    pay = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = (short)a0;
    pay[1] = a1;
    pay[2] = 0;
    pay[3] = arg5 + -1;
    pay[4] = arg6;
    pay[6] = arg7;
    if (a0 == 0) {
      *(undefined4 *)(pay + 0xe) = a2;
    }
    else {
      *(undefined4 *)(pay + 0x10) = a2;
    }
    /* a3 is a GUEST address (keyframe descriptor); resolve fields via
     * guest_at and store it as a 32-bit guest pointer, not a host ptr.
     * It may arrive as a guest address in pointer costume (clean callers)
     * or as a host pointer (clean_table wrap); br2_gaddr yields the
     * canonical guest address for both. */
    uint32_t p4g = br2_gaddr(a3);
    *(uint32_t *)((char *)pay + 0x18) = p4g;
    *(undefined4 *)((char *)pay + 0x24) = *(uint32_t *)guest_at(p4g + 8);
    *(undefined4 *)((char *)pay + 0x28) = *(uint32_t *)guest_at(p4g + 12);
    *(undefined4 *)((char *)pay + 0x14) = *(uint32_t *)guest_at(p4g + 16);
    fld0 = *(undefined2 *)guest_at(p4g);
    pay[5] = 0;
    pay[7] = 0;
    pay[8] = fld0;
    *(int *)(pay + 0x16) = (int)(short)pay[5];
    *(int *)(pay + 0x18) = (int)(short)pay[7];
    ret = node;
  }
  return ret;

}

void tween_apply(short *seq,int v0,int v1)
{
  short sVar1;
  short sVar2;
  ushort uVar3;
  bool bVar4;
  int iVar5;
  short *psVar6;
  short *psVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  uint in_t4;
  int in_t6;
  short sVar11;
  int unaff_gp = (int)guest_gp();
  int local_18 [4];
  if (seq == (short *)0x0) {
    return;
  }
  sVar11 = (short)v1;
  if (seq[1] == 1) {
    if (((*seq == 0) && (seq[7] == v0)) && (seq[5] == v1)) {
      return;
    }
    seq[7] = (short)v0;
    seq[5] = sVar11;
    *(int *)(seq + 0x16) = v1;
    *(int *)(seq + 0x18) = v0;
    *(undefined4 *)(seq + 10) =
         *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0x10)));
    seq[8] = *(short *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc))));
    if (*seq == 0) {
      *(undefined4 *)(seq + 0x12) =
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 8)));
      *(undefined4 *)(seq + 0x14) =
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0xc)));
      verts_offset_apply((undefined4 *)guest_at(*(uint32_t *)(seq + 0x12)),(uint *)guest_at(*(uint32_t *)(seq + 0x14)),
                   *(undefined4 *)(seq + 0xe),(short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],
                   v1);
      return;
    }
    if (*seq != 2) {
      return;
    }
    iVar5 = seq[7] * 0x14 + *(int *)(seq + 0xc);
    if (*(int *)guest_at((uint32_t)(uintptr_t)((unaff_gp + 0x24) ))== 0) {
      iVar10 = *(int *)(seq + 0x10);
    }
    else {
      iVar10 = *(int *)(seq + 0x10) + seq[4] * 4;
    }
    verts_rgb_apply((uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar5 + 8)))),(uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar5 + 0xc)))),iVar10,
                 (short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],v1);
    return;
  }
  if (seq[1] != 2) {
    return;
  }
  if ((seq[7] == v0) && (seq[5] == v1)) {
    return;
  }
  local_18[1] = 0;
  iVar5 = (int)seq[7];
  if (iVar5 < v0) {
    in_t6 = 1;
    in_t4 = 0;
    local_18[2] = v1;
    local_18[0] = *(int *)guest_at((uint32_t)(uintptr_t)((iVar5 * 0x14 + *(int *)(seq + 0xc) + 4) ))- (int)seq[5];
  }
  else {
    if (v0 < iVar5) {
      in_t6 = -1;
      local_18[0] = -(int)seq[5];
      iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((v0 * 0x14 + *(int *)(seq + 0xc) + 4)));
      in_t4 = 0;
    }
    else {
      bVar4 = (int)in_t4 < 3;
      if (iVar5 != v0) goto LAB_801a9d14;
      in_t6 = 0;
      iVar5 = (int)seq[5];
      in_t4 = 2;
    }
    local_18[2] = v1 - iVar5;
  }
  bVar4 = in_t4 < 3;
LAB_801a9d14:
  if (bVar4) {
    do {
      iVar5 = 0;
      if (0 < seq[8]) {
        iVar10 = 0;
        do {
          iVar9 = local_18[in_t4];
          psVar6 = (short *)guest_at((uint32_t)(uintptr_t)((iVar5 * 8 + *(int *)(seq + 0x12))));
          sVar1 = psVar6[1];
          sVar2 = psVar6[2];
          if (*psVar6 * iVar9 != 0) {
            psVar7 = (short *)guest_at((uint32_t)(uintptr_t)((*(short *)guest_at((uint32_t)(uintptr_t)((iVar10 + *(int *)(seq + 10)) ))* 8 +
                              *(int *)(seq + 0xe))));
            *psVar7 = *psVar7 + (short)(*psVar6 * iVar9);
          }
          if (sVar1 * iVar9 != 0) {
            iVar8 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar10 + *(int *)(seq + 10)) ))* 8 + *(int *)(seq + 0xe);
            *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 2) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 2) ))+ (short)(sVar1 * iVar9);
          }
          if (sVar2 * iVar9 != 0) {
            iVar8 = *(short *)guest_at((uint32_t)(uintptr_t)((iVar10 + *(int *)(seq + 10)) ))* 8 + *(int *)(seq + 0xe);
            *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((iVar8 + 4) ))+ (short)(sVar2 * iVar9);
          }
          iVar5 = iVar5 + 1;
          iVar10 = iVar10 + 2;
        } while (iVar5 < seq[8]);
      }
      if (in_t4 == 1) {
        uVar3 = seq[7];
        seq[7] = (short)((uint)uVar3 + in_t6);
        iVar5 = (int)(((uint)uVar3 + in_t6) * 0x10000) >> 0x10;
LAB_801a9ec0:
        if (iVar5 == v0) {
          in_t4 = 2;
        }
      }
      else if ((int)in_t4 < 2) {
        if (in_t4 == 0) {
          uVar3 = seq[7];
          seq[7] = (short)((uint)uVar3 + in_t6);
          local_18[1] = *(int *)guest_at((uint32_t)(uintptr_t)((((int)(((uint)uVar3 + in_t6) * 0x10000) >> 0x10) * 0x14 +
                                 *(int *)(seq + 0xc) + 4) ))* in_t6;
          iVar5 = (int)seq[7];
          in_t4 = 1;
          goto LAB_801a9ec0;
        }
      }
      else if (in_t4 == 2) {
        in_t4 = 3;
      }
      *(undefined4 *)(seq + 0x12) =
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 8)));
      *(undefined4 *)(seq + 10) =
           *(undefined4 *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0x10)));
      seq[8] = *(short *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc))));
    } while ((int)in_t4 < 3);
  }
  seq[5] = sVar11;
  *(int *)(seq + 0x16) = (int)sVar11;
  *(int *)(seq + 0x18) = (int)seq[7];
  return;

}

void rect_prim_tick(uint *prim)
{
  uint cnt;
  uint *cur;
  int gp = (int)guest_gp();
  cnt = prim[7];
  prim[7] = cnt - 1;
  if ((int)(cnt - 1) < 0) {
    task_slot_step();
  }
  else {
    *prim = *(uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp + 0x48) )))| 0x4000000;
    *(uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp + 0x48) )))= guest_of(prim) & 0xffffff;
    cur = (uint *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp + 0x48))));
    prim[5] = *cur | 0x1000000;
    *cur = guest_of(prim + 5) & 0xffffff;
  }
  return;

}

void prim_tag_set(int prim)
{
  *(undefined1 *)guest_at((uint32_t)(uintptr_t)((prim + 3) ))= 4;
  *(undefined1 *)guest_at((uint32_t)(uintptr_t)((prim + 7) ))= 100;
  return;

}

void prim_packet_init(int pkt,int a1,int a2,uint fl)
{
  uint tagw;
  *(undefined1 *)guest_at((uint32_t)(uintptr_t)((pkt + 3) ))= 1;
  tagw = 0xe1000000;
  if (a2 != 0) {
    tagw = 0xe1000200;
  }
  fl = fl & 0x9ff;
  if (a1 != 0) {
    fl = fl | 0x400;
  }
  *(uint *)guest_at((uint32_t)(uintptr_t)((pkt + 4) ))= tagw | fl;
  return;

}

void verts_offset_apply(undefined4 *v,uint *c,int n,short *off,int cnt, undefined4 fl)
{
  uint *cz;
  short t;
  short *vp;
  uint xy;
  undefined4 tx;
  undefined4 ty;
  undefined4 tz;
  undefined4 cur;
  setCopReg(2,0x4000,fl);
  if (cnt != 0) {
    do {
      xy = *c;
      cnt = cnt + -1;
      cz = c + 1;
      c = c + 2;
      setCopReg(2,0x4800,xy & 0xffff);
      setCopReg(2,0x5000,xy >> 0x10);
      cur = *v;
      setCopReg(2,0x5800,(int)(short)*cz);
      copFunction(2,0x98003d);
      t = *off;
      off = off + 1;
      vp = (short *)guest_at((uint32_t)(uintptr_t)((t * 8 + n)));
      tx = getCopReg(2,0xc800);
      t = *(short *)(v + 1);
      v = v + 2;
      ty = getCopReg(2,0xd000);
      tz = getCopReg(2,0xd800);
      *vp = (short)cur + (short)tx;
      vp[1] = (short)((uint)cur >> 0x10) + (short)ty;
      vp[2] = t + (short)tz;
    } while (0 < cnt);
  }
  return;

}

void verts_rgb_apply(uint *v,uint *c,int n,short *off,int cnt, undefined4 fl)
{
  short vi;
  char *vp;
  uint cur;
  uint tgt;
  undefined4 dr;
  undefined4 dg;
  undefined4 db;
  setCopReg(2,0x4000,fl);
  if (cnt != 0) {
    do {
      cur = *v;
      v = v + 1;
      tgt = *c;
      c = c + 1;
      vi = *off;
      cnt = cnt + -1;
      setCopReg(2,0x4800,(tgt & 0xff) - (cur & 0xff));
      setCopReg(2,0x5000,(tgt >> 8 & 0xff) - (cur >> 8 & 0xff));
      setCopReg(2,0x5800,(tgt >> 0x10 & 0xff) - (cur >> 0x10 & 0xff));
      off = off + 1;
      copFunction(2,0x98003d);
      vp = (char *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((vi * 4 + n))));
      dr = getCopReg(2,0xc800);
      dg = getCopReg(2,0xd000);
      db = getCopReg(2,0xd800);
      *vp = (char)cur + (char)dr;
      vp[1] = (char)(cur >> 8) + (char)dg;
      vp[2] = (char)(cur >> 0x10) + (char)db;
    } while (0 < cnt);
  }
  return;

}

void verts_add_apply(int n,undefined4 *v,ushort *c,int d)
{
  short dz;
  ushort vi;
  undefined4 cur;
  undefined4 dlt;
  short *vp;
  if (d != 0) {
    do {
      d = d + -1;
      vi = *c;
      c = c + 1;
      vp = (short *)guest_at((uint32_t)(uintptr_t)(((uint)vi * 8 + n)));
      cur = *(undefined4 *)vp;
      dlt = *v;
      dz = *(short *)(v + 1);
      v = v + 2;
      *vp = (short)cur + (short)dlt;
      vp[1] = (short)((uint)cur >> 0x10) + (short)((uint)dlt >> 0x10);
      vp[2] = vp[2] + dz;
    } while (0 < d);
  }
  return;

}

void verts_sub_apply(int n,undefined4 *v,ushort *c,int d)
{
  short dz;
  ushort vi;
  undefined4 cur;
  undefined4 dlt;
  short *vp;
  if (d != 0) {
    do {
      d = d + -1;
      vi = *c;
      c = c + 1;
      vp = (short *)guest_at((uint32_t)(uintptr_t)(((uint)vi * 8 + n)));
      cur = *(undefined4 *)vp;
      dlt = *v;
      dz = *(short *)(v + 1);
      v = v + 2;
      *vp = (short)cur - (short)dlt;
      vp[1] = (short)((uint)cur >> 0x10) - (short)((uint)dlt >> 0x10);
      vp[2] = vp[2] - dz;
    } while (0 < d);
  }
  return;

}

/* ---- batch19b: fixed_point_divide manual port (capstone) ---- */

/* fixed_point_divide — capstone-verified fixed-point divide (2-pass Newton refinement).
   div= signed 32/32 mflo, multu= unsigned 32x32 -> mfhi:mflo. */
int fixed_point_divide(int num,int den)
{
  uint32_t v0 = num < 0 ? (uint32_t)-num : (uint32_t)num;
  uint32_t v1 = den < 0 ? (uint32_t)-den : (uint32_t)den;
  uint32_t t2 = 2;
  uint32_t t3;
  if (v0 < v1) {
    uint32_t t = v0; v0 = v1; v1 = t;
  }
  if (v1 == 0) {
    return (int)v0;
  }
  t3 = v0 << 0xf;
  v1 = v1 << 0xf;
  do {
    uint32_t t0 = (uint32_t)((int32_t)v1 / (int32_t)v0);
    uint64_t pq = (uint64_t)t0 * t0;
    uint32_t lo = (uint32_t)pq;
    uint32_t t1 = (lo >> 0xf) + 0x20000;
    uint64_t p1, p2;
    t0 = (uint32_t)((int32_t)lo / (int32_t)t1);
    p1 = (uint64_t)((t3 << 1) & 0xffffffffU) * t0;
    t1 = ((uint32_t)(p1 >> 32) << 0x11) | ((uint32_t)p1 >> 0xf);
    t3 = t3 + t1;
    v0 = t3 >> 0xf;
    p2 = (uint64_t)v1 * t0;
    v1 = (((uint32_t)(p2 >> 32) << 0x11)) | ((uint32_t)p2 >> 0xf);
    t2 = t2 - 1;
  } while (0 < (int32_t)t2);
  return (int)v0;
}

/* ---- batch19c: keyframe_cursor_search manual port (capstone) ---- */

/* keyframe_cursor_search — capstone-verified keyframe cursor search.
   node: +0x6 lh count, +0xa/+0xe lh cursors, +0x18 table, +0x2c lw total.
   entries stride 0x14, field +4 = frame time. Ends: tween_apply(node,idx,acc)
   then lhu(+0x2c)->+0xa. */
void keyframe_cursor_search(uint *cur)
{
  int unaff_gp = (int)guest_gp();
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  uint *puVar7;
  int iVar8;

  if (*(int *)guest_at((uint32_t)(uintptr_t)((unaff_gp + 0x44) ))== 1) {
    return;
  }
  if (cur == (uint *)0x0) {
    return;
  }
  pbVar6 = (byte *)cur;
  iVar1 = (int)*(short *)(pbVar6 + 0xa);
  iVar2 = *(int *)(pbVar6 + 0x2c);
  if (iVar2 == iVar1) {
    return;
  }
  iVar4 = 1;
  iVar5 = (int)*(short *)(pbVar6 + 0xe);
  iVar3 = 0;
  if (iVar1 < iVar2) {
    iVar4 = -1;
  }
  iVar1 = iVar2 - iVar1;
  if (iVar1 < 0) {
LAB_801a9784:
    iVar1 = (int)*(short *)(pbVar6 + 0xa) - *(int *)(pbVar6 + 0x2c);
    if (iVar3 >= iVar1) {
      goto LAB_801a9830;
    }
  }
  else if (iVar3 >= iVar1) {
    goto LAB_801a9830;
  }
  for (;;) {
LAB_801a97a0:
    puVar7 = (uint *)guest_at(*(uint32_t *)(pbVar6 + 0x18));
    iVar8 = *(int *)(puVar7 + iVar5 * 5 + 1);
    iVar2 = iVar2 + iVar4;
    if (iVar8 < iVar2) {
      if (0 <= iVar2) {
        goto LAB_801a9810;
      }
      iVar5 = iVar5 + -1;
      puVar7 = (uint *)guest_at(*(uint32_t *)(pbVar6 + 0x18));
      iVar8 = *(int *)(puVar7 + iVar5 * 5 + 1);
      iVar2 = iVar8 + -1;
      if (iVar5 < 0) {
        iVar5 = 0;
        iVar2 = 0;
        goto LAB_801a9830;
      }
      goto LAB_801a9810;
    }
    iVar5 = iVar5 + 1;
    iVar2 = 1;
    if ((int)*(short *)(pbVar6 + 6) <= iVar5) {
      iVar5 = (int)*(short *)(pbVar6 + 6);
      iVar2 = 0;
      goto LAB_801a9830;
    }
LAB_801a9810:
    iVar1 = *(int *)(pbVar6 + 0x2c) - (int)*(short *)(pbVar6 + 0xa);
    iVar3 = iVar3 + 1;
    if (iVar1 < 0) {
      goto LAB_801a9784;
    }
    if (iVar3 < iVar1) {
      continue;
    }
    break;
  }
LAB_801a9830:
  *(undefined2 *)(pbVar6 + 0xa) = *(undefined2 *)(pbVar6 + 0x2c);
  tween_apply((short *)cur,iVar5,iVar2);
  return;
}
/* globals/callees for batch24 ports */
extern uint8_t  *g_DAT_1f8001f3;
#define DAT_1f8001f3   (*g_DAT_1f8001f3)
extern uint32_t *g__DAT_801c4494,*g__DAT_801c44e4;
#define _DAT_801c4494  (*g__DAT_801c4494)
#define _DAT_801c44e4  (*g__DAT_801c44e4)
extern uint8_t  *g_PTR_DAT_801b1ec0;
extern uint8_t  *g_DAT_801b1eac;
#define DAT_801b1eac   (*g_DAT_801b1eac)
extern undefined **g_DAT_801b22ec;
#define PTR_DAT_801b22ec (*g_DAT_801b22ec)
extern uint16_t *g_DAT_801b2308,*g_DAT_801b2314;
#define DAT_801b2308   (*g_DAT_801b2308)
#define DAT_801b2314   (*g_DAT_801b2314)
extern uint32_t *g__DAT_801b22f8,*g__DAT_801b22fc,*g__DAT_801b2304;
#define DAT_801b22f8   (*g__DAT_801b22f8)
#define DAT_801b22fc   (*g__DAT_801b22fc)
#define DAT_801b2304   (*g__DAT_801b2304)
extern int16_t  *g_DAT_801b8614;
#define DAT_801b8614   (*g_DAT_801b8614)
extern uint8_t  *g_DAT_801b180c;
#define DAT_801b180c   (*g_DAT_801b180c)
extern uint32_t *g_PTR_DAT_801b1d44;
#define PTR_DAT_801b1d44 ((undefined4)(uintptr_t)g_PTR_DAT_801b1d44)
extern void *g_PTR_FUN_801b1ec0;
#define PTR_FUN_801b1ec0 (*g_PTR_FUN_801b1ec0)
extern void motion_integrator_tail();
extern void keyframe_advance();
extern void spark_trail_task_update();
extern void confetti_burst_update();
extern void LAB_8013e558(undefined4 *cur);
extern void LAB_80140510(int *cur);
extern uint8_t *g_DAT_801b180c;
#define DAT_801b180c (*g_DAT_801b180c)
extern void ring_effect_task_update();
extern void firework_task_update(int);

/* guest popup_spawn @0x8013ab8c */
void popup_spawn(int a0,uint a1,uint a2)

{
  int iVar1;
  int *piVar2;
  undefined1 *puVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  
  iVar4 = 0;
  puVar3 = &DAT_1f8000e8;
  while (((*(uint *)(puVar3 + 0x58) & 0xc0000000) == 0 || (puVar3[5] != '\0'))) {
    iVar4 = iVar4 + 1;
    puVar3 = puVar3 + 0x108;
    if (1 < iVar4) {
      iVar1 = (int)(uintptr_t)task_node_alloc(1,2,10,0x8013a9d8,0,0x1c);
      iVar4 = _DAT_801c4578;
      if (iVar1 != 0) {
        piVar5 = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar1 + 0x10))));
        iVar1 = 0;
        if (DAT_1f8000eb == a1) {
          iVar1 = _DAT_801c4494;
        }
        if (DAT_1f8001f3 == a1) {
          iVar1 = _DAT_801c44e4;
        }
        if (iVar1 == 0) {
          task_node_free(0);
        }
        else {
          piVar2 = (int *)guest_at((uint32_t)(uintptr_t)(((a2 & 0x7f) * 0x18 + iVar1)));
          *piVar5 = a0;
          iVar1 = piVar2[1];
          iVar6 = piVar2[2];
          iVar7 = piVar2[3];
          piVar5[1] = *piVar2;
          piVar5[2] = iVar1;
          piVar5[3] = iVar6;
          piVar5[4] = iVar7;
          iVar1 = piVar2[5];
          piVar5[5] = piVar2[4];
          piVar5[6] = iVar1;
          *(undefined2 *)((intptr_t)piVar5 + 6) = 0;
          if (((iVar4 == 0) || (iVar4 == 3)) && (0x2d < (short)piVar5[3])) {
            *(undefined2 *)(piVar5 + 3) = 0x2d;
          }
          if (400 < (short)piVar5[6]) {
            *(undefined2 *)(piVar5 + 6) = 400;
          }
          if ((*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*piVar5 + 0x38) ))+ 0x100) ))!= '\0') &&
             (*(ushort *)((intptr_t)piVar5 + 10) =
                   (ushort)(byte)(&DAT_801b180c)[*(short *)((intptr_t)piVar5 + 10)],
             (a2 & 0x80) != 0)) {
            *(short *)(piVar5 + 4) = 0x1000 - (short)piVar5[4];
          }
          *(short *)(piVar5 + 5) = (short)piVar5[5] + -0x400;
          *(short *)(piVar5 + 4) = (short)piVar5[4] + -0x400;
        }
      }
      return;
    }
  }
  return;
}

/* guest confetti_burst_spawn @0x8013da2c */
void confetti_burst_spawn(undefined4 a0,undefined4 a1)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  int iVar9;
  undefined4 *puVar10;
  
  iVar4 = (int)(uintptr_t)task_node_alloc(1,2,0xb,0x8013d7e4,0,0x598);
  iVar9 = 0;
  if (iVar4 != 0) {
    puVar10 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x10))));
    puVar10[0x155] = 0x10;
    puVar10[0x152] = a0;
    puVar10[0x154] = a1;
    uVar1 = rand_lcg();
    uVar2 = rand_lcg();
    puVar7 = puVar10;
    puVar8 = puVar10;
    do {
      uVar1 = uVar1 + 99;
      uVar2 = uVar2 + 0x85;
      *(ushort *)(puVar7 + 0x140) = (uVar1 & 0x1ff) - 0x100;
      *(ushort *)((intptr_t)puVar7 + 0x502) = (uVar2 & 0x1ff) - 0x100;
      *(undefined2 *)(puVar8 + 0x156) = 0x1000;
      uVar3 = rand_lcg();
      *(ushort *)(puVar8 + 0x15e) = (uVar3 & 0x3f) + 10;
      puVar8 = (undefined4 *)((intptr_t)puVar8 + 2);
      iVar9 = iVar9 + 1;
      puVar7 = puVar7 + 1;
    } while (iVar9 < 0x10);
    iVar9 = 0;
    iVar4 = 0;
    puVar7 = puVar10;
    do {
      iVar6 = 0;
      puVar8 = puVar7;
      iVar5 = iVar4;
      do {
        *puVar8 = 0x9000000;
        *(undefined1 *)((intptr_t)puVar8 + 7) = 0x2c;
        *(undefined2 *)((intptr_t)puVar10 + iVar5 + 0x16) = 0x3f;
        *(undefined2 *)((intptr_t)puVar10 + iVar5 + 0xe) = 31000;
        iVar6 = iVar6 + 1;
        *(undefined1 *)(puVar8 + 3) = 0x40;
        *(undefined1 *)((intptr_t)puVar8 + 0xd) = 0x80;
        *(undefined1 *)(puVar8 + 5) = 0x48;
        *(undefined1 *)((intptr_t)puVar8 + 0x15) = 0x80;
        *(undefined1 *)(puVar8 + 7) = 0x40;
        *(undefined1 *)((intptr_t)puVar8 + 0x1d) = 0x88;
        *(undefined1 *)(puVar8 + 9) = 0x48;
        *(undefined1 *)((intptr_t)puVar8 + 0x25) = 0x88;
        *(undefined1 *)(puVar8 + 1) = 0x7f;
        *(undefined1 *)((intptr_t)puVar8 + 5) = 0x7f;
        *(undefined1 *)((intptr_t)puVar8 + 6) = 0x7f;
        *(byte *)((intptr_t)puVar8 + 7) = *(byte *)((intptr_t)puVar8 + 7) & 0xfe | 2;
        puVar8 = puVar8 + 10;
        iVar5 = iVar5 + 0x28;
      } while (iVar6 < 0x10);
      puVar7 = puVar7 + 0xa0;
      iVar9 = iVar9 + 1;
      iVar4 = iVar4 + 0x280;
    } while (iVar9 < 2);
  }
  return;
}

/* guest speech_bubble_spawn @0x8013f178 */
void speech_bubble_spawn(undefined4 *a0,short a1,int a2,ushort a3)

{
  short *psVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  
  if (((DAT_1f8000b1 == '\0') && (_DAT_801f0930 < 3)) &&
     ((*(ushort *)(_g_pad_buffer + 2) >> 4 & 1) != 0)) {
    if ((DAT_1f800098 != '\x01') && ((a3 & 0xf0) == 0)) {
      psVar1 = (short *)guest_at((uint32_t)(uintptr_t)(fx_record_alloc((undefined4)br2_gaddr(a0),6,0x8000000a,2,PTR_DAT_801b1d44)));
      if (psVar1 != (short *)0x0) {
        *psVar1 = a1 + 0x800;
      }
    }
    iVar2 = (int)(uintptr_t)task_node_alloc(1,2,5,0x8013e558,0,0x48);
    if (iVar2 != 0) {
      piVar5 = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar2 + 0x10))));
      iVar3 = (int)(uintptr_t)prim_group_build((undefined4)br2_gaddr(piVar5),*(undefined4 *)(&DAT_801c43a8 + a2 * 4),0,1);
      *piVar5 = (int)(uintptr_t)iVar3;
      if (iVar3 != 0) {
        if ((a3 & 0xf) != 0) {
          prim_attr_word_set(iVar3,(a3 & 0xf) << 6 | 0x781a);
        }
        iVar3 = tween_ensure(*piVar5);
        if (iVar3 != 0) {
          *(undefined2 *)((intptr_t)piVar5 + 0x46) = 0;
          iVar3 = DAT_801b1848;
          iVar2 = DAT_801b1844;
          piVar5[1] = DAT_801b1840;
          piVar5[2] = iVar2;
          piVar5[3] = iVar3;
          iVar3 = DAT_801b1854;
          iVar2 = DAT_801b1850;
          piVar5[4] = DAT_801b184c;
          piVar5[5] = iVar2;
          piVar5[6] = iVar3;
          iVar2 = DAT_801b185c;
          piVar5[7] = DAT_801b1858;
          piVar5[8] = iVar2;
          *(undefined2 *)(piVar5 + 0x11) = 0x138;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*piVar5 ))+ 0x14) ))= *a0;
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*piVar5 ))+ 0x18) ))= a0[1];
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*piVar5 ))+ 0x1c) ))= a0[2];
          iVar2 = angle_from_xy(*a0,a0[2]);
          uVar4 = (int)(iVar2 + 0x200U & 0xfff) >> 10;
          if (uVar4 == 1) {
            iVar2 = 0x400;
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*piVar5 ))+ 0x14) ))= (int)_DAT_801f093a;
          }
          else if (uVar4 < 2) {
            if (uVar4 == 0) {
              iVar2 = 0x800;
              *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*piVar5 ))+ 0x1c) ))= -(int)_DAT_801f093e;
            }
          }
          else if (uVar4 == 2) {
            iVar2 = 0;
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*piVar5 ))+ 0x1c) ))= (int)_DAT_801f093e;
          }
          else if (uVar4 == 3) {
            iVar2 = 0xc00;
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*piVar5 ))+ 0x14) ))= -(int)_DAT_801f093a;
          }
          rotmat_rtps_build(iVar2,piVar5 + 9);
          _DAT_801f0930 = _DAT_801f0930 + 1;
          return;
        }
        node_tree_free(*piVar5);
      }
      task_node_free((int *)(uintptr_t)iVar2);
    }
  }
  return;
}

/* guest effect_arg_set @0x8013f5f8 */
void effect_arg_set(undefined4 arg)

{
  undefined4 *puVar1;
  
  if ((DAT_1f8000b1 == '\0') &&
     (puVar1 = (undefined4 *)guest_at((uint32_t)(uintptr_t)(task_alloc_wrap_e(4,(int)br2_gaddr(g_PTR_FUN_801b1ec0),(int)guest_of(&DAT_801b1eac)))),
     puVar1 != (undefined4 *)0x0)) {
    *puVar1 = arg;
  }
  return;
}

/* guest hit_spark_spawn @0x80140710 */
void hit_spark_spawn(undefined4 pos,uint a1,undefined2 a2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  
  if ((DAT_1f8000b1 == '\0') && ((a1 & 0x7f) < 3)) {
    iVar1 = (int)(uintptr_t)task_node_alloc(1,2,8,0x80140510,0,0x14);
    if (iVar1 != 0) {
      puVar2 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar1 + 0x10))));
      /* guest: lw v1, (0x801B22EC + (a1&0x7f)*4) — u32 table of guest
       * data addrs. g_DAT_801b22ec is undefined** so [&PTR][i] reads 8
       * bytes at i*8; index as u32 instead. */
      puVar3 = (undefined *)guest_at(((uint32_t *)&PTR_DAT_801b22ec)[a1 & 0x7f]);
      *puVar2 = (undefined4)br2_gaddr(puVar3);
      puVar2[1] = (undefined4)br2_gaddr(puVar3);
      *(undefined2 *)(puVar2 + 3) = 0;
      *(undefined2 *)((intptr_t)puVar2 + 0xe) = 0;
      puVar2[2] = pos;
      *(ushort *)(puVar2 + 4) = (ushort)a1 & 0x80;
      *(undefined2 *)((intptr_t)puVar2 + 0x12) = a2;
    }
  }
  return;
}

/* guest sprite_burst_spawn @0x80140b18 */
void sprite_burst_spawn(undefined4 a0,undefined4 a1)

{
  undefined2 uVar1;
  short sVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined2 *puVar6;
  undefined2 *puVar7;
  int iVar8;
  undefined4 *puVar9;
  
  if (DAT_1f8000b1 == '\0') {
    iVar3 = (int)(uintptr_t)task_node_alloc(1,2,0xe,0x801407cc,0,400);
    iVar8 = 0;
    if (iVar3 != 0) {
      puVar9 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar3 + 0x10))));
      puVar9[0x58] = a0;
      puVar9[0x59] = a1;
      puVar9[0x5a] = 0x14;
      sVar2 = rand_lcg();
      puVar7 = &DAT_801b2314;
      puVar6 = &DAT_801b2308;
      puVar4 = puVar9;
      do {
        uVar1 = *puVar6;
        puVar6 = puVar6 + 1;
        *(undefined2 *)(puVar4 + 0x5e) = uVar1;
        uVar1 = *puVar7;
        iVar8 = iVar8 + 1;
        *(short *)(puVar4 + 0x5b) = sVar2;
        sVar2 = sVar2 + 0x2aa;
        puVar7 = puVar7 + 1;
        *(undefined2 *)(puVar4 + 0x61) = uVar1;
        puVar4 = (undefined4 *)((intptr_t)puVar4 + 2);
      } while (iVar8 < 6);
      iVar3 = 0;
      puVar4 = puVar9;
      do {
        iVar8 = 0;
        puVar5 = puVar4;
        do {
          puVar5[1] = DAT_801b22f8;
          puVar5[3] = DAT_801b22fc;
          puVar5[5] = DAT_801b2304;
          *puVar5 = 0x6000000;
          *(undefined1 *)((intptr_t)puVar5 + 7) = 0x32;
          iVar8 = iVar8 + 1;
          puVar5 = puVar5 + 7;
        } while (iVar8 < 6);
        puVar9[0x54] = 0x1000000;
        puVar9[0x55] = 0xe1000020;
        puVar9 = puVar9 + 2;
        iVar3 = iVar3 + 1;
        puVar4 = puVar4 + 0x2a;
      } while (iVar3 < 2);
    }
  }
  return;
}

/* guest firework_spawn @0x80141050 */
void firework_spawn(undefined2 *pos)

{
  ushort uVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  
  iVar2 = (int)(uintptr_t)(int)(uintptr_t)task_node_alloc(1,2,0xb,0x80140d38,0,0x3d8);
  iVar6 = 0;
  if (iVar2 != 0) {
    iVar4 = 0;
    puVar5 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iVar2 + 0x10))));
    *(undefined2 *)(puVar5 + 0xf5) = 0x1a;
    puVar8 = puVar5;
    do {
      iVar7 = 0;
      puVar3 = puVar8;
      iVar2 = iVar4;
      do {
        *puVar3 = 0x9000000;
        *(undefined1 *)((intptr_t)puVar3 + 7) = 0x2e;
        *(undefined2 *)((intptr_t)puVar5 + iVar2 + 0x16) = 0x39;
        *(undefined2 *)((intptr_t)puVar5 + iVar2 + 0xe) = 0x789a;
        *(undefined1 *)(puVar3 + 3) = 0xc0;
        *(undefined1 *)((intptr_t)puVar3 + 0xd) = 0x20;
        *(undefined1 *)(puVar3 + 5) = 0xdf;
        *(undefined1 *)((intptr_t)puVar3 + 0x15) = 0x20;
        *(undefined1 *)(puVar3 + 7) = 0xc0;
        *(undefined1 *)((intptr_t)puVar3 + 0x1d) = 0x3f;
        *(undefined1 *)(puVar3 + 9) = 0xdf;
        *(undefined1 *)((intptr_t)puVar3 + 0x25) = 0x3f;
        *(undefined1 *)(puVar3 + 1) = 0xa2;
        *(undefined1 *)((intptr_t)puVar3 + 5) = 0xa2;
        *(undefined1 *)((intptr_t)puVar3 + 6) = 0xa2;
        iVar7 = iVar7 + 1;
        puVar3 = puVar3 + 10;
        iVar2 = iVar2 + 0x28;
      } while (iVar7 < 10);
      puVar8 = puVar8 + 100;
      iVar6 = iVar6 + 1;
      iVar4 = iVar4 + 400;
    } while (iVar6 < 2);
    iVar2 = 0;
    puVar8 = puVar5;
    do {
      iVar2 = iVar2 + 1;
      uVar1 = rand_lcg();
      *(ushort *)(puVar8 + 0xf0) = (uVar1 & 0x1f) + 8;
      *(undefined2 *)(puVar5 + 200) = *pos;
      puVar8 = (undefined4 *)((intptr_t)puVar8 + 2);
      *(undefined2 *)((intptr_t)puVar5 + 0x322) = pos[2];
      *(undefined2 *)(puVar5 + 0xc9) = pos[4];
      uVar1 = rand_lcg();
      *(ushort *)(puVar5 + 0xdc) = 0x20 - (uVar1 & 0x3f);
      uVar1 = rand_lcg();
      *(ushort *)((intptr_t)puVar5 + 0x372) = 0x20 - (uVar1 & 0x3f);
      uVar1 = rand_lcg();
      *(ushort *)(puVar5 + 0xdd) = 0x20 - (uVar1 & 0x3f);
      puVar5 = puVar5 + 2;
    } while (iVar2 < 10);
  }
  return;
}

/* guest gte_mat_vec_swap @0x80185290 */
void gte_mat_vec_swap(undefined4 *a0,ushort *a1,undefined4 *a2)

{
  ushort uVar1;
  ushort uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  
  setCopControlWord(2,0,*a2);
  setCopControlWord(2,0x800,a2[1]);
  setCopControlWord(2,0x1000,a2[2]);
  setCopControlWord(2,0x1800,a2[3]);
  setCopControlWord(2,0x2000,a2[4]);
  setCopReg(2,0,(int)(short)*a1);
  setCopReg(2,0x800,(int)(short)a1[6]);
  copFunction(2,0x486012);
  uVar1 = *(ushort *)(a2 + 2);
  uVar2 = *(ushort *)((intptr_t)a2 + 0xe);
  uVar3 = getCopReg(2,0x4800);
  uVar4 = getCopReg(2,0x5000);
  uVar5 = getCopReg(2,0x5800);
  setCopReg(2,0,-(int)(short)a1[6] & 0xffff);
  setCopReg(2,0x800,(int)(short)*a1);
  a1[1] = *(ushort *)((intptr_t)a2 + 2);
  copFunction(2,0x486012);
  a1[4] = uVar1;
  a1[7] = uVar2;
  *a1 = (ushort)uVar3;
  a1[3] = (ushort)uVar4;
  a1[6] = (ushort)uVar5;
  uVar4 = getCopReg(2,0x4800);
  uVar5 = getCopReg(2,0x5000);
  uVar3 = getCopReg(2,0xb);
  *(undefined4 *)(a1 + 8) = uVar3;
  a1[2] = (ushort)uVar4;
  a1[5] = (ushort)uVar5;
  setCopControlWord(2,0,*a0);
  setCopControlWord(2,0x800,a0[1]);
  setCopControlWord(2,0x1000,a0[2]);
  setCopControlWord(2,0x1800,a0[3]);
  setCopControlWord(2,0x2000,a0[4]);
  setCopReg(2,0,(uint)*a1 | *(uint *)(a1 + 2) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(a1 + 6));
  copFunction(2,0x486012);
  uVar3 = getCopReg(2,0x4800);
  uVar4 = getCopReg(2,0x5000);
  uVar5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)a1[1] | *(int *)(a1 + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)a1[7]);
  copFunction(2,0x486012);
  uVar6 = getCopReg(2,0x4800);
  uVar7 = getCopReg(2,0x5000);
  uVar8 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint)a1[2] | *(uint *)(a1 + 4) & 0xffff0000);
  setCopReg(2,0x800,*(undefined4 *)(a1 + 8));
  copFunction(2,0x486012);
  *a1 = (ushort)uVar3;
  a1[3] = (ushort)uVar4;
  a1[6] = (ushort)uVar5;
  a1[1] = (ushort)uVar6;
  a1[4] = (ushort)uVar7;
  a1[7] = (ushort)uVar8;
  uVar4 = getCopReg(2,0x4800);
  uVar5 = getCopReg(2,0x5000);
  uVar3 = getCopReg(2,0xb);
  *(undefined4 *)(a1 + 8) = uVar3;
  a1[2] = (ushort)uVar4;
  a1[5] = (ushort)uVar5;
  return;
}

/* guest trig_cos @0x80185404 */
int trig_cos(int ang)

{
  int idx;
  uint q;
  
  q = ang + 0x400;
  idx = (q & 0x3ff) * 2;
  if ((q & 0x400) != 0) {
    idx = (q & 0x3ff) * -2 + 0x800;
  }
  if ((q & 0x800) != 0) {
    return -(int)*(short *)(&DAT_801247a8 + idx);
  }
  return (int)*(short *)(&DAT_801247a8 + idx);
}

/* guest trig_sin @0x80185408 */
int trig_sin(uint ang)

{
  int idx;
  
  idx = (ang & 0x3ff) * 2;
  if ((ang & 0x400) != 0) {
    idx = (ang & 0x3ff) * -2 + 0x800;
  }
  if ((ang & 0x800) != 0) {
    return -(int)*(short *)(&DAT_801247a8 + idx);
  }
  return (int)*(short *)(&DAT_801247a8 + idx);
}

/* guest fighter_anim_advance @0x80185e6c */
void fighter_anim_advance(int f,int n)

{
  byte bVar1;
  undefined2 uVar2;
  short sVar3;
  int iVar4;
  
  if (*(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc0) ))== 0) {
    motion_integrator_tail(f,n);
  }
  if (*(char *)guest_at((uint32_t)(uintptr_t)((n + 0x9c) ))!= '\0') {
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18)));
    uVar2 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x23c)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x23c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x1c4)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18) ))+ 0x1c4) ))= uVar2;
    iVar4 = *(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18)));
    uVar2 = *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x28c)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x28c) ))= *(undefined2 *)guest_at((uint32_t)(uintptr_t)((iVar4 + 0x264)));
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18) ))+ 0x264) ))= uVar2;
  }
  bVar1 = *(byte *)guest_at((uint32_t)(uintptr_t)((n + 0x9c)));
  *(undefined1 *)guest_at((uint32_t)(uintptr_t)((n + 0x9c) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((n + 0xc0) ))= 0;
  *(byte *)guest_at((uint32_t)(uintptr_t)((n + 0x100) ))= *(byte *)guest_at((uint32_t)(uintptr_t)((n + 0x100) ))^ bVar1;
  sVar3 = *(short *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((n + 0xb0))));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((n + 0xc4) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((n + 0xc6) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((n + 0xca) ))= 0;
  *(int *)guest_at((uint32_t)(uintptr_t)((n + 0xb4) ))= *(int *)guest_at((uint32_t)(uintptr_t)((n + 0xb0) ))+ 4;
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc2) ))= sVar3 << 4;
  sVar3 = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((n + 0xb0) ))+ 6)));
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc6) ))+ 1;
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xcc) ))= sVar3 << 4;
  keyframe_advance((byte *)guest_at((uint32_t)f),(byte *)guest_at((uint32_t)n));
  return;
}

/* guest pose_chain_build @0x801862a8 */
void pose_chain_build(int node)

{
  int iVar1;
  undefined2 *puVar2;
  undefined4 *puVar3;
  short *psVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = 0x13;
  psVar4 = (short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(((*(byte *)guest_at((uint32_t)(uintptr_t)((node + 5) ))& 1) * 0x1c +
                              *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x38) ))+ 0x10) ))+ 0x14) ))+
                    *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x30))))));
  iVar5 = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x20) ))+ 0x5f0;
  if ('\x13' < *(char *)guest_at((uint32_t)(uintptr_t)((node + 8)))) {
    puVar3 = (undefined4 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x20) ))+ 0x638)));
    puVar2 = (undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x40) ))+ 0x21c)));
    do {
      *(uint32_t *)(puVar2 + 8) = (uint32_t)guest_of(psVar4);
      if (((int)*psVar4 & 0x8000U) != 0) {
        iVar1 = *(int *)guest_at((uint32_t)(uintptr_t)((puVar3[-2] + 0x44)));
        if (iVar1 == iVar5) {
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((puVar3[-2] + 0x44) ))= *puVar3;
        }
        else {
          for (; *(int *)guest_at((uint32_t)(uintptr_t)((iVar1 + 0x48) ))!= iVar5; iVar1 = *(int *)guest_at((uint32_t)(uintptr_t)((iVar1 + 0x48)))) {
          }
          *(undefined4 *)guest_at((uint32_t)(uintptr_t)((iVar1 + 0x48) ))= *puVar3;
        }
      }
      puVar2[6] = 0;
      puVar2[5] = 0;
      puVar2[4] = 0;
      puVar2[3] = 0;
      puVar2[2] = 0;
      puVar2[1] = 0;
      *puVar2 = 0;
      puVar2 = puVar2 + 0xe;
      puVar3 = puVar3 + 0x14;
      iVar5 = iVar5 + 0x50;
      iVar6 = iVar6 + 1;
      psVar4 = psVar4 + psVar4[1] + 2;
    } while (iVar6 < *(char *)guest_at((uint32_t)(uintptr_t)((node + 8))));
  }
  return;
}

/* guest euler_rotmat @0x8018dc18 */
undefined2 * euler_rotmat(uint node,uint a1,uint a2,undefined2 *a3)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  short sVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  
  iVar2 = (a1 & 0x3ff) * 2;
  iVar4 = (a1 & 0x3ff) * -2 + 0x800;
  iVar3 = iVar2;
  if ((a1 & 0x400) != 0) {
    iVar3 = iVar4;
    iVar4 = iVar2;
  }
  iVar4 = (int)*(short *)(&DAT_801247a8 + iVar4);
  uVar12 = node + a2;
  if ((a1 + 0x400 & 0x800) != 0) {
    iVar4 = -iVar4;
  }
  sVar1 = *(short *)(&DAT_801247a8 + iVar3);
  iVar3 = (int)sVar1;
  iVar5 = (node & 0x3ff) * 2;
  iVar6 = (node & 0x3ff) * -2 + 0x800;
  iVar2 = iVar5;
  if ((node & 0x400) != 0) {
    iVar2 = iVar6;
    iVar6 = iVar5;
  }
  iVar6 = (int)*(short *)(&DAT_801247a8 + iVar6);
  if ((node + 0x400 & 0x800) != 0) {
    iVar6 = -iVar6;
  }
  iVar2 = (int)*(short *)(&DAT_801247a8 + iVar2);
  iVar7 = (a2 & 0x3ff) * 2;
  iVar9 = (a2 & 0x3ff) * -2 + 0x800;
  iVar5 = iVar7;
  if ((a2 & 0x400) != 0) {
    iVar5 = iVar9;
    iVar9 = iVar7;
  }
  iVar9 = (int)*(short *)(&DAT_801247a8 + iVar9);
  if ((a2 + 0x400 & 0x800) != 0) {
    iVar9 = -iVar9;
  }
  iVar5 = (int)*(short *)(&DAT_801247a8 + iVar5);
  iVar10 = (uVar12 & 0x3ff) * 2;
  iVar11 = (uVar12 & 0x3ff) * -2 + 0x800;
  iVar7 = iVar10;
  if ((uVar12 & 0x400) != 0) {
    iVar7 = iVar11;
    iVar11 = iVar10;
  }
  iVar11 = (int)*(short *)(&DAT_801247a8 + iVar11);
  if ((node & 0x800) != 0) {
    iVar2 = -iVar2;
  }
  iVar7 = (int)*(short *)(&DAT_801247a8 + iVar7);
  if ((uVar12 + 0x400 & 0x800) != 0) {
    iVar11 = -iVar11;
  }
  if ((a2 & 0x800) != 0) {
    iVar5 = -iVar5;
  }
  *a3 = (short)(iVar9 * iVar4 >> 0xc);
  a3[7] = (short)(iVar2 * iVar4 >> 0xc);
  sVar8 = -sVar1;
  if ((a1 & 0x800) != 0) {
    iVar3 = -iVar3;
    sVar8 = sVar1;
  }
  a3[6] = sVar8;
  if ((uVar12 & 0x800) != 0) {
    iVar7 = -iVar7;
  }
  iVar11 = iVar11 * (iVar3 + -0x1000);
  a3[8] = (short)(iVar6 * iVar4 >> 0xc);
  node = node - a2;
  a3[3] = (short)(iVar5 * iVar4 >> 0xc);
  iVar6 = (node & 0x3ff) * 2;
  iVar7 = iVar7 * (iVar3 + -0x1000);
  iVar2 = (node & 0x3ff) * -2 + 0x800;
  iVar4 = iVar6;
  if ((node & 0x400) != 0) {
    iVar4 = iVar2;
    iVar2 = iVar6;
  }
  iVar2 = (int)*(short *)(&DAT_801247a8 + iVar2);
  if ((node + 0x400 & 0x800) != 0) {
    iVar2 = -iVar2;
  }
  iVar2 = iVar2 * (iVar3 + 0x1000);
  iVar4 = (int)*(short *)(&DAT_801247a8 + iVar4);
  if ((node & 0x800) != 0) {
    iVar4 = -iVar4;
  }
  iVar4 = iVar4 * (iVar3 + 0x1000);
  a3[2] = (short)((iVar11 + iVar2) >> 0xd);
  a3[4] = (short)((iVar2 - iVar11) >> 0xd);
  a3[1] = (short)((iVar7 + iVar4) >> 0xd);
  a3[5] = (short)((iVar7 - iVar4) >> 0xd);
  return a3;
}

/* ---- batch25: task callbacks + motion_integrator_tail (verified vs disasm) ---- */
#define _DAT_1f8003fe (*(uint16_t *)((char *)&_DAT_1f8003fc + 2))

/* spark_trail_task_update — spark-trail task update (EV7 sub 3). */
void spark_trail_task_update(int *rec)
{
  ushort life;
  short pos;
  int t;
  int sp;

  if (DAT_801bef20 != 0) {
    return;
  }
  life = (ushort)((short)rec[1] - 1);
  *(ushort *)(rec + 1) = life;
  if ((int)((uint)life << 0x10) < 0) {
    task_slot_step();
    return;
  }
  if ((_DAT_801c4578 == 0) || (_DAT_801c4578 == 3)) {
    if ((*(ushort *)(rec + 1) & 1) != 0) goto LAB_8013ab68;
    sp = ((int)(short)rec[3] << 3) / 10;
  }
  else {
    sp = (int)(short)rec[3];
  }
  t = (uint)*(ushort *)((intptr_t)rec + 6) << 0x10;
  if ((t >> 0x13 < (t >> 0x10) + (int)(short)rec[2]) >> 3) {
    t = (int)(short)rec[6];
    if (t < 0) {
      t = t + 0xf;
    }
    impact_fx_spawn((undefined4 *)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ *(short *)((intptr_t)rec + 10) * 0x50 + 0x34),0x80000005,
                 (int)(short)rec[4] + (int)*(short *)guest_at((uint32_t)(uintptr_t)((*rec + 0x4e))),(int)(short)rec[5],
                 (uint)(sp << 0x10 | t >> 4));
    *(short *)(rec + 4) = (short)rec[4] + *(short *)((intptr_t)rec + 0x12);
    *(short *)(rec + 3) = (short)rec[3] + *(short *)((intptr_t)rec + 0xe);
    pos = (short)rec[6] + *(short *)((intptr_t)rec + 0x1a);
    *(short *)(rec + 6) = pos;
    *(short *)(rec + 5) = (short)rec[5] + *(short *)((intptr_t)rec + 0x16);
    if (pos < 0xa0) {
      *(undefined2 *)(rec + 6) = 0xa0;
    }
    if ((short)rec[3] < 0xf) {
      *(undefined2 *)(rec + 3) = 0xf;
    }
  }
LAB_8013ab68:
  *(short *)((intptr_t)rec + 6) = *(short *)((intptr_t)rec + 6) + (short)rec[2];
  return;
}

/* confetti_burst_update — confetti/debris burst update (16 sprites x 2 rows).
 * in_zero->cop2 reg 0, in_at->cop2 reg 1 (0x800 encoding), verified
 * against lwc2 $0/$1 at 0x8013d8ac-0x8013d8b0. */
void confetti_burst_update(int rec)
{
  short *yp;
  undefined4 org;
  int t;
  short *wp;
  uint *prim;
  int i;
  short ox;
  short oy;

  prim = (uint *)guest_at((uint32_t)(uintptr_t)((rec + _DAT_1f80000c * 0x280)));
  if ((_DAT_1f800082 == 0) &&
     (t = *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x554) ))+ -1, *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x554) ))= t, t == 0)) {
    task_slot_step();
  }
  else {
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((rec + 0x540) ))=
         *(undefined2 *)
          (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x550) ))* 0x50 + *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x548) ))+ 0x20) ))+ 0x34);
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((rec + 0x542) ))=
         *(undefined2 *)
          (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x550) ))* 0x50 + *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x548) ))+ 0x20) ))+ 0x38);
    *(undefined2 *)guest_at((uint32_t)(uintptr_t)((rec + 0x544) ))=
         *(undefined2 *)
          (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x550) ))* 0x50 + *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x548) ))+ 0x20) ))+ 0x3c);
    setCopReg(2,0,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x540))));
    setCopReg(2,0x800,*(undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 0x544))));
    copFunction(2,0x180001);
    org = (undefined4)getCopReg(2,0xe);
    i = 0;
    wp = (short *)((intptr_t)prim + 0x22);
    t = rec;
    do {
      if (_DAT_1f800082 == 0) {
        *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x500) ))=
             (short)((int)*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x500) ))* (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x558) ))>> 0xc);
        *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x502) ))=
             (short)((int)*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x502) ))* (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x558) ))>> 0xc);
        *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x558) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x558) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x578)));
      }
      ox = (short)org;
      wp[-0xd] = ox + *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x500)));
      oy = (short)((uint)org >> 0x10);
      wp[-0xc] = oy + *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x502)));
      wp[-9] = ox + *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x500) ))+ 0x14;
      wp[-8] = oy + *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x502)));
      wp[-5] = ox + *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x500)));
      t = t + 2;
      wp[-4] = oy + *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x502) ))+ 0x14;
      i = i + 1;
      wp[-1] = ox + *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x500) ))+ 0x14;
      yp = (short *)guest_at((uint32_t)(uintptr_t)((rec + 0x502)));
      rec = rec + 4;
      *wp = oy + *yp + 0x14;
      wp = wp + 0x14;
      *prim = *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))| 0x9000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))= (uint)guest_of(prim) & 0xffffff;
      prim = prim + 10;
    } while (i < 0x10);
  }
  return;
}

/* ring_effect_task_update — ring/effect task update (6 sprites, matrix transform). */
void ring_effect_task_update(int rec)
{
  undefined4 org;
  int t;
  int trig;
  short *wp;
  uint *prim;
  int i;
  undefined4 in0;
  undefined4 in1;

  prim = (uint *)guest_at((uint32_t)(uintptr_t)((rec + _DAT_1f80000c * 0xa8)));
  if ((_DAT_1f800082 == 0) &&
     (t = *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x168))), *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x168) ))= t + -1, t < 0)) {
    task_slot_step();
  }
  else {
    /* Guest 0x80140848-0x801408B4: the record address
     *   lw(rec+0x164)*0x50 + lw(lw(rec+0x160)+0x20)
     * is a guest address; the halfword loads at +0x34/+0x38/+0x3c go
     * through guest_at(). The decompiled form dropped the guest_at()
     * wrapper and dereferenced the raw guest address as a host pointer. */
    {
      int ring_ent = (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x164) ))* 0x50 +
                     *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x160) ))+ 0x20) )));
      in0 = ((undefined4)*(undefined2 *)
                   guest_at((uint32_t)(uintptr_t)(ring_ent + 0x38)) << 16) |
               *(undefined2 *)
                 guest_at((uint32_t)(uintptr_t)(ring_ent + 0x34));
      in1 = (*(undefined2 *)
                   guest_at((uint32_t)(uintptr_t)(ring_ent + 0x3c))) & 0xffff;
    }
    setCopControlWord(2,0,((uint32_t *)g_DAT_801be838)[0]);
    setCopControlWord(2,0x800,((uint32_t *)g_DAT_801be838)[1]);
    setCopControlWord(2,0x1000,((uint32_t *)g_DAT_801be838)[2]);
    setCopControlWord(2,0x1800,((uint32_t *)g_DAT_801be838)[3]);
    setCopControlWord(2,0x2000,((uint32_t *)g_DAT_801be838)[4]);
    setCopControlWord(2,0x2800,((uint32_t *)g_DAT_801be838)[5]);
    setCopControlWord(2,0x3000,((uint32_t *)g_DAT_801be838)[6]);
    setCopControlWord(2,0x3800,((uint32_t *)g_DAT_801be838)[7]);
    setCopReg(2,0,in0);
    setCopReg(2,0x800,in1);
    copFunction(2,0x180001);
    org = (undefined4)getCopReg(2,0xe);
    i = 0;
    wp = (short *)((intptr_t)prim + 0x1a);
    t = rec;
    do {
      *(undefined4 *)(wp + -9) = org;
      trig = lut_cos_q12((int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x16c))));
      wp[-5] = wp[-9] + (short)((trig << 9) >> 0xc);
      trig = lut_sin_q12((int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x16c))));
      wp[-4] = wp[-8] + (short)((trig << 9) >> 0xc);
      trig = lut_cos_q12((int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x16c) ))+ (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x184))));
      wp[-1] = wp[-9] + (short)(trig * 0x164 >> 0xc);
      trig = lut_sin_q12((int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x16c) ))+ (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x184))));
      *wp = wp[-8] + (short)(trig * 0x164 >> 0xc);
      if (_DAT_1f800082 == 0) {
        *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x16c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x16c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x178)));
        *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x178) ))= (short)(*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x178) ))* 0x753 >> 0xb);
        *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x184) ))= (short)(*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x184) ))* 0x753 >> 0xb);
      }
      wp = wp + 0xe;
      t = t + 2;
      i = i + 1;
      *prim = *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))| 0x6000000;
      *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))= (uint)guest_of(prim) & 0xffffff;
      prim = prim + 7;
    } while (i < 6);
    *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f80000c * 8 + rec + 0x150) ))= *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))| 0x1000000;
    *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))= (rec + _DAT_1f80000c * 8 + 0x150) & 0xffffff;
  }
  return;
}

/* firework_task_update — firework/particle task update (10 emitters). */
void firework_task_update(int rec)
{
  short sVar1;
  ushort life;
  int t;
  int prod;
  char *bp;
  short *psVar6;
  uint *puVar7;
  short *psVar8;
  short *psVar9;
  undefined4 *puVar10;
  short *psVar11;
  int i;

  t = _DAT_1f80000c * 400;
  if (_DAT_1f800082 == 0) {
    life = (ushort)(*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4) ))- 1);
    *(ushort *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4) ))= life;
    psVar9 = (short *)guest_at((uint32_t)(uintptr_t)((rec + 800)));
    if ((int)((uint)life << 0x10) < 0) {
      task_slot_step();
      return;
    }
    psVar11 = (short *)guest_at((uint32_t)(uintptr_t)((rec + 0x370)));
    i = 0;
    psVar6 = (short *)guest_at((uint32_t)(uintptr_t)((rec + 0x374)));
    psVar8 = (short *)guest_at((uint32_t)(uintptr_t)((rec + 0x324)));
    bp = (char *)guest_at((uint32_t)(uintptr_t)((rec + t + 6)));
    t = rec;
    do {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4) ))< 10) {
        bp[-2] = bp[-2] + -0x20;
        *bp = *bp + -0x20;
        bp[-1] = bp[-1] + -0x20;
      }
      if (10 < *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4)))) {
        *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x3c0) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x3c0) ))+ 4;
        prod = *psVar11 * 0xf0a;
        if (prod < 0) {
          prod = prod + 0xfff;
        }
        *psVar11 = (short)(prod >> 0xc);
        prod = psVar6[-1] * 0xf0a;
        if (prod < 0) {
          prod = prod + 0xfff;
        }
        psVar6[-1] = (short)(prod >> 0xc);
        prod = *psVar6 * 0xf0a;
        if (prod < 0) {
          prod = prod + 0xfff;
        }
        *psVar6 = (short)(prod >> 0xc);
      }
      bp = bp + 0x28;
      sVar1 = *psVar11;
      psVar11 = psVar11 + 4;
      t = t + 2;
      i = i + 1;
      *psVar9 = *psVar9 + sVar1;
      psVar9 = psVar9 + 4;
      psVar8[-1] = psVar8[-1] + psVar6[-1];
      sVar1 = *psVar6;
      psVar6 = psVar6 + 4;
      *psVar8 = *psVar8 + sVar1;
      psVar8 = psVar8 + 4;
    } while (i < 10);
  }
  puVar10 = (undefined4 *)guest_at((uint32_t)(uintptr_t)((rec + 800)));
  t = 0;
  puVar7 = (uint *)guest_at((uint32_t)(uintptr_t)((rec + _DAT_1f80000c * 400)));
  psVar9 = (short *)((intptr_t)puVar7 + 0x22);
  do {
    setCopReg(2,0,*puVar10);
    setCopReg(2,0x800,puVar10[1]);
    copFunction(2,0x180001);
    _DAT_1f8003fc = (uint32_t)getCopReg(2,0xe);
    _DAT_1f8003f8 = (uint32_t)getCopReg(2,0x9800);
    _DAT_1f8003f8 = _DAT_1f8003f8 >> 2;
    sVar1 = *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3c0)));
    /* guest @0x80140F70-0x80140F7C: lhu @0x1f800024 then sll16/sra16 =
     * sext16 of the low halfword only — NOT the full u32 cell. */
    i = (int)(int16_t)(uint16_t)_DAT_1f800024;
    prod = _DAT_1f8003f8 << 2;
    psVar9[-0xd] = (short)_DAT_1f8003fc;
    psVar9[-0xc] = _DAT_1f8003fe;
    sVar1 = (short)mips_div_lo((int32_t)(sVar1 * i),prod); /* guest div @0x80140F94 */
    psVar9[-9] = (short)_DAT_1f8003fc + sVar1;
    psVar9[-8] = _DAT_1f8003fe;
    psVar9[-5] = (short)_DAT_1f8003fc;
    psVar9[-4] = _DAT_1f8003fe + sVar1;
    puVar10 = puVar10 + 2;
    psVar9[-1] = (short)_DAT_1f8003fc + sVar1;
    rec = rec + 2;
    *psVar9 = _DAT_1f8003fe + sVar1;
    t = t + 1;
    psVar9 = psVar9 + 0x14;
    *puVar7 = *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x10) ))| 0x9000000;
    *(uint *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x10) ))= guest_of(puVar7) & 0xffffff;
    puVar7 = puVar7 + 10;
  } while (t < 10);
  return;
}

/* motion_integrator_tail — motion integrator tail: rotate velocity by facing,
 * decay position deltas. Called by fighter_anim_advance when +0xc0==0. */
void motion_integrator_tail(int a0,int a1)
{
  short px;
  short pz;
  short pw;
  short py;
  int t1;
  undefined2 *wp;
  int ang;
  int t0;

  t0 = 1;
  wp = (undefined2 *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((a0 + 0x40) ))+ 0x24)));
  do {
    wp[-1] = (short)(((int)((uint)(ushort)wp[-1] << 0x13) >> 0x14) << 1);
    if ((t0 != 1) && (t0 != 4)) {
      wp[2] = 0;
      wp[1] = 0;
      *wp = 0;
    }
    t0 = t0 + 1;
    wp = wp + 0xe;
  } while (t0 < 0x13);
  px = *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xdc)));
  ang = (int)*(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd0)));
  py = *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xde)));
  pz = *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xe0)));
  t0 = lut_cos_q12(ang);
  t1 = lut_sin_q12(ang);
  *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd6) ))+ (py >> 4);
  *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd4) ))=
       *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd4) ))+ (short)((uint)(px * t0 - pz * t1) >> 0x10);
  t0 = lut_sin_q12(ang);
  t1 = lut_cos_q12(ang);
  py = *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xe4)));
  pw = *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd2)));
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a1 + 0xdc) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a1 + 0xde) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a1 + 0xe0) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a1 + 0xe4) ))= 0;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd2) ))= 0;
  *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd0) ))= (short)((ang + pw + (int)py) * 0x100000 >> 0x14);
  *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd8) ))=
       *(short *)guest_at((uint32_t)(uintptr_t)((a1 + 0xd8) ))+ (short)((uint)(px * t0 + pz * t1) >> 0x10);
  return;
}

/* LAB_8013e558 — speech-bubble/voice-print task update (EV2/3 path).
 * Alternate entry: task callback registered via task_node_alloc. */
void LAB_8013e558(undefined4 *ctx)
{
  undefined2 sh;
  undefined2 *wp;
  int nd;
  short t;
  undefined4 rx;
  undefined4 ry;
  undefined4 rz;

  if (_DAT_1f800082 == 0) {
    t = *(short *)((intptr_t)ctx + 0x46) + 0x200;
    *(short *)((intptr_t)ctx + 0x46) = t;
    *(short *)(ctx + 0x11) = *(short *)(ctx + 0x11) + 0x80;
    if (0x1000 < t) {
      node_tree_free(*ctx);
      task_slot_step();
      if (-1 < _DAT_801f0930 + -1) {
        _DAT_801f0930 = _DAT_801f0930 + -1;
        return;
      }
      _DAT_801f0930 = 0;
      return;
    }
    tween_seek((int)*ctx,0);
  }
  sh = *(undefined2 *)(ctx + 0x11);
  *(undefined2 *)(ctx + 1) = sh;
  *(undefined2 *)(ctx + 3) = sh;
  *(undefined2 *)(ctx + 5) = sh;
  setCopControlWord(2,0,ctx[9]);
  setCopControlWord(2,0x800,ctx[10]);
  setCopControlWord(2,0x1000,ctx[0xb]);
  setCopControlWord(2,0x1800,ctx[0xc]);
  setCopControlWord(2,0x2000,ctx[0xd]);
  setCopReg(2,0x4800,(uint)*(ushort *)(ctx + 1));
  setCopReg(2,0x5000,(uint)*(ushort *)((intptr_t)ctx + 10));
  setCopReg(2,0x5800,(uint)*(ushort *)(ctx + 4));
  copFunction(2,0x49e012);
  wp = (undefined2 *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(*ctx)));
  rx = (undefined4)getCopReg(2,0x4800);
  ry = (undefined4)getCopReg(2,0x5000);
  rz = (undefined4)getCopReg(2,0x5800);
  *wp = (short)rx;
  wp[3] = (short)ry;
  wp[6] = (short)rz;
  setCopReg(2,0x4800,(uint)*(ushort *)((intptr_t)ctx + 6));
  setCopReg(2,0x5000,(uint)*(ushort *)(ctx + 3));
  setCopReg(2,0x5800,(uint)*(ushort *)((intptr_t)ctx + 0x12));
  copFunction(2,0x49e012);
  nd = *(int *)guest_at((uint32_t)(uintptr_t)(*ctx));
  rx = (undefined4)getCopReg(2,0x4800);
  ry = (undefined4)getCopReg(2,0x5000);
  rz = (undefined4)getCopReg(2,0x5800);
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 2) ))= (short)rx;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 8) ))= (short)ry;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 0xe) ))= (short)rz;
  setCopReg(2,0x4800,(uint)*(ushort *)(ctx + 2));
  setCopReg(2,0x5000,(uint)*(ushort *)((intptr_t)ctx + 0xe));
  setCopReg(2,0x5800,(uint)*(ushort *)(ctx + 5));
  copFunction(2,0x49e012);
  nd = *(int *)guest_at((uint32_t)(uintptr_t)(*ctx));
  rx = (undefined4)getCopReg(2,0x4800);
  ry = (undefined4)getCopReg(2,0x5000);
  rz = (undefined4)getCopReg(2,0x5800);
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 4) ))= (short)rx;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 10) ))= (short)ry;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 0x10) ))= (short)rz;
  return;
}

/* LAB_80140510 — hit-spark sequence player task update (EV22 path).
 * Walks {op,arg,jump} records; spawns impact bursts via impact_fx_spawn. */
void LAB_80140510(int *ctx)
{
  undefined2 w;
  uint op;
  undefined2 *nxt;
  ushort *ip;
  uint arg;
  short *rec;
  int i;

  if (_DAT_1f800082 == 0) {
    while( true ) {
      while( true ) {
        ip = (ushort *)guest_at((uint32_t)(uintptr_t)(ctx[1]));
        arg = *ip & 0xff;
        op = *ip & 0xff00;
        if (op != 0xfe00) break;
        nxt = (undefined2 *)guest_at((uint32_t)(uintptr_t)((*ctx + (short)ip[1] * 8)));
        ctx[1] = (int)guest_of(nxt);
        *(undefined2 *)(ctx + 3) = *nxt;
      }
      if (0xfe00 < op) break;
      if (op != 0xfc00) goto LAB_80140624;
      if ((int)arg < (int)*(short *)((intptr_t)ctx + 0xe)) {
        ctx[1] = (int)guest_of(ip + 4);
      }
      else {
        nxt = (undefined2 *)guest_at((uint32_t)(uintptr_t)((*ctx + (short)ip[1] * 8)));
        ctx[1] = (int)guest_of(nxt);
        w = *nxt;
        *(short *)((intptr_t)ctx + 0xe) = *(short *)((intptr_t)ctx + 0xe) + 1;
        *(undefined2 *)(ctx + 3) = w;
      }
    }
    if (op == 0xff00) {
      if ((int)*(short *)((intptr_t)ctx + 0xe) <= (int)arg) {
        task_slot_step();
        return;
      }
      ctx[1] = (int)guest_of(ip + 4);
      return;
    }
LAB_80140624:
    rec = (short *)guest_at((uint32_t)(uintptr_t)(ctx[1]));
    if ((short)ctx[3] == *rec) {
      do {
        op = (uint)rec[2];
        if ((short)ctx[4] != 0) {
          op = (uint)g_DAT_801b180c[op];
        }
        i = 0;
        if (0 < rec[1]) {
          do {
            if (*(short *)guest_at((uint32_t)(uintptr_t)((ctx[1] + 6) ))== 0) {
              impact_fx_spawn((undefined4 *)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((ctx[2] + 0x20) ))+ op * 0x50 + 0x34),0x80000003,
                           (int)*(short *)((intptr_t)ctx + 0x12),0,0);
            }
            i = i + 1;
          } while (i < *(short *)guest_at((uint32_t)(uintptr_t)((ctx[1] + 2))));
        }
        i = ctx[1];
        rec = (short *)guest_at((uint32_t)(uintptr_t)((i + 8)));
        ctx[1] = (int)guest_of(rec);
      } while ((short)ctx[3] == *(short *)guest_at((uint32_t)(uintptr_t)((i + 8))));
    }
    *(short *)(ctx + 3) = (short)ctx[3] + 1;
  }
  return;
}

/* ---- batch28: HUD/announce/scroll helpers (disasm-verified widths) ---- */
extern void *guest_at(uint32_t addr);
#define BA8(a)   (*(uint8_t  *)guest_at(a))
#define BA16(a)  (*(uint16_t *)guest_at(a))
#define BA32(a)  (*(uint32_t *)guest_at(a))
#define BA16S(a) (*(int16_t  *)guest_at(a))
extern char *s_Print_mode_error_8011f140;
extern undefined4 text_win_init(uint32_t,uint32_t);
extern uint32_t text_widget_alloc(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern undefined4 text_line_select(uint32_t);
extern undefined4 printf_forward(const char *, ...);
extern void bitfield_or_write(void *, uint32_t);
extern void spr_desc_a(uint32_t);
extern void spr_desc_b(uint32_t,uint32_t);
extern void spr_desc_c(uint32_t,uint32_t);
extern uint32_t cd_cmd_retry(uintptr_t);

/* announce_param_block_init — round/announce parameter block init @0x1f80038c.
 * All fields u16 (sh) except +0x1c u32 (sw); verified vs disasm. */
void announce_param_block_init(uint32_t a0,uint32_t a1,const void *a2,
                  uint32_t a3,uint32_t arg5,int32_t arg6)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BA32(0x1f8003a8) = guest_of(a2);
  b[3] = (uint16_t)a3;
  b[4] = (uint16_t)arg5;
  b[5] = 0xf; b[6] = 0xf; b[7] = 0xffff; b[8] = 0xf;
  b[9] = 1; b[10] = 1;
  b[11] = (uint16_t)((((uint32_t)arg6 + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  b[12] = 4;
  b[0] = 0; b[1] = 0; b[2] = 0;
  text_render_full((uint *)guest_at(a0), a1 & 0xffff,
               0x1f80038c);
}

/* print_mode_dispatch — print-mode dispatch (mode 1 = text window). */
void print_mode_dispatch_b(int32_t a0,uint32_t a1,uint32_t a2)
{
  BA32(0x1f800384) = (uint32_t)a0;
  if (a0 == 1) {
    uint32_t widget;
    text_win_init(0x3c0,0);
    widget = text_widget_alloc(a1,a2,0x140,0xf0,0,0x200);
    text_line_select(widget);
    return;
  }
  if (a0 < 2) {
    if (a0 == 0) return;
  } else if (a0 == 2) {
    return;
  }
  printf_forward(s_Print_mode_error_8011f140);
}

/* scroll_offset_accumulate — scroll offset accumulate (u16 pair @0x1f80037c). */
void scroll_offset_accumulate(int16_t x,int16_t y)
{
  if (BA8(0x1f800015) != 0) y = (int16_t)(y << 1);
  BA16(0x1f80037c) = (uint16_t)(BA16(0x1f80037c) + (uint16_t)x);
  BA16(0x1f80037e) = (uint16_t)(BA16(0x1f80037e) + (uint16_t)y);
}

/* scroll_offset_set — scroll offset set (-1 = keep). */
void scroll_offset_set(int32_t x,int32_t y)
{
  if (BA8(0x1f800015) != 0) y <<= 1;
  if (0 <= x) BA16(0x1f80037c) = (uint16_t)x;
  if (0 <= y) BA16(0x1f80037e) = (uint16_t)y;
}

/* scroll_offset_y_get — scroll-offset Y-component read leaf @0x8012ce20
 * (4 insns, no prologue): v0 = lh(0x1f80037e); jr ra. 0x1f80037e is the
 * second (y) half of the scroll-offset u16 pair @0x1f80037c/0x37e
 * written by scroll_offset_set/accumulate. Hot leaf — many jal callers.
 * Args unused. */
int32_t scroll_offset_y_get(void)
{
  return _DAT_1f80037e;
}

/* rgb_triple_store — RGB triple store (two mirrored triples). */
void rgb_triple_store(uint8_t r,uint8_t g,uint8_t b)
{
  BA8(0x801e40b9) = r; BA8(0x801e40ba) = g; BA8(0x801e40bb) = b;
  BA8(0x801e6209) = r; BA8(0x801e620a) = g; BA8(0x801e620b) = b;
}

/* brightness_packet_send — brightness/gamma level -> 4-byte packet to cd_cmd_retry.
 * DAT_801bef70 = s16 level; DAT_801f36c0 u16 flag selects direct vs
 * scaled (x*63/127) encoding. Verified vs disasm incl. stack buf. */
void brightness_packet_send(int32_t val)
{
  /* Generated 0x80156114: addiu sp,sp,-32; the 4-byte packet lives at
   * sp+0x10 and val=sp+0x10 — a GUEST stack address — is passed to
   * cd_cmd_retry -> cd_cmd_bytes -> guest_at(val). A host stack buffer
   * would be truncated to a garbage guest address, so the packet must
   * sit in guest-visible memory. Reserve the same 0x20 guest frame and
   * use the sp+0x10 slot exactly like the MIPS body. */
  uint32_t frm = br2_guest_frame(0x20);
  uint8_t *buf = (uint8_t *)guest_at(frm + 0x10);
  if (val == 0) {
    buf[0] = buf[1] = buf[2] = buf[3] = 0;
  } else {
    int16_t *lv = (int16_t *)guest_at(0x801bef70);
    if (0 <= val) *lv = (int16_t)val;
    if (BA16(0x801f36c0) == 0) {
      uint8_t v = *(uint8_t *)guest_at(0x801bef70);
      buf[0] = v; buf[1] = 0; buf[2] = v; buf[3] = 0;
    } else {
      int32_t r = ((int32_t)*lv * 63) / 127;
      buf[0] = buf[1] = buf[2] = buf[3] = (uint8_t)r;
    }
  }
  cd_cmd_retry((uintptr_t)(frm + 0x10));
  br2_guest_frame_free();
}

/* hud_sprite_desc_init — HUD/sprite descriptor block init @0x801f0660.
 * Four 0x5c-byte descriptor groups + 0x68-byte mirror copy.
 * Faithful to disasm (sb/sh/sw widths verified). */
void hud_sprite_desc_init(void)
{
  uint8_t *b = (uint8_t *)guest_at(0x801f0660);
  uint32_t *t = (uint32_t *)guest_at(0x801f0640);
  uint16_t v0 = BA16(0x1f800020);
  int i;

  b[0x03] = 3; b[0x07] = 0x60;
  *(uint16_t *)(b + 0x08) = 0; *(uint16_t *)(b + 0x0a) = 0;
  *(uint16_t *)(b + 0x0e) = 0; *(uint16_t *)(b + 0x0c) = v0;
  bitfield_or_write(b + 0x04, t[0]);

  b[0x5b] = 3; b[0x5f] = 0x60;
  *(uint16_t *)(b + 0x60) = 0; *(uint16_t *)(b + 0x62) = 0;
  *(uint16_t *)(b + 0x66) = 0; *(uint16_t *)(b + 0x64) = v0;
  bitfield_or_write(b + 0x5c, t[3]);

  b[0x13] = 8; b[0x17] = 0x38;
  *(uint16_t *)(b + 0x28) = 0; *(uint16_t *)(b + 0x18) = 0;
  *(uint16_t *)(b + 0x30) = v0; *(uint16_t *)(b + 0x20) = v0;
  bitfield_or_write(b + 0x14, t[0]);
  bitfield_or_write(b + 0x1c, t[0]);
  bitfield_or_write(b + 0x24, t[1]);
  bitfield_or_write(b + 0x2c, t[1]);

  b[0x37] = 8; b[0x3b] = 0x38;
  *(uint16_t *)(b + 0x4c) = 0; *(uint16_t *)(b + 0x3c) = 0;
  *(uint16_t *)(b + 0x54) = v0; *(uint16_t *)(b + 0x44) = v0;
  bitfield_or_write(b + 0x38, t[2]);
  bitfield_or_write(b + 0x40, t[2]);
  bitfield_or_write(b + 0x48, t[3]);
  bitfield_or_write(b + 0x50, t[3]);

  /* mirror: 0x660..0x6c8 -> 0x6c8..0x730 (6x16B + tail 8B) */
  for (i = 0; i < 6; i++)
    memcpy(b + 0x68 + i * 0x10, b + i * 0x10, 0x10);
  memcpy(b + 0xc8, b + 0x60, 8);
}

/* sprite_desc_table_init — 800-slot sprite descriptor table init @0x801e8528.
 * Per slot: spr_desc_a/4060/4090 calls + 0x7f bytes + u16 0x2f. */
void sprite_desc_table_init(uint32_t tbl)
{
  uint32_t s1 = 0x801e8528;
  int i;
  BA32(0x1f800384) = 0;
  BA16(0x1f800380) = 0; BA16(0x1f80037e) = 0; BA16(0x1f80037c) = 0;
  BA32(0x1f800388) = 0xffffffff;
  for (i = 0; i < 800; i++) {
    spr_desc_a(s1);
    spr_desc_b(s1,tbl);
    spr_desc_c(s1,1);
    BA8(s1 + 4) = 0x7f; BA8(s1 + 5) = 0x7f; BA8(s1 + 6) = 0x7f;
    BA16(s1 + 0x16) = 0x2f;
    s1 += 0x28;
  }
}

/* announce_param_block_init_alt — variant of announce_param_block_init: same param block @0x1f80038c
 * but field +0xa preset to 0x1e (vs 0xf). Disasm-verified widths. */
void announce_param_block_init_alt(uint32_t tbl,uint32_t a1,const void *a2,
                  uint32_t a3,uint32_t arg5,int32_t arg6)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BA16(0x1f800398) = 0x1e;
  BA16(0x1f80039a) = 0xffff;
  BA16(0x1f80039e) = 1;
  BA16(0x1f8003a0) = 1;
  BA16(0x1f80038c) = 0;
  BA16(0x1f80038e) = 0;
  BA16(0x1f800390) = 0;
  BA16(0x1f800396) = 0xf;
  BA16(0x1f80039c) = 0xf;
  BA16(0x1f8003a4) = 4;
  BA16(0x1f8003a2) = (uint16_t)((((uint32_t)arg6 + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  BA16(0x1f800394) = (uint16_t)arg5;
  BA16(0x1f800392) = (uint16_t)a3;
  BA32(0x1f8003a8) = guest_of(a2);
  text_render_full((uint *)guest_at(tbl), a1 & 0xffff,
               0x1f80038c);
}

/* hud_time_score_draw — battle HUD time/score draw dispatcher. Formats the
 * frame timer as HH:MM:SS-ish "%02u:%02u:%02u" then issues text-draw
 * calls (announce_param_block_init_b/2e184/2e61c/2e73c variants) per layout mode
 * arg6. arg5 = score/time record ptr; a2 = layout base;
 * a3 = color/attr; tbl/2 = draw env pair. */
extern void hud_text_a(undefined4,undefined2,const void *,undefined2,undefined2,short);
extern void hud_text_b(undefined4,undefined4,const void *,undefined4,undefined2,short);
extern void hud_text_c(undefined4,undefined4,const void *,undefined4,undefined2,undefined4);
extern void print_dispatch(const char *,...);
extern void slot_name_measure(char *,uint32_t);
void hud_time_score_draw(uint32_t tbl,uint16_t a1,int32_t a2,int32_t a3,
                  uint8_t *arg5,int32_t arg6)
{
  int32_t iVar1, iVar2, iVar4, iVar5, iVar6, iVar7;
  int16_t sVar3;
  uint32_t uVar8;
  /* auStack_48 = sp+0x18 (frame 0x60) -> guest_sp()-0x48;
   * `addiu $tbl,$sp,0x18` before sprintf jal in 0x80163e38. The string is
   * re-read by announce_param_block_init_b / hud_text_* / slot_name_measure
   * across many nested calls, so reserve the guest frame window
   * (sp+0x18 -> frm+0x18) or callee frames overwrite it. */
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  uint32_t htsd_frm = br2_guest_frame(0x60);
  char *auStack_48 = (char *)guest_at(htsd_frm + 0x18);
  iVar5 = *(int32_t *)(arg5 + 8) / 0x32;
  iVar6 = iVar5 / 0x3c;
  iVar5 = iVar5 % 0x3c;
  iVar4 = (*(int32_t *)(arg5 + 8) % 0x32) * 2;
  if (99 < iVar6) { iVar4 = 99; iVar6 = 99; iVar5 = 0x3b; }
  iVar7 = iVar4;
  sprintf_core(auStack_48,(char *)guest_at(0x80121fa8),iVar6,iVar5,iVar4);
  iVar2 = ((int32_t)*(int16_t *)(arg5 + 0xc) % 1000) * 0x10000 >> 0x10;
  iVar1 = ((int32_t)*(int16_t *)(arg5 + 0xe) % 1000) * 0x10000 >> 0x10;
  sVar3 = (int16_t)a3;
  switch (arg6) {
  case 0:
    iVar5 = (int32_t)sVar3;
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + 0x10) * 0x10000 >> 0x10),(uint16_t)iVar5,1);
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + 0xa0) * 0x10000 >> 0x10),(uint16_t)iVar5,1);
    announce_param_block_init_b(tbl,a1,arg5,(uint16_t)((a2 + 0x130) * 0x10000 >> 0x10),(uint16_t)iVar5,4);
    break;
  case 1:
    sprintf_core(auStack_48,(char *)guest_at(0x80121fb8),iVar6,iVar5,iVar4);
    print_dispatch((const char *)guest_at(0x80121fc8),(const char *)arg5,auStack_48);
    goto LAB_80164124;
  case 2:
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + 0x10) * 0x10000 >> 0x10),(uint16_t)sVar3,1);
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + 0xa0) * 0x10000 >> 0x10),(uint16_t)sVar3,1);
    break;
  case 3:
    print_dispatch((const char *)guest_at(0x80121fd4),(const char *)arg5,iVar2);
LAB_80164124:
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    print_dispatch(auStack_48);
    break;
  case 4:
    iVar5 = sprintf_core(auStack_48,(char *)guest_at(0x80121fe0),iVar2,iVar1,iVar7);
    iVar6 = (int32_t)sVar3;
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + (3 - iVar5) * 0x10) * 0x10000 >> 0x10),(uint16_t)iVar6,4);
    announce_param_block_init_b(tbl,a1,guest_at(0x80121fe4),(uint16_t)((a2 + 0x30) * 0x10000 >> 0x10),(uint16_t)iVar6,1);
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + 0xa0) * 0x10000 >> 0x10),(uint16_t)iVar6,1);
    announce_param_block_init_b(tbl,a1,arg5,(uint16_t)((a2 + 0x130) * 0x10000 >> 0x10),(uint16_t)iVar6,4);
    break;
  case 5:
    iVar5 = sprintf_core(auStack_48,(char *)guest_at(0x80121fe0),iVar1,iVar1,iVar7);
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + (3 - iVar5) * 0x10) * 0x10000 >> 0x10),(uint16_t)sVar3,4);
    iVar5 = (a3 + 10) * 0x10000 >> 0x10;
    hud_text_a(tbl,a1,guest_at(0x80121fe4),(uint16_t)((a2 + 0x30) * 0x10000 >> 0x10),(uint16_t)iVar5,1);
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    hud_text_a(tbl,a1,auStack_48,(uint16_t)((a2 + 0x74) * 0x10000 >> 0x10),(uint16_t)iVar5,1);
    announce_param_block_init_b(tbl,a1,arg5,(uint16_t)((a2 + 0xb4) * 0x10000 >> 0x10),(uint16_t)sVar3,4);
    break;
  case 6:
    iVar5 = sprintf_core(auStack_48,(char *)guest_at(0x80121fe0),iVar1,iVar1,iVar7);
    announce_param_block_init_b(tbl,a1,auStack_48,(uint16_t)((a2 + (3 - iVar5) * 0x10) * 0x10000 >> 0x10),(uint16_t)sVar3,4);
    iVar5 = (a3 + 10) * 0x10000 >> 0x10;
    hud_text_a(tbl,a1,guest_at(0x80121fe4),(uint16_t)((a2 + 0x30) * 0x10000 >> 0x10),(uint16_t)iVar5,1);
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    hud_text_a(tbl,a1,auStack_48,(uint16_t)((a2 + 0x74) * 0x10000 >> 0x10),(uint16_t)iVar5,1);
    break;
  case 0x12:
    hud_text_b(tbl,a1,auStack_48,a2 + 0x10,(uint16_t)a3,5);
    goto LAB_801645f8;
  case 0x13:
    if ((_DAT_1f8000dc & 4) == 0) uVar8 = 0; else uVar8 = 5;
    hud_text_c(tbl,a1,auStack_48,a2 + 0x10,(uint16_t)a3,uVar8);
    goto LAB_80164538;
  case 0x14:
    iVar2 = iVar1;
    goto LAB_8016457c;
  case 0x15:
    iVar5 = sprintf_core(auStack_48,(char *)guest_at(0x80121fe0),iVar1,iVar1,iVar7);
    if ((_DAT_1f8000dc & 4) == 0) {
      hud_text_c(tbl,a1,auStack_48,a2 + (3 - iVar5) * 0x10 + 0x10,(uint16_t)a3,0);
      uVar8 = 0;
    } else {
      hud_text_c(tbl,a1,auStack_48,a2 + (3 - iVar5) * 0x10 + 0x10,(uint16_t)a3,5);
      uVar8 = 5;
    }
    announce_param_block_init_alt(tbl,a1,guest_at(0x80121fe4),(uint16_t)(a2 + 0x40),(uint16_t)a3,uVar8);
LAB_80164538:
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    announce_param_block_init_alt(tbl,a1,auStack_48,(uint16_t)(a2 + 0xf8),(uint16_t)a3,2);
    break;
  case 0x16:
LAB_8016457c:
    iVar5 = sprintf_core(auStack_48,(char *)guest_at(0x80121fe0),iVar2,iVar1,iVar7);
    hud_text_b(tbl,a1,auStack_48,a2 + (3 - iVar5) * 0x10 + 0x10,(uint16_t)a3,5);
    announce_param_block_init(tbl,a1,guest_at(0x80121fe4),(uint16_t)((a2 + 0x40) * 0x10000 >> 0x10),(uint16_t)sVar3,5);
LAB_801645f8:
    announce_param_block_init(tbl,a1,arg5,(uint16_t)((a2 + 0xa8) * 0x10000 >> 0x10),(uint16_t)sVar3,0);
    slot_name_measure(auStack_48,*(uint8_t *)(arg5 + 4));
    announce_param_block_init(tbl,a1,auStack_48,(uint16_t)((a2 + 0xf8) * 0x10000 >> 0x10),(uint16_t)sVar3,2);
  }
  br2_guest_frame_free();
}

extern void flow_cfg_init(int);
extern int battle_struct_init(uint32_t,uint32_t);
extern void record_copy_1a(int);
extern int per_side_setup(int,int);
extern int sprite_anim_chain_build(uint32_t);
extern void clear_f0300_records(void);

/* rec_copy_init — if byte @0x1f800099 == 0: copy 0x44-byte record
 * 0x8011f484 + idx*0x44 -> 0x1f8002f8, init record @0x801f0228
 * (+0=src guest addr, +4/+6/+8/+0xa u16=1, +0x14 u16=0).
 * Disasm 0x8012e97c-0x8012ea10. */
void rec_copy_init(uint32_t rec)
{
  uint32_t src = 0x8011f484 + rec * 0x44;
  if (*(int8_t *)guest_at(0x1f800099) == 0) {
    uint8_t *d = (uint8_t *)guest_at(0x1f8002f8);
    memcpy(d, guest_at(src), 0x44);
    {
      uint8_t *r = (uint8_t *)guest_at(0x801f0228);
      *(uint16_t *)(r + 0x14) = 0;
      *(uint16_t *)(r + 4) = 0;
      *(uint16_t *)(r + 0xa) = 1;
      *(uint16_t *)(r + 8) = 1;
      *(uint16_t *)(r + 6) = 1;
      *(uint32_t *)r = src;
    }
  }
}

/* fighter_reinit_tail — fighter re-init tail: gate on byte @0x801e3d00 >= 0,
 * then battle_camera_step/fighter_tint_fade/fighter_trail_emit/slot_height_proj chain.
 * Disasm 0x80166f64-0x80167004. */
int fighter_reinit_tail(uint32_t f)
{
  uint8_t *s0 = (uint8_t *)guest_at(f);
  if (*(int8_t *)guest_at(0x801e3d00) < 0) return -1;
  *(uint8_t *)(s0 + 0x9d) = 0;
  battle_camera_step(*(uint32_t *)(s0 + 0xbc),(uint8_t *)guest_at(0x1f8000e8),(uint8_t *)guest_at(0x1f8001f0),(int)guest_of(s0));
  fighter_tint_fade(*(uint32_t *)(s0 + 8),*(uint32_t *)(s0 + 0xc),(int)guest_of(s0),(int)(int8_t)*(uint8_t *)(s0 + 0x98));
  fighter_trail_emit((undefined4 *)(uintptr_t)(s0 + 0x28),0x1f80033c,*(int *)(s0 + 0xbc),(int)guest_of(s0));
  slot_height_proj(*(uint32_t *)(s0 + 8) + 0x207c,*(uint32_t *)(s0 + 0xbc),*(uint32_t *)(s0 + 0xc),(int)guest_of(s0));
  return 0;
}

/* fighter_respawn_init — fighter respawn/init: gate @0x801e3d00, save/restore
 * byte +0x99, flow_cfg_init(0), battle_struct_init, zero +9d/+9e/+99,
 * record_copy_1a/+0x98 idx, per_side_setup, rec_copy_init, sprite_anim_chain_build,
 * clear_f0300_records. Disasm 0x80166eb0-0x80166f60. */
int fighter_respawn_init(uint32_t f)
{
  uint8_t *s0 = (uint8_t *)guest_at(f);
  int8_t s1 = *(int8_t *)(s0 + 0x99);
  int8_t gv = *(int8_t *)guest_at(0x801e3d00);
  uint8_t gu = *(uint8_t *)guest_at(0x801e3d00);
  if (gv < 0) return -1;
  *(uint8_t *)(s0 + 0x98) = gu;
  flow_cfg_init(0);
  battle_struct_init(*(uint32_t *)(s0 + 0xbc),f);
  *(uint8_t *)(s0 + 0x9d) = 0;
  *(uint8_t *)(s0 + 0x9e) = 0;
  *(uint8_t *)(s0 + 0x99) = 0;
  record_copy_1a((int)(int8_t)*(uint8_t *)(s0 + 0x98));
  per_side_setup((int)(int8_t)*(uint8_t *)(s0 + 0x98),(int)(int8_t)*(uint8_t *)(s0 + 0x99));
  rec_copy_init((uint8_t)*(uint8_t *)(s0 + 0x98));
  sprite_anim_chain_build(f);
  clear_f0300_records();
  *(uint8_t *)(s0 + 0x99) = (uint8_t)s1;
  return 0;
}

/* ================= guest task callbacks (payload ports) =================
 * Registered via task_node_alloc/node_lazy_attach as 32-bit guest code addrs;
 * dispatched through fn_lookup -> task_invoke. param_1 is a
 * HOST pointer into the node's inline guest data area; fields that hold
 * guest addresses are accessed via guest_at / stored via guest_of. */

/* LAB_80137590 — GTE control-word reload task (slot 9). Guest 0x80137590. */
void LAB_80137590(void)
{
  setCopControlWord(2,0,((uint32_t *)g_DAT_801be838)[0]);
  setCopControlWord(2,0x800,((uint32_t *)g_DAT_801be838)[1]);
  setCopControlWord(2,0x1000,((uint32_t *)g_DAT_801be838)[2]);
  setCopControlWord(2,0x1800,((uint32_t *)g_DAT_801be838)[3]);
  setCopControlWord(2,0x2000,((uint32_t *)g_DAT_801be838)[4]);
  setCopControlWord(2,0x2800,((uint32_t *)g_DAT_801be838)[5]);
  setCopControlWord(2,0x3000,((uint32_t *)g_DAT_801be838)[6]);
  setCopControlWord(2,0x3800,((uint32_t *)g_DAT_801be838)[7]);
  return;
}

/* LAB_80137680 — channel-script cursor task. Guest 0x80137680.
 * Data layout (guest u32 fields): [0]/[1] output ptrs, [2] jump-table
 * base, [3] u16 table base, [4] script cursor, [5] frame countdown,
 * [6] repeat counter. */
void LAB_80137680(int *ctx)
{
  short w;
  ushort arg;
  uint op;
  ushort *ip;
  uint subop;

  if (_DAT_1f800082 == 0) {
    ctx[5] = ctx[5] + -1;
  }
  if (ctx[5] < 1) {
    ctx[4] = ctx[4] + 4;
    w = *(short *)guest_at((uint32_t)ctx[4]);
    while (((int)w & 0xff00U) != 0) {
      ip = (ushort *)guest_at((uint32_t)ctx[4]);
      subop = *ip & 0xff;
      op = *ip & 0xff00;
      if (op == 0xfe00) {
        arg = ip[1];
LAB_80137758:
        ctx[4] = ctx[2] + (short)arg * 4;
      }
      else if (op < 0xfe01) {
        if (op == 0xfc00) {
          if ((int)subop < ctx[6]) goto LAB_80137790;
          ctx[6] = ctx[6] + 1;
          arg = *(ushort *)guest_at((uint32_t)(ctx[4] + 2));
          goto LAB_80137758;
        }
      }
      else if (op == 0xff00) {
        if ((int)subop <= ctx[6]) {
          task_slot_step();
          return;
        }
LAB_80137790:
        ctx[4] = (int)guest_of(ip + 2);
      }
      w = *(short *)guest_at((uint32_t)ctx[4]);
    }
    *(undefined2 *)guest_at((uint32_t)ctx[0]) =
        *(undefined2 *)guest_at((uint32_t)(*(short *)guest_at((uint32_t)ctx[4]) * 2 + ctx[3]));
    *(undefined2 *)guest_at((uint32_t)ctx[1]) =
        *(undefined2 *)guest_at((uint32_t)(*(short *)guest_at((uint32_t)ctx[4]) * 2 + ctx[3]));
    ctx[5] = (int)*(short *)guest_at((uint32_t)(ctx[4] + 2));
  }
  return;
}

/* LAB_80137acc — channel command interpreter task. Guest 0x80137acc.
 * [2] script cursor (guest addr), [3] handler table base (guest addr),
 * [1] record table base (guest addr); byte +0x10 current op,
 * byte +0x12 repeat index. */
void LAB_80137acc(int *ctx)
{
  short idx;
  short op;
  short *rec;
  int t;
  short *opp;
  int *curp;

  /* Bail-unwind in flight (g_psx_call_bail): the generated interpreter's CPS
   * frames return immediately without consuming script records. Checking
   * here keeps the cursor/tick untouched so the pending records re-fire
   * once the unwind resolves — otherwise every handler below would be
   * silently dropped by br2_call_generated while the cursor still advanced
   * (the announce-wedge DAT1=9/0xF0 loss, proof/announce_script_handler_drop.md). */
  { extern int br2_bail_pending(void);
    if (br2_bail_pending()) return; }
  { extern uint32_t g_tramp_calls,g_tramp_last_node,g_tramp_last_cur,g_tramp_last_op;
    g_tramp_calls++;
    g_tramp_last_node = (uint32_t)(uintptr_t)guest_of(ctx);
    g_tramp_last_cur  = (uint32_t)ctx[2];
    g_tramp_last_op   = (uint32_t)*(unsigned char *)((intptr_t)ctx + 0x10); }
  opp = (short *)((intptr_t)ctx + 0x10);
  curp = ctx + 2;
  if (_DAT_1f800082 != 0) {
    return;
  }
  do {
    while( true ) {
      rec = (short *)guest_at((uint32_t)*curp);
      op = *rec;
      if (op != 0x2000) break;
      if (*(short *)((intptr_t)ctx + 0x12) < rec[1]) {
        rec = (short *)guest_at((uint32_t)(ctx[1] + rec[2] * 6));
        *curp = (int)guest_of(rec);
        idx = *(short *)((intptr_t)ctx + 0x12);
        op = *rec;
LAB_80137bb8:
        *opp = op;
        *(short *)((intptr_t)ctx + 0x12) = idx + 1;
      }
      else {
LAB_80137bc8:
        *curp = (int)guest_of(rec + 3);
        *(undefined2 *)((intptr_t)ctx + 0x12) = 0;
      }
    }
    if (op < 0x2001) {
      if (op == 0x1000) {
        if (rec[1] <= *(short *)((intptr_t)ctx + 0x12)) goto LAB_80137bc8;
        *curp = (int)guest_of(rec + -3);
        idx = *(short *)((intptr_t)ctx + 0x12);
        op = rec[-3];
        goto LAB_80137bb8;
      }
      goto LAB_80137c14;
    }
    if (op != 0x3000) {
      if (op == 0x4000) {
        task_slot_step();
        return;
      }
LAB_80137c14:
      rec = (short *)guest_at((uint32_t)*curp);
      if (*opp == *rec) {
        do {
          t = 0;
          if (0 < rec[1]) {
            do {
              t = t + 1;
              {
                /* guest fn-ptr table: ctx[3] + rec[2]*4 */
                uint32_t fn_g = *(uint32_t *)guest_at((uint32_t)(ctx[3] + rec[2] * 4));
                void *fn = fn_lookup(fn_g);
                if (fn != (void *)0x0) {
                  ((void (*)(uintptr_t))fn)(ctx[0] != 0 ? (uintptr_t)guest_at((uint32_t)ctx[0]) : 0);
                }
                else {
                  /* Unported jalr target: ctx = ctx[0] (guest addr or 0). */
                  br2_call_clean(fn_g,(uint32_t)ctx[0],0,0,0);
                }
              }
              rec = (short *)guest_at((uint32_t)*curp);
            } while (t < rec[1]);
          }
          /* A handler began a bail unwind: do NOT advance the script cursor
           * or tick — the generated frame would return here and the record
           * group re-fires on the next invocation after the unwind resolves.
           * Advancing now would consume records whose handlers were dropped. */
          { extern int br2_bail_pending(void);
            if (br2_bail_pending()) return; }
          t = *curp;
          rec = (short *)guest_at((uint32_t)(t + 6));
          *curp = (int)guest_of(rec);
        } while (*opp == *(short *)guest_at((uint32_t)(t + 6)));
      }
      *opp = *opp + 1;
      return;
    }
    rec = (short *)guest_at((uint32_t)(ctx[1] + rec[1] * 6));
    *curp = (int)guest_of(rec);
    op = *rec;
    *(undefined2 *)((intptr_t)ctx + 0x12) = 0;
    *opp = op;
  } while( true );
}

/* LAB_80137cd0 — channel-script cursor task (variant B). Guest 0x80137cd0.
 * [1] kill-notify ptr, [2] u16 table base, [3] jump-table base,
 * [4] script cursor; byte +0x14 countdown, byte +0x16 repeat index. */
void LAB_80137cd0(int *ctx)
{
  short w;
  uint operand;
  short *ops;
  uint op;
  ushort *ip;

  if (_DAT_1f800082 == 0) {
    *(short *)(ctx + 5) = *(short *)(ctx + 5) + -1;
  }
  if (*(short *)(ctx + 5) < 1) {
    ctx[4] = ctx[4] + 4;
    w = *(short *)guest_at((uint32_t)ctx[4]);
    while (((int)w & 0xff00U) != 0) {
      ip = (ushort *)guest_at((uint32_t)ctx[4]);
      operand = *ip & 0xff;
      op = *ip & 0xff00;
      if (op == 0xfe00) {
        ctx[4] = ctx[3] + operand * 4;
      }
      else if (op < 0xfe01) {
        if (op == 0xfc00) {
          if ((int)operand < *(short *)((intptr_t)ctx + 0x16)) {
            ctx[4] = (int)guest_of(ip + 2);
          }
          else {
            *(short *)((intptr_t)ctx + 0x16) = *(short *)((intptr_t)ctx + 0x16) + 1;
            ctx[4] = ctx[3] + *(short *)guest_at((uint32_t)(ctx[4] + 2)) * 4;
          }
        }
      }
      else if (op == 0xff00) {
        if ((int)operand <= *(short *)((intptr_t)ctx + 0x16)) {
          if (ctx[1] != 0) {
            *(undefined4 *)guest_at((uint32_t)ctx[1]) = 0;
          }
          task_slot_step();
          return;
        }
        ctx[4] = (int)guest_of(ip + 2);
      }
      w = *(short *)guest_at((uint32_t)ctx[4]);
    }
    ops = (short *)guest_at((uint32_t)(ctx[2] + *(short *)guest_at((uint32_t)ctx[4]) * 4));
    {
      /* guest: jal 0x801ACB08 — ctx = *(u32*)(ctx+0) (guest word),
       * a1 = s16 *ops, a2 = s16 ops[1] (sign-extended lh loads). */
      {
        void *fn = fn_lookup(0x801acb08);
        if (fn != (void *)0x0) {
          ((void (*)(uint32_t,int,int))fn)((uint32_t)ctx[0],(int)*ops,(int)ops[1]);
        }
        else {
          fighter_chan_delta_accum((uint32_t)ctx[0],
                                   (uint32_t)(int)*ops,
                                   (uint32_t)(int)ops[1],0);
        }
      }
    }
    *(undefined2 *)(ctx + 5) = *(undefined2 *)guest_at((uint32_t)(ctx[4] + 2));
  }
  return;
}

/* LAB_8013eb04 — speech/effect emitter task (spawns 0x8013e72c subtasks).
 * Guest 0x8013eb04. Data: [4] flag, [5] record-table base, [6] cursor,
 * byte +0x1c index, byte +0x1e countdown; [2]/[3] receive RTPS outputs. */
void LAB_8013eb04(int *a0)
{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar4;
  undefined4 *puVar5;
  undefined1 *puVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  uint16_t uVar3_;

  if (_DAT_1f800082 == 0) {
    if (*(short *)guest_at((uint32_t)a0[6]) < 0) {
      uVar3_ = *(uint16_t *)((intptr_t)a0 + 0x1e) - 1;
      *(uint16_t *)((intptr_t)a0 + 0x1e) = uVar3_;
      if ((int)((uint)uVar3_ << 0x10) < 0) {
        task_slot_step();
        return;
      }
    }
    else {
      if (*(short *)(a0 + 7) == *(short *)guest_at((uint32_t)a0[6])) {
        do {
          iVar4 = (int)(intptr_t)task_node_alloc(1,2,0xe,0x8013e72c,0,0x200);
          if (iVar4 != 0) {
            iVar11 = 0;
            iVar10 = 0;
            puVar8 = (undefined4 *)guest_at(*(uint32_t *)guest_at((uint32_t)(iVar4 + 0x10)));
            iVar4 = *(short *)guest_at((uint32_t)(a0[6] + 2)) * 0x12 + a0[5];
            uVar12 = *(undefined4 *)guest_at((uint32_t)(iVar4 + -0xe));
            uVar13 = *(undefined4 *)guest_at((uint32_t)(iVar4 + -10));
            uVar14 = *(undefined4 *)guest_at((uint32_t)(iVar4 + -6));
            puVar8[0x78] = *(undefined4 *)guest_at((uint32_t)(iVar4 + -0x12));
            puVar8[0x79] = uVar12;
            puVar8[0x7a] = uVar13;
            puVar8[0x7b] = uVar14;
            *(undefined2 *)((intptr_t)puVar8 + 0x1f0) = *(undefined2 *)guest_at((uint32_t)(iVar4 + -2));
            puVar8[0x7d] = (int)guest_of((char *)a0 + 8);
            puVar8[0x7e] = (int)guest_of((char *)a0 + 0xc);
            puVar9 = puVar8;
            do {
              iVar7 = 0;
              puVar5 = puVar9;
              puVar6 = (undefined1 *)guest_at(0x801b1ec4);
              iVar4 = iVar10;
              do {
                *puVar5 = 0x9000000;
                *(undefined1 *)((intptr_t)puVar5 + 7) = 0x2e;
                *(undefined2 *)((intptr_t)puVar8 + iVar4 + 0x16) = 0x39;
                uVar2 = 0x7a5a;
                if (a0[4] == 0) {
                  uVar2 = 0x7a9a;
                }
                *(undefined2 *)((intptr_t)puVar8 + iVar4 + 0xe) = uVar2;
                *(undefined1 *)(puVar5 + 1) = 0xe0;
                *(undefined1 *)((intptr_t)puVar5 + 5) = 0xe0;
                *(undefined1 *)((intptr_t)puVar5 + 6) = 0xe0;
                uVar1 = puVar6[1];
                *(undefined1 *)(puVar5 + 3) = *puVar6;
                *(undefined1 *)((intptr_t)puVar5 + 0xd) = uVar1;
                uVar1 = puVar6[3];
                *(undefined1 *)(puVar5 + 5) = puVar6[2];
                *(undefined1 *)((intptr_t)puVar5 + 0x15) = uVar1;
                uVar1 = puVar6[5];
                *(undefined1 *)(puVar5 + 7) = puVar6[4];
                *(undefined1 *)((intptr_t)puVar5 + 0x1d) = uVar1;
                uVar1 = puVar6[7];
                *(undefined1 *)(puVar5 + 9) = puVar6[6];
                *(undefined1 *)((intptr_t)puVar5 + 0x25) = uVar1;
                puVar6 = puVar6 + 8;
                puVar5 = puVar5 + 10;
                iVar7 = iVar7 + 1;
                iVar4 = iVar4 + 0x28;
              } while (iVar7 < 6);
              puVar9 = puVar9 + 0x3c;
              iVar11 = iVar11 + 1;
              iVar10 = iVar10 + 0xf0;
            } while (iVar11 < 2);
            puVar8[0x7f] = 0;
            if (*(short *)((intptr_t)a0 + 0x1e) <= *(short *)((intptr_t)puVar8 + 0x1e0)) {
              *(short *)((intptr_t)a0 + 0x1e) = *(short *)((intptr_t)puVar8 + 0x1e0) + 1;
            }
          }
          iVar4 = a0[6];
          a0[6] = iVar4 + 4;
        } while (*(short *)(a0 + 7) == *(short *)guest_at((uint32_t)(iVar4 + 4)));
      }
      *(short *)(a0 + 7) = *(short *)(a0 + 7) + 1;
    }
  }
  /* GTE tail: lwc2 r0/r1 <- param[0..1], cop2 op, swc2 r14 -> param[2],
   * mfc2 r19 -> param[3] >> 2 (disasm 0x8013ed50-0x8013ed7c). */
  setCopReg(2,0,(uint32_t)a0[0]);
  setCopReg(2,1,(uint32_t)a0[1]);
  copFunction(2,0x180001);
  a0[2] = (int)getCopReg(2,14);
  a0[3] = getCopReg(2,0x9800) >> 2;
  return;
}

/* ======== batch29: small task/effect wrappers + list walkers ===== */
/* externs for batch29 callees */
extern int  cd_dispatch_tbl4(int a0,uint32_t a1);
extern int  mdec_stat_wait_a();
extern int  mdec_stat_wait_b();
extern int  mdec_status_read();   /* guest 0x8018fe44 — no C def yet (weak-stub needed) */
extern int  mdec_decode_issue(uint32_t a0,uint32_t a1);
extern void gpu_load_image_cmd();
extern uint32_t gpu_rect(const void *p);

/* guest file_op_dispatch @0x8018fac8 — file-op dispatch wrapper: cd_dispatch_tbl4(0,arg). */
int file_op_dispatch(int op)
{
  return cd_dispatch_tbl4(0,(uint32_t)op);
}

/* guest channel_script_emit @0x8013514c — channel-script emit wrapper. */
void channel_script_emit(int ctx,undefined4 a1,int a2)
{
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8) )+ 0xf0,ctx + 0x2074,a1,3);
  return;
}

/* guest mode_select_wrapper @0x801340f0 — mode select then deferred_cmd_post(0x400,sel,0). */
void mode_select_wrapper(int mode)
{
  if (mode == 0) {
    deferred_cmd_post(0x400,0x80,0);
  }
  else {
    deferred_cmd_post(0x400,0x82,0);
  }
  return;
}

/* guest file_state_bit29_query @0x8018fa50 — file-state query: op0 -> mdec_stat_wait_a(),
 * else mdec_status_read()>>29 & 1. */
int file_state_bit29_query(int st)
{
  int res;
  if (st == 0) {
    res = mdec_stat_wait_a();
  }
  else {
    res = mdec_status_read() >> 0x1d & 1;
  }
  return res;
}

/* guest file_state_bit24_query @0x8018fa8c — file-state query: op0 -> mdec_stat_wait_b(),
 * else mdec_status_read()>>24 & 1. */
int file_state_bit24_query(int st)
{
  int res;
  if (st == 0) {
    res = mdec_stat_wait_b();
  }
  else {
    res = mdec_status_read() >> 0x18 & 1;
  }
  return res;
}

/* guest node_distance_check @0x80133708 — distance check vs node->h2a tracker.
 * Returns -1 when atan2_lut(dx,dy) < current h2a (h2a always rewritten). */
int node_distance_check(int na,int nb,int rng)
{
  int ang;
  short lim;
  ang = atan2_lut(*(int *)guest_at((uint32_t)(uintptr_t)nb) - *(int *)guest_at((uint32_t)(uintptr_t)na),
                       *(int *)guest_at((uint32_t)(uintptr_t)(nb + 8)) - *(int *)guest_at((uint32_t)(uintptr_t)(na + 8)));
  lim = *(short *)guest_at((uint32_t)(uintptr_t)(rng + 0x2a));
  *(short *)guest_at((uint32_t)(uintptr_t)(rng + 0x2a)) = lim;
  if (ang < lim) {
    return -1;
  }
  return 0;
}

/* guest channel_script_emit_pair @0x80135078 — dual channel-script emit (stride pair). */
void channel_script_emit_pair(int ctx,undefined4 a1,int a2)
{
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),ctx + 0x2048,a1,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,ctx + 0x2074,a1,4);
  return;
}

/* guest channel_script_emit_pair_gated @0x801350dc — emit pair; first gated on h(c)>4 (count-4). */
void channel_script_emit_pair_gated(int ctx,undefined4 a1,int a2)
{
  if (4 < *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc))) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,ctx + 0x2074,a1,
                 (int)*(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc)) - 4);
  }
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),ctx + 0x2074,a1,1);
  return;
}

/* guest channel_script_emit_pair_2gate @0x80134128 — emit pair; count-4 gated + h4-count gated. */
void channel_script_emit_pair_2gate(int ctx,undefined4 a1,int a2)
{
  int cnt;
  cnt = (int)*(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc));
  if (cnt - 4 > 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,ctx + 0x2074,a1,cnt - 4);
  }
  cnt = (int)*(short *)guest_at((uint32_t)(uintptr_t)(a2 + 4));
  if (cnt != 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),ctx + 0x2074,a1,cnt);
  }
  return;
}

/* guest channel_script_emit_triple @0x801341a0 — emit triple (2048 x2, 2074 gated on c-5). */
void channel_script_emit_triple(int ctx,undefined4 a1,int a2)
{
  int tmp;
  tmp = ctx + 0x2048;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),tmp,a1,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,tmp,a1,1);
  tmp = (int)*(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc)) - 5;
  if (tmp > 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xc8,ctx + 0x2074,a1,tmp);
  }
  return;
}

/* guest channel_script_emit_chain @0x80134e44 — emit chain; h26 normalized to boolean and
 * selects between (+0xf0,+0x140 / +0x118) tails before the +0xa0 emit. */
void channel_script_emit_chain(int ctx,undefined4 a1,int a2)
{
  int flag;
  int dst;
  dst = ctx + 0x2074;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),dst,a1,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xc8,dst,a1,1);
  flag = (int)*(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26));
  flag = (flag == 0) ? 1 : 0;
  *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26)) = (short)flag;
  if (flag != 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xf0,dst,a1,1);
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x140,dst,a1,2);
  }
  else {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x118,dst,a1,3);
  }
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,ctx + 0x2074,a1,1);
  return;
}

/* guest vec3_midpoint_angles @0x80132128 — midpoint of two vec3s into out+0x42/44/46,
 * then two atan2_lut angle extractions stored at out+0x30/0x32 (+0x34=0). */
void vec3_midpoint_angles(int pa,int pb,int dst)
{
  int mx;
  int my;
  int dx;
  int dx2;
  int t;
  int dz;
  int dz2;
  int dy;
  mx = *(int *)guest_at((uint32_t)(uintptr_t)pa) + *(int *)guest_at((uint32_t)(uintptr_t)pb);
  mx = (mx + (int)((uint32_t)mx >> 0x1f)) >> 1;
  *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x42)) = (short)mx;
  my = *(int *)guest_at((uint32_t)(uintptr_t)(pa + 4)) + *(int *)guest_at((uint32_t)(uintptr_t)(pb + 4));
  my = (my + (int)((uint32_t)my >> 0x1f)) >> 1;
  dx = (short)mx - *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x4a));
  dx2 = dx * dx;
  t = *(int *)guest_at((uint32_t)(uintptr_t)(pa + 8)) + *(int *)guest_at((uint32_t)(uintptr_t)(pb + 8));
  t = (t + (int)((uint32_t)t >> 0x1f)) >> 1;
  dz = (short)t - *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x4e));
  dz2 = dz * dz;
  *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x44)) = (short)my;
  *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x46)) = (short)t;
  dy = (short)my - *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x4c));
  t = FUN_80194500(dx2 + dz2,pb);
  t = atan2_lut(t,dy);
  *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x30)) = (short)t;
  t = atan2_lut(dz,dx);
  *(short *)guest_at((uint32_t)(uintptr_t)(dst + 0x32)) = (short)t;
  *(undefined2 *)guest_at((uint32_t)(uintptr_t)(dst + 0x34)) = 0;
  return;
}

/* guest task_node_alloc_init @0x8013f714 — alloc/init task node via task_node_alloc,
 * callback 0x8013f648. True entry is +8 vs listed 0x8013f71c (DAT_1f8000b1
 * flag load precedes the frame). Returns node ptr or the flag byte. */
int task_node_alloc_init(int kind,int cnt,int sz)
{
  int node;
  int *rec;
  int out;
  out = (int)DAT_1f8000b1;
  if (out != 0) {
    return out;
  }
  node = (int)(intptr_t)task_node_alloc(1,2,8,0x8013f648,0,0xc);
  if (node == 0) {
    return 0;
  }
  out = *(int *)guest_at((uint32_t)(uintptr_t)(node + 0x10));
  rec = (int *)guest_at((uint32_t)(uintptr_t)out);
  rec[0] = kind;
  rec[1] = cnt;
  if (sz < 0) {
    sz = 0;
  }
  *(short *)(rec + 2) = (short)sz;
  return out;
}

/* guest task_cb_voice_line @0x8013f648 — task callback: voice/line table lookup +
 * fx_group_alloc alloc + noop_b register, then task_slot_step. */
void task_cb_voice_line(int arg)
{
  int ent;
  int t;
  int base;
  ent = *(int *)guest_at((uint32_t)(uintptr_t)arg);
  t = (uint)*(byte *)guest_at((uint32_t)(uintptr_t)(ent + 2));
  t = (int)*(byte *)guest_at((uint32_t)(uintptr_t)(t + 0x801f0ad8));
  if (t != 0) {
    task_slot_step();
    return;
  }
  if (*(short *)guest_at(0x1f800082) != 0) {
    return;
  }
  t = (int)*(short *)guest_at((uint32_t)(uintptr_t)(arg + 8));
  if (t != 0) {
    *(short *)guest_at((uint32_t)(uintptr_t)(arg + 8)) = (short)t;
    return;
  }
  ent = fx_group_alloc(1,1,0,9);
  if (ent == 0) {
    task_slot_step();
    return;
  }
  t = *(int *)guest_at((uint32_t)(uintptr_t)arg);
  base = *(int *)guest_at((uint32_t)(uintptr_t)(t + 0x20));
  noop_b(ent,base + *(int *)guest_at((uint32_t)(uintptr_t)(arg + 4)) * 0x40 + t * 0x10 + 0x34,
                (undefined4)br2_gaddr(&DAT_801f0760));
  task_slot_step();
  return;
}


/* ======== batch30 (rebuilt): emit chains + save/record iterators ===== */
/* externs for batch30 callees */
extern int  cd_search_file(undefined4 *arg,uint32_t a1);  /* guest 0x80197b30 — battle.c */
extern int cd_file_rec_find(uint32_t arg,uint32_t a1);  /* cd.c 0x801980ec */
extern int cd_strncmp12_eq(uint32_t arg,uint32_t a1);  /* cd.c 0x80197e08 */
extern uint32_t cd_dir_scan(uint32_t arg,uint32_t a1);  /* cd.c 0x80197e28 */
extern int  cd_mode_flag_set(int arg,void *a1,int a2);
extern int  cd_timer_read(int arg);
extern uint32_t call_guest(uint32_t fn, uint32_t a, uint32_t b);
extern void part_matrix_compute(undefined4 *arg,byte *a1,short *a2,undefined4 a3);

/* guest channel_script_emit_chain_tick @0x80134b34 — emit chain on 203c/2078/2074 bases;
 * h26 counter +1 (h28 +=0x400 every 4th), de6c+86f5c on s1+0xc4. */
void channel_script_emit_chain_tick(int ctx,int a1,int a2,int a3)
{
  int tmp;
  tmp = ctx + 0x203c;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x140,tmp,a1,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)),tmp,a1,4);
  tmp = ctx + 0x2078;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xc8,tmp,a1,3);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),tmp,a1,1);
  tmp = (int)*(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26)) + 1;
  *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26)) = (short)tmp;
  if ((tmp & 3) == 0) {
    *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x28)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0x28)) + 0x400);
  }
  rotmat_rtps_build((uint)(int)*(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x28)),
               (uint *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc4)));
  gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)a3),(undefined2 *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc4)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,ctx + 0x2074,a1,1);
  return;
}

/* guest packet_timer_update @0x8018499c — packet/timer update on s0: h3c countdown
 * (reload high byte of *h24 on 0), advance h24 by 4 on last tick, then when
 * lh6e!=0 and lw78 within 0xbf of s0+0x7c run sprintf_core and advance
 * lw78 by its return. */
int packet_timer_update(int u0,int u1,int pkt)
{
  int p;
  int t;
  int w;
  p = pkt;
  if (*(short *)guest_at((uint32_t)(uintptr_t)(p + 0x3c)) == 0) {
    *(short *)guest_at((uint32_t)(uintptr_t)(p + 0x3c)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)))) >> 8);
  }
  t = (int)*(ushort *)guest_at((uint32_t)(uintptr_t)(p + 0x3c)) - 1;
  *(short *)guest_at((uint32_t)(uintptr_t)(p + 0x3c)) = (short)t;
  *(ushort *)guest_at((uint32_t)(uintptr_t)(p + 6)) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)) + 2));
  if (t == 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)) =
      *(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)) + 4;
  }
  if ((*(short *)guest_at((uint32_t)(uintptr_t)(p + 0x6e)) != 0) &&
      ((*(int *)guest_at((uint32_t)(uintptr_t)(p + 0x78)) - 0x7c - p) < 0xbf)) {
    w = (int)*(ushort *)guest_at((uint32_t)(uintptr_t)(p + 6));
    t = sprintf_core((char *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(p + 0x78)))),
                 (char *)guest_at(0x80123750),
                 *(int *)guest_at((uint32_t)(uintptr_t)(0x801b52f0 + ((w >> 0xc) << 2))),
                 *(int *)guest_at((uint32_t)(uintptr_t)(0x801b52f0 + ((w >> 3) & 0x1c) + 0x40)),
                 (w & 0x400) != 0 ? (char *)guest_at(0x801230ac) : (char *)guest_at(0x801230b0));
    *(int *)guest_at((uint32_t)(uintptr_t)(p + 0x78)) =
      *(int *)guest_at((uint32_t)(uintptr_t)(p + 0x78)) + t;
  }
  return 0;
}

/* guest node_table_lookup_emit @0x801863b4 — node lookup (flag5&1 picks 0x1c-stride
 * table via *(p1+0x38)+0x10/+0x30), FUN_80194660 fill, h2(s1)+=0x1000,
 * halves h6 into s1+6, part_matrix_compute + rtps_point_xform2/gte_rtps_xform3 emit. */
void node_table_lookup_emit(int f,int node,int out,int idx)
{
  int ent;
  int half;
  int mb;
  int sel;
  sel = (*(byte *)guest_at((uint32_t)(uintptr_t)(f + 5)) & 1) * 0x1c;
  ent = *(int *)guest_at((uint32_t)(uintptr_t)(sel +
          *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(f + 0x38)) + 0x10))));
  ent = ent + *(int *)guest_at((uint32_t)(uintptr_t)(f + 0x30));
  ent = (int)*(short *)guest_at((uint32_t)(uintptr_t)(ent + idx * 2)) * 0x1c +
          *(int *)guest_at((uint32_t)(uintptr_t)(f + 0x40));
  mb = *(int *)guest_at((uint32_t)(uintptr_t)(node + 0x40));
  FUN_80194660((undefined2 *)guest_at((uint32_t)(uintptr_t)ent),(undefined2 *)guest_at((uint32_t)(uintptr_t)out));
  *(short *)guest_at((uint32_t)(uintptr_t)(out + 2)) =
    (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(out + 2)) + 0x1000);
  half = (short)*(short *)guest_at((uint32_t)(uintptr_t)(ent + 6)) / 2;
  *(short *)guest_at((uint32_t)(uintptr_t)(out + 6)) = (short)half;
  part_matrix_compute((undefined4 *)guest_at((uint32_t)(uintptr_t)(node + 4)),
               (byte *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(f + 0x48)) + idx * 0x20)),
               (short *)guest_at((uint32_t)(uintptr_t)out),half / 2);
  ent = *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)mb + 0x40))) + 0x20;
  rtps_point_xform2((undefined4 *)guest_at((uint32_t)(uintptr_t)ent),
               (undefined4 *)guest_at((uint32_t)(uintptr_t)(node + 0x18)),
               (int *)guest_at((uint32_t)(uintptr_t)(node + 0x34)));
  gte_rtps_xform3((undefined4 *)guest_at((uint32_t)(uintptr_t)ent),
               (ushort *)guest_at((uint32_t)(uintptr_t)(node + 4)),
               (uint *)guest_at((uint32_t)(uintptr_t)(node + 0x20)));
  return;
}

/* guest camera_scroll_adjust @0x80186bfc — camera/scroll adjust: nudge 0x801f5958
 * halfwords by table_byte_next_a flags, then lut_sin_q12-driven table lookup,
 * rotmat_rtps_build + gte_rtps_xform1 transform emit. */
void camera_scroll_adjust(int cam)
{
  int t;
  int sy;
  int dy;
  ushort *cs;
  ushort *lut;
  ushort vec [4];
  uint rot [6];
  t = table_byte_next_a();
  cs = (ushort *)guest_at(0x801f5958);
  if ((t & 1) != 0) {
    cs[6] = (ushort)(cs[6] + 0x40);
  }
  else {
    cs[6] = (ushort)(cs[6] - 0x40);
  }
  if ((t & 2) != 0) {
    cs[7] = (ushort)(cs[7] + 0x80);
  }
  else {
    cs[7] = (ushort)(cs[7] - 0x80);
  }
  t = lut_sin_q12((int)(short)cs[6]);
  lut = (ushort *)guest_at(0x801237e8 + (((cam << 0x10) >> 0xe)));
  sy = lut_sin_q12((int)(short)cs[7]);
  cs[4] = (ushort)(lut[0] + ((t << 9) >> 0xc));
  dy = (int)*(short *)guest_at((uint32_t)(uintptr_t)(0x801237e8 + (((cam << 0x10) >> 0xe)) + 2)) * sy;
  vec[0] = 0;
  vec[1] = 0;
  t = (ushort)(lut[1] + (dy >> 0xc));
  vec[2] = (ushort)t;
  vec[3] = 0;
  cs[5] = (ushort)t;
  rotmat_rtps_build((uint)(int)(short)cs[4],rot);
  gte_rtps_xform1((undefined4 *)rot,(undefined4 *)vec,(undefined2 *)cs);
  return;
}

/* guest record_init_sweep @0x8018d8e4 — pad/record init: retry cd_mode_flag_set(0xe)
 * until success, cd_timer_read(3) once via 0x801c4580 flag, reads count
 * @0x801b66dc (0 -> 0x169 sweep of 0x801b5b94 8-byte records), per record
 * cd_search_file decode then writes *slot = ptr and rec+4 = (size+0x7ff)>>0xb. */
void pad_record_init(void)
{
  int t;
  int off;
  int md;
  int tb;
  int ent;
  int nd;
  byte abStack_28 [4];
  undefined4 auStack_10 [8];
  abStack_28[0] = 0x80;
  do {
    t = cd_mode_flag_set(0xe,abStack_28,0);
  } while (t == 0);
  if (*(byte *)guest_at(0x801c4580) == 0) {
    cd_timer_read(3);
  }
  *(byte *)guest_at(0x801c4580) = 1;
  t = (int)*(short *)guest_at(0x801b66dc);
  ent = 0x801b5b94;
  tb = 0x801b66dc;
  if (t == 0) {
    t = 0x169;
    md = 1;
  }
  else {
    t = 0;
    md = 0;
  }
  if (0 < t) {
    off = 2;
    tb = 0;
    do {
      /* rec = 0x801b5b94 + (mode? iter*8 : lh(0x801b66dc+off)*8) */
      if (md == 0) {
        ent = (*(short *)guest_at((uint32_t)(uintptr_t)(off + 0x801b66dc)) << 3) + 0x801b5b94;
      }
      else {
        ent = tb * 8 + 0x801b5b94;
      }
      nd = *(int *)guest_at((uint32_t)(uintptr_t)ent);
      if ((*(int *)guest_at((uint32_t)(uintptr_t)nd) != 0) && (cd_search_file(auStack_10,nd) != 0)) {
        *(undefined4 *)guest_at((uint32_t)(uintptr_t)nd) = auStack_10[0];
        *(int *)guest_at((uint32_t)(uintptr_t)(ent + 4)) =
          (int)(((uint)auStack_10[1] + 0x7ff) >> 0xb);
      }
      t = t - 1;
      off = off + 2;
      tb = tb + 1;
    } while (0 < t);
  }
  return;
}

/* guest cd_search_file @0x80197b30 — CdSearchFile: resolve '\'-separated
 * path a1 (guest str, must start with 0x5c) through the CD directory
 * tables. Each token before the last is looked up via cd_file_rec_find
 * (parent-id chain starting at 1, max 8 levels); the final component is
 * matched by FUN_80198190, then the 0x40-entry file table @0x801fc700
 * (stride 0x18, flag/name at +8) is scanned with cd_strncmp12_eq. On hit the
 * 0x18-byte record is copied to a0 (host) and its guest addr returned.
 * Verbosity gate: lw 0x801cc850 (>0 prints, >=2 prints search hits).
 * 0x801ccb74 caches the media-change generation @0x801cc85c; mismatch
 * re-inits via FUN_80197e28 (unported -> call_guest). */
int cd_search_file(undefined4 *path,uint32_t arg)
{
  const uint8_t *name = (const uint8_t *)guest_at(arg);
  const uint8_t *s0;
  uint8_t *s1;
  /* token buffer must live in guest RAM: cd_file_rec_find/cd_strncmp12_eq
   * receive it as a u32 guest address (cd_arg_ptr). Guest frame is 0x50
   * (addiu sp,-0x50 @0x80197b40) with the 64-byte token buf at sp+0, so
   * reserve the window on the live guest sp — callee frames would
   * otherwise land on lbuf between the per-token rebuild and the
   * cd_strncmp12_eq scan. gpu_rect uses sp-0x10..sp-1, so the guest's
   * own sp-0x50..sp-0x11 scratch is reproduced by frm+0..frm+0x3f. */
  extern uint32_t br2_guest_frame(uint32_t);
  extern void br2_guest_frame_free(void);
  uint32_t csf_frm = br2_guest_frame(0x50);
  uint32_t lbuf_g = csf_frm;
  char *lbuf = (char *)guest_at(lbuf_g);
  int s2;
  int a0v;
  int c;
  int i;
  uint32_t r;

  if (*(uint32_t *)guest_at(0x801ccb74) != *(uint32_t *)guest_at(0x801cc85c)) {
    if (cd_dir_scan((uint32_t)guest_of(path),arg) == 0) {
      br2_guest_frame_free();
      return 0;
    }
    *(uint32_t *)guest_at(0x801ccb74) = *(uint32_t *)guest_at(0x801cc85c);
  }
  if (*name != 0x5c) {
    br2_guest_frame_free();
    return 0;
  }
  lbuf[0] = 0;
  a0v = 1;
  s2 = 0;
  s0 = name;
  for (;;) {
    s1 = (uint8_t *)lbuf;
    c = (uint8_t)*s0;
    if (c == 0x5c) goto tok_end;
    for (;;) {
      if (c == 0) goto str_end;
      s0++;
      *s1++ = (char)c;
      c = (uint8_t)*s0;
      if (c == 0x5c) break;
    }
tok_end:
    if (c == 0) goto str_end;
    s0++;
    *s1 = 0;
    a0v = cd_file_rec_find((uint32_t)a0v,lbuf_g);
    if (a0v == -1) {
      lbuf[0] = 0;
      goto have;                  /* slti s2,8 always true here */
    }
    s2++;
    if (s2 < 8) continue;
    goto many;                    /* path level overflow */
str_end:
    goto have;                    /* slti s2,8 always true here */
have:
    if (lbuf[0] == 0) {
      if (0 < (int)*(int32_t *)guest_at(0x801cc850)) {
        printf_forward((const char *)guest_at(0x801258f4),(const char *)name);
      }
      br2_guest_frame_free();
      return 0;
    }
    *s1 = 0;
    r = call_guest(0x80198190,(uint32_t)a0v,0);
    if (r == 0) {
      if (0 < (int)*(int32_t *)guest_at(0x801cc850)) {
        printf_forward((const char *)guest_at(0x8012590c));
      }
      br2_guest_frame_free();
      return 0;
    }
    s2 = 0;
    if (2 <= (int)*(int32_t *)guest_at(0x801cc850)) {
      printf_forward((const char *)guest_at(0x80125928),lbuf);
    }
    for (i = 0; i < 0x40; i++) {
      const uint8_t *flag = (const uint8_t *)guest_at(0x801fc708 + i * 0x18);
      const uint8_t *rec  = (const uint8_t *)guest_at(0x801fc700 + i * 0x18);
      if (*flag == 0) break;
      if (cd_strncmp12_eq(0x801fc708 + i * 0x18,lbuf_g) != 0) {
        if (2 <= (int)*(int32_t *)guest_at(0x801cc850)) {
          printf_forward((const char *)guest_at(0x80125948),lbuf);
        }
        memcpy(path,rec,0x18);
        br2_guest_frame_free();
        return (int)(uint32_t)(0x801fc700 + i * 0x18);
      }
    }
    if (0 < (int)*(int32_t *)guest_at(0x801cc850)) {
      printf_forward((const char *)guest_at(0x80125954),lbuf);
    }
    br2_guest_frame_free();
    return 0;
many:
    if (0 < (int)*(int32_t *)guest_at(0x801cc850)) {
      printf_forward((const char *)guest_at(0x801258d8),(const char *)name,s2);
    }
    br2_guest_frame_free();
    return 0;
  }
}

/* guest gte_vec_transform_emit @0x80186aa8 — copies 3 hwords from p2+0x34..3c into
 * p3+8..c, GTE transform via rtps_point_xform2, resets p2+4..0x14 to a
 * 0x1000-strided pattern then gte_rtps_xform3 emit. p1 unused. */
void gte_vec_transform_emit(int a0,int a1,int a2)
{
  int mtx;
  mtx = *(int *)guest_at((uint32_t)(uintptr_t)(a1 + 0x40)) + 0x20;
  *(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) = *(ushort *)guest_at((uint32_t)(uintptr_t)(a1 + 0x34));
  *(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0xa)) = *(ushort *)guest_at((uint32_t)(uintptr_t)(a1 + 0x38));
  *(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc)) = *(ushort *)guest_at((uint32_t)(uintptr_t)(a1 + 0x3c));
  rtps_point_xform2((undefined4 *)guest_at((uint32_t)(uintptr_t)mtx),
               (undefined4 *)guest_at((uint32_t)(uintptr_t)(a1 + 0x18)),
               (undefined4 *)guest_at((uint32_t)(uintptr_t)(a1 + 0x34)));
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a1 + 4)) = 0x1000;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a1 + 8)) = 0;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a1 + 0xc)) = 0x1000;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a1 + 0x10)) = 0;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a1 + 0x14)) = 0x1000;
  gte_rtps_xform3((undefined4 *)guest_at((uint32_t)(uintptr_t)mtx),
               (ushort *)guest_at((uint32_t)(uintptr_t)(a1 + 4)),
               (uint *)guest_at((uint32_t)(uintptr_t)(a1 + 0x20)));
  return;
}

/* guest node_list_collect @0x8013cde8 — walks linked list at *p2 (next ptr at
 * +0): stores each node into (*p1)+0x40; nodes with +0x17&&+0x18 call
 * task_slot_step, +0x17-only nodes jalr through +8 fn(+0x10 arg). */
void node_list_collect(int head,int out)
{
  int cur;
  cur = out;
  while (cur != 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)head) + 0x40)) = cur;
    if (*(char *)guest_at((uint32_t)(uintptr_t)(cur + 0x17)) != 0) {
      if (*(char *)guest_at((uint32_t)(uintptr_t)(cur + 0x18)) != 0) {
        task_slot_step();
      }
      else {
        call_guest(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(cur + 8)),
                     *(uint32_t *)guest_at((uint32_t)(uintptr_t)(cur + 0x10)),0);
      }
    }
    cur = *(int *)guest_at((uint32_t)(uintptr_t)cur);
  }
  return;
}

/* guest corner_emit_cond_offset @0x8018bda4 — conditional-offset corner emit: follows
 * p1+8 link when flag&8, packs 4 halfwords on stack, calls
 * gpu_load_image_cmd(stack, t0+0x14 + (p4+p5*lh10)*2). */
void corner_emit_cond_offset(int a0,int a1,int a2,int a3,int arg5,int arg6,int arg7)
{
  int t;
  short asStack_10 [4];
  t = a0;
  if ((*(int *)guest_at((uint32_t)(uintptr_t)(a0 + 4)) & 8) != 0) {
    t = a0 + *(int *)guest_at((uint32_t)(uintptr_t)(a0 + 8));
  }
  asStack_10[0] = (short)a1;
  asStack_10[1] = (short)a2;
  asStack_10[2] = (short)arg6;
  asStack_10[3] = (short)arg7;
  t = t + ((a3 + arg5 * (int)*(short *)guest_at((uint32_t)(uintptr_t)(t + 0x10))) << 1) + 0x14;
  /* guest rect = $sp+0x10 (frame 0x20, disasm 0x8018bda4) -> frm+0x10;
   * reserved frame staging, see spr_draw_off in gfx.c. */
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t cec_frm = br2_guest_frame(0x20);
    memcpy(guest_at(cec_frm + 0x10), asStack_10, 8);
    gpu_load_image_cmd(cec_frm + 0x10,t);
    br2_guest_frame_free();
  }
  return;
}

/* guest corner_emit_variant @0x8018be28 — corner emit variant: p6!=0 uses raw
 * p2/p3 else adds t0+0xc/0xe base; h10/h12 always offset by p4/p5. */
void corner_emit_variant(int a0,int a1,int a2,int a3,int arg5,int arg6)
{
  int t;
  short asStack_10 [4];
  t = a0;
  if ((*(int *)guest_at((uint32_t)(uintptr_t)(a0 + 4)) & 8) != 0) {
    t = a0 + *(int *)guest_at((uint32_t)(uintptr_t)(a0 + 8));
  }
  if (arg6 != 0) {
    asStack_10[0] = (short)a1;
    asStack_10[1] = (short)a2;
  }
  else {
    asStack_10[0] = (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(t + 0xc)) + a1);
    asStack_10[1] = (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(t + 0xe)) + a2);
  }
  asStack_10[2] = (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(t + 0x10)) + a3);
  asStack_10[3] = (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(t + 0x12)) + arg5);
  /* guest rect = $sp+0x10 (frame 0x20, disasm 0x8018be28) -> frm+0x10;
   * reserved frame staging, see spr_draw_off in gfx.c. */
  {
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t cev_frm = br2_guest_frame(0x20);
    memcpy(guest_at(cev_frm + 0x10), asStack_10, 8);
    gpu_load_image_cmd(cev_frm + 0x10,t + 0x14);
    br2_guest_frame_free();
  }
  return;
}

/* ======== batch31: debug putc + emit chains + vec lerp + record init ===== */
/* externs for batch31 callees */
extern int  console_emit(int a0);
extern void bios_tramp_b035();
extern int  rotate_xy_pairs(int a0,uint32_t a1);  /* guest 0x80195570 — battle.c */
extern int spr_pool_b_alloc();   /* weak-stubbed in bind.c; returns prim ptr */
extern void *spr_desc_slot_alloc(void);   /* gfx.c — returns host ptr */

/* guest debug_line_putc @0x8018ee28 — debug line-buffer putc: tab expands to
 * spaces to col&7==0, CR resets col, other chars bump col per 0x801b6701
 * class table; appends to 32B buf @0x801f68f0 flushed via bios_tramp_b035. */
void debug_line_putc(int ch)
{
  int c;
  c = (int)(char)ch;
  if (c == 9) {
    do {
      console_emit(0x20);
    } while ((*(int *)guest_at(0x801b6790) & 7) != 0);
  }
  else if (c == 0xa) {
    console_emit(0xd);
    *(int *)guest_at(0x801b6790) = 0;
    goto LAB_append;
  }
  else {
    if ((*(char *)guest_at((uint32_t)(uintptr_t)(0x801b6701 + (ch & 0xff))) & 0x97) != 0) {
      *(int *)guest_at(0x801b6790) = *(int *)guest_at(0x801b6790) + 1;
    }
LAB_append:
    if (*(int *)guest_at(0x801b6794) >= 0x20) {
      bios_tramp_b035(1,guest_at(0x801f68f0),*(int *)guest_at(0x801b6794));
      *(int *)guest_at(0x801b6794) = 0;
    }
    *(byte *)guest_at((uint32_t)(uintptr_t)(0x801f68f0 + *(int *)guest_at(0x801b6794))) = (byte)ch;
    *(int *)guest_at(0x801b6794) = *(int *)guest_at(0x801b6794) + 1;
  }
  if (0 < *(int *)guest_at(0x801b6794)) {
    bios_tramp_b035(1,guest_at(0x801f68f0),*(int *)guest_at(0x801b6794));
    *(int *)guest_at(0x801b6794) = 0;
  }
  return;
}

/* guest channel_script_emit_chain_gate @0x80134f2c — emit chain (2048/2078 bases) + gated
 * de6c/86f5c block on (h26+0x17)&1==0, then s5-anchored 86f5c + final emit. */
void channel_script_emit_chain_gate(int ctx,undefined4 a1,int a2,int a3,int arg5)
{
  int t;
  int dst;
  int obj;
  obj = *(int *)guest_at((uint32_t)(uintptr_t)(a3 + 0xbc));
  t = ctx + 0x2048;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),t,a1,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,t,a1,1);
  dst = ctx + 0x2078;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x190,dst,a1,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xc8,dst,a1,3);
  t = (int)*(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26)) + 0x17;
  *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26)) = (short)t;
  if ((t & 1) == 0) {
    t = (short)t;
    rotmat_rtps_build((uint)t,(uint *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc4)));
    gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)arg5),(undefined2 *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc4)));
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x168,ctx + 0x2074,a1,1);
  }
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a2 + 0x8c)) = 0;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a2 + 0x88)) = (uint)*(short *)guest_at((uint32_t)(uintptr_t)(obj + 0x4a));
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(a2 + 0x90)) = (uint)*(short *)guest_at((uint32_t)(uintptr_t)(obj + 0x4e));
  gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)arg5),(undefined2 *)guest_at((uint32_t)(uintptr_t)(a2 + 0x74)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x140,ctx + 0x2078,a1,1);
  return;
}

/* guest vec3_approach_step @0x80133944 — vec3 approach: diff = p3-p2, normalize
 * via FUN_80194630, snap to target when |d|<p4 else step p4/4096 along it;
 * out p1 receives the applied delta (or raw diff on snap). */
int vec3_approach_step(int out,int pos,int dst,int amt)
{
  int step;
  int len;
  undefined4 delta [3];
  undefined4 ndir [3];
  delta[0] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)dst) - *(undefined4 *)guest_at((uint32_t)(uintptr_t)pos);
  delta[1] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)(dst + 4)) - *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 4));
  delta[2] = *(undefined4 *)guest_at((uint32_t)(uintptr_t)(dst + 8)) - *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 8));
  step = amt;
  if (amt == 0) {
    step = 1;
  }
  len = FUN_80194630(delta,ndir);
  len = FUN_80194500(len);
  if (len < step) {
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)pos) = *(undefined4 *)guest_at((uint32_t)(uintptr_t)dst);
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 4)) = *(undefined4 *)guest_at((uint32_t)(uintptr_t)(dst + 4));
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 8)) = *(undefined4 *)guest_at((uint32_t)(uintptr_t)(dst + 8));
  }
  else {
    delta[0] = (undefined4)((int)ndir[0] * step >> 0xc);
    delta[1] = (undefined4)((int)ndir[1] * step >> 0xc);
    delta[2] = (undefined4)((int)ndir[2] * step >> 0xc);
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)pos) = *(undefined4 *)guest_at((uint32_t)(uintptr_t)pos) + delta[0];
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 4)) = *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 4)) + delta[1];
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 8)) = *(undefined4 *)guest_at((uint32_t)(uintptr_t)(pos + 8)) + delta[2];
  }
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)out) = delta[0];
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(out + 4)) = delta[1];
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(out + 8)) = delta[2];
  return out;
}

/* guest task_alloc_setup @0x8013f490 — allocs a task via task_node_alloc (cb
 * 0x8013e3f4), sets up node data: prim_group_build handle stored at *s0,
 * copies 8 words from 0x801b1840, writes h28/h2a=0x400, emits via
 * rotmat_rtps_build/gte_rtps_xform3. */
void task_alloc_setup(int pool)
{
  int node;
  int rec;
  int grp;
  node = (int)(intptr_t)task_node_alloc(1,2,8,0x8013e3f4,0,0x2c);
  if (node == 0) {
    return;
  }
  rec = *(int *)guest_at((uint32_t)(uintptr_t)(node + 0x10));
  grp = (int)(intptr_t)prim_group_build(rec,*(int *)guest_at(0x801c43e4),0,1);
  *(int *)guest_at((uint32_t)(uintptr_t)rec) = grp;
  if (grp == 0) {
    task_node_free((int *)guest_at((uint32_t)(uintptr_t)node));
    return;
  }
  if (tween_chan_alloc(grp,0) == 0) {
    task_node_free((int *)guest_at((uint32_t)(uintptr_t)node));
    node_tree_free(*(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)rec))));
    return;
  }
  obj_flag34_clear(*(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)rec))));
  tween_dur_spread(*(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)rec))),3);
  *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)rec) + 4)) = *(int *)guest_at((uint32_t)(uintptr_t)pool);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 8))  = *(undefined4 *)guest_at(0x801b1840);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 0xc)) = *(undefined4 *)guest_at(0x801b1844);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 0x10)) = *(undefined4 *)guest_at(0x801b1848);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 0x14)) = *(undefined4 *)guest_at(0x801b184c);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 0x18)) = *(undefined4 *)guest_at(0x801b1850);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 0x1c)) = *(undefined4 *)guest_at(0x801b1854);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 0x20)) = *(undefined4 *)guest_at(0x801b1858);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(rec + 0x24)) = *(undefined4 *)guest_at(0x801b185c);
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x2a)) = 0x400;
  rand_lcg();
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x28)) = 0x400;
  grp = *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)rec)));
  rotmat_rtps_build(0x400,(uint *)guest_at((uint32_t)(uintptr_t)grp));
  gte_rtps_xform3((undefined4 *)guest_at(0x801f0760),(ushort *)guest_at((uint32_t)(uintptr_t)grp),(uint *)guest_at((uint32_t)(uintptr_t)grp));
  return;
}

/* guest paired_task_cb @0x8013e3f4 — task cb (installed by task_alloc_setup):
 * skips when *p1==0 (calls task_slot_step), else bumps h28/h2a by
 * 0x138/0x1c4 once when flag 0x1f800082==0, runs obj_flag34_set +
 * angle_vec_build, then three gte_rtps_xform3 emits and writes averaged
 * midpoints into **p1 +0x14/0x18/0x1c. */
void paired_task_cb(int arg)
{
  int t;
  int nd;
  int mtx;
  int tgt;
  if (*(int *)guest_at((uint32_t)(uintptr_t)arg) == 0) {
    task_slot_step();
    return;
  }
  if (*(short *)guest_at(0x1f800082) == 0) {
    *(short *)guest_at((uint32_t)(uintptr_t)(arg + 0x28)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(arg + 0x28)) + 0x138);
    *(short *)guest_at((uint32_t)(uintptr_t)(arg + 0x2a)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(arg + 0x2a)) + 0x1c4);
  }
  obj_flag34_set(*(int *)guest_at((uint32_t)(uintptr_t)arg));
  t = (int)*(ushort *)guest_at((uint32_t)(uintptr_t)(arg + 0x2a));
  *(short *)guest_at((uint32_t)(uintptr_t)(arg + 8)) = (short)t;
  *(short *)guest_at((uint32_t)(uintptr_t)(arg + 0x10)) = (short)t;
  angle_vec_build((uint)(short)*(short *)guest_at((uint32_t)(uintptr_t)(arg + 0x28)),
               (uint *)guest_at(0x1f8003e0));
  gte_rtps_xform3((undefined4 *)guest_at((uint32_t)(uintptr_t)(arg + 8)),
               (ushort *)guest_at(0x1f8003e0),(uint *)guest_at(0x1f8003e0));
  t = *(int *)guest_at((uint32_t)(uintptr_t)arg);
  gte_rtps_xform3((undefined4 *)guest_at(0x801f0760),(ushort *)guest_at(0x1f8003e0),
               (uint *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)t))));
  nd = *(int *)guest_at((uint32_t)(uintptr_t)(arg + 4));
  mtx = *(int *)guest_at((uint32_t)(uintptr_t)(nd + 0x20));
  tgt = *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)arg)));
  *(int *)guest_at((uint32_t)(uintptr_t)(tgt + 0x14)) =
    (*(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x84)) +
     *(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x1c4)) +
     ((*(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x84)) +
       *(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x1c4))) >> 31)) >> 1;
  *(int *)guest_at((uint32_t)(uintptr_t)(tgt + 0x18)) =
    (*(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x88)) +
     *(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x1c8)) +
     ((*(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x88)) +
       *(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x1c8))) >> 31)) >> 1;
  *(int *)guest_at((uint32_t)(uintptr_t)(tgt + 0x1c)) =
    (*(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x8c)) +
     *(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x1cc)) +
     ((*(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x8c)) +
       *(int *)guest_at((uint32_t)(uintptr_t)(mtx + 0x1cc))) >> 31)) >> 1;
  return;
}

/* guest task_packet_emit @0x8013e0f0 — allocates a type-0xb task node
 * (callback 0x8014dbd8, args 0/0xc4) and fills the node's +0x10 packet
 * buffer: u16 angle fields from lut_cos/lut_sin(ang+0x400)>>6 at +0xb8/+0xbc,
 * a 3x3 u16 matrix copied column-wise from a1 (+0xa0..+0xb4) with the
 * two middle entries scaled by the trig results <<2, then two passes of
 * four 0x28-byte packet records (tag 0x09000000) whose byte fields depend
 * on the u8 flag at arg+2. Disasm 0x8013e0f0-0x8013e370. */
void task_packet_emit(int f,int m,int ang)
{
  int node;
  int hdg;
  uint32_t pkt;
  uint32_t uVar4;
  int iVar5;
  int iVar6;

  node = (int)(intptr_t)task_node_alloc(1,2,0xb,0x8014dbd8,0,0xc4);
  if (node == 0) {
    return;
  }
  pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(node + 0x10));
  *(short *)guest_at(pkt + 0xc2) = (short)ang;
  *(short *)guest_at(pkt + 0xc0) = (short)ang;
  hdg = (int)*(short *)guest_at((uint32_t)(uintptr_t)(f + 0x4e));
  *(short *)guest_at(pkt + 0xb8) =
    (short)(lut_cos_q12(hdg + 0x400) >> 6);
  *(short *)guest_at(pkt + 0xbc) =
    (short)(lut_sin_q12(hdg + 0x400) >> 6);
  /* 3x3 u16 matrix at pkt+0xa0: column j = u16 of m word j
   * (guest reads lhu at +0/+4/+8, writes sh at row/col strides 8/2). */
  *(ushort *)guest_at(pkt + 0xa0) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 0));
  *(ushort *)guest_at(pkt + 0xa8) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 0));
  *(ushort *)guest_at(pkt + 0xb0) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 0));
  *(ushort *)guest_at(pkt + 0xa2) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 4));
  *(ushort *)guest_at(pkt + 0xaa) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 4));
  *(ushort *)guest_at(pkt + 0xb2) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 4));
  *(ushort *)guest_at(pkt + 0xa4) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 8));
  *(ushort *)guest_at(pkt + 0xac) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 8));
  *(ushort *)guest_at(pkt + 0xb4) =
    *(ushort *)guest_at((uint32_t)(uintptr_t)(m + 8));
  *(ushort *)guest_at(pkt + 0xa8) =
    (ushort)(*(ushort *)guest_at(pkt + 0xa8) +
             (*(ushort *)guest_at(pkt + 0xb8) << 2));
  *(ushort *)guest_at(pkt + 0xac) =
    (ushort)(*(ushort *)guest_at(pkt + 0xac) +
             (*(ushort *)guest_at(pkt + 0xbc) << 2));
  /* two outer passes of two 0x28-byte records each (guest ang/t1 += 0x50) */
  for (iVar5 = 0; iVar5 < 2; iVar5++) {
    uVar4 = pkt + iVar5 * 0x50;
    for (iVar6 = 0; iVar6 < 2; iVar6++) {
      uint32_t uVar7 = uVar4 + iVar6 * 0x28;
      *(uint32_t *)guest_at(uVar7) = 0x09000000;
      *(byte *)guest_at(uVar7 + 7) = 44;
      *(short *)guest_at(uVar7 + 0x16) = 63;
    }
    {
      byte bVar9 = *(byte *)guest_at((uint32_t)(uintptr_t)(f + 2));
      if (bVar9 == 0) {
      for (iVar6 = 0; iVar6 < 2; iVar6++) {
        *(short *)guest_at(uVar4 + iVar6 * 0x28 + 0xe) = 0x7846;
      }
      /* variant B: rec0 bytes, rec1 head, shared rec1 tail */
      *(byte *)guest_at(uVar4 + 0xc)  = 144;
      *(byte *)guest_at(uVar4 + 0x1c) = 144;
      *(byte *)guest_at(uVar4 + 0xd)  = 0;
      *(byte *)guest_at(uVar4 + 0x14) = 160;
      *(byte *)guest_at(uVar4 + 0x15) = 0;
      *(byte *)guest_at(uVar4 + 0x1d) = 32;
      *(byte *)guest_at(uVar4 + 0x24) = 160;
      *(byte *)guest_at(uVar4 + 0x25) = 32;
      *(byte *)guest_at(uVar4 + 0x28 + 0xc)  = 160;
      *(byte *)guest_at(uVar4 + 0x28 + 0xd)  = 0;
      *(byte *)guest_at(uVar4 + 0x28 + 0x14) = 192;
      *(byte *)guest_at(uVar4 + 0x28 + 0x15) = 0;
      }
    else {
      for (iVar6 = 0; iVar6 < 2; iVar6++) {
        *(short *)guest_at(uVar4 + iVar6 * 0x28 + 0xe) = 0x7847;
      }
      /* variant C: same layout, colour bytes 64/96 */
      *(byte *)guest_at(uVar4 + 0xc)  = 144;
      *(byte *)guest_at(uVar4 + 0x1c) = 144;
      *(byte *)guest_at(uVar4 + 0xd)  = 64;
      *(byte *)guest_at(uVar4 + 0x14) = 160;
      *(byte *)guest_at(uVar4 + 0x15) = 64;
      *(byte *)guest_at(uVar4 + 0x1d) = 96;
      *(byte *)guest_at(uVar4 + 0x24) = 160;
      *(byte *)guest_at(uVar4 + 0x25) = 96;
      *(byte *)guest_at(uVar4 + 0x28 + 0xc)  = 160;
      *(byte *)guest_at(uVar4 + 0x28 + 0xd)  = 64;
      *(byte *)guest_at(uVar4 + 0x28 + 0x14) = 192;
      *(byte *)guest_at(uVar4 + 0x28 + 0x15) = 64;
    }
    /* shared tail on rec1 (uVar4+0x28): +0x1c=160, +0x1d=variant,
     * +0x24=192, +0x25=variant — emitted identically in both variants
     * with the variant byte in f (32 / 96). */
    *(byte *)guest_at(uVar4 + 0x28 + 0x1c) = 160;
    *(byte *)guest_at(uVar4 + 0x28 + 0x1d) = (bVar9 == 0) ? 32 : 96;
    *(byte *)guest_at(uVar4 + 0x28 + 0x24) = 192;
    *(byte *)guest_at(uVar4 + 0x28 + 0x25) = (bVar9 == 0) ? 32 : 96;
    /* both records: colour bytes +4..+6 = 127, tag byte +7 RMW */
    for (iVar6 = 0; iVar6 < 2; iVar6++) {
      uint32_t uVar7 = uVar4 + iVar6 * 0x28;
      byte bVar8;
      *(byte *)guest_at(uVar7 + 4) = 127;
      *(byte *)guest_at(uVar7 + 5) = 127;
      *(byte *)guest_at(uVar7 + 6) = 127;
      bVar8 = *(byte *)guest_at(uVar7 + 7);
      *(byte *)guest_at(uVar7 + 7) = (byte)((bVar8 & 0xfe) | 2);
    }
    }
  }
  return;
}

/* guest sprite_prim_update @0x8018694c — sprite/prim update: GTE transform via
 * rtps_point_xform2, copies 3 hwords from *(p3+0x18)+4 into p3+0..4 (h6=0),
 * calls part_matrix_compute, decrements/adjusts counters h0e/h10/h12 with
 * division-based deltas on h0a, then rotate_xy_pairs + gte_rtps_xform3. */
void sprite_prim_update(int f,int nd,int out,int idx)
{
  int t;
  int va;
  int mtx;
  mtx = *(int *)guest_at((uint32_t)(uintptr_t)(nd + 0x40));
  t = *(int *)guest_at((uint32_t)(uintptr_t)(out + 0x18));
  rtps_point_xform2((undefined4 *)guest_at((uint32_t)(uintptr_t)(mtx + 0x20)),
               (undefined4 *)guest_at((uint32_t)(uintptr_t)(nd + 0x18)),
               (undefined4 *)guest_at((uint32_t)(uintptr_t)(nd + 0x34)));
  *(short *)guest_at((uint32_t)(uintptr_t)(out + 6)) = 0;
  *(ushort *)guest_at((uint32_t)(uintptr_t)out) = *(ushort *)guest_at((uint32_t)(uintptr_t)(t + 4));
  *(ushort *)guest_at((uint32_t)(uintptr_t)(out + 2)) = *(ushort *)guest_at((uint32_t)(uintptr_t)(t + 6));
  *(ushort *)guest_at((uint32_t)(uintptr_t)(out + 4)) = *(ushort *)guest_at((uint32_t)(uintptr_t)(t + 8));
  t = (int)((*(ushort *)guest_at((uint32_t)(uintptr_t)(out + 6)) << 0x10) >> 0x11);
  part_matrix_compute((undefined4 *)guest_at((uint32_t)(uintptr_t)(nd + 4)),
               (byte *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(f + 0x48)) + idx * 0x20)),
               (short *)guest_at((uint32_t)(uintptr_t)out),t);
  if (0 < (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xe))) {
    t = ((int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x14)) -
             (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa))) /
            (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xe));
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0xe)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(out + 0xe)) - 1);
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) + t);
  }
  else if (0 < (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x10))) {
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0x10)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(out + 0x10)) - 1);
  }
  else if (0 < (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x12))) {
    t = (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) /
            (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x12));
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0x12)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(out + 0x12)) - 1);
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) =
      (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) - t);
  }
  t = (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa));
  va = nd + 4;
  rotate_xy_pairs(t,va);
  gte_rtps_xform3((undefined4 *)guest_at((uint32_t)(uintptr_t)(mtx + 0x20)),
               (ushort *)guest_at((uint32_t)(uintptr_t)va),
               (uint *)guest_at((uint32_t)(uintptr_t)(nd + 0x20)));
  return;
}

/* guest rotate_xy_pairs @0x80195570 — rotate the three (x,y) s16 pairs at
 * nd+6..0x10 by angle f (4096 units/turn; negative angle negates
 * sin only). Cos/sin come from the 4096-entry u32 table @0x801b87a0
 * (hi16 = cos*4096 sra, lo16 = sin*4096 s16), index = |angle| & 0xfff.
 * Per pair: nx = (cos*x - sin*y) >> 12, ny = (sin*x + cos*y) >> 12,
 * multu/mflo low-32 products, subu/addu wrap, sra 12. Returns nd
 * (v0 = nd). */
int rotate_xy_pairs(int n,uint32_t ang)
{
  int32_t cs,sn;
  uint32_t e,m,idx;
  int16_t *v;
  int32_t x0,x1,x2,y0,y1,y2;
  m = (n < 0) ? (uint32_t)(-(int64_t)n) : (uint32_t)n;
  idx = m & 0xfff;
  e = *(uint32_t *)guest_at(0x801b87a0 + idx * 4);
  cs = (int32_t)e >> 16;
  sn = (n < 0) ? -(int32_t)(int16_t)e : (int32_t)(int16_t)e;
  v = (int16_t *)guest_at(ang);
  x0 = v[3]; x1 = v[4]; x2 = v[5];
  y0 = v[6]; y1 = v[7]; y2 = v[8];
  v[3] = (int16_t)(((int32_t)((uint32_t)cs * (uint32_t)x0 -
                              (uint32_t)sn * (uint32_t)y0)) >> 12);
  v[4] = (int16_t)(((int32_t)((uint32_t)cs * (uint32_t)x1 -
                              (uint32_t)sn * (uint32_t)y1)) >> 12);
  v[5] = (int16_t)(((int32_t)((uint32_t)cs * (uint32_t)x2 -
                              (uint32_t)sn * (uint32_t)y2)) >> 12);
  v[6] = (int16_t)(((int32_t)((uint32_t)sn * (uint32_t)x0 +
                              (uint32_t)cs * (uint32_t)y0)) >> 12);
  v[7] = (int16_t)(((int32_t)((uint32_t)sn * (uint32_t)x1 +
                              (uint32_t)cs * (uint32_t)y1)) >> 12);
  v[8] = (int16_t)(((int32_t)((uint32_t)sn * (uint32_t)x2 +
                              (uint32_t)cs * (uint32_t)y2)) >> 12);
  return (int)ang;
}

/* guest channel_script_emit_chain_triple @0x80134234 — emit chain: three de6c+86f5c blocks on
 * p3+0xc4/0x114/0x164 (h26/h28/h2a counters +5/+2/+1) interleaved with
 * seven prim_task_run emits against p1+0x2070/0x2074/0x2078. */
void channel_script_emit_chain_triple(int ctx,int a1,int a2,int a3)
{
  int t;
  int dst;
  t = ctx + 0x2070;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xc8,t,a1,1);
  dst = ctx + 0x2074;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xa0,dst,a1,1);
  t = ctx + 0x2078;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x370,t,a1,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)a2),t,a1,1);
  t = (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26)) + 5);
  *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x26)) = (short)t;
  rotmat_rtps_build((uint)t,(uint *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc4)));
  gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)a3),(undefined2 *)guest_at((uint32_t)(uintptr_t)(a2 + 0xc4)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0xf0,ctx + 0x2070,a1,2);
  t = (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0x28)) + 2);
  *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x28)) = (short)t;
  rotmat_rtps_build((uint)t,(uint *)guest_at((uint32_t)(uintptr_t)(a2 + 0x114)));
  gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)a3),(undefined2 *)guest_at((uint32_t)(uintptr_t)(a2 + 0x114)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x140,ctx + 0x2070,a1,8);
  t = (short)(*(ushort *)guest_at((uint32_t)(uintptr_t)(a2 + 0x2a)) + 1);
  *(short *)guest_at((uint32_t)(uintptr_t)(a2 + 0x2a)) = (short)t;
  rotmat_rtps_build((uint)t,(uint *)guest_at((uint32_t)(uintptr_t)(a2 + 0x164)));
  gte_xform3_t_ip((undefined4 *)guest_at((uint32_t)a3),(undefined2 *)guest_at((uint32_t)(uintptr_t)(a2 + 0x164)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(a2 + 8)) + 0x280,dst,a1,6);
  return;
}

/* guest sprite_prim_build @0x8018c578 — builds a sprite prim via spr_pool_b_alloc:
 * uv/size deltas t1/t3 from p3 bytes +1, scaled by sext16(p5)/sext16(p6)
 * >>12, writes texpage/colour/clut fields and halfword corners, ORs
 * 0x09000000 into *p1 and stores low24(prim) back to *p1. */
void sprite_prim_build(int ot,int pos,int dim,int uv,int sw,int sh,int col)
{
  int spr;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  spr = spr_pool_b_alloc(ot,pos,dim,uv);
  if (spr != 0) {
    iVar2 = ((dim >> 0x10) & 0xff) + 1;
    iVar3 = (int)((uint32_t)dim >> 0x18) + 1;
    iVar4 = uv & 0xff;
    iVar5 = (uv >> 8) & 0xff;
    iVar6 = iVar2 * (int)(short)sw;
    iVar7 = iVar3 * (int)(short)sh;
    *(byte *)guest_at((uint32_t)(uintptr_t)(spr + 0x15)) = (byte)iVar5;
    if (iVar6 < 0) {
      iVar6 = iVar6 + 0xfff;
    }
    if (iVar7 < 0) {
      iVar7 = iVar7 + 0xfff;
    }
    iVar2 = iVar6 >> 0xc;
    iVar3 = iVar7 >> 0xc;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)(spr + 0xc)) = uv;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x16)) = (short)dim;
    *(byte *)guest_at((uint32_t)(uintptr_t)(spr + 0x14)) = (byte)(iVar4 + iVar2);
    *(byte *)guest_at((uint32_t)(uintptr_t)(spr + 0x1c)) = (byte)iVar4;
    *(byte *)guest_at((uint32_t)(uintptr_t)(spr + 0x1d)) = (byte)(iVar5 + iVar3);
    *(byte *)guest_at((uint32_t)(uintptr_t)(spr + 0x24)) = (byte)(iVar4 + iVar2);
    *(byte *)guest_at((uint32_t)(uintptr_t)(spr + 0x25)) = (byte)(iVar5 + iVar3);
    /* corner halves: negated values use sign of the negated operand,
     * positive use sign of the adjusted raw product (iVar6/iVar7) */
    iVar4 = pos + (((-iVar2) + (int)((uint32_t)(-iVar2) >> 0x1f)) >> 1);
    iVar5 = (pos >> 0x10) + (((-iVar3) + (int)((uint32_t)(-iVar3) >> 0x1f)) >> 1);
    iVar6 = pos + ((iVar2 + (int)((uint32_t)iVar6 >> 0x1f)) >> 1);
    iVar7 = (pos >> 0x10) + ((iVar3 + (int)((uint32_t)iVar7 >> 0x1f)) >> 1);
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 8)) = (short)iVar4;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x18)) = (short)iVar4;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x1a)) = (short)iVar7;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x22)) = (short)iVar7;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0xa)) = (short)iVar5;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x12)) = (short)iVar5;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x10)) = (short)iVar6;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x20)) = (short)iVar6;
    iVar4 = ((col & 0xff) * 0x010101) | 0x2e000000;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)(spr + 4)) = iVar4;
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)ot) =
      *(undefined4 *)guest_at((uint32_t)(uintptr_t)ot) | 0x09000000;
    *(int *)guest_at((uint32_t)(uintptr_t)ot) = spr & 0xffffff;
  }
  return;
}

/* guest debug_line_flush @0x8018edf4 — flush debug line buffer via
 * bios_tramp_b035 when count (dim) > 0, then reset count @0x801b6794. */
void debug_line_flush(int u0,int u1,int n)
{
  if (0 < n) {
    bios_tramp_b035(1,guest_at(0x801f68f0),n);
    *(int *)guest_at(0x801b6794) = 0;
  }
  return;
}

/* guest sprite_obj_alloc_emit @0x8018ca8c — alloc sprite obj via spr_desc_slot_alloc,
 * fills uv/size halfwords + flags, emits prim_packet_init_tex packet at +0x14,
 * links low24 addrs through *p1; returns 0 or -1. */
int sprite_obj_alloc_emit(int a0,int a1,int a2,int a3,int arg5)
{
  int spr;
  spr = (int)guest_of(spr_desc_slot_alloc());
  if (spr == 0) {
    return -1;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x10)) =
    (short)(((a2 >> 0x10) & 0xff) + 1);
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x12)) =
    (short)((int)((uint32_t)a2 >> 0x18) + 1);
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0xa)) = (short)(a1 >> 0x10);
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 8)) = (short)a1;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(spr + 0xc)) = a3;
  *(byte *)guest_at((uint32_t)(uintptr_t)(spr + 7)) =
    (byte)((*(byte *)guest_at((uint32_t)(uintptr_t)(spr + 7)) & 0xfd) | 1);
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(spr + 4)) = 0x64808080;
  prim_packet_init_tex(spr + 0x14,*(byte *)guest_at(0x801e4117),*(byte *)guest_at(0x801e4116),
               (uint)(short)a2,(byte *)guest_at((uint32_t)(uintptr_t)arg5));
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)spr) =
    *(undefined4 *)guest_at((uint32_t)(uintptr_t)spr) | 0x04000000;
  *(int *)guest_at((uint32_t)(uintptr_t)a0) = spr & 0xffffff;
  *(undefined4 *)guest_at((uint32_t)(uintptr_t)(spr + 0x14)) =
    (spr & 0xffffff) | 0x02000000;
  *(int *)guest_at((uint32_t)(uintptr_t)a0) = (spr + 0x14) & 0xffffff;
  return 0;
}
