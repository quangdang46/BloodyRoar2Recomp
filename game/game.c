/* game.c — top-level game shell: boot, main state machine,
 * CD file pump, frame driver, match loop, stage load.
 *
 * Guest-faithful verbatim ports (Ghidra decompiles preserved in
 * decomp/). Guest addresses are passed as raw ints per project
 * convention; globals via the g_DAT arena bindings in bind.c.
 *
 * Decoded architecture (live + static verified):
 *   entry_0x80126178  _start: BSS clear -> entry_0x80126234
 *   entry_0x80126234  main(): boot(FUN_80126f04) -> settings ->
 *                     g_game_state SM (0-13) via switchD_80126370
 *   FUN_80126f04      boot: ~25 subsystem inits + CD file pump
 *                     FUN_80126870 until state 0x120 -> -1
 *   FUN_80126870      CD streaming pump: loads ~27 files into
 *                     _DAT_801e3cfc heap cursor, DMA queue per file
 *   FUN_8012aeec      frame driver: pad poll, GPU sync/DMA kick,
 *                     per-fighter buffer toggle (+0xc 0/1),
 *                     calls FUN_8016362c logic + FUN_8012cd9c end
 *   FUN_80147034      match loop: DAT_1f800001 stage machine,
 *                     300-frame attract timeout, FUN_8016347c logic
 *   FUN_80159028      scene/stage entry: file-table load, VRAM
 *                     window setup, stream reader, palette, then
 *                     FUN_8012ae3c(1) battle start + 2 frames
 *   FUN_8012ae3c      battle session arm: fighter buf ptrs +0x4/+8
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
extern uint32_t *g_PTR_DAT_801b5b94; /* file ptr table */
extern uint8_t  *g_DAT_801b5b98;
extern uint8_t  *g__DAT_801f68d8;
extern uint8_t  *g_DAT_801b55ec;
extern uint8_t  *_g_pad_buffer;
extern uint8_t  *_g_system_config_ptr;

#define DAT_1f800000    (*(uint32_t *)g_DAT_1f800000)
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
int FUN_80157368();
int FUN_80195d8c();
int FUN_80198af0();
int FUN_80198e2c();
int FUN_8018d150();
void FUN_8012a884(uint8_t *);
extern void entry_0x80126234(void);

typedef void code(void);

/* ---- callee externs (ported in later batches; weak-bound meanwhile) ---- */
extern void FUN_80157a00(void);
extern void FUN_80156d98();
extern void FUN_8012a884(uint8_t *);
extern void FUN_80156ff0(void);
extern uint FUN_8019a460(int);
extern void FUN_8018bd50(int,int);
extern void FUN_8012a72c(intptr_t);
extern void FUN_8019117c(int);
extern void FUN_80190e00(int);
extern void FUN_801919ac(int);
extern void FUN_801917e0(int);
extern void FUN_801910e4(int);
extern void FUN_80191428(int,int);
extern void FUN_80191770(int);
extern void FUN_80191300(int,int,uint8_t,uint8_t);
extern uint32_t FUN_80191ed8(void);
extern void FUN_80129f78(void);
extern void FUN_8016362c(int);
extern void FUN_8012a088(void);
extern void FUN_8012a30c(void);
extern void FUN_8012cd9c(int,int);
extern void FUN_8012a0f0(int,int,int);
extern void FUN_80191668(uint32_t,int);
extern void FUN_801274d8(uint8_t);
extern void FUN_801560f8(uint32_t);
extern int  FUN_8018cfdc(int32_t,uint32_t);
extern void FUN_801274d8_(void);
extern void FUN_801566a0(int,int);
extern void FUN_801566d4(int,int);
extern uint32_t FUN_80163154(uint32_t,int *,int);
extern void FUN_8018bf40(int,int,int,int);
extern void FUN_8018bfcc(int,int,int,int,int,int,int);
extern uint8_t *FUN_80127680(void);
extern void FUN_80127574(void);
extern void FUN_8012d090(int);
extern void FUN_8012cbc0(int,int,int);
extern void FUN_8015ab4c(void);
extern void FUN_80167a64(void);
extern void FUN_80163ab4(void);
extern void FUN_8018babc(uint8_t);
extern void FUN_80156048(uint8_t,int);
extern void FUN_801561c8(uint8_t);
extern void FUN_8019a680(void);
extern void FUN_80199f30(int);
extern void FUN_80194478(void);
extern void FUN_8018f850(int);
extern int32_t FUN_80190f74(uint32_t);
extern void FUN_8019b1d0(int);
extern void FUN_80195a80(void);
extern void FUN_80195ca8(int);
extern void FUN_801689a8(void);
extern void FUN_801267dc(void);
extern void FUN_80126f04(void);
extern void FUN_8012748c(void);
extern void FUN_8012708c(uint32_t);
extern int  FUN_80126870(int,int);
extern void FUN_80127524(void);
extern void FUN_80127574(void);
extern void FUN_8012ae3c(intptr_t,int);
extern undefined4 FUN_80159028(int,int,int);
extern void FUN_8012aeec(intptr_t);
extern undefined4 FUN_8015f644(void);
extern void FUN_80147034(void);
extern void FUN_80126848(void);
extern void FUN_80127574(void);
extern void FUN_80155f64(int);
extern void FUN_801560f8_(void);
extern void FUN_801910e4_(void);
extern int  FUN_80126870(int,int);
extern void FUN_80127524(void);
extern void FUN_8012ae3c(intptr_t,int);
extern void FUN_8012aeec(intptr_t);
extern void FUN_8012708c(uint32_t);
extern void FUN_80195290(int,int);
extern void FUN_80195290(int,int);
extern void FUN_801952b0(int);
extern void FUN_80193f60(int,int,int,int,int);
extern void FUN_80193ea0(int,int,int,int,int);
extern void FUN_80127424(void);
extern void FUN_80136ddc(void);
extern void FUN_8019117c_(void);
extern void FUN_801274c4(int);
extern void FUN_8018c034(void);
extern void FUN_8018c118(void);
extern void FUN_8018cb98(void);
extern void FUN_8018cd94(void);
extern void FUN_8012b49c(int);
extern void FUN_801525c4(int);
extern void FUN_801757b8(void);
extern uint32_t FUN_8018e3bc(uint32_t);
extern void FUN_80129f64(uint32_t);
extern void FUN_80129ff4(void);
extern void FUN_8012a030(int);
extern void FUN_8012aa6c(int,int);
extern void FUN_801a36f0(uint8_t*,uint8_t*);
extern void FUN_801a1250(void);
extern void FUN_8012755c(void);
extern int FUN_8015f590(void);
extern void FUN_80165cb4(int);
extern void FUN_80167330(int);
extern void FUN_80164668(int);
extern void FUN_8016fcc4(uint16_t*);
extern void FUN_80167840(int);
extern void FUN_80164ea4(int);
extern int  FUN_8015c754(void);
extern int  FUN_8015b9a0(void);
extern void FUN_801294cc(void*);
extern void FUN_80156ef4(int,int,int);
extern int  FUN_80146e38(int,void*);
extern void FUN_8014751c(void);
extern void FUN_8018c824(int,int,uint32_t,uint32_t);
extern void FUN_8018cc6c(int,int,int,int,int,int,int);
extern int  FUN_8012aaa8(void);
extern void FUN_8012b23c(intptr_t);
extern undefined4 FUN_8016347c(intptr_t);
extern void FUN_8012e078(int,int,const void *,int,int,int);
extern undefined4 FUN_8015f644(void);
extern undefined4 FUN_80159028(int,int,int);
extern void FUN_80156114(int);
extern void FUN_80159734(int,int,int,int,int,int*);
extern void FUN_801597ec(uint8_t*,void*,int*);
extern int  FUN_80159864(void);
extern int FUN_80159a14(int,int*);
extern void FUN_80159ce4(uint32_t);
extern void FUN_80191390(uint16_t*,int,int,int);
extern void FUN_8018f9b4(int,int);
extern void FUN_8018fa30(int,int);
extern void FUN_80159c24(int,int);
extern void FUN_801594a4(int*,int);
extern int  FUN_80195ffc(int,int,int);
extern void FUN_8018faec(int);
extern void FUN_80199070(void);
extern void FUN_80190260(int);
extern int  FUN_8019917c(uint32_t*);
extern void FUN_8019a5d8(uint32_t,int);
extern undefined4 FUN_8015f6a0(int);
extern void FUN_8018e2f0(uint32_t,int);

/* ==================================================================
 * entry_0x80126178 — _start: BSS clear + main call.
 * BSS 0x801beef0..0x801ff650 zeroed; FUN_8018e2f0 = memset;
 * then entry_0x80126234 (main); trap(1) on return.
 * ================================================================== */
