#!/usr/bin/env python3
# sibling_watch.py — arm gwatch+watch on already-running versus-fight
# instances and poll for the ph 8->1 flicker; dump rings on change.
import socket,json,time,sys
PORTS=[int(x) for x in sys.argv[1:]] or [4397,4401,4404,4405,4520]
def conn(p):
    """Open a TCP connection to the debug server, retrying briefly."""
    for _ in range(30):
        try: return socket.create_connection(('127.0.0.1',p),5)
        except OSError: time.sleep(0.3)
    return None
def cmd(p,c):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    s=conn(p)
    if not s: return {}
    try:
        s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(8); b=b''
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None or 'entries' in d: break
            except: pass
    except: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except: return {}
def rd(p,a,n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r=cmd(p,{"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
armed={}
for p in PORTS:
    r=cmd(p,{"cmd":"gwatch","addr":"0x1f800000","len":16})
    armed[p]=r
    print(p,"gwatch",r,flush=True)
t0=time.time(); last={}
while time.time()-t0<600:
    for p in PORTS:
        st=rd(p,0x1f800000,4)
        if len(st)<4: print(p,"dead?",flush=True); continue
        gs,ph=st[0],st[1]
        if last.get(p)!=(gs,ph):
            print("PORT",p,"gs",gs,"ph",ph,"t",round(time.time()-t0,1),flush=True)
            last[p]=(gs,ph)
            if (gs,ph)!=(8,8):
                d=cmd(p,{"cmd":"gwatch_dump","count":80,"newest":1})
                ents=d.get("entries",[])
                print("  DUMP total",d.get("total"),"n",len(ents),flush=True)
                for e in ents[-20:]:
                    print("   ",e.get("addr"),e.get("rip"),e.get("store_pc"),e.get("func"),"old",e.get("old","")[:20],"new",e.get("new","")[:20],flush=True)
    time.sleep(0.4)
