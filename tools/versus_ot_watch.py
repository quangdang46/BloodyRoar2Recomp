#!/usr/bin/env python3
# versus_ot_watch.py — nav to versus (ph=8), then poll OT head cells every
# poll interval; log every value CHANGE with frame number; on stall dump.
import socket, json, struct, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5310

def cmd(c, timeout=6):
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
        try:
            return json.loads(b.split(b'\n')[0])
        except Exception:
            return {}
    except Exception:
        return {}

def rd(a, n):
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": hex(n)})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

def w32(b, o):
    return struct.unpack('<I', b[o:o+4])[0] if len(b) >= o + 4 else 0

def press(btn, frames, slot):
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

START = 0xFFF7; CROSS = 0xBFFF; DOWN = 0xFFBF
# head cells: buf0 base 0x801e4110, buf1 0x801e6260; heads at +0xc,+0x10,+0x40,+0x207c
CELLS = [
    (0x801e411c, 'A.c'), (0x801e4120, 'A.10'), (0x801e4150, 'A.40'), (0x801e618c, 'A.h'),
    (0x801e627c, 'B.c'), (0x801e6280, 'B.10'), (0x801e62b0, 'B.40'), (0x801e82dc, 'B.h'),
]

def snap():
    out = {}
    for a, n in CELLS:
        out[n] = w32(rd(a, 4), 0)
    out['otb'] = w32(rd(0x1f800008, 4), 0)
    out['idx'] = w32(rd(0x1f80000c, 4), 0)
    return out

stage = "title"; sent = {}
last = {}; last_fr = None; last_chg = time.time(); fight_frame = None
t0 = time.time()
while time.time() - t0 < 500:
    st = rd(0x1f800000, 0x9d)
    if len(st) < 0x9d:
        time.sleep(0.2); continue
    gs, ph = st[0], st[1]
    fr = cmd({"cmd": "frame"}, timeout=8).get('frame')
    if fr != last_fr:
        last_fr = fr; last_chg = time.time()
    # nav (aggressive: also handle gs==8 ph==1 title-confirm)
    if stage == "title":
        if ph == 1 and 'start' not in sent:
            press(START, 30, 0); sent['start'] = 1
        if ph == 2 and 'menu_start' not in sent:
            press(START, 40, 0); sent['menu_start'] = 1
        if ph == 8:
            stage = "fight"; fight_frame = fr
            print('PH8 at t=%.0f frame=%s' % (time.time()-t0, fr), flush=True)
        elif ph == 2 and sent.get('menu_start'):
            stage = "menu"
        elif ph == 3 and 'cross' not in sent:
            press(CROSS, 30, 0); press(CROSS, 30, 1); sent['cross'] = 1
        elif ph == 7 and 'gate' not in sent:
            press(START, 30, 0); press(START, 30, 1); sent['gate'] = 1
    elif stage == "menu":
        if gs == 8 and ph == 0 and 'down' not in sent:
            for _ in range(2): press(DOWN, 12, 0)
            sent['down'] = 1
        if gs == 8 and ph == 0 and 'vs' not in sent and sent.get('down'):
            press(START, 40, 0); sent['vs'] = 1
        if gs == 8 and ph == 241:
            stage = "vs_init"
    elif stage == "vs_init":
        if gs == 8 and ph == 8:
            stage = "fight"; fight_frame = fr
            print('PH8(vs) t=%.0f frame=%s' % (time.time()-t0, fr), flush=True)

    if stage == "fight":
        cur = snap()
        # log changes vs last snapshot (per ~0.5s)
        diffs = []
        for k in CELLS:
            n = k[1]
            if last.get(n) != cur[n]:
                diffs.append('%s:%06x->%06x' % (n, (last.get(n) or 0) & 0xffffff, cur[n] & 0xffffff))
        if diffs:
            print('t=%.0f fr=%s otb=%08x idx=%d | %s' % (
                time.time()-t0, fr, cur['otb'], cur['idx'], ' '.join(diffs)), flush=True)
            last.update({k[1]: cur[k[1]] for k in CELLS})
        if fight_frame is not None and fr is not None and fr > fight_frame:
            if time.time() - last_chg > 8:
                print('STALL fr=%s t=%.0f' % (fr, time.time()-t0), flush=True)
                # final dump
                cur = snap()
                print('final otb=%08x idx=%d' % (cur['otb'], cur['idx']), flush=True)
                for a, n in CELLS:
                    print('  %s @%08x = %08x' % (n, a, w32(rd(a,4),0)), flush=True)
                break
    time.sleep(0.4)
print('done', flush=True)
