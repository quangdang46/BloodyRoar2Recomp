#!/usr/bin/env python3
# versus_crash_watch.py — nav title->menu->2P VS->char lock->start->ph=8,
# then KEEP POLLING (never sends quit) until the emulator dies / crashes.
# On ph=8, snapshots ev-stream dispatch state and watches for process death.
import socket, json, struct, sys, time, os
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5330

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

START = 0xFFF7; CROSS = 0xBFFF; DOWN = 0xFFBF
t0 = time.time(); stage = "title"; sent = {}
fight_frame = None; last_fr = None; last_chg = time.time()
ev_seen = set()
print("watch start", flush=True)
while time.time() - t0 < 1200:
    st = rd(0x1f800000, 0x9d)
    if len(st) < 0x9d:
        # emulator may be dead — retry a few times before declaring death
        ok = False
        for _ in range(10):
            fr = cmd({"cmd": "frame"}, 15).get("frame")
            if fr is not None:
                ok = True; break
            time.sleep(1.0)
        if not ok:
            print("EMULATOR UNREACHABLE t=%.0f" % (time.time()-t0), flush=True)
            break
        time.sleep(0.4); continue
    gs, ph, sub = st[0], st[1], st[0x9C]; sig = (st[0x9B], st[0x9C])
    fr = cmd({"cmd": "frame"}, 5).get("frame")
    now = time.time()
    if stage != "fight" or (fr != last_fr):
        print("t=%.0f stage=%s gs=%d ph=%d sub=%d sig=%s frame=%s" % (now-t0, stage, gs, ph, sub, sig, fr), flush=True)
    if gs == 5 and ph == 1:
        if now - sent.get('t', 0) >= 0.5: press(START, 30, 0); sent['t'] = now
    elif gs == 5 and ph == 2:
        stage = "menu"
    elif stage == "menu":
        if gs == 5 and ph == 2:
            if sig != (2, 1):
                if now - sent.get('d', 0) >= 0.5: press(DOWN, 3, 0); sent['d'] = now
            else:
                if now - sent.get('s', 0) >= 1.0: press(START, 40, 0); sent['s'] = now
        elif gs == 8: stage = "vs_init"
        elif gs != 5: stage = "title"
    elif stage in ("title", "vs_init"):
        if gs == 8:
            stage = "vs_init"
            if ph == 6:
                for s in (0, 1):
                    if now - sent.get(('c', s), 0) >= 1.0: press(CROSS, 30, s); sent[('c', s)] = now
            elif ph == 7:
                for s in (0, 1):
                    if now - sent.get(('g', s), 0) >= 1.0: press(START, 30, s); sent[('g', s)] = now
            elif ph == 8:
                stage = "fight"
                if fight_frame is None:
                    fight_frame = fr; last_chg = now
                    print("*** FIGHT REACHED frame=%s t=%.0f ***" % (fr, now-t0), flush=True)
    elif stage == "fight":
        if fr != last_fr:
            last_fr = fr; last_chg = now
        # poll decomp_trace occasionally for tag-60 ev dispatch ops
        if int(now) % 10 == 0:
            r = cmd({"cmd": "decomp_trace", "count": 400}, 6)
            ents = (r.get("ring") or {}).get("entries") or []
            for e in ents:
                # tag names are strings; ev dispatch records tag "60"-> look for op field
                pass
        if last_chg and now - last_chg > 20:
            print("FIGHT STALL frame=%s t=%.0f" % (last_fr, now-t0), flush=True)
            # keep watching a bit more
            last_chg = now
    elif gs == 6:
        stage = "title"
    time.sleep(0.3)
print("watch done", flush=True)
