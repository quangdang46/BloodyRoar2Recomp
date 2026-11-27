#!/usr/bin/env python3
# ph_flicker_watch.py — versus nav + continuous gs/ph sampling + gwatch arming.
# Captures the intermittent DAT_1f800001 8->1 write. gwatch stderr mirror
# (GW ... RP=<host rip>) survives FAIL-FAST; ring dumps polled as backup.
import socket,json,time,sys,os
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4522
OUT=sys.argv[2] if len(sys.argv)>2 else "/tmp/ph_flicker_capture.jsonl"
def cmd(c,timeout=10):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
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
                if 'hex' in d or d.get('ok') is not None or 'entries' in d: break
            except: pass
    except: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except: return {}
def rd(a,n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def press(btn,frames,slot):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd":"press","slot":slot,"buttons":btn&0xFFFF,"frames":frames})
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF

out=open(OUT,"a",buffering=1)
def log(ev,**kw):
    """Append a timestamped line to the log file."""
    rec={"t":round(time.time(),3),"ev":ev}; rec.update(kw)
    out.write(json.dumps(rec)+"\n")
    print(json.dumps(rec),flush=True)

armed=False
# arm gwatch as soon as the server is up — scratchpad window covers gs/ph.
for _ in range(120):
    r=cmd({"cmd":"ping"},3)
    if r: break
    time.sleep(0.5)
log("gwatch_arm",r=cmd({"cmd":"gwatch","addr":"0x1f800000","len":16}))
armed=True
log("wtrace_arm",r=cmd({"cmd":"wtrace_range","lo":"0x1f800000","hi":"0x1f800010"}))

t0=time.time(); stage="title"; sent={}
last=None; last_dump=0.0; fight_mash=0.0
while time.time()-t0<420:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        log("read_fail"); time.sleep(0.5); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    cur=(gs,ph)
    now=time.time()
    if cur!=last:
        log("state",gs=gs,ph=ph,sub=sub,sig=sig,dt=round(now-t0,2))
        last=cur
        # interesting transition -> immediate ring dump
        if last and (gs,ph)!=(8,8) or (last and gs!=8):
            d=cmd({"cmd":"gwatch_dump","count":60,"newest":1})
            log("gwatch_dump",n=len(d.get("entries",[])),entries=d.get("entries",[])[-15:])
    # periodic ring drain so nothing is lost before a crash
    if now-last_dump>1.0:
        last_dump=now
        d=cmd({"cmd":"gwatch_dump","count":40,"newest":1})
        ents=d.get("entries",[])
        if ents:
            log("gwatch_poll",total=d.get("total"),n=len(ents),tail=ents[-8:])
    # nav state machine (from nav_to_fight.py)
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
            stage="fight"
            log("fight_reached")
        elif gs==5: stage="title"
    elif stage=="fight":
        # gentle input to keep the bout advancing; avoid START (pause).
        if now-fight_mash>6.0:
            fight_mash=now
            press(CROSS,4,0)
    time.sleep(0.15)
log("done")
