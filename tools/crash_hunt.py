#!/usr/bin/env python3
# crash_hunt.py — launch N headless instances, drive each with
# arcade_fight_mash.py, auto-detect the ~frame-1630 0x29CC crash / frame
# stall, and dump decisive diagnostics (wtrace/rwatchpc/rings/mem) to
# proof/crash_<port>.json. Kills siblings once a crash is confirmed
# (or keeps them with --keep).
import socket, json, sys, time, os, subprocess, signal, glob

ROOT = "/Users/tranquangdang21/Projects/br2-recomp"
BIN = os.path.join(ROOT, "build/BloodyRoar2_Recompiled")
DISC = "/Users/tranquangdang21/Projects/bloody-roar-2/Game.cue"
TOML = os.path.join(ROOT, "game_1pad.toml")
OUTDIR = os.path.join(ROOT, "proof")

def cmd(port, c, timeout=10):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
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

def rd(port, a, n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r = cmd(port, {"cmd": "read_ram", "addr": hex(a), "len": n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

def u32(b, o):
    """Extract a little-endian u32 at offset o (0 if short)."""
    return int.from_bytes(b[o:o+4], 'little') if len(b) >= o+4 else None

def deref_chain(port):
    """Walk *0x108 -> pcb -> pcb[0] (current TCB candidate)."""
    out = {}
    w108 = rd(port, 0x108, 4)
    p108 = u32(w108, 0) if w108 else None
    out['w108'] = hex(p108) if p108 is not None else None
    if p108:
        phys = p108 & 0x1FFFFFFF
        wp = rd(port, phys, 4)
        pp = u32(wp, 0) if wp else None
        out['pcb_ptr'] = hex(phys)
        out['pcb0'] = hex(pp) if pp is not None else None
        # also dump a few words around pcb
        if pp is not None:
            tcb = pp & 0x1FFFFFFF
            out['tcb_dump'] = rd(port, tcb, 0xA0).hex() if tcb < 0x200000 else None
    # TCB region 0x8FC0-0x9100
    out['kernel_low'] = rd(port, 0x8FC0, 0x140).hex()
    # stack region
    out['stack'] = rd(port, 0x1FFDC0, 0xC0).hex()
    return out

def dump_diag(port, tag):
    """Collect registers/chain diagnostics under a tag."""
    d = {"tag": tag, "t": time.time(), "port": port}
    d["regs"] = cmd(port, {"cmd": "get_registers"}, 15)
    d["chain"] = deref_chain(port)
    d["irqctx"] = cmd(port, {"cmd": "irqctx_ring", "count": 64}, 20)
    d["sp_ring"] = cmd(port, {"cmd": "sp_ring", "count": 64}, 20)
    d["tcb_ring"] = cmd(port, {"cmd": "thread_ctx_ring", "count": 64}, 20)
    d["dispatch"] = cmd(port, {"cmd": "dispatch_tail", "count": 64}, 20)
    d["fn_entry"] = cmd(port, {"cmd": "fn_entry_tail", "count": 64}, 20)
    # wtrace: rich armed ranges (0x8FD0 slot, 0x108 cell, stack ctx region)
    d["wtrace_108"] = cmd(port, {"cmd": "wtrace_dump",
        "addr_lo": "0x108", "addr_hi": "0x10C", "newest": 1, "count": 64}, 30)
    d["wtrace_8fd0"] = cmd(port, {"cmd": "wtrace_dump",
        "addr_lo": "0x8FD0", "addr_hi": "0x8FD8", "newest": 1, "count": 64}, 30)
    d["wtrace_all_8fd0"] = cmd(port, {"cmd": "wtrace_all_dump",
        "addr_lo": "0x8FD0", "addr_hi": "0x8FD8", "newest": 1, "count": 32}, 60)
    d["rwatchpc"] = cmd(port, {"cmd": "rwatchpc_dump", "count": 65536, "newest": 1}, 60)
    d["gwatch"] = cmd(port, {"cmd": "gwatch_dump", "count": 256, "newest": 1}, 30)
    # stack-window probes (armed below): every write + every $ra load in the
    # kernel-frame collision zone around 0x801FFE58.
    d["wtrace_stack"] = cmd(port, {"cmd": "wtrace_dump",
        "addr_lo": "0x1FFE40", "addr_hi": "0x1FFE80", "newest": 1, "count": 1024}, 30)
    d["disp_ring"] = cmd(port, {"cmd": "disp_ring", "count": 128}, 20)
    d["wtrace_all_stack"] = cmd(port, {"cmd": "wtrace_all_dump",
        "addr_lo": "0x1FFE58", "addr_hi": "0x1FFE5C", "newest": 1, "count": 64}, 60)
    # cd_ctx_save frame 0x801BCD40-0x801BCD80: writes to saved-sp cell
    # 0x1BCD54 carry new_val = the SP the CD coroutine is resumed on.
    d["wtrace_ctx"] = cmd(port, {"cmd": "wtrace_dump",
        "addr_lo": "0x1BCD40", "addr_hi": "0x1BCD80", "newest": 1, "count": 512}, 30)
    d["wtrace_all_ctx"] = cmd(port, {"cmd": "wtrace_all_dump",
        "addr_lo": "0x1BCD40", "addr_hi": "0x1BCD80", "newest": 1, "count": 256}, 60)
    # postmortem RAM reads (may fail if the debug server is already dead).
    d["mem_ctx"] = cmd(port, {"cmd": "mem_words",
        "addr": "0x801bcd50", "count": 16}, 10)
    d["mem_stkwin"] = cmd(port, {"cmd": "mem_words",
        "addr": "0x801ffe40", "count": 16}, 10)
    path = os.path.join(OUTDIR, "crash_%d.json" % port)
    with open(path, "w") as f:
        json.dump(d, f, indent=1)
    print("DIAG DUMP -> %s" % path, flush=True)
    return path

def arm(port):
    # rich wtrace ONLY on the two decisive kernel cells — writes are rare
    # (thread switch / init) so the 256K ring retains the whole run and
    # names the writer (pc/func/sp) if the corruptor uses cpu->write_word.
    cmd(port, {"cmd": "wtrace_arm", "lo": "0x108", "hi": "0x10C"})
    cmd(port, {"cmd": "wtrace_arm", "lo": "0x8FD0", "hi": "0x8FD8"})
    # decisive: EVERY $ra load (rt=31) sourced from the kernel-stack collision
    # window 0x1FFE00-0x1FFE80. vsync_wait_n's `lw ra,24(sp)` lands here; the
    # crash-time entries show which load returned the wild 0xEE93-class value.
    cmd(port, {"cmd": "rwatchpc_arm", "lo": "0x1FFE00", "hi": "0x1FFE80", "rt": 31})
    # decisive #2 (cd_ctx_save hypothesis — FALSIFIED on crash_9101: saved sp
    # is 0x801BDD30 thread stack). Keep the ctx write watch anyway.
    cmd(port, {"cmd": "wtrace_arm", "lo": "0x1BCD40", "hi": "0x1BCD80"})
    # decisive #3: EVERY $ra load (rt=31) sourced from the whole collision
    # lane 0x1FFE40-0x1FFE80 — catches vsync_wait_n's epilogue lw ra,24(sp)
    # AND any other frame's ra restore in the zone, with cpu_pc attribution.
    cmd(port, {"cmd": "rwatchpc_arm", "lo": "0x1FFE40", "hi": "0x1FFE80", "rt": 31})
    # wtrace the collision lane 0x1FFE40-0x1FFE80: captures BOTH cd_timer_read
    # lanes' spills (FFE58/60 @sp=FFE48; FFE70/78 @sp=FFE60), vsync_wait_n's
    # ra save (FFE58 @pc=8019A5F8 IF it ever runs at sp=FFE40), and s0/ra
    # slots of the FFE60-lane activation.
    cmd(port, {"cmd": "wtrace_arm", "lo": "0x1FFE40", "hi": "0x1FFE80"})
    # gwatch: mprotect page-watch on the two kernel cells — catches
    # UNTRACED writes (clean-C guest_at stores, DMA) that wtrace misses.
    cmd(port, {"cmd": "gwatch", "addr": "0x80000100", "len": 16})
    cmd(port, {"cmd": "gwatch", "addr": "0x80008FC0", "len": 32})

def main():
    """Entry point: run the watch/nav loop until timeout or exit condition."""
    ports = [int(x) for x in sys.argv[1:]] or list(range(9990, 9998))
    procs = {}
    mash = {}
    for p in ports:
        log = open(os.path.join(OUTDIR, "emu_%d.log" % p), "w")
        pr = subprocess.Popen([BIN, "--game", TOML, "--disc", DISC,
                               "--debug-port", str(p), "--headless"],
                              stdout=log, stderr=subprocess.STDOUT, cwd=ROOT)
        procs[p] = (pr, log)
        # wait for debug port
        for _ in range(100):
            if cmd(p, {"cmd": "frame"}, 3): break
            time.sleep(0.2)
        arm(p)
        mp = subprocess.Popen(["python3", os.path.join(ROOT, "tools/arcade_fight_mash.py"), str(p)],
                              stdout=open(os.path.join(OUTDIR, "mash_%d.log" % p), "w"),
                              stderr=subprocess.STDOUT, cwd=ROOT)
        mash[p] = mp
        print("launched port %d pid %d" % (p, pr.pid), flush=True)
    # monitor
    last_fr = {p: None for p in ports}
    last_chg = {p: time.time() for p in ports}
    dumped = set()
    t0 = time.time()
    while time.time() - t0 < 2400:
        time.sleep(5)
        for p in ports:
            pr, log = procs[p]
            # emulator died?
            if pr.poll() is not None and p not in dumped:
                print("port %d EMU EXIT rc=%s" % (p, pr.returncode), flush=True)
                dump_diag(p, "emu_exit")
                dumped.add(p)
                continue
            r = cmd(p, {"cmd": "frame"}, 8)
            fr = r.get("frame")
            if fr is None:
                if p not in dumped:
                    # maybe livelocked — check regs
                    regs = cmd(p, {"cmd": "get_registers"}, 8)
                    print("port %d frame=None regs=%s" % (p, str(regs)[:200]), flush=True)
                continue
            if fr != last_fr[p]:
                last_fr[p] = fr; last_chg[p] = time.time()
            stall = time.time() - last_chg[p]
            # fatal halt in log?
            fatal = False
            try:
                log.flush()
                with open(os.path.join(OUTDIR, "emu_%d.log" % p), errors="replace") as f:
                    tail = f.read()[-4000:]
                fatal = ("FATAL" in tail or "fatal" in tail) and "DISPATCH" in tail.upper() or "DISPATCH FATAL" in tail
            except Exception:
                pass
            if (stall > 30 or fatal) and p not in dumped:
                print("port %d SUSPECT fr=%s stall=%.0f fatal=%s" % (p, fr, stall, fatal), flush=True)
                dump_diag(p, "stall_or_fatal")
                dumped.add(p)
        if len(dumped) >= 1 and len(sys.argv) > 1 and "keep" not in sys.argv:
            pass
        alive = sum(1 for p in ports if procs[p][0].poll() is None)
        if alive == 0:
            break
    print("hunt done dumped=%s" % sorted(dumped), flush=True)
    # leave everything running for post-analysis; caller kills

if __name__ == "__main__":
    main()
