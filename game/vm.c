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
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);

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
#define VX_ENDW    0x28   /* u16 cleared by the End epilogue */

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
    /* ScriptVM source decoders lw ctrl+0x2c and dereference it with no
     * zero test; guest_at(0) reproduces the low-memory read. */
    return (uint8_t *)guest_at(GU32(ctrl, FC_FOE));
}

/* RNG hook: host supplies 0..99 for weighted Bra. */
int host_rand(void);

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

int vm_step(uint8_t *ctx, uint8_t *ctrl, uint8_t *a3obj)
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
            int n = GU8(pc,2), i, acc = 0, pick = host_rand() % 100;
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
            uint8_t *sbase = (uint8_t *)guest_at(sbaseg);
            int idx = GU8(ctx, 4);
            int b0 = GU8(sbase, idx);
            int b1 = GU8(sbase, idx+1);
            int b2 = GU8(sbase, idx+2);
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
            w = (int)(((uint32_t)GU16(pc,2) * (uint32_t)(host_rand()&0xff)) >> 8) + 1;
            break;
        case 9: case 10: {  /* wait while ctrl+0x6c & 0x10000 */
            int on = (ST(ctrl,FC_STATE5) & 0x10000) != 0;
            w = (sub == 9) ? (on ? 1 : 0) : (on ? 0 : 1);
            len2 = 2; break;
        }
        case 11:            /* rand wait v3 + mirror u16 -> ctx+6 */
            GS16(ctx, VX_WAIT) = G16(pc,2);
            w = (int)(((uint32_t)GU16(pc,2) * (uint32_t)(host_rand()&0xff)) >> 8) + 1;
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
        uint8_t *cam = (uint8_t *)guest_at(camg);
        int csub = (int8_t)sub;
        if (csub < 0 || csub > 9) {
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
 * ctx+0x6e trace flag gates per-op logging (host hook trace_op). */
void trace_op(uint8_t *ctx);
int vm_run(uint8_t *ctx, uint8_t *ctrl, uint8_t *a3obj)
{
    int n = 0;
    for (;;) {
        if (GU16(ctx, VX_DBG))
            trace_op(ctx);              /* optional host hook */
        if (!vm_step(ctx, ctrl, a3obj))
            return 0;
        if (++n >= 0x32)
            return 1;
    }
}

/* ================================================================== */
/* ScriptVM opcode handlers — faithful ports of the guest functions     */
/* behind the opcode jump table @0x801b5528 (19 entries). Each handler  */
/* is invoked by script_op_dispatch (0x80184f28) as h(a0,a1,ctx,a3) and */
/* returns the s16/v0 continuation flag: 0 = script over, 1 = continue.  */
/* ctx arrives as a2 (gpr[6]); the clean_table wrapper guest_at()s it.  */
/* ================================================================== */

/* Shared decode for ops 08/09/0a (Sub/Mul/Div): insn word at ctx->pc,
 * sub = hi byte, dst = sub>>4, src = sub&0xf.
 *   src 0     -> sv = 0,            pc NOT advanced (guest hangs; kept)
 *   src 1..5  -> sv = s16 reg[src], pc += 2
 *   src 6..14 -> sv = src literal,  pc NOT advanced (guest hangs; kept)
 *   src 15    -> sv = s16 imm @pc+2, pc += 4
 * Returns sv and writes the guest pc advance (0 = leave ctx->pc alone). */
static int32_t arith_src(uint8_t *ctx, uint8_t *pc, int src, uint32_t *adv)
{
    *adv = 0;
    if (src >= 1 && src <= 5) {
        *adv = 2;
        return VREG(ctx, src);              /* lh: sign-extended s16 */
    }
    if (src == 15) {
        *adv = 4;
        return G16(pc, 2);                  /* lh imm16 */
    }
    return src;                             /* 0 -> 0, 6..14 -> literal */
}

/* op00 End — guest 0x80182f3c (also the Ret-underflow tail 0x801835c4):
 *   sh zero, 0x28(a2);  v0 = 0;  jr ra */
int scriptop_end(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    (void)a0; (void)a1; (void)a3;
    GS16(ctx, VX_ENDW) = 0;
    return 0;
}

/* op03 Ret/Brr — guest 0x80183558. sub<10 = conditional return on
 * ctx->cmp (s32 @0x38); sub 0/1 and sub>=10 = unconditional.
 * Conditional-fail path: ctx->pc += 2, return 1.
 * Unconditional: pop return stack; depth 0 runs the End epilogue
 * (ctx+0x28 = 0, return 0); depth>0 resumes at retstk[depth-1]. */
int scriptop_ret(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    int32_t cmp;
    int pop;
    int d;
    (void)a0; (void)a1; (void)a3;

    if (sub < 10) {
        cmp = GS32(ctx, VX_CMP);
        switch (sub) {
        case 0: case 1: pop = 1; break;
        case 2: case 5: pop = cmp != 0; break;
        case 3: case 4: pop = cmp == 0; break;
        case 6:          pop = cmp >  0; break;
        case 7:          pop = cmp >= 0; break;
        case 8:          pop = cmp <  0; break;
        default:         pop = cmp <= 0; break;      /* sub 9 */
        }
        if (!pop) {
            GSU32(ctx, VX_PC) = pcg + 2;
            return 1;
        }
    }
    /* unconditional depth pop (guest block 0x80183594):
     * lh s16 RETD; RETD==0 -> epilogue 0x801835C4 (ctx+0x28=0, v0=0).
     * Else RETD-- (s16), PC = RETSTK[(RETD-1) & 0xffff], return 1.
     * The previous port tested depth-1==0, i.e. ended at RETD==1 and
     * never popped slot 0; at RETD==0 it stored RETD=-1 and read
     * RETSTK[0xffff] -> VX_PC = garbage (observed 0 -> opcode 0xCC
     * -> DISPATCH FATAL at frame ~1880). */
    d = (int32_t)G16(ctx, VX_RETD);
    if (d == 0) {                    /* depth 0 -> script over */
        GS16(ctx, VX_ENDW) = 0;
        return 0;
    }
    d = d - 1;
    GS16(ctx, VX_RETD) = (int16_t)d;
    GSU32(ctx, VX_PC) = GU32(ctx, VX_RETSTK + ((uint32_t)d & 0xffff) * 4);
    return 1;
}

/* op0c SFlg — guest 0x80183bd0. sub==1 -> flags &= mask; else flags |= mask.
 * (vm.c's earlier model had set/clear inverted; this is the guest truth.) */
int scriptop_sflag(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    uint16_t m = G16(pc, 2);
    uint16_t f = GU16(ctx, VX_FLAGS);
    int sub = GU8(pc, 1);
    (void)a0; (void)a1; (void)a3;
    GS16(ctx, VX_FLAGS) = (int16_t)((sub == 1) ? (f & m) : (f | m));
    GSU32(ctx, VX_PC) = pcg + 4;
    return 1;
}

/* op08 Sub — guest 0x80184810. dst read ZERO-extended (lhu),
 * src sign-extended/literal; res = dst - src stored s16. */
int scriptop_sub(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    int dst = sub >> 4, src = sub & 0xf;
    uint32_t adv;
    int32_t sv = arith_src(ctx, pc, src, &adv);
    uint32_t dv = GU16(ctx, VX_REGS + dst * 2);   /* lhu: zero-extend */
    (void)a0; (void)a1; (void)a3;
    if (adv) GSU32(ctx, VX_PC) = pcg + adv;
    GS16(ctx, VX_REGS + dst * 2) = (int16_t)(dv - (uint32_t)sv);
    return 1;
}

/* op09 Mul — guest 0x80184890. dst read SIGN-extended (lh),
 * res = (s16)dst * sv, low 16 stored. */
int scriptop_mul(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    int dst = sub >> 4, src = sub & 0xf;
    uint32_t adv;
    int32_t sv = arith_src(ctx, pc, src, &adv);
    int32_t dv = G16(ctx, VX_REGS + dst * 2);     /* lh: sign-extend */
    (void)a0; (void)a1; (void)a3;
    if (adv) GSU32(ctx, VX_PC) = pcg + adv;
    GS16(ctx, VX_REGS + dst * 2) = (int16_t)((uint32_t)(dv * sv));
    return 1;
}

/* op0a Div — guest 0x80184914. dst read SIGN-extended (lh).
 * div-by-zero yields the MIPS LO result: dv>=0 -> -1, else 1
 * (generated: lo = dv>=0 ? 0xFFFFFFFF : 1). */
int scriptop_div(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    int dst = sub >> 4, src = sub & 0xf;
    uint32_t adv;
    int32_t sv = arith_src(ctx, pc, src, &adv);
    int32_t dv = G16(ctx, VX_REGS + dst * 2);     /* lh: sign-extend */
    int32_t res;
    (void)a0; (void)a1; (void)a3;
    if (adv) GSU32(ctx, VX_PC) = pcg + adv;
    if (sv != 0) res = dv / sv;
    else         res = (dv >= 0) ? -1 : 1;
    GS16(ctx, VX_REGS + dst * 2) = (int16_t)res;
    return 1;
}

/* ---- guest-space scalar loads (a0/a3 arrive as guest addresses) ---- */
static int32_t  gld_s16(uint32_t g) { return *(const int16_t  *)guest_at(g); }
static uint32_t gld_u16(uint32_t g) { return *(const uint16_t *)guest_at(g); }
static uint32_t gld_u32(uint32_t g) { return *(const uint32_t *)guest_at(g); }
static uint32_t gld_u8 (uint32_t g) { return *(const uint8_t  *)guest_at(g); }
extern void br2_st8 (uint32_t a, uint8_t  v);
extern void br2_st16(uint32_t a, uint16_t v);
extern void br2_st32(uint32_t a, uint32_t v);

/* op01 Cmp — guest 0x80184378. sub = hi byte of insn: dst = sub>>4,
 * src = sub&0xf. src dispatches through table @0x8012367C indexed by
 * (src-1): src1-5 -> s16 reg[src]; src6-11 & src13 -> literal src;
 * src12 -> s16 foe->gauge; src14 -> s16 foe->maxHP - s16 foe->hp;
 * src15 -> s16 imm @pc+2 (runpc += 2). cmp = dv - sv stored s32
 * @ctx+0x38; returns 1.
 *   dst 1..5 -> dv = s16 reg[dst], pc = runpc + 2
 *   dst 15   -> dv = s16 imm @runpc+2, pc = runpc + 4
 *   dst 6..14-> dv = dst literal, pc untouched (guest hangs; kept)
 *   dst 0    -> cmp = -sv, pc untouched (guest hangs; kept)
 * Note: the guest table has 15 entries covering src 1..15; src==0 skips
 * it and uses sv = 0 literal. */
int scriptop_cmp(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    uint32_t foe_g = gld_u32(a0 + FC_FOE);
    int sub = GU8(pc, 1);
    int dst = sub >> 4, src = sub & 0xf;
    int32_t sv = 0, dv = 0;
    uint32_t runpc = pcg, npc = 0;
    int have_pc = 0;
    (void)a1; (void)a3;

    switch (src) {
    case 1: case 2: case 3: case 4: case 5:
        sv = VREG(ctx, src); break;
    case 12: sv = gld_s16(foe_g + FC_GAUGE); break;
    case 14: sv = gld_s16(foe_g + FC_MAXHP) - gld_s16(foe_g + FC_HP); break;
    case 15: sv = gld_s16(runpc + 2); runpc += 2; break;
    default: sv = src; break;
    }

    if (dst >= 1 && dst <= 5) {
        dv = VREG(ctx, dst);
        npc = runpc + 2; have_pc = 1;
    } else if (dst == 15) {
        dv = gld_s16(runpc + 2);
        npc = runpc + 4; have_pc = 1;
    } else {
        dv = dst;                    /* 0 or 6..14: literal, no pc */
    }

    if (have_pc) GSU32(ctx, VX_PC) = npc;
    SETCMP(ctx, dv - sv);
    return 1;
}

/* op07 Add — guest 0x80184710. reg[dst] = (u16)reg[dst] + sv, sh.
 *   src 1..5  -> sv = s16 reg[src],            pc += 2
 *   src 7     -> sv = s16 ctrl->distX,         pc += 2
 *   src 8     -> sv = s16 ctrl->distY,         pc += 2
 *   src 9     -> sv = u16 ctx+8 (VX_AUX8),     pc += 2
 *   src 10    -> sv = u16 a3obj+0x94 & 0x7fff, pc += 2
 *   src 11    -> sv = s16 ctrl->gauge,         pc += 2
 *   src 12    -> sv = s16 foe->gauge,          pc += 2
 *   src 13    -> sv = s16 ctrl->maxHP - ctrl->hp,  pc += 2
 *   src 14    -> sv = s16 foe->maxHP - foe->hp,    pc += 2
 *   src 15    -> sv = s16 imm @pc+2,           pc += 4
 *   src 0/6   -> sv = 0 / 6 literal, pc untouched (guest hangs; kept)
 * ctrl = param_1 (a0), a3obj = param_4 (a3). */
int scriptop_add(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    uint32_t foe_g = gld_u32(a0 + FC_FOE);
    int sub = GU8(pc, 1);
    int dst = sub >> 4, src = sub & 0xf;
    int32_t sv = 0;
    uint32_t npc = 0;
    int have_pc = 0;
    (void)a1;

    switch (src) {
    case 1: case 2: case 3: case 4: case 5:
        sv = VREG(ctx, src); npc = pcg + 2; have_pc = 1; break;
    case 7:  sv = gld_s16(a0 + FC_DISTX); npc = pcg + 2; have_pc = 1; break;
    case 8:  sv = gld_s16(a0 + FC_DISTY); npc = pcg + 2; have_pc = 1; break;
    case 9:  sv = (int32_t)GU16(ctx, VX_AUX8); npc = pcg + 2; have_pc = 1; break;
    case 10: sv = (int32_t)(gld_u16(a3 + MO_FLAGT) & 0x7fff);
             npc = pcg + 2; have_pc = 1; break;
    case 11: sv = gld_s16(a0 + FC_GAUGE); npc = pcg + 2; have_pc = 1; break;
    case 12: sv = gld_s16(foe_g + FC_GAUGE); npc = pcg + 2; have_pc = 1; break;
    case 13: sv = gld_s16(a0 + FC_MAXHP) - gld_s16(a0 + FC_HP);
             npc = pcg + 2; have_pc = 1; break;
    case 14: sv = gld_s16(foe_g + FC_MAXHP) - gld_s16(foe_g + FC_HP);
             npc = pcg + 2; have_pc = 1; break;
    case 15: sv = gld_s16(pcg + 2); npc = pcg + 4; have_pc = 1; break;
    default: sv = src; break;      /* 0 -> 0, 6 -> 6: literal, no pc */
    }

    if (have_pc) GSU32(ctx, VX_PC) = npc;
    GS16(ctx, VX_REGS + dst * 2) =
        (int16_t)(uint16_t)(GU16(ctx, VX_REGS + dst * 2) + (uint32_t)sv);
    return 1;
}

/* ================================================================== */
/* op01 Bra — guest 0x80182F48. sub = insn byte1. Jump table          */
/* @0x801232E4 covers sub 0..11; sub>=12 takes the unconditional       */
/* default (pc = base + u16@pc+2). base = ctrl+0x30 (FC_ANI). The      */
/* ctx+0x6e trace ring (ctx+0x78, call 0x8018EF60) is debug-only and   */
/* omitted here exactly like the other ported handlers. All paths      */
/* return 1.                                                          */
/* ================================================================== */

/* Stream random byte: *(u8*)(ctx->stream + (ctx->idx & 0xff)), then
 * ctx->idx = idx+1 (sb). Used by the weighted-Bra pick. */
static uint32_t vm_stream_byte(uint8_t *ctx)
{
    uint32_t base = GU32(ctx, 0x00);
    uint32_t idx  = GU8(ctx, 0x04);
    GSU8(ctx, 0x04) = (uint8_t)(idx + 1);
    return gld_u8(base + (idx & 0xff));
}

/* Weighted pick shared by Bra sub10 / Bsr sub6: n = u8@pc+2 weight
 * bytes at pc+3.., target table at pc + ((n+4) & 0x1fe), u16 entries.
 * pick = first index whose cumulative weight exceeds
 * X = ((stream_byte * sum) >> 8) - w[0] logic per guest blocks
 * 0x8018310C..0x801831C8. n==0 is degenerate (guest reads an
 * uninitialised stack slot); real scripts never emit it — treated as
 * the n==0 tail (pc + ((3n+4)&0x7fe) = pc+4 absolute). */
static uint32_t bra_random_pick(uint8_t *ctx, uint8_t *pc, int n)
{
    /* guest 0x8018310C..0x801831DC: n weight bytes @pc+3 are stored as
     * u16 on the stack; X = ((stream_byte*sum)>>8) - w[0]. pick counts
     * how many times the SECOND weight w[1] must be subtracted before
     * X goes negative — the guest loop re-reads the same stack slot
     * every iteration (a1 = sp+18 never advances, @0x80183190-0x801831a0;
     * v1++ sits in the bgez delay slot so it counts the final subtract).
     * For n<2 the guest reads an uninitialised stack slot; GU8(pc,4) is
     * the deterministic stream-byte approximation. n==0 leaves w[0]
     * uninitialised too — treated as pick 0 (real scripts never emit it). */
    int32_t sum = 0;
    int i, pick = 0;
    int32_t a0v;
    uint32_t b;

    for (i = 0; i < n; i++)
        sum += GU8(pc, 3 + i);
    b = vm_stream_byte(ctx);
    /* mult lo = b*sum (both small); sra 8 */
    a0v = (int32_t)(((int64_t)b * sum) >> 8) - (n > 0 ? GU8(pc, 3) : 0);
    if (n > 0 && a0v >= 0) {
        int32_t w1 = GU8(pc, 4);
        do {
            a0v -= w1;
            pick++;
        } while (a0v >= 0);
    }
    return GU16(pc, ((n + 4) & 0x1fe) + pick * 2);
}

/* Conditional-jump target evaluation for Bra (op01, table
 * @0x801232E4). Returns the new guest pc. sub is the raw insn byte1.
 * cmp = ctx+0x38 s32. */
static uint32_t bra_target(uint8_t *ctx, uint8_t *pc, uint32_t pcg,
                           int sub, uint32_t base_g, int32_t cmp)
{
    switch (sub) {
    case 0:                                     /* unconditional */
        return base_g + GU16(pc, 2);
    case 1: {                                   /* if/else 8B */
        uint32_t sel;
        if (cmp > 0) sel = GU16(pc, 2);
        else { GS32(ctx, VX_CMP) = 0; sel = GU16(pc, 6); }
        /* guest tail 0x80183034: sel==0 -> pc+8 absolute, no base add */
        return sel ? base_g + sel : pcg + 8;
    }
    case 2: case 5:                             /* jump if cmp != 0 */
        return cmp == 0 ? pcg + 4 : base_g + GU16(pc, 2);
    case 3: case 4:                             /* jump if cmp == 0 */
        return cmp != 0 ? pcg + 4 : base_g + GU16(pc, 2);
    case 6:                                     /* jump if cmp > 0 */
        return cmp <= 0 ? pcg + 4 : base_g + GU16(pc, 2);
    case 7:                                     /* jump if cmp >= 0 */
        return cmp <  0 ? pcg + 4 : base_g + GU16(pc, 2);
    case 8:                                     /* jump if cmp < 0 */
        return cmp >= 0 ? pcg + 4 : base_g + GU16(pc, 2);
    case 9:                                     /* jump if cmp <= 0 */
        return cmp >  0 ? pcg + 4 : base_g + GU16(pc, 2);
    case 10: {                                  /* weighted random */
        int n = GU8(pc, 2);
        uint32_t sel = bra_random_pick(ctx, pc, n);
        /* guest tail 0x801831C0: sel==0 -> pc + ((3n+4)&0x7fe) absolute
         * (no base add); sel!=0 -> base + sel. Applies for n==0 too. */
        return sel ? base_g + sel : pcg + ((3 * n + 4) & 0x7fe);
    }
    case 11: {                                  /* if/else 6B */
        uint32_t sel = cmp ? GU16(pc, 2) : GU16(pc, 4);
        return sel ? base_g + sel : pcg + 6;
    }
    default:                                    /* sub >= 12: uncond */
        return base_g + GU16(pc, 2);
    }
}

/* Push return address onto the ctx+0x4c stack (depth ctx+0x6c, max 8).
 * Returns the call target (base + u16@pc+2) on success, or `ret` when
 * the stack is full (guest then just advances pc, no call). */
static uint32_t bsr_call(uint8_t *ctx, uint32_t pcg, uint8_t *pc,
                         uint32_t base_g, uint32_t ret)
{
    /* guest 0x801832F0: lh RETD then andi 0xffff — the depth test is on
     * the UNSIGNED u16 (slti vs 0..0xffff), not the s16 value. A
     * negative/corrupt RETD must not push at a negative index. */
    uint32_t depth = GU16(ctx, VX_RETD);
    if (depth < 8) {
        GSU32(ctx, VX_RETSTK + depth * 4) = pcg + 4;
        GS16(ctx, VX_RETD) = (int16_t)(depth + 1);
        return base_g + GU16(pc, 2);
    }
    return ret;
}

/* scriptop_bra — script Bra: resolve target via bra_target()
 * against the anim base + compare value, store to ctx->pc. */
int scriptop_bra(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    uint32_t base_g = gld_u32(a0 + FC_ANI);
    int32_t cmp = GS32(ctx, VX_CMP);
    (void)a1; (void)a3;

    GSU32(ctx, VX_PC) = bra_target(ctx, pc, pcg, sub, base_g, cmp);
    return 1;
}

/* Shared Bsr tail (guest 0x80183368, reached by sub1/10/11): the
 * selected u16 was already stored to ctx->pc. sel==0 or a full return
 * stack -> pc = ret absolute; otherwise push ret and pc = base+sel. */
static uint32_t bsr_sel_call(uint8_t *ctx, uint32_t sel, uint32_t base_g,
                             uint32_t ret)
{
    uint32_t depth;
    if (sel == 0)
        return ret;
    depth = GU16(ctx, VX_RETD);             /* u16 depth, guest andi */
    if (depth < 8) {
        GSU32(ctx, VX_RETSTK + depth * 4) = ret;
        GS16(ctx, VX_RETD) = (int16_t)(depth + 1);
        return base_g + sel;
    }
    return ret;
}

/* op02 Bsr — guest 0x80183248. sub = insn byte1 indexes table
 * @0x80183314 directly (12 entries; sub>=12 falls through to the sub0
 * unconditional-call block). Every taken branch pushes a return
 * address onto the ctx+0x4c stack (depth ctx+0x6c u16, max 8) and
 * jumps to base + target; untaken/full-stack paths just advance pc.
 * Return addresses: sub1 -> pc+8, sub10 -> pc+((3n+4)&0x7fe),
 * sub11 -> pc+6, all others -> pc+4. */
int scriptop_bsr(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    uint32_t base_g = gld_u32(a0 + FC_ANI);
    int32_t cmp = GS32(ctx, VX_CMP);
    uint32_t npc;
    int call;
    (void)a1; (void)a3;

    switch (sub) {
    case 1: {                                   /* if/else call, ret pc+8 */
        uint32_t sel;
        if (cmp > 0) sel = GU16(pc, 2);
        else { GS32(ctx, VX_CMP) = 0; sel = GU16(pc, 6); }
        npc = bsr_sel_call(ctx, sel, base_g, pcg + 8);
        break;
    }
    case 10: {                                  /* weighted-random call */
        int n = GU8(pc, 2);
        uint32_t sel = bra_random_pick(ctx, pc, n);
        npc = bsr_sel_call(ctx, sel, base_g,
                           pcg + ((3 * n + 4) & 0x7fe));
        break;
    }
    case 11:                                    /* if/else call, ret pc+6 */
        npc = bsr_sel_call(ctx, cmp ? GU16(pc, 2) : GU16(pc, 4),
                           base_g, pcg + 6);
        break;
    default:
        /* sub2/5: call if cmp!=0 ; sub3/4: if cmp==0 ; sub6: cmp>0 ;
         * sub7: cmp>=0 ; sub8: cmp<0 ; sub9: cmp<=0 ; sub0/>=12: uncond */
        switch (sub) {
        case 2: case 5: call = cmp != 0; break;
        case 3: case 4: call = cmp == 0; break;
        case 6:         call = cmp >  0; break;
        case 7:         call = cmp >= 0; break;
        case 8:         call = cmp <  0; break;
        case 9:         call = cmp <= 0; break;
        default:        call = 1; break;
        }
        npc = call ? bsr_call(ctx, pcg, pc, base_g, pcg + 4) : pcg + 4;
        break;
    }
    GSU32(ctx, VX_PC) = npc;
    return 1;
}

/* ================================================================== */
/* op0b Wait group — guest 0x80183664. Wait counter is the s16 at      */
/* ctx+0x3c (r0 slot). Tick tail shared by the frame waits: counter   */
/* decremented each invocation; while (u16)(cnt-1) keeps bit pattern  */
/* the handler returns 0 with pc untouched, else pc advances and the  */
/* counter clears. Returns 1 only for the 2-byte conditional waits    */
/* and the sub1/sub11 expiries.                                        */
/* ================================================================== */

/* 3-byte stream sum wait operand (sub1 / sub11): reads stream bytes
 * at ctx->stream + idx, idx+1, idx+2 (each &0xff), bumps ctx->idx by
 * 3, then w = ((256 - ctx->aux8) * (b0+b1+b2)) / 8192 trunc-to-zero
 * (guest adds 8191 before sra 13 when the product is negative). */
static int32_t wait_rand3(uint8_t *ctx)
{
    uint32_t base = GU32(ctx, 0x00);
    uint32_t idx  = GU8(ctx, 0x04);
    int32_t b0 = (int32_t)gld_u8(base + ((idx + 0) & 0xff));
    int32_t b1 = (int32_t)gld_u8(base + ((idx + 1) & 0xff));
    int32_t b2 = (int32_t)gld_u8(base + ((idx + 2) & 0xff));
    int32_t sum, num;
    GSU8(ctx, 0x04) = (uint8_t)(idx + 3);
    sum = b0 + b1 + b2;
    num = (256 - (int32_t)GU16(ctx, VX_AUX8)) * sum;
    return (num >= 0 ? num : num + 8191) >> 13;
}

/* Shared frame-wait tick. cnt already holds the wait value (loaded
 * by the caller when it was 0). adv = guest pc to store on expiry,
 * ret = handler return on expiry. Returns the handler result. */
static int wait_tick(uint8_t *ctx, uint32_t pcg, uint32_t adv, int ret)
{
    int32_t d = (int32_t)GU16(ctx, 0x3c) - 1;   /* lhu - 1 */
    GS16(ctx, 0x3c) = (int16_t)d;               /* sh */
    /* sll v0,16 then signed >0 test: waits iff d in [1,32767] */
    if ((int32_t)((uint32_t)d << 16) > 0)
        return 0;                               /* still waiting */
    GS16(ctx, 0x3c) = 0;
    GSU32(ctx, VX_PC) = adv;
    return ret;
}

/* Load the wait counter from the insn when it is 0 (lh signed test). */
static void wait_arm(uint8_t *ctx, int32_t w)
{
    if (GS16(ctx, 0x3c) == 0)
        GS16(ctx, 0x3c) = (int16_t)w;
}

/* scriptop_wait — script Wait family: sub-op dispatch — rand3
 * wait arm/tick, foe-state waits; arms ctx+0x3c once. */
int scriptop_wait(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    uint32_t foe_g = gld_u32(a0 + FC_FOE);
    int32_t st, w;
    uint32_t adv;
    (void)a1; (void)a3;

    switch (sub) {
    case 1:                                     /* rand3 wait, 2B insn */
        /* stream operand consumed only on the arming invocation */
        if (GS16(ctx, 0x3c) == 0)               /* lh signed */
            GS16(ctx, 0x3c) = (int16_t)wait_rand3(ctx);
        adv = pcg + 2;
        return wait_tick(ctx, pcg, adv, 1);
    case 2: case 3: {                           /* self/foe attack win */
        uint32_t w_g = (sub == 3) ? foe_g : a0;
        st = (int32_t)gld_u32(w_g + FC_STATE0);
        /* continue iff not-attacking, or atkwin2==0 && atkwin1!=0 */
        if (st >= 0 ||
            (gld_s16(w_g + FC_ATKWIN2) == 0 &&
             gld_s16(w_g + FC_ATKWIN1) != 0)) {
            GSU32(ctx, VX_PC) = pcg + 2;
            return 1;
        }
        return 0;
    }
    case 4: case 5: {                           /* ... via atkwin3 */
        uint32_t w_g = (sub == 5) ? foe_g : a0;
        st = (int32_t)gld_u32(w_g + FC_STATE0);
        if (st >= 0 || gld_s16(w_g + FC_ATKWIN3) > 0) {
            GSU32(ctx, VX_PC) = pcg + 2;
            return 1;
        }
        return 0;
    }
    case 6: case 7: {                           /* wait while attacking */
        uint32_t w_g = (sub == 7) ? foe_g : a0;
        if ((int32_t)gld_u32(w_g + FC_STATE0) >= 0) {
            GSU32(ctx, VX_PC) = pcg + 2;
            return 1;
        }
        return 0;
    }
    case 8: {                                   /* rand wait v2, 4B insn */
        if (GS16(ctx, 0x3c) == 0) {             /* lh signed */
            uint32_t b = vm_stream_byte(ctx);
            GS16(ctx, 0x3c) =
                (int16_t)(((uint32_t)GU16(pc, 2) * b) >> 8) + 1;
        }
        adv = pcg + 4;
        return wait_tick(ctx, pcg, adv, 0);
    }
    case 9: case 10: {                          /* atk & state5 bit24 */
        uint32_t w_g = (sub == 10) ? foe_g : a0;
        st = (int32_t)gld_u32(w_g + FC_STATE0);
        int on = (gld_u32(w_g + FC_STATE5) & 0x01000000) != 0;
        if (st >= 0 || !on) {
            GSU32(ctx, VX_PC) = pcg + 2;
            return 1;
        }
        return 0;
    }
    case 11: {                                  /* rand3 + mirror, 4B */
        /* operand bytes are consumed only on the arming invocation */
        if (GS16(ctx, 0x3c) == 0)               /* lh signed */
            GS16(ctx, 0x3c) = (int16_t)wait_rand3(ctx);
        /* while still waiting the operand u16 is mirrored to ctx+6 */
        {
            int32_t d = (int32_t)GU16(ctx, 0x3c) - 1;
            GS16(ctx, 0x3c) = (int16_t)d;
            if ((int32_t)((uint32_t)d << 16) > 0) {
                GS16(ctx, VX_WAIT) = G16(pc, 2);
                return 0;
            }
        }
        GS16(ctx, 0x3c) = 0;
        GSU32(ctx, VX_PC) = pcg + 4;
        return 1;
    }
    default:                                    /* sub 0 / >=12: frames */
        wait_arm(ctx, GU16(pc, 2));
        adv = pcg + 4;
        return wait_tick(ctx, pcg, adv, 0);
    }
    (void)w;
}

/* ================================================================== */
/* op04 Chk — guest 0x80183C30. cond = insn byte1 (u16 pc >> 8).       */
/* 45-entry dispatch table @0x801235C4; cond >= 0x2d aliases case 0.   */
/* self = a0 (ctrl guest addr), foe = ctrl+0x2c.                       */
/*                                                                     */
/* Faithful port (disasm-verified insn-for-insn):                     */
/*  - ctx+0x38 receives the RAW computed value — simple conds store   */
/*    the masked state word / 0-1 result, mask conds (0x20-0x2a)      */
/*    store state & operand (NOT normalised to boolean).               */
/*  - ctx+0x24 advances +2, or +6 for the mask family (u32 operand    */
/*    = lhu(pc+2) | lhu(pc+4)<<16).                                    */
/*  - ctx+0x6e (lh) gates debug tracing: when set and the ctx+0x78    */
/*    cursor sits inside [ctx+0x7c, ctx+0x138) the guest sprintf's    */
/*    "(%s)"+cond-name @tbl 0x801b5440 pre-dispatch, and for the      */
/*    mask family "%8X" of the trace arg post-store into               */
/*    [ctx+0x7c, ctx+0x13b). sprintf_core_guest is the clean port of  */
/*    guest 0x8018EF60; its return (formatted len) advances the       */
/*    cursor exactly like the guest's `addu v1,v1,v0`.                */
/*  - Return $v0 is 1 on every path EXCEPT the 0x2b/0x2c compound     */
/*    sub-case where (state1 & 0x40080) != 0 — there the guest leaves */
/*    v0 = that nonzero mask (bnez -> epilogue without reloading 1).  */
/* ================================================================== */

extern int sprintf_core_guest(char *dst,char *fmt,uint32_t a2,
                              uint32_t a3,uint32_t a4,
                              uint32_t a5,uint32_t a6,
                              uint32_t a7);

/* cond-name strings for the "(%s)" debug trace — guest ptrs @0x801b5440 */
static const uint32_t chk_cond_name[45] = {
    0x801235ac,0x801235a0,0x80123598,0x80123590,0x80123588,0x80123580,
    0x80123574,0x80123568,0x80123560,0x80123558,0x80123550,0x80123548,
    0x80123540,0x80123538,0x80123530,0x80123528,0x80123520,0x80123518,
    0x8012350c,0x80123500,0x801234f4,0x801234e8,0x801234e0,0x801234d8,
    0x801234d0,0x801234c8,0x801234c0,0x801234b8,0x801234ac,0x801234a0,
    0x80123494,0x80123488,0x80123484,0x80123480,0x8012347c,0x80123478,
    0x80123474,0x80123470,0x8012346c,0x80123468,0x80123460,0x80123054,
    0x8012345c,0x80123450,0x80123444
};

/* debug-window bounds reproduce the guest's `cur - 0x7c - ctx` slti
 * range tests (signed): name trace <0xbc, result trace <0xbf. */
static int chk_trace_open(uint8_t *ctx, uint32_t bound)
{
    uint32_t cur = GU32(ctx, VX_TRACE);
    uint32_t cg  = (uint32_t)guest_of(ctx);
    return (int32_t)(cur - 0x7cu - cg) < (int32_t)bound;
}

int scriptop_chk(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t self = a0;
    uint32_t foe  = gld_u32(self + 0x2c);          /* lw 0x2c(s3)  */
    uint32_t pcg  = GU32(ctx, VX_PC);              /* lw 0x24(s2)  */
    uint8_t *pc   = (uint8_t *)guest_at(pcg);
    uint32_t cond = (uint32_t)(GU16(pc, 0) >> 8);  /* lhu; srl 8   */
    uint32_t res  = 0;
    uint32_t adv  = 2;
    uint32_t rv   = 1;
    uint32_t tr_arg = 0;        /* %8X vararg for mask-family trace   */
    int      tr_res  = 0;       /* 0: none, 1: operand arg, 2: result */
    uint32_t st;
    int32_t  d;
    (void)a1; (void)a3;

    /* ---- pre-dispatch name trace (ctx+0x6e gate, s4 < 0x2d) ---- */
    if (G16(ctx, VX_DBG) != 0 && cond < 0x2d && chk_trace_open(ctx, 0xbc)) {
        uint32_t cur = GU32(ctx, VX_TRACE);
        int n = sprintf_core_guest((char *)guest_at(cur),
                                   (char *)guest_at(0x801235b8u),
                                   chk_cond_name[cond],0,0,0,0,0);
        GSU32(ctx, VX_TRACE) = cur + (uint32_t)n;
    }

    switch (cond) {
    case 0:  /* + default (cond >= 0x2d aliases here) */
        res = gld_u32(self + 0x58) & 0x8000; break;
    case 1:  res = gld_u32(foe  + 0x58) & 0x8000; break;
    case 2:  res = gld_u32(self + 0x58) & 0x10000000; break;
    case 3:  res = gld_u32(foe  + 0x58) & 0x10000000; break;
    case 4:  res = gld_u32(self + 0x58) & 0x800; break;
    case 5:  res = gld_u32(foe  + 0x58) & 0x800; break;
    case 6:  /* ((st >>u 11) ^ 1) & 1 — !(state0 bit11) */
        res = ((gld_u32(self + 0x58) >> 11) ^ 1) & 1; break;
    case 7:  res = ((gld_u32(foe  + 0x58) >> 11) ^ 1) & 1; break;
    case 8:  res = gld_u32(self + 0x58) & 0x80000000; break;
    case 9:  res = gld_u32(foe  + 0x58) & 0x80000000; break;
    case 10: /* state0<0 && atkwin2 != 0 */
        st = gld_u32(self + 0x58);
        res = ((int32_t)st < 0 && gld_s16(self + 0xba) != 0) ? 1 : 0;
        break;
    case 11:
        st = gld_u32(foe + 0x58);
        res = ((int32_t)st < 0 && gld_s16(foe + 0xba) != 0) ? 1 : 0;
        break;
    case 12: /* state0<0 && atkwin2==0 && atkwin1!=0 */
        st = gld_u32(self + 0x58);
        res = ((int32_t)st < 0 && gld_s16(self + 0xba) == 0 &&
               gld_s16(self + 0xb8) != 0) ? 1 : 0;
        break;
    case 13:
        st = gld_u32(foe + 0x58);
        res = ((int32_t)st < 0 && gld_s16(foe + 0xba) == 0 &&
               gld_s16(foe + 0xb8) != 0) ? 1 : 0;
        break;
    case 14: /* state0<0 && 0 <s atkwin3 */
        st = gld_u32(self + 0x58);
        res = ((int32_t)st < 0 && gld_s16(self + 0xbc) > 0) ? 1 : 0;
        break;
    case 15:
        st = gld_u32(foe + 0x58);
        res = ((int32_t)st < 0 && gld_s16(foe + 0xbc) > 0) ? 1 : 0;
        break;
    case 16: res = gld_u8(self + 5) & 1; break;
    case 17: res = gld_u8(foe  + 5) & 1; break;
    case 18: /* state0<0 && (state5 & 0x01000000) != 0 */
        st = gld_u32(self + 0x58);
        res = ((int32_t)st < 0 &&
               (gld_u32(self + 0x6c) & 0x01000000u) != 0) ? 1 : 0;
        break;
    case 19:
        st = gld_u32(foe + 0x58);
        res = ((int32_t)st < 0 &&
               (gld_u32(foe + 0x6c) & 0x01000000u) != 0) ? 1 : 0;
        break;
    case 20: /* state0<0 && (state5 & 0x02000000) != 0 */
        st = gld_u32(self + 0x58);
        res = ((int32_t)st < 0 &&
               (gld_u32(self + 0x6c) & 0x02000000u) != 0) ? 1 : 0;
        break;
    case 21:
        st = gld_u32(foe + 0x58);
        res = ((int32_t)st < 0 &&
               (gld_u32(foe + 0x6c) & 0x02000000u) != 0) ? 1 : 0;
        break;
    case 22: res = gld_u32(self + 0x58) & 1; break;
    case 23: res = gld_u32(foe  + 0x58) & 1; break;
    case 24: res = gld_u32(self + 0x58) & 0x10; break;
    case 25: res = gld_u32(foe  + 0x58) & 0x10; break;
    case 26: res = gld_u32(self + 0x58) & 0x20; break;
    case 27: res = gld_u32(foe  + 0x58) & 0x20; break;
    case 28: /* !(state0<0) || atkwin3 > 0  (guest: bgez->1, else bc>0) */
        st = gld_u32(self + 0x58);
        res = ((int32_t)st >= 0 || gld_s16(self + 0xbc) > 0) ? 1 : 0;
        break;
    case 29:
        st = gld_u32(foe + 0x58);
        res = ((int32_t)st >= 0 || gld_s16(foe + 0xbc) > 0) ? 1 : 0;
        break;
    case 30: /* !(mode5&1) && gauge >=s 0x1e00 */
        res = ((gld_u8(self + 5) & 1) == 0 &&
               gld_s16(self + 0xd6) >= 0x1e00) ? 1 : 0;
        break;
    case 31:
        res = ((gld_u8(foe + 5) & 1) == 0 &&
               gld_s16(foe + 0xd6) >= 0x1e00) ? 1 : 0;
        break;
    /* ---- mask family 0x20-0x2a: operand u32 @pc+2, adv 6 ---- */
    case 32: case 33: case 34: case 35:
    case 36: case 37: case 38: case 39:
    case 41: case 42: {
        /* field by cond: even self / odd foe; +0x64,+0x5c,+0x58,+0x68,
         * then 0x29/0x2a -> +0x6c. self-side paths (even, and the two
         * +0x6c entries via 84260) differ in the %8X arg: the direct
         * 84278 entries trace the raw operand, the 84260 entries trace
         * the masked result — reproduced via tr_res. */
        static const uint8_t fld_of[10] = {
            0x64,0x64, 0x5c,0x5c, 0x58,0x58, 0x68,0x68, 0x6c,0x6c };
        static const uint8_t self_of[10] = {
            1,0, 1,0, 1,0, 1,0, 1,0 };
        static const uint8_t argres_of[10] = {
            1,2, 1,2, 1,2, 1,2, 2,2 };   /* operand vs result trace arg */
        int mi = (cond >= 0x29) ? (cond - 0x29 + 8) : (cond - 0x20);
        uint32_t who = self_of[mi] ? self : foe;
        uint32_t operand = GU16(pc, 2) | ((uint32_t)GU16(pc, 4) << 16);
        res = gld_u32(who + fld_of[mi]) & operand;
        adv = 6;
        tr_res = argres_of[mi];
        tr_arg = (tr_res == 2) ? res : operand;
        break;
    }
    case 40: /* |distX| >=s 0x6ab */
        d = gld_s16(self + 0x74);
        if (d < 0) d = -d;
        res = (d >= 0x6ab) ? 1 : 0;
        break;
    case 43: case 44: {   /* guard-reaction compound (0x2b/0x2c) */
        uint32_t who = (cond == 43) ? self : foe;
        res = 0; rv = 1;
        st = gld_u32(who + 0x58);
        if ((int32_t)st < 0 && gld_s16(who + 0xba) > 0) {
            uint32_t m = gld_u32(who + 0x5c) & 0x00040080u;
            if (m != 0) {
                /* guest: bnez -> epilogue with v0 = m (nonzero),
                 * ctx+0x38 already stored 0 */
                res = 0;
                rv  = m;
            } else {
                res = ((gld_u32(who + 0x64) & 0x40002000u)
                       == 0x40002000u) ? 1 : 0;
            }
        }
        break;
    }
    default:
        res = gld_u32(self + 0x58) & 0x8000; break;
    }

    GSU32(ctx, VX_CMP) = res;

    /* ---- post-store result trace (mask family only, 0x20-0x2a) ---- */
    if (tr_res != 0 && G16(ctx, VX_DBG) != 0 && chk_trace_open(ctx, 0xbf)) {
        uint32_t cur = GU32(ctx, VX_TRACE);
        int n = sprintf_core_guest((char *)guest_at(cur),
                                   (char *)guest_at(0x801235c0u),
                                   tr_arg,0,0,0,0,0);
        GSU32(ctx, VX_TRACE) = cur + (uint32_t)n;
    }

    GSU32(ctx, VX_PC) = pcg + adv;
    return (int)rv;
}

/* ================================================================== */
/* op0f GPad — guest 0x80184AA0. Polls fighter state + pad stream     */
/* each invocation; always returns 0 (suspends the dispatch batch).   */
/* ================================================================== */

int scriptop_gpad(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    uint32_t foe_g = gld_u32(a0 + FC_FOE);
    uint32_t st0 = gld_u32(a0 + FC_STATE0);
    uint32_t st1 = gld_u32(a0 + FC_STATE1);
    uint32_t flags;
    (void)a1;

    /* Path A: entered when guard/hit state bits are up. */
    if ((st0 & 0x10000000) || (st1 & 0x02000000)) {
        /* deep path: guest tests (st1 & 0x02000080) == 0x02000000 —
         * bit25 set AND bit7 clear — plus ctrl+0xcc < 97 + stream rand */
        if ((st1 & 0x02000080) == 0x02000000 &&
            gld_s16(a0 + 0xcc) < 97) {
            uint32_t sbase = GU32(ctx, 0x00);
            uint32_t idx   = GU8(ctx, 0x04);
            uint32_t b;
            GSU8(ctx, 0x04) = (uint8_t)(idx + 1);
            b = gld_u8(sbase + (idx & 0xff));
            if (b < (GU16(ctx, VX_AUX8) >> 3))
                GS16(ctx, VX_WAIT) = 0x2000;
        }
        GS16(ctx, VX_ENDW) = 3;
        return 0;
    }

    /* Path B: reload counter from insn sub byte when 0. */
    if (GS16(ctx, 0x3c) == 0) {                /* lh signed */
        GS16(ctx, 0x3c) = (int16_t)GU8(pc, 1);
        flags = GU16(ctx, VX_FLAGS) & 0xfffd;
        GS16(ctx, VX_FLAGS) = (int16_t)flags;
    }
    GS16(ctx, VX_WAIT) = G16(pc, 2);           /* sh lhu @pc+2 */

    /* foe attack-window stream compare -> flags bit1 */
    if ((int32_t)gld_u32(foe_g + FC_STATE0) < 0 &&
        gld_s16(foe_g + 0x88) >= gld_s16(foe_g + FC_ATKWIN2) &&
        gld_s16(foe_g + FC_ATKWIN2) >= 0) {
        uint32_t sbase = GU32(ctx, 0x00);
        uint32_t idx   = GU8(ctx, 0x04);
        uint32_t p1c   = GU32(ctx, 0x1c);
        uint32_t b0, b1;
        GSU8(ctx, 0x04) = (uint8_t)(idx + 1);
        b0 = gld_u8(sbase + (idx & 0xff));
        /* guest adds (aux8 >> 4) — computed once at fn entry — to the
         * ctx+0x1c pointer */
        b1 = gld_u8(p1c + (GU16(ctx, VX_AUX8) >> 4));
        if (b0 < b1)
            GS16(ctx, VX_FLAGS) = (int16_t)(GU16(ctx, VX_FLAGS) | 2);
    }

    flags = GU16(ctx, VX_FLAGS);
    if (flags & 2) {
        if (gld_s16(foe_g + FC_ATKWIN1) == 0) {
            GS16(ctx, VX_FLAGS) = (int16_t)(flags & 0xfffd);
        } else {
            uint32_t w  = GU16(ctx, VX_WAIT);
            uint32_t w1 = w & 0xdfff;
            uint32_t st3 = gld_u32(foe_g + FC_STATE3);
            GS16(ctx, VX_WAIT) = (int16_t)w1;
            if (st3 & 0x80) {
                GS16(ctx, VX_WAIT) = (int16_t)(w & 0x9fff);
            } else if (st3 & 0x40) {
                /* guest @0x80184c88-0x80184c94: beq tests st3&0x40, then
                 * the delay-slot ori v0,a1,0x4000 overwrites v0 with
                 * (w&0xdfff)|0x4000 BEFORE the sh — the stored value is
                 * the ori result, not 0x40. */
                GS16(ctx, VX_WAIT) = (int16_t)(w1 | 0x4000);
            }
        }
    }

    /* counter tick: advance pc only when it reaches exactly 0 */
    {
        int32_t d = (int32_t)GU16(ctx, 0x3c) - 1;
        GS16(ctx, 0x3c) = (int16_t)d;
        if ((int32_t)((uint32_t)d << 16) == 0)
            GSU32(ctx, VX_PC) = pcg + 4;
    }
    return 0;
}

/* ================================================================== */
/* op11 Camera — guest 0x80184D3C. cam = *(u32*)0x1F8000BC guest ptr. */
/* sub = sign-extended insn byte1; dispatch table @0x8012376C has 10  */
/* entries (sub 0..9); sub>=10 unsigned -> pc+2, ret 1.               */
/*   0: cam+0x76 = u16@2            (+4)                              */
/*   1: cam+0x52/54/56 = u16@2/4/6  (+8)                              */
/*   2: cam+0x30=u16@2, +0x32=u16@4, +0x8a=u16@4, +0x34=u16@6 (+8)   */
/*   3: cam+0x64/66/68 = u16@2/4/6  (+8)                              */
/*   4: cam+0x42/44/46 = u16@2/4/6  (+8)                              */
/*   5: cam[0x80+ctrl.b2]=insn.b2, cam[0x80+foe.b2]=insn.b3 (+4)      */
/*   6: cam+0x5a = u16@2            (+4)                              */
/*   7: cam+0x38 = u16@2            (+4)                              */
/*   8: cam+0x6c = u16@2            (+4)                              */
/*   9: preset: cam+0x7e=idx; cam+0x4a/4c/4e = s16s at               */
/*      0x8011F944 + (s16)idx*8 (+0,+2,+4)          (+4)               */
/* ================================================================== */

int scriptop_camera(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = (int8_t)GU8(pc, 1);
    uint32_t cam_g = gld_u32(0x1f8000bc);
    uint32_t npc = pcg + 2;
    (void)a1; (void)a3;

    if (sub < 0 || sub >= 10)
        goto store;

    switch (sub) {
    case 0:
        br2_st16(cam_g + 0x76, GU16(pc, 2));
        npc = pcg + 4; break;
    case 1:
        br2_st16(cam_g + 0x52, GU16(pc, 2));
        br2_st16(cam_g + 0x54, GU16(pc, 4));
        br2_st16(cam_g + 0x56, GU16(pc, 6));
        npc = pcg + 8; break;
    case 2:
        br2_st16(cam_g + 0x30, GU16(pc, 2));
        br2_st16(cam_g + 0x32, GU16(pc, 4));
        br2_st16(cam_g + 0x8a, GU16(pc, 4));
        br2_st16(cam_g + 0x34, GU16(pc, 6));
        npc = pcg + 8; break;
    case 3:
        br2_st16(cam_g + 0x64, GU16(pc, 2));
        br2_st16(cam_g + 0x66, GU16(pc, 4));
        br2_st16(cam_g + 0x68, GU16(pc, 6));
        npc = pcg + 8; break;
    case 4:
        br2_st16(cam_g + 0x42, GU16(pc, 2));
        br2_st16(cam_g + 0x44, GU16(pc, 4));
        br2_st16(cam_g + 0x46, GU16(pc, 6));
        npc = pcg + 8; break;
    case 5: {                                   /* per-fighter byte pair */
        uint32_t foe_g = gld_u32(a0 + FC_FOE);
        br2_st8(cam_g + 0x80 + gld_u8(a0 + 2), GU8(pc, 2));
        br2_st8(cam_g + 0x80 + gld_u8(foe_g + 2), GU8(pc, 3));
        npc = pcg + 4; break;
    }
    case 6:
        br2_st16(cam_g + 0x5a, GU16(pc, 2));
        npc = pcg + 4; break;
    case 7:
        br2_st16(cam_g + 0x38, GU16(pc, 2));
        npc = pcg + 4; break;
    case 8:
        br2_st16(cam_g + 0x6c, GU16(pc, 2));
        npc = pcg + 4; break;
    case 9: {                                   /* preset vector */
        int32_t idx = (int16_t)GU16(pc, 2);
        /* guest: idx<<16 sra13 == (s16)idx*8; reloads cam+0x7e as lh */
        uint32_t e = 0x8011f944 + (uint32_t)(idx * 8);
        br2_st16(cam_g + 0x7e, (uint16_t)idx);
        br2_st16(cam_g + 0x4a, (uint16_t)gld_u16(e));
        br2_st16(cam_g + 0x4c, (uint16_t)gld_u16(e + 2));
        br2_st16(cam_g + 0x4e, (uint16_t)gld_u16(e + 4));
        npc = pcg + 4; break;
    }
    }
store:
    GSU32(ctx, VX_PC) = npc;
    return 1;
}

/* ================================================================== */
/* op05 Move — guest 0x8018444C. sub = insn byte1: dst = sub>>4,       */
/* src = sub&0xf. Only src 1..5 (regs), 7 (imm16), 15 (ext byte)       */
/* advance pc; every other src stores the literal and leaves pc       */
/* untouched — the guest then re-executes the same insn forever        */
/* (kept). ext byte @pc+2 selects a field source for src==15:         */
/*   1 distX 2 distY 3 ctx+8 4 a3+0x94&7fff 5/6 gauge>>6 self/foe     */
/*   7/8 dmg% self/foe(per SELF maxHP) 9/10 obj+0x178-own+0x94        */
/*   11/12 red-bar% self/foe(per SELF maxHP); 0 or >12 -> literal.    */
/* ================================================================== */

static int32_t move_gauge_s16(uint32_t g)
{
    int32_t v = gld_s16(g + FC_GAUGE);
    return v >= 0 ? v >> 6 : (v + 0x3f) >> 6;
}

/* 100*(maxHP-hp)/maxHP style percent; den==0 -> -1 (MIPS LO div0). */
static int32_t pct100(int32_t num, int32_t den)
{
    return den ? (int32_t)((int64_t)num * 100 / den) : -1;
}

/* scriptop_move — script Move: dst/src nibble decode — imm16
 * sign-extended or vm-register source -> store to dst. */
int scriptop_move(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    uint32_t pcg = GU32(ctx, VX_PC);
    uint8_t *pc = (uint8_t *)guest_at(pcg);
    int sub = GU8(pc, 1);
    int dst = sub >> 4, src = sub & 0xf;
    uint32_t foe_g = gld_u32(a0 + FC_FOE);
    int32_t sv = 0;
    uint32_t npc = 0;
    int have_pc = 0;
    (void)a1;

    if (src == 7) {                         /* imm16, 4B insn */
        sv = gld_s16(pcg + 2);
        npc = pcg + 4; have_pc = 1;
    } else if (src >= 1 && src <= 5) {      /* vm reg, 2B insn */
        sv = VREG(ctx, src);
        npc = pcg + 2; have_pc = 1;
    } else if (src == 15) {                 /* ext byte @pc+2, 4B */
        int e = GU8(pc, 2);
        if (e >= 1 && e <= 12) {
            switch (e) {
            case 1:  sv = gld_s16(a0 + FC_DISTX); break;
            case 2:  sv = gld_s16(a0 + FC_DISTY); break;
            case 3:  sv = (int32_t)GU16(ctx, VX_AUX8); break;
            case 4:  sv = (int32_t)(gld_u16(a3 + 0x94) & 0x7fff); break;
            case 5:  sv = move_gauge_s16(a0); break;
            case 6:  sv = move_gauge_s16(foe_g); break;
            case 7:  sv = pct100(gld_s16(a0 + FC_MAXHP) -
                                 gld_s16(a0 + FC_HP),
                                 gld_s16(a0 + FC_MAXHP)); break;
            case 8:  sv = pct100(gld_s16(foe_g + FC_MAXHP) -
                                 gld_s16(foe_g + FC_HP),
                                 gld_s16(a0 + FC_MAXHP)); break; /* SELF maxHP */
            case 9: case 10: {                       /* obj+0x178 - +0x94 */
                uint32_t w = (e == 9) ? a0 : foe_g;
                uint32_t vth = gld_u32(w + FC_VTH);
                sv = (int32_t)(gld_u32(vth + 0x178) -
                               (uint32_t)gld_s16(w + 0x94));
                break;
            }
            case 11: sv = pct100(gld_s16(a0 + FC_HP) -
                                 gld_s16(a0 + FC_HPDISP),
                                 gld_s16(a0 + FC_MAXHP)); break;
            case 12: sv = pct100(gld_s16(foe_g + FC_HP) -
                                 gld_s16(foe_g + FC_HPDISP),
                                 gld_s16(a0 + FC_MAXHP)); break; /* SELF maxHP */
            }
            npc = pcg + 4; have_pc = 1;
        } else {
            sv = e;                     /* literal ext, pc hangs */
        }
    } else {
        sv = src;                       /* 0,6,8..14 literal, hangs */
    }

    if (have_pc) GSU32(ctx, VX_PC) = npc;
    GS16(ctx, VX_REGS + dst * 2) = (int16_t)sv;
    return 1;
}

/* script_ctx28_clear — guest func_801835C4, a standalone leaf between
 * scriptop_ret and scriptop_bsr: `sh zero,0x28(a2); move v0,zero; jr ra`.
 * Clears the u16 ctx field at +0x28 (VX_PC is +0x24). */
int script_ctx28_clear(uint32_t a0, uint32_t a1, uint8_t *ctx, uint32_t a3)
{
    (void)a0; (void)a1; (void)a3;
    GS16(ctx, 0x28) = 0;
    return 0;
}

/* ================================================================== */
/* Event-VM descriptor opcode guest entries (0x8017AB74..0x8017D758    */
/* region). These are the table-dispatched handlers reached through    */
/* ev_stream_dispatch / sibling split-jumps. Convention: a0=ctrl,      */
/* a1=motion, a2=record-stream pc (guest addrs), a3=aux/target ctrl   */
/* selected by the caller; return value is the next stream guest addr */
/* in $v0. Indirect `jr table[i]` tails become call_guest4 so the     */
/* callee's `jr $ra` lands back here and its $v0 propagates.          */
/* ================================================================== */
extern uint32_t call_guest4(uint32_t fn, uint32_t a0, uint32_t a1,
                            uint32_t a2, uint32_t a3);

/* ev4_branch_dispatch — guest @0x8017AB74 (EV4 conditional branch).
 * cls = pc[1] & 0xc0 selects the compared ctrl: 0x40 proceeds only
 * when !(ctrl+5 & 1), 0x80 only when (ctrl+5 & 1), 0xc0 uses the foe
 * ctrl (ctrl+0x2c), 0x00 uses ctrl. op = pc[1] & 0x3f indexes table
 * 0x8012272c (27 entries); op >= 27 takes the fail path
 * ctrl+0x30 (.ANI base) + sx16(pc+2). */
uint32_t ev4_branch_dispatch(uint32_t ctrl, uint32_t motion, uint32_t pc)
{
    uint8_t *c = (uint8_t *)guest_at(ctrl);
    uint8_t *p = (uint8_t *)guest_at(pc);
    uint32_t cls = GU8(p, 1) & 0xc0;
    uint32_t a3;
    uint32_t op;

    if (cls == 0x40) {
        if (GU8(c, FC_MODE5) & 1) return pc + 4;
        a3 = ctrl;
    } else if (cls == 0x80) {
        if (!(GU8(c, FC_MODE5) & 1)) return pc + 4;
        a3 = ctrl;
    } else if (cls == 0xc0) {
        a3 = GU32(c, FC_FOE);
    } else {
        a3 = ctrl;
    }
    op = GU8(p, 1) & 0x3f;
    if (op >= 27u) {
        /* 0x8017AE00: v0 = lhu(pc+2) + lw(ctrl+0x30), jr $ra.
         * The guest load is LHU (0x94C30002, opcode 0x25) — zero-extend;
         * sx16 would subtract 0x10000 for offsets >= 0x8000. */
        return (uint32_t)GU16(p, 2) + GU32(c, FC_ANI);
    }
    /* 0x8017AC18: jr lw(0x8012272c + op*4) with a3 = selected ctrl */
    return call_guest4(GU32((const uint8_t *)guest_at(0x8012272cu), op * 4),
                       ctrl, motion, pc, a3);
}

/* ev6_guard_sync_dispatch — guest @0x8017B0D4 (EV6 guard/counter sync).
 * b = sx8(pc[1]); index b+4 into table 0x801227bc covers -4..1:
 *  -4: if motion+0x9c == 0 -> pc+2, else fall into -1
 *  -1: motion+0x9c = 0; motion+0x100 ^= 1
 *  -3: copy foe-motion (foe=ctrl+0x2c -> +0x38) +0x9c -> motion+0x9c;
 *      if motion+0x100 == foe+0x100 -> pc+2 else copy foe+0x100
 *  -2: motion+0x9c = 0; if motion+0x100 == 0 -> pc+2 else = 0
 *   0/1: motion+0x9c = pc[1]
 *  other: pc+2. Surviving cases run the common tail swapping the u16
 *  pairs at sub=ctrl+0x18 offsets 0x1c4<->0x23c and 0x264<->0x28c. */
uint32_t ev6_guard_sync_dispatch(uint32_t ctrl, uint32_t motion,
                                 uint32_t pc)
{
    uint8_t *c = (uint8_t *)guest_at(ctrl);
    uint8_t *m = (uint8_t *)guest_at(motion);
    uint8_t *p = (uint8_t *)guest_at(pc);
    int8_t b = (int8_t)GU8(p, 1);
    uint8_t *sub;
    uint32_t foe, fm;
    uint16_t a, d;

    switch (b) {
    case -4:                                    /* 0x8017B174 */
        if (GU8(m, 0x9c) == 0) return pc + 2;
        /* fall through */
    case -1:                                    /* 0x8017B184 */
        GSU8(m, 0x9c)  = 0;
        GSU8(m, 0x100) = GU8(m, 0x100) ^ 1;
        break;
    case -3: {                                  /* 0x8017B114 */
        foe = GU32(c, FC_FOE);
        fm  = GU32((const uint8_t *)guest_at(foe), FC_MOTION);
        GSU8(m, 0x9c) = GU8((const uint8_t *)guest_at(fm), 0x9c);
        if (GU8(m, 0x100) ==
            GU8((const uint8_t *)guest_at(fm), 0x100))
            return pc + 2;
        GSU8(m, 0x100) = GU8((const uint8_t *)guest_at(fm), 0x100);
        break;
    }
    case -2:                                    /* 0x8017B15C */
        GSU8(m, 0x9c) = 0;
        if (GU8(m, 0x100) == 0) return pc + 2;
        GSU8(m, 0x100) = 0;
        break;
    case 0: case 1:                             /* 0x8017B1D4 */
        GSU8(m, 0x9c) = GU8(p, 1);
        return pc + 2;
    default:
        return pc + 2;
    }
    /* 0x8017B194 common tail: swap u16 pairs in ctrl+0x18 sub-struct */
    sub = (uint8_t *)guest_at(GU32(c, 0x18));
    a = GU16(sub, 0x1c4); d = GU16(sub, 0x23c);
    GSU16(sub, 0x1c4) = d; GSU16(sub, 0x23c) = a;
    a = GU16(sub, 0x264); d = GU16(sub, 0x28c);
    GSU16(sub, 0x264) = d; GSU16(sub, 0x28c) = a;
    return pc + 2;
}

/* ev20_timer_dispatch — guest @0x8017C9D4 (EV20 x16 timers).
 * op = sx8(pc[1]); op < 5 (unsigned) indexes table 0x8012294c:
 *   0 -> 0x8017CA0C (store only if ctrl+0xc0 >= 0) then +0xc0
 *   1 -> ctrl+0xd0, 2 -> ctrl+0xd2, 3 -> ctrl+0xcc, 4 -> ctrl+0xd4
 * each writes sx16(pc+2) << 4. All paths return pc+4. */
uint32_t ev20_timer_dispatch(uint32_t ctrl, uint32_t motion, uint32_t pc,
                             uint32_t a3)
{
    (void)motion;
    {
        int8_t op = (int8_t)GU8((const uint8_t *)guest_at(pc), 1);
        if ((uint32_t)op >= 5u) return pc + 4;
        return call_guest4(
            GU32((const uint8_t *)guest_at(0x8012294cu), (uint32_t)op * 4),
            ctrl, motion, pc, a3);
    }
}

/* desc_flagbit_dispatch — guest @0x8017BAD4 (descriptor flag-bit ops).
 * op = pc[1] & 0x3f; op < 35 indexes table 0x8012285C (lui 0x8012 +
 * addiu 0x285C @0x8017BAEC/0x8017BAF0 — verified against SLES_017.22
 * bytes; entries point into this function's own 0x8017BBxx handler
 * blocks). op >= 35 -> pc+6. The selected handler returns the next
 * stream addr in $v0. */
uint32_t desc_flagbit_dispatch(uint32_t a0, uint32_t a1, uint32_t a2,
                               uint32_t a3)
{
    uint32_t op = GU8((const uint8_t *)guest_at(a2), 1) & 0x3f;
    if (op >= 35u) return a2 + 6;
    return call_guest4(
        GU32((const uint8_t *)guest_at(0x8012285Cu), op * 4),
        a0, a1, a2, a3);
}

/* desc_analog_dispatch — guest @0x8017B668 (descriptor analog/dpad op
 * dispatcher, sibling of desc_flagbit_dispatch). op = live $v1
 * (cpu->gpr[3], masked &0xF by caller cps_8017b5f4); op < 11 indexes
 * table 0x801227FC whose entries are this function's own handler
 * blocks; op >= 11 -> a2+4. Every handler tails `jr $ra` with
 * v0 = a2+4, so the whole dispatch returns a2+4. Handlers do s16/u16
 * field arithmetic on the a0/a1/a2 records; the +208/+228 channels
 * are clamped to s12 via the guest's <<20>>20 idiom. Incoming a3 is
 * dead — op6 rebuilds it as the dpad offset. */
uint32_t desc_analog_dispatch(uint32_t a0, uint32_t a1, uint32_t a2,
                              uint32_t a3)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t op = cpu->gpr[3];
    uint8_t *r0 = (uint8_t *)guest_at(a0);
    uint8_t *r1 = (uint8_t *)guest_at(a1);
    uint8_t *r2 = (uint8_t *)guest_at(a2);
    int32_t v, param, lim, old, a3v;
    uint32_t fl;
    (void)a3;

#define CLAMP12(x) ((int32_t)((uint32_t)(x) << 20) >> 20)

    param = G16(r2, 2);                     /* s16 h(a2+2) operand */
    switch (op) {
    case 0:                                 /* B690: +208 = u16 operand */
        GSU16(r1, 208) = GU16(r2, 2);
        break;
    case 1:                                 /* B69C: +208 += param */
        v = G16(r1, 208) + param;
        GSU16(r1, 208) = (uint16_t)CLAMP12(v);
        break;
    case 2: {                               /* B6AC: +208 = clamp(*(a0+44)+78 + param) */
        uint8_t *sub = (uint8_t *)guest_at(GU32(r0, 44));
        v = G16(sub, 78) + param;
        GSU16(r1, 208) = (uint16_t)CLAMP12(v);
        break;
    }
    case 3:                                 /* B6D0: +228 = clamp(h(a0+116) + param) */
        v = G16(r0, 116) + param;
        GSU16(r1, 228) = (uint16_t)CLAMP12(v);
        break;
    case 4:                                 /* B6DC: +228 = clamp(h196 - (h78-2048) + param) */
        v = G16(r0, 196) - (G16(r0, 78) - 2048) + param;
        GSU16(r1, 228) = (uint16_t)CLAMP12(v);
        break;
    case 5:                                 /* B704: +228 = clamp(h(a0+112) + param) */
        v = G16(r0, 112) + param;
        GSU16(r1, 228) = (uint16_t)CLAMP12(v);
        break;
    case 6: {                               /* B710: dpad flags -> signed offset */
        fl = GU32(r0, 108);
        a3v = 0;
        if (fl & 0x10) a3v = -1024;
        if (fl & 0x20) a3v =  1024;
        if (fl & 0x40) a3v =     0;
        if (fl & 0x80) a3v = -2048;
        if (fl & 0x01) a3v = -1024;
        if (fl & 0x02) a3v =  1024;
        if (fl & 0x04) a3v =     0;
        if (fl & 0x08) a3v = -2048;
        old = G16(r1, 228);
        v = param + a3v + old - G16(r0, 78);
        GSU16(r1, 228) = (uint16_t)CLAMP12(v);
        break;
    }
    case 7: {                               /* B79C: gated clamped step on +228 */
        if (GU32(r0, 92) & 0x100000) break;
        lim = G16(r0, 116);
        old = G16(r1, 228);
        if (lim < -param)      v = old - param;
        else if (param < lim)  v = old + param;
        else                   v = old + lim;
        GSU16(r1, 228) = (uint16_t)CLAMP12(v);
        break;
    }
    case 8:                                 /* B858: no-op */
        break;
    case 9: {                               /* B7F8: copy obj+372/380 -> +212/+216, then op4 math */
        uint8_t *sub = (uint8_t *)guest_at(GU32(r0, 32));
        GSU16(r1, 212) = GU16(sub, 372);
        GSU16(r1, 216) = GU16(sub, 380);
        v = G16(r0, 196) - (G16(r0, 78) - 2048) + param;
        GSU16(r1, 228) = (uint16_t)CLAMP12(v);
        break;
    }
    case 10:                                /* B83C: +228 += param */
        v = G16(r1, 228) + param;
        GSU16(r1, 228) = (uint16_t)CLAMP12(v);
        break;
    default:                                /* op >= 11 -> a2+4 */
        break;
    }
#undef CLAMP12
    return a2 + 4;
}

