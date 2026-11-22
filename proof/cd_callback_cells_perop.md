# CD wedge — fntrace evidence: callback cells are per-op, SM driver is the gap

Date: live capture on debug port **8973**, fresh instance, `fntrace_arm`
on 10 suspects before navigation. Wedge reproduced deterministically
(`seq` frozen at 81861, `last_sector` lba 8801, `gs=8 ph=03 b4=0300`,
producer `0x801fc6a4`=2 / finalize `0x801fc6a8`=0, all four callback
cells `0x801bc848/4c/a8/ac` = 0 — identical to port-8999 capture).

## fntrace result (1094 entries, armed from boot)

Only **4** non-per-sector entries in the entire run:

| seq | target | ra | frame | note |
|---|---|---|---|---|
| 1051 | `init_done_cb` 0x80198590 | 0x80197A9C | 3825 | restores bc848 from bcba8, bcba4=0 |
| 1052 | `read_setup` 0x80198C90 | 0x8018D198 | 3826 | parks bc848→bcba8, bc84c→bcbac; zeroes live cells |
| 1055 | `read_open` 0x80198908 | 0x80198E08 | 3826 | discards parked, installs bc84c=`read_step_cb` |
| ~1056-1090 | `read_step_cb` 0x801985C4 ×~35 | 0x80197A6C | 3753-3837 | per-IRQ sector callback, healthy |
| 1093 | `init_done_cb` 0x80198590 | 0x80197A9C | 3839 | **LAST ENTRY EVER** — Pause INT2 handler |

Inside the final `read_step_cb` tail (seqs ~1091-1092):
`mc_session_b(*(bcbac))` restores bc84c, `mc_session(0x80198590)` installs
`init_done_cb` into bc848, `Pause(9)` issued. `init_done_cb` then restores
bc848=bcba8 and clears bcba4.

## Conclusions

1. **`cd_chan_reset_b` (0x8019746C), `cd_subsystem_init` (0x801974B8),
   `cd_init_mode_dispatch` (0x80195C1C), `cd_probe_retry` (0x80195AA4):
   ZERO hits** — the reset/loss hypothesis is RULED OUT. Nothing cleared
   the cells; they are legitimately 0 between read sessions by design
   (park/restore protocol).

2. The cells being 0 at the wedge is **normal idle state**, not corruption.
   The real gap: after `init_done_cb` (seq 1093), **no new
   `cd_read_setup`/`cd_read_open` ever ran** — the next file-load session
   was never initiated.

3. `cd_mc_state_machine` (0x80199440) is driven per-frame by `hp_bar_tick`
   (`game/menu.c:11551-11555`) and the tile-stream path
   (`game/menu.c:2277-2279`), both gated on
   `0x801f4738 != 0 && 0x801fc68c != 0`; the flag is cleared after each
   run. At the wedge `0x801fc68c = 0` → the SM never runs → producer
   starves → finalize never happens → no ch4 → no `DeliverEvent` →
   `TestEvent` spin. **The open question is who is supposed to SET
   `0x801fc68c`** — that setter never fired (or fired and was cleared
   without the SM completing the entries).

4. Consistent with `proof/cd_finalize_gate.md`: the `0x80010000` arena
   overwrite is real but secondary; the producer has no driver because
   the SM is never invoked.

## Next probe

Find every writer of `0x801fc68c` (nonzero) — likely inside generated
`cd_dir_entry_store`/channel-entry producer code or a deferred command —
and determine why it didn't run after seq 1093. Runtime instance 8973
left wedged for follow-up probes.
