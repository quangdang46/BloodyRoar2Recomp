# Phase-8 runtime probe — battle phase words / player-ready flags at DAT1=8

Date: this session. Two live headless instances:

- **Port 9910** — pre-existing wedged instance (`game_1pad.toml`,
  `p2_device="none"`). NOTE: a leftover `tools/arcade_fight_mash.py 9910`
  (PID 46812, started 08:44) is STILL RUNNING and mashing slot0+slot1
  attack buttons `[0xEFFF,0x7FFF,0xDFFF,0xBFFF]` every ~0.5 s. Its button
  set never includes bit `0x0800`, which turned out to be the edge bit
  `bcfg_step_a` requires — explaining why +0xb6 stayed 0 there for hours.
- **Port 9931** — fresh instance launched this session, driven by
  `tools/nav_arcade.py 9931` to the same wedge, with `gwatch` armed on the
  scratchpad phase/edge words **before** the battle phase began.

Disc: `/Users/tranquangdang21/Projects/bloody-roar-2/Game.cue`
(`Game.cue` does NOT exist at repo root — use the bloody-roar-2 path or
`disc/…(Europe).cue`).

## Commands used

```
./build/BloodyRoar2_Recompiled --game game_1pad.toml \
    --disc /Users/tranquangdang21/Projects/bloody-roar-2/Game.cue \
    --debug-port 9931 --headless
python3 tools/nav_arcade.py 9931
# debug JSON-lines on 127.0.0.1:9931 / :9910:
gwatch {"addr":"0x1f8000b4","len":"8"}   # +0xb4/+0xb6 phase words
gwatch {"addr":"0x1f800092","len":"8"}   # global phase/state u16
gwatch {"addr":"0x1f800068","len":"8"}   # pad edge u16s (+0x68/+0x6a)
gwatch {"addr":"0x1f80008c","len":"8"}   # +0x8c progress word
gwatch {"addr":"0x1f80006a","len":"2"}
gwatch_dump {"count":N,"newest":0|1} / gwatch_reset / gwatch_disarm
mem_words {"addr":"0x1f800060","count":32}
read_ram {"addr":"0x801befa8"|"0x801befca","len":16}   # pad1/pad2 reports
press {"slot":S,"buttons":0xXXXX,"frames":N}
freeze_check
```

`gwatch` **works on scratchpad** `0x1F800xxx` (mprotect covers the
scratchpad arena; `guest_watch.c` maps phys `0x1F800000+` via
`memory_get_scratchpad_ptr`). Early concern about 'oor' was unfounded —
`oor` counts out-of-range accesses on the guarded page (the scratchpad
page is extremely hot, millions of hits); in-range hits are recorded
with `in_range:1`. Caveat: `store_pc` = `g_debug_last_store_pc`, which is
only updated by *generated* stores — for clean-C `guest_at` writers it is
stale (always showed `0x8019A48C` = `sw v0,0x10(sp)` in `cd_timer_read`,
a stack store). Writer identity must be inferred from `func`/`ra`/timing,
not `store_pc`.

## Timeline — fresh instance 9931

1. Armed 4 gwatch ranges at boot (before any battle state).
2. `nav_arcade.py` drove title → mode menu → `gs=8`; DAT1 (`byte@0x1f800001`)
   walked 0→3→8. Wedge reached with:
   `+0xb4=0x0300, +0xb6=0x0000, +0x92=0x0100, +0x8c=0xffff, +0x9b=0→2,
   +0x68/+0x6a edges 0, pad reports 0x801befa8/0x801befca = "0041ffff" (neutral)`.
3. **Pre-injection gwatch evidence**: the ring (32 K entries, ~120 K
   dropped — flooded by the per-pad-poll `+0x68` writer) contained
   **80+ writes to `0x1F8000B4` at one-per-frame** (frames ~8245–8305+)
   with `old==new` (value-rewrite, no transition), plus 2 writes to
   `0x1F800092` and 1 to `0x1F800098`. After the wedge settled, a
   narrowed watch (+0x68 removed) recorded **ZERO** writes to
   +0xb4/+0x92/+0x6a/+0x8c for 90+ s (~thousands of `battle_frame`
   iterations; `bce_calls` grew 812 K→1.57 M).
4. **Pad-2 injection (the decisive experiment)** — slot-1 presses:
   - `press {"slot":1,"buttons":0xf7ff,"frames":600}` (~bit `0x0800`
     active-low) → `+0x6a` edge word showed `0x0800`, `+0x6e` held `0x0800`.
   - Sequence `0xfff7, 0xf7ff, 0xbfff, 0xefff, 0xdfff, 0xffff` (45 f each)
     → **`+0xb6` advanced `0x0000 → 0x0300`**. `+0xb4` stayed `0x0300`.
