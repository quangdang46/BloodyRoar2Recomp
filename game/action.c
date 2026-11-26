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
#include "cpu_state.h"

extern void *guest_at(uint32_t addr);
extern uint32_t guest_of(const void *p);
extern uint16_t *euler_rotmat(uint32_t,uint32_t,uint32_t,uint16_t *);
extern int32_t skeleton_init(uint32_t,uint32_t,uint32_t);

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
    /* Guest loads ctrl+0x38 and dereferences it unconditionally
     * (slot_apply/entry_setup/action_enter). guest_at(0) is the arena
     * base, so a zero field still produces a valid low-memory pointer —
     * never collapse it to NULL. */
    return (uint8_t *)guest_at(GU32(ctrl, FC_MOTION));
}
static uint8_t *ctrl_param(uint8_t *ctrl)
{
    /* Same rule for ctrl+0x28 (param/form table): unconditional lw. */
    return (uint8_t *)guest_at(GU32(ctrl, FC_PARAM));
}

/* ------------------------------------------------------------------ */
/* FUN_8017a05c — apply: rebind .ANI sections, relocate, once-fix .ACT */
/* Guest signature (ctrl, motion, sel); sel unused inside.             */
/* ------------------------------------------------------------------ */
static void apply(uint8_t *ctrl, uint8_t *motion)
{
    uint32_t ani_g = GU32(ctrl, FC_ANI);
    /* func_8017A05C: lw a3,0x30(ctrl) then lw 0(a3)/lw 4(a3)/... with no
     * zero test — ani==0 reads/writes guest low memory via guest_at(0). */
    uint8_t *ani = (uint8_t *)guest_at(ani_g);
    uint8_t *tbl = ctrl_param(ctrl);
    uint32_t vm_g  = GU32(ctrl, FC_VM);
    /* lw v1,0x3c(ctrl); sw sec[4],0xc(v1) @0x8017A090 — unconditional. */
    uint8_t *vm  = (uint8_t *)guest_at(vm_g);
    uint32_t foe_g = GU32(ctrl, FC_FOE);
    /* The guest never NULL-checks foe: lbu 3(foe) at 0x8017A0C4 and
     * 0x8017A3A0 reads guest addr 3 when FC_FOE==0, and the mirror tail
     * lw's *(foe+0x38)+0xac unconditionally. guest_at(0) is the arena
     * base, so dereferencing it reproduces those low-memory reads. */
    uint8_t *foe = (uint8_t *)guest_at(foe_g);
    uint32_t base = ani_g;                   /* guest pointer value   */

    uint32_t *h = (uint32_t *)ani;            /* [0]=size [1..8]=sec  */
    uint32_t size = h[0];

    /* Section slots -> motion (file offsets, relocated below). */
    GSU32(motion, MO_SEC0) = h[1];            /* S0 */
    GSU32(motion, 0x04)    = h[2];            /* S1 (overridden)      */
    GSU32(motion, 0x08)    = h[3];            /* S2 */
    GSU32(motion, 0x0c)    = h[4];            /* S3 */
    GSU32(vm, 0x0c) = h[5];                 /* S4 cat tab -> VM ctx */
    GSU32(motion, MO_AC)   = size + 4;        /* .ACT hdr +4          */
    GSU32(motion, MO_S5)   = h[6];            /* S5 skeleton desc     */
    uint32_t *s6 = (uint32_t *)(ani + h[7]);  /* S6: 4 u32 -> +0xe8.. */
    for (int i = 0; i < 4; i++) GSU32(motion, 0xe8 + i * 4) = s6[i];

    /* motion+4 = u32 inside S1 indexed by FOE char id (0x8017A0BC-0x8017A0DC;
     * foe char read is unconditional — foe==0 indexes by guest byte @3). */
    GSU32(motion, 0x04) = GU32(ani + h[2] + GU8(foe, FC_CHAR) * 4, 0);

    /* ---- post-header param block at blob+0x24 (u32-indexed) ---- */
    /* piVar7[9..0xe] = blob +0x24..0x38 */
    GS8 (tbl, 0x02) = (int8_t)GU8(ani, 0x24);          /* (char)piVar7[9] */
    GS8 (tbl, 0x26) = GU8(ani, 0x26);
    GS16(tbl, 0x20) = (int16_t)GU32(ani, 0x28);        /* (short)piVar7[10] */
    GSU16(tbl, 0x44) = GU16(ani, 0x2a);
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
        /* lbu v0,4(v1) @0x8017A22C is unconditional even when cfg_g==0. */
        uint32_t cfgbyte = GU8((const uint8_t *)guest_at(cfg_g), 4);
        if (cfgbyte) {
            int32_t div = (int16_t)GU16(ani, 0x2c);
            GS16(ctrl, 0xf8) = div ? (int16_t)((int32_t)(cfgbyte << 8) / div + 1) : 0;
        } else {
            GS16(ctrl, 0xf8) = 0;
        }
    }

    /* ---- S7 template -> param_tbl (0x18-byte record at ani+h[8]) -- */
    uint8_t *s7 = ani + h[8];
    GS8 (tbl, 0x03)  = (int8_t)GU8(s7, 0x00);
    GS8 (tbl, 0x04)  = GU8(s7, 0x04);
    GS8 (tbl, 0x27)  = GU8(s7, 0x02);
    GS8 (tbl, 0x28)  = GU8(s7, 0x06);
    GSU32(tbl, 0x08) = GU32(s7, 0x08);
    GSU32(tbl, 0x0c) = GU32(s7, 0x10);
    GSU32(tbl, 0x2c) = GU32(s7, 0x0c);
    GSU32(tbl, 0x30) = GU32(s7, 0x14);

    /* ---- relocate all stored pointers (file off -> absolute) ---- */
    static const uint8_t moffs[] = {0x00,0x04,0x08,0x0c,0x10,0xac,0xe8,0xec,0xf0,0xf4};
    for (unsigned i = 0; i < sizeof moffs; i++)
        GSU32(motion, moffs[i]) += base;
    /* NOTE: vm+0x0c (S4 cat_tab) is deliberately NOT relocated here.
     * Guest func_8017a05c seeds it raw (sec_off[4]) at +0x8017a09c and
     * leaves it file-relative; func_80181db8 (ani_fixup) adds the blob
     * base exactly once. Relocating it here too double-adds base ->
     * cat_tab = sec_off[4] + 2*base (e.g. 0x000eacec), which then feeds
     * garbage cat_tab[1]/[2]/[5] into the ScriptVM and wild-dispatches.
     * (N+85) */
    {
        static const uint8_t toffs[] = {0x08,0x0c,0x2c,0x30};
        for (unsigned i = 0; i < sizeof toffs; i++)
            GSU32(tbl, toffs[i]) += base;
    }

    /* ---- .ACT cursor share/mirror (generated 0x8017A388-0x8017A3E8) --
     * player==0 or char differs: v = lh[*motion+0xac]; cursor += 4;
     * motion+0xa4 = v.  Same char: mirror foe's cursor, +0xa4 = lh[cur-4].
     * The foe char compare reads foe+3 unconditionally (foe==0 -> guest
     * byte @3), so there is NO skip path: cursor-share always runs and
     * the once-relocation below always starts at act_base+8 (self) —
     * the field1 count u32 at act_base+4 must stay raw. */
    if (GU8(ctrl, FC_PLAYER) == 0 ||
        GU8(ctrl, FC_CHAR) != GU8(foe, FC_CHAR)) {
        uint32_t cur_g = GU32(motion, MO_AC);
        uint8_t *cur = (uint8_t *)guest_at(cur_g);
        uint16_t v = GU16(cur, 0);
        GSU32(motion, MO_AC) = cur_g + 4;
        GSU16(motion, 0xa4) = v;
    } else {
        /* mirror: guest lw's *(foe+0x38) then +0xac with no NULL checks
         * (0x8017A3B0-0x8017A3CC); guest_at keeps every deref valid. */
        uint32_t fm_g = GU32(foe, FC_MOTION);
        uint8_t *fm = (uint8_t *)guest_at(fm_g);
        uint32_t cur = GU32(fm, MO_AC);
        GSU32(motion, MO_AC) = cur;
        GSU16(motion, 0xa4) = GU16((uint8_t *)guest_at(cur - 4), 0);
    }

    /* ---- .ACT once-relocation (ctrl+0xb == 0), generated
     * 0x8017A3F8-0x8017A4C4 — runs AFTER the cursor-share and walks
     * from the POST-share motion+0xac (act_base+8 = sec_off[0]), so
     * the field1 count u32 at act_base+4 is never relocated.
     * motion+0xac: u32 list, -1 term, positive += base+size (.ACT base)
     * motion+0xa8: u32 list, -1 term, positive += pa8_g-8 (list-relative) */
    if (GU8(ctrl, FC_FLAGB) == 0) {
        uint32_t ac_g = GU32(motion, MO_AC);
        uint32_t pa8_g = GU32(motion, MO_PA8);
        /* lw a0,0xac(t0)/lw a1,0xa8(t0) then lw 0(a0)/lw 0(a1) are
         * unconditional (@0x8017A3F8/@0x8017A470): zero list ptrs walk
         * guest low memory exactly like any other address. */
        uint32_t *p = (uint32_t *)guest_at(ac_g);
        while (*p != 0xffffffffu) {
            if ((int32_t)*p > 0) *p += base + size;
            p++;
        }
        p = (uint32_t *)guest_at(pa8_g);
        if (*p != 0xffffffffu) {
            do {
                if ((int32_t)*p > 0) *p = (uint32_t)((int32_t)*p + (int32_t)pa8_g - 8);
                p++;
            } while (*p != 0xffffffffu);
        }
    }

    /* ---- big transient reset (generated 0x8017A4C4-0x8017A580).
     * Byte offsets verified against the generated stores — the earlier
     * int*-scaled "approx" offsets were wrong. */
    GSU32(ctrl, 0x58) = 0;  GSU32(ctrl, 0x5c) = 0;
    GSU32(ctrl, 0x60) = 0;  GSU32(ctrl, 0x64) = 0;
    GSU32(ctrl, 0x68) = 0;  GSU32(ctrl, 0x6c) = 0;
    GS16 (motion, 0x14)  = 0;
    GSU32(motion, 0x18)  = 0;
    GS16 (motion, 0x1c)  = 0xffff;
    GS16 (motion, 0x1e)  = 0;
    GSU32(motion, 0x30)  = 0;
    GS16 (motion, 0x34)  = 0xffff;
    GS16 (motion, 0x36)  = 0;
    GSU32(motion, 0x20)  = 0;
    GS16 (motion, 0x24)  = 0xffff;
    GS16 (motion, 0x26)  = 0;
    GS16 (motion, 0x2a)  = 0xffff;
    GS8  (motion, 0x7c)  = 0;
    GS8  (motion, 0x7d)  = 0;
    GS8  (motion, 0x7e)  = 0xff;
    GS16 (motion, 0x90)  = 0;
    GS16 (motion, 0x9e)  = 1;
    GS16 (motion, 0xa0)  = 1;
    GS16 (motion, 0xc0)  = 1;
    GSU32(motion, 0xb0)  = 0;
    GS16 (ctrl, 0x88)   = GS16(ctrl, 0x86);
    GS16 (motion, 0x9a)  = 0xffff;
    GS8  (motion, 0x101) = 0xff;
    GS8  (motion, 0x102) = 0xff;
    GS8  (motion, 0x103) = 0xff;
    GS8  (motion, 0x104) = 0xff;
    GS16 (motion, 0x106) = 0;
    GS16 (motion, 0x108) = 0;
    GS16 (motion, 0x10a) = 0;
    GS8  (ctrl, 0x26)   = 0xff;
    /* 0x8017A55C-0x8017A568: byte[*ctrl+0x14] = 0xff — ONE guest
     * dereference (same shape as action_switch_guest's ctrl+0x14 store). */
    GS8((uint8_t *)guest_at(GU32(ctrl, 0x14)), 0) = 0xff;
    GS8  (ctrl, 0x27)   = 0;
    GS16 (motion, 0x10e) = 0xffff;
    GS16 (motion, 0x110) = 0xffff;
    GS16 (motion, 0x112) = 0xffff;
    GS16 (motion, 0x114) = 0xffff;
    GS16 (motion, 0x116) = 0;
    /* part-table loops (generated 0x8017A584-0x8017A67C):
     * tbl = *(ctrl+0x28); four sub-tables at tbl+{0x10,0x14,0x34,0x38};
     * each is an array of 0x28-stride records whose +0xc halfword is:
     *   k = 0..0x1a -> 0 ;  then ptr[0x14]+0x1c4 = 1 and
     *   ptr[0x38]+0x23c = 1 ;  then k = 0xf..0x10 -> 1. */
    {
        static const uint8_t parr[] = {0x10, 0x14, 0x34, 0x38};
        uint8_t *arr[4];
        for (unsigned a = 0; a < 4; a++)
            arr[a] = (uint8_t *)guest_at(GU32(tbl, parr[a]));
        for (int k = 0; k < 0x1b; k++)
            for (unsigned a = 0; a < 4; a++)
                GSU16(arr[a] + k * 0x28, 0xc) = 0;
        GSU16(arr[1], 0x1c4) = 1;
        GSU16(arr[3], 0x23c) = 1;
        for (int k = 0xf; k < 0x11; k++)
            for (unsigned a = 0; a < 4; a++)
                GSU16(arr[a] + k * 0x28, 0xc) = 1;
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
/* FUN_80181db8 — .ANI S4 category fixup (faithful port).
 * Guest args: a0=ctrl, a2=cat=*(ctrl+0x3c), a3=battle(0x1f800000),
 * variant arrives on the caller's stack frame. cat+0xc is relocated
 * by ctrl[0x30]; the 5-word table it points at is relocated too, once
 * per fighter (gated by ctrl+0xb, set by the caller after return).
 * Then the descriptor fields are filled and cat+0x28 is set from the
 * variant (1 -> 9, bit6 -> variant, else untouched). Split tails:
 * 0x80181ee8 (cat+0x28=0), 0x80181ef0 (variant==2, unreachable),
 * 0x80181f10 (cat+0x28=9), 0x80181f18 (plain return).            */
/* ------------------------------------------------------------------ */
/* cat/battle arrive as guest arguments (a2/a3) — callers pass
 * cat = *(ctrl+0x3c) and battle = the battle-context pointer; the
 * ports below pass the same values action_enter derives. */
static void ani_fixup(uint8_t *ctrl, int variant, uint8_t *cat,
                      const uint8_t *battle)
{
    uint32_t base = GU32(ctrl, 0x30);
    uint8_t *tbl;
    int i;

    /* relocate the S4 category pointer itself */
    GSU32(cat, 0x0c) = GU32(cat, 0x0c) + base;

    /* once-only: relocate the 5 category words the pointer references */
    if ((int8_t)GU8(ctrl, 0x0b) == 0) {
        tbl = (uint8_t *)guest_at(GU32(cat, 0x0c));
        for (i = 0; i < 5; i++)
            GSU32(tbl, i * 4) = GU32(tbl, i * 4) + base;
    }

    tbl = (uint8_t *)guest_at(GU32(cat, 0x0c));
    GSU32(cat, 0x14) = GU32(tbl, 4);
    GSU32(cat, 0x18) = GU32(tbl, 8);
    GSU32(cat, 0x10) = 0x801b5228;
    GSU32(cat, 0x1c) = 0x801b5268;
    GSU32(cat, 0x20) = 0x801b5278;
    GSU32(cat, 0x00) = 0x801f8400;
    GS8 (cat, 0x04)  = (uint8_t)(GU8(ctrl, 0x02) << 7);
    GS16(cat, 0x06)  = 0;
    GS16(cat, 0x28)  = 0;
    GS16(cat, 0x0a)  = 0;
    GS16(cat, 0x2a)  = 0;
    GS16(cat, 0x6c)  = 0;
    GSU32(cat, 0x70) = 0;
    GSU32(cat, 0x74) = 0;
    GS16(cat, 0x08)  = (int16_t)GU16(battle, 0xa0);
    for (i = 0; i < 7; i++)
        GS16(cat, 0x3c + i * 2) = 0;

    /* variant tail (generated 0x80181EA4-0x80181EE4):
     *   ==1 or ==3 -> cat+0x28 = 9   (both split to 0x80181F10)
     *   ==2        -> cat+0x28 = 8 ; cat+0x24 = tbl[5] + base (0x80181EF0)
     *   &0x40      -> cat+0x28 = variant (0x80181EE0)
     *   else       -> untouched (0x80181F18)                          */
    if (variant == 1 || variant == 3) {
        GS16(cat, 0x28) = 9;
    } else if (variant == 2) {
        GS16(cat, 0x28) = 8;
        GSU32(cat, 0x24) = GU32(tbl, 0x14) + base;
    } else if (variant & 0x40) {
        GS16(cat, 0x28) = (int16_t)variant;
    }
}

/* ------------------------------------------------------------------ */
void part_record_reset(uint16_t *rec);
void action_switch_guest(int ctrl_g,int *motion,int idx);

/* slot_apply — slot_apply: select one of two form descriptors.      */
/* param_tbl + slot*0x24 = 0x24-byte form descriptor (verified).       */
/* ------------------------------------------------------------------ */
static void slot_apply(uint8_t *ctrl, uint32_t slot)
{
    uint8_t *tbl = ctrl_param(ctrl);
    uint8_t *motion = ctrl_motion(ctrl);
    /* func_80175E08 lws tbl/motion/old+new part+mat ptrs and memcpy's
     * unconditionally — zero fields copy through guest low memory. */
    uint8_t *d = tbl + slot * 0x24;

    GS8(ctrl, 0x06) = (int8_t)GU8(d, 0x00);
    GS8(ctrl, 0x08) = GU8(d, 0x02);
    uint32_t op_g = GU32(ctrl, 0x20);
    uint32_t np_g = GU32(d, 0x18);
    uint32_t om_g = GU32(ctrl, 0x40);
    uint8_t *old_parts = (uint8_t *)guest_at(op_g);
    uint8_t *new_parts = (uint8_t *)guest_at(np_g);
    uint8_t *old_mats  = (uint8_t *)guest_at(om_g);

    GSU32(ctrl, 0x18) = GU32(d, 0x10);
    GSU32(ctrl, 0x1c) = GU32(d, 0x14);
    GSU32(ctrl, 0x20) = GU32(d, 0x18);
    GSU32(ctrl, 0x40) = GU32(d, 0x1c);

    memcpy(new_parts + 0x20, old_parts + 0x20, 0x20);
    memcpy(new_parts + 0x160, old_parts + 0x160, 0x10);
    memcpy(new_parts + 0x170, old_parts + 0x170, 0x10);
    GSU32(motion, 0xf8) = GU32(motion, 0xe8 + slot * 8);
    GSU32(motion, 0xfc) = GU32(motion, 0xec + slot * 8);
    GS8 (ctrl, 0x09) = GU8(d, 0x03);
    GSU32(ctrl, 0x0c) = GU32(d, 0x08);
    GS8 (ctrl, 0x0a) = GU8(d, 0x04);
    GSU32(ctrl, 0x10) = GU32(d, 0x0c);
    GS16(ctrl, 0x24) = (int16_t)GU16(d, 0x20);

    {
        uint32_t nm_g = GU32(ctrl, 0x40);
        uint8_t *nm = (uint8_t *)guest_at(nm_g);
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
    /* entry_setup lws ctrl+0x38/+0x20/+0x18/+0x1c and dereferences them
     * unconditionally (@0x80175AAC/@0x80175B08/@0x80175B60/@0x80175DAC). */
    uint8_t *fobj = (uint8_t *)guest_at(fobj_g);

    /* part-record resets (loops over ctrl+8 / ctrl+6 counts) */
    int n = (int8_t)GU8(ctrl, 0x08) - 1;
    for (int i = n; i >= 0; i--) {
        part_record_reset((uint16_t *)(fobj + i * 0x50));
    }
    int pc = (int8_t)GU8(ctrl, 0x06) - 1;
    for (int i = pc; i >= 0; i--) {
        uint8_t *p18 = (uint8_t *)guest_at(GU32(ctrl, 0x18));
        uint8_t *p1c = (uint8_t *)guest_at(GU32(ctrl, 0x1c));
        GSU32(p18,0)=0; GSU32(p18,0x10)=0; GSU32(p18,0x14)=0; GSU32(p18,0x18)=0;
        GSU32(p1c,0)=0; GSU32(p1c,0x10)=0; GSU32(p1c,0x14)=0; GSU32(p1c,0x18)=0;
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
            /* guest @0x80175d24: beq chr,v0 with v0==1 (set @0x80175cfc,
             * the slot==1 path skips the v0=4 reload @0x80175d04) ->
             * chr==1 gets x=-0x300, NOT chr4/5. */
            else if (chr == 1)      x = -0x300;
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
static void part_link_state(uint16_t *src, int16_t *dst, int link_mode)
{
    int16_t st = dst[0];
    if (st == 0) {
        dst[0x22] = 0;
        dst[0x23] = 0;
        st = dst[0];
    }
    if (st != 2) dst[0] = (int16_t)link_mode;
    dst[0x24] = 0;
    dst[0x25] = 0;
    *(uint32_t *)((uint8_t *)dst + 0x40) = src ? guest_of(src) : 0;
    if (src != NULL) {
        uint32_t state = *src;
        if (link_mode == 3) {
            if (state == 1) {
                *src = 2;
                *(uint32_t *)((uint8_t *)dst + 0x48) = *(uint32_t *)((uint8_t *)src + 0x44);
            } else if (state < 2) {
                if (state != 0) {
                    dst[0x20] = 0;
                    dst[0x21] = 0;
                    goto done;
                }
                *src = 2;
            } else {
                if (state != 3) {
                    dst[0x20] = 0;
                    dst[0x21] = 0;
                    dst[0x26] = 0;
                    dst[0x27] = 0;
                    return;
                }
                *src = 4;
                *(uint32_t *)((uint8_t *)dst + 0x48) = *(uint32_t *)((uint8_t *)src + 0x44);
            }
        } else {
            if (state == 2 || state == 4) {
                uint32_t chain_g = *(uint32_t *)((uint8_t *)src + 0x44);
                uint8_t *chain = (uint8_t *)guest_at(chain_g);
                *(uint32_t *)((uint8_t *)dst + 0x48) = GU32(chain, 0x48);
                GSU32(chain, 0x48) = guest_of(dst);
                goto done;
            }
            if (state == 0) *src = 1;
            else          *(uint32_t *)((uint8_t *)dst + 0x48) = *(uint32_t *)((uint8_t *)src + 0x44);
        }
        *(uint32_t *)((uint8_t *)src + 0x44) = guest_of(dst);
    }
done:
    dst[0x26] = 0;
    dst[0x27] = 0;
}

/* ------------------------------------------------------------------ */
/* skeleton_init — skeleton_init: part/matrix init from S5 descriptor.  */
/* desc = 0x1c-byte block inside S5 (two per section, slot*0x1c).      */
/* Ports the verified loop structure; matrix decode helpers            */
/* (euler_rotmat euler->matrix, part_link_unlink part-copy) are host      */
/* hooks pending their own decode.                                     */
/* ------------------------------------------------------------------ */
static void skeleton_init_wrap(uint8_t *ctrl, uint32_t desc_g)
{
    /* func_80179AE0 is invoked unconditionally with a1 = lw 0x10(motion)
     * (+0x1c on the second call). Pass the raw guest descriptor address
     * through so desc==0 hits guest low memory exactly like the guest. */
    skeleton_init(guest_of(ctrl), desc_g, 0);
}

/* ------------------------------------------------------------------ */
/* action_switch_guest — action_switch: S0 table -> event stream.             */
/* ------------------------------------------------------------------ */
void action_switch_guest(int ctrl_g,int *motion,int idx);
void part_record_reset(uint16_t *rec);
static void action_switch(uint8_t *ctrl, uint8_t *motion, int32_t idx)
{
    /* func_8017A6E8 has no zero tests — ctrl/motion are always live guest
     * objects here (guest_at(0) for a zero field is still a pointer). */
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
        ani_fixup(ctrl, variant,
                    (uint8_t *)guest_at(GU32(ctrl, 0x3c)),
                    (const uint8_t *)guest_at(0x1f800000u));
        GS8(ctrl, FC_FLAGB) = 1;   /* guest: caller sets ctrl+0xb after fixup */
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
        /* lw a1,0x10(s2) @0x801760DC/@0x80176114 is unconditional —
         * skeleton_init runs even when motion+0x10 == 0. */
        slot_apply(ctrl, 0);
        entry_setup(ctrl, sel >> 4);
        skeleton_init_wrap(ctrl, GU32(motion, MO_S5));
        part_link(ctrl);

        GS8(ctrl, 0x05) = 0x11;
        slot_apply(ctrl, 1);
        entry_setup(ctrl, sel >> 4);
        skeleton_init_wrap(ctrl, GU32(motion, MO_S5) + 0x1c);
        part_link(ctrl);

        uint8_t b3 = (saved5 & 0x10) ? 0x11 : 0;
        GS8(ctrl, 0x05) = b3;
        slot_apply(ctrl, b3 & 1);

        int variant = GU8(ctrl, 0x01) ? ((GU8(ctrl, 0x01) & 0x7f) - 1) : 0;
        ani_fixup(ctrl, variant,
                    (uint8_t *)guest_at(GU32(ctrl, 0x3c)),
                    (const uint8_t *)guest_at(0x1f800000u));
        GS8(ctrl, 0x0b) = 1;
    }

    /* shared tail — guest @0x80176208 derefs motion+0x80/+0x82 and calls
     * action_switch/ev dispatch unconditionally (motion==0 -> low RAM). */
    GSU16(motion, MO_PREV) = GU16(motion, MO_ACT);
    action_switch(ctrl, motion, (int16_t)GU16(motion, MO_ACT));
    /* step 6: ev_dispatch — caller runs ev_step(ctrl, motion) */
    return cls;
}

/* ------------------------------------------------------------------ */
/* Guest-faithful ports (verbatim Ghidra transliteration).            */
/* ------------------------------------------------------------------ */

/* part_record_reset — part-record reset: identity 4x3 matrix + tail call. */
void part_link_unlink(uint16_t *src,short *dst,int mode);
void part_record_reset(uint16_t *rec)
{
  *(uint32_t *)(rec + 2) = 0x1000;
  *(uint32_t *)(rec + 4) = 0;
  *(uint32_t *)(rec + 6) = 0x1000;
  *(uint32_t *)(rec + 8) = 0;
  *(uint32_t *)(rec + 10) = 0x1000;
  rec[0xc] = 0;
  rec[0xd] = 0;
  rec[0xe] = 0;
  rec[0xf] = 0;
  *(uint32_t *)(rec + 0x10) = 0x1000;
  *(uint32_t *)(rec + 0x12) = 0;
  *(uint32_t *)(rec + 0x14) = 0x1000;
  *(uint32_t *)(rec + 0x16) = 0;
  *(uint32_t *)(rec + 0x18) = 0x1000;
  *(uint32_t *)(rec + 0x1a) = 0;
  *(uint32_t *)(rec + 0x1c) = 0;
  *(uint32_t *)(rec + 0x1e) = 0;
  *rec = 0;
  part_link_unlink((uint16_t *)0,(short *)rec,0);
}

/* action_switch_guest — action_switch (guest-verbatim; superset of the      */
/* static approximation above — adds part-struct resets).            */
void action_switch_guest(int ctrl_g,int *motion,int idx)
{
  uint16_t sel_idx;
  int sel_val;
  /* ctrl_g arrives as a guest address; motion is a host pointer to
   * the motion object (int* scaled access matches guest byte offsets). */
  uint8_t *p1 = (uint8_t *)guest_at((uint32_t)ctrl_g);
  sel_val = (int)GU32(p1, 0x30);
  sel_idx = *(uint16_t *)guest_at((uint32_t)*motion + (uint32_t)idx * 2);
  *(uint16_t *)(motion + 0x24) = 0;
  sel_val = (uint32_t)sel_idx + sel_val;
  motion[0x22] = sel_val;
  motion[0x21] = sel_val;
  *(uint16_t *)(p1 + 0x8c) = 0;
  *(uint16_t *)(p1 + 0x8e) = 0;
  *(uint8_t *)(p1 + 0x26) = 0xff;
  *(uint32_t *)(p1 + 0x5c) = *(uint32_t *)(p1 + 0x5c) & 0xfffc7fd7;
  *(uint32_t *)(p1 + 0x60) = *(uint32_t *)(p1 + 0x60) & 0xa9dfffee;
  *(uint32_t *)(p1 + 0x6c) = *(uint32_t *)(p1 + 0x6c) & 0xffffdfff;
  *(uint8_t *)guest_at(GU32(p1, 0x14)) = 0xff;
  *(uint8_t *)(p1 + 0x27) = 0;
  *(uint16_t *)(p1 + 0xba) = 0;
  *(uint16_t *)(p1 + 0xb8) = 0;
  *(uint16_t *)(p1 + 0xb6) = 0;
  *(uint16_t *)(motion + 0x44) = 0xffff;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x23c) = 0;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x23c) = 1;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x1c4) = 0;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x1c4) = 1;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x28c) = 1;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x28c) = 1;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x18)) + 0x264) = 1;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x1c)) + 0x264) = 1;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x18) +
      (uint32_t)(*(int8_t *)(p1 + 6) * 0x28) - 0x1c)) = 0;
  *(uint16_t *)((uint8_t *)guest_at(GU32(p1, 0x1c) +
      (uint32_t)(*(int8_t *)(p1 + 6) * 0x28) - 0x1c)) = 0;
}

