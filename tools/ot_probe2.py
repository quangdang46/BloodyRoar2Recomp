#!/usr/bin/env python3
import socket,json,time,sys
PORT=5250
def cmd(c):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    try:
        s=socket.create_connection(('127.0.0.1',PORT),1); s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(1); b=b''
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None: break
            except: pass
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except: return {}
    except Exception: return {}
def rd(a,n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r=cmd({'cmd':'read_ram','addr':hex(a),'len':hex(n)})
    return bytes.fromhex(r['hex']) if r.get('ok') and r.get('hex') else b''
def w(b,o=0):
    """Extract a little-endian u32 word at offset o (0 if short)."""
    return int.from_bytes(b[o:o+4],'little') if len(b)>=o+4 else 0
t0=time.time(); last=''
while time.time()-t0<170:
    st=rd(0x1f800000,0xa4)
    if len(st)<0xa4: time.sleep(0.05); continue
    gs,ph=st[0],st[1]
    if gs!=8: time.sleep(0.05); continue
    ob=w(rd(0x1f800008,4)); idx=w(rd(0x1f80000c,4))
    o0=w(rd(0x801e4110+0x58,4)); o1=w(rd(0x801e6260+0x58,4))
    fr=cmd({'cmd':'frame'}).get('frame')
    line="t=%.2f ph=%d f=%s ob=%08x idx=%d O0=%06x O1=%06x"%(time.time()-t0,ph,fr,ob,idx,o0&0xffffff,o1&0xffffff)
    if line!=last: print(line,flush=True); last=line
    time.sleep(0.05)
print("done")
