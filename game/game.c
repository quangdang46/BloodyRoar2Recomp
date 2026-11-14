/* game.c — top-level game shell: boot, main state machine,
 * CD file pump, frame driver, match loop, stage load.
 *
 * Guest-faithful verbatim ports (Ghidra decompiles preserved in
 * decomp/). Guest addresses are passed as raw ints per project
 * convention; globals via the g_DAT arena bindings in bind.c.
 *
 * Decoded architecture (live + static verified):
 *   entry_0x80126178  _start: BSS clear -> entry_0x80126234
 *   entry_0x80126234  main(): boot(boot) -> settings ->
 *                     g_game_state SM (0-13) via switchD_80126370
 *   boot      boot: ~25 subsystem inits + CD file pump
 *                     cd_file_pump until state 0x120 -> -1
 *   cd_file_pump      CD streaming pump: loads ~27 files into
 *                     _DAT_801e3cfc heap cursor, DMA queue per file
 *   frame_tick      frame driver: pad poll, GPU sync/DMA kick,
 *                     per-fighter buffer toggle (+0xc 0/1),
 *                     calls menu_noop logic + scroll_offset_set end
 *   match_loop      match loop: DAT_1f800001 stage machine,
 *                     300-frame attract timeout, match_end_check logic
 *   scene_entry      scene/stage entry: file-table load, VRAM
 *                     window setup, stream reader, palette, then
 *                     battle_session_arm(1) battle start + 2 frames
 *   battle_session_arm      battle session arm: fighter buf ptrs +0x4/+8
 *                     = param_2*0x2150 + fixed guest addrs
 */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t  byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef uint8_t  undefined1;
typedef uint8_t  undefined;
typedef int64_t  longlong;
typedef uint64_t ulonglong;
#define CONCAT22(hi,lo) (((undefined4)(uint16_t)(hi) << 16) | (uint16_t)(lo))
#define CONCAT11(hi,lo) (((uint16_t)(uint8_t)(hi) << 8) | (uint8_t)(lo))

/* ---- guest globals (bindings in bind.c) ---- */
extern uint8_t  *g_DAT_1f800000;   /* g_game_state (u32 at battle base) */
extern uint8_t  *g_DAT_1f800001;   /* frame-loop stage */
extern uint8_t  *g_DAT_1f800002;
extern uint8_t  *g_DAT_1f800003;
extern uint8_t  *g_DAT_1f800004;
extern uint8_t  *g_DAT_1f800008;
extern uint8_t  *g_DAT_1f80000c;
extern uint8_t  *g_DAT_1f800011;
extern uint8_t  *g_DAT_1f800013;
extern uint8_t  *g_DAT_1f800015;
extern uint8_t  *g_DAT_1f80009b;
extern uint8_t  *g_DAT_1f80009c;
extern uint8_t  *g_DAT_1f80009e;
extern uint8_t  *g_DAT_1f80009f;
extern uint8_t  *g_DAT_1f8000a2;
extern uint8_t  *g_DAT_1f8000a4;
extern uint8_t  *g_DAT_1f8000a8;
extern uint8_t  *g_DAT_1f8000ac;
extern uint8_t  *g_DAT_1f8000b0;
extern uint8_t  *g_DAT_1f8000b1;
extern uint8_t  *g_DAT_1f8000b2;
extern uint8_t  *g_DAT_1f8000b8;
extern uint8_t  *g_DAT_1f8000bc;
extern uint8_t  *g_DAT_1f8000cc;
extern uint8_t  *g_DAT_1f8000d8;
extern uint8_t  *g_DAT_1f8000e0;
extern uint8_t  *g_DAT_1f8000e4;
extern uint8_t  *g_DAT_1f800092;
extern uint8_t  *g_DAT_1f800088;
extern uint8_t  *g_DAT_1f80008c;
extern uint8_t  *g_DAT_1f80008e;
extern uint8_t  *g_DAT_1f800084;
extern uint8_t  *g_DAT_1f800080;
extern uint8_t  *g_DAT_1f8000dc;
extern uint8_t  *g_DAT_801e4110;
extern uint8_t  *g_DAT_1f800078;
extern uint8_t  *g_DAT_1f800070;
extern uint8_t  *g_DAT_1f80006c;
extern uint8_t  *g_DAT_1f800010;    /* +0x10 u8 (frame driver) */
extern uint8_t  *g_DAT_1f800012;    /* +0x12 u8 */
extern uint8_t  *g_DAT_1f80013;    /* +0x13 u8 */
extern uint8_t  *g_DAT_1f80015;    /* +0x15 u8 */
extern uint8_t  *g_DAT_1f800082;    /* +0x82 u16 flags (pad poll) */
extern uint8_t  *g_DAT_1f8000d0;    /* +0xd0 u16 dma count */
extern uint8_t  *g_DAT_1f8000d2;    /* +0xd2 u16 dma count */
extern uint8_t  *g_DAT_1f80001a;    /* +0x1a u16 vblank ts */
extern uint8_t  *g_DAT_1f80001c;    /* +0x1c u16 vblank ts */
extern uint8_t  *g_DAT_1f800020;    /* +0x20 screen W */
extern uint8_t  *g_DAT_1f800022;    /* +0x22 screen H */
extern uint8_t  *g_DAT_1f800024;    /* +0x24 screen OX */
extern uint8_t  *g_DAT_1f80007c;    /* +0x7c u32 flags */
extern uint8_t  *g_DAT_801bef90;
extern uint8_t  *g_DAT_801befa8;
extern uint8_t  *g_DAT_801befca;
extern uint8_t  *g_DAT_801bef38;
extern uint8_t  *g_DAT_801befec;
extern uint8_t  *g_DAT_801befa6;
extern uint8_t  *g_DAT_801c1200;   /* system config block */
extern uint8_t  *g_DAT_801c11d8;
extern uint8_t  *g_DAT_801c1444;
extern uint8_t  *g_DAT_801c1446;
extern uint8_t  *g_DAT_801c1448;
extern uint8_t  *g_DAT_801c144a;
extern uint8_t  *g_DAT_801c144c;
extern uint8_t  *g_DAT_801c1450;
extern uint8_t  *g_DAT_801c15f0;
extern uint8_t  *g_DAT_801c15f4;
extern uint8_t  *g_DAT_801c15f8;
extern uint8_t  *g_DAT_801c43a0;
extern uint8_t  *g_DAT_801c4578;
extern uint8_t  *g_DAT_801c457c;
extern uint8_t  *g_DAT_801c4580;
extern uint8_t  *g_DAT_801c4581;
extern uint8_t  *g_DAT_801c4582;
extern uint8_t  *g_DAT_801c47b8;
extern uint8_t  *g_DAT_801c47c0;
extern uint8_t  *g_DAT_801e3cfc;
extern uint8_t  *g_DAT_801e3d00;
extern uint8_t  *g_DAT_801bccac;
extern uint8_t  *g_DAT_801bccb0;
extern uint8_t  *g_DAT_801bccb4;
extern uint8_t  *g_DAT_801bccb8;
extern uint8_t  *g_DAT_801bcca8;
extern uint8_t  *g_DAT_801bdde0;
extern uint8_t  *g_DAT_801e40a0;
extern uint8_t  *g_DAT_801e40fc;
extern uint8_t  *g_DAT_801e4104;
extern uint8_t  *g_DAT_801e4106;
extern uint8_t  *g_DAT_801e4108;
extern uint8_t  *g_DAT_801e410a;
extern uint8_t  *g_DAT_801e410c;
extern uint8_t  *g_DAT_801e4104;
extern uint8_t  *g_DAT_801e4106;
extern uint8_t  *g_DAT_801e4108;
extern uint8_t  *g_DAT_801e410a;
extern uint8_t  *g_DAT_801e410c;
extern uint8_t  *g_DAT_801e410e;
extern uint8_t  *g_DAT_801e4110;
extern uint8_t  *g_DAT_801e40a0;
extern uint8_t  *g_DAT_801e40b6;
extern uint8_t  *g_DAT_801e40b7;
extern uint8_t  *g_DAT_801e40b8;
extern uint8_t  *g_DAT_801e40b9;
extern uint8_t  *g_DAT_801e40ba;
extern uint8_t  *g_DAT_801e40bb;
extern uint8_t  *g_DAT_801e40fc;
extern uint8_t  *g_DAT_801e61f0;
extern uint8_t  *g_DAT_801e624c;
extern uint8_t  *g_DAT_801e6254;
extern uint8_t  *g_DAT_801e6256;
extern uint8_t  *g_DAT_801e6258;
extern uint8_t  *g_DAT_801e625a;
extern uint8_t  *g_DAT_801e625c;
extern uint8_t  *g_DAT_801e625e;
extern uint8_t  *g_DAT_801e6206;
extern uint8_t  *g_DAT_801e6207;
extern uint8_t  *g_DAT_801e6208;
extern uint8_t  *g_DAT_801e6209;
extern uint8_t  *g_DAT_801e620a;
extern uint8_t  *g_DAT_801e620b;
extern uint8_t  *g_DAT_801e61f0;
extern uint8_t  *g_DAT_801e624c;
extern uint8_t  *g_DAT_801e6254;
extern uint8_t  *g_DAT_801e6256;
extern uint8_t  *g_DAT_801e6258;
extern uint8_t  *g_DAT_801e625a;
extern uint8_t  *g_DAT_801e625c;
extern uint8_t  *g_DAT_801e625e;
extern uint8_t  *g_DAT_801e8500;
extern uint8_t  *g_DAT_801f0270;
extern uint8_t  *g_DAT_801f4700;
extern uint8_t  *g_DAT_801f4708;
extern uint8_t  *g_DAT_801f4714;
extern uint8_t  *g_DAT_801f4730;
extern uint8_t  *g_DAT_801f4732;
extern uint8_t  *g_DAT_801f473c;
extern uint8_t  *g_DAT_801f4740;
extern uint8_t  *g_DAT_801f4744;
extern uint8_t  *g_DAT_801f4748;
extern uint8_t  *g_DAT_801f474c;
extern uint8_t  *g_DAT_801f4750;
extern uint8_t  *g_DAT_801f4754;
extern uint8_t  *g_DAT_801f4758;
extern uint8_t  *g_DAT_801f0664;
extern uint8_t  *g_DAT_801f0668;
extern uint8_t  *g_DAT_801f066a;
extern uint8_t  *g_DAT_801f066e;
extern uint8_t  *g_DAT_801202e4;
extern uint8_t  *g_DAT_801202e8;
extern uint8_t  *g_DAT_801202ec;
extern uint8_t  *g_DAT_801202f0;
extern uint8_t  *g_DAT_801202f4;
extern uint8_t  *g_DAT_80121e3c;   /* "PRESS START" string */
extern uint8_t  *g_DAT_801b1280;   /* settings defaults table */
extern uint8_t  *g_DAT_801b12e0;
extern uint8_t  *g_DAT_801b12e6;
extern uint8_t  *g_DAT_801b3ed8;   /* stage param table (0x1c stride) */
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t *g_PTR_DAT_801b5b94; /* file ptr table */
extern uint8_t  *g_DAT_801b5b98;
extern uint8_t  *g__DAT_801f68d8;
extern uint8_t  *g_DAT_801b55ec;
extern uint8_t  *_g_pad_buffer;
extern uint8_t  *_g_system_config_ptr;