/* slot_apply_guest — guest entry @0x80175e08: delegates to the faithful static port. */
void slot_apply_guest(int ctrl_g,int slot)
{
  slot_apply((uint8_t *)guest_at((uint32_t)ctrl_g),(uint32_t)slot);
}

/* part_link_unlink — part-record link/unlink: intrusive list via
 * rec+0x44 next / rec-head +0x22 tail, state machine on rec[0].
 * param_3: 3 = detach-to-slot mode, else link mode. */
void part_link_unlink(uint16_t *src,short *dst,int mode)
{
  uint16_t state;
  short st;

  st = *dst;
  if (st == 0) {
    dst[0x22] = 0;
    dst[0x23] = 0;
    st = *dst;
  }
  if (st != 2) {
    *dst = (short)mode;
  }
  dst[0x24] = 0;
  dst[0x25] = 0;
  /* rec+0x40 = source-record back-pointer: a 4-byte GUEST address cell.
   * src is a host pointer — store guest_of(src), never the
   * host pointer itself (an 8-byte store here also clobbered the
   * rec+0x44 link field and corrupted the part chain). */
  *(uint32_t *)((uint8_t *)dst + 0x40) = src ? guest_of(src) : 0;
  if (src != (uint16_t *)0x0) {
    if (mode == 3) {
      state = *src;
      if (state == 1) {
        *src = 2;
        *(uint32_t *)(dst + 0x24) = *(uint32_t *)(src + 0x22);
      }
      else if (state < 2) {
        if (state != 0) {
          dst[0x20] = 0;
          dst[0x21] = 0;
          goto LAB_80179a60;
        }
        *src = 2;
      }
      else {
        if (state != 3) {
          dst[0x20] = 0;
          dst[0x21] = 0;
          dst[0x26] = 0;
          dst[0x27] = 0;
          return;
        }
        *src = 4;
        *(uint32_t *)(dst + 0x24) = *(uint32_t *)(src + 0x22);
      }
    }
    else {
      state = *src;
      if ((state == 2) || (state == 4)) {
        *(uint32_t *)(dst + 0x24) = *(uint32_t *)
            ((uint8_t *)guest_at((uint32_t)*(int *)(src + 0x22)) + 0x48);
        *(uint32_t *)((uint8_t *)guest_at((uint32_t)*(int *)(src + 0x22)) + 0x48)
            = guest_of(dst);
        goto LAB_80179a60;
      }
      if (state == 0) {
        *src = 1;
      }
      else {
        *(uint32_t *)(dst + 0x24) = *(uint32_t *)(src + 0x22);
      }
    }
    *(uint32_t *)(src + 0x22) = guest_of(dst);
  }
LAB_80179a60:
  dst[0x26] = 0;
  dst[0x27] = 0;
  return;
}

