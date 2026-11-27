#!/usr/bin/env python3
# vs_fight_monitor.py — poll versus fight state every N sec wall-clock:
# gs/ph/sub, p1pc/p2pc anim stream PCs, p1ani/p2ani ptrs, scratchpad diff cells.
import socket, json, time, sys, struct

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5430
DUR  = float(sys.argv[2]) if len(sys.argv) > 2 else 1500
IVL  = float(sys.argv[3]) if len(sys.argv) > 3 else 5.0

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
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r = cmd({"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

def u32(b, off):
    """Extract a little-endian u32 at offset o (0 if short)."""
    return struct.unpack('<I', b[off:off+4])[0] if len(b) >= off+4 else None

def press(btn, frames, slot):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF; ATK=[0xEFFF,0x7FFF,0xDFFF,0xBFFF]
t0 = time.time(); stage="init"; sent={}
prev_sp = None; last_report = {}
mash_i = 0
print("monitor start port=%d dur=%.0f ivl=%.1f" % (PORT, DUR, IVL), flush=True)
while time.time() - t0 < DUR:
    sp = rd(0x1f800000, 0x400)
    if len(sp) < 0x9d:
        time.sleep(1.0); continue
    gs, ph, sub = sp[0], sp[1], sp[0x9C]
    sig = (sp[0x9B], sp[0x9C])
    fr = cmd({"cmd": "frame"}, 5).get("frame")
    now = time.time()

    # navigation state machine (same as versus_fight_mash)
    if stage in ("init", "title"):
        if gs == 5 and ph == 1:
            stage="title"
            if now - sent.get('t',0) >= 0.5: press(START,30,0); sent['t']=now
        elif gs == 5 and ph == 2:
            stage="menu"
            if sig != (2,1):
                if now - sent.get('d',0) >= 0.5: press(DOWN,3,0); sent['d']=now
            else:
                if now - sent.get('s',0) >= 1.0: press(START,40,0); sent['s']=now
        elif gs == 8:
            stage="vs_init"
        elif gs != 5:
            stage="init"
    elif stage == "menu":
        if gs == 5 and ph == 2:
            if sig != (2,1):
                if now - sent.get('d',0) >= 0.5: press(DOWN,3,0); sent['d']=now
            else:
                if now - sent.get('s',0) >= 1.0: press(START,40,0); sent['s']=now
        elif gs == 8: stage="vs_init"
        elif gs != 5: stage="title"
    elif stage == "vs_init":
        if gs == 8:
            if ph == 6:
                for s in (0,1):
                    if now - sent.get(('c',s),0) >= 1.0: press(CROSS,30,s); sent[('c',s)]=now
            elif ph == 7:
                for s in (0,1):
                    if now - sent.get(('g',s),0) >= 1.0: press(START,30,s); sent[('g',s)]=now
            elif ph == 8:
                stage="fight"
                print("*** FIGHT gs=8 ph=8 fr=%s t=%.0f ***" % (fr, now-t0), flush=True)
        elif gs == 6: stage="title"
    elif stage == "fight":
        # mash both pads ~2Hz to drive action_switch / round progress
        if int(now*2) != mash_i:
            mash_i = int(now*2)
            press(ATK[mash_i % 4], 4, 0)
            press(ATK[(mash_i+1) % 4], 4, 1)
        if gs != 8 or ph != 8:
            print("*** STATE CHANGE gs=%d ph=%d sub=%d sig=%s fr=%s t=%.0f ***" %
                  (gs, ph, sub, sig, fr, now-t0), flush=True)
            stage = "postfight"
            if gs == 5: stage="title"
            elif gs == 8: stage="vs_init"

    # sample
    p1pc = u32(rd(0x801F53AC,4),0); p2pc = u32(rd(0x801F54C4,4),0)
    p1ani = u32(sp,0x118); p2ani = u32(sp,0x220)
    # candidate timer cells: scratchpad 0x14..0x40 region + known countdown-ish
    timers = {hex(0x1f800000+o): u32(sp,o) for o in range(0x10,0x40,4)}
    # find decreasing small counters among ALL scratchpad u32s vs last sample
    changes = []
    if prev_sp:
        for o in range(0, 0x400, 4):
            a = struct.unpack('<I', prev_sp[o:o+4])[0]
            b = struct.unpack('<I', sp[o:o+4])[0]
            if a != b:
                changes.append((0x1f800000+o, a, b))
    prev_sp = sp
    # keep only plausible countdowns: small positive ints
    countdn = [(hex(a),a_,b_) for a,a_,b_ in changes
               if isinstance(a_,int) and isinstance(b_,int) and b_ < a_ and 0 < b_ <= 0x200]
    print("t=%.0f f=%s gs=%d ph=%d sub=%d sig=%s p1pc=%s p2pc=%s p1ani=%s p2ani=%s dn=%s" % (
        now-t0, fr, gs, ph, sub, sig,
        hex(p1pc) if p1pc is not None else None,
        hex(p2pc) if p2pc is not None else None,
        hex(p1ani) if p1ani is not None else None,
        hex(p2ani) if p2ani is not None else None,
        countdn[:6]), flush=True)
    time.sleep(IVL)
print("monitor done", flush=True)
