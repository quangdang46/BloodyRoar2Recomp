#!/usr/bin/env python3
# cpu_phase_probe.py — navigate headless BloodyRoar2 to arcade char-select and
# watch the CPU (slot1) phase seed path: battle+0xb4/+0xb6, edge words
# +0x68/+0x6a, mode bytes +0x9b/+0x9c, +0x92, and the pad-1 record @0x801befca.
import socket, json, time, sys

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 8940

def cmd(c):
    for _ in range(60):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), 10); break
        except OSError:
            time.sleep(0.2)
    else:
        return {}
    s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(10); b = b''
    try:
        while True:
            x = s.recv(1 << 20)
            if not x: break
            b += x
            try:
                d = json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None: break
            except Exception:
                pass
    except Exception:
        pass
    s.close()
    try:
        return json.loads(b.split(b'\n')[0])
    except Exception:
        return {}

def rd(a, n):
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""

def u16(b, o): return b[o] | (b[o+1] << 8) if len(b) > o+1 else 0
def u32(b, o): return (b[o] | (b[o+1]<<8) | (b[o+2]<<16) | (b[o+3]<<24)) if len(b) > o+3 else 0

def press(btn, frames, slot):
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

START = 0xFFF7; CROSS = 0xBFFF; DOWN = 0xFFBF

def dump(tag):
    b = rd(0x1f800000, 0xa0)
    if len(b) < 0x9d: return
    pad1 = rd(0x801befca, 16)
    pad1s = ' '.join('%02x' % x for x in pad1)
    print("%s DAT1=%d gs? +92=%04x +9b=%02x +9c=%02x +68=%04x +6a=%04x +b4=%04x +b6=%04x pad1rec=[%s]" % (
        tag, b[1], u16(b,0x92), b[0x9b], b[0x9c], u16(b,0x68), u16(b,0x6a),
        u16(b,0xb4), u16(b,0xb6), pad1s), flush=True)

t0 = time.time(); stage = "title"; sent = {}
lastdump = 0
while time.time() - t0 < 200:
    st = rd(0x1f800000, 0x9d)
    if len(st) < 0x9d:
        time.sleep(0.3); continue
    gs, ph, sub = st[0], st[1], st[0x9C]; sig = (st[0x9B], st[0x9C])
    now = time.time()
    # periodic dump during interesting states
    if now - lastdump > 1.0 and (gs >= 5):
        dump("t=%.0f gs=%d ph=%d sub=%d sig=%s" % (now-t0, gs, ph, sub, sig))
        lastdump = now
    if stage == "title":
        if gs == 5 and ph == 2:
            stage = "menu"
        elif gs == 8:
            stage = "arcade_init"
        elif now - sent.get('t', 0) >= 0.8:
            press(START, 3, 0); sent['t'] = now
    elif stage == "menu":
        if gs == 5 and ph == 2:
            if now - sent.get('s', 0) >= 0.8:
                press(START, 3, 0); sent['s'] = now; time.sleep(0.3)
        elif gs == 8:
            stage = "arcade_init"
        elif gs != 5:
            stage = "title"
    elif stage == "arcade_init":
        if gs == 8 and ph == 8:
            print("FIGHT REACHED", flush=True)
            for _ in range(8):
                time.sleep(0.4); dump("fight")
            break
        elif gs == 8 and ph in (6,) and sub > 0:
            if now - sent.get('c', 0) >= 1.5:
                press(CROSS, 3, 0); sent['c'] = now
        elif gs == 5:
            stage = "title"
    time.sleep(0.15)
print("done", flush=True)
