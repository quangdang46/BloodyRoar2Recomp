/*
 * vm.c — clean-room reference implementation of the two BR2 VMs.
 *
 * VM1 "script VM" (AI/move scripts in .ANI): 19 opcodes, decoded from
 * handlers @0x80182f3c..0x80184f28 (RE_NOTES.md, vmops asm dumps).
 * VM2 "event VM" (.ACT/.ANI event streams): 24 event types + nested
 * ISAs, dispatcher ev_stream_dispatch, tables 0x801b4e68/0x801226e4..
 * 0x8012294c.
 *
 * Faithful to guest semantics: s16 regs, u16 stream operands, s32 cmp.
 * All state is accessed by raw guest offsets — see types.h for the
 * struct view. Pointers are guest-space pointers (as in the recomp:
 * the guest RAM image is mapped into the host address space).
 *
 * Project-original clean-room code (MIT); encodes observed behaviour.
 */
#include <stdint.h>
#include <string.h>
#include "types.h"

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);

/* ---- guest-offset accessors ---- */
#define G16(p,o)   (*(const int16_t  *)((const uint8_t *)(p) + (o)))
#define GU16(p,o)  (*(const uint16_t *)((const uint8_t *)(p) + (o)))
#define GU32(p,o)  (*(const uint32_t *)((const uint8_t *)(p) + (o)))
#define GU8(p,o)   (((const uint8_t *)(p))[(o)])
#define GSU8(p,o)  (((uint8_t *)(p))[(o)])
#define GS16(p,o)  (*(int16_t  *)((uint8_t *)(p) + (o)))
#define GSU16(p,o) (*(uint16_t *)((uint8_t *)(p) + (o)))
#define GS32(p,o)  (*(int32_t  *)((uint8_t *)(p) + (o)))
#define GSU32(p,o) (*(uint32_t *)((uint8_t *)(p) + (o)))

/* ScriptVM ctx offsets (authoritative, from handler disasm) */
#define VX_WAIT    0x06
#define VX_AUX8    0x08
#define VX_FLAGS   0x0a
#define VX_PC      0x24
#define VX_CMP     0x38
#define VX_REGS    0x3c   /* s16 regs, index = nibble 0..15 */
#define VX_RETSTK  0x4c   /* u32 return-PC stack[8]         */
#define VX_RETD    0x6c   /* s16 return depth               */
#define VX_DBG     0x6e
#define VX_TRACE   0x78

/* FighterCtrl offsets */
#define FC_FOE     0x2c
#define FC_VTH     0x20
#define FC_ANI     0x30
#define FC_MOTION  0x38
#define FC_DISTX   0x74
#define FC_DISTY   0x76
#define FC_STATE0  0x58   /* u32 group AF  */
#define FC_STATE1  0x5c   /* u32 group PF  */
#define FC_STATE2  0x60   /* u32 group 2   */
#define FC_STATE3  0x64   /* u32 group KF  */
#define FC_STATE4  0x68   /* u32 group DF  */
#define FC_STATE5  0x6c   /* u32 group EF  */
#define FC_HP      0xac
#define FC_HPDISP  0xae
#define FC_MAXHP   0xb0
#define FC_ATKWIN1 0xb8
#define FC_ATKWIN2 0xba
#define FC_ATKWIN3 0xbc
#define FC_GAUGE   0xd6
#define FC_MODE5   0x05   /* u8 bit0 beast/variant */

/* MotionState offsets */
#define MO_STREAM  0x84
#define MO_FLAGT   0x94

#define VREG(c,i)    GS16(c, VX_REGS + (i) * 2)
#define SVREG(c,i,v) (GS16(c, VX_REGS + (i) * 2) = (int16_t)(v))
#define ST(s,o)      GU32(s, o)
#define SETCMP(c,v)  (GS32(c, VX_CMP) = (int32_t)(v))
#define SETPC(c,p)   (GSU32(c, VX_PC) = guest_of(p))
#define GETPC(c)     ((uint8_t *)guest_at(GU32(c, VX_PC)))

static uint8_t *foe_of(uint8_t *ctrl)
{
    uint32_t a = GU32(ctrl, FC_FOE);
    return a ? (uint8_t *)guest_at(a) : NULL;
}

/* RNG hook: host supplies 0..99 for weighted Bra. */
int br2_rand(void);

/* ================================================================== */
/* SCRIPT VM — operand source decoders                                 */
/* ================================================================== */

