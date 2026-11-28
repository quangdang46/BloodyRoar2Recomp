#!/bin/bash
# bisect_keep.sh <keep-ranges> <port> [wait_secs]
# Launch headless with PSX_CPS_KEEP=<ranges>, wait, report gs/state + vblank health.
KEEP="$1"; PORT="${2:-19400}"; WAIT="${3:-50}"
cd "/Users/tranquangdang21/Projects/br2-recomp/build"
PSX_CPS_KEEP="$KEEP" ./BloodyRoar2_Recompiled \
  --disc "../disc/Bloody Roar 2 - Bringer of the New Age (Europe).cue" \
  --debug-port "$PORT" --headless --game ../game_1pad.toml > "/tmp/br2_keep_${PORT}.log" 2>&1 &
PID=$!
sleep "$WAIT"
python3 - "$PORT" <<'PY'
import socket, json, sys
port = int(sys.argv[1])
def cmd(c):
    try:
        s=socket.create_connection(('127.0.0.1',port),3)
        s.sendall((json.dumps(c)+'\n').encode()); s.settimeout(8); b=b''
        while True:
            x=s.recv(1<<20)
            if not x: break
            b+=x
            try: json.loads(b.split(b'\n')[0]); break
            except: pass
        s.close(); return json.loads(b.split(b'\n')[0])
    except Exception as e: return {'err':str(e)}
st = cmd({"cmd":"read_ram","addr":"0x1f800000","len":8}).get('hex','?')
df = cmd({"cmd":"defer_state"})
vd = df.get('vblank_deliver'); va = df.get('vblank_ack')
ee = df.get('exception_entries')
print(f"state={st} vblank_deliver={vd} ack={va} exc_entries={ee}")
PY
kill $PID 2>/dev/null; wait $PID 2>/dev/null
