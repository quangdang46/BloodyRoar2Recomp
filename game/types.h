/*
 * types.h — canonical Bloody Roar 2 runtime layout.
 *
 * Consolidated from static MIPS disassembly + live RAM correlation
 * (see RE_NOTES.md). Offsets are byte offsets into guest structs.
 * Only CONFIRMED fields are named; the rest stay as padding.
 * Guest is little-endian; s16 fields are signed.
 *
 * Blocks:
 *   FighterCtrl  "ctrl"   P1 0x1F8000E8 / P2 0x1F8001F0
 *   MotionState  "motion" P1 0x801F5328   / P2 0x801F5440
 *   ScriptVM     "vmctx"  per-fighter AI/script context
 *   CameraObj    *(u32)0x1F8000BC
 */
#ifndef BR2_TYPES_H
#define BR2_TYPES_H
#include <stdint.h>

/* ============================ FighterCtrl ========================= */
/* Confirmed fields only; unnamed regions are padding.                */
typedef struct FighterCtrl {
    uint8_t  pad_00[0x14];
    uint32_t hit_slots;        /* 0x14 -> 8x {s8 flag,s8 cnt,u16 param} */
    uint32_t sub_18;           /* 0x18 -> sub-struct (guard field swap) */
    uint32_t pad_1c;
    uint32_t fighter_obj;      /* 0x20 -> render/physics object         */
    uint32_t pad_24;
    uint32_t param_tbl;        /* 0x28 -> action param records (36B)   */
    uint32_t foe;              /* 0x2c -> other fighter ctrl           */
    uint32_t ani_base;         /* 0x30 -> loaded .ANI file image       */
    uint32_t pad_34;
    uint32_t motion;           /* 0x38 -> MotionState                  */
    uint32_t script_ctx;       /* 0x3c -> ScriptVM                     */
    uint32_t aux_40;           /* 0x40 -> sub-struct (vel mirror)      */
    uint32_t aux_44;           /* 0x44 -> fighter-obj sub-struct       */
    uint32_t aux_48;           /* 0x48 -> fighter-obj sub-struct       */
    /* init func 0x801757b8: ctrl+2=player slot, +3=variant byte */

    int16_t  pos_x;            /* 0x4e logical position X (s12 domain) */
    uint8_t  pad_50[0x08];

    /* state words — FLAG35 ISA targets, order: 58,60,5c,64,68 */
    uint32_t st_58;            /* 0x58 AF attack flags                 */
    uint32_t st_5c;            /* 0x5c PF physics/proc flags           */
    uint32_t st_60;            /* 0x60 KF?                             */
    uint32_t st_64;            /* 0x64 (Chk KF mask group)             */
    uint32_t st_68;            /* 0x68 DF?                             */
    uint32_t st_6c;            /* 0x6c EF combo/cancel flags           */
    uint32_t st_70;            /* 0x70 aux (NEST5 reads)               */

    int16_t  dist_x;           /* 0x74 signed distance/X (e_Back)      */
    int16_t  dist_y;           /* 0x76                                   */
    uint8_t  pad_78[0x0e];

    uint16_t power;            /* 0x86 attack power (record-set)       */
    uint16_t dmg_src;          /* 0x88 effective damage source         */
    uint16_t vel_a0, vel_a1;   /* 0x8a/0x8c vel pair A                  */
    uint16_t vel_c0;           /* 0x8e? (NEST3) — see notes            */
    uint16_t vel_b0, vel_b1;   /* 0x90/0x92 vel pair C                  */
    uint16_t aux_94;           /* 0x94 (Move ext9/10 operand)           */
    uint16_t vel_x;            /* 0x96 velocity X                       */
    int16_t  vel_y;            /* 0x98 velocity Y, clamp[-0x500,0x1000]*/
    uint8_t  pad_9a[0x12];

    int16_t  hp;               /* 0xac current HP                       */
    int16_t  hp_disp;          /* 0xae displayed HP (red-bar lag)       */
    int16_t  max_hp;           /* 0xb0 max HP (150 observed)            */
    uint8_t  pad_b2[0x06];
    int16_t  atk_win1;         /* 0xb8 attack-window counter            */
    int16_t  atk_win2;         /* 0xba                                   */
    int16_t  atk_win3;         /* 0xbc                                   */
    int16_t  guard_st;         /* 0xbe (<2 => vulnerable, DMG2)         */
    int16_t  timer_c0;         /* 0xc0 (EV20 x16)                        */
    int16_t  aux_c2;           /* 0xc2 (DMG5 reads atk side)             */
    int16_t  pos_c4;           /* 0xc4 position-related                  */
    int16_t  aux_c6, aux_c8, last_hit, aux_cc; /* 0xc6..0xcd        */
    uint8_t  pad_ce[0x02];

    int16_t  timer_d0;         /* 0xd0 x16 timer (EV20)                 */
    int16_t  timer_d2;         /* 0xd2 x16 timer/offset                 */
    int16_t  timer_d4;         /* 0xd4 x16 timer                        */
    int16_t  gauge;            /* 0xd6 beast/super gauge                */
    int16_t  aux_d8;           /* 0xd8                                   */
    uint8_t  dmg_a;            /* 0xda (EV14 chip byte)                 */
    uint8_t  dmg_b;            /* 0xdb (EV15)                           */
    uint8_t  dmg_c;            /* 0xdc (EV14)                           */
    uint8_t  pad_dd;
    uint8_t  pad_de[0x1e];
    int16_t  alt_power;        /* 0xfc power override (FLAG35 f0)       */
} FighterCtrl;

