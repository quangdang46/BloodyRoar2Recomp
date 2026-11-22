# CD read-session completion → game-visible event (`proof/cd_read_done_event.md`)

Scope: the final CD-ROM file-load session issued during the arcade →
versus transition (SLES-01722, headless LLE build). Question answered:
*which cell tracks sector/read progress, what the completion path does,
whether it delivers an event / sets a flag / invokes `bc848`, and whether
the observed versus-spawn wedge is a lost CD completion.*

**Verdict up front:** on the current build the completion path fires
faithfully — `cd_read_step_cb`'s final-sector branch issues
`Pause(9)`, sets the session-done flag `0x801BCBA4`, restores the
`0x801BC84C` callback cell, installs `0x801A8590` into `0x801BC848`,
and tail-calls the user callback `*(0x801BC7B0) = 0x80195B9C`, which is a
thin trampoline to BIOS `B0/0x07 DeliverEvent(0xF0000003, 0x40)` — the
file-read completion event. Runtime on debug port 9010 confirms the
event was delivered (PSX_CDEV log shows `F0000003/40`), the CD session
block shows `sectors-remaining = 0`, and the emulated machine kept
running ~48k frames into the versus spawn/select phase
(`0x1F800001=3`, `0x1F8000B4=0x0300`). **The residual wedge is the
battle spawn/state machine (slot-2 / spawn-task phase), not a lost CD
completion.** Earlier "missing event" readings on port 8997 were from a
stale/pre-fix instance and are superseded.

---

## 1. Session state block (`ctx = 0x801BCB80`)

`cd_read_step_cb` (`0x801985C4`) and `cd_read_finish_cb` (`0x80198838`)
both operate on the same 0x40-byte session block at `0x801BCB80`
(`s0 = 0x801C0000 - 0x3480 = 0x801BCB80` in both bodies):

| Offset | Guest addr | Role |
|--------|-----------|------|
| +0x00 | `0x801BCB80` | sector count / mode word (was `0x20`) |
| +0x04 | `0x801BCB84` | buffer base A (`0x801C47C0`) |
| +0x08 | `0x801BCB88` | **read cursor** — advanced `+ stride*4` per sector |
| +0x0C | `0x801BCB8C` | buffer base B (`0x801D47C0`) |
| +0x10 | `0x801BCB90` | **stride** (dwords per sector step; was `0xA0`) |
| +0x14 | `0x801BCB94` | **sectors remaining** — decremented once per sector; `0` = done |
| +0x18 | `0x801BCB98` | timer stamp of last issue |
| +0x1C | `0x801BCB9C` | timer stamp |
| +0x20 | `0x801BCBA0` | sectors completed counter (incremented per sector) |
| +0x24 | `0x801BCBA4` | **read-session completion flag** — set `1` on final sector, cleared by `cd_init_done_cb` (`0x80198590`) at next session start |
| +0x28 | `0x801BCBA8` | saved `0x801BC848` callback (restored via `mc_session`) |
| +0x2C | `0x801BCBAC` | saved `0x801BC84C` callback (restored via `mc_session_b`) |
| +0x30 | `0x801BCBB0` | arg for `0x801961A8` when `*(0x801BC7B4)&1` |
| +0x34 | `0x801BCBB4` | user-context word passed as `a1` to the final callback |

Sector progress is tracked **per sector in `0x801BCB94`** (decrement)
with the destination cursor in `0x801BCB88` (advance by
`0x801BCB90 << 2` bytes) and a completion counter in `0x801BCBA0`.
At runtime on 9010 after the wedge: `bcb94 = 0`, `bcba0 = 0x2262`,
`bcba4 = 0` (cleared again by the *next* session's `cd_init_done_cb` —
the versus path already started another CD transaction), consistent
with the final read having completed.

## 2. Per-sector stepping — `cd_read_step_cb` (`0x801985C4`)

Clean wrapper in `game/cd.c` delegates to generated
(`br2_call_generated(0x801985C4,...)`) because the body crosses the
deferred-IRQ coroutine boundary (`psx_check_interrupts_at` per block).

Generated body (full_46.c, 628 bytes, 39 blocks) — entry stores the
callback context `a1` into `ctx->0x34`, then per invocation:

- reads sector header via `0x801961A8` / `0x80196188` / `0x801961CC` /
  `0x80196168` (param-block + status helpers),
