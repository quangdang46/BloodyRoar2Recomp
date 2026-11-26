/*
 * ev.c — clean-room reference implementation of the BR2 event VM.
 *
 * Event streams (.ACT/.ANI embedded) are consumed by the dispatcher
 * ev_stream_dispatch: each record starts with a type byte (0..23) dispatched
 * through table 0x801b4e68. Handlers return the next stream pointer;
 * NULL ends the stream. motion+0x90 is a frame countdown decremented
 * by motion+0xce each dispatcher call; while >0 the dispatcher
 * returns early (frame waits).
 *
 * Conventions (verified):
 *   a0 = fighter ctrl, a1 = motion obj, a2 = stream ptr,
 *   a3 = param/target object (often 0xFFFFFFFF).
 *   Record u16@+0: byte0 = event type, byte1 = class/op byte.
 *   class bits byte1>>8 & 0xc0 select target (self / foe=ctrl+0x2c /
 *   beast-gated); low bits select the nested op.
 *
 * All state via raw guest offsets — see types.h / vm.c.
 * Project-original clean-room code (MIT).
 */
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "lut.h"
#include "cpu_state.h"

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
/* faithful guest-verified ports living in menu.c (same (ctrl,motion,pc)
 * guest-addr calling convention, return = next stream guest addr) */
extern uint32_t desc_cursor_select(uint32_t,uint32_t,uint32_t);
extern uint32_t desc_transform_dispatch(uint32_t,uint32_t,uint32_t);
extern uint32_t desc_slot_alloc(uint32_t,uint32_t,uint32_t);
extern void slot_apply_guest(int,int);
extern void slot_mark2_set(uint32_t);
/* keyframe_advance @0x80185ddc — clean port in resolve.c; (ctrl,motion)
 * host pointers into the guest arena, same convention as the callers. */
extern void keyframe_advance(uint8_t *ctrl, uint8_t *motion);

typedef uint8_t  byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint64_t undefined8;
typedef uint64_t ulonglong;

#define G16(p,o)   (*(const int16_t  *)((const uint8_t *)(p) + (o)))
#define GU16(p,o)  (*(const uint16_t *)((const uint8_t *)(p) + (o)))
#define GU32(p,o)  (*(const uint32_t *)((const uint8_t *)(p) + (o)))
#define GU8(p,o)   (((const uint8_t *)(p))[(o)])
#define GS16(p,o)  (*(int16_t  *)((uint8_t *)(p) + (o)))
#define GSU16(p,o) (*(uint16_t *)((uint8_t *)(p) + (o)))
#define GS32(p,o)  (*(int32_t  *)((uint8_t *)(p) + (o)))
#define GSU32(p,o) (*(uint32_t *)((uint8_t *)(p) + (o)))
#define GSU8(p,o)  (((uint8_t *)(p))[(o)])
#define ST(s,o)    GU32(s, o)
/* sign-extend low 12 bits — guest sll/sra pair truncating s16 params
 * (ev8 @0x8017b6c0/0x8017b84c, ev9 op5 @0x8017b99c) */
#define SX12(x)   ((int32_t)(((int32_t)(x)) << 20) >> 20)

/* ctrl offsets */
#define FC_FOE     0x2c
#define FC_ANI     0x30
#define FC_MODE5   0x05
#define FC_DISTX   0x74
#define FC_STATE0  0x58
#define FC_STATE1  0x5c
#define FC_STATE3  0x64
#define FC_STATE4  0x68
#define FC_STATE5  0x6c
#define FC_HP      0xac
#define FC_HPDISP  0xae
#define FC_MAXHP   0xb0
#define FC_ATK1    0xb8
#define FC_ATK2    0xba
#define FC_ATK3    0xbc
#define FC_CHIP_A  0xda
#define FC_CHIP_B  0xdb
#define FC_CHIP_C  0xdc
#define FC_C0      0xc0
#define FC_D0      0xd0
#define FC_D2      0xd2
#define FC_D4      0xd4
#define FC_D6      0xd6
#define FC_FLAG27  0x27
#define FC_4E      0x4e
#define FC_54      0x54
#define FC_60      0x60
#define FC_70      0x70
#define FC_76      0x76
#define FC_86      0x86
#define FC_88      0x88
#define FC_94      0x94
#define FC_8A      0x8a
#define FC_8C      0x8c
#define FC_8E      0x8e
#define FC_90      0x90
#define FC_92      0x92
#define FC_96      0x96
#define FC_98      0x98
#define FC_C4      0xc4
#define FC_C6      0xc6
#define FC_CC      0xcc
#define FC_FC      0xfc

/* motion offsets */
#define MO_PREV    0x82
#define MO_STREAM  0x84
#define MO_ALT     0x88
#define MO_98      0x98
#define MO_9A      0x9a
#define MO_9C      0x9c
#define MO_A0      0xa0
#define MO_A2      0xa2
#define MO_A4      0xa4
#define MO_A8      0xa8
#define MO_AC      0xac
#define MO_B0      0xb0
#define MO_B4      0xb4
#define MO_BC      0xbc
#define MO_BE      0xbe
#define MO_C6      0xc6
#define MO_CA      0xca
#define MO_CC      0xcc
#define MO_D0      0xd0
#define MO_D2      0xd2
#define MO_D4      0xd4
#define MO_D6      0xd6
#define MO_D8      0xd8
#define MO_E4      0xe4
#define MO_10A     0x10a
#define MO_10C     0x10c
#define MO_100     0x100
#define MO_101     0x101
#define MO_104     0x104
#define MO_F8      0xf8
#define MO_FLAGT   0x94
#define MO_C8      0xc8
#define MO_102     0x102
#define MO_103     0x103
#define MO_80      0x80
#define MO_8C      0x8c
#define MO_3A      0x3a
#define MO_112     0x112
#define MO_114     0x114
#define MO_116     0x116

