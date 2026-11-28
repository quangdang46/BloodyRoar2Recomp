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

# Multiple fnmap names can alias one guest addr (e.g. a clean impl plus its
# _guest ABI entry). Emit ONE wrapper per addr, preferring the renames.map
# canonical name when present.
_preferred = {}
try:
    for line in open(os.path.join(GAME, 'renames.map')):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        a, n = line.split(None, 1)
        _preferred[int(a, 16)] = n
except FileNotFoundError:
    pass
_by_addr = {}
for addr, name in entries:
    cur = _by_addr.get(addr)
    if cur is None or (_preferred.get(addr) == name and cur[1] != name):
        _by_addr[addr] = (addr, name)
# Clean-CPS re-entry addrs: coroutine continuation/branch-target PCs that are
# NOT function entries (so they have no fnmap row) but must still route to a
# clean body so a yielded CD/IRQ coroutine resumes inside clean C. Format is
# "addr name" like renames.map; the named symbol must be defined in game/*.c.
# Multiple addrs may name the same symbol — each emits its own wrap_<addr>
# and clean_try_dispatch hands the fn cpu->pc == the dispatched addr so the
# body's pc-switch picks the right continuation block.
cps_addrs = set()
try:
    for line in open(os.path.join(GAME, 'clean_cps.map')):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        a, n = line.split(None, 1)
        _by_addr[int(a, 16)] = (int(a, 16), n)
        cps_addrs.add(int(a, 16))
except FileNotFoundError:
    pass
entries = sorted(_by_addr.values())

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

def clean_frame(addr):
    """Guest stack-frame bytes the MIPS prologue reserves for this function.

    Clean bodies run on the HOST stack and never touch cpu->gpr[29], so every
    clean ancestor's frame is missing from the guest sp seen by generated
    callees — our sp runs shallower than HW by the cumulative clean-frame
    deficit, which relocates every spill's landing address (this shifted a
    session-pointer spill onto the dispatched device-command rec+0x14 during
    CD file-loads -> wild dispatch). Decode the function's first MIPS insn:
    a leading `addiu sp,sp,-N` (0x27BDxxxx, imm>=0x8000) is a static frame of
    N bytes; anything else (leaf, dynamic alloc, non-standard prologue) -> 0
    so behavior is unchanged for those."""
    off = addr - EXE_TEXT_VADDR
    if not (0 <= off and off + 4 <= len(_text)):
        return 0
    w = int.from_bytes(_text[off:off + 4], 'little')
    if (w & 0xFFFF0000) != 0x27BD0000:
        return 0
    imm = w & 0xFFFF
    if imm < 0x8000:
        return 0
    return 0x10000 - imm

def clean_cost(addr):
    """Estimated clean-C cycle cost for a guest function (instruction+memory)."""
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

# ---- index definitions: scan game/*.c for non-static prototypes ----
defs = {}
files = [f for f in sorted(glob.glob(os.path.join(GAME, '*.c')))
         if not f.endswith(('bind.c', 'fnmap.c', 'selftest.c'))]