/* ================================================================== */
/* EV46 camera-param op family — guest 0x8017E840 dispatcher plus the  */
/* ten table handlers @0x80122B1C (idx 0..9) it tail-calls. CameraObj */
/* = *(u32*)0x1F8000BC; the dispatcher overwrites a1 with it before   */
/* the jr, so handlers see (a0=ctrl, a1=cam, a2=stream pc, a3=aux).   */
/* Semantics cross-checked against scriptop_camera (VM1 op11 twin,    */
/* same cam field map).                                               */
/* ================================================================== */

/* ev_cam_param_dispatch — guest @0x8017E840 (EV op 46).
 * sub = sx8(pc[1]); (u32)sub >= 10 -> pc+2. Else tail-jr through
 * table 0x80122B1C[sub] with a1 = cam. Table: idx0 ev_cam_op00,
 * idx1..9 ev_cam_op01..09. */
uint32_t ev_cam_param_dispatch(uint32_t a0, uint32_t a1, uint32_t a2,
                               uint32_t a3)
{
    (void)a1;
    int32_t sub = (int8_t)gld_u8(a2 + 1);
    uint32_t cam_g = gld_u32(0x1f8000bc);
    if ((uint32_t)sub >= 10u) return a2 + 2;
    return call_guest4(
        gld_u32(0x80122b1c + (uint32_t)sub * 4),
        a0, cam_g, a2, a3);
}

