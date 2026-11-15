#!/usr/bin/env python3
# dump_postmortem.py — after the versus->fight crash, dump the full diagnostic
# set from the (still-live) debug server: registers, CD state+history, SPU TA
# ring, HLE kernel-call ring, EvCB snapshot/walk, DMA trace, event ring.
import socket, json, sys

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4382
OUT  = sys.argv[2] if len(sys.argv) > 2 else "/tmp/postmortem.json"

def cmd(c, timeout=8):
    try:
        s = socket.create_connection(('127.0.0.1', PORT), timeout)
        s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(timeout)
        b = b''
        while True:
            try: x = s.recv(1 << 22)
            except socket.timeout: break
            if not x: break
            b += x
            try:
                d = json.loads(b.split(b'\n')[0])
                if 'ok' in d: break
            except Exception: pass
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except Exception: return {'raw': b[:4000].decode('utf8','replace')}
    except Exception as e:
        return {'err': str(e)}

queries = [
    ('frame',        {'cmd':'frame'}),
    ('regs',         {'cmd':'get_registers'}),
    ('irq_state',    {'cmd':'irq_state'}),
    ('vblank_rate',  {'cmd':'vblank_rate'}),
    ('cdrom_state',  {'cmd':'cdrom_state'}),
    ('cdrom_hist',   {'cmd':'cdrom_command_history','count':80}),
    ('cdrom_trace',  {'cmd':'cdrom_trace_dump','count':80}),
    ('spu_ta',       {'cmd':'spu_ta','count':400}),
    ('spu_status',   {'cmd':'spu_status'}),
    ('spu_events',   {'cmd':'spu_events','count':120}),
    ('hle_dump',     {'cmd':'hle_dump','tail':120}),
    ('evcb_snap',    {'cmd':'evcb_snapshot'}),
    ('evcb_walk',    {'cmd':'evcb_walk_dump','count':40}),
    ('dma_state',    {'cmd':'dma_state'}),
    ('dma_trace',    {'cmd':'dma_trace_dump','count':120}),
    ('event_ring',   {'cmd':'event_ring_dump','count':200}),
    ('bios_info',    {'cmd':'bios_info'}),
]
out = {}
for name, q in queries:
    r = cmd(q)
    out[name] = r
    print(f"{name}: {'ERR '+str(r.get('err')) if 'err' in r else 'ok len='+str(len(json.dumps(r)))}", flush=True)
json.dump(out, open(OUT,'w'), indent=1)
print('wrote', OUT)