for path in files:
    src = open(path).read()
    code = re.sub(r'/\*.*?\*/', lambda m: '\n' * m.group(0).count('\n') + ' ' * (len(m.group(0)) - m.group(0).count('\n')), src, flags=re.S)
    code = re.sub(r'//[^\n]*', '', code)
    # Blank preprocessor-directive lines (#undef/#define/#if/...): their text
    # (e.g. "undef local_39") is not a C prototype and otherwise leaks into the
    # preceding function's return-type capture, corrupting the emitted extern.
    code = re.sub(r'(?m)^[ \t]*#[^\n]*', '', code)
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
        # NOT get a clean dispatch wrapper when they delegate to THEMSELVES
        # (wrap -> thunk -> br2_call_generated(self) -> clean_try_dispatch
        # -> wrap = infinite recursion) or through an INDIRECT target that
        # cannot be proven non-self. A LITERAL delegate to a DIFFERENT guest
        # addr is safe: the callee's own dispatch decides clean-vs-generated,
        # and a leaf generated callee cannot route back here. The guest addr
        # is not known at def-scan time, so record each call's first arg and
        # let the emit loop (which knows addr) apply the self/indirect check.
        bend = code.find('\n}', j)
        gen_args = []
        call_args = []
        if bend > 0:
            _body = code[j:bend]
            gen_args = [gm.group(1).strip() for gm in
                        re.finditer(r'br2_call_generated\(\s*([^,]+)',
                                    _body)]
            # Self-delegation is not limited to br2_call_generated: the CD/IRQ
            # boundary ports forward their whole body through call_guest /
            # call_guest4 / br2_call_clean / psx_dispatch_call with their OWN
            # guest addr as the target (a §4 delegate). Record every bridge
            # call's first arg so the emit loop can flag a literal self-target;
            # non-literal (indirect jalr) targets are normal and ignored here.
            call_args = [gm.group(1).strip() for gm in
                         re.finditer(r'(?:br2_call_generated|br2_call_clean|'
                                     r'call_guest4|call_guest3|call_guest2|'
                                     r'call_guest|psx_dispatch_call)'
                                     r'\(\s*([^,]+)', _body)]
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
        defs[name] = (ret_raw, params, os.path.basename(path), gen_args,
                      call_args)

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
    """Classify a guest prototype return type for the wrapper signature."""
    r = ret.strip()
    if re.fullmatch(r'BR2_WEAK_UINT32_\d+', r): return 'u32'
    if r == 'void' or r.endswith(' void'): return 'void'
    if '*' in r: return 'ptr'
    if re.search(r'\b(double|float|int128)\b', r): return 'skip'
    if re.search(r'\b(undefined8|long long|int64_t|uint64_t)\b', r): return 'u64'
    if BAD_RET.search(r): return 'u32'
    return 'skip'

# Guest addrs whose clean body calls br2_call_generated() only for OTHER
# targets (delegated sub-operations), proven safe to wrap: br2_call_generated
# bypasses clean_try_dispatch, so the inner call runs the generated body
# directly and can never re-enter this wrapper. Self-thunks
# (br2_call_generated of the function's OWN guest addr) must still be
# skipped — psx_dispatch_call would route back through fnmap to the same
# clean body and recurse forever.
WRAP_OK_DELEGATE = {
    0x8017ea7c,   # ev_stream_dispatch: op>=24 stream handlers via guest table
    0x801a1e84,   # dev_cmd_round_dispatch: jalr targets are runtime-loaded
                  # guest fn ptrs (0x801BE808 table / 0x801BE780 error fn);
                  # br2_call_generated bypasses clean_try_dispatch so even a
                  # self-referential table entry runs the generated body —
                  # no wrap->clean->generated recursion is possible.
    0x801a1b50,   # dev_cmd_seq: jalr targets are runtime-loaded guest fn
                  # ptrs (*0x801BE7A0 / *0x801BE7A4 per-slot callbacks);
                  # same bypass argument as dev_cmd_round_dispatch.
    0x801a3350,   # dev_cmd_process: jalr targets are runtime-loaded guest
                  # fn ptrs (*0x801BE78C/88/A0/A4/80); same bypass argument.
    0x80137acc,   # LAB_80137acc channel-cmd interpreter: jalr target is a
                  # runtime-loaded guest fn ptr from the handler table
                  # (param_1[3] + op*4); same bypass argument — the inner
                  # br2_call_generated runs the generated body directly and
                  # can never re-enter this wrapper.
    0x8012ecbc,   # player_light_apply: jalr target from light-fn table
                  # DAT_801b140c; same bypass argument.
    0x8012ee48,   # camera_state_cache: jalr target from camera-fn table
                  # DAT_801b144c; same bypass argument.
    0x80135180,   # fighter_trail_emit: jalr target from trail-fn table
                  # guest RAM 0x801b1734 (PTR_FUN_801b1734 fix); same bypass.
    0x80184f28,   # script_op_dispatch: opcode handlers via guest table
                  # 0x801b5528; same bypass argument (op>=table reads are
                  # faithful OOB, see N+84 — no bounds check added).
    0x80186b2c,   # fighter_attach_run: part-attach handlers via guest table
                  # 0x801b55c0; same bypass argument.
    # NOTE: 0x80138cec impact_fx_update is intentionally NOT here — its clean
    # body self-delegates (br2_call_generated(0x80138CECu)) into a
    # mid-function CPS continuation that consumes live s-regs; wrapping it
    # would recurse. It stays generated-authoritative per the delegation rule.
}

