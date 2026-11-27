#!/usr/bin/env python3
"""gen_cps_clean.py — mechanically transliterate a generated CPS function
(generated/SLES_017.22_full_*.c `br2eu_func_*`/`br2eu_psx_alias_body_*`) into a
clean game/*.c coroutine body.

The generated CPS code is ALREADY the right shape: a cpu->pc continuation
switch, block_* labels + goto, psx_check_interrupts_at yield checks, and
cpu->pc=<callee>; return tail-dispatches. The "clean port" is a
source-to-source transform that:

  * keeps  cpu->gpr[]/cpu->pc/psx_check_interrupts_at/goto/case structure,
  * drops all cycle/icache/cosim/debug/PGXP instrumentation and #ifdef guards,
  * unwraps the { uint32_t _pgx* = ...; REAL_OP; PGXP_*(...); } temporaries,
  * rewrites psx_cyc_load_*/cpu->write_* to br2_ld*/br2_st*,
  * emits `void <name>(void)` with `CPUState *cpu = clean_cpu_state();`.

Usage: python3 game/gen_cps_clean.py 0x80197B30 [more addrs...]
Prints the transformed C for each addr to stdout (for inspection / paste).
"""
import re, sys, glob, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GEN = os.path.join(ROOT, 'generated')

# instrumentation / non-semantic substrings (a line containing one of these is
# dropped UNLESS it also carries a real cpu-> op we must keep)
DROP_SUBSTR = [
    'psx_cyc_step', 'psx_icache_fetch', 'cosim_', 'debug_server',
    'PGXP_', 'psx_slice_block', 'psx_cyc_bb', 'psx_store_cycle_barrier',
    'g_debug_last_store_pc', 'psx_vsync_query', 'debug_server_log_call_entry',
    # stale-static guard: clean cps_ siblings are always "native", so the
    # guarded direct-call (next line) is always the correct path.
    'psx_game_text_native_ok',
]
DROP_LINE_RE = [
    re.compile(r'^\s*#'),                 # all preprocessor directives
    re.compile(r'_psx_cyc_bb_guard'),
    re.compile(r'label compatibility'),
]

# a "real op" inside a _pgx brace: gpr assign, or a mem read/write call
REAL_OP_RE = re.compile(
    r'(cpu->gpr\[\d+\]\s*=[^;]*;'
    r'|cpu->(?:write|read)_(?:word|half|byte)\s*\([^;]*\);'
    r'|cpu->gpr\[\d+\]\s*=\s*psx_cyc_load_\w+\s*\([^;]*\);)')

# --timing mode: CD/IRQ-region faithful CPS. Keeps the per-instruction
# psx_cyc_step / psx_cyc_load_* / psx_cyc_lwc2_read / psx_icache_fetch /
# psx_slice_block calls that the default transform strips — without them a
# cps_ piece runs its whole block on a frozen cycle counter and device
# deadlines (CD IRQ, DMA, VBlank) never service inside the piece (the
# cps_fallthrough_cycle_wedge.md intra-piece timing loss). Still drops
# debug/cosim/PGXP instrumentation and the cycle-batch defer calls (charges
# then take the unbatched psx_cyc_charge -> psx_advance_cycles path, which is
# the same accounting generated code performs at every flush boundary).
TIMING = False
DROP_SUBSTR_TIMING = [d for d in DROP_SUBSTR
                      if d not in ('psx_cyc_step', 'psx_icache_fetch',
                                   'psx_slice_block')]

_LOAD_RE = re.compile(
    r'psx_cyc_load_(word|half|byte)\s*\(\s*cpu\s*,\s*(.+?)\s*,\s*\d+\s*,\s*0x[0-9a-fA-F]+u?\s*\)')
# cpu->read_word/read_half/read_byte are function pointers bound to the
# MMIO-aware psx_* accessors — correct for BOTH main RAM and 0x1F801xxx
# hardware regs. This is the faithful choice: br2_ld32 would read the wrong
# memory for MMIO (guest_at aliases 0x1F801xxx into low RAM).
_LOAD_RD = {'word': 'cpu->read_word', 'byte': 'cpu->read_byte', 'half': 'cpu->read_half'}