/* ctrl+5 bit0 = "beast/variant active" — gates FLAG35 f0 power copy
 * and EV5/EV7/EV15/EV21 target-select branches. Byte offset 0x05.   */

/* ============================ MotionState ========================= */
/* NOTE: fields interleave across 0x80-0x110; declaration order below
 * follows guest offset order. MO_* macros are the authority.       */
typedef struct MotionState {
    uint8_t  pad_00[0x80];

    uint16_t action_cur;      /* 0x80 current action/move index        */
    uint16_t action_prev;     /* 0x82 previous action index            */
    uint32_t ev_stream;       /* 0x84 current event-stream ptr (.ACT)  */
    uint32_t ev_stream_alt;   /* 0x88 alternate stream ptr (EV1)       */
    uint32_t aux_8c;          /* 0x8c (undocumented)                   */
    uint16_t ev_countdown;    /* 0x90 frame countdown (step +0xce)     */
    /* 0x92..0xcd dense/undocumented — incl. MO_FLAGTEST @0x94,      */
    /* MO_HIT_FLAG_9C @0x9c, EV13 slot table @0xf8                  */
    uint8_t  pad_92[0x3c];
    int16_t  step_ce;         /* 0xce countdown step (0x10)            */
    int16_t  param_d0;        /* 0xd0 motion param (EV8 m0-m2)         */
    int16_t  aux_d2;          /* 0xd2 (EV17 reads motion+0xd2)         */
    int16_t  pos_d4;          /* 0xd4 pos X (12-bit)                   */
    int16_t  pos_d6;          /* 0xd6 pos Y                            */
    int16_t  pos_d8;          /* 0xd8 pos Z                            */
    uint8_t  pad_da[0x0a];    /* 0xda..0xe3 dense                      */
    int16_t  angle_e4;        /* 0xe4 facing/angle (12-bit)            */
    /* 0xe6..0x10d dense: hit-slot table @0xf8, slot idx @0x100,     */
    /* EV16 clears @0x101-0x104, approach @0x10a/0x10c — use macros */
    uint8_t  pad_e6[0x28];
} MotionState;
/* WARNING: MotionState has interleaved fields across 0x90-0x110;
 * prefer BR2_MOTION_* offset macros for unverified regions.        */