# Guest addrs that must stay GENERATED-authoritative: their fnmap clean port
# is correct for direct C callers but cannot replace the generated CPS frame
# when reached via cpu->pc / jal dispatch — the §4 deferred-IRQ coroutine
# contract needs live resume-PC + per-block psx_check_interrupts_at that a
# flag-0 wrapper cannot supply (clean_try_dispatch forces pc=$ra on return,
# clobbering any coroutine yield). These addrs were previously shadowed by
# clean_cps.map rows (flag 1, KEEP-filterable to generated); with those rows
# retired the fnmap port would win and flip them to flag 0. Force-skip the
# wrapper so dispatch falls through to generated, matching the proven
# PSX_CPS_KEEP boot config. fn_lookup still resolves them for indirect calls.
GENERATED_ONLY = {
    0x80194470, 0x8019457c, 0x80194614, 0x8019466c, 0x80194940,
    0x80196370, 0x801968cc, 0x80196b4c, 0x80196e14, 0x801979f8,
    0x801985c4, 0x80198af0, 0x80198c90, 0x80198e2c, 0x801990f0,
    0x80199440, 0x8019a00c, 0x8019a09c, 0x8019a350, 0x8019a364,
    0x8019a378, 0x8019a3f0, 0x8019a460, 0x8019a5d8, 0x8019a6e0,
    0x8019a814, 0x8019a8ec, 0x8019aabc, 0x8019b308, 0x8019b310,
    0x8019d500, 0x8019d508, 0x8019e330, 0x8019e39c, 0x8019e4c8,
    0x8019e55c, 0x8019e5cc, 0x8019e600, 0x801a0660, 0x801a0be0,
    0x801a0ddc, 0x801a1a40, 0x801a1b50, 0x801a1e84, 0x801a2808,
    0x801a2ec8, 0x801a36f0,
}

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