# MIPS unaligned word ops. psx_lwl/lwr(cpu, addr, rtval, rt, mask) merge an
# aligned word into rt; psx_swl/swr(cpu, addr, rtval) store part of rtval.
# br2_* equivalents use the MMIO-aware cpu->read/write accessors (no per-access
# cycle charge — clean_cost is the sole timing model).
_UNAL_RE = re.compile(
    r'psx_(lwl|lwr|swl|swr)\s*\(\s*cpu\s*,\s*(.+?)\s*,\s*(.+?)\s*'
    r'(?:\s*,\s*\d+\s*,\s*0x[0-9a-fA-F]+u?\s*)?\s*\)')
# split-piece fallthrough: br2eu_func_<entry>(cpu) / br2eu_psx_alias_body_<e>(cpu)
# is a synchronous call to the next CPS split piece of the same logical fn.
_FT_RE = re.compile(
    r'\b(?:br2eu_func_|br2eu_psx_alias_body_)([0-9a-fA-F]{8})\s*\(\s*cpu\s*\)')
# --timing mode: psx_lwl/lwr/swl/swr are static inline in the generated
# SLES decls header (invisible to game/*.c). Route them to file-local
# cpst_* helpers that keep the generated cycle semantics verbatim
# (psx_cyc_load_word interlock for loads, psx_store_cycle_barrier for
# stores). Arg list is captured whole — args are simple exprs, at most
# one paren nesting level.
_UNAL_T_RE = re.compile(
    r'\bpsx_(lwl|lwr|swl|swr)\s*\(\s*((?:[^()]|\([^()]*\))*)\)')
# 2-arg alias dispatch: br2eu_psx_alias_body_<base>(cpu, 0x<entry>u) tail-enters
# the shared host body at guest entry <entry> — equivalent to dispatching
# cpu->pc = <entry> through clean_try_dispatch.
_ALIAS_CALL_RE = re.compile(
    r'\bbr2eu_psx_alias_body_[0-9a-fA-F]{8}\s*\(\s*cpu\s*,\s*'
    r'0x([0-9a-fA-F]+)u?\s*\)')


# COP2 lwc2: psx_cyc_lwc2_read(cpu, addr) is a 32-bit GTE load == cpu->read_word
# (the cycle accounting is instrumentation; clean_cost is the timing model).
_LWC2_RE = re.compile(r'psx_cyc_lwc2_read\s*\(\s*cpu\s*,\s*(.+?)\s*\)')
# temp decl inside a _pgx brace group:  uint32_t _pgxa = EXPR;  /  _pgxv = ...
_PGX_DECL_RE = re.compile(r'uint32_t\s+(_pgx\w+)\s*=\s*([^;]+);')


def _sub_temps(expr, tmap):
    """Replace _pgx* identifiers in expr with their (already-resolved) values."""
    for name in sorted(tmap, key=len, reverse=True):
        expr = re.sub(r'\b' + re.escape(name) + r'\b', '(' + tmap[name] + ')', expr)
    return expr


def _conv(expr):
    """Apply load/store semantic rewrites to an expression/statement."""
    if not TIMING:
        expr = _LWC2_RE.sub(lambda m: f'cpu->read_word({m.group(1).strip()})', expr)
        expr = _sub_loads(expr)
    return expr


