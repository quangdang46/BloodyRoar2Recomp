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

## Static follow-up: who writes 0x801FC68C

Exhaustive grep of `generated/` for the address (all base+offset forms):

| PC | Instr | Effect |
|---|---|---|
| 0x80159880 | `lw $v0,-14708($s0=0x80200000)` | hp_bar_tick reads flag |
| 0x80159898 | `sw $zero,-14708($s0)` | hp_bar_tick clears after SM returns |
| 0x8019904C | `sw $zero,0xC68C($s0)` | cd_dir_clear path — clears |
| **0x8019949C** | **`sw $a0,0xC68C($at=0x80200000)`** | **cd_mc_state_machine entry — the ONLY nonzero writer in the binary** |

The SM store sits in the **delay slot** of the `bnez $v0` at 0x80199498
(flag = `*(0x801FC6B0)`); it executes on every SM entry that reaches
block_8019948C, writing the SM's own `$a0` argument into fc68c. When the
flag is nonzero it also increments `*(0x801FC6A0)` (a sequence counter).

SM callers: `hp_bar_tick` jal @0x80159890 (does NOT set $a0 — inherits
caller's stale a0) and `cd_mc_sm_wrap` 0x80198FE4 (pure trampoline, also
no arg setup) reached only via dispatch/jalr. Both known callers clear
fc68c=0 after SM returns, so the flag persists only if SM is invoked via
a third path (IRQ callback / task dispatch) that does not clear it —
or if $a0 happens to be 0 on the hp_bar_tick path.

Open: what $a0 is at each SM entry, and which caller leaves fc68c set.
Runtime wtrace on 0x801FC68C during a fresh repro is the decisive probe
(agent 516f0e6e).
