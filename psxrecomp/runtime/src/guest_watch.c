/* guest_watch.c — hardware-page-protection guest-RAM WRITE watch.
 *
 * WHY THIS EXISTS
 * ---------------
 * The recompiled code's stores go through cpu->write_word -> psx_write_word
 * -> ram[], which the wtrace rings see. The clean-C ports in game/ do NOT:
 * their store macros (DB32/MW32/BR2_W32/...) are raw
 * `*(uint32_t *)guest_at(addr) = v`, bypassing every traced write path. A
 * corrupting store issued by clean code is therefore invisible to wtrace_all,
 * the event ring, and every write-side probe. This module closes that
 * observability gap WITHOUT touching the hot path: no per-store checks are
 * added anywhere; cost is zero until a range is armed, and while armed only
 * stores landing inside the watched pages pay (two signals per store).
 *
 * MECHANISM (POSIX, page-granular)
 * -------------------------------
 *   arm:   mprotect(PROT_READ) the host pages covering the watched guest-RAM
 *          byte range. Reads still work; every WRITE — generated store,
 *          clean-C raw store, DMA memcpy, memset — faults SIGSEGV.
 *   fault: the handler records old[16] at the fault address + the faulting
 *          host instruction PC, un-protects the watched data pages, marks a
 *          pending entry, and returns — the store re-executes natively
 *          (correct size/value for ANY store encoding: str/strh/strb/stp/
 *          stxr/SIMD/atomics — no decode needed).
 *   finish: guest_watch_block_tick() — invoked from the compiled-block
 *          observe hook AND from clean_bridge's guest_at() — completes the
 *          pending entry once the store has retired: reads new[16], pushes
 *          the ring entry (host RIP for atos/nm attribution + guest
 *          store_pc/func/ra + DMA provenance + guest cycle/frame), and
 *          re-protects the data pages. arm64e W^X forbids writable+exec
 *          pages, so a BRK single-step patch is impossible on Darwin; the
 *          lazy finish is the faithful equivalent — writes landing in the
 *          (microseconds-wide) pending window merge into the pending entry,
 *          and the recorded RIP still names the first writer.
 *
 * SAFETY
 * ------
 *   * At most one pending watch fault is in flight (single emulation thread;
 *     the pending window closes at the next guest_at()/block boundary).
 *   * If no guest_at()/block boundary ever arrives (a long clean-C stretch
 *     with no memory access and no generated call), the pending window stays
 *     open and watched writes merge silently — bounded, self-healing at the
 *     next boundary; disarm also force-finishes.
 *   * Non-watch SIGSEGVs chain to the previously installed handler
 *     (crash_trace's reporter), so crash reporting is unchanged.
 *   * Windows has no POSIX signals/mprotect: the module compiles to inert
 *     stubs there (commands return an error).
 */

#if defined(__APPLE__)
#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE   /* ucontext/mcontext + SA_SIGINFO on Darwin */
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700  /* ucontext.h gate on modern SDKs */
#endif
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* Fixed-4-byte-instruction ISAs only: the PC+4 post-store BRK patch assumes
 * uniform 4-byte instructions (A64). x86_64 would need real decoding — keep
 * the module inert there. */
#if (defined(__APPLE__) || defined(__linux__)) && \
    (defined(__aarch64__) || defined(__arm64__))
#define GW_POSIX 1
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ucontext.h>
#endif

/* ---- externs (runtime) ---- */
extern uint8_t *memory_get_ram_ptr(void);
extern uint8_t *memory_get_scratchpad_ptr(void);
extern uint32_t g_debug_last_store_pc;
extern uint32_t g_debug_current_func_addr;
extern int      g_dma_exec_depth;
extern int      g_dma_cur_ch;
extern uint32_t g_dma_cur_madr;
extern uint32_t g_dma_cur_bcr;
extern uint32_t g_dma_initiator_pc;
extern uint64_t s_frame_count;
extern uint64_t psx_get_cycle_count(void);
extern void   *debug_cpu_ptr; /* CPUState* — gpr[] layout per cpu_state.h */

#define GW_RAM_SIZE   (2u * 1024u * 1024u)
#define GW_MAX_RANGES 8
#define GW_RING_CAP   (1u << 15)
#define GW_WINDOW     16