static uint8_t *foe_of(uint8_t *ctrl)
{
    /* Guest handlers lw ctrl+0x2c and dereference it unconditionally;
     * guest_at(0) reproduces the low-memory read, never return NULL. */
    return (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
}
static uint8_t *mof(uint8_t *ctrl)
{
    return (uint8_t *)guest_at(GU32(ctrl, 0x38));
}


static uint8_t *ev0_branch(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev1_altstream(uint8_t*,uint8_t*);
static uint8_t *ev2_hitstun_gate(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev3_wait(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev4_branch(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev5_action(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev6_guard_sync(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev7_position(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev8_motion_param(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev9_physics(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev10_flags(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev12_damage(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev13_hit_slots(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev14_chip(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev15_hitstun_set(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev16_atkwin_init(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev17_approach(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev18_beast(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev19_flag27(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev20_timers(uint8_t*,uint8_t*,uint8_t*);
static uint8_t *ev21_skip(uint8_t*,uint8_t*,uint8_t*);

/* action_switch_guest: select event stream by action index (writes
 * motion+0x84/+0x88, resets motion+0x90). Host supplies this — it
 * indexes the .ANI move table. */
extern void action_select(uint8_t *ctrl, int idx);
/* fighter_anim_advance @0x80185e6c: guest calls it (a0=ctrl, a1=motion)
 * at 0x8017b090 after storing the reaction entry at motion+0xb0. */
extern void fighter_anim_advance(int,int);


/* ================================================================== */
/* Dispatcher                                                           */
/* ================================================================== */

/* Returns next stream ptr, or NULL to end. Mirrors ev_stream_dispatch:
 * countdown gate first, then type-byte dispatch. */
uint8_t *ev_step(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    int type;
    if (!pc) return NULL;
    /* countdown: while motion+0x90 > 0 subtract step and wait */
    if (GS16(motion, 0x90) > 0) {
        GS16(motion, 0x90) -= GS16(motion, 0xce);
        return pc;                     /* same ptr = still waiting */
    }
    type = GU8(pc, 0);
    if (type > 23) return NULL;
    /* handlers below; each returns next ptr (pc+N / branch / NULL) */
    switch (type) {
    case 0:  return ev0_branch(ctrl, motion, pc);
    case 1:  return ev1_altstream(motion, pc);
    case 2:  return ev2_hitstun_gate(ctrl, motion, pc);
    case 3:  return ev3_wait(ctrl, motion, pc);
    case 4:  return ev4_branch(ctrl, motion, pc);
    case 5:  return ev5_action(ctrl, motion, pc);
    case 6:  return ev6_guard_sync(ctrl, motion, pc);
    case 7:  return ev7_position(ctrl, motion, pc);
    case 8:  return ev8_motion_param(ctrl, motion, pc);
    case 9:  return ev9_physics(ctrl, motion, pc);
    case 10: return ev10_flags(ctrl, motion, pc);
    case 11: return pc + 2;                       /* nop */
    case 12: return ev12_damage(ctrl, motion, pc);
    case 13: return ev13_hit_slots(ctrl, motion, pc);
    case 14: return ev14_chip(ctrl, motion, pc);
    case 15: return ev15_hitstun_set(ctrl, motion, pc);
    case 16: return ev16_atkwin_init(ctrl, motion, pc);
    case 17: return ev17_approach(ctrl, motion, pc);
    case 18: return ev18_beast(ctrl, motion, pc);
    case 19: return ev19_flag27(ctrl, motion, pc);
    case 20: return ev20_timers(ctrl, motion, pc);
    case 21: return ev21_skip(ctrl, motion, pc);
    case 22: case 23:
        return NULL;                    /* battle-flow helpers, not per-fighter */
    }
    return NULL;
}

/* ================================================================== */
/* EV0 — conditional skip on motion+0x94 (9-way tbl 0x801226e4)        */
/* guest @0x8017a8b4 — faithful port lives in menu.c as                */
/* desc_cursor_select (verified insn-for-insn against the exe).        */
/* sel = sx8(byte1); reads motion+0x8c/+0x94, writes motion+0x84 on    */
/* op7, calls state_pair_init on op8.                                  */
/* ================================================================== */
static uint8_t *ev0_branch(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint32_t r = desc_cursor_select((uint32_t)guest_of(ctrl),
                                    (uint32_t)guest_of(motion),
                                    (uint32_t)guest_of(pc));
    return r ? (uint8_t *)guest_at(r) : NULL;
}

/* ================================================================== */
/* EV1 — alternate stream pointer  (guest @0x8017a990)                 */
/* b = sx8(byte1): 0 -> return motion+0x88 ; 1 -> motion+0x88 = pc+2,  */
/* return pc+2 ; anything else -> guest returns literal 1.            */
/* ================================================================== */
static uint8_t *ev1_altstream(uint8_t *motion, uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    if (b == 0) {
        uint32_t a = GU32(motion, MO_ALT);
        return a ? (uint8_t *)guest_at(a) : NULL;
    }
    if (b == 1) {
        uint8_t *np = pc + 2;
        GSU32(motion, MO_ALT) = guest_of(np);
        return np;
    }
    return (uint8_t *)guest_at(1);       /* guest: v0 = 1 */
}

/* ================================================================== */
/* EV2 — hitstun/counter gate  (guest @0x8017a9d0)                      */
/* b = sx8(byte1). b>=0: ret NULL iff s16 motion+0xc6 < b, else pc+2.  */
/* b in -7..-1: 7-way tbl 0x8012270c on victim/self state; ret pc+2    */
/* or NULL. Other negative -> NULL.                                    */
/* ================================================================== */
static uint8_t *ev2_hitstun_gate(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    uint8_t *obj;
    int ok;
    if (b >= 0)
        return (G16(motion, MO_C6) < b) ? NULL : pc + 2;
    switch (b + 7) {
    case 0: /* b==-7: ctrl+0x5c&0x28 || ctrl+0xbc>0 */
        ok = (ST(ctrl, FC_STATE1) & 0x28) != 0 || G16(ctrl, FC_ATK3) > 0;
        break;
    case 1: /* b==-6: ctrl+0xd2==0 || (mo cc==0 && ca<=0) */
        ok = G16(ctrl, FC_D2) == 0 ||
             (G16(motion, MO_CC) == 0 && G16(motion, MO_CA) <= 0);
        break;
    case 2: /* b==-5: ctrl+0x94 - obj+0x178 < 0xe7 (signed) */
        obj = (uint8_t *)guest_at(GU32(ctrl, 0x20));
        ok = (G16(ctrl, FC_94) - (int32_t)GU32(obj, 0x178)) < 0xe7;
        break;
    case 3: /* b==-4: ctrl+0x5c&0x28 || (mo cc==0 && ca<=0) */
        ok = (ST(ctrl, FC_STATE1) & 0x28) != 0 ||
             (G16(motion, MO_CC) == 0 && G16(motion, MO_CA) <= 0);
        break;
    case 4: /* b==-3: foe ctrl+0x5c & 0x80 == 0 */
        obj = (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
        ok = (GU32(obj, FC_STATE1) & 0x80) == 0;
        break;
    case 5: /* b==-2: obj+0x38 >= ctrl+0x94 (signed) */
        obj = (uint8_t *)guest_at(GU32(ctrl, 0x20));
        ok = (int32_t)GU32(obj, 0x38) >= (int32_t)G16(ctrl, FC_94);
        break;
    case 6: /* b==-1: mo cc==0 && ca<=0 */
        ok = G16(motion, MO_CC) == 0 && G16(motion, MO_CA) <= 0;
        break;
    default:
        return NULL;
    }
    return ok ? pc + 2 : NULL;
}

/* ================================================================== */
/* EV3 — frame wait  (guest @0x8017ab30)                               */
/* byte1==0xff -> frames = s16 ctrl+0xbe <<4 ; else byte1<<4.          */
/* Always stores motion+0x84 = pc+2 and returns 0 (end this dispatch). */
/* ================================================================== */
static uint8_t *ev3_wait(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint8_t b = GU8(pc, 1);
    int16_t frames;
    if (b == 0xff) frames = (int16_t)(G16(ctrl, 0xbe) << 4);
    else           frames = (int16_t)(b << 4);
    GS16(motion, 0x90) = frames;
    GSU32(motion, MO_STREAM) = guest_of(pc + 2);
    return NULL;
}

/* ================================================================== */
/* EV4 — conditional branch  (guest @0x8017ab74, tbl 0x8012272c, 27)    */
/* cls = byte1 & 0xc0: 0x00/0x40 -> t=ctrl (0x40 needs !(ctrl+5&1)     */
/* else ret pc+4), 0x80 -> t=ctrl iff ctrl+5&1 (else ret pc+4),        */
/* 0xc0 -> t=foe ctrl. op = byte1 & 0x3f; op>=27 -> fail.              */
/* pass -> pc+4 ; fail -> ctrl+0x30 (.ANI base) + u16@pc+2.            */
/* ops 5-9 also store motion+0x8c = pc+4 on the fail path.             */
/* ================================================================== */
static uint8_t *ev4_branch(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint8_t cls = GU8(pc, 1) & 0xc0;
    int op = GU8(pc, 1) & 0x3f;
    uint8_t *t;
    int32_t f;
    int pass;

    if (cls == 0x40) {
        if (GU8(ctrl, FC_MODE5) & 1) return pc + 4;
        t = ctrl;
    } else if (cls == 0x80) {
        if (!(GU8(ctrl, FC_MODE5) & 1)) return pc + 4;
        t = ctrl;
    } else if (cls == 0xc0) {
        t = (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
    } else {
        t = ctrl;
    }
    if (op >= 0x1b) goto fail;

    f = GS32(motion, MO_FLAGT);
    switch (op) {
    case 0: case 15: pass = 0; break;                    /* always fail */
    case 1:  pass = (f != 0); break;
    case 2:  pass = (f == 0); break;
    case 3:  pass = (f <  0); break;
    case 4:  pass = (f >= 0); break;
    case 5:  GSU32(motion, MO_8C) = guest_of(pc + 4); pass = 0; break;
    case 6:  pass = (f != 0);
             if (!pass) GSU32(motion, MO_8C) = guest_of(pc + 4); break;
    case 7:  pass = (f == 0);
             if (!pass) GSU32(motion, MO_8C) = guest_of(pc + 4); break;
    case 8:  pass = (f <  0);
             if (!pass) GSU32(motion, MO_8C) = guest_of(pc + 4); break;
    case 9:  pass = (f >= 0);
             if (!pass) GSU32(motion, MO_8C) = guest_of(pc + 4); break;
    case 10: pass = (GU8(t, 5) & 1) != 0; break;
    case 11: pass = (GU8(t, 5) & 1) == 0; break;
    case 12: pass = !(G16(motion, MO_CC) == 0 && G16(motion, MO_CA) <= 0);
             break;
    case 13: pass = (GU8(t, 0) & 0x80) == 0; break;
    case 14: pass = !(GU8(t, 5) & 0x10) && G16(t, FC_D6) < 0x1e00; break;
    case 16: pass = GU8(t, 3) != 0; break;
    case 17: case 18: case 19: case 20: case 21:
    case 22: case 23: case 24: case 25: case 26:
        pass = GU8(t, 3) != (uint8_t)(op - 16); break;    /* char-id != N */
    default: pass = 0; break;
    }
    if (pass) return pc + 4;
fail:
    return (uint8_t *)guest_at(GU32(ctrl, FC_ANI) + GU16(pc, 2));
}

/* ================================================================== */
/* EV5 — action/move select (tbl 0x8012279c, 8 ops)                    */
/* ================================================================== */
/* EV5 = ev5_action_select (guest-verified). Record = 4 bytes:
 *   {u8 pad, u8 ctrl, s16 param}
 * ctrl: op = ctrl&0xf, foe-ctrl arg = ctrl&0x40, mode = (ctrl>>4)&3.
 * All state writes go to SELF motion; the foe bit only swaps the ctrl
 * arg passed to action_switch (target_g in guest). op5 alone reads the
 * OPPONENT's reaction tables (ctrl+0x2c -> +0x38) but still writes
 * self motion. param==-1 = auto-index via motion+0xa0 (wrap +0xa4).
 * mode gate (approx — Ghidra merged mode1 branches): mode1 always
 * skips per literal decompile; mode2 skips unless ctrl+5&1. */
static uint8_t *ev5_action(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint8_t  cb   = GU8(pc, 1);
    int      foe  = (cb & 0x40) != 0;
    int      mode = (cb >> 4) & 3;
    int      op   = cb & 0x0f;
    int16_t  param = G16(pc, 2);
    uint8_t *swctrl = foe ? foe_of(ctrl) : ctrl;
    int16_t  idx;

    /* guest @0x8017ae88: mode1 tests ctrl+5&1 — flag SET -> pc+4,
     * flag CLEAR -> fall through to op dispatch @0x8017aebc.
     * (Same gate shape as EV8/EV9 mode1.) */
    if (mode == 1 && (GU8(ctrl, FC_MODE5) & 1)) return pc + 4;
    if (mode == 2 && !(GU8(ctrl, FC_MODE5) & 1))
        return pc + 4;

    switch (op) {
    case 0: {   /* .ACT/reaction table select, idx = param or auto */
        int16_t cnt = G16(motion, MO_A4);
        uint8_t *tabA, *tabB;
        uint32_t eA, eB;
        if (param == -1) {
            idx = G16(motion, MO_A0);
            if (idx >= cnt) { GS16(motion, MO_A0) = 0; idx = 0; }
            GS16(motion, MO_A0) = (int16_t)(idx + 1);
        } else idx = param;
        if (idx >= cnt) break;
        {
            uint32_t ta = GU32(motion, MO_A8);
            uint32_t tb = GU32(motion, MO_AC);
            /* guest lws +0xa8/+0xac and reads the indexed u32 unconditionally */
            tabA = (uint8_t *)guest_at(ta);
            tabB = (uint8_t *)guest_at(tb);
        }
        eA = GU32(tabA, idx * 4);
        eB = GU32(tabB, idx * 4);
        /* guest tests tabB(+0xac) entry first: <0x80000001 -> use
         * tabA(+0xa8) entry as data ptr (0 -> fallback action 1);
         * >=0x80000001 -> tabB entry IS the ptr, be=0x80. */
        if (eB < 0x80000001u) {
            if (eA == 0) goto fallback;
            GS16(motion, MO_BE) = G16(motion, MO_BC);
            GSU32(motion, MO_B0) = eA;
        } else {
            GS16(motion, MO_BE) = 0x80;
            GSU32(motion, MO_B0) = eB;
        }
        GS16(motion, MO_A2) = (int16_t)idx;
        fighter_anim_advance((int)guest_of(ctrl),(int)guest_of(motion));
        return pc + 4;
    fallback:
        GS16(motion, 0x80) = 1;
        GS16(motion, MO_PREV) = 1;
        action_select(swctrl, 1);
        {
            uint32_t s = GU32(motion, MO_STREAM);
            return s ? (uint8_t *)guest_at(s) : NULL;
        }
    }
    case 1:     /* action switch: idx = param==-1 ? motion+0x98 : param */
        idx = (param == -1) ? G16(motion, MO_98) : param;
        GS16(motion, 0x80) = idx;
        GS16(motion, MO_PREV) = idx;
        action_select(swctrl, idx);
        {   /* new stream ptr */
            uint32_t s = GU32(motion, MO_STREAM);
            return s ? (uint8_t *)guest_at(s) : NULL;
        }
    case 2: GS16(motion, MO_98) = param; break;
    case 3: GS16(motion, MO_9A) = param; break;
    case 4: GS16(motion, 0x16) = param; break;
    case 5: {   /* forced reaction via OPPONENT's tables (writes self) */
        uint8_t *foe_ctrl = foe_of(ctrl);
        uint8_t *fm = mof(foe_ctrl);
        int16_t fcnt = G16(fm, MO_A4);
        uint32_t fta = GU32(fm, MO_AC);
        uint8_t *ftab = (uint8_t *)guest_at(fta);
        uint32_t e;
        /* guest 0x8017b008-0x8017b00c: param >= foe count -> common
         * return (pc+4), NO fallback side effects. Only an in-range
         * entry <= 0x80000000 takes the action-1 fallback. */
        if (param < fcnt) {
            if ((e = GU32(ftab, param * 4)) > 0x80000000u) {
                GS16(motion, MO_BE) = G16(motion, MO_BC);
                GSU32(motion, MO_B0) = e;
                GS16(motion, MO_A2) = param;
                fighter_anim_advance((int)guest_of(ctrl),(int)guest_of(motion));
            } else {
                GS16(motion, 0x80) = 1;
                GS16(motion, MO_PREV) = 1;
                action_select(swctrl, 1);
                {
                    uint32_t s = GU32(motion, MO_STREAM);
                    return s ? (uint8_t *)guest_at(s) : NULL;
                }
            }
        }
        break;
    }
    case 6: GS16(motion, 0x14) = param; break;
    case 7: {   /* indexed u16 slot write */
        int slot = GU8(motion, 0x7d) - 1;
        GS16(motion, 0x3c + slot*2) = param;
        break;
    }
    }
    return pc + 4;
}

/* ================================================================== */
/* EV6 — guard/counter state sync (guest @0x8017b0d4)                  */
/* b = sx8(byte1): -4..1 handled, others -> pc+2. Cases -4/-1/-2/-3   */
/* touch motion+0x9c/+0x100 (self or foe-motion copy); all but the    */
/* early-outs run a common tail swapping u16 pairs in *(ctrl+0x18).    */
/* ================================================================== */
static uint8_t *ev6_guard_sync(uint8_t *ctrl, uint8_t *motion,
                               uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    uint8_t *sub;
    uint16_t a, c;

    switch (b) {
    case -4:                                    /* 0x8017b174 */
        if (GU8(motion, MO_9C) == 0) return pc + 2;
        /* fall through */
    case -1:                                    /* 0x8017b184 */
        GSU8(motion, MO_9C)  = 0;
        GSU8(motion, MO_100) = GU8(motion, MO_100) ^ 1;
        break;
    case -3: {                                  /* 0x8017b114 */
        uint8_t *fo = (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
        uint8_t *fm = (uint8_t *)guest_at(GU32(fo, 0x38));
        GSU8(motion, MO_9C) = GU8(fm, MO_9C);
        if (GU8(motion, MO_100) == GU8(fm, MO_100)) return pc + 2;
        GSU8(motion, MO_100) = GU8(fm, MO_100);
        break;
    }
    case -2:                                    /* 0x8017b15c */
        GSU8(motion, MO_9C) = 0;
        if (GU8(motion, MO_100) == 0) return pc + 2;
        GSU8(motion, MO_100) = 0;
        break;
    case 0: case 1:                             /* 0x8017b1d4 */
        GSU8(motion, MO_9C) = GU8(pc, 1);
        return pc + 2;
    default:
        return pc + 2;
    }
    /* common tail: swap u16 pairs in ctrl+0x18 sub-struct */
    sub = (uint8_t *)guest_at(GU32(ctrl, 0x18));
    a = GU16(sub, 0x1c4); c = GU16(sub, 0x23c);
    GSU16(sub, 0x1c4) = c; GSU16(sub, 0x23c) = a;
    a = GU16(sub, 0x264); c = GU16(sub, 0x28c);
    GSU16(sub, 0x264) = c; GSU16(sub, 0x28c) = a;
    return pc + 2;
}

/* ================================================================== */
/* EV7 — position/physics command  (guest @0x8017b1ec)                 */
/* 257-insn transform/effect dispatcher (10-way class tbl @0x801227d4 */
/* + nested op decode, live guest-stack/GTE-adjacent state). Faithful */
/* port lives in menu.c as desc_transform_dispatch — already the     */
/* clean_table target for guest callers of 0x8017B1EC.                */
/* ================================================================== */
static uint8_t *ev7_position(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint32_t r = desc_transform_dispatch((uint32_t)guest_of(ctrl),
        (uint32_t)guest_of(motion), (uint32_t)guest_of(pc));
    return r ? (uint8_t *)guest_at(r) : NULL;
}

/* ================================================================== */
/* EV8 — motion-param ISA  (guest @0x8017b5f4)                         */
/* b = sx8(byte1); mode = b>>4, op = b&0xf. mode1 -> ret pc+4 iff      */
/* ctrl+5&1; mode2 -> ret pc+4 iff !(ctrl+5&1); op>=0xb -> pc+4.       */
/* 4-byte records; writes motion+0xd0/+0xe4/+0xd4/+0xd8.               */
/* ================================================================== */
static uint8_t *ev8_motion_param(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    int mode = b >> 4;
    int op = b & 0xf;
    int16_t p = G16(pc, 2);
    uint8_t *t;
    uint32_t f;
    int32_t dir, d, e;

    if (mode == 1) { if (GU8(ctrl, FC_MODE5) & 1) return pc + 4; }
    else if (mode == 2) { if (!(GU8(ctrl, FC_MODE5) & 1)) return pc + 4; }
    if (op >= 0xb) return pc + 4;

    switch (op) {
    case 0:  GSU16(motion, MO_D0) = GU16(pc, 2); break;
    /* guest stores sx12(sum) for every op except op0 (lhu->sh direct)
     * and op8 (ret pc+4); see sll/sra 20 @0x8017b6c0,0x8017b84c */
    case 1:  GS16(motion, MO_D0) = (int16_t)SX12(G16(motion, MO_D0) + p); break;
    case 2:  t = (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
             GS16(motion, MO_D0) = (int16_t)SX12(G16(t, FC_4E) + p); break;
    case 3:  GS16(motion, MO_E4) = (int16_t)SX12(G16(ctrl, FC_DISTX) + p); break;
    case 4:  GS16(motion, MO_E4) = (int16_t)SX12(G16(ctrl, FC_C4)
                 - (G16(ctrl, FC_4E) - 0x800) + p); break;
    case 5:  GS16(motion, MO_E4) = (int16_t)SX12(G16(ctrl, FC_70) + p); break;
    case 6:  /* facing decode via ctrl+0x6c bits — guest assigns a3 per
         * tested bit in order 0x10,0x20,0x40,0x80,0x01,0x02,0x04,0x08
         * (last set bit wins; @0x8017b710-0x8017b77c) */
        f = ST(ctrl, FC_STATE5);
        dir = 0;
        if (f & 0x10) dir = -0x400;
        if (f & 0x20) dir = 0x400;
        if (f & 0x40) dir = 0;
        if (f & 0x80) dir = -0x800;
        if (f & 0x01) dir = -0x400;
        if (f & 0x02) dir = 0x400;
        if (f & 0x04) dir = 0;
        if (f & 0x08) dir = -0x800;
        GS16(motion, MO_E4) = (int16_t)SX12(dir
            - (G16(ctrl, FC_4E) - G16(motion, MO_E4)) + p);
        break;
    case 7:
        if (!(ST(ctrl, FC_STATE1) & 0x100000)) {
            d = G16(ctrl, FC_DISTX); e = G16(motion, MO_E4);
            if (d < -p)      GS16(motion, MO_E4) = (int16_t)SX12(e - p);
            else if (p < d)  GS16(motion, MO_E4) = (int16_t)SX12(e + p);
            else             GS16(motion, MO_E4) = (int16_t)SX12(e + d);
        }
        break;
    case 8:  break;                                     /* nop */
    case 9:  t = (uint8_t *)guest_at(GU32(ctrl, 0x20));
        GS16(motion, MO_D4) = (int16_t)GU16(t, 0x174);
        GS16(motion, MO_D8) = (int16_t)GU16(t, 0x17c);
        GS16(motion, MO_E4) = (int16_t)SX12(G16(ctrl, FC_C4)
            - (G16(ctrl, FC_4E) - 0x800) + p);
        break;
    case 10: GS16(motion, MO_E4) = (int16_t)SX12(G16(motion, MO_E4) + p); break;
    }
    return pc + 4;
}

/* ================================================================== */
/* EV9 — physics/velocity ISA  (guest @0x8017b860)                     */
/* b = sx8(byte1); mode = b>>4, op = b&0xf; mode1 -> pc+6 iff flag,   */
/* mode2 -> pc+6 iff !flag; op>=0xb -> pc+6. 6-byte records; writes    */
/* ctrl+0x8a..0x98 (velocities/clamps).                                */
/* ================================================================== */
static uint8_t *ev9_physics(uint8_t *ctrl, uint8_t *motion,
                            uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    int mode = b >> 4;
    int op = b & 0xf;
    int16_t p1 = G16(pc, 2), p2 = G16(pc, 4);
    int32_t a1, v;

    if (mode == 1) { if (GU8(ctrl, FC_MODE5) & 1) return pc + 6; }
    else if (mode == 2) { if (!(GU8(ctrl, FC_MODE5) & 1)) return pc + 6; }
    if (op >= 0xb) return pc + 6;

    switch (op) {
    case 0:  GSU16(ctrl, FC_8A) = GU16(pc, 2);
             GSU16(ctrl, FC_8C) = GU16(pc, 4); break;
    case 1:  GSU16(ctrl, FC_96) = GU16(pc, 2);
             GSU16(ctrl, FC_98) = GU16(pc, 4); break;
    case 2:  GSU16(ctrl, FC_90) = GU16(pc, 2);
             GSU16(ctrl, FC_92) = GU16(pc, 4); break;
    case 3:  GSU16(ctrl, FC_8E) = GU16(pc, 2); break;
    case 4:  /* ctrl+0x96 = u16 ctrl+0x74 ; ctrl+0x98 = (min(p1,ctrl+0x76)
             * 128) / p2  (MIPS div; p2==0 -> LO undefined, use -1) */
        GSU16(ctrl, FC_96) = GU16(ctrl, FC_DISTX);
        a1 = G16(ctrl, FC_76);
        if (p1 < a1) a1 = p1;               /* a1 = min(p1, ctrl+0x76) */
        /* guest div @0x8017B97C: p2==0 -> LO = (dividend>=0 ? -1 : +1),
         * dividend = a1<<7 whose sign is a1's sign (a1 is s16-range). */
        GS16(ctrl, FC_98) = (int16_t)(p2 ? ((a1 << 7) / p2)
                                       : (a1 < 0 ? 1 : -1));
        break;
    case 5:  GS16(ctrl, FC_96) = (int16_t)SX12(G16(ctrl, FC_70) + p1);
             GSU16(ctrl, FC_98) = GU16(pc, 4); break;
    case 6:  GSU16(ctrl, FC_96) = GU16(pc, 2); break;
    case 7:  GSU16(ctrl, FC_92) = GU16(pc, 2); break;
    case 8:  GSU16(ctrl, FC_98) = (uint16_t)(GU16(ctrl, FC_98)
                                          + GU16(pc, 2)); break;
    case 9:  /* ctrl+0x98 = sx16(u16 sum) clamped [-0x500, 0x1000] */
        v = (int16_t)(GU16(ctrl, FC_98) + GU16(pc, 2));
        if (v >= 0x1001)      v = 0x1000;
        else if (v < -0x500)  v = -0x500;
        GS16(ctrl, FC_98) = (int16_t)v;
        break;
    case 10: GS16(ctrl, FC_98) = (int16_t)(((int32_t)G16(ctrl, FC_98)
                                          * p1) >> 7); break;
    }
    (void)a1; (void)v; (void)p2; (void)motion;
    return pc + 6;
}

/* ================================================================== */
/* EV10 — FLAG35 state/flag ISA  (guest @0x8017ba3c)                   */
/* cls = byte1 & 0xc0 (same gate as EV4, ret pc+6 on reject);          */
/* op = byte1 & 0x3f (<0x23). val = u16@+2 | u16@+4<<16.               */
/* op<25: field = {0x58,0x60,0x5c,0x64,0x68}[op%5], grp = op/5;        */
/* op25-29: field 0x6c; op30-34: field 0x54. grp: 0 SET 1 AND 2 OR    */
/* 3 XOR 4 TEST (result -> motion+0x94). op0 also commits ctrl+0x88.   */
/* ================================================================== */
static uint8_t *ev10_flags(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint8_t cls = GU8(pc, 1) & 0xc0;
    int op = GU8(pc, 1) & 0x3f;
    uint32_t val = GU16(pc, 2) | ((uint32_t)GU16(pc, 4) << 16);
    uint8_t *t;
    static const int fo[5] = { 0x58, 0x60, 0x5c, 0x64, 0x68 };
    int off, grp;
    uint32_t cur, r;

    if (cls == 0x40) {
        if (GU8(ctrl, FC_MODE5) & 1) return pc + 6;
        t = ctrl;
    } else if (cls == 0x80) {
        if (!(GU8(ctrl, FC_MODE5) & 1)) return pc + 6;
        t = ctrl;
    } else if (cls == 0xc0) {
        t = (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
    } else {
        t = ctrl;
    }
    if (op >= 0x23) return pc + 6;

    if (op < 25)      { off = fo[op % 5]; grp = op / 5; }
    else if (op < 30) { off = FC_STATE5; grp = op - 25; }
    else              { off = FC_54;    grp = op - 30; }

    if (grp == 4) {                       /* TEST: motion+0x94 = t[off]&val */
        GSU32(motion, MO_FLAGT) = ST(t, off) & val;
        return pc + 6;
    }
    cur = ST(t, off);
    switch (grp) {
    case 0: r = val; break;
    case 1: r = cur & val; break;
    case 2: r = cur | val; break;
    default: r = cur ^ val; break;
    }
    GSU32(t, off) = r;
    if (op == 0) {
        /* attack-power commit: t+0x88 = u16 t+0x86; if flag &&
         * (val&C0000000)==0x80000000 && sx16 motion+0x80 != 0xbe ->
         * t+0x88 = u16 t+0xfc */
        GSU16(t, FC_88) = GU16(t, FC_86);
        if ((GU8(t, FC_MODE5) & 1) &&
            (val & 0xc0000000u) == 0x80000000u &&
            G16(motion, MO_80) != 0xbe)
            GSU16(t, FC_88) = GU16(t, FC_FC);
    }
    return pc + 6;
}

/* ================================================================== */
/* EV12 — damage ISA  (guest @0x8017bd94, stat_field_dispatch)         */
/* 4-byte record. sub-op = sx8(pc[0]) (0..7) indexes the jump table    */
/* @0x801228EC (8 entries, read live from guest RAM). Each case writes  */
/* sx16/u16 fields into motion+0xc2..+0xcc (and ctrl+0x88 mirrors);    */
/* case 1 (0x8017be38) additionally bumps motion+0xc6 and tail-calls   */
/* keyframe_advance(ctrl,motion). Every path returns pc+4.             */
/* ================================================================== */
static uint8_t *ev12_damage(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    int32_t op = (int8_t)GU8(pc, 0);
    uint32_t tgt;
    if ((uint32_t)op >= 8) return pc + 4;      /* sltiu idx,8 -> BFA4 */
    tgt = GU32((const uint8_t *)guest_at(0x801228ecu), (uint32_t)op * 4);

    switch (tgt) {

    /* ---- case 0: 0x8017BDE0 ---- operand = sx16(pc+2) -------------- */
    case 0x8017bde0: {
        int32_t a0 = G16(pc, 2);               /* lh */
        int32_t c  = (a0 < 2);                 /* slti (pre-shift) */
        a0 <<= 4;                              /* delay slot */
        if (c == 0) {
            /* BE18: op>=2 — mirror op*16 into +c8/+ca, fold into +c2 */
            int32_t v1 = GU16(motion, MO_CA);
            int32_t w0 = GU16(motion, 0xc2);
            GSU16(motion, MO_C8) = (uint16_t)a0;
            GSU16(motion, MO_CA) = (uint16_t)a0;
            v1 = a0 - v1;
            w0 = w0 + v1;
            GSU16(motion, 0xc2) = (uint16_t)w0;
        } else {
            /* BDF4: op<2 — +c2 += ctrl88 - ca; then j BF4C whose delay
             * slot stores ctrl88 into +c8 AND +ca (BF14/BF50). */
            int32_t v1 = GU16(ctrl, FC_88);
            int32_t t0 = GU16(motion, MO_CA);
            int32_t w0 = GU16(motion, 0xc2);
            v1 -= t0;
            w0 += v1;
            GSU16(motion, 0xc2) = (uint16_t)w0;
            v1 = GU16(ctrl, FC_88);            /* BE0C reload */
            GSU16(motion, MO_C8) = (uint16_t)v1;/* BE14 delay store */
            GSU16(motion, MO_CA) = (uint16_t)v1;/* BF50 delay store */
        }
        return pc + 4;
    }

    /* ---- case 1: 0x8017BE38 ---- operand = u16(pc+2) --------------- */
    case 0x8017be38: {
        uint32_t v0 = GU16(pc, 2);             /* lhu */
        int32_t  v1 = (int32_t)GU32(motion, MO_B4);
        GSU16(motion, MO_C6) = (uint16_t)v0;   /* sh -> +c6 */
        /* sll16/sra14 == sx16(v0)*4 */
        v0 = (uint32_t)(((int32_t)(v0 << 16)) >> 14);
        v0 = v0 + (uint32_t)v1;                /* record ptr (guest) */
        {
            const uint8_t *rec = (const uint8_t *)guest_at(v0);
            int32_t a0 = G16(rec, 2);          /* lh 2(rec) */
            int32_t c  = (a0 < 2);
            a0 <<= 4;                          /* delay slot */
            if (c != 0) {
                /* BE64 (t<2): +c2 += ctrl88 - cc; +cc = ctrl88;
                 * j BEA0 — the BE88 tail is NOT executed on this path. */
                int32_t t1 = GU16(ctrl, FC_88);
                int32_t t0 = GU16(motion, MO_CC);
                int32_t w0 = GU16(motion, 0xc2);
                t1 -= t0;
                w0 += t1;
                GSU16(motion, 0xc2) = (uint16_t)w0;
                GSU16(motion, MO_CC) = GU16(ctrl, FC_88);
            } else {
                /* BE88 (t>=2): +cc = t*16; +c2 += t*16 - old_cc */
                int32_t u1 = GU16(motion, MO_CC);
                int32_t u0 = GU16(motion, 0xc2);
                GSU16(motion, MO_CC) = (uint16_t)a0;
                u1 = a0 - u1;
                u0 += u1;
                GSU16(motion, 0xc2) = (uint16_t)u0;
            }
        }
        /* BEA0: +c6++ then keyframe_advance(ctrl,motion) */
        GSU16(motion, MO_C6) = (uint16_t)(GU16(motion, MO_C6) + 1);
        keyframe_advance(ctrl, motion);
        return pc + 4;
    }

    /* ---- case 2: 0x8017BEBC ---- operand = sx16(ctrl+0xbe) --------- */
    case 0x8017bebc: {
        int32_t a0 = G16(ctrl, 0xbe);
        int32_t c  = (a0 < 2);
        int32_t v0 = a0 << 4;                 /* BECC delay slot -> v0 */
        if (c == 0) {
            /* BF48: +c8 = t*16, j BF4C -> +ca = t*16 (BF50 delay) */
            GSU16(motion, MO_C8) = (uint16_t)v0;
            GSU16(motion, MO_CA) = (uint16_t)v0;
        } else {
            /* BED0: +c8 = ctrl88 (BED8 delay), j BF4C -> +ca = ctrl88 */
            uint16_t w = GU16(ctrl, FC_88);
            GSU16(motion, MO_C8) = w;
            GSU16(motion, MO_CA) = w;
        }
        return pc + 4;
    }

    /* ---- case 3: 0x8017BEDC ---- operand = sx16(pc+2) -------------- */
    case 0x8017bedc: {
        int32_t a0 = G16(pc, 2);
        int32_t v1 = GU16(motion, MO_CC);
        int32_t v0 = a0 << 4;
        int32_t w0 = GU16(motion, 0xc2);
        v1 += v0;
        w0 += v0;
        GSU16(motion, MO_CC) = (uint16_t)v1;
        GSU16(motion, 0xc2)  = (uint16_t)w0;
        return pc + 4;
    }

    /* ---- case 4: 0x8017BF00 ---- ctrl+0xb2 / pc+2 blend ------------ */
    case 0x8017bf00: {
        int32_t v1 = G16(ctrl, 0xb2);
        int32_t a0, v0, c2;
        if (v1 < 0) v1 += 511;                /* BF08 bgez / BF10 */
        v1 >>= 9;                             /* sra */
        a0 = v1 + G16(pc, 2);
        if (a0 >= 16) {                       /* BF24 bne / BF2C clamp */
            a0 = 15;
            c2 = 0;
        } else {
            c2 = (a0 < 2);                    /* BF28 delay slti */
        }
        v0 = a0 << 4;                         /* BF38 delay slot */
        if (c2 != 0) {
            /* BF3C: v0 = ctrl88, j BF4C (delay stores +c8) */
            v0 = GU16(ctrl, FC_88);
        }
        /* BF48 (c2==0) or BF44 delay (c2!=0): +c8 = v0 */
        GSU16(motion, MO_C8) = (uint16_t)v0;
        GSU16(motion, MO_CA) = (uint16_t)v0;   /* BF50 delay store */
        return pc + 4;
    }

    /* ---- case 5: 0x8017BF54 ---- ctrl+0xc2 -> shared BF64 ---------- */
    case 0x8017bf54:
        GSU16(motion, MO_CC) = (uint16_t)(
            GU16(motion, MO_CC) + ((uint32_t)G16(ctrl, 0xc2) << 4));
        return pc + 4;

    /* ---- case 6: 0x8017BF60 ---- ctrl+0xbe -> shared BF64 ---------- */
    case 0x8017bf60:
        GSU16(motion, MO_CC) = (uint16_t)(
            GU16(motion, MO_CC) + ((uint32_t)G16(ctrl, 0xbe) << 4));
        return pc + 4;

    /* ---- case 7: 0x8017BF78 ---- operand = sx16(pc+2) -------------- */
    case 0x8017bf78: {
        int32_t a0 = G16(pc, 2) << 4;
        GSU16(motion, MO_CA) = (uint16_t)(GU16(motion, MO_CA) + a0);
        GSU16(motion, 0xc2)  = (uint16_t)(GU16(motion, 0xc2) + a0);
        GSU16(motion, MO_C8) = (uint16_t)(GU16(motion, MO_C8) + a0);
        return pc + 4;
    }

    default:
        return pc + 4;
    }
}

/* ================================================================== */
/* EV13 — hit-slot manager  (guest @0x8017bfb8)                        */
/* ~232-insn slot-table allocator/commit. Faithful port lives in      */
/* menu.c as desc_slot_alloc — already the clean_table target for     */
/* guest callers of 0x8017BFB8.                                       */
/* ================================================================== */
static uint8_t *ev13_hit_slots(uint8_t *ctrl, uint8_t *motion,
                               uint8_t *pc)
{
    uint32_t r = desc_slot_alloc((uint32_t)guest_of(ctrl),
        (uint32_t)guest_of(motion), (uint32_t)guest_of(pc));
    return r ? (uint8_t *)guest_at(r) : NULL;
}

/* MIPS signed-divide-by-constant idiom: mult x,magic ; mfhi ;
 * (hi + x) >> sh ; - (x >> 31). Used by EV14 op3 (0xda77b1c9, >>17)
 * and EV17 (0xb40b40b5, >>7). */
static int32_t ev_fixdiv(int32_t x, uint32_t magic, int sh)
{
    int64_t p = (int64_t)x * (int32_t)magic;
    int32_t hi = (int32_t)(p >> 32);
    int32_t s = (int32_t)(hi + x) >> sh;
    return s - (x >> 31);
}

/* ================================================================== */
/* EV14 — chip bytes -> ctrl+0xda/+0xdc + foe-motion gauge fields      */
/* guest @0x8017c35c. t0 = u8@pc+2, t1 = sx8@pc+3. mode gate like      */
/* EV8/9 (ret pc+4). op = byte1&0xf: 0 assign, 1/2 add, 3 scaled       */
/* assign, >=4 no write. Tail always: motion+0x3a += t0 (u16),         */
/* foe-motion+0x112 = (ctrl+0x64&0x10000000)?0:0x40,                    */
/* foe-motion+0x114 from ctrl+0x64&0x38 / chip thresholds.              */
/* ================================================================== */
static uint8_t *ev14_chip(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    int mode = b >> 4;
    int op = GU8(pc, 1) & 0xf;
    int32_t t0 = GU8(pc, 2);                  /* guest $t0: u8 or scaled */
    int8_t  t1 = (int8_t)GU8(pc, 3);
    uint8_t *fo, *fm;
    uint32_t f;
    int32_t scale, prod, v;
    int chip;

    if (mode == 1) { if (GU8(ctrl, FC_MODE5) & 1) return pc + 4; }
    else if (mode == 2) { if (!(GU8(ctrl, FC_MODE5) & 1)) return pc + 4; }

    switch (op) {
    case 0:                                     /* assign */
        GSU8(ctrl, FC_CHIP_A) = (uint8_t)t0;
        GSU8(ctrl, FC_CHIP_C) = (uint8_t)t1;
        break;
    case 1: case 2:                             /* add (u8 wrap) */
        GSU8(ctrl, FC_CHIP_A) = (uint8_t)(GU8(ctrl, FC_CHIP_A) + t0);
        GSU8(ctrl, FC_CHIP_C) = (uint8_t)(GU8(ctrl, FC_CHIP_C) + t1);
        break;
    case 3: {                                   /* scaled assign */
        int32_t d6 = G16(ctrl, FC_D6);
        if (ST(ctrl, FC_STATE5) & 0x100)
            scale = d6 * 2 + 0x12bfb;
        else
            scale = d6 * 3 + 0x1a3f9;
        prod = (int32_t)((int64_t)scale * t0);  /* mflo */
        t0 = ev_fixdiv(prod, 0xda77b1c9u, 17);   /* full int32 kept */
        GSU8(ctrl, FC_CHIP_A) = (uint8_t)t0;
        GSU8(ctrl, FC_CHIP_C) = (uint8_t)t1;
        break;
    }
    default: break;                             /* op>=4: no write */
    }

    /* common tail */
    GSU16(motion, MO_3A) = (uint16_t)(GU16(motion, MO_3A) + (uint32_t)t0);
    fo = (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
    fm = (uint8_t *)guest_at(GU32(fo, 0x38));
    if (ST(ctrl, FC_STATE3) & 0x10000000)
        GS16(fm, MO_112) = 0;
    else
        GS16(fm, MO_112) = 0x40;
    f = GU32(ctrl, FC_STATE3) & 0x38;
    if (f == 8)            v = 0x4b;
    else if (f < 9) {
        if (f == 0)        v = 4;
        else              goto chip_path;
    } else if (f == 0x10) v = 0x4c;
    else if (f == 0x20)   v = 4;
    else {
chip_path:
        chip = GU8(ctrl, FC_CHIP_A);
        if (chip >= 0x1a)      v = 4;
        else if (chip >= 0x15) v = 0x4c;
        else                   v = 0x4b;
    }
    /* guest @0x8017c4f4-0x8017c5ac: every +0x114 store uses
     * lw 56(a3) = SELF motion (ctrl+0x38), not the foe motion used
     * for +0x112 above. */
    {
        uint8_t *sm = (uint8_t *)guest_at(GU32(ctrl, 0x38));
        GS16(sm, MO_114) = (int16_t)v;
    }
    return pc + 4;
}

/* ================================================================== */
/* EV15 — hitstun-class set  (guest @0x8017c5b8)                       */
/* mode gate (ret pc+4). ctrl+0xdb = u8@pc+2 ; ctrl+0xc6 =             */
/* sx16(u16@pc+2)>>4, negated iff motion+0x100 != 0.                   */
/* ================================================================== */
static uint8_t *ev15_hitstun_set(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    int mode = b >> 4;
    int32_t v;
    if (mode == 1) { if (GU8(ctrl, FC_MODE5) & 1) return pc + 4; }
    else if (mode == 2) { if (!(GU8(ctrl, FC_MODE5) & 1)) return pc + 4; }
    GSU8(ctrl, FC_CHIP_B) = GU8(pc, 2);
    v = G16(pc, 2) >> 4;
    if (GU8(motion, MO_100) != 0) v = -v;
    GS16(ctrl, FC_C6) = (int16_t)v;
    return pc + 4;
}

/* ================================================================== */
/* EV16 — attack-window/cancel counter init  (guest @0x8017c66c)       */
/* mode gate (ret pc+4 on mode1&flag / mode2&!flag). Otherwise         */
/* motion+0x101..0x104 = 0xff, *(ctrl+0x14) = ctrl+0x26 = 0xff,         */
/* ctrl+0xbc = (byte1&0xf)?-1:0, then ctrl+0xba/0xb8 from sx8 bytes    */
/* @pc+2/+3 (pc+2<0 -> slot_mark2_set(ctrl+2), 0xba=-1),               */
/* motion+0x116 = u16 ctrl+0xba.                                       */
/* ================================================================== */
static uint8_t *ev16_atkwin_init(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    int mode = b >> 4;
    int flag = GU8(ctrl, FC_MODE5) & 1;
    uint8_t *sub, *mo;
    uint8_t vb;
    int8_t  b3;

    if (mode == 1 && flag) return pc + 4;
    if (mode == 2 && !flag) return pc + 4;
    /* error stores — all non-early-return mode paths write all four */
    GSU8(motion, MO_101) = 0xff;
    GSU8(motion, MO_102) = 0xff;
    GSU8(motion, MO_103) = 0xff;
    GSU8(motion, MO_104) = 0xff;
    sub = (uint8_t *)guest_at(GU32(ctrl, 0x14));
    GSU8(ctrl, 0x26) = 0xff;
    GSU8(sub, 0) = 0xff;
    GS16(ctrl, FC_ATK3) = (GU8(pc, 1) & 0xf) ? -1 : 0;
    if ((int8_t)GU8(pc, 2) < 0) {
        slot_mark2_set(GU8(ctrl, 2));
        GS16(ctrl, FC_ATK2) = -1;
    } else {
        vb = GU8(pc, 2);
        GS16(ctrl, FC_ATK2) = (int16_t)(vb << 4);
        if (vb == 0) GSU8(ctrl, 0x26) = 0;
    }
    b3 = (int8_t)GU8(pc, 3);
    GS16(ctrl, FC_ATK1) = (b3 < 0) ? -1 : (int16_t)(b3 << 4);
    mo = mof(ctrl);
    GS16(mo, MO_116) = GU16(ctrl, FC_ATK2);
    return pc + 4;
}

/* ================================================================== */
/* EV17 — range-gated approach  (guest @0x8017c7b0)                    */
/* b = sx8(byte1). b==0: thresh = (ctrl+0x5c&0x40)?G16(pc,2):0x400;     */
/* dist = sx16(ctrl+0x74 + motion+0xd2) abs; dist>=thresh -> ret pc+4. */
/* else motion+0x10a = fixdiv(thresh,0xb40b40b5,7), +0x10c = thresh.   */
/* b!=0: motion+0x10a = b, +0x10c = u16@pc+2.                          */
/* tail: ctrl+0x5c = (old&0xf7ffffff) then bit0x20000 = (+0x10a!=0).   */
/* ================================================================== */
static uint8_t *ev17_approach(uint8_t *ctrl, uint8_t *motion,
                              uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    int32_t thresh, dist, step;
    uint32_t s1;

    if (b == 0) {
        thresh = (ST(ctrl, FC_STATE1) & 0x40) ? G16(pc, 2) : 0x400;
        /* guest: sx16(ctrl+0x74) + sx16(motion+0xd2), then <<20>>20
         * = sign-extend low 12 bits before abs (0x8017c7ec-0x8017c800). */
        dist = (int32_t)((G16(ctrl, FC_DISTX) + G16(motion, MO_D2)) << 20) >> 20;
        if (dist < 0) dist = -dist;
        if (dist >= thresh) return pc + 4;
        GS16(motion, MO_10C) = (int16_t)thresh;
        step = ev_fixdiv(thresh, 0xb40b40b5u, 7);
        GS16(motion, MO_10A) = (int16_t)step;
    } else {
        GS16(motion, MO_10A) = (int16_t)b;
        GSU16(motion, MO_10C) = GU16(pc, 2);
    }
    s1 = ST(ctrl, FC_STATE1) & 0xf7ffffffu;
    if (G16(motion, MO_10A) != 0) s1 |= 0x20000;
    else                          s1 &= 0xfffdffffu;
    GSU32(ctrl, FC_STATE1) = s1;
    return pc + 4;
}

/* ================================================================== */
/* EV18 — beast/variant mode switch  (guest @0x8017c97c)               */
/* ctrl+5 = sx8(byte1); slot_apply_guest(ctrl, byte1 & 1); ret pc+2.   */
/* ================================================================== */
static uint8_t *ev18_beast(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    GSU8(ctrl, FC_MODE5) = (uint8_t)b;
    slot_apply_guest((int)guest_of(ctrl), b & 1);
    (void)motion;
    return pc + 2;
}

/* ================================================================== */
/* EV19 — single flag byte                                             */
/* ================================================================== */
static uint8_t *ev19_flag27(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    GSU8(ctrl, FC_FLAG27) = GU8(pc, 1);
    (void)motion;
    return pc + 2;
}

/* ================================================================== */
/* EV20 — x16 timers  (guest @0x8017c9d4)                              */
/* op = sx8(byte1) (<5 unsigned). ctrl+{c0,d0,d2,cc,d4} = s16@+2 <<4;  */
/* op0 only stores if current ctrl+0xc0 >= 0. ret pc+4 always.        */
/* ================================================================== */
static uint8_t *ev20_timers(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    int op = (int8_t)GU8(pc, 1);
    int16_t v;
    if ((uint32_t)op >= 5u) return pc + 4;
    v = (int16_t)(G16(pc, 2) << 4);
    switch (op) {
    case 0: if (G16(ctrl, FC_C0) >= 0) GS16(ctrl, FC_C0) = v; break;
    case 1: GS16(ctrl, FC_D0) = v; break;
    case 2: GS16(ctrl, FC_D2) = v; break;
    case 3: GS16(ctrl, FC_CC) = v; break;
    case 4: GS16(ctrl, FC_D4) = v; break;
    }
    (void)motion;
    return pc + 4;
}

/* ================================================================== */
/* EV21 — 6-byte record skip  (guest @0x8017ca84)                      */
/* mode/flag checks exist in guest but every path returns pc+6.        */
/* ================================================================== */
static uint8_t *ev21_skip(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    (void)ctrl; (void)motion;
    return pc + 6;
}
extern void fighter_anim_advance(int,int);
extern void action_switch_guest(int,int*,int);
ushort * ev5_action_select(int ctrl_g,int foe_g,ushort *stream)
{
  short act_idx;
  undefined4 pend_g;
  ushort *next;
  uint opcode;
  int motion_g;
  int foe_motion_g;
  int op_word;
  int off;
  uint subop;
  ushort operand;
  int idx;
  int target_g;
  /* ctrl_g/foe_g arrive as guest addresses; stream is a host
   * stream pointer. Bridge the guest addrs once here. */
  uint8_t *ctrl = (uint8_t *)guest_at((uint32_t)ctrl_g);
  uint8_t *foe = (uint8_t *)guest_at((uint32_t)foe_g);
  uint8_t *motion;
  uint8_t *foe_motion;
  operand = stream[1];
  idx = (int)(short)operand;
  op_word = ((int)((uint)*stream << 0x10) >> 0x18) << 0x10;
  opcode = op_word >> 0x10;
  target_g = ctrl_g;
  if ((opcode & 0x40) != 0) {
    target_g = (int)GU32(ctrl, 0x2c);
  }
  motion_g = (int)GU32(ctrl, 0x38);
  motion = (uint8_t *)guest_at((uint32_t)motion_g);
  subop = op_word >> 0x14 & 3;
  if (subop == 1) {
    if ((GU8(ctrl, 5) & 1) == 0) goto L_8017aebc;
    goto L_8017ae9c;
  }
  if (((1 < subop) && (subop == 2)) && ((GU8(ctrl, 5) & 1) == 0)) {
    return stream + 2;
  }
L_8017aebc:
  next = (ushort *)0x0;
  switch(opcode & 0xf) {
  case 0:
    if (idx == -1) {
      idx = (int)*(short *)(foe + 0xa0);
      if (*(short *)(foe + 0xa4) <= idx) {
        *(undefined2 *)(foe + 0xa0) = 0;
        idx = 0;
      }
      *(short *)(foe + 0xa0) = *(short *)(foe + 0xa0) + 1;
    }
    off = idx * 4;
    if (idx < *(short *)(foe + 0xa4)) {
      if (*(uint *)((uint8_t *)guest_at(GU32(foe, 0xac)) + off) < 0x80000001) {
        if (*(int *)((uint8_t *)guest_at(GU32(foe, 0xa8)) + off) == 0) goto L_8017b034;
        *(undefined2 *)(foe + 0xbe) = *(undefined2 *)(foe + 0xbc);
        *(undefined4 *)(foe + 0xb0) = *(undefined4 *)((uint8_t *)guest_at(GU32(foe, 0xa8)) + off);
      }
      else {
        *(undefined2 *)(foe + 0xbe) = 0x80;
        *(undefined4 *)(foe + 0xb0) = *(undefined4 *)((uint8_t *)guest_at(GU32(foe, 0xac)) + off);
      }
      *(short *)(foe + 0xa2) = (short)idx;
L_8017b090:
      fighter_anim_advance(ctrl_g,foe_g);
      return stream + 2;
    }
    break;
  case 1:
    if (idx == -1) {
      operand = *(ushort *)(motion + 0x98);
    }
    *(ushort *)(motion + 0x80) = operand;
    act_idx = *(short *)(motion + 0x80);
    *(ushort *)(motion + 0x82) = operand;
    goto L_8017b044;
  case 2:
    *(ushort *)(foe + 0x98) = operand;
    break;
  case 3:
    *(ushort *)(foe + 0x9a) = operand;
    break;
  case 4:
    *(ushort *)(foe + 0x16) = operand;
    break;
  case 5:
    foe_motion_g = (int)GU32((const uint8_t *)guest_at(GU32(ctrl, 0x2c)), 0x38);
    foe_motion = (uint8_t *)guest_at((uint32_t)foe_motion_g);
    if (idx < *(short *)(foe_motion + 0xa4)) {
      if (0x80000000 < *(uint *)((uint8_t *)guest_at(GU32(foe_motion, 0xac)) + idx * 4)) {
        *(undefined2 *)(foe + 0xbe) = *(undefined2 *)(foe + 0xbc);
        pend_g = *(undefined4 *)((uint8_t *)guest_at(GU32(foe_motion, 0xac)) + idx * 4);
        *(ushort *)(foe + 0xa2) = operand;
        *(undefined4 *)(foe + 0xb0) = pend_g;
        goto L_8017b090;
      }
L_8017b034:
      *(undefined2 *)(motion + 0x80) = 1;
      act_idx = *(short *)(motion + 0x80);
      *(undefined2 *)(motion + 0x82) = 1;
L_8017b044:
      action_switch_guest(target_g,(int *)motion,(int)act_idx);
      {
        uint32_t ns = GU32(foe, 0x84);
        return ns ? (ushort *)guest_at(ns) : (ushort *)0;
      }
    }
    break;
  case 6:
    *(ushort *)(foe + 0x14) = operand;
    break;
  case 7:
    next = stream + 2;
    *(ushort *)(foe + (*(byte *)(foe + 0x7d) - 1) * 2 + 0x3c) = operand;
  default:
    goto L_8017aedc_default;
  }
L_8017ae9c:
  next = stream + 2;
L_8017aedc_default:
  return next;

}

/* ---- guest dispatch table 0x801b4e68 ----
 * Guest handlers are called as h(ctrl, motion) with the stream ptr
 * passed residually in a2 (the stream pc); return = next stream ptr
 * in $v0 or 0 to end. Our handlers take pc explicitly as pc_g —
 * identical semantics; the dispatcher mirrors each nonzero next-pc
 * into motion+0x84 exactly like the guest's sw @0x8017EAF4.
 * Slots 22/23 are the
 * handlers 0x80141b78/0x80141d2c ported below.
 * pc_g = guest stream pc passed explicitly like the guest's $a2. */
typedef uint8_t *evhandler_t(uint8_t *ctrl, uint8_t *motion, uint32_t pc_g);
extern uint8_t *g_DAT_1f8000b1, *g_DAT_1f8000e8, *g_DAT_801f0760;
extern uint32_t *g__DAT_801f093a, *g__DAT_801f093e;
extern int fx_group_alloc(undefined4,undefined4,int,int);
extern undefined4 noop_c(int);
extern void noop_a(int,undefined4,undefined4,undefined4);
extern void node_param_quad_set(int,undefined4,undefined4,undefined4,undefined4);
extern void hit_spark_spawn(undefined4,uint,undefined2);
extern void speech_bubble_spawn(undefined4*,short,int,ushort);
extern void color_rec_table_init(uint32_t,uint32_t);
extern void effect_arg_set(int);
extern void sprite_burst_spawn(int,int);
extern void confetti_burst_spawn(int,int);
extern void popup_spawn(int,int,int);
extern void firework_spawn(undefined2*);
extern void triple_track_spawn(int,undefined2,int);
extern void iris_wipe_spawn(int,int);
extern void noop_b(int,undefined4,undefined4);
extern void emit_task_spawn_b(undefined4*,undefined4,undefined4,undefined4);
extern int fx_record_alloc(undefined4,int,uint,undefined4,undefined4);
extern uint rand_lcg(void);
extern uint32_t *g__DAT_1f8000c4, *g__DAT_801f0780, *g__DAT_801f0790;
extern uint8_t *g_DAT_801b180c;
extern uint8_t *g_DAT_801b1d5c;
extern uint32_t *g_PTR_DAT_801b1d44;
extern void fighter_reset_fx(int,byte*);

/* ev_spark_spawn — EV22: spawn hit-spark/effect task. a2 = pc (stream
 * ptr); consumes one u16 record, returns pc+2. */
ushort * ev_spark_spawn(int ctrl_g,undefined4 unused,ushort *stream)
{
  ushort rec;
  int task_g;
  int tmp;
  /* ctrl_g arrives as a guest address; stream is a host stream ptr. */
  uint8_t *ctrl = (uint8_t *)guest_at((uint32_t)ctrl_g);
  uint8_t *task;

  if (*g_DAT_1f8000b1 == 0) {
    /* guest: s1 = sx8(byte@pc+1); slti s1,3 is SIGNED — negative
     * opcodes still take the alloc path (@0x80141bb4-0x80141bc4) */
    tmp = (int)(int8_t)(*stream >> 8);
    if (tmp < 3) {
      tmp = fx_group_alloc(1,1,0,0);
      if (tmp != 0) {
        task_g = (int)noop_c(tmp);
        task = (uint8_t *)guest_at((uint32_t)task_g);
        noop_a(tmp,GU32(ctrl, 0x20) + 0x124,
                     (uint32_t)ctrl_g,guest_of(g_DAT_801f0760));
        rec = *stream;
        *(uint32_t *)(task + 0x18) = 0x80140710;
        *(uint32_t *)(task + 0x1c) = 0;
        /* guest: sll16/sra24 -> sx8(byte@pc+1), sw @task+0x14
         * (@0x80141c38-0x80141c44) */
        *(int *)(task + 0x14) = (int)(int8_t)(rec >> 8);
      }
    }
    else if ((tmp == 3) &&
               (tmp = fx_group_alloc(1,1,GU8(ctrl,2) + 1,0), tmp != 0)) {
        task_g = (int)noop_c(tmp);
        task = (uint8_t *)guest_at((uint32_t)task_g);
        node_param_quad_set(tmp,(uint32_t)ctrl_g,
                     guest_of(g_DAT_1f8000e8 + (GU8(ctrl,2) ^ 1) * 0x108),
                     (uint32_t)(GU8(ctrl,2) + 1),
                     guest_of(g_DAT_801f0760));
        *(uint32_t *)(task + 0x14) =
             (uint32_t)GU8((uint8_t *)guest_at(GU32(ctrl, 0x38)), 0x100);
        if (*(int8_t *)(ctrl + 3) == 3) {
          *(uint32_t *)(task + 0x18) = 0x1f800000;
          /* guest: lh @0x801f093a / lh @0x801f093e — signed 16-bit
           * loads (@0x80141ce8/@0x80141cf8), not u32 */
          *(int *)(task + 0x1c) = (int)*(const int16_t *)g__DAT_801f093a;
          tmp = (int)*(const int16_t *)g__DAT_801f093e;
          *(uint32_t *)(task + 0x24) = 0x8013f178;
          *(int *)(task + 0x20) = tmp;
        }
    }
  }
  return stream + 1;
}

static uint8_t *w22(uint8_t *c, uint8_t *m, uint32_t pc_g)
{
    return (uint8_t *)ev_spark_spawn(
        (int)guest_of(c), 0, (ushort *)guest_at(pc_g));
}

/* ev_cmd_dispatch — EV23: event/sound/effect command dispatcher.
 * Consumes a 4-byte record {u8 op, u8 arg, u16 param}; returns pc+4. */
ushort * ev_cmd_dispatch(int ctrl_g,undefined4 unused,ushort *stream)
{
  ushort param16;
  undefined2 rand_val;
  undefined2 *fx_rec;
  int *fx_rec2;
  int tmp;
  uint arg7;
  uint param32;
  undefined2 vec[4];
  /* ctrl_g arrives as a guest address; stream is a host stream ptr. */
  uint8_t *ctrl = (uint8_t *)guest_at((uint32_t)ctrl_g);
  uint8_t *motion;

  param16 = stream[1];
  param32 = (uint)param16;
  if (*g_DAT_1f8000b1 == 0) {
    switch((int)((uint)*stream << 0x10) >> 0x18) {
    case 0:
      motion = (uint8_t *)guest_at(GU32(ctrl, 0x38));
      if ((*(int8_t *)(motion + 0x100) == 0) || ((param16 & 0x80) == 0)) {
        arg7 = param32 & 0x7f;
      }
      else {
        arg7 = param32 & 0xff;
      }
      hit_spark_spawn(ctrl_g,arg7,(int)(int8_t)(param32 >> 8));
      break;
    case 1:
      triple_track_spawn(GU8(ctrl,2),param32 & 0xff,(int)(int8_t)(param32 >> 8));
      break;
    case 2:
      /* guest @0x80141DF0: a2 = sx8(param>>8) = sx8(byte@pc+3),
       * a3 = param&0xFF = byte@pc+2 — order matters, not swapped. */
      speech_bubble_spawn((undefined4 *)guest_at(GU32(ctrl, 0x20) + 0x124),
                   *(short *)(ctrl + 0x4e) + -0x400,
                   (int)(int8_t)(param32 >> 8),(ushort)(param32 & 0xff));
      break;
    case 3:
      speech_bubble_spawn((undefined4 *)guest_at(GU32(ctrl, 0x20) + 0x124),
                   *(short *)(ctrl + 0x4e) + 0x400,
                   (int)(int8_t)(param32 >> 8),(ushort)(param32 & 0xff));
      break;
    case 4:
      effect_arg_set(ctrl_g);
      break;
    case 5:
      sprite_burst_spawn(ctrl_g,(int)(short)param16);
      break;
    case 6:
      confetti_burst_spawn(ctrl_g,(int)(short)param16);
      break;
    case 7:
      if (*(int8_t *)((uint8_t *)guest_at(*g__DAT_1f8000c4) + 2) != 0) {
        popup_spawn(ctrl_g,(int)(int8_t)(param32 >> 8),param32 & 0xff);
      }
      break;
    case 8:
      iris_wipe_spawn(ctrl_g,(int)(short)param16);
      break;
    case 9: {
      /* guest: s0 = sx16(param); if motion+0x100: s0 = lbu(0x801b180c+s0);
       * offset = s0*0x50 (@0x80141f24-0x80141f74). Index stays signed —
       * guest adds sx16 to the table base, so negative params read
       * before the table exactly like the guest does. */
      int32_t pi = (int16_t)param16;
      motion = (uint8_t *)guest_at(GU32(ctrl, 0x38));
      if (*(int8_t *)(motion + 0x100) != 0) {
        pi = (int32_t)g_DAT_801b180c[pi];
      }
      tmp = fx_group_alloc(1,1,0,0xf);
      if (tmp != 0) {
        noop_b(tmp,GU32(ctrl, 0x20) + (uint32_t)(pi * 0x50) + 0x34,
                     guest_of(g_DAT_801f0760));
      }
      break;
    }
    case 10:
      color_rec_table_init(GU8(ctrl,3),(uint32_t)(int16_t)param16);
      break;
    case 0xb:
      firework_spawn((undefined2 *)guest_at(GU32(ctrl, 0x20) +
                        (uint32_t)((int16_t)param16 * 0x50) + 0x34));
      break;
    case 0xc: {
      uint8_t *obj;
      arg7 = (int)(int8_t)(param32 >> 8) & 0x7f;
      motion = (uint8_t *)guest_at(GU32(ctrl, 0x38));
      if ((*(int8_t *)(motion + 0x100) != 0) &&
         (((int)(param32 << 0x10) >> 0x10 & 0x8000U) != 0)) {
        arg7 = (uint)g_DAT_801b180c[arg7];
      }
      tmp = arg7 * 0x50;
      obj = (uint8_t *)guest_at(GU32(ctrl, 0x20));
      /* guest copies 3 u16 to its stack vec; emit_task_spawn_b reads
       * 8 bytes — keep the host vec contiguous and padded like the
       * guest frame (@0x80141e18-0x80141e50) */
      vec[0] = *(undefined2 *)(obj + tmp + 0x34);
      vec[1] = *(undefined2 *)(obj + tmp + 0x38);
      vec[2] = *(undefined2 *)(obj + tmp + 0x3c);
      vec[3] = 0;
      tmp = (param32 & 0xff) * 4;
      emit_task_spawn_b((undefined4 *)vec,
                   *(uint32_t *)((char *)g__DAT_801f0790 + tmp),
                   *(uint32_t *)((char *)g__DAT_801f0780 + tmp),0);
      break;
    }
    case 0xd:
      fx_rec = (undefined2 *)guest_at((uint32_t)fx_record_alloc(GU32(ctrl, 0x20) + 0x124,6,
                            0x80000010,2,*g_PTR_DAT_801b1d44));
      if (fx_rec != (undefined2 *)0x0) {
        rand_val = (undefined2)rand_lcg();
        *fx_rec = rand_val;
      }
      break;
    case 0xe:
      fx_rec2 = (int *)guest_at((uint32_t)fx_record_alloc(GU32(ctrl, 0x20) + 0x444,0xc,
                                   0x80000012,0x8e,guest_of(g_DAT_801b1d5c)));
      if (fx_rec2 != (int *)0x0) {
        *fx_rec2 = ctrl_g;
        fx_rec2[1] = (int)guest_of(g_DAT_1f8000e8 + (GU8(ctrl,2) ^ 1) * 0x108);
      }
      break;
    case 0xf:
      fighter_reset_fx(1,ctrl);
    }
  }
  return stream + 2;
}

static uint8_t *w23(uint8_t *c, uint8_t *m, uint32_t pc_g)
{
    return (uint8_t *)ev_cmd_dispatch(
        (int)guest_of(c), 0, (ushort *)guest_at(pc_g));
}
#define EVWRAP(nm, fn) \
    static uint8_t *nm(uint8_t *c, uint8_t *m, uint32_t pc_g) \
    { return fn(c, m, (uint8_t *)guest_at(pc_g)); }
EVWRAP(w0,  ev0_branch)
static uint8_t *w1(uint8_t *c, uint8_t *m, uint32_t pc_g)
{ (void)c; return ev1_altstream(m, (uint8_t *)guest_at(pc_g)); }
EVWRAP(w2,  ev2_hitstun_gate)
EVWRAP(w3,  ev3_wait)
EVWRAP(w4,  ev4_branch)
static uint8_t *w5(uint8_t *c, uint8_t *m, uint32_t pc_g)
{
    return (uint8_t *)ev5_action_select(
        (int)guest_of(c), (int)guest_of(m),
        (ushort *)guest_at(pc_g));
}
EVWRAP(w6,  ev6_guard_sync)
EVWRAP(w7,  ev7_position)
EVWRAP(w8,  ev8_motion_param)
EVWRAP(w9,  ev9_physics)
EVWRAP(w10, ev10_flags)
static uint8_t *w11(uint8_t *c, uint8_t *m, uint32_t pc_g)
{ (void)c; return (uint8_t *)guest_at(pc_g + 2); }
EVWRAP(w12, ev12_damage)
EVWRAP(w13, ev13_hit_slots)
EVWRAP(w14, ev14_chip)
EVWRAP(w15, ev15_hitstun_set)
EVWRAP(w16, ev16_atkwin_init)
EVWRAP(w17, ev17_approach)
EVWRAP(w18, ev18_beast)
EVWRAP(w19, ev19_flag27)
EVWRAP(w20, ev20_timers)
EVWRAP(w21, ev21_skip)

evhandler_t *PTR_FUN_801b4e68[24] = {
    w0, w1, w2, w3, w4, w5, w6, w7,
    w8, w9, w10, w11, w12, w13, w14, w15,
    w16, w17, w18, w19, w20, w21, w22, w23
};

/* ==========================================================================
 * ev24_fx_spawn — event opcode 24 handler @0x801421C4.
 *
 * ONE logical guest function split by CPS into host pieces
 * (br2eu_func_801421C4 2172B / br2eu_func_80142A40 204B /
 * br2eu_func_80142B0C 48B epilogue). Verified vs disc/SLES_017.22:
 * the only pointers into [0x801421C4,0x80142B3C) are the 9-entry
 * jump table @0x80120228 ({0x80142ac4,0x80142b00 x3,0x801429f4,
 * 0x80142a44,0x80142b00,0x801428d4,0x80142954}) consumed by the
 * single `jr $v0` @0x801428CC, plus the ev-handler table slot
 * 0x801b4ec8 (opcode 24). No external jal/jalr targets the interior
 * alias addresses, so they live inside this one clean frame.
 *
 * Faithful port: single clean function, block_* labels + goto,
 * live cpu->gpr[] mirroring, real 88-byte guest frame reproduced
 * on $sp (prologue spills s0-s7/fp/ra @sp+48..84; epilogue
 * 0x80142B0C reloads them, sp+=88, jr ra). The `jr $v0` @0x801428CC
 * became a switch->goto on the resolved table target (all six are
 * interior; default tail-dispatches via call_guest4). Each `jal`
 * became br2_jal(target,link)+goto-resume — synchronous, identical
 * register image. `lw t2,0x60(sp)` @0x80142B00 reads the caller
 * frame exactly like the guest (frame is live, not elided).
 * ======================================================================== */

extern uint32_t br2_ld8(uint32_t);
extern uint16_t br2_ld16(uint32_t);
extern uint32_t br2_ld32(uint32_t);
extern void br2_st16(uint32_t, uint16_t);
extern void br2_st32(uint32_t, uint32_t);
extern int br2_bail_pending(void);
extern uint32_t call_guest4(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
extern CPUState *clean_cpu_state(void);
extern int clean_try_dispatch(CPUState *cpu, uint32_t addr);
extern void psx_dispatch_call(CPUState *cpu, uint32_t target_addr,
                              uint32_t return_addr);

/* br2_jal — faithful guest `jal` from clean C (same contract as
 * bind.c bind_jal / cd.c cd_jal): args already live in gpr[4..7],
 * caller-saved regs keep callee leftovers, $ra = guest link so BIOS
 * services inside the callee see the same caller token, $v0 returns
 * in gpr[2]. */
static uint32_t br2_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
    psx_dispatch_call(cpu, fn, link);
    cpu->gpr[29] = sp;
    {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
}

static uint32_t ev24_cluster(uint32_t entry)
{
  CPUState *cpu = clean_cpu_state();
  switch (entry) {
  case 0x801421C4u: goto block_801421C4;
  case 0x801428D4u: goto block_801428D4;
  case 0x80142954u: goto block_80142954;
  case 0x801429F4u: goto block_801429F4;
  case 0x80142A40u: goto block_80142A40;
  case 0x80142A44u: goto block_80142A44;
  case 0x80142AC4u: goto block_80142AC4;
  case 0x80142B00u: goto block_80142B00;
  case 0x80142B0Cu: goto block_80142B0C;
  default: return cpu->gpr[2];
  }
block_801421C4:
    { uint32_t _pgx1 = cpu->gpr[29]; cpu->gpr[29] = cpu->gpr[29] + -88;
    }  /* 0x801421C4: 0x27BDFFA8 */
    { uint32_t _pgxa = cpu->gpr[29] + 56; br2_st32(cpu->gpr[29] + 56, cpu->gpr[18]);
    }  /* 0x801421C8: 0xAFB20038 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[18] = cpu->gpr[4];  /* move */
    }  /* 0x801421CC: 0x00809021 */
    { uint32_t _pgxa = cpu->gpr[29] + 84; br2_st32(cpu->gpr[29] + 84, cpu->gpr[31]);
    }  /* 0x801421D0: 0xAFBF0054 */
    { uint32_t _pgxa = cpu->gpr[29] + 80; br2_st32(cpu->gpr[29] + 80, cpu->gpr[30]);
    }  /* 0x801421D4: 0xAFBE0050 */
    { uint32_t _pgxa = cpu->gpr[29] + 76; br2_st32(cpu->gpr[29] + 76, cpu->gpr[23]);
    }  /* 0x801421D8: 0xAFB7004C */
    { uint32_t _pgxa = cpu->gpr[29] + 72; br2_st32(cpu->gpr[29] + 72, cpu->gpr[22]);
    }  /* 0x801421DC: 0xAFB60048 */
    { uint32_t _pgxa = cpu->gpr[29] + 68; br2_st32(cpu->gpr[29] + 68, cpu->gpr[21]);
    }  /* 0x801421E0: 0xAFB50044 */
    { uint32_t _pgxa = cpu->gpr[29] + 64; br2_st32(cpu->gpr[29] + 64, cpu->gpr[20]);
    }  /* 0x801421E4: 0xAFB40040 */
    { uint32_t _pgxa = cpu->gpr[29] + 60; br2_st32(cpu->gpr[29] + 60, cpu->gpr[19]);
    }  /* 0x801421E8: 0xAFB3003C */
    { uint32_t _pgxa = cpu->gpr[29] + 52; br2_st32(cpu->gpr[29] + 52, cpu->gpr[17]);
    }  /* 0x801421EC: 0xAFB10034 */
    { uint32_t _pgxa = cpu->gpr[29] + 48; br2_st32(cpu->gpr[29] + 48, cpu->gpr[16]);
    }  /* 0x801421F0: 0xAFB00030 */
    { uint32_t _pgxa = cpu->gpr[29] + 96; br2_st32(cpu->gpr[29] + 96, cpu->gpr[6]);
    }  /* 0x801421F4: 0xAFA60060 */
    { uint32_t _pgxa = cpu->gpr[6] + 2; cpu->gpr[17] = br2_ld16(cpu->gpr[6] + 2);
    }  /* 0x801421F8: 0x94D10002 */
    { uint32_t _pgxa = cpu->gpr[6]; cpu->gpr[2] = br2_ld16(cpu->gpr[6]);
    }  /* 0x801421FC: 0x94C20000 */
    { uint32_t _pgxa = cpu->gpr[6] + 4; cpu->gpr[16] = br2_ld16(cpu->gpr[6] + 4);
    }  /* 0x80142200: 0x94D00004 */
    cpu->gpr[21] = cpu->gpr[17] & 0x00FF;  /* 0x80142204: 0x323500FF */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 16;
    }  /* 0x80142208: 0x00021400 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[4] = (int32_t)cpu->gpr[2] >> 24;
    }  /* 0x8014220C: 0x00022603 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[19] = cpu->gpr[4];  /* move */
    }  /* 0x80142210: 0x00809821 */
    { uint32_t _pgx1 = cpu->gpr[17]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[17];  /* move */
    }  /* 0x80142214: 0x02202821 */
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[22] = cpu->gpr[16];  /* move */
    }  /* 0x80142218: 0x0200B021 */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[2] = (uint32_t)cpu->gpr[16] >> 8;
    }  /* 0x8014221C: 0x00101202 */
    cpu->gpr[30] = cpu->gpr[2] & 0x000F;  /* 0x80142220: 0x305E000F */
    cpu->gpr[2] = 0x1F80 << 16;  /* 0x1F800000 */
    { uint32_t _pgxa = cpu->gpr[2] + 177; cpu->gpr[2] = (int32_t)(int8_t)br2_ld8(cpu->gpr[2] + 177);
    }  /* 0x80142228: 0x804200B1 */
    int cond_v0_eq_zero = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[23] = cpu->gpr[16] & 0x00FF;  /* 0x80142234: 0x321700FF */
    if (cond_v0_eq_zero) {
        goto block_80142240;  /* taken */
    } else {
        goto block_80142238;  /* not taken */
    }
block_80142238:
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[2] = cpu->gpr[6] + 6;
    }  /* 0x8014223C: 0x24C20006 */
    goto block_80142B0C;
block_80142240:
    { uint32_t _pgxa = cpu->gpr[18] + 56; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 56);
    }  /* 0x80142240: 0x8E420038 */
    { uint32_t _pgxa = cpu->gpr[2] + 256; cpu->gpr[2] = br2_ld8(cpu->gpr[2] + 256);
    }  /* 0x80142248: 0x90420100 */
    int cond_v0_eq_zero_2 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[2] = cpu->gpr[4] & 0x0080;  /* 0x80142254: 0x30820080 */
    if (cond_v0_eq_zero_2) {
        goto block_80142270;  /* taken */
    } else {
        goto block_80142258;  /* not taken */
    }
block_80142258:
    ;
    int cond_v0_eq_zero_3 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[3] = 0x801B << 16;  /* 0x801B0000 */
    if (cond_v0_eq_zero_3) {
        goto block_80142270;  /* taken */
    } else {
        goto block_80142260;  /* not taken */
    }
block_80142260:
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] + 6156;
    }  /* 0x80142260: 0x2463180C */
    cpu->gpr[2] = cpu->gpr[4] & 0x007F;  /* 0x80142264: 0x3082007F */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x80142268: 0x00431021 */
    { uint32_t _pgxa = cpu->gpr[2]; cpu->gpr[19] = br2_ld8(cpu->gpr[2]);
    }  /* 0x8014226C: 0x90530000 */
    /* fall through to block_80142270 */
block_80142270:
    cpu->gpr[19] = cpu->gpr[19] & 0x007F;  /* 0x80142274: 0x3273007F */
    cpu->gpr[3] = cpu->gpr[5] & 0xF000;  /* 0x80142278: 0x30A3F000 */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 0x8000;
    }  /* 0x8014227C: 0x34028000 */
    int cond_v1_eq_v0 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[5]; cpu->gpr[20] = cpu->gpr[5] << 16;
    }  /* 0x80142284: 0x0005A400 */
    if (cond_v1_eq_v0) {
        goto block_80142884;  /* taken */
    } else {
        goto block_80142288;  /* not taken */
    }
block_80142288:
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x80142288: 0x0043102A */
    int cond_v0_ne_zero = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 0xB000;
    }  /* 0x80142290: 0x3402B000 */
    if (cond_v0_ne_zero) {
        goto block_801422F8;  /* taken */
    } else {
        goto block_80142294;  /* not taken */
    }
block_80142294:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 12288;
    }  /* 0x80142294: 0x24023000 */
    int cond_v1_eq_v0_2 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 12289) ? 1 : 0;  /* 0x8014229C: 0x28623001 */
    if (cond_v1_eq_v0_2) {
        goto block_80142434;  /* taken */
    } else {
        goto block_801422A0;  /* not taken */
    }
block_801422A0:
    ;
    int cond_v0_eq_zero_4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 4096;
    }  /* 0x801422A4: 0x24021000 */
    if (cond_v0_eq_zero_4) {
        goto block_801422C0;  /* taken */
    } else {
        goto block_801422A8;  /* not taken */
    }
block_801422A8:
    ;
    int cond_v1_eq_v0_3 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 8192;
    }  /* 0x801422AC: 0x24022000 */
    if (cond_v1_eq_v0_3) {
        goto block_80142358;  /* taken */
    } else {
        goto block_801422B0;  /* not taken */
    }
block_801422B0:
    ;
    int cond_v1_eq_v0_4 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[4] = 1;
    }  /* 0x801422B4: 0x24040001 */
    if (cond_v1_eq_v0_4) {
        goto block_801423A8;  /* taken */
    } else {
        goto block_801422B8;  /* not taken */
    }
block_801422B8:
    goto block_80142B00;
block_801422C0:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 20480;
    }  /* 0x801422C0: 0x24025000 */
    int cond_v1_eq_v0_5 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 20481) ? 1 : 0;  /* 0x801422C8: 0x28625001 */
    if (cond_v1_eq_v0_5) {
        goto block_801424DC;  /* taken */
    } else {
        goto block_801422CC;  /* not taken */
    }
block_801422CC:
    ;
    int cond_v0_eq_zero_5 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 16384;
    }  /* 0x801422D0: 0x24024000 */
    if (cond_v0_eq_zero_5) {
        goto block_801422E4;  /* taken */
    } else {
        goto block_801422D4;  /* not taken */
    }
block_801422D4:
    ;
    int cond_v1_eq_v0_6 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[4] = 1;
    }  /* 0x801422D8: 0x24040001 */
    if (cond_v1_eq_v0_6) {
        goto block_80142484;  /* taken */
    } else {
        goto block_801422DC;  /* not taken */
    }
block_801422DC:
    goto block_80142B00;
block_801422E4:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 24576;
    }  /* 0x801422E4: 0x24026000 */
    int cond_v1_eq_v0_7 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[4] = 1;
    }  /* 0x801422EC: 0x24040001 */
    if (cond_v1_eq_v0_7) {
        goto block_8014256C;  /* taken */
    } else {
        goto block_801422F0;  /* not taken */
    }
block_801422F0:
    goto block_80142B00;
block_801422F8:
    ;
    int cond_v1_eq_v0_8 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x801422FC: 0x0043102A */
    if (cond_v1_eq_v0_8) {
        goto block_8014269C;  /* taken */
    } else {
        goto block_80142300;  /* not taken */
    }
block_80142300:
    ;
    int cond_v0_ne_zero_2 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 0xD000;
    }  /* 0x80142304: 0x3402D000 */
    if (cond_v0_ne_zero_2) {
        goto block_80142324;  /* taken */
    } else {
        goto block_80142308;  /* not taken */
    }
block_80142308:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 0x9000;
    }  /* 0x80142308: 0x34029000 */
    int cond_v1_eq_v0_9 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 0xA000;
    }  /* 0x80142310: 0x3402A000 */
    if (cond_v1_eq_v0_9) {
        goto block_801427FC;  /* taken */
    } else {
        goto block_80142314;  /* not taken */
    }
block_80142314:
    ;
    int cond_v1_eq_v0_10 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    cpu->gpr[16] = cpu->gpr[17] & 0x00FF;  /* 0x80142318: 0x323000FF */
    if (cond_v1_eq_v0_10) {
        goto block_8014263C;  /* taken */
    } else {
        goto block_8014231C;  /* not taken */
    }
block_8014231C:
    goto block_80142B00;
block_80142324:
    ;
    int cond_v1_eq_v0_11 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x80142328: 0x0043102A */
    if (cond_v1_eq_v0_11) {
        goto block_801426F8;  /* taken */
    } else {
        goto block_8014232C;  /* not taken */
    }
block_8014232C:
    ;
    int cond_v0_ne_zero_3 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 0xE000;
    }  /* 0x80142330: 0x3402E000 */
    if (cond_v0_ne_zero_3) {
        goto block_80142348;  /* taken */
    } else {
        goto block_80142334;  /* not taken */
    }
