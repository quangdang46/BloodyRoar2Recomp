#!/usr/bin/env python3
# nav_to_fight.py — drive headless BloodyRoar2_Recompiled (debug :4382) from
# title -> mode menu -> 2P VS -> char select -> fight, input-only (no RAM
# pokes). Proven sequence from bloody-roar2/agent_mode.py menu_to_vs2p,
# with corrected input timing found via runtime pad/decoded-state tracing:
#   title: HOLD START ~30 frames (a 3-frame pulse is too short to survive
#          the title's brief valid-sample window and usually lands in
#          attract gs=6 instead of the mode menu gs=5,ph=2)
#   menu : press DOWN until (0x9b,0x9c)==(2,1) [2P VS], then HOLD START
#          ~40 frames to confirm
#   vs   : gs=8,ph==6 -> HOLD CROSS on BOTH pad slots ~30 frames (char lock)
#          gs=8,ph==7 -> HOLD START on both slots ~30 frames (pad gate)
#          gs=8,ph==8 -> fight init reached; keep watching for frame advance
# gs=8 ph=241 (0xF1) is a NORMAL transition stage, not a stall.
# Active-low pad words. Requires p2_device set so both SIO pads connect.
# STATUS (post CD-stall/ph=2/scriptop_ret fixes): attract demo (gs=6) now
# executes a full fight round. Versus nav (gs=8) is UNVERIFIED on the
# current build — needs a fresh run once the blocker below is fixed.
# CURRENT shared blocker is the x28 callee-saved stale-restore crash at
# battle_intro_flow state 0xf0 (game/menu.c ~1969) — hits attract
# post-round and will hit versus the same way.
# OLD blocker (resolved): FAIL-FAST unknown dispatch addr=0x01000000
# ra=0x80126664 from a BIOS TestEvent spin in the cd_seek path — the CD
# read-completion typos that caused it are fixed (see ACCURACY_BURNDOWN
# N+36/N+37/N+105). Do NOT treat ph=8 alone as success — verify the frame
# counter keeps increasing after the gate AND survives the 0xf0 round-end.
import socket,json,time,sys
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4370
def cmd(c):
    for _ in range(50):
        try: s=socket.create_connection(('127.0.0.1',PORT),10); break
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
fight_frame=None; stall_at=None
while time.time()-t0<150:
    st=rd(0x1f800000,0x9d)
    if len(st)<0x9d: time.sleep(0.3); continue
    gs,ph,sub=st[0],st[1],st[0x9C]; sig=(st[0x9B],st[0x9C])
    fr=cmd({"cmd":"frame"}).get("frame")
    print("stage=%s gs=%d ph=%d sub=%d sig=%s frame=%s"%(stage,gs,ph,sub,sig,fr),flush=True)
    now=time.time()
    if stage=="title":
        # Title window is short; hold START long enough for a valid edge.
        if gs==5 and ph==1:
            if now-sent.get('t',0)>=0.5: press(START,30,0); sent['t']=now
        elif gs==5 and ph==2: stage="menu"
        elif gs==8: stage="vs_init"
        elif gs==6: stage="title"   # missed window -> attract; mashes will retry
    elif stage=="menu":
        if gs==5 and ph==2:
            if sig!=(2,1):
                if now-sent.get('d',0)>=0.5: press(DOWN,3,0); sent['d']=now
            else:
                if now-sent.get('s',0)>=1.0: press(START,40,0); sent['s']=now
        elif gs==8: stage="vs_init"
        elif gs!=5: stage="title"
    elif stage=="vs_init":
        if gs==8 and ph==6:
            for s in (0,1):
                if now-sent.get(('c',s),0)>=1.0: press(CROSS,30,s); sent[('c',s)]=now
        elif gs==8 and ph==7:
            for s in (0,1):
                if now-sent.get(('g',s),0)>=1.0: press(START,30,s); sent[('g',s)]=now
        elif gs==8 and ph==8:
            if fight_frame is None:
                fight_frame=fr
            else:
                print("FIGHT frames %s -> %s"%(fight_frame,fr),flush=True)
                if isinstance(fr,int) and isinstance(fight_frame,int) and fr>fight_frame+30:
                    break
                if stall_at is None: stall_at=now
                elif now-stall_at>10:
                    print("FIGHT STALLED at ph=8 frame=%s"%fr,flush=True); break
        elif gs==5: stage="title"
    time.sleep(0.15)
print("done stage=%s"%stage,flush=True)
try: cmd({"cmd":"quit"})
except: pass
