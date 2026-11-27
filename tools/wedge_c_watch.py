#!/usr/bin/env python3
# wedge_c_watch.py — navigate to ARCADE 1P, log DAT1/phase/+0x92 transitions,
# arm wtrace on the battle phase cells, dump on wedge.
import socket, json, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 8932

def cmd(c, timeout=8):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    for _ in range(40):
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
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

def press(btn, frames, slot):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

START=0xFFF7; CROSS=0xBFFF

# arm wtrace on battle phase/+0x92 region EARLY (generated-code writes only)
print(cmd({"cmd":"wtrace_arm","lo":"0x1f800090","hi":"0x1f8000c0"}), flush=True)
print(cmd({"cmd":"wtrace_arm","lo":"0x1f800001","hi":"0x1f800002"}), flush=True)

t0 = time.time(); stage = "title"; sent = {}
last_sig = None; wedge_since = None; dumped = False
log = open("/tmp/wedge_c_%d.log" % PORT, "w", buffering=1)

while time.time() - t0 < 1500:
    st = rd(0x1f800000, 0xa0)
    if len(st) < 0xa0:
        time.sleep(0.5); continue
    gs, ph, sub = st[0], st[1], st[0x9c]
    b92 = int.from_bytes(st[0x92:0x94], "little")
    b94 = int.from_bytes(st[0x94:0x96], "little")
    b9b, b9c = st[0x9b], st[0x9c]
    b4  = int.from_bytes(st[0xb4:0xb6], "little")
    b6  = int.from_bytes(st[0xb6:0xb8], "little")
    b8c = int.from_bytes(st[0x8c:0x8e], "little")
    sig = (gs, ph, b9b, b9c, b92, b4, b6, b8c)
    now = time.time()
    if sig != last_sig:
        el = now - t0
        line = "t=%7.1f gs=%d ph=%d 9b=%d 9c=%d +92=%04x +94=%04x b4=%04x b6=%04x +8c=%04x" % (
            el, gs, ph, b9b, b9c, b92, b94, b4, b6, b8c)
        print(line, flush=True); log.write(line + "\n")
        last_sig = sig
    # navigation (arcade = default top item)
    if stage == "title" and gs == 5 and ph == 1:
        if now - sent.get('t', 0) >= 0.5: press(START, 30, 0); sent['t'] = now
    elif stage in ("title", "menu", "vs_init"):
        if gs == 5 and ph == 2:
            stage = "menu"
            if now - sent.get('s', 0) >= 1.0: press(START, 40, 0); sent['s'] = now
        elif gs == 8:
            stage = "vs_init"
            if ph == 6:
                if now - sent.get('c', 0) >= 1.0:
                    press(CROSS, 30, 0)
                    sent['c'] = now
            elif ph == 7:
                if now - sent.get('g', 0) >= 1.0:
                    press(START, 30, 0); sent['g'] = now
            elif ph == 8:
                stage = "fight"
                print("*** FIGHT ph=8 t=%.0f ***" % (now-t0), flush=True)
        elif gs == 6:
            stage = "title"
    elif stage == "fight":
        pass
    # wedge detect: gs==8 ph==3 persisting with no sig change for >20s
    if gs == 8 and ph == 3:
        if wedge_since is None: wedge_since = now
        elif now - wedge_since > 20 and not dumped:
            dumped = True
            print("*** WEDGE ph=3 sustained %.0fs — dumping ***" % (now-wedge_since), flush=True)
            print(cmd({"cmd":"wtrace_dump","count":400,"newest":1,"addr_lo":"0x1f800000","addr_hi":"0x1f800100"}, 20), flush=True)
            print(cmd({"cmd":"fn_entry_tail","count":64}, 15), flush=True)
            print(cmd({"cmd":"sp_ring","count":64}, 15), flush=True)
    else:
        wedge_since = None
        if dumped and gs == 8 and ph != 3:
            dumped = False  # recovered; re-arm dump for next wedge
    time.sleep(0.25)
print("done", flush=True)
