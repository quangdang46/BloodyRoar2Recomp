#!/usr/bin/env python3
# Watch CD/DeliverEvent starvation: pc_probe + fntrace + mem_words polling.
# Usage: cdstarve_watch.py <port> <logfile> [max_seconds]
import socket, json, time, sys

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 5100
OUT = open(sys.argv[2] if len(sys.argv) > 2 else "/tmp/cdstarve.jsonl", "a", buffering=1)
MAXT = float(sys.argv[3]) if len(sys.argv) > 3 else 3600

def cmd(c, timeout=15, raw=False):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    for _ in range(80):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), timeout); break
        except OSError:
            time.sleep(0.25)
    else:
        return {}
    try:
        s.sendall((json.dumps(c) + '\n').encode())
        s.settimeout(timeout)
        b = b''
        while True:
            try:
                x = s.recv(1 << 20)
            except socket.timeout:
                break
            if not x:
                break
            b += x
            try:
                d = json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None or 'error' in d or 'words' in d or 'armed' in d:
                    break
            except Exception:
                pass
    finally:
        s.close()
    try:
        return json.loads(b.split(b'\n')[0])
    except Exception:
        return {'raw': b[:400].decode('utf-8', 'replace')}

def cmd_multi(c, timeout=15):
    """For multi-line responses (pc_probe_dump): return all lines."""
    for _ in range(80):
        try:
            s = socket.create_connection(('127.0.0.1', PORT), timeout); break
        except OSError:
            time.sleep(0.25)
    else:
        return []
    try:
        s.sendall((json.dumps(c) + '\n').encode())
        s.settimeout(timeout)
        b = b''
        # read until closing '}' line of the json array-ish payload; simpler: read until timeout or ']' line
        while True:
            try:
                x = s.recv(1 << 20)
            except socket.timeout:
                break
            if not x:
                break
            b += x
            if b.rstrip().endswith(b']}') or b.rstrip().endswith(b']'):
                # pc_probe_dump ends with "]}" on last line; fntrace similar
                pass
            # stop when we have a line that is just "]}" or "]"
            lines = b.split(b'\n')
            if len(lines) > 2 and lines[-2].strip() in (b']', b']}') :
                break
            if len(lines) > 2 and lines[-1].strip() == b'}':
                break
    finally:
        s.close()
    return b.decode('utf-8', 'replace')

PCS = [
    # func leaders + func_8019B7E0 INTERNAL block leaders
    # (pc_probe only counts dispatch block leaders, not mid-function PCs)
    0x8019B89C, 0x8019B944, 0x8019B954, 0x8019B7E0,
    0x8019B7FC, 0x8019B82C, 0x8019B830, 0x8019B83C,
    0x8019B850, 0x8019B864, 0x8019B880, 0x8019B88C,
    0x8019AF7C, 0x8019A5D8, 0x801565EC, 0x8018CFDC,
    0x8019CFC0, 0x8019D500, 0x8019D5B0,
]
PCS_STR = ','.join(hex(p) for p in PCS)

FNTARGETS = [
    0x8019B89C, 0x8019B944, 0x8019B7E0, 0x8019B7E8, 0x8019B008,
    0x8019AF7C, 0x8019AF84, 0x801565EC, 0x8018CFDC,
    0x801979F8,  # INTC tbl[2] CDROM INT2 handler guess
]

WATCH = [
    (0x801BDF00, 1), (0x801BDDE0, 1), (0x801BDEE4, 1), (0x801BDEE8, 1),
    (0x801BDE04, 1), (0x801BDDF0, 1),
    (0x801BDDFC, 10),   # DFDC..E20 handler table
]

def log(rec):
    """Append a timestamped line to the log file."""
    OUT.write(json.dumps(rec) + '\n')

def read_words(addr, count):
    """mem_words helper: return the word list for addr/count."""
    r = cmd({'cmd': 'mem_words', 'addr': hex(addr), 'count': count})
    return r.get('words', [])

def main():
    """Entry point: run the watch/nav loop until timeout or exit condition."""
    log({'ev': 'start', 'port': PORT, 'pcs': PCS_STR})
    # arm probes
    r = cmd({'cmd': 'pc_probe_arm', 'pcs': PCS_STR, 'n': 48})
    log({'ev': 'pc_probe_arm', 'r': r})
    for t in FNTARGETS:
        r = cmd({'cmd': 'fntrace_arm', 'target': hex(t)})
        log({'ev': 'fntrace_arm', 't': hex(t), 'r': r})

    prev = {}
    last_frame = -1
    last_frame_t = time.time()
    stall_dumped = False
    poll_n = 0
    t0 = time.time()
    while time.time() - t0 < MAXT:
        poll_n += 1
        rec = {'t': time.time() - t0, 'n': poll_n}
        fr = cmd({'cmd': 'frame'})
        rec['frame'] = fr.get('frame')
        cd = cmd({'cmd': 'cdrom_state'})
        rec['cd_seq'] = cd.get('seq')
        rec['cd_stat'] = cd.get('stat')
        rec['cd_reading'] = cd.get('reading')
        rec['cd_pending'] = cd.get('pending')
        # watched words (one mem_words per contiguous range)
        for addr, cnt in WATCH:
            ws = read_words(addr, cnt)
            rec['w%08X' % addr] = ws
        # transitions vs prev
        trans = []
        flat_prev = {k: v for k, v in prev.items() if k.startswith('w')}
        for addr, cnt in WATCH:
            key = 'w%08X' % addr
            cur = rec[key]
            old = flat_prev.get(key)
            if old is not None and old != cur:
                # find changed indices
                diffs = []
                for i in range(min(len(old), len(cur))):
                    if old[i] != cur[i]:
                        diffs.append('%08X:%s->%s' % (addr + i * 4, old[i], cur[i]))
                trans.append({'key': key, 'diffs': diffs})
        rec['trans'] = trans
        # fps estimate
        f = rec['frame']
        if f is not None:
            dt = time.time() - last_frame_t
            if f != last_frame:
                rec['fps'] = round((f - last_frame) / dt, 2) if last_frame >= 0 else None
                last_frame = f
                last_frame_t = time.time()
            else:
                rec['fps'] = 0.0
        # stall detection: frame unchanged > 15s AND cd seq unchanged
        now = time.time()
        if last_frame >= 0 and (now - last_frame_t) > 15 and not stall_dumped:
            stall_dumped = True
            rec['STALL'] = True
            log(rec)
            dump_all('stall')
            break
        log(rec)
        prev = rec
        time.sleep(2.0)
    log({'ev': 'exit'})

def dump_all(tag):
    """Collect all diagnostic dumps (pc_probe, irq, dma, cdrom, bios) under one tag."""
    out = {'ev': 'dump', 'tag': tag}
    for name in ['pc_probe_dump', 'irq_state', 'dma_state', 'cdrom_state',
                 'bios_svc_trace', 'sp_ring', 'dispatch_tail', 'frame',
                 'fntrace_dump', 'cdrom_command_history', 'cdrom_timing_dump',
                 'cycles_to_next_event', 'current_tcb']:
        r = cmd({'cmd': name}, timeout=20)
        out[name] = r
        OUT.write(json.dumps({'ev': 'dumppart', 'tag': tag, 'name': name,
                              'data': r if isinstance(r, (dict, list)) else str(r)[:2000]}) + '\n')
    OUT.write(json.dumps(out) + '\n')

if __name__ == '__main__':
    main()
    dump_all('end')
    print('watcher done')
