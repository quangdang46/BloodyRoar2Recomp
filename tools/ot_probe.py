#!/usr/bin/env python3
# ot_probe.py — poll OT state during versus; dump ot_push ring + OT slots.
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 5250
def cmd(c):
    try:
        s=socket.create_connection(('127.0.0.1',PORT),2); s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(2); b=b''
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None: break
            except: pass
        s.close()
        try: return json.loads(b.split(b'\n')[0])
        except: return {}
    except Exception as e: return {'err':str(e)}
def rd(a,n):
    r=cmd({'cmd':'read_ram','addr':hex(a),'len':hex(n)})
    return bytes.fromhex(r['hex']) if r.get('ok') and r.get('hex') else b''
def w32(b,o): return int.from_bytes(b[o:o+4],'little') if len(b)>=o+4 else None
t0=time.time()
while time.time()-t0<160:
    st=rd(0x1f800000,0xa0)
    if len(st)<0xa0: time.sleep(0.2); continue
    gs,ph=st[0],st[1]
    if gs!=8: time.sleep(0.2); continue
    # versus: dump OT state
    otbase=w32(rd(0x1f800008,4),0) or 0
    otidx =w32(rd(0x1f80000c,4),0) or 0
    # OT0 @0x801e4110, OT1 @0x801e6260 ; slot 0x16 -> +0x58 ; draw-ot +0x207c
    ot0=rd(0x801e4110+0x58,4); ot1=rd(0x801e6260+0x58,4)
    d0=rd(0x801e4110+0x207c,8); d1=rd(0x801e6260+0x207c,8)
    fr=cmd({'cmd':'frame'}).get('frame')
    print("t=%.1f gs=%d ph=%d frame=%s otbase=%08x idx=%d OT0[16]=%s OT1[16]=%s d0=%s d1=%s"%(
        time.time()-t0,gs,ph,fr,otbase,otidx,
        ot0.hex() if ot0 else '?', ot1.hex() if ot1 else '?',
        d0.hex() if d0 else '?', d1.hex() if d1 else '?'),flush=True)
    # dump ot_push ring tail (0x801fd000, 128*16)
    ring=rd(0x801fd000,0x800)
    if ring:
        # find max seq
        best=-1; entries=[]
        for i in range(128):
            o=i*16
            pkt=w32(ring,o); sl=w32(ring,o+4); ob=w32(ring,o+8); sq=w32(ring,o+12)
            if sq is not None and sq>best: best=sq
            entries.append((sq,pkt,sl,ob))
        entries.sort()
        tail=entries[-12:]
        print("   ring tail:", " ".join("sq%d:p%x:s%x:b%x"%(sq or 0,(pkt or 0)&0xffffff,(sl or 0)&0xff,(ob or 0)&0xffffff) for sq,pkt,sl,ob in tail),flush=True)
    time.sleep(0.15)
print("done")
