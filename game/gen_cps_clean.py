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


def _sub_loads(s):
    return _LOAD_RE.sub(lambda m: f'{_LOAD_RD[m.group(1)]}({m.group(2).strip()})', s)


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
        # _pgx / PGXP wrapped op (spans up to ~4 lines until '}')
        if '{' in ln and ('_pgx' in ln or 'PGXP' in ln or 'g_debug' in ln
                          or 'psx_store_cycle_barrier' in ln):
            grp = [ln]
            j = i
            while '}' not in grp[-1] and j + 1 < n and j - i < 5:
                j += 1
                grp.append(lines[j])
            text = '\n'.join(grp)
            m = REAL_OP_RE.search(text)
            if m:
                out.append('    ' + rewrite_op(m.group(1)))
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
        out = [FILE_HDR]
        for a in args[1:]:
            r = emit_fn(int(a, 16))
            if r:
                out.append(r)
        sys.stdout.write(''.join(out))
    else:
        for a in args:
            addr = int(a, 16)
            r = emit_fn(addr)
            if r:
                print(r)
                print('=' * 60)
