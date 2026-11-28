#!/usr/bin/env python3
# arm_test.py — channel-record ARM oracle.
# Polls slot0 record +0x24 (active flag) @0x801FF0A4 until nonzero or timeout.
# On all-generated builds the record arms within seconds of boot (gs~4);
# on builds with a clean-port culprit in the arm chain it stays 0 forever.
# Usage: arm_test.py <port> [timeout_s]
import socket, json, sys, time

PORT = int(sys.argv[1])
TIMEOUT = float(sys.argv[2]) if len(sys.argv) > 2 else 150.0

def cmd(c, tries=8, to=15):
    for _ in range(tries):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), 2)
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
            time.sleep(0.4)
    return {}

def rec_field():
    r = cmd({"cmd": "read_ram", "addr": "0x801ff080", "len": "0xf0"})
    h = r.get("hex", "")
    if not h:
        return None
    b = bytes.fromhex(h)
    return b

T0 = time.time()
while time.time() - T0 < TIMEOUT:
    b = rec_field()
    if b is not None and len(b) >= 0x60:
        active = b[0x24]
        cmdb = b[0x44:0x48].hex()
        gs = cmd({"cmd": "read_ram", "addr": "0x1f800000", "len": "2"}).get("hex", "")
        gsph = bytes.fromhex(gs) if gs else b""
        print("t=%5.1f gs=%s ph=%s active=%d cmd44=%s" % (
            time.time() - T0,
            gsph[0] if len(gsph) > 0 else -1,
            gsph[1] if len(gsph) > 1 else -1,
            active, cmdb), flush=True)
        if active != 0:
            print("ARMED")
            sys.exit(0)
    time.sleep(2.0)
print("NOT_ARMED")
sys.exit(1)
