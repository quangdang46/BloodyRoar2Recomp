#!/usr/bin/env python3
"""Generate src/clean_table.c — per-function CPUState adapters for the
clean-room source in game/. For every fnmap entry whose definition has a
parseable prototype, emit a wrapper that marshals cpu->gpr[4..7] + guest
stack args into the clean C ABI, converts pointer params via guest_at(),
and writes gpr[2]/pc on return.

Usage: python3 game/gen_clean_table.py  (run from repo root)
"""
import re, glob, sys, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.join(ROOT, 'game')
OUT = os.path.join(ROOT, 'src', 'clean_table.c')

# ---- collect fnmap entries: addr -> name ----
entries = []
for line in open(os.path.join(GAME, 'fnmap.c')):
    m = re.match(r'\s*\{\s*0x([0-9a-fA-F]+)u?,\s*\(void \*\)(\w+)\s*\}', line)
    if m:
        entries.append((int(m.group(1), 16), m.group(2)))

# ---- index definitions: name -> (ret_type, params_text, file) ----
defs = {}
files = [f for f in sorted(glob.glob(os.path.join(GAME, '*.c')))
         if not f.endswith(('bind.c', 'fnmap.c', 'selftest.c'))]
for path in files:
    src = open(path).read()
    code = re.sub(r'/\*.*?\*/', lambda m: '\n' * m.group(0).count('\n') + ' ' * (len(m.group(0)) - m.group(0).count('\n')), src, flags=re.S)
    code = re.sub(r'//[^\n]*', '', code)
    for m in re.finditer(r'([A-Za-z_][\w\s\*]*?)\b([A-Za-z_]\w*)\s*\(', code):
        name = m.group(2)
        if name in defs:
            continue
        # skip keywords / calls: must be followed by ') {' eventually
        i = m.end(); depth = 1
        while i < len(code) and depth:
            c = code[i]
            if c == '(': depth += 1
            elif c == ')': depth -= 1
            i += 1
        if depth: continue
        j = i
        while j < len(code) and code[j] in ' \t\r\n': j += 1
        if j >= len(code) or code[j] != '{': continue
        # exclude static defs (not callable cross-TU anyway, but be safe)
        k = m.start() - 1
        while k >= 0 and code[k] not in ';{}': k -= 1
        decl = re.sub(r'/\*.*?\*/', '', code[k+1:m.start()], flags=re.S)
        if re.search(r'\bstatic\b', decl): continue
        params = code[m.end():i-1]
        ret = m.group(1).strip().split()[-1] if m.group(1).strip() else ''
        # ret type may itself contain '*' e.g. 'uint8_t *' -> last token 'uint8_t', '*' lost;
        # rebuild: everything before name, keep raw
        ret_raw = code[code.rfind('\n', 0, m.start(1))+1:m.start(2)].strip()
        # strip leading storage-class words
        ret_raw = re.sub(r'^(extern|static|inline|__inline)\s+', '', ret_raw).strip()
        defs[name] = (ret_raw, params, os.path.basename(path))

def split_params(p):
    """Split param list on top-level commas."""
    out, depth, cur = [], 0, ''
    for c in p:
        if c in '([': depth += 1
        elif c in ')]': depth -= 1
        if c == ',' and depth == 0:
            out.append(cur); cur = ''
        else:
            cur += c
    if cur.strip(): out.append(cur)
    return [x.strip() for x in out]

def param_kind(pt):
    """-> ('void',) | ('ptr', ctype) | ('val', ctype) | ('skip',)"""
    if not pt or pt == 'void': return ('void',)
    if '(' in pt and '*' in pt.split('(')[0]: return ('skip',)   # fn pointer
    if '...' in pt: return ('skip',)
    is_ptr = ('*' in pt) or ('[' in pt)
    # strip trailing parameter name (last identifier, possibly with [])
    t = re.sub(r'\s+', ' ', pt).strip()
    # remove array suffix on name
    mm = re.match(r'^(.*?)\b([A-Za-z_]\w*)\s*(\[[^\]]*\])?$', t)
    if mm and mm.group(1):
        ctype = mm.group(1).strip()
        # ctype may end with '*' already included
    else:
        ctype = t
    if is_ptr:
        ctype = ctype.replace('[]', '').strip()
        if not ctype.endswith('*'): ctype += ' *'
        return ('ptr', ctype)
    return ('val', ctype if ctype else 'uint32_t')

BAD_RET = re.compile(r'\b(void|int|uint32_t|int32_t|uint16_t|int16_t|uint8_t|int8_t|'
                     r'byte|ushort|short|char|bool|undefined\d?|uint|s32|u32|s16|u16|s8|u8)\b')

def ret_kind(ret):
    r = ret.strip()
    if re.fullmatch(r'BR2_WEAK_UINT32_\d+', r): return 'u32'
    if r == 'void' or r.endswith(' void'): return 'void'
    if '*' in r: return 'ptr'
    if re.search(r'\b(undefined8|long long|int64_t|uint64_t|double|float|int128)\b', r): return 'skip'
    if BAD_RET.search(r): return 'u32'
    return 'skip'

