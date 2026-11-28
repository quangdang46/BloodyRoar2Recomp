#!/usr/bin/env python3
# arm_bisect.py — bisect which named clean-port cluster blocks channel-record ARM.
# Usage: arm_bisect.py <cluster_indices_csv|x:raw_exclude> <port> [timeout_s]
#   Launches with PSX_NO_CPS=1 + CLEAN_EXCLUDE=<clusters>, runs arm_test.
#   ARMED => culprit inside the excluded set; NOT_ARMED => culprit still clean.
import re, socket, json, sys, time, subprocess, os

REPO = "/Users/tranquangdang21/Projects/br2-recomp"
BUILD = os.path.join(REPO, "build")
DISC = os.path.join(REPO, "disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue")

def clusters():
    src = open(os.path.join(REPO, "src/clean_table.c")).read()
    ents = re.findall(r'\{\s*(0x[0-9a-fA-F]+)u,\s*wrap_([0-9a-fA-F]+),\s*(\d+)u,\s*(\d+)u,\s*(\d+)u\s*\}', src)
    named = sorted(int(a, 16) for a, w, c, fr, f in ents if int(f) == 0)
    cls = []
    for a in named:
        if cls and a - cls[-1][1] <= 0x800:
            cls[-1][1] = a
        else:
            cls.append([a, a])
    return cls

def main():
    port = int(sys.argv[2])
    tmo = sys.argv[3] if len(sys.argv) > 3 else "100"
    if sys.argv[1].startswith("x:"):
        excl = sys.argv[1][2:]
    else:
        idxs = [int(x) for x in sys.argv[1].split(",") if x != ""]
        cls = clusters()
        excl = ",".join("0x%x-0x%x" % (cls[i][0], cls[i][1]) for i in idxs)
    env = dict(os.environ)
    env["PSX_NO_CPS"] = "1"
    env["CLEAN_EXCLUDE"] = excl
    log = "/tmp/br2_armbisect_%d.log" % port
    lf = open(log, "w")
    p = subprocess.Popen(
        [os.path.join(BUILD, "BloodyRoar2_Recompiled"), "--disc", DISC,
         "--debug-port", str(port), "--headless", "--game",
         os.path.join(REPO, "game_1pad.toml")],
        cwd=BUILD, env=env, stdout=lf, stderr=subprocess.STDOUT)
    try:
        t = subprocess.run(["python3", os.path.join(REPO, "tools/arm_test.py"),
                            str(port), tmo], capture_output=True, text=True,
                           timeout=float(tmo) + 120)
        out = t.stdout + t.stderr
        print(out[-3000:])
        verdict = "NOT_ARMED" if "NOT_ARMED" in out else ("ARMED" if "ARMED" in out else "UNKNOWN")
        print("VERDICT:", verdict, "port", port)
    finally:
        p.terminate()
        try:
            p.wait(5)
        except Exception:
            p.kill()

if __name__ == "__main__":
    main()