void entry_0x80126178(void)
{
    undefined4 *puVar1;
    undefined4 unaff_retaddr = 0;

    puVar1 = (undefined4 *)guest_at(0x801beef0);
    do {
        *puVar1 = 0;
        puVar1 = puVar1 + 1;
    } while (puVar1 < (undefined4 *)guest_at(0x801ff650));
    uRam801beef0 = unaff_retaddr;
    FUN_8018e2f0(0x801ff654,(iRam80126228 - iRam801be720) + -0x1ff650);
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
    FUN_8015ab4c();
    FUN_80167a64();
    FUN_80163ab4();
    FUN_8018babc(*(undefined1 *)(param_1 + 9));
    FUN_80156048(*(undefined1 *)((int)param_1 + 0xe),
                 ((uint)*(byte *)((int)param_1 + 0xf) << 3) / 10);
    FUN_801561c8(*(byte *)(param_1 + 3) ^ 1);
}

/* ==================================================================
 * entry_0x80126234 — main(): boot + g_game_state machine.
 * State map (switchD_80126370 @0x8011e800, 14 entries):
 *   0 -> init sync, ->1      1 -> mode set + load, ->2
 *   2 -> ->3                 3 -> stage select, ->4
 *   4 -> FUN_80159028 demo fight load ->5 (retry on 0)
 *   5 -> per-frame: 2x FUN_8012aeec + FUN_8015f644 + FUN_80147034
 *   6 -> post-match ->3      7,8 -> options (FUN_8016fcc4) ->5
 *   9 -> FUN_80167840 menus  10,12,13 -> nop ->5
 *   11-> FUN_8015c754/5b9a0 save mgr ->5
 * ================================================================== */
void entry_0x80126234(void)
{
    bool bVar1;
    byte bVar2;
    int iVar3;
    uint uVar4;
    undefined4 uVar5;
    ushort uVar6;
    ushort auStack_30 [8];

    func_0x80126170();
    uVar6 = 0;
    FUN_80126f04();
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
    iVar3 = FUN_8015f590();
    if (iVar3 == 0) {
        apply_saved_settings_or_defaults((undefined4 *)_g_system_config_ptr);
    }
    else {
        auStack_30[0] = (ushort)*(byte *)(_g_system_config_ptr + 0x1d);
        FUN_8018babc(*(undefined1 *)(_g_system_config_ptr + 0x24));
        FUN_80156048(*(undefined1 *)(_g_system_config_ptr + 0xe),
                     ((uint)*(byte *)(_g_system_config_ptr + 0xf) << 3) / 10);
        FUN_801561c8(*(byte *)(_g_system_config_ptr + 0xc) ^ 1);
    }
    FUN_8012748c();
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
        FUN_801560f8(1);
        g_game_state = 1;
        goto switchD_80126370_caseD_80126344;
    case 1: /* 0x8012638c */
        FUN_8012708c(*(undefined1 *)(_g_system_config_ptr + 0xb));
        FUN_8012748c();
        FUN_80165cb4(0xf);
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
        FUN_801560f8(uVar5);
        FUN_8012708c(*(undefined1 *)(_g_system_config_ptr + 0xb));
        FUN_8012748c();
        FUN_80165cb4(3);
        FUN_80165cb4(1);
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
        FUN_801560f8(uVar5);
        FUN_8012748c();
        FUN_80156048(*(undefined1 *)(_g_system_config_ptr + 0xe),
                     ((((uint)*(byte *)(_g_system_config_ptr + 0xf) * 0x10a) / 100) * 8) / 10);
        iVar3 = FUN_80159028(0x80010000,0,1);
        if (iVar3 != 0) {
            g_game_state = 5;
        }
        FUN_80156048(*(undefined1 *)(_g_system_config_ptr + 0xe),
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
        FUN_801560f8(uVar5);
        FUN_80167330(0);
        if ((g_game_state != 6) || (FUN_80164668(0), g_game_state != 6))
            goto switchD_80126370_caseD_80126344;
        goto LAB_8012661c;
    case 7: /* 0x80126624 */
        uVar5 = 2;
        if (*(char *)(_g_system_config_ptr + 0x20) != '\0') {
            uVar5 = 1;
        }
        FUN_801560f8(uVar5);
        bVar2 = 7;
        goto LAB_8012666c;
    case 8: /* 0x80126654 */
        FUN_801560f8(1);
        FUN_8016fcc4(auStack_30);
        bVar2 = 8;
LAB_8012666c:
        if (g_game_state == bVar2) {
            g_game_state = 5;
        }
        goto switchD_80126370_caseD_80126344;
    case 9: /* 0x8012667c */
        FUN_801560f8(1);
        FUN_80167840(0);
        if (DAT_1f80009c != '\x04') {
            if (DAT_1f80009c == '\x05') {
                g_game_state = 0xb;
                g_sub_state = '\x04';
            }
            else {
                FUN_80164ea4(0);
                g_game_state = 5;
            }
            goto switchD_80126370_caseD_80126344;
        }
        break;
    case 11: /* 0x801266c0 */
        FUN_801560f8(1);
        if (g_sub_state == '\x03') {
            iVar3 = FUN_8015c754();
        }
        else {
            if (g_sub_state != '\x04') goto switchD_80126370_caseD_80126344;
            iVar3 = FUN_8015b9a0();
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
    FUN_801560f8(uVar5);
    FUN_8012708c(*(undefined1 *)(_g_system_config_ptr + 0xb));
    FUN_8012aeec(0x1f800000);
    FUN_8015f644();
    FUN_8012748c();
    FUN_8012aeec(0x1f800000);
    FUN_8015f644();
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
 * FUN_80126f04 — boot: subsystem inits + CD file pump.
 * ================================================================== */
void FUN_80126f04(void)
{
    int iVar1;

    FUN_8019a680();
    FUN_80199f30(0);
    FUN_80194478();
    FUN_8018f850(0);
    FUN_80190f74(0);
    FUN_80190e00(0);
    FUN_8019b1d0(1);
    FUN_80195a80();
    FUN_80195ca8(0);
    DAT_801c4580 = 0;
    FUN_801689a8();
    FUN_801267dc();
    FUN_80126848();
    FUN_80155f64(0x1f800000);
    FUN_801560f8(1);
    DAT_801c4578 = 2;
    DAT_801c457c = (uint)*(uint8_t *)g_DAT_801b12e6;
    FUN_801910e4(0);
    iVar1 = FUN_80126870(0x1f800000,0);
    FUN_80127524();
    FUN_80127574();
    FUN_8012d090(0);
    FUN_8012cbc0(2,0,0);
    apply_saved_settings_or_defaults((undefined4 *)_g_system_config_ptr);
    while (-1 < iVar1) {
        iVar1 = FUN_80126870(0x1f800000,iVar1);
        FUN_8019a460(0);
    }
    FUN_8012ae3c(0x1f800000,0);
    FUN_8012708c(*(undefined1 *)(_g_system_config_ptr + 0xb));
    FUN_8012a0f0(0x40,(_DAT_1f800022 * 0xb) / 0xc,0);
    FUN_8012748c();
    return;
}

/* FUN_8012a0f0 — not yet decompiled; extern decl below */

/* ==================================================================
 * FUN_80126870 — CD file streaming pump.
 * param_2 = pump state: 0 init -> 0x100 streaming -> 0x110 tail ->
 * 0x120 done -> -1. Each step queues one file's DMA reads.
 * ================================================================== */
int FUN_80126870(int param_1,int param_2)
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
            iVar3 = FUN_8018cfdc(0x89,_DAT_801e3cfc);
            if (iVar3 == 0) {
                return 0x110;
            }
            FUN_801566a0(_DAT_801e3cfc,0);
            FUN_801566d4(0,0);
            return 0x120;
        }
        if (param_2 != 0) {
            return param_2;
        }
        FUN_8018cfdc(0xffffffff,0);
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
    uVar2 = guest_of(FUN_80127680());
    iVar3 = FUN_8018cfdc(0x80,uVar2);
    uVar1 = DAT_1f800013;
    if (iVar3 != 0) {
        FUN_801274d8(0);
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0);
        FUN_8018bf40(_DAT_801e3cfc,0x3c0,0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),1);
        FUN_8018bf40(_DAT_801e3cfc,0x380,0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),2);
        FUN_8018bf40(_DAT_801e3cfc,0x1f0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),3);
        FUN_8018bf40(_DAT_801e3cfc,0x1e0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),4);
        FUN_8018bf40(_DAT_801e3cfc,0x1d0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),5);
        FUN_8018bf40(_DAT_801e3cfc,0x170,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),6);
        FUN_8018bf40(_DAT_801e3cfc,0x160,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x10);
        FUN_8018bf40(_DAT_801e3cfc,0x1d0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x11);
        FUN_8018bf40(_DAT_801e3cfc,0x1f0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x12);
        FUN_8018bf40(_DAT_801e3cfc,0x1e0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x13);
        FUN_8018bf40(_DAT_801e3cfc,0x1c0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x14);
        FUN_8018bf40(_DAT_801e3cfc,0x180,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x15);
        FUN_8018bf40(_DAT_801e3cfc,400,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x16);
        FUN_8018bf40(_DAT_801e3cfc,0x1a0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x17);
        FUN_8018bf40(_DAT_801e3cfc,0x1b0,0x1f0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x18);
        FUN_8018bf40(_DAT_801e3cfc,0x340,0x100,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x19);
        FUN_8018bf40(_DAT_801e3cfc,0x200,0x100,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x1a);
        FUN_8018bf40(_DAT_801e3cfc,0x1c0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),0x1b);
        FUN_8018bf40(_DAT_801e3cfc,0x1b0,0x1e0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),7);
        FUN_8018bfcc(_DAT_801e3cfc,0x160,0x1e0,1,0x340,0x1c0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),8);
        FUN_8018bf40(_DAT_801e3cfc,0x380,0x1c0,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        uVar2 = guest_of(FUN_80127680());
        iVar3 = FUN_80163154(_DAT_801e3cfc,(int *)guest_at(uVar2),9);
        FUN_8018bf40(_DAT_801e3cfc,0x3a0,0x180,1);
        _DAT_801e3cfc = _DAT_801e3cfc + iVar3;
        FUN_8019a460(0);
        FUN_8019a460(0);
        FUN_801274d8(uVar1);
        param_2 = 0x110;
    }
    return param_2;
}