block_80142334:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 0xC000;
    }  /* 0x80142334: 0x3402C000 */
    int cond_v1_eq_v0_12 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    if (cond_v1_eq_v0_12) {
        goto block_801428A0;  /* taken */
    } else {
        goto block_80142340;  /* not taken */
    }
block_80142340:
    goto block_80142B00;
block_80142348:
    ;
    int cond_v1_eq_v0_13 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    cpu->gpr[2] = cpu->gpr[19] & 0x00FF;  /* 0x8014234C: 0x326200FF */
    if (cond_v1_eq_v0_13) {
        goto block_80142734;  /* taken */
    } else {
        goto block_80142350;  /* not taken */
    }
block_80142350:
    goto block_80142B00;
block_80142358:
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[18];  /* move */
    }  /* 0x80142358: 0x02402021 */
    { uint32_t _pgx1 = cpu->gpr[20]; cpu->gpr[5] = (int32_t)cpu->gpr[20] >> 24;
    }  /* 0x8014235C: 0x00142E03 */
    cpu->gpr[5] = cpu->gpr[5] & 0x000F;  /* 0x80142360: 0x30A5000F */
    { uint32_t _pgx1 = cpu->gpr[5]; cpu->gpr[5] = cpu->gpr[5] | 0x0010;
    }  /* 0x80142364: 0x34A50010 */
    { uint32_t _pgx1 = cpu->gpr[21]; cpu->gpr[7] = cpu->gpr[21] << 4;
    }  /* 0x80142368: 0x00153900 */
    { uint32_t _pgx1 = cpu->gpr[22]; cpu->gpr[2] = cpu->gpr[22] << 16;
    }  /* 0x8014236C: 0x00161400 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    }  /* 0x80142370: 0x00021403 */
    { uint32_t _pgxa = cpu->gpr[4] + 32; cpu->gpr[6] = br2_ld32(cpu->gpr[4] + 32);
    }  /* 0x80142374: 0x8C860020 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x80142378: 0x326300FF */
    { uint32_t _pgxa = cpu->gpr[29] + 16; br2_st32(cpu->gpr[29] + 16, cpu->gpr[2]);
    }  /* 0x8014237C: 0xAFA20010 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x80142380: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x80142384: 0x00431021 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x80142388: 0x00021100 */
    { uint32_t _pgxa = cpu->gpr[29] + 20; br2_st32(cpu->gpr[29] + 20, cpu->gpr[23]);
    }  /* 0x8014238C: 0xAFB70014 */
    { uint32_t _pgxa = cpu->gpr[29] + 24; br2_st32(cpu->gpr[29] + 24, cpu->gpr[30]);
    }  /* 0x80142390: 0xAFBE0018 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[2];
    }  /* 0x80142394: 0x00C23021 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] + 52;
    }  /* 0x8014239C: 0x24C60034 */
    cpu->gpr[2] = br2_jal(0x80141814u, 0x801423A0u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801423A0;
block_801423A0:
    goto block_80142B00;
block_801423A8:
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[4];  /* move */
    }  /* 0x801423A8: 0x00802821 */
    { uint32_t _pgx1 = cpu->gpr[0]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[6] = cpu->gpr[0];  /* move */
    }  /* 0x801423AC: 0x00003021 */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[7] = 11;
    }  /* 0x801423B4: 0x2407000B */
    cpu->gpr[2] = br2_jal(0x801AD38Cu, 0x801423B8u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801423B8;
block_801423B8:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[16] = cpu->gpr[2];  /* move */
    }  /* 0x801423B8: 0x00408021 */
    int cond_s0_eq_zero = (cpu->gpr[16] == cpu->gpr[0]);  /* save branch cond before delay slot */
    if (cond_s0_eq_zero) {
        goto block_80142B00;
    } else {
        goto block_801423C4;  /* not taken */
    }
