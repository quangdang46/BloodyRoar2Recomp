/* fight.c — per-fighter fight logic: action-request resolution.
 *
 * Clean-room C port of decoded guest functions. All state accessed
 * through raw guest-layout buffers (same caveat as ev.c: 32-bit
 * guest addresses need the recomp address-space bridge on 64-bit).
 *
 * Guest map:
 *   FUN_80176248  action-request resolver (ctrl, battle) -> action idx
 *                 (clean: action_resolve in resolve.c; dispatch-registered
 *                 via renames.map)
 *   pending_action_gate  pending-action permission gate (motion, ctrl, a, b)
 *                 (clean: pending_gate here; bind.c guest-ABI wrapper)
 *   FUN_801787dc  per-fighter frame update (calls resolver, writes
 *                 motion+0x80) (clean: fighter_frame in resolve.c;
 *                 dispatch-registered via renames.map)
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

/* ================================================================== */
/* fighter_hud_emit_pair — per-frame dual-fighter HUD/status emit        */
/*                                                                    */
/* Faithful port of guest 0x801343BC (652B, 25 blocks). Emits five       */
/* prim_task_run channel-script records against the caller's emit      */
/* bases (p1+0x2048 / p1+0x2074), fills the record's transform fields    */
/* from the +0xbc-linked status struct, runs gte_xform3_t_ip on the     */
/* record's +0x74 matrix, then for BOTH fighter status structs          */
/* (scratchpad 0x1F8000E8 / 0x1F8001F0) conditionally allocates a       */
/* gauge-bar FX group and posts deferred_cmd_post(0x400,0x84,0).        */
/*                                                                    */
/* Guest ABI: a0=emit base, a1=script ctx, a2=record (guest addr),      */
/* a3=ctx whose +0xbc is a guest status-struct ptr, stack arg = xform   */
/* target (guest addr). Clean wrapper marshals a0-a3 + stack arg.       */
/* ================================================================== */

extern void *guest_at(uint32_t addr);
extern int prim_task_run(int param_1, uint32_t param_2, uint32_t param_3, int param_4);
extern uint32_t *gte_xform3_t_ip(uint32_t *param_1, uint16_t *param_2);
extern int fx_group_alloc(uint32_t param_1, uint32_t param_2, int param_3, int param_4);
extern uint32_t noop_c(int param_1);
extern void deferred_cmd_post(uint32_t param_1, int param_2, int16_t param_3);

/* Absolute guest-address cell access (guest addr -> host mirror). */
#define GA8(a)   (*(const int8_t  *)guest_at(a))
#define GA16(a)  (*(const int16_t  *)guest_at(a))
#define GA32(a)  (*(const uint32_t *)guest_at(a))
#define GAS32(a) (*(int32_t  *)guest_at(a))
#define GASU32(a) (*(uint32_t *)guest_at(a))

/* One fighter status block — guest duplicates this body verbatim for
 * s6=0x1F8000E8 then s7=0x1F8001F0; a helper keeps the port auditable.
 * fb is the guest address of the fighter status struct. */
static void fighter_hud_gauge_block(uint32_t fb)
{
    /* (flags & 0x00080080) == 0x00080000  and  h94 > 0 (signed) */
    if ((GA32(fb + 0x5c) & 0x00080080u) != 0x00080000u) return;
    if ((int32_t)GA16(fb + 0x94) <= 0) return;
    if ((GA32(fb + 0x60) & 0x2u) == 0) {
        int alloc = fx_group_alloc(2, 1, 3, 0);
        if (alloc != 0) {
            uint32_t rec = noop_c(alloc);          /* guest addr */
            uint32_t src = GA32(fb + 0x20);        /* guest addr */
            GASU32(rec + 4)  = GA32(src + 0x174);
            GASU32(rec + 8)  = GA32(src + 0x178);
            GASU32(rec + 0xc)= GA32(src + 0x17c);
        }
        /* jal delay slot sets a2=0 on BOTH the alloc==0 and fall-through
         * paths -> deferred_cmd_post(0x400,0x84,0) unconditionally here. */
        deferred_cmd_post(0x400, 0x84, 0);
    }
    GASU32(fb + 0x60) = GA32(fb + 0x60) | 0x2u;
}

