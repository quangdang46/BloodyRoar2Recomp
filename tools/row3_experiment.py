#!/usr/bin/env python3
# row3_experiment.py — drive row3 (sig 1,4) with configurable pad choreography.
# usage: row3_experiment.py PORT MODE [SECS]
#   MODE: twosame | p1only | p2first | p2thenp1sel | watch
import socket, json, sys, time

PORT = int(sys.argv[1])
MODE = sys.argv[2] if len(sys.argv) > 2 else "p1only"
SECS = int(sys.argv[3]) if len(sys.argv) > 3 else 240

START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF; UP=0xFFEF; SELECT=0xFFFE
L1=0xFFEF  # placeholder; raw map: bit11=L1? we'll probe

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

def setin(btn, slot=0):
    return cmd({"cmd": "set_input", "slot": slot, "buttons": btn & 0xFFFF})

def clearin(slot=0):
    return cmd({"cmd": "clear_input", "slot": slot})

def state():
    st = rd(0x1f800000, 0xb8)
    if len(st) < 0xb8: return None
    return {
        'gs': st[0], 'dat1': st[1], 'sig': (st[0x9b], st[0x9c]),
        'b4': int.from_bytes(st[0xb4:0xb6],'little'),
        'b6': int.from_bytes(st[0xb6:0xb8],'little'),
        'e0': int.from_bytes(st[0x68:0x6a],'little'),
        'e1': int.from_bytes(st[0x6a:0x6c],'little'),
        'h0': int.from_bytes(st[0x6c:0x6e],'little'),
        'h1': int.from_bytes(st[0x6e:0x70],'little'),
        'p92': int.from_bytes(st[0x92:0x94],'little'),
    }

def frame_no():
    return cmd({"cmd": "frame"}, 5).get('frame')

def log(*a):
    print("t=%.0f" % (time.time()-t0), *a, flush=True)

t0 = time.time()
log("row3_experiment port=%d mode=%s" % (PORT, MODE))

# ---- Phase A: title -> menu ----
sent = {}
while time.time() - t0 < 300:
    s = state()
    if s is None:
        time.sleep(0.5); continue
    now = time.time()
    if s['gs'] == 5 and s['dat1'] == 1:
        if now - sent.get('t',0) >= 0.8:
            press(START,30,0); sent['t']=now; log("title -> START")
    elif s['gs'] == 5 and s['dat1'] == 2:
        log("MENU sig=%s" % (s['sig'],)); break
    time.sleep(0.4)
else:
    log("no menu"); sys.exit(1)

# ---- pad decode probe: press candidate buttons on slot1, watch e1 (+0x6a edge) ----
if MODE == "padmap":
    cands = {
        'START':0xFFF7,'CROSS':0xBFFF,'CIRCLE':0x7FFF,'SQUARE':0xDFFF,'TRI':0xEFFF,
        'SELECT':0xFFFE,'UP':0xFFEF,'DOWN':0xFFBF,'LEFT':0xFEFF,'RIGHT':0xFDFF,
        'L1?':0xFBFF,'R1?':0xF7FF,'L2?':0xEFFF|0,'combo_CR':0xFFFF&~0x4000,
    }
    # raw pad word bits (active low): bit0=sel? standard: 0xFFFF neutral
    # PSX pad: bit0 select, bit1 L3, bit2 R3, bit3 start, bit4 up,5 right,6 down,7 left,
    #          bit8 L2,9 R2,10 L1,11 R1, bit12-15 face: tri,cir,x,sq
    cands = {
        'SELECT(bit0)':0xFFFE,'L3(bit1)':0xFFFD,'R3(bit2)':0xFFFB,'START(bit3)':0xFFF7,
        'UP(bit4)':0xFFEF,'RIGHT(bit5)':0xFFDF,'DOWN(bit6)':0xFFBF,'LEFT(bit7)':0xFF7F,
        'L2(bit8)':0xFEFF,'R2(bit9)':0xFDFF,'L1(bit10)':0xFBFF,'R1(bit11)':0xF7FF,
        'TRI(bit12)':0xEFFF,'CIR(bit13)':0xDFFF,'CROSS(bit14)':0xBFFF,'SQR(bit15)':0x7FFF,
    }
    for name,btn in cands.items():
        setin(0xFFFF,1); time.sleep(0.15)
        press(btn,2,1)
        time.sleep(0.3)
        s = state()
        if s: log("slot1 %-14s raw=%04x -> e1=%04x h1=%04x" % (name, btn, s['e1'], s['h1']))
        setin(0xFFFF,1); press(0xFFFF,2,1)
        time.sleep(0.2)
    log("padmap done"); sys.exit(0)

# ---- Phase B: nav to row3 (2x DOWN) ----
for i in range(2):
    press(DOWN,3,0); time.sleep(1.0)
    s=state(); log("down#%d sig=%s gs=%d d1=%02x" % (i+1, s['sig'] if s else None, s['gs'] if s else -1, s['dat1'] if s else -1))
time.sleep(0.5)

# ---- Phase C: confirm choreography ----
confirmed=False; cend=time.time()+90; ci=0
while time.time()<cend:
    if MODE == "twosame":
        # hold both slots' START simultaneously via set_input, ~3 frames, clear
        setin(START,0); setin(START,1)
        time.sleep(0.25)
        clearin(0); clearin(1)
        press(0xFFFF,2,0); press(0xFFFF,2,1)
    elif MODE == "p2first":
        press(START,40,1)
    elif MODE == "p2thenp1sel":
        press(START,40,1)
    else:  # p1only / watch
        press(START if ci%2==0 else CROSS, 40, 0)
    ci+=1
    time.sleep(1.0)
    s=state()
    if s and s['gs']!=5:
        log("CONFIRMED gs=%d dat1=%02x sig=%s b4=%04x b6=%04x" % (s['gs'],s['dat1'],s['sig'],s['b4'],s['b6']))
        confirmed=True; break
    if s: log("menu gs=%d d1=%d sig=%s b4=%04x b6=%04x" % (s['gs'],s['dat1'],s['sig'],s['b4'],s['b6']))
if not confirmed:
    log("CONFIRM FAILED"); sys.exit(2)

# ---- Phase D: monitor; during gs==8 dat1==6 (select) try P2 join presses ----
mon_end=time.time()+SECS; last=None; mash=0; p2join_done=False
while time.time()<mon_end:
    s=state(); now=time.time()
    if s is None: time.sleep(0.5); continue
    key=(s['gs'],s['dat1'],s['sig'],s['b4'],s['b6'],s['p92'])
    if key!=last:
        last=key
        log("gs=%02x dat1=%02x sig=%s b4=%04x b6=%04x 92=%04x" % (s['gs'],s['dat1'],s['sig'],s['b4'],s['b6'],s['p92']))
    # P2 join attempt during select screen
    if MODE=="p2thenp1sel" and s['gs']==8 and s['dat1']==6 and not p2join_done:
        p2join_done=True
        log("select screen: P2 join attempt (START+CROSS)")
        press(START,30,1); press(CROSS,30,1)
    if s['gs']==8:
        mash+=1
        b=[CROSS,START,CROSS,DOWN][mash%4]
        press(b,4,0); press(b,4,1)
    time.sleep(0.3)
log("done")
