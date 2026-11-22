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

## SM driver identified: cd_mc_sm_wrap is installed as a session callback

In `generated/SLES_017.22_full_46.c` (~pc 0x80198FAC-0x80198FC4, inside
the read-session setup path):

- 0x80198FB4: `$a0 = 0x801A0000-28432 = 0x801990F0` (**cd_dir_entry_finalize**)
  → `jal 0x801961A8` (session-callback installer A)
- 0x80198FC0: `$a0 = 0x801A0000-28700 = 0x80198FE4` (**cd_mc_sm_wrap**)
  → `jal 0x80195D78` (**mc_session_b** — installs into the bc84c-family
  per-event callback cell)

So `cd_mc_state_machine` runs as the **per-CD-IRQ step callback** while a
read session is active (bc84c = cd_mc_sm_wrap), PLUS opportunistically
per-frame via hp_bar_tick/tile-pump when fc68c is set. Between sessions
(bc84c restored to parked=0) the SM is dormant **by design** — the wedge
is therefore upstream: the game never started the next read session,
because it is waiting for `DeliverEvent(0xF0000009)` that only the SM's
channel-entry finalize produces. The question reduces to: why didn't the
SM get invoked once more (via tick with fc68c set, or a new session)
after seq-1093 `init_done_cb`?

## bc84c invocation signature + fc68c lifecycle (generated 0x80197A5C-6C)

`cd_response_pump` invokes `bc84c` as:

```
$v0 = *(0x801BC84C);  if ($v0 == 0) skip;
$a0 = lbu (*$s1);                    // CD status/IRQ reason byte
$a1 = 0x801FC6D0;                    // channel-state block +0x10
jalr $v0                            // -> 0x80197A6C
```

fntrace confirms: `read_step_cb` saw `a0=0x1` (data-ready byte). When
`bc84c = cd_mc_sm_wrap`, the SM entry therefore writes
`fc68c = a0 = small nonzero` — **the flag persists after the IRQ returns
(nobody clears it in IRQ context)**, so the next `tile_stream_pump`/
`hp_bar_tick` tick sees `fc68c != 0` and runs the SM once more, then
clears it. `0x801F4738` (pump gate) verified = **1** at the wedge, so the
tick path is armed — `fc68c = 0` is the ONLY blocker.

## Refined deadlock model

1. During a read session `bc84c` cycles: `read_setup` parks old bc84c →
   `read_open` installs `read_step_cb` → its tail restores bc84c from the
   parked cell. If the parked value was `cd_mc_sm_wrap` (installed at
   session setup pc 0x80198FC0), IRQs keep driving the SM between reads.
2. Each IRQ-driven SM run leaves `fc68c = nonzero` → per-frame tick runs
   SM → SM drains channel entries → finalize → ch4 → `DeliverEvent`.
3. At the wedge: `bc84c = 0`, `bcbac = 0`, `fc68c = 0`, producer=2,
   finalize=0, `0x801FC6A0` seq=0, flag `0x801FC6B0`=0 — the whole
   park/restore chain restored zeros, so no IRQ driver AND no tick
   driver. The pending entries (producer 2 > finalize 0) never process.
4. Game spins in BIOS `B0:11` event wait (1.46M calls) on a task
   (tcb 0x8FE0) looping `0x8019A87C`/`0x8019A53C` per frame — waiting
   for the event only SM finalize produces. Circular: new session needs
   the event; the event needs the SM; the SM needs a session or fc68c.

## Remaining unknown (runtime)

Why the parked `bcbac` was 0 — i.e., whether `cd_mc_sm_wrap` was ever
installed into the live bc84c chain this session, or whether an extra
`read_setup` parked a zero. wtrace on `0x801BC84C`/`0x801BCBAC`/
`0x801FC68C` across a fresh repro gives the exact write sequence.

## Install site owner: cd_mode_set_0e (0x80198F60)

The `bc84c = cd_mc_sm_wrap` + finalize-callback installs (pcs
0x80198FAC-0x80198FC4) live inside **`cd_mode_set_0e`** — they run on a
CD mode transition to 0x0E, not per-read. So `sm_wrap` occupies bc84c
for the whole 0x0E mode window; `cd_read_setup`/`cd_read_open` park and
restore around it per file read. If a mode transition away from 0x0E (or
a reset path) clears bc84c without reinstalling, the SM loses its IRQ
driver — matching the all-zero wedge state.

## cd_mode_set_0e decoded (0x80198F60, 132B/9 blocks — port candidate)

```
s0 = a0;
cd_mode_flag_set(0x0E, &s0_lo_byte, 0);        // jal 0x80195D8C
if ((s0 & 0x0100) == 0) goto end;             // bit8 = "install SM cbs"
if ((s0 & 0x0020) == 0) *(0x801FC6F8) = 1;    // bit5 clear -> flag=1
else               *(0x801FC6F8) = 0;         // bit5 set  -> flag=0
install finalize cb: a0=0x801990F0 -> jal 0x801961A8
install sm_wrap cb:  a0=0x80198FE4 -> jal 0x80195D78 (bc84c)
end: epilogue
```

- Caller: `tile_stream_pump` path, `cd_mode_set_0e(0x1e0)` (menu.c:2375)
  → 0x1e0 has bits 0x100+0x20 → installs cbs, `0x801FC6F8 = 0`.
- `0x801FC6F8` is read by the finalize path (pc 0x80199180, gates a
  branch) and by the SM (pc ~0x80199A19 area) — a shared "work pending"
  flag for the channel-entry machinery.
- Wedge dump: `0x801FC6F0..0x801FC70C` **all zero** — flag clear,
  consistent with drained state + orphaned pending entries.
