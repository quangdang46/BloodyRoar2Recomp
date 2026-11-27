#!/usr/bin/env python3
import socket, json, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 9940
START=0xfff7; DOWN=0xffbf; UP=0xffef
def cmd(c, timeout=8):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
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
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''
def press(btn, frames, slot=0):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})
t0=time.time(); sent={}
print("nav2 wedgeA port=%d"%PORT, flush=True)
order=[(0,0),(2,1),(1,4),(0,3),(3,2)]
while time.time()-t0 < 900:
    st = rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        time.sleep(0.4); continue
    gs,ph,sub = st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    fr = cmd({"cmd":"frame"}).get("frame")
    now=time.time()
    print("t=%.0f gs=%d ph=%d sub=%d sig=%s fr=%s"%(now-t0,gs,ph,sub,sig,fr),flush=True)
    if gs==8:
        print("*** gs=8 sig=%s t=%.0f ***"%(sig,now-t0),flush=True)
        while True:
            st = rd(0x1f800000,0x9d)
            gs,ph,sub = st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C]); d1=st[1]
            fr = cmd({"cmd":"frame"}).get("frame")
            print("WEDGE t=%.0f gs=%d ph=%d DAT1=%d sub=%d sig=%s fr=%s"%(time.time()-t0,gs,ph,d1,sub,sig,fr),flush=True)
            time.sleep(5)
    if gs==5 and ph==2:
        if sig==(1,4):
            if now-sent.get('s',0)>=1.5: press(START,60,0); sent['s']=now
        else:
            try: cur=order.index(sig)
            except ValueError: cur=-1
            if cur<2:
                if now-sent.get('d',0)>=1.0: press(DOWN,15,0); sent['d']=now
            elif cur>2:
                if now-sent.get('u',0)>=1.0: press(UP,15,0); sent['u']=now
            else:
                if now-sent.get('d',0)>=1.0: press(DOWN,15,0); sent['d']=now
    elif gs==5 and ph==1:
        if now-sent.get('t',0)>=0.8: press(START,40,0); sent['t']=now
    time.sleep(0.8)
print("done",flush=True)
