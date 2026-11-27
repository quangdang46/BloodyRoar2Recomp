#!/usr/bin/env python3
"""One-shot: transliterate the two overlapping-alias CPS bodies that own the
11 requested w3-e06 entries into game/w3_e06.c, reusing gen_cps_clean's
transform(). Not committed."""
import re, sys, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'game'))
import gen_cps_clean as G

GEN = os.path.join(ROOT, 'generated', 'SLES_017.22_full_20.c')
txt = open(GEN, errors='replace').read()

def extract(sym):
    """Pull the body of a generated alias function out of the .c source."""
    m = re.search(r'void ' + sym + r'\(CPUState\*\s*cpu,\s*uint32_t\s+entry\)\s*\{', txt)
    assert m, sym
    start = m.end()
    end = txt.find('\n}\n', start)
    assert end > 0
    return txt[start:end]

def emit(sym_host, guest_anchor, name, comment):
    """Emit one clean-C wrapper for a guest alias entry."""
    body = extract('br2eu_psx_alias_body_' + sym_host)
    core = G._GPR_RE.sub(G._gpr_names, G.transform(body))
    core = G.rename_temps(core)
    core = G.fold_idioms(core)
    core = G.fold_cps_readability(core)
    core = G.fold_setcc(core)
    core = G.name_comment_targets(core)
    core = G.fix_label_decls(core)
    core = core.replace('switch (entry)', 'switch (_sel)')
    # C23: a label immediately followed by a declaration needs a null
    # statement between them (block_*/L_*/dispatch_* CPS labels).
    core = re.sub(r'(?m)^(\s*(?:block|L_|dispatch)_\w+:)\n(\s*(?:int|uint\d+_t|int\d+_t|bool|CPUState)\b)',
                  r'\1;\n\2', core)
    out = []
    out.append(f'\n/* {name} — clean CPS transliteration of the overlapping-alias\n'
               f' * body br2eu_psx_alias_body_{sym_host} (host {guest_anchor}),\n'
               f' * {comment}\n'
               f' * from generated/SLES_017.22_full_20.c. Registered addrs live\n'
               f' * in game/clean_cps.map; continuation PCs resume via cpu->pc. */\n')
    out.append(f'void {name}(void)\n{{\n')
    out.append('  CPUState *cpu = clean_cpu_state();\n')
    out.append('  if (!cpu) return;\n')
    out.append('  uint32_t _sel = cpu->pc;\n')
    # core begins with the generated `if (cpu->pc != 0u) {...}` prologue
    out.append(core)
    out.append('}\n')
    return ''.join(out)

HDR = '''/* w3_e06.c — clean CPS ports for the 0x80157948-0x80158230 alias-entry
 * family (worktree orch-w3-e06).
 *
 * All eleven requested addrs are entries into two overlapping-alias CPS
 * bodies in generated/SLES_017.22_full_20.c, NOT standalone functions:
 *
 *   0x80157948/50/58/60 -> br2eu_psx_alias_body_8015787C  (confirm_code_select
 *                          dialog-code thunk family)
 *   0x80158000/58/70, 0x80158170/88, 0x80158228/30
 *                        -> br2eu_psx_alias_body_80157A00  (memcard_tick
 *                          dialog/state-handler family)
 *
 * Each clean body keeps the generated cpu->pc continuation switch and the
 * entry switch (driven by _sel = the dispatched addr the clean_table CPS
 * wrapper installs). Multi-addr routing lives in game/clean_cps.map. Bodies
 * were produced by the same transform as game/cps_clean.c
 * (game/gen_cps_clean.py): instrumentation stripped, cpu->read/write_*
 * MMIO-aware accessors, br2_lwX/br2_swX for unaligned ops, named GPRs.
 */
#include <stdint.h>
#include "cpu_state.h"

extern CPUState *clean_cpu_state(void);
extern void psx_check_interrupts_at(CPUState *cpu, uint32_t resume_pc);
extern void psx_muldiv_stall(CPUState *cpu);

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

parts = [HDR]
parts.append(emit('8015787C', '0x8015787C',
                  'confirm_code_entry_cps',
                  'entries are the confirm-code dialog select thunks'))
parts.append(emit('80157A00', '0x80157A00',
                  'memcard_dialog_cps',
                  'entries are memcard dialog/state handlers'))

open(os.path.join(ROOT, 'game', 'w3_e06.c'), 'w').write(''.join(parts))
print('wrote game/w3_e06.c')
