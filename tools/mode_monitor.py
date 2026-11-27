#!/usr/bin/env python3
# mode_monitor.py — generic non-combat mode driver + wedge monitor.
# usage: mode_monitor.py PORT MODE [SECS]
#   MODE: attract | row1 | row3 | row5 | row6 | row7
# Menu rows (1-based cursor, sd.sel starts at 1):
#   row1 -> sig(0,0)  row2 -> (2,1) versus  row3 -> (1,4)
#   row4 -> (0,3) arcade  row5 -> (3,2)  row6 -> options_b(gs=11,sub4)
#   row7 -> options_a(gs=11,sub3)
import socket, json, sys, time, os

PORT = int(sys.argv[1])
MODE = sys.argv[2] if len(sys.argv) > 2 else "row1"
SECS = int(sys.argv[3]) if len(sys.argv) > 3 else 150

START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF; UP=0xFFEF; SELECT=0xFFFE

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

def state():
    """Snapshot the scratchpad game-state block and return a dict of fields."""
    st = rd(0x1f800000, 0xa4)
    if len(st) < 0xa4: return None
    return {
        'gs': st[0], 'dat1': st[1], 'sig': (st[0x9b], st[0x9c]),
        'b4': int.from_bytes(st[0xb4:0xb6],'little'),
        'b6': int.from_bytes(st[0xb6:0xb8],'little'),
        'pad0': int.from_bytes(st[0x68:0x6a],'little'),
        'pad1': int.from_bytes(st[0x70:0x72],'little'),
        'raw': st.hex(),
    }

def frame_no():
    """Query the frame counter via the 'frame' debug command."""
    return cmd({"cmd": "frame"}, 5).get('frame')

def freeze():
    """Run freeze_check and keep the bail/call counters."""
    r = cmd({"cmd": "freeze_check", "window": 64}, 15)
    keep = {k: r.get(k) for k in ('bce_calls','pbd_calls','bce_phase','pbd_phase') if k in r}
    return keep

def cdrom():
    """Query cdrom_state and return the pending/reading/irq/last-sector fields."""
    r = cmd({"cmd": "cdrom_state"}, 10)
    return {k: r.get(k) for k in ('pending','reading','irq_flag','last_sector') if k in r}

def dump_diag(tag):
    """Collect registers/chain diagnostics under a tag."""
    out = {"tag": tag, "mode": MODE, "port": PORT, "t": time.time()}
    for name, c in [
        ("cdrom_state", {"cmd":"cdrom_state"}),
        ("cdrom_command_history", {"cmd":"cdrom_command_history","count":64}),
        ("bioscall", {"cmd":"bioscall_dump","count":400}),
        ("sp_ring", {"cmd":"sp_ring","count":64}),
        ("dispatch_tail", {"cmd":"dispatch_tail","count":64}),
        ("fn_entry_tail", {"cmd":"fn_entry_tail","count":64}),
        ("fn_stats", {"cmd":"fn_stats","count":150}),
        ("event_ring_tail", {"cmd":"event_ring_tail","count":64}),
        ("mdec_state", {"cmd":"mdec_state"}),
        ("freeze_check", {"cmd":"freeze_check","window":64}),
        ("get_registers", {"cmd":"get_registers"}),
        ("irq_state", {"cmd":"irq_state"}),
    ]:
        out[name] = cmd(c, 20)
    path = "/tmp/mode_%d_%s_%s.json" % (PORT, MODE, tag)
    with open(path, "w") as f:
        json.dump(out, f, indent=1)
    print("DIAG DUMP -> %s" % path, flush=True)

DOWNS = {"row1":0, "row3":2, "row5":4, "row6":5, "row7":6}

t0 = time.time()
print("mode_monitor port=%d mode=%s downs=%s" % (PORT, MODE, DOWNS.get(MODE)), flush=True)

if MODE == "attract":
    # passive watch: sample gs/ph/sig each second for SECS
    seen = []
    while time.time() - t0 < SECS:
        s = state(); fr = frame_no()
        if s:
            key = (s['gs'], s['dat1'], s['sig'])
            if not seen or seen[-1][0] != key:
                seen.append((key, fr, time.time()-t0))
                print("t=%.0f fr=%s gs=%02x dat1=%02x sig=%s" % (time.time()-t0, fr, s['gs'], s['dat1'], s['sig']), flush=True)
        time.sleep(1.0)
    print("attract transitions:", seen, flush=True)
    print("done", flush=True)
    sys.exit(0)

# ---- Phase A: reach title (gs=5, dat1=1) then menu (gs=5, dat1=2) ----
stage = "boot"; sent = {}
menu_t = None
while time.time() - t0 < 360:
    s = state()
    if s is None:
        time.sleep(0.5); continue
    gs, d1 = s['gs'], s['dat1']
    now = time.time()
    if gs == 5 and d1 == 1:
        if now - sent.get('t',0) >= 0.8:
            press(START, 30, 0); sent['t'] = now
            print("t=%.0f title -> START" % (now-t0), flush=True)
    elif gs == 5 and d1 == 2:
        print("t=%.0f MENU reached fr=%s sig=%s" % (now-t0, frame_no(), s['sig']), flush=True)
        stage = "menu"; menu_t = now; break
    elif gs not in (3,4,5,6):
        print("t=%.0s unexpected gs=%d dat1=%d" % (now-t0, gs, d1), flush=True)
    time.sleep(0.4)
