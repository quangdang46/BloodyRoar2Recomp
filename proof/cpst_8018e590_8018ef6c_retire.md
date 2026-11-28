# cpst_8018e590 / cpst_8018ef6c retirement — printf/sprintf CPS piece rows

Agent: devin-r, branch agent/devin-r (base 8bcaf7ea).

## Verdict: RETIRE (residual CPS coverage), not port

Both targets are the **printf/sprintf engine** — already clean-ported at
their real function entries. The `cpst_*` rows are intra-function CPS
piece/resume PCs, i.e. residual transliteration scaffolding inside
already-ported fn spans (AGENTS.md session-31 rule: "a cps_* symbol whose
addr range sits inside an already-ported fnmap function's span is
residual CPS coverage, not a port target — retire the rows").

### cpst_8018e590 — printf_core (104 rows)

- Guest fn `printf_core` @0x8018E590 (1684B / 125 blocks) is clean-ported
  as a synchronous whole-function C port: `game/gfx.c:1482`
  (`uint32_t printf_core(uint32_t chan, const uint8_t *fmt, const uint32_t *args)`).
  fnmap.c:2926 routes the entry there — the cpst_ rows never covered the
  entry itself (rows start at 0x8018e5c4).
- The 104 row addrs are CPS piece-leader/block PCs inside the printf
  body (format-flag parse, arg fetch, digit-gen, emit tail). Verified:
  **zero `CPS jal ->` targets** into any of the 104 addrs in
  `generated/SLES_017.22_full_*.c` — they are only reachable as CPS
  continuation PCs from within printf's own body.
- Since the entry runs clean synchronous C (no `psx_check_interrupts_at`
  yield, no cpu->pc continuation), the generated printf body never
  executes and no resume can land on a piece addr. The only live
  tail-dispatchers into this range are the `printf_flags_*` /
  `printf_cps_*` piece ports in `game/gfx_emit_p05.c`/`p06.c`, which set
  `cpu->pc = <shared block>` (e.g. 0x8018E60C) and return — those addrs
  now fall through to the generated alias body
  (`br2eu_psx_alias_body_8018E590`), which resumes the identical block
  with live `cpu->gpr[]` state. Faithful.
- All 104 addrs verified present in `generated/SLES_017.22_dispatch.c`
  (alias-body entries) — no PC=0 gap (the session-35 failure mode).

### cpst_8018ef6c — sprintf engine (113 rows)

- The sprintf engine core @0x8018EF6C (2164B / 121 blocks) is reached in
  guest code ONLY through wrapper @0x8018EF60
  (`sw a1,4(sp); sw a2,8(sp); sw a3,12(sp)` — vararg spill — then
  fallthrough into EF6C; generated br2eu_func_8018EF60, 12 bytes).
- Every `CPS jal ->` in generated targets **0x8018EF60**, never EF6C.
  0x8018EF60 is clean-ported as `sprintf_core_guest`
  (`game/game.c:2319`) which marshals a2-a3 + caller stack words into
  `sprintf_core` (`game/battle.c:8615`, complete clean port of the
  engine: %d/%u/%x/%s/%c/%%/%n, width/precision/flags).
- EF6C itself is an internal continuation: it expects EF60's spill
  layout at sp+4/8/12, so it is NOT independently callable with a fresh
  standard varargs ABI — generated fallback is the faithful choice for
  any CPS resume that ever lands there (impossible in practice: the
  clean sprintf_core never yields).
- fnmap.c:2961 maps 0x8018ef6c → `sprintf_core_engine`
  (gfx_emit_p07.c:95), a `br2_call_generated` self-delegate that
  gen_clean_table.py skips → dispatch already fell to generated once
  the cpst_ row is gone. All 113 addrs have generated dispatch coverage
  (incl. alias entries 0x8018F220/F22C/... via
  `br2eu_psx_alias_body_8018EF6C`).

## Changes

- `game/clean_cps.map`: removed 217 rows (104 `cpst_8018e590` +
  113 `cpst_8018ef6c`). 11594 rows remain.
- `src/clean_table.c`: regenerated via `python3 game/gen_clean_table.py`
  → 5837 wrappers, 19 skipped (all self-delegate). Diff is deletion-only
  for the retired addrs.
- `game/cd_irq_probe.c`: `cpst_8018e590`/`cpst_8018ef6c` bodies left in
  place as dead code (batch-44/rotmat precedent — they are
  generator-owned transliterations; removal optional later).

## Runtime verification

Build: `cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo` +
`cmake --build build -j8` — clean.

Headless: `PSX_FN_FILTER="0x80010000:0x80200000"
./build/BloodyRoar2_Recompiled --disc "disc/...cue" --debug-port 19861
--headless --game game_1pad.toml` + `python3 tools/arcade_fight_mash.py
19861`:

- Boot healthy: CD `seq` 3135→384281 climbing, `reading:1`, frames
  advancing from frame 105.
- `*** FIGHT REACHED frame=1954 ***` then sustained combat: gs=8↔9
  phase cycling observed at t=42/110/173/253/328/428s, frames to 25705+.
- `dispatch_stats`: `static_hits=5,523,826 miss_total=0 miss_unique=0`.
- Zero wedge, zero crash across ~430s of versus/arcade mash incl. CD
  streaming (asset loads exercise printf via debug/text paths).

## Leftovers / notes

- The `cpst_*` bodies remain compiled-but-unreferenced in
  `game/cd_irq_probe.c` (dead code, same as prior retire batches).
- Sibling printf-family rows intentionally KEPT: the 9 `printf_cps_*`
  rows (gfx_emit_p05.c full CPS transliteration, hand-verified) and the
  `printf_flags_*`/`printf_core_piece_*`/`gpu_pkt_cmd_*` fnmap piece
  entries — those are deliberate per-piece clean ports, not cpst_
  shadows.
- If a future audit wants printf piece coverage in clean C *with*
  per-instruction timing, `printf_cps_body` (gfx_emit_p05.c) could be
  extended with psx_cyc_step charging and the piece addrs re-pointed to
  it — but with zero observed dispatches to those addrs, generated
  fallback is the faithful and simpler choice.
