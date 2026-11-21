#!/usr/bin/env python3
# arcade_fight_mash.py — drive ARCADE (1P vs AI) mode to a sustained fight,
# mash P1 inputs to drive rounds to completion, and watch for stalls/crashes.
# Adapted from versus_fight_mash.py; differs:
#   - mode menu: confirm the DEFAULT top item (arcade/1P) — NO DOWN press.
#   - char select: CROSS on slot 0 only (P2 is CPU).
#   - ph==7 START gate only on slot 0.
# Wedge/stall detection + dump_wedge identical to versus_fight_mash.
import socket, json, sys, time, os
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5900

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

def press(btn, frames, slot): return cmd({"cmd": "press", "slot": slot, "buttons": btn & 0xFFFF, "frames": frames})

def bioscount(idx):
    r = cmd({"cmd": "bioscall_dump", "count": 400}, 15)
    for e in (r.get('counts') or []):
        if e.get('table') == '0x000000B0' and e.get('index') == idx:
            return e.get('count', 0)
    return 0

def last_sector_frame():
    r = cmd({"cmd": "cdrom_state"}, 10)
    return (r.get('last_sector') or {}).get('frame')

def dump_wedge(tag):
    out = {"tag": tag, "t": time.time()}
    for name, c in [
        ("irq_state", {"cmd":"irq_state"}),
        ("cdrom_state", {"cmd":"cdrom_state"}),
        ("bioscall", {"cmd":"bioscall_dump","count":400}),
        ("evcb_walk", {"cmd":"evcb_walk_dump","count":400}),
        ("evcb_snapshot", {"cmd":"evcb_snapshot"}),
        ("sp_ring", {"cmd":"sp_ring","count":64}),
        ("fn_entry_tail", {"cmd":"fn_entry_tail","count":64}),
        ("fn_stats", {"cmd":"fn_stats","count":200}),
        ("event_ring_tail", {"cmd":"event_ring_tail","count":64}),
        ("dispatch_tail", {"cmd":"dispatch_tail","count":64}),
        ("current_tcb", {"cmd":"current_tcb"}),
        ("bios_svc_tail", {"cmd":"bios_svc_trace","count":256}),
    ]:
        out[name] = cmd(c, 20)
    cells = [0x801bcb24,0x801bcb0c,0x801bc848,0x801bc84c,0x801bc854,0x801bc858,
             0x801bcd18,0x801bcd1a,0x801bcd48,0x801bdda4,0x801bdda8,0x801bddac,
             0x801bddb0,0x801bcb18,0x801bcb1c,0x1f801070,0x1f801074,0x1f8010f0,
             0x801fc6c8,0x801fc6cc,0x801fc6d0]
    out["cells"] = {hex(a): cmd({"cmd":"read_ram","addr":hex(a),"len":8},10).get("hex") for a in cells}
    out["cd_blk"] = cmd({"cmd":"read_ram","addr":"0x801bcd00","len":256},10).get("hex")
    path = "/tmp/wedge_arcade_%d.json" % PORT
    with open(path, "w") as f:
        json.dump(out, f, indent=1)
    print("WEDGE DUMP -> %s" % path, flush=True)

START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF
ATK = [0xEFFF, 0x7FFF, 0xDFFF, 0xBFFF]

t0 = time.time(); stage = "title"; sent = {}
fight_frame = None; last_fr = None; last_chg = time.time()
prev_b11 = prev_fr = prev_ls = prev_t = None
ls_last_change = time.time(); last_ls_val = None
mash_i = 0; last_sample = 0.0
print("arcade fight mash watch start port=%d" % PORT, flush=True)
while time.time() - t0 < 1800:
    st = rd(0x1f800000, 0x9d)
    if len(st) < 0x9d:
        ok = False
        for _ in range(10):
            fr = cmd({"cmd": "frame"}, 15).get("frame")
            if fr is not None: ok = True; break
            time.sleep(1.0)
        if not ok:
            print("EMULATOR UNREACHABLE t=%.0f" % (time.time()-t0), flush=True); break
        time.sleep(0.4); continue
    gs, ph, sub = st[0], st[1], st[0x9C]; sig = (st[0x9B], st[0x9C])
    fr = cmd({"cmd": "frame"}, 5).get("frame")
    now = time.time()
    if stage == "title" and gs == 5 and ph == 1:
        if now - sent.get('t', 0) >= 0.5: press(START, 30, 0); sent['t'] = now
    if stage == "menu" or (gs == 5 and ph == 2):
        stage = "menu"
        if gs == 5 and ph == 2:
            # arcade = default top item; confirm with START, never DOWN
            if now - sent.get('s', 0) >= 1.0: press(START, 40, 0); sent['s'] = now
        elif gs == 8: stage = "vs_init"
        elif gs != 5: stage = "title"
    elif stage in ("title", "vs_init"):
        if gs == 8:
            stage = "vs_init"
            if ph == 6:
                if now - sent.get('c', 0) >= 1.0:
                    press(CROSS, 30, 0)
                    press(CROSS, 30, 1)  # harmless if ignored
                    sent['c'] = now
            elif ph == 7:
                if now - sent.get('g', 0) >= 1.0:
                    press(START, 30, 0); sent['g'] = now
            elif ph == 8:
                stage = "fight"
                if fight_frame is None:
                    fight_frame = fr; last_chg = now
                    print("*** FIGHT REACHED frame=%s t=%.0f sig=%s ***" % (fr, now-t0, sig), flush=True)
        elif gs == 6:
            stage = "title"
    elif stage == "fight":
        if int(now * 2) != mash_i:
            mash_i = int(now * 2)
            press(ATK[mash_i % len(ATK)], 4, 0)
            press(ATK[(mash_i + 1) % len(ATK)], 4, 1)
        if fr != last_fr:
            last_fr = fr; last_chg = now
        if now - last_chg > 25:
            print("FRAME STALL fr=%s t=%.0f" % (last_fr, now-t0), flush=True)
            dump_wedge("frame_stall")
            last_chg = now
        if now - last_sample >= 1.0:
            last_sample = now
            b11 = bioscount(11)
            ls = last_sector_frame()
            if ls != last_ls_val:
                last_ls_val = ls; ls_last_change = now
            if prev_b11 is not None and prev_fr is not None and fr and fr != prev_fr:
                dt = max(1e-9, now - prev_t)
                rate = (b11 - prev_b11) / dt
                cd_idle = (now - ls_last_change)
                if rate > 1500 and cd_idle > 8:
                    print("WEDGE SUSPECT b11rate=%.0f/s ls=%s ls_idle=%.0fs fr=%s gs=%d ph=%d t=%.0f" %
                          (rate, ls, cd_idle, fr, gs, ph, now-t0), flush=True)
                    dump_wedge("testevent_spin")
                    ls_last_change = now
            prev_b11, prev_fr, prev_t = b11, fr, now
        if gs != 8 or ph != 8:
            print("LEFT FIGHT gs=%d ph=%d fr=%s sig=%s t=%.0f" % (gs, ph, fr, sig, now-t0), flush=True)
            stage = "title"
    time.sleep(0.3)
print("watch done", flush=True)
