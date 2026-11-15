#!/usr/bin/env python3
# bp_watch.py — set pc_break on the DuckStation oracle (:4371), wait for the
# auto-pause hit, dump pc_hit_last, then continue. Repeat N times.
# Usage: bp_watch.py <addr_hex> [hits] [port]
import socket,json,time,sys
PORT=int(sys.argv[3]) if len(sys.argv)>3 else 4371
ADDR=int(sys.argv[1],16)
HITS=int(sys.argv[2]) if len(sys.argv)>2 else 3
def cmd(c,timeout=8):
    for _ in range(60):
        try: s=socket.create_connection(('127.0.0.1',PORT),timeout); break
        except OSError: time.sleep(0.2)
    else: return {}
    s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(timeout); b=b''
    try:
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None or 'valid' in d: break
            except: pass
    except: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except: return {}
def paused():
    # frame advances only when running; detect via two quick frame reads
    f1=cmd({'cmd':'frame'}).get('frame'); time.sleep(0.25)
    f2=cmd({'cmd':'frame'}).get('frame')
    return f1==f2
print('set pc_break 0x%08X ->'%ADDR, cmd({'cmd':'pc_break','addr':hex(ADDR)}))
cmd({'cmd':'continue'})
seen=0; t0=time.time()
while seen<HITS and time.time()-t0<120:
    if paused():
        h=cmd({'cmd':'pc_hit_last'})
        if h.get('valid'):
            seen+=1
            print('HIT %d addr=0x%s pc=0x%s ra=0x%s a0=0x%s a1=0x%s a2=0x%s'%(
                seen,h.get('addr_hit'),h.get('pc'),h.get('ra'),
                h.get('regs',{}).get('a0'),h.get('regs',{}).get('a1'),h.get('regs',{}).get('a2')),flush=True)
            cmd({'cmd':'pc_hit_clear'})
        cmd({'cmd':'continue'})
    else:
        time.sleep(0.2)
print('done, hits=%d'%seen)
cmd({'cmd':'pc_unbreak','addr':hex(ADDR)})