/* Verified motion offsets as macros (authoritative):              */
#define MO_ACTION_CUR   0x80   /* u16 current action index            */
#define MO_ACTION_PREV  0x82   /* u16 previous action index           */
#define MO_EV_STREAM    0x84   /* u32 event-stream ptr                */
#define MO_EV_STREAM_ALT 0x88  /* u32 alternate stream ptr            */
#define MO_EV_COUNTDOWN 0x90   /* u16 frame countdown                 */
#define MO_HIT_FLAG_9C  0x9c   /* u8  guard-sync flag                 */
#define MO_HIT_FLAG_100 0x100  /* u8  hit-slot/state                  */
#define MO_PARAM_D0     0xd0   /* s16 motion param (12-bit domain)    */
#define MO_POS_X        0xd4   /* s16 position X                      */
#define MO_POS_Y        0xd6   /* s16 position Y                      */
#define MO_POS_Z        0xd8   /* s16 position Z                      */
#define MO_ANGLE        0xe4   /* s16 facing/angle                    */
#define MO_STEP         0xce   /* u16 countdown step                  */
#define MO_FLAGTEST     0x94   /* s32 FLAG35 test-result read-back    */
#define MO_APPROACH_A   0x10a  /* s16 EV17 approach step              */
#define MO_APPROACH_B   0x10c  /* s16 EV17                            */

/* ============================ ScriptVM ============================ */
/* ctrl+0x3c -> this ctx. Initialized by func_0x80181db8 at fight
 * init: relocates the .ANI sec[4] category table (first 5 entries
 * += ani_base, hardcoded loop), installs shared LUTs, sets pc/mode.
 * ctrl+0xb = once-flag (skips re-patch); cleared by template restore.
 * LIVE VERIFIED @0x801f55c0 (P1): +0c=ani+s4, +10/1c/20=LUTs,
 * +24=pc into blob, +4c=blob ptr (ret-stack[0] plausible).        */
typedef struct ScriptVM {
    uint32_t obj_registry;    /* +0x00 = 0x801e8400 global base     */
    uint32_t char_id_shl7;    /* +0x04 = ctrl[2] << 7               */
    uint16_t param_a0;        /* +0x08 = *(initArg+0xa0)            */
    uint16_t flags;           /* +0x0a SFlg script flags            */
    uint32_t cat_tab;         /* +0x0c -> .ANI sec[4] 10-entry cat  */
    uint32_t lut_a;           /* +0x10 = 0x801b5228 u16-pair LUT    */
    uint32_t cat_1;           /* +0x14 = cat_tab[1] (absolute)      */
    uint32_t cat_2;           /* +0x18 = cat_tab[2] (absolute)      */
    uint32_t lut_b;           /* +0x1c = 0x801b5268 byte LUT        */
    uint32_t lut_c;           /* +0x20 = 0x801b5278 byte LUT        */
    /* +0x24 dual-use: init mode==2 seeds it with cat_tab[5]+blob as
     * the initial script entry PC; thereafter it is the VM pc.      */
    uint32_t pc;              /* +0x24 script PC -> .ANI blob       */
    uint32_t mode;            /* +0x28 = 0/8/9 (init t0 dispatch)   */
    uint8_t  pad_2c[0x0c];
    int32_t  cmp;             /* +0x38 last Cmp result               */
    int16_t  r[8];            /* +0x3c registers (init zeroes 7)    */
    uint32_t ret_stack[8];    /* +0x4c Bsr return-PC stack           */
    int16_t  ret_depth;       /* +0x6c stack depth                   */
    int16_t  dbg_flag;        /* +0x6e trace flag                    */
    uint8_t  pad_70[0x08];
    uint32_t trace_cur;       /* +0x78 trace-string cursor           */
} ScriptVM;

/* ============================ CameraObj =========================== */
typedef struct CameraObj {
    uint8_t  pad_00[0x30];
    int16_t  pos_x, pos_y, pos_z;  /* 0x30/0x32/0x34 (pos_z->+0x8a)  */
    uint8_t  pad_36[0x0c];
    int16_t  vec_a[3];             /* 0x42/0x44/0x46                  */
    uint8_t  pad_48[0x0a];
    int16_t  vec_b[3];             /* 0x52/0x54/0x56                  */
    uint8_t  pad_58[0x02];
    int16_t  f_5a;                 /* 0x5a                             */
    int16_t  vec_c[3];             /* 0x64/0x66/0x68                  */
    uint8_t  pad_6a[0x02];
    int16_t  f_6c;                 /* 0x6c                             */
    int16_t  f_6e, f_70, f_72, f_74, f_76; /* 0x6e..0x76 (0x76 = op18 s0) */
    int16_t  preset_idx;           /* 0x7e (op18 s9)                   */
    uint8_t  pad_80[0x80];         /* 0x80.. per-slot byte table       */
} CameraObj;

#endif /* BR2_TYPES_H */
