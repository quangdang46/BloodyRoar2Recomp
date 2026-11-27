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
 *                     = slot*0x2150 + fixed guest addrs
 */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#define CONCAT22(hi,lo) (((uint32_t)(uint16_t)(hi) << 16) | (uint16_t)(lo))
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
extern uint32_t br2_call_clean(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern uint32_t call_guest4(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
extern void br2_st8(uint32_t a, uint8_t v);
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
#define DAT_1f80008c    (*(uint16_t *)g_DAT_1f80008c)
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
#define _DAT_1f800020  (*(uint16_t *)g_DAT_1f800020)
#define _DAT_1f800022  (*(uint16_t *)g_DAT_1f800022)
#define _DAT_1f800024  (*(uint16_t *)g_DAT_1f800024)
#define _DAT_1f80007c  (*(uint16_t *)g_DAT_1f80007c) /* guest lhu @0x1f80007c (0x80147034,0x80159028,0x8015f6a0) */
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
#define DAT_801c4580    (*(uint8_t *)g_DAT_801c4580) /* guest sb @0x801c4580 (0x80159028) */
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
#define _DAT_801f4730   (*(int16_t *)g_DAT_801f4730) /* guest lh @0x801f4730 (0x80159028) */
#define _DAT_801f4732   (*(int16_t *)g_DAT_801f4732) /* guest lh @0x801f4732 */
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
#define _DAT_1f80008c   (*(int16_t *)g_DAT_1f80008c) /* guest sh/lh @0x1f80008c (-1 idle; cf. int16_t derefs below) */
#define _DAT_1f80008e   (*(uint16_t *)g_DAT_1f80008e)
#define _DAT_1f8000a8   (*(uint32_t *)g_DAT_1f8000a8)
#define _DAT_1f8000ac   (*(uint32_t *)g_DAT_1f8000ac)
#define _DAT_1f8000a4   (*(uint16_t *)g_DAT_1f8000a4)
#define _DAT_1f8000a2   (*(uint16_t *)g_DAT_1f8000a2)
#define _DAT_1f8000b2   (*(uint16_t *)g_DAT_1f8000b2)
#define _DAT_1f800080   (*(uint16_t *)g_DAT_1f800080)
#define _DAT_1f800078   (*(uint16_t *)g_DAT_1f800078)
#define _DAT_1f800070   (*(uint32_t *)g_DAT_1f800070)
#define _DAT_1f80006c   (*(uint32_t *)g_DAT_1f80006c)
#define _DAT_1f8000b8   (*(uint16_t *)g_DAT_1f8000b8)
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
extern uint32_t call_guest(uint32_t,uint32_t,uint32_t);
int mc_init_reset();
int cd_mode_flag_set();
int cd_init_session();
int cd_read_poll();
int cd_read_issue();
void cursor_sprite_anim(uint8_t *);
extern void entry_0x80126234(void);


/* ---- callee externs (ported in later batches; weak-bound meanwhile) ---- */
extern void memcard_tick(void);
extern void deferred_cmd_drain();
extern void cursor_sprite_anim(uint8_t *);
extern void memcard_slot_poll(void);
extern uint32_t cd_timer_read(int);
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
extern uint32_t gte_screen_init_defer(void);   /* guest 0x80194478 clean port */
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
extern uint32_t scene_entry(int,int,int);
extern void frame_tick(intptr_t);
extern void br2_irq_poll(void);
extern uint32_t memcard_poll_frame(void);
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
extern void gte_ofs_set(int,int);
extern void gte_cw_d000_set(int);
extern void sprite_attr_init_b(int,int,int,int,int);
extern void sprite_attr_init_a(int,int,int,int,int);
extern void oam_list_rebuild(void);
extern void hud_sprite_desc_init(void);
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
extern uint32_t match_end_check(intptr_t);
extern void hud_text_blk_alt(int,int,const void *,int,int,int);
extern uint32_t memcard_poll_frame(void);
extern uint32_t scene_entry(int,int,int);
extern void brightness_packet_send(int);
extern void tile_stream_job_init(int,int,int,int,int,int*);
extern void tile_stream_open(uint8_t*,void*,int*);
extern int  tile_stream_pump(void);
extern int blk_io_retry(int,int*);
extern void cd_stream_mode_init(uint32_t);
extern void gpu_clear_image_cmd2(uint32_t,int,int,int);
extern void gpu_pkt_attr_set(int,int);
extern void cd_fileop_forward(uint32_t,uint32_t);
extern void tile_decode_wait(int,int);
extern void display_frame_present(int*,int);
extern int  cd_cmd_issue_sync(int,int,int);
extern void cd_fileop_dispatch(int);
extern void cd_subsystem_reset(void);
extern void vram_tile_decode(int);
extern int  cd_file_open_b(uint32_t);
extern void vsync_wait_n(uint32_t,int);
extern uint32_t memcard_file_flow(int);
extern void guest_bzero(uint32_t,int);

/* ==================================================================
 * entry_0x80126178 — _start: BSS clear + main call.
 * BSS 0x801ceef0..0x801ff650 zeroed; guest_bzero = memset;
 * then entry_0x80126234 (main); trap(1) on return.
 * ================================================================== */
static void entry_0x80126178(void)
{
    /* Faithful _start (disasm 0x80126178-0x8012621c, delay slots incl.):
     *   BSS clear 0x801CEEF0..0x801FF650 (sw $zero loop, bss_end exclusive)
     *   sp = (*(u32*)0x80126228) | 0x80000000   — literal-pool load, NOT a
     *     BSS read; a0 = 0x801FF650 then sll/srl 3 -> phys 0x1FF650
     *   a1 = v0 - *(0x801BE720) - 0x1FF650     — size for BIOS bzero
     *   save $ra at 0x801CEEF0, gp=0x801BEEDC, fp=sp
     *   jal bzero_tramp(0x801FF654, a1);  jal main(0x80126234); break.
     * sp/gp/fp setup is skipped: the runtime owns cpu->gpr[29] and the
     * clean ports address globals absolutely via guest_at(). */
    uint32_t *bss_cur = (uint32_t *)guest_at(0x801ceef0);
    uint32_t *bss_end    = (uint32_t *)guest_at(0x801ff650);
    uint32_t lit, v1, size;
    while (bss_cur < bss_end) *bss_cur++ = 0;
    *(uint32_t *)guest_at(0x801ceef0) = 0;   /* saved-ra slot (unused) */
    lit  = *(uint32_t *)guest_at(0x80126228); /* literal pool: 2MB top */
    v1   = *(uint32_t *)guest_at(0x801be720);
    size = (lit - v1) - 0x1ff650u;            /* guest subu wraps identically */
    guest_bzero(0x801ff654, size);
    entry_0x80126234();
    /* main() returned early on a bail unwind — propagate it to _start's
     * dispatch frame instead of running the guest trap(1) epilogue. */
    { extern int br2_bail_pending(void);
      if (br2_bail_pending()) return; }
    trap(1);
    halt_baddata();
}

/* func_0x80126170 / func_0x80163840 — jr-ra thunks (empty).
 * empty_leaf_e is the renames.map-registered form of guest 0x80126170
 * (`jr ra; nop`, 8-uint8_t body — a bind_jal waypoint slot). */
void empty_leaf_e(void) { }
static void func_0x80126170(void) { empty_leaf_e(); }
static void func_0x80163840(int p) { (void)p; }

/* apply_saved_settings_or_defaults @0x80126720 */
static void apply_saved_settings_or_defaults(uint32_t *sys_cfg)
{
    uint32_t *src_rec;
    uint32_t *dst_rec;
    uint32_t w1;
    uint32_t w2;
    uint32_t w3;

    src_rec = (uint32_t *)g_DAT_801b1280;
    dst_rec = sys_cfg;
    do {
        w1 = src_rec[1];
        w2 = src_rec[2];
        w3 = src_rec[3];
        *dst_rec = *src_rec;
        dst_rec[1] = w1;
        dst_rec[2] = w2;
        dst_rec[3] = w3;
        src_rec = src_rec + 4;
        dst_rec = dst_rec + 4;
    } while (src_rec != (uint32_t *)g_DAT_801b12e0);
    *dst_rec = *(uint32_t *)g_DAT_801b12e0;
    save_defaults_fill();
    save_block_init();
    save_tables_commit();
    dbg_marker_mode_set(*(uint8_t *)(sys_cfg + 9));
    gauge_scale_fixed(*(uint8_t *)((uint8_t *)sys_cfg + 0xe),
                 ((uint32_t)*(uint8_t *)((uint8_t *)sys_cfg + 0xf) << 3) / 10);
    brightness_commit(*(uint8_t *)(sys_cfg + 3) ^ 1);
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
    bool state_lt_14;
    uint8_t ret_state;
    int call_ret;
    uint32_t state;
    uint32_t mmode;
    uint16_t retry_count;
    /* pad_buf = the pad/input buffer. In the original it lives on the guest
     * stack at sp_old-0x30 (generated 0x80126234: addiu sp,-64; buffer at
     * sp+16). The pad driver is generated/BIOS code that writes guest RAM, so
     * _g_pad_buffer must alias that guest buffer — a host stack array would
     * never receive the controller data. main() never returns, so the
     * reservation is never freed: it reproduces the guest's permanent
     * 0x40-uint8_t main frame and keeps every nested call's frame below the
     * pad buffer instead of overwriting it (frame 0x40 -> sp+0x10 maps to
     * frm+0x10). */
    extern uint32_t br2_guest_frame(uint32_t);
    uint32_t mn_frm = br2_guest_frame(0x40);
    uint16_t *pad_buf = (uint16_t *)guest_at(mn_frm + 0x10);

    func_0x80126170();
    retry_count = 0;
    boot();
    pad_buf[1] = 0x1c7f;
    _g_pad_buffer = (uint8_t *)pad_buf;
    pad_buf[0] = 0;
    DAT_1f8000b8 = 0;
    pad_buf[2] = 0;
    pad_buf[3] = 0;
    pad_buf[4] = 0;
    pad_buf[5] = 0;
    pad_buf[6] = 0;
    pad_buf[7] = 0;
    g_game_state = 0;
    g_sub_state = '\0';
    DAT_1f80009b = 0;
    DAT_1f80009c = '\0';
    call_ret = memcard_info_query();
    if (call_ret == 0) {
        apply_saved_settings_or_defaults((uint32_t *)_g_system_config_ptr);
    }
    else {
        pad_buf[0] = (uint16_t)*(uint8_t *)(_g_system_config_ptr + 0x1d);
        dbg_marker_mode_set(*(uint8_t *)(_g_system_config_ptr + 0x24));
        gauge_scale_fixed(*(uint8_t *)(_g_system_config_ptr + 0xe),
                     ((uint32_t)*(uint8_t *)(_g_system_config_ptr + 0xf) << 3) / 10);
        brightness_commit(*(uint8_t *)(_g_system_config_ptr + 0xc) ^ 1);
    }
    frame_advance_helper();
    func_0x80163840(0);
L_80126344_dispatch:
    /* Bail-unwind in flight (g_psx_call_bail): main() is a clean-C
     * mega-loop — every state dispatch re-enters here. Return so the
     * unwind reaches the outermost psx_dispatch_impl (which flattens the
     * wild flow) instead of latching the flag forever while every
     * br2_call_generated below silently drops. Abandon only the host-side
     * frame record: the guest never ran main's epilogue, so $sp keeps the
     * frame exactly as generated CPS bail-returns leave it. */
    { extern int br2_bail_pending(void);
      extern void br2_guest_frame_abandon(void);
      if (br2_bail_pending()) { br2_guest_frame_abandon(); return; } }
    state = (uint32_t)g_game_state;
code_r0x8012634c:
    state_lt_14 = state < 0xe;
L_80126350:
    do {
    } while (!state_lt_14);
    switch(state) {
    case 0: /* 0x80126378 */
        match_mode_set(1);
        g_game_state = 1;
        goto L_80126344_dispatch;
    case 1: /* 0x8012638c */
        display_mode_setup(*(uint8_t *)(_g_system_config_ptr + 0xb));
        frame_advance_helper();
        transition_screen_flow(0xf);
        state = (uint32_t)g_game_state;
        state_lt_14 = state < 0xe;
        if (state == 1) goto L_801265b8;
        goto L_80126350;
    case 2: /* 0x801263c8 */
        goto L_801263c8_dispatch;
    case 3: /* 0x801263e0 */
        mmode = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            mmode = 1;
        }
        match_mode_set(mmode);
        display_mode_setup(*(uint8_t *)(_g_system_config_ptr + 0xb));
        frame_advance_helper();
        transition_screen_flow(3);
        transition_screen_flow(1);
        state = (uint32_t)g_game_state;
        state_lt_14 = state < 0xe;
        if (state == 3) {
            g_game_state = 4;
            goto L_80126344_dispatch;
        }
        goto L_80126350;
    case 4: /* 0x8012644c */
        mmode = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            mmode = 1;
        }
        match_mode_set(mmode);
        frame_advance_helper();
        gauge_scale_fixed(*(uint8_t *)(_g_system_config_ptr + 0xe),
                     ((((uint32_t)*(uint8_t *)(_g_system_config_ptr + 0xf) * 0x10a) / 100) * 8) / 10);
        call_ret = scene_entry(0x80010000,0,1);
        if (call_ret != 0) {
            g_game_state = 5;
        }
        gauge_scale_fixed(*(uint8_t *)(_g_system_config_ptr + 0xe),
                     ((uint32_t)*(uint8_t *)(_g_system_config_ptr + 0xf) << 3) / 10);
        if (g_game_state != 4) goto L_80126344_dispatch;
        break;
    case 5: /* 0x80126534 */
        goto L_80126534_dispatch;
    case 6: /* 0x801265c8 */
        mmode = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            mmode = 1;
        }
        match_mode_set(mmode);
        battle_intro_flow(0);
        if ((g_game_state != 6) || (results_scoreboard(0), g_game_state != 6))
            goto L_80126344_dispatch;
        goto L_8012661c;
    case 7: /* 0x80126624 */
        mmode = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            mmode = 1;
        }
        match_mode_set(mmode);
        ret_state = 7;
        goto L_8012666c;
    case 8: /* 0x80126654 */
        match_mode_set(1);
        /* a0 is the pad/input buffer. pad_buf is already the host pointer
         * into guest RAM (guest_at of the staged sp-0x30 buffer); the callee
         * dereferences it directly, so pass the host pointer, not guest_of. */
        battle_flow_state_machine((intptr_t)pad_buf);
        ret_state = 8;