#define DAT_1f800000    (*(uint8_t *)g_DAT_1f800000)
#define g_game_state    DAT_1f800000
#define DAT_1f800001    (*g_DAT_1f800001)
#define DAT_1f800002    (*g_DAT_1f800002)
#define g_sub_state     DAT_1f800002
#define DAT_1f800003    (*g_DAT_1f800003)
#define DAT_1f800004    (*(uint32_t *)g_DAT_1f800004)
#define DAT_1f800008    (*(uint32_t *)g_DAT_1f800008)
#define DAT_1f80000c    (*(uint32_t *)g_DAT_1f80000c)
#define DAT_1f800011    (*g_DAT_1f800011)
#define DAT_1f800013    (*g_DAT_1f800013)
#define DAT_1f800015    (*g_DAT_1f800015)
#define DAT_1f80009b    (*g_DAT_1f80009b)
#define DAT_1f80009c    (*g_DAT_1f80009c)
#define DAT_1f80009e    (*g_DAT_1f80009e)
#define DAT_1f80009f    (*g_DAT_1f80009f)
#define DAT_1f8000a2    (*g_DAT_1f8000a2)
#define DAT_1f8000a4    (*g_DAT_1f8000a4)
#define DAT_1f8000a8    (*g_DAT_1f8000a8)
#define DAT_1f8000ac    (*g_DAT_1f8000ac)
#define DAT_1f8000b0    (*g_DAT_1f8000b0)
#define DAT_1f8000b1    (*g_DAT_1f8000b1)
#define DAT_1f8000b2    (*g_DAT_1f8000b2)
#define DAT_1f8000b8    (*(uint32_t *)g_DAT_1f8000b8)
#define DAT_1f8000bc    (*(uint32_t *)g_DAT_1f8000bc)
#define DAT_1f8000cc    (*(uint32_t *)g_DAT_1f8000cc)
#define DAT_1f8000d8    (*(uint32_t *)g_DAT_1f8000d8)
#define DAT_1f8000e0    (*(uint32_t *)g_DAT_1f8000e0)
#define DAT_1f8000e4    (*(uint32_t *)g_DAT_1f8000e4)
#define DAT_1f800092    (*(uint16_t *)g_DAT_1f800092)
#define DAT_1f800088    (*(uint16_t *)g_DAT_1f800088)
#define DAT_1f80008c    (*(uint32_t *)g_DAT_1f80008c)
#define DAT_1f80008e    (*(uint16_t *)g_DAT_1f80008e)
#define DAT_1f800084    (*(uint32_t *)g_DAT_1f800084)
#define _DAT_1f800084   (*(uint32_t *)g_DAT_1f800084)
#define DAT_1f800078    (*(uint32_t *)g_DAT_1f800078)
#define DAT_1f800070    (*(uint32_t *)g_DAT_1f800070)
#define DAT_1f80006c    (*(uint32_t *)g_DAT_1f80006c)
#define DAT_1f800010    (*g_DAT_1f800010)
#define DAT_1f800012    (*g_DAT_1f80012)
#define DAT_1f80013     (*g_DAT_1f80013)
#define DAT_1f80015     (*g_DAT_1f80015)
#define DAT_1f800082    (*(uint16_t *)g_DAT_1f80082)
#define DAT_1f8000d0    (*(uint16_t *)g_DAT_1f8000d0)
#define DAT_1f8000d2    (*(uint16_t *)g_DAT_1f8000d2)
#define DAT_1f80001a    (*(uint16_t *)g_DAT_1f8001a)
#define DAT_1f80001c    (*(uint16_t *)g_DAT_1f8001c)
#define _DAT_1f800020  (*(uint32_t *)g_DAT_1f800020)
#define _DAT_1f800022  (*(uint32_t *)g_DAT_1f800022)
#define _DAT_1f800024  (*(uint32_t *)g_DAT_1f800024)
#define _DAT_1f80007c  (*(uint32_t *)g_DAT_1f80007c)
#define DAT_801bef90    (*(uint8_t *)g_DAT_801bef90)
#define DAT_801befa8    (*(uint8_t *)g_DAT_801befa8)
#define DAT_801befca    (*(uint8_t *)g_DAT_801befca)
#define DAT_801befec    (*g_DAT_801befec)
#define DAT_801befa6    (*g_DAT_801befa6)
extern uint8_t *_g_system_config_ptr;
#define DAT_801c11d8    (*(uint32_t *)g_DAT_801c11d8)
#define DAT_801c15f0    (*(uint32_t *)g_DAT_801c15f0)
#define DAT_801c15f4    (*(uint32_t *)g_DAT_801c15f4)
#define DAT_801c43a0    (*(uint32_t *)g_DAT_801c43a0)
#define DAT_801c4578    (*(uint32_t *)g_DAT_801c4578)
#define DAT_801c457c    (*(uint32_t *)g_DAT_801c457c)
#define DAT_801c4580    (*(uint32_t *)g_DAT_801c4580)
#define _DAT_801f68d8   (*(int32_t *)g__DAT_801f68d8)
#define DAT_801b55ec    (*(uint8_t *)g_DAT_801b55ec)
#define DAT_801c4581    (*g_DAT_801c4581)
#define DAT_801c4582    (*g_DAT_801c4582)
#define _DAT_801c47b8   (*(uint32_t *)g_DAT_801c47b8)
#define _DAT_801e3cfc   (*(int32_t *)g_DAT_801e3cfc)
#define DAT_801e3d00    (*g_DAT_801e3d00)
#define DAT_801bccac    (*(uint32_t *)g_DAT_801bccac)
#define DAT_801bccb0    (*(uint32_t *)g_DAT_801bccb0)
#define DAT_801bccb4    (*(uint32_t *)g_DAT_801bccb4)
#define DAT_801bccb8    (*(uint32_t *)g_DAT_801bccb8)
#define DAT_801bcca8    (*(uint32_t *)g_DAT_801bcca8)
#define DAT_801bdde0    (*(uint32_t *)g_DAT_801bdde0)
#define _DAT_801e8500   (*(uint32_t *)g_DAT_801e8500)
#define DAT_801bef38    ((uint32_t *)guest_at(0x801bef38))
#define DAT_801f0664    ((uint32_t *)guest_at(0x801f0664))
#define DAT_801f0668    ((uint32_t *)guest_at(0x801f0668))
#define DAT_801f066a    ((uint8_t  *)guest_at(0x801f066a))
#define DAT_801f066e    ((int16_t  *)guest_at(0x801f066e))
#define _DAT_801f0270   (*(uint32_t *)g_DAT_801f0270)
#define _DAT_801f4700   (*(uint32_t *)g_DAT_801f4700)
#define _DAT_801f4708   (*(uint32_t *)g_DAT_801f4708)
#define _DAT_801f4714   (*(uint32_t *)g_DAT_801f4714)
#define _DAT_801f4730   (*(int32_t *)g_DAT_801f4730)
#define _DAT_801f4732   (*(int32_t *)g_DAT_801f4732)
#define _DAT_801f473c   (*(uint32_t *)g_DAT_801f473c)
#define _DAT_801f4740   (*(uint32_t *)g_DAT_801f4740)
#define _DAT_801f4744   (*(uint32_t *)g_DAT_801f4744)
#define _DAT_801f4748   (*(uint32_t *)g_DAT_801f4748)
#define _DAT_801f474c   (*(uint32_t *)g_DAT_801f474c)
#define _DAT_801f4750   (*(uint32_t *)g_DAT_801f4750)
#define _DAT_801f4754   (*(uint32_t *)g_DAT_801f4754)
#define _DAT_801f4758   (*(uint32_t *)g_DAT_801f4758)
#define _DAT_1f800004   (*(uint32_t *)g_DAT_1f800004)
#define _DAT_1f800008   (*(uint32_t *)g_DAT_1f800008)
#define _DAT_1f80000c   (*(uint32_t *)g_DAT_1f80000c)
#define _DAT_1f80000d8  (*(uint32_t *)g_DAT_1f8000d8)
#define _DAT_1f8000dc   (*(uint32_t *)g_DAT_1f8000dc)
#define _DAT_1f8000e4   (*(uint32_t *)g_DAT_1f8000e4)
#define _DAT_1f800092   (*(uint16_t *)g_DAT_1f800092)
#define _DAT_1f800088   (*(uint16_t *)g_DAT_1f800088)
#define _DAT_1f80008c   (*(uint32_t *)g_DAT_1f80008c)
#define _DAT_1f80008e   (*(uint16_t *)g_DAT_1f80008e)
#define _DAT_1f8000a8   (*(uint32_t *)g_DAT_1f8000a8)
#define _DAT_1f8000ac   (*(uint32_t *)g_DAT_1f8000ac)
#define _DAT_1f8000a4   (*(uint32_t *)g_DAT_1f8000a4)
#define _DAT_1f8000a2   (*(uint32_t *)g_DAT_1f8000a2)
#define _DAT_1f8000b2   (*(uint32_t *)g_DAT_1f8000b2)
#define _DAT_1f800080   (*(uint32_t *)g_DAT_1f800080)
#define _DAT_1f800078   (*(uint32_t *)g_DAT_1f800078)
#define _DAT_1f800070   (*(uint32_t *)g_DAT_1f800070)
#define _DAT_1f80006c   (*(uint32_t *)g_DAT_1f80006c)
#define _DAT_1f8000b8   (*(uint32_t *)g_DAT_1f8000b8)
#define _DAT_1f8000bc   (*(uint32_t *)g_DAT_1f8000bc)
#define _DAT_1f8000cc   (*(uint32_t *)g_DAT_1f8000cc)
#define _DAT_801c15f0   (*(uint32_t *)g_DAT_801c15f0)
#define _DAT_801c11d8   (*(uint32_t *)g_DAT_801c11d8)
#define _DAT_801c15f4   (*(uint32_t *)g_DAT_801c15f4)
#define _DAT_801c15f8   (*(uint32_t *)g_DAT_801c15f8)
#define _DAT_801c43a0   (*(uint32_t *)g_DAT_801c43a0)
#define _DAT_801e3d00   (*(uint32_t *)g_DAT_801e3d00)
#define _DAT_801bccac   (*(uint32_t *)g_DAT_801bccac)
#define _DAT_801bccb0   (*(uint32_t *)g_DAT_801bccb0)
#define _DAT_801bccb4   (*(uint32_t *)g_DAT_801bccb4)
#define _DAT_801bccb8   (*(uint32_t *)g_DAT_801bccb8)
#define _DAT_801bcca8   (*(uint32_t *)g_DAT_801bcca8)
#define _DAT_801bdde0   (*(uint32_t *)g_DAT_801bdde0)
#define _DAT_1f8000e0   (*(uint32_t *)g_DAT_1f8000e0)
#define _DAT_1f8000d8   (*(uint32_t *)g_DAT_1f8000d8)


/* _DAT_801e40xx / _DAT_801e61xx / _DAT_801e62xx are u16 fields inside
 * per-player OAM/sprite blocks; accessed via (short*) casts below. */

extern uint32_t uRam801beef0, iRam80126228, iRam801be720;
extern void halt_baddata(void);
extern void trap(int);
extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint32_t guest_sp(void);
extern void *fn_lookup(uint32_t addr);
int mc_init_reset();
int cd_mode_flag_set();
int cd_init_session();
int cd_read_poll();
int cd_read_issue();
void cursor_sprite_anim(uint8_t *);
extern void entry_0x80126234(void);

typedef void code(void);

/* ---- callee externs (ported in later batches; weak-bound meanwhile) ---- */
extern void memcard_tick(void);
extern void deferred_cmd_drain();
extern void cursor_sprite_anim(uint8_t *);
extern void memcard_slot_poll(void);
extern uint cd_timer_read(int);
extern void gpu_pkt_unlink_n(int,int);
extern void pad_state_refresh(intptr_t);
extern void gpu_draw_sync(int);
extern void gpu_graph_reset(int);
extern void draw_env_timing(int);
extern void gpu_draw_env_put(int);
extern void gpu_disp_mask_set(int);
extern void gpu_load_image_cmd(int,int);
extern void gpu_draw_otag(int);
extern void gpu_clear_image_cmd(int,int,uint8_t,uint8_t);
extern uint32_t gpu_stat_sign(void);
extern void table_byte_next_a(void);
extern void menu_noop(int);
extern void debug_stat_print(void);
extern void ot_packet_chain_fixup(void);
extern void scroll_offset_set(int,int);
extern void sprite_rect_table_init(int,int,int);
extern void gpu_clear_otag_r(uint32_t,int);
extern void scratch_013_set(uint8_t);
extern void match_mode_set(uint32_t);
extern int  cd_file_load_sm(int32_t,uint32_t);
extern void FUN_801274d8_(void);
extern void fighter_record_apply(int,int);
extern void page_table_ptr_set(int,int);
extern uint32_t stream_block_decode(uint32_t,int *,int);
extern void spr_draw_off(int,int,int,int);
extern void spr_draw_pair(int,int,int,int,int,int,int);
extern uint8_t *tag_table_get(void);
extern void game_globals_reset(void);
extern void sprite_desc_table_init(int);
extern void print_mode_dispatch_b(int,int,int);
extern void save_defaults_fill(void);
extern void save_block_init(void);
extern void save_tables_commit(void);
extern void dbg_marker_mode_set(uint8_t);
extern void gauge_scale_fixed(uint8_t,int);
extern void brightness_commit(uint8_t);
extern void cd_dispatch_tblc(void);
extern void cd_read_param_set(int);
extern void FUN_80194478(void);
extern void cd_fileop_reset(int);
extern int32_t set_graph_debug(uint32_t);
extern void cd_xchg_bde20(int);
extern void cd_probe_retry(void);
extern void cd_xchg_bc850(int);
extern void cd_subsys_boot(void);
extern void cd_channel_init_seq(void);
extern void boot(void);
extern void frame_advance_helper(void);
extern void display_mode_setup(uint32_t);
extern int  cd_file_pump(int,int);
extern void gfx_tbls_init(void);
extern void game_globals_reset(void);
extern void battle_session_arm(intptr_t,int);
extern undefined4 scene_entry(int,int,int);
extern void frame_tick(intptr_t);
extern undefined4 memcard_poll_frame(void);
extern void match_loop(void);
extern void scratch_zero_reset(void);
extern void game_globals_reset(void);
extern void boot_globals_init(int);
extern void FUN_801560f8_(void);
extern void FUN_801910e4_(void);
extern int  cd_file_pump(int,int);
extern void gfx_tbls_init(void);
extern void battle_session_arm(intptr_t,int);
extern void frame_tick(intptr_t);
extern void display_mode_setup(uint32_t);
extern void FUN_80195290(int,int);
extern void FUN_80195290(int,int);
extern void gte_cw_d000_set(int);
extern void sprite_attr_init_b(int,int,int,int,int);
extern void sprite_attr_init_a(int,int,int,int,int);
extern void oam_list_rebuild(void);
extern void hud_sprite_desc_init(void);
extern void FUN_8019117c_(void);
extern void frame_counter_bump(int);
extern void spr_desc_tbl_a_init(void);
extern void spr_desc_tbl_b_init(void);
extern void spr_tbl_init_a(void);
extern void spr_tbl_init_b(void);
extern void flow_cfg_init(int);
extern void rec_timer_pair_clear(int);
extern void players_ctrl_init(void);
extern uint32_t u16_lut_lookup(uint32_t);
extern void mode_flag_pair_set(uint32_t);
extern void perm_table_load(void);
extern void perm_table_apply(int);
extern void scroll_cfg_set(int,int);
extern void cd_channel_table_init(uint8_t*,uint8_t*);
extern void cd_chan_init_wrap(void);
extern void clear_battle_counters(void);
extern int memcard_info_query(void);
extern void transition_screen_flow(int);
extern void battle_intro_flow(int);
extern void results_scoreboard(int);
extern void battle_flow_state_machine(int);
extern void unlock_progress_scene_entry(int);
extern void player_name_entry(int);
extern int  system_config_menu_a(void);
extern int  system_config_menu_b(void);
extern void menu_stepper_state_advance(void*);
extern void deferred_cmd_post(int,int,int);
extern int  attract_stage_load_step(int,void*);
extern void attract_menu_dispatch(void);
extern void spr_slot_pkt_submit(int,int,uint32_t,uint32_t);
extern void spr_attr_alloc_a(int,int,int,int,int,int,int);
extern int  frame_divisor_get(void);
extern void frame_present(intptr_t);
extern undefined4 match_end_check(intptr_t);
extern void hud_text_blk_alt(int,int,const void *,int,int,int);
extern undefined4 memcard_poll_frame(void);
extern undefined4 scene_entry(int,int,int);
extern void brightness_packet_send(int);
extern void tile_stream_job_init(int,int,int,int,int,int*);
extern void tile_stream_open(uint8_t*,void*,int*);
extern int  tile_stream_pump(void);
extern int blk_io_retry(int,int*);
extern void cd_stream_mode_init(uint32_t);
extern void gpu_clear_image_cmd2(undefined4,int,int,int);
extern void gpu_pkt_attr_set(int,int);
extern void cd_fileop_forward(int,int);
extern void tile_decode_wait(int,int);
extern void display_frame_present(int*,int);
extern int  cd_cmd_issue_sync(int,int,int);
extern void cd_fileop_dispatch(int);
extern void cd_subsystem_reset(void);
extern void vram_tile_decode(int);
extern int  cd_file_open_b(uint32_t);
extern void vsync_wait_n(uint32_t,int);
extern undefined4 memcard_file_flow(int);
extern void guest_bzero(uint32_t,int);

/* ==================================================================
 * entry_0x80126178 — _start: BSS clear + main call.
 * BSS 0x801ceef0..0x801ff650 zeroed; guest_bzero = memset;
 * then entry_0x80126234 (main); trap(1) on return.
 * ================================================================== */
static void entry_0x80126178(void)
{
    /* Faithful _start (disasm 0x80126178-0x8012621c, delay slots incl.):
     *   BSS clear 0x801CEEF0..0x801FF650 (sw $zero loop, end exclusive)
     *   sp = (*(u32*)0x80126228) | 0x80000000   — literal-pool load, NOT a
     *     BSS read; a0 = 0x801FF650 then sll/srl 3 -> phys 0x1FF650
     *   a1 = v0 - *(0x801BE720) - 0x1FF650     — size for BIOS bzero
     *   save $ra at 0x801CEEF0, gp=0x801BEEDC, fp=sp
     *   jal bzero_tramp(0x801FF654, a1);  jal main(0x80126234); break.
     * sp/gp/fp setup is skipped: the runtime owns cpu->gpr[29] and the
     * clean ports address globals absolutely via guest_at(). */
    uint32_t *puVar1 = (uint32_t *)guest_at(0x801ceef0);
    uint32_t *end    = (uint32_t *)guest_at(0x801ff650);
    uint32_t lit, v1, size;
    while (puVar1 < end) *puVar1++ = 0;
    *(uint32_t *)guest_at(0x801ceef0) = 0;   /* saved-ra slot (unused) */
    lit  = *(uint32_t *)guest_at(0x80126228); /* literal pool: 2MB top */
    v1   = *(uint32_t *)guest_at(0x801be720);
    size = (lit - v1) - 0x1ff650u;            /* guest subu wraps identically */
    guest_bzero(0x801ff654, size);
    entry_0x80126234();
    trap(1);
    halt_baddata();
}

/* func_0x80126170 / func_0x80163840 — jr-ra thunks (empty) */
static void func_0x80126170(void) { }
static void func_0x80163840(int p) { (void)p; }

