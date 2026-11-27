#!/usr/bin/env python3
# attract_watch.py — lean continuous watcher: log every gs/ph/s08c/FC_ANI/heap change
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4900
OUT=sys.argv[2] if len(sys.argv)>2 else '/tmp/attract_watch.log'
def cmd(c,timeout=5):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    try: s=socket.create_connection(('127.0.0.1',PORT),2)
    except OSError: return {}
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
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def u32(b,o):
    """Extract a little-endian u32 at offset o (0 if short)."""
    return int.from_bytes(b[o:o+4],'little') if len(b)>=o+4 else 0
def u16(b,o):
    """Extract a little-endian u16 at offset o (0 if short)."""
    return int.from_bytes(b[o:o+2],'little') if len(b)>=o+2 else 0

out=open(OUT,'a')
t0=time.time(); last=None; lastframe=0; lastft=0
while True:
    st=rd(0x1f800000,0x240)
    if len(st)<0x240:
        line="t=%.1f EMU DOWN\n"%(time.time()-t0)
        out.write(line); out.flush(); time.sleep(1); continue
    gs,ph=st[0],st[1]
    s08c=u16(st,0x8c); s08e=u16(st,0x8e)
    s090=u32(st,0x90)&0xffff
    p1ani=u32(st,0x118); p2ani=u32(st,0x220)
    fr=cmd({"cmd":"frame"})
    frame=fr.get('frame',-1)
    if frame==lastframe: pass
    ha=u32(rd(0x801c47b8,4),0); hb=u32(rd(0x801e3cfc,4),0)
    snap=(gs,ph,s08c,s08e,s090,p1ani,p2ani,ha,hb)
    if snap!=last:
        line="t=%.1f f=%d gs=%d ph=%d s08c=%04x s08e=%04x s090=%04x P1ani=%08x P2ani=%08x heapA=%08x heapB=%08x\n"%(time.time()-t0,frame,gs,ph,s08c,s08e,s090,p1ani,p2ani,ha,hb)
        out.write(line); out.flush()
        last=snap; lastframe=frame
    # detect frame freeze (crash/halt)
    if frame==lastft and frame>0:
        fr2=cmd({"cmd":"frame"})
        if fr2.get('frame')==frame:
            out.write("t=%.1f FRAME-FROZEN f=%d gs=%d ph=%d P2ani=%08x heapA=%08x heapB=%08x\n"%(time.time()-t0,frame,gs,ph,p2ani,ha,hb)); out.flush()
            time.sleep(2)
            fr3=cmd({"cmd":"frame"})
            out.write("t=%.1f post-freeze frame=%s\n"%(time.time()-t0,fr3.get('frame','?'))); out.flush()
            if fr3.get('frame')==frame:
                out.write("t=%.1f CONFIRMED-HALT\n"%(time.time()-t0)); out.flush()
                # keep watching state anyway
    lastft=frame
    time.sleep(0.05)
