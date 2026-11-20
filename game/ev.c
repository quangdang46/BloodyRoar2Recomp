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

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint32_t br2_call_generated(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
/* faithful guest-verified ports living in menu.c (same (ctrl,motion,pc)
 * guest-addr calling convention, return = next stream guest addr) */
extern uint32_t desc_cursor_select(uint32_t,uint32_t,uint32_t);
extern void slot_apply_guest(int,int);
extern void slot_mark2_set(uint32_t);

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
    uint32_t a = GU32(ctrl, FC_FOE);
    return a ? (uint8_t *)guest_at(a) : NULL;
}
static uint8_t *mof(uint8_t *ctrl)
{
    uint32_t a = GU32(ctrl, 0x38);
    return a ? (uint8_t *)guest_at(a) : NULL;
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
/* fighter_anim_advance: special-reaction loader (hit reaction by table entry). */
extern void reaction_load(uint8_t *ctrl, uint32_t entry);


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
 * arg passed to action_switch (iVar10 in guest). op5 alone reads the
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
            tabA = ta ? (uint8_t *)guest_at(ta) : NULL;
            tabB = tb ? (uint8_t *)guest_at(tb) : NULL;
        }
        eA = tabA ? GU32(tabA, idx * 4) : 0;
        eB = tabB ? GU32(tabB, idx * 4) : 0;
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
        reaction_load(ctrl, GSU32(motion, MO_B0));
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
        uint8_t *ftab = fta ? (uint8_t *)guest_at(fta) : NULL;
        uint32_t e;
        if (param < fcnt && ftab &&
            (e = GU32(ftab, param * 4)) > 0x80000000u) {
            GS16(motion, MO_BE) = G16(motion, MO_BC);
            GSU32(motion, MO_B0) = e;
            GS16(motion, MO_A2) = param;
            reaction_load(ctrl, e);
        } else {
            GS16(motion, 0x80) = 1;
            GS16(motion, MO_PREV) = 1;
            action_select(swctrl, 1);
            {
                uint32_t s = GU32(motion, MO_STREAM);
                return s ? (uint8_t *)guest_at(s) : NULL;
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
/* 257-insn transform/effect dispatcher (10-way class tbl + nested    */
/* op decode, live guest-stack/GTE-adjacent state). Delegated to the  */
/* generated implementation — clean C cannot safely express it.       */
/* ================================================================== */
static uint8_t *ev7_position(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint32_t r = br2_call_generated(0x8017b1ec,
        (uint32_t)guest_of(ctrl), (uint32_t)guest_of(motion),
        (uint32_t)guest_of(pc), 0);
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
        GS16(ctrl, FC_98) = (int16_t)(p2 ? ((a1 << 7) / p2) : -1);
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
/* EV12 — damage ISA  (guest @0x8017bd94)                              */
/* ~136-insn damage accumulator/state dispatcher. Delegated to the     */
/* generated implementation.                                           */
/* ================================================================== */
static uint8_t *ev12_damage(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint32_t r = br2_call_generated(0x8017bd94,
        (uint32_t)guest_of(ctrl), (uint32_t)guest_of(motion),
        (uint32_t)guest_of(pc), 0);
    return r ? (uint8_t *)guest_at(r) : NULL;
}

/* ================================================================== */
/* EV13 — hit-slot manager  (guest @0x8017bfb8)                        */
/* ~232-insn slot-table allocator/commit. Delegated to generated.      */
/* ================================================================== */
static uint8_t *ev13_hit_slots(uint8_t *ctrl, uint8_t *motion,
                               uint8_t *pc)
{
    uint32_t r = br2_call_generated(0x8017bfb8,
        (uint32_t)guest_of(ctrl), (uint32_t)guest_of(motion),
        (uint32_t)guest_of(pc), 0);
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
    if (mo) GS16(mo, MO_116) = GU16(ctrl, FC_ATK2);
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
ushort * ev5_action_select(int param_1,int param_2,ushort *param_3)
{
  short sVar1;
  undefined4 uVar2;
  ushort *puVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  ushort uVar8;
  int iVar9;
  int iVar10;
  /* param_1/param_2 arrive as guest addresses; param_3 is a host
   * stream pointer. Bridge the guest addrs once here. */
  uint8_t *p1 = (uint8_t *)guest_at((uint32_t)param_1);
  uint8_t *p2 = (uint8_t *)guest_at((uint32_t)param_2);
  uint8_t *m5;
  uint8_t *fmc;
  uVar8 = param_3[1];
  iVar9 = (int)(short)uVar8;
  iVar6 = ((int)((uint)*param_3 << 0x10) >> 0x18) << 0x10;
  uVar4 = iVar6 >> 0x10;
  iVar10 = param_1;
  if ((uVar4 & 0x40) != 0) {
    iVar10 = (int)GU32(p1, 0x2c);
  }
  iVar5 = (int)GU32(p1, 0x38);
  m5 = (uint8_t *)guest_at((uint32_t)iVar5);
  uVar7 = iVar6 >> 0x14 & 3;
  if (uVar7 == 1) {
    if ((GU8(p1, 5) & 1) == 0) goto LAB_8017aebc;
    goto LAB_8017ae9c;
  }
  if (((1 < uVar7) && (uVar7 == 2)) && ((GU8(p1, 5) & 1) == 0)) {
    return param_3 + 2;
  }
LAB_8017aebc:
  puVar3 = (ushort *)0x0;
  switch(uVar4 & 0xf) {
  case 0:
    if (iVar9 == -1) {
      iVar9 = (int)*(short *)(p2 + 0xa0);
      if (*(short *)(p2 + 0xa4) <= iVar9) {
        *(undefined2 *)(p2 + 0xa0) = 0;
        iVar9 = 0;
      }
      *(short *)(p2 + 0xa0) = *(short *)(p2 + 0xa0) + 1;
    }
    iVar6 = iVar9 * 4;
    if (iVar9 < *(short *)(p2 + 0xa4)) {
      if (*(uint *)((uint8_t *)guest_at(GU32(p2, 0xac)) + iVar6) < 0x80000001) {
        if (*(int *)((uint8_t *)guest_at(GU32(p2, 0xa8)) + iVar6) == 0) goto LAB_8017b034;
        *(undefined2 *)(p2 + 0xbe) = *(undefined2 *)(p2 + 0xbc);
        *(undefined4 *)(p2 + 0xb0) = *(undefined4 *)((uint8_t *)guest_at(GU32(p2, 0xa8)) + iVar6);
      }
      else {
        *(undefined2 *)(p2 + 0xbe) = 0x80;
        *(undefined4 *)(p2 + 0xb0) = *(undefined4 *)((uint8_t *)guest_at(GU32(p2, 0xac)) + iVar6);
      }
      *(short *)(p2 + 0xa2) = (short)iVar9;
LAB_8017b090:
      fighter_anim_advance(param_1,param_2);
      return param_3 + 2;
    }
    break;
  case 1:
    if (iVar9 == -1) {
      uVar8 = *(ushort *)(m5 + 0x98);
    }
    *(ushort *)(m5 + 0x80) = uVar8;
    sVar1 = *(short *)(m5 + 0x80);
    *(ushort *)(m5 + 0x82) = uVar8;
    goto LAB_8017b044;
  case 2:
    *(ushort *)(p2 + 0x98) = uVar8;
    break;
  case 3:
    *(ushort *)(p2 + 0x9a) = uVar8;
    break;
  case 4:
    *(ushort *)(p2 + 0x16) = uVar8;
    break;
  case 5:
    iVar6 = (int)GU32((const uint8_t *)guest_at(GU32(p1, 0x2c)), 0x38);
    fmc = (uint8_t *)guest_at((uint32_t)iVar6);
    if (iVar9 < *(short *)(fmc + 0xa4)) {
      if (0x80000000 < *(uint *)((uint8_t *)guest_at(GU32(fmc, 0xac)) + iVar9 * 4)) {
        *(undefined2 *)(p2 + 0xbe) = *(undefined2 *)(p2 + 0xbc);
        uVar2 = *(undefined4 *)((uint8_t *)guest_at(GU32(fmc, 0xac)) + iVar9 * 4);
        *(ushort *)(p2 + 0xa2) = uVar8;
        *(undefined4 *)(p2 + 0xb0) = uVar2;
        goto LAB_8017b090;
      }
LAB_8017b034:
      *(undefined2 *)(m5 + 0x80) = 1;
      sVar1 = *(short *)(m5 + 0x80);
      *(undefined2 *)(m5 + 0x82) = 1;
LAB_8017b044:
      action_switch_guest(iVar10,(int *)m5,(int)sVar1);
      return (ushort *)guest_at(GU32(p2, 0x84));
    }
    break;
  case 6:
    *(ushort *)(p2 + 0x14) = uVar8;
    break;
  case 7:
    puVar3 = param_3 + 2;
    *(ushort *)(p2 + (*(byte *)(p2 + 0x7d) - 1) * 2 + 0x3c) = uVar8;
  default:
    goto switchD_8017aedc_default;
  }
LAB_8017ae9c:
  puVar3 = param_3 + 2;
switchD_8017aedc_default:
  return puVar3;

}

/* ---- guest dispatch table 0x801b4e68 ----
 * Guest handlers are called as h(ctrl, motion) with the stream ptr
 * passed residually in a2 (the stream pc); return = next stream ptr
 * in $v0 or 0 to end. Our handlers take pc explicitly as pc_g —
 * identical semantics, and motion+0x84 is left to the handlers.
 * Slots 22/23 are the
 * handlers 0x80141b78/0x80141d2c ported below.
 * pc_g = guest stream pc passed explicitly like the guest's $a2 —
 * the dispatcher does NOT mirror it into motion+0x84 (guest-faithful;
 * only handlers themselves write +0x84, e.g. EV3 wait resume). */
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
ushort * ev_spark_spawn(int param_1,undefined4 param_2,ushort *param_3)
{
  ushort uVar1;
  int iVar2;
  int iVar3;
  /* param_1 arrives as a guest address; param_3 is a host stream ptr. */
  uint8_t *p1 = (uint8_t *)guest_at((uint32_t)param_1);
  uint8_t *ti;

  if (*g_DAT_1f8000b1 == 0) {
    /* guest: s1 = sx8(byte@pc+1); slti s1,3 is SIGNED — negative
     * opcodes still take the alloc path (@0x80141bb4-0x80141bc4) */
    iVar3 = (int)(int8_t)(*param_3 >> 8);
    if (iVar3 < 3) {
      iVar3 = fx_group_alloc(1,1,0,0);
      if (iVar3 != 0) {
        iVar2 = (int)noop_c(iVar3);
        ti = (uint8_t *)guest_at((uint32_t)iVar2);
        noop_a(iVar3,GU32(p1, 0x20) + 0x124,
                     (uint32_t)param_1,guest_of(g_DAT_801f0760));
        uVar1 = *param_3;
        *(uint32_t *)(ti + 0x18) = 0x80140710;
        *(uint32_t *)(ti + 0x1c) = 0;
        /* guest: sll16/sra24 -> sx8(byte@pc+1), sw @task+0x14
         * (@0x80141c38-0x80141c44) */
        *(int *)(ti + 0x14) = (int)(int8_t)(uVar1 >> 8);
      }
    }
    else if ((iVar3 == 3) &&
               (iVar3 = fx_group_alloc(1,1,GU8(p1,2) + 1,0), iVar3 != 0)) {
        iVar2 = (int)noop_c(iVar3);
        ti = (uint8_t *)guest_at((uint32_t)iVar2);
        node_param_quad_set(iVar3,(uint32_t)param_1,
                     guest_of(g_DAT_1f8000e8 + (GU8(p1,2) ^ 1) * 0x108),
                     (uint32_t)(GU8(p1,2) + 1),
                     guest_of(g_DAT_801f0760));
        *(uint32_t *)(ti + 0x14) =
             (uint32_t)GU8((uint8_t *)guest_at(GU32(p1, 0x38)), 0x100);
        if (*(int8_t *)(p1 + 3) == 3) {
          *(uint32_t *)(ti + 0x18) = 0x1f800000;
          /* guest: lh @0x801f093a / lh @0x801f093e — signed 16-bit
           * loads (@0x80141ce8/@0x80141cf8), not u32 */
          *(int *)(ti + 0x1c) = (int)*(const int16_t *)g__DAT_801f093a;
          iVar3 = (int)*(const int16_t *)g__DAT_801f093e;
          *(uint32_t *)(ti + 0x24) = 0x8013f178;
          *(int *)(ti + 0x20) = iVar3;
        }
    }
  }
  return param_3 + 1;
}

static uint8_t *w22(uint8_t *c, uint8_t *m, uint32_t pc_g)
{
    return (uint8_t *)ev_spark_spawn(
        (int)guest_of(c), 0, (ushort *)guest_at(pc_g));
}

/* ev_cmd_dispatch — EV23: event/sound/effect command dispatcher.
 * Consumes a 4-byte record {u8 op, u8 arg, u16 param}; returns pc+4. */
ushort * ev_cmd_dispatch(int param_1,undefined4 param_2,ushort *param_3)
{
  ushort uVar1;
  undefined2 uVar2;
  undefined2 *puVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined2 uStack_18[4];
  /* param_1 arrives as a guest address; param_3 is a host stream ptr. */
  uint8_t *p1 = (uint8_t *)guest_at((uint32_t)param_1);
  uint8_t *mo;

  uVar1 = param_3[1];
  uVar7 = (uint)uVar1;
  if (*g_DAT_1f8000b1 == 0) {
    switch((int)((uint)*param_3 << 0x10) >> 0x18) {
    case 0:
      mo = (uint8_t *)guest_at(GU32(p1, 0x38));
      if ((*(int8_t *)(mo + 0x100) == 0) || ((uVar1 & 0x80) == 0)) {
        uVar6 = uVar7 & 0x7f;
      }
      else {
        uVar6 = uVar7 & 0xff;
      }
      hit_spark_spawn(param_1,uVar6,(int)(int8_t)(uVar7 >> 8));
      break;
    case 1:
      triple_track_spawn(GU8(p1,2),uVar7 & 0xff,(int)(int8_t)(uVar7 >> 8));
      break;
    case 2:
      /* guest @0x80141DF0: a2 = sx8(param>>8) = sx8(byte@pc+3),
       * a3 = param&0xFF = byte@pc+2 — order matters, not swapped. */
      speech_bubble_spawn((undefined4 *)guest_at(GU32(p1, 0x20) + 0x124),
                   *(short *)(p1 + 0x4e) + -0x400,
                   (int)(int8_t)(uVar7 >> 8),(ushort)(uVar7 & 0xff));
      break;
    case 3:
      speech_bubble_spawn((undefined4 *)guest_at(GU32(p1, 0x20) + 0x124),
                   *(short *)(p1 + 0x4e) + 0x400,
                   (int)(int8_t)(uVar7 >> 8),(ushort)(uVar7 & 0xff));
      break;
    case 4:
      effect_arg_set(param_1);
      break;
    case 5:
      sprite_burst_spawn(param_1,(int)(short)uVar1);
      break;
    case 6:
      confetti_burst_spawn(param_1,(int)(short)uVar1);
      break;
    case 7:
      if (*(int8_t *)((uint8_t *)guest_at(*g__DAT_1f8000c4) + 2) != 0) {
        popup_spawn(param_1,(int)(int8_t)(uVar7 >> 8),uVar7 & 0xff);
      }
      break;
    case 8:
      iris_wipe_spawn(param_1,(int)(short)uVar1);
      break;
    case 9: {
      /* guest: s0 = sx16(param); if mo+0x100: s0 = lbu(0x801b180c+s0);
       * offset = s0*0x50 (@0x80141f24-0x80141f74). Index stays signed —
       * guest adds sx16 to the table base, so negative params read
       * before the table exactly like the guest does. */
      int32_t pi = (int16_t)uVar1;
      mo = (uint8_t *)guest_at(GU32(p1, 0x38));
      if (*(int8_t *)(mo + 0x100) != 0) {
        pi = (int32_t)g_DAT_801b180c[pi];
      }
      iVar5 = fx_group_alloc(1,1,0,0xf);
      if (iVar5 != 0) {
        noop_b(iVar5,GU32(p1, 0x20) + (uint32_t)(pi * 0x50) + 0x34,
                     guest_of(g_DAT_801f0760));
      }
      break;
    }
    case 10:
      color_rec_table_init(GU8(p1,3),(uint32_t)(int16_t)uVar1);
      break;
    case 0xb:
      firework_spawn((undefined2 *)guest_at(GU32(p1, 0x20) +
                        (uint32_t)((int16_t)uVar1 * 0x50) + 0x34));
      break;
    case 0xc: {
      uint8_t *obj;
      uVar6 = (int)(int8_t)(uVar7 >> 8) & 0x7f;
      mo = (uint8_t *)guest_at(GU32(p1, 0x38));
      if ((*(int8_t *)(mo + 0x100) != 0) &&
         (((int)(uVar7 << 0x10) >> 0x10 & 0x8000U) != 0)) {
        uVar6 = (uint)g_DAT_801b180c[uVar6];
      }
      iVar5 = uVar6 * 0x50;
      obj = (uint8_t *)guest_at(GU32(p1, 0x20));
      /* guest copies 3 u16 to its stack vec; emit_task_spawn_b reads
       * 8 bytes — keep the host vec contiguous and padded like the
       * guest frame (@0x80141e18-0x80141e50) */
      uStack_18[0] = *(undefined2 *)(obj + iVar5 + 0x34);
      uStack_18[1] = *(undefined2 *)(obj + iVar5 + 0x38);
      uStack_18[2] = *(undefined2 *)(obj + iVar5 + 0x3c);
      uStack_18[3] = 0;
      iVar5 = (uVar7 & 0xff) * 4;
      emit_task_spawn_b((undefined4 *)uStack_18,
                   *(uint32_t *)((char *)g__DAT_801f0790 + iVar5),
                   *(uint32_t *)((char *)g__DAT_801f0780 + iVar5),0);
      break;
    }
    case 0xd:
      puVar3 = (undefined2 *)guest_at((uint32_t)fx_record_alloc(GU32(p1, 0x20) + 0x124,6,
                            0x80000010,2,*g_PTR_DAT_801b1d44));
      if (puVar3 != (undefined2 *)0x0) {
        uVar2 = (undefined2)rand_lcg();
        *puVar3 = uVar2;
      }
      break;
    case 0xe:
      piVar4 = (int *)guest_at((uint32_t)fx_record_alloc(GU32(p1, 0x20) + 0x444,0xc,
                                   0x80000012,0x8e,guest_of(g_DAT_801b1d5c)));
      if (piVar4 != (int *)0x0) {
        *piVar4 = param_1;
        piVar4[1] = (int)guest_of(g_DAT_1f8000e8 + (GU8(p1,2) ^ 1) * 0x108);
      }
      break;
    case 0xf:
      fighter_reset_fx(1,p1);
    }
  }
  return param_3 + 2;
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
