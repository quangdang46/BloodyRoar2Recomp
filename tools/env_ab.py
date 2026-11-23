#!/usr/bin/env python3
# env_ab.py — launch instances with env overrides, drive with mash, report
# crash frame + signature. Usage: env_ab.py NAME=VAL NAME=VAL ... -- ports
import socket, json, sys, time, os, subprocess

ROOT = "/Users/tranquangdang21/Projects/br2-recomp"
BIN = os.path.join(ROOT, "build/BloodyRoar2_Recompiled")
DISC = "/Users/tranquangdang21/Projects/bloody-roar-2/Game.cue"
TOML = os.path.join(ROOT, "game_1pad.toml")
OUTDIR = os.path.join(ROOT, "proof")

def cmd(port, c, timeout=10):
    try:
        s = socket.create_connection(('127.0.0.1', port), 3)
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

def run(port, env_extra, tag):
    log = open(os.path.join(OUTDIR, "emu_%s_%d.log" % (tag, port)), "w")
    env = dict(os.environ); env.update(env_extra)
    pr = subprocess.Popen([BIN, "--game", TOML, "--disc", DISC,
                           "--debug-port", str(port), "--headless"],
                          stdout=log, stderr=subprocess.STDOUT, cwd=ROOT, env=env)
    for _ in range(200):
        if cmd(port, {"cmd": "frame"}, 3): break
        time.sleep(0.2)
    mp = subprocess.Popen(["python3", os.path.join(ROOT, "tools/arcade_fight_mash.py"), str(port)],
                          stdout=open(os.path.join(OUTDIR, "mash_%s_%d.log" % (tag, port)), "w"),
                          stderr=subprocess.STDOUT, cwd=ROOT)
    last_fr, last_chg, t0 = None, time.time(), time.time()
    result = {"tag": tag, "port": port, "env": env_extra}
    while time.time() - t0 < 1500:
        time.sleep(4)
        if pr.poll() is not None:
            result["emu_exit"] = pr.returncode; break
        r = cmd(port, {"cmd": "frame"}, 10)
        fr = r.get("frame")
        if fr is not None and fr != last_fr:
            last_fr, last_chg = fr, time.time()
        stall = time.time() - last_chg
        fatal = False
        try:
            log.flush()
            with open(os.path.join(OUTDIR, "emu_%s_%d.log" % (tag, port)), errors="replace") as f:
                tail = f.read()[-6000:]
            fatal = "DISPATCH FATAL" in tail
        except Exception:
            pass
        if fatal or stall > 30:
            result["crash_frame"] = last_fr
            result["fatal"] = fatal
            result["stall"] = stall
            regs = cmd(port, {"cmd": "get_registers"}, 10)
            result["regs"] = regs
            break
        if fr and fr > 5000:
            result["survived"] = True; result["frame"] = fr; break
    else:
        result["timeout"] = True
    result["last_fr"] = last_fr
    try: mp.terminate()
    except Exception: pass
    print(json.dumps(result), flush=True)
    return result

def main():
    # args: pairs of tag:PORT:K=V,K=V
    for spec in sys.argv[1:]:
        parts = [p for p in spec.split(":") if p != ""]
        if len(parts) < 2:
            print("skip malformed spec: %r" % spec, flush=True)
            continue
        tag, port = parts[0], int(parts[1])
        env = {}
        for kv in (parts[2] if len(parts) > 2 else "").split(","):
            if "=" in kv:
                k, v = kv.split("=", 1); env[k] = v
        run(port, env, tag)

if __name__ == "__main__":
    main()
