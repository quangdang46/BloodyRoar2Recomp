/*
 * br2_ev.c — clean-room reference implementation of the BR2 event VM.
 *
 * Event streams (.ACT/.ANI embedded) are consumed by the dispatcher
 * FUN_8017ea7c: each record starts with a type byte (0..23) dispatched
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
 * All state via raw guest offsets — see br2_types.h / br2_vm.c.
 * Project-original clean-room code (MIT).
 */
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "lut.h"

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);

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

static uint8_t *objpos(uint8_t *ctrl)
{
    uint32_t a = GU32(ctrl, 0x20);
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
static uint8_t *objpos(uint8_t *ctrl);

/* FUN_8017a6e8: select event stream by action index (writes
 * motion+0x84/+0x88, resets motion+0x90). Host supplies this — it
 * indexes the .ANI move table. */
extern void br2_action_select(uint8_t *ctrl, int idx);
/* FUN_80185e6c: special-reaction loader (hit reaction by table entry). */
extern void br2_reaction_load(uint8_t *ctrl, uint32_t entry);


/* ================================================================== */
/* Dispatcher                                                           */
/* ================================================================== */

/* Returns next stream ptr, or NULL to end. Mirrors FUN_8017ea7c:
 * countdown gate first, then type-byte dispatch. */
uint8_t *br2_ev_step(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
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
/* record: {u8 type, s8 cond} — tests FLAG35 read-back word.           */
/* ================================================================== */
static uint8_t *ev0_branch(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    int32_t f = GS32(motion, MO_FLAGT);
    int8_t c = (int8_t)GU8(pc, 1);
    uint8_t *next = pc + 2;
    int take;
    switch (c) {
    case 0: take = (f == 0); break;
    case 1: take = (f != 0); break;
    case 2: take = (f <  0); break;
    case 3: take = (f >  0); break;
    /* remaining ops: stream-switch / fx-call variants — approx: */
    default: take = 1; break;
    }
    if (!take) return NULL;              /* halt stream */
    return next;
}

/* ================================================================== */
/* EV1 — alternate stream pointer                                      */
/* ================================================================== */
static uint8_t *ev1_altstream(uint8_t *motion, uint8_t *pc)
{
    if (GU8(pc,1) == 0) {
        uint32_t a = GU32(motion, MO_ALT);
        return a ? (uint8_t *)guest_at(a) : NULL;
    }
    if (GU8(pc,1) == 1) {
        uint8_t *np = pc + 2;
        GSU32(motion, MO_ALT) = guest_of(np);
        return np;
    }
    return NULL;
}

/* ================================================================== */
/* EV2 — hitstun/counter gate                                          */
/* byte1>=0: continue iff s16 motion+0xc6 < param(u16@+2)              */
/* byte1<0: 7-way sub-conditions on victim accumulators / flags        */
/* ================================================================== */
static uint8_t *ev2_hitstun_gate(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    int8_t b = (int8_t)GU8(pc, 1);
    if (b >= 0) {
        uint16_t param = GU16(pc, 2);
        return (G16(motion, MO_C6) < (int16_t)param) ? pc + 4 : NULL;
    }
    /* negative: victim-state gates — approximated from notes:
     * checks foe motion +0xcc/+0xca, ctrl+0x5c bits, distance. */
    {
        uint8_t *foe = foe_of(ctrl);
        uint8_t *fm = mof(foe);
        int sub = b + 7;
        int ok = 1;
        switch (sub) {
        case 0: ok = (G16(fm, MO_CC) == 0); break;
        case 1: ok = (G16(fm, MO_CA) <= 0); break;
        case 2: ok = (ST(foe, FC_STATE1) & 0x80) == 0; break;
        case 3: ok = (ST(ctrl, FC_STATE1) & 0x28) == 0; break;
        default: ok = 1; break;
        }
        return ok ? pc + 2 : NULL;
    }
}

/* ================================================================== */
/* EV3 — frame wait                                                    */
/* byte1==0xff -> countdown = s16 ctrl+0xbe <<4 ; else byte1<<4       */
/* ================================================================== */
static uint8_t *ev3_wait(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint8_t b = GU8(pc,1);
    int16_t frames;
    if (b == 0xff) frames = G16(ctrl, FC_ATK3) << 4;
    else           frames = (int16_t)b << 4;
    GS16(motion, 0x90) = frames;
    return pc + 2;
}

/* ================================================================== */
/* EV4 — conditional branch (tbl 0x8012272c, 27 ops)                   */
/* pass -> pc+4 ; fail -> ctrl+0x30 (.ANI base) + u16@pc+2             */
/* ================================================================== */
static uint8_t *ev4_branch(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint8_t cls = GU8(pc,1) >> 6;         /* bits 6-7 of byte1 hi? —
                                              notes: byte1>>8&0xc0 on
                                              u16; using u16 operand */
    uint16_t op16 = GU16(pc, 0);
    uint8_t hi = (uint8_t)(op16 >> 8);
    uint8_t cls2 = hi & 0xc0;
    int op = hi & 0x3f;
    uint8_t *t;
    uint8_t *self = ctrl, *foe = foe_of(ctrl);
    int32_t f;
    int take;
    (void)cls;

    /* target select: 0x00 self, 0x40 self-if-flag5, 0x80 self,
     * 0xc0 foe — approximate: cls2 bit7 -> foe */
    if (cls2 & 0x80) t = foe; else t = self;
    if (cls2 == 0x40 && !(GU8(self, FC_MODE5) & 1)) t = self;

    /* condition tests (decoded ops): */
    switch (op) {
    case 0:  f = GS32(motion, MO_FLAGT); take = (f == 0); break;
    case 1:  f = GS32(motion, MO_FLAGT); take = (f != 0); break;
    case 2:  f = GS32(motion, MO_FLAGT); take = (f <  0); break;
    case 3:  f = GS32(motion, MO_FLAGT); take = (f >  0); break;
    case 4:  f = GS32(motion, MO_FLAGT); take = (f >= 0); break;
    case 5:  f = GS32(motion, MO_FLAGT); take = (f <= 0); break;
    /* ops store a2+4 -> motion+0x8c variants (bookkeeping) */
    case 12: take = (G16(motion, MO_CC) == 0) && (G16(motion, MO_CA) <= 0);
             break;
    case 14: take = !(GU8(t,5) & 0x10) && G16(t, FC_D6) < 0x1e00; break;
    default:
        if (op >= 16 && op <= 26) {          /* char-ID equality */
            take = (GU8(t, 0x03) == (uint8_t)(op - 15));
        } else take = 1;
        break;
    }
    if (take) return pc + 4;
    /* fail -> branch: .ANI base + u16@rec+2 */
    return (uint8_t *)guest_at(GU32(ctrl, FC_ANI)) + GU16(pc, 2);
}

/* ================================================================== */
/* EV5 — action/move select (tbl 0x8012279c, 8 ops)                    */
/* ================================================================== */
/* EV5 = FUN_8017ae18 (guest-verified). Record = 4 bytes:
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

    if (mode == 1) return pc + 4;                    /* guest: both
        branches -> skip; suspicious merge, kept literal */
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
        br2_reaction_load(ctrl, GSU32(motion, MO_B0));
        return pc + 4;
    fallback:
        GS16(motion, 0x80) = 1;
        GS16(motion, MO_PREV) = 1;
        br2_action_select(swctrl, 1);
        {
            uint32_t s = GU32(motion, MO_STREAM);
            return s ? (uint8_t *)guest_at(s) : NULL;
        }
    }
    case 1:     /* action switch: idx = param==-1 ? motion+0x98 : param */
        idx = (param == -1) ? G16(motion, MO_98) : param;
        GS16(motion, 0x80) = idx;
        GS16(motion, MO_PREV) = idx;
        br2_action_select(swctrl, idx);
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
            (e = GU32(ftab, (uint16_t)param * 4)) > 0x80000000u) {
            GS16(motion, MO_BE) = G16(motion, MO_BC);
            GSU32(motion, MO_B0) = e;
            GS16(motion, MO_A2) = param;
            br2_reaction_load(ctrl, e);
        } else {
            GS16(motion, 0x80) = 1;
            GS16(motion, MO_PREV) = 1;
            br2_action_select(swctrl, 1);
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
/* EV6 — guard/counter state sync (6 ops, tbl 0x801227bc)              */
/* ops copy/toggle flag bytes motion+0x9c/+0x100 self<->foe;          */
/* master op swaps u16 pairs in *(ctrl+0x18) sub-struct.               */
/* ================================================================== */
static uint8_t *ev6_guard_sync(uint8_t *ctrl, uint8_t *motion,
                               uint8_t *pc)
{
    int op = GU8(pc, 1);
    uint8_t *fm = mof(foe_of(ctrl));
    switch (op) {
    case 0: GSU8(motion, MO_9C) = GU8(fm, MO_9C); break;
    case 1: GSU8(fm, MO_9C)   = GU8(motion, MO_9C); break;
    case 2: GSU8(motion, MO_100)= GU8(fm, MO_100); break;
    case 3: GSU8(fm, MO_100)  = GU8(motion, MO_100); break;
    case 4: {   /* swap u16 pairs in ctrl+0x18 sub-struct */
        uint32_t sg = GU32(ctrl, 0x18);
        uint8_t *sub = sg ? (uint8_t *)guest_at(sg) : NULL;
        uint16_t a = GU16(sub, 0x1c4), b = GU16(sub, 0x23c);
        GSU16(sub, 0x1c4) = b; GSU16(sub, 0x23c) = a;
        a = GU16(sub, 0x264); b = GU16(sub, 0x28c);
        GSU16(sub, 0x264) = b; GSU16(sub, 0x28c) = a;
        break;
    }
    default: break;
    }
    return pc + 2;
}

/* ================================================================== */
/* EV7 — position/physics command (9 ops, tbl 0x801227d4)              */
/* record 6B {u16 cls, s16 p1, s16 p2}: writes motion pos triple      */
/* +0xd4/+0xd6/+0xd8 via abs-set or sin/cos rotated deltas.            */
/* ================================================================== */
static uint8_t *ev7_position(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint16_t op16 = GU16(pc, 0);
    int op = op16 & 0x0f;
    int16_t p1 = G16(pc, 2), p2 = G16(pc, 4);
    uint32_t og = GU32(ctrl, 0x20);
    uint8_t *obj = og ? (uint8_t *)guest_at(og) : NULL;
    int32_t ca, sa, dx, dy;
    (void)obj;

    switch (op) {
    case 0:  /* absolute set from object pos */
        GS16(motion, MO_D4) = G16(obj, 0x174);
        GS16(motion, MO_D6) = G16(obj, 0x176);
        GS16(motion, MO_D8) = G16(obj, 0x17c);
        break;
    case 1: case 2: case 3: {  /* rotated delta: pos += (p1*cos-p2*sin)>>16 */
        uint16_t ang = (uint16_t)GS16(motion, MO_E4) & 0x3ff;
        ca = br2_cos_lut[ang & 0x3ff];
        sa = br2_sin_lut[ang & 0x3ff];
        dx = ((int32_t)p1 * ca - (int32_t)p2 * sa) >> 16;
        dy = ((int32_t)p1 * sa + (int32_t)p2 * ca) >> 16;
        GS16(motion, MO_D4) += (int16_t)dx;
        GS16(motion, MO_D6) += (int16_t)dy;
        break;
    }
    case 4:  /* facing/flag-bit mirror modes via ctrl+0x6c bits */
        if (ST(ctrl, FC_STATE5) & 1)
            GS16(motion, MO_D4) = -GS16(motion, MO_D4);
        GS16(motion, MO_D4) += p1;
        break;
    case 5:  /* approach step toward foe */
        GS16(motion, MO_D4) += (int16_t)((p1 * 0xb40b40b5LL) >> 39);
        break;
    default: break;
    }
    return pc + 6;
}

/* ================================================================== */
/* EV8 — motion-param ISA (11 ops, tbl 0x801227fc); 4B recs            */
/* writes motion+0xd0 param / +0xe4 angle (s12 fixed point)            */
/* ================================================================== */
static uint8_t *ev8_motion_param(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    uint16_t op16 = GU16(pc, 0);
    int op = op16 & 0x0f;
    int16_t p = G16(pc, 2);
    uint8_t *foe = foe_of(ctrl);
    int32_t v;
    switch (op) {
    case 0:  GS16(motion, MO_D0) = GU16(pc,2); break;   /* u16 store */
    case 1:  GS16(motion, MO_D0) += p; break;
    case 2:  GS16(motion, MO_D0) = G16(foe, 0x4e) + p; break;
    case 3:  GS16(motion, MO_E4) = G16(ctrl, FC_DISTX) + p; break;
    case 4:  GS16(motion, MO_E4) = G16(ctrl, 0xc4)
                 - (G16(ctrl, 0x4e) - 0x800) + p; break;
    case 5:  GS16(motion, MO_E4) = G16(ctrl, 0x70) + p; break;
    case 6: {   /* facing decode via ctrl+0x6c bits -> dir +-0x400/800 */
        uint32_t f = ST(ctrl, FC_STATE5);
        int dir = 0;
        if (f & 0x10) dir += 0x400;
        if (f & 0x20) dir -= 0x400;
        if (f & 0x40) dir += 0x800;
        if (f & 0x80) dir -= 0x800;
        GS16(motion, MO_E4) = dir - (G16(ctrl,0x4e) - G16(motion,MO_E4)) + p;
        break;
    }
    case 7:
        if (!(ST(ctrl, FC_STATE1) & 0x10000000))
            GS16(motion, MO_E4) += (G16(ctrl,FC_DISTX) > 0) ? p : -p;
        break;
    case 8:  break;                                     /* nop */
    case 9:
        GS16(motion, MO_D4) = G16(objpos(ctrl), 0x174);
        GS16(motion, MO_D8) = G16(objpos(ctrl), 0x17c);
        GS16(motion, MO_E4) = G16(ctrl,0xc4) - (G16(ctrl,0x4e)-0x800) + p;
        break;
    case 10: GS16(motion, MO_E4) += p; break;
    }
    (void)v;
    return pc + 4;
}

/* ================================================================== */
/* EV9 — physics/velocity ISA (11 ops, tbl 0x8012282c); 6B recs        */
/* writes ctrl vel/pos +0x8a..+0x98 (launch vel, clamps)              */
/* ================================================================== */
static uint8_t *ev9_physics(uint8_t *ctrl, uint8_t *motion,
                            uint8_t *pc)
{
    uint16_t op16 = GU16(pc, 0);
    int op = op16 & 0x0f;
    int16_t p1 = G16(pc, 2), p2 = G16(pc, 4);
    int32_t v;
    switch (op) {
    case 0:  GS16(ctrl, 0x8a) = p1; break;
    case 1:  GS16(ctrl, 0x8c) = p1; break;
    case 2:  GS16(ctrl, 0x8e) = p1; break;
    case 3:  GS16(ctrl, 0x90) = p1; break;
    case 4:  GS16(ctrl, 0x92) = p1; break;
    case 5:  /* launch vel: min(p1,lim)*128/div style scaling */
        v = p1 < p2 ? p1 : p2;
        GS16(ctrl, 0x96) = (int16_t)((v * 128) / 0x100);
        break;
    case 6:  /* vel_y clamp [-0x500, 0x1000] */
        v = p1;
        if (v < -0x500) v = -0x500;
        if (v >  0x1000) v =  0x1000;
        GS16(ctrl, 0x98) = (int16_t)v;
        break;
    case 7:  GS16(ctrl, 0xd0) = p1; break;
    case 8:  GS16(ctrl, 0xd4) = p1; break;
    case 9:  GS16(ctrl, 0xd8) = p1; break;
    default: break;
    }
    (void)motion;
    return pc + 6;
}

/* ================================================================== */
/* EV10 — FLAG35 state/flag ISA (35 ops, tbl 0x8012285c); 6B recs      */
/* {u16 classop, u32 maskval}; class bits byte1>>8&0xc0 -> target      */
/* ops: SET/AND/OR/XOR/TEST on state words +0x54/58/5c/60/64/68/6c    */
/* ================================================================== */
static uint8_t *ev10_flags(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint16_t op16 = GU16(pc, 0);
    uint8_t hi = (uint8_t)(op16 >> 8);
    int cls = hi & 0xc0;
    int op = hi & 0x3f;
    uint32_t val = GU16(pc,2) | ((uint32_t)GU16(pc,4) << 16);
    uint8_t *t = (cls & 0x80) ? foe_of(ctrl) : ctrl;
    int field;

    /* field select: low 3 bits of op pick state word; op class in
     * high bits chooses SET/AND/OR/XOR/TEST — approximated from the
     * decoded flag ISA (see RE_NOTES FLAG35 section) */
    field = op & 7;
    {
        static const int fo[8] = { 0x54, 0x58, 0x5c, 0x60,
                                   0x64, 0x68, 0x6c, 0x94 };
        int off = fo[field];
        uint32_t cur = (off == 0x94) ? GU32(motion, MO_FLAGT)
                                     : ST(t, off);
        uint32_t r;
        switch ((op >> 3) & 7) {
        case 0: r = val; break;                    /* SET */
        case 1: r = cur & val; break;              /* AND */
        case 2: r = cur | val; break;              /* OR  */
        case 3: r = cur ^ val; break;              /* XOR */
        default: r = cur & val; break;             /* TEST -> readback */
        }
        if (off == 0x94) GSU32(motion, MO_FLAGT) = r;
        else            GSU32(t, off) = r;
        /* op0 special: commit attack power ctrl+0x86 -> +0x88 with
         * 0xC0000000-class override to +0xfc (FLAG35 op0) */
        if (op == 0 && cls == 0) {
            uint32_t pw = GU32(t, 0x86);
            if (pw & 0xc0000000u) GSU32(t, 0xfc) = pw;
            else                  GSU32(t, 0x88) = pw;
        }
    }
    return pc + 6;
}

/* ================================================================== */
/* EV12 — damage ISA (8 ops, tbl 0x801228ec); 4B recs                  */
/* operates on victim accumulators +0xc8/ca/c2/cc/c6 using            */
/* attacker +0x88 power (see RE_NOTES DMG8).                          */
/* ================================================================== */
static uint8_t *ev12_damage(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint16_t op16 = GU16(pc, 0);
    int op = op16 & 0x0f;
    int16_t p = G16(pc, 2);
    uint8_t *vic = foe_of(ctrl);          /* damage lands on foe */
    uint8_t *vm = mof(vic);
    int32_t pow = G16(ctrl, 0x88);
    int32_t dmg;
    switch (op) {
    case 0:  /* direct power damage */
        GSU16(vic, FC_HPDISP) = GU16(vic, FC_HPDISP) + (uint16_t)pow;
        GS16(vm, MO_CA) += (int16_t)pow;
        break;
    case 1:  /* fixed p*16 damage */
        dmg = (int32_t)p * 16;
        GS16(vic, FC_HPDISP) += (int16_t)dmg;
        GS16(vm, MO_C6) += (int16_t)(dmg >> 4);
        break;
    case 2:  /* guard chip */
        if (G16(vic, FC_ATK3) < 2)
            GS16(vic, FC_HPDISP) += (int16_t)(pow >> 2);
        break;
    case 3:  /* flat add to accumulators */
        GS16(vm, MO_CC) += p;
        GS16(vm, 0xc2)  += p;
        break;
    case 4:  /* scaled class-select via victim param table */
        dmg = (int32_t)((int64_t)pow * p / 0x100);
        GS16(vm, MO_C8) += (int16_t)dmg;
        break;
    case 5:  dmg = pow * 16;               /* p16-scaled */
        GS16(vm, MO_CA) += (int16_t)dmg;
        break;
    case 6:  GS16(vm, MO_CC) += (int16_t)(G16(ctrl,0xc2) * 16); break;
    case 7:  /* multi-accumulator add */
        GS16(vm, MO_CA) += p;
        GS16(vm, 0xc2)  += p;
        GS16(vm, 0xc8)  += p;
        break;
    }
    (void)motion;
    return pc + 4;
}

/* ================================================================== */
/* EV13 — hit-slot manager (motion+0xf8 slot table, 8x4B ctrl+0x14)     */
/* ================================================================== */
static uint8_t *ev13_hit_slots(uint8_t *ctrl, uint8_t *motion,
                               uint8_t *pc)
{
    int op = GU8(pc, 1) & 0x0f;
    int slot = GU8(motion, MO_100);
    uint8_t *ent = (uint8_t *)guest_at(GU32(motion, MO_F8)) + slot * 4;
    switch (op) {
    case 0: GSU8(ent, 0) = 1; GSU8(ent, 1) = 0; break;      /* alloc */
    case 1: GSU8(ent, 0) = 0; break;                        /* clear */
    case 2: GSU8(ent, 1)++; break;                          /* counter++ */
    case 3: GSU16(ent, 2) = GU16(pc, 2); break;             /* store param */
    }
    return pc + 4;
}

/* ================================================================== */
/* EV14 — chip/damage bytes -> ctrl+0xda/+0xdc; gauge scaling          */
/* ================================================================== */
static uint8_t *ev14_chip(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    uint16_t op16 = GU16(pc, 0);
    uint8_t *t = (op16 & 0x4000) ? foe_of(ctrl) : ctrl;
    int32_t chip = G16(pc, 2);
    if (ST(t, FC_STATE5) & 0x100) {          /* gauge-scaled variant */
        chip = (int32_t)((int64_t)chip * G16(t, FC_D6) / 0x10000);
    }
    GSU8(t, FC_CHIP_A) = (uint8_t)(GU8(t, FC_CHIP_A) + (uint8_t)chip);
    GSU8(t, FC_CHIP_B) = (uint8_t)(GU8(t, FC_CHIP_B) + (uint8_t)(chip >> 8));
    return pc + 4;
}

/* ================================================================== */
/* EV15 — direct hitstun-class set                                      */
/* ctrl+0xdb = byte@+2 ; motion+0xc6 = s12@+2 (negated iff +0x100!=0) */
/* ================================================================== */
static uint8_t *ev15_hitstun_set(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    int16_t v = G16(pc, 2);
    GSU8(ctrl, FC_CHIP_B) = GU8(pc, 2);
    if (GU8(motion, MO_100) != 0) v = -v;
    GS16(motion, MO_C6) = (int16_t)((v << 20) >> 20);   /* s12 */
    return pc + 4;
}

/* ================================================================== */
/* EV16 — attack-window/cancel counter init                            */
/* ================================================================== */
static uint8_t *ev16_atkwin_init(uint8_t *ctrl, uint8_t *motion,
                                 uint8_t *pc)
{
    int op = GU8(pc, 1) & 0x0f;
    GSU8(motion, MO_101) = 0;
    GSU8(motion, MO_102) = 0;
    GSU8(motion, MO_103) = 0;
    GSU8(motion, MO_104) = 0;
    GS16(ctrl, 0x26) = 0;
    if (op == 0) GS16(ctrl, FC_ATK3) = -1;
    else         GS16(ctrl, FC_ATK3) =  0;
    GS16(ctrl, FC_ATK2) = (int16_t)((int8_t)GU8(pc,2) << 4);
    return pc + 4;
}

/* ================================================================== */
/* EV17 — range-gated approach                                         */
/* if |ctrl+0x74 + motion+0xd2| >= param -> motion+0x10a/0x10c =      */
/* param*0xb40b40b5>>7 (fixed-point step)                             */
/* ================================================================== */
static uint8_t *ev17_approach(uint8_t *ctrl, uint8_t *motion,
                              uint8_t *pc)
{
    int16_t param = G16(pc, 2);
    int32_t dist = G16(ctrl, FC_DISTX) + G16(motion, MO_D2);
    if (dist < 0) dist = -dist;
    if (dist >= param) {
        int32_t step = (int32_t)((int64_t)param * 0xb40b40b5LL >> 55);
        GS16(motion, MO_10A) = (int16_t)step;
        GS16(motion, MO_10C) = (int16_t)step;
        GS32(ctrl, FC_STATE1) |= 0x40;
    } else {
        GS32(ctrl, FC_STATE1) &= ~0x42;
    }
    return pc + 4;
}

/* ================================================================== */
/* EV18 — beast/variant mode switch                                    */
/* ================================================================== */
static uint8_t *ev18_beast(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    GSU8(ctrl, FC_MODE5) = GU8(pc, 1);
    if (GU8(pc,1) & 1)
        br2_action_select(ctrl, GU16(pc, 2));   /* param-table reload */
    return pc + 4;
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
/* EV20 — x16 timers (5 ops, tbl 0x8012294c)                           */
/* ctrl+0xc0 (only if >=0), +0xd0, +0xd2, +0xcc, +0xd4 = s16@+2 <<4   */
/* ================================================================== */
static uint8_t *ev20_timers(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    int op = GU8(pc, 1) & 0x07;
    int16_t v = (int16_t)(G16(pc, 2) << 4);
    switch (op) {
    case 0: if (G16(ctrl, FC_C0) >= 0) GS16(ctrl, FC_C0) = v; break;
    case 1: GS16(ctrl, FC_D0) = v; break;
    case 2: GS16(ctrl, FC_D2) = v; break;
    case 3: GS16(ctrl, 0xcc)  = v; break;
    case 4: GS16(ctrl, FC_D4) = v; break;
    }
    (void)motion;
    return pc + 4;
}

/* ================================================================== */
/* EV21 — beast-gated 6-byte record skip                               */
/* ================================================================== */
static uint8_t *ev21_skip(uint8_t *ctrl, uint8_t *motion, uint8_t *pc)
{
    if (!(GU8(ctrl, FC_MODE5) & 1)) return pc + 6;
    return pc + 2;
}
extern void FUN_80185e6c(int,int);
extern void FUN_8017a6e8(int,int*,int);
ushort * FUN_8017ae18(int param_1,int param_2,ushort *param_3)
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
      FUN_80185e6c(param_1,param_2);
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
      FUN_8017a6e8(iVar10,(int *)m5,(int)sVar1);
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
 * passed residually in a2 = motion+0x84; return = next stream ptr
 * (stored back to motion+0x84) or NULL to end. Our handlers take pc
 * explicitly, so each table entry wraps the semantic handler reading
 * pc from motion+0x84 — identical semantics. Slots 22/23 are the
 * handlers 0x80141b78/0x80141d2c ported below. */
typedef uint8_t *evhandler_t(uint8_t *ctrl, uint8_t *motion);
extern uint8_t *g_DAT_1f8000b1, *g_DAT_1f8000e8, *g_DAT_801f0760;
extern uint32_t *g__DAT_801f093a, *g__DAT_801f093e;
extern int FUN_801ad38c(undefined4,undefined4,int,int);
extern undefined4 FUN_801ad284(int);
extern void FUN_801ad240(int,undefined4,undefined4,undefined4);
extern void FUN_801ad260(int,undefined4,undefined4,undefined4,undefined4);
extern void FUN_80140710();
extern void FUN_8013f178();
extern void FUN_8013f5f8(int);
extern void FUN_80140b18(int,int);
extern void FUN_8013da2c(int,int);
extern void FUN_8013ab8c(int,int,int);
extern void FUN_80141050();
extern void FUN_80138a24(int,undefined2,int);
extern void FUN_8013b5a8(int,int);
extern void FUN_801ad224(int,undefined4,undefined4);
extern void FUN_8013ed98(undefined4*,undefined4,undefined4,undefined4);
extern int FUN_8013b088(undefined4,int,uint,undefined4,undefined4);
extern uint FUN_8018e4d0(void);
extern uint32_t *g__DAT_1f8000c4, *g__DAT_801f0780, *g__DAT_801f0790;
extern uint8_t *g_DAT_801b180c;
extern uint8_t *g_DAT_801b1d5c;
extern uint32_t *g_PTR_DAT_801b1d44;
extern void FUN_80140ca0(int,int);

/* FUN_80141b78 — EV22: spawn hit-spark/effect task. a2 = pc (stream
 * ptr); consumes one u16 record, returns pc+2. */
ushort * FUN_80141b78(int param_1,undefined4 param_2,ushort *param_3)
{
  ushort uVar1;
  int iVar2;
  int iVar3;
  /* param_1 arrives as a guest address; param_3 is a host stream ptr. */
  uint8_t *p1 = (uint8_t *)guest_at((uint32_t)param_1);
  uint8_t *ti;

  if (*g_DAT_1f8000b1 == 0) {
    iVar3 = (int)((uint)*param_3 << 0x10) >> 0x18;
    if (-1 < iVar3) {
      if (iVar3 < 3) {
        iVar3 = FUN_801ad38c(1,1,0,0);
        if (iVar3 != 0) {
          iVar2 = (int)FUN_801ad284(iVar3);
          ti = (uint8_t *)guest_at((uint32_t)iVar2);
          FUN_801ad240(iVar3,GU32(p1, 0x20) + 0x124,
                       (uint32_t)param_1,guest_of(g_DAT_801f0760));
          uVar1 = *param_3;
          *(uint32_t *)(ti + 0x18) = 0x80140710;
          *(uint32_t *)(ti + 0x1c) = 0;
          *(int *)(ti + 0x14) = (int)((uint)uVar1 << 0x10) >> 0x18;
        }
      }
      else if ((iVar3 == 3) &&
               (iVar3 = FUN_801ad38c(1,1,GU8(p1,2) + 1,0), iVar3 != 0)) {
        iVar2 = (int)FUN_801ad284(iVar3);
        ti = (uint8_t *)guest_at((uint32_t)iVar2);
        FUN_801ad260(iVar3,(uint32_t)param_1,
                     guest_of(g_DAT_1f8000e8 + (GU8(p1,2) ^ 1) * 0x108),
                     (uint32_t)(GU8(p1,2) + 1),
                     guest_of(g_DAT_801f0760));
        *(uint32_t *)(ti + 0x14) =
             (uint32_t)GU8((uint8_t *)guest_at(GU32(p1, 0x38)), 0x100);
        if (*(int8_t *)(p1 + 3) == 3) {
          *(uint32_t *)(ti + 0x18) = 0x1f800000;
          *(int *)(ti + 0x1c) = (int)*g__DAT_801f093a;
          iVar3 = (int)*g__DAT_801f093e;
          *(uint32_t *)(ti + 0x24) = 0x8013f178;
          *(int *)(ti + 0x20) = iVar3;
        }
      }
    }
  }
  return param_3 + 1;
}

static uint8_t *w22(uint8_t *c, uint8_t *m)
{
    return (uint8_t *)FUN_80141b78(
        (int)guest_of(c), 0, (ushort *)guest_at(GU32(m, 0x84)));
}

/* FUN_80141d2c — EV23: event/sound/effect command dispatcher.
 * Consumes a 4-byte record {u8 op, u8 arg, u16 param}; returns pc+4. */
ushort * FUN_80141d2c(int param_1,undefined4 param_2,ushort *param_3)
{
  ushort uVar1;
  undefined2 uVar2;
  undefined2 *puVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined2 uStack_18;
  undefined2 uStack_16;
  undefined2 uStack_14;
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
      FUN_80140710(param_1,uVar6,(int)(uVar7 << 0x10) >> 0x18);
      break;
    case 1:
      FUN_80138a24(GU8(p1,2),uVar7 & 0xff,(int)(uVar7 << 0x10) >> 0x18);
      break;
    case 2:
      FUN_8013f178((undefined4 *)guest_at(GU32(p1, 0x20) + 0x124),
                   *(short *)(p1 + 0x4e) + -0x400,
                   (int)(uVar7 << 0x10) >> 0x18,uVar7 & 0xff);
      break;
    case 3:
      FUN_8013f178((undefined4 *)guest_at(GU32(p1, 0x20) + 0x124),
                   *(short *)(p1 + 0x4e) + 0x400,
                   (int)(uVar7 << 0x10) >> 0x18,uVar7 & 0xff);
      break;
    case 4:
      FUN_8013f5f8(param_1);
      break;
    case 5:
      FUN_80140b18(param_1,(int)(short)uVar1);
      break;
    case 6:
      FUN_8013da2c(param_1,(int)(short)uVar1);
      break;
    case 7:
      if (*(int8_t *)((uint8_t *)guest_at(*g__DAT_1f8000c4) + 2) != 0) {
        FUN_8013ab8c(param_1,(int)(uVar7 << 0x10) >> 0x18,uVar7 & 0xff);
      }
      break;
    case 8:
      FUN_8013b5a8(param_1,(int)(short)uVar1);
      break;
    case 9:
      uVar7 = (uint)(short)uVar1;
      mo = (uint8_t *)guest_at(GU32(p1, 0x38));
      if (*(int8_t *)(mo + 0x100) != 0) {
        uVar7 = (uint)g_DAT_801b180c[uVar7];
      }
      iVar5 = FUN_801ad38c(1,1,0,0xf);
      if (iVar5 != 0) {
        FUN_801ad224(iVar5,GU32(p1, 0x20) + uVar7 * 0x50 + 0x34,
                     guest_of(g_DAT_801f0760));
      }
      break;
    case 10:
      FUN_80141050(GU8(p1,3),(int)(short)uVar1);
      break;
    case 0xb:
      FUN_80141050((int)(GU32(p1, 0x20) + (uint32_t)((int16_t)uVar1 * 0x50) + 0x34));
      break;
    case 0xc: {
      uint8_t *obj;
      uVar6 = (int)(uVar7 << 0x10) >> 0x18 & 0x7f;
      mo = (uint8_t *)guest_at(GU32(p1, 0x38));
      if ((*(int8_t *)(mo + 0x100) != 0) &&
         (((int)(uVar7 << 0x10) >> 0x10 & 0x8000U) != 0)) {
        uVar6 = (uint)g_DAT_801b180c[uVar6];
      }
      iVar5 = uVar6 * 0x50;
      obj = (uint8_t *)guest_at(GU32(p1, 0x20));
      uStack_18 = *(undefined2 *)(obj + iVar5 + 0x34);
      uStack_16 = *(undefined2 *)(obj + iVar5 + 0x38);
      uStack_14 = *(undefined2 *)(obj + iVar5 + 0x3c);
      iVar5 = (uVar7 & 0xff) * 4;
      FUN_8013ed98(&uStack_18,
                   *(uint32_t *)((char *)g__DAT_801f0790 + iVar5),
                   *(uint32_t *)((char *)g__DAT_801f0780 + iVar5),0);
      break;
    }
    case 0xd:
      puVar3 = (undefined2 *)guest_at((uint32_t)FUN_8013b088(GU32(p1, 0x20) + 0x124,6,
                            0x80000010,2,guest_of(g_PTR_DAT_801b1d44)));
      if (puVar3 != (undefined2 *)0x0) {
        uVar2 = (undefined2)FUN_8018e4d0();
        *puVar3 = uVar2;
      }
      break;
    case 0xe:
      piVar4 = (int *)guest_at((uint32_t)FUN_8013b088(GU32(p1, 0x20) + 0x444,0xc,
                                   0x80000012,0x8e,guest_of(g_DAT_801b1d5c)));
      if (piVar4 != (int *)0x0) {
        *piVar4 = param_1;
        piVar4[1] = (int)guest_of(g_DAT_1f8000e8 + (GU8(p1,2) ^ 1) * 0x108);
      }
      break;
    case 0xf:
      FUN_80140ca0(1,param_1);
    }
  }
  return param_3 + 2;
}

