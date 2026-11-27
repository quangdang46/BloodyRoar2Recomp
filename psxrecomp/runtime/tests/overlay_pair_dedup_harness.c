#define PSX_OVERLAY_DLL_BUILD 1
#include "overlay_loader.h"
#undef PSX_OVERLAY_DLL_BUILD

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#define TEST_MARKER 0xC001CAFEu
#define RAM_SIZE (2u * 1024u * 1024u)

_Static_assert(PSX_OVERLAY_TEST_CANDIDATE_CAP == 4,
               "pair-dedup harness must exercise the exact four-slot cap");

static uint8_t s_ram[RAM_SIZE];
static uint8_t s_scratch[1024];

uint32_t g_debug_current_func_addr;
uint32_t g_debug_last_store_pc;
uint32_t g_overlay_region_floor;
uint32_t i_stat, i_mask;
int g_exec_phase;
int g_idle_note_suppress;
int g_psx_call_bail;
uint32_t g_dirty_ram_code_gen;
uint64_t g_psx_bail_first, g_psx_bail_resolved;
uint64_t s_frame_count;
int g_shadow_mmio_watch;
uint64_t g_shadow_mmio_hits;

/* memory get ram ptr: return s_ram; */
uint8_t *memory_get_ram_ptr(void) { return s_ram; }
/* memory get scratchpad ptr: return s_scratch; */
uint8_t *memory_get_scratchpad_ptr(void) { return s_scratch; }
/* overlay watch pagegen sum */
uint32_t overlay_watch_pagegen_sum(uint32_t phys, uint32_t len) {
    (void)phys; (void)len; return 0;
}
/* overlay watch set range */
void overlay_watch_set_range(uint32_t phys, uint32_t len) {
    (void)phys; (void)len;
}
/* ds init */
void ds_init(const char *cache_dir, const char *game_id) {
    (void)cache_dir; (void)game_id;
}
/* dirty ram get bitmap word: (void)word; return 0; */
uint32_t dirty_ram_get_bitmap_word(uint32_t word) { (void)word; return 0; }
/* dirty ram is dirty: (void)phys; return 0; */
int dirty_ram_is_dirty(uint32_t phys) { (void)phys; return 0; }
/* dirty ram dispatch */
int dirty_ram_dispatch(CPUState *cpu, uint32_t addr, uint32_t stop) {
    (void)cpu; (void)addr; (void)stop; return 0;
}
/* dirty ram xprobe call note */
void dirty_ram_xprobe_call_note(CPUState *cpu, uint32_t target,
                                uint32_t ra, uint8_t phase) {
    (void)cpu; (void)target; (void)ra; (void)phase;
}

/* psx dispatch call */
void psx_dispatch_call(CPUState *cpu, uint32_t addr, uint32_t ra) {
    (void)cpu; (void)addr; (void)ra;
}
/* psx check interrupts: (void)cpu; */
void psx_check_interrupts(CPUState *cpu) { (void)cpu; }
/* psx check interrupts at */
void psx_check_interrupts_at(CPUState *cpu, uint32_t pc) {
    (void)cpu; (void)pc;
}
/* psx interrupt delivery needed: (void)cpu; return 0; */
int psx_interrupt_delivery_needed(const CPUState *cpu) { (void)cpu; return 0; }
/* psx get in exception: return 0; */
int psx_get_in_exception(void) { return 0; }
/* psx exception setjmp epoch: return 0; */
uint64_t psx_exception_setjmp_epoch(void) { return 0; }
/* psx restore state escape */
void psx_restore_state_escape(void) {}
/* psx rfe mark escape */
void psx_rfe_mark_escape(void) {}
/* psx syscall */
int psx_syscall(CPUState *cpu, uint32_t code) {
    (void)cpu; (void)code; return 0;
}
/* psx unknown dispatch */
void psx_unknown_dispatch(CPUState *cpu, uint32_t addr, uint32_t phys) {
    (void)cpu; (void)addr; (void)phys;
}