/* Scratchpad region: guest phys 0x1F800000..0x1F8003FF maps to
 * memory_get_scratchpad_ptr()[0..1023]. Clean-C stores reach it through raw
 * guest_at() pointers exactly like RAM, so it needs the same watch coverage —
 * this is where the shared per-frame phase byte (DAT_1f800001) lives. */
#define GW_SCRATCH_PHYS  0x1F800000u
#define GW_SCRATCH_GSIZE 0x400u
#define GW_SCRATCH_HSIZE 0x4000u  /* page-aligned SCRATCHPAD_ALLOC in memory.c */

#ifdef GW_POSIX

/* CPUState gpr offsets we read for provenance (MIPS ABI: 4=a0 5=a1 29=sp
 * 31=ra). Only valid when debug_cpu_ptr is bound; stale during clean-C
 * execution but still recorded — host RIP is the authoritative attribution. */
#define GW_GPR_A0 4
#define GW_GPR_A1 5
#define GW_GPR_SP 29
#define GW_GPR_RA 31

typedef struct {
    uint64_t seq;
    uint32_t guest_addr;   /* KSEG0 view: 0x80000000 | phys */
    uint32_t phys;
    uint64_t rip;          /* host PC of the faulting store instruction */
    uint32_t store_pc;     /* g_debug_last_store_pc (guest, generated path) */
    uint32_t func_addr;    /* g_debug_current_func_addr */
    uint32_t ra, sp, a0, a1;
    uint32_t s0, s1, s2, s3, s4, s5;
    uint32_t frame;
    int32_t  dma_depth, dma_ch;
    uint32_t dma_madr, dma_bcr, dma_init_pc;
    uint64_t cycle;
    uint64_t host_fp, host_lr, host_sp; /* host frame regs at fault (arm64) */
    uint64_t ra_chain[10];  /* unwound host return addrs at fault time */
    uint32_t ra_count;
    uint8_t  in_range;     /* fault inside watched byte range (vs page spill) */
    uint8_t  pad[7];
    uint8_t  oldv[GW_WINDOW];
    uint8_t  newv[GW_WINDOW];
} GwEntry;

static GwEntry  s_ring[GW_RING_CAP];
static uint64_t s_seq       = 0;
static uint32_t s_head      = 0;
static uint64_t s_total     = 0;   /* logged in-range hits */
static uint64_t s_oor_hits  = 0;   /* faults in protected pages, outside range */
static uint64_t s_dropped   = 0;

/* Armed ranges (guest phys offsets into main RAM). */
typedef struct { uint32_t lo, hi; } GwRange;
static GwRange s_ranges[GW_MAX_RANGES];
static int     s_range_count = 0;
static int     s_armed = 0;

/* Host pages we have mprotected PROT_READ. */
static uintptr_t s_pages[256];
static int       s_page_count = 0;
static size_t    s_page_size = 0;

/* Pending single fault: store insn at s_pend_insn already executed once the
 * (lazy finish — see header). */
volatile int g_guest_watch_pending = 0;
static uintptr_t         s_pend_host = 0;   /* fault address (host) */
static uint64_t          s_pend_rip  = 0;
static uint64_t          s_pend_fp = 0, s_pend_lr = 0, s_pend_sp = 0;
static uint64_t          s_pend_ra[10];
static uint32_t          s_pend_ra_count = 0;
static uint8_t           s_pend_old[GW_WINDOW];
static int               s_pend_in_range = 0;
static uintptr_t         s_pend_brk  = 0;   /* unused (no code patching) */
static int               s_handlers_installed = 0;
static struct sigaction  s_old_segv, s_old_trap, s_old_bus;
/* Diagnostics: last fault/trap PCs and patch failures, surfaced in dumps. */
static uint64_t s_last_segv_pc = 0, s_last_segv_addr = 0;
static uint64_t s_last_trap_pc = 0;
static uint64_t s_patch_fail   = 0;

static int gw_is_scratch(uint32_t phys)
{ return phys >= GW_SCRATCH_PHYS && phys < GW_SCRATCH_PHYS + GW_SCRATCH_GSIZE; }

static int gw_phys_ok(uint32_t phys)
{ return phys < GW_RAM_SIZE || gw_is_scratch(phys); }

