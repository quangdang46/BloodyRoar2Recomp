# Versus frame-divisor residual — post-CD-fix status (N+105)

Question: is the documented "0.1 fps fight-frame progression / scratchpad
byte `0x1F800019` stuck 0" still live after `be17669` (armed-IRQ-presentation
deadline fix)?

## Verdict

**The frame-divisor issue is NOT live. It is stale / resolved as fallout of
the CD wedge fix (`be17669`, documented in `ee4dcea`).** No mistranslation in
the divisor path exists. The current versus stall is a *different,
downstream* blocker: the battle spawn phase waits on fighter asset loads
that the still-imperfect CD consumer path has not delivered.

## 0x1F800019 provenance (complete)

Guest scratchpad RAM `0x1F800000–0x1F8003FF` — not hardware.

- **Only writer (clean tree):** `scroll_cfg_set()` — `game/save.c:1490`.
  `param_2 == 0` → byte = 0; else byte = `(char)(0x100 / param_2) + 1`
  (divisor 8 → `0x21`). All known callers pass `(0,0)` or `(1|2, 8)`.
- **Only reader (clean tree):** `frame_divisor_get()` — `game/menu.c:2903`,
  now a direct `*(uint8_t *)guest_at(0x1f800019)` (the bound-pointer
  indirection was already removed because it dereferenced a wild address
  inside an inlined `battle_intro_flow` copy).
- **Gate semantics — 0 is the PASS value, not a block:**
  `battle_frame()` case 0 (`game/menu.c:8050`):
  `if (frame_divisor_get() != 0) goto L_tail;` — a *nonzero* divisor skips
  bringup. `0x1F800019 == 0` therefore lets the battle proceed; it is only
  nonzero during scroll-configured menu/attract stretches.

## Runtime evidence (headless, port 8125, post-`be17669` build)

- `0x1F800019 = 0x00` while `gs=8`, `ph(byte 0x1f800001)=3` — expected:
  versus/attract battle path calls `scroll_cfg_set(0,0)`, and 0 passes the
  case-0 gate.
- Host `frame` advanced 24054 → 25112 in ~0.5 s ≈ **211 fps** — this is the
  turbo/fast-forward ceiling (`PSX_FAST_FORWARD_SPEED=max`), i.e. guest
  vsync waits complete every emulated frame; **not** a 0.1 fps wedge.
- `gpu_state` cumulative `gp0_draw ≈ 3.94 M`, `gp0_env ≈ 324 k`,
  `gp0_copy ≈ 63.6 k` — GPU command generation is live, not frozen.
- phase_profile: ~50 % `vsync_wait_n` (0x8019a5d8) + ~37 %
  `cd_timer_read` (0x8019a460, generated-delegated) — normal
  frame-synced waiting, not a spin.

## Where the versus flow actually stalls now

`gs=8` → `battle_flow_state_machine` → (mode byte `0x1f80009b = 0`,
attract/arcade battle after versus select reset) → `battle_frame()`
internal `for(;;)` dispatcher on `0x1f800001`.

Byte is stuck at **3** = `L_case3` (`game/menu.c:8099`, guest `0x8014D134`):
decrements the `sp+0x2c` frame counter, calls
`fighter_spawn_setup(0x1f800000, engA, engB, sp+0x18, 0)` — verified
line-for-line against `generated/SLES_017.22_full_15.c:15161` (the earlier
awk-filtered "goto L_tail only" reading was wrong; the clean body is
faithful, including the `go` dead-variable which is a decompiler artifact —
generated uses `gpr8=1` for the `(rec&1)==1` test, matching clean).

`fighter_spawn_setup` (`game/menu.c:3735`) advances the phase byte only
when `rec+0x10==1 && (battle+0xb4/b6 phase & 0xf00) == 0x700`.
Runtime values:

- `0x1f8000b4 = 0x0300` → spawn phase **0x300, needs 0x700** — fighter
  assets have not finished loading.
- `0x1f80009b = 0`, `0x1f800001 = 03` (frozen across minutes).
- Engine ptrs `0x801f10c0/c4` are allocated (`0x801d660c/0x801d71fc`) —
  battle-init state0 completed; `s8c = 0xffff` — bringup done.

So the chain is: **CD consumer stall → fighter data never lands → spawn
phase pinned at 0x300 → phase byte pinned at 3 → battle never reaches the
fight emit.** This matches the earlier `cdrom_state` capture:
`reading=0`, pending cmd `0x09` inactive, `last_lba ≈ 8769/8801`,
`ring_dropped=1`, `status=0x02` — a separate, still-open CD read/consumer
issue downstream of the IRQ-presentation fix (see `cd_seek_blocker.md`,
`cd_wedge_mistranslation.md`).

## Conclusion

- Frame-divisor "stuck 0 → 0.1 fps": **resolved/stale.** Divisor 0 is the
  correct battle value; frames and GPU work advance at turbo rate.
- No faithful fix to invent: the divisor path is already correct.
- **Next blocker (not this task's scope):** CD consumer/read path —
  inactive pending command `0x09`, pinned LBA, spawn phase 0x300 vs
  required 0x700. That is the faithful-fix target for versus combat.