/* op05 Move extension byte @pc+2 -> field source (sel 15 path) */
static int32_t move_ext(uint8_t *ctx, uint8_t *ctrl, uint8_t *a3obj,
                        int e)
{
    uint8_t *foe = foe_of(ctrl);
    switch (e) {
    case 1: return G16(ctrl, FC_DISTX);
    case 2: return G16(ctrl, FC_DISTY);
    case 3: return GU16(ctx, VX_AUX8);
    case 4: return GU16(a3obj, 0x94) & 0x7fff;
    case 5: case 6: {
        uint8_t *w = (e == 5) ? ctrl : foe;
        int32_t g = G16(w, FC_GAUGE);
        return g >= 0 ? g >> 6 : (g + 0x3f) >> 6;
    }
    case 7: {   /* self damage % = (maxHP-hp)*100/maxHP */
        int32_t d = G16(ctrl, FC_MAXHP) - G16(ctrl, FC_HP);
        return (int32_t)((int64_t)d * 100 / G16(ctrl, FC_MAXHP));
    }
    case 8: {   /* foe damage % / SELF maxHP (guest quirk, kept) */
        int32_t d = G16(foe, FC_MAXHP) - G16(foe, FC_HP);
        return (int32_t)((int64_t)d * 100 / G16(ctrl, FC_MAXHP));
    }
    case 9: case 10: {  /* obj+0x178 - own +0x94 */
        uint8_t *w = (e == 9) ? ctrl : foe;
        uint8_t *obj = (uint8_t *)guest_at(GU32(w, FC_VTH));
        return (int32_t)(GU32(obj, 0x178) - G16(w, 0x94));
    }
    case 11: case 12: { /* red-bar % = (hp-hpdisp)*100/maxHP */
        uint8_t *w = (e == 11) ? ctrl : foe;
        int32_t d = G16(w, FC_HP) - G16(w, FC_HPDISP);
        return (int32_t)((int64_t)d * 100 / G16(w, FC_MAXHP));
    }
    }
    return 0;
}

/* op05 Move src: 0=0, 1-6=r1-r6, 7=imm16(len4), 8-14=literal,
 * 15=ext-byte field source (len4). */
static int32_t move_src(uint8_t *ctx, uint8_t *ctrl, uint8_t *a3obj,
                        uint8_t *pc, int src, int *len)
{
    *len = 2;
    if (src == 0) return 0;
    if (src <= 6) return VREG(ctx, src);
    if (src == 7) { int16_t v = G16(pc, 2); *len = 4; return v; }
    if (src <= 14) return src;
    return move_ext(ctx, ctrl, a3obj, GU8(pc, 2));
}

/* op06 Cmp src (tbl 0x8012367c): 0=0,1-5=r,6-11&13=lit,12=foe gauge,
 * 14=foe(maxHP-hp),15=imm16(len4). */
static int32_t cmp_src(uint8_t *ctx, uint8_t *ctrl, uint8_t *pc,
                       int src, int *len)
{
    uint8_t *foe;
    *len = 2;
    if (src == 0) return 0;
    if (src <= 5) return VREG(ctx, src);
    foe = foe_of(ctrl);
    if (src == 12) return G16(foe, FC_GAUGE);
    if (src == 14) return G16(foe, FC_MAXHP) - G16(foe, FC_HP);
    if (src == 15) { int16_t v = G16(pc, 2); *len = 4; return v; }
    return src;   /* 6-11,13 literal */
}
/* op06 Cmp dst: 0=0,1-5=r,6-14=lit,15=imm16(len4, read after src) */
static int32_t cmp_dst(uint8_t *ctx, uint8_t *pc, int dst, int *len)
{
    *len = 0;
    if (dst == 0) return 0;
    if (dst <= 5) return VREG(ctx, dst);
    if (dst == 15) { int16_t v = G16(pc, *len); *len = 2; return v; }
    return dst;
}

/* op07 Add src (tbl 0x80123714): 0=0,1-5=r,6=lit6,7=distX,8=distY,
 * 9=ctx+8,10=a3+0x94&7fff,11=self gauge,12=foe gauge,
 * 13=self(maxHP-hp),14=foe(maxHP-hp),15=imm16(len4). */