block_801423C4:
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x801423C8: 0x02002021 */
    cpu->gpr[2] = br2_jal(0x801AD284u, 0x801423CCu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801423CC;
block_801423CC:
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x801423CC: 0x02002021 */
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[18];  /* move */
    }  /* 0x801423D0: 0x02402821 */
    cpu->gpr[6] = 0x801F << 16;  /* 0x801F0000 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] + 1888;
    }  /* 0x801423D8: 0x24C60760 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[16] = cpu->gpr[2];  /* move */
    }  /* 0x801423E0: 0x00408021 */
    cpu->gpr[2] = br2_jal(0x801AD224u, 0x801423E4u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801423E4;
block_801423E4:
    { uint32_t _pgx1 = cpu->gpr[20]; cpu->gpr[2] = (int32_t)cpu->gpr[20] >> 24;
    }  /* 0x801423E4: 0x00141603 */
    cpu->gpr[2] = cpu->gpr[2] & 0x000F;  /* 0x801423E8: 0x3042000F */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 25;
    }  /* 0x801423EC: 0x24420019 */
    { uint32_t _pgxa = cpu->gpr[16] + 12; br2_st32(cpu->gpr[16] + 12, cpu->gpr[2]);
    }  /* 0x801423F0: 0xAE02000C */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[4] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x801423F4: 0x8E440020 */
    { uint32_t _pgx1 = cpu->gpr[21]; cpu->gpr[2] = cpu->gpr[21] << 4;
    }  /* 0x801423F8: 0x00151100 */
    { uint32_t _pgxa = cpu->gpr[16] + 20; br2_st32(cpu->gpr[16] + 20, cpu->gpr[2]);
    }  /* 0x801423FC: 0xAE020014 */
    { uint32_t _pgx1 = cpu->gpr[22]; cpu->gpr[2] = cpu->gpr[22] << 16;
    }  /* 0x80142400: 0x00161400 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    }  /* 0x80142404: 0x00021403 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x80142408: 0x326300FF */
    { uint32_t _pgxa = cpu->gpr[16] + 24; br2_st32(cpu->gpr[16] + 24, cpu->gpr[2]);
    }  /* 0x8014240C: 0xAE020018 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x80142410: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x80142414: 0x00431021 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x80142418: 0x00021100 */
    { uint32_t _pgxa = cpu->gpr[16] + 28; br2_st32(cpu->gpr[16] + 28, cpu->gpr[23]);
    }  /* 0x8014241C: 0xAE17001C */
    { uint32_t _pgxa = cpu->gpr[16] + 32; br2_st32(cpu->gpr[16] + 32, cpu->gpr[30]);
    }  /* 0x80142420: 0xAE1E0020 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    }  /* 0x80142424: 0x00822021 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = cpu->gpr[4] + 32;
    }  /* 0x80142428: 0x24840020 */
    { uint32_t _pgxa = cpu->gpr[16] + 16; br2_st32(cpu->gpr[16] + 16, cpu->gpr[4]);
    }  /* 0x80142430: 0xAE040010 */
    goto block_80142B00;
