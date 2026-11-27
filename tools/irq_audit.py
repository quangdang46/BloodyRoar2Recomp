#!/usr/bin/env python3
# irq_audit.py — single-instance repro of the ~frame-1560 kernel-stack
# collision crash, with devtrace (every IRQ-raise edge) + dma_trace +
# cdrom_trace armed so the crash dump names WHICH device IRQ landed in the
# vsync_wait_n window and at what guest cycle.
import socket, json, sys, time, os, subprocess

ROOT = "/Users/tranquangdang21/Projects/br2-recomp"
BIN = os.path.join(ROOT, "build/BloodyRoar2_Recompiled")
DISC = "/Users/tranquangdang21/Projects/bloody-roar-2/Game.cue"
TOML = os.path.join(ROOT, "game_1pad.toml")
OUTDIR = os.path.join(ROOT, "proof")
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 9996

def cmd(c, timeout=15):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    try:
        s = socket.create_connection(('127.0.0.1', PORT), 3)
    except OSError:
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

def arm():
    # IRQ-raise edges for the whole run (1M ring)
    cmd({"cmd": "devtrace_ctl", "arm": 1})
    # stack collision slot diagnostics (same as crash_hunt)
    cmd({"cmd": "wtrace_arm", "lo": "0x1FFE50", "hi": "0x1FFE64"})
    cmd({"cmd": "rwatchpc_arm", "lo": "0x1FFE00", "hi": "0x1FFE80", "rt": 31})

def dump(tag):
    """Dump scratchpad/pad diagnostic bytes."""
    d = {"tag": tag, "t": time.time(), "port": PORT}
    d["regs"] = cmd({"cmd": "get_registers"}, 15)
    d["frame"] = cmd({"cmd": "frame"}, 15)
    d["irqctx"] = cmd({"cmd": "irqctx_ring", "count": 128}, 30)
    d["devtrace"] = cmd({"cmd": "devtrace_dump", "count": 65536}, 60)
    d["dma_trace"] = cmd({"cmd": "dma_trace_dump", "count": 512}, 30)
    d["dma_state"] = cmd({"cmd": "dma_state"}, 15)
    d["cdrom_timing"] = cmd({"cmd": "cdrom_timing_dump", "count": 256}, 30)
    d["cdrom_state"] = cmd({"cmd": "cdrom_state"}, 15)
    d["sp_ring"] = cmd({"cmd": "sp_ring", "count": 64}, 20)
    d["event_ring"] = cmd({"cmd": "event_ring_dump", "count": 256}, 30)
    path = os.path.join(OUTDIR, "irqaudit_%d.json" % PORT)
    with open(path, "w") as f:
        json.dump(d, f, indent=1)
    print("DUMP -> %s" % path, flush=True)
    return path

def main():
    """Entry point: run the watch/nav loop until timeout or exit condition."""
    log = open(os.path.join(OUTDIR, "emu_irq_%d.log" % PORT), "w")
    pr = subprocess.Popen([BIN, "--game", TOML, "--disc", DISC,
                           "--debug-port", str(PORT), "--headless"],
                          stdout=log, stderr=subprocess.STDOUT, cwd=ROOT)
    for _ in range(200):
        if cmd({"cmd": "frame"}, 3): break
        time.sleep(0.2)
    print("port up", flush=True)
    arm()
    mp = subprocess.Popen(["python3", os.path.join(ROOT, "tools/arcade_fight_mash.py"), str(PORT)],
                          stdout=open(os.path.join(OUTDIR, "mash_irq_%d.log" % PORT), "w"),
                          stderr=subprocess.STDOUT, cwd=ROOT)
    last_fr, last_chg = None, time.time()
    t0 = time.time()
    dumped = False
    while time.time() - t0 < 2400:
        time.sleep(4)
        if pr.poll() is not None:
            print("EMU EXIT rc=%s" % pr.returncode, flush=True)
            break
        r = cmd({"cmd": "frame"}, 10)
        fr = r.get("frame")
        if fr is not None and fr != last_fr:
            last_fr, last_chg = fr, time.time()
            if fr % 200 < 8:
                print("frame %s t=%.0f" % (fr, time.time() - t0), flush=True)
        stall = time.time() - last_chg
        fatal = False
        try:
            log.flush()
            with open(os.path.join(OUTDIR, "emu_irq_%d.log" % PORT), errors="replace") as f:
                tail = f.read()[-6000:]
            fatal = "DISPATCH FATAL" in tail or ("FATAL" in tail and "misaligned" in tail)
        except Exception:
            pass
        if (stall > 25 or fatal) and not dumped:
            print("SUSPECT fr=%s stall=%.0f fatal=%s" % (fr, stall, fatal), flush=True)
            dump("stall_or_fatal")
            dumped = True
            break
        if fr and fr > 4000 and not dumped:
            print("past crash window fr=%s — keep running" % fr, flush=True)
            dumped = True  # don't re-dump; keep watching a bit
    print("done dumped=%s last_fr=%s" % (dumped, last_fr), flush=True)

if __name__ == "__main__":
    main()