def unwrap_pgx_group(text):
    """Unwrap a `{ ... _pgx ... PGXP_* ... }` instrumentation group.

    The generated code declares block-scoped `uint32_t _pgxa/_pgxv = EXPR;`
    temps, runs the real op using them, then a PGXP_* probe. We record each
    temp's resolved expr (substituting earlier temps + lwc2->read_word), drop
    the decls/probes/barriers, and emit the surviving real op(s) with temps
    substituted. Handles COP2 lwc2/swc2 (cpu->gte_data / gte_write_data /
    gte_precision_store_word) which REAL_OP_RE alone would drop or leave
    undeclared."""
    tmap = {}
    # resolve temp decls in order (exprs may reference earlier temps)
    for m in _PGX_DECL_RE.finditer(text):
        name, expr = m.group(1), m.group(2).strip()
        expr = _sub_temps(expr, tmap)
        tmap[name] = _conv(expr).strip()
    # strip temp decls, PGXP_* probes, debug/barrier lines
    body = _PGX_DECL_RE.sub('', text)
    body = re.sub(r'PGXP_\w+\s*\([^;]*\)\s*;?', '', body)
    body = re.sub(r'g_debug_last_store_pc\s*=\s*[^;]+;', '', body)
    body = re.sub(r'psx_store_cycle_barrier\s*\(\s*\)\s*;?', '', body)
    if not TIMING:
        body = re.sub(r'psx_cyc_step\s*\([^;]*\)\s*;?', '', body)
    # drop stray braces / whitespace
    body = body.replace('{', ' ').replace('}', ' ')
    # keep only real statements: gpr/gte_data assign, cpu->read/write_*,
    # gte_write_data / gte_precision_store_word / gte_read_data calls, cps_ fallthrough
    keep_re = (r'^(cpu->gpr\[|cpu->gte_data\[|cpu->(?:read|write)_(?:word|half|byte)\s*\(|'
               r'gte_write_data\s*\(|gte_precision_store_word\s*\(|gte_read_data\s*\(|'
               r'cps_[0-9a-f]|br2_lw|br2_sw)')
    if TIMING:
        keep_re = keep_re[:-1] + r'|psx_|_jt_)'
    stmts = []
    for st in body.split(';'):
        st = st.strip()
        if not st:
            continue
        if re.match(keep_re, st):
            stmts.append(_sub_loads(_sub_temps(st, tmap)).strip() + ';')
    return stmts


def _sub_loads(s):
    if not TIMING:
        s = _LOAD_RE.sub(lambda m: f'{_LOAD_RD[m.group(1)]}({m.group(2).strip()})', s)
        s = _LWC2_RE.sub(lambda m: f'cpu->read_word({m.group(1).strip()})', s)
        s = _UNAL_RE.sub(lambda m: f'br2_{m.group(1)}(cpu, {m.group(2).strip()}, {m.group(3).strip()})', s)
    else:
        s = _UNAL_T_RE.sub(lambda m: f'cpst_{m.group(1)}({m.group(2).strip()})', s)
    s = _FT_RE.sub(lambda m: f'cps_{m.group(1).lower()}()', s)
    s = _ALIAS_CALL_RE.sub(lambda m: f'cpu->pc = 0x{m.group(1)}u; return;', s)
    return s


def rewrite_op(op):
    """Rewrite one real-op statement (no leading brace)."""
    op = op.strip()
    op = _sub_loads(op)
    # cpu->write_*/cpu->read_* are already MMIO-safe — keep verbatim.
    return op


def rewrite_line(ln):
    """Rewrite a non-brace semantic line; return None to drop it."""
    if not ln.strip():
        return ln
    drops = DROP_SUBSTR_TIMING if TIMING else DROP_SUBSTR
    for d in drops:
        if d in ln and 'psx_check_interrupts_at' not in ln:
            return None
    if any(r.search(ln) for r in DROP_LINE_RE):
        return None
    return _sub_loads(ln)


def find_body(addr):
    """Locate the generated function whose block-range contains `addr`.
    Returns (func_name, body_text, path). Continuations share the parent body."""
    best = None
    for path in glob.glob(os.path.join(GEN, 'SLES_017.22_full_*.c')):
        txt = open(path, errors='replace').read()
        for m in re.finditer(
                r'void (br2eu_func_[0-9a-fA-F]+|br2eu_psx_alias_body_[0-9a-fA-F]+)'
                r'\(CPUState\*\s*cpu\s*(?:,\s*uint32_t\s+\w+\s*)?\)\s*\{', txt):
            name = m.group(1)
            start = m.end()
            end = txt.find('\n}\n', start)
            if end < 0:
                continue
            body = txt[start:end]
            covers = (f'case 0x{addr:08X}u' in body or
                      f'case 0x{addr:x}u' in body or
                      f'block_{addr:08X}:' in body or
                      f'block_{addr:08x}:' in body or
                      # embedded entry: addr is an instruction inside a block
                      # (jal target mid-block — e.g. 0x8019D508 inside
                      # br2eu_func_8019D500's first block). The /* 0xADDR: ENC */
                      # op comment is the per-instruction anchor.
                      f'/* 0x{addr:08X}:' in body)
            if covers and 'block_' in body:
                if best is None or len(body) > len(best[1]):
                    best = (name, body, path)
    return best


