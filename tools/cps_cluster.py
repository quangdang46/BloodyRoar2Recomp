#!/usr/bin/env python3
"""cps_cluster.py — reconstruct the logical-function cluster behind a cps_ entry.

cps_clean.c splits each guest function into per-block cps_<addr> defs linked by
  - direct calls      cps_<addr>();
  - dispatch edges    cpu->pc = 0x<addr>; return;   (resolved via clean_cps.map)
  - resume cases      case 0x<addr>u: goto block_<addr>;
  - terminals         cpu->pc = <reg>; return;  (jr ra / jr v0 / jr t9 ...)

Usage:
  python3 tools/cps_cluster.py 0x801289d0 [0x80128a44 ...]
  python3 tools/cps_cluster.py --list 0x80120000 0x80121000   # defs in range

Output per entry: ordered cluster (BFS), edge list, terminals, yield count,
ext-jal count, and a C skeleton suggestion. Read-only analysis — does NOT
modify any file.
"""
import re, sys, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CPS_C = os.path.join(ROOT, "game", "cps_clean.c")
CPS_MAP = os.path.join(ROOT, "game", "clean_cps.map")

DEF_RE = re.compile(r"^void (cps_([0-9a-fA-F]{8}))\(void\)\s*$", re.M)
BODY_RE = re.compile(r"^void cps_([0-9a-fA-F]{8})\(void\)\n\{(.*?)^\}", re.M | re.S)


def load_defs():
    src = open(CPS_C).read()
    defs = {}
    for m in BODY_RE.finditer(src):
        addr = int(m.group(1), 16)
        body = m.group(2)
        info = {"calls": [], "pc_edges": [], "resume": [], "terms": [], "jals": []}
        # direct sequential calls: cps_XXXXXXXX();
        for cm in re.finditer(r"\bcps_([0-9a-fA-F]{8})\s*\(", body):
            info["calls"].append(int(cm.group(1), 16))
        # cpu->pc = 0x...; return;  (constant dispatch edges)
        for pm in re.finditer(r"cpu->pc\s*=\s*(0x[0-9a-fA-Fa-f]+u?|[a-zA-Z_][a-zA-Z0-9_]*)\s*;\s*return", body):
            v = pm.group(1)
            if v.startswith("0x"):
                info["pc_edges"].append(int(v.rstrip("uU"), 16))
            else:
                info["terms"].append(v)
        # resume cases
        for rm in re.finditer(r"case\s+(0x[0-9a-fA-Fa-f]+)u?:\s*goto\s+block_([0-9a-fA-Fa-f]+)", body):
            info["resume"].append(int(rm.group(1), 16))
        # indirect terminal: cpu->pc = <reg>; return;  (already captured non-hex above)
        # explicit jal-ish: call_guest / br2_call / psx_dispatch inside body
        for jm in re.finditer(r"\b(call_guest4?|br2_call_clean|br2_call_generated|psx_dispatch_call|bind_jal|cd_jal|ev_jal)\s*\(", body):
            info["jals"].append(jm.group(1))
        # yield points
        info["yields"] = len(re.findall(r"psx_check_interrupts_at", body))
        defs[addr] = info
    return defs


def cluster(defs, entry):
    seen, order, edges = set(), [], []
    stack = [entry]
    while stack:
        a = stack.pop()
        if a in seen or a not in defs:
            continue
        seen.add(a)
        order.append(a)
        d = defs[a]
        for t in d["calls"] + d["pc_edges"] + d["resume"]:
            edges.append((a, t))
            if t not in seen:
                stack.append(t)
    return order, edges


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    defs = load_defs()
    if "--list" in sys.argv:
        lo, hi = (int(a, 16) for a in args[:2])
        for a in sorted(defs):
            if lo <= a <= hi:
                d = defs[a]
                print(f"cps_{a:08x} calls={len(d['calls'])} pc_edges={len(d['pc_edges'])} "
                      f"resume={len(d['resume'])} terms={d['terms']} jals={len(d['jals'])} y={d['yields']}")
        return
    all_resume = set()
    for d in defs.values():
        all_resume.update(d["resume"])

    def is_internal(t):
        return t in defs or t in all_resume

    for arg in args:
        entry = int(arg, 16)
        if entry not in defs:
            print(f"0x{entry:08x}: no cps_ def")
            continue
        order, edges = cluster(defs, entry)
        ext = sorted({t for _, t in edges if not is_internal(t)})
        terms = [f"cps_{a:08x}:{defs[a]['terms']}" for a in order if defs[a]["terms"]]
        jals = sum(len(defs[a]["jals"]) for a in order)
        y = sum(defs[a]["yields"] for a in order)
        print(f"== cluster entry 0x{entry:08x}: {len(order)} defs, "
              f"{len(edges)} edges, {y} yields, {jals} ext-jals, "
              f"{len(ext)} external targets")
        print("defs:", " ".join(f"cps_{a:08x}" for a in sorted(order)))
        if ext:
            print("external:", " ".join(f"0x{t:08x}" for t in ext))
        if terms:
            print("terminals:", "; ".join(terms))


if __name__ == "__main__":
    main()