static int32_t add_src(uint8_t *ctx, uint8_t *ctrl, uint8_t *a3obj,
                       uint8_t *pc, int src, int *len)
{
    uint8_t *foe = foe_of(ctrl);
    *len = 2;
    if (src == 0) return 0;
    if (src <= 5) return VREG(ctx, src);
    switch (src) {
    case 6: return 6;
    case 7: return G16(ctrl, FC_DISTX);
    case 8: return G16(ctrl, FC_DISTY);
    case 9: return GU16(ctx, VX_AUX8);
    case 10: return GU16(a3obj, 0x94) & 0x7fff;
    case 11: return G16(ctrl, FC_GAUGE);
    case 12: return G16(foe, FC_GAUGE);
    case 13: return G16(ctrl, FC_MAXHP) - G16(ctrl, FC_HP);
    case 14: return G16(foe, FC_MAXHP) - G16(foe, FC_HP);
    case 15: { int16_t v = G16(pc, 2); *len = 4; return v; }
    }
    return src;
}

/* ops 08/09/10 simple src: 0=0,1-5=r,6-14=literal,15=imm16(len4) */
static int32_t simple_src(uint8_t *ctx, uint8_t *pc, int src, int *len)
{
    *len = 2;
    if (src == 0) return 0;
    if (src <= 5) return VREG(ctx, src);
    if (src == 15) { int16_t v = G16(pc, 2); *len = 4; return v; }
    return src;
}

/* ================================================================== */
/* Chk conditions (op04)                                                */
/* cond byte values observed: c00-c2c + c32-c39. Self=even, foe=odd.   */
/* ================================================================== */

/* c00-c0e locomotion/state bits on ctrl+0x58 (u32 group AF) */
static int chk_state(uint8_t *t, int which)
{
    uint32_t st = ST(t, FC_STATE0);
    switch (which) {
    case 0x00: return (st & 0x8000)    != 0;   /* damage  */
    case 0x02: return (st & 0x10000000)!= 0;   /* guard   */
    case 0x04: return (st & 0x800)     != 0;   /* air     */
    case 0x06: return (st & 0x800)     == 0;   /* ground  */
    case 0x08: return (st & 0x80000000)!= 0;   /* attack  */
    case 0x0a: return (st & 0x1)       != 0;   /* stand   */
    case 0x0c: return (st & 0x10)      != 0;   /* sit     */
    case 0x0e: return (st & 0x20)      != 0;   /* fuse    */
    }
    return 0;
}

/* Evaluate one Chk condition. Returns 1/0 and sets *len (insn size).
 * Mask conds c32-c39 consume a u32 mask at pc+2 (6B total). */
static int chk_eval(uint8_t *ctrl, uint8_t *pc,
                    int cond, int *len)
{
    uint8_t *self = ctrl, *foe = foe_of(ctrl);
    uint8_t *t = (cond & 1) ? foe : self;
    int32_t d;
    *len = 2;

    if (cond >= 0x20 && cond < 0x28) {
        /* c32-c39: bitmask test on state groups KF/PF/AF/DF
         * fields +0x64,+0x5c,+0x58,+0x68 by pair index */
        static const int fld[4] = { FC_STATE3, FC_STATE1,
                                    FC_STATE0, FC_STATE4 };
        uint32_t mask = GU16(pc,2) | ((uint32_t)GU16(pc,4) << 16);
        *len = 6;
        return (ST(t, fld[(cond - 0x20) >> 1]) & mask) != 0;
    }
    if (cond == 0x28 || cond == 0x29) {          /* e_Back */
        /* |distX| >= 0x6ab ; odd = foe distance? approximate self */
        d = G16(ctrl, FC_DISTX); if (d < 0) d = -d;
        return d >= 0x6ab;
    }
    if (cond >= 0x2b && cond <= 0x2c) {          /* Cancel compound */
        return (ST(t,FC_STATE0)&0x80000000) && G16(t,FC_ATKWIN2) > 0
            && !(ST(t,FC_STATE1)&0x00040080)
            && (ST(t,FC_STATE3)&0x40002000)==0x40002000;
    }
    if (cond >= 0x2d) {                          /* default: +0x58&0x8000 */
        return (ST(t, FC_STATE0) & 0x8000) != 0;
    }
    switch (cond & 0x7e) {
    case 0x00: case 0x02: case 0x04: case 0x06:
    case 0x08: case 0x0a: case 0x0c: case 0x0e:
        return chk_state(t, cond & 0x7e);
    case 0x10: return (ST(t,FC_STATE0)&0x80000000) && G16(t,FC_ATKWIN2)!=0;
    case 0x12: return (ST(t,FC_STATE0)&0x80000000) && G16(t,FC_ATKWIN2)==0
                     && G16(t,FC_ATKWIN1)!=0;
    case 0x14: return (ST(t,FC_STATE0)&0x80000000) && G16(t,FC_ATKWIN3)>0;
    case 0x16: return (GU8(t, FC_MODE5) & 1) != 0;          /* beast */
    case 0x18: return (ST(t,FC_STATE0)&0x80000000)
                     && (ST(t,FC_STATE5)&0x01000000)!=0;   /* combo0 */
    case 0x1a: return (ST(t,FC_STATE0)&0x80000000)
                     && (ST(t,FC_STATE5)&0x02000000)!=0;   /* combo1 */
    case 0x1c: return !(ST(t,FC_STATE0)&0x80000000)
                     || G16(t,FC_ATKWIN3)>0;
    case 0x1e: return !(GU8(t,FC_MODE5)&1) && G16(t,FC_GAUGE)>=0x1e00;
    }
    return 0;
}

