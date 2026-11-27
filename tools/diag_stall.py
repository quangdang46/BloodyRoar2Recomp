#!/usr/bin/env python3
"""Watch frame counter; when it stalls, dump all device diagnostics once.
Usage: diag_stall.py <port> <outfile>
"""
import socket, json, sys, time

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4382
OUT = sys.argv[2] if len(sys.argv) > 2 else "/tmp/stall.json"

DIAG = ["cdrom_state","dma_state","irq_state","vblank_rate","spu_ta",
        "spu_events","hle_dump","evcb_snapshot","bioscall_dump",
        "cdrom_command_history","dma_cdrom_history","cycles_to_next_event",
        "get_registers","cdrom_trace_dump"]

def cmd(c, timeout=5):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    try:
        s = socket.create_connection(("127.0.0.1", PORT), timeout)
        s.sendall((json.dumps(c) + "\n").encode())
        s.settimeout(timeout)
        buf = b""
        while True:
            chunk = s.recv(1 << 16)
            if not chunk: break
            buf += chunk
            try:
                d = json.loads(buf.split(b"\n")[0])
                if "ok" in d or "error" in d: break
            except Exception: pass
        s.close()
        try: return json.loads(buf.split(b"\n")[0])
        except Exception: return {"raw": buf[:300].decode("utf-8","replace")}
    except Exception as e:
        return {"err": str(e)}

def frame_no():
    """Query the frame counter via the 'frame' debug command."""
    r = cmd({"cmd":"frame"})
    # frame reply shape unknown; try common keys
    for k in ("frame","frame_count","count","value"):
        if isinstance(r, dict) and k in r:
            try: return int(r[k])
            except Exception: pass
    return r

def main():
    """Entry point: run the watch/nav loop until timeout or exit condition."""
    last = None; same = 0; dumped = False
    out = open(OUT, "w", buffering=1)
    while True:
        f = frame_no()
        fn = f if isinstance(f, int) else None
        if fn is not None and fn == last:
            same += 1
        else:
            same = 0; last = fn
        # stall = same frame for >=6 polls (~3s) OR connection dead after having frames
        if (same >= 6 or (fn is None and last is not None and same >= 3)) and not dumped:
            rec = {"stall_frame": last, "polls_same": same, "t": time.time()}
            for c in DIAG:
                rec[c] = cmd({"cmd": c})
            out.write(json.dumps(rec) + "\n")
            dumped = True
            print("DUMPED at frame", last, file=sys.stderr)
            # keep polling a bit in case it recovers / multiple stalls
            time.sleep(2)
            dumped = False
            same = 0
        time.sleep(0.5)

if __name__ == "__main__":
    main()
