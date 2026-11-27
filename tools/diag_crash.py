#!/usr/bin/env python3
# diag_crash.py — poll debug :4382 and dump the versus->fight crash diagnostics.
# Captures evcb snapshot, spu_ta, hle_dump tail, cdrom state+command history,
# registers, and the callback/event tables in a tight loop to catch pre-crash.
import socket, json, time, sys

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4382
OUT  = sys.argv[2] if len(sys.argv) > 2 else "/tmp/diag_crash.jsonl"

def cmd(c, timeout=6):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    try:
        s = socket.create_connection(('127.0.0.1', PORT), timeout)
        s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(timeout)
        b = b''
        while True:
            try: x = s.recv(1 << 20)
            except socket.timeout: break
            if not x: break
            b += x
            try:
                d = json.loads(b.split(b'\n')[0])
                if 'ok' in d or 'hex' in d or 'addrs' in d or 'entries' in d: break
            except Exception: pass
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except Exception: return {'raw': b[:2000].decode('utf8', 'replace')}
    except Exception as e:
        return {'err': str(e)}

def rd(a, n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r['hex']) if r.get('ok') and r.get('hex') else b''

fout = open(OUT, 'w')
t0 = time.time()
seen_fight = False
while time.time() - t0 < 240:
    f = cmd({'cmd': 'frame'}).get('frame')
    st = rd(0x1f800000, 0x9d)
    gs = st[0] if len(st) > 0 else -1
    ph = st[1] if len(st) > 1 else -1
    rec = {'t': round(time.time() - t0, 2), 'frame': f, 'gs': gs, 'ph': ph}
    if gs == 8:
        seen_fight = True
        # fight region: dump the diagnostic set once gs==8, then keep polling frame
        if 'dumped' not in rec:
            rec['regs'] = cmd({'cmd': 'get_registers'})
            rec['evcb'] = cmd({'cmd': 'evcb_snapshot'})
            rec['spu_ta'] = cmd({'cmd': 'spu_ta'})
            rec['hle'] = cmd({'cmd': 'hle_dump', 'tail': 60})
            rec['cdrom'] = cmd({'cmd': 'cdrom_state'})
            rec['cdhist'] = cmd({'cmd': 'cdrom_command_history', 'count': 50})
            # callback/event tables
            rec['cb_tbl'] = rd(0x801bddc0, 0x40).hex()
            rec['evcb_area'] = rd(0x800008e0, 0x40).hex()
            rec['dumped'] = True
    fout.write(json.dumps(rec) + '\n'); fout.flush()
    if 'err' in cmd({'cmd': 'frame'}) or f is None:
        # process may have died; keep trying a bit then bail
        if time.time() - t0 > 5 and seen_fight:
            pass
    time.sleep(0.4)
fout.close()
print('done ->', OUT)
