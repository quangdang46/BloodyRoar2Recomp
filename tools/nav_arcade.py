#!/usr/bin/env python3
# nav_arcade.py — drive headless BloodyRoar2_Recompiled (debug :4370) title ->
# mode menu -> ARCADE (1P, first item) -> char select -> fight. Input-only.
# Active-low pad words (proven): START=0xFFF7 CROSS=0xBFFF DOWN=0xFFBF.
# Arcade = mode-menu default (no DOWN). Reaches gs=8 ph=8 with populated
# fighters -> triggers the fight-init CD file-load / TestEvent(0xF0000009)
# wedge that wtrace/fntrace are armed to capture.
import socket, json, time, sys

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4370

def cmd(c):
    for _ in range(50):
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

def press(btn, frames, slot):
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

START = 0xFFF7; CROSS = 0xBFFF; DOWN = 0xFFBF
# mode-menu sig: arcade is the default top item. We press START on whatever
# is selected first (no DOWN) -> arcade/1P. Char select: CROSS locks P1.

t0 = time.time(); stage = "title"; sent = {}
while time.time() - t0 < 180:
    st = rd(0x1f800000, 0x9d)
    if len(st) < 0x9d:
        time.sleep(0.3); continue
    gs, ph, sub = st[0], st[1], st[0x9C]; sig = (st[0x9B], st[0x9C])
    print("stage=%s gs=%d ph=%d sub=%d sig=%s" % (stage, gs, ph, sub, sig), flush=True)
    now = time.time()
    if stage == "title":
        # mash START until mode menu (gs=5 ph=2) or straight into gs=8
        if gs == 5 and ph == 2:
            stage = "menu"
        elif gs == 8:
            stage = "arcade_init"
        elif now - sent.get('t', 0) >= 0.8:
            press(START, 3, 0); sent['t'] = now
    elif stage == "menu":
        # mode menu: confirm the DEFAULT (top) item = arcade/1P
        if gs == 5 and ph == 2:
            if now - sent.get('s', 0) >= 0.8:
                press(START, 3, 0); sent['s'] = now; time.sleep(0.3)
        elif gs == 8:
            stage = "arcade_init"
        elif gs != 5:
            stage = "title"
    elif stage == "arcade_init":
        # arcade: ph walks 0->3->6->8; char-select confirm on CROSS (P1)
        # then fight. Watch for ph==8 (fight live) or a stall.
        if gs == 8 and ph == 8:
            print("FIGHT REACHED gs=8 ph=8", flush=True)
            # keep watching a few more samples to confirm it holds / wedges
            for _ in range(6):
                time.sleep(0.5)
                st2 = rd(0x1f800000, 4)
                if len(st2) >= 2:
                    print("  ... gs=%d ph=%d" % (st2[0], st2[1]), flush=True)
            break
        elif gs == 8 and ph in (6,) and sub > 0:
            # char select: lock P1 with CROSS
            if now - sent.get('c', 0) >= 1.5:
                press(CROSS, 3, 0); sent['c'] = now
        elif gs == 5:
            stage = "title"
    time.sleep(0.15)
print("done", flush=True)
