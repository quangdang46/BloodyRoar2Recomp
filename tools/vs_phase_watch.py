#!/usr/bin/env python3
# versus nav + phase-cell wtrace: arms write-watch on battle phase words
# (+0xb4/+0xb6) and confirm word (+0x92) BEFORE navigation, then dumps every
# write with its guest PC so we can name the P1 phase init writer (or prove
# absence). Usage: vs_phase_watch.py <port> [max_seconds]
import socket,json,time,sys
PORT=int(sys.argv[1]) if len(sys.argv)>1 else 8600
TMAX=float(sys.argv[2]) if len(sys.argv)>2 else 300
def cmd(c):
    """Send a JSON command to the runtime debug server; return the parsed reply ({} on failure)."""
    for _ in range(60):
        try: s=socket.create_connection(('127.0.0.1',PORT),10); break
        except OSError: time.sleep(0.2)
    else: return {}
    s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(15); b=b''
    try:
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None: break
            except Exception: pass
    except Exception: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except Exception: return {}
def rd(a,n):
    """read_ram helper: fetch n guest bytes at address a (b'' on failure)."""
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def press(btn,frames,slot):
    """Inject a pad button press (buttons&0xFFFF) for `frames` frames on `slot`."""
    return cmd({"cmd":"press","slot":slot,"buttons":btn&0xFFFF,"frames":frames})
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF

# arm write watches on the phase/confirm cells up front
print("wtrace_add b4:",cmd({"cmd":"wtrace_add","lo":"0x1f8000b4","hi":"0x1f8000b8"}),flush=True)
print("wtrace_add 92:",cmd({"cmd":"wtrace_add","lo":"0x1f800092","hi":"0x1f800094"}),flush=True)
print("wtrace_add 9b:",cmd({"cmd":"wtrace_add","lo":"0x1f80009b","hi":"0x1f80009d"}),flush=True)

t0=time.time(); stage="title"; sent={}
last_sig=None; last_b=None
snapshots=[]
while time.time()-t0<TMAX:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d:
        time.sleep(0.3); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    b4=int.from_bytes(st[0xb4:0xb6],'little'); b6=int.from_bytes(st[0xb6:0xb8],'little')
    c92=int.from_bytes(st[0x92:0x94],'little')
    now=time.time()
    if (b4,b6,c92)!=(last_b if last_b else (None,None,None)):
        print("PHASE gs=%d ph=%d sub=%d sig=%s b4=%04x b6=%04x c92=%04x t=%.1f"%(
            gs,ph,sub,sig,b4,b6,c92,now-t0),flush=True)
        last_b=(b4,b6,c92)
    if sig!=last_sig:
        print("SIG gs=%d ph=%d sub=%d sig=%s t=%.1f"%(gs,ph,sub,sig,now-t0),flush=True)
        last_sig=sig
    if stage=="title":
        if gs==5 and ph==1: press(START,3,0); time.sleep(0.3)
        elif gs==5 and ph==2: stage="menu"
        elif gs==8: stage="vs_init"
    elif stage=="menu":
        if gs==5 and ph==2:
            if sig!=(2,1):
                if now-sent.get('d',0)>=0.4: press(DOWN,3,0); sent['d']=now
            else:
                if now-sent.get('s',0)>=0.8: press(START,3,0); sent['s']=now; time.sleep(0.3)
        elif gs==8: stage="vs_init"
        elif gs!=5: stage="title"
    elif stage=="vs_init":
        if gs==8 and ph==6 and sub>0:
            for s in (0,1):
                if now-sent.get(s,0)>=1.5: press(CROSS,3,s); time.sleep(0.15); sent[s]=now
        elif gs==8 and ph==7:
            for s in (0,1):
                if now-sent.get(('g',s),0)>=1.0: press(START,3,s); sent[('g',s)]=now
        elif gs==8 and ph==8:
            print("FIGHT ph=8 sig=%s b4=%04x b6=%04x c92=%04x"%(sig,b4,b6,c92),flush=True)
            time.sleep(8)
            break
        elif gs==5: stage="title"
    # stall detect: battle case3 wedge signature
    if gs==8 and ph==3 and b4&0xf00==0x300 and b6==0:
        print("WEDGE-SIG detected t=%.1f"%(now-t0),flush=True)
        time.sleep(3); break
    time.sleep(0.15)
print("=== wtrace ranges ===",flush=True)
print(json.dumps(cmd({"cmd":"wtrace_ranges"}))[:2000],flush=True)
print("=== wtrace_dump (writes to phase cells) ===",flush=True)
r=cmd({"cmd":"wtrace_dump","count":400})
print(json.dumps(r)[:30000],flush=True)
print("=== final state ===",flush=True)
print("rd:",rd(0x1f800090,0x30).hex(),flush=True)