block_80142434:
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[18];  /* move */
    }  /* 0x80142434: 0x02402021 */
    { uint32_t _pgx1 = cpu->gpr[20]; cpu->gpr[5] = (int32_t)cpu->gpr[20] >> 24;
    }  /* 0x80142438: 0x00142E03 */
    cpu->gpr[5] = cpu->gpr[5] & 0x000F;  /* 0x8014243C: 0x30A5000F */
    { uint32_t _pgx1 = cpu->gpr[5]; cpu->gpr[5] = cpu->gpr[5] + 26;
    }  /* 0x80142440: 0x24A5001A */
    { uint32_t _pgx1 = cpu->gpr[21]; cpu->gpr[7] = cpu->gpr[21] << 4;
    }  /* 0x80142444: 0x00153900 */
    { uint32_t _pgx1 = cpu->gpr[22]; cpu->gpr[2] = cpu->gpr[22] << 16;
    }  /* 0x80142448: 0x00161400 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    }  /* 0x8014244C: 0x00021403 */
    { uint32_t _pgxa = cpu->gpr[4] + 32; cpu->gpr[6] = br2_ld32(cpu->gpr[4] + 32);
    }  /* 0x80142450: 0x8C860020 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x80142454: 0x326300FF */
    { uint32_t _pgxa = cpu->gpr[29] + 16; br2_st32(cpu->gpr[29] + 16, cpu->gpr[2]);
    }  /* 0x80142458: 0xAFA20010 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x8014245C: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x80142460: 0x00431021 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x80142464: 0x00021100 */
    { uint32_t _pgxa = cpu->gpr[29] + 20; br2_st32(cpu->gpr[29] + 20, cpu->gpr[23]);
    }  /* 0x80142468: 0xAFB70014 */
    { uint32_t _pgxa = cpu->gpr[29] + 24; br2_st32(cpu->gpr[29] + 24, cpu->gpr[30]);
    }  /* 0x8014246C: 0xAFBE0018 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[2];
    }  /* 0x80142470: 0x00C23021 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] + 52;
    }  /* 0x80142478: 0x24C60034 */
    cpu->gpr[2] = br2_jal(0x80142108u, 0x8014247Cu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8014247C;
block_8014247C:
    goto block_80142B00;
block_80142484:
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[4];  /* move */
    }  /* 0x80142484: 0x00802821 */
    { uint32_t _pgx1 = cpu->gpr[0]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[6] = cpu->gpr[0];  /* move */
    }  /* 0x80142488: 0x00003021 */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[7] = 13;
    }  /* 0x80142490: 0x2407000D */
    cpu->gpr[2] = br2_jal(0x801AD38Cu, 0x80142494u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142494;
block_80142494:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[16] = cpu->gpr[2];  /* move */
    }  /* 0x80142494: 0x00408021 */
    int cond_s0_eq_zero_2 = (cpu->gpr[16] == cpu->gpr[0]);  /* save branch cond before delay slot */
    if (cond_s0_eq_zero_2) {
        goto block_80142B00;
    } else {
        goto block_801424A0;  /* not taken */
    }