static uint8_t *w23(uint8_t *c, uint8_t *m)
{
    return (uint8_t *)FUN_80141d2c(
        (int)guest_of(c), 0, (ushort *)guest_at(GU32(m, 0x84)));
}
#define EVWRAP(nm, fn) \
    static uint8_t *nm(uint8_t *c, uint8_t *m) \
    { return fn(c, m, (uint8_t *)guest_at(GU32(m, 0x84))); }
EVWRAP(w0,  ev0_branch)
static uint8_t *w1(uint8_t *c, uint8_t *m)
{ (void)c; return ev1_altstream(m, (uint8_t *)guest_at(GU32(m, 0x84))); }
EVWRAP(w2,  ev2_hitstun_gate)
EVWRAP(w3,  ev3_wait)
EVWRAP(w4,  ev4_branch)
static uint8_t *w5(uint8_t *c, uint8_t *m)
{
    return (uint8_t *)FUN_8017ae18(
        (int)guest_of(c), (int)guest_of(m),
        (ushort *)guest_at(GU32(m, 0x84)));
}
EVWRAP(w6,  ev6_guard_sync)
EVWRAP(w7,  ev7_position)
EVWRAP(w8,  ev8_motion_param)
EVWRAP(w9,  ev9_physics)
EVWRAP(w10, ev10_flags)
static uint8_t *w11(uint8_t *c, uint8_t *m)
{ (void)c; return (uint8_t *)guest_at(GU32(m, 0x84) + 2); }
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
