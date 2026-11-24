# Mode (1,4) match-end — VERDICT: faithful, matches DO end (versus-attract loop)

Date: post-b88ea82. Port 4411, `tools/row3_cpuwin2.py` + live `read_ram`/`press`.

## Question

Does menu row-3 mode signature `g+0x9b=0x01, g+0x9c=0x04` ever end, or is
match-end detection broken? Prior observation: `gs=8` rounds cycled with
per-side `+0xe4` counters at 3-vs-2 and no conclusion.

## Answer: it ends — every ~5 s cycle IS a complete match

Live phase trace (`s1+0x92` low byte = `post_battle_dispatch` phase,
`g+1` = `dat1`, side blocks `0x1f8000e8`/`0x1f8001f0`):

```
d1=03 ph=00  round/match setup (side blocks re-init, e4=0)
d1=04 ph=00  stage/intro
  -> d1=08 fight; ph cycles 01->02->07->0c->01->02->07->0c->01->02->0e
     each ph=07 entry: winner/loser e4 += 1 (menu.c:7160-7161 inc2/inc3)
ph=0e (case 14 = vs_confirm_e) 8c ticks ~0x120  <- MATCH-END victory seq
d1=0b                                      <- results tally (hud_state_machine)
d1=03 e4=0                                 <- NEXT match begins
```

One full match = score accumulation `e4` 0-0 -> 1-1 -> 2-2 (draw:
`case 11` equal-score path writes `s1+0x99=0xff`, phase `0xc` continue)
-> 3-2 -> `case 11` tiebreak `e3<e2` -> `L_dwin` -> phase `0xe`
(`side_phase_advance(s1,g2,g3,1)`, `menu.c:7220-7244`) -> victory announce
-> results tally -> next match. `gs` stays 8 because this is the
versus/attract path — it demo-loops matches forever, which is correct.

## The two match-end gates (both faithful)

### 1. `case 11` (phase 0xb) — the one that actually fires

`game/menu.c:7177-7248`. `lim = MB8(MB32(s1+0xc0)+4)`:

- `MB32(0x1f8000c0)` = `0x801c1200` (live dump); record byte `+4` = **2**
  (rounds-to-win threshold; record head `00 00 00 00 02 01 01 03`).
- `e2 = MB8(g2+0xe4)`, `e3 = MB8(g3+0xe4)` — the SAME `+0xe4` counters the
  prior agent watched. Both >= lim -> higher score wins (`0xd`/`0xe`),
  equal -> draw continue (`0xc`, `s1+0x99=0xff`).

Observed final eval: e2=3, e3=2, lim=2 -> both >= lim, `e3<e2` ->
`L_dwin` -> phase `0xe`. Exactly what the trace shows.

### 2. `vs_confirm_c` tail gate (phases 5/6) — faithful, dormant here

`game/menu.c:7884-7895` vs generated `br2eu_func_80153EE8`
(`generated/SLES_017.22_full_18.c` blocks `0x801542B0`-`0x80154348`):

| guest | clean | verdict |
|---|---|---|
| `lh v0,0x8c(s0)`; `slti v0,v0,120`; skip if <0x78 | `MBS16(s0+0x8c) >= 0x78` | identical (signed) |
| `sb 0,0x9e(s0)`; `sh 0,0x8c(s0)` (delay slot, unconditional) | `MW8/MW16` before gate | identical |
| `lh v0,0x94(s0)`; `beqz -> cls 0xb` | `MBS16(s0+0x94) != 0` required | identical |
| `lbu v1,0(s1)`; `beq 0x81 -> cls 0xb` | `s1[0] != 0x81` required | identical |
| `lw v0,0xc0(s0)`; `lbu v1,0xe4(s1)`; `lbu v0,4(v0)`; `sltu` e4<thr -> 0xb | `(uint32_t)s1[0xe4] >= (uint32_t)MB8(MB32(s0+0xc0)+4)` | identical (sltu = unsigned >=) |
| `lbu v0,2(s2)`; `bnez -> cls 0xa` else `0x9` | `cls = (s2[2]!=0) ? 0xa : 9` | identical |
| `sh (lh&0xff00)|cls, 0x92(s0)` | same | identical |

Register map confirmed from prologue: `s2=a0`, `s1=a1`, `s0=a2`
(gpr18/gpr17/gpr16). Call sites `menu.c:7122/7125` pass
`(&DAT_1f8000e8, &DAT_1f8001f0, 0x1f800000)` (case 6 swaps).

Phases 5/6 (vs_confirm_c) are entered from `case 4` ONLY when exactly one
side-block byte0 goes negative (KO/timeout split, `menu.c:7091-7118`).
In this CPU/demo battle both byte0 stay `0x81` (both-negative -> phase
`0x07` round-tally path), so the vs_confirm_c gate legitimately never
runs — match-end flows through `case 11` instead. Not a mistranslation.

## Why the prior agent saw "3 vs 2 and kept going"

`e4` is a PER-MATCH score, reset when the side blocks re-init at match
start (`d1=03`). Score went 1-1, 2-2 (draw -> continue), 3-2 -> match
over. What looked like "kept going" was the NEXT attract match starting
after the results tally — gs stays 8 by design.

## Conclusion

No bug. `vs_confirm_c`'s match-end gate and `case 11`'s threshold check
are both faithful to generated; threshold=2 is the real config value;
matches complete and loop as versus-attract. No code change.
