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
    expr = _LWC2_RE.sub(lambda m: f'cpu->read_word({m.group(1).strip()})', expr)
    return _sub_loads(expr)


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
    body = re.sub(r'psx_cyc_step\s*\([^;]*\)\s*;?', '', body)
    # drop stray braces / whitespace
    body = body.replace('{', ' ').replace('}', ' ')
    # keep only real statements: gpr/gte_data assign, cpu->read/write_*,
    # gte_write_data / gte_precision_store_word / gte_read_data calls, cps_ fallthrough
    stmts = []
    for st in body.split(';'):
        st = st.strip()
        if not st:
            continue
        if re.match(r'^(cpu->gpr\[|cpu->gte_data\[|cpu->(?:read|write)_(?:word|half|byte)\s*\(|gte_write_data\s*\(|gte_precision_store_word\s*\(|gte_read_data\s*\(|cps_[0-9a-f]|br2_lw|br2_sw)', st):
            stmts.append(_sub_loads(_sub_temps(st, tmap)).strip() + ';')
    return stmts


def _sub_loads(s):
    s = _LOAD_RE.sub(lambda m: f'{_LOAD_RD[m.group(1)]}({m.group(2).strip()})', s)
    s = _UNAL_RE.sub(lambda m: f'br2_{m.group(1)}(cpu, {m.group(2).strip()}, {m.group(3).strip()})', s)
    s = _FT_RE.sub(lambda m: f'cps_{m.group(1).lower()}()', s)
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
    for d in DROP_SUBSTR:
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
                r'\(CPUState\*\s*cpu\)\s*\{', txt):
            name = m.group(1)
            start = m.end()
            end = txt.find('\n}\n', start)
            if end < 0:
                continue
            body = txt[start:end]
            covers = (f'case 0x{addr:08X}u' in body or
                      f'case 0x{addr:x}u' in body or
                      f'block_{addr:08X}:' in body or
                      f'block_{addr:08x}:' in body)
            if covers and 'block_' in body:
                if best is None or len(body) > len(best[1]):
                    best = (name, body, path)
    return best


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
    return '\n'.join(out)


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
extern int  psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);

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
'''


def emit_fn(addr):
    found = find_body(addr)
    if not found:
        print(f'/* {addr:#x}: no generated body found */', file=sys.stderr)
        return None
    name, body, path = found
    sym = f'cps_{addr:08x}'
    core = transform(body)
    hdr = (f'\n/* {sym} — clean CPS transliteration of {name} (guest {addr:#x})\n'
           f' * from {os.path.basename(path)}. */\n')
    return hdr + f'void {sym}(void)\n{{\n  CPUState *cpu = clean_cpu_state();\n  if (!cpu) return;\n' + core + '\n}\n'


if __name__ == '__main__':
    args = sys.argv[1:]
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
                syms.append(f'cps_{a:08x}')
        protos = ''.join(f'void {s}(void);\n' for s in syms)
        sys.stdout.write(FILE_HDR + protos + ''.join(fns))
    else:
        for a in args:
            addr = int(a, 16)
            r = emit_fn(addr)
            if r:
                print(r)
                print('=' * 60)
