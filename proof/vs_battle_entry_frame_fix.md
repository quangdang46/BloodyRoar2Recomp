# Versus battle-entry wedge — root cause: unreserved guest stack frames

## Symptom

Versus (menu item 2, `battle+0x9b=2`, `+0x9c=1`) wedged before the fight:
`battle+0xb4=0x0300` (P1 side) but `battle+0xb6=0x0000` (P2 side) forever,
`battle_frame` stuck in state 3 (`fighter_spawn_setup`/`battle_config_eval`
loop). Pad-2 Start edges reached the P2 input field (`+0x6a=0x0800`) but
never armed the phase — looked like input starvation or a missing writer.

## Root cause (two instances of the same bug class)

Clean-C ports that model a guest stack frame with
`guest_at(guest_sp() - N)` **without** calling `br2_guest_frame()` leave the
live guest `$sp` unchanged. Every nested guest/clean call then allocates its
own frame in that same window and overwrites the "locals". Two functions in
the versus path had this bug:

1. `battle_frame` (`game/menu.c`, guest `0x8014cdd0`, frame `0x180`):
   the per-side argblock pointers at `sp+0x130`/`sp+0x134` were clobbered by
   callee frames → `fighter_spawn_setup`/`battle_config_eval` ran on wrong
   records → P2 phase `+0xb6` never initialized.
   Fix: `br2_guest_frame(0x180)` at entry, `br2_guest_frame_free()` at the
   single exit; frame base from the returned guest sp.

2. `select_screen_tick` (`game/menu.c`, guest `0x8014f260`, prologue
   `addiu sp,-0xf8`, `s7=sp+0x40`, `sp+0x18 -> cell 0x801f10bc`): the two
   0x30-byte per-player select records (`sp+0x40`) and the `sp+0x18` param
   block were clobbered every frame → the `+0x10`/`+0x24` confirm-state
   fields never reached the exit condition (`local_94[0]==2 &&
   local_94[0xc]==2`) → versus select wedged at `DAT_1f800001==6`.
   Fix: `br2_guest_frame(0xf8)` (aligns to `0x100`) at entry; locals keep
   their guest absolute addresses via `sst_frm+0x20/+0x48/+0x58/+0x6c`;
   `br2_guest_frame_free()` before the single `return`.

The `+0xb6` writer was never absent: `battle_config_eval`'s class-0 path →
`bcfg_step_a` (Start-edge `0x800` + `battle_state_gate` + `ret_zero_b`)
stores `0x700`, and the `0x300` phase is armed by the select/battle setup
once the argblocks are correct. Pad slot 1 (controller 2) input routing was
verified working (`+0x6a` edge `0x0800` observed); versus P2 is a human
pad-2 side in mode `(2,1)`, not CPU-only.

## Runtime proof (headless, `--debug-port 8900`, clean build)

Menu: title START → DOWN → START (selects versus, `sig=(2,1)`) → CROSS on
both pads during select.

```
gs=5 ph=2 sig=(2,1)                      <- versus menu item chosen
gs=8 ph=0 b4=0100 b6=0100 c92=0300       <- select screen, both joined
gs=8 ph=3 b4=0300 b6=0300                <- battle_frame state 3, BOTH phases armed
gs=8 ph=4 -> ph=8                        <- setup completes, fight begins
ph=8 c92: 0300->0301->0302->0303->0307->030b->030c->030d   <- round state machine
b4/b6:    0300/0300 -> 1400/0500 -> 1100/0100 -> 0300/0300 <- round end -> next round
```

The fight loop repeated continuously for 90+ s of polling (dozens of round
transitions, `ph` cycling 8/6/1/0/3/4/8), `fn_stats direct_seen` > 17M
dispatches — a live versus battle, not a wedge. States polled via
`read_ram 0x1f800000` (`gs`=byte0, `ph`=byte1, `9b/9c`=mode bytes,
`b4/b6`=phase words @0xb4/0xb6, `c92`=confirm word @0x92).

## Files changed

- `game/menu.c` — `battle_frame` + `select_screen_tick` guest-frame
  reservation (the only functional changes; diagnostic latches reverted).
- `tools/vs_phase_watch.py` — debug-server watcher used during diagnosis.