def _truncate_embedded_entry(body, entry):
    """Emit a body variant whose FIRST block starts executing at guest
    instruction `entry` (a mid-block jal target). Only the first block can
    host an embedded entry: it is reachable solely via function dispatch,
    so dropping its leading instructions is safe — no internal goto targets
    the block head. Later blocks are branch targets and must stay whole.

    Returns the rewritten body, or None if `entry` is not an instruction
    anchor inside the first block (caller must not emit a body then — a
    plain parent transliteration would execute instructions the guest
    entry point skips)."""
    m = re.search(r'(?m)^block_[0-9A-Fa-f]+:', body)
    if not m:
        return None
    lines = body.split('\n')
    # first-block line range: [bi, b_end)
    bi = next(i for i, l in enumerate(lines) if re.match(r'^block_[0-9A-Fa-f]+:', l))
    b_end = next((i for i in range(bi + 1, len(lines))
                  if re.match(r'^block_[0-9A-Fa-f]+:', lines[i])), len(lines))
    block_label = re.match(r'block_([0-9A-Fa-f]+):', lines[bi]).group(1)
    block_addr = int(block_label, 16)
    # find the op line anchored at entry inside the first block
    pat = f'/* 0x{entry:08X}:'
    op_i = None
    for i in range(bi, b_end):
        if pat in lines[i]:
            op_i = i
            break
    if op_i is None or op_i == bi:
        return None
    # entry instr's prelude starts after the previous instr's cosim_instr
    # #endif. Walk back from the op line to the previous cosim_instr.
    cut = None
    for i in range(op_i - 1, bi - 1, -1):
        if 'cosim_instr(' in lines[i]:
            for j in range(i + 1, min(i + 6, len(lines))):
                if lines[j].strip() == '#endif':
                    cut = j + 1
                    break
            break
    if cut is None:
        return None
    # prologue = block head through the first timing fetch/step line
    # (kept so the icache fetch + slice guard still bracket the block).
    p_end = None
    for i in range(bi, cut):
        if 'psx_icache_fetch(' in lines[i] or 'psx_cyc_step(' in lines[i]:
            p_end = i + 1
            break
    if p_end is None:
        return None
    # rewrite the slice guard inside the prologue only: entry addr +
    # reduced instr budget (bcyc counts 4-byte instr slots block-start..end).
    skipped = (entry - block_addr) // 4
    prologue = []
    for l in lines[bi:p_end]:
        if 'psx_slice_block(' in l:
            l = re.sub(
                r'psx_slice_block\(cpu, 0x[0-9A-Fa-f]+u, (\d+)u,',
                lambda mm: (f'psx_slice_block(cpu, 0x{entry:08X}u, '
                            f'{max(1, int(mm.group(1)) - skipped)}u,'),
                l, count=1)
        prologue.append(l)
    kept = lines[:bi] + prologue + lines[cut:]
    body2 = '\n'.join(kept)
    # rename the truncated block label so it reads as the embedded entry
    body2 = body2.replace(f'block_{block_label}:', f'block_{entry:08X}:', 1)
    # explicit _cont case for the embedded entry (default fallthrough would
    # also reach it, but the case documents the dispatch contract).
    body2 = re.sub(r'(switch \(_cont\) \{\n)',
                   rf'\1            case 0x{entry:08X}u: goto block_{entry:08X};\n',
                   body2, count=1)
    return body2


def transform(body):
    out = []
    lines = body.split('\n')
    i, n = 0, len(lines)
    while i < n:
        ln = lines[i]
        s = ln.strip()
        # _pgx / PGXP wrapped op (spans up to ~5 lines until '}')
        if '{' in ln and ('_pgx' in ln or 'PGXP' in ln or 'g_debug' in ln
                          or 'psx_store_cycle_barrier' in ln):
            grp = [ln]
            j = i
            while '}' not in grp[-1] and j + 1 < n and j - i < 6:
                j += 1
                grp.append(lines[j])
            for st in unwrap_pgx_group('\n'.join(grp)):
                out.append('    ' + st)
            i = j + 1
            continue
        # bare PGXP continuation line (safety)
        if 'PGXP_' in ln:
            i += 1
            continue
        op = rewrite_line(ln)
        if op is not None:
            out.append(op)
        i += 1
    body = '\n'.join(out)
    # Route every split-piece / next-function fallthrough through dispatch.
    # _FT_RE emits a bare cps_X() call, which runs the whole piece chain in ONE
    # clean_try_dispatch — cost is charged once for the entry piece and cpu->pc
    # is never updated between pieces, so the per-piece cycle accounting and
    # CPS resume contract collapse and IRQ/device deadlines starve (the early
    # boot wedge). Emitting `cpu->pc = piece; return` makes each boundary a
    # real dispatch — the same tail-dispatch the generated body uses.
    body = re.sub(r'cps_([0-9a-fA-F]{8})\(\)\s*;\s*(?:return\s*;)?',
                  lambda m: f'cpu->pc = 0x{m.group(1)}u; return;', body)
    return body


