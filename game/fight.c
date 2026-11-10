/* fight.c — per-fighter fight logic: action-request resolution.
 *
 * Clean-room C port of decoded guest functions. All state accessed
 * through raw guest-layout buffers (same caveat as ev.c: 32-bit
 * guest addresses need the recomp address-space bridge on 64-bit).
 *
 * Guest map:
 *   FUN_80176248  action-request resolver (ctrl, battle) -> action idx
 *   pending_action_gate  pending-action permission gate (motion, ctrl, a, b)
 *   FUN_801787dc  per-fighter frame update (calls resolver, writes
 *                 motion+0x80)
 *   ev5_action_select  EV5 action-control record interpreter (in ev.c)
 */

#include <stdint.h>
#include <stddef.h>

#define G8(p,o)   (((const uint8_t *)(p))[(o)])
#define G16(p,o)  (*(const int16_t  *)((const uint8_t *)(p) + (o)))
#define GU16(p,o) (*(const uint16_t *)((const uint8_t *)(p) + (o)))
#define G32(p,o)  (*(const int32_t  *)((const uint8_t *)(p) + (o)))
#define GU32(p,o) (*(const uint32_t *)((const uint8_t *)(p) + (o)))
#define S8(p,o,v)  (((uint8_t *)(p))[(o)] = (uint8_t)(v))
#define S16(p,o,v) (*(int16_t  *)((uint8_t *)(p) + (o)) = (int16_t)(v))
#define SU16(p,o,v) (*(uint16_t *)((uint8_t *)(p) + (o)) = (uint16_t)(v))
#define SU32(p,o,v) (*(uint32_t *)((uint8_t *)(p) + (o)) = (uint32_t)(v))

/* ctrl offsets (shared with ev.c convention) */
#define FC_FOE     0x2c
#define FC_MOTION  0x38
#define FC_MODE5   0x05
#define FC_STATE0  0x58
#define FC_STATE1  0x5c
#define FC_STATE3  0x64
#define FC_STATE4  0x68
#define FC_STATE5  0x6c

/* motion offsets */
#define MO_14      0x14
#define MO_16      0x16
#define MO_18      0x18     /* u32 pending-action permission flags */
#define MO_1C      0x1c     /* s16 queued action (-1 = none) */
#define MO_24      0x24     /* s16 stored sub-action -> +0x90 bias */
#define MO_80      0x80     /* u16 current action index */
#define MO_82      0x82     /* u16 previous action index */
#define MO_9A      0x9a     /* s16 pending action field */
#define MO_D6      0xd6     /* s16 (clamped >=0 when flag set) */

extern void pending_hit_a_clear(uint8_t *ctrl);
extern void hit_chain_clear(uint8_t *ctrl);

/* ================================================================== */
/* pending_action_gate — pending-action permission gate                        */
/*                                                                    */
/* (motion, ctrl, reqA=ctrl+0x5c, reqB=ctrl+0x58) -> action or -1.     */
/* motion+0x18 bit 0x100000 mirrors into ctrl+0x6c bit 0x100.          */
/* Permission: reqA/reqB bits 0x800/0x10 select which motion+0x18     */
/* mask bits (1/2/4/8) must be set for the queued motion+0x1c action  */
/* to fire. Denied -> clear ctrl+0x6c bit 0x100, return -1.            */
/* Allowed -> consume queue (motion+0x1c = -1), hitstun helpers,      */
/* return queued + 0x90.                                              */
/* ================================================================== */
int32_t pending_gate(uint8_t *motion, uint8_t *ctrl,
                         uint32_t req_a, uint32_t req_b)
{
    int16_t pend;

    if ((GU32(motion, MO_18) & 0x100000u) != 0)
        SU32(ctrl, FC_STATE5, GU32(ctrl, FC_STATE5) | 0x100u);

    /* guest: skip the whole permission block when reqA&0x800 AND
     * motion+0x18&0x10 — i.e. an explicit cancel-permit. */
    if (!((req_a & 0x800u) != 0 && (GU32(motion, MO_18) & 0x10u) != 0)) {
        uint32_t perm;
        if ((req_b & 0x800u) != 0) {
            if ((GU32(motion, MO_18) & 0x4u) != 0) goto accept;
            perm = GU32(motion, MO_18) & 0x8u;
        } else {
            perm = ((req_b & 0x10u) != 0) ? (GU32(motion, MO_18) & 0x2u)
                                          : (GU32(motion, MO_18) & 0x1u);
        }
        if (perm == 0) {
            SU32(ctrl, FC_STATE5, GU32(ctrl, FC_STATE5) & ~0x100u);
            return -1;
        }
    }
accept:
    pend = G16(motion, MO_1C);
    S16(motion, MO_1C, -1);
    pending_hit_a_clear(ctrl);
    hit_chain_clear(ctrl);
    return (int32_t)pend + 0x90;
}