/* psx advance cycles: (void)cycles; */
void psx_advance_cycles(uint32_t cycles) { (void)cycles; }
/* psx get cycle count: return 0; */
uint64_t psx_get_cycle_count(void) { return 0; }
/* psx cycle replay begin: (void)cycle; return 1; */
int psx_cycle_replay_begin(uint64_t cycle) { (void)cycle; return 1; }
/* psx cycle replay end: return 0; */
uint64_t psx_cycle_replay_end(void) { return 0; }
/* psx cyc load word */
uint32_t psx_cyc_load_word(CPUState *cpu, uint32_t addr, uint32_t rt,
                           uint32_t mask) {
    (void)cpu; (void)addr; (void)rt; (void)mask; return 0;
}
/* psx cyc load word slow: return psx_cyc_load_word(cpu, addr, rt, mask) */
uint32_t psx_cyc_load_word_slow(CPUState *cpu, uint32_t addr, uint32_t rt,
                                uint32_t mask) {
    return psx_cyc_load_word(cpu, addr, rt, mask);
}
/* psx cyc load half */
uint16_t psx_cyc_load_half(CPUState *cpu, uint32_t addr, uint32_t rt,
                           uint32_t mask) {
    (void)cpu; (void)addr; (void)rt; (void)mask; return 0;
}
/* psx cyc load half slow: return psx_cyc_load_half(cpu, addr, rt, mask) */
uint16_t psx_cyc_load_half_slow(CPUState *cpu, uint32_t addr, uint32_t rt,
                                uint32_t mask) {
    return psx_cyc_load_half(cpu, addr, rt, mask);
}
/* psx cyc load byte */
uint8_t psx_cyc_load_byte(CPUState *cpu, uint32_t addr, uint32_t rt,
                          uint32_t mask) {
    (void)cpu; (void)addr; (void)rt; (void)mask; return 0;
}
/* psx cyc lwc2 read */
uint32_t psx_cyc_lwc2_read(CPUState *cpu, uint32_t addr) {
    (void)cpu; (void)addr; return 0;
}
/* psx icache shadow record begin: return 1; */
int psx_icache_shadow_record_begin(void) { return 1; }
/* psx icache shadow replay begin: return 1; */
int psx_icache_shadow_replay_begin(void) { return 1; }
/* psx icache shadow replay end */
void psx_icache_shadow_replay_end(void) {}
/* psx icache shadow abort */
void psx_icache_shadow_abort(void) {}
/* psx icache fetch: (void)cpu; (void)addr; */
void psx_icache_fetch(CPUState *cpu, uint32_t addr) { (void)cpu; (void)addr; }
/* psx icache fetch fn */
void psx_icache_fetch_fn(CPUState *cpu, uint32_t addr) {
    psx_icache_fetch(cpu, addr);
}
/* psx muldiv set */
void psx_muldiv_set(CPUState *cpu, uint32_t latency) {
    (void)cpu; (void)latency;
}
/* psx muldiv stall: (void)cpu; */
void psx_muldiv_stall(CPUState *cpu) { (void)cpu; }
/* psx mult latency s: (void)rs; return 1; */
uint32_t psx_mult_latency_s(uint32_t rs) { (void)rs; return 1; }
/* psx mult latency u: (void)rs; return 1; */
uint32_t psx_mult_latency_u(uint32_t rs) { (void)rs; return 1; }
/* psx gte stall: (void)cpu; */
void psx_gte_stall(CPUState *cpu) { (void)cpu; }
/* psx gte read: (void)cpu; (void)rt; */
void psx_gte_read(CPUState *cpu, uint32_t rt) { (void)cpu; (void)rt; }
/* psx slice block */
int psx_slice_block(CPUState *cpu, uint32_t addr, uint32_t cycles,
                    int side_effects) {
    (void)cpu; (void)addr; (void)cycles; (void)side_effects; return 0;
}
/* psx slice block impl: return psx_slice_block(cpu, addr, cycles, side_effects) */
int psx_slice_block_impl(CPUState *cpu, uint32_t addr, uint32_t cycles,
                         int side_effects) {
    return psx_slice_block(cpu, addr, cycles, side_effects);
}

