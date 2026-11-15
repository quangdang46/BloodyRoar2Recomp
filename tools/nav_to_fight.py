#!/usr/bin/env python3
# nav_to_fight.py — drive headless BloodyRoar2_Recompiled (debug :4382) from
# title -> mode menu -> 2P VS -> char select -> fight, input-only (no RAM
# pokes). Proven sequence from bloody-roar-2/agent_mode.py menu_to_vs2p:
#   title: mash START until gs=5,ph=2 (mode menu)
#   menu : press DOWN until (0x9b,0x9c)==(2,1) [2P VS], then START
#   vs   : gs=8,ph==6 -> CROSS on BOTH pad slots (char lock)
#          gs=8,ph==7 -> mash START on both slots (pad gate)
#          gs=8,ph==8 -> fight live
# Active-low pad words. Requires p2_device set so both SIO pads connect.
# NOTE: reaching ph==8 currently triggers a guest pc=0 exception storm in
# fight init (see freeze_check pczero_count) — the versus->fight handoff bug.
import socket,json,time,sys
def cmd(c):
    for _ in range(50):
        try: s=socket.create_connection(('127.0.0.1',4382),10); break
        except OSError: time.sleep(0.2)
    else: return {}
    s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(10); b=b''
    try:
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try:
                d=json.loads(b.split(b'\n')[0])
                if 'hex' in d or d.get('ok') is not None: break
            except: pass
    except: pass
    s.close()
    try: return json.loads(b.split(b'\n')[0])
    except: return {}
def rd(a,n):
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":n})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b""
def press(btn,frames,slot): return cmd({"cmd":"press","slot":slot,"buttons":btn&0xFFFF,"frames":frames})
START=0xFFF7; CROSS=0xBFFF; DOWN=0xFFBF
t0=time.time(); stage="title"; sent={}
while time.time()-t0<150:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d: time.sleep(0.3); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    print("stage=%s gs=%d ph=%d sub=%d sig=%s"%(stage,gs,ph,sub,sig),flush=True)
    now=time.time()
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
            print("FIGHT REACHED",flush=True); break
        elif gs==5: stage="title"
    time.sleep(0.15)
