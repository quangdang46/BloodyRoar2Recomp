# CD dir-entry `+4` progress — completion-gate anatomy (agent cbb95a10 + live confirm)

## The finalize gate (read-only compare, NOT the writer)

`cd_mc_state_machine` generated body, `br2eu_func_80199440`
(`generated/SLES_017.22_full_46.c:17049-17115`, mirror
`psxrecomp/docs/internal/cdmc_80199440.asm:479-490`):

```
0x80199BBC  lw   $v0, -0x14e8($v0)   ; v0 = *(0x801FEB18) cached ACTIVE entry ptr
0x80199BC4  lhu  $v1, 6($v0)         ; expected  = entry+6 (u16)
0x80199BC8  lhu  $v0, 4($v0)         ; received = entry+4 (u16)
0x80199BCC  addiu $v1,$v1,-1         ; expected-1
0x80199BD0  bne  $v1,$v0, skip       ; finalize only when received == expected-1
0x80199BD8..                           ; on equal: 0x801FC69C=1, issue next cmd
```

`cd_channel_poll` (`game/cd.c:1658`) only *consumes*: waits `u16 entry+0==2`,
writes `entry+0=4`, never touches `+4`.
`cd_dir_entry_finalize`/`cd_dir_entry_store` write `entry+0=2`, not progress.

## The ONLY CPU store that can reach dir-entry `+4`

`gwatch` on `0x80010000..0x800103FF` caught exactly one non-DMA store to a
`+4`-aligned field:

```
guest PC 0x80199ADC : sw $v0,($a1)   ; gen SLES_017.22_full_46.c:16383
   = entry0+4/+6 word, written by the bulk 32-byte entry COPY
0x80199AB0  sh state=1 -> *0x801FEB18        ; claim working entry
0x80199AB4  a1 = *0x801FC6C0  (entry0 base)  ; v1 = *0x801FEB18 (src)
0x80199ACC  *0x801FC6A4 = 0                  ; reset produce cursor
0x80199AD0  8-word copy loop  src->entry0    ; iter2 hits entry0+4
0x80199AE8  *0x801FEB18 = *0x801FC6C0        ; rebase cached entry to entry0
```

This is a **bulk entry copy, not a per-sector increment** — it seeds `+4/+6`
from the source entry. `cd_mc_state_machine` contains **no** `lhu/addiu/sh`
RMW targeting `entry+4`; no dedicated `+4++` exists in it, `cd_channel_poll`,
`cd_dir_entry_finalize/store`, `cd_read_finish_cb`, or the sub-channel cbs.

Sub-channel cbs `0x801AA244`/`0x801AA2AC` write **channel-entry** fields
`+68/+70/+72` (`|=1` ready flags) — channel bookkeeping, not dir `+4`.

## Live confirmation (port 6100)

```
0x801FC6C0 = 0x80010000   dir/entry table base
0x801FC6A4 = 2            produce/submit cursor
0x801FC6A8 = 0            finalize cursor   (never advanced)
0x801FC6AC = 30           consume/poll cursor
0x801FEB18 = 0x80010020   cached ACTIVE entry = index1
```

Active entry `0x80010020` fields (live):
`u16@+0 = 2` (state ready) but `u16@+4 = 1` (received), `u16@+6 = 0` (expected).
Finalize needs `received == expected-1` → `1 == -1` never true → the entry can
never re-finalize; `+6` was never seeded with a real count.

`cd_mc_state_machine` DOES run (pc_probe=136), `cd_result_wait_b`=395,
`cd_read_step/finish_cb` are generated-faithful; CD→RAM DMA streams
(lba 305→9541 across the wedge). MDEC output_pos frozen at 10829568/12405504.

## Conclusion

The wedge is **upstream of the completion predicate**: the channel/sector-ready
consumer that should (a) seed `entry+6` expected-count and (b) credit `entry+4`
per completed sector **never supplies the values**. The only `+4` write is the
entry-claim bulk copy; no per-sector progress RMW exists in any clean or
generated path examined. Next: find the consumer that reads channel-entry
ready flags `+68/+70/+72` and bridges them into dir-entry `+4/+6` — likely a
computed-pointer store in generated code (invisible to literal-offset grep).

## Bisect result (2026-11-21): NOT a regression — wedge predates 57aef5b

Built a clean-source worktree @57aef5b (the commit runtime-verified
"versus reaches fight and sustains combat") and ran it headless. It shows
the IDENTICAL wedge as HEAD:

```
0x801FC6A4=2 (produce)  0x801FC6A8=0 (finalize)  0x801FC6AC=30 (consume)
MDEC output_pos frozen ~10.8M/12.4M, decode_macroblocks ~17118, busy=0
```

