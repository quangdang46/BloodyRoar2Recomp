/*
 * ani.c — clean-room .ANI container accessors.
 *
 * Implements the verified layout from ani.h. Works on the raw
 * file image AND the runtime blob (they are byte-identical except
 * the five relocated S4 category pointers, which this module treats
 * as opaque u32s — callers resolve them against the blob base when
 * needed).
 *
 * Verified facts encoded here:
 *   - header {u32 size, u32 sec_off[8]}, offsets file-relative
 *   - S0: u16[action] -> event-stream file offset
 *   - S2: 16B index recs until 16B zero terminator; data_ptr ->
 *     shared stream of 10B keyframe records
 *   - S3: u32 table of file-absolute offsets -> u16 param records
 *   - S4: 10-u32 category table at section start; entries 0-4 may
 *     be absolute (relocated) or file-relative depending on whether
 *     the fixup ran — callers must handle both.
 *
 * Project-original clean-room code (MIT).
 */
#include <stdint.h>
#include <string.h>
#include "ani.h"

static uint16_t rd16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static uint32_t rd32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }

const AniHeader *br2_ani_header(const uint8_t *blob)
{
    return (const AniHeader *)blob;
}

const uint8_t *br2_ani_sec(const uint8_t *blob, int sec)
{
    const AniHeader *h = br2_ani_header(blob);
    if (sec < 0 || sec > 7 || h->sec_off[sec] == 0) return NULL;
    return blob + h->sec_off[sec];
}

/* S0: action index -> event-stream file offset (u16 entries). */
uint16_t br2_ani_stream_off(const uint8_t *blob, uint32_t action)
{
    const uint8_t *s0 = br2_ani_sec(blob, ANI_SEC_ACTION_MAP);
    if (!s0) return 0;
    return rd16(s0 + action * 2);
}

const uint8_t *br2_ani_stream(const uint8_t *blob, uint32_t action)
{
    uint16_t off = br2_ani_stream_off(blob, action);
    return off ? blob + off : NULL;
}

/* S2: walk channel index records until the 16B zero terminator.
 * cb(param_a, param_b, chan_id, tag, duration, key_ptr) per record;
 * key_ptr points into the shared 10B-record stream (file-relative
 * resolved against blob). Returns number of records visited. */
typedef void (*br2_chan_cb)(void *ud,
    int16_t pa, int16_t pb, uint16_t chan, uint16_t tag,
    uint16_t dur, const AniKey *keys);

uint32_t br2_ani_channels(const uint8_t *blob, br2_chan_cb cb, void *ud)
{
    const uint8_t *s2 = br2_ani_sec(blob, ANI_SEC_CHANNELS);
    if (!s2) return 0;
    uint32_t n = 0;
    for (;;) {
        const AniChanIndex *ix = (const AniChanIndex *)(s2 + n * sizeof(*ix));
        /* Terminator: all u16 fields zero (the 14B zero pad before
         * the keyframe stream reads as one such record; data_ptr is
         * ignored — it overlaps stream bytes). Verified: no real
         * index record has all-zero u16 fields. */
        int is_end = (ix->param_a == 0 && ix->param_b == 0 &&
                      ix->chan_id == 0 && ix->tag == 0 &&
                      ix->duration == 0);
        if (is_end) break;
        const AniKey *keys = (const AniKey *)(blob + ix->data_ptr);
        cb(ud, ix->param_a, ix->param_b, ix->chan_id, ix->tag,
           ix->duration, keys);
        n++;
        if (n > 0x400) break;   /* corrupt-guard */
    }
    return n;
}

/* S3: group table. Returns record ptr (file-relative resolved) for
 * group index, or NULL. Records are variable-length u16 fields;
 * caller parses. */
const uint8_t *br2_ani_group(const uint8_t *blob, uint32_t group)
{
    const uint8_t *s3 = br2_ani_sec(blob, ANI_SEC_GROUPS);
    if (!s3) return NULL;
    uint32_t off = rd32(s3 + group * 4);
    /* S3 offsets are file-absolute (already include section base). */
    if (off == 0 || off >= rd32(blob)) return NULL;
    return blob + off;
}

/* S4: category table — 10 u32 entries at section start.
 * Entries 0-4 are absolute after the loader fixup; entries 5-9 are
 * file-relative. runtime_base = guest address the blob is loaded
 * at (0 when parsing a file image). Resolves both forms. */
const uint8_t *br2_ani_cat(const uint8_t *blob, int idx, uint32_t runtime_base)
{
    const uint8_t *s4 = br2_ani_sec(blob, ANI_SEC_SCRIPT);
    if (!s4 || idx < 0 || idx > 9) return NULL;
    uint32_t v = rd32(s4 + idx * 4);
    if (v == 0) return NULL;
    uint32_t size = rd32(blob);
    if (v < size) return blob + v;                    /* file offset */
    if (runtime_base && v >= runtime_base &&
        v - runtime_base < size)                      /* relocated abs */
        return blob + (v - runtime_base);
    return NULL;
}