static int gw_addr_in_ranges(uint32_t phys)
{
    for (int i = 0; i < s_range_count; i++)
        if (phys >= s_ranges[i].lo && phys < s_ranges[i].hi) return 1;
    return 0;
}

/* phys -> host pointer into main RAM or scratchpad (caller guarantees
 * gw_phys_ok). */
static inline uint8_t *gw_host(uint32_t phys)
{
    if (gw_is_scratch(phys))
        return memory_get_scratchpad_ptr() + (phys - GW_SCRATCH_PHYS);
    return memory_get_ram_ptr() + phys;
}

/* host fault/watched address -> guest phys. Returns 1 and sets *out when the
 * address lands inside a watched region's host array, else 0. */
static int gw_phys_of_host(uintptr_t fa, uint32_t *out)
{
    uint8_t *sp  = memory_get_scratchpad_ptr();
    uint8_t *ram = memory_get_ram_ptr();
    if (fa >= (uintptr_t)sp && fa < (uintptr_t)sp + GW_SCRATCH_HSIZE) {
        *out = GW_SCRATCH_PHYS + (uint32_t)(fa - (uintptr_t)sp);
        return 1;
    }
    if (fa >= (uintptr_t)ram && fa < (uintptr_t)ram + GW_RAM_SIZE + 256) {
        *out = (uint32_t)(fa - (uintptr_t)ram);
        return 1;
    }
    return 0;
}

/* Bytes readable from host pointer h to the end of its containing region's
 * host array — used to bound the 16B old/new windows without crossing the
 * array edge (scratchpad is only 0x500 host bytes). */
static size_t gw_host_avail(uintptr_t h)
{
    uint8_t *sp  = memory_get_scratchpad_ptr();
    uint8_t *ram = memory_get_ram_ptr();
    if (h >= (uintptr_t)sp && h < (uintptr_t)sp + GW_SCRATCH_HSIZE)
        return (size_t)((uintptr_t)sp + GW_SCRATCH_HSIZE - h);
    if (h >= (uintptr_t)ram && h < (uintptr_t)ram + GW_RAM_SIZE + 256)
        return (size_t)((uintptr_t)ram + GW_RAM_SIZE + 256 - h);
    return 0;
}

static void gw_protect_pages(int prot)
{
    for (int i = 0; i < s_page_count; i++)
        mprotect((void *)s_pages[i], s_page_size, prot);
}

static void gw_push_entry(void)
{
    if (s_total >= GW_RING_CAP) { s_dropped++; return; }
    GwEntry *e = &s_ring[s_head];
    s_head = (s_head + 1) % GW_RING_CAP;
    memset(e, 0, sizeof(*e));
    e->seq        = s_seq++;
    {
        uint32_t phys = 0;
        gw_phys_of_host((uintptr_t)s_pend_host, &phys);
        e->phys = phys;
    }
    /* RAM phys -> KSEG0 0x8xxxxxxx; scratchpad phys is already the KSEG1
     * 0x1F800xxx guest address the code uses. */
    e->guest_addr = gw_is_scratch(e->phys) ? e->phys : (0x80000000u | e->phys);
    e->rip        = s_pend_rip;
    e->store_pc   = g_debug_last_store_pc;
    e->func_addr  = g_debug_current_func_addr;
    e->frame      = (uint32_t)s_frame_count;
    e->dma_depth  = g_dma_exec_depth;
    e->dma_ch     = g_dma_cur_ch;
    e->dma_madr   = g_dma_cur_madr;
    e->dma_bcr    = g_dma_cur_bcr;
    e->dma_init_pc= g_dma_initiator_pc;
    e->cycle      = psx_get_cycle_count();
    e->host_fp    = s_pend_fp;
    e->host_lr    = s_pend_lr;
    e->host_sp    = s_pend_sp;
    e->ra_count   = s_pend_ra_count;
    memcpy(e->ra_chain, s_pend_ra, sizeof(e->ra_chain));
    e->in_range   = (uint8_t)s_pend_in_range;
    if (debug_cpu_ptr) {
        uint32_t *gpr = (uint32_t *)debug_cpu_ptr;
        /* CPUState begins with gpr[32] on every backend layout used here —
         * verify against cpu_state.h before relying on indices beyond. */
        e->a0 = gpr[GW_GPR_A0]; e->a1 = gpr[GW_GPR_A1];
        e->sp = gpr[GW_GPR_SP]; e->ra = gpr[GW_GPR_RA];
        e->s0 = gpr[16]; e->s1 = gpr[17]; e->s2 = gpr[18];
        e->s3 = gpr[19]; e->s4 = gpr[20]; e->s5 = gpr[21];
    }
    memcpy(e->oldv, s_pend_old, GW_WINDOW);
    /* newv filled by caller (trap or recover) before/after push — instead we
     * fill it here from the host window: at trap time the store has run. */
    {
        uint8_t *h = (uint8_t *)s_pend_host;
        size_t avail = gw_host_avail((uintptr_t)h);
        size_t n = avail < GW_WINDOW ? avail : GW_WINDOW;
        memcpy(e->newv, h, n);
        if (n < GW_WINDOW) memset(e->newv + n, 0, GW_WINDOW - n);
    }
    s_total++;
}