/* ev_cam_op00 — guest @0x8017E87C. cam+0x76 = u16@pc+2; ret pc+4. */
uint32_t ev_cam_op00(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    br2_st16(a1 + 0x76, (uint16_t)gld_u16(a2 + 2));
    return a2 + 4;
}

/* ev_cam_op01 — guest @0x8017E88C. cam+0x52/54/56 = u16@pc+2/4/6;
 * ret pc+8. */
uint32_t ev_cam_op01(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    br2_st16(a1 + 0x52, (uint16_t)gld_u16(a2 + 2));
    br2_st16(a1 + 0x54, (uint16_t)gld_u16(a2 + 4));
    br2_st16(a1 + 0x56, (uint16_t)gld_u16(a2 + 6));
    return a2 + 8;
}

/* ev_cam_op02 — guest @0x8017E8B4. cam+0x30=u16@2, +0x32=u16@4,
 * +0x8a=u16@4 (same operand stored twice), +0x34=u16@6; ret pc+8. */
uint32_t ev_cam_op02(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    uint16_t v4 = (uint16_t)gld_u16(a2 + 4);
    br2_st16(a1 + 0x30, (uint16_t)gld_u16(a2 + 2));
    br2_st16(a1 + 0x32, v4);
    br2_st16(a1 + 0x8a, v4);
    br2_st16(a1 + 0x34, (uint16_t)gld_u16(a2 + 6));
    return a2 + 8;
}

