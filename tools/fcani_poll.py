#!/usr/bin/env python3
# fcani_poll.py — rapid poll FC_ANI + heap cursors + periodic wtrace/pc_probe dumps
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4890
def cmd(c,timeout=10):
    for _ in range(80):
        try: s=socket.create_connection(('127.0.0.1',PORT),3); break
        except OSError: time.sleep(0.1)
    else: return {}
    s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(timeout); b=b''
    try:
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'ok' in d or 'error' in d: break
            except: pass
    except Exception: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except: return {}
def rd(a,n):
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def u32(b,o): return int.from_bytes(b[o:o+4],'little') if len(b)>=o+4 else 0
def u16(b,o): return int.from_bytes(b[o:o+2],'little') if len(b)>=o+2 else 0

out=open('/tmp/fcani_poll.log','w')
t0=time.time(); last=None; lastdump=0
while time.time()-t0<240:
    st=rd(0x1f800000,0x240)
    if len(st)<0x240: time.sleep(0.2); continue
    gs,ph=st[0],st[1]
    s08c=u16(st,0x8c); s08e=u16(st,0x8e)
    p1ani=u32(st,0x118); p2ani=u32(st,0x220)
    ha=u32(rd(0x801c47b8,4),0); hb=u32(rd(0x801e3cfc,4),0)
    snap=(gs,ph,s08c,s08e,p1ani,p2ani,ha,hb)
    if snap!=last:
        line="t=%.1f gs=%d ph=%d s08c=%04x s08e=%04x P1ani=%08x P2ani=%08x heapA=%08x heapB=%08x\n"%(time.time()-t0,gs,ph,s08c,s08e,p1ani,p2ani,ha,hb)
        out.write(line); out.flush()
        last=snap
    if time.time()-lastdump>3:
        lastdump=time.time()
        w=cmd({"cmd":"wtrace_dump","count":200})
        out.write("WTRACE "+json.dumps(w)[:200000]+"\n"); out.flush()
        p=cmd({"cmd":"pc_probe_dump"})
        out.write("PCPROBE "+json.dumps(p)[:200000]+"\n"); out.flush()
    time.sleep(0.15)
out.close()
