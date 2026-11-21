#!/usr/bin/env python3
# stall_dump.py <port> — dump all diagnostic state at a versus ph=8 stall.
import socket, json, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4720

def cmd(c, t=10):
    for _ in range(30):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), 5); break
        except OSError:
            time.sleep(0.2)
    else:
        return {}
    try:
        s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(t); b = b''
        while True:
            x = s.recv(1 << 22)
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
    except Exception as e:
        return {'err': str(e)}

def rd(a, n):
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""

def w32(b, o): return int.from_bytes(b[o:o+4], 'little') if len(b) >= o+4 else None

out = {}
out['frame'] = cmd({"cmd": "frame"})
st = rd(0x1f800000, 0x9d)
out['gs_ph_sub'] = {'gs': st[0] if st else None, 'ph': st[1] if st else None,
                    'sig9b': st[0x9b] if len(st) > 0x9b else None,
                    'sub9c': st[0x9c] if len(st) > 0x9c else None}
for name, c in [
    ('cdrom_state', {"cmd": "cdrom_state"}),
    ('cdrom_history', {"cmd": "cdrom_command_history"}),
    ('bioscall', {"cmd": "bioscall_dump"}),
    ('bios_svc_trace', {"cmd": "bios_svc_trace"}),
    ('irq_state', {"cmd": "irq_state"}),
    ('sp_ring', {"cmd": "sp_ring"}),
    ('dispatch_tail', {"cmd": "dispatch_tail"}),
    ('fn_entry_tail', {"cmd": "fn_entry_tail"}),
    ('fn_stats', {"cmd": "fn_stats"}),
    ('sio_state', {"cmd": "sio_state"}),
    ('sio_trace', {"cmd": "sio_trace"}),
]:
    out[name] = cmd(c)

# memory cells
cells = {
    'cd_desc_801FEB40': (0x801FEB40, 0x40),
    'cd_busy_801BCBA4': (0x801BCBA4, 0x20),
    'cd_drv_B0-D8': (0x801BDECC, 0x20),
    'file_sm_8018CFDC': (0x8018CFDC, 0x40),
    'padrec_P1_801befa8': (0x801befa8, 0x40),
    'task_pool_801F04A8': (0x801F04A8, 0x80),
    'fight_state': (0x1f800000, 0x20),
    'fighterA_801e1b3c': (0x801e1b3c, 0x40),
    'fighterB_801e2c1c': (0x801e2c1c, 0x40),
    'cd_chan_801ff080': (0x801ff080, 0x40),
    'cd_chan_801ff170': (0x801ff170, 0x40),
    'commit_801BE7C0': (0x801BE7C0, 0x40),
}
for name, (a, n) in cells.items():
    d = rd(a, n)
    out[name] = d.hex() if d else None

json.dump(out, open('/tmp/stall_out.json','w'), indent=1)
