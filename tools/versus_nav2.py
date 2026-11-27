#!/usr/bin/env python3
# versus_nav2.py — careful state-verified nav to versus fight (gs=8 ph=8),
# then watch OT head cells. PORT arg.
import socket, json, struct, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5310

def cmd(c, timeout=8):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    try:
        s = socket.create_connection(('127.0.0.1', PORT), 3)
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
    except Exception: return {}

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
    st = rd(0x1f800000, 0x20)
    fr = cmd({"cmd": "frame"}).get('frame')
    return st[0], st[1], fr

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
# Phase 1: reach title (gs==5, ph==0 or 1) then press START once -> gs==8 menu
print('waiting for title gs=5...', flush=True)
while time.time() - t0 < 240:
    gs, ph, fr = state()
    print('  gs=%d ph=%d fr=%s t=%.0f' % (gs, ph, fr, time.time()-t0), flush=True)
    if gs == 5 and ph in (0, 1):
        press(START, 40, 0)
        break
    time.sleep(0.8)

# Phase 2: in menu gs==8; navigate to versus. From run1: gs=8 ph=1 -> ph=0 menu,
# then DOWN x2 + START selects versus? Actually prior: ph==0 DOWN,DOWN then START.
print('waiting for gs==8 menu...', flush=True)
menu_seen = 0
while time.time() - t0 < 400:
    gs, ph, fr = state()
    print('  gs=%d ph=%d fr=%s t=%.0f' % (gs, ph, fr, time.time()-t0), flush=True)
    if gs in (3,4,5,6):
        press(START, 40, 0)
        time.sleep(0.5)
        continue
    if gs == 8 and ph == 0:
        menu_seen += 1
        if menu_seen == 1:
            # move selection down twice then confirm
            for _ in range(2): press(DOWN, 15, 0)
            time.sleep(0.5)
            press(START, 40, 0)
            menu_seen = -100  # done pressing
        elif menu_seen > 1 and menu_seen < 90:
            pass
    if gs == 8 and ph == 3:
        press(CROSS, 30, 0); press(CROSS, 30, 1)
        time.sleep(0.4)
    if gs == 8 and ph == 7:
        press(START, 30, 0); press(START, 30, 1)
        time.sleep(0.4)
    if gs == 8 and ph == 8:
        print('*** VERSUS FIGHT ph=8 fr=%s t=%.0f ***' % (fr, time.time()-t0), flush=True)
        break
    time.sleep(0.5)

# Phase 3: watch OT head cells
last = {}; last_fr = None; last_chg = time.time(); ff = fr
print('watching OT cells...', flush=True)
while time.time() - t0 < 700:
    gs, ph, fr = state()
    cur = snap()
    diffs = []
    for a, n in CELLS:
        if last.get(n) != cur[n]:
            diffs.append('%s:%08x->%08x' % (n, last.get(n) or 0, cur[n]))
    if diffs:
        print('t=%.0f gs=%d ph=%d fr=%s otb=%08x idx=%d | %s' % (
            time.time()-t0, gs, ph, fr, cur['otb'], cur['idx'], ' '.join(diffs)), flush=True)
        last.update({n: cur[n] for _, n in CELLS})
    if fr != last_fr:
        last_fr = fr; last_chg = time.time()
    if ff is not None and fr is not None and fr > ff and time.time() - last_chg > 10:
        print('STALL fr=%s' % fr, flush=True)
        cur = snap()
        for a, n in CELLS:
            print('  %s @%08x = %08x' % (n, a, w32(rd(a, 4), 0)), flush=True)
        break
    time.sleep(0.5)
print('done', flush=True)