FILE_HDR = '''/* cps_clean.c — AUTO-GENERATED clean CPS transliterations.
 * Regenerate:  python3 game/gen_cps_clean.py --file 0xADDR [0xADDR...]
 * Do NOT hand-edit — fix game/gen_cps_clean.py and regenerate.
 *
 * Each cps_<addr> is a faithful CPS transliteration of the generated
 * br2eu_func_* body: identical guest ops/order/yield points, minus cycle/
 * icache/cosim/debug instrumentation. Memory ops use the cpu->read_word /
 * cpu->write_word accessor family (MMIO-aware) so both main RAM and
 * 0x1F801xxx hardware regs resolve correctly. Dispatch addrs are registered
 * in game/clean_cps.map.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern int  psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);\nextern void psx_restore_state_escape(void);

/* MIPS unaligned word accessors — faithful to psx_lwl/lwr/swl/swr but on the
 * MMIO-aware cpu read/write accessor path (no per-access cycle charge). */
static inline uint32_t br2_lwl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return (rt_value & 0x00FFFFFFu) | (word << 24);
        case 1: return (rt_value & 0x0000FFFFu) | (word << 16);
        case 2: return (rt_value & 0x000000FFu) | (word << 8);
        default: return word;
    }
}
static inline uint32_t br2_lwr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t word = cpu->read_word(addr & ~3u);
    switch (addr & 3u) {
        case 0: return word;
        case 1: return (rt_value & 0xFF000000u) | (word >> 8);
        case 2: return (rt_value & 0xFFFF0000u) | (word >> 16);
        default: return (rt_value & 0xFFFFFF00u) | (word >> 24);
    }
}
static inline void br2_swl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = (word & 0xFFFFFF00u) | (rt_value >> 24); break;
        case 1: word = (word & 0xFFFF0000u) | (rt_value >> 16); break;
        case 2: word = (word & 0xFF000000u) | (rt_value >> 8); break;
        default: word = rt_value; break;
    }
    cpu->write_word(aligned, word);
}
static inline void br2_swr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = rt_value; break;
        case 1: word = (word & 0x000000FFu) | (rt_value << 8); break;
        case 2: word = (word & 0x0000FFFFu) | (rt_value << 16); break;
        default: word = (word & 0x00FFFFFFu) | (rt_value << 24); break;
    }
    cpu->write_word(aligned, word);
}

/* --timing unaligned word ops: verbatim ports of the generated
 * SLES_017.22_decls.h psx_lwl/lwr/swl/swr (static inline there, so
 * invisible to game/*.c). Loads keep the full psx_cyc_load_word
 * interlock; stores keep psx_store_cycle_barrier + read-modify-write. */
static inline uint32_t cpst_lwl(CPUState *cpu, uint32_t addr, uint32_t rt_value, uint32_t rt, uint32_t reg_mask) {
    uint32_t word = psx_cyc_load_word(cpu, addr & ~3u, rt, reg_mask);
    switch (addr & 3u) {
        case 0: return (rt_value & 0x00FFFFFFu) | (word << 24);
        case 1: return (rt_value & 0x0000FFFFu) | (word << 16);
        case 2: return (rt_value & 0x000000FFu) | (word << 8);
        default: return word;
    }
}
static inline uint32_t cpst_lwr(CPUState *cpu, uint32_t addr, uint32_t rt_value, uint32_t rt, uint32_t reg_mask) {
    uint32_t word = psx_cyc_load_word(cpu, addr & ~3u, rt, reg_mask);
    switch (addr & 3u) {
        case 0: return word;
        case 1: return (rt_value & 0xFF000000u) | (word >> 8);
        case 2: return (rt_value & 0xFFFF0000u) | (word >> 16);
        default: return (rt_value & 0xFFFFFF00u) | (word >> 24);
    }
}
static inline void cpst_swl(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    psx_store_cycle_barrier();
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = (word & 0xFFFFFF00u) | (rt_value >> 24); break;
        case 1: word = (word & 0xFFFF0000u) | (rt_value >> 16); break;
        case 2: word = (word & 0xFF000000u) | (rt_value >> 8); break;
        default: word = rt_value; break;
    }
    cpu->write_word(aligned, word);
}
static inline void cpst_swr(CPUState *cpu, uint32_t addr, uint32_t rt_value) {
    psx_store_cycle_barrier();
    uint32_t aligned = addr & ~3u;
    uint32_t word = cpu->read_word(aligned);
    switch (addr & 3u) {
        case 0: word = rt_value; break;
        case 1: word = (word & 0x000000FFu) | (rt_value << 8); break;
        case 2: word = (word & 0x0000FFFFu) | (rt_value << 16); break;
        default: word = (word & 0x00FFFFFFu) | (rt_value << 24); break;
    }
    cpu->write_word(aligned, word);
}

/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability). */
#define zero cpu->gpr[0]
#define at   cpu->gpr[1]
#define v0   cpu->gpr[2]
#define v1   cpu->gpr[3]
#define a0   cpu->gpr[4]
#define a1   cpu->gpr[5]
#define a2   cpu->gpr[6]
#define a3   cpu->gpr[7]
#define t0   cpu->gpr[8]
#define t1   cpu->gpr[9]
#define t2   cpu->gpr[10]
#define t3   cpu->gpr[11]
#define t4   cpu->gpr[12]
#define t5   cpu->gpr[13]
#define t6   cpu->gpr[14]
#define t7   cpu->gpr[15]
#define s0   cpu->gpr[16]
#define s1   cpu->gpr[17]
#define s2   cpu->gpr[18]
#define s3   cpu->gpr[19]
#define s4   cpu->gpr[20]
#define s5   cpu->gpr[21]
#define s6   cpu->gpr[22]
#define s7   cpu->gpr[23]
#define t8   cpu->gpr[24]
#define t9   cpu->gpr[25]
#define k0   cpu->gpr[26]
#define k1   cpu->gpr[27]
#define gp   cpu->gpr[28]
#define sp   cpu->gpr[29]
#define fp   cpu->gpr[30]
#define ra   cpu->gpr[31]
'''