/* ------------------------------------------------------------------ */
/* Guest entry points (fnmap-dispatched). Each exposes the faithful    */
/* static port above under the guest ABI the recompiler callers use.   */
/* ------------------------------------------------------------------ */

/* apply_guest — guest func_8017A05C (ctrl, motion, sel).
 * The guest body ends in a class tail on sel&0xf: motion fields are
 * cleared for every class except 1 and >=5 — exactly apply_tail's
 * condition (verified against the 0x8017A680..0x8017A6B4 dispatch). */
void apply_guest(uint8_t *ctrl, uint8_t *motion, uint32_t sel)
{
    apply(ctrl, motion);
    apply_tail(motion, sel & 0xf);
}

/* reset_guest — guest func_80175898 (ctrl, sel). */
void reset_guest(uint8_t *ctrl, uint32_t sel)
{
    reset(ctrl, sel);
}

/* entry_setup_guest — guest func_80175A7C (ctrl, group). */
void entry_setup_guest(uint8_t *ctrl, uint32_t group)
{
    entry_setup(ctrl, group);
}

/* ani_fixup_guest — guest func_80181DB8 (ctrl, a1, cat, battle,
 * variant@stack). The guest reads cat from a2, battle from a3 and the
 * variant from caller stack arg5 (sp+16); a1 is unused. */
