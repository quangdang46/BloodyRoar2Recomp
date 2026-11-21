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