/* small frame/console helpers */
void FUN_801267dc(void)
{
    FUN_801a36f0(g_DAT_801befa8,g_DAT_801befca);
    FUN_801a1250();
    FUN_8018babc(0);
}

void FUN_80126848(void)
{
    undefined4 *puVar1;

    puVar1 = (undefined4 *)g_DAT_1f800000;
    do {
        *puVar1 = 0;
        puVar1 = puVar1 + 1;
    } while (puVar1 < (undefined4 *)guest_at(0x1f800400));
}

/* FUN_8012708c — display mode + screen window setup.
 * param_1 = video mode (0-7 from system config +0xb):
 *   0,2,4,6 -> 0xf0-height variant (interlaced?), odd -> 0x1e0.
 * Sets _DAT_1f800020/22/24 screen geometry, OAM window regs via
 * FUN_80195290/2b0, sprite blocks at 0x801e40a0/0x801e61f0/0x801e40fc/
 * 0x801e624c, then FUN_80127424 mode commit + FUN_80136ddc. */
void FUN_8012708c(uint32_t param_1)
{
    int iVar1;
    undefined4 uVar2;

    FUN_8019a460(2);
    FUN_801274d8(0);
    FUN_80190e00(1);
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
    FUN_801952b0((int)_DAT_1f800024);
    *(uint8_t *)g_DAT_801e625e = 1;
    *(uint8_t *)g_DAT_801e410e = 1;
    if (DAT_1f800015 == '\0') {
        FUN_80193ea0(0x801e40a0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        FUN_80193ea0(0x801e61f0,0,0xf0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        FUN_80193f60(0x801e40fc,0,0xf0,(int)_DAT_1f800020,0);
        uVar2 = 0;
        iVar1 = 0;
    }
    else {
        FUN_80193ea0(0x801e40a0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        FUN_80193ea0(0x801e61f0,0,0,(int)_DAT_1f800020,(int)(short)_DAT_1f800022);
        FUN_80193f60(0x801e40fc,0,2,(int)_DAT_1f800020,(short)_DAT_1f800022 + -2);
        uVar2 = 2;
        iVar1 = (short)_DAT_1f800022 + -2;
    }
    FUN_80193f60(0x801e624c,0,uVar2,(int)_DAT_1f800020,iVar1);
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
    FUN_80127424();
    FUN_80136ddc();
    FUN_8019a460(0);
    return;
}

void FUN_8012748c(void)
{
    FUN_8019117c(0);
    FUN_8019a460(0);
    FUN_801274c4(1);
    FUN_801274d8(1);
}

void FUN_801274d8(uint8_t param_1)
{
    DAT_1f800013 = param_1;
}

void FUN_80127524(void)
{
    FUN_8018c034();
    FUN_8018c118();
    FUN_8018cb98();
    FUN_8018cd94();
}

void FUN_80127574(void)
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
    FUN_8012b49c(0);
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
    FUN_8012755c();
    DAT_1f800011 = 0;
    _DAT_1f8000b2 = 0;
    DAT_1f8000b1 = 0;
    DAT_1f8000b0 = 0;
    FUN_801525c4(0x1f800000);
    _g_system_config_ptr = g_DAT_801c1200;
    _DAT_1f8000bc = 0x801f0270;
    _DAT_1f8000b8 = 0;
    _DAT_1f8000cc = 0x801c15f8;
    FUN_801757b8();
    {
        uint32_t uVar1 = FUN_8018e3bc(0xf2000002);
        FUN_80129f64(uVar1);
    }
    FUN_80129ff4();
    FUN_8012a030(0);
    FUN_8012aa6c(0,0);
    DAT_801c43a0 = 0;
    DAT_801c11d8 = 0;
    DAT_801c15f4 = 0;
}

uint8_t *FUN_80127680(void)
{
    return g_DAT_801c47c0;
}

void FUN_801279c8(void)
{
    return;
}

void FUN_801279d0(uint32_t param_1,int param_2)
{
    if ((param_2 != 0) && (param_2 == 1)) {
        DAT_801c15f0 = 1;
    }
}

void FUN_801279f4(int param_1,int param_2)
{
    short sVar1;
    int iVar2;
    int iVar3;

    iVar3 = (uint)*(byte *)(param_1 + 3) * 0x24;
    switch(DAT_1f80009c) {
    default:
        if (param_2 == 0) {
            sVar1 = *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb2));
            *(short *)((char *)g_DAT_801c144c + iVar3) = *(short *)((char *)g_DAT_801c144c + iVar3) + 1;
            *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb2)) = sVar1 + 1;
            iVar2 = (uint)*(byte *)guest_at(*(uint32_t *)(param_1 + 0x2c) + 3) * 2 + iVar3;
            *(short *)((char *)g_DAT_801c1444 + iVar2 + 0xe) = *(short *)((char *)g_DAT_801c1444 + iVar2 + 0xe) + 1;
        }
        else {
            sVar1 = *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb6));
            *(short *)((char *)g_DAT_801c1448 + iVar3) = *(short *)((char *)g_DAT_801c1448 + iVar3) + 1;
            *(short *)guest_at((uint32_t)(iVar3 + -0x7fe3ebb6)) = sVar1 + 1;
        }
        *(short *)((char *)g_DAT_801c1446 + iVar3) = *(short *)((char *)g_DAT_801c1446 + iVar3) + 1;
        if (*(ushort *)((char *)g_DAT_801c1450 + iVar3) < *(ushort *)(param_1 + 0xe2)) {
            *(ushort *)((char *)g_DAT_801c1450 + iVar3) = *(ushort *)(param_1 + 0xe2);
        }
        break;
    case 2:
        break;
    }
    return;
}

/* FUN_80127424 — mode set: OAM list rebuild for both players */
void FUN_80127424(void)
{
    FUN_8019117c(0);
    FUN_8019a460(0);
    FUN_801910e4(0);
    FUN_801919ac(0x801e40fc);
    FUN_801917e0(0x801e40a0);
    FUN_8019a460(0);
    FUN_801919ac(0x801e624c);
    FUN_801917e0(0x801e61f0);
}

/* FUN_8012ae3c — battle session arm (fighter slot buffers) */
void FUN_8012ae3c(intptr_t param_1,int param_2)
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
        FUN_8012755c();
        if (param_2 == 0) {
            FUN_80191668(0x801e4110,0x820);
            iVar1 = -0x7fe19da0;
        }
        FUN_80191668((uint32_t)iVar1,0x820);
    }
}

/* FUN_8016347c — match-end check: pad-start prompt timer.
 * Returns -1 normally; 0xffffff9d (-99) while prompting exit. */
undefined4 FUN_8016347c(intptr_t param_1)
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
        FUN_8012e078(*(undefined4 *)(param_1 + 8),*(undefined4 *)(param_1 + 0xc),
                     g_DAT_80121e3c,
                     ((((iVar2 >> 0x10) - (iVar2 >> 0x1f)) >> 1) + -0x45) * 0x10000 >> 0x10,
                     (iVar3 << 0xe) >> 0x10,4);
        uVar1 = 0xffffff9d;
    }
    return uVar1;
}

/* FUN_8015f644 — per-frame memcard/save poll */
undefined4 FUN_8015f644(void)
{
    undefined4 uVar1;

    if (*(char *)(_g_system_config_ptr + 0x22) == '\0') {
        uVar1 = 0xffffffff;
    }
    else {
        uVar1 = 0xffffffff;
        if (DAT_801c15f0 != 0) {
            DAT_801c15f0 = 0;
            uVar1 = FUN_8015f6a0(1);
        }
    }
    return uVar1;
}

/* ==================================================================
 * FUN_8012aeec — frame driver (called 2x/frame: fighter slot toggle).
 * param_1 = battle object guest addr (0x1f800000).
 * +0xc fighter index XORs 0/1 each call -> double-buffered submit.
 * ================================================================== */