/* apply_saved_settings_or_defaults @0x80126720 */
static void apply_saved_settings_or_defaults(undefined4 *param_1)
{
    undefined4 *puVar1;
    undefined4 *puVar2;
    undefined4 uVar3;
    undefined4 uVar4;
    undefined4 uVar5;

    puVar1 = (undefined4 *)g_DAT_801b1280;
    puVar2 = param_1;
    do {
        uVar3 = puVar1[1];
        uVar4 = puVar1[2];
        uVar5 = puVar1[3];
        *puVar2 = *puVar1;
        puVar2[1] = uVar3;
        puVar2[2] = uVar4;
        puVar2[3] = uVar5;
        puVar1 = puVar1 + 4;
        puVar2 = puVar2 + 4;
    } while (puVar1 != (undefined4 *)g_DAT_801b12e0);
    *puVar2 = *(undefined4 *)g_DAT_801b12e0;
    save_defaults_fill();
    save_block_init();
    save_tables_commit();
    dbg_marker_mode_set(*(undefined1 *)(param_1 + 9));
    gauge_scale_fixed(*(undefined1 *)((uint8_t *)param_1 + 0xe),
                 ((uint)*(byte *)((uint8_t *)param_1 + 0xf) << 3) / 10);
    brightness_commit(*(byte *)(param_1 + 3) ^ 1);
}

/* ==================================================================
 * entry_0x80126234 — main(): boot + g_game_state machine.
 * State map (switchD_80126370 @0x8011e800, 14 entries):
 *   0 -> init sync, ->1      1 -> mode set + load, ->2
 *   2 -> ->3                 3 -> stage select, ->4
 *   4 -> scene_entry demo fight load ->5 (retry on 0)
 *   5 -> per-frame: 2x frame_tick + memcard_poll_frame + match_loop
 *   6 -> post-match ->3      7,8 -> options (battle_flow_state_machine) ->5
 *   9 -> unlock_progress_scene_entry menus  10,12,13 -> nop ->5
 *   11-> system_config_menu_a/5b9a0 save mgr ->5
 * ================================================================== */
void entry_0x80126234(void)
{
    bool bVar1;
    byte bVar2;
    int iVar3;
    uint uVar4;
    undefined4 uVar5;
    ushort uVar6;
    /* auStack_30 = the pad/input buffer. In the original it lives on the guest
     * stack at sp_old-0x30 (generated 0x80126234: addiu sp,-64; buffer at
     * sp+16). The pad driver is generated/BIOS code that writes guest RAM, so
     * _g_pad_buffer must alias that guest buffer — a host stack array would
     * never receive the controller data. */
    ushort *auStack_30 = (ushort *)guest_at((uint32_t)(guest_sp() - 0x30));

    func_0x80126170();
    uVar6 = 0;
    boot();
    auStack_30[1] = 0x1c7f;
    _g_pad_buffer = (uint8_t *)auStack_30;
    auStack_30[0] = 0;
    DAT_1f8000b8 = 0;
    auStack_30[2] = 0;
    auStack_30[3] = 0;
    auStack_30[4] = 0;
    auStack_30[5] = 0;
    auStack_30[6] = 0;
    auStack_30[7] = 0;
    g_game_state = 0;
    g_sub_state = '\0';
    DAT_1f80009b = 0;
    DAT_1f80009c = '\0';
    iVar3 = memcard_info_query();
    if (iVar3 == 0) {
        apply_saved_settings_or_defaults((undefined4 *)_g_system_config_ptr);
    }
    else {
        auStack_30[0] = (ushort)*(byte *)(_g_system_config_ptr + 0x1d);
        dbg_marker_mode_set(*(undefined1 *)(_g_system_config_ptr + 0x24));
        gauge_scale_fixed(*(undefined1 *)(_g_system_config_ptr + 0xe),
                     ((uint)*(byte *)(_g_system_config_ptr + 0xf) << 3) / 10);
        brightness_commit(*(byte *)(_g_system_config_ptr + 0xc) ^ 1);
    }
    frame_advance_helper();
    func_0x80163840(0);
switchD_80126370_caseD_80126344:
    uVar4 = (uint)g_game_state;
code_r0x8012634c:
    bVar1 = uVar4 < 0xe;
LAB_80126350:
    do {
    } while (!bVar1);
    switch(uVar4) {
    case 0: /* 0x80126378 */
        match_mode_set(1);
        g_game_state = 1;
        goto switchD_80126370_caseD_80126344;
    case 1: /* 0x8012638c */
        display_mode_setup(*(undefined1 *)(_g_system_config_ptr + 0xb));
        frame_advance_helper();
        transition_screen_flow(0xf);
        uVar4 = (uint)g_game_state;
        bVar1 = uVar4 < 0xe;
        if (uVar4 == 1) goto LAB_801265b8;
        goto LAB_80126350;
    case 2: /* 0x801263c8 */
        goto switchD_80126370_caseD_801263c8;
    case 3: /* 0x801263e0 */
        uVar5 = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            uVar5 = 1;
        }
        match_mode_set(uVar5);
        display_mode_setup(*(undefined1 *)(_g_system_config_ptr + 0xb));
        frame_advance_helper();
        transition_screen_flow(3);
        transition_screen_flow(1);
        uVar4 = (uint)g_game_state;
        bVar1 = uVar4 < 0xe;
        if (uVar4 == 3) {
            g_game_state = 4;
            goto switchD_80126370_caseD_80126344;
        }
        goto LAB_80126350;
    case 4: /* 0x8012644c */
        uVar5 = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            uVar5 = 1;
        }
        match_mode_set(uVar5);
        frame_advance_helper();
        gauge_scale_fixed(*(undefined1 *)(_g_system_config_ptr + 0xe),
                     ((((uint)*(byte *)(_g_system_config_ptr + 0xf) * 0x10a) / 100) * 8) / 10);
        iVar3 = scene_entry(0x80010000,0,1);
        if (iVar3 != 0) {
            g_game_state = 5;
        }
        gauge_scale_fixed(*(undefined1 *)(_g_system_config_ptr + 0xe),
                     ((uint)*(byte *)(_g_system_config_ptr + 0xf) << 3) / 10);
        if (g_game_state != 4) goto switchD_80126370_caseD_80126344;
        break;
    case 5: /* 0x80126534 */
        goto switchD_80126370_caseD_80126534;
    case 6: /* 0x801265c8 */
        uVar5 = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            uVar5 = 1;
        }
        match_mode_set(uVar5);
        battle_intro_flow(0);
        if ((g_game_state != 6) || (results_scoreboard(0), g_game_state != 6))
            goto switchD_80126370_caseD_80126344;
        goto LAB_8012661c;
    case 7: /* 0x80126624 */
        uVar5 = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            uVar5 = 1;
        }
        match_mode_set(uVar5);
        bVar2 = 7;
        goto LAB_8012666c;
    case 8: /* 0x80126654 */
        match_mode_set(1);
        /* a0 is the pad-buffer GUEST address (matches clean_table's raw
         * gpr[4] contract and the callee's int param_1). */
        battle_flow_state_machine((int)guest_of(auStack_30));
        bVar2 = 8;
LAB_8012666c:
        if (g_game_state == bVar2) {
            g_game_state = 5;
        }
        goto switchD_80126370_caseD_80126344;
    case 9: /* 0x8012667c */
        match_mode_set(1);
        unlock_progress_scene_entry(0);
        if (DAT_1f80009c != '\x04') {
            if (DAT_1f80009c == '\x05') {
                g_game_state = 0xb;
                g_sub_state = '\x04';
            }
            else {
                player_name_entry(0);
                g_game_state = 5;
            }
            goto switchD_80126370_caseD_80126344;
        }
        break;
    case 11: /* 0x801266c0 */
        match_mode_set(1);
        if (g_sub_state == '\x03') {
            iVar3 = system_config_menu_a();
        }
        else {
            if (g_sub_state != '\x04') goto switchD_80126370_caseD_80126344;
            iVar3 = system_config_menu_b();
        }
        if (iVar3 != 0) {
            g_game_state = 5;
        }
        goto switchD_80126370_caseD_80126344;
    default: /* 10,12,13 -> 0x80126344 re-dispatch (hang) */
        goto switchD_80126370_caseD_80126344;
    }
    g_game_state = 5;
    goto switchD_80126370_caseD_80126344;
switchD_80126370_caseD_80126534:
    uVar5 = 2;
    if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
        uVar5 = 1;
    }
    uVar6 = uVar6 + 1;
    match_mode_set(uVar5);
    display_mode_setup(*(undefined1 *)(_g_system_config_ptr + 0xb));
    frame_tick(0x1f800000);
    memcard_poll_frame();
    frame_advance_helper();
    frame_tick(0x1f800000);
    memcard_poll_frame();
    uVar4 = (uint)g_game_state;
    bVar1 = uVar4 < 0xe;
    if (uVar4 == 5) {
        if (uVar6 < 4) {
            g_game_state = 6;
        }
        else {
LAB_801265b8:
            g_game_state = 2;
        }
        goto switchD_80126370_caseD_80126344;
    }
    goto LAB_80126350;
switchD_80126370_caseD_801263c8:
    uVar4 = (uint)g_game_state;
    uVar6 = 0;
    if (uVar4 == 2) goto LAB_8012661c;
    goto code_r0x8012634c;
LAB_8012661c:
    g_game_state = 3;
    goto switchD_80126370_caseD_80126344;
}

/* ==================================================================
 * boot — boot: subsystem inits + CD file pump.
 * ================================================================== */
void boot(void)
{
    int iVar1;

    cd_dispatch_tblc();
    cd_read_param_set(0);
    FUN_80194478();
    cd_fileop_reset(0);
    set_graph_debug(0);
    gpu_graph_reset(0);
    cd_xchg_bde20(1);
    cd_probe_retry();
    cd_xchg_bc850(0);
    DAT_801c4580 = 0;
    cd_subsys_boot();
    cd_channel_init_seq();
    scratch_zero_reset();
    boot_globals_init(0x1f800000);
    match_mode_set(1);
    DAT_801c4578 = 2;
    DAT_801c457c = (uint)*(uint8_t *)g_DAT_801b12e6;
    gpu_disp_mask_set(0);
    iVar1 = cd_file_pump(0x1f800000,0);
    gfx_tbls_init();
    game_globals_reset();
    sprite_desc_table_init(0);
    print_mode_dispatch_b(2,0,0);
    apply_saved_settings_or_defaults((undefined4 *)_g_system_config_ptr);
    while (-1 < iVar1) {
        iVar1 = cd_file_pump(0x1f800000,iVar1);
        cd_timer_read(0);
    }
    battle_session_arm(0x1f800000,0);
    display_mode_setup(*(undefined1 *)(_g_system_config_ptr + 0xb));
    sprite_rect_table_init(0x40,(_DAT_1f800022 * 0xb) / 0xc,0);
    frame_advance_helper();
    return;
}

/* sprite_rect_table_init — not yet decompiled; extern decl below */

/* ==================================================================
 * cd_file_pump — CD file streaming pump.
 * param_2 = pump state: 0 init -> 0x100 streaming -> 0x110 tail ->
 * 0x120 done -> -1. Each step queues one file's DMA reads.
 * ================================================================== */
int cd_file_pump(int param_1,int param_2)
{
    undefined1 uVar1;
    undefined4 uVar2;
    int iVar3;

    if (param_2 != 0x100) {
        if (0x100 < param_2) {
            if (param_2 != 0x110) {
                if (param_2 != 0x120) {
                    return param_2;
                }
                return -1;
            }
            iVar3 = cd_file_load_sm(0x89,_DAT_801e3cfc);
            if (iVar3 == 0) {
                return 0x110;
            }
            fighter_record_apply(_DAT_801e3cfc,0);
            page_table_ptr_set(0,0);
            return 0x120;
        }
        if (param_2 != 0) {
            return param_2;
        }
        cd_file_load_sm(0xffffffff,0);
        param_2 = 0x100;
        DAT_801c43a0 = 0;
        DAT_801befec = 0xff;
        DAT_801befa6 = 0xff;
        DAT_801c4582 = 0xff;
        DAT_801c4581 = 0xff;
        DAT_801e3d00 = 0xff;
        _DAT_801c47b8 = 0x80010000;
        _DAT_801e3cfc = -0x7fff0000;
    }
    uVar2 = guest_of(tag_table_get());
    iVar3 = cd_file_load_sm(0x80,uVar2);
    uVar1 = DAT_1f800013;
    if (iVar3 != 0) {
        scratch_013_set(0);
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0);
        spr_draw_off(_DAT_801e3cfc,0x3c0,0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),1);
        spr_draw_off(_DAT_801e3cfc,0x380,0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),2);
        spr_draw_off(_DAT_801e3cfc,0x1f0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),3);
        spr_draw_off(_DAT_801e3cfc,0x1e0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),4);
        spr_draw_off(_DAT_801e3cfc,0x1d0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),5);
        spr_draw_off(_DAT_801e3cfc,0x170,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),6);
        spr_draw_off(_DAT_801e3cfc,0x160,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x10);
        spr_draw_off(_DAT_801e3cfc,0x1d0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x11);
        spr_draw_off(_DAT_801e3cfc,0x1f0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x12);
        spr_draw_off(_DAT_801e3cfc,0x1e0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x13);
        spr_draw_off(_DAT_801e3cfc,0x1c0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x14);
        spr_draw_off(_DAT_801e3cfc,0x180,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x15);
        spr_draw_off(_DAT_801e3cfc,400,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x16);
        spr_draw_off(_DAT_801e3cfc,0x1a0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x17);
        spr_draw_off(_DAT_801e3cfc,0x1b0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x18);
        spr_draw_off(_DAT_801e3cfc,0x340,0x100,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x19);
        spr_draw_off(_DAT_801e3cfc,0x200,0x100,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x1a);
        spr_draw_off(_DAT_801e3cfc,0x1c0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),0x1b);
        spr_draw_off(_DAT_801e3cfc,0x1b0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),7);
        spr_draw_pair(_DAT_801e3cfc,0x160,0x1e0,1,0x340,0x1c0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),8);
        spr_draw_off(_DAT_801e3cfc,0x380,0x1c0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(tag_table_get());
        iVar3 = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(uVar2),9);
        spr_draw_off(_DAT_801e3cfc,0x3a0,0x180,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        cd_timer_read(0);
        cd_timer_read(0);
        scratch_013_set(uVar1);
        param_2 = 0x110;
    }
    return param_2;
}

/* small frame/console helpers */
void cd_channel_init_seq(void)
{
    cd_channel_table_init(g_DAT_801befa8,g_DAT_801befca);
    cd_chan_init_wrap();
    dbg_marker_mode_set(0);
}

void scratch_zero_reset(void)
{
    undefined4 *puVar1;

    puVar1 = (undefined4 *)g_DAT_1f800000;
    do {
        *puVar1 = 0;
        puVar1 = puVar1 + 1;
    } while (puVar1 < (undefined4 *)guest_at(0x1f800400));
}

/* display_mode_setup — display mode + screen window setup.
 * param_1 = video mode (0-7 from system config +0xb):
 *   0,2,4,6 -> 0xf0-height variant (interlaced?), odd -> 0x1e0.
 * Sets _DAT_1f800020/22/24 screen geometry, OAM window regs via
 * FUN_80195290/2b0, sprite blocks at 0x801e40a0/0x801e61f0/0x801e40fc/
 * 0x801e624c, then oam_list_rebuild mode commit + hud_sprite_desc_init. */
