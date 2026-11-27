#!/usr/bin/env python3
# ani_stomp_watch.py v3 — versus nav + gwatch on the TWO stream-PC cells
# (P1 motion 0x801F5328+0x84=0x801F53AC, P2 0x801F5440+0x84=0x801F54C4),
# heap cursors, and both ctrl+0x30 (.ANI base) cells. Cold cells only —
# no big .ANI range (that flooded the ring last run).
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4880
OUT=sys.argv[2] if len(sys.argv)>2 else "/tmp/ani_stomp_capture.jsonl"
def cmd(c,timeout=15):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    for _ in range(80):
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
                if 'hex' in d or d.get('ok') is not None or 'entries' in d: break
            except Exception: pass
    except Exception: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except Exception: return {}
def rd(a,n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def w32(a):
    """Read a 32-bit guest word at address a (None on failure)."""
    b=rd(a,4)
    return int.from_bytes(b,'little') if len(b)==4 else None
def press(btn,frames,slot):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd":"press","slot":slot,"buttons":btn&0xFFFF,"frames":frames})
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF

out=open(OUT,"a",buffering=1)
def log(ev,**kw):
    """Append a timestamped line to the log file."""
    rec={"t":round(time.time(),3),"ev":ev}; rec.update(kw)
    out.write(json.dumps(rec)+"\n")
    print(json.dumps(rec)[:1500],flush=True)

for _ in range(150):
    r=cmd({"cmd":"ping"},3)
    if r: break
    time.sleep(0.5)
log("gw_p1pc",r=cmd({"cmd":"gwatch","addr":"0x801f53ac","len":4}))
log("gw_p2pc",r=cmd({"cmd":"gwatch","addr":"0x801f54c4","len":4}))
log("gw_heapB",r=cmd({"cmd":"gwatch","addr":"0x801e3cfc","len":8}))
log("gw_heapA",r=cmd({"cmd":"gwatch","addr":"0x801c47b8","len":8}))
log("gw_p1ani",r=cmd({"cmd":"gwatch","addr":"0x1f800118","len":8}))
log("gw_p2ani",r=cmd({"cmd":"gwatch","addr":"0x1f800220","len":8}))
log("armed")

t0=time.time(); stage="title"; sent={}
last=None; last_dump=0.0
prev={}
CELLS={"p1pc":0x801f53ac,"p2pc":0x801f54c4,"p1ani":0x1f800118,"p2ani":0x1f800220,
       "heapB":0x801e3cfc,"heapA":0x801c47b8}
while time.time()-t0<600:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        log("read_fail"); time.sleep(1.0); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    cur=(gs,ph)
    now=time.time()
    vals={k:w32(a) for k,a in CELLS.items()}
    for k,v in vals.items():
        if v!=prev.get(k):
            log("cell",k=k,val=hex(v) if v else v,gs=gs,ph=ph,sub=sub,
                frame=cmd({"cmd":"frame"}).get("frame"))
            prev[k]=v
    if cur!=last:
        log("state",gs=gs,ph=ph,sub=sub,sig=sig,dt=round(now-t0,2),vals={k:(hex(v) if v else v) for k,v in vals.items()})
        last=cur
    if now-last_dump>10.0:
        last_dump=now
        d=cmd({"cmd":"gwatch_dump","count":60,"newest":1})
        ents=d.get("entries",[])
        if ents:
            log("gwatch_poll",total=d.get("total"),oor=d.get("oor"),tail=ents[-5:])
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
            log("fight_reached"); stage="fight"
        elif gs==5: stage="title"
    elif stage=="fight":
        if now-sent.get('m',0)>6.0:
            sent['m']=now; press(CROSS,4,0)
    time.sleep(0.15)
log("done")
