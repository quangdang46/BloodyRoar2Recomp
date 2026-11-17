#!/usr/bin/env python3
# fcani_watch.py — poll FC_ANI cells + heap cursors during versus nav
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4890
def cmd(c):
    for _ in range(60):
        try: s=socket.create_connection(('127.0.0.1',PORT),10); break
        except OSError: time.sleep(0.2)
    else: return {}
    s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(10); b=b''
    try:
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None: break
            except: pass
    except: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except: return {}
def rd(a,n):
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def u32(b,o): return int.from_bytes(b[o:o+4],'little') if len(b)>=o+4 else 0
def u16(b,o): return int.from_bytes(b[o:o+2],'little') if len(b)>=o+2 else 0

t0=time.time(); last=None
while time.time()-t0<300:
    st=rd(0x1f800000,0x240)
    if len(st)<0x240: time.sleep(0.3); continue
    gs,ph=st[0],st[1]
    s08c=u16(st,0x8c); s08e=u16(st,0x8e)
    p1ani=u32(st,0x118); p2ani=u32(st,0x220)
    p1q=u32(st,0x114); p2q=u32(st,0x21c)
    ha=u32(rd(0x801c47b8,4),0); hb=u32(rd(0x801e3cfc,4),0)
    snap=(gs,ph,s08c,s08e,p1ani,p2ani,ha,hb)
    if snap!=last:
        h1=h2=b''
        if 0x80010000<=p1ani<0x80100000: h1=rd(p1ani,16).hex()
        if 0x80010000<=p2ani<0x80100000: h2=rd(p2ani,16).hex()
        print("t=%.1f gs=%d ph=%d s08c=%04x s08e=%04x P1ani=%08x P2ani=%08x heapA=%08x heapB=%08x"%(time.time()-t0,gs,ph,s08c,s08e,p1ani,p2ani,ha,hb),flush=True)
        print("   P1head=%s P2head=%s"%(h1,h2),flush=True)
        last=snap
    time.sleep(0.25)