# Guest addrs whose parameters arrive in NON-a0-a3 registers. These are
# mid-function / alternate entries (or v0/v1-consuming continuations) where
# the guest contract reads gpr[2]/gpr[3] (or other regs) instead of the
# standard a0-a3 arg regs. Keyed by guest addr -> tuple of cpu->gpr indices,
# one per clean-C parameter (positions past the tuple fall back to the
# normal a0-a3/stack marshalling). Verified against generated bodies:
#   0x80199100 cd_dir_entry_store: entry = v1 + (v0 << 5) — index in v0,
#     table base in v1 (generated func_801990F0 falls through; v0/v1 are
#     loaded from 0x801fc6a8/0x801fc6c0 by the finalize prologue).
#   0x80199184 cd_file_open_seq: alternate entry into func_8019917C at +8 —
#     the flag-cell load is skipped so the branch tests the CALLER's v0;
#     path arg stays in a0.
ARG_GPRS = {
    0x80199100: (2, 3),
    0x80199184: (2, 4),
    # record-copy family: second pointer arrives in $t0 (param index 3)
    0x801639d8: (4, 5, 6, 8),
    0x80163a20: (4, 5, 6, 8),
    # part_flag100_set: count/base arg in $t0 (param index 3)
    0x801796cc: (4, 5, 6, 8),
    # part_desc_clear: descriptor base arrives in $t0 (param index 0)
    0x8017a6b4: (8,),
    # prim_persp_ot_emit: hand-written leaf — count in a3, out-delta in
    # $t4, in-stride in $t7, out-stride in $t8 (verified vs generated body:
    # gpr[12]/[15]/[24] are read but never written inside 0x8018B76C).
    0x8018b76c: (4, 5, 7, 12, 15, 24),
    # prim_emit_tail_advance: shared CPS-split loop tail — same live-reg
    # contract as prim_persp_ot_emit (cur/ot/count/delta/in_stride/out_stride
    # in a0/a1/a3/$t4/$t7/$t8); verified vs br2eu_func_8018B884.
    0x8018b884: (4, 5, 7, 12, 15, 24),
    # dev_cmd_round_dispatch: a0 ctx, a2/a3 pass-through (a1 is overwritten
    # by the prologue's counter-cell pointer before any call).
    0x801a1e84: (4, 6, 7),
    # stride-0x10 record flag scan family @0x80156458/70: table base in
    # $v1 (param 2); a0/a1 stay running index/limit (verified vs
    # generated/SLES_017.22_full_19.c:0x80156458-0x80156483).
    0x80156458: (4, 5, 3),
    0x80156470: (4, 5, 3),
    # FUN_80158514 field hand-off: source struct base in $v0 (param 0),
    # dest in a0 (verified vs full_20:0x80158514 — lw v1,0x36e8(v0)).
    0x80158514: (2, 4),
    # FUN_80163220 offset resolve: base index in $v0 (param 0), addend
    # in a0, slot index in a2 (verified vs full_25:0x80163220).
    0x80163220: (2, 4, 6),
    # FUN_801631d0 record chase: pre-shifted offset in $v0 (param 0),
    # base in a0, slot in a2 (verified vs full_25:0x801631d0).
    0x801631d0: (2, 4, 6),
    # rec scan head @0x80163b10: cursor a0, field limit a1, count a2,
    # running index in $v1 (param 3) (verified vs full_25:0x80163b10).
    0x80163b10: (4, 5, 6, 3),
    # rec scan latch @0x80163b28: slt result in $v0 (param 0), index in
    # $v1, cursor a0, field limit a1, count a2 (verified vs full_25).
    0x80163b28: (2, 3, 4, 5, 6),
    # FUN_80159f44 +0xa0 field set: select flag in $v0 (param 0),
    # struct a0, value a1 (verified vs full_21:0x80159f44).
    0x80159f44: (2, 4, 5),
    # FUN_801574cc +0xe nibble rw: set/clear flag in a0, struct ptr in
    # $v1 (param 1) (verified vs full_20:0x801574cc — lbu/sb v1+0xe).
    0x801574cc: (4, 3),
    # FUN_801597e0 u16 pair store: dest in $t0 (param 0), values in
    # $a3/$t3 (params 1/2) (verified vs full_20:0x801597e0).
    0x801597e0: (8, 7, 11),
    # byte-range compare pair @0x80167b00/20: start index in a1,
    # src bases in a2/a3 (verified vs full_27:0x80167b00-0x80167b33).
    0x80167b00: (5, 6, 7),
    0x80167b20: (5, 6, 7),
    # elem_pair_field_set alias mid-entries (BIOS CD sub-channel cbs
    # @0x801FEB44/@0x801FEB48): 0x801AA244 reads v0=elem-array base,
    # a0=ctx, a1=idx*4, a2/a3=fields; 0x801AA2AC reads v1=elem ptr.
    # Verified insn-by-insn vs generated/SLES_017.22_full_52.c:15441-15760.
    0x801aa244: (2, 4, 5, 6, 7),
    0x801aa2ac: (3,),
}