/* ================================================================== */
/* Script VM step                                                       */
/* ================================================================== */

int br2_vm_step(uint8_t *ctx, uint8_t *ctrl, uint8_t *a3obj)
{
    uint8_t *pc = GETPC(ctx);
    uint8_t op = GU8(pc,0), sub = GU8(pc,1);
    int dst = sub >> 4, src = sub & 0xf;
    int len = 2, sl = 2, dl = 0;
    uint8_t *base = (uint8_t *)guest_at(GU32(ctrl, FC_ANI));
    uint8_t *t;
    int32_t v, sv, dv;

    switch (op) {
    case 0: case 13: case 14: case 15:
        return 0;                                       /* End */

    case 1: {                                           /* Bra */
        int16_t tgt = G16(pc,2);
        switch (sub) {
        case 0x00: t = base + tgt; break;
        case 0x01:                                      /* if/else 8B */
            t = base + (GS32(ctx,VX_CMP) > 0 ? tgt
                                          : G16(pc,6));
            len = 8; break;
        case 0x02: case 0x05:                           /* if cmp!=0 */
            if (GS32(ctx,VX_CMP)) { t = base+tgt; len=4; break; }
            SETPC(ctx, pc+4); return 1;
        case 0x03: case 0x04:                           /* if cmp==0 */
            if (!GS32(ctx,VX_CMP)) { t = base+tgt; len=4; break; }
            SETPC(ctx, pc+4); return 1;
        case 0x06: if (GS32(ctx,VX_CMP)<=0) goto bra_next;
                  t = base+tgt; len=4; break;
        case 0x07: if (GS32(ctx,VX_CMP)< 0) goto bra_next;
                  t = base+tgt; len=4; break;
        case 0x08: if (GS32(ctx,VX_CMP)>=0) goto bra_next;
                  t = base+tgt; len=4; break;
        case 0x09: if (GS32(ctx,VX_CMP)> 0) goto bra_next;
                  t = base+tgt; len=4; break;
        case 0x0a: {                                    /* weighted random */
            int n = GU8(pc,2), i, acc = 0, pick = br2_rand() % 100;
            uintptr_t tp;
            for (i = 0; i < n; i++) {
                acc += GU8(pc,3+i);
                if (pick < acc) break;
            }
            if (i >= n) i = n-1;
            tp = (uintptr_t)(pc + 3 + n + ((n & 1) ? 1 : 0));
            SETPC(ctx, base + GU16(tp, i*2));
            return 1;
        }
        case 0x0b:                                      /* if/else 6B */
            t = base + (GS32(ctx,VX_CMP) ? tgt : G16(pc,4));
            len = 6; break;
        default: t = base + tgt; break;
        }
        SETPC(ctx, t);
        return 1;
    bra_next:
        SETPC(ctx, pc + len);
        return 1;
    }

    case 2: {                                           /* Bsr: call */
        int depth = GS16(ctx, VX_RETD);
        int16_t tgt = G16(pc,2);
        if (depth < 8) {
            GSU32(ctx, VX_RETSTK + depth*4) = guest_of(pc + 4);
            GS16(ctx, VX_RETD) = depth + 1;
        }
        SETPC(ctx, base + tgt);
        return 1;
    }

    case 3: {                                           /* Ret */
        int depth = GS16(ctx, VX_RETD);
        if (depth <= 0) return 0;
        depth--;
        GS16(ctx, VX_RETD) = depth;
        SETPC(ctx, (uint8_t *)guest_at(GU32(ctx, VX_RETSTK + depth*4)));
        (void)sub;
        return 1;
    }

    case 4: {                                           /* Chk */
        SETCMP(ctx, chk_eval(ctrl, pc, sub, &len));
        SETPC(ctx, pc + len);
        return 1;
    }

    case 5:                                             /* Move */
        v = move_src(ctx, ctrl, a3obj, pc, src, &sl);
        SVREG(ctx, dst, v);
        SETPC(ctx, pc + sl);
        return 1;

    case 6: {                                           /* Cmp */
        sv = cmp_src(ctx, ctrl, pc, src, &sl);
        dv = cmp_dst(ctx, pc + sl, dst, &dl);
        SETCMP(ctx, dv - sv);
        SETPC(ctx, pc + sl + dl);
        return 1;
    }

    case 7:                                             /* Add */
        sv = add_src(ctx, ctrl, a3obj, pc, src, &sl);
        SVREG(ctx, dst,
              (int16_t)(uint16_t)((uint16_t)VREG(ctx,dst)
                                  + (uint16_t)sv));
        SETPC(ctx, pc + sl);
        return 1;

    case 8: case 9: case 10: {                          /* Sub/Mul/Div */
        sv = simple_src(ctx, pc, src, &sl);
        dv = VREG(ctx, dst);
        switch (op) {
        case 8:  v = dv - sv; break;
        case 9:  v = (int16_t)((int32_t)(int16_t)dv
                               * (int32_t)(int16_t)sv); break;
        default: v = sv ? dv / sv : 0; break;
        }
        SVREG(ctx, dst, v);
        SETPC(ctx, pc + sl);
        return 1;
    }

    case 11: {                                          /* Wait group */
        /* FUN_80183664 — 12 conds @0x80123414. Counter lives in
         * ctx+0x3c (r0 slot reused as wait counter). */
        int w = 0, len2 = 4;
        uint8_t *wfoe = foe_of(ctrl);
        switch (sub) {
        case 0: case 12: case 13: case 14: case 15:
            w = G16(pc,2); break;                       /* frame wait */
        case 1: {   /* random wait: 3 operand bytes via ctx+0 stream */
            uint32_t sbaseg = GU32(ctx, 0);
            uint8_t *sbase = sbaseg ? (uint8_t *)guest_at(sbaseg) : NULL;
            int idx = GU8(ctx, 4);
            int b0 = sbase ? GU8(sbase, idx)   : 0;
            int b1 = sbase ? GU8(sbase, idx+1) : 0;
            int b2 = sbase ? GU8(sbase, idx+2) : 0;
            w = (int)(((b0 + b1 + b2) * (0x100 - GU8(ctx,8))) >> 13);
            len2 = 2; break;
        }
        case 2: case 3: {   /* wait while self/foe +0x58<0 and
                               attack-window +0xba/+0xb8 active */
            uint8_t *t = (sub == 3) ? wfoe : ctrl;
            int busy = (ST(t,FC_STATE0) & 0x80000000) == 0
                    && (G16(t,FC_ATKWIN2) != 0 || G16(t,FC_ATKWIN1) != 0);
            w = busy ? 1 : 0; len2 = 2; break;
        }
        case 4: case 5: {   /* same via +0xbc */
            uint8_t *t = (sub == 5) ? wfoe : ctrl;
            int busy = (ST(t,FC_STATE0) & 0x80000000) == 0
                    && G16(t,FC_ATKWIN3) != 0;
            w = busy ? 1 : 0; len2 = 2; break;
        }
        case 6: case 7:     /* +0x58>=0 immediate-pass */
            w = 0; len2 = 2; break;
        case 8:             /* rand wait v2 */
            w = (int)(((uint32_t)GU16(pc,2) * (uint32_t)(br2_rand()&0xff)) >> 8) + 1;
            break;
        case 9: case 10: {  /* wait while ctrl+0x6c & 0x10000 */
            int on = (ST(ctrl,FC_STATE5) & 0x10000) != 0;
            w = (sub == 9) ? (on ? 1 : 0) : (on ? 0 : 1);
            len2 = 2; break;
        }
        case 11:            /* rand wait v3 + mirror u16 -> ctx+6 */
            GS16(ctx, VX_WAIT) = G16(pc,2);
            w = (int)(((uint32_t)GU16(pc,2) * (uint32_t)(br2_rand()&0xff)) >> 8) + 1;
            break;
        default: w = G16(pc,2); break;
        }
        VREG(ctx,0) = (int16_t)w;      /* ctx+0x3c wait counter */
        SETPC(ctx, pc + len2);
        return 1;
    }

    case 12: {                                          /* SFlg */
        uint16_t m = GU16(pc,2);
        if (sub) GS16(ctx, VX_FLAGS) |= m;
        else     GS16(ctx, VX_FLAGS) &= ~m;
        SETPC(ctx, pc + 4);
        return 1;
    }

    case 16: {                                          /* Pad: hold N */
        GS16(ctx, VX_WAIT) = G16(pc,2);
        GS16(ctx, VX_AUX8) = G16(pc,4);
        SETPC(ctx, pc + 6);
        return 1;
    }

    case 17:                                             /* GPad */
        SETPC(ctx, pc + 4);
        return 1;

    case 18: {                                          /* Camera */
        /* cam = *(void**)0x1F8000BC in guest space */
        uint32_t camg = GU32(guest_at(0x1f8000bc), 0);
        uint8_t *cam = camg ? (uint8_t *)guest_at(camg) : NULL;
        int csub = (int8_t)sub;
        if (csub < 0 || csub > 9 || !cam) {
            SETPC(ctx, pc + 2);
            return 1;
        }
        switch (csub) {
        case 0: GS16(cam,0x76)=G16(pc,2); len=4; break;
        case 1:
            GS16(cam,0x52)=G16(pc,2); GS16(cam,0x54)=G16(pc,4);
            GS16(cam,0x56)=G16(pc,6); len=8; break;
        case 2:
            GS16(cam,0x30)=G16(pc,2); GS16(cam,0x32)=G16(pc,4);
            GS16(cam,0x8a)=G16(pc,4); GS16(cam,0x34)=G16(pc,6);
            len=8; break;
        case 3:
            GS16(cam,0x64)=G16(pc,2); GS16(cam,0x66)=G16(pc,4);
            GS16(cam,0x68)=G16(pc,6); len=8; break;
        case 4:
            GS16(cam,0x42)=G16(pc,2); GS16(cam,0x44)=G16(pc,4);
            GS16(cam,0x46)=G16(pc,6); len=8; break;
        case 5: {                                       /* per-slot byte */
            uint8_t slot = GU8(pc,2);
            uint8_t val  = GU8(pc,3);
            GSU8(cam, 0x80 + slot) = GU8(ctrl, slot);
            GSU8(cam, 0x80 + slot) = val;   /* guest writes foe's? keep */
            len = 4; break;
        }
        case 6: GS16(cam,0x5a)=G16(pc,2); len=4; break;
        case 7: GS16(cam,0x38)=G16(pc,2); len=4; break;
        case 8: GS16(cam,0x6c)=G16(pc,2); len=4; break;
        case 9: {                                       /* preset vector */
            uint16_t idx = GU16(pc,2);
            const uint8_t *tbl = (const uint8_t *)guest_at(0x8011f944);
            GS16(cam,0x7e)=idx;
            GS16(cam,0x4a)=G16(tbl, idx*8+0);
            GS16(cam,0x4c)=G16(tbl, idx*8+2);
            GS16(cam,0x4e)=G16(tbl, idx*8+4);
            len=4; break;
        }
        }
        SETPC(ctx, pc + len);
        return 1;
    }
    }
    SETPC(ctx, pc + 2);
    return 1;
}

/* Guest step loop — func_0x80184f28. Runs up to 50 insns per call,
 * stops early when an insn handler returns 0 (End / Ret-underflow).
 * ctx+0x6e trace flag gates per-op logging (host hook br2_trace_op). */
void br2_trace_op(uint8_t *ctx);
int br2_vm_run(uint8_t *ctx, uint8_t *ctrl, uint8_t *a3obj)
{
    int n = 0;
    for (;;) {
        if (GU16(ctx, VX_DBG))
            br2_trace_op(ctx);              /* optional host hook */
        if (!br2_vm_step(ctx, ctrl, a3obj))
            return 0;
        if (++n >= 0x32)
            return 1;
    }
}