L_8012666c:
        if (g_game_state == ret_state) {
            g_game_state = 5;
        }
        goto L_80126344_dispatch;
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
            goto L_80126344_dispatch;
        }
        break;
    case 11: /* 0x801266c0 */
        match_mode_set(1);
        if (g_sub_state == '\x03') {
            call_ret = system_config_menu_a();
        }
        else {
            if (g_sub_state != '\x04') goto L_80126344_dispatch;
            call_ret = system_config_menu_b();
        }
        if (call_ret != 0) {
            g_game_state = 5;
        }
        goto L_80126344_dispatch;
    default: /* 10,12,13 -> 0x80126344 re-dispatch (hang) */
        goto L_80126344_dispatch;
    }
    g_game_state = 5;
    goto L_80126344_dispatch;
L_80126534_dispatch:
    mmode = 2;
    if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
        mmode = 1;
    }
    retry_count = retry_count + 1;
    match_mode_set(mmode);
    display_mode_setup(*(uint8_t *)(_g_system_config_ptr + 0xb));
    /* Faithful call order (generated block_80126534 chain): frame_tick,
     * frame_advance_helper, frame_tick, memcard_poll_frame, then match_loop
     * as the last call before the g_game_state re-check at block_80126594.
     * match_loop runs the whole match and only returns when DAT1 hits 0xff;
     * omitting it dead-loops gs=5 with no combat. */
    frame_tick(0x1f800000);
    frame_advance_helper();
    frame_tick(0x1f800000);
    memcard_poll_frame();
    match_loop();
    state = (uint32_t)g_game_state;
    state_lt_14 = state < 0xe;
    if (state == 5) {
        if (retry_count < 4) {
            g_game_state = 6;
        }
        else {
L_801265b8:
            g_game_state = 2;
        }
        goto L_80126344_dispatch;
    }
    goto L_80126350;
L_801263c8_dispatch:
    state = (uint32_t)g_game_state;
    retry_count = 0;
    if (state == 2) goto L_8012661c;
    goto code_r0x8012634c;
L_8012661c:
    g_game_state = 3;
    goto L_80126344_dispatch;
}

/* ==================================================================
 * boot — boot: subsystem inits + CD file pump.
 * ================================================================== */
void boot(void)
{
    int pump_state;

    cd_dispatch_tblc();
    cd_read_param_set(0);
    gte_screen_init_defer();   /* guest 0x80194478 (was FUN_80194478 -> generated) */
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
    DAT_801c457c = (uint32_t)*(uint8_t *)g_DAT_801b12e6;
    gpu_disp_mask_set(0);
    pump_state = cd_file_pump(0x1f800000,0);
    gfx_tbls_init();
    game_globals_reset();
    sprite_desc_table_init(0);
    print_mode_dispatch_b(2,0,0);
    apply_saved_settings_or_defaults((uint32_t *)_g_system_config_ptr);
    while (-1 < pump_state) {
        pump_state = cd_file_pump(0x1f800000,pump_state);
        cd_timer_read(0);
    }
    battle_session_arm(0x1f800000,0);
    display_mode_setup(*(uint8_t *)(_g_system_config_ptr + 0xb));
    sprite_rect_table_init(0x40,(_DAT_1f800022 * 0xb) / 0xc,0);
    frame_advance_helper();
    return;
}

/* sprite_rect_table_init — not yet decompiled; extern decl below */

/* ==================================================================
 * cd_file_pump — CD file streaming pump.
 * state = pump state: 0 init -> 0x100 streaming -> 0x110 tail ->
 * 0x120 done -> -1. Each step queues one file's DMA reads.
 * ================================================================== */
int cd_file_pump(int cd_base,int state)
{
    uint8_t saved_013;
    uint32_t tag_addr;
    int n;

    if (state != 0x100) {
        if (0x100 < state) {
            if (state != 0x110) {
                if (state != 0x120) {
                    return state;
                }
                return -1;
            }
            n = cd_file_load_sm(0x89,_DAT_801e3cfc);
            if (n == 0) {
                return 0x110;
            }
            fighter_record_apply(_DAT_801e3cfc,0);
            page_table_ptr_set(0,0);
            return 0x120;
        }
        if (state != 0) {
            return state;
        }
        cd_file_load_sm(0xffffffff,0);
        state = 0x100;
        DAT_801c43a0 = 0;
        DAT_801befec = 0xff;
        DAT_801befa6 = 0xff;
        DAT_801c4582 = 0xff;
        DAT_801c4581 = 0xff;
        DAT_801e3d00 = 0xff;
        _DAT_801c47b8 = 0x80010000;
        _DAT_801e3cfc = -0x7fff0000;
    }
    tag_addr = guest_of(tag_table_get());
    n = cd_file_load_sm(0x80,tag_addr);
    saved_013 = DAT_1f800013;
    if (n != 0) {
        scratch_013_set(0);
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0);
        spr_draw_off(_DAT_801e3cfc,0x3c0,0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),1);
        spr_draw_off(_DAT_801e3cfc,0x380,0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),2);
        spr_draw_off(_DAT_801e3cfc,0x1f0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),3);
        spr_draw_off(_DAT_801e3cfc,0x1e0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),4);
        spr_draw_off(_DAT_801e3cfc,0x1d0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),5);
        spr_draw_off(_DAT_801e3cfc,0x170,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),6);
        spr_draw_off(_DAT_801e3cfc,0x160,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x10);
        spr_draw_off(_DAT_801e3cfc,0x1d0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x11);
        spr_draw_off(_DAT_801e3cfc,0x1f0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x12);
        spr_draw_off(_DAT_801e3cfc,0x1e0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x13);
        spr_draw_off(_DAT_801e3cfc,0x1c0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x14);
        spr_draw_off(_DAT_801e3cfc,0x180,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x15);
        spr_draw_off(_DAT_801e3cfc,400,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x16);
        spr_draw_off(_DAT_801e3cfc,0x1a0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x17);
        spr_draw_off(_DAT_801e3cfc,0x1b0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x18);
        spr_draw_off(_DAT_801e3cfc,0x340,0x100,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x19);
        spr_draw_off(_DAT_801e3cfc,0x200,0x100,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x1a);
        spr_draw_off(_DAT_801e3cfc,0x1c0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),0x1b);
        spr_draw_off(_DAT_801e3cfc,0x1b0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),7);
        spr_draw_pair(_DAT_801e3cfc,0x160,0x1e0,1,0x340,0x1c0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),8);
        spr_draw_off(_DAT_801e3cfc,0x380,0x1c0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        tag_addr = guest_of(tag_table_get());
        n = stream_block_decode(_DAT_801e3cfc,(int *)guest_at(tag_addr),9);
        spr_draw_off(_DAT_801e3cfc,0x3a0,0x180,1);
        _DAT_801e3cfc = _DAT_801e3cfc + n;
        cd_timer_read(0);
        cd_timer_read(0);
        scratch_013_set(saved_013);
        state = 0x110;
    }
    return state;
}

/* small frame/console helpers */
void cd_channel_init_seq(void)
{
    cd_channel_table_init(g_DAT_801befa8,g_DAT_801befca);
    /* Guest 0x801267DC tail-calls 0x801A1250 -> cd_channel_main_init:
     * Deq+Enq's pad/device IntRP 0x801FEFC0 into kernel queue 2 plus the
     * two channel-init jalr's. Without it the IntRP never links and the
     * per-interrupt channel poll never fills 0x801BEFA8/0x801BEFCA. */
    call_guest(0x801a1250u,0,0);
}

void scratch_zero_reset(void)
{
    uint32_t *cell;

    cell = (uint32_t *)g_DAT_1f800000;
    do {
        *cell = 0;
        cell = cell + 1;
    } while (cell < (uint32_t *)guest_at(0x1f800400));
}

/* display_mode_setup — display mode + screen window setup.
 * mode = video mode (0-7 from system config +0xb):
 *   0,2,4,6 -> 0xf0-height variant (interlaced?), odd -> 0x1e0.
 * Sets _DAT_1f800020/22/24 screen geometry, OAM window regs via
 * gte_ofs_set/gte_cw_d000_set, sprite blocks at 0x801e40a0/0x801e61f0/0x801e40fc/
 * 0x801e624c, then oam_list_rebuild mode commit + hud_sprite_desc_init. */