void fighter_hud_emit_pair(uint32_t p1, uint32_t p2, uint32_t p3,
                           uint32_t p4, uint32_t p5)
{
    uint32_t rec = p3;                    /* s0 — guest record addr */
    uint32_t s4  = GA32(p4 + 0xbc);       /* guest status-struct ptr */
    uint32_t base1 = p1 + 0x2048;         /* s1 */
    uint32_t base2;                       /* s2 after update */

    /* Five prim_task_run emits (guest jal 0x8018A4F8). a1 = emit base,
     * a2 = script ctx (p2), a3 = channel/stride literal. The guest
     * reloads lw a0,8(s0) before EVERY call — rec+8 may be rewritten
     * by the callee, so it is re-read each time, not CSE'd. */
    prim_task_run((int)GA32(rec + 0x00), base1, p2, 1);
    prim_task_run((int)(GA32(rec + 8) + 0xc8), base1, p2, 1);
    base2 = p1 + 0x2074;
    prim_task_run((int)(GA32(rec + 8) + 0xf0), base2, p2, 3);
    prim_task_run((int)(GA32(rec + 8) + 0xa0), base2, p2, 1);

    /* Record scalar fields + transform. lh sign-extends into sw. */
    GASU32(rec + 0x8c) = 0;
    GAS32 (rec + 0x88) = GA16(s4 + 0x4a);
    GAS32 (rec + 0x90) = GA16(s4 + 0x4e);
    gte_xform3_t_ip((uint32_t *)guest_at(p5),
                    (uint16_t *)guest_at(rec + 0x74));
    prim_task_run((int)(GA32(rec + 8) + 0x168), base2, p2, 1);

    /* Dual fighter gauge blocks (guest order: 0x1F8000E8 then 0x1F8001F0). */
    fighter_hud_gauge_block(0x1f8000e8u);
    fighter_hud_gauge_block(0x1f8001f0u);
}

/* ================================================================== */
/* node_list_walk_* — intrusive linked-list apply, 5 identical guest    */
/* template instances (0x80137600 / 0x801387DC / 0x80138C6C /           */
/* 0x8013E374 / 0x8013F7A8 — byte-identical modulo branch targets).     */
/*                                                                    */
/* Guest body (verified insn-by-insn on all five):                      */
/*   do {                                                              */
/*     head  = *list_holder;         // lw v0,(s1)                     */
/*     next  = *node;                // lw s0,(a1)  (node->next @ +0)  */
/*     head->back = node;            // sw a1,0x40(v0)                  */
/*     if (*(s8)(node+0x17) != 0) {                                   */
/*       if (*(s8)(node+0x18) != 0) task_slot_step();                 */
/*       else call node->fn8(node->f10, node, a2, a3)  // jalr v0      */
/*     }                                                               */
/*     node = next;                                                    */
/*   } while (next != 0);                                              */
/*                                                                    */
/* The indirect jalr target is a guest fn ptr read from node+8 with    */
/* a0=node+0x10 field, a1=node, a2/a3 = the caller's untouched a2/a3   */
/* (nothing in the loop writes them) -> call_guest4 reproduces the     */
/* jalr ABI exactly, including the $t0=target convention.              */
/* ================================================================== */

extern void task_slot_step(void);
extern uint32_t call_guest4(uint32_t fn, uint32_t a0, uint32_t a1,
                            uint32_t a2, uint32_t a3);

static void node_list_walk_core(uint32_t list_holder, uint32_t node,
                                uint32_t a2, uint32_t a3)
{
    for (;;) {
        uint32_t head = GA32(list_holder);
        uint32_t next = GA32(node);
        GASU32(head + 0x40) = node;
        if (GA8(node + 0x17) != 0) {        /* lb — only ==0 tested */
            if (GA8(node + 0x18) != 0) {
                task_slot_step();
            } else {
                uint32_t fn = GA32(node + 8);
                uint32_t arg0 = GA32(node + 0x10);
                call_guest4(fn, arg0, node, a2, a3);
            }
        }
        if (next == 0) break;
        node = next;
    }
}

void node_list_walk_a(uint32_t list_holder, uint32_t node,
                      uint32_t a2, uint32_t a3)
{ node_list_walk_core(list_holder, node, a2, a3); }

void node_list_walk_b(uint32_t list_holder, uint32_t node,
                      uint32_t a2, uint32_t a3)
{ node_list_walk_core(list_holder, node, a2, a3); }

void node_list_walk_c(uint32_t list_holder, uint32_t node,
                      uint32_t a2, uint32_t a3)
{ node_list_walk_core(list_holder, node, a2, a3); }

void node_list_walk_d(uint32_t list_holder, uint32_t node,
                      uint32_t a2, uint32_t a3)
{ node_list_walk_core(list_holder, node, a2, a3); }

void node_list_walk_e(uint32_t list_holder, uint32_t node,
                      uint32_t a2, uint32_t a3)
{ node_list_walk_core(list_holder, node, a2, a3); }