void display_mode_setup(uint32_t param_1)
{
    int iVar1;
    undefined4 uVar2;

    cd_timer_read(2);
    scratch_013_set(0);
    gpu_graph_reset(1);
    switch(param_1) {
    case 0:
        _DAT_1f800020 = 0x140;
        goto LAB_80127134;
    case 1:
        _DAT_1f800020 = 0x140;
        break;
    case 2:
        _DAT_1f800020 = 0x180;
        goto LAB_80127134;
    case 3:
        _DAT_1f800020 = 0x180;
        break;
    case 4:
        _DAT_1f800020 = 0x200;
        goto LAB_80127134;
    case 5:
        _DAT_1f800020 = 0x200;
        break;
    case 6:
        _DAT_1f800020 = 0x280;
LAB_80127134:
        _DAT_1f800022 = 0xf0;
        DAT_1f800015 = '\0';
        goto LAB_80127160;
    case 7:
        _DAT_1f800020 = 0x280;
        break;
    default:
        _DAT_1f800020 = 0x200;
    }
    _DAT_1f800022 = 0x1e0;
    DAT_1f800015 = '\x01';
LAB_80127160:
    _DAT_1f800024 = 0x168;
    FUN_80195290(_DAT_1f800020 >> 1,(short)_DAT_1f800022 >> 1);
    gte_cw_d000_set((int)_DAT_1f800024);
    *(uint8_t *)g_DAT_801e625e = 1;
    *(uint8_t *)g_DAT_801e410e = 1;
    if (DAT_1f800015 == '\0') {
        sprite_attr_init_a(0x801e40a0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_a(0x801e61f0,0,0xf0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_b(0x801e40fc,0,0xf0,(int)_DAT_1f800020,0);
        uVar2 = 0;
        iVar1 = 0;
    }
    else {
        sprite_attr_init_a(0x801e40a0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_a(0x801e61f0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_b(0x801e40fc,0,2,(int)_DAT_1f800020,(short)_DAT_1f800022 + -2);
        uVar2 = 2;
        iVar1 = (short)_DAT_1f800022 + -2;
    }
    sprite_attr_init_b(0x801e624c,0,uVar2,(int)_DAT_1f800020,iVar1);
    if (DAT_1f800015 == '\0') {
        *(uint16_t *)g_DAT_801e410a = _DAT_1f800022;
        *(uint16_t *)g_DAT_801e625a = _DAT_1f800022;
    }
    else {
        *(uint16_t *)g_DAT_801e410a =
             (short)((((int)((uint)_DAT_1f800022 << 0x10) >> 0x10) -
                     ((int)((uint)_DAT_1f800022 << 0x10) >> 0x1f)) >> 1) - 2;
        *(uint16_t *)g_DAT_801e625a =
             (short)((((int)((uint)_DAT_1f800022 << 0x10) >> 0x10) -
                     ((int)((uint)_DAT_1f800022 << 0x10) >> 0x1f)) >> 1) - 2;
    }
    *(uint16_t *)g_DAT_801e6258 = 0x100;
    *(uint16_t *)g_DAT_801e6256 = 0x18;
    *(uint16_t *)g_DAT_801e6254 = 0;
    *(uint16_t *)g_DAT_801e4108 = 0x100;
    *(uint16_t *)g_DAT_801e4106 = 0x18;
    *(uint16_t *)g_DAT_801e4104 = 0;
    *(uint8_t *)g_DAT_801e6206 = 1;
    *(uint8_t *)g_DAT_801e40b6 = 1;
    *(uint8_t *)g_DAT_801e6207 = 0;
    *(uint8_t *)g_DAT_801e40b7 = 0;
    *(uint8_t *)g_DAT_801e6208 = 1;
    *(uint8_t *)g_DAT_801e40b8 = 1;
    *(uint8_t *)g_DAT_801e40b9 = 0;
    *(uint8_t *)g_DAT_801e40ba = 0;
    *(uint8_t *)g_DAT_801e40bb = 0;
    *(uint8_t *)g_DAT_801e6209 = 0;
    *(uint8_t *)g_DAT_801e620a = 0;
    *(uint8_t *)g_DAT_801e620b = 0;
    *(uint8_t *)g_DAT_801e625c = DAT_1f800015;
    *(uint8_t *)g_DAT_801e410c = DAT_1f800015;
    oam_list_rebuild();
    hud_sprite_desc_init();
    cd_timer_read(0);
    return;
}

void frame_advance_helper(void)
{
    gpu_draw_sync(0);
    cd_timer_read(0);
    frame_counter_bump(1);
    scratch_013_set(1);
}

void scratch_013_set(uint8_t param_1)
{
    DAT_1f800013 = param_1;
}

void gfx_tbls_init(void)
{
    spr_desc_tbl_a_init();
    spr_desc_tbl_b_init();
    spr_tbl_init_a();
    spr_tbl_init_b();
}

void game_globals_reset(void)
{
    g_game_state = 0;
    DAT_1f800001 = 0;
    g_sub_state = 0;
    DAT_1f800003 = 0;
    _DAT_1f800092 = 0;
    _DAT_1f800088 = 0;
    _DAT_1f80008c = 0;
    _DAT_1f80008e = 0;
    _DAT_1f800084 = 0x801bef90;
    flow_cfg_init(0);
    _DAT_1f8000a8 = 0;
    _DAT_1f8000ac = 0;
    _DAT_1f8000a4 = 0;
    _DAT_1f8000a2 = 0;
    DAT_1f8000d0 = 0;
    DAT_1f8000d2 = 0;
    _DAT_1f800080 = 0;
    _DAT_1f800078 = 0;
    _DAT_1f800070 = 0;
    _DAT_1f80006c = 0;
    clear_battle_counters();
    DAT_1f800011 = 0;
    _DAT_1f8000b2 = 0;
    DAT_1f8000b1 = 0;
    DAT_1f8000b0 = 0;
    rec_timer_pair_clear(0x1f800000);
    _g_system_config_ptr = g_DAT_801c1200;
    _DAT_1f8000bc = 0x801f0270;
    _DAT_1f8000b8 = 0;
    _DAT_1f8000cc = 0x801c15f8;
    players_ctrl_init();
    {
        uint32_t uVar1 = u16_lut_lookup(0xf2000002);
        mode_flag_pair_set(uVar1);
    }
    perm_table_load();
    perm_table_apply(0);
    scroll_cfg_set(0,0);
    DAT_801c43a0 = 0;
    DAT_801c11d8 = 0;
    DAT_801c15f4 = 0;
}

uint8_t *tag_table_get(void)
{
    return g_DAT_801c47c0;
}

void ret_stub_b(void)
{
    return;
}

void status_flag_cond_set(uint32_t param_1,int param_2)
{
    if ((param_2 != 0) && (param_2 == 1)) {
        DAT_801c15f0 = 1;
    }
}

void stat_counter_accum(int param_1,int param_2)
{
    /* param_1 is a raw guest address (clean_table passes gpr[4] verbatim);
     * resolve it once — the direct (param_1+off) derefs below would read
     * guest address bits as a host pointer. */
    uint8_t *rec = (uint8_t *)guest_at((uint32_t)param_1);
    short sVar1;
    int iVar2;
    int iVar3;

    iVar3 = (uint)rec[3] * 0x24;
    switch(DAT_1f80009c) {
    default:
        if (param_2 == 0) {
            sVar1 = *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb2));
            *(short *)((char *)g_DAT_801c144c + iVar3) = *(short *)((char *)g_DAT_801c144c + iVar3) + 1;
            *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb2)) = sVar1 + 1;
            iVar2 = (uint)*(byte *)guest_at(*(uint32_t *)(rec + 0x2c) + 3) * 2 + iVar3;
            *(short *)((char *)g_DAT_801c1444 + iVar2 + 0xe) = *(short *)((char *)g_DAT_801c1444 + iVar2 + 0xe) + 1;
        }
        else {
            sVar1 = *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb6));
            *(short *)((char *)g_DAT_801c1448 + iVar3) = *(short *)((char *)g_DAT_801c1448 + iVar3) + 1;
            *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb6)) = sVar1 + 1;
        }
        *(short *)((char *)g_DAT_801c1446 + iVar3) = *(short *)((char *)g_DAT_801c1446 + iVar3) + 1;
        if (*(ushort *)((char *)g_DAT_801c1450 + iVar3) < *(ushort *)(rec + 0xe2)) {
            *(ushort *)((char *)g_DAT_801c1450 + iVar3) = *(ushort *)(rec + 0xe2);
        }
        break;
    case 2:
        break;
    }
    return;
}

/* oam_list_rebuild — mode set: OAM list rebuild for both players */
void oam_list_rebuild(void)
{
    gpu_draw_sync(0);
    cd_timer_read(0);
    gpu_disp_mask_set(0);
    draw_env_timing(0x801e40fc);
    gpu_draw_env_put(0x801e40a0);
    cd_timer_read(0);
    draw_env_timing(0x801e624c);
    gpu_draw_env_put(0x801e61f0);
}

/* battle_session_arm — battle session arm (fighter slot buffers) */
void battle_session_arm(intptr_t param_1,int param_2)
{
    int iVar1;

    param_1 = (intptr_t)guest_at((uint32_t)param_1);
    if (-1 < param_2) {
        iVar1 = param_2 * 0x2150 + -0x7fe1bef0;
        *(undefined2 *)(param_1 + 0xd0) = 0;
        *(undefined2 *)(param_1 + 0xd2) = 0;
        *(undefined1 *)(param_1 + 0x10) = 0;
        _DAT_1f8000d8 = 0;
        _DAT_1f8000e4 = 0;
        *(int *)(param_1 + 0xc) = param_2;
        *(int *)(param_1 + 4) = param_2 * 0x2150 + -0x7fe1bf60;
        *(int *)(param_1 + 8) = iVar1;
        clear_battle_counters();
        if (param_2 == 0) {
            gpu_clear_otag_r(0x801e4110,0x820);
            iVar1 = -0x7fe19da0;
        }
        gpu_clear_otag_r((uint32_t)iVar1,0x820);
    }
}

/* match_end_check — match-end check: pad-start prompt timer.
 * Returns -1 normally; 0xffffff9d (-99) while prompting exit. */
undefined4 match_end_check(intptr_t param_1)
{
    undefined4 uVar1;
    int iVar2;
    int iVar3;

    param_1 = (intptr_t)guest_at((uint32_t)param_1);
    if ((_DAT_1f8000dc >> 4 & 3U) == 0) {
        uVar1 = 0xffffffff;
    }
    else {
        iVar3 = *(short *)(param_1 + 0x22) * 3;
        iVar2 = (uint)*(ushort *)(param_1 + 0x20) << 0x10;
        if (iVar3 < 0) {
            iVar3 = iVar3 + 3;
        }
        hud_text_blk_alt(*(undefined4 *)(param_1 + 8),*(undefined4 *)(param_1 + 0xc),
                     g_DAT_80121e3c,
                     ((((iVar2 >> 0x10) - (iVar2 >> 0x1f)) >> 1) + -0x45) * 0x10000 >> 0x10,
                     (iVar3 << 0xe) >> 0x10,4);
        uVar1 = 0xffffff9d;
    }
    return uVar1;
}

/* memcard_poll_frame — per-frame memcard/save poll */
undefined4 memcard_poll_frame(void)
{
    undefined4 uVar1;

    if (*(char *)(_g_system_config_ptr + 0x22) == '\0') {
        uVar1 = 0xffffffff;
    }
    else {
        uVar1 = 0xffffffff;
        if (DAT_801c15f0 != 0) {
            DAT_801c15f0 = 0;
            uVar1 = memcard_file_flow(1);
        }
    }
    return uVar1;
}

/* ==================================================================
 * frame_tick — frame driver (called 2x/frame: fighter slot toggle).
 * param_1 = battle object guest addr (0x1f800000).
 * +0xc fighter index XORs 0/1 each call -> double-buffered submit.
 * ================================================================== */
void frame_tick(intptr_t param_1)
{
    undefined1 uVar1;
    undefined1 uVar2;
    undefined2 uVar3;
    int iVar4;
    uint uVar5;
    int iVar6;
    int iVar7;
    int iVar8;
    int iVar9;

    param_1 = (intptr_t)guest_at((uint32_t)param_1);
    iVar8 = *(int *)(param_1 + 4);
    iVar9 = *(int *)(param_1 + 8);
    memcard_tick();
    deferred_cmd_drain();
    cursor_sprite_anim((uint8_t *)param_1);
    memcard_slot_poll();
    iVar4 = cd_timer_read(1);
    if (iVar4 < 0xf1) {
        gpu_pkt_unlink_n(iVar9 + 0x207c,0x81f);
    }
    pad_state_refresh(param_1);
    if ((*(char *)(param_1 + 0x15) == '\0') && ((*(ushort *)(param_1 + 0x82) & 1) != 0)) {
        uVar3 = cd_timer_read(1);
        *(undefined2 *)(param_1 + 0x1a) = uVar3;
        gpu_draw_sync(0);
        uVar3 = cd_timer_read(1);
        *(undefined2 *)(param_1 + 0x1c) = uVar3;
        cd_timer_read(*(undefined1 *)(param_1 + 0x10));
    }
    else {
        uVar3 = cd_timer_read(1);
        *(undefined2 *)(param_1 + 0x1c) = uVar3;
        *(undefined2 *)(param_1 + 0x1a) = uVar3;
        cd_timer_read(*(undefined1 *)(param_1 + 0x10));
        gpu_graph_reset(1);
    }
    _DAT_1f8000e0 = _DAT_1f8000e4;
    _DAT_1f8000e4 = 0;
    _DAT_801e8500 = gpu_stat_sign();
    draw_env_timing(iVar8 + 0x5c);
    gpu_draw_env_put(iVar8);
    if (*(byte *)(param_1 + 0x12) != 0) {
        gpu_disp_mask_set(*(byte *)(param_1 + 0x12) - 1);
        *(undefined1 *)(param_1 + 0x12) = 0;
    }
    iVar4 = (int)*(short *)(param_1 + 0xd0);
    while (0 < iVar4) {
        gpu_load_image_cmd(iVar4 * 8 + -0x7fe3bad0,((uint32_t *)g_DAT_801bef38)[iVar4 + -1]);
        iVar4 = iVar4 + -1;
    }
    iVar4 = (int)*(short *)(param_1 + 0xd2);
    *(undefined2 *)(param_1 + 0xd0) = 0;
    if (0 < iVar4) {
        iVar7 = iVar4 * 0x18 + 0x20d8;
        iVar6 = iVar4 * 0x18 + iVar8;
        do {
            uVar5 = iVar8 + iVar7;
            iVar7 = iVar7 + -0x18;
            iVar4 = iVar4 + -1;
            /* iVar6/iVar9 hold GUEST addresses (OT/packet bases stored
             * as 0x801e.... ints) — resolve through guest_at, not a raw
             * host dereference. */
            *(uint *)guest_at((uint32_t)(iVar6 + 0x20d8)) =
                *(uint *)guest_at((uint32_t)(iVar9 + 0x207c)) | 0x5000000;
            iVar6 = iVar6 + -0x18;
            *(uint *)guest_at((uint32_t)(iVar9 + 0x207c)) = uVar5 & 0xffffff;
        } while (0 < iVar4);
    }
    *(undefined2 *)(param_1 + 0xd2) = 0;
    if (*(char *)(param_1 + 0x13) != '\0') {
        iVar4 = *(int *)(param_1 + 0xc) * 0x68;
        if ((0 < *(short *)((char *)g_DAT_801f066e + iVar4)) && (*(char *)(param_1 + 0x9e) == '\0')) {
            uVar1 = *(undefined1 *)guest_at((uint32_t)(iVar4 + -0x7fe0f99b));
            uVar2 = *(undefined1 *)guest_at((uint32_t)(iVar4 + -0x7fe0f99a));
            *(undefined2 *)((char *)g_DAT_801f066a + iVar4) =
                 *(undefined2 *)guest_at((uint32_t)(*(int *)(param_1 + 0xc) * 0x2150 + -0x7fe1bf5e));
            gpu_clear_image_cmd(guest_of((char *)g_DAT_801f0668 + iVar4),
                         (uint32_t)(uintptr_t)*(uint32_t *)((char *)g_DAT_801f0664 + iVar4),uVar1,uVar2);
        }
        gpu_draw_otag(iVar9 + 0x207c);
    }
    table_byte_next_a();
    uVar5 = *(uint *)(param_1 + 0xc) ^ 1;
    iVar4 = uVar5 * 0x2150;
    _DAT_1f8000dc = _DAT_1f8000dc + 1;
    *(uint *)(param_1 + 0xc) = uVar5;
    *(int *)(param_1 + 4) = iVar4 + -0x7fe1bf60;
    *(int *)(param_1 + 8) = iVar4 + -0x7fe1bef0;
    gpu_clear_otag_r(iVar4 + -0x7fe1bef0,0x820);
    *(uint *)guest_at((uint32_t)(iVar4 + -0x7fe1bef0)) = *(uint *)guest_at((uint32_t)(iVar4 + -0x7fe1bef0)) | 0xffffff;
    menu_noop(param_1);
    if (*(char *)guest_at(*(uint32_t *)(param_1 + 200) + 2) < '\0') {
        debug_stat_print();
        ot_packet_chain_fixup();
    }
    scroll_offset_set(0,0);
    return;
}

