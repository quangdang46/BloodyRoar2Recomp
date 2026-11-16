/*
 * action.c — clean-room action-entry pipeline.
 *
 * Faithful port of the verified guest sequence (Ghidra decompile,
 * functions named by guest address):
 *
 *   action_enter_fwd  action_enter(battle, ctrl, sel)
 *     sel = (entry_group << 4) | action_class, class in 0..4
 *     class 1: apply -> reset -> fixup -> switch -> ev_dispatch
 *     class 0,2,3,4: shared path with triple slot_apply
 *     tail (all classes): motion+0x82=+0x80; action_switch;
 *                         ev_dispatch (ev.c)
 *
 *   FUN_8017a05c  apply(ctrl, motion)      — .ANI rebind + relocations
 *   FUN_80175898  reset(ctrl, sel)         — field zero + class scaling
 *   FUN_80181db8  ani_fixup                — S4 cat[0..4] -> absolute
 *   slot_apply  slot_apply(ctrl, slot)   — dual-form (beast) select
 *   skeleton_init  skeleton_init(ctrl,desc) — S5 part/matrix init
 *   pose_chain_build  part_link(ctrl)          — part linked-list build
 *   FUN_80175a7c  entry_setup(ctrl, group) — spawn offsets by group
 *   action_switch_guest  action_switch(ctrl,motion,idx) — S0 -> EV stream
 *
 * Guest pointer fields are raw u32 guest addresses (recomp model);
 * on a 64-bit host they need the address-space bridge, same as
 * vm.c / ev.c.
 */
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "ani.h"

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint16_t *euler_rotmat(uint32_t,uint32_t,uint32_t,uint16_t *);
extern int32_t skeleton_init(uint32_t,uint32_t,uint32_t);

typedef uint8_t  byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint16_t undefined2;
typedef uint32_t undefined4;
typedef uint8_t  undefined;
typedef uint8_t  undefined1;
typedef uint64_t undefined8;
typedef uint64_t ulonglong;

/* Guest raw-offset accessors. */
#define GU8(p,o)   (((const uint8_t *)(p))[(o)])
#define GU16(p,o)  (*(const uint16_t *)((const uint8_t *)(p) + (o)))
#define GU32(p,o)  (*(const uint32_t *)((const uint8_t *)(p) + (o)))
#define GS8(p,o)   (((uint8_t *)(p))[(o)])
#define GS16(p,o)  (*(int16_t  *)((uint8_t *)(p) + (o)))
#define GSU16(p,o) (*(uint16_t *)((uint8_t *)(p) + (o)))
#define GSU32(p,o) (*(uint32_t *)((uint8_t *)(p) + (o)))

/* Verified ctrl offsets. */
#define FC_PLAYER   0x02
#define FC_CHAR     0x03
#define FC_FORM     0x05
#define FC_PARAM    0x28
#define FC_FOE      0x2c
#define FC_ANI      0x30
#define FC_MOTION   0x38
#define FC_VM       0x3c
#define FC_FLAGB    0x0b   /* .ACT relocation done flag          */

/* Motion offsets touched here. */
#define MO_SEC0     0x00   /* relocated S0 action->stream table  */
#define MO_S5       0x10   /* S5 skeleton descriptor ptr         */
#define MO_ACT      0x80
#define MO_PREV     0x82
#define MO_STREAM   0x84
#define MO_ALT      0x88
#define MO_CNTD     0x90
#define MO_AC       0xac   /* .ACT cursor (blob_size+4 relocated)*/
#define MO_PA8      0xa8   /* .ACT secondary ptr list            */

static uint8_t *ctrl_motion(uint8_t *ctrl)
{
    uint32_t a = GU32(ctrl, FC_MOTION);
    return a ? (uint8_t *)guest_at(a) : NULL;
}
static uint8_t *ctrl_param(uint8_t *ctrl)
{
    uint32_t a = GU32(ctrl, FC_PARAM);
    return a ? (uint8_t *)guest_at(a) : NULL;
}