=> The attract-FMV dir-entry +4/+6 gap is a LONG-STANDING unresolved RE
issue, not introduced by the post-57aef5b cd.c/save.c/menu.c/gfx.c commits
(all of which were verified faithful fixes — e.g. cd_cmd_rec_scan ternary
confirmed against generated 0x801A3AB4-0x801A3ACC: rec[52]<7->t1=rec[52],
else t1=6, matching HEAD).

=> versus combat is proven reachable (57aef5b) via a path that does NOT
require this attract-FMV dir-entry completion. blk_io_retry returns -1
after 2000 failed cd_channel_poll tries, so the game fails gracefully past
the FMV rather than hard-hanging — the attract FMV is a visual/resource
stall, not a hard gameplay deadlock.

NEXT: verify versus/arcade reach combat on HEAD (the attract FMV wedge is
a separate, narrower issue — the +4/+6 credit consumer at 0x801AA4FC
family per agent 62e13f70).

## Refinement (2026-11-21, agent 18c5088f + live gwatch)

- `0x801AA4FC` is a **false lead** — `game/fnmap.c` maps it to
  `tween_seq_step`, an animation task registered via `task_node_alloc`
  (`game/battle.c:11789`). It reads `+68/+70/+72` on a *different* struct.
  Not the CD consumer. No edit made.
- `gwatch` armed on active entry `0x80010020` (`*0x801FEB18`) captured
  **zero CPU writes** over ~7s → the cached active entry is **stale**, not
  the live write target. The claim/copy path `0x80199AB0`/`0x80199ADC`
  (which would rebase `*0x801FEB18→entry0` and seed `+4/+6`) is **not
  running** — pc_probe 0 hits for `0x80199AB0`, `0x80199ADC`, `0x80199BC4`,
  `cd_dir_entry_finalize 0x801990F0`, `cd_read_finish_cb 0x80198838`.
- `cd_read_step_cb 0x801985C4` fires ~329×/window — sectors step
  continuously but never reach finish/claim. `cd_mc_state_machine` runs
  (pc_probe 136 in an earlier window, 0 in a later one — probe-window and
  `br2_call_generated` visibility dependent) but is **gated before the
  claim path** — likely on a pending-response / command-status /
  `0x801FC6B0` mode flag, not on entry `+4/+6`.
- Menu IS reachable: injected START → `scene_byte` (`DAT_1f800001`)=2,
  `g_game_state`=5 (`0x1f800000` byte0). `attract_menu_dispatch`
  (`menu.c:2715`) `param_2`=1..6 are **button-derived action codes**
  (via `menu_stepper_button_wait`/`fn_lookup`), not cursor positions;
  confirm → `mc_pad_button_handler` → `g_game_state=8` (combat). Versus
  nav is a multi-step stepper sequence — versus combat already proven
  @`57aef5b`, HEAD = 57aef5b + verified fixes only.

**Revised wedge model:** `cd_mc_state_machine` is gated *upstream* of the
entry-claim path. The `+4/+6` fields are seeded by the claim bulk-copy
`0x80199ADC` from a source descriptor — the missing link is whatever
should trigger the claim (a channel/sector-ready bridge or a
read-descriptor builder), NOT a per-sector `+4++`.

## CRITICAL refinement (2026-11-21): the `+4/+6` mechanism WORKS — wedge is FMV-pattern-specific

After the attract exited to menu (scene `DAT_1f800001`=2, `g_game_state`=5),
the SAME dir-entry pipeline is **healthy**:

```
produce=finalize=consume=0xA   (all converged — 10 entries fully processed)
active_entry *0x801FEB18 = 0x80010120  (index 9)
MDEC: output_pos=230400/246528, decode_macroblocks=321  (small menu stream,
      NOT the frozen 10.8M/12.4M attract FMV)
CD: lba=9246 streaming normally
```

=> `cd_dir_entry_finalize`/`cd_channel_poll`/`+4/+6` credit **function
correctly for normal whole-file reads** (menu resources complete,
finalize cursor advances). The wedge is **specific to the attract-FMV
streaming read** — likely `cd_file_load_sm`/`stream_block_decode`
incremental consumption where the credit path differs, OR the FMV read
was issued with `+6`/`+4` params the consumer never satisfies. The
attract→menu transition (button/timeout) abandons the wedged FMV read and
starts a fresh pipeline — which is why the game still reaches menu/combat
despite the FMV stall.

NEXT: compare the attract-FMV read's entry params (`+4/+6/+8`) vs a
working menu read's — find which field/pattern the FMV sets that the
credit path can't satisfy.

## FMV read path identified (2026-11-21)