void ani_fixup_guest(uint8_t *ctrl, uint32_t a1, uint8_t *cat,
                     const uint8_t *battle, int32_t variant)
{
    (void)a1;   /* guest a1 is dead in func_80181DB8 */
    ani_fixup(ctrl, variant, cat, battle);
}

/* pending_gate_guest — guest func_80178664 (motion, ctrl, reqA, reqB).
 * Same gate body as fight.c's pending_gate; this is the dispatch entry
 * (bind.c's pending_action_gate is excluded from fnmap scanning, so the
 * guest address had no clean dispatch until now). motion arrives as a
 * guest address and is translated here. */
extern int32_t pending_gate(uint8_t *motion, uint8_t *ctrl,
                            uint32_t req_a, uint32_t req_b);
int32_t pending_gate_guest(uint32_t motion, uint8_t *ctrl,
                           uint32_t req_a, uint32_t req_b)
{
    return pending_gate((uint8_t *)guest_at(motion), ctrl,
                        req_a, req_b);
}

/* part_flag100_set — guest func_801796CC (24B leaf before the shared
 * `jr ra` at 0x801796E4): if ($t0 > 0) OR bit 0x100 into the u32 at
 * a2+0x5c and return it; otherwise return $v0 unchanged (guest leaves
 * v0 stale on the early-out path, so read it back from the live CPU).
 * Disasm 0x801796cc-0x801796e0. */
