# MARK-event delivery failure — the real arcade battle-init wedge

Fresh traced run on port **9400** (current HEAD, `game_1pad.toml`, headless,
slot-1 confirm injected to pass the CPU-phase seed). The wedge reproduced
deterministically at `gs=8 / DAT1=0x03 / b4=b6=0x0300` and was instrumented
with `fntrace`, `bioscall_dump`, `bios_svc_trace`, `freeze_check`, EvCB RAM
reads, and the freeze-dump `ev_agg`/`bios_calls` rings.

## TL;DR

The stall is **not** CD sector delivery, not the channel-finalize SM, not the
callback cells, and not game-logic spawn starvation. It is a **BIOS event
delivery failure**: the game waits on EvCB slot 8 — a *MARK*-mode event
(`class=0xF0000009, spec=0x20`) — which `DeliverEvent(0xF0000009,0x20)` fires
thousands of times during the wedge yet never marks READY. Both the blocking
`WaitEvent` and the polling `TestEvent` on that handle spin forever, so the
file-load never completes → `battle_init` never returns → `DAT1` stays `3` →
`battle_frame_tick`/`fighter_spawn_setup` never run.

## The awaited event (EvCB slot 8)

Opened by `cd_ext_init_once` (`0x8019b308`): `OpenEvent(0xF0000009, 0x20,
0x2000, 0)` → handle `0xF1000000|8 = 0xF1000008`, stored at `0x801bde38`.

Live EvCB dump (table base `0x8E08`, stride `0x1C`; slot8 @ `0x8EE8`):

```
slot8: class=0xF0000009  status=0x2000(ENABLED)  spec=0x00000020
       mode =0x2000(MARK) func=0x00000000
```

Slots 0–7 are all `mode=0x1000` CALLBACK with func pointers (CD/SPU IRQ
handlers) — **slot8 is the ONLY MARK-mode event in the table.**

`0x801bde38` (the TestEvent/WaitEvent handle cell) = `0xF1000008` confirmed.

## Who waits on it (fntrace + bioscall ra decode)

| svc | handle | count | ra → function |
|-----|--------|-------|----------------|
| B0:0x0B TestEvent | 0xF1000008 | **162,263,022** | `0x8019D550` → `cd_seek_step` (+0x48) |
| B0:0x0A WaitEvent  | 0xF1000008 | ~36,000 | `0x8019CF48` → `cd_mode_apply_deferred` (+0x138) |
| B0:0x0B TestEvent | 0xF1000001/3/4 | ~400–470k | `cd_job_start_e` (+0x100/+0x138) — secondary |

`cd_seek_step` (`0x8019d508`) and `cd_seek_complete_poll` (`0x8019d500`) are
`br2_call_generated` delegations whose generated bodies spin in
`TestEvent(*(0x801bde38))`. `cd_mode_apply_deferred` blocks in `WaitEvent`.

## The producer that should satisfy it

`DeliverEvent(0xF0000009,0x20)` is emitted by `cd_dma_chan_reset`
(`0x8019b7e8` → `cd_bios_svc_07` → `bios_svc(0xb0,7,…)`) on CD channel-4 DMA
completion. `bios_calls` tail shows it firing **interleaved with the wedge**:

```
fn=0x07 a0=0xF0000009 a1=0x20 ra=0x8019B88C  rep=32741   <- DeliverEvent
fn=0x0B a0=0xF1000008 ra=0x8019D550          rep=112712424 <- TestEvent spin
fn=0x07 a0=0xF0000009 a1=0x20 ra=0x8019B88C  rep=1715
fn=0x0B a0=0xF1000008 ra=0x8019D550          rep=49546374
fn=0x07 a0=0xF0000009 a1=0x20 ra=0x8019B88C  rep=753
```

Yet slot8 status sampled 60× over ~3 s is **always `0x2000` (ENABLED), never
`0x4000` (READY)** — despite class+spec+ENABLED+MARK all matching.

## Backend: LLE OpenBIOS (call-HLE OFF)

`bios_info`: image `OPENBIOS`, `deliver_event_ret=0x0`. Per
`bios_hle.c:338`, `s_call_hle_on = (call_hle && deliver_event_ret!=0)` →
**call-HLE is disabled**, so every B0 event call runs in the **recompiled
OpenBIOS kernel** (LLE). The `ev_agg`/`bioscall` rings record both routes.

Kernel spin addresses (`dispatch_tail`): `0x3E14↔0x3CB4`, `0x3E48↔0x3CD8`,
`0x2B20`, `0x3D10/0x3D1C/0x3E90/0x3A4C/0x3AE8/0x3B1C` — OpenBIOS
thread/event-wait internals (`OpenBIOS_func_00003DE8_cont_BFC21DE8`,
`OpenBIOS_func_00003C70_cont_BFC21C88`, `OpenBIOS_func_00002B20`).

## Interpretation

`DeliverEvent(class,spec)` must iterate ENABLED EvCBs and, for MARK mode,
set `status=READY`. It fires with matching class+spec while slot8 is ENABLED
and MARK — yet READY never appears. CALLBACK-mode siblings (slots 0–7) are
believed to still fire (CD IRQ path is alive: `cd_intc_dispatch` ticks every
frame, `svc 0x17` polls continue). The failure is therefore specific to the
**MARK/flag delivery branch** in the LLE OpenBIOS `DeliverEvent` (or to the
OpenBIOS event-state machine that consumes it), not to CD, DMA, or IRQ
delivery.

Note: the earlier `arcade_dat1_battle_init_frame.md` "CD is NOT the bug /
battle_frame spawn starvation" correction describes a *different* manifestation
of the same intermittent, timing-sensitive wedge (that run got further — CD
finished and `battle_init` returned). This run wedges earlier, inside the
file-load's `cd_seek_step` event wait. Both are consistent with missed
completion/timing in the LLE event path.

## Next step

Root-cause the OpenBIOS `DeliverEvent`/`TestEvent`/`WaitEvent` recompiled
bodies (`psxrecomp/generated/OpenBIOS_full.c`, kernel RAM `0x1xxx–0x4xxx`):
verify the MARK branch writes `status=READY` to the real EvCB slot, check the
EvCB table base OpenBIOS actually uses (may differ from the SCPH1001
`0xA0000120→0x8E08` layout assumed above), and whether `WaitEvent`'s block
releases on READY. Compare against the Beetle oracle event behavior.
