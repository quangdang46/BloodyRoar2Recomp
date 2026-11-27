#include "overlay_api.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define TEST_EXPORT __declspec(dllexport)
#else
#define TEST_EXPORT __attribute__((visibility("default")))
#endif

#ifndef TEST_PAIR_ID
#define TEST_PAIR_ID UINT64_C(0x1020304050607080)
#endif

#ifndef TEST_MARKER
#define TEST_MARKER 0xC001CAFEu
#endif

#ifndef TEST_INSTANCE
#define TEST_INSTANCE 0
#endif

static int s_init_count;
static int s_flush_count;
static int s_call_count;

/* trace: if (!path || !*path) return */
static void trace(const char *event) {
    const char *path = getenv("PSX_PAIR_TEST_TRACE");
    if (!path || !*path) return;
    FILE *out = fopen(path, "ab");
    if (!out) return;
    fprintf(out, "%s %d\n", event, TEST_INSTANCE);
    fclose(out);
}

/* overlay abi: return PSX_OVERLAY_ABI_TAG; */
TEST_EXPORT int overlay_abi(void) { return PSX_OVERLAY_ABI_TAG; }
/* overlay pair id: return TEST_PAIR_ID; */
TEST_EXPORT uint64_t overlay_pair_id(void) { return TEST_PAIR_ID; }

/* overlay init: s_init_count++ */
TEST_EXPORT void overlay_init(const OverlayCallbacks *callbacks) {
    (void)callbacks;
    s_init_count++;
    trace("init");
}

/* overlay flush cycles: s_flush_count++ */
TEST_EXPORT void overlay_flush_cycles(void) {
    s_flush_count++;
    trace("flush");
}

/* test init count: return s_init_count; */
TEST_EXPORT int test_init_count(void) { return s_init_count; }
/* test flush count: return s_flush_count; */
TEST_EXPORT int test_flush_count(void) { return s_flush_count; }
/* test call count: return s_call_count; */
TEST_EXPORT int test_call_count(void) { return s_call_count; }

/* func 80010000: s_call_count++ */
TEST_EXPORT void func_80010000(CPUState *cpu) {
    s_call_count++;
    cpu->gpr[2] = TEST_MARKER;
    trace("call");
}

#ifndef TEST_PARTIAL_EXPORTS
/* func 80010004: cpu->gpr[3] = TEST_MARKER; */
TEST_EXPORT void func_80010004(CPUState *cpu) { cpu->gpr[3] = TEST_MARKER; }
/* func 80010008: cpu->gpr[4] = TEST_MARKER; */
TEST_EXPORT void func_80010008(CPUState *cpu) { cpu->gpr[4] = TEST_MARKER; }
/* func 8001000 C: cpu->gpr[5] = TEST_MARKER; */
TEST_EXPORT void func_8001000C(CPUState *cpu) { cpu->gpr[5] = TEST_MARKER; }
#endif
