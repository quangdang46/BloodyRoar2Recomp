#!/usr/bin/env python3
import socket, json, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 9940
START=0xfff7; DOWN=0xffbf
def cmd(c, timeout=8):
    for _ in range(40):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), 3); break
        except OSError:
            time.sleep(0.2)
    else:
        return {}
    try:
        s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(timeout); b = b''
        while True:
            x = s.recv(1 << 20)
            if not x: break
            b += x
            try:
                d = json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None or 'error' in d:
                    break
            except Exception:
                pass
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except Exception: return {}
    except Exception:
        return {}
def rd(a, n):
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''
def press(btn, frames, slot=0):
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})
t0=time.time(); stage="boot"; sent={}
print("nav wedgeA port=%d"%PORT, flush=True)
while time.time()-t0 < 600:
    st = rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        time.sleep(0.4); continue
    gs,ph,sub = st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    fr = cmd({"cmd":"frame"}).get("frame")
    now=time.time()
    print("t=%.0f gs=%d ph=%d sub=%d sig=%s fr=%s"%(now-t0,gs,ph,sub,sig,fr),flush=True)
    # title -> start
    if gs==5 and ph==1:
        if now-sent.get('t',0)>=0.8: press(START,40,0); sent['t']=now
    # mode menu gs=5 ph=2
    if gs==5 and ph==2:
        if sig!=(1,4):
            # cursor starts sel1 (0,0); DOWN x2 -> sel3 (1,4). Only press DOWN
            # if sig is behind (0,0)->down1->(2,1)->down2->(1,4)
            cur = {(0,0):0,(2,1):1,(1,4):2,(0,3):3,(3,2):4}.get(sig,-1)
            tgt = 2
            if cur>=0 and cur<tgt:
                if now-sent.get('d',0)>=0.8: press(DOWN,20,0); sent['d']=now
            elif cur<0:
                if now-sent.get('d',0)>=0.8: press(DOWN,20,0); sent['d']=now
        else:
            if now-sent.get('s',0)>=1.0: press(START,60,0); sent['s']=now
    if gs==8:
        print("*** gs=8 entered sig=%s t=%.0f ***"%(sig,now-t0),flush=True)
        # keep watching; wedge = stuck here
        stage="wedge_watch"
        while time.time()-t0 < 600:
            st = rd(0x1f800000,0x9d)
            gs,ph,sub = st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
            fr = cmd({"cmd":"frame"}).get("frame")
            d1 = st[1]
            print("WEDGE t=%.0f gs=%d ph=%d DAT1=%d sub=%d sig=%s fr=%s"%(time.time()-t0,gs,ph,d1,sub,sig,fr),flush=True)
            time.sleep(3)
    time.sleep(0.6)
print("done",flush=True)