block_801424A0:
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x801424A4: 0x02002021 */
    cpu->gpr[2] = br2_jal(0x801AD284u, 0x801424A8u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801424A8;
block_801424A8:
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x801424A8: 0x02002021 */
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[18];  /* move */
    }  /* 0x801424AC: 0x02402821 */
    cpu->gpr[6] = 0x801F << 16;  /* 0x801F0000 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] + 1888;
    }  /* 0x801424B4: 0x24C60760 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[16] = cpu->gpr[2];  /* move */
    }  /* 0x801424BC: 0x00408021 */
    cpu->gpr[2] = br2_jal(0x801AD224u, 0x801424C0u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801424C0;
block_801424C0:
    { uint32_t _pgx1 = cpu->gpr[20]; cpu->gpr[2] = (int32_t)cpu->gpr[20] >> 24;
    }  /* 0x801424C0: 0x00141603 */
    cpu->gpr[2] = cpu->gpr[2] & 0x000F;  /* 0x801424C4: 0x3042000F */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 28;
    }  /* 0x801424C8: 0x2442001C */
    { uint32_t _pgxa = cpu->gpr[16] + 12; br2_st32(cpu->gpr[16] + 12, cpu->gpr[2]);
    }  /* 0x801424CC: 0xAE02000C */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[4] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x801424D0: 0x8E440020 */
    { uint32_t _pgx1 = cpu->gpr[21]; cpu->gpr[2] = cpu->gpr[21] << 4;
    }  /* 0x801424D8: 0x00151100 */
    goto block_80142534;  /* j */
block_801424DC:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[4] = 1;
    }  /* 0x801424DC: 0x24040001 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[4];  /* move */
    }  /* 0x801424E0: 0x00802821 */
    { uint32_t _pgx1 = cpu->gpr[0]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[6] = cpu->gpr[0];  /* move */
    }  /* 0x801424E4: 0x00003021 */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[7] = 14;
    }  /* 0x801424EC: 0x2407000E */
    cpu->gpr[2] = br2_jal(0x801AD38Cu, 0x801424F0u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801424F0;
block_801424F0:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[16] = cpu->gpr[2];  /* move */
    }  /* 0x801424F0: 0x00408021 */
    int cond_s0_eq_zero_3 = (cpu->gpr[16] == cpu->gpr[0]);  /* save branch cond before delay slot */
    if (cond_s0_eq_zero_3) {
        goto block_80142B00;
    } else {
        goto block_801424FC;  /* not taken */
    }
block_801424FC:
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x80142500: 0x02002021 */
    cpu->gpr[2] = br2_jal(0x801AD284u, 0x80142504u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142504;
block_80142504:
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x80142504: 0x02002021 */
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[18];  /* move */
    }  /* 0x80142508: 0x02402821 */
    cpu->gpr[6] = 0x801F << 16;  /* 0x801F0000 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] + 1888;
    }  /* 0x80142510: 0x24C60760 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[16] = cpu->gpr[2];  /* move */
    }  /* 0x80142518: 0x00408021 */
    cpu->gpr[2] = br2_jal(0x801AD224u, 0x8014251Cu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8014251C;
block_8014251C:
    { uint32_t _pgx1 = cpu->gpr[20]; cpu->gpr[2] = (int32_t)cpu->gpr[20] >> 24;
    }  /* 0x8014251C: 0x00141603 */
    cpu->gpr[2] = cpu->gpr[2] & 0x000F;  /* 0x80142520: 0x3042000F */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 29;
    }  /* 0x80142524: 0x2442001D */
    { uint32_t _pgxa = cpu->gpr[16] + 12; br2_st32(cpu->gpr[16] + 12, cpu->gpr[2]);
    }  /* 0x80142528: 0xAE02000C */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[4] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x8014252C: 0x8E440020 */
    { uint32_t _pgx1 = cpu->gpr[21]; cpu->gpr[2] = cpu->gpr[21] << 6;
    }  /* 0x80142530: 0x00151180 */
    /* fall through to block_80142534 */
block_80142534:
    { uint32_t _pgxa = cpu->gpr[16] + 20; br2_st32(cpu->gpr[16] + 20, cpu->gpr[2]);
    }  /* 0x80142534: 0xAE020014 */
    { uint32_t _pgx1 = cpu->gpr[22]; cpu->gpr[2] = cpu->gpr[22] << 16;
    }  /* 0x80142538: 0x00161400 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    }  /* 0x8014253C: 0x00021403 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x80142540: 0x326300FF */
    { uint32_t _pgxa = cpu->gpr[16] + 24; br2_st32(cpu->gpr[16] + 24, cpu->gpr[2]);
    }  /* 0x80142544: 0xAE020018 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x80142548: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x8014254C: 0x00431021 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x80142550: 0x00021100 */
    { uint32_t _pgxa = cpu->gpr[16] + 28; br2_st32(cpu->gpr[16] + 28, cpu->gpr[23]);
    }  /* 0x80142554: 0xAE17001C */
    { uint32_t _pgxa = cpu->gpr[16] + 32; br2_st32(cpu->gpr[16] + 32, cpu->gpr[30]);
    }  /* 0x80142558: 0xAE1E0020 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    }  /* 0x8014255C: 0x00822021 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = cpu->gpr[4] + 52;
    }  /* 0x80142560: 0x24840034 */
    { uint32_t _pgxa = cpu->gpr[16] + 16; br2_st32(cpu->gpr[16] + 16, cpu->gpr[4]);
    }  /* 0x80142568: 0xAE040010 */
    goto block_80142B00;
block_8014256C:
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[4];  /* move */
    }  /* 0x8014256C: 0x00802821 */
    { uint32_t _pgx1 = cpu->gpr[0]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[6] = cpu->gpr[0];  /* move */
    }  /* 0x80142570: 0x00003021 */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[7] = 10;
    }  /* 0x80142578: 0x2407000A */
    cpu->gpr[2] = br2_jal(0x801AD38Cu, 0x8014257Cu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8014257C;
block_8014257C:
    ;
    int cond_v0_eq_zero_6 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    if (cond_v0_eq_zero_6) {
        goto block_80142B00;
    } else {
        goto block_80142584;  /* not taken */
    }
block_80142584:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[2];  /* move */
    }  /* 0x80142588: 0x00402021 */
    cpu->gpr[2] = br2_jal(0x801AD284u, 0x8014258Cu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8014258C;
block_8014258C:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[2];  /* move */
    }  /* 0x8014258C: 0x00402821 */
    { uint32_t _pgx1 = cpu->gpr[17]; cpu->gpr[3] = cpu->gpr[17] << 16;
    }  /* 0x80142590: 0x00111C00 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = (int32_t)cpu->gpr[3] >> 12;
    }  /* 0x80142594: 0x00031303 */
    { uint32_t _pgxa = cpu->gpr[5] + 4; br2_st32(cpu->gpr[5] + 4, cpu->gpr[18]);
    }  /* 0x80142598: 0xACB20004 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[4] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x8014259C: 0x8E440020 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0F00;  /* 0x801425A0: 0x30420F00 */
    { uint32_t _pgxa = cpu->gpr[5] + 12; br2_st32(cpu->gpr[5] + 12, cpu->gpr[2]);
    }  /* 0x801425A4: 0xACA2000C */
    cpu->gpr[2] = cpu->gpr[17] & 0x000F;  /* 0x801425A8: 0x3222000F */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 8;
    }  /* 0x801425AC: 0x00021200 */
    { uint32_t _pgxa = cpu->gpr[5] + 16; br2_st32(cpu->gpr[5] + 16, cpu->gpr[2]);
    }  /* 0x801425B0: 0xACA20010 */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[2] = cpu->gpr[16] << 3;
    }  /* 0x801425B4: 0x001010C0 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0780;  /* 0x801425B8: 0x30420780 */
    { uint32_t _pgxa = cpu->gpr[5] + 24; br2_st32(cpu->gpr[5] + 24, cpu->gpr[2]);
    }  /* 0x801425BC: 0xACA20018 */
    { uint32_t _pgx1 = cpu->gpr[22]; cpu->gpr[2] = cpu->gpr[22] << 16;
    }  /* 0x801425C0: 0x00161400 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;
    }  /* 0x801425C4: 0x00021403 */
    { uint32_t _pgxa = cpu->gpr[5] + 28; br2_st32(cpu->gpr[5] + 28, cpu->gpr[2]);
    }  /* 0x801425C8: 0xACA2001C */
    cpu->gpr[2] = cpu->gpr[23] & 0x000F;  /* 0x801425CC: 0x32E2000F */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 24;
    }  /* 0x801425D0: 0x00031E03 */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x801425D4: 0x3063000F */
    { uint32_t _pgxa = cpu->gpr[5] + 36; br2_st32(cpu->gpr[5] + 36, cpu->gpr[3]);
    }  /* 0x801425D8: 0xACA30024 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x801425DC: 0x326300FF */
    { uint32_t _pgxa = cpu->gpr[5] + 32; br2_st32(cpu->gpr[5] + 32, cpu->gpr[2]);
    }  /* 0x801425E0: 0xACA20020 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x801425E4: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x801425E8: 0x00431021 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x801425EC: 0x00021100 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    }  /* 0x801425F0: 0x00822021 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = cpu->gpr[4] + 52;
    }  /* 0x801425F4: 0x24840034 */
    { uint32_t _pgxa = cpu->gpr[5] + 8; br2_st32(cpu->gpr[5] + 8, cpu->gpr[4]);
    }  /* 0x801425F8: 0xACA40008 */
    { uint32_t _pgxa = cpu->gpr[18] + 56; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 56);
    }  /* 0x801425FC: 0x8E420038 */
    { uint32_t _pgxa = cpu->gpr[2] + 256; cpu->gpr[2] = br2_ld8(cpu->gpr[2] + 256);
    }  /* 0x80142604: 0x90420100 */
    int cond_v0_eq_zero_7 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[3] = cpu->gpr[16] & 0x0F00;  /* 0x80142610: 0x32030F00 */
    if (cond_v0_eq_zero_7) {
        goto block_80142634;  /* taken */
    } else {
        goto block_80142614;  /* not taken */
    }
block_80142614:
    { uint32_t _pgxa = cpu->gpr[29] + 96; cpu->gpr[10] = br2_ld32(cpu->gpr[29] + 96);
    }  /* 0x80142614: 0x8FAA0060 */
    { uint32_t _pgxa = cpu->gpr[10]; cpu->gpr[2] = (int32_t)(int16_t)br2_ld16(cpu->gpr[10]);
    }  /* 0x8014261C: 0x85420000 */
    cpu->gpr[2] = cpu->gpr[2] & 0x8000;  /* 0x80142624: 0x30428000 */
    int cond_v0_eq_zero_8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 4096;
    }  /* 0x8014262C: 0x24021000 */
    if (cond_v0_eq_zero_8) {
        goto block_80142634;  /* taken */
    } else {
        goto block_80142630;  /* not taken */
    }
