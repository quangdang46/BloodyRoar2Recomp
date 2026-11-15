#!/usr/bin/env python3
# nav_to_fight_oracle.py — drive the DuckStation oracle (debug :4371) from
# title -> mode menu -> 2P VS -> char select -> fight. Same input sequence as
# nav_to_fight.py but reads game state via `read_scratch` (the oracle's
# read_ram does not expose the 0x1f800000 scratchpad region; it returns zeros).
# State layout is identical: gs=st[0], ph=st[1], sig=(st[0x9B],st[0x9C]).
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4371
def cmd(c):
    for _ in range(50):
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
def rd_state(n=0x9d):
    r=cmd({"cmd":"read_scratch","offset":0,"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def press(btn,frames,slot): return cmd({"cmd":"press","slot":slot,"buttons":btn&0xFFFF,"frames":frames})
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF
t0=time.time(); stage="title"; sent={}
while time.time()-t0<240:
    st=rd_state()
    if len(st)<0x9d: time.sleep(0.3); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    print("stage=%s gs=%d ph=%d sub=%d sig=%s"%(stage,gs,ph,sub,sig),flush=True)
    now=time.time()
    if stage=="title":
        if gs==5 and ph==1: press(START,3,0); time.sleep(0.3)
        elif gs==5 and ph==2: stage="menu"
        elif gs==8: stage="vs_init"
        elif gs not in (4,5,8): stage="title"
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
            print("FIGHT REACHED",flush=True); break
        elif gs==5: stage="title"
    time.sleep(0.15)