/* Complete a pending watch fault: read new[], push entry if in range, restore
 * the patched instruction, re-protect data pages, clear pending. */
static void gw_finish_pending(void)
{
    if (!g_guest_watch_pending) return;
    if (s_pend_in_range) gw_push_entry();
    else s_oor_hits++;
    s_pend_brk = 0;
    g_guest_watch_pending = 0;
    gw_protect_pages(PROT_READ);
}

/* Force-recover a stuck pending (patched site never executed — e.g. PC+4 was
 * literal-pool data). Safe to call any time; no-op when nothing pending. */
void guest_watch_block_tick(void)
{
    if (!g_guest_watch_pending) return;
    gw_finish_pending();
}

static void gw_segv_handler(int sig, siginfo_t *si, void *uc_v)
{
    ucontext_t *uc = (ucontext_t *)uc_v;
#if defined(__APPLE__)
    uint64_t pc = (uint64_t)uc->uc_mcontext->__ss.__pc;
#else /* Linux arm64 */
    uint64_t pc = (uint64_t)uc->uc_mcontext.pc;
#endif
    uint8_t *ram = memory_get_ram_ptr();
    uint8_t *sp  = memory_get_scratchpad_ptr();
    uintptr_t fa = (uintptr_t)si->si_addr;

    s_last_segv_pc = pc; s_last_segv_addr = (uint64_t)fa;
    int ours = 0;
    int in_region =
        (fa >= (uintptr_t)ram && fa < (uintptr_t)ram + GW_RAM_SIZE + 256) ||
        (fa >= (uintptr_t)sp  && fa < (uintptr_t)sp  + GW_SCRATCH_HSIZE);
    if (s_armed && in_region) {
        /* Only treat as ours if the fault page is one we protected. The byte
         * range check decides in_range vs spill. */
        uintptr_t pg = fa & ~(uintptr_t)(s_page_size - 1);
        for (int i = 0; i < s_page_count; i++)
            if (s_pages[i] == pg) { ours = 1; break; }
    }
    if (!ours || g_guest_watch_pending) {
        /* Not ours (or unexpected reentry): chain to previous handler.
         * NOTE: sa_handler and sa_sigaction share a union — only call the
         * sigaction form when the old handler was installed with SA_SIGINFO
         * (crash_trace uses signal(), i.e. the 1-arg form). */
        struct sigaction *old = (sig == SIGBUS) ? &s_old_bus : &s_old_segv;
        if ((old->sa_flags & SA_SIGINFO) && old->sa_sigaction)
            old->sa_sigaction(sig, si, uc_v);
        else if (old->sa_handler && old->sa_handler != SIG_DFL &&
                 old->sa_handler != SIG_IGN)
            ((void (*)(int))old->sa_handler)(sig);
        else { signal(sig, SIG_DFL); raise(sig); }
        return;
    }

    /* Record the pre-store window. */
    uint32_t phys = 0;
    gw_phys_of_host(fa, &phys);
    s_pend_host = fa;
    s_pend_rip  = pc;
#if defined(__APPLE__) && defined(__aarch64__)
    s_pend_fp = (uint64_t)uc->uc_mcontext->__ss.__x[29];
    s_pend_lr = (uint64_t)uc->uc_mcontext->__ss.__x[30];
    s_pend_sp = (uint64_t)uc->uc_mcontext->__ss.__sp;
#elif defined(__linux__) && defined(__aarch64__)
    s_pend_fp = (uint64_t)uc->uc_mcontext.regs[29];
    s_pend_lr = (uint64_t)uc->uc_mcontext.regs[30];
    s_pend_sp = (uint64_t)uc->uc_mcontext.sp;
#endif
    /* Unwind the host frame-pointer chain AT fault time so the caller survives
     * later stack reuse. Each arm64 frame record is {prev_fp @+0, lr @+8};
     * frames sit at higher addresses than sp. Bound every hop to the plausible
     * stack window above sp so a corrupt fp can't fault inside the handler. */
    s_pend_ra_count = 0;
#if defined(__aarch64__)
    {
        uint64_t sp = s_pend_sp, f = s_pend_fp;
        uint64_t hi = sp + 0x1000000ull;   /* 16MB stack ceiling */
        for (int i = 0; i < 10; i++) {
            if (f < sp || f >= hi || (f & 0xFULL)) break;
            uint64_t prev = *(volatile uint64_t *)f;
            uint64_t lr   = *(volatile uint64_t *)(f + 8);
            s_pend_ra[i] = lr;
            s_pend_ra_count++;
            if (prev <= f || prev >= hi || (prev & 0xFULL)) break;
            f = prev;
        }
    }
#endif
    s_pend_in_range = gw_addr_in_ranges(phys) ? 1 : 0;
    {
        size_t avail = gw_host_avail(fa);
        size_t n = avail < GW_WINDOW ? avail : GW_WINDOW;
        memcpy(s_pend_old, (void *)fa, n);
        if (n < GW_WINDOW) memset(s_pend_old + n, 0, GW_WINDOW - n);
    }

    /* arm64e W^X forbids writable+executable pages, so the post-store BRK
     * patch is impossible on Darwin. Instead: unprotect the watched pages,
     * let the store retire natively, and finish the entry at the next
     * guest_at()/compiled-block boundary (guest_watch_block_tick). Writes to
     * the watched range in that tiny window merge into this entry — the
     * recorded RIP still names the FIRST writer, which is what provenance
     * questions need. */
    s_pend_brk = 0;
    g_guest_watch_pending = 1;
    gw_protect_pages(PROT_READ | PROT_WRITE);
    /* Return: kernel re-executes the store at PC against writable pages. */
}

