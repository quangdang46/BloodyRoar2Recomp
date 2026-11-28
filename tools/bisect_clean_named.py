#!/usr/bin/env python3
# bisect_clean_named.py — bisect which named clean-port cluster breaks attract exit.
# Usage: bisect_clean_named.py <cluster_indices_csv> <port>
#   Runs one instance with PSX_NO_CPS=1 + CLEAN_EXCLUDE=<those clusters>,
#   then attract_exit_test. Prints VERDICT: EXIT or STUCK.
import re, socket, json, sys, time, subprocess, os, signal

REPO = "/Users/tranquangdang21/Projects/br2-recomp"
BUILD = os.path.join(REPO, "build")
DISC = os.path.join(REPO, "disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue")

# cluster table regenerated from src/clean_table.c (named, flag==0 entries,
# gap-split at 0x800) — same clustering as the analysis pass.
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
    if sys.argv[1].startswith("x:"):
        excl = sys.argv[1][2:]
    else:
        idxs = [int(x) for x in sys.argv[1].split(",") if x != ""]
        cls = clusters()
        excl = ",".join("0x%x-0x%x" % (cls[i][0], cls[i][1]) for i in idxs)
    env = dict(os.environ)
    env["PSX_NO_CPS"] = "1"
    env["CLEAN_EXCLUDE"] = excl
    log = "/tmp/br2_bisect_%d.log" % port
    lf = open(log, "w")
    p = subprocess.Popen(
        [os.path.join(BUILD, "BloodyRoar2_Recompiled"), "--disc", DISC,
         "--debug-port", str(port), "--headless", "--game",
         os.path.join(REPO, "game_1pad.toml")],
        cwd=BUILD, env=env, stdout=lf, stderr=subprocess.STDOUT)
    try:
        t = subprocess.run(["python3", os.path.join(REPO, "tools/attract_exit_test.py"),
                            str(port), "10"], capture_output=True, text=True, timeout=300)
        out = t.stdout + t.stderr
        print(out)
        verdict = "EXIT" if "ATTRACT EXITED" in out else ("STUCK" if "STILL IN ATTRACT" in out else "UNKNOWN")
        print("VERDICT:", verdict, "port", port, "excluded", len(idxs), "clusters")
    finally:
        p.terminate()
        try:
            p.wait(5)
        except Exception:
            p.kill()

if __name__ == "__main__":
    main()