void FUN_8012aeec(intptr_t param_1)
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
    FUN_80157a00();
    FUN_80156d98();
    FUN_8012a884((uint8_t *)param_1);
    FUN_80156ff0();
    iVar4 = FUN_8019a460(1);
    if (iVar4 < 0xf1) {
        FUN_8018bd50(iVar9 + 0x207c,0x81f);
    }
    FUN_8012a72c(param_1);
    if ((*(char *)(param_1 + 0x15) == '\0') && ((*(ushort *)(param_1 + 0x82) & 1) != 0)) {
        uVar3 = FUN_8019a460(1);
        *(undefined2 *)(param_1 + 0x1a) = uVar3;
        FUN_8019117c(0);
        uVar3 = FUN_8019a460(1);
        *(undefined2 *)(param_1 + 0x1c) = uVar3;
        FUN_8019a460(*(undefined1 *)(param_1 + 0x10));
    }
    else {
        uVar3 = FUN_8019a460(1);
        *(undefined2 *)(param_1 + 0x1c) = uVar3;
        *(undefined2 *)(param_1 + 0x1a) = uVar3;
        FUN_8019a460(*(undefined1 *)(param_1 + 0x10));
        FUN_80190e00(1);
    }
    _DAT_1f8000e0 = _DAT_1f8000e4;
    _DAT_1f8000e4 = 0;
    _DAT_801e8500 = FUN_80191ed8();
    FUN_801919ac(iVar8 + 0x5c);
    FUN_801917e0(iVar8);
    if (*(byte *)(param_1 + 0x12) != 0) {
        FUN_801910e4(*(byte *)(param_1 + 0x12) - 1);
        *(undefined1 *)(param_1 + 0x12) = 0;
    }
    iVar4 = (int)*(short *)(param_1 + 0xd0);
    while (0 < iVar4) {
        FUN_80191428(iVar4 * 8 + -0x7fe3bad0,((uint32_t *)g_DAT_801bef38)[iVar4 + -1]);
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
            *(uint *)(iVar6 + 0x20d8) = *(uint *)(iVar9 + 0x207c) | 0x5000000;
            iVar6 = iVar6 + -0x18;
            *(uint *)(iVar9 + 0x207c) = uVar5 & 0xffffff;
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
            FUN_80191300(guest_of((char *)g_DAT_801f0668 + iVar4),
                         (uint32_t)(uintptr_t)*(uint32_t *)((char *)g_DAT_801f0664 + iVar4),uVar1,uVar2);
        }
        FUN_80191770(iVar9 + 0x207c);
    }
    FUN_80129f78();
    uVar5 = *(uint *)(param_1 + 0xc) ^ 1;
    iVar4 = uVar5 * 0x2150;
    _DAT_1f8000dc = _DAT_1f8000dc + 1;
    *(uint *)(param_1 + 0xc) = uVar5;
    *(int *)(param_1 + 4) = iVar4 + -0x7fe1bf60;
    *(int *)(param_1 + 8) = iVar4 + -0x7fe1bef0;
    FUN_80191668(iVar4 + -0x7fe1bef0,0x820);
    *(uint *)guest_at((uint32_t)(iVar4 + -0x7fe1bef0)) = *(uint *)guest_at((uint32_t)(iVar4 + -0x7fe1bef0)) | 0xffffff;
    FUN_8016362c(param_1);
    if (*(char *)guest_at(*(uint32_t *)(param_1 + 200) + 2) < '\0') {
        FUN_8012a088();
        FUN_8012a30c();
    }
    FUN_8012cd9c(0,0);
    return;
}

/* ==================================================================
 * FUN_80147034 — match loop (infinite until DAT_1f800001 == 0xff).
 * Stage machine: 0 boot select -> 1 running -> 2 ending ->
 * 0xf0/0xf1 shutdown. 300-frame attract timeout via unaff_s2.
 * ================================================================== */
void FUN_80147034(void)
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
            FUN_8012cd9c(0,0);
            if ((_DAT_1f80007c & 0x10) == 0) {
                FUN_801294cc(&local_88);
            }
            else {
                FUN_80156ef4(0x400,0xd,0);
                DAT_1f800001 = 1;
            }
LAB_801472f8:
            FUN_8018c824(_DAT_1f800008 + 4,0,0xefff0088,0x7c000000);
            FUN_8018c824(_DAT_1f800008 + 4,0x100,0xefff008a,0x7c400000);
            FUN_8018c824(_DAT_1f800008 + 4,0xf00000,0xefff0099,0x7c800000);
            FUN_8018c824(_DAT_1f800008 + 4,0xf00100,0xefff009b,0x7cc00000);
            FUN_8018c824(_DAT_1f800008,0x1880074,0xf97000f,0x7b1fb810);
            FUN_8018c824(_DAT_1f800008,0x189010c,0xf7f000f,0x7b1f9038);
            FUN_8018c824(_DAT_1f800008,0x19800a4,0xfb7000f,0x7b1f9000);
            FUN_8018c824(_DAT_1f800008,0x1a800a5,0xf39000f,0x7b1fa840);
            FUN_8018c824(_DAT_1f800008,0x1a800e0,0xf17000f,0x7b1f90e4);
            FUN_8018c824(_DAT_1f800008,0x1a800fc,0xf5d000f,0x7b1fa87a);
            FUN_8018c824(_DAT_1f800008,0x11201e4,0x70f000f,0x7b1fb8e0);
        }
        else if (DAT_1f800001 < 3) {
            if (DAT_1f800001 == 0) {
                iVar1 = FUN_8012aaa8();
                if (iVar1 == 0) {
                    FUN_8018cc6c(_DAT_1f800008,0,0,(int)_DAT_1f800020,(int)_DAT_1f800022,0,0);
                    iVar1 = FUN_80146e38(0x1f800000,&local_40);
                    if (iVar1 != 0) {
                        FUN_80156ef4(0x204,0,0);
                        FUN_8012cbc0(2,0,0);
                        FUN_80156ef4(0x400,0x82,0);
                        local_7c = (void (*)(void))FUN_8014751c;
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
                        FUN_801274d8(0);
                        FUN_8018bfcc(local_40,0,0x1f0,1,0x200,0,1);
                        FUN_8018bfcc(local_3c,0,0x1f1,1,0x280,0,1);
                        FUN_8018bfcc(local_38,0,0x1f2,1,0x240,0x100,1);
                        FUN_8018bfcc(local_34,0,499,1,0x2c0,0x100,1);
                        FUN_8019a460(0);
                        FUN_801274d8(1);
                        _DAT_1f80008c = 0;
                        FUN_8012ae3c(0x1f800000,0xffffffff);
                        unaff_s2 = 300;
                        DAT_1f800001 = DAT_1f800001 + 1;
                        goto LAB_801472a4;
                    }
                }
            }
            else if (DAT_1f800001 == 1) {
LAB_801472a4:
                iVar1 = FUN_8016347c(0x1f800000);
                if ((iVar1 < 0) && (unaff_s2 = unaff_s2 + -1, unaff_s2 == 0)) {
                    DAT_1f800001 = 0xf0;
                }
                _DAT_1f80008c = _DAT_1f80008c + 1;
                if ((_DAT_1f80007c & 0x840) != 0) {
                    FUN_80156ef4(0x400,0xc,0);
                    DAT_1f800001 = 2;
                }
                goto LAB_801472f8;
            }
        }
        else if (DAT_1f800001 == 0xf0) {
            FUN_80156ef4(0x204,0,0);
            FUN_8012b23c(0x1f800000);
            FUN_8012aa6c(1,8);
            DAT_1f800001 = DAT_1f800001 + 1;
        }
        else if ((DAT_1f800001 == 0xf1) && (iVar1 = FUN_8012aaa8(), iVar1 == 0)) {
            DAT_1f800001 = 0xff;
            goto joined_r0x801470b0;
        }
        FUN_8012aeec(0x1f800000);
    } while( true );
}

/* ==================================================================
 * FUN_80159028 — scene/stage entry.
 * param_1 = VRAM arena base, param_2 = stage/file-table index,
 * param_3 = demo flag. Loads stage file, sets VRAM windows,
 * streams data, uploads palette/OAM, arms battle, runs 2 frames.
 * Returns 1 on success (state->5), 0 to retry.
 * ================================================================== */