extern CPUState *clean_cpu_state(void);
int32_t part_flag100_set(uint32_t arg0, uint32_t arg1, uint32_t rec, int32_t limit)
{
    uint32_t flags;
    (void)arg0; (void)arg1;
    if (limit <= 0) return (int32_t)clean_cpu_state()->gpr[2];
    flags = *(uint32_t *)guest_at(rec + 0x5c);
    flags |= 0x100u;
    *(uint32_t *)guest_at(rec + 0x5c) = flags;
    return (int32_t)flags;
}

/* part_desc_clear — guest func_8017A6B4 (44B leaf before the shared
 * `jr ra` at 0x8017A6E0): zero two flag bytes and nine flag halfwords
 * in the descriptor block whose base arrives in $t0.
 * Disasm 0x8017a6b4-0x8017a6dc. */
void part_desc_clear(uint32_t desc)
{
    uint8_t *d = (uint8_t *)guest_at(desc);
    d[0x100] = 0;
    d[0x09c] = 0;
    *(uint16_t *)(d + 0x0d2) = 0;
    *(uint16_t *)(d + 0x0d0) = 0;
    *(uint16_t *)(d + 0x0e4) = 0;
    *(uint16_t *)(d + 0x0d4) = 0;
    *(uint16_t *)(d + 0x0d6) = 0;
    *(uint16_t *)(d + 0x0d8) = 0;
    *(uint16_t *)(d + 0x0dc) = 0;
    *(uint16_t *)(d + 0x0de) = 0;
    *(uint16_t *)(d + 0x0e0) = 0;
}

/* part_desc_clear_gate — guest func_8017A6A8 (12B): `slti v0,a2,5;
 * beq v0,zero -> jr ra`. When a2 < 5 the guest falls through into
 * func_8017A6B4's clear body; that fall-through is inlined here (the
 * guest performs no call, and call_guest would clobber the $t0 base
 * the clear body reads). $v0 on exit is the slti result: 1 on the
 * clear path, 0 on the gate path.
 * Disasm 0x8017a6a8-0x8017a6e0. */
int32_t part_desc_clear_gate(uint32_t arg0, uint32_t arg1, int32_t count,
                             uint32_t arg3)
{
    (void)arg0; (void)arg1; (void)arg3;
    if (count >= 5) return 0;
    part_desc_clear(clean_cpu_state()->gpr[8]);
    return 1;
}
