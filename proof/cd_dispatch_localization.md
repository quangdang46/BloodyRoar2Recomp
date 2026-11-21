# CD wedge — dispatch-level localization (pc_probe hit counts)

Session on fresh instance `:6100` (new binary, post `c11c718`).

## Method
`pc_probe_arm` hit-counter probe (counts guest-PC hits, works across
generated coroutine/IRQ paths where `fntrace` entry hooks are blind).
Armed 12 PCs for 4s at frame ~108063.

## Hit counts (the decisive data)

| guest PC | function | hits |
|---|---|---|
| 0x801AA244 | sub-channel cb A | **381** |
| 0x801AA2AC | sub-channel cb B | **762** |
| 0x8019AE68 | cd_cb8_dispatch (type-0 INT handler) | **382** |
| 0x80199440 | cd_mc_state_machine | **0** |
| 0x80199BC8 | state-machine cont (progress gate) | 0 |
| 0x80199CF0 | state-machine cont (submit) | 0 |
| 0x801990F0 | cd_dir_entry_finalize | 0 |
| 0x80198FE4 | cd_mc_sm_wrap (cb84c hook) | 0 |
| 0x801979F8 | cd_response_pump (type-2 handler) | 0 |
| 0x8019AF7C | INT handler | 0 |
| 0x8019B7E8 | cd_dma_chan_reset | 0 |

## What this proves

1. **BIOS IntRP sub-channel dispatch WORKS.** The registered sub-channel
   callbacks `0x801AA244`/`0x801AA2AC` (installed via `bios_tramp_c003`
   into `0x801FEB44/48`) fire 381/762 times. The recompiled kernel DOES
   dispatch CD sub-channel events to the game handlers.

2. **CD INT dispatch works.** `cd_cb8_dispatch` (type-0 handler,
   registered via `cd_dispatch_tbl8(0,…)` in `cd_subsystem_init`) fires
   382× — once per sub-channel pair, i.e. on each CD data-ready IRQ.

3. **The break is exactly here:** `cd_cb8_dispatch` fires, but
   `cd_response_pump` (`0x801979F8`, registered as **type-2** via
   `cd_dispatch_tbl8(2,…)`) gets **0 hits** → `cd_mc_sm_wrap`
   (`0x80198FE4`, the `0x801BC84C` hook — confirmed installed live)
   never runs → `cd_mc_state_machine` (`0x80199440`) never runs →
   no read issue / no `cd_dir_entry_finalize` → entries never reach
   `state==2`.

So: **type-0 CD events fire; type-2 command-response events do not.**

## Live corroboration (same instance)

- `cdrom_state`: `sector_available=1`, `sector_read_pos=2060/2340`,
  `reading=1`, `request=0x80` (BFRD set), `mode=0xE0`, `read_cmd=0x1B`.
- `dma_state`: **ch1 (MDECout) ACTIVE** (`remaining_words=1482`) — MDEC
  is draining a decode; **ch3 (CD) idle**, ch0 (MDECin) idle.
- Command queue EMPTY: `0x801b8448 == 0x801b844c == 0x24`.
- Dir-entry table (`0x801fc6c0`→`0x80010000`) essentially idle: gwatch
  on page `0x80010000` caught only 2 writes in 3 s, both to entry[0];
  the polled entry idx30 (`0x801fc6ac`→`0x800103c0`) holds `0x77777777`
  garbage — never produced.
- `cd_channel_poll` polls entry idx `0x801fc6ac`=30 for `state==2`;
  `cd_dir_entry_finalize` (`0x801990F0`) is the only `state=2` writer and
  it never runs.
- `cd_dir_entry_probe` magic-div **verified faithful** (clean port ==
  generated `(hi + a0>>2)>>8 - sign`); consume cursor idx is correct.

## Dispatch-table decode

- `cd_dispatch_tbl8(type,handler)` → `tbl=*0x801bdda0=0x801BDD80`,
  `fn=*(tbl+8)=0x8019AABC`, calls it — i.e. `0x8019AABC` is the kernel
  "register CD event handler" routine. Handlers stored for types 0 and 2.
- `cd_cb8_dispatch` iterates an 8-entry sub-table (`0x801C0000-8768` =
  `0x801BDDC0` region) via `jalr`; type-0 events reach it.

## Open question (next session)

Why do **type-2** (command-response) events never fire? Candidates:

1. The runtime CD model raises INT1/data-ready (drives type-0 +
   sub-channel cbs) but does **not** raise the command-response
   completion that the kernel maps to type-2 → `cd_response_pump`.
   Check `cdrom.c` INT1/INT2/INT3 generation vs the game's
   `irq_enable=0x07` mask and `cd_response_pump`'s expected trigger.
2. `cd_response_pump` reads the response FIFO (`0x1F801801`); if the
   kernel only schedules type-2 on a response-count edge that never
   occurs (`response_read=1,response_count=1` — exactly one stale byte),
   it starves.
3. The `0x801BC84C` hook fires only inside `cd_response_pump`; since the
   pump never runs, `cd_mc_sm_wrap` never runs regardless of install.

## Fixes landed this session (committed `c11c718`)

