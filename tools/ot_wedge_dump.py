#!/usr/bin/env python3
# ot_wedge_dump.py — capture OT + packet-chain state at the versus wedge.
import socket, json, struct, sys
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5271

def cmd(c):
    s = socket.create_connection(('127.0.0.1', PORT), 5)
    s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(20); b = b''
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

st = rd(0x1f800000, 0x20)
print('gs=%d ph=%d f4=%08x f8=%08x fc=%08x dc=%08x' % (
    st[0], st[1], w(st, 4), w(st, 8), w(st, 0xc), w(st, 0xdc)))
print('e0=%08x e4=%08x 8500=%08x 83b4=%08x' % (
    w(rd(0x1f8000e0, 4), 0), w(rd(0x1f8000e4, 4), 0),
    w(rd(0x801e8500, 4), 0), w(rd(0x801e83b4, 4), 0)))

for base, name in [(0x801e4110, 'buf0'), (0x801e6260, 'buf1')]:
    ot = rd(base, 0x80)
    nz = [(i, w(ot, i * 4)) for i in range(0x20) if w(ot, i * 4)]
    print('%s OT[0..31] nonzero:' % name, ' '.join('[%x]=%08x' % (i, v) for i, v in nz))
    # scan whole 0x820-word OT for nonzero entries (sample every word, 2 reads of 0x1040)
    full = rd(base, 0x2080)
    entries = []
    for i in range(0x820):
        v = w(full, i * 4)
        if v:
            entries.append((i, v))
    print('%s OT nonzero count=%d first20:' % (name, len(entries)),
          ' '.join('[%x]=%06x' % (i, v & 0xffffff) for i, v in entries[:20]))
    print('%s OT last10:' % name,
          ' '.join('[%x]=%08x' % (i, v) for i, v in entries[-10:]))
    # chain head cell at +0x207c
    cell = w(full, 0x207c)
    print('%s +207c cell = %08x' % (name, cell))
    # follow the packet chain from +0x207c head (guest addr = 0x80000000|cell)
    head = cell & 0xffffff
    seen = {}
    cur = head
    chain = []
    for _ in range(64):
        if cur == 0xffffff or cur == 0 or cur in seen:
            break
        seen[cur] = len(chain)
        g = 0x80000000 | cur if cur < 0x800000 else cur
        hw = rd(g, 8)
        if not hw:
            chain.append((cur, None, None)); break
        hdr = w(hw, 0)
        chain.append((cur, hdr >> 24, hdr & 0xffffff))
        cur = hdr & 0xffffff
    print('%s chain(+207c):' % name,
          ' '.join('%06x(wc=%s,nx=%06x)' % (c, ('%d' % wc) if wc is not None else '?',
                                           (nx or 0) & 0xffffff)
                    for c, wc, nx in chain))
    # also follow from OT slot 0x16/0x12 heads
    for s in (0x12, 0x16):
        slot = w(full, s * 4)
        if not slot:
            continue
        cur = slot & 0xffffff
        seen2 = {}
        chain2 = []
        for _ in range(48):
            if cur == 0xffffff or cur == 0 or cur in seen2:
                break
            seen2[cur] = 1
            g = 0x80000000 | cur if cur < 0x800000 else cur
            hw = rd(g, 8)
            if not hw:
                chain2.append((cur, None, None)); break
            hdr = w(hw, 0)
            chain2.append((cur, hdr >> 24, hdr & 0xffffff))
            cur = hdr & 0xffffff
        print('%s slot[%x]=%06x chain:' % (name, s, slot & 0xffffff),
              ' '.join('%06x(wc=%s,nx=%06x)' % (c, ('%d' % wc) if wc is not None else '?',
                                               (nx or 0) & 0xffffff)
                        for c, wc, nx in chain2))
print('done')
