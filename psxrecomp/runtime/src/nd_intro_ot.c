/* nd_intro_ot.c — CTR ND intro OT batch base cache for sibling flap depth. */
#include "nd_intro_ot.h"

static uint32_t s_wood_batch_ot;
static uint32_t s_wood_batch_ot_code;
static uint32_t s_wood_batch_ot_glow;
static uint32_t s_wood_batch_ot_box;
static uint32_t s_sibling_ot_hint;

/* looks like ot base: return base >= 0x800F0000u && base < 0x800F8000u && (base & 3u) == 0u */
static int looks_like_ot_base(uint32_t base)
{
    /* Wood batch OTs sit in gameplay RAM below the main OT windows. */
    return base >= 0x800F0000u && base < 0x800F8000u && (base & 3u) == 0u;
}

/* psx nd note wood batch ot: if (looks_like_ot_base(base)) */
void psx_nd_note_wood_batch_ot(uint32_t base)
{
    if (looks_like_ot_base(base))
        s_wood_batch_ot = base;
}

/* psx nd note wood batch ot tagged: if (!looks_like_ot_base(base)) */
void psx_nd_note_wood_batch_ot_tagged(uint32_t base, uint32_t tag)
{
    if (!looks_like_ot_base(base))
        return;
    s_wood_batch_ot = base;
    /* Prefer CODE (digit rain) > GLOW > BOX_ for sibling OT rewrite. */
    if (tag == 0x45444F43u) /* CODE */
        s_wood_batch_ot_code = base;
    else if (tag == 0x574F4C47u) /* GLOW */
        s_wood_batch_ot_glow = base;
    else if (tag == 0x5F584F42u) /* BOX_ */
        s_wood_batch_ot_box = base;
}

/* psx nd wood batch ot: if (s_wood_batch_ot_code) */
uint32_t psx_nd_wood_batch_ot(void)
{
    if (s_wood_batch_ot_code)
        return s_wood_batch_ot_code;
    if (s_wood_batch_ot_glow)
        return s_wood_batch_ot_glow;
    if (s_wood_batch_ot_box)
        return s_wood_batch_ot_box;
    return s_wood_batch_ot;
}

/* psx nd note sibling ot hint: if (looks_like_ot_base(base)) */
void psx_nd_note_sibling_ot_hint(uint32_t base)
{
    if (looks_like_ot_base(base))
        s_sibling_ot_hint = base;
}

/* psx nd sibling ot hint: return s_sibling_ot_hint */
uint32_t psx_nd_sibling_ot_hint(void)
{
    return s_sibling_ot_hint;
}