/* ev_cam_op03 — guest @0x8017E8E0. cam+0x64/66/68 = u16@pc+2/4/6;
 * ret pc+8. */
uint32_t ev_cam_op03(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    br2_st16(a1 + 0x64, (uint16_t)gld_u16(a2 + 2));
    br2_st16(a1 + 0x66, (uint16_t)gld_u16(a2 + 4));
    br2_st16(a1 + 0x68, (uint16_t)gld_u16(a2 + 6));
    return a2 + 8;
}

/* ev_cam_op04 — guest @0x8017E908. cam+0x42/44/46 = u16@pc+2/4/6;
 * ret pc+8. */
uint32_t ev_cam_op04(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    br2_st16(a1 + 0x42, (uint16_t)gld_u16(a2 + 2));
    br2_st16(a1 + 0x44, (uint16_t)gld_u16(a2 + 4));
    br2_st16(a1 + 0x46, (uint16_t)gld_u16(a2 + 6));
    return a2 + 8;
}

/* ev_cam_op05 — guest @0x8017E930. Per-fighter cam byte pair:
 * cam[0x80 + ctrl.b2] = pc[2]; cam[0x80 + foe.b2] = pc[3]
 * (foe = *(ctrl+0x2c); second index lbu(foe+2), value is the high
 * byte of u16@pc+2 == pc[3]); ret pc+4. */