static void gw_trap_handler(int sig, siginfo_t *si, void *uc_v)
{
    (void)sig; (void)si;
    ucontext_t *uc = (ucontext_t *)uc_v;
#if defined(__APPLE__)
    uint64_t pc = (uint64_t)uc->uc_mcontext->__ss.__pc;
#else /* Linux arm64 */
    uint64_t pc = (uint64_t)uc->uc_mcontext.pc;
#endif
    s_last_trap_pc = pc;
    /* No BRK patch sites exist (W^X) — any SIGTRAP is foreign; chain on. */
    if ((s_old_trap.sa_flags & SA_SIGINFO) && s_old_trap.sa_sigaction)
        s_old_trap.sa_sigaction(sig, si, uc_v);
    else if (s_old_trap.sa_handler && s_old_trap.sa_handler != SIG_DFL &&
             s_old_trap.sa_handler != SIG_IGN)
        ((void (*)(int))s_old_trap.sa_handler)(sig);
    else { signal(sig, SIG_DFL); raise(sig); }
}

static void gw_install_handlers(void)
{
    if (s_handlers_installed) return;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = gw_segv_handler;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, &s_old_segv);
    sigaction(SIGBUS, &sa, &s_old_bus);   /* Darwin delivers mprotect write
                                           * violations as SIGBUS, not SIGSEGV */
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = gw_trap_handler;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTRAP, &sa, &s_old_trap);
    s_handlers_installed = 1;
    s_page_size = (size_t)sysconf(_SC_PAGESIZE);
    if (s_page_size == 0) s_page_size = 4096;
}