/* ==================================================================
 * match_loop — match loop (infinite until DAT_1f800001 == 0xff).
 * Stage machine: 0 boot select -> 1 running -> 2 ending ->
 * 0xf0/0xf1 shutdown. 300-frame attract timeout via unaff_s2.
 * ================================================================== */
void match_loop(void)
{
    int iVar1;
    int unaff_s2;
    undefined2 local_88;
    undefined1 local_86;
    undefined1 local_85;
    undefined1 local_84;
    undefined1 local_83;
    undefined2 local_82;
    undefined2 local_80;
    void (*local_7c)(void);
    undefined2 local_78;
    undefined2 local_76;
    undefined2 local_74;
    undefined2 local_72;
    undefined2 local_70;
    undefined2 local_6e;
    undefined2 local_6c;
    undefined2 local_6a;
    undefined4 local_40;
    undefined4 local_3c;
    undefined4 local_38;
    undefined4 local_34;
    undefined4 local_30;
    undefined4 local_2c;
    undefined4 local_28;
    undefined4 local_24;
    undefined4 local_20;

    DAT_1f80008c = 0;
    local_30 = *(undefined4 *)g_DAT_801202e4;
    local_2c = *(undefined4 *)g_DAT_801202e8;
    local_28 = *(undefined4 *)g_DAT_801202ec;
    local_24 = *(undefined4 *)g_DAT_801202f0;
    local_20 = *(undefined4 *)g_DAT_801202f4;
joined_r0x801470b0:
    do {
        if (DAT_1f800001 == 0xff) {
            DAT_1f800001 = 0;
            return;
        }
        if (DAT_1f800001 == 2) {
            scroll_offset_set(0,0);
            if ((_DAT_1f80007c & 0x10) == 0) {
                menu_stepper_state_advance(&local_88);
            }
            else {
                deferred_cmd_post(0x400,0xd,0);
                DAT_1f800001 = 1;
            }
LAB_801472f8:
            spr_slot_pkt_submit(_DAT_1f800008 + 4,0,0xefff0088,0x7c000000);
            spr_slot_pkt_submit(_DAT_1f800008 + 4,0x100,0xefff008a,0x7c400000);
            spr_slot_pkt_submit(_DAT_1f800008 + 4,0xf00000,0xefff0099,0x7c800000);
            spr_slot_pkt_submit(_DAT_1f800008 + 4,0xf00100,0xefff009b,0x7cc00000);
            spr_slot_pkt_submit(_DAT_1f800008,0x1880074,0xf97000f,0x7b1fb810);
            spr_slot_pkt_submit(_DAT_1f800008,0x189010c,0xf7f000f,0x7b1f9038);
            spr_slot_pkt_submit(_DAT_1f800008,0x19800a4,0xfb7000f,0x7b1f9000);
            spr_slot_pkt_submit(_DAT_1f800008,0x1a800a5,0xf39000f,0x7b1fa840);
            spr_slot_pkt_submit(_DAT_1f800008,0x1a800e0,0xf17000f,0x7b1f90e4);
            spr_slot_pkt_submit(_DAT_1f800008,0x1a800fc,0xf5d000f,0x7b1fa87a);
            spr_slot_pkt_submit(_DAT_1f800008,0x11201e4,0x70f000f,0x7b1fb8e0);
        }
        else if (DAT_1f800001 < 3) {
            if (DAT_1f800001 == 0) {
                iVar1 = frame_divisor_get();
                if (iVar1 == 0) {
                    spr_attr_alloc_a(_DAT_1f800008,0,0,(int)_DAT_1f800020,(int)_DAT_1f800022,0,0);
                    iVar1 = attract_stage_load_step(0x1f800000,&local_40);
                    if (iVar1 != 0) {
                        deferred_cmd_post(0x204,0,0);
                        print_mode_dispatch_b(2,0,0);
                        deferred_cmd_post(0x400,0x82,0);
                        /* struct+0xc is a u32 GUEST code address read by
                         * menu_stepper_state_advance -> fn_lookup; storing
                         * a host fn pointer leaks 64-bit host bits. The
                         * guest value is 0x8014751c (attract_menu_dispatch
                         * in fnmap). */
                        local_7c = (void (*)(void))(uintptr_t)0x8014751c;
                        local_82 = 7;
                        local_88 = 0;
                        local_86 = 0;
                        local_85 = 0;
                        local_84 = 1;
                        local_83 = 0;
                        local_80 = 1;
                        local_78 = 0xffff;
                        local_76 = 0xffff;
                        local_74 = 0;
                        local_72 = 0;
                        local_70 = 0;
                        local_6e = 0;
                        local_6c = 0;
                        local_6a = 0;
                        scratch_013_set(0);
                        spr_draw_pair(local_40,0,0x1f0,1,0x200,0,1);
                        spr_draw_pair(local_3c,0,0x1f1,1,0x280,0,1);
                        spr_draw_pair(local_38,0,0x1f2,1,0x240,0x100,1);
                        spr_draw_pair(local_34,0,499,1,0x2c0,0x100,1);
                        cd_timer_read(0);
                        scratch_013_set(1);
                        _DAT_1f80008c = 0;
                        battle_session_arm(0x1f800000,0xffffffff);
                        unaff_s2 = 300;
                        DAT_1f800001 = DAT_1f800001 + 1;
                        goto LAB_801472a4;
                    }
                }
            }
            else if (DAT_1f800001 == 1) {
LAB_801472a4:
                iVar1 = match_end_check(0x1f800000);
                if ((iVar1 < 0) && (unaff_s2 = unaff_s2 + -1, unaff_s2 == 0)) {
                    DAT_1f800001 = 0xf0;
                }
                _DAT_1f80008c = _DAT_1f80008c + 1;
                if ((_DAT_1f80007c & 0x840) != 0) {
                    deferred_cmd_post(0x400,0xc,0);
                    DAT_1f800001 = 2;
                }
                goto LAB_801472f8;
            }
        }
        else if (DAT_1f800001 == 0xf0) {
            deferred_cmd_post(0x204,0,0);
            frame_present(0x1f800000);
            scroll_cfg_set(1,8);
            DAT_1f800001 = DAT_1f800001 + 1;
        }
        else if ((DAT_1f800001 == 0xf1) && (iVar1 = frame_divisor_get(), iVar1 == 0)) {
            DAT_1f800001 = 0xff;
            goto joined_r0x801470b0;
        }
        frame_tick(0x1f800000);
    } while( true );
}

/* ==================================================================
 * scene_entry — scene/stage entry.
 * param_1 = VRAM arena base, param_2 = stage/file-table index,
 * param_3 = demo flag. Loads stage file, sets VRAM windows,
 * streams data, uploads palette/OAM, arms battle, runs 2 frames.
 * Returns 1 on success (state->5), 0 to retry.
 * ================================================================== */
undefined4 scene_entry(int param_1,int param_2,int param_3)
{
    int iVar1;
    int iVar2;
    undefined4 uVar3;
    int iVar4;
    int *piVar5;
    undefined4 *puVar6;
    undefined4 uVar7;
    /* local_48 = sp+0x18 (frame 0x60) -> guest_sp()-0x48;
     * `sb $v0,0x18($sp)` + `addiu $a1,$sp,0x18` in 0x80159028. */
    undefined1 *local_48 = (undefined1 *)guest_at((uint32_t)(guest_sp() - 0x48));
    /* ClearImage rect = sp+0x20..0x27 (frame 0x60) -> guest_sp()-0x40.
     * gpu_clear_image_cmd2 forwards param_1 to generated GPU code as a
     * raw guest address, so the rect must live in the guest frame —
     * host stack locals are invisible to it. */
    uint16_t *local_40 = (uint16_t *)guest_at((uint32_t)(guest_sp() - 0x40));
    /* local_38 = sp+0x28 (frame 0x60) -> guest_sp()-0x38;
     * `swl/swr $t2,0x28($sp)` + `addiu $a0,$sp,0x28` in 0x80159028. */
    undefined4 *local_38 = (undefined4 *)guest_at((uint32_t)(guest_sp() - 0x38));

    uVar7 = 0;
    DAT_801c43a0 = 0;
    DAT_801e3d00 = 0xff;
    _DAT_801f4744 = param_1 + 0x10000;
    _DAT_801f4748 = param_1 + 0x38000;
    _DAT_801f474c = param_1 + 0x60000;
    _DAT_801f4750 = param_1 + 0x67800;
    _DAT_801f4758 = param_1 + 0x6f000;
    iVar1 = param_2 * 0x1c;
    puVar6 = (undefined4 *)guest_at(((uint32_t *)g_PTR_DAT_801b5b94)[(param_2 + 8) * 2]);
    piVar5 = (int *)((char *)g_DAT_801b3ed8 + iVar1);
    _DAT_801f4754 = param_1;
    frame_tick(0x1f800000);
    brightness_packet_send(0x7f);
    scratch_013_set(0);
    cd_timer_read(0);
    _DAT_801f473c = 1;
    if (*piVar5 == 0) {
        iVar2 = *(int *)((char *)g_DAT_801b3ed8 + 0xc + iVar1);
    }
    else {
        iVar2 = (*(int *)((char *)g_DAT_801b3ed8 + 0xc + iVar1) * 3) / 2;
    }
    if (*piVar5 == 0) {
        iVar4 = *(int *)((char *)g_DAT_801b3ed8 + 0xc + iVar1);
    }
    else {
        iVar4 = (*(int *)((char *)g_DAT_801b3ed8 + 0xc + iVar1) * 3) / 2;
    }
    tile_stream_job_init(0x801f4700,iVar2,*(undefined4 *)((char *)g_DAT_801b3ed8 + 0x10 + iVar1),iVar4,
                 *(int *)((char *)g_DAT_801b3ed8 + 0x10 + iVar1) + *(int *)((char *)g_DAT_801b3ed8 + 0x18 + iVar1),piVar5);
    tile_stream_open((uint8_t *)puVar6,(void *)(uintptr_t)0x80159864,piVar5);
    vram_tile_decode(_DAT_801f4758);
    while (iVar2 = (int)blk_io_retry(0x801f4700,piVar5), iVar2 == -1) {
        local_38[0] = *puVar6;
        cd_stream_mode_init(guest_of(local_38));
    }
    _DAT_801f4740 = 0;
    gpu_disp_mask_set(0);
    local_40[0] = 0;
    local_40[1] = 0;
    if (*piVar5 == 0) {
        local_40[2] = *(undefined2 *)((char *)g_DAT_801b3ed8 + 0x14 + iVar1);
    }
    else {
        local_40[2] = (undefined2)
                   ((uint)(*(int *)((char *)g_DAT_801b3ed8 + 0x14 + iVar1) * 3 -
                          (*(int *)((char *)g_DAT_801b3ed8 + 0x14 + iVar1) * 3 >> 0x1f)) >> 1);
    }
    local_40[3] = (undefined2)(*(int *)((char *)g_DAT_801b3ed8 + 0x18 + iVar1) << 1);
    /* param_1 is a raw guest address (the sp+0x20 rect), matching the
     * callee's undefined4 signature — not a host pointer. */
    gpu_clear_image_cmd2((undefined4)(guest_sp() - 0x40),0,0,0);
    do {
        uVar3 = 2;
        if (*piVar5 != 0) {
            uVar3 = 3;
        }
        gpu_pkt_attr_set(*(uint32_t *)guest_at(_DAT_801f4708 * 4 + -0x7fe0b900),uVar3);
        iVar2 = _DAT_801f4732 + -1;
        if (iVar2 < 0) {
            iVar2 = _DAT_801f4732 + 0xe;
        }
        cd_fileop_forward(*(uint32_t *)guest_at(_DAT_801f4714 * 4 + -0x7fe0b8f4),
                     _DAT_801f4730 * 0x10 * ((iVar2 >> 4) + 1) >> 1);
        while (iVar2 = (int)blk_io_retry(0x801f4700,piVar5), iVar2 == -1) {
            iVar2 = cd_file_open_b(guest_of(local_38));
            if ((*(int *)((char *)g_DAT_801b3ed8 + 0x8 + iVar1) < iVar2) || (iVar2 < 1)) {
                local_38[0] = *puVar6;
            }
            cd_stream_mode_init(guest_of(local_38));
        }
        tile_decode_wait(0x801f4700,0);
        display_frame_present(piVar5,0xffffffff);
        if (_DAT_801f4740 == 1) goto LAB_801593a8;
    } while ((param_3 == 0) || ((_DAT_1f80007c & 0x850) == 0));
    uVar7 = 1;
LAB_801593a8:
    brightness_packet_send(0);
    local_48[0] = 0x80;
    _DAT_1f80000c = (uint)(_DAT_1f80000c == 0);
    _DAT_1f800004 = _DAT_1f80000c * 0x2150 + -0x7fe1bf60;
    _DAT_1f800008 = _DAT_1f80000c * 0x2150 + -0x7fe1bef0;
    cd_cmd_issue_sync(0xe,(int)guest_of(local_48),0);
    DAT_801c4580 = 1;
    cd_fileop_dispatch(0);
    cd_subsystem_reset();
    cd_cmd_issue_sync(9,0,0);
    cd_timer_read(0);
    display_mode_setup(*(undefined1 *)(_g_system_config_ptr + 0xb));
    battle_session_arm(0x1f800000,1);
    frame_tick(0x1f800000);
    frame_advance_helper();
    frame_tick(0x1f800000);
    return uVar7;
}

/* ======== 0x8018cfdc ======== */

