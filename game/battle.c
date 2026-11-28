/* battle.c — battle_frame_tick battle-frame tick.
 * Faithful verbatim port: guest identifiers/labels preserved.
 * param_1 = battle object (guest addr as int), param_2 = phase mask.
 * Fighter ctrls are the absolute globals 0x1f8000e8 (P1) /
 * 0x1f8001f0 (P2); &DAT_x yields the guest base pointer. */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "cpu_state.h"


extern void    *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint32_t br2_gaddr(const void *p);
extern uint32_t br2_call_generated(uint32_t fn,uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3);
extern uint32_t br2_call_clean(uint32_t fn,uint32_t a0,uint32_t a1,uint32_t a2,uint32_t a3);
extern uint32_t call_guest(uint32_t fn, uint32_t a, uint32_t b);
extern uint32_t call_guest4(uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
extern uint32_t br2_guest_frame(uint32_t);
extern void    br2_guest_frame_free(void);
extern void    br2_st32(uint32_t,uint32_t);
extern uint32_t br2_ld8(uint32_t);
extern uint32_t br2_ld16(uint32_t);
extern uint32_t br2_ld32(uint32_t);
extern int     clean_try_dispatch(CPUState *cpu,uint32_t addr);
extern CPUState *clean_cpu_state(void);
extern uint32_t guest_sp(void);
extern uint32_t guest_gp(void);
extern void    *fn_lookup(uint32_t addr);
#define CONCAT22(hi,lo) (((uint32_t)(uint16_t)(hi) << 16) | (uint16_t)(lo))
#define CONCAT11(hi,lo) (((uint16_t)(uint8_t)(hi) << 8) | (uint8_t)(lo))

/* guest absolute-address globals (host binds storage) */
extern uint8_t  *g_DAT_1f8000e8, *g_DAT_1f8001f0;
extern uint8_t  *g_DAT_1f8000ea, *g_DAT_1f8001f2, *g_DAT_1f8000ee, *g_DAT_1f8001f6;
extern uint8_t  *g_DAT_801f4d72;
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
extern uint8_t  *g_DAT_801b144c, *g_DAT_801b140c, *g_DAT_1f800098;
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
#define _DAT_801f022c  (*(int16_t *)g__DAT_801f022c) /* guest lh/sh @0x801f022c (0x8012ee48,0x8012ecbc) */

/* host hooks (guest-faithful names; runtime binds implementations) */
extern void fighter_frame(uint8_t*ctrl,uint8_t*motion);
#define FUN_801787dc fighter_frame
extern void sprite_strip_draw_dual(int src_a,int src_b,int uv,int n,uint16_t *pal,int x0, int y0,int tag);
extern int task_alloc_wrap_a(uint32_t w0,uint32_t w1,int w2,uint32_t w3);
extern int task_alloc_wrap_b(uint32_t w0,uint32_t w1,uint32_t w2,uint32_t w3, int w4);
extern void hud_cell_frame_set(int val,int cell);
extern void hud_digits_color(int val,int cell);
extern bool fighter_height_check(int f);
extern uint32_t sprite_rect_set_a(int rec,short x,int y,uint32_t w,int h);
extern uint32_t sprite_rect_set_b(int rec,short x,int y,uint32_t w,int h);
extern uint32_t sprite_rect_set_c(int rec,short x,int y,uint32_t w,int h);
extern int * rtps_point_xform(uint32_t *rt,uint32_t *in,int *out,int flag);
extern int * rtps_point_xform2(uint32_t *rt,uint32_t *in,int *out);
extern uint32_t * rotmat_rtps_build(uint32_t ang,uint32_t *m);
extern uint32_t * rotmat2_pair_build(uint32_t ang0,uint32_t ang1,uint32_t *out);
extern int sprintf_core(char *dst,char *fmt,...);
extern uint32_t * rtps_basis_xform(uint32_t *rt,uint16_t *bas,uint32_t *out);
extern uint32_t * rotmat2_build(uint32_t ang,uint32_t *m);
extern int * task_node_alloc(int kind,uint32_t cnt,uint32_t sz,int cb,int cb_arg,int dsize);
extern void task_node_free(int *node);
extern void task_pools_tick(int pool);
extern void noop_a(int node,uint32_t w0,uint32_t w1,uint32_t w2);
extern int fx_group_alloc(uint32_t pool,uint32_t kind,int ix,int iy);
extern void noop_b(int node,uint32_t w0,uint32_t w1);
extern uint32_t noop_c(int node);
extern void node_tree_free(int node);
extern void prim_attr_word_set(int prim,uint16_t attr);
extern void gpu_pkt_tag_0c3c(uint32_t a0g);
extern uint32_t * prim_group_build(uint32_t prim,int attr,uint32_t a2,uint32_t a3);
extern uint32_t tween_chan_alloc(int grp,int idx);
extern void tween_dur_spread(int grp,int n);
extern int str_len(char *s);
extern char * strchr_bounded(char *s,char c,int n);
extern uint8_t * mem_move(uint8_t *s,uint8_t *src,int n);
extern uint32_t *g__DAT_801f0654, *g__DAT_801f0656;
extern uint8_t  *g__DAT_1f800082;
/* guest lh/lhu @0x1f800082 — a u16 sim/pause gate, NOT a u32. Reading
 * 4 bytes at the unaligned address picks up the low half of the pointer
 * stored at 0x1f800084 (0x801BEF90 -> 0xEF900000) and the gate reads
 * nonzero forever, freezing every channel-script task (LAB_80137acc et
 * al.) so the ROUND-FIGHT phase never advances past 0x300. menu.c:432
 * and save.c:236 already bind it as u16. */
#define _DAT_1f800082   (*(uint16_t *)g__DAT_1f800082)
extern void script_stream_tick(int *stream);
extern uint32_t always_one(int unused);
extern void triple_track_spawn(int idx,uint16_t val,int tid);
extern void xform_accum_spawn(int mode,int lvl);
extern void impact_fx_spawn(uint32_t*pos,uint32_t fid,int ang,int aux,uint32_t fl);
extern int fx_record_alloc(uint32_t pos_g,int kind,uint32_t fl,uint32_t aux,uint32_t pal_g);
extern void emit_task_spawn(int*pos,int kind,uint16_t x,short y,uint16_t p5,uint16_t p6);
extern void emit_task_spawn_b(uint32_t*pos,uint32_t ea,uint32_t eb,uint32_t fl);
extern void quad_fx_spawn(int f,int kind,uint32_t*pos,uint16_t aux,uint16_t fl,int p6,int p7);
extern void group_fx_spawn(uint32_t arg,uint32_t w0,uint32_t w1,uint32_t w2,uint32_t w3,uint32_t w4,uint32_t w5);
extern uint32_t *vec3_step_toward(uint32_t*out,short*pos,short*dst,short stp);
extern uint32_t *vec3_step_toward_spd(uint32_t*out,short*pos,short*dst,int spd);
extern uint32_t *vec3_step_toward_q(uint32_t*out,short*pos,short*dst,short stp);
extern uint32_t *vec3_step_toward_q_spd(uint32_t*out,short*pos,short*dst,int spd);
extern uint8_t table_byte_next_a(void);
extern uint8_t table_byte_next_b(void);
extern void pad_edge_scan(int ctrl_g);
extern void cmd_input_parse(int ctrl_g);
extern void action_from_input(int ctrl_g);
extern uint32_t seg_sphere_hit(int rad0,int rad1,int*seg0,int*seg1,int*cen,short*hit);
extern uint32_t sphere_pt_hit(int r0,int r1,int*p,int*q,short*mid);
extern void debug_strike_fx_a(short*pos,int x,int y,uint32_t unused,int f);
extern void debug_strike_fx_b(short*pos,uint32_t x,uint32_t y,uint16_t ang,int f);
extern void noop_fn();
extern uint16_t *rtps_pt_xform(uint16_t*pt,uint16_t*out);
extern int32_t FUN_80194660(const void *in,void *out);
extern void node_xform_apply(int node,uint32_t mat,uint32_t flg);
extern uint16_t attr_flag_mask(uint32_t node);
extern uint16_t pad_hist_record(int ctrl_g,int pad,uint16_t flags);
extern void fighter_script_fsm(int ctrl_g,uint32_t pad,int*script,int ctx);
extern bool pt_box_radius_test(int r0,int r1,int*p,int*q);
extern void fighter_state_tick(uint32_t*ctrl,uint32_t pad,int ctx);
extern uint32_t fighter_atk_dir(int atk,int def);
extern void hit_pose_swap(int f0,int f1);
extern uint32_t hit_elig_check(int f0,int f1);
extern uint32_t hitbox_pair_test(int f0,int f1,uint16_t*out);
extern uint32_t fighter_range_code(int f0,int f1);
extern int hit_scale_calc(int f0,int f1);
extern uint32_t hit_react_apply(int atk,int def,int scale,short*hits,int n);
extern uint32_t hitbox_wall_collide(int f0,uint16_t*hits,int battle);
extern void camera_aim_offset(int f0,int f1,uint32_t ang);
extern void node_translate_tree(int node,int dx,int dy,int dz);
extern void fighter_attach_run(int f);
extern void fighter_height_track(int f,int st);
extern void hitbox_corners_xform(int f0,int f1);
extern void fighter_contact_resolve(int f0,int f1,int battle);
extern void fighter_pair_update(int f0,int f1,int battle);
extern void battle_camera_step(uint32_t cam,uint8_t*f0,uint8_t*f1,int battle);
extern void battle_hud_draw(int unused,int fighter,int aux0,int aux1);
extern void fighter_flash_update(uint32_t battle,uint32_t slot,int ctx2,uint8_t*rec);
extern void camera_state_cache(uint32_t unused,int battle);
extern void player_light_apply(uint8_t*ctrl,int side);
extern int  atan2_lut(int x,int y);

extern void hud_text_blk_alt(uint32_t ot,uint16_t font,const void *str,uint16_t a3, uint16_t arg5,short tm);
extern void announce_param_block_init_b(uint32_t ot,uint16_t font,const void *str,uint16_t a3, uint16_t arg5,short tm);
extern void slot_triple_set(int slot,int c0,int c1,int c2);
extern void rec_flag1_set(uint16_t *src);
extern void target_angles_set(short *dst,int idx);
extern void target_depth_set(short *dst,int idx);
extern void pair_depth_set(short *pa,short *pb,int idx);
extern void target_angles_depth_set(short *dst,int idx);
extern void midpoint_angles_set(int *pa,int *pb,int idx);
extern void dual_target_track(int tgt,int f_a,int f_b);
extern void dual_target_track_b(int cam,int fa,int fb);
extern void camera_track_shot(int cam,int f_a,int f_b);
extern void camera_track_step(int cam,int fa,int fb);
extern int u16_step_toward(uint16_t *cur,uint16_t *tgt,short step);
extern int fixed_approach_s16(uint16_t *cur,short *tgt,uint32_t step);
extern void fighter_trail_emit(uint32_t *mat,uint32_t a1,int ent,int spec);
extern bool burst_slot_active(int slot);
extern void prim_packet_init_tex(int pkt,int flag1,int flag2,uint32_t fl,uint8_t *tex);
extern int32_t FUN_80194630(const void *in,void *out);
extern void gte_xform_accum(short *acc,uint32_t *vec,int n,uint32_t tr);
extern uint32_t * gte_rtps_xform3(uint32_t *rt,uint16_t *in,uint32_t *out);
extern uint32_t * gte_xform3_nold(uint16_t *in,uint32_t *out);
extern int * gte_mvmva_add(uint32_t *rt,uint32_t *vec,int *out);
extern uint16_t * gte_rtps_xform1(uint32_t *rt,uint32_t *in,uint16_t *out);
extern void gte_rot_load(uint32_t *m);
extern void gte_light_load(uint32_t *light);
extern void gte_tr_load(int trec);
extern void gte_cw_d000_set(uint32_t cw);
extern uint32_t * gte_op_a00428(uint32_t *in,uint32_t *out);
extern void orbit_fx_spawn(int var);
extern void task_heads_clear(int pool);
extern void task_slot_step(void);
extern void task_slot_fns_run(int slot);
extern void task_pool_init(int pool,int kind,int count,int base);
extern int task_pool_alloc(int pool,int kind,int size);
extern void task_pool_free(int pool);
extern void gp_slot48_set(uint32_t v);
extern void gp_slot24_set(uint32_t v);
extern int prim_batch_emit(int desc,int idx,uint32_t w0,uint32_t w1,int *otc, uint32_t arg6,uint32_t arg7,int num,int sel,uint32_t fl, uint32_t kind);
extern void obj_flag34_clear(int obj);
extern int tween_seq_alloc(int mode,uint16_t a1,uint32_t a2,uint32_t a3,short arg5, uint16_t arg6,uint32_t arg7,uint32_t kind);
extern void gte_matrix_base_set(uint32_t mat,uint32_t aux);
extern int lazy_node_wrap(uint32_t proto,uint32_t arg,uint16_t aux);
extern int prim_group_alloc(uint32_t a0,int attr,int num,int mode,int flg,int div, uint32_t kind);
extern void node_param_quad_set(int node,uint32_t q0,uint32_t q1,uint32_t q2, uint32_t q3);
extern int * prim_list_gte_invoke(int *lst,uint32_t a1,uint32_t a2,int total,uint32_t mtx_t,uint32_t mtx_ip);
extern uint32_t text_render_full(uint32_t *ot,uint32_t font,int cfg);
extern uint32_t text_render_simple(uint32_t *a0,uint32_t font,int cfg);
extern void particle_burst_spawn(int type,int idx);
extern void particle_burst_update(int ctx,uint32_t arg,uint32_t pos,int rec);
extern void radial_glow_draw(int *rec);
extern void iris_wipe_spawn(int req,int arg);
extern uint32_t * gte_xform3_ip(uint32_t *rt,uint16_t *in);
extern void deferred_cmd_post(uint32_t cmd,int arg,short aux);
extern int range_circle_test(int x,int y,uint32_t *box,int radius);
extern void vec_emit_spawn(uint32_t *rec);
extern void iris_wipe_update(int *rec);
extern uint32_t script_op_dispatch(uint32_t ctrl,uint32_t pad,int script,uint32_t ctx);
extern int trig_sin_lookup(uint32_t ang);
extern uint32_t * angle_vec_build(uint32_t ang,uint32_t *out);
extern uint32_t rand_lcg(void);
extern int isqrt(int n);
extern uint32_t * gte_rtps_xform3_t(uint32_t *rm,uint16_t *ang,uint32_t *out);
extern uint32_t * gte_xform1_ir(uint32_t *mtx,uint32_t *out);
extern uint32_t * gte_rtps_xform1_raw(uint32_t *mtx,uint32_t *in,uint32_t *out);
extern int * node_lazy_attach(int n,int en,int aux,int par);
extern uint32_t tween_ensure(int chan);
extern void obj_flag34_set(int obj);
extern void tween_seek(int seq,int pos);
extern void tween_seq_step(short *seq);
extern void prim_group_gc(int grp);
extern void rect_prim_spawn(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1, uint8_t b0,uint8_t b1,uint16_t fl,uint16_t aux, uint32_t kind);
extern uint32_t isqrt_refine(int n);
extern uint32_t cop2_clz(uint32_t v);
extern void gte_points_xform(uint32_t *ip,uint32_t *sz,uint16_t *sx,uint16_t *sy, uint16_t *cc,int n);
extern int tween_track_alloc(int mode,uint16_t a1,uint32_t a2,uint16_t *kf,short arg5,uint16_t arg6,uint16_t arg7,uint32_t kind);
extern void tween_apply(short *seq,int v0,int v1);
void keyframe_cursor_search(uint32_t *a0);
extern void rect_prim_tick(uint32_t *prim);
extern void prim_tag_set(int prim);
extern void prim_packet_init(int pkt,int flag1,int flag2,uint32_t fl);
extern void verts_offset_apply(uint32_t *v,uint32_t *c,int n,short *off,int cnt, uint32_t fl);
extern void verts_rgb_apply(uint32_t *v,uint32_t *c,int n,short *off,int cnt, uint32_t fl);
extern void verts_add_apply(int n,uint32_t *v,uint16_t *c,int d);
extern void verts_sub_apply(int n,uint32_t *v,uint16_t *c,int d);
extern int getCopControlWord(int fmt,int reg);
extern int fixed_point_divide(int num,int den);
extern uint32_t * gte_xform3_t_ip(uint32_t *rt,uint16_t *in);
extern int prim_task_run(int rec,uint32_t a1,uint32_t a2,int mode);
extern uint32_t br2_guest_frame(uint32_t bytes);
extern void br2_guest_frame_free(void);
extern uint16_t * vec2_rot(uint32_t ang0,uint32_t ang1,uint16_t *dst);
extern uint32_t * angle_gte_vec(uint32_t x,uint32_t y,uint32_t *out);
extern int * prim_list_persp_step(int *lst,uint32_t a1,uint32_t a2,int n,uint32_t arg5,int xf,uint32_t arg7,uint32_t arg8);
extern void gte_light_set(uint32_t x,uint32_t y,uint32_t z);
extern int * gte_xform_add_off(int x,uint32_t *y,int *z);
extern uint32_t * angle_pair_rotmat(uint32_t ang0,uint32_t ang1,uint32_t *out);
extern int32_t sprite_slot_commit(uint32_t w0,uint32_t w1,uint32_t w2,
                                  uint32_t w3,uint32_t aux5,uint32_t aux6);
extern void slot_height_proj(uint32_t a0,int obj,int idx,int xf);
extern void battle_intro_fsm(uint32_t mat,uint32_t a1,uint32_t a2,int st);
extern int task_alloc_wrap_c(uint32_t w0,uint32_t w1,int w2,uint32_t w3);
extern int task_alloc_wrap_d(uint32_t w0,uint32_t w1,uint32_t w2,uint32_t w3,int w4);
extern int task_alloc_wrap_e(int w0,int w1,int w2);
extern int task_alloc_wrap_f(uint32_t w0,uint32_t w1,uint32_t w2,int seq);
extern void slot_triple_tick(int *slot);
extern void xform_accum_tick(uint32_t *rec);
extern void impact_fx_update(int rec);
extern void impact_fx_emit_tick(int rec);
extern void fx_record_init(uint32_t *rec);
extern void quad_fx_update(int rec);
extern int angle_from_xy(int x,int y);
extern void table_1f0ae0_clear(int idx);
extern void sprite_place_centered(uint32_t rec,uint32_t st,short x,short y,int w, int h);
extern void fighter_tint_fade(int scene,int idx,int battle,int anim);

/* battle_frame_tick @0x80166544 — per-frame battle tick: fighter contact
 * resolve + pair update (gated on battle+0x82), phase-gated HUD draw,
 * camera step + state cache, then the remaining per-frame systems. */
void battle_frame_tick(int battle,uint32_t phase)

{
  char light;
  int hud_ot;
  uint32_t prim_list;
  uint32_t aux;
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
  battle_camera_step(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),&DAT_1f8000e8,&DAT_1f8001f0,battle);
  camera_state_cache(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),battle);
  if (((phase & 1) != 0) && ((_DAT_1f800148 & 2) == 0)) {
    fighter_flash_update(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle,&DAT_1f8000e8);
    if (((_DAT_1f800154 & 0x400) == 0) ||
       (*(short *)(&DAT_801f4d72 + (uint32_t)DAT_1f8000ea * 0xbc) != 0)) {
      if (((_DAT_1f800154 & 0x8000000) == 0) || ((_DAT_1f8000dc & 1) == 0)) {
        player_light_apply(&DAT_1f8000e8,1);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = _DAT_1f800100;
        light = DAT_1f8000ee;
      }
      else {
        player_light_apply(&DAT_1f8000e8,1);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800110 + 0x34)));
        light = *(char *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800110 + 0x24)));
      }
      prim_list_gte_invoke((int *)guest_at((uint32_t)prim_list),hud_ot + 0x40,aux,(int)light,ctx,0x1f80033c);
    }
    if ((((int)phase >> 6 & 1U) != 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))== '\0')) {
      prim_list_persp_step((int *)guest_at((uint32_t)_DAT_1f800104),*(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x2040,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),0xf,
                   ctx,0x1f80035c,*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 8 + -0x7fe4e94c,
                   *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x206c);
    }
  }
  if ((((int)phase >> 1 & 1U) != 0) && ((_DAT_1f800250 & 2) == 0)) {
    fighter_flash_update(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle,&DAT_1f8001f0);
    if (((_DAT_1f80025c & 0x400) == 0) ||
       (*(short *)(&DAT_801f4d72 + (uint32_t)DAT_1f8001f2 * 0xbc) != 0)) {
      if (((_DAT_1f80025c & 0x8000000) == 0) || ((_DAT_1f8000dc & 1) == 0)) {
        player_light_apply(&DAT_1f8001f0,2);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = _DAT_1f800208;
        light = DAT_1f8001f6;
      }
      else {
        player_light_apply(&DAT_1f8001f0,1);
        hud_ot = *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8)));
        aux = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc)));
        prim_list = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800218 + 0x34)));
        light = *(char *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800218 + 0x24)));
      }
      prim_list_gte_invoke((int *)guest_at((uint32_t)prim_list),hud_ot + 0x40,aux,(int)light,ctx,0x1f80033c);
    }
    if ((((int)phase >> 6 & 1U) != 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))== '\0')) {
      prim_list_persp_step((int *)guest_at((uint32_t)_DAT_1f80020c),*(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x2040,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),0xf,
                   ctx,0x1f80035c,*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 8 + -0x7fe4e94c,
                   *(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x206c);
    }
  }
  if (((int)phase >> 4 & 1U) == 0) {
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xb1) ))= 1;
  }
  battle_intro_fsm(ctx,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle);
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
    fighter_tint_fade(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 8))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),battle,
                 (int)*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98))));
  }
  if (((int)phase >> 3 & 1U) != 0) {
    fighter_trail_emit((uint32_t *)guest_at((uint32_t)ctx),0x1f80033c,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),battle);
  }
  if (((*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))== '\0') && (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x9e) ))== '\0')) &&
     (((int)phase >> 5 & 1U) != 0)) {
    slot_height_proj(*(int *)guest_at((uint32_t)(uintptr_t)((battle + 8) ))+ 0x207c,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc))),
                 *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xc))),(int)(uintptr_t)battle);
  }
  return;

}
/* fighter_contact_resolve @0x8017f398 — per-frame contact pass for the fighter pair:
 * state_tick both fighters against the battle camera, then run the
 * contact/hit-resolution stages. */
void fighter_contact_resolve(int f0,int f1,int battle)
{
  int dir0;
  int dir1;
  uint32_t cam;
  int work1;
  int work0;

  cam = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0xbc)));
  work0 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38)));
  work1 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
  fighter_state_tick((uint32_t *)guest_at((uint32_t)(uintptr_t)(f0)),battle,cam);
  fighter_state_tick((uint32_t *)guest_at((uint32_t)(uintptr_t)(f1)),battle,cam);
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x10000) != 0) {
    return;
  }
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x40100000) != 0) ||
     ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x40100000) != 0)) {
    if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x40000000) != 0) &&
        ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x100000) != 0 && (0 < *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0xd4))))) &&
         ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x80) ))& 0xc0) == 0xc0)))) &&
       ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x7a) ))& 0xc0) != 0)) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x4000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x4000000;
      return;
    }
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x40000000) == 0) {
      return;
    }
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x100000) == 0) {
      return;
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0xd4) ))< 1) {
      return;
    }
    if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x80) ))& 0xc0) != 0xc0) {
      return;
    }
    if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x7a) ))& 0xc0) == 0) {
      return;
    }
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x4000000;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x4000000;
    return;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))& 0x8000) == 0) {
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))& 0x8000) == 0) {
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x6000000) != 0) {
        return;
      }
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x6000000) != 0) {
        return;
      }
      dir0 = fighter_atk_dir(f0,f1);
      dir1 = fighter_atk_dir(f1,f0);
      if (dir0 == 0 && dir1 == 0) {
        return;
      }
      if (dir0 == dir1) {
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))& 0x4000) != 0) {
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x600000;
        }
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))& 0x4000) != 0) {
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x600000;
        }
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
      }
      else {
        if (dir0 == -1) {
          if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))& 0x4000) != 0) {
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x600000;
          }
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
        }
        else if (dir0 == 1) {
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x400000;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x800000;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x24)));
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
          hit_pose_swap(f0,work0);
          hit_pose_swap(f1,work1);
        }
        if (dir1 != -1) {
          if (dir1 != 1) {
            return;
          }
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x400000;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x800000;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x24)));
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))= 0;
          goto LAB_8017f680;
        }
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))& 0x4000) != 0) {
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x600000;
        }
      }
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))= 0;
      return;
    }
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x20) ))= 0;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x400000;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x800000;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x24)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))| 0x10000000;
  }
  else {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x20) ))= 0;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| 0x400000;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))| 0x800000;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work1 + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((work0 + 0x24)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))| 0x10000000;
  }
LAB_8017f680:
  hit_pose_swap(f0,work0);
  hit_pose_swap(f1,work1);
  return;

}

/* camera_state_cache @0x8012ee48 — snapshot camera/light globals 0x1f80030c-0x320
 * into the 0x801f025x cache; uses rec_flag1_set values when _DAT_801f022c,
 * then derives aim angles via angle_gte_vec. */
void camera_state_cache(uint32_t unused,int battle)
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
    angle_gte_vec((int)_DAT_1f8002fa,(int)_DAT_1f8002f8,(uint32_t *)guest_at((uint32_t)0x1f80035c));
    vec2_rot((int)_DAT_1f8002fe,(int)_DAT_1f8002fc,(uint16_t*)guest_at((uint32_t)0x1f80033c));
    vec2_rot((int)_DAT_1f800302,(int)_DAT_1f800300,(uint16_t*)guest_at((uint32_t)0x1f800342));
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

/* player_light_apply @0x8012ecbc — load per-side GTE light vector, then (when the
 * rec_flag1_set latch is armed) rotate a secondary light by fighter
 * facing and dispatch the DAT_801b140c mode callback. */
void player_light_apply(uint8_t *ctrl,int side)
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
    vec2_rot(0x800U - (int)ang & 0xfff,0x1000 - tmp,(uint16_t*)guest_at((uint32_t)0x1f800348));
    if (side == 2) {
      _DAT_801f022c = 0;
    }
  }
  if (*(uint32_t *)(&DAT_801b140c + ((int)((uint32_t)DAT_1f800098 << 0x18) >> 0x16)) != 0) {
    uint32_t fn_g = *(uint32_t *)(&DAT_801b140c + ((int)((uint32_t)DAT_1f800098 << 0x18) >> 0x16));
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
  gte_light_load((uint32_t *)g_DAT_801f0250);
  return;

}
/* fighter_pair_update @0x801809c8 — per-frame update driver for the fighter pair:
 * heading/eligibility checks, height track, attach run, flash update and
 * the contact resolve cascade. */
void fighter_pair_update(int f0,int f1,int battle)
{
  short head0;
  short head1;
  uint16_t q;
  int t4;
  int elig1;
  uint32_t t10;
  int t7;
  int t8;
  int t9;
  /* scale0/scale1 feed hit_react_apply only on paths where the matching
   * hit_elig_check block already assigned them — dead-path init. */
  int scale0 = 0;
  uint32_t t11;
  int scale1 = 0;
  /* Guest stack layout (br2eu_func_801809C8): the two hitbox-result buffers
   * are 8-uint8_t slots at sp+0x18 and sp+0x20. hitbox_pair_test /
   * seg_sphere_hit / sphere_pt_hit write result shorts at indices 0..2
   * (6 bytes); the decompiler split each 8-uint8_t slot into a 2-short array
   * plus a trailing scalar, so index-2 writes ran off the end of the array
   * and smashed fighter_pair_update's saved x28 on the host stack
   * (observed: x28 low16 overwritten -> fighter_flash_update a3
   * garbage -> SIGSEGV). Model each slot as a 4-short array; local_34 is
   * hitres0[2], local_2c is hitres1[2]. */
  short hitres0 [4];
  short hitres1 [4];

  t9 = 0;
  t8 = 0;
  hitbox_corners_xform(f0,f1);
  fighter_height_track(f0,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38))));
  fighter_height_track(f1,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38))));
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x10000000) != 0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))| 0x40000000;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x10000000) != 0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))| 0x40000000;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x800000) != 0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))| 0x2000;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x800000) != 0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))| 0x2000;
  }
  t4 = hit_elig_check(f0,f1);
  elig1 = hit_elig_check(f1,f0);
  t7 = 0;
  if (t4 != 0) {
    t10 = fighter_range_code(f0,f1);
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))= t10;
    scale0 = hit_scale_calc(f0,f1);
    t9 = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0xda) ))* scale0;
    if (t9 < 0) {
      t9 = t9 + 0x7f;
    }
    t9 = t9 >> 7;
    t7 = hitbox_pair_test(f0,f1,(uint16_t *)hitres0);
  }
  t4 = 0;
  if (elig1 != 0) {
    t10 = fighter_range_code(f1,f0);
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x68) ))= t10;
    scale1 = hit_scale_calc(f1,f0);
    t8 = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0xda) ))* scale1;
    if (t8 < 0) {
      t8 = t8 + 0x7f;
    }
    t8 = t8 >> 7;
    t4 = hitbox_pair_test(f1,f0,(uint16_t *)hitres1);
  }
  if (t7 != 0) {
    if (t4 != 0) {
      if (t9 < t8) {
        t7 = 0;
      }
      else if (t8 < t9) {
        t4 = 0;
      }
    }
    if (t7 != 0) {
      hit_react_apply(f0,f1,scale0,hitres0,t7);
    }
  }
  if (t4 != 0) {
    hit_react_apply(f1,f0,scale1,hitres1,t4);
  }
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x48000) == 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x4000000) == 0)
     ) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xdfffffff;
  }
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x10000000) == 0) &&
     ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x2000000) == 0)) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xbfffdfff;
  }
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x48000) == 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x4000000) == 0)
     ) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0xdfffffff;
  }
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x10000000) == 0) &&
     ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x2000000) == 0)) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0xbfffdfff;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x60000000) == 0) {
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0xdd) ))= 0;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x60000000) == 0) {
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0xdd) ))= 0;
  }
  t9 = atan2_lut(*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))-
                       *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c))),
                       *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34) ))-
                       *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34))));
  camera_aim_offset(f0,f1,t9);
  t8 = hitbox_wall_collide(f0,(uint16_t *)hitres0,battle);
  t7 = hitbox_wall_collide(f1,(uint16_t *)hitres1,battle);
  if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x40000000) == 0) ||
      ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x100000) == 0)) &&
     (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x40000000) == 0 ||
      ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x100000) == 0)))) {
    if (t8 != 0) {
      node_translate_tree(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))),(int)hitres0[0],0,(int)hitres0[2]);
      *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))+ hitres0[0];
      *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))+ hitres0[2];
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18) ))=
           *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a) ))=
           *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c) ))=
           *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
    }
    if (t7 == 0) goto LAB_80180f88;
    node_translate_tree(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))),(int)hitres1[0],0,(int)hitres1[2]);
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))+ hitres1[0];
    t8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
  }
  else {
    if (t8 == 0 && t7 == 0) goto LAB_80180f88;
    hitres0[0] = hitres0[0] + hitres1[0];
    hitres0[2] = hitres0[2] + hitres1[2];
    node_translate_tree(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))),(int)hitres0[0],0,(int)hitres0[2]);
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))+ hitres0[0];
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))+ hitres0[2];
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
    node_translate_tree(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))),(int)hitres0[0],0,(int)hitres0[2]);
    *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))+ hitres0[0];
    t8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
    hitres1[2] = hitres0[2];
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8) ))+ hitres1[2];
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18) ))=
       *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a) ))=
       *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c) ))=
       *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c)));
LAB_80180f88:
  head0 = *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x4e)));
  head1 = *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x4e)));
  t8 = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
  t10 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
  t7 = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34)));
  t11 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c)));
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x74) ))= (short)((t9 - head0) * 0x100000 >> 0x14);
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x74) ))=
       (short)((int)(((t9 + 0x800U & 0xfff) - (int)head1) * 0x100000) >> 0x14);
  t9 = atan2_lut(t10,-t8);
  q = fixed_point_divide(t10,t8);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x72) ))= q;
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x70) ))= (short)((t9 - head0) * 0x100000 >> 0x14);
  t9 = atan2_lut(t11,-t7);
  q = fixed_point_divide(t11,t7);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x72) ))= q;
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x70) ))= (short)((t9 - head1) * 0x100000 >> 0x14);
  fighter_attach_run(f0);
  fighter_attach_run(f1);
  return;

}

/* fighter_flash_update @0x8016cd8c — fighter hit-flash/invuln blink update: per-fidx
 * record at DAT_801f4d70 (+0xbc stride) drives prim brightness/flags. */
void fighter_flash_update(uint32_t battle,uint32_t slot,int ctx2,uint8_t *rec)
{
  int roff;
  uint32_t flags;
  short *prec;
  uint8_t *prim;
  uint32_t fidx;
  uint8_t val;
  int bright;
  uint32_t pflags;
  short *recp;
  fidx = (uint32_t)*(uint8_t *)(rec + 2);
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
        *(uint16_t *)(&DAT_801f4d76 + roff) = 0;
        bright = 0;
      }
    }
    else {
      *(uint16_t *)(&DAT_801f4d76 + roff) = 0;
      bright = 0xff;
    }
    *(short *)(&DAT_801f4d74 + roff) = (short)bright;
  }
  else {
    bright = (int)*(short *)(&DAT_801f4d74 + roff);
  }
  if (bright == 0xff) {
    flags = *(uint32_t *)(rec + 0x6c) | 0x400;
  }
  else {
    flags = *(uint32_t *)(rec + 0x6c) & 0xfffffbff;
  }
  *(uint32_t *)(rec + 0x6c) = flags;
  if ((bright == *(short *)(&DAT_801f4d78 + roff)) && (bright == *(short *)(&DAT_801f4d7a + roff)))
  {
    return;
  }
  *(uint16_t *)(&DAT_801f4d7a + roff) = *(uint16_t *)(&DAT_801f4d78 + roff);
  *(short *)(&DAT_801f4d78 + roff) = (short)bright;
  val = (uint8_t)bright;
  switch(*(short *)(&DAT_801f4d72 + roff)) {
  case 0:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = val;
    *(uint8_t *)((intptr_t)recp + slot * 0x58 + 0x41) = val;
    *(uint8_t *)(recp + slot * 0x2c + 0x21) = val;
    val = (&DAT_801e40b6)[slot * 0x2150];
    pflags = 0x40;
    goto LAB_8016d1cc;
  case 1:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = val;
    *(uint8_t *)((intptr_t)recp + slot * 0x58 + 0x41) = val;
    *(uint8_t *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 2:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = val;
    *(uint8_t *)((intptr_t)recp + slot * 0x58 + 0x41) = 0;
    goto LAB_8016d1c0;
  case 3:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = 0;
    goto LAB_8016d1bc;
  case 4:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = 0;
    *(uint8_t *)((intptr_t)recp + slot * 0x58 + 0x41) = 0;
    *(uint8_t *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 5:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = val;
    goto LAB_8016d1bc;
  case 6:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = val;
    *(uint8_t *)((intptr_t)recp + slot * 0x58 + 0x41) = 0;
    *(uint8_t *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 7:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    *(uint8_t *)(recp + slot * 0x2c + 0x20) = 0;
    *(uint8_t *)((intptr_t)recp + slot * 0x58 + 0x41) = val;
    *(uint8_t *)(recp + slot * 0x2c + 0x21) = val;
    break;
  case 8:
    prim = &DAT_801f4d7c + slot * 0x58 + fidx * 0xbc;
    prec = recp + slot * 0x2c + 0x1e;
    *(char *)(recp + slot * 0x2c + 0x20) = (char)(bright / 2);
LAB_8016d1bc:
    *(uint8_t *)((intptr_t)prec + 5) = val;
LAB_8016d1c0:
    *(uint8_t *)(prec + 3) = 0;
    break;
  default:
    goto switchD_8016cecc_default;
  }
  val = (&DAT_801e40b6)[slot * 0x2150];
  pflags = 0x20;
LAB_8016d1cc:
  prim_packet_init_tex((int)guest_of(prim),1,val,pflags,0);
switchD_8016cecc_default:
  *(uint32_t *)(recp + slot * 0x2c + 0x2c) =
       *(uint32_t *)(recp + slot * 0x2c + 0x2c) & 0xff000000 |
       *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x207c) ))& 0xffffff;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x207c) ))=
       *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x207c) ))& 0xff000000 | guest_of(recp + slot * 0x2c + 6) & 0xffffff;
  return;

}
#define PTR_DAT_801b47ec   (*g_PTR_DAT_801b47ec)

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
extern uint8_t  **g_PTR_DAT_801b47ec;
extern uint8_t  *_g_pad_buffer;
extern uint8_t  *g_DAT_801b4b94, *g_DAT_801b4b95, *g_DAT_801b4b96, *g_DAT_801b4b97, *g_DAT_801b4e28;
extern uint16_t *g_DAT_801b4b54, *g_DAT_801b4b56;
extern uint8_t  *g_DAT_801b1840, *g_DAT_801b1844, *g_DAT_801b1848, *g_DAT_801b184c, *g_DAT_801b1850, *g_DAT_801b1854, *g_DAT_801b1858, *g_DAT_801b185c, *g_DAT_801b1d78, *g_DAT_801b1d70, *g_DAT_8011fd7a, *g_DAT_1f800394, *g_DAT_1f80039c, *g_DAT_8011fd78, *g_DAT_8011fd98, *g_DAT_801f04a8, *g_DAT_801f04c8, *g_DAT_801f04c9, *g_DAT_801f04e4, *g_DAT_1f8003a8, *g_DAT_1f8003a0, *g_DAT_1f8003b4, *g_DAT_801b1ea4, *g_DAT_1f800108, *g_DAT_1f8003f8, *g_DAT_801be838, *g_DAT_801b17f4, *g_DAT_801b1528, *g_DAT_801b152a, *g_DAT_1f8000eb, *g_DAT_1f8000ec, *g_DAT_1f800148, *g_DAT_1f8003e0, *g_DAT_8011fea0, *g_DAT_8011fea1, *g_DAT_8011fea2, *g_DAT_8011feac, *g_DAT_8011fead, *g_DAT_8011feae, *g_DAT_801bef20, *g_DAT_801e40ba, *g_DAT_801e40bb, *g_DAT_801f0664, *g_DAT_801f0750, *g_DAT_801f0780, *g_DAT_801f0790, *g_DAT_801f0ad8, *g_DAT_801f0ad9, *g_DAT_801f0ae4, *g_DAT_801f4cbc, *g_DAT_8011ed80, *g_DAT_8011ede0, *g_DAT_8011ee40, *g_DAT_8011ee5b, *g_DAT_8011ee74, *g_DAT_8011ee8f, *g_DAT_8011eea4, *g_DAT_8011eed4, *g_DAT_8011ef04, *g_DAT_8011ef64, *g_DAT_8011ef75, *g_DAT_801f0300, *g_DAT_801f0304, *g_DAT_801f0328, *g_DAT_801237e0, *g_DAT_80124048;
extern int32_t *g__DAT_801f04a4;
extern uint32_t *g__DAT_1f80000c, *g__DAT_1f8003e0, *g__DAT_1f8003e4, *g__DAT_1f8003e8, *g__DAT_1f8003ec, *g__DAT_1f8003f0, *g__DAT_1f8003f4, *g__DAT_1f8003f8, *g__DAT_1f8003fc, *g__DAT_801c4508, *g__DAT_801f0298, *g__DAT_801f029a, *g__DAT_1f80038e, *g__DAT_1f800392, *g__DAT_1f800396, *g__DAT_1f80039a, *g__DAT_1f80039e, *g__DAT_1f8003a2, *g__DAT_801f0228, *g__DAT_801f04b0, *g__DAT_801f04c0, *g__DAT_801f04c6, *g__DAT_801f0730, *g__DAT_801f0734, *g__DAT_801f0738, *g__DAT_801f0740, *g__DAT_801f0744, *g__DAT_801f0758, *g__DAT_801f075c, *g__DAT_801f0930, *g__DAT_801f0938, *g__DAT_801f093a, *g__DAT_801f093c, *g__DAT_801f093e, *g__DAT_801f0940, *g__DAT_801f0944, *g__DAT_801f4cb4, *g__DAT_801f4cb8, *g__DAT_801f4cba, *g__DAT_1f80037c, *g__DAT_1f80037e, *g__DAT_1f800380, *g__DAT_1f800388, *g__DAT_1f8003cc, *g__DAT_1f8003d0, *g__DAT_1f8003d4, *g__DAT_1f8003d8, *g__DAT_1f8003dc, *g__DAT_1f800008, *g__DAT_1f800024, *g__DAT_801f68e0, *g__DAT_801f04a0, *g__DAT_801f04b8, *g__DAT_801f04bc, *g__DAT_801f0760, *g__DAT_801f0764, *g__DAT_801f0768, *g__DAT_801f076c, *g__DAT_801f0770;
extern uint32_t PTR_entry_0x80182f3c_801b5528[];
extern uint8_t **g_PTR_DAT_801be84c;
extern uint32_t **g_PTR_PTR_801be894;
#define PTR_DAT_801be84c   (*g_PTR_DAT_801be84c)
#define PTR_PTR_801be894   (*g_PTR_PTR_801be894)
extern code    **PTR_FUN_801b1734;
extern void     trap(int code);
extern uint8_t  *g_DAT_801e40b9, *g_DAT_801f066e, *g_DAT_1f800015, *g_DAT_801247a8, *g_DAT_801b4b6c, *g_DAT_801b4b6d, *g_DAT_801b4b6e, *g_DAT_801b4b6f, *g_DAT_801b67a0, *g_DAT_801b67a4, *g_DAT_801b67a8;
extern uint32_t *g__DAT_1f8000c4;
#define _DAT_1f8000c4   (*g__DAT_1f8000c4)
extern char     s__1___02d__1x_0_80123264[];
extern char     s_0123456789ABCDEF_80125208[], s_0123456789abcdef_8012521c[];
extern void     setCopReg(int fmt,int reg,int v);
extern void     copFunction(int fmt,int op);
extern int      getCopReg(int fmt,int reg);
extern void     setCopControlWord(int fmt,int reg,int v);
extern CPUState *clean_cpu_state(void);
/* Incoming guest register value for the Ghidra "in_*" spill locals: the
 * guest saves s0-s7/t0-t9/ra to the 0x1f8003e0 scratch area on entry, so the
 * faithful host value is the live CPUState gpr at call time. */
static uint32_t br2_gpr(int r){ CPUState *c = clean_cpu_state(); return c ? c->gpr[r] : 0; }
extern int lut_cos_q12(int angle);
extern int lut_sin_q12(uint32_t angle);
extern int FUN_80194500(uint32_t val, ...);

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

/* battle_camera_step @0x8012fb10 — per-frame battle camera update: track
 * both fighters, apply shot/track modes, collision resolve and write
 * the camera state consumed by battle_hud_draw/GTE setup. */
void battle_camera_step(uint32_t cam,uint8_t *f0,uint8_t *f1,int battle)
{
  char fi;
  short t5;
  uint16_t ta;
  uint16_t tb;
  uint16_t t3;
  short t1;
  uint32_t ti;
  int t4;
  int t0;
  uint32_t *dst;
  int t2;
  uint32_t stp;
  /* ba/ff are guest $s2/$s5 (subu @0x80131750 -> gpr[18], lbu @0x80131748
   * -> gpr[21]): only the cam+0x78==0 init blocks write them, but the
   * t1==1/2/3 approach paths read them whenever cam+0x78 was left
   * nonzero by an earlier state — i.e. the callee-saved register
   * values live at that point (entry values if unwritten this call). */
  short ba = (short)br2_gpr(18);
  uint32_t ff = br2_gpr(21);
  /* Guest vec3 slots: target_angles_depth_set / target_depth_set read
   * 3 contiguous s16 through ang_prev; FUN_80194630, pair_depth_set and
   * midpoint_angles_set read/write 3 contiguous u32 through dv,
   * pa and pb. The decompiler split each vec3 into scalars.
   * ang_prev[0..2] = old ang_prev/local_96/local_94;
   * dv[0..2] = old local_90/local_8c/local_88;
   * pa[0..2] = old local_80/local_7c/local_78;
   * pb[0..2] = old local_70/local_6c/local_68. */
  short ang_prev [3];
  int dv [3];
  int pa [3];
  int pb [3];
  uint32_t v0 [2];
  uint32_t v1 [2];
  uint32_t v2 [2];
  uint32_t v3 [2];
  uint32_t v4 [2];
  uint32_t v5 [2];
  uint32_t v6 [2];
  uint16_t lim [4];

  switch(*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x7f) {
  case 0:
    if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x80) == 0) {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a) ))< 1) {
        vec3_step_toward_spd(v1,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),4);
      }
      else {
        vec3_step_toward(v0,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a))));
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38) ))< 1) {
        vec3_step_toward_q_spd(v3,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      }
      else {
        vec3_step_toward_q(v2,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38))));
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))< 1) {
        vec3_step_toward_spd(v5,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),4);
      }
      else {
        vec3_step_toward(v4,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c))));
      }
    }
    else {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x7f;
    }
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
    goto LAB_80131f4c;
  case 1:
    t0 = (int)*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80)));
    if (t0 < 0) {
      if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81)))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
        fi = *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
        t0 = *(int *)(f1 + 0x20);
        goto LAB_8012fec0;
      }
    }
    else if (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0') {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
           *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t0 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
      fi = *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
           *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
      t0 = *(int *)(f0 + 0x20);
LAB_8012fec0:
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((fi * 0x50 + t0 + 0x3c)));
    }
    else {
      pa[0] = *(int *)guest_at((uint32_t)(uintptr_t)((t0 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
      pb[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= (short)((pa[0] + pb[0]) / 2);
      pa[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
      pb[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= (short)((pa[1] + pb[1]) / 2);
      pa[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
      pb[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= (short)((pa[2] + pb[2]) / 2);
    }
    if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x80) == 0) {
      vec3_step_toward_spd(v0,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42))),4);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a) ))< 1) {
        vec3_step_toward_spd(v2,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),4);
      }
      else {
        vec3_step_toward(v1,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a))));
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38) ))< 1) {
        vec3_step_toward_q_spd(v4,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      }
      else {
        vec3_step_toward_q(v3,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38))));
      }
      dst = v5;
      if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c)))) goto LAB_80130048;
      dst = v6;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
        if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
          dst = v6;
        }
        else {
          pair_depth_set((short *)pa,(short *)pb,cam);
          dst = v6;
        }
      }
LAB_8013049c:
      vec3_step_toward_spd(dst,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),4);
    }
    else {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
        if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
        }
        else {
          pair_depth_set((short *)pa,(short *)pb,cam);
        }
      }
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x7f;
    }
    break;
  case 2:
    t0 = (int)*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80)));
    if (t0 < 0) {
      if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81)))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
        t1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
        t5 = *(short *)(f1 + 0x4e);
        goto LAB_801302d0;
      }
    }
    else {
      if (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0') {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t0 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
        t1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
        t5 = *(short *)(f0 + 0x4e);
LAB_801302d0:
        t3 = (t1 + 0x1000) - t5;
      }
      else {
        pa[0] = *(int *)guest_at((uint32_t)(uintptr_t)((t0 * 0x50 + *(int *)(f0 + 0x20) + 0x34)));
        pb[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= (short)((pa[0] + pb[0]) / 2);
        pa[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
        pb[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= (short)((pa[1] + pb[1]) / 2);
        pa[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
        pb[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
        t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= (short)((pa[2] + pb[2]) / 2);
      }
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= t3 & 0xfff;
    }
    if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))& 0x80) == 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a) ))< 1) {
        vec3_step_toward_spd(v1,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),4);
      }
      else {
        vec3_step_toward(v0,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5a))));
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38) ))< 1) {
        vec3_step_toward_q_spd(v3,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      }
      else {
        vec3_step_toward_q(v2,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x38))));
      }
      dst = v4;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))< 1) {
        dst = v5;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
          if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
            target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
            dst = v5;
          }
          else {
            pair_depth_set((short *)pa,(short *)pb,cam);
            dst = v5;
          }
        }
        goto LAB_8013049c;
      }
LAB_80130048:
      vec3_step_toward(dst,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 100))),*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c))));
    }
    else {
      ta = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      tb = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= tb;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= tb;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= ta;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= ta;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= ta;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= tb;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56)));
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x6c) ))== -1) {
        if ((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))< '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))< '\0')) {
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
        }
        else {
          pair_depth_set((short *)pa,(short *)pb,cam);
        }
      }
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
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
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
        gte_cw_d000_set(0x168);
      }
      dual_target_track_b((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    }
    else {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))!= 0x168) {
        lim[0] = 0x168;
        u16_step_toward((uint16_t *)guest_at((uint32_t)(battle + 0x24)),(uint16_t *)(uintptr_t)lim,10);
        gte_cw_d000_set((int)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      }
      dual_target_track((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    }
    break;
  case 4:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set((uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      ff = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x38) + 0x100)));
      ba = 0x1000 - (*(short *)(f0 + 0x4e) + *(short *)(f0 + 0xc6));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))= 0;
      t3 = table_byte_next_a();
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= (t3 & 3) + 1;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x128)));
    }
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x124)));
    t0 = *(int *)(f0 + 0x20);
    goto LAB_8013181c;
  case 5:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set((uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      ff = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x38) + 0x100)));
      ba = 0x1000 - (*(short *)(f1 + 0x4e) + *(short *)(f1 + 0xc6));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))= 0;
      t3 = table_byte_next_a();
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= (t3 & 3) + 1;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x128)));
    }
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x124)));
    t0 = *(int *)(f1 + 0x20);
LAB_8013181c:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t0 + 300)));
    t1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78)));
    if (t1 == 2) {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x380;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x280;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x180;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30) ))= 0xff80;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= ba;
      }
      t1 = fixed_approach_s16((uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),4);
      t0 = cam + 0x60;
      if (t1 == 0) {
        t2 = cam + 0x68;
        stp = 2;
        goto LAB_80131a34;
      }
    }
    else if (t1 < 3) {
      if (t1 != 1) goto LAB_80131f4c;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x380;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x280;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x100;
        if (ff == 0) {
          t1 = -0x600;
        }
        else {
          t1 = 0x600;
        }
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= ba + t1;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= ba;
      }
      t1 = fixed_approach_s16((uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32))),10);
      t0 = cam + 0x60;
      if (t1 == 0) {
        t2 = cam + 0x68;
        stp = 2;
        goto LAB_80131a34;
      }
    }
    else if (t1 == 3) {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x300;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
        if (ff == 0) {
          t1 = -0xc00;
        }
        else {
          t1 = 0xc00;
        }
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= ba + t1;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= ba;
      }
      t1 = fixed_approach_s16((uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32))),10);
      t0 = cam + 0x60;
      if (t1 == 0) {
        t2 = cam + 0x68;
        stp = 2;
LAB_80131a34:
        fixed_approach_s16((uint16_t *)guest_at((uint32_t)t0),(short *)guest_at((uint32_t)t2),stp);
      }
    }
    else {
      if (t1 != 4) goto LAB_80131f4c;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))== 0) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68) ))= 0x380;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x80;
        if (ff == 0) {
          t1 = -0xe00;
        }
        else {
          t1 = 0xe00;
        }
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= ba + t1;
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= ba;
      }
      t1 = fixed_approach_s16((uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68))),2);
      t0 = cam + 0x2a;
      if (t1 == 0) {
        t2 = cam + 0x32;
        stp = 5;
        goto LAB_80131a34;
      }
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8c) ))+ 1;
    goto LAB_80131f4c;
  case 6:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= *(uint16_t *)(f0 + 0x4e);
    goto LAB_80131ae0;
  case 7:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= *(uint16_t *)(f1 + 0x4e);
LAB_80131ae0:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0x400;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0xa00;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set((uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
LAB_80131b40:
      if (0x300 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60)))) {
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))+ -2;
      }
    }
    else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 1) goto LAB_80131b40;
    t3 = 0x1000 - *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a)));
    goto LAB_80131b64;
  case 8:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    camera_track_shot((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    break;
  case 9:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    camera_track_shot((int)(uintptr_t)cam,(int)guest_of(f1),(int)guest_of(f0));
    break;
  case 10:
  case 0xb:
    switch(*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78)))) {
    case 0:
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
      gte_cw_d000_set(0x168);
      ti = table_byte_next_a();
      ti = ti & 3;
      if (ti == 1) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
          t0 = *(int *)(f0 + 0x20);
        }
        else {
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
          t0 = *(int *)(f1 + 0x20);
        }
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x17c)));
        ti = table_byte_next_b();
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= *(uint16_t *)(&DAT_8011fa04 + (ti & 3) * 2);
        ti = table_byte_next_b();
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x72) ))= *(uint16_t *)(&DAT_8011fa0c + (ti & 1) * 2);
        t3 = table_byte_next_b();
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x70) ))= t3 & 1;
        t1 = table_byte_next_b();
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= t1 << 4;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 2;
        goto switchD_801306ac_caseD_2;
      }
      if (ti < 2) {
        if (ti == 0) {
          ti = table_byte_next_a();
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)(&DAT_8011f9c4 + (ti & 7) * 8);
          ti = table_byte_next_a();
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)(&DAT_8011f9c6 + (ti & 7) * 8);
          ti = table_byte_next_a();
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)(&DAT_8011f9c8 + (ti & 7) * 8);
          pa[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
          pa[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
          pa[2] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
          if (-0x100 < pa[1]) {
            pa[1] = -0x100;
          }
          pb[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
          pb[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
          pb[2] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
          if (-0x100 < pb[1]) {
            pb[1] = -0x100;
          }
          midpoint_angles_set(pa,pb,cam);
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 1;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
        }
      }
      else {
        if (ti == 2) {
          if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
            t1 = *(short *)(f0 + 0x4e);
          }
          else {
            t1 = *(short *)(f1 + 0x4e);
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0x800 - t1;
          ti = table_byte_next_b();
          if ((ti & 1) == 0) {
            t1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ -0x180;
          }
          else {
            t1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ 0x180;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= t1;
          if (*(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))== '\x06') {
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
          }
          else {
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0xff00;
          }
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 3;
          goto switchD_801306ac_caseD_3;
        }
        if (ti == 3) {
          if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
            t0 = (int)*(short *)(f1 + 0x4e) + (int)*(short *)(f1 + 0x74);
          }
          else {
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
            t0 = (int)*(short *)(f0 + 0x4e) + (int)*(short *)(f0 + 0x74);
          }
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= 0xff00;
          ti = table_byte_next_a();
          if ((ti & 1) == 0) {
            t0 = t0 + 0x1000;
          }
          else {
            t0 = t0 + 0x800;
          }
          t2 = lut_cos_q12(t0);
          t2 = t2 * 0x300;
          if (t2 < 0) {
            t2 = t2 + 0xfff;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))+ (short)(t2 >> 0xc);
          t0 = lut_sin_q12(t0);
          t0 = t0 * 0x300;
          if (t0 < 0) {
            t0 = t0 + 0xfff;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))+ (short)(t0 >> 0xc);
          target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
          target_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= 4;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
        }
      }
    case 1:
switchD_801306ac_caseD_1:
      pa[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
      pa[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
      pa[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
      if (((*(uint32_t *)(f0 + 0x5c) & 0x80000) == 0) && (-0x100 < pa[1])) {
        pa[1] = -0x100;
      }
      pb[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
      pb[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
      pb[2] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
      if (((*(uint32_t *)(f1 + 0x5c) & 0x80000) == 0) && (-0x100 < pb[1])) {
        pb[1] = -0x100;
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= (short)((pa[0] + pb[0]) / 2);
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= (short)((pa[1] + pb[1]) / 2);
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= (short)((pa[2] + pb[2]) / 2);
      dv[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a)));
      dv[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c)));
      dv[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e)));
      t0 = FUN_80194630((uint32_t *)dv,(uint32_t *)dv);
      if (t0 != 0) {
        t0 = FUN_80194500(t0);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))+ (short)((uint32_t)(dv[0] * t0) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))+ (short)((uint32_t)(dv[1] * t0) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))+ (short)((uint32_t)(dv[2] * t0) >> 0x10);
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      vec3_step_toward_q(v0,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),0x40);
      pair_depth_set((short *)pa,(short *)pb,cam);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) goto LAB_80131e94;
      t2 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60)));
      t4 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      t0 = t2 - t4;
      if (t4 <= t2) goto LAB_80131278;
      if (t0 < 0) {
        t0 = -t0;
      }
      if (t0 < 0) {
        t0 = t0 + 3;
      }
      t0 = t0 << 0xe;
      goto LAB_8013129c;
    case 2:
switchD_801306ac_caseD_2:
      t1 = -8;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x70) ))== 0) {
        t1 = 8;
      }
      t1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))+ t1;
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= t1;
      t0 = lut_cos_q12((int)t1);
      t0 = t0 * *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x72)));
      if (t0 < 0) {
        t0 = t0 + 0xfff;
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))+ (short)(t0 >> 0xc);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)));
      t0 = lut_sin_q12((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a))));
      t0 = t0 * *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x72)));
      if (t0 < 0) {
        t0 = t0 + 0xfff;
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))+ (short)(t0 >> 0xc);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
        ti = *(uint32_t *)(f0 + 0x5c);
      }
      else {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
        ti = *(uint32_t *)(f1 + 0x5c);
      }
      if (((ti & 0x80000) == 0) && (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c))))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= 0xff00;
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      break;
    case 3:
switchD_801306ac_caseD_3:
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
        ti = *(uint32_t *)(f0 + 0x5c);
      }
      else {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
        ti = *(uint32_t *)(f1 + 0x5c);
      }
      if (((ti & 0x80000) == 0) && (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c))))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
      }
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
      break;
    case 4:
      if (((*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 10) && ((*f1 & 0x80) == 0)) ||
         ((*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0xb && ((*f0 & 0x80) != 0)))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
        ti = *(uint32_t *)(f0 + 0x5c);
      }
      else {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
        ti = *(uint32_t *)(f1 + 0x5c);
      }
      if (((ti & 0x80000) == 0) && (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c))))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= 0xff00;
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      vec3_step_toward_q(v0,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),0x40);
    }
    break;
  case 0xc:
  case 0xd:
switchD_8012fb70_caseD_c:
    camera_track_step((int)(uintptr_t)cam,(int)guest_of(f0),(int)guest_of(f1));
    break;
  case 0xe:
  case 0x10:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0x400;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0xff00;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x300;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= 0;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0xe) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x124)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x128)));
      t0 = *(int *)(f0 + 0x20);
    }
    else {
LAB_8013062c:
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x124)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x128)));
      t0 = *(int *)(f1 + 0x20);
    }
    goto LAB_8013065c;
  case 0xf:
  case 0x11:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0xf) {
      if ((*f1 & 0x80) != 0) {
        ti = *(uint32_t *)(f1 + 0x68);
LAB_80130574:
        if ((ti & 0x2000) != 0) goto switchD_8012fb70_caseD_c;
      }
    }
    else if ((*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))== 0x11) && ((*f0 & 0x80) != 0)) {
      ti = *(uint32_t *)(f0 + 0x68);
      goto LAB_80130574;
    }
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= 0xfe00;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x24) ))= 0x168;
    gte_cw_d000_set((uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x24))));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))= 0;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x76) ))!= 0xf) goto LAB_8013062c;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x124)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x128)));
    t0 = *(int *)(f0 + 0x20);
LAB_8013065c:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t0 + 300)));
    if (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)))) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
    }
    break;
  default:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))!= 0) break;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
    goto LAB_80131e98;
  case 0x1c:
    ang_prev[0] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
    ang_prev[1] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
    ang_prev[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
    ti = *(uint32_t *)(f0 + 0x5c);
    goto LAB_80131c60;
  case 0x1d:
    ang_prev[0] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
    ang_prev[1] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
    ang_prev[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
    ti = *(uint32_t *)(f1 + 0x5c);
LAB_80131c60:
    if (((ti & 0x80000) == 0) && (-0x100 < ang_prev[1])) {
      ang_prev[1] = -0x100;
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= (&DAT_8011f944)[*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 4];
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
           *(uint16_t *)(&DAT_8011f946 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
           *(uint16_t *)(&DAT_8011f948 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      target_angles_depth_set(ang_prev,cam);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
    }
    else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 1) {
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x42) ))= ang_prev[0];
      dv[0] = (int)ang_prev[0] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x44) ))= ang_prev[1];
      dv[1] = (int)ang_prev[1] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x46) ))= ang_prev[2];
      dv[2] = (int)ang_prev[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e)));
      t0 = FUN_80194630((uint32_t *)dv,(uint32_t *)dv);
      if (t0 != 0) {
        t0 = FUN_80194500(t0);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))+ (short)((uint32_t)(dv[0] * t0) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))+ (short)((uint32_t)(dv[1] * t0) >> 0x10);
        *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))+ (short)((uint32_t)(dv[2] * t0) >> 0x10);
      }
      target_angles_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
      vec3_step_toward_q(v0,(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30))),0x40);
      target_depth_set(ang_prev,cam);
      if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
LAB_80131e94:
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
LAB_80131e98:
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
      }
      else {
        t2 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60)));
        t4 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
        t0 = t2 - t4;
        if (t2 < t4) {
          if (t0 < 0) {
            t0 = -t0;
          }
          if (t0 < 0) {
            t0 = t0 + 3;
          }
          t0 = t0 << 0xe;
        }
        else {
LAB_80131278:
          t2 = t2 - t4;
          if (t2 < 0) {
            t2 = -t2;
          }
          if (t2 < 0) {
            t2 = t2 + 0xf;
          }
          t0 = t2 << 0xc;
        }
LAB_8013129c:
        u16_step_toward((uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60))),(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68))),t0 >> 0x10);
      }
    }
    break;
  case 0x1e:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
    }
    else {
      t0 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)))) * 0x100000 >> 0x14
      ;
      if (t0 < 0) {
        t0 = -t0;
      }
      if (t0 < 0) {
        t0 = t0 + 0x1f;
      }
      fixed_approach_s16((uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32))),(t0 << 0xb) >> 0x10);
    }
    break;
  case 0x20:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
      pa[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x174)));
      pa[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x178)));
      pa[2] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x17c)));
      if (((*(uint32_t *)(f0 + 0x5c) & 0x80000) == 0) && (-0x100 < pa[1])) {
        pa[1] = -0x100;
      }
      pb[0] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x174)));
      pb[1] = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x178)));
      pb[2] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x17c)));
      if (((*(uint32_t *)(f1 + 0x5c) & 0x80000) == 0) && (-0x100 < pb[1])) {
        pb[1] = -0x100;
      }
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= (&DAT_8011f944)[*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 4];
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
           *(uint16_t *)(&DAT_8011f946 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
           *(uint16_t *)(&DAT_8011f948 + *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7e) ))* 8);
      midpoint_angles_set(pa,pb,cam);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x42)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x44)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x46)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
    }
    else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))!= 1) break;
    goto switchD_801306ac_caseD_1;
  case 0x21:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd8)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xdc)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd8)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xdc)));
    }
    else {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x38) ))+ -0x180;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x3c)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x34)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x38)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x3c)));
    }
    target_angles_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
      goto LAB_801315ec;
    }
LAB_801315d4:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0x200;
    goto LAB_801315f4;
  case 0x22:
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))== 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xd8)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0xdc)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xd8)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0xdc)));
    }
    else {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x38) ))+ -0x180;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f1 + 0x20) + 0x3c)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x34)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x38)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x3e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(f0 + 0x20) + 0x3c)));
    }
    target_angles_depth_set((short *)guest_at((uint32_t)(uintptr_t)((cam + 0x3a))),cam);
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x30)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7c) ))!= 0) goto LAB_801315d4;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0;
LAB_801315ec:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0;
LAB_801315f4:
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ -0x400;
    goto LAB_80131f4c;
  case 0x23:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x34)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x38)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x80) ))* 0x50 + *(int *)(f0 + 0x20) + 0x3c)));
    t3 = (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))+ 0x1000) - *(short *)(f0 + 0x4e) & 0xfff;
    goto LAB_80131b64;
  case 0x24:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x34)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x38)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(char *)guest_at((uint32_t)(uintptr_t)((cam + 0x81) ))* 0x50 + *(int *)(f1 + 0x20) + 0x3c)));
    t3 = (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x8a) ))+ 0x1000) - *(short *)(f1 + 0x4e) & 0xfff;
LAB_80131b64:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= t3;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= t3;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
    goto LAB_80131f4c;
  }
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
LAB_80131f4c:
  t1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x84)));
  t0 = t1 * -3;
  if (t0 < 0) {
    t0 = t0 + 3;
  }
  t5 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x86)));
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x84) ))= (short)(t0 >> 2);
  t0 = t5 * -3;
  if (t0 < 0) {
    t0 = t0 + 3;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x86) ))= (short)(t0 >> 2);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x48) ))= 0x1000;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x4c) ))= 0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x50) ))= 0x1000;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x54) ))= 0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((battle + 0x58) ))= 0x1000;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x5c) ))= -(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a)));
  t0 = battle + 0x28;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x60) ))= -(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)));
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 100) ))= -(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e)));
  rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a))),(uint32_t *)guest_at((uint32_t)t0));
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))+ (int)t5;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x40) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))+ ((int)t1 << 8) / 10000;
  *(int *)guest_at((uint32_t)(uintptr_t)((battle + 0x44) ))=
       mips_div_lo((int32_t)((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))<< 0xc),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x7a)))); /* guest div @0x80132030 */
  rtps_basis_xform((uint32_t *)guest_at((uint32_t)t0),(uint16_t *)guest_at((uint32_t)(battle + 0x48)),(uint32_t *)guest_at((uint32_t)t0));
  rtps_basis_xform((uint32_t *)guest_at((uint32_t)cam),(uint16_t *)guest_at((uint32_t)t0),(uint32_t *)guest_at((uint32_t)t0));
  return;

}
/* battle_hud_draw @0x801737ec — HUD element dispatcher: digit/cell sprite
 * emission, gauge bars, portraits and per-type element cases drawing
 * into the shared OT via sprite_strip/prim helpers. */
void battle_hud_draw(int unused,int fighter,int aux0,int aux1)
{
  short r1;
  short r0;
  uint32_t t8;
  int t0;
  uint32_t *el;
  int t6;
  int t7;
  uint32_t *w0;
  uint32_t *w1;
  int *slot;
  uint32_t ad;
  uint32_t t9;
  uint8_t *aux;
  uint32_t *rd;
  short *tA;
  uint32_t *pkt;
  short *tB;
  int t1;
  int t2;
  int t3;
  uint16_t *tC;
  int t4;
  int t5;
  int ap [2];
  uint32_t *rectbl;
  uint32_t *rectbl2;
  uint32_t *rectbl3;
  uint32_t *o0;
  int o1;
  int *o2;
  int o3;
  uint8_t *o4;
  int o5;
  uint32_t *o6;
  int o7;
  int *o8;
  int o9;
  uint8_t *o10;
  int o11;
  int o12;
  int o13;
  uint32_t *o14;

  t0 = *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc)));
  rectbl2 = (uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 * 0xc0 + -0x7fe0b0d0)));
  rectbl3 = (uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 * 0x90 + -0x7fe0af50)));
  rectbl = (uint32_t *)guest_at(*(uint32_t *)(&DAT_801f5210 + t0 * 4));
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc4) ))+ 8) ))>> 1 & 1) == 0) {
    _DAT_801f4f20 = _DAT_801f4f20 + 1;
    ap[0] = aux0;
    ap[1] = aux1;
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
    pkt = rectbl + 10;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((fighter + 0x94) ))< 0) {
      t0 = -1;
    }
    else {
      t0 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((fighter + 0x94) ))/ 0x32) * 0x10000 >> 0x10;
    }
    hud_digits_color(t0,(int)guest_of(pkt));
    hud_cell_frame_set(t0,(int)guest_of(pkt));
    t0 = 0;
    do {
      t0 = t0 + 1;
      *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(pkt) & 0xffffff;
      pkt = pkt + 10;
    } while (t0 < 2);
    *rectbl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(rectbl) & 0xffffff;
    if (0x20 < (int)_DAT_801f4f20) {
      t9 = 0x50;
      if (*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0') {
        t9 = 0x28;
      }
      hud_text_blk_alt(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                   (const void *)guest_at(((uint32_t *)&PTR_DAT_801b47ec)[(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((aux0 + 3) ))* 2]),0x22,t9,0);
      hud_text_blk_alt(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                   (const void *)guest_at(((uint32_t *)&PTR_DAT_801b47ec)[(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((aux1 + 3) ))* 2]),
                   (int)*(short *)(&DAT_801b4a30 + (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((aux1 + 3) ))* 2),t9,0);
    }
    t2 = 0;
    aux = &DAT_801f51d8;
    t4 = 0x7f8;
    t1 = 0;
    o13 = 0;
    t3 = 0x5c8;
    pkt = (uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)(&DAT_801f5210 + *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))* 4) + 0x5c8)));
    t0 = aux0;
    do {
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xd6) ))< 0x1e00) || (0x1dff < *(short *)(aux + 0x12))) {
LAB_80173dd8:
        if (-1 < *(short *)(aux + 0x1a)) goto LAB_80173de8;
      }
      else {
        if (*(short *)(aux + 0x1a) < 0) {
          if (*(char *)guest_at((uint32_t)(uintptr_t)((t0 + 5) ))== '\0') {
            task_alloc_wrap_a(_DAT_801f5210 + t3 + 0xe,_DAT_801f5214 + t3 + 0xe, (uint32_t)br2_gaddr(&DAT_801b4de8),
                          (uint32_t)br2_gaddr(&DAT_801b4e08));
            *(uint16_t *)(aux + 0x18) = 0x1000;
            *(uint16_t *)(aux + 0x1a) = 0x78;
            slot = (int *)((intptr_t)ap + t1);
            triple_track_spawn(*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*slot + 2))),0x30,0);
            t0 = fx_group_alloc(1,1,0,0x13);
            if (t0 != 0) {
              noop_a(t0,fighter + t2 * 2 + 0x6c,*(int *)guest_at((uint32_t)(uintptr_t)((*slot + 0x20) ))+ 0x124,
                            (uint32_t)br2_gaddr(&DAT_801f0760));
            }
            group_fx_spawn(*slot,0x1b,*(int *)guest_at((uint32_t)(uintptr_t)((*slot + 0x20) ))+ 0x124,0xe00,0x2000,0,5);
          }
          goto LAB_80173dd8;
        }
LAB_80173de8:
        sprite_rect_set_c((int)guest_of(pkt),(int)*(short *)((intptr_t)&DAT_801b4c88 + o13) +
                             ((int)*(short *)((intptr_t)&DAT_801b4c9c + t1) *
                              (int)*(short *)(aux + 0x18) >> 0xc),
                     (int)*(short *)((intptr_t)&DAT_801b4c8a + o13) +
                     ((int)*(short *)((intptr_t)&DAT_801b4c9e + t1) * (int)*(short *)(aux + 0x18)
                     >> 0xc),(int)*(short *)((intptr_t)&DAT_801b4c8c + o13),
                     (int)*(short *)((intptr_t)&DAT_801b4c8e + o13));
        if (0x50 < *(short *)(aux + 0x1a)) {
          *(short *)(aux + 0x18) = (short)(*(short *)(aux + 0x18) * 0x1a9 >> 9);
        }
        if (*(short *)(aux + 0x1a) == 100) {
          slot = (int *)(&DAT_801f4f28 + t1);
          if (*slot != 0) {
            task_node_free((int *)(uintptr_t)*slot);
          }
          t0 = task_alloc_wrap_b(_DAT_801f5210 + t4,_DAT_801f5214 + t4,(uint32_t)br2_gaddr(slot), (uint32_t)br2_gaddr(&DAT_801b4dd4),
                                (uint32_t)br2_gaddr(&DAT_801b4dbc));
          *slot = t0;
        }
        if (*(short *)(aux + 0x1a) < 0x28) {
          *(short *)(aux + 0x18) = *(short *)(aux + 0x18) + 500;
        }
        *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= guest_of(pkt) & 0xffffff;
        *(short *)(aux + 0x1a) = *(short *)(aux + 0x1a) + -1;
      }
      pkt = pkt + 10;
      aux = aux + 0x1c;
      t4 = t4 + 0x28;
      t1 = t1 + 4;
      t3 = t3 + 0x28;
      t2 = t2 + 1;
      o13 = o13 + 8;
      t0 = aux1;
    } while (t2 < 2);
    tA = &DAT_801b4ccc;
    if ((-1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99)))) && (0x20 < (int)_DAT_801f4f20)) {
      t1 = 0;
      slot = ap;
      t0 = -0x7fe0af50;
      aux = &DAT_801f51d8;
      tB = &DAT_801b4cd2;
      el = rectbl3 + 8;
      pkt = rectbl3;
      do {
        t2 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((*slot + 0xac)));
        if (-1 < t2) {
          *(uint16_t *)(aux + 4) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*slot + 0xac)));
          t2 = sprite_rect_set_b((int)guest_of(pkt),(int)*tA,(int)tB[-2],
                                mips_div_lo((int32_t)((int)tB[-1] * (*(short *)guest_at((uint32_t)(uintptr_t)((*slot + 0xb0) ))- t2)),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((*slot + 0xb0)))), (int)*tB);
          if (t2 == 0) {
            if (t1 == 0) {
              _DAT_801f50c0 = (short)el[-6] + -1;
              _DAT_801f50d0 = (short)el[-6] + -1;
              _DAT_801f5150 = (short)el[-6] + -1;
              _DAT_801f5160 = (short)el[-6] + -1;
            }
            else {
              _DAT_801f50e4 = (short)el[-6] + 1;
              _DAT_801f50f4 = (short)el[-6] + 1;
              _DAT_801f5174 = (short)el[-6] + 1;
              _DAT_801f5184 = (short)el[-6] + 1;
            }
          }
          else {
            t2 = (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 0x90 + t0;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x10) ))= el[-4];
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x20) ))= *el;
          }
          if (-1 < *(short *)guest_at((uint32_t)(uintptr_t)((*slot + 0xac)))) {
            *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x8000000;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(pkt) & 0xffffff;
          }
        }
        el = el + 9;
        pkt = pkt + 9;
        tB = tB + 4;
        tA = tA + 4;
        slot = slot + 1;
        t0 = t0 + 0x24;
        t1 = t1 + 1;
        aux = aux + 0x1c;
      } while (t1 < 2);
    }
    tA = &DAT_801b4ccc;
    pkt = rectbl2 + 0x18;
    if ((0x20 < (int)_DAT_801f4f20) && (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99))))) {
      t2 = 0;
      aux = &DAT_801f51d8;
      w0 = (uint32_t *)0x0;
      tB = &DAT_801b4cd2;
      rd = rectbl2 + 0x1c;
      t0 = aux1;
      t1 = aux0;
      el = rectbl3;
      do {
        t8 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1;
        if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t1 + 0x58) ))& 0x8000) == 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x5c) ))& 0x40) == 0)) {
          r0 = *(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xac)));
          *(uint16_t *)(aux + 6) = 1;
          *(int *)(aux + 0xc) = (int)r0;
        }
        w1 = w0;
        if (*(short *)(aux + 6) != 0) {
          t0 = *(int *)(aux + 8) + 2;
          if (*(int *)(aux + 0xc) < *(int *)(aux + 8)) {
            *(int *)(aux + 8) = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xac)));
          }
          else {
            *(int *)(aux + 8) = t0;
            if (*(int *)(aux + 0xc) <= t0) {
              *(int *)(aux + 8) = *(int *)(aux + 0xc);
              *(uint16_t *)(aux + 6) = 0;
            }
            o14 = (uint32_t *)(uintptr_t)w0;
            sprite_rect_set_a((int)guest_of(pkt),(int)*tA,(int)tB[-2],
                         mips_div_lo((int32_t)((int)tB[-1] *
                         ((int)*(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xb0) ))- *(int *)(aux + 8))),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xb0)))), (int)*tB); /* guest div @0x80174294 */
            w0[t8 * 0x30 + -0x1ff82c19] = rd[-1];
            w0[t8 * 0x30 + -0x1ff82c17] = rd[1];
            w1 = o14;
          }
        }
        *(short *)(rd + -2) = (short)el[4];
        *(short *)rd = (short)el[8];
        w0[t8 * 0x30 + -0x1ff82c1a] = rd[-2];
        w0[t8 * 0x30 + -0x1ff82c18] = *rd;
        if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xac)))) {
          *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x5000000;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(pkt) & 0xffffff;
        }
        rd = rd + 6;
        pkt = pkt + 6;
        tB = tB + 4;
        tA = tA + 4;
        el = el + 9;
        aux = aux + 0x1c;
        t2 = t2 + 1;
        w0 = w1 + 6;
        t0 = aux0;
        t1 = aux1;
      } while (t2 < 2);
    }
    tA = &DAT_801b4ccc;
    pkt = rectbl2 + 0x24;
    if ((0x20 < (int)_DAT_801f4f20) && (t0 = 0, -1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99))))) {
      aux = &DAT_801f51d8;
      t2 = 0;
      tB = &DAT_801b4cd2;
      w0 = rectbl2 + 0x25;
      el = rectbl2;
      t1 = aux0;
      do {
        t3 = (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 0xc0 + -0x7fe0b040 + t2;
        if (*(short *)(aux + 0x10) != *(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xaa)))) {
          *(uint16_t *)(aux + 0x10) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t1 + 0xaa)));
          o14 = (uint32_t *)(uintptr_t)el;
          sprite_rect_set_a((int)guest_of(pkt),(int)*tA,(int)tB[-2],
                       mips_div_lo((int32_t)((int)tB[-1] *
                       ((int)*(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xb0) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xaa))))),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xb0)))), (int)*tB); /* guest div @0x80174448 */
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 8) ))= w0[1];
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0xc) ))= w0[2];
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0x10) ))= w0[3];
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0x14) ))= w0[4];
          el = o14;
        }
        if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((t1 + 0xac)))) {
          *(short *)(w0 + 1) = (short)el[0x1b];
          *(short *)(w0 + 3) = (short)el[0x1d];
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 8) ))= w0[1];
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0x10) ))= w0[3];
          if ((short)w0[1] != (short)w0[2]) {
            if (((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t1 + 5) ))& 0x10) == 0) || ((_DAT_801f4f20 & 1) != 0)) {
              t8 = 0x28a06040;
            }
            else {
              t8 = 0x28ffdcdc;
            }
            *w0 = t8;
            *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x5000000;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(pkt) & 0xffffff;
          }
        }
        w0 = w0 + 6;
        pkt = pkt + 6;
        tB = tB + 4;
        tA = tA + 4;
        el = el + 6;
        aux = aux + 0x1c;
        t0 = t0 + 1;
        t2 = t2 + 0x18;
        t1 = aux1;
      } while (t0 < 2);
    }
    tA = &DAT_801b4cec;
    t1 = 0;
    tB = &DAT_801b4cf2;
    o10 = &DAT_801f51d8;
    o11 = 0;
    o12 = 0x618;
    el = rectbl + 0x186;
    pkt = rectbl3 + 0x12;
    w0 = rectbl + 0x18e;
    rd = rectbl3 + 0x1a;
    t0 = aux0;
    do {
      if ((0x1dff < *(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xd6)))) && ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t0 + 5) ))& 0x10) == 0)) {
        t8 = _DAT_801f4f20;
        if ((int)_DAT_801f4f20 < 0) {
          t8 = _DAT_801f4f20 + 3;
        }
        *(uint16_t *)((intptr_t)w0 + -0x12) = (uint16_t)((t8 & 0x1c) << 4) | 0x7a1d;
      }
      r0 = *(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xd6)));
      *(short *)(o10 + 0x12) = r0;
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xd6) ))< 0x1e00) || ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t0 + 5) ))& 0x10) != 0)) {
        t2 = (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 0x90 + -0x7fe0af08 + (o11 + t1) * 4;
        if ((*(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xd6) ))< 0x1e00) && ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t0 + 5) ))& 0x10) == 0)) {
          t4 = (int)tB[-1] * (int)*(short *)(o10 + 0x12);
          r0 = *tA;
          r1 = *tB;
          t5 = (int)tB[-2] + (uint32_t)(*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0');
          t3 = (int)((uint64_t)((int64_t)t4 * 0x88888889) >> 0x20);
        }
        else {
          t4 = (int)tB[-1] * (int)*(short *)(o10 + 0x12);
          t3 = (int)((uint64_t)((int64_t)t4 * 0x4445678f) >> 0x20);
          r0 = *tA;
          r1 = *tB;
          t5 = (int)tB[-2] + (uint32_t)(*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0');
        }
        sprite_rect_set_b((int)guest_of(pkt),(int)r0,t5,(t3 >> 0xc) - (t4 >> 0x1f),(int)r1);
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 8) ))= rd[-6];
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x10) ))= rd[-4];
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x18) ))= rd[-2];
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x20) ))= *rd;
      }
      else {
        t2 = *(int *)(&DAT_801f5210 + (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc) ))^ 1) * 4) + o12;
        sprite_rect_set_c((int)guest_of(el),(int)*tA,
                     (int)tB[-2] + (uint32_t)(*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0'),
                     ((int)tB[-1] * (int)r0) / 0x3bff,(int)*tB);
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 8) ))= w0[-6];
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x10) ))= w0[-4];
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x18) ))= w0[-2];
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x20) ))= *w0;
      }
      if (0x20 < (int)_DAT_801f4f20) {
        if ((*(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xd6) ))< 0x1e00) || ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t0 + 5) ))& 0x10) != 0)) {
          *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x8000000;
          t0 = *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8)));
          w1 = pkt;
        }
        else {
          *el = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
          t0 = *(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8)));
          w1 = el;
        }
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10) ))= guest_of(w1) & 0xffffff;
      }
      rd = rd + 9;
      pkt = pkt + 9;
      w0 = w0 + 10;
      el = el + 10;
      tB = tB + 4;
      tA = tA + 4;
      t1 = t1 + 1;
      o10 = o10 + 0x1c;
      o11 = o11 + 8;
      o12 = o12 + 0x28;
      t0 = aux1;
    } while (t1 < 2);
    t1 = 0;
    t0 = 0x7f8;
    slot = (int *)&DAT_801f4f28;
    pkt = rectbl;
    do {
      if (*slot != 0) {
        pkt[0x1fe] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= (guest_of(rectbl) + t0) & 0xffffff;
      }
      t0 = t0 + 0x28;
      pkt = pkt + 10;
      t1 = t1 + 1;
      slot = slot + 1;
    } while (t1 < 2);
    t2 = 0;
    t1 = 0x578;
    o4 = &DAT_801f51d8;
    o3 = 0;
    o5 = 0;
    o6 = (uint32_t *)&DAT_801f51d0;
    o7 = 0;
    o8 = (int *)&DAT_801f5218;
    o9 = 0;
    pkt = rectbl + 0x15e;
    t0 = aux0;
    do {
      r0 = *(short *)(o4 + 0x14);
      t3 = fighter_height_check(t0);
      if (r0 != t3) {
        if (*o8 != 0) {
          task_node_free((int *)(uintptr_t)*o8);
          *o8 = 0;
        }
        slot = (int *)(&DAT_801f51d0 + o3);
        if (*slot != 0) {
          task_node_free((int *)(uintptr_t)*slot);
          *slot = 0;
        }
        r0 = fighter_height_check(t0);
        t3 = o7;
        t0 = o5;
        *(short *)(o4 + 0x14) = r0;
        if (r0 == 1) {
          t5 = 0;
          t4 = 0x1b;
          if (t2 == 0) {
            t4 = 0x13;
          }
          t4 = t4 * 0x28;
          do {
            t5 = t5 + 1;
            t6 = t4 + _DAT_801f5210;
            t7 = t4 + _DAT_801f5214;
            t4 = t4 + 0x28;
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0xe) ))= 0x781e;
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t6 + 0xe) ))= 0x781e;
          } while (t5 < 8);
          t4 = 0;
          tA = (short *)((intptr_t)&DAT_801b4c30 + o5);
          slot = (int *)&DAT_801f5210;
          do {
            t4 = t4 + 1;
            sprite_rect_set_c(*slot + t3 + 0x578,(int)*tA,
                         (int)*(short *)((intptr_t)&DAT_801b4c32 + t0),
                         (int)*(short *)((intptr_t)&DAT_801b4c34 + t0),
                         (int)*(short *)((intptr_t)&DAT_801b4c36 + t0));
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t3 + *slot + 0x586) ))= 0x791d;
            slot = slot + 1;
          } while (t4 < 2);
          t9 = task_alloc_wrap_b(_DAT_801f5210 + t1,_DAT_801f5214 + t1,0, (uint32_t)br2_gaddr(&DAT_801b4d34),
                                 (uint32_t)br2_gaddr(&DAT_801b4d28));
          *o6 = t9;
        }
        else if (r0 < 2) {
          t3 = 0;
          if (r0 == 0) {
            t4 = 0x1b;
            if (t2 == 0) {
              t4 = 0x13;
            }
            t4 = t4 * 0x28;
            do {
              t3 = t3 + 1;
              t7 = t4 + _DAT_801f5210;
              t5 = t4 + _DAT_801f5214;
              t4 = t4 + 0x28;
              *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xe) ))= 0x791e;
              *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0xe) ))= 0x791e;
            } while (t3 < 8);
            t4 = 0;
            t3 = -0x7fe0af08;
            do {
              t5 = t3 + o9;
              t3 = t3 + 0x90;
              t4 = t4 + 1;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 4) ))= 0x20;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 5) ))= 0xf0;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 6) ))= 0xff;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xc) ))= 0x20;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xd) ))= 0xf0;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xe) ))= 0xff;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x14) ))= 0;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x15) ))= 0x40;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x16) ))= 0xc0;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1c) ))= 0;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1d) ))= 0x40;
              *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1e) ))= 0xc0;
            } while (t4 < 2);
            t3 = 0;
            tA = (short *)(&DAT_801b4c50 + o5);
            slot = (int *)&DAT_801f5210;
            do {
              t4 = *slot;
              slot = slot + 1;
              t3 = t3 + 1;
              sprite_rect_set_c(t4 + t1,(int)*tA,(int)*(short *)(&DAT_801b4c52 + t0),
                           (int)*(short *)(&DAT_801b4c54 + t0),
                           (int)*(short *)(&DAT_801b4c56 + t0));
            } while (t3 < 2);
            t0 = task_alloc_wrap_a(_DAT_801f5210 + t1 + 0xe,_DAT_801f5214 + t1 + 0xe,
                                  (uint32_t)br2_gaddr(&DAT_801b4d7c), (uint32_t)br2_gaddr(&DAT_801b4d90));
            *o8 = t0;
            t9 = task_alloc_wrap_b(_DAT_801f5210 + t1,_DAT_801f5214 + t1,0, (uint32_t)br2_gaddr(&DAT_801b4dac),
                                   (uint32_t)br2_gaddr(&DAT_801b4d98));
            *o6 = t9;
          }
        }
        else if (r0 == 2) {
          t4 = 0;
          t3 = 0x1b;
          if (t2 == 0) {
            t3 = 0x13;
          }
          t3 = t3 * 0x28;
          do {
            t4 = t4 + 1;
            t7 = t3 + _DAT_801f5210;
            t5 = t3 + _DAT_801f5214;
            t3 = t3 + 0x28;
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xe) ))= 0x795e;
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0xe) ))= 0x795e;
          } while (t4 < 8);
          t4 = 0;
          t3 = -0x7fe0af08;
          do {
            t5 = t3 + o9;
            t3 = t3 + 0x90;
            t4 = t4 + 1;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 4) ))= 0xff;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 5) ))= 0xf0;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 6) ))= 0x20;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xc) ))= 0xff;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xd) ))= 0xf0;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0xe) ))= 0x20;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x14) ))= 0xa0;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x15) ))= 0x60;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x16) ))= 0;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1c) ))= 0xa0;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1d) ))= 0x60;
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1e) ))= 0;
          } while (t4 < 2);
          t3 = 0;
          tA = (short *)(&DAT_801b4c40 + o5);
          slot = (int *)&DAT_801f5210;
          do {
            t4 = *slot;
            slot = slot + 1;
            t3 = t3 + 1;
            sprite_rect_set_c(t4 + t1,(int)*tA,(int)*(short *)(&DAT_801b4c42 + t0),
                         (int)*(short *)(&DAT_801b4c44 + t0),
                         (int)*(short *)(&DAT_801b4c46 + t0));
          } while (t3 < 2);
          t0 = task_alloc_wrap_a(_DAT_801f5210 + t1 + 0xe,_DAT_801f5214 + t1 + 0xe,
                                (uint32_t)br2_gaddr(&DAT_801b4d3c), (uint32_t)br2_gaddr(&DAT_801b4d50));
          *o8 = t0;
          t9 = task_alloc_wrap_b(_DAT_801f5210 + t1,_DAT_801f5214 + t1,0, (uint32_t)br2_gaddr(&DAT_801b4d6c),
                                (int)guest_of(&DAT_801b4d58));
          *o6 = t9;
        }
      }
      if ((*(short *)(o4 + 0x1a) < 100) && (*(int *)(&DAT_801f4f28 + o3) == 0)) {
        *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= guest_of(pkt) & 0xffffff;
      }
      pkt = pkt + 10;
      t1 = t1 + 0x28;
      t2 = t2 + 1;
      o3 = o3 + 4;
      o4 = o4 + 0x1c;
      o5 = o5 + 8;
      o6 = o6 + 1;
      o7 = o7 + 0x28;
      o8 = o8 + 1;
      o9 = o9 + 0x24;
      t0 = aux1;
    } while (t2 < 2);
    t0 = 0;
    pkt = rectbl + 0x1e;
    do {
      t0 = t0 + 1;
      *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(pkt) & 0xffffff;
      pkt = pkt + 10;
    } while (t0 < 0x10);
    t0 = 0;
    do {
      t0 = t0 + 1;
      *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x9000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= guest_of(pkt) & 0xffffff;
      pkt = pkt + 10;
    } while (t0 < 0x10);
    t1 = 0;
    t0 = 0x30;
    pkt = rectbl2;
    do {
      t1 = t1 + 1;
      *pkt = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x5000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(pkt) & 0xffffff;
      t8 = (intptr_t)rectbl2 + t0;
      t0 = t0 + 0x18;
      pkt[0xc] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))| 0x5000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x10) ))= t8 & 0xffffff;
      pkt = pkt + 6;
    } while (t1 < 2);
    t0 = 0;
    if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x99)))) {
      t2 = 0x6b8;
      aux = &DAT_801f51d8;
      t1 = aux1;
      do {
        if (*(uint16_t *)(aux + 0x16) != (uint16_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t1 + 0xe4)))) {
          t4 = 0;
          t3 = (intptr_t)rectbl + t2;
          if (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t1 + 0xe4) ))!= 0) {
            do {
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0xc) ))= DAT_801b4c28;
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0xd) ))= DAT_801b4c29;
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0x14) ))= DAT_801b4c28 + DAT_801b4c2a;
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0x15) ))= DAT_801b4c29;
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0x1c) ))= DAT_801b4c28;
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0x1d) ))= DAT_801b4c29 + DAT_801b4c2b;
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0x24) ))= DAT_801b4c28 + DAT_801b4c2a;
              t4 = t4 + 1;
              *(char *)guest_at((uint32_t)(uintptr_t)((t3 + 0x25) ))= DAT_801b4c29 + DAT_801b4c2b;
              t3 = t3 + 0x28;
            } while (t4 < (int)(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t1 + 0xe4))));
          }
          if (t4 < (int)(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4)))) {
            t1 = t4 * 0x28 + (intptr_t)rectbl + t2;
            do {
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0xc) ))= DAT_801b4c24;
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0xd) ))= DAT_801b4c25;
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0x14) ))= DAT_801b4c24 + DAT_801b4c26;
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0x15) ))= DAT_801b4c25;
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0x1c) ))= DAT_801b4c24;
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0x1d) ))= DAT_801b4c25 + DAT_801b4c27;
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0x24) ))= DAT_801b4c24 + DAT_801b4c26;
              *(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0x25) ))= DAT_801b4c25 + DAT_801b4c27;
              t4 = t4 + 1;
              t1 = t1 + 0x28;
            } while (t4 < (int)(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4))));
          }
        }
        t2 = t2 + 0xa0;
        t0 = t0 + 1;
        aux = aux + 0x1c;
        t1 = aux0;
      } while (t0 < 2);
      t0 = 0;
      pkt = rectbl + 0x1ae;
      do {
        t1 = 0;
        el = pkt;
        if (*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4) ))!= '\0') {
          do {
            *el = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= guest_of(el) & 0xffffff;
            t1 = t1 + 1;
            el = el + 10;
          } while (t1 < (int)(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc0) ))+ 4))));
        }
        t0 = t0 + 1;
        pkt = pkt + 0x28;
      } while (t0 < 2);
    }
    task_pools_tick(0);
    t8 = 0;
    o14 = (uint32_t *)0x801f5220;
    t4 = 0;
    t5 = 4;
    tC = &DAT_801b4e20;
    o1 = 0;
    o2 = (int *)&DAT_801f0940;
    o0 = rectbl;
    t0 = aux1;
    t1 = aux0;
    t2 = (intptr_t)o14;
    t3 = (intptr_t)o14;
    do {
      t7 = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t1 + 0xdd) ))+ *o2;
      if (t7 != 0) {
        r0 = *(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xac)));
        slot = (int *)((intptr_t)o14 + (t8 ^ 1) * 4);
        *slot = (int)r0;
        if (r0 < 0) {
          *slot = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t0 + 0xb0)));
        }
      }
      if (*(char *)guest_at((uint32_t)(uintptr_t)((t1 + 0xdd) ))== '\0') {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x30) ))= 0;
        *o2 = 0;
      }
      else if ((1 < t7) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x58) ))& 0x8000) != 0)) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x30) ))== 0) {
          t1 = 0;
          t0 = t4;
          do {
            *(uint16_t *)((intptr_t)o14 + t0 + 0x10) = *tC;
            t1 = t1 + 1;
            *(uint16_t *)((intptr_t)o14 + t0 + 0x1c) =
                 *(uint16_t *)((intptr_t)&DAT_801b4e20 + t5);
            t0 = t0 + 2;
          } while (t1 < 3);
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x2c) ))= 0x50;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28) ))= 2;
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x30) ))= 1;
        }
        t0 = 0;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28) ))< t7) {
          t1 = t4;
          do {
            *(uint16_t *)((intptr_t)o14 + t1 + 0x10) = *tC;
            t0 = t0 + 1;
            *(uint16_t *)((intptr_t)o14 + t1 + 0x1c) =
                 *(uint16_t *)((intptr_t)&DAT_801b4e20 + t5);
            t1 = t1 + 2;
          } while (t0 < 2);
          *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x2c) ))= 0x78;
          *(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28) ))= (short)t7;
        }
      }
      if (99 < *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28)))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28) ))= 99;
      }
      if (*(short *)guest_at((uint32_t)(uintptr_t)((fighter + 0x82) ))== 0) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x2c) ))!= 0) {
          *(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x2c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x2c) ))+ -1;
          goto LAB_8017544c;
        }
      }
      else {
LAB_8017544c:
        t1 = 0;
        t0 = t4;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x2c) ))!= 0) {
          do {
            t6 = (intptr_t)o14 + t0;
            t7 = *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1c) ))* 0xed8;
            *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x10) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x10) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1c)));
            if (t7 < 0) {
              t7 = t7 + 0xfff;
            }
            *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1c) ))= (short)(t7 >> 0xc);
            t1 = t1 + 1;
            t0 = t0 + 2;
          } while (t1 < 3);
          r0 = *(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28)));
          /* 0x801B4E10 / 0x801B4E14 are guest POINTER cells (lw in the guest):
           * each holds a guest address of a 1-char digit buffer. Deref the
           * u32 entry, then the pointed-to uint8_t — never the host ** value. */
          if (r0 < 10) {
            *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b4e10)) = 0x20;
          }
          else {
            *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b4e10)) =
                 (((char)((int)r0 / 10) + (char)(r0 >> 7)) - (char)(r0 >> 0xf)) + '0';
          }
          *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x801b4e14)) =
               (char)*(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28) ))+ (char)((int)*(short *)guest_at((uint32_t)(uintptr_t)((t2 + 0x28) ))/ 10) * -10 +
               '0';
          t9 = 0x5e;
          if (*(char *)guest_at((uint32_t)(uintptr_t)((fighter + 0x15) ))== '\0') {
            t9 = 0x2f;
          }
          announce_param_block_init_b(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                       (const void *)guest_at(*(uint32_t *)guest_at(0x801b4e10)),(int)*(short *)guest_at((uint32_t)(uintptr_t)((t3 + 0x10))),t9,1);
          announce_param_block_init_b(*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fighter + 0xc))),
                       (const void *)guest_at(*(uint32_t *)guest_at(0x801b4e14)),(int)((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0x12) ))+ 0x10) * 0x10000) >> 0x10
                       ,t9,1);
          ad = guest_of(rectbl) + (uint32_t)o1 + 0x668;
          sprite_rect_set_c(ad,*(short *)guest_at((uint32_t)(uintptr_t)((t3 + 0x14) ))+ 0x24,0x72,0x20,0x18);
          o0[0x19a] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))| 0x9000000;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((fighter + 8) ))+ 0x2038) ))= ad & 0xffffff;
        }
      }
      t3 = t3 + 6;
      t4 = t4 + 6;
      t2 = t2 + 2;
      t5 = t5 + 2;
      tC = tC + 1;
      t8 = t8 + 1;
      o0 = o0 + 10;
      o1 = o1 + 0x28;
      o2 = o2 + 1;
      t0 = aux0;
      t1 = aux1;
    } while ((int)t8 < 2);
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
extern void trig_lut_read(uint32_t ctrl_g,uint8_t *motion);
extern void trig_vel_flat(uint32_t ctrl_g,uint8_t *motion);
extern void trig_vel_up(uint32_t ctrl_g,uint8_t *motion);
extern void trig_vel_diag(uint32_t ctrl_g,uint8_t *motion);

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
extern uint32_t *g__DAT_1f80038c, *g__DAT_1f800390, *g__DAT_1f800394, *g__DAT_1f800398, *g__DAT_1f80039c, *g__DAT_1f8003a0, *g__DAT_1f8003a4, *g__DAT_1f8003a8, *g__DAT_1f8003ac, *g__DAT_1f8003b0, *g__DAT_1f8003b2, *g__DAT_1f8003b4, *g__DAT_1f8003b6, *g__DAT_1f8003b8, *g__DAT_1f8003ba, *g__DAT_1f8003bc, *g__DAT_1f8003be, *g__DAT_1f8003c0, *g__DAT_801c4578, *g__DAT_801f0780, *g__DAT_801f0784, *g__DAT_801f0788, *g__DAT_801f0790, *g__DAT_801f0794, *g__DAT_801f0798;
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

/* fighter_state_tick @0x8017fb34 — per-fighter state tick: sample pad edge flags,
 * update heading (ctrl pos vs state anim offset), advance timers and
 * dispatch the state machine for this controller. */
void fighter_state_tick(uint32_t *ctrl,uint32_t pad,int ctx)
{
  short tmr;
  uint16_t padf;
  int hdg;
  uint16_t afl;
  uint32_t st;
  uint32_t tv;

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
    tv = (uint32_t)(tv == 0);
  }
  if ((*ctrl & 0x80) == 0) {
    padf = attr_flag_mask(*(uint8_t *)((intptr_t)ctrl + 2));
  }
  if (*(uint8_t *)((intptr_t)ctrl + 1) == 0) {
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
      afl = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl[0xf] + 6)));
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
  padf = pad_hist_record(pad,*(uint8_t *)((intptr_t)ctrl + 2),afl);
  tv = ctrl[0x1f];
  *(uint16_t *)(ctrl + 0x1f) = padf;
  *(uint16_t *)((intptr_t)ctrl + 0x7e) = (uint16_t)tv;
  *(uint16_t *)((intptr_t)ctrl + 0x7a) = padf & (padf ^ (uint16_t)tv);
  tmr = *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1e)));
  if ((*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1e) ))!= 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1e) ))= tmr + -1, tmr == 1)) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x1c) ))= 0xffff;
  }
  tmr = *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36)));
  if ((*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36) ))!= 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36) ))= tmr + -1, tmr == 1)) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x34) ))= 0xffff;
  }
  tmr = *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x26)));
  if (((*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x26) ))!= 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x8000) == 0)) &&
     (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x26) ))= tmr + -1, tmr == 1)) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x24) ))= 0xffff;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x2a) ))= 0xffff;
  }
  if ((-1 < (int)ctrl[0x16]) && (*(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7c) ))!= *(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7d))))) {
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))= 0;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7c) ))= 0;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7e) ))= 0xff;
  }
  if ((ctrl[0x1e] & 0x880000) == 0x80000) {
    ctrl[0x18] = ctrl[0x18] | 0x40000;
    table_1f0ae0_clear(*(uint8_t *)((intptr_t)ctrl + 2));
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

/* fighter_atk_dir @0x8017f7c4 — attack-direction eligibility: returns 0 when the
 * attacker state/anim flags disallow a directional attack, else the
 * facing/direction code relative to the defender. */
uint32_t fighter_atk_dir(int atk,int def)
{
  int dist;
  uint32_t res;
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
  if (((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x7a) ))& 8) == 0) && ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x7c) ))& 8) != 0)) {
    return 0;
  }
  if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x10008000) != 0) &&
      ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) == 0)) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 100) ))& 0x8000) == 0)) {
    return 0;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x1040000) != 0) {
    return 0;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))& 0x40) != 0) {
    return 0;
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x100) != 0) {
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
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))& 0x80) == 0) {
      return 0;
    }
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x1000) == 0) {
      return 0;
    }
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x4000) != 0) {
      return 0;
    }
  }
  if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x80) != 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0x1800) != 0)) ||
     (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x58) ))& 0xa0000000) == 0xa0000000 ||
      ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))& 0x4000) == 0)))) {
    return 0;
  }
  if ((((*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd0) ))== 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x60) ))& 0x40000000) == 0)) &&
      ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x400000) != 0 &&
        ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x80080000) != 0x80080000)) ||
       (((-1 < *(int *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))|| ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 100) ))& 0x8000) != 0)) ||
        (0 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbc))))))))) &&
     (((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x76) ))<= *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x28) ))&&
       ((uint16_t)(*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x74) ))+ 0xaaU) < 0x155)) &&
      ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x1000) != 0 && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x400000) == 0))
       || (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x2000) != 0 && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x400000) != 0)
           ))))))) {
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x200) != 0) {
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) != 0) {
        return 0xffffffff;
      }
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 1) != 0) {
        return 1;
      }
    }
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x400) != 0) {
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) != 0) {
        return 0xffffffff;
      }
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x30) != 0) {
        return 1;
      }
    }
    if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x800) == 0) ||
       (res = 1, (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) == 0)) {
      res = 0xffffffff;
    }
    return res;
  }
  return 0xffffffff;

}

/* hit_pose_swap @0x8017eb10 — on a registered hit (f1+0x9c), toggle the hit flag
 * and swap attacker anim pose slots +0x23c<->+0x1c4 and +0x28c<->+0x264. */
void hit_pose_swap(int f0,int f1)
{
  uint16_t tmp;
  int anim;

  if (*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))!= '\0') {
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= 0;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x100) ))= *(uint8_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x100) ))^ 1;
    anim = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18)));
    tmp = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((anim + 0x23c)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((anim + 0x23c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((anim + 0x1c4)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x1c4) ))= tmp;
    anim = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18)));
    tmp = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((anim + 0x28c)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((anim + 0x28c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((anim + 0x264)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x264) ))= tmp;
  }
  return;

}

/* hit_elig_check @0x801458ac — both fighters must be in a hittable state: rejects
 * dead/invalid char slots and shield/invuln flag combos on f1, then
 * checks facing flag bits on f0+0x64 vs f1+0x58. */
uint32_t hit_elig_check(int f0,int f1)
{
  uint32_t fb;
  uint32_t fa;

  if ((((*(char *)guest_at((uint32_t)(uintptr_t)((f0 + 0xdc) ))== '\0') || (*(char *)guest_at((uint32_t)(uintptr_t)((f0 + 0x26) ))== -1)) ||
      (*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 0x27) ))== -1)) ||
     (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))& 4) != 0 || ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x80080000) != 0)))) {
    return 0;
  }
  fa = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 100)));
  fb = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58)));
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

/* hitbox_pair_test @0x80145970 — pairwise hitbox overlap test between two fighters;
 * writes per-box results into out[] and returns the aggregated verdict. */
uint32_t hitbox_pair_test(int f0,int f1,uint16_t *out)
{
  char op;
  uint32_t fl;
  int t;
  int hit;
  int cur;
  int rp;
  int k;
  int ht1;
  /* Guest stores the sign-extended hit-sphere center as 3 contiguous
   * words at sp+0x18/0x1c/0x20 and passes sp+0x18 to seg_sphere_hit,
   * which reads a3[0..2]. Ghidra split the vec3 into 3 scalars —
   * keep it one array (hp[i] == old local_5X). */
  int hp[3];
  int i;
  int nb;
  int md;
  uint32_t fl2;
  int lst;
  int ht;
  int off;
  uint16_t *dst;

  md = 0;
  fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58)));
  cur = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x14)));
  lst = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x10)));
  ht = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
  ht1 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
  if (((fl & 0x40000) != 0) ||
     ((((fl & 0x30000) != 0 && ((fl & 0x8000000) == 0)) &&
      ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x80000080) != 0x80)))) {
    md = 1;
  }
  i = 0;
  off = 0;
  nb = (int)*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 10)));
  fl2 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 100) ))& 0x1000;
  dst = out;
  do {
    op = *(char *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x14))))));
    if (op == -1) {
      return 0;
    }
    if ((op != -2) && ((md == 0 || (fl2 != 0)))) {
      t = *(char *)guest_at((uint32_t)(uintptr_t)((cur + 1) ))* 0x1c;
      hp[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0xe)));
      hp[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0x10)));
      hp[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0x12)));
      t = ht + *(char *)guest_at((uint32_t)(uintptr_t)((cur + 1) ))* 0x50 + 0x34;
      if (t != 0) {
        if (*(short *)guest_at((uint32_t)(uintptr_t)((cur + 2) ))== 1) {
          if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0x40080) == 0) {
            t = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x76)));
            if (t < 0) {
              t = -t;
            }
            if (t < 0x300) {
              *dst = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ht1 + 0x34)));
              dst[1] = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ht1 + 0x38)));
              dst[2] = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ht1 + 0x3c)));
              return 4;
            }
          }
        }
        else {
          k = 0;
          rp = lst;
          if (0 < nb) {
            do {
              hit = ht1 + *(char *)guest_at((uint32_t)(uintptr_t)((rp + 1) ))* 0x50 + 0x34;
              if (hit != 0) {
                if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0x800) == 0) {
                  hit = seg_sphere_hit((int)*(short *)guest_at((uint32_t)(uintptr_t)((cur + 2))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((rp + 2))),(int *)guest_at((uint32_t)(uintptr_t)(t)),
                                       hp,(int *)guest_at((uint32_t)(uintptr_t)(hit)),dst);
                }
                else {
                  hit = sphere_pt_hit((int)*(short *)guest_at((uint32_t)(uintptr_t)((cur + 2))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((rp + 2))),(int *)guest_at((uint32_t)(uintptr_t)(t)),
                                       (int *)guest_at((uint32_t)(uintptr_t)(hit)),dst);
                }
                if (hit != 0) {
                  return 1;
                }
              }
              k = k + 1;
              rp = rp + 4;
            } while (k < nb);
          }
        }
      }
    }
    off = off + 4;
    cur = cur + 4;
    i = i + 1;
    if (7 < i) {
      return 0;
    }
  } while( true );

}

/* fighter_range_code @0x80143bb8 — distance-band code from relative z separation:
 * returns 9/5 inside 0x155, 4/8 mid-range, ORs 2 beyond 0x4e3. */
uint32_t fighter_range_code(int f0,int f1)
{
  int rel;
  uint32_t code;

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

/* hit_scale_calc @0x80143c6c — compute the hit-reaction scale/launch power
 * from attacker/defender stats for hit_react_apply. */
int hit_scale_calc(int f0,int f1)
{
  uint32_t fa;
  int scale;

  fa = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 100)));
  scale = 0x80;
  if ((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))< 0) &&
     ((*(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0xba) ))!= 0 || (*(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0xb8) ))!= 0)))) {
    scale = 0x99;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))| 0x8000000;
  }
  if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x20) != 0) && ((fa & 0x40) != 0)) &&
     ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x10) == 0)) {
    return scale << 1;
  }
  if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x68) ))& 2) == 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 8) == 0)) &&
     (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))!= 0x402)) {
    return scale;
  }
  return scale * 0x99 >> 7;

}

/* hit_react_apply @0x80143d38 — apply a resolved hit: damage/guard checks,
 * launch velocity, hit-flash, sound/anim triggers and flag updates on
 * attacker and defender. */
uint32_t hit_react_apply(int atk,int def,int scale,short *hits,int n)
{
  char cnt;
  short t9;
  uint16_t t10;
  short t8;
  int t0;
  uint32_t eb;
  int t6;
  uint32_t t4;
  uint32_t dc;
  int t7;
  int pw0;
  uint32_t df;
  uint32_t t5;
  uint32_t af;
  uint32_t rc;

  *(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x26) ))= 0xff;
  df = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x68)));
  af = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 100)));
  if (n == 4) {
    t0 = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))* (0x300 - *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x76))));
    if (t0 < 0) {
      t0 = t0 + 0x1ff;
    }
    *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))= (char)(t0 >> 9);
    if (0x13 < (t0 >> 9 & 0xffU)) {
      af = af | 0x1000000;
    }
  }
  t0 = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))* scale;
  if (t0 < 0) {
    t0 = t0 + 0x7f;
  }
  scale = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xdc) ))* scale;
  pw0 = t0 >> 7;
  if (scale < 0) {
    scale = scale + 0x7f;
  }
  t5 = scale >> 7;
  if (n == 2) {
    debug_strike_fx_a(hits,0,0,t5,atk);
    return 1;
  }
  if (n == 3) {
    debug_strike_fx_b(hits,0,0,t5,atk);
    return 2;
  }
  eb = 0x100000;
  if ((((af & 0x40) != 0) || (eb = 0x400000, (af & 0x100) != 0)) ||
     (eb = 0x200000, (af & 0x80) != 0)) {
    df = df | eb;
  }
  eb = 0;
  if ((af & 0x4000) == 0) {
    dc = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c)));
    if ((dc & 1) != 0) {
      if ((af & 0x40) == 0) {
        if ((af & 0x80) == 0) {
          if ((af & 0x100) != 0) {
            if ((dc & 0x80) == 0) goto LAB_80143f44;
            if ((dc & 0x40000000) == 0) {
              eb = 0x42000000;
            }
          }
        }
        else if ((dc & 0x80) == 0) {
LAB_80143f44:
          dc = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 1;
          if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x20) == 0) goto LAB_80143f58;
        }
        else if ((dc & 0x40000000) == 0) {
          eb = 0x42000000;
        }
      }
      else if ((dc & 0x80) == 0) {
        t4 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58)));
        if ((t4 & 0x10) == 0) {
          if (((t4 & 0x20) == 0) && (dc = af & 0x80, (t4 & 1) != 0)) {
LAB_80143f58:
            if (dc != 0) goto LAB_80143f60;
          }
        }
        else {
LAB_80143f60:
          eb = 0x42000000;
        }
      }
      else if ((dc & 0x40000000) == 0) {
        eb = 0x42000000;
      }
      if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 2) != 0) && ((eb & 0x2000000) != 0)) {
        eb = eb | 4;
      }
    }
  }
  if (((af & 0x20000) != 0) && ((eb & 4) == 0)) {
    if ((eb & 0x2000000) == 0) {
      df = df | 0x800400;
    }
    else {
      pw0 = (pw0 - (t0 >> 0x1f)) >> 1;
    }
    eb = eb & 0xbdffffff;
  }
  if (eb != 0) {
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))=
         (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x74)))) * 0x10) >> 4;
    if (*(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))== '\0') {
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))= 0x40;
    }
    cnt = '\x01';
    if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000000) == 0) ||
       (cnt = *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))+ '\x01', *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))!= -1)) {
      *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))= cnt;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)t5 * 0x60 + 0x800;
    if ((af & 0x20000) != 0) {
      if (((int)_DAT_801f0292 + (int)*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e)))) * 0x100000 < 0) {
        _DAT_801f02f6 = (uint16_t)((int)(t5 * -3) / 2);
      }
      else {
        _DAT_801f02f6 = (uint16_t)((int)(t5 * 3) / 2);
      }
    }
    _DAT_801f02f8 = 4;
    if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) != 0) {
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))= (short)(-pw0 / 10);
    }
    if ((eb & 4) == 0) {
      t0 = ((int)(t5 - (scale >> 0x1f)) >> 1) + 5;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= (short)t0;
      if ((*(char *)guest_at((uint32_t)(uintptr_t)((def + 1) ))== '\0') && ((DAT_801b55d0 & 1) != 0)) {
        t0 = t0 * 0x10000;
        (&DAT_801f596d)[(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] =
             (char)(((t0 >> 0x10) - (t0 >> 0x1f)) >> 1);
        (&DAT_801f5971)[(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] = 1;
      }
      t10 = 10;
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))< 10) || (t10 = 0x18, 0x18 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe))))) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= t10;
      }
      t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))+ -2;
    }
    else {
      t0 = t5 * 3;
      if (t0 < 0) {
        t0 = t0 + 3;
      }
      t0 = (t0 >> 2) + 6;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= (short)t0;
      if ((*(char *)guest_at((uint32_t)(uintptr_t)((def + 1) ))== '\0') && ((DAT_801b55d0 & 1) != 0)) {
        t0 = t0 * 0x10000;
        (&DAT_801f596d)[(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] =
             (char)(((t0 >> 0x10) - (t0 >> 0x1f)) >> 1);
        (&DAT_801f5971)[(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((def + 2) ))* 2] = 1;
      }
      t10 = 0xd;
      if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))< 0xd) {
LAB_80144ac4:
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= t10;
      }
      else if (0x78 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe)))) {
        t10 = 0x78;
        goto LAB_80144ac4;
      }
      t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))+ -0xc;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xbe) ))= t8;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xcc) ))= 0;
    if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((def + 5) ))& 0x10) == 0) {
      t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+ (uint16_t)(*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))>> 3) * 0x3c;
    }
    else {
      t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+ (uint16_t)(*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))>> 3) * -0x3c;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))= t8;
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x800) != 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= 0xfd60;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0x92) ))= 0xe0;
      t8 = (short)mips_div_lo((int32_t)(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))* 1000),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 0x24)))); /* guest div @0x80144C14 */
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= t8;
      if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) != 0) {
        *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= (short)((t8 * 3) / 2);
      }
    }
    df = df & 0xfffffcbf;
    rc = 2;
    if ((int)t5 < 0x1a) {
      if ((int)t5 < 0x15) {
        df = df | 0x40;
      }
      else {
        df = df | 0x100;
      }
    }
    else {
      df = df | 0x200;
    }
    if ((df & 0x800000) == 0) {
      xform_accum_spawn(2,t5);
      debug_strike_fx_a(hits,(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))+ 0x400) * 0x100000 >> 0x14,
                   (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))<< 4,t5 | 0x1000,atk);
      dc = 0x20;
    }
    else {
      xform_accum_spawn(0,t5);
      noop_fn(0x100000c,hits,0,200,0xf,1);
      dc = 0x20;
    }
    goto LAB_80144d0c;
  }
  cnt = '\x01';
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x20000000) == 0) ||
     (cnt = *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))+ '\x01', *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))!= -1)) {
    *(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd) ))= cnt;
  }
  t0 = pw0 >> 0x1f;
  switch(*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xdd)))) {
  case 0:
  case 1:
    goto switchD_8014402c_caseD_0;
  case 2:
    t6 = t5 * 9;
    t0 = (int)((uint64_t)((int64_t)t6 * 0x66666667) >> 0x20);
    t7 = pw0 << 3;
    goto LAB_80144130;
  case 3:
    t6 = t5 << 3;
    t0 = (int)((uint64_t)((int64_t)t6 * 0x66666667) >> 0x20);
    t7 = pw0 << 3;
    break;
  case 4:
    t6 = t5 * 7;
    t0 = (int)((uint64_t)((int64_t)t6 * 0x66666667) >> 0x20);
    t7 = pw0 * 7;
    break;
  case 5:
    t5 = (int)(t5 * 6) / 10;
    t0 = pw0 * 6 >> 0x1f;
    pw0 = (pw0 * 6) / 10 + t0;
    goto LAB_801441c4;
  case 6:
    t5 = (int)(t5 - (scale >> 0x1f)) >> 1;
    pw0 = pw0 / 2;
    goto switchD_8014402c_caseD_0;
  case 7:
    t6 = t5 << 2;
    t0 = (int)((uint64_t)((int64_t)t6 * 0x66666667) >> 0x20);
    t7 = pw0 << 2;
    break;
  case 8:
    t6 = t5 * 3;
    t0 = (int)((uint64_t)((int64_t)t6 * 0x66666667) >> 0x20);
    t7 = pw0 << 1;
LAB_80144130:
    t7 = t7 + pw0;
    break;
  case 9:
    t5 = (int)t5 / 5;
    pw0 = pw0 / 5 + t0;
    goto LAB_801441c4;
  default:
    t5 = (int)t5 / 10;
    pw0 = pw0 / 10 + t0;
LAB_801441c4:
    pw0 = pw0 - t0;
    goto switchD_8014402c_caseD_0;
  }
  t5 = (t0 >> 2) - (t6 >> 0x1f);
  pw0 = t7 / 10;
switchD_8014402c_caseD_0:
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)t5 * 0x60 + 0x800;
  eb = 0x24000000;
  if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) == 0) {
    t8 = *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xfe)));
  }
  else {
    t8 = *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x100)));
  }
  pw0 = pw0 * t8;
  if (pw0 < 0) {
    pw0 = pw0 + 0x7f;
  }
  if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((def + 5) ))& 0x10) == 0) {
    t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x102)));
  }
  else {
    t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x104)));
  }
  t0 = (pw0 >> 7) * (int)t8;
  if (t0 < 0) {
    t0 = t0 + 0x7f;
  }
  pw0 = t0 >> 7;
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x20000000) == 0) || ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x100) == 0))
  {
    if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x800) == 0) &&
       (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x8030000) != 0 && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 100) ))& 0x1000) == 0))
       )) {
      t0 = pw0 * 3;
      pw0 = t0 >> 2;
      if (t0 < 0) {
        pw0 = (t0 + 3) >> 2;
      }
    }
  }
  else {
    pw0 = (pw0 - (t0 >> 0x1f)) >> 1;
  }
  if (t5 == 0) {
    t5 = 1;
  }
  if (pw0 == 0) {
    pw0 = 1;
  }
  if (((int)_DAT_801f0292 + (int)*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e)))) * 0x100000 < 0) {
    t0 = t5 * -3;
  }
  else {
    t0 = t5 * 3;
  }
  if (t0 < 0) {
    t0 = t0 + 3;
  }
  _DAT_801f02f6 = (uint16_t)(t0 >> 2);
  df = df & 0xfffffcbf;
  _DAT_801f02f8 = 4;
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))= (short)(t5 + 6);
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))= (short)pw0;
  if ((int)((t5 + 6) * 0x10000) >> 0x10 < 0x10) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))= 0x10;
  }
  t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2)));
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xcc) ))= (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))+ -5) * 0x10;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))< 0x1b) {
    t9 = t8 + -0x10;
    if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))< 0x16) {
      df = df | 0x40;
    }
    else {
      df = df | 0x100;
      t9 = t8 + -0x15;
    }
  }
  else {
    df = df | 0x200;
    t9 = t8 + -0x1a;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc2) ))= t9;
  dc = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58)));
  if ((dc & 0x800) != 0) {
LAB_801443f4:
    if ((af & 0x80000) == 0) {
      t4 = 0x840000;
      if (((af & 0x4000000) == 0) && (t4 = 0x810000, (af & 0x1000000) == 0)) {
        if ((af & 0x200000) == 0) {
          if ((af & 0x100000) == 0) {
            if ((af & 0x400000) == 0) {
              t4 = 0x804400;
            }
            else {
              t4 = 0x804400;
            }
          }
          else {
            t4 = 0x804400;
          }
        }
        else {
          t4 = 0x802400;
        }
      }
      else {
LAB_80144748:
        t4 = t4 | 0x400;
      }
    }
    else if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x80) == 0) {
      t4 = 0x800c00;
    }
    else {
      t4 = 0x804400;
    }
    goto LAB_8014474c;
  }
  t4 = 0x2000000;
  if ((dc & 0x8030000) != 0) goto LAB_80144748;
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x40000) != 0) goto LAB_801443f4;
  if ((dc & 0x100) == 0) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0x92) ))= 0;
    if ((af & 0x80000) != 0) {
      if ((af & 0x10000) == 0) {
        if (((df & 0x8000000) != 0) || ((df & 0x800000) != 0)) {
LAB_801445d8:
          df = df | 0xc00;
          goto LAB_80144754;
        }
        t4 = 0x804400;
      }
      else {
        if ((df & 0x8000000) == 0) goto LAB_80144754;
        if ((df & 0x800000) != 0) goto LAB_801445d8;
        t4 = 0x804400;
      }
      goto LAB_8014474c;
    }
    if ((af & 0x400000) == 0) {
      if ((af & 0x200000) == 0) {
        if ((af & 0x1000000) == 0) {
          if ((af & 0x4000000) == 0) {
            if ((af & 0x100000) == 0) {
              if ((af & 0x800000) == 0) {
                if ((((af & 0x1000) != 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x58) ))& 0x30000) != 0)) &&
                   (t4 = 0x2000000, (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0x80) == 0)) goto LAB_80144748;
              }
              else if (((af & 0x10000) == 0) || ((df & 0x8000000) != 0)) {
                df = df | 0x8400;
              }
            }
            else if (((af & 0x10000) == 0) || ((df & 0x8000000) != 0)) {
              df = df | 0x1400;
            }
          }
          else {
            if ((af & 0x10000) == 0) {
              t4 = 0x840000;
              if ((df & 0x8000000) == 0) {
                t4 = 0x40000;
              }
              goto LAB_80144748;
            }
            if ((df & 0x8000000) != 0) {
              t4 = 0x40400;
              goto LAB_8014474c;
            }
          }
        }
        else {
          if ((af & 0x10000) == 0) {
            t4 = 0x810000;
            if ((df & 0x8000000) == 0) {
              t4 = 0x10000;
            }
            goto LAB_80144748;
          }
          if ((df & 0x8000000) != 0) {
            t4 = 0x10400;
            goto LAB_8014474c;
          }
        }
      }
      else {
        if ((af & 0x10000) == 0) {
          if ((df & 0x8000000) != 0) {
            t4 = 0x802400;
            goto LAB_8014474c;
          }
        }
        else if ((df & 0x8000000) == 0) goto LAB_80144754;
        df = df | 0x2400;
      }
    }
    else {
      if ((af & 0x10000) == 0) {
        if ((df & 0x8000000) != 0) {
          if ((df & 0x800000) != 0) goto LAB_801445d8;
          t4 = 0x804400;
          goto LAB_8014474c;
        }
      }
      else if ((df & 0x8000000) == 0) goto LAB_80144754;
      df = df | 0x4400;
    }
  }
  else {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0x90) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0x92) ))= 0;
    if ((af & 0x80000) == 0) {
      if ((af & 0x400000) == 0) {
        if ((af & 0x200000) == 0) {
          t4 = 0x810000;
          if (((af & 0x1000000) != 0) || (t4 = 0x840000, (af & 0x4000000) != 0))
          goto LAB_80144748;
          if ((af & 0x100000) == 0) goto LAB_80144754;
          t4 = 0x804400;
        }
        else {
          t4 = 0x802400;
        }
      }
      else {
        t4 = 0x804400;
      }
    }
    else {
      t4 = 0x800c00;
    }
LAB_8014474c:
    df = df | t4;
  }
LAB_80144754:
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))=
       (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x74)))) * 0x10) >> 4;
  if ((df & 0x800400) == 0x800400) {
    if (*(char *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))== '\0') {
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))= 0x40;
    }
    else {
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))=
           (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xc6)))) * 0x10) >> 4;
    }
  }
  pw0 = (uint32_t)*(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))<< 0x10;
  t0 = pw0 >> 0x10;
  if (-1 < t0) {
    if ((df & 0x2000000) == 0) {
      t0 = (uint32_t)*(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))+ t5 * 0x10;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))= (short)t0;
      t10 = 0x12c0;
      if (t0 * 0x10000 >> 0x10 < 0x12c1) goto LAB_80144804;
    }
    else {
      t10 = (uint16_t)((t0 - (pw0 >> 0x1f)) >> 1);
    }
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xc0) ))= t10;
  }
LAB_80144804:
  if (((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 5) ))& 0x10) == 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x6c) ))& 0x8000) == 0)) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xd8) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xd8) ))+ (uint16_t)(*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xda) ))>> 1) * 0x3c;
  }
  if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((def + 5) ))& 0x10) == 0) {
    t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+
            (short)((uint32_t)(((int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))/ 3) * 0x10000) >> 0x10) * 0x3c;
  }
  else {
    t8 = *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((def + 200) ))* -0x78;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xd8) ))= t8;
  rc = 3;
  xform_accum_spawn(3,t5);
  dc = 8;
  debug_strike_fx_b(hits,(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xc4) ))+ -0x400) * 0x100000 >> 0x14,
               (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xdb) ))<< 4,t5 | 0x1000,atk);
LAB_80144d0c:
  if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))!= 0) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xb4) ))= 0x68;
  }
  if (((df & 0x800) == 0) && ((df & 0x804000) != 0x804000)) {
    if ((df & 0x810000) == 0x810000) {
      trig_vel_flat(atk,(uint8_t *)guest_at(def));
    }
    else if ((df & 0x802000) == 0x802000) {
      trig_vel_up(atk,(uint8_t *)guest_at(def));
    }
    else if ((df & 0x840000) == 0x840000) {
      trig_vel_diag(atk,(uint8_t *)guest_at(def));
    }
  }
  else {
    trig_lut_read(atk,(uint8_t *)guest_at(def));
  }
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x54) ))& 0x20) != 0) {
    t0 = (uint32_t)*(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))<< 0x10;
    pw0 = (uint32_t)*(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0x98) ))<< 0x10;
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)(((t0 >> 0x10) - (t0 >> 0x1f)) >> 1);
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0x98) ))= (short)(((pw0 >> 0x10) - (pw0 >> 0x1f)) >> 1);
  }
  if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))!= 0) {
    if (*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))< 0) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= 0;
    }
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x6c) ))& 0xf0) != 0) {
      *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xc4) ))=
           (short)((*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x4e) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x70)))) * 0x10) >> 4;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((atk + 0xb4) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xb4)));
      t0 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))<< 1;
      *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xb2) ))=
           (short)((uint64_t)((int64_t)t0 * 0x55555556) >> 0x20) - (short)(t0 >> 0x1f);
      t0 = (uint32_t)*(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))<< 0x10;
      *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= (short)(((t0 >> 0x10) - (t0 >> 0x1f)) >> 1);
      *(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xb2) ))=
           (short)mips_div_lo((int32_t)(*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0xb2) ))* 500),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((atk + 0x24)))); /* guest div @0x80144EB0 */
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))=
         (short)mips_div_lo((int32_t)(*(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))* 500),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((def + 0x24)))); /* guest div @0x80144EE0 */
    if (0x6000 < *(short *)guest_at((uint32_t)(uintptr_t)((def + 0xb2)))) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= 0x6000;
    }
  }
  if (((af & 0x20000000) != 0) && ((eb & 0x4000000) != 0)) {
    df = df | 0x80000;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((def + 0xb2) ))= 0;
  }
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x68) ))= df;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((def + 0x5c) ))& 0xf9ffffff | eb;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((atk + 0x5c) ))| dc;
  return rc;

}

/* hitbox_wall_collide @0x8014642c — hitbox vs stage wall collision resolve: push
 * out, kill velocity components, set launch/knock flags on f0, write the
 * per-side pen channels 0x801f04ec-0x4f2, and return the collision mask. */
uint32_t hitbox_wall_collide(int f0,uint16_t *hits,int battle)
{
  bool en_n0;
  bool en_p0;
  bool en_n1;
  bool tb;
  bool en_p1;
  int t6;
  int d0;
  uint32_t sel;
  int t9;
  uint32_t flg;
  int rad;
  int t12;
  uint32_t wh;
  uint32_t near;
  int t15;
  int pen0;
  int pen1;

  pen0 = 0;
  pen1 = 0;
  wh = 0;
  near = 0;
  en_p1 = true;
  en_n1 = true;
  en_p0 = true;
  en_n0 = true;
  t9 = *(char *)guest_at((uint32_t)(uintptr_t)((battle + 0x98) ))* 8;
  flg = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58)));
  if ((flg & 0x100000) == 0) {
    if (((flg & 0x8000) == 0) &&
       (((-1 < (int)flg || ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 100) ))& 0x1000) == 0)) ||
        (*(uint8_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x2c) )))< 0x81)))) {
      if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x20000000) != 0) ||
         ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x80000) != 0)) {
        en_p1 = false;
        en_n1 = false;
        en_p0 = false;
        en_n0 = false;
      }
    }
    else {
      en_n0 = _DAT_801f04e4 != 0;
      en_p0 = _DAT_801f04e6 != 0;
      en_n1 = _DAT_801f04e8 != 0;
      en_p1 = true;
      if (_DAT_801f04ea == 0) {
        en_p1 = false;
      }
    }
  }
  t15 = 1;
  t12 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0xc)));
  if (1 < *(char *)guest_at((uint32_t)(uintptr_t)((f0 + 9)))) {
    do {
      t6 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ *(char *)guest_at((uint32_t)(uintptr_t)((t12 + 5) ))* 0x50;
      d0 = *(int *)guest_at((uint32_t)(uintptr_t)((t6 + 0x34)));
      rad = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t12 + 6)));
      if (d0 < 0) {
        d0 = -(d0 - rad) - (int)*(short *)(&DAT_801b1590 + t9);
        if (-0x40 < d0) {
          near = near | 2;
        }
        if ((0 < d0) && (tb = pen0 < d0, en_p0)) {
          wh = wh | 2;
          goto LAB_8014660c;
        }
      }
      else {
        d0 = (int)*(short *)(&DAT_801b1590 + t9) - (d0 + rad);
        if (d0 < 0x40) {
          near = near | 1;
        }
        if ((d0 < 0) && (tb = d0 < pen0, en_n0)) {
          wh = wh | 1;
LAB_8014660c:
          if (tb) {
            pen0 = d0;
          }
        }
      }
      t6 = *(int *)guest_at((uint32_t)(uintptr_t)((t6 + 0x3c)));
      if (t6 < 0) {
        t6 = -(t6 - rad) - (int)*(short *)(&DAT_801b1592 + t9);
        if (-0x40 < t6) {
          near = near | 8;
        }
        if ((0 < t6) && (tb = pen1 < t6, en_p1)) {
          wh = wh | 8;
          goto LAB_80146684;
        }
      }
      else {
        t6 = (int)*(short *)(&DAT_801b1592 + t9) - (t6 + rad);
        if (t6 < 0x40) {
          near = near | 4;
        }
        if ((t6 < 0) && (tb = t6 < pen1, en_n1)) {
          wh = wh | 4;
LAB_80146684:
          if (tb) {
            pen1 = t6;
          }
        }
      }
      t15 = t15 + 1;
      t12 = t12 + 4;
    } while (t15 < *(char *)guest_at((uint32_t)(uintptr_t)((f0 + 9))));
  }
  t9 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
  t12 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
  flg = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xfeffffff;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0xffffff00;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= flg;
  if (wh == 0) goto LAB_8014672c;
  t15 = t9;
  if (t9 < 0) {
    t15 = -t9;
  }
  t6 = t12;
  if (t12 < 0) {
    t6 = -t12;
  }
  if (t15 < t6) {
    sel = wh & 4;
    if (t12 < 1) {
      sel = wh & 8;
      t9 = t12;
      goto joined_r0x80146714;
    }
  }
  else {
    sel = wh & 1;
    if (t9 < 1) {
      sel = wh & 2;
joined_r0x80146714:
      if (-1 < t9) goto LAB_8014672c;
    }
  }
  if (sel != 0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= flg | 0x1000000;
  }
LAB_8014672c:
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x20000000) == 0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))| wh | near << 4;
  }
  *hits = 0;
  hits[2] = 0;
  if (wh == 0) {
    wh = 0;
  }
  else {
    *hits = (short)pen0;
    if (pen0 < 0) {
      pen0 = -pen0;
    }
    hits[2] = (short)pen1;
    if (pen1 < 0) {
      pen1 = -pen1;
    }
    if (0x80 < pen0) {
      pen0 = 0x80;
    }
    if (0x80 < pen1) {
      pen1 = 0x80;
    }
    if (((wh & 1) != 0) && (_DAT_801f04f4 == 0)) {
      _DAT_801f04ec = (uint16_t)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* pen0) / 0x500);
    }
    if (((wh & 2) != 0) && (_DAT_801f04f6 == 0)) {
      _DAT_801f04ee = (uint16_t)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* pen0) / 0x500);
    }
    if (((wh & 4) != 0) && (_DAT_801f04f8 == 0)) {
      _DAT_801f04f0 = (uint16_t)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* pen1) / 0x500);
    }
    if (((wh & 8) != 0) && (_DAT_801f04fa == 0)) {
      _DAT_801f04f2 = (uint16_t)((*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24) ))* pen1) / 0x500);
    }
  }
  return wh;

}

/* camera_aim_offset @0x80145d84 — compute camera aim offset angles from the fighter
 * pair midpoint delta; writes aim angles to f0/f1 +0x76/+0x78 and clears
 * flag bits in +0x5c. (The 0x801f04ec-0x4f2 pen channels belong to
 * hitbox_wall_collide.) */
void camera_aim_offset(int f0,int f1,uint32_t ang)
{
  bool rev;
  short ad;
  int sep;
  int t4;
  uint32_t t5;
  int q;
  int c1;
  uint32_t k;
  int p0;
  int e0;
  uint32_t i;
  /* Guest vec3 slots: rtps_pt_xform / FUN_80194660 (vec3_s16_normalize)
   * and gte_rtps_xform1 all read/write THREE contiguous s16 through
   * dlt / tb. The decompiler split each 6-uint8_t vec3 into
   * scalars + a 2-short array, so index-2 writes overflowed the array
   * (same host-stack-smash class as fighter_pair_update's l1).
   * dlt[0..2] = old dlt/local_6e/local_6c;
   * tb[0..2] = old tb[0..1]/local_64. */
  short dlt [3];
  short tb [3];
  uint8_t rt [32];
  int na;
  int nb;
  int l1;
  int n0;
  int n1;

  rotmat_rtps_build(ang,rt);
  dlt[0] = (short)(((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x10) ))+
                      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x60)))) -
                     (*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x10) ))+
                     *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x60))))) / 2);
  dlt[1] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 100) ))- *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 100)))
  ;
  dlt[2] = (short)(((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x18) ))+
                      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x18) ))+ 0x68)))) -
                     (*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x18) ))+
                     *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x18) ))+ 0x68))))) / 2);
  gte_rtps_xform1(rt,dlt,dlt);
  ad = dlt[2];
  if (dlt[2] < 0) {
    ad = -dlt[2];
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x76) ))= ad;
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x76) ))= ad;
  *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x78) ))= dlt[1];
  *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x78) ))= -dlt[1];
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0xff7fffff;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))& 0xff7fffff;
  sep = (int)dlt[2];
  if (sep < 0) {
    sep = -sep;
  }
  if (sep < 0x181) {
    sep = (int)dlt[1];
    if (sep < 0) {
      sep = -sep;
    }
    if ((((sep < 0x141) && (n0 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))), *(int *)guest_at((uint32_t)(uintptr_t)((n0 + 0x40) ))== 0))
        && (n1 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))), *(int *)guest_at((uint32_t)(uintptr_t)((n1 + 0x40) ))== 0)) &&
       (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x60) ))& 0x200000) == 0 &&
        (sep = 0, (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x60) ))& 0x200000) == 0)))) {
      rev = false;
      l1 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0xc)));
      if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x30000) != 0) &&
          ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x58) ))& 0x8000000) == 0)) ||
         (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x30000) != 0 &&
          ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x58) ))& 0x8000000) == 0)))) {
        rev = true;
      }
      rev = !rev;
      if (rev) {
        na = (int)*(char *)guest_at((uint32_t)(uintptr_t)((f0 + 9)));
        nb = (int)*(char *)guest_at((uint32_t)(uintptr_t)((f1 + 9)));
      }
      else {
        nb = 1;
        na = 1;
      }
      i = (uint32_t)rev;
      t5 = (uint32_t)rev;
      if ((int)i < na) {
        e0 = i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0xc)));
        do {
          p0 = n0 + *(char *)guest_at((uint32_t)(uintptr_t)((e0 + 1) ))* 0x50 + 0x34;
          if ((int)t5 < nb) {
            c1 = t5 * 4 + l1;
            k = t5;
            do {
              q = n1 + *(char *)guest_at((uint32_t)(uintptr_t)((c1 + 1) ))* 0x50 + 0x34;
              t4 = pt_box_radius_test((int)*(short *)guest_at((uint32_t)(uintptr_t)((e0 + 2))),(int)*(short *)guest_at((uint32_t)(uintptr_t)((c1 + 2))),(int *)guest_at((uint32_t)(uintptr_t)(p0)),
                                   (int *)guest_at((uint32_t)(uintptr_t)(q)));
              if (t4 != 0) {
                rtps_pt_xform((uint16_t *)guest_at((uint32_t)(uintptr_t)(p0)),dlt);
                rtps_pt_xform((uint16_t *)guest_at((uint32_t)(uintptr_t)(q)),tb);
                t4 = ((int)tb[2] + (int)*(short *)guest_at((uint32_t)(uintptr_t)((c1 + 2)))) -
                        ((int)dlt[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((e0 + 2))));
                if (sep < t4) {
                  sep = t4;
                }
              }
              k = k + 1;
              c1 = c1 + 4;
            } while ((int)k < nb);
          }
          i = i + 1;
          e0 = e0 + 4;
        } while ((int)i < na);
      }
      if (sep != 0) {
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))| 0x800000;
        t5 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c)));
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x5c) ))= t5 | 0x800000;
        if (200 < sep) {
          sep = 200;
        }
        sep = sep * 3;
        if (sep < 0) {
          sep = sep + 3;
        }
        i = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x5c) ))& 0x8000;
        if ((i == 0) && ((t5 & 0x8000) == 0)) {
          k = (uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x24)));
          e0 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x24)));
          i = k + e0;
        }
        else {
          e0 = 0;
          k = (uint32_t)(i != 0);
          i = (uint32_t)(i != 0);
          if ((t5 & 0x8000) != 0) {
            e0 = 1;
            i = i + 1;
          }
        }
        p0 = (sep >> 2) * 0x80;
        dlt[1] = 0;
        dlt[0] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x40) ))+ 0xe) ))-
                   *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0xe)));
        dlt[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x40) ))+ 0x12) ))-
                   *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x40) ))+ 0x12)));
        FUN_80194660(dlt,tb);
        e0 = mips_div_lo((int32_t)(p0 * e0),(int32_t)(int)i); /* guest div @0x80146268 */
        sep = tb[0] * e0;
        e0 = tb[2] * e0;
        dlt[0] = (short)(sep >> 0x13);
        dlt[2] = (short)(e0 >> 0x13);
        node_translate_tree(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20))),-(sep >> 0x13),0,-(e0 >> 0x13));
        e0 = mips_div_lo((int32_t)(p0 * k),(int32_t)(int)i); /* guest div @0x801462C8 */
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd4) ))- dlt[0];
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38) ))+ 0xd8) ))- dlt[2];
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c)));
        sep = tb[0] * e0;
        e0 = tb[2] * e0;
        tb[0] = (short)(sep >> 0x13);
        tb[2] = (short)(e0 >> 0x13);
        node_translate_tree(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20))),sep >> 0x13,0,e0 >> 0x13);
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd4) ))+ tb[0];
        *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd8) ))=
             *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38) ))+ 0xd8) ))+ tb[2];
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38)));
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c) ))=
             *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c)));
      }
    }
  }
  return;

}

/* node_translate_tree @0x8018d700 — translate a node subtree: add (dx,-dy,dz) to
 * the node position at +0x34/+0x38/+0x3c, translate the +0x4c child's own
 * position (+0x10/+0x14/+0x18), and recurse via the +0x44/+0x48 links. */
void node_translate_tree(int node,int dx,int dy,int dz)
{
  int child;
  int z0;
  uint32_t child2;

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
    child2 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x44)));
    *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x3c) ))= z0 + dz;
    node_translate_tree(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x48))),dx,dy,dz);
    node_translate_tree(child2,dx,dy,dz);
  }
  return;

}

/* guest uint8_t add: lbu + addu + sb — only the low 8 bits survive. */
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
 *       apply uint8_t-pair deltas to record j (stride below).
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
        /* chan 5: count cell t4+0x70, 40-uint8_t records, base +0x25,
         * pairs at record offsets {0xC,0x14,0x1C,0x24}. */
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
          /* chan 6: count cell t4+0x74, 40-uint8_t records, base +0x25,
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
        /* chan 4: count cell t4+0x6c, 32-uint8_t records, base +0x1d,
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

/* fighter_attach_run @0x80186b2c — run the attach/bone-follow update for fighter f:
 * propagate parent bone transforms into attached nodes. */
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
      if (*(uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ent + 0x18) )))== 0xffff) {
        return;
      }
      ni = idx + 1;
      {
        uint32_t fn_idx = *(uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ent + 0x18) )))& 0xf;
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

/* fighter_height_track @0x80179638 — track fighter height: compare target height
 * f+0x94 against current node height, set the +0x5c dirty bit on change,
 * and snap/zero climb state when the fighter lands. */
void fighter_height_track(int f,int st)
{
  short hnew;
  int cur_h;
  uint32_t fl;
  int hnew_i;

  hnew = *(short *)guest_at((uint32_t)(uintptr_t)((f + 0x94)));
  hnew_i = (int)hnew;
  fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))& 0xfffffe7f;
  cur_h = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x20) ))+ 0x38)));
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= fl;
  if ((cur_h != hnew_i) || (*(short *)guest_at((uint32_t)(uintptr_t)((f + 0x90) ))!= 0 || *(short *)guest_at((uint32_t)(uintptr_t)((f + 0x92) ))!= 0)) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= fl | 0x80;
    if ((hnew_i <= cur_h) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x58) ))& 0x1000000) == 0)) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((f + 0x92) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((f + 0x90) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x6c) ))| 0x80000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))& 0xffffff7f;
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x20) ))+ 0x38) ))= hnew_i;
      *(short *)guest_at((uint32_t)(uintptr_t)((st + 0xd6) ))= hnew;
      return;
    }
    if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((f + 0x90)))) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 0x5c) ))| 0x100;
    }
  }
  return;

}

/* hitbox_corners_xform @0x8017fe60 — transform both fighters' hitbox corner
 * vertices through their world matrices into the shared corner buffer. */
void hitbox_corners_xform(int f0,int f1)
{
  short ez;
  short ex;
  short ey;
  short ez2;
  int t5;
  int t6;
  int t7;
  int t8;
  int t9;
  int t10;
  uint32_t t30;

  t8 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x38)));
  t9 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x38)));
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0x60000) == 0) {
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))& 0x60000) == 0) {
      ez = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xe0)));
      t6 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
      ex = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xdc)));
      t7 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd0)));
      ey = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xde)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t6 + 0x18) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      t8 = lut_cos_q12(t7);
      t5 = lut_sin_q12(t7);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))+ (ey >> 4);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))+ (short)((uint32_t)(ex * t8 - ez * t5) >> 0x10);
      t8 = lut_sin_q12(t7);
      t5 = lut_cos_q12(t7);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))+ (short)((uint32_t)(ex * t8 + ez * t5) >> 0x10);
      *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x38)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t6 + 0x3c)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 4)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x24) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 8)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x28) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0xc)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x2c) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x10)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x30) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x14)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c)));
      node_xform_apply(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x44))),*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20,
                   0);
      ez = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xe0)));
      t8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
      ex = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xdc)));
      ey = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd0)));
      ez2 = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xde)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0x18) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    }
    else {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f1 + 0x6c) ))& 0xfffdffff;
      ez = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xe0)));
      t7 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
      ex = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xdc)));
      t10 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd0)));
      ey = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xde)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      t5 = lut_cos_q12(t10);
      t6 = lut_sin_q12(t10);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))+ (ey >> 4);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))+ (short)((uint32_t)(ex * t5 - ez * t6) >> 0x10);
      t5 = lut_sin_q12(t10);
      t6 = lut_cos_q12(t10);
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))=
           *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))+ (short)((uint32_t)(ex * t5 + ez * t6) >> 0x10);
      *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
      *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x34)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x38)));
      *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x3c)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 4)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x24) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 8)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x28) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0xc)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x2c) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x10)));
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x30) ))=
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x14)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a)));
      *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c)));
      node_xform_apply(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x44))),*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20,
                   0);
      ez = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xe0)));
      ex = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xdc)));
      ey = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd0)));
      ez2 = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xde)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8)));
      t8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0x18) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6)));
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8)));
    }
    t30 = (int)ez;
    t6 = (int)ey;
    t9 = lut_cos_q12(t6);
    t5 = lut_sin_q12(t6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))+ (ez2 >> 4);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))+ (short)((uint32_t)(ex * t9 - t30 * t5) >> 0x10);
    t9 = lut_sin_q12(t6);
    t5 = lut_cos_q12(t6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))+ (short)((uint32_t)(ex * t9 + t30 * t5) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x34)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x38)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0x3c)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x20) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 4)))
    ;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x24) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 8)))
    ;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x28) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0xc)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x2c) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x10)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x30) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x14)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c)));
    t8 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
  }
  else {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f0 + 0x6c) ))& 0xfffdffff;
    ez = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xe0)));
    t7 = *(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20)));
    ex = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xdc)));
    t10 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd0)));
    ey = *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xde)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    t5 = lut_cos_q12(t10);
    t6 = lut_sin_q12(t10);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))+ (ey >> 4);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))+ (short)((uint32_t)(ex * t5 - ez * t6) >> 0x10);
    t5 = lut_sin_q12(t10);
    t6 = lut_cos_q12(t10);
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))+ (short)((uint32_t)(ex * t5 + ez * t6) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x34)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x38)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f1 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t7 + 0x3c)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x20) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 4)))
    ;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x24) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 8)))
    ;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x28) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0xc)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x2c) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x10)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x30) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x14)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x18)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1a)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x1c)));
    node_xform_apply(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x44))),*(int *)guest_at((uint32_t)(uintptr_t)((f1 + 0x20) ))+ 0x20,0)
    ;
    ez = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xe0)));
    ex = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xdc)));
    t6 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd0)));
    ey = *(short *)guest_at((uint32_t)(uintptr_t)((t8 + 0xde)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd4)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd6)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t9 + 0xd8)));
    t5 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x18) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd4)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd6)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0xd8)));
    t8 = lut_cos_q12(t6);
    t9 = lut_sin_q12(t6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))+ (ey >> 4);
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))+ (short)((uint32_t)(ex * t8 - ez * t9) >> 0x10);
    t8 = lut_sin_q12(t6);
    t9 = lut_cos_q12(t6);
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))+ (short)((uint32_t)(ex * t8 + ez * t9) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x18) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x18) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1a) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c)));
    *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1c) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x18) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x34)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1a) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x38)));
    *(short *)guest_at((uint32_t)(uintptr_t)((f0 + 0x9e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x1c) ))- *(short *)guest_at((uint32_t)(uintptr_t)((t5 + 0x3c)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x20) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 4)))
    ;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x24) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 8)))
    ;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x28) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0xc)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x2c) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x10)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x30) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x14)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x34) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x18)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x38) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1a)));
    *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x3c) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20) ))+ 0x1c)));
    t8 = *(int *)guest_at((uint32_t)(uintptr_t)((f0 + 0x20)));
  }
  node_xform_apply(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t8 + 0x44))),t8 + 0x20,0);
  return;

}

/* pad_edge_scan @0x80142df4 — scan the 0x10 controller pad slots and build the
 * per-button repeat and multi-press bitmasks at ctrl_g+0x80/+0x84,
 * with per-button hold-time table walk. */
void pad_edge_scan(int ctrl_g)
{
  uint16_t cur;
  uint16_t prev;
  uint32_t old;
  int ent;
  uint32_t *slot;
  uint32_t bit;
  uint32_t i;
  uint32_t edge;
  uint32_t multi;
  uint32_t rep;
  edge = 0;
  rep = 0;
  multi = 0;
  i = 0;
  cur = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c)));
  prev = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a)));
  ent = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x84;
  slot = (uint32_t *)(&DAT_801f0fb0 + ent);
  do {
    bit = 1 << (i & 0x1f);
    if ((prev & bit) == 0) {
      if ((cur & bit) == 0) {
        *slot = 0;
      }
      else {
        old = *slot;
        *slot = old + 1;
        if ((old + 1 < 0x11) && ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82) ))& bit) != 0)) {
          edge = edge | bit;
        }
      }
      slot[0x10] = slot[0x10] + 1;
    }
    else {
      if ((slot[0x10] <= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ent + -0x7fe0efd0)))) && (slot[0x10] < 0xc)) {
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
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ent + -0x7fe0efd0) ))= 0;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x84) ))= (short)multi;
  *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80) ))= (short)rep;
  *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82) ))= (short)edge;
  return;

}

/* cmd_input_parse @0x801434d4 — parse raw pad state into the command stream for
 * ctrl_g: per-button press/repeat timing produces command codes. */
void cmd_input_parse(int ctrl_g)
{
  bool pos;
  short prev;
  bool clr;
  short cnt;
  uint8_t nxt;
  uint16_t cw;
  uint32_t t7;
  int cur;
  uint32_t j;
  int t10;
  short *tab;
  uint16_t *tp;
  short *rc;
  int t14;
  int st;
  /* btn is guest $t5 (and 0xFFDF @0x80143834 on gpr[13]): the inner
   * switch's >7 fallthrough reads it without a write — previous loop
   * iteration's value, or the caller's $t5 on the first pass. */
  uint16_t btn = (uint16_t)br2_gpr(13);
  int i;
  int *hd;
  uint32_t opc;
  short arg;
  opc = 0xffffffff;
  arg = 0;
  st = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x38)));
  t14 = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x168;
  tab = (short *)(&DAT_801f0ae0 + t14);
  hd = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0xc))));
  clr = false;
  if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) == 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7e) ))< '\0')) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x3a) ))= 0;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7c) ))= 0;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))= 0;
  }
  t7 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))& 0xfcffffff;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))= t7;
  prev = *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x14)));
  if (-1 < *(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7e)))) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))= t7 | 0x1000000;
    prev = *(short *)guest_at((uint32_t)(uintptr_t)((st + *(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7e) ))* 2 + 0x3c)));
  }
  i = 0;
  rc = (short *)guest_at((uint32_t)(uintptr_t)((t14 + -0x7fe0f51e)));
LAB_8014359c:
  if (*hd != 0) {
    t14 = *hd + *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x30)));
    do {
      switch(*(uint16_t *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14)))) {
      case 0:
        goto switchD_801435e8_caseD_0;
      case 1:
        if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)))) goto LAB_80143970;
        cw = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14 + 2)));
        pos = (cw & 0x4000) == 0;
        if (pos) {
          t7 = 1;
        }
        else {
          t7 = (cw & 0xfff) + 1;
        }
        j = (uint32_t)!pos;
        if (t7 <= j) goto LAB_80143970;
        goto LAB_8014365c;
      case 2:
        if (((-1 < *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)))) || (((uint16_t)rc[1] & 0xfff) != (int)prev)) ||
           (((rc[1] & 0x1000U) != 0 &&
            ((*(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7c) ))!= *(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))||
             ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) != 0)))))) goto LAB_80143970;
        if ((-1 < rc[2]) &&
           ((*(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7e) ))< '\0' && (rc[2] < *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xbc)))))))
        goto switchD_801435e8_default;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c) ))| 0x2000000;
        switch(*(uint16_t *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14 + 4)))) {
        case 0:
        case 1:
          btn = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c)));
          break;
        case 2:
          btn = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a)));
          break;
        case 3:
          btn = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          break;
        case 4:
          btn = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x84)));
          break;
        case 5:
          btn = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82)));
          break;
        case 6:
          btn = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c) ))& 0xf000 | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          break;
        case 7:
          btn = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          cw = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          if ((cw != 0xc0) && (cw != 0x88)) goto switchD_801435e8_default;
        }
        if (((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd6) ))< 0x1e00)) {
          btn = btn & 0xffdf;
        }
        t10 = *tab * 2 + t14;
        if ((btn & *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t10 + 6)))) != *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t10 + 8))))
        goto switchD_801435e8_default;
        cnt = *rc;
        *rc = cnt + 1;
        if ((int)(short)(cnt + 1) < (int)(uint32_t)*(uint16_t *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14 + 2))))
        goto switchD_801435e8_default;
        *tab = *tab + 5;
        *rc = 0;
        break;
      case 3:
        cw = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14 + 2)));
        clr = false;
        if ((cw & 0x2000) == 0) goto LAB_801438fc;
        t10 = 0;
        if (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))== 0) goto LAB_801438fc;
        cur = st;
        goto LAB_801438e0;
      case 4:
        rc[2] = *(short *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14 + 2) ))<< 4;
        rc[3] = *(short *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14 + 4) ))<< 4;
        cnt = *tab + 3;
        goto LAB_801439c4;
      default:
        goto switchD_801435e8_default;
      }
    } while( true );
  }
  goto LAB_801439e8;
  while (cur = cur + 2, t10 < (int)(uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d)))) {
LAB_801438e0:
    t10 = t10 + 1;
    if ((cw & 0xfff) == (int)*(short *)guest_at((uint32_t)(uintptr_t)((cur + 0x3c)))) goto LAB_80143970;
  }
LAB_801438fc:
  opc = cw & 0xfff;
  arg = rc[3];
  rc[1] = (short)opc;
  rc[2] = -1;
  rc[3] = 0;
  *tab = *tab + 2;
  *rc = 0;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((*tab * 2 + t14) ))== 0) goto LAB_801439d8;
switchD_801435e8_default:
  hd = hd + 1;
  rc = rc + 5;
  tab = tab + 5;
LAB_801439d8:
  i = i + 1;
  if (0x23 < i) {
LAB_801439e8:
    if (clr) {
      t14 = 0x23;
      tp = (uint16_t *)(&DAT_801f0ae0 + (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x168);
      do {
        *tp = 0;
        t14 = t14 + -1;
        tp = tp + 5;
      } while (-1 < t14);
    }
    if (-1 < (int)(opc << 0x10)) {
      if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) == 0) && (*(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7e) ))< '\0')) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x3a) ))= (uint16_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xda)));
      }
      *(short *)guest_at((uint32_t)(uintptr_t)((st + (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))* 2 + 0x3c) ))= (short)opc;
      *(short *)guest_at((uint32_t)(uintptr_t)((st + (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))* 2 + 0x5c) ))= arg;
      nxt = *(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))+ 1;
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7e) ))= *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d)));
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))= nxt - (nxt & 0xf0);
    }
    return;
  }
  goto LAB_8014359c;
LAB_80143970:
  *tab = 0;
  goto switchD_801435e8_default;
LAB_8014365c:
  cw = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(((j + (int)*tab) * 2 + t14 + 2)));
  if ((((cw & 0x1000) != 0) &&
      ((*(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7c) ))!= *(char *)guest_at((uint32_t)(uintptr_t)((st + 0x7d) ))||
       ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40) != 0)))) || ((int)prev != (cw & 0xfff)))
  goto LAB_801436a8;
  rc[1] = cw;
  rc[2] = -1;
  rc[3] = 0;
  cnt = *tab + 1 + (short)t7;
LAB_801439c4:
  *tab = cnt;
  *rc = 0;
  goto switchD_801435e8_default;
LAB_801436a8:
  j = j + 1;
  if ((int)t7 <= (int)j) goto LAB_801436b8;
  goto LAB_8014365c;
LAB_801436b8:
  *tab = 0;
  goto switchD_801435e8_default;
switchD_801435e8_caseD_0:
  *tab = 0;
  clr = true;
  goto switchD_801435e8_default;

}

extern void br2_st8(uint32_t,uint8_t);
extern void br2_st16(uint32_t,uint16_t);
extern void fighter_reset_fx(int,uint8_t *);
extern void slot_apply_guest(int,int);
extern void hit_chain_clear(uint8_t *);
extern void pending_hit_a_clear(uint8_t *);
extern void pending_hit_b_clear(uint8_t *);

/* fighter_motion_step — per-fighter motion integrator + conditional reset
 * tail @0x8017CAF4 (55 CPS blocks). a0 = fighter state record, a1 dead,
 * a2 = motion-param record (all guest addrs). Mode = s8(param_h0>>8)
 * selects one of four integrator arms over the state's halfword channels
 * (+172/+176 bounds, +192/+200/+202/+204 channels, +258/+260/+262
 * counters, +216/+214 velocities); every arm merges at the shared commit
 * tail which runs the battle_reset_tail-style reset sequence when the
 * +5 flag bit and the +0/+0xe5/+0xd6 conditions hold. Returns param+4. */
uint32_t fighter_motion_step(uint32_t st,uint32_t unused,uint32_t prm)
{
  (void)unused;
  uint32_t s0 = st;
  uint32_t s1 = prm;
  uint32_t a1 = br2_ld32(s0 + 44);            /* char/attrib record */
  int32_t mode = (int32_t)(int8_t)(br2_ld16(s1) >> 8);
  int32_t v0, v1, a0;
  int64_t prod;
  int32_t hi, q;

  if (mode == 0) {
    /* arm 0: 3/10-rate integrator (magic 0x66666667, mult = saved*3) */
    v0 = br2_ld16(s0 + 204);
    a0 = (int16_t)br2_ld16(s0 + 200);
    br2_st16(s0 + 204, (uint16_t)(v0 + 32));
    v0 = (int16_t)br2_ld16(s0 + 172) + a0;    /* lo + cur */
    if ((int16_t)br2_ld16(s0 + 176) >= (int32_t)v0) {
      v0 = br2_ld16(s0 + 200);
      if (a0 != 0) br2_st16(s0 + 202, (uint16_t)v0);
      v1 = (int16_t)br2_ld16(s0 + 202);
      a0 = 0x66666667;
      v0 = v1 * 3;
      prod = (int64_t)v0 * (int64_t)a0;
      hi = (int32_t)(prod >> 32);
      q = (hi >> 2) - ((int32_t)v0 >> 31);
      v0 = (int16_t)br2_ld16(s0 + 192);
      v1 = br2_ld16(s0 + 192);
      if (v0 >= 0) br2_st16(s0 + 192, (uint16_t)(v1 + q * 16));
      /* shared tail-of-arm: +200 += q, clamp vs +172/+176 */
      v0 = br2_ld16(s0 + 200);
      v1 = (int16_t)br2_ld16(s0 + 172);
      v0 = (uint32_t)((int32_t)v0 + q);
      br2_st16(s0 + 200, (uint16_t)v0);
      v0 = (int16_t)v0;
      v1 += v0;
      a0 = (int16_t)br2_ld16(s0 + 176);
      v0 = br2_ld16(s0 + 176);
      if (a0 < v1) {
        v0 = (uint32_t)(v0 - br2_ld16(s0 + 172));
        br2_st16(s0 + 200, (uint16_t)v0);
      }
    }
  } else if (mode == 1) {
    /* arm 1: 1/10-rate integrator, q truncated to s16 */
    v0 = br2_ld16(s0 + 204);
    a0 = (int16_t)br2_ld16(s0 + 200);
    br2_st16(s0 + 204, (uint16_t)(v0 + 32));
    v0 = (int16_t)br2_ld16(s0 + 172) + a0;
    if ((int16_t)br2_ld16(s0 + 176) >= (int32_t)v0) {
      v0 = br2_ld16(s0 + 200);
      if (a0 != 0) br2_st16(s0 + 202, (uint16_t)v0);
      v1 = br2_ld16(s0 + 202);
      a0 = 0x66666667;
      v0 = (int32_t)(v1 << 16) >> 16;          /* s16 saved */
      prod = (int64_t)v0 * (int64_t)a0;
      hi = (int32_t)(prod >> 32);
      q = (hi >> 2) - ((int32_t)(v1 << 16) >> 31);
      a0 = (int16_t)q;                          /* truncate to s16 */
      v0 = (int16_t)br2_ld16(s0 + 192);
      v1 = br2_ld16(s0 + 192);
      if (v0 >= 0) br2_st16(s0 + 192, (uint16_t)(v1 + a0 * 16));
      v0 = br2_ld16(s0 + 200);
      v1 = (int16_t)br2_ld16(s0 + 172);
      v0 = (uint32_t)((int32_t)v0 + a0);
      br2_st16(s0 + 200, (uint16_t)v0);
      v0 = (int16_t)v0;
      v1 += v0;
      a0 = (int16_t)br2_ld16(s0 + 176);
      v0 = br2_ld16(s0 + 176);
      if (a0 < v1) {
        v0 = (uint32_t)(v0 - br2_ld16(s0 + 172));
        br2_st16(s0 + 200, (uint16_t)v0);
      }
    }
  } else if (mode == 2) {
    /* arm 2: attr-scaled double-threshold integrator (uses product LO) */
    int32_t pm = (int16_t)br2_ld16(s1 + 2);
    int32_t attr = br2_ld8(a1 + 218);
    int32_t sel, acc, b0, m2, lo2, lo3, h;
    prod = (int64_t)attr * (int64_t)pm;
    a0 = (int32_t)prod;                          /* cpu->lo */
    sel = br2_ld8(a1 + 5) & 0x10;
    m2 = (int16_t)br2_ld16(a1 + (sel ? 256 : 254));
    lo2 = (int32_t)((int64_t)a0 * (int64_t)m2);
    if (lo2 >= 0) a0 = lo2 >> 7; else a0 = (lo2 + 127) >> 7;
    sel = br2_ld8(s0 + 5) & 0x10;
    m2 = (int16_t)br2_ld16(s0 + (sel ? 260 : 258));
    lo3 = (int32_t)((int64_t)a0 * (int64_t)m2);
    if (lo3 >= 0) a0 = lo3 >> 7; else a0 = (lo3 + 127) >> 7;
    acc = a0 + br2_ld8(s0 + 262);
    br2_st8(s0 + 262, (uint8_t)(acc & 0x7f));
    a0 = acc >> 7;
    b0 = (int16_t)br2_ld16(s0 + 192);
    if (b0 >= 0) br2_st16(s0 + 192, (uint16_t)(a0 * 16));
    v0 = br2_ld16(s0 + 200);
    br2_st16(s0 + 204, 0);
    br2_st16(s0 + 200, (uint16_t)(v0 + a0));
    if (!(br2_ld8(a1 + 5) & 0x10) && !(br2_ld32(s0 + 108) & 0x8000)) {
      h = a0 + (int32_t)((uint32_t)a0 >> 31);
      br2_st16(a1 + 216, (uint16_t)(br2_ld16(a1 + 216) + (h >> 1) * 60));
    }
    if (br2_ld8(s0 + 5) & 0x10) {
      br2_st16(s0 + 216, (uint16_t)(br2_ld16(s0 + 216) - a0 * 120));
    } else {
      prod = (int64_t)a0 * (int64_t)0x55555556;
      q = (int32_t)(prod >> 32) - (a0 >> 31);
      br2_st16(s0 + 216, (uint16_t)(br2_ld16(s0 + 216) + q * 60));
    }
  } else if (mode == 3) {
    /* arm 3: param-driven channel set */
    int32_t p1 = (int16_t)br2_ld16(s1 + 2);
    if ((int16_t)br2_ld16(s0 + 192) >= 0)
      br2_st16(s0 + 192, (uint16_t)(p1 * 16));
    v0 = br2_ld16(s0 + 200);
    br2_st16(s0 + 204, 0);
    br2_st16(s0 + 200, (uint16_t)(v0 + p1));
  }

  /* shared commit tail: conditional reset sequence */
  if (br2_ld8(s0 + 5) & 1) {
    int rst = 0;
    if (br2_ld8(s0) & 0x80) {
      uint32_t p = br2_ld32(0x1f8000c0);
      if (br2_ld8(s0 + 229) >= br2_ld8(p + 4)) rst = 1;
    }
    if (!rst && (int16_t)br2_ld16(s0 + 214) < 0) rst = 1;
    if (rst) {
      deferred_cmd_post(0x400, 7, 0);
      fighter_reset_fx(1, (uint8_t *)guest_at(s0));
      v0 = br2_ld16(s0 + 172);
      br2_st16(s0 + 214, 0);
      br2_st16(s0 + 216, 0);
      br2_st8(s0 + 5, 0);
      br2_st16(s0 + 174, (uint16_t)v0);
      slot_apply_guest((int)s0, 0);
      hit_chain_clear((uint8_t *)guest_at(s0));
      pending_hit_a_clear((uint8_t *)guest_at(s0));
      pending_hit_b_clear((uint8_t *)guest_at(s0));
    }
  }
  return s1 + 4;
}

/* action_from_input @0x80142f3c — map controller input to the action/state
 * selection state machine (button timers, combo windows, state codes). */
void action_from_input(int ctrl_g)
{
  short prev;
  int t2;
  uint16_t fl3;
  int bi;
  uint32_t fl;
  uint32_t fl2;
  short *lim;
  int st;
  uint32_t *rec;
  short *tm;
  short *cnt;
  /* t3 is guest $t3 (and 0xFFDF @0x801433CC on gpr[11]): lim[2] values
   * outside 0-7 fall through the switch and read it stale — previous
   * k-iteration's value, or the caller's $t3 on the first pass. */
  uint16_t t3 = (uint16_t)br2_gpr(11);
  int k;
  uint32_t *btn;
  st = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x38)));
  t2 = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))* 0x100;
  cnt = (short *)(&DAT_801f0db0 + t2);
  btn = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 8))));
  k = 0;
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x8000) == 0) {
    rec = btn + 2;
    tm = (short *)guest_at((uint32_t)(uintptr_t)((t2 + -0x7fe0f24a)));
    do {
      if (*btn == 0) {
        return;
      }
      if (((*btn & 0x100000) == 0) || (_DAT_801c4578 != 0)) {
LAB_80142fa0:
        bi = *cnt + -1;
        t2 = bi * 4;
        if (bi < 0) {
          bi = 0;
          t2 = 0;
        }
        lim = (short *)guest_at((uint32_t)(uintptr_t)((rec[1] + (t2 + bi) * 2 + *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x30))))));
        switch(lim[2]) {
        case 0:
          *cnt = 0;
          fl = *btn;
          if ((fl & 0x80000) == 0) {
            if ((((fl & 0x40) != 0) && ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0)) &&
               (((fl & 0x80) != 0 || (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd6) ))< 0x1e00)))) goto LAB_801434b4;
          }
          else if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0) goto LAB_801434b4;
          fl = *btn;
          if ((fl & 0x80) == 0) {
            if ((fl & 0x100) == 0) {
              if ((fl & 0x10000) == 0) {
                if (*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1c) ))< (short)rec[-1]) {
                  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x18) ))= fl;
                  *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1c) ))= (short)rec[-1];
                  *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x1e) ))= *(short *)((intptr_t)rec + -2);
                }
                goto LAB_801434b4;
              }
              fl2 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)));
              if ((fl2 & 0x1000) == 0) goto LAB_801434b4;
              if ((fl & 0x20000) == 0) {
                if ((fl & 0x40000) != 0) {
                  if ((fl2 & 0x80002010) == 0x2000) goto LAB_801431cc;
                  goto LAB_801434b4;
                }
                if (((fl2 & 0x800) == 0) || ((short)rec[-1] <= *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x34)))))
                goto LAB_801434b4;
                *(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x30) ))= fl;
                *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x34) ))= (short)rec[-1];
                *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36) ))= *(short *)((intptr_t)rec + -2);
                fl3 = *(uint16_t *)((intptr_t)rec + 2);
                *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x38) ))= fl3;
                fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x54)));
              }
              else {
                if ((fl2 & 0x2800) != 0) goto LAB_801434b4;
LAB_801431cc:
                if ((short)rec[-1] <= *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x34)))) goto LAB_801434b4;
                *(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x30) ))= fl;
                *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x34) ))= (short)rec[-1];
                *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x36) ))= *(short *)((intptr_t)rec + -2);
                fl3 = *(uint16_t *)((intptr_t)rec + 2);
                *(uint16_t *)guest_at((uint32_t)(uintptr_t)((st + 0x38) ))= fl3;
                fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x54)));
              }
              if ((fl & 8) == 0) {
                if ((fl & 0x10) != 0) {
                  t2 = (short)fl3 * 7;
                  if (t2 < 0) {
                    t2 = t2 + 7;
                  }
                  *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x38) ))= (short)(t2 >> 3);
                }
              }
              else {
                *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x38) ))=
                     (short)((((int)((uint32_t)fl3 << 0x10) >> 0x10) -
                             ((int)((uint32_t)fl3 << 0x10) >> 0x1f)) >> 1);
              }
              goto LAB_801434b4;
            }
            if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x800) == 0) ||
                ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x8000) != 0)) || (k <= *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x2a)))))
            goto LAB_801434b4;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))= fl;
          }
          else {
            if ((((fl & 0x3000) != 0x3000) &&
                ((((fl & 0x1000) != 0 &&
                  ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x2c) ))+ 0x5c) ))& 0x400000) != 0)) ||
                 (((fl & 0x2000) != 0 &&
                  ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x2c) ))+ 0x5c) ))& 0x400000) == 0)))))) ||
               ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x800) != 0 ||
                 ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))& 0x8000) != 0)) || (k <= *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x2a)))))
               )) goto LAB_801434b4;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0x20) ))= *btn;
          }
          *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x24) ))= (short)rec[-1];
          *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x26) ))= *(short *)((intptr_t)rec + -2);
          prev = *(short *)((intptr_t)rec + 2);
          *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x2a) ))= (short)k;
          *(short *)guest_at((uint32_t)(uintptr_t)((st + 0x28) ))= prev;
          goto LAB_801434b4;
        case 1:
          t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c)));
          break;
        case 2:
          t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a)));
          break;
        case 3:
          t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          break;
        case 4:
          t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x84)));
          break;
        case 5:
          t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x82)));
          break;
        case 6:
          t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7c) ))& 0xf000 | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          break;
        case 7:
          t3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x80)));
          fl3 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x7a) ))& 0xe8;
          if ((fl3 == 0xc0) || (fl3 == 0x88)) break;
          goto LAB_80143448;
        }
        if (((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 5) ))& 1) == 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd6) ))< 0x1e00)) {
          t3 = t3 & 0xffdf;
        }
        if ((t3 & lim[3]) == lim[4]) {
          if (*cnt == 0) {
            *cnt = *cnt + 1;
            *tm = 0;
            tm[-1] = 0;
            tm[-2] = 0;
          }
          prev = tm[-2];
          tm[-2] = prev + 1;
          if ((short)(prev + 1) < *lim) goto LAB_80143484;
          *cnt = *cnt + 1;
          tm[-2] = 0;
          tm[-1] = 0;
          goto LAB_80142fa0;
        }
LAB_80143448:
        if (*cnt == 0) goto LAB_801434b4;
        prev = tm[-1];
        tm[-1] = prev + 1;
        if (lim[1] <= (short)(prev + 1)) {
          *cnt = 0;
        }
LAB_80143484:
        prev = *tm;
        *tm = prev + 1;
        if ((short)*rec <= (short)(prev + 1)) {
          *cnt = 0;
        }
      }
LAB_801434b4:
      tm = tm + 4;
      cnt = cnt + 4;
      rec = rec + 4;
      k = k + 1;
      btn = btn + 4;
    } while (k < 0x20);
  }
  return;

}

/* seg_sphere_hit @0x801454f0 — segment vs sphere intersection test (two radii);
 * on hit writes the contact point to hit[0..2] and returns 1. */
uint32_t seg_sphere_hit(int rad0,int rad1,int *seg0,int *seg1,int *cen,short *hit)
{
  short sz;
  int r2;
  int t3;
  int t4;
  int t5;
  int t6;
  int dz;
  int iz;
  uint32_t q;
  r2 = (rad0 + rad1) * (rad0 + rad1);
  t4 = *seg0;
  t3 = seg0[1];
  t6 = *cen;
  t5 = cen[1];
  dz = cen[2] - seg0[2];
  _DAT_1f800398 = t6 - t4;
  _DAT_1f80039c = t5 - t3;
  _DAT_1f8003a0 = dz;
  gte_op_a00428(&DAT_1f800398,&DAT_1f8003a4);
  if ((int32_t)(_DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac) < r2) {
    *hit = (short)*seg0 + (short)((t6 - t4) / 2);
    hit[1] = (short)seg0[1] + (short)((t5 - t3) / 2);
    sz = (short)seg0[2];
    dz = dz - (dz >> 0x1f);
  }
  else {
    if (((*seg1 - *seg0 == 0) && (seg1[1] - seg0[1] == 0)) &&
       (seg1[2] - seg0[2] == 0)) {
      return 0;
    }
    t4 = *cen;
    t3 = *seg1;
    t6 = cen[1];
    t5 = seg1[1];
    dz = cen[2] - seg1[2];
    _DAT_1f80038c = *seg1 - *seg0;
    _DAT_1f800390 = seg1[1] - seg0[1];
    _DAT_1f800394 = seg1[2] - seg0[2];
    _DAT_1f8003a4 = t4 - t3;
    _DAT_1f8003a8 = t6 - t5;
    _DAT_1f8003ac = dz;
    gte_op_a00428(&DAT_1f8003a4,&DAT_1f8003a4);
    if (r2 <= (int32_t)(_DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac)) {
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
      t3 = _DAT_1f8003a4;
      gte_op_a00428(&DAT_1f80038c,&DAT_1f8003a4);
      t4 = (int32_t)(_DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac);
      if (t4 < t3) {
        return 0;
      }
      t4 = t4 >> 8;
      if (t4 == 0) {
        return 0;
      }
      q = (int32_t)((uint32_t)t3 << 4) / t4;
      if (q < 0x1001) {
        _DAT_1f8003b0 = (uint16_t)q;
        _DAT_1f8003b4 = 0;
        _DAT_1f8003b2 = 0;
        _DAT_1f8003b8 = _DAT_1f8003b0;
        _DAT_1f8003c0 = _DAT_1f8003b0;
        gte_mvmva_add(&DAT_1f8003b0,&DAT_1f80038c,&DAT_1f8003a4);
        t5 = _DAT_1f8003a4 + *seg0;
        t3 = *cen - t5;
        t6 = _DAT_1f8003a8 + seg0[1];
        t4 = cen[1] - t6;
        iz = _DAT_1f8003ac + seg0[2];
        dz = cen[2] - iz;
        _DAT_1f80038c = t3;
        _DAT_1f800390 = t4;
        _DAT_1f800394 = dz;
        gte_op_a00428(&DAT_1f80038c,&DAT_1f8003a4);
        if ((int32_t)(_DAT_1f8003a4 + _DAT_1f8003a8 + _DAT_1f8003ac) < r2) {
          *hit = (short)t5 + (short)(t3 / 2);
          hit[1] = (short)t6 + (short)(t4 / 2);
          hit[2] = (short)iz + (short)(dz / 2);
          return 1;
        }
      }
      return 0;
    }
    *hit = (short)*seg1 + (short)((t4 - t3) / 2);
    dz = dz - (dz >> 0x1f);
    hit[1] = (short)seg1[1] + (short)((t6 - t5) / 2);
    sz = (short)seg1[2];
  }
  hit[2] = sz + (short)(dz >> 1);
  return 1;

}

/* sphere_pt_hit @0x801453a0 — point vs dual-radius sphere test; writes the hit
 * point offset to mid[] and returns the inside flag. */
uint32_t sphere_pt_hit(int r0,int r1,int *p,int *q,short *mid)
{
  int ad;
  int dy;
  int dz;
  int dx;
  /* Guest vec3 slot: gte_op_a00428 reads/writes 3 contiguous u32
   * through vdif; the decompiler split the 12-uint8_t vec3 into
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
         (vdif[0] = dx, vdif[1] = dy, vdif[2] = dz, gte_op_a00428((uint32_t *)vdif,(uint32_t *)vdif),
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

/* debug_strike_fx_a @0x8013f034 — debug strike-FX variant A: spawn hit spark at pos
 * with screen offsets (x,y) for fighter f. */
void debug_strike_fx_a(short *pos,int x,int y,uint32_t unused,int f)
{
  int en;
  /* Guest vec3 slot: quad_fx_spawn reads y[0..2] as 3
   * contiguous u32; the decompiler split the 12-uint8_t vec3 into
   * scalars. vpos[0..2] = old vpos/local_24/local_20. */
  int vpos [3];
  if ((((DAT_1f8000b1 == '\0') && (en = always_one(8), en != 0)) &&
      ((*(uint16_t *)(_g_pad_buffer + 2) >> 4 & 1) != 0)) && (DAT_1f8000b1 == '\0')) {
    impact_fx_spawn(pos,0x40000001,0,0,0);
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 100) ))& 0x20000) != 0) {
      vpos[0] = (int)*pos;
      vpos[1] = (int)pos[1];
      vpos[2] = (int)pos[2];
      quad_fx_spawn(f,0x13,(uint32_t *)vpos,800,0x1000,0,1);
      emit_task_spawn(pos,6,x,y,0x18,1);
      emit_task_spawn(pos,6,x + 0x800,0x800 - y,0xc,1);
    }
  }
  return;

}

/* debug_strike_fx_b @0x8013ee40 — debug strike-FX variant B: spawn hit spark with
 * packed offset args and heading ang for fighter f. */
void debug_strike_fx_b(short *pos,uint32_t x,uint32_t y,uint16_t ang,int f)
{
  char mode;
  uint8_t *pal;
  uint16_t *rec;
  uint32_t fl;
  uint16_t t;
  uint16_t dur;
  uint32_t ea;
  uint32_t eb;
  if (DAT_1f8000b1 == '\0') {
    mode = *(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x38) ))+ 0x101)));
    ang = ang & 0xfff;
    if ((((mode == '\x02') || (mode == '\f')) && (*(char *)guest_at((uint32_t)(uintptr_t)((_DAT_1f8000c4 + 2) ))!= '\0')) &&
       ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 100) ))& 0x20000) == 0)) {
      impact_fx_spawn(pos,0x40000000,ang,0,0);
      if (ang < 0xf) {
        pal = &DAT_801b1bd4;
      }
      else {
        pal = &DAT_801b1be8;
      }
      rec = (uint16_t *)guest_at((uint32_t)(uintptr_t)(fx_record_alloc((uint32_t)br2_gaddr(pos),6,0x40000005,0,(uint32_t)br2_gaddr(pal))));
      if (rec != (uint16_t *)0x0) {
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
      fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((f + 100)));
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

/* noop_fn @0x80140c90 — empty task callback (guest fn-table placeholder). */
void noop_fn(void)
{
  return;

}

/* rtps_pt_xform @0x8018d5b4 — transform one s16 vec3 point through the current
 * GTE RTPS matrix; writes s16 x/y/z to out[0..2]. */
uint16_t * rtps_pt_xform(uint16_t *pt,uint16_t *out)
{
  uint32_t tx;
  uint32_t ty;
  uint32_t tz;
  setCopReg(2,0,*(int *)(pt + 2) << 0x10 | (uint32_t)*pt);
  setCopReg(2,0x800,*(uint32_t *)(pt + 4));
  copFunction(2,0x486012);
  tx = getCopReg(2,0x4800);
  ty = getCopReg(2,0x5000);
  tz = getCopReg(2,0x5800);
  *out = (short)tx;
  out[1] = (short)ty;
  out[2] = (short)tz;
  return out;

}


/* node_xform_apply @0x8018d788 — recursively transform a node tree: RTPS the local
 * vec/angles at +0x18/+0x20, then descend +0x48 (same mat) and +0x44
 * (local mat at node+0x20) children. */
void node_xform_apply(int node,uint32_t mat,uint32_t flg)
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
    rtps_point_xform((uint32_t *)guest_at((uint32_t)mat),(uint32_t *)guest_at((uint32_t)(node + 0x18)),(int *)guest_at((uint32_t)(node + 0x34)),(int)flg);
    gte_xform3_nold((uint16_t *)guest_at((uint32_t)(node + 4)),(uint32_t *)guest_at((uint32_t)(node + 0x20)));
    node_xform_apply(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x48))),mat,1);
    aux = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x4c)));
    if (aux != 0) {
      rtps_point_xform2((uint32_t *)guest_at((uint32_t)(node + 0x20)),(uint32_t *)guest_at((uint32_t)(aux + 4)),(int *)guest_at((uint32_t)(aux + 0x10)));
    }
    node_xform_apply(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x44))),node + 0x20,aux);
  }
  return;

}

/* attr_flag_mask @0x80143ac8 — compose the attribute flag mask for a node class:
 * base flags from DAT_1f80006c[class*2] plus per-class table words
 * selected by the various fl bits (0x80/0x40/0x20/.../0x1). */
uint16_t attr_flag_mask(uint32_t node)
{
  uint16_t fl;
  int base;
  uint16_t m;
  fl = *(uint16_t *)(&DAT_1f80006c + (node & 0xff) * 2);
  base = (node & 0xff) * 0x10;
  m = fl & 0xf000;
  if ((fl & 0x80) != 0) {
    m = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b880) ))| m;
  }
  if ((fl & 0x40) != 0) {
    m = m | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b87e)));
  }
  if ((fl & 0x20) != 0) {
    m = m | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b87c)));
  }
  if ((fl & 0x10) != 0) {
    m = m | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b87a)));
  }
  if ((fl & 8) != 0) {
    m = m | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b878)));
  }
  if ((fl & 2) != 0) {
    m = m | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b876)));
  }
  if ((fl & 4) != 0) {
    m = m | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b874)));
  }
  if ((fl & 1) != 0) {
    m = m | *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + -0x7fe0b872)));
  }
  return m;

}

/* pad_hist_record @0x801289b4 — push pad state into the u16 history
 * ring at DAT_801cf88c (per-controller state ptr via ctrl_g+0xcc);
 * returns the edge-mapped button flags for this pad. */
uint16_t pad_hist_record(int ctrl_g,int pad,uint16_t flags)
{
  uint16_t *hist;
  /* cell at ctrl_g+0xcc holds a guest u32 pointer to the u16 history
   * array — read the guest word then translate (never a host-ptr-size
   * deref of guest memory). */
  hist = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc))));
  switch(*hist) {
  default:
    return flags;
  case 3:
    if (599 < (short)hist[3]) {
      hist[3] = 0;
    }
    if ((int)(short)hist[3] != (int)(short)hist[2]) {
      return *(uint16_t *)(&DAT_801cf88c + (short)hist[3] * 2 + pad * 0x4b0);
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
      return *(uint16_t *)(&DAT_801cf88c + (short)hist[3] * 2 + pad * 0x4b0);
    }
    *hist = 1;
  case 1:
  case 2:
    if (599 < (short)hist[3]) {
      hist[3] = 0;
    }
    hist[2] = hist[3];
    *(uint16_t *)(&DAT_801cf88c + (short)hist[3] * 2 + pad * 0x4b0) = flags;
    return flags;
  }

}

/* fighter_script_fsm @0x80181f20 — per-fighter script bytecode VM: fetch
 * opcode at ip, dispatch handler through the opcode table, update
 * phase/operand regs until the script ends. */
void fighter_script_fsm(int ctrl_g,uint32_t pad,int *script,int ctx)
{
  uint8_t o0;
  uint8_t o1;
  short ph;
  uint16_t lo;
  uint16_t t2;
  int t0;
  uint16_t *tp;
  uint8_t *tb;
  uint32_t fl;
  uint8_t ip;
  uint16_t v;
  int t;
  uint32_t t1;
  t = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x2c)));
  /* guest stores the guest addr s0+0x7c (inline buf cursor) at +0x78;
   * script is the host mirror of that guest struct. */
  script[0x1e] = (int)(guest_of(script) + 0x7c);
  v = 0;
  *(uint8_t *)(script + 0x1f) = 0;
  t1 = (uint32_t)(*(uint16_t *)(script + 2) >> 4);
  if (((*(char *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))== '\0') && (*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctx + 200) ))+ 4) ))== 4)) ||
     ((*(char *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 2) ))== '\x01' && (*(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((ctx + 200) ))+ 6) ))== 4)))) {
    v = 1;
  }
  *(uint16_t *)((intptr_t)script + 0x6e) = v;
  script[0x1c] = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)));
  t0 = *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x6c)));
  *(uint16_t *)((intptr_t)script + 6) = 0;
  script[0x1d] = t0;
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
      *(uint16_t *)((intptr_t)script + 6) = 0x400;
      goto LAB_801824f8;
    }
    ph = *(short *)((intptr_t)script + 0x2a);
    if (ph == 0) {
      ip = *(uint8_t *)(script + 1);
      *(uint8_t *)(script + 1) = ip + 1;
      v = 0xffff;
      if (0x3f < *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip)))) {
        v = 1;
      }
      *(uint16_t *)((intptr_t)script + 0x2a) = v;
LAB_80182908:
      ip = *(uint8_t *)(script + 1);
      *(uint8_t *)(script + 1) = ip + 1;
      if ((uint8_t)(&DAT_801b52b8)[t1] < *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip))))
      goto switchD_80181ff0_caseD_9;
    }
    else {
      if (0 < ph) {
        if (ph != 1) goto switchD_80181ff0_caseD_9;
        goto LAB_80182908;
      }
      if (ph != -1) goto switchD_80181ff0_caseD_9;
    }
    if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd2) ))& 0x10) != 0) {
      *(uint16_t *)((intptr_t)script + 6) = 0x8400;
      goto switchD_80181ff0_caseD_9;
    }
    goto LAB_80182e9c;
  case 7:
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x2000000) != 0) {
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x4000000) != 0) {
        *(uint16_t *)((intptr_t)script + 0x2a) = 0;
      }
      if ((*(short *)((intptr_t)script + 0x2a) == 0) &&
         (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))<= *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x86))))) {
        ip = *(uint8_t *)(script + 1);
        *(uint8_t *)(script + 1) = ip + 1;
        if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))< (uint8_t)(&DAT_801b5288)[t1]) &&
           ((-1 < *(int *)guest_at((uint32_t)(uintptr_t)((t + 0x58) ))||
            (*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x86) ))* 5 < (int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0xba))))))) {
          *(uint8_t *)(script + 1) = ip + 2;
          v = *(uint16_t *)
                    (&DAT_801b5370 + (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)(uint8_t)(ip + 1)) ))& 7) * 2);
          *(uint16_t *)(script + 10) = 0;
          *(uint16_t *)((intptr_t)script + 6) = v;
        }
        *(uint16_t *)((intptr_t)script + 0x2a) = 0xffff;
      }
      goto switchD_80181ff0_caseD_9;
    }
    goto LAB_801824f8;
  case 8:
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x60) ))& 0x8000) == 0) {
LAB_80182a6c:
      script_op_dispatch(ctrl_g,pad,(int)guest_of(script),ctx);
    }
  case 9:
    goto switchD_80181ff0_caseD_9;
  default:
    switch((int)(((*(uint16_t *)(script + 10) & 0x47) - 0x40) * 0x10000) >> 0x10) {
    case 0:
      break;
    case 1:
      if (*(int *)guest_at((uint32_t)(uintptr_t)((t + 0x58) ))< 0) {
        *(uint16_t *)((intptr_t)script + 0x2a) = 4;
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x60000000) != 0) {
          *(uint16_t *)(script + 10) = *(uint16_t *)(script + 10) | 8;
        }
      }
      else if (*(short *)((intptr_t)script + 0x2a) < 1) {
        *(uint16_t *)(script + 10) = *(uint16_t *)(script + 10) & 0xfff7;
      }
      else {
        *(short *)((intptr_t)script + 0x2a) = *(short *)((intptr_t)script + 0x2a) + -1;
      }
      break;
    case 2:
      v = 0x4000;
      goto LAB_80182d20;
    case 3:
      v = 0x1000;
LAB_80182d20:
      *(uint16_t *)((intptr_t)script + 6) = v;
      break;
    case 4:
      *(uint16_t *)(script + 10) = *(uint16_t *)(script + 10) | 0x18;
      if (((((*(int *)guest_at((uint32_t)(uintptr_t)((t + 0x58) ))< 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x76) ))< 0x400)) &&
           (0xb < mips_div_lo((int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0xba) )),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x88)))))) && /* guest div @0x80182B00 */
          ((*(short *)guest_at((uint32_t)(uintptr_t)((t + 0xbc) ))< 1 && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x40) != 0)))) &&
         (ip = *(uint8_t *)(script + 1), *(uint8_t *)(script + 1) = ip + 1,
         (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))& 3) == 0)) goto LAB_80182b6c;
      if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) != 0) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))< 0x61))
         && (ip = *(uint8_t *)(script + 1), *(uint8_t *)(script + 1) = ip + 1,
            *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))< 0x15)) goto LAB_80182d94;
      *(uint16_t *)((intptr_t)script + 10) = *(uint16_t *)((intptr_t)script + 10) | 1;
      t0 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x74)));
      if (t0 < 0) {
        t0 = -t0;
      }
      if ((0x6aa < t0) || (0x200 < t0)) goto LAB_80182df0;
      if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((t + 0x58)))) goto switchD_80181ff0_caseD_9;
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x2000000) != 0) goto LAB_80182c28;
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) == 0) {
        if (0x3ff < *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x76)))) goto switchD_80181ff0_caseD_9;
        t1 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100)));
        if (((t1 & 0x40) == 0) && ((t1 & 0x180) != 0x100)) {
          if ((t1 & 0x20000) == 0) {
            *(uint16_t *)((intptr_t)script + 6) = 0;
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
    if (((((*(uint16_t *)(script + 10) >> 4 & 1) != 0) &&
         ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) != 0)) && (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))< 0x61)) &&
       (ip = *(uint8_t *)(script + 1), *(uint8_t *)(script + 1) = ip + 1,
       *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))< 0x15)) goto LAB_80182d94;
    if ((*(uint16_t *)(script + 10) >> 3 & 1) == 0) {
      t2 = *(uint16_t *)((intptr_t)script + 10) & 0xfffe;
      goto LAB_80182ee0;
    }
    *(uint16_t *)((intptr_t)script + 10) = *(uint16_t *)((intptr_t)script + 10) | 1;
    t0 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x74)));
    if (t0 < 0) {
      t0 = -t0;
    }
    if ((0x6aa < t0) || (0x200 < t0)) {
LAB_80182df0:
      *(uint16_t *)((intptr_t)script + 6) = 0xc000;
      goto switchD_80181ff0_caseD_9;
    }
    if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((t + 0x58)))) goto switchD_80181ff0_caseD_9;
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x2000000) == 0) {
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c) ))& 0x40000000) != 0) goto LAB_80182118;
      if (0x3ff < *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x76)))) goto switchD_80181ff0_caseD_9;
      if ((((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x40) == 0) && ((*(uint16_t *)(script + 10) & 0x47) != 0x42))
         && (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x180) != 0x100 ||
             ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) == 0)))) {
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x20000) == 0) {
          *(uint16_t *)((intptr_t)script + 6) = 0;
        }
        else {
LAB_80182ec4:
          *(uint16_t *)((intptr_t)script + 6) = 0x400;
        }
        goto switchD_80181ff0_caseD_9;
      }
      t1 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100)));
joined_r0x80182c88:
      if ((t1 & 0x20000) != 0) {
LAB_80182e9c:
        *(uint16_t *)((intptr_t)script + 6) = 0x4400;
        goto switchD_80181ff0_caseD_9;
      }
      goto LAB_80182ea8;
    }
LAB_80182c28:
    if (*(short *)((intptr_t)script + 6) != 0) goto switchD_80181ff0_caseD_9;
    goto LAB_80182118;
  }
  ip = *(uint8_t *)(script + 1);
  tp = (uint16_t *)guest_at((uint32_t)(uintptr_t)((script[4] + t1 * 4)));
  t2 = tp[1];
  lo = *tp;
  *(uint8_t *)(script + 1) = ip + 1;
  t0 = (((uint32_t)t2 - (uint32_t)lo) + 1) * (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip)));
  if (t0 < 0) {
    t0 = t0 + 0xff;
  }
  t2 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((script[4] + t1 * 4)));
  script[0xc] = -1;
  *(uint16_t *)(script + 10) = 1;
  *(uint16_t *)((intptr_t)script + 10) = *(uint16_t *)((intptr_t)script + 10) | 1;
  t0 = (uint32_t)t2 + (t0 >> 8);
  *(short *)((intptr_t)script + 0x2a) = (short)t0;
  t0 = t0 * 0x10000;
  *(short *)(script + 0xb) = (short)(((t0 >> 0x10) - (t0 >> 0x1f)) >> 1);
switchD_80181ff0_caseD_1:
  fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58)));
  if ((fl & 0xc0000000) != 0) goto switchD_80181ff0_caseD_9;
  if ((fl & 0x10000000) == 0) {
    if ((fl & 0x8000) != 0) {
      script[0xc] = 0;
      goto LAB_80182544;
    }
    t0 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x74)));
    if (t0 < 0) {
      t0 = -t0;
    }
    if (0x6aa < t0) {
      script[0xc] = 1;
      goto LAB_80182544;
    }
    if (0x200 < t0) {
      script[0xc] = 2;
      goto LAB_80182544;
    }
    ph = *(short *)((intptr_t)script + 0x2a);
    if ((*(short *)((intptr_t)script + 0x2a) == 0) ||
       (*(short *)((intptr_t)script + 0x2a) = ph + -1, ph == 1)) {
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x60) ))& 0x8000) == 0) {
        script[0xc] = 3;
        goto LAB_80182544;
      }
      goto switchD_80181ff0_caseD_9;
    }
    fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 0x58)));
    if (-1 < (int)fl) {
      script[0xc] = 6;
      t1 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((script[6] + 0x48)));
      while (t1 != 0) {
        if ((fl & t1) == t1) {
          if (*(short *)((intptr_t)script + 0x2a) <= (short)script[0xb]) goto LAB_80182544;
          if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
            *(uint16_t *)((intptr_t)script + 6) = 0x4000;
          }
          script[0xc] = -1;
          *(uint16_t *)(script + 10) = 1;
          goto switchD_80181ff0_caseD_9;
        }
        t = script[0xc];
        script[0xc] = t + 1;
        t1 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(((t + 1) * 0xc + script[6])));
      }
      if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
        *(uint16_t *)((intptr_t)script + 6) = 0x4000;
      }
      script[0xc] = -1;
      goto switchD_80181ff0_caseD_9;
    }
    if (0 < *(short *)guest_at((uint32_t)(uintptr_t)((t + 0xbc)))) {
      script[0xc] = 5;
      goto LAB_80182544;
    }
    ip = *(uint8_t *)(script + 1);
    *(uint8_t *)(script + 1) = ip + 1;
    if (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))< *(uint8_t *)guest_at((uint32_t)(uintptr_t)((script[7] + t1)))) {
      *(uint8_t *)(script + 1) = ip + 2;
      if (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((script[8] + t1) ))<= *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)(uint8_t)(ip + 1))))) {
        do {
          script[0xc] = 4;
LAB_80182544:
          ph = *(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x76)));
          script[0xd] = 0;
          t2 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(script[5]));
          while ((int)(uint32_t)t2 < (int)ph) {
            t = script[0xd];
            script[0xd] = t + 1;
            t2 = ((uint16_t *)guest_at((uint32_t)(uintptr_t)(script[5])))[t + 1];
          }
          script[9] = (uint32_t)*(uint16_t *)guest_at((uint32_t)(uintptr_t)((script[0xc] * 0xc + script[6] + script[0xd] * 2 + 4) ))+
                       *(int *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x30)));
          if ((*(short *)((intptr_t)script + 0x6e) != 0) &&
             ((script[0x1e] + -0x7c) - (int)guest_of(script) < 0xbb)) {
            t = sprintf_core((char *)guest_at((uint32_t)script[0x1e]),s__1___02d__1x_0_80123264,script[0xc],script[0xd])
            ;
            script[0x1e] = script[0x1e] + t;
          }
          ip = *(uint8_t *)(script + 1);
          *(uint8_t *)(script + 1) = ip + 1;
          o0 = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip)));
          *(uint8_t *)(script + 1) = ip + 2;
          o1 = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)(uint8_t)(ip + 1))));
          *(uint8_t *)(script + 1) = ip + 3;
          t = (0x100 - (uint32_t)*(uint16_t *)(script + 2)) *
                   ((uint32_t)o0 + (uint32_t)o1 + (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)(uint8_t)(ip + 2))))
                   );
          if (t < 0) {
            t = t + 0x1fff;
          }
          *(short *)((intptr_t)script + 0x2a) = (short)(t >> 0xd);
          *(uint16_t *)(script + 0x1b) = 0;
          *(uint16_t *)(script + 10) = 4;
switchD_80181ff0_caseD_4:
          t1 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x8000;
          if ((t1 == 0) || ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x30000) == 0)) {
            v = 6;
            if (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xd2) ))!= 0) {
LAB_80182768:
              *(uint16_t *)(script + 10) = v;
              *(uint16_t *)((intptr_t)script + 0x2a) = 0;
              goto switchD_80181ff0_caseD_9;
            }
            if (((script[0x1c] & 0x8000U) == 0) && (t1 != 0)) goto LAB_801824f8;
            if (*(short *)((intptr_t)script + 0x2a) == 0) goto LAB_80182a6c;
            *(short *)((intptr_t)script + 0x2a) = *(short *)((intptr_t)script + 0x2a) + -1;
            goto switchD_80181ff0_caseD_9;
          }
          ip = *(uint8_t *)(script + 1);
          *(uint16_t *)(script + 10) = 5;
          *(uint8_t *)(script + 1) = ip + 1;
          if (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))< 0x20) {
            t2 = *(uint16_t *)((intptr_t)script + 10) | 4;
          }
          else {
            t2 = *(uint16_t *)((intptr_t)script + 10) & 0xfffb;
          }
          *(uint16_t *)((intptr_t)script + 10) = t2;
switchD_80181ff0_caseD_5:
          if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x2000000) != 0) &&
             (v = 7, (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x68) ))& 0x800000) != 0)) goto LAB_80182768;
        } while ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x30000) == 0);
        if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x40000) != 0) || (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xc0) ))< 0))
        goto switchD_80181ff0_caseD_9;
        if (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xc0) ))== 0) {
          ip = *(uint8_t *)(script + 1);
          *(uint8_t *)(script + 1) = ip + 1;
          ip = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip)));
          if (ip < 0x30) {
LAB_80182d94:
            *(uint16_t *)((intptr_t)script + 6) = 0x2000;
            goto switchD_80181ff0_caseD_9;
          }
          if (0x5f < ip) {
            if (ip < 0x90) {
LAB_80182b6c:
              *(uint16_t *)((intptr_t)script + 6) = 0x1000;
            }
            else if (ip < 0xc0) {
              *(uint16_t *)((intptr_t)script + 6) = 0x4000;
            }
            goto switchD_80181ff0_caseD_9;
          }
        }
        else {
          ip = *(uint8_t *)(script + 1);
          *(uint8_t *)(script + 1) = ip + 1;
          ip = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip)));
          if ((uint32_t)((int)(*(uint16_t *)(script + 2) + 0x20) >> 1) <= (uint32_t)ip)
          goto switchD_80181ff0_caseD_9;
          if ((*(uint16_t *)((intptr_t)script + 10) & 4) != 0) {
            *(uint16_t *)((intptr_t)script + 6) = 0x40;
            goto switchD_80181ff0_caseD_9;
          }
          if ((ip & 1) == 0) {
LAB_80182ea8:
            *(uint16_t *)((intptr_t)script + 6) = 0x4000;
            goto switchD_80181ff0_caseD_9;
          }
        }
        *(uint16_t *)((intptr_t)script + 6) = 0x8000;
        goto switchD_80181ff0_caseD_9;
      }
      goto LAB_801823b8;
    }
    *(uint16_t *)(script + 10) = 2;
switchD_80181ff0_caseD_2:
    if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 0x58) ))& 0x8000) != 0) || (*(short *)guest_at((uint32_t)(uintptr_t)((t + 0xb8) ))== 0)) {
      *(uint16_t *)(script + 10) = 0;
    }
    if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x800) != 0) {
      t2 = *(uint16_t *)((intptr_t)script + 10) | 1;
LAB_80182ee0:
      *(uint16_t *)((intptr_t)script + 10) = t2;
      goto switchD_80181ff0_caseD_9;
    }
    *(uint16_t *)((intptr_t)script + 10) = *(uint16_t *)((intptr_t)script + 10) & 0xfffe;
    if ((0x400 < *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x76)))) || ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x80) == 0))
    goto switchD_80181ff0_caseD_9;
    t = mips_div_lo((int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0xba) )),(int32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x88)))); /* guest div @0x8018235C */
    if (t == 5) {
      ip = *(uint8_t *)(script + 1);
      *(uint8_t *)(script + 1) = ip + 1;
      if (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))< 0x40) {
        *(uint16_t *)((intptr_t)script + 6) = 0x4000;
      }
      goto switchD_80181ff0_caseD_9;
    }
    if (4 < t) goto switchD_80181ff0_caseD_9;
  }
  else if ((((*(int *)guest_at((uint32_t)(uintptr_t)((t + 0x58) ))< 0) &&
            (*(short *)guest_at((uint32_t)(uintptr_t)((t + 0xba) ))<= *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x88))))) &&
           (-1 < *(short *)guest_at((uint32_t)(uintptr_t)((t + 0xba))))) &&
          (ip = *(uint8_t *)(script + 1), *(uint8_t *)(script + 1) = ip + 1,
          *(uint8_t *)guest_at((uint32_t)(uintptr_t)((*script + (uint32_t)ip) ))< *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t1 + script[7]))))) {
LAB_801823b8:
    *(uint16_t *)(script + 10) = 3;
switchD_80181ff0_caseD_3:
    fl = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x5c)));
    if ((fl & 0x2000000) == 0) {
      if ((fl & 0x40000000) != 0) goto LAB_80182458;
      if ((*(short *)guest_at((uint32_t)(uintptr_t)((t + 0x76) ))< 0x400) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x40) != 0)) {
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x20000) == 0) {
          *(uint16_t *)((intptr_t)script + 6) = 0x4000;
        }
        else {
          *(uint16_t *)((intptr_t)script + 6) = 0x4400;
        }
      }
      else if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 100) ))& 0x20000) == 0) {
        *(uint16_t *)((intptr_t)script + 6) = 0;
      }
      else {
        *(uint16_t *)((intptr_t)script + 6) = 0x400;
      }
    }
    else {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0xcc) ))< 0x61) {
        if ((fl & 0x80) == 0) {
          ip = *(uint8_t *)(script + 1);
          t0 = *script;
          *(uint8_t *)(script + 1) = ip + 1;
          tb = &DAT_801b5298;
        }
        else {
          ip = *(uint8_t *)(script + 1);
          t0 = *script;
          *(uint8_t *)(script + 1) = ip + 1;
          tb = &DAT_801b52a8;
        }
        if (*(uint8_t *)guest_at((uint32_t)(uintptr_t)((t0 + (uint32_t)ip) ))< (uint8_t)tb[t1]) {
          *(uint16_t *)((intptr_t)script + 6) = 0x2000;
        }
      }
      if (*(short *)((intptr_t)script + 6) == 0) {
LAB_80182458:
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
          *(uint16_t *)((intptr_t)script + 6) = 0x4000;
        }
      }
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((t + 0xb8) ))== 0) {
LAB_801824f8:
      *(uint16_t *)(script + 10) = 0;
    }
    goto switchD_80181ff0_caseD_9;
  }
LAB_80182118:
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((ctrl_g + 0x58) ))& 0x10) != 0) {
    *(uint16_t *)((intptr_t)script + 6) = 0x4000;
  }
switchD_80181ff0_caseD_9:
  if ((*(uint16_t *)((intptr_t)script + 6) & 0x400) != 0) {
    *(uint16_t *)((intptr_t)script + 6) = *(uint16_t *)((intptr_t)script + 6) | 8;
  }
  if ((*(uint16_t *)((intptr_t)script + 10) & 1) != 0) {
    *(uint16_t *)((intptr_t)script + 6) = *(uint16_t *)((intptr_t)script + 6) | 0x800;
  }
  return;

}

/* pt_box_radius_test @0x80145c90 — sphere/sphere overlap test: sum the
 * radii r0+r1 and compare squared distance between points p and q
 * (per-axis bounding reject first); returns inside flag. */
bool pt_box_radius_test(int r0,int r1,int *p,int *q)
{
  int r2;
  int ad;
  /* Guest vec3 slot: gte_op_a00428 reads/writes 3 contiguous u32
   * through vdif; the decompiler split the 12-uint8_t vec3 into
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
        gte_op_a00428((uint32_t *)vdif,(uint32_t *)vdif);
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
/* always_one @0x80137570 — task callback returning constant 1. */
uint32_t always_one(int unused)
{
  (void)unused;
  return 1;

}

/* triple_track_spawn @0x80138a24 — spawn a triple-slot angle tracker
 * (slot_triple_*) bound to table index tid with initial value val. */
void triple_track_spawn(int idx,uint16_t val,int tid)
{
  uint16_t uv;
  int task;
  uint16_t *src;
  uint32_t *dst;
  if (DAT_1f8000b1 == '\0') {
    if (*(int *)(&DAT_801f0740 + idx * 4) != 0) {
      task_node_free((int *)(uintptr_t)*(int *)(&DAT_801f0740 + idx * 4));
    }
    task = (int)(intptr_t)task_node_alloc(1,2,8,0x8013885c,0,0x10);
    *(int *)(&DAT_801f0740 + idx * 4) = task;
    if (task != 0) {
      dst = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((task + 0x10))));
      /* Table @0x801b1e8c holds 4-uint8_t GUEST pointers to u16-pair data.
       * g_PTR_DAT_801b1e8c is uint8_t** so [&PTR][i] reads 8 bytes at i*8,
       * folding two guest entries into one bogus host ptr (SIGSEGV
       * 0x801B1DE4801B1DD4). Index as u32 and guest_at() the entry. */
      src = (uint16_t *)guest_at(((uint32_t *)&PTR_DAT_801b1e8c)[tid]);
      *(uint16_t *)((intptr_t)dst + 0xe) = val;
      *(short *)(dst + 2) = (short)idx;
      *(uint16_t *)(dst + 3) = 0;
      uv = *src;
      src = src + 1;
      *dst = (uint32_t)br2_gaddr(src);
      dst[1] = (uint32_t)br2_gaddr(src);
      *(uint16_t *)((intptr_t)dst + 10) = uv;
    }
  }
  return;

}

/* xform_accum_spawn @0x80138b00 — spawn an accumulated-transform FX
 * record for mode/lvl. */
void xform_accum_spawn(int mode,int lvl)
{
  uint32_t tmp;
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
  *(uint32_t *)rec = _DAT_1f80031c;
  *(uint32_t *)(rec + 2) = tmp;
  tmp = _DAT_1f800320;
  *(uint32_t *)(rec + 4) = _DAT_1f80031c;
  *(uint32_t *)(rec + 6) = tmp;
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

/* impact_fx_spawn @0x80139fd0 — spawn an impact FX record: allocate the
 * node, fill motion vec/color vec/scalars from pos/fid/ang/aux/fl. */
void impact_fx_spawn(uint32_t *pos,uint32_t fid,int ang,int aux,uint32_t fl)
{
  uint16_t hw;
  uint16_t rw;
  short dz;
  int t0;
  int t3;
  uint32_t r;
  uint32_t r2;
  uint8_t *tp;
  uint32_t *cur;
  int t1;
  short *mv;
  uint32_t py;
  uint16_t *cv;
  uint32_t *rec;
  if (DAT_1f8000b1 != '\0') {
    return;
  }
  t0 = always_one(1);
  if (t0 == 0) {
    return;
  }
  t0 = (int)(intptr_t)task_node_alloc(1,2,10,0x80138cec,0x80139e0c,
                       *(short *)(&DAT_801b1880 + (fid & 0xfffffff) * 2) + 0x68);
  if (t0 == 0) {
    return;
  }
  rec = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
  { extern uint32_t g_pu_raw,g_pu_guestof,g_pu_gbuf,g_pu_node,g_pu_count;
    uint32_t _gof = (uint32_t)guest_of(rec);
    uint32_t _gbuf = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10)));
    g_pu_count++;
    g_pu_raw=(uint32_t)(uintptr_t)rec; g_pu_guestof=_gof;
    g_pu_gbuf=_gbuf; g_pu_node=(uint32_t)t0;
  }
  if (((fid & 0xf0000000) == 0x40000000) || ((fid & 0xf0000000) != 0x80000000)) {
    py = pos[1];
    rec[0x16] = *pos;
    rec[0x17] = py;
  }
  else {
    *(uint16_t *)(rec + 0x16) = *(uint16_t *)pos;
    *(uint16_t *)((intptr_t)rec + 0x5a) = *(uint16_t *)(pos + 1);
    *(uint16_t *)(rec + 0x17) = *(uint16_t *)(pos + 2);
  }
  *(short *)(rec + 0x19) = (short)fid;
  rec[0x15] = (int)*(short *)(&DAT_801b18a8 + ((int)(fid << 0x10) >> 0xf));
  if (*(short *)(&DAT_801b1880 + *(short *)(rec + 0x19) * 2) == 0) {
    rec[0x18] = 0;
  }
  else {
    rec[0x18] = *(int *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10) ))+ 0x68;
  }
  t1 = 0;
  rec[0x14] = 1;
  *rec = 0x9000000;
  *(uint8_t *)((intptr_t)rec + 7) = 0x2c;
  rec[10] = 0x9000000;
  *(uint8_t *)((intptr_t)rec + 0x2f) = 0x2c;
  cur = rec;
  do {
    *(uint16_t *)((intptr_t)cur + 0xe) =
         *(uint16_t *)(&DAT_801b18d0 + *(short *)(rec + 0x19) * 2);
    hw = *(uint16_t *)(&DAT_801b1904 + *(short *)(rec + 0x19) * 2);
    t1 = t1 + 1;
    *(uint8_t *)(cur + 1) = 0x7f;
    *(uint8_t *)((intptr_t)cur + 5) = 0x7f;
    *(uint8_t *)((intptr_t)cur + 6) = 0x7f;
    *(uint8_t *)((intptr_t)cur + 7) = *(uint8_t *)((intptr_t)cur + 7) & 0xfe | 2;
    *(uint16_t *)((intptr_t)cur + 0x16) = hw;
    t3 = _DAT_801c4578;
    cur = cur + 10;
  } while (t1 < 2);
  switch(*(short *)(rec + 0x19)) {
  case 0:
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(rec + 0x14), (uint32_t)br2_gaddr(&DAT_801b1b1c), (uint32_t)br2_gaddr(&DAT_801b1ad8));
    if (t1 != 0) {
      if (ang < 10) {
        rec[0x15] = 0xa4;
        return;
      }
      if (ang < 0xf) {
        rec[0x15] = 200;
        return;
      }
      rec[0x15] = 0x104;
      return;
    }
    break;
  case 1:
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(rec + 0x14), (uint32_t)br2_gaddr(&DAT_801b1b78), (uint32_t)br2_gaddr(&DAT_801b1b5c));
    if (t1 != 0) {
      cv = (uint16_t *)guest_at((uint32_t)(uintptr_t)(rec[0x18]));
      hw = rand_lcg();
      t0 = 0;
      *cv = hw;
      cv[1] = 0x29a;
      do {
        *(uint8_t *)(rec + 1) = 0xc0;
        *(uint8_t *)((intptr_t)rec + 5) = 0xc0;
        *(uint8_t *)((intptr_t)rec + 6) = 0xc0;
        t0 = t0 + 1;
        rec = rec + 10;
      } while (t0 < 2);
      return;
    }
    break;
  case 2:
  case 3:
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(rec + 0x14), (uint32_t)br2_gaddr(&DAT_801b1a74), (uint32_t)br2_gaddr(&DAT_801b1a50));
    if (t1 != 0) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[0x18] ))= (uint32_t)guest_of(pos);
      r = rand_lcg();
      t0 = 0;
      rec[0x15] = (r & 0x7f) + 0x60;
      do {
        *(uint8_t *)(rec + 1) = 0xff;
        *(uint8_t *)((intptr_t)rec + 5) = 0xff;
        *(uint8_t *)((intptr_t)rec + 6) = 0xff;
        if (ang != 0) {
          *(uint16_t *)((intptr_t)rec + 0xe) = ((short)ang + 0x1e0U & 0x1ff) * 0x40 + 0x1a;
        }
        t0 = t0 + 1;
        rec = rec + 10;
      } while (t0 < 2);
      return;
    }
    break;
  case 4:
  case 9:
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(rec + 0x14), (uint32_t)br2_gaddr(&DAT_801b1ab8), (uint32_t)br2_gaddr(&DAT_801b1a94));
    if (t1 != 0) {
      return;
    }
    break;
  case 5:
    cur = rec + 0x14;
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(cur), (uint32_t)br2_gaddr(&DAT_801b1bb4), (uint32_t)br2_gaddr(&DAT_801b1b90));
    if (t1 != 0) {
      cv = (uint16_t *)guest_at((uint32_t)(uintptr_t)(rec[0x18]));
      rec[0x15] = (int)fl >> 0xf;
      r = rand_lcg();
      t1 = (int)((fl & 0xffff) * ((r & 0x7ff) + 0x400)) >> 0xc;
      r = rand_lcg();
      t3 = ang + -0x100 + (r & 0x1ff);
      r = rand_lcg();
      t0 = lut_cos_q12(t3);
      *cv = (short)(t0 * t1 >> 0xb);
      t0 = lut_sin_q12(aux + -0x100 + (r & 0x1ff));
      cv[1] = (short)(t0 * t1 >> 0xb);
      t0 = lut_sin_q12(t3);
      cv[2] = (short)(t0 * t1 >> 0xb);
      do {
        if ((_DAT_801c4578 != 0) && (_DAT_801c4578 != 3)) {
          *(uint8_t *)((intptr_t)rec + 7) = *(uint8_t *)((intptr_t)rec + 7) & 0xfd;
        }
        *(uint8_t *)(rec + 1) = 0xa0;
        *(uint8_t *)((intptr_t)rec + 5) = 0xff;
        *(uint8_t *)((intptr_t)rec + 6) = 100;
        rec = rec + 10;
      } while ((intptr_t)rec < (intptr_t)cur);
      return;
    }
    break;
  case 6:
    t0 = 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec[0x18] ))= 0x20;
    cur = rec;
    do {
      *(uint8_t *)(cur + 1) = 0xa0;
      *(uint8_t *)((intptr_t)cur + 5) = 0xff;
      *(uint8_t *)((intptr_t)cur + 6) = 100;
      *(uint8_t *)(cur + 3) = 0xe0;
      *(uint8_t *)((intptr_t)cur + 0xd) = 0xb0;
      *(uint8_t *)(cur + 5) = 0xef;
      *(uint8_t *)((intptr_t)cur + 0x15) = 0xb0;
      *(uint8_t *)(cur + 7) = 0xe0;
      *(uint8_t *)((intptr_t)cur + 0x1d) = 0xbf;
      *(uint8_t *)(cur + 9) = 0xef;
      *(uint8_t *)((intptr_t)cur + 0x25) = 0xbf;
      t0 = t0 + 1;
      cur = cur + 10;
    } while (t0 < 2);
    if (_DAT_801c4578 == 0) {
      return;
    }
    if (_DAT_801c4578 == 3) {
      return;
    }
    rec[0x15] = 0x96;
    return;
  case 7:
    mv = (short *)guest_at((uint32_t)(uintptr_t)(rec[0x18]));
    mv[4] = 0x20;
    py = rec[0x17];
    *(uint32_t *)mv = rec[0x16];
    *(uint32_t *)(mv + 2) = py;
    if ((t3 != 0) && (t3 != 3)) {
      rec[0x15] = 0x5a;
    }
    if (ang == 1) {
      dz = mv[2] - *(short *)(rec + 0x15);
    }
    else {
      if (ang < 2) {
        if (ang == 0) {
          *mv = *mv - *(short *)(rec + 0x15);
        }
        goto LAB_8013a628;
      }
      if (ang == 2) {
        *mv = *mv + *(short *)(rec + 0x15);
        goto LAB_8013a628;
      }
      if (ang != 3) goto LAB_8013a628;
      dz = mv[2] + *(short *)(rec + 0x15);
    }
    mv[2] = dz;
LAB_8013a628:
    t0 = 0;
    mv[1] = mv[1] + *(short *)(rec + 0x15);
    do {
      *(uint8_t *)(rec + 1) = 0xa0;
      *(uint8_t *)((intptr_t)rec + 5) = 0xff;
      *(uint8_t *)((intptr_t)rec + 6) = 100;
      *(uint8_t *)(rec + 3) = 0xe0;
      *(uint8_t *)((intptr_t)rec + 0xd) = 0xb0;
      *(uint8_t *)(rec + 5) = 0xef;
      *(uint8_t *)((intptr_t)rec + 0x15) = 0xb0;
      *(uint8_t *)(rec + 7) = 0xe0;
      *(uint8_t *)((intptr_t)rec + 0x1d) = 0xbf;
      *(uint8_t *)(rec + 9) = 0xef;
      *(uint8_t *)((intptr_t)rec + 0x25) = 0xbf;
      t0 = t0 + 1;
      rec = rec + 10;
    } while (t0 < 2);
    return;
  case 8:
  case 0x12:
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(rec + 0x14), (uint32_t)br2_gaddr(&DAT_801b1c30), (uint32_t)br2_gaddr(&DAT_801b1bfc));
    if (t1 != 0) {
      t3 = task_alloc_wrap_c((uint32_t)br2_gaddr((uint8_t *)rec + 0xe),(uint32_t)br2_gaddr((uint8_t *)rec + 0x36),(int)guest_of(&DAT_801b1c60),(int)guest_of(&DAT_801b1c74));
      if (t3 != 0) {
        if (*(short *)(rec + 0x19) != 0x12) {
          return;
        }
        cv = (uint16_t *)guest_at((uint32_t)(uintptr_t)(rec[0x18]));
        t0 = lut_cos_q12(aux);
        *cv = (short)(t0 >> 7);
        t0 = lut_sin_q12(ang);
        cv[1] = (short)(t0 >> 9);
        t0 = lut_sin_q12(aux);
        cv[2] = (short)(t0 >> 7);
        return;
      }
      task_node_free((int *)(uintptr_t)t1);
    }
    break;
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
    /* guest: lw aux, (0x801B19D4 + (idx-10)*4) — u32 table of guest addrs,
     * passed raw to task_alloc_wrap_d. g_PTR_DAT_801b19d4 is uint8_t** so
     * [&PTR][i] would read 8 bytes at i*8; index as u32 instead. */
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(rec + 0x14),(uint32_t)((uint32_t *)&PTR_DAT_801b19d4)[*(short *)(rec + 0x19) + -10], (uint32_t)br2_gaddr(&DAT_801b192c));
    if (t1 != 0) {
      cv = (uint16_t *)guest_at((uint32_t)(uintptr_t)(rec[0x18]));
      r = rand_lcg();
      t1 = (r & 0xc63) + 0x200;
      r = rand_lcg();
      r2 = rand_lcg();
      t3 = ang + -0x400 + (r2 & 0x7ff);
      r2 = rand_lcg();
      t0 = lut_cos_q12(t3);
      t0 = (t0 >> 7) * t1;
      if (t0 < 0) {
        t0 = t0 + 0xfff;
      }
      *cv = (short)(t0 >> 0xc);
      t0 = lut_sin_q12(t3);
      t1 = (t0 >> 7) * t1;
      if (t1 < 0) {
        t1 = t1 + 0xfff;
      }
      cv[2] = (short)(t1 >> 0xc);
      if (fl == 0x10) {
        rw = rand_lcg();
        dz = (rw & 0x3f) + 0x20;
      }
      else {
        t0 = lut_sin_q12(aux + -0x400 + (r2 & 0x7ff));
        t0 = (t0 >> 7) * ((r & 0xc63) + 0x200);
        if (t0 < 0) {
          t0 = t0 + 0xfff;
        }
        dz = (short)(t0 >> 0xc);
      }
      cv[1] = dz;
      t0 = 0;
      do {
        t0 = t0 + 1;
        hw = *(uint16_t *)(&DAT_801b18d0 + ((uint8_t)(&DAT_801b18f8)[DAT_1f800098] + 10) * 2);
        *(uint8_t *)((intptr_t)rec + 7) = *(uint8_t *)((intptr_t)rec + 7) & 0xfd;
        *(uint16_t *)((intptr_t)rec + 0xe) = hw;
        rec = rec + 10;
      } while (t0 < 2);
      return;
    }
    break;
  case 0xe:
  case 0xf:
    if (*(short *)(rec + 0x19) == 0xe) {
      tp = &DAT_801b1a18;
    }
    else {
      tp = &DAT_801b1a3c;
    }
    t1 = task_alloc_wrap_d((uint32_t)br2_gaddr(rec),(uint32_t)br2_gaddr(rec + 10),(uint32_t)br2_gaddr(rec + 0x14), (uint32_t)br2_gaddr(&DAT_801b1a2c),(uint32_t)br2_gaddr(tp));
    if (t1 != 0) {
      if (*(short *)(rec + 0x19) != 0xe) {
        return;
      }
      cv = (uint16_t *)guest_at((uint32_t)(uintptr_t)(rec[0x18]));
      t0 = lut_cos_q12(ang);
      t0 = (t0 >> 5) * fl;
      if (t0 < 0) {
        t0 = t0 + 0xfff;
      }
      *cv = (short)(t0 >> 0xc);
      t0 = lut_sin_q12(aux);
      t0 = (t0 >> 5) * fl;
      if (t0 < 0) {
        t0 = t0 + 0xfff;
      }
      cv[1] = (short)(t0 >> 0xc);
      t0 = lut_sin_q12(ang);
      t0 = (t0 >> 5) * fl;
      if (t0 < 0) {
        t0 = t0 + 0xfff;
      }
      cv[2] = (short)(t0 >> 0xc);
      t0 = rand_lcg();
      *(uint32_t *)(cv + 4) = t0 >> 8 & 1;
      *(uint32_t *)(cv + 6) = 0;
      return;
    }
    break;
  default:
    goto switchD_8013a1f8_caseD_10;
  }
  task_node_free((int *)(uintptr_t)t0);
switchD_8013a1f8_caseD_10:
  return;

}

/* fx_record_alloc @0x8013b088 — allocate an FX task node and initialize
 * its record payload: pos_g -> +8, fl -> +0x10/+0x18, aux -> +0x14,
 * +0xc = rec+0x1c. kind is only used as kind+0x1c for the alloc size;
 * pal_g is forwarded to task_alloc_wrap_e. */
int fx_record_alloc(uint32_t pos_g,int kind,uint32_t fl,uint32_t aux,uint32_t pal_g)
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
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x10) ))= fl & 0xfffffff;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x18) ))= fl & 0xf0000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x14) ))= aux;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 8) ))= pos_g;
      *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))= rec + 0x1c;
      /* rec is a guest address; fx_record_init dereferences pos_g as a
       * host pointer, so resolve it (a raw cast sign-extends KSEG0 addrs). */
      fx_record_init((uint32_t *)guest_at((uint32_t)rec));
      res = *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc)));
    }
  }
  return res;

}

/* emit_task_spawn @0x8013d6b0/spawn_b/quad_fx_spawn/group_fx_spawn —
 * task-node spawners for emit/quad/group FX variants; fill pay payload
 * and link into the task lists. */
void emit_task_spawn(int *pos,int kind,uint16_t x,short y,uint16_t p5, uint16_t p6)
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
      *(uint16_t *)(pay + 7) = 0;
      *(short *)((intptr_t)pay + 0x1e) = (short)kind;
      *(uint16_t *)(pay + 8) = 0;
      *(short *)(pay + 9) = 0x400 - y;
      *(uint16_t *)((intptr_t)pay + 0x22) = x;
      *(uint16_t *)((intptr_t)pay + 0x26) = p5;
      tmp = pos[1];
      pay[5] = *pos;
      pay[6] = tmp;
      *(uint16_t *)(pay + 10) = p6;
    }
  }
  return;

}

/* emit_task_spawn_b @0x8013ed98 — see emit_task_spawn. */
void emit_task_spawn_b(uint32_t *pos,uint32_t ea,uint32_t eb,uint32_t fl)
{
  int node;
  uint32_t *pay;
  uint32_t py;
  node = (int)(intptr_t)task_node_alloc(1,2,0xb,0x8013eb04,0,0x20);
  if (node != 0) {
    pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    py = pos[1];
    *pay = *pos;
    pay[1] = py;
    pay[5] = eb;
    pay[6] = ea;
    *(uint16_t *)(pay + 7) = 0;
    *(uint16_t *)((intptr_t)pay + 0x1e) = 0;
    pay[4] = fl;
  }
  return;

}

/* quad_fx_spawn @0x80141814 — see emit_task_spawn (quad variant). */
void quad_fx_spawn(int f,int kind,uint32_t *pos,uint16_t aux,uint16_t fl, int p6,int p7)
{
  int d0;
  int d1;
  int d2;
  int t0;
  int t;
  uint16_t *tp;
  uint16_t w;
  int *pay;
  if (DAT_1f8000b1 != '\0') {
    return;
  }
  t0 = (int)(intptr_t)task_node_alloc(1,2,7,0x80141610,0,0x78);
  if (t0 == 0) {
    return;
  }
  pay = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
  t = (int)(intptr_t)prim_group_build((uint32_t)br2_gaddr(pay + 1),*(uint32_t *)(&DAT_801c43a8 + kind * 4),0,1);
  pay[1] = t;
  if (t != 0) {
    t = tween_chan_alloc(t,0);
    if (t != 0) {
      pay[2] = 0;
      if (kind != 0x17) {
LAB_80141918:
        *pay = f;
        pay[3] = (intptr_t)pos;
        *(uint16_t *)(pay + 0x1c) = fl;
        *(short *)((intptr_t)pay + 0x72) = (short)p6;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x14) ))= *pos;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x18) ))= pos[1];
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x1c) ))= pos[2];
        if (p7 != 0) {
          prim_attr_word_set(pay[1],(p7 + 0x1e0U & 0x1ff) << 6 | 0x1a);
        }
        if (p6 == 1) {
          rotmat2_build(0xfffffc00,pay + 4);
          if (-1 < *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x20) ))+ 0x128)))) {
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x18) ))= (int)*(short *)guest_at((uint32_t)(uintptr_t)((f + 0x94)));
          }
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(pay[1] ))+ 0x18) ))= 0;
        }
        else if (p6 < 2) {
          if (p6 == 0) {
            angle_pair_rotmat((uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)((f + 0xdb) ))* -0x10 + 0xc00,
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
        *(uint16_t *)(pay + 0x14) = aux;
        *(uint16_t *)(pay + 0x16) = aux;
        pay[0x10] = d2;
        pay[0x18] = d2;
        *(uint16_t *)(pay + 0x18) = aux;
        if ((fl & 0x1000) == 0) {
          *(uint16_t *)(pay + 0xc) = DAT_801b2340;
          *(uint16_t *)(pay + 0xe) = DAT_801b2342;
          *(uint16_t *)(pay + 0x10) = DAT_801b2344;
          tp = &DAT_801b2348;
          w = DAT_801b2348;
        }
        else {
          *(uint16_t *)(pay + 0xc) = DAT_801b2330;
          *(uint16_t *)(pay + 0xe) = DAT_801b2332;
          *(uint16_t *)(pay + 0x10) = DAT_801b2334;
          tp = &DAT_801b2338;
          w = DAT_801b2338;
        }
        *(uint16_t *)(pay + 0x1d) = w;
        *(uint16_t *)((intptr_t)pay + 0x76) = tp[1];
        return;
      }
      t = task_alloc_wrap_f((uint32_t)(uintptr_t)pay[1],(uint32_t)br2_gaddr(pay + 2), (uint32_t)br2_gaddr(&DAT_801b1cbc), (uint32_t)br2_gaddr(&DAT_801b1ccc));
      pay[2] = t;
      if (t != 0) goto LAB_80141918;
    }
    node_tree_free(pay[1]);
  }
  task_node_free((int *)(uintptr_t)t0);
  return;

}

/* stage_env_task_spawn @0x8013C9CC — task-node spawner for the event-24
 * environment FX (sole caller ev24_fx_spawn @0x801421C4). Allocates a
 * 7200-byte payload task (cb 0x8013B72C), writes the header, builds two
 * 2496-byte regions of 48 52-byte GPU packet records, a prim group at
 * pay+5772, and 32 prim records at pay+5776. ctx_g is a guest pointer
 * into the 0x1F8000E8 scratch records; ctx[2]/ctx[4] select texture
 * and attribute variants. Faithful port of cps_8013c9cc. */
int stage_env_task_spawn(int32_t a0,uint32_t a1,uint32_t ctx_g, uint32_t a3)
{
  int *node;
  uint32_t node_g;
  uint32_t pay_g;
  uint8_t *pay;
  int ctx_b2;
  int ctx_b4;
  uint32_t texid;
  uint32_t s4v;
  uint32_t s5v;
  int i;
  int j;
  int r;
  int c;
  int n;
  int p;
  int q;
  uint8_t *reg;
  uint8_t *rec;
  uint8_t *cell;
  uint8_t *r2;
  uint32_t off;
  uint8_t *wp;
  uint32_t grp;
  uint32_t attr;
  int a1v;
  int a0v;

  if ((int8_t)DAT_1f8000b1 != 0) {
    return (int32_t)(int8_t)DAT_1f8000b1;
  }
  node = task_node_alloc(1,2,10,0x8013B72C,0,7200);
  if (node == (int *)0x0) {
    return 0;
  }
  node_g = (uint32_t)(intptr_t)node;
  pay_g = *(uint32_t *)guest_at(node_g + 16);
  pay = (uint8_t *)guest_at(pay_g);
  ctx_b2 = *(uint8_t *)guest_at(ctx_g + 2);
  ctx_b4 = *(uint8_t *)guest_at(ctx_g + 4);

  *(uint16_t *)(pay + 7192) = (uint16_t)a1;
  *(uint16_t *)(pay + 7194) = 0;
  *(uint16_t *)(pay + 7196) = 0;
  *(uint16_t *)(pay + 7198) = (uint16_t)a0;
  *(uint32_t *)(pay + 5768) = a3;
  *(uint32_t *)(pay + 5756) = 0xFFFFFFFFu;
  *(uint32_t *)(pay + 5764) = ctx_g;

  off = 4992;
  wp = pay + 5536;
  for (i = 0; i < 4; i++) {
    *(uint32_t *)wp = pay_g + off;
    wp += 4;
    off += 136;
  }

  for (i = 0; i < 2; i++) {
    reg = pay + i * 2496;
    texid = ctx_b4 != 0 ? 31002 : 31514;
    s4v = ctx_b2 != 0 ? 64 : 0;
    s5v = s4v | 1;
    for (j = 0; j < 48; j++) {
      rec = reg + j * 52;
      *(uint16_t *)(rec + 26) = 63;
      rec[12] = 32;
      rec[13] = (uint8_t)s4v;
      rec[24] = 33;
      rec[25] = (uint8_t)s4v;
      rec[36] = 32;
      rec[37] = (uint8_t)s5v;
      rec[48] = 33;
      rec[49] = (uint8_t)s5v;
      gpu_pkt_tag_0c3c(pay_g + i * 2496 + j * 52);
      rec[7] = (rec[7] & 0xFE) | 2;
      *(uint16_t *)(rec + 14) = (uint16_t)texid;
    }
    n = 0;
    for (r = 0; r < 3; r++) {
      a1v = 160 - 60 * r;
      a0v = 120 - 60 * r;
      for (c = 0; c < 16; c++) {
        cell = reg + n * 52;
        cell[4] = (uint8_t)a1v;
        cell[5] = (uint8_t)a1v;
        cell[6] = (uint8_t)a1v;
        cell[16] = (uint8_t)a0v;
        cell[17] = (uint8_t)a0v;
        cell[18] = (uint8_t)a0v;
        cell[28] = (uint8_t)a1v;
        cell[29] = (uint8_t)a1v;
        cell[30] = (uint8_t)a1v;
        cell[40] = (uint8_t)a0v;
        cell[41] = (uint8_t)a0v;
        cell[42] = (uint8_t)a0v;
        n++;
      }
    }
  }

  grp = (uint32_t)(intptr_t)prim_group_build(
      (uint32_t)br2_gaddr(pay + 5772),
      *(uint32_t *)(&DAT_801c43a8 + (ctx_b2 != 0 ? 240 : 160)),0,1);
  *(uint32_t *)(pay + 5772) = grp;
  if (grp == 0) {
    task_node_free(node);
    return 0;
  }
  if (ctx_b2 == 1) {
    fighter_chan_delta_accum(grp,0,64,0);
  }

  for (p = 0; p < 2; p++) {
    for (q = 0; q < 16; q++) {
      r2 = pay + 5776 + p * 640 + q * 40;
      *(uint16_t *)(r2 + 22) = 31;
      *(uint32_t *)r2 = 0x09000000u;
      r2[7] = 44;
      if (ctx_b2 == 0) {
        r2[12] = 32;
        r2[13] = 0;
        r2[20] = 39;
        r2[21] = 0;
        r2[28] = 32;
        r2[29] = 15;
        r2[36] = 39;
        r2[37] = 15;
        attr = ctx_b4 != 0 ? 30790 : 30726;
      }
      else {
        r2[13] = 64;
        r2[21] = 64;
        r2[12] = 32;
        r2[20] = 39;
        r2[28] = 32;
        r2[29] = 79;
        r2[36] = 39;
        r2[37] = 79;
        attr = ctx_b4 != 0 ? 30791 : 30727;
      }
      *(uint16_t *)(r2 + 14) = (uint16_t)attr;
      prim_attr_word_set((int)grp,(uint16_t)attr);
      r2[4] = 128;
      r2[5] = 128;
      r2[6] = 128;
      r2[7] = (r2[7] | 1) & 0xFD;
    }
  }
  return 0;

}

/* br2_waypoint_jal — faithful guest `jal` to an interrupt-check waypoint
 * (jr-$ra stub): saves ra/pc/sp, links ra, dispatches clean-first via
 * clean_try_dispatch else psx_dispatch_call, restores ra/pc and returns
 * gpr[2]. Same contract as bind.c's bind_jal, kept file-local. */
static uint32_t br2_waypoint_jal(uint32_t fn,uint32_t link)
{
  CPUState *cpu = clean_cpu_state();
  uint32_t ra,pc,sp;
  extern int br2_bail_pending(void);
  if (!cpu || fn == 0) return 0;
  if (br2_bail_pending()) return cpu->gpr[2];
  ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
  cpu->gpr[31] = link;
  if (clean_try_dispatch(cpu,fn)) {
    uint32_t r = cpu->gpr[2];
    cpu->gpr[31] = ra; cpu->pc = pc;
    return r;
  }
  psx_dispatch_call(cpu,fn,link);
  cpu->gpr[29] = sp;
  {
    uint32_t r = cpu->gpr[2];
    cpu->gpr[31] = ra; cpu->pc = pc;
    return r;
  }
}

int fighter_emit_state_run(uint32_t a0,uint32_t a1,uint32_t mat,const void *ctrl,int32_t mode)
{
    /* guest 0x801468EC — fighter emit state machine. Loads the 8-word
    * matrix at mat into GTE CT0..CT7, normalizes the mode index mode
    * through the 6-entry jr table at 0x801202CC ({0,4,2,6,1,7}), then
    * runs three bit-selected emit sections (bit0/bit1/bit2 of the
    * normalized mode) that walk 4-byte record lists hanging off the
    * FighterCtrl at ctrl and jal the 8-byte jr-$ra padding stubs
    * 0x80136F70/0x80136F80 (interrupt-check waypoints; the args are
    * dead but reproduced faithfully). Faithful notes:
    *  - guest frame is exactly 0x60; incoming arg5 (mode) is already
    *    marshalled at old_sp+0x10 by the dispatch wrapper; outgoing
    *    arg5/arg6 land at fr+0x10/0x14 and the 3-word vector at
    *    fr+0x18 is passed by guest pointer — all via br2_guest_frame.
    *  - the prologue spills s0-s7/fp/ra + the a0/a1 home slots into
    *    the frame so the stack image matches generated.
    *  - the ctc2 sequence mirrors generated: direct gte_ctrl[] stores
    *    for CT0-3 and CT5-7, gte_write_ctrl() for CT4 (16-bit reg). */
    CPUState *c = clean_cpu_state();
    uint32_t fr, s4, P, L2, T, fp, s0;
    int32_t s6, s2, s3, s5v, s7, ret = 0;
    if (!c) {
        /* no dispatch context: every guest-call bridge (br2_call_clean /
         * br2_call_generated / psx_dispatch_call) no-ops on a NULL cpu, and
         * a self-addressed call would recurse through this wrapper anyway. */
        return 0;
    }
    s4 = guest_of(ctrl);
    s6 = mode;
    c->gpr[4] = a0; c->gpr[5] = a1; c->gpr[6] = mat; c->gpr[7] = s4;
    fr = br2_guest_frame(0x60);
    br2_st32(fr + 0x38, c->gpr[16]);
    br2_st32(fr + 0x3c, c->gpr[17]);
    br2_st32(fr + 0x40, c->gpr[18]);
    br2_st32(fr + 0x44, c->gpr[19]);
    br2_st32(fr + 0x48, c->gpr[20]);
    br2_st32(fr + 0x4c, c->gpr[21]);
    br2_st32(fr + 0x50, c->gpr[22]);
    br2_st32(fr + 0x54, c->gpr[23]);
    br2_st32(fr + 0x58, c->gpr[30]);
    br2_st32(fr + 0x5c, c->gpr[31]);
    br2_st32(fr + 0x60, a0);
    br2_st32(fr + 0x64, a1);
    P  = br2_ld32(s4 + 0x2c); br2_st32(fr + 0x28, P);
    L2 = br2_ld32(s4 + 0x10); br2_st32(fr + 0x2c, L2);
    T  = br2_ld32(s4 + 0x14); br2_st32(fr + 0x30, T);
    fp = br2_ld32(s4 + 0x20);
    s0 = br2_ld32(s4 + 0x0c);

    /* GTE CT0..CT7 <- 8 matrix words at mat (generated order preserved) */
    {
        uint32_t m0 = br2_ld32(mat + 0x00), m1 = br2_ld32(mat + 0x04);
        uint32_t m2 = br2_ld32(mat + 0x08), m3 = br2_ld32(mat + 0x0c);
        uint32_t m4 = br2_ld32(mat + 0x10), m5 = br2_ld32(mat + 0x14);
        uint32_t m6 = br2_ld32(mat + 0x18), m7 = br2_ld32(mat + 0x1c);
        c->gte_ctrl[0] = m0; c->gte_ctrl[1] = m1;
        c->gte_ctrl[2] = m2; c->gte_ctrl[3] = m3;
        gte_write_ctrl(c, 4, m4);
        c->gte_ctrl[5] = m5; c->gte_ctrl[6] = m6; c->gte_ctrl[7] = m7;
    }
    /* jr-table mode normalize (table at 0x801202CC): input 0..5 ->
    * {0,4,2,6,1,7}; inputs >=6 keep the raw index. */
    if ((uint32_t)s6 < 6u) {
        static const int8_t mode_map[6] = {0, 4, 2, 6, 1, 7};
        s6 = mode_map[s6];
    }

    /* 0x801469E0: s6 != 0 -> waypoint call, arg5 = 26 */
    if (s6 != 0) {
        c->gpr[4] = a0; c->gpr[5] = a1;
        c->gpr[6] = fp + 0x34; c->gpr[7] = 0;
        br2_st32(fr + 0x10, 26);
        (void)br2_waypoint_jal(0x80136f70u, 0x80146a00u);
    }

    /* ---- bit0 section (0x80146A00..0x80146AE4) ---- */
    if (s6 & 1) {
        uint32_t f60 = br2_ld32(s4 + 0x60);
        if (!(f60 & 0x200000u)) {
            uint32_t f58 = br2_ld32(s4 + 0x58);
            int32_t single;
            /* flag pair on st_58, else on partner ctrl P+0x58:
            * "hit" = (f & 0x30000) && !(f & 0x8000000) */
            if ((f58 & 0x30000u) != 0 && (f58 & 0x08000000u) == 0) {
                single = 1;
            } else {
                uint32_t fP = br2_ld32(P + 0x58);
                single = (fP & 0x30000u) != 0 && (fP & 0x08000000u) == 0;
            }
            if (single) { s2 = 0; s3 = 1; }
            else        { s2 = 1; s3 = (int8_t)br2_ld8(s4 + 9); }
            while (s2 < s3) {
                uint32_t e = s0 + (uint32_t)s2 * 4u;
                int32_t idx = (int8_t)br2_ld8(e + 1);
                int32_t val = (int16_t)br2_ld16(e + 2);
                c->gpr[4] = a0; c->gpr[5] = a1;
                c->gpr[6] = fp + 0x34 + (uint32_t)((int32_t)idx * 0x50);
                c->gpr[7] = 2;
                br2_st32(fr + 0x10, (uint32_t)(val << 1));
                (void)br2_waypoint_jal(0x80136f70u, 0x80146ad8u);
                s2++;
            }
        }
    }

    /* ---- bit1 section (0x80146AE4..0x80146C24) ---- */
    if (((uint32_t)s6 >> 1) & 1u) {
        int8_t b27 = (int8_t)br2_ld8(s4 + 0x27);
        if (b27 >= 0) {
            uint32_t f58 = br2_ld32(s4 + 0x58);
            s7 = 0;
            if (f58 & 0x40000u) {
                s7 = 1;
            } else if (f58 & 0x30000u) {
                if (!(f58 & 0x08000000u)) {
                    uint32_t f5c = br2_ld32(s4 + 0x5c);
                    if ((f5c & 0x80000080u) != 0x80u) s7 = 1;
                }
            }
            {
                uint32_t p64 = br2_ld32(P + 0x64);
                s5v = (int32_t)(p64 & 0x1000u);
                s3 = (int8_t)br2_ld8(s4 + 0x0a);
                if (s7 == 0 || s5v != 0) {
                    if (s3 > 0) {
                        uint32_t cur = L2;
                        int32_t n = 0;
                        do {
                            int32_t idx = (int8_t)br2_ld8(cur + 1);
                            int32_t val = (int16_t)br2_ld16(cur + 2);
                            cur += 4; n++;
                            c->gpr[4] = a0; c->gpr[5] = a1;
                            c->gpr[6] = fp + 0x34 + (uint32_t)((int32_t)idx * 0x50);
                            c->gpr[7] = 1;
                            br2_st32(fr + 0x10, (uint32_t)(val << 1));
                            (void)br2_waypoint_jal(0x80136f70u, 0x80146bb8u);
                        } while (n < s3);
                    }
                }
            }
        }
        /* BC8: bit1 still set -> st_5c & 0x2000 gates the stub80 call */
        if (br2_ld32(s4 + 0x5c) & 0x2000u) {
            int32_t x = (((int32_t)(int16_t)br2_ld16(s4 + 0x4e) - 0x400) << 20) >> 20;
            int32_t y = (((int32_t)(int16_t)br2_ld16(s4 + 0xc4) - 0x400) << 20) >> 20;
            c->gpr[4] = a0; c->gpr[5] = a1;
            c->gpr[6] = fp + 0x34; c->gpr[7] = (uint32_t)y;
            br2_st32(fr + 0x10, 0x400);
            br2_st32(fr + 0x14, (uint32_t)x);
            (void)br2_waypoint_jal(0x80136f80u, 0x80146c24u);
            /* guest v0 is overwritten by (s6>>2)&1 at 0x80146C24 — ret stays 0 here */
        } else {
            ret = 0;
        }
    }

    /* ---- bit2 section (0x80146C24..0x80146E00) ---- */
    if (((uint32_t)s6 >> 2) & 1u) {
        int8_t b26 = (int8_t)br2_ld8(s4 + 0x26);
        if (b26 >= 0) {
            uint32_t f58 = br2_ld32(s4 + 0x58);
            s7 = 0;
            if (f58 & 0x40000u) {
                s7 = 1;
            } else if (f58 & 0x30000u) {
                if (!(f58 & 0x08000000u)) {
                    uint32_t f5c = br2_ld32(s4 + 0x5c);
                    if ((f5c & 0x80000080u) != 0x80u) s7 = 1;
                }
            }
            /* guest s2/s3 are 0 on every path reaching the scan loop */
            {
                uint32_t p64 = br2_ld32(P + 0x64);
                s5v = (int32_t)(p64 & 0x1000u);
                uint32_t cur = T;
                int32_t n;
                for (n = 0; n < 8; n++, cur += 4) {
                    int32_t cmd = (int8_t)br2_ld8(cur);
                    int32_t idx, val;
                    if (cmd == -1) break;                    /* -> D94 */
                    if (cmd == -2) continue;                 /* -> D80 */
                    if ((s7 != 0) != (s5v != 0)) continue;   /* flag gate */
                    idx = (int8_t)br2_ld8(cur + 1);
                    val = (int16_t)br2_ld16(cur + 2);
                    c->gpr[4] = a0; c->gpr[5] = a1;
                    c->gpr[6] = fp + 0x34 + (uint32_t)((int32_t)idx * 0x50);
                    c->gpr[7] = 3;
                    br2_st32(fr + 0x10, (uint32_t)(val << 1));
                    (void)br2_waypoint_jal(0x80136f70u, 0x80146d20u);
                    if (!(br2_ld32(s4 + 0x6c) & 0x800u)) {
                        /* second waypoint: 3-word vector at fr+0x18 */
                        uint32_t rec = br2_ld32(s4 + 0x40) +
                                      (uint32_t)((int32_t)cmd * 0x1c);
                        int32_t v2 = (int16_t)br2_ld16(cur + 2);
                        br2_st32(fr + 0x18, (uint32_t)(int32_t)(int16_t)br2_ld16(rec + 0x0e));
                        br2_st32(fr + 0x1c, (uint32_t)(int32_t)(int16_t)br2_ld16(rec + 0x10));
                        br2_st32(fr + 0x20, (uint32_t)(int32_t)(int16_t)br2_ld16(rec + 0x12));
                        c->gpr[4] = a0; c->gpr[5] = a1;
                        c->gpr[6] = fr + 0x18; c->gpr[7] = 5;
                        br2_st32(fr + 0x10, (uint32_t)(v2 << 1));
                        (void)br2_waypoint_jal(0x80136f70u, 0x80146d80u);
                    }
                }
            }
        }
        /* D94: bit2 retest (always set on this path) + st_58 sign gate */
        {
            int32_t f58s = (int32_t)br2_ld32(s4 + 0x58);
            if (f58s < 0) {
                int32_t x = (((int32_t)(int16_t)br2_ld16(s4 + 0x4e)
                            + (int32_t)(int16_t)br2_ld16(s4 + 0xc6) - 0x400) << 20) >> 20;
                int32_t z = (((int32_t)(int16_t)br2_ld16(s4 + 0x4e) - 0x400) << 20) >> 20;
                c->gpr[4] = a0; c->gpr[5] = a1;
                c->gpr[6] = fp + 0x34; c->gpr[7] = (uint32_t)x;
                br2_st32(fr + 0x10, (uint32_t)((uint32_t)(uint8_t)br2_ld8(s4 + 0xdb) << 4));
                br2_st32(fr + 0x14, (uint32_t)z);
                (void)br2_waypoint_jal(0x80136f80u, 0x80146e00u);
                ret = z;
            } else {
                ret = f58s;
            }
        }
    }

    /* 0x80146E00: trailing waypoint (jr-$ra stub), then epilogue */
    (void)br2_waypoint_jal(0x80136f78u, 0x80146e08u);
    br2_guest_frame_free();
    return ret;
}

/* group_fx_spawn @0x80142108 — allocate an FX group record and store the
 * 6-word payload w0..w5 into rec+0xc..0x20; arg goes through noop_b to
 * the group record (+4), not the payload area. */
void group_fx_spawn(uint32_t arg,uint32_t w0,uint32_t w1,uint32_t w2, uint32_t w3,uint32_t w4,uint32_t w5)
{
  int grp;
  int rec;
  grp = fx_group_alloc(1,1,0,0xc);
  if (grp != 0) {
    rec = noop_c(grp);
    noop_b(grp,arg, (uint32_t)br2_gaddr(&DAT_801f0760));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))= w0;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x10) ))= w1;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x14) ))= w2;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x18) ))= w3;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x1c) ))= w4;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x20) ))= w5;
  }
  return;

}

/* vec3_step_toward @0x80133aa0 family — move pos toward dst by at most
 * stp units along the normalized delta; _spd/_q/_q_spd are the
 * speed-scaled and fixed-point variants. */
uint32_t * vec3_step_toward(uint32_t *out,short *pos,short *dst,short stp)
{
  uint32_t len2;
  int len;
  short dc;
  short dy;
  int step;
  /* Guest vec3 slots: FUN_80194630 reads/writes 3 contiguous u32 through
   * delta (old local_40) and ndir (normalized out, old local_30); the
   * decompiler split each 12-uint8_t vec3 into scalars. delta[0..2] = old
   * local_40/local_3c/local_38; ndir[0..2] = old
   * local_30/local_2c/local_28. */
  int delta [3];
  int ndir [3];
  /* Guest frame 0x50: the packed step pair lives at sp+0x30..0x37 as
   * halfwords — dc@0x30, dy@0x32 (together = step_a), dz@0x34, and the
   * high half of step_b @0x36 is never written, so it stays whatever
   * the guest stack held. Only the len>=step path stores the halves;
   * the len<step path copies the frame slots verbatim. */
  uint32_t vst_frm = br2_guest_frame(0x50);
  uint16_t *stp_h = (uint16_t *)guest_at(vst_frm + 0x30);
  delta[0] = (int)*dst - (int)*pos;
  delta[1] = (int)dst[1] - (int)pos[1];
  delta[2] = (int)dst[2] - (int)pos[2];
  if (stp == 0) {
    stp = 1;
  }
  step = (int)stp;
  len2 = FUN_80194630((uint32_t *)delta,(uint32_t *)ndir);
  len = FUN_80194500(len2);
  if (len < step) {
    *pos = *dst;
    pos[1] = dst[1];
    pos[2] = dst[2];
  }
  else {
    dc = (short)(ndir[0] * step >> 0xc);
    *pos = *pos + dc;
    stp_h[0] = (uint16_t)dc;
    dy = (short)(ndir[1] * step >> 0xc);
    stp_h[1] = (uint16_t)dy;
    pos[1] = pos[1] + dy;
    dc = (short)(ndir[2] * step >> 0xc);
    stp_h[2] = (uint16_t)dc;
    pos[2] = pos[2] + dc;
  }
  *out = ((uint32_t)stp_h[1] << 16) | stp_h[0];
  out[1] = ((uint32_t)stp_h[3] << 16) | stp_h[2];
  br2_guest_frame_free();
  return out;

}

/* vec3_step_toward_spd @0x80133c08 — see vec3_step_toward. */
uint32_t * vec3_step_toward_spd(uint32_t *out,short *pos,short *dst,int spd)
{
  short rz;
  int t;
  uint32_t sh;
  uint16_t ry;
  uint32_t rx;
  /* Guest vec3 slots: FUN_80194630 reads/writes 3 contiguous u32 through
   * delta (old local_38) and ndir (normalized out, old local_28); the
   * decompiler split each 12-uint8_t vec3 into scalars. delta[0..2] = old
   * local_38/local_34/local_30; ndir[0..2] = old
   * local_28/local_24/local_20. */
  int delta [3];
  int ndir [3] = {0, 0, 0};
  /* Guest frame 0x48: the packed result lives at sp+0x30..0x37 as
   * halfwords — rx@0x30, ry@0x32 (together = res_a), rz@0x34, and the
   * high half of res_b @0x36 is never written, so it stays whatever
   * the guest stack held. The t==0 path stores zero to 0x30/0x32/0x34
   * only; the else path stores all three components then applies the
   * three single-half fixups. */
  uint32_t spd_frm = br2_guest_frame(0x48);
  uint16_t *rs_h = (uint16_t *)guest_at(spd_frm + 0x30);
  delta[0] = (int)*dst - (int)*pos;
  delta[1] = (int)dst[1] - (int)pos[1];
  delta[2] = (int)dst[2] - (int)pos[2];
  t = FUN_80194630((uint32_t *)delta,(uint32_t *)ndir);
  if (t == 0) {
    rs_h[0] = 0;
    rs_h[1] = 0;
    rs_h[2] = 0;
  }
  else {
    t = FUN_80194500((uint32_t)t);
    sh = (spd + 0xc) * 0x10000 >> 0x10;
    rx = ndir[0] * t >> (sh & 0x1f);
    ry = (uint16_t)(ndir[1] * t >> (sh & 0x1f));
    rz = (short)(ndir[2] * t >> (sh & 0x1f));
    rs_h[0] = (uint16_t)rx;
    rs_h[1] = ry;
    rs_h[2] = (uint16_t)rz;
    if (((rx & 0xffff) == 0) && (ndir[0] != 0)) {
      rs_h[0] = 1;
    }
    if ((rs_h[1] == 0) && (ndir[1] != 0)) {
      rs_h[1] = 1;
    }
    if ((rz == 0) && (ndir[2] != 0)) {
      rs_h[2] = 1;
    }
    *pos = *pos + (int16_t)rs_h[0];
    pos[1] = pos[1] + rs_h[1];
    pos[2] = pos[2] + (int16_t)rs_h[2];
  }
  *out = ((uint32_t)rs_h[1] << 16) | rs_h[0];
  out[1] = ((uint32_t)rs_h[3] << 16) | rs_h[2];
  br2_guest_frame_free();
  return out;

}

/* vec3_step_toward_q @0x80133db0 — see vec3_step_toward. */
uint32_t * vec3_step_toward_q(uint32_t *out,short *pos,short *dst,short stp)
{
  uint32_t len2;
  int len;
  short dc;
  short dy;
  int step;
  /* Guest vec3 slots: FUN_80194660 (vec3_s16_normalize) reads 3
   * contiguous s16 through pk (old local_28, packed into the two u32
   * halves of pk[0] plus the high half of pk[1]) and writes 3
   * contiguous s16 through ndir (old local_20). The decompiler split
   * the s16 vec3 into scalars. ndir[0..2] = old local_20/local_1e/local_1c. */
  uint32_t pk [2];
  short ndir [3];
  pk[0] = CONCAT22((short)((dst[1] - pos[1]) * 0x10) >> 4,
                      (short)((*dst - *pos) * 0x10) >> 4);
  pk[1] = CONCAT22((pk[1] >> 16),(short)((dst[2] - pos[2]) * 0x10) >> 4);
  if (stp == 0) {
    stp = 1;
  }
  step = (int)stp;
  len2 = FUN_80194660((uint16_t *)pk,(uint16_t *)ndir);
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

/* vec3_step_toward_q_spd @0x80133f30 — see vec3_step_toward. */
uint32_t * vec3_step_toward_q_spd(uint32_t *out,short *pos,short *dst,int spd)
{
  short rz;
  int t;
  uint32_t sh;
  uint16_t ry;
  uint32_t rx;
  /* Guest vec3 slots: FUN_80194660 (vec3_s16_normalize) reads 3
   * contiguous s16 packed through pk[0] (old local_20) plus the high
   * half of pk[1], and writes 3 contiguous s16 through ndir (old
   * local_18). The decompiler split the s16 vec3 into scalars.
   * ndir[0..2] = old local_18/local_16/local_14. */
  uint32_t pk [2];
  short ndir [3];
  pk[0] = CONCAT22((short)((dst[1] - pos[1]) * 0x10) >> 4,
                      (short)((*dst - *pos) * 0x10) >> 4);
  pk[1] = (pk[1] & 0xffff0000) | (uint16_t)((short)((dst[2] - pos[2]) * 0x10) >> 4);
  t = FUN_80194660((uint16_t *)pk,(uint16_t *)ndir);
  if (t == 0) {
    pk[1] = (uint32_t)(pk[1] >> 16) << 0x10;
    pk[0] = 0;
  }
  else {
    t = FUN_80194500((uint32_t)t);
    sh = (spd + 0xc) * 0x10000 >> 0x10;
    rx = ndir[0] * t >> (sh & 0x1f);
    ry = (uint16_t)(ndir[1] * t >> (sh & 0x1f));
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

/* table_byte_next_a @0x80129f78/b — advance the shared uint8_t-table cursor and
 * return the next uint8_t (wrapping at table end). */
uint8_t table_byte_next_a(void)
{
  uint8_t idx;
  idx = DAT_801e83aa;
  DAT_801e83aa = DAT_801e83aa + 1;
  return (&DAT_8011ec40)[idx];

}

/* table_byte_next_b @0x80129fc4 — second uint8_t-table cursor advance (see _a). */
uint8_t table_byte_next_b(void)
{
  uint8_t idx;
  idx = DAT_801e83b0;
  DAT_801e83b0 = DAT_801e83b0 + 1;
  return *(uint8_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x801e8400)));

}
/* sprite_strip_draw_dual @0x801721a4 — draw a dual-source sprite
 * strip: src_a/src_b are destination sprite records advanced by 0x28
 * per element, pal is the coordinate/UV table; n elements, tag mode. */
void sprite_strip_draw_dual(int src_a,int src_b,int uv,int n,uint16_t *pal,int x0, int y0,int tag)
{
  uint16_t *pa;
  uint16_t *pb;
  uint16_t pv;
  int xfp;
  int yfp;
  uint16_t *pq;
  int i;
  int cur_a;
  int cur_b;
  if (tag == 1) {
    i = 0;
    if (0 < n) {
      x0 = x0 * uv;
      pq = pal + 3;
      y0 = y0 * uv;
      do {
        xfp = x0;
        if (x0 < 0) {
          xfp = x0 + 0xfff;
        }
        yfp = y0;
        if (y0 < 0) {
          yfp = y0 + 0xfff;
        }
        sprite_place_centered(src_a,0x1f800000,(int)(((uint32_t)*pal + (xfp >> 0xc)) * 0x10000) >> 0x10,
                     (int)(((uint32_t)pq[-2] + (yfp >> 0xc)) * 0x10000) >> 0x10,
                     (int)(short)pq[-1],(int)(short)*pq);
        i = i + 1;
        pa = pq + -2;
        pb = pq + -1;
        pv = *pq;
        pq = pq + 4;
        sprite_place_centered(src_b,0x1f800000,(int)(((uint32_t)*pal + (xfp >> 0xc)) * 0x10000) >> 0x10,
                     (int)(((uint32_t)*pa + (yfp >> 0xc)) * 0x10000) >> 0x10,(int)(short)*pb,
                     (int)(short)pv);
        pal = pal + 4;
        src_a = src_a + 0x28;
        src_b = src_b + 0x28;
      } while (i < n);
    }
  }
  else if (tag < 2) {
    if ((tag == 0) && (i = 0, 0 < n)) {
      x0 = x0 * uv;
      pq = pal + 3;
      y0 = y0 * uv;
      do {
        xfp = x0;
        if (x0 < 0) {
          xfp = x0 + 0xfff;
        }
        yfp = y0;
        if (y0 < 0) {
          yfp = y0 + 0xfff;
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
      x0 = x0 * uv;
      pq = pal + 3;
      y0 = y0 * uv;
      cur_a = src_a;
      do {
        xfp = x0;
        if (x0 < 0) {
          xfp = x0 + 0xfff;
        }
        yfp = y0;
        if (y0 < 0) {
          yfp = y0 + 0xfff;
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
    x0 = x0 * uv;
    pq = pal + 3;
    y0 = y0 * uv;
    cur_b = src_a;
    do {
      xfp = x0;
      if (x0 < 0) {
        xfp = x0 + 0xfff;
      }
      yfp = y0;
      if (y0 < 0) {
        yfp = y0 + 0xfff;
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

/* task_alloc_wrap_a @0x801725a4 — see task_alloc_wrap_c. */
int task_alloc_wrap_a(uint32_t w0,uint32_t w1,int w2,uint32_t w3)
{
  int node;
  uint32_t *pay;
  node = (int)(intptr_t)task_node_alloc(0,2,0,0x80137680,0,0x1c);
  if (node != 0) {
    pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = w0;
    pay[1] = w1;
    pay[2] = w2;
    pay[3] = w3;
    pay[4] = w2 + -4;
    pay[5] = 0;
    pay[6] = 0;
  }
  return node;

}

/* task_alloc_wrap_b @0x80172640 — see task_alloc_wrap_c (5-word payload variant). */
int task_alloc_wrap_b(uint32_t w0,uint32_t w1,uint32_t w2,uint32_t w3, int w4)
{
  int node;
  uint32_t *pay;
  { extern uint32_t g_wb_last_p1,g_wb_last_p2,g_wb_last_p3,g_wb_last_p4,g_wb_last_p5;
    extern uint32_t g_wb_bad_count,g_wb_bad_node,g_wrap_src;
    g_wrap_src=1;
    g_wb_last_p1=(uint32_t)w0; g_wb_last_p2=(uint32_t)w1;
    g_wb_last_p3=(uint32_t)w2; g_wb_last_p4=(uint32_t)w3;
    g_wb_last_p5=(uint32_t)w4;
    if (((uint32_t)w0 < 0x80100000u || (uint32_t)w1 < 0x80100000u) &&
        (uint32_t)w0 != 0u && (uint32_t)w1 != 0u) {
      g_wb_bad_count++; g_wb_bad_node=(uint32_t)w3;
    }
  }
  node = (int)(intptr_t)task_node_alloc(0,3,0,0x80137824,0,0x1c);
  if (node != 0) {
    pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = w0;
    pay[1] = w1;
    pay[2] = w2;
    pay[3] = w3;
    pay[4] = w4;
    pay[5] = w4 + -4;
    *(uint16_t *)(pay + 6) = 0;
    *(uint16_t *)((intptr_t)pay + 0x1a) = 0;
  }
  return node;

}

/* hud_cell_frame_set @0x801726ec — write the HUD cell frame/UV word for digit
 * value val into cell record (uses DAT_801b4b54/56 + scroll offset). */
void hud_cell_frame_set(int val,int cell)
{
  uint16_t fr;
  if (val < 10) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cell + 0xe) ))=
         (short)((DAT_801b4b54 & 0x3ff) >> 4) +
         (DAT_801b4b56 + (uint16_t)(uint8_t)(&DAT_801b4e28)[_DAT_801f4f20 % 3] & 0x1ff) * 0x40;
  }
  else {
    if (val < 0x14) {
      fr = DAT_801b4b56 + 1;
    }
    else {
      fr = DAT_801b4b56 + 3;
    }
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cell + 0xe) ))= (short)((DAT_801b4b54 & 0x3ff) >> 4) + (fr & 0x1ff) * 0x40;
  }
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cell + 0x36) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cell + 0xe)));
  return;

}

/* hud_digits_color @0x801727b8 — set the HUD digit color/attr fields for value
 * val into the cell record. */
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
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((cell + 0xd) ))= (&DAT_801b4b6d)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x14) ))= *dg + (&DAT_801b4b6e)[tens_off];
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((cell + 0x15) ))= (&DAT_801b4b6d)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x1c) ))= *dg;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((cell + 0x1d) ))= (&DAT_801b4b6d)[tens_off] + (&DAT_801b4b6f)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x24) ))= *dg + (&DAT_801b4b6e)[tens_off];
    ones_off = (val % 10 & 0xffU) * 4;
    dg = &DAT_801b4b6c + ones_off;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((cell + 0x25) ))= (&DAT_801b4b6d)[tens_off] + (&DAT_801b4b6f)[tens_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x34) ))= *dg;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((cell + 0x35) ))= (&DAT_801b4b6d)[ones_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x3c) ))= *dg + (&DAT_801b4b6e)[ones_off];
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((cell + 0x3d) ))= (&DAT_801b4b6d)[ones_off];
    *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x44) ))= *dg;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((cell + 0x45) ))= (&DAT_801b4b6d)[ones_off] + (&DAT_801b4b6f)[ones_off];
    c0 = *dg;
  }
  *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x4c) ))= c0 + dg[2];
  *(char *)guest_at((uint32_t)(uintptr_t)((cell + 0x4d) ))= dg[1] + dg[3];
  return;

}

/* fighter_height_check @0x80172a04 — return the height-limit code for a fighter:
 * bit 0x10 set -> true, else 0x1dff < f+0xd6 short. */
bool fighter_height_check(int f)
{
  if ((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((f + 5) ))& 0x10) == 0) {
    return 0x1dff < *(short *)guest_at((uint32_t)(uintptr_t)((f + 0xd6)));
  }
  return (bool)2;

}

/* sprite_rect_set_a @0x801756b0/b/c — sprite rect variants: write position (x,y)
 * and packed size (w,h) into the sprite record; _a differs from _b/_c
 * (which are identical to each other) in corner/anchor convention. */
uint32_t sprite_rect_set_a(int rec,short x,int y,uint32_t w,int h)
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

/* sprite_rect_set_b @0x80175708 — see sprite_rect_set_a. */
uint32_t sprite_rect_set_b(int rec,short x,int y,uint32_t w,int h)
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

/* sprite_rect_set_c @0x80175760 — see sprite_rect_set_a. */
uint32_t sprite_rect_set_c(int rec,short x,int y,uint32_t w,int h)
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

/* rtps_point_xform @0x8018d538/xform2 — GTE RTPS point transform: transform
 * s16 vec3 in -> out via cop2 op 0x486012. flag==0 additionally loads
 * rt[0..4] into the RT control regs first (xform2 always loads them);
 * no perspective divide in either. */
int * rtps_point_xform(uint32_t *rt,uint32_t *in,int *out,int flag)
{
  uint32_t cop2_idx0 = 0;      /* lwc2 gte[0] */
  uint32_t cop2_idx1 = 0x800;    /* lwc2 gte[1] */
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
  setCopReg(2,cop2_idx0,*in);
  setCopReg(2,cop2_idx1,in[1]);
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

/* rtps_point_xform2 @0x8018d540 — RTPS point transform without the flag variant
 * (see rtps_point_xform). */
int * rtps_point_xform2(uint32_t *rt,uint32_t *in,int *out)
{
  uint32_t cop2_idx0 = 0;
  uint32_t cop2_idx1 = 0x800;
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
  setCopReg(2,cop2_idx0,*in);
  setCopReg(2,cop2_idx1,in[1]);
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

/* rotmat_rtps_build @0x8018de6c — build an RTPS rotation matrix from a packed
 * euler angle word (10-bit fields + sign/axis flags) into the packed
 * five-word form m[0..4] (m[4] low half only). */
uint32_t * rotmat_rtps_build(uint32_t ang,uint32_t *m)
{
  int ia;
  int ic;
  uint32_t tv0;
  int ib;
  uint32_t tv1;
  m[2] = 0x1000;
  ia = (ang & 0x3ff) * 2;
  ib = (ang & 0x3ff) * -2 + 0x800;
  ic = ia;
  if ((ang & 0x400) != 0) {
    ic = ib;
    ib = ia;
  }
  tv0 = (uint32_t)*(uint16_t *)(&DAT_801247a8 + ic);
  if ((ang & 0x800) != 0) {
    tv0 = -tv0 & 0xffff;
  }
  tv1 = (uint32_t)*(uint16_t *)(&DAT_801247a8 + ib);
  if ((ang + 0x400 & 0x800) != 0) {
    tv1 = -tv1 & 0xffff;
  }
  *m = tv1;
  m[1] = tv0;
  m[3] = -tv0 & 0xffff;
  *(short *)(m + 4) = (short)tv1;
  return m;

}

/* rotmat2_pair_build @0x8018df00 — build a two-angle rotation matrix from packed
 * angle words ang0/ang1 via the DAT_801247a8 sin LUT into out[0..4]. */
uint32_t * rotmat2_pair_build(uint32_t ang0,uint32_t ang1,uint32_t *out)
{
  int t0;
  int t1;
  int t2;
  int t3;
  uint32_t g0;
  uint32_t g1;
  uint32_t g2;
  int t4;
  t0 = (ang0 & 0x3ff) * 2;
  t4 = (ang0 & 0x3ff) * -2 + 0x800;
  t3 = t0;
  if ((ang0 & 0x400) != 0) {
    t3 = t4;
    t4 = t0;
  }
  t3 = (int)*(short *)(&DAT_801247a8 + t3);
  if ((ang0 & 0x800) != 0) {
    t3 = -t3;
  }
  t1 = (ang1 & 0x3ff) * 2;
  t2 = (ang1 & 0x3ff) * -2 + 0x800;
  t0 = t1;
  if ((ang1 & 0x400) != 0) {
    t0 = t2;
    t2 = t1;
  }
  g1 = (uint32_t)*(short *)(&DAT_801247a8 + t0);
  if ((ang1 & 0x800) != 0) {
    g1 = -g1;
  }
  g2 = (uint32_t)*(short *)(&DAT_801247a8 + t2);
  if ((ang1 + 0x400 & 0x800) != 0) {
    g2 = -g2;
  }
  g0 = (uint32_t)*(short *)(&DAT_801247a8 + t4);
  if ((ang0 + 0x400 & 0x800) != 0) {
    g0 = -g0;
  }
  out[1] = g1 & 0xffff | ((int)(g1 * t3) >> 0xc) << 0x10;
  out[2] = g0 & 0xffff | ((int)(g2 * -t3) >> 0xc) << 0x10;
  *out = g2 & 0xffff;
  out[3] = t3 << 0x10 | -((int)(g1 * g0) >> 0xc) & 0xffffU;
  *(short *)(out + 4) = (short)((int)(g2 * g0) >> 0xc);
  return out;

}

/* sprintf_core @0x8018ef6c — printf engine: parse fmt, handle %d/%u/%x/%s/
 * %c/%%/%n with width/precision/flags, write digits via wp cursor.
 * (clean port of guest sprintf_core_engine; wrapper entry is 0x8018ef60) */
int sprintf_core(char *dst,char *fmt,...)
{
  char ch;
  char *nx;
  uint32_t num;
  int fc;
  uint32_t nd;
  uint8_t *wp;
  int *np;
  int n;
  char *fp;
  va_list ap;
  /* Guest: one 0x200-uint8_t digit scratch at sp+0x10..0x20f; the uint8_t
   * cursor starts at sp+0x210 (one past the top, adjacent to the
   * fl flags word) and walks down. %c stores its uint8_t at the
   * scratch top-1 (sp+0x20f == old local_39). Ghidra split the scratch
   * into &fl/&local_39 scalars, so digits written wp-1 ran
   * off into unrelated host stack — keep it one array. */
  uint8_t sc_buf[0x200];
#define sc_top sc_buf[0x1ff]
  uint32_t fl;
  uint32_t wid;
  uint32_t prc;
  va_start(ap,fmt);
  ch = *fmt;
  n = 0;
  fp = fmt;
joined_r0x8018efa8:
  fc = (int)ch;
  if (fc != 0) {
    if (fc == 0x25) {
      fl = DAT_801b67a0;
      wid = DAT_801b67a4;
      prc = DAT_801b67a8;
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                nx = fp + 1;
                fc = (int)fp[1];
                if (fc != 0x2d) break;
                fl = fl | 1;
                fp = nx;
              }
              if (fc != 0x2b) break;
              fl = fl | 2;
              fp = nx;
            }
            if (fc != 0x20) break;
            fl = (fl & 0xffff0000) | CONCAT11(fp[1],(uint8_t)fl);
            fp = nx;
          }
          if (fc != 0x23) break;
          fl = fl | 4;
          fp = nx;
        }
        if (fc != 0x30) break;
        fl = fl | 8;
        fp = nx;
      }
      if (fc == 0x2a) {
        wid = va_arg(ap,uint32_t);

        if ((int)wid < 0) {
          wid = -wid;
          fl = fl | 1;
        }
        fc = (int)fp[2];
        fp = fp + 2;
      }
      else {
        while (fp = nx, fc - 0x30U < 10) {
          wid = wid * 10 + -0x30 + fc;
          nx = fp + 1;
          fc = (int)fp[1];
        }
      }
      if (fc == 0x2e) {
        fc = (int)fp[1];
        nx = fp;
        if (fc == 0x2a) {
          prc = va_arg(ap,uint32_t);

          fc = (int)fp[2];
          fp = fp + 2;
        }
        else {
          while (fp = nx + 1, fc - 0x30U < 10) {
            prc = prc * 10 + -0x30 + fc;
            fc = (int)nx[2];
            nx = fp;
          }
        }
        if (-1 < (int)prc) {
          fl = fl | 0x10;
        }
      }
      wp = sc_buf + sizeof(sc_buf);
      if ((fl & 1) != 0) {
        fl = fl & 0xfffffff7;
      }
      do {
        switch(fc) {
        case 0x4c:
          fl = fl | 0x80;
          break;
        default:
          goto switchD_8018f218_caseD_4d;
        case 99:
          wp = &sc_top;
          nd = 1;
          sc_top = (uint8_t)va_arg(ap,uint32_t);

          goto LAB_8018f700;
        case 100:
        case 0x69:
          num = va_arg(ap,uint32_t);
          if ((fl >> 5 & 1) != 0) {
            num = (uint32_t)(short)num;
          }
          if ((int)num < 0) {
            num = -num;
            fl = (fl & 0xffff0000) | CONCAT11(0x2d,(uint8_t)fl);
          }
          else if ((fl >> 1 & 1) != 0) {
            fl = (fl & 0xffff0000) | CONCAT11(0x2b,(uint8_t)fl);
          }
          goto LAB_8018f2f0;
        case 0x68:
          fl = fl | 0x20;
          break;
        case 0x6c:
          fl = fl | 0x40;
          break;
        case 0x6e:
          {
            uintptr_t nv = va_arg(ap,uintptr_t);
            uint32_t ng = guest_of((const void *)nv);
            np = (int *)(ng ? guest_at(ng)
                        : ((uint32_t)nv < 0x20000000u || (uint32_t)nv >= 0x80000000u
                           ? guest_at((uint32_t)nv) : (void *)(uintptr_t)nv));
          }

          if ((fl >> 5 & 1) == 0) {
            *np = n;
          }
          else {
            *(short *)np = (short)n;
          }
          goto LAB_8018f78c;
        case 0x6f:
          num = va_arg(ap,uint32_t);

          if ((fl >> 5 & 1) != 0) {
            num = num & 0xffff;
          }
          if ((fl >> 4 & 1) == 0) {
            if ((fl >> 3 & 1) != 0) {
              prc = wid;
            }
            if ((int)prc < 1) {
              prc = 1;
            }
          }
          nd = 0;
          for (; num != 0; num = num >> 3) {
            wp = wp + -1;
            *wp = ((uint8_t)num & 7) + 0x30;
            nd = nd + 1;
          }
          if ((((fl >> 2 & 1) != 0) && (nd != 0)) && (*wp != 0x30)) {
            wp = wp + -1;
            *wp = 0x30;
            nd = nd + 1;
          }
          for (; (int)nd < (int)prc; nd = nd + 1) {
            wp = wp + -1;
            *wp = 0x30;
          }
          goto LAB_8018f700;
        case 0x70:
          prc = 8;
          fl = fl | 0x50;
        case 0x58:
          nx = s_0123456789ABCDEF_80125208;
          goto LAB_8018f504;
        case 0x73:
          {
            uintptr_t sv = va_arg(ap,uintptr_t);
            uint32_t sg = guest_of((const void *)sv);
            wp = sg ? (uint8_t *)guest_at(sg)
                        : ((uint32_t)sv < 0x20000000u || (uint32_t)sv >= 0x80000000u
                           ? (uint8_t *)guest_at((uint32_t)sv) : (uint8_t *)sv);
          }

          if ((fl >> 2 & 1) == 0) {
            if ((fl >> 4 & 1) == 0) {
              nd = str_len(wp);
            }
            else {
              fc = (int)(intptr_t)strchr_bounded((char *)(uintptr_t)wp,0,prc);
              nd = fc - (intptr_t)wp;
              if (fc == 0) {
                nd = prc;
              }
            }
          }
          else {
            nd = (uint32_t)*wp;
            wp = wp + 1;
            if (((fl >> 4 & 1) != 0) && ((int)prc < (int)nd)) {
              nd = prc;
            }
          }
          goto LAB_8018f700;
        case 0x75:
          num = va_arg(ap,uint32_t);
          if ((fl >> 5 & 1) != 0) {
            num = num & 0xffff;
          }
          fl = (fl & 0xffff0000) | (uint16_t)(uint8_t)fl;
LAB_8018f2f0:

          if ((fl >> 4 & 1) == 0) {
            if (((fl >> 3 & 1) != 0) && (prc = wid, ((fl >> 8) & 0xff) != 0)) {
              prc = wid - 1;
            }
            if ((int)prc < 1) {
              prc = 1;
            }
          }
          nd = 0;
          while (num != 0) {
            wp = wp + -1;
            nd = nd + 1;
            *wp = (char)num + (char)(num / 10) * -10 + 0x30;
            num = num / 10;
          }
          for (; (int)nd < (int)prc; nd = nd + 1) {
            wp = wp + -1;
            *wp = 0x30;
          }
          if (((fl >> 8) & 0xff) != 0) {
            wp = wp + -1;
            *wp = (char)((fl >> 8) & 0xff);
            nd = nd + 1;
          }
          goto LAB_8018f700;
        case 0x78:
          nx = s_0123456789abcdef_8012521c;
LAB_8018f504:
          num = va_arg(ap,uint32_t);

          if ((fl >> 5 & 1) != 0) {
            num = num & 0xffff;
          }
          if ((fl >> 4 & 1) == 0) {
            if (((fl >> 3 & 1) != 0) && (prc = wid, (fl >> 2 & 1) != 0)) {
              prc = wid - 2;
            }
            if ((int)prc < 1) {
              prc = 1;
            }
          }
          nd = 0;
          for (; num != 0; num = num >> 4) {
            wp = wp + -1;
            nd = nd + 1;
            *wp = nx[num & 0xf];
          }
          for (; (int)nd < (int)prc; nd = nd + 1) {
            wp = wp + -1;
            *wp = 0x30;
          }
          if ((fl >> 2 & 1) != 0) {
            wp[-1] = (uint8_t)fc;
            wp = wp + -2;
            *wp = 0x30;
            nd = nd + 2;
          }
LAB_8018f700:
          if (((int)nd < (int)wid) && ((fl & 1) == 0)) {
            do {
              *(uint8_t *)(dst + n)= 0x20;
              wid = wid - 1;
              n = n + 1;
            } while ((int)nd < (int)wid);
          }
          mem_move((uint8_t *)(dst + n),wp,nd);
          n = n + nd;
          for (; (int)nd < (int)wid; nd = nd + 1) {
            *(uint8_t *)(dst + n)= 0x20;
            n = n + 1;
          }
          goto LAB_8018f78c;
        }
        fc = (int)fp[1];
        fp = fp + 1;
      } while( true );
    }
    goto LAB_8018f6f4;
  }
LAB_8018f7ac:
  *(uint8_t *)(dst + n)= 0;
  return n;
switchD_8018f218_caseD_4d:
  if (fc != 0x25) goto LAB_8018f7ac;
LAB_8018f6f4:
  *(char *)(dst + n) = (char)fc;
  n = n + 1;
LAB_8018f78c:
  ch = fp[1];
  fp = fp + 1;
  goto joined_r0x8018efa8;

}
#undef sc_top

/* rtps_basis_xform @0x80186d20 — transform a basis-vector set through matrix rt
 * and repack into out. */
uint32_t * rtps_basis_xform(uint32_t *rt,uint16_t *bas,uint32_t *out)
{
  uint16_t in0;
  uint32_t in2;
  uint32_t t0;
  int t1;
  uint32_t t2;
  int t6;
  int t3;
  int in1;
  uint32_t t4;
  int t5;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,CONCAT22(bas[3],*bas));
  setCopReg(2,0x800,*(uint32_t *)(bas + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t2 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)(uint16_t)bas[1] | *(uint32_t *)(bas + 4) << 0x10);
  setCopReg(2,0x800,*(uint32_t *)(bas + 6) >> 0x10);
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)(uint16_t)bas[2] | *(uint32_t *)(bas + 4) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(bas + 8));
  copFunction(2,0x486012);
  in0 = bas[10];
  in1 = *(int *)(bas + 0xc);
  in2 = *(uint32_t *)(bas + 0xe);
  *out = t0 & 0xffff | t3 << 0x10;
  out[3] = t5 << 0x10 | t2 & 0xffff;
  t2 = getCopReg(2,0x4800);
  t3 = getCopReg(2,0x5000);
  t0 = getCopReg(2,0xb);
  out[4] = t0;
  setCopReg(2,0,(uint32_t)in0 | in1 << 0x10);
  setCopReg(2,0x800,in2);
  copFunction(2,0x486012);
  out[1] = t2 & 0xffff | t1 << 0x10;
  out[2] = t3 << 0x10 | t4 & 0xffff;
  t3 = getCopReg(2,0xc800);
  t1 = getCopReg(2,0xd000);
  in1 = getCopReg(2,0xd800);
  t5 = rt[6];
  t6 = rt[7];
  out[5] = t3 + rt[5];
  out[6] = t1 + t5;
  out[7] = in1 + t6;
  return out;

}

/* rotmat2_build @0x8018da2c — build a rotation matrix from a single packed angle
 * into m (RTPS layout). */
uint32_t * rotmat2_build(uint32_t ang,uint32_t *m)
{
  int ia;
  int ic;
  uint32_t tv0;
  int ib;
  uint32_t tv1;
  *m = 0x1000;
  ia = (ang & 0x3ff) * 2;
  ib = (ang & 0x3ff) * -2 + 0x800;
  ic = ia;
  if ((ang & 0x400) != 0) {
    ic = ib;
    ib = ia;
  }
  tv0 = (uint32_t)*(uint16_t *)(&DAT_801247a8 + ic);
  if ((ang & 0x800) != 0) {
    tv0 = -tv0;
  }
  tv1 = (uint32_t)*(uint16_t *)(&DAT_801247a8 + ib);
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
 *  - pointer-typed ports (`int *`, `uint32_t *`, ...) treat it as a
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
  case 0x8013e3f4:   /* paired_task_cb(int)      — guest u32 arg (guest_at inside) */
  case 0x8013f648:   /* task_cb_voice_line(int)      — guest u32 arg (guest_at inside) */
  case 0x801407cc:   /* ring_effect_task_update(int)      — guest u32 arg */
  case 0x80140d38:   /* firework_task_update(int)      — guest u32 arg */
  case 0x801411e8:   /* emitter_30_teardown(uint32_t)      — guest u32 arg (MB32) */
  case 0x80141610:   /* quad_fx_update(int)      — guest u32 arg */
  case 0x8014afc0:   /* event_record_walk(uint32_t)      — guest u32 arg (MBS16) */
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

/* task_node_alloc @0x801a4308 — allocate a task node from pool kind/cnt/sz
 * with callback cb and payload size; returns node or 0. */
int * task_node_alloc(int kind,uint32_t cnt,uint32_t sz,int cb,int cb_arg,int dsize)
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
      *(uint8_t *)((intptr_t)node + 0x17) = 1;
      node[1] = 0;
      node[2] = cb;
      node[3] = cb_arg;
      *(char *)(node + 5) = (char)kind;
      *(char *)((intptr_t)node + 0x15) = (char)cnt;
      *(char *)((intptr_t)node + 0x16) = (char)sz;
      node[4] = (int)guest_of((char *)(node + 7));
      *(uint8_t *)(node + 6) = 0;
      return (int *)(uintptr_t)guest_of(node);
    }
  }
  return (int *)0x0;

}

/* task_node_free @0x801a48fc — free a task node back to its pool and run
 * the destructor chain. */
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
      *(uint8_t *)(node + 6) = 1;
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

/* task_pools_tick @0x801a49d0 — run one tick over a task pool: invoke each
 * live node's fn callback. */
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

/* noop_a @0x801ad240 / noop_b @0x801ad224 / noop_c @0x801ad284 —
 * task-payload data accessors: _a/_b store w0.. into the record at
 * pay+8, _c returns that record pointer. */
void noop_a(int node,uint32_t w0,uint32_t w1,uint32_t w2)
{
  int datap;
  datap = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10) ))+ 8)));
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 4) ))= w0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 8) ))= w1;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 0xc) ))= w2;
  return;

}

/* fx_group_alloc @0x801ad38c — allocate an FX group node from the
 * pool/kind/ix/iy tables (DAT -0x7fe009e0 indexed). */
int fx_group_alloc(uint32_t pool,uint32_t kind,int ix,int iy)
{
  int node;
  uint32_t *pay;
  int *grp;
  grp = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((iy * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((ix * 4 + -0x7fe009e0)))))));
  node = (int)(intptr_t)task_node_alloc(pool,kind,4,0x801b0db8,0,grp[1] + 0x1c);
  if (node != 0) {
    pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = 0;
    pay[2] = (uint32_t)br2_gaddr(pay + 7);
    pay[1] = (uint32_t)br2_gaddr(grp);
    pay[3] = (uint32_t)(br2_gaddr(grp) + (uint32_t)*grp);
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pay[2] ))= pay[2];
  }
  return node;

}

/* noop_b @0x801ad224 — see noop_a. */
void noop_b(int node,uint32_t w0,uint32_t w1)
{
  int datap;
  datap = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10) ))+ 8)));
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 4) ))= w0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 8) ))= w1;
  return;

}

/* noop_c @0x801ad284 — see noop_a. */
uint32_t noop_c(int node)
{
  return *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10) ))+ 8)));

}

/* node_tree_free @0x801ac758 — recursively free a node tree: release all
 * children reachable via the +0x10 array, the +0x1c/+0x20/+0x24 triple,
 * and the +0x18/+0x48/+0x14 single slots (the node itself is not freed). */
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
    if ((uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0xc) )))!= (uint32_t *)0x0) {
      *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0xc) )))= 0;
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
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x34)))) ))= 0;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x38)))) ))= 0;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x3c)))) ))= 0;
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

/* prim_attr_word_set @0x801ac9b8 — write the attribute halfword into a
 * primitive record. */
void prim_attr_word_set(int prim,uint16_t attr)
{
  int rec;
  uint32_t k;
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
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((head + 0x70) ))& 0x7fffffff) == 0) {
LAB_801acae8:
          tab = tab + 1;
        }
        else {
          do {
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ))= attr;
            k = k + 1;
            rec = rec + 0x28;
          } while (k < (uint32_t)(*(int *)guest_at((uint32_t)(uintptr_t)((ent + 0x5c) ))<< 1));
          tab = tab + 1;
        }
      }
      else if (slot < 6) {
        if (slot == 4) {
          rec = *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*tab + 0x10))));
          k = 0;
          if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((head + 0x6c) ))& 0x7fffffff) == 0) goto LAB_801acae8;
          do {
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ))= attr;
            k = k + 1;
            rec = rec + 0x20;
          } while (k < (uint32_t)(*(int *)guest_at((uint32_t)(uintptr_t)((ent + 0x5c) ))<< 1));
          tab = tab + 1;
        }
      }
      else if (slot == 6) {
        rec = *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*tab + 0x10))));
        k = 0;
        if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((head + 0x74) ))& 0x7fffffff) != 0) {
          do {
            *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0xe) ))= attr;
            k = k + 1;
            rec = rec + 0x28;
          } while (k < (uint32_t)(*(int *)guest_at((uint32_t)(uintptr_t)((ent + 0x5c) ))<< 1));
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

/* prim_group_build @0x801a6bd0 — build a primitive group node: allocate
 * via prim_group_alloc and emit the member primitives. */
uint32_t * prim_group_build(uint32_t prim,int attr,uint32_t a2,uint32_t a3)
{
  uint32_t *grp;
  int lnode;
  uint32_t *bd;
  uint32_t i;
  /* emit is only read under the same attr+0x20!=0 guard that runs the
   * do-while (which executes at least once), so it is always assigned —
   * init only to silence the conditional-uninitialized false positive. */
  int emit = 0;
  int *outp;
  uint32_t xf [2];
  bd = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x80))));
  grp = (uint32_t *)
           prim_group_alloc(2,attr,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x20))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x28))),
                        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x24))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x18))),a3);
  if (grp != (uint32_t *)0x0) {
    /* grp holds a GUEST address (prim_group_alloc returns the node's
     * guest pointer). Field accesses must resolve through guest_at; the
     * guest addr itself is what node_tree_free/return propagate. */
    uint32_t g1 = (uint32_t)(uintptr_t)grp;
    outp = (int *)guest_at(*(uint32_t *)guest_at(g1 + 0x10));
    xf[0] = *(uint32_t *)guest_at(g1 + 0x30);
    *(uint32_t *)guest_at(g1 + 0x8) = attr;
    *(uint32_t *)guest_at(g1 + 0xc) = prim;
    i = 0;
    if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x20) ))!= 0) {
      do {
        /* bd is a HOST pointer into the batch-descriptor table; the
         * callee indexes it as a GUEST address via guest_at(prim+off)
         * (generated passes the descriptor's guest addr in $prim). Hand it the
         * guest address, not the truncated host pointer. */
        emit = prim_batch_emit((int)(intptr_t)br2_gaddr(bd),*bd,*(uint32_t *)guest_at(g1 + 0x28),*(uint32_t *)guest_at(g1 + 0x2c),xf,
                                *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x2c))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x34))),
                                *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x30))),a2,
                                *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x1c))),a3);
        *outp = emit;
        outp = outp + 1;
        if (emit == 0) goto LAB_801a6cfc;
        i = i + 1;
        bd = bd + 0xd;
      } while (i < *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x20))));
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x20) ))!= 0) {
      lnode = lazy_node_wrap(emit,*(uint32_t *)guest_at(g1 + 0x4),*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x10) ))!= 0);
      *(uint32_t *)guest_at(g1 + 0x18) = lnode;
      if (lnode == 0) {
LAB_801a6cfc:
        node_tree_free((int)(uintptr_t)grp);
        grp = (uint32_t *)0x0;
      }
      else {
        *(uint32_t *)guest_at(g1) = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((lnode + 0x10)));
      }
    }
  }
  return grp;

}

/* tween_chan_alloc @0x801a72a0/dur_spread — allocate a tween channel
 * for group/idx and spread its duration across channels. */
uint32_t tween_chan_alloc(int grp,int idx)
{
  uint32_t seq;
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
    seq = tween_seq_alloc(2,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((sub + 0x14))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x30))),
                         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((sub + 0x8c))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((sub + 0x24))),
                         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 8) ))+ 0x30))),
                         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((idx * 8 + *(int *)guest_at((uint32_t)(uintptr_t)((sub + 0x7c) ))+ 4))),
                         (int)*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x14) ))+ 0x14))));
    idx = idx * 4;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((idx + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))) ))= seq;
    sub = *(int *)guest_at((uint32_t)(uintptr_t)((idx + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24))))));
    if (sub != 0) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((idx + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c)))) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((sub + 0x10)));
      return 1;
    }
  }
  return 0;

}

/* tween_dur_spread @0x801a73e4 — see tween_chan_alloc. */
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

/* str_len @0x8018e510 — strlen over guest memory. */
int str_len(char *s)
{
  char ch;
  int i;
  int len;
  len = 0;
  i = 0;
  if (s != (char *)0x0) {
    while (len = i, ch = *s, s = s + 1, ch != '\0') {
      i = len + 1;
    }
  }
  return len;

}

/* strchr_bounded @0x8018eca0 — memchr-style bounded strchr: find c in s within n
 * bytes, return pointer or NULL. */
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

/* mem_move @0x8018f7e0 — memmove over guest memory; returns the advanced
 * dst (dst+n) on the forward path, original dst on the backward path. */
uint8_t * mem_move(uint8_t *s,uint8_t *src,int n)
{
  uint8_t tmp;
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

/* script_stream_tick @0x80137824 — advance a channel script stream one tick:
 * fetch opcode, dispatch handler, update cursor/counters. */
void script_stream_tick(int *stream)
{
  short opw;
  int pc;
  uint32_t arg;
  char *col;
  uint32_t opc;
  uint16_t *ip;
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
      ip = (uint16_t *)guest_at((uint32_t)(uintptr_t)(stream[5]));
      arg = *ip & 0xff;
      opc = *ip & 0xff00;
      if (opc == 0xfe00) {
        ip = (uint16_t *)guest_at((uint32_t)(uintptr_t)((stream[4] + arg * 4)));
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
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)(stream[2] ))= 0;
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
    *(uint16_t *)(stream + 6) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((stream[5] + 2)));
  }
  return;

}

/* slot_height_proj @0x80136b64 — project a slot height through the xf
 * transform: load the 8-word matrix xf+0x28..0x44 into cop2 RT+TR ctrl
 * regs, scale sin(obj+0x2a angle) by _DAT_801f0654, emit the result
 * for the idx*0x68 slot record. */
void slot_height_proj(uint32_t a0,int obj,int idx,int xf)
{
  short ang;
  int off;
  uint32_t res;
  uint32_t cop2_idx0 = 0;
  uint32_t at_out = 0x800;
  int tmp;
  int scl;
  /* at_val keeps the high half of GTE data reg 1 (cop2 idx 0x800>>11):
   * the guest reads the register file, replaces the low half, writes it
   * back — so seed from the live COP2 state, not a host local. */
  uint32_t at_val = (uint32_t)getCopReg(2, at_out);
  off = idx * 0x68;
  ang = *(short *)guest_at((uint32_t)(uintptr_t)((obj + 0x2a)));
  setCopControlWord(2,0,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x28))));
  setCopControlWord(2,0x800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x2c))));
  setCopControlWord(2,0x1000,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x30))));
  setCopControlWord(2,0x1800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x34))));
  setCopControlWord(2,0x2000,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x38))));
  setCopControlWord(2,0x2800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x3c))));
  setCopControlWord(2,0x3000,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x40))));
  setCopControlWord(2,0x3800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x44))));
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
  setCopReg(2,cop2_idx0,res);
  setCopReg(2,at_out,at_val);
  copFunction(2,0x180001);
  tmp = getCopReg(2,0xe);
  if (tmp < 0) {
    tmp = tmp + 0xffff;
  }
  *(short *)(&DAT_801f066e + off) = (short)((uint32_t)tmp >> 0x10);
  idx = idx * 0x2150;
  (&DAT_801e40b9)[idx] = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((off + -0x7fe0f944)));
  (&DAT_801e40ba)[idx] = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((off + -0x7fe0f943)));
  (&DAT_801e40bb)[idx] = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((off + -0x7fe0f942)));
  if (*(short *)(&DAT_801f066e + off) < 0) {
    *(uint16_t *)(&DAT_801f066e + off) = 0;
  }
  else if (*(short *)guest_at((uint32_t)(uintptr_t)((xf + 0x22) ))< *(short *)(&DAT_801f066e + off)) {
    *(uint16_t *)(&DAT_801f066e + off) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((xf + 0x22)));
  }
  return;

}

/* battle_intro_fsm @0x80137098 — battle intro state machine: pool/task
 * init phases then per-frame intro camera/fx sequencing. */
void battle_intro_fsm(uint32_t mat,uint32_t a1,uint32_t a2,int st)
{
  char cnt;
  int fx;
  uint32_t grp;
  int *pa;
  int *pb;
  int i;
  int t7;
  int *pp;
  if (*(char *)guest_at((uint32_t)(uintptr_t)((st + 0xb1) ))< '\0') {
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
    DAT_801bef20 = (uint32_t)(*(short *)guest_at((uint32_t)(uintptr_t)((st + 0x82) ))!= 0);
    i = 0;
    t7 = 0;
    do {
      cnt = (&DAT_801f0ad8)[i];
      if (cnt != '\0') {
        (&DAT_801f0ad8)[i] = cnt + -1;
      }
      if ((&DAT_1f8000eb)[t7] == '\b') {
        fx = noop_c(*(uint32_t *)(&DAT_801f0750 + (uint32_t)(uint8_t)(&DAT_1f8000ea)[t7] * 4)
                            );
        if ((*(uint32_t *)(&DAT_1f800148 + t7) & 2) == 0) {
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((fx + 0x14) ))= 0;
        }
        else {
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((fx + 0x14) ))= 1;
        }
      }
      i = i + 1;
      t7 = t7 + 0x108;
    } while (i < 2);
    gp_slot48_set(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 8))));
    gp_slot24_set(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((st + 0xc))));
    gte_matrix_base_set(mat,mat);
    angle_pair_rotmat(-(int)_DAT_801f0298,(int)_DAT_801f029a,(uint32_t *)(uintptr_t)&DAT_801f0760);
    gte_rot_load((uint32_t *)(uintptr_t)&DAT_801be838);
    gte_tr_load(DAT_801be838);
    task_pools_tick(2);
    task_slot_fns_run(1);
    return;
  }
  _DAT_801f0738 = 3;
  task_node_alloc(1,2,9,0x80137590,0,0);
  _DAT_801f0734 = 0;
  t7 = (int)DAT_1f800098;
  _DAT_801f0730 = (int)*(short *)(&DAT_801b17f4 + t7 * 2);
  _DAT_801f0740 = 0;
  _DAT_801f0744 = 0;
  i = t7 * 8;
  _DAT_801f0938 = *(short *)(&DAT_801b1528 + i) + -200;
  _DAT_801f093a = *(uint16_t *)(&DAT_801b1528 + i);
  _DAT_801f0748 = 0;
  _DAT_801f093c = *(short *)(&DAT_801b152a + i) + -200;
  _DAT_801f093e = *(uint16_t *)(&DAT_801b152a + i);
  if (DAT_1f8000b1 == '\0') {
    switch(t7) {
    case 0:
      grp = 0;
      break;
    default:
      goto switchD_80137384_caseD_1;
    case 3:
      fx_group_alloc(2,1,3,1);
      goto LAB_801373c0;
    case 6:
      grp = 6;
    }
    orbit_fx_spawn(grp);
  }
switchD_80137384_caseD_1:
LAB_801373c0:
  i = 0;
  pp = (int *)&DAT_801f0750;
  t7 = 0;
  do {
    if ((&DAT_1f8000eb)[t7] == '\x04') {
      if ((&DAT_1f8000ec)[t7] == '\x02') {
        iris_wipe_spawn((int)guest_of(&DAT_1f8000e8 + t7),0xfffffff);
      }
    }
    else if ((&DAT_1f8000eb)[t7] == '\b') {
      fx = fx_group_alloc(1,1,i + 1,8);
      *pp = fx;
      if (fx != 0) {
        node_param_quad_set(*pp,(int)guest_of(&DAT_1f8000e8 + t7),*(int *)(&DAT_1f800108 + t7) + 0x110,0,0);
      }
    }
    pp = pp + 1;
    i = i + 1;
    t7 = t7 + 0x108;
  } while (i < 2);
  i = 0;
  pb = (int *)&DAT_801f0790;
  pa = (int *)&DAT_801f0780;
  pp = (int *)guest_at(_DAT_801c4508);
  do {
    *pa = *pp;
    i = i + 1;
    *pb = pp[4];
    pp = pp + 1;
    *pa = *pa + (int)_DAT_801c4508;
    pa = pa + 1;
    *pb = *pb + (int)_DAT_801c4508;
    pb = pb + 1;
  } while (i < 4);
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

/* task_alloc_wrap_c @0x80137e70 — task-node alloc wrappers (family
 * _a @0x801725a4/_b @0x80172640/_c @0x80137e70/_d @0x80137f0c/
 * _e @0x80137fb8/_f @0x80138040) binding guest fn pointers
 * (0x80137680/0x80137824/0x80137acc/0x80137cd0 family);
 * payload words w0.. are copied verbatim into the node pay area
 * (except _e, which derives pay[0] and rearranges w1/w2). */
int task_alloc_wrap_c(uint32_t w0,uint32_t w1,int w2,uint32_t w3)
{
  int node;
  uint32_t *pay;
  node = (int)(intptr_t)task_node_alloc(1,2,6,0x80137680,0,0x1c);
  if (node != 0) {
    pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = w0;
    pay[1] = w1;
    pay[2] = w2;
    pay[3] = w3;
    pay[4] = w2 + -4;
    pay[5] = 0;
    pay[6] = 0;
  }
  return node;

}

/* task_alloc_wrap_d @0x80137f0c — see task_alloc_wrap_c (5-word payload variant). */
int task_alloc_wrap_d(uint32_t w0,uint32_t w1,uint32_t w2,uint32_t w3,int w4)
{
  int node;
  uint32_t *pay;
  /* creation-time arg latch: the dest-ptr args must be real guest addrs
   * (>=0x80100000). Recording them lets freeze_check tell a bad br2_gaddr /
   * puVar14 at alloc apart from the stream state being overwritten later. */
  { extern uint32_t g_wd_last_p1,g_wd_last_p2,g_wd_last_p3,g_wd_last_p4,g_wd_last_p5;
    extern uint32_t g_wd_bad_count,g_wd_bad_node,g_wrap_src;
    g_wrap_src=2;
    g_wd_last_p1=(uint32_t)w0; g_wd_last_p2=(uint32_t)w1;
    g_wd_last_p3=(uint32_t)w2; g_wd_last_p4=(uint32_t)w3;
    g_wd_last_p5=(uint32_t)w4;
    if (((uint32_t)w0 < 0x80100000u || (uint32_t)w1 < 0x80100000u ||
         (uint32_t)w2 < 0x80100000u) &&
        (uint32_t)w0 != 0u && (uint32_t)w1 != 0u) {
      g_wd_bad_count++; g_wd_bad_node=(uint32_t)(uintptr_t)w3;
    }
  }
  node = (int)(intptr_t)task_node_alloc(1,2,6,0x80137824,0,0x1c);
  if (node != 0) {
    pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = w0;
    pay[1] = w1;
    pay[2] = w2;
    pay[3] = w3;
    pay[4] = w4;
    pay[5] = w4 + -4;
    *(uint16_t *)(pay + 6) = 0;
    *(uint16_t *)((intptr_t)pay + 0x1a) = 0;
  }
  return node;

}

/* task_alloc_wrap_e @0x80137fb8 — see task_alloc_wrap_c (3-word payload variant). */
int task_alloc_wrap_e(int w0,int w1,int w2)
{
  int res;
  int *pay;
  { extern uint32_t g_wrape_calls,g_wrape_last_ret,g_wrape_p1,g_wrape_p2,g_wrape_p3;
    g_wrape_calls++; g_wrape_p1=(uint32_t)w0;
    g_wrape_p2=(uint32_t)w1; g_wrape_p3=(uint32_t)w2; }
  res = (int)(intptr_t)task_node_alloc(1,2,6,0x80137acc,0,w0 + 0x14);
  if (res == 0) {
    res = 0;
    { extern uint32_t g_wrape_last_ret; g_wrape_last_ret = 0; }
  }
  else {
    { extern uint32_t g_wrape_last_ret; g_wrape_last_ret = (uint32_t)res; }
    pay = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((res + 0x10))));
    pay[1] = w2;
    pay[2] = w2;
    pay[4] = 0;
    pay[3] = w1;
    res = *(int *)guest_at((uint32_t)(uintptr_t)((res + 0x10) ))+ 0x14;
    *pay = res;
  }
  return res;

}

/* task_alloc_wrap_f @0x80138040 — see task_alloc_wrap_c; seq is the keyframe
 * descriptor whose +2 next-index seeds the pay cursor. */
int task_alloc_wrap_f(uint32_t w0,uint32_t w1,uint32_t w2,int seq)
{
  short seq_next;
  int node;
  uint32_t *pay;
  node = (int)(intptr_t)task_node_alloc(1,2,6,0x80137cd0,0,0x18);
  if (node != 0) {
    pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = w0;
    pay[1] = w1;
    pay[2] = w2;
    pay[3] = seq;
    pay[4] = seq;
    seq_next = *(short *)guest_at((uint32_t)(uintptr_t)((seq + 2)));
    *(uint16_t *)((intptr_t)pay + 0x16) = 0;
    *(short *)(pay + 5) = seq_next + 1;
  }
  return node;

}

/* slot_triple_tick @0x8013885c — advance a triple-slot tracker: step the angle
 * cursor through the slot table and reset on wrap. */
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
    *(uint16_t *)(slot + 3) = 0;
  }
  slot[1] = npc;
  if (_DAT_1f800082 == 0) {
    *(short *)((intptr_t)slot + 0xe) = *(short *)((intptr_t)slot + 0xe) + -1;
  }
  if (*(short *)((intptr_t)slot + 0xe) == 0) {
    slot_triple_set((short)slot[2] + 1,0xffffffff,0xffffffff,0xffffffff);
    *(uint32_t *)(&DAT_801f0740 + (short)slot[2] * 4) = 0;
    task_slot_step();
  }
  return;

}

/* xform_accum_tick @0x8013893c — per-tick update for an accumulated-transform
 * record (advance phase, decay, emit on completion). */
void xform_accum_tick(uint32_t *rec)
{
  short cnt;
  _DAT_1f8003f8 = *rec;
  _DAT_1f8003fc = rec[1];
  gte_xform_accum((short *)(uintptr_t)&DAT_1f8003f8,(uint32_t *)(uintptr_t)(rec + 2),1,(int)*(short *)(rec + 4));
  rec_flag1_set((uint16_t *)(uintptr_t)&DAT_1f8003f8);
  if (*(short *)(rec + 4) == 0) {
    task_slot_step();
    _DAT_801f0748 = 0;
  }
  else if (((_DAT_1f800082 == 0) &&
           (cnt = *(short *)((intptr_t)rec + 0x12) + 1, *(short *)((intptr_t)rec + 0x12) = cnt,
           10 < cnt)) &&
          (cnt = *(short *)(rec + 4), *(uint16_t *)(rec + 4) = cnt - 0x88U,
          (int)((uint32_t)(uint16_t)(cnt - 0x88U) << 0x10) < 0)) {
    *(uint16_t *)(rec + 4) = 0;
  }
  return;

}

/* impact_fx_update @0x80138cec — per-tick update for an impact FX record. */
void impact_fx_update(int rec)
{
  /* guest impact_fx_update @0x80138CEC: after the type switch, EVERY
   * non-returning case reaches a computed goto — lw v0,(0x801200B8+type*4);
   * jr v0 — into mid-function continuations (0x80138F48..0x8013A014) that
   * consume the function's live s-register file (s1=record, s3=0x1F8003FC,
   * s4=0x1F8000xx, s5=0x1F8003F8 plus working s0/s2; the prologue saves
   * only s0-s5+ra — no s6/s7/fp).
   * That register state cannot be reconstructed from clean C locals, so the
   * whole body delegates to the generated implementation (faithful per the
   * delegation rule) rather than guessing the jump-table dispatch. */
  call_guest4(0x80138CECu,(uint32_t)rec,0,0,0);
}

/* impact_fx_emit_tick @0x80139e0c — emit pass for an impact FX record. */
void impact_fx_emit_tick(int rec)
{
  uint32_t slot;
  int tmp;
  uint32_t type;
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
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x5a) ))= 0;
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
  impact_fx_spawn((uint32_t *)guest_at((uint32_t)(rec + 0x58)),type,slot,0,0);
  return;

}

/* fx_record_init @0x8013adb4 — initialize an FX record payload to defaults. */
void fx_record_init(uint32_t *rec)
{
  uint16_t *src;
  uint32_t w1;
  if (rec[6] == -0x80000000) {
    src = (uint16_t *)guest_at((uint32_t)(uintptr_t)(rec[2]));
    *(uint16_t *)rec = *src;
    *(uint16_t *)((intptr_t)rec + 2) = src[2];
    *(uint16_t *)(rec + 1) = src[4];
    return;
  }
  w1 = ((uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[2])))[1];
  *rec = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[2]));
  rec[1] = w1;
  return;

}

/* quad_fx_update @0x80141610 — per-tick update for a quad FX record. */
void quad_fx_update(int rec)
{
  short mode;
  uint16_t *tab;
  uint8_t *va;
  uint32_t mat;
  uint8_t *vb;
  if (*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) ))== 0) {
    if (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 8) ))!= 0) {
      task_node_free((int *)(uintptr_t)*(int *)guest_at((uint32_t)(uintptr_t)((rec + 8))));
    }
    task_slot_step();
    return;
  }
  if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x70) ))& 0x2000) != 0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) )))+ 0x14) ))= *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0xc))));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) )))+ 0x18) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))+ 4)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4) )))+ 0x1c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0xc) ))+ 8)));
  }
  if (_DAT_1f800082 == 0) {
    if ((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x70) ))& 0x1000) == 0) {
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
      gte_rtps_xform3((uint32_t *)guest_at((uint32_t)(rec + 0x10)),(uint16_t *)guest_at((uint32_t)(rec + 0x30)),(uint32_t *)(uintptr_t)&DAT_1f8003e0);
      va = (uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x50)));
      mat = *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4))));
      vb = &DAT_1f8003e0;
      goto LAB_801417f8;
    }
    if (mode == 2) {
      gte_rtps_xform3((uint32_t *)(uintptr_t)&DAT_801f0760,(uint16_t *)guest_at((uint32_t)(rec + 0x30)),(uint32_t *)(uintptr_t)&DAT_1f8003e0);
      va = &DAT_1f8003e0;
      mat = *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4))));
      vb = (uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x50)));
      goto LAB_801417f8;
    }
    if (mode != 3) {
      return;
    }
  }
  gte_rtps_xform3((uint32_t *)guest_at((uint32_t)(rec + 0x10)),(uint16_t *)guest_at((uint32_t)(rec + 0x30)),(uint32_t *)(uintptr_t)&DAT_1f8003e0);
  va = (uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x50)));
  mat = *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 4))));
LAB_801417f8:
  gte_rtps_xform3((uint32_t *)(uintptr_t)va,(uint16_t *)(uintptr_t)vb,(uint32_t *)guest_at((uint32_t)mat));
  return;

}

/* angle_from_xy @0x80142b3c — heading angle from delta (x,y) via octant compare
 * and the DAT_801247a8-ish slope math; returns a 12-bit angle. */
int angle_from_xy(int x,int y)
{
  int ay;
  int ax;
  if (x == 0) {
    x = 1;
  }
  if (y == 0) {
    y = 1;
  }
  ax = x;
  if (x < 1) {
    ax = -x;
  }
  ay = y;
  if (y < 1) {
    ay = -y;
  }
  if (ay < ax) {
    if (x < 1) {
      if (y < 1) {
        ay = ax * -8;
      }
      else {
        ay = ax * 8;
      }
      ax = 0xc00 - (y * 0x200 + ay) / ax;
    }
    else {
      if (y < 1) {
        ay = ax * -8;
      }
      else {
        ay = ax * 8;
      }
      ax = (y * 0x200 + ay) / ax + 0x400;
    }
  }
  else if (y < 1) {
    if (0 < x) {
      return (x * 0x200 + ay * 8) / ay;
    }
    ax = (x * 0x200 + ay * -8) / ay + 0xfff;
  }
  else {
    if (x < 1) {
      ax = ay * -8;
    }
    else {
      ax = ay * 8;
    }
    ax = 0x800 - (x * 0x200 + ax) / ay;
  }
  return ax;

}

/* table_1f0ae0_clear @0x80142cd8 — clear entry idx of the 1f0ae0 table. */
void table_1f0ae0_clear(int idx)
{
  int i;
  i = 0;
  idx = idx * 0x168;
  do {
    i = i + 1;
    *(uint16_t *)(&DAT_801f0ae0 + idx) = 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0f51e) ))= 0;
    *(uint16_t *)(&DAT_801f0ae4 + idx) = 0;
    idx = idx + 10;
  } while (i < 0x24);
  return;

}

/* sprite_place_centered @0x80148e70 — position a sprite record centered on (x,y)
 * with size (w,h). */
void sprite_place_centered(uint32_t rec,uint32_t st,short x,short y,int w, int h)
{
  int h_s;
  int w_s;
  h_s = (h << 0x10) >> 0x10;
  w_s = (w << 0x10) >> 0x10;
  sprite_rect_set_c(rec,(int)x - ((w_s - ((w << 0x10) >> 0x1f)) >> 1),
               (int)y - ((h_s - ((h << 0x10) >> 0x1f)) >> 1),w_s,h_s);
  return;

}

/* fighter_tint_fade @0x8016d244 — fade a fighter's tint/flash record over time for
 * the scene/idx slot. */
void fighter_tint_fade(int scene,int idx,int battle,int anim)
{
  uint8_t th1;
  uint8_t th2;
  int rec;
  uint8_t fade;
  int tint;
  anim = anim * 0x1a;
  rec = idx * 0x68;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((battle + 0x82) ))!= 0) {
    tint = (int)_DAT_801f4cb4;
    goto LAB_8016d2ec;
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
LAB_8016d2ec:
  *(bool *)guest_at((uint32_t)(uintptr_t)((battle + 0x9f) ))= tint == 0xff;
  if ((tint != _DAT_801f4cb8) || (tint != _DAT_801f4cba)) {
    _DAT_801f4cba = _DAT_801f4cb8;
    _DAT_801f4cb8 = (short)tint;
    th1 = (&DAT_8011fea2)[anim];
    th2 = (&DAT_8011fea1)[anim];
    if ((int)((uint32_t)(uint8_t)(&DAT_8011fea0)[anim] - tint) < 0) {
      (&DAT_801f0664)[rec] = 0;
    }
    else {
      (&DAT_801f0664)[rec] = (char)((uint32_t)(uint8_t)(&DAT_8011fea0)[anim] - tint);
    }
    if ((int)((uint32_t)th2 - tint) < 0) {
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99b) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99b) ))= (char)((uint32_t)th2 - tint);
    }
    if ((int)((uint32_t)th1 - tint) < 0) {
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99a) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f99a) ))= (char)((uint32_t)th1 - tint);
    }
    th1 = (&DAT_8011feae)[anim];
    th2 = (&DAT_8011fead)[anim];
    if ((int)((uint32_t)(uint8_t)(&DAT_8011feac)[anim] - tint) < 0) {
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f944) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f944) ))= (char)((uint32_t)(uint8_t)(&DAT_8011feac)[anim] - tint);
    }
    if ((int)((uint32_t)th2 - tint) < 0) {
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f943) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f943) ))= (char)((uint32_t)th2 - tint);
    }
    if ((int)((uint32_t)th1 - tint) < 0) {
      *(uint8_t *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f942) ))= 0;
    }
    else {
      *(char *)guest_at((uint32_t)(uintptr_t)((rec + -0x7fe0f942) ))= (char)((uint32_t)th1 - tint);
    }
    if (tint < 0x40) {
      fade = 0;
    }
    else {
      fade = (uint8_t)(((tint + -0x40) * 0x100) / 0xc0);
    }
    idx = idx * 0x58;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b310) ))= fade;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b30f) ))= fade;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b30e) ))= fade;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b2f8) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((idx + -0x7fe0b2f8) ))& 0xff000000 | *(uint32_t *)guest_at((uint32_t)(uintptr_t)((scene + 0x207c) ))& 0xffffff;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((scene + 0x207c) ))=
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((scene + 0x207c) ))& 0xff000000 | guest_of(&DAT_801f4cbc + idx) & 0xffffff;
  }
  return;

}

/* hud_text_blk_alt @0x8012e078 — 0x1f80038c text-block variant of
 * hud_text_blk: near-identical field init except +0x0e (_DAT_1f80039a)
 * = 0xffff (vs 8), str is a host pointer converted via guest_of,
 * and font is passed unmasked. */
void hud_text_blk_alt(uint32_t ot,uint16_t font,const void *str,uint16_t a3, uint16_t arg5,short tm)
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
  _DAT_1f8003a2 = (tm + 0x1e0U & 0x1ff) * 0x40 + 0x1f;
  _DAT_1f800394 = arg5;
  _DAT_1f800392 = a3;
  _DAT_1f8003a8 = guest_of(str);
  text_render_simple((uint32_t *)guest_at(ot),font,0x1f80038c);
  return;

}

/* hud_text_blk — 0x1f80038c text-block variant: +0x1c = raw string guest
 * addr, +0xa..0x10 all 8, +0x16 timing formula; disasm 0x8012e100-0x8012e180. */
void hud_text_blk(uint32_t ot,uint16_t font,uint32_t str,uint16_t a3, uint16_t arg5,short tm)
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
  _DAT_1f8003a2 = (tm + 0x1e0U & 0x1ff) * 0x40 + 0x1f;
  _DAT_1f8003a4 = 0;
  _DAT_1f8003a8 = str;
  text_render_simple((uint32_t *)guest_at(ot),font & 0xffff,0x1f80038c);
  return;
}

/* please_wait_announce — "PLEASE WAIT" announce when record flag bit 0x800 set
 * and 0x1f8000dc mode bits 4-5 nonzero; disasm 0x801524b0-0x80152440. */
void please_wait_announce(uint32_t ent,int en)
{
  uint8_t *rec = (uint8_t *)guest_at(ent);
  int32_t yadj;
  uint32_t fl;
  if (rec == NULL) return;
  if (en) yadj = (int32_t)*(int16_t *)(rec + 0x20) - 0x50;
  else    yadj = 0x50;
  fl = *(uint16_t *)((uint8_t *)guest_at(*(uint32_t *)(rec + 0xc8)) + 2);
  if (((fl >> 0xb) & 1) == 0) return;
  if (((_DAT_1f8000dc >> 4) & 3) == 0) return;
  hud_text_blk_alt(*(uint32_t *)(rec + 8),*(uint16_t *)(rec + 0xc),
               (const void *)guest_at(0x8012098c),
               (uint16_t)(int16_t)(yadj - 0x21),0x20,1);
}

/* ctr_clamp — signed counter adjust on a0+0xa0 gated by mode uint8_t
 * a0+0x9c (mode 0, mode>5, or mode==5&&+0x9b==0); clamps result to
 * [0,0xff] (negatives stored as 0).
 * Disasm 0x80159eec-0x80159f58. */
void ctr_clamp(uint32_t ent,int32_t delta)
{
  uint8_t *rec = (uint8_t *)guest_at(ent);
  int mode, adj = 0;
  if (rec == NULL) return;
  mode = rec[0x9c];
  if (mode < 5) { if (mode == 0) adj = 1; }
  else if (mode != 5 || rec[0x9b] == 0) adj = 1;
  if (!adj) return;
  delta += *(int16_t *)(rec + 0xa0);
  if (delta < 0) { *(int16_t *)(rec + 0xa0) = 0; return; }
  *(int16_t *)(rec + 0xa0) = (delta < 0x100) ? (int16_t)delta : (int16_t)0xff;
}

/* announce_param_block_init_b @0x8012e334 — init the 0x1f80038c announce
 * param block and render the full text string. */
void announce_param_block_init_b(uint32_t ot,uint16_t font,const void *str,uint16_t a3, uint16_t arg5,short tm)
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
  _DAT_1f8003a2 = (tm + 0x1e0U & 0x1ff) * 0x40 + 0x1f;
  _DAT_1f800394 = arg5;
  _DAT_1f800392 = a3;
  _DAT_1f8003a8 = guest_of(str);
  text_render_full((uint32_t *)guest_at(ot),font,0x1f80038c);
  return;

}

/* slot_triple_set @0x8012ea18 — load the three cursor values for a triple-slot
 * tracker index. */
void slot_triple_set(int slot,int c0,int c1,int c2)
{
  int base;
  int off;
  base = _DAT_801f0228;
  if (c0 == -1) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((slot * 8 + 0x1f800324) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((_DAT_801f0228 + slot * 8 + 0x2c)));
  }
  else {
    *(short *)guest_at((uint32_t)(uintptr_t)((slot * 8 + 0x1f800324) ))= (short)c0;
  }
  off = slot * 8;
  if (c1 == -1) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((off + 0x1f800326) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + off + 0x2e)));
  }
  else {
    *(short *)guest_at((uint32_t)(uintptr_t)((off + 0x1f800326) ))= (short)c1;
  }
  slot = slot * 8;
  if (c2 == -1) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((slot + 0x1f800328) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((base + slot + 0x30)));
    return;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((slot + 0x1f800328) ))= (short)c2;
  return;

}

/* rec_flag1_set @0x8012ee0c — latch a 3-word record into the
 * _DAT_801f022c/234/236/238 globals and arm the flag. */
void rec_flag1_set(uint16_t *src)
{
  _DAT_801f022c = 1;
  _DAT_801f0234 = *src;
  _DAT_801f0236 = src[1];
  _DAT_801f0238 = src[2];
  return;

}

/* target_angles_set @0x80132084 — target_angles_/depth_/pair_depth_/midpoint_angles_ —
 * camera target helpers: write aim angles/depth for a target idx or
 * midpoint of a fighter pair into dst buffers. */
void target_angles_set(short *dst,int idx)
{
  short y0;
  short y1;
  uint16_t ang;
  uint32_t d2;
  int dz;
  int dx;
  dx = (int)*dst - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  dz = (int)dst[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  y0 = dst[1];
  y1 = *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  d2 = FUN_80194500(dx * dx + dz * dz);
  ang = atan2_lut(d2,(int)y0 - (int)y1);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x30) ))= ang;
  ang = atan2_lut(dz,dx);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x32) ))= ang;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x34) ))= 0;
  return;

}

/* target_depth_set @0x80132224 — see target_angles_set family. */
void target_depth_set(short *dst,int idx)
{
  int dep2;
  int dep;
  _DAT_1f80038c = *dst - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f80038e = dst[1] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800390 = dst[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  {
    uint32_t *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x28))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x2a))),
                       (uint32_t *)(uintptr_t)&DAT_1f8003a0);
    gte_rtps_xform1_raw((uint32_t *)rm,(uint32_t *)(uintptr_t)&DAT_1f80038c,(uint32_t *)(uintptr_t)&DAT_1f800394);
  }
  dep = _DAT_1f800394;
  if ((int32_t)_DAT_1f800394 < 0) {
    dep = -(int32_t)_DAT_1f800394;
  }
  dep2 = _DAT_1f800398;
  if ((int32_t)_DAT_1f800398 < 0) {
    dep2 = -(int32_t)_DAT_1f800398;
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

/* pair_depth_set @0x8013237c — see target_angles_set family. */
void pair_depth_set(short *pa,short *pb,int idx)
{
  short dmin;
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
    uint32_t *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x28))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x2a))),
                       &DAT_1f8003b4);
    gte_rtps_xform1_raw((uint32_t *)rm,(uint32_t *)(uintptr_t)&DAT_1f80038c,(uint32_t *)(uintptr_t)&DAT_1f80039c);
  }
  gte_xform1_ir(&DAT_1f800394,&DAT_1f8003a8);
  ax = _DAT_1f80039c;
  if ((int32_t)_DAT_1f80039c < 0) {
    ax = -(int32_t)_DAT_1f80039c;
  }
  ay = _DAT_1f8003a8;
  if ((int32_t)_DAT_1f8003a8 < 0) {
    ay = -(int32_t)_DAT_1f8003a8;
  }
  if (ay <= ax) {
    ay = ax;
  }
  ax = _DAT_1f8003a0;
  if ((int32_t)_DAT_1f8003a0 < 0) {
    ax = -(int32_t)_DAT_1f8003a0;
  }
  az = _DAT_1f8003ac;
  if ((int32_t)_DAT_1f8003ac < 0) {
    az = -(int32_t)_DAT_1f8003ac;
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
  if ((int32_t)_DAT_1f8003a4 < (int32_t)_DAT_1f8003b0) {
    dmin = _DAT_1f8003a4;
  }
  else {
    dmin = _DAT_1f8003b0;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x68) ))= (short)ay - dmin;
  return;

}

/* target_angles_depth_set @0x8013258c — see target_angles_set family. */
void target_angles_depth_set(short *dst,int idx)
{
  short dep;
  uint16_t ang;
  uint32_t d2;
  int t4;
  int dx;
  int dz;
  dx = (int)*dst - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f80038c = (uint16_t)dx;
  t4 = (int)dst[1] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f80038e = (uint16_t)t4;
  dz = (int)dst[2] - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  _DAT_1f800390 = (uint16_t)dz;
  d2 = FUN_80194500(dx * dx + dz * dz);
  ang = atan2_lut(d2,t4);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x30) ))= ang;
  ang = atan2_lut(dz,dx);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x32) ))= ang;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x34) ))= 0;
  {
    uint32_t *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x30))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x32))),
                       &DAT_1f8003a0);
    gte_rtps_xform1_raw((uint32_t *)rm,(uint32_t *)(uintptr_t)&DAT_1f80038c,(uint32_t *)(uintptr_t)&DAT_1f800394);
  }
  dep = _DAT_1f80039c;
  t4 = _DAT_1f800394;
  if ((int32_t)_DAT_1f800394 < 0) {
    t4 = -(int32_t)_DAT_1f800394;
  }
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x66) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 100) ))= 0;
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x68) ))= (short)(((t4 + 0x180) * 0x168) / 0x140) - dep;
  return;

}

/* midpoint_angles_set @0x801326d4 — see target_angles_set family. */
void midpoint_angles_set(int *pa,int *pb,int idx)
{
  short s1;
  short mz;
  uint16_t ang;
  uint32_t d2;
  short my;
  int ad2;
  int ad;
  int dep;
  s1 = (short)((*pa + *pb) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x42) ))= s1;
  ad = (int)s1 - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  my = (short)((pa[1] + pb[1]) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x44) ))= my;
  mz = (short)((pa[2] + pb[2]) / 2);
  dep = (int)mz - (int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  s1 = *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x46) ))= mz;
  d2 = FUN_80194500(ad * ad + dep * dep);
  ang = atan2_lut(d2,(int)my - (int)s1);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x30) ))= ang;
  ang = atan2_lut(dep,ad);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x32) ))= ang;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((idx + 0x34) ))= 0;
  _DAT_1f80038c = (short)*pa - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f80038e = (short)pa[1] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800390 = (short)pa[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  _DAT_1f800394 = (short)*pb - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4a)));
  _DAT_1f800396 = (short)pb[1] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4c)));
  _DAT_1f800398 = (short)pb[2] - *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x4e)));
  {
    uint32_t *rm = rotmat2_pair_build((int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x30))),-(int)*(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x32))),
                       &DAT_1f8003b4);
    gte_rtps_xform1_raw((uint32_t *)rm,(uint32_t *)(uintptr_t)&DAT_1f80038c,(uint32_t *)(uintptr_t)&DAT_1f80039c);
  }
  gte_xform1_ir(&DAT_1f800394,&DAT_1f8003a8);
  ad = _DAT_1f80039c;
  if ((int32_t)_DAT_1f80039c < 0) {
    ad = -(int32_t)_DAT_1f80039c;
  }
  dep = _DAT_1f8003a8;
  if ((int32_t)_DAT_1f8003a8 < 0) {
    dep = -(int32_t)_DAT_1f8003a8;
  }
  if (dep <= ad) {
    dep = ad;
  }
  ad = _DAT_1f8003a0;
  if ((int32_t)_DAT_1f8003a0 < 0) {
    ad = -(int32_t)_DAT_1f8003a0;
  }
  ad2 = _DAT_1f8003ac;
  if ((int32_t)_DAT_1f8003ac < 0) {
    ad2 = -(int32_t)_DAT_1f8003ac;
  }
  dep = ((dep * 2 + 0x180) * 0x168) / 0x140;
  if (ad2 <= ad) {
    ad2 = ad;
  }
  ad = ((ad2 * 2 + 0x180) * 0x168) / 0xf0;
  if (ad <= dep) {
    ad = dep;
  }
  dep = 0x4a4;
  if (0x4a3 < ad) {
    dep = ad;
  }
  if ((int32_t)_DAT_1f8003a4 < (int32_t)_DAT_1f8003b0) {
    s1 = _DAT_1f8003a4;
  }
  else {
    s1 = _DAT_1f8003b0;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((idx + 0x68) ))= (short)dep - s1;
  return;

}

/* dual_target_track @0x801329d8(_b) — dual-target camera tracking:
 * compute midpoint angles/depth between two fighters and steer the
 * camera target (tgt record or cam+fa+fb variant). */
void dual_target_track(int tgt,int f_a,int f_b)
{
  short my;
  int t;
  int t3;
  int bz;
  int dx;
  uint32_t nang;
  int dz;
  int bx;
  int by;
  int ax;
  int ay;
  /* Guest vec3 slots: FUN_80194630 / pair_depth_set read and write 3
   * contiguous u32 through tpa (delta / first target pos) and
   * tpb (normalized out / second target pos); the decompiler
   * split each 12-uint8_t vec3 into scalars. tpa[0..2] = old
   * tpa/local_44/local_40; tpb[0..2] = old
   * tpb/local_34/local_30. */
  int tpa [3];
  int tpb [3];
  t3 = *(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20)));
  ay = *(int *)guest_at((uint32_t)(uintptr_t)((t3 + 0x178)));
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f_a + 0x5c) ))& 0x40000) == 0) {
    ax = *(int *)guest_at((uint32_t)(uintptr_t)((t3 + 0x34)));
    t3 = *(int *)guest_at((uint32_t)(uintptr_t)((t3 + 0x3c)));
  }
  else {
    ax = *(int *)guest_at((uint32_t)(uintptr_t)((t3 + 0x174)));
    t3 = *(int *)guest_at((uint32_t)(uintptr_t)((t3 + 0x17c)));
  }
  bz = *(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20)));
  by = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x178)));
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f_b + 0x5c) ))& 0x40000) == 0) {
    bx = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x34)));
    bz = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x3c)));
  }
  else {
    bx = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x174)));
    bz = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x17c)));
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x52) ))= (short)((bx + ax) / 2);
  my = (short)((by + ay) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x54) ))= my;
  if (((-0x100 < my) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f_a + 0x5c) ))& 0x80000) == 0)) &&
     ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((f_b + 0x5c) ))& 0x80000) == 0)) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((tgt + 0x54) ))= 0xff00;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x56) ))= (short)((bz + t3) / 2);
  tpa[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x52) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4a)));
  tpa[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x54) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4c)));
  tpa[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x56) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4e)));
  t = FUN_80194630((uint32_t *)tpa,(uint32_t *)tpb);
  dz = bz - t3;
  if (t != 0) {
    t = FUN_80194500((uint32_t)t);
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4a) ))+ (short)((uint32_t)(tpb[0] * t) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4c) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4c) ))+ (short)((uint32_t)(tpb[1] * t) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x4e) ))+ (short)((uint32_t)(tpb[2] * t) >> 0x10);
  }
  dx = bx - ax;
  t = FUN_80194500(dz * dz + dx * dx);
  if (0x80 < t) {
    t = atan2_lut(dx,dz);
    nang = -t;
    t = (*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x2a) ))+ t) * 0x100000 >> 0x14;
    if (t < 0) {
      t = -t;
    }
    if (0x400 < t) {
      nang = nang + 0x800;
    }
    t = (int)(((int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x2a) ))- (nang & 0xfff)) * 0x100000) >> 0x14;
    if (t < 0) {
      t = -t;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x32) ))= (short)(nang & 0xfff);
    if (t < 0) {
      t = t + 0x3f;
    }
    t = t >> 6;
    if (t < 1) {
      t = 1;
    }
    fixed_approach_s16((uint16_t *)guest_at((uint32_t)(tgt + 0x2a)),(short *)guest_at((uint32_t)(tgt + 0x32)),(int)(short)t);
  }
  t = *(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x28) ))+ -0x5b;
  if (t < 0) {
    t = -t;
  }
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((tgt + 0x30) ))= 0x5b;
  if (t < 0) {
    t = t + 0x1f;
  }
  t = t >> 5;
  if (t < 1) {
    t = 1;
  }
  fixed_approach_s16((uint16_t *)guest_at((uint32_t)(tgt + 0x28)),(short *)guest_at((uint32_t)(tgt + 0x30)),(int)(short)t);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((tgt + 0x34) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((tgt + 0x2c) ))= 0;
  if (-0x80 < ay) {
    ay = -0x80;
  }
  if (-0x80 < by) {
    by = -0x80;
  }
  tpa[0] = ax;
  tpa[1] = ay;
  tpa[2] = t3;
  tpb[0] = bx;
  tpb[1] = by;
  tpb[2] = bz;
  pair_depth_set((short *)tpa,(short *)tpb,tgt);
  t3 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x60) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x68)));
  if ((int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x60) ))< (int)*(short *)guest_at((uint32_t)(uintptr_t)((tgt + 0x68)))) {
    if (t3 < 0) {
      t3 = -t3;
    }
    if (t3 < 0) {
      t3 = t3 + 3;
    }
    t3 = t3 << 0xe;
  }
  else {
    if (t3 < 0) {
      t3 = -t3;
    }
    if (t3 < 0) {
      t3 = t3 + 0xf;
    }
    t3 = t3 << 0xc;
  }
  u16_step_toward((uint16_t *)guest_at((uint32_t)(tgt + 0x60)),(uint16_t *)guest_at((uint32_t)(tgt + 0x68)),t3 >> 0x10);
  return;

}

/* dual_target_track_b @0x80132d70 — see dual_target_track. */
void dual_target_track_b(int cam,int fa,int fb)
{
  short s1;
  uint16_t wang;
  int t;
  int az;
  int bz;
  uint16_t nang;
  int dx;
  int tmp;
  int bx;
  int by;
  int ax;
  int ay;
  /* Guest vec3 slots: pair_depth_set reads 3 contiguous u32 through
   * tpa (first target pos) and tpb (second target pos); the
   * decompiler split each 12-uint8_t vec3 into scalars. tpa[0..2] =
   * old tpa/local_44/local_40; tpb[0..2] = old
   * tpb/local_34/local_30. */
  int tpa [3];
  int tpb [3];
  az = *(int *)guest_at((uint32_t)(uintptr_t)((fa + 0x20)));
  ay = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x178)));
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fa + 0x5c) ))& 0x40000) == 0) {
    ax = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x34)));
    az = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x3c)));
  }
  else {
    ax = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x174)));
    az = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x17c)));
  }
  bz = *(int *)guest_at((uint32_t)(uintptr_t)((fb + 0x20)));
  by = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x178)));
  if ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fb + 0x5c) ))& 0x40000) == 0) {
    bx = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x34)));
    bz = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x3c)));
  }
  else {
    bx = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x174)));
    bz = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x17c)));
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= (short)((bx + ax) / 2);
  s1 = (short)((by + ay) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= s1;
  tmp = bz - az;
  if (s1 < -0xff) {
LAB_80132e74:
    t = tmp * tmp;
  }
  else {
    t = tmp * tmp;
    if (((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fa + 0x5c) ))& 0x80000) == 0) && ((*(uint32_t *)guest_at((uint32_t)(uintptr_t)((fb + 0x5c) ))& 0x80000) == 0)
       ) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
      tmp = bz - az;
      goto LAB_80132e74;
    }
  }
  dx = bx - ax;
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= (short)((bz + az) / 2);
  t = FUN_80194500(t + dx * dx);
  if (t < 0x81) goto LAB_80132f50;
  tmp = atan2_lut(dx,tmp);
  nang = -(short)tmp;
  s1 = *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x82)));
  if (s1 == -2) {
    wang = nang + 0x800;
LAB_80132f40:
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= wang;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= wang;
  }
  else {
    if (s1 < -1) {
      if (s1 != -3) {
        *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x82) ))= 0;
        goto LAB_80132f50;
      }
      tmp = (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ tmp) * 0x100000 >> 0x14;
      if (tmp < 0) {
        tmp = -tmp;
      }
      wang = nang & 0xfff;
      if (0x400 < tmp) {
        wang = nang + 0x800 & 0xfff;
      }
      goto LAB_80132f40;
    }
    if (s1 == -1) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= nang;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))= nang;
    }
  }
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x82) ))= 0;
LAB_80132f50:
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
  if (-0x80 < ay) {
    ay = -0x80;
  }
  if (-0x80 < by) {
    by = -0x80;
  }
  tpa[0] = ax;
  tpa[1] = ay;
  tpa[2] = az;
  tpb[0] = bx;
  tpb[1] = by;
  tpb[2] = bz;
  pair_depth_set((short *)tpa,(short *)tpb,cam);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
  return;

}

/* camera_track_shot @0x80132fdc/step — scripted camera shot tracking
 * and per-frame camera step toward the tracked target. */
void camera_track_shot(int cam,int f_a,int f_b)
{
  uint16_t pit;
  short prev;
  uint32_t r;
  int t;
  /* Guest vec3 slot: FUN_80194630 reads/writes 3 contiguous u32
   * through delta in place; the decompiler split the 12-uint8_t vec3
   * into scalars. delta[0..2] = old delta/local_2c/local_28. */
  int delta [3];
  if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))== 0) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x178)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x17c)));
    if (-0x100 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)))) {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= 0xff00;
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
    pit = *(uint16_t *)(&DAT_8011fd7a + r * 4);
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= 0xfc00;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= 0xfc00;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= 0xfc00;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))= pit;
    pair_depth_set((short *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174)),(short *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20) ))+ 0x174)),(int)(uintptr_t)cam);
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 100)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x5e) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x66)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))+ 1;
  }
  else if (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x78) ))!= 1) goto LAB_801332f0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x178)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x17c)));
  if (-0x80 < *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x54)))) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= 0xff80;
  }
  delta[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a)));
  delta[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)));
  delta[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e)));
  t = FUN_80194630((uint32_t *)delta,(uint32_t *)delta);
  if (t != 0) {
    t = FUN_80194500(t);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))+ (short)((uint32_t)(delta[0] * t) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))+ (short)((uint32_t)(delta[1] * t) >> 0x10);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))=
         *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))+ (short)((uint32_t)(delta[2] * t) >> 0x10);
  }
  pair_depth_set((short *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_a + 0x20) ))+ 0x174)),(short *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((f_b + 0x20) ))+ 0x174)),(int)(uintptr_t)cam);
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
  u16_step_toward((uint16_t *)guest_at((uint32_t)(cam + 0x60)),(uint16_t *)guest_at((uint32_t)(cam + 0x68)),t >> 0x10);
LAB_801332f0:
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x20) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x28)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x22) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x24) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c)));
  return;

}

/* camera_track_step @0x80133328 — see camera_track_shot. */
void camera_track_step(int cam,int fa,int fb)
{
  uint16_t stp;
  short my;
  int t;
  int dst;
  int bz;
  uint32_t nang;
  int az;
  int t8;
  int by;
  int ay;
  int ax;
  int bx;
  /* Guest vec3 slots: FUN_80194630 / pair_depth_set read and write 3
   * contiguous u32 through tpa (delta / first target pos) and
   * tpb (normalized out / second target pos); the decompiler
   * split each 12-uint8_t vec3 into scalars. tpa[0..2] = old
   * tpa/local_4c/local_48; tpb[0..2] = old
   * tpb/local_3c/local_38. bx stays a scalar. */
  int tpa [3];
  int tpb [3];
  az = *(int *)guest_at((uint32_t)(uintptr_t)((fa + 0x20)));
  bz = *(int *)guest_at((uint32_t)(uintptr_t)((fb + 0x20)));
  ax = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x174)));
  bx = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x174)));
  ay = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x178)));
  by = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x178)));
  az = *(int *)guest_at((uint32_t)(uintptr_t)((az + 0x17c)));
  bz = *(int *)guest_at((uint32_t)(uintptr_t)((bz + 0x17c)));
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))= (short)((bx + ax) / 2);
  my = (short)((by + ay) / 2);
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= my;
  if (-0x100 < my) {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))= 0xff00;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))= (short)((bz + az) / 2);
  tpa[0] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x52) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a)));
  tpa[1] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x54) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c)));
  tpa[2] = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x56) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e)));
  t = FUN_80194630((uint32_t *)tpa,(uint32_t *)tpb);
  if (t != 0) {
    t = FUN_80194500(t);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4a) ))+ (short)(tpb[0] * t >> 0xc);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4c) ))+ (short)(tpb[1] * t >> 0xc);
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))= *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x4e) ))+ (short)(tpb[2] * t >> 0xc);
  }
  t = bz - az;
  t8 = bx - ax;
  dst = FUN_80194500(t * t + t8 * t8);
  if (0x80 < dst) {
    t = atan2_lut(t8,t);
    nang = -t;
    t = (*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))+ t) * 0x100000 >> 0x14;
    if (t < 0) {
      t = -t;
    }
    if (0x400 < t) {
      nang = nang + 0x800;
    }
    nang = nang & 0xfff;
    stp = *(uint16_t *)(&DAT_8011fd98 + ((int)((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))- nang) >> 9 & 6));
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= (short)nang;
    t8 = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x2a) ))- ((int)((nang + stp) * 0x10000) >> 0x10)) *
            0x100000 >> 0x14;
    if (t8 < 0) {
      t8 = -t8;
    }
    *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x32) ))= (short)(nang + stp);
    if (t8 < 0) {
      t8 = t8 + 7;
    }
    t8 = t8 >> 3;
    if (t8 < 1) {
      t8 = 1;
    }
    fixed_approach_s16((uint16_t *)guest_at((uint32_t)(cam + 0x2a)),(short *)guest_at((uint32_t)(cam + 0x32)),(int)(short)t8);
  }
  t8 = t << 1;
  if (0x400 < t) {
    t = 0x800 - t;
    t8 = t * 2;
  }
  t = (0x400 - (dst + t8 + t)) * 0x2d;
  if (t < 0) {
    t = t + 0xff;
  }
  t = t >> 8;
  if (0 < t) {
    t = 0;
  }
  my = atan2_lut(0x5a0,t);
  *(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x30) ))= -my;
  t = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x28) ))- (int)-my) * 0x100000 >> 0x14;
  if (t < 0) {
    t = -t;
  }
  if (t < 0) {
    t = t + 7;
  }
  t = t >> 3;
  if (t < 1) {
    t = 1;
  }
  fixed_approach_s16((uint16_t *)guest_at((uint32_t)(cam + 0x28)),(short *)guest_at((uint32_t)(cam + 0x30)),(int)(short)t);
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x34) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cam + 0x2c) ))= 0;
  if (-0x80 < ay) {
    ay = -0x80;
  }
  if (-0x80 < by) {
    by = -0x80;
  }
  tpb[0] = bx;
  tpa[0] = ax;
  tpa[1] = ay;
  tpa[2] = az;
  tpb[1] = by;
  tpb[2] = bz;
  pair_depth_set((short *)tpa,(short *)tpb,cam);
  bz = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))- (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)));
  if ((int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x60) ))< (int)*(short *)guest_at((uint32_t)(uintptr_t)((cam + 0x68)))) {
    if (bz < 0) {
      bz = -bz;
    }
    if (bz < 0) {
      bz = bz + 3;
    }
    bz = bz << 0xe;
  }
  else {
    if (bz < 0) {
      bz = -bz;
    }
    if (bz < 0) {
      bz = bz + 0xf;
    }
    bz = bz << 0xc;
  }
  u16_step_toward((uint16_t *)guest_at((uint32_t)(cam + 0x60)),(uint16_t *)guest_at((uint32_t)(cam + 0x68)),bz >> 0x10);
  return;

}

/* u16_step_toward @0x80133828/fixed_approach_s16 — fixed-point
 * approach helpers: step a u16/s16 value toward target by step;
 * return the signed residual movement (0 only when delta == 0,
 * the reaching step can still return nonzero). */
int u16_step_toward(uint16_t *cur,uint16_t *tgt,short step)
{
  uint16_t old;
  short d;
  int delta;
  old = *cur;
  delta = (uint32_t)*tgt - (uint32_t)old;
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

/* fixed_approach_s16 @0x801338c4 — see u16_step_toward. */
int fixed_approach_s16(uint16_t *cur,short *tgt,uint32_t step)
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
    *cur = (uint16_t)((uint32_t)t20 >> 0x14);
    return diff >> 0x14;
  }
  return 0;

}

/* fighter_trail_emit @0x80135180 — emit fighter trail particles:
 * spawn burst slots along the motion vector with life/width taper. */
void fighter_trail_emit(uint32_t *mat,uint32_t a1,int ent,int spec)
{
  uint16_t lif;
  bool far;
  short o0;
  int t4;
  int nd;
  int q;
  /* t3/sp are dead-path false positives: q is bounded while(q<4) so
   * the q==0..3 chain always assigns t3, and (tp+0x4c & 3) is always
   * <4 so the sp branches cover every case. */
  int t3 = 0;
  short o1;
  int sp = 0;
  int hdg;
  uint8_t *tp;
  int base;
  uint32_t arg;
  /* Guest vec3 slot: gte_xform_add_off writes out[0..2] as 3
   * contiguous int; the decompiler split the 12-uint8_t vec3 into
   * scalars. out[0..2] = old out/local_40/local_3c. */
  uint32_t out [3];
  short off [4];
  uint32_t fl0;
  uint32_t fl1;
  tp = &DAT_801f04a8;
  arg = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((spec + 0xc)));
  base = *(int *)guest_at((uint32_t)(uintptr_t)((spec + 8)));
  hdg = ((int)*(short *)guest_at((uint32_t)(uintptr_t)((ent + 0x2a) ))<< 0x14) >> 0x14;
  setCopControlWord(2,0,*mat);
  setCopControlWord(2,0x800,mat[1]);
  setCopControlWord(2,0x1000,mat[2]);
  setCopControlWord(2,0x1800,mat[3]);
  setCopControlWord(2,0x2000,mat[4]);
  setCopControlWord(2,0x2800,mat[5]);
  setCopControlWord(2,0x3000,mat[6]);
  setCopControlWord(2,0x3800,mat[7]);
  q = 0;
  if (*(char *)guest_at((uint32_t)(uintptr_t)((spec + 0x9e) ))== '\0') {
    fl0 = (uint32_t)(-hdg < 0x4e4);
    fl1 = (uint32_t)(-hdg < 0x31c);
    do {
      if (0 < *(short *)(tp + 0x3c)) {
        off[2] = 0;
        off[1] = 0;
        off[0] = 0;
        if (*(short *)(tp + 0x4c) == 0) {
          t4 = (uint32_t)*(uint16_t *)(tp + 0x3c) -
                  ((((int)((uint32_t)*(uint16_t *)(tp + 0x44) << 0x10) >> 0x10) -
                   ((int)((uint32_t)*(uint16_t *)(tp + 0x44) << 0x10) >> 0x1f)) >> 1);
          *(short *)(tp + 0x3c) = (short)t4;
          if (t4 * 0x10000 < 1) {
LAB_80135314:
            *(uint16_t *)(tp + 0x3c) = 1;
          }
LAB_8013531c:
          *(uint16_t *)(tp + 0x4c) = 0x100;
        }
        else if (*(short *)(tp + 0x4c) < 0) {
          lif = *(uint16_t *)(tp + 0x3c);
          *(short *)(tp + 0x3c) = (short)((uint32_t)lif - (uint32_t)*(uint16_t *)(tp + 0x44));
          if ((int)(((uint32_t)lif - (uint32_t)*(uint16_t *)(tp + 0x44)) * 0x10000) < 1) {
            if ((*(short *)(tp + 0x44) != 10000) || (DAT_801f04c9 != '\0')) goto LAB_80135314;
            particle_burst_spawn(q,(int)*(char *)guest_at((uint32_t)(uintptr_t)((spec + 0x98))));
            *(uint16_t *)(tp + 0x34) = 0xffff;
            *(uint16_t *)(tp + 0x3c) = 0;
            DAT_801f04c9 = '\x01';
            goto LAB_80135630;
          }
          goto LAB_8013531c;
        }
        if ((*(uint16_t *)(tp + 0x4c) & 3) < 2) {
          sp = (int)*(short *)(tp + 0x44);
          if (0x80 < sp) {
            sp = 0x80;
          }
        }
        else if ((*(uint16_t *)(tp + 0x4c) & 3) < 4) {
          sp = 0;
        }
        if (*(short *)guest_at((uint32_t)(uintptr_t)((spec + 0x82) ))== 0) {
          t4 = *(short *)(tp + 0x44) * 7;
          *(short *)(tp + 0x4c) = *(short *)(tp + 0x4c) + 1;
          if (t4 < 0) {
            t4 = t4 + 7;
          }
          *(short *)(tp + 0x44) = (short)(t4 >> 3);
          if ((t4 >> 3 & 0xffffU) == 0) {
            *(uint16_t *)(tp + 0x4c) = 0;
          }
        }
        o1 = (short)sp;
        if (q == 1) {
          o0 = -o1;
          o1 = off[2];
        }
        else {
          o0 = off[0];
          if (q < 2) {
            o0 = o1;
            o1 = off[2];
            if (q != 0) {
              o0 = off[0];
            }
          }
          else if ((q != 2) && (o1 = -o1, q != 3)) {
            o1 = off[2];
          }
        }
        off[2] = o1;
        off[0] = o0;
        if ((DAT_801f04c8 & 1) != 0) {
          far = false;
          if (q == 1) {
            t4 = hdg + -0x400;
LAB_80135460:
            t4 = (t4 << 0x14) >> 0x14;
            if (t4 < 0) {
              if (0x31b < -t4) {
                far = true;
              }
            }
            else {
joined_r0x801354dc:
              if (0x31b < t4) {
LAB_801354fc:
                far = true;
              }
            }
          }
          else {
            if (q < 2) {
              if (q != 0) goto LAB_80135504;
              t4 = hdg + 0x400;
              goto LAB_80135460;
            }
            if (q == 2) {
              if (hdg < 0) {
                if (fl0 != 0) {
                  far = true;
                }
                goto LAB_80135504;
              }
              if (0x4e3 < hdg) goto LAB_80135504;
              goto LAB_801354fc;
            }
            if (q == 3) {
              t4 = hdg;
              if (hdg < 0) {
                if (fl1 == 0) goto LAB_801354fc;
                goto LAB_80135504;
              }
              goto joined_r0x801354dc;
            }
          }
LAB_80135504:
          gte_xform_add_off((int)guest_of(mat),(uint32_t *)off,(int *)out);
          setCopControlWord(2,0x2800,out[0]);
          setCopControlWord(2,0x3000,out[1]);
          setCopControlWord(2,0x3800,out[2]);
          if (*(char *)guest_at((uint32_t)(uintptr_t)((spec + 0x9f) ))== '\0') {
            t3 = (int)*(short *)(tp + 0x34);
            if (((t3 < 0) || (t4 = _DAT_801f04b0 + t3 * 0x28, _DAT_801f04c6 != 0)) ||
               (!far)) goto LAB_80135630;
            nd = base + 0x203c;
          }
          else {
            if (q == 1) {
              t3 = 1;
            }
            else if (q < 2) {
              if (q == 0) {
                t3 = 0;
              }
            }
            else if (q == 2) {
              t3 = 3;
            }
            else if (q == 3) {
              t3 = 2;
            }
            nd = base + 0x203c;
            if (far) {
              t4 = _DAT_801f04c0 + t3 * 0x28;
            }
            else {
              nd = base + 0x18;
              t4 = _DAT_801f04c0 + t3 * 0x28;
            }
          }
          prim_task_run(t4,nd,arg,1);
        }
      }
LAB_80135630:
      q = q + 1;
      tp = tp + 2;
    } while (q < 4);
    if (*(char *)guest_at((uint32_t)(uintptr_t)((spec + 0x9e) ))== '\0') {
      if (*(char *)guest_at((uint32_t)(uintptr_t)((spec + 0x9f) ))== '\0') {
        /* PTR_FUN_801b1734 is the guest u32 code-pointer table @0x801B1734
         * (channel_script_emit_* handlers). It was never populated with host
         * pointers; dispatch the real guest jalr — mat=s7=*(spec+8),
         * a1=fp=*(spec+0xc), ent=0x801F04A8, spec=spec, arg5@sp+16=
         * mat's guest address. */
        {
          int _ti = (int)*(char *)guest_at((uint32_t)(uintptr_t)((spec + 0x98)));
          uint32_t _tf = *(uint32_t *)guest_at(0x801b1734u + (uint32_t)_ti * 4u);
          if (_tf != 0) {
            uint32_t _fr = br2_guest_frame(0x20);
            br2_st32(_fr + 0x10, br2_gaddr(mat));
            br2_call_clean(_tf,(uint32_t)base,arg,0x801f04a8u,(uint32_t)spec);
            br2_guest_frame_free();
          }
        }
      }
      else {
        prim_task_run(_DAT_801f04c0 + 0xa0,base + 0x2074,arg,1);
      }
    }
  }
  if (_DAT_801f04c6 == 0) {
    particle_burst_update(base,arg,(uint32_t)br2_gaddr(mat),spec);
  }
  if (0 < _DAT_801f04c6) {
    _DAT_801f04c6 = _DAT_801f04c6 + -1;
  }
  return;

}

/* burst_slot_active @0x80135724 — check whether a particle burst slot
 * index is still live. */
bool burst_slot_active(int slot)
{
  return *(short *)(&DAT_801f04e4 + slot * 2) != 0;

}

/* prim_packet_init_tex @0x801941e0 — init a primitive packet header with
 * a texture binding: tag word (flag2 selects 0xe1000200 vs 0xe1000000),
 * attr flags fl&0x9ff (+0x400 when flag1), and a packed texdesc word at
 * pkt+8 built from the tex record (or 0 when tex == NULL). */
void prim_packet_init_tex(int pkt,int flag1,int flag2,uint32_t fl,uint8_t *tex)
{
  uint32_t pw;
  pw = 0xe1000000;
  *(uint8_t *)guest_at((uint32_t)(uintptr_t)((pkt + 3) ))= 2;
  if (flag2 != 0) {
    pw = 0xe1000200;
  }
  fl = fl & 0x9ff;
  if (flag1 != 0) {
    fl = fl | 0x400;
  }
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pkt + 4) ))= pw | fl;
  if (tex == (uint8_t *)0x0) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pkt + 8) ))= 0;
  }
  else {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pkt + 8) ))=
         (uint32_t)(tex[2] >> 3) << 0xf | (uint32_t)(*tex >> 3) << 10 | 0xe2000000 |
         (-(int)*(short *)(tex + 6) & 0xf8U) << 2 |
         (int)(-(int)*(short *)(tex + 4) & 0xffU) >> 3;
  }
  return;

}


/* gte_xform_accum @0x80194840 — accumulate a vec3 transform: acc +=
 * per-element GTE transform. tr is a scalar operand loaded into cop2
 * data reg 0x4000 (not a matrix); vec components go to 0x4800/0x5000/
 * 0x5800 and cop2 op 0x198003d produces the s16 results. */
void gte_xform_accum(short *acc,uint32_t *vec,int n,uint32_t tr)
{
  uint32_t old;
  uint32_t *end;
  uint32_t rx;
  uint32_t ry;
  uint32_t rz;
  end = vec + n * 2;
  setCopReg(2,0x4000,tr);
  do {
    setCopReg(2,0x4800,*vec & 0xffff);
    setCopReg(2,0x5000,(int)*vec >> 0x10);
    setCopReg(2,0x5800,(int)(short)vec[1]);
    copFunction(2,0x198003d);
    old = *(uint32_t *)acc;
    vec = vec + 2;
    rx = getCopReg(2,0x4800);
    ry = getCopReg(2,0x5000);
    rz = getCopReg(2,0x5800);
    *acc = (short)old + (short)rx;
    acc[1] = (short)((uint32_t)old >> 0x10) + (short)ry;
    acc[2] = acc[2] + (short)rz;
    acc = acc + 4;
  } while (vec != end);
  return;

}

/* gte_rtps_xform3 @0x80194960 family — GTE RTPS vec3 transforms:
 * load matrix/angles, transform, write s16 outputs; _t is the RT+TR
 * (with-translation) variant appending out[5..7]. */
uint32_t * gte_rtps_xform3(uint32_t *rt,uint16_t *in,uint32_t *out)
{
  uint32_t t0;
  int t1;
  uint32_t t2;
  int t3;
  uint32_t t4;
  int t5;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,(uint32_t)*in | *(uint32_t *)(in + 2) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(in + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t2 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)in[1] | *(int *)(in + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)in[7]);
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)in[2] | *(uint32_t *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(in + 8));
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

/* gte_xform3_nold @0x80194b60 — see gte_rtps_xform3. */
uint32_t * gte_xform3_nold(uint16_t *in,uint32_t *out)
{
  uint32_t t0;
  int t1;
  uint32_t t2;
  int t3;
  uint32_t t4;
  int t5;
  setCopReg(2,0,(uint32_t)*in | *(uint32_t *)(in + 2) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(in + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t2 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)in[1] | *(int *)(in + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)in[7]);
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)in[2] | *(uint32_t *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(in + 8));
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

/* gte_mvmva_add @0x80194db0 — GTE MVMVA: matrix*vector + translation add. */
int * gte_mvmva_add(uint32_t *rt,uint32_t *vec,int *out)
{
  uint32_t vx;
  int t3;
  uint32_t vy;
  int t4;
  uint32_t vz;
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

/* gte_rtps_xform1 @0x801951b0 — see gte_rtps_xform3 (single-output variant). */
uint16_t * gte_rtps_xform1(uint32_t *rt,uint32_t *in,uint16_t *out)
{
  uint32_t cop2_idx0 = 0;
  uint32_t cop2_idx1 = 0x800;
  uint32_t gx;
  uint32_t gy;
  uint32_t gz;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,cop2_idx0,*in);
  setCopReg(2,cop2_idx1,in[1]);
  copFunction(2,0x486012);
  gx = getCopReg(2,0x4800);
  gy = getCopReg(2,0x5000);
  gz = getCopReg(2,0x5800);
  *out = (short)gx;
  out[1] = (short)gy;
  out[2] = (short)gz;
  return out;

}

/* gte_rot_load @0x80195210/light_load/tr_load/cw_d000_set — load the
 * GTE rotation matrix / light vector / translation record / control
 * word d000 into cop2 registers. */
void gte_rot_load(uint32_t *m)
{
  setCopControlWord(2,0,*m);
  setCopControlWord(2,0x800,m[1]);
  setCopControlWord(2,0x1000,m[2]);
  setCopControlWord(2,0x1800,m[3]);
  setCopControlWord(2,0x2000,m[4]);
  return;

}

/* gte_light_load @0x80195240 — load the 5-word light/color matrix into
 * GTE ctrl regs 0x8000-0xa000 (light-matrix counterpart of gte_rot_load). */
void gte_light_load(uint32_t *light)
{
  setCopControlWord(2,0x8000,*light);
  setCopControlWord(2,0x8800,light[1]);
  setCopControlWord(2,0x9000,light[2]);
  setCopControlWord(2,0x9800,light[3]);
  setCopControlWord(2,0xa000,light[4]);
  return;

}

/* gte_tr_load @0x80195270 — see gte_rot_load. */
void gte_tr_load(int trec)
{
  setCopControlWord(2,0x2800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((trec + 0x14))));
  setCopControlWord(2,0x3000,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((trec + 0x18))));
  setCopControlWord(2,0x3800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((trec + 0x1c))));
  return;

}

/* gte_cw_d000_set @0x801952b0 — see gte_rot_load. */
void gte_cw_d000_set(uint32_t cw)
{
  setCopControlWord(2,0xd000,cw);
  return;

}

/* gte_op_a00428 @0x80195398 — GTE SQR (square vector): load vec3 from
 * in into cop2 data regs, run op 0xa00428, write MAC1-3 results to out. */
uint32_t * gte_op_a00428(uint32_t *in,uint32_t *out)
{
  uint32_t res;
  uint32_t r0 = 0;
  uint32_t r1 = 0x800;
  uint32_t r2 = 0x1000;
  setCopReg(2,r0,*in);
  setCopReg(2,r1,in[1]);
  setCopReg(2,r2,in[2]);
  copFunction(2,0xa00428);
  res = getCopReg(2,0x19);
  *out = res;
  res = getCopReg(2,0x1a);
  out[1] = res;
  res = getCopReg(2,0x1b);
  out[2] = res;
  return out;

}

/* orbit_fx_spawn @0x801385dc — spawn an orbiting FX record bound to
 * variable var. */
void orbit_fx_spawn(int var)
{
  int tmp3;
  int tmp;
  int tmp2;
  int *cur;
  uint16_t *dp;
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
      tmp2 = (int)(intptr_t)prim_group_build((uint32_t)br2_gaddr(cur),*(uint32_t *)
                                   (&DAT_801c43a8 + ((uint8_t)(&DAT_801b1d70)[tmp] + 0x1e) * 4),0,1);
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
    *(uint16_t *)(pay + 0xd) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((var + -0x7fe4e27c)));
    *(uint16_t *)((intptr_t)pay + 0x36) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((var + -0x7fe4e27a)));
    *(uint16_t *)(pay + 0xe) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((var + -0x7fe4e278)));
  }
  return;

}

/* task_heads_clear @0x801a42d0 — clear the 16 task-pool head pointers
 * for pool (stride 0x48, head array at pool*0x48-0x7fe00c68). */
void task_heads_clear(int pool)
{
  int i;
  i = 0;
  pool = pool * 0x48;
  do {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pool + -0x7fe00c68) ))= 0;
    i = i + 1;
    pool = pool + 4;
  } while (i < 0x10);
  return;

}

/* task_slot_step @0x801a482c — advance/free the current task slot
 * (mark dead, run destructor, recycle). */
void task_slot_step(void)
{
  int pbase;
  int *node;
  int gp_save = (int)guest_gp();
  pbase = *(int *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x1c) ))* 0x48;
  node = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((pbase + -0x7fe00c28))));
  if ((char)node[6] == '\0') {
    *(uint8_t *)(node + 6) = 1;
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

/* task_slot_fns_run @0x801a4aac — invoke the fn callbacks registered
 * on a task slot. */
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
        uint32_t fn_g = (uint32_t)*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + ctx)));
        void *fn = fn_lookup(fn_g);
        uint32_t nodeg_ = (uint32_t)*(int *)guest_at((uint32_t)(uintptr_t)((argblk[0] + i * 4)));
        if (fn != (void *)0x0) {
          /* Guest jalr passes slot = &ctx_base cell (sp+0x10) and
           * a1 = ctx_base[i] node-head guest addr.  The clean slot fns take
           * host pointers, so resolve a1 through guest_at. */
          ((void (*)(uintptr_t,uintptr_t))fn)((uintptr_t)argblk, (uintptr_t)guest_at(nodeg_));
        } else {
          /* Unported slot fn: reproduce the guest jalr — a0 = guest addr of
           * the {row_base, i} cell (sp+0x10 in guest), a1 = node-head guest
           * addr, a2/a3 = live caller regs. The guest cell lived on the
           * stack; synthesize it in the dead gp small-data cells +0x28/+0x2c
           * (0x801bef04/08 — no generated or game code references gp+40/44). */
          CPUState *cpu = clean_cpu_state();
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)(gp_save + 0x28)) = (uint32_t)argblk[0];
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)(gp_save + 0x2c)) = (uint32_t)i;
          br2_call_clean(fn_g, (uint32_t)(uintptr_t)(gp_save + 0x28), nodeg_,
              cpu != NULL ? cpu->gpr[6] : 0u, cpu != NULL ? cpu->gpr[7] : 0u);
        }
      }
    }
    i = i + 1;
  } while (i < 0x10);
  return;

}

/* task_pool_init @0x801a4e2c/alloc/free — task pool management:
 * init a pool region, allocate/free nodes by pool+kind+size. */
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
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((hdr[1] + 0xc) ))= 0;
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

/* task_pool_alloc @0x801a4ebc — see task_pool_init. */
int task_pool_alloc(int pool,int kind,int size)
{
  short flag;
  int *prev;
  uint32_t need;
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
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((blk + 0x18) ))= (*(int *)guest_at((uint32_t)(uintptr_t)((cur + 8) ))+ -0x10) - need;
    *(int *)guest_at((uint32_t)(uintptr_t)((blk + 0x10) ))= cur;
    /* free-list link fields hold GUEST addresses: resolve before deref,
     * store via guest_of */
    prev = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((cur + 4))));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((blk + 0x14) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((cur + 4)));
    *prev = nb;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((blk + 0x1c) ))= 0;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((cur + 8) ))= need;
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

/* task_pool_free @0x801a4fd0 — see task_pool_init. */
void task_pool_free(int pool)
{
  short kind;
  int *bh;
  int next;
  int phdr;
  bh = (int *)guest_at((uint32_t)(uintptr_t)((pool + -0x10)));
  kind = *(short *)guest_at((uint32_t)(uintptr_t)((pool + -4)));
  next = *(int *)guest_at((uint32_t)(uintptr_t)((pool + -0xc)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((pool + -4) ))= 0;
  phdr = *(short *)guest_at((uint32_t)(uintptr_t)((pool + -2) ))* 0x3c + -0x7fe00da4 + kind * 0xc;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((next + 0xc) ))== 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)((pool + -8) ))= *(int *)guest_at((uint32_t)(uintptr_t)((pool + -8) ))+ 0x10 + *(int *)guest_at((uint32_t)(uintptr_t)((next + 8)));
    if (next == *(int *)guest_at((uint32_t)(uintptr_t)((phdr + 4)))) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((phdr + 4) ))= (uint32_t)guest_of(bh);
    }
    /* free-list links are GUEST addresses: resolve inner ptr, store guest addr */
    *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) ))+ 4) )))= (uint32_t)guest_of(bh);
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) ))+ 4)));
  }
  next = *bh;
  if (*(short *)guest_at((uint32_t)(uintptr_t)((next + 0xc) ))== 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)((next + 8) ))= *(int *)guest_at((uint32_t)(uintptr_t)((next + 8) ))+ 0x10 + *(int *)guest_at((uint32_t)(uintptr_t)((pool + -8)));
    if ((int)guest_of(bh) == *(int *)guest_at((uint32_t)(uintptr_t)((phdr + 4)))) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((phdr + 4) ))= (uint32_t)*bh;
    }
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*bh + 4) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pool + -0xc)));
    *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((pool + -0xc) )))= (uint32_t)*bh;
  }
  return;

}

/* gp_slot48_set @0x801a6bb8 / gp_slot24_set @0x801a6bc4 — stash a
 * value into the guest gp frame slots +0x48/+0x24 (callee-saved
 * scratch for nested calls). */
void gp_slot48_set(uint32_t v)
{
  int gp_save = (int)guest_gp();
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x48) ))= v;
  return;

}

/* gp_slot24_set @0x801a6bc4 — see gp_slot48_set. */
void gp_slot24_set(uint32_t v)
{
  int gp_save = (int)guest_gp();
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x24) ))= v;
  return;

}

/* prim_batch_emit @0x801a7570 — emit a batch of primitives: walk the
 * descriptor streams (d0-d3/s0-s6), emit OT entries via otc cursor,
 * tag with sel/fl/kind. */
int prim_batch_emit(int desc,int idx,uint32_t w0,uint32_t w1,int *otc, uint32_t arg6,uint32_t arg7,int num,int sel,uint32_t fl, uint32_t kind)
{
  uint16_t t1;
  uint8_t cnt;
  int node;
  int t0;
  uint32_t t2;
  uint8_t *b0;
  uint8_t *b1;
  uint32_t *s0;
  uint16_t *s1;
  uint32_t *s2;
  uint32_t *d1;
  int wpos;
  uint16_t *s3;
  uint16_t *s4;
  uint32_t *d2;
  uint32_t *d0;
  uint16_t *s5;
  uint32_t *d3;
  uint16_t *s6;
  uint32_t *rec;
  /* PTR_PTR_801be894 / PTR_DAT_801be84c are guest pointer-tables: entry
   * [sel] is a guest sub-table address, then [idx] indexes it.
   * Read the 32-bit guest entries via guest_at (never host ** deref). */
  uint32_t ptrsub = *(uint32_t *)guest_at(0x801be894u + (uint32_t)sel * 4);
  uint32_t datsub = *(uint32_t *)guest_at(0x801be84cu + (uint32_t)sel * 4);
  node = (int)(intptr_t)task_node_alloc(kind,2,0xd,*(uint32_t *)guest_at(ptrsub + (uint32_t)idx * 4),0,
                       (uint32_t)*(uint8_t *)guest_at(datsub + (uint32_t)idx) * *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8) ))* 2
                       + *(int *)guest_at((uint32_t)(uintptr_t)((desc + 0xc) ))* 0x10 + 0x38);
  if (node == 0) {
    return 0;
  }
  rec = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
  d0 = rec + 0xe;
  d1 = d0 + *(int *)guest_at((uint32_t)(uintptr_t)((desc + 0xc) ))* 4;
  *rec = (uint32_t)br2_gaddr(d1);
  cnt = *(uint8_t *)guest_at(datsub + (uint32_t)idx);
  t0 = *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)));
  rec[2] = (uint32_t)br2_gaddr(d0);
  rec[1] = (uint32_t)(br2_gaddr(d1) + (uint32_t)cnt * t0);
  t0 = *(int *)guest_at((uint32_t)(uintptr_t)((desc + 0xc)));
  rec[4] = w0;
  rec[7] = w1;
  rec[3] = (uint32_t)br2_gaddr(d0 + t0 * 2);
  rec[6] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x18)));
  t1 = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((desc + 8)));
  rec[10] = node;
  rec[0xb] = 0;
  *(uint16_t *)(rec + 0xc) = t1;
  rec[8] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x1c)));
  rec[9] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20)));
  t2 = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24)));
  *(uint16_t *)((intptr_t)rec + 0x32) = 0;
  *(uint16_t *)(rec + 0xd) = 1;
  rec[5] = t2;
  t0 = 0;
  if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 0xc) ))<< 1) {
    do {
      *d0 = 0x1000000;
      d0[1] = (fl & 0x4f) << 5 | 0xe1000000;
      t0 = t0 + 1;
      d0 = d0 + 2;
    } while (t0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 0xc) ))<< 1);
  }
  switch(idx + sel * 0x100) {
  case 0:
    t0 = 0;
    d1 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec));
    d0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]));
    s0 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
    d3 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
    s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
    d2 = d3 + num;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
      b1 = (uint8_t *)((intptr_t)d0 + 7);
      b0 = (uint8_t *)((intptr_t)d1 + 7);
      do {
        if (rec[7] == 0) {
          *(uint32_t *)(b0 + -3) = *s0;
          *(uint32_t *)(b1 + -3) = *s0;
          *b0 = *b0 | 1;
          cnt = *b1 | 1;
        }
        else {
          *b0 = *b0 & 0xfe;
          cnt = *b1 & 0xfe;
        }
        *b1 = cnt;
        if (num != 0) {
          *d3 = (uint32_t)br2_gaddr(d1 + 1);
          d3 = d3 + 1;
          *d2 = (uint32_t)br2_gaddr(d0 + 1);
          d2 = d2 + 1;
        }
        *d1 = 0x4000000;
        *b0 = 0x20;
        *d0 = 0x4000000;
        *b1 = 0x20;
        if ((*s4 & 0x100) == 0) {
          *b0 = *b0 & 0xfd;
          cnt = *b1 & 0xfd;
        }
        else {
          *b0 = *b0 | 2;
          cnt = *b1 | 2;
        }
        *b1 = cnt;
        s4 = s4 + 1;
        s0 = s0 + 1;
        b0 = b0 + 0x14;
        d1 = d1 + 5;
        b1 = b1 + 0x14;
        t0 = t0 + 1;
        d0 = d0 + 5;
      } while (t0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
    }
    wpos = *otc;
    t0 = *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8) ))<< 2;
    break;
  case 1:
    t0 = 0;
    d1 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec));
    d0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]));
    s0 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
    d3 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
    s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
    d2 = d3 + num;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
      b1 = (uint8_t *)((intptr_t)d0 + 7);
      b0 = (uint8_t *)((intptr_t)d1 + 7);
      do {
        if (rec[7] == 0) {
          *(uint32_t *)(b0 + -3) = *s0;
          *(uint32_t *)(b1 + -3) = *s0;
          *b0 = *b0 | 1;
          cnt = *b1 | 1;
        }
        else {
          *b0 = *b0 & 0xfe;
          cnt = *b1 & 0xfe;
        }
        *b1 = cnt;
        if (num != 0) {
          *d3 = (uint32_t)br2_gaddr(d1 + 1);
          d3 = d3 + 1;
          *d2 = (uint32_t)br2_gaddr(d0 + 1);
          d2 = d2 + 1;
        }
        *d1 = 0x5000000;
        *b0 = 0x28;
        *d0 = 0x5000000;
        *b1 = 0x28;
        if ((*s4 & 0x100) == 0) {
          *b0 = *b0 & 0xfd;
          cnt = *b1 & 0xfd;
        }
        else {
          *b0 = *b0 | 2;
          cnt = *b1 | 2;
        }
        *b1 = cnt;
        s4 = s4 + 1;
        s0 = s0 + 1;
        b0 = b0 + 0x18;
        d1 = d1 + 6;
        b1 = b1 + 0x18;
        t0 = t0 + 1;
        d0 = d0 + 6;
      } while (t0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
    }
    wpos = *otc;
    t0 = *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8) ))<< 2;
    break;
  case 2:
    t0 = 0;
    d3 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec));
    s0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]));
    d2 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
    d0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
    s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
    d1 = d0 + num;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
      b1 = (uint8_t *)((intptr_t)s0 + 7);
      b0 = (uint8_t *)((intptr_t)d3 + 7);
      s2 = d2 + 2;
      do {
        if (rec[7] == 0) {
          *(uint32_t *)(b0 + -3) = *d2;
          *(uint32_t *)(b1 + -3) = *d2;
          *(uint32_t *)(b0 + 5) = s2[-1];
          *(uint32_t *)(b1 + 5) = s2[-1];
          *(uint32_t *)(b0 + 0xd) = *s2;
          *(uint32_t *)(b1 + 0xd) = *s2;
          *b0 = *b0 | 1;
          cnt = *b1 | 1;
        }
        else {
          *b0 = *b0 & 0xfe;
          cnt = *b1 & 0xfe;
        }
        *b1 = cnt;
        if (num != 0) {
          *d0 = (uint32_t)br2_gaddr(d3 + 1);
          d0[1] = (uint32_t)(br2_gaddr(d3) + 3);
          d0[2] = (uint32_t)(br2_gaddr(d3) + 5);
          d0 = d0 + 3;
          *d1 = (uint32_t)br2_gaddr(s0 + 1);
          d1[1] = (uint32_t)(br2_gaddr(s0) + 3);
          d1[2] = (uint32_t)(br2_gaddr(s0) + 5);
          d1 = d1 + 3;
        }
        *d3 = 0x6000000;
        *b0 = 0x30;
        *s0 = 0x6000000;
        *b1 = 0x30;
        if ((*s4 & 0x100) == 0) {
          *b0 = *b0 & 0xfd;
          cnt = *b1 & 0xfd;
        }
        else {
          *b0 = *b0 | 2;
          cnt = *b1 | 2;
        }
        *b1 = cnt;
        s4 = s4 + 1;
        s2 = s2 + 3;
        d2 = d2 + 3;
        b0 = b0 + 0x1c;
        d3 = d3 + 7;
        b1 = b1 + 0x1c;
        t0 = t0 + 1;
        s0 = s0 + 7;
      } while (t0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
    }
    goto LAB_801a83ec;
  case 3:
    t0 = 0;
    d3 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec));
    s0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]));
    d2 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
    d1 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
    s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
    d0 = d1 + num;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
      b1 = (uint8_t *)((intptr_t)s0 + 7);
      b0 = (uint8_t *)((intptr_t)d3 + 7);
      s2 = d2 + 3;
      do {
        if (rec[7] == 0) {
          *(uint32_t *)(b0 + -3) = *d2;
          *(uint32_t *)(b1 + -3) = *d2;
          *(uint32_t *)(b0 + 5) = s2[-2];
          *(uint32_t *)(b1 + 5) = s2[-2];
          *(uint32_t *)(b0 + 0xd) = s2[-1];
          *(uint32_t *)(b1 + 0xd) = s2[-1];
          *(uint32_t *)(b0 + 0x15) = *s2;
          *(uint32_t *)(b1 + 0x15) = *s2;
          *b0 = *b0 | 1;
          cnt = *b1 | 1;
        }
        else {
          *b0 = *b0 & 0xfe;
          cnt = *b1 & 0xfe;
        }
        *b1 = cnt;
        if (num != 0) {
          *d1 = (uint32_t)br2_gaddr(d3 + 1);
          d1[1] = (uint32_t)(br2_gaddr(d3) + 3);
          d1[2] = (uint32_t)(br2_gaddr(d3) + 5);
          d1[3] = (uint32_t)(br2_gaddr(d3) + 7);
          d1 = d1 + 4;
          *d0 = (uint32_t)br2_gaddr(s0 + 1);
          d0[1] = (uint32_t)(br2_gaddr(s0) + 3);
          d0[2] = (uint32_t)(br2_gaddr(s0) + 5);
          d0[3] = (uint32_t)(br2_gaddr(s0) + 7);
          d0 = d0 + 4;
        }
        *d3 = 0x8000000;
        *b0 = 0x38;
        *s0 = 0x8000000;
        *b1 = 0x38;
        if ((*s4 & 0x100) == 0) {
          *b0 = *b0 & 0xfd;
          cnt = *b1 & 0xfd;
        }
        else {
          *b0 = *b0 | 2;
          cnt = *b1 | 2;
        }
        *b1 = cnt;
        s4 = s4 + 1;
        s2 = s2 + 4;
        d2 = d2 + 4;
        b0 = b0 + 0x24;
        d3 = d3 + 9;
        b1 = b1 + 0x24;
        t0 = t0 + 1;
        s0 = s0 + 9;
      } while (t0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
    }
    goto LAB_801a85d4;
  case 4:
    t0 = 0;
    d1 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec));
    d0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]));
    s0 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
    s3 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x30))));
    s6 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x28))));
    s5 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x2c))));
    d3 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
    s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
    d2 = d3 + num;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
      b1 = (uint8_t *)((intptr_t)d0 + 7);
      b0 = (uint8_t *)((intptr_t)d1 + 7);
      s1 = s3 + 2;
      do {
        *(uint16_t *)(b0 + 7) = *s6;
        *(uint16_t *)(b1 + 7) = *s6;
        *(uint16_t *)(b0 + 0xf) = *s5;
        *(uint16_t *)(b1 + 0xf) = *s5;
        *(uint16_t *)(b0 + 5) = *s3;
        *(uint16_t *)(b1 + 5) = *s3;
        *(uint16_t *)(b0 + 0xd) = s1[-1];
        *(uint16_t *)(b1 + 0xd) = s1[-1];
        *(uint16_t *)(b0 + 0x15) = *s1;
        *(uint16_t *)(b1 + 0x15) = *s1;
        if (rec[7] == 0) {
          *(uint32_t *)(b0 + -3) = *s0;
          *(uint32_t *)(b1 + -3) = *s0;
          *b0 = *b0 | 1;
          cnt = *b1 | 1;
        }
        else {
          *b0 = *b0 & 0xfe;
          cnt = *b1 & 0xfe;
        }
        *b1 = cnt;
        if (num != 0) {
          *d3 = (uint32_t)br2_gaddr(d1 + 1);
          d3 = d3 + 1;
          *d2 = (uint32_t)br2_gaddr(d0 + 1);
          d2 = d2 + 1;
        }
        *d1 = 0x7000000;
        *b0 = 0x24;
        *d0 = 0x7000000;
        *b1 = 0x24;
        if ((*s4 & 0x100) == 0) {
          *b0 = *b0 & 0xfd;
          cnt = *b1 & 0xfd;
        }
        else {
          *b0 = *b0 | 2;
          cnt = *b1 | 2;
        }
        *b1 = cnt;
        s4 = s4 + 1;
        s0 = s0 + 1;
        s1 = s1 + 3;
        s3 = s3 + 3;
        s6 = s6 + 1;
        s5 = s5 + 1;
        b0 = b0 + 0x20;
        d1 = d1 + 8;
        b1 = b1 + 0x20;
        t0 = t0 + 1;
        d0 = d0 + 8;
      } while (t0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
    }
    wpos = *otc;
    t0 = *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8) ))<< 2;
    break;
  case 5:
    t0 = 0;
    d1 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec));
    d0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]));
    s0 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
    s3 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x30))));
    s6 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x28))));
    s5 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x2c))));
    d3 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
    s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
    d2 = d3 + num;
    if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
      b1 = (uint8_t *)((intptr_t)d0 + 7);
      b0 = (uint8_t *)((intptr_t)d1 + 7);
      s1 = s3 + 3;
      do {
        *(uint16_t *)(b0 + 7) = *s6;
        *(uint16_t *)(b1 + 7) = *s6;
        *(uint16_t *)(b0 + 0xf) = *s5;
        *(uint16_t *)(b1 + 0xf) = *s5;
        *(uint16_t *)(b0 + 5) = *s3;
        *(uint16_t *)(b1 + 5) = *s3;
        *(uint16_t *)(b0 + 0xd) = s1[-2];
        *(uint16_t *)(b1 + 0xd) = s1[-2];
        *(uint16_t *)(b0 + 0x15) = s1[-1];
        *(uint16_t *)(b1 + 0x15) = s1[-1];
        *(uint16_t *)(b0 + 0x1d) = *s1;
        *(uint16_t *)(b1 + 0x1d) = *s1;
        if (rec[7] == 0) {
          *(uint32_t *)(b0 + -3) = *s0;
          *(uint32_t *)(b1 + -3) = *s0;
          *b0 = *b0 | 1;
          cnt = *b1 | 1;
        }
        else {
          *b0 = *b0 & 0xfe;
          cnt = *b1 & 0xfe;
        }
        *b1 = cnt;
        if (num != 0) {
          *d3 = (uint32_t)br2_gaddr(d1 + 1);
          d3 = d3 + 1;
          *d2 = (uint32_t)br2_gaddr(d0 + 1);
          d2 = d2 + 1;
        }
        *d1 = 0x9000000;
        *b0 = 0x2c;
        *d0 = 0x9000000;
        *b1 = 0x2c;
        if ((*s4 & 0x100) == 0) {
          *b0 = *b0 & 0xfd;
          cnt = *b1 & 0xfd;
        }
        else {
          *b0 = *b0 | 2;
          cnt = *b1 | 2;
        }
        *b1 = cnt;
        s4 = s4 + 1;
        s0 = s0 + 1;
        s1 = s1 + 4;
        s3 = s3 + 4;
        s6 = s6 + 1;
        s5 = s5 + 1;
        b0 = b0 + 0x28;
        d1 = d1 + 10;
        b1 = b1 + 0x28;
        t0 = t0 + 1;
        d0 = d0 + 10;
      } while (t0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
    }
    wpos = *otc;
    t0 = *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8) ))<< 2;
    break;
  case 6:
    t0 = 0;
    do {
      d0 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
      s3 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x30))));
      s6 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x28))));
      s5 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x2c))));
      s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
      if (t0 == 0) {
        s0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
      }
      else {
        s0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)((*otc + num * 4)));
      }
      wpos = 0;
      if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
        b0 = (uint8_t *)((intptr_t)d1 + 7);
        s1 = s3 + 2;
        d3 = d0 + 2;
        do {
          *(uint16_t *)(b0 + 7) = *s6;
          *(uint16_t *)(b0 + 0x13) = *s5;
          *(uint16_t *)(b0 + 5) = *s3;
          *(uint16_t *)(b0 + 0x11) = s1[-1];
          *(uint16_t *)(b0 + 0x1d) = *s1;
          if (rec[7] == 0) {
            *(uint32_t *)(b0 + -3) = *d0;
            *(uint32_t *)(b0 + 9) = d3[-1];
            *(uint32_t *)(b0 + 0x15) = *d3;
            cnt = *b0 | 1;
          }
          else {
            cnt = *b0 & 0xfe;
          }
          *b0 = cnt;
          if (num != 0) {
            *s0 = (uint32_t)br2_gaddr(d1 + 1);
            s0[1] = (uint32_t)(br2_gaddr(d1) + 4);
            s0[2] = (uint32_t)(br2_gaddr(d1) + 7);
            s0 = s0 + 3;
          }
          *d1 = 0x9000000;
          *b0 = 0x34;
          cnt = 0x36;
          if ((*s4 & 0x100) == 0) {
            cnt = 0x34;
          }
          *b0 = cnt;
          s4 = s4 + 1;
          d3 = d3 + 3;
          d0 = d0 + 3;
          s1 = s1 + 3;
          s3 = s3 + 3;
          s6 = s6 + 1;
          s5 = s5 + 1;
          b0 = b0 + 0x28;
          wpos = wpos + 1;
          d1 = d1 + 10;
        } while (wpos < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
      }
      t0 = t0 + 1;
    } while (t0 < 2);
LAB_801a83ec:
    *otc = *otc + *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8) ))* 0xc;
    return node;
  case 7:
    t0 = 0;
    do {
      d0 = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x24))));
      s3 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x30))));
      s6 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x28))));
      s5 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x2c))));
      s4 = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((desc + 0x20))));
      if (t0 == 0) {
        s0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)(*otc));
      }
      else {
        s0 = (uint32_t *)guest_at((uint32_t)(uintptr_t)((*otc + num * 4)));
      }
      wpos = 0;
      if (0 < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8)))) {
        b0 = (uint8_t *)((intptr_t)d1 + 7);
        s1 = s3 + 3;
        d3 = d0 + 3;
        do {
          *(uint16_t *)(b0 + 7) = *s6;
          *(uint16_t *)(b0 + 0x13) = *s5;
          *(uint16_t *)(b0 + 5) = *s3;
          *(uint16_t *)(b0 + 0x11) = s1[-2];
          *(uint16_t *)(b0 + 0x1d) = s1[-1];
          *(uint16_t *)(b0 + 0x29) = *s1;
          if (rec[7] == 0) {
            *(uint32_t *)(b0 + -3) = *d0;
            *(uint32_t *)(b0 + 9) = d3[-2];
            *(uint32_t *)(b0 + 0x15) = d3[-1];
            *(uint32_t *)(b0 + 0x21) = *d3;
            cnt = *b0 | 1;
          }
          else {
            cnt = *b0 & 0xfe;
          }
          *b0 = cnt;
          if (num != 0) {
            *s0 = (uint32_t)br2_gaddr(d1 + 1);
            s0[1] = (uint32_t)(br2_gaddr(d1) + 4);
            s0[2] = (uint32_t)(br2_gaddr(d1) + 7);
            s0[3] = (uint32_t)(br2_gaddr(d1) + 10);
            s0 = s0 + 4;
          }
          *d1 = 0xc000000;
          *b0 = 0x3c;
          cnt = 0x3e;
          if ((*s4 & 0x100) == 0) {
            cnt = 0x3c;
          }
          *b0 = cnt;
          s4 = s4 + 1;
          d3 = d3 + 4;
          d0 = d0 + 4;
          s1 = s1 + 4;
          s3 = s3 + 4;
          s6 = s6 + 1;
          s5 = s5 + 1;
          b0 = b0 + 0x34;
          wpos = wpos + 1;
          d1 = d1 + 0xd;
        } while (wpos < *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8))));
      }
      t0 = t0 + 1;
    } while (t0 < 2);
LAB_801a85d4:
    wpos = *otc;
    t0 = *(int *)guest_at((uint32_t)(uintptr_t)((desc + 8) ))<< 4;
    break;
  default:
    goto switchD_801a776c_default;
  }
  *otc = wpos + t0;
switchD_801a776c_default:
  return node;

}

/* obj_flag34_clear @0x801a74f0 / obj_flag34_set @0x801a7498 —
 * clear/set the u16 field at +0x34 on each child element of an
 * object record. */
void obj_flag34_clear(int obj)
{
  int i;
  if ((obj != 0) && (i = 0, 0 < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))))) {
    do {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x10)))) ))+ 0x10) ))+ 0x34) ))= 0;
      i = i + 1;
    } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))));
  }
  return;

}

/* tween_seq_alloc @0x801a9644/track_alloc — allocate a tween
 * sequence/track node: pack mode/count/args into pay and bind the
 * keyframe descriptor. */
int tween_seq_alloc(int mode,uint16_t a1,uint32_t a2,uint32_t a3,short arg5, uint16_t arg6,uint32_t arg7,uint32_t kind)
{
  int node;
  uint16_t *pay;
  node = (int)(intptr_t)task_node_alloc(kind,2,3,0x801aa4fc,0,0x4c);
  if (node == 0) {
    node = 0;
  }
  else {
    pay = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = (short)mode;
    pay[1] = a1;
    pay[2] = 1;
    pay[3] = arg5 + -1;
    *(uint32_t *)(pay + 0xc) = a3;
    pay[4] = arg6;
    if (mode == 0) {
      *(uint32_t *)(pay + 0xe) = a2;
    }
    else {
      *(uint32_t *)(pay + 0x10) = a2;
    }
    pay[5] = 0xffff;
    *(uint32_t *)(pay + 10) = 0;
    pay[8] = 0;
    pay[7] = 0;
    pay[0x1c] = 0;
    pay[0x1d] = 0xffff;
    pay[0x22] = 0;
    *(uint32_t *)(pay + 0x1a) = arg7;
  }
  return node;

}

/* gte_matrix_base_set @0x801aad04 — set the GTE matrix base pointer
 * (DAT_801be838) and aux gp slot. */
void gte_matrix_base_set(uint32_t mat,uint32_t aux)
{
  int gp_save = (int)guest_gp();
  DAT_801be838 = mat;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp_save + 0x34) ))= aux;
  return;

}

/* lazy_node_wrap @0x801ac070 — attach a lazy node via node_lazy_attach
 * (proto selects the node class) and fill its pay fields: arg @+0x20,
 * 0 @+0x24, aux @+0x28. */
int lazy_node_wrap(uint32_t proto,uint32_t arg,uint16_t aux)
{
  int node;
  int pay;
  node = (int)(uintptr_t)node_lazy_attach(proto,0x801aae08,0,0x2c);
  if (node == 0) {
    node = 0;
  }
  else {
    pay = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pay + 0x20) ))= arg;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pay + 0x24) ))= 0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((pay + 0x28) ))= aux;
  }
  return node;

}

/* prim_group_alloc @0x801ac334 — allocate a primitive group node
 * with attr/num/mode sizing. */
int prim_group_alloc(uint32_t a0,int attr,int num,int mode,int flg,int div, uint32_t kind)
{
  uint16_t ff;
  int node;
  int off;
  uint32_t *src;
  uint32_t *dst;
  uint32_t i;
  int grp;
  uint32_t w1;
  int m;
  int pad;
  m = mode;
  if ((mode == 0) && (flg != 0)) {
    m = 1;
  }
  pad = (uint32_t)(div != 0x1000) * 0x20;
  node = (int)(intptr_t)task_node_alloc(kind,2,1,0x801ac154,0,
                       num * 4 + m * 0x18 + 0x4c + *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x2c) ))* 8 +
                       *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x34) ))* 8 + *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x30) ))* 8 + pad);
  if (node == 0) {
    grp = 0;
  }
  else {
    grp = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10)));
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x14) ))= node;
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x40) ))= num;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x18) ))= 0;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x28) ))= 0;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= 0;
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x44) ))= mode;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x48) ))= 0;
    if ((div != 0x1000) == 0) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))= 0;
    }
    else {
      /* guest @0x801AC41C-0x801AC430: num=0x01000000, divisor div;
       * break @0x801AC42C is a no-op -> dv==0 yields LO=-1 (num>=0),
       * stored truncated to u16. C /0 would SIGFPE — guard faithfully. */
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))= grp + 0x4c;
      ff = (uint16_t)(div == 0 ? (int32_t)0xffffffff
                                      : 0x1000000 / div);
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x4c) ))= ff;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x54) ))= ff;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x10) ))= ff;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 2) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 4) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 6) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 10) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0xc) ))= 0;
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0xe) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x14) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x18) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 4) ))+ 0x1c) ))= 0;
    }
    pad = grp + 0x4c + pad;
    *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x10) ))= pad;
    node = 0;
    if (0 < num) {
      off = 0;
      do {
        node = node + 1;
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x10)))) ))= 0;
        off = node * 4;
      } while (node < num);
    }
    pad = pad + num * 4;
    if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x2c) ))== 0) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x28) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x90)));
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x28) ))= pad;
      i = 0;
      node = 0;
      if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x2c) ))!= 0) {
        node = 0;
        do {
          dst = (uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x28))))));
          src = (uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x90))))));
          w1 = src[1];
          *dst = *src;
          dst[1] = w1;
          i = i + 1;
          node = i * 8;
        } while (i < *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x2c))));
        node = *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x2c)));
      }
      pad = pad + node * 8;
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x34) ))== 0) {
      if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x10) ))== 1) {
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x94)));
      }
      else {
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= 0;
      }
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c) ))= pad;
      i = 0;
      node = 0;
      if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x34) ))!= 0) {
        node = 0;
        do {
          dst = (uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x2c))))));
          src = (uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x94))))));
          w1 = src[1];
          *dst = *src;
          dst[1] = w1;
          i = i + 1;
          node = i * 8;
        } while (i < *(uint32_t *)guest_at((uint32_t)(uintptr_t)((attr + 0x34))));
        node = *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x34)));
      }
      pad = pad + node * 8;
    }
    if (*(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x30) ))== 0) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x30) ))= 0;
    }
    else {
      *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x30) ))= pad;
      pad = pad + *(int *)guest_at((uint32_t)(uintptr_t)((attr + 0x30) ))* 8;
    }
    node = m * 4;
    if (m == 0) {
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x20) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x24) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x34) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x38) ))= 0;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c) ))= 0;
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
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c)))) ))= 0;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x20)))) ))= 0;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))) ))= 0;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x34)))) ))= 0;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x38)))) ))= 0;
          pad = pad + 1;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c)))) ))= 0;
        } while (pad < m);
      }
    }
  }
  return grp;

}

/* node_param_quad_set @0x801ad260 — write a 4-word param quad
 * into a node record. */
void node_param_quad_set(int node,uint32_t q0,uint32_t q1,uint32_t q2, uint32_t q3)
{
  int datap;
  datap = *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x10) ))+ 8)));
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 4) ))= q0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 8) ))= q1;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 0xc) ))= q2;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((datap + 0x10) ))= q3;
  return;

}

/* prim_list_gte_invoke @0x801871bc/persp_step — walk a primitive
 * list applying the GTE basis transform (gte_xform3_ip/t_ip + MVMVA)
 * per element, then call_guest4 the record's emit callbacks. */
int * prim_list_gte_invoke(int *lst,uint32_t a1,uint32_t a2,int total,uint32_t mtx_t ,uint32_t mtx_ip)
{
  uint32_t cop2_idx0 = 0;
  uint32_t at_out = 0x800;
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  uint32_t s0_in = br2_gpr(16);
  uint32_t s1_in = br2_gpr(17);
  uint32_t s2_in = br2_gpr(18);
  int *rp;
  uint32_t s3_in = br2_gpr(19);
  uint32_t s4_in = br2_gpr(20);
  uint32_t s5_in = br2_gpr(21);
  uint32_t s6_in = br2_gpr(22);
  uint32_t ra_in = br2_gpr(31);
  /* Two distinct guest counters the decompiler merged into num:
   * num_out = $s3 ← $a3=total @0x801870E4, decremented @0x80187180 and
   * tested @0x80187184 (outer list loop); num_in = $s6, loaded from
   * *lst[7] @0x80187130, decremented @0x80187174, tested @0x80187178
   * (inner record loop). Sharing one host local made the outer loop
   * exit after a single record pass. */
  int num_out = total;
  int num_in;
  if (0 < total) {
    setCopControlWord(2,0xe800,0x555);
    setCopControlWord(2,0xf000,0x400);
    _DAT_1f8003e0 = s6_in;
    _DAT_1f8003e4 = s5_in;
    _DAT_1f8003e8 = s4_in;
    _DAT_1f8003ec = s3_in;
    _DAT_1f8003f0 = s2_in;
    _DAT_1f8003f4 = s1_in;
    _DAT_1f8003f8 = s0_in;
    _DAT_1f8003fc = ra_in;
    do {
      if (((short)lst[3] == 0) && (*lst != 0)) {
        gte_xform3_ip((uint32_t *)guest_at((uint32_t)mtx_ip),(uint16_t *)guest_at((uint32_t)(*lst + 0x20)));
        gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)mtx_t),(uint16_t *)guest_at((uint32_t)(*lst + 0x20)));
        setCopReg(2,cop2_idx0,lst[1]);
        setCopReg(2,at_out,lst[2]);
        copFunction(2,0x480012);
        num_in = *(int *)guest_at((uint32_t)(uintptr_t)(lst[7]));
        t0 = getCopReg(2,0xc800);
        rp = (int *)guest_at((uint32_t)(uintptr_t)(lst[7] ))+ 1;
        if (num_in != 0) {
          t1 = getCopReg(2,0xd000);
          t2 = getCopReg(2,0xd800);
          setCopControlWord(2,0x2800,t0);
          setCopControlWord(2,0x3000,t1);
          setCopControlWord(2,0x3800,t2);
          /* Generated 0x801871BC jalr's each record fn with the full lst-num
           * set: lst = record[2] (data cursor), a1 = a1 + lh(lst
           * +0xE), a2 = a2, num = record[1] (per-record field).
           * task_invoke/call_guest only program $lst/$a1 — $a1 arrived as 0
           * and $a2/$num stale, so the GTE emit leaf 0x801876BC used a1=0 as
           * its OT/link-table base and SWL'd into BIOS kernel RAM
           * (phys 0x0000-0x1FFC). call_guest4 reproduces the guest ABI
           * exactly (same fix as prim_task_run). */
          {
            uint32_t prim_a1 = (uint32_t)a1
                + (uint32_t)(int)*(int16_t *)((char *)lst + 0xe);
            do {
              num_in = num_in + -1;
              call_guest4((uint32_t)rp[3],(uint32_t)rp[2],prim_a1,
                          (uint32_t)a2,(uint32_t)rp[1]);
              rp = rp + 4;
            } while (0 < num_in);
          }
        }
      }
      num_out = num_out + -1;
      lst = lst + 10;
    } while (0 < num_out);
  }
  return lst;

}

/* text_render_full @0x8012d154 — full text renderer: walk the string,
 * fetch per-glyph font metrics, and emit textured quads into the
 * packet/OT stream; font selects the glyph table, cfg the attrs. */
uint32_t text_render_full(uint32_t *ot,uint32_t font,int cfg)
{
  uint8_t wd;
  uint16_t aw;
  char tint;
  char ac;
  char dw;
  short by;
  short sx;
  char tc;
  short rx;
  char gy;
  uint16_t xc0;
  uint32_t ch;
  short *gp;
  char gx;
  uint8_t t0;
  short sy;
  uint16_t gw;
  short lh;
  uint32_t *pkt;
  uint16_t xc1;
  int t2;
  uint8_t *tp;
  uint8_t *nx;
  uint16_t xc2;
  dw = DAT_1f800015;
  font = font & 0xffff;
  if (_DAT_1f800388 != font) {
    _DAT_1f800380 = 0;
    _DAT_1f800388 = font;
  }
  if (_DAT_1f800380 < 400) {
    pkt = (uint32_t *)guest_at((uint32_t)(uintptr_t)((font * 16000 + _DAT_1f800380 * 0x28 + -0x7fe17ad8)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cfg + 6) ))< 0x1000) {
      _DAT_1f80037c = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 6)));
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cfg + 8) ))< 0x1000) {
      _DAT_1f80037e = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 8)));
    }
    gw = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 10)));
    aw = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x10)));
    if (((*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x18) ))< 6) && (3 < *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x18))))) ||
       (DAT_1f800015 == '\0')) {
      lh = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0xc)));
    }
    else {
      lh = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0xc) ))<< 1;
    }
    /* +0x1c cell stores a GUEST text pointer — resolve via guest_at before
     * dereferencing (raw guest addr faults as a host pointer). */
    tp = (uint8_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x1c))));
    if (*tp != 0) {
      t2 = (int)*(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0xe)));
      ac = (char)*(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0xe)));
      xc1 = (uint16_t)DAT_8011ede0;
      xc2 = (uint16_t)DAT_8011ef64;
      gp = (short *)((intptr_t)pkt + 0x22);
      xc0 = (uint16_t)DAT_8011ed80;
      do {
        gp[-10] = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x16)));
        ch = (uint32_t)*tp;
        nx = tp + 1;
        sy = _DAT_1f80037e + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 4)));
        sx = _DAT_1f80037c + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 2)));
        if (ch != 0x20) {
          if (ch < 0x21) {
            if (ch == 10) {
LAB_8012d3f4:
              _DAT_1f80037c = 0;
              _DAT_1f80037e = _DAT_1f80037e + lh + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x14)));
              goto LAB_8012dca4;
            }
          }
          else if (ch == 0x23) {
            t0 = *nx;
            ch = (uint32_t)t0;
            nx = tp + 2;
            if (ch != 0x23) {
              if (ch != 0x6e) {
                *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x16) ))=
                     (*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x16) ))& 0xfc3f) + (t0 - 0x30 & 0x1ff) * 0x40;
                goto LAB_8012dca4;
              }
              goto LAB_8012d3f4;
            }
          }
          tint = (char)aw;
          switch(*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x18)))) {
          default:
            gp[-6] = 0x2f;
            ch = ch - 0x20;
            gx = ((uint8_t)ch & 0xf) * '\b';
            gy = (char)((ch & 0xf0) >> 1);
            if (t2 < 0) {
              t0 = (&DAT_8011ed80)[ch & 0xff];
              gw = (uint16_t)t0;
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)((intptr_t)gp + 3) = gy + tint;
              gx = t0 + gx;
              *(char *)(gp + -7) = gx;
              *(char *)(gp + 1) = gx;
            }
            else {
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)(gp + -7) = gx + ac;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)(gp + 1) = gx + ac;
              *(char *)((intptr_t)gp + 3) = gy + tint;
            }
            gp[-9] = sx + gw;
            gp[-0xd] = sx;
            gp[-0xc] = sy;
            gp[-8] = sy;
            gp[-5] = sx;
            by = sy;
            if (dw != '\0') {
              by = sy + -1;
            }
            gp[-4] = by + lh;
            gp[-1] = sx + gw;
            if (dw == '\0') {
              by = sy + lh;
            }
            else {
              by = sy + -1 + lh;
            }
            break;
          case 1:
            gp[-6] = 0x2f;
            t0 = (uint8_t)(ch - 0x20);
            gx = (t0 & 0xf) * '\b';
            gy = (t0 & 0xf0) + 0x30;
            if (t2 < 0) {
              t0 = (&DAT_8011ede0)[ch - 0x20 & 0xff];
              gw = (uint16_t)t0;
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)((intptr_t)gp + 3) = gy + tint;
              gx = t0 + gx;
              *(char *)(gp + -7) = gx;
              *(char *)(gp + 1) = gx;
            }
            else {
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)(gp + -7) = gx + ac;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)(gp + 1) = gx + ac;
              *(char *)((intptr_t)gp + 3) = gy + tint;
            }
            gp[-9] = sx + gw;
            gp[-0xd] = sx;
            gp[-0xc] = sy;
            gp[-8] = sy;
            gp[-5] = sx;
            by = sy;
            if (dw != '\0') {
              by = sy + -1;
            }
            gp[-4] = by + lh;
            gp[-1] = sx + gw;
            if (dw == '\0') {
              by = sy + lh;
            }
            else {
              by = sy + -1 + lh;
            }
            break;
          case 2:
            gp[-6] = 0x2f;
            switch(ch) {
            case 0x21:
              ch = 0x1b;
              break;
            case 0x22:
              ch = 0x24;
              break;
            default:
              if ((ch - 0x30 & 0xff) < 10) {
                ch = ch - 10;
              }
              else if (ch < 0x61) {
                ch = ch - 0x41;
              }
              else {
                ch = ch - 0x61;
              }
              break;
            case 0x26:
              ch = 0x1d;
              break;
            case 0x27:
              t0 = 0xc0;
              gx = '`';
              ch = 0x30;
              goto LAB_8012d724;
            case 0x2a:
              ch = 0x1f;
              break;
            case 0x2d:
              ch = 0x1e;
              break;
            case 0x2e:
              ch = 0x1a;
              break;
            case 0x3c:
              ch = 0x20;
              break;
            case 0x3e:
              ch = 0x22;
              break;
            case 0x3f:
              ch = 0x1c;
              break;
            case 0x5f:
              ch = 0x23;
              break;
            case 0x7e:
              ch = 0x25;
              break;
            case 0x7f:
              ch = 0x21;
            }
            t0 = (uint8_t)((ch & 7) << 4) | 0x80;
            gx = (char)((ch & 0xff) >> 3) * '\x18';
LAB_8012d724:
            gy = t0 + ac + -1;
            if (t2 < 0) {
              wd = (&DAT_8011ee40)[ch & 0xff];
              gw = (uint16_t)wd;
              *(uint8_t *)(gp + -0xb) = t0;
              *(char *)((intptr_t)gp + -0x15) = gx;
              *(char *)((intptr_t)gp + -0xd) = gx;
              *(uint8_t *)(gp + -3) = t0;
              *(char *)((intptr_t)gp + -5) = gx + tint;
              *(char *)((intptr_t)gp + 3) = gx + tint;
              gp[-0xd] = sx;
              gp[-0xc] = sy;
              gp[-8] = sy;
              gp[-5] = sx;
              *(uint8_t *)(gp + -7) = wd + t0;
              *(uint8_t *)(gp + 1) = wd + t0;
              gp[-9] = sx + (uint16_t)wd;
              by = sy;
              if (dw != '\0') {
                by = sy + -1;
              }
              gp[-4] = by + lh;
              gp[-1] = sx + (uint16_t)wd;
              if (dw == '\0') {
                by = sy + lh;
              }
              else {
                by = sy + -1 + lh;
              }
            }
            else {
              *(uint8_t *)(gp + -0xb) = t0;
              *(char *)((intptr_t)gp + -0x15) = gx;
              *(char *)(gp + -7) = gy;
              *(char *)((intptr_t)gp + -0xd) = gx;
              *(uint8_t *)(gp + -3) = t0;
              *(char *)((intptr_t)gp + -5) = gx + tint;
              *(char *)(gp + 1) = gy;
              *(char *)((intptr_t)gp + 3) = gx + tint;
              gp[-0xd] = sx;
              gp[-0xc] = sy;
              by = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 10)));
              gp[-8] = sy;
              gp[-5] = sx;
              gp[-9] = by + sx + -1;
              by = sy;
              if (dw != '\0') {
                by = sy + -1;
              }
              gp[-4] = by + lh;
              gp[-1] = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 10) ))+ sx + -1;
              if (dw == '\0') {
                by = sy + lh;
              }
              else {
                by = sy + -1 + lh;
              }
            }
            break;
          case 3:
            gp[-6] = 0x2e;
            switch(ch) {
            case 0x21:
              ch = 0x1b;
              break;
            default:
              if ((ch - 0x30 & 0xff) < 10) {
                ch = ch - 10;
              }
              else if (ch < 0x61) {
                ch = ch - 0x41;
              }
              else {
                ch = ch - 0x61;
              }
              break;
            case 0x26:
              ch = 0x1d;
              break;
            case 0x2a:
              ch = 0x1f;
              break;
            case 0x2d:
              ch = 0x1e;
              break;
            case 0x2e:
              ch = 0x1a;
              break;
            case 0x3c:
              ch = 0x20;
              break;
            case 0x3e:
              ch = 0x22;
              break;
            case 0x3f:
              ch = 0x1c;
              break;
            case 0x5f:
              ch = 0x23;
              break;
            case 0x7e:
              ch = 0x25;
              break;
            case 0x7f:
              ch = 0x21;
            }
            ch = ch & 0xff;
            gx = (&DAT_8011eea4)[ch];
            gy = (&DAT_8011eed4)[ch];
            if (t2 < 0) {
              t0 = (&DAT_8011ee74)[ch];
              gw = (uint16_t)t0;
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)((intptr_t)gp + 3) = gy + tint;
              gp[-0xd] = sx;
              gp[-0xc] = sy;
              gp[-8] = sy;
              gp[-5] = sx;
              *(uint8_t *)(gp + -7) = t0 + gx;
              *(uint8_t *)(gp + 1) = t0 + gx;
              gp[-9] = sx + (uint16_t)t0;
              by = sy;
              if (dw != '\0') {
                by = sy + -1;
              }
              gp[-4] = by + lh;
              gp[-1] = sx + (uint16_t)t0;
              if (dw == '\0') {
                by = sy + lh;
              }
              else {
                by = sy + -1 + lh;
              }
            }
            else {
              tc = gx + ac + -1;
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)(gp + -7) = tc;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)(gp + 1) = tc;
              *(char *)((intptr_t)gp + 3) = gy + tint;
              gp[-0xd] = sx;
              gp[-0xc] = sy;
              by = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 10)));
              gp[-8] = sy;
              gp[-5] = sx;
              gp[-9] = by + sx + -1;
              by = sy;
              if (dw != '\0') {
                by = sy + -1;
              }
              gp[-4] = by + lh;
              gp[-1] = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 10) ))+ sx + -1;
              if (dw == '\0') {
                by = sy + lh;
              }
              else {
                by = sy + -1 + lh;
              }
            }
            break;
          case 4:
            if (0x5f < ch) {
              gw = 0xf;
              gp[-6] = 0x5e;
              tint = (char)ch * '\x10';
              t0 = (char)ch + 0xa0U & 0xf0;
              gy = t0 - 0x28;
              gx = t0 - 0x18;
              *(char *)(gp + -0xb) = tint;
              *(char *)(gp + -3) = tint;
              rx = sx + 0x10;
              *(char *)((intptr_t)gp + -5) = gx;
              *(char *)((intptr_t)gp + 3) = gx;
              by = sy + 0x10;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)(gp + -7) = tint + '\x10';
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + 1) = tint + '\x10';
              goto LAB_8012dc2c;
            }
            gp[-6] = 0x5d;
            ch = ch - 0x20;
            gx = (uint8_t)ch * '\x10';
            gy = ((uint8_t)ch & 0xf0) - 0x40;
            if (t2 < 0) {
              t0 = (&DAT_8011ef64)[ch & 0xff];
              gw = (uint16_t)t0;
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)((intptr_t)gp + 3) = gy + tint;
              gx = t0 + gx;
              *(char *)(gp + -7) = gx;
              *(char *)(gp + 1) = gx;
            }
            else {
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)(gp + -7) = gx + ac;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)(gp + 1) = gx + ac;
              *(char *)((intptr_t)gp + 3) = gy + tint;
            }
            if ((t2 < 0) || ((ch & 0xff) != 0x11)) goto LAB_8012dc28;
            ch = (uint32_t)DAT_8011ef75;
            gp[-0xc] = sy;
            gp[-8] = sy;
            gp[-4] = sy + lh;
            *gp = sy + lh;
            sx = sx + (short)((int)(t2 - ch) / 2);
            gp[-0xd] = sx;
            gp[-9] = gw + sx;
            gp[-5] = sx;
            gp[-1] = gw + sx;
            goto LAB_8012dc4c;
          case 5:
            gp[-6] = 0x3e;
            ch = ch - 0x20;
            gx = (char)ch * '\b';
            gy = (char)((ch & 0xe0) >> 2) + -0x40;
            if (t2 < 0) {
              t0 = (&DAT_8011ef04)[ch & 0xff];
              gw = (uint16_t)t0;
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)((intptr_t)gp + 3) = gy + tint;
              gx = t0 + gx;
              *(char *)(gp + -7) = gx;
              *(char *)(gp + 1) = gx;
            }
            else {
              *(char *)(gp + -0xb) = gx;
              *(char *)((intptr_t)gp + -0x15) = gy;
              *(char *)(gp + -7) = gx + ac;
              *(char *)((intptr_t)gp + -0xd) = gy;
              *(char *)(gp + -3) = gx;
              *(char *)((intptr_t)gp + -5) = gy + tint;
              *(char *)(gp + 1) = gx + ac;
              *(char *)((intptr_t)gp + 3) = gy + tint;
            }
LAB_8012dc28:
            rx = sx + gw;
            by = sy + lh;
LAB_8012dc2c:
            gp[-0xd] = sx;
            gp[-0xc] = sy;
            gp[-9] = rx;
            gp[-8] = sy;
            gp[-5] = sx;
            gp[-4] = by;
            gp[-1] = rx;
          }
          *gp = by;
LAB_8012dc4c:
          gp = gp + 0x14;
          *pkt = *ot | 0x9000000;
          *ot = guest_of(pkt) & 0xffffff;
          _DAT_1f800380 = _DAT_1f800380 + 1;
          pkt = pkt + 10;
          if (399 < _DAT_1f800380) {
            return 0xffffffff;
          }
          sx = gw + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
          goto LAB_8012dc9c;
        }
        if (-1 < t2) {
          _DAT_1f80037c = _DAT_1f80037c + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 10) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
          goto LAB_8012dca4;
        }
        switch(*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x18)))) {
        default:
          sx = xc0 + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
          break;
        case 1:
          sx = xc1 + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
          break;
        case 2:
          t0 = DAT_8011ee5b;
          goto LAB_8012d390;
        case 3:
          t0 = DAT_8011ee8f;
LAB_8012d390:
          _DAT_1f80037c = _DAT_1f80037c + (uint16_t)t0 + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
          goto LAB_8012dca4;
        case 4:
          sx = xc2 + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
        }
LAB_8012dc9c:
        _DAT_1f80037c = _DAT_1f80037c + sx;
LAB_8012dca4:
        tp = nx;
      } while (*nx != 0);
    }
  }
  return 0;

}

/* text_render_simple @0x8012dce4/full — text renderer: glyph quad
 * emission with font/cfg selection, spacing, tint and clipping into
 * the OT. */
uint32_t text_render_simple(uint32_t *a0,uint32_t font,int cfg)
{
  uint8_t aux;
  short adv;
  uint16_t tint;
  short y1;
  char mode;
  char col;
  char rx;
  short x1;
  char t3;
  short *gp;
  char t2;
  uint32_t ch;
  short cy;
  uint8_t *tx;
  uint8_t *nx;
  uint16_t gw;
  uint32_t *ot;
  uint16_t tw;
  short lh;
  mode = DAT_1f800015;
  font = font & 0xffff;
  if (_DAT_1f800388 != font) {
    _DAT_1f800380 = 0;
    _DAT_1f800388 = font;
  }
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x18) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x18) ))& 3;
  if (_DAT_1f800380 < 400) {
    ot = (uint32_t *)guest_at((uint32_t)(uintptr_t)((font * 16000 + _DAT_1f800380 * 0x28 + -0x7fe17ad8)));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cfg + 6) ))< 0x1000) {
      _DAT_1f80037c = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 6)));
    }
    if (*(short *)guest_at((uint32_t)(uintptr_t)((cfg + 8) ))< 0x1000) {
      _DAT_1f80037e = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 8)));
    }
    gw = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 10)));
    adv = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0xe)));
    tint = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x10)));
    if (mode == '\0') {
      lh = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0xc)));
    }
    else {
      lh = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0xc) ))<< 1;
    }
    /* The +0x1c cell stores a GUEST pointer (guest_of(text buf)); resolve it
     * through guest_at before dereferencing or the host faults on the raw
     * guest address. */
    tx = (uint8_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x1c))));
    if (*tx != 0) {
      tw = (uint16_t)DAT_8011ed80;
      gp = (short *)((intptr_t)ot + 0x22);
      do {
        gp[-10] = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x16)));
        ch = (uint32_t)*tx;
        nx = tx + 1;
        x1 = _DAT_1f80037e + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 4)));
        cy = _DAT_1f80037c + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 2)));
        if (ch == 0x20) {
          if (adv < 0) {
            x1 = tw + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
          }
          else {
            x1 = *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 10) ))+ *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
          }
          _DAT_1f80037c = _DAT_1f80037c + x1;
        }
        else {
          if (ch < 0x21) {
            if (ch == 10) {
LAB_8012dec4:
              _DAT_1f80037c = 0;
              _DAT_1f80037e = _DAT_1f80037e + lh + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x14)));
              goto LAB_8012e044;
            }
            gp[-6] = 0x2f;
          }
          else if (ch == 0x23) {
            aux = *nx;
            nx = tx + 2;
            if (aux != 0x23) {
              if (aux == 0x6e) goto LAB_8012dec4;
              *(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x16) ))=
                   (*(uint16_t *)guest_at((uint32_t)(uintptr_t)((cfg + 0x16) ))& 0xfc3f) + (aux - 0x30 & 0x1ff) * 0x40;
              goto LAB_8012e044;
            }
            gp[-6] = 0x2f;
            ch = 0x23;
          }
          else {
            gp[-6] = 0x2f;
          }
          ch = ch - 0x20;
          t2 = ((uint8_t)ch & 0xf) * '\b';
          t3 = (char)((ch & 0xf0) >> 1);
          col = (char)tint;
          if (adv < 0) {
            aux = (&DAT_8011ed80)[ch & 0xff];
            gw = (uint16_t)aux;
            col = t3 + col;
            *(char *)(gp + -0xb) = t2;
            *(char *)((intptr_t)gp + -0x15) = t3;
            *(char *)((intptr_t)gp + -0xd) = t3;
            *(char *)(gp + -3) = t2;
            *(char *)((intptr_t)gp + -5) = col;
            *(char *)((intptr_t)gp + 3) = col;
            t2 = aux + t2;
            *(char *)(gp + -7) = t2;
            *(char *)(gp + 1) = t2;
          }
          else {
            rx = (char)adv + t2;
            col = t3 + col;
            *(char *)(gp + -0xb) = t2;
            *(char *)((intptr_t)gp + -0x15) = t3;
            *(char *)(gp + -7) = rx;
            *(char *)((intptr_t)gp + -0xd) = t3;
            *(char *)(gp + -3) = t2;
            *(char *)((intptr_t)gp + -5) = col;
            *(char *)(gp + 1) = rx;
            *(char *)((intptr_t)gp + 3) = col;
          }
          gp[-9] = cy + gw;
          gp[-0xd] = cy;
          gp[-0xc] = x1;
          gp[-8] = x1;
          gp[-5] = cy;
          y1 = x1;
          if (mode != '\0') {
            y1 = x1 + -1;
          }
          gp[-4] = y1 + lh;
          gp[-1] = cy + gw;
          if (mode != '\0') {
            x1 = x1 + -1;
          }
          *gp = x1 + lh;
          gp = gp + 0x14;
          *ot = *a0 | 0x9000000;
          *a0 = guest_of(ot) & 0xffffff;
          _DAT_1f800380 = _DAT_1f800380 + 1;
          ot = ot + 10;
          if (399 < _DAT_1f800380) {
            return 0xffffffff;
          }
          _DAT_1f80037c = _DAT_1f80037c + gw + *(short *)guest_at((uint32_t)(uintptr_t)((cfg + 0x12)));
        }
LAB_8012e044:
        tx = nx;
      } while (*nx != 0);
    }
  }
  return 0;

}

/* particle_burst_spawn @0x80136114/update — spawn and per-tick
 * update a particle burst record (type/idx bound). */
void particle_burst_spawn(int type,int idx)
{
  short tv;
  int r;
  int step;
  int cnt;
  uint16_t *p;
  int rad;
  int i;
  uint16_t *rm;
  rm = (uint16_t *)&DAT_801f0300;
  _DAT_801f04a0 = 9;
  idx = idx * 8;
  if (type == 1) {
    rad = (int)*(short *)(&DAT_801b152a + idx);
    cnt = (int)_DAT_801f04bc;
    step = rad << 1;
    i = 0;
    if (0 < cnt) {
      p = (uint16_t *)&DAT_801f0328;
      do {
        p[3] = 0xfc00;
        p[2] = 0x400;
        r = table_byte_next_a();
        r = r * -0x80;
        if (r < 0) {
          r = r + 0xff;
        }
        p[4] = (short)((uint32_t)r >> 8);
        rotmat_rtps_build(0xfffffc00,rm + 2);
        rad = rad - step / (cnt + 1);
        tv = *(short *)(&DAT_801b1528 + idx);
        *(uint32_t *)(p + -6) = 0xffffff00;
        *(int *)(p + -4) = rad;
        *(int *)(p + -8) = -(int)tv;
        r = table_byte_next_a();
        r = (r + 0xc0) * -0x33;
        if (r < 0) {
          r = r + 0xff;
        }
        p[-2] = (short)((uint32_t)r >> 8);
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
        p = (uint16_t *)&DAT_801f0328;
        do {
          p[3] = 0x400;
          p[2] = 0x400;
          r = table_byte_next_a();
          r = r * -0x80;
          if (r < 0) {
            r = r + 0xff;
          }
          p[4] = (short)((uint32_t)r >> 8);
          rotmat_rtps_build(0x400,rm + 2);
          rad = rad - step / (cnt + 1);
          tv = *(short *)(&DAT_801b1528 + idx);
          *(uint32_t *)(p + -6) = 0xffffff00;
          *(int *)(p + -4) = rad;
          *(int *)(p + -8) = (int)tv;
          r = table_byte_next_a();
          r = (r + 0xc0) * 0x33;
          if (r < 0) {
            r = r + 0xff;
          }
          p[-2] = (short)((uint32_t)r >> 8);
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
      p = (uint16_t *)&DAT_801f0328;
      do {
        p[3] = 0;
        p[2] = 0x400;
        r = table_byte_next_a();
        r = r * -0x80;
        if (r < 0) {
          r = r + 0xff;
        }
        rad = rad - step / (cnt + 1);
        p[4] = (short)((uint32_t)r >> 8);
        *(uint32_t *)(p + -0x12) = 0x1000;
        *(uint32_t *)(p + -0x10) = 0;
        *(uint32_t *)(p + -0xe) = 0x1000;
        *(uint32_t *)(p + -0xc) = 0;
        *(uint32_t *)(p + -10) = 0x1000;
        *(int *)(p + -8) = rad;
        *(uint32_t *)(p + -6) = 0xffffff00;
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
        *p = (short)((uint32_t)r >> 8);
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
      p = (uint16_t *)&DAT_801f0328;
      do {
        p[3] = 0x800;
        p[2] = 0x400;
        r = table_byte_next_a();
        r = r * -0x80;
        if (r < 0) {
          r = r + 0xff;
        }
        p[4] = (short)((uint32_t)r >> 8);
        rotmat_rtps_build(0x800,rm + 2);
        rad = rad - step / (cnt + 1);
        *(int *)(p + -8) = rad;
        *(uint32_t *)(p + -6) = 0xffffff00;
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
        *p = (short)((uint32_t)r >> 8);
        p = p + 0x1a;
        *rm = 0x10;
        rm = rm + 0x1a;
      } while (i < _DAT_801f04bc);
    }
  }
  return;

}

/* particle_burst_update @0x801365a4 — see particle_burst_spawn. */
void particle_burst_update(int ctx,uint32_t arg,uint32_t pos,int rec)
{
  short vx;
  short vy;
  short *bnd;
  int t;
  uint8_t *pr;
  short *slot;
  int k;
  /* Guest GTE output slot: gte_rtps_xform3_t writes pos[0..7] as
   * 8 contiguous u32; the decompiler split them into scalars.
   * xf[0..7] = old xf/local_44/local_40/local_3c/
   * local_38/local_34/local_30/local_2c. */
  uint32_t xf [8];
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
        if (*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x82) ))== 0) {
          *(short *)(pr + 0x22) = *(short *)(pr + 0x22) + 2;
          *(int *)(pr + 0x1c) = *(int *)(pr + 0x1c) + (int)*(short *)(pr + 0x24);
          *(int *)(pr + 0x14) = *(int *)(pr + 0x14) + (int)*(short *)(pr + 0x20);
          bnd = (short *)(uintptr_t)_DAT_801f04a4;
          t = *(int *)(pr + 0x18);
          *(int *)(pr + 0x18) = t + *(short *)(pr + 0x22);
          t = (t + *(short *)(pr + 0x22)) - (int)bnd[1];
          if (*(char *)guest_at((uint32_t)(uintptr_t)((rec + 0x98) ))== '\x02') {
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
            *(uint16_t *)(pr + 0x28) = 0;
            *(short *)(pr + 0x22) =
                 -(short)((((int)((uint32_t)*(uint16_t *)(pr + 0x22) << 0x10) >> 0x10) -
                          ((int)((uint32_t)*(uint16_t *)(pr + 0x22) << 0x10) >> 0x1f)) >> 1);
            *(short *)(pr + 0x20) =
                 (short)((((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x10) -
                         ((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
            *(short *)(pr + 0x24) =
                 (short)((((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x10) -
                         ((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
            *(short *)(pr + 0x2c) =
                 (short)((((int)((uint32_t)*(uint16_t *)(pr + 0x2c) << 0x10) >> 0x10) -
                         ((int)((uint32_t)*(uint16_t *)(pr + 0x2c) << 0x10) >> 0x1f)) >> 1);
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
                     -(short)((((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x10) -
                              ((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
                *(short *)(pr + 0x24) =
                     (short)((((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x10) -
                             ((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
                *(short *)(pr + 0x2c) = -*(short *)(pr + 0x2c);
              }
              t = (int)_DAT_801f04a4[3];
              if (((t < *(int *)(pr + 0x1c)) && (0 < *(short *)(pr + 0x24))) ||
                 ((t = -t, *(int *)(pr + 0x1c) < t &&
                  (*(short *)(pr + 0x24) < 0)))) {
                deferred_cmd_post(0x400,0x83,0);
                *(int *)(pr + 0x1c) = t;
                *(short *)(pr + 0x20) =
                     -(short)((((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x10) -
                              ((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
                *(short *)(pr + 0x24) =
                     (short)((((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x10) -
                             ((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
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
                   -(short)((((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x10) -
                            ((int)((uint32_t)*(uint16_t *)(pr + 0x20) << 0x10) >> 0x1f)) >> 1);
              *(short *)(pr + 0x24) =
                   -(short)((((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x10) -
                            ((int)((uint32_t)*(uint16_t *)(pr + 0x24) << 0x10) >> 0x1f)) >> 1);
              *(int *)(pr + 0x14) = *(int *)(pr + 0x14) - (int)vx;
              *(int *)(pr + 0x1c) = *(int *)(pr + 0x1c) - (int)vy;
              *(short *)(pr + 0x2c) = -*(short *)(pr + 0x2c);
            }
          }
          angle_pair_rotmat((int)*(short *)(pr + 0x28),(int)*(short *)(pr + 0x2a),pr);
        }
        gte_rtps_xform3_t((uint32_t *)(uintptr_t)pos,(uint16_t *)(uintptr_t)pr,(uint32_t *)xf);
        setCopControlWord(2,0,xf[0]);
        setCopControlWord(2,0x800,xf[1]);
        setCopControlWord(2,0x1000,xf[2]);
        setCopControlWord(2,0x1800,xf[3]);
        setCopControlWord(2,0x2000,xf[4]);
        setCopControlWord(2,0x2800,xf[5]);
        setCopControlWord(2,0x3000,xf[6]);
        setCopControlWord(2,0x3800,xf[7]);
        prim_task_run(_DAT_801f04b8 + k * 0x28,ctx + 0x2068,arg,1);
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

/* radial_glow_draw @0x801380e4 — draw a radial glow sprite quad
 * from the record parameters. */
void radial_glow_draw(int *rec)
{
  uint32_t xy;
  uint32_t cop2_idx0 = 0;
  uint32_t cop2_idx1 = 0x800;
  short ox;
  short cy;
  short oy;
  int rad;
  int cs;
  short cx;
  uint16_t lvl;
  int t;
  int *cur;
  int k;
  short ex;
  short ey;
  short sx;
  short sy;
  uint32_t rnd;
  setCopReg(2,cop2_idx0,rec[0xd]);
  setCopReg(2,cop2_idx1,rec[0xe]);
  copFunction(2,0x180001);
  xy = getCopReg(2,0xe);
  ex = (short)xy;
  ey = (short)((uint32_t)xy >> 0x10);
  angle_vec_build(((int)ex + (int)ey) * 2,&DAT_1f8003e0);
  rad = isqrt(((0xff - ex) * (0xff - ex) +
                       (0xff - ey) * (0xff - ey)) * 0x1000);
  rad = rad >> 0xc;
  k = 0;
  if ((uint16_t)(ex + 0x32U) < 0x265) {
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
      lvl = (uint16_t)rad;
      if (0xfff < k) {
        lvl = 0xfff;
      }
      rad = 1;
      *(uint16_t *)(rec + 5) = lvl;
      *(uint16_t *)(rec + 7) = lvl;
      *(uint16_t *)(rec + 9) = lvl;
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
            gte_rtps_xform3((uint32_t *)(uintptr_t)&DAT_801f0760,(uint16_t *)(uintptr_t)&DAT_1f8003e0,(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(*rec))));
          }
          else if (rad == 1) {
            gte_rtps_xform3((uint32_t *)(rec + 5),(uint16_t *)(uintptr_t)&DAT_801f0760,(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec[1]))));
          }
          else {
            *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(*cur )))= _DAT_801f0760;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 4) ))= _DAT_801f0764;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 8) ))= _DAT_801f0768;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 0xc) ))= _DAT_801f076c;
            *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*cur ))+ 0x10) ))= _DAT_801f0770;
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

/* iris_wipe_spawn @0x8013b5a8/update — iris wipe transition: spawn the
 * wipe record (the per-tick aperture advance lives in iris_wipe_update). */
void iris_wipe_spawn(int req,int arg)
{
  char x1;
  int tmp;
  uint8_t *bp;
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
      *(uint16_t *)(pay + 0xd2) = 0;
      cur = pay + 0x1d;
      if (*(char *)guest_at((uint32_t)(uintptr_t)((req + 3) ))== '\x04') {
        *(uint16_t *)((intptr_t)pay + 0x34a) = 1;
        if (*(char *)guest_at((uint32_t)(uintptr_t)((req + 5) ))== '\x11') {
          *(uint16_t *)(pay + 0xd3) = 1;
        }
        else {
          *(uint16_t *)(pay + 0xd3) = 0;
        }
      }
      else {
        *(uint16_t *)((intptr_t)pay + 0x34a) = 0;
      }
      tmp = 0;
      *(uint16_t *)(pay + 1) = 10;
      *(uint16_t *)((intptr_t)pay + 6) = 0xffe9;
      *(uint16_t *)(pay + 2) = 0xffd8;
      *(uint16_t *)(pay + 3) = 0xfff6;
      *(uint16_t *)((intptr_t)pay + 0xe) = 0xffe9;
      *(uint16_t *)(pay + 4) = 0xffd8;
      do {
        j = 0;
        bp = (uint8_t *)((intptr_t)cur + 7);
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

/* gte_xform3_ip @0x80186e64/t_ip — GTE basis-vector transform: load rt[0..4]
 * into RT ctrl regs, MVMVA-transform the three packed basis vectors in
 * `in`, and write the packed results into GTE ctrl regs 0x4000-0x6000
 * (light-matrix slots). Returns rt; nothing is stored through in. */
uint32_t * gte_xform3_ip(uint32_t *rt,uint16_t *in)
{
  uint32_t t0;
  int t1;
  uint32_t t2;
  uint32_t t3;
  int t4;
  uint32_t t5;
  int t6;
  uint32_t t7;
  int t8;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,CONCAT22(in[3],*in));
  setCopReg(2,0x800,*(uint32_t *)(in + 6));
  copFunction(2,0x486012);
  t3 = getCopReg(2,0x4800);
  t4 = getCopReg(2,0x5000);
  t5 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)(uint16_t)in[1] | *(uint32_t *)(in + 4) << 0x10);
  setCopReg(2,0x800,*(uint32_t *)(in + 6) >> 0x10);
  copFunction(2,0x486012);
  t6 = getCopReg(2,0x4800);
  t7 = getCopReg(2,0x5000);
  t8 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)(uint16_t)in[2] | *(uint32_t *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(in + 8));
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

/* gte_xform3_t_ip @0x80186f5c — see gte_xform3_ip; same three basis
 * transforms but results land in RT ctrl regs 0-4 plus a fourth
 * transformed vector and rt[5..7] translation in TR ctrl regs
 * 0x2800/0x3000/0x3800 — i.e. the full RT+TR matrix build. */
uint32_t * gte_xform3_t_ip(uint32_t *rt,uint16_t *in)
{
  uint32_t t0;
  int t1;
  uint32_t t2;
  int t3;
  int t4;
  int t5;
  int t6;
  uint32_t t7;
  uint32_t t8;
  int t9;
  uint32_t t10;
  int t11;
  setCopControlWord(2,0,*rt);
  setCopControlWord(2,0x800,rt[1]);
  setCopControlWord(2,0x1000,rt[2]);
  setCopControlWord(2,0x1800,rt[3]);
  setCopControlWord(2,0x2000,rt[4]);
  setCopReg(2,0,CONCAT22(in[3],*in));
  setCopReg(2,0x800,*(uint32_t *)(in + 6));
  copFunction(2,0x486012);
  t2 = getCopReg(2,0x4800);
  t6 = getCopReg(2,0x5000);
  t7 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)(uint16_t)in[1] | *(uint32_t *)(in + 4) << 0x10);
  setCopReg(2,0x800,*(uint32_t *)(in + 6) >> 0x10);
  copFunction(2,0x486012);
  t9 = getCopReg(2,0x4800);
  t10 = getCopReg(2,0x5000);
  t11 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)(uint16_t)in[2] | *(uint32_t *)(in + 4) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(in + 8));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t3 = getCopReg(2,0x5000);
  t8 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)(uint16_t)in[10] | *(int *)(in + 0xc) << 0x10);
  setCopReg(2,0x800,*(uint32_t *)(in + 0xe));
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

/* prim_task_run @0x8018a4f8 — run a primitive task node: dispatch the
 * packet emit chain for rec with mode selector. */
int prim_task_run(int rec,uint32_t a1,uint32_t a2,int mode)
{
  uint32_t t1;
  uint32_t t2;
  uint32_t t3;
  uint32_t t4;
  uint32_t t5;
  uint32_t t6;
  uint32_t sv_ra;
  uint32_t sv_s6;
  uint32_t sv_s3;
  uint32_t sv_s2;
  uint32_t sv_s1;
  uint32_t sv_s0;
  uint32_t s0_in = br2_gpr(16);
  uint32_t s1_in = br2_gpr(17);
  uint32_t s2_in = br2_gpr(18);
  int *pi;
  uint32_t s3_in = br2_gpr(19);
  uint32_t s6_in = br2_gpr(22);
  int n;
  uint32_t ra_in = br2_gpr(31);
  sv_ra = ra_in;
  sv_s6 = s6_in;
  sv_s3 = s3_in;
  sv_s2 = s2_in;
  sv_s1 = s1_in;
  sv_s0 = s0_in;
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

/* vec2_rot @0x8018b050 — build s16 rotation-matrix elements from an
 * angle pair: dst[0..2] = sin/cos LUT products (no input vector). */
uint16_t * vec2_rot(uint32_t ang0,uint32_t ang1,uint16_t *dst)
{
  uint16_t c0;
  int ns0;
  int p;
  int c1;
  ns0 = lut_sin_q12(ang0);
  ns0 = -ns0;
  c0 = lut_cos_q12(ang0);
  dst[1] = c0;
  p = lut_sin_q12(ang1);
  p = ns0 * p;
  c1 = lut_cos_q12(ang1);
  *dst = (short)(p >> 0xc);
  dst[2] = (short)(ns0 * c1 >> 0xc);
  return dst;

}

/* angle_gte_vec @0x8018b0a0 — build a packed GTE matrix/vector record
 * in out from the angle pair (x,y are angles, not a vector): seeds
 * 0x1000 constants and fills rotation terms via sin/cos lookups. */
uint32_t * angle_gte_vec(uint32_t x,uint32_t y,uint32_t *out)
{
  int sx;
  int xy;
  int xz;
  uint32_t one;
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

/* prim_list_persp_step @0x8018b208 — see prim_list_gte_invoke. */
int * prim_list_persp_step(int *lst,uint32_t a1,uint32_t a2,int n,uint32_t arg5 ,int xf,uint32_t arg7,uint32_t arg8)
{
  uint32_t cop2_idx0 = 0;
  uint32_t at_out = 0x800;
  int t;
  int vp;
  int sz;
  uint32_t t4_in = br2_gpr(12);
  uint32_t t5_in = br2_gpr(13);
  uint32_t t6_in = br2_gpr(14);
  uint32_t t7_in = br2_gpr(15);
  uint32_t s0_in = br2_gpr(16);
  uint32_t s1_in = br2_gpr(17);
  uint32_t s2_in = br2_gpr(18);
  int *rp;
  uint32_t s3_in = br2_gpr(19);
  uint32_t s4_in = br2_gpr(20);
  uint32_t s5_in = br2_gpr(21);
  uint32_t s6_in = br2_gpr(22);
  uint32_t s7_in = br2_gpr(23);
  uint32_t t8_in = br2_gpr(24);
  uint32_t ra_in = br2_gpr(31);
  if (0 < n) {
    _DAT_1f8003d4 = arg5;
    _DAT_1f8003d0 = arg7;
    _DAT_1f8003cc = arg8;
    _DAT_1f8003d8 = a1;
    _DAT_1f8003dc = s7_in;
    _DAT_1f8003e0 = s6_in;
    _DAT_1f8003e4 = s5_in;
    _DAT_1f8003e8 = s4_in;
    _DAT_1f8003ec = s3_in;
    _DAT_1f8003f0 = s2_in;
    _DAT_1f8003f4 = s1_in;
    _DAT_1f8003f8 = s0_in;
    _DAT_1f8003fc = ra_in;
    do {
      if (((short)lst[3] == 0) && (*lst != 0)) {
        vp = *lst + 0x20;
        gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)xf),(uint16_t *)guest_at((uint32_t)(*lst + 0x20)));
        setCopReg(2,cop2_idx0,lst[1]);
        setCopReg(2,at_out,lst[2]);
        copFunction(2,0x480012);
        t = getCopReg(2,0xc800);
        sz = getCopReg(2,0xd800);
        _DAT_1f8003a0 = t6_in;
        _DAT_1f8003a4 = t4_in;
        _DAT_1f8003a8 = t7_in;
        _DAT_1f8003ac = t8_in;
        _DAT_1f8003b0 = t5_in;
        _DAT_1f8003b8 = range_circle_test(t,sz,(uint32_t *)guest_at((uint32_t)_DAT_1f8003d0),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((vp + 0x18))));
        if (_DAT_1f8003b8 != 0) {
          t = t + ((int)*(short *)guest_at((uint32_t)(uintptr_t)((xf + 2) ))* -_DAT_1f8003b8 >> 0xc);
          sz = sz + ((int)*(short *)guest_at((uint32_t)(uintptr_t)((xf + 0xe) ))* -_DAT_1f8003b8 >> 0xc);
        }
        _DAT_1f8003b4 = t;
        _DAT_1f8003bc = sz;
        gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)_DAT_1f8003d4),(uint16_t *)(uintptr_t)&DAT_1f8003a0);
        t = *(int *)guest_at((uint32_t)(uintptr_t)(lst[7]));
        rp = (int *)guest_at((uint32_t)(uintptr_t)(lst[7] ))+ 1;
        if (t != 0) {
          /* Generated 0x8018B208 jalr's each record fn with the full lst-n
           * set, same {count,[x,a3field,arg0,func]} stride-16 table as
           * prim_task_run: lst = entry[+8] (data cursor), a1 = a1,
           * a2 = a2 (both loaded once per node before the entry loop),
           * n = entry[+4] (per-record field). task_invoke/call_guest only
           * program $lst/$a1, leaving $a2/$n as stale register-file values
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
      n = n + -1;
      lst = lst + 10;
    } while (0 < n);
  }
  return lst;

}

/* gte_light_set @0x8018d2ec — load GTE light direction/color. */
void gte_light_set(uint32_t x,uint32_t y,uint32_t z)
{
  setCopControlWord(2,0x6800,x);
  setCopControlWord(2,0x7000,y);
  setCopControlWord(2,0x7800,z);
  return;

}


/* gte_xform_add_off @0x8018d568 — transform vec and add offset. */
int * gte_xform_add_off(int x,uint32_t *y,int *z)
{
  uint32_t cop2_idx0 = 0;
  uint32_t cop2_idx1 = 0x800;
  int oy;
  int oz;
  int tx;
  int ty;
  int tz;
  setCopReg(2,cop2_idx0,*y);
  setCopReg(2,cop2_idx1,y[1]);
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

/* angle_pair_rotmat @0x8018dac0/angle_vec_build — build rotation
 * artifacts from packed euler angles via the sin LUT. */
uint32_t * angle_pair_rotmat(uint32_t ang0,uint32_t ang1,uint32_t *out)
{
  int t0;
  int t1;
  int t2;
  int t3;
  uint32_t g0;
  uint32_t g1;
  int t4;
  t0 = (ang0 & 0x3ff) * 2;
  t4 = (ang0 & 0x3ff) * -2 + 0x800;
  t3 = t0;
  if ((ang0 & 0x400) != 0) {
    t3 = t4;
    t4 = t0;
  }
  t3 = (int)*(short *)(&DAT_801247a8 + t3);
  if ((ang0 & 0x800) != 0) {
    t3 = -t3;
  }
  t1 = (ang1 & 0x3ff) * 2;
  t2 = (ang1 & 0x3ff) * -2 + 0x800;
  t0 = t1;
  if ((ang1 & 0x400) != 0) {
    t0 = t2;
    t2 = t1;
  }
  t0 = (int)*(short *)(&DAT_801247a8 + t0);
  if ((ang1 & 0x800) != 0) {
    t0 = -t0;
  }
  g1 = (uint32_t)*(short *)(&DAT_801247a8 + t2);
  if ((ang1 + 0x400 & 0x800) != 0) {
    g1 = -g1;
  }
  g0 = (uint32_t)*(short *)(&DAT_801247a8 + t4);
  if ((ang0 + 0x400 & 0x800) != 0) {
    g0 = -g0;
  }
  out[3] = -t0 & 0xffffU | ((int)(g1 * t3) >> 0xc) << 0x10;
  *out = g1 & 0xffff | (t0 * t3 >> 0xc) << 0x10;
  out[1] = (int)(t0 * g0) >> 0xc & 0xffff;
  out[2] = g0 & 0xffff | t3 * -0x10000;
  *(short *)(out + 4) = (short)((int)(g1 * g0) >> 0xc);
  return out;

}

/* vec_emit_spawn @0x80137048 — spawn a vector-emit FX group:
 * fx_group_alloc(1,1,3,3) then store the 3-word payload rec[0..2]
 * via noop_a. */
void vec_emit_spawn(uint32_t *rec)
{
  int grp;
  grp = fx_group_alloc(1,1,3,3);
  if (grp != 0) {
    noop_a(grp,*rec,rec[1],rec[2]);
  }
  return;

}

/* iris_wipe_update @0x8013b12c — see iris_wipe_spawn. */
void iris_wipe_update(int *rec)
{
  short cv;
  uint32_t cop2_idx0 = 0;
  uint32_t cop2_idx1 = 0x800;
  int t;
  int t3;
  uint16_t *out;
  int off;
  int *row;
  int t7;
  uint32_t t8;
  uint32_t oy;
  uint32_t oz;
  short *nxt;
  short *wr;
  uint32_t *ot;
  short *vp;
  int t15;
  short vtx [24];
  uint8_t aux [24];
  uint16_t scr [12];
  if (DAT_801bef20 == 0) {
    t = rec[0xd1];
    rec[0xd1] = t + -1;
    if (t + -1 < 1) {
      task_slot_step();
      return;
    }
    if (*(short *)((intptr_t)rec + 0x34a) != 0) {
      if (*(char *)guest_at((uint32_t)(uintptr_t)((*rec + 5) ))== '\0') {
        *(uint16_t *)(rec + 0xd3) = 0;
        return;
      }
      if ((short)rec[0xd3] == 0) {
        *(uint16_t *)(rec + 0xd3) = 1;
        *(uint16_t *)(rec + 0xd2) = 0;
      }
    }
    t15 = 0;
    t7 = 0x14;
    t = 4;
    *(short *)(rec + 0xd2) = (short)rec[0xd2] + 1;
    row = rec;
    do {
      t3 = *(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20)));
      setCopControlWord(2,0,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0xc0))));
      setCopControlWord(2,0x800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0xc4))));
      setCopControlWord(2,0x1000,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 200))));
      setCopControlWord(2,0x1800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0xcc))));
      setCopControlWord(2,0x2000,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t3 + 0xd0))));
      setCopReg(2,cop2_idx0,*(uint32_t *)((intptr_t)rec + t));
      setCopReg(2,cop2_idx1,((uint32_t *)((intptr_t)rec + t))[1]);
      copFunction(2,0x486012);
      out = (uint16_t *)((intptr_t)rec + t7);
      t8 = getCopReg(2,0x4800);
      oy = getCopReg(2,0x5000);
      oz = getCopReg(2,0x5800);
      *out = (short)t8;
      out[1] = (short)oy;
      out[2] = (short)oz;
      *(short *)(row + 5) = (short)row[5] + *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ 0xd4)));
      t7 = t7 + 0x30;
      *(short *)((intptr_t)row + 0x16) =
           *(short *)((intptr_t)row + 0x16) + *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ 0xd8)));
      t = t + 8;
      t15 = t15 + 1;
      *(short *)(row + 6) = (short)row[6] + *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ 0xdc)));
      row = row + 0xc;
    } while (t15 < 2);
    t7 = 0;
    t = 0;
    do {
      t3 = 5;
      t15 = 0x28;
      do {
        t3 = t3 + -1;
        off = t3 * 8 + t;
        t8 = *(uint32_t *)((intptr_t)rec + off + 0x18);
        *(uint32_t *)((intptr_t)rec + t15 + t + 0x14) =
             *(uint32_t *)((intptr_t)rec + off + 0x14);
        *(uint32_t *)((intptr_t)rec + t15 + t + 0x18) = t8;
        t15 = t3 * 8;
      } while (0 < t3);
      t7 = t7 + 1;
      t = t + 0x30;
    } while (t7 < 2);
  }
  vp = vtx;
  gte_rot_load((uint32_t *)(uintptr_t)&DAT_801be838);
  t7 = 0;
  gte_tr_load(DAT_801be838);
  gte_points_xform(rec + 5,vp,scr,aux,aux,0xc);
  t = _DAT_1f80000c * 0x5a + 0x1d;
  do {
    ot = (uint32_t *)(rec + t);
    t = 0;
    wr = (short *)((intptr_t)ot + 0x22);
    nxt = vp + 3;
    do {
      if ((short)rec[0xd2] <= t) break;
      t8 = angle_from_xy((int)nxt[-1] - (int)*vp,(int)*nxt - (int)nxt[-2]);
      t3 = mips_div_lo((int32_t)((int)((uint32_t)_DAT_1f800024 << 0x10) >> 0xc),(int32_t)(int)((uint32_t)scr[0] << 2)); /* guest div @0x8013B3F0 */
      t15 = lut_cos_q12(t8);
      wr[-0xd] = *vp + (short)(t3 * t15 >> 0xc);
      t15 = lut_cos_q12(t8);
      wr[-9] = *vp - (short)(t3 * t15 >> 0xc);
      t15 = lut_sin_q12(t8);
      wr[-0xc] = nxt[-2] + (short)(t3 * t15 >> 0xc);
      t15 = lut_sin_q12(t8);
      wr[-8] = nxt[-2] - (short)(t3 * t15 >> 0xc);
      t15 = lut_cos_q12(t8);
      wr[-5] = nxt[-1] + (short)(t3 * t15 >> 0xc);
      t15 = lut_cos_q12(t8);
      wr[-1] = nxt[-1] - (short)(t3 * t15 >> 0xc);
      t15 = lut_sin_q12(t8);
      wr[-4] = *nxt + (short)(t3 * t15 >> 0xc);
      t15 = lut_sin_q12(t8);
      t = t + 1;
      vp = vp + 2;
      cv = *nxt;
      nxt = nxt + 2;
      *wr = cv - (short)(t3 * t15 >> 0xc);
      wr = wr + 0x12;
      *ot = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x40) ))| 0x8000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x40) ))= guest_of(ot) & 0xffffff;
      ot = ot + 9;
    } while (t < 5);
    t7 = t7 + 1;
    t = _DAT_1f80000c * 0x5a + 0x4a;
    vp = vtx + 12;
    if (1 < t7) {
      return;
    }
  } while( true );

}

/* script_op_dispatch @0x80184f28 — script opcode dispatch loop:
 * fetch op uint8_t, sprintf debug string, jalr handler until cont==0
 * or iter cap. */
uint32_t script_op_dispatch(uint32_t ctrl,uint32_t pad,int script,uint32_t ctx)
{
  uint8_t op;
  short cont;
  int n;
  int iter;
  iter = 0;
  do {
    op = *(uint8_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((script + 0x24))));
    if (*(short *)guest_at((uint32_t)(uintptr_t)((script + 0x6e) ))!= 0) {
      if ((*(int *)guest_at((uint32_t)(uintptr_t)((script + 0x78) ))+ -0x7c) - script < 0xbc) {
        /* guest: ctrl = *(u32*)(s0+0x78) guest cursor addr; script = guest
         * string-table entry @0x801b5574[op] (guest addr -> host). */
        n = sprintf_core((char *)guest_at((uint32_t)*(int *)guest_at(script + 0x78)),
                             &DAT_801237e0,
                             (char *)guest_at(*(uint32_t *)guest_at(0x801b5574 + (uint32_t)op * 4)),
                             ctx);
        *(int *)guest_at((uint32_t)(uintptr_t)((script + 0x78) ))= *(int *)guest_at((uint32_t)(uintptr_t)((script + 0x78) ))+ n;
      }
    }
    /* PTR_entry_0x80182f3c_801b5528 is the guest opcode-handler jump table
     * @0x801b5528 (19 entries, handlers @0x80182f3c..0x80184f28). It was never
     * populated with host pointers, so indexing it called address 0. Dispatch
     * through the real guest table via br2_call_generated — the same jalr the
     * original performs — returning the handler's $v0 as the s16 result. */
    cont = (short)br2_call_clean(
        *(uint32_t *)guest_at(0x801b5528u + (uint32_t)op * 4u),
        ctrl, pad, (uint32_t)script, ctx);
    iter = iter + 1;
  } while ((cont != 0) && (iter * 0x10000 >> 0x10 < 0x32));
  return 0;

}

/* trig_sin_lookup @0x8018d244 — sine lookup by angle index. */
int trig_sin_lookup(uint32_t ang)
{
  uint32_t idx;
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

/* angle_vec_build @0x8018e028 — see angle_pair_rotmat. */
uint32_t * angle_vec_build(uint32_t ang,uint32_t *out)
{
  int t0;
  int t2;
  uint32_t g0;
  int t1;
  uint32_t g1;
  *(uint16_t *)(out + 4) = 0x1000;
  t0 = (ang & 0x3ff) * 2;
  t1 = (ang & 0x3ff) * -2 + 0x800;
  t2 = t0;
  if ((ang & 0x400) != 0) {
    t2 = t1;
    t1 = t0;
  }
  g0 = (uint32_t)*(uint16_t *)(&DAT_801247a8 + t2);
  if ((ang & 0x800) != 0) {
    g0 = -g0;
  }
  g1 = (uint32_t)*(uint16_t *)(&DAT_801247a8 + t1);
  if ((ang + 0x400 & 0x800) != 0) {
    g1 = -g1 & 0xffff;
  }
  *out = g1 | g0 * -0x10000;
  out[1] = g0 * 0x10000;
  out[2] = g1;
  out[3] = 0;
  return out;

}

/* rand_lcg @0x8018e4d0 — LCG random step. */
uint32_t rand_lcg(void)
{
  _DAT_801f68e0 = _DAT_801f68e0 * 0x41c64e6d + 0x3039;
  return _DAT_801f68e0 >> 0x10 & 0x7fff;

}

/* isqrt @0x801943d4/isqrt_refine — integer square root + refinement. */
int isqrt(int n)
{
  int r;
  uint32_t sh;
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

/* gte_rtps_xform3_t @0x80194c50 — see gte_rtps_xform3 (_t = RT+TR
 * variant: same three basis transforms into out[0..4], plus a fourth
 * transform whose MAC results + rm[5..7] translation append as
 * out[5..7]). */
uint32_t * gte_rtps_xform3_t(uint32_t *rm,uint16_t *ang,uint32_t *out)
{
  uint32_t t0;
  int t1;
  int t2;
  uint32_t t3;
  int t4;
  int t5;
  uint32_t t6;
  int t7;
  setCopControlWord(2,0,*rm);
  setCopControlWord(2,0x800,rm[1]);
  setCopControlWord(2,0x1000,rm[2]);
  setCopControlWord(2,0x1800,rm[3]);
  setCopControlWord(2,0x2000,rm[4]);
  setCopReg(2,0,(uint32_t)*ang | *(uint32_t *)(ang + 2) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(ang + 6));
  copFunction(2,0x486012);
  t0 = getCopReg(2,0x4800);
  t1 = getCopReg(2,0x5000);
  t3 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)ang[1] | *(int *)(ang + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)ang[7]);
  copFunction(2,0x486012);
  t5 = getCopReg(2,0x4800);
  t6 = getCopReg(2,0x5000);
  t7 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)ang[2] | *(uint32_t *)(ang + 4) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(ang + 8));
  copFunction(2,0x486012);
  *out = t5 << 0x10 | t0 & 0xffff;
  out[3] = t7 << 0x10 | t3 & 0xffff;
  t3 = getCopReg(2,0x4800);
  t5 = getCopReg(2,0x5000);
  t0 = getCopReg(2,0xb);
  out[4] = t0;
  setCopReg(2,0,(uint32_t)ang[10] | *(int *)(ang + 0xc) << 0x10);
  setCopReg(2,0x800,*(uint32_t *)(ang + 0xe));
  copFunction(2,0x486012);
  out[1] = t3 & 0xffff | t1 << 0x10;
  out[2] = t5 << 0x10 | t6 & 0xffff;
  t5 = getCopReg(2,0xc800);
  t1 = getCopReg(2,0xd000);
  t7 = getCopReg(2,0xd800);
  t2 = rm[6];
  t4 = rm[7];
  out[5] = t5 + rm[5];
  out[6] = t1 + t2;
  out[7] = t7 + t4;
  return out;

}

/* gte_xform1_ir @0x80194f10/raw — single-row GTE transform variants. */
uint32_t * gte_xform1_ir(uint32_t *mtx,uint32_t *out)
{
  uint32_t v;
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

/* gte_rtps_xform1_raw @0x80195160 — see gte_xform1_ir. */
uint32_t * gte_rtps_xform1_raw(uint32_t *mtx,uint32_t *in,uint32_t *out)
{
  uint32_t v;
  uint32_t cop2_idx0 = 0;
  uint32_t cop2_idx1 = 0x800;
  setCopControlWord(2,0,*mtx);
  setCopControlWord(2,0x800,mtx[1]);
  setCopControlWord(2,0x1000,mtx[2]);
  setCopControlWord(2,0x1800,mtx[3]);
  setCopControlWord(2,0x2000,mtx[4]);
  setCopReg(2,cop2_idx0,*in);
  setCopReg(2,cop2_idx1,in[1]);
  copFunction(2,0x486012);
  v = getCopReg(2,0x19);
  *out = v;
  v = getCopReg(2,0x1a);
  out[1] = v;
  v = getCopReg(2,0x1b);
  out[2] = v;
  return out;

}

/* node_lazy_attach @0x801a4414 — attach a node of class cls from
 * pool tables; returns node or 0. */
int * node_lazy_attach(int n,int en,int aux,int par)
{
  char cls;
  uint8_t sh;
  int *nd;
  int *slot;
  cls = *(char *)guest_at((uint32_t)(uintptr_t)((n + 0x14)));
  if ((en == 0) ||
     (nd = (int *)guest_at((uint32_t)(uintptr_t)(task_pool_alloc((int)cls,(int)*(char *)guest_at((uint32_t)(uintptr_t)((n + 0x15))),par + 0x1c))),
     nd == (int *)0x0)) {
    nd = (int *)0x0;
  }
  else {
    slot = (int *)guest_at((uint32_t)(uintptr_t)((cls * 0x48 + -0x7fe00c68 + *(char *)guest_at((uint32_t)(uintptr_t)((n + 0x16) ))* 4)));
    if (*slot == n) {
      *slot = (int)guest_of(nd);
    }
    else {
      /* field at n+4 holds a GUEST addr; resolve before deref */
      *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(n + 4)))= guest_of(nd);
    }
    *nd = n;
    nd[1] = *(int *)guest_at((uint32_t)(uintptr_t)((n + 4)));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((n + 4) ))= guest_of(nd);
    nd[2] = en;
    nd[3] = aux;
    *(uint8_t *)(nd + 5) = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((n + 0x14)));
    *(uint8_t *)((intptr_t)nd + 0x15) = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((n + 0x15)));
    sh = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((n + 0x16)));
    *(uint8_t *)((intptr_t)nd + 0x17) = 1;
    nd[4] = (int)guest_of((char *)(nd + 7));
    *(uint8_t *)(nd + 6) = 0;
    *(uint8_t *)((intptr_t)nd + 0x16) = sh;
  }
  return (int *)(uintptr_t)guest_of(nd);

}

/* tween_ensure @0x801a6f44 — ensure a channel has a live tween track;
 * allocate via tween_track_alloc if missing. */
uint32_t tween_ensure(int chan)
{
  uint32_t ret;
  int sub;
  ret = 0;
  if ((chan != 0) && (ret = 0, *(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))== 0)) {
    sub = *(int *)guest_at((uint32_t)(uintptr_t)((chan + 8)));
    ret = 1;
    if (*(int *)guest_at((uint32_t)(uintptr_t)((sub + 0x14) ))== 1) {
      ret = tween_track_alloc(2,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((sub + 0x14))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x30))),
                           (uint16_t *)(uintptr_t)*(uint32_t *)guest_at((uint32_t)(uintptr_t)((sub + 0x8c))),*(uint32_t *)guest_at((uint32_t)(uintptr_t)((sub + 0x24))),
                           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((chan + 8) ))+ 0x30))),
                           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((chan + 8) ))+ 0x38))),
                           (int)*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((chan + 0x14) ))+ 0x14))));
      *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))= ret;
      if (*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))== 0) {
        ret = 0;
      }
      else {
        *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x3c) )))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((chan + 0x24) )))+ 0x10)));
        ret = 1;
      }
    }
  }
  return ret;

}

/* obj_flag34_set @0x801a7498 — see obj_flag34_clear. */
void obj_flag34_set(int obj)
{
  int i;
  if ((obj != 0) && (i = 0, 0 < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))))) {
    do {
      *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((i * 4 + *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x10)))) ))+ 0x10) ))+ 0x34) ))= 1;
      i = i + 1;
    } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((obj + 0x40))));
  }
  return;

}

/* tween_seek @0x801a99e0 — seek a tween sequence to position pos. */
void tween_seek(int seq,int pos)
{
  uint32_t ok;
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
            ok = (uint32_t)((_quo < p) ^ 1);
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

/* tween_seq_step @0x801aa4fc — advance a tween sequence one step. */
void tween_seq_step(short *seq)
{
  short t;
  uint8_t *ip;
  int off;
  uint32_t op;
  uint8_t arg;
  int va;
  int gp_in = (int)guest_gp();
  if ((((*(int *)guest_at((uint32_t)(uintptr_t)((gp_in + 0x44) ))!= 1) && (seq != (short *)0x0)) && (-1 < seq[0x22])) &&
     (seq[0x22] < 2)) {
    if (seq[5] < 0) {
      t = seq[0x1d] + 1;
LAB_801aa558:
      seq[0x1d] = t;
LAB_801aa55c:
      do {
        ip = (uint8_t *)guest_at((uint32_t)(uintptr_t)((seq[0x1d] * 2 + *(int *)guest_at((uint32_t)(uintptr_t)((seq[0x1c] * 4 + *(int *)(seq + 0x1a))))
                         )));
        arg = *ip;
        op = (int)*(short *)ip & 0xff00;
        if (op == 0x2000) {
          seq[0x22] = seq[0x22] | 0x100;
          return;
        }
        if (0x2000 < op) {
          if (op == 0x4000) {
            seq[2] = -1;
            seq[0x22] = -1;
            return;
          }
          if (op == 0x8000) {
            seq[2] = -2;
            seq[0x22] = -1;
            return;
          }
LAB_801aa618:
          if (seq[0x22] == 0) {
LAB_801aa65c:
            if ((int)((uint32_t)arg << 0x18) < 0) {
              seq[0x1f] = -1;
              arg = -arg;
            }
            else {
              seq[0x1f] = 1;
            }
          }
          else if (seq[0x22] == 1) {
            if ((int)(char)arg != (int)seq[0x23]) goto LAB_801aa65c;
            seq[0x1d] = 0;
            seq[0x22] = 0;
            seq[0x1c] = seq[0x24];
            goto LAB_801aa55c;
          }
          seq[7] = (short)((char)arg + -1);
          seq[0x1e] = (short)(char)arg;
          seq[5] = *(short *)guest_at((uint32_t)(uintptr_t)(((((char)arg + -1) * 0x10000 >> 0x10) * 0x14 +
                                  *(int *)(seq + 0xc) + 4) ))+ -1;
          *(uint32_t *)(seq + 10) =
               *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0x10)));
          seq[8] = *(short *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc))));
          *(uint32_t *)(seq + 0x12) =
               *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 8)));
          *(uint32_t *)(seq + 0x14) =
               *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0xc)));
          if (seq[1] == 1) {
            /* guest div @0x801AA75C/0x801AA79C: num=0x1000, den=lh[p1+0xa]+1;
             * break @0x801AA768/@0x801AA7A8 (den==0) are recompiler no-ops
             * -> LO = den==0 ? -1 : 0x1000/den. C /0 would SIGFPE. */
            int32_t _den = (int32_t)seq[5] + 1;
            int32_t _q = (_den == 0) ? -1 : 0x1000 / _den;
            if (seq[0x1f] < 1) {
              seq[0x20] = 0x1000;
              t = -(short)_q;
            }
            else {
              t = (short)_q;
              seq[0x20] = 0;
            }
            seq[0x21] = t;
            seq[0x20] = seq[0x20] + seq[0x21];
          }
          break;
        }
        if (op == 0x100) goto LAB_801aa600;
        if (op != 0x200) goto LAB_801aa618;
        seq[0x1c] = (short)(char)arg;
        seq[0x1d] = 0;
      } while( true );
    }
    if (seq[1] == 2) {
      if (seq[0x1f] < 1) {
        verts_sub_apply(*(uint32_t *)(seq + 0xe),(uint32_t *)guest_at(*(uint32_t *)(seq + 0x12)),
                     (uint16_t *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8]);
      }
      else {
        verts_add_apply(*(uint32_t *)(seq + 0xe),(uint32_t *)guest_at(*(uint32_t *)(seq + 0x12)),
                     (uint16_t *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8]);
      }
    }
    else {
      t = seq[0x20];
      if (t < 0) {
        seq[0x20] = 0;
        t = seq[0x20];
      }
      if (0x1000 < t) {
        seq[0x20] = 0x1000;
      }
      if (*seq == 0) {
        verts_offset_apply((uint32_t *)guest_at(*(uint32_t *)(seq + 0x12)),(uint32_t *)guest_at(*(uint32_t *)(seq + 0x14)),
                     *(uint32_t *)(seq + 0xe),(short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],
                     (int)seq[0x20]);
      }
      else if (*seq == 2) {
        off = seq[7] * 0x14 + *(int *)(seq + 0xc);
        if (*(int *)guest_at((uint32_t)(uintptr_t)((gp_in + 0x24) ))== 0) {
          va = *(int *)(seq + 0x10);
        }
        else {
          va = *(int *)(seq + 0x10) + seq[4] * 4;
        }
        verts_rgb_apply((uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + 8)))),(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + 0xc)))),va,
                     (short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],(int)seq[0x20]);
      }
      seq[0x20] = seq[0x20] + seq[0x21];
    }
    seq[5] = seq[5] + -1;
  }
  return;
LAB_801aa600:
  t = (short)(char)arg;
  goto LAB_801aa558;

}

/* prim_group_gc @0x801ac154 — garbage-collect dead entries in a
 * primitive group. */
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
          task_node_free((int *)(uintptr_t)*(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c)))))));
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x1c)))) ))= 0;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x34)))) ))= 0;
        }
        else if (st == -1) goto LAB_801ac2ac;
      }
      ent = *(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x38))))));
      if (ent != 0) {
        st = *(short *)guest_at((uint32_t)(uintptr_t)((ent + 4)));
        if (st == -2) {
          task_node_free((int *)(uintptr_t)*(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x20)))))));
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x20)))) ))= 0;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x38)))) ))= 0;
        }
        else if (st == -1) goto LAB_801ac2ac;
      }
      ent = *(int *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c))))));
      if (ent != 0) {
        st = *(short *)guest_at((uint32_t)(uintptr_t)((ent + 4)));
        if (st == -2) {
          task_node_free((int *)(uintptr_t)*(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))))));
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x24)))) ))= 0;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((off + *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x3c)))) ))= 0;
        }
        else if (st == -1) {
LAB_801ac2ac:
          node_tree_free(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x14) ))+ 0x10))));
          return;
        }
      }
      i = i + 1;
      off = off + 4;
    } while (i < *(int *)guest_at((uint32_t)(uintptr_t)((grp + 0x44))));
  }
  return;

}

/* rect_prim_spawn @0x801ace48 — spawn a rect primitive task node:
 * write x0/y0/x1/y1 + flags into the record. */
void rect_prim_spawn(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1, uint8_t b0,uint8_t b1,uint16_t fl,uint16_t aux, uint32_t kind)
{
  int t;
  t = (int)(uintptr_t)task_node_alloc(kind,2,0xe,0x801acdbc,0,0x20);
  if (t != 0) {
    t = *(int *)guest_at((uint32_t)(uintptr_t)((t + 0x10)));
    prim_packet_init(t + 0x14,0,0,fl);
    prim_tag_set(t);
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t + 8) ))= x0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t + 10) ))= y0;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t + 0x10) ))= x1;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t + 0x12) ))= y1;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t + 0xc) ))= b0;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t + 0xd) ))= b1;
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((t + 0xe) ))= aux;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 0x1c) ))= 1;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t + 7) ))= *(uint8_t *)guest_at((uint32_t)(uintptr_t)((t + 7) ))| 3;
  }
  return;

}

/* isqrt_refine @0x80194280 — integer square-root refinement: paired
 * g[]/p[] recurrences seeded n±0x5d50ad, iterated i=1..6 (see the
 * in-body comment for the full recurrence description). */
uint32_t isqrt_refine(int n)
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
  return (uint32_t)g[6];

}

/* cop2_clz @0x801954b4 — cop2 count-leading-zeros op. */
uint32_t cop2_clz(uint32_t v)
{
  uint32_t res;
  setCopReg(2,0xf000,v);
  res = getCopReg(2,0xf800);
  return res;

}

/* gte_points_xform @0x80195500 — transform n points: ip -> sz/sx/sy/cc
 * output streams. */
void gte_points_xform(uint32_t *ip,uint32_t *sz,uint16_t *sx,uint16_t *sy, uint16_t *cc,int n)
{
  uint32_t cop2_idx0 = 0;
  uint32_t cop2_idx1 = 0x800;
  uint32_t code;
  uint32_t t0;
  uint32_t t1;
  do {
    setCopReg(2,cop2_idx0,*ip);
    setCopReg(2,cop2_idx1,ip[1]);
    copFunction(2,0x180001);
    ip = ip + 2;
    n = n + -1;
    t0 = getCopReg(2,0xe);
    *sz = t0;
    t0 = getCopReg(2,0x9800);
    t1 = getCopReg(2,0x4000);
    code = getCopControlWord(2,0xf800);
    *sx = (short)t0;
    *sy = (short)t1;
    *cc = (short)(code >> 0xc);
    sz = sz + 1;
    sx = sx + 1;
    sy = sy + 1;
    cc = cc + 1;
  } while (0 < n);
  return;

}

/* tween_track_alloc @0x801a953c — see tween_seq_alloc (track variant). */
int tween_track_alloc(int mode,uint16_t a1,uint32_t a2,uint16_t *kf,short arg5 ,uint16_t arg6,uint16_t arg7,uint32_t kind)
{
  uint16_t fld0;
  int node;
  int ret;
  uint16_t *pay;
  node = (int)(uintptr_t)task_node_alloc(kind,2,3,0x801a972c,0,0x4c);
  ret = 0;
  if (node != 0) {
    pay = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
    *pay = (short)mode;
    pay[1] = a1;
    pay[2] = 0;
    pay[3] = arg5 + -1;
    pay[4] = arg6;
    pay[6] = arg7;
    if (mode == 0) {
      *(uint32_t *)(pay + 0xe) = a2;
    }
    else {
      *(uint32_t *)(pay + 0x10) = a2;
    }
    /* kf is a GUEST address (keyframe descriptor); resolve fields via
     * guest_at and store it as a 32-bit guest pointer, not a host ptr.
     * It may arrive as a guest address in pointer costume (clean callers)
     * or as a host pointer (clean_table wrap); br2_gaddr yields the
     * canonical guest address for both. */
    uint32_t p4g = br2_gaddr(kf);
    *(uint32_t *)((char *)pay + 0x18) = p4g;
    *(uint32_t *)((char *)pay + 0x24) = *(uint32_t *)guest_at(p4g + 8);
    *(uint32_t *)((char *)pay + 0x28) = *(uint32_t *)guest_at(p4g + 12);
    *(uint32_t *)((char *)pay + 0x14) = *(uint32_t *)guest_at(p4g + 16);
    fld0 = *(uint16_t *)guest_at(p4g);
    pay[5] = 0;
    pay[7] = 0;
    pay[8] = fld0;
    *(int *)(pay + 0x16) = (int)(short)pay[5];
    *(int *)(pay + 0x18) = (int)(short)pay[7];
    ret = node;
  }
  return ret;

}

/* tween_apply @0x801a9c24 — apply tween value v0->v1 with phase/dir
 * accumulation to the seq target. */
void tween_apply(short *seq,int v0,int v1)
{
  short d1;
  short d2;
  uint16_t cur;
  bool run;
  int t;
  short *d;
  short *av;
  int va;
  int wt;
  int vo;
  /* phase is guest $t4 (slti 3 @0x801A9CF8/0x801A9D10, =2 @0x801A9D14):
   * the t==v0 path reads it before any write, i.e. the caller's $t4. */
  uint32_t phase = br2_gpr(12);
  /* dir is guest $t6 (li 1/-1 @0x801A9C88/0x801A9CC8): the phase==1
   * apply path reads it without a write this call, i.e. caller's $t6. */
  int dir = (int)br2_gpr(14);
  short v1s;
  int gp_in = (int)guest_gp();
  /* acc is guest $sp+0x18..0x27 (frame 0x30): acc[phase] is indexed by
   * the stale-$t4 phase on the t==v0 path and acc[0]/acc[2] are only
   * conditionally written, so back it with the guest stack window
   * (entry sp-0x18; no guest calls intervene before the apply loop). */
  int32_t *acc = (int32_t *)guest_at((uint32_t)clean_cpu_state()->gpr[29] - 0x18);
  if (seq == (short *)0x0) {
    return;
  }
  v1s = (short)v1;
  if (seq[1] == 1) {
    if (((*seq == 0) && (seq[7] == v0)) && (seq[5] == v1)) {
      return;
    }
    seq[7] = (short)v0;
    seq[5] = v1s;
    *(int *)(seq + 0x16) = v1;
    *(int *)(seq + 0x18) = v0;
    *(uint32_t *)(seq + 10) =
         *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0x10)));
    seq[8] = *(short *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc))));
    if (*seq == 0) {
      *(uint32_t *)(seq + 0x12) =
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 8)));
      *(uint32_t *)(seq + 0x14) =
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0xc)));
      verts_offset_apply((uint32_t *)guest_at(*(uint32_t *)(seq + 0x12)),(uint32_t *)guest_at(*(uint32_t *)(seq + 0x14)),
                   *(uint32_t *)(seq + 0xe),(short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],
                   v1);
      return;
    }
    if (*seq != 2) {
      return;
    }
    t = seq[7] * 0x14 + *(int *)(seq + 0xc);
    if (*(int *)guest_at((uint32_t)(uintptr_t)((gp_in + 0x24) ))== 0) {
      vo = *(int *)(seq + 0x10);
    }
    else {
      vo = *(int *)(seq + 0x10) + seq[4] * 4;
    }
    verts_rgb_apply((uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 8)))),(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t + 0xc)))),vo,
                 (short *)guest_at(*(uint32_t *)(seq + 10)),(int)seq[8],v1);
    return;
  }
  if (seq[1] != 2) {
    return;
  }
  if ((seq[7] == v0) && (seq[5] == v1)) {
    return;
  }
  acc[1] = 0;
  t = (int)seq[7];
  if (t < v0) {
    dir = 1;
    phase = 0;
    acc[2] = v1;
    acc[0] = *(int *)guest_at((uint32_t)(uintptr_t)((t * 0x14 + *(int *)(seq + 0xc) + 4) ))- (int)seq[5];
  }
  else {
    if (v0 < t) {
      dir = -1;
      acc[0] = -(int)seq[5];
      t = *(int *)guest_at((uint32_t)(uintptr_t)((v0 * 0x14 + *(int *)(seq + 0xc) + 4)));
      phase = 0;
    }
    else {
      run = (int)phase < 3;
      if (t != v0) goto LAB_801a9d14;
      dir = 0;
      t = (int)seq[5];
      phase = 2;
    }
    acc[2] = v1 - t;
  }
  run = phase < 3;
LAB_801a9d14:
  if (run) {
    do {
      t = 0;
      if (0 < seq[8]) {
        vo = 0;
        do {
          wt = acc[phase];
          d = (short *)guest_at((uint32_t)(uintptr_t)((t * 8 + *(int *)(seq + 0x12))));
          d1 = d[1];
          d2 = d[2];
          if (*d * wt != 0) {
            av = (short *)guest_at((uint32_t)(uintptr_t)((*(short *)guest_at((uint32_t)(uintptr_t)((vo + *(int *)(seq + 10)) ))* 8 +
                              *(int *)(seq + 0xe))));
            *av = *av + (short)(*d * wt);
          }
          if (d1 * wt != 0) {
            va = *(short *)guest_at((uint32_t)(uintptr_t)((vo + *(int *)(seq + 10)) ))* 8 + *(int *)(seq + 0xe);
            *(short *)guest_at((uint32_t)(uintptr_t)((va + 2) ))= *(short *)guest_at((uint32_t)(uintptr_t)((va + 2) ))+ (short)(d1 * wt);
          }
          if (d2 * wt != 0) {
            va = *(short *)guest_at((uint32_t)(uintptr_t)((vo + *(int *)(seq + 10)) ))* 8 + *(int *)(seq + 0xe);
            *(short *)guest_at((uint32_t)(uintptr_t)((va + 4) ))= *(short *)guest_at((uint32_t)(uintptr_t)((va + 4) ))+ (short)(d2 * wt);
          }
          t = t + 1;
          vo = vo + 2;
        } while (t < seq[8]);
      }
      if (phase == 1) {
        cur = seq[7];
        seq[7] = (short)((uint32_t)cur + dir);
        t = (int)(((uint32_t)cur + dir) * 0x10000) >> 0x10;
LAB_801a9ec0:
        if (t == v0) {
          phase = 2;
        }
      }
      else if ((int)phase < 2) {
        if (phase == 0) {
          cur = seq[7];
          seq[7] = (short)((uint32_t)cur + dir);
          acc[1] = *(int *)guest_at((uint32_t)(uintptr_t)((((int)(((uint32_t)cur + dir) * 0x10000) >> 0x10) * 0x14 +
                                 *(int *)(seq + 0xc) + 4) ))* dir;
          t = (int)seq[7];
          phase = 1;
          goto LAB_801a9ec0;
        }
      }
      else if (phase == 2) {
        phase = 3;
      }
      *(uint32_t *)(seq + 0x12) =
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 8)));
      *(uint32_t *)(seq + 10) =
           *(uint32_t *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc) + 0x10)));
      seq[8] = *(short *)guest_at((uint32_t)(uintptr_t)((seq[7] * 0x14 + *(int *)(seq + 0xc))));
    } while ((int)phase < 3);
  }
  seq[5] = v1s;
  *(int *)(seq + 0x16) = (int)v1s;
  *(int *)(seq + 0x18) = (int)seq[7];
  return;

}

/* rect_prim_tick @0x801acdbc — per-tick update for a rect primitive. */
void rect_prim_tick(uint32_t *prim)
{
  uint32_t cnt;
  uint32_t *cur;
  int gp = (int)guest_gp();
  cnt = prim[7];
  prim[7] = cnt - 1;
  if ((int)(cnt - 1) < 0) {
    task_slot_step();
  }
  else {
    *prim = *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp + 0x48) )))| 0x4000000;
    *(uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp + 0x48) )))= guest_of(prim) & 0xffffff;
    cur = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((gp + 0x48))));
    prim[5] = *cur | 0x1000000;
    *cur = guest_of(prim + 5) & 0xffffff;
  }
  return;

}

/* prim_tag_set @0x801acf94 — set the primitive tag/OT link bits. */
void prim_tag_set(int prim)
{
  *(uint8_t *)guest_at((uint32_t)(uintptr_t)((prim + 3) ))= 4;
  *(uint8_t *)guest_at((uint32_t)(uintptr_t)((prim + 7) ))= 100;
  return;

}

/* prim_packet_init @0x801acfb4 — init a primitive packet
 * header: tag word + attribute flags (texture-less variant;
 * see prim_packet_init_tex for the tex-bound form). */
void prim_packet_init(int pkt,int flag1,int flag2,uint32_t fl)
{
  uint32_t tagw;
  *(uint8_t *)guest_at((uint32_t)(uintptr_t)((pkt + 3) ))= 1;
  tagw = 0xe1000000;
  if (flag2 != 0) {
    tagw = 0xe1000200;
  }
  fl = fl & 0x9ff;
  if (flag1 != 0) {
    fl = fl | 0x400;
  }
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)((pkt + 4) ))= tagw | fl;
  return;

}

/* verts_offset_apply @0x801acfe4 — verts_*_apply — vertex-array passes: offset / rgb /
 * add / sub transforms over cnt verts with color stride; n is the guest
 * base address the per-vertex pointers index into. */
void verts_offset_apply(uint32_t *v,uint32_t *c,int n,short *off,int cnt, uint32_t fl)
{
  uint32_t *cz;
  short t;
  short *vp;
  uint32_t xy;
  uint32_t tx;
  uint32_t ty;
  uint32_t tz;
  uint32_t cur;
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
      vp[1] = (short)((uint32_t)cur >> 0x10) + (short)ty;
      vp[2] = t + (short)tz;
    } while (0 < cnt);
  }
  return;

}

/* verts_rgb_apply @0x801ad070 — see verts_offset_apply. */
void verts_rgb_apply(uint32_t *v,uint32_t *c,int n,short *off,int cnt, uint32_t fl)
{
  short vi;
  char *vp;
  uint32_t cur;
  uint32_t tgt;
  uint32_t dr;
  uint32_t dg;
  uint32_t db;
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

/* verts_add_apply @0x801ad11c — see verts_offset_apply. */
void verts_add_apply(int n,uint32_t *v,uint16_t *c,int d)
{
  short dz;
  uint16_t vi;
  uint32_t cur;
  uint32_t dlt;
  short *vp;
  if (d != 0) {
    do {
      d = d + -1;
      vi = *c;
      c = c + 1;
      vp = (short *)guest_at((uint32_t)(uintptr_t)(((uint32_t)vi * 8 + n)));
      cur = *(uint32_t *)vp;
      dlt = *v;
      dz = *(short *)(v + 1);
      v = v + 2;
      *vp = (short)cur + (short)dlt;
      vp[1] = (short)((uint32_t)cur >> 0x10) + (short)((uint32_t)dlt >> 0x10);
      vp[2] = vp[2] + dz;
    } while (0 < d);
  }
  return;

}

/* verts_sub_apply @0x801ad180 — see verts_offset_apply. */
void verts_sub_apply(int n,uint32_t *v,uint16_t *c,int d)
{
  short dz;
  uint16_t vi;
  uint32_t cur;
  uint32_t dlt;
  short *vp;
  if (d != 0) {
    do {
      d = d + -1;
      vi = *c;
      c = c + 1;
      vp = (short *)guest_at((uint32_t)(uintptr_t)(((uint32_t)vi * 8 + n)));
      cur = *(uint32_t *)vp;
      dlt = *v;
      dz = *(short *)(v + 1);
      v = v + 2;
      *vp = (short)cur - (short)dlt;
      vp[1] = (short)((uint32_t)cur >> 0x10) - (short)((uint32_t)dlt >> 0x10);
      vp[2] = vp[2] - dz;
    } while (0 < d);
  }
  return;

}

/* ---- batch19b: fixed_point_divide manual port (capstone) ---- */

/* fixed_point_divide @0x8018d300 — capstone-verified fixed-point divide (2-pass Newton refinement).
   div= signed 32/32 mflo, multu= unsigned 32x32 -> mfhi:mflo. */
/* fixed_point_divide @0x8018d300 — fixed-point division helper. */
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
   entries stride 0x14, field +4 = frame time. Ends: lhu(+0x2c)->+0xa
   then tween_apply(node,idx,acc). */
/* keyframe_cursor_search @0x801a972c — binary/linear search for the
 * keyframe cursor position in a sequence. */
void keyframe_cursor_search(uint32_t *cur)
{
  int gp_in = (int)guest_gp();
  int t0;
  int pos;
  int it;
  int dir;
  int idx;
  uint8_t *cb;
  uint32_t *ent;
  int tv;

  if (*(int *)guest_at((uint32_t)(uintptr_t)((gp_in + 0x44) ))== 1) {
    return;
  }
  if (cur == (uint32_t *)0x0) {
    return;
  }
  cb = (uint8_t *)cur;
  t0 = (int)*(short *)(cb + 0xa);
  pos = *(int *)(cb + 0x2c);
  if (pos == t0) {
    return;
  }
  dir = 1;
  idx = (int)*(short *)(cb + 0xe);
  it = 0;
  if (t0 < pos) {
    dir = -1;
  }
  t0 = pos - t0;
  if (t0 < 0) {
LAB_801a9784:
    t0 = (int)*(short *)(cb + 0xa) - *(int *)(cb + 0x2c);
    if (it >= t0) {
      goto LAB_801a9830;
    }
  }
  else if (it >= t0) {
    goto LAB_801a9830;
  }
  for (;;) {
LAB_801a97a0:
    ent = (uint32_t *)guest_at(*(uint32_t *)(cb + 0x18));
    tv = *(int *)(ent + idx * 5 + 1);
    pos = pos + dir;
    if (tv < pos) {
      if (0 <= pos) {
        goto LAB_801a9810;
      }
      idx = idx + -1;
      ent = (uint32_t *)guest_at(*(uint32_t *)(cb + 0x18));
      tv = *(int *)(ent + idx * 5 + 1);
      pos = tv + -1;
      if (idx < 0) {
        idx = 0;
        pos = 0;
        goto LAB_801a9830;
      }
      goto LAB_801a9810;
    }
    idx = idx + 1;
    pos = 1;
    if ((int)*(short *)(cb + 6) <= idx) {
      idx = (int)*(short *)(cb + 6);
      pos = 0;
      goto LAB_801a9830;
    }
LAB_801a9810:
    t0 = *(int *)(cb + 0x2c) - (int)*(short *)(cb + 0xa);
    it = it + 1;
    if (t0 < 0) {
      goto LAB_801a9784;
    }
    if (it < t0) {
      continue;
    }
    break;
  }
LAB_801a9830:
  *(uint16_t *)(cb + 0xa) = *(uint16_t *)(cb + 0x2c);
  tween_apply((short *)cur,idx,pos);
  return;
}
/* globals/callees for batch24 ports */
extern uint8_t  *g_DAT_1f8001f3;
#define DAT_1f8001f3   (*g_DAT_1f8001f3)
extern uint32_t *g__DAT_801c4494,*g__DAT_801c44e4;
#define _DAT_801c4494  (*g__DAT_801c4494)
#define _DAT_801c44e4  (*g__DAT_801c44e4)
extern uint8_t  *g_DAT_801b1eac;
#define DAT_801b1eac   (*g_DAT_801b1eac)
extern uint8_t **g_DAT_801b22ec;
#define PTR_DAT_801b22ec (*g_DAT_801b22ec)
extern uint16_t *g_DAT_801b2308,*g_DAT_801b2314;
#define DAT_801b2308   (*g_DAT_801b2308)
#define DAT_801b2314   (*g_DAT_801b2314)
extern uint32_t *g__DAT_801b22f8,*g__DAT_801b22fc,*g__DAT_801b2304;
#define DAT_801b22f8   (*g__DAT_801b22f8)
#define DAT_801b22fc   (*g__DAT_801b22fc)
#define DAT_801b2304   (*g__DAT_801b2304)
extern uint8_t  *g_DAT_801b180c;
#define DAT_801b180c   (*g_DAT_801b180c)
extern uint32_t *g_PTR_DAT_801b1d44;
#define PTR_DAT_801b1d44 ((uint32_t)(uintptr_t)g_PTR_DAT_801b1d44)
extern void *g_PTR_FUN_801b1ec0;
extern void motion_integrator_tail(int f,int n);
extern void keyframe_advance(uint8_t *ctrl,uint8_t *motion);
extern void spark_trail_task_update(int *rec);
extern void confetti_burst_update(int rec);
extern void LAB_8013e558(uint32_t *ctx);
extern void LAB_80140510(int *ctx);
extern void ring_effect_task_update(int rec);
extern void firework_task_update(int rec);

/* guest popup_spawn @0x8013ab8c */
void popup_spawn(int ent,uint32_t kind,uint32_t fl)

{
  int t0;
  int *src;
  uint8_t *cur;
  int t1;
  int *rec;
  int t2;
  int t3;

  t1 = 0;
  cur = &DAT_1f8000e8;
  while (((*(uint32_t *)(cur + 0x58) & 0xc0000000) == 0 || (cur[5] != '\0'))) {
    t1 = t1 + 1;
    cur = cur + 0x108;
    if (1 < t1) {
      t0 = (int)(uintptr_t)task_node_alloc(1,2,10,0x8013a9d8,0,0x1c);
      t1 = _DAT_801c4578;
      if (t0 != 0) {
        rec = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
        t0 = 0;
        if (DAT_1f8000eb == kind) {
          t0 = _DAT_801c4494;
        }
        if (DAT_1f8001f3 == kind) {
          t0 = _DAT_801c44e4;
        }
        if (t0 == 0) {
          task_node_free(0);
        }
        else {
          src = (int *)guest_at((uint32_t)(uintptr_t)(((fl & 0x7f) * 0x18 + t0)));
          *rec = ent;
          t0 = src[1];
          t2 = src[2];
          t3 = src[3];
          rec[1] = *src;
          rec[2] = t0;
          rec[3] = t2;
          rec[4] = t3;
          t0 = src[5];
          rec[5] = src[4];
          rec[6] = t0;
          *(uint16_t *)((intptr_t)rec + 6) = 0;
          if (((t1 == 0) || (t1 == 3)) && (0x2d < (short)rec[3])) {
            *(uint16_t *)(rec + 3) = 0x2d;
          }
          if (400 < (short)rec[6]) {
            *(uint16_t *)(rec + 6) = 400;
          }
          if ((*(char *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x38) ))+ 0x100) ))!= '\0') &&
             (*(uint16_t *)((intptr_t)rec + 10) =
                   (uint16_t)(uint8_t)(&DAT_801b180c)[*(short *)((intptr_t)rec + 10)],
             (fl & 0x80) != 0)) {
            *(short *)(rec + 4) = 0x1000 - (short)rec[4];
          }
          *(short *)(rec + 5) = (short)rec[5] + -0x400;
          *(short *)(rec + 4) = (short)rec[4] + -0x400;
        }
      }
      return;
    }
  }
  return;
}

/* guest confetti_burst_spawn @0x8013da2c */
void confetti_burst_spawn(uint32_t ent,uint32_t arg)

{
  uint16_t r0;
  uint16_t r1;
  uint16_t r2;
  int t0;
  int off;
  int it2;
  uint32_t *d0;
  uint32_t *d1;
  int it;
  uint32_t *rec;

  t0 = (int)(uintptr_t)task_node_alloc(1,2,0xb,0x8013d7e4,0,0x598);
  it = 0;
  if (t0 != 0) {
    rec = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
    rec[0x155] = 0x10;
    rec[0x152] = ent;
    rec[0x154] = arg;
    r0 = rand_lcg();
    r1 = rand_lcg();
    d0 = rec;
    d1 = rec;
    do {
      r0 = r0 + 99;
      r1 = r1 + 0x85;
      *(uint16_t *)(d0 + 0x140) = (r0 & 0x1ff) - 0x100;
      *(uint16_t *)((intptr_t)d0 + 0x502) = (r1 & 0x1ff) - 0x100;
      *(uint16_t *)(d1 + 0x156) = 0x1000;
      r2 = rand_lcg();
      *(uint16_t *)(d1 + 0x15e) = (r2 & 0x3f) + 10;
      d1 = (uint32_t *)((intptr_t)d1 + 2);
      it = it + 1;
      d0 = d0 + 1;
    } while (it < 0x10);
    it = 0;
    t0 = 0;
    d0 = rec;
    do {
      it2 = 0;
      d1 = d0;
      off = t0;
      do {
        *d1 = 0x9000000;
        *(uint8_t *)((intptr_t)d1 + 7) = 0x2c;
        *(uint16_t *)((intptr_t)rec + off + 0x16) = 0x3f;
        *(uint16_t *)((intptr_t)rec + off + 0xe) = 31000;
        it2 = it2 + 1;
        *(uint8_t *)(d1 + 3) = 0x40;
        *(uint8_t *)((intptr_t)d1 + 0xd) = 0x80;
        *(uint8_t *)(d1 + 5) = 0x48;
        *(uint8_t *)((intptr_t)d1 + 0x15) = 0x80;
        *(uint8_t *)(d1 + 7) = 0x40;
        *(uint8_t *)((intptr_t)d1 + 0x1d) = 0x88;
        *(uint8_t *)(d1 + 9) = 0x48;
        *(uint8_t *)((intptr_t)d1 + 0x25) = 0x88;
        *(uint8_t *)(d1 + 1) = 0x7f;
        *(uint8_t *)((intptr_t)d1 + 5) = 0x7f;
        *(uint8_t *)((intptr_t)d1 + 6) = 0x7f;
        *(uint8_t *)((intptr_t)d1 + 7) = *(uint8_t *)((intptr_t)d1 + 7) & 0xfe | 2;
        d1 = d1 + 10;
        off = off + 0x28;
      } while (it2 < 0x10);
      d0 = d0 + 0xa0;
      it = it + 1;
      t0 = t0 + 0x280;
    } while (it < 2);
  }
  return;
}

/* guest speech_bubble_spawn @0x8013f178 */
void speech_bubble_spawn(uint32_t *pos,short ang,int idx,uint16_t fl)

{
  short *fx;
  int t0;
  int t1;
  uint32_t q;
  int *rec;

  if (((DAT_1f8000b1 == '\0') && (_DAT_801f0930 < 3)) &&
     ((*(uint16_t *)(_g_pad_buffer + 2) >> 4 & 1) != 0)) {
    if ((DAT_1f800098 != '\x01') && ((fl & 0xf0) == 0)) {
      fx = (short *)guest_at((uint32_t)(uintptr_t)(fx_record_alloc((uint32_t)br2_gaddr(pos),6,0x8000000a,2,PTR_DAT_801b1d44)));
      if (fx != (short *)0x0) {
        *fx = ang + 0x800;
      }
    }
    t0 = (int)(uintptr_t)task_node_alloc(1,2,5,0x8013e558,0,0x48);
    if (t0 != 0) {
      rec = (int *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
      t1 = (int)(uintptr_t)prim_group_build((uint32_t)br2_gaddr(rec),*(uint32_t *)(&DAT_801c43a8 + idx * 4),0,1);
      *rec = (int)(uintptr_t)t1;
      if (t1 != 0) {
        if ((fl & 0xf) != 0) {
          prim_attr_word_set(t1,(fl & 0xf) << 6 | 0x781a);
        }
        t1 = tween_ensure(*rec);
        if (t1 != 0) {
          *(uint16_t *)((intptr_t)rec + 0x46) = 0;
          t1 = DAT_801b1848;
          t0 = DAT_801b1844;
          rec[1] = DAT_801b1840;
          rec[2] = t0;
          rec[3] = t1;
          t1 = DAT_801b1854;
          t0 = DAT_801b1850;
          rec[4] = DAT_801b184c;
          rec[5] = t0;
          rec[6] = t1;
          t0 = DAT_801b185c;
          rec[7] = DAT_801b1858;
          rec[8] = t0;
          *(uint16_t *)(rec + 0x11) = 0x138;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*rec ))+ 0x14) ))= *pos;
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*rec ))+ 0x18) ))= pos[1];
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*rec ))+ 0x1c) ))= pos[2];
          t0 = angle_from_xy(*pos,pos[2]);
          q = (int)(t0 + 0x200U & 0xfff) >> 10;
          if (q == 1) {
            t0 = 0x400;
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*rec ))+ 0x14) ))= (int)_DAT_801f093a;
          }
          else if (q < 2) {
            if (q == 0) {
              t0 = 0x800;
              *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*rec ))+ 0x1c) ))= -(int)_DAT_801f093e;
            }
          }
          else if (q == 2) {
            t0 = 0;
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*rec ))+ 0x1c) ))= (int)_DAT_801f093e;
          }
          else if (q == 3) {
            t0 = 0xc00;
            *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(*rec ))+ 0x14) ))= -(int)_DAT_801f093a;
          }
          rotmat_rtps_build(t0,rec + 9);
          _DAT_801f0930 = _DAT_801f0930 + 1;
          return;
        }
        node_tree_free(*rec);
      }
      task_node_free((int *)(uintptr_t)t0);
    }
  }
  return;
}

/* guest effect_arg_set @0x8013f5f8 */
void effect_arg_set(uint32_t arg)

{
  uint32_t *ent;

  if ((DAT_1f8000b1 == '\0') &&
     (ent = (uint32_t *)guest_at((uint32_t)(uintptr_t)(task_alloc_wrap_e(4,(int)br2_gaddr(g_PTR_FUN_801b1ec0),(int)guest_of(&DAT_801b1eac)))),
     ent != (uint32_t *)0x0)) {
    *ent = arg;
  }
  return;
}

/* guest hit_spark_spawn @0x80140710 */
void hit_spark_spawn(uint32_t pos,uint32_t kind,uint16_t aux)

{
  int node;
  uint32_t *rec;
  uint8_t *dat;

  if ((DAT_1f8000b1 == '\0') && ((kind & 0x7f) < 3)) {
    node = (int)(uintptr_t)task_node_alloc(1,2,8,0x80140510,0,0x14);
    if (node != 0) {
      rec = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((node + 0x10))));
      /* guest: lw v1, (0x801B22EC + (kind&0x7f)*4) — u32 table of guest
       * data addrs. g_DAT_801b22ec is uint8_t** so [&PTR][i] reads 8
       * bytes at i*8; index as u32 instead. */
      dat = (uint8_t *)guest_at(((uint32_t *)&PTR_DAT_801b22ec)[kind & 0x7f]);
      *rec = (uint32_t)br2_gaddr(dat);
      rec[1] = (uint32_t)br2_gaddr(dat);
      *(uint16_t *)(rec + 3) = 0;
      *(uint16_t *)((intptr_t)rec + 0xe) = 0;
      rec[2] = pos;
      *(uint16_t *)(rec + 4) = (uint16_t)kind & 0x80;
      *(uint16_t *)((intptr_t)rec + 0x12) = aux;
    }
  }
  return;
}

/* guest sprite_burst_spawn @0x80140b18 */
void sprite_burst_spawn(uint32_t ent,uint32_t arg)

{
  uint16_t t;
  short ang;
  int t0;
  uint32_t *d0;
  uint32_t *d1;
  uint16_t *s0;
  uint16_t *s1;
  int it;
  uint32_t *rec;

  if (DAT_1f8000b1 == '\0') {
    t0 = (int)(uintptr_t)task_node_alloc(1,2,0xe,0x801407cc,0,400);
    it = 0;
    if (t0 != 0) {
      rec = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
      rec[0x58] = ent;
      rec[0x59] = arg;
      rec[0x5a] = 0x14;
      ang = rand_lcg();
      s1 = &DAT_801b2314;
      s0 = &DAT_801b2308;
      d0 = rec;
      do {
        t = *s0;
        s0 = s0 + 1;
        *(uint16_t *)(d0 + 0x5e) = t;
        t = *s1;
        it = it + 1;
        *(short *)(d0 + 0x5b) = ang;
        ang = ang + 0x2aa;
        s1 = s1 + 1;
        *(uint16_t *)(d0 + 0x61) = t;
        d0 = (uint32_t *)((intptr_t)d0 + 2);
      } while (it < 6);
      t0 = 0;
      d0 = rec;
      do {
        it = 0;
        d1 = d0;
        do {
          d1[1] = DAT_801b22f8;
          d1[3] = DAT_801b22fc;
          d1[5] = DAT_801b2304;
          *d1 = 0x6000000;
          *(uint8_t *)((intptr_t)d1 + 7) = 0x32;
          it = it + 1;
          d1 = d1 + 7;
        } while (it < 6);
        rec[0x54] = 0x1000000;
        rec[0x55] = 0xe1000020;
        rec = rec + 2;
        t0 = t0 + 1;
        d0 = d0 + 0x2a;
      } while (t0 < 2);
    }
  }
  return;
}

/* guest firework_spawn @0x80141050 */
void firework_spawn(uint16_t *pos)

{
  uint16_t r;
  int t0;
  uint32_t *d1;
  int off;
  uint32_t *rec;
  int it;
  int it2;
  uint32_t *d0;

  t0 = (int)(uintptr_t)(int)(uintptr_t)task_node_alloc(1,2,0xb,0x80140d38,0,0x3d8);
  it = 0;
  if (t0 != 0) {
    off = 0;
    rec = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((t0 + 0x10))));
    *(uint16_t *)(rec + 0xf5) = 0x1a;
    d0 = rec;
    do {
      it2 = 0;
      d1 = d0;
      t0 = off;
      do {
        *d1 = 0x9000000;
        *(uint8_t *)((intptr_t)d1 + 7) = 0x2e;
        *(uint16_t *)((intptr_t)rec + t0 + 0x16) = 0x39;
        *(uint16_t *)((intptr_t)rec + t0 + 0xe) = 0x789a;
        *(uint8_t *)(d1 + 3) = 0xc0;
        *(uint8_t *)((intptr_t)d1 + 0xd) = 0x20;
        *(uint8_t *)(d1 + 5) = 0xdf;
        *(uint8_t *)((intptr_t)d1 + 0x15) = 0x20;
        *(uint8_t *)(d1 + 7) = 0xc0;
        *(uint8_t *)((intptr_t)d1 + 0x1d) = 0x3f;
        *(uint8_t *)(d1 + 9) = 0xdf;
        *(uint8_t *)((intptr_t)d1 + 0x25) = 0x3f;
        *(uint8_t *)(d1 + 1) = 0xa2;
        *(uint8_t *)((intptr_t)d1 + 5) = 0xa2;
        *(uint8_t *)((intptr_t)d1 + 6) = 0xa2;
        it2 = it2 + 1;
        d1 = d1 + 10;
        t0 = t0 + 0x28;
      } while (it2 < 10);
      d0 = d0 + 100;
      it = it + 1;
      off = off + 400;
    } while (it < 2);
    t0 = 0;
    d0 = rec;
    do {
      t0 = t0 + 1;
      r = rand_lcg();
      *(uint16_t *)(d0 + 0xf0) = (r & 0x1f) + 8;
      *(uint16_t *)(rec + 200) = *pos;
      d0 = (uint32_t *)((intptr_t)d0 + 2);
      *(uint16_t *)((intptr_t)rec + 0x322) = pos[2];
      *(uint16_t *)(rec + 0xc9) = pos[4];
      r = rand_lcg();
      *(uint16_t *)(rec + 0xdc) = 0x20 - (r & 0x3f);
      r = rand_lcg();
      *(uint16_t *)((intptr_t)rec + 0x372) = 0x20 - (r & 0x3f);
      r = rand_lcg();
      *(uint16_t *)(rec + 0xdd) = 0x20 - (r & 0x3f);
      rec = rec + 2;
    } while (t0 < 10);
  }
  return;
}

/* guest gte_mat_vec_swap @0x80185290 */
void gte_mat_vec_swap(uint32_t *mb,uint16_t *vec,uint32_t *ma)

{
  uint16_t e0;
  uint16_t e1;
  uint32_t x;
  uint32_t y;
  uint32_t z;
  uint32_t x2;
  uint32_t y2;
  uint32_t z2;

  setCopControlWord(2,0,*ma);
  setCopControlWord(2,0x800,ma[1]);
  setCopControlWord(2,0x1000,ma[2]);
  setCopControlWord(2,0x1800,ma[3]);
  setCopControlWord(2,0x2000,ma[4]);
  setCopReg(2,0,(int)(short)*vec);
  setCopReg(2,0x800,(int)(short)vec[6]);
  copFunction(2,0x486012);
  e0 = *(uint16_t *)(ma + 2);
  e1 = *(uint16_t *)((intptr_t)ma + 0xe);
  x = getCopReg(2,0x4800);
  y = getCopReg(2,0x5000);
  z = getCopReg(2,0x5800);
  setCopReg(2,0,-(int)(short)vec[6] & 0xffff);
  setCopReg(2,0x800,(int)(short)*vec);
  vec[1] = *(uint16_t *)((intptr_t)ma + 2);
  copFunction(2,0x486012);
  vec[4] = e0;
  vec[7] = e1;
  *vec = (uint16_t)x;
  vec[3] = (uint16_t)y;
  vec[6] = (uint16_t)z;
  y = getCopReg(2,0x4800);
  z = getCopReg(2,0x5000);
  x = getCopReg(2,0xb);
  *(uint32_t *)(vec + 8) = x;
  vec[2] = (uint16_t)y;
  vec[5] = (uint16_t)z;
  setCopControlWord(2,0,*mb);
  setCopControlWord(2,0x800,mb[1]);
  setCopControlWord(2,0x1000,mb[2]);
  setCopControlWord(2,0x1800,mb[3]);
  setCopControlWord(2,0x2000,mb[4]);
  setCopReg(2,0,(uint32_t)*vec | *(uint32_t *)(vec + 2) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(vec + 6));
  copFunction(2,0x486012);
  x = getCopReg(2,0x4800);
  y = getCopReg(2,0x5000);
  z = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)vec[1] | *(int *)(vec + 4) << 0x10);
  setCopReg(2,0x800,(int)(short)vec[7]);
  copFunction(2,0x486012);
  x2 = getCopReg(2,0x4800);
  y2 = getCopReg(2,0x5000);
  z2 = getCopReg(2,0x5800);
  setCopReg(2,0,(uint32_t)vec[2] | *(uint32_t *)(vec + 4) & 0xffff0000);
  setCopReg(2,0x800,*(uint32_t *)(vec + 8));
  copFunction(2,0x486012);
  *vec = (uint16_t)x;
  vec[3] = (uint16_t)y;
  vec[6] = (uint16_t)z;
  vec[1] = (uint16_t)x2;
  vec[4] = (uint16_t)y2;
  vec[7] = (uint16_t)z2;
  y = getCopReg(2,0x4800);
  z = getCopReg(2,0x5000);
  x = getCopReg(2,0xb);
  *(uint32_t *)(vec + 8) = x;
  vec[2] = (uint16_t)y;
  vec[5] = (uint16_t)z;
  return;
}

/* guest trig_cos @0x80185404 */
int trig_cos(int ang)

{
  int idx;
  uint32_t q;

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
int trig_sin(uint32_t ang)

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
  uint8_t flg;
  uint16_t t;
  short v;
  int rec;

  if (*(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc0) ))== 0) {
    motion_integrator_tail(f,n);
  }
  if (*(char *)guest_at((uint32_t)(uintptr_t)((n + 0x9c) ))!= '\0') {
    rec = *(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18)));
    t = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x23c)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x23c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x1c4)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18) ))+ 0x1c4) ))= t;
    rec = *(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18)));
    t = *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x28c)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x28c) ))= *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x264)));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x18) ))+ 0x264) ))= t;
  }
  flg = *(uint8_t *)guest_at((uint32_t)(uintptr_t)((n + 0x9c)));
  *(uint8_t *)guest_at((uint32_t)(uintptr_t)((n + 0x9c) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xc0) ))= 0;
  *(uint8_t *)guest_at((uint32_t)(uintptr_t)((n + 0x100) ))= *(uint8_t *)guest_at((uint32_t)(uintptr_t)((n + 0x100) ))^ flg;
  v = *(short *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)((n + 0xb0))));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xc4) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xc6) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xca) ))= 0;
  *(int *)guest_at((uint32_t)(uintptr_t)((n + 0xb4) ))= *(int *)guest_at((uint32_t)(uintptr_t)((n + 0xb0) ))+ 4;
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc2) ))= v << 4;
  v = *(short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((n + 0xb0) ))+ 6)));
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xc6) ))+ 1;
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xcc) ))= v << 4;
  keyframe_advance((uint8_t *)guest_at((uint32_t)f),(uint8_t *)guest_at((uint32_t)n));
  return;
}

/* guest pose_chain_build @0x801862a8 */
void pose_chain_build(int node)

{
  int lnk;
  uint16_t *dst;
  uint32_t *src;
  short *kf;
  int tag;
  int it;

  it = 0x13;
  kf = (short *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)(((*(uint8_t *)guest_at((uint32_t)(uintptr_t)((node + 5) ))& 1) * 0x1c +
                              *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x38) ))+ 0x10) ))+ 0x14) ))+
                    *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x30))))));
  tag = *(int *)guest_at((uint32_t)(uintptr_t)((node + 0x20) ))+ 0x5f0;
  if ('\x13' < *(char *)guest_at((uint32_t)(uintptr_t)((node + 8)))) {
    src = (uint32_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x20) ))+ 0x638)));
    dst = (uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((node + 0x40) ))+ 0x21c)));
    do {
      *(uint32_t *)(dst + 8) = (uint32_t)guest_of(kf);
      if (((int)*kf & 0x8000U) != 0) {
        lnk = *(int *)guest_at((uint32_t)(uintptr_t)((src[-2] + 0x44)));
        if (lnk == tag) {
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((src[-2] + 0x44) ))= *src;
        }
        else {
          for (; *(int *)guest_at((uint32_t)(uintptr_t)((lnk + 0x48) ))!= tag; lnk = *(int *)guest_at((uint32_t)(uintptr_t)((lnk + 0x48)))) {
          }
          *(uint32_t *)guest_at((uint32_t)(uintptr_t)((lnk + 0x48) ))= *src;
        }
      }
      dst[6] = 0;
      dst[5] = 0;
      dst[4] = 0;
      dst[3] = 0;
      dst[2] = 0;
      dst[1] = 0;
      *dst = 0;
      dst = dst + 0xe;
      src = src + 0x14;
      tag = tag + 0x50;
      it = it + 1;
      kf = kf + kf[1] + 2;
    } while (it < *(char *)guest_at((uint32_t)(uintptr_t)((node + 8))));
  }
  return;
}

/* guest euler_rotmat @0x8018dc18 */
uint16_t * euler_rotmat(uint32_t node,uint32_t ang0,uint32_t ang1,uint16_t *out)

{
  short g0;
  int t0;
  int t1;
  int t2;
  int t3;
  int t4;
  int t5;
  short g0n;
  int t7;
  int t8;
  int t9;
  uint32_t an;

  t0 = (ang0 & 0x3ff) * 2;
  t2 = (ang0 & 0x3ff) * -2 + 0x800;
  t1 = t0;
  if ((ang0 & 0x400) != 0) {
    t1 = t2;
    t2 = t0;
  }
  t2 = (int)*(short *)(&DAT_801247a8 + t2);
  an = node + ang1;
  if ((ang0 + 0x400 & 0x800) != 0) {
    t2 = -t2;
  }
  g0 = *(short *)(&DAT_801247a8 + t1);
  t1 = (int)g0;
  t3 = (node & 0x3ff) * 2;
  t4 = (node & 0x3ff) * -2 + 0x800;
  t0 = t3;
  if ((node & 0x400) != 0) {
    t0 = t4;
    t4 = t3;
  }
  t4 = (int)*(short *)(&DAT_801247a8 + t4);
  if ((node + 0x400 & 0x800) != 0) {
    t4 = -t4;
  }
  t0 = (int)*(short *)(&DAT_801247a8 + t0);
  t5 = (ang1 & 0x3ff) * 2;
  t7 = (ang1 & 0x3ff) * -2 + 0x800;
  t3 = t5;
  if ((ang1 & 0x400) != 0) {
    t3 = t7;
    t7 = t5;
  }
  t7 = (int)*(short *)(&DAT_801247a8 + t7);
  if ((ang1 + 0x400 & 0x800) != 0) {
    t7 = -t7;
  }
  t3 = (int)*(short *)(&DAT_801247a8 + t3);
  t8 = (an & 0x3ff) * 2;
  t9 = (an & 0x3ff) * -2 + 0x800;
  t5 = t8;
  if ((an & 0x400) != 0) {
    t5 = t9;
    t9 = t8;
  }
  t9 = (int)*(short *)(&DAT_801247a8 + t9);
  if ((node & 0x800) != 0) {
    t0 = -t0;
  }
  t5 = (int)*(short *)(&DAT_801247a8 + t5);
  if ((an + 0x400 & 0x800) != 0) {
    t9 = -t9;
  }
  if ((ang1 & 0x800) != 0) {
    t3 = -t3;
  }
  *out = (short)(t7 * t2 >> 0xc);
  out[7] = (short)(t0 * t2 >> 0xc);
  g0n = -g0;
  if ((ang0 & 0x800) != 0) {
    t1 = -t1;
    g0n = g0;
  }
  out[6] = g0n;
  if ((an & 0x800) != 0) {
    t5 = -t5;
  }
  t9 = t9 * (t1 + -0x1000);
  out[8] = (short)(t4 * t2 >> 0xc);
  node = node - ang1;
  out[3] = (short)(t3 * t2 >> 0xc);
  t4 = (node & 0x3ff) * 2;
  t5 = t5 * (t1 + -0x1000);
  t0 = (node & 0x3ff) * -2 + 0x800;
  t2 = t4;
  if ((node & 0x400) != 0) {
    t2 = t0;
    t0 = t4;
  }
  t0 = (int)*(short *)(&DAT_801247a8 + t0);
  if ((node + 0x400 & 0x800) != 0) {
    t0 = -t0;
  }
  t0 = t0 * (t1 + 0x1000);
  t2 = (int)*(short *)(&DAT_801247a8 + t2);
  if ((node & 0x800) != 0) {
    t2 = -t2;
  }
  t2 = t2 * (t1 + 0x1000);
  out[2] = (short)((t9 + t0) >> 0xd);
  out[4] = (short)((t0 - t9) >> 0xd);
  out[1] = (short)((t5 + t2) >> 0xd);
  out[5] = (short)((t5 - t2) >> 0xd);
  return out;
}

/* ---- batch25: task callbacks + motion_integrator_tail (verified vs disasm) ---- */
#define _DAT_1f8003fe (*(uint16_t *)((char *)&_DAT_1f8003fc + 2))

/* spark_trail_task_update @0x8013a9d8 — spark-trail task update (EV7 sub 3). */
void spark_trail_task_update(int *rec)
{
  uint16_t life;
  short pos;
  int t;
  int sp;

  if (DAT_801bef20 != 0) {
    return;
  }
  life = (uint16_t)((short)rec[1] - 1);
  *(uint16_t *)(rec + 1) = life;
  if ((int)((uint32_t)life << 0x10) < 0) {
    task_slot_step();
    return;
  }
  if ((_DAT_801c4578 == 0) || (_DAT_801c4578 == 3)) {
    if ((*(uint16_t *)(rec + 1) & 1) != 0) goto LAB_8013ab68;
    sp = ((int)(short)rec[3] << 3) / 10;
  }
  else {
    sp = (int)(short)rec[3];
  }
  t = (uint32_t)*(uint16_t *)((intptr_t)rec + 6) << 0x10;
  if ((t >> 0x13 < (t >> 0x10) + (int)(short)rec[2]) >> 3) {
    t = (int)(short)rec[6];
    if (t < 0) {
      t = t + 0xf;
    }
    impact_fx_spawn((uint32_t *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((*rec + 0x20) ))+ *(short *)((intptr_t)rec + 10) * 0x50 + 0x34)),0x80000005,
                 (int)(short)rec[4] + (int)*(short *)guest_at((uint32_t)(uintptr_t)((*rec + 0x4e))),(int)(short)rec[5],
                 (uint32_t)(sp << 0x10 | t >> 4));
    *(short *)(rec + 4) = (short)rec[4] + *(short *)((intptr_t)rec + 0x12);
    *(short *)(rec + 3) = (short)rec[3] + *(short *)((intptr_t)rec + 0xe);
    pos = (short)rec[6] + *(short *)((intptr_t)rec + 0x1a);
    *(short *)(rec + 6) = pos;
    *(short *)(rec + 5) = (short)rec[5] + *(short *)((intptr_t)rec + 0x16);
    if (pos < 0xa0) {
      *(uint16_t *)(rec + 6) = 0xa0;
    }
    if ((short)rec[3] < 0xf) {
      *(uint16_t *)(rec + 3) = 0xf;
    }
  }
LAB_8013ab68:
  *(short *)((intptr_t)rec + 6) = *(short *)((intptr_t)rec + 6) + (short)rec[2];
  return;
}

/* confetti_burst_update @0x8013d7e4 — confetti/debris burst update (16 sprites x 2 rows).
 * cop2_idx0->cop2 reg 0, cop2_idx1->cop2 reg 1 (0x800 encoding), verified
 * against lwc2 $0/$1 at 0x8013d8ac-0x8013d8b0. */
void confetti_burst_update(int rec)
{
  short *yp;
  uint32_t org;
  int t;
  short *wp;
  uint32_t *prim;
  int i;
  short ox;
  short oy;

  prim = (uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + _DAT_1f80000c * 0x280)));
  if ((_DAT_1f800082 == 0) &&
     (t = *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x554) ))+ -1, *(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x554) ))= t, t == 0)) {
    task_slot_step();
  }
  else {
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x540) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)
          (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x550) ))* 0x50 + *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x548) ))+ 0x20) ))+ 0x34));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x542) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)
          (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x550) ))* 0x50 + *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x548) ))+ 0x20) ))+ 0x38));
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x544) ))=
         *(uint16_t *)guest_at((uint32_t)(uintptr_t)
          (*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x550) ))* 0x50 + *(int *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((rec + 0x548) ))+ 0x20) ))+ 0x3c));
    setCopReg(2,0,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x540))));
    setCopReg(2,0x800,*(uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x544))));
    copFunction(2,0x180001);
    org = (uint32_t)getCopReg(2,0xe);
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
      oy = (short)((uint32_t)org >> 0x10);
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
      *prim = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))| 0x9000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))= (uint32_t)guest_of(prim) & 0xffffff;
      prim = prim + 10;
    } while (i < 0x10);
  }
  return;
}

/* ring_effect_task_update @0x801407cc — ring/effect task update (6 sprites, matrix transform). */
void ring_effect_task_update(int rec)
{
  uint32_t org;
  int t;
  int trig;
  short *wp;
  uint32_t *prim;
  int i;
  uint32_t in0;
  uint32_t in1;

  prim = (uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + _DAT_1f80000c * 0xa8)));
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
      in0 = ((uint32_t)*(uint16_t *)
                   guest_at((uint32_t)(uintptr_t)(ring_ent + 0x38)) << 16) |
               *(uint16_t *)
                 guest_at((uint32_t)(uintptr_t)(ring_ent + 0x34));
      in1 = (*(uint16_t *)
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
    org = (uint32_t)getCopReg(2,0xe);
    i = 0;
    wp = (short *)((intptr_t)prim + 0x1a);
    t = rec;
    do {
      *(uint32_t *)(wp + -9) = org;
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
      *prim = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))| 0x6000000;
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))= (uint32_t)guest_of(prim) & 0xffffff;
      prim = prim + 7;
    } while (i < 6);
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f80000c * 8 + rec + 0x150) ))= *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))| 0x1000000;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0xc) ))= (rec + _DAT_1f80000c * 8 + 0x150) & 0xffffff;
  }
  return;
}

/* firework_task_update @0x80140d38 — firework/particle task update (10 emitters). */
void firework_task_update(int rec)
{
  short dv;
  uint16_t life;
  int t;
  int prod;
  char *px;
  short *vel2;
  uint32_t *ot;
  short *pos;
  short *acc;
  uint32_t *src;
  short *vel;
  int k;

  t = _DAT_1f80000c * 400;
  if (_DAT_1f800082 == 0) {
    life = (uint16_t)(*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4) ))- 1);
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4) ))= life;
    acc = (short *)guest_at((uint32_t)(uintptr_t)((rec + 800)));
    if ((int)((uint32_t)life << 0x10) < 0) {
      task_slot_step();
      return;
    }
    vel = (short *)guest_at((uint32_t)(uintptr_t)((rec + 0x370)));
    k = 0;
    vel2 = (short *)guest_at((uint32_t)(uintptr_t)((rec + 0x374)));
    pos = (short *)guest_at((uint32_t)(uintptr_t)((rec + 0x324)));
    px = (char *)guest_at((uint32_t)(uintptr_t)((rec + t + 6)));
    t = rec;
    do {
      if (*(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4) ))< 10) {
        px[-2] = px[-2] + -0x20;
        *px = *px + -0x20;
        px[-1] = px[-1] + -0x20;
      }
      if (10 < *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3d4)))) {
        *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x3c0) ))= *(short *)guest_at((uint32_t)(uintptr_t)((t + 0x3c0) ))+ 4;
        prod = *vel * 0xf0a;
        if (prod < 0) {
          prod = prod + 0xfff;
        }
        *vel = (short)(prod >> 0xc);
        prod = vel2[-1] * 0xf0a;
        if (prod < 0) {
          prod = prod + 0xfff;
        }
        vel2[-1] = (short)(prod >> 0xc);
        prod = *vel2 * 0xf0a;
        if (prod < 0) {
          prod = prod + 0xfff;
        }
        *vel2 = (short)(prod >> 0xc);
      }
      px = px + 0x28;
      dv = *vel;
      vel = vel + 4;
      t = t + 2;
      k = k + 1;
      *acc = *acc + dv;
      acc = acc + 4;
      pos[-1] = pos[-1] + vel2[-1];
      dv = *vel2;
      vel2 = vel2 + 4;
      *pos = *pos + dv;
      pos = pos + 4;
    } while (k < 10);
  }
  src = (uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + 800)));
  t = 0;
  ot = (uint32_t *)guest_at((uint32_t)(uintptr_t)((rec + _DAT_1f80000c * 400)));
  acc = (short *)((intptr_t)ot + 0x22);
  do {
    setCopReg(2,0,*src);
    setCopReg(2,0x800,src[1]);
    copFunction(2,0x180001);
    _DAT_1f8003fc = (uint32_t)getCopReg(2,0xe);
    _DAT_1f8003f8 = (uint32_t)getCopReg(2,0x9800);
    _DAT_1f8003f8 = _DAT_1f8003f8 >> 2;
    dv = *(short *)guest_at((uint32_t)(uintptr_t)((rec + 0x3c0)));
    /* guest @0x80140F70-0x80140F7C: lhu @0x1f800024 then sll16/sra16 =
     * sext16 of the low halfword only — NOT the full u32 cell. */
    k = (int)(int16_t)(uint16_t)_DAT_1f800024;
    prod = _DAT_1f8003f8 << 2;
    acc[-0xd] = (short)_DAT_1f8003fc;
    acc[-0xc] = _DAT_1f8003fe;
    dv = (short)mips_div_lo((int32_t)(dv * k),prod); /* guest div @0x80140F94 */
    acc[-9] = (short)_DAT_1f8003fc + dv;
    acc[-8] = _DAT_1f8003fe;
    acc[-5] = (short)_DAT_1f8003fc;
    acc[-4] = _DAT_1f8003fe + dv;
    src = src + 2;
    acc[-1] = (short)_DAT_1f8003fc + dv;
    rec = rec + 2;
    *acc = _DAT_1f8003fe + dv;
    t = t + 1;
    acc = acc + 0x14;
    *ot = *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x10) ))| 0x9000000;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)((_DAT_1f800008 + 0x10) ))= guest_of(ot) & 0xffffff;
    ot = ot + 10;
  } while (t < 10);
  return;
}

/* motion_integrator_tail @0x80185c70 — motion integrator tail: rotate velocity by facing,
 * decay position deltas. Called by fighter_anim_advance when +0xc0==0. */
void motion_integrator_tail(int f,int n)
{
  short px;
  short pz;
  short pw;
  short py;
  int t1;
  uint16_t *wp;
  int ang;
  int t0;

  t0 = 1;
  wp = (uint16_t *)guest_at((uint32_t)(uintptr_t)((*(int *)guest_at((uint32_t)(uintptr_t)((f + 0x40) ))+ 0x24)));
  do {
    wp[-1] = (short)(((int)((uint32_t)(uint16_t)wp[-1] << 0x13) >> 0x14) << 1);
    if ((t0 != 1) && (t0 != 4)) {
      wp[2] = 0;
      wp[1] = 0;
      *wp = 0;
    }
    t0 = t0 + 1;
    wp = wp + 0xe;
  } while (t0 < 0x13);
  px = *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xdc)));
  ang = (int)*(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd0)));
  py = *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xde)));
  pz = *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xe0)));
  t0 = lut_cos_q12(ang);
  t1 = lut_sin_q12(ang);
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd6) ))= *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd6) ))+ (py >> 4);
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd4) ))=
       *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd4) ))+ (short)((uint32_t)(px * t0 - pz * t1) >> 0x10);
  t0 = lut_sin_q12(ang);
  t1 = lut_cos_q12(ang);
  py = *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xe4)));
  pw = *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd2)));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xdc) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xde) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xe0) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xe4) ))= 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)((n + 0xd2) ))= 0;
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd0) ))= (short)((ang + pw + (int)py) * 0x100000 >> 0x14);
  *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd8) ))=
       *(short *)guest_at((uint32_t)(uintptr_t)((n + 0xd8) ))+ (short)((uint32_t)(px * t0 + pz * t1) >> 0x10);
  return;
}

/* LAB_8013e558 @0x8013e558 — speech-bubble/voice-print task update (EV2/3 path).
 * Alternate entry: task callback registered via task_node_alloc. */
void LAB_8013e558(uint32_t *ctx)
{
  uint16_t sh;
  uint16_t *wp;
  int nd;
  short t;
  uint32_t rx;
  uint32_t ry;
  uint32_t rz;

  if (_DAT_1f800082 == 0) {
    t = *(short *)((intptr_t)ctx + 0x46) + 0x200;
    *(short *)((intptr_t)ctx + 0x46) = t;
    *(short *)(ctx + 0x11) = *(short *)(ctx + 0x11) + 0x80;
    if (0x1000 < t) {
      node_tree_free(*ctx);
      task_slot_step();
      _DAT_801f0930 = _DAT_801f0930 + -1;
      if (-1 < (int32_t)_DAT_801f0930) {
        return;
      }
      _DAT_801f0930 = 0;
      return;
    }
    tween_seek((int)*ctx,0);
  }
  sh = *(uint16_t *)(ctx + 0x11);
  *(uint16_t *)(ctx + 1) = sh;
  *(uint16_t *)(ctx + 3) = sh;
  *(uint16_t *)(ctx + 5) = sh;
  setCopControlWord(2,0,ctx[9]);
  setCopControlWord(2,0x800,ctx[10]);
  setCopControlWord(2,0x1000,ctx[0xb]);
  setCopControlWord(2,0x1800,ctx[0xc]);
  setCopControlWord(2,0x2000,ctx[0xd]);
  setCopReg(2,0x4800,(uint32_t)*(uint16_t *)(ctx + 1));
  setCopReg(2,0x5000,(uint32_t)*(uint16_t *)((intptr_t)ctx + 10));
  setCopReg(2,0x5800,(uint32_t)*(uint16_t *)(ctx + 4));
  copFunction(2,0x49e012);
  wp = (uint16_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(uintptr_t)(*ctx)));
  rx = (uint32_t)getCopReg(2,0x4800);
  ry = (uint32_t)getCopReg(2,0x5000);
  rz = (uint32_t)getCopReg(2,0x5800);
  *wp = (short)rx;
  wp[3] = (short)ry;
  wp[6] = (short)rz;
  setCopReg(2,0x4800,(uint32_t)*(uint16_t *)((intptr_t)ctx + 6));
  setCopReg(2,0x5000,(uint32_t)*(uint16_t *)(ctx + 3));
  setCopReg(2,0x5800,(uint32_t)*(uint16_t *)((intptr_t)ctx + 0x12));
  copFunction(2,0x49e012);
  nd = *(int *)guest_at((uint32_t)(uintptr_t)(*ctx));
  rx = (uint32_t)getCopReg(2,0x4800);
  ry = (uint32_t)getCopReg(2,0x5000);
  rz = (uint32_t)getCopReg(2,0x5800);
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 2) ))= (short)rx;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 8) ))= (short)ry;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 0xe) ))= (short)rz;
  setCopReg(2,0x4800,(uint32_t)*(uint16_t *)(ctx + 2));
  setCopReg(2,0x5000,(uint32_t)*(uint16_t *)((intptr_t)ctx + 0xe));
  setCopReg(2,0x5800,(uint32_t)*(uint16_t *)(ctx + 5));
  copFunction(2,0x49e012);
  nd = *(int *)guest_at((uint32_t)(uintptr_t)(*ctx));
  rx = (uint32_t)getCopReg(2,0x4800);
  ry = (uint32_t)getCopReg(2,0x5000);
  rz = (uint32_t)getCopReg(2,0x5800);
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 4) ))= (short)rx;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 10) ))= (short)ry;
  *(short *)guest_at((uint32_t)(uintptr_t)((nd + 0x10) ))= (short)rz;
  return;
}

/* LAB_80140510 @0x80140510 — hit-spark sequence player task update (EV22 path).
 * Walks {op,arg,jump} records; spawns impact bursts via impact_fx_spawn. */
void LAB_80140510(int *ctx)
{
  uint16_t w;
  uint32_t op;
  uint16_t *nxt;
  uint16_t *ip;
  uint32_t arg;
  short *rec;
  int i;

  if (_DAT_1f800082 == 0) {
    while( true ) {
      while( true ) {
        ip = (uint16_t *)guest_at((uint32_t)(uintptr_t)(ctx[1]));
        arg = *ip & 0xff;
        op = *ip & 0xff00;
        if (op != 0xfe00) break;
        nxt = (uint16_t *)guest_at((uint32_t)(uintptr_t)((*ctx + (short)ip[1] * 8)));
        ctx[1] = (int)guest_of(nxt);
        *(uint16_t *)(ctx + 3) = *nxt;
      }
      if (0xfe00 < op) break;
      if (op != 0xfc00) goto LAB_80140624;
      if ((int)arg < (int)*(short *)((intptr_t)ctx + 0xe)) {
        ctx[1] = (int)guest_of(ip + 4);
      }
      else {
        nxt = (uint16_t *)guest_at((uint32_t)(uintptr_t)((*ctx + (short)ip[1] * 8)));
        ctx[1] = (int)guest_of(nxt);
        w = *nxt;
        *(short *)((intptr_t)ctx + 0xe) = *(short *)((intptr_t)ctx + 0xe) + 1;
        *(uint16_t *)(ctx + 3) = w;
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
        op = (uint32_t)rec[2];
        if ((short)ctx[4] != 0) {
          op = (uint32_t)g_DAT_801b180c[op];
        }
        i = 0;
        if (0 < rec[1]) {
          do {
            if (*(short *)guest_at((uint32_t)(uintptr_t)((ctx[1] + 6) ))== 0) {
              impact_fx_spawn((uint32_t *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)((ctx[2] + 0x20) ))+ op * 0x50 + 0x34)),0x80000003,
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
#define BA8(a)   (*(uint8_t  *)guest_at(a))
#define BA16(a)  (*(uint16_t *)guest_at(a))
#define BA32(a)  (*(uint32_t *)guest_at(a))
#define BA16S(a) (*(int16_t  *)guest_at(a))
extern char *s_Print_mode_error_8011f140;
extern void text_win_init(uint32_t w,int32_t h);
extern int32_t text_widget_alloc(uint16_t x,uint16_t y,int32_t attr,uint16_t aux,int32_t mode,int32_t count);
extern void text_line_select(int32_t line);
extern void printf_forward(const char *fmt, ...);
extern void bitfield_or_write(void *p, uint32_t val);
extern void spr_desc_a(uint32_t slot_g);
extern void spr_desc_b(uint32_t slot_g,uint32_t on);
extern void spr_desc_c(uint32_t slot_g,uint32_t on);
extern uint32_t cd_cmd_retry(uintptr_t buf);

/* announce_param_block_init @0x8012e58c — round/announce parameter block init @0x1f80038c.
 * All fields u16 (sh) except +0x1c u32 (sw); verified vs disasm. */
void announce_param_block_init(uint32_t tbl,uint32_t a1,const void *a2,
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
  text_render_full((uint32_t *)guest_at(tbl), a1 & 0xffff,
               0x1f80038c);
}

/* print_mode_dispatch_b @0x8012cbc0 — print_mode_dispatch — print-mode dispatch (mode 1 = text window). */
void print_mode_dispatch_b(int32_t mode,uint32_t a1,uint32_t a2)
{
  BA32(0x1f800384) = (uint32_t)mode;
  if (mode == 1) {
    uint32_t widget;
    text_win_init(0x3c0,0);
    widget = text_widget_alloc(a1,a2,0x140,0xf0,0,0x200);
    text_line_select(widget);
    return;
  }
  if (mode < 2) {
    if (mode == 0) return;
  } else if (mode == 2) {
    return;
  }
  printf_forward(s_Print_mode_error_8011f140);
}

/* scroll_offset_accumulate @0x8012cdd8 — scroll offset accumulate (u16 pair @0x1f80037c). */
void scroll_offset_accumulate(int16_t x,int16_t y)
{
  if (BA8(0x1f800015) != 0) y = (int16_t)(y << 1);
  BA16(0x1f80037c) = (uint16_t)(BA16(0x1f80037c) + (uint16_t)x);
  BA16(0x1f80037e) = (uint16_t)(BA16(0x1f80037e) + (uint16_t)y);
}

/* scroll_offset_set @0x8012cd9c — scroll offset set (-1 = keep). */
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

/* rgb_triple_store @0x801274fc — RGB triple store (two mirrored triples). */
void rgb_triple_store(uint8_t r,uint8_t g,uint8_t b)
{
  BA8(0x801e40b9) = r; BA8(0x801e40ba) = g; BA8(0x801e40bb) = b;
  BA8(0x801e6209) = r; BA8(0x801e620a) = g; BA8(0x801e620b) = b;
}

/* brightness_packet_send @0x80156114 — brightness/gamma level -> 4-uint8_t packet to cd_cmd_retry.
 * DAT_801bef70 = s16 level; DAT_801f36c0 u16 flag selects direct vs
 * scaled (x*63/127) encoding. Verified vs disasm incl. stack buf. */
void brightness_packet_send(int32_t val)
{
  /* Generated 0x80156114: addiu sp,sp,-32; the 4-uint8_t packet lives at
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

/* hud_sprite_desc_init @0x80136ddc — HUD/sprite descriptor block init @0x801f0660.
 * Four interleaved field clusters inside a 0x68-uint8_t block, then a
 * 0x68-uint8_t mirror copy to +0x68. Faithful to disasm (sb/sh/sw widths verified). */
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

/* sprite_desc_table_init @0x8012d090 — 800-slot sprite descriptor table init @0x801e8528.
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

/* announce_param_block_init_alt @0x8012e6a8 — variant of announce_param_block_init: same param block @0x1f80038c
 * but field +0xc preset to 0x1e (vs 0xf). Disasm-verified widths. */
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
  text_render_full((uint32_t *)guest_at(tbl), a1 & 0xffff,
               0x1f80038c);
}

/* hud_time_score_draw — battle HUD time/score draw dispatcher. Formats the
 * frame timer as HH:MM:SS-ish "%02u:%02u:%02u" then issues text-draw
 * calls (announce_param_block_init_b/2e184/2e61c/2e73c variants) per layout mode
 * arg6. arg5 = score/time record ptr; a2 = layout base;
 * a3 = color/attr; tbl/2 = draw env pair. */
extern void hud_text_a(uint32_t ot_ptr,uint16_t flags,const void *str,uint16_t attr_a,uint16_t attr_b,short tile_row);
extern void hud_text_b(uint32_t ot_ptr,uint32_t flags,const void *str,uint32_t attr,uint16_t attr_b,short tile_row);
extern void hud_text_c(uint32_t ot_ptr,uint32_t flags,const void *str,uint32_t attr,uint16_t attr_b,uint32_t tile_row);
extern void print_dispatch(const char *fmt,...);
extern int slot_name_measure(void *a0,int32_t a1);
/* hud_time_score_draw @0x80163e38 — draw the time/score readout: split the
 * frame counter into mm:ss:ff (clamped), sprintf_core into a reserved
 * guest-frame buffer, then render via the hud_text/announce helpers. */
void hud_time_score_draw(uint32_t tbl,uint16_t font,int32_t a2,int32_t clr,
                  uint8_t *rec,int32_t mode)
{
  int32_t s1, s2, ff, t5, t6, fc;
  int16_t col;
  uint32_t fl;
  /* line = sp+0x18 (frame 0x60) -> guest_sp()-0x48;
   * `addiu $tbl,$sp,0x18` before sprintf jal in 0x80163e38. The string is
   * re-read by announce_param_block_init_b / hud_text_* / slot_name_measure
   * across many nested calls, so reserve the guest frame window
   * (sp+0x18 -> frm+0x18) or callee frames overwrite it. */
  uint32_t htsd_frm = br2_guest_frame(0x60);
  char *line = (char *)guest_at(htsd_frm + 0x18);
  t5 = *(int32_t *)(rec + 8) / 0x32;
  t6 = t5 / 0x3c;
  t5 = t5 % 0x3c;
  ff = (*(int32_t *)(rec + 8) % 0x32) * 2;
  if (99 < t6) { ff = 99; t6 = 99; t5 = 0x3b; }
  fc = ff;
  sprintf_core(line,(char *)guest_at(0x80121fa8),t6,t5,ff);
  s2 = ((int32_t)*(int16_t *)(rec + 0xc) % 1000) * 0x10000 >> 0x10;
  s1 = ((int32_t)*(int16_t *)(rec + 0xe) % 1000) * 0x10000 >> 0x10;
  col = (int16_t)clr;
  switch (mode) {
  case 0:
    t5 = (int32_t)col;
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + 0x10) * 0x10000 >> 0x10),(uint16_t)t5,1);
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + 0xa0) * 0x10000 >> 0x10),(uint16_t)t5,1);
    announce_param_block_init_b(tbl,font,rec,(uint16_t)((a2 + 0x130) * 0x10000 >> 0x10),(uint16_t)t5,4);
    break;
  case 1:
    sprintf_core(line,(char *)guest_at(0x80121fb8),t6,t5,ff);
    print_dispatch((const char *)guest_at(0x80121fc8),(const char *)rec,line);
    goto LAB_80164124;
  case 2:
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + 0x10) * 0x10000 >> 0x10),(uint16_t)col,1);
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + 0xa0) * 0x10000 >> 0x10),(uint16_t)col,1);
    break;
  case 3:
    print_dispatch((const char *)guest_at(0x80121fd4),(const char *)rec,s2,s1);
LAB_80164124:
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    print_dispatch(line);
    break;
  case 4:
    t5 = sprintf_core(line,(char *)guest_at(0x80121fe0),s2,s1,fc);
    t6 = (int32_t)col;
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + (3 - t5) * 0x10) * 0x10000 >> 0x10),(uint16_t)t6,4);
    announce_param_block_init_b(tbl,font,guest_at(0x80121fe4),(uint16_t)((a2 + 0x30) * 0x10000 >> 0x10),(uint16_t)t6,1);
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + 0xa0) * 0x10000 >> 0x10),(uint16_t)t6,1);
    announce_param_block_init_b(tbl,font,rec,(uint16_t)((a2 + 0x130) * 0x10000 >> 0x10),(uint16_t)t6,4);
    break;
  case 5:
    t5 = sprintf_core(line,(char *)guest_at(0x80121fe0),s1,s1,fc);
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + (3 - t5) * 0x10) * 0x10000 >> 0x10),(uint16_t)col,4);
    t5 = (clr + 10) * 0x10000 >> 0x10;
    hud_text_a(tbl,font,guest_at(0x80121fe4),(uint16_t)((a2 + 0x30) * 0x10000 >> 0x10),(uint16_t)t5,1);
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    hud_text_a(tbl,font,line,(uint16_t)((a2 + 0x74) * 0x10000 >> 0x10),(uint16_t)t5,1);
    announce_param_block_init_b(tbl,font,rec,(uint16_t)((a2 + 0xb4) * 0x10000 >> 0x10),(uint16_t)col,4);
    break;
  case 6:
    t5 = sprintf_core(line,(char *)guest_at(0x80121fe0),s1,s1,fc);
    announce_param_block_init_b(tbl,font,line,(uint16_t)((a2 + (3 - t5) * 0x10) * 0x10000 >> 0x10),(uint16_t)col,4);
    t5 = (clr + 10) * 0x10000 >> 0x10;
    hud_text_a(tbl,font,guest_at(0x80121fe4),(uint16_t)((a2 + 0x30) * 0x10000 >> 0x10),(uint16_t)t5,1);
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    hud_text_a(tbl,font,line,(uint16_t)((a2 + 0x74) * 0x10000 >> 0x10),(uint16_t)t5,1);
    break;
  case 0x12:
    hud_text_b(tbl,font,line,a2 + 0x10,(uint16_t)clr,5);
    goto LAB_801645f8;
  case 0x13:
    if ((_DAT_1f8000dc & 4) == 0) fl = 0; else fl = 5;
    hud_text_c(tbl,font,line,a2 + 0x10,(uint16_t)clr,fl);
    goto LAB_80164538;
  case 0x14:
    s2 = s1;
    goto LAB_8016457c;
  case 0x15:
    t5 = sprintf_core(line,(char *)guest_at(0x80121fe0),s1,s1,fc);
    if ((_DAT_1f8000dc & 4) == 0) {
      hud_text_c(tbl,font,line,a2 + (3 - t5) * 0x10 + 0x10,(uint16_t)clr,0);
      fl = 0;
    } else {
      hud_text_c(tbl,font,line,a2 + (3 - t5) * 0x10 + 0x10,(uint16_t)clr,5);
      fl = 5;
    }
    announce_param_block_init_alt(tbl,font,guest_at(0x80121fe4),(uint16_t)(a2 + 0x40),(uint16_t)clr,fl);
LAB_80164538:
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    announce_param_block_init_alt(tbl,font,line,(uint16_t)(a2 + 0xf8),(uint16_t)clr,2);
    break;
  case 0x16:
LAB_8016457c:
    t5 = sprintf_core(line,(char *)guest_at(0x80121fe0),s2,s1,fc);
    hud_text_b(tbl,font,line,a2 + (3 - t5) * 0x10 + 0x10,(uint16_t)clr,5);
    announce_param_block_init(tbl,font,guest_at(0x80121fe4),(uint16_t)((a2 + 0x40) * 0x10000 >> 0x10),(uint16_t)col,5);
LAB_801645f8:
    announce_param_block_init(tbl,font,rec,(uint16_t)((a2 + 0xa8) * 0x10000 >> 0x10),(uint16_t)col,0);
    slot_name_measure(line,*(uint8_t *)(rec + 4));
    announce_param_block_init(tbl,font,line,(uint16_t)((a2 + 0xf8) * 0x10000 >> 0x10),(uint16_t)col,2);
  }
  br2_guest_frame_free();
}

extern void flow_cfg_init(uint8_t a0);
extern void battle_struct_init(uint32_t battle_g,uint32_t env_g);
extern void record_copy_1a(int idx);
extern int per_side_setup(int side,int flag);
extern void sprite_anim_chain_build(uint32_t a0);
extern void clear_f0300_records(void);

/* rec_copy_init — if uint8_t @0x1f800099 == 0: copy 0x44-uint8_t record
 * 0x8011f484 + idx*0x44 -> 0x1f8002f8, init record @0x801f0228
 * (+0=src guest addr, +4 u16=0, +6/+8/+0xa u16=1, +0x14 u16=0).
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

/* fighter_reinit_tail — fighter re-init tail: gate on uint8_t @0x801e3d00 >= 0,
 * then battle_camera_step/fighter_tint_fade/fighter_trail_emit/slot_height_proj chain.
 * Disasm 0x80166f64-0x80167004. */
int fighter_reinit_tail(uint32_t f)
{
  uint8_t *s0 = (uint8_t *)guest_at(f);
  if (*(int8_t *)guest_at(0x801e3d00) < 0) return -1;
  *(uint8_t *)(s0 + 0x9d) = 0;
  battle_camera_step(*(uint32_t *)(s0 + 0xbc),(uint8_t *)guest_at(0x1f8000e8),(uint8_t *)guest_at(0x1f8001f0),(int)guest_of(s0));
  fighter_tint_fade(*(uint32_t *)(s0 + 8),*(uint32_t *)(s0 + 0xc),(int)guest_of(s0),(int)(int8_t)*(uint8_t *)(s0 + 0x98));
  fighter_trail_emit((uint32_t *)(uintptr_t)(s0 + 0x28),0x1f80033c,*(int *)(s0 + 0xbc),(int)guest_of(s0));
  slot_height_proj(*(uint32_t *)(s0 + 8) + 0x207c,*(uint32_t *)(s0 + 0xbc),*(uint32_t *)(s0 + 0xc),(int)guest_of(s0));
  return 0;
}

/* fighter_respawn_init — fighter respawn/init: gate @0x801e3d00, save/restore
 * uint8_t +0x99, flow_cfg_init(0), battle_struct_init, zero +9d/+9e/+99,
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
  uint16_t arg;
  uint32_t op;
  uint16_t *ip;
  uint32_t subop;

  if (_DAT_1f800082 == 0) {
    ctx[5] = ctx[5] + -1;
  }
  if (ctx[5] < 1) {
    ctx[4] = ctx[4] + 4;
    w = *(short *)guest_at((uint32_t)ctx[4]);
    while (((int)w & 0xff00U) != 0) {
      ip = (uint16_t *)guest_at((uint32_t)ctx[4]);
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
          arg = *(uint16_t *)guest_at((uint32_t)(ctx[4] + 2));
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
    *(uint16_t *)guest_at((uint32_t)ctx[0]) =
        *(uint16_t *)guest_at((uint32_t)(*(short *)guest_at((uint32_t)ctx[4]) * 2 + ctx[3]));
    *(uint16_t *)guest_at((uint32_t)ctx[1]) =
        *(uint16_t *)guest_at((uint32_t)(*(short *)guest_at((uint32_t)ctx[4]) * 2 + ctx[3]));
    ctx[5] = (int)*(short *)guest_at((uint32_t)(ctx[4] + 2));
  }
  return;
}

/* LAB_80137acc — channel command interpreter task. Guest 0x80137acc.
 * [2] script cursor (guest addr), [3] handler table base (guest addr),
 * [1] record table base (guest addr); halfword +0x10 current op,
 * halfword +0x12 repeat index. */
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
        *(uint16_t *)((intptr_t)ctx + 0x12) = 0;
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
                  /* Guest jalr passes a0 = ctx[0] (guest addr of the
                   * channel state).  The clean handlers (LAB_80137590 /
                   * LAB_80137680 / LAB_80137acc / LAB_80137cd0 family) take
                   * host pointers into guest RAM, so resolve via guest_at;
                   * the generated fallback below gets the raw guest addr. */
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
    *(uint16_t *)((intptr_t)ctx + 0x12) = 0;
    *opp = op;
  } while( true );
}

/* LAB_80137cd0 — channel-script cursor task (variant B). Guest 0x80137cd0.
 * [1] kill-notify ptr, [2] u16 table base, [3] jump-table base,
 * [4] script cursor; halfword +0x14 countdown, halfword +0x16 repeat index. */
void LAB_80137cd0(int *ctx)
{
  short w;
  uint32_t operand;
  short *ops;
  uint32_t op;
  uint16_t *ip;

  if (_DAT_1f800082 == 0) {
    *(short *)(ctx + 5) = *(short *)(ctx + 5) + -1;
  }
  if (*(short *)(ctx + 5) < 1) {
    ctx[4] = ctx[4] + 4;
    w = *(short *)guest_at((uint32_t)ctx[4]);
    while (((int)w & 0xff00U) != 0) {
      ip = (uint16_t *)guest_at((uint32_t)ctx[4]);
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
            *(uint32_t *)guest_at((uint32_t)ctx[1]) = 0;
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
    *(uint16_t *)(ctx + 5) = *(uint16_t *)guest_at((uint32_t)(ctx[4] + 2));
  }
  return;
}

/* LAB_8013eb04 — speech/effect emitter task (spawns 0x8013e72c subtasks).
 * Guest 0x8013eb04. Data: [4] flag, [5] record-table base, [6] cursor,
 * halfword +0x1c index, u16 +0x1e countdown; [2]/[3] receive RTPS outputs. */
void LAB_8013eb04(int *ctx)
{
  uint8_t pb;
  uint16_t tp;
  int t;
  uint32_t *cell;
  uint8_t *pat;
  int i;
  uint32_t *pay;
  uint32_t *rowp;
  int off;
  int row;
  uint32_t d0;
  uint32_t d1;
  uint32_t d2;
  uint16_t life;

  if (_DAT_1f800082 == 0) {
    if (*(short *)guest_at((uint32_t)ctx[6]) < 0) {
      life = *(uint16_t *)((intptr_t)ctx + 0x1e) - 1;
      *(uint16_t *)((intptr_t)ctx + 0x1e) = life;
      if ((int)((uint32_t)life << 0x10) < 0) {
        task_slot_step();
        return;
      }
    }
    else {
      if (*(short *)(ctx + 7) == *(short *)guest_at((uint32_t)ctx[6])) {
        do {
          t = (int)(intptr_t)task_node_alloc(1,2,0xe,0x8013e72c,0,0x200);
          if (t != 0) {
            row = 0;
            off = 0;
            pay = (uint32_t *)guest_at(*(uint32_t *)guest_at((uint32_t)(t + 0x10)));
            t = *(short *)guest_at((uint32_t)(ctx[6] + 2)) * 0x12 + ctx[5];
            d0 = *(uint32_t *)guest_at((uint32_t)(t + -0xe));
            d1 = *(uint32_t *)guest_at((uint32_t)(t + -10));
            d2 = *(uint32_t *)guest_at((uint32_t)(t + -6));
            pay[0x78] = *(uint32_t *)guest_at((uint32_t)(t + -0x12));
            pay[0x79] = d0;
            pay[0x7a] = d1;
            pay[0x7b] = d2;
            *(uint16_t *)((intptr_t)pay + 0x1f0) = *(uint16_t *)guest_at((uint32_t)(t + -2));
            pay[0x7d] = (int)guest_of((char *)ctx + 8);
            pay[0x7e] = (int)guest_of((char *)ctx + 0xc);
            rowp = pay;
            do {
              i = 0;
              cell = rowp;
              pat = (uint8_t *)guest_at(0x801b1ec4);
              t = off;
              do {
                *cell = 0x9000000;
                *(uint8_t *)((intptr_t)cell + 7) = 0x2e;
                *(uint16_t *)((intptr_t)pay + t + 0x16) = 0x39;
                tp = 0x7a5a;
                if (ctx[4] == 0) {
                  tp = 0x7a9a;
                }
                *(uint16_t *)((intptr_t)pay + t + 0xe) = tp;
                *(uint8_t *)(cell + 1) = 0xe0;
                *(uint8_t *)((intptr_t)cell + 5) = 0xe0;
                *(uint8_t *)((intptr_t)cell + 6) = 0xe0;
                pb = pat[1];
                *(uint8_t *)(cell + 3) = *pat;
                *(uint8_t *)((intptr_t)cell + 0xd) = pb;
                pb = pat[3];
                *(uint8_t *)(cell + 5) = pat[2];
                *(uint8_t *)((intptr_t)cell + 0x15) = pb;
                pb = pat[5];
                *(uint8_t *)(cell + 7) = pat[4];
                *(uint8_t *)((intptr_t)cell + 0x1d) = pb;
                pb = pat[7];
                *(uint8_t *)(cell + 9) = pat[6];
                *(uint8_t *)((intptr_t)cell + 0x25) = pb;
                pat = pat + 8;
                cell = cell + 10;
                i = i + 1;
                t = t + 0x28;
              } while (i < 6);
              rowp = rowp + 0x3c;
              row = row + 1;
              off = off + 0xf0;
            } while (row < 2);
            pay[0x7f] = 0;
            if (*(short *)((intptr_t)ctx + 0x1e) <= *(short *)((intptr_t)pay + 0x1e0)) {
              *(short *)((intptr_t)ctx + 0x1e) = *(short *)((intptr_t)pay + 0x1e0) + 1;
            }
          }
          t = ctx[6];
          ctx[6] = t + 4;
        } while (*(short *)(ctx + 7) == *(short *)guest_at((uint32_t)(t + 4)));
      }
      *(short *)(ctx + 7) = *(short *)(ctx + 7) + 1;
    }
  }
  /* GTE tail: lwc2 r0/r1 <- param[0..1], cop2 op, swc2 r14 -> param[2],
   * mfc2 r19 -> param[3] >> 2 (disasm 0x8013ed50-0x8013ed7c). */
  setCopReg(2,0,(uint32_t)ctx[0]);
  setCopReg(2,1,(uint32_t)ctx[1]);
  copFunction(2,0x180001);
  ctx[2] = (int)getCopReg(2,14);
  ctx[3] = getCopReg(2,0x9800) >> 2;
  return;
}

/* ======== batch29: small task/effect wrappers + list walkers ===== */
/* externs for batch29 callees */
extern int  cd_dispatch_tbl4(int a0,uint32_t a1);
extern int32_t mdec_stat_wait_a(void);
extern int32_t mdec_stat_wait_b(void);
extern uint32_t mdec_status_read(void);
extern void gpu_load_image_cmd(uint32_t rect,uint32_t src);
extern uint32_t gpu_rect(const void *p);

/* guest file_op_dispatch @0x8018fac8 — file-op dispatch wrapper: cd_dispatch_tbl4(0,arg). */
int file_op_dispatch(int op)
{
  return cd_dispatch_tbl4(0,(uint32_t)op);
}

/* guest channel_script_emit @0x8013514c — channel-script emit wrapper. */
void channel_script_emit(int ctx,uint32_t arg,int rec)
{
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8) )+ 0xf0,ctx + 0x2074,arg,3);
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
void channel_script_emit_pair(int ctx,uint32_t arg,int rec)
{
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),ctx + 0x2048,arg,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,ctx + 0x2074,arg,4);
  return;
}

/* guest channel_script_emit_pair_gated @0x801350dc — emit pair; first gated on h(c)>4 (count-4). */
void channel_script_emit_pair_gated(int ctx,uint32_t arg,int rec)
{
  if (4 < *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0xc))) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,ctx + 0x2074,arg,
                 (int)*(short *)guest_at((uint32_t)(uintptr_t)(rec + 0xc)) - 4);
  }
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),ctx + 0x2074,arg,1);
  return;
}

/* guest channel_script_emit_pair_2gate @0x80134128 — emit pair; count-4 gated + h4-count gated. */
void channel_script_emit_pair_2gate(int ctx,uint32_t arg,int rec)
{
  int cnt;
  cnt = (int)*(short *)guest_at((uint32_t)(uintptr_t)(rec + 0xc));
  if (cnt - 4 > 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,ctx + 0x2074,arg,cnt - 4);
  }
  cnt = (int)*(short *)guest_at((uint32_t)(uintptr_t)(rec + 4));
  if (cnt != 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),ctx + 0x2074,arg,cnt);
  }
  return;
}

/* guest channel_script_emit_triple @0x801341a0 — emit triple (2048 x2, 2074 gated on c-5). */
void channel_script_emit_triple(int ctx,uint32_t arg,int rec)
{
  int tmp;
  tmp = ctx + 0x2048;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),tmp,arg,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,tmp,arg,1);
  tmp = (int)*(short *)guest_at((uint32_t)(uintptr_t)(rec + 0xc)) - 5;
  if (tmp > 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xc8,ctx + 0x2074,arg,tmp);
  }
  return;
}

/* guest channel_script_emit_slot_cycle @0x80134c30 — emit the channel
 * tasks, advance the slot counter, then commit the matching sprite packet. */
int32_t channel_script_emit_slot_cycle(uint32_t ctx,uint32_t arg,uint32_t rec)
{
  uint32_t frame = br2_guest_frame(48);
  uint32_t tasks = *(uint32_t *)guest_at(rec + 8);
  uint32_t phase;
  uint32_t stack_fifth;
  uint32_t stack_sixth;
  int32_t result;

  prim_task_run((int)*(uint32_t *)guest_at(rec), ctx + 0x2048u, arg, 1);
  prim_task_run((int)(tasks + 0xc8u), ctx + 0x2048u, arg, 1);
  prim_task_run((int)(tasks + 0xf0u), ctx + 0x2074u, arg, 1);
  prim_task_run((int)(tasks + 0xa0u), ctx + 0x2074u, arg, 1);

  phase = (uint32_t)(*(int16_t *)guest_at(rec + 0x28u) + 20) & 0xffffu;
  *(uint16_t *)guest_at(rec + 0x28u) = (uint16_t)phase;
  phase &= 0x700u;
  /* The 0x300/0x700 arms skip the guest's store to [sp+16]. */
  stack_fifth = *(uint32_t *)guest_at(frame + 16u);
  if (phase != 0x300u && phase != 0x700u) {
    stack_fifth = 272u;
    *(uint32_t *)guest_at(frame + 16u) = stack_fifth;
  }
  stack_sixth = 480u + ((phase >> 8) & 3u);
  *(uint32_t *)guest_at(frame + 20u) = stack_sixth;
  result = sprite_slot_commit(272u, phase < 0x400u ? 484u : 485u,
                              16u, 1u, stack_fifth, stack_sixth);
  br2_guest_frame_free();
  return result;
}

/* guest channel_script_emit_chain @0x80134e44 — emit chain; h26 normalized to boolean and
 * selects between (+0xf0,+0x140 / +0x118) tails before the +0xa0 emit. */
void channel_script_emit_chain(int ctx,uint32_t arg,int rec)
{
  int flag;
  int dst;
  dst = ctx + 0x2074;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),dst,arg,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xc8,dst,arg,1);
  flag = (int)*(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x26));
  flag = (flag == 0) ? 1 : 0;
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x26)) = (short)flag;
  if (flag != 0) {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xf0,dst,arg,1);
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x140,dst,arg,2);
  }
  else {
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x118,dst,arg,3);
  }
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,ctx + 0x2074,arg,1);
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
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)(dst + 0x34)) = 0;
  return;
}

/* guest task_node_alloc_init @0x8013f714 — alloc/init task node via task_node_alloc,
 * callback 0x8013f648. True entry is +8 vs listed 0x8013f71c (DAT_1f8000b1
 * flag load precedes the frame). Returns node ptr or the flag uint8_t. */
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
  t = (uint32_t)*(uint8_t *)guest_at((uint32_t)(uintptr_t)(ent + 2));
  t = (int)*(uint8_t *)guest_at((uint32_t)(uintptr_t)(t + 0x801f0ad8));
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
                (uint32_t)br2_gaddr(&DAT_801f0760));
  task_slot_step();
  return;
}


/* ======== batch30 (rebuilt): emit chains + save/record iterators ===== */
/* externs for batch30 callees */
extern int  cd_search_file(uint32_t *path,uint32_t arg);  /* guest 0x80197b30 — battle.c */
extern int cd_file_rec_find(uint32_t a0,uint32_t a1);  /* cd.c 0x801980ec */
extern int cd_strncmp12_eq(uint32_t a0,uint32_t a1);  /* cd.c 0x80197e08 */
extern uint32_t cd_dir_scan(uint32_t a0,uint32_t a1);  /* cd.c 0x80197e28 */
extern uint32_t cd_mode_flag_set(int8_t opcode,int param_addr,uint32_t param_val);
extern int  cd_timer_read(int mode);
extern void part_matrix_compute(uint32_t *mat,uint8_t *parts,short *euler,uint32_t angle);

/* guest channel_script_emit_chain_tick @0x80134b34 — emit chain on 203c/2078/2074 bases;
 * h26 counter +1 (h28 +=0x400 every 4th), de6c+86f5c on s1+0xc4. */
void channel_script_emit_chain_tick(int ctx,int arg,int rec,int xf)
{
  int tmp;
  tmp = ctx + 0x203c;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x140,tmp,arg,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)),tmp,arg,4);
  tmp = ctx + 0x2078;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xc8,tmp,arg,3);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),tmp,arg,1);
  tmp = (int)*(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x26)) + 1;
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x26)) = (short)tmp;
  if ((tmp & 3) == 0) {
    *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x28)) =
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x28)) + 0x400);
  }
  rotmat_rtps_build((uint32_t)(int)*(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x28)),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0xc4)));
  gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)xf),(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0xc4)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,ctx + 0x2074,arg,1);
  return;
}

/* guest packet_timer_update @0x8018499c — packet/timer update on s0: h3c countdown
 * (reload high uint8_t of *h24 on 0), advance h24 by 4 on last tick, then when
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
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)))) >> 8);
  }
  t = (int)*(uint16_t *)guest_at((uint32_t)(uintptr_t)(p + 0x3c)) - 1;
  *(short *)guest_at((uint32_t)(uintptr_t)(p + 0x3c)) = (short)t;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)(p + 6)) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)) + 2));
  if (t == 0) {
    *(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)) =
      *(int *)guest_at((uint32_t)(uintptr_t)(p + 0x24)) + 4;
  }
  if ((*(short *)guest_at((uint32_t)(uintptr_t)(p + 0x6e)) != 0) &&
      ((*(int *)guest_at((uint32_t)(uintptr_t)(p + 0x78)) - 0x7c - p) < 0xbf)) {
    w = (int)*(uint16_t *)guest_at((uint32_t)(uintptr_t)(p + 6));
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
  sel = (*(uint8_t *)guest_at((uint32_t)(uintptr_t)(f + 5)) & 1) * 0x1c;
  ent = *(int *)guest_at((uint32_t)(uintptr_t)(sel +
          *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(f + 0x38)) + 0x10))));
  ent = ent + *(int *)guest_at((uint32_t)(uintptr_t)(f + 0x30));
  ent = (int)*(short *)guest_at((uint32_t)(uintptr_t)(ent + idx * 2)) * 0x1c +
          *(int *)guest_at((uint32_t)(uintptr_t)(f + 0x40));
  mb = *(int *)guest_at((uint32_t)(uintptr_t)(node + 0x40));
  FUN_80194660((uint16_t *)guest_at((uint32_t)(uintptr_t)ent),(uint16_t *)guest_at((uint32_t)(uintptr_t)out));
  *(short *)guest_at((uint32_t)(uintptr_t)(out + 2)) =
    (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 2)) + 0x1000);
  half = (short)*(short *)guest_at((uint32_t)(uintptr_t)(ent + 6)) / 2;
  *(short *)guest_at((uint32_t)(uintptr_t)(out + 6)) = (short)half;
  part_matrix_compute((uint32_t *)guest_at((uint32_t)(uintptr_t)(node + 4)),
               (uint8_t *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(f + 0x48)) + idx * 0x20)),
               (short *)guest_at((uint32_t)(uintptr_t)out),half / 2);
  ent = *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)mb + 0x40))) + 0x20;
  rtps_point_xform2((uint32_t *)guest_at((uint32_t)(uintptr_t)ent),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(node + 0x18)),
               (int *)guest_at((uint32_t)(uintptr_t)(node + 0x34)));
  gte_rtps_xform3((uint32_t *)guest_at((uint32_t)(uintptr_t)ent),
               (uint16_t *)guest_at((uint32_t)(uintptr_t)(node + 4)),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(node + 0x20)));
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
  uint16_t *cs;
  uint16_t *lut;
  uint16_t vec [4];
  uint32_t rot [6];
  t = table_byte_next_a();
  cs = (uint16_t *)guest_at(0x801f5958);
  if ((t & 1) != 0) {
    cs[6] = (uint16_t)(cs[6] + 0x40);
  }
  else {
    cs[6] = (uint16_t)(cs[6] - 0x40);
  }
  if ((t & 2) != 0) {
    cs[7] = (uint16_t)(cs[7] + 0x80);
  }
  else {
    cs[7] = (uint16_t)(cs[7] - 0x80);
  }
  t = lut_sin_q12((int)(short)cs[6]);
  lut = (uint16_t *)guest_at(0x801237e8 + (((cam << 0x10) >> 0xe)));
  sy = lut_sin_q12((int)(short)cs[7]);
  cs[4] = (uint16_t)(lut[0] + ((t << 9) >> 0xc));
  dy = (int)*(short *)guest_at((uint32_t)(uintptr_t)(0x801237e8 + (((cam << 0x10) >> 0xe)) + 2)) * sy;
  vec[0] = 0;
  vec[1] = 0;
  t = (uint16_t)(lut[1] + (dy >> 0xc));
  vec[2] = (uint16_t)t;
  vec[3] = 0;
  cs[5] = (uint16_t)t;
  rotmat_rtps_build((uint32_t)(int)(short)cs[4],rot);
  gte_rtps_xform1((uint32_t *)rot,(uint32_t *)vec,(uint16_t *)cs);
  return;
}

/* guest record_init_sweep @0x8018d8e4 — pad/record init: retry cd_mode_flag_set(0xe)
 * until success, cd_timer_read(3) once via 0x801c4580 flag, reads count
 * @0x801b66dc (0 -> 0x169 sweep of 0x801b5b94 8-uint8_t records), per record
 * cd_search_file decode then writes *slot = ptr and rec+4 = (size+0x7ff)>>0xb. */
void pad_record_init(void)
{
  int t;
  int off;
  int md;
  int tb;
  int ent;
  int nd;
  /* guest frame 0x50: param block @sp+0x28 ({0x80} mode uint8_t read by the
   * CD BIOS via param_addr), cd_search_file's 0x18-uint8_t record lands at
   * sp+0x10. Both must live in guest RAM — host stack pointers would be
   * dereferenced as guest addresses by the delegated generated issue path. */
  uint32_t pri_frm = br2_guest_frame(0x50);
  uint8_t *cdbuf = (uint8_t *)guest_at(pri_frm + 0x28);
  uint32_t *sent = (uint32_t *)guest_at(pri_frm + 0x10);
  cdbuf[0] = 0x80;
  do {
    t = (int)cd_mode_flag_set(0xe,(int)(pri_frm + 0x28),0);
  } while (t == 0);
  if (*(uint8_t *)guest_at(0x801c4580) == 0) {
    cd_timer_read(3);
  }
  *(uint8_t *)guest_at(0x801c4580) = 1;
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
      if ((*(int *)guest_at((uint32_t)(uintptr_t)nd) != 0) && (cd_search_file(sent,nd) != 0)) {
        *(uint32_t *)guest_at((uint32_t)(uintptr_t)nd) = sent[0];
        *(int *)guest_at((uint32_t)(uintptr_t)(ent + 4)) =
          (int)(((uint32_t)sent[1] + 0x7ff) >> 0xb);
      }
      t = t - 1;
      off = off + 2;
      tb = tb + 1;
    } while (0 < t);
  }
  br2_guest_frame_free();
  return;
}

/* guest cd_search_file @0x80197b30 — CdSearchFile: resolve '\'-separated
 * path a1 (guest str, must start with 0x5c) through the CD directory
 * tables. Each token before the last is looked up via cd_file_rec_find
 * (parent-id chain starting at 1, max 8 levels); the final component is
 * matched by FUN_80198190, then the 0x40-entry file table @0x801fc700
 * (stride 0x18, flag/name at +8) is scanned with cd_strncmp12_eq. On hit the
 * 0x18-uint8_t record is copied to a0 (host) and its guest addr returned.
 * Verbosity gate: lw 0x801cc850 (>0 prints, >=2 prints search hits).
 * 0x801ccb74 caches the media-change generation @0x801cc85c; mismatch
 * re-inits via FUN_80197e28 (unported -> call_guest). */
int cd_search_file(uint32_t *path,uint32_t arg)
{
  const uint8_t *name = (const uint8_t *)guest_at(arg);
  const uint8_t *s0;
  uint8_t *s1;
  /* token buffer must live in guest RAM: cd_file_rec_find/cd_strncmp12_eq
   * receive it as a u32 guest address (cd_arg_ptr). Guest frame is 0x50
   * (addiu sp,-0x50 @0x80197b40) with the 64-uint8_t token buf at sp+0, so
   * reserve the window on the live guest sp — callee frames would
   * otherwise land on lbuf between the per-token rebuild and the
   * cd_strncmp12_eq scan. gpu_rect uses sp-0x10..sp-1, so the guest's
   * own sp-0x50..sp-0x11 scratch is reproduced by frm+0..frm+0x3f. */
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
void gte_vec_transform_emit(int a0,int src,int dst)
{
  int mtx;
  mtx = *(int *)guest_at((uint32_t)(uintptr_t)(src + 0x40)) + 0x20;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)(dst + 8)) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(src + 0x34));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)(dst + 0xa)) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(src + 0x38));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)(dst + 0xc)) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(src + 0x3c));
  rtps_point_xform2((uint32_t *)guest_at((uint32_t)(uintptr_t)mtx),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 0x18)),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 0x34)));
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 4)) = 0x1000;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 8)) = 0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 0xc)) = 0x1000;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 0x10)) = 0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 0x14)) = 0x1000;
  gte_rtps_xform3((uint32_t *)guest_at((uint32_t)(uintptr_t)mtx),
               (uint16_t *)guest_at((uint32_t)(uintptr_t)(src + 4)),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(src + 0x20)));
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
void corner_emit_cond_offset(int spr,int x0,int y0,int idx,int mul,int x1,int y1)
{
  int t;
  short rect [4];
  t = spr;
  if ((*(int *)guest_at((uint32_t)(uintptr_t)(spr + 4)) & 8) != 0) {
    t = spr + *(int *)guest_at((uint32_t)(uintptr_t)(spr + 8));
  }
  rect[0] = (short)x0;
  rect[1] = (short)y0;
  rect[2] = (short)x1;
  rect[3] = (short)y1;
  t = t + ((idx + mul * (int)*(short *)guest_at((uint32_t)(uintptr_t)(t + 0x10))) << 1) + 0x14;
  /* guest rect = $sp+0x10 (frame 0x20, disasm 0x8018bda4) -> frm+0x10;
   * reserved frame staging, see spr_draw_off in gfx.c. */
  {
    uint32_t cec_frm = br2_guest_frame(0x20);
    memcpy(guest_at(cec_frm + 0x10), rect, 8);
    gpu_load_image_cmd(cec_frm + 0x10,t);
    br2_guest_frame_free();
  }
  return;
}

/* guest corner_emit_variant @0x8018be28 — corner emit variant: p6!=0 uses raw
 * p2/p3 else adds t0+0xc/0xe base; h10/h12 always offset by p4/p5. */
void corner_emit_variant(int spr,int x0,int y0,int dx,int dy,int abs)
{
  int t;
  short rect [4];
  t = spr;
  if ((*(int *)guest_at((uint32_t)(uintptr_t)(spr + 4)) & 8) != 0) {
    t = spr + *(int *)guest_at((uint32_t)(uintptr_t)(spr + 8));
  }
  if (abs != 0) {
    rect[0] = (short)x0;
    rect[1] = (short)y0;
  }
  else {
    rect[0] = (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(t + 0xc)) + x0);
    rect[1] = (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(t + 0xe)) + y0);
  }
  rect[2] = (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(t + 0x10)) + dx);
  rect[3] = (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(t + 0x12)) + dy);
  /* guest rect = $sp+0x10 (frame 0x20, disasm 0x8018be28) -> frm+0x10;
   * reserved frame staging, see spr_draw_off in gfx.c. */
  {
    uint32_t cev_frm = br2_guest_frame(0x20);
    memcpy(guest_at(cev_frm + 0x10), rect, 8);
    gpu_load_image_cmd(cev_frm + 0x10,t + 0x14);
    br2_guest_frame_free();
  }
  return;
}

/* ======== batch31: debug putc + emit chains + vec lerp + record init ===== */
/* externs for batch31 callees */
extern void console_emit(uint32_t c);
extern int32_t bios_tramp_b035(int32_t a0,const void *a1,int32_t a2);
extern int  rotate_xy_pairs(int n,uint32_t ang);  /* guest 0x80195570 — battle.c */
extern void * spr_pool_b_alloc(void);  /* gfx.c def — returns host ptr; guest_of() it */
extern void *spr_desc_slot_alloc(void);   /* gfx.c — returns host ptr */

/* guest debug_line_putc @0x8018ee28 — debug line-buffer putc: tab expands to
 * spaces to col&7==0, LF (0xa) emits CR and resets col, other chars bump
 * col per 0x801b6701 class table; appends to 32B buf @0x801f68f0 flushed
 * via bios_tramp_b035. */
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
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)(0x801f68f0 + *(int *)guest_at(0x801b6794))) = (uint8_t)ch;
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
void channel_script_emit_chain_gate(int ctx,uint32_t arg,int rec,int ent,int xf)
{
  int t;
  int dst;
  int obj;
  obj = *(int *)guest_at((uint32_t)(uintptr_t)(ent + 0xbc));
  t = ctx + 0x2048;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),t,arg,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,t,arg,1);
  dst = ctx + 0x2078;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x190,dst,arg,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xc8,dst,arg,3);
  t = (int)*(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x26)) + 0x17;
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x26)) = (short)t;
  if ((t & 1) == 0) {
    t = (short)t;
    rotmat_rtps_build((uint32_t)t,(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0xc4)));
    gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)xf),(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0xc4)));
    prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x168,ctx + 0x2074,arg,1);
  }
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x8c)) = 0;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x88)) = (uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)(obj + 0x4a));
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x90)) = (uint32_t)*(short *)guest_at((uint32_t)(uintptr_t)(obj + 0x4e));
  gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)xf),(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x74)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x140,ctx + 0x2078,arg,1);
  return;
}

/* guest vec3_approach_step @0x80133944 — vec3 approach: diff = p3-p2, normalize
 * via FUN_80194630, snap to target when |d|<p4 else step p4/4096 along it;
 * out p1 receives the applied delta (or raw diff on snap). */
int vec3_approach_step(int out,int pos,int dst,int amt)
{
  int step;
  int len;
  uint32_t delta [3];
  uint32_t ndir [3];
  delta[0] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)dst) - *(uint32_t *)guest_at((uint32_t)(uintptr_t)pos);
  delta[1] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(dst + 4)) - *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 4));
  delta[2] = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(dst + 8)) - *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 8));
  step = amt;
  if (amt == 0) {
    step = 1;
  }
  len = FUN_80194630(delta,ndir);
  len = FUN_80194500(len);
  if (len < step) {
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)pos) = *(uint32_t *)guest_at((uint32_t)(uintptr_t)dst);
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 4)) = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(dst + 4));
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 8)) = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(dst + 8));
  }
  else {
    delta[0] = (uint32_t)((int)ndir[0] * step >> 0xc);
    delta[1] = (uint32_t)((int)ndir[1] * step >> 0xc);
    delta[2] = (uint32_t)((int)ndir[2] * step >> 0xc);
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)pos) = *(uint32_t *)guest_at((uint32_t)(uintptr_t)pos) + delta[0];
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 4)) = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 4)) + delta[1];
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 8)) = *(uint32_t *)guest_at((uint32_t)(uintptr_t)(pos + 8)) + delta[2];
  }
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)out) = delta[0];
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(out + 4)) = delta[1];
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(out + 8)) = delta[2];
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
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 8))  = *(uint32_t *)guest_at(0x801b1840);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0xc)) = *(uint32_t *)guest_at(0x801b1844);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x10)) = *(uint32_t *)guest_at(0x801b1848);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x14)) = *(uint32_t *)guest_at(0x801b184c);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x18)) = *(uint32_t *)guest_at(0x801b1850);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x1c)) = *(uint32_t *)guest_at(0x801b1854);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x20)) = *(uint32_t *)guest_at(0x801b1858);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x24)) = *(uint32_t *)guest_at(0x801b185c);
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x2a)) = 0x400;
  rand_lcg();
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x28)) = 0x400;
  grp = *(int *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)rec)));
  rotmat_rtps_build(0x400,(uint32_t *)guest_at((uint32_t)(uintptr_t)grp));
  gte_rtps_xform3((uint32_t *)guest_at(0x801f0760),(uint16_t *)guest_at((uint32_t)(uintptr_t)grp),(uint32_t *)guest_at((uint32_t)(uintptr_t)grp));
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
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(arg + 0x28)) + 0x138);
    *(short *)guest_at((uint32_t)(uintptr_t)(arg + 0x2a)) =
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(arg + 0x2a)) + 0x1c4);
  }
  obj_flag34_set(*(int *)guest_at((uint32_t)(uintptr_t)arg));
  t = (int)*(uint16_t *)guest_at((uint32_t)(uintptr_t)(arg + 0x2a));
  *(short *)guest_at((uint32_t)(uintptr_t)(arg + 8)) = (short)t;
  *(short *)guest_at((uint32_t)(uintptr_t)(arg + 0x10)) = (short)t;
  angle_vec_build((uint32_t)(short)*(short *)guest_at((uint32_t)(uintptr_t)(arg + 0x28)),
               (uint32_t *)guest_at(0x1f8003e0));
  gte_rtps_xform3((uint32_t *)guest_at((uint32_t)(uintptr_t)(arg + 8)),
               (uint16_t *)guest_at(0x1f8003e0),(uint32_t *)guest_at(0x1f8003e0));
  t = *(int *)guest_at((uint32_t)(uintptr_t)arg);
  gte_rtps_xform3((uint32_t *)guest_at(0x801f0760),(uint16_t *)guest_at(0x1f8003e0),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)t))));
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
 * buffer: u16 angle fields from lut_cos/lut_sin(hdg+0x400)>>6 at +0xb8/+0xbc
 * (hdg = f+0x4e heading; ang is stored raw at +0xc0/+0xc2), a 3x3 u16
 * matrix copied column-wise from a1 (+0xa0..+0xb4) with the two middle
 * entries scaled by the trig results <<2, then two passes of two
 * 0x28-uint8_t packet records each (blk = pkt + i*0x50, tag 0x09000000)
 * whose uint8_t fields depend on the u8 flag at f+2. Disasm 0x8013e0f0-0x8013e370. */
void task_packet_emit(int f,int m,int ang)
{
  int node;
  int hdg;
  uint32_t pkt;
  uint32_t blk;
  int i;
  int j;

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
  *(uint16_t *)guest_at(pkt + 0xa0) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 0));
  *(uint16_t *)guest_at(pkt + 0xa8) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 0));
  *(uint16_t *)guest_at(pkt + 0xb0) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 0));
  *(uint16_t *)guest_at(pkt + 0xa2) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 4));
  *(uint16_t *)guest_at(pkt + 0xaa) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 4));
  *(uint16_t *)guest_at(pkt + 0xb2) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 4));
  *(uint16_t *)guest_at(pkt + 0xa4) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 8));
  *(uint16_t *)guest_at(pkt + 0xac) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 8));
  *(uint16_t *)guest_at(pkt + 0xb4) =
    *(uint16_t *)guest_at((uint32_t)(uintptr_t)(m + 8));
  *(uint16_t *)guest_at(pkt + 0xa8) =
    (uint16_t)(*(uint16_t *)guest_at(pkt + 0xa8) +
             (*(uint16_t *)guest_at(pkt + 0xb8) << 2));
  *(uint16_t *)guest_at(pkt + 0xac) =
    (uint16_t)(*(uint16_t *)guest_at(pkt + 0xac) +
             (*(uint16_t *)guest_at(pkt + 0xbc) << 2));
  /* two outer passes of two 0x28-uint8_t records each (guest ang/t1 += 0x50) */
  for (i = 0; i < 2; i++) {
    blk = pkt + i * 0x50;
    for (j = 0; j < 2; j++) {
      uint32_t rp = blk + j * 0x28;
      *(uint32_t *)guest_at(rp) = 0x09000000;
      *(uint8_t *)guest_at(rp + 7) = 44;
      *(short *)guest_at(rp + 0x16) = 63;
    }
    {
      uint8_t var = *(uint8_t *)guest_at((uint32_t)(uintptr_t)(f + 2));
      if (var == 0) {
      for (j = 0; j < 2; j++) {
        *(short *)guest_at(blk + j * 0x28 + 0xe) = 0x7846;
      }
      /* variant B: rec0 bytes, rec1 head, shared rec1 tail */
      *(uint8_t *)guest_at(blk + 0xc)  = 144;
      *(uint8_t *)guest_at(blk + 0x1c) = 144;
      *(uint8_t *)guest_at(blk + 0xd)  = 0;
      *(uint8_t *)guest_at(blk + 0x14) = 160;
      *(uint8_t *)guest_at(blk + 0x15) = 0;
      *(uint8_t *)guest_at(blk + 0x1d) = 32;
      *(uint8_t *)guest_at(blk + 0x24) = 160;
      *(uint8_t *)guest_at(blk + 0x25) = 32;
      *(uint8_t *)guest_at(blk + 0x28 + 0xc)  = 160;
      *(uint8_t *)guest_at(blk + 0x28 + 0xd)  = 0;
      *(uint8_t *)guest_at(blk + 0x28 + 0x14) = 192;
      *(uint8_t *)guest_at(blk + 0x28 + 0x15) = 0;
      }
    else {
      for (j = 0; j < 2; j++) {
        *(short *)guest_at(blk + j * 0x28 + 0xe) = 0x7847;
      }
      /* variant C: same layout, colour bytes 64/96 */
      *(uint8_t *)guest_at(blk + 0xc)  = 144;
      *(uint8_t *)guest_at(blk + 0x1c) = 144;
      *(uint8_t *)guest_at(blk + 0xd)  = 64;
      *(uint8_t *)guest_at(blk + 0x14) = 160;
      *(uint8_t *)guest_at(blk + 0x15) = 64;
      *(uint8_t *)guest_at(blk + 0x1d) = 96;
      *(uint8_t *)guest_at(blk + 0x24) = 160;
      *(uint8_t *)guest_at(blk + 0x25) = 96;
      *(uint8_t *)guest_at(blk + 0x28 + 0xc)  = 160;
      *(uint8_t *)guest_at(blk + 0x28 + 0xd)  = 64;
      *(uint8_t *)guest_at(blk + 0x28 + 0x14) = 192;
      *(uint8_t *)guest_at(blk + 0x28 + 0x15) = 64;
    }
    /* shared tail on rec1 (blk+0x28): +0x1c=160, +0x1d=variant,
     * +0x24=192, +0x25=variant — emitted identically in both variants
     * with the variant uint8_t in f (32 / 96). */
    *(uint8_t *)guest_at(blk + 0x28 + 0x1c) = 160;
    *(uint8_t *)guest_at(blk + 0x28 + 0x1d) = (var == 0) ? 32 : 96;
    *(uint8_t *)guest_at(blk + 0x28 + 0x24) = 192;
    *(uint8_t *)guest_at(blk + 0x28 + 0x25) = (var == 0) ? 32 : 96;
    /* both records: colour bytes +4..+6 = 127, tag uint8_t +7 RMW */
    for (j = 0; j < 2; j++) {
      uint32_t rp = blk + j * 0x28;
      uint8_t tag;
      *(uint8_t *)guest_at(rp + 4) = 127;
      *(uint8_t *)guest_at(rp + 5) = 127;
      *(uint8_t *)guest_at(rp + 6) = 127;
      tag = *(uint8_t *)guest_at(rp + 7);
      *(uint8_t *)guest_at(rp + 7) = (uint8_t)((tag & 0xfe) | 2);
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
  rtps_point_xform2((uint32_t *)guest_at((uint32_t)(uintptr_t)(mtx + 0x20)),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(nd + 0x18)),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(nd + 0x34)));
  *(short *)guest_at((uint32_t)(uintptr_t)(out + 6)) = 0;
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)out) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(t + 4));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 2)) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(t + 6));
  *(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 4)) = *(uint16_t *)guest_at((uint32_t)(uintptr_t)(t + 8));
  t = (int)((*(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 6)) << 0x10) >> 0x11);
  part_matrix_compute((uint32_t *)guest_at((uint32_t)(uintptr_t)(nd + 4)),
               (uint8_t *)guest_at((uint32_t)(uintptr_t)(*(int *)guest_at((uint32_t)(uintptr_t)(f + 0x48)) + idx * 0x20)),
               (short *)guest_at((uint32_t)(uintptr_t)out),t);
  if (0 < (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xe))) {
    t = ((int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x14)) -
             (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa))) /
            (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xe));
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0xe)) =
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 0xe)) - 1);
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) =
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) + t);
  }
  else if (0 < (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x10))) {
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0x10)) =
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 0x10)) - 1);
  }
  else if (0 < (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x12))) {
    t = (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) /
            (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0x12));
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0x12)) =
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 0x12)) - 1);
    *(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) =
      (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(out + 0xa)) - t);
  }
  t = (int)*(short *)guest_at((uint32_t)(uintptr_t)(out + 0xa));
  va = nd + 4;
  rotate_xy_pairs(t,va);
  gte_rtps_xform3((uint32_t *)guest_at((uint32_t)(uintptr_t)(mtx + 0x20)),
               (uint16_t *)guest_at((uint32_t)(uintptr_t)va),
               (uint32_t *)guest_at((uint32_t)(uintptr_t)(nd + 0x20)));
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
void channel_script_emit_chain_triple(int ctx,int arg,int rec,int xf)
{
  int t;
  int dst;
  t = ctx + 0x2070;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xc8,t,arg,1);
  dst = ctx + 0x2074;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xa0,dst,arg,1);
  t = ctx + 0x2078;
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x370,t,arg,1);
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)rec),t,arg,1);
  t = (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x26)) + 5);
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x26)) = (short)t;
  rotmat_rtps_build((uint32_t)t,(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0xc4)));
  gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)xf),(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0xc4)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0xf0,ctx + 0x2070,arg,2);
  t = (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x28)) + 2);
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x28)) = (short)t;
  rotmat_rtps_build((uint32_t)t,(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x114)));
  gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)xf),(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x114)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x140,ctx + 0x2070,arg,8);
  t = (short)(*(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x2a)) + 1);
  *(short *)guest_at((uint32_t)(uintptr_t)(rec + 0x2a)) = (short)t;
  rotmat_rtps_build((uint32_t)t,(uint32_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x164)));
  gte_xform3_t_ip((uint32_t *)guest_at((uint32_t)xf),(uint16_t *)guest_at((uint32_t)(uintptr_t)(rec + 0x164)));
  prim_task_run(*(int *)guest_at((uint32_t)(uintptr_t)(rec + 8)) + 0x280,dst,arg,6);
  return;
}

/* guest sprite_prim_build @0x8018c578 — builds a sprite prim via spr_pool_b_alloc:
 * uv/size deltas t1/t3 from p3 bytes +1, scaled by sext16(p5)/sext16(p6)
 * >>12, writes texpage/colour/clut fields and halfword corners, ORs
 * 0x09000000 into *p1 and stores low24(prim) back to *p1. */
void sprite_prim_build(int ot,int pos,int dim,int uv,int sw,int sh,int col)
{
  int spr;
  int w;
  int h;
  int t2;
  int t3;
  int t0;
  int t1;
  spr = (int)guest_of(spr_pool_b_alloc());
  if (spr != 0) {
    w = ((dim >> 0x10) & 0xff) + 1;
    h = (int)((uint32_t)dim >> 0x18) + 1;
    t2 = uv & 0xff;
    t3 = (uv >> 8) & 0xff;
    t0 = w * (int)(short)sw;
    t1 = h * (int)(short)sh;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 0x15)) = (uint8_t)t3;
    if (t0 < 0) {
      t0 = t0 + 0xfff;
    }
    if (t1 < 0) {
      t1 = t1 + 0xfff;
    }
    w = t0 >> 0xc;
    h = t1 >> 0xc;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)(spr + 0xc)) = uv;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x16)) = (short)dim;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 0x14)) = (uint8_t)(t2 + w);
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 0x1c)) = (uint8_t)t2;
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 0x1d)) = (uint8_t)(t3 + h);
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 0x24)) = (uint8_t)(t2 + w);
    *(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 0x25)) = (uint8_t)(t3 + h);
    /* corner halves: negated values use sign of the negated operand,
     * positive use sign of the adjusted raw product (t0/t1) */
    t2 = pos + (((-w) + (int)((uint32_t)(-w) >> 0x1f)) >> 1);
    t3 = (pos >> 0x10) + (((-h) + (int)((uint32_t)(-h) >> 0x1f)) >> 1);
    t0 = pos + ((w + (int)((uint32_t)t0 >> 0x1f)) >> 1);
    t1 = (pos >> 0x10) + ((h + (int)((uint32_t)t1 >> 0x1f)) >> 1);
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 8)) = (short)t2;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x18)) = (short)t2;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x1a)) = (short)t1;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x22)) = (short)t1;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0xa)) = (short)t3;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x12)) = (short)t3;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x10)) = (short)t0;
    *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x20)) = (short)t0;
    t2 = ((col & 0xff) * 0x010101) | 0x2e000000;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)(spr + 4)) = t2;
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)ot) =
      *(uint32_t *)guest_at((uint32_t)(uintptr_t)ot) | 0x09000000;
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
int sprite_obj_alloc_emit(int out,int pos,int dim,int arg,int clut)
{
  int spr;
  spr = (int)guest_of(spr_desc_slot_alloc());
  if (spr == 0) {
    return -1;
  }
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x10)) =
    (short)(((dim >> 0x10) & 0xff) + 1);
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0x12)) =
    (short)((int)((uint32_t)dim >> 0x18) + 1);
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 0xa)) = (short)(pos >> 0x10);
  *(short *)guest_at((uint32_t)(uintptr_t)(spr + 8)) = (short)pos;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(spr + 0xc)) = arg;
  *(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 7)) =
    (uint8_t)((*(uint8_t *)guest_at((uint32_t)(uintptr_t)(spr + 7)) & 0xfd) | 1);
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(spr + 4)) = 0x64808080;
  prim_packet_init_tex(spr + 0x14,*(uint8_t *)guest_at(0x801e4117),*(uint8_t *)guest_at(0x801e4116),
               (uint32_t)(short)dim,(uint8_t *)guest_at((uint32_t)(uintptr_t)clut));
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)spr) =
    *(uint32_t *)guest_at((uint32_t)(uintptr_t)spr) | 0x04000000;
  *(int *)guest_at((uint32_t)(uintptr_t)out) = spr & 0xffffff;
  *(uint32_t *)guest_at((uint32_t)(uintptr_t)(spr + 0x14)) =
    (spr & 0xffffff) | 0x02000000;
  *(int *)guest_at((uint32_t)(uintptr_t)out) = (spr + 0x14) & 0xffffff;
  return 0;
}
