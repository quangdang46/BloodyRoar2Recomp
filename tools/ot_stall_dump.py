#!/usr/bin/env python3
# ot_stall_dump.py — watch frame counter on PORT; the instant it stops
# advancing while gs==8 ph==8, dump OT buffers + packet chains.
import socket, json, struct, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5417

def cmd(c, timeout=20):
    s = socket.create_connection(('127.0.0.1', PORT), 5)
    s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(timeout); b = b''
    while True:
        x = s.recv(1 << 20)
        if not x: break
        b += x
        try:
            d = json.loads(b.split(b'\n')[0])
            if 'hex' in d or d.get('ok') is not None or 'error' in d:
                break
        except Exception:
            pass
    s.close()
    try:
        return json.loads(b.split(b'\n')[0])
    except Exception:
        return {}

def rd(a, n):
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": hex(n)})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

def w(b, o):
    return struct.unpack('<I', b[o:o+4])[0] if len(b) >= o + 4 else 0

def follow(head24, limit=100):
    cur = head24 & 0xffffff
    seen = set(); out = []
    for _ in range(limit):
        if cur == 0xffffff or cur == 0 or cur in seen:
            return out, cur
        seen.add(cur)
        g = cur if cur >= 0x800000 else 0x80000000 | cur
        hw = rd(g, 8)
        if len(hw) < 8:
            out.append((cur, -1, -1)); return out, cur
        hdr = w(hw, 0)
        out.append((cur, hdr >> 24, hdr & 0xffffff))
        cur = hdr & 0xffffff
    return out, cur

def dump(tag):
    print('=== DUMP %s ===' % tag, flush=True)
    st = rd(0x1f800000, 0x20)
    if len(st) < 0x20:
        print('no ram'); return
    print('gs=%d ph=%d f4=%08x f8=%08x fc=%08x dc=%08x' % (
        st[0], st[1], w(st, 4), w(st, 8), w(st, 0xc), w(st, 0xdc)))
    print('e0=%08x e4=%08x 8500=%08x 83b4=%08x' % (
        w(rd(0x1f8000e0, 4), 0), w(rd(0x1f8000e4, 4), 0),
        w(rd(0x801e8500, 4), 0), w(rd(0x801e83b4, 4), 0)))
    for base, name in [(0x801e4110, 'buf0'), (0x801e6260, 'buf1')]:
        full = rd(base, 0x2080)
        if len(full) < 0x100:
            print(name, 'read fail'); continue
        entries = [(i, w(full, i * 4)) for i in range(0x820) if w(full, i * 4)]
        print('%s OT nonzero=%d:' % (name, len(entries)),
              ' '.join('[%x]=%06x' % (i, v & 0xffffff) for i, v in entries[:24]))
        print('%s OT tail:' % name,
              ' '.join('[%x]=%08x' % (i, v) for i, v in entries[-8:]))
        cell = w(full, 0x207c)
        print('%s +207c=%08x' % (name, cell))
        ch, endc = follow(cell)
        print('%s +207c chain end=%06x nodes=%d:' % (name, endc, len(ch)),
              ' '.join('%06x>%06x' % (c, nx & 0xffffff) for c, wc, nx in ch[:40]))
        for s in range(0x20):
            slot = w(full, s * 4)
            if not slot:
                continue
            ch2, end2 = follow(slot, 40)
            print('%s slot[%x]=%06x end=%06x n=%d:' % (name, s, slot & 0xffffff, end2, len(ch2)),
                  ' '.join('%06x>%06x' % (c, nx & 0xffffff) for c, wc, nx in ch2[:30]))

last_fr = None; last_chg = time.time(); dumped = 0
t0 = time.time()
while time.time() - t0 < 400:
    try:
        st = rd(0x1f800000, 0xa0)
    except Exception:
        time.sleep(0.3); continue
    if len(st) < 0xa0:
        time.sleep(0.3); continue
    fr_r = cmd({"cmd": "frame"})
    fr = fr_r.get("frame")
    if fr != last_fr:
        last_fr = fr; last_chg = time.time()
    if st[0] == 8 and st[1] == 8 and fr is not None and time.time() - last_chg > 4 and dumped < 4:
        dump('stall f%s' % fr)
        dumped += 1
        last_chg = time.time() + 30   # don't re-dump instantly
    time.sleep(0.15)
print('done', flush=True)