/* cd_file_load_sm — per-file CD load state machine. The clean port passed
 * piVar2 = guest_at(filetab[..]) — a HOST arena pointer — into
 * cd_read_issue -> cd_mode_flag_set -> cd_cmd_issue(2,..), which is delegated
 * to generated code that reads the SetLoc MSF params treating param_2 as a
 * GUEST address. The host pointer decoded to a bogus address -> SetLoc
 * [0,0,0] -> read lead-in zeros -> game rejected data and retried forever
 * (the Pause->SetLoc[0,0,0]->Setmode->ReadN livelock at LBAs -148/-149/-150).
 * The generated body keeps filetab[..] as a guest address end-to-end and has
 * per-block interrupt checks for the poll loops. Delegate it. */
int cd_file_load_sm(int param_1,uint32_t param_2)

{
  /* file-load dest trace (tag 10): dst=dest arg, src=file_id. Clean-C path
   * bypasses wtrace, so this ring is the only visibility into which file is
   * aimed at which heap cursor before the DMA lands. Dedup consecutive
   * identical (file_id,dest) so the post-corruption cd_file_load_sm retry
   * livelock does not flood the 16K ring and evict the pump decode history. */
  { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    static uint32_t last_f=-1,last_d=-1;
    if((uint32_t)param_1!=last_f||param_2!=last_d){
      last_f=(uint32_t)param_1;last_d=param_2;
      decomp_trace_record(10,param_2,(uint32_t)param_1,0,0);} }
  return (int)br2_call_generated(0x8018cfdc,(uint32_t)param_1,param_2,0,0);
}

/* ======== 0x8012b23c ======== */

void frame_present(intptr_t param_1)

{
  param_1 = (intptr_t)guest_at((uint32_t)param_1);
  undefined1 uVar1;
  undefined1 uVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  
  iVar7 = *(int *)(param_1 + 4);
  memcard_tick();
  deferred_cmd_drain();
  cursor_sprite_anim((uint8_t *)param_1);
  memcard_slot_poll();
  pad_state_refresh(param_1);
  if ((*(char *)(param_1 + 0x15) == '\0') && ((*(ushort *)(param_1 + 0x82) & 1) != 0)) {
    uVar3 = cd_timer_read(1);
    *(undefined2 *)(param_1 + 0x1a) = uVar3;
    gpu_draw_sync(0);
    uVar3 = cd_timer_read(1);
    *(undefined2 *)(param_1 + 0x1c) = uVar3;
    cd_timer_read(*(undefined1 *)(param_1 + 0x10));
  }
  else {
    uVar3 = cd_timer_read(1);
    *(undefined2 *)(param_1 + 0x1c) = uVar3;
    *(undefined2 *)(param_1 + 0x1a) = uVar3;
    cd_timer_read(*(undefined1 *)(param_1 + 0x10));
    gpu_graph_reset(1);
  }
  DAT_1f8000e0 = DAT_1f8000e4;
  DAT_1f8000e4 = 0;
  _DAT_801e8500 = gpu_stat_sign();
  draw_env_timing(iVar7 + 0x5c);
  gpu_draw_env_put(iVar7);
  if (*(byte *)(param_1 + 0x12) != 0) {
    gpu_disp_mask_set(*(byte *)(param_1 + 0x12) - 1);
    *(undefined1 *)(param_1 + 0x12) = 0;
  }
  iVar4 = (int)*(short *)(param_1 + 0xd0);
  while (0 < iVar4) {
    gpu_load_image_cmd((int)(iVar4 * 8 + -0x7fe3bad0),*(undefined4 *)((char *)DAT_801bef38 + (iVar4 + -1) * 4));
    iVar4 = iVar4 + -1;
  }
  *(undefined2 *)(param_1 + 0xd0) = 0;
  if (*(char *)(param_1 + 0x13) != '\0') {
    uVar5 = *(uint *)(param_1 + 0xc) ^ 1;
    iVar4 = uVar5 * 0x68;
    if ((0 < *(short *)((char *)DAT_801f066e + iVar4)) && (*(char *)(param_1 + 0x9e) == '\0')) {
      uVar1 = *(undefined1 *)((char *)g_DAT_801f0664 + iVar4 + 4);
      uVar2 = *(undefined1 *)((char *)g_DAT_801f0664 + iVar4 + 5);
      *(undefined2 *)((char *)g_DAT_801f066a + iVar4) = *(undefined2 *)((char *)g_DAT_801f0664 + iVar4 - 0x42);
      gpu_clear_image_cmd(guest_of((char *)g_DAT_801f0668 + iVar4),((uint32_t *)g_DAT_801f0664)[iVar4 / 4],uVar1,uVar2);
    }
    uVar6 = 0x801e618c;
    if (iVar7 == -0x7fe1bf60) {
      uVar6 = 0x801e82dc;
    }
    gpu_draw_otag(uVar6);
  }
  return;
}

/* task_table_init — task/effect-table init: clears _DAT_801f0738 +
 * DAT_801bef20, runs gp_20_store_default then registers 7 named tables via
 * aux_tbl_store_a/aux_tbl_store_b. */
extern void *guest_at(uint32_t addr);
extern void gp_20_store_default(void);
extern void aux_tbl_store_a(uint32_t,int32_t);
extern void aux_tbl_store_b(uint32_t,int32_t);
void task_table_init(int32_t param_1)
{
  (void)param_1;
  *(uint32_t *)guest_at(0x801f0738) = 0;
  *(uint8_t  *)guest_at(0x801bef20) = 0;
  gp_20_store_default();
  aux_tbl_store_a(0x801c43a8,0);
  aux_tbl_store_b(0x801c44e8,0);
  aux_tbl_store_a(0x801c4448,1);
  aux_tbl_store_a(0x801c4498,2);
  aux_tbl_store_a(0x801c4420,3);
  aux_tbl_store_b(0x801c4470,1);
  aux_tbl_store_b(0x801c44c0,2);
  aux_tbl_store_b(0x801c4434,3);
}

/* intro_movie_state — intro/movie playback state machine. State byte
 * _DAT_1f80008c: -1 idle; 0 init; 0x10..0x30 staged file loads via
 * cd_file_load_sm(file_id, heap=_DAT_801e3cfc); stream_block_decode advances
 * frames; param_1 selects which queued load to start (0/1/2). */
int32_t intro_movie_state(int32_t param_1)
{
  uint8_t uVar1;
  int32_t iVar2, iVar3;
  int16_t sVar4;
  sVar4 = *(int16_t *)g_DAT_1f80008c;
  if (sVar4 < 0) return -1;
  if (sVar4 != 0x10) {
    if (0x10 < sVar4) {
      if (sVar4 == 0x20) {
        iVar2 = cd_file_load_sm(0x8b,(uint32_t)_DAT_801e3cfc);
        if (iVar2 == 0) { *(int16_t *)g_DAT_1f80008c = sVar4; return 0; }
        sVar4 = 0x30;
        fighter_record_apply(_DAT_801e3cfc,1);
        page_table_ptr_set(1,1);
        cd_timer_read(0);
      } else {
        sVar4 = *(int16_t *)g_DAT_1f80008c;
        if (sVar4 != 0x30) return 0;
      }
      iVar2 = cd_file_load_sm(0x16,(uint32_t)_DAT_801e3cfc);
      uVar1 = DAT_1f800013;
      if (iVar2 == 0) { *(int16_t *)g_DAT_1f80008c = sVar4; return 0; }
      scratch_013_set(0);
      spr_draw_off(_DAT_801e3cfc,0,0x1e0,1);
      goto LAB_8012c330;
    }
    if (sVar4 != 0) return 0;
    cd_file_load_sm(-1,0);
    sVar4 = 0x10;
    _DAT_801c43a0 = 0;
    DAT_801e3d00 = 0xff;
    _DAT_801c47b8 = (uint32_t)-0x7fff0000;
    _DAT_801e3cfc = 0x800d0000;
  }
  if (param_1 == 1) {
    iVar2 = cd_file_load_sm(0x14,(uint32_t)_DAT_801e3cfc);
    if (iVar2 != 0) {
      iVar2 = 0;
      do {
        iVar3 = stream_block_decode((uint32_t)_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),iVar2);
        iVar2++;
        _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)iVar3;
      } while (iVar2 < 0xb);
      sVar4 = 0x20;
    }
  } else if (param_1 < 2) {
    if (param_1 == 0 &&
        (iVar2 = cd_file_load_sm(0x14,(uint32_t)_DAT_801e3cfc),
         uVar1 = DAT_1f800013, iVar2 != 0)) {
      scratch_013_set(0);
      uint8_t *ctr = (uint8_t *)guest_at(0x801b1408);
      *ctr = *ctr + 1;
      if (10 < *ctr) *ctr = 0;
      iVar2 = stream_block_decode(_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),*ctr);
      spr_draw_off((int)_DAT_801c47b8,0x3c0,0x100,1);
      _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)iVar2;
LAB_8012c330:
      cd_timer_read(0);
      scratch_013_set(uVar1);
      *(int16_t *)g_DAT_1f80008c = -1;
      return 1;
    }
  } else if (param_1 == 2 &&
             (iVar2 = cd_file_load_sm(0x15,(uint32_t)_DAT_801e3cfc), iVar2 != 0)) {
    iVar2 = 0;
    do {
      iVar3 = stream_block_decode((uint32_t)_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),iVar2);
      iVar2++;
      _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)iVar3;
    } while (iVar2 < 0x17);
    sVar4 = 0x20;
  }
  *(int16_t *)g_DAT_1f80008c = sVar4;
  return 0;
}

/* ---- batch13: menu stepper helpers + HUD/text/sprite helpers ----
 * Guest-faithful ports verified against psx_payload.bin disasm.
 * Text-draw helpers all poke the 0x1f80038c u16 block then call
 * text_render_full/text_render_simple with the block's host pointer. */

#define BR2B8(a)   (*(uint8_t  *)guest_at(a))
#define BR2B16(a)  (*(uint16_t *)guest_at(a))
#define BR2B32(a)  (*(uint32_t *)guest_at(a))
#define BR2B16S(a) (*(int16_t  *)guest_at(a))

extern int      sprintf_core(char *,char *,...);
extern int32_t   text_win_printf(int32_t,uint32_t,uint32_t,uint32_t,...);
extern uint32_t guest_of(const void *);
extern undefined4 text_render_full(uint *,uint,int);
extern undefined4 text_render_simple(uint *,uint,int);
extern void     prim_packet_init_tex(int,int,int,uint,byte *);
extern void    *spr_desc_slot_alloc(void);

/* Varargs forwarders: the guest dispatchers pass fmt + 8 raw 32-bit
 * varargs straight through; C cannot forward ..., so collect and
 * re-emit with fixed arity (same convention as sprintf_core's
 * va_arg(ap,uint32_t)). */
static void text9_dbg(const char *fmt,const uint32_t *v)
{
  /* text_win_printf takes guest u32 args; fmt is a host pointer into
   * arena/scratch, so recover its guest address. */
  text_win_printf((int32_t)guest_of(fmt),(int)v[0],(int)v[1],(int)v[2],
               (int)v[3],(int)v[4],(int)v[5],(int)v[6],(int)v[7]);
}
static int text9_spr(uint32_t buf,const char *fmt,const uint32_t *v)
{
  /* buf is a guest address; sprintf_core takes a host buffer pointer. */
  return sprintf_core((char *)guest_at(buf),(char *)fmt,(int)v[0],(int)v[1],(int)v[2],
                      (int)v[3],(int)v[4],(int)v[5],(int)v[6],(int)v[7]);
}

/* print_dispatch — localized print dispatcher on mode @0x1f800384:
 * 1 = debug printf, 2 = sprintf into stack buf + queue text draw via
 * text_render_simple on the 0x1f80038c block, else nothing. */
void print_dispatch(const char *param_1,...)
{
  __builtin_va_list ap;
  uint32_t v[8];
  /* buf = sp+0x28 (frame 0x130) -> guest_sp()-0x108;
   * `addiu $a0/$v0,$sp,0x28` in 0x8012cc6c (stored to block+0x1c). */
  uint8_t *buf = (uint8_t *)guest_at((uint32_t)(guest_sp() - 0x108));
  uint16_t *b;
  uint32_t mode = BR2B32(0x1f800384);
  int i;
  __builtin_va_start(ap,param_1);
  for (i = 0; i < 8; i++) v[i] = (uint32_t)__builtin_va_arg(ap,uint32_t);
  __builtin_va_end(ap);
  if (mode == 1) { text9_dbg(param_1,v); return; }
  if ((int32_t)mode < 2) return;
  if (mode != 2) return;
  text9_spr(guest_of(buf),param_1,v);
  b = (uint16_t *)guest_at(0x1f80038c);
  b[3] = 0x1000; b[4] = 0x1000;
  b[5] = 8; b[6] = 8; b[7] = 8; b[8] = 8;
  b[0] = 0; b[1] = 0; b[2] = 0; b[9] = 0; b[10] = 0;
  b[11] = 0x78df; b[13] = 0;
  *(uint32_t *)(b + 14) = guest_of(buf);   /* +0x1c */
  text_render_simple((uint *)guest_at(BR2B32(0x1f800008)),
               BR2B16(0x1f80000c),(int)guest_of(b));
}

/* print_dispatch_selfdesc — same dispatcher but self-describing: mode 2 copies
 * caller +0x10/+0x12 u16 into block +2/+4 before the draw. */
void print_dispatch_selfdesc(void *param_1,const char *param_2,...)
{
  __builtin_va_list ap;
  uint32_t v[8];
  /* buf = sp+0x28 (frame 0x138) -> guest_sp()-0x110;
   * `addiu $a0/$v0,$sp,0x28` in 0x8012cf64 (stored to block+0x1c). */
  uint8_t *buf = (uint8_t *)guest_at((uint32_t)(guest_sp() - 0x110));
  uint16_t *b;
  uint8_t *self = (uint8_t *)param_1;
  uint32_t mode = BR2B32(0x1f800384);
  int i;
  __builtin_va_start(ap,param_2);
  for (i = 0; i < 8; i++) v[i] = (uint32_t)__builtin_va_arg(ap,uint32_t);
  __builtin_va_end(ap);
  if (mode == 1) { text9_dbg(param_2,v); return; }
  if ((int32_t)mode < 2) return;
  if (mode != 2) return;
  text9_spr(guest_of(buf),param_2,v);
  b = (uint16_t *)guest_at(0x1f80038c);
  b[0] = 0;
  b[1] = *(uint16_t *)(self + 0x10);
  b[2] = *(uint16_t *)(self + 0x12);
  b[3] = 0x1000; b[4] = 0x1000;
  b[5] = 8; b[6] = 8; b[7] = 8; b[8] = 8;
  b[11] = 0x78df; b[9] = 0; b[10] = 0;
  *(uint32_t *)(b + 14) = guest_of(buf);   /* +0x1c */
  b[12] = 0;
  text_render_simple((uint *)guest_at(BR2B32(0x1f800008)),
               BR2B16(0x1f80000c),(int)guest_of(b));
}

/* menu_row_paint / menu_row_paint_b — menu cursor row painters: emit "  > "
 * or "    " through print_dispatch_selfdesc when stepper byte+4 is clear. */
void menu_row_paint(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  if (p[4] == 0)
    print_dispatch_selfdesc(param_1,(const char *)guest_at(0x8011ec20));
}
void menu_row_paint_b(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  if (p[4] == 0)
    print_dispatch_selfdesc(param_1,(const char *)guest_at(0x8011ec28));
}

