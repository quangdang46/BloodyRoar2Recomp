#!/usr/bin/env python3
"""progress.py — measure clean-port coverage and emit docs/progress.{svg,json}.

Adapted from AnyPS5's tools/progress.py (same squarify treemap renderer,
stdlib only). Metrics are counted from the source, not from gameplay:

  done  = real clean-C ports (fn defined in game/*.c, body does not
          delegate to br2_call_generated, name is not a _cps/cpst_
          transliteration)
  todo  = everything still machine-transliterated or delegated:
          - _cps/_cpst_/cps_ transliteration bodies, weighted by their
            clean_cps.map row count (rows = CPS basic blocks still
            load-bearing)
          - thin br2_call_generated delegates (§4 must-delegate), weight 1

Groups are the game/*.c source files — the same partition the port
workflow uses (one workstream per subsystem file).
"""
import json
import re
import sys
from html import escape
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GAME = ROOT / "game"
OUT = ROOT / "docs"

DONE_COLOR, TODO_COLOR, BORDER, TEXT = "#2ea043", "#6e7681", "#0d1117", "#ffffff"
PANEL_WIDTH, GAP, MAP_HEIGHT, HEADER = 900, 10, 320, 30
GRID_COLUMNS, GRID_ROW = 4, 74

# fn definition: return-type words + name + params + '{' — no ';' inside
FN_DEF = re.compile(r"^(?:[A-Za-z_][\w\s\*]*?)\b(\w+)\s*\([^;{}]*\)\s*\{", re.M)
DELEGATE = re.compile(r"\bbr2_call_generated\s*\(")
CPS_NAME = re.compile(r"(_cps|^cps_|^cpst_|^br2eu_)")
STRIP_COMMENTS = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)

SKIP_FILES = {"fnmap.c", "selftest.c"}


def fn_bodies(text):
    """Yield (name, body_text) for each top-level fn definition."""
    for m in FN_DEF.finditer(text):
        name = m.group(1)
        start = m.end() - 1  # at '{'
        depth, i = 0, start
        while i < len(text):
            c = text[i]
            if c == "{":
                depth += 1
            elif c == "}":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        yield name, text[start:i + 1]


def is_transliteration_file(path):
    return bool(re.match(r"(w\d+_e\d+|cps_clean|cd_irq_probe|gen_)", path.name))


def collect():
    # clean_cps.map: symbol -> row count (CPS blocks still load-bearing)
    rows = {}
    map_path = GAME / "clean_cps.map"
    if map_path.is_file():
        for line in map_path.read_text().splitlines():
            parts = line.split()
            if len(parts) >= 2 and not line.startswith("#"):
                rows[parts[1]] = rows.get(parts[1], 0) + 1

    groups = {}
    for path in sorted(GAME.glob("*.c")):
        if path.name in SKIP_FILES:
            continue
        text = path.read_text(errors="ignore")
        if "(" not in text:
            continue
        done = todo = 0
        for name, body in fn_bodies(text):
            code = STRIP_COMMENTS.sub("", body)
            if CPS_NAME.search(name) or is_transliteration_file(path):
                todo += rows.get(name, 1)
            elif DELEGATE.search(code):
                todo += 1
            else:
                done += 1
        if done or todo:
            groups[path.stem] = {"name": path.stem, "label": path.stem,
                                 "done": done, "todo": todo}
    total_done = sum(g["done"] for g in groups.values())
    total_todo = sum(g["todo"] for g in groups.values())
    total = total_done + total_todo
    return {
        "done": total_done, "todo": total_todo, "total": total,
        "percent": round(100 * total_done / total, 2) if total else 0,
        "groups": sorted(groups.values(), key=lambda g: -(g["done"] + g["todo"])),
        "cps_rows_remaining": sum(rows.values()),
    }


# ---------- squarified treemap (from AnyPS5 tools/progress.py) ----------

def worst_ratio(row, side):
    area = sum(row)
    return max(max(side * side * r / area ** 2, area ** 2 / (side * side * r)) for r in row)


def place(row, x, y, w, h, rects):
    thickness = sum(row) / min(w, h)
    offset = 0
    for area in row:
        length = area / thickness
        if w >= h:
            rects.append((x, y + offset, thickness, length))
        else:
            rects.append((x + offset, y, length, thickness))
        offset += length
    return (x + thickness, y, w - thickness, h) if w >= h else (x, y + thickness, w, h - thickness)


def squarify(values, x, y, w, h):
    total = sum(values)
    areas = [v * w * h / total for v in values]
    rects, row = [], []
    while areas:
        side = min(w, h)
        if not row or worst_ratio(row + [areas[0]], side) <= worst_ratio(row, side):
            row.append(areas.pop(0))
            continue
        x, y, w, h = place(row, x, y, w, h, rects)
        row = []
    if row:
        place(row, x, y, w, h, rects)
    return rects


