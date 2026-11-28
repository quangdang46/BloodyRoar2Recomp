#!/usr/bin/env python3
"""cps_to_recon.py — convert a timing-mode cpst_ CPS body (game/cd_irq_probe.c)
into an atomic clean-C port (recon_*.c style).

Transform:
  * drop the cpu->pc continuation-switch prologue (no CPS re-entry),
  * drop psx_slice_block yield lines (precise-slice is parked; the port has
    no mid-function resume — an IRQ at a psx_check_interrupts_at resumes in
    generated code at resume_pc, exactly like the CPS coroutine did),
  * keep psx_cyc_step / psx_cyc_load_* / psx_icache_fetch / psx_check_interrupts_at
    verbatim (timing-faithful, same as the --timing CPS source),
  * rewrite `CPS jal` tail-dispatch pairs into rs_jal() + bail check +
    psx_check_interrupts_at(link) + goto block_<link>,
  * keep `CPS: jr` tail dispatches verbatim (jr $ra return / jr reg tail call
    are already correct dispatch exits).

Usage: python3 tools/cps_to_recon.py cpst_801906e8 text_win_emit_0x801906e8
"""
import re, sys, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'game', 'cd_irq_probe.c')

GPR_DEFS = '''/* MIPS GPR names — file-local aliases for cpu->gpr[N] (readability). */
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

HELPER = '''
/* rs_jal — faithful guest `jal` from clean C: dispatch the callee
 * synchronously (clean port when covered, generated body otherwise),
 * return its v0. Same contract as recon_8015.c's rs_jal / ev.c's br2_jal. */
static uint32_t rs_jal(uint32_t fn, uint32_t link)
{
    CPUState *cpu = clean_cpu_state();
    uint32_t ra, pc, sp;
    if (!cpu || fn == 0) return 0;
    if (br2_bail_pending()) return cpu->gpr[2];
    ra = cpu->gpr[31]; pc = cpu->pc; sp = cpu->gpr[29];
    cpu->gpr[31] = link;
    if (clean_try_dispatch(cpu, fn)) {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
    psx_dispatch_call(cpu, fn, link);
    cpu->gpr[29] = sp;
    {
        uint32_t r = cpu->gpr[2];
        cpu->gpr[31] = ra; cpu->pc = pc;
        return r;
    }
}
'''

DROP_SUBSTR = (
    'psx_slice_block', 'psx_cyc_bb', 'debug_server', 'cosim_',
    'PGXP_', 'psx_store_cycle_barrier', 'g_debug_last_store_pc',
    'psx_vsync_query', 'debug_server_log_call_entry',
    'psx_game_text_native_ok', 'psx_cyc_bb_defer',
)

def extract(path, fname):
    lines = open(path).read().split('\n')
    start = None
    for i, l in enumerate(lines):
        if re.match(r'^void %s\(void\)\s*$' % re.escape(fname), l):
            start = i
            break
    if start is None:
        sys.exit('def not found: ' + fname)
    # body opens at next '{'
    j = start + 1
    while lines[j].strip() != '{':
        j += 1
    depth = 0
    end = None
    for i in range(j, len(lines)):
        depth += lines[i].count('{') - lines[i].count('}')
        if depth == 0:
            end = i
            break
    return lines[start:end + 1]

def convert(fname, newname):
    body = extract(SRC, fname)
    # find prologue switch: '    if (cpu->pc != 0u) {' .. matching '    }'
    # it ends right before the '    /* Address:' comment line.
    addr_comment = next(i for i, l in enumerate(body)
                       if '/* Address:' in l)
    # prologue starts at first 'if (cpu->pc != 0u)' before addr_comment
    pstart = next(i for i, l in enumerate(body[:addr_comment])
                  if 'if (cpu->pc != 0u)' in l)
    # the switch block closes with a line '    }' just before addr_comment
    pend = addr_comment - 1
    while body[pend].strip() == '':
        pend -= 1
    assert body[pend].strip() == '}', body[pend]
    inner = body[pend + 1:addr_comment]  # nothing expected between } and comment
    assert not any(l.strip() for l in inner), inner
    blocks = body[addr_comment:]          # from /* Address: ... */ to end

    out = []
    for l in blocks:
        s = l.strip()
        if s.startswith('#'):
            continue
        if any(d in l for d in DROP_SUBSTR):
            continue
        out.append(l)
    text = '\n'.join(out)

    # ---- rewrite CPS jal tail-dispatch pairs ----
    # match:  psx_check_interrupts_at(cpu, 0xCALLEE);
    #         cpu->pc = 0xCALLEE; return;  /* CPS jal ... */
    jal_re = re.compile(
        r'(?P<indent>[ \t]*)psx_check_interrupts_at\(cpu, (?P<callee>0x[0-9A-Fa-f]+)u\);\n'
        r'(?P=indent)cpu->pc = (?P=callee)u; return;  /\* CPS jal[^\n]*')

    def find_link(prefix):
        m = None
        for mm in re.finditer(r'ra = (0x[0-9A-Fa-f]+)u;  /\* jal link before delay slot \*/',
                              prefix):
            m = mm
        if not m:
            raise RuntimeError('no jal link found before ' + prefix[-200:])
        return m.group(1)

    pieces = []
    pos = 0
    njal = 0
    for m in jal_re.finditer(text):
        callee = m.group('callee')
        link = find_link(text[:m.start()])
        lab = 'block_' + link[2:].upper()
        if ('%s:' % lab) not in text:
            raise RuntimeError('missing label ' + lab + ' for jal ' + callee)
        rep = (
            '%spsx_check_interrupts_at(cpu, %su);\n'
            '%srs_jal(%su, %su);\n'
            '%sif (br2_bail_pending()) return cpu->gpr[2];\n'
            '%spsx_check_interrupts_at(cpu, %su);\n'
            '%sgoto %s;'
        ) % (m.group('indent'), callee, m.group('indent'), callee, link,
             m.group('indent'), m.group('indent'), link, m.group('indent'), lab)
        pieces.append(text[pos:m.start()])
        pieces.append(rep)
        pos = m.end()
        njal += 1
    pieces.append(text[pos:])
    text = ''.join(pieces)

    # sanity: no leftover CPS dispatch returns other than jr
    for m in re.finditer(r'cpu->pc = [^;]+; return;[^\n]*', text):
        if 'CPS: jr' not in m.group(0) and 'CPS jal' not in m.group(0):
            print('WARN leftover dispatch:', m.group(0), file=sys.stderr)

    header = (
        'void %s(void)\n'
        '{\n'
        '  CPUState *cpu = clean_cpu_state();\n'
        '  if (!cpu) return;\n'
        '\n'
    ) % newname
    # HELPER before GPR_DEFS: rs_jal locals are named ra/pc/sp and must not
    # be macro-expanded (same ordering as recon_8015.c).
    return header + HELPER + GPR_DEFS + text + '\n'

if __name__ == '__main__':
    fname, newname = sys.argv[1], sys.argv[2]
    sys.stdout.write(convert(fname, newname))