uint32_t ev_cam_op05(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a3;
    uint32_t foe_g = gld_u32(a0 + FC_FOE);
    br2_st8(a1 + 0x80 + gld_u8(a0 + 2), (uint8_t)gld_u8(a2 + 2));
    br2_st8(a1 + 0x80 + gld_u8(foe_g + 2),
            (uint8_t)(gld_u16(a2 + 2) >> 8));
    return a2 + 4;
}

/* ev_cam_op06 — guest @0x8017E960. cam+0x5a = u16@pc+2; ret pc+4. */
uint32_t ev_cam_op06(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    br2_st16(a1 + 0x5a, (uint16_t)gld_u16(a2 + 2));
    return a2 + 4;
}

/* ev_cam_op07 — guest @0x8017E970. cam+0x38 = u16@pc+2; ret pc+4. */
uint32_t ev_cam_op07(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    br2_st16(a1 + 0x38, (uint16_t)gld_u16(a2 + 2));
    return a2 + 4;
}

/* ev_cam_op08 — guest @0x8017E980. cam+0x6c = u16@pc+2; ret pc+4. */
uint32_t ev_cam_op08(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    br2_st16(a1 + 0x6c, (uint16_t)gld_u16(a2 + 2));
    return a2 + 4;
}

/* ev_cam_op09 — guest @0x8017E990. Preset vector: idx = sx16(u16@pc+2);
 * cam+0x7e = idx (u16); rec = 0x8011F944 + idx*8 (8-byte records);
 * cam+0x4a/4c/4e = u16@rec+0/+2/+4; ret pc+4. The guest recomputes the
 * record pointer three times, reloading cam+0x7e with lh each time —
 * same value, single computation is faithful. */