`intro_movie_state` (`game.c:1817`) → `cd_file_load_sm` (`game.c:1703`,
thin wrapper → generated `0x8018cfdc`) → calls `cd_read_issue`/`cd_read_setup`/
`cd_read_poll`/`cd_init_session`/`cd_mode_flag_set` — **all delegated to
generated** (IRQ/deferred-coroutine boundary, `cd.c:162-303`). This is a
DIFFERENT read mechanism than the menu's `cd_channel_poll`/`blk_io_retry`/
`cd_dir_entry_probe` path.

`cd_read_setup` (`cd.c:290`, comment 293-300) registers the INT2/step
callbacks via `mc_session(_b)` and its own doc names the wedge symptom:
*"the consumer stops mid-sector and the completion event never fires."*

Live probe correlation:
- `cd_read_step_cb` `0x801985C4` = **329 hits** (sectors stepping)
- `cd_read_finish_cb` `0x80198838` = **0 hits** (read never finishes)
- => the per-sector step callback runs but the read-completion callback
  never fires → `+4/+6` never credited → dir-entry never finalizes.

The missing link is whatever should trigger `cd_read_finish_cb` — the
sector-data consumer that drains the read buffer to completion. For menu
reads an equivalent consumer runs (entries reach 10/10/10); for the FMV's
`cd_read_*` sequence it does not. Likely the MDEC-feed/channel-drain that
copies sector data out of the read buffer and marks the final sector done.
Circular-suspect: MDEC frozen may be cause OR symptom — needs the
finish-callback trigger identified, not assumed.

## cd_read_poll mechanism mapped (static, gen SLES_017.22_full_46.c:7029)

`cd_read_poll(0x80198e2c)` is a **timeout-guarded one-shot status poll**,
NOT a data pump:
- ctx base `gpr[17]=0x801BCB9C` (read-start timestamp `DAT_801bcb9c`),
  `gpr[18]=0x801BCBA4` (session flag).
- calls `cd_timer_read(-1)` (`0x8019A460`) for current frame count.
- deadline math: `*(0x801BCB9C) + 0x4B0` vs now → timeout path.
- Returns busy/done/timeout; the CALLER (`cd_file_load_sm` read loop)
  re-polls. Actual data movement is IRQ/DMA-callback-driven
  (INT2/step/finish), not inside the poll.

=> The wedge is that the read never reaches "done": the INT2/step
callbacks fire (`cd_read_step_cb` 329×) but the completion callback
`cd_read_finish_cb` (read-ctx `0x801CCB80`: `ctx+8+=ctx+0x10*4`,
`ctx+0x14--` remaining, `ctx+0x20++` done) never fires → `cd_read_poll`
returns busy until the `0x4B0`-frame timeout → read aborts/retries.

`cd_read_finish_cb` registration is conditional: `cd_read_open` calls
`cd_cmd_dispatch3(0x80198838)` ONLY if `(DAT_801bc7b4 & 1)!=0`
(`cd.c:521`); `DAT_801bc7b4=0` live (set to 0 by handler-triple-install
`cd.c:1049`). Either finish_cb legitimately isn't used (polling covers
completion) OR the flag should be 1 for this read type — the agents are
determining which. The REAL completion trigger to find: what marks the
read's last sector consumed (the channel-drain / MDEC-feed handoff).

## VERDICT (2026-11-21, agent 224c8e29): versus/arcade COMBAT reachable on HEAD

Runtime-verified on a dedicated instance (port 6400). `g_game_state=8`,
`scene=3`, phase pointer `0x1f800004` cycling `0x801E40A0<->0x801E61F0` —
the exact "gs=8 ph cycling" signature proven at 57aef5b. Sustained ~4min,
frames advancing ~50fps, wc/mmio/mc counters live. The attract-FMV wedge
is bypassed by a single correct START tap; menu needs no directional input
(repeated START confirms through to battle).

INPUT ENCODING (corrected): `press`/`set_input` `buttons` is the RAW
active-low pad word written to `sio_set_pad_state_slot` — pressed = bit
CLEARED. START-only = `0xFFFF & ~0x0008 = 0xFFF7`; CROSS-only =
`0xFFFF & ~0x4000 = 0xBFFF`. (Active-high masks press everything EXCEPT
the named button.)