GPR_NAMES = ['zero','at','v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4',
             't5','t6','t7','s0','s1','s2','s3','s4','s5','s6','s7','t8','t9',
             'k0','k1','gp','sp','fp','ra']
_GPR_RE = re.compile(r'cpu->gpr\[(\d+)\]')

def _cond_slug(expr):
    """Expression-derived identifier for a saved branch-condition temp."""
    e = re.sub(r'\((?:u?int(?:8|16|32|64)_t|char|short|ushort|byte|long|uint|'
               r'bool|size_t)\)', '', expr.strip())
    e = e.strip('() ')
    e = e.replace('<<', ' §L§ ').replace('>>', ' §R§ ')
    for a, b in (('&&', '_and_'), ('||', '_or_'), ('==', '_eq_'),
                 ('!=', '_ne_'), ('<=', '_le_'), ('>=', '_ge_'),
                 ('<', '_lt_'), ('>', '_gt_'), ('!', '_not_'), ('~', '_not_')):
        e = e.replace(a, b)
    e = e.replace(' §L§ ', '_shl_').replace(' §R§ ', '_shr_')
    toks = re.findall(r'[A-Za-z_]\w*|0x[0-9a-fA-F]+|\d+', e)
    s = re.sub(r'_+', '_', '_'.join(toks)).strip('_')
    s = re.sub(r'[^A-Za-z0-9_]', '', 'cond_' + s)
    return s[:56].rstrip('_') if len(s) > 56 else s


