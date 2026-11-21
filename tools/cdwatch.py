#!/usr/bin/env python3
# cdwatch.py — poll CD-ROM debug state during versus file load.
# Writes JSONL snapshots to proof/cdwatch_<port>.jsonl.
import socket, json, time, sys, os
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5340
OUT = "proof/cdwatch_%d.jsonl" % PORT

def cmd(c, timeout=5):
    try:
        s = socket.create_connection(('127.0.0.1', PORT), 2)
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
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

t0 = time.time(); last_sig = None
with open(OUT, 'w') as f:
    while time.time() - t0 < 1500:
        st = rd(0x1f800000, 0x9d)
        gs, ph = (st[0], st[1]) if len(st) >= 2 else (-1, -1)
        sig = (gs, ph)
        if sig != last_sig:
            last_sig = sig
            f.write(json.dumps({"t": time.time()-t0, "gs": gs, "ph": ph, "event": "sig_change"}) + "\n"); f.flush()
        # Poll full CD state every ~1.5s once in versus phases (gs==8)
        if gs == 8:
            snap = {"t": time.time()-t0, "gs": gs, "ph": ph}
            for name, c in (("state", {"cmd":"cdrom_state"}),
                            ("hist",  {"cmd":"cdrom_command_history"}),
                            ("trace", {"cmd":"cdrom_trace_dump","count":200}),
                            ("bios",  {"cmd":"bioscall_dump"}),
                            ("svc",   {"cmd":"bios_svc_trace"})):
                r = cmd(c, 8)
                snap[name] = r
            f.write(json.dumps(snap) + "\n"); f.flush()
        # keep connection alive cheaply
        time.sleep(1.5)
print("cdwatch done")