/* menu_stepper_redraw — menu stepper redraw: cb(+0xc) with 0x100, then for
 * i = 1..h(+6) paint row i (menu_row_paint when i == sel(+8) else
 * menu_row_paint_b) and cb with 0x100+i, finally cb with 0x1ff. */
void menu_stepper_redraw(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  void (*cb)(void *,uint32_t);
  uint32_t i;
  /* p+0xc is a u32 GUEST code address (same field menu_stepper_state_advance
   * resolves via fn_lookup) — not a host pointer. Resolve it the same way. */
  cb = (void (*)(void *,uint32_t))fn_lookup(*(uint32_t *)(p + 0xc));
  if (cb == NULL) return;
  cb(param_1,0x100);
  i = 1;
  for (;;) {
    if (i == *(uint16_t *)(p + 8)) menu_row_paint(param_1);
    else                            menu_row_paint_b(param_1);
    cb(param_1,(i + 0x100) & 0xffff);
    i++;
    if (*(uint16_t *)(p + 6) < i) break;
  }
  cb(param_1,0x1ff);
}

/* menu_stepper_input — menu stepper input: pad flags @0x1f80007e; byte+5
 * selects button pair (0: 0x4000/0x1000 sfx 0xa, else 0x2000/0x8000
 * sfx 0xb). Up wraps cur(+8) to 1 past max(+6); down wraps 1 -> max. */
void menu_stepper_input(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  uint16_t cur,max;
  if (p[5] == 0) {
    if (BR2B16(0x1f80007e) & 0x4000) {
      deferred_cmd_post(0x400,0xa,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur < max) ? (uint16_t)(cur + 1) : (uint16_t)1;
    }
    if (BR2B16(0x1f80007e) & 0x1000) {
      deferred_cmd_post(0x400,0xa,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur == 1) ? max : (uint16_t)(cur - 1);
    }
  } else {
    if (BR2B16(0x1f80007e) & 0x2000) {
      deferred_cmd_post(0x400,0xb,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur < max) ? (uint16_t)(cur + 1) : (uint16_t)1;
    }
    if (BR2B16(0x1f80007e) & 0x8000) {
      deferred_cmd_post(0x400,0xb,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur == 1) ? max : (uint16_t)(cur - 1);
    }
  }
}

/* hud_text_a — HUD text variant A (block +0x16 attr +0x1f). */
void hud_text_a(undefined4 param_1,undefined2 param_2,const void *param_3,
                  undefined2 param_4,undefined2 param_5,short param_6)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(param_3);
  b[3] = (uint16_t)param_4;
  b[5] = 8; b[7] = 0xffff;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[6] = 0x10; b[8] = 0x10; b[9] = 0; b[10] = 1; b[12] = 1;
  b[4] = (uint16_t)param_5;
  b[11] = (uint16_t)((((uint32_t)param_6 + 0x1e0) & 0x1ff) * 0x40 + 0x1f);
  text_render_full((uint *)guest_at(param_1),(uint)param_2 & 0xffff,
               (int)guest_of(b));
}

