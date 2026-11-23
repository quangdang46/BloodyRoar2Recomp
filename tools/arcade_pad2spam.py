#!/usr/bin/env python3
# arcade_pad2spam.py — nav to arcade (0,3); during gs=8 spam BOTH slots'
# confirm/dpad continuously so pad-2 edges land whenever arcade's select
# polls them (phases differ from versus). Watch b4/b6 seed + fight progress.
import socket, json, sys, time
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 5370
def cmd(c,timeout=8):
    for _ in range(40):
        try: s=socket.create_connection(('127.0.0.1',PORT),3); break
        except OSError: time.sleep(0.2)
    else: return {}
    try:
        s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(timeout); b=b''
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None or 'error' in d: break
            except: pass
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except: return {}
    except: return {}
def rd(a,n):
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''
def press(b,f,slot=0): return cmd({"cmd":"press","slot":slot,"buttons":b&0xFFFF,"frames":f})
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF; ATK=0xEFFF
t0=time.time(); stage="title"; sent={}; spam_i=0
print("arcade pad2-spam port=%d"%PORT,flush=True)
last_rep=0
while time.time()-t0<600:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d: time.sleep(0.4); continue
    gs,ph=st[0],st[1]; sig=(st[0x9B],st[0x9C]); now=time.time()
    if now-last_rep>1.0:
        last_rep=now
        print("t=%.0f gs/ph=%02x%02x sig=%s b4b6=%s"%(now-t0,gs,ph,sig,rd(0x1f8000b4,4).hex()),flush=True)
    if stage=="title" and gs==5 and ph==1:
        if now-sent.get('t',0)>=0.6: press(START,30,0); sent['t']=now
    if gs==5 and ph==2:
        stage="menu"
        if sig!=(0,3):
            if now-sent.get('d',0)>=0.5: press(DOWN,3,0); sent['d']=now
        else:
            if now-sent.get('s',0)>=1.0: press(START,40,0); sent['s']=now
    elif gs==8:
        stage="ingame"
        # SPAM both slots: alternate CROSS(confirm)/dpad/attack so pad-2 edges fire
        spam_i+=1
        b=[CROSS,DOWN,ATK,START][spam_i%4]
        press(b,6,0); press(b,6,1)
    elif gs==9:
        stage="gs9"; print("*** gs=9 t=%.0f b4b6=%s ***"%(now-t0,rd(0x1f8000b4,4).hex()),flush=True)
        # keep spamming slot1 in case gs9 is still select-ish
        spam_i+=1; press([CROSS,START,ATK][spam_i%3],6,0); press([CROSS,START,ATK][spam_i%3],6,1)
    elif gs!=5 and gs!=8 and gs!=9:
        stage="title"
    time.sleep(0.25)
print("done",flush=True)