undefined4 FUN_80159028(int param_1,int param_2,int param_3)
{
    int iVar1;
    int iVar2;
    undefined4 uVar3;
    int iVar4;
    int *piVar5;
    undefined4 *puVar6;
    undefined4 uVar7;
    undefined1 *local_48 = (undefined1 *)guest_at(0x1f801340);
    undefined2 local_40;
    undefined2 local_3e;
    undefined2 local_3c;
    undefined2 local_3a;
    undefined4 *local_38 = (undefined4 *)guest_at(0x1f801300);

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
    FUN_8012aeec(0x1f800000);
    FUN_80156114(0x7f);
    FUN_801274d8(0);
    FUN_8019a460(0);
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
    FUN_80159734(0x801f4700,iVar2,*(undefined4 *)((char *)g_DAT_801b3ed8 + 0x10 + iVar1),iVar4,
                 *(int *)((char *)g_DAT_801b3ed8 + 0x10 + iVar1) + *(int *)((char *)g_DAT_801b3ed8 + 0x18 + iVar1),piVar5);
    FUN_801597ec((uint8_t *)puVar6,(void *)(uintptr_t)0x80159864,piVar5);
    FUN_80190260(_DAT_801f4758);
    while (iVar2 = (int)FUN_80159a14(0x801f4700,piVar5), iVar2 == -1) {
        local_38[0] = *puVar6;
        FUN_80159ce4(guest_of(local_38));
    }
    _DAT_801f4740 = 0;
    FUN_801910e4(0);
    local_40 = 0;
    local_3e = 0;
    if (*piVar5 == 0) {
        local_3c = *(undefined2 *)((char *)g_DAT_801b3ed8 + 0x14 + iVar1);
    }
    else {
        local_3c = (undefined2)
                   ((uint)(*(int *)((char *)g_DAT_801b3ed8 + 0x14 + iVar1) * 3 -
                          (*(int *)((char *)g_DAT_801b3ed8 + 0x14 + iVar1) * 3 >> 0x1f)) >> 1);
    }
    local_3a = (undefined2)(*(int *)((char *)g_DAT_801b3ed8 + 0x18 + iVar1) << 1);
    FUN_80191390((uint16_t *)&local_40,0,0,0);
    do {
        uVar3 = 2;
        if (*piVar5 != 0) {
            uVar3 = 3;
        }
        FUN_8018f9b4(_DAT_801f4708 * 4 + -0x7fe0b900,uVar3);
        iVar2 = _DAT_801f4732 + -1;
        if (iVar2 < 0) {
            iVar2 = _DAT_801f4732 + 0xe;
        }
        FUN_8018fa30(_DAT_801f4714 * 4 + -0x7fe0b8f4,
                     _DAT_801f4730 * 0x10 * ((iVar2 >> 4) + 1) >> 1);
        while (iVar2 = (int)FUN_80159a14(0x801f4700,piVar5), iVar2 == -1) {
            iVar2 = FUN_8019917c(local_38);
            if ((*(int *)((char *)g_DAT_801b3ed8 + 0x8 + iVar1) < iVar2) || (iVar2 < 1)) {
                local_38[0] = *puVar6;
            }
            FUN_80159ce4(guest_of(local_38));
        }
        FUN_80159c24(0x801f4700,0);
        FUN_801594a4(piVar5,0xffffffff);
        if (_DAT_801f4740 == 1) goto LAB_801593a8;
    } while ((param_3 == 0) || ((_DAT_1f80007c & 0x850) == 0));
    uVar7 = 1;
LAB_801593a8:
    FUN_80156114(0);
    local_48[0] = 0x80;
    _DAT_1f80000c = (uint)(_DAT_1f80000c == 0);
    _DAT_1f800004 = _DAT_1f80000c * 0x2150 + -0x7fe1bf60;
    _DAT_1f800008 = _DAT_1f80000c * 0x2150 + -0x7fe1bef0;
    FUN_80195ffc(0xe,(int)guest_of(local_48),0);
    DAT_801c4580 = 1;
    FUN_8018faec(0);
    FUN_80199070();
    FUN_80195ffc(9,0,0);
    FUN_8019a460(0);
    FUN_8012708c(*(undefined1 *)(_g_system_config_ptr + 0xb));
    FUN_8012ae3c(0x1f800000,1);
    FUN_8012aeec(0x1f800000);
    FUN_8012748c();
    FUN_8012aeec(0x1f800000);
    return uVar7;
}

/* ======== 0x8018cfdc ======== */

int FUN_8018cfdc(int param_1,uint32_t param_2)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined1 local_20 [8];
  uint32_t *filetab;
  
  filetab = (uint32_t *)g_PTR_DAT_801b5b94;
  piVar2 = (int *)guest_at(filetab[param_1 * 2]);
  uVar3 = filetab[param_1 * 2 + 1];
  if (param_1 < 0) {
    FUN_80157368();
    FUN_80198af0();
    local_20[0] = 0x80;
    FUN_80195d8c(0xe,local_20,0);
    if (DAT_801c4580 == '\0') {
      _DAT_801f68d8 = 3;
    }
    else {
      _DAT_801f68d8 = 0;
    }
    DAT_801b55ec = 0;
    iVar1 = _DAT_801f68d8;
  }
  else {
    if (*piVar2 == 0) {
      return 0xffffffff;
    }
    iVar1 = _DAT_801f68d8 + -1;
    if (_DAT_801f68d8 == 0) {
      DAT_801c4580 = '\x01';
      if (DAT_801b55ec != 0) {
        if (DAT_801b55ec == 2) {
          DAT_801c4580 = 1;
          return 1;
        }
        iVar1 = FUN_80198e2c(1,0);
        if (0 < iVar1) {
          return 0;
        }
        if (iVar1 < 0) {
          FUN_8018d150(piVar2,param_2,uVar3);
          return 0;
        }
        DAT_801b55ec = 0;
        return 1;
      }
      FUN_8018d150(piVar2,param_2,uVar3);
      DAT_801b55ec = 1;
      iVar1 = _DAT_801f68d8;
    }
  }
  _DAT_801f68d8 = iVar1;
  return 0;
}

/* ======== 0x8012b23c ======== */

void FUN_8012b23c(intptr_t param_1)

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
  FUN_80157a00();
  FUN_80156d98();
  FUN_8012a884((uint8_t *)param_1);
  FUN_80156ff0();
  FUN_8012a72c(param_1);
  if ((*(char *)(param_1 + 0x15) == '\0') && ((*(ushort *)(param_1 + 0x82) & 1) != 0)) {
    uVar3 = FUN_8019a460(1);
    *(undefined2 *)(param_1 + 0x1a) = uVar3;
    FUN_8019117c(0);
    uVar3 = FUN_8019a460(1);
    *(undefined2 *)(param_1 + 0x1c) = uVar3;
    FUN_8019a460(*(undefined1 *)(param_1 + 0x10));
  }
  else {
    uVar3 = FUN_8019a460(1);
    *(undefined2 *)(param_1 + 0x1c) = uVar3;
    *(undefined2 *)(param_1 + 0x1a) = uVar3;
    FUN_8019a460(*(undefined1 *)(param_1 + 0x10));
    FUN_80190e00(1);
  }
  DAT_1f8000e0 = DAT_1f8000e4;
  DAT_1f8000e4 = 0;
  _DAT_801e8500 = FUN_80191ed8();
  FUN_801919ac(iVar7 + 0x5c);
  FUN_801917e0(iVar7);
  if (*(byte *)(param_1 + 0x12) != 0) {
    FUN_801910e4(*(byte *)(param_1 + 0x12) - 1);
    *(undefined1 *)(param_1 + 0x12) = 0;
  }
  iVar4 = (int)*(short *)(param_1 + 0xd0);
  while (0 < iVar4) {
    FUN_80191428((int)(iVar4 * 8 + -0x7fe3bad0),*(undefined4 *)((char *)DAT_801bef38 + (iVar4 + -1) * 4));
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
      FUN_80191300(guest_of((char *)g_DAT_801f0668 + iVar4),((uint32_t *)g_DAT_801f0664)[iVar4 / 4],uVar1,uVar2);
    }
    uVar6 = 0x801e618c;
    if (iVar7 == -0x7fe1bf60) {
      uVar6 = 0x801e82dc;
    }
    FUN_80191770(uVar6);
  }
  return;
}

/* FUN_80136fb0 — task/effect-table init: clears _DAT_801f0738 +
 * DAT_801bef20, runs FUN_801a6ae8 then registers 7 named tables via
 * FUN_801ad59c/FUN_801ad1e4. */
extern void *guest_at(uint32_t addr);
extern void FUN_801a6ae8(void);
extern void FUN_801ad59c(uint32_t,int32_t);
extern void FUN_801ad1e4(uint32_t,int32_t);
void FUN_80136fb0(int32_t param_1)
{
  (void)param_1;
  *(uint32_t *)guest_at(0x801f0738) = 0;
  *(uint8_t  *)guest_at(0x801bef20) = 0;
  FUN_801a6ae8();
  FUN_801ad59c(0x801c43a8,0);
  FUN_801ad1e4(0x801c44e8,0);
  FUN_801ad59c(0x801c4448,1);
  FUN_801ad59c(0x801c4498,2);
  FUN_801ad59c(0x801c4420,3);
  FUN_801ad1e4(0x801c4470,1);
  FUN_801ad1e4(0x801c44c0,2);
  FUN_801ad1e4(0x801c4434,3);
}

/* FUN_8012c0b4 — intro/movie playback state machine. State byte
 * _DAT_1f80008c: -1 idle; 0 init; 0x10..0x30 staged file loads via
 * FUN_8018cfdc(file_id, heap=_DAT_801e3cfc); FUN_80163154 advances
 * frames; param_1 selects which queued load to start (0/1/2). */