- `0x801962F0` (`cd_stamp_to_sector`) converts the MSF stamp,
- `0x80197AD0` issues the next sector read,
- advances `ctx->0x08 += ctx->0x10 * 4`, `ctx->0x14--`, `ctx->0x20++`
  (same arithmetic as `cd_read_finish_cb`),
- loops while `ctx->0x14 != 0`.

### Completion tail (reached when `ctx->0x14 == 0`) — exact decode

| Addr | Action |
|------|--------|
| `0x801987AC` | `a0 = ctx->0x2C` (`0x801BCBAC`); `jal 0x80195D78` = `mc_session_b` → restores saved `0x801BC84C` |
| `0x801987BC` | `v0 = *(0x801BC7B4) & 1` (sign-extended `lw` from `0x801C0000-0x384C`); if set → `0x801987D0` |
| `0x801987D0` | `a0 = ctx->0x30`; `jal 0x801961A8` |
| `0x801987E4` | `a0 = 0x801A8590`; `jal 0x80195D64` = `mc_session` → installs `0x801A8590` as `0x801BC848` |
| `0x801987E8` | `a0 = 9`, `a1 = 0`; `jal 0x80195EC8` = `cd_cmd_issue_async` → **issues CD command `0x09` = Pause** |
| `0x801987F8` | `v1 = *(0x801BC7B0)` (sign-extended `lw` from `0x801C0000-0x3850`) |
| `0x80198804` | `ctx->0x24` (`0x801BCBA4`) `= 1` — **session-done flag set** (delay slot, always executes) |
| `0x80198800` | `beq v1, zero → 0x80198824` epilogue — callback skipped only if cell is NULL |
| `0x80198808` | `v0 = ctx->0x14`; `a0 = (v0 != 0) ? 5 : 2` → `2` on the final sector |
| `0x8019881C` | `jalr v1` → `*(0x801BC7B0)` with `a0=2`, `a1 = ctx->0x34` |

So the completion is **data-ready-sector driven** (the last sector's
`cd_read_step_cb` invocation), and `Pause(0x09)` is cleanup issued by
the same completion tail — it is *not* the source of the game-visible
event.

### The user callback `0x80195B9C` → `DeliverEvent`

`*(0x801BC7B0) = 0x80195B9C` at runtime. Its body (full_45.c, 28 bytes):

```
a0 = 0xF0000003 ; a1 = 0x40 ; a2 = (passthrough a0 of caller = 2)
jal 0x80195BD0
```

`0x80195BD0` is a BIOS trampoline: `a0 = 0xB0` (table), `a1 = 7`
(`DeliverEvent`), tail-dispatch. Net effect:
**`DeliverEvent(class=0xF0000003, spec=0x40)`** — the CD file-read
completion event class observed throughout `PSX_CDEV` logs.

### Alternate finish path `cd_read_finish_cb` (`0x80198838`)

Armed only when `*(0x801BC7B4) & 1` (set by `cd_cmd_dispatch3` in
`cd_read_open` when `DAT_801bc7b4 & 1`). Runtime showed
`0x801BC7B4 = 0` for the final session → this callback was **not**
armed; its zero fntrace count is expected, not a dropped call. Its
completion block (0x80198890+) is semantically identical: `mc_session_b
(ctx->0x2C)` → conditional `0x801961A8(ctx->0x30)` →
`mc_session(0x801A8590)` → `0x80195EC8(9,0)` → `0x801BCBA4 = 1` →
`jalr *(0x801BC7B0)` with `a0 = 2`. Clean wrapper
(`game/cd.c:816`) delegates to generated.

## 3. What actually delivers "read done" to the game

Two distinct signals, do not conflate:

1. **File-read completion** — `DeliverEvent(0xF0000003, 0x40)` via the
   `0x801BC7B0` tail-call above, fired synchronously inside the last
   `cd_read_step_cb` invocation (CD data-ready IRQ context).
2. **Channel/SPU doorbell completion** — `0xF0000009/0x20`, delivered by
   `cd_dma_chan_reset` (`0x8019B7E0`) when the ch4 (SPU DMA) transfer
   completes. This services the "channel" (SPU coprocessor) work queue,
   not the raw sector reads. The versus path issues several of these
   (observed kicks/dones at frames 318, 348, 1226, 1476, 1523/1527 on
   9010).

`0x801BDE40` is *not* the file-read done flag — it is polled by
`cd_seek_complete_poll` for seek/channel ops and legitimately reads `0`
once idle.