int guest_watch_add(uint32_t guest_addr, uint32_t len)
{
    if (!s_armed) {
        if (s_range_count != 0) return -2; /* disarm first */
        gw_install_handlers();
        s_page_size = (size_t)sysconf(_SC_PAGESIZE);
        if (!s_page_size) s_page_size = 4096;
    }
    /* guest_addr may be KSEG0 (0x8xxxxxxx) or phys; fold mirrors like
     * psx_phys_addr for the RAM region. */
    uint32_t phys = guest_addr & 0x1FFFFFFFu;
    if (phys < 0x00800000u) phys &= (GW_RAM_SIZE - 1u);
    if (!gw_phys_ok(phys) || len == 0) return -1;
    if (gw_is_scratch(phys)) {
        uint32_t stop = GW_SCRATCH_PHYS + GW_SCRATCH_GSIZE;
        if (phys + len > stop) len = stop - phys;
        if (len == 0) return -1;
    } else {
        if (phys + len > GW_RAM_SIZE + 256) return -1;
        if (phys + len > GW_RAM_SIZE) len = GW_RAM_SIZE - phys;
    }
    if (s_range_count >= GW_MAX_RANGES) return -3;

    s_ranges[s_range_count].lo = phys;
    s_ranges[s_range_count].hi = phys + len;
    s_range_count++;

    /* (Re)compute the protected page set from all ranges. Pages are taken in
     * HOST space via gw_host so a scratchpad range protects the scratchpad's
     * own pages, not ram[]'s. */
    s_page_count = 0;
    for (int r = 0; r < s_range_count; r++) {
        uintptr_t lo  = (uintptr_t)gw_host(s_ranges[r].lo)
                        & ~(uintptr_t)(s_page_size - 1);
        uintptr_t last = (uintptr_t)gw_host(s_ranges[r].hi - 1);
        uintptr_t hi  = (last & ~(uintptr_t)(s_page_size - 1)) + s_page_size;
        for (uintptr_t p = lo; p < hi && s_page_count < 256; p += s_page_size) {
            int dup = 0;
            for (int i = 0; i < s_page_count; i++)
                if (s_pages[i] == p) { dup = 1; break; }
            if (!dup) s_pages[s_page_count++] = p;
        }
    }
    gw_protect_pages(PROT_READ);
    s_armed = 1;
    return 0;
}

void guest_watch_disarm(void)
{
    if (g_guest_watch_pending) gw_finish_pending();
    if (s_page_count) gw_protect_pages(PROT_READ | PROT_WRITE);
    s_page_count = 0;
    s_range_count = 0;
    s_armed = 0;
}

void guest_watch_reset_ring(void)
{
    s_seq = 0; s_head = 0; s_total = 0; s_oor_hits = 0; s_dropped = 0;
    memset(s_ring, 0, sizeof(s_ring));
}

/* Serialize one entry as a JSON object body (no leading comma). */
static void gw_entry_json(char *buf, size_t cap, const GwEntry *e)
{
    char oldh[GW_WINDOW * 2 + 1], newh[GW_WINDOW * 2 + 1];
    char rah[10 * 20 + 2];
    size_t rp = 0;
    rah[rp++] = '[';
    for (uint32_t i = 0; i < e->ra_count && i < 10; i++)
        rp += (size_t)snprintf(rah + rp, sizeof(rah) - rp, "%s\"0x%llX\"",
                               i ? "," : "", (unsigned long long)e->ra_chain[i]);
    rah[rp++] = ']'; rah[rp] = '\0';
    for (int i = 0; i < GW_WINDOW; i++) {
        sprintf(oldh + i * 2, "%02x", e->oldv[i]);
        sprintf(newh + i * 2, "%02x", e->newv[i]);
    }
    snprintf(buf, cap,
        "{\"seq\":%llu,\"addr\":\"0x%08X\",\"phys\":\"0x%08X\","
        "\"rip\":\"0x%016llX\",\"store_pc\":\"0x%08X\",\"func\":\"0x%08X\","
        "\"ra\":\"0x%08X\",\"sp\":\"0x%08X\",\"a0\":\"0x%08X\",\"a1\":\"0x%08X\","
        "\"s0\":\"0x%08X\",\"s1\":\"0x%08X\",\"s2\":\"0x%08X\","
        "\"s3\":\"0x%08X\",\"s4\":\"0x%08X\",\"s5\":\"0x%08X\","
        "\"frame\":%u,\"cyc\":%llu,"
        "\"hfp\":\"0x%016llX\",\"hlr\":\"0x%016llX\",\"hsp\":\"0x%016llX\","
        "\"ra\":%s,"
        "\"dma\":{\"depth\":%d,\"ch\":%d,\"madr\":\"0x%08X\",\"bcr\":\"0x%08X\","
        "\"init_pc\":\"0x%08X\"},\"in_range\":%u,"
        "\"old\":\"%s\",\"new\":\"%s\"}",
        (unsigned long long)e->seq, e->guest_addr, e->phys,
        (unsigned long long)e->rip, e->store_pc, e->func_addr,
        e->ra, e->sp, e->a0, e->a1,
        e->s0, e->s1, e->s2, e->s3, e->s4, e->s5,
        e->frame, (unsigned long long)e->cycle,
        (unsigned long long)e->host_fp, (unsigned long long)e->host_lr,
        (unsigned long long)e->host_sp, rah,
        e->dma_depth, e->dma_ch, e->dma_madr, e->dma_bcr, e->dma_init_pc,
        (unsigned)e->in_range, oldh, newh);
}