/* hud_text_b — HUD text variant B (uniform 0xf metrics, +0x16). */
void hud_text_b(undefined4 param_1,undefined4 param_2,const void *param_3,
                  undefined4 param_4,undefined2 param_5,short param_6)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(param_3);
  b[5] = 0xf; b[6] = 0xf; b[7] = 0xf; b[8] = 0xf;
  b[9] = 1; b[10] = 1;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[3] = (uint16_t)param_4;
  b[12] = 4;
  b[4] = (uint16_t)param_5;
  b[11] = (uint16_t)((((uint32_t)param_6 + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  text_render_full((uint *)guest_at(param_1),(uint)param_2 & 0xffff,
               (int)guest_of(b));
}

/* hud_text_c — HUD text variant C (0x1e line gap, +0x16). */
void hud_text_c(undefined4 param_1,undefined4 param_2,const void *param_3,
                  undefined4 param_4,undefined2 param_5,undefined4 param_6)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(param_3);
  b[3] = (uint16_t)param_4;
  b[6] = 0x1e;
  b[9] = 1; b[10] = 1;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[5] = 0xf; b[7] = 0xf; b[8] = 0xf;
  b[12] = 4;
  b[4] = (uint16_t)param_5;
  b[11] = (uint16_t)((((uint32_t)param_6 + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  text_render_full((uint *)guest_at(param_1),(uint)param_2 & 0xffff,
               (int)guest_of(b));
}

/* hud_text_d — HUD text variant D (metric 8, flag 5, +0x16). */
void hud_text_d(uint32_t param_1,uint32_t param_2,const void *param_3,
                  int32_t param_4,int32_t param_5,int32_t param_6)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(param_3);
  b[3] = (uint16_t)param_4;
  b[7] = 0xffff;
  b[9] = 1; b[10] = 1;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[5] = 8; b[6] = 8; b[8] = 8;
  b[12] = 5;
  b[4] = (uint16_t)param_5;
  b[11] = (uint16_t)((((uint32_t)param_6 + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  text_render_full((uint *)guest_at(param_1),param_2 & 0xffff,
               (int)guest_of(b));
}

/* sprite_node_alloc — sprite-node pool alloc: 8 slots of 0x34 at
 * 0x801f5d38, generation counter @0x1f8000dc, cursor @0x801b55e8.
 * Returns slot+4 host pointer or NULL when generation matches. */
void *sprite_node_alloc(void)
{
  int32_t v1 = (int32_t)BR2B32(0x801b55e8);
  uint32_t gen = BR2B32(0x1f8000dc);
  uint8_t *slot;
  if (v1 < 0) v1 = 0;
  slot = (uint8_t *)guest_at(0x801f5d38 + (uint32_t)v1 * 0x34);
  if (*(uint32_t *)slot == gen || *(uint32_t *)slot == gen - 1)
    return (void *)0;
  v1++;
  if (v1 >= 8) v1 = 0;
  BR2B32(0x801b55e8) = (uint32_t)v1;
  *(uint32_t *)slot = gen;
  return slot + 4;
}

/* sprite_node_emit_a — sprite-list node emit (pool sprite_node_alloc, tag
 * 0x08000000, packet tail +0x24). 10 args; param_10 selects GPU
 * command variant 0x3a/0x38000000 and stride. */
int sprite_node_emit_a(uint32_t param_1,int32_t param_2,int32_t param_3,
                 int32_t param_4,int32_t param_5,uint32_t param_6,
                 uint32_t param_7,uint32_t param_8,uint32_t param_9,
                 int32_t param_10)
{
  uint32_t *head = (uint32_t *)guest_at(param_1);
  uint8_t *node = (uint8_t *)sprite_node_alloc();
  uint32_t guest;
  int32_t tag;
  if (node == (uint8_t *)0) return -1;
  guest = guest_of(node);
  *(uint32_t *)(node + 0x0c) = param_7;
  *(uint32_t *)(node + 0x14) = param_8;
  *(uint16_t *)(node + 0x08) = (uint16_t)param_2;
  *(uint16_t *)(node + 0x0a) = (uint16_t)param_3;
  *(uint16_t *)(node + 0x10) = (uint16_t)(param_2 + param_4);
  *(uint16_t *)(node + 0x12) = (uint16_t)param_3;
  *(uint16_t *)(node + 0x18) = (uint16_t)param_2;
  *(uint16_t *)(node + 0x1a) = (uint16_t)(param_3 + param_5);
  *(uint32_t *)(node + 0x1c) = param_9;
  *(uint16_t *)(node + 0x20) = (uint16_t)(param_2 + param_4);
  *(uint16_t *)(node + 0x22) = (uint16_t)(param_3 + param_5);
  if (param_10 != 0) {
    *(uint32_t *)(node + 4) = param_6 | 0x3a000000;
    tag = (param_10 - 1) << 5;
  } else {
    *(uint32_t *)(node + 4) = param_6 | 0x38000000;
    tag = 0;
  }
  prim_packet_init_tex((int)guest_of(node + 0x24),
               BR2B8(0x801e40b7),BR2B8(0x801e40b6),(uint)tag,(byte *)0);
  *(uint32_t *)node = *head | 0x08000000;
  *head = guest & 0xffffff;
  *(uint32_t *)(node + 0x24) = (guest & 0xffffff) | 0x02000000;
  *head = (guest + 0x24) & 0xffffff;
  return 0;
}

/* sprite_node_emit_b — sprite-list node emit (pool spr_desc_slot_alloc, tag
 * 0x04000000, packet tail +0x14). param_2 packs x|y<<16,
 * param_3 packs w|h<<16; param_5 low byte = colour, param_6 picks
 * GPU variant 0x66/0x64000000 + stride. */
int sprite_node_emit_b(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                 uint32_t param_4,int32_t param_5,int32_t param_6)
{
  uint32_t *head = (uint32_t *)guest_at(param_1);
  uint8_t *node = (uint8_t *)spr_desc_slot_alloc();
  uint32_t guest;
  int32_t tag;
  if (node == (uint8_t *)0) return -1;
  guest = guest_of(node);
  *(uint16_t *)(node + 0x10) = (uint16_t)((((int32_t)param_3 >> 16) & 0xff) + 1);
  *(uint16_t *)(node + 0x12) = (uint16_t)(((uint32_t)param_3 >> 24) + 1);
  *(uint32_t *)(node + 0x0c) = param_4;
  *(uint16_t *)(node + 0x08) = (uint16_t)param_2;
  *(uint16_t *)(node + 0x0a) = (uint16_t)(param_2 >> 16);
  node[7] |= 1;
  if (param_6 != 0) {
    uint32_t c = param_5 & 0xff;
    *(uint32_t *)(node + 4) = (c | (c << 8) | (c << 16)) | 0x66000000;
    tag = (param_6 - 1) << 5;
  } else {
    uint32_t c = param_5 & 0xff;
    *(uint32_t *)(node + 4) = (c | (c << 8) | (c << 16)) | 0x64000000;
    tag = 0;
  }
  prim_packet_init_tex((int)guest_of(node + 0x14),
               BR2B8(0x801e40b7),BR2B8(0x801e40b6),
               (uint32_t)((int32_t)(param_3 << 16) >> 16) | (uint32_t)tag,
               (byte *)0);
  *(uint32_t *)node = *head | 0x04000000;
  *head = guest & 0xffffff;
  *(uint32_t *)(node + 0x14) = (guest & 0xffffff) | 0x02000000;
  *head = (guest + 0x14) & 0xffffff;
  return 0;
}


/* frame_counter_bump — bump byte counter @0x1f800012 by arg+1. Disasm-verified. */
void frame_counter_bump(int param_1)
{
  *(uint8_t *)guest_at(0x1f800012) = (uint8_t)(param_1 + 1);
}

/* clear_battle_counters — clear battle-side counters @0x1f800068/74 (u32) and
 * @0x1f80007c/7e (u16). Disasm 0x8012755c-0x80127570. */
void clear_battle_counters(void)
{
  *(uint16_t *)guest_at(0x1f80007e) = 0;
  *(uint16_t *)guest_at(0x1f80007c) = 0;
  *(uint32_t *)guest_at(0x1f800074) = 0;
  *(uint32_t *)guest_at(0x1f800068) = 0;
}

/* u16_lut_lookup — u16 lookup: idx=a0&0xffff; <3 returns lhu at
 * *(u32*)0x801b66e4 + idx*0x10, else 0. Disasm 0x8018e3bc-0x8018e3f0. */
uint32_t u16_lut_lookup(uint32_t param_1)
{
  uint32_t idx = param_1 & 0xffff;
  uint32_t tbl;
  if (idx >= 3) return 0;
  tbl = *(uint32_t *)guest_at(0x801b66e4);
  return *(uint16_t *)guest_at(tbl + idx * 0x10);
}

/* set_graph_debug — SetGraphDebug level setter: byte @0x801b8322 = arg;
 * when nonzero calls PTR_FUN_801b831c (text_win_printf text-window printf)
 * with "SetGraphDebug:level:%d,type:%d reverse:%d". Returns old byte.
 * Disasm 0x80190f74-0x80190fc8. */
extern void *PTR_FUN_801b831c;
int32_t set_graph_debug(uint32_t param_1)
{
  uint8_t *flag = (uint8_t *)guest_at(0x801b8322);
  int32_t old = flag[0];
  int (*cb)(int32_t,uint32_t,uint32_t,uint32_t,...) =
      (int (*)(int32_t,uint32_t,uint32_t,uint32_t,...))PTR_FUN_801b831c;
  flag[0] = (uint8_t)param_1;
  if ((param_1 & 0xff) == 0) return old;
  cb(0x80125444,flag[0],flag[-2],flag[1]);
  return old;
}

/* memcard_info_query — memcard/info query: mc_data_read(0, tbl[0..2],
 * tbl[3..9], 0x801c11e0, 0x414) over table @0x801b45f4; ret 0 clears
 * @0x801c15f0 and returns byte @(*(u32*)0x1f8000c0)+0x22, ret<8&&!=0
 * ->1, ret>=8 ->0. Disasm 0x8015f590-0x8015f63c. */
extern int32_t mc_data_read(int32_t,uint32_t,uint32_t,uint32_t,uint32_t,
                            uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,
                            uint32_t,uint32_t,uint32_t);
int32_t memcard_info_query(void)
{
  uint32_t *t = (uint32_t *)guest_at(0x801b45f4);
  int32_t r;
  if (t == NULL) return -1;
  r = mc_data_read(0,t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],
                   0x801c11e0,0x414);
  if (r >= 8) return 0;
  if (r != 0) return 1;
  *(uint32_t *)guest_at(0x801c15f0) = 0;
  return *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x1f8000c0) + 0x22);
}

/* debug_stat_print — debug stat print: scroll_offset_set(0,0xdc) then
 * print_dispatch("CPU(%d)\nGPU(%d)\n" or "#2CPU(%d)\nGPU(%d)\n",
 * u16@0x1f80001a, u16@0x1f80001c) selected by u16@0x801e83a8.
 * Disasm 0x8012a088-0x8012a0e8. */
void debug_stat_print(void)
{
  uint16_t a1 = *(uint16_t *)guest_at(0x1f80001a);
  uint16_t a2 = *(uint16_t *)guest_at(0x1f80001c);
  scroll_offset_set(0,0xdc);
  if (*(uint16_t *)guest_at(0x801e83a8) == 0)
    print_dispatch((const char *)guest_at(0x8011ed40),a1,a2);
  else
    print_dispatch((const char *)guest_at(0x8011ed54),a1,a2);
}

/* players_ctrl_init — two-player control-block init: fills guest structs at
 * 0x1f8000e8 / 0x1f8001f0 and descriptor tables 0x801f5258/0x801f52a0
 * with ptr pairs into 0x801e1b3c region. Disasm 0x801757b8-0x80175890. */
void players_ctrl_init(void)
{
  uint8_t *pa = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *pb = (uint8_t *)guest_at(0x1f8001f0);
  uint8_t *ta = (uint8_t *)guest_at(0x801f5258);
  uint8_t *tb = ta + 0x48;
  const uint32_t v1 = 0x801e1b3c;
  if (!pa || !pb || !ta) return;
  pa[2] = 0; pa[3] = 0;
  *(uint32_t *)(pa + 0x28) = 0x801f5258;
  *(uint32_t *)(ta + 0x3c) = v1 + 0x870;
  *(uint32_t *)(ta + 0x1c) = v1 - 0xbd0;
  *(uint32_t *)(ta + 0x40) = v1 - 0x8dc;
  *(uint32_t *)(ta + 0x18) = v1;
  *(uint32_t *)(pa + 0x44) = v1 - 0x1950;
  *(uint32_t *)(pa + 0x48) = v1 - 0x1290;
  *(uint32_t *)(pa + 0x2c) = 0x1f8001f0;
  *(uint32_t *)(pa + 0x38) = 0x801f5328;
  *(uint32_t *)(pa + 0x3c) = 0x801f55c0;
  pb[2] = 1; pb[3] = 5;
  *(uint32_t *)(pb + 0x28) = 0x801f52a0;
  *(uint32_t *)(tb + 0x18) = v1 + 0x10e0;
  *(uint32_t *)(tb + 0x3c) = v1 + 0x1950;
  *(uint32_t *)(tb + 0x1c) = v1 - 0x5e8;
  *(uint32_t *)(tb + 0x40) = v1 - 0x2f4;
  *(uint32_t *)(pb + 0x44) = v1 - 0x15f0;
  *(uint32_t *)(pb + 0x48) = v1 - 0xf30;
  *(uint32_t *)(pb + 0x2c) = 0x1f8000e8;
  *(uint32_t *)(pb + 0x38) = 0x801f5440;
  *(uint32_t *)(pb + 0x3c) = 0x801f5704;
  *(uint8_t *)guest_at(0x1f800098) = 0;
}

/* ot_packet_chain_fixup — OT/GPU packet-chain fixup: positions two 0x18-byte
 * records at 0x801e83e8/0x801e83d0 from u16 fields + scroll offsets,
 * then links five slots at 0x801e8348..0x801e8388 plus tail slots
 * 0x801e8398/0x801e83d0/0x801e83b8: each slot gets (*T1 | tag) and
 * *T1 becomes the slot's 24-bit address. T1 = *(u32*)0x1f800008.
 * Disasm 0x8012a30c-0x8012a4b0. */
void ot_packet_chain_fixup(void)
{
  uint32_t h;
  uint32_t t1 = *(uint32_t *)guest_at(0x1f800008);
  static const uint32_t offs[5] = {0x10,0x20,0x30,0x40,0x50};
  int i;
  uint8_t *A = (uint8_t *)guest_at(0x801e83e8);
  uint8_t *A2 = (uint8_t *)guest_at(0x801e83d0);
  if (A == NULL || A2 == NULL) return;
  h = (uint32_t)*(uint16_t *)(A + 8) + *(uint16_t *)guest_at(0x1f80001a);
  *(uint16_t *)(A + 0x14) = (uint16_t)h;
  *(uint16_t *)(A + 0xc)  = (uint16_t)h;
  h = (uint32_t)*(uint16_t *)(A2 + 8) + *(uint16_t *)guest_at(0x1f80001c);
  *(uint16_t *)(A2 + 0x14) = (uint16_t)h;
  *(uint16_t *)(A2 + 0xc)  = (uint16_t)h;
  h = (uint32_t)*(uint16_t *)(A2 + 8) + *(uint16_t *)guest_at(0x1f80001e);
  *(uint16_t *)guest_at(0x801e83a4) = (uint16_t)h;
  *(uint16_t *)guest_at(0x801e83a0) = (uint16_t)h;
  for (i = 0; i < 5; i++) {
    uint32_t slot = 0x801e8398 - offs[i];
    uint32_t v = *(uint32_t *)guest_at(t1) | 0x3000000;
    *(uint32_t *)guest_at(slot) = v;
    *(uint32_t *)guest_at(t1) = slot & 0xffffff;
  }
  {
    uint32_t slot = 0x801e8398;
    *(uint32_t *)guest_at(slot) = *(uint32_t *)guest_at(t1) | 0x3000000;
    *(uint32_t *)guest_at(t1) = slot & 0xffffff;
  }
  {
    uint32_t slot = 0x801e83d0;
    *(uint32_t *)guest_at(slot) = *(uint32_t *)guest_at(t1) | 0x5000000;
    *(uint32_t *)guest_at(t1) = slot & 0xffffff;
  }
  {
    uint32_t slot = 0x801e83b8;
    *(uint32_t *)guest_at(slot) = *(uint32_t *)guest_at(t1) | 0x5000000;
    *(uint32_t *)guest_at(t1) = slot & 0xffffff;
  }
}

/* sprite_rect_table_init — sprite/UV rect table init: spr_prim_tag_a x3 over
 * 0x18-stride recs @0x801e83b8, fills three textured-quad descriptors
 * (u16 pos/uv fields from args, byte tags 0x28/0xc8), spr_prim_tag_b x6
 * over 0x10-stride recs @0x801e8348, then six flat-shade quads.
 * Disasm 0x8012a0f0-0x8012a304. */
extern void spr_prim_tag_a(uint32_t);
extern void spr_prim_tag_b(uint32_t);
static void sh16(uint32_t a,uint32_t o,uint32_t v)
{ *(uint16_t *)((uint8_t *)guest_at(a) + o) = (uint16_t)v; }
static void sb8(uint32_t a,uint32_t o,uint32_t v)
{ *(uint8_t *)((uint8_t *)guest_at(a) + o) = (uint8_t)v; }
void sprite_rect_table_init(int32_t param_1,int32_t param_2,int32_t param_3)
{
  uint32_t s3 = (uint32_t)param_1, s2 = (uint32_t)param_2;
  (void)param_3;
  uint32_t v1 = 0x801e83b8, a1 = v1 + 0x18, a0 = v1 + 0x30;
  uint32_t b, i;
  for (i = 0; i < 3; i++) spr_prim_tag_a(v1 + i * 0x18);
  /* three 0x18 records */
  sh16(v1,0x12,s2+0xf);  sh16(v1,0x16,s2+0xf);
  sh16(v1,0x08,s3);      sh16(v1,0x0a,s2);
  sh16(v1,0x0c,s3+0xff); sh16(v1,0x0e,s2);
  sh16(v1,0x10,s3);      sh16(v1,0x14,s3+0xff);
  sh16(a1,0x0a,s2+9);    sh16(a1,0x0e,s2+9);
  sh16(a1,0x12,s2+0xe);  sh16(a1,0x16,s2+0xe);
  sh16(a1,0x08,s3);      sh16(a1,0x0c,s3+0xff);
  sh16(a1,0x10,s3);      sh16(a1,0x14,s3+0xff);
  sh16(a0,0x0a,s2+1);    sh16(a0,0x0e,s2+1);
  sh16(a0,0x12,s2+6);    sh16(a0,0x16,s2+6);
  sh16(a0,0x08,s3);      sh16(a0,0x0c,s3+0xff);
  sh16(a0,0x10,s3);      sh16(a0,0x14,s3+0xff);
  sb8(v1,4,0x28); sb8(v1,5,0x28); sb8(v1,6,0x28);
  sb8(a1,4,0xc8); sb8(a1,5,0xc8); sb8(a1,6,0);
  sb8(a0,4,0xc8); sb8(a0,5,0);    sb8(a0,6,0);
  for (i = 0; i < 6; i++) spr_prim_tag_b(0x801e8348 + i * 0x10);
  /* six 0x10 records */
  {
    static const uint32_t xs[6] = {0xfa,0xc8,0x96,0x64,0x32,0};
    uint32_t r;
    for (i = 0; i < 6; i++) {
      r = 0x801e8348 + i * 0x10;
      sh16(r,0x08, i == 5 ? s3 : s3 + xs[i]);
      sh16(r,0x0c, i == 5 ? s3 : s3 + xs[i]);
      sh16(r,0x0a, s2);
      sh16(r,0x0e, s2 + 0xf);
      if (i == 5) { sb8(r,4,0xff); sb8(r,5,0xff); sb8(r,6,0xff); }
      else        { sb8(r,4,0);    sb8(r,5,0);    sb8(r,6,(i & 1) ? 0xff : 0x40); }
    }
  }
}

/* empty_ret — empty function (guest: jr ra; nop). */
void empty_ret(void) { }

/* clear_f0300_records — clear 0x34-stride records @0x801f0300, count =
 * s16 @0x801f04a8+0x14. Disasm 0x801360cc-0x80136108+. */
void clear_f0300_records(void)
{
  uint16_t *a0 = (uint16_t *)guest_at(0x801f0300);
  int16_t *cnt = (int16_t *)guest_at(0x801f04a8 + 0x14);
  int32_t n = cnt ? *cnt : 0, i;
  for (i = 0; i < n; i++) a0[i * 0x1a] = 0;   /* 0x34 bytes = 0x1a u16 */
}

/* reset_combat_slots — reset both combatant slots: 42c5c/42c9c/42cd8
 * on slots 0 and 1. Disasm 0x80142d58-0x80142d98. */
extern void reset_combat_record(int32_t);
extern void hitrec_table_clear(int32_t);
extern void table_1f0ae0_clear(int32_t);
void reset_combat_slots(void)
{
  reset_combat_record(0); reset_combat_record(1);
  hitrec_table_clear(0); hitrec_table_clear(1);
  table_1f0ae0_clear(0); table_1f0ae0_clear(1);
}

/* blk_1f26c0_init — init block @0x801f26c0: +4=0x100, clear
 * @0x801f36c4, cd_chan_mode_apply(block), cd_chan_mode_set(0).
 * Disasm 0x80155e30-0x80155e64. */
extern void cd_chan_mode_apply(uint32_t);
extern void cd_chan_mode_set(int32_t);
void blk_1f26c0_init(void)
{
  uint32_t blk = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = 0;
  *(uint32_t *)guest_at(blk + 4) = 0x100;
  cd_chan_mode_apply(blk);
  cd_chan_mode_set(0);
}

/* clear_f4ee8_u16s — clear 10 u16s at 0x801f4ee8 + byte(a0+2)*24 + 0x12,
 * plus u16 @+0x14/+0x16 and 9 words before +0x12. Disasm verified. */
void clear_f4ee8_u16s(uint32_t a0)
{
  uint8_t *ctrl = (uint8_t *)guest_at(a0);
  uint8_t *v1 = (uint8_t *)guest_at(0x801f4ee8) + ctrl[2] * 24;
  uint16_t *v0 = (uint16_t *)(v1 + 0x12);
  int i;
  v1[0x14] = 0; v1[0x15] = 0;
  *(uint16_t *)(v1 + 0x16) = 0;
  for (i = 9; i >= 0; i--) *v0-- = 0;
}

/* blk_1f26c0_init_b — block @0x801f26c0 variant init: +0=7, +4=0x100,
 * +8/+0xa=0x3fff, clear @0x801f36c4, cd_chan_mode_apply, cd_chan_flags_to_dir,
 * cd_chan_mode_set(1). Disasm 0x80155dd0-0x80155e28. */
extern void cd_chan_flags_to_dir(uint32_t);
void blk_1f26c0_init_b(void)
{
  uint32_t s0 = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = 0;
  *(uint32_t *)guest_at(s0) = 7;
  *(uint32_t *)guest_at(s0 + 4) = 0x100;
  *(uint16_t *)guest_at(s0 + 8) = 0x3fff;
  *(uint16_t *)guest_at(s0 + 0xa) = 0x3fff;
  cd_chan_mode_apply(s0);
  cd_chan_flags_to_dir(s0);
  cd_chan_mode_set(1);
}

/* scratchpad_pair_init — scratchpad pair init: select_phase_update(0x1f800000),
 * clear +0x99/+0x9a/+0xe4/+0xf4 on 0x1f8000e8 & 0x1f8001f0,
 * bump u16 +0xe8 on both, gauge_leaf_update. Disasm 0x8016fbfc-6c6c. */
extern int select_phase_update(uint32_t);
extern int32_t gauge_leaf_update(uint32_t);
void scratchpad_pair_init(void)
{
  uint8_t *s0 = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *s1 = (uint8_t *)guest_at(0x1f8001f0);
  uint8_t *z = (uint8_t *)guest_at(0x1f800000);
  (void)select_phase_update(0x1f800000);
  if (z) { z[0x99] = 0; z[0x9a] = 0; }
  if (s0) {
    s0[0xe4] = 0;
    *(uint32_t *)(s0 + 0xf4) = 0;
    *(uint16_t *)(s0 + 0xe8) += 1;
  }
  if (s1) {
    s1[0xe4] = 0;
    *(uint32_t *)(s1 + 0xf4) = 0;
    *(uint16_t *)(s1 + 0xe8) += 1;
  }
  gauge_leaf_update(0x1f800000);
}

/* reset_sprite_ot_block — reset sprite/OT block: zero 8 u16 at *(a0+0xcc),
 * s8(+0x98)->+0x12, s8(+0x99)->+0x14, lhu(+0xa0)->+0x16, then five
 * u16=-1 stores stride -0x235c from 0x801cd530. a0 guest addr.
 * Disasm 0x80127be4-0x80127c58. */
void reset_sprite_ot_block(uint32_t a0)
{
  uint8_t *b = (uint8_t *)guest_at(a0);
  uint16_t *v1;
  uint32_t v0;
  int i;
  if (b == NULL) return;
  v1 = (uint16_t *)guest_at(*(uint32_t *)(b + 0xcc));
  if (v1 == NULL) return;
  for (i = 0; i < 8; i++) v1[i] = 0;
  v1[9] = (uint16_t)(int8_t)b[0x98];
  v1[10] = (uint16_t)(int8_t)b[0x99];
  v1[11] = *(uint16_t *)(b + 0xa0);
  v0 = 0x801cd530;
  for (i = 4; i >= 0; i--) {
    *(uint16_t *)guest_at(v0) = 0xffff;
    v0 -= 0x235c;
  }
}

/* reset_combat_record — reset combat record block: 16 sub-slots stride 4
 * at 0x801f0fb0 + a0*0x84: [i]=0, [i+0x40]=0x64. Disasm verified. */
void reset_combat_record(int32_t a0)
{
  uint8_t *base = (uint8_t *)guest_at(0x801f0fb0 + a0 * 0x84);
  int i;
  if (base == NULL) return;
  for (i = 0; i < 16; i++) {
    *(uint32_t *)(base + i * 4) = 0;
    *(uint32_t *)(base + 0x40 + i * 4) = 0x64;
  }
}

/* blk_1f26c0_init_c — block @0x801f26c0 variant init B: +0x36c4=a0,
 * +0=7, +4=a0|0x100, +8/+0xa=a1, 9c0f0, 9caa0, 9cb20(1,0xffffFF08),
 * 9bf10(1). Disasm 0x80155e6c-0x80155ed4. */
extern void cd_reg_win_rmw_cc_cd(int32_t,uint32_t);
void blk_1f26c0_init_c(int32_t a0,uint32_t a1)
{
  uint32_t s0 = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = (uint32_t)a0;
  *(uint32_t *)guest_at(s0) = 7;
  *(uint32_t *)guest_at(s0 + 4) = (uint32_t)a0 | 0x100;
  *(uint16_t *)guest_at(s0 + 8) = (uint16_t)a1;
  *(uint16_t *)guest_at(s0 + 0xa) = (uint16_t)a1;
  cd_chan_mode_apply(s0);
  cd_chan_flags_to_dir(s0);
  cd_reg_win_rmw_cc_cd(1,0xffffFF08);
  cd_chan_mode_set(1);
}

/* record_copy_1a — copy 0x1a-byte record 0x8011fea0 + idx*0x1a ->
 * 0x801f0640 then hud_sprite_desc_init. Disasm 0x80136d28-0x80136dd4. */
void record_copy_1a(int a0)
{
  memcpy(guest_at(0x801f0640),guest_at(0x8011fea0 + a0 * 0x1a),0x1a);
  hud_sprite_desc_init();
}
