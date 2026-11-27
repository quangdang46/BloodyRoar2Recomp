#!/usr/bin/env python3
# sibling_watch2.py — poll gwatch_dump on live versus instances; report every
# NEW write to 0x1F800001 (ph byte) or 0x1F800000 (gs byte) with rip.
import socket,json,time,sys
PORTS=[int(x) for x in sys.argv[1:]] or [4397,4401,4404,4405,4520]
def conn(p):
    """Open a TCP connection to the debug server, retrying briefly."""
    for _ in range(20):
        try: return socket.create_connection(('127.0.0.1',p),5)
        except OSError: time.sleep(0.3)
    return None
def cmd(p,c):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    s=conn(p)
    if not s: return {}
    try:
        s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(10); b=b''
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
seen={p:set() for p in PORTS}
# seed: mark current ring contents as seen
t0=time.time()
for p in PORTS:
    d=cmd(p,{"cmd":"gwatch_dump","count":400,"newest":1})
    for e in d.get("entries",[]):
        if e.get("addr") in ("0x1F800000","0x1F800001"): seen[p].add(e.get("seq"))
    print(p,"seeded total",d.get("total"),"seen",len(seen[p]),flush=True)
while time.time()-t0<900:
    for p in PORTS:
        d=cmd(p,{"cmd":"gwatch_dump","count":200,"newest":1})
        ents=d.get("entries",[])
        new=[e for e in ents if e.get("addr") in ("0x1F800000","0x1F800001") and e.get("seq") not in seen[p]]
        for e in new: seen[p].add(e.get("seq"))
        for e in new:
            nv=e.get("new","")
            try: ph=int(nv[0:2],16) if nv else -1
            except: ph=-1
            print("PORT",p,"t",round(time.time()-t0,1),"addr",e.get("addr"),"rip",e.get("rip"),
                  "store_pc",e.get("store_pc"),"func",e.get("func"),"phbyte",ph,
                  "old",e.get("old","")[:24],"new",e.get("new","")[:24],
                  "frame",e.get("frame"),flush=True)
        if not ents and d.get("total")==0:
            pass
    time.sleep(1.0)