5. **gwatch caught the +0xb6 writer red-handed** — exactly 3 entries, all
   in frame 8929 (~260 guest cycles apart), chronological:

   | seq | addr | old u16@+0xb6 | new u16@+0xb6 |
   |-----|-----|---------------|---------------|
   | 0 | 0x1F8000B6 | 0x0000 | **0x0700** |
   | 1 | 0x1F8000B6 | 0x0700 | 0x0100 |
   | 2 | 0x1F8000B6 | 0x0100 | **0x0300** |

   (`old`/`new` are 16-byte windows anchored at the fault addr; byte1 is
   the +0xb7 high byte of the u16.) All three are clean-C writes
   (`store_pc` stale `0x8019A48C`), `func=0x000029CC`, `sp=0x801FFE40`,
   `ra` host chain present. This matches `bcfg_step_a`'s `L_set700`
   (0x000→0x700 on edge `& 0x800`), then `bcfg_step_c` reset (0x700→0x100),
   then a re-seed to 0x300 — i.e. **the CPU-side phase word IS writable
   and the pad-2 edge path works end-to-end when the right button edge
   (bit 0x0800) is supplied.**
6. **Post-confirm stall**: with `+0xb4=+0xb6=0x0300`, a further 75 s of
   alternating slot0+slot1 presses (`0xf7ff/0xfff7/0xbfff/0xefff/0xffff`,
   ~190 press pairs) produced **no further writes** to any watched word
   (gwatch ring still holds only the 3 entries above), `+0x92` stayed
   `0x0100`, `+0x8c` stayed `0xffff`, DAT1 stayed 8, `pbd_calls=0`,
   `bce_calls` kept growing (~1.9 M total). Frames advanced normally
   (~2.5 fps headless). **The residual stall is NOT input-gated.**

## Port 9910 cross-check

- Same wedge (`+0xb4=0x0300, +0xb6` eventually `0x0300`, `+0x92=0x0100`,
  DAT1=8). Its pad records at `0x801befa8/0x801befca` cycle through
  `0xffef/0xff7f/0xffbf/0xffdf` — the mash script's attack buttons —
  and `+0x68/+0x6a` edges flicker accordingly, yet +0xb6 stayed 0 for
  hours because **none of the mashed buttons asserts bit 0x0800**.
- Injecting `press {"slot":1,"buttons":0xf7ff,"frames":60}` +
  `0xbfff` left `+0xb6=0x0300` (already advanced by then — consistent:
  the mechanism is the same, timing differs).
- gwatch on 9910 (armed mid-wedge): 6+ min, **zero** in-range writes to
  +0xb4/+0x92/+0x6a/+0x8c while `bce_calls` grew ~1.5 M — same "writer
  exists but isn't firing" signature as 9931 post-settle.

## freeze_check / counters (9931 at end)

`frame_count≈9346` (advancing), `bce_calls≈1.89 M` (growing ~linearly),
`bce_fz0=132`, `bce_cls=0x000`, `pbd_calls=0`, `tpool_fail_count=1`,
`warpe_calls=3`. DAT_1f800001 = **8** throughout (byte 1 of
`mem_words 0x1f800000` = `0x00000808`).

## Verdict

**(b) for the +0xb6 seed, then (c) for the residual stall — both
confirmed at runtime:**

- The per-player phase words DO have live writers. `+0xb6` sat at
  `0x0000` purely because **no slot-1 (CPU/pad-2) button edge with bit
  `0x0800` ever arrived** — `p2_device="none"` means the CPU side's
  confirm must come through the pad-2 decode channel
  (`0x801befca` → `pad_state_refresh` → `battle+0x6a` edge →
  `bcfg_step_a`). This matches `proof/arcade_cpu_confirm.md` and
  `proof/cpu_phase_wedge_disconnected_pad.md`; there is no CPU/AI pad
  synthesizer (re-confirmed: no non-SIO writer of `+0x6a` exists).
- Once `+0xb6` reached `0x0300` via injection, **both** phase words are
  `0x0300` and the game still sits at DAT1=8 indefinitely. No guest
  writer fires for +0xb4/+0xb6/+0x92/+0x8c anymore (gwatch-clean for
  minutes), `post_battle_dispatch` never runs (`pbd_calls=0`), and
  ~190 both-slot press pairs change nothing. Phase class `0x300` is
  announce-terminal in `battle_config_eval` — the advance out of it
  belongs to the downstream spawn/task/asset-ready path
  (`fighter_spawn_setup` commit needs `rec+0x10==1 && phase==0x700`;
  the transient 0x700 self-reset to 0x100→0x300 within one frame),
  i.e. **a faithful stall awaiting a spawn/event condition that never
  arrives** — consistent with `proof/cpu_input_source_arcade.md`'s
  "residual wedge" and `proof/downstream_spawn_wedge.md`.

## Cleanup

- Port 9931 instance killed after probing (this session's instance).
- Port 9910 instance and the `arcade_fight_mash.py 9910` process were
  left running — they predate this session and were not ours to kill.
  gwatch ranges remain armed on 9910 (4 ranges: +0xb4/+0x92/+0x6a/+0x8c).