int32_t FUN_8012c0b4(int32_t param_1)
{
  uint8_t uVar1;
  int32_t iVar2, iVar3;
  int16_t sVar4;
  sVar4 = *(int16_t *)g_DAT_1f80008c;
  if (sVar4 < 0) return -1;
  if (sVar4 != 0x10) {
    if (0x10 < sVar4) {
      if (sVar4 == 0x20) {
        iVar2 = FUN_8018cfdc(0x8b,(uint32_t)_DAT_801e3cfc);
        if (iVar2 == 0) { *(int16_t *)g_DAT_1f80008c = sVar4; return 0; }
        sVar4 = 0x30;
        FUN_801566a0(_DAT_801e3cfc,1);
        FUN_801566d4(1,1);
        FUN_8019a460(0);
      } else {
        sVar4 = *(int16_t *)g_DAT_1f80008c;
        if (sVar4 != 0x30) return 0;
      }
      iVar2 = FUN_8018cfdc(0x16,(uint32_t)_DAT_801e3cfc);
      uVar1 = DAT_1f800013;
      if (iVar2 == 0) { *(int16_t *)g_DAT_1f80008c = sVar4; return 0; }
      FUN_801274d8(0);
      FUN_8018bf40(_DAT_801e3cfc,0,0x1e0,1);
      goto LAB_8012c330;
    }
    if (sVar4 != 0) return 0;
    FUN_8018cfdc(-1,0);
    sVar4 = 0x10;
    _DAT_801c43a0 = 0;
    DAT_801e3d00 = 0xff;
    _DAT_801c47b8 = (uint32_t)-0x7fff0000;
    _DAT_801e3cfc = 0x800d0000;
  }
  if (param_1 == 1) {
    iVar2 = FUN_8018cfdc(0x14,(uint32_t)_DAT_801e3cfc);
    if (iVar2 != 0) {
      iVar2 = 0;
      do {
        iVar3 = FUN_80163154((uint32_t)_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),iVar2);
        iVar2++;
        _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)iVar3;
      } while (iVar2 < 0xb);
      sVar4 = 0x20;
    }
  } else if (param_1 < 2) {
    if (param_1 == 0 &&
        (iVar2 = FUN_8018cfdc(0x14,(uint32_t)_DAT_801e3cfc),
         uVar1 = DAT_1f800013, iVar2 != 0)) {
      FUN_801274d8(0);
      uint8_t *ctr = (uint8_t *)guest_at(0x801b1408);
      *ctr = *ctr + 1;
      if (10 < *ctr) *ctr = 0;
      iVar2 = FUN_80163154(_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),*ctr);
      FUN_8018bf40((int)_DAT_801c47b8,0x3c0,0x100,1);
      _DAT_801c47b8 = _DAT_801c47b8 + (uint32_t)iVar2;
LAB_8012c330:
      FUN_8019a460(0);
      FUN_801274d8(uVar1);
      *(int16_t *)g_DAT_1f80008c = -1;
      return 1;
    }
  } else if (param_1 == 2 &&
             (iVar2 = FUN_8018cfdc(0x15,(uint32_t)_DAT_801e3cfc), iVar2 != 0)) {
    iVar2 = 0;
    do {
      iVar3 = FUN_80163154((uint32_t)_DAT_801c47b8,(int *)guest_at(_DAT_801e3cfc),iVar2);
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
 * FUN_8012d154/FUN_8012dce4 with the block's host pointer. */

#define BR2B8(a)   (*(uint8_t  *)guest_at(a))
#define BR2B16(a)  (*(uint16_t *)guest_at(a))
#define BR2B32(a)  (*(uint32_t *)guest_at(a))
#define BR2B16S(a) (*(int16_t  *)guest_at(a))

extern int      FUN_8018ef60(char *,char *,...);
extern int32_t   FUN_80190a04(int32_t,uint32_t,uint32_t,uint32_t,...);
extern uint32_t guest_of(const void *);
extern undefined4 FUN_8012d154(uint *,uint,int);
extern undefined4 FUN_8012dce4(uint *,uint,int);
extern void     FUN_801941e0(int,int,int,uint,byte *);
extern void    *FUN_8018c18c(void);

/* Varargs forwarders: the guest dispatchers pass fmt + 8 raw 32-bit
 * varargs straight through; C cannot forward ..., so collect and
 * re-emit with fixed arity (same convention as FUN_8018ef60's
 * va_arg(ap,uint32_t)). */
static void br2_text9_dbg(const char *fmt,const uint32_t *v)
{
  /* FUN_80190a04 takes guest u32 args; fmt is a host pointer into
   * arena/scratch, so recover its guest address. */
  FUN_80190a04((int32_t)guest_of(fmt),(int)v[0],(int)v[1],(int)v[2],
               (int)v[3],(int)v[4],(int)v[5],(int)v[6],(int)v[7]);
}
static int br2_text9_spr(uint32_t buf,const char *fmt,const uint32_t *v)
{
  /* buf is a guest address; FUN_8018ef60 takes a host buffer pointer. */
  return FUN_8018ef60((char *)guest_at(buf),(char *)fmt,(int)v[0],(int)v[1],(int)v[2],
                      (int)v[3],(int)v[4],(int)v[5],(int)v[6],(int)v[7]);
}

/* FUN_8012cc6c — localized print dispatcher on mode @0x1f800384:
 * 1 = debug printf, 2 = sprintf into stack buf + queue text draw via
 * FUN_8012dce4 on the 0x1f80038c block, else nothing. */
void FUN_8012cc6c(const char *param_1,...)
{
  __builtin_va_list ap;
  uint32_t v[8];
  uint8_t *buf = (uint8_t *)guest_at(0x1f802000);
  uint16_t *b;
  uint32_t mode = BR2B32(0x1f800384);
  int i;
  __builtin_va_start(ap,param_1);
  for (i = 0; i < 8; i++) v[i] = (uint32_t)__builtin_va_arg(ap,uint32_t);
  __builtin_va_end(ap);
  if (mode == 1) { br2_text9_dbg(param_1,v); return; }
  if ((int32_t)mode < 2) return;
  if (mode != 2) return;
  br2_text9_spr(guest_of(buf),param_1,v);
  b = (uint16_t *)guest_at(0x1f80038c);
  b[3] = 0x1000; b[4] = 0x1000;
  b[5] = 8; b[6] = 8; b[7] = 8; b[8] = 8;
  b[0] = 0; b[1] = 0; b[2] = 0; b[9] = 0; b[10] = 0;
  b[11] = 0x78df; b[13] = 0;
  *(uint32_t *)(b + 14) = guest_of(buf);   /* +0x1c */
  FUN_8012dce4((uint *)guest_at(BR2B32(0x1f800008)),
               BR2B16(0x1f80000c),(int)guest_of(b));
}

/* FUN_8012cf64 — same dispatcher but self-describing: mode 2 copies
 * caller +0x10/+0x12 u16 into block +2/+4 before the draw. */
void FUN_8012cf64(void *param_1,const char *param_2,...)
{
  __builtin_va_list ap;
  uint32_t v[8];
  uint8_t *buf = (uint8_t *)guest_at(0x1f802100);
  uint16_t *b;
  uint8_t *self = (uint8_t *)param_1;
  uint32_t mode = BR2B32(0x1f800384);
  int i;
  __builtin_va_start(ap,param_2);
  for (i = 0; i < 8; i++) v[i] = (uint32_t)__builtin_va_arg(ap,uint32_t);
  __builtin_va_end(ap);
  if (mode == 1) { br2_text9_dbg(param_2,v); return; }
  if ((int32_t)mode < 2) return;
  if (mode != 2) return;
  br2_text9_spr(guest_of(buf),param_2,v);
  b = (uint16_t *)guest_at(0x1f80038c);
  b[0] = 0;
  b[1] = *(uint16_t *)(self + 0x10);
  b[2] = *(uint16_t *)(self + 0x12);
  b[3] = 0x1000; b[4] = 0x1000;
  b[5] = 8; b[6] = 8; b[7] = 8; b[8] = 8;
  b[11] = 0x78df; b[9] = 0; b[10] = 0;
  *(uint32_t *)(b + 14) = guest_of(buf);   /* +0x1c */
  b[12] = 0;
  FUN_8012dce4((uint *)guest_at(BR2B32(0x1f800008)),
               BR2B16(0x1f80000c),(int)guest_of(b));
}

/* FUN_80129910 / FUN_80129944 — menu cursor row painters: emit "  > "
 * or "    " through FUN_8012cf64 when stepper byte+4 is clear. */
void FUN_80129910(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  if (p[4] == 0)
    FUN_8012cf64(param_1,(const char *)guest_at(0x8011ec20));
}
void FUN_80129944(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  if (p[4] == 0)
    FUN_8012cf64(param_1,(const char *)guest_at(0x8011ec28));
}

/* FUN_80129660 — menu stepper redraw: cb(+0xc) with 0x100, then for
 * i = 1..h(+6) paint row i (FUN_80129910 when i == sel(+8) else
 * FUN_80129944) and cb with 0x100+i, finally cb with 0x1ff. */
void FUN_80129660(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  void (*cb)(void *,uint32_t);
  uint32_t i;
  __builtin_memcpy(&cb,p + 0xc,sizeof cb);
  cb(param_1,0x100);
  i = 1;
  for (;;) {
    if (i == *(uint16_t *)(p + 8)) FUN_80129910(param_1);
    else                            FUN_80129944(param_1);
    cb(param_1,(i + 0x100) & 0xffff);
    i++;
    if (*(uint16_t *)(p + 6) < i) break;
  }
  cb(param_1,0x1ff);
}

/* FUN_801297dc — menu stepper input: pad flags @0x1f80007e; byte+5
 * selects button pair (0: 0x4000/0x1000 sfx 0xa, else 0x2000/0x8000
 * sfx 0xb). Up wraps cur(+8) to 1 past max(+6); down wraps 1 -> max. */
void FUN_801297dc(void *param_1)
{
  uint8_t *p = (uint8_t *)param_1;
  uint16_t cur,max;
  if (p[5] == 0) {
    if (BR2B16(0x1f80007e) & 0x4000) {
      FUN_80156ef4(0x400,0xa,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur < max) ? (uint16_t)(cur + 1) : (uint16_t)1;
    }
    if (BR2B16(0x1f80007e) & 0x1000) {
      FUN_80156ef4(0x400,0xa,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur == 1) ? max : (uint16_t)(cur - 1);
    }
  } else {
    if (BR2B16(0x1f80007e) & 0x2000) {
      FUN_80156ef4(0x400,0xb,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur < max) ? (uint16_t)(cur + 1) : (uint16_t)1;
    }
    if (BR2B16(0x1f80007e) & 0x8000) {
      FUN_80156ef4(0x400,0xb,0);
      cur = *(uint16_t *)(p + 8); max = *(uint16_t *)(p + 6);
      *(uint16_t *)(p + 8) = (cur == 1) ? max : (uint16_t)(cur - 1);
    }
  }
}

/* FUN_8012e184 — HUD text variant A (block +0x16 attr +0x1f). */
void FUN_8012e184(undefined4 param_1,undefined2 param_2,const void *param_3,
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
  FUN_8012d154((uint *)guest_at(param_1),(uint)param_2 & 0xffff,
               (int)guest_of(b));
}

/* FUN_8012e61c — HUD text variant B (uniform 0xf metrics, +0x16). */
void FUN_8012e61c(undefined4 param_1,undefined4 param_2,const void *param_3,
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
  FUN_8012d154((uint *)guest_at(param_1),(uint)param_2 & 0xffff,
               (int)guest_of(b));
}

/* FUN_8012e73c — HUD text variant C (0x1e line gap, +0x16). */
void FUN_8012e73c(undefined4 param_1,undefined4 param_2,const void *param_3,
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
  FUN_8012d154((uint *)guest_at(param_1),(uint)param_2 & 0xffff,
               (int)guest_of(b));
}

/* FUN_8012e7cc — HUD text variant D (metric 8, flag 5, +0x16). */
void FUN_8012e7cc(uint32_t param_1,uint32_t param_2,const void *param_3,
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
  FUN_8012d154((uint *)guest_at(param_1),param_2 & 0xffff,
               (int)guest_of(b));
}

/* FUN_8018cdf8 — sprite-node pool alloc: 8 slots of 0x34 at
 * 0x801f5d38, generation counter @0x1f8000dc, cursor @0x801b55e8.
 * Returns slot+4 host pointer or NULL when generation matches. */
void *FUN_8018cdf8(void)
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

/* FUN_8018ce70 — sprite-list node emit (pool FUN_8018cdf8, tag
 * 0x08000000, packet tail +0x24). 10 args; param_10 selects GPU
 * command variant 0x3a/0x38000000 and stride. */
int FUN_8018ce70(uint32_t param_1,int32_t param_2,int32_t param_3,
                 int32_t param_4,int32_t param_5,uint32_t param_6,
                 uint32_t param_7,uint32_t param_8,uint32_t param_9,
                 int32_t param_10)
{
  uint32_t *head = (uint32_t *)guest_at(param_1);
  uint8_t *node = (uint8_t *)FUN_8018cdf8();
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
  FUN_801941e0((int)guest_of(node + 0x24),
               BR2B8(0x801e40b7),BR2B8(0x801e40b6),(uint)tag,(byte *)0);
  *(uint32_t *)node = *head | 0x08000000;
  *head = guest & 0xffffff;
  *(uint32_t *)(node + 0x24) = (guest & 0xffffff) | 0x02000000;
  *head = (guest + 0x24) & 0xffffff;
  return 0;
}

/* FUN_8018c92c — sprite-list node emit (pool FUN_8018c18c, tag
 * 0x04000000, packet tail +0x14). param_2 packs x|y<<16,
 * param_3 packs w|h<<16; param_5 low byte = colour, param_6 picks
 * GPU variant 0x66/0x64000000 + stride. */
int FUN_8018c92c(uint32_t param_1,uint32_t param_2,uint32_t param_3,
                 uint32_t param_4,int32_t param_5,int32_t param_6)
{
  uint32_t *head = (uint32_t *)guest_at(param_1);
  uint8_t *node = (uint8_t *)FUN_8018c18c();
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
  FUN_801941e0((int)guest_of(node + 0x14),
               BR2B8(0x801e40b7),BR2B8(0x801e40b6),
               (uint32_t)((int32_t)(param_3 << 16) >> 16) | (uint32_t)tag,
               (byte *)0);
  *(uint32_t *)node = *head | 0x04000000;
  *head = guest & 0xffffff;
  *(uint32_t *)(node + 0x14) = (guest & 0xffffff) | 0x02000000;
  *head = (guest + 0x14) & 0xffffff;
  return 0;
}


/* FUN_801274c4 — bump byte counter @0x1f800012 by arg+1. Disasm-verified. */
void FUN_801274c4(int param_1)
{
  *(uint8_t *)guest_at(0x1f800012) = (uint8_t)(param_1 + 1);
}

/* FUN_8012755c — clear battle-side counters @0x1f800068/74 (u32) and
 * @0x1f80007c/7e (u16). Disasm 0x8012755c-0x80127570. */
void FUN_8012755c(void)
{
  *(uint16_t *)guest_at(0x1f80007e) = 0;
  *(uint16_t *)guest_at(0x1f80007c) = 0;
  *(uint32_t *)guest_at(0x1f800074) = 0;
  *(uint32_t *)guest_at(0x1f800068) = 0;
}

/* FUN_8018e3bc — u16 lookup: idx=a0&0xffff; <3 returns lhu at
 * *(u32*)0x801b66e4 + idx*0x10, else 0. Disasm 0x8018e3bc-0x8018e3f0. */
uint32_t FUN_8018e3bc(uint32_t param_1)
{
  uint32_t idx = param_1 & 0xffff;
  uint32_t tbl;
  if (idx >= 3) return 0;
  tbl = *(uint32_t *)guest_at(0x801b66e4);
  return *(uint16_t *)guest_at(tbl + idx * 0x10);
}

/* FUN_80190f74 — SetGraphDebug level setter: byte @0x801b8322 = arg;
 * when nonzero calls PTR_FUN_801b831c (FUN_80190a04 text-window printf)
 * with "SetGraphDebug:level:%d,type:%d reverse:%d". Returns old byte.
 * Disasm 0x80190f74-0x80190fc8. */
extern void *PTR_FUN_801b831c;
int32_t FUN_80190f74(uint32_t param_1)
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

/* FUN_8015f590 — memcard/info query: FUN_8016809c(0, tbl[0..2],
 * tbl[3..9], 0x801c11e0, 0x414) over table @0x801b45f4; ret 0 clears
 * @0x801c15f0 and returns byte @(*(u32*)0x1f8000c0)+0x22, ret<8&&!=0
 * ->1, ret>=8 ->0. Disasm 0x8015f590-0x8015f63c. */
extern int32_t FUN_8016809c(int32_t,uint32_t,uint32_t,uint32_t,uint32_t,
                            uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,
                            uint32_t,uint32_t,uint32_t);
int32_t FUN_8015f590(void)
{
  uint32_t *t = (uint32_t *)guest_at(0x801b45f4);
  int32_t r;
  if (t == NULL) return -1;
  r = FUN_8016809c(0,t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],
                   0x801c11e0,0x414);
  if (r >= 8) return 0;
  if (r != 0) return 1;
  *(uint32_t *)guest_at(0x801c15f0) = 0;
  return *(uint8_t *)guest_at(*(uint32_t *)guest_at(0x1f8000c0) + 0x22);
}

/* FUN_8012a088 — debug stat print: FUN_8012cd9c(0,0xdc) then
 * FUN_8012cc6c("CPU(%d)\nGPU(%d)\n" or "#2CPU(%d)\nGPU(%d)\n",
 * u16@0x1f80001a, u16@0x1f80001c) selected by u16@0x801e83a8.
 * Disasm 0x8012a088-0x8012a0e8. */
void FUN_8012a088(void)
{
  uint16_t a1 = *(uint16_t *)guest_at(0x1f80001a);
  uint16_t a2 = *(uint16_t *)guest_at(0x1f80001c);
  FUN_8012cd9c(0,0xdc);
  if (*(uint16_t *)guest_at(0x801e83a8) == 0)
    FUN_8012cc6c((const char *)guest_at(0x8011ed40),a1,a2);
  else
    FUN_8012cc6c((const char *)guest_at(0x8011ed54),a1,a2);
}

/* FUN_801757b8 — two-player control-block init: fills guest structs at
 * 0x1f8000e8 / 0x1f8001f0 and descriptor tables 0x801f5258/0x801f52a0
 * with ptr pairs into 0x801e1b3c region. Disasm 0x801757b8-0x80175890. */
void FUN_801757b8(void)
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

/* FUN_8012a30c — OT/GPU packet-chain fixup: positions two 0x18-byte
 * records at 0x801e83e8/0x801e83d0 from u16 fields + scroll offsets,
 * then links five slots at 0x801e8348..0x801e8388 plus tail slots
 * 0x801e8398/0x801e83d0/0x801e83b8: each slot gets (*T1 | tag) and
 * *T1 becomes the slot's 24-bit address. T1 = *(u32*)0x1f800008.
 * Disasm 0x8012a30c-0x8012a4b0. */
void FUN_8012a30c(void)
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

/* FUN_8012a0f0 — sprite/UV rect table init: FUN_801940c0 x3 over
 * 0x18-stride recs @0x801e83b8, fills three textured-quad descriptors
 * (u16 pos/uv fields from args, byte tags 0x28/0xc8), FUN_80194160 x6
 * over 0x10-stride recs @0x801e8348, then six flat-shade quads.
 * Disasm 0x8012a0f0-0x8012a304. */
extern void FUN_801940c0(uint32_t);
extern void FUN_80194160(uint32_t);
static void sh16(uint32_t a,uint32_t o,uint32_t v)
{ *(uint16_t *)((uint8_t *)guest_at(a) + o) = (uint16_t)v; }
static void sb8(uint32_t a,uint32_t o,uint32_t v)
{ *(uint8_t *)((uint8_t *)guest_at(a) + o) = (uint8_t)v; }
void FUN_8012a0f0(int32_t param_1,int32_t param_2,int32_t param_3)
{
  uint32_t s3 = (uint32_t)param_1, s2 = (uint32_t)param_2;
  (void)param_3;
  uint32_t v1 = 0x801e83b8, a1 = v1 + 0x18, a0 = v1 + 0x30;
  uint32_t b, i;
  for (i = 0; i < 3; i++) FUN_801940c0(v1 + i * 0x18);
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
  for (i = 0; i < 6; i++) FUN_80194160(0x801e8348 + i * 0x10);
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

/* FUN_8012aab8 — empty function (guest: jr ra; nop). */
void FUN_8012aab8(void) { }

/* FUN_801360cc — clear 0x34-stride records @0x801f0300, count =
 * s16 @0x801f04a8+0x14. Disasm 0x801360cc-0x80136108+. */
void FUN_801360cc(void)
{
  uint16_t *a0 = (uint16_t *)guest_at(0x801f0300);
  int16_t *cnt = (int16_t *)guest_at(0x801f04a8 + 0x14);
  int32_t n = cnt ? *cnt : 0, i;
  for (i = 0; i < n; i++) a0[i * 0x1a] = 0;   /* 0x34 bytes = 0x1a u16 */
}

/* FUN_80142d58 — reset both combatant slots: 42c5c/42c9c/42cd8
 * on slots 0 and 1. Disasm 0x80142d58-0x80142d98. */
extern void FUN_80142c5c(int32_t);
extern void FUN_80142c9c(int32_t);
extern void FUN_80142cd8(int32_t);
void FUN_80142d58(void)
{
  FUN_80142c5c(0); FUN_80142c5c(1);
  FUN_80142c9c(0); FUN_80142c9c(1);
  FUN_80142cd8(0); FUN_80142cd8(1);
}

/* FUN_80155e30 — init block @0x801f26c0: +4=0x100, clear
 * @0x801f36c4, FUN_8019c0f0(block), FUN_8019bf10(0).
 * Disasm 0x80155e30-0x80155e64. */
extern void FUN_8019c0f0(uint32_t);
extern void FUN_8019bf10(int32_t);
void FUN_80155e30(void)
{
  uint32_t blk = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = 0;
  *(uint32_t *)guest_at(blk + 4) = 0x100;
  FUN_8019c0f0(blk);
  FUN_8019bf10(0);
}

/* FUN_8016f2c4 — clear 10 u16s at 0x801f4ee8 + byte(a0+2)*24 + 0x12,
 * plus u16 @+0x14/+0x16 and 9 words before +0x12. Disasm verified. */
void FUN_8016f2c4(uint8_t *a0)
{
  uint8_t *v1 = (uint8_t *)guest_at(0x801f4ee8) + a0[2] * 24;
  uint16_t *v0 = (uint16_t *)(v1 + 0x12);
  int i;
  v1[0x14] = 0; v1[0x15] = 0;
  *(uint16_t *)(v1 + 0x16) = 0;
  for (i = 9; i >= 0; i--) *v0-- = 0;
}

/* FUN_80155dd0 — block @0x801f26c0 variant init: +0=7, +4=0x100,
 * +8/+0xa=0x3fff, clear @0x801f36c4, FUN_8019c0f0, FUN_8019caa0,
 * FUN_8019bf10(1). Disasm 0x80155dd0-0x80155e28. */
extern void FUN_8019caa0(uint32_t);
void FUN_80155dd0(void)
{
  uint32_t s0 = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = 0;
  *(uint32_t *)guest_at(s0) = 7;
  *(uint32_t *)guest_at(s0 + 4) = 0x100;
  *(uint16_t *)guest_at(s0 + 8) = 0x3fff;
  *(uint16_t *)guest_at(s0 + 0xa) = 0x3fff;
  FUN_8019c0f0(s0);
  FUN_8019caa0(s0);
  FUN_8019bf10(1);
}

/* FUN_8016fbfc — scratchpad pair init: FUN_8015a1e4(0x1f800000),
 * clear +0x99/+0x9a/+0xe4/+0xf4 on 0x1f8000e8 & 0x1f8001f0,
 * bump u16 +0xe8 on both, FUN_80159dac. Disasm 0x8016fbfc-6c6c. */
extern int FUN_8015a1e4(uint32_t);
extern int32_t FUN_80159dac(uint32_t);
void FUN_8016fbfc(void)
{
  uint8_t *s0 = (uint8_t *)guest_at(0x1f8000e8);
  uint8_t *s1 = (uint8_t *)guest_at(0x1f8001f0);
  uint8_t *z = (uint8_t *)guest_at(0x1f800000);
  (void)FUN_8015a1e4(0x1f800000);
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
  FUN_80159dac(0x1f800000);
}

/* FUN_80127be4 — reset sprite/OT block: zero 8 u16 at *(a0+0xcc),
 * s8(+0x98)->+0x12, s8(+0x99)->+0x14, lhu(+0xa0)->+0x16, then five
 * u16=-1 stores stride -0x235c from 0x801cd530. a0 guest addr.
 * Disasm 0x80127be4-0x80127c58. */
void FUN_80127be4(uint32_t a0)
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

/* FUN_80142c5c — reset combat record block: 16 sub-slots stride 4
 * at 0x801f0fb0 + a0*0x84: [i]=0, [i+0x40]=0x64. Disasm verified. */
void FUN_80142c5c(int32_t a0)
{
  uint8_t *base = (uint8_t *)guest_at(0x801f0fb0 + a0 * 0x84);
  int i;
  if (base == NULL) return;
  for (i = 0; i < 16; i++) {
    *(uint32_t *)(base + i * 4) = 0;
    *(uint32_t *)(base + 0x40 + i * 4) = 0x64;
  }
}

/* FUN_80155e6c — block @0x801f26c0 variant init B: +0x36c4=a0,
 * +0=7, +4=a0|0x100, +8/+0xa=a1, 9c0f0, 9caa0, 9cb20(1,0xffffFF08),
 * 9bf10(1). Disasm 0x80155e6c-0x80155ed4. */
extern void FUN_8019cb20(int32_t,uint32_t);
void FUN_80155e6c(int32_t a0,uint32_t a1)
{
  uint32_t s0 = 0x801f26c0;
  *(uint32_t *)guest_at(0x801f36c4) = (uint32_t)a0;
  *(uint32_t *)guest_at(s0) = 7;
  *(uint32_t *)guest_at(s0 + 4) = (uint32_t)a0 | 0x100;
  *(uint16_t *)guest_at(s0 + 8) = (uint16_t)a1;
  *(uint16_t *)guest_at(s0 + 0xa) = (uint16_t)a1;
  FUN_8019c0f0(s0);
  FUN_8019caa0(s0);
  FUN_8019cb20(1,0xffffFF08);
  FUN_8019bf10(1);
}

/* FUN_80136d28 — copy 0x1a-byte record 0x8011fea0 + idx*0x1a ->
 * 0x801f0640 then FUN_80136ddc. Disasm 0x80136d28-0x80136dd4. */
void FUN_80136d28(int a0)
{
  memcpy(guest_at(0x801f0640),guest_at(0x8011fea0 + a0 * 0x1a),0x1a);
  FUN_80136ddc();
}
