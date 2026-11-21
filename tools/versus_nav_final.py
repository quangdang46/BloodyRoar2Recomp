#!/usr/bin/env python3
# versus_nav_final.py — Bloody Roar 2 (SLES-01722) 2-player VERSUS navigator.
# Debug-server JSON-lines; PORT = argv[1] (default 4910).
#
# ===================== VERIFIED MENU STRUCTURE =====================
# Title:      gs=5 ph=1  — tap START (~4f) opens the mode menu.
# Mode menu:  gs=5 ph=2  — match_loop() stepper (game/game.c:1449,
#             menu_stepper_state_advance + attract_menu_dispatch,
#             sd.max=7, sel starts at 1). The selection index lives in
#             the stepper descriptor on the stack (desc+8 u16), NOT in
#             the 0x1f800000 block; the practical cursor observable is
#             the live-preview pair 0x1f80009b / 0x1f80009c, which
#             attract_menu_dispatch (game/menu.c:2634) rewrites EVERY
#             FRAME to the highlighted entry's (mode,sub-mode):
#
#   DOWN-order sig (9b,9c) observed at runtime:
#     sel1 -> (0,0)      sel2 -> (2,1)   sel3 -> (1,4)
#     sel4 -> (0,3)      sel5 -> (3,2)   sel6 -> OPTIONS
#                          (sig stays (3,2); sets g_sub_state=4 ->
#                           gs=11 system_config_menu_b, game/game.c:739)
#
# Confirm = HOLD START ~40-60f (short taps only move the cursor).
#
# sel2 "2P VS" -> gs=8 versus flow:
#     ph==6 -> HOLD CROSS both pad slots (char lock)
#     ph==7 -> HOLD START both pad slots (pad gate)
#     ph==8 -> fight; watch 0x1f80009b + frame counter.
#
# ===================== ABOUT MODE BYTE 0x9b==4 =====================
# 0x1f80009b is the battle-type byte. battle_flow_state_machine
# (game/menu.c:755) branches on it: ==4 -> battle_sides_commit (true
# 2P versus input path, also gated at menu.c:5055/6080), else
# post_battle_dispatch. The ONLY clean-source writer of 4 is
# dialog_sel_dispatch_wide (game/save.c:5516): inside
# system_config_menu_b (the OPTIONS screen, gs=11 sub_state=4), row
# sel==1 counter @s4+0x14 == 3 -> 9b=4, then gs=8 ph=0xf0 directly.
# I.e. mode 4 is reached through the OPTIONS dialog's battle-type row,
# NOT through the versus char-select entry (which commits 9b=2,9c=1).
# Whether versus char-select later upgrades 9b->4 in still-generated
# code is UNVERIFIED — every versus transition attempt on this build
# wedged the emulator ("emu busy or frozen") or died with the known
# unknown-dispatch 0x80122DC8 before/at ph=8.
#
# This script implements the proven versus path and reports the mode
# byte live; if the runtime ever writes 4 en route it is logged.
import socket, json, sys, time

PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 4910
START=0xfff7; CROSS=0xbfff; DOWN=0xffbf; UP=0xffdf
LEFT=0xfeff; RIGHT=0xf7ff

def cmd(c, tries=10, to=30):
    for _ in range(tries):
        try:
            s=socket.create_connection(('127.0.0.1',PORT),3)
            s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(to)
            b=b''
            while True:
                x=s.recv(1<<20)
                if not x: break
                b+=x
                try:
                    d=json.loads(b.split(b'\n')[0])
                    if 'hex' in d or d.get('ok') is not None or 'error' in d: break
                except Exception: pass
            s.close()
            try: return json.loads(b.split(b'\n')[0])
            except Exception: return {}
        except Exception: time.sleep(0.5)
    return {}

def rd(a,n):
    r=cmd({"cmd":"read_ram","addr":hex(a),"len":hex(n)})
    return bytes.fromhex(r["hex"]) if r.get("ok") and r.get("hex") else b''

