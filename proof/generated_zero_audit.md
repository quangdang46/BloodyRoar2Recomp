# Generated-dispatch audit — br2_call_generated / psx_dispatch_call sites

Audit of every path by which clean C can reach the generated/interpreted
implementation, post batch-9 (self-delegate count = 0). Question: are there
any remaining sites that bypass `clean_try_dispatch` and force generated
execution where a clean (timed `cpst_`) body now exists?

## Method

```
grep -rn "br2_call_generated\s*(" src/ game/ --include="*.c" | grep -v "extern\|define"
grep -rn "psx_dispatch_call\s*("   game/*.c | grep -v extern
grep -rn "\bcall_guest4\?\s*(\|br2_call_clean\s*(" game/*.c | wc -l
```

Plus per-site coverage check: every literal delegate target was looked up
in `game/clean_cps.map` (CLEAN_ENT_CPS routes dispatch to `cpst_` bodies
before fnmap wrappers are consulted — `_by_addr` in gen_clean_table.py
is overwritten by clean_cps.map rows).

## Findings

### 1. br2_call_generated literal sites — 20, ALL shadowed dead code

All 20 are whole-body §4 self-delegates in peer files (fn addr == target):

| file | sites | targets | dispatch route now |
|---|---|---|---|
| game/gfx_emit_p01.c | 5 | 0x8018BC2C/BC78/BCB4/BCD0/BCE4 (pad_slot_walk aliases) | cpst_8018bba8 / cpst_8018bc78 (batch 8) |
| game/gfx_emit_p07.c | 9 | 0x8018E86C/E8CC/E8D0/E904/E910/EBC0/EBD4, 0x8018EF6C, 0x8018F220 | cpst_8018e590 / cpst_8018e8d0 / cpst_8018ef6c (batches 3/8/9) |
| game/w2_e06.c | 6 | 0x8018371C/837B0/838B0/838E8/83920/83948 (scriptop_wait fragments) | cpst_80183664 (batch 8); 83920/83948 -> peer clean-C scriptop_wait_atk3_self/foe |

**Every target addr has a clean_cps.map row.** Guest dispatch
(psx_game_backend -> clean_try_dispatch) resolves these addrs to the
clean bodies BEFORE the fnmap wrappers are consulted, so the delegate
bodies are unreachable via guest control flow. Verified: zero direct C
callers of any of the 20 function names outside their own files/fnmap.

The delegates are NOT a fidelity bug — they are unreachable. Removing or
re-pointing them is cosmetic (peer-file scope); left for the orchestrator.

### 2. br2_call_generated framework uses — intentional

- `src/clean_bridge.c:514` — the definition itself.
- `src/clean_bridge.c:615` — `gpu_invoke`, the libgpu syscall gateway:
  dynamic fn from the game's GPU table, deliberately generated-only
  (BIOS/kernel-boundary contract documented at the definition).

### 3. psx_dispatch_call direct sites — 3, all clean-first fallbacks

`game/bind.c:1883` (bind_jal), `game/cd.c:76` (cd_jal), `game/ev.c:1644`
(ev_jal) — each is the `else` branch AFTER `clean_try_dispatch(cpu, fn)`
fails inside a jal-faithful helper (no arg restore, no $t0 write — the
correct `jal` contract). These are clean-first by construction.

### 4. Clean-first dispatch helpers — by design

call_guest (2-arg), call_guest4, br2_call_clean: 107 + 46 + 37 call sites
in game/*.c — all try `clean_try_dispatch` first, fall back to
`psx_dispatch_call`. These are the sanctioned path.

## Verdict

**Zero reachable generated-bypass sites.** The only forced-generated
paths are (a) shadowed self-delegate bodies that guest dispatch never
reaches, and (b) the intentional gpu_invoke/BIOS-kernel contract. Every
guest-reachable CD/IRQ-region addr now resolves through clean dispatch —
`cpst_` timed CPS where registered, clean-C ports elsewhere.

Residual generated execution in 0x8018–0x801B is now purely the
unconverted parent functions (~1347 generated-only piece-groups, mostly
large unnamed bodies — the data-pointer frontier), not delegate bypass.