/* ------------------------------------------------------------------ */
/* FUN_8017a05c — apply: rebind .ANI sections, relocate, once-fix .ACT */
/* Guest signature (ctrl, motion, sel); sel unused inside.             */
/* ------------------------------------------------------------------ */
static void apply(uint8_t *ctrl, uint8_t *motion)
{
    uint32_t ani_g = GU32(ctrl, FC_ANI);
    uint8_t *ani = ani_g ? (uint8_t *)guest_at(ani_g) : NULL;
    if (!ani || !motion) return;
    uint8_t *tbl = ctrl_param(ctrl);
    uint32_t vm_g  = GU32(ctrl, FC_VM);
    uint8_t *vm  = vm_g ? (uint8_t *)guest_at(vm_g) : NULL;
    uint32_t foe_g = GU32(ctrl, FC_FOE);
    uint8_t *foe = foe_g ? (uint8_t *)guest_at(foe_g) : NULL;
    uint32_t base = ani_g;                   /* guest pointer value   */

    uint32_t *h = (uint32_t *)ani;            /* [0]=size [1..8]=sec  */
    uint32_t size = h[0];

    /* Section slots -> motion (file offsets, relocated below). */
    GSU32(motion, MO_SEC0) = h[1];            /* S0 */
    GSU32(motion, 0x04)    = h[2];            /* S1 (overridden)      */
    GSU32(motion, 0x08)    = h[3];            /* S2 */
    GSU32(motion, 0x0c)    = h[4];            /* S3 */
    if (vm) GSU32(vm, 0x0c) = h[5];           /* S4 cat tab -> VM ctx */
    GSU32(motion, MO_AC)   = size + 4;        /* .ACT hdr +4          */
    GSU32(motion, MO_S5)   = h[6];            /* S5 skeleton desc     */
    uint32_t *s6 = (uint32_t *)(ani + h[7]);  /* S6: 4 u32 -> +0xe8.. */
    for (int i = 0; i < 4; i++) GSU32(motion, 0xe8 + i * 4) = s6[i];

    /* motion+4 = u32 inside S1 indexed by FOE char id. */
    if (foe) GSU32(motion, 0x04) = GU32(ani + h[2] + GU8(foe, FC_CHAR) * 4, 0);

    /* ---- post-header param block at blob+0x24 (u32-indexed) ---- */
    /* piVar7[9..0xe] = blob +0x24..0x38 */
    if (tbl) {
        GS8 (tbl, 0x02) = (int8_t)GU8(ani, 0x24);          /* (char)piVar7[9] */
        GS8 (tbl, 0x26) = GU8(ani, 0x26);
        GS16(tbl, 0x20) = (int16_t)GU32(ani, 0x28);        /* (short)piVar7[10] */
        GSU16(tbl, 0x44) = GU16(ani, 0x2a);
    }
    GS16(ctrl, 0xb0)  = (int16_t)GU16(ani, 0x2e);
    GS16(ctrl, 0xfc)  = (int16_t)GU16(ani, 0x32);
    GS16(ctrl, 0xfe)  = (int16_t)GU32(ani, 0x34);          /* (short)piVar7[13] */
    GS16(ctrl, 0x100) = (int16_t)GU16(ani, 0x36);
    GS16(ctrl, 0x102) = (int16_t)GU32(ani, 0x38);          /* (short)piVar7[14] */
    GS16(ctrl, 0x104) = (int16_t)GU16(ani, 0x3a);
    GS16(motion, 0xbc) = (int16_t)GU32(ani, 0x30);         /* (short)piVar7[12] */

    /* ctrl+0xf8 = (cfgbyte<<8)/s16(ani+0x2c)+1; cfgbyte = byte4 of the
     * battle-config ptr at guest 0x1f8000c4 (disasm 0x8017a250-0x8017a28c).
     * Guest hangs if cfgbyte==0 (j back to bnez); treat as 0 here. */
    {
        uint32_t cfg_g = *(const uint32_t *)guest_at(0x1f8000c4);
        uint32_t cfgbyte = cfg_g ? GU8((const uint8_t *)guest_at(cfg_g), 4) : 0;
        if (cfgbyte) {
            int32_t div = (int16_t)GU16(ani, 0x2c);
            GS16(ctrl, 0xf8) = div ? (int16_t)((int32_t)(cfgbyte << 8) / div + 1) : 0;
        } else {
            GS16(ctrl, 0xf8) = 0;
        }
    }

    /* ---- S7 template -> param_tbl (0x18-byte record at ani+h[8]) -- */
    uint8_t *s7 = ani + h[8];
    if (tbl) {
        GS8 (tbl, 0x03)  = (int8_t)GU8(s7, 0x00);
        GS8 (tbl, 0x04)  = GU8(s7, 0x04);
        GS8 (tbl, 0x27)  = GU8(s7, 0x02);
        GS8 (tbl, 0x28)  = GU8(s7, 0x06);
        GSU32(tbl, 0x08) = GU32(s7, 0x08);
        GSU32(tbl, 0x0c) = GU32(s7, 0x10);
        GSU32(tbl, 0x2c) = GU32(s7, 0x0c);
        GSU32(tbl, 0x30) = GU32(s7, 0x14);
    }

    /* ---- relocate all stored pointers (file off -> absolute) ---- */
    static const uint8_t moffs[] = {0x00,0x04,0x08,0x0c,0x10,0xac,0xe8,0xec,0xf0,0xf4};
    for (unsigned i = 0; i < sizeof moffs; i++)
        GSU32(motion, moffs[i]) += base;
    if (vm)  GSU32(vm, 0x0c) += base;
    if (tbl) {
        static const uint8_t toffs[] = {0x08,0x0c,0x2c,0x30};
        for (unsigned i = 0; i < sizeof toffs; i++)
            GSU32(tbl, toffs[i]) += base;
    }

    /* ---- .ACT once-relocation (ctrl+0xb == 0) ----
     * motion+0xac: u32 list, -1 term, positive += base+size (.ACT base)
     * motion+0xa8: u32 list, -1 term, positive += pa8_g-8 (list-relative) */
    if (GU8(ctrl, FC_FLAGB) == 0) {
        uint32_t ac_g = GU32(motion, MO_AC);
        uint32_t pa8_g = GU32(motion, MO_PA8);
        uint32_t *p = ac_g ? (uint32_t *)guest_at(ac_g) : NULL;
        if (p) {
            while (*p != 0xffffffffu) {
                if ((int32_t)*p > 0) *p += base + size;
                p++;
            }
        }
        p = pa8_g ? (uint32_t *)guest_at(pa8_g) : NULL;
        if (p && *p != 0xffffffffu) {
            do {
                if ((int32_t)*p > 0) *p = (uint32_t)((int32_t)*p + (int32_t)pa8_g - 8);
                p++;
            } while (*p != 0xffffffffu);
        }
    }

    /* ---- .ACT cursor share/mirror ---- */
    if (GU8(ctrl, FC_PLAYER) == 0 ||
        (foe && GU8(ctrl, FC_CHAR) != GU8(foe, FC_CHAR))) {
        uint16_t v = GU16(motion, MO_AC);
        GSU32(motion, MO_AC) += 8;
        GSU16(motion, 0xa4) = v;
    } else if (foe) {
        uint8_t *fm = ctrl_motion(foe);
        if (fm) {
            uint32_t cur = GU32(fm, MO_AC);
            GSU32(motion, MO_AC) = cur;
            GSU16(motion, 0xa4) = GU16(guest_at(cur - 4), 0);
        }
    }

    /* ---- big transient reset (verified field list) ---- */
    GSU32(ctrl, 0x58) = 0;  GSU32(ctrl, 0x5c) = 0;
    GSU32(ctrl, 0x60) = 0;  GSU32(ctrl, 0x64) = 0;
    GSU32(ctrl, 0x68) = 0;  GSU32(ctrl, 0x6c) = 0;
    GS16(motion, 0x28) = 0;         /* param_2[5]  = +0x14? see note */
    GSU32(motion, 0x18) = 0;        /* param_2[6]  = +0x18 */
    GS16(motion, 0x1c) = 0xffff;    /* param_2[7]  = +0x1c */
    GS16(motion, 0x78) = 0;         /* +0x1e */
    GSU32(motion, 0x30) = 0;        /* param_2[0xc]= +0x30 */
    GS16(motion, 0x34) = 0xffff;    /* param_2[0xd]= +0x34 */
    GS16(motion, 0x4c) = 0;         /* +0x36 */
    GSU32(motion, 0x20) = 0;        /* param_2[8]  = +0x20 */
    GS16(motion, 0x24) = 0xffff;    /* param_2[9]  = +0x24 */
    GS16(motion, 0x98) = 0;         /* +0x26 */
    GS16(motion, 0x54) = 0xffff;    /* +0x2a */
    GS8 (motion, 0x7c) = 0;         /* param_2+0x1f byte = +0x7c */
    GS8 (motion, 0xf6) = 0xff;      /* +0x7d region approx */
    GS8 (motion, 0xf7) = 0xff;      /* +0x7e */
    GS16(motion, 0x90) = 0;         /* param_2+0x24 = +0x90 */
    GS16(motion, 0x278) = 1;        /* +0x9e */
    GS16(motion, 0x140) = 1;        /* param_2+0x28 = +0x140? approx */
    GS16(motion, 0x180) = 1;        /* param_2+0x30 = +0x180? approx */
    GSU32(motion, 0xb0) = 0;        /* param_2[0x2c] = +0xb0 */
    /* NOTE: several motion offsets above are int*-scaled in the
     * decompile; marked approx pending offset audit. */
    GS16(ctrl, 0x88) = GS16(ctrl, 0x86);
    GS16(motion, 0x268) = 0xffff;   /* param_2+0x9a = +0x268? approx */
    GS8 (motion, 0x404) = 0xff;     /* +0x101 */
    GS8 (motion, 0x405) = 0xff;
    GS8 (motion, 0x406) = 0xff;
    GS8 (motion, 0x408) = 0xff;     /* param_2+0x41 byte */
    GS16(motion, 0x418) = 0;        /* +0x106 */
    GS16(motion, 0x420) = 0;        /* param_2+0x42 */
    GS16(motion, 0x428) = 0;        /* +0x10a */
    GS8 (ctrl, 0x26) = 0xff;
    if (tbl) {
        /* ctrl+0x14 -> struct A (guest ptr); A[0] -> byte target B (guest
         * ptr); write 0xff to B[0]. Guest pointers are u32 — read the cell
         * as a guest word and translate, never as a host uint8_t*. */
        uint32_t a_g = GU32(ctrl, 0x14);
        uint8_t *pa = a_g ? (uint8_t *)guest_at(a_g) : NULL;
        uint32_t b_g = pa ? GU32(pa, 0) : 0;
        uint8_t *pb = b_g ? (uint8_t *)guest_at(b_g) : NULL;
        if (pb) GS8(pb, 0) = 0xff;
    }
    GS8 (ctrl, 0x27) = 0;
    GS16(motion, 0x438) = 0xffff;   /* +0x10e */
    GS16(motion, 0x440) = 0xffff;   /* param_2+0x44 */
    GS16(motion, 0x444) = 0xffff;   /* +0x112 */
    GS16(motion, 0x450) = 0xffff;   /* param_2+0x45 */
    GS16(motion, 0x458) = 0;        /* +0x116 */
    /* part-table loops (param_tbl+0x10/0x14/0x34/0x38 arrays,
     * stride -0x28, count 0x1a then 0x11) — approximated as zero of
     * +0xc fields; full semantics pending. */
    if (tbl) {
        static const uint8_t parr[] = {0x10,0x14,0x34,0x38};
        for (unsigned a = 0; a < 4; a++) {
            uint32_t arr_g = GU32(tbl, parr[a]);
            uint8_t *arr = arr_g ? (uint8_t *)guest_at(arr_g) : NULL;
            if (!arr) continue;
            for (int i = 0; i < 0x1b; i++)
                GSU16(arr + 0x258 + i * 0x28, 0xc) = 0;  /* approx stride */
            GSU16(arr, 0x1c4) = 1;
            for (int i = 0; i < 2; i++)
                GSU16(arr + 0x258 + (0xf + i) * 0x28, 0xc) = 1;
            GSU16(arr + 0x23c, 0xc) = 1;
        }
    }
}