def cells(count, x, y, w, h):
    if not count or w <= 0 or h <= 0:
        return []
    rows = max(1, min(count, round((count * h / w) ** 0.5)))
    result = []
    for r in range(rows):
        in_row = count // rows + (r < count % rows)
        cw = w / in_row
        result += [(x + c * cw, y + r * h / rows, cw, h / rows) for c in range(in_row)]
    return result


def rect(x, y, w, h, color, stroke=0.5):
    return (f'<rect x="{x:.2f}" y="{y:.2f}" width="{w:.2f}" height="{h:.2f}" '
            f'fill="{color}" stroke="{BORDER}" stroke-width="{stroke}"/>')


def text(x, y, value, size):
    return (f'<text x="{x:.2f}" y="{y:.2f}" font-family="sans-serif" font-size="{size}" fill="{TEXT}" '
            f'stroke="{BORDER}" stroke-width="3" paint-order="stroke">{escape(value)}</text>')


def tile_label(group, w, h):
    label_length = int((w - 10) / 7)
    label = group["label"][:label_length] if label_length > 0 else ""
    return label if (label == group["label"] or len(label) >= 3) and h > 22 else ""


def tile(group, x, y, w, h):
    total = group["done"] + group["todo"]
    parts = [f'<g><title>{escape(group["name"])}: {group["done"]} done / '
             f'{group["todo"]} todo ({100 * group["done"] / total:.0f}%)</title>']
    # proportional fill: cap rendered cells so huge tiles stay small SVGs
    n = min(max(total, 1), 120)
    frac = group["done"] / total
    done_cells = round(n * frac)
    for i, cell in enumerate(cells(n, x + 1, y + 1, w - 2, h - 2)):
        parts.append(rect(*cell, DONE_COLOR if i < done_cells else TODO_COLOR))
    parts.append(f'<rect x="{x:.2f}" y="{y:.2f}" width="{w:.2f}" height="{h:.2f}" fill="none" stroke="{BORDER}" stroke-width="2"/>')
    label = tile_label(group, w, h)
    if label:
        parts.append(text(x + 5, y + 16, label, 12))
    parts.append("</g>")
    return parts


def render(data):
    groups = data["groups"]
    width = PANEL_WIDTH + GAP + PANEL_WIDTH
    height = HEADER + MAP_HEIGHT
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {height}" width="{width}" height="{height}">',
             rect(0, 0, width, height, BORDER, 0)]
    parts.append(text(4, 21, f'Clean-port coverage: {data["percent"]}% '
                            f'({data["done"]} done / {data["todo"]} todo units, '
                            f'{data["cps_rows_remaining"]} CPS rows left)', 16))
    # treemap on the left
    shown = groups[:14]
    rects = squarify([g["done"] + g["todo"] for g in shown], 0, HEADER, PANEL_WIDTH, MAP_HEIGHT)
    for g, (x, y, w, h) in zip(shown, rects):
        parts += tile(g, x, y, w, h)
    # grid of ALL groups on the right
    gx = PANEL_WIDTH + GAP
    parts.append(text(gx + 4, HEADER - 6, "All subsystem files", 14))
    cell_w = PANEL_WIDTH / GRID_COLUMNS
    per_row = GRID_COLUMNS
    for i, g in enumerate(groups):
        cx = gx + (i % per_row) * cell_w
        cy = HEADER + (i // per_row) * GRID_ROW
        parts += tile(g, cx, cy, cell_w - 2, GRID_ROW - 2)
    grid_rows = -(-len(groups) // per_row)
    height = HEADER + grid_rows * GRID_ROW
    # rebuild svg with corrected height
    parts[0] = (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {height}" '
                f'width="{width}" height="{height}">')
    parts.append("</svg>")
    return "\n".join(parts)


def main():
    data = collect()
    OUT.mkdir(exist_ok=True)
    (OUT / "progress.json").write_text(json.dumps(data, indent=2))
    (OUT / "progress.svg").write_text(render(data))
    print(f'clean-port coverage {data["percent"]}% '
          f'({data["done"]} done / {data["todo"]} todo units; '
          f'{data["cps_rows_remaining"]} CPS rows left)')
    for g in data["groups"][:12]:
        t = g["done"] + g["todo"]
        print(f'  {g["name"]:24s} {g["done"]:5d} done / {g["todo"]:5d} todo  '
              f'({100 * g["done"] / t:.0f}%)')


if __name__ == "__main__":
    sys.exit(main())
