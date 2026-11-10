/* ani.h — Bloody Roar 2 .ANI container, verified layout.
 *
 * File image is loaded VERBATIM into RAM; only the five leading
 * u32s of the S4 category table are relocated to absolute pointers
 * (blob_base + off). All other section offsets stay file-relative
 * and consumers add the blob base themselves.
 *
 * Header: {u32 size, u32 sec_off[8]} — 0x24 bytes.
 * Section offsets are file-relative.
 *
 * Verified on YUG1P.ANI (size 0xbb58) and cross-checked on all
 * 12 characters: section layout is uniform, only data differs.
 */
#ifndef BR2_ANI_H
#define BR2_ANI_H

#include <stdint.h>

typedef struct {
    uint32_t size;          /* +0x00 total blob size */
    uint32_t sec_off[8];    /* +0x04 section offsets (file-relative) */
} AniHeader;                /* 0x24 */

enum {
    ANI_SEC_ACTION_MAP = 0, /* S0: u16[action_idx] -> event-stream off */
    ANI_SEC_TABLE1    = 1,  /* S1: u32 -> 0x20-byte records */
    ANI_SEC_CHANNELS  = 2,  /* S2: channel/keyframe index + data */
    ANI_SEC_GROUPS    = 3,  /* S3: u32 offset table -> group params */
    ANI_SEC_SCRIPT    = 4,  /* S4: 10-u32 category table + VM bytecode */
    ANI_SEC_AUX5      = 5,
    ANI_SEC_AUX6      = 6,
    ANI_SEC_AUX7      = 7,
};

/* ---- S0: action -> event stream ----------------------------------
 * u16 table, ~647-700 entries. stream_file_off = table[action].
 * Runtime: motion+0x00 = relocated S0 base; stream = ani_base + off.
 * Verified live: table bytes identical file<->RAM, cursor at
 * motion+0x84 advances through EV records (ev.c). */

/* ---- S2: animation/hit channels -----------------------------------
 * Index: 16-byte records until a 16-byte zero terminator:
 *   {s16 param_a, s16 param_b, u16 chan_id, u16 tag,
 *    u16 duration, u16 pad, u32 data_ptr}
 * tag=0x0d -> transform keyframe windows (chan 0x80-0x8f)
 * tag=0x04/0x08 -> secondary streams (chan 0xd0-0xdf)
 * data_ptr -> shared stream of 10-byte keyframe records:
 *   {u16 type(=1), u16 channel, u16 sub, s16 v1, s16 v2}
 * Index records may share data_ptr with different param_a/b
 * (variant selector, mirrors .ACT tag model).
 * chan_id 0x80-0x87 sequential + 0x8f; 0xd0-0xdf for tag 4/8.
 */
typedef struct {
    int16_t  param_a;    /* variant param (s16 angle/pos) */
    int16_t  param_b;
    uint16_t chan_id;
    uint16_t tag;        /* 0x0d transform, 0x04/0x08 secondary */
    uint16_t duration;   /* semantic: frames/keys, NOT byte size */
    uint16_t pad;
    uint32_t data_ptr;   /* file offset into keyframe stream */
} AniChanIndex;          /* 0x10 */

typedef struct {
    uint16_t type;       /* observed =1 */
    uint16_t channel;
    uint16_t sub;
    int16_t  v1;         /* fixed-point value */
    int16_t  v2;
} AniKey;                /* 0x0a */

/* ---- S3: action-group parameter table ------------------------------
 * u32 table of FILE-ABSOLUTE offsets (consumer adds ani_base),
 * 33 entries for YUG; each -> variable-length u16 param record
 * (0x1a-0x34 bytes) containing sentinels 0xffff/0xe800/0x8000,
 * small ints, and embedded group IDs ~0x93-0xac.
 * Field-level semantics still open. */

/* ---- S4: category table + ScriptVM bytecode ------------------------
 * First 10 u32 entries = category pointer table (file-relative).
 * Entries 0-4 are relocated to absolute at load (fixup fn
 * 0x80181db8, once-flag ctrl+0x0b); entries 5-9 stay relative.
 * Category record families (all chars consistent):
 *   idx 0,1,3,4 -> type-A "80 01 00 02 00 03 ff ff" records
 *                  (u16 tables -> structured records w/ EV bytes)
 *   idx 2       -> "00 00" sibling records
 *   idx 5       -> type-C "12 XX" parameter records
 *   idx 6-9     -> type-B u16 channel/stream data
 * VM init (mode 2) seeds ctx+0x24 pc = catTab[5] + blob.
 * ctx+0x0c = ani + sec_off[4] (category table base). */

#endif /* BR2_ANI_H */