=> The core game (menu -> mode select -> versus/arcade combat) WORKS on
HEAD. The attract-FMV `cd_read_finish_cb` gap is a fidelity issue (intro
video doesn't complete), NOT a gameplay blocker.

## cd_mc_state_machine invocation map (agent 18c5088f, live :6100)

`cd_mc_state_machine` (0x80199440) is invoked via exactly 3 gated paths —
all closed when no read session is active (current menu/idle state, which
is why pc_probe=0 now; earlier 136 hits were during active reads):

1. `tile_stream_pump`/`hp_bar_tick` — require `fc68c!=0`, but `fc68c` is
   set ONLY inside the machine itself (`0x8019949C`) → chicken-and-egg.
2. `cd_mc_sm_wrap` (`0x80198FE4`) — unconditional, but only called from
   `cd_response_pump` when `cd_response_process()` returns `s0&4` AND
   `*(u32*)0x801BC84C != 0`. `bc84c`=0 now (no read session).

`bc84c` (`0x801BC84C`) is the per-sector response callback slot — should
be `0x801985c4` (cd_read_step_cb) during a read, installed by the
read-open path. Entry predicates: `fc69c==1`→finalize tail; `fc688==0`→
idle; **G3**: `*(0x1F801098)`(DMA ch1 CHCR) bit24 must be set else idle.

Exhaustive store audit: NO `+4/+6` store exists in the state machine —
only entry+0 (state). The `+4/+6` are seeded by the `0x80199ADC` bulk copy
from a source descriptor. `cd_read_step_cb` writes only session fields,
never dir entries. `elem_pair_field_set`(+0x44/46/48) is the ANIMATION
element system, NOT the CD dir table (prior "+68/+70/+72" notes were these).

**Root cause chain:** no read session → nothing kicks the machine. CD sits
in Pause(0x09)-retry (`stat=0x02`). `tile_stream_open` ran (set
fc6c0/fc6c4/fc688) but the first Read was never issued/completed. NEXT:
trace `blk_io_retry`/`cd_stream_mode_init`/`cd_mode_flag_set` for where
the first Read should issue + why pending Pause never completes.

## Combat re-confirmed on persistent :6100 + timestamp correction

Second independent instance (:6100) reached combat via 3× START (`0xFFF7`
active-low): `gs=8 scene=3`, phase `0x1f800004` cycling
`0x801e40a0<->0x801e61f0`, frames advancing ~50fps, sustained. `config
+0x10`=8 / `9b`=0 / `9c`=0 → menu option param_2==1 (first/default mode).

CORRECTION: `0x1f80001c` is a **u16 vblank timestamp** (`game.c:91,257`),
NOT a round timer — the "frozen 0x3a=58" seen earlier is a sampled
timestamp, not a stall indicator. The 2-value phase cycle is the normal
battle update loop, not a stuck intro. Combat is real simulation.

## cd_read_poll done-predicate (gen SLES_017.22_full_46.c:7029-7700)

`cd_read_poll` reads the CD read-session block at `0x801BCB80`-family:
- `0x801BCB80` = total sectors requested (`DAT_801bcb80`, loaded into s0)
- `0x801BCB94` = progress/status counter (`DAT_801bcb94`; `<0` = error,
  set to sector-count on open, `0xffffffff` on open-error)
- `0x801BCB98`/`0x801BCB9C` = start timestamps (sub-timeout `+0x3C`,
  overall `+0x4B0` frame ticks)
- `0x801BCBA4` = session-active flag

Done-condition compares progress vs total + session flag + timeouts.
`cd_read_step_cb` (firing 329×) should decrement `bcb94` per sector; if it
doesn't reach the target the poll returns busy→timeout→retry. NEXT live
check on attract instance: read `0x801BCB80/94/98/9C/A4` to see whether
`bcb94` is decrementing (sectors consumed) or frozen (consumer stalled).

## BREAKTHROUGH — fresh boot reaches attract demo combat; FMV is fidelity gap not blocker

Live capture on fresh instance :6800 (no input, straight boot):

- Boot -> intro FMV: MDEC decoded to output_pos=10,829,576 / 12,393,216
  (~87%), then decode_input_pos pinned at 3796/3840 and dma_in_words froze
  at 111,520 -> **MDEC input starvation** (encoded stream stops reaching
  MDEC input DMA; CD reads themselves complete: bcb94 prog 51->0, next
  94-sector session issued, sectors flowing lba~8537).
- intro_movie_state byte @0x1f80008c = -1 (idle) -> state machine exited.
- Game then entered ATTRACT DEMO FIGHT: phase ptr @0x1f800004 cycles
  0x801E40A0 <-> 0x801E61F0 (identical signature to versus combat),
  frames advancing ~170/s, 0x3A vblank timestamp region live.

CONCLUSION: the attract-FMV wedge is a VIDEO-FIDELITY gap (intro stream
stalls ~87%, MDEC starved for input), NOT a gameplay blocker. The game
fails gracefully past it and reaches a live CPU-vs-CPU demo fight —
i.e. core gameplay/attract/combat all work end-to-end on HEAD.

Remaining fidelity bug to fix: WHY the decompressed FMV stream stops
reaching MDEC input (mdec_pump_b / lzss_decode / stream_block_decode feed
path), so the intro video renders fully instead of stalling at ~87%.
