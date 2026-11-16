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

# ---- per-function cycle cost from the generated oracle ----
# Decode the actual MIPS words from the PS-X EXE so the clean dispatch charges
# the same per-instruction base the recompiler folds in (psx_instr_base_cycles
# == 1) plus a memory-access surcharge for guest loads/stores (charged in the
# runtime memory path, not the opcode base). A flat multiplier is wrong: the
# faithful model is instruction-count + memory-op aware, matching the
# interpreter's psx_cyc accounting closely enough that device deadlines stay
# synchronized whether a body runs clean or generated.
EXE_PATH = os.path.join(ROOT, 'disc', 'SLES_017.22')
EXE_TEXT_VADDR = 0x8011E800
EXE_TEXT_FILEOFF = 0x800
EXE_TEXT_SIZE = 0xA0800
try:
    _exe = open(EXE_PATH, 'rb').read()
    _text = _exe[EXE_TEXT_FILEOFF:EXE_TEXT_FILEOFF + EXE_TEXT_SIZE]
except OSError:
    _text = b''

def _insn_cost(word):
    """1 base cycle + memory-op surcharge, mirroring psx_instr_base_cycles +
    the load/store path charge. Returns the guest cycles for one insn."""
    op = (word >> 26) & 0x3F
    # I-type load/store opcodes (incl. unaligned LWL/LWR/SWL/SWR, coproc,
    # LL/LR, CACHE). Each guest data access costs ~+1 over the execute base.
    if op in (0x20,0x21,0x22,0x23,0x24,0x25,0x26,          # LB LH LWL LW LBU LHU LWR
              0x28,0x29,0x2A,0x2B,0x2E,                  # SB SH SWL SW SWR
              0x2F,                                       # CACHE
              0x30,0x33,                                  # LL LR
              0x31,0x32,0x35,0x36,0x39,0x3A,0x3D,0x3E):   # LWC1/2 LDC1/2 SWC1/2 SDC1/2
        return 2
    return 1

fn_size = {}
for gpath in glob.glob(os.path.join(ROOT, 'generated', '*.c')):
    for gm in re.finditer(r'Address: 0x([0-9A-Fa-f]+), Size: (\d+) bytes', open(gpath).read()):
        a = int(gm.group(1), 16)
        s = int(gm.group(2))
        if a not in fn_size or s > fn_size[a]:
            fn_size[a] = s

