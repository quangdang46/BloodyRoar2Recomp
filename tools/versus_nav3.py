#!/usr/bin/env python3
# versus_nav3.py — resume versus nav from gs=8 char-select onward (port arg).
import socket, json, struct, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5330

def cmd(c, timeout=8):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    for _ in range(30):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), 3); break
        except OSError:
            time.sleep(0.2)
    else:
        return {}
    try:
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
        try: return json.loads(b.split(b'\n')[0])
        except Exception: return {}
    except Exception:
        return {}

def rd(a, n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": hex(n)})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

def w32(b, o):
    """Read a 32-bit guest word at address a (None on failure)."""
    return struct.unpack('<I', b[o:o+4])[0] if len(b) >= o + 4 else 0

def press(btn, frames, slot=0):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

def state():
    """Snapshot the scratchpad game-state block and return a dict of fields."""
    for _ in range(10):
        st = rd(0x1f800000, 0x20)
        if len(st) >= 2:
            fr = cmd({"cmd": "frame"}).get('frame')
            return st[0], st[1], fr
        time.sleep(0.3)
    return -1, -1, None

START = 0xFFF7; CROSS = 0xBFFF; DOWN = 0xFFBF
CELLS = [
    (0x801e411c, 'A.c'), (0x801e4120, 'A.10'), (0x801e4150, 'A.40'), (0x801e618c, 'A.h'),
    (0x801e627c, 'B.c'), (0x801e6280, 'B.10'), (0x801e62b0, 'B.40'), (0x801e82dc, 'B.h'),
]
def snap():
    """Snapshot named guest cells into a dict."""
    out = {}
    for a, n in CELLS: out[n] = w32(rd(a, 4), 0)
    out['otb'] = w32(rd(0x1f800008, 4), 0)
    out['idx'] = w32(rd(0x1f80000c, 4), 0)
    return out

t0 = time.time()
last_press = 0.0
fight_fr = None
last = {}; last_fr = None; last_chg = time.time()
print('resuming versus nav...', flush=True)
while time.time() - t0 < 900:
    gs, ph, fr = state()
    print('  gs=%s ph=%s fr=%s t=%.0f' % (gs, ph, fr, time.time()-t0), flush=True)
    now = time.time()
    if gs == 8 and ph == 3:
        if now - last_press > 1.0:
            press(CROSS, 30, 0); press(CROSS, 30, 1); last_press = now
    elif gs == 8 and ph == 7:
        if now - last_press > 1.0:
            press(START, 30, 0); press(START, 30, 1); last_press = now
    elif gs == 8 and ph == 8:
        if fight_fr is None:
            fight_fr = fr
            print('*** VERSUS FIGHT ph=8 fr=%s t=%.0f ***' % (fr, time.time()-t0), flush=True)
        cur = snap()
        diffs = []
        for a, n in CELLS:
            if last.get(n) != cur[n]:
                diffs.append('%s:%08x->%08x' % (n, last.get(n) or 0, cur[n]))
        if diffs or (last_fr != fr and int(fr or 0) % 60 == 0):
            print('t=%.0f gs=%d ph=%d fr=%s otb=%08x idx=%d | %s' % (
                time.time()-t0, gs, ph, fr, cur['otb'], cur['idx'], ' '.join(diffs)), flush=True)
        last.update({n: cur[n] for _, n in CELLS})
        if fr != last_fr:
            last_fr = fr; last_chg = time.time()
        if fr is not None and last_chg and time.time() - last_chg > 15:
            print('STALL fr=%s' % fr, flush=True)
            cur = snap()
            for a, n in CELLS:
                print('  %s @%08x = %08x' % (n, a, w32(rd(a, 4), 0)), flush=True)
            break
    elif gs == 5:
        # dropped back to title; mash START to re-enter menu
        if now - last_press > 1.0:
            press(START, 40, 0); last_press = now
    time.sleep(0.5)
print('done', flush=True)
