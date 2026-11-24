# Mode (1,4) tally wedge — RESOLVED (clean-port bug, not faithful guest)

## Verdict

The `gs=8, dat1=0x0b` park was a **clean-port mistranslation**, not faithful
guest behavior. `hud_state_machine`'s `mode1body` dropped the guest's
unconditional `mode = 2` store (a MIPS delay-slot write), so `mode` stayed
`1` and `modechk → mode1body` re-ran forever. Fixed in `game/save.c`.

## Root cause

`hud_state_machine @0x8016e21c` keeps TWO state locals:

- `fp`  — display/frame state (0,1,2,0xf0,0xf1,0xff), drives the loop-top
  `case0/case1/case2` dispatch.
- `mode`— the `a0` argument copied to `sp+0x98` at `0x8016E258`
  (`sw $a0,0x98($sp)`), drives `modechk → mode0body/mode1body/mode2sel`.

The tally body (`mode1body`, guest `0x8016F0FC`–`0x8016F148`):

```
0x8016F0FC  lw   $t4, 0x48($sp)        ; sp48 (latched winner flag)
0x8016F100  li   $t5, 2
0x8016F104  beq  $t4, $zero, ->poll    ; if (sp48==0) skip body
0x8016F108  sw   $t5, 0x98($sp)        ; DELAY SLOT — mode = 2, ALWAYS
; --- sp48 != 0 path (0x8016F10C) ---
0x8016F138  sw   $zero, 0x98($sp)      ; mode = 0
0x8016F13C  sh   $zero, 0x8e($s5)      ; g+0x8e = 0
0x8016F140/148  sh  (table[..]<<3),0x30($sp)  ; tally working value
```

Because `sw $t5,0x98($sp)` is the **delay slot** of the `sp48==0` branch, it
executes on BOTH paths:

- `sp48==0` → `mode = 2` → next `modechk` → `mode2sel` → `fp=0xf0` →
  `fp=0xff` → `return` (tally exit).
- `sp48!=0` → `mode = 0` (overwrites the 2) → `mode0body` (stats pages).

The clean port had only `if (sp48==0) goto poll; mode = 0; …` — it wrote
`mode=0` on the `sp48!=0` path but **never wrote `mode=2`**, so the
`sp48==0` path left `mode==1` → infinite `mode1body` → the wedge.

## Fix (`game/save.c` `mode1body`)

```c
mode = 2;                 /* guest delay-slot store, unconditional */
if (sp48 == 0) goto poll; /* -> mode==2 -> mode2sel -> tally exit */
mode = 0;                 /* guest 0x8016F138, sp48!=0 path only */
*(uint16_t *)(g + 0x8e) = 0;
*(uint16_t *)(g + 0x30) = …;
goto poll;
```

## Runtime proof (port 9977, `tools/row3_experiment.py p1only`)

Before: `dat1` parked at `0x0b` forever after the first round tally.
After: `dat1` cycles `03 → 04 → 08 (battle) → 0b (tally) → 03 …` — the
tally now exits every round via `mode2sel`. `sp48==0` (side at
`0x1f8000e8` leads `e4` 3–2) takes the fixed path each time.

## Notes / follow-ups

- `g+0x30` in clean is used as storage for the guest's `sp+0x30` **stack
  local** (tally working value: inc `0x8016EF60`, mask `0x8016EF78`,
  set `0x8016F148`). The guest never writes `g+0x30`/`0x1f800030` in this
  function. Clean's use is self-consistent across 8 sites and displays
  correctly — a clean-room storage substitution, but worth auditing if
  `0x1f800030` is read by other code.
- The match kept cycling rounds (`dat1` never left `gs=8`) with `e4` at
  3–2. Match-end is decided elsewhere (`menu.c:7889` tick:
  `s1[0xe4] >= MB8(c0+4)` → phase `0xa`/`0x9`). Whether mode (1,4)
  legitimately loops (endless/needs-2P) or has a separate match-end issue
  is **open** — the tally wedge itself is resolved.
