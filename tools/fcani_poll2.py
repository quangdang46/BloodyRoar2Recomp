#!/usr/bin/env python3
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 4890
def cmd(c,timeout=10):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    for _ in range(80):
        try: s=socket.create_connection(('127.0.0.1',PORT),3); break
        except OSError: time.sleep(0.1)
    else: return {}
    s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(timeout); b=b''
    try:
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'ok' in d or 'error' in d: break
            except: pass
    except Exception: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except: return {}
out=open('/tmp/fcani_poll2.log','w')
t0=time.time(); last=None; lastdump=0; seen_w=set(); seen_p={}
WATCH={'0x1F800118','0x1F80011C','0x1F800220','0x1F800224','0x1F800114','0x1F80021C'}
while time.time()-t0<240:
    st=cmd({"cmd":"read_ram","addr":"0x1f800000","len":4})
    if not st.get('ok'): time.sleep(0.3); continue
    gs=int(st['hex'][0:2],16); ph=int(st['hex'][2:4],16)
    if (gs,ph)!=last:
        out.write("t=%.1f gs=%d ph=%d\n"%(time.time()-t0,gs,ph)); out.flush(); last=(gs,ph)
    if time.time()-lastdump>2:
        lastdump=time.time()
        w=cmd({"cmd":"wtrace_dump","count":400})
        for e in w.get('entries',[]):
            if e.get('addr') in WATCH and e.get('seq') not in seen_w:
                seen_w.add(e.get('seq'))
                out.write("W %s %s->%s pc=%s ra=%s frame=%s a0=%s a1=%s\n"%(e['addr'],e['old'],e['new'],e['pc'],e['ra'],e['frame'],e['a0'],e['a1'])); out.flush()
        p=cmd({"cmd":"pc_probe_dump"})
        for s in p.get('slots',[]):
            pc=s['pc']
            if seen_p.get(pc)!=s['count']:
                seen_p[pc]=s['count']
                out.write("P %s count=%s last_t0=%s last_v0=%s last_fp=%s frame=%s\n"%(pc,s['count'],s.get('last_t0'),s.get('last_v0'),s.get('last_fp'),s.get('last_frame'))); out.flush()
        for sm in p.get('samples',[]):
            out.write("PS %s frame=%s t0=%s fp=%s v0=%s\n"%(sm['pc'],sm['frame'],sm['t0'],sm['fp'],sm['v0'])); out.flush()
    time.sleep(0.2)
out.close()
