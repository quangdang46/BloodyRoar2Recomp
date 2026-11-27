#!/usr/bin/env python3
import socket, json, sys, time, re
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5370
REPO='/Users/tranquangdang21/Projects/br2-recomp'
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
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF; UP=0xFEFF; LEFT=0xFDFF; RIGHT=0xFBFF; SELECT=0xFEFF
ATK=[0xEFFF,0x7FFF,0xDFFF,0xBFFF]
t0=time.time(); stage="title"; sent={}
print("P1-ONLY ARCADE watch port=%d"%PORT, flush=True)
last_rep=0
while time.time()-t0 < 900:
    st = rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        time.sleep(0.4); continue
    gs,ph,sub = st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    now=time.time()
    if now-last_rep>1.0:
        last_rep=now
        b4=rd(0x1f8000b4,4).hex()
        print("t=%.0f gs/ph=%02x%02x sig=%s b4b6=%s"%(now-t0,gs,ph,sig,b4),flush=True)
    if stage=="title" and gs==5 and ph==1:
        if now-sent.get('t',0)>=0.5: press(START,30,0); sent['t']=now
    if stage=="menu" or (gs==5 and ph==2):
        stage="menu"
        if gs==5 and ph==2:
            if sig!=(0,3):
                if now-sent.get('d',0)>=0.5: press(DOWN,3,0); sent['d']=now
            else:
                if now-sent.get('s',0)>=1.0: press(START,40,0); sent['s']=now
        elif gs==8: stage="vs_init"
        elif gs!=5: stage="title"
    elif stage in ("title","vs_init"):
        if gs==8:
            stage="vs_init"
            if ph==6:
                if now-sent.get('c',0)>=1.0: press(CROSS,30,0); sent['c']=now
            elif ph==7:
                if now-sent.get('g',0)>=1.0: press(START,30,0); sent['g']=now
            elif ph==8:
                stage="fight"; print("*** FIGHT (P1-only) t=%.0f ***"%(now-t0),flush=True)
        elif gs==6: stage="title"
    elif stage=="fight":
        if int(now*2)!=sent.get('m',-1):
            sent['m']=int(now*2); press(CROSS,4,0)
        if gs!=8: stage="title"; print("left fight gs=%d ph=%d t=%.0f"%(gs,ph,now-t0),flush=True)
    time.sleep(0.3)
print("done",flush=True)
