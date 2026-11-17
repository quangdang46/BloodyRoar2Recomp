#!/usr/bin/env python3
# versus nav + ph watcher: arms wtrace_range + gwatch when ph>=6, dumps on flicker/crash
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4512
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
def press(btn,frames,slot): return cmd({"cmd":"press","slot":slot,"buttons":btn&0xFFFF,"frames":frames})
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF
armed={'w':False,'g':False}
t0=time.time(); stage="title"; sent={}
last_ph=None; flick=0
while time.time()-t0<240:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        time.sleep(0.3); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    now=time.time()
    if last_ph is not None and ph!=last_ph and gs==8:
        print("PHCHANGE %d->%d gs=%d sub=%d t=%.1f"%(last_ph,ph,gs,sub,now-t0),flush=True)
        if ph==1 or (last_ph==8 and ph!=8): flick+=1
    last_ph=ph
    # arm probes once versus select/fight region reached
    if gs==8 and ph>=5 and not armed['w']:
        print("ARM wtrace_range", cmd({"cmd":"wtrace_range","lo":"0x1f800000","hi":"0x1f800010"}),flush=True)
        armed['w']=True
    if gs==8 and ph>=6 and not armed['g']:
        print("ARM gwatch", cmd({"cmd":"gwatch","addr":"0x1f800001","len":16}),flush=True)
        armed['g']=True
    print("stage=%s gs=%d ph=%d sub=%d sig=%s armed=%s"%(stage,gs,ph,sub,sig,armed),flush=True)
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
            print("FIGHT REACHED flick=%d"%flick,flush=True)
            # keep watching for flicker a while
            time.sleep(5)
            break
        elif gs==5: stage="title"
    time.sleep(0.15)
# dump traces
print("=== wtrace_dump ===",flush=True)
print(json.dumps(cmd({"cmd":"wtrace_dump","count":200}))[:6000],flush=True)
print("=== gwatch_dump ===",flush=True)
print(json.dumps(cmd({"cmd":"gwatch_dump","count":100,"newest":1}))[:8000],flush=True)
