#!/usr/bin/env python3
# heapA cursor audit driver: nav to versus fight while arming gwatch on
# the heapA cursor cell 0x801C47B8 (+heapB 0x801E3CFC) once gs==8 is seen,
# then dump every cursor write with host-RIP attribution.
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4771
def cmd(c):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    for _ in range(60):
        try: s=socket.create_connection(('127.0.0.1',PORT),10); break
        except OSError: time.sleep(0.2)
    else: return {}
    s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(15); b=b''
    try:
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None: break
            except Exception: pass
    except Exception: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except Exception: return {}
def rd(a,n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def press(btn,frames,slot):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd":"press","slot":slot,"buttons":btn&0xFFFF,"frames":frames})
def w32(a):
    """Read a 32-bit guest word at address a (None on failure)."""
    b=rd(a,4)
    return int.from_bytes(b,'little') if len(b)==4 else None
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF
armed=False
t0=time.time(); stage="title"; sent={}
last_dump=0
while time.time()-t0<240:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        time.sleep(0.3); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    cur=w32(0x801c47b8); hb=w32(0x801e3cfc)
    print("t=%5.1f stage=%s gs=%d ph=%d sub=%d sig=%s heapA=%s heapB=%s"%(
        time.time()-t0,stage,gs,ph,sub,sig,
        hex(cur) if cur else '?',hex(hb) if hb else '?'),flush=True)
    if not armed and gs==8:
        print("ARMING gwatch",cmd({"cmd":"gwatch","addr":"0x801c47b8","len":8}),flush=True)
        print("ARMING gwatch",cmd({"cmd":"gwatch","addr":"0x801e3cfc","len":8}),flush=True)
        armed=True
    now=time.time()
    if armed and now-last_dump>10:
        d=cmd({"cmd":"gwatch_dump","count":400,"newest":1})
        ents=d.get("entries",[])
        print("  gwatch total=%s n=%d tail=%s"%(d.get("total"),len(ents),
              json.dumps(ents[-3:])[:600]),flush=True)
        last_dump=now
    if stage=="title":
        if gs==5 and ph==1: press(START,3,0); time.sleep(0.3)
        elif gs==5 and ph==2: stage="menu"
        elif gs==8: stage="vs_init"
    elif stage=="menu":
        if gs==5 and ph==2:
            if sig!=(2,1):
                if now-sent.get('d',0)>=0.4: press(DOWN,3,0); sent['d']=now
            else:
                if now-sent.get('s',0)>=0.8: press(START,3,0); sent['s']=now; time.sleep(0.3)
        elif gs==8: stage="vs_init"
        elif gs!=5: stage="title"
    elif stage=="vs_init":
        if gs==8 and ph==6 and sub>0:
            for s in (0,1):
                if now-sent.get(s,0)>=1.5: press(CROSS,3,s); time.sleep(0.15); sent[s]=now
        elif gs==8 and ph==7:
            for s in (0,1):
                if now-sent.get(('g',s),0)>=1.0: press(START,3,s); sent[('g',s)]=now
        elif gs==8 and ph==8:
            print("FIGHT REACHED",flush=True)
            # keep sampling a bit then dump
            time.sleep(5)
            break
        elif gs==5: stage="title"
    time.sleep(0.15)
print("=== final dump ===",flush=True)
d=cmd({"cmd":"gwatch_dump","count":2000,"newest":1})
ents=d.get("entries",[])
print("total=%s dumped=%d"%(d.get("total"),len(ents)),flush=True)
for e in ents:
    print(json.dumps(e),flush=True)