block_80142630:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[2] - cpu->gpr[3];
    }  /* 0x80142630: 0x00431823 */
    /* fall through to block_80142634 */
block_80142634:
    { uint32_t _pgxa = cpu->gpr[5] + 20; br2_st32(cpu->gpr[5] + 20, cpu->gpr[3]);
    }  /* 0x80142638: 0xACA30014 */
    goto block_80142B00;
block_8014263C:
    cpu->gpr[5] = 0x8000 << 16;  /* 0x80000000 */
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[5]; cpu->gpr[5] = cpu->gpr[16] | cpu->gpr[5];
    }  /* 0x80142640: 0x02052825 */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[6] = 127;
    }  /* 0x80142644: 0x2406007F */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[7] = cpu->gpr[6];  /* move */
    }  /* 0x80142648: 0x00C03821 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x8014264C: 0x326300FF */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x80142650: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x80142654: 0x00431021 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[4] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x80142658: 0x8E440020 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x8014265C: 0x00021100 */
    { uint32_t _pgxa = cpu->gpr[29] + 16; br2_st32(cpu->gpr[29] + 16, cpu->gpr[0]);
    }  /* 0x80142660: 0xAFA00010 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    }  /* 0x80142664: 0x00822021 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = cpu->gpr[4] + 52;
    }  /* 0x8014266C: 0x24840034 */
    cpu->gpr[2] = br2_jal(0x80139FD0u, 0x80142670u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142670;
block_80142670:
    ;
    int cond_s0_ne_zero = (cpu->gpr[16] != cpu->gpr[0]);  /* save branch cond before delay slot */
    if (cond_s0_ne_zero) {
        goto block_80142B00;
    } else {
        goto block_80142678;  /* not taken */
    }
block_80142678:
    { uint32_t _pgxa = cpu->gpr[18] + 88; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 88);
    }  /* 0x80142678: 0x8E420058 */
    cpu->gpr[2] = cpu->gpr[2] & 0x8000;  /* 0x80142680: 0x30428000 */
    int cond_v0_eq_zero_9 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    if (cond_v0_eq_zero_9) {
        goto block_801427D4;  /* taken */
    } else {
        goto block_8014268C;  /* not taken */
    }
block_8014268C:
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[3] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x8014268C: 0x92430002 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 2368;
    }  /* 0x80142690: 0x24420940 */
    cpu->gpr[3] = cpu->gpr[3] ^ 0x0001;  /* 0x80142698: 0x38630001 */
    goto block_801427E0;  /* j */
block_8014269C:
    { uint32_t _pgx1 = cpu->gpr[0]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[0];  /* move */
    }  /* 0x8014269C: 0x00002821 */
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x801426A0: 0x322200FF */
    cpu->gpr[6] = 0x8000 << 16;  /* 0x80000000 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[2] | cpu->gpr[6];
    }  /* 0x801426A8: 0x00463025 */
    cpu->gpr[7] = cpu->gpr[16] & 0x00FF;  /* 0x801426AC: 0x320700FF */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[3] = cpu->gpr[16] << 16;
    }  /* 0x801426B0: 0x00101C00 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 24;
    }  /* 0x801426B4: 0x00031E03 */
    cpu->gpr[4] = cpu->gpr[19] & 0x00FF;  /* 0x801426B8: 0x326400FF */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[2] = cpu->gpr[4] << 2;
    }  /* 0x801426BC: 0x00041080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[4]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];
    }  /* 0x801426C0: 0x00441021 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x801426C4: 0x00021100 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[4] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x801426C8: 0x8E440020 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 2;
    }  /* 0x801426CC: 0x00031880 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    }  /* 0x801426D0: 0x00822021 */
    cpu->gpr[2] = 0x801B << 16;  /* 0x801B0000 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 7480;
    }  /* 0x801426D8: 0x24421D38 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x801426DC: 0x00621821 */
    { uint32_t _pgxa = cpu->gpr[3]; cpu->gpr[2] = br2_ld32(cpu->gpr[3]);
    }  /* 0x801426E0: 0x8C620000 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = cpu->gpr[4] + 52;
    }  /* 0x801426E4: 0x24840034 */
    { uint32_t _pgxa = cpu->gpr[29] + 16; br2_st32(cpu->gpr[29] + 16, cpu->gpr[2]);
    }  /* 0x801426EC: 0xAFA20010 */
    cpu->gpr[2] = br2_jal(0x8013B088u, 0x801426F0u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801426F0;
block_801426F0:
    goto block_80142B00;
block_801426F8:
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x801426F8: 0x322200FF */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 6;
    }  /* 0x801426FC: 0x00021180 */
    { uint32_t _pgx1 = cpu->gpr[17]; cpu->gpr[3] = (uint32_t)cpu->gpr[17] >> 8;
    }  /* 0x80142700: 0x00111A02 */
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x80142704: 0x3063000F */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 17;
    }  /* 0x80142708: 0x00031C40 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x8014270C: 0x00431021 */
    { uint32_t _pgxa = cpu->gpr[29] + 16; br2_st32(cpu->gpr[29] + 16, cpu->gpr[2]);
    }  /* 0x80142710: 0xAFA20010 */
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[18];  /* move */
    }  /* 0x80142714: 0x02402021 */
    cpu->gpr[5] = cpu->gpr[19] & 0x00FF;  /* 0x80142718: 0x326500FF */
    cpu->gpr[6] = cpu->gpr[16] & 0x00FF;  /* 0x8014271C: 0x320600FF */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[7] = cpu->gpr[16] << 16;
    }  /* 0x80142720: 0x00103C00 */
    { uint32_t _pgx1 = cpu->gpr[7]; cpu->gpr[7] = (uint32_t)cpu->gpr[7] >> 24;
    }  /* 0x80142728: 0x00073E02 */
    cpu->gpr[2] = br2_jal(0x801401C8u, 0x8014272Cu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8014272C;
block_8014272C:
    goto block_80142B00;
block_80142734:
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[2] << 2;
    }  /* 0x80142734: 0x00021880 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x80142738: 0x00621821 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x8014273C: 0x8E420020 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 4;
    }  /* 0x80142740: 0x00031900 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x80142744: 0x00621021 */
    { uint32_t _pgxa = cpu->gpr[2] + 52; cpu->gpr[2] = br2_ld16(cpu->gpr[2] + 52);
    }  /* 0x80142748: 0x94420034 */
    { uint32_t _pgxa = cpu->gpr[29] + 32; br2_st16(cpu->gpr[29] + 32, (uint16_t)cpu->gpr[2]);
    }  /* 0x80142750: 0xA7A20020 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x80142754: 0x8E420020 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x8014275C: 0x00621021 */
    { uint32_t _pgxa = cpu->gpr[2] + 56; cpu->gpr[2] = br2_ld16(cpu->gpr[2] + 56);
    }  /* 0x80142760: 0x94420038 */
    { uint32_t _pgx1 = cpu->gpr[29]; cpu->gpr[4] = cpu->gpr[29] + 32;
    }  /* 0x80142764: 0x27A40020 */
    { uint32_t _pgxa = cpu->gpr[29] + 34; br2_st16(cpu->gpr[29] + 34, (uint16_t)cpu->gpr[2]);
    }  /* 0x80142768: 0xA7A20022 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x8014276C: 0x8E420020 */
    cpu->gpr[6] = cpu->gpr[16] & 0x000F;  /* 0x80142770: 0x3206000F */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x80142774: 0x00621821 */
    { uint32_t _pgxa = cpu->gpr[3] + 60; cpu->gpr[2] = br2_ld16(cpu->gpr[3] + 60);
    }  /* 0x80142778: 0x9462003C */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] << 8;
    }  /* 0x8014277C: 0x00063200 */
    { uint32_t _pgxa = cpu->gpr[29] + 36; br2_st16(cpu->gpr[29] + 36, (uint16_t)cpu->gpr[2]);
    }  /* 0x80142780: 0xA7A20024 */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[2] = cpu->gpr[16] << 16;
    }  /* 0x80142784: 0x00101400 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[7] = (int32_t)cpu->gpr[2] >> 24;
    }  /* 0x80142788: 0x00023E03 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 12;
    }  /* 0x8014278C: 0x00021303 */
    cpu->gpr[2] = cpu->gpr[2] & 0x0F00;  /* 0x80142790: 0x30420F00 */
    { uint32_t _pgxa = cpu->gpr[18] + 78; cpu->gpr[5] = (int32_t)(int16_t)br2_ld16(cpu->gpr[18] + 78);
    }  /* 0x80142794: 0x8645004E */
    { uint32_t _pgxa = cpu->gpr[18] + 116; cpu->gpr[3] = (int32_t)(int16_t)br2_ld16(cpu->gpr[18] + 116);
    }  /* 0x80142798: 0x86430074 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + -1024;
    }  /* 0x8014279C: 0x2442FC00 */
    { uint32_t _pgxa = cpu->gpr[29] + 16; br2_st32(cpu->gpr[29] + 16, cpu->gpr[18]);
    }  /* 0x801427A0: 0xAFB20010 */
    { uint32_t _pgx1 = cpu->gpr[5]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[3];
    }  /* 0x801427A4: 0x00A32821 */
    { uint32_t _pgx1 = cpu->gpr[5]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];
    }  /* 0x801427AC: 0x00A22821 */
    cpu->gpr[2] = br2_jal(0x8013EE40u, 0x801427B0u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801427B0;
block_801427B0:
    { uint32_t _pgxa = cpu->gpr[18] + 88; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 88);
    }  /* 0x801427B0: 0x8E420058 */
    cpu->gpr[2] = cpu->gpr[2] & 0x8000;  /* 0x801427B8: 0x30428000 */
    int cond_v0_eq_zero_10 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    if (cond_v0_eq_zero_10) {
        goto block_801427D4;  /* taken */
    } else {
        goto block_801427C4;  /* not taken */
    }
block_801427C4:
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[3] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x801427C4: 0x92430002 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 2368;
    }  /* 0x801427C8: 0x24420940 */
    cpu->gpr[3] = cpu->gpr[3] ^ 0x0001;  /* 0x801427D0: 0x38630001 */
    goto block_801427E0;  /* j */
block_801427D4:
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[3] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x801427D8: 0x92430002 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 2368;
    }  /* 0x801427DC: 0x24420940 */
    /* fall through to block_801427E0 */
block_801427E0:
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 2;
    }  /* 0x801427E0: 0x00031880 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x801427E4: 0x00621821 */
    { uint32_t _pgxa = cpu->gpr[3]; cpu->gpr[2] = br2_ld32(cpu->gpr[3]);
    }  /* 0x801427E8: 0x8C620000 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 1;
    }  /* 0x801427F0: 0x24420001 */
    { uint32_t _pgxa = cpu->gpr[3]; br2_st32(cpu->gpr[3], cpu->gpr[2]);
    }  /* 0x801427F8: 0xAC620000 */
    goto block_80142B00;
block_801427FC:
    { uint32_t _pgx1 = cpu->gpr[29]; cpu->gpr[4] = cpu->gpr[29] + 40;
    }  /* 0x801427FC: 0x27A40028 */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[6] = cpu->gpr[16] << 16;
    }  /* 0x80142800: 0x00103400 */
    cpu->gpr[2] = cpu->gpr[19] & 0x00FF;  /* 0x80142804: 0x326200FF */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[2] << 2;
    }  /* 0x80142808: 0x00021880 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x8014280C: 0x00621821 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 4;
    }  /* 0x80142810: 0x00031900 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[5] = (uint32_t)cpu->gpr[6] >> 24;
    }  /* 0x80142814: 0x00062E02 */
    cpu->gpr[7] = cpu->gpr[16] & 0x000F;  /* 0x80142818: 0x3207000F */
    { uint32_t _pgx1 = cpu->gpr[7]; cpu->gpr[7] = cpu->gpr[7] << 8;
    }  /* 0x8014281C: 0x00073A00 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x80142820: 0x8E420020 */
    { uint32_t _pgxa = cpu->gpr[18] + 78; cpu->gpr[8] = (int32_t)(int16_t)br2_ld16(cpu->gpr[18] + 78);
    }  /* 0x80142824: 0x8648004E */
    { uint32_t _pgxa = cpu->gpr[18] + 116; cpu->gpr[9] = (int32_t)(int16_t)br2_ld16(cpu->gpr[18] + 116);
    }  /* 0x80142828: 0x86490074 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x8014282C: 0x00621021 */
    { uint32_t _pgxa = cpu->gpr[2] + 52; cpu->gpr[2] = br2_ld16(cpu->gpr[2] + 52);
    }  /* 0x80142830: 0x94420034 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = (int32_t)cpu->gpr[6] >> 12;
    }  /* 0x80142834: 0x00063303 */
    { uint32_t _pgxa = cpu->gpr[29] + 40; br2_st16(cpu->gpr[29] + 40, (uint16_t)cpu->gpr[2]);
    }  /* 0x80142838: 0xA7A20028 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x8014283C: 0x8E420020 */
    cpu->gpr[6] = cpu->gpr[6] & 0x0F00;  /* 0x80142840: 0x30C60F00 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x80142844: 0x00621021 */
    { uint32_t _pgxa = cpu->gpr[2] + 56; cpu->gpr[2] = br2_ld16(cpu->gpr[2] + 56);
    }  /* 0x80142848: 0x94420038 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[8]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[8];
    }  /* 0x8014284C: 0x00C83021 */
    { uint32_t _pgxa = cpu->gpr[29] + 42; br2_st16(cpu->gpr[29] + 42, (uint16_t)cpu->gpr[2]);
    }  /* 0x80142850: 0xA7A2002A */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[2] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x80142854: 0x8E420020 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[9]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[9];
    }  /* 0x80142858: 0x00C93021 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x8014285C: 0x00621821 */
    { uint32_t _pgxa = cpu->gpr[3] + 60; cpu->gpr[2] = br2_ld16(cpu->gpr[3] + 60);
    }  /* 0x80142860: 0x9462003C */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] + -1024;
    }  /* 0x80142864: 0x24C6FC00 */
    { uint32_t _pgxa = cpu->gpr[29] + 44; br2_st16(cpu->gpr[29] + 44, (uint16_t)cpu->gpr[2]);
    }  /* 0x80142868: 0xA7A2002C */
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x8014286C: 0x322200FF */
    { uint32_t _pgxa = cpu->gpr[29] + 16; br2_st32(cpu->gpr[29] + 16, cpu->gpr[2]);
    }  /* 0x80142870: 0xAFA20010 */
    { uint32_t _pgxa = cpu->gpr[29] + 20; br2_st32(cpu->gpr[29] + 20, cpu->gpr[0]);
    }  /* 0x80142878: 0xAFA00014 */
    cpu->gpr[2] = br2_jal(0x8013D6B0u, 0x8014287Cu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_8014287C;
block_8014287C:
    goto block_80142B00;
block_80142884:
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[18];  /* move */
    }  /* 0x80142884: 0x02402021 */
    cpu->gpr[5] = cpu->gpr[19] & 0x00FF;  /* 0x80142888: 0x326500FF */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[6] = cpu->gpr[16] << 16;
    }  /* 0x8014288C: 0x00103400 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = (int32_t)cpu->gpr[6] >> 16;
    }  /* 0x80142894: 0x00063403 */
    cpu->gpr[2] = br2_jal(0x8013F714u, 0x80142898u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142898;
block_80142898:
    goto block_80142B00;
block_801428A0:
    { uint32_t _pgxa = cpu->gpr[18] + 3; cpu->gpr[2] = br2_ld8(cpu->gpr[18] + 3);
    }  /* 0x801428A0: 0x92420003 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[2] + -1;
    }  /* 0x801428A8: 0x2443FFFF */
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)9) ? 1 : 0;  /* 0x801428AC: 0x2C620009 */
    int cond_v0_eq_zero_11 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[2] = 0x8012 << 16;  /* 0x80120000 */
    if (cond_v0_eq_zero_11) {
        goto block_80142B00;
    } else {
        goto block_801428B8;  /* not taken */
    }
block_801428B8:
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 552;
    }  /* 0x801428B8: 0x24420228 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 2;
    }  /* 0x801428BC: 0x00031880 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x801428C0: 0x00621821 */
    { uint32_t _pgxa = cpu->gpr[3]; cpu->gpr[2] = br2_ld32(cpu->gpr[3]);
    }  /* 0x801428C4: 0x8C620000 */
    uint32_t jr_target = cpu->gpr[2];  /* latch indirect target before delay slot */
    switch (jr_target) {
    case 0x80142AC4u: goto block_80142AC4;
    case 0x80142B00u: goto block_80142B00;
    case 0x801429F4u: goto block_801429F4;
    case 0x80142A44u: goto block_80142A44;
    case 0x801428D4u: goto block_801428D4;
    case 0x80142954u: goto block_80142954;
    default:
        return call_guest4(jr_target, cpu->gpr[4], cpu->gpr[5], cpu->gpr[6], cpu->gpr[7]);
    }
block_801428D4:
    cpu->gpr[2] = 0x801F << 16;  /* 0x801F0000 */
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[3] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x801428D8: 0x92430002 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] + 1872;
    }  /* 0x801428DC: 0x24420750 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 2;
    }  /* 0x801428E0: 0x00031880 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];
    }  /* 0x801428E4: 0x00621821 */
    { uint32_t _pgxa = cpu->gpr[3]; cpu->gpr[4] = br2_ld32(cpu->gpr[3]);
    }  /* 0x801428E8: 0x8C640000 */
    cpu->gpr[2] = br2_jal(0x801AD284u, 0x801428F4u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801428F4;
block_801428F4:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[2];  /* move */
    }  /* 0x801428F4: 0x00402021 */
    cpu->gpr[3] = cpu->gpr[17] & 0x00FF;  /* 0x801428F8: 0x322300FF */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 254;
    }  /* 0x801428FC: 0x240200FE */
    int cond_v1_eq_v0_14 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    cpu->gpr[2] = ((int32_t)cpu->gpr[3] < 255) ? 1 : 0;  /* 0x80142904: 0x286200FF */
    if (cond_v1_eq_v0_14) {
        goto block_8014294C;  /* taken */
    } else {
        goto block_80142908;  /* not taken */
    }