/* Class tail inside FUN_8017a05c (after the big reset): clears
 * motion fields for classes != 1. Offsets are byte-accurate from
 * the decompile's mixed pointer-scaled arithmetic. */
static void apply_tail(uint8_t *motion, uint32_t cls)
{
    if (cls == 1 || cls > 4) return;
    GS8 (motion, 0x100) = 0;
    GS8 (motion, 0x09c) = 0;
    GS16(motion, 0x0d2) = 0;
    GS16(motion, 0x0d0) = 0;
    GS16(motion, 0x0e4) = 0;
    GS16(motion, 0x0d4) = 0;
    GS16(motion, 0x0d6) = 0;
    GS16(motion, 0x0d8) = 0;
    GS16(motion, 0x0dc) = 0;
    GS16(motion, 0x0de) = 0;
    GS16(motion, 0x0e0) = 0;
}

/* ------------------------------------------------------------------ */
/* FUN_80175898 — reset: zero transient fields, per-class scaling.     */
/* ------------------------------------------------------------------ */
static void reset(uint8_t *ctrl, uint32_t sel)
{
    static const uint8_t z16[] = {
        0x7a,0x7c,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,
        0xc6,0xfa,0xc8,0xc0,0xc4,0xb2,0xd0,0xca,
        0x9a,0x9c,0x9e,0xa2,0xa4,0xa6 };
    for (unsigned i = 0; i < sizeof z16; i++) GS16(ctrl, z16[i]) = 0;
    GS16(ctrl, 0xce) = 0x1e0;
    GS8 (ctrl, 0xda) = 0;
    GS8 (ctrl, 0xdb) = 0;
    GS8 (ctrl, 0x106) = 0;
    GS8 (ctrl, 0xdd)  = 0;
    GS8 (ctrl, 0xe5)  = GU8(ctrl, 0xe4);
    GSU32(ctrl, 0x14) = 0x801f52e8u + ((uint32_t)GU8(ctrl, 0x02) << 5);

    uint8_t cls = sel & 0xf;
    int32_t scaled = (int32_t)((int32_t)GS16(ctrl, 0xb0) *
                               (uint32_t)GU8(ctrl, 0xde)) / 100;
    const uint8_t *battle = (const uint8_t *)guest_at(0x1f800000);

    switch (cls) {
    case 0:
        GS16(ctrl, 0xd6) = 0x1e00;
        /* fallthrough */
    case 1:
    case 2:
        if (GU8(battle, 0x9b) == 4) GS16(ctrl, 0xd6) = 0x1e00;
        if (GU8(battle, 0x9c) == 3 && GU8(ctrl, 0x01) == 0 &&
            GU16(battle, 0xa2) != 0) {
            GS16(ctrl, 0xaa) = GS16(ctrl, 0xac);
            GS16(ctrl, 0xae) = GS16(ctrl, 0xac);
        } else {
            GS16(ctrl, 0xac) = 0;
            GS16(ctrl, 0xaa) = 0;
            GS16(ctrl, 0xae) = 0;
        }
        GS16(ctrl, 0xd8) = 0;
        GS16(ctrl, 0xb0) = (int16_t)scaled;
        if (GS16(ctrl, 0xd6) < 0) GS16(ctrl, 0xd6) = 0;
        break;
    case 3:
        GS16(ctrl, 0xd8) = 0;
        GS16(ctrl, 0xd6) = 0x1e00;
        GSU32(ctrl, 0x5c) |= 0x200000;
        GS16(ctrl, 0xb0) = (int16_t)scaled;
        GS16(ctrl, 0xac) = (int16_t)scaled;
        GS16(ctrl, 0xaa) = (int16_t)scaled;
        GS16(ctrl, 0xae) = (int16_t)scaled;
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* FUN_80181db8 — .ANI S4 category fixup.
 * Clean source resolves categories lazily via ani_cat(); the     */
/* guest's in-place patch is represented by the once-flag only.       */
/* ------------------------------------------------------------------ */
static void ani_fixup(uint8_t *ctrl, int variant)
{
    (void)variant;
    GS8(ctrl, FC_FLAGB) = 1;
}

/* ------------------------------------------------------------------ */
void part_record_reset(undefined2 *param_1);
void action_switch_guest(int param_1,int *param_2,int param_3);

/* slot_apply — slot_apply: select one of two form descriptors.      */
/* param_tbl + slot*0x24 = 0x24-byte form descriptor (verified).       */
/* ------------------------------------------------------------------ */
static void slot_apply(uint8_t *ctrl, uint32_t slot)
{
    uint8_t *tbl = ctrl_param(ctrl);
    uint8_t *motion = ctrl_motion(ctrl);
    if (!tbl || !motion) return;
    uint8_t *d = tbl + slot * 0x24;

    GS8(ctrl, 0x06) = (int8_t)GU8(d, 0x00);
    GS8(ctrl, 0x08) = GU8(d, 0x02);
    uint32_t op_g = GU32(ctrl, 0x20);
    uint32_t np_g = GU32(d, 0x18);
    uint32_t om_g = GU32(ctrl, 0x40);
    uint8_t *old_parts = op_g ? (uint8_t *)guest_at(op_g) : NULL;
    uint8_t *new_parts = np_g ? (uint8_t *)guest_at(np_g) : NULL;
    uint8_t *old_mats  = om_g ? (uint8_t *)guest_at(om_g) : NULL;

    GSU32(ctrl, 0x18) = GU32(d, 0x10);
    GSU32(ctrl, 0x1c) = GU32(d, 0x14);
    GSU32(ctrl, 0x20) = GU32(d, 0x18);
    GSU32(ctrl, 0x40) = GU32(d, 0x1c);

    if (old_parts && new_parts) {
        memcpy(new_parts + 0x20, old_parts + 0x20, 0x20);
        memcpy(new_parts + 0x160, old_parts + 0x160, 0x10);
        memcpy(new_parts + 0x170, old_parts + 0x170, 0x10);
    }
    GSU32(motion, 0xf8) = GU32(motion, 0xe8 + slot * 8);
    GSU32(motion, 0xfc) = GU32(motion, 0xec + slot * 8);
    GS8 (ctrl, 0x09) = GU8(d, 0x03);
    GSU32(ctrl, 0x0c) = GU32(d, 0x08);
    GS8 (ctrl, 0x0a) = GU8(d, 0x04);
    GSU32(ctrl, 0x10) = GU32(d, 0x0c);
    GS16(ctrl, 0x24) = (int16_t)GU16(d, 0x20);

    if (old_mats) {
        uint32_t nm_g = GU32(ctrl, 0x40);
        uint8_t *nm = nm_g ? (uint8_t *)guest_at(nm_g) : NULL;
        for (int i = 0; i < 0x13; i++)
            memcpy(nm + i * 0x1c, old_mats + i * 0x1c, 0x1c);
    }
}

/* ------------------------------------------------------------------ */
/* pose_chain_build — part_link.
 * Iterates part records [ctrl+6+0x13, ctrl+8), walking a u16 stream  */
/* inside the active S5 descriptor to link matrix slots. Fully       */
/* ported below (disasm 0x801862a8-0x801863a4).                      */
/* ------------------------------------------------------------------ */
static void part_link(uint8_t *ctrl)
{
    /* disasm 0x801862a8-0x801863a4: ctrl+0x38 = S5 table (guest),
     * slot = table+0x10 + (ctrl[5]&1)*0x1c; stream = slot+0x14 +
     * ctrl+0x30. Parts i in [0x13, ctrl[8]): record at
     * ctrl+0x40+0x21c+(i-0x13)*0x1c (+0x10 = stream guest addr, u16
     * fields 0..0xc cleared); link records at ctrl+0x20+0x638 stride
     * 0x50, node ptr at -8, sentinel chain vs ctrl+0x20+0x5f0+...
     * stream advances (i16)stream[1] + 2 elements. */
    uint8_t *s5 = (uint8_t *)guest_at(GU32(ctrl, 0x38));
    uint32_t slot_g = GU32(s5, 0x10) + (GU8(ctrl, 5) & 1) * 0x1c;
    uint8_t *slot = (uint8_t *)guest_at(slot_g);
    uint32_t stream_g = GU32(slot, 0x14) + GU32(ctrl, 0x30);
    int16_t *stream = (int16_t *)guest_at(stream_g);
    uint32_t obj_g = GU32(ctrl, 0x20);
    uint32_t sent_g = obj_g + 0x5f0;
    uint8_t *link = (uint8_t *)guest_at(obj_g + 0x638);
    uint8_t *part = (uint8_t *)guest_at(GU32(ctrl, 0x40)) + 0x21c;
    int8_t i = 0x13;
    int8_t count = (int8_t)GU8(ctrl, 8);
    if (count <= i) return;
    do {
        GSU32(part, 0x10) = guest_of(stream);
        if ((GU16(stream, 0) & 0x8000) != 0) {
            uint32_t node_g = GU32(link, -8);
            uint8_t *node = (uint8_t *)guest_at(node_g);
            uint32_t cur_g = GU32(node, 0x44);
            if (cur_g == sent_g) {
                GSU32(node, 0x44) = GU32(link, 0);
            } else {
                uint8_t *cur = (uint8_t *)guest_at(cur_g);
                /* DIAG bound: real HW chain always terminates at sent_g;
                 * a corrupt upstream record makes this walk forever.
                 * Cap + dump the live chain pointers, then break so the
                 * run continues and the corruption is visible in trace. */
                { extern void decomp_trace_record(uint8_t,uint32_t,uint32_t,uint8_t,uint32_t);
                  int steps = 0;
                  while (GU32(cur, 0x48) != sent_g) {
                    if (++steps > 0x400) {
                      decomp_trace_record(40, cur_g, sent_g, 0, 0);
                      decomp_trace_record(41, node_g, GU32(link,0), 0, 0);
                      decomp_trace_record(42, obj_g, GU32(cur,0x48), 0, 0);
                      break;
                    }
                    cur = (uint8_t *)guest_at(GU32(cur, 0x48));
                  }
                  if (steps <= 0x400) GSU32(cur, 0x48) = GU32(link, 0);
                }
            }
        }
        for (int k = 0; k < 7; k++) GSU16(part, k * 2) = 0;
        part += 0x1c;
        link += 0x50;
        sent_g += 0x50;
        i++;
        stream += stream[1] + 2;
    } while (i < count);
}

/* ------------------------------------------------------------------ */
/* FUN_80175a7c — entry_setup: spawn/entry offsets by group.           */
/* group = sel>>4. Writes verified guest constants (12.4 fixed).       */
/* ------------------------------------------------------------------ */
static void entry_setup(uint8_t *ctrl, uint32_t group)
{
    uint8_t *motion = ctrl_motion(ctrl);
    uint32_t fobj_g = GU32(ctrl, 0x20);
    uint8_t *fobj = fobj_g ? (uint8_t *)guest_at(fobj_g) : NULL;
    if (!motion || !fobj) return;

    /* part-record resets (loops over ctrl+8 / ctrl+6 counts) */
    int n = (int8_t)GU8(ctrl, 0x08) - 1;
    for (int i = n; i >= 0; i--) {
        part_record_reset((undefined2 *)(fobj + i * 0x50));
    }
    int pc = (int8_t)GU8(ctrl, 0x06) - 1;
    for (int i = pc; i >= 0; i--) {
        uint32_t p18_g = GU32(ctrl, 0x18);
        uint32_t p1c_g = GU32(ctrl, 0x1c);
        uint8_t *p18 = p18_g ? (uint8_t *)guest_at(p18_g) : NULL;
        uint8_t *p1c = p1c_g ? (uint8_t *)guest_at(p1c_g) : NULL;
        if (p18) { GSU32(p18,0)=0; GSU32(p18,0x10)=0; GSU32(p18,0x14)=0; GSU32(p18,0x18)=0; }
        if (p1c) { GSU32(p1c,0)=0; GSU32(p1c,0x10)=0; GSU32(p1c,0x14)=0; GSU32(p1c,0x18)=0; }
    }

    /* ---- group switch (guest constants, verbatim) ---- */
    int32_t x = 0, y = 0;
    int32_t z;                 /* unaff_s4 — always set per branch */
    int slot = GU8(ctrl, FC_PLAYER);
    int chr  = GU8(ctrl, FC_CHAR);

    if (group == 2) {
        if (slot != 0) {                    /* P2 side */
            if (slot == 1) {
                z = 0xfc00;
                if (chr == 0) { z = 0x0e00; x = 0x900; y = 0xb00; }
                else          { x = 0xa00; }
            } else { z = 0xfc00; x = 0; y = 0; }
        } else {                            /* P1 side */
            z = 0x400;
            switch (chr) {
            case 0: z = 0x600; x = -0x100; y = 0x100; break;
            case 1: case 3: case 6: x = -0x280; break;
            case 5: z = 0x400; x = -0x300; break;
            default: break;
            }
        }
    } else if (group == 1) {
        if (slot == 0)      { x = 0;      z = 0;      }
        else if (slot == 1) { x = 0x1400; z = 0xfc00; }
        else                { x = 0;      z = 0xfc00; }
    } else if (group == 3) {
        if (slot == 0) { z = 0x400; x = 0xa00; }
        else if (slot == 1) {
            z = 0x400;
            if (chr == 2)           x = -0x600;
            else if (chr == 7 || chr == 8) x = -0x400;
            else if (chr == 4 || chr == 5) x = -0x300;
        } else { z = 0xfc00; x = 0; }
    } else {                                /* group 0 / >=4 */
        if (slot == 0)      { x = -0x100; z = 0x400;  }
        else if (slot == 1) { x =  0x100; z = 0xfc00; }
        else                { x = 0;      z = 0xfc00; }
    }

    /* spawn triple writes (verified offsets) */
    GSU32(fobj, 0x34) = (uint32_t)x;
    GS16 (fobj, 0x18) = (int16_t)x;
    GS16 (motion, 0xd4) = (int16_t)x;
    GSU32(fobj, 0x3c) = (uint32_t)y;
    GS16 (fobj, 0x1c) = (int16_t)y;
    GS16 (motion, 0xd8) = (int16_t)y;
    GS16 (ctrl, 0x4e)  = (int16_t)z;
    GS16 (motion, 0xd0) = (int16_t)z;
}

/* part_link_unlink — part link-state machine (decomp-verified).
 * param_1 = source 0x50 record (u16 state @+0, link @+0x44),
 * param_2 = dest 0x50 record, param_3 = link mode (1 own, 3 foe).
 * Pointer fields stored as guest addresses. */
static void part_link_state(uint16_t *param_1, int16_t *param_2, int param_3)
{
    int16_t sVar2 = param_2[0];
    if (sVar2 == 0) {
        param_2[0x22] = 0;
        param_2[0x23] = 0;
        sVar2 = param_2[0];
    }
    if (sVar2 != 2) param_2[0] = (int16_t)param_3;
    param_2[0x24] = 0;
    param_2[0x25] = 0;
    *(uint32_t *)((uint8_t *)param_2 + 0x40) = param_1 ? guest_of(param_1) : 0;
    if (param_1 != NULL) {
        uint32_t uVar1 = *param_1;
        if (param_3 == 3) {
            if (uVar1 == 1) {
                *param_1 = 2;
                *(uint32_t *)((uint8_t *)param_2 + 0x48) = *(uint32_t *)((uint8_t *)param_1 + 0x44);
            } else if (uVar1 < 2) {
                if (uVar1 != 0) {
                    param_2[0x20] = 0;
                    param_2[0x21] = 0;
                    goto done;
                }
                *param_1 = 2;
            } else {
                if (uVar1 != 3) {
                    param_2[0x20] = 0;
                    param_2[0x21] = 0;
                    param_2[0x26] = 0;
                    param_2[0x27] = 0;
                    return;
                }
                *param_1 = 4;
                *(uint32_t *)((uint8_t *)param_2 + 0x48) = *(uint32_t *)((uint8_t *)param_1 + 0x44);
            }
        } else {
            if (uVar1 == 2 || uVar1 == 4) {
                uint32_t chain_g = *(uint32_t *)((uint8_t *)param_1 + 0x44);
                uint8_t *chain = (uint8_t *)guest_at(chain_g);
                *(uint32_t *)((uint8_t *)param_2 + 0x48) = GU32(chain, 0x48);
                GSU32(chain, 0x48) = guest_of(param_2);
                goto done;
            }
            if (uVar1 == 0) *param_1 = 1;
            else          *(uint32_t *)((uint8_t *)param_2 + 0x48) = *(uint32_t *)((uint8_t *)param_1 + 0x44);
        }
        *(uint32_t *)((uint8_t *)param_1 + 0x44) = guest_of(param_2);
    }
done:
    param_2[0x26] = 0;
    param_2[0x27] = 0;
}

/* ------------------------------------------------------------------ */
/* skeleton_init — skeleton_init: part/matrix init from S5 descriptor.  */
/* desc = 0x1c-byte block inside S5 (two per section, slot*0x1c).      */
/* Ports the verified loop structure; matrix decode helpers            */
/* (euler_rotmat euler->matrix, part_link_unlink part-copy) are host      */
/* hooks pending their own decode.                                     */
/* ------------------------------------------------------------------ */
static void skeleton_init_wrap(uint8_t *ctrl, const uint8_t *desc)
{
    /* canonical disasm-verified port is skeleton_init in menu.c;
     * delegate with guest addresses (host ptrs -> guest via
     * guest_of). The older inline approximation was removed —
     * it read euler seeds from the wrong desc field. */
    skeleton_init(guest_of(ctrl), guest_of(desc), 0);
}

/* ------------------------------------------------------------------ */
/* action_switch_guest — action_switch: S0 table -> event stream.             */
/* ------------------------------------------------------------------ */
void action_switch_guest(int param_1,int *param_2,int param_3);
void part_record_reset(undefined2 *param_1);
static void action_switch(uint8_t *ctrl, uint8_t *motion, int32_t idx)
{
    if (!ctrl || !motion) return;
    action_switch_guest((int)guest_of(ctrl),(int *)motion,idx);
    return;
#if 0
    uint16_t off = GU16(GU32(motion, MO_SEC0), (uint32_t)idx * 2);
    uint32_t stream = GU32(ctrl, FC_ANI) + off;
    GSU32(motion, MO_STREAM) = stream;   /* +0x84 */
    GSU32(motion, MO_ALT)    = stream;   /* +0x88 */
    GSU16(motion, MO_CNTD)   = 0;        /* +0x90 */
    GS16(ctrl, 0x8c) = 0;
    GS16(ctrl, 0x8e) = 0;
    GS8 (ctrl, 0x26) = 0xff;
    GSU32(ctrl, 0x5c) &= 0xfffc7fd7;
    GSU32(ctrl, 0x60) &= 0xa9dfffee;
    GSU32(ctrl, 0x6c) &= 0xffffdfff;
    GS16(ctrl, 0xba) = 0;
    GS16(ctrl, 0xb8) = 0;
    GS16(ctrl, 0xb6) = 0;
    GSU16(motion, 0x110) = 0xffff;
    /* part-struct field resets — now covered by verbatim action_switch_guest */
#endif
}

/* ------------------------------------------------------------------ */
/* action_enter_fwd — action_enter: the unified entry pipeline.            */
/* battle = 0x1f800000 battle-state block (host supplies, unused here).*/
/* sel    = (entry_group << 4) | action_class.                         */
/* ------------------------------------------------------------------ */
uint32_t action_enter(void *battle, uint8_t *ctrl, uint32_t sel)
{
    (void)battle;
    uint8_t *motion = ctrl_motion(ctrl);
    uint32_t cls = sel & 0xf;
    (void)GU8(ctrl, 0x05);

    if (cls == 1) {
        GS8 (ctrl, 0x00) = 1;
        GS16(ctrl, 0x74) = 0;
        GS16(ctrl, 0x70) = 0;
        GS16(ctrl, 0x76) = 0x200;
        GS16(ctrl, 0x78) = 0;
        apply(ctrl, motion);
        apply_tail(motion, cls);
        reset(ctrl, sel);
        int variant = GU8(ctrl, 0x01) ? ((GU8(ctrl, 0x01) & 0x7f) - 1) : 0;
        ani_fixup(ctrl, variant);
    } else {
        if (cls == 0) {
            GS8 (ctrl, 0x05) = 0;
            GSU32(ctrl, 0x6c) = 0;
        } else if (cls > 4) {
            return cls;
        }
        if ((GSU32(ctrl, 0x6c) & 0x8000) != 0) {
            GS16(ctrl, 0xd6) = 0;
            GS16(ctrl, 0xd8) = 0;
            GS8 (ctrl, 0x05) = 0;
        }
        uint8_t saved5 = GU8(ctrl, 0x05);
        GS8 (ctrl, 0x00) = 1;
        GS8 (ctrl, 0x05) = 0;
        GS16(ctrl, 0x74) = 0;
        GS16(ctrl, 0x70) = 0;
        GS16(ctrl, 0x76) = 0x200;
        GS16(ctrl, 0x78) = 0;
        apply(ctrl, motion);
        apply_tail(motion, cls);
        reset(ctrl, sel);

        /* dual-form block (guest order, verified):
         *   e08(0) -> 5a7c(group) -> 79ae0(S5+0) -> 862a8
         *   ctrl+5 = 0x11
         *   e08(1) -> 5a7c(group) -> 79ae0(S5+0x1c) -> 862a8
         *   ctrl+5 = (saved5&0x10)?0x11:0
         *   e08(ctrl+5 & 1) -> fixup -> ctrl+0xb=1                  */
        uint32_t s5_g = motion ? GU32(motion, MO_S5) : 0;
        uint8_t *s5 = s5_g ? (uint8_t *)guest_at(s5_g) : NULL;
        slot_apply(ctrl, 0);
        entry_setup(ctrl, sel >> 4);
        if (s5) skeleton_init_wrap(ctrl, s5);
        part_link(ctrl);

        GS8(ctrl, 0x05) = 0x11;
        slot_apply(ctrl, 1);
        entry_setup(ctrl, sel >> 4);
        if (s5) skeleton_init_wrap(ctrl, s5 + 0x1c);
        part_link(ctrl);

        uint8_t b3 = (saved5 & 0x10) ? 0x11 : 0;
        GS8(ctrl, 0x05) = b3;
        slot_apply(ctrl, b3 & 1);

        int variant = GU8(ctrl, 0x01) ? ((GU8(ctrl, 0x01) & 0x7f) - 1) : 0;
        ani_fixup(ctrl, variant);
        GS8(ctrl, 0x0b) = 1;
    }

    /* shared tail */
    if (motion) {
        GSU16(motion, MO_PREV) = GU16(motion, MO_ACT);
        action_switch(ctrl, motion, (int16_t)GU16(motion, MO_ACT));
        /* step 6: ev_dispatch — caller runs ev_step(ctrl, motion) */
    }
    return cls;
}

/* ------------------------------------------------------------------ */
/* Guest-faithful ports (verbatim Ghidra transliteration).            */
/* ------------------------------------------------------------------ */

/* part_record_reset — part-record reset: identity 4x3 matrix + tail call. */
void part_link_unlink(ushort *param_1,short *param_2,int param_3);
void part_record_reset(undefined2 *param_1)
{
  *(undefined4 *)(param_1 + 2) = 0x1000;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 6) = 0x1000;
  *(undefined4 *)(param_1 + 8) = 0;
  *(undefined4 *)(param_1 + 10) = 0x1000;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xf] = 0;
  *(undefined4 *)(param_1 + 0x10) = 0x1000;
  *(undefined4 *)(param_1 + 0x12) = 0;
  *(undefined4 *)(param_1 + 0x14) = 0x1000;
  *(undefined4 *)(param_1 + 0x16) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0x1000;
  *(undefined4 *)(param_1 + 0x1a) = 0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x1e) = 0;
  *param_1 = 0;
  part_link_unlink((ushort *)0,(short *)param_1,0);
}