- `cd_fileop_forward` now forwards a0/a1 to `mdec_pump_b` (was `void()`
  dropping args — MDECin read addr 0/size 0x20).
- `gen_clean_table.py` strips `#`-directive lines before prototype scan
  (latent bug corrupting `rtps_basis_xform` extern).
- Regenerated `src/clean_table.c`; full build clean; fresh instance
  reaches identical wedge (no regression).

## Registration mechanism (decoded from generated 0x8019AABC)

`cd_dispatch_tbl8(type,handler)` → `*(tbl+8)=0x8019AABC(type,handler)`:

- stores `handler` into slot `0x801BCD0C + type*4` (table base
  `0x801C0000-13028 = 0x801BCD0C`)
- sets bit `1<<type` in an enable mask at table `+44/+48`
- calls BIOS trampolines `0x80199FE0` (B0/0x5b) and `0x8019A670`
  (C0/0x0a) to **arm the kernel interrupt source** for that type

So type-0 (`cd_cb8_dispatch`) and type-2 (`cd_response_pump`) are
separately-registered, separately-enabled IRQ handler slots. Type-0's
IRQ fires (382×); type-2's never does → `cd_response_pump` starves.

The type→IRQ-source mapping is armed via C0/0x0a (kernel INTC/CD
event enable). Next step: identify which `CDIRQ_*` (`DATA_READY`/
`DATA_END`/`ACK`/`ERROR` in `cdrom.c set_irq`) the runtime must raise
to trip the type-2 slot, and confirm it isn't being raised for the
stalled read session.

## Handler table + INTC decode (live `:6100`)

`cd_intc_dispatch` (0x8019a8ec, delegated) is the real IRQ dispatcher:
mask = `*ptrA(0x1F801070=INTC_STAT) & *ptrB(0x1F801074=INTC_MASK) &
u16@0x801bcd48`, then calls `handler_table[i]` @`0x801bcd1c+i*4` for
each set bit i<11.

Live handler table `0x801bcd1c`:
```
[0]=0x8019AE68 cd_cb8_dispatch   <- FIRES (382x)
[1]=0
[2]=0x801979F8 cd_response_pump  <- 0 hits  (INTC bit2 = CDROM)
[3]=0x8019AF7C                   <- 0 hits
[4..10]=0
```
local mask `0x801bcd48=0xD` (bits 0,2,3 enabled); `irq_state`:
`i_mask=0xD`, `i_stat=0`, `pending=0`.

So `cd_response_pump` is the **CDROM (INTC bit 2)** handler and it is
enabled, but INTC bit2 is never observed pending at dispatch time even
though CD data-ready IRQs fire `cd_cb8_dispatch` (index 0) ~382x.

Two candidate breaks (next session):
- The CD data-ready IRQ is being routed to handler index 0 rather than
  index 2 — i.e. the game's INTC/CD-IRQ source mapping differs from a
  naive bit==index assumption, OR `cd_cb8_dispatch` is invoked by a
  different path (not the bit-table) and INTC bit2 genuinely never
  asserts.
- The runtime raises the CD IRQ on a line the game's mask/handler
  indexing doesn't map to slot 2 (INTC bit assignment / CD IRQ routing
  in `cdrom.c` `psx_irq_raise` vs the game's `0x801bdda4/b` ptr cells).

## IRQ line verification (live `:6100`)

- `0x1F801070` = `i_stat`, `0x1F801074` = `i_mask` (memory.c:854-855).
  `cd_intc_dispatch` mask = `i_stat & i_mask & local(0x801bcd48=0xD)`.
- `irq_state`: `i_mask=0xD` (bits 0,2,3 unmasked) — CDROM bit2 enabled.
- 40-sample `i_stat` poll: only `0x0`/`0x1` seen — bit0=VBLANK; bit2 is
  a brief per-IRQ pulse (AND-ack clears it), so sampling misses it.
- `cdrom_timing_dump`: recent sectors `intc_seen:1` — CD IRQs DO reach
  INTC and fire `psx_irq_raise(2)` (i_stat bit2) per sector.

## Conclusion

`i_stat` bit2 (CDROM) **is** asserted per CD IRQ, `cd_intc_dispatch`
runs 609×, `cd_cb8_dispatch` (handler[0]) fires 411× — but
`cd_response_pump` (handler[2]) gets **0 hits**. The dispatch mask never
contains bit2 at read time ⇒ **i_stat bit2 is cleared before
`cd_intc_dispatch` samples it** — an IRQ ack-ordering/timing defect in
the interrupt path (consistent with the documented "i_stat bit2"
wedge class in `cdrom.c` comments, lines 92-142).

Prime suspects for next session:
1. BIOS/kernel INTC prologue acks `i_stat` bit2 before the game's
   `cd_intc_dispatch` reads `i_stat&i_mask&local` — ordering bug in the
   recompiled-BIOS exception path or the game's ack placement.
2. `i_mask` bit2 transiently cleared during CD IRQs (game masks it
   around command issue) so the mask AND drops bit2.
3. `cd_response_pump` registered on the wrong slot — but table[2]=
   `0x801979F8` and bit2==CDROM match, so this is least likely.