def st():
    b=rd(0x1f800000,0xa0)
    if len(b)<0xa0: return None
    return (b[0],b[1],b[0x9b],b[0x9c])

def press(btn,frames,slot=0):
    return cmd({"cmd":"press","slot":slot,"buttons":btn&0xffff,
                "frames":frames},tries=12,to=90)

def frame_no():
    return cmd({"cmd":"frame"},tries=3,to=15).get('frame')

T0=time.time()
def log(*a): print('t=%5.0f'%(time.time()-T0),*a,flush=True)

# ---------------------------------------------------------------- title
log('waiting for title gs=5 ph=1 ...')
while time.time()-T0<420:
    s=st()
    if s is None: time.sleep(0.6); continue
    gs,ph,m,sub=s
    if gs==5 and ph==2: log('MODE MENU sig=(%d,%d)'%(m,sub)); break
    if gs==5 and ph==1:
        press(START,4); log('title tap'); time.sleep(0.6)
    elif gs in (3,4,6): time.sleep(0.5)          # attract scene
    elif gs==8:
        log('overshot into attract/versus fight gs=8 ph=%d m=%d — '
            'cannot quit; restart the instance'%(ph,m)); time.sleep(2)
    else: time.sleep(0.4)
else:
    log('TIMEOUT waiting for mode menu'); sys.exit(1)

# ------------------------------------------------------------- menu nav
# Step DOWN until the live-preview sig reads (2,1) = sel2 "2P VS".
sig_seen=None
for i in range(12):
    s=st()
    if s is None: time.sleep(0.8); continue
    sig=(s[2],s[3])
    if sig!=sig_seen:
        sig_seen=sig; log('menu sig=%s'%(sig,))
    if sig==(2,1): break
    press(DOWN,12); time.sleep(0.8)
else:
    log('never reached sig (2,1); last',st()); sys.exit(2)

# Hold START to confirm sel2 -> gs=8 versus flow.
press(START,50); time.sleep(1.5)
s=st(); log('post-confirm',s)

# ------------------------------------------------------- versus select
fight_fr=None; last_fr=None; lastchg=time.time()
none_streak=0
for i in range(600):
    s=st()
    if s is None:
        none_streak+=1
        # server starvation fallback: keep pads warm so char-select can't
        # miss its window (CROSS only; START stays phase-gated)
        if none_streak>=3 and i%3==0:
            press(CROSS,20,0); press(CROSS,20,1)
        time.sleep(0.8); continue
    none_streak=0
    gs,ph,m,sub=s
    if m==4:
        log('*** 0x1f80009b==4 observed (gs=%d ph=%d sub=%d) ***'%(gs,ph,sub))
    if gs==8 and ph==6:
        press(CROSS,30,0); press(CROSS,30,1); log('ph=6 CROSS both pads')
        time.sleep(1.0)
    elif gs==8 and ph==7:
        press(START,30,0); press(START,30,1); log('ph=7 START both pads')
        time.sleep(1.0)
    elif gs==8 and ph==8:
        fr=frame_no()
        if fight_fr is None:
            fight_fr=fr; last_fr=fr; lastchg=time.time()
            log('*** FIGHT gs=8 ph=8 mode=%d sub=%d fr=%s ***'%(m,sub,fr))
        elif fr!=last_fr:
            last_fr=fr; lastchg=time.time()
            if isinstance(fr,int) and isinstance(fight_fr,int) and fr>fight_fr+60:
                log('VERSUS FIGHT LIVE: frames %s -> %s, mode 0x9b=%d'
                    %(fight_fr,fr,m))
                break
        if time.time()-lastchg>25:
            log('ph=8 stall fr=%s mode=%d sub=%d'%(fr,m,sub)); break
    elif gs==5:
        log('dropped to title; exiting (restart instance to retry)')
        break
    time.sleep(0.5)

log('done t=%.0f'%(time.time()-T0))