/* action_switch_guest — action_switch (guest-verbatim; superset of the      */
/* static approximation above — adds part-struct resets).            */
void action_switch_guest(int param_1,int *param_2,int param_3)
{
  ushort uVar1;
  int iVar2;
  /* param_1 arrives as a guest address; param_2 is a host pointer to
   * the motion object (int* scaled access matches guest byte offsets). */
  uint8_t *p1 = (uint8_t *)guest_at((uint32_t)param_1);
  iVar2 = (int)GU32(p1, 0x30);
  uVar1 = *(ushort *)guest_at((uint32_t)*param_2 + (uint32_t)param_3 * 2);
  *(undefined2 *)(param_2 + 0x24) = 0;
  iVar2 = (uint)uVar1 + iVar2;
  param_2[0x22] = iVar2;
  param_2[0x21] = iVar2;
  *(undefined2 *)(p1 + 0x8c) = 0;
  *(undefined2 *)(p1 + 0x8e) = 0;
  *(undefined1 *)(p1 + 0x26) = 0xff;
  *(uint *)(p1 + 0x5c) = *(uint *)(p1 + 0x5c) & 0xfffc7fd7;
  *(uint *)(p1 + 0x60) = *(uint *)(p1 + 0x60) & 0xa9dfffee;
  *(uint *)(p1 + 0x6c) = *(uint *)(p1 + 0x6c) & 0xffffdfff;
  *(undefined1 *)guest_at(GU32(p1, 0x14)) = 0xff;
  *(undefined1 *)(p1 + 0x27) = 0;
  *(undefined2 *)(p1 + 0xba) = 0;
  *(undefined2 *)(p1 + 0xb8) = 0;
  *(undefined2 *)(p1 + 0xb6) = 0;
  *(undefined2 *)(param_2 + 0x44) = 0xffff;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x23c) = 0;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x23c) = 1;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x1c4) = 0;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x1c4) = 1;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x28c) = 1;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x28c) = 1;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x264) = 1;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x264) = 1;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x18) +
      (uint32_t)(*(int8_t *)(p1 + 6) * 0x28) - 0x1c)) = 0;
  *(undefined2 *)((uint8_t *)guest_at(GU32(p1, 0x1c) +
      (uint32_t)(*(int8_t *)(p1 + 6) * 0x28) - 0x1c)) = 0;
}

