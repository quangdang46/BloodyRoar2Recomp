# cd: 5 jal-target clean ports — decode-bug postmortem + row1 verify

Commit ports five previously-delegated `jal`-target CD functions into
`game/cd.c` (all re-verified insn-by-insn against `generated/`):

- `cd_cmd_queue_load` 0x80192634 — DMA-descriptor arm for the cmd queue.
- `cd_queue_push`     0x80192F14 — ring enqueue + direct-call fast path.
- `cd_queue_drain`    0x801931C4 — ring drain/dispatch (now returns the
  guest value: 1 on entry-status early-out, else (head-tail)&0x3f).
- `cd_reg_stream_push` 0x8019B620 — streaming register push.
- `cd_read_open`      0x80198908 — CdRead Shell_open (Setmode buf now a
  live `br2_guest_frame(0x28)+0x10` guest address).

## The bug that shipped in the first cut

`cd_cmd_queue_load` stored `*cell8438 = *cell8440 + n*4 - 4` — the DMA
descriptor END pointer was derived from the pointer CELL's value instead
of the caller's `a0` arg. Guest truth (full_44.c @0x8019267C-0x80192684):
`v0 = s0<<2 - 4` (s0 = a1 = param_2), `a0 += v0`, `sw a0, 0(*cell8438ptr)`
→ `*P3 = param_1 + param_2*4 - 4`. The wild write corrupted kernel/TCB
memory → HLE scheduler resumed a thread whose saved PC was a heap pointer
(0x000027AC) → `FAIL-FAST unknown dispatch` at frame 352, 0.2% CPU
(psx_fatal_halt nanosleep). Baseline (delegated) build ran clean past
frame 13k — bisect isolated it in minutes.

Second transcription slip in the same review pass: `cd_queue_push`'s
full-queue test is `(head+1) & 0x3f == tail` (guest `addiu`+`andi 0x3f`),
not a raw compare — fixed; matters only at head==63/tail==0 but is now
faithful.

## Verification

- `python3 game/gen_clean_table.py` + `cmake --build . -j8` — clean.
- Headless run: frame 352 wedge GONE; `mode_monitor.py 4374 row1` →
  "t=5 MENU reached fr=13155" (title→menu CD loads all flow through the
  ported paths). Phase-B menu confirm did not select row1 within 40s —
  pad injection verifiably reaches the game (dat1 flips 1↔2 on
  START/CROSS); treated as menu-nav/input quirk, not a CD wedge: gs=5,
  frames advance, cdrom seq=503875, irq_present_max_late=0,
  pending 0x09 is the known completed-Pause residue.