uint64_t guest_watch_total(void)    { return s_total; }
uint64_t guest_watch_oor(void)      { return s_oor_hits; }
uint64_t guest_watch_dropped(void)  { return s_dropped; }
int      guest_watch_armed(void)    { return s_armed; }
int      guest_watch_pending(void)  { return g_guest_watch_pending; }
void     guest_watch_diag(uint64_t *segv_pc, uint64_t *segv_addr,
                          uint64_t *trap_pc, uint64_t *patch_fail)
{
    *segv_pc = s_last_segv_pc; *segv_addr = s_last_segv_addr;
    *trap_pc = s_last_trap_pc; *patch_fail = s_patch_fail;
}
int      guest_watch_range_count(void) { return s_range_count; }
void     guest_watch_range(int i, uint32_t *lo, uint32_t *hi)
{
    if (i >= 0 && i < s_range_count) { *lo = s_ranges[i].lo; *hi = s_ranges[i].hi; }
    else { *lo = *hi = 0; }
}

/* Emit up to `max` entries (oldest-first from ring) into buf; returns bytes
 * written. Caller formats the JSON envelope. */
int guest_watch_dump(char *buf, size_t cap, int max, int newest_first)
{
    if (!s_armed && s_total == 0) return snprintf(buf, cap, "[]");
    uint64_t avail = s_total < GW_RING_CAP ? s_total : GW_RING_CAP;
    uint64_t oldest = s_total - avail;
    int n = 0, first = 1;
    size_t pos = 0;
    pos += snprintf(buf + pos, cap - pos, "[");
    for (int64_t i = 0; i < avail && n < max; i++) {
        uint64_t seq;
        if (newest_first) seq = s_total - 1 - (uint64_t)i;
        else              seq = oldest + (uint64_t)i;
        /* ring slot for seq: entries are contiguous modulo cap */
        uint64_t idx = seq % GW_RING_CAP;
        /* The ring head advances only on push; slot = seq % CAP works
         * because seq starts at 0 and pushes are sequential. */
        GwEntry *e = &s_ring[idx];
        if (e->seq != seq) continue; /* stale/empty slot */
        char tmp[1408];
        gw_entry_json(tmp, sizeof(tmp), e);
        pos += snprintf(buf + pos, cap - pos, "%s%s", first ? "" : ",", tmp);
        first = 0; n++;
        if (pos > cap - 1500) break;
    }
    pos += snprintf(buf + pos, cap - pos, "]");
    return (int)pos;
}

#else /* !_WIN32 POSIX — Windows stub */

int  guest_watch_add(uint32_t a, uint32_t l) { (void)a; (void)l; return -99; }
void guest_watch_disarm(void) {}
void guest_watch_reset_ring(void) {}
void guest_watch_block_tick(void) {}
uint64_t guest_watch_total(void) { return 0; }
uint64_t guest_watch_oor(void) { return 0; }
uint64_t guest_watch_dropped(void) { return 0; }
int  guest_watch_armed(void) { return 0; }
int  guest_watch_pending(void) { return 0; }
int  guest_watch_range_count(void) { return 0; }
void guest_watch_range(int i, uint32_t *lo, uint32_t *hi) { (void)i; *lo=*hi=0; }
int  guest_watch_dump(char *buf, size_t cap, int max, int nf) {
    (void)max; (void)nf; return snprintf(buf, cap, "[]");
}
void guest_watch_diag(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d) {
    *a=*b=*c=*d=0;
}

#endif /* GW_POSIX */