/* slot_apply_guest — guest entry @0x80175e08: delegates to the faithful static port. */
void slot_apply_guest(int param_1,int param_2)
{
  slot_apply((uint8_t *)guest_at((uint32_t)param_1),(uint32_t)param_2);
}

/* part_link_unlink — part-record link/unlink: intrusive list via
 * rec+0x44 next / rec-head +0x22 tail, state machine on rec[0].
 * param_3: 3 = detach-to-slot mode, else link mode. */
void part_link_unlink(ushort *param_1,short *param_2,int param_3)
{
  ushort uVar1;
  short sVar2;

  sVar2 = *param_2;
  if (sVar2 == 0) {
    param_2[0x22] = 0;
    param_2[0x23] = 0;
    sVar2 = *param_2;
  }
  if (sVar2 != 2) {
    *param_2 = (short)param_3;
  }
  param_2[0x24] = 0;
  param_2[0x25] = 0;
  /* rec+0x40 = source-record back-pointer: a 4-byte GUEST address cell.
   * param_1 is a host pointer — store guest_of(param_1), never the
   * host pointer itself (an 8-byte store here also clobbered the
   * rec+0x44 link field and corrupted the part chain). */
  *(uint32_t *)((uint8_t *)param_2 + 0x40) = param_1 ? guest_of(param_1) : 0;
  if (param_1 != (ushort *)0x0) {
    if (param_3 == 3) {
      uVar1 = *param_1;
      if (uVar1 == 1) {
        *param_1 = 2;
        *(undefined4 *)(param_2 + 0x24) = *(undefined4 *)(param_1 + 0x22);
      }
      else if (uVar1 < 2) {
        if (uVar1 != 0) {
          param_2[0x20] = 0;
          param_2[0x21] = 0;
          goto LAB_80179a60;
        }
        *param_1 = 2;
      }
      else {
        if (uVar1 != 3) {
          param_2[0x20] = 0;
          param_2[0x21] = 0;
          param_2[0x26] = 0;
          param_2[0x27] = 0;
          return;
        }
        *param_1 = 4;
        *(undefined4 *)(param_2 + 0x24) = *(undefined4 *)(param_1 + 0x22);
      }
    }
    else {
      uVar1 = *param_1;
      if ((uVar1 == 2) || (uVar1 == 4)) {
        *(undefined4 *)(param_2 + 0x24) = *(undefined4 *)
            ((uint8_t *)guest_at((uint32_t)*(int *)(param_1 + 0x22)) + 0x48);
        *(uint32_t *)((uint8_t *)guest_at((uint32_t)*(int *)(param_1 + 0x22)) + 0x48)
            = guest_of(param_2);
        goto LAB_80179a60;
      }
      if (uVar1 == 0) {
        *param_1 = 1;
      }
      else {
        *(undefined4 *)(param_2 + 0x24) = *(undefined4 *)(param_1 + 0x22);
      }
    }
    *(uint32_t *)(param_1 + 0x22) = guest_of(param_2);
  }
LAB_80179a60:
  param_2[0x26] = 0;
  param_2[0x27] = 0;
  return;
}