wrappers, table, skipped = [], [], []
for addr, name in sorted(entries):
    if name not in defs:
        skipped.append((addr, name, 'no-def')); continue
    ret_raw, params, f = defs[name]
    rk = ret_kind(ret_raw)
    if rk == 'skip':
        skipped.append((addr, name, 'ret:' + ret_raw)); continue
    kinds = []
    ok = True
    for pt in split_params(params):
        k = param_kind(pt)
        if k[0] == 'skip': ok = False; break
        kinds.append(k)
    if not ok:
        skipped.append((addr, name, 'params')); continue
    if len(kinds) > 8:
        skipped.append((addr, name, 'nargs>8')); continue
    nargs = 0 if kinds == [('void',)] else len(kinds)
    # build call args
    args = []
    for i, k in enumerate(kinds):
        if k[0] == 'void': break
        raw = f'cpu->gpr[{4+i}]' if i < 4 else f'(uint32_t)cpu->read_word((cpu->gpr[29] + {16+4*(i-4)}) & 0xffffffffu)'
        if k[0] == 'ptr':
            args.append(f'(({k[1]})guest_at({raw}))')
        else:
            args.append(f'(({k[1]}){raw})')
    call = f'{name}({", ".join(args)})' if args else f'{name}()'
    wname = f'wrap_{addr:08x}'
    # extern decl: reuse verbatim signature
    sig_params = params if params.strip() else 'void'
    decl_ret = {'void':'void','u32':'uint32_t'}.get(rk, ret_raw)
    decl = f'extern {decl_ret} {name}({sig_params});'
    if rk == 'void':
        body = f'    {call};\n'
    elif rk == 'u32':
        body = f'    cpu->gpr[2] = (uint32_t)({call});\n'
    else:  # ptr return -> guest addr
        body = f'    cpu->gpr[2] = guest_of((const void *)({call}));\n'
    wrappers.append(f'static void {wname}(CPUState *cpu) {{\n{decl}\n{body}}}\n')
    table.append((addr, wname))

out = []
out.append('/* Auto-generated by game/gen_clean_table.py — clean-source CPUState\n')
out.append(' * dispatch adapters. Do not edit by hand. */\n')
out.append('#include <stdint.h>\n#include <stdbool.h>\n')
out.append('#include "cpu_state.h"\n\n')
out.append('extern void *guest_at(uint32_t addr);\n')
out.append('extern uint32_t guest_of(const void *p);\n')
out.append('extern void guest_sp_set(uint32_t sp);\n\n')
out.append('typedef uint8_t undefined1; typedef uint16_t undefined2;\n')
out.append('typedef uint32_t undefined4; typedef uint64_t undefined8;\n')
out.append('typedef unsigned int uint; typedef uint8_t byte;\n')
out.append('typedef uint16_t ushort; typedef uint8_t u8; typedef int8_t s8;\n')
out.append('typedef uint16_t u16; typedef int16_t s16; typedef uint32_t u32;\n')
out.append('typedef int32_t s32; typedef int64_t s64; typedef uint64_t u64;\n')
out.append('typedef uint8_t undefined;\n\n')
out.append(''.join(wrappers))
out.append('typedef void (*CleanWrapFn)(CPUState *cpu);\n')
out.append('typedef struct { uint32_t addr; CleanWrapFn fn; } CleanDispatchEnt;\n')
out.append('static const CleanDispatchEnt k_clean_dispatch[] = {\n')
for addr, w in table:
    out.append(f'    {{ 0x{addr:08x}u, {w} }},')
out.append('};\n\n')
out.append('')
out.append('''static const CleanDispatchEnt *clean_find(uint32_t addr) {
    uint32_t lo = 0, hi = (uint32_t)(sizeof(k_clean_dispatch) / sizeof(k_clean_dispatch[0]));
    while (lo < hi) {
        uint32_t mid = (lo + hi) >> 1;
        if (k_clean_dispatch[mid].addr < addr) lo = mid + 1; else hi = mid;
    }
    if (lo < (uint32_t)(sizeof(k_clean_dispatch) / sizeof(k_clean_dispatch[0])) &&
        k_clean_dispatch[lo].addr == addr) return &k_clean_dispatch[lo];
    return 0;
}

/* clean_try_dispatch — run the clean-room implementation for guest addr.
 * Returns 1 when handled (caller must not fall through to generated code),
 * 0 when no clean entry exists. */
static int clean_range_ok(uint32_t addr) {
    static int init = -1, lo = 0, hi = 0;
    if (init < 0) {
        const char *a = getenv("CLEAN_MIN"), *b = getenv("CLEAN_MAX");
        lo = a ? (int)strtoul(a, 0, 16) : 0;
        hi = b ? (int)strtoul(b, 0, 16) : 0x7fffffff;
        init = (a || b) ? 1 : 0;
    }
    if (!init) return 1;
    return (int)addr >= lo && (int)addr <= hi;
}

int clean_try_dispatch(CPUState *cpu, uint32_t addr) {
    const CleanDispatchEnt *e;
    if (!clean_range_ok(addr)) return 0;
    e = clean_find(addr);
    if (!e) return 0;
    extern void psx_check_interrupts_dispatch_entry(CPUState *cpu, uint32_t resume_pc);
    psx_check_interrupts_dispatch_entry(cpu, addr);
    guest_sp_set(cpu->gpr[29]);
    e->fn(cpu);
    cpu->pc = cpu->gpr[31];
    return 1;
}
''')

open(OUT, 'w').write(''.join(out))
print(f'wrote {OUT}: {len(table)} wrappers, {len(skipped)} skipped')
from collections import Counter
print(Counter(r for _,_,r in skipped).most_common(8))