uint32_t ev_cam_op09(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a0; (void)a3;
    int32_t idx = (int16_t)gld_u16(a2 + 2);
    uint32_t e = 0x8011f944 + (uint32_t)(idx * 8);
    br2_st16(a1 + 0x7e, (uint16_t)idx);
    br2_st16(a1 + 0x4a, (uint16_t)gld_u16(e));
    br2_st16(a1 + 0x4c, (uint16_t)gld_u16(e + 2));
    br2_st16(a1 + 0x4e, (uint16_t)gld_u16(e + 4));
    return a2 + 4;
}

/* ev_cam_ret_pc2 — guest @0x8017E9EC. `jr ra` with v0 = a2+2 in the
 * delay slot: the EV46 out-of-range / shared skip-2 leaf. */
uint32_t ev_cam_ret_pc2(uint32_t a0, uint32_t a1, uint32_t a2,
                        uint32_t a3)
{
    (void)a0; (void)a1; (void)a3;
    return a2 + 2;
}

/* ev_aux_byte_op — guest @0x8017E9F4 (EV op 47, table 0x801B4F24).
 * sub = sx8(pc[1]); only 0/1/2 act, everything else ret pc+4:
 *   0: ctrl[0xdf] = pc[2]
 *   1: ctrl[0xdf] = ctrl[0xdf] + pc[2]   (u8 wrap)
 *   2: *(u32*)(a1+0x94) = ctrl[0xdf] - sx16(pc+2)  (32-bit subu,
 *      negative wraps into the u32 store exactly like the guest) */
uint32_t ev_aux_byte_op(uint32_t a0, uint32_t a1, uint32_t a2,
                        uint32_t a3)
{
    (void)a3;
    int32_t sub = (int8_t)gld_u8(a2 + 1);
    uint32_t c_g = a0;
    switch (sub) {
    case 0:
        GSU8((uint8_t *)guest_at(c_g), 0xdf) = gld_u8(a2 + 2);
        break;
    case 1:
        GSU8((uint8_t *)guest_at(c_g), 0xdf) =
            (uint8_t)(gld_u8(c_g + 0xdf) + gld_u8(a2 + 2));
        break;
    case 2: {
        int32_t d = (int32_t)gld_u8(c_g + 0xdf) - gld_s16(a2 + 2);
        br2_st32(a1 + 0x94, (uint32_t)d);
        break;
    }
    default:
        break;
    }
    return a2 + 4;
}