void display_mode_setup(uint32_t mode)
{
    int rect_h;
    uint32_t rect_y;

    cd_timer_read(2);
    scratch_013_set(0);
    gpu_graph_reset(1);
    switch(mode) {
    case 0:
        _DAT_1f800020 = 0x140;
        goto L_80127134;
    case 1:
        _DAT_1f800020 = 0x140;
        break;
    case 2:
        _DAT_1f800020 = 0x180;
        goto L_80127134;
    case 3:
        _DAT_1f800020 = 0x180;
        break;
    case 4:
        _DAT_1f800020 = 0x200;
        goto L_80127134;
    case 5:
        _DAT_1f800020 = 0x200;
        break;
    case 6:
        _DAT_1f800020 = 0x280;
L_80127134:
        _DAT_1f800022 = 0xf0;
        DAT_1f800015 = '\0';
        goto L_80127160;
    case 7:
        _DAT_1f800020 = 0x280;
        break;
    default:
        _DAT_1f800020 = 0x200;
    }
    _DAT_1f800022 = 0x1e0;
    DAT_1f800015 = '\x01';
L_80127160:
    _DAT_1f800024 = 0x168;
    gte_ofs_set(_DAT_1f800020 >> 1,(short)_DAT_1f800022 >> 1);
    gte_cw_d000_set((int)_DAT_1f800024);
    *(uint8_t *)g_DAT_801e625e = 1;
    *(uint8_t *)g_DAT_801e410e = 1;
    if (DAT_1f800015 == '\0') {
        sprite_attr_init_a(0x801e40a0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_a(0x801e61f0,0,0xf0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_b(0x801e40fc,0,0xf0,(int)_DAT_1f800020,0);
        rect_y = 0;
        rect_h = 0;
    }
    else {
        sprite_attr_init_a(0x801e40a0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_a(0x801e61f0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        sprite_attr_init_b(0x801e40fc,0,2,(int)_DAT_1f800020,(short)_DAT_1f800022 + -2);
        rect_y = 2;
        rect_h = (short)_DAT_1f800022 + -2;
    }
    sprite_attr_init_b(0x801e624c,0,rect_y,(int)_DAT_1f800020,rect_h);
    if (DAT_1f800015 == '\0') {
        *(uint16_t *)g_DAT_801e410a = _DAT_1f800022;
        *(uint16_t *)g_DAT_801e625a = _DAT_1f800022;
    }
    else {
        *(uint16_t *)g_DAT_801e410a =
             (short)((((int)((uint32_t)_DAT_1f800022 << 0x10) >> 0x10) -
                     ((int)((uint32_t)_DAT_1f800022 << 0x10) >> 0x1f)) >> 1) - 2;
        *(uint16_t *)g_DAT_801e625a =
             (short)((((int)((uint32_t)_DAT_1f800022 << 0x10) >> 0x10) -
                     ((int)((uint32_t)_DAT_1f800022 << 0x10) >> 0x1f)) >> 1) - 2;
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

void scratch_013_set(uint8_t val)
{
    DAT_1f800013 = val;
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
    *(uint32_t *)guest_at(0x1f8000c0) = guest_of(g_DAT_801c1200);
    _DAT_1f8000bc = 0x801f0270;
    _DAT_1f8000b8 = 0;
    _DAT_1f8000cc = 0x801c15f8;
    players_ctrl_init();
    {
        uint32_t flags = u16_lut_lookup(0xf2000002);
        mode_flag_pair_set(flags);
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

void status_flag_cond_set(uint32_t arg,int flag)
{
    if ((flag != 0) && (flag == 1)) {
        DAT_801c15f0 = 1;
    }
}

void stat_counter_accum(int rec_addr,int kind)
{
    /* rec_addr is a raw guest address (clean_table passes gpr[4] verbatim);
     * resolve it once — the direct (rec_addr+off) derefs below would read
     * guest address bits as a host pointer. */
    uint8_t *rec = (uint8_t *)guest_at((uint32_t)rec_addr);
    short ctr;
    int sub_idx;
    int rec_off;

    rec_off = (uint32_t)rec[3] * 0x24;
    switch(DAT_1f80009c) {
    default:
        if (kind == 0) {
            ctr = *(short *)guest_at((uint32_t)(rec_off + -0x7fe3ebb2));
            *(short *)((char *)g_DAT_801c144c + rec_off) = *(short *)((char *)g_DAT_801c144c + rec_off) + 1;
            *(short *)guest_at((uint32_t)(rec_off + -0x7fe3ebb2)) = ctr + 1;
            sub_idx = (uint32_t)*(uint8_t *)guest_at(*(uint32_t *)(rec + 0x2c) + 3) * 2 + rec_off;
            *(short *)((char *)g_DAT_801c1444 + sub_idx + 0xe) = *(short *)((char *)g_DAT_801c1444 + sub_idx + 0xe) + 1;
        }
        else {
            ctr = *(short *)guest_at((uint32_t)(rec_off + -0x7fe3ebb6));
            *(short *)((char *)g_DAT_801c1448 + rec_off) = *(short *)((char *)g_DAT_801c1448 + rec_off) + 1;
            *(short *)guest_at((uint32_t)(rec_off + -0x7fe3ebb6)) = ctr + 1;
        }
        *(short *)((char *)g_DAT_801c1446 + rec_off) = *(short *)((char *)g_DAT_801c1446 + rec_off) + 1;
        if (*(uint16_t *)((char *)g_DAT_801c1450 + rec_off) < *(uint16_t *)(rec + 0xe2)) {
            *(uint16_t *)((char *)g_DAT_801c1450 + rec_off) = *(uint16_t *)(rec + 0xe2);
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
void battle_session_arm(intptr_t base,int slot)
{
    int addr;

    base = (intptr_t)guest_at((uint32_t)base);
    if (-1 < slot) {
        addr = slot * 0x2150 + -0x7fe1bef0;
        *(uint16_t *)(base + 0xd0) = 0;
        *(uint16_t *)(base + 0xd2) = 0;
        *(uint8_t *)(base + 0x10) = 0;
        _DAT_1f8000d8 = 0;
        _DAT_1f8000e4 = 0;
        *(int *)(base + 0xc) = slot;
        *(int *)(base + 4) = slot * 0x2150 + -0x7fe1bf60;
        *(int *)(base + 8) = addr;
        clear_battle_counters();
        if (slot == 0) {
            gpu_clear_otag_r(0x801e4110,0x820);
            addr = -0x7fe19da0;
        }
        gpu_clear_otag_r((uint32_t)addr,0x820);
    }
}

/* match_end_check — match-end check: pad-start prompt timer.
 * Returns -1 normally; 0xffffff9d (-99) while prompting exit. */
uint32_t match_end_check(intptr_t ctx)
{
    uint32_t ret;
    int w;
    int h;

    ctx = (intptr_t)guest_at((uint32_t)ctx);
    if ((_DAT_1f8000dc >> 4 & 3U) == 0) {
        ret = 0xffffffff;
    }
    else {
        h = *(short *)(ctx + 0x22) * 3;
        w = (uint32_t)*(uint16_t *)(ctx + 0x20) << 0x10;
        if (h < 0) {
            h = h + 3;
        }
        hud_text_blk_alt(*(uint32_t *)(ctx + 8),*(uint32_t *)(ctx + 0xc),
                     g_DAT_80121e3c,
                     ((((w >> 0x10) - (w >> 0x1f)) >> 1) + -0x45) * 0x10000 >> 0x10,
                     (h << 0xe) >> 0x10,4);
        ret = 0xffffff9d;
    }
    return ret;
}

/* memcard_poll_frame — per-frame memcard/save poll */
uint32_t memcard_poll_frame(void)
{
    uint32_t ret;

    if (*(char *)(_g_system_config_ptr + 0x22) == '\0') {
        ret = 0xffffffff;
    }
    else {
        ret = 0xffffffff;
        if (DAT_801c15f0 != 0) {
            DAT_801c15f0 = 0;
            ret = memcard_file_flow(1);
        }
    }
    return ret;
}

/* ==================================================================
 * frame_tick — frame driver (called 2x/frame: fighter slot toggle).
 * ctx = battle object guest addr (0x1f800000).
 * +0xc fighter index XORs 0/1 each call -> double-buffered submit.
 * ================================================================== */
void frame_tick(intptr_t ctx)
{
    uint8_t img_w;
    uint8_t img_h;
    uint16_t ticks;
    int tmp;
    uint32_t next_slot;
    int pkt_ptr;
    int env_off;
    int env_ptr;
    int ot_ptr;

    /* Clean-C frame loops (match_loop, battle_flow_state_machine, the
     * g_game_state SM) never cross a generated-code boundary on idle or
     * menu paths, so psx_check_interrupts_at -- and with it VBlank IRQ
     * delivery, device deadlines and debug_server_poll -- starves. The
     * guest equivalent checks interrupts at every basic-block boundary;
     * br2_irq_poll supplies that once per frame_tick call (2x/frame). */
    br2_irq_poll();

    ctx = (intptr_t)guest_at((uint32_t)ctx);
    env_ptr = *(int *)(ctx + 4);
    ot_ptr = *(int *)(ctx + 8);
    memcard_tick();
    deferred_cmd_drain();
    cursor_sprite_anim((uint8_t *)ctx);
    memcard_slot_poll();
    tmp = cd_timer_read(1);
    if (tmp < 0xf1) {
        gpu_pkt_unlink_n(ot_ptr + 0x207c,0x81f);
    }
    pad_state_refresh(ctx);
    if ((*(char *)(ctx + 0x15) == '\0') && ((*(uint16_t *)(ctx + 0x82) & 1) != 0)) {
        ticks = cd_timer_read(1);
        *(uint16_t *)(ctx + 0x1a) = ticks;
        gpu_draw_sync(0);
        ticks = cd_timer_read(1);
        *(uint16_t *)(ctx + 0x1c) = ticks;
        cd_timer_read(*(uint8_t *)(ctx + 0x10));
    }
    else {
        ticks = cd_timer_read(1);
        *(uint16_t *)(ctx + 0x1c) = ticks;
        *(uint16_t *)(ctx + 0x1a) = ticks;
        cd_timer_read(*(uint8_t *)(ctx + 0x10));
        gpu_graph_reset(1);
    }
    _DAT_1f8000e0 = _DAT_1f8000e4;
    _DAT_1f8000e4 = 0;
    _DAT_801e8500 = gpu_stat_sign();
    draw_env_timing(env_ptr + 0x5c);
    gpu_draw_env_put(env_ptr);
    if (*(uint8_t *)(ctx + 0x12) != 0) {
        gpu_disp_mask_set(*(uint8_t *)(ctx + 0x12) - 1);
        *(uint8_t *)(ctx + 0x12) = 0;
    }
    tmp = (int)*(short *)(ctx + 0xd0);
    while (0 < tmp) {
        gpu_load_image_cmd(tmp * 8 + -0x7fe3bad0,((uint32_t *)g_DAT_801bef38)[tmp + -1]);
        tmp = tmp + -1;
    }
    tmp = (int)*(short *)(ctx + 0xd2);
    *(uint16_t *)(ctx + 0xd0) = 0;
    if (0 < tmp) {
        env_off = tmp * 0x18 + 0x20d8;
        pkt_ptr = tmp * 0x18 + env_ptr;
        do {
            next_slot = env_ptr + env_off;
            env_off = env_off + -0x18;
            tmp = tmp + -1;
            /* pkt_ptr/ot_ptr hold GUEST addresses (OT/packet bases stored
             * as 0x801e.... ints) — resolve through guest_at, not a raw
             * host dereference. */
            *(uint32_t *)guest_at((uint32_t)(pkt_ptr + 0x20d8)) =
                *(uint32_t *)guest_at((uint32_t)(ot_ptr + 0x207c)) | 0x5000000;
            pkt_ptr = pkt_ptr + -0x18;
            *(uint32_t *)guest_at((uint32_t)(ot_ptr + 0x207c)) = next_slot & 0xffffff;
        } while (0 < tmp);
    }
    *(uint16_t *)(ctx + 0xd2) = 0;
    if (*(char *)(ctx + 0x13) != '\0') {
        tmp = *(int *)(ctx + 0xc) * 0x68;
        if ((0 < *(short *)((char *)g_DAT_801f066e + tmp)) && (*(char *)(ctx + 0x9e) == '\0')) {
            img_w = *(uint8_t *)guest_at((uint32_t)(tmp + -0x7fe0f99b));
            img_h = *(uint8_t *)guest_at((uint32_t)(tmp + -0x7fe0f99a));
            *(uint16_t *)((char *)g_DAT_801f066a + tmp) =
                 *(uint16_t *)guest_at((uint32_t)(*(int *)(ctx + 0xc) * 0x2150 + -0x7fe1bf5e));
            gpu_clear_image_cmd(guest_of((char *)g_DAT_801f0668 + tmp),
                         (uint32_t)(uintptr_t)*(uint32_t *)((char *)g_DAT_801f0664 + tmp),img_w,img_h);
        }
        gpu_draw_otag(ot_ptr + 0x207c);
    }
    table_byte_next_a();
    next_slot = *(uint32_t *)(ctx + 0xc) ^ 1;
    tmp = next_slot * 0x2150;
    _DAT_1f8000dc = *(uint32_t *)guest_at((uint32_t)(tmp + -0x7fe1bf60)) + 1;
    *(uint32_t *)(ctx + 0xc) = next_slot;
    *(int *)(ctx + 4) = tmp + -0x7fe1bf60;
    *(int *)(ctx + 8) = tmp + -0x7fe1bef0;
    gpu_clear_otag_r(tmp + -0x7fe1bef0,0x820);
    *(uint32_t *)guest_at((uint32_t)(tmp + -0x7fe1bef0)) = *(uint32_t *)guest_at((uint32_t)(tmp + -0x7fe1bef0)) | 0xffffff;
    menu_noop(ctx);
    if (*(char *)guest_at(*(uint32_t *)(ctx + 200) + 2) < '\0') {
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
/* match_stepper_desc — contiguous menu-stepper descriptor. In the original
 * frame this is a single stack block at sp+0x20: menu_stepper_state_advance /
 * menu_stepper_input / menu_stepper_redraw and the +0xc callback all address
 * it by uint8_t offset, so the fields MUST be contiguous. Declaring them as
 * separate C locals lets the compiler reorder/pad them, which breaks the
 * +0xc callback read (fn_lookup gets garbage -> dead menu). */
struct match_stepper_desc {
    uint16_t unk00;      /* +0x00 */
    uint8_t  state;      /* +0x02 0 -> armed to 1 */
    uint8_t  gate;       /* +0x03 input+callback enable */
    uint8_t  unk04;      /* +0x04 */
    uint8_t  nav_mode;   /* +0x05 0: 0x4000/0x1000 else 0x2000/0x8000 */
    uint16_t max;        /* +0x06 row count */
    uint16_t sel;        /* +0x08 cursor */
    uint16_t unk0a;      /* +0x0a */
    uint32_t callback;   /* +0x0c guest code address */
    uint16_t unk10;      /* +0x10 */
    uint16_t unk12;
    uint16_t unk14;
    uint16_t unk16;
    uint16_t unk18;
    uint16_t unk1a;
    uint16_t unk1c;
    uint16_t unk1e;      /* +0x1e */
};                      /* 0x20 bytes */

void match_loop(void)
{
    int ret;
    int attract_timer;
    struct match_stepper_desc sd;
    /* Guest: attract_stage_load_step writes 4 contiguous words at
     * sp+0x68..0x77 (a1 = sp+0x68); spr_draw_pair then loads
     * sp+0x68/0x6c/0x70/0x74. Ghidra split the block into 4 scalars —
     * keep it one array (stage_sprs[i] == old local_4X). */
    uint32_t stage_sprs[4];
    uint32_t cfg_w0;
    uint32_t cfg_w1;
    uint32_t cfg_w2;
    uint32_t cfg_w3;
    uint32_t cfg_w4;

    DAT_1f80008c = 0;
    cfg_w0 = *(uint32_t *)g_DAT_801202e4;
    cfg_w1 = *(uint32_t *)g_DAT_801202e8;
    cfg_w2 = *(uint32_t *)g_DAT_801202ec;
    cfg_w3 = *(uint32_t *)g_DAT_801202f0;
    cfg_w4 = *(uint32_t *)g_DAT_801202f4;
joined_r0x801470b0:
    { extern int br2_bail_pending(void);
      if (br2_bail_pending()) return; }
    do {
        if (DAT_1f800001 == 0xff) {
            DAT_1f800001 = 0;
            return;
        }
        if (DAT_1f800001 == 2) {
            scroll_offset_set(0,0);
            if ((_DAT_1f80007c & 0x10) == 0) {
                menu_stepper_state_advance(&sd);
            }
            else {
                deferred_cmd_post(0x400,0xd,0);
                DAT_1f800001 = 1;
            }
L_801472f8:
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
                ret = frame_divisor_get();
                if (ret == 0) {
                    spr_attr_alloc_a(_DAT_1f800008,0,0,(int)_DAT_1f800020,(int)_DAT_1f800022,0,0);
                    ret = attract_stage_load_step(0x1f800000,stage_sprs);
                    if (ret != 0) {
                        deferred_cmd_post(0x204,0,0);
                        print_mode_dispatch_b(2,0,0);
                        deferred_cmd_post(0x400,0x82,0);
                        /* struct+0xc is a u32 GUEST code address read by
                         * menu_stepper_state_advance -> fn_lookup; storing
                         * a host fn pointer leaks 64-bit host bits. The
                         * guest value is 0x8014751c (attract_menu_dispatch
                         * in fnmap). sd must stay contiguous (see struct). */
                        memset(&sd, 0, sizeof sd);
                        sd.callback = 0x8014751c;
                        sd.max = 7;
                        sd.unk04 = 1;
                        sd.sel = 1;
                        sd.unk10 = 0xffff;
                        sd.unk12 = 0xffff;
                        scratch_013_set(0);
                        spr_draw_pair(stage_sprs[0],0,0x1f0,1,0x200,0,1);
                        spr_draw_pair(stage_sprs[1],0,0x1f1,1,0x280,0,1);
                        spr_draw_pair(stage_sprs[2],0,0x1f2,1,0x240,0x100,1);
                        spr_draw_pair(stage_sprs[3],0,499,1,0x2c0,0x100,1);
                        cd_timer_read(0);
                        scratch_013_set(1);
                        _DAT_1f80008c = 0;
                        battle_session_arm(0x1f800000,0xffffffff);
                        attract_timer = 300;
                        DAT_1f800001 = DAT_1f800001 + 1;
                        goto L_801472a4;
                    }
                }
            }
            else if (DAT_1f800001 == 1) {
L_801472a4:
                ret = match_end_check(0x1f800000);
                if ((ret < 0) && (attract_timer = attract_timer + -1, attract_timer == 0)) {
                    DAT_1f800001 = 0xf0;
                }
                _DAT_1f80008c = _DAT_1f80008c + 1;
                if ((_DAT_1f80007c & 0x840) != 0) {
                    deferred_cmd_post(0x400,0xc,0);
                    DAT_1f800001 = 2;
                }
                goto L_801472f8;
            }
        }
        else if (DAT_1f800001 == 0xf0) {
            deferred_cmd_post(0x204,0,0);
            frame_present(0x1f800000);
            scroll_cfg_set(1,8);
            DAT_1f800001 = DAT_1f800001 + 1;
        }
        else if ((DAT_1f800001 == 0xf1) && (ret = frame_divisor_get(), ret == 0)) {
            /* Same stale-callee-saved hazard as battle_intro_flow: the hoisted
             * 0xff can sit in a register the IRQ setjmp/longjmp restores
             * stale. br2_st8 materializes the value in the arg register. */
            br2_st8(0x1f800001, 0xff);
            goto joined_r0x801470b0;
        }
        frame_tick(0x1f800000);
    } while( true );
}

/* ==================================================================
 * scene_entry — scene/stage entry.
 * vram_base = VRAM arena base, stage_idx = stage/file-table index,
 * demo_flag = demo flag. Loads stage file, sets VRAM windows,
 * streams data, uploads palette/OAM, arms battle, runs 2 frames.
 * Returns 1 on success (state->5), 0 to retry.
 * ================================================================== */
uint32_t scene_entry(int vram_base,int stage_idx,int demo_flag)
{
    int tbl_off;
    int tmp;
    uint32_t pkt_attr;
    int tmp2;
    int *io_ctx;
    uint32_t *src;
    uint32_t ret;
    /* guest sp-0x60..sp frame image (frame 0x60). cmd_byte/clear_rect/
     * io_ptr are read and written across blk_io_retry/cd_stream_mode_init/
     * gpu_clear_image_cmd2 nested calls, so reserve the window on the live
     * guest sp: sp+0x18 -> frm+0x18, sp+0x20 -> frm+0x20, sp+0x28 -> frm+0x28. */
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t se_frm = br2_guest_frame(0x60);
    /* cmd_byte = sp+0x18 (frame 0x60);
     * `sb $v0,0x18($sp)` + `addiu $a1,$sp,0x18` in 0x80159028. */
    uint8_t *cmd_byte = (uint8_t *)guest_at(se_frm + 0x18);
    /* ClearImage rect = sp+0x20..0x27 (frame 0x60) -> frm+0x20.
     * gpu_clear_image_cmd2 forwards vram_base to generated GPU code as a
     * raw guest address, so the rect must live in the guest frame —
     * host stack locals are invisible to it. */
    uint16_t *clear_rect = (uint16_t *)guest_at(se_frm + 0x20);
    /* io_ptr = sp+0x28 (frame 0x60) -> frm+0x28;
     * `swl/swr $t2,0x28($sp)` + `addiu $a0,$sp,0x28` in 0x80159028. */
    uint32_t *io_ptr = (uint32_t *)guest_at(se_frm + 0x28);

    ret = 0;
    DAT_801c43a0 = 0;
    DAT_801e3d00 = 0xff;
    _DAT_801f4744 = vram_base + 0x10000;
    _DAT_801f4748 = vram_base + 0x38000;
    _DAT_801f474c = vram_base + 0x60000;
    _DAT_801f4750 = vram_base + 0x67800;
    _DAT_801f4758 = vram_base + 0x6f000;
    tbl_off = stage_idx * 0x1c;
    src = (uint32_t *)guest_at(((uint32_t *)g_PTR_DAT_801b5b94)[(stage_idx + 8) * 2]);
    io_ctx = (int *)((char *)g_DAT_801b3ed8 + tbl_off);
    _DAT_801f4754 = vram_base;
    frame_tick(0x1f800000);
    brightness_packet_send(0x7f);
    scratch_013_set(0);
    cd_timer_read(0);
    _DAT_801f473c = 1;
    if (*io_ctx == 0) {
        tmp = *(int *)((char *)g_DAT_801b3ed8 + 0xc + tbl_off);
    }
    else {
        tmp = (*(int *)((char *)g_DAT_801b3ed8 + 0xc + tbl_off) * 3) / 2;
    }
    if (*io_ctx == 0) {
        tmp2 = *(int *)((char *)g_DAT_801b3ed8 + 0xc + tbl_off);
    }
    else {
        tmp2 = (*(int *)((char *)g_DAT_801b3ed8 + 0xc + tbl_off) * 3) / 2;
    }
    tile_stream_job_init(0x801f4700,tmp,*(uint32_t *)((char *)g_DAT_801b3ed8 + 0x10 + tbl_off),tmp2,
                 *(int *)((char *)g_DAT_801b3ed8 + 0x10 + tbl_off) + *(int *)((char *)g_DAT_801b3ed8 + 0x18 + tbl_off),io_ctx);
    tile_stream_open((uint8_t *)src,(void *)(uintptr_t)0x80159864,io_ctx);
    vram_tile_decode(_DAT_801f4758);
    while (tmp = (int)blk_io_retry(0x801f4700,io_ctx), tmp == -1) {
        io_ptr[0] = *src;
        cd_stream_mode_init(guest_of(io_ptr));
    }
    _DAT_801f4740 = 0;
    gpu_disp_mask_set(0);
    clear_rect[0] = 0;
    clear_rect[1] = 0;
    if (*io_ctx == 0) {
        clear_rect[2] = *(uint16_t *)((char *)g_DAT_801b3ed8 + 0x14 + tbl_off);
    }
    else {
        clear_rect[2] = (uint16_t)
                   ((uint32_t)(*(int *)((char *)g_DAT_801b3ed8 + 0x14 + tbl_off) * 3 -
                          (*(int *)((char *)g_DAT_801b3ed8 + 0x14 + tbl_off) * 3 >> 0x1f)) >> 1);
    }
    clear_rect[3] = (uint16_t)(*(int *)((char *)g_DAT_801b3ed8 + 0x18 + tbl_off) << 1);
    /* vram_base is a raw guest address (the sp+0x20 rect), matching the
     * callee's uint32_t signature — not a host pointer. */
    gpu_clear_image_cmd2((uint32_t)(se_frm + 0x20),0,0,0);
    do {
        pkt_attr = 2;
        if (*io_ctx != 0) {
            pkt_attr = 3;
        }
        gpu_pkt_attr_set(*(uint32_t *)guest_at(_DAT_801f4708 * 4 + -0x7fe0b900),pkt_attr);
        tmp = _DAT_801f4732 + -1;
        if (tmp < 0) {
            tmp = _DAT_801f4732 + 0xe;
        }
        cd_fileop_forward(*(uint32_t *)guest_at(_DAT_801f4714 * 4 + -0x7fe0b8f4),
                     _DAT_801f4730 * 0x10 * ((tmp >> 4) + 1) >> 1);
        while (tmp = (int)blk_io_retry(0x801f4700,io_ctx), tmp == -1) {
            tmp = cd_file_open_b(guest_of(io_ptr));
            if ((*(int *)((char *)g_DAT_801b3ed8 + 0x8 + tbl_off) < tmp) || (tmp < 1)) {
                io_ptr[0] = *src;
            }
            cd_stream_mode_init(guest_of(io_ptr));
        }
        tile_decode_wait(0x801f4700,0);
        display_frame_present(io_ctx,0xffffffff);
        if (_DAT_801f4740 == 1) goto L_801593a8;
    } while ((demo_flag == 0) || ((_DAT_1f80007c & 0x850) == 0));
    ret = 1;
L_801593a8:
    brightness_packet_send(0);
    cmd_byte[0] = 0x80;
    _DAT_1f80000c = (uint32_t)(_DAT_1f80000c == 0);
    _DAT_1f800004 = _DAT_1f80000c * 0x2150 + -0x7fe1bf60;
    _DAT_1f800008 = _DAT_1f80000c * 0x2150 + -0x7fe1bef0;
    cd_cmd_issue_sync(0xe,(int)guest_of(cmd_byte),0);
    DAT_801c4580 = 1;
    cd_fileop_dispatch(0);
    cd_subsystem_reset();
    cd_cmd_issue_sync(9,0,0);
    cd_timer_read(0);
    display_mode_setup(*(uint8_t *)(_g_system_config_ptr + 0xb));
    battle_session_arm(0x1f800000,1);
    frame_tick(0x1f800000);
    frame_advance_helper();
    frame_tick(0x1f800000);
    br2_guest_frame_free();
    return ret;
}

/* ======== 0x8018cfdc ======== */

/* cd_file_load_sm — per-file CD load state machine. The clean port passed
 * dst = guest_at(filetab[..]) — a HOST arena pointer — into
 * cd_read_issue -> cd_mode_flag_set -> cd_cmd_issue(2,..), which is delegated
 * to generated code that reads the SetLoc MSF params treating dst as a
 * GUEST address. The host pointer decoded to a bogus address -> SetLoc
 * [0,0,0] -> read lead-in zeros -> game rejected data and retried forever
 * (the Pause->SetLoc[0,0,0]->Setmode->ReadN livelock at LBAs -148/-149/-150).
 * The generated body keeps filetab[..] as a guest address end-to-end and has
 * per-block interrupt checks for the poll loops. Delegate it. */
int cd_file_load_sm(int file_id,uint32_t dst)

{
  /* file-load dest trace (tag 10): dst=dest arg, src=file_id. Clean-C path
   * bypasses wtrace, so this ring is the only visibility into which file is
   * aimed at which heap cursor before the DMA lands. Dedup consecutive
   * identical (file_id,dest) so the post-corruption cd_file_load_sm retry
   * livelock does not flood the 16K ring and evict the pump decode history. */
  { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
    extern uint32_t clean_caller_ra(void);
    extern uint32_t g_cdfn_log[24];
    static uint32_t last_f=-1,last_d=-1;
    g_cdfn_log[15]++;   /* CDFN_FILE_LOAD_SM */
    if((uint32_t)file_id!=last_f||dst!=last_d){
      last_f=(uint32_t)file_id;last_d=dst;
      /* len field carries the generated caller's $ra (post-jal addr) so the
       * file_load provenance names which function aimed file_id at dst. */
      decomp_trace_record(10,dst,(uint32_t)file_id,0,clean_caller_ra());} }
  return (int)br2_call_clean(0x8018cfdc,(uint32_t)file_id,dst,0,0);
}

/* ======== 0x8012b23c ======== */

void frame_present(intptr_t ctx)

{
  ctx = (intptr_t)guest_at((uint32_t)ctx);
  uint8_t img_w;
  uint8_t img_h;
  uint16_t ticks;
  int tmp;
  uint32_t next_slot;
  uint32_t ot_sel;
  int env_ptr;
  
  env_ptr = *(int *)(ctx + 4);
  memcard_tick();
  deferred_cmd_drain();
  cursor_sprite_anim((uint8_t *)ctx);
  memcard_slot_poll();
  pad_state_refresh(ctx);
  if ((*(char *)(ctx + 0x15) == '\0') && ((*(uint16_t *)(ctx + 0x82) & 1) != 0)) {
    ticks = cd_timer_read(1);
    *(uint16_t *)(ctx + 0x1a) = ticks;
    gpu_draw_sync(0);
    ticks = cd_timer_read(1);
    *(uint16_t *)(ctx + 0x1c) = ticks;
    cd_timer_read(*(uint8_t *)(ctx + 0x10));
  }
  else {
    ticks = cd_timer_read(1);
    *(uint16_t *)(ctx + 0x1c) = ticks;
    *(uint16_t *)(ctx + 0x1a) = ticks;
    cd_timer_read(*(uint8_t *)(ctx + 0x10));
    gpu_graph_reset(1);
  }
  DAT_1f8000e0 = DAT_1f8000e4;
  DAT_1f8000e4 = 0;
  _DAT_801e8500 = gpu_stat_sign();
  draw_env_timing(env_ptr + 0x5c);
  gpu_draw_env_put(env_ptr);
  if (*(uint8_t *)(ctx + 0x12) != 0) {
    gpu_disp_mask_set(*(uint8_t *)(ctx + 0x12) - 1);
    *(uint8_t *)(ctx + 0x12) = 0;
  }
  tmp = (int)*(short *)(ctx + 0xd0);
  while (0 < tmp) {
    gpu_load_image_cmd((int)(tmp * 8 + -0x7fe3bad0),*(uint32_t *)((char *)DAT_801bef38 + (tmp + -1) * 4));
    tmp = tmp + -1;
  }
  *(uint16_t *)(ctx + 0xd0) = 0;
  if (*(char *)(ctx + 0x13) != '\0') {
    next_slot = *(uint32_t *)(ctx + 0xc) ^ 1;
    tmp = next_slot * 0x68;
    if ((0 < *(short *)((char *)DAT_801f066e + tmp)) && (*(char *)(ctx + 0x9e) == '\0')) {
      img_w = *(uint8_t *)((char *)g_DAT_801f0664 + tmp + 4);
      img_h = *(uint8_t *)((char *)g_DAT_801f0664 + tmp + 5);
      *(uint16_t *)((char *)g_DAT_801f066a + tmp) = *(uint16_t *)((char *)g_DAT_801f0664 + tmp - 0x42);
      gpu_clear_image_cmd(guest_of((char *)g_DAT_801f0668 + tmp),((uint32_t *)g_DAT_801f0664)[tmp / 4],img_w,img_h);
    }
    ot_sel = 0x801e618c;
    if (env_ptr == -0x7fe1bf60) {
      ot_sel = 0x801e82dc;
    }
    gpu_draw_otag(ot_sel);
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
void task_table_init(int32_t arg)
{
  (void)arg;
  *(uint32_t *)guest_at(0x801f0738) = 0;
  *(uint32_t *)guest_at(0x801bef20) = 0;
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

/* intro_movie_state — intro/movie playback state machine. State uint8_t
 * _DAT_1f80008c: -1 idle; 0 init; 0x10..0x30 staged file loads via
 * cd_file_load_sm(file_id, heap=_DAT_801e3cfc); stream_block_decode advances
 * frames; which selects which queued load to start (0/1/2). */
int32_t intro_movie_state(int32_t which)
{
  uint8_t saved_013;
  int32_t ret, decoded;
  int16_t state;
  state = *(int16_t *)g_DAT_1f80008c;
  if (state < 0) return -1;
  if (state != 0x10) {
    if (0x10 < state) {
      if (state == 0x20) {
        ret = cd_file_load_sm(0x8b,(uint32_t)_DAT_801e3cfc);
        if (ret == 0) { *(int16_t *)g_DAT_1f80008c = state; return 0; }
        state = 0x30;
        fighter_record_apply(_DAT_801e3cfc,1);
        page_table_ptr_set(1,1);
        cd_timer_read(0);
      } else {
        state = *(int16_t *)g_DAT_1f80008c;
        if (state != 0x30) return 0;
      }
      ret = cd_file_load_sm(0x16,(uint32_t)_DAT_801e3cfc);
      saved_013 = DAT_1f800013;
      if (ret == 0) { *(int16_t *)g_DAT_1f80008c = state; return 0; }
      scratch_013_set(0);
      spr_draw_off(_DAT_801e3cfc,0,0x1e0,1);
      goto L_8012c330;
    }
    if (state != 0) return 0;
    cd_file_load_sm(-1,0);
    state = 0x10;
    _DAT_801c43a0 = 0;
    DAT_801e3d00 = 0xff;
    _DAT_801c47b8 = (uint32_t)-0x7fff0000;
    _DAT_801e3cfc = 0x800d0000;
  }
  if (which == 1) {
    ret = cd_file_load_sm(0x14,(uint32_t)_DAT_801e3cfc);
    if (ret != 0) {
      ret = 0;
      do {
        decoded = stream_block_decode((uint32_t)_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),ret);
        ret++;
        _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)decoded;
      } while (ret < 0xb);
      state = 0x20;
    }
  } else if (which < 2) {
    if (which == 0 &&
        (ret = cd_file_load_sm(0x14,(uint32_t)_DAT_801e3cfc),
         saved_013 = DAT_1f800013, ret != 0)) {
      scratch_013_set(0);
      uint8_t *ctr = (uint8_t *)guest_at(0x801b1408);
      *ctr = *ctr + 1;
      if (10 < *ctr) *ctr = 0;
      ret = stream_block_decode(_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),*ctr);
      spr_draw_off((int)_DAT_801c47b8,0x3c0,0x100,1);
      _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)ret;
L_8012c330:
      cd_timer_read(0);
      scratch_013_set(saved_013);
      *(int16_t *)g_DAT_1f80008c = -1;
      return 1;
    }
  } else if (which == 2 &&
             (ret = cd_file_load_sm(0x15,(uint32_t)_DAT_801e3cfc), ret != 0)) {
    ret = 0;
    do {
      decoded = stream_block_decode((uint32_t)_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),ret);
      ret++;
      _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)decoded;
    } while (ret < 0x17);
    state = 0x20;
  }
  *(int16_t *)g_DAT_1f80008c = state;
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
extern void     print_mode_dispatch(uint32_t,uint32_t,uint32_t,uint32_t,
                                    uint32_t,uint32_t,uint32_t,uint32_t,
                                    uint32_t);
extern uint32_t printf_core(uint32_t,const uint8_t *,const uint32_t *);
extern uint32_t guest_of(const void *);
extern uint32_t text_render_full(uint32_t *,uint32_t,int);
extern uint32_t text_render_simple(uint32_t *,uint32_t,int);
extern void     prim_packet_init_tex(int,int,int,uint32_t,uint8_t *);
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
void print_dispatch(const char *fmt,...)
{
  __builtin_va_list ap;
  uint32_t v[8];
  /* buf = sp+0x28 (frame 0x130); assigned inside the reserved frame in
   * the mode==2 path below (`addiu $a0/$v0,$sp,0x28` in 0x8012cc6c,
   * stored to block+0x1c). */
  uint8_t *buf;
  uint16_t *b;
  uint32_t mode = BR2B32(0x1f800384);
  int i;
  __builtin_va_start(ap,fmt);
  for (i = 0; i < 8; i++) v[i] = (uint32_t)__builtin_va_arg(ap,uint32_t);
  __builtin_va_end(ap);
  if (mode == 1) { text9_dbg(fmt,v); return; }
  if ((int32_t)mode < 2) return;
  if (mode != 2) return;
  {
    /* buf = sp+0x28 (frame 0x130) -> guest_sp()-0x108. sprintf_core writes
     * it and text_render_simple re-reads it through the 0x1f80038c block
     * pointer, so reserve the guest frame window (sp+0x28 -> frm+0x28)
     * for the write->draw span. */
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t pd_frm = br2_guest_frame(0x130);
    buf = (uint8_t *)guest_at(pd_frm + 0x28);
    text9_spr(guest_of(buf),fmt,v);
    b = (uint16_t *)guest_at(0x1f80038c);
    b[3] = 0x1000; b[4] = 0x1000;
    b[5] = 8; b[6] = 8; b[7] = 8; b[8] = 8;
    b[0] = 0; b[1] = 0; b[2] = 0; b[9] = 0; b[10] = 0;
    b[11] = 0x78df; b[13] = 0;
    *(uint32_t *)(b + 14) = guest_of(buf);   /* +0x1c */
    text_render_simple((uint32_t *)guest_at(BR2B32(0x1f800008)),
                 BR2B16(0x1f80000c),(int)guest_of(b));
    br2_guest_frame_free();
  }
}

/* print_dispatch_selfdesc — same dispatcher but self-describing: mode 2 copies
 * caller +0x10/+0x12 u16 into block +2/+4 before the draw. */
void print_dispatch_selfdesc(void *self,const char *fmt,...)
{
  __builtin_va_list ap;
  uint32_t v[8];
  /* buf = sp+0x28 (frame 0x138); assigned inside the reserved frame in
   * the mode==2 path below (`addiu $a0/$v0,$sp,0x28` in 0x8012cf64,
   * stored to block+0x1c). */
  uint8_t *buf;
  uint16_t *b;
  uint8_t *selfb = (uint8_t *)self;
  uint32_t mode = BR2B32(0x1f800384);
  int i;
  __builtin_va_start(ap,fmt);
  for (i = 0; i < 8; i++) v[i] = (uint32_t)__builtin_va_arg(ap,uint32_t);
  __builtin_va_end(ap);
  if (mode == 1) { text9_dbg(fmt,v); return; }
  if ((int32_t)mode < 2) return;
  if (mode != 2) return;
  {
    /* buf = sp+0x28 (frame 0x138) -> guest_sp()-0x110. Same guest-frame
     * reservation as print_dispatch: sprintf writes, text_render_simple
     * re-reads through the 0x1f80038c block (sp+0x28 -> frm+0x28). */
    extern uint32_t br2_guest_frame(uint32_t);
    extern void br2_guest_frame_free(void);
    uint32_t pds_frm = br2_guest_frame(0x138);
    buf = (uint8_t *)guest_at(pds_frm + 0x28);
    text9_spr(guest_of(buf),fmt,v);
    b = (uint16_t *)guest_at(0x1f80038c);
    b[0] = 0;
    b[1] = *(uint16_t *)(selfb + 0x10);
    b[2] = *(uint16_t *)(selfb + 0x12);
    b[3] = 0x1000; b[4] = 0x1000;
    b[5] = 8; b[6] = 8; b[7] = 8; b[8] = 8;
    b[11] = 0x78df; b[9] = 0; b[10] = 0;
    *(uint32_t *)(b + 14) = guest_of(buf);   /* +0x1c */
    b[12] = 0;
    text_render_simple((uint32_t *)guest_at(BR2B32(0x1f800008)),
                 BR2B16(0x1f80000c),(int)guest_of(b));
    br2_guest_frame_free();
  }
}

/* ---- guest-ABI entries for the varargs print/format front-ends ----
 * The implementations above/below are host-varargs (they serve the many
 * clean call sites); the generated->clean dispatch wrapper can only
 * marshal a fixed arg list, so each *_guest entry re-emits the o32 arg
 * slots (a0-a3 + caller stack words at entry_sp+0x10..) as fixed params
 * and forwards them. The 8-param marshal window covers stack words
 * +0x10..+0x1c; deeper caller stack words are recovered from the live
 * guest stack via guest_sp()+0x20/+0x24 — the same slots the guest
 * prologue loads unconditionally. Forwarded args are cast to uintptr_t
 * so both va_arg(ap,uint32_t) and va_arg(ap,uintptr_t) consumers in the
 * callees read the correct low-32 bits. */

/* print_dispatch @0x8012cc6c — guest reads a0-a3 + 5 caller stack words
 * (args 5-9); a7 covers stack word 3, arg9 (+0x20) recovered. */
void print_dispatch_guest(const char *fmt,uint32_t a1,uint32_t a2,
                          uint32_t a3,uint32_t a4,uint32_t a5,
                          uint32_t a6,uint32_t a7)
{
  print_dispatch(fmt,(uintptr_t)a1,(uintptr_t)a2,
                 (uintptr_t)a3,(uintptr_t)a4,(uintptr_t)a5,
                 (uintptr_t)a6,(uintptr_t)a7,
                 (uintptr_t)BR2B32((uint32_t)(guest_sp() + 0x20)));
}

/* print_dispatch_selfdesc @0x8012cf64 — a0=self, a1=fmt, a2-a3 + 6 caller
 * stack words are the 8 varargs; stack words 4-5 (+0x20/+0x24) recovered. */
void print_dispatch_selfdesc_guest(void *self,const char *fmt,
                                   uint32_t a2,uint32_t a3,
                                   uint32_t a4,uint32_t a5,
                                   uint32_t a6,uint32_t a7)
{
  print_dispatch_selfdesc(self,fmt,(uintptr_t)a2,
                          (uintptr_t)a3,(uintptr_t)a4,
                          (uintptr_t)a5,(uintptr_t)a6,
                          (uintptr_t)a7,
                          (uintptr_t)BR2B32((uint32_t)(guest_sp() + 0x20)),
                          (uintptr_t)BR2B32((uint32_t)(guest_sp() + 0x24)));
}

/* print_mode_dispatch @0x8012ce30 — 9 fixed args; the clean impl is already
 * fixed-arity but exceeds the 8-param marshal window, so arg9 (+0x20) is
 * recovered from the caller stack here. */
void print_mode_dispatch_guest(uint32_t a0,uint32_t a1,
                               uint32_t a2,uint32_t a3,
                               uint32_t a4,uint32_t a5,
                               uint32_t a6,uint32_t a7)
{
  print_mode_dispatch(a0,a1,a2,a3,a4,a5,
                      a6,a7,
                      BR2B32((uint32_t)(guest_sp() + 0x20)));
}

/* printf_forward @0x8018e550 — packs a1-a3 + caller stack words into the
 * arg array and calls printf_core(1, fmt, args); the guest body tail-
 * propagates printf_core's v0, so the entry returns it. 8th vararg
 * (+0x20) recovered. */
uint32_t printf_forward_guest(const char *fmt,uint32_t a1,
                              uint32_t a2,uint32_t a3,
                              uint32_t a4,uint32_t a5,
                              uint32_t a6,uint32_t a7)
{
  uint32_t args[8];
  args[0] = a1; args[1] = a2; args[2] = a3;
  args[3] = a4; args[4] = a5; args[5] = a6;
  args[6] = a7;
  args[7] = BR2B32((uint32_t)(guest_sp() + 0x20));
  return printf_core(1,(const uint8_t *)fmt,args);
}

/* sprintf_core @0x8018ef60 — a0=dst, a1=fmt host pointers; a2-a3 + caller
 * stack words are the varargs; stack words 4-5 (+0x20/+0x24) recovered.
 * Returns the formatted length in v0 like the guest. */
int sprintf_core_guest(char *dst,char *fmt,uint32_t a2,
                       uint32_t a3,uint32_t a4,uint32_t a5,
                       uint32_t a6,uint32_t a7)
{
  return sprintf_core(dst,fmt,(uintptr_t)a2,(uintptr_t)a3,
                      (uintptr_t)a4,(uintptr_t)a5,(uintptr_t)a6,
                      (uintptr_t)a7,
                      (uintptr_t)BR2B32((uint32_t)(guest_sp() + 0x20)),
                      (uintptr_t)BR2B32((uint32_t)(guest_sp() + 0x24)));
}

/* text_win_printf @0x80190a04 — a0..a3 are the four named params; caller
 * stack words are the va_args; stack words 4-5 (+0x20/+0x24) recovered.
 * Returns the window-write count / -1 in v0 like the guest. */
int32_t text_win_printf_guest(int32_t a0,uint32_t a1,
                              uint32_t a2,uint32_t a3,
                              uint32_t a4,uint32_t a5,
                              uint32_t a6,uint32_t a7)
{
  return text_win_printf(a0,a1,a2,a3,(uintptr_t)a4,
                         (uintptr_t)a5,(uintptr_t)a6,
                         (uintptr_t)a7,
                         (uintptr_t)BR2B32((uint32_t)(guest_sp() + 0x20)),
                         (uintptr_t)BR2B32((uint32_t)(guest_sp() + 0x24)));
}

/* menu_row_paint / menu_row_paint_b — menu cursor row painters: emit "  > "
 * or "    " through print_dispatch_selfdesc when stepper uint8_t+4 is clear. */
void menu_row_paint(void *desc)
{
  uint8_t *p = (uint8_t *)desc;
  if (p[4] == 0)
    print_dispatch_selfdesc(desc,(const char *)guest_at(0x8011ec20));
}
void menu_row_paint_b(void *desc)
{
  uint8_t *p = (uint8_t *)desc;
  if (p[4] == 0)
    print_dispatch_selfdesc(desc,(const char *)guest_at(0x8011ec28));
}

/* menu_stepper_redraw — menu stepper redraw: cb(+0xc) with 0x100, then for
 * i = 1..h(+6) paint row i (menu_row_paint when i == sel(+8) else
 * menu_row_paint_b) and cb with 0x100+i, finally cb with 0x1ff. */
void menu_stepper_redraw(void *desc)
{
  uint8_t *p = (uint8_t *)desc;
  void (*cb)(void *,uint32_t);
  uint32_t i;
  /* p+0xc is a u32 GUEST code address (same field menu_stepper_state_advance
   * resolves via fn_lookup) — not a host pointer. Resolve it the same way.
   * Every guest jalr is unconditional: a0 = desc guest addr, a1 = row code;
   * unported targets fall back to call_guest so the generated body runs. */
  uint32_t cb_g = *(uint32_t *)(p + 0xc);
  cb = (void (*)(void *,uint32_t))fn_lookup(cb_g);
  if (cb != NULL) cb(desc,0x100);
  else            call_guest(cb_g,guest_of(desc),0x100);
  i = 1;
  for (;;) {
    uint32_t code;
    if (i == *(uint16_t *)(p + 8)) menu_row_paint(desc);
    else                            menu_row_paint_b(desc);
    code = (i + 0x100) & 0xffff;
    if (cb != NULL) cb(desc,code);
    else            call_guest(cb_g,guest_of(desc),code);
    i++;
    if (*(uint16_t *)(p + 6) < i) break;
  }
  if (cb != NULL) cb(desc,0x1ff);
  else            call_guest(cb_g,guest_of(desc),0x1ff);
}

/* menu_stepper_input — menu stepper input: pad flags @0x1f80007e; uint8_t+5
 * selects button pair (0: 0x4000/0x1000 sfx 0xa, else 0x2000/0x8000
 * sfx 0xb). Up wraps cur(+8) to 1 past max(+6); down wraps 1 -> max. */
void menu_stepper_input(void *desc)
{
  uint8_t *p = (uint8_t *)desc;
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
void hud_text_a(uint32_t ot_ptr,uint16_t flags,const void *str,
                  uint16_t attr_a,uint16_t attr_b,short tile_row)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(str);
  b[3] = (uint16_t)attr_a;
  b[5] = 8; b[7] = 0xffff;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[6] = 0x10; b[8] = 0x10; b[9] = 0; b[10] = 1; b[12] = 1;
  b[4] = (uint16_t)attr_b;
  b[11] = (uint16_t)((((uint32_t)tile_row + 0x1e0) & 0x1ff) * 0x40 + 0x1f);
  text_render_full((uint32_t *)guest_at(ot_ptr),(uint32_t)flags & 0xffff,
               (int)guest_of(b));
}

/* hud_text_b — HUD text variant B (uniform 0xf metrics, +0x16). */
void hud_text_b(uint32_t ot_ptr,uint32_t flags,const void *str,
                  uint32_t attr,uint16_t attr_b,short tile_row)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(str);
  b[5] = 0xf; b[6] = 0xf; b[7] = 0xf; b[8] = 0xf;
  b[9] = 1; b[10] = 1;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[3] = (uint16_t)attr;
  b[12] = 4;
  b[4] = (uint16_t)attr_b;
  b[11] = (uint16_t)((((uint32_t)tile_row + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  text_render_full((uint32_t *)guest_at(ot_ptr),(uint32_t)flags & 0xffff,
               (int)guest_of(b));
}

/* hud_text_c — HUD text variant C (0x1e line gap, +0x16). */
void hud_text_c(uint32_t ot_ptr,uint32_t flags,const void *str,
                  uint32_t attr,uint16_t attr_b,uint32_t tile_row)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(str);
  b[3] = (uint16_t)attr;
  b[6] = 0x1e;
  b[9] = 1; b[10] = 1;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[5] = 0xf; b[7] = 0xf; b[8] = 0xf;
  b[12] = 4;
  b[4] = (uint16_t)attr_b;
  b[11] = (uint16_t)((((uint32_t)tile_row + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  text_render_full((uint32_t *)guest_at(ot_ptr),(uint32_t)flags & 0xffff,
               (int)guest_of(b));
}

/* hud_text_d — HUD text variant D (metric 8, flag 5, +0x16). */
void hud_text_d(uint32_t ot_ptr,uint32_t flags,const void *str,
                  int32_t attr,int32_t attr_b,int32_t tile_row)
{
  uint16_t *b = (uint16_t *)guest_at(0x1f80038c);
  BR2B32(0x1f8003a8) = guest_of(str);
  b[3] = (uint16_t)attr;
  b[7] = 0xffff;
  b[9] = 1; b[10] = 1;
  b[0] = 0; b[1] = 0; b[2] = 0;
  b[5] = 8; b[6] = 8; b[8] = 8;
  b[12] = 5;
  b[4] = (uint16_t)attr_b;
  b[11] = (uint16_t)((((uint32_t)tile_row + 0x1e0) & 0x1ff) * 0x40 + 0x16);
  text_render_full((uint32_t *)guest_at(ot_ptr),flags & 0xffff,
               (int)guest_of(b));
}

/* sprite_node_alloc — sprite-node pool alloc: 8 slots of 0x34 at
 * 0x801f5d38, generation counter @0x1f8000dc, cursor @0x801b55e8.
 * Returns slot+4 host pointer or NULL when generation matches. */
void *sprite_node_alloc(void)
{
  int32_t slot_idx = (int32_t)BR2B32(0x801b55e8);
  uint32_t gen = BR2B32(0x1f8000dc);
  uint8_t *slot;
  if (slot_idx < 0) slot_idx = 0;
  slot = (uint8_t *)guest_at(0x801f5d38 + (uint32_t)slot_idx * 0x34);
  if (*(uint32_t *)slot == gen || *(uint32_t *)slot == gen - 1)
    return (void *)0;
  slot_idx++;
  if (slot_idx >= 8) slot_idx = 0;
  BR2B32(0x801b55e8) = (uint32_t)slot_idx;
  *(uint32_t *)slot = gen;
  return slot + 4;
}

/* sprite_node_emit_a — sprite-list node emit (pool sprite_node_alloc, tag
 * 0x08000000, packet tail +0x24). 10 args; variant selects GPU
 * command variant 0x3a/0x38000000 and stride. */
int sprite_node_emit_a(uint32_t head_ptr,int32_t x,int32_t y,
                 int32_t w,int32_t h,uint32_t cmd,
                 uint32_t aux0,uint32_t aux1,uint32_t aux2,
                 int32_t variant)
{
  uint32_t *head = (uint32_t *)guest_at(head_ptr);
  uint8_t *node = (uint8_t *)sprite_node_alloc();
  uint32_t guest;
  int32_t tag;
  if (node == (uint8_t *)0) return -1;
  guest = guest_of(node);
  *(uint32_t *)(node + 0x0c) = aux0;
  *(uint32_t *)(node + 0x14) = aux1;
  *(uint16_t *)(node + 0x08) = (uint16_t)x;
  *(uint16_t *)(node + 0x0a) = (uint16_t)y;
  *(uint16_t *)(node + 0x10) = (uint16_t)(x + w);
  *(uint16_t *)(node + 0x12) = (uint16_t)y;
  *(uint16_t *)(node + 0x18) = (uint16_t)x;
  *(uint16_t *)(node + 0x1a) = (uint16_t)(y + h);
  *(uint32_t *)(node + 0x1c) = aux2;
  *(uint16_t *)(node + 0x20) = (uint16_t)(x + w);
  *(uint16_t *)(node + 0x22) = (uint16_t)(y + h);
  if (variant != 0) {
    *(uint32_t *)(node + 4) = cmd | 0x3a000000;
    tag = (variant - 1) << 5;
  } else {
    *(uint32_t *)(node + 4) = cmd | 0x38000000;
    tag = 0;
  }
  prim_packet_init_tex((int)guest_of(node + 0x24),
               BR2B8(0x801e40b7),BR2B8(0x801e40b6),(uint32_t)tag,(uint8_t *)0);
  *(uint32_t *)node = *head | 0x08000000;
  *head = guest & 0xffffff;
  *(uint32_t *)(node + 0x24) = (guest & 0xffffff) | 0x02000000;
  *head = (guest + 0x24) & 0xffffff;
  return 0;
}

/* sprite_node_emit_b — sprite-list node emit (pool spr_desc_slot_alloc, tag
 * 0x04000000, packet tail +0x14). xy packs x|y<<16,
 * wh packs w|h<<16; color low uint8_t = colour, variant picks
 * GPU variant 0x66/0x64000000 + stride. */
int sprite_node_emit_b(uint32_t head_ptr,uint32_t xy,uint32_t wh,
                 uint32_t aux,int32_t color,int32_t variant)
{
  uint32_t *head = (uint32_t *)guest_at(head_ptr);
  uint8_t *node = (uint8_t *)spr_desc_slot_alloc();
  uint32_t guest;
  int32_t tag;
  if (node == (uint8_t *)0) return -1;
  guest = guest_of(node);
  *(uint16_t *)(node + 0x10) = (uint16_t)((((int32_t)wh >> 16) & 0xff) + 1);
  *(uint16_t *)(node + 0x12) = (uint16_t)(((uint32_t)wh >> 24) + 1);
  *(uint32_t *)(node + 0x0c) = aux;
  *(uint16_t *)(node + 0x08) = (uint16_t)xy;
  *(uint16_t *)(node + 0x0a) = (uint16_t)(xy >> 16);
  node[7] |= 1;
  if (variant != 0) {
    uint32_t c = color & 0xff;
    *(uint32_t *)(node + 4) = (c | (c << 8) | (c << 16)) | 0x66000000;
    tag = (variant - 1) << 5;
  } else {
    uint32_t c = color & 0xff;
    *(uint32_t *)(node + 4) = (c | (c << 8) | (c << 16)) | 0x64000000;
    tag = 0;
  }
  prim_packet_init_tex((int)guest_of(node + 0x14),
               BR2B8(0x801e40b7),BR2B8(0x801e40b6),
               (uint32_t)((int32_t)(wh << 16) >> 16) | (uint32_t)tag,
               (uint8_t *)0);
  *(uint32_t *)node = *head | 0x04000000;
  *head = guest & 0xffffff;
  *(uint32_t *)(node + 0x14) = (guest & 0xffffff) | 0x02000000;
  *head = (guest + 0x14) & 0xffffff;
  return 0;
}


/* frame_counter_bump — bump uint8_t counter @0x1f800012 by arg+1. Disasm-verified. */
void frame_counter_bump(int n)
{
  *(uint8_t *)guest_at(0x1f800012) = (uint8_t)(n + 1);
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
uint32_t u16_lut_lookup(uint32_t key)
{
  uint32_t idx = key & 0xffff;
  uint32_t tbl;
  if (idx >= 3) return 0;
  tbl = *(uint32_t *)guest_at(0x801b66e4);
  return *(uint16_t *)guest_at(tbl + idx * 0x10);
}

/* u16_lut_slot_clear — sibling of u16_lut_lookup: idx=a0&0xffff; <3
 * clears the u16 at *(u32*)0x801b66e4 + idx*0x10 and returns 1, else 0.
 * Disasm 0x8018e458-0x8018e488 (lui/addiu base, sll idx,4, sh $zero). */
uint32_t u16_lut_slot_clear(uint32_t key)
{
  uint32_t idx = key & 0xffff;
  uint32_t tbl;
  if (idx >= 3) return 0;
  tbl = *(uint32_t *)guest_at(0x801b66e4);
  *(uint16_t *)guest_at(tbl + idx * 0x10) = 0;
  return 1;
}

/* lut_mask_or — per-index attribute mask OR: idx=a0&0xffff;
 * mask = *(u32*)(0x801b66e8 + idx*4); base = *(u32*)0x801b66e0;
 * *(u32*)(base+4) |= mask; returns (int32)idx < 3.
 * Disasm 0x8018e3f4-0x8018e420. */
uint32_t lut_mask_or(uint32_t key)
{
  uint32_t idx = key & 0xffff;
  uint32_t base = *(uint32_t *)guest_at(0x801b66e0);
  uint32_t mask = *(uint32_t *)guest_at(0x801b66e8 + idx * 4);
  uint32_t flags = *(uint32_t *)guest_at(base + 4);
  *(uint32_t *)guest_at(base + 4) = flags | mask;
  return (uint32_t)((int32_t)idx < 3);
}

/* lut_mask_clear — same table walk as lut_mask_or but clears the mask:
 * *(u32*)(base+4) &= ~mask; always returns 1.
 * Disasm 0x8018e424-0x8018e454 (not + and+slti absent). */
uint32_t lut_mask_clear(uint32_t key)
{
  uint32_t idx = key & 0xffff;
  uint32_t base = *(uint32_t *)guest_at(0x801b66e0);
  uint32_t mask = *(uint32_t *)guest_at(0x801b66e8 + idx * 4);
  uint32_t flags = *(uint32_t *)guest_at(base + 4);
  *(uint32_t *)guest_at(base + 4) = flags & ~mask;
  return 1;
}

/* rand_seed — LCG state seed: *(u32*)0x801f68e0 = a0 (the state word
 * rand_lcg @0x8018e4d0 advances as state*0x41c64e6d+0x3039).
 * Disasm 0x8018e500-0x8018e510 (lui 0x801f, sw a0, +0x68e0). */
void rand_seed(uint32_t seed)
{
  *(uint32_t *)guest_at(0x801f68e0) = seed;
}

/* ctype_adj_m32 — printf ctype-table adjust: c = ctype_tbl[a0 & 0xff]
 * (@0x801b6701); when (c & 2) returns (int8_t)(a0 - 32), else (int8_t)a0.
 * Disasm 0x8018ec30-0x8018ec60 (lb, andi 2, a0-32 vs a0, <<24 >>24). */
int32_t ctype_adj_m32(uint32_t ch)
{
  uint8_t c = *(uint8_t *)guest_at(0x801b6701 + (ch & 0xff));
  int32_t r = (c & 2) ? (int32_t)(ch - 32) : (int32_t)ch;
  return (int8_t)r;
}

/* ctype_adj_p32 — printf ctype-table adjust: c = ctype_tbl[a0 & 0xff];
 * when (c & 1) returns (int8_t)(a0 + 32), else (int8_t)a0.
 * Disasm 0x8018ec64-0x8018ec94 (lb, andi 1, a0+32 vs a0, <<24 >>24). */
int32_t ctype_adj_p32(uint32_t ch)
{
  uint8_t c = *(uint8_t *)guest_at(0x801b6701 + (ch & 0xff));
  int32_t r = (c & 1) ? (int32_t)(ch + 32) : (int32_t)ch;
  return (int8_t)r;
}

/* file_rec_size_get — CD file-record size getter: a0 < 0 (signed) -> 0;
 * else returns word1 of the 8-uint8_t record @0x801b5b94 + a0*8 (the
 * (size+0x7ff)>>0xb page count written by pad_record_init).
 * Disasm 0x8018cfb4-0x8018cfd0 (bltz a0 -> shared ret-0 tail 0x8018cfd4,
 * sll a0,3, lw +4). */
uint32_t file_rec_size_get(int32_t file_id)
{
  if (file_id < 0) return 0;
  return *(uint32_t *)guest_at(0x801b5b98 + (uint32_t)file_id * 8);
}

/* set_graph_debug — SetGraphDebug level setter: uint8_t @0x801b8322 = arg;
 * when nonzero calls PTR_FUN_801b831c (text_win_printf text-window printf)
 * with "SetGraphDebug:level:%d,type:%d reverse:%d". Returns old uint8_t.
 * Disasm 0x80190f74-0x80190fc8. */
extern void *PTR_FUN_801b831c;
int32_t set_graph_debug(uint32_t level)
{
  uint8_t *flag = (uint8_t *)guest_at(0x801b8322);
  int32_t old = flag[0];
  int (*cb)(int32_t,uint32_t,uint32_t,uint32_t,...) =
      (int (*)(int32_t,uint32_t,uint32_t,uint32_t,...))PTR_FUN_801b831c;
  flag[0] = (uint8_t)level;
  if ((level & 0xff) == 0) return old;
  cb(0x80125444,flag[0],flag[-2],flag[1]);
  return old;
}

/* memcard_info_query — memcard/info query: mc_data_read(0, tbl[0..2],
 * tbl[3..9], 0x801c11e0, 0x414) over table @0x801b45f4; ret 0 clears
 * @0x801c15f0 and returns uint8_t @(*(u32*)0x1f8000c0)+0x22, ret<8&&!=0
 * ->1, ret>=8 ->0. Disasm 0x8015f590-0x8015f63c. */
extern int32_t mc_data_read(int32_t,uint32_t,uint32_t,uint32_t,uint32_t,
                            uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,
                            uint32_t,uint32_t,uint32_t);
int32_t memcard_info_query(void)
{
  uint32_t *tbl = (uint32_t *)guest_at(0x801b45f4);
  int32_t ret;
  if (tbl == NULL) return -1;
  ret = mc_data_read(0,tbl[0],tbl[1],tbl[2],tbl[3],tbl[4],tbl[5],tbl[6],tbl[7],tbl[8],tbl[9],
                   0x801c11e0,0x414);
  /* Generated 0x8015F590-0x8015F63C: every ret!=0 path returns v0=0
   * (li v0,0 in the 0x8015F610 delay slot); only ret==0 clears
   * 0x801c15f0 and returns the config uint8_t. */
  if (ret != 0) return 0;
  *(uint32_t *)guest_at(0x801c15f0) = 0;
  return *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x1f8000c0) + 0x22);
}

/* debug_stat_print — debug stat print: scroll_offset_set(0,0xdc) then
 * print_dispatch("CPU(%d)\nGPU(%d)\n" or "#2CPU(%d)\nGPU(%d)\n",
 * u16@0x1f80001a, u16@0x1f80001c) selected by u16@0x801e83a8.
 * Disasm 0x8012a088-0x8012a0e8. */
void debug_stat_print(void)
{
  uint16_t cpu_ts = *(uint16_t *)guest_at(0x1f80001a);
  uint16_t gpu_ts = *(uint16_t *)guest_at(0x1f80001c);
  scroll_offset_set(0,0xdc);
  if (*(uint16_t *)guest_at(0x801e83a8) == 0)
    print_dispatch((const char *)guest_at(0x8011ed40),cpu_ts,gpu_ts);
  else
    print_dispatch((const char *)guest_at(0x8011ed54),cpu_ts,gpu_ts);
}

/* players_ctrl_init — two-player control-block init: fills guest structs at
 * 0x1f8000e8 / 0x1f8001f0 and descriptor tables 0x801f5258/0x801f52a0
 * with ptr pairs into 0x801e1b3c region. Disasm 0x801757b8-0x80175890. */
void players_ctrl_init(void)
{
  uint8_t *ctrl_a = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *ctrl_b = (uint8_t *)guest_at(0x1f8001f0);
  uint8_t *tbl_a = (uint8_t *)guest_at(0x801f5258);
  uint8_t *tbl_b = tbl_a + 0x48;
  const uint32_t base = 0x801e1b3c;
  if (!ctrl_a || !ctrl_b || !tbl_a) return;
  ctrl_a[2] = 0; ctrl_a[3] = 0;
  *(uint32_t *)(ctrl_a + 0x28) = 0x801f5258;
  *(uint32_t *)(tbl_a + 0x3c) = base + 0x870;
  *(uint32_t *)(tbl_a + 0x1c) = base - 0xbd0;
  *(uint32_t *)(tbl_a + 0x40) = base - 0x8dc;
  *(uint32_t *)(tbl_a + 0x18) = base;
  *(uint32_t *)(ctrl_a + 0x44) = base - 0x1950;
  *(uint32_t *)(ctrl_a + 0x48) = base - 0x1290;
  *(uint32_t *)(ctrl_a + 0x2c) = 0x1f8001f0;
  *(uint32_t *)(ctrl_a + 0x38) = 0x801f5328;
  *(uint32_t *)(ctrl_a + 0x3c) = 0x801f55c0;
  ctrl_b[2] = 1; ctrl_b[3] = 5;
  *(uint32_t *)(ctrl_b + 0x28) = 0x801f52a0;
  *(uint32_t *)(tbl_b + 0x18) = base + 0x10e0;
  *(uint32_t *)(tbl_b + 0x3c) = base + 0x1950;
  *(uint32_t *)(tbl_b + 0x1c) = base - 0x5e8;
  *(uint32_t *)(tbl_b + 0x40) = base - 0x2f4;
  *(uint32_t *)(ctrl_b + 0x44) = base - 0x15f0;
  *(uint32_t *)(ctrl_b + 0x48) = base - 0xf30;
  *(uint32_t *)(ctrl_b + 0x2c) = 0x1f8000e8;
  *(uint32_t *)(ctrl_b + 0x38) = 0x801f5440;
  *(uint32_t *)(ctrl_b + 0x3c) = 0x801f5704;
  *(uint8_t *)guest_at(0x1f800098) = 0;
}

/* ot_packet_chain_fixup — OT/GPU packet-chain fixup: positions two 0x18-uint8_t
 * records at 0x801e83e8/0x801e83d0 from u16 fields + scroll offsets,
 * then links five slots at 0x801e8348..0x801e8388 plus tail slots
 * 0x801e8398/0x801e83d0/0x801e83b8: each slot gets (*T1 | tag) and
 * *T1 becomes the slot's 24-bit address. T1 = *(u32*)0x1f800008.
 * Disasm 0x8012a30c-0x8012a4b0. */
void ot_packet_chain_fixup(void)
{
  uint32_t pos;
  uint32_t ot_head = *(uint32_t *)guest_at(0x1f800008);
  static const uint32_t offs[5] = {0x10,0x20,0x30,0x40,0x50};
  int i;
  uint8_t *rec_a = (uint8_t *)guest_at(0x801e83e8);
  uint8_t *rec_b = (uint8_t *)guest_at(0x801e83d0);
  if (rec_a == NULL || rec_b == NULL) return;
  pos = (uint32_t)*(uint16_t *)(rec_a + 8) + *(uint16_t *)guest_at(0x1f80001a);
  *(uint16_t *)(rec_a + 0x14) = (uint16_t)pos;
  *(uint16_t *)(rec_a + 0xc)  = (uint16_t)pos;
  pos = (uint32_t)*(uint16_t *)(rec_b + 8) + *(uint16_t *)guest_at(0x1f80001c);
  *(uint16_t *)(rec_b + 0x14) = (uint16_t)pos;
  *(uint16_t *)(rec_b + 0xc)  = (uint16_t)pos;
  pos = (uint32_t)*(uint16_t *)(rec_b + 8) + *(uint16_t *)guest_at(0x1f80001e);
  *(uint16_t *)guest_at(0x801e83a4) = (uint16_t)pos;
  *(uint16_t *)guest_at(0x801e83a0) = (uint16_t)pos;
  for (i = 0; i < 5; i++) {
    uint32_t slot = 0x801e8398 - offs[i];
    uint32_t v = *(uint32_t *)guest_at(ot_head) | 0x3000000;
    *(uint32_t *)guest_at(slot) = v;
    *(uint32_t *)guest_at(ot_head) = slot & 0xffffff;
  }
  {
    uint32_t slot = 0x801e8398;
    *(uint32_t *)guest_at(slot) = *(uint32_t *)guest_at(ot_head) | 0x3000000;
    *(uint32_t *)guest_at(ot_head) = slot & 0xffffff;
  }
  {
    uint32_t slot = 0x801e83d0;
    *(uint32_t *)guest_at(slot) = *(uint32_t *)guest_at(ot_head) | 0x5000000;
    *(uint32_t *)guest_at(ot_head) = slot & 0xffffff;
  }
  {
    uint32_t slot = 0x801e83b8;
    *(uint32_t *)guest_at(slot) = *(uint32_t *)guest_at(ot_head) | 0x5000000;
    *(uint32_t *)guest_at(ot_head) = slot & 0xffffff;
  }
}

/* sprite_rect_table_init — sprite/UV rect table init: spr_prim_tag_a x3 over
 * 0x18-stride recs @0x801e83b8, fills three textured-quad descriptors
 * (u16 pos/uv fields from args, uint8_t tags 0x28/0xc8), spr_prim_tag_b x6
 * over 0x10-stride recs @0x801e8348, then six flat-shade quads.
 * Disasm 0x8012a0f0-0x8012a304. */
extern void spr_prim_tag_a(uint32_t);
extern void spr_prim_tag_b(uint32_t);
static void sh16(uint32_t addr,uint32_t off,uint32_t val)
{ *(uint16_t *)((uint8_t *)guest_at(addr) + off) = (uint16_t)val; }
static void sb8(uint32_t addr,uint32_t off,uint32_t val)
{ *(uint8_t *)((uint8_t *)guest_at(addr) + off) = (uint8_t)val; }
void sprite_rect_table_init(int32_t x0,int32_t y0,int32_t unused)
{
  uint32_t x = (uint32_t)x0, y = (uint32_t)y0;
  (void)unused;
  uint32_t rec0 = 0x801e83b8, rec1 = rec0 + 0x18, rec2 = rec0 + 0x30;
  uint32_t b, i;
  for (i = 0; i < 3; i++) spr_prim_tag_a(rec0 + i * 0x18);
  /* three 0x18 records */
  sh16(rec0,0x12,y+0xf);  sh16(rec0,0x16,y+0xf);
  sh16(rec0,0x08,x);      sh16(rec0,0x0a,y);
  sh16(rec0,0x0c,x+0xff); sh16(rec0,0x0e,y);
  sh16(rec0,0x10,x);      sh16(rec0,0x14,x+0xff);
  sh16(rec1,0x0a,y+9);    sh16(rec1,0x0e,y+9);
  sh16(rec1,0x12,y+0xe);  sh16(rec1,0x16,y+0xe);
  sh16(rec1,0x08,x);      sh16(rec1,0x0c,x+0xff);
  sh16(rec1,0x10,x);      sh16(rec1,0x14,x+0xff);
  sh16(rec2,0x0a,y+1);    sh16(rec2,0x0e,y+1);
  sh16(rec2,0x12,y+6);    sh16(rec2,0x16,y+6);
  sh16(rec2,0x08,x);      sh16(rec2,0x0c,x+0xff);
  sh16(rec2,0x10,x);      sh16(rec2,0x14,x+0xff);
  sb8(rec0,4,0x28); sb8(rec0,5,0x28); sb8(rec0,6,0x28);
  sb8(rec1,4,0xc8); sb8(rec1,5,0xc8); sb8(rec1,6,0);
  sb8(rec2,4,0xc8); sb8(rec2,5,0);    sb8(rec2,6,0);
  for (i = 0; i < 6; i++) spr_prim_tag_b(0x801e8348 + i * 0x10);
  /* six 0x10 records */
  {
    static const uint32_t xs[6] = {0xfa,0xc8,0x96,0x64,0x32,0};
    uint32_t r;
    for (i = 0; i < 6; i++) {
      r = 0x801e8348 + i * 0x10;
      sh16(r,0x08, i == 5 ? x : x + xs[i]);
      sh16(r,0x0c, i == 5 ? x : x + xs[i]);
      sh16(r,0x0a, y);
      sh16(r,0x0e, y + 0xf);
      if (i == 5) { sb8(r,4,0xff); sb8(r,5,0xff); sb8(r,6,0xff); }
      else        { sb8(r,4,0);    sb8(r,5,0);    sb8(r,6,(i & 1) ? 0xff : 0x40); }
    }
  }
}

/* empty_ret — empty function (guest: jr ra; nop). */
void empty_ret(void) { }

/* empty_leaf_a/b/c/d — standalone `jr ra; nop` function heads
 * (verified F-records + generated bodies): bind_jal waypoint/callback
 * slots that intentionally do nothing. */
void empty_leaf_a(void) { }
void empty_leaf_b(void) { }
void empty_leaf_c(void) { }
void empty_leaf_d(void) { }

/* clear_f0300_records — clear 0x34-stride records @0x801f0300, count =
 * s16 @0x801f04a8+0x14. Disasm 0x801360cc-0x80136108+. */
void clear_f0300_records(void)
{
  uint16_t *recs = (uint16_t *)guest_at(0x801f0300);
  int16_t *cnt = (int16_t *)guest_at(0x801f04a8 + 0x14);
  int32_t n = cnt ? *cnt : 0, i;
  for (i = 0; i < n; i++) recs[i * 0x1a] = 0;   /* 0x34 bytes = 0x1a u16 */
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
  uint32_t block = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = 0;
  *(uint32_t *)guest_at(block + 4) = 0x100;
  cd_chan_mode_apply(block);
  cd_chan_mode_set(0);
}

/* clear_f4ee8_u16s — clear 10 u16s at 0x801f4ee8 + uint8_t(a0+2)*24 + 0x12,
 * plus u16 @+0x14/+0x16 and 9 words before +0x12. Disasm verified. */
void clear_f4ee8_u16s(uint32_t src)
{
  uint8_t *src_b = (uint8_t *)guest_at(src);
  uint8_t *rec = (uint8_t *)guest_at(0x801f4ee8) + src_b[2] * 24;
  uint16_t *wp = (uint16_t *)(rec + 0x12);
  int i;
  rec[0x14] = 0; rec[0x15] = 0;
  *(uint16_t *)(rec + 0x16) = 0;
  for (i = 9; i >= 0; i--) *wp-- = 0;
}

/* blk_1f26c0_init_b — block @0x801f26c0 variant init: +0=7, +4=0x100,
 * +8/+0xa=0x3fff, clear @0x801f36c4, cd_chan_mode_apply, cd_chan_flags_to_dir,
 * cd_chan_mode_set(1). Disasm 0x80155dd0-0x80155e28. */
extern void cd_chan_flags_to_dir(uint32_t);
void blk_1f26c0_init_b(void)
{
  uint32_t block = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = 0;
  *(uint32_t *)guest_at(block) = 7;
  *(uint32_t *)guest_at(block + 4) = 0x100;
  *(uint16_t *)guest_at(block + 8) = 0x3fff;
  *(uint16_t *)guest_at(block + 0xa) = 0x3fff;
  cd_chan_mode_apply(block);
  cd_chan_flags_to_dir(block);
  cd_chan_mode_set(1);
}

/* scratchpad_pair_init — scratchpad pair init: select_phase_update(0x1f800000),
 * clear +0x99/+0x9a/+0xe4/+0xf4 on 0x1f8000e8 & 0x1f8001f0,
 * bump u16 +0xe8 on both, gauge_leaf_update. Disasm 0x8016fbfc-6c6c. */
extern int select_phase_update(uint32_t);
extern int32_t gauge_leaf_update(uint32_t);
void scratchpad_pair_init(void)
{
  uint8_t *ctrl_a = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *ctrl_b = (uint8_t *)guest_at(0x1f8001f0);
  uint8_t *base = (uint8_t *)guest_at(0x1f800000);
  (void)select_phase_update(0x1f800000);
  if (base) { base[0x99] = 0; base[0x9a] = 0; }
  if (ctrl_a) {
    ctrl_a[0xe4] = 0;
    *(uint32_t *)(ctrl_a + 0xf4) = 0;
    *(uint16_t *)(ctrl_a + 0xe8) += 1;
  }
  if (ctrl_b) {
    ctrl_b[0xe4] = 0;
    *(uint32_t *)(ctrl_b + 0xf4) = 0;
    *(uint16_t *)(ctrl_b + 0xe8) += 1;
  }
  gauge_leaf_update(0x1f800000);
}

/* reset_sprite_ot_block — reset sprite/OT block: zero 8 u16 at *(a0+0xcc),
 * s8(+0x98)->+0x12, s8(+0x99)->+0x14, lhu(+0xa0)->+0x16, then five
 * u16=-1 stores stride -0x235c from 0x801cd530. a0 guest addr.
 * Disasm 0x80127be4-0x80127c58. */
void reset_sprite_ot_block(uint32_t ctx)
{
  uint8_t *ctx_b = (uint8_t *)guest_at(ctx);
  uint16_t *tbl;
  uint32_t addr;
  int i;
  if (ctx_b == NULL) return;
  tbl = (uint16_t *)guest_at(*(uint32_t *)(ctx_b + 0xcc));
  if (tbl == NULL) return;
  for (i = 0; i < 8; i++) tbl[i] = 0;
  tbl[9] = (uint16_t)(int8_t)ctx_b[0x98];
  tbl[10] = (uint16_t)(int8_t)ctx_b[0x99];
  tbl[11] = *(uint16_t *)(ctx_b + 0xa0);
  addr = 0x801cd530;
  for (i = 4; i >= 0; i--) {
    *(uint16_t *)guest_at(addr) = 0xffff;
    addr -= 0x235c;
  }
}

/* reset_combat_record — reset combat record block: 16 sub-slots stride 4
 * at 0x801f0fb0 + a0*0x84: [i]=0, [i+0x40]=0x64. Disasm verified. */
void reset_combat_record(int32_t slot)
{
  uint8_t *base = (uint8_t *)guest_at(0x801f0fb0 + slot * 0x84);
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
void blk_1f26c0_init_c(int32_t mode,uint32_t bound)
{
  uint32_t block = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = (uint32_t)mode;
  *(uint32_t *)guest_at(block) = 7;
  *(uint32_t *)guest_at(block + 4) = (uint32_t)mode | 0x100;
  *(uint16_t *)guest_at(block + 8) = (uint16_t)bound;
  *(uint16_t *)guest_at(block + 0xa) = (uint16_t)bound;
  cd_chan_mode_apply(block);
  cd_chan_flags_to_dir(block);
  cd_reg_win_rmw_cc_cd(1,0xffffFF08);
  cd_chan_mode_set(1);
}

/* record_copy_1a — copy 0x1a-uint8_t record 0x8011fea0 + idx*0x1a ->
 * 0x801f0640 then hud_sprite_desc_init. Disasm 0x80136d28-0x80136dd4. */
void record_copy_1a(int idx)
{
  memcpy(guest_at(0x801f0640),guest_at(0x8011fea0 + idx * 0x1a),0x1a);
  hud_sprite_desc_init();
}
