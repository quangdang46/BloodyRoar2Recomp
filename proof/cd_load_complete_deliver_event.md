# CD load-complete → DeliverEvent(0xF0000009,0x20) — drop-point analysis

Status: **drop point localized; root cause still open.** This document records
hard runtime + disassembly evidence gathered with a new env-gated DMA probe
(`PSX_DMAPROBE=1`, `dmaprobe()` in `psxrecomp/runtime/src/dma.c`) on a fresh
repro (port 8941) that reached the identical wedge signature as ports
8915/8930/8920/8934 (last sector LBA 8801, `sector_read_pos=2060/2340`,
`pending.cmd=0x09` Pause idle, `reading=0`).

## Proven facts (this session)

1. **The file-load data path is DMA channel 3, never channel 4.**
   `dmaprobe` logged every ch3/ch4 kick+completion for the whole run:
   ~2100 ch3 kick/done pairs (pcs `0x801978AC`, `0x80199694`, `0x80199CEC`,
   `0xBFC1E88C` BIOS, `0x80192FA4`), **zero ch4 events of any kind**.
   Therefore `cd_dma_chan_reset` (`0x8019B7E0`, the ch4 done-handler that
   calls `DeliverEvent(0xF0000009,0x20)` when `*0x801BDEE8==0`) is **not**
   the producer for this load. The earlier "ch4 completion chain works in
   versus" observation is a different (audio/SPU) path.

2. **ch3 completions intentionally raise no DMA IRQ.** Live DICR stayed
   `0x00900000` = bit23 (master) + bit20 (ch4 per-channel enable) only.
   ch3's enable bit (bit19) is clear, so `channel_irq_enabled(3)==false`
   and `complete_transfer(3)` never latches DICR bit27 — faithful to HW.
   The ch3 handler-table entry (`0x801BDE00`) is also 0. ch3 completion is
   consumed by the **CD-ROM IRQ2 path**, not by `cd_cb7_run`'s table.

3. **`cd_cb7_run` does run.** Its generated body (`br2eu_func_8019AF7C`,
   spans `0x8019AF7C–0x8019B0FC`) performs the DICR ack write at guest pc
   `0x8019B008` — observed 1101× in the probe, last at frame 1551.

4. **All DMA went silent at frame ~1551 and never resumed** (frame counter
   kept advancing past 25000; game logic wedged). Last ch3 done at f=1547;
   final DICR writes f=1549–1551 from `0x80192FA4`/`0x8019B008`/`0x80193210`.
   After that: no kicks, no completions, no DICR writes — CD subsystem idle
   while the guest spins.

5. **IRQ2 completion dispatch is `cd_cb8_dispatch` (`0x8019AE68`, 108 B,
   delegated to generated).** Decoded: it bumps a counter at `0x801BDDE0`,
   then loops `i=0..7` calling `tbl[i]` where `tbl = 0x801BDDC0`
   (clean twin: `cd_cb8_run_param`, game/cd.c ~line 5346). The table is
   zeroed by `cd_init_stage_a` (`0x8019AE10`) and slots are registered
   indirectly through unnamed generated fn `0x8019AED4`
   (`tbl[a0]=a1` if different).

6. **At the wedge all 8 callback slots `0x801BDDC0..0x801BDDDC` are 0**
   (verified `mem_words` on wedged 8915: entire region zero except
   `0x801BDDDC=0x801A0110`, `0x801BDDE0=0x0011F2EF`, `0x801BDDE4=0x1F801114`,
   `0x801BDDF0=0x1F8010F4`). So `cd_cb8_dispatch` fires on CD IRQ2 but
   calls **nothing** — the per-command completion callbacks are absent.

7. **`DeliverEvent(0xF0000009,0x20)` has exactly one guest producer in the
   clean tree**: `cd_dma_chan_reset` via `cd_bios_svc_07` (ch4 path), plus
   generated `br2eu_func_8019B7E0` doing `jr 0xB0` svc7. Other svc7
   trampolines deliver different classes (`cd_bios_cmd20` →
   `0xF0000003/0x20`, `cd_bios_cmd40*` → `0xF0000003/0x40`).

8. **XA audio is not being delivered**: `cdrom_sector_history` shows
   `xa_audio_delivered:0` on every sampled Mode-2 XA sector (raw_mode 0x02,
   xa_submode 0x08/0x89). Consistent with zero ch4 (SPU) DMA all run.

## Drop-point hypothesis (needs one more probe)

The missing event is upstream of `DeliverEvent`: the game's CD command
layer registers per-command completion callbacks into `tbl[0..7]`
(`0x801BDDC0`) via `0x8019AED4`. At the wedge the table is empty, so IRQ2
completion dispatches to nothing. Two candidate break points:

- (a) The final `Pause(0x09)`/read-tail completion path never re-registers
  (or prematurely clears) the callback slot that would run the
  "transfer complete → arm/DeliverEvent 0xF0000009" step; or
- (b) The deferred coroutine (`cd_ctx_restore → cd_chan_full_init →
  cd_intc_dispatch → cd_cb8_dispatch`) does not resume for the final
  IRQ2, so `tbl[]` is never even consulted (consistent with prior
  `cd_ctx_restore` zero-invocation observations).

The dmaprobe data cannot distinguish (a)/(b) because it only covers DMA.
Next instrumentation: env-gated `fprintf` at `cd_cb8_dispatch` entry
(generated — needs a runtime hook or pc_probe on `0x8019AE68`),
`0x8019AED4` registration calls, and `cd_ctx_restore` (`0x8019ADCC`)
entries, correlated by `s_frame_count`.

## Repro recipe

```
PSX_DMAPROBE=1 ./build/BloodyRoar2_Recompiled \
  --disc ../bloody-roar-2/Game.cue --debug-port 8941 --headless \
  > /tmp/br2_8941.log 2>&1 &
python3 tools/arcade_fight_mash.py 8941 > /tmp/mash_8941.log 2>&1 &
```

Wedge reproduces deterministically (same LBA-8801 signature); probe tail
in `/tmp/br2_8941.log` ends at `[dmaprobe] f=1551`.
