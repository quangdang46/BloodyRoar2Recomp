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
#include "cpu_state.h"

static uint16_t rd16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static uint32_t rd32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }

/* ani_header — animation blob header sits at offset 0. */
const AniHeader *ani_header(const uint8_t *blob)
{
    return (const AniHeader *)blob;
}

const uint8_t *ani_sec(const uint8_t *blob, int sec)
{
    const AniHeader *h = ani_header(blob);
    if (sec < 0 || sec > 7 || h->sec_off[sec] == 0) return NULL;
    return blob + h->sec_off[sec];
}

/* S0: action index -> event-stream file offset (u16 entries). */
uint16_t ani_stream_off(const uint8_t *blob, uint32_t action)
{
    const uint8_t *s0 = ani_sec(blob, ANI_SEC_ACTION_MAP);
    if (!s0) return 0;
    return rd16(s0 + action * 2);
}

const uint8_t *ani_stream(const uint8_t *blob, uint32_t action)
{
    uint16_t off = ani_stream_off(blob, action);
    return off ? blob + off : NULL;
}

/* S2: walk channel index records until the 16B zero terminator.
 * cb(param_a, param_b, chan_id, tag, duration, key_ptr) per record;
 * key_ptr points into the shared 10B-record stream (file-relative
 * resolved against blob). Returns number of records visited. */
typedef void (*chan_cb)(void *ud,
    int16_t pa, int16_t pb, uint16_t chan, uint16_t tag,
    uint16_t dur, const AniKey *keys);