# ---- emit loop: classify each entry, build wrapper + dispatch row ----
wrappers, table, skipped = [], [], []
for addr, name in sorted(entries):
    if addr in GENERATED_ONLY:
        skipped.append((addr, name, 'generated-only')); continue
    if name not in defs:
        skipped.append((addr, name, 'no-def')); continue
    ret_raw, params, f, gen_args, call_args = defs[name]
    # Self-delegation and indirect-delegation thunks stay unwrapped (see the
    # def-scan comment): wrapping them recurses through clean_try_dispatch.
    # Literal delegates to a different guest addr are safe to wrap.
    deleg_bad = False
    for ga in gen_args:
        lit = re.fullmatch(r'0x[0-9a-fA-F]+[uUlL]*|\d+[uUlL]*', ga)
        if lit is None:
            if addr in WRAP_OK_DELEGATE:
                continue  # proven-safe indirect delegate (see WRAP_OK_DELEGATE)
            skipped.append((addr, name, 'indirect-delegate')); deleg_bad = True; break
        v = lit.group(0).rstrip('uUlL')
        iv = int(v, 16) if v.lower().startswith('0x') else int(v, 10)
        if iv == addr:
            skipped.append((addr, name, 'self-delegate')); deleg_bad = True; break
    # Literal self-target via ANY bridge call (call_guest/call_guest4/
    # br2_call_clean/psx_dispatch_call) is the same recursion trap: the named
    # port forwards to its own guest addr, and once its cps_ shadow is gone a
    # wrap would route straight back into itself. Only literal targets count —
    # an indirect jalr target is a normal cross/indirect call, not a thunk.
    if not deleg_bad:
        for ca in call_args:
            lit = re.fullmatch(r'0x[0-9a-fA-F]+[uUlL]*|\d+[uUlL]*', ca)
            if lit is None:
                continue
            v = lit.group(0).rstrip('uUlL')
            iv = int(v, 16) if v.lower().startswith('0x') else int(v, 10)
            if iv == addr:
                skipped.append((addr, name, 'self-delegate')); deleg_bad = True; break
    if deleg_bad:
        continue
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
    if len(kinds) > 16:
        skipped.append((addr, name, 'nargs>16')); continue
    nargs = 0 if kinds == [('void',)] else len(kinds)
    # build call args
    args = []
    force_ptr = HOST_PTR_PARAMS.get(addr, set())
    src_regs = ARG_GPRS.get(addr)
    for i, k in enumerate(kinds):
        if k[0] == 'void': break
        if src_regs is not None and i < len(src_regs):
            raw = f'cpu->gpr[{src_regs[i]}]'
        else:
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
    elif rk == 'u64':
        body = (f'    {{ uint64_t _r = (uint64_t)({call});\n'
                f'      cpu->gpr[2] = (uint32_t)_r; cpu->gpr[3] = (uint32_t)(_r >> 32); }}\n')
    else:  # ptr return -> guest addr
        body = f'    cpu->gpr[2] = guest_of((const void *)({call}));\n'
    wrappers.append(f'static void {wname}(CPUState *cpu) {{\n{decl}\n{body}}}\n')
    table.append((addr, wname, clean_cost(addr), clean_frame(addr),
                  1 if addr in cps_addrs else 0))

# ---- assemble src/clean_table.c ----
out = []
out.append('/* Auto-generated by game/gen_clean_table.py — clean-source CPUState\n')
out.append(' * dispatch adapters. Do not edit by hand. */\n')
out.append('#include <stdint.h>\n#include <stdbool.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdio.h>\n')
out.append('#include "cpu_state.h"\n#include "psx_cycles.h"\n\n')
out.append('extern void *guest_at(uint32_t addr);\n')
out.append('extern uint32_t guest_of(const void *p);\n')
out.append('extern void guest_sp_set(uint32_t sp);\nextern void clean_bridge_set_cpu(CPUState *cpu);\nextern int br2_bail_pending(void);\n\n')
out.append('typedef uint8_t undefined1; typedef uint16_t undefined2;\n')
out.append('typedef uint32_t undefined4; typedef uint64_t undefined8;\n')
out.append('typedef unsigned int uint; typedef uint8_t byte;\n')
out.append('typedef uint16_t ushort; typedef uint8_t u8; typedef int8_t s8;\n')
out.append('typedef uint16_t u16; typedef int16_t s16; typedef uint32_t u32;\n')
out.append('typedef int32_t s32; typedef int64_t s64; typedef uint64_t u64;\n')
out.append('typedef uint8_t undefined;\n\n')
out.append(''.join(wrappers))
out.append('typedef void (*CleanWrapFn)(CPUState *cpu);\n')
out.append('#define CLEAN_ENT_CPS 1u\n')
out.append('typedef struct { uint32_t addr; CleanWrapFn fn; uint32_t cost; uint32_t frame; uint32_t flags; } CleanDispatchEnt;\n')
out.append('static const CleanDispatchEnt k_clean_dispatch[] = {\n')
for addr, w, c, fr, fl in table:
    out.append(f'    {{ 0x{addr:08x}u, {w}, {c}u, {fr}u, {fl}u }},')
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

