/* action_table.h — Bloody Roar 2 .ACT container, verified layout.
 *
 * Per-frame action/animation parameter database (~190KB per char).
 * Loaded VERBATIM after the .ANI blob; ALL nonzero header section
 * offsets are relocated to absolute (blob_base + off) at load.
 *
 * Header: {u32 size, u32 field1, u32 sec_off[16]} — 0x48 bytes.
 * YUG: field1=0x1df; only sec slots 0,1,2,14,15 non-empty
 * (ALI: 3 sections — usage varies per character).
 *
 * ALL sections share the indexed layout (verified BUS/JEN/STU/ALI):
 *   {u32 count, (u16 rec_off, u16 tag)[count], records_blob}
 * rec_off is relative to records_blob (right after the index).
 * Multiple index entries share rec_off with different tag =
 * variant selector (same model as .ANI S2).
 * Dominant record stride = 0x7a across nearly all sections/chars;
 * large sections (typically sec0) use bigger strides (0x82/0x86
 * observed) or variable records — authoritative stride comes from
 * the runtime consumer, not the file. First rec_off (0x10-0x34)
 * = per-section sub-header size inside the records blob.
 *
 * Records contain IEEE754 float keyframe curves — animation
 * parameters, NOT script bytecode, NOT s16 hitbox triples.
 *
 * Runtime links (live-verified, P1 fight):
 *   motion+0xac = &sec_off[0] (blob+8)
 *   motion+0xa4 = field1 (0x1df)
 *   motion+0xa8 = secondary ptr-pair index table
 * Consumers: 23 lw +0xac sites; main evaluator cluster
 *   0x80173c40-0x80175650; EV5/EV13 handlers 0x8017af34/0x8017b014.
 */
#ifndef BR2_ACT_H
#define BR2_ACT_H

#include <stdint.h>

typedef struct {
    uint32_t size;          /* +0x00 blob size */
    uint32_t field1;        /* +0x04 count/descriptor (0x1df for YUG) */
    uint32_t sec_off[16];   /* +0x08 section offsets (file-relative,
                               relocated to absolute at runtime) */
} ActHeader;                /* 0x48 */

enum {
    ACT_SEC0 = 0,            /* largest section, bigger records */
    ACT_SEC1 = 1,
    ACT_SEC2 = 2,
    /* sections 3-15: same indexed layout, usage varies per char */
};

/* Indexed-section index entry. rec_off relative to records blob. */
typedef struct {
    uint16_t rec_off;
    uint16_t tag;           /* variant selector */
} ActIndexEnt;              /* 0x04 */

/* Section header as laid out in file:
 *   u32 count; ActIndexEnt idx[count]; u8 records[];
 * record stride is per-section (0x86 / 0x7a), records begin at
 * section + 4 + count*4. */

#endif /* BR2_ACT_H */
