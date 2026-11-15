#!/usr/bin/env python3
"""Poll psxrecomp debug server for device/event state -> JSONL log.
Single-threaded server: keep commands small, tolerate errors.
Usage: diag_poll.py <port> <outfile> [interval_ms]
"""
import socket, json, sys, time

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4382
OUT = sys.argv[2] if len(sys.argv) > 2 else "/tmp/diag.jsonl"
IV = float(sys.argv[3]) if len(sys.argv) > 3 else 0.35

CMDS = [
    "frame", "cdrom_state", "dma_state", "irq_state", "vblank_rate",
    "spu_ta", "spu_events", "hle_dump", "evcb_snapshot", "bioscall_dump",
    "cdrom_command_history", "dma_cdrom_history", "cycles_to_next_event",
]

def cmd(c, timeout=4):
    try:
        s = socket.create_connection(("127.0.0.1", PORT), timeout)
        s.sendall((json.dumps(c) + "\n").encode())
        s.settimeout(timeout)
        buf = b""
        while True:
            chunk = s.recv(1 << 16)
            if not chunk:
                break
            buf += chunk
            try:
                d = json.loads(buf.split(b"\n")[0])
                if "ok" in d or "error" in d:
                    break
            except Exception:
                pass
        s.close()
        try:
            return json.loads(buf.split(b"\n")[0])
        except Exception:
            return {"raw": buf[:200].decode("utf-8", "replace")}
    except Exception as e:
        return {"err": str(e)}

def main():
    out = open(OUT, "a", buffering=1)
    n = 0
    while True:
        rec = {"t": time.time(), "n": n}
        for c in CMDS:
            r = cmd({"cmd": c})
            rec[c] = r
        out.write(json.dumps(rec) + "\n")
        n += 1
        time.sleep(IV)

if __name__ == "__main__":
    main()