def rename_temps(body):
    """Per-function rename of generated _bc_<addr>/_jt_<addr> temps to
    readable cond_*/jr_target names. Scoped to one emitted body so shared
    guest addrs across sibling pieces never collide."""
    lines = body.split('\n')
    ren = {}
    used = set()
    # _bc_<addr> decls carry their predicate expr — slug it.
    for ln in lines:
        m = re.search(r'\b(_bc_[0-9a-fA-F]+)\b\s*=\s*(\(.*\))\s*;', ln)
        if not m or m.group(1) in ren:
            continue
        base = _cond_slug(m.group(2))
        cand, c = base, 2
        while cand in used or cand in ren.values():
            cand = f'{base}_{c}'
            c += 1
        ren[m.group(1)] = cand
        used.add(cand)
    # _jt_<addr> latches -> jr_target, jr_target_2, ...
    jt = 0
    for ln in lines:
        for m in re.finditer(r'\b(_jt_[0-9a-fA-F]+)\b', ln):
            addr = m.group(1)
            if addr in ren:
                continue
            jt += 1
            cand = 'jr_target' if jt == 1 else f'jr_target_{jt}'
            while re.search(r'\b' + re.escape(cand) + r'\b', body):
                jt += 1
                cand = f'jr_target_{jt}'
            ren[addr] = cand
    if not ren:
        return body
    for old, new in ren.items():
        body = re.sub(r'\b' + re.escape(old) + r'\b', new, body)
    return body


def _gpr_names(m):
    return GPR_NAMES[int(m.group(1))]

def emit_fn(addr):
    found = find_body(addr)
    if not found:
        print(f'/* {addr:#x}: no generated body found */', file=sys.stderr)
        return None
    name, body, path = found
    # Embedded entry: addr is not a block label or _cont case but anchors an
    # instruction inside the parent's FIRST block (mid-block jal target).
    # Truncate the first block so execution starts at that instruction —
    # emitting the untruncated parent would run instructions the guest entry
    # point skips (e.g. 0x8019D508 skips func_8019D500's lui/lw prelude).
    has_entry = (f'case 0x{addr:08X}u' in body or f'case 0x{addr:x}u' in body or
                 f'block_{addr:08X}:' in body or f'block_{addr:08x}:' in body)
    if not has_entry:
        tb = _truncate_embedded_entry(body, addr)
        if tb is None:
            print(f'/* {addr:#x}: embedded entry not in first block of '
                  f'{name} — no body emitted */', file=sys.stderr)
            return None
        body = tb
    sym = f'cpst_{addr:08x}' if TIMING else f'cps_{addr:08x}'
    core = _GPR_RE.sub(_gpr_names, transform(body))
    core = rename_temps(core)
    if 'alias_body' in name:
        # Overlapping-alias bodies take (cpu, entry): the alias-entry
        # switch dispatches on the C param, not cpu->pc. In clean CPS the
        # dispatched addr always arrives in cpu->pc, so bind entry to it
        # before the _cont consumption zeroes cpu->pc. Resume addrs are
        # still caught by the _cont switch first; alias entries fall
        # through to switch(entry) exactly as generated.
        core = '    uint32_t entry = cpu->pc;\n' + core
    hdr = (f'\n/* {sym} — clean CPS transliteration of {name} (guest {addr:#x})\n'
           f' * from {os.path.basename(path)}. */\n')
    return hdr + f'void {sym}(void)\n{{\n  CPUState *cpu = clean_cpu_state();\n  if (!cpu) return;\n' + core + '\n}\n'


if __name__ == '__main__':
    args = sys.argv[1:]
    if '--timing' in args:
        TIMING = True
        args.remove('--timing')
    if args and args[0] == '--file':
        addrs = [int(a, 16) for a in args[1:]]
        # two passes: emit prototypes for every cps_ sym first (split-piece
        # fallthroughs call siblings defined later in the file).
        fns = []
        syms = []
        for a in addrs:
            r = emit_fn(a)
            if r:
                fns.append(r)
                syms.append(f'cpst_{a:08x}' if TIMING else f'cps_{a:08x}')
        protos = ''.join(f'void {s}(void);\n' for s in syms)
        sys.stdout.write(FILE_HDR + protos + ''.join(fns))
    else:
        for a in args:
            addr = int(a, 16)
            r = emit_fn(addr)
            if r:
                print(r)
                print('=' * 60)
