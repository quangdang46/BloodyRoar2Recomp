#!/usr/bin/env python3
# wedge_hunt.py — retry loop: boot headless emu, drive arcade fight mash,
# poll DAT1 (0x1f800001) + call_bail until either the DAT1==8 announce wedge
# latches (bump latched, drops climbing) or the run crashes/exits.
# Usage: wedge_hunt.py <port> <attempts> [mash_seconds]
import socket, json, subprocess, sys, time, os, signal

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 9991
ATTEMPTS = int(sys.argv[2]) if len(sys.argv) > 2 else 4
MASH_SECS = int(sys.argv[3]) if len(sys.argv) > 3 else 220
ROOT = "/Users/tranquangdang21/Projects/br2-recomp"
EXE = ROOT + "/build/BloodyRoar2_Recompiled"
DISC = ROOT + "/disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue"

def cmd(c, timeout=8):
    try:
        s = socket.create_connection(('127.0.0.1', PORT), 3)
    except OSError:
        return {}
    try:
        s.sendall((json.dumps(c) + '\n').encode()); s.settimeout(timeout)
        b = b''
        while b'\n' not in b:
            x = s.recv(1 << 20)
            if not x: break
            b += x
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except Exception: return {}
    except Exception:
        return {}

def rd8(addr):
    r = cmd({"cmd": "read_ram", "addr": hex(addr), "len": 1})
    if r.get("ok") and r.get("hex"):
        return int(r["hex"], 16)
    return None

def alive(pid):
    try: os.kill(pid, 0); return True
    except OSError: return False

for att in range(ATTEMPTS):
    log = open(ROOT + "/proof/wedge_%d.log" % PORT, "ab")
    emu = subprocess.Popen([EXE, "--game", "game_1pad.toml", "--disc", DISC,
                            "--debug-port", str(PORT), "--headless"],
                           cwd=ROOT, stdout=log, stderr=subprocess.STDOUT)
    mash = subprocess.Popen(["python3", ROOT + "/tools/arcade_fight_mash.py", str(PORT)],
                            cwd=ROOT, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print("[att %d] emu pid=%d port=%d" % (att, emu.pid, PORT), flush=True)
    t0 = time.time(); wedge = False; last = None; stable8 = 0
    while time.time() - t0 < MASH_SECS:
        time.sleep(12)
        if not alive(emu.pid):
            print("[att %d] emu exited" % att, flush=True); break
        fr = cmd({"cmd": "frame"}).get("frame")
        cb = cmd({"cmd": "call_bail"})
        d1 = rd8(0x1f800001)
        bail = cb.get("bail"); drops = cb.get("drops")
        line = "t=%ds frame=%s DAT1=%s bail=%s drops=%s last_fn=%s" % (
            int(time.time()-t0), fr, d1, bail, drops, cb.get("last_fn"))
        print(line, flush=True)
        log.write((line + "\n").encode()); log.flush()
        if d1 == 8:
            stable8 += 1
            if stable8 >= 3 and bail == 1:
                wedge = True
                print("[att %d] WEDGE CONFIRMED DAT1==8 bail latched" % att, flush=True)
                break
        else:
            stable8 = 0
        last = (fr, d1, bail, drops)
    # dump final state
    cb = cmd({"cmd": "call_bail"})
    print("[att %d] final call_bail=%s" % (att, json.dumps(cb)), flush=True)
    log.write(("final call_bail=" + json.dumps(cb) + "\n").encode()); log.flush()
    try: emu.terminate()
    except Exception: pass
    try: mash.terminate()
    except Exception: pass
    time.sleep(1)
    try: emu.kill()
    except Exception: pass
    if wedge:
        print("WEDGE REPRODUCED on attempt %d port %d — leaving emu down; "
              "rerun manually for inspection" % (att, PORT), flush=True)
        break
print("hunt done", flush=True)