block_80142908:
    ;
    int cond_v0_eq_zero_12 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 255;
    }  /* 0x8014290C: 0x240200FF */
    if (cond_v0_eq_zero_12) {
        goto block_80142920;  /* taken */
    } else {
        goto block_80142910;  /* not taken */
    }
block_80142910:
    ;
    int cond_v1_eq_zero = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[2] = cpu->gpr[16] << 16;
    }  /* 0x80142914: 0x00101400 */
    if (cond_v1_eq_zero) {
        goto block_80142930;  /* taken */
    } else {
        goto block_80142918;  /* not taken */
    }
block_80142918:
    goto block_80142B00;
block_80142920:
    ;
    int cond_v1_eq_v0_15 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 1;
    }  /* 0x80142924: 0x24020001 */
    if (cond_v1_eq_v0_15) {
        goto block_80142944;  /* taken */
    } else {
        goto block_80142928;  /* not taken */
    }
block_80142928:
    goto block_80142B00;
block_80142930:
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;
    }  /* 0x80142930: 0x00021603 */
    { uint32_t _pgxa = cpu->gpr[4] + 12; br2_st32(cpu->gpr[4] + 12, cpu->gpr[2]);
    }  /* 0x80142934: 0xAC82000C */
    cpu->gpr[2] = cpu->gpr[16] & 0x00FF;  /* 0x80142938: 0x320200FF */
    { uint32_t _pgxa = cpu->gpr[4] + 16; br2_st32(cpu->gpr[4] + 16, cpu->gpr[2]);
    }  /* 0x80142940: 0xAC820010 */
    goto block_80142B00;
block_80142944:
    { uint32_t _pgxa = cpu->gpr[4] + 20; br2_st32(cpu->gpr[4] + 20, cpu->gpr[2]);
    }  /* 0x80142948: 0xAC820014 */
    goto block_80142B00;
block_8014294C:
    { uint32_t _pgxa = cpu->gpr[4] + 20; br2_st32(cpu->gpr[4] + 20, cpu->gpr[0]);
    }  /* 0x80142950: 0xAC800014 */
    goto block_80142B00;
block_80142954:
    cpu->gpr[3] = cpu->gpr[17] & 0x00FF;  /* 0x80142954: 0x322300FF */
    int cond_v1_eq_zero_2 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[2] = 1;
    }  /* 0x8014295C: 0x24020001 */
    if (cond_v1_eq_zero_2) {
        goto block_80142970;  /* taken */
    } else {
        goto block_80142960;  /* not taken */
    }
block_80142960:
    ;
    int cond_v1_eq_v0_16 = (cpu->gpr[3] == cpu->gpr[2]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[18];  /* move */
    }  /* 0x80142964: 0x02402021 */
    if (cond_v1_eq_v0_16) {
        goto block_801429C4;  /* taken */
    } else {
        goto block_80142968;  /* not taken */
    }
block_80142968:
    goto block_80142B00;
block_80142970:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[4] = 1;
    }  /* 0x80142970: 0x24040001 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[4];  /* move */
    }  /* 0x80142974: 0x00802821 */
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[6] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x80142978: 0x92460002 */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[7] = 8;
    }  /* 0x8014297C: 0x24070008 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[4]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[4];
    }  /* 0x80142984: 0x00C43021 */
    cpu->gpr[2] = br2_jal(0x801AD38Cu, 0x80142988u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142988;
block_80142988:
    ;
    int cond_v0_eq_zero_13 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    if (cond_v0_eq_zero_13) {
        goto block_80142B00;
    } else {
        goto block_80142990;  /* not taken */
    }
block_80142990:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[2];  /* move */
    }  /* 0x80142994: 0x00402021 */
    cpu->gpr[2] = br2_jal(0x801AD284u, 0x80142998u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142998;
block_80142998:
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[3] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x80142998: 0x92430002 */
    cpu->gpr[4] = cpu->gpr[19] & 0x00FF;  /* 0x8014299C: 0x326400FF */
    { uint32_t _pgxa = cpu->gpr[2] + 4; br2_st32(cpu->gpr[2] + 4, cpu->gpr[3]);
    }  /* 0x801429A0: 0xAC430004 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[3] = cpu->gpr[4] << 2;
    }  /* 0x801429A4: 0x00041880 */
    { uint32_t _pgx1 = cpu->gpr[3]; uint32_t _pgx2 = cpu->gpr[4]; cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];
    }  /* 0x801429A8: 0x00641821 */
    { uint32_t _pgxa = cpu->gpr[18] + 32; cpu->gpr[4] = br2_ld32(cpu->gpr[18] + 32);
    }  /* 0x801429AC: 0x8E440020 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] << 4;
    }  /* 0x801429B0: 0x00031900 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[3];
    }  /* 0x801429B4: 0x00832021 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = cpu->gpr[4] + 52;
    }  /* 0x801429B8: 0x24840034 */
    { uint32_t _pgxa = cpu->gpr[2] + 8; br2_st32(cpu->gpr[2] + 8, cpu->gpr[4]);
    }  /* 0x801429C0: 0xAC440008 */
    goto block_80142B00;
block_801429C4:
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[6] = cpu->gpr[16] << 16;
    }  /* 0x801429C4: 0x00103400 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = (int32_t)cpu->gpr[6] >> 16;
    }  /* 0x801429C8: 0x00063403 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x801429CC: 0x326300FF */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x801429D0: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x801429D4: 0x00431021 */
    { uint32_t _pgxa = cpu->gpr[4] + 32; cpu->gpr[5] = br2_ld32(cpu->gpr[4] + 32);
    }  /* 0x801429D8: 0x8C850020 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x801429DC: 0x00021100 */
    { uint32_t _pgx1 = cpu->gpr[5]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];
    }  /* 0x801429E0: 0x00A22821 */
    { uint32_t _pgx1 = cpu->gpr[5]; cpu->gpr[5] = cpu->gpr[5] + 52;
    }  /* 0x801429E8: 0x24A50034 */
    cpu->gpr[2] = br2_jal(0x8013E0F0u, 0x801429ECu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_801429EC;
block_801429EC:
    goto block_80142B00;
block_801429F4:
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x801429F4: 0x322200FF */
    int cond_v0_ne_zero_4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[3] = 0x1F80 << 16;  /* 0x1F800000 */
    if (cond_v0_ne_zero_4) {
        goto block_80142B00;
    } else {
        goto block_80142A00;  /* not taken */
    }
block_80142A00:
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] | 0x00E8;
    }  /* 0x80142A00: 0x346300E8 */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[4] = cpu->gpr[16] << 16;
    }  /* 0x80142A04: 0x00102400 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = (int32_t)cpu->gpr[4] >> 27;
    }  /* 0x80142A08: 0x000426C3 */
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[2] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x80142A0C: 0x92420002 */
    cpu->gpr[5] = cpu->gpr[16] & 0x07FF;  /* 0x80142A10: 0x320507FF */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[6] = cpu->gpr[2] << 5;
    }  /* 0x80142A14: 0x00023140 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[2];
    }  /* 0x80142A18: 0x00C23021 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] << 3;
    }  /* 0x80142A1C: 0x000630C0 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[3];
    }  /* 0x80142A20: 0x00C33021 */
    cpu->gpr[2] = cpu->gpr[2] ^ 0x0001;  /* 0x80142A24: 0x38420001 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[7] = cpu->gpr[2] << 5;
    }  /* 0x80142A28: 0x00023940 */
    { uint32_t _pgx1 = cpu->gpr[7]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[7] = cpu->gpr[7] + cpu->gpr[2];
    }  /* 0x80142A2C: 0x00E23821 */
    { uint32_t _pgx1 = cpu->gpr[7]; cpu->gpr[7] = cpu->gpr[7] << 3;
    }  /* 0x80142A30: 0x000738C0 */
    { uint32_t _pgx1 = cpu->gpr[7]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[7] = cpu->gpr[7] + cpu->gpr[3];
    }  /* 0x80142A38: 0x00E33821 */
    cpu->gpr[2] = br2_jal(0x8013C9CCu, 0x80142A3Cu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142A3C;
block_80142A3C:
    goto block_80142B00;
block_80142A40:
block_80142A44:
    cpu->gpr[17] = cpu->gpr[17] & 0x00FF;  /* 0x80142A44: 0x323100FF */
    cpu->gpr[2] = ((int32_t)cpu->gpr[17] < 2) ? 1 : 0;  /* 0x80142A48: 0x2A220002 */
    int cond_v0_eq_zero_14 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    if (cond_v0_eq_zero_14) {
        goto block_80142B00;  /* taken */
    } else {
        goto block_80142A54;  /* not taken */
    }
block_80142A54:
    ;
    int cond_s1_lt_0 = ((int32_t)cpu->gpr[17] < 0);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[4] = 1;
    }  /* 0x80142A58: 0x24040001 */
    if (cond_s1_lt_0) {
        goto block_80142B00;  /* taken */
    } else {
        goto block_80142A5C;  /* not taken */
    }
block_80142A5C:
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[4];  /* move */
    }  /* 0x80142A5C: 0x00802821 */
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[6] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x80142A60: 0x92460002 */
    { uint32_t _pgx1 = cpu->gpr[16]; cpu->gpr[7] = cpu->gpr[16] << 16;
    }  /* 0x80142A64: 0x00103C00 */
    { uint32_t _pgx1 = cpu->gpr[7]; cpu->gpr[7] = (int32_t)cpu->gpr[7] >> 16;
    }  /* 0x80142A68: 0x00073C03 */
    { uint32_t _pgx1 = cpu->gpr[6]; uint32_t _pgx2 = cpu->gpr[4]; cpu->gpr[6] = cpu->gpr[6] + cpu->gpr[4];
    }  /* 0x80142A70: 0x00C43021 */
    cpu->gpr[2] = br2_jal(0x801AD38Cu, 0x80142A74u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142A74;
block_80142A74:
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[16] = cpu->gpr[2];  /* move */
    }  /* 0x80142A74: 0x00408021 */
    int cond_s0_eq_zero_4 = (cpu->gpr[16] == cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x80142A7C: 0x02002021 */
    if (cond_s0_eq_zero_4) {
        goto block_80142B00;  /* taken */
    } else {
        goto block_80142A80;  /* not taken */
    }
block_80142A80:
    { uint32_t _pgx1 = cpu->gpr[18]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[5] = cpu->gpr[18];  /* move */
    }  /* 0x80142A80: 0x02402821 */
    cpu->gpr[3] = cpu->gpr[19] & 0x00FF;  /* 0x80142A84: 0x326300FF */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[3] << 2;
    }  /* 0x80142A88: 0x00031080 */
    { uint32_t _pgx1 = cpu->gpr[2]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];
    }  /* 0x80142A8C: 0x00431021 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[2] = cpu->gpr[2] << 4;
    }  /* 0x80142A90: 0x00021100 */
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[6] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x80142A94: 0x92460002 */
    { uint32_t _pgxa = cpu->gpr[5] + 32; cpu->gpr[7] = br2_ld32(cpu->gpr[5] + 32);
    }  /* 0x80142A98: 0x8CA70020 */
    { uint32_t _pgx1 = cpu->gpr[6]; cpu->gpr[6] = cpu->gpr[6] + 1;
    }  /* 0x80142A9C: 0x24C60001 */
    { uint32_t _pgx1 = cpu->gpr[7]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[7] = cpu->gpr[7] + cpu->gpr[2];
    }  /* 0x80142AA0: 0x00E23821 */
    { uint32_t _pgx1 = cpu->gpr[7]; cpu->gpr[7] = cpu->gpr[7] + 52;
    }  /* 0x80142AA8: 0x24E70034 */
    cpu->gpr[2] = br2_jal(0x801AD240u, 0x80142AACu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142AAC;
block_80142AAC:
    ;
    int cond_s1_ne_zero = (cpu->gpr[17] != cpu->gpr[0]);  /* save branch cond before delay slot */
    { uint32_t _pgx1 = cpu->gpr[16]; uint32_t _pgx2 = cpu->gpr[0]; cpu->gpr[4] = cpu->gpr[16];  /* move */
    }  /* 0x80142AB0: 0x02002021 */
    if (cond_s1_ne_zero) {
        goto block_80142B00;  /* taken */
    } else {
        goto block_80142AB4;  /* not taken */
    }
block_80142AB4:
    { uint32_t _pgx1 = cpu->gpr[0]; cpu->gpr[5] = 2;
    }  /* 0x80142AB8: 0x24050002 */
    cpu->gpr[2] = br2_jal(0x801A46C8u, 0x80142ABCu);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142ABC;
block_80142ABC:
    goto block_80142B00;  /* j */
block_80142AC4:
    cpu->gpr[2] = cpu->gpr[17] & 0x00FF;  /* 0x80142AC4: 0x322200FF */
    int cond_v0_ne_zero_5 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    cpu->gpr[3] = 0x1F80 << 16;  /* 0x1F800000 */
    if (cond_v0_ne_zero_5) {
        goto block_80142B00;  /* taken */
    } else {
        goto block_80142AD0;  /* not taken */
    }
block_80142AD0:
    { uint32_t _pgxa = cpu->gpr[18] + 2; cpu->gpr[2] = br2_ld8(cpu->gpr[18] + 2);
    }  /* 0x80142AD0: 0x92420002 */
    { uint32_t _pgx1 = cpu->gpr[3]; cpu->gpr[3] = cpu->gpr[3] | 0x00E8;
    }  /* 0x80142AD4: 0x346300E8 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[4] = cpu->gpr[2] << 5;
    }  /* 0x80142AD8: 0x00022140 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[2];
    }  /* 0x80142ADC: 0x00822021 */
    { uint32_t _pgx1 = cpu->gpr[4]; cpu->gpr[4] = cpu->gpr[4] << 3;
    }  /* 0x80142AE0: 0x000420C0 */
    { uint32_t _pgx1 = cpu->gpr[4]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[4] = cpu->gpr[4] + cpu->gpr[3];
    }  /* 0x80142AE4: 0x00832021 */
    cpu->gpr[2] = cpu->gpr[2] ^ 0x0001;  /* 0x80142AE8: 0x38420001 */
    { uint32_t _pgx1 = cpu->gpr[2]; cpu->gpr[5] = cpu->gpr[2] << 5;
    }  /* 0x80142AEC: 0x00022940 */
    { uint32_t _pgx1 = cpu->gpr[5]; uint32_t _pgx2 = cpu->gpr[2]; cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];
    }  /* 0x80142AF0: 0x00A22821 */
    { uint32_t _pgx1 = cpu->gpr[5]; cpu->gpr[5] = cpu->gpr[5] << 3;
    }  /* 0x80142AF4: 0x000528C0 */
    { uint32_t _pgx1 = cpu->gpr[5]; uint32_t _pgx2 = cpu->gpr[3]; cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[3];
    }  /* 0x80142AFC: 0x00A32821 */
    cpu->gpr[2] = br2_jal(0x8013F7A0u, 0x80142B00u);
    if (br2_bail_pending()) return cpu->gpr[2];
    goto block_80142B00;
block_80142B00:
    { uint32_t _pgxa = cpu->gpr[29] + 96; cpu->gpr[10] = br2_ld32(cpu->gpr[29] + 96);
    }  /* 0x80142B00: 0x8FAA0060 */
    { uint32_t _pgx1 = cpu->gpr[10]; cpu->gpr[2] = cpu->gpr[10] + 6;
    }  /* 0x80142B08: 0x25420006 */
block_80142B0C:
    { uint32_t _pgxa = cpu->gpr[29] + 84; cpu->gpr[31] = br2_ld32(cpu->gpr[29] + 84);
    }  /* 0x80142B0C: 0x8FBF0054 */
    { uint32_t _pgxa = cpu->gpr[29] + 80; cpu->gpr[30] = br2_ld32(cpu->gpr[29] + 80);
    }  /* 0x80142B10: 0x8FBE0050 */
    { uint32_t _pgxa = cpu->gpr[29] + 76; cpu->gpr[23] = br2_ld32(cpu->gpr[29] + 76);
    }  /* 0x80142B14: 0x8FB7004C */
    { uint32_t _pgxa = cpu->gpr[29] + 72; cpu->gpr[22] = br2_ld32(cpu->gpr[29] + 72);
    }  /* 0x80142B18: 0x8FB60048 */
    { uint32_t _pgxa = cpu->gpr[29] + 68; cpu->gpr[21] = br2_ld32(cpu->gpr[29] + 68);
    }  /* 0x80142B1C: 0x8FB50044 */
    { uint32_t _pgxa = cpu->gpr[29] + 64; cpu->gpr[20] = br2_ld32(cpu->gpr[29] + 64);
    }  /* 0x80142B20: 0x8FB40040 */
    { uint32_t _pgxa = cpu->gpr[29] + 60; cpu->gpr[19] = br2_ld32(cpu->gpr[29] + 60);
    }  /* 0x80142B24: 0x8FB3003C */
    { uint32_t _pgxa = cpu->gpr[29] + 56; cpu->gpr[18] = br2_ld32(cpu->gpr[29] + 56);
    }  /* 0x80142B28: 0x8FB20038 */
    { uint32_t _pgxa = cpu->gpr[29] + 52; cpu->gpr[17] = br2_ld32(cpu->gpr[29] + 52);
    }  /* 0x80142B2C: 0x8FB10034 */
    { uint32_t _pgxa = cpu->gpr[29] + 48; cpu->gpr[16] = br2_ld32(cpu->gpr[29] + 48);
    }  /* 0x80142B30: 0x8FB00030 */
    uint32_t jr_target_2 = cpu->gpr[31];  /* latch indirect target before delay slot */
    { uint32_t _pgx1 = cpu->gpr[29]; cpu->gpr[29] = cpu->gpr[29] + 88;
    }  /* 0x80142B38: 0x27BD0058 */
    return cpu->gpr[2];

}

/* Public clean entry — ev handler table slot 0x801b4ec8 (opcode 24).
 * a0=ctrl, a1=motion, a2=stream pc (all guest addrs in gpr[4..7]). */
uint32_t ev24_fx_spawn(uint32_t ctrl_g, uint32_t motion_g,
                       uint32_t pc_g, uint32_t unused)
{
  (void)ctrl_g; (void)motion_g; (void)pc_g; (void)unused;
  return ev24_cluster(0x801421C4u);
}