uint32_t ani_channels(const uint8_t *blob, chan_cb cb, void *ud)
{
    const uint8_t *s2 = ani_sec(blob, ANI_SEC_CHANNELS);
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
const uint8_t *ani_group(const uint8_t *blob, uint32_t group)
{
    const uint8_t *s3 = ani_sec(blob, ANI_SEC_GROUPS);
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
const uint8_t *ani_cat(const uint8_t *blob, int idx, uint32_t runtime_base)
{
    const uint8_t *s4 = ani_sec(blob, ANI_SEC_SCRIPT);
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

/* =====================================================================
 * Guest-function ports — animation / skeleton transform math.
 *
 * These are the leaf routines the .ANI pose path (skeleton_pose_step ->
 * part_matrix_compute -> rtps_point_xform2) bottoms out in: the GTE
 * vec3 normalize family, the fixed-point rsqrt helpers, and the
 * angle-approach stepper. All are pure computation — no IRQ, no
 * blocking, no live guest sp — so they port faithfully to clean C.
 *
 * Two register ABIs appear here:
 *   - standard a0-a3 (wrapper marshals gpr[4..7], guest_at's pointers)
 *   - the GTE leaf ABI: vector in $t0-$t2 (cpu->gpr[8..10]), result
 *     written back to $t0-$t2. These take no C arguments; the body
 *     reads/writes the live CPUState via clean_cpu_state().
 * ===================================================================== */

extern CPUState *clean_cpu_state(void);
extern void *guest_at(uint32_t addr);
extern void setCopReg(int,int,uint32_t);
extern int  getCopReg(int,int);
extern void copFunction(int,int);

#define GTE_REG(n) ((n) * 0x800)   /* COP2 reg field encoding used by bind.c */

/* Shared s16 reciprocal/magnitude table used by the normalize core and
 * fixed_rsqrt_q12_ex. Guest RAM @0x801B8614 — generated loads compute
 * 0x801C0000 + idx - 31212 at 0x801851D8 (addiu @0x80185248, 0x25AD8614),
 * 0x80194690 (lh offset @0x8019470C, 0x85AD8614) and 0x80194590
 * (lh offset @0x801945FC, 0x85AD8614) — indexed by (m-64)*2 where m is the
 * normalized mantissa. 0x801C0000 - 31212 == 0x801B8614. */
#define BR2_RECIP_TABLE 0x801B8614u
/* Second table used only by fixed_rsqrt_q12. Guest RAM @0x801B8480. */
#define BR2_RSQRT_TABLE 0x801B8480u

/* gte_vec3_normalize_core — the SQR -> LZCS/LZCR -> table -> GPF sequence
 * shared verbatim by guest 0x801851D8 and 0x80194690 (instruction-level
 * diff shows only scheduling/nop differences; same table, same GPF op
 * 0x190003D, same $t0-$t2 ABI). */
static void gte_vec3_normalize_core(CPUState *c)
{
    uint32_t s, lz, sh, m, idx, v0;
    int32_t d, n;

    /* IR1..IR3 = input vector ($t0-$t2). */
    setCopReg(2, GTE_REG(9),  c->gpr[8]);
    setCopReg(2, GTE_REG(10), c->gpr[9]);
    setCopReg(2, GTE_REG(11), c->gpr[10]);
    copFunction(2, 0x0A00428);              /* SQR: MAC1..3 = IR1..3^2 */

    s  = (uint32_t)getCopReg(2, GTE_REG(25));   /* MAC1 */
    s += (uint32_t)getCopReg(2, GTE_REG(26));   /* MAC2 */
    s += (uint32_t)getCopReg(2, GTE_REG(27));   /* MAC3 */

    setCopReg(2, GTE_REG(30), s);           /* LZCS: latch source, lzcr */
    lz = (uint32_t)getCopReg(2, GTE_REG(31)) & 0xFFFFFFFEu;
    sh = (uint32_t)(((int32_t)(31u - lz)) >> 1);   /* sra */

    d = (int32_t)lz - 24;
    if (d < 0) {
        n = (24 - (int32_t)lz) & 31;
        m = (uint32_t)((int32_t)s >> n);    /* sra */
    } else {
        m = s << (d & 31);                  /* sll */
    }
    idx = (m - 64u) << 1;
    v0 = (uint32_t)*(const int16_t *)guest_at(BR2_RECIP_TABLE + idx);

    /* IR0 = table mantissa, IR1..IR3 = original vector, then GPF. */
    setCopReg(2, GTE_REG(8),  v0);
    setCopReg(2, GTE_REG(9),  c->gpr[8]);
    setCopReg(2, GTE_REG(10), c->gpr[9]);
    setCopReg(2, GTE_REG(11), c->gpr[10]);
    copFunction(2, 0x190003D);              /* GPF */

    n = (int32_t)(sh & 31);
    c->gpr[8]  = (uint32_t)((int32_t)(uint32_t)getCopReg(2, GTE_REG(25)) >> n);
    c->gpr[9]  = (uint32_t)((int32_t)(uint32_t)getCopReg(2, GTE_REG(26)) >> n);
    c->gpr[10] = (uint32_t)((int32_t)(uint32_t)getCopReg(2, GTE_REG(27)) >> n);
}

/* guest 0x801851D8 — animation-side copy of the vec3 normalize.
 * Called by part_matrix_compute; $t0-$t2 in/out, no C args. */
void anim_vec3_normalize(void)
{
    CPUState *c = clean_cpu_state();
    if (c) gte_vec3_normalize_core(c);
}

/* guest 0x80194690 — libgte vec3 normalize core.
 * Same algorithm as 0x801851D8; entered via jal from the 0x801946xx
 * wrapper family. $t0-$t2 in/out, no C args. */
void gte_vec3_normalize(void)
{
    CPUState *c = clean_cpu_state();
    if (c) gte_vec3_normalize_core(c);
}

/* guest 0x80194660 (falls through split tail 0x8019466C):
 * normalize a vec3 of signed 16-bit lanes.
 * a0 = in (3 x s16), a1 = out (3 x s16, low halves of $t0-$t2). */
void vec3_s16_normalize(uint8_t *in, uint8_t *out)
{
    CPUState *c = clean_cpu_state();
    if (!c) return;
    c->gpr[8]  = (uint32_t)(int32_t)*(const int16_t *)(in + 0);
    c->gpr[9]  = (uint32_t)(int32_t)*(const int16_t *)(in + 2);
    c->gpr[10] = (uint32_t)(int32_t)*(const int16_t *)(in + 4);
    gte_vec3_normalize_core(c);
    *(uint16_t *)(out + 0) = (uint16_t)c->gpr[8];
    *(uint16_t *)(out + 2) = (uint16_t)c->gpr[9];
    *(uint16_t *)(out + 4) = (uint16_t)c->gpr[10];
}

/* guest 0x80194630: normalize a vec3 of 32-bit lanes (lw in / sw out). */
void vec3_s32_normalize(uint32_t *in, uint32_t *out)
{
    CPUState *c = clean_cpu_state();
    if (!c) return;
    c->gpr[8]  = in[0];
    c->gpr[9]  = in[1];
    c->gpr[10] = in[2];
    gte_vec3_normalize_core(c);
    out[0] = c->gpr[8];
    out[1] = c->gpr[9];
    out[2] = c->gpr[10];
}

/* guest 0x8019461C (falls through split tail 0x8019466C):
 * normalize a vec3 of 32-bit lanes, store signed 16-bit results.
 * a0 = in (3 x u32), a1 = out (3 x s16). */
void vec3_s32_h_normalize(uint32_t *in, uint16_t *out)
{
    CPUState *c = clean_cpu_state();
    if (!c) return;
    c->gpr[8]  = in[0];
    c->gpr[9]  = in[1];
    c->gpr[10] = in[2];
    gte_vec3_normalize_core(c);
    out[0] = (uint16_t)c->gpr[8];
    out[1] = (uint16_t)c->gpr[9];
    out[2] = (uint16_t)c->gpr[10];
}

/* guest 0x80194500 — fixed-point rsqrt approximation, Q12 result.
 * a0 = input u32 (LZCS source). Returns 0 when input == 0
 * (LZCR == 32, the guest's taken branch to the 0x8019457C stub).
 * Table @0x801B8480 differs from the normalize table. */
uint32_t fixed_rsqrt_q12(uint32_t val)
{
    uint32_t lz, sh, m, idx, v;
    int32_t d, n;

    setCopReg(2, GTE_REG(30), val);         /* LZCS */
    lz = (uint32_t)getCopReg(2, GTE_REG(31));
    if (lz == 32) return 0;
    lz &= 0xFFFFFFFEu;
    sh = (uint32_t)(((int32_t)(31u - lz)) >> 1);   /* sra */

    d = (int32_t)lz - 24;
    if (d < 0) {
        n = (24 - (int32_t)lz) & 31;
        m = (uint32_t)((int32_t)val >> n);  /* sra */
    } else {
        m = val << (d & 31);                /* sll */
    }
    idx = (m - 64u) << 1;
    v = (uint32_t)*(const int16_t *)guest_at(BR2_RSQRT_TABLE + idx);
    return (v << (sh & 31)) >> 12;          /* sll then srl */
}

/* guest 0x80194590 — rsqrt with exponent/mantissa outputs.
 * a0 = input u32; a1 = out mantissa (s16 table value, stored as word);
 * a2 = out scale exponent. Returns 1 on success, -1 when the input is
 * 0 or has the top bit set (LZCR == 32 or 0 -> 0x80194614 stub). */
int fixed_rsqrt_q12_ex(uint32_t val, int32_t *mant, int32_t *exp)
{
    uint32_t lz, sh, m, idx;
    int32_t d, n;
    int32_t v;

    setCopReg(2, GTE_REG(30), val);         /* LZCS */
    lz = (uint32_t)getCopReg(2, GTE_REG(31));
    if (lz == 32 || lz == 0) return -1;
    lz &= 0xFFFFFFFEu;
    sh = (uint32_t)(((int32_t)(31u - lz)) >> 1);   /* sra */

    d = (int32_t)lz - 24;
    if (d < 0) {
        n = (24 - (int32_t)lz) & 31;
        m = (uint32_t)((int32_t)val >> n);  /* sra */
    } else {
        m = val << (d & 31);                /* sll */
    }
    idx = (m - 64u) << 1;
    v = *(const int16_t *)guest_at(BR2_RECIP_TABLE + idx);
    *exp  = (int32_t)sh;                    /* guest stores $t1 -> a2 */
    *mant = v;                              /* guest stores table s16 -> a1 */
    return 1;
}

/* guest 0x801858F8 — angle/velocity approach stepper (callback reached
 * via function pointer; no static jal callers).
 * a0 = state ptr (s16 field at +6), a1 = target ptr (s16 at +4),
 * a2 = direction flag (nonzero negates the delta),
 * a3 = Q12 rate scale.
 * new = old_u16 + ((2*d - x) * a3 >> 12 with floor rounding). */
void anim_angle_approach(uint8_t *a0, uint8_t *a1, int32_t a2, int32_t a3)
{
    int32_t d, x, q;
    uint32_t xu;

    d = (int32_t)(int16_t)*(const uint16_t *)(a1 + 4);   /* lh */
    if (a2 != 0) d = -d;                                  /* beqz a2 keeps d */
    x  = (int32_t)(int16_t)*(const uint16_t *)(a0 + 6);   /* lh */
    xu = *(const uint16_t *)(a0 + 6);                     /* lhu */
    /* mult low word: (d*2 - x) * a3, wrapped to 32 bits. */
    q = (int32_t)((int64_t)((d << 1) - x) * (int64_t)a3);
    if (q < 0) q += 4095;                                 /* floor /4096 */
    q >>= 12;                                             /* sra */
    *(uint16_t *)(a0 + 6) = (uint16_t)(xu + (uint32_t)q);
}

/* guest 0x80185444 — 8-byte alignment nop (jr ra; nop) sitting between
 * trig_sin and skeleton_pose_step; trig_sin tail-branches here. */
void anim_ret_nop(void)
{
}
