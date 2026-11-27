/* psx_game_backend.c — universal multi-region game-image registry.
 *
 * Compiled for every target (harmless): when PSX_GAME_BACKEND_REGISTRY is not
 * defined the file is empty and the generated dispatcher's own strong
 * definitions are used, exactly as before. When it IS defined the registry
 * owns the unprefixed psx_game_* interface and forwards to the selected
 * image, and game_dispatch_compat.c opts out so it cannot double-define them.
 */

#include "psx_game_backend.h"

#include <ctype.h>
#include <stdlib.h>

#if defined(PSX_GAME_BACKEND_REGISTRY)

#define PSX_GAME_BACKEND_MAX 8

static const PsxGameBackend* g_backends[PSX_GAME_BACKEND_MAX];
static int                   g_backend_count = 0;
static const PsxGameBackend* g_active = 0;

/* psx game backend register: if (!backend) return */
void psx_game_backend_register(const PsxGameBackend* backend) {
    if (!backend) return;
    if (g_backend_count < PSX_GAME_BACKEND_MAX)
        g_backends[g_backend_count++] = backend;
}

/* psx game backend active: if (g_active) return g_active */
const PsxGameBackend* psx_game_backend_active(void) {
    /* A single-image build (the isolated per-region targets, or a dev run with
     * no mounted disc) has exactly one candidate, so use it without requiring
     * an explicit serial select. A universal build with several images fails
     * closed until a serial picks one. */
    if (g_active) return g_active;
    if (g_backend_count == 1) return g_backends[0];
    return 0;
}

/* Identity comparison that ignores separators and case, so "SLES-01722",
 * "SLES_017.22" and "sles01722" all name the same image. */
static int key_equal(const char* a, const char* b) {
    if (!a || !b) return 0;
    for (;;) {
        while (*a && !isalnum((unsigned char)*a)) ++a;
        while (*b && !isalnum((unsigned char)*b)) ++b;
        if (*a == 0 || *b == 0) return (*a == 0 && *b == 0);
        int ca = toupper((unsigned char)*a++);
        int cb = toupper((unsigned char)*b++);
        if (ca != cb) return 0;
    }
}

/* psx game backend match: for (int i = 0; i < g_backend_count; ++i)  */
const PsxGameBackend* psx_game_backend_match(const char* serial) {
    for (int i = 0; i < g_backend_count; ++i) {
        if (key_equal(g_backends[i]->game_id, serial) ||
            key_equal(g_backends[i]->boot_exe_stem, serial)) {
            return g_backends[i];
        }
    }
    return 0;
}

/* psx game backend select: if (be)  */
int psx_game_backend_select(const char* serial) {
    const PsxGameBackend* be = psx_game_backend_match(serial);
    if (be) {
        g_active = be;
        return 1;
    }
    return 0;
}

/* Clean-room source dispatch (game/ hand ports). Weak default: targets
 * without src/clean_table.c fall through to generated code. When linked,
 * clean_table.c's strong definition claims covered guest addresses first;
 * uncovered addresses still take the generated path below. */
__attribute__((weak))
/* clean try dispatch: (void)cpu; (void)addr; return 0; */
int clean_try_dispatch(CPUState* cpu, uint32_t addr) { (void)cpu; (void)addr; return 0; }

/* Generated-fallback observability for the timed CD/IRQ band
 * [0x80180000,0x801C0000): every dispatch that clean_try_dispatch declines
 * inside the band runs generated/untimed code. Count them and keep a small
 * addr ring so coverage audits can name the exact residual entries instead
 * of guessing. psx_dispatch_game_compiled is the single funnel — top-level
 * dispatches AND nested psx_dispatch_call paths (clean_bridge call_guest/
 * call_guest4/br2_call_clean, peer game/*.c helpers) all reach it via
 * dirty_ram_dispatch_inner, so this counter sees every in-region generated
 * execution. Read via the "clean_fallback_stats" debug command. */
#define BR2_FB_LO 0x80180000u
#define BR2_FB_HI 0x801C0000u
#define BR2_FB_RING 128
static uint64_t g_gen_fb_count;
static uint32_t g_gen_fb_ring[BR2_FB_RING];
static uint32_t g_gen_fb_pos;

/* gen fb note: if (addr >= BR2_FB_LO && addr < BR2_FB_HI)  */
static void gen_fb_note(uint32_t addr) {
    if (addr >= BR2_FB_LO && addr < BR2_FB_HI) {
        g_gen_fb_count++;
        g_gen_fb_ring[g_gen_fb_pos] = addr;
        g_gen_fb_pos = (g_gen_fb_pos + 1) & (BR2_FB_RING - 1);
    }
}
/* psx gen fallback inregion count: return g_gen_fb_count; */
uint64_t psx_gen_fallback_inregion_count(void) { return g_gen_fb_count; }
/* psx gen fallback inregion at: if (i >= BR2_FB_RING) return 0 */
uint32_t psx_gen_fallback_inregion_at(uint32_t i) {
    if (i >= BR2_FB_RING) return 0;
    return g_gen_fb_ring[(g_gen_fb_pos + BR2_FB_RING - 1 - i) & (BR2_FB_RING - 1)];
}

/* Unprefixed interface: forwarded to the selected image. Returning the
 * fail-closed default before selection guarantees no regional code can run
 * against the wrong image during early boot. */
int psx_dispatch_game_compiled(CPUState* cpu, uint32_t addr) {
    /* PSX_NO_CLEAN=1 forces every dispatch onto the generated path — an A/B
     * diagnostic to isolate whether a clean/CPS transliteration wedges boot
     * vs the runtime/hardware layer. Default off = normal clean-first order. */
    static int _no_clean = -1;
    if (_no_clean < 0) {
        const char *_s = getenv("PSX_NO_CLEAN");
        _no_clean = (_s && _s[0] == '1');
    }
    if (!_no_clean && clean_try_dispatch(cpu, addr)) return 1;
    gen_fb_note(addr);
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->dispatch_compiled(cpu, addr) : 0;
}

/* psx game is function entry: return be ? be->is_function_entry(addr) : 0 */
int psx_game_is_function_entry(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->is_function_entry(addr) : 0;
}

/* psx game text native ok: return be ? be->text_native_ok(addr) : 0 */
int psx_game_text_native_ok(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->text_native_ok(addr) : 0;
}

/* psx game text native ok full: return be ? be->text_native_ok_full(addr) : 0 */
int psx_game_text_native_ok_full(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->text_native_ok_full(addr) : 0;
}

/* psx game address in text: return be ? be->address_in_text(addr) : 0 */
int psx_game_address_in_text(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->address_in_text(addr) : 0;
}

#endif /* PSX_GAME_BACKEND_REGISTRY */