/* gte execute: (void)cpu; (void)cmd; */
void gte_execute(CPUState *cpu, uint32_t cmd) { (void)cpu; (void)cmd; }
/* gte read data */
uint32_t gte_read_data(CPUState *cpu, uint8_t reg) {
    (void)cpu; (void)reg; return 0;
}
/* gte read ctrl */
uint32_t gte_read_ctrl(CPUState *cpu, uint8_t reg) {
    (void)cpu; (void)reg; return 0;
}
/* gte write data */
void gte_write_data(CPUState *cpu, uint8_t reg, uint32_t val) {
    (void)cpu; (void)reg; (void)val;
}
/* gte write ctrl */
void gte_write_ctrl(CPUState *cpu, uint8_t reg, uint32_t val) {
    (void)cpu; (void)reg; (void)val;
}
/* psx ws plane nx: return nx; */
int32_t psx_ws_plane_nx(int32_t nx) { return nx; }
/* psx ws xclip bound: return vanilla; */
uint32_t psx_ws_xclip_bound(uint32_t vanilla) { return vanilla; }
/* gte precision store word */
void gte_precision_store_word(uint32_t addr, uint8_t reg) {
    (void)addr; (void)reg;
}
/* gte precision speculative begin: if (ops) *ops = 0 */
void gte_precision_speculative_begin(void) {}
/* gte precision speculative end: if (ops) *ops = 0 */
void gte_precision_speculative_end(void) {}
/* gte replay side effects begin: return 1; */
int gte_replay_side_effects_begin(void) { return 1; }
/* gte replay side effects end: if (ops) *ops = 0 */
void gte_replay_side_effects_end(void) {}

/* ls shadow record begin: return 1; */
int ls_shadow_record_begin(void) { return 1; }
/* ls shadow record end: if (ops) *ops = 0 */
int ls_shadow_record_end(uint32_t *ops, int *exc) {
    if (ops) *ops = 0;
    if (exc) *exc = 0;
    return 1;
}
/* ls shadow replay begin: return 1; */
int ls_shadow_replay_begin(void) { return 1; }
/* ls shadow replay end: return 1 */
int ls_shadow_replay_end(uint32_t *ops, int *kind, uint32_t *pc,
                         uint32_t *addr, uint32_t *expected,
                         uint32_t *actual) {
    (void)ops; (void)kind; (void)pc; (void)addr; (void)expected; (void)actual;
    return 1;
}
/* ls shadow abort */
void ls_shadow_abort(void) {}

/* psx ws backdrop x: return x; */
int psx_ws_backdrop_x(int x) { return x; }
/* psx ws x margin: return 0; */
int psx_ws_x_margin(void) { return 0; }
/* psx ws sprite tag: (void)cpu; */
void psx_ws_sprite_tag(CPUState *cpu) { (void)cpu; }
/* psx ws backdrop value */
uint32_t psx_ws_backdrop_value(uint32_t orig, int end, int cols) {
    (void)end; (void)cols; return orig;
}
/* psx ws depth bound: return imm; */
int32_t psx_ws_depth_bound(int32_t imm) { return imm; }
/* psx ws player x bound: return vanilla; */
int32_t psx_ws_player_x_bound(int32_t vanilla) { return vanilla; }

typedef int (*CounterFn)(void);

