#!/usr/bin/env python3
# attract_exit_test.py — launch-independent attract-exit probe.
# Presses START on slot0 until gs leaves 6 (title=5) or attempts run out.
# Usage: attract_exit_test.py <port> [max_presses]
import socket, json, sys, time

PORT = int(sys.argv[1])
MAXP = int(sys.argv[2]) if len(sys.argv) > 2 else 12

def cmd(c, tries=10, to=20):
    for _ in range(tries):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), 3)
            s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(to)
            b = b''
            while True:
                x = s.recv(1 << 20)
                if not x: break
                b += x
                try:
                    d = json.loads(b.split(b'\n')[0])
                    if 'hex' in d or d.get('ok') is not None or 'error' in d: break
                except Exception:
                    pass
            s.close()
            return json.loads(b.split(b'\n')[0])
        except Exception:
            time.sleep(0.5)
    return {}

def gs_ph():
    r = cmd({"cmd": "read_ram", "addr": "0x1f800000", "len": "0x10"})
    h = r.get("hex", "")
    if not h:
        return None
    b = bytes.fromhex(h)
    return (b[0], b[1])

# wait for boot into attract (gs==6) — poll up to 120s
for _ in range(240):
    s = gs_ph()
    if s and s[0] == 6:
        break
    time.sleep(0.5)
print("booted:", s, flush=True)

for i in range(MAXP):
    cmd({"cmd": "press", "slot": 0, "buttons": 0xfff7, "frames": 8})
    time.sleep(0.9)
    s = gs_ph()
    print(i, s, flush=True)
    if s and s[0] != 6:
        print("ATTRACT EXITED ->", s)
        sys.exit(0)
print("STILL IN ATTRACT")
sys.exit(1)
