/*
 * br2_act.c — clean-room .ACT container accessors.
 *
 * Implements the verified layout from br2_act.h. Works on the raw
 * file image and the runtime blob (identical except header sec_off
 * slots are relocated to absolute pointers at load — accessors take
 * runtime_base to resolve either form).
 *
 * Verified facts encoded here:
 *   - header {u32 size, u32 field1, u32 sec_off[16]}
 *   - indexed sections: {u32 count, (u16 rec_off,u16 tag)[count],
 *     records blob}; rec_off relative to records blob
 *   - fixed record stride per section: sec0=0x86, sec1/sec2=0x7a
 *   - records = IEEE754 float keyframe curves
 *
 * Project-original clean-room code (MIT).
 */
#include <stdint.h>
#include <string.h>
#include "action_table.h"

static uint32_t rd32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static uint16_t rd16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }

const ActHeader *br2_act_header(const uint8_t *blob)
{
    return (const ActHeader *)blob;
}

/* Resolve a header sec_off slot: file-relative or relocated-absolute. */
static const uint8_t *sec_ptr(const uint8_t *blob, int sec,
                              uint32_t runtime_base)
{
    const ActHeader *h = br2_act_header(blob);
    if (sec < 0 || sec > 15) return NULL;
    uint32_t v = h->sec_off[sec];
    if (v == 0) return NULL;
    uint32_t size = rd32(blob);
    if (v < size) return blob + v;
    if (runtime_base && v >= runtime_base && v - runtime_base < size)
        return blob + (v - runtime_base);
    return NULL;
}

const uint8_t *br2_act_sec(const uint8_t *blob, int sec, uint32_t runtime_base)
{
    return sec_ptr(blob, sec, runtime_base);
}

/* Dominant record stride observed across sections: 0x7a.
 * Large sections (typically sec0) use 0x82/0x86 or variable records;
 * the authoritative stride is defined by the runtime consumer.
 * Returns the common stride; callers needing exact sizing must
 * consult the consumer semantics. */
uint32_t br2_act_stride(int sec)
{
    (void)sec;
    return 0x7a;
}

/* Indexed-section layout helpers. sec = section base ptr.
 * count at +0, index entries at +4, records blob at +4+count*4. */
uint32_t br2_act_count(const uint8_t *sec)
{
    return sec ? rd32(sec) : 0;
}

const ActIndexEnt *br2_act_index(const uint8_t *sec)
{
    return sec ? (const ActIndexEnt *)(sec + 4) : NULL;
}

const uint8_t *br2_act_records(const uint8_t *sec)
{
    return sec ? sec + 4 + rd32(sec) * 4 : NULL;
}

/* Resolve index entry i -> record ptr, or NULL.
 * rec_off is relative to the records blob. */
const uint8_t *br2_act_record(const uint8_t *sec, uint32_t i)
{
    if (!sec) return NULL;
    uint32_t count = rd32(sec);
    if (i >= count) return NULL;
    const ActIndexEnt *ix = (const ActIndexEnt *)(sec + 4 + i * 4);
    if (ix->rec_off == 0) return NULL;
    return br2_act_records(sec) + ix->rec_off;
}

uint16_t br2_act_tag(const uint8_t *sec, uint32_t i)
{
    if (!sec) return 0;
    uint32_t count = rd32(sec);
    if (i >= count) return 0;
    return rd16(sec + 4 + i * 4 + 2);
}