/* Per-entry hit counters: g_clean_hits[i] counts how many times dispatch
 * slot i actually ran its clean body (after range/CPS filters pass).
 * Unlike the 512-entry trace ring this answers "did clean fn X ever run"
 * for infrequent paths. br2_dbg_clean_hit_at maps slot -> (addr,count). */
static uint64_t g_clean_hits[sizeof(k_clean_dispatch) / sizeof(k_clean_dispatch[0])];
uint32_t br2_dbg_clean_hit_count(void) {
    return (uint32_t)(sizeof(g_clean_hits) / sizeof(g_clean_hits[0]));
}
uint32_t br2_dbg_clean_hit_at(uint32_t i, uint64_t *cnt) {
    if (i >= br2_dbg_clean_hit_count()) return 0;
    *cnt = g_clean_hits[i];
    return k_clean_dispatch[i].addr;
}

int clean_try_dispatch(CPUState *cpu, uint32_t addr) {
    const CleanDispatchEnt *e;
    if (!clean_range_ok(addr)) return 0;
    e = clean_find(addr);
    if (!e) return 0;
    /* CPS A/B diagnostics (parsed once):
     *   PSX_NO_CPS=1    — route EVERY CLEAN_ENT_CPS entry to generated.
     *   PSX_CPS_KEEP=lo-hi[,lo-hi...] — keep cps_ ONLY inside these hex
     *     ranges; cps_ entries outside -> generated. Named clean ports are
     *     never filtered. Used to bisect which cps_ region wedges boot. */
    static int _no_cps = -1;
    static uint32_t _cps_keep[32]; static int _cps_keep_n = -1;
    if (_no_cps < 0) {
        const char *_s = getenv("PSX_NO_CPS");
        _no_cps = (_s && _s[0] == '1');
    }
    if (_cps_keep_n < 0) {
        _cps_keep_n = 0;
        const char *_k = getenv("PSX_CPS_KEEP");
        if (_k) {
            char _buf[512]; size_t _l = strlen(_k); if (_l >= sizeof _buf) _l = sizeof _buf - 1;
            memcpy(_buf, _k, _l); _buf[_l] = 0;
            for (char *t = strtok(_buf, ","); t && _cps_keep_n < 16; t = strtok(NULL, ",")) {
                uint32_t lo = 0, hi = 0;
                if (sscanf(t, "%i-%i", (unsigned *)&lo, (unsigned *)&hi) == 2 ||
                    sscanf(t, "0x%x-0x%x", &lo, &hi) == 2) {
                    _cps_keep[_cps_keep_n*2] = lo; _cps_keep[_cps_keep_n*2+1] = hi; _cps_keep_n++;
                }
            }
        }
    }
    if ((e->flags & CLEAN_ENT_CPS) && _no_cps) return 0;
    if ((e->flags & CLEAN_ENT_CPS) && _cps_keep_n > 0) {
        int _in = 0;
        for (int _i = 0; _i < _cps_keep_n; _i++)
            if (addr >= _cps_keep[_i*2] && addr < _cps_keep[_i*2+1]) { _in = 1; break; }
        if (!_in) return 0;
    }
    g_clean_trace[g_clean_trace_n & 511] = addr;
    g_clean_trace_ra[g_clean_trace_n & 511] = cpu->gpr[31];
    g_clean_trace_n++;
    g_clean_hits[e - k_clean_dispatch]++;
    extern void psx_check_interrupts_dispatch_entry(CPUState *cpu, uint32_t resume_pc);
    psx_check_interrupts_dispatch_entry(cpu, addr);
    /* Emulate the MIPS prologue/epilogue stack frame the clean body skips:
     * clean C runs on the host stack, so without this gpr[29] stays shallower
     * than HW by every clean ancestor frame and generated callees spill onto
     * the wrong guest addresses. e->frame is the prologue's `addiu sp,sp,-N`
     * decoded from the EXE; 0 for leaf/non-standard entries (unchanged).
     * PSX_CLEAN_FRAME=1 enables the model for bring-up (default OFF: the
     * clean ports' br2_guest_frame/guest_sp-relative stack usage is calibrated
     * around frame-less sp, so applying prologue frames shifts every callee
     * spill and livelocks the CD path — faithful sp needs the ports reworked
     * stack-aware first). On a call-bail unwind the guest function never runs
     * its epilogue (generated CPS frames skip it identically), so $sp must
     * stay decremented — mirror br2_guest_frame_abandon, don't force-restore. */
    static int _clean_frame_en = -1;
    if (_clean_frame_en < 0) {
        const char *_s = getenv("PSX_CLEAN_FRAME");
        _clean_frame_en = (_s && _s[0] == '1');
    }
    int _cps = (e->flags & CLEAN_ENT_CPS) != 0;
    uint32_t _saved_sp = cpu->gpr[29];
    /* CPS coroutine entries own gpr[29] end-to-end: their prologue/epilogue
     * manage the guest frame and it must persist across yields/continuations.
     * The save/restore below emulates a prologue for ordinary clean fns that
     * skip it — applying it to a CPS fn would undo sp-=N on entry-return and
     * force sp back over the epilogue's sp+=N, corrupting the frame (reads of
     * the saved s0-s3/ra slots then hit wrong addresses -> jr $ra to garbage). */
    int _framed = (!_cps && _clean_frame_en && e->frame);
    if (_framed) cpu->gpr[29] = _saved_sp - e->frame;
    guest_sp_set(cpu->gpr[29]);
    clean_bridge_set_cpu(cpu);
    /* CPS entries (clean_cps.map) are coroutine continuations: mirror the
     * generated dispatch (cpu->pc = resume_pc before fn) so the body's
     * pc-switch picks its block on re-entry after a yield, and preserve a
     * tail-dispatch cpu->pc = callee instead of forcing the `jr $ra` epilogue.
     * Ordinary entries keep the historical contract exactly: cpu->pc is left
     * untouched on entry and always rewritten to $ra on return — BIOS/kernel
     * services and generated callees read the stale pc, so setting it here for
     * every wrapper broke boot. */
    if (_cps) cpu->pc = addr;
    e->fn(cpu);
    if (!_cps && (!_framed || !br2_bail_pending())) cpu->gpr[29] = _saved_sp;
    guest_sp_set(cpu->gpr[29]);
    if (_cps) {
        if (cpu->pc == addr) cpu->pc = cpu->gpr[31];
    } else {
        cpu->pc = cpu->gpr[31];
    }
    if (e->cost) {
        /* PSX_CPS_COST_MUL=<pct> — diagnostic: scale the flat per-dispatch
         * cycle charge for CLEAN_ENT_CPS coroutine pieces (default 100 =
         * unchanged). cps_ transliterations strip the per-instruction
         * psx_cyc_* load-wait/interlock charges that generated code folds
         * in, so a piece may under-charge vs its generated twin; this knob
         * tests whether that desync wedges the CD/IRQ wait path. */
        static int _cost_mul = -1;
        if (_cost_mul < 0) {
            const char *_m = getenv("PSX_CPS_COST_MUL");
            _cost_mul = (_m && _m[0]) ? atoi(_m) : 100;
            if (_cost_mul <= 0) _cost_mul = 100;
        }
        uint32_t _c = (_cps && _cost_mul != 100)
            ? (uint32_t)(((uint64_t)e->cost * (uint64_t)_cost_mul) / 100u)
            : e->cost;
        if (_c) psx_advance_cycles(_c);
    }
    return 1;
}
''')

open(OUT, 'w').write(''.join(out))
print(f'wrote {OUT}: {len(table)} wrappers, {len(skipped)} skipped')
from collections import Counter
print(Counter(r for _,_,r in skipped).most_common(8))
