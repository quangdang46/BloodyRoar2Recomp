#!/usr/bin/env python3
# row3_twopad.py — drive menu row3 (sig 1,4) confirming on BOTH pads.
# mode (1,4) is 2P: side1's +0xb6 phase arms only via a pad-slot-1 0x840 edge
# AT THE MAIN MENU while row3 is highlighted. Single-pad confirm leaves
# side1 unarmed -> tally wedge (faithful). This presses both slots.
# Mirrors tools/mode_monitor.py Phase A/B nav, adds slot-1 confirm.
# usage: row3_twopad.py PORT [SECS]
import socket, json, sys, time
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5370
SECS = int(sys.argv[2]) if len(sys.argv) > 2 else 300
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF; ATK=0xEFFF

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

def press(b, f, slot=0): return cmd({"cmd": "press", "slot": slot, "buttons": b & 0xFFFF, "frames": f})

def state():
    st = rd(0x1f800000, 0x9d)
    if len(st) < 0x9d: return None
    return {'gs': st[0], 'dat1': st[1], 'sig': (st[0x9B], st[0x9C]),
            'b4': int.from_bytes(st[0xb4:0xb6],'little'),
            'b6': int.from_bytes(st[0xb6:0xb8],'little'),
            'p1e4': rd(0x1f8001cc,1), 'p2e4': rd(0x1f8002d4,1)}

t0=time.time(); sent={}
print("row3 twopad port=%d secs=%d" % (PORT, SECS), flush=True)

# ---- Phase A: title (gs5,d1=1) -> menu (gs5,d1=2) ----
stage="boot"
while time.time()-t0 < 300:
    s = state()
    if s is None: time.sleep(0.5); continue
    now=time.time()
    if s['gs']==5 and s['dat1']==1:
        if now-sent.get('t',0)>=0.8:
            press(START,30,0); sent['t']=now; print("t=%.0f title->START"%(now-t0),flush=True)
    elif s['gs']==5 and s['dat1']==2:
        print("t=%.0f MENU sig=%s"%(now-t0,s['sig']),flush=True); stage="menu"; break
    time.sleep(0.4)
if stage!="menu":
    print("no menu"); sys.exit(1)

# ---- Phase B: DOWN x2 to row3, confirm BOTH pads ----
for i in range(2):
    press(DOWN,3,0); time.sleep(1.0)
    s=state(); print("t=%.0f down#%d sig=%s"%(time.time()-t0,i+1,s['sig'] if s else None),flush=True)
time.sleep(0.5)
confirmed=False; cend=time.time()+60; ci=0
while time.time()<cend:
    b = START if ci%2==0 else CROSS; ci+=1
    press(b,40,0); press(b,40,1)   # BOTH pads confirm -> arms +0xb6
    time.sleep(1.2)
    s=state()
    if s and s['gs']!=5:
        print("t=%.0f CONFIRMED gs=%d dat1=%02x sig=%s b4=%04x b6=%04x"%(time.time()-t0,s['gs'],s['dat1'],s['sig'],s['b4'],s['b6']),flush=True)
        confirmed=True; break
    if s: print("t=%.0f menu gs=%d d1=%d sig=%s b4=%04x b6=%04x"%(time.time()-t0,s['gs'],s['dat1'],s['sig'],s['b4'],s['b6']),flush=True)
if not confirmed:
    print("CONFIRM FAILED"); sys.exit(2)

# ---- Phase C: monitor + dual-pad drive through battle/tally ----
mon_end=time.time()+SECS; last=None; mash=0
while time.time()<mon_end:
    s=state(); now=time.time()
    if s is None: time.sleep(0.5); continue
    key=(s['gs'],s['dat1'],s['sig'],s['b4'],s['b6'],s['p1e4'].hex(),s['p2e4'].hex())
    if key!=last:
        last=key
        print("t=%.0f gs=%02x dat1=%02x sig=%s b4=%04x b6=%04x e4=%s/%s"%(now-t0,s['gs'],s['dat1'],s['sig'],s['b4'],s['b6'],s['p1e4'].hex(),s['p2e4'].hex()),flush=True)
    if s['gs']==8:
        mash+=1
        b=[CROSS,START,CROSS,DOWN,ATK][mash%5]
        press(b,4,0); press(b,4,1)
    elif s['gs']==9 or (s['gs']==8 and s['dat1'] not in (0,3,4,8)):
        press(CROSS,4,0); press(CROSS,4,1)
    time.sleep(0.3)
print("done t=%.0f"%(time.time()-t0),flush=True)
