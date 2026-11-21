#!/usr/bin/env python3
# nav_and_dump.py — nav to versus fight on PORT, then IMMEDIATELY dump OT
# state when frames stall at ph=8. Single process, minimal window.
import socket, json, struct, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5399

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

def press(btn, frames, slot):
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

START = 0xFFF7; CROSS = 0xBFFF; DOWN = 0xFFBF

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
        # follow +207c chain
        def follow(head24, limit=80):
            cur = head24 & 0xffffff
            seen = set(); out = []
            for _ in range(limit):
                if cur == 0xffffff or cur == 0 or cur in seen:
                    break
                seen.add(cur)
                g = cur if cur >= 0x800000 else 0x80000000 | cur
                hw = rd(g, 8)
                if len(hw) < 8:
                    out.append((cur, -1, -1)); break
                hdr = w(hw, 0)
                out.append((cur, hdr >> 24, hdr & 0xffffff))
                cur = hdr & 0xffffff
            return out, cur
        ch, endc = follow(cell)
        print('%s +207c chain end=%06x:' % (name, endc),
              ' '.join('%06x>%06x' % (c, nx & 0xffffff) for c, wc, nx in ch))
        for s in (0x12, 0x16):
            slot = w(full, s * 4)
            if not slot:
                continue
            ch2, end2 = follow(slot)
            print('%s slot[%x]=%06x chain end=%06x:' % (name, s, slot & 0xffffff, end2),
                  ' '.join('%06x>%06x' % (c, nx & 0xffffff) for c, wc, nx in ch2))

t0 = time.time(); stage = "title"; sent = {}
fight_frame = None; stall_at = None
last_fr = None; last_chg = time.time()
while time.time() - t0 < 400:
    try:
        st = rd(0x1f800000, 0x9d)
    except Exception:
        time.sleep(0.3); continue
    if len(st) < 0x9d:
        time.sleep(0.3); continue
    gs, ph, sub = st[0], st[1], st[0x9C]
    fr = cmd({"cmd": "frame"}).get("frame")
    el = time.time() - t0
    if fr != last_fr:
        last_fr = fr; last_chg = time.time()
    print("t=%.0f stage=%s gs=%d ph=%d frame=%s" % (el, stage, gs, ph, fr), flush=True)
    # nav logic (same as nav_to_fight)
    if stage == "title":
        if ph == 1 and 'start' not in sent:
            press(START, 30, 0); sent['start'] = 1
        if ph == 2 and 'menu_start' not in sent:
            press(START, 40, 0); sent['menu_start'] = 1
        if ph == 2 and sent.get('menu_start'):
            stage = "menu"
    elif stage == "menu":
        if gs == 8 and ph == 0 and 'down' not in sent:
            for _ in range(2): press(DOWN, 12, 0)
            sent['down'] = 1
        if gs == 8 and ph == 0 and 'vs' not in sent and sent.get('down'):
            press(START, 40, 0); sent['vs'] = 1; stage = "vs_init"
        elif gs == 8 and ph == 241:
            stage = "vs_init"
    elif stage == "vs_init":
        if gs == 8 and ph == 3 and 'cross' not in sent:
            press(CROSS, 30, 0); press(CROSS, 30, 1); sent['cross'] = 1
        if gs == 8 and ph == 7 and 'gate' not in sent:
            press(START, 30, 0); press(START, 30, 1); sent['gate'] = 1
        if gs == 8 and ph == 8:
            if fight_frame is None:
                fight_frame = fr; print('FIGHT start frame=%s' % fr, flush=True)
            elif fr is not None and fr > fight_frame:
                if stall_at is None:
                    stall_at = fr
                if time.time() - last_chg > 6:
                    print('STALL detected frame=%s' % fr, flush=True)
                    dump('wedge')
                    # keep dumping a few times
                    for i in range(3):
                        time.sleep(1); dump('wedge+%d' % i)
                    break
    time.sleep(0.12)
print('done', flush=True)