## 4. Runtime evidence (port 9010, fresh instance, `PSX_CDEV=1`,
`PSX_DMAPROBE=1`)

- `cdrom_state`: `last_sector.lba = 8801` (MSF 1:58:70 — the final
  `Setloc`/`ReadN` issued at frame ~1510), `reading = 0`,
  `pending.cmd = 0x09` **inactive** (the Pause was issued and
  completed), `sector_read_pos = 2060 / sector_size = 2340`.
- `sector_read_pos` semantics: it is `RB_.pos`, the consumer offset
  into the *current* sector ring buffer (`cdrom.c:836,3299`). 280
  unconsumed bytes remain because the file's payload ended mid-sector;
  `sector_available = 1` confirms the bytes are still buffered, not
  lost. Not evidence of a short transfer.
- `0x801BCB94 = 0` (sectors remaining exhausted),
  `0x801BCBA0 = 0x2262` (completed-sector counter advanced).
- `0x801BC7B0 = 0x80195B9C`, `0x801BC7B4 = 0`,
  `0x801BC848 = 0x801BC84C = 0` (session callback cells drained after
  use — normal).
- `bios_svc_trace` / `PSX_CDEV`: repeated `DeliverEvent
  (0xF0000003, 0x40)` file-read completions, including the final pair
  at guest cycles ≈1.101 G / 1.110 G (≈ frames 1625/1638), immediately
  after the last ch3 sector DMAs (through frame 1639).
- **Post-completion liveness:** `sp_ring` shows guest frames advancing
  47875 → 48133+ (kernel threads `0x8019A53C`/`0x8019A87C` cycling
  normally), and phase cells read `gs=8, ph=03, b4=0x0300` — the
  machine is in the versus spawn/select battle loop, *past* all CD
  waits. No `TestEvent` spin, no stalled `cd_seek_complete_poll`.

## 5. Status of prior hypotheses (superseded / confirmed)

| Hypothesis | Status |
|---|---|
| Sector-ring overflow (`ring_dropped=1`, `bfrd_set/clear` skew) | Symptom of fast consumption, not the wedge cause; reads completed byte-exact. |
| Deferred-coroutine starvation dropping `cd_read_finish_cb` | `0x801BC7B4=0` → finish cb never armed for this session; N/A. |
| Missing `0x801BDE40` flag / `TestEvent` spin | `0x801BDE40` is the *seek/channel* poll flag, not file-read done; on 9010 there is no TestEvent spin at all. |
| Missing ch4 doorbell for final op | All versus channel ops that were submitted completed (kicks through f1527 + done IRQs → `F0000009/20`). The game stopped submitting channel ops because it moved on to battle spawn logic. |
| `cd_response_pump` delegation | Retained as faithful boundary; fresh build with delegation shows identical (working) CD path — not the bug. |
| **Current residual wedge** | Versus spawn/select state machine (`battle_frame`, spawn records at `0x801FFE80+`, slot-2 CPU auto-confirm path). Separate subsystem — see battle-flow investigation; **not** a CD completion failure. |

## 6. Minimal faithful-fix location

**No CD-side fix is indicated.** The completion mechanism is faithful:
generated `cd_read_step_cb`/`cd_read_finish_cb` (delegated via
`br2_call_generated` in `game/cd.c:~800-817`) reproduce the exact
generated MIPS semantics including the `DeliverEvent` tail-call. If a
future wedge *is* proven to be a lost `F0000003/40`, the places to look
are, in order:

1. IRQ2 dispatch delivery of the CD data-ready interrupt that invokes
   the `0x801BC84C` sector callback (`cd_cb8_dispatch` /
   `chan_job_pump` path) — proof family `proof/cd_event_drop_point.md`.
2. `0x80195EC8` (`cd_cmd_issue_async`) Pause issue timing vs. the
   session-callback restore — but Pause completing is already observed.
3. The `0x801BC7B0` callback cell being cleared before the final
   sector — currently it holds `0x80195B9C` correctly.

The actual current blocker is downstream: versus spawn phase
(`DAT_1f800001=3`, `0x1F8000B4=0x0300`) waiting on battle-frame spawn
records — investigate `battle_flow_state_machine` (`0x8016FCC4`) /
`battle_frame` (`0x801506D0`-family) slot-2 auto-confirm, not `game/cd.c`.