#ifdef _WIN32
typedef HMODULE TestModule;
/* module is loaded: return GetModuleHandleA(path) != NULL */
static int module_is_loaded(const char *path) {
    return GetModuleHandleA(path) != NULL;
}
/* module open: return LoadLibraryA(path); */
static TestModule module_open(const char *path) { return LoadLibraryA(path); }
/* module counter: return (CounterFn)GetProcAddress(module, name) */
static CounterFn module_counter(TestModule module, const char *name) {
    return (CounterFn)GetProcAddress(module, name);
}
/* module close: if (module) FreeLibrary(module); */
static void module_close(TestModule module) { if (module) FreeLibrary(module); }
#else
typedef void *TestModule;
/* module is loaded: void *module = dlopen(path, RTLD_NOW | RTLD_NOLOAD) */
static int module_is_loaded(const char *path) {
#ifndef RTLD_NOLOAD
#error "pair-dedup handle ownership test requires RTLD_NOLOAD"
#else
    void *module = dlopen(path, RTLD_NOW | RTLD_NOLOAD);
    if (!module) return 0;
    dlclose(module);
    return 1;
#endif
}
/* module open: return dlopen(path, RTLD_NOW | RTLD_LOCAL) */
static TestModule module_open(const char *path) {
    return dlopen(path, RTLD_NOW | RTLD_LOCAL);
}
/* module counter: return (CounterFn)dlsym(module, name) */
static CounterFn module_counter(TestModule module, const char *name) {
    return (CounterFn)dlsym(module, name);
}
/* module close: if (module) dlclose(module); */
static void module_close(TestModule module) { if (module) dlclose(module); }
#endif

/* counter value: TestModule module = module_open(path) */
static int counter_value(const char *path, const char *name) {
    TestModule module = module_open(path);
    if (!module) return -1000;
    CounterFn counter = module_counter(module, name);
    int value = counter ? counter() : -1001;
    module_close(module);
    return value;
}

/* expect int: if (actual == expected) return 1 */
static int expect_int(const char *what, long long actual, long long expected) {
    if (actual == expected) return 1;
    fprintf(stderr, "%s: got %lld, expected %lld\n", what, actual, expected);
    return 0;
}

/* loader owner count: uint32_t loads = 0 */
static uint32_t loader_owner_count(void) {
    uint32_t loads = 0;
    overlay_loader_get_counters(&loads, NULL, NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL, NULL, NULL);
    return loads;
}

/* replace suffix: size_t path_len = strlen(path) */
static int replace_suffix(char *out, size_t cap, const char *path,
                          const char *suffix) {
    size_t path_len = strlen(path);
#ifdef _WIN32
    const char *extension = ".dll";
#else
    const char *extension = ".so";
#endif
    size_t extension_len = strlen(extension);
    if (path_len < extension_len ||
        strcmp(path + path_len - extension_len, extension) != 0)
        return 0;
    int written = snprintf(out, cap, "%.*s%s",
                           (int)(path_len - extension_len), path, suffix);
    return written > 0 && (size_t)written < cap;
}

