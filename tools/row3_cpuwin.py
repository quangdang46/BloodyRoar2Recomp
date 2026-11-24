#!/usr/bin/env python3
# row3_cpuwin.py — row3 (1,4), P1 confirm, then DO NOT FIGHT (let CPU side1 win).
# Watch whether tally (dat1=0b) completes when side1 wins the match.
import socket, json, sys, time
PORT=int(sys.argv[1]); SECS=int(sys.argv[2]) if len(sys.argv)>2 else 200
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF
def cmd(c, timeout=8):
    for _ in range(40):
        try:
            s=socket.create_connection(('127.0.0.1',PORT),3); break
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
            except Exception: pass
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except Exception: return {}
    except Exception: return {}
def rd(a,n):
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''
def press(b,f,slot=0): return cmd({"cmd":"press","slot":slot,"buttons":b&0xFFFF,"frames":f})
def state():
    st=rd(0x1f800000,0xb8)
    if len(st)<0xb8: return None
    return {'gs':st[0],'dat1':st[1],'sig':(st[0x9b],st[0x9c]),
            'b4':int.from_bytes(st[0xb4:0xb6],'little'),'b6':int.from_bytes(st[0xb6:0xb8],'little'),
            'e0':rd(0x1f8000e8+0xe4,1).hex(),'e1':rd(0x1f8001f0+0xe4,1).hex(),
            'n99':rd(0x1f800099,1).hex(),'p92':int.from_bytes(st[0x92:0x94],'little')}
t0=time.time(); sent={}
log=lambda *a: print("t=%.0f"%(time.time()-t0),*a,flush=True)
while time.time()-t0<300:
    s=state()
    if s is None: time.sleep(0.5); continue
    now=time.time()
    if s['gs']==5 and s['dat1']==1:
        if now-sent.get('t',0)>=0.8: press(START,30,0); sent['t']=now; log("title START")
    elif s['gs']==5 and s['dat1']==2:
        log("MENU"); break
    time.sleep(0.4)
else: sys.exit(1)
for i in range(2):
    press(DOWN,3,0); time.sleep(1.0)
time.sleep(0.5)
# confirm P1 only ONCE (START), then hands off
press(START,40,0)
time.sleep(1.0)
s=state(); log("after confirm gs=%d dat1=%02x sig=%s b4=%04x b6=%04x"%(s['gs'],s['dat1'],s['sig'],s['b4'],s['b6']))
# monitor only — NO pad0 mashing during fight; pad1 mashing is harmless/CPU-side
mon_end=time.time()+SECS; last=None
while time.time()<mon_end:
    s=state(); now=time.time()
    if s is None: time.sleep(0.5); continue
    key=(s['gs'],s['dat1'],s['sig'],s['b4'],s['b6'],s['e0'],s['e1'],s['n99'],s['p92'])
    if key!=last:
        last=key
        log("gs=%02x dat1=%02x sig=%s b4=%04x b6=%04x e4=%s/%s 99=%s 92=%04x"%(s['gs'],s['dat1'],s['sig'],s['b4'],s['b6'],s['e0'],s['e1'],s['n99'],s['p92']))
    time.sleep(0.4)
log("done")