if stage != "menu":
    print("FAILED to reach menu", flush=True); dump_diag("no_menu"); sys.exit(1)

# ---- Phase B: navigate rows + confirm ----
nd = DOWNS.get(MODE, 0)
for i in range(nd):
    press(DOWN, 3, 0); time.sleep(1.0)
    s = state()
    print("t=%.0f after DOWN #%d sig=%s" % (time.time()-t0, i+1, s['sig'] if s else None), flush=True)
time.sleep(0.5)
confirmed = False
cend = time.time() + 40
ci = 0
while time.time() < cend:
    press(START if ci % 2 == 0 else CROSS, 40, 0); ci += 1
    time.sleep(1.2)
    s = state()
    if s and s['gs'] != 5:
        print("t=%.0f CONFIRMED gs=%d dat1=%02x sig=%s" % (time.time()-t0, s['gs'], s['dat1'], s['sig']), flush=True)
        confirmed = True; break
    if s: print("t=%.0f still menu gs=%d dat1=%d sig=%s" % (time.time()-t0, s['gs'], s['dat1'], s['sig']), flush=True)
if not confirmed:
    print("CONFIRM FAILED (mode %s)" % MODE, flush=True); dump_diag("confirm_fail"); sys.exit(2)

# ---- Phase C: monitor ----
mon_end = time.time() + SECS
last_sig_state = None; last_change = time.time(); last_fr = None
last_fr_change = time.time()
dumped = set(); mash_i = 0
cd_pending09_since = None; last_cd = None
print("t=%.0f monitoring %ds..." % (time.time()-t0, SECS), flush=True)
while time.time() < mon_end:
    s = state(); fr = frame_no(); now = time.time()
    if s is None:
        time.sleep(0.5); continue
    key = (s['gs'], s['dat1'], s['sig'], s['b4'], s['b6'])
    if key != last_sig_state:
        last_sig_state = key; last_change = now
        print("t=%.0f fr=%s gs=%d dat1=%02x sig=%s b4=%04x b6=%04x pad=%04x/%04x" %
              (now-t0, fr, s['gs'], s['dat1'], s['sig'], s['b4'], s['b6'], s['pad0'], s['pad1']), flush=True)
    if fr != last_fr:
        last_fr = fr; last_fr_change = now
    # mash during gs==8 (select screens may need input; harmless otherwise)
    if s['gs'] == 8:
        if int(now*2) != mash_i:
            mash_i = int(now*2)
            btn = [CROSS, START, CROSS, DOWN][mash_i % 4]
            press(btn, 4, 0); press(CROSS, 4, 1)
    # options exit attempt for row6/7: after 20s in gs==11, nav+cross
    if MODE in ("row6","row7") and s['gs'] == 11 and now - last_change > 20 and 'opt_exit' not in dumped:
        dumped.add('opt_exit')
        for _ in range(4): press(DOWN,3,0); time.sleep(0.4)
        press(CROSS,30,0); time.sleep(0.5)
        print("t=%.0f options: sent exit nav" % (now-t0), flush=True)
    # diagnostics every 10s
    if int(now) % 10 == 0 and int(now*10) % 100 == 0:
        fz = freeze(); cd = cdrom()
        print("t=%.0f fr=%s fz=%s cd=%s" % (now-t0, fr, fz, cd), flush=True)
        last_cd = cd
        pend = (cd.get('pending') or {})
        if pend.get('cmd') == 0x09 and pend.get('phase') == 1:
            if cd_pending09_since is None: cd_pending09_since = now
            elif now - cd_pending09_since > 20 and 'cd09' not in dumped:
                dumped.add('cd09'); print("CD PENDING-0x09 PHASE1 >20s", flush=True); dump_diag("cd09_pending")
        else:
            cd_pending09_since = None
    # wedge detectors
    if now - last_fr_change > 15 and 'frz' not in dumped:
        dumped.add('frz'); print("FRAME FROZEN fr=%s" % last_fr, flush=True); dump_diag("frame_frozen")
    if now - last_change > 75 and 'stall' not in dumped and s['gs'] not in (3,4,5,6):
        dumped.add('stall'); print("STATE STALL key=%s" % (key,), flush=True); dump_diag("state_stall")
    time.sleep(0.4)

s = state()
print("FINAL t=%.0f gs=%s dat1=%s sig=%s b4=%s b6=%s" %
      (time.time()-t0, s and s['gs'], s and ('%02x'%s['dat1']), s and s['sig'],
       s and ('%04x'%s['b4']), s and ('%04x'%s['b6'])), flush=True)
dump_diag("final")
print("done", flush=True)
