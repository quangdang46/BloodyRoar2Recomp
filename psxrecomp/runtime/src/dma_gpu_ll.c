#include "dma_gpu_ll.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Diagnostic: ring of recent OT node hops, dumped on node-cap (hit_limit)
 * to expose the divergent next-pointer / cycle that sends the walker into
 * garbage. BR2_OT_TRACE=0 disables. */
#define OT_TRACE_CAP 256u
static uint32_t s_ot_tr_addr[OT_TRACE_CAP];
static uint32_t s_ot_tr_next[OT_TRACE_CAP];
static uint32_t s_ot_tr_n;
static uint32_t s_ot_tr_start;
static int      s_ot_tr_on = -1;
static void ot_trace_reset(void) {
    if (s_ot_tr_on < 0) {
        const char *e = getenv("BR2_OT_TRACE");
        s_ot_tr_on = (e && e[0] == '1');   /* opt-in: BR2_OT_TRACE=1 */
    }
    s_ot_tr_n = 0;
}
static void ot_trace_push(uint32_t addr, uint32_t next) {
    if (!s_ot_tr_on) return;
    uint32_t i = s_ot_tr_n & (OT_TRACE_CAP - 1u);
    s_ot_tr_addr[i] = addr; s_ot_tr_next[i] = next; s_ot_tr_n++;
}
static void ot_trace_dump(void) {
    if (!s_ot_tr_on || s_ot_tr_n == 0) return;
    uint32_t start = s_ot_tr_n > OT_TRACE_CAP ? s_ot_tr_n - OT_TRACE_CAP : 0;
    fprintf(stderr, "[ot_ll] HIT_LIMIT start=%08x nodes=%u dump_from=%u\n",
            s_ot_tr_start, s_ot_tr_n, start);
    for (uint32_t k = start; k < s_ot_tr_n; k++) {
        uint32_t i = k & (OT_TRACE_CAP - 1u);
        fprintf(stderr, "  [%u] %08x -> next %06x\n", k,
                s_ot_tr_addr[i], s_ot_tr_next[i]);
    }
    fflush(stderr);
}

/* Charge one guest clock for each 32-bit word read: one header per node and
 * one clock per payload word. There is no separate setup charge. Header and
 * payload reads remain separate events so guest writes can affect an active
 * transfer before DMA reaches the changed word. */
#define DMA_GPU_LL_HEADER_CYCLES 1u
#define DMA_GPU_LL_SETUP_CYCLES  0u

static uint32_t resolve_address(const DMAGPULinkedListOps *ops, void *opaque,
                                uint32_t address) {
    return ops->resolve_address ? ops->resolve_address(opaque, address) : address;
}

static void finish(DMAGPULinkedList *state,
                   const DMAGPULinkedListOps *ops, void *opaque) {
    int hit_limit = state->hit_limit != 0;
    if (hit_limit) ot_trace_dump();
    state->active = 0;
    state->phase = DMA_GPU_LL_PHASE_IDLE;
    state->cycles_remaining = 0;
    if (ops->complete) ops->complete(opaque, hit_limit);
}

void dma_gpu_ll_start(DMAGPULinkedList *state, uint32_t start_addr,
                      uint32_t max_nodes) {
    memset(state, 0, sizeof(*state));
    state->active = 1;
    state->phase = DMA_GPU_LL_PHASE_HEADER;
    state->start_addr = start_addr;
    state->current_addr = start_addr;
    state->cycles_remaining = DMA_GPU_LL_HEADER_CYCLES;
    state->max_nodes = max_nodes;
    state->empty_rank = UINT32_MAX;
    s_ot_tr_start = start_addr;
    ot_trace_reset();
}

void dma_gpu_ll_cancel(DMAGPULinkedList *state) {
    memset(state, 0, sizeof(*state));
}

uint32_t dma_gpu_ll_cycles_to_event(const DMAGPULinkedList *state) {
    if (!state->active) return UINT32_MAX;
    return state->cycles_remaining ? state->cycles_remaining : 1u;
}

void dma_gpu_ll_advance(DMAGPULinkedList *state, uint32_t cycles,
                        const DMAGPULinkedListOps *ops, void *opaque) {
    if (!state->active || cycles == 0 || !ops || !ops->read_word) return;
    for (;;) {
        if (cycles < state->cycles_remaining) {
            state->cycles_remaining -= cycles;
            return;
        }
        cycles -= state->cycles_remaining;
        state->cycles_remaining = 0;

        if (state->phase == DMA_GPU_LL_PHASE_HEADER) {
            if (state->nodes_processed >= state->max_nodes) {
                state->hit_limit = 1;
                finish(state, ops, opaque);
                return;
            }

            state->current_addr = resolve_address(
                ops, opaque, state->current_addr);
            uint32_t header = ops->read_word(opaque, state->current_addr);
            if (ops->observe_header)
                ops->observe_header(opaque, state->current_addr, header);
            state->word_count = header >> 24;
            state->payload_index = 0;
            state->next_addr = header & 0x00FFFFFFu;
            state->nodes_processed++;
            state->total_words++;
            ot_trace_push(state->current_addr, state->next_addr);
            if (state->word_count == 0)
                state->empty_rank = state->empty_rank == UINT32_MAX
                    ? 0u : state->empty_rank + 1u;
            if (state->word_count != 0) {
                state->emit_node = 1u;
                state->phase = DMA_GPU_LL_PHASE_SETUP;
                state->cycles_remaining = DMA_GPU_LL_SETUP_CYCLES;
            } else {
                state->emit_node = ops->begin_node
                    ? (uint8_t)(ops->begin_node(
                          opaque, state->current_addr, 0u) != 0)
                    : 1u;

                if (state->next_addr == 0x00FFFFFFu) {
                    finish(state, ops, opaque);
                    return;
                }
                state->current_addr = resolve_address(
                    ops, opaque, state->next_addr);
                state->cycles_remaining = DMA_GPU_LL_HEADER_CYCLES;
            }
        } else if (state->phase == DMA_GPU_LL_PHASE_SETUP) {
            state->emit_node = ops->begin_node
                ? (uint8_t)(ops->begin_node(
                      opaque, state->current_addr, state->word_count) != 0)
                : 1u;
            state->phase = DMA_GPU_LL_PHASE_PAYLOAD;
            state->cycles_remaining = 1u;
        } else if (state->phase == DMA_GPU_LL_PHASE_PAYLOAD) {
            uint32_t word_addr = resolve_address(
                ops, opaque, state->current_addr + 4u +
                             state->payload_index * 4u);
            if (state->emit_node) {
                uint32_t word = ops->read_word(opaque, word_addr);
                if (ops->emit_word)
                    ops->emit_word(opaque, word_addr, word);
            }
            state->payload_index++;
            state->total_words++;

            if (state->payload_index < state->word_count) {
                state->cycles_remaining = 1u;
            } else if (state->next_addr == 0x00FFFFFFu) {
                finish(state, ops, opaque);
                return;
            } else {
                state->phase = DMA_GPU_LL_PHASE_HEADER;
                state->current_addr = resolve_address(
                    ops, opaque, state->next_addr);
                state->cycles_remaining = DMA_GPU_LL_HEADER_CYCLES;
            }
        } else {
            state->hit_limit = 1;
            finish(state, ops, opaque);
            return;
        }

        if (!state->active) return;
        /* Consume a zero-cost phase transition in this service call. A later
         * read still waits for its own non-zero event. */
        if (cycles == 0 && state->cycles_remaining != 0) return;
    }
}
