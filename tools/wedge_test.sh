#!/bin/bash
# wedge_test.sh <commit> <port> — checkout commit, rebuild, boot-probe, report WEDGE/CLEAN/CRASH
set -e
C="$1"; PORT="${2:-19500}"
cd /Users/tranquangdang21/Projects/br2-recomp
git checkout -q "$C" || { echo "CHECKOUT_FAIL $C"; exit 1; }
cd build
cmake .. > /tmp/bisect_cmake.log 2>&1 || { echo "CMAKE_FAIL $C"; exit 1; }
cmake --build . -j8 > /tmp/bisect_build.log 2>&1 || { echo "BUILD_FAIL $C"; exit 1; }
cd ..
./tools/bisect_keep.sh "" "$PORT" 55 > /tmp/bisect_probe.txt 2>&1 || true
cat /tmp/bisect_probe.txt
STATE=$(grep -o 'state=[0-9a-f]*' /tmp/bisect_probe.txt | cut -d= -f2)
VD=$(grep -o 'vblank_deliver=[0-9]*' /tmp/bisect_probe.txt | cut -d= -f2)
VA=$(grep -o 'vblank_ack=[0-9]*' /tmp/bisect_probe.txt | cut -d= -f2)
if [ -z "$STATE" ]; then echo "RESULT $C: NO_STATE"; exit 0; fi
# wedge signature: gs stuck 0x20/0x60 in high byte + vblank storm (deliver >> ack, deliver > 100k)
if [ "${VD:-0}" -gt 100000 ] && [ "$STATE" != "0000000000000000" ]; then
  echo "RESULT $C: WEDGE state=$STATE vd=$VD va=$VA"
elif [ "$STATE" = "0000000000000000" ]; then
  echo "RESULT $C: CRASH/BOOT_STALL state=$STATE vd=$VD va=$VA"
else
  echo "RESULT $C: CLEAN state=$STATE vd=$VD va=$VA"
fi