def clean_cost(addr):
    s = fn_size.get(addr)
    if not s:
        return 8  # tiny default for thunks/aliases with no emitted body
    # Decode the function's instructions from the EXE text for an accurate
    # instruction+memory cost; fall back to instruction-count if bytes absent.
    off = addr - EXE_TEXT_VADDR
    total = 0
    if 0 <= off < len(_text):
        end = min(off + s, len(_text))
        for i in range(off, end, 4):
            if i + 4 > len(_text):
                break
            w = int.from_bytes(_text[i:i+4], 'little')
            total += _insn_cost(w)
    if total == 0:
        total = max(4, s // 4)  # instruction-count floor (MIPS is 4-byte)
    return max(4, total)

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
        # thunk bodies (br2_call_generated -> recompiled guest code) must
        # NOT get a clean dispatch wrapper: the wrapper would re-enter the
        # thunk and recurse forever through clean_try_dispatch.
        bend = code.find('\n}', j)
        if bend > 0 and 'br2_call_generated(' in code[j:bend]:
            continue
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

# Guest addrs whose leading param is a HOST pointer even though declared as a
# scalar (intptr_t). These clean fns dereference param_1 directly as a host
# buffer (e.g. battle_flow_state_machine reads lhu at +2/+4 pad buttons), so
# the wrapper MUST translate the incoming guest address via guest_at. Without
# this, param_kind() classifies intptr_t as 'val' and emits the raw guest
# address -> the callee dereferences a guest address as a host pointer ->
# SIGSEGV / wild reads. Keyed by guest addr -> set of 0-based param indices
# to force 'ptr' (guest_at translation).
HOST_PTR_PARAMS = {
    0x8012a72c: {0},   # pad_state_refresh(intptr_t)       -> host pad buffer
    0x8014751c: {0},   # attract_menu_dispatch(intptr_t)   -> host menu ctx
    0x8016fcc4: {0},   # battle_flow_state_machine(intptr_t)-> host pad buf
}

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
    force_ptr = HOST_PTR_PARAMS.get(addr, set())
    for i, k in enumerate(kinds):
        if k[0] == 'void': break
        raw = f'cpu->gpr[{4+i}]' if i < 4 else f'(uint32_t)cpu->read_word((cpu->gpr[29] + {16+4*(i-4)}) & 0xffffffffu)'
        if k[0] == 'ptr' or i in force_ptr:
            # 'ptr' params carry a declared C type; forced host-ptr scalars
            # (intptr_t) keep their declared type for the cast but still get
            # the guest_at translation.
            ctype = k[1] if k[0] == 'ptr' else 'intptr_t'
            args.append(f'(({ctype})guest_at({raw}))')
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
    table.append((addr, wname, clean_cost(addr)))

out = []
out.append('/* Auto-generated by game/gen_clean_table.py — clean-source CPUState\n')
out.append(' * dispatch adapters. Do not edit by hand. */\n')
out.append('#include <stdint.h>\n#include <stdbool.h>\n#include <stdlib.h>\n#include <string.h>\n')
out.append('#include "cpu_state.h"\n#include "psx_cycles.h"\n\n')
out.append('extern void *guest_at(uint32_t addr);\n')
out.append('extern uint32_t guest_of(const void *p);\n')
out.append('extern void guest_sp_set(uint32_t sp);\nextern void clean_bridge_set_cpu(CPUState *cpu);\n\n')
out.append('typedef uint8_t undefined1; typedef uint16_t undefined2;\n')
out.append('typedef uint32_t undefined4; typedef uint64_t undefined8;\n')
out.append('typedef unsigned int uint; typedef uint8_t byte;\n')
out.append('typedef uint16_t ushort; typedef uint8_t u8; typedef int8_t s8;\n')
out.append('typedef uint16_t u16; typedef int16_t s16; typedef uint32_t u32;\n')
out.append('typedef int32_t s32; typedef int64_t s64; typedef uint64_t u64;\n')
out.append('typedef uint8_t undefined;\n\n')
out.append(''.join(wrappers))
out.append('typedef void (*CleanWrapFn)(CPUState *cpu);\n')
out.append('typedef struct { uint32_t addr; CleanWrapFn fn; uint32_t cost; } CleanDispatchEnt;\n')
out.append('static const CleanDispatchEnt k_clean_dispatch[] = {\n')
for addr, w, c in table:
    out.append(f'    {{ 0x{addr:08x}u, {w}, {c}u }},')
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
 * 0 when no clean entry exists.
 *
 * Selection env (bisect/debug only — both clean and generated bodies are the
 * same game logic, this only picks which implementation runs):
 *   CLEAN_RANGES="lo-hi,lo-hi,..."  hex inclusive allow-list (overrides MIN/MAX)
 *   CLEAN_EXCLUDE="lo-hi,..."       hex inclusive deny-list (always vetoes)
 *   CLEAN_MIN / CLEAN_MAX           legacy single inclusive allow range
 *   (none set)                      all clean entries enabled
 */
#define BR2_CLEAN_MAX_RANGES 48
static int g_clean_allow[BR2_CLEAN_MAX_RANGES][2];
static int g_clean_nallow = 0;
static int g_clean_excl[BR2_CLEAN_MAX_RANGES][2];
static int g_clean_nexcl = 0;

static int br2_parse_ranges(const char *env, int out[][2]) {
    int n = 0;
    if (!env || !*env) return 0;
    char buf[1024];
    size_t L = strlen(env);
    if (L >= sizeof buf) L = sizeof buf - 1;
    memcpy(buf, env, L); buf[L] = 0;
    for (char *tok = strtok(buf, ","); tok && n < BR2_CLEAN_MAX_RANGES; tok = strtok(0, ",")) {
        unsigned lo = 0, hi = 0;
        char *sep = strchr(tok, '-');
        if (!sep) sep = strchr(tok, ':');
        if (sep) { *sep = 0; lo = (unsigned)strtoul(tok, 0, 16); hi = (unsigned)strtoul(sep + 1, 0, 16); }
        else { lo = hi = (unsigned)strtoul(tok, 0, 16); }
        out[n][0] = (int)lo; out[n][1] = (int)hi; n++;
    }
    return n;
}

static int br2_in_ranges(uint32_t addr, int r[][2], int n) {
    for (int i = 0; i < n; i++)
        if ((int)addr >= r[i][0] && (int)addr <= r[i][1]) return 1;
    return 0;
}

static int clean_range_ok(uint32_t addr) {
    static int init = -1;
    if (init < 0) {
        init = 1;
        g_clean_nallow = br2_parse_ranges(getenv("CLEAN_RANGES"), g_clean_allow);
        g_clean_nexcl = br2_parse_ranges(getenv("CLEAN_EXCLUDE"), g_clean_excl);
        if (g_clean_nallow == 0) {
            const char *a = getenv("CLEAN_MIN"), *b = getenv("CLEAN_MAX");
            if (a || b) {
                int lo = a ? (int)strtoul(a, 0, 16) : 0;
                int hi = b ? (int)strtoul(b, 0, 16) : 0x7fffffff;
                g_clean_allow[0][0] = lo; g_clean_allow[0][1] = hi; g_clean_nallow = 1;
            }
        }
    }
    if (g_clean_nexcl && br2_in_ranges(addr, g_clean_excl, g_clean_nexcl)) return 0;
    if (g_clean_nallow <= 0) return 1;
    return br2_in_ranges(addr, g_clean_allow, g_clean_nallow);
}

static uint32_t g_clean_trace[512];
static uint32_t g_clean_trace_ra[512];
static uint32_t g_clean_trace_n;
uint32_t br2_dbg_clean_trace(uint32_t i) { return g_clean_trace[i & 511]; }
uint32_t br2_dbg_clean_trace_ra(uint32_t i) { return g_clean_trace_ra[i & 511]; }
uint32_t br2_dbg_clean_count(void) { return g_clean_trace_n; }

int clean_try_dispatch(CPUState *cpu, uint32_t addr) {
    const CleanDispatchEnt *e;
    if (!clean_range_ok(addr)) return 0;
    e = clean_find(addr);
    if (!e) return 0;
    g_clean_trace[g_clean_trace_n & 511] = addr;
    g_clean_trace_ra[g_clean_trace_n & 511] = cpu->gpr[31];
    g_clean_trace_n++;
    extern void psx_check_interrupts_dispatch_entry(CPUState *cpu, uint32_t resume_pc);
    psx_check_interrupts_dispatch_entry(cpu, addr);
    guest_sp_set(cpu->gpr[29]);
    clean_bridge_set_cpu(cpu);
    e->fn(cpu);
    cpu->pc = cpu->gpr[31];
    if (e->cost) psx_advance_cycles(e->cost);
    return 1;
}
''')

open(OUT, 'w').write(''.join(out))
print(f'wrote {OUT}: {len(table)} wrappers, {len(skipped)} skipped')
from collections import Counter
print(Counter(r for _,_,r in skipped).most_common(8))
