#!/usr/bin/env python3
"""Generate game/fnmap.c: guest-address -> host-function table."""
import re,glob
names={}   # name -> addr_hex
addr_src={}  # name -> 'ren'|'enc'|'mark'  (source reliability for dedup)
ren={}
try:
    for line in open('game/renames.map'):
        line=line.strip()
        if not line or line.startswith('#'): continue
        a,n=line.split(None,1); ren[n]='0x'+a
except FileNotFoundError: pass
for f in sorted(glob.glob('game/*.c')):
    if f.endswith(('bind.c','fnmap.c')): continue
    src=open(f).read()
    # comment-blanked copy (positions preserved) for def scanning;
    # markers live in comments so lookback uses original src
    code=re.sub(r'/\*.*?\*/',lambda m:'\n'*m.group(0).count('\n')+' '*(len(m.group(0))-m.group(0).count('\n')),src,flags=re.S)
    code=re.sub(r'//[^\n]*','',code)
    # candidate defs: NAME(...) {  — any identifier, then check marker/static
    for m in re.finditer(r'\b([A-Za-z_]\w*)\s*\(',code):
        name=m.group(1)
        if name in ('if','for','while','switch','return','sizeof','extern','static','void','int','uint32_t','int32_t','uint16_t','int16_t','uint8_t','int8_t','char','short','long','const','struct','union','enum','do','else','case','break','continue','typedef','defined'):
            continue
        i=m.end(); depth=1
        while i<len(code) and depth:
            c=code[i]
            if c=='(':depth+=1
            elif c==')':depth-=1
            i+=1
        if depth: continue
        j=i
        while j<len(code) and code[j] in ' \t\r\n': j+=1
        if j>=len(code) or code[j]!='{': continue
        # look back past whitespace/comments for 'static' or attribute
        k=m.start()-1
        while k>=0 and code[k] not in ';{}': k-=1
        decl=code[k+1:m.start()]
        decl_nc=re.sub(r'/\*.*?\*/','',decl,flags=re.S)
        decl_nc=re.sub(r'//.*','',decl_nc)
        if re.search(r'\bstatic\b',decl_nc): continue
        if '__attribute__' in decl_nc: continue
        # entry-point symbols (_start/main) belong to the generated boot
        # path — dispatching them clean would run the whole game inside one
        # C call tree where unported callees hit weak stubs instead of
        # falling back to generated code. Keep them out of the map.
        if name.startswith('entry_0x'):
            continue
        # determine guest addr: FUN_/LAB_ name, or @0x marker within 500 chars back
        addr=None; src_kind='mark'
        fm=re.match(r'^(?:FUN_|LAB_)([0-9a-f]{8})$',name)
        if fm: addr=fm.group(1); src_kind='enc'
        else:
            # addr-encoded names (entry_0x…, func_0x…, *_0x801…) carry the
            # guest address in the suffix — more reliable than @markers,
            # which doc comments may attach to jump tables/other addrs.
            sm=re.search(r'_0x([0-9a-f]{8})$',name)
            if sm: addr=sm.group(1); src_kind='enc'
            elif name in ren: addr=ren[name][2:]; src_kind='ren'
            else:
                back=src[max(0,m.start()-600):m.start()]
                marks=re.findall(r'@0x([0-9a-f]{8})',back)
                if marks: addr=marks[-1]; src_kind='mark'
        if addr is None: continue
        # only map plausible guest text addrs
        if not (0x80110000 <= int(addr,16) <= 0x80200000): continue
        if name in names and names[name]!=addr:
            print("CONFLICT",name,names[name],addr,f); continue
        names[name]=addr; addr_src[name]=src_kind
# Dedup by guest addr: two clean names can resolve to the same addr (an
# internal helper/impl picks up a stray @0x<addr> doc marker that belongs to
# a different function — e.g. ev_step/print_mode_dispatch vs the renames.map
# guest-ABI wrappers). fn_table is keyed by addr for binary-search lookup, so
# a collision emits two entries and which one wins is sort-order fragile.
# Keep the most authoritative source: renames.map > addr-encoded name >
# @marker lookback. On a tie keep the renames.map name, else alphabetical.
_rank={'ren':3,'enc':2,'mark':1}
by_addr={}
for n,a in names.items():
    by_addr.setdefault(a,[]).append(n)
drop=set()
for a,ns in by_addr.items():
    if len(ns)<2: continue
    best=max(ns,key=lambda n:(_rank.get(addr_src.get(n,'mark'),0), n in ren, n))
    for n in ns:
        if n!=best:
            drop.add(n)
            print("DEDUP addr",a,"keep",best,"drop",n,"src",addr_src.get(n))
for n in drop:
    del names[n]; addr_src.pop(n,None)
out=['''/* Auto-generated guest-code-address -> host-function map.
 * Regenerate: scan src/*.c for non-static FUN_/LAB_ defs and @0x
 * marker defs (excludes bind.c weak stubs). Do not edit by hand. */

#include <stdint.h>

''']
for n in sorted(names):
    out.append(f'extern void {n}(void);\n')
out.append('''typedef struct { uint32_t addr; void *fn; } Br2FnEnt;

const Br2FnEnt fn_table[] = {
''')
for n in sorted(names,key=lambda n:int(names[n],16)):
    out.append(f'  {{ 0x{names[n]}, (void *){n} }},\n')
out.append('''};


void *fn_lookup(uint32_t addr)
{
  uint32_t lo = 0, hi = (uint32_t)(sizeof(fn_table)/sizeof(fn_table[0]));
  while (lo < hi) {
    uint32_t mid = (lo + hi) >> 1;
    if (fn_table[mid].addr < addr) lo = mid + 1; else hi = mid;
  }
  if (lo < (uint32_t)(sizeof(fn_table)/sizeof(fn_table[0])) && fn_table[lo].addr == addr)
    return fn_table[lo].fn;
  return 0;
}

''')
open('game/fnmap.c','w').write(''.join(out))
print(len(names),"entries")