/* reveal second pair: char pending[1024], ranges[1024], ranges_pending[1024] */
static int reveal_second_pair(const char *second) {
    char pending[1024], ranges[1024], ranges_pending[1024];
    char resident[1024], resident_pending[1024];
    int pending_n = snprintf(pending, sizeof(pending), "%s.pending", second);
    if (pending_n <= 0 || (size_t)pending_n >= sizeof(pending) ||
        !replace_suffix(ranges, sizeof(ranges), second, ".ranges") ||
        !replace_suffix(resident, sizeof(resident), second, ".resident"))
        return 0;
    int ranges_n = snprintf(ranges_pending, sizeof(ranges_pending),
                            "%s.pending", ranges);
    int resident_n = snprintf(resident_pending, sizeof(resident_pending),
                              "%s.pending", resident);
    if (ranges_n <= 0 || (size_t)ranges_n >= sizeof(ranges_pending) ||
        resident_n <= 0 || (size_t)resident_n >= sizeof(resident_pending))
        return 0;
    /* Reveal metadata first and the DLL last, matching transactional publication. */
    if (rename(ranges_pending, ranges) != 0 ||
        rename(resident_pending, resident) != 0 ||
        rename(pending, second) != 0) {
        perror("reveal staged pair");
        return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "usage: %s <cache-root> <scenario> <first> <second>\n",
                argv[0]);
        return 2;
    }
    const char *scenario = argv[2];
    const char *first = argv[3];
    const char *second = argv[4];
    memset(s_ram, 0, sizeof(s_ram));
    overlay_loader_init(argv[1], "PAIR-TEST", 0);

    int alias = strcmp(scenario, "alias-at-cap") == 0;
    int partial = strcmp(scenario, "partial-first") == 0;
    int ok = 1;

    /* Only the first physical pair exists at init. This pins canonical order,
     * then makes rescan responsible for discovering the staged second pair. */
    ok &= expect_int("pre-rescan registered", overlay_loader_registered_count(),
                     partial ? 0 : (alias ? 4 : 2));
    ok &= expect_int("pre-rescan aliases",
                     (long long)overlay_loader_pair_aliases(), 0);
    ok &= expect_int("pre-rescan owners", loader_owner_count(), partial ? 0 : 1);
    ok &= expect_int("pre-rescan first init",
                     counter_value(first, "test_init_count"), partial ? 0 : 1);
    ok &= expect_int("pre-rescan first retained", module_is_loaded(first),
                     partial ? 0 : 1);
    if (!reveal_second_pair(second)) return 3;
    overlay_loader_rescan();

    ok &= expect_int("registered", overlay_loader_registered_count(),
                     alias ? 4 : (partial ? 2 : 4));
    ok &= expect_int("pair aliases", (long long)overlay_loader_pair_aliases(),
                     alias ? 1 : 0);
    ok &= expect_int("candidate overflow",
                     (long long)overlay_loader_candidate_overflow(), 0);
    ok &= expect_int("loader owners", loader_owner_count(),
                     alias || partial ? 1 : 2);
    ok &= expect_int("first init", counter_value(first, "test_init_count"),
                     partial ? 0 : 1);
    ok &= expect_int("second init", counter_value(second, "test_init_count"),
                     alias ? 0 : 1);

    int first_loaded = module_is_loaded(first);
    int second_loaded = module_is_loaded(second);
    ok &= expect_int("first retained", first_loaded, partial ? 0 : 1);
    ok &= expect_int("second retained", second_loaded, alias ? 0 : 1);

    if (alias) {
        CPUState cpu;
        memset(&cpu, 0, sizeof(cpu));
        ok &= expect_int("canonical dispatch",
                         overlay_loader_dispatch(&cpu, 0x80010000u), 1);
        ok &= expect_int("canonical marker", cpu.gpr[2], TEST_MARKER);
        ok &= expect_int("canonical call count",
                         counter_value(first, "test_call_count"), 1);
        ok &= expect_int("redundant call count",
                         counter_value(second, "test_call_count"), 0);
        ok &= expect_int("canonical flush count",
                         counter_value(first, "test_flush_count"), 1);
        ok &= expect_int("redundant flush count",
                         counter_value(second, "test_flush_count"), 0);
    }

    /* A second rescan must neither reacquire an alias handle nor publish
     * another owner/candidate set for an already satisfied physical path. */
    overlay_loader_rescan();
    ok &= expect_int("idempotent registered", overlay_loader_registered_count(),
                     alias ? 4 : (partial ? 2 : 4));
    ok &= expect_int("idempotent aliases",
                     (long long)overlay_loader_pair_aliases(), alias ? 1 : 0);
    ok &= expect_int("idempotent owners", loader_owner_count(),
                     alias || partial ? 1 : 2);

    if (!ok) {
        fprintf(stderr, "loader: %s; lazy=%d overflow=%d\n",
                overlay_loader_last_msg(), overlay_loader_lazy_manifest_count(),
                overlay_loader_lazy_manifest_overflow());
        return 1;
    }
    printf("PASS %s registered=%d aliases=%llu\n", scenario,
           overlay_loader_registered_count(),
           (unsigned long long)overlay_loader_pair_aliases());
    return 0;
}
