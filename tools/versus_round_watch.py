#!/usr/bin/env python3
# versus_round_watch.py — nav to versus fight, mash both pads, and watch:
#   gs/ph state, anim stream PCs (0x801F53AC/0x801F54C4), fighter anim obj
#   ptrs (sp+0x118/0x220), round timer candidates (obj+0xf0), fighter state
#   bytes, and any gs/ph transition (round end). Prints a line per sample.
import socket, json, struct, sys, time

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5500
DUR  = float(sys.argv[2]) if len(sys.argv) > 2 else 2400
IVL  = float(sys.argv[3]) if len(sys.argv) > 3 else 6.0

def cmd(c, timeout=10):
    for _ in range(60):
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
                if 'hex' in d or d.get('ok') is not None or 'entries' in d:
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

def u32(b, off):
    return struct.unpack('<I', b[off:off+4])[0] if len(b) >= off+4 else None
def u16(b, off):
    return struct.unpack('<H', b[off:off+2])[0] if len(b) >= off+2 else None

def press(btn, frames, slot): return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF; ATK=[0xEFFF,0x7FFF,0xDFFF,0xBFFF]
t0=time.time(); stage="init"; sent={}; mash_i=0
last_state=None
print("round watch start port=%d dur=%.0f ivl=%.1f" % (PORT,DUR,IVL), flush=True)
while time.time()-t0 < DUR:
    sp = rd(0x1f800000, 0x400)
    if len(sp) < 0x9d:
        ok=False
        for _ in range(10):
            fr = cmd({"cmd":"frame"},15).get("frame")
            if fr is not None: ok=True; break
            time.sleep(1.0)
        if not ok:
            print("EMULATOR UNREACHABLE t=%.0f" % (time.time()-t0), flush=True); break
        time.sleep(0.5); continue
    gs, ph, sub = sp[0], sp[1], sp[0x9C]
    sig = (sp[0x9B], sp[0x9C])
    fr = cmd({"cmd":"frame"},5).get("frame")
    now = time.time()

    if stage in ("init","title"):
        if gs==5 and ph==1:
            stage="title"
            if now-sent.get('t',0)>=0.5: press(START,30,0); sent['t']=now
        elif gs==5 and ph==2:
            stage="menu"
            if sig!=(2,1):
                if now-sent.get('d',0)>=0.5: press(DOWN,3,0); sent['d']=now
            else:
                if now-sent.get('s',0)>=1.0: press(START,40,0); sent['s']=now
        elif gs==8: stage="vs_init"
        elif gs!=5: stage="init"
    elif stage=="menu":
        if gs==5 and ph==2:
            if sig!=(2,1):
                if now-sent.get('d',0)>=0.5: press(DOWN,3,0); sent['d']=now
            else:
                if now-sent.get('s',0)>=1.0: press(START,40,0); sent['s']=now
        elif gs==8: stage="vs_init"
        elif gs!=5: stage="title"
    elif stage=="vs_init":
        if gs==8:
            if ph==6:
                for s in (0,1):
                    if now-sent.get(('c',s),0)>=1.0: press(CROSS,30,s); sent[('c',s)]=now
            elif ph==7:
                for s in (0,1):
                    if now-sent.get(('g',s),0)>=1.0: press(START,30,s); sent[('g',s)]=now
            elif ph==8:
                stage="fight"
                print("*** FIGHT gs=8 ph=8 fr=%s t=%.0f ***" % (fr, now-t0), flush=True)
        elif gs==6: stage="title"
    elif stage=="fight":
        if int(now*2)!=mash_i:
            mash_i=int(now*2)
            press(ATK[mash_i%4],4,0)
            press(ATK[(mash_i+1)%4],4,1)
        if gs!=8 or ph!=8:
            print("*** STATE CHANGE gs=%d ph=%d sub=%d sig=%s fr=%s t=%.0f ***" %
                  (gs,ph,sub,sig,fr,now-t0), flush=True)
            if gs==5: stage="title"
            elif gs==8: stage="vs_init"
            else: stage="init"

    state=(gs,ph,sub,sig)
    p1pc=u32(rd(0x801F53AC,4),0); p2pc=u32(rd(0x801F54C4,4),0)
    p1ani=u32(sp,0x118); p2ani=u32(sp,0x220)
    # timer candidates: obj+0xf0 for each side anim obj, plus a few scratchpad cells
    t1=t2=None
    if p1ani: t1=u32(rd(p1ani+0xf0,4),0)
    if p2ani: t2=u32(rd(p2ani+0xf0,4),0)
    # fighter state bytes: scan sp for plausible state fields near known offsets
    # print raw u32s at sp+0x100..0x140 and 0x200..0x240 when changed
    print("t=%.0f f=%s gs=%d ph=%d sub=%d sig=%s p1pc=%s p2pc=%s t1=%s t2=%s" % (
        now-t0, fr, gs, ph, sub, sig,
        hex(p1pc) if p1pc is not None else None,
        hex(p2pc) if p2pc is not None else None,
        t1, t2), flush=True)
    time.sleep(IVL)
print("round watch done", flush=True)
